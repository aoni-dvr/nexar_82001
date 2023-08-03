/*******************************************************************************/
/*  Copyright 2019 Ambarella Inc.                                              */
/*******************************************************************************/

#ifndef CVAPI_AMBA_SEG39_FC_CATEGORY_H_FILE
#define CVAPI_AMBA_SEG39_FC_CATEGORY_H_FILE

/* Output - 0 */
#define AMBANET_SEG39_FC_CAT0_BACKGROUND                (0U)
#define AMBANET_SEG39_FC_CAT1_ROAD                      (1U)
#define AMBANET_SEG39_FC_CAT2_CURB                      (2U)
#define AMBANET_SEG39_FC_CAT3_LANE_MARKING              (3U)
#define AMBANET_SEG39_FC_NUM_OF_CATEGORIES              4

/* Output - 1 */
#define AMBANET_SEG39_FC_COLOR0_BACKGROUND              (0U)
#define AMBANET_SEG39_FC_COLOR1_WHITE                   (1U)
#define AMBANET_SEG39_FC_COLOR2_YELLOW                  (2U)
#define AMBANET_SEG39_FC_COLOR3_RED                     (3U)
#define AMBANET_SEG39_FC_COLOR4_BLUE                    (4U)
#define AMBANET_SEG39_FC_NUM_OF_COLORS                  5

/* Output - 2 */
#define AMBANET_SEG39_FC_TYPE0_BACKGROUND               (0U)
#define AMBANET_SEG39_FC_TYPE1_SOLID                    (1U)
#define AMBANET_SEG39_FC_TYPE2_DASH                     (2U)
#define AMBANET_SEG39_FC_TYPE3_DOUBLESOLID              (3U) /* includes solid-dashed and dashed-solid */
#define AMBANET_SEG39_FC_TYPE4_DASHDOUBLE               (4U)
#define AMBANET_SEG39_FC_NUM_OF_TYPES                   5

/* Output - 3 */
#define AMBANET_SEG39_FC_INSTANCE0_BACKGROUND           (0U)
#define AMBANET_SEG39_FC_INSTANCE1_DRIVINGLEFT          (1U)
#define AMBANET_SEG39_FC_INSTANCE2_DRIVINGRIGHT         (2U)
#define AMBANET_SEG39_FC_INSTANCE3_ADJACENTLEFT         (3U)
#define AMBANET_SEG39_FC_INSTANCE4_ADJACENTRIGHT        (4U)
#define AMBANET_SEG39_FC_INSTANCE5_CENTER               (5U)
#define AMBANET_SEG39_FC_NUM_OF_INSTANCES               6

#endif  /* CVAPI_AMBA_SEG39_FC_CATEGORY_H_FILE */
