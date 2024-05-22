/*
 * Codec_Test.c
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
#include <semaphore.h>

#include "C674xTypes.h"
#include "Ultratec_Enums.h"
#include "Codec.h"

#define AUDIO_CHANNEL_LEFT		     0
#define AUDIO_CHANNEL_RIGHT          1


#define AUDIO_CHANNEL_HANDSET   AUDIO_CHANNEL_LEFT
#define AUDIO_CHANNEL_POTS      AUDIO_CHANNEL_RIGHT

#define MAX_CODEC_VOLUME    118

//inputs
const char *selem_name_R_PGA_Mic3L = "Right PGA Mixer Mic3L Switch";
const char *selem_name_R_PGA_Mic3R = "Right PGA Mixer Mic3R Switch";
const char *selem_name_R_PGA_Line1L = "Right PGA Mixer Line1L Switch";
const char *selem_name_R_PGA_Line1R = "Right PGA Mixer Line1R Switch";
const char *selem_name_R_PGA_Line2R = "Right PGA Mixer Line2R Switch";
const char *selem_name_L_PGA_Line1L = "Left PGA Mixer Line1L Switch";
const char *selem_name_L_PGA_Line1R = "Left PGA Mixer Line1R Switch";
const char *selem_name_L_PGA_Line2L = "Left PGA Mixer Line2L Switch";
const char *selem_name_L_PGA_Mic3L = "Left PGA Mixer Mic3L Switch";
const char *selem_name_L_PGA_Mic3R = "Left PGA Mixer Mic3R Switch";

//outputs
const char *selem_name_Mono_DACL1 = "Mono Mixer DACL1 Switch";
const char *selem_name_Mono_DACR1 = "Mono Mixer DACR1 Switch";
const char *selem_name_LeftHP_DACL1 = "Left HP Mixer DACL1 Switch";
const char *selem_name_LeftHP_DACR1 = "Left HP Mixer DACR1 Switch";
const char *selem_name_LeftHPCOM_DACL1 = "Left HPCOM Mixer DACL1 Switch";
const char *selem_name_LeftHPCOM_DACR1 = "Left HPCOM Mixer DACR1 Switch";
const char *selem_name_LeftLine_DACL1 = "Left Line Mixer DACL1 Switch";
const char *selem_name_LeftLine_DACR1 = "Left Line Mixer DACR1 Switch";
const char *selem_name_RightHP_DACL1 = "Right HP Mixer DACL1 Switch";
const char *selem_name_RightHP_DACR1 = "Right HP Mixer DACR1 Switch";
const char *selem_name_RightHPCOM_DACL1 = "Right HPCOM Mixer DACL1 Switch";
const char *selem_name_RightHPCOM_DACR1 = "Right HPCOM Mixer DACR1 Switch";
const char *selem_name_RightLine_DACL1 = "Right Line Mixer DACL1 Switch";
const char *selem_name_RightLine_DACR1 = "Right Line Mixer DACR1 Switch";

const char *card = "default";
const char *selem_name_PCM = "PCM";
const char *selem_name_PGA = "PGA";

pthread_t CodecControlThread = 0;
sem_t CodecControlSem;

BOOL CodecControlThread_KeepGoing = TRUE;

int CodecPendingGain = 0;
BOOL CodecInProcess = TRUE;

CodecControlStates_t CodecControlState = CODEC_INIT;

void InitSoundDrivers(void);
void CloseSoundDrivers(void);

void InitCodec(void);
void CloseCodec(void);
void SelectHandset(void);
static BOOL SetCodecOutputGain(snd_mixer_selem_id_t *sid, snd_mixer_t *handle, snd_mixer_selem_channel_id_t channel, long Gain);
static BOOL SetCodecInputGain(snd_mixer_selem_id_t *sid, snd_mixer_t *handle, snd_mixer_selem_channel_id_t channel, long Gain);
static BOOL SetCodecMixerSetting(snd_hctl_t *hctl, snd_ctl_elem_id_t *id, snd_ctl_elem_value_t *control, int index, const char *MixerSetting, long value);

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
void InitCodec(void)
{
	pthread_create( &CodecControlThread, NULL, CodecControl, NULL );
	sem_init(&CodecControlSem, 0, 0);
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
void CloseCodec(void)
{
	CodecControlThread_KeepGoing = FALSE;
	sem_post(&CodecControlSem);
	pthread_join(CodecControlThread, NULL);
	sem_destroy(&CodecControlSem);
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
void *CodecControl(void *arg)
{
    //BOOL KeepGoing = TRUE;
    snd_hctl_t *hctl = NULL;
    snd_ctl_elem_id_t *id = NULL;
    snd_ctl_elem_value_t *control = NULL;

    int err;

    snd_mixer_t *handle = NULL;
    snd_mixer_selem_id_t *sid = NULL;
    int i;
    const char *selem_name_inputs[] = {
                                 selem_name_R_PGA_Mic3L,
                                 selem_name_R_PGA_Mic3R,
                                 selem_name_R_PGA_Line1L,
                                 selem_name_R_PGA_Line1R,
                                 selem_name_R_PGA_Line2R,
                                 selem_name_L_PGA_Line1L,
                                 selem_name_L_PGA_Line1R,
                                 selem_name_L_PGA_Line2L,
                                 selem_name_L_PGA_Mic3L,
                                 selem_name_L_PGA_Mic3R,
                                 NULL
    };

    const char *selem_name_outputs[] = {
                                  selem_name_Mono_DACL1,
                                  selem_name_Mono_DACR1,
                                  selem_name_LeftHP_DACL1,
                                  selem_name_LeftHP_DACR1,
                                  selem_name_LeftHPCOM_DACL1,
                                  selem_name_LeftHPCOM_DACR1,
                                  selem_name_LeftLine_DACL1,
                                  selem_name_LeftLine_DACR1,
                                  selem_name_RightHP_DACL1,
                                  selem_name_RightHP_DACR1,
                                  selem_name_RightHPCOM_DACL1,
                                  selem_name_RightHPCOM_DACR1,
                                  selem_name_RightLine_DACL1,
                                  selem_name_RightLine_DACR1,
                                  NULL
    };

    while(CodecControlThread_KeepGoing)
    {
        switch(CodecControlState)
        {
            case CODEC_INIT:
                CodecControlState = CODEC_SHUT_DOWN;    //FYI

                err = snd_hctl_open(&hctl, card, 0);

                if(err < 0)
                {
                    printf(" snd_hctl_open fail: %d\n", err);
                }
                else
                {
                    err = snd_hctl_load(hctl);
                    if(err < 0)
                    {
                        printf(" snd_hctl_load fail: %d\n", err);
                    }
                    else
                    {
                        //routing
                        snd_ctl_elem_id_alloca(&id);
                        snd_ctl_elem_id_set_interface(id, SND_CTL_ELEM_IFACE_MIXER);
                        snd_ctl_elem_value_alloca(&control);

                        //audio levels
                        snd_mixer_open(&handle, 0);
                        snd_mixer_attach(handle, card);
                        snd_mixer_selem_register(handle, NULL, NULL);
                        snd_mixer_load(handle);

                        snd_mixer_selem_id_alloca(&sid);
                        snd_mixer_selem_id_set_index(sid, 0);

                        //set defaults
                        if((id != NULL) && (hctl != NULL) && (control != NULL) && (handle != NULL) && (sid != NULL))
                        {
                            //disable all inputs
                            i = 0;
                            while(selem_name_inputs[i] != NULL)
                            {
                                SetCodecMixerSetting(hctl, id, control, AUDIO_CHANNEL_LEFT, selem_name_inputs[i], 0);

                                i++;
                            }

                            //disable all outputs
                            i = 0;
                            while(selem_name_outputs[i] != NULL)
                            {
                                SetCodecMixerSetting(hctl, id, control, AUDIO_CHANNEL_LEFT, selem_name_outputs[i], 0);

                                i++;
                            }

                            //mute and turn off
                            SetCodecMixerSetting(hctl, id, control, AUDIO_CHANNEL_LEFT, "Line Line2 Bypass Volume", 0);
                            SetCodecMixerSetting(hctl, id, control, AUDIO_CHANNEL_RIGHT, "Line Line2 Bypass Volume", 0);

                            SetCodecMixerSetting(hctl, id, control, AUDIO_CHANNEL_LEFT, "Line PGA Bypass Volume", 0);
                            SetCodecMixerSetting(hctl, id, control, AUDIO_CHANNEL_RIGHT, "Line PGA Bypass Volume", 0);

                            SetCodecMixerSetting(hctl, id, control, AUDIO_CHANNEL_LEFT, "Mono Line2 Bypass Volume", 0);
                            SetCodecMixerSetting(hctl, id, control, AUDIO_CHANNEL_RIGHT, "Mono Line2 Bypass Volume", 0);

                            SetCodecMixerSetting(hctl, id, control, AUDIO_CHANNEL_LEFT, "Mono PGA Bypass Volume", 0);
                            SetCodecMixerSetting(hctl, id, control, AUDIO_CHANNEL_RIGHT, "Mono PGA Bypass Volume", 0);

                            SetCodecMixerSetting(hctl, id, control, 0, "Left Line Mixer Line2R Bypass Volume", 0);
                            SetCodecMixerSetting(hctl, id, control, 0, "Left Line Mixer Line2R Bypass Switch", 0);

                            SetCodecMixerSetting(hctl, id, control, 0, "Left Line Mixer PGAR Bypass Volume", 0);
                            SetCodecMixerSetting(hctl, id, control, 0, "Left Line Mixer PGAR Bypass Switch", 0);

                            SetCodecMixerSetting(hctl, id, control, 0, "Right Line Mixer Line2L Bypass Volume", 0);
                            SetCodecMixerSetting(hctl, id, control, 0, "Right Line Mixer Line2L Bypass Switch", 0);

                            SetCodecMixerSetting(hctl, id, control, 0, "Right Line Mixer PGAL Bypass Volume", 0);
                            SetCodecMixerSetting(hctl, id, control, 0, "Right Line Mixer PGAL Bypass Switch", 0);

                            //pick the output we want to NOT mute and set to 100%, we only want 1 volume control
                            SetCodecMixerSetting(hctl, id, control, AUDIO_CHANNEL_LEFT, "Line DAC Playback Volume", MAX_CODEC_VOLUME);
                            SetCodecMixerSetting(hctl, id, control, AUDIO_CHANNEL_RIGHT, "Line DAC Playback Volume", MAX_CODEC_VOLUME);

                            SetCodecMixerSetting(hctl, id, control, AUDIO_CHANNEL_LEFT, "Mono DAC Playback Volume", MAX_CODEC_VOLUME);
                            SetCodecMixerSetting(hctl, id, control, AUDIO_CHANNEL_RIGHT, "Mono DAC Playback Volume", MAX_CODEC_VOLUME);

                            //make any inputs/outputs from single ended to differential
                            /*Simple mixer control 'something',0
                              Capabilities: enum
                              Items: 'single-ended' 'differential'
                              Item0: 'single-ended' */
                            SetCodecMixerSetting(hctl, id, control, 0, "Left Line1L Mux", 1); //handset Mic
                            //SetCodecMixerSetting(hctl, id, control, 0, "Left Line2L Mux", 1); //Ext mic
                            SetCodecMixerSetting(hctl, id, control, 0, "Right Line2R Mux", 1); //POTS in

                            //turn on default paths

                            //turn on phone line in and out
                            //in
                            SetCodecMixerSetting(hctl, id, control, 0, selem_name_R_PGA_Line2R, 1);

                            //out
                            SetCodecMixerSetting(hctl, id, control, 0, selem_name_Mono_DACR1, 1);

                            //turn on handset spkr/mic
                            //in
                            SetCodecMixerSetting(hctl, id, control, 0, selem_name_L_PGA_Line1L, 1);

                            //out
                            SetCodecMixerSetting(hctl, id, control, 0, selem_name_LeftLine_DACL1, 1);


                            //now set up default gain values
                            SetCodecInputGain(sid, handle, AUDIO_CHANNEL_HANDSET, ADC_MUTE);
                            SetCodecOutputGain(sid, handle, AUDIO_CHANNEL_HANDSET,  DAC_MUTE);
                            SetCodecInputGain(sid, handle, AUDIO_CHANNEL_POTS, ADC_PLUS_0DB);
                            SetCodecOutputGain(sid, handle, AUDIO_CHANNEL_POTS,  DAC_MINUS_0DB);


                            CodecControlState = CODEC_IDLE;

                            //audio - launches threads
                            printf("Init Sound\n");
                            InitSoundDrivers();

                            //printf("Codec Init Complete\n");
                        }
                        else
                        {
                            CodecControlThread_KeepGoing = FALSE;
                        }
                    }
                }

            break;

            case CODEC_IDLE:
                CodecInProcess = FALSE;
                sem_wait(&CodecControlSem);

                //printf("Leaving Idle for: %d and gain %d\n", CodecControlState, CodecPendingGain);

                if(CodecControlThread_KeepGoing == FALSE)
                {
                    CodecControlState = CODEC_SHUT_DOWN;
                }
            break;

            case CODEC_SWITCH_EXT_SPKR:
                //turn off handset speaker
                SetCodecMixerSetting(hctl, id, control, AUDIO_CHANNEL_LEFT, selem_name_LeftLine_DACL1, 0);

                //turn on external speaker
                SetCodecMixerSetting(hctl, id, control, AUDIO_CHANNEL_LEFT, selem_name_RightLine_DACL1, 1);

                CodecControlState = CODEC_IDLE;
            break;

            case CODEC_SWITCH_EXT_MIC:
                //turn off handset
                SetCodecMixerSetting(hctl, id, control, AUDIO_CHANNEL_LEFT, selem_name_L_PGA_Line1L, 0);

                //turn on external
                SetCodecMixerSetting(hctl, id, control, AUDIO_CHANNEL_LEFT, selem_name_L_PGA_Line2L, 1);

                CodecControlState = CODEC_IDLE;
            break;

            case CODEC_SWITCH_HANDSET_SPKR:
                //turn off external speaker
                SetCodecMixerSetting(hctl, id, control, AUDIO_CHANNEL_LEFT, selem_name_RightLine_DACL1, 0);

                //turn on handset speaker
                SetCodecMixerSetting(hctl, id, control, AUDIO_CHANNEL_LEFT, selem_name_LeftLine_DACL1, 1);

                CodecControlState = CODEC_IDLE;
            break;

            case CODEC_SWITCH_HANDSET_MIC:
                //turn off external
                SetCodecMixerSetting(hctl, id, control, AUDIO_CHANNEL_LEFT, selem_name_L_PGA_Line2L, 0);

                //turn on handset
                SetCodecMixerSetting(hctl, id, control, AUDIO_CHANNEL_LEFT, selem_name_L_PGA_Line1L, 1);

                CodecControlState = CODEC_IDLE;
            break;

            case CODEC_SET_USER_ADC:
                SetCodecInputGain(sid, handle, AUDIO_CHANNEL_HANDSET, CodecPendingGain);
                CodecControlState = CODEC_IDLE;
            break;

            case CODEC_SET_USER_DAC:
                SetCodecOutputGain(sid, handle, AUDIO_CHANNEL_HANDSET,  CodecPendingGain);
                CodecControlState = CODEC_IDLE;
            break;

            case CODEC_SET_POTS_ADC:
                SetCodecInputGain(sid, handle, AUDIO_CHANNEL_POTS, CodecPendingGain);
                CodecControlState = CODEC_IDLE;
            break;

            case CODEC_SET_POTS_DAC:
                SetCodecOutputGain(sid, handle, AUDIO_CHANNEL_POTS,  CodecPendingGain);
                CodecControlState = CODEC_IDLE;
            break;

            case CODEC_ENABLE_SIDETONE:
                CodecControlState = CODEC_IDLE;
            break;

            case CODEC_DISABLE_SIDETONE:
                CodecControlState = CODEC_IDLE;
            break;

            case CODEC_SHUT_DOWN:
                CodecControlState = CODEC_INVALID;
                printf("Doing Shut Down\n");
                CodecControlThread_KeepGoing = FALSE;
            break;

            default:
            break;
        }
    }

    printf("Close Sound\n");
	CloseSoundDrivers();

    if(hctl != NULL)
    {
        snd_hctl_close(hctl);
    }

    if(handle != NULL)
    {
        snd_mixer_close(handle);
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
void ChangeCodecState(CodecControlStates_t state, int gain)
{
    if((CodecControlState == CODEC_IDLE) && (CodecInProcess == FALSE))
    {
        switch(state)
        {
            case CODEC_SET_USER_ADC:
            case CODEC_SET_USER_DAC:
            case CODEC_SET_POTS_ADC:
            case CODEC_SET_POTS_DAC:
                CodecPendingGain = gain;
            break;

            default:
            break;
        }

        CodecInProcess = TRUE;
        CodecControlState = state;
        sem_post(&CodecControlSem);

        while((CodecInProcess == TRUE) && (CodecControlThread_KeepGoing))
        {
            usleep(1000);
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
static BOOL SetCodecMixerSetting(snd_hctl_t *hctl, snd_ctl_elem_id_t *id, snd_ctl_elem_value_t *control, int index, const char *MixerSetting, long value)
{
    BOOL retVal = FALSE;
    snd_hctl_elem_t *elem;


    snd_ctl_elem_id_set_name(id, MixerSetting);
    elem = snd_hctl_find_elem(hctl, id);

    //printf("attempting 2 CODEC_SWITCH_HANDSET_SPKR, elem: %d\n", elem);

    if(elem != 0)
    {
        snd_ctl_elem_value_set_id(control, id);

        snd_ctl_elem_value_set_integer(control, index, value);
        if(snd_hctl_elem_write(elem, control) == 0)
        {
            retVal = TRUE;
        }
        else
        {
            printf("Failed mixer setting %s: %d in channel %d\n", MixerSetting, (int)value, index);
        }
    }
    else
    {
        printf("elem NULL for %s in SetCodecMixerSetting on channel %d\n", MixerSetting, index);
    }

    return retVal;
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
static BOOL SetCodecInputGain(snd_mixer_selem_id_t *sid, snd_mixer_t *handle, snd_mixer_selem_channel_id_t channel, long Gain)
{
    BOOL retVal = FALSE;
    snd_mixer_elem_t *Mixer_elem = NULL;
    long min, max;

    snd_mixer_selem_id_set_name(sid, selem_name_PGA);
    Mixer_elem = snd_mixer_find_selem(handle, sid);

    if(Mixer_elem != NULL)
    {
        if(Gain == ADC_MUTE)
        {
            if(snd_mixer_selem_set_capture_switch(Mixer_elem, channel, 0) == 0)
            {
                retVal = TRUE;
            }
        }
        else
        {
            if(snd_mixer_selem_set_capture_switch(Mixer_elem, channel, 1) == 0)
            {
                if(snd_mixer_selem_get_capture_dB_range(Mixer_elem, &min, &max) == 0)
                {
                    if((Gain >= min) && (Gain <= max))
                    {
                        if (snd_mixer_selem_has_capture_volume(Mixer_elem))
                        {
                            if(snd_mixer_selem_set_capture_dB(Mixer_elem, channel, Gain, 1) == 0)
                            {
                                retVal = TRUE;
                            }
                        }
                    }
                }
            }
        }
    }

    if(!retVal)
    {
        printf("Failed to set ADC gain channel: %d, gain: %d\n", channel, (int)Gain);
    }

    return retVal;
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
static BOOL SetCodecOutputGain(snd_mixer_selem_id_t *sid, snd_mixer_t *handle, snd_mixer_selem_channel_id_t channel, long Gain)
{
    BOOL retVal = FALSE;
    snd_mixer_elem_t *Mixer_elem = NULL;
    long min, max;

    snd_mixer_selem_id_set_name(sid, selem_name_PCM);
    Mixer_elem = snd_mixer_find_selem(handle, sid);

    if(Mixer_elem != NULL)
    {
        if(Gain == DAC_MUTE)
        {
            if(snd_mixer_selem_set_playback_dB(Mixer_elem, channel, SND_CTL_TLV_DB_GAIN_MUTE, 1) == 0)
            {
                retVal = TRUE;
            }
        }
        else if(snd_mixer_selem_get_playback_dB_range(Mixer_elem, &min, &max) == 0)
        {
            if((Gain >= min) && (Gain <= max))
            {
                if(snd_mixer_selem_set_playback_dB(Mixer_elem, channel, Gain, 1) == 0)
                {
                    retVal = TRUE;
                }
            }
        }

    }

    if(!retVal)
    {
        printf("Failed to set DAC gain channel: %d, gain: %d\n", channel, (int)Gain);
    }

    return retVal;
}


/******************************************************************************/
/*  void SelectExternalSpeaker(void)                                          */
/*                                                                            */
/*  This routine enables the codec output for the Speakerphone speaker.       */
/*                                                                            */
/*  Parameters:  None                                                         */
/*                                                                            */
/*  Returns:  None                                                            */
/*                                                                            */
/*  Notes:  None                                                              */
/*                                                                            */
/******************************************************************************/
void SelectExternalSpeaker(BOOL AMUse)
{
   /* Point the DAC output to the external speaker, but keep it muted for now.
      Also, mute the ADC channel during this phase. */
    ChangeCodecState(CODEC_SWITCH_EXT_SPKR, 0);
    ChangeCodecState(CODEC_SET_USER_ADC, ADC_MUTE);
    ChangeCodecState(CODEC_SET_USER_DAC, DAC_MUTE);
}

/*******************************************************************************************************************************
 * void SelectExternalMic(void)
 *
 * desc
 *
 * Parameters:
 *       -None
 *
 * Returns: None
 *
 * Notes:
 *
 ******************************************************************************************************************************/
void SelectExternalMic(void)
{
    ChangeCodecState(CODEC_SWITCH_EXT_MIC, 0);

    ChangeCodecState(CODEC_SET_USER_ADC, ADC_PLUS_30DB);
    ChangeCodecState(CODEC_SET_USER_DAC, DAC_MUTE);
}

/*******************************************************************************************************************************
 * void SelectHandset(void)
 *
 * desc
 *
 * Parameters:
 *       -None
 *
 * Returns: None
 *
 * Notes:
 *
 ******************************************************************************************************************************/
void SelectHandset(void)
{
    ChangeCodecState(CODEC_SWITCH_HANDSET_MIC, 0);
    ChangeCodecState(CODEC_SWITCH_HANDSET_SPKR, 0);
}

/*******************************************************************************************************************************
 * Function
 *
 * Description
 *
 * Parameters:
 *       -None
 *
 * Returns: None
 *
 * Notes:
 *
 ******************************************************************************************************************************/
void SetDACGain(U8 WhichDAC, int GainValue)
{
    if(WhichDAC == PHONELINE)
    {
        ChangeCodecState(CODEC_SET_POTS_DAC, GainValue);
    }
    else
    {
        ChangeCodecState(CODEC_SET_USER_DAC, GainValue);
    }
}

/*******************************************************************************************************************************
 * Function
 *
 * Description
 *
 * Parameters:
 *       -None
 *
 * Returns: None
 *
 * Notes:
 *
 ******************************************************************************************************************************/
void SetADCGain(U8 WhichADC, int GainValue)
{
    if(WhichADC == PHONELINE)
    {
        ChangeCodecState(CODEC_SET_POTS_ADC, GainValue);
    }
    else
    {
        ChangeCodecState(CODEC_SET_USER_ADC, GainValue);
    }
}

