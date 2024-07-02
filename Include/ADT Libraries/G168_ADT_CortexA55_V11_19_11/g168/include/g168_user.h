/*******************************************************************

 Program name  : g168_user.h

 Description : public header file for G.168 

 Revision History
 11/23/2015
 Update for 64-bit

 5/23/2016
 Update for better double talk

 (c) Copyright 2001 -2016, Adaptive Digital Technologies, Inc.

********************************************************************/

#ifndef _G168_USER_H
#define _G168_USER_H
#if (defined(__SSE2__) || (_M_IX86_FP == 2)) && !defined(DISABLE_SSE2)
#define SSE2
#endif
#if (defined(WIN32) && defined(_USRDLL) && defined(__cplusplus))
#include "forcelib.h"
#endif

#include "align.h"

#ifndef ADT_FT
#ifdef WANT_STATIC
#define ADT_FT static
#else
#define ADT_FT
#endif
#endif
#define EC_VARIANT_LEC 0
#define EC_VARIANT_NEC 1
#define EC_VARIANT_PEC 2
#define EC_VARIANT_PES 3
#define EC_VARIANT_AEC 4
#define EC_VARIANT_AEC_LT 5
#ifndef EC_VARIANT
#error EC_VARIANT is not #defined. Please set it to EC_VARIANT_LEC, EC_VARIANT_NEC, or EC_VARIANT_PEC based upon the variant of EC library that you are using
#endif

#ifdef AECG1_5_ENABLE
 #ifndef MAX_SUPP_TAP_LENGTH
  #ifdef USE_EC_DEFAULTS
   #define MAX_SUPP_TAP_LENGTH 256
  #else
   #error MAX_SUPP_TAP_LENGTH not #defined. Define it or define USE_EC_DEFAULTS to use the default value
  #endif
 #endif
#endif
#ifndef MAX_FRAME_SIZE
 #ifdef USE_EC_DEFAULTS
  #define MAX_FRAME_SIZE 80
 #else
  #error MAX_FRAME_SIZE not defined. Define it or define USE_EC_DEFAULTS to use the default value
 #endif
#endif

#if EC_VARIANT==EC_VARIANT_LEC
 #define SHORT_TAIL	//Define this to disable tail search stuff
 #ifndef MAX_TAP_LENGTH
  #ifdef USE_EC_DEFAULTS
   #define MAX_TAP_LENGTH 128
  #else
   #error MAX_TAP_LENGTH not #defined. Define it or define USE_EC_DEFAULTS to use the default value
  #endif
 #endif
 #ifndef MAX_FIR_SEGMENTS
  #define MAX_FIR_SEGMENTS 1
 #endif
 #ifndef MAX_FIR_SEGMENT_LENGTH
  #define MAX_FIR_SEGMENT_LENGTH MAX_TAP_LENGTH
 #endif
#else
 #undef  SHORT_TAIL	//UnDefine this long tail version
#endif
#if (EC_VARIANT==EC_VARIANT_NEC) || (EC_VARIANT==EC_VARIANT_PEC) || (EC_VARIANT==EC_VARIANT_AEC_LT)
 #define LONG_TAIL
#endif

#if (EC_VARIANT == EC_VARIANT_NEC)
 #ifndef MAX_TAP_LENGTH
  #ifdef USE_EC_DEFAULTS
   #define MAX_TAP_LENGTH 1024
  #else
   #error MAX_TAP_LENGTH not #defined. Define it or define USE_EC_DEFAULTS to use the default value
  #endif
 #endif
 #ifndef MAX_FIR_SEGMENTS
  #ifdef USE_EC_DEFAULTS
   #define MAX_FIR_SEGMENTS 3
  #else
   #error MAX_FIR_SEGMENTS not #defined. Define it or define USE_EC_DEFAULTS to use the default value
  #endif
 #endif
 #if MAX_FIR_SEGMENTS > 3
  #error MAX_FIR_SEGMENTS must not exceed 3 for NEC configuration
 #endif
 #ifndef MAX_FIR_SEGMENT_LENGTH
  #ifdef USE_EC_DEFAULTS
   #define MAX_FIR_SEGMENT_LENGTH 48
  #else
   #error MAX_FIR_SEGMENT_LENGTH not #defined. Define it or define USE_EC_DEFAULTS to use the default value
  #endif
 #endif

