#include <utils.hpp>
#include <ppu.hpp>
#include <crom.hpp>

#ifndef _MEMORY_H_
#define _MEMORY_H_

class Memory
{
    public:
        Memory(CardROM* cptr, PPU* pptr);
        ~Memory();
        
        void init();

        // Transfers
        void store(u16 m_address, u8 value);
        u8 retreive(u16 m_address);

    private:
        CardROM* crom;
        PPU* ppu; 

        u8 block[2048];
};

#endif