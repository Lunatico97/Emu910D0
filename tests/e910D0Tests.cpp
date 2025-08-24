/*
    Test Suite v1.0 [E910D0]
    Author: Diwas Adhikari
    Comments: Writing all these tests might seem a lot to do but, it was absolutely essential for me to get
              the CPU working right for the emulator to work as expected ! Enjoy :)
*/

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

void test_brc()
{
    // Initialize status
    mmu->load_reg(ST, 0x00 | HX_NUSE);
    // BCC $90H(-) 
    mmu->load_pc(0x0090);
    cpu->decode({0x90, 0x90});
    assert(mmu->fetch_pc() == 0x0020);
    // BCC $70H [+]
    cpu->decode({0x90, 0x70});
    assert(mmu->fetch_pc() == 0x0090);
    // BCC $90H [No branching]
    mmu->load_reg(ST, HX_CARY | HX_NUSE);
    cpu->decode({0x90, 0x90});
    assert(mmu->fetch_pc() == 0x0090);
    // BCS $90H [-] 
    cpu->decode({0xB0, 0xB0});
    assert(mmu->fetch_pc() == 0x0040);
    // BCS $40H [+]
    cpu->decode({0xB0, 0x70});
    assert(mmu->fetch_pc() == 0x00B0);
    // BCS $90H [No branching]
    mmu->load_reg(ST, 0x00 | HX_NUSE);
    cpu->decode({0xB0, 0xB0});
    assert(mmu->fetch_pc() == 0x00B0);
}

void test_jmp()
{
    // JMP $014C
    cpu->decode({0x4C, 0x4C, 0x01});
    assert(mmu->fetch_pc() == 0x014C);
    // JMP ($016C)
    mmu->store(0x016C, 0x6C);
    mmu->store(0x016D, 0x02);
    cpu->decode({0x6C, 0x6C, 0x01});
    assert(mmu->fetch_pc() == 0x026C);
    // JMP ($016C) [No page offset]
    mmu->store(0x0100, 0x01);
    mmu->store(0x01FF, 0x6C);
    mmu->store(0x0200, 0x02);
    cpu->decode({0x6C, 0xFF, 0x01});
    assert(mmu->fetch_pc() == 0x016C);
    // JSR $0120
    mmu->load_pc(0x0160);
    cpu->decode({0x20, 0x20, 0x01});
    assert(mmu->fetch_reg(SP) == 0xFD);
    assert(mmu->fetch_pc() == 0x0120);
    // RTS
    cpu->decode({0x0060});
    assert(mmu->fetch_reg(SP) == 0xFF);
    assert(mmu->fetch_pc() == 0x0160);
}

void test_flags()
{
    // Initialize status
    mmu->load_reg(ST, 0x00 | HX_NUSE);
    // SEC
    cpu->decode({0x38});
    assert(mmu->fetch_reg(ST) == (HX_NUSE | HX_CARY));
    // CLC
    cpu->decode({0x18});
    assert(mmu->fetch_reg(ST) == HX_NUSE);
    // BIT $24H
    mmu->load_reg(A, 0x00);
    mmu->store(0x0024, 0xC0);
    cpu->decode({0x24, 0x24});
    assert(mmu->fetch_reg(ST) == (HX_NUSE | HX_SIGN | HX_OVFW | HX_ZERO));
    // CLV
    cpu->decode({0xB8});
    assert(mmu->fetch_reg(ST) == (HX_NUSE | HX_SIGN | HX_ZERO));
    // BIT $012CH
    mmu->store(0x012C, 0xC0);
    cpu->decode({0x2C, 0x2C, 0x01});
    assert(mmu->fetch_reg(ST) == (HX_NUSE | HX_SIGN | HX_OVFW | HX_ZERO));
}

void test_asl()
{
    // Initialize status
    mmu->load_reg(ST, 0x00 | HX_NUSE);
    // ASL
    mmu->load_reg(A, 0x0A);
    cpu->decode({0x0A});
    assert(mmu->fetch_reg(A) == 0x14);
    assert(mmu->fetch_reg(ST) == HX_NUSE);
    // ASL $06H
    mmu->store(0x0006, 0x06);
    cpu->decode({0x06, 0x06});
    assert(mmu->retreive(0x0006) == 0x0C);
    assert(mmu->fetch_reg(ST) == HX_NUSE);
    // ASL $16H, X
    mmu->load_reg(X, 0x10);
    mmu->store(0x0026, 0x16);
    cpu->decode({0x16, 0x16});
    assert(mmu->retreive(0x0026) == 0x2C);
    assert(mmu->fetch_reg(ST) == HX_NUSE);
    // ASL $010EH
    mmu->store(0x010E, 0x8E);
    cpu->decode({0x0E, 0x0E, 0x01});
    assert(mmu->retreive(0x010E) == 0x1C);
    assert(mmu->fetch_reg(ST) == (HX_NUSE | HX_CARY));
    // ASL $011EH, X
    mmu->store(0x012E, 0x0E);
    cpu->decode({0x1E, 0x1E, 0x01});
    assert(mmu->retreive(0x012E) == 0x1C);
    assert(mmu->fetch_reg(ST) == HX_NUSE);
}

