#include <utils.hpp>

#ifndef __MAPPER_H__
#define __MAPPER_H__

class Mapper
{
    public:
        virtual u16 map_cpu(u16 cpu_addr) = 0;
        virtual u16 map_ppu(u16 ppu_addr) = 0;
};

#endif