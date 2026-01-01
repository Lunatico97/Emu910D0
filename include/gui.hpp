#include <renderer.hpp>
#include <cpu.hpp>
#include <mmu.hpp>
#include <apu.hpp>

#include <crom.hpp>
#include <logger.hpp>

#ifndef __GUI_H__
#define __GUI_H__

class GUI
{
    public:
        GUI(const char *rom_path);     
        void run_gui();

    private:
        void cleanup();

        // Global timer
        u32 system_clock;

        // Console pointer objects
        Controller* controller;
        CardROM* crom;
        CPU* cpu;
        PPU* ppu;
        MMU* mmu;
        APU* apu;

        // Graphic status & pointer objects
        bool _active;
        Renderer* rndr;
        SDL_Event event;
};

#endif