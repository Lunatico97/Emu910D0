#include <mappers/mapper.hpp>
/*
    Mapper 069 [FME-7 - JxROM]
    Author: Diwas Adhikari
*/
class Mapper069: public Mapper
{
    public:
        Mapper069(u8 prg, u8 chr)
        {
            prg_units = prg*2;
            chr_units = chr;
            override_ram = 1;
            memset(bank_reg, 0x00, 12);
        }

        u32 map_cpu(u16 cpu_addr)
        {
            assert(cpu_addr >= 0x6000 && cpu_addr <= 0xFFFF);
            if(cpu_addr >= 0x6000 && cpu_addr <= 0x7FFF)
            {
                if(override_ram)
                {
                    prg_addr =  (bank_reg[8]*8192);
                    return prg_addr + (cpu_addr & 0x1FFF);
                } 
                else return (cpu_addr & 0x1FFF);
            }
            else
            {
                // PRG              
                if(cpu_addr >= 0x8000 && cpu_addr <= 0x9FFF) asic_vals.prg_select = bank_reg[9]; 
                else if(cpu_addr >= 0xA000 && cpu_addr <= 0xBFFF) asic_vals.prg_select = bank_reg[10];
                else if(cpu_addr >= 0xC000 && cpu_addr <= 0xDFFF) asic_vals.prg_select = bank_reg[11];
                else asic_vals.prg_select = (prg_units-1);

                prg_addr = asic_vals.prg_select*8192;
                prg_offset = cpu_addr & 0x1FFF;
                return prg_addr + prg_offset;
            }
        }

        u32 map_ppu(u16 ppu_addr)
        {
            assert(ppu_addr >= 0x0000 && ppu_addr < 0x2000);
            // CHR         
            asic_vals.chr_select = bank_reg[(ppu_addr & 0x1C00) >> 10];        
            chr_addr = asic_vals.chr_select*1024;
            chr_offset = ppu_addr & 0x03FF;
            return chr_addr + chr_offset;
        }

        void map_cpu_wr(u16 cpu_addr, u8 data)
        {
            assert(cpu_addr >= 0x8000 && cpu_addr <= 0xFFFF);
            configure((cpu_addr & 0x3000) >> 13, data);
        }

        void map_ppu_wr(u16 ppu_addr, u8 data)
        {
            assert(ppu_addr >= 0x0000 && ppu_addr < 0x2000);
        }
        
        void clock_cpu_irq() 
        {
            if(asic_vals.irq_cen)
            {
                fire_irq = (--asic_vals.irq_counter == 0xFFFF && asic_vals.irq_en);
            }
        }

        void clock_ppu_irq(u16 ppu_addr) { return; }
    
    private:
        void configure(u8 asic_index, u8 data)
        {
            if(asic_index == 0x00) asic_vals.cmd_reg = (data & 0x0F);
            else execute(data);
        }

        void execute(u8 param_reg)
        {
            if(asic_vals.cmd_reg < 0x0C) 
            {
                if(asic_vals.cmd_reg < 0x08) bank_reg[asic_vals.cmd_reg] = param_reg;
                else bank_reg[asic_vals.cmd_reg] = (param_reg & (prg_units-1));
                
                if(asic_vals.cmd_reg == 0x08)
                {
                    override_ram = (param_reg & D6) == 0;
                    asic_vals.ram_enable = (param_reg & D7);
                }
            }
            else
            {
                switch(asic_vals.cmd_reg)
                {
                    case 0x0C: 
                        mirror_mode = (param_reg & 0x03) ^ 0b10; 
                        break;

                    case 0x0D: 
                        asic_vals.irq_en = (param_reg & D0); 
                        asic_vals.irq_cen = (param_reg & D7);
                        fire_irq = false;
                        break;

                    case 0x0E:
                        asic_vals.irq_counter = (asic_vals.irq_counter & 0xFF00) | param_reg;
                        break;

                    case 0x0F:
                        asic_vals.irq_counter = (asic_vals.irq_counter & 0x00FF) | static_cast<u16>(param_reg) << 8;
                        break;

                    default: break;
                }
            }
        }

        struct
        {
            bool chr_mode, ram_enable, irq_en, irq_cen;
            u8 cmd_reg, param_reg, chr_select, prg_select;
            u16 irq_counter = 0x0000;
        } asic_vals;

        u8 bank_reg[12];
        u8 prg_units, chr_units;
        u16 prg_offset, chr_offset;
        u32 prg_addr, chr_addr;
};
