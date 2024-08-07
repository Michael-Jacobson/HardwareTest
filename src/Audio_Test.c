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
#include "Codec.h"

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

#define AUDIO_LATENCY			50000



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

int AveragesTX[2] = {0, 0};
int AveragesRX[2] = {0, 0};

/* Handle for the PCM device */
snd_pcm_t *playback_handle;
snd_pcm_t *capture_handle;
short Rxbuf[BUFF_SIZE];
short Txbuf[BUFF_SIZE];
short DeinterlaceRxdBuf[2][PERIOD_LEN*2]; //0 is handset, 1 is Phone line
short DeinterlaceTxdBuf[2][PERIOD_LEN*2]; //0 is handset, 1 is phone line

short RawLiveCallPOTSSamples[PERIOD_LEN*2];
short RawLiveCallHSSamples[PERIOD_LEN*2];

short RawLiveCallPOTSTxSamples[PERIOD_LEN*2];
short RawLiveCallHSTxSamples[PERIOD_LEN*2];

BOOL TxRunning = FALSE;

int POTS_Sample_Index = PHONELINE;
int HS_Sample_Index = HANDSET;

static void *ReadAudioSamples(void *arg);
static void *WriteAudioSamples(void *arg);
static void *ProcessRxAudioSamples(void *arg);
static void *ProcessTxAudioSamples(void *arg);

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

    pthread_join(RxAudioProcessingThread, NULL);

    pthread_join(AudioTxThread, NULL);

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
    int dir = 0;
    //snd_pcm_uframes_t bufferedframes = BUFFER_FRAME_SIZE;
	unsigned int BufferTime = AUDIO_LATENCY;
	snd_pcm_uframes_t FrameSize = PERIOD_LEN;
	
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
    err = snd_pcm_hw_params_set_period_size_near(capture_handle, hw_params, &FrameSize, &dir);
    if (err < 0)
    {
        fprintf (stderr, "cannot set RX channel period (%s)\n", snd_strerror (err));
    }
    if (FrameSize != PERIOD_LEN)
    {
        fprintf(stderr, "The RX period %d is not supported by your hardware.\n ==> Wants %ld instead. Exiting\n", PERIOD_LEN, FrameSize);
        exit (1);
    }

    /* Set number of channels */
    if ((err = snd_pcm_hw_params_set_channels(capture_handle, hw_params, NUM_OF_CHANNELS)) < 0)
    {
        fprintf (stderr, "cannot set RX channel count (%s)\n", snd_strerror (err));
        exit (1);
    }


	if( (err = snd_pcm_hw_params_set_buffer_time_near (capture_handle, hw_params, &BufferTime, &dir)) < 0)
	{
		fprintf (stderr, "cannot set RX buffer time near (%s)\n", snd_strerror (err));
	}
	if(BufferTime != AUDIO_LATENCY)
	{
		printf("The Rx Buffer time is not Supported. using %d instead\n", BufferTime);
	}

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

    /*
     * we need playback to start before
     * the capture because the ssi renesas patch says it needs to. we also need to start them at the same time
     * or else the interleaved samples where we assume odd samples are path1, even samples are path2 for both
     * rx and tx won't be true. it will be random and sometimes it will work out and sometimes it will be
     * opposite
     */
    while(!TxRunning)
    {
    	usleep(1);
    }

    snd_pcm_start(capture_handle);

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
			/* Read data into the buffer. */
			if ((err = snd_pcm_readi (capture_handle, Rxbuf, PERIOD_LEN)) != PERIOD_LEN)
			{
				printf("Number of Samples trying to Read: %d\nNumber of Samples actually Read: %d\n", (PERIOD_LEN), err);
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
				//printf("Captures %d\n", RxGoodCount);
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
    int dir = 0;
    /* Playback stream */
    snd_pcm_stream_t stream = SND_PCM_STREAM_PLAYBACK;
    /* This structure contains information about the hardware and can be used to specify the configuration to be used for */
    /* the PCM stream. */
    snd_pcm_hw_params_t *hw_params;
    //snd_pcm_uframes_t bufferedframes = BUFFER_FRAME_SIZE;
	unsigned int BufferTime = AUDIO_LATENCY;
	snd_pcm_uframes_t FrameSize = PERIOD_LEN;
    /* Name of the PCM device, like plughw:0,0 */
    /* The first number is the number of the soundcard, the second number is the number of the device. */
    static char *device = "default"; /* playback device */

    /* Open PCM. The last parameter of this function is the mode. */
    if ((err = snd_pcm_open (&playback_handle, device, stream, 0)) < 0)
    {
        fprintf (stderr, "cannot open TX audio device (%s)\n", snd_strerror (err));
        exit (1);
    }

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
    err = snd_pcm_hw_params_set_period_size_near(playback_handle, hw_params, &FrameSize, &dir);
    if (err < 0)
    {
        fprintf (stderr, "cannot set TX channel period (%s)\n", snd_strerror (err));
    }
    if (FrameSize != (PERIOD_LEN))
    {
        fprintf(stderr, "The TX period %d is not supported by your hardware.\n ==> Wants %ld instead. Exiting\n", (PERIOD_LEN), FrameSize);
        exit (1);
    }

    /* Set number of channels */
    if ((err = snd_pcm_hw_params_set_channels (playback_handle, hw_params, NUM_OF_CHANNELS)) < 0)
    {
        fprintf (stderr, "cannot set TX channel count (%s)\n", snd_strerror (err));
        exit (1);
    }

    if( (err = snd_pcm_hw_params_set_buffer_time_near (playback_handle, hw_params, &BufferTime, &dir)) < 0)
	{
		fprintf (stderr, "cannot set TX buffer time near (%s)\n", snd_strerror (err));
	}
    if(BufferTime != AUDIO_LATENCY)
	{
		printf("The Tx Buffer time is not Supported. using %d instead\n", BufferTime);
	}

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

    snd_pcm_start(playback_handle);

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
			if ((err = snd_pcm_writei (playback_handle, Txbuf, PERIOD_LEN)) != PERIOD_LEN)
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
					TxRunning = TRUE;
					TxGoodCount++;
					sem_post(&AudioTxSem);
				}
			}
		}
        else
        {
        	TxBadCount++;
        }
    }

    TxRunning = FALSE;
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
	int RunOnce = 0;

	while(RxAudioProcessingThread_KeepGoing)
	{
		sem_wait(&AudioRxSem);

		index = 0;
		avg1 = 0;
		avg2 = 0;

		//separate out the audio samples, they come interlaced which doesn't help us process the data.
		for(i = 0; i < (PERIOD_LEN*NUM_OF_CHANNELS); i+=2)
		{
			avg1 += Rxbuf[i+1];
			avg2 += Rxbuf[i];


			DeinterlaceRxdBuf[0][index] = Rxbuf[i];
			DeinterlaceRxdBuf[1][index++] = Rxbuf[i+1];
		}

		AveragesRX[1] = avg1/(PERIOD_LEN);
		AveragesRX[0] = avg2/(PERIOD_LEN);

		if(RunOnce <= 10)
		{
			RunOnce++;
			/*
			 * the samples for POTS should have an average because the HS is muted at boot.
			 * the drivers did a bad job synchronizing the samples to the channels. sometimes
			 * the samples start with POTS, sometimes it starts with Handset. there is no way
			 * to know which is which. so we have to play this averages game to figure it out
			 */
			if((AveragesRX[PHONELINE] == 0) && (AveragesRX[HANDSET] != 0))
			{
				printf("Switching Inputs on #%d!\n", RunOnce);
				POTS_Sample_Index = HANDSET;
				HS_Sample_Index = PHONELINE;
				RunOnce = 11;
			}
		}
		else
		{
			RunOnce++;
		}
		memcpy(RawLiveCallHSSamples, DeinterlaceRxdBuf[HS_Sample_Index], (PERIOD_LEN*2));
		memcpy(RawLiveCallPOTSSamples, DeinterlaceRxdBuf[POTS_Sample_Index], (PERIOD_LEN*2));

        //if(g_hook_sw_status != ONHOOK)
        {
        	memcpy(RawLiveCallPOTSTxSamples, RawLiveCallHSSamples, (PERIOD_LEN*2));
        	memcpy(RawLiveCallHSTxSamples, RawLiveCallPOTSSamples, (PERIOD_LEN*2));
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
        memset(DeinterlaceTxdBuf, 0, sizeof(DeinterlaceTxdBuf));

        ////////////////////
        //Tx Processing here
        //if(g_hook_sw_status != ONHOOK)
        {
			memcpy(DeinterlaceTxdBuf[PHONELINE], RawLiveCallPOTSTxSamples, (PERIOD_LEN*2));
			memcpy(DeinterlaceTxdBuf[HANDSET], RawLiveCallHSTxSamples, (PERIOD_LEN*2));
        }

        index = 0;
        avg1 = 0;
        avg2 = 0;

        //re-interlace the audio samples for the tx system
        for(i = 0; i < (PERIOD_LEN*NUM_OF_CHANNELS); i+=2)
        {
            Txbuf[i] = DeinterlaceTxdBuf[HANDSET][index];
            Txbuf[i+1] = DeinterlaceTxdBuf[PHONELINE][index++];
			avg1 += Txbuf[i+1];
			avg2 += Txbuf[i];
        }

        AveragesTX[PHONELINE] = avg1/(PERIOD_LEN);
        AveragesTX[HANDSET] = avg2/(PERIOD_LEN);
    }


    return 0;
}
