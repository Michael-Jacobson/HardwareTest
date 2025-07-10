/*
 * RTC_test.c
 *
 *  Created on: Jun 21, 2018
 *      Author: cpe
 */
#include <stdio.h>
#include <linux/rtc.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <stdint.h>

//extern char g_outString[100];

struct rtc_time CurrentDateTime;

int rtc_fd = 0;
char RTC_KeepGoing = 1;
pthread_t RTCThread = 0;
struct rtc_time rtc_tm;

void *RTC_Seconds_Thread(void *arg);

/*
 * This expects the new RTC class driver framework, working with
 * clocks that will often not be clones of what the PC-AT had.
 * Use the command line to specify another RTC if you need one.
 */
//static const char *default_rtc = "/dev/rtc0";

/*************************************************************************
 *
 */
void InitRTC(void)
{
    RTC_KeepGoing = 1;

	pthread_create( &RTCThread, NULL, RTC_Seconds_Thread, NULL );
}

/*************************************************************************
 *
 */
void CloseRTC(void)
{
    if(RTCThread != 0)
    {
        RTC_KeepGoing = 0;
        pthread_join(RTCThread, NULL);
        close(rtc_fd);
    }
}

/*************************************************************************
 *
 */
int SetRealTime(void)
{
	int retVal = 0;

    //shit shit

    return retVal;
}



/*************************************************************************
 *
 */
void *RTC_Seconds_Thread(void *arg)
{
    char *DaysOfWeek[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", 0};
    char *Months[] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December", 0};
    char *ampm[] = {"am", "pm", 0};
    int timeout = 0;

    while(RTC_KeepGoing)
    {
    	time_t result = time(NULL);
    	struct tm *p_timeResult = localtime(&result);

    	if(result != (time_t)(-1))
    	{
    		int hour = 0;
    		int ampmselect = 0;

    		if(p_timeResult->tm_hour >= 12)
    		{
    			ampmselect = 1;

    			if(p_timeResult->tm_hour > 12)
    			{
    				hour = p_timeResult->tm_hour - 12;
    			}
    		}
    		else if(p_timeResult->tm_hour == 0)
    		{
    			hour = 12;
    		}
    		else
    		{
    			hour = p_timeResult->tm_hour;
    		}

    	    printf("The current time is %s\n",
    	               asctime(gmtime(&result)));
    		printf("Extracted time: %d:%d%s %s %s %d %d\n",
    				hour,
					p_timeResult->tm_min,
					ampm[ampmselect],
					DaysOfWeek[p_timeResult->tm_wday],
					Months[p_timeResult->tm_mon],
					p_timeResult->tm_mday,
					(p_timeResult->tm_year+1900));



    	}

    	timeout = 0;

    	while((timeout < 60) && (RTC_KeepGoing))
    	{
    		sleep(1);
    		timeout++;
    	}
    }

    return 0;
}
