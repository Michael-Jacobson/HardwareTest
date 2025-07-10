/*
 * i2c_test.c
 *
 *  Created on: Jul 23, 2024
 *      Author: michaeljacobson
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <errno.h>
#include "i2c_test.h"


#define I2C_BUS_0_PATH	"/dev/i2c-0"
#define I2C_BUS_1_PATH	"/dev/i2c-1"
#define I2C_BUS_3_PATH	"/dev/i2c-3"



int i2c0_fd = 0;
int i2c1_fd = 0;
int i2c3_fd = 0;

/**********************************************************************
 *
 */
void init_i2c(char which_i2c)
{
	switch(which_i2c)
	{
		case 0:
			if(i2c0_fd == 0)
			{
				i2c0_fd = open(I2C_BUS_0_PATH, O_RDWR);
			}
			else
			{
				printf("i2c 0 is already open!\n");
			}
		break;

		case 1:
			if(i2c1_fd == 0)
			{
				i2c1_fd = open(I2C_BUS_1_PATH, O_RDWR);
			}
			else
			{
				printf("i2c 1 is already open!\n");
			}
		break;

		case 3:
			if(i2c3_fd == 0)
			{
				i2c3_fd = open(I2C_BUS_3_PATH, O_RDWR);
			}
			else
			{
				printf("i2c 3 is already open!\n");
			}
		break;

		case ALL_I2C_BUSES:
			if(i2c0_fd == 0)
			{
				i2c0_fd = open(I2C_BUS_0_PATH, O_RDWR);
			}

			if(i2c1_fd == 0)
			{
				i2c1_fd = open(I2C_BUS_1_PATH, O_RDWR);
			}

			if(i2c3_fd == 0)
			{
				i2c3_fd = open(I2C_BUS_3_PATH, O_RDWR);
			}
		break;

		default:
			printf("Not Valid i2c option open: %d\n", which_i2c);
		break;
	}
}

/**********************************************************************
 *
 */
void close_i2c(char which_i2c)
{
	switch(which_i2c)
	{
		case 0:
			if(i2c0_fd != 0)
			{
				close(i2c0_fd);
			}
		break;

		case 1:
			if(i2c1_fd != 0)
			{
				close(i2c1_fd);
			}
		break;

		case 3:
			if(i2c3_fd != 0)
			{
				close(i2c3_fd);
			}
		break;

		case ALL_I2C_BUSES:
			if(i2c0_fd != 0)
			{
				close(i2c0_fd);
			}

			if(i2c1_fd != 0)
			{
				close(i2c1_fd);
			}

			if(i2c3_fd != 0)
			{
				close(i2c3_fd);
			}
		break;

		default:
			printf("Not Valid i2c option close: %d\n", which_i2c);
		break;
	}
}


/**********************************************************************
 *
 */
int read_i2c(char which_i2c, char device_addr, unsigned char reg_addr, unsigned char *p_data, int len)
{
	int *p_fd_used = NULL;
	int data_returned = 0;

	switch(which_i2c)
	{
		case 0:
			p_fd_used = &i2c0_fd;
		break;

		case 1:
			p_fd_used = &i2c1_fd;
		break;

		case 3:
			p_fd_used = &i2c3_fd;
		break;

		default:
			printf("Not Valid i2c option read: %d\n", which_i2c);
		break;
	}

	if((p_fd_used != NULL) && (*p_fd_used != 0))
	{
		struct i2c_msg messages[2] ;
		struct i2c_rdwr_ioctl_data ioctl_data = { messages, 2 };

		messages[0].addr = device_addr;
		messages[0].flags = 0;
		messages[0].len = 1;
		messages[0].buf = &reg_addr;

		messages[1].addr = device_addr;
		messages[1].flags = I2C_M_RD;
		messages[1].len = len;
		messages[1].buf = p_data;

		int result = ioctl(*p_fd_used, I2C_RDWR, &ioctl_data);
		if (result < 0)
		{
			printf("failed to i2c read fd=%d i2c bus=%d device=%d register=%d length=%d, error=%d, %s\n",
					*p_fd_used, which_i2c, device_addr, reg_addr, len, errno, strerror(errno));
			data_returned = -1;
		}
		else
		{
			data_returned = len;
		}
	}
	else
	{
		printf("Bad fd i2c read\n");
	}

	return data_returned;
}


/**********************************************************************
 *
 */
int write_i2c(char which_i2c, char device_addr, unsigned char reg_addr, unsigned char *p_data, int len)
{
	int *p_fd_used = NULL;
	int data_written = 0;

	switch(which_i2c)
	{
		case 0:
			p_fd_used = &i2c0_fd;
		break;

		case 1:
			p_fd_used = &i2c1_fd;
		break;

		case 3:
			p_fd_used = &i2c3_fd;
		break;

		default:
			printf("Not Valid i2c option read: %d\n", which_i2c);
		break;
	}

	if((p_fd_used != NULL) && (*p_fd_used != 0) && (len <= 100))
	{
		unsigned char command[100];
		struct i2c_msg message = { device_addr, 0, (len+1), command };
		struct i2c_rdwr_ioctl_data ioctl_data = { &message, 1 };

		memset(command, 0, sizeof(command));

		command[0] = reg_addr;

		memcpy(&command[1], p_data, len);

		int result = ioctl(*p_fd_used, I2C_RDWR, &ioctl_data);
		if (result < 0)
		{
			printf("failed to i2c write fd=%d i2c bus=%d device=%d register=%d length=%d error=%d, %s\n",
					*p_fd_used, which_i2c, device_addr, reg_addr, len, errno, strerror(errno));
			data_written = -1;
		}
		else
		{
			data_written = len;
		}
	}
	else
	{
		printf("Bad fd i2c read or too long. len = %d\n", len);
	}

	return data_written;
}

/**********************************************************************
 *
 */
void Test_DigitPOT(void)
{
	unsigned char value = 0;
	char pass = 0;

#if 0
	if(read_i2c(0, 0x28, 0, &value, 1) > 0)
	{
		unsigned char new_value = value + 1;

		usleep(100000);

		if(write_i2c(0, 0x28, 0, &new_value, 1) > 0)
		{
			printf("New Value Written=%d\n", new_value);
			new_value = 0;

			usleep(100000);

			if(read_i2c(0, 0x28, 0, &new_value, 1) > 0)
			{
				printf("New Value Read=%d\n", new_value);
				if(new_value == (value + 1))
				{
					printf("DigiPOT pass: %d=%d+1\n", new_value, value);
					pass = 1;
				}
			}
		}
	}
#else
	if(read_i2c(0, 0x28, 0, &value, 1) > 0)
	{
		printf("DigiPOT pass: %d\n", value);
		pass = 1;
	}
#endif

	if(!pass)
	{
		printf("DigiPOT Failed\n");
	}
}






