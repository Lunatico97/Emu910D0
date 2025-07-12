#include <ppu.hpp>

PPU::PPU(MMU& mmui): mmu(mmui) {}

void PPU::set_ppu_ctrl(const u8& index)
{
    T = index & (D1 | D0);
}

void PPU::set_oam_addr(const u8& addr = 0x00)
{
    mmu.load_mem(OAMADDR, addr);
}

void PPU::set_oam_dma(const u8& src)
{
    mmu.load_mem(OAMDMA, (src << 8));
}