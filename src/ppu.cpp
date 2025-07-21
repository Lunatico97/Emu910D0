#include <ppu.hpp>

PPU::PPU() : W(0x00) {}

void PPU::write_from_cpu(u16 addr, u8 value)
{
    switch(addr)
    {
        case PPUCTRL: set_ppu_ctrl(value); break;
        case PPUSTATUS: break;
        case PPUMASK: set_ppu_mask(value); break;
        case OAMADDR: break;
        case OAMDATA: break;
        case PPUSCROLL: break;
        case PPUADDR: set_ppu_addr(value); break;
        case PPUDATA: set_ppu_data(value); break;
        default: break;
    }
}

u8 PPU::read_from_cpu(u16 addr)
{
    u8 value;

    switch(addr)
    {
        case PPUCTRL: break;
        case PPUSTATUS: return get_ppu_stat();
        case PPUMASK: break;
        case OAMADDR: break;
        case OAMDATA: break;
        case PPUSCROLL: break;
        case PPUADDR: break;
        case PPUDATA: return get_ppu_data();
        default: break;
    }

    return value;
}

u8 PPU::fetch_vram(u16 addr)
{
    if(addr >= 0x2000 && addr < 0x2800) return NAME[addr-0x2000];
    else if(addr >= 0x2800 && addr < 0x3F00) return NAME[(addr-0x2800) & 0x07FF];
    else if(addr >= 0x3F00 && addr < 0x3F20) return PAL[addr-0x3F00];
    else if(addr >= 0x3F20 && addr < 0x4000) return PAL[(addr-0x3F20) & 0x001F];
    else throw(std::runtime_error("Out of bounds from VRAM"));
}

void PPU::store_vram(u16 addr, u8 value)
{
    if(addr >= 0x2000 && addr < 0x2800) NAME[addr-0x2000] = value;
    else if(addr >= 0x2800 && addr < 0x3F00) NAME[(addr-0x2800) & 0x07FF] = value;
    else if(addr >= 0x3F00 && addr < 0x3F20) PAL[addr-0x3F00] = value;
    else if(addr >= 0x3F20 && addr < 0x4000) PAL[(addr-0x3F20) % 0x001F] = value;
    else throw(std::runtime_error("Out of bounds from VRAM"));
}

void PPU::set_ppu_ctrl(u8 ctrl)
{
    base_vram_addr = 0x2000 + 0x0400*((ctrl & D1) | (ctrl & D0));
    vblank = ((ctrl & D7) == D7);
    if((ctrl & D2) != D2) vram_icr = 0x01;
    else vram_icr = 0x20; 
    if((ctrl & D3) != D3) spr_addr = 0x0000;
    else spr_addr = 0x1000; 
    if((ctrl & D4) != D4) bg_addr = 0x0000;
    else bg_addr = 0x1000; 
    if((ctrl & D5) != D5) spr_size = 0x08;
    else spr_size = 0x10;
}

void PPU::set_ppu_mask(u8 mask)
{
    ppu_mask = mask;
}

void PPU::set_ppu_scrl(u8 pos)
{
    W = ~W;
}

void PPU::set_ppu_addr(u8 addr)
{
    if(W == 0x00) ppu_addr = (ppu_addr & 0x00FF) | (addr << 8); // higher nibble
    else ppu_addr = (ppu_addr & 0xFF00) | addr; // lower nibble
    W = ~W;
}
 
void PPU::set_ppu_data(u8 data)
{
   store_vram(ppu_addr, data);
}

u8 PPU::get_ppu_data()
{
    if(ppu_addr >= 0x3F00) return ppu_data_buffer;
    else
    {
        u8 value = ppu_data_buffer;
        ppu_data_buffer = fetch_vram(ppu_addr);
        return value;
    }
}

u8 PPU::get_ppu_stat()
{
    W = 0x00;
    ppu_status &= ~D7;
    return (ppu_status & 0xE0) | (ppu_data_buffer & 0x1F);
}

void PPU::run_ppu()
{
    // Prefetch
    set_ppu_ctrl(fetch_vram(PPUCTRL));
    V = base_vram_addr + vram_icr;
    T = V;
    // act_tile = base + y*32 + x
    // attr = base + ATRB_OFF

    // Get color - RGB_PAL[read_ppu(PAL_INDEX + (pallette << 2) + pixel)] 

    /*
    The PPU runs at approximately 5.37 MHz (NTSC).

    There are 3 PPU clock cycles for every 1 CPU clock cycle.

    Each scanline consists of 341 PPU cycles.

    A full frame consists of 262 scanlines.
    NN YYYYY XXXXX FFF
    || ||||| ||||| +-- Fine Y scroll (3 bits, 0-7, pixel row within tile)
    || ||||| +-----+-- Coarse X scroll (5 bits, 0-31, tile column in Nametable)
    || +-----------+-- Coarse Y scroll (5 bits, 0-29 usually, tile row in Nametable)
    ++-------------+--- Nametable select (2 bits, 0-3, which of the 4 logical Nametables
    */
    

    // Render
}