/*
 **************************************************************
 *
 * Copyright (c) 2000-2019, Adaptive Digital Technologies, Inc.
 *
 *
 **************************************************************
 */
#ifndef _VADCNG_USER_H
#define _VADCNG_USER_H

#include <common/include/adt_typedef_user.h>

#ifdef _USRDLL
//#include "./forcelib.h"
#endif

#define DEMO_TIME_OUT -1
#define SILENCE     0
#define VOICE       1
#define NON_VOICE   2

#define VAD_LIB_VER 504
#define VAD_API_VER 502


typedef struct VADCNG_Instance_t
{
   ADT_UInt64 vad_vars[30];
} VADCNG_Instance_t;

typedef struct VADCNG_Status_t
{
	ADT_Int16 VadFrameSize;
	ADT_Int16 CngFrameSize;
	ADT_UInt32 SortEnergy;
	ADT_Int16 LastVad;
} VADCNG_Status_t;

ADT_API ADT_UInt16 VAD_ADT_getInstanceSize (void);

ADT_API ADT_UInt16 VAD_ADT_getLibVersion (void);

ADT_API ADT_UInt16 VAD_ADT_getAPIVersion (void);

ADT_API void VADCNG_ADT_getStatus(VADCNG_Instance_t *ChannelInst,
						                VADCNG_Status_t *pStatus);

// Function Prototypes.
ADT_API void VADCNG_ADT_init(VADCNG_Instance_t *ChannelInst, 
					ADT_Int16 Thres,
					ADT_Int16 HangMSec,
					ADT_Int16 VadFrameSize,
					ADT_Int16 CngFrameSize,
					ADT_Int16 WindowSize,
					ADT_Int16 SamplingRate,
					ADT_Int16 CNGFIRLength	/* Max is 32, zero to use lookup instead */
					);
					
ADT_API ADT_Int16 VADCNG_ADT_vad (VADCNG_Instance_t *ChannelInst, 
						ADT_Int16 pcm_in[], 
						ADT_Int16 *VadLvl);
						
ADT_API ADT_Int16 VADCNG_ADT_cng (VADCNG_Instance_t *ChannelInst, 
					ADT_Int16 VadFlag, 
					ADT_Int16 Level, 
					ADT_Int16 OutputBuffer[]);

#endif  /* file inclusion */
