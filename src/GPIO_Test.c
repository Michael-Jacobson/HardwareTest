/*int GPIO_Dev_fd
 * GPIO_Test.c
 *
 *  Created on: May 22, 2018
 *      Author: cpe
 *
 *      how GPIO works for renesas rzg2l:
 *      https://renesas.info/wiki/RZ-G/RZG_kernel#GPIO_Pin_Usage
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/gpio.h>
#include <errno.h>
#include <pthread.h>
#include <linux/input.h>
#include <sys/select.h>
//#include <math.h>
#include <sys/time.h>
#include "C674xTypes.h"
#include "Ultratec_Enums.h"

#define GPIO_DEV_PATH "/dev/gpiochip0" //gpio4 = 43:0 = item 43*8 + 0 = 344

//#define GPIO_OFFHOOK	0

//struct gpiohandle_request HookControl;

int GPIOKeys_fd = 0;
int GPIOKeysPolled_fd = 0;
int GPIO_Dev_fd = 0;

char GPIO_keys_keepGoing = 1;
char GPIO_keys_polled_keepGoing = 1;

pthread_t GPIOKeysPolledThread = 0;
pthread_t GPIOKeysThread = 0;

void InitGPIOKeyboard(void);
void CloseGPIOKeyboard(void);

void InitGPIO(void);
void CloseGPIO(void);
void *GPIOKeys(void *arg);
void *GPIOKeysPolled(void *arg);
/*int write_GPIO(int GPIOLine, unsigned char value);
int read_GPIO(int GPIOLine, struct gpiohandle_data *data);
void SetHookswitch(unsigned char hook_sw_status);*/

/**********************************************************************
 *
 */
void InitGPIO(void)
{
	int ret = 0;
	struct gpiochip_info cinfo;

	//set up the GPIO inputs as debounced keys
	GPIOKeysPolled_fd = open("/dev/input/event0", O_RDONLY);

	if(GPIOKeysPolled_fd != 0)
	{
		int rep[2];

		//change repeat rate
		rep[0] = 1000;
		rep[1] = 1000;

		if(ioctl(GPIOKeysPolled_fd, EVIOCSREP, rep) < 0)
		{
			perror(" GPIOKeysPolled_fd evdev ioctl");
		}
		else
		{
			pthread_create( &GPIOKeysPolledThread, NULL, GPIOKeysPolled, NULL );
		}
	}
	else
	{
		printf(" Failed to open GPIO keys Polled\n");
	}
/*
	GPIOKeys_fd = open("/dev/input/event2", O_RDONLY);

	if(GPIOKeys_fd != 0)
	{
		//int rep[2];
		unsigned char key_states[KEY_MAX/8 + 1];

		//change repeat rate
		//rep[0] = 1000;
		//rep[1] = 1000;

		//if(ioctl(GPIOKeys_fd, EVIOCSREP, rep) < 0)
		if(ioctl(GPIOKeys_fd, EVIOCGKEY(sizeof(key_states)), key_states) < 0)
		{
			perror(" GPIOKeys_fd evdev ioctl");
		}
		//else
		{
			pthread_create( &GPIOKeysThread, NULL, GPIOKeys, NULL );
		}
	}
	else
	{
		printf(" Failed to open GPIO keys\n");
	}
*/
	//set up gpio control
	GPIO_Dev_fd = open(GPIO_DEV_PATH, O_RDWR);

	if(GPIO_Dev_fd > 0)
	{
		ret = ioctl(GPIO_Dev_fd, GPIO_GET_CHIPINFO_IOCTL, &cinfo);

		if(ret >= 0)
		{
			InitGPIOKeyboard();
		}
		else
		{
			ret = -errno;
			fprintf(stderr, " Failed to request %s (%d), %s\n",
							"GPIO43_0_OFFHOOK GPIO_GET_CHIPINFO_IOCTL 3", ret, strerror(errno));
		}

	}
	else
	{
		printf(" Failed to open Hookswitch GPIO\n");
	}
}

/**********************************************************************
 *
 */
void CloseGPIO(void)
{
	CloseGPIOKeyboard();

	GPIO_keys_keepGoing = 0;
	GPIO_keys_polled_keepGoing = 0;

    if(GPIO_Dev_fd > 0)
    {
        close(GPIO_Dev_fd);
    }

    if(GPIOKeys_fd > 0)
    {
        close(GPIOKeys_fd);
        //join thread
        if(GPIOKeysThread != 0)
        {
            pthread_join( GPIOKeysThread, NULL);
        }
    }

    if(GPIOKeysPolled_fd > 0)
	{
		close(GPIOKeysPolled_fd);
		//join thread
		if(GPIOKeysPolledThread != 0)
		{
			pthread_join( GPIOKeysPolledThread, NULL);
		}
	}
}
#if 0
/**********************************************************************
 *
 */
