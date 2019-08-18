/* Lightcyber - 64k Intro by Team210 at Evoke 2k19
 * Copyright (C) 2019 Alexander Kraus <nr4@z10.info>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

// #define DEBUG // Shader debug i/o
// #define MIDI // APC40 mkII controls

const char *demoname = "Lightcyber/Team210";
unsigned int muted = 0.;

int _fltused = 0;

// Standard library and CRT rewrite for saving executable size
void *memset(void *ptr, int value, size_t num)
{
    for(int i=num-1; i>=0; i--)
        ((unsigned char *)ptr)[i] = value;
    return ptr;
}

size_t strlen(const char *str)
{
    int len = 0;
    while(str[len] != '\0') ++len;
    return len;
}

#include "common.h"

#ifdef MIDI

int btns = 1;
void select_button(int index)
{
    for(int i=0; i<40; ++i)
    {
        DWORD out_msg = 0x8 << 4 | i << 8 | 0 << 16;
        midiOutShortMsg(hMidiOut, out_msg);
    }

    if(index < 40)
    {
        override_index = index+1;
        scene_override = 1;
    }
    
    DWORD out_msg = 0x9 << 4 | index << 8 | btns << 16;
    btns = 1+(btns+1)%125;
    midiOutShortMsg(hMidiOut, out_msg);
}

#define NOTE_OFF 0x8
#define NOTE_ON 0x9
#define CONTROL_CHANGE 0xB

#define TIME_DIAL 0x14
#define TIME_FINE_DIAL 0x15
#define TIME_VERYFINE_DIAL 0x16

void CALLBACK MidiInProc_apc40mk2(HMIDIIN hMidiIn, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
    if(wMsg == MIM_DATA)
    {
        BYTE b1 = (dwParam1 >> 24) & 0xFF,
            b2 = (dwParam1 >> 16) & 0xFF,
            b3 = (dwParam1 >> 8) & 0xFF,
            b4 = dwParam1 & 0xFF;
        BYTE b3lo = b3 & 0xF,
            b3hi = (b3 >> 4) & 0xF,
            b4lo = b4 & 0xF,
            b4hi = (b4 >> 4) & 0xF;
        
        BYTE channel = b4lo,
            button = b3;
            
        if(b4hi == NOTE_ON)
        {
            paused = 0;
            
            waveOutReset(hWaveOut);
            select_button(button);
            
            if(button == 0x0)
            {
                header.lpData = smusic1;
                header.dwBufferLength = 4 * music1_size;
                
            }
            else if(button == 0x1)
            {
                int delta = 5. * (double)sample_rate;
                header.lpData = smusic1+delta;
                header.dwBufferLength = 4 * (music1_size-delta);
            }
            else if(button == 0x2)
            {
                int delta = 49.655 * (double)sample_rate;
                header.lpData = smusic1+delta;
                header.dwBufferLength = 4 * (music1_size-delta);
            }
            else if(button == 0x3)
            {
                int delta = 82.76 * (double)sample_rate;
                header.lpData = smusic1+delta;
                header.dwBufferLength = 4 * (music1_size-delta);
            }
            else if(button == 0x4)
            {
                int delta = 99.31 * (double)sample_rate;
                header.lpData = smusic1+delta;
                header.dwBufferLength = 4 * (music1_size-delta);
            }
            else if(button == 0x5)
            {
                int delta = 112. * (double)sample_rate;
                header.lpData = smusic1+delta;
                header.dwBufferLength = 4 * (music1_size-delta);
            }
            
            waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
            waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
            waveOutRestart(hWaveOut);
            
        }
        else if(b4hi == NOTE_OFF)
        {
            select_button(button);
            
            // Logo 210
            if(button == 0x59)
            {
                char data[40] = 
                {
                    1,  1,  0,  1,  0,  1,  1,  0,
                    12, 12, 1,  1,  1,  12, 12, 1,
                    0,  0,  1,  1,  1,  0,  0,  1,
                    0,  0,  1,  1,  1,  0,  0,  1,
                    1,  1,  12, 1,  12, 1,  1,  12
                };

                for(int i=0; i<40; ++i)
                {
                    
                    DWORD out_msg;
                    if(data[i] == 0) 
                    {
                        out_msg = 0x8 << 4 | i << 8 | 0 << 16;
                    }
                    else
                    {
                        out_msg = 0x9 << 4 | i << 8 | 1+(data[i]+btns) %125 << 16;
                    }
                    midiOutShortMsg(hMidiOut, out_msg);
                }
                btns = 1+(btns+1)%125;
            }
            // Kewlers Logo
            else if(button == 0x57)
            {
                char data[40] = 
                {
                    3,3,3,3,1,1,1,1,
                    3,3,3,3,1,1,1,1,
                    3,3,3,0,0,1,1,1,
                    0,3,0,0,0,0,1,0,
                    0,7,0,0,0,0,9,0
                };

                for(int i=0; i<40; ++i)
                {
                    
                    DWORD out_msg;
                    if(data[i] == 0) 
                    {
                        out_msg = 0x8 << 4 | i << 8 | 0 << 16;
                    }
                    else
                    {
                        out_msg = 0x9 << 4 | i << 8 | 1+(data[i]+btns) %125 << 16;
                    }
                    midiOutShortMsg(hMidiOut, out_msg);
                }
                btns = 1+(btns+1)%125;
            }
        }
        else if(b4hi == CONTROL_CHANGE)// Channel select
        {
            if(button == TIME_DIAL)
            {
                waveOutReset(hWaveOut);
                time_dial = (double)b2/(double)0x7F;
                
                int delta = (.9*time_dial+.09*time_fine_dial+.01*time_very_fine_dial) * t_end * (double)sample_rate;
                header.lpData = min(max(smusic1, smusic1+delta), smusic1+music1_size);
                header.dwBufferLength = 4 * (music1_size-delta);
                waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
                waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
                waveOutPause(hWaveOut);
                paused = 1;
            }
            else if(button == TIME_FINE_DIAL)
            {
                waveOutReset(hWaveOut);
                time_fine_dial = (double)b2/(double)0x7F;
                
                int delta = (.9*time_dial+.09*time_fine_dial+.01*time_very_fine_dial) * t_end * (double)sample_rate;
                header.lpData = min(max(smusic1, smusic1+delta), smusic1+music1_size);
                header.dwBufferLength = 4 * (music1_size-delta);
                waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
                waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
                waveOutPause(hWaveOut);
                paused = 1;
            }
            else if(button == TIME_VERYFINE_DIAL)
            {
                waveOutReset(hWaveOut);
                time_very_fine_dial = (double)b2/(double)0x7F;
                
                int delta = (.9*time_dial+.09*time_fine_dial+.01*time_very_fine_dial) * t_end * (double)sample_rate;
                header.lpData = min(max(smusic1, smusic1+delta), smusic1+music1_size);
                header.dwBufferLength = 4 * (music1_size-delta);
                waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
                waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
                waveOutPause(hWaveOut);
                paused = 1;
            }
            else
            {
                if(channel == 0 && button == 0x07) fader0 = (double)b2/(double)0x7F;
                else if(channel == 1 && button == 0x07) fader1 = (double)b2/(double)0x7F;
                else if(channel == 2 && button == 0x07) fader2 = (double)b2/(double)0x7F;
                else if(channel == 3 && button == 0x07) fader3 = (double)b2/(double)0x7F;
                else if(channel == 4 && button == 0x07) fader4 = (double)b2/(double)0x7F;
                else if(channel == 5 && button == 0x07) fader5 = (double)b2/(double)0x7F;
                else if(channel == 6 && button == 0x07) fader6 = (double)b2/(double)0x7F;
                else if(channel == 7 && button == 0x07) fader7 = (double)b2/(double)0x7F;
            }
        }

        draw();
        
        printf("wMsg=MIM_DATA, dwParam1=%08x, byte=%02x %02x h_%01x l_%01x %02x, dwParam2=%08x\n", dwParam1, b1, b2, b3hi, b3lo, b4, dwParam2);
    }
    
	return;
}
#endif

void load_demo()
{
	// Load loading bar shader
	printf("++++ Creating Loading bar.\n");
	int load_size = strlen(load_frag);
	load_handle = glCreateShader(GL_FRAGMENT_SHADER);
	load_program = glCreateProgram();
	glShaderSource(load_handle, 1, (GLchar **)&load_frag, &load_size);
	glCompileShader(load_handle);
	printf("---> Load shader:\n");
#ifdef DEBUG
	debug(load_handle);
#endif
	glAttachShader(load_program, load_handle);
	glLinkProgram(load_program);
	printf("---> Load Program:\n");
#ifdef DEBUG
	debugp(load_program);
#endif
	glUseProgram(load_program);
	load_progress_location = glGetUniformLocation(load_program, LOAD_VAR_IPROGRESS);
	load_time_location = glGetUniformLocation(load_program, LOAD_VAR_ITIME);
	load_resolution_location = glGetUniformLocation(load_program, LOAD_VAR_IRESOLUTION);
	printf("++++ Loading bar created.\n");

	// Create framebuffer for rendering first pass to
	glGenFramebuffers(1, &first_pass_framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, first_pass_framebuffer);
	glGenTextures(1, &first_pass_texture);
	glBindTexture(GL_TEXTURE_2D, first_pass_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, first_pass_texture, 0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	// Generate music framebuffer
	// Initialize sequence texture
	printf("sequence texture width is: %d\n", sequence_texture_size); // TODO: remove
	glGenTextures(1, &sequence_texture_handle);
	glBindTexture(GL_TEXTURE_2D, sequence_texture_handle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sequence_texture_size, sequence_texture_size, 0, GL_RGBA, GL_UNSIGNED_BYTE, sequence_texture);

	glGenFramebuffers(1, &snd_framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, snd_framebuffer);
	glPixelStorei(GL_PACK_ALIGNMENT, 4);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	unsigned int snd_texture;
	glGenTextures(1, &snd_texture);
	glBindTexture(GL_TEXTURE_2D, snd_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texs, texs, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, snd_texture, 0);

	// Music allocs
	nblocks1 = sample_rate * duration1 / block_size + 1;
	music1_size = nblocks1 * block_size;
	smusic1 = (float*)malloc(4 * music1_size);
	short *dest = (short*)smusic1;
	for (int i = 0; i < 2 * music1_size; ++i)
		dest[i] = 0;

	updateBar();

	// Load music shader
	LoadMusicThread(0);
	updateBar();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	LoadSymbols();
	LoadPrograms();

	LoadTextThread(0);
	updateBar();

	for (int music_block = 0; music_block < nblocks1; ++music_block)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, snd_framebuffer);
		glUseProgram(sfx_program);

		printf("Rendering SFX block %d/%d -> %le\n", music_block, nblocks1, .5*(float)music_block / (float)nblocks1);
		double tstart = (double)(music_block*block_size);

		glViewport(0, 0, texs, texs);

		glUniform1f(sfx_volumelocation, 1.);
		glUniform1f(sfx_samplerate_location, (float)sample_rate);
		glUniform1f(sfx_blockoffset_location, (float)tstart);
		glUniform1i(sfx_texs_location, texs);
		glUniform1i(sfx_sequence_texture_location, 0);
		glUniform1f(sfx_sequence_width_location, sequence_texture_size);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, sequence_texture_handle);

		quad();

		glReadPixels(0, 0, texs, texs, GL_RGBA, GL_UNSIGNED_BYTE, smusic1 + music_block * block_size);
		glFlush();

		unsigned short *buf = (unsigned short*)smusic1;
		short *dest = (short*)smusic1;
		if (!muted)
			for (int j = 2 * music_block*block_size; j < 2 * (music_block + 1)*block_size; ++j)
				dest[j] = (buf[j] - (1 << 15));
		else
			for (int j = 2 * music_block*block_size; j < 2 * (music_block + 1)*block_size; ++j)
				dest[j] = 0.;

        progress += .5/nblocks1;
		updateBar();
	}

	glUseProgram(0);

	initialize_sound();
    
#ifdef MIDI
    UINT nMidiDeviceNum;
    MIDIINCAPS caps;
    
	nMidiDeviceNum = midiInGetNumDevs();
	if(nMidiDeviceNum == 0) 
    {
        printf("No MIDI input devices connected.\n");
    }
    else
    {
        printf("Available MIDI devices:\n");
        for (unsigned int i = 0; i < nMidiDeviceNum; ++i) 
        {
            midiInGetDevCaps(i, &caps, sizeof(MIDIINCAPS));
            printf("->%d: %s ", i, caps.szPname);
            
            if(!strcmp("APC40 mkII", caps.szPname))
            {
                HMIDIIN hMidiDevice;
                MMRESULT rv = midiInOpen(&hMidiDevice, i, (DWORD)(void*)MidiInProc_apc40mk2, 0, CALLBACK_FUNCTION);
                midiInStart(hMidiDevice);
                
                printf(" >> opened.\n");
            }
            else
            {
                printf("(Unsupported MIDI controller).\n");
            }
        }
    }
    
    MIDIOUTCAPS ocaps;
    nMidiDeviceNum = midiOutGetNumDevs();

    if(nMidiDeviceNum == 0) 
    {
        printf("No MIDI output devices connected.\n");
    }
    else
    {
        printf("Available MIDI devices:\n");
        for (unsigned int i = 0; i < nMidiDeviceNum; ++i) 
        {
            midiOutGetDevCaps(i, &ocaps, sizeof(MIDIOUTCAPS));
            printf("->%d: %s ", i, ocaps.szPname);
            
            if(!strcmp("APC40 mkII", ocaps.szPname))
            {
                MMRESULT rv = midiOutOpen (&hMidiOut, i, 0, 0, CALLBACK_NULL);
            }
            else
            {
                printf("(Unsupported MIDI controller).\n");
            }
        }
    }
    
#endif
}

unsigned long __stdcall LoadMusicThread( void *lpParam)
{
    int sfx_size = strlen(sfx_frag);
    sfx_handle = glCreateShader(GL_FRAGMENT_SHADER);
    sfx_program = glCreateProgram();
    glShaderSource(sfx_handle, 1, (GLchar **)&sfx_frag, &sfx_size);
    glCompileShader(sfx_handle);
    printf("---> SFX shader:\n");
#ifdef DEBUG
    debug(sfx_handle);
#endif
    glAttachShader(sfx_program, sfx_handle);
    glLinkProgram(sfx_program);
    printf("---> SFX program:\n");
#ifdef DEBUG
    debugp(sfx_program);
#endif
    glUseProgram(sfx_program);
    sfx_samplerate_location = glGetUniformLocation(sfx_program, SFX_VAR_ISAMPLERATE);
    sfx_blockoffset_location = glGetUniformLocation(sfx_program, SFX_VAR_IBLOCKOFFSET);
    sfx_volumelocation = glGetUniformLocation(sfx_program, SFX_VAR_IVOLUME);
    sfx_texs_location = glGetUniformLocation(sfx_program, SFX_VAR_ITEXSIZE);
    sfx_sequence_texture_location = glGetUniformLocation(sfx_program, SFX_VAR_ISEQUENCE);
    sfx_sequence_width_location = glGetUniformLocation(sfx_program, SFX_VAR_ISEQUENCEWIDTH);
    printf("++++ SFX shader created.\n");
    
    music_loading = 1;
    progress += .1/NSHADERS; 
    
    return 0;
}

unsigned long __stdcall LoadTextThread(void * lpParam)
{
    // Initialize font texture
    printf("font texture width is: %d\n", font_texture_size); // TODO: remove
    glGenTextures(1, &font_texture_handle);
    glBindTexture(GL_TEXTURE_2D, font_texture_handle);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, font_texture_size, font_texture_size, 0, GL_RGBA, GL_UNSIGNED_BYTE, font_texture);
    
    progress += .1/NSHADERS;
    
    return 0;
}

void quad()
{
    glBegin(GL_QUADS);
    glVertex3f(-1,-1,0);
    glVertex3f(-1,1,0);
    glVertex3f(1,1,0);
    glVertex3f(1,-1,0);
    glEnd();
    glFlush();
}

void updateBar()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    // Render first pass
    glViewport(0,0,w,h);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glUseProgram(load_program);
    glUniform2f(load_resolution_location, w, h);
    
    glUniform1f(load_progress_location, progress);
    
    quad();
    
	flip_buffers();
    
    glUseProgram(0);
}

// Pure opengl drawing code, essentially cross-platform
void draw()
{
    glBindFramebuffer(GL_FRAMEBUFFER, first_pass_framebuffer);
    
    t = t_now;
    if(t > t_end)
        ExitProcess(0);
    
#ifdef MIDI
    if(time_dial != 0 ||  time_fine_dial != 0 || time_very_fine_dial != 0)
    {
        t = t_now + (.9*time_dial+.09*time_fine_dial+.01*time_very_fine_dial) * t_end;
        scene_override = 0;
    }
#endif
    
    if(scene_override)
    {
        if(override_index == 1)
        {
            t = t_now;
        }
        else if(override_index == 2)
        {
            t = t_now + 11.;
        }
        else if(override_index == 3) // graffiti
        {
            t = t_now + 23.;
        }
        else if(override_index == 4) // cube
        {
            t = t_now + 39.;
        }
        else if(override_index == 5) // honey hell
        {
            t = t_now + 55.;
        }
        else if(override_index == 6) // ... also graffiti
        {
            t = t_now + 71.;
        }
        else if(override_index == 7) // tentacle
        {
            t = t_now + 86.5;
        }
        else if(override_index == 8) // bubbles
        {
            t = t_now + 103.;
        }
        else if(override_index == 9) // smoke
        {
            t = t_now + 118.;
        }
        else if(override_index == 10)
        {
            t = t_now + 133.;
        }
        else if(override_index == 11)
        {
            t = t_now + 141.;
        }
        else if(override_index == 12)
        {
            t = t_now + 156.;
        }
        else if(override_index == 13)
        {
            t = t_now + 156.+1.*1.8182;
        }
        else if(override_index == 14)
        {
            t = t_now + 156.+2.*1.8182;
        }
        else if(override_index == 15)
        {
            t = t_now + 156.+3.*1.8182;
        }
        else if(override_index == 16)
        {
            t = t_now + 156.+3.5*1.8182;
        }
        else if(override_index == 17)
        {
            t = t_now + 156.+4.*1.8182;
        }
        else if(override_index == 18)
        {
            t = t_now + 156.+5.*1.8182;
        }
        else if(override_index == 19)
        {
            t = t_now + 156.+6.*1.8182;
        }
        else if(override_index == 20)
        {
            t = t_now + 156.+8.*1.8182;
        }
        else if(override_index == 21)
        {
            t = t_now + 156.+9.*1.8182;
        }
        else if(override_index == 22)
        {
            t = t_now + 156.+10.*1.8182;
        }
        else if(override_index == 23)
        {
            t = t_now + 156.+11.*1.8182;
        }
        else if(override_index == 24)
        {
            t = t_now + 156.+11.5*1.8182;
        }
        else if(override_index == 25)
        {
            t = t_now + 156.+12.*1.8182;
        }
        else if(override_index == 26)
        {
            t = t_now + 156.+13.*1.8182;
        }
        else if(override_index == 27)
        {
            t = t_now + 156.+14.*1.8182;
        }
    }
    
    if(t < 11.)
    {
        glUseProgram(logo210_program);
        glUniform1f(logo210_iTime_location, t);
        glUniform2f(logo210_iResolution_location, w, h);
        
#ifdef MIDI
        
        glUniform1f(logo210_iFader0_location, fader0);
        glUniform1f(logo210_iFader1_location, fader1);
        glUniform1f(logo210_iFader2_location, fader2);
        glUniform1f(logo210_iFader3_location, fader3);
        glUniform1f(logo210_iFader4_location, fader4);
        glUniform1f(logo210_iFader5_location, fader5);
        glUniform1f(logo210_iFader6_location, fader6);
        glUniform1f(logo210_iFader7_location, fader7);

        if(override_index == 0)
        {
            select_button(override_index);
            scene_override = 0;
        }
#endif
    }
    else if(t < 23)
    {
        glUseProgram(evoke_program);
        glUniform1f(evoke_iTime_location, t-11);
        glUniform2f(evoke_iResolution_location, w, h);
        
#ifdef MIDI
        glUniform1f(evoke_iFader0_location, fader0);
        glUniform1f(evoke_iFader1_location, fader1);
        glUniform1f(evoke_iFader2_location, fader2);
        glUniform1f(evoke_iFader3_location, fader3);
        glUniform1f(evoke_iFader4_location, fader4);
        glUniform1f(evoke_iFader5_location, fader5);
        glUniform1f(evoke_iFader6_location, fader6);
        glUniform1f(evoke_iFader7_location, fader7);
        
        if(override_index == 1)
        {
            select_button(override_index);
            scene_override = 0;
        }
#endif
    }
    else if(t < 39)
    {
        glUseProgram(graffiti_program);
        glUniform1f(graffiti_iTime_location, t-23);
        glUniform2f(graffiti_iResolution_location, w, h);
        
#ifdef MIDI
        glUniform1f(graffiti_iFader0_location, fader0);
        glUniform1f(graffiti_iFader1_location, fader1);
        glUniform1f(graffiti_iFader2_location, fader2);
        glUniform1f(graffiti_iFader3_location, fader3);
        glUniform1f(graffiti_iFader4_location, fader4);
        glUniform1f(graffiti_iFader5_location, fader5);
        glUniform1f(graffiti_iFader6_location, fader6);
        glUniform1f(graffiti_iFader7_location, fader7);
        
        if(override_index == 2)
        {
            select_button(override_index);
            scene_override = 0;
        }
#endif
    }
    else if(t < 55)
    {
        glUseProgram(groundboxes_program);
        glUniform1f(groundboxes_iTime_location, t-39);
        glUniform2f(groundboxes_iResolution_location, w, h);
        
#ifdef MIDI
        glUniform1f(groundboxes_iFader0_location, fader0);
        glUniform1f(groundboxes_iFader1_location, fader1);
        glUniform1f(groundboxes_iFader2_location, fader2);
        glUniform1f(groundboxes_iFader3_location, fader3);
        glUniform1f(groundboxes_iFader4_location, fader4);
        glUniform1f(groundboxes_iFader5_location, fader5);
        glUniform1f(groundboxes_iFader6_location, fader6);
        glUniform1f(groundboxes_iFader7_location, fader7);
        
        if(override_index == 3) 
        {
            select_button(override_index);
            scene_override = 0;
        }
#endif
    }
    else if(t < 71)
    {
        glUseProgram(canal_program);
        glUniform1f(canal_iTime_location, t-55.);
        glUniform2f(canal_iResolution_location, w, h);
        
#ifdef MIDI
        glUniform1f(canal_iFader0_location, fader0);
        glUniform1f(canal_iFader1_location, fader1);
        glUniform1f(canal_iFader2_location, fader2);
        glUniform1f(canal_iFader3_location, fader3);
        glUniform1f(canal_iFader4_location, fader4);
        glUniform1f(canal_iFader5_location, fader5);
        glUniform1f(canal_iFader6_location, fader6);
        glUniform1f(canal_iFader7_location, fader7);
        
        if(override_index == 4)
        {
            select_button(override_index);
            scene_override = 0;
        }
#endif
    }
    else if(t < 86.5)
    {
        glUseProgram(graffiti_program);
        glUniform1f(graffiti_iTime_location, t);
        glUniform2f(graffiti_iResolution_location, w, h);
        
#ifdef MIDI
        glUniform1f(graffiti_iFader0_location, fader0);
        glUniform1f(graffiti_iFader1_location, fader1);
        glUniform1f(graffiti_iFader2_location, fader2);
        glUniform1f(graffiti_iFader3_location, fader3);
        glUniform1f(graffiti_iFader4_location, fader4);
        glUniform1f(graffiti_iFader5_location, fader5);
        glUniform1f(graffiti_iFader6_location, fader6);
        glUniform1f(graffiti_iFader7_location, fader7);
        
        if(override_index == 5)
        {
            select_button(override_index);
            scene_override = 0;
        }
#endif
    }
    else if(t < 103)
    {
        glUseProgram(voronoidesign_program);
        glUniform1f(voronoidesign_iTime_location, t-86.5);
        glUniform2f(voronoidesign_iResolution_location, w, h);
      
#ifdef MIDI
        glUniform1f(voronoidesign_iFader0_location, fader0);
        glUniform1f(voronoidesign_iFader1_location, fader1);
        glUniform1f(voronoidesign_iFader2_location, fader2);
        glUniform1f(voronoidesign_iFader3_location, fader3);
        glUniform1f(voronoidesign_iFader4_location, fader4);
        glUniform1f(voronoidesign_iFader5_location, fader5);
        glUniform1f(voronoidesign_iFader6_location, fader6);
        glUniform1f(voronoidesign_iFader7_location, fader7);
        
        if(override_index == 6) 
        {
            select_button(override_index);
            scene_override = 0;
        }
#endif
    }
    else if(t < 118.)
    {
        glUseProgram(transbubbles_program);
        glUniform1f(transbubbles_iTime_location, t-103);
        glUniform2f(transbubbles_iResolution_location, w, h);
      
#ifdef MIDI
        glUniform1f(transbubbles_iFader0_location, fader0);
        glUniform1f(transbubbles_iFader1_location, fader1);
        glUniform1f(transbubbles_iFader2_location, fader2);
        glUniform1f(transbubbles_iFader3_location, fader3);
        glUniform1f(transbubbles_iFader4_location, fader4);
        glUniform1f(transbubbles_iFader5_location, fader5);
        glUniform1f(transbubbles_iFader6_location, fader6);
        glUniform1f(transbubbles_iFader7_location, fader7);
        
        if(override_index == 7) 
        {
            select_button(override_index);
            scene_override = 0;
        }
#endif
    }
    else if(t < 133.)
    {
        glUseProgram(volclouds_program);
        glUniform1f(volclouds_iTime_location, t-118);
        glUniform2f(volclouds_iResolution_location, w, h);
      
#ifdef MIDI
        glUniform1f(volclouds_iFader0_location, fader0);
        glUniform1f(volclouds_iFader1_location, fader1);
        glUniform1f(volclouds_iFader2_location, fader2);
        glUniform1f(volclouds_iFader3_location, fader3);
        glUniform1f(volclouds_iFader4_location, fader4);
        glUniform1f(volclouds_iFader5_location, fader5);
        glUniform1f(volclouds_iFader6_location, fader6);
        glUniform1f(volclouds_iFader7_location, fader7);
        
        if(override_index == 8) 
        {
            select_button(override_index);
            scene_override = 0;
        }
#endif
    }
    else if(t < 141.)
    {
        glUseProgram(chart_program);
        glUniform1f(chart_iTime_location, t-133);
        glUniform2f(chart_iResolution_location, w, h);
      
#ifdef MIDI
        glUniform1f(chart_iFader0_location, fader0);
        glUniform1f(chart_iFader1_location, fader1);
        glUniform1f(chart_iFader2_location, fader2);
        glUniform1f(chart_iFader3_location, fader3);
        glUniform1f(chart_iFader4_location, fader4);
        glUniform1f(chart_iFader5_location, fader5);
        glUniform1f(chart_iFader6_location, fader6);
        glUniform1f(chart_iFader7_location, fader7);
        
        if(override_index == 9) 
        {
            select_button(override_index);
            scene_override = 0;
        }
#endif
    }
    else if(t < 156.)
    {
        glUseProgram(greet_program);
        glUniform1f(greet_iTime_location, t-141);
        glUniform2f(greet_iResolution_location, w, h);
      
#ifdef MIDI
        glUniform1f(greet_iFader0_location, fader0);
        glUniform1f(greet_iFader1_location, fader1);
        glUniform1f(greet_iFader2_location, fader2);
        glUniform1f(greet_iFader3_location, fader3);
        glUniform1f(greet_iFader4_location, fader4);
        glUniform1f(greet_iFader5_location, fader5);
        glUniform1f(greet_iFader6_location, fader6);
        glUniform1f(greet_iFader7_location, fader7);
        
        if(override_index == 10) 
        {
            select_button(override_index);
            scene_override = 0;
        }
#endif
    }
    // FIXME hier geballer, taktlänge 1.8182
    else if(t < 156.+1.*1.8182)
    {
        glUseProgram(volclouds_program);
        glUniform1f(volclouds_iTime_location, t);
        glUniform2f(volclouds_iResolution_location, w, h);
      
#ifdef MIDI
        glUniform1f(volclouds_iFader0_location, fader0);
        glUniform1f(volclouds_iFader1_location, fader1);
        glUniform1f(volclouds_iFader2_location, fader2);
        glUniform1f(volclouds_iFader3_location, fader3);
        glUniform1f(volclouds_iFader4_location, fader4);
        glUniform1f(volclouds_iFader5_location, fader5);
        glUniform1f(volclouds_iFader6_location, fader6);
        glUniform1f(volclouds_iFader7_location, fader7);
        
        if(override_index == 11) 
        {
            select_button(override_index);
            scene_override = 0;
        }
#endif
    }
    else if(t < 156.+2.*1.8182)
    {
        glUseProgram(graffiti_program);
        glUniform1f(graffiti_iTime_location, t);
        glUniform2f(graffiti_iResolution_location, w, h);
      
#ifdef MIDI
        glUniform1f(graffiti_iFader0_location, fader0);
        glUniform1f(graffiti_iFader1_location, fader1);
        glUniform1f(graffiti_iFader2_location, fader2);
        glUniform1f(graffiti_iFader3_location, fader3);
        glUniform1f(graffiti_iFader4_location, fader4);
        glUniform1f(graffiti_iFader5_location, fader5);
        glUniform1f(graffiti_iFader6_location, fader6);
        glUniform1f(graffiti_iFader7_location, fader7);
        
        if(override_index == 12) 
        {
            select_button(override_index);
            scene_override = 0;
        }
#endif
    }
    else if(t < 156.+3.*1.8182)
    {
        glUseProgram(transbubbles_program);
        glUniform1f(transbubbles_iTime_location, t);
        glUniform2f(transbubbles_iResolution_location, w, h);
      
#ifdef MIDI
        glUniform1f(transbubbles_iFader0_location, fader0);
        glUniform1f(transbubbles_iFader1_location, fader1);
        glUniform1f(transbubbles_iFader2_location, fader2);
        glUniform1f(transbubbles_iFader3_location, fader3);
        glUniform1f(transbubbles_iFader4_location, fader4);
        glUniform1f(transbubbles_iFader5_location, fader5);
        glUniform1f(transbubbles_iFader6_location, fader6);
        glUniform1f(transbubbles_iFader7_location, fader7);
        
        if(override_index == 13) 
        {
            select_button(override_index);
            scene_override = 0;
        }
#endif
    }
    else if(t < 156.+3.5*1.8182)
    {
        glUseProgram(voronoidesign_program);
        glUniform1f(voronoidesign_iTime_location, t);
        glUniform2f(voronoidesign_iResolution_location, w, h);
      
#ifdef MIDI
        glUniform1f(voronoidesign_iFader0_location, fader0);
        glUniform1f(voronoidesign_iFader1_location, fader1);
        glUniform1f(voronoidesign_iFader2_location, fader2);
        glUniform1f(voronoidesign_iFader3_location, fader3);
        glUniform1f(voronoidesign_iFader4_location, fader4);
        glUniform1f(voronoidesign_iFader5_location, fader5);
        glUniform1f(voronoidesign_iFader6_location, fader6);
        glUniform1f(voronoidesign_iFader7_location, fader7);
        
        if(override_index == 14) 
        {
            select_button(override_index);
            scene_override = 0;
        }
#endif
    }
    else if(t < 156.+4.*1.8182)
    {
        glUseProgram(groundboxes_program);
        glUniform1f(groundboxes_iTime_location, t);
        glUniform2f(groundboxes_iResolution_location, w, h);
      
#ifdef MIDI
        glUniform1f(groundboxes_iFader0_location, fader0);
        glUniform1f(groundboxes_iFader1_location, fader1);
        glUniform1f(groundboxes_iFader2_location, fader2);
        glUniform1f(groundboxes_iFader3_location, fader3);
        glUniform1f(groundboxes_iFader4_location, fader4);
        glUniform1f(groundboxes_iFader5_location, fader5);
        glUniform1f(groundboxes_iFader6_location, fader6);
        glUniform1f(groundboxes_iFader7_location, fader7);
        
        if(override_index == 15) 
        {
            select_button(override_index);
            scene_override = 0;
        }
#endif
    }
    else if(t < 156.+5.*1.8182)
    {
        glUseProgram(volclouds_program);
        glUniform1f(volclouds_iTime_location, t);
        glUniform2f(volclouds_iResolution_location, w, h);
      
#ifdef MIDI
        glUniform1f(volclouds_iFader0_location, fader0);
        glUniform1f(volclouds_iFader1_location, fader1);
        glUniform1f(volclouds_iFader2_location, fader2);
        glUniform1f(volclouds_iFader3_location, fader3);
        glUniform1f(volclouds_iFader4_location, fader4);
        glUniform1f(volclouds_iFader5_location, fader5);
        glUniform1f(volclouds_iFader6_location, fader6);
        glUniform1f(volclouds_iFader7_location, fader7);
        
        if(override_index == 16) 
        {
            select_button(override_index);
            scene_override = 0;
        }
#endif
    }
    else if(t < 156.+6.*1.8182)
    {
        glUseProgram(transbubbles_program);
        glUniform1f(transbubbles_iTime_location, t);
        glUniform2f(transbubbles_iResolution_location, w, h);
      
#ifdef MIDI
        glUniform1f(transbubbles_iFader0_location, fader0);
        glUniform1f(transbubbles_iFader1_location, fader1);
        glUniform1f(transbubbles_iFader2_location, fader2);
        glUniform1f(transbubbles_iFader3_location, fader3);
        glUniform1f(transbubbles_iFader4_location, fader4);
        glUniform1f(transbubbles_iFader5_location, fader5);
        glUniform1f(transbubbles_iFader6_location, fader6);
        glUniform1f(transbubbles_iFader7_location, fader7);
        
        if(override_index == 17) 
        {
            select_button(override_index);
            scene_override = 0;
        }
#endif
    }
    else if(t < 156.+8.*1.8182)
    {
        glUseProgram(canal_program);
        glUniform1f(canal_iTime_location, t);
        glUniform2f(canal_iResolution_location, w, h);
      
#ifdef MIDI
        glUniform1f(canal_iFader0_location, fader0);
        glUniform1f(canal_iFader1_location, fader1);
        glUniform1f(canal_iFader2_location, fader2);
        glUniform1f(canal_iFader3_location, fader3);
        glUniform1f(canal_iFader4_location, fader4);
        glUniform1f(canal_iFader5_location, fader5);
        glUniform1f(canal_iFader6_location, fader6);
        glUniform1f(canal_iFader7_location, fader7);
        
        if(override_index == 18) 
        {
            select_button(override_index);
            scene_override = 0;
        }
#endif
    }
    else if(t < 156.+9.*1.8182)
    {
        glUseProgram(groundboxes_program);
        glUniform1f(groundboxes_iTime_location, t);
        glUniform2f(groundboxes_iResolution_location, w, h);
      
#ifdef MIDI
        glUniform1f(groundboxes_iFader0_location, fader0);
        glUniform1f(groundboxes_iFader1_location, fader1);
        glUniform1f(groundboxes_iFader2_location, fader2);
        glUniform1f(groundboxes_iFader3_location, fader3);
        glUniform1f(groundboxes_iFader4_location, fader4);
        glUniform1f(groundboxes_iFader5_location, fader5);
        glUniform1f(groundboxes_iFader6_location, fader6);
        glUniform1f(groundboxes_iFader7_location, fader7);
        
        if(override_index == 19) 
        {
            select_button(override_index);
            scene_override = 0;
        }
#endif
    }
    else if(t < 156.+10.*1.8182)
    {
        glUseProgram(voronoidesign_program);
        glUniform1f(voronoidesign_iTime_location, t);
        glUniform2f(voronoidesign_iResolution_location, w, h);
      
#ifdef MIDI
        glUniform1f(voronoidesign_iFader0_location, fader0);
        glUniform1f(voronoidesign_iFader1_location, fader1);
        glUniform1f(voronoidesign_iFader2_location, fader2);
        glUniform1f(voronoidesign_iFader3_location, fader3);
        glUniform1f(voronoidesign_iFader4_location, fader4);
        glUniform1f(voronoidesign_iFader5_location, fader5);
        glUniform1f(voronoidesign_iFader6_location, fader6);
        glUniform1f(voronoidesign_iFader7_location, fader7);
        
        if(override_index == 20) 
        {
            select_button(override_index);
            scene_override = 0;
        }
#endif
    }
    else if(t < 156.+11.*1.8182)
    {
        glUseProgram(canal_program);
        glUniform1f(canal_iTime_location, t);
        glUniform2f(canal_iResolution_location, w, h);
      
#ifdef MIDI
        glUniform1f(canal_iFader0_location, fader0);
        glUniform1f(canal_iFader1_location, fader1);
        glUniform1f(canal_iFader2_location, fader2);
        glUniform1f(canal_iFader3_location, fader3);
        glUniform1f(canal_iFader4_location, fader4);
        glUniform1f(canal_iFader5_location, fader5);
        glUniform1f(canal_iFader6_location, fader6);
        glUniform1f(canal_iFader7_location, fader7);
        
        if(override_index == 21) 
        {
            select_button(override_index);
            scene_override = 0;
        }
#endif
    }
    else if(t < 156.+11.5*1.8182)
    {
        glUseProgram(graffiti_program);
        glUniform1f(graffiti_iTime_location, t);
        glUniform2f(graffiti_iResolution_location, w, h);
      
#ifdef MIDI
        glUniform1f(graffiti_iFader0_location, fader0);
        glUniform1f(graffiti_iFader1_location, fader1);
        glUniform1f(graffiti_iFader2_location, fader2);
        glUniform1f(graffiti_iFader3_location, fader3);
        glUniform1f(graffiti_iFader4_location, fader4);
        glUniform1f(graffiti_iFader5_location, fader5);
        glUniform1f(graffiti_iFader6_location, fader6);
        glUniform1f(graffiti_iFader7_location, fader7);
        
        if(override_index == 22) 
        {
            select_button(override_index);
            scene_override = 0;
        }
#endif
    }
    else if(t < 156.+12.*1.8182)
    {
        glUseProgram(transbubbles_program);
        glUniform1f(transbubbles_iTime_location, t);
        glUniform2f(transbubbles_iResolution_location, w, h);
      
#ifdef MIDI
        glUniform1f(transbubbles_iFader0_location, fader0);
        glUniform1f(transbubbles_iFader1_location, fader1);
        glUniform1f(transbubbles_iFader2_location, fader2);
        glUniform1f(transbubbles_iFader3_location, fader3);
        glUniform1f(transbubbles_iFader4_location, fader4);
        glUniform1f(transbubbles_iFader5_location, fader5);
        glUniform1f(transbubbles_iFader6_location, fader6);
        glUniform1f(transbubbles_iFader7_location, fader7);
        
        if(override_index == 23) 
        {
            select_button(override_index);
            scene_override = 0;
        }
#endif
    }
    else if(t < 156.+13.*1.8182)
    {
        glUseProgram(volclouds_program);
        glUniform1f(volclouds_iTime_location, t);
        glUniform2f(volclouds_iResolution_location, w, h);
      
#ifdef MIDI
        glUniform1f(volclouds_iFader0_location, fader0);
        glUniform1f(volclouds_iFader1_location, fader1);
        glUniform1f(volclouds_iFader2_location, fader2);
        glUniform1f(volclouds_iFader3_location, fader3);
        glUniform1f(volclouds_iFader4_location, fader4);
        glUniform1f(volclouds_iFader5_location, fader5);
        glUniform1f(volclouds_iFader6_location, fader6);
        glUniform1f(volclouds_iFader7_location, fader7);
        
        if(override_index == 24) 
        {
            select_button(override_index);
            scene_override = 0;
        }
#endif
    }
    else if(t < 156.+14.*1.8182)
    {
        glUseProgram(groundboxes_program);
        glUniform1f(groundboxes_iTime_location, t);
        glUniform2f(groundboxes_iResolution_location, w, h);
      
#ifdef MIDI
        glUniform1f(groundboxes_iFader0_location, fader0);
        glUniform1f(groundboxes_iFader1_location, fader1);
        glUniform1f(groundboxes_iFader2_location, fader2);
        glUniform1f(groundboxes_iFader3_location, fader3);
        glUniform1f(groundboxes_iFader4_location, fader4);
        glUniform1f(groundboxes_iFader5_location, fader5);
        glUniform1f(groundboxes_iFader6_location, fader6);
        glUniform1f(groundboxes_iFader7_location, fader7);
        
        if(override_index == 25) 
        {
            select_button(override_index);
            scene_override = 0;
        }
#endif
    }
    else if(t < t_end)
    {
        glUseProgram(voronoidesign_program);
        glUniform1f(voronoidesign_iTime_location, t);
        glUniform2f(voronoidesign_iResolution_location, w, h);
      
#ifdef MIDI
        glUniform1f(voronoidesign_iFader0_location, fader0);
        glUniform1f(voronoidesign_iFader1_location, fader1);
        glUniform1f(voronoidesign_iFader2_location, fader2);
        glUniform1f(voronoidesign_iFader3_location, fader3);
        glUniform1f(voronoidesign_iFader4_location, fader4);
        glUniform1f(voronoidesign_iFader5_location, fader5);
        glUniform1f(voronoidesign_iFader6_location, fader6);
        glUniform1f(voronoidesign_iFader7_location, fader7);
        
        if(override_index == 26) 
        {
            select_button(override_index);
            scene_override = 0;
        }
#endif
    }
    else ExitProcess(0);
    
    quad();

    // Render post processing to buffer
    glUseProgram(post_program);
    glUniform2f(post_iResolution_location, w, h);
    glUniform1f(post_iFSAA_location, fsaa);
    glUniform1i(post_iChannel0_location, 0);
    glUniform1f(post_iTime_location, t-11);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, first_pass_texture);
//     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    quad();
    
    // Render to screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    glUseProgram(text_program);
    glUniform2f(text_iResolution_location, w, h);
    glUniform1f(text_iFontWidth_location, font_texture_size);
    glUniform1f(text_iTime_location, t-11);
    glUniform1i(text_iChannel0_location, 0);
    glUniform1i(text_iFont_location, 1);
    glUniform1f(text_iFSAA_location, fsaa);
    
#ifdef MIDI
    glUniform1f(text_iFader0_location, fader0);
    glUniform1f(text_iFader1_location, fader1);
    glUniform1f(text_iFader2_location, fader2);
    glUniform1f(text_iFader3_location, fader3);
    glUniform1f(text_iFader4_location, fader4);
    glUniform1f(text_iFader5_location, fader5);
    glUniform1f(text_iFader6_location, fader6);
    glUniform1f(text_iFader7_location, fader7);
#endif
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, first_pass_texture);
//     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, font_texture_handle);
//     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, font_texture_size, font_texture_size, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    
    quad();
    glBindTexture(GL_TEXTURE_2D, 0);
}



