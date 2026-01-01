#include <global.hpp>
#include <SDL2/SDL_audio.h>

#ifndef _APU_H_
#define _APU_H_

#define APUP1DC 0x4000
#define APUP1SW 0x4001
#define APUP1TM 0x4002
#define APUP1LC 0x4003

#define APUP2DC 0x4004
#define APUP2SW 0x4005
#define APUP2TM 0x4006
#define APUP2LC 0x4007

#define APUTGLN 0x4008
#define APUTGTM 0x400A
#define APUTGLC 0x400B

#define APUNSEN 0x400C
#define APUNSTM 0x400E
#define APUNSLC 0x400F

#define APUSTAT 0x4015
#define APUFCNT 0x4017

class APU
{
    public:
        APU();
        ~APU();

        // APU Controls
        void init();
        void toggle_apu();
        void clock_apu_fcnt();

        // R/W Operations
        u8 read_from_cpu(u16 cpu_addr);
        void write_from_cpu(u16 cpu_addr, u8 data);
    
    private:
        // Pulse Channel
        struct PULSE_CH {
            u16 timer; // 11-bit timer
            u16 counter;
            u8 duty_cycle;
            u8 sequencer;
            u8 volume;
            u8 length;
            u8 period;
            u8 decay;
            u8 shift;
            bool lc_halt;
            bool const_vol;
            bool swp_en;
            bool neg_en;
        };

        // Triangular Channel
        struct TRIG_CH {
            u16 timer; // 11-bit timer
            u16 counter;
            u8 length;
            u8 sequencer = 0x0F;
            u8 step = 0x00;
            u8 linear_set;
            u8 linear_cnt;
            bool lc_halt;
            bool ln_set;
        };

        struct NOISE_CH {
            u16 lfsr = 0x0001; // 15-bit LFSR
            u16 counter;
            u16 timer;
            u8 length;
            u8 volume;
            u8 decay;
            u8 steps;
            bool mode;
            bool lc_halt;
            bool const_vol;
        };

        // Callbacks
        static void apu_callback(void *data, u8* stream, int len);
        static void clock_pwm(PULSE_CH* pulse_ch, bool qtr_frame, bool half_frame);
        static void clock_tri(TRIG_CH& trig_ch,  bool qtr_frame, bool half_frame);
        static void clock_wno(NOISE_CH& trig_ch,  bool qtr_frame, bool half_frame);

        // Pulse Channel
        void set_pulse_duty(u8 index, u8 data); 
        void set_pulse_lcnt(u8 index, u8 data);
        void set_pulse_sweep(u8 index, u8 data);
        void set_pulse_timer(u8 index, u8 data);

        // Triangular Channel
        void set_trig_linc(u8 data);
        void set_trig_lcnt(u8 data);
        void set_trig_timer(u8 data);

        // Noise Channel
        void set_noise_envl(u8 data);
        void set_noise_lcnt(u8 data);
        void set_noise_timer(u8 data);

        // APU Common Internals
        void set_apu_stat(u8 data);
        void set_apu_fcnt(u8 data);

        // Status register
        struct {
            bool dmc_en = false;
            bool wno_en = false;
            bool tri_en = false;
            bool pu1_en = false;
            bool pu0_en = false;
        } apu_stat;

        // Frame counter
        struct {
            u16 frame_cnt;
            bool step_mode;
            bool irq_flag;
        } apu_fcnt;

        // Audio Thread Data
        struct APU_DATA {
            bool half_frame = false;
            bool quarter_frame = false;
            PULSE_CH pulse_ch[2];
            TRIG_CH trig_ch;
            NOISE_CH noise_ch;
        } apu_data;
        
        SDL_AudioDeviceID device_id;
        SDL_AudioSpec spec;
        bool mute_apu = true;
};

#endif