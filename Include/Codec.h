/********************************************************************************************************************************/
/********************************************************************************************************************************/
/*                                                         CODECAIC20K.H                                                        */
/*                                COPYRIGHT NOTICE: (c) Ultratec, Inc.  2016  ALL RIGHTS RESERVED                               */
/********************************************************************************************************************************/
/*                                                                                                                              */
/* This file defines the TLV320AIC20K CODEC for use on the 2400iBT.                                                                */
/*                                                                                                                              */
/*                                                                                                                              */
/********************************************************************************************************************************/
#ifndef CODEC_AIC20K_H
#define CODEC_AIC20K_H

/****************/
/*   Includes   */
/****************/

/****************/
/* File defines */
/****************/

/* Define which CODEC channel gets what function. */
#define EXTSPKR	    0
#define HANDSET	    0
#define PHONELINE	1


/****************/
/* File Enums   */
/****************/
typedef enum
{
	CODEC_INVALID = 0,
	CODEC_INIT,
	CODEC_IDLE,
	CODEC_SET_OUTPUT_FILTER,
	CODEC_SWITCH_EXT_SPKR,
	CODEC_SWITCH_EXT_MIC,
	CODEC_SWITCH_HANDSET_SPKR,
	CODEC_SWITCH_HANDSET_MIC,
	CODEC_SWITCH_3_5MM_SPKR,
	CODEC_SWITCH_3_5MM_MIC,
	CODEC_SWITCH_2_5MM_SPKR,
	CODEC_SWITCH_2_5MM_MIC,
	CODEC_SET_USER_ADC,
	CODEC_SET_USER_DAC,
	CODEC_SET_POTS_ADC,
	CODEC_SET_POTS_DAC,
	CODEC_SHUT_DOWN
} CodecControlStates_t;

/****************/
/* File structs */
/****************/

/****************/
/* File Externs */
/****************/


/*******************************
 * Public function prototypes
 *******************************/
void *CodecControl(void *arg);

void SetDefaultAudioinputOutput(void);

void ChangeCodecState(CodecControlStates_t state, int gain);
CodecControlStates_t GetCodecControlState(void);
#endif

