#include <gui.hpp>

GUI::GUI():system_clock(0)
{
    crom = new CardROM();
    ppu = new PPU(crom);
    mmu = new MMU(crom, ppu);
    cpu = new CPU(mmu);
    crom->load_rom("/home/diwas/Downloads/Emu910D0/roms/donkey_kong.nes");
    rndr = new Renderer("E910D0", SCRW, SCRH);
    sys_font = rndr->loadFont("rsrc/font.ttf", FTPT);
    sys_text = rndr->loadText("E910D0 [ Commands: PAUSE(6) | STEP(7) | IRQ (8) | NMI (9) | RST (0) ]", sys_font, CLR_CYAN);
    flag_text = rndr->loadText(" N   V   x   B   D   I   Z   C ", sys_font, CLR_GREEN);
    _active = true;
    cpu->rst();    
}

void GUI::cleanup()
{
    rndr->freeTex(flag_text);
    rndr->freeTex(sys_text);
    rndr->freeFont(sys_font);
    rndr->cleanRenderer();
    delete crom;
    delete rndr;
    delete ppu;
    delete cpu;
}

void GUI::draw_tile(u16 tile_addr, int x, int y, bool px = true)
{
    u8 tile_data[16] = {0};
    u8 place = D7;
    for(u8 i=0x00; i<0x10; i++) tile_data[i] = crom->read_from_ppu(tile_addr+i);

    for(u8 i=0x00; i<0x08; i++)
    {
        for(u8 j=0x00; j<0x08; j++)
        {
            place = D7 >> j;
            u8 index = ((tile_data[i] & place) >> (0x07-j)) | (((tile_data[i+0x08] & place) >> (0x07-j)) << 1);
            assert(index < 0x04);
            switch(index)
            {
                case 0x00: rndr->setColor(0,66,66,255); break;
                case 0x01: rndr->setColor(0,0,128,255); break;
                case 0x02: rndr->setColor(252,216,168,255); break;
                case 0x03: rndr->setColor(255,0,0,255); break;
                default: break;
            }

            if(px) rndr->renderPt(x+j, y+i);
            else rndr->renderRect({x+(j*5), y+(i*5), 5, 5}, true);
        }
    }
}

void GUI::draw_mem()
{
    std::string text;
    for(u16 i=0x0000; i<0x0100; i++)
    {
        text = Utils::logU8("", mmu->retreive(i));
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
        text = Utils::logU8("", mmu->retreive(i));
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
        u8 bit = (mmu->fetch_reg(ST) & (0x80 >> i)) << i;
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
        text = Utils::logU8(registers[i], mmu->fetch_reg((REG)i));
        reg_text = rndr->loadText(text.c_str(), sys_font, CLR_YELLOW);
        rndr->render(500, 50+i*(FTPT), reg_text);
        rndr->freeTex(reg_text);
    }

    // SPRs
    for(u8 i=4; i<7; i++)
    {
        text = Utils::logU8(registers[i], mmu->fetch_reg((REG)i));
        reg_text = rndr->loadText(text.c_str(), sys_font,  CLR_MAGENTA);
        rndr->render(550, 50+(i-4)*(FTPT), reg_text);
        rndr->freeTex(reg_text);
    }
}

void GUI::run_gui()
{
    u8 size = 0x08, offset = 0x00;
    bool pause = false;

    while(_active)
    {
        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_QUIT) _active = false;
            if(event.type == SDL_KEYUP)
            {
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

        // rndr->setColor(0, 0, 0, 255);
        // rndr->clear();
        // rndr->render(5, 5, sys_text);
        // draw_psw();
        // draw_reg_bank();
        // draw_stack();
        // draw_mem();

        /* 
          Don't use these below statements while unpausing emulation; these statements are cost expensive 
          as they keep fetching tile data on every render cycle but I don't need to make amends to this right now !
        */

        // Pixel 64*64 Sprite
        // draw_tile(0x0000+offset, 150, 350);
        // draw_tile(0x0000+(offset+0x10), 150, 350+size);
        // draw_tile(0x0000+(offset+0x20), 150+size, 350);
        // draw_tile(0x0000+(offset+0x30), 150+size, 350+size);
        // offset += 0x40;

        // Pixel Tilemap
        // for(u16 offset = 0x0000; offset < 0x1000; offset += 0x0010)
        // {
        //     draw_tile(offset, 150+(((offset%0x0100)/0x0010)*size), 250+((offset/0x0100)*size));
        //     draw_tile(offset+0x1000, (150+(((offset%0x0100)/0x0010)*size))+170, 250+((offset/0x0100)*size));
        // }

        if(!pause)
        {
            ppu->run_ppu(rndr);
            // ppu->draw_palette_table(rndr);

            if((system_clock % 3) == 0)
            {
                cpu->clock();
            }

            if(ppu->trigger_nmi)
            {
                cpu->nmi();
                ppu->trigger_nmi = false;
            }

            system_clock++;
            //SDL_Delay(100);
        }     
        // rndr->display();     
    }

    cleanup();
}