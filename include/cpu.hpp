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

        void load_catridge(CardROM* crom, const char* filename);
        void step(CardROM* crom);

        // Hardware Interrupts
        void rst();
        void nmi();
        void irq();

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

        MMU* mmu;
        ALU alu;
        u16 IREG;
};

#endif