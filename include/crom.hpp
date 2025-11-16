#include <global.hpp>
#include <mappers/mapper.hpp>

#ifndef __CROM_H__
#define __CROM_H__

class CardROM
{
    public:
        CardROM();
        ~CardROM();
        u8 read_from_cpu(u16 cpu_addr);
        u8 read_from_ppu(u16 ppu_addr);
        void load_rom(const char *filename);
        bool mirror_mode; // Mirror mode: [0 - Horizontal / 1 - Vertical]

    private:
        void decode(u8 header[]);

        u8 *PRGROM, *CHRROM;
        u8 prg_units = 0x01, chr_units = 0x01; 
        u8 mapper_num = 0x00;
        Mapper* mapper;
};

#endif