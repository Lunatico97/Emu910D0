#include <global.hpp>

#ifndef _APU_H_
#define _APU_H_

class APU
{
    public:
        enum GENERATOR
        {
            SINE = 0,
            TRIG, PULSE, NOISE
        };

    public:
        APU();
        void generate_pwm(const u8& frequency, const u8& duration, const GENERATOR& gen);
    
    private:
        static void sine_stream(void* data, u8 *stream, int len);
        static void trig_stream(void* data, u8 *stream, int len);
        static void pulse_stream(void* data, u8 *stream, int len);
        static void noise_stream(void* data, u8 *stream, int len);
};

#endif