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
    if(addr >= 0x0000 && addr < 0x2000) return crom->read_from_ppu(addr);
    else if(addr >= 0x2000 && addr < 0x3000){
        if(crom->mirror_mode)
        {
            if(addr >= 0x2400 && addr < 0x2800) addr &= 0x03FF;
            else if(addr >= 0x2C00 && addr < 0x3000) addr &= 0x07FF;
        }
        else addr &= 0x07FF;
        return NAME[addr];
    }
    else if(addr >= 0x3000 && addr < 0x3F00) return NAME[(addr & 0x0EFF)];
    else if(addr >= 0x3F00 && addr < 0x4000)
    {
        u8 pal_idx = addr & 0x1F;
        if ((pal_idx & 0x03) == 0) pal_idx &= 0x0F; 
        return PAL[pal_idx];
    }
    else throw(std::runtime_error(Utils::logU16("Out of bounds (R) from VRAM: ", addr)));
}

void PPU::store_vram(u16 addr, u8 value)
{
    if(addr >= 0x0000 && addr < 0x2000) return;
     else if(addr >= 0x2000 && addr < 0x3000){
        if(crom->mirror_mode)
        {
            if(addr >= 0x2400 && addr < 0x2800) addr &= 0x03FF;
            else if(addr >= 0x2C00 && addr < 0x3000) addr &= 0x07FF;
        }
        else addr &= 0x07FF;
        NAME[addr] = value;
    }
    else if(addr >= 0x3000 && addr < 0x3F00) NAME[(addr & 0x0EFF)] = value;
    else if(addr >= 0x3F00 && addr < 0x4000)
    {
        u8 pal_idx = addr & 0x1F;
        if ((pal_idx & 0x03) == 0) pal_idx &= 0x0F; 
        PAL[pal_idx] = value;
    }
    else throw(std::runtime_error(Utils::logU16("Out of bounds (W) to VRAM: ", addr)));
}

void PPU::set_ppu_ctrl(u8 ctrl)
{
    std::cout << Utils::logU8("PPUCTRL: ", ctrl) << std::endl;
    bool old_nmi = CTRL_REG.nmi_enabled;
    bg_addr = CTRL_REG.bg_addr ? 0x1000: 0x0000;
    spr_addr = CTRL_REG.spr_addr ? 0x1000: 0x0000;
    CTRL_REG.byte = ctrl;
    T.nm_select = CTRL_REG.nm_addr;
    // if(!old_nmi && CTRL_REG.nmi_enabled && STAT_REG.vblank) trigger_nmi = true;
}

void PPU::set_ppu_mask(u8 mask)
{
    std::cout << Utils::logU8("PPUMASK: ", mask) << std::endl;
    MASK_REG.byte = mask;
}

void PPU::set_ppu_scrl(u8 pos)
{
    std::cout << Utils::logU8("PPUSCLL: ", pos) << std::endl;
    if(W == 0x00)
    {
        // Get horizontal scroll position
        X = pos & 0x07;
        T.coarse_x = (pos >> 3);
    }
    else
    {
        // Get vertical scroll position
        T.fine_y = pos & 0x07;
        T.coarse_y = (pos >> 3);
    }
    // Toggle latch
    W = ~W;
}

void PPU::set_ppu_addr(u8 addr)
{
    if(W == 0)
    {
       // Get higher address byte
       addr &= 0x3F;
       T.addr = (T.addr & 0x00FF) | (static_cast<u16>(addr) << 8);
    }
    else 
    {
        // Get lower address byte
        T.addr = (T.addr & 0xFF00) | addr;
        V.addr = T.addr;
    }
    // Toggle latch
    W = ~W;
}
 
void PPU::set_ppu_data(u8 data)
{
   std::cout << Utils::logU16("PPUADDR: ", V.addr) << " " << Utils::logU8("PPUDATA: ", data) << std::endl;
   store_vram(V.addr, data);
   V.addr += (CTRL_REG.vram_icr ? 0x20: 0x01);
}

u8 PPU::get_ppu_data() 
{
    u8 value;
    if(V.addr >= 0x3F00) value = fetch_vram(V.addr);
    else
    {
        value = ppu_data_buffer;
        ppu_data_buffer = fetch_vram(V.addr);
    }
    V.addr += (CTRL_REG.vram_icr ? 0x20: 0x01);
    return value;
}

u8 PPU::get_ppu_stat()
{
    W = 0;
    u8 out_stat = (STAT_REG.byte & 0xE0) | (ppu_data_buffer & 0x1F);
    STAT_REG.vblank = 0;
    return out_stat;
}

void PPU::update_v(bool vt = 0)
{
    if(!vt)
    {
        // Update horizontal scroll position
        if(V.coarse_x == 0x20)
        {
            V.coarse_x = 0x00;         
            V.nm_select = 0b01;         
        }
        else V.coarse_x += 0x01;  
    }
    else
    {
        // Update vertical scroll position
        if (V.fine_y != 0x07) V.fine_y += 0x01;
        else
        {
            V.fine_y = 0x00;
            if (V.coarse_y == 0x1D)
            {
                V.coarse_y = 0x00;
                V.nm_select = 0b10;
            }    
            else if (V.coarse_y == 0x1F) V.coarse_y = 0x00;
            else V.coarse_y += 0x01;
        }
    }         
}

