#include <utils.hpp>
#include <crom.hpp>
#include <ppu.hpp>

#ifndef _MMU_H_
#define _MMU_H_

#define SP_INDEX 0x0100

enum REG
{
    A = 0, X, Y, ST, SP, PCH, PCL, NON
};

enum ADR
{
    ABS = 0, ABX, ABY, IND, IIX, IXI, REL, ZER, ZEX, ZEY
};

class MMU
{
    public:
        MMU(CardROM* cptr, PPU* pptr);
        ~MMU();

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

        // Addressing
        u16 get_addr(ADR mode, u16 addr, u8 off);

        // Flags
        void updf(REG r);

        // Register operations
        void load_reg(REG r, const u8& value);
        void load_pc(const u16& addr);
        u8 fetch_reg(REG r);
        u16 fetch_pc();

        // Check cross-page status
        bool cross_page(u16 pre_addr, u16 post_addr);

        // Memory operations
        void store(u16 m_addr, u8 value);
        u8 retreive(u16 m_addr);

        // Penalty tracker
        u8 cycle_penalty = 0x00;

    private:
        // Reset MMU
        void reset();

        // References
        CardROM *crom;
        PPU *ppu;

        u8 BANK[7];
        u8 RAM[2048];
};

#endif