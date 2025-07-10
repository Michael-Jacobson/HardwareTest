/********************************************************************************************************************************/
/********************************************************************************************************************************/
/*                                                     CodecAIC3106.h                                                       */
/*                                COPYRIGHT NOTICE: (c) Ultratec, Inc.  2016  ALL RIGHTS RESERVED                               */
/********************************************************************************************************************************/
/*                                                                                                                              */
/* This file contains all the definitions, enums, structures, etc. for                        */
/*                                                                                                                              */
/********************************************************************************************************************************/
#ifndef INCLUDE_CODECAIC3106_H_
#define INCLUDE_CODECAIC3106_H_
/****************/
/*   Includes   */
/****************/

/****************/
/* File defines */
/****************/
#define CODECAIC3106_I2C_ADDR       0x18u

#define CODEC_PAGE0_REG0_PAGE1      0x01u

#define CODEC_PAGE0_REG1_RESET      0x80u

#define CODEC_PAGE0_REG2_ADC_FS     0xA0u
#define CODEC_PAGE0_REG2_DAC_FS     0x0Au

#define CODEC_PAGE0_REG3_PLL_EN     0x80u
#define CODEC_PAGE0_REG3_SET_P      0x01u
#define CODEC_PAGE0_REG3_SET_Q      (4u<<3u)


#define CODEC_PAGE0_REG4_SET_J      0x20u

#define CODEC_PAGE0_REG5_SET_D_MSB  0x1Eu

#define CODEC_PAGE0_REG6_SET_D_LSB  0x00u

#define CODEC_PAGE0_REG7_LEFT_DAC_PLAYS_LEFT    0x08u
#define CODEC_PAGE0_REG7_RIGHT_DAC_PLAYS_RIGHT  0x02u

#define CODEC_PAGE0_REG8_BCLK_OUT   0x80u
#define CODEC_PAGE0_REG8_WCLK_OUT   0x40u
#define CODEC_PAGE0_REG8_BCLK_RUN   0x10u
#define CODEC_PAGE0_REG8_DMIC_EN_128_SR 0x01u

#define CODEC_PAGE0_REG9_DSP_MODE   0x40u

#define CODEC_PAGE0_REG10_BIT_OFFSET    0x01u

#define CODEC_PAGE0_REG11_SET_R     0x01u

#define CODEC_PAGE0_REG12_LEFT_HIGH_PASS_0125       0x80u
#define CODEC_PAGE0_REG12_LEFT_HIGH_PASS_025        0xC0u
#define CODEC_PAGE0_REG12_RIGHT_HIGH_PASS_0125      0x20u
#define CODEC_PAGE0_REG12_RIGHT_HIGH_PASS_025       0x30u
#define CODEC_PAGE0_REG12_LEFT_DIG_EFF_FILTER_ON    0x08u
#define CODEC_PAGE0_REG12_LEFT_DE_EMPHASIS_ON       0x04u
#define CODEC_PAGE0_REG12_RIGHT_DIG_EFF_FILTER_ON   0x02u
#define CODEC_PAGE0_REG12_RIGHT_DE_EMPHASIS_ON      0x01u

#define CODEC_PAGE0_REG13_EN_HEADSET_DET            0x80u
#define CODEC_PAGE0_REG13_HEADSET_DEBOUNCE_128MS    (3u<<2)
#define CODEC_PAGE0_REG13_HEADSET_DEBOUNCE_512MS    (5u<<2)
#define CODEC_PAGE0_REG13_HEADSET_DET_MASK          0x60u
#define CODEC_PAGE0_REG13_HEADSET_W_MIC_DET         0x60u

#define CODEC_PAGE0_REG14_AC_COUPLED            0x80u
#define CODEC_PAGE0_REG14_STEREO_DIFFERENTIAL   0x40u
#define CODEC_PAGE0_REG14_HEADSET_DETECTED      0x10u

#define CODEC_PAGE0_REG15_LEFT_ADC_MUTE     0x80u
#define CODEC_PAGE0_REG15_LEFT_ADC_PGA_0DB  0x00u

#define CODEC_PAGE0_REG16_RIGHT_ADC_MUTE    0x80u
#define CODEC_PAGE0_REG16_RIGHT_ADC_PGA_0DB 0x00u

#define CODEC_PAGE0_REG17_MIC3L_OFF_LEFT_ADC   0xF0u
#define CODEC_PAGE0_REG17_MIC3R_OFF_LEFT_ADC   0x0Fu

#define CODEC_PAGE0_REG18_MIC3L_OFF_RIGHT_ADC  0xF0u
#define CODEC_PAGE0_REG18_MIC3R_OFF_RIGHT_ADC  0x0Fu

