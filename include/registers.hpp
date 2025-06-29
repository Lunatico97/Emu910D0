#include <utils.hpp>

#ifndef _REGISTER_BANK_H_
#define _REGISTER_BANK_H_

enum REG
{
    A = 0, X, Y, ST, SP, PCH, PCL, NON
};

class Registers
{
    public:
        Registers();
        
        // Initialization
        void init();

        // Transfers
        void loadREG(REG r, u8 value);
        void loadPC(u16 value);
        u8 fetchREG(REG r);
        u16 fetchPC();

    private:
        // Register Bank
        u8 BANK[7];
};

#endif