int write_GPIO(int GPIOLine, unsigned char value)
{
    int ret = 0;
    struct gpiohandle_data data;

    //memset(&data, 0, sizeof(data));

    if(HookControl.fd > 0)
    {
        data.values[GPIOLine] = value;
        ret = ioctl(HookControl.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data);

        if(ret < 0)
        {
            ret = -errno;
            fprintf(stderr, "Failed to issue %s (%d), %s\n",
                "GPIOHANDLE_SET_LINE_VALUES_IOCTL", ret, strerror(errno));
        }
    }
    else
    {
        printf("GPIO Bank fd <= 0\n");
    }

    return ret;
}

/**********************************************************************
 *
 */
int read_GPIO(int GPIOLine, struct gpiohandle_data *data)
{
    int ret = 0;

    memset(&data, 0, sizeof(data));

    if(HookControl.fd > 0)
    {
        ret = ioctl(HookControl.fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, data);
    }
    else
    {
        ret = -errno;
        fprintf(stderr, "Failed to issue %s (%d), %s\n",
            "GPIOHANDLE_SET_LINE_VALUES_IOCTL", ret, strerror(errno));
    }

    return ret;
}

/**********************************************************************
 *
 */
void SetHookswitch(unsigned char hook_sw_status)
{
    //printf("Setting hookswitch with value %d\n", hook_sw_status);
    write_GPIO(GPIO_OFFHOOK, hook_sw_status);
}
#endif
/********************************************************************************************************************************
 *  void function(void)
 *
 *  Desc
 *
 *  Parameters:
 *      - none
 *
 *  Returns:  none
 *
 *  Notes:  notes
 ********************************************************************************************************************************/
void *GPIOKeys(void *arg)
{
    /* how many bytes were read */
    ssize_t rb;
    /* the events (up to 64 at once) */
    struct input_event ev[64];
    int yalv;
    fd_set read_fds, write_fds, except_fds;
    struct timeval gpiokeys_timeout;

    while(GPIO_keys_keepGoing)
    {
        //set timeout to read function
        FD_ZERO(&read_fds);
        FD_ZERO(&write_fds);
        FD_ZERO(&except_fds);
        FD_SET(GPIOKeys_fd, &read_fds);

        //set timeout
        gpiokeys_timeout.tv_sec = 0;
        gpiokeys_timeout.tv_usec = 100*1000;    //100ms because ring detect requires it for timeout value

        // Wait for input to become ready or until the time out; the first parameter is
        // 1 more than the largest file descriptor in any of the sets
        int fdsetret = select((GPIOKeys_fd+1), &read_fds, NULL, NULL, &gpiokeys_timeout);

        //if we are shutting down do not run anything in this thread
        if(GPIO_keys_keepGoing == FALSE)
        {
            break;
        }

        if(fdsetret < 0)
        {
            perror("keyboard Select Error");
        }
        else if (fdsetret == 1)
        {
            rb=read(GPIOKeys_fd,ev,sizeof(struct input_event)*64);

            if(rb < 0)
            {
                perror("gpio key: error read");
            }
            else if (rb < (int) sizeof(struct input_event))
            {
                perror("gpio key: short read");
            }
            else if (rb >= (int) sizeof(struct input_event))
            {
                for (yalv = 0;
                     yalv < (int) (rb / sizeof(struct input_event));
                     yalv++)
                {
                    //if (EV_SW == ev[yalv].type)
                	if ((EV_KEY == ev[yalv].type) || (EV_SW == ev[yalv].type))
                    {
                		printf("Key detected!\n");
                        switch(ev[yalv].code)
                        {
                            case HANDSET_OFFHOOK:
                            {
								if(ev[yalv].value == 1)
								{
									printf("Off hook\n");
								}
								else
								{
									printf("On hook\n");
								}
                            }
                            break;

                            case RING_EVENT_ON:
                            {
								if(ev[yalv].value == 1)
								{
									printf("Ring On\n");
								}
								else
								{
									printf("Ring Off\n");
								}
                            }
                            break;

                            case LINE_IN_USE_HIGH:
                            {
								if(ev[yalv].value == 1)
								{
									printf("Line in Use High\n");
								}
								else
								{
									printf("Line in Use Low\n");
								}
                            }
							break;

                            case LOOP_DETECT_HIGH:
                            {
								if(ev[yalv].value == 1)
								{
									printf("Loop Detect High\n");
								}
								else
								{
									printf("Loop Detect Low\n");
								}
                            }
							break;

                            default:
                            	if(ev[yalv].value >= 1)
								{
									printf("Unknown On\n");
								}
								else
								{
									printf("Unknown Other: %d\n", ev[yalv].value);
								}
                            break;
                        }
                    }
                    else
                    {
                    	if(ev[yalv].code != 0)
                    	{
                    		printf("Other kind of type: %d, code: %d, value: %d\n",
                    	                    			ev[yalv].type, ev[yalv].code, ev[yalv].value);
                    	}
                    }
                }
            }
        }
        else
        {
        	//printf("Nothing to see here fdsetret = %d\n", fdsetret);
        }
    }

    GPIO_keys_keepGoing = FALSE;

    return 0;
}

