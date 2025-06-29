#include <apu.hpp>
#include <cpu.hpp>

int main(int argc, char* argv[]) {
    CPU *cpu = new CPU();
    cpu->create_machine_code("");
    cpu->load_program();
    
    // APU *apu = new APU();
    // apu->generate_pwm(0xDC, 0x0A, APU::GENERATOR::SINE);
    // delete apu;
    
    delete cpu;
    return 0;
}