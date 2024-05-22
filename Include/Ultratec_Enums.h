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

typedef enum
{
	INVALID_TONE = 0,
	LOW_PASS = 1,
	ALL_PASS = 2,
	HIGH_PASS = 3,
	CUSTOM = 4
} TONE_CONTROL_SETTINGS_t;

typedef enum
{
   CUSTOM_TONE_90DB = 0,
   CUSTOM_TONE_80DB = 1,
   CUSTOM_TONE_70DB = 2,
   CUSTOM_TONE_60DB = 3,
   CUSTOM_TONE_50DB = 4,
   CUSTOM_TONE_40DB = 5,
   CUSTOM_TONE_30DB = 6
} CustomTonePointValues_t;

typedef enum {
	RING_VOL_OFF = 0,
	RING_VOL_LVL1 = 1,
	RING_VOL_LVL2 = 2,
	RING_VOL_LVL3 = 3,
	RING_VOL_LVL4 = 4,
	RING_VOL_LVL5 = 5,
	RING_VOL_LVL6 = 6,
	RING_VOL_LVL7 = 7,
	RING_VOL_LVL8 = 8,
	RING_VOL_LVL9 = 9,
	RING_VOL_LVL10 = 10
} RingerVol_t;


typedef enum {
	PITCH_MIN = 0,
	PITCH_MED = 1,
	PITCH_MAX = 2
} PitchLevel_t;


typedef enum
{
    NULL_SOCKET = 0,
	CAPTION_SOCKET,
	HEARTBEAT_SOCKET,
	OTW_SOCKET,
	CONTACTS_SOCKET,
	CCA_CONTROL_SOCKET,
	CCA_AUDIO_SOCKET
} SocketChoice_t;

typedef enum
{
   SOCKET_WAIT_FOR_ETHERNET_READY = 0,
   SOCKET_OPEN_SESSION,
   SOCKET_WAITING_FOR_1ST_DNS,
   SOCKET_REFRESH_DNS,
   SOCKET_READY,
   SOCKET_CREATE,
   SOCKET_CONNECT_INITIAL,
   SOCKET_CONNECT_REDIRECT,
   SOCKET_LISTEN,
   SOCKET_ACCEPT,
   SOCKET_CONNECTED,
   SOCKET_CLOSE,
   SOCKET_CLOSE_SESSION,
   SOCKET_SETUP,                    /* Need to delete this */
   SOCKET_WAITING_FOR_DHCP,                    /* Need to delete this */
   SOCKET_NULL
}SocketMachine_t;

typedef enum
{
	CAPTION_SOCKET_OK = 0,
	CAPTION_DNS_FAILURE,
	CAPTION_CONNECT_FAILURE,
	CAPTION_RECEIVE_FAILURE,
	CAPTION_TRANSMIT_FAILURE,
	CAPTION_FAREND_DROPPED,
	CAPTION_RECEIVE_FAREND_DROPPED,
	CAPTIONS_GAINSPAN_FAILURE
} CapSocketFailCodes_t;

typedef enum
{
	CONTACT_SOCKET_OK = 0,
	CONTACT_DNS_FAILURE,
	CONTACT_CONNECT_FAILURE,
	CONTACT_RECEIVE_FAILURE,
	CONTACT_TRANSMIT_FAILURE,
	CONTACT_FAREND_DROPPED,
	CONTACT_RECEIVE_FAREND_DROPPED,
	CONTACT_GAINSPAN_FAILURE,
	CONTACT_CONNECTION_TIMEOUT,
	CONTACT_SDCARD_READ_FAIL,
	CONTACT_SDCARD_WRITE_FAIL
} ContactSocketFailCodes_t;

typedef enum
{
	OTW_SOCKET_OK = 0,
	OTW_DNS_FAILURE,
	OTW_CONNECT_FAILURE,
	OTW_RECEIVE_FAILURE,
	OTW_TRANSMIT_FAILURE,
	OTW_FAREND_DROPPED,
	OTW_RECEIVE_FAREND_DROPPED,
	OTW_GAINSPAN_FAILURE,
	OTW_TIMER_TIMEOUT
} OTWSocketFailCodes_t;

typedef enum
{
	NO_CONNECTION = 0,
	WIFI_CONNECTION,
	ETHERNET_CONNECTION
} InternetConnectionTypes_t;

typedef enum
{
   SETUP_BOOTING = 0,
	SETUP_SELECT_LANGUAGE,
	SETUP_CHECK_PHONELINE,
	SETUP_DIAL_PREFIX,
	SETUP_SELECT_INTERNET_CONN,
	SETUP_SHOW_WIFI_NETWORKS,
	SETUP_FIRST_OTW,
	SETUP_OTW_INSTALL,
	SETUP_CLOCK_TIMEZONE,
	SETUP_CLOCK,
	SETUP_DUMB_TERMINAL,
	SETUP_COMPLETE,
	SETUP_FAIL,
	SETUP_IDLE
}	FirstTimeSetUpStates_t;

