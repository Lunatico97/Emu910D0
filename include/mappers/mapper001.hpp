#include <mappers/mapper.hpp>

class Mapper001: public Mapper
{
    public:
        Mapper001(u8 prg, u8 chr)
        {
            prg_units = prg;
            chr_units = chr;
            shift_reg = 0x10;
            memset(asic_reg, 0x00, 4);
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
                switch((asic_reg[0] & 0x0C) >> 2)
                {
                    case 0x00:
                    case 0x01:
                        prg_offset = cpu_addr & 0x7FFF;
                        prg_addr = (asic_vals.prg_select & 0x0E)*PRG_BANK;
                        break;

                    case 0x02: 
                        prg_offset = cpu_addr & 0x3FFF;
                        if(cpu_addr >= 0x8000 && cpu_addr <= 0xBFFF) prg_addr = 0x0000; 
                        else prg_addr = asic_vals.prg_select*PRG_BANK;
                        break;

                    case 0x03: 
                        prg_offset = cpu_addr & 0x3FFF;
                        if(cpu_addr >= 0x8000 && cpu_addr <= 0xBFFF) prg_addr = asic_vals.prg_select*PRG_BANK; 
                        else prg_addr = (prg_units-1)*PRG_BANK;
                        break;

                    default: 
                        break;
                }
                return prg_addr + prg_offset;
            }
        }

        u32 map_ppu(u16 ppu_addr)
        {
            assert(ppu_addr >= 0x0000 && ppu_addr < 0x2000);
            // CHR         
            if(asic_vals.chr_mode)
            {
                chr_offset = ppu_addr & 0x0FFF;
                if(ppu_addr >= 0x0000 && ppu_addr < 0x1000) chr_addr = asic_vals.chr_select1*4096;
                else chr_addr = asic_vals.chr_select2*4096;
            }
            else
            {
                chr_offset = ppu_addr & 0x1FFF;
                chr_addr = (asic_vals.chr_select1 & 0x1E)*4096;
            }
            return chr_addr + chr_offset;
        }

        void map_cpu_wr(u16 cpu_addr, u8 data)
        {
            assert(cpu_addr >= 0x8000 && cpu_addr <= 0xFFFF);
            if(data & D7)
            { 
                shift_reg = 0x10;
                asic_reg[0] |= 0x0C;
            }
            else shift(cpu_addr, data);
        }

        void map_ppu_wr(u16 ppu_addr, u8 data)
        {
            assert(ppu_addr >= 0x0000 && ppu_addr < 0x2000);
        }
    
    private:
        void configure(u8 asic_index)
        {
            switch(asic_index)
            {
                case 0x00: 
                    mirror_mode = asic_reg[0] & 0x03;
                    asic_vals.chr_mode = (asic_reg[0] & D4) > 0;
                    break;

                case 0x01: 
                    asic_vals.chr_select1 = asic_reg[1] & 0x1F; 
                    break;

                case 0x02: 
                    asic_vals.chr_select2 = asic_reg[2] & 0x1F; 
                    break;

                case 0x03: 
                    asic_vals.prg_select = asic_reg[3] & 0x0F;
                    asic_vals.prg_disabled = asic_reg[3] & D4; // MMC1(B) 
                    break;

                default: break;
            }
        }

        void shift(u16 addr_in, u8 data_in)
        {
            if(shift_reg & D0)
            {
                u8 ctrl_select = (addr_in & 0x6000) >> 13;
                asic_reg[ctrl_select] = ((shift_reg & 0x1E) >> 1) | ((data_in & D0) << 4);
                configure(ctrl_select & 0x03);
                shift_reg = 0x10;
            }
            else
            {
                shift_reg >>= 1;  
                shift_reg |= ((data_in & D0) << 4);
            }
        }

        struct
        {
            bool chr_mode, prg_disabled;
            u8 prg_select, chr_select1, chr_select2;
        } asic_vals;

        u8 prg_units, chr_units;
        u8 shift_reg, asic_reg[4];
        u16 prg_offset, chr_offset;
        u32 prg_addr, chr_addr;
};