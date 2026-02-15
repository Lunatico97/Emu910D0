#include <gui.hpp>

GUI::GUI(): _active(true)
{
    if(Global::debug) logger.init("bin/e910D0.log");
    controller = new Controller();
    rndr = new Renderer("E910D0", SCRW, SCRH);
    rndr->init_wxs();
}

void GUI::cleanup()
{
    eject_rom();
    rndr->cleanup();
    rndr->cleanRenderer();
    delete controller;
    delete rndr;
}

void GUI::load_rom(const char *rom_file)
{
    eject_rom();
    nes_state = new NES();
    nes_state->apu = new APU();
    nes_state->crom = new CardROM();
    nes_state->ppu = new PPU(nes_state->crom, rndr);
    nes_state->mmu = new MMU(nes_state->crom, nes_state->ppu, nes_state->apu, controller);
    nes_state->cpu = new CPU(nes_state->mmu);

    nes_state->system_clock = 0;
    nes_state->rom = Global::rom_path;
    current_rom = strdup(rom_file);
    nes_state->crom->load_rom(nes_state->rom.append(rom_file).c_str());
    nes_state->crom->persist_ram(current_rom, true);
    nes_state->cpu->pow();
    nes_state->apu->init();
    _rom_ld = true;
}

void GUI::eject_rom()
{
    if(nes_state != nullptr)
    {
        nes_state->crom->persist_ram(current_rom);
        _mmu_vw = false;
        _ppu_vw = false;
        _rom_vw = false;
        _rom_ld = false;
        free(current_rom);
        delete nes_state->apu;
        delete nes_state->cpu;
        delete nes_state->ppu;
        delete nes_state->crom;
        delete nes_state;
        nes_state = nullptr;
    }
}

void GUI::run_nes()
{
    if(!_pause)
    {
        nes_state->ppu->trigger_events = false;
        nes_state->ppu->run_ppu();

        if((nes_state->system_clock % 3) == 0)
        {
            if(nes_state->mmu->dma_rqst) nes_state->mmu->perform_dma(nes_state->system_clock % 2);
            else
            {
                nes_state->cpu->clock();
                nes_state->apu->clock_apu_fcnt(); 
            }
        }

        if(nes_state->ppu->trigger_nmi)
        {
            nes_state->cpu->nmi();
            nes_state->ppu->trigger_nmi = false;
        }

        if(nes_state->apu->frame_irq || nes_state->crom->map_irq)
        {
        	nes_state->cpu->irq();
        	nes_state->crom->map_irq = false;
		}

        nes_state->system_clock++;
    }
}

bool GUI::check_ppu_events()
{
    if(nes_state != nullptr) return nes_state->ppu->trigger_events;
    else return true;
}

void GUI::run_gui()
{
    controller->configure();

    while(_active)
    {
        while(check_ppu_events() && SDL_PollEvent(&event))
        {
            controller->handleInput(&event);
            ImGui_ImplSDL2_ProcessEvent(&event);
            if(event.type == SDL_EventType::SDL_QUIT) _active = false;
            if(event.type == SDL_EventType::SDL_KEYUP && _rom_ld)
            {
                nes_state->ppu->trigger_events = true;
                switch(event.key.keysym.sym)
                {
                    case SDLK_ESCAPE: _active = false; break;
                    case SDLK_5: nes_state->apu->toggle_apu(); break;
                    case SDLK_6: _pause = !_pause; break;
                    case SDLK_7: nes_state->cpu->step(); break;
                    case SDLK_8: nes_state->cpu->irq(); break;
                    case SDLK_9: nes_state->cpu->nmi(); break;
                    case SDLK_0: nes_state->cpu->rst(); break;            
                    default: break;
                }
            }
        }

        if(_rom_ld)
        {
            this->run_nes();
        }

        if(check_ppu_events())
        {
            rndr->set_frame();
            
            this->create_menu();
            if(_rom_vw) this->create_rom_loader(&_rom_vw);
            if(_mmu_vw) nes_state->mmu->peek_mmu(&_mmu_vw);
            if(_ppu_vw) nes_state->ppu->peek_ppu(&_ppu_vw);

            rndr->fit_frame();
            rndr->display();
            rndr->clear();
        }
    }

    cleanup();
}

void GUI::create_rom_loader(bool *rom_up)
{
    ImGui::SetNextWindowPos({0.0f, 20.0f});
    ImGui::SetNextWindowSize({240.0f, 200.f});
    ImGui::Begin("Load ROM", rom_up, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
    std::vector<std::string> roms = Global::scan_files(Global::rom_path, "nes");
    if(ImGui::BeginListBox("##ROMS", {-FLT_MIN, 140.0f}))
    {
        for(int i=0; i<roms.size(); i++)
        {
            if(ImGui::Selectable(roms[i].c_str(), index == i)) index = i;
        }
        ImGui::EndListBox();
    }
    if(ImGui::Button("Load"))
    {
        this->load_rom(roms[index].c_str());
    }
    ImGui::End();
}

void GUI::create_menu()
{
    ImGui::BeginMainMenuBar();
    if(ImGui::BeginMenu("File"))
    {
        if(ImGui::MenuItem("Load ROM")) _rom_vw = true;
        if(ImGui::MenuItem("Eject ROM")) this->eject_rom();
        if(ImGui::MenuItem("Quit Emu910D0", "ESC", nullptr, true)) _active = false;
        ImGui::EndMenu();
    }
    if(ImGui::BeginMenu("Options"))
    {
        if(ImGui::MenuItem("Pause/Unpause", "NUM_6", nullptr, _rom_ld)) _pause = !_pause;
        if(ImGui::MenuItem("Mute/Unmute", "NUM_5", nullptr, _rom_ld)) nes_state->apu->toggle_apu();
        if(ImGui::MenuItem("Reset", "NUM_0", nullptr, _rom_ld)) nes_state->cpu->rst();
        ImGui::EndMenu();
    }
    if(ImGui::BeginMenu("Debug"))
    {    
        if(ImGui::BeginMenu("Peeks"))
        {
            if(ImGui::MenuItem("Memory Viewer", NULL, nullptr, _rom_ld)) _mmu_vw = true;
            if(ImGui::MenuItem("Palette RAM", NULL, nullptr, _rom_ld)) _ppu_vw = true;
            ImGui::EndMenu();
        }
        if(ImGui::BeginMenu("Interrupts"))
        {
            if(ImGui::MenuItem("STEP", "NUM_7", nullptr, _rom_ld)) nes_state->cpu->step();
            if(ImGui::MenuItem("IRQ", "NUM_8", nullptr, _rom_ld)) nes_state->cpu->irq();
            if(ImGui::MenuItem("NMI", "NUM_9", nullptr, _rom_ld)) nes_state->cpu->nmi();
            ImGui::EndMenu();
        }
        ImGui::EndMenu();
    }
    if(ImGui::BeginMenu("Info"))
    {
        if(ImGui::BeginMenu("About"))
        {
            ImGui::TextDisabled("EMU910D0 v1.0.0");
            ImGui::TextDisabled("Author: Diwas Adhikari");
            ImGui::TextLinkOpenURL("Github", "https://github.com/Lunatico97/Emu910D0");
            ImGui::EndMenu();
        }
        if(ImGui::BeginMenu("Dependencies"))
        {
            ImGui::TextDisabled("SDL v2.0.20");
            ImGui::TextDisabled("Dear ImGui v1.92.5");
            ImGui::EndMenu();
        }
        ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
}