#endif
#if (EC_VARIANT==EC_VARIANT_PEC) || (EC_VARIANT==EC_VARIANT_AEC_LT)
 #if EC_VARIANT == EC_VARIANT_PEC
  #define G168_PLUS
 #endif
 #ifndef MAX_TAP_LENGTH
  #ifdef USE_EC_DEFAULTS
   #define MAX_TAP_LENGTH 4096
  #else
   #error MAX_TAP_LENGTH not #defined. Define it or define USE_EC_DEFAULTS to use the default value
  #endif
 #endif
 #ifndef MAX_FIR_SEGMENTS
  #ifdef USE_EC_DEFAULTS
   #define MAX_FIR_SEGMENTS 1
  #else
   #error MAX_FIR_SEGMENTS not #defined. Define it or define USE_EC_DEFAULTS to use the default value
  #endif
 #endif
 #if MAX_FIR_SEGMENTS > 3
  #error MAX_FIR_SEGMENTS must not exceed 3 for PEC configuration
 #endif
 #ifndef MAX_FIR_SEGMENT_LENGTH
  #ifdef USE_EC_DEFAULTS
   #define MAX_FIR_SEGMENT_LENGTH 96
  #else
   #error MAX_FIR_SEGMENT_LENGTH not #defined. Define it or define USE_EC_DEFAULTS to use the default value
  #endif
 #endif
#endif
#if EC_VARIANT==EC_VARIANT_AEC
 #define SHORT_TAIL
#define MAX_TAP_LENGTH 256
#endif

#define V10
#ifdef V10
//#ifndef SHORT_TAIL
#define V10_HIGH_ERL
#define USE_ERL2
//#endif
#endif

#include "common/include/adt_typedef_user.h"

//-----------------------------------------------
// NLP Types
//-----------------------------------------------

#define NLP_OFF 0
#define NLP_MUTE 1
#define NLP_RAND_CNG 2
#define NLP_HOTH_CNG 3
#define NLP_SUPP 4
#define NLP_CNG1 5
#define NLP_SUPP_AUTO 6

#define NLP_DEFAULT_CNG NLP_HOTH_CNG

//-----------------------------------------------
// MIPS Conservation Modes
//-----------------------------------------------
#define MIPS_CONSERVE_OFF		0
#define MIPS_CONSERVE_LOW		1
#define MIPS_CONSERVE_MEDIUM	2
#define MIPS_CONSERVE_HIGH		3
#define MIPS_CONSERVE_EXTREME	4

// SmartPacketModeSelect values
#define SMART_PACKET_SELECT_OFF 0
#define SMART_PACKET_SELECT_PARTIAL_OPTIMIZATION 1
#define SMART_PACKET_SELECT_MAXIMUM_OPTIMIZATION 2
#define SMART_PACKET_SELECT_SUPPRESSION_ONLY 3

// SmartPacketMode status values
#define SMART_PACKET_CANCEL 0
#define SMART_PACKET_SUPPRESS 1
#define SMART_PACKET_BYPASS 2

//-----------------------------------------------
// CNG States
//-----------------------------------------------
#define CNG_OFF 0
#define CNG_ONSET 1
#define CNG_ON 2
#define CNG_RAMP_DOWN 3
#ifdef AECG1_5_ENABLE
#define CNG_SUPP 4
#endif
#define DYNAMIC_NLP_DISABLE (-1)
#define MAX_DYNAMIC_NLP_AGGRESSIVENESS (20)
//-----------------------------------------------
// LEC_ADT_g168Init result codes
//-----------------------------------------------

