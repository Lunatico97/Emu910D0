#include <global.hpp>
#include <mappers/mapper.hpp>

#ifndef __CROM_H__
#define __CROM_H__

#define PRG_BANK 16384
#define CHR_BANK 8192

class CardROM
{
    public:
        CardROM();
        ~CardROM();
        u8 read_from_cpu(u16 cpu_addr);
        u8 read_from_ppu(u16 ppu_addr);
        void write_from_cpu(u16 cpu_addr, u8 data);
        void write_from_ppu(u16 ppu_addr, u8 data);
        void load_rom(const char *filename);
        bool mirror_mode; // Mirror mode: [0 - Horizontal / 1 - Vertical]

    private:
        void decode(u8 header[]);

        u8 *PRGROM, *CHRROM, *CHRRAM;
        u8 prg_units = 0x01, chr_units = 0x01; 
        u8 mapper_num = 0x00;
        Mapper* mapper;
};

#endif