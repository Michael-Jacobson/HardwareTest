/*******************************************************************

 Program name  : agc_user.h

 Description : public header file for AGC .

 Date   :  5/15/2001

 Version  :  1.0

 Revision History
 6/29/2015
 Update for 64-bit

 (c) Copyright 2001 -2015, Adaptive Digital Technologies, Inc.

********************************************************************/
#ifndef AGC_USER_H
#define AGC_USER_H

#ifdef __cplusplus
extern "C" {
#endif

#define AGC_LIB_VER 309
#define AGC_API_VER 300

#define AGC_INIT_OK 0
#define AGC_INIT_ERR_WRONG_API_VERSION 1
#define AGC_INIT_INSTANCE_SIZE_TOO_SMALL 2
#define AGC_INIT_UNSUPPORTED_SAMPLING_RATE 3

#include "common/include/adt_typedef_user.h"
#define AGC_SIZE 17

#ifdef __TMS320C55XX__ 
#define AGC_INSTANCE_SIZE_I64 13
#elif defined(_TMS320C5XX)
#define AGC_INSTANCE_SIZE_I64 20
#else
#define AGC_INSTANCE_SIZE_I64 12
#endif

typedef struct AGCInstance_t
{
   ADT_Int64 agcvars[AGC_INSTANCE_SIZE_I64]; 
} AGCInstance_t;

// Define sampling rates in Hz
#define FS_8000 8000
#define FS_11250 11250
#define FS_12000 12000
#define FS_16000 16000
#define FS_22500 22050
#define FS_32000 32000
#define FS_44100 44100
#define FS_48000 48000
#define STICKY_OFF    0
#define STICKY_SPEECH 1
#define STICKY_ALL    2

typedef struct AGCParamV3_00_t
{
   ADT_UInt32 APIVersion;
   ADT_UInt32 instanceSizeI8;	  // InstanceSize in bytes
   ADT_Int16  targetPowerLowdBm;  // INPUT range: 0 ... -30
   ADT_Int16  targetPowerHighdBm; // INPUT range: 0 ... -30
   ADT_Int16  maxLossdB;          // INPUT range: 0 ... -23
   ADT_Int16  maxGaindB;          // INPUT range: 0 ... +23
   ADT_Int16  lowSigThreshdBm;    // INPUT threshold in dBm of low energy signal 
   ADT_Int16  stickyControl;      // INPUT Flag, 0/1/2, to remember gain when signal below above
   ADT_Int16  stickyInitialGaindB;// INPUT range: -23 ... +23
   ADT_Int32  samplingRateHz;     // INPUT sampling rate of the input/output data

} AGCParamV3_00_t;

/*-------------------------------------------
  This section contains function prototypes for use when using the algorithm
  in a statically linked mode.
-------------------------------------------*/
ADT_API ADT_UInt16 AGC_ADT_getLibraryVersion(void);
ADT_API ADT_UInt16 AGC_ADT_getAPIVersion(void);

ADT_API ADT_UInt16 AGC_ADT_getInstanceSize (
   AGCParamV3_00_t * pParams
);

ADT_API ADT_UInt8 AGC_ADT_agcInit(
   AGCInstance_t * pChannel,
   AGCParamV3_00_t * pParams
);

ADT_API ADT_UInt8 AGC_ADT_agcSetParams(
   AGCInstance_t *pChannel,
   AGCParamV3_00_t *pParams
);

ADT_API void AGC_ADT_agcRun(
   AGCInstance_t *pChannel,
   ADT_Int16 * pInputSamples,
   ADT_Int16 FrameSize,
   ADT_Int16 ForceLowSignalState,	
   ADT_Int16 BlockShift,
   ADT_Int16 * pOutputSamples,
   ADT_Int32 * Gains);

ADT_API void AGC_ADT_agcComputeGain(
  AGCInstance_t *pInstance,
  ADT_Int16 * pInputSamples,
  ADT_Int16 FrameSize,
  ADT_Int16 BlockShift
);
ADT_API void AGC_ADT_agcApplyGain(
  AGCInstance_t *pInstance,
  ADT_Int16 * pInputSamples,
  ADT_Int16 FrameSize,
  ADT_Int16 ForceLowSignalState,
  ADT_Int16 * pOutputSamples,
  ADT_Int32 * Gains);

#ifdef __cplusplus
}
#endif

#endif  /* file inclusion */
