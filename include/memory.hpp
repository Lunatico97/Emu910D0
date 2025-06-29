#include <utils.hpp>

#ifndef _MEMORY_H_
#define _MEMORY_H_

class Memory
{
    public:
        Memory();
        void init();

        // Transfers
        void store(u16 m_address, u8 value);
        u8 retreive(u16 m_address);

    private:
        u8 block[65536];
};

#endif