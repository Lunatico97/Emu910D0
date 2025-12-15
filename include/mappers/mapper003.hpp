#include <mappers/mapper.hpp>

class Mapper003: public Mapper
{
    public:
        Mapper003(u8 prg, u8 chr, u8 *prg_rom)
        {
            prg_units = prg;
            chr_units = chr;
            prg_ptr = prg_rom;
        }

        u32 map_cpu(u16 cpu_addr)
        {
            assert(cpu_addr >= 0x6000 && cpu_addr <= 0xFFFF);
            if(cpu_addr >= 0x6000 && cpu_addr <= 0x7FFF) return (cpu_addr & 0x07FF);
            else return (cpu_addr & (prg_units*PRG_BANK - 1));
        }

        u32 map_ppu(u16 ppu_addr)
        {
            assert(ppu_addr >= 0x0000 && ppu_addr < 0x2000);
            return (bank_select*CHR_BANK) + (ppu_addr & 0x1FFF);
        }

        void map_cpu_wr(u16 cpu_addr, u8 data)
        {
            assert(cpu_addr >= 0x8000 && cpu_addr <= 0xFFFF);
            bank_select = (data & *(prg_ptr + map_cpu(cpu_addr)) & (chr_units - 1));
        }

        void map_ppu_wr(u16 ppu_addr, u8 data)
        {
            assert(ppu_addr >= 0x0000 && ppu_addr < 0x2000);
        }
    
    private:
        u8 prg_units, chr_units;
        u8 bank_select = 0x00;
        u8* prg_ptr;
};