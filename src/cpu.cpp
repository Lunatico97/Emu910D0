#include <cpu.hpp>

CPU::CPU(): mmu(), alu(mmu) {}

MMU& CPU::get_mmu()
{
    return mmu;
}

void CPU::load_machine_code()
{
    // u8 hexes[] = {
    //     0xA9, 0x00, 0x69, 0x01, 0xC9, 0x0A, 0xD0, 0xFA, 0x00
    // };

    u8 hexes[] = {
        0xA9, 0x00, 0xA2, 0x01, 0x69, 0x01, 0xE8, 0x9D, 0xF0, 0x00, 0xE0, 0x0A, 0xD0, 0xF6, 0x00
    };

    for(u16 i=0x0000; i<sizeof(hexes)/sizeof(u8); i++)
    {
        mmu.load_mem(i, hexes[i]);
    }

    mmu.load_mem(IRQ_VECTOR, 0xFE);
    mmu.load_mem(IRQ_VECTOR+0x00001, 0x00);
    mmu.load_mem(0x00FE, 0x40);

    IREG = 0x0000;
}

void CPU::step()
{   
        HEX current;
        u8 l = IL_MAP[mmu.fetch_mem(IREG)];
        mmu.init_pc(mmu.tapPC()+l);

        // Tap registers stepwise
        // std::cout << Utils::logU16("IR", IREG);
        // std::cout << Utils::logU16("PC", mmu.tapPC());
        // std::cout << Utils::logU8("SP", mmu.tapREG(SP));
        // std::cout << Utils::logU8("ST", mmu.tapREG(ST));
        // std::cout << Utils::logU8("A", mmu.tapREG(A));
        // std::cout << Utils::logU8("X", mmu.tapREG(X));
        // std::cout << Utils::logU8("Y", mmu.tapREG(Y));
        // std::cout << Utils::logHEX(current);

        for(int i=0; i<l; i++)
        {
            current.h8[i] = mmu.fetch_mem(IREG+i); 
        }

        decode(current);
        IREG = mmu.tapPC();    
}

