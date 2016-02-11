
/* sound.c      graphics-Routinen (c) Markus Hoffmann    */


/* all this stuff needs improvements ... can you help?   */


/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ======================================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <math.h>
#include <sys/types.h>
#include "defs.h"
#ifdef USE_ALSA
  #include <signal.h>
  #include <sys/stat.h>
  #include <fcntl.h>
  #include <alsa/asoundlib.h>
  #include <pthread.h>   /* Posix 1003.1c threads */
#endif

#include "x11basic.h"
#include "sound.h"

void speaker(int frequency);

int soundsystem_available=0;

#ifdef USE_SDL
static SDL_AudioSpec audioformat;
static void mixeAudio(void *nichtVerwendet, Uint8 *stream, int laenge);
#endif
#ifdef USE_ALSA

#define GAIN 5000.0

static char *device = "default";			 /* playback device */
static snd_pcm_t *handle;
static unsigned char *mixbuf;
static int mixpos;
static int mixlen;
static const int format=16/8;  /*16/8*/
static const int channels=1;  /* 2 */
static snd_pcm_uframes_t buffer_size = 256;
static snd_pcm_uframes_t period_size = 64;
static int rrate=44100;  /* sample rate */

#ifdef SNDERR
#undef SNDERR
#endif

#define SNDERR if(e<0) printf("SOUND: Error: %d, %s\n",e,snd_strerror(e))

#endif

static struct sample {
    Uint8 *data;
    Uint32 dpos;
    Uint32 dlen;
    double pitch;
    int endless;      /* 1 if the sample should be played in a loop.*/
    int generator;    /* 1 if the generator is active, 0 if a sample is talen */
    double (*waveform)(double);   /* Waveform of sound can also be sin,triag,square and noise*/
    int timecount;    /* time since key press or release in samples */
    double phase;     /* waveform phase */
    double frequency; /* generator frequency */
    double gain;      /* gain (volume) */
    int attack; /* the time taken for initial run-up of level from nil to peak.*/
    int decay;  /* time taken for the subsequent run down from the attack level to the designated sustain level.*/
    double sustain; /* level (relative to attack ? ) during the main sequence of the sound's duration */
    int release;    /* time taken for the level to decay from the sustain level to zero after the key is released */
    double envelope;
} sounds[MAX_SOUND_CHANNELS];


typedef struct{
  char riff[4];
  Uint32 fsize;
  char wavefmt[8];
  Uint32 f16;
  Uint16 audio_format;
  Uint16 num_channels;
  Uint32 sample_rate;
  Uint32 byte_rate;
  Uint16 block_align;
  Uint16 bits_per_sample;
  char data[4];
  Uint32 dsize;
} WAVHEADER;




#ifdef USE_ALSA

static double sawtooth(double a) {
  a=fmod(a,2*PI);
  return(a/2/PI*2-1);
}
static double triag(double a) {
  a=fmod(a,2*PI);
  if(a>PI) return((2*PI-a)/PI*2-1);
  else return(a/PI*2-1);
}
static double square(double a) {
  a=fmod(a,2*PI);
  return(a<PI?-1:1);
}
static double noise(double a)   {return(2*(double)random()/RAND_MAX-1);}
static double silence(double a) {return(0);}



/* Enveloppe function for synthesizer */

static double envelope(int *note_active, int gate, double *env_level, int t, 
                int attack, int decay, double sustain, int release) {
//printf("a=%d, d=%d, s=%g, r=%d\n",attack,decay,sustain,release);
//if((t>>3) & 1) printf("envelope: %d acive=%d gate=%d %g \n",t,*note_active,gate,*env_level);

    if (gate==1)  {
        if(t>attack+decay) return(*env_level=sustain);
        if(t>attack) {
	  if(decay) return(*env_level=1.0-(1.0-sustain)*(t-attack)/(double)decay);
          else return(*env_level=sustain);
        }
	if(attack) return(*env_level=(double)t/(double)attack);
	else return(*env_level=1.0);
    } else {
        if(t>release) {
            if(note_active) *note_active = 0;
            return(*env_level=0);
        }
       if(release) return(*env_level*(1.0-(double)t/(double)release));
       else return(*env_level=0);
    }
}