typedef enum
{
	NOT_READY_FOR_CONNECT = 0,
	READY_FOR_CONNECT,
	CONNECTION_PENDING,
	CONNECTED,
	DISCONNECTING
} IPConnectionStates_t;

typedef enum
{
	CAPTEL_IDLE = 0,
	CAPTEL_RINGING = 1,
	CAPTEL_LINE_IN_USE = 2,
	CAPTEL_OFFHOOK = 3,
	CAPTEL_DIAL = 4,
	CAPTEL_IN_MENU = 5,
	CAPTEL_INFO_SCREEN = 6,
	CAPTEL_SETTING_TIME = 7,
	CAPTEL_SETUP_TONE = 8,
	CAPTEL_IN_TEST_FIXTURE = 10,
	CAPTEL_IN_FINAL_TEST = 11,
	CAPTEL_DTMF_PROGRAMMING = 12,
	CAPTEL_ONHOOK_DIALING = 13,
	CAPTEL_ENG_PROGRAMMING = 14,
	CAPTEL_SETUP_STATIC_IP = 15,
	CAPTEL_FIRST_TIME_SETUP = 16,
	CAPTEL_IN_PEND_DIAL = 17,
	CAPTEL_SHOW_CONTACTS = 18,
	CAPTEL_ADD_EDIT_CONTACTS = 19,
	CAPTEL_SHOW_FAVORITES = 20,
	CAPTEL_IN_REDIAL = 21,
	CAPTEL_ANSWERING_MACHINE = 22,
	CAPTEL_SHOW_NETWORK_ERROR = 23,
	CAPTEL_SHOW_CALL_HISTORY = 24,
	CAPTEL_SHOW_AM_MESSAGES = 25,
	CAPTEL_CH_REVIEW_CONVERSATION = 26,
	CAPTEL_AM_REVIEW_MESSAGE = 27,
	CAPTEL_AM_PLAYING_MESSAGE = 28,
	CAPTEL_RECORDING_AM_GREETING = 29,
	CAPTEL_REGISTRATION_DUMB_TERMINAL = 30,
	CAPTEL_OTW = 31,
	CAPTEL_SHOW_WIFI_NETWORKS = 32,
	CAPTEL_ENTER_WIFI_SECURITY = 33,
	CAPTEL_DIAL_HOME = 34,
	CAPTEL_SYNCING_CONTACTS = 35,
	CAPTEL_SCREEN_CALIBRATION = 36,
	CAPTEL_SWITCHING_FROM_WIFI_TO_ETH = 37,
	CAPTEL_SWITCHING_FROM_NONE_TO_ETH = 38,
#ifdef USE_BT_BUILD	
	CAPTEL_CAPTION_EXTERNAL_ANS_MACH = 39,
	CAPTEL_SHOW_BLUETOOTH_NETWORKS = 40,
	CAPTEL_INSTALL_BT_FIRMWARE = 41,
    CAPTEL_SYNCING_SMARTPHONE_CONTACTS = 42
#else
	CAPTEL_CAPTION_EXTERNAL_ANS_MACH = 39
#endif	
} PhoneStates_t;

//This enum needs explicit values because it is used to feed the progress bar that
//requires a range from 0-8
typedef enum
{
	CON_NO_CONNECTION = 0,
	CON_1ST_CONNECTION = 1,
	CON_1ST_PID_RECEIVED = 2,
	CON_1ST_IB_RECEIVED = 3,
	CON_REDIRECT_RECEIVED = 4,
	CON_2ND_CONNECTION = 5,
	CON_2ND_PID_RECEIVED = 6,
	CON_2ND_IB_RECEIVED = 7,
	CON_RECEIVED_CAPTIONS = 8
}	ConnectionStates_t;


