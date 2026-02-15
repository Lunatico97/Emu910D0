#include <mappers/mapper.hpp>

class Mapper000: public Mapper
{
    public:
        Mapper000(u8 prg, u8 chr)
        {
            prg_units = prg;
            chr_units = chr;
        }

        u32 map_cpu(u16 cpu_addr)
        {
            assert(cpu_addr >= 0x6000 && cpu_addr <= 0xFFFF);
            if(cpu_addr >= 0x6000 && cpu_addr <= 0x7FFF) return (cpu_addr & 0x1FFF);
            else return (cpu_addr & (prg_units*PRG_BANK - 1));
        }

        u32 map_ppu(u16 ppu_addr)
        {
            assert(ppu_addr >= 0x0000 && ppu_addr < 0x2000);
            return ppu_addr;
        }

        void map_cpu_wr(u16 cpu_addr, u8 data) { return; }
        void map_ppu_wr(u16 ppu_addr, u8 data) { return; }
        void clock_irq(u16 ppu_addr) { return; }
    
    private:
        u8 prg_units, chr_units;
};
