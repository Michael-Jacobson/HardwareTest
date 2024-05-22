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




/* ADC PGA Values.  These values are loaded into registers 5A. */
#define ADC_MUTE				-1
#define ADC_PLUS_54DB			5400
#define ADC_PLUS_48DB			4800
#define ADC_PLUS_42DB			4200
#define ADC_PLUS_40_5DB			4050
#define ADC_PLUS_39DB			3900
#define ADC_PLUS_37_5DB			3750
#define ADC_PLUS_36DB			3600
#define ADC_PLUS_34_5DB			3450
#define ADC_PLUS_33DB			3300
#define ADC_PLUS_31_5DB			3150
#define ADC_PLUS_30DB			3000
#define ADC_PLUS_28_5DB			2850
#define ADC_PLUS_27DB			2700
#define ADC_PLUS_25_5DB			2550
#define ADC_PLUS_24DB			2400
#define ADC_PLUS_22_5DB			2250
#define ADC_PLUS_21DB			2100
#define ADC_PLUS_19_5DB			1950
#define ADC_PLUS_18DB			1800
#define ADC_PLUS_16_5DB			1650
#define ADC_PLUS_15DB			1500
#define ADC_PLUS_13_5DB			1350
#define ADC_PLUS_12DB			1200
#define ADC_PLUS_10_5DB			1050
#define ADC_PLUS_9DB			900
#define ADC_PLUS_7_5DB			750
#define ADC_PLUS_6DB		    600
#define ADC_PLUS_4_5DB			450
#define ADC_PLUS_3DB			300
#define ADC_PLUS_1_5DB			150
#define ADC_PLUS_0DB			0

/* DAC PGA Values.  These values are loaded into registers 5B. */
#define DAC_MUTE					-1
#define DAC_MINUS_54DB			-5400
#define DAC_MINUS_48DB			-4800
#define DAC_MINUS_42DB			-4200
#define DAC_MINUS_40_5DB		-4050
#define DAC_MINUS_39DB			-3900
#define DAC_MINUS_37_5DB		-3750
#define DAC_MINUS_36DB			-3600
#define DAC_MINUS_34_5DB		-3450
#define DAC_MINUS_33DB			-3300
#define DAC_MINUS_31_5DB		-3150
#define DAC_MINUS_30DB			-3000
#define DAC_MINUS_28_5DB		-2850
#define DAC_MINUS_27DB			-2700
#define DAC_MINUS_25_5DB		-2550
#define DAC_MINUS_24DB			-2400
#define DAC_MINUS_22_5DB		-2250
#define DAC_MINUS_21DB			-2100
#define DAC_MINUS_19_5DB		-1950
#define DAC_MINUS_18DB			-1800
#define DAC_MINUS_16_5DB		-1650
#define DAC_MINUS_15DB			-1500
#define DAC_MINUS_13_5DB		-1350
#define DAC_MINUS_12DB			-1200
#define DAC_MINUS_10_5DB		-1050
#define DAC_MINUS_9DB			-900
#define DAC_MINUS_7_5DB			-750
#define DAC_MINUS_6DB			-600
#define DAC_MINUS_4_5DB			-450
#define DAC_MINUS_3DB			-300
#define DAC_MINUS_1_5DB			-150
#define DAC_MINUS_0DB			-0

#define MIN_LO_FREQ				4637		/* 566 Hz */
#define MIN_HI_FREQ				5759		/* 703 Hz */
#define MED_LO_FREQ				9429		/* 1151 Hz */
#define MED_HI_FREQ				11690		/* 1427 Hz */
#define MAX_LO_FREQ				13713		/* 1674 Hz */
#define MAX_HI_FREQ				17006		/* 2076 Hz */

#define RING_OFF		0
#define RING_LVL1		3276		/* -dB */
#define RING_LVL2		6552		/* -dB */
#define RING_LVL3		9828		/* -dB */
#define RING_LVL4		13104		/* -dB */
#define RING_LVL5		16380		/* -dB */
#define RING_LVL6		19656		/* -dB */
#define RING_LVL7		22932		/* -dB */
#define RING_LVL8		26208		/* -dB */
#define RING_LVL9		29484		/* -dB */
#define RING_LVL10	    32767		/* 0dB */

/****************/
/* File Enums   */
/****************/
typedef enum
{
    CODEC_INVALID = 0,
    CODEC_INIT,
    CODEC_IDLE,
    CODEC_SWITCH_EXT_SPKR,
    CODEC_SWITCH_EXT_MIC,
    CODEC_SWITCH_HANDSET_SPKR,
    CODEC_SWITCH_HANDSET_MIC,
    CODEC_SET_USER_ADC,
    CODEC_SET_USER_DAC,
    CODEC_SET_POTS_ADC,
    CODEC_SET_POTS_DAC,
    CODEC_ENABLE_SIDETONE,
    CODEC_DISABLE_SIDETONE,
    CODEC_SHUT_DOWN
} CodecControlStates_t;

/****************/
/* File structs */
/****************/

/****************/
/* File Externs */
/****************/
extern volatile S16 BlueTooth_dac_sample;
extern volatile S16 BlueTooth_adc_sample;


/*******************************
 * Public function prototypes
 *******************************/
void *CodecControl(void *arg);

void SetDefaultAudioinputOutput(void);

void SelectExternalSpeaker(BOOL AMUse);
void SelectExternalMic(void);
void DetectHeadsetPlug(void);
void SelectSpeakerOut(void);
void SetDACGain(U8 WhichDAC, int GainValue);
void SetADCGain(U8 WhichADC, int GainValue);
#endif

