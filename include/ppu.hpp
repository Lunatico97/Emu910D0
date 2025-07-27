#include <utils.hpp>
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
#define OAMDMA 0x4014

#define ATRB_INDEX 0x23C0 // start of last 64 bytes for name table
#define PAL_INDEX 0x3F00

class PPU
{
    public:
        PPU(CardROM *crom);

        // CPU Interactions
        u8 read_from_cpu(u16 addr);
        void write_from_cpu(u16 addr, u8 value);

        // PPU Cycle
        void run_ppu(Renderer *rndr);

        // PPU Counters
        u16 cycles, lines;
        bool trigger_nmi;

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

        // Update routines
        void update_v(bool vt);

        // Memory interactors
        u8 fetch_vram(u16 addr);
        void store_vram(u16 addr, u8 value);
        
        // PPU Memory
        u8 NAME[2048], OAM[256], SPAM[32], PAL[32]; 
        u8 ppu_data_buffer, ppu_status, ppu_mask, ppu_ctrl;

        // PPU Internal Registers
        u16 LBSHF, HBSHF;
        u8 LASHF, HASHF;
        u16 T, V; // 0 yyy NN YYYYY XXXXX
        bool W;
        u8 X;

        // Controls
        bool nmi_enabled = 0, bg_enabled = 1, spr_enabled = 1;
        u16 spr_addr;
        u16 bg_addr;
        u8 spr_size;
        u8 vram_icr = 0x01;
        
        // Status flags
        bool spr_ovf = 0, spr_hit = 0;

        // Latch
        u16 HBL, LBL;
        u8 name_byte, attr_byte;

        // CROM
        CardROM *crom;
};

#endif