
#ifndef _NRADT_USER_H_
#define _NRADT_USER_H_

#include "common/include/adt_typedef_user.h"
#ifdef ALG_DUMP_ENABLE
#include <common/private/algdump/algdump.h>
#endif

#ifdef __TMS320C55XX__
#define NRADT_SCRATCH_SIZE (7720/8+1)   
#else
#define NRADT_SCRATCH_SIZE (5672/8+1)
#endif

#ifndef ALG_DUMP_ENABLE
#define NRADT_CHANNEL_SIZE (12034/8+1+2)
#else
#define NRADT_CHANNEL_SIZE (11961/8+1+50000/8+2)
#endif



typedef struct NRADT_Scratch_t {
ADT_Int64  data[NRADT_SCRATCH_SIZE];
} NRADT_Scratch_t;

typedef struct NRADT_Channel_t {
ADT_Int64 data[NRADT_CHANNEL_SIZE];
} NRADT_Channel_t;

ADT_API char *NR_ADT_getLibraryVersion(void) ;
ADT_API void NR_ADT_getSizes(ADT_UInt32 *pChannelSize, ADT_UInt32 *pScratchSize);
ADT_API	void NR_ADT_reduce(NRADT_Channel_t *Channel, ADT_Int16 Input[], ADT_Int16 Output[], ADT_Int16 Length); 
ADT_API	void NR_ADT_init (NRADT_Channel_t *Channel, NRADT_Scratch_t *pScratch); 
ADT_API	void NR_ADT_initWithParam (NRADT_Channel_t *Channel, NRADT_Scratch_t *pScratch, ADT_Int16 ReductionAmount); 

#endif
