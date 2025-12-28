#include <apu.hpp>

#define FREQ 44100
#define SAMPLES 735
#define VOLUME 1.25f

const u8 waveform[4][8] = 
{
    0, 1, 0, 0, 0, 0, 0, 0, // 12.5%
    0, 1, 1, 0, 0, 0, 0, 0, // 25%
    0, 1, 1, 1, 1, 0, 0, 0, // 50%
    1, 0, 0, 1, 1, 1, 1, 1  // 75% (25% negated) 
};

APU::APU() {}

APU::~APU()
{
	SDL_CloseAudioDevice(device_id);
}

u8 APU::read_from_cpu(u16 cpu_addr)
{
    switch(cpu_addr)
    {
        case APUP1DC: break;
        case APUP1SW: break;
        case APUP1TM: break;
        case APUP1LC: break;
        case APUSTAT: break;
        case APUFCNT: break;
        default: break;
    }

    return 0x00;
}

void APU::write_from_cpu(u16 cpu_addr, u8 data)
{
    switch(cpu_addr)
    {
		// Pulse 1 Channel
        case APUP1DC: set_pulse_duty(0, data); break;
        case APUP1SW: set_pulse_sweep(0, data); break;
        case APUP1TM: set_pulse_timer(0, data); break;
        case APUP1LC: set_pulse_lcnt(0, data); break;
		// Pulse 2 Channel
		case APUP2DC: set_pulse_duty(1, data); break;
        case APUP2SW: set_pulse_sweep(1, data); break;
        case APUP2TM: set_pulse_timer(1, data); break;
        case APUP2LC: set_pulse_lcnt(1, data); break;
		// Triangular Channel
		case APUTGLN: set_trig_linc(data); break;
		case APUTGTM: set_trig_timer(data); break;
		case APUTGLC: set_trig_lcnt(data); break;
		// APU Common
        case APUSTAT: set_apu_stat(data); break;
        case APUFCNT: set_apu_fcnt(data); break;
        default: break;
    }
}

void APU::init()
{
	// Setup audio
	spec.freq = FREQ;
	spec.format = AUDIO_U8;
	spec.channels = 1;
	spec.samples = SAMPLES;
	spec.userdata = &apu_data;
	spec.callback = (*APU::apu_callback);
	
	device_id = SDL_OpenAudioDevice(NULL, 0, &spec, NULL, 0);
	if (device_id < 0) 
	{  
		std::cerr << "Failed to open audio: " << SDL_GetError() << std::endl;
		return;
	}
	
	toggle_apu();
}

void APU::toggle_apu()
{
	mute_apu = !mute_apu;
	SDL_PauseAudioDevice(device_id, mute_apu); 
}

void APU::clock_pwm(PULSE_CH *pulse_ch)
{
	for(u8 index = 0; index < 2; index++)
	{
		if(pulse_ch[index].length != 0x00 && !pulse_ch[index].lc_halt)
		{
			pulse_ch[index].length--;
		}

		if(pulse_ch[index].counter == 0x00)
		{
			pulse_ch[index].counter = pulse_ch[index].timer;
			if(pulse_ch[index].sequencer == 0x00)
			{
				pulse_ch[index].sequencer = 0x07;
			}
			else
			{
				pulse_ch[index].sequencer--;
			}
		}
		else
		{
			pulse_ch[index].counter--;
		}

		// Sweep pulse wave
		if(pulse_ch[index].swp_en)
		{
			u8 amt = pulse_ch[index].timer >> pulse_ch[index].shift;
			if(!pulse_ch[index].neg_en) pulse_ch[index].timer += amt;
			else pulse_ch[index].timer += (index == 0) ? ~amt : (~amt + 1); 
		}
	}
}

void APU::clock_trig(TRIG_CH& trig_ch)
{
	if(trig_ch.length != 0x00 && !trig_ch.lc_halt)
	{
		trig_ch.length--;
	}

	if(trig_ch.ln_set)
	{
		trig_ch.linear_cnt = trig_ch.linear_set;
	}
	else
	{
		if(trig_ch.linear_cnt != 0x00)
		{
			trig_ch.linear_cnt--;
		}
	}

	if(trig_ch.linear_cnt != 0x00 && trig_ch.length != 0x00)
	{
		if(trig_ch.counter == 0x00)
		{
			trig_ch.counter = trig_ch.timer;

			if(trig_ch.step <= 0x0F) trig_ch.sequencer--;
			else trig_ch.sequencer++;

			if(trig_ch.step == 0x1F) trig_ch.step = 0x00;
			else trig_ch.step++;
		}
		else
		{
			trig_ch.counter--;
		}	
	}
}

