#include <utils.hpp>

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
#define OAMDMA 0x4014

#define ATRB_OFF 0x03C0 // start of last 64 bytes for name table
#define PAL_INDEX 0x3F00

class PPU
{
    public:
        PPU();

        // CPU Interactions
        u8 read_from_cpu(u16 addr);
        void write_from_cpu(u16 addr, u8 value);

        // PPU Cycle
        void run_ppu();

    private:
        // Setter routines
        void set_ppu_ctrl(u8 ctrl);
        void set_ppu_mask(u8 mask);
        void set_ppu_scrl(u8 pos);
        void set_ppu_addr(u8 addr);
        void set_ppu_data(u8 data);

        // Getter routines
        u8 get_ppu_stat();
        u8 get_ppu_data();
        u8 get_oam_data();

        // Memory interactors
        u8 fetch_vram(u16 addr);
        void store_vram(u16 addr, u8 value);
        
        // PPU Memory & Registers
        u8 NAME[2048], OAM[256], SPAM[32], PAL[32]; 
        u16 ppu_addr;
        u8 ppu_data_buffer;
        u8 ppu_status;
        u8 ppu_mask;
        u8 T, V, W, X;

        // Controls
        bool vblank = 0;
        u16 base_vram_addr;
        u16 spr_addr;
        u16 bg_addr;
        u8 spr_size;
        u8 vram_icr = 0x01;
        
        // Status flags
        bool spr_ovf = 0, spr_hit = 0;
        
};

#endif