#define LEC_ADT_INIT_OK 0
#define LEC_ADT_INIT_ERR_BADAPIVERSION 1
#define LEC_ADT_INIT_ERR_CHANNEL_SIZE 2
#define LEC_ADT_INIT_ERR_ECHOPATH_SIZE 3
#define LEC_ADT_INIT_ERR_DASTATE_SIZE 4
#define LEC_ADT_INIT_ERR_SASTATE_SIZE 5
#define LEC_ADT_INIT_ERR_SASCRATCH_SIZE 6
#define LEC_ADT_INIT_ERR_BGECHOPATH_SIZE 7
#define LEC_ADT_INIT_ERR_DASCRATCH_SIZE 8
#define LEC_ADT_INIT_ERR_ECHOPATH_ALIGN 9
#define LEC_ADT_INIT_ERR_DASTATE_ALIGN 10
#define LEC_ADT_INIT_ERR_SASTATE_ALIGN 11
#define LEC_ADT_INIT_ERR_SASCRATCH_ALIGN 12
#define LEC_ADT_INIT_ERR_BGECHOPATH_ALIGN 13
#define LEC_ADT_INIT_ERR_DASCRATCH_ALIGN 14

// Decimate BG LMS Stuff
//-----------------------------------------------
#define DEC_RATE 4 // 8, May3,2002 back to 8 using Brian's filter


#define IABSY_STRIDE 4
#define FIR_SIZE 48

#define MAX_TAP_LENGTH_D (MAX_TAP_LENGTH/DEC_RATE)
#ifndef _TMS320C6X
#define MAX_IABSY_SIZE (MAX_TAP_LENGTH_D/IABSY_STRIDE)
#else
#define MAX_IABSY_SIZE ((MAX_TAP_LENGTH_D/IABSY_STRIDE) + (MAX_FRAME_SIZE/DEC_RATE))
#endif


//-----------------------------------------------
// The G168Params structure contains parameters that are to be set
// during echo canceller initialization. Many of these parameters 
// can also be changed at any time
//-----------------------------------------------
typedef union
{
    struct 
    {
    	ADT_UInt16 Unused:10;
    	ADT_UInt16  G165NearFlagLatched:1;
    	ADT_UInt16  G165NearFlag:1;
    	ADT_UInt16  G164NearFlag:1;
    	ADT_UInt16  G165FarFlagLatched:1;
    	ADT_UInt16  G165FarFlag:1;
    	ADT_UInt16  G164FarFlag:1;
	}		Bits;
	ADT_UInt16 Word;
} G165Report_t;
typedef union
{
	struct
	{
		ADT_UInt16 Unused:12;
		ADT_UInt16 NLPOn:1;
		ADT_UInt16 DoubleTalkActive:1;
		ADT_UInt16 NarrowbandToneActive:1;
		ADT_UInt16 SS7ToneActive:1;
	}		Bits;
	ADT_UInt16 Word;
}		G168StatusFlags_t;
typedef union
{
	struct
	{
		ADT_UInt16 Unused:15;
		ADT_UInt16 Reconvergence:1;
	}		Bits;
	ADT_UInt16 Word;
}		G168EventFlags_t;

typedef struct
{
	ADT_UInt8 FrameCount;
	ADT_UInt8 ReductionAmount;
}		MIPSConserve_t;

