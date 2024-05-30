/*
 * Audio_Test.c
 *
 *  Created on: Feb 15, 2024
 *      Author: michaeljacobson
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/gpio.h>
#include <errno.h>
#include <pthread.h>
#include <linux/input.h>
#include <sys/select.h>
#include <alsa/asoundlib.h>
#include <alsa/mixer.h>
#include <alsa/pcm.h>
#include <semaphore.h>
//#include <math.h>
#include <sys/time.h>
#include "C674xTypes.h"
#include "Ultratec_Enums.h"

#define BUFF_SIZE 4096

#define NUM_OF_CHANNELS     2
#define BYTES_PER_CHANNEL   2
#define BYTES_IN_FRAME      (NUM_OF_CHANNELS*BYTES_PER_CHANNEL)
#define FRAMES_IN_PERIOD    80
#define NUM_PERIODS         1
#define PERIOD_LEN          (FRAMES_IN_PERIOD*NUM_PERIODS)
#define AUDIO_READ_WRITE_LEN  (BYTES_IN_FRAME*FRAMES_IN_PERIOD*NUM_PERIODS)
#define PCM_WAIT_TIME		(10*NUM_PERIODS+2)

#define BUFFER_FRAME_SIZE	80

#define AUDIO_CHANNEL_HANDSET   0
#define AUDIO_CHANNEL_POTS      1

#define AUDIO_LATENCY			50000

#define DEFAULT_CIRC_BUF_LEN		1024	//this needs to be a power of 2

typedef struct
{
	S16 data[DEFAULT_CIRC_BUF_LEN];
	U32 head;
	U32 tail;
	pthread_mutex_t mutex;
	BOOL MutexInit;
} DataPipeS16_t;

pthread_t AudioRxThread = 0;
pthread_t AudioTxThread = 0;
pthread_t RxAudioProcessingThread = 0;
pthread_t TxAudioProcessingThread = 0;

sem_t AudioRxSem;
sem_t AudioTxSem;

BOOL AudioRxThread_KeepGoing = TRUE;
BOOL AudioTxThread_KeepGoing = TRUE;
BOOL RxAudioProcessingThread_KeepGoing = TRUE;
BOOL TxAudioProcessingThread_KeepGoing = TRUE;

int RxGoodCount = 0;
int RxBadCount = 0;
int TxGoodCount = 0;
int TxBadCount = 0;
int AvgTxOutputPOTS = 0;
int AvgTxOutputHS = 0;
int AvgRxInputPOTS = 0;
int AvgRxInputMic = 0;

/* Handle for the PCM device */
snd_pcm_t *playback_handle;
snd_pcm_t *capture_handle;
short Rxbuf[BUFF_SIZE];
short Txbuf[BUFF_SIZE];
short DeinterlaceRxdBuf[2][PERIOD_LEN*2]; //0 is handset, 1 is Phone line
short DeinterlaceTxdBuf[2][PERIOD_LEN*2]; //0 is handset, 1 is phone line
DataPipeS16_t WorkingCaptureBuffer[NUM_OF_CHANNELS];
DataPipeS16_t WorkingPlaybackBuffer[NUM_OF_CHANNELS];
int EffectivePeriodSizeRx = PERIOD_LEN;
int EffectivePeriodSizeTx = PERIOD_LEN;

short RawLiveCallHSSamples[PERIOD_LEN*2];
short RawLiveCallPOTSSamples[PERIOD_LEN*2];

short RawLiveCallPOTSTxSamples[PERIOD_LEN*2];
short RawLiveCallHSTxSamples[PERIOD_LEN*2];

static void *ReadAudioSamples(void *arg);
static void *WriteAudioSamples(void *arg);
static void *ProcessRxAudioSamples(void *arg);
static void *ProcessTxAudioSamples(void *arg);

BOOL InitCircBufferS16(DataPipeS16_t *p_CircBuffer, BOOL mutex);
void KillCircBufS16Mutex(DataPipeS16_t *p_CircBuffer);
BOOL isCircBufS16Empty(DataPipeS16_t *p_CircBuffer);
U32 CircBufS16Freespace(DataPipeS16_t *p_CircBuffer);
U32 GetCircBufS16DataRemaining(DataPipeS16_t *p_CircBuffer);
U32 HandleCircBufS16PushPull(DataPipeS16_t *p_CircBuffer, S16 *p_buf, U32 len, BOOL push);