#define CODEC_PAGE0_REG19_LINE1L_DIFF           0x80u
#define CODEC_PAGE0_REG19_LINE1L_OFF_LEFT_ADC   0x78u
#define CODEC_PAGE0_REG19_LEFT_ADC_POWERED_UP   0x04u
#define CODEC_PAGE0_REG19_LEFT_ADC_NO_SOFT_STEP 0x03u

#define CODEC_PAGE0_REG20_LINE2L_DIFF           0x80u
#define CODEC_PAGE0_REG20_LINE2L_OFF_LEFT_ADC   0x78u
#define CODEC_PAGE0_REG20_LEFT_ADC_BIASED       0x04u

#define CODEC_PAGE0_REG21_LINE1R_DIFF           0x80u
#define CODEC_PAGE0_REG21_LINE1R_OFF_LEFT_ADC   0x78u

#define CODEC_PAGE0_REG22_LINE1R_DIFF               0x80u
#define CODEC_PAGE0_REG22_LINE1R_OFF_RIGHT_ADC      0x78u
#define CODEC_PAGE0_REG22_RIGHT_ADC_POWERED_UP      0x04u
#define CODEC_PAGE0_REG22_RIGHT_ADC_NO_SOFT_STEP    0x03u

#define CODEC_PAGE0_REG23_LINE2R_DIFF           0x80u
#define CODEC_PAGE0_REG23_LINE2R_OFF_RIGHT_ADC  0x78u
#define CODEC_PAGE0_REG23_RIGHT_ADC_BIASED      0x04u

#define CODEC_PAGE0_REG24_LINE1L_DIFF           0x80u
#define CODEC_PAGE0_REG24_LINE1L_OFF_RIGHT_ADC  0x78u

#define CODEC_PAGE0_REG25_MICBIAS_2V            0x40u
#define CODEC_PAGE0_REG25_MICBIAS_2P5V          0x80u
#define CODEC_PAGE0_REG25_MICBIAS_VDD           0xC0u
#define CODEC_PAGE0_REG25_LEFT_DMIC_USED        0x10u

#define CODEC_PAGE0_REG36_ADC_FLAG_LEFT_PGA_PROGRAMMED  0x80u
#define CODEC_PAGE0_REG36_ADC_FLAG_LEFT_ADC_POWERED_UP  0x40u
#define CODEC_PAGE0_REG36_ADC_FLAG_RIGHT_PGA_PROGRAMMED 0x08u
#define CODEC_PAGE0_REG36_ADC_FLAG_RIGHT_ADC_POWERED_UP 0x04u

#define CODEC_PAGE0_REG37_LEFT_DAC_POWERED_UP       0x80u
#define CODEC_PAGE0_REG37_RIGHT_DAC_POWERED_UP      0x40u
#define CODEC_PAGE0_REG37_HPLCOM_SINGLE_ENDED       0x20u

#define CODEC_PAGE0_REG38_ENABLE_SHORT_CIRCUIT_PROTECTION 0x04u

#define CODEC_PAGE0_REG40_CM_VOLTAGE_1P5            0x40u
#define CODEC_PAGE0_REG40_CM_VOLTAGE_1P65           0x80u
#define CODEC_PAGE0_REG40_CM_VOLTAGE_1P8            0xC0u
#define CODEC_PAGE0_REG40_DISABLE_SOFT_STEP         0x02u

#define CODEC_PAGE0_REG41_LEFT_DAC_TO_LEFT_LINE_OUT     0x40u
#define CODEC_PAGE0_REG41_LEFT_DAC_TO_LEFT_HIGH_POWER   0x80u
#define CODEC_PAGE0_REG41_RIGHT_DAC_TO_RIGHT_LINE_OUT   0x10u
#define CODEC_PAGE0_REG41_RIGHT_DAC_TO_RIGHT_HIGH_POWER 0x20u

#define CODEC_PAGE0_REG43_LEFT_DAC_MUTE             0x80u
#define CODEC_PAGE0_REG43_LEFT_DAC_PGA_0DB          0x00u

#define CODEC_PAGE0_REG44_RIGHT_DAC_MUTE            0x80u
#define CODEC_PAGE0_REG44_RIGHT_DAC_PGA_0DB         0x00u

#define CODEC_PAGE0_REG47_DACL1_TO_HPLOUT           0x80u

#define CODEC_PAGE0_REG51_HPLOUT_POWERED_UP         0x01u
#define CODEC_PAGE0_REG51_HPLOUT_UNMUTE             0x08u
#define CODEC_PAGE0_REG51_9DB_OUTPUT_LEVEL          0x90u

