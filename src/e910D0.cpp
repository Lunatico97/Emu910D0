/*
    E910D0 v1.0 [NES Emulator]
    Author: Diwas Adhikari
    Description: NES Emulator to relive my childhood memories of playing with a bootleg NES console 
                 which looked like a PS1 and had a vertical cartridge slot like a SNES :)
*/

#include <apu.hpp>
#include <gui.hpp>

Logger logger;

int main(int argc, char* argv[]) {
    GUI* gui = new GUI();
    gui->run_gui();
    
    // APU *apu = new APU();
    // apu->generate_pwm(0xDC, 0x0A, APU::GENERATOR::SINE);
    // delete apu;
    
    delete gui;
    return 0;
}