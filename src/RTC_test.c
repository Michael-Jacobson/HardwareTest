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
    //rtc_fd = open(default_rtc, O_RDWR);

    //if(rtc_fd)
    {
    	pthread_create( &RTCThread, NULL, RTC_Seconds_Thread, NULL );
    }
}

/*************************************************************************
 *
 */
void CloseRTC(void)
{
    if(RTCThread != 0)
    {
        /*int retval = *///ioctl(rtc_fd, RTC_UIE_OFF, 0);
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

#if 0
    int val = 0;
    struct rtc_time rtc_temp;

    memset(&rtc_temp, 0, sizeof(rtc_temp));

    rtc_temp.tm_hour = 12;
    rtc_temp.tm_min = 31;
    rtc_temp.tm_sec = 1;
    rtc_temp.tm_mon = 6;
    rtc_temp.tm_mday = 22;
    rtc_temp.tm_year = 2024-1900;
    //rtc_temp.tm_wday =
    //rtc_temp.

    val = ioctl(rtc_fd, RTC_SET_TIME, &rtc_temp);
    if (val == -1)
    {
       perror("RTC_RD_TIME ioctl");
    }
#else
    //shit shit
#endif

    return retVal;
}



/*************************************************************************
 *
 */
void *RTC_Seconds_Thread(void *arg)
{
#if 0
    fd_set read_fds, write_fds, except_fds;
    struct timeval rtc_timeout;
    int retVal = 0;

    if(rtc_fd > 0)
    {
        /* Turn on update interrupts (one per second) */
        retVal = ioctl(rtc_fd, RTC_UIE_ON, 0);
        if (retVal == -1)
        {
            if (errno == ENOTTY) {
                    fprintf(stderr,
                            "\n...Update IRQs not supported.\n");
            }
            perror("RTC_UIE_ON ioctl");
            RTC_KeepGoing = 0;
        }

        while(RTC_KeepGoing)
        {
        	int fdsetret = 0;

            //set timeout to read function
            FD_ZERO(&read_fds);
            FD_ZERO(&write_fds);
            FD_ZERO(&except_fds);
            FD_SET(rtc_fd, &read_fds);

            //set timeout 1.5s
            rtc_timeout.tv_sec = 1;
            rtc_timeout.tv_usec = 500*1000;

            // Wait for input to become ready or until the time out; the first parameter is
            // 1 more than the largest file descriptor in any of the sets
            fdsetret = select((rtc_fd+1), &read_fds, NULL, NULL, &rtc_timeout);

            if(fdsetret < 0)
            {
                perror("select");
            }
            else if (fdsetret == 1)
            {
                /* how many bytes were read */
                ssize_t rb;
                unsigned long data = 0;

                rb=read(rtc_fd,&data,sizeof(unsigned long));

                if(rb < 0)
                {
                    perror("rtc: error read");
                }
                else if (rb < (int) sizeof(unsigned long))
                {
                    perror("rtc: short read");
                    //exit (1);
                }
                else if (rb >= (int) sizeof(unsigned long))
                {
                    int val = 0;
                    static int minute = 0;

                    //printf("Value: %d\n", data);

                    /* Read the RTC time/date */
                    val = ioctl(rtc_fd, RTC_RD_TIME, &rtc_tm);
                    if (val == -1)
                    {
                        perror("RTC_RD_TIME ioctl");
                    }
                    else
                    {
                        if(minute != rtc_tm.tm_min)
                        {
                            printf("Current Time: %d:%02d %d/%d/%d\n",
                                                               rtc_tm.tm_hour, rtc_tm.tm_min, rtc_tm.tm_mday, rtc_tm.tm_mon, (rtc_tm.tm_year+1900));

                            minute = rtc_tm.tm_min;
                        }
                    }
                }
            }
            else
            {

            }
        }
    }
#else
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

#endif
    return 0;
}