/********************************************************************************************************************************
 *  void function(void)
 *
 *  Desc
 *
 *  Parameters:
 *      - none
 *
 *  Returns:  none
 *
 *  Notes:  notes
 ********************************************************************************************************************************/
void InitSoundDrivers(void)
{
	sem_init(&AudioRxSem, 0, 0);
	sem_init(&AudioTxSem, 0, 0);

	if(pthread_create( &RxAudioProcessingThread, NULL, ProcessRxAudioSamples, NULL ) == 0)
	{
		if(pthread_create( &TxAudioProcessingThread, NULL, ProcessTxAudioSamples, NULL ) == 0)
		{
			if(pthread_create( &AudioTxThread, NULL, WriteAudioSamples, NULL ) == 0)
			{
				if(pthread_create( &AudioRxThread, NULL, ReadAudioSamples, NULL ) == 0)
				{

				}
			}
		}
	}
}

/********************************************************************************************************************************
 *  void function(void)
 *
 *  Desc
 *
 *  Parameters:
 *      - none
 *
 *  Returns:  none
 *
 *  Notes:  notes
 ********************************************************************************************************************************/
void CloseSoundDrivers(void)
{
	AudioRxThread_KeepGoing = FALSE;
	AudioTxThread_KeepGoing = FALSE;
	RxAudioProcessingThread_KeepGoing = FALSE;
	TxAudioProcessingThread_KeepGoing = FALSE;

	sem_post(&AudioRxSem);
	sem_post(&AudioTxSem);

    //close the one that triggers all the rest first
    pthread_join(AudioRxThread, NULL);

    pthread_join(AudioTxThread, NULL);

    pthread_join(RxAudioProcessingThread, NULL);

    pthread_join(TxAudioProcessingThread, NULL);

    snd_config_update_free_global();

    sem_destroy(&AudioRxSem);
    sem_destroy(&AudioTxSem);
}

/********************************************************************************************************************************
 *  void function(void)
 *
 *  Desc
 *
 *  Parameters:
 *      - none
 *
 *  Returns:  none
 *
 *  Notes:  notes
 ********************************************************************************************************************************/