void CPU::decode(const HEX& hex)
{
    u16 h16 = (static_cast<u16>(hex.h8[1]) | (static_cast<u16>(hex.h8[2]) << 8));
    switch(hex.h8[0])
    {
        case 0xEA: break;
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

        case 0x69: alu.adc((ADR)-1, 0x0000, hex.h8[1]); break;
        case 0x65: alu.adc(ADR::ZER, 0x0000, hex.h8[1]); break;
        case 0x75: alu.adc(ADR::ZEX, 0x0000, hex.h8[1]); break;
        case 0x6D: alu.adc(ADR::ABS, h16, 0x00); break;
        case 0x7D: alu.adc(ADR::ABX, h16, 0x00); break;
        case 0x79: alu.adc(ADR::ABY, h16, 0x00); break;
        case 0x61: alu.adc(ADR::IXI, h16, mmu.tapREG(X)); break;
        case 0x71: alu.adc(ADR::IIX, h16, mmu.tapREG(Y)); break;

        case 0xE9: alu.sbc((ADR)-1, 0x0000, hex.h8[1]); break;
        case 0xE5: alu.sbc(ADR::ZER, 0x0000, hex.h8[1]); break;
        case 0xF5: alu.sbc(ADR::ZEX, 0x0000, hex.h8[1]); break;
        case 0xED: alu.sbc(ADR::ABS, h16, 0x00); break;
        case 0xFD: alu.sbc(ADR::ABX, h16, 0x00); break;
        case 0xF9: alu.sbc(ADR::ABY, h16, 0x00); break;
        case 0xE1: alu.sbc(ADR::IXI, h16, mmu.tapREG(X)); break;
        case 0xF1: alu.sbc(ADR::IIX, h16, mmu.tapREG(Y)); break;

        case 0x29: alu.ana((ADR)-1, 0x0000, hex.h8[1]); break;
        case 0x25: alu.ana(ADR::ZER, 0x0000, hex.h8[1]); break;
        case 0x35: alu.ana(ADR::ZEX, 0x0000, hex.h8[1]); break;
        case 0x2D: alu.ana(ADR::ABS, h16, 0x00); break;
        case 0x3D: alu.ana(ADR::ABX, h16, 0x00); break;
        case 0x39: alu.ana(ADR::ABY, h16, 0x00); break;
        case 0x21: alu.ana(ADR::IXI, h16, mmu.tapREG(X)); break;
        case 0x31: alu.ana(ADR::IIX, h16, mmu.tapREG(Y)); break;

        case 0x49: alu.eor((ADR)-1, 0x0000, hex.h8[1]); break;
        case 0x45: alu.eor(ADR::ZER, 0x0000, hex.h8[1]); break;
        case 0x55: alu.eor(ADR::ZEX, 0x0000, hex.h8[1]); break;
        case 0x4D: alu.eor(ADR::ABS, h16, 0x00); break;
        case 0x5D: alu.eor(ADR::ABX, h16, 0x00); break;
        case 0x59: alu.eor(ADR::ABY, h16, 0x00); break;
        case 0x41: alu.eor(ADR::IXI, h16, mmu.tapREG(X)); break;
        case 0x51: alu.eor(ADR::IIX, h16, mmu.tapREG(Y)); break;

        case 0x09: alu.ora((ADR)-1, 0x0000, hex.h8[1]); break;
        case 0x05: alu.ora(ADR::ZER, 0x0000, hex.h8[1]); break;
        case 0x15: alu.ora(ADR::ZEX, 0x0000, hex.h8[1]); break;
        case 0x0D: alu.ora(ADR::ABS, h16, 0x00); break;
        case 0x1D: alu.ora(ADR::ABX, h16, 0x00); break;
        case 0x19: alu.ora(ADR::ABY, h16, 0x00); break;
        case 0x01: alu.ora(ADR::IXI, h16, mmu.tapREG(X)); break;
        case 0x11: alu.ora(ADR::IIX, h16, mmu.tapREG(Y)); break;

        case 0xC9: alu.cmp(A, (ADR)-1, 0x0000, hex.h8[1]); break;
        case 0xC5: alu.cmp(A, ADR::ZER, 0x0000, hex.h8[1]); break;
        case 0xD5: alu.cmp(A, ADR::ZEX, 0x0000, hex.h8[1]); break;
        case 0xCD: alu.cmp(A, ADR::ABS, h16, 0x00); break;
        case 0xDD: alu.cmp(A, ADR::ABX, h16, 0x00); break;
        case 0xD9: alu.cmp(A, ADR::ABY, h16, 0x00); break;
        case 0xC1: alu.cmp(A, ADR::IXI, h16, mmu.tapREG(X)); break;
        case 0xD1: alu.cmp(A, ADR::IIX, h16, mmu.tapREG(Y)); break;

        case 0xE0: alu.cmp(X, (ADR)-1, 0x0000, hex.h8[1]); break;
        case 0xE4: alu.cmp(X, ADR::ZER, 0x0000, hex.h8[1]); break;
        case 0xEC: alu.cmp(X, ADR::ABS, h16, 0x00); break;

        case 0xC0: alu.cmp(Y, (ADR)-1, 0x0000, hex.h8[1]); break;
        case 0xC4: alu.cmp(Y, ADR::ZER, 0x0000, hex.h8[1]); break;
        case 0xCC: alu.cmp(Y, ADR::ABS, h16, 0x00); break;

        case 0x0A: alu.asl((ADR)-1, 0x0000, hex.h8[1]); break;
        case 0x06: alu.asl(ADR::ZER, 0x0000, hex.h8[1]); break;
        case 0x16: alu.asl(ADR::ZEX, 0x0000, hex.h8[1]); break;
        case 0x0E: alu.asl(ADR::ABS, h16, 0x00); break;
        case 0x1E: alu.asl(ADR::ABX, h16, 0x00); break;

        case 0x4A: alu.lsr((ADR)-1, 0x0000, hex.h8[1]); break;
        case 0x46: alu.lsr(ADR::ZER, 0x0000, hex.h8[1]); break;
        case 0x56: alu.lsr(ADR::ZEX, 0x0000, hex.h8[1]); break;
        case 0x4E: alu.lsr(ADR::ABS, h16, 0x00); break;
        case 0x5E: alu.lsr(ADR::ABX, h16, 0x00); break;

        case 0x2A: alu.rol((ADR)-1, 0x0000, hex.h8[1]); break;
        case 0x26: alu.rol(ADR::ZER, 0x0000, hex.h8[1]); break;
        case 0x36: alu.rol(ADR::ZEX, 0x0000, hex.h8[1]); break;
        case 0x2E: alu.rol(ADR::ABS, h16, 0x00); break;
        case 0x3E: alu.rol(ADR::ABX, h16, 0x00); break;

        case 0x6A: alu.ror((ADR)-1, 0x0000, hex.h8[1]); break;
        case 0x66: alu.ror(ADR::ZER, 0x0000, hex.h8[1]); break;
        case 0x76: alu.ror(ADR::ZEX, 0x0000, hex.h8[1]); break;
        case 0x6E: alu.ror(ADR::ABS, h16, 0x00); break;
        case 0x7E: alu.ror(ADR::ABX, h16, 0x00); break;

        case 0xC6: alu.dec(ADR::ZER, 0x0000, hex.h8[1]); break;
        case 0xD6: alu.dec(ADR::ZEX, 0x0000, hex.h8[1]); break;
        case 0xCE: alu.dec(ADR::ABS, h16, 0x00); break;
        case 0xDE: alu.dec(ADR::ABX, h16, 0x00); break;

        case 0xE6: alu.inc(ADR::ZER, 0x0000, hex.h8[1]); break;
        case 0xF6: alu.inc(ADR::ZEX, 0x0000, hex.h8[1]); break;
        case 0xEE: alu.inc(ADR::ABS, h16, 0x00); break;
        case 0xFE: alu.inc(ADR::ABX, h16, 0x00); break;

        case 0xCA: alu.dec(X); break;
        case 0x88: alu.dec(Y); break;

        case 0xE8: alu.inc(X); break;
        case 0xC8: alu.inc(Y); break;

        case 0x38: alu.set_flag(HX_CARY); break;
        case 0xF8: alu.set_flag(HX_DECM); break;
        case 0x78: alu.set_flag(HX_INTD); break;

        case 0x18: alu.clr_flag(HX_CARY); break;
        case 0xD8: alu.clr_flag(HX_DECM); break;
        case 0x58: alu.clr_flag(HX_INTD); break;
        case 0xB8: alu.clr_flag(HX_OVFW); break;

        case 0x24: alu.set_flag(mmu.fetch_mem(static_cast<u16>(hex.h8[1]))); break;
        case 0x2C: alu.set_flag(mmu.fetch_mem(h16)); break;
        
        case 0x90: bcc(hex.h8[1]); break;
        case 0xB0: bcs(hex.h8[1]); break;
        case 0xF0: beq(hex.h8[1]); break;
        case 0x30: bmi(hex.h8[1]); break;
        case 0xD0: bne(hex.h8[1]); break;
        case 0x10: bpl(hex.h8[1]); break;
        case 0x50: bvc(hex.h8[1]); break;
        case 0x70: bvs(hex.h8[1]); break;

        case 0x4C: jmp(h16); break; 
               
        case 0x20: jsr(h16); break;
        case 0x60: rts(); break;

        case 0x00: brk(); break;
        case 0x40: rti(); break;

        default: break;
    }
}