#define ADT_EC_API_VERSION 11
typedef struct
{
   ADT_Int16 APIVersion;			//API Version
    ADT_Int16 ChannelSizeI8;		//Channel (Instance) structure size, in bytes
	ADT_Int16 EchoPathSizeI8;		//Echo path array size, in bytes
	ADT_Int16 DAStateSizeI8;	//DAState structure size in bytes
	ADT_Int16 SAStateSizeI8;	//SAState structure size in bytes
	ADT_Int16 SAScratchSizeI8;		//SAScratch structure size in bytes
	ADT_Int16 BGEchoPathSizeI8;		//BGEchoPath array size in bytes
	ADT_Int16 DAScratchSizeI8;	//DAScratch structure size in bytes

// Control Variables 
   ADT_Int16 FrameSize;         // Number of samples per frame
   ADT_Int16 TapLength;			// The number of taps (tail length) 
   ADT_Int16 NLPType;			// Type of NLP selected
   ADT_Int16 AdaptEnable;		// Enables Coefficinet updates
   ADT_Int16 BypassEnable;		// If set,1: EC is bypassed, 2: EC is bypassed but still adapt and update its states
   ADT_Int16 G165DetectEnable;	// Enables the G.165 tone disabler
   ADT_Int16 DoubleTalkThres; 	// Double talk threshold
   ADT_Int16 MaxDoubleTalkThres; // Maximum double-talk threshold
   ADT_Int16 SaturationLevel;	// Far end input level above which significant saturation/nonlinearity may be expected.  

   ADT_Int16 DynamicNLPAggressiveness; //0..MAX_DYNAMIC_NLP_AGGRESSIVENESS, or DYNAMIC_NLP_DISABLED
   ADT_Int16 NLPThres;			// NLP center clipped Threshold
   ADT_Int16 NLPUpperLimitThresConv;// Upper limit for dynamic NLP Threshold after EC converged
   ADT_Int16 NLPUpperLimitThresUnConv;// Upper limit for dynamic NLP Threshold when EC not converged yet
   ADT_Int16 NLPSaturationThreshold;	//NLP threshold under conditions of possible saturation
   ADT_Int16 NLPMaxSupp;		// NLP supress scale,limit on scale the NLP will suppress
   ADT_Int16 CNGNoiseThreshold;	// Maximum residual level considered
							// for CNG noise level calculation
   ADT_Int16 AdaptLimit;		// Maximum number of adaptations per frame
   ADT_Int16 CrossCorrLimit;	// Maximum number of cross-correlation updates per frame
   ADT_Int16 FIRTapCheckPeriod;	// How often to look for a peak
   ADT_Int16 NFIRSegments;		// Number of FIR segments
   ADT_Int16 FIRSegmentLength;	// Number of taps in each filter segment

   ADT_Int16 TandemOperationEnable; //Enable tandem operation
   ADT_Int16 MixedFourWireMode;	// Handle possible 4-wire (echo-free) lines
   ADT_Int16 ReconvergenceCheckEnable; //2 enables, 0 disables

   MIPSConserve_t *pMIPSConserve;	//MIPS Conserve Common Data
   ADT_Int16 ChannelNumber;	//Channel number to be used for the purpose of MIPS conservation


// Smart Packet Mode specific parameters
	ADT_Int16 SmartPacketModeSelect;			//see SMART_PACKET_SELECT_* options above
	ADT_Int16 SmartPacketBypassERLThresholddB; //If ERL is less than this number, bypass the EC
	ADT_Int16 SmartPacketSuppressERLThresholddB; //if ERL is less than this number, perform suppression only, no cancellation
	ADT_Int16 SmartPacketNLPThresholddB;		//If we are performing suppress only, use this value as the NLP threshold

// Status Variables
   ADT_Int16 AdaptReport;			// Counts number of adaptations happened in current frame
   ADT_Int16 CrossCorrReport;		// Counts number of cross-correlation updates that occured in current frame
   G165Report_t G165Status;     // G165 tone detect status
//#ifdef AECG1_5_ENABLE
   ADT_Int16 SuppTapLength;		// Number of taps beyond TapLength that covers suppress (not cancel) range
   ADT_Int16 SuppThresh;		// NLP Threshold for suppress range
//#endif
}     G168ParamsV11_t;


