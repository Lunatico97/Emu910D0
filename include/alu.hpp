#include <mmu.hpp>

#ifndef _ALU_H_
#define _ALU_H_

#define HX_SIGN 0x80
#define HX_OVFW 0x40
#define HX_NUSE 0x20
#define HX_BREK 0x10
#define HX_DECM 0x08
#define HX_INTD 0x04
#define HX_ZERO 0x02
#define HX_CARY 0x01

class ALU
{
    public:
        ALU(MMU* mmu_ptr);

        // Flags
        void update_flags();

        // Arithmetic Operations
        void adc(ADR mode, u16 addr, u8 off);
        void sbc(ADR mode, u16 addr, u8 off);

        // Logical Operations
        void ana(ADR mode, u16 addr, u8 off); // can't use 'and' as it is a keyword ;)
        void eor(ADR mode, u16 addr, u8 off);
        void ora(ADR mode, u16 addr, u8 off);
        void cmp(REG r, ADR mode, u16 addr, u8 off);

        // Shift Operations
        void asl(ADR mode, u16 addr, u8 off);
        void lsr(ADR mode, u16 addr, u8 off);

        // Rotate Operations
        void rol(ADR mode, u16 addr, u8 off);
        void ror(ADR mode, u16 addr, u8 off);

        // Incrementors / Decrementors
        void dec(REG r);
        void inc(REG r);
        void dec(ADR mode, u16 addr, u8 off);
        void inc(ADR mode, u16 addr, u8 off);

        // Flags
        void bit(ADR mode, u16 addr, u8 off);
        void set_flag(u8 mask);
        void clr_flag(u8 mask);

    private:
        u8 TEMP1, TEMP2, SF; // [S V x B D I Z C]
        MMU* mmu;

        // Registers
        void fetchIMD(u8 off);
        void fetchMEM(ADR mode, u16 addr, u8 off, bool cp_check = false);
        void loadMEM(ADR mode, u16 addr, u8 off);
        void fetchREG(REG r);
        void loadREG(REG r);
};

#endif