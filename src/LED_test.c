/*
 * LED_test.c
 *
 *  Created on: Jun 27, 2018
 *      Author: cpe
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

#define RING_FLASHER_LED_PATH   "/sys/class/leds/RingFlasher/brightness"
#define CAPTIONS_LED_PATH       "/sys/class/leds/captions_led/brightness"
#define SPEAKERPHONE_LED_PATH   "/sys/class/leds/spkr_led/brightness"
#define KEYPAD_LED_PATH         "/sys/class/leds/keypad_led/brightness"
#define MUTE_LED_PATH           "/sys/class/leds/mute_led/brightness"
#define VOLUME1_LED_PATH        "/sys/class/leds/vol1_led/brightness"
#define VOLUME2_LED_PATH        "/sys/class/leds/vol2_led/brightness"
#define VOLUME3_LED_PATH        "/sys/class/leds/vol3_led/brightness"
#define VOLUME4_LED_PATH        "/sys/class/leds/vol4_led/brightness"
#define VOLUME5_LED_PATH        "/sys/class/leds/vol5_led/brightness"
#define VOLUME6_LED_PATH        "/sys/class/leds/vol6_led/brightness"

int RingFlasherLED = 0;
int CaptionsLED = 0;
int SpeakerphoneLED = 0;
int KeypadLED = 0;
int MuteLED = 0;
int Volume1LED = 0;
int Volume2LED = 0;
int Volume3LED = 0;
int Volume4LED = 0;
int Volume5LED = 0;
int Volume6LED = 0;

static void SetLED(int WhichLED, char value);
//static void GetLED(int WhichLED, char *value);

void TurnOnAllLEDs(void);
void TurnOffAllLEDs(void);

/*****************************************************************************
 *
 */
void InitLEDs(void)
{
	RingFlasherLED = open(RING_FLASHER_LED_PATH, O_RDWR);
	if(RingFlasherLED <= 0)
	{
		printf(" Failed to Open RingFlasherLED\n");
	}

	CaptionsLED = open(CAPTIONS_LED_PATH, O_RDWR);
	if(CaptionsLED <= 0)
	{
		printf(" Failed to Open CaptionsLED\n");
	}

	SpeakerphoneLED = open(SPEAKERPHONE_LED_PATH, O_RDWR);
	if(SpeakerphoneLED <= 0)
	{
		printf(" Failed to Open SpeakerphoneLED\n");
	}

	KeypadLED = open(KEYPAD_LED_PATH, O_RDWR);
	if(KeypadLED <= 0)
	{
		printf(" Failed to Open KeypadLED\n");
	}

	MuteLED = open(MUTE_LED_PATH, O_RDWR);
	if(MuteLED <= 0)
	{
		printf(" Failed to Open MuteLED\n");
	}

	Volume1LED = open(VOLUME1_LED_PATH, O_RDWR);
	if(Volume1LED <= 0)
	{
		printf(" Failed to Open Volume1LED\n");
	}

	Volume2LED = open(VOLUME2_LED_PATH, O_RDWR);
	if(Volume2LED <= 0)
	{
		printf(" Failed to Open Volume2LED\n");
	}

	Volume3LED = open(VOLUME3_LED_PATH, O_RDWR);
	if(Volume3LED <= 0)
	{
		printf(" Failed to Open Volume3LED\n");
	}

	Volume4LED = open(VOLUME4_LED_PATH, O_RDWR);
	if(Volume4LED <= 0)
	{
		printf(" Failed to Open Volume4LED\n");
	}

	Volume5LED = open(VOLUME5_LED_PATH, O_RDWR);
	if(Volume5LED <= 0)
	{
		printf(" Failed to Open Volume5LED\n");
	}

	Volume6LED = open(VOLUME6_LED_PATH, O_RDWR);
	if(Volume6LED <= 0)
	{
		printf(" Failed to Open Volume6LED\n");
	}

	//turn off all LEDs
	TurnOffAllLEDs();
}

/*****************************************************************************
 *
 */
void TurnOnAllLEDs(void)
{
	SetLED(RingFlasherLED, 1);
	SetLED(CaptionsLED, 1);
	SetLED(SpeakerphoneLED, 1);
	SetLED(KeypadLED, 1);
	SetLED(MuteLED, 1);
	SetLED(Volume1LED, 1);
	SetLED(Volume2LED, 1);
	SetLED(Volume3LED, 1);
	SetLED(Volume4LED, 1);
	SetLED(Volume5LED, 1);
	SetLED(Volume6LED, 1);
}

/*****************************************************************************
 *
 */
void TurnOffAllLEDs(void)
{
	SetLED(RingFlasherLED, 0);
	SetLED(CaptionsLED, 0);
	SetLED(SpeakerphoneLED, 0);
	SetLED(KeypadLED, 0);
	SetLED(MuteLED, 0);
	SetLED(Volume1LED, 0);
	SetLED(Volume2LED, 0);
	SetLED(Volume3LED, 0);
	SetLED(Volume4LED, 0);
	SetLED(Volume5LED, 0);
	SetLED(Volume6LED, 0);
}

/*****************************************************************************
 *
 */
void CloseLEDs(void)
{
	//shut down all the LEDS right away because we might have lost power
	TurnOffAllLEDs();

	if(RingFlasherLED > 0)
	{
		close(RingFlasherLED);
		RingFlasherLED = 0;
	}

	if(CaptionsLED > 0)
	{
		close(CaptionsLED);
		CaptionsLED = 0;
	}

	if(SpeakerphoneLED > 0)
	{
		close(SpeakerphoneLED);
		SpeakerphoneLED = 0;
	}

	if(KeypadLED > 0)
	{
		close(KeypadLED);
		KeypadLED = 0;
	}

	if(MuteLED > 0)
	{
		close(MuteLED);
		MuteLED = 0;
	}

	if(Volume1LED > 0)
	{
		close(Volume1LED);
		Volume1LED = 0;
	}

	if(Volume2LED > 0)
	{
		close(Volume2LED);
		Volume2LED = 0;
	}

	if(Volume3LED > 0)
	{
		close(Volume3LED);
		Volume3LED = 0;
	}

	if(Volume4LED > 0)
	{
		close(Volume4LED);
		Volume4LED = 0;
	}

	if(Volume5LED > 0)
	{
		close(Volume5LED);
		Volume5LED = 0;
	}

	if(Volume6LED > 0)
	{
		close(Volume6LED);
		Volume6LED = 0;
	}
}

/*****************************************************************************
 *
 */
static void SetLED(int WhichLED, char value)
{
    char realValue[2] = {0, 0};

    realValue[0] = value | 0x30;

    if(WhichLED > 0)
    {
        ssize_t returnVal = write(WhichLED, realValue, 1);

        if(returnVal != 1)
        {
            fprintf(stderr, "No LED written today, friend-o: %d %s\n", errno, strerror(errno));
        }
        /*else
        {
            fsync(WhichLED);
        }*/
    }
}

/*****************************************************************************
 *
 */
/*static void GetLED(int WhichLED, char *value)
{
    if(WhichLED > 0)
    {
        off_t offset = lseek(WhichLED, 0, SEEK_SET);

        if(offset == 0)
        {
            ssize_t returnVal = read(WhichLED, value, 1);   //it seems like it returns an ASCII var, not a binary var

            if(returnVal != 1)
            {
                fprintf(stderr, "No LED Read today friend-o: %d %s\n", errno, strerror(errno));
            }
        }
        else
        {
            printf("lseek failed\n");
        }
    }
}*/
