/*
 * Keyboard_Test_Alternative.c
 *
 *  Created on: Mar 29, 2024
 *      Author: michaeljacobson
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

extern int GPIO_Dev_fd;

struct gpiohandle_request KeyboardLinesCols;
struct gpiohandle_request KeyboardLinesRows;
char GPIO_keyboard_keepGoing = 1;

pthread_t GPIOKeyboardThread = 0;

void InitGPIOKeyboard(void);
void CloseGPIOKeyboard(void);
void *GPIOKeyboard(void *arg);

void PerformKeyboardScan(void);
int DecodeKeypress(int column, struct gpiohandle_data data);

/**********************************************************************
 *
 */
void InitGPIOKeyboard(void)
{
	printf("Init Keyboard\n");
	pthread_create( &GPIOKeyboardThread, NULL, GPIOKeyboard, NULL );
}

/**********************************************************************
 *
 */
void CloseGPIOKeyboard(void)
{
	GPIO_keyboard_keepGoing = 0;

	//join thread
	if(GPIOKeyboardThread != 0)
	{
		pthread_join( GPIOKeyboardThread, NULL);
	}
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
void *GPIOKeyboard(void *arg)
{
	struct gpiochip_info cinfo;
	int Colsfd = 0;
	int Rowsfd = 0;
	int ret = 0;
	int good_to_go = 0;

	if(GPIO_Dev_fd > 0)
	{
		ret = ioctl(GPIO_Dev_fd, GPIO_GET_CHIPINFO_IOCTL, &cinfo);
		if(ret >= 0)
		{
			//get all the keys we need to read and write
			memset(&KeyboardLinesCols, 0, sizeof(KeyboardLinesCols));

			KeyboardLinesCols.lineoffsets[0] = (3*8 + 0);
			KeyboardLinesCols.lineoffsets[1] = (2*8 + 0);
			KeyboardLinesCols.lineoffsets[2] = (2*8 + 1);
			KeyboardLinesCols.lineoffsets[3] = (40*8 + 2);
			KeyboardLinesCols.lines = 4;
			KeyboardLinesCols.flags = GPIOHANDLE_REQUEST_OUTPUT;
			strcpy(KeyboardLinesCols.consumer_label, "Keyboard_Cols");

			Colsfd = ioctl(GPIO_Dev_fd, GPIO_GET_LINEHANDLE_IOCTL, &KeyboardLinesCols);
			if(Colsfd < 0)
			{
				ret = -errno;
				fprintf(stderr, " Failed to request %s (%d), %s\n",
								"keyboard Cols GPIOs IOCTL", ret, strerror(errno));
			}
			else
			{
				good_to_go++;
			}

			memset(&KeyboardLinesRows, 0, sizeof(KeyboardLinesRows));

			KeyboardLinesRows.lineoffsets[0] = (41*8 + 0);
			KeyboardLinesRows.lineoffsets[1] = (43*8 + 1);
			KeyboardLinesRows.lineoffsets[2] = (42*8 + 2);
			KeyboardLinesRows.lineoffsets[3] = (42*8 + 3);
			KeyboardLinesRows.lineoffsets[4] = (3*8 + 1);
			KeyboardLinesRows.lines = 5;
			KeyboardLinesRows.flags = GPIOHANDLE_REQUEST_INPUT;
			strcpy(KeyboardLinesRows.consumer_label, "Keyboard_Rows");

			Rowsfd = ioctl(GPIO_Dev_fd, GPIO_GET_LINEHANDLE_IOCTL, &KeyboardLinesRows);
			if(Rowsfd < 0)
			{
				ret = -errno;
				fprintf(stderr, " Failed to request %s (%d), %s\n",
								"keyboard Rows GPIOs IOCTL", ret, strerror(errno));
			}
			else
			{
				good_to_go++;
			}

			if(good_to_go == 2)
			{
				while(GPIO_keyboard_keepGoing)
				{
					PerformKeyboardScan();

					usleep(4*1000); //4ms
				}
			}
		}
		else
		{
			ret = -errno;
			fprintf(stderr, " Failed to request %s (%d), %s\n",
							"Failed GPIO Keyboard GPIO_GET_CHIPINFO_IOCTL", ret, strerror(errno));
		}
	}

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
void PerformKeyboardScan(void)
{
	int ret = 0;
	struct gpiohandle_data col_data;
	struct gpiohandle_data row_data;
	int scan_key[4];
	static int old_scan_key[4];
	int i = 0;

	memset(scan_key, 0, sizeof(scan_key));
	//memset(old_scan_key, 0, sizeof(old_scan_key));

	if((KeyboardLinesCols.fd > 0) && (KeyboardLinesRows.fd > 0))
	{
		for(i = 0; i < KeyboardLinesCols.lines; i++)
		{
			//start off by clearing all but 1 column to high
			col_data.values[0] = (i != 0);
			col_data.values[1] = (i != 1);
			col_data.values[2] = (i != 2);
			col_data.values[3] = (i != 3);

			ret = ioctl(KeyboardLinesCols.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &col_data);

			if(ret < 0)
			{
				ret = -errno;
				fprintf(stderr, "Failed to issue %s (%d), %s\n",
					"GPIO Column GPIOHANDLE_SET_LINE_VALUES_IOCTL", ret, strerror(errno));
			}
			else
			{
				//wait ius for settling
				usleep(1);

				//read the rows
				memset(&row_data, 0, sizeof(row_data));

				ret = ioctl(KeyboardLinesRows.fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &row_data);

				if(ret < 0)
				{
					ret = -errno;
					fprintf(stderr, "Failed to issue %s (%d), %s\n",
						"GPIO Rows GPIOHANDLE_GET_LINE_VALUES_IOCTL", ret, strerror(errno));
				}
				else
				{
					//we got the data, turn it into something we can decode
					scan_key[i] = DecodeKeypress(i, row_data);

					if(old_scan_key[i] == scan_key[i])
					{
						//holding down the button
					}
					else
					{
						//do something?
						switch(scan_key[i])
						{
							case BUT_SPKRPHN:
								//
								printf("Speakerphone Button\n");
							break;

							case BUT_MUTE:
								//
								printf("Mute Button\n");
							break;

							case BUT_CAPTION:
								//
								printf("Captions Button\n");
							break;

							case BUT_VOLDN:
								//
								printf("Volume Down Button\n");
							break;

							case BUT_VOLUP:
								//
								printf("Volume Up Button\n");
							break;

							case BUT_CSTMR_SRVC:
								//
								printf("Customer Service Button\n");
							break;

							case BUT_HOME:
								//
								printf("Home Button\n");
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
								printf("DTMF Button %c\n", (scan_key[i] & 0x3F));
							break;

							case BUT_DTMFST:
								//
								printf("DTMF Button *\n");
							break;

							case BUT_DTMFLB:
								//
								printf("DTMF Button #\n");
							break;

							default:
								//
							break;

						}
					}

					old_scan_key[i] = scan_key[i];
				}
			}
		}
	}
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
int DecodeKeypress(int column, struct gpiohandle_data rows)
{
	int return_keypress = 0;

	switch(column)
	{
		case 0:
			if(rows.values[0] == 0)
			{
				return_keypress = BUT_VOLUP;
			}
			else if(rows.values[1] == 0)
			{
				return_keypress = BUT_DTMF1;
			}
			else if(rows.values[2] == 0)
			{
				return_keypress = BUT_DTMF4;
			}
			else if(rows.values[3] == 0)
			{
				return_keypress = BUT_DTMF7;
			}
			else if(rows.values[4] == 0)
			{
				return_keypress = BUT_DTMFST;
			}
		break;

		case 1:
			if(rows.values[0] == 0)
			{
				return_keypress = BUT_VOLDN;
			}
			else if(rows.values[1] == 0)
			{
				return_keypress = BUT_DTMF2;
			}
			else if(rows.values[2] == 0)
			{
				return_keypress = BUT_DTMF5;
			}
			else if(rows.values[3] == 0)
			{
				return_keypress = BUT_DTMF8;
			}
			else if(rows.values[4] == 0)
			{
				return_keypress = BUT_DTMF0;
			}
		break;

		case 2:
			if(rows.values[0] == 0)
			{
				return_keypress = 0;
			}
			else if(rows.values[1] == 0)
			{
				return_keypress = BUT_DTMF3;
			}
			else if(rows.values[2] == 0)
			{
				return_keypress = BUT_DTMF6;
			}
			else if(rows.values[3] == 0)
			{
				return_keypress = BUT_DTMF9;
			}
			else if(rows.values[4] == 0)
			{
				return_keypress = BUT_DTMFLB;
			}
		break;

		case 3:
			if(rows.values[0] == 0)
			{
				return_keypress = BUT_CSTMR_SRVC;
			}
			else if(rows.values[1] == 0)
			{
				return_keypress = BUT_HOME;
			}
			else if(rows.values[2] == 0)
			{
				return_keypress = BUT_CAPTION;
			}
			else if(rows.values[3] == 0)
			{
				return_keypress = BUT_SPKRPHN;
			}
			else if(rows.values[4] == 0)
			{
				return_keypress = BUT_MUTE;
			}
		break;

		default:
			return_keypress = 0;
		break;
	}

	return return_keypress;
}