void *ReadAudioSamples(void *arg)
{
    //BOOL KeepGoing = TRUE;
    int err;
    int rate = 8000; /* Sample rate */
    unsigned int exact_rate; /* Sample rate returned by */
    int dir;
    //snd_pcm_uframes_t bufferedframes = BUFFER_FRAME_SIZE;
	unsigned int BufferTime = AUDIO_LATENCY;

    /* This structure contains information about the hardware and can be used to specify the configuration to be used for */
    /* the PCM stream. */
    snd_pcm_hw_params_t *hw_params;

    /* Name of the PCM device, like hw:0,0 */
    /* The first number is the number of the soundcard, the second number is the number of the device. */
    //static char *device = "hw:0,0"; /* capture device */
    static char *device = "default"; /* capture device */

    /* Open PCM. The last parameter of this function is the mode. */
    if ((err = snd_pcm_open (&capture_handle, device, SND_PCM_STREAM_CAPTURE, 0)) < 0)
    {
        fprintf (stderr, "cannot open RX audio device (%s)\n", snd_strerror (err));
        exit (1);
    }

    memset(Rxbuf,0,sizeof(Rxbuf));

#if 1
    /* Allocate the snd_pcm_hw_params_t structure on the stack. */
    if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0)
    {
        fprintf (stderr, "cannot allocate RX hardware parameter structure (%s)\n", snd_strerror (err));
        exit (1);
    }

    /* Init hwparams with full configuration space */
    if ((err = snd_pcm_hw_params_any (capture_handle, hw_params)) < 0)
    {
        fprintf (stderr, "cannot initialize RX hardware parameter structure (%s)\n", snd_strerror (err));
        exit (1);
    }

    /* Set access type. */
    if ((err = snd_pcm_hw_params_set_access (capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0)
    {
        fprintf (stderr, "cannot set access type (%s)\n", snd_strerror (err));
        exit (1);
    }

    /* Set sample format */
    if ((err = snd_pcm_hw_params_set_format (capture_handle, hw_params, SND_PCM_FORMAT_S16_LE)) < 0)
    {
        fprintf (stderr, "cannot set RX sample format (%s)\n", snd_strerror (err));
        exit (1);
    }

    /* Set sample rate. If the exact rate is not supported by the hardware, use nearest possible rate. */
    exact_rate = rate;
    if ((err = snd_pcm_hw_params_set_rate_near (capture_handle, hw_params, &exact_rate, 0)) < 0)
    {
        fprintf (stderr, "cannot set RX sample rate (%s)\n", snd_strerror (err));
        exit (1);
    }

    if (rate != exact_rate) {
        fprintf(stderr, "The RX rate %d Hz is not supported by your hardware.\n ==> Using %d Hz instead.\n", rate, exact_rate);
    }

    /* Set period size to 80 frames. */
    err = snd_pcm_hw_params_set_period_size_near(capture_handle, hw_params, &EffectivePeriodSizeRx, &dir);
    if (err < 0)
    {
        fprintf (stderr, "cannot set RX channel period (%s)\n", snd_strerror (err));
    }
    if (EffectivePeriodSizeRx != PERIOD_LEN)
    {
        fprintf(stderr, "The RX period %d is not supported by your hardware.\n ==> Using %d instead.\n", PERIOD_LEN, (int)EffectivePeriodSizeRx);
    }

    /* Set number of channels */
    if ((err = snd_pcm_hw_params_set_channels(capture_handle, hw_params, NUM_OF_CHANNELS)) < 0)
    {
        fprintf (stderr, "cannot set RX channel count (%s)\n", snd_strerror (err));
        exit (1);
    }

#if 0
    if( (err = snd_pcm_hw_params_set_buffer_size_near (capture_handle, hw_params, &bufferedframes)) < 0)
    {
        fprintf (stderr, "cannot set RX buffer size near (%s)\n", snd_strerror (err));
    }
    if(bufferedframes != BUFFER_FRAME_SIZE)
    {
    	printf("The Rx Buffer size is not Supported. using %ld instead\n", bufferedframes);
    }
#else
    /*if((err = snd_pcm_hw_params_get_buffer_size(hw_params, &bufferedframes)) == 0)
    {
    	printf("Rx Buffer size is %ld\n", bufferedframes);
    }
    else
    {
    	fprintf (stderr, "cannot get RX buffer size (%d: %s)\n", err, snd_strerror (err));
    }*/
    
	if( (err = snd_pcm_hw_params_set_buffer_time_near (capture_handle, hw_params, &BufferTime, &dir)) < 0)
	{
		fprintf (stderr, "cannot set RX buffer time near (%s)\n", snd_strerror (err));
	}
	if(BufferTime != AUDIO_LATENCY)
	{
		printf("The Rx Buffer time is not Supported. using %d instead\n", BufferTime);
	}
#endif

    /* Apply HW parameter settings to PCM device and prepare device. */
    if ((err = snd_pcm_hw_params (capture_handle, hw_params)) < 0)
    {
        fprintf (stderr, "cannot set RX parameters (%s)\n", snd_strerror (err));
        exit (1);
    }

    if ((err = snd_pcm_prepare (capture_handle)) < 0)
    {
        fprintf (stderr, "cannot prepare RX audio interface for use (%s)\n", snd_strerror (err));
        exit (1);
    }
#else
    if ((err = snd_pcm_set_params(capture_handle,
    		SND_PCM_FORMAT_S16_LE,
			SND_PCM_ACCESS_RW_INTERLEAVED,
			2,
			8000,
			0,
			50000)) < 0) /* 0.05sec */
    {
            printf("Playback open error: %s\n", snd_strerror(err));
            exit(EXIT_FAILURE);
    }
#endif

    snd_pcm_start(capture_handle);

    InitCircBufferS16(&WorkingCaptureBuffer[AUDIO_CHANNEL_HANDSET], TRUE);
    InitCircBufferS16(&WorkingCaptureBuffer[AUDIO_CHANNEL_POTS], TRUE);

    /***************************************************************************************************************/
    /***************************************************************************************************************/
    /***************************************************************************************************************/
    /***************************************************************************************************************/
    /***************************************************************************************************************/
    /***************************************************************************************************************/
    /***************************************************************************************************************/
    /***************************************************************************************************************/
    /***************************************************************************************************************/
    while(AudioRxThread_KeepGoing)
    {
        if(snd_pcm_wait(capture_handle, PCM_WAIT_TIME) >= 0)
        {
			// Read data into the buffer.
        	if ((err = snd_pcm_readi (capture_handle, Rxbuf, EffectivePeriodSizeRx)) != EffectivePeriodSizeRx)
			{
        		printf("Number of Samples trying to Read: %d\nNumber of Samples actually Read: %d\n", (EffectivePeriodSizeRx), err);
				if(err == -EPIPE)
				{
					fprintf (stderr, "read Overrun (%s)\n", snd_strerror (err));
					snd_pcm_prepare(capture_handle);
				}
				else if(err < 0)
				{
					fprintf (stderr, "read from audio interface failed (%d: %s)\nGood Count: %d\n", err, snd_strerror (err), RxGoodCount);
					int frames = snd_pcm_recover(capture_handle, err, 0);
					if(frames < 0)
					{
						fprintf (stderr, "recover read from audio interface failed (%d: %s)\n", frames, snd_strerror (frames));
						//exit (1);
					}
				}
				else
				{
					printf("Non-Period length Read: %d instead of %d\n", err, PERIOD_LEN);
				}
			}
			else
			{
				RxGoodCount++;
				sem_post(&AudioRxSem);
			}
        }
        else
        {
        	RxBadCount++;
        	int avail_frames = snd_pcm_avail(capture_handle);
        	if(avail_frames > 0)
        	{
        		printf("Available frames for Rx: %d\n", avail_frames);
        	}
        }
    }

    snd_pcm_drop(capture_handle);
    snd_pcm_hw_params_free (hw_params);
    snd_pcm_close (capture_handle);

    return 0;
}

/********************************************************************************************************************************
 *  void function(void)
 *
 *  Desc
 *
 *  Parameters:
 *      - none
 *
 *  Returns:  none
 *
 *  Notes:  notes
 ********************************************************************************************************************************/
void *WriteAudioSamples(void *arg)
{
    //BOOL KeepGoing = TRUE;
    int err;
    int rate = 8000; /* Sample rate */
    unsigned int exact_rate; /* Sample rate returned by */
    int dir;
    /* Playback stream */
    snd_pcm_stream_t stream = SND_PCM_STREAM_PLAYBACK;
    /* This structure contains information about the hardware and can be used to specify the configuration to be used for */
    /* the PCM stream. */
    snd_pcm_hw_params_t *hw_params;
    //snd_pcm_uframes_t bufferedframes = BUFFER_FRAME_SIZE;
	unsigned int BufferTime = AUDIO_LATENCY;

    /* Name of the PCM device, like plughw:0,0 */
    /* The first number is the number of the soundcard, the second number is the number of the device. */
    static char *device = "default"; /* playback device */

    /* Open PCM. The last parameter of this function is the mode. */
    if ((err = snd_pcm_open (&playback_handle, device, stream, 0)) < 0)
    {
        fprintf (stderr, "cannot open TX audio device (%s)\n", snd_strerror (err));
        exit (1);
    }
#if 1
    /* Allocate the snd_pcm_hw_params_t structure on the stack. */
    if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0)
    {
        fprintf (stderr, "cannot allocate TX hardware parameters (%s)\n", snd_strerror (err));
        exit (1);
    }

    /* Init hwparams with full configuration space */
    if ((err = snd_pcm_hw_params_any (playback_handle, hw_params)) < 0)
    {
        fprintf (stderr, "cannot initialize TX hardware parameter structure (%s)\n", snd_strerror (err));
        exit (1);
    }

    /* Set access type. */
    if ((err = snd_pcm_hw_params_set_access (playback_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0)
    {
        fprintf (stderr, "cannot set TX access type (%s)\n", snd_strerror (err));
        exit (1);
    }

    /* Set sample format */
    if ((err = snd_pcm_hw_params_set_format (playback_handle, hw_params, SND_PCM_FORMAT_S16_LE)) < 0)
    {
        fprintf (stderr, "cannot set TX sample format (%s)\n", snd_strerror (err));
        exit (1);
    }

    /* Set sample rate. If the exact rate is not supported by the hardware, use nearest possible rate. */
    exact_rate = rate;
    if ((err = snd_pcm_hw_params_set_rate_near (playback_handle, hw_params, &exact_rate, 0)) < 0)
    {
        fprintf (stderr, "cannot set TX sample rate (%s)\n", snd_strerror (err));
        exit (1);
    }

    if (rate != exact_rate)
    {
        fprintf(stderr, "The Tx rate %d Hz is not supported by your hardware.\n ==> Using %d Hz instead.\n", rate, exact_rate);
    }

    /* Set period size to 80 frames. */
    err = snd_pcm_hw_params_set_period_size_near(playback_handle, hw_params, &EffectivePeriodSizeTx, &dir);
    if (err < 0)
    {
        fprintf (stderr, "cannot set TX channel period (%s)\n", snd_strerror (err));
    }
    if (EffectivePeriodSizeTx != (PERIOD_LEN))
    {
        fprintf(stderr, "The TX period %d is not supported by your hardware.\n ==> Using %d instead.\n", (PERIOD_LEN), (int)EffectivePeriodSizeTx);
    }

    /* Set number of channels */
    if ((err = snd_pcm_hw_params_set_channels (playback_handle, hw_params, NUM_OF_CHANNELS)) < 0)
    {
        fprintf (stderr, "cannot set TX channel count (%s)\n", snd_strerror (err));
        exit (1);
    }
#if 0
    if( (err = snd_pcm_hw_params_set_buffer_size_near (playback_handle, hw_params, &bufferedframes)) < 0)
    {
        fprintf (stderr, "cannot set TX buffer size near (%s)\n", snd_strerror (err));
    }
    if(bufferedframes != BUFFER_FRAME_SIZE)
    {
    	printf("The Tx Buffer size is not Supported. using %ld instead\n", bufferedframes);
    }
#else
    /*if((err = snd_pcm_hw_params_get_buffer_size(hw_params, &bufferedframes)) == 0)
    {
    	printf("Tx Buffer size is %ld\n", bufferedframes);
    }
    else
    {
    	fprintf (stderr, "cannot get TX buffer size (%d: %s)\n", err, snd_strerror (err));
    }*/
    
    if( (err = snd_pcm_hw_params_set_buffer_time_near (playback_handle, hw_params, &BufferTime, &dir)) < 0)
	{
		fprintf (stderr, "cannot set TX buffer time near (%s)\n", snd_strerror (err));
	}
	if(BufferTime != AUDIO_LATENCY)
	{
		printf("The Tx Buffer time is not Supported. using %d instead\n", BufferTime);
	}


#endif
    //Now we apply the configuration to the PCM device pointed to by pcm_handle and prepare the PCM device.

    /* Apply HW parameter settings to PCM device and prepare device. */
    if ((err = snd_pcm_hw_params (playback_handle, hw_params)) < 0)
    {
        fprintf (stderr, "cannot set TX parameters (%s)\n", snd_strerror (err));
        exit (1);
    }

    if ((err = snd_pcm_prepare (playback_handle)) < 0)
    {
        fprintf (stderr, "cannot prepare TX audio interface for use (%s)\n", snd_strerror (err));
        exit (1);
    }
#else
    if ((err = snd_pcm_set_params(playback_handle,
    		SND_PCM_FORMAT_S16_LE,
			SND_PCM_ACCESS_RW_INTERLEAVED,
			2,
			8000,
			0,
			50000)) < 0) /* 0.05sec */
    {
            printf("Playback open error: %s\n", snd_strerror(err));
            exit(EXIT_FAILURE);
    }
#endif
    snd_pcm_start(playback_handle);

    InitCircBufferS16(&WorkingPlaybackBuffer[AUDIO_CHANNEL_HANDSET], TRUE);
	InitCircBufferS16(&WorkingPlaybackBuffer[AUDIO_CHANNEL_POTS], TRUE);

    /***************************************************************************************************************/
    /***************************************************************************************************************/
    /***************************************************************************************************************/
    /***************************************************************************************************************/
    /***************************************************************************************************************/
    /***************************************************************************************************************/
    /***************************************************************************************************************/
    /***************************************************************************************************************/
    /***************************************************************************************************************/
    while(AudioTxThread_KeepGoing)
    {
        if(snd_pcm_wait(playback_handle, PCM_WAIT_TIME) == 1)
        {
			/* Write some junk data to produce sound. */
        	if ((err = snd_pcm_writei (playback_handle, Txbuf, EffectivePeriodSizeTx)) != EffectivePeriodSizeTx)
			{
				if(err == -EPIPE)
				{
					//underrrun
					fprintf (stderr, "write Underrun (%s)\n", snd_strerror (err));
					snd_pcm_prepare(playback_handle);
				}
				else if (err < 0)
				{
					fprintf (stderr, "write to audio interface failed (%d: %s)\nGood Count: %d\n", err, snd_strerror (err), TxGoodCount);
					int frames = snd_pcm_recover(playback_handle, err, 0);
					if(frames < 0)
					{
						fprintf (stderr, "recover write to audio interface failed (%d: %s)\n", frames, snd_strerror (frames));
						//exit (1);
					}
				}
				else
				{
					printf("Non-Period length write: %d instead of %d\n", err, PERIOD_LEN);
				}
			}
			else
			{
				if(TxAudioProcessingThread != 0)
				{
					TxGoodCount++;
					sem_post(&AudioTxSem);
				}
			}
		}
        else
        {
        	TxBadCount++;
        	//printf("snd_pcm_wait Playback timed out with no writes pending\n");
        	/*int avail_frames = snd_pcm_avail(playback_handle);
			if(avail_frames > 0)
			{
				printf("Available frames for Tx: %d\n", avail_frames);
			}

			if(TxBadCount > 100)
			{
				TxBadCount = 0;
				if ((err = snd_pcm_start (playback_handle)) < 0)
				{
					fprintf (stderr, "cannot prepare TX audio interface for use (%s)\n", snd_strerror (err));
					exit (1);
				}
				printf("Tx is in state: %d\n", snd_pcm_state(playback_handle));
			}*/
        }
    }

	//snd_pcm_drain(playback_handle);
    snd_pcm_hw_params_free (hw_params);
    snd_pcm_close (playback_handle);

    return 0;
}

/********************************************************************************************************************************
 *  void function(void)
 *
 *  Desc
 *
 *  Parameters:
 *      - none
 *
 *  Returns:  none
 *
 *  Notes:  notes
 ********************************************************************************************************************************/
void *ProcessRxAudioSamples(void *arg)
{
	int i;
	int index = 0;
	int avg1 = 0;
	int avg2 = 0;

	while(RxAudioProcessingThread_KeepGoing)
	{
		sem_wait(&AudioRxSem);

		index = 0;
		//separate out the audio samples, they come interlaced which doesn't help us process the data.
        for(i = 0; i < (EffectivePeriodSizeTx*NUM_OF_CHANNELS); i+=2)
		{
			avg1 += Rxbuf[i+1];
			avg2 += Rxbuf[i];
			DeinterlaceRxdBuf[AUDIO_CHANNEL_HANDSET][index] = Rxbuf[i];
			DeinterlaceRxdBuf[AUDIO_CHANNEL_POTS][index++] = Rxbuf[i+1];
		}

		AvgRxInputPOTS = avg1/(PERIOD_LEN);
		AvgRxInputMic = avg2/(PERIOD_LEN);

        HandleCircBufS16PushPull(&WorkingCaptureBuffer[AUDIO_CHANNEL_HANDSET],
				DeinterlaceRxdBuf[AUDIO_CHANNEL_HANDSET],
				EffectivePeriodSizeRx,
				TRUE);

        HandleCircBufS16PushPull(&WorkingCaptureBuffer[AUDIO_CHANNEL_POTS],
				DeinterlaceRxdBuf[AUDIO_CHANNEL_POTS],
				EffectivePeriodSizeRx,
				TRUE);


        //special situations that affect the samples coming in
		while(GetCircBufS16DataRemaining(&WorkingCaptureBuffer[AUDIO_CHANNEL_HANDSET]) >= PERIOD_LEN)
		{
			HandleCircBufS16PushPull(&WorkingCaptureBuffer[AUDIO_CHANNEL_HANDSET],
					RawLiveCallHSSamples,
					EffectivePeriodSizeTx,
					FALSE);

			//if((g_hook_sw_status != ONHOOK))
			{
				HandleCircBufS16PushPull(&WorkingPlaybackBuffer[AUDIO_CHANNEL_POTS],
						RawLiveCallHSSamples,
						EffectivePeriodSizeTx,
						TRUE);
			}
		}

		while(GetCircBufS16DataRemaining(&WorkingCaptureBuffer[AUDIO_CHANNEL_POTS]) >= PERIOD_LEN)
		{
			HandleCircBufS16PushPull(&WorkingCaptureBuffer[AUDIO_CHANNEL_POTS],
					RawLiveCallPOTSSamples,
					EffectivePeriodSizeTx,
					FALSE);
			//if((g_hook_sw_status != ONHOOK))
			{
				HandleCircBufS16PushPull(&WorkingPlaybackBuffer[AUDIO_CHANNEL_HANDSET],
						RawLiveCallPOTSSamples,
						EffectivePeriodSizeTx,
						TRUE);
			}
		}
	}

	return 0;
}

/********************************************************************************************************************************
 *  void function(void)
 *
 *  Desc
 *
 *  Parameters:
 *      - none
 *
 *  Returns:  none
 *
 *  Notes:  notes
 ********************************************************************************************************************************/
void *ProcessTxAudioSamples(void *arg)
{
	int i;
	int index = 0;
	int avg1 = 0;
	int avg2 = 0;

	while(TxAudioProcessingThread_KeepGoing)
	{
		sem_wait(&AudioTxSem);

		//blank output by default
		memset(DeinterlaceTxdBuf[AUDIO_CHANNEL_HANDSET], 0, sizeof(DeinterlaceTxdBuf[AUDIO_CHANNEL_HANDSET]));
		memset(DeinterlaceTxdBuf[AUDIO_CHANNEL_POTS], 0, sizeof(DeinterlaceTxdBuf[AUDIO_CHANNEL_POTS]));

		//processing...
        if(GetCircBufS16DataRemaining(&WorkingPlaybackBuffer[AUDIO_CHANNEL_HANDSET]) >= EffectivePeriodSizeTx)
		{
			HandleCircBufS16PushPull(&WorkingPlaybackBuffer[AUDIO_CHANNEL_HANDSET],
					DeinterlaceTxdBuf[AUDIO_CHANNEL_HANDSET],
					EffectivePeriodSizeTx,
					FALSE);
		}

		if(GetCircBufS16DataRemaining(&WorkingPlaybackBuffer[AUDIO_CHANNEL_POTS]) >= EffectivePeriodSizeTx)
		{
			HandleCircBufS16PushPull(&WorkingPlaybackBuffer[AUDIO_CHANNEL_POTS],
					DeinterlaceTxdBuf[AUDIO_CHANNEL_POTS],
					EffectivePeriodSizeTx,
					FALSE);
		}
        //re-interlace the audio samples for the tx system
        index = 0;
        for(i = 0; i < (EffectivePeriodSizeTx*NUM_OF_CHANNELS); i+=2)
		{
			Txbuf[i] = DeinterlaceTxdBuf[AUDIO_CHANNEL_HANDSET][index];
			Txbuf[i+1] = DeinterlaceTxdBuf[AUDIO_CHANNEL_POTS][index++];
			avg1 += Txbuf[i+1];
			avg2 += Txbuf[i];
		}

		AvgTxOutputPOTS = avg1/(PERIOD_LEN);
		AvgTxOutputHS = avg2/(PERIOD_LEN);
	}

	return 0;
}



/*******************************************************************************************************************************
 * void ()
 *
 * processes
 *
 * Parameters:
 *       -None
 *
 * Returns: None
 *
 * Notes:
 *
 ******************************************************************************************************************************/
BOOL isCircBufS16Empty(DataPipeS16_t *p_CircBuffer)
{
    return (p_CircBuffer->tail == p_CircBuffer->head);
}

/*******************************************************************************************************************************
 * void ()
 *
 * processes
 *
 * Parameters:
 *       -None
 *
 * Returns: None
 *
 * Notes:
 *
 ******************************************************************************************************************************/
U32 CircBufS16Freespace(DataPipeS16_t *p_CircBuffer)
{
    U32 freespace = 0;

    //check if have enough room
    if(p_CircBuffer->head >= p_CircBuffer->tail)
    {
        freespace = (DEFAULT_CIRC_BUF_LEN - 1) - (p_CircBuffer->head - p_CircBuffer->tail);
    }
    else
    {
        freespace = (p_CircBuffer->tail - p_CircBuffer->head) - 1;
    }

    return freespace;
}

/*******************************************************************************************************************************
 * void ()
 *
 * processes
 *
 * Parameters:
 *       -None
 *
 * Returns: None
 *
 * Notes:
 *
 ******************************************************************************************************************************/
U32 GetCircBufS16DataRemaining(DataPipeS16_t *p_CircBuffer)
{
	return (DEFAULT_CIRC_BUF_LEN - CircBufS16Freespace(p_CircBuffer));
}

/*******************************************************************************************************************************
 * void ()
 *
 * processes
 *
 * Parameters:
 *       -None
 *
 * Returns: None
 *
 * Notes:
 *
 ******************************************************************************************************************************/
BOOL InitCircBufferS16(DataPipeS16_t *p_CircBuffer, BOOL init_mutex)
{
    BOOL retVal = TRUE;

    memset(p_CircBuffer->data, 0, DEFAULT_CIRC_BUF_LEN*2);
    p_CircBuffer->head = 0;
    p_CircBuffer->tail = 0;


    if(init_mutex && (p_CircBuffer->MutexInit == FALSE))
    {
        if (pthread_mutex_init(&p_CircBuffer->mutex, NULL) != 0)
        {
            printf("p_CircBuffer mutex init has failed\n");
            retVal = FALSE;
            p_CircBuffer->MutexInit = FALSE;
        }
        else
        {
            p_CircBuffer->MutexInit = TRUE;
        }
    }

    return retVal;
}

/*******************************************************************************************************************************
 * void ()
 *
 * processes
 *
 * Parameters:
 *       -None
 *
 * Returns: None
 *
 * Notes:
 *
 ******************************************************************************************************************************/
void KillCircBufS16Mutex(DataPipeS16_t *p_CircBuffer)
{
    if(p_CircBuffer->MutexInit)
    {
        pthread_mutex_destroy(&p_CircBuffer->mutex);
        p_CircBuffer->MutexInit = FALSE;
    }
}

/*******************************************************************************************************************************
 * void ()
 *
 * processes
 *
 * Parameters:
 *       -None
 *
 * Returns: None
 *
 * Notes:
 *
 ******************************************************************************************************************************/
U32 HandleCircBufS16PushPull(DataPipeS16_t *p_CircBuffer, S16 *p_buf, U32 len, BOOL push)
{
    U32 retVal = 0;

    if((p_CircBuffer != NULL) && (p_buf != NULL) && (len > 0))
    {
        if(p_CircBuffer->MutexInit == TRUE)
        {
            pthread_mutex_lock(&p_CircBuffer->mutex);
        }

        ///////////////////////////////////////////////////////////////////////
        //attempt to add if push
        if(push)
        {
            U32 freespace = 0;

            //check if have enough room
            freespace = CircBufS16Freespace(p_CircBuffer);

            if(freespace < len)
            {
                printf("Changing Length because freespace: want = %d, giving = %d\n", len, freespace);
                len = 0;//freespace;
            }

            retVal = len;

            if(len > 0)
            {
                if((len + p_CircBuffer->head) >= DEFAULT_CIRC_BUF_LEN)
                {
                    U32 newLen = DEFAULT_CIRC_BUF_LEN - p_CircBuffer->head;
                    U32 remainder = len - newLen;

                    memcpy(&p_CircBuffer->data[p_CircBuffer->head], p_buf, newLen*2);
                    p_CircBuffer->head = 0;

                    if(remainder)
                    {
                        memcpy(&p_CircBuffer->data[p_CircBuffer->head], &p_buf[newLen], remainder*2);
                        p_CircBuffer->head = remainder;
                    }
                }
                else
                {
                    memcpy(&p_CircBuffer->data[p_CircBuffer->head], p_buf, len*2);
                    p_CircBuffer->head += len;
                }
            }
            else
            {
                printf("No Freespace Available S16\n");
            }
        }
        //////////////////////////////////////////////////////////
        // attempt to take out if pull
        else
        {
            if(p_CircBuffer->head != p_CircBuffer->tail)    //not empty
            {
                if(p_CircBuffer->tail < p_CircBuffer->head)
                {
                    if(p_CircBuffer->tail + len > p_CircBuffer->head) //make sure not trying to pull more than we have
                    {
                        len = p_CircBuffer->head - p_CircBuffer->tail;
                    }

                    memcpy(p_buf, &p_CircBuffer->data[p_CircBuffer->tail], len*2);
                    p_CircBuffer->tail += len;
                    p_CircBuffer->tail &= (DEFAULT_CIRC_BUF_LEN-1);
                    retVal = len;
                }
                else    //wrap around potential
                {
                    if(p_CircBuffer->tail + len >= DEFAULT_CIRC_BUF_LEN) //wrapping
                    {
                        U32 newLen = DEFAULT_CIRC_BUF_LEN - p_CircBuffer->tail;
                        U32 remainder = len - newLen;

                        if(remainder > p_CircBuffer->head)
                        {
                            remainder = p_CircBuffer->head;
                        }

                        retVal = newLen + remainder;

                        memcpy(p_buf, &p_CircBuffer->data[p_CircBuffer->tail], newLen*2);
                        p_CircBuffer->tail = 0;

                        if(remainder)
                        {
                            memcpy(&p_buf[newLen], &p_CircBuffer->data[p_CircBuffer->tail], remainder*2);
                            p_CircBuffer->tail = remainder;
                        }
                    }
                    else
                    {
                        memcpy(p_buf, &p_CircBuffer->data[p_CircBuffer->tail], len*2);
                        p_CircBuffer->tail += len;
                        p_CircBuffer->tail &= (DEFAULT_CIRC_BUF_LEN-1);
                        retVal = len;
                    }
                }
            }
        }

        if(p_CircBuffer->MutexInit == TRUE)
        {
            pthread_mutex_unlock(&p_CircBuffer->mutex);
        }

    }

    return retVal;
}
