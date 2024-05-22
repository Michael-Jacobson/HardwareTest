/********************************************************************************************************************************/
/********************************************************************************************************************************/
/*                                                         C674XTYPES.H                                                         */
/*                                COPYRIGHT NOTICE: (c) Ultratec, Inc.  2016  ALL RIGHTS RESERVED                               */
/********************************************************************************************************************************/
/*                                                                                                                              */
/* This file contains all the typedefs that helps make our code portable.  In addition, we include two SYSBIOS includes that    */
/* are needed for 90% of our source files.  This file supports TI's TMS320C6748 DSP.                                            */
/*                                                                                                                              */
/********************************************************************************************************************************/
#ifndef C674XTYPES_H
#define C674XTYPES_H
/****************/
/*   Includes   */
/****************/

/****************/
/* File defines */
/****************/
#ifndef TRUE
#define TRUE   1U
#endif

#ifndef FALSE
#define FALSE  0
#endif

#ifndef NULL
#define NULL   0
#endif

#define FIVE_MSECS                  (5U)
#define TEN_MSECS                   (10U)
#define FIFTEEN_MSECS               (15U)
#define TWENTY_MSECS                (20U)
#define TWENTY_FIVE_MSECS           (25U)
#define ONE_HUNDRED_MSECS           (100U)
#define THREE_HUNDRED_MSECS         (300U)
#define FIVE_HUNDRED_MSECS          (500U)
#define ONE_SECOND_IN_MSECS         (1000U)
#define THREE_SECONDS_IN_MSECS      (3U * 1000U)
#define FIVE_SECONDS_IN_MSECS       (5U * 1000U)
#define TEN_SECONDS_IN_MSECS        (10U * 1000U)
#define FIFTEEN_SECONDS_IN_MSECS    (15U * 1000U)
#define THIRTY_SECONDS_IN_MSECS     (30U * 1000U)
#define ONE_MINUTE_IN_MSECS         (60U * 1000U)
#define TWO_MINUTES_IN_MSECS        (2U * 60U * 1000U)
#define FIVE_MINUTES_IN_MSECS       (5U * 60U * 1000U)
#define ONE_HOUR_IN_MSECS           (60U * 60U * 1000U)
#define TWO_HOURS_IN_MSECS          (2U * 60U * 60U * 1000U)
#define FOUR_HOURS_IN_MSECS         (4U * 60U * 60U * 1000U)
#define EIGHT_HOURS_IN_MSECS        (8U * 60U * 60U * 1000U)

/****************/
/* File Enums   */
/****************/

/****************/
/* File structs */
/****************/
/* The following typedefs work for the COFF or EABI object formats from the C6000 complier. */
typedef signed char           S8;      /* -128 to 127 */
typedef signed short          S16;     /* -32,768 to +32,767 */
typedef signed int            S32;     /* -2,147,483,648 to +2,147,483,647 */
//typedef __int40_t             S40;     /* -549,755,813,888 to +549,755,813,887 */
typedef signed long long      S64;     /* -9,233,372,036,854,775,808 to +9,233,372,036,854,775,807 */

typedef unsigned char         U8;      /* 0 to 255 */
typedef unsigned short        U16;     /* 0 to 65,535 */
typedef unsigned int          U32;     /* 0 to 4,294,967,295 */
//typedef unsigned __int40_t    U40;     /* 0 to 1,099,511,627,775 */
typedef unsigned long long    U64;     /* 18,446,744,073,709,551,615 */

typedef float                 F32;     /* 1.175494e-38 to 3.40282346e+38 */
typedef double                F64;     /* 2.22507385e-308 to 1.79769313e+308 */
typedef unsigned char         BOOL;    /* 0 or 1 */
typedef unsigned char         CCHAR;   /* 0 to 255 */
typedef char *                STRING;  /* All pointers are unsigned 32 bit:  0x00000000 to 0xFFFFFFFF */

/****************/
/* File Externs */
/****************/

/*******************************
 * Public function prototypes
 *******************************/
#endif	/* C674XTYPES_H */

