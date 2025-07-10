/********************************************************************************************************************************/
/********************************************************************************************************************************/
/*                                                       CodecAIC3106.c                                                      */
/*                                COPYRIGHT NOTICE: (c) Ultratec, Inc.  2016  ALL RIGHTS RESERVED                               */
/********************************************************************************************************************************/
/*                                                                                                                              */
/* This file contains                      */
/*                                                                                                                              */
/* Product:                                                                                                        */
/*                                                                                                                              */
/********************************************************************************************************************************/
/****************
 * Includes
 ****************/
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

#include "C674xTypes.h"
#include "Ultratec_Enums.h"
#include "CodecAIC3106.h"
#include "i2c_test.h"
//#include "Misra_Rules.h"                  /* MISRA rules are always last.  Open the header file for an explanation on why. */

/**************
 * Defines
 *************/

/*********************
 * Global Variables
 *********************/

/**********************
 * Static Variables
 **********************/
DigitalEffectsFilter_t NoFilter = {
                                   FALSE,
                                   TRUE,
                                   0,
                                   0,
                                   0,
                                   0,
                                   0,
                                   0,
                                   0,
                                   0,
                                   0,
                                   0 };

/*
 * Text "* Sample Rate = 8000"
 Text "* Filter 1 High Pass 2nd Order Butterworth, 200 Hz Fc 0.0 dB "
 Text "* Filter 2 EQ 2000 Hz Fc 6.0 dB 1000 Hz BW"

 Text "* BQ: H(z) = (N0 + 2*N1/z + N2/(z2)) / (32768 - 2*D1/z - D2/(z2))"
 Text "* Filter Coefficients in format N0, N1, N2, D1, D2"

 Text "* 1O: H(z) = (N0 + N1/z) / (32768 - D1/z)"
 Text "* Filter Coefficients in format N0, N1, D1"

 Text "* Filter 1 BQ "
 0x7289
 0x8D77
 0x7289
 0x71D3
 0x9981
 Text "* Filter 2 BQ "
 0x7FFF
 0x0000
 0x0C29
 0x0000
 0xCAFC
 */
DigitalEffectsFilter_t TIA470Filter = {
                                        TRUE,
                                        TRUE,
                                        0x7289,
                                        0x8D77,
                                        0x7289,
                                        0x71D3,
                                        0x9981,
                                        0x7FFF,
                                        0x0000,
                                        0x0C29,
                                        0x0000,
                                        0xCAFC };

/*
 * Target 0db +/- 4dB HFE
 * Text "* Sample Rate = 8000"
 Text "* Filter 1 EQ 1250 Hz Fc -9.0 dB 750 Hz BW"
 Text "* Filter 2 EQ 400 Hz Fc -4.0 dB 200 Hz BW"

 Text "* BQ: H(z) = (N0 + 2*N1/z + N2/(z2)) / (32768 - 2*D1/z - D2/(z2))"
 Text "* Filter Coefficients in format N0, N1, N2, D1, D2"

 Text "* 1O: H(z) = (N0 + N1/z) / (32768 - D1/z)"
 Text "* Filter Coefficients in format N0, N1, D1"

 Text "* Filter 1 BQ "
 0x59EF
 0xD9AB
 0x3012
 0x2655
 0xF5FE
 Text "* Filter 2 BQ "
 0x7AC1
 0x93C5
 0x68D8
 0x6C3B
 0x9C65
 */
DigitalEffectsFilter_t FlatFilter = {
                                      TRUE,
                                      TRUE,
                                      0x59EF,
                                      0xD9AB,
                                      0x3012,
                                      0x2655,
                                      0xF5FE,
                                      0x7AC1,
                                      0x93C5,
                                      0x68D8,
                                      0x6C3B,
                                      0x9C65 };
/*
 * Target 9db +/- 4dB HFE
 Text "* Sample Rate = 8000"
 Text "* Filter 1 Treble Shelf 2250 Hz Fc 6.0 dB"
 Text "* Filter 2 Bass Shelf 400 Hz Fc -12.0 dB"

 Text "* BQ: H(z) = (N0 + 2*N1/z + N2/(z2)) / (32768 - 2*D1/z - D2/(z2))"
 Text "* Filter Coefficients in format N0, N1, N2, D1, D2"

 Text "* 1O: H(z) = (N0 + N1/z) / (32768 - D1/z)"
 Text "* Filter Coefficients in format N0, N1, D1"

 Text "* Filter 1 BQ "
 0x7FFF
 0x0200
 0x15FB
 0xE494
 0xE5DD
 Text "* Filter 2 BQ "
 0x680B
 0xAE10
 0x43B4
 0x4C12
 0xC885
 */
DigitalEffectsFilter_t SlightFilter = {
                                        TRUE,
                                        TRUE,
                                        0x7FFF,
                                        0x0200,
                                        0x15FB,
                                        0xE494,
                                        0xE5DD,
                                        0x680B,
                                        0xAE10,
                                        0x43B4,
                                        0x4C12,
                                        0xC885 };

