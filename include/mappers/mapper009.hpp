#include <mappers/mapper.hpp>

class Mapper009: public Mapper
{
    public:
        Mapper009(u8 prg, u8 chr)
        {
            prg_units = prg*2;
            chr_units = chr;
        }

        u32 map_cpu(u16 cpu_addr)
        {
            assert(cpu_addr >= 0x6000 && cpu_addr <= 0xFFFF);
            if(cpu_addr >= 0x6000 && cpu_addr <= 0x7FFF)
            {
                return (cpu_addr & 0x1FFF);
            }
            else
            {
                // PRG              
                prg_offset = cpu_addr & 0x1FFF;
                if(cpu_addr >= 0x8000 && cpu_addr <= 0x9FFF) prg_addr = asic_vals.prg_select*8192; 
                else if(cpu_addr >= 0xA000 && cpu_addr <= 0xBFFF) prg_addr = (prg_units-3)*8192;
                else if(cpu_addr >= 0xC000 && cpu_addr <= 0xDFFF) prg_addr = (prg_units-2)*8192;
                else prg_addr = (prg_units-1)*8192;

                return prg_addr + prg_offset;
            }
        }

        u32 map_ppu(u16 ppu_addr)
        {
            assert(ppu_addr >= 0x0000 && ppu_addr < 0x2000);
            // CHR         
            chr_offset = ppu_addr & 0x0FFF;
            if(ppu_addr >= 0x0000 && ppu_addr < 0x1000) 
            {
                chr_addr = asic_vals.chr_select1[asic_vals.latch1]*4096;
                if(ppu_addr == 0x0FD8) asic_vals.latch1 = false;
                else if(ppu_addr == 0x0FE8) asic_vals.latch1 = true;
            }
            else 
            {
                chr_addr = asic_vals.chr_select2[asic_vals.latch2]*4096;
                if(ppu_addr >= 0x1FD8 && ppu_addr <= 0x1FDF) asic_vals.latch2 = false;
                else if(ppu_addr >= 0x1FE8 && ppu_addr <= 0x1FEF) asic_vals.latch2 = true;
            }
            return chr_addr + chr_offset;
        }

        void map_cpu_wr(u16 cpu_addr, u8 data)
        {
            assert(cpu_addr >= 0x8000 && cpu_addr <= 0xFFFF);
            configure((cpu_addr & 0xF000) >> 12, data);
        }

        void map_ppu_wr(u16 ppu_addr, u8 data)
        {
            assert(ppu_addr >= 0x0000 && ppu_addr < 0x2000);
        }
        
        void clock_irq(u16 ppu_addr) { return; }
    
    private:
        void configure(u8 asic_index, u8 data)
        {
            switch(asic_index)
            {
                case 0x0A: asic_vals.prg_select = data & 0x0F; break;
                case 0x0B: asic_vals.chr_select1[0] = data & 0x1F; break;
                case 0x0C: asic_vals.chr_select1[1] = data & 0x1F; break;
                case 0x0D: asic_vals.chr_select2[0] = data & 0x1F; break;
                case 0x0E: asic_vals.chr_select2[1] = data & 0x1F; break;
                case 0x0F: mirror_mode = (data & D0) + 0x02; break;
                default: break;
            }
        }

        struct
        {
            bool chr_mode, prg_disabled, latch1, latch2;
            u8 prg_select = 0x00, chr_select1[2] = {0, 0}, chr_select2[2] = {1, 1};
        } asic_vals;

        u8 prg_units, chr_units;
        u16 prg_offset, chr_offset;
        u32 prg_addr, chr_addr;
};