void APU::apu_callback(void* data, u8 *stream, int len) 
{
	APU_DATA *apu_data = (APU_DATA*) data;
	double cycle_accumulator = 0.0f;
    const float cycles_per_sample = 1789773.0f / (float)FREQ;

    for (int i = 0; i < len; i++) 
    {
        cycle_accumulator += cycles_per_sample;      
        while (cycle_accumulator >= 1.0f) 
        {
			APU::clock_pwm(apu_data->pulse_ch);
			APU::clock_trig(apu_data->trig_ch);
            cycle_accumulator -= 1.0f;
        }

        u8 pulse1 = apu_data->pulse_ch[0].volume*(waveform[apu_data->pulse_ch[0].duty_cycle][apu_data->pulse_ch[0].sequencer]);
		u8 pulse2 = apu_data->pulse_ch[1].volume*(waveform[apu_data->pulse_ch[1].duty_cycle][apu_data->pulse_ch[1].sequencer]);
		u8 trig = apu_data->trig_ch.sequencer;
		stream[i] = (pulse1 + pulse2) + 0.00851*trig;
    }
}

void APU::set_pulse_duty(u8 index, u8 data)
{
	apu_data.pulse_ch[index].duty_cycle = (data & 0xC0) >> 6;
	apu_data.pulse_ch[index].lc_halt = (data & D5);
	apu_data.pulse_ch[index].const_vol = (data & D4);
	if(apu_data.pulse_ch[index].const_vol) apu_data.pulse_ch[index].volume = (data & 0x0F);
	else apu_data.pulse_ch[index].decay = (data & 0x0F);
}

void APU::set_pulse_lcnt(u8 index, u8 data)
{
	apu_data.pulse_ch[index].timer = (apu_data.pulse_ch[index].timer & 0x0F) | (static_cast<u16>(data & 0x07) << 8);
	apu_data.pulse_ch[1].length = apu_stat.pu1_en ? APU_LC[(data & 0xF8) >> 3] : 0x00;
	apu_data.pulse_ch[0].length = apu_stat.pu0_en ? APU_LC[(data & 0xF8) >> 3] : 0x00;
}

void APU::set_pulse_sweep(u8 index, u8 data)
{
	apu_data.pulse_ch[index].swp_en = (data & D7);
	apu_data.pulse_ch[index].neg_en = (data & D3);
	apu_data.pulse_ch[index].shift = (data & 0x07);
	apu_data.pulse_ch[index].period = (data & 0x70) >> 4;
}

void APU::set_pulse_timer(u8 index, u8 data)
{
	apu_data.pulse_ch[index].timer = (apu_data.pulse_ch[index].timer & 0xF0) | data ; 
}

void APU::set_trig_linc(u8 data)
{
	apu_data.trig_ch.lc_halt = (data & D7);
	apu_data.trig_ch.linear_set = (data & 0x7F);
	if(!apu_data.trig_ch.lc_halt) apu_data.trig_ch.ln_set = false;
}

void APU::set_trig_lcnt(u8 data)
{
	apu_data.trig_ch.timer = (apu_data.trig_ch.timer & 0x0F) | (static_cast<u16>(data & 0x07) << 8);
	apu_data.trig_ch.length = apu_stat.tri_en ? APU_LC[(data & 0xF8) >> 3] : 0x00;
	apu_data.trig_ch.ln_set = true;
}

void APU::set_trig_timer(u8 data)
{
	apu_data.trig_ch.timer = (apu_data.trig_ch.timer & 0xF0) | data ; 
}

void APU::set_apu_stat(u8 data)
{
	apu_stat.dmc_en = (data & D4);
	apu_stat.wno_en = (data & D3);
	apu_stat.tri_en = (data & D2);
	apu_stat.pu1_en = (data & D1);
	apu_stat.pu0_en = (data & D0);
	apu_data.pulse_ch[1].lc_halt = !apu_stat.pu1_en;
	apu_data.pulse_ch[0].lc_halt = !apu_stat.pu0_en;
}

void APU::set_apu_fcnt(u8 data)
{
	apu_fcnt.step_mode = (data & D7);
	apu_fcnt.irq_flag = (data & D6);
}
