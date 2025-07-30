#include <memory.hpp>

Memory::Memory(CardROM *cptr, PPU* pptr): crom(cptr), ppu(pptr) {}
Memory::~Memory() {}

void Memory::init()
{
    for (int i = 0; i < 2048; ++i) block[i] = 0x00;
}

void Memory::store(u16 m_address, u8 value)
{
    if(m_address >= 0x0000 && m_address < 0x2000) block[(m_address & 0x07FF)] = value;
    else if(m_address >= 0x2000 && m_address < 0x4000) ppu->write_from_cpu((m_address & 0x2007), value);
    else if(m_address >= 0x4000 && m_address < 0x4020) return;
    else if(m_address >= 0x4020 && m_address < 0x8000) return;
    else return;
    //else crom->write_from_cpu(); 
    // Right now, we use only mapper 000 that doesn't perform writes on card
}

u8 Memory::retreive(u16 m_address)
{
    u8 value;
    if(m_address >= 0x0000 && m_address < 0x2000) return block[(m_address & 0x07FF)];
    else if(m_address >= 0x2000 && m_address < 0x4000) return ppu->read_from_cpu(m_address & 0x2007);
    else if(m_address >= 0x4000 && m_address < 0x4020) return 0x00;
    else if(m_address >= 0x4020 && m_address < 0x8000) return 0x00;
    else return crom->read_from_cpu(m_address);
}
