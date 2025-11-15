#include <alu.hpp>
#include <mmu.hpp>
#include <crom.hpp>

#ifndef _CPU_H_
#define _CPU_H_

#define RST_VECTOR 0xFFFC
#define NMI_VECTOR 0xFFFA
#define IRQ_VECTOR 0xFFFE

class CPU
{
    public:
        CPU(MMU* mmu_ptr);
        ~CPU();

        // CPU runners
        void clock();
        void step(bool debug = 0);

        // Hardware Interrupts
        void rst();
        void nmi();
        void irq();

        // Decoder
        void decode(const HEX& hex);

    private:
        // Branching
        void brc_set(u8 hx_flag, u8 rel_addr);
        void brc_rst(u8 hx_flag, u8 rel_addr);

        // Routines & Interrupts
        void jmpi(u16 address);
        void jmp(u16 address);
        void jsr(u16 address);
        void brk();

        // Returns
        void rts();
        void rti();

        u8 cycles;
        MMU* mmu;
        ALU alu;
        u16 IREG;
};

#endif