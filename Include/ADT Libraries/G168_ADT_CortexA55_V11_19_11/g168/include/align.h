#ifndef _ALIGN_H
#define _ALIGN_H
#include "common/include/adt_typedef_user.h"
// Functions/Macros to help with array and structure alignment

// Define padding sizes and address masks
//   NBITS is the number of bits per word
//   Y is the required byte alignment
// PAD is the number of IntX elements that need to be padded to the array
// MASK is the address Mask to help compute an aligned address

// BYTE_ALIGN_COUNT is set on a platform-by-platform basis. This value specifies the byte boundary alignment
//  that is required to make use of aligned wide word loads and stores.
#ifdef __TMS320C55X__
 #define BYTE_ALIGN_COUNT 2
#else
 #define BYTE_ALIGN_COUNT 16	//Worst case
 #define ALIGN_DATA
#endif
#if 0
 #ifdef SSE2
  #define ALIGN_DATA
// For now, set the BYTE_ALIGN_COUNT to 1 because GCC doesn't support the 128-bit aligned load/store intrinsics
//   (FYI: MS C and intel ICC both do support it)
//#define BYTE_ALIGN_COUNT 16
  #define BYTE_ALIGN_COUNT 16
 #else	// default at byte align count of 16, just to be safe
  #define BYTE_ALIGN_COUNT 16
 #endif
#endif

#define MAX_PAD(NBITS,BYTE_ALIGN) (BYTE_ALIGN - 1)
#ifndef __TMS320C55XX__
#define MASK(NBITS,BYTE_ALIGN) (0xffffffffffffffff ^ MAX_PAD(NBITS,BYTE_ALIGN))
#define LENGTH(L,NBITS,BYTE_ALIGN) (L+MAX_PAD(NBITS,BYTE_ALIGN))
#define PTR_MASK(BYTE_ALIGN) (0xffffffffffffffff ^ (BYTE_ALIGN-1))
#else
#define MASK(NBITS,BYTE_ALIGN) (0xffffffff ^ MAX_PAD(NBITS,BYTE_ALIGN))
#define LENGTH(L,NBITS,BYTE_ALIGN) (L+MAX_PAD(NBITS,BYTE_ALIGN))
#define PTR_MASK(BYTE_ALIGN) (0xffffffff ^ (BYTE_ALIGN-1))
#endif

#ifdef ALIGN_DATA
#define PTR_ALIGN(P, BYTE_ALIGN) ((void *) ( (((ADT_Int64) P) + BYTE_ALIGN-1) & PTR_MASK(BYTE_ALIGN)) )
#else
#define PTR_ALIGN(P, BYTE_ALIGN) P
#endif
#ifdef ALIGN_DATA
#define ADT_Int16Aligned(_array, _length, _align_bytes) ADT_Int16 _array[LENGTH(_length, 16, _align_bytes)]
#define ADT_Int32Aligned(_array, _length, _align_bytes) ADT_Int32 _array[LENGTH(_length, 32, _align_bytes)]
#else
#define ADT_Int16Aligned(_array, _length, _align_bytes) ADT_Int16 _array[_length]
#define ADT_Int32Aligned(_array, _length, _align_bytes) ADT_Int32 _array[_length]
#endif

// Sample usage
//  ADT_Int16Aligned(a, 10, 8)  // declare a short int array "a" of length 10 for use with 8 byte alignment
//  short int *pa_align8 = PTR_ALIGN(a, 8)

// Specific byte aligns

#endif
