#include <global.hpp>

#ifndef __MAPPER_H__
#define __MAPPER_H__

#define PRG_BANK 16384
#define CHR_BANK 8192

class Mapper
{
    public:
        u8 mirror_mode = 0xFF;
        bool override_ram = false;
        virtual u32 map_cpu(u16 cpu_addr) = 0;
        virtual u32 map_ppu(u16 ppu_addr) = 0;
        virtual void map_cpu_wr(u16 cpu_addr, u8 data) = 0;
        virtual void map_ppu_wr(u16 cpu_addr, u8 data) = 0;
        
        // IRQs
        bool fire_irq = false;
        virtual void clock_cpu_irq() = 0;
        virtual void clock_ppu_irq(u16 ppu_addr) = 0;
};

#endif
