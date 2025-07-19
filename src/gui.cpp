#include <gui.hpp>

GUI::GUI()
{
    cpu = new CPU();
    cpu->load_machine_code("");
    rndr = new Renderer("E910D0", SCRW, SCRH);
    sys_font = rndr->loadFont("rsrc/font.ttf", FTPT);
    sys_text = rndr->loadText("E910D0 [ Commands: STEP(7) | IRQ (8) | NMI (9) | RST (0) ]", sys_font, CLR_CYAN);
    flag_text = rndr->loadText(" N   V   x   B   D   I   Z   C ", sys_font, CLR_GREEN);
    mmu = cpu->get_mmu(); 
    _active = true;    
}

void GUI::cleanup()
{
    rndr->freeTex(flag_text);
    rndr->freeTex(sys_text);
    rndr->freeFont(sys_font);
    rndr->cleanRenderer();
    delete rndr;
    delete cpu;
}

void GUI::draw_mem()
{
    std::string text;
    for(u16 i=0x0000; i<0x0100; i++)
    {
        text = Utils::logU8("", mmu.fetch_mem(i));
        reg_text = rndr->loadText(text.c_str(), sys_font, CLR_WHITE);
        rndr->render(5+(i%16)*25, 50+(i/16)*FTPT, reg_text);
        rndr->freeTex(reg_text);
    }
}

void GUI::draw_stack()
{
    std::string text;
    for(u16 i=0x01F0; i<=0x01FF; i++)
    {
        text = Utils::logU8("", mmu.fetch_mem(i));
        reg_text = rndr->loadText(text.c_str(), sys_font, CLR_BLUE);
        rndr->render(425, 50+(i-0x01F0)*FTPT, reg_text);
        rndr->freeTex(reg_text);
    }
}

void GUI::draw_psw()
{
    rndr->render(475, 5, flag_text);

    SDL_Rect rect = {475, 30, 8, 8};
    for(u8 i=0; i<8; i++)
    {
        u8 bit = (mmu.tapREG(ST) & (0x80 >> i)) << i;
        if(i == 2) rndr->renderRect(rect, 128, 128, 128, bit);
        else rndr->renderRect(rect, 255, 0, 0, bit);
        rect.x += 20;
    }
}

void GUI::draw_reg_bank()
{
    std::string text;
    const char* registers[] = { "A", "X", "Y", "ST", "SP", "PCH", "PCL" };

    // GPRs
    for(u8 i=0; i<3; i++)
    {
        text = Utils::logU8(registers[i], mmu.tapREG((REG)i));
        reg_text = rndr->loadText(text.c_str(), sys_font, CLR_YELLOW);
        rndr->render(500, 50+i*(FTPT), reg_text);
        rndr->freeTex(reg_text);
    }

    // SPRs
    for(u8 i=4; i<7; i++)
    {
        text = Utils::logU8(registers[i], mmu.tapREG((REG)i));
        reg_text = rndr->loadText(text.c_str(), sys_font,  CLR_MAGENTA);
        rndr->render(550, 50+(i-4)*(FTPT), reg_text);
        rndr->freeTex(reg_text);
    }
}

void GUI::run_gui()
{
    while(_active)
    {
        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_QUIT) _active = false;
            if(event.type == SDL_KEYUP)
            {
                switch(event.key.keysym.sym)
                {
                    case SDLK_7: cpu->step(); break;
                    case SDLK_8: cpu->irq(); break;
                    case SDLK_9: cpu->nmi(); break;
                    case SDLK_0: cpu->rst(); break;
                    
                    default: break;
                }

                mmu = cpu->get_mmu();
            }
        }

        rndr->setColor(0, 0, 0, 255);
        rndr->clear();
        rndr->render(5, 5, sys_text);
        draw_psw();
        draw_reg_bank();
        draw_stack();
        draw_mem();
        rndr->display();
    }

    cleanup();
}