//-----------------------------------------------
// The modifiable Params structure contains parameters that can 
// be changed at any time
//-----------------------------------------------
typedef struct
{
// Control Variables 
   ADT_Int16 NLPType;			// Type of NLP selected
   ADT_Int16 AdaptEnable;		// Enables Coefficinet updates
   ADT_Int16 BypassEnable;		// If set,1: EC is bypassed, 2: EC is bypassed but still adapt and update its states
   ADT_Int16 G165DetectEnable;	// Enables the G.165 tone disabler
   ADT_Int16 DoubleTalkThres; 	// Double talk threshold
   ADT_Int16 MaxDoubleTalkThres; // Maximum double-talk threshold
   ADT_Int16 SaturationLevel;	// Far end input level above which significant saturation/nonlinearity may be expected.  

   ADT_Int16 NLPThres;			// NLP center clipped Threshold
   ADT_Int16 NLPUpperLimitThresConv;			// Upper limit for dynamic NLP Threshold after EC converged
   ADT_Int16 NLPUpperLimitThresUnConv;			// Upper limit for dynamic NLP Threshold when EC not converged yet
   ADT_Int16 NLPSaturationThreshold;	//NLP threshold under conditions of possible saturation
   ADT_Int16 NLPMaxSupp;		// NLP supress scale,limit on scale the NLP will suppress
   ADT_Int16 CNGNoiseThreshold;	// Maximum residual level considered
							// for CNG noise level calculation
   ADT_Int16 AdaptLimit;		// Maximum number of adaptations per frame
   ADT_Int16 CrossCorrLimit;	// Maximum number of cross-correlation updates per frame
   ADT_Int16 FIRTapCheckPeriod;	// How often to look for a peak

   ADT_Int16 TandemOperationEnable; //Enable tandem operation
   ADT_Int16 MixedFourWireMode;	// Handle possible 4-wire (echo-free) lines
   ADT_Int16 ReconvergenceCheckEnable;
#ifdef AECG1_5_ENABLE
   ADT_Int16 SuppTapLength;		// Number of taps beyond TapLength that covers suppress (not cancel) range
   ADT_Int16 SuppThresh;		// NLP Threshold for suppress range
#endif
}     G168RuntimeParamsV11_t;

typedef struct
{
	ADT_Int16 AdaptReport;
	ADT_Int16 CrossCorrReport;
	G165Report_t G165Status;
	ADT_Int16 ConvergenceStat;
	ADT_Int16 NLPFlag;
	ADT_Int16 DoubleTalkFlag;
	ADT_Int16 SmartPacketMode;
	ADT_Int16 ERL1;
	ADT_Int16 ERL2;
	ADT_Int16 ERLE;
	G168StatusFlags_t StatusFlags;
	G168EventFlags_t EventFlags;	//Latched events, cleared by LEC_ADT_g168GetStatus

#ifdef LONG_TAIL
	ADT_Int16 ReflectorLocation[3];
	ADT_Int16 ReflectorLength[3];
#endif
}		G168StatusV11_t;

//-----------------------------------------------
// The SmartFIRData_t structure is used to implement
// the segmented FIR filter. The Offset is used to 
// store the distance between the end of one FIR segment
// to the beginning of the next one. The Length is used
// to store the number of taps in a filter segment. There
// is one instance of the SmartFIRData_t structure for each
// of the N segments in the multi-segment FIR.
//-----------------------------------------------

typedef struct
{
	ADT_Int16 Offset;	//Number of taps to skip from end of
						//Previous FIR Segment
	ADT_Int16 Length;	//Length of current FIR segment
}		SmartFIRData_t;


//-----------------------------------------------
// The DTFIRSave_t structure is used to save the coefficients.
// This data can be recalled at the onset of 
// double-talk to allow the filter to go back to a more
// converged state.
//-----------------------------------------------

typedef struct
{
	ADT_Int16 StructSize;
	ADT_Int16 ChannelTapLength;
	ADT_Int16 CoefQ;
	ADT_Int16 BGCoefQ;
#ifdef LONG_TAIL
	ADT_Int16 NFIRSegments;
	ADT_Int16 FIRSegmentLength;
	SmartFIRData_t SmartFIR[3];	// Smart FIR Data
	ADT_Int16 Coef[3*MAX_FIR_SEGMENT_LENGTH]; // Coefficients
	ADT_Int16 BG_Coef[MAX_TAP_LENGTH_D];
#else
	ADT_Int16 Coef[MAX_TAP_LENGTH]; // Coefficients
#endif
}		DTFIRSave_t;

