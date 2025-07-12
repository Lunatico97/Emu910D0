#include <cpu.hpp>

CPU::CPU(): mmu(), alu(mmu) {}

void CPU::create_machine_code(const char* filename)
{
    u8 hexes[] = {0xA9, 0xAB, 0xAA, 0x48, 0x9A, 0xFF};
    for(u16 i=0x0000; i<6; i++)
    {
        mmu.load_mem(i, hexes[i]);
    }

    IREG = 0x0000;
}

void CPU::load_program()
{   
    HEX current;
    // Tap origin
    Utils::logU16("ORG", IREG);
    std::cout << "\n";

    while(1)
    {
        u8 l = IL_MAP[mmu.fetch_mem(IREG)];
        mmu.init_pc(mmu.tapPC()+l);

        // Tap registers stepwise
        Utils::logU16("IR", IREG);
        Utils::logU16("PC", mmu.tapPC());
        Utils::logU8("SP", mmu.tapREG(SP));
        Utils::logU8("ST", mmu.tapREG(ST));
        Utils::logU8("A", mmu.tapREG(A));
        Utils::logU8("X", mmu.tapREG(X));
        Utils::logU8("Y", mmu.tapREG(Y));
        Utils::logHEX(current);

        for(int i=0; i<l; i++)
        {
            current.h8[i] = mmu.fetch_mem(IREG+i); 
        }

        decode(current);
        IREG = mmu.tapPC();
    }    
}

void CPU::decode(const HEX& hex)
{
    u16 h16 = (static_cast<u16>(hex.h8[1]) | (static_cast<u16>(hex.h8[2]) << 8));
    switch(hex.h8[0])
    {
        case 0xFF: exit(0); break;
        case 0xA9: mmu.ld(A, hex.h8[1]); break;
        case 0xA5: mmu.ldo(A, NON, h16); break;
        case 0xB5: mmu.ldo(A, X, h16); break;
        case 0xAD: mmu.ld(A, NON, h16); break;
        case 0xBD: mmu.ld(A, X, h16); break;
        case 0xB9: mmu.ld(A, Y, h16); break;
        case 0xA1: mmu.ldi(A, X, h16); break;
        case 0xB1: mmu.ldix(A, Y, h16); break;

        case 0xA2: mmu.ld(X, hex.h8[1]); break;
        case 0xA6: mmu.ldo(X, NON, h16); break;
        case 0xB6: mmu.ldo(X, Y, h16); break;
        case 0xAE: mmu.ld(X, NON, h16); break;
        case 0xBE: mmu.ld(X, Y, h16); break;

        case 0xA0: mmu.ld(Y, hex.h8[1]); break;
        case 0xA4: mmu.ldo(Y, NON, h16); break;
        case 0xB4: mmu.ldo(Y, X, h16); break;
        case 0xAC: mmu.ld(Y, NON, h16); break;
        case 0xBC: mmu.ld(Y, X, h16); break;

        case 0x85: mmu.sto(A, NON, h16); break;
        case 0x95: mmu.sto(A, X, h16); break;
        case 0x8D: mmu.st(A, NON, h16); break;
        case 0x9D: mmu.st(A, X, h16); break;
        case 0x99: mmu.st(A, Y, h16); break;
        case 0x81: mmu.sti(A, X, h16); break;
        case 0x91: mmu.stix(A, Y, h16); break;

        case 0x86: mmu.sto(X, NON, h16); break;
        case 0x96: mmu.sto(X, Y, h16); break;
        case 0x8E: mmu.st(X, NON, h16); break;

        case 0x84: mmu.sto(Y, NON, h16); break;
        case 0x94: mmu.sto(Y, X, h16); break;
        case 0x8C: mmu.st(Y, NON, h16); break;

        case 0xAA: mmu.tr(X, A); break;
        case 0xA8: mmu.tr(Y, A); break;
        case 0xBA: mmu.tr(X, SP); break;
        case 0x8A: mmu.tr(A, X); break;
        case 0x9A: mmu.tr(SP, X); break;
        case 0x98: mmu.tr(A, Y); break;

        case 0x48: mmu.push(A); break;
        case 0x68: mmu.pop(A); break;
        case 0x08: mmu.push(ST); break;
        case 0x28: mmu.pop(ST); break;
        
        case 0x90: bcc(h16); break;
        case 0xB0: bcs(h16); break;
        case 0xF0: beq(h16); break;
        case 0x30: bmi(h16); break;
        case 0xD0: bne(h16); break;
        case 0x10: bpl(h16); break;
        case 0x50: bvc(h16); break;
        case 0x70: bvs(h16); break;
        case 0x4C: jmp(h16); break;
        
        case 0x20: jsr(h16); break;
        case 0x60: rts(); break;

        default: break;
    }
}

void CPU::jmp(u16 address)
{
    mmu.init_pc(address);
}

void CPU::bcc(u16 address)
{
    if(mmu.tapREG(ST) & HX_CARY != HX_CARY) mmu.init_pc(address);
    else return;
}

void CPU::bcs(u16 address)
{
    if(mmu.tapREG(ST) & HX_CARY == HX_CARY) mmu.init_pc(address);
    else return;
}

void CPU::beq(u16 address)
{
    if(mmu.tapREG(ST) & HX_ZERO == HX_ZERO) mmu.init_pc(address);
    else return;
}

void CPU::bmi(u16 address)
{
    if(mmu.tapREG(ST) & HX_SIGN == HX_SIGN) mmu.init_pc(address);
    else return;
}

void CPU::bne(u16 address)
{
    if(mmu.tapREG(ST) & HX_ZERO != HX_ZERO) mmu.init_pc(address);
    else return;
}

void CPU::bpl(u16 address)
{
    if(mmu.tapREG(ST) & HX_SIGN != HX_SIGN) mmu.init_pc(address);
    else return;
}

void CPU::bvc(u16 address)
{
    if(mmu.tapREG(ST) & HX_OVFW != HX_OVFW) mmu.init_pc(address);
    else return;
}

void CPU::bvs(u16 address)
{
    if(mmu.tapREG(ST) & HX_OVFW == HX_OVFW) mmu.init_pc(address);
    else return;
}

void CPU::jsr(u16 address)
{
    mmu.push(PCL);
    mmu.push(PCH);
    mmu.init_pc(address);
}

void CPU::rts()
{
    mmu.pop(PCH);
    mmu.pop(PCL);
}