/*
 * Target 14db +/- 4dB HFE
 Text "* Sample Rate = 8000"
 Text "* Filter 1 Treble Shelf 2000 Hz Fc 12.0 dB"
 Text "* Filter 2 Bass Shelf 400 Hz Fc -12.0 dB"

 Text "* BQ: H(z) = (N0 + 2*N1/z + N2/(z2)) / (32768 - 2*D1/z - D2/(z2))"
 Text "* Filter Coefficients in format N0, N1, N2, D1, D2"

 Text "* 1O: H(z) = (N0 + N1/z) / (32768 - D1/z)"
 Text "* Filter Coefficients in format N0, N1, D1"

 Text "* Filter 1 BQ "
 0x7FFF
 0xD13E
 0x223F
 0xFD79
 0xEA02
 Text "* Filter 2 BQ "
 0x680B
 0xAE10
 0x43B4
 0x4C12
 0xC885
 */
DigitalEffectsFilter_t SteepMildFilter = {
                                           TRUE,
                                           TRUE,
                                           0x7FFF,
                                           0xD13E,
                                           0x223F,
                                           0xFD79,
                                           0xEA02,
                                           0x680B,
                                           0xAE10,
                                           0x43B4,
                                           0x4C12,
                                           0xC885 };

/*
 * Target 25db +/- 4dB HFE
 Text "* Sample Rate = 8000"
 Text "* Filter 1 High Pass 2nd Order Butterworth, 2250 Hz Fc 12.0 dB "

 Text "* BQ: H(z) = (N0 + 2*N1/z + N2/(z2)) / (32768 - 2*D1/z - D2/(z2))"
 Text "* Filter Coefficients in format N0, N1, N2, D1, D2"

 Text "* 1O: H(z) = (N0 + N1/z) / (32768 - D1/z)"
 Text "* Filter Coefficients in format N0, N1, D1"

 Text "* Filter 1 BQ "
 0x7916
 0x86EA
 0x7916
 0xF142
 0xE8D9
 32768
 */
DigitalEffectsFilter_t SteepModerateFilter = {
                                               TRUE,
                                               TRUE,
                                               0x7916,
                                               0x86EA,
                                               0x7916,
                                               0xF142,
                                               0xE8D9,
                                               32768,
                                               0,
                                               0,
                                               0,
                                               0 };

/*
 * Target 21db +/- 4dB HFE
 Text "* Sample Rate = 8000"
 Text "* Filter 1 High Pass 2nd Order Butterworth, 1500 Hz Fc 12.0 dB "

 Text "* BQ: H(z) = (N0 + 2*N1/z + N2/(z2)) / (32768 - 2*D1/z - D2/(z2))"
 Text "* Filter Coefficients in format N0, N1, N2, D1, D2"

 Text "* 1O: H(z) = (N0 + N1/z) / (32768 - D1/z)"
 Text "* Filter Coefficients in format N0, N1, D1"

 Text "* Filter 1 BQ "
 0x7FFF
 0x8001
 0x7FFF
 0x1DA0
 0xE52B
 32768
 */
DigitalEffectsFilter_t SteepSevereFilter = {
                                             TRUE,
                                             TRUE,
                                             0x7FFF,
                                             0x8001,
                                             0x7FFF,
                                             0x1DA0,
                                             0xE52B,
                                             32768,
                                             0,
                                             0,
                                             0,
                                             0 };

/*****************************/
/* Local Function Prototypes */
/*****************************/
static BOOL Codec_i2c_read(U8 I2CAddr, U8 RegAddr, U8 *p_read_data);
static BOOL Codec_i2c_write(U8 I2CAddr, U8 RegAddr, U8 write_data);

static BOOL WriteToCodecReg(U8 RegisterNum, U8 RegisterValue, BOOL verify, U8 Mask);

/********************************************************************************************************************************/
/********************************************************************************************************************************/
/********************************************Global Routines*********************************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/

/********************************************************************************************************************************/
static BOOL Codec_i2c_read(U8 I2CAddr, U8 RegAddr, U8 *p_read_data)
{
    BOOL pass = FALSE;

    //usleep(100000);

	if(read_i2c(0, I2CAddr, RegAddr, p_read_data, 1) >= 0)
	{
		pass = TRUE;
	}

    return (pass);
}

/********************************************************************************************************************************/

/********************************************************************************************************************************/
static BOOL Codec_i2c_write(U8 I2CAddr, U8 RegAddr, U8 write_data)
{
    BOOL pass = FALSE;

    //usleep(100000);

    if(write_i2c(0, I2CAddr, RegAddr, &write_data, 1) >= 0)
    {
    	pass = TRUE;
    }

    return (pass);
}

/*******************************************************************************************************************************
 * Function
 *
 * Description
 *
 * Parameters:
 *       -None
 *
 * Returns: None
 *
 * Notes:
 *
 ******************************************************************************************************************************/
