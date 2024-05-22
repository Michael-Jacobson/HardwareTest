/*
 * Keyboard_Test.c
 *
 *  Created on: May 31, 2018
 *      Author: cpe
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/input.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/select.h>
#include "Ultratec_Enums.h"

pthread_t KeyboardThread = 0;
char Keyboard_KeepGoing = 1;
//char g_holding_key = 0;


void *MonitorKeyboardThread(void* arg);


/******************************************************************************
 *
 ******************************************************************************/
void initKeyboard(void)
{
    Keyboard_KeepGoing = 1;
    pthread_create( &KeyboardThread, NULL, MonitorKeyboardThread, NULL );
}

/******************************************************************************
 *
 ******************************************************************************/
void CloseKeyboard(void)
{
    if(KeyboardThread != 0)
    {
        Keyboard_KeepGoing = 0;
        pthread_join(KeyboardThread, NULL);
    }
}

/******************************************************************************
 *
 ******************************************************************************/
void *MonitorKeyboardThread(void* arg)
{
    /* how many bytes were read */
    ssize_t rb;
    /* the events (up to 64 at once) */
    struct input_event ev[64];
    int yalv;
    int rep[2];
    int fd = 0;
    fd_set read_fds, write_fds, except_fds;
    struct timeval keyboard_timeout;
    //static char PingPong = 1;

    fd = open("/dev/input/event1", O_RDONLY);

    if(fd <= 0)
    {
    	printf(" Failed to open keyboard\n");
    }
    else
    {
    	printf(" Keyboard Open %d\n", fd);
    }

    //change repeat rate
    rep[0] = 2500;
    rep[1] = 1000;

    if(ioctl(fd, EVIOCSREP, rep))
    {
        perror(" keyboard evdev ioctl");
    }

    while(Keyboard_KeepGoing)
    {
        //set timeout to read function
        FD_ZERO(&read_fds);
        FD_ZERO(&write_fds);
        FD_ZERO(&except_fds);
        FD_SET(fd, &read_fds);

        //set timeout
        keyboard_timeout.tv_sec = 1;
        keyboard_timeout.tv_usec = 0;

        // Wait for input to become ready or until the time out; the first parameter is
        // 1 more than the largest file descriptor in any of the sets
        int fdsetret = select((fd+1), &read_fds, NULL, NULL, &keyboard_timeout);

        if(fdsetret < 0)
        {
            perror("Select Error");
        }
        else if (fdsetret == 1)
        {
            rb=read(fd,ev,sizeof(struct input_event)*64);

            if(rb < 0)
            {
                perror("evtest: error read");
            }
            else if (rb < (int) sizeof(struct input_event))
            {
                perror("evtest: short read");
                //exit (1);
            }
            else if (rb >= (int) sizeof(struct input_event))
            {
                for (yalv = 0;
                     yalv < (int) (rb / sizeof(struct input_event));
                     yalv++)
                {
                    if (EV_KEY == ev[yalv].type)
                    {
                        switch(ev[yalv].code)
                        {
                            case BUT_FLASH:
                                //
                                if(ev[yalv].value == 1)
                                {
                                	printf("Flash Button\n");
                                }
                            break;

                            case BUT_REDIAL:
                                //
                                if(ev[yalv].value == 1)
                                {
                                	printf("Redial Button\n");
                                }
                            break;

                            case BUT_SPKRPHN:
                                //
                                if(ev[yalv].value == 1)
                                {
                                    printf("Speakerphone Button\n");
                                }
                            break;

                            case BUT_MUTE:
                                //
                                if(ev[yalv].value == 1)
                                {
                                	printf("Mute Button\n");
                                }
                            break;

                            case BUT_CAPTION:
                                //
                                if(ev[yalv].value == 1)
                                {
                                    printf("Captions Button\n");
                                }
                            break;

                            case BUT_VOLDN:
                                //
                                if(ev[yalv].value == 1)
                                {
                                    printf("Volume Down Button\n");
                                }
                            break;

                            case BUT_VOLUP:
                                //
                                if(ev[yalv].value == 1)
                                {
                                    printf("Volume Up Button\n");
                                }
                            break;

                            case BUT_CSTMR_SRVC:
                                //
                                if(ev[yalv].value == 1)
                                {
                                    printf("Customer Service Button\n");
                                }
                            break;

                            case BUT_HOME:
                                //
                                if(ev[yalv].value == 1)
                                {
                                    printf("Home Button\n");
                                }
                            break;

                                      /* Special "DTMF" keys */
                            case BUT_DTMF0:
                            case BUT_DTMF1:
                            case BUT_DTMF2:
                            case BUT_DTMF3:
                            case BUT_DTMF4:
                            case BUT_DTMF5:
                            case BUT_DTMF6:
                            case BUT_DTMF7:
                            case BUT_DTMF8:
                            case BUT_DTMF9:
                            	//
								if(ev[yalv].value == 1)
								{
									printf("DTMF Button %d\n", (ev[yalv].code & 0x3F));
									//g_holding_key = 1;
								}
								/*else if(ev[yalv].value == 2)
								{
									g_holding_key = 1;
								}
								else
								{
									//SetGPIOValue(0);
									g_holding_key = 0;
								}*/
                            break;

                            case BUT_DTMFST:
                                //
                                if(ev[yalv].value == 1)
                                {
                                    printf("DTMF Button *\n");
                                }
                            break;

                            case BUT_DTMFLB:
                                //
                                if(ev[yalv].value == 1)
                                {
                                    printf("DTMF Button #\n");
                                }
                            break;

                            default:
                                //
                            break;

                        }
#if 1
                        if(ev[yalv].value == 1)
                        {
                            printf("%ld.%06ld ",                   ev[yalv].time.tv_sec,                   ev[yalv].time.tv_usec);
                            printf("type %d code %x value %d\n",                   ev[yalv].type,                   ev[yalv].code, ev[yalv].value);
                        }
#endif
                    }
                }
            }
        }
#if 0
        else if(fdsetret > 0)
        {
            printf("fd selected %d\n", fdsetret);
        }
#endif
    }

    close(fd);

    return 0;
}
