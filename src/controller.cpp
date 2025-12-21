#include <controller.hpp>

Controller::Controller() {}

Controller::~Controller()
{
    SDL_GameControllerClose(gamepad);
}

void Controller::configure()
{
    if(SDL_NumJoysticks() < 1) std::cout << "No controllers connected | Use keyboard !\n";
    else
    {
        //Load joystick
        gamepad = SDL_GameControllerOpen(0);
        if(gamepad == nullptr)
        {
            std::cerr << "Warning: Unable to open game controller! [" << SDL_GetError() << "]\n";
        }
        else
        {
            std::cout << "Controller connected: " << SDL_GameControllerName(gamepad) << " !\n";
            gamepad_input = true;
        }
    }
}

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
        switch(event->key.keysym.sym)
        {
            case SDLK_LEFTBRACKET: inst_state[0] &= ~D7; break;
            case SDLK_RIGHTBRACKET: inst_state[0] &= ~D6; break;
            case SDLK_SPACE: inst_state[0] &= ~D5; break;
            case SDLK_RETURN: inst_state[0] &= ~D4; break;
            case SDLK_w: inst_state[0] &= ~D3; break;
            case SDLK_s: inst_state[0] &= ~D2; break;
            case SDLK_a: inst_state[0] &= ~D1; break;
            case SDLK_d: inst_state[0] &= ~D0; break;
            default: break;
        }
    }

    if(gamepad_input && event->type == SDL_EventType::SDL_CONTROLLERBUTTONDOWN)
    {
        switch(event->cbutton.button)
        {
            case SDL_CONTROLLER_BUTTON_Y: inst_state[0] |= D7; break;
            case SDL_CONTROLLER_BUTTON_B: inst_state[0] |= D6; break;
            case SDL_CONTROLLER_BUTTON_BACK: inst_state[0] |= D5; break;
            case SDL_CONTROLLER_BUTTON_START: inst_state[0] |= D4; break;
            case SDL_CONTROLLER_BUTTON_DPAD_UP: inst_state[0] |= D3; break;
            case SDL_CONTROLLER_BUTTON_DPAD_DOWN: inst_state[0] |= D2; break;
            case SDL_CONTROLLER_BUTTON_DPAD_LEFT: inst_state[0] |= D1; break;
            case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: inst_state[0] |= D0; break;
            default: break;
        }
    }

    if(gamepad_input && event->type == SDL_EventType::SDL_CONTROLLERBUTTONUP)
    {
        switch(event->cbutton.button)
        {
            case SDL_CONTROLLER_BUTTON_Y: inst_state[0] &= ~D7; break;
            case SDL_CONTROLLER_BUTTON_B: inst_state[0] &= ~D6; break;
            case SDL_CONTROLLER_BUTTON_BACK: inst_state[0] &= ~D5; break;
            case SDL_CONTROLLER_BUTTON_START: inst_state[0] &= ~D4; break;
            case SDL_CONTROLLER_BUTTON_DPAD_UP: inst_state[0] &= ~D3; break;
            case SDL_CONTROLLER_BUTTON_DPAD_DOWN: inst_state[0] &= ~D2; break;
            case SDL_CONTROLLER_BUTTON_DPAD_LEFT: inst_state[0] &= ~D1; break;
            case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: inst_state[0] &= ~D0; break;
            default: break;
        }
    }

    if(gamepad_input && event->type == SDL_EventType::SDL_CONTROLLERDEVICEREMOVED)
    {
        std::cout << "Controller removed: " << SDL_GameControllerName(gamepad) << "!\n";
        SDL_GameControllerClose(gamepad);
        gamepad = nullptr;        
    }

    if(gamepad == nullptr && event->type == SDL_EventType::SDL_CONTROLLERDEVICEADDED)
    {
        configure();
    }
}