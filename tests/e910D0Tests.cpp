#include <cpu.hpp>

MMU* mmu = nullptr;
CPU* cpu = nullptr;

void test_lda()
{
    // LDA #$A9H
    cpu->decode({0xA9, 0xA9});
    assert(mmu->fetch_reg(A) == 0xA9);
    // LDA $A5H
    mmu->store(0x00A5, 0xA5);
    cpu->decode({0xA5, 0xA5});
    assert(mmu->fetch_reg(A) == 0xA5);
    // LDA $B5H, X
    mmu->load_reg(X, 0x10);
    mmu->store(0x00C5, 0xB5);
    cpu->decode({0xB5, 0xB5});
    assert(mmu->fetch_reg(A) == 0xB5);
    // LDA $01ADH
    mmu->store(0x01AD, 0xAD);
    cpu->decode({0xAD, 0xAD, 0x01});
    assert(mmu->fetch_reg(A) == 0xAD);
    // LDA $01BDH, X
    mmu->store(0x01CD, 0xBD);
    cpu->decode({0xBD, 0xBD, 0x01});
    assert(mmu->fetch_reg(A) == 0xBD);
    // LDA $01B9H, Y
    mmu->load_reg(Y, 0x10);
    mmu->store(0x01C9, 0xB9);
    cpu->decode({0xB9, 0xB9, 0x01});
    assert(mmu->fetch_reg(A) == 0xB9);
    // LDA ($A1H, X)
    mmu->store(0x01A1, 0xA1);
    mmu->store(0x00B1, 0xA1);
    mmu->store(0x00B2, 0x01);
    cpu->decode({0xA1, 0xA1});
    assert(mmu->fetch_reg(A) == 0xA1);
    // LDA ($B1H), Y
    mmu->store(0x01C1, 0xB1);
    mmu->store(0x00B1, 0xB1);
    mmu->store(0x00B2, 0x01);
    cpu->decode({0xB1, 0xB1});
    assert(mmu->fetch_reg(A) == 0xB1);
}

void test_ldx()
{
    // LDX #$A2H
    cpu->decode({0xA2, 0xA2});
    assert(mmu->fetch_reg(X) == 0xA2);
    // LDX $A6H
    mmu->store(0x00A6, 0xA6);
    cpu->decode({0xA6, 0xA6});
    assert(mmu->fetch_reg(X) == 0xA6);
    // LDX $B6H, Y
    mmu->load_reg(Y, 0x10);
    mmu->store(0x00C6, 0xB6);
    cpu->decode({0xB6, 0xB6});
    assert(mmu->fetch_reg(X) == 0xB6);
    // LDX $01AEH
    mmu->store(0x01AE, 0xAE);
    cpu->decode({0xAE, 0xAE, 0x01});
    assert(mmu->fetch_reg(X) == 0xAE);
    // LDX $01BEH, Y
    mmu->store(0x01CE, 0xBE);
    cpu->decode({0xBE, 0xBE, 0x01});
    assert(mmu->fetch_reg(X) == 0xBE);
}

void test_ldy()
{
    // LDY #$A0H
    cpu->decode({0xA0, 0xA0});
    assert(mmu->fetch_reg(Y) == 0xA0);
    // LDY $A4H
    mmu->store(0x00A4, 0xA4);
    cpu->decode({0xA4, 0xA4});
    assert(mmu->fetch_reg(Y) == 0xA4);
    // LDY $B4H, X
    mmu->load_reg(X, 0x10);
    mmu->store(0x00C4, 0xB4);
    cpu->decode({0xB4, 0xB4});
    assert(mmu->fetch_reg(Y) == 0xB4);
    // LDY $01ACH
    mmu->store(0x01AC, 0xAC);
    cpu->decode({0xAC, 0xAC, 0x01});
    assert(mmu->fetch_reg(Y) == 0xAC);
    // LDY $01BCH, X
    mmu->store(0x01CC, 0xBC);
    cpu->decode({0xBC, 0xBC, 0x01});
    assert(mmu->fetch_reg(Y) == 0xBC);
}