#define CODEC_PAGE0_REG61_DACL1_TO_HPROUT           0x80u

#define CODEC_PAGE0_REG65_HPROUT_UNMUTE             0x08u
#define CODEC_PAGE0_REG65_HPROUT_POWERED_UP         0x01u

#define CODEC_PAGE0_REG78_DACR1_TO_MONOLO           0x80u

#define CODEC_PAGE0_REG79_MONOLO_UNMUTE             0x08u
#define CODEC_PAGE0_REG79_MONOLO_POWERED_UP         0x01u

#define CODEC_PAGE0_REG81_PGAL_ROUTED_LEFTLO        0x80u

#define CODEC_PAGE0_REG82_DACL1_TO_LEFTLO           0x80u

#define CODEC_PAGE0_REG86_LEFTLO_UNMUTE             0x08u
#define CODEC_PAGE0_REG86_7DB                       0x70u
#define CODEC_PAGE0_REG86_LEFTLO_GAINS_APPLIED      0x02u
#define CODEC_PAGE0_REG86_LEFTLO_POWERED_UP         0x01u

#define CODEC_PAGE0_REG89_DACL1_TO_RIGHTLO          0x80u

#define CODEC_PAGE0_REG93_RIGHTLO_UNMUTE            0x08u
#define CODEC_PAGE0_REG93_7DB                       0x70u
#define CODEC_PAGE0_REG93_RIGHTLO_POWERED_UP        0x01u

#define CODEC_PAGE0_REG94_LEFT_DAC_POWERED_UP       0x80u
#define CODEC_PAGE0_REG94_RIGHT_DAC_POWERED_UP      0x40u
#define CODEC_PAGE0_REG94_MONOLOP_DAC_POWERED_UP    0x20u
#define CODEC_PAGE0_REG94_LEFT_LOP_POWERED_UP       0x10u
#define CODEC_PAGE0_REG94_RIGHT_LOP_POWERED_UP      0x08u
#define CODEC_PAGE0_REG94_HPLOUT_POWERED_UP         0x04u
#define CODEC_PAGE0_REG94_HPROUT_POWERED_UP         0x02u

#define CODEC_PAGE0_REG95_HPLCOM_POWERED_UP         0x08u
#define CODEC_PAGE0_REG95_HPRCOM_POWERED_UP         0x04u

#define CODEC_PAGE0_REG98_GPIO1_DMIC_CLK            0xA0u
#define CODEC_PAGE0_REG98_GPIO1_HEADSET_DET_OUT     0xD0u
#define CODEC_PAGE0_REG98_GPIO1_CLK_MUX_DIVIDER		0x08

#define CODEC_PAGE0_REG99_GPIO2_DMIC_DATA           0x50u

#define CODEC_PAGE0_REG107_LEFT_DMIC_USED           0x10u
#define CODEC_PAGE0_REG107_ANALOG_MICS_USED         0x30u

#define CODEC_PAGE0_REG109_100_PERCENT_INCR         0xC0u

#define ADC_MUTE           0x80u
#define ADC_PLUS_42DB      84u
#define ADC_PLUS_30DB      60u
#define ADC_PLUS_19DB      38u
#define ADC_PLUS_18DB      36u
#define ADC_PLUS_17_5DB    35u
#define ADC_PLUS_15DB      30u
#define ADC_PLUS_14DB      28u
#define ADC_PLUS_13DB      26u
#define ADC_PLUS_12DB      24u
#define ADC_PLUS_11_5DB    23u
#define ADC_PLUS_9DB       18u
#define ADC_PLUS_7_5DB     15u
#define ADC_PLUS_6DB       12u
#define ADC_PLUS_5_5DB     11u
#define ADC_PLUS_0DB        0u

