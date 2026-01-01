#include <apu.hpp>

#define FREQ 44100
#define SAMPLES 735
#define VOLUME 1.25f
#define QUARTER_FRAME 7457

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
		// Noise Channel
		case APUNSEN: set_noise_envl(data); break;
		case APUNSTM: set_noise_timer(data); break;
		case APUNSLC: set_noise_lcnt(data); break;
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

void APU::clock_pwm(PULSE_CH *pulse_ch, bool qtr_frame, bool half_frame)
{
	for(u8 index = 0; index < 2; index++)
	{
		// Sequencer & Timer
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

		if(qtr_frame)
		{
			// Envelope & Decay
			// if(!pulse_ch[index].const_vol && !pulse_ch[index].lc_halt)
			// {
			// 	if(pulse_ch[index].volume <= 0x00) pulse_ch[index].volume = 0x0F;
			// 	else pulse_ch[index].volume -= pulse_ch[index].decay;
			// }
		}

		if(half_frame)
		{
			// Length counter
			if(pulse_ch[index].length != 0x00 && !pulse_ch[index].lc_halt)
			{
				pulse_ch[index].length--;
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
}

void APU::clock_tri(TRIG_CH& trig_ch,  bool qtr_frame, bool half_frame)
{
	if(trig_ch.counter == 0x00)
	{
		trig_ch.counter = trig_ch.timer;

		if(trig_ch.linear_cnt != 0x00 && trig_ch.length != 0x00)
		{
			if(trig_ch.step <= 0x0F) trig_ch.sequencer--;
			else trig_ch.sequencer++;

			if(trig_ch.step == 0x1F) trig_ch.step = 0x00;
			else trig_ch.step++;
		}			
	}
	else
	{
		trig_ch.counter--;
	}

	if(qtr_frame)
	{
		// Linear Counter
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

		// Reload Prevention Control
		if(!trig_ch.lc_halt)
		{
			trig_ch.ln_set = false;
		}
    }

	if(half_frame)
	{
		// Length Counter
		if(trig_ch.length != 0x00 && !trig_ch.lc_halt)
		{
			trig_ch.length--;
		}
	}
}

void APU::clock_wno(NOISE_CH& noise_ch,  bool qtr_frame, bool half_frame)
{
	if(noise_ch.counter == 0x00)
	{
		noise_ch.counter = noise_ch.timer;
		u16 feedback = (noise_ch.lfsr & D0) ^ (noise_ch.mode ? ((noise_ch.lfsr & D6) >> 6) : ((noise_ch.lfsr & D1) >> 1));
		noise_ch.lfsr >>= 1;
		noise_ch.lfsr = (noise_ch.lfsr & 0x3FFF) | (feedback << 14);			
	}
	else
	{
		noise_ch.counter--;
	}

	if(qtr_frame)
	{

	}

	if(half_frame)
	{
		if(noise_ch.length != 0x00 && !noise_ch.lc_halt)
		{
			noise_ch.length--;
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
			APU::clock_pwm(apu_data->pulse_ch, apu_data->quarter_frame, apu_data->half_frame);
			APU::clock_tri(apu_data->trig_ch, apu_data->quarter_frame, apu_data->half_frame);
			APU::clock_wno(apu_data->noise_ch, apu_data->quarter_frame, apu_data->half_frame);
			apu_data->quarter_frame = false;
			apu_data->half_frame = false;
            cycle_accumulator -= 1.0f;
        }

        u8 pulse1 = apu_data->pulse_ch[0].length == 0x00 ? 0x00 : apu_data->pulse_ch[0].volume*(waveform[apu_data->pulse_ch[0].duty_cycle][apu_data->pulse_ch[0].sequencer]);
		u8 pulse2 = apu_data->pulse_ch[1].length == 0x00 ? 0x00 : apu_data->pulse_ch[1].volume*(waveform[apu_data->pulse_ch[1].duty_cycle][apu_data->pulse_ch[1].sequencer]);
		u8 trig = (apu_data->trig_ch.length == 0x00 || apu_data->trig_ch.linear_cnt == 0x00) ? 0x00 : apu_data->trig_ch.sequencer;
		u8 noise = apu_data->noise_ch.length == 0x00 ? 0x00 : apu_data->noise_ch.volume*(apu_data->noise_ch.lfsr & D0);
		stream[i] = (pulse1 + pulse2) + 0.00851*trig + noise;
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

void APU::set_noise_envl(u8 data)
{
	apu_data.noise_ch.lc_halt = (data & D7);
	apu_data.noise_ch.const_vol = (data & D4);
	if(apu_data.noise_ch.const_vol) apu_data.noise_ch.volume = (data & 0x0F);
	else apu_data.noise_ch.decay = (data & 0x0F);
}

void APU::set_noise_lcnt(u8 data)
{
	apu_data.noise_ch.length = apu_stat.wno_en ? APU_LC[(data & 0xF8) >> 3] : 0x00;
}

void APU::set_noise_timer(u8 data)
{
	apu_data.noise_ch.mode = (data & D7);
	apu_data.noise_ch.timer = APU_TM[(data & 0x0F)]; 
	apu_data.noise_ch.steps = (data & D7) > 0 ? 0x005D : 0x7FFF;
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
	apu_fcnt.frame_cnt = 0x0000;
}

void APU::clock_apu_fcnt()
{
	apu_fcnt.frame_cnt += 0x0001;
	// printf("FC: %d\n", apu_fcnt.frame_cnt);
	if(apu_fcnt.step_mode)
	{
		// 4-step frame mode
		if(apu_fcnt.frame_cnt == QUARTER_FRAME)
		{
			apu_data.quarter_frame = true;
		}
		else if(apu_fcnt.frame_cnt == QUARTER_FRAME*2)
		{
			apu_data.quarter_frame = true;
			apu_data.half_frame = true;
		}
		else if(apu_fcnt.frame_cnt == QUARTER_FRAME*3)
		{
			apu_data.quarter_frame = true;
		}
		else if(apu_fcnt.frame_cnt == QUARTER_FRAME*4)
		{
			// quarter & half frame with irq & reset
			apu_data.quarter_frame = true;
			apu_data.half_frame = true;
			apu_fcnt.frame_cnt = 0x0000;
			apu_fcnt.irq_flag = true;
		}
	}
	else
	{
		// 5-step frame mode
		if(apu_fcnt.frame_cnt == QUARTER_FRAME)
		{
			apu_data.quarter_frame = true;
		}
		else if(apu_fcnt.frame_cnt == QUARTER_FRAME*2)
		{
			apu_data.quarter_frame = true;
			apu_data.half_frame = true;
		}
		else if(apu_fcnt.frame_cnt == QUARTER_FRAME*3)
		{
			apu_data.quarter_frame = true;
		}
		else if(apu_fcnt.frame_cnt == QUARTER_FRAME*5)
		{
			// quarter & half frame with reset
			apu_data.quarter_frame = true;
			apu_data.half_frame = true;
			apu_fcnt.frame_cnt = 0x0000;
		}
	}
}