//-----------------------------------------------
// The DTStruct structure contains all the information needed to 
// handle the save / restore of FIR coefficients and associated 
// information.
// The DTStruct has two instances of the DTFIRSave structure. We 
// store the DTFIRSave information from the previous two frames.
// This is because it is possible that double-talk had begun
// during the previous frame, but was not yet detected. Some
// filter divergence could have occured already in the previous
// frame's coefficients. We therefore restore the coefficients and
// SmartFIRData from two frames back.
//-----------------------------------------------

typedef struct
{
	ADT_Int16 RestoreFlag;	//If set, restore has been done during 
							//the current double-talk burst.
	ADT_Int16 CoefQ;
#ifdef LONG_TAIL
	SmartFIRData_t SmartFIR[3];
	ADT_Int16 Coef[3*MAX_FIR_SEGMENT_LENGTH];
#else
	ADT_Int16 Coef[MAX_TAP_LENGTH];
#endif
}		DTStruct_t;


typedef struct
{
#ifdef SHORT_TAIL
	ADT_Int16Aligned(Coef,MAX_TAP_LENGTH,BYTE_ALIGN_COUNT);
#else
	ADT_Int16Aligned(BG_Coef, MAX_TAP_LENGTH_D,BYTE_ALIGN_COUNT);
#endif
}		G168_DA_State_t;
//#ifndef AECG1_5_ENABLE
#define G165_DETECT_ENABLE
//#endif
typedef struct
{
	#ifdef G165_DETECT_ENABLE
		ADT_Int32 G165DetFarChannel[20];
		ADT_Int32 G165DetNearChannel[20];
	#endif
	ADT_Int32 LuHistory[16];
	DTStruct_t DTStruct;
	ADT_Int32 M2[MAX_TAP_LENGTH/16+1];
	ADT_Int16 M[MAX_TAP_LENGTH/16+1];

#ifdef AECG1_5_ENABLE
	ADT_Int16 MSuppress[MAX_SUPP_TAP_LENGTH/16+1];
#endif

	#ifdef LONG_TAIL
		ADT_Int16 iabsyHist[MAX_TAP_LENGTH/16];
		ADT_Int16 Coef[MAX_FIR_SEGMENTS*MAX_FIR_SEGMENT_LENGTH];
		ADT_Int16 BG_iabsyHist[MAX_IABSY_SIZE];
		ADT_Int16Aligned(BG_FarFIRState,FIR_SIZE,BYTE_ALIGN_COUNT);
		ADT_Int16Aligned(BG_NearFIRState,FIR_SIZE,BYTE_ALIGN_COUNT);
//		ADT_Int16 BG_FarFIRState[FIR_SIZE];
//		ADT_Int16 BG_NearFIRState[FIR_SIZE];
	#endif
	
}		G168_SA_State_t;
//-----------------------------------------------
// The Scratch structure contains echo canceller scratch
// (not persistent and not per-channel) arrays. By declaring them
// here, it is the users choice whether to share scratch amongst all
// channels or allocate scratch for each channel, which would be
// necessary in a preemptive environment.
//-----------------------------------------------
typedef struct
{
	ADT_Int16 NearCopy[MAX_FRAME_SIZE * 2];

	ADT_Int16 ResidualLocked[MAX_FRAME_SIZE];
	ADT_Int16 ResidualUnlocked[MAX_FRAME_SIZE];

#ifndef SHORT_TAIL
	ADT_Int16 ExpHistogram[32];
	ADT_Int16 Seg16Flags[MAX_TAP_LENGTH/16];
	ADT_Int32 Seg32Flags[MAX_TAP_LENGTH/16];
	ADT_Int16 Space;	//In case a 16 bit alignment pushed us out a bit

#endif
	DTStruct_t DTStruct;
#ifdef _TMS320C6X
	ADT_Int16 UpdateBuffer[MAX_FRAME_SIZE>>2];
	ADT_Int16 NearDecBuffer[MAX_FRAME_SIZE>>2];
#endif

}		G168_Scratch_t;

typedef struct
{
	ADT_Int16Aligned (Coef, MAX_TAP_LENGTH, BYTE_ALIGN_COUNT);
}		G168_DAScratch_t;

//-----------------------------------------------

