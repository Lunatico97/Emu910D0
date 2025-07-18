#include <alu.hpp>
#include <mmu.hpp>

#ifndef _CPU_H_
#define _CPU_H_

#define RST_VECTOR 0xFC
#define NMI_VECTOR 0xFA
#define IRQ_VECTOR 0xFE

class CPU
{
    public:
        CPU();

        void create_machine_code(const char* filename);
        MMU& get_mmu();
        void step();

    private:
        void decode(const HEX& hex);

        // Branching
        void bcc(u8 rel_addr);
        void bcs(u8 rel_addr);
        void beq(u8 rel_addr);
        void bmi(u8 rel_addr);
        void bne(u8 rel_addr);
        void bpl(u8 rel_addr);
        void bvc(u8 rel_addr);
        void bvs(u8 rel_addr);

        // Routines & Interrupts
        void jmp(u16 address);
        void jsr(u16 address);
        void brk();

        // Returns
        void rts();
        void rti();

        // System Interrupts
        void rst();
        void nmi();
        void irq();

        MMU mmu;
        ALU alu;
        u16 IREG;
};

#endif