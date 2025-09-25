/********************************************************************************************************************************/
/********************************************************************************************************************************/
/*                                                     Ultratec_Enums.h                                                        */
/*                                COPYRIGHT NOTICE: (c) Ultratec, Inc.  2016  ALL RIGHTS RESERVED                               */
/********************************************************************************************************************************/
/*                                                                                                                              */
/* This file contains all the public enums for the Model2400iBT.                       */
/*                                                                                                                              */
/********************************************************************************************************************************/
#ifndef ULTRATEC_ENUMS_H_
#define ULTRATEC_ENUMS_H_
/****************/
/*   Includes   */
/****************/

/****************/
/* File defines */
/****************/

/****************/
/* File Enums   */
/****************/
typedef enum
{
    BUT_NULL = 0,
    /* Simulated key values */
    AM_FAR_END_DC_W_CAPTIONS  =  0x7E,
    AM_CONTROL_NEXT           =  0x7F,
    AM_CONTROL_DELETE         =  0x80,
    AM_CONTROL_REPLAY         =  0x81,
    AM_PIN_ACCEPTED           =  0x82,
    INITIATE_DIAL_HOME        =  0x83,
    WIFI_TIMEOUT              =  0x84,
    LEAVE_WIFI_SETUP          =  0x85,
    FAR_END_DISCONNECT        =  0x86,
    CALL_LOST_HANG_UP         =  0x87,
    HANDSET_OFFHOOK           =  0x07, //0x88,
    HANDSET_ONHOOK            =  0x89,
    RING_EVENT_ON             =  0x08, //0x8A,
    RING_EVENT_OFF            =  0x8B,
    RINGING_HAS_ENDED         =  0x8C,
    AM_IGNORE_MESSAGE         =  0x8D,
    ANSWERING_MACHINE_ANSWER  =  0x8E,
    AM_MESSAGE_DONE           =  0x8F,
    //#define                    0x8F
	LINE_IN_USE_HIGH		  = 0x09, //0x90,
	LINE_IN_USE_LOW     	  = 0x91,
	LOOP_DETECT_HIGH 		  = 0x0A, //0x92,
	LOOP_DETECT_LOW			  = 0x93,
    /* Special keys */
    BUT_FLASH        =     0x94,
    BUT_REDIAL       =     0x95,
    BUT_SPKRPHN      =     0x96,
    BUT_MUTE         =     0x97,
    BUT_CAPTION      =     0x98,
    BUT_VOLDN        =     0x99,
    BUT_VOLUP        =     0x9A,
    BUT_CSTMR_SRVC   =     0x9E,
    BUT_HOME         =     0x9F,
      /* Special "DTMF" keys */
    BUT_DTMF0        =     0xB0,
    BUT_DTMF1        =     0xB1,
    BUT_DTMF2        =     0xB2,
    BUT_DTMF3        =     0xB3,
    BUT_DTMF4        =     0xB4,
    BUT_DTMF5        =     0xB5,
    BUT_DTMF6        =     0xB6,
    BUT_DTMF7        =     0xB7,
    BUT_DTMF8        =     0xB8,
    BUT_DTMF9        =     0xB9,
    BUT_DTMFST       =     0xBA,
    BUT_DTMFLB       =     0xBB
} KeyboardKeys_t;

/****************/
/* File structs */
/****************/

/****************/
/* File Externs */
/****************/

/*******************************
 * Public function prototypes
 *******************************/
#endif /* ULTRATEC_ENUMS_H_ */
