#include <mappers/mapper.hpp>

class Mapper007: public Mapper
{
    public:
        Mapper007(u8 prg, u8 chr)
        {
            prg_units = prg;
            chr_units = chr;
        }

        u32 map_cpu(u16 cpu_addr)
        {
            assert(cpu_addr >= 0x6000 && cpu_addr <= 0xFFFF);
            if(cpu_addr >= 0x6000 && cpu_addr <= 0x7FFF) return (cpu_addr & 0x1FFF);
            else
            {
                bank_addr = bank_select*PRG_BANK*2;
                return bank_addr + (cpu_addr & 0x7FFF);
            }
        }

        u32 map_ppu(u16 ppu_addr)
        {
            assert(ppu_addr >= 0x0000 && ppu_addr < 0x2000);
            return ppu_addr;
        }

        void map_cpu_wr(u16 cpu_addr, u8 data)
        {
            assert(cpu_addr >= 0x8000 && cpu_addr <= 0xFFFF);
            bank_select = (data & 0x07);
            mirror_mode = (data & 0x10) >> 4;
        }

        void map_ppu_wr(u16 ppu_addr, u8 data)
        {
            assert(ppu_addr >= 0x0000 && ppu_addr < 0x2000);
        }
        
        void clock_irq(u16 ppu_addr) { return; }
    
    private:
        u8 prg_units, chr_units;
        u8 bank_select = 0x00;
        u32 bank_addr;
};
