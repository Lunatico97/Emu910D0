#include <gui.hpp>

GUI::GUI():system_clock(0)
{
    if(Global::debug) logger.init("bin/e910D0.log");
    controller = new Controller();
    crom = new CardROM();
    rndr = new Renderer("E910D0", SCRW, SCRH);
    ppu = new PPU(crom, rndr);
    mmu = new MMU(crom, ppu, controller);
    cpu = new CPU(mmu);
    crom->load_rom("roms/smb1.nes");
    _active = true;
    cpu->rst();
}

void GUI::cleanup()
{
    rndr->cleanRenderer();
    delete crom;
    delete rndr;
    delete ppu;
    delete cpu;
}

void GUI::run_gui()
{
    bool pause = false;

    while(_active)
    {
        while(ppu->trigger_events && SDL_PollEvent(&event))
        {
            controller->handleInput(&event);
            if(event.type == SDL_EventType::SDL_QUIT) _active = false;
            if(event.type == SDL_EventType::SDL_KEYUP)
            {
                ppu->trigger_events = true;
                switch(event.key.keysym.sym)
                {
                    case SDLK_6: pause = !pause; break;
                    case SDLK_7: cpu->step(); break;
                    case SDLK_8: cpu->irq(); break;
                    case SDLK_9: cpu->nmi(); break;
                    case SDLK_0: cpu->rst(); break;
                    
                    default: break;
                }
            }
        }

        if(!pause)
        {
            ppu->trigger_events = false;
            ppu->run_ppu();

            if((system_clock % 3) == 0)
            {
                if(mmu->dma_rqst) mmu->perform_dma(system_clock % 2);
                else cpu->clock();
            }

            if(ppu->trigger_nmi)
            {
                cpu->nmi();
                ppu->trigger_nmi = false;
            }

            system_clock++;
        }     
    }

    cleanup();
}