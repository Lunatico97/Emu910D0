#include <memory.hpp>

Memory::Memory() {}

void Memory::store(u16 m_address, u8 value)
{
    block[m_address] = value;
}

u8 Memory::retreive(u16 m_address)
{
    return block[m_address];
}