#define DAC_MUTE           0x80u //limit -78.3dB
#define DAC_MINUS_58_3DB   110u
#define DAC_MINUS_48_2DB   96u
#define DAC_MINUS_45_2DB   90u
#define DAC_MINUS_43_8DB   87u
#define DAC_MINUS_42_2DB   84u
#define DAC_MINUS_40_7DB   81u
#define DAC_MINUS_39_7DB   79u
#define DAC_MINUS_39_2DB   78u
#define DAC_MINUS_38_2DB   76u
#define DAC_MINUS_37_7DB   75u
#define DAC_MINUS_37_1DB   74u
#define DAC_MINUS_36_7DB   73u
#define DAC_MINUS_36_1DB   72u
#define DAC_MINUS_35_1DB   70u
#define DAC_MINUS_34_6DB   69u
#define DAC_MINUS_33_6DB   67u
#define DAC_MINUS_33_1DB   66u
#define DAC_MINUS_32_1DB   64u
#define DAC_MINUS_31_6DB   63u
#define DAC_MINUS_30_6DB   61u
#define DAC_MINUS_30_1DB   60u
#define DAC_MINUS_29_1DB   58u
#define DAC_MINUS_28_6DB   57u
#define DAC_MINUS_27_6DB   55u
#define DAC_MINUS_27_1DB   54u
#define DAC_MINUS_26_1DB   52u
#define DAC_MINUS_25_6DB   51u
#define DAC_MINUS_24_6DB   49u
#define DAC_MINUS_24_1DB   48u
#define DAC_MINUS_23_1DB   46u
#define DAC_MINUS_22_6DB   45u
#define DAC_MINUS_21_6DB   43u
#define DAC_MINUS_21_1DB   42u
#define DAC_MINUS_20_1DB   40u
#define DAC_MINUS_19_6DB   39u
#define DAC_MINUS_19_1DB   38u
#define DAC_MINUS_18_5DB   37u
#define DAC_MINUS_18DB     36u
#define DAC_MINUS_17DB     34u
#define DAC_MINUS_16_5DB   33u
#define DAC_MINUS_15DB     30u
#define DAC_MINUS_13_5DB   27u
#define DAC_MINUS_12DB     24u
#define DAC_MINUS_10_5DB   21u
#define DAC_MINUS_9DB      18u
#define DAC_MINUS_7_5DB    15u
#define DAC_MINUS_6DB      12u
#define DAC_MINUS_4_5DB    9u
#define DAC_MINUS_3DB      6u
#define DAC_MINUS_2DB      4u
#define DAC_MINUS_1_5DB    3u
#define DAC_MINUS_0DB      0u

#define CODEC_REG_READ_DEFAULT_MASK	0xFFu

#define NO_FILTER_COEF_N0_3     32768u
#define NO_FILTER_COEF_N1_4     0u
#define NO_FILTER_COEF_N2_5     0u
#define NO_FILTER_COEF_D1_4     0u
#define NO_FILTER_COEF_D2_5     0u

/****************/
/* File Enums   */
/****************/
typedef enum
{
    CODEC_FILTER_INVALID = -1,
    CODEC_FILTER_NONE = 0,
    CODEC_FILTER_TIA4953_FLAT = 1,
    CODEC_FILTER_TIA4953_SLIGHT = 2,
    CODEC_FILTER_TIA4953_STEEP_MILD = 3,
    CODEC_FILTER_TIA4953_STEEP_MODERTE = 4,
    CODEC_FILTER_TIA4953_STEEP_SEVERE = 5,
    CODEC_FILTER_TIA470 = 6
} CodecHardwareFilterChoices_t;

/****************/
/* File structs */
/****************/
typedef struct
{
    BOOL enable;
    BOOL UserOutput;
    U16 DEF_N0;
    U16 DEF_N1;
    U16 DEF_N2;
    U16 DEF_D1;
    U16 DEF_D2;
    U16 DEF_N3;
    U16 DEF_N4;
    U16 DEF_N5;
    U16 DEF_D4;
    U16 DEF_D5;
} DigitalEffectsFilter_t;

typedef struct
{
    BOOL enable;
    BOOL POTSInput;
    U8 Target;
    U8 AttackTime;
    U8 AttackTimeMultiplicationFactor;
    U8 DecayTime;
    U8 DecayTimeMultiplicationFactor;
    U8 NoiseGateThreshold;
    U8 MaxGainApplied;
    U8 Hysteresis;
    BOOL ClipStepping;
    U8 NoiseDebounce;
    U8 SignalDebounce;
} AIC3106AGCSettings_t;

/****************/
/* File Externs */
/****************/

/*******************************
 * Public function prototypes
 *******************************/
BOOL InitCodecAIC3106(void);
BOOL AIC3106_Codec_Set_Output_Filter(CodecHardwareFilterChoices_t which_filter);
BOOL AIC3106_Codec_Set_POTS_AGC(AIC3106AGCSettings_t *p_AGCSettings);
BOOL AIC3106_Codec_Enable_Ext_Spkr(void);
BOOL AIC3106_Codec_Enable_Ext_Mic(void);
BOOL AIC3106_Codec_Enable_Handset_Spkr(void);
BOOL AIC3106_Codec_Enable_Handset_Mic(void);
BOOL AIC3106_Codec_Set_User_ADC(U32 Gain);
BOOL AIC3106_Codec_Set_User_DAC(U32 Gain);
BOOL AIC3106_Codec_Set_POTS_ADC(U32 Gain);
BOOL AIC3106_Codec_Set_POTS_DAC(U32 Gain);
#endif 
