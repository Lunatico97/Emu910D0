#include <renderer.hpp>
#include <cpu.hpp>
#include <mmu.hpp>
#include <apu.hpp>

#include <crom.hpp>
#include <logger.hpp>

#ifndef __GUI_H__
#define __GUI_H__

struct NES
{
    // ROM Path
    std::string rom;
    // Global timer
    u32 system_clock = 0;
    // Components
    CardROM* crom;
    CPU* cpu;
    PPU* ppu;
    MMU* mmu;
    APU* apu;
};

class GUI
{
    public:
        GUI();  
        void run_gui();
        void load_rom(const char *rom_path);  

    private:
        // NES Operations
        void eject_rom();
        void run_nes();
        void cleanup();

        // Checks
        bool check_ppu_events();

        // UX Widgets
        void create_rom_loader(bool *rom_up);
        void create_menu();

        // Graphic status & pointer objects
        int index = 0;
        Renderer* rndr;
        NES* nes_state;
        SDL_Event event;
        Controller* controller;
        bool _active, _pause, _rom_ld;
        bool _mmu_vw, _ppu_vw, _rom_vw;
};

#endif