void PPU::run_ppu(Renderer *rndr)
{
    // Reset cycles and lines
    cycles += 0x0001;
    if(cycles == 340)
    {
        cycles = 0x0000;
        if(lines == 261)
        {
            lines = 0x0000;
            STAT_REG.spr_hit = 0;
            STAT_REG.spr_ovf = 0;
            STAT_REG.vblank = 0;
            W = 0;

            rndr->display();
            rndr->setColor(0, 0, 0, 255);
            rndr->clear();
        }
        lines += 0x0001;
    }

    u8 bg_index = 0x00;
    bool visible_element = (lines >= 0 && lines < 240) && (cycles >= 1 && cycles <= 256);

    if(visible_element)
    {
        if(MASK_REG.bg_enabled)
        {
            u8 color_index = ((((P1SHF >> (0x0F - X)) & 0x01)) << 1) | ((P0SHF >> (0x0F - X)) & 0x01);
            u8 palette_index = ((((HASHF >> (0x07 - X)) & 0x01)) << 1) | ((LASHF >> (0x07 - X)) & 0x01);
            if(color_index == 0x00) bg_index = fetch_vram(0x3F00);
            else bg_index = fetch_vram(0x3F00 | (palette_index << 2) | color_index);
        }
        else bg_index = fetch_vram(0x3F00);

        // Foreground computation stuff (later ...)

        // Priority (Depth & Collision) (later ...)

        // Render pixel
        // std::cout << "(" << cycles-1 << ", " << lines << ")" << std::endl;
        rndr->setColor(RGB_PAL[bg_index].r, RGB_PAL[bg_index].g, RGB_PAL[bg_index].b, 255);
        // rndr->renderPt(cycles-1, lines);
        rndr->renderRect({0+((cycles-1)*PIX), 0+(lines*PIY), PIX, PIY}, true);
    }

    // Shift plane data for tiles
    if(MASK_REG.bg_enabled || MASK_REG.spr_enabled)
    {
        P0SHF = P0SHF << 1;
        P1SHF = P1SHF << 1;
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
                // Load latched data to shift registers
                P0SHF = static_cast<u16>(P0L) << 8;
                P1SHF = static_cast<u16>(P1L) << 8;
                LASHF = (palette_bits & D0) == D0 ? 0xFF: 0x00;
                HASHF = (palette_bits & D1) == D1 ? 0xFF: 0x00;
                // Update V register horizontal
                if(MASK_REG.bg_enabled || MASK_REG.spr_enabled) update_v();
                break;

            case 0x01:
                // Fetch name table byte
                name_byte = fetch_vram(0x2000 | (V.addr & 0x0FFF));
                break;
            
            case 0x03:
                // Fetch attribute byte
                attr_addr = ATRB_INDEX | (V.addr & 0x0C00) | ((V.addr >> 4) & 0x38) | ((V.addr >> 2) & 0x07);
                attr_byte = fetch_vram(attr_addr);
                break;

            case 0x05:
                // Latch plane-0 data
                P0L = fetch_vram(bg_addr + (name_byte << 4) + V.fine_y);
                palette_select = ((V.addr & 0x02) >> 1) | ((V.addr & 0x40) >> 5);
                break;

            case 0x07:
                // Latch plane-1 data
                P1L = fetch_vram(bg_addr + (name_byte << 4) + V.fine_y + 0x08);
                palette_bits = (attr_byte >> (palette_select * 2)) & 0x03;
                break;
            
            default:
                break;
        }

        // Render-only operations
        if(MASK_REG.bg_enabled || MASK_REG.spr_enabled)
        {
            if(cycles == 256)
            {
                // Update V register vertical
                update_v(1);
            }

            if(cycles == 257)
            {
                // Perform horizontal transfer
                V.coarse_x = T.coarse_x;
                V.nm_select = T.nm_select & 0b01;
            }

            if(lines == 261)
            {
                if(cycles >= 280 && cycles <=304)
                {
                    // Perform vertical transfer
                    V.coarse_y = T.coarse_y;
                    V.fine_y = T.fine_y;
                    V.nm_select = T.nm_select & 0b10;
                }
            }
        }
    }

    // Frame timing for sprites (later...)

    if(lines == 241 && cycles == 1)
    {
        STAT_REG.vblank = 1; 
        std::cout << Utils::logU8("PPUSTAT: ", STAT_REG.byte) << std::endl;
        if(CTRL_REG.nmi_enabled) trigger_nmi = true; // Trigger NMI if enabled
    }
}

void PPU::draw_palette_table(Renderer *rndr)
{
    u8 pal;
    for(u8 i=0x00; i<0x20; i++)
    {
        pal = PAL[i];
        rndr->renderRect({400+(i%0x10)*10, 200+(i/0x10)*10, 10, 10}, RGB_PAL[pal].r, RGB_PAL[pal].g, RGB_PAL[pal].b, 1);
        rndr->renderRect({400+(i%0x10)*10, 400+(i/0x10)*10, 10, 10}, RGB_PAL[i].r, RGB_PAL[i].g, RGB_PAL[i].b, 1);
    }
}
