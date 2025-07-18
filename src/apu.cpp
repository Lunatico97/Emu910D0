#include <apu.hpp>
#include <SDL2/SDL_audio.h>
#include <unistd.h>

#define FREQ 44100
#define SAMPLES 8192 

#define VOLUME 127.0

APU::APU() {}

float pos, step;

void APU::generate_pwm(const u8& frequency, const u8& duration, const GENERATOR& gen)
{
    SDL_AudioSpec spec;

	// Setup audio
	spec.freq = FREQ;
	spec.format = AUDIO_U8;
	spec.channels = 1;
	spec.samples = SAMPLES;
	spec.userdata = NULL;

    switch(gen)
    {
        case GENERATOR::PULSE: spec.callback = (*APU::pulse_stream); break;
        case GENERATOR::TRIG: spec.callback = (*APU::trig_stream); break;
        case GENERATOR::NOISE: spec.callback = (*APU::noise_stream); break;
        default: spec.callback = (*APU::sine_stream); break;
    }

	if (SDL_OpenAudio(&spec, NULL) < 0) { 
		std::cerr << "Failed to open audio: " << SDL_GetError() << std::endl;
		exit(1); 
	} 

    pos = 0;
    step = 2 * M_PI * (int)frequency / FREQ;

    int i = 0;
	while(i<duration){
        SDL_PauseAudio(0); 
        sleep(1);
        SDL_PauseAudio(1);
        sleep(4);
        i++;
    }

	SDL_CloseAudio();
}

void APU::sine_stream(void* data, u8 *stream, int len) 
{
	int i=0;
	for (i=0; i<len; i++) 
    {
		stream[i] = (u8) (VOLUME * sin((float)pos))+127;
		pos += step;
	}
}

void APU::pulse_stream(void* data, u8 *stream, int len) 
{
	int i=0;
	for (i=0; i<len; i++) 
    {
		stream[i] = (u8) (VOLUME * (((int)pos % 10) < 5 ? 5 : 0));
		pos += step;
	}
}

void APU::trig_stream(void* data, u8 *stream, int len) 
{
	int i=0;
	for (i=0; i<len; i++) 
    {
		stream[i] = (u8) (VOLUME * (std::abs((int)pos % 10) - 5));
		pos += step;
	}
}

void APU::noise_stream(void* data, u8 *stream, int len) 
{
	int i=0;
	for (i=0; i<len; i++) 
    {
		stream[i] = (u8) (VOLUME * (rand() % 0x0F));
		pos += step;
	}
}