//-----------------------------------------------
// The G168Channel structure contains all the per-channel (per-instance)
// persistant variables and arrays.
//-----------------------------------------------


typedef struct	//G168Channel_t
{
#if defined(G168_PLUS)
	ADT_Int16 ChannelData[372];
#elif defined(LONG_TAIL)
	ADT_Int16 ChannelData[368];
#elif defined (SHORT_TAIL)
	ADT_Int16 ChannelData[348];
#endif
}		G168ChannelInst_t;

#if !defined(ADT_FTG) && (defined(_WINDOWS) || defined(WIN32))
 #if defined(ADTEC) || defined(CPP) //compiling ADTEC.CPP or other DLL
  #define ADT_FTG extern "C"
 #else
  #define ADT_FTG
 #endif
#elif !defined(ADT_FTG)
 #define ADT_FTG ADT_FT
#endif

ADT_FT void LEC_ADT_coefStore(G168ChannelInst_t *Channel, ADT_Int16 *store_p);
ADT_FT void LEC_ADT_coefLoad(G168ChannelInst_t *Channel, ADT_Int16 *store_p);
ADT_FT ADT_Int16 LEC_ADT_coefExpand(ADT_Int16 *store_p, ADT_Int16 *Expanded_p);

//-----------------------------------------------
// Function Prototypes
//-----------------------------------------------
// LEC initialization
ADT_API ADT_UInt8 LEC_ADT_g168Init (G168ChannelInst_t *Channel, 
					   G168ParamsV11_t *Params, 
					   ADT_Int16 *EchoPath_p,
					   G168_DA_State_t *G168_DAState_p, 
					   G168_SA_State_t *G168_SAState_p,
					   G168_Scratch_t *G168Scratch_p, 
					   ADT_Int16 *BG_EchoPath_p,
					   G168_DAScratch_t *G168_DAScratch_p);
// Combined LEC echo cancel call
ADT_API void LEC_ADT_g168Cancel(G168ChannelInst_t *Channel, ADT_Int16 NearEnd[], ADT_Int16 FarEnd[], ADT_Int16 Near_PreNLP[]);


// Split LEC echo cancel call
// Both functions must be called, even users only need the pre-NLP near output

// cancel echo, no NLP yet
ADT_API void LEC_ADT_g168echoCancel(G168ChannelInst_t *Channel, ADT_Int16 NearEnd[], ADT_Int16 FarEnd[]);
// post echo-cancel actions
ADT_API void LEC_ADT_g168postCancel(G168ChannelInst_t *Channel, ADT_Int16 NearEnd[], ADT_Int16 FarEnd[]);


// Utility to show the current param setting and status of the EC
// get the setting utility
ADT_API void LEC_ADT_g168GetConfig (G168ChannelInst_t *Channel, 
					   G168ParamsV11_t *Params);
// Utility to change the modifiable parameters without calling init function
// config utility
ADT_API void LEC_ADT_g168RuntimeConfig (G168ChannelInst_t *Channel, 
					   G168RuntimeParamsV11_t *Params);

ADT_API void LEC_ADT_g168ChangeChannelNumber(G168ChannelInst_t *Channel, ADT_Int16 ChannelNumber);


// Utility to show some EC status
// This function runs faster than  LEC_ADT_g168GetConfig. It does not return configuration settings
// that have to be recomputed

ADT_API void LEC_ADT_g168GetStatus(G168ChannelInst_t *ChannelInst, 
					   G168StatusV11_t *Status);


ADT_API ADT_UInt8 LEC_ADT_getSizes(G168ParamsV11_t *pInParams, G168ParamsV11_t *pOutParams);

// In _DEBUG mode only. Otherwise stubbed out to prevent inclusion of printf
ADT_API ADT_UInt8 LEC_ADT_printSizes(G168ParamsV11_t *pParams);

// Reserved
ADT_API G168ChannelInst_t *LEC_ADT_create(G168ParamsV11_t *pParams, ADT_UInt8 *pErrorCode);
ADT_API void LEC_ADT_delete(G168ChannelInst_t *pInstance);

#endif //_G168_USER_H
