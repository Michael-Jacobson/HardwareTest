
#ifndef AES_USER_H
#define AES_USER_H
#include <common/include/adt_typedef_user.h>

#ifdef CPP
 #define ADT_FT extern "C"
#else
 //#define ADT_FT
#endif

#if ((defined(__TMS320C55XX__)) || (defined(_TMS320C5XX)) )
#define CHAN_INST_SIZE 242
#else
#define CHAN_INST_SIZE 122
#endif
typedef struct 
{
	ADT_UInt32 Channel_data[CHAN_INST_SIZE];
} AES_ADT_ChanInst_t; 

ADT_FT void AES_ADT_KeyInit (ADT_UInt8 Key[], short KeyLength, AES_ADT_ChanInst_t *AesChanInst);
ADT_FT void AES_ADT_Encrypt (	ADT_UInt8 *Text, ADT_UInt8 *CipherText, AES_ADT_ChanInst_t *AesChanInst);
ADT_FT void AES_ADT_Decrypt (	ADT_UInt8 *CipherText, ADT_UInt8 *OutputText, AES_ADT_ChanInst_t *AesChanInst);

#endif
