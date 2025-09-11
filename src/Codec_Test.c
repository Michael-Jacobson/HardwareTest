/*
 * Codec_Test.c
 *
 *  Created on: Feb 15, 2024
 *      Author: michaeljacobson
 */

/********************************************************************************************************************************
 ********************************************************************************************************************************
 *                                                             UTI_Codec.c
 *                                COPYRIGHT NOTICE: (c) Ultratec, Inc.  2020  ALL RIGHTS RESERVED
 *															Author: Michael_J
 ********************************************************************************************************************************
 ********************************************************************************************************************************
 *
 * This file contains
 *
 ********************************************************************************************************************************/

/*******************/
/* File #include's */
/*******************/
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
#include "CodecAIC3106.h"

/**************
 * Defines
 **************/

/********************/
/* Global Variables */
/********************/

/*******************/
/* Local Variables */
/*******************/
pthread_t CodecControlThread = 0;
sem_t CodecControlSem;

BOOL CodecControlThread_KeepGoing = TRUE;

int CodecPendingGain = 0;
static CodecHardwareFilterChoices_t CodecPendingFilter;
BOOL CodecInProcess = TRUE;

CodecControlStates_t CodecControlState = CODEC_INIT;

extern BOOL RxRunning;

void InitSoundDrivers(void);
void CloseSoundDrivers(void);

void InitCodec(void);
void CloseCodec(void);

/*****************************/
/* Local Function Prototypes */
/*****************************/
static BOOL Codec_Init(void);
static BOOL Codec_Set_OutputFilter(void);
static BOOL Codec_Enable_Ext_Spkr(void);
static BOOL Codec_Enable_Ext_Mic(void);
static BOOL Codec_Enable_Handset_Spkr(void);
static BOOL Codec_Enable_Handset_Mic(void);

static BOOL Codec_Set_User_ADC(U32 Gain);
static BOOL Codec_Set_User_DAC(U32 Gain);
static BOOL Codec_Set_POTS_ADC(U32 Gain);
static BOOL Codec_Set_POTS_DAC(U32 Gain);

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
	//audio - launches threads
	printf("Init Sound\n");
	InitSoundDrivers(); //sound has to be started first. it sets codec values that we have to change after it starts

	while(!RxRunning)
	{
		usleep(10);
	}

	usleep(100);

	while(CodecControlThread_KeepGoing)
    {
        switch (CodecControlState)
        {
/********************/
            case CODEC_INIT:
            	//printf("Init Codec\n");
                if (Codec_Init() == FALSE)
                {
                    CodecControlState = CODEC_SHUT_DOWN; //if we fail do a reset
                }
                else
                {
                    CodecControlState = CODEC_IDLE;
                }
            break;
/********************/
            case CODEC_IDLE:
                //
                CodecInProcess = FALSE;
                sem_wait(&CodecControlSem);

				//printf("Leaving Idle for: %d and gain %d\n", CodecControlState, CodecPendingGain);

				if(CodecControlThread_KeepGoing == FALSE)
				{
					CodecControlState = CODEC_SHUT_DOWN;
				}
            break;
/********************/
            case CODEC_SET_OUTPUT_FILTER:
                //
                Codec_Set_OutputFilter();

                CodecControlState = CODEC_IDLE;
            break;
/********************/
            case CODEC_SWITCH_EXT_SPKR:
                //
                Codec_Enable_Ext_Spkr();

                CodecControlState = CODEC_IDLE;
            break;
/********************/
            case CODEC_SWITCH_EXT_MIC:
                //
                Codec_Enable_Ext_Mic();

                CodecControlState = CODEC_IDLE;
            break;
/********************/
            case CODEC_SWITCH_HANDSET_SPKR:
                //
                Codec_Enable_Handset_Spkr();

                CodecControlState = CODEC_IDLE;
            break;
/********************/
            case CODEC_SWITCH_HANDSET_MIC:
                //
                Codec_Enable_Handset_Mic();

                CodecControlState = CODEC_IDLE;
            break;

/********************/
            case CODEC_SET_USER_ADC:
                //
                Codec_Set_User_ADC(CodecPendingGain);

                CodecControlState = CODEC_IDLE;
            break;
/********************/
            case CODEC_SET_USER_DAC:
                //
                Codec_Set_User_DAC(CodecPendingGain);

                CodecControlState = CODEC_IDLE;
            break;
/********************/
            case CODEC_SET_POTS_ADC:
                //
                Codec_Set_POTS_ADC(CodecPendingGain);

                CodecControlState = CODEC_IDLE;
            break;
/********************/
            case CODEC_SET_POTS_DAC:
                //
                Codec_Set_POTS_DAC(CodecPendingGain);

                CodecControlState = CODEC_IDLE;
            break;
/********************/
            case CODEC_SHUT_DOWN:
                //
            	//printf("Doing Codec Shut Down\n");
				CodecControlThread_KeepGoing = FALSE;
            break;
/********************/
            default:
                //
                CodecControlState = CODEC_SHUT_DOWN;
            break;
        }
    }

	printf("Close Sound\n");
	CloseSoundDrivers();

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
CodecControlStates_t GetCodecControlState(void)
{
    return CodecControlState;
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
void SelectSpeakerphone(void)
{
    ChangeCodecState(CODEC_SWITCH_EXT_MIC, 0);
    ChangeCodecState(CODEC_SWITCH_EXT_SPKR, 0);
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
static BOOL Codec_Init(void)
{
    BOOL pass = FALSE;

    pass = InitCodecAIC3106();

    return pass;
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
static BOOL Codec_Set_OutputFilter(void)
{
    BOOL pass = FALSE;

    if (CodecPendingFilter != CODEC_FILTER_INVALID)
    {
        pass = AIC3106_Codec_Set_Output_Filter(CodecPendingFilter);
        CodecPendingFilter = CODEC_FILTER_INVALID;
    }

    return pass;
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
static BOOL Codec_Enable_Ext_Spkr(void)
{
    BOOL pass = FALSE;

    pass = AIC3106_Codec_Enable_Ext_Spkr();

    return pass;
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
static BOOL Codec_Enable_Ext_Mic(void)
{
    BOOL pass = FALSE;

    pass = AIC3106_Codec_Enable_Ext_Mic();

    return pass;
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
static BOOL Codec_Enable_Handset_Spkr(void)
{
    BOOL pass = FALSE;

    pass = AIC3106_Codec_Enable_Handset_Spkr();

    return pass;
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
static BOOL Codec_Enable_Handset_Mic(void)
{
    BOOL pass = FALSE;

    pass = AIC3106_Codec_Enable_Handset_Mic();

    return pass;
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
static BOOL Codec_Set_User_ADC(U32 Gain)
{
    BOOL pass = FALSE;

    pass = AIC3106_Codec_Set_User_ADC(Gain);

    return pass;
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
static BOOL Codec_Set_User_DAC(U32 Gain)
{
    BOOL pass = FALSE;

    pass = AIC3106_Codec_Set_User_DAC(Gain);

    return pass;
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
static BOOL Codec_Set_POTS_ADC(U32 Gain)
{
    BOOL pass = FALSE;

    pass = AIC3106_Codec_Set_POTS_ADC(Gain);

    return pass;
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
static BOOL Codec_Set_POTS_DAC(U32 Gain)
{
    BOOL pass = FALSE;

    pass = AIC3106_Codec_Set_POTS_DAC(Gain);

    return pass;
}

