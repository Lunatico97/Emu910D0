#include <alu.hpp>
#include <mmu.hpp>

#ifndef _CPU_H_
#define _CPU_H_

class CPU
{
    public:
        CPU();
        void create_machine_code(const char* filename);
        void load_program();

    private:
        void decode(const HEX& hex);

        // Branching (Jumps)
        void jmp(u16 address);
        void bcc(u16 address);
        void bcs(u16 address);
        void beq(u16 address);
        void bmi(u16 address);
        void bne(u16 address);
        void bpl(u16 address);
        void bvc(u16 address);
        void bvs(u16 address);

        // Branching (Calls)
        void jsr(u16 address);

        // Branching (Returns)
        void rts();

        MMU mmu;
        ALU alu;
        u16 IREG;
};

#endif