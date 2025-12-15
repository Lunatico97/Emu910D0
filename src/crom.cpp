#include <crom.hpp>
#include <mappers/mapper000.hpp>
#include <mappers/mapper002.hpp>
#include <mappers/mapper003.hpp>

CardROM::CardROM(){}
CardROM::~CardROM()
{
    delete mapper;
    free(PRGROM);
    free(CHRROM);
}

void CardROM::decode(u8 header[])
{
    assert(header[0] == 0x4E); // N
    assert(header[1] == 0x45); // E
    assert(header[2] == 0x53); // S
    assert(header[3] == 0x1A); // \n
    prg_units = header[4]; // PRG ROM SIZE
    chr_units = header[5]; // CHR ROM SIZE
    assert(prg_units != 0);
    PRGRAM = (u8*)calloc(2048, sizeof(u8));
    CHRRAM = (u8*)calloc(CHR_BANK, sizeof(u8));
    PRGROM = (u8*)calloc(prg_units*PRG_BANK, sizeof(u8));
    CHRROM = (u8*)calloc(chr_units*CHR_BANK, sizeof(u8));
    mapper_num = (header[7] & 0xF0) | (header[6] >> 4);
    mirror_mode = (header[6] & D0);    
    switch(mapper_num)
    {
        case 0x00: mapper = new Mapper000(prg_units, chr_units); break;
        case 0x02: mapper = new Mapper002(prg_units, chr_units); break;
        case 0x03: mapper = new Mapper003(prg_units, chr_units, PRGROM); break;
        default: break;
    }
}

void CardROM::load_rom(const char *filename)
{
    std::stringstream ss(Global::readTextFromFile(filename));
    ss >> std::hex >> std::noskipws;
    u32 counter = 0x0000;
    u8 current_hex = 0x00;
    u8 header[16] = {0};  

    while(ss >> current_hex)
    {
        if(counter < 0x0010)
        {
            header[counter] = current_hex;
            counter += 0x0001;
        }
        else break;
    }

    decode(header);
    std::cout << "PRG Banks: [ " << static_cast<int>(prg_units) << " ]\n";  
    std::cout << "CHR Banks: [ " << static_cast<int>(chr_units) << " ]\n";  
    std::cout << "Mapper: [ " << static_cast<int>(mapper_num) << " ]\n";
    std::cout << "Mirror Mode: " << (mirror_mode ? "Vertical": "Horizontal") << std::endl;

    *(PRGROM + 0) = current_hex;
    u32 prg_limit = 0x000F + (prg_units*PRG_BANK);

    while(ss >> current_hex)
    {
        if(counter < prg_limit) *(PRGROM + (counter-0x000F)) = current_hex; 
        else *(CHRROM + (counter-prg_limit)) = current_hex; 
        counter += 0x0001;
    } 
}

u8 CardROM::read_from_cpu(u16 cpu_addr)
{
    if(cpu_addr >= 0x6000 && cpu_addr <= 0x7FFF) return *(PRGRAM + mapper->map_cpu(cpu_addr));
    else return *(PRGROM + mapper->map_cpu(cpu_addr));
}

u8 CardROM::read_from_ppu(u16 ppu_addr)
{
    u32 mapped_addr = mapper->map_ppu(ppu_addr);
    return chr_units == 0 ? *(CHRRAM + mapped_addr): *(CHRROM + mapped_addr);
}

void CardROM::write_from_cpu(u16 cpu_addr, u8 data)
{
    if(cpu_addr >= 0x6000 && cpu_addr <= 0x7FFF) *(PRGRAM + mapper->map_cpu(cpu_addr)) = data;
    else mapper->map_cpu_wr(cpu_addr, data);
}

void CardROM::write_from_ppu(u16 ppu_addr, u8 data)
{
    mapper->map_ppu_wr(ppu_addr, data);
    *(CHRRAM + ppu_addr) = data;
}