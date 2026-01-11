/*
    E910D0 v1.0 [NES Emulator]
    Author: Diwas Adhikari
    Description: NES Emulator to relive my childhood memories of playing with a bootleg NES console 
                 which looked like a PS1 and had a vertical cartridge slot like a SNES :)
*/

#include <gui.hpp>

Logger logger;

int main(int argc, char* argv[]) {
    GUI* gui = new GUI();
    if(argc < 2) std::cout << "ROM file not provided ! [ e910D0 _.nes ]\nPlease load ROM from UI [ File -> Load ROM ] !\n" ;
    else gui->load_rom(argv[1]);
    gui->run_gui();
    
    delete gui;
    return 0;
}