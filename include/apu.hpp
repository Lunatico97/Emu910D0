#include <global.hpp>
#include <SDL2/SDL_audio.h>
#include <imgui/imgui.h>

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

#define APUDMTM 0x4010
#define APUDMDL 0x4011
#define APUDMSA 0x4012
#define APUDMSL 0x4013

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
        bool frame_irq = false;
        u8 read_from_cpu(u16 cpu_addr);
        void write_from_cpu(u16 cpu_addr, u8 data);
        void peek_apu(bool* apu_up);
    
    private:
        // Pulse Channel
        struct PULSE_CH {
            u16 timer = 0x07FF; // 11-bit timer
            u16 counter;
            u16 swp_target = 0x0000;
            u8 duty_cycle;
            u8 sequencer;
            u8 length = 0x00;
            u8 shift;
            u8 env_out;
            u8 env_dcy;
            u8 env_vol = 0x00;
            u8 env_dvr;
            u8 swp_dvr;
            u8 swp_hfs;
            bool env_set;
            bool swp_set;
            bool swp_en = 0;
            bool lc_halt;
            bool const_vol;
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

        // Noise Channel
        struct NOISE_CH {
            u16 lfsr = 0x0001; // 15-bit LFSR
            u16 counter;
            u16 timer;
            u8 length;
            u8 env_out;
            u8 env_dcy;
            u8 env_vol = 0x00;
            u8 env_dvr;
            bool mode;
            bool env_set;
            bool lc_halt;
            bool const_vol;
        };

        // DMC Channel
        struct DMC_CH {
            u16 cur_addr;
            u16 smp_addr;
            u16 smp_len;
            u16 timer;
            u8 buffer = 0x00;
            u8 counter;
            u8 bit_cnt = 8;
            u8 dmc_out = 0x00;
            u8 dmc_rtsr = 0x00;
            u16 byte_rem = 0x00;
            bool buf_empty = 1;
            bool dmc_en = 0;
            bool dmc_int = 0;
            bool dmc_slc = 1;
            bool loop_en = 0;
        };

        // Callbacks
        static void apu_callback(void *data, u8* stream, int len);
        static void clock_pwm(PULSE_CH* pulse_ch, bool qtr_frame, bool half_frame);
        static void clock_tri(TRIG_CH& trig_ch,  bool qtr_frame, bool half_frame);
        static void clock_wno(NOISE_CH& noise_ch,  bool qtr_frame, bool half_frame);
        static void clock_dmc(DMC_CH& dmc_ch);

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

         // DMC Channel
        void set_dmc_timer(u8 data);
        void set_dmc_outl(u8 data);
        void set_dmc_smpaddr(u8 data);
        void set_dmc_smplen(u8 data);

        // APU Common Internals
        u8 get_apu_stat();
        void set_apu_stat(u8 data);
        void set_apu_fcnt(u8 data);

        // Status register
        struct {
            bool dmc_en = false;
            bool wno_en = false;
            bool tri_en = false;
            bool pul_en[2] = {false, false};
        } apu_stat;

        // Frame counter
        struct {
            u16 frame_cnt = 0x0000;
            u8 irq_latency = 0x03;
            bool step_mode = 0;
            bool irq_inb = 0;
        } apu_fcnt;

        // Audio Thread Data
        struct APU_DATA {
            bool half_frame = false;
            bool quarter_frame = false;
            bool dmc_on = true;
            bool trig_on = true;
            bool noise_on = true;
            bool pulse_on[2] = {true, true};
            PULSE_CH pulse_ch[2];
            TRIG_CH trig_ch;
            NOISE_CH noise_ch;
            DMC_CH dmc_ch;
        } apu_data;
        
        SDL_AudioDeviceID device_id;
        SDL_AudioSpec spec;
        bool mute_apu = true;

    public:
        // DMA Reference
        DMC_CH *refDMC = &apu_data.dmc_ch;
};

#endif