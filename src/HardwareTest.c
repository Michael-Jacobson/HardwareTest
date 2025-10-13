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
#include "i2c_test.h"
#include "CodecAIC3106.h"

#define CLOCKID CLOCK_REALTIME
#define SIG SIGRTMIN
#define MAX_PRINT_QUEUE_SIZE 50

#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE); \
                       } while (0)

char g_RunProcess = 1;

char RxString[1024];
char g_outString[100];

char collectString = 1;

extern int POTS_Sample_Rx_Index;
extern int HS_Sample_Rx_Index;
extern int POTS_Sample_Tx_Index;
extern int HS_Sample_Tx_Index;

extern int RxGoodCount;
extern int RxBadCount;
extern int TxGoodCount;
extern int TxBadCount;
extern int AveragesTX[2];
extern int AveragesRX[2];


//void initKeyboard(void);
void CloseKeyboard(void);
void InitCodec(void);
void CloseCodec(void);
void SelectHandset(void);
void SelectSpeakerphone(void);
char InitBacklight(void);
void InitRTC(void);
void CloseRTC(void);
void InitLEDs(void);
void CloseLEDs(void);
void TurnOnAllLEDs(void);
void TurnOffAllLEDs(void);
void TestFlasher(void);
void InitGPIO(void);
void CloseGPIO(void);
void GoOffhook(void);
void GoOnhook(void);
void SetHookswitch(unsigned char hook_sw_status);
char SetBacklightValue(int percent);
int SetRealTime(void);
void initTouch(void);
void CloseTouch(void);
char InitWiFi(void);
void DoWiFiScan(void);
void CloseWiFi(void);
void ConnectToWiFiNetwork(int index);
void SetPasswordForWiFiNetwork(int index, char *p_password);
void GetWiFiStatus(void);
void DisconnectFromWiFi(void);

int main(void)
{
	printf("\n\n\n!!!!!Hardware Test!!!!!\n\n");

	init_i2c(ALL_I2C_BUSES);

	printf("Init LEDs\n");
	InitLEDs();

	printf("Init GPIO\n");
	InitGPIO();

	GoOnhook(); //default to onhook
	//printf("Init Keyboard\n");
	//initKeyboard();
	printf("Init RTC\n");
	InitRTC();

	printf("Init Codec\n");
	InitCodec();
	while(GetCodecControlState() != CODEC_IDLE)
	{
		usleep(2);
	}

	printf("Init Touch\n");
	initTouch();

	printf("Init Backlight\n");
	if(InitBacklight() != 2)
	{
		g_RunProcess = 1;
	}

	ChangeCodecState(CODEC_SET_POTS_ADC, ADC_PLUS_7_5DB);

	Test_DigitPOT();

	printf("Init WiFi\n");
	if(InitWiFi())
	{
		//printf("pass\n");
	}
	else
	{
		printf("WiFi fail\n");
	}

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

					printf("**Setting Handset\n");
					SelectHandset();
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
				else if(strstr(RxString, "flasher") != 0)
				{
					printf("**Testing Flasher for 5s\n");
					TestFlasher();
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
					ChangeCodecState(CODEC_SET_USER_ADC, ADC_PLUS_7_5DB);
					ChangeCodecState(CODEC_SET_USER_DAC, DAC_MINUS_7_5DB);
				}
				else if(strstr(RxString, "mute") != 0)
				{
					printf("**Muting Handset\n");
					ChangeCodecState(CODEC_SET_USER_ADC, ADC_MUTE);
					ChangeCodecState(CODEC_SET_USER_DAC, DAC_MUTE);
				}
				else if(strstr(RxString, "Handset") != 0)
				{
					printf("**Selecting Handset\n");
					SelectHandset();
				}
				else if(strstr(RxString, "Speaker") != 0)
				{
					printf("**Selecting Speakerphone\n");
					SelectSpeakerphone();
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
							AveragesRX[PHONELINE], AveragesRX[HANDSET]);
				}
				else if(strstr(RxString, "Reverse Rx") != 0)
				{
					printf("**Reversing POTS and HS Rx Channels\n");

					printf("**From: POTS: %d\nHandset: %d\n\n", POTS_Sample_Rx_Index, HS_Sample_Rx_Index);
					POTS_Sample_Rx_Index = !POTS_Sample_Rx_Index;
					HS_Sample_Rx_Index = !HS_Sample_Rx_Index;
					printf("**To: POTS: %d\nHandset: %d\n\n", POTS_Sample_Rx_Index, HS_Sample_Rx_Index);
				}
				else if(strstr(RxString, "Reverse Tx") != 0)
				{
					printf("**Reversing POTS and HS Tx Channels\n");

					printf("**From: POTS: %d\nHandset: %d\n\n", POTS_Sample_Tx_Index, HS_Sample_Tx_Index);
					POTS_Sample_Tx_Index = !POTS_Sample_Tx_Index;
					HS_Sample_Tx_Index = !HS_Sample_Tx_Index;
					printf("**To: POTS: %d\nHandset: %d\n\n", POTS_Sample_Tx_Index, HS_Sample_Tx_Index);
				}
				else if(strstr(RxString, "wifi scan") != 0)
				{
					printf("**WiFi Scan Started\n");
					DoWiFiScan();
				}
				else if(strstr(RxString, "wifi pw ") != 0)
				{
					char *p_start = strstr(RxString, "wifi pw ");
					if(p_start != NULL)
					{
						p_start += strlen("wifi pw");
						char *p_index_loc = strchr(p_start, ' ');
						if(p_index_loc != NULL)
						{
							p_index_loc++;

							char *p_pw_loc = strchr(p_index_loc, ' ');

							if(p_pw_loc != NULL)
							{
								*p_pw_loc = 0;
								p_pw_loc++;

								int index = atoi(p_index_loc);

								//printf("**WiFi %d PW: %s\n", index, p_pw_loc);
								SetPasswordForWiFiNetwork(index, p_pw_loc);
							}
						}
					}
				}
				else if(strstr(RxString, "wifi join ") != 0)
				{
					char *p_start = strstr(RxString, "wifi join ");
					if(p_start != NULL)
					{
						p_start += strlen("wifi join");
						char *p_index_loc = strchr(p_start, ' ');
						if(p_index_loc != NULL)
						{
							p_index_loc++;
							int index = atoi(p_index_loc);
							printf("**WiFi joining network: %d\n", index);
							ConnectToWiFiNetwork(index);
						}
					}
				}
				else if(strstr(RxString, "wifi status") != 0)
				{
					printf("**Getting WiFi Status\n");
					GetWiFiStatus();
				}
				else if(strstr(RxString, "wifi leave") != 0)
				{
					printf("**Leaving WiFi Network\n");
					DisconnectFromWiFi();
				}
				else if(strstr(RxString, "quit") != 0)
				{
					collectString = 0;
					g_RunProcess = 0;
					printf("\n**Trying to Quit\n\n");
				}
				else if(strstr(RxString, "help") != 0)
				{
					printf("**Commands:\n  \
offhook\n  \
onhook\n  \
leds on\n  \
leds off\n  \
flasher\n  \
backlight on\n  \
backlight off\n  \
mute\n  \
unmute\n  \
Speaker\n  \
Handset\n  \
set time\n  \
count\n  \
averages\n  \
Reverse Rx\n  \
Reverse Tx\n  \
wifi scan\n  \
wifi pw <index> <password>\n \
wifi join <index>\n	\
wifi status\n \
quit\n");
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
	printf("Close WiFi\n");
	CloseWiFi();

	close_i2c(ALL_I2C_BUSES);

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
