/********************************************************************************************************************************/
/********************************************************************************************************************************/
/*                                                       Touchscreen.c                                                    */
/*                                COPYRIGHT NOTICE: (c) Ultratec, Inc.  2019  ALL RIGHTS RESERVED                               */
/********************************************************************************************************************************/
/*                                                                                                                              */
/* This file contains the means to detect a touch event. the only reason this file/section exists right now is to determine if
 * the user touched the screen and if we need to delay or come out of backlight night mode/sleep                                */
/*                                                                                                                              */
/* Product:                                                                                                         */
/*                                                                                                                              */
/********************************************************************************************************************************/
/*******************/
/* File #include's */
/*******************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/input.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/select.h>
#include <pthread.h>
#include "C674xTypes.h"
#include "Ultratec_Enums.h"

/**************
 * Defines
 **************/

/**************/
/*  Globals   */
/**************/
pthread_t TouchThread = 0;

/*******************/
/* Local Variables */
/*******************/
int touch_fd = 0;
BOOL TouchThread_KeepGoing = TRUE;

/*****************************/
/* Local Function Prototypes */
/*****************************/
void *TouchDetect(void *arg);

/******************************************************************************
 *
 ******************************************************************************/
void initTouch(void)
{
    touch_fd = open("/dev/input/event1", O_RDONLY);

    if(touch_fd > 0)
    {
    	pthread_create( &TouchThread, NULL, TouchDetect, NULL );
    }
    else
    {
    	printf(" Failed to open Touchscreen\n");
    }
}

/******************************************************************************
 *
 ******************************************************************************/
void CloseTouch(void)
{
	TouchThread_KeepGoing = FALSE;
    pthread_join( TouchThread, NULL);
    close(touch_fd);
}

/**********************************************************************
 *
 */
void *TouchDetect(void *arg)
{
    /* how many bytes were read */
    ssize_t rb;
    /* the events (up to 64 at once) */
    struct input_event ev[64];

    fd_set read_fds, write_fds, except_fds;
    struct timeval touch_timeout;

    while(TouchThread_KeepGoing)
    {
        //set timeout to read function
        FD_ZERO(&read_fds);
        FD_ZERO(&write_fds);
        FD_ZERO(&except_fds);
        FD_SET(touch_fd, &read_fds);

        //set timeout
        touch_timeout.tv_sec = 0;
        touch_timeout.tv_usec = 100*1000;

        // Wait for input to become ready or until the time out; the first parameter is
        // 1 more than the largest file descriptor in any of the sets
        int fdsetret = select((touch_fd+1), &read_fds, NULL, NULL, &touch_timeout);

        if(fdsetret < 0)
        {
            perror("touch Select Error\n");
        }
        else if (fdsetret == 1)
        {
            rb=read(touch_fd,ev,sizeof(struct input_event)*64);

            if(rb < 0)
            {
                perror("touch evtest: error read\n");
            }
            else if (rb < (int) sizeof(struct input_event))
            {
                perror("touch evtest: short read\n");
                //exit (1);
            }
            else if (rb >= (int) sizeof(struct input_event))
            {
                //SetBackLightStatus(BACKLIGHT_ON);
                printf("Touch Detected!\n");
            }
        }
        else if(fdsetret > 0)
        {
            printf("touch fd selected %d\n", fdsetret);
        }
    }



    return 0;
}
