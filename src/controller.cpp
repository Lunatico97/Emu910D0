#include <controller.hpp>

Controller::Controller() {}

u8 Controller::read_state(bool strobe)
{
    u8 serial_out = ((state[strobe] & D7) > 0);
    state[strobe] <<= 1;
    return serial_out;
}

void Controller::write_state(bool strobe)
{
    state[strobe] = inst_state[strobe];
}

void Controller::handleInput(SDL_Event* event)
{
    if(event->type == SDL_EventType::SDL_KEYDOWN)
    {
        switch(event->key.keysym.sym)
        {
            case SDLK_LEFTBRACKET: inst_state[0] |= D7; break;
            case SDLK_RIGHTBRACKET: inst_state[0] |= D6; break;
            case SDLK_SPACE: inst_state[0] |= D5; break;
            case SDLK_RETURN: inst_state[0] |= D4; break;
            case SDLK_w: inst_state[0] |= D3; break;
            case SDLK_s: inst_state[0] |= D2; break;
            case SDLK_a: inst_state[0] |= D1; break;
            case SDLK_d: inst_state[0] |= D0; break;
            default: break;
        }
    }
    if(event->type == SDL_EventType::SDL_KEYUP)
    {
        inst_state[0] = 0x00;
    }
}