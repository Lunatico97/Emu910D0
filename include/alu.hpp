#include <mmu.hpp>

#ifndef _ALU_H_
#define _ALU_H_

#define HX_SIGN 0x80
#define HX_OVFW 0x40
#define HX_BREK 0x10
#define HX_DECM 0x08
#define HX_INTD 0x04
#define HX_ZERO 0x02
#define HX_CARY 0x01

class ALU
{
    public:
        ALU();
        ALU(MMU& mmu);

        // Flags
        void update_flags();

        // Arithmetic Operations
        void adc(u8 value);
        void sbc(u8 value);

        // Logical Operations
        void ana(u8 value); // can't use 'and' as it is a keyword ;)
        void eor(u8 value);
        void ora(u8 value);
        void cmp(u8 value);

        // Shift Operations
        void asl(u8 value);
        void lsr(u8 value);

        // Rotate Operations
        void rol(u8 value);
        void ror(u8 value);

        // Incrementors / Decrementors
        void dec(u8 value);
        void inc(u8 value);

        // Flags
        void set_flag(u8 mask);
        void clear_flag(u8 mask);

    private:
        u8 TEMP, SF; // [S V x B D I Z C]
        MMU& mmu;

        // Registers
        void fetch(ADR mode, REG r, u16 value);
        void load(REG r);
};

#endif