static void mixeAudio(Uint8 *stream, int laenge) {
  int i,j;
  double ev;
  Uint32 menge;
  memset(stream, 0, laenge*format*channels);

  for(i=0; i<MAX_SOUND_CHANNELS; ++i ) {
    if(sounds[i].generator==0) {
        menge = (sounds[i].dlen-sounds[i].dpos);
        if(menge>laenge) menge=laenge;
	if(menge>0) {
	  printf("copy channel %d, [%d->%d]\n",i,(int)sounds[i].dpos,(int)menge);	
/* hier entweder sample abspielen oder tongenerator */ 

	  for(j=0;j<menge;j++) {
	    stream[j]+=sounds[i].data[sounds[i].dpos+j];
	
	  }
	}
        sounds[i].dpos += menge;
	if(sounds[i].dpos==sounds[i].dlen && sounds[i].endless) sounds[i].dpos=0;
    } else {
      Uint16 *a=(Uint16 *)stream;
      for(j=0;j<laenge;j++) {
        ev=envelope(&sounds[i].generator,(sounds[i].generator==1),&(sounds[i].envelope),sounds[i].timecount+j,sounds[i].attack,sounds[i].decay,
	sounds[i].sustain,sounds[i].release);
	
        a[j]+=(Uint16)(GAIN*sounds[i].gain*ev*
        (sounds[i].waveform)((double)2*PI*sounds[i].frequency*j/rrate+sounds[i].phase));
	
	
  	//  printf("%d %g\n",sounds[i].timecount+j,ev);
	}
      sounds[i].phase=fmod(laenge*2*PI*sounds[i].frequency/rrate+sounds[i].phase,2*PI);
      sounds[i].timecount+=laenge;
    } 
  }
}

void *soundthread(void *arg) {
  int e;
#if DEBUG
  printf("soundthread gestartet...\n");
#endif
  mixpos=0;
  mixlen=buffer_size*format*channels;
  mixbuf=malloc(mixlen);
  e=snd_pcm_prepare(handle);SNDERR;
  e=snd_pcm_start(handle);SNDERR;
  while(1) {
   // avail = snd_pcm_avail_update(handle);

      mixeAudio(mixbuf,buffer_size);
    //  printf("schreibe...pos=%d len=%d\n",mixpos,mixlen);
      e=snd_pcm_writei(handle,mixbuf,buffer_size);
      if(e<0) {
        SNDERR;
	snd_pcm_prepare (handle);
      } 
  }
  e=snd_pcm_stop(handle);SNDERR;
  free(mixbuf);
}
#endif


