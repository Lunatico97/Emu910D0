#include <mappers/mapper.hpp>

class Mapper000: public Mapper
{
    public:
        Mapper000(u8 prg, u8 chr)
        {
            prg_units = prg;
            chr_units = chr;
        }

        u16 map_cpu(u16 cpu_addr)
        {
            // std::cout << Utils::logU16("ADDR: ", cpu_addr) << std::endl;
            assert(cpu_addr >= 0x8000 && cpu_addr <= 0xFFFF);
            if(prg_units == 1)
            {
                return (cpu_addr & 0x3FFF);
            }
            else
            {
                return cpu_addr;
            }
        }

        u16 map_ppu(u16 ppu_addr)
        {
            assert(ppu_addr >= 0x0000 && ppu_addr < 0x2000);
            return ppu_addr;
        }
    
    private:
        u8 prg_units, chr_units;
};