#include <apu.hpp>
#include <gui.hpp>

int main(int argc, char* argv[]) {
    GUI* gui = new GUI();
    gui->run_gui();
    
    // APU *apu = new APU();
    // apu->generate_pwm(0xDC, 0x0A, APU::GENERATOR::SINE);
    // delete apu;
    
    delete gui;
    return 0;
}