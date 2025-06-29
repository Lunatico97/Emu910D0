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
        void jz(u16 address);
        void jnc(u16 address);
        void jnz(u16 address);

        // Branching (Calls)
        void call(u16 address);

        // Branching (Returns)
        void rts();

        MMU mmu;
        ALU alu;
        u16 IREG;
};

#endif