/********************************************************************************************************************************
 *  void function(void)
 *
 *  Desc
 *
 *  Parameters:
 *      - none
 *
 *  Returns:  none
 *
 *  Notes:  notes
 ********************************************************************************************************************************/
void *GPIOKeysPolled(void *arg)
{
    /* how many bytes were read */
    ssize_t rb;
    /* the events (up to 64 at once) */
    struct input_event ev[64];
    int yalv;
    fd_set read_fds, write_fds, except_fds;
    struct timeval gpiokeys_timeout;

    while(GPIO_keys_polled_keepGoing)
    {
        //set timeout to read function
        FD_ZERO(&read_fds);
        FD_ZERO(&write_fds);
        FD_ZERO(&except_fds);
        FD_SET(GPIOKeysPolled_fd, &read_fds);

        //set timeout
        gpiokeys_timeout.tv_sec = 0;
        gpiokeys_timeout.tv_usec = 100*1000;    //100ms because ring detect requires it for timeout value

        // Wait for input to become ready or until the time out; the first parameter is
        // 1 more than the largest file descriptor in any of the sets
        int fdsetret = select((GPIOKeysPolled_fd+1), &read_fds, NULL, NULL, &gpiokeys_timeout);

        //if we are shutting down do not run anything in this thread
        if(GPIO_keys_polled_keepGoing == FALSE)
        {
            break;
        }

        if(fdsetret < 0)
        {
            //perror("keyboard Select Error");
        }
        else if (fdsetret == 1)
        {
            rb=read(GPIOKeysPolled_fd,ev,sizeof(struct input_event)*64);

            if(rb < 0)
            {
                perror("gpio key polled: error read");
            }
            else if (rb < (int) sizeof(struct input_event))
            {
                perror("gpio key polled: short read");
            }
            else if (rb >= (int) sizeof(struct input_event))
            {
                for (yalv = 0;
                     yalv < (int) (rb / sizeof(struct input_event));
                     yalv++)
                {
                	if (EV_SW == ev[yalv].type)
                    {
                        switch(ev[yalv].code)
                        {
                            case HANDSET_OFFHOOK:
                            {
								if(ev[yalv].value == 1)
								{
									printf("Off hook\n");
								}
								else
								{
									printf("On hook\n");
								}
                            }
                            break;

                            case RING_EVENT_ON:
                            {
								if(ev[yalv].value == 1)
								{
									printf("Ring On\n");
								}
								else
								{
									printf("Ring Off\n");
								}
                            }
                            break;

                            case LINE_IN_USE_HIGH:
                            {
								if(ev[yalv].value == 1)
								{
									printf("Line in Use High\n");
								}
								else
								{
									printf("Line in Use Low\n");
								}
                            }
							break;

                            case LOOP_DETECT_HIGH:
                            {
								if(ev[yalv].value == 1)
								{
									printf("Loop Detect High\n");
								}
								else
								{
									printf("Loop Detect Low\n");
								}
                            }
							break;

                            default:
                                //
                            break;
                        }
                    }
                    else
                    {
                    	printf("Other kind of type polled: %d, code: %d, value: %d\n",
                    			ev[yalv].type, ev[yalv].code, ev[yalv].value);
                    }
                }
            }
        }
    }

    GPIO_keys_polled_keepGoing = FALSE;

    return 0;
}
