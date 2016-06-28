#include "main.h"
#include "soundpipe.h"

#define MY_BUFSIZE 64

static volatile uint8_t nextbuf;
static int16_t buf0[MY_BUFSIZE];
static int16_t buf1[MY_BUFSIZE];

/* SOUNDPIPE */

#define NOSCS 5

static const uint8_t scale[] = {48, 60, 65, 67, 72, 79, 64, 52};

static uint8_t please_play = 0;
static sp_data *sp;
static sp_ftbl *ft;
static sp_osc *osc;
static sp_fosc *fosc[NOSCS];
static sp_revsc *revsc;

uint32_t compute_buffer(int16_t *pbuf, int  bufsize);

__IO uint32_t XferCplt = 0;

void ms_delay(int ms)
{
   while (ms-- > 0) {
      volatile int x=5971;
      while (x-- > 0)
         __asm("nop");
   }
}

void WavePlayBack(uint32_t AudioFreq)
{ 
    /* note: these mallocs are NOT freed at the moment */
    sp_create(&sp);
    sp->sr = AudioFreq;

    sp_ftbl_create(sp, &ft, 8192);
    sp_gen_sine(sp, ft);

    sp_osc_create(&osc);

    sp_osc_init(sp, osc, ft, 0);
    osc->freq = 0.2f;
    osc->amp = 1.f;
    uint32_t i;

    for(i = 0; i < NOSCS; i++) {
        sp_fosc_create(&fosc[i]);
        sp_fosc_init(sp, fosc[i], ft);
        fosc[i]->freq = sp_midi2cps(scale[i]);
        fosc[i]->amp = 0.1f;
    }

    for(i = 0; i < MY_BUFSIZE; i++) {
        buf0[i] = 0;
        buf1[i] = 0;
    }

    sp_revsc_create(&revsc);
    sp_revsc_init(sp, revsc);

    /* Initialize wave player (Codec, DMA, I2C) */
    WavePlayerInit(AudioFreq);
    nextbuf = 1;

    compute_buffer(buf1, MY_BUFSIZE);
    Audio_MAL_Play((uint32_t)buf0, MY_BUFSIZE / 2);

    EVAL_AUDIO_Mute(AUDIO_MUTE_ON);
    while(1) {
        while(nextbuf == 1);
        if(please_play == 0) {
            EVAL_AUDIO_Mute(AUDIO_MUTE_OFF);
            please_play = 1;
        }
        compute_buffer(buf0, MY_BUFSIZE);
        while(nextbuf == 0);
        compute_buffer(buf1, MY_BUFSIZE);
    };

}
int WavePlayerInit(uint32_t AudioFreq)
{ 
    /* Initialize I2S interface */  
    EVAL_AUDIO_SetAudioInterface(AUDIO_INTERFACE_I2S);
    /* Initialize the Audio codec and all related peripherals (I2S, I2C, IOExpander, IOs...) */  
    EVAL_AUDIO_Init(OUTPUT_DEVICE_AUTO, 60, AudioFreq );  
  
    return 0;
}

uint32_t compute_buffer(int16_t *pbuf, int bufsize) 
{
    int i, o;
    SPFLOAT fm = 0, lfo = 0;
    SPFLOAT tmp = 0;
    SPFLOAT r0, r1;
    for(i = 0; i < bufsize / 2; i+=2) {
        sp_osc_compute(sp, osc, NULL, &lfo);
        fm = 0;
        for(o = 0; o < NOSCS; o++) { 
            tmp = 0;
            fosc[o]->indx = ((1.0 + lfo) * 0.5) * 6;
            sp_fosc_compute(sp, fosc[o], NULL, &tmp);
            fm += tmp;
        }
      
        sp_revsc_compute(sp, revsc, &fm, &fm,  &r0, &r1);
        pbuf[i] = (fm * 0.8 + r0 * 0.1) * 32767;
        pbuf[i + 1] = (fm * 0.8  + r1 * 0.1) * 32767;
    }
    return 0;
}

void EVAL_AUDIO_TransferComplete_CallBack(uint32_t pBuffer, uint32_t Size)
{

    STM_EVAL_LEDOn(LED4);
    if(nextbuf == 0) { ;
        Audio_MAL_Play((uint32_t)buf0, MY_BUFSIZE);
        nextbuf = 1;
    } else {
        Audio_MAL_Play((uint32_t)buf1, MY_BUFSIZE);
        nextbuf = 0;
    }
}

void EVAL_AUDIO_HalfTransfer_CallBack(uint32_t pBuffer, uint32_t Size)
{  
}

void EVAL_AUDIO_Error_CallBack(void* pData)
{
    while (1);
}

uint16_t EVAL_AUDIO_GetSampleCallBack(void)
{
    return 0;
}