typedef enum
{
	UVT_CHN_INVALID 				= 0,
	UVT_CHN_COMMAND 				= 1,	/* RX only */
	UVT_CHN_COMMAND_RESPONSE 		= 2,	/* TX only */
	UVT_CHN_CAPTIONED_TEXT 			= 3,	/* RX only */
	UVT_CHN_COMPRESSED_AUDIO 		= 4,	/* RX and TX */
	UVT_CHN_INFORMATION_BLOCK 		= 5,	/* This channel only used by the Rackmount modems in the platform. */
	UVT_CHN_SPEEDDIAL 				= 6,	/* RX and TX */
	UVT_CHN_PHONEBOOK 				= 7,	/* RX and TX */
	UVT_CHN_SYMBOL_TABLE 			= 8,	/* TX only */
	UVT_CHN_CID 					= 9,	/* RX only */
	UVT_CHN_VOICEMAIL 				= 10,	/* RX only */
	UVT_CHN_ENC_CAPTIONED_TEXT 	    = 11,	/* RX only */
	UVT_CHN_ENC_COMPRESSED_AUDIO 	= 12,	/* RX and TX */
	UVT_CHN_ENC_SPEEDDIAL 			= 13,	/* RX and TX */
	UVT_CHN_ENC_PHONEBOOK 			= 14,	/* RX and TX */
	UVT_CHN_ENC_CID 				= 15,	/* RX only */
	UVT_CHN_ENC_VOICEMAIL 			= 16,	/* RX only */
	UVT_CHN_ENC_COMMAND 			= 17,	/* RX only */
	UVT_CHN_ENC_REGISTRATION_MODE   = 18, 	/* RX and TX */
	UVT_CHN_BRANDING_SCREEN         = 19,   /* RX and TX */
	UVT_CHN_ENC_G4_CAPTIONED_TEXT   = 20	/* RX only */
} UVTChannelProtocol_t;


