#include <mappers/mapper.hpp>

class Mapper004: public Mapper
{
    public:
        Mapper004(u8 prg, u8 chr)
        {
            prg_units = prg*2;
            chr_units = chr;
            memset(bank_reg, 0x00, 8);
        }

        u32 map_cpu(u16 cpu_addr)
        {
            assert(cpu_addr >= 0x6000 && cpu_addr <= 0xFFFF);
            if(cpu_addr >= 0x6000 && cpu_addr <= 0x7FFF) return (cpu_addr & 0x1FFF);
            else
            {
                // PRG              
                switch((cpu_addr & 0x6000) >> 13)
                {
                    case 0x00: asic_vals.prg_select = asic_vals.prg_mode ? (prg_units-2) : bank_reg[6]; break;
                    case 0x01: asic_vals.prg_select = bank_reg[7]; break;
                    case 0x02: asic_vals.prg_select = asic_vals.prg_mode ? bank_reg[6] : (prg_units-2); break;
                    case 0x03: asic_vals.prg_select = (prg_units-1); break;
                    default: break;
                }

                prg_addr = asic_vals.prg_select*8192;
                prg_offset = cpu_addr & 0x1FFF;
                
                return prg_addr + prg_offset;
            }
        }

        u32 map_ppu(u16 ppu_addr)
        {
            assert(ppu_addr >= 0x0000 && ppu_addr < 0x2000);
            // CHR
            switch((ppu_addr & 0x1C00) >> 10)
            {
                case 0x00: asic_vals.chr_select = asic_vals.chr_mode ? bank_reg[2] : bank_reg[0]; break;
                case 0x01: asic_vals.chr_select = asic_vals.chr_mode ? bank_reg[3] : bank_reg[0]; break;
                case 0x02: asic_vals.chr_select = asic_vals.chr_mode ? bank_reg[4] : bank_reg[1]; break;
                case 0x03: asic_vals.chr_select = asic_vals.chr_mode ? bank_reg[5] : bank_reg[1]; break;
                case 0x04: asic_vals.chr_select = asic_vals.chr_mode ? bank_reg[0] : bank_reg[2]; break;
                case 0x05: asic_vals.chr_select = asic_vals.chr_mode ? bank_reg[0] : bank_reg[3]; break;
                case 0x06: asic_vals.chr_select = asic_vals.chr_mode ? bank_reg[1] : bank_reg[4]; break;
                case 0x07: asic_vals.chr_select = asic_vals.chr_mode ? bank_reg[1] : bank_reg[5]; break;
                default: break;
            }

            if(ppu_addr >= 0x0000 && ppu_addr < 0x1000)
            {
                chr_addr = (asic_vals.chr_select)*1024;
                if(asic_vals.chr_mode) chr_offset = ppu_addr & 0x03FF;
                else chr_offset = ppu_addr & 0x07FF;
            }
            else
            {
                chr_addr = (asic_vals.chr_select)*1024;
                if(asic_vals.chr_mode) chr_offset = ppu_addr & 0x07FF;
                else chr_offset = ppu_addr & 0x03FF;
            }

            return chr_addr + chr_offset;
        }

        void map_cpu_wr(u16 cpu_addr, u8 data)
        {
            assert(cpu_addr >= 0x8000 && cpu_addr <= 0xFFFF);
            u8 ctrl_select = (cpu_addr & 0x6000) >> 13;
            configure(ctrl_select, data, (cpu_addr & D0) == 0x00);
        }

        void map_ppu_wr(u16 ppu_addr, u8 data)
        {
            assert(ppu_addr >= 0x0000 && ppu_addr < 0x2000);
        }
    
    private:
        void configure(u8 asic_index, u8 data, bool even)
        {
            switch(asic_index)
            {
                case 0x00: 
                    if(even)
                    {
                        asic_vals.bank_select = (data & 0x07);
                        asic_vals.prg_mode = (data & D6);
                        asic_vals.chr_mode = (data & D7);                    
                    }
                    else
                    {
                        if(asic_vals.bank_select == 0x00 || asic_vals.bank_select == 0x01) data &= 0xFE;
                        // else if(asic_vals.bank_select == 0x06 || asic_vals.bank_select == 0x07) data &= 0x3F;
                        bank_reg[asic_vals.bank_select] = data;
                    }
                    break;

                case 0x01: 
                    if(even)
                    {
                        mirror_mode = (data & D0) + 0x02;
                    }
                    else
                    {
                        asic_vals.prg_en = (data & D7);
                        asic_vals.prg_wp = (data & D6);                      
                    }
                    break;

                case 0x02: 
                    if(even)
                    {
                        asic_vals.irq_reload = data;
                    }
                    else
                    {
                        asic_vals.irq_counter = 0x00;
                        // reload at next rising edge of PPU address (cycle 260 of current scanline)                     
                    }
                    break;

                case 0x03: 
                    if(even)
                    {
                        asic_vals.irq_en = false;
                    }
                    else
                    {
                        asic_vals.irq_en = true;                    
                    }
                    break;

                default: break;
            }
        }

        struct
        {
            bool prg_mode, chr_mode, prg_en, prg_wp, irq_en;
            u8 bank_select, prg_select, chr_select;
            u8 irq_reload, irq_counter;
        } asic_vals;

        u8 bank_reg[8];
        u8 prg_units, chr_units;
        u16 prg_offset, chr_offset;
        u32 prg_addr, chr_addr;
};
