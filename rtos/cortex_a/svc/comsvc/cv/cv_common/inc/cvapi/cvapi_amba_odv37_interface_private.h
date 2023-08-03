/*******************************************************************************/
/*  Copyright 2018 Ambarella Inc.                                              */
/*  Author: Shengxuan Sun                                                      */
/*  Email:  sxsun@ambarella.com                                                */
/*******************************************************************************/

#ifndef CVAPI_AMBA_ODV37_INTERFACE_INTERNAL_H_FILE
#define CVAPI_AMBA_ODV37_INTERFACE_INTERNAL_H_FILE

#include "cvapi_ambaod.h"
#include "cvapi_amba_odv37_interface.h"
#include "cvapi_ambanet_interface.h"

#define IONAME_AMBA_ODV37_STEP1         "AMBA_ODV37_STEP1_OUTPUT"
#define IONAME_AMBA_ODV37_STEP8         "AMBA_ODV37_STEP8_OUTPUT"

#define AMBA_ODV37_MAX_NUM_PPSL         (65536) /* max number of input proposals */
#define AMBA_ODV37_NUM_SCALES           (5)
#define AMBA_ODV37_FEATURE_MAP_SCALES   (7)

#if (defined(__linux__) || defined(ORCVIS))
#define ALIGN_32_BYTE(x)                ((((x) + 31) >> 5) << 5)
#endif
#define INVALID_ROI_INDEX               (255)

typedef struct {
    uint32_t  frame_num;
    uint32_t  capture_time;
    /* the ROI index from HL cvtask */
    int32_t   roi_index;
    uint32_t  coord_frac_bits;
    /* number of bbox for each scale */
    uint32_t  proposal_count[AMBA_ODV37_NUM_SCALES];
    uint32_t  proposal_offset_shift[AMBA_ODV37_NUM_SCALES];
    /* each offset points to a list of amba_odv4_step1_candidate_t */
    rptr32_t  proposal_offset[AMBA_ODV37_NUM_SCALES];
    rptr32_t  feature_scale[AMBA_ODV37_FEATURE_MAP_SCALES];
    uint32_t  channel_id;
} amba_odv37_step1_out_t;

typedef struct {
    uint16_t  score;
    uint16_t  class;
    uint16_t  lt_x;
    uint16_t  lt_y;
    uint16_t  br_x;
    uint16_t  br_y;
    uint16_t  padding[10];
}  amba_odv37_step1_candidate_t;

typedef struct {
    uint32_t dim_shift;             /* shift value for dimention output   */
    uint32_t oren_shift;            /* shift value for orentation output  */
    uint32_t center_offset_shift;   /* shift value for center_offst output  */
    uint32_t uncertainty_shift;     /* shift value for uncertainty output  */
    uint32_t depth_shift;           /* shift value for depth output  */
    uint32_t num_objects;           /* size of object list                */
    rptr32_t objects_offset;        /* relative pointer to object list    */
    uint32_t roi_index;             /* roi index                          */
} amba_odv37_step8_out_t;

typedef struct {
    int16_t dimension[32];          /* bbox dimension   */
    int16_t confidence[32];         /* bbox confidence  */
    int16_t orientation[32];        /* bbox orientation */
    int16_t center_offset[32];      /* kp offset */
    uint16_t depth[32];              /* bbox depth */
    int16_t uncertainty[32];        /* bbox uncertainty */
} amba_odv37_step8_candidate_t;

/* config message format */
#define AMBANET_MSG_TYPE_MASK           0x1000
#define AMBANET_MSG_VRSN_MASK           0x0000
typedef struct {
    uint16_t msg_type;              /* AMBA_MASK_CONFIG_MSG_TYPE          */
    uint16_t version;               /* version number of this message     */
    uint32_t msg_length;            /* total length of the message        */
    uint16_t mask_width;            /* mask width                         */
    uint16_t mask_height;           /* mask height                        */
} amba_od_mask_config_t;

#define AMBANET_MSG_TYPE_OD_STEP1       0x3000
#define AMBANET_MSG_VRSN_OD_STEP1       0x0000
typedef struct {
    uint16_t msg_type;              /* AMBANET_MSG_TYPE_OD_STEP1          */
    uint16_t version;               /* version number of this message     */
    uint32_t max_pre_nms_results_m1;/* max num of output objects          */
    uint32_t image_pyramid_index;   /* level of idsp image pyramid        */
    uint16_t roi_start_col;         /* start column of roi                */
    uint16_t roi_start_row;         /* start row of roi                   */
    uint16_t roi_width;             /* roi width                          */
    uint16_t roi_height;            /* roi height                         */
    uint16_t img_start_col;
    uint16_t img_start_row;
    uint8_t  reserved[64-28];       /* align to 64-bytes                  */
} amba_od_step1_config_t;

typedef struct {
    uint32_t max_final_results_m1;  /* max output bbox                    */
    uint8_t  iou_enable;            /* 0: disabled, 1: iou enabled        */
    uint8_t  iom_enable;            /* 0: disabled, 1: iom enabled        */
    uint8_t  score_count;           /* number of score threshold: 1 or 4  */
    uint8_t  padding;
    uint16_t iou_threshold;         /* default to 0.16                    */
    uint16_t iom_threshold;         /* default to 0.16                    */
    uint16_t bb_width_threshold;    /* bounding box minimum width         */
    uint16_t bb_height_threshold;   /* bounding box minimum height        */
    uint16_t nms_config_id;         /* id of this config set              */
    uint16_t score_threshold[4];    /* score thresholds                   */
} amba_od_nms_config_t;

#define AMBANET_MSG_TYPE_OD_NMS         0x3001
#define AMBANET_MSG_VRSN_OD_NMS         0x0000
typedef struct {
#define MAX_NUM_NMS_IN_CONFIGS          50
    uint16_t msg_type;              /* AMBANE_MSG_TYPE_OD_NMS             */
    uint16_t version;               /* version number of this message     */
    uint32_t num_configs;           /* number of configs                  */
    amba_od_nms_config_t            config[MAX_NUM_NMS_IN_CONFIGS];
} amba_od_nms_configs_t;

#define AMBANET_MSG_TYPE_OD_GROUPING    0x3005
#define AMBANET_MSG_VRSN_OD_GROUPING    0x0000
typedef struct {
    uint16_t msg_type;              /* AMBANET_MSG_TYPE_OD_GROUPING       */
    uint16_t version;               /* version number of this message     */
    uint32_t group_id[128];
} amba_od_grouping_t;

#define AMBANET_MSG_TYPE_3D_CAMERA      0x3008
#define AMBANET_MSG_VRSN_3D_CAMERA      0x0001
typedef struct {
    uint16_t msg_type;              /* AMBANET_MSG_TYPE_3D_CAMERA         */
    uint16_t version;               /* version number of this message     */
    uint16_t minDist;               /* Minimum distance to output 3D bbox */
    uint16_t maxDist;               /* Maximum distance to output 3D bbox */
    uint32_t camera_id;             /* ID of the camera                   */
    FLOAT    intrinsic[9];          /* intrinsic 3x3 matrix               */
    FLOAT    extrinsic_R[3];        /* orientation: yaw, pitch, roll      */
    FLOAT    extrinsic_T[3];        /* position: x, y, z                  */
} amba_od_camera_config_t;

#endif  /* CVAPI_AMBA_ODV37_INTERFACE_INTERNAL_H_FILE */
