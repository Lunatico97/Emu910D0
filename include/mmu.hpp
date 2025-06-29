#include <registers.hpp>
#include <memory.hpp>

#ifndef _MMU_H_
#define _MMU_H_

#define SP_INDEX 0x0100

class MMU
{
    public:
        MMU();
        void load_mem(u16 address, u8 value);
        u8 fetch_mem(u16 address);

        // Program Counter
        void init_pc(u16 value);

        // Stack Pointer
        void push(REG r);
        void pop(REG r);

        // Implied Transfers
        void tr(REG des, REG src);

        // Immediate Transfers
        void ld(REG des, u8 value);

        // Addressed Transfers
        void ld(REG des, REG off, u16 addr);
        void st(REG des, REG off, u16 addr);

        // Zero-Addressed Transfers
        void ldo(REG des, REG off, u16 addr);
        void sto(REG des, REG off, u16 addr);

        // Indexed Indirect Transfers
        void ldi(REG des, REG off, u16 addr);
        void sti(REG des, REG off, u16 addr);

        // Indirect Indexed Transfers
        void ldix(REG des, REG off, u16 addr);
        void stix(REG des, REG off, u16 addr);

        // Taps
        u8 tapREG(REG r);
        u16 tapPC();

    private:
        Registers rb;
        Memory mem;
};

#endif