void CPU::jmp(u16 address)
{
    mmu.init_pc(address);
}

void CPU::bcc(u8 rel_addr)
{
    if((mmu.tapREG(ST) & HX_CARY) != HX_CARY) mmu.init_pc(mmu.get_addr(ADR::REL, 0x0000, rel_addr));
    else return;
}

void CPU::bcs(u8 rel_addr)
{
    if((mmu.tapREG(ST) & HX_CARY) == HX_CARY) mmu.init_pc(mmu.get_addr(ADR::REL, 0x0000, rel_addr));
    else return;
}

void CPU::beq(u8 rel_addr)
{
    if((mmu.tapREG(ST) & HX_ZERO) == HX_ZERO) mmu.init_pc(mmu.get_addr(ADR::REL, 0x0000, rel_addr));
    else return;
}

void CPU::bmi(u8 rel_addr)
{
    if((mmu.tapREG(ST) & HX_SIGN) == HX_SIGN) mmu.init_pc(mmu.get_addr(ADR::REL, 0x0000, rel_addr));
    else return;
}

void CPU::bne(u8 rel_addr)
{
    if((mmu.tapREG(ST) & HX_ZERO) != HX_ZERO) mmu.init_pc(mmu.get_addr(ADR::REL, 0x0000, rel_addr));
    else return;
}

