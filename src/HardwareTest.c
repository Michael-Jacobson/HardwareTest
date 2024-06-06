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

extern int RxGoodCount;
extern int RxBadCount;
extern int TxGoodCount;
extern int TxBadCount;
extern int AvgTxOutputHS;
extern int AvgRxInputMic;

void InitCodec(void);
void CloseCodec(void);

int SetRealTime(void);

int main(void)
{
	printf("\n\n\n!!!!!Hardware Test!!!!!\n\n"); /* prints !!!Hello World!!! */

	printf("Init Codec\n");
	InitCodec();

	while(g_RunProcess)
	{
		collectString = 1;

		printf("\nReady\nType \"help\" for commands\n");

		memset(RxString, 0, sizeof(RxString));

		while(collectString)
		{
			if(fgets(RxString,sizeof(RxString), stdin) != 0)
			{
				if(strstr(RxString, "set time") != 0)
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
					printf("**Average Handset Tx:%d\n**Average Handset Rx:%d\n",
							AvgTxOutputHS, AvgRxInputMic);
				}
				else if(strstr(RxString, "quit") != 0)
				{
					collectString = 0;
					g_RunProcess = 0;
					printf("\n**Trying to Quit\n\n");
				}
				else if(strstr(RxString, "help") != 0)
				{
					printf("**Commands:\n  set time\n  quit\n");
				}

				memset(RxString, 0, sizeof(RxString));
			}

			sleep(1);
		}
	}

	printf("Close Codec\n");
	//there is no close backlight because it isn't a thread
	CloseCodec();

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
