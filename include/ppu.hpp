#include <utils.hpp>
#include <mmu.hpp>

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

class PPU
{
    public:
        PPU();
        PPU(MMU& mmu);

        void set_ppu_ctrl(const u8& ctrl);
        void set_ppu_mask(const u8& mask);
        void set_ppu_scrl(const u8& ctrl);
        void set_ppu_addr(const u8& addr);
        void set_ppu_data(const u8& data);
        void set_oam_addr(const u8& addr);
        void set_oam_data(const u8& data);
        void set_oam_dma(const u8& src);

        void get_ppu_stat();
        void get_ppu_data();
        void get_oam_data();

    private:
        MMU& mmu;
        u8 VRAM[2048], OAM[256]; 
        u8 T, V, W, X;
};

#endif