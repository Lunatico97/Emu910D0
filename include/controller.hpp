#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include <global.hpp>

#define CTRL_P1 0x4016
#define CTRL_P2 0x4017

class Controller
{
    public:
        Controller();
        ~Controller();
        
        void configure();
        u8 read_state(bool strobe);
        void write_state(bool strobe);
        void handleInput(SDL_Event *event);

    private:
        // Order of input state: [A, B, SL, ST, U, D, L, R]
        u8 inst_state[2], state[2];
        bool gamepad_input = false;
        SDL_GameController* gamepad;
};

#endif