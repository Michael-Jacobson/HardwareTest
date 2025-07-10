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

/****************/
/* File defines */
/****************/
#define DEFAULT_CAPTIONS_PORT		  5007u
#define DEFAULT_OTW_PORT			  5012u
#define DEFAULT_HEARTBEAT_PORT	      5007u
#define DEFAULT_CONTACT_SERVER_PORT   5500u
#define DEFAULT_VOIP_PORT             5071u
#define DEFAULT_VOIP_PORT_TLS         5163u

#define PHONEBOOK_NAME_LEN			29u
#define PHONE_NUMBER_LEN			28u			/* This len does NOT include the NULL. */

#define PENDING_PIPE_BUF_1K_SIZE    1024u
#define PENDING_PIPE_BUF_2K_SIZE    (PENDING_PIPE_BUF_1K_SIZE*2u)
#define PENDING_PIPE_BUF_4K_SIZE    (PENDING_PIPE_BUF_1K_SIZE*4u)
#define PENDING_PIPE_BUF_8K_SIZE	(PENDING_PIPE_BUF_1K_SIZE*8u)
#define PENDING_PIPE_BUF_80K_SIZE    (PENDING_PIPE_BUF_1K_SIZE*80u)


#define UVT_MAX_SPEECH_FRAMES		4u
#define TWO_SEC_BILLING_DELAY		16000u		/* 125uS timer (2 / 125uS = 16000) */

#define DTMF_TX_ON_TIME_MS    		80u
#define DTMF_TX_OFF_TIME_MS    		140u

/* KEYBOARD CONSTANTS (Based on ISO 8859-1 (Latin-1) Character table) */
#define ASCII_STX				0x02u
#define ASCII_ETX				0x03u
#define ASCII_BS				0x08u
#define ASCII_HTAB              0x09u
#define ASCII_LF				0x0Au
#define ASCII_CR				0x0Du
#define ASCII_ESC				0x1Bu
#define ASCII_RS				0x1Eu
#define ASCII_SPACE				0x20u
#define ASCII_PLUS              0x2Bu
#define ASCII_COMMA             0x2Cu
#define ASCII_COLON             0x3Au

/* Conversation reserved values */
#define MAX_VISIBLE_LINES_OF_CONV			8u			// A row value of 8 accommodates the MAX number of visible rows on the screen in the smallest font.
#define MAX_CONV_CHARS						360000u	// SD card reserved conversation memory size.
#define MIN_CONV_CHARS_PER_LINE				16u			// 48 pt. Myriad Pro letter "W".
#define MAX_CONV_LINES						(MAX_CONV_CHARS / MIN_CONV_CHARS_PER_LINE)	// = 22500

/* CONSTANTS */
#define COMMA					0x002Cu

#define EIGHT_HUNDRED_MS		800u
#define ONE_SECOND				1000u
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
#define FORTY_FIVE_SECONDS      (ONE_SECOND*45)
#define ONE_MINUTE				(ONE_SECOND*60)
#define TWO_MINUTES           	(ONE_MINUTE*2)
#define THREE_MINUTES			(ONE_MINUTE*3)
#define FIVE_MINUTES          	(ONE_MINUTE*5)
#define TEN_MINUTES				(ONE_MINUTE*10)
#define FIFTEEN_MINUTES         (ONE_MINUTE*15)
#define ONE_HOUR                (ONE_MINUTE*60)

#define MAX_REDIAL_NUM_LEN			56u

#define SIZE_OF_TIME_ARRAY			7u
#define HOUR_POS					0u
#define MINUTE_POS					1u
#define MONTH_POS					2u
#define DAY_POS						3u
#define TOP_YEAR_POS				4u
#define LOWER_YEAR_POS				5u
#define DAY_OF_THE_WEEK_POS 		6u

#define TIME_DEFAULT_YEAR_UPPER		20u
#define TIME_DEFAULT_YEAR_LOWER		24u
#define TIME_DEFAULT_MONTH			1u
#define TIME_DEFAULT_DAY			1u
#define TIME_DEFAULT_HOUR			12u
#define TIME_DEFAULT_MINUTE			0u

#define CAPTIONS_RECYCLE_TIMEOUT 	(FOUR_SECONDS)
#define NUM_VOL_LEVELS 				12

#define HEARTBEAT_CONNECTION_TIMEOUT    (FORTY_FIVE_SECONDS)

#define STR_DEFAULT_ESN             (char *)"NO ESN"
#define STR_INVALID_NVRAM_ESN       (char *)"ESNFAILURE"
#define STR_CUSTOMER_SRVC_NUM       (char *)"18882697477"
#define STR_CUSTOMER_SRVC_NUM_DASH  (char *)"1-888-269-7477"
#define STR_SPAN_CUSTOMER_SRVC_NUM  (char *)"18666709134"
#define STR_CAPTEL_URL              (char *)"www.hybridcaptel.com"
#define STR_HEARTBEAT_URL           (char *)"cpeutil.captelservices.com"
#define STR_CONTACT_SERVER_URL      (char *)"syncgw.hybridcaptel.com"
#define STR_HYBRID_OTW_URL          (char *)"www.hybridcaptel-otw.com"
#define STR_VOIP_URL                (char *)"prod.voipcaptel.com"
#define STR_ENGLISH_LANG            (char *)"EN"
#define STR_SPANISH_LANG            (char *)"ES"
#define STR_EMERGENCY_NUMBER        (char *)"911"
#define STR_DEMO_MODE_ESN			(char *)"DEMOPHONE"
#define RECAPTION_PHONE_NUMBER      (char *)"5900"

/****************/
/* File Enums   */
/****************/
// Vendor Types for display screens
typedef enum {
    Uti_Version = 0,
    Hamilton_Version = 2
} CapTelVendor_t;

/****************/
/* File structs */
/****************/

/****************/
/* File Externs */
/****************/

/*******************************
 * Public function prototypes
 *******************************/

#endif	/* __ULTRATEC_H */

