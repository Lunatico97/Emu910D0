#include <crom.hpp>
#include <mappers/mapper000.hpp>
#include <mappers/mapper001.hpp>
#include <mappers/mapper002.hpp>
#include <mappers/mapper003.hpp>
#include <mappers/mapper004.hpp>

CardROM::CardROM(){}
CardROM::~CardROM()
{
    delete mapper;
    free(PRGROM);
    free(CHRROM);
    free(PRGRAM);
    free(CHRRAM);
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
    PRGRAM = (u8*)calloc(8192, sizeof(u8));
    CHRRAM = (u8*)calloc(CHR_BANK, sizeof(u8));
    PRGROM = (u8*)calloc(prg_units*PRG_BANK, sizeof(u8));
    CHRROM = (u8*)calloc(chr_units*CHR_BANK, sizeof(u8));
    mapper_num = (header[7] & 0xF0) | (header[6] >> 4);
    mirror_mode = (header[6] & D0);    
    persistence = (header[6] & D1);
    switch(mapper_num)
    {
        case 0x00: mapper = new Mapper000(prg_units, chr_units); break;
        case 0x01: mapper = new Mapper001(prg_units, chr_units); break;
        case 0x02: mapper = new Mapper002(prg_units, chr_units); break;
        case 0x03: mapper = new Mapper003(prg_units, chr_units, PRGROM); break;
        case 0x04: mapper = new Mapper004(prg_units, chr_units); break;
        default: break;
    }
}

void CardROM::load_rom(const char *filename)
{
    std::ifstream fs(filename, std::ios::binary);
    if(!fs.is_open())
    {
        std::cerr << "Error: Could not open file: '" << filename << "'\n";
        return;
    }
        
    u8 header[16] = {0};  
    fs.read(reinterpret_cast<char*>(header), 16);

    decode(header);
    std::cout << "ROM: " << filename << "\n";
    std::cout << "PRG Banks: [ " << static_cast<int>(prg_units) << " ]\n";  
    std::cout << "CHR Banks: [ " << static_cast<int>(chr_units) << " ]\n";  
    std::cout << "Mapper: [ " << static_cast<int>(mapper_num) << " ]\n";
    std::cout << "Mirror Mode: " << (mirror_mode ? "Vertical": "Horizontal") << std::endl;

    fs.read(reinterpret_cast<char*>(PRGROM), prg_units*PRG_BANK);
    fs.read(reinterpret_cast<char*>(CHRROM), chr_units*CHR_BANK);
    fs.close();
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

void CardROM::persist_ram(const char *rom, bool on_load)
{
    if(persistence)
    {
        std::string save_path = "saves/";
        save_path += rom;
        save_path += ".sav";
        if(on_load) Global::readDataFromFile(save_path.c_str(), PRGRAM, 0x2000);
        else Global::writeDataToFile(save_path.c_str(), PRGRAM, 0x2000);
    }
}

u8 CardROM::get_mirror_mode()
{
    if(mapper->mirror_mode != 0xFF) return mapper->mirror_mode;
    else return !mirror_mode + 0x02;
}