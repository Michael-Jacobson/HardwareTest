/********************************************************************************************************************************/
/********************************************************************************************************************************/
/*                                                     Ultratec.h                                                        */
/*                                COPYRIGHT NOTICE: (c) Ultratec, Inc.  2016  ALL RIGHTS RESERVED                               */
/********************************************************************************************************************************/
/*                                                                                                                              */
/* This file contains all the definitions, enums, structures, etc. for the general use on the Model2400iBT.                       */
/*                                                                                                                              */
/********************************************************************************************************************************/

#ifndef ULTRATEC_H
#define ULTRATEC_H
/****************/
/*   Includes   */
/****************/
//#include <xdc/std.h>
//#include <xdc/cfg/global.h>
//#include <ti/sysbios/BIOS.h>

/****************/
/* File defines */
/****************/
#define DEFAULT_CAPTIONS_PORT		  5007
#define DEFAULT_OTW_PORT			  5012
#define DEFAULT_HEARTBEAT_PORT	      5007
#define DEFAULT_DATE_TIME_PORT	      5507
#define DEFAULT_CONTACT_SERVER_PORT   5500

//ballsack this really belongs somewhere else
#define LCD_PIXEL_WIDTH    			1024
#define LCD_PIXEL_HEIGHT   			600


#define TONE_BUF_LEN				20			/* 2.5mS buffer */

#define PHONEBOOK_NAME_LEN			29
#define PHONE_NUMBER_LEN			28			/* This len does NOT include the NULL. */

#define BYTES_IN_WORD				2
#define BITS_PER_WORD				16
#define BITS_PER_BYTE				8
#define PENDING_TEXT_BUF_SIZE		1024	//this needs to be a power of 2
#define UVT_MAX_SPEECH_FRAMES		4
#define UVT_MAX_TX_PACKET_SIZE		32
#define DEBUG_BUF_LENGTH			2048
#define AEC_MIC_DELAY				80
#define TWO_SEC_BILLING_DELAY		16000		/* 125uS timer (2 / 125uS = 16000) */
#define FLASH_TIMEOUT_HWI        	6400   /* 800ms with 8000 times/sec =>   */

#define DTMF_TX_ON_TIME_MS    		80
#define DTMF_TX_OFF_TIME_MS    		140


#define CLIPPING_TIMEOUT 	100 /* ms timeout for turning the MIC signal meter red to show clipping */

/*  We need to redefine the default length of the data pump's transmit sample 
    buffer to allow a longer far-end echo canceller.  2047 samples gives over 
    250ms of echo canceller modelling length for our 8kHz sample rate.  This 
    value must be one less than the allocated length of TxSampBig in the 
    linker command file since this is a circular buffer. */
#define TX_SAMPLE_BIG_LEN	2047

/*  The default transmit data buffer (Tx_data[]) is too short for our longer
    packets, so we are making a bigger one.  This is the length of our new
    tx data buffer.  This value must be one less than the allocated length
    of TxDataUti in the linker command file since this is a circular buffer. */
#define TX_DATA_LEN_UTI		63

#define HW_SYSTEM_DELAY		22

#define MINUS_1DB		29204;		/* -1 dB */
#define MINUS_3DB		23198;		/* -3 dB */
#define MINUS_6DB		16384;		/* -6 dB */
#define MINUS_7DB		14636;		/* -7 dB */
#define MINUS_10DB		10362;		/* -10 dB */
#define MINUS_11DB	 	9236;		/* -11 dB */
#define MINUS_12DB	 	8231;		/* -12 dB */
#define MINUS_15DB	 	5827;		/* -15 dB */

/*	This 8bit value represents the digits for the generator polynomial:
	x^^8 + x^^4 + x^^3 + 1, I think.  In any case, this is the generator
	constant we are using.  See below for explanation of a generator
	constant. */
#define	CRC_8BIT_GENERATOR	0x8C

/* KEYBOARD CONSTANTS (Based on ISO 8859-1 (Latin-1) Character table) */
#define ASCII_STX				0x02
#define ASCII_ETX				0x03
#define ASCII_BS				0x08
#define ASCII_LF				0x0A
#define ASCII_CR				0x0D
#define ASCII_ESC				0x1B
#define ASCII_RS				0x1E
#define ASCII_SPACE				0x20


/* Conversation reserved values */
#define SCROLL_STEP_SIZE_SLOW				5			// The number of pixel rows moved per smooth scroll step.
#define SCROLL_STEP_SIZE_FAST               10
#define MAX_VISIBLE_LINES_OF_CONV			8			// A row value of 8 accommodates the MAX number of visible rows on the screen in the smallest font.
#define MAX_CONV_CHARS						360000	// SD card reserved conversation memory size.
#define MAX_CONV_CHARS_PER_LINE				100		// 30 pt. Myriad Pro letter "l".
#define MIN_CONV_CHARS_PER_LINE				16			// 48 pt. Myriad Pro letter "W".
#define MAX_CONV_LINES						(MAX_CONV_CHARS / MIN_CONV_CHARS_PER_LINE)	// = 22500
#define MAX_DISPLAY_ROWS_CHARS_PER_LINE		130		// A col length of 100 would accommodate the MAX number of 99 "l" characters (the most narrow letter) before line wrap in the smallest font.

