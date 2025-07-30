#include <ppu.hpp>

PPU::PPU(CardROM *crom) : W(0), crom(crom), cycles(0), lines(0), trigger_nmi(false) {}

void PPU::write_from_cpu(u16 addr, u8 value)
{
    switch(addr)
    {
        case PPUCTRL: set_ppu_ctrl(value); break;
        case PPUMASK: set_ppu_mask(value); break;
        case PPUSTATUS: break;
        case OAMADDR: break;
        case OAMDATA: break;
        case PPUSCROLL: set_ppu_scrl(value); break;
        case PPUADDR: set_ppu_addr(value); break;
        case PPUDATA: set_ppu_data(value); break;
        default: break;
    }
}

u8 PPU::read_from_cpu(u16 addr)
{
    switch(addr)
    {
        case PPUCTRL: break;
        case PPUMASK: break;
        case PPUSTATUS: return get_ppu_stat();
        case OAMADDR: break;
        case OAMDATA: break;
        case PPUSCROLL: break;
        case PPUADDR: break;
        case PPUDATA: return get_ppu_data();
        default: break;
    }
    
    return 0x00; 
}

u8 PPU::fetch_vram(u16 addr)
{
    addr &= 0x3FFF;
    if(addr >= 0x0000 && addr < 0x2000) return crom->read_from_ppu(addr);
    else if(addr >= 0x2000 && addr < 0x3F00) return NAME[(addr & 0x07FF)];
    else if(addr >= 0x3F00 && addr < 0x4000)
    {
        u16 pal_idx = addr & 0x001F; // Original mask
        if ((pal_idx & 0x0003) == 0) pal_idx &= 0x000F; 
        return PAL[pal_idx];
    }
    else throw(std::runtime_error(Utils::logU16("Out of bounds (R) from VRAM: ", addr)));
}

void PPU::store_vram(u16 addr, u8 value)
{
    addr &= 0x3FFF;
    if(addr >= 0x0000 && addr < 0x2000) return;
    if(addr >= 0x2000 && addr < 0x3F00) NAME[(addr & 0x07FF)] = value;
    else if(addr >= 0x3F00 && addr < 0x4000)
    {
        u16 pal_idx = addr & 0x001F; // Original mask
        if((pal_idx & 0x0003) == 0) pal_idx &= 0x000F; 
        PAL[pal_idx] = value;
    }
    else throw(std::runtime_error(Utils::logU16("Out of bounds (W) to VRAM: ", addr)));
}

void PPU::set_ppu_ctrl(u8 ctrl)
{
    std::cout << Utils::logU8("PPUCTRL: ", ctrl) << std::endl;
    bool old_nmi = ((ppu_ctrl & D7) == D7);
    ppu_ctrl = ctrl;
    T &= ~0x0C00;
    T |= ((u16)(ctrl & 0x03) << 10);
    nmi_enabled = ((ctrl & D7) == D7);
    if((ctrl & D2) != D2) vram_icr = 0x01;
    else vram_icr = 0x20; 
    if((ctrl & D3) != D3) spr_addr = 0x0000;
    else spr_addr = 0x1000; 
    if((ctrl & D4) != D4) bg_addr = 0x0000;
    else bg_addr = 0x1000;
    if((ctrl & D5) != D5) spr_size = 0x08;
    else spr_size = 0x10;

    if(!old_nmi && nmi_enabled && (ppu_status & D7) == D7) trigger_nmi = true;
}

void PPU::set_ppu_mask(u8 mask)
{
    std::cout << Utils::logU8("PPUMASK: ", mask) << std::endl;
    ppu_mask = mask;
}

void PPU::set_ppu_scrl(u8 pos)
{
    std::cout << Utils::logU8("PPUSCLL: ", pos) << std::endl;
    if(W == 0x00)
    {
        X = pos & 0x07; // Fine X
        T = (T & 0xFFE0) | ((pos >> 3) & 0x1F); // Coarse X
        T = (T & 0xFBFF) | ((pos & 0x08) << 7); // Name Table
    }
    else
    {
        T = (T & 0x8FFF) | ((pos & 0x07) << 12); // Fine Y
        T = (T & 0xFC1F) | ((pos & 0xF8) << 2); // Coarse Y
    }
    W = ~W;
}

void PPU::set_ppu_addr(u8 addr)
{
    if(W == 0) T = (T & 0x00FF) | (addr << 8); // higher nibble
    else 
    {
        T = (T & 0xFF00) | addr; // lower nibble
        V = T;
    }

    W = ~W;
}
 
void PPU::set_ppu_data(u8 data)
{
   std::cout << Utils::logU16("PPUADDR: ", V) << " " << Utils::logU8("PPUDATA: ", data) << std::endl;
   store_vram(V, data);
   V += vram_icr;
}

u8 PPU::get_ppu_data()
{
    u8 value;
    if(V >= 0x3F00) value = fetch_vram(V);
    else
    {
        value = ppu_data_buffer;
        ppu_data_buffer = fetch_vram(V);
    }
    V += vram_icr;
    return value;
}

u8 PPU::get_ppu_stat()
{
    W = 0;
    // ppu_status |= D7;
    u8 out_stat = (ppu_status & 0xE0) | (ppu_data_buffer & 0x1F);
    ppu_status &= ~D7;
    return out_stat;
}

