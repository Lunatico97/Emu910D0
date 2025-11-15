#include <ppu.hpp>

PPU::PPU(CardROM *crom, Renderer *rndr) : W(0), crom(crom), cycles(0), lines(0), rndr(rndr), trigger_nmi(false) 
{
    frame_buf = (u32*)calloc(FRAME_W*FRAME_H, sizeof(u32));
    frame = rndr->loadTexture(FRAME_W, FRAME_H);
}

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
    else if(addr >= 0x2000 && addr < 0x3000)
    {
        addr &= 0x0FFF;
        if(crom->mirror_mode)
        {
            // Vertical mirror mode (Horizontal arrangement)
            addr &= 0x07FF;
        }
        else
        {
            // Horizontal mirror mode (Vertical arrangement)
            if(addr >= 0x0400 && addr < 0x0800) addr &= 0x03FF;
            addr &= 0x0BFF;
        }
        return NAME[addr];
    }
    else if(addr >= 0x3000 && addr < 0x3F00) return fetch_vram(addr-0x1000);
    else if(addr >= 0x3F00 && addr < 0x4000)
    {
        addr = addr & 0x1F;
        if ((addr & 0x03) == 0) addr &= 0x0F; 
        return PAL[addr];
    }
    else throw(std::runtime_error(Utils::logU16("Out of bounds (R) from VRAM: ", addr)));
}

void PPU::store_vram(u16 addr, u8 value)
{
    addr &= 0x3FFF;
    if(addr >= 0x0000 && addr < 0x2000) return;
     else if(addr >= 0x2000 && addr < 0x3000)
     {
        addr &= 0x0FFF;
        if(crom->mirror_mode)
        {
            // Vertical mirror mode (Horizontal arrangement)
            addr &= 0x07FF;
        }
        else
        {
            // Horizontal mirror mode (Vertical arrangement)
            if(addr >= 0x0400 && addr < 0x0800) addr &= 0x03FF;
            addr &= 0x0BFF;
        }
        NAME[addr] = value;
    }
    else if(addr >= 0x3000 && addr < 0x3F00) store_vram(addr-0x1000, value);
    else if(addr >= 0x3F00 && addr < 0x4000)
    {
        addr = addr & 0x1F;
        if ((addr & 0x03) == 0) addr &= 0x0F; 
        PAL[addr] = value;
    }
    else throw(std::runtime_error(Utils::logU16("Out of bounds (W) to VRAM: ", addr)));
}

void PPU::set_ppu_ctrl(u8 ctrl)
{
    bool old_nmi = CVALS.nmi_enabled;
    CVALS.nmi_enabled = (ctrl & D7) ? 1: 0;
    CVALS.bg_addr = (ctrl & D4) ? 0x1000: 0x0000;
    CVALS.spr_addr = (ctrl & D3) ? 0x1000: 0x0000;
    CVALS.vram_icr = (ctrl & D2) ? 0x20: 0x01;
    T.nm_select = (ctrl & 0x03);
    if(!old_nmi && CVALS.nmi_enabled && STAT_REG.vblank) trigger_nmi = true;
}

void PPU::set_ppu_mask(u8 mask)
{
    MASK_REG.byte = mask;
}

void PPU::set_ppu_scrl(u8 pos)
{
    if(!W)
    {
        // Set horizontal scroll position
        X = pos & 0x07;
        T.coarse_x = (pos >> 3);
    }
    else
    {
        // Set vertical scroll position
        T.fine_y = pos & 0x07;
        T.coarse_y = (pos >> 3);
    }
    // Toggle latch
    W = !W;
}

void PPU::set_ppu_addr(u8 addr)
{
    if(!W)
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
    W = !W;
}
 
void PPU::set_ppu_data(u8 data)
{
   store_vram(V.addr, data);
   V.addr += CVALS.vram_icr;
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
    V.addr += CVALS.vram_icr;
    return value;
}

u8 PPU::get_ppu_stat()
{
    W = 0;
    u8 out_stat = (STAT_REG.byte & 0xE0) | (ppu_data_buffer & 0x1F);
    STAT_REG.vblank = 0;
    return out_stat;
}

void PPU::update_horzv()
{
    // Update horizontal scroll position
    if(V.coarse_x == 0x1F)
    {
        V.coarse_x = 0x00;         
        V.nm_select ^= 0b01;  
    }
    else V.coarse_x += 0x01;               
}

void PPU::update_vertv()
{
    // Update vertical scroll position      
    if (V.fine_y < 0x07) V.fine_y += 0x01;
    else
    {
        V.fine_y = 0x00;
        if (V.coarse_y == 0x1D)
        {
            V.coarse_y = 0x00;
            V.nm_select ^= 0b10;
        }    
        else if (V.coarse_y == 0x1F) V.coarse_y = 0x00;
        else V.coarse_y += 0x01;    
    }
}

