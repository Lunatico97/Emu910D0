#include <ppu.hpp>

PPU::PPU(CardROM *crom, Renderer *rndr) : W(0), crom(crom), cycles(0), lines(0), rndr(rndr), trigger_nmi(false) 
{
    frame_buf = (u32*)calloc(FRAME_W*FRAME_H, sizeof(u32));
    frame = rndr->loadTexture(FRAME_W, FRAME_H);
}

PPU::~PPU()
{
    rndr->freeTex(frame);
    free(frame_buf);
}

void PPU::write_from_cpu(u16 addr, u8 value)
{
    switch(addr)
    {
        case PPUCTRL: set_ppu_ctrl(value); break;
        case PPUMASK: set_ppu_mask(value); break;
        case PPUSTATUS: break;
        case OAMADDR: set_oam_addr(value); break;
        case OAMDATA: set_oam_data(value); break;
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
        case OAMDATA: get_oam_data(); break;
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
        switch(crom->get_mirror_mode())
        {
            // Single screen - 0
            case 0x00: addr &= 0x3FFF; break;
            // Single screen - 1
            case 0x01: addr = 0x4000 | (addr & 0x3FFF); break;
            // Vertical mirror mode (Horizontal arrangement)
            case 0x02: addr &= 0x07FF; break;
            // Horizontal mirror mode (Vertical arrangement)
            case 0x03: addr = (addr & 0x03FF) | ((addr & 0x0800) >> 1); break;

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
    else throw(std::runtime_error("Out of bounds (R) from VRAM: " + addr));
}

void PPU::store_vram(u16 addr, u8 value)
{
    addr &= 0x3FFF;
    if(addr >= 0x0000 && addr < 0x2000) crom->write_from_ppu(addr, value);
    else if(addr >= 0x2000 && addr < 0x3000)
    {
        addr &= 0x0FFF;
        switch(crom->get_mirror_mode())
        {
            // Single screen - 0
            case 0x00: addr &= 0x03FF; break;
            // Single screen - 1
            case 0x01: addr = 0x0400 | (addr & 0x03FF); break;
            // Vertical mirror mode (Horizontal arrangement)
            case 0x02: addr &= 0x07FF; break;
            // Horizontal mirror mode (Vertical arrangement)
            case 0x03: addr = (addr & 0x03FF) | ((addr & 0x0800) >> 1); break;
            // Default
            default: break;

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
    else throw(std::runtime_error("Out of bounds (W) to VRAM: " + addr));
}

void PPU::set_ppu_ctrl(u8 ctrl)
{
    bool old_nmi = CVALS.nmi_enabled;
    CVALS.nmi_enabled = (ctrl & D7) ? 1: 0;
    CVALS.spr_size = (ctrl & D5) ? 16: 8;
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

void PPU::set_oam_addr(u8 addr)
{
    oam_addr = addr;
}

void PPU::set_oam_data(u8 data)
{
    OAM[oam_addr] = data;
    oam_addr++;
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
    if(V.addr >= 0x3F00)
    {
        value = fetch_vram(V.addr);
        ppu_data_buffer = value;
    }
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

u8 PPU::get_oam_data()
{
    return OAM[oam_addr];
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
    bool visible_element = (lines > 0 && lines < 240) && (cycles >= 1 && cycles <= 257);

    // Frame timing for background 
    // [Based on NesWiki diagram: https://www.nesdev.org/w/images/default/4/4f/Ppu.svg]
    if((lines >= 0 && lines < 240) || lines == 261)
    {
        if((cycles >= 1 && cycles <= 257) || (cycles >= 321 && cycles <= 337))
        {
            // Shift plane data for tiles
            if(MASK_REG.bg_enabled)
            {
                P0SHF = P0SHF << 1;
                P1SHF = P1SHF << 1;
                LASHF = LASHF << 1;
                HASHF = HASHF << 1;
            }

            // Shift plane data for sprites
            if(MASK_REG.spr_enabled && (cycles >= 1 && cycles <= 257))
            {
                for(u8 i=0; i<8; i++)
                {
                    if(SPAM[4*i+3] > 0) SPAM[4*i+3]--;
                    else 
                    {
                        S0SHF[i] <<= 1;
                        S1SHF[i] <<= 1;
                    }
                }
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
                    LASHF = (LASHF & 0xFF00) | ((palette_bits & D0) ? 0xFF : 0x00);
                    HASHF = (HASHF & 0xFF00) | ((palette_bits & D1) ? 0xFF : 0x00);
                    // Fetch name table byte
                    name_byte = fetch_vram(NAME_INDEX | (V.addr & 0x0FFF));
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

            if(cycles == 338 || cycles == 340)
            {
                name_byte = fetch_vram(NAME_INDEX | (V.addr & 0x0FFF));
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

    // Frame timing for sprites
    if(lines >= 0 && lines < 240)
    {
        // Sprite Evaluation
        if(cycles == 257)
        {    
            // Secondary OAM Clear
            memset(SPAM, 0xFF, sizeof(SPAM));

            spr_cnt = 0x00;
            spr_zero_loaded = false;

            for(u8 i = 0; i < 0x40 && spr_cnt < 9; i++)
			{
				int range = lines - OAM[4*i];				
				if(range >= 0 && range < CVALS.spr_size)
				{

                    if(i == 0) spr_zero_loaded = true;

					if(spr_cnt < 8)
					{
						for(u8 j = 0; j < 4; j++)
                        {
                            SPAM[4*spr_cnt+j] = OAM[4*i+j];
                        }

						spr_cnt++;
					}				
				}
			} 

			STAT_REG.spr_ovf = (spr_cnt > 8);
        }

        if(cycles >= 257 && cycles < 321)
        {
            if((cycles - 257) % 8 == 0)
            {
                u16 tile_addr;
                u8 tile_index, tile_off;
                
                spr_cnt = (cycles - 257) / 8;
                // For sprites: 8*8 size
                if(CVALS.spr_size == 0x08)
                {
                    tile_addr = CVALS.spr_addr;
                    tile_index = SPAM[4*spr_cnt+1];
                    tile_off = (lines - SPAM[4*spr_cnt]);
                    // Vertical flip
                    if(SPAM[4*spr_cnt+2] & D7) tile_off = 0x07 - tile_off;
                }
                // For sprites: 8*16 size
                else
                {
                   tile_addr = (SPAM[4*spr_cnt+1] & D0) ? 0x1000: 0x0000;
                    tile_index = (SPAM[4*spr_cnt+1] & 0xFE);           
                    tile_off = (lines - SPAM[4*spr_cnt]);
                    
                    // Vertical flip
                    if(SPAM[4*spr_cnt+2] & D7)
                    {
                        // Top half sprite
                        if(tile_off < 0x08) tile_index += 0x01;
                        else tile_off &= 0x07;
                        tile_off = 0x07 - tile_off;
                    }
                    else
                    {
                        // Bottom half sprite
                        if(tile_off >= 0x08) 
                        {
                            tile_off &= 0x07;
                            tile_index += 0x01;
                        }
                    }
                }

                tile_addr = tile_addr + (tile_index << 4) + tile_off;
                S0SHF[spr_cnt] = fetch_vram(tile_addr);
                S1SHF[spr_cnt] = fetch_vram(tile_addr + 0x08);

                // Horizontal flip
                if(SPAM[4*spr_cnt+2] & D6)
                {
                    S0SHF[spr_cnt] = ((S0SHF[spr_cnt] & D0) << 7) | ((S0SHF[spr_cnt] & D1) << 5) | ((S0SHF[spr_cnt] & D2) << 3) |
                                     ((S0SHF[spr_cnt] & D3) << 1) | ((S0SHF[spr_cnt] & D4) >> 1) | ((S0SHF[spr_cnt] & D5) >> 3) |
                                     ((S0SHF[spr_cnt] & D6) >> 5) | ((S0SHF[spr_cnt] & D7) >> 7);
                    S1SHF[spr_cnt] = ((S1SHF[spr_cnt] & D0) << 7) | ((S1SHF[spr_cnt] & D1) << 5) | ((S1SHF[spr_cnt] & D2) << 3) |
                                     ((S1SHF[spr_cnt] & D3) << 1) | ((S1SHF[spr_cnt] & D4) >> 1) | ((S1SHF[spr_cnt] & D5) >> 3) |
                                     ((S1SHF[spr_cnt] & D6) >> 5) | ((S1SHF[spr_cnt] & D7) >> 7);
                }
            }
        }

        if(cycles >= 321 && cycles < 341)
        {
            oam_buffer = SPAM[0];
        }
    }

    if(lines == 241 && cycles == 1)
    {
        STAT_REG.vblank = 1; 
        if(CVALS.nmi_enabled) trigger_nmi = true; // Trigger NMI if enabled
    }

    // Formulate pixels to render the frame
    if(visible_element)
    {
        bool spr_priority = false;
        u8 bg_index = 0x00, spr_index = 0x00, final_index;
        u8 bg_palette_index, spr_palette_index, final_palette_index;

        // Background computation
        if(MASK_REG.bg_enabled)
        {
            u16 bit_mask = 0x8000 >> X;
            bg_index = (((P1SHF & bit_mask) > 0) << 1) | ((P0SHF & bit_mask) > 0);
            bg_palette_index = (((HASHF & bit_mask) > 0) << 1) | ((LASHF & bit_mask) > 0);
        }

        // Foreground computation
        if(MASK_REG.spr_enabled)
        {
            spr_zero_opaque = false;
            
            for(u8 i=0; i<8; i++)
            {
                if(SPAM[4*i+3] == 0)
                {
                    spr_index = (((S1SHF[i] & D7) > 0) << 1) | ((S0SHF[i] & D7) > 0);
                    spr_priority = ((SPAM[4*i+2] & D5) == 0);
                    spr_palette_index = (SPAM[4*i+2] & 0x03) + 0x04;

                    if(spr_index != 0x00)
                    {
                        if(i == 0) spr_zero_opaque = true;
                        break;
                    }
                }
            }
        }

        // Priority (Depth & Collision)
        if(bg_index == 0x00 && spr_index == 0x00)
        {
            final_index = 0x00;
            final_palette_index = 0x00;
        }
        else if(bg_index > 0x00 && spr_index == 0x00)
        {
            final_index = bg_index;
            final_palette_index = bg_palette_index;
        }
        else if(bg_index == 0x00 && spr_index > 0x00)
        {
            final_index = spr_index;
            final_palette_index = spr_palette_index;
        }
        else if(bg_index > 0x00 && spr_index > 0x00)
        {
            final_index = spr_priority ? spr_index : bg_index;
            final_palette_index = spr_priority ? spr_palette_index : bg_palette_index;

            if (spr_zero_loaded && spr_zero_opaque)
            {
                bool leftmost_contention = MASK_REG.bg_left | MASK_REG.spr_left;
                STAT_REG.spr_hit = (cycles >= (leftmost_contention ? 9: 1) && cycles < 258);
            }
        }

        // Populate pixel to frame buffer
        u8 color_select = fetch_vram(PAL_INDEX | (final_palette_index << 2) | final_index);
        frame_buf[lines*FRAME_W+(cycles-1)] = RGB_PAL[color_select];
    }

    // Reset cycles and lines
    cycles += 0x0001;
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
            trigger_events = true;
            rndr->renderFrame({0, 0, PPFW, PPFH}, frame, frame_buf, FRAME_W);
            rndr->display();
            rndr->clear();
        }
        lines += 0x0001;
    }
}