void test_sta()
{
    // STA $85H
    mmu->load_reg(A, 0x85);
    cpu->decode({0x85, 0x85});
    assert(mmu->retreive(0x0085) == 0x85);
    // STA $95H, X
    mmu->load_reg(A, 0x95);
    mmu->load_reg(X, 0x10);
    cpu->decode({0x95, 0x95});
    assert(mmu->retreive(0x00A5) == 0x95);
    // STA $018DH
    mmu->load_reg(A, 0x8D);
    cpu->decode({0x8D, 0x8D, 0x01});
    assert(mmu->retreive(0x018D) == 0x8D);
    // STA $019DH, X
    mmu->load_reg(A, 0x9D);
    mmu->load_reg(X, 0x10);
    cpu->decode({0x9D, 0x9D, 0x01});
    assert(mmu->retreive(0x01AD) == 0x9D);
    // STA $0199H, Y
    mmu->load_reg(A, 0x99);
    mmu->load_reg(Y, 0x10);
    cpu->decode({0x99, 0x99, 0x01});
    assert(mmu->retreive(0x01A9) == 0x99);
    // STA ($81H, X)
    mmu->load_reg(A, 0x81);
    mmu->store(0x0091, 0x81);
    mmu->store(0x0092, 0x01);
    cpu->decode({0x81, 0x81});
    assert(mmu->retreive(0x0181) == 0x81);
    // STA ($91H), Y
    mmu->load_reg(A, 0x91);
    mmu->store(0x0091, 0x91);
    mmu->store(0x0092, 0x01);
    cpu->decode({0x91, 0x91});
    assert(mmu->retreive(0x01A1) == 0x91);
}

void test_stx()
{
    // STX $86H
    mmu->load_reg(X, 0x86);
    cpu->decode({0x86, 0x86});
    assert(mmu->retreive(0x0086) == 0x86);
    // STX $96H, Y
    mmu->load_reg(X, 0x96);
    mmu->load_reg(Y, 0x10);
    cpu->decode({0x96, 0x96});
    assert(mmu->retreive(0x00A6) == 0x96);
    // STA $018EH
    mmu->load_reg(X, 0x8E);
    cpu->decode({0x8E, 0x8E, 0x01});
    assert(mmu->retreive(0x018E) == 0x8E);
}

void test_sty()
{
    // STY $84H
    mmu->load_reg(Y, 0x84);
    cpu->decode({0x84, 0x84});
    assert(mmu->retreive(0x0084) == 0x84);
    // STY $94H, X
    mmu->load_reg(Y, 0x94);
    mmu->load_reg(X, 0x10);
    cpu->decode({0x94, 0x94});
    assert(mmu->retreive(0x00A4) == 0x94);
    // STY $018CH
    mmu->load_reg(Y, 0x8C);
    cpu->decode({0x8C, 0x8C, 0x01});
    assert(mmu->retreive(0x018C) == 0x8C);
}

void test_tr()
{
    // TAX
    mmu->load_reg(A, 0xAA);
    cpu->decode({0xAA});
    assert(mmu->fetch_reg(X) == 0xAA);
    // TAY
    mmu->load_reg(A, 0xA8);
    cpu->decode({0xA8});
    assert(mmu->fetch_reg(Y) == 0xA8);
    // TSX
    mmu->load_reg(SP, 0xBA);
    cpu->decode({0xBA});
    assert(mmu->fetch_reg(X) == 0xBA);
    // TXA
    mmu->load_reg(X, 0x8A);
    cpu->decode({0x8A});
    assert(mmu->fetch_reg(A) == 0x8A);
    // TXS
    mmu->load_reg(X, 0x9A);
    cpu->decode({0x9A});
    assert(mmu->fetch_reg(SP) == 0x9A);
    // TYA
    mmu->load_reg(Y, 0x98);
    cpu->decode({0x98});
    assert(mmu->fetch_reg(A) == 0x98);
}

void test_stack()
{
    // Reset TOS
    mmu->load_reg(SP, 0xFF);
    // PHA
    mmu->load_reg(A, 0x48);
    cpu->decode({0x48});
    assert(mmu->fetch_reg(SP) == 0xFE);
    assert(mmu->retreive(0x01FF) == 0x48);
    // PLA
    cpu->decode({0x68});
    assert(mmu->fetch_reg(SP) == 0xFF);
    assert(mmu->fetch_reg(A) == 0x48);
    // PHP
    mmu->load_reg(ST, 0x08);
    cpu->decode({0x08});
    assert(mmu->fetch_reg(SP) == 0xFE);
    assert(mmu->retreive(0x01FF) == 0x08);  
    // PLP
    cpu->decode({0x28});
    assert(mmu->fetch_reg(SP) == 0xFF);
    assert(mmu->fetch_reg(ST) == 0x08);  
}

void test_runner()
{
    test_lda();
    test_ldx();
    test_ldy();
    test_sta();
    test_stx();
    test_sty();
    test_tr();
    test_stack();
}

int main(int argc, char* argv[]) 
{
    mmu = new MMU(nullptr, nullptr);
    cpu = new CPU(mmu);
    test_runner();
    delete cpu;
    return 0;
}