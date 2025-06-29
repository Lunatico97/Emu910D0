#include <mmu.hpp>

#ifndef _ALU_H_
#define _ALU_H_

#define HX_SIGN 0x80
#define HX_ZERO 0x40
#define HX_AUXC 0x10
#define HX_PARY 0x04
#define HX_CARY 0x01

class ALU
{
    public:
        ALU();
        ALU(MMU& mmu);

        // Flags
        void update_flags();

        // Arithmetic Operations
        void add(REG r);
        void adc(REG r);
        void adi(u8 value);
        void aci(u8 value);
        void sub(REG r);
        void sbb(REG r);
        void sui(u8 value);
        void sbi(u8 value);
        void daa();
        void cma();
        void cmc();
        void stc();

        // Logical Operations
        void ana(REG r);
        void ani(u8 value);
        void ora(REG r);
        void ori(u8 value);
        void xra(REG r);
        void xri(u8 value);
        void cmp(REG r);
        void cpi(u8 value);

        // Rotate Operations
        void rlc();
        void ral();
        void rrc();
        void rar();

    private:
        u8 ACC, TEMP, SF; // [S Z x AC x P x CY]
        MMU& mmu;

        // Registers
        void fetch(u8 value);
        void fetch(REG r);
        void load();
};

#endif