/* This enumeration defines valid packet types for the UVT protocol.  The
	packet format field is 8 bits.  The packet types are as follows:

	basic types:
	CMD:		Command packet.
	CONFIG:	Configuration packet.
	DATA:		Speech and/or text packet.

	sub-types:
	T1:			One text character is contained within the packet.
	S1:			One speech frame is contained within the packet.
	S2:			Two speech frames are contained within the packet.
	CRC:			A CRC is included at the end of the packet.

	DON'T USE 0xDA TO SIMPLIFY DETECTING START OF PACKET (SOP).
*/
typedef enum
{
	DATA_T1S2_G729A				= 0x09,			/* (PID 1) DSPModem TX & RX, Client TX & RX */
	DATA_S2_G729A					= 0x19,			/* (PID 1) DSPModem TX & RX, Client TX & RX */
	CMD_DTMFPROXY_CRC				= 0x1C,			/* (PID 1) DSPModem RX only, Client TX only */
	CMD_REQUEST_IB_CRC			= 0x20,			/* (PID 1) DSPModem TX only, Client RX only */
	CMD_IB_RESPONSE_CRC			= 0x21,			/* (PID 1) DSPModem RX only, Client TX only */
	CMD_REQUEST_PID_CRC			= 0x22,			/* (PID 1) DSPModem TX only, Client RX only */
	CMD_PID_RESPONSE_CRC			= 0x23,			/* (PID 1) DSPModem RX only, Client TX only */
	CMD_DEBUG_MODE_ENTER			= 0x24,			/* (PID 2) DSPModem TX only, Client RX only */
	CMD_DEBUG_MODE_EXIT			= 0x25,			/* (PID 2) DSPModem TX only, Client RX only */
	CMD_DEBUG_MODE_READ			= 0x26,			/* (PID 2) DSPModem TX only, Client RX only */
	DATA_DEBUG_PACKET				= 0x27,			/* (PID 2) DSPModem TX & RX, Client TX & RX */
	DATA_DEBUG_READ_CRC			= 0x28,			/* (PID 2) DSPModem RX only, Client TX only */
	CMD_SWUPDATE_ENTER			= 0x29,			/* (PID 3) DSPModem TX only, Client RX only */
	CMD_SWUPDATE_READY			= 0x2A,			/* (PID 3) DSPModem RX only, Client TX only */
	DATA_SWUPDATE_CRC				= 0x2B,			/* (PID 3) DSPModem TX only, Client RX only */
	CMD_SWUPDATE_RESEND			= 0x2C,			/* (PID 3) DSPModem RX only, Client TX only */
	CMD_SWUPDATE_CHKSUM			= 0x2D,			/* (PID 3) DSPModem TX only, Client RX only */
	CMD_UVT_PASS					= 0x2E,			/* (PID 3) DSPModem RX only, Client TX only */
	CMD_UVT_FAIL					= 0x2F,			/* (PID 3) DSPModem RX only, Client TX only */
	CMD_REQ_NVRAM_BLK_CRC		= 0x30,			/* (PID 3) DSPModem TX only, Client RX only */
	DATA_NVRAM_BLOCK_CRC			= 0x31,			/* (PID 3) DSPModem TX & RX, Client TX & RX */
	CMD_BOOTLOAD					= 0x32,			/* (PID 3) DSPModem TX only, Client RX only */
	CMD_REQ_NVRAM_SYM_CRC		= 0x33,			/* (PID 3) DSPModem TX only, Client RX only */
	DATA_NVRAM_SYMBOL_CRC		= 0x34,			/* (PID 3) DSPModem RX & TX, Client RX & TX */
	CMD_CLIENT_HANGUP				= 0x35,			/* (PID 3) DSPModem RX & TX, Client TX & RX */
	CMD_REDIRECT_CRC				= 0x36,			/* (PID 4) DSPModem TX only, Client RX only */
	CMD_SEND_SPDDIAL_CRC			= 0x37,			/* (PID 4) DSPModem TX only, Client RX only */
	CMD_SEND_PHNBK_CRC			= 0x38,			/* (PID 4) DSPModem TX only, Client RX only */
	CMD_SEND_SMBLTBL_CRC			= 0x39,			/* (PID 4) DSPModem TX only, Client RX only */
	CMD_START_CAPTIONS_CRC		= 0x3A,			/* (PID 4) DSPModem TX only, Client RX only */
	CMD_START_2LINE_AUDIO_CRC	= 0x3B,			/* (PID 5) DSPModem TX only, Client RX only */
	CMD_STOP_2LINE_AUDIO			= 0x3C,			/* (PID 5) DSPModem TX only, Client RX only */
	CMD_START_RINGING				= 0x3D,			/* (PID 6) DSPModem TX only, Client RX only */
	CMD_CAPTION_ON 				= 0x3E,			/* (PID 6) DSPModem RX only, Client TX only */
	CMD_CAPTION_OFF				= 0x3F,			/* (PID 6) DSPModem RX only, Client TX only */
	CMD_NEW_VOICEMAIL_CHECK		= 0x40,			/* (PID 6) DSPModem RX only, Client TX only */
	CMD_NEW_VOICEMAIL_RESP		= 0x41,			/* (PID 6) DSPModem TX only, Client RX only */
	CMD_REDIRECT_REQUEST			= 0x42,			/* (PID 6) DSPModem RX only, Client TX only */
	CMD_REQUEST_ALL_VOICEMAIL	= 0x43,			/* (PID 6) DSPModem RX only, Client TX only */
	CMD_PLAY_VOICEMAIL			= 0x44,			/* (PID 6) DSPModem RX only, Client TX only */
	CMD_DELETE_VOICEMAIL			= 0x45,			/* (PID 6) DSPModem RX only, Client TX only */
	CMD_STOP_PLAYING_VOICEMAIL	= 0x46,			/* (PID 6) DSPModem RX only, Client TX only */
	CMD_INCOMING_CALL_POLL_INTERVAL = 0x47,	/* (PID 6) DSPModem TX only, Client RX only */
	CMD_VM_PLAY_COMPLETED 		= 0x48,			/* (PID 6) DSPModem TX only, Client RX only */
	DATA_COLORED_TEXT_CRC        		= 0x49,			/* (PID 7) DSPModem TX , Client RX */
	CMD_LIVE_CALL					= 0x4A,			/* (PID 7) DSPModem RX only, Client TX only */
	CMD_FAR_END_DISCONNECT		= 0x4B,			/* (PID 7) DSPModem RX only, Client TX only */
	/* fill in here */
	CMD_CORRECTIONS_ON			= 0x50,	/* (PID 6) DSPModem RX only, Client TX only */
	CMD_CORRECTIONS_OFF			= 0x51,	/* (PID 6) DSPModem RX only, Client TX only */
	CMD_GET_TIME_CRC				= 0x52,	/* (PID 8) DSPModem TX only, Client RX only */
/* RESERVED					= 0xDA, */
	UNKNOWN_UVT_PACKET			= 0xFF			/* Not defined */
} uvt_packet_type_t;

// Vendor Types for display screens
typedef enum {
	Uti_Version = 0,
	Sprint_Version = 1,
	Hamilton_Version = 2
} CapTelVendor_t;


typedef enum
{
	TIMEZONE_EAST_NO_DLS 		= 12,
	TIMEZONE_CENTRAL_NO_DLS 	= 11,
	TIMEZONE_MOUNTAIN_NO_DLS 	= 10,
	TIMEZONE_PACIFIC_NO_DLS 	= 9,
	TIMEZONE_ALASKAN_NO_DLS 	= 8,
	TIMEZONE_HAWAIIAN_NO_DLS 	= 7,

	TIMEZONE_EAST_DLS = 6,
	TIMEZONE_CENTRAL_DLS = 5,
	TIMEZONE_MOUNTAIN_DLS = 4,
	TIMEZONE_PACIFIC_DLS = 3,
	TIMEZONE_ALASKAN_DLS = 2,
	TIMEZONE_HAWAIIAN_DLS = 1,
	TIMEZONE_INVALID = 0
} TimeZones_t;

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
