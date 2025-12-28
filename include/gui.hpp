#include <renderer.hpp>
#include <cpu.hpp>
#include <mmu.hpp>
#include <apu.hpp>

#include <crom.hpp>
#include <logger.hpp>

#ifndef __GUI_H__
#define __GUI_H__

#define FTPT 16
#define CLR_RED {255, 0, 0, 255}
#define CLR_GREEN {0, 255, 0, 255}
#define CLR_BLUE {0, 0, 255, 255}
#define CLR_CYAN {0, 255, 255, 255}
#define CLR_YELLOW {255, 255, 0, 255} 
#define CLR_MAGENTA {255, 0, 255, 255}
#define CLR_WHITE {255, 255, 255, 255}
#define CLR_GREY {127, 127, 127, 255}

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