void PPU::run_ppu()
{
    bool visible_element = (lines >= 0 && lines < 240) && (cycles >= 1 && cycles <= 256);
    cycles += 0x0001;

    // Frame timing for background 
    // [Based on NesWiki diagram: https://www.nesdev.org/w/images/default/4/4f/Ppu.svg]
    if((lines >= 0 && lines < 240) || lines == 261)
    {
        // Shift plane data for tiles
        if(MASK_REG.bg_enabled)
        {
            P0SHF = P0SHF << 1;
            P1SHF = P1SHF << 1;
            LASHF = LASHF << 1;
            HASHF = HASHF << 1;
        }

        // In every cycle to draw pixel, some specific computation happens which obviously repeats every tile (8px)
        u8 cycle_num = cycles % 8;
        switch (cycle_num)
        {
            case 0x00:
                // Update V register horizontal
                if(MASK_REG.bg_enabled || MASK_REG.spr_enabled) update_horzv();
                break;

            case 0x01:
                // Load latched data to shift registers
                P0SHF = (P0SHF & 0xFF00) | P0L; 
                P1SHF = (P1SHF & 0xFF00) | P1L;
                LASHF = (LASHF & 0xFF00) | ((palette_bits & D0) ? 0xFF: 0x00);
                HASHF = (HASHF & 0xFF00) | ((palette_bits & D1) ? 0xFF: 0x00);
                // Fetch name table byte
                name_byte = fetch_vram(0x2000 | (V.addr & 0x0FFF));
                break;
            
            case 0x03:
                // Fetch attribute byte
                attr_byte = fetch_vram(ATRB_INDEX | (V.addr & 0x0C00) | ((V.addr >> 4) & 0x38) | ((V.addr >> 2) & 0x07));
                palette_select = ((V.addr & 0x02) >> 1) | ((V.addr & 0x40) >> 5);
                palette_bits = (attr_byte >> (palette_select * 2)) & 0x03;
                break;

            case 0x05:
                // Latch plane-0 data
                P0L = fetch_vram(CVALS.bg_addr + (name_byte << 4) + V.fine_y);
                break;

            case 0x07:
                // Latch plane-1 data
                P1L = fetch_vram(CVALS.bg_addr+ (name_byte << 4) + V.fine_y + 0x08);
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
                update_vertv();
            }

            if(cycles == 257)
            {
                // Perform horizontal transfer
                V.coarse_x = T.coarse_x;
                V.nm_select = (V.nm_select & 0b10) | (T.nm_select & 0b01);
            }

            if(lines == 261 && ((cycles >= 280 && cycles <= 304)))
            {
                // Perform vertical transfer
                V.coarse_y = T.coarse_y;
                V.fine_y = T.fine_y;
                V.nm_select = (V.nm_select & 0b01) | (T.nm_select & 0b10);
            }
        }
    }

    // Frame timing for sprites (later...)

    if(lines == 241 && cycles == 1)
    {
        STAT_REG.vblank = 1; 
        if(CVALS.nmi_enabled) trigger_nmi = true; // Trigger NMI if enabled
    }

    // Formulate pixels to render the frame
    if(visible_element)
    {
        u8 bg_index = 0x00;
        if(MASK_REG.bg_enabled)
        {
            u16 bit_mask = D7 >> X;
            u8 color_index = (((P1SHF & bit_mask) > 0) << 1) | ((P0SHF & bit_mask) > 0);
            u8 palette_index = (((HASHF & bit_mask) > 0) << 1) | ((LASHF & bit_mask) > 0);
            bg_index = fetch_vram(PAL_INDEX | (palette_index << 2) | color_index);
        }
        else bg_index = fetch_vram(PAL_INDEX);

        // Foreground computation stuff (later ...)

        // Priority (Depth & Collision) (later ...)

        // Populate pixel to frame buffer
        frame_buf[lines*FRAME_W+(cycles-1)] = RGB_PAL[bg_index];
    }

    // Reset cycles and lines
    if(cycles == 340)
    {
        cycles = 0x0000;
        if(lines == 261)
        {
            // Clear VBLANK period, sprite hit and sprite overflow flag
            lines = 0x0000;
            STAT_REG.spr_hit = 0;
            STAT_REG.spr_ovf = 0;
            STAT_REG.vblank = 0;
            W = 0;

            // Render frame
            rndr->renderFrame({0, 0, SCRW, SCRH}, frame, frame_buf, FRAME_W);
            rndr->display();
            rndr->clear();
        }
        lines += 0x0001;
    }
}