/* CONSTANTS */
#define COMMA					0x002C
#define NINETY_MS				90
#define EIGHT_HUNDRED_MS		800
#define ONE_SECOND				1000
#define TWO_SECONDS				(ONE_SECOND*2)
#define THREE_SECONDS			(ONE_SECOND*3)
#define FOUR_SECONDS			(ONE_SECOND*4)
#define FIVE_SECONDS			(ONE_SECOND*5)
#define SIX_SECONDS				(ONE_SECOND*6)
#define SEVEN_SECONDS			(ONE_SECOND*7)
#define EIGHT_SECONDS			(ONE_SECOND*8)
#define NINE_SECONDS			(ONE_SECOND*9)
#define TEN_SECONDS				(ONE_SECOND*10)
#define THIRTEEN_SECONDS		(ONE_SECOND*13)
#define FOURTEEN_SECONDS      	(ONE_SECOND*14)
#define FIFTEEN_SECONDS			(ONE_SECOND*15)
#define TWENTY_SECONDS			(ONE_SECOND*20)
#define TWENTY_ONE_SECONDS    	(ONE_SECOND*21)
#define THIRTY_SECONDS			(ONE_SECOND*30)
#define ONE_MINUTE				(ONE_SECOND*60)
#define TWO_MINUTES           	(ONE_MINUTE*2)
#define THREE_MINUTES			(ONE_MINUTE*3)
#define FIVE_MINUTES          	(ONE_MINUTE*5)
#define TEN_MINUTES				(ONE_MINUTE*10)

#define ROW1						1
#define ROW2						2
#define ROW3						3
#define ROW4						4
#define ROW5						5
#define PASSED_JIG_TEST				0x69
#define NAME_CHARS					1
#define NUM_CHARS					2
#define ISPRINT_CHARS				3
#define ALNUM_CHARS					4
#define DTMF_CHARS					5
#define MAX_REDIAL_NUMBERS			10
#define MAX_REDIAL_NUM_LEN			56

#define DOWN_ARROW_ICON				0x81
#define UP_ARROW_ICON				0x82
/* DTMF Rx values */
#define RX_DTMF_ID                  0x1300
#define DTMF_ZERO					(RX_DTMF_ID | 0x0000)
#define DTMF_ONE					(RX_DTMF_ID | 0x0001)
#define DTMF_TWO					(RX_DTMF_ID | 0x0002)
#define DTMF_THREE					(RX_DTMF_ID | 0x0003)
#define DTMF_FOUR					(RX_DTMF_ID | 0x0004)
#define DTMF_FIVE					(RX_DTMF_ID | 0x0005)
#define DTMF_SIX					(RX_DTMF_ID | 0x0006)
#define DTMF_SEVEN					(RX_DTMF_ID | 0x0007)
#define DTMF_EIGHT					(RX_DTMF_ID | 0x0008)
#define DTMF_NINE					(RX_DTMF_ID | 0x0009)
	/* Ultratec Captions Setup Tone DTMF C */
#define CST_TONE					(RX_DTMF_ID | 0x000C)

#define SIZE_OF_TIME_ARRAY			7
#define HOUR_POS					0
#define MINUTE_POS					1
#define MONTH_POS					2
#define DAY_POS						3
#define TOP_YEAR_POS				4
#define LOWER_YEAR_POS				5
#define DAY_OF_THE_WEEK_POS 		6

#define TIME_DEFAULT_YEAR_UPPER		20
#define TIME_DEFAULT_YEAR_LOWER		18
#define TIME_DEFAULT_MONTH			1
#define TIME_DEFAULT_DAY			1
#define TIME_DEFAULT_HOUR			12
#define TIME_DEFAULT_MINUTE			0

#define CAPTIONS_RECYCLE_TIMEOUT 		(THREE_SECONDS)
#define HEARTBEAT_CONNECTION_TIMEOUT	(FIVE_SECONDS)
#define HEARTBEAT_DEFAULT_INTERVAL		(12*60)  //seconds timer, 12 hours 12*60 seconds*60 minutes

#define CAPTIONS_RECYCLE_TIMEOUT 	(THREE_SECONDS)
#define NUM_VOL_LEVELS 				12

#define STR_DEFAULT_ESN             (char *)"NO ESN"
#define STR_INVALID_NVRAM_ESN       (char *)"ESNFAILURE"
#define STR_CUSTOMER_SRVC_NUM       (char *)"18882697477"
#define STR_CUSTOMER_SRVC_NUM_DASH  (char *)"1-888-269-7477"
#define STR_SPAN_CUSTOMER_SRVC_NUM  (char *)"18666709134"
#define STR_CAPTEL_URL              (char *)"www.hybridcaptel.com"
#define STR_HEARTBEAT_URL           (char *)"www.hybridcaptel.com"
#define STR_CONTACT_SERVER_URL      (char *)"syncgw.hybridcaptel.com"
#define STR_HYBRID_OTW_URL          (char *)"www.hybridcaptel-otw.com"
#define STR_ENGLISH_LANG            (char *)"EN"
#define STR_SPANISH_LANG            (char *)"ES"
#define STR_FINAL_TEST_IPADDR       (char *)"192.168.1.2"
#define STR_EMERGENCY_NUMBER        (char *)"911"
#define STR_DEMO_MODE_ESN			(char *)"DEMOPHONE"

/****************/
/* File Enums   */
/****************/

/****************/
/* File structs */
/****************/
typedef struct
{
	CCHAR text[PENDING_TEXT_BUF_SIZE];
	U16 head;
	U16 tail;
} DataPipe_t;

/****************/
/* File Externs */
/****************/

/*******************************
 * Public function prototypes
 *******************************/
void InitModel2400iHardware(void);
#endif	/* __ULTRATEC_H */