void test_lsr()
{
    // Initialize status
    mmu->load_reg(ST, 0x00 | HX_NUSE);
    // LSR
    mmu->load_reg(A, 0x4A);
    cpu->decode({0x4A});
    assert(mmu->fetch_reg(A) == 0x25);
    assert(mmu->fetch_reg(ST) == HX_NUSE);
    // LSR $46H
    mmu->store(0x0046, 0x46);
    cpu->decode({0x46, 0x46});
    assert(mmu->retreive(0x0046) == 0x23);
    assert(mmu->fetch_reg(ST) == HX_NUSE);
    // LSR $56H, X
    mmu->load_reg(X, 0x10);
    mmu->store(0x0066, 0x56);
    cpu->decode({0x56, 0x56});
    assert(mmu->retreive(0x0066) == 0x2B);
    assert(mmu->fetch_reg(ST) == HX_NUSE);
    // LSR $014EH
    mmu->store(0x014E, 0x4F);
    cpu->decode({0x4E, 0x4E, 0x01});
    assert(mmu->retreive(0x014E) == 0x27);
    assert(mmu->fetch_reg(ST) == (HX_NUSE | HX_CARY));
    // LSR $015EH, X
    mmu->store(0x016E, 0x5E);
    cpu->decode({0x5E, 0x5E, 0x01});
    assert(mmu->retreive(0x016E) == 0x2F);
    assert(mmu->fetch_reg(ST) == HX_NUSE);
}

void test_rol()
{
    // Initialize status
    mmu->load_reg(ST, 0x00 | HX_NUSE);
    // ROL
    mmu->load_reg(A, 0x2A);
    cpu->decode({0x2A});
    assert(mmu->fetch_reg(A) == 0x54);
    assert(mmu->fetch_reg(ST) == HX_NUSE);
    // ROL $26H
    mmu->store(0x0026, 0x26);
    cpu->decode({0x26, 0x26});
    assert(mmu->retreive(0x0026) == 0x4C);
    assert(mmu->fetch_reg(ST) == HX_NUSE);
    // ROL $36H, X
    mmu->load_reg(X, 0x10);
    mmu->store(0x0046, 0x36);
    cpu->decode({0x36, 0x36});
    assert(mmu->retreive(0x0046) == 0x6C);
    assert(mmu->fetch_reg(ST) == HX_NUSE);
    // ROL $012EH
    mmu->store(0x012E, 0x8E);
    cpu->decode({0x2E, 0x2E, 0x01});
    assert(mmu->retreive(0x012E) == 0x1C);
    assert(mmu->fetch_reg(ST) == (HX_NUSE | HX_CARY));
    // ROL $013EH, X
    mmu->store(0x014E, 0x0E);
    cpu->decode({0x3E, 0x3E, 0x01});
    assert(mmu->retreive(0x014E) == 0x1D);
    assert(mmu->fetch_reg(ST) == HX_NUSE);
}

void test_ror()
{
// Initialize status
    mmu->load_reg(ST, 0x00 | HX_NUSE);
    // ROR
    mmu->load_reg(A, 0x6A);
    cpu->decode({0x6A});
    assert(mmu->fetch_reg(A) == 0x35);
    assert(mmu->fetch_reg(ST) == HX_NUSE);
    // ROR $66H
    mmu->store(0x0066, 0x66);
    cpu->decode({0x66, 0x66});
    assert(mmu->retreive(0x0066) == 0x33);
    assert(mmu->fetch_reg(ST) == HX_NUSE);
    // ROR $76H, X
    mmu->load_reg(X, 0x10);
    mmu->store(0x0086, 0x76);
    cpu->decode({0x76, 0x76});
    assert(mmu->retreive(0x0086) == 0x3B);
    assert(mmu->fetch_reg(ST) == HX_NUSE);
    // ROR $016EH
    mmu->store(0x016E, 0x6F);
    cpu->decode({0x6E, 0x6E, 0x01});
    assert(mmu->retreive(0x016E) == 0x37);
    assert(mmu->fetch_reg(ST) == (HX_NUSE | HX_CARY));
    // ROR $017EH, X
    mmu->store(0x018E, 0x7E);
    cpu->decode({0x7E, 0x7E, 0x01});
    assert(mmu->retreive(0x018E) == 0xBF);
    assert(mmu->fetch_reg(ST) == (HX_NUSE | HX_SIGN));
}

void test_runner()
{
    test_lda();
    test_ldx();
    test_ldy();
    std::cout << "Load Tests Passed ! \n";
    test_sta();
    test_stx();
    test_sty();
    std::cout << "Store Tests Passed ! \n";
    test_tr();
    std::cout << "Transfer Tests Passed ! \n";
    test_stack();
    std::cout << "Stack Tests Passed ! \n";
    test_brc();
    test_jmp();
    std::cout << "Branch Tests Passed ! \n";
    test_flags();
    std::cout << "Flag Tests Passed ! \n";
    test_asl();
    test_lsr();
    std::cout << "Shift Tests Passed ! \n";
    test_rol();
    test_ror();
    std::cout << "Rotate Tests Passed ! \n";
}

int main(int argc, char* argv[]) 
{
    mmu = new MMU(nullptr, nullptr);
    cpu = new CPU(mmu);
    test_runner();
    delete cpu;
    return 0;
}