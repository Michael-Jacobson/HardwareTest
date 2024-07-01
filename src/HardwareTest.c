/*
 ============================================================================
 Name        : HardwareTest.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <sys/signal.h>
#include <sys/errno.h>
#include <semaphore.h>

#include "C674xTypes.h"
#include "Ultratec_Enums.h"
#include "Codec.h"

#define CLOCKID CLOCK_REALTIME
#define SIG SIGRTMIN
#define MAX_PRINT_QUEUE_SIZE 50

#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE); \
                       } while (0)

char g_RunProcess = 1;

char RxString[1024];
char g_outString[100];

char collectString = 1;

extern int POTS_Sample_Index;
extern int HS_Sample_Index;

extern int RxGoodCount;
extern int RxBadCount;
extern int TxGoodCount;
extern int TxBadCount;
extern int AveragesTX[2];
extern int AveragesRX[2];

void initKeyboard(void);
void CloseKeyboard(void);
void InitCodec(void);
void CloseCodec(void);
void SelectHandset(void);
void SetDACGain(U8 WhichDAC, int GainValue);
void SetADCGain(U8 WhichADC, int GainValue);
char InitBacklight(void);
void InitRTC(void);
void CloseRTC(void);
void InitLEDs(void);
void CloseLEDs(void);
void TurnOnAllLEDs(void);
void TurnOffAllLEDs(void);
void InitGPIO(void);
void CloseGPIO(void);
void GoOffhook(void);
void GoOnhook(void);
void SetHookswitch(unsigned char hook_sw_status);
char SetBacklightValue(int percent);
int SetRealTime(void);
void initTouch(void);
void CloseTouch(void);

int main(void)
{
	printf("\n\n\n!!!!!Hardware Test!!!!!\n\n"); /* prints !!!Hello World!!! */

	printf("Init LEDs\n");
	InitLEDs();
	printf("Init GPIO\n");
	InitGPIO();
	GoOnhook(); //default to onhook
	//printf("Init Keyboard\n");
	//initKeyboard();
	printf("Init RTC\n");
	InitRTC();
	//sleep(1);
	printf("Init Codec\n");
	InitCodec();
	sleep(1);
	printf("Init Touch\n");
	initTouch();

	/*printf("Init Backlight\n");
	if(InitBacklight() != 2)
	{
		g_RunProcess = 1;
	}*/

	SelectHandset();
	SetADCGain(PHONELINE, ADC_PLUS_7_5DB);


	while(g_RunProcess)
	{
		collectString = 1;

		printf("\nReady\nType \"help\" for commands\n");

		memset(RxString, 0, sizeof(RxString));

		while(collectString)
		{
			if(fgets(RxString,sizeof(RxString), stdin) != 0)
			{
				if(strstr(RxString, "offhook") != 0)
				{
					printf("**Going Off hook\n");

					GoOffhook();
				}
				else if(strstr(RxString, "onhook") != 0)
				{
					printf("**Going On hook\n");
					GoOnhook();
				}
				else if(strstr(RxString, "leds on") != 0)
				{
					printf("**Turning on all LEDs\n");
					TurnOnAllLEDs();
				}
				else if(strstr(RxString, "leds off") != 0)
				{
					printf("**Turning off all LEDs\n");
					TurnOffAllLEDs();
				}
				else if(strstr(RxString, "backlight on") != 0)
				{
					printf("**Setting Backlight to 100 Percent\n");
					SetBacklightValue(100);
				}
				else if(strstr(RxString, "backlight off") != 0)
				{
					printf("**Setting Backlight to 0 Percent\n");
					SetBacklightValue(0);
				}
				else if(strstr(RxString, "unmute") != 0)
				{
					printf("**Unmuting Handset\n");
					SetADCGain(HANDSET, ADC_PLUS_7_5DB);
					SetDACGain(HANDSET, DAC_MINUS_7_5DB);
				}
				else if(strstr(RxString, "mute") != 0)
				{
					printf("**Muting Handset\n");
					SetADCGain(HANDSET, ADC_MUTE);
					SetDACGain(HANDSET, DAC_MUTE);
				}
				else if(strstr(RxString, "set time") != 0)
				{
					printf("**Setting Time\n");
					SetRealTime();
				}
				else if(strstr(RxString, "count") != 0)
				{
					printf("**Rx Good Count=%d\n**Rx Bad Count=%d\n**Tx Good Count=%d\n**Tx Bad Count=%d\n",
							RxGoodCount, RxBadCount, TxGoodCount, TxBadCount);
				}
				else if(strstr(RxString, "averages") != 0)
				{
					printf("**Average POTS Tx:%d\n**Average Handset Tx:%d\n\n**Average POTS Rx:%d\n**Average Handset Rx:%d\n",
							AveragesTX[PHONELINE], AveragesTX[HANDSET],
							AveragesRX[POTS_Sample_Index], AveragesRX[HS_Sample_Index]);
				}
				else if(strstr(RxString, "quit") != 0)
				{
					collectString = 0;
					g_RunProcess = 0;
					printf("\n**Trying to Quit\n\n");
				}
				else if(strstr(RxString, "help") != 0)
				{
					printf("**Commands:\n  offhook\n  onhook\n  leds on\n  leds off\n  backlight on\n  backlight off\n  mute\n  unmute\n  set time\n  quit\n");
				}

				memset(RxString, 0, sizeof(RxString));
			}

			sleep(1);
		}

		printf("Going On Hook for quitting\n");
		GoOnhook();
	}

	printf("Close GPIO\n");
	CloseGPIO();
	printf("Close Keyboard\n");
	CloseKeyboard();
	printf("Close Codec\n");
	//there is no close backlight because it isn't a thread
	CloseCodec();
	printf("Close RTC\n");
	CloseRTC();
	printf("Close LEDs\n");
	CloseLEDs();
	printf("Close Touch\n");
	CloseTouch();

	printf("\n!!!!!Done Hardware Test!!!!!\n\n\n");

	return EXIT_SUCCESS;
}


/*****************************************************************************************
 *
 */
void GoOnhook(void)
{
    SetHookswitch(1);
    //SetHandsetSpkr(1);
}


/*****************************************************************************************
 *
 */
void GoOffhook(void)
{
    SetHookswitch(0);
    //SetHandsetSpkr(1);
}