int init_soundsystem() {
#ifdef USE_SDL
  if(SDL_Init(SDL_INIT_AUDIO)<0) {
    printf("ERROR: could not initialize the audio system.\n");
  } else {
  
   /* Format: 16 Bit, stereo, 22 KHz */
    audioformat.freq = 22050;
    audioformat.format = AUDIO_S16;
    audioformat.channels = 2;
    audioformat.samples = 512;        /* ein guter Wert für Spiele */
    audioformat.callback = mixeAudio;
    audioformat.userdata = NULL;
    /* das Audio-Gerät öffnen und das Abspielen beginnen */
    if(SDL_OpenAudio(&audioformat, NULL) < 0) {
      printf("Audio-Gerät konnte nicht geöffnet werden: %s\n", SDL_GetError());
    } else {
      soundsystem_available=1;
      SDL_PauseAudio(0);
    }
  }
#endif
#ifdef USE_ALSA
  int e,c;
  snd_pcm_hw_params_t *hw_params;
  snd_pcm_sw_params_t *sw_params;

  pthread_t sthread;
  
  if((e=snd_pcm_open(&handle, device, SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
    fprintf(stderr, "cannot open audio device %s (%s)\n", 
	device, snd_strerror (e));
	handle = NULL;
  } else {
    snd_pcm_hw_params_malloc (&hw_params);
    e=snd_pcm_hw_params_any (handle, hw_params); SNDERR;
    e=snd_pcm_hw_params_set_access     (handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);SNDERR;
    e=snd_pcm_hw_params_set_format     (handle, hw_params, SND_PCM_FORMAT_S16_LE);SNDERR;
    e=snd_pcm_hw_params_set_rate       (handle, hw_params, rrate, 0);SNDERR;
    e=snd_pcm_hw_params_set_channels   (handle, hw_params, 1);SNDERR;
    e=snd_pcm_hw_params_set_buffer_size(handle, hw_params, buffer_size);SNDERR;
    e=snd_pcm_hw_params_set_period_size(handle, hw_params, period_size, 0);SNDERR;
    e=snd_pcm_hw_params (handle, hw_params);SNDERR;
    snd_pcm_hw_params_free (hw_params);

  
 #if 0 
    if((e=snd_pcm_set_params(handle,SND_PCM_FORMAT_U8,
	SND_PCM_ACCESS_RW_INTERLEAVED,
	1,
	48000,
	1,
	500000)) < 0) {   /* 0.5sec */
      printf("Playback open error: %s\n", snd_strerror(e));
    } else {
#endif
#if 0
      e=snd_pcm_sw_params_malloc(&sw_params);SNDERR;
      e=snd_pcm_sw_params_current(handle, sw_params);SNDERR;
 //     e=snd_pcm_sw_params_set_start_threshold(handle, sw_params, buffer_size - period_size);
 //     e=snd_pcm_sw_params_set_avail_min(handle, sw_params, period_size);
      e=snd_pcm_sw_params(handle, sw_params);SNDERR;
      snd_pcm_sw_params_free (sw_params);
#endif

    /* prepare the structure */
    for(c=0;c<MAX_SOUND_CHANNELS; ++c) {
      sounds[c].waveform=square;
      sounds[c].sustain=1;
      sounds[c].gain=1;
    }
    /* initiate the sound processing thread */
    pthread_create(&sthread, NULL, soundthread, NULL);
    soundsystem_available=1;
    
  }

#endif
  return(soundsystem_available?0:-1);
}
void sound_activate() {
  static int activated=0;
  if(!activated) {
    init_soundsystem();
    activated=1;
  }
}





int do_sound(int c,double freq, double volume, int duration) {
#if defined USE_SDL 
  int numsamples=44100;
  int rrate=44100;
  char *wav=malloc(sizeof(WAVHEADER)+numsamples*1*2);
  int i;
  WAVHEADER *wpt=(WAVHEADER *)wav;
  Uint16 *wdta=(Uint16 *)(wav+sizeof(WAVHEADER));
  wpt->riff[0]='R';
  wpt->riff[1]='I';
  wpt->riff[2]='F';
  wpt->riff[3]='F';
  wpt->wavefmt[0]='W';
  wpt->wavefmt[1]='A';
  wpt->wavefmt[2]='V';
  wpt->wavefmt[3]='E';
  wpt->wavefmt[4]='f';
  wpt->wavefmt[5]='m';
  wpt->wavefmt[6]='t';
  wpt->wavefmt[7]=' ';
  wpt->f16=16;
  wpt->audio_format=1;
  wpt->num_channels=1;
  wpt->sample_rate=rrate;
  wpt->bits_per_sample=16; 
  wpt->byte_rate=wpt->sample_rate*wpt->num_channels*(wpt->bits_per_sample/8);
  wpt->block_align=wpt->num_channels*(wpt->bits_per_sample/8);
  wpt->data[0]='d';
  wpt->data[1]='a';
  wpt->data[2]='t';
  wpt->data[3]='a';
  wpt->dsize=numsamples*wpt->block_align;
  wpt->fsize=12+24+wpt->dsize+8;
  for(i=0;i<numsamples;i++) wdta[i]=(int)(sin(2*PI*i*freq/rrate)*volume); //+0x8000;
//  printf("nums=%g\n",rrate/freq/2/PI);
  /* Wav erzeugen */
  c=do_playsound(c, wav,wpt->fsize,256,0xffff,1);
  
  free(wav);
#if DEBUG
  printf("sound %d %g %d %d\n",c,freq,volume,duration);
#endif
#else
  #ifdef USE_ALSA
  
  if(c<0) {
    /* einen leeren Audio-Slot suchen */
    for(c=0;c<MAX_SOUND_CHANNELS; ++c) {
        if ( sounds[c].dpos == sounds[c].dlen ) {
            break;
        }
    }
  }
  if(c>=MAX_SOUND_CHANNELS) return(-1);

  if(volume>=0) sounds[c].gain=volume;

  if(freq>0) {  /* Ton wird angeschlagen */
    sounds[c].frequency=freq;
    sounds[c].timecount=0;    
    sounds[c].generator=1;
  } else if (freq==0) { 
    sounds[c].generator=2; /* Losgelassen  */
    sounds[c].timecount=0;
  }
 //printf("dosound: freq=%g volume=%g gen=%d\n",sounds[c].frequency,sounds[c].gain,sounds[c].generator);


  #else
  speaker(freq);
  #endif 
#endif
  return(0);
}

int do_wave(int c,int form, double attack, double decay, double sustain, double release ) {
  #ifdef USE_ALSA
  if(c<0) {
    /* einen leeren Audio-Slot suchen */
    for(c=0;c<MAX_SOUND_CHANNELS; ++c) {
        if ( sounds[c].dpos == sounds[c].dlen ) {
            break;
        }
    }
  }
  if(c>=MAX_SOUND_CHANNELS) return(-1);

   /*  to be completed */
  switch (form) {
  case -1: break;
  case 0:
    sounds[c].waveform=silence;break;
  case 1:
    sounds[c].waveform=sin;break;
  case 2:
    sounds[c].waveform=square;break;
  case 3:
    sounds[c].waveform=triag;break;
  case 4:
    sounds[c].waveform=sawtooth;break;
  case 5:
    sounds[c].waveform=noise;break;
  default:
    printf("WAVE: Unknown waveform %d\n",form);
  }
  if(attack>=0)  sounds[c].attack=(int)(attack*(double)rrate/1000);
  if(decay>=0)   sounds[c].decay=(int)(decay*(double)rrate/1000);
  if(sustain>=0) sounds[c].sustain=sustain;
  if(release>=0)   sounds[c].release=(int)(release*(double)rrate/1000);
 // printf("a=%d, d=%d, s=%g, r=%d\n",sounds[c].attack,sounds[c].decay,sounds[c].sustain,sounds[c].release);
#endif
  return(0);
}


int do_playsound(int c, char *dataa,int size, int pitch, int volume, int flag) {
#ifdef USE_SDL
  SDL_AudioSpec wave;
  Uint8 *data;
  Uint32 dlen;
  SDL_AudioCVT cvt;
  SDL_RWops *src;
  src=SDL_RWFromMem(dataa,size);
#endif

  if(c<0) {
    /* einen leeren Audio-Slot suchen */
    for(c=0;c<MAX_SOUND_CHANNELS; ++c) {
        if ( sounds[c].dpos == sounds[c].dlen ) {
            break;
        }
    }
  }
  if(c>=MAX_SOUND_CHANNELS) return(-1);
#ifdef USE_SDL
    /* Audio-Daten laden und nach 16 Bit und 22KHz wandeln */
    if ( SDL_LoadWAV_RW(src,1, &wave, &data, &dlen) == NULL ) {
        fprintf(stderr, "Konnte  nicht laden: %s\n", SDL_GetError());
        return(-1);
    }
    SDL_BuildAudioCVT(&cvt, wave.format, wave.channels, wave.freq*pitch/256,
                            AUDIO_S16,   2,             22050);
    cvt.buf = malloc(dlen*cvt.len_mult);
    memcpy(cvt.buf, data, dlen);
    cvt.len = dlen;
    SDL_ConvertAudio(&cvt);
    SDL_FreeWAV(data);

    /* die Audiodaten in den Slot schreiben (Abspielen startet sofort) */
    if(sounds[c].data) free(sounds[c].data);
    SDL_LockAudio();
    sounds[c].data = cvt.buf;
    sounds[c].dlen = cvt.len_cvt;
    sounds[c].dpos = 0;
    sounds[c].endless = flag;
    sounds[c].generator = 0; /* 0=sample */
    SDL_UnlockAudio();
#endif
#if DEBUG
  printf("playsound %d %d %d %d %d\n",c,(int)data,size,pitch,volume);
#endif
  return(c);
}

#ifdef USE_SDL
static void mixeAudio(void *nichtVerwendet, Uint8 *stream, int laenge) {
    int i;
    Uint32 menge;

    for ( i=0; i<MAX_SOUND_CHANNELS; ++i ) {
        menge = (sounds[i].dlen-sounds[i].dpos);
        if ( menge > laenge ) {
            menge = laenge;
        }
        SDL_MixAudio(stream, &sounds[i].data[sounds[i].dpos], menge, SDL_MIX_MAXVOLUME);
        sounds[i].dpos += menge;
	if(sounds[i].dpos==sounds[i].dlen && sounds[i].endless) sounds[i].dpos=0;
    }
}
#endif

#if 0
#ifdef USE_ALSA
static void ALSA_playaudio() {
  int status;
  snd_pcm_uframes_t frames_left;
  const Uint8 *sample_buf = (const Uint8 *) mixbuf;
  const int frame_size = format*channels;

  frames_left = ((snd_pcm_uframes_t) mixlen);

  while (frames_left>0) {
    snd_pcm_wait(handle, -1);

    status =snd_pcm_writei(handle, sample_buf, frames_left);
    if(status<0) {
      if(status==-EAGAIN) {
        printf("ALSA_playaudio: EAGAIN.\n");				
	continue;
      }
      status=snd_pcm_recover(handle,status,0);
      if(status<0) {
	printf("ALSA write failed (unrecoverable): %s\n",snd_strerror(status));
	return;
      }
      continue;
    }
    sample_buf+=status*frame_size;
    frames_left-=status;
  }
}
#endif
#endif
