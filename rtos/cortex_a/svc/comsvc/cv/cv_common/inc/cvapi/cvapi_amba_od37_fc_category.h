/*******************************************************************************/
/*  Copyright 2019 Ambarella Inc.                                              */
/*******************************************************************************/

#ifndef CVAPI_AMBA_OD37_FC_CATEGORY_H_FILE
#define CVAPI_AMBA_OD37_FC_CATEGORY_H_FILE

#define AMBANET_OD37_FC_NUM_OF_CATEGORIES       16
#define AMBANET_OD37_FC_CAT0_PERSON             (0U)
#define AMBANET_OD37_FC_CAT1_RIDER              (1U)
#define AMBANET_OD37_FC_CAT2_BICYCLE            (2U)
#define AMBANET_OD37_FC_CAT3_MOTORCYCLE         (3U)
#define AMBANET_OD37_FC_CAT4_CAR                (4U)
#define AMBANET_OD37_FC_CAT5_TRUCK              (5U)
#define AMBANET_OD37_FC_CAT6_RED                (6U)
#define AMBANET_OD37_FC_CAT7_GREEN              (7U)
#define AMBANET_OD37_FC_CAT8_SIGN               (8U)
#define AMBANET_OD37_FC_CAT9_OTHER              (9U)
#define AMBANET_OD37_FC_CAT10_ARROW             (10U)
#define AMBANET_OD37_FC_CAT11_SPEEDBUMP         (11U)
#define AMBANET_OD37_FC_CAT12_CROSSWALK         (12U)
#define AMBANET_OD37_FC_CAT13_STOPLINE          (13U)
#define AMBANET_OD37_FC_CAT14_YIELDLINE         (14U)
#define AMBANET_OD37_FC_CAT15_YIELDMARK         (15U)

/* TrafficSign Classifier v3.0.0 */
#define AMBANET_TSC3_NUM_OF_CATEGORIES          9
#define AMBANET_TSC3_CAT0_SPEEDLIMIT            (0U)
#define AMBANET_TSC3_CAT1_STOP                  (1U)
#define AMBANET_TSC3_CAT2_YIELD                 (2U)
#define AMBANET_TSC3_CAT3_NOTURNS               (3U)
#define AMBANET_TSC3_CAT4_EXCLUSIONARY          (4U)
#define AMBANET_TSC3_CAT5_CONSTRUCTION          (5U)
#define AMBANET_TSC3_CAT6_BACKSIDE              (6U)
#define AMBANET_TSC3_CAT7_OTHER                 (7U)
#define AMBANET_TSC3_CAT8_BACKGROUND            (8U)

/* SpeedLimit Classifier v3.0.0 */
#define AMBANET_SL1_NUM_OF_CATEGORIES           19
#define AMBANET_SL1_CAT0_5                      (0U)
#define AMBANET_SL1_CAT1_10                     (1U)
#define AMBANET_SL1_CAT2_15                     (2U)
#define AMBANET_SL1_CAT3_20                     (3U)
#define AMBANET_SL1_CAT4_25                     (4U)
#define AMBANET_SL1_CAT5_30                     (5U)
#define AMBANET_SL1_CAT6_35                     (6U)
#define AMBANET_SL1_CAT7_40                     (7U)
#define AMBANET_SL1_CAT8_45                     (8U)
#define AMBANET_SL1_CAT9_50                     (9U)
#define AMBANET_SL1_CAT10_55                    (10U)
#define AMBANET_SL1_CAT11_60                    (11U)
#define AMBANET_SL1_CAT12_65                    (12U)
#define AMBANET_SL1_CAT13_70                    (13U)
#define AMBANET_SL1_CAT14_80                    (14U)
#define AMBANET_SL1_CAT15_90                    (15U)
#define AMBANET_SL1_CAT16_100                   (16U)
#define AMBANET_SL1_CAT17_110                   (17U)
#define AMBANET_SL1_CAT18_120                   (18U)


/* TrafficLight Color Classifier v1.1.0 */
#define AMBANET_TLC11_NUM_OF_CATEGORIES         4
#define AMBANET_TLC11_CAT0_GREEN                (0U)
#define AMBANET_TLC11_CAT1_RED                  (1U)
#define AMBANET_TLC11_CAT2_OTHER                (2U)
#define AMBANET_TLC11_CAT3_BACKGROUND           (3U)


/* TrafficLight Symbol Classifier v1.0.0 */
#define AMBANET_TLS1_NUM_OF_CATEGORIES          12
#define AMBANET_TLS1_CAT0_CIRCLE                (0U)
#define AMBANET_TLS1_CAT1_LEFT                  (1U)
#define AMBANET_TLS1_CAT2_RIGHT                 (2U)
#define AMBANET_TLS1_CAT3_UP                    (3U)
#define AMBANET_TLS1_CAT4_DOWN                  (4U)
#define AMBANET_TLS1_CAT5_UPRIGHT               (5U)
#define AMBANET_TLS1_CAT6_NORTHEAST             (6U)
#define AMBANET_TLS1_CAT7_NORTHWEST             (7U)
#define AMBANET_TLS1_CAT8_PEDESTRIAN            (8U)
#define AMBANET_TLS1_CAT9_BICYCLE               (9U)
#define AMBANET_TLS1_CAT10_OTHER                (10U)
#define AMBANET_TLS1_CAT11_BACKGROUND           (11U)


/* Arrow Classifier v1.0.0 */
#define AMBANET_AR1_NUM_OF_CATEGORIES           9
#define AMBANET_AR1_CAT0_STRAIGHT               (0U)
#define AMBANET_AR1_CAT1_LEFT                   (1U)
#define AMBANET_AR1_CAT2_RIGHT                  (2U)
#define AMBANET_AR1_CAT3_STRAIGHTLEFT           (3U)
#define AMBANET_AR1_CAT4_STRAIGHTRIGHT          (4U)
#define AMBANET_AR1_CAT5_LEFTRIGHT              (5U)
#define AMBANET_AR1_CAT6_STRAIGHTLEFTRIGHT      (6U)
#define AMBANET_AR1_CAT7_OTHER                  (7U)
#define AMBANET_AR1_CAT8_BACKGROUND             (8U)


#endif  /* CVAPI_AMBA_OD37_FC_CATEGORY_H_FILE */