BOOL InitCodecAIC3106(void)
{
    BOOL pass = TRUE;
    U8 WriteData = 0;

    //reset codec
    //set to page 0
    if (WriteToCodecReg(0, 0, TRUE, CODEC_REG_READ_DEFAULT_MASK))
    {
#if 0
    	//do a reset to start
    	WriteData = CODEC_PAGE0_REG1_RESET;
		if (WriteToCodecReg(1, WriteData, FALSE, CODEC_REG_READ_DEFAULT_MASK) == FALSE)
		{
			pass = FALSE;
		}

		usleep(100);
#endif
        //**********************************************************************************
        //set ADC/DAC volume to mute user audio    ************************************************************
        WriteData = CODEC_PAGE0_REG15_LEFT_ADC_MUTE;
        if (WriteToCodecReg(15, WriteData, TRUE, CODEC_REG_READ_DEFAULT_MASK) == FALSE)
        {
            pass = FALSE;
        }

        WriteData = CODEC_PAGE0_REG16_RIGHT_ADC_PGA_0DB;
        if (WriteToCodecReg(16, WriteData, TRUE, CODEC_REG_READ_DEFAULT_MASK) == FALSE)
        {
            pass = FALSE;
        }

        WriteData = CODEC_PAGE0_REG43_LEFT_DAC_MUTE;
        if (WriteToCodecReg(43, WriteData, TRUE, CODEC_REG_READ_DEFAULT_MASK) == FALSE)
        {
            pass = FALSE;
        }

        WriteData = DAC_MINUS_6DB;
        if (WriteToCodecReg(44, WriteData, TRUE, CODEC_REG_READ_DEFAULT_MASK) == FALSE)
        {
            pass = FALSE;
        }

        //**********************************************************************************
        //set up pll values    ************************************************************
#if 0
        WriteData = CODEC_PAGE0_REG2_ADC_FS | CODEC_PAGE0_REG2_DAC_FS;
        if (WriteToCodecReg(2, WriteData, TRUE, CODEC_REG_READ_DEFAULT_MASK) == FALSE)
        {
            pass = FALSE;
        }
#endif
        WriteData = CODEC_PAGE0_REG12_LEFT_HIGH_PASS_025 | CODEC_PAGE0_REG12_RIGHT_HIGH_PASS_025;
        if (WriteToCodecReg(12, WriteData, TRUE, CODEC_REG_READ_DEFAULT_MASK) == FALSE)
        {
            pass = FALSE;
        }
#if 0
        WriteData = CODEC_PAGE0_REG3_SET_Q;
        if (WriteToCodecReg(3, WriteData, TRUE, CODEC_REG_READ_DEFAULT_MASK) == FALSE)
        {
            pass = FALSE;
        }
#endif
        //common mode voltage
        WriteData = CODEC_PAGE0_REG40_CM_VOLTAGE_1P65;
        if (WriteToCodecReg(40, WriteData, TRUE, CODEC_REG_READ_DEFAULT_MASK) == FALSE)
        {
            pass = FALSE;
        }

        //*******************************************************************************************
        //power up legs and routing.    ************************************************************
        //we use LINE1L, LINE1R, LINE2R ADCs and MONOLO, RIGHTLO, LEFTLO DACs

        ////ADCs//////////////////////////////////////////////
        //power up left ADC and leave LINE1L (handset) unconnected
        WriteData = CODEC_PAGE0_REG19_LINE1L_DIFF | CODEC_PAGE0_REG19_LINE1L_OFF_LEFT_ADC | CODEC_PAGE0_REG19_LEFT_ADC_POWERED_UP | CODEC_PAGE0_REG19_LEFT_ADC_NO_SOFT_STEP;
        if (WriteToCodecReg(19, WriteData, TRUE, CODEC_REG_READ_DEFAULT_MASK) == FALSE)
        {
            pass = FALSE;
        }

        //power up right ADC and leave LINE1R (ext mic) unconnected
        WriteData = CODEC_PAGE0_REG22_LINE1R_DIFF | CODEC_PAGE0_REG22_LINE1R_OFF_RIGHT_ADC | CODEC_PAGE0_REG22_RIGHT_ADC_POWERED_UP | CODEC_PAGE0_REG22_RIGHT_ADC_NO_SOFT_STEP;
        if (WriteToCodecReg(22, WriteData, TRUE, CODEC_REG_READ_DEFAULT_MASK) == FALSE)
        {
            pass = FALSE;
        }

        //connect right ADC to line2R POTS
        WriteData = CODEC_PAGE0_REG23_LINE2R_DIFF;
        if (WriteToCodecReg(23, WriteData, TRUE, CODEC_REG_READ_DEFAULT_MASK) == FALSE)
        {
            pass = FALSE;
        }

        //line1L differential
        WriteData = CODEC_PAGE0_REG24_LINE1L_DIFF | CODEC_PAGE0_REG24_LINE1L_OFF_RIGHT_ADC;
        if (WriteToCodecReg(24, WriteData, TRUE, CODEC_REG_READ_DEFAULT_MASK) == FALSE)
        {
            pass = FALSE;
        }

        /////DACs//////////////////////////////////
        //power up DACS
        WriteData = CODEC_PAGE0_REG37_LEFT_DAC_POWERED_UP | CODEC_PAGE0_REG37_RIGHT_DAC_POWERED_UP;
        if (WriteToCodecReg(37, WriteData, TRUE, CODEC_REG_READ_DEFAULT_MASK) == FALSE)
        {
            pass = FALSE;
        }

        //set DACs to play right and left data and not muted
        WriteData = (CODEC_PAGE0_REG7_LEFT_DAC_PLAYS_LEFT | CODEC_PAGE0_REG7_RIGHT_DAC_PLAYS_RIGHT);
        if (WriteToCodecReg(7, WriteData, TRUE, CODEC_REG_READ_DEFAULT_MASK) == FALSE)
        {
            pass = FALSE;
        }

        //set DACs to stereo differential
        WriteData = (CODEC_PAGE0_REG14_AC_COUPLED | CODEC_PAGE0_REG14_STEREO_DIFFERENTIAL);
        if (WriteToCodecReg(14, WriteData, TRUE, 0xC8) == FALSE)
        {
            pass = FALSE;
        }

        //route DACR1 to MONOLO - POTS, 0dB
        WriteData = CODEC_PAGE0_REG78_DACR1_TO_MONOLO;
        if (WriteToCodecReg(78, WriteData, TRUE, CODEC_REG_READ_DEFAULT_MASK) == FALSE)
        {
            pass = FALSE;
        }

        //route DACL1 to rightLO - external SPKR 0dB
        WriteData = CODEC_PAGE0_REG89_DACL1_TO_RIGHTLO;
        if (WriteToCodecReg(89, WriteData, TRUE, CODEC_REG_READ_DEFAULT_MASK) == FALSE)
        {
            pass = FALSE;
        }

        //route DACL1 to LeftLO - Handset 0dB
        WriteData = CODEC_PAGE0_REG82_DACL1_TO_LEFTLO;
        if (WriteToCodecReg(82, WriteData, TRUE, CODEC_REG_READ_DEFAULT_MASK) == FALSE)
        {
            pass = FALSE;
        }

        //power up MONOLO - POTS - add power up to mask so verify returns true 0dB
        WriteData = CODEC_PAGE0_REG79_MONOLO_UNMUTE | CODEC_PAGE0_REG79_MONOLO_POWERED_UP;
        if (WriteToCodecReg(79, WriteData, TRUE, 0xF8) == FALSE)
        {
            pass = FALSE;
        }

        //power up LEFT_LO - handset spkr - 0dB
        WriteData = CODEC_PAGE0_REG86_LEFTLO_POWERED_UP;
        if (WriteToCodecReg(86, WriteData, TRUE, 0xFD) == FALSE)
        {
            pass = FALSE;
        }

        //Power up RIGHTLO - external speaker - add power up to mask so verify returns true 0dB
        WriteData = CODEC_PAGE0_REG93_RIGHTLO_POWERED_UP;
        if (WriteToCodecReg(93, WriteData, TRUE, 0xFD) == FALSE)
        {
            pass = FALSE;
        }
#if 0
        //**********************************************************************************
        //set mode    ************************************************************
        WriteData = CODEC_PAGE0_REG9_DSP_MODE;
        if (WriteToCodecReg(9, WriteData, TRUE, CODEC_REG_READ_DEFAULT_MASK) == FALSE)
        {
            pass = FALSE;
        }

        //**********************************************************************************
        //set bit offset    ************************************************************
        WriteData = CODEC_PAGE0_REG10_BIT_OFFSET;
        if (WriteToCodecReg(10, WriteData, TRUE, CODEC_REG_READ_DEFAULT_MASK) == FALSE)
        {
            pass = FALSE;
        }
#endif
        //**********************************************************************************
        //setup output pins    ************************************************************
#if 0
        WriteData = CODEC_PAGE0_REG8_BCLK_OUT | CODEC_PAGE0_REG8_WCLK_OUT | CODEC_PAGE0_REG8_BCLK_RUN | CODEC_PAGE0_REG8_DMIC_EN_128_SR;
        if (WriteToCodecReg(8, WriteData, TRUE, CODEC_REG_READ_DEFAULT_MASK) == FALSE)
        {
            pass = FALSE;
        }
#else
        if (Codec_i2c_read(CODECAIC3106_I2C_ADDR, 8, &WriteData))
		{
			WriteData |= CODEC_PAGE0_REG8_DMIC_EN_128_SR;

			if (WriteToCodecReg(8, WriteData, TRUE, CODEC_REG_READ_DEFAULT_MASK) == FALSE)
			{
				pass = FALSE;
			}
		}
#endif

        //**********************************************************************************
        //set digital Mic pins    **********************************************************
        WriteData = CODEC_PAGE0_REG98_GPIO1_DMIC_CLK;
        if (WriteToCodecReg(98, WriteData, TRUE, 0xFD) == FALSE)
        {
            pass = FALSE;
        }

        WriteData = CODEC_PAGE0_REG99_GPIO2_DMIC_DATA;
        if (WriteToCodecReg(99, WriteData, TRUE, 0xFB) == FALSE)
        {
            pass = FALSE;
        }

        //default to analog mics so the POTS line detector can ID the channel that POTS data is in
        WriteData = CODEC_PAGE0_REG107_ANALOG_MICS_USED;
        if (WriteToCodecReg(107, WriteData, TRUE, 0xFC) == FALSE)
        {
            pass = FALSE;
        }

        //enabled mic bias
        WriteData = CODEC_PAGE0_REG25_MICBIAS_2V;
        if (WriteToCodecReg(25, WriteData, TRUE, 0xC0) == FALSE)
        {
            pass = FALSE;
        }

        //100% current
        WriteData = CODEC_PAGE0_REG109_100_PERCENT_INCR;
        if (WriteToCodecReg(109, WriteData, TRUE, CODEC_REG_READ_DEFAULT_MASK) == FALSE)
        {
            pass = FALSE;
        }

        /*WriteData = ;
         if(WriteToCodecReg(, WriteData, TRUE, 0x) == FALSE)
         {
         pass = FALSE;
         }*/
    }

    return pass;
}

