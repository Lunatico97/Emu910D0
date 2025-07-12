#include <alu.hpp>

ALU::ALU(MMU& mmui): mmu(mmui) {}

void ALU::update_flags()
{
    if(TEMP & 0x80 == 0x80) SF |= HX_SIGN;
    if(TEMP == 0x00) SF |= HX_ZERO;
}

void ALU::adc(u8 value)
{
    TEMP += value;
}

void ALU::sbc(u8 value)
{
    TEMP -= value;
}

void ALU::ana(u8 value)
{
    TEMP &= value;
}

void ALU::eor(u8 value)
{
    TEMP ^= value;
}

void ALU::ora(u8 value)
{
    TEMP |= value;
}

void ALU::cmp(u8 value)
{
    if(TEMP >= value) { SF |= HX_CARY; /* A >= M => C = 1 */ }
    if(TEMP == value) { SF |= HX_ZERO; /* A == M => Z = 1 */ }
}

void ALU::asl(u8 value)
{
    TEMP = value << 1;
    SF |= (value & D7) >> 7;
}

void ALU::lsr(u8 value)
{
    TEMP = value >> 1;
    SF |= (value & D0);
}

void ALU::rol(u8 value)
{
    TEMP = value << 1;
    TEMP |= (SF & HX_CARY);
    SF |= (value & D7) >> 7;
}

void ALU::ror(u8 value)
{
    TEMP = value >> 1;
    TEMP |= (SF & HX_CARY) << 7;
    SF |= (value & 0x01);
}

void ALU::inc(u8 value)
{
    TEMP += 0x01;
}

void ALU::dec(u8 value)
{
    TEMP -= 0x01;
}

void ALU::set_flag(u8 mask)
{
    SF = mmu.tapREG(ST);
    SF |= mask;
    mmu.ld(ST, SF);
}

void ALU::clear_flag(u8 mask)
{
    SF = mmu.tapREG(ST);
    SF &= ~mask;
    mmu.ld(ST, SF);
}

void ALU::fetch(ADR mode, REG r, u16 value)
{

}

void ALU::load(REG r)
{
    update_flags();
    mmu.ld(r, TEMP);
    mmu.ld(ST, SF);
}
