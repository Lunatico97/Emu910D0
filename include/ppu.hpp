#include <global.hpp>
#include <renderer.hpp>
#include <crom.hpp>

#ifndef _PPU_H_
#define _PPU_H_

#define PPUCTRL 0x2000 // [ V P H B S I N N ]
#define PPUMASK 0x2001
#define PPUSTATUS 0x2002
#define OAMADDR 0x2003
#define OAMDATA 0x2004
#define PPUSCROLL 0x2005
#define PPUADDR 0x2006
#define PPUDATA 0x2007

#define NAME_INDEX 0x2000 // start of first name table
#define ATRB_INDEX 0x23C0 // start of last 64 bytes for first name table
#define PAL_INDEX 0x3F00

#define FRAME_W 256
#define FRAME_H 240

class PPU
{
    public:
        PPU(CardROM *crom, Renderer *rndr);
        ~PPU();

        // CPU Interactions
        u8 read_from_cpu(u16 addr);
        void write_from_cpu(u16 addr, u8 value);

        // PPU Cycle
        void run_ppu();
        void peek_ppu(bool* ppu_up);

        // PPU Counters
        u16 cycles, lines;
        bool trigger_nmi, trigger_events;

    private:
        // Setter routines
        void set_ppu_ctrl(u8 ctrl);
        void set_ppu_mask(u8 mask);
        void set_ppu_scrl(u8 pos);
        void set_ppu_addr(u8 addr);
        void set_ppu_data(u8 data);
        void set_oam_addr(u8 addr);
        void set_oam_data(u8 data);

        // Getter routines
        u8 get_ppu_stat();
        u8 get_ppu_data();
        u8 get_oam_data();

        // Update routines
        void update_horzv();
        void update_vertv();

        // PPU Interactions
        u8 fetch_vram(u16 addr);
        u8 fetch_palette(u16 addr);
        void store_vram(u16 addr, u8 value);
        void store_palette(u16 addr, u8 value);
        
        // PPU Memory
        u8 NAME[2048], OAM[256], SPAM[32], PAL[32]; 
        u8 ppu_data_buffer, oam_addr;

        // OAM Counters
        u8 spr_cnt = 0x00, oam_buffer = 0xFF;
        bool spr_zero_loaded, spr_zero_opaque;

        // Shift & Latch Registers
        u8 P0L, P1L;
        u16 P0SHF, P1SHF;
        u16 LASHF, HASHF;
        u8 S0SHF[8], S1SHF[8];

        // Latch
        u8 name_byte, attr_byte, palette_select, palette_bits;

        // Internal registers
        u8 X;
        bool W;
        union PPUINTL_REG
        {
            u16 addr; // 0 yyy NN YYYYY XXXXX
            struct
            {
                u16 coarse_x: 5;
                u16 coarse_y: 5;
                u16 nm_select: 2;
                u16 fine_y: 3;
            }; 
        } T, V;

        // Control register values
        struct PPUCTRL_VAL 
        {
            u16 bg_addr;
            u16 spr_addr;
            u8 vram_icr;
            u8 spr_size;
            bool nmi_enabled;
        } CVALS;

        // Mask register
        union PPUMASK_REG
        {
            u8 byte;
            struct
            {
                u8 greyscale: 1;
                u8 bg_left: 1;
                u8 spr_left: 1;
                u8 bg_enabled: 1;
                u8 spr_enabled: 1;
                u8 red: 1;
                u8 green: 1;
                u8 blue: 1;
            };
        } MASK_REG;

        // Status register
        union PPUSTAT_REG
        {
            u8 byte;
            struct
            {
                u8 bus: 5;
                u8 spr_ovf: 1;
                u8 spr_hit: 1;
                u8 vblank: 1;
            };
        } STAT_REG;

        // Frame buffer
        u32* frame_buf;
        SDL_Texture* frame;

        // CROM
        Renderer* rndr;
        CardROM *crom;

    public:
        u8* refOAM = OAM;
        u8* refoam_addr = &oam_addr;
};

#endif