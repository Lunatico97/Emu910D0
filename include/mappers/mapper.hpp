#include <global.hpp>

#ifndef __MAPPER_H__
#define __MAPPER_H__

class Mapper
{
    public:
        virtual u32 map_cpu(u16 cpu_addr) = 0;
        virtual u32 map_ppu(u16 ppu_addr) = 0;
        virtual void map_cpu_wr(u16 cpu_addr, u8 data) = 0;
        virtual void map_ppu_wr(u16 cpu_addr, u8 data) = 0;
};

#endif