void PPU::update_v(bool vt = 0)
{
    if(!vt)
    {
        // Coarse scrolling X
        if((V & 0x001F) == 31)
        {
            V &= ~0x001F;         
            V ^= 0x0400; // NN = 01         
        }
        else V += 1;  
    }
    else
    {
        // Fine scrolling Y
        if ((V & 0x7000) != 0x7000) V += 0x1000;
        else
        {
            V &= ~0x7000;
        
            // Coarse scrolling Y
            u16 y = (V & 0x03E0) >> 5; // coarse Y
            
            if (y == 29)
            {
                y = 0;
                V ^= 0x0800; // NN = 10
            }    
            else if (y == 31) y = 0;
            else y += 1;
            
            V = (V & ~0x03E0) | (y << 5); // put coarse Y back into v
        }
    }         
}

void PPU::run_ppu(Renderer *rndr)
{
    cycles += 0x0001;

    if(cycles == 340)
    {
        cycles = 0x0000;
        if(lines == 261)
        {
            lines = 0x0000;
            spr_hit = 0;
            spr_ovf = 0;
            W = 0;
            ppu_status &= ~D7;

            rndr->display();
            rndr->setColor(0, 0, 0, 255);
            rndr->clear();
        }
        lines += 0x0001;
    }

    bg_enabled = ((ppu_mask & D3) == D3);
    spr_enabled = ((ppu_mask & D4) == D4);

    u8 bg_index = 0x00;
    bool visible_element = (lines >= 0 && lines < 240) && (cycles >= 1 && cycles <= 256);

    if(visible_element)
    {
        if(bg_enabled)
        {
            u8 color_index = ((((HBSHF >> (0x0F - X)) & 0x01)) << 1) | ((LBSHF >> (0x0F - X)) & 0x01);
            u8 palette_index = ((((HASHF >> (0x07 - X)) & 0x01)) << 1) | ((LASHF >> (0x07 - X)) & 0x01);
            if(color_index == 0x00) bg_index = fetch_vram(0x3F00);
            else bg_index = fetch_vram(0x3F00 | (palette_index << 2) | color_index);
            // std::cout << Utils::logU8("Index: ", bg_index) << std::endl;
        }
        else bg_index = fetch_vram(0x3F00);

        // Foreground computation stuff (later ...)

        // Priority (Depth & Collision) (later ...)

        // Render pixel
        // std::cout << "(" << cycles-1 << ", " << lines << ")" << std::endl;
        rndr->setColor(RGB_PAL[bg_index].r, RGB_PAL[bg_index].g, RGB_PAL[bg_index].b, 255);
        rndr->renderPt(cycles-1, lines);
        // rndr->renderRect({0+((cycles-1)*5), 0+(lines*5), 5, 5}, true);
    }

    if(bg_enabled || spr_enabled)
    {
        LBSHF = LBSHF << 1;
        HBSHF = HBSHF << 1;
        LASHF = LASHF << 1;
        HASHF = HASHF << 1;
    }

    // Frame timing for background 
    // [Based on NesWiki diagram: https://www.nesdev.org/w/images/default/4/4f/Ppu.svg ]
    u16 tile_addr, attr_addr;

    if((lines >= 0 && lines < 240) || lines == 261)
    {
        // In every cycle to draw pixel, some specific computation happens which obviously repeats every tile (8px)
        u8 cycle_num = cycles % 8;
        switch (cycle_num)
        {
            case 0x00:
                // Update V register horizontal
                LBSHF = LBL;
                HBSHF = HBL;
                if(bg_enabled || spr_enabled) update_v();
                break;

            case 0x01:
                // Read name table
                tile_addr = 0x2000 | (V & 0x0FFF);
                name_byte = fetch_vram(tile_addr);
                break;
            
            case 0x03:
                // Read attribute from name table
                attr_addr = ATRB_INDEX | (V & 0x0C00) | ((V >> 4) & 0x38) | ((V >> 2) & 0x07);
                attr_byte = fetch_vram(attr_addr);
                break;

            case 0x05:
                // Get shifter low bits
                LBL = fetch_vram(bg_addr + (name_byte*0x10) + ((V >> 12) & 0x0007));
                break;

            case 0x07:
                // Get shifter high bits
                HBL = fetch_vram(bg_addr + (name_byte*0x10) + ((V >> 12) & 0x0007) + 0x08);
                break;
            
            default:
                break;
        }

        if(cycles == 256)
        {
            if(bg_enabled || spr_enabled) update_v(1);
        }

        if(cycles == 257)
        {
            V = (V & 0x7BE0) | (T & 0x041F); // Horizontal transfer
        }
    }

    // Frame timing for sprites (later...)

    if(lines == 241 && cycles == 1)
    {
        ppu_status |= D7; 
        std::cout << Utils::logU8("PPUSTAT: ", ppu_status) << std::endl;
        if(nmi_enabled) trigger_nmi = true; // Trigger NMI if enabled
    }

    if(lines == 261)
    {
        if(cycles  >= 280 && cycles <= 304)
        {
            V = (V & 0x841F) | (T & 0x7BE0); // Vertical transfer
        }
    }
}
