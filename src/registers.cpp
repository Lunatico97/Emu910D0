#include <registers.hpp>

Registers::Registers() {}

void Registers::init()
{
    for(int i=0; i<7; i++)
    {
        BANK[i] = 0x0000;
    }
}

u8 Registers::fetchREG(REG r)
{
    return *(BANK+r);
}

u16 Registers::fetchPC()
{
    return (static_cast<u16>(*(BANK+PCH)) << 8) | static_cast<u16>(*(BANK+PCL));
}

void Registers::loadREG(REG r, u8 value)
{
    *(BANK+r) = value;
}

void Registers::loadPC(u16 value)
{
    *(BANK+PCL) = (value & 0x00FF);
    *(BANK+PCH) = (value & 0xFF00) >> 8;
}