/********************************************************************************************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/
/********************************************Static Routines*********************************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/
/********************************************************************************************************************************/
/*******************************************************************************************************************************
 * Function
 *
 * Description
 *
 * Parameters:
 *       -None
 *
 * Returns: None
 *
 * Notes:
 *
 ******************************************************************************************************************************/
static BOOL WriteToCodecReg(U8 RegisterNum, U8 RegisterValue, BOOL verify, U8 Mask)
{
    BOOL pass = FALSE;
    U8 ReadData = 0;

    Codec_i2c_read(CODECAIC3106_I2C_ADDR, RegisterNum, &ReadData);

    //printf("Register %d has value: 0x%02x, writing: 0x%02x\n", RegisterNum, ReadData, RegisterValue);

    if (Codec_i2c_write(CODECAIC3106_I2C_ADDR, RegisterNum, RegisterValue))
    {
        if (verify)
        {
            if (Codec_i2c_read(CODECAIC3106_I2C_ADDR, RegisterNum, &ReadData))
            {
                if ((RegisterValue & Mask) == (ReadData & Mask))
                {
                    pass = TRUE;
                }
                else
                {
                	printf("Failed to set Register: Register=%d Value written=0x%x Value read=0x%x Mask=0x%x\n",
                			RegisterNum, RegisterValue, ReadData, Mask);
                    asm(" NOP");
                }
            }
            else
            {
            	printf("Verify Read Failed\n");
            }
        }
        else
        {
            pass = TRUE;
        }
    }
    else
    {
    	printf("Codec write failed\n");
    }

    return pass;
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
 *  Notes:  When you pass in a simple filter that only uses N0,1,2, D1,2 the right thing to do is to use the exact
 *  same values for N3,4,5, D4,5 because the filter it is creating does use all the variables, but if you load in all
 *  0's for the second order filter it's like taking the result of filter 1 and multiplying it by 0. see page 28 of
 *  the datasheet for a better representation on how the filter work.
 ********************************************************************************************************************************/
BOOL AIC3106_Codec_Set_Output_Filter(CodecHardwareFilterChoices_t which_filter)
{
    BOOL pass = FALSE;
    BOOL retVal = FALSE;
    U8 ReadData = 0;
    DigitalEffectsFilter_t *p_FilterVals = NULL;
    BOOL UseDigEffectsFilter = TRUE;

    //set up which filter we are going to need
    switch (which_filter)
    {
        case CODEC_FILTER_NONE:
            //
            p_FilterVals = &NoFilter;
        break;

        case CODEC_FILTER_TIA4953_FLAT:
            //
            p_FilterVals = &FlatFilter;
        break;

        case CODEC_FILTER_TIA4953_SLIGHT:
            //
            p_FilterVals = &SlightFilter;
        break;

        case CODEC_FILTER_TIA4953_STEEP_MILD:
            //
            p_FilterVals = &SteepMildFilter;
        break;

        case CODEC_FILTER_TIA4953_STEEP_MODERTE:
            //
            p_FilterVals = &SteepModerateFilter;
        break;

        case CODEC_FILTER_TIA4953_STEEP_SEVERE:
            //
            p_FilterVals = &SteepSevereFilter;
        break;

        case CODEC_FILTER_TIA470:
            //
            p_FilterVals = &TIA470Filter;
        break;

        default:
            //
            p_FilterVals = &NoFilter;
        break;

    }

    //read reg 12 to get base values. no need to blindly overwrite existing settings
    Codec_i2c_read(CODECAIC3106_I2C_ADDR, 12, &ReadData);

    //disable effects filter
    if (p_FilterVals->UserOutput == TRUE)
    {
        if ((p_FilterVals->DEF_N0 != 0) && (p_FilterVals->DEF_N2 == 0))
        {
            UseDigEffectsFilter = FALSE;
        }
        else
        {
            UseDigEffectsFilter = TRUE;
        }

        ReadData &= (U8) (~(CODEC_PAGE0_REG12_LEFT_DE_EMPHASIS_ON | CODEC_PAGE0_REG12_LEFT_DIG_EFF_FILTER_ON));
        retVal = WriteToCodecReg(12, ReadData, TRUE, CODEC_REG_READ_DEFAULT_MASK);
    }
    else
    {
        if ((p_FilterVals->DEF_N0 != 0) && (p_FilterVals->DEF_N2 == 0))
        {
            UseDigEffectsFilter = FALSE;
        }
        else
        {
            UseDigEffectsFilter = TRUE;
        }

        ReadData &= (U8) (~(CODEC_PAGE0_REG12_RIGHT_DE_EMPHASIS_ON | CODEC_PAGE0_REG12_RIGHT_DIG_EFF_FILTER_ON));
        retVal = WriteToCodecReg(12, ReadData, TRUE, CODEC_REG_READ_DEFAULT_MASK);
    }

    if (retVal == TRUE)
    {
        if (p_FilterVals->enable == TRUE)
        {
            //set to page 1
            if (WriteToCodecReg(0, CODEC_PAGE0_REG0_PAGE1, TRUE, CODEC_REG_READ_DEFAULT_MASK))
            {
                if (p_FilterVals->UserOutput == TRUE)
                {
                    if (UseDigEffectsFilter)
                    {
                        /* set filter values. 8 bit registers so 2 writes per coefficient. you have to write the MSB
                         * bits of the coef first, then the LSB. also, you cannot read the MSB bits until
                         * after you write the LSBs.
                         *
                         */
                        //First Filter:
                        //N0
                        WriteToCodecReg(1, (U8) (p_FilterVals->DEF_N0 >> 8), FALSE, CODEC_REG_READ_DEFAULT_MASK); //MSB
                        WriteToCodecReg(2, (U8) (p_FilterVals->DEF_N0), TRUE, CODEC_REG_READ_DEFAULT_MASK); //LSB
                        Codec_i2c_read(CODECAIC3106_I2C_ADDR, 1, &ReadData);
                        if (ReadData != (U8) (p_FilterVals->DEF_N0 >> 8))
                        {
                            asm(" NOP");
                        }

                        //N1
                        WriteToCodecReg(3, (U8) (p_FilterVals->DEF_N1 >> 8), FALSE, CODEC_REG_READ_DEFAULT_MASK); //MSB
                        WriteToCodecReg(4, (U8) (p_FilterVals->DEF_N1), TRUE, CODEC_REG_READ_DEFAULT_MASK); //LSB
                        Codec_i2c_read(CODECAIC3106_I2C_ADDR, 3, &ReadData);
                        if (ReadData != (U8) (p_FilterVals->DEF_N1 >> 8))
                        {
                            asm(" NOP");
                        }

                        //N2
                        WriteToCodecReg(5, (U8) (p_FilterVals->DEF_N2 >> 8), FALSE, CODEC_REG_READ_DEFAULT_MASK); //MSB
                        WriteToCodecReg(6, (U8) (p_FilterVals->DEF_N2), TRUE, CODEC_REG_READ_DEFAULT_MASK); //LSB
                        Codec_i2c_read(CODECAIC3106_I2C_ADDR, 5, &ReadData);
                        if (ReadData != (U8) (p_FilterVals->DEF_N2 >> 8))
                        {
                            asm(" NOP");
                        }

                        //D1
                        WriteToCodecReg(13, (U8) (p_FilterVals->DEF_D1 >> 8), FALSE, CODEC_REG_READ_DEFAULT_MASK); //MSB
                        WriteToCodecReg(14, (U8) (p_FilterVals->DEF_D1), TRUE, CODEC_REG_READ_DEFAULT_MASK); //LSB
                        Codec_i2c_read(CODECAIC3106_I2C_ADDR, 13, &ReadData);
                        if (ReadData != (U8) (p_FilterVals->DEF_D1 >> 8))
                        {
                            asm(" NOP");
                        }

                        //D2
                        WriteToCodecReg(15, (U8) (p_FilterVals->DEF_D2 >> 8), FALSE, CODEC_REG_READ_DEFAULT_MASK); //MSB
                        WriteToCodecReg(16, (U8) (p_FilterVals->DEF_D2), TRUE, CODEC_REG_READ_DEFAULT_MASK); //LSB
                        Codec_i2c_read(CODECAIC3106_I2C_ADDR, 15, &ReadData);
                        if (ReadData != (U8) (p_FilterVals->DEF_D2 >> 8))
                        {
                            asm(" NOP");
                        }

                        //Second Filer
                        //N3
                        WriteToCodecReg(7, (U8) (p_FilterVals->DEF_N3 >> 8), FALSE, CODEC_REG_READ_DEFAULT_MASK); //MSB
                        WriteToCodecReg(8, (U8) (p_FilterVals->DEF_N3), TRUE, CODEC_REG_READ_DEFAULT_MASK); //LSB
                        Codec_i2c_read(CODECAIC3106_I2C_ADDR, 7, &ReadData);
                        if (ReadData != (U8) (p_FilterVals->DEF_N3 >> 8))
                        {
                            asm(" NOP");
                        }

                        //N4
                        WriteToCodecReg(9, (U8) (p_FilterVals->DEF_N4 >> 8), FALSE, CODEC_REG_READ_DEFAULT_MASK); //MSB
                        WriteToCodecReg(10, (U8) (p_FilterVals->DEF_N4), TRUE, CODEC_REG_READ_DEFAULT_MASK); //LSB
                        Codec_i2c_read(CODECAIC3106_I2C_ADDR, 9, &ReadData);
                        if (ReadData != (U8) (p_FilterVals->DEF_N4 >> 8))
                        {
                            asm(" NOP");
                        }

                        //N5
                        WriteToCodecReg(11, (U8) (p_FilterVals->DEF_N5 >> 8), FALSE, CODEC_REG_READ_DEFAULT_MASK); //MSB
                        WriteToCodecReg(12, (U8) (p_FilterVals->DEF_N5), TRUE, CODEC_REG_READ_DEFAULT_MASK); //LSB
                        Codec_i2c_read(CODECAIC3106_I2C_ADDR, 11, &ReadData);
                        if (ReadData != (U8) (p_FilterVals->DEF_N5 >> 8))
                        {
                            asm(" NOP");
                        }

                        //D4
                        WriteToCodecReg(17, (U8) (p_FilterVals->DEF_D4 >> 8), FALSE, CODEC_REG_READ_DEFAULT_MASK); //MSB
                        WriteToCodecReg(18, (U8) (p_FilterVals->DEF_D4), TRUE, CODEC_REG_READ_DEFAULT_MASK); //LSB
                        Codec_i2c_read(CODECAIC3106_I2C_ADDR, 17, &ReadData);
                        if (ReadData != (U8) (p_FilterVals->DEF_D4 >> 8))
                        {
                            asm(" NOP");
                        }

                        //D5
                        WriteToCodecReg(19, (U8) (p_FilterVals->DEF_D5 >> 8), FALSE, CODEC_REG_READ_DEFAULT_MASK); //MSB
                        WriteToCodecReg(20, (U8) (p_FilterVals->DEF_D5), TRUE, CODEC_REG_READ_DEFAULT_MASK); //LSB
                        Codec_i2c_read(CODECAIC3106_I2C_ADDR, 19, &ReadData);
                        if (ReadData != (U8) (p_FilterVals->DEF_D5 >> 8))
                        {
                            asm(" NOP");
                        }

                        //set back to page 0
                        if (WriteToCodecReg(0, 0, TRUE, CODEC_REG_READ_DEFAULT_MASK))
                        {
                            //enable effects filter
                            Codec_i2c_read(CODECAIC3106_I2C_ADDR, 12, &ReadData);
                            ReadData |= CODEC_PAGE0_REG12_LEFT_DIG_EFF_FILTER_ON;
                            pass = WriteToCodecReg(12, ReadData, TRUE, CODEC_REG_READ_DEFAULT_MASK);
                        }
                        else
                        {
                            asm(" NOP");
                            //failure to set back to page 0
                        }
                    }
                    else
                    {
                        //de-emphasis filter - much simpler
                        //First Filter:
                        //N0
                        WriteToCodecReg(21, (U8) (p_FilterVals->DEF_N0 >> 8), FALSE, CODEC_REG_READ_DEFAULT_MASK); //MSB
                        WriteToCodecReg(22, (U8) (p_FilterVals->DEF_N0), TRUE, CODEC_REG_READ_DEFAULT_MASK); //LSB
                        Codec_i2c_read(CODECAIC3106_I2C_ADDR, 1, &ReadData);
                        if (ReadData != (U8) (p_FilterVals->DEF_N0 >> 8))
                        {
                            asm(" NOP");
                        }

                        //N1
                        WriteToCodecReg(23, (U8) (p_FilterVals->DEF_N1 >> 8), FALSE, CODEC_REG_READ_DEFAULT_MASK); //MSB
                        WriteToCodecReg(24, (U8) (p_FilterVals->DEF_N1), TRUE, CODEC_REG_READ_DEFAULT_MASK); //LSB
                        Codec_i2c_read(CODECAIC3106_I2C_ADDR, 3, &ReadData);
                        if (ReadData != (U8) (p_FilterVals->DEF_N1 >> 8))
                        {
                            asm(" NOP");
                        }
                        //D1
                        WriteToCodecReg(25, (U8) (p_FilterVals->DEF_D1 >> 8), FALSE, CODEC_REG_READ_DEFAULT_MASK); //MSB
                        WriteToCodecReg(26, (U8) (p_FilterVals->DEF_D1), TRUE, CODEC_REG_READ_DEFAULT_MASK); //LSB
                        Codec_i2c_read(CODECAIC3106_I2C_ADDR, 13, &ReadData);
                        if (ReadData != (U8) (p_FilterVals->DEF_D1 >> 8))
                        {
                            asm(" NOP");
                        }

                        //set back to page 0
                        if (WriteToCodecReg(0, 0, TRUE, CODEC_REG_READ_DEFAULT_MASK))
                        {
                            //enable effects filter
                            Codec_i2c_read(CODECAIC3106_I2C_ADDR, 12, &ReadData);
                            ReadData |= CODEC_PAGE0_REG12_LEFT_DE_EMPHASIS_ON;
                            pass = WriteToCodecReg(12, ReadData, TRUE, CODEC_REG_READ_DEFAULT_MASK);
                        }
                        else
                        {
                            asm(" NOP");
                            //failure to set back to page 0
                        }
                    }
                }
            }
            else
            {
                asm(" NOP");
                //failure to set page 1
            }
        }
        else
        {
            pass = TRUE; //disabling the filter
        }
    }
    else
    {
        asm(" NOP");
        //failure to disable audio filters
    }

    return pass;
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
BOOL AIC3106_Codec_Enable_Ext_Spkr(void)
{
    BOOL pass = FALSE;

    //mute handset
    if (WriteToCodecReg(86, CODEC_PAGE0_REG86_LEFTLO_POWERED_UP, FALSE, CODEC_REG_READ_DEFAULT_MASK))
    {
        //unmute speaker
        pass = WriteToCodecReg(93, (CODEC_PAGE0_REG93_RIGHTLO_UNMUTE | CODEC_PAGE0_REG93_RIGHTLO_POWERED_UP), FALSE, CODEC_REG_READ_DEFAULT_MASK);
    }

    return pass;
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
BOOL AIC3106_Codec_Enable_Ext_Mic(void)
{
    BOOL pass = FALSE;

    //mute handset mic by unrouting it from left ADC
    if (WriteToCodecReg(19, (CODEC_PAGE0_REG19_LINE1L_DIFF | CODEC_PAGE0_REG19_LEFT_ADC_POWERED_UP | CODEC_PAGE0_REG19_LINE1L_OFF_LEFT_ADC), TRUE, CODEC_REG_READ_DEFAULT_MASK))
    {
        pass = WriteToCodecReg(107, CODEC_PAGE0_REG107_LEFT_DMIC_USED, TRUE, 0xFC);
    }

    //printf("External Mic pass=%d\n", pass);

    return pass;
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
BOOL AIC3106_Codec_Enable_Handset_Spkr(void)
{
    BOOL pass = FALSE;

    //Mute DAC while we play?
    //WriteToCodecReg(43, CODEC_PAGE0_REG43_LEFT_DAC_MUTE, FALSE, CODEC_REG_READ_DEFAULT_MASK);

    //mute ext speaker
    if (WriteToCodecReg(93, CODEC_PAGE0_REG93_RIGHTLO_POWERED_UP, FALSE, 0xFD))
    {
        //unmute handset
        pass = WriteToCodecReg(86, (CODEC_PAGE0_REG86_7DB | CODEC_PAGE0_REG86_LEFTLO_UNMUTE | CODEC_PAGE0_REG86_LEFTLO_POWERED_UP), FALSE, 0xFD);
    }

    return pass;
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
BOOL AIC3106_Codec_Enable_Handset_Mic(void)
{
    BOOL pass = FALSE;

    //enable handset mic by routing it to left adc
    if (WriteToCodecReg(19, (CODEC_PAGE0_REG19_LINE1L_DIFF | CODEC_PAGE0_REG19_LEFT_ADC_POWERED_UP), FALSE, CODEC_REG_READ_DEFAULT_MASK))
    {
        pass = WriteToCodecReg(107, CODEC_PAGE0_REG107_ANALOG_MICS_USED, TRUE, 0xFC);
    }

    return pass;
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
BOOL AIC3106_Codec_Set_User_ADC(U32 Gain)
{
    BOOL pass = FALSE;

    pass = WriteToCodecReg(15, Gain, FALSE, CODEC_REG_READ_DEFAULT_MASK);

    return pass;
}
extern int G168_En;
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
BOOL AIC3106_Codec_Set_User_DAC(U32 Gain)
{
    U8 SidetoneVolume[] = {
                            DAC_MUTE,
                            DAC_MINUS_43_8DB,
                            DAC_MINUS_42_2DB,
                            DAC_MINUS_40_7DB,
                            DAC_MINUS_39_2DB,
                            DAC_MINUS_37_7DB,
                            DAC_MINUS_36_1DB,
                            DAC_MINUS_34_6DB,
                            DAC_MINUS_33_1DB,
                            DAC_MINUS_31_6DB,
                            DAC_MINUS_30_1DB,
                            DAC_MINUS_28_6DB,
                            DAC_MINUS_27_1DB };

    BOOL pass = FALSE;

    pass = WriteToCodecReg(43, Gain, FALSE, CODEC_REG_READ_DEFAULT_MASK);

    //route left PGA to LeftLO for sidetone - dB
    if (G168_En)
    {
    	//printf("Enabling sidetone\n");
    	WriteToCodecReg(81, (CODEC_PAGE0_REG81_PGAL_ROUTED_LEFTLO | SidetoneVolume[10]), TRUE, CODEC_REG_READ_DEFAULT_MASK);
    }
    else
    {
    	//printf("Disabling sidetone\n");
        WriteToCodecReg(81, 0, FALSE, CODEC_REG_READ_DEFAULT_MASK);
    }

    return pass;
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
BOOL AIC3106_Codec_Set_POTS_ADC(U32 Gain)
{
    BOOL pass = FALSE;

    pass = WriteToCodecReg(16, Gain, FALSE, CODEC_REG_READ_DEFAULT_MASK);

    return pass;
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
BOOL AIC3106_Codec_Set_POTS_DAC(U32 Gain)
{
    BOOL pass = FALSE;

    pass = WriteToCodecReg(44, Gain, FALSE, CODEC_REG_READ_DEFAULT_MASK);

    return pass;
}