void CPU::bpl(u8 rel_addr)
{
    if((mmu.tapREG(ST) & HX_SIGN) != HX_SIGN) mmu.init_pc(mmu.get_addr(ADR::REL, 0x0000, rel_addr));
    else return;
}

void CPU::bvc(u8 rel_addr)
{
    if((mmu.tapREG(ST) & HX_OVFW) != HX_OVFW) mmu.init_pc(mmu.get_addr(ADR::REL, 0x0000, rel_addr));
    else return;
}

void CPU::bvs(u8 rel_addr)
{
    if((mmu.tapREG(ST) & HX_OVFW) == HX_OVFW) mmu.init_pc(mmu.get_addr(ADR::REL, 0x0000, rel_addr));
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

void CPU::brk()
{
    if((mmu.tapREG(ST) & HX_INTD) != HX_INTD)
    {
        mmu.ld(ST, mmu.tapREG(ST) | HX_BREK | HX_INTD | HX_NUSE);
        mmu.push(PCL);
        mmu.push(PCH);
        mmu.push(ST);
        mmu.ld(PCL, NON, IRQ_VECTOR);
        mmu.ld(PCH, NON, IRQ_VECTOR+0x0001);
    }
}

void CPU::rti()
{
    mmu.pop(ST);
    mmu.ld(ST, mmu.tapREG(ST) & ~HX_NUSE & ~HX_INTD);
    mmu.pop(PCH);
    mmu.pop(PCL);
}

void CPU::rst()
{
    mmu.ld(A, 0x00);
    mmu.ld(X, 0x00);
    mmu.ld(Y, 00);
    mmu.ld(ST, 0x00 | HX_NUSE);
    mmu.ld(PCL, NON, RST_VECTOR);
    mmu.ld(PCH, NON, RST_VECTOR+0x0001);
}

void CPU::irq()
{
    if((mmu.tapREG(ST) & HX_INTD) != HX_INTD)
    {
        mmu.ld(ST, mmu.tapREG(ST) & ~HX_BREK | HX_INTD | HX_NUSE);
        mmu.push(PCL);
        mmu.push(PCH);
        mmu.push(ST);
        mmu.ld(PCL, NON, IRQ_VECTOR);
        mmu.ld(PCH, NON, IRQ_VECTOR+0x0001);
    }
}

void CPU::nmi()
{
    mmu.ld(ST, mmu.tapREG(ST) & ~HX_BREK | HX_INTD | HX_NUSE);
    mmu.push(PCL);
    mmu.push(PCH);
    mmu.push(ST);
    mmu.ld(PCL, NON, NMI_VECTOR);
    mmu.ld(PCH, NON, NMI_VECTOR+0x0001);
}