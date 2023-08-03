/*******************************************************************************/
/*  Copyright 2018 Ambarella Inc.                                              */
/*  Author: Joey Li                                                            */
/*  Email:  jli@ambarella.com                                                  */
/*******************************************************************************/

#ifndef CVAPI_AMBA_OD_INTERFACE_H_FILE
#define CVAPI_AMBA_OD_INTERFACE_H_FILE

#include "cvapi_ambanet_interface.h"

#include "cvapi_amba_mask_interface.h"

#define IONAME_AMBA_OD_KITTI            "AMBA_OD_STEP5_KITTI"

/*  version 3.0 specific */
#define TASKNAME_AMBA_OD_STEP1          "AMBA_OD_STEP1"
#define TASKNAME_AMBA_OD_STEP3          "AMBA_OD_STEP3"
#define TASKNAME_AMBA_OD_STEP4          "AMBA_OD_STEP4"
#define TASKNAME_AMBA_OD_STEP5          "AMBA_OD_STEP5"
#define TASKNAME_AMBA_OD_STEP6          "AMBA_OD_STEP6"
#define TASKNAME_AMBA_OD_STEP7          "AMBA_OD_STEP7"
#define TASKNAME_AMBA_OD_STEP8          "AMBA_OD_STEP8"
#define TASKNAME_AMBA_OD_3DBOX          "AMBA_OD_3DBOX"
#define IONAME_AMBA_OD_STEP1            "AMBA_OD_STEP1_OUTPUT"
#define IONAME_AMBA_OD_STEP3            "AMBA_OD_STEP3_OUTPUT"
#define IONAME_AMBA_OD_STEP4            "AMBA_OD_STEP4_OUTPUT"
#define IONAME_AMBA_OD_STEP6            "AMBA_OD_STEP6_OUTPUT"
#define IONAME_AMBA_OD_STEP8            "AMBA_OD_STEP8_OUTPUT"
/*  end of version 3.0  */

/*  version 3.5/4.0 specific */
#define TASKNAME_AMBA_ODV4_STEP1        "AMBA_ODV4_STEP1"
#define TASKNAME_AMBA_ODV3_STEP1        "AMBA_ODV3_STEP1"
#define TASKNAME_AMBA_ODV4_STEP5        "AMBA_ODV4_STEP5"
#define TASKNAME_AMBA_ODV4_3DBOX        "AMBA_ODV4_3DBOX"
#define IONAME_AMBA_ODv4_STEP1          "AMBA_ODv4_STEP1_OUTPUT"
#define TASKNAME_AMBA_SEGOD_STEP1       "AMBA_SEGOD_STEP1"
/*  end of version 3.5/4.0  */

/*  version 3.6 specific */
#define TASKNAME_AMBA_ODV36_STEP5       "AMBA_ODV36_STEP5"
/*  end of version 3.6 specific */

/*  version 3.7 specific */
#define TASKNAME_AMBA_ODV37_STEP1        "AMBA_ODV37_STEP1"
#define TASKNAME_AMBA_ODV37_STEP6        "AMBA_OD_STEP6"
#define IONAME_AMBA_ODv37_STEP1          "AMBA_ODv4_STEP1_OUTPUT"
#define IONAME_AMBA_ODv37_STEP6          "AMBA_OD_STEP6_OUTPUT"

/*  end of version 3.7 specific */

/*  version 3.7 specific */
#define TASKNAME_AMBA_TSC_STEP1          "AMBA_TSC_STEP1"
/*  end of version 3.7 specific */

#define AMBA_OD_OUTPUT_NAME             AMBANET_OUTPUT_NAME
#define AMBA_OD_OUTPUT_TYPE             0x6F626474 /* "obdt" */
#define AMBA_KP_OUTPUT_NAME             AMBANET_OUTPUT_NAME
#define AMBA_KP_OUTPUT_TYPE             0x6F646B70 /* "odkp" */
#define AMBA_3D_OUTPUT_NAME             AMBANET_OUTPUT_NAME
#define AMBA_3D_OUTPUT_TYPE             0x6F645164 /* "od3d" */
#define AMBA_IS_OUTPUT_NAME             AMBANET_OUTPUT_NAME
#define AMBA_IS_OUTPUT_TYPE             0x6F646973 /* "odis" */

/* Tracker */
#define TASKNAME_AMBA_OD_TRCKR          "AMBA_OD_TRACKER"
#define AMBA_TRACKER_PT_LABEL           "AMBA_TRACKER_PASS_THROUGH"

/* max number of ROIs */
#define MAX_NUM_ROIS                    3
#define AMBA_ODV4_MAX_NUM_BBOX          150     /* max number of output bbox */

/* categories */
#define MAX_NUM_CATEGORIES              128

/* message types */
#define AMBANET_MSG_TYPE_OD_STEP1       0x3000
#define AMBANET_MSG_TYPE_OD_NMS         0x3001
#define AMBANET_MSG_TYPE_OD_STEP5       0x3002
#define AMBANET_MSG_TYPE_OD_STEP6       0x3003
#define AMBANET_MSG_TYPE_TRACKER        0x3004
#define AMBANET_MSG_TYPE_OD_GROUPING    0x3005
#define AMBANET_MSG_TYPE_OD_ROIS        0x3006
#define AMBANET_MSG_TYPE_OD_WINDOWS     0x3007
#define AMBANET_MSG_TYPE_3D_CAMERA      0x3008
#define AMBANET_MSG_TYPE_OD_BENCHMARK   0x3009

#ifndef AMBANET_MSG_TYPE_LICENSE
#define AMBANET_MSG_TYPE_LICENSE        0xAB001000U
#endif

/* size limitation */
#define AMBANET_MAX_MASK_WIDTH          1920
#define AMBANET_MAX_MASK_HEIGHT         1080

#define AMBANET_KP_NUM                  17

/* to step 1 */
typedef struct {
    uint32_t msg_type;              /* AMBANET_MSG_TYPE_OD_STEP1          */
    uint32_t max_pre_nms_results_m1;/* max num of output objects          */
    uint32_t image_pyramid_index;   /* level of idsp image pyramid        */
    uint16_t roi_start_col;         /* start column of roi                */
    uint16_t roi_start_row;         /* start row of roi                   */
    uint16_t roi_width;             /* roi width                          */
    uint16_t roi_height;            /* roi height                         */
    uint16_t img_start_col;
    uint16_t img_start_row;
} amba_od_step1_config_t;

/* to step 3 & 5 */
typedef struct {
    uint32_t max_final_results_m1;  /* max output bbox                    */
    uint16_t iou_enable;            /* 0: disabled, 1: iou enabled        */
    uint16_t iom_enable;            /* 0: disabled, 1: iom enabled        */
    uint16_t iou_threshold;         /* default to 0.16                    */
    uint16_t iom_threshold;         /* default to 0.16                    */
    uint16_t bb_width_threshold;    /* bounding box minimum width         */
    uint16_t bb_height_threshold;   /* bounding box minimum height        */
    uint16_t nms_config_id;         /* id of this config set              */
    uint16_t score_threshold;       /* score threshold                    */
} amba_od_nms_config_t;

#define MAX_NUM_NMS_IN_CONFIGS          50
typedef struct {
    uint32_t msg_type;              /* AMBANET_MSG_TYPE_OD_NMS            */
    uint32_t num_configs;           /* number of configs                  */
    amba_od_nms_config_t            config[MAX_NUM_NMS_IN_CONFIGS];
} amba_od_nms_configs_t;

/* old amba_od_step5_config_t struct,  kept for backward compatibility        */
typedef struct {
    uint32_t msg_type;              /* AMBANET_MSG_TYPE_OD_STEP5          */
    int16_t  pyramid_offset_col[3]; /* global offset x of each image      */
    /* pyramid input in pre-hierarchical  */
    /* resampler space                    */
    int16_t  pyramid_offset_row[3]; /* global offset y of each image      */
    /* pyramid input in pre-hierarchical  */
    /* resampler space                    */
    uint16_t pyramid_ph_inc_col[3]; /* u8.8 phase increment x of each     */
    /* image pyramid input with respect   */
    /* to pre-hierarchical resampler      */
    /* space                              */
    uint16_t pyramid_ph_inc_row[3]; /* u8.8 phase increment y of each     */
    /* image pyramid input with respect   */
    /* to pre-hierarchical resampler      */
    /* space                              */
    uint16_t roi_start_col[3];      /* start x of each roi within its     */
    /* image pyramid input                */
    uint16_t roi_start_row[3];      /* start y of each roi within its     */
    /* image pyramid input                */
    uint8_t  roi_score_scale[3];    /* u1.7 score scale of each roi,      */
    /* multiplied to each score           */
    uint8_t  padding;               /* pad to word (11 words, 44 bytes)   */
    uint16_t frame_width;
    uint16_t frame_height;
} amba_od_step5_config_t;
/* end of old amba_od_step5_config_t struct                                   */

/* to step 5/6/7/8, 3D converter */
typedef struct {
    uint32_t msg_type;              /* AMBANET_MSG_TYPE_OD_ROIS           */
    uint16_t frame_width;           /* full image width                   */
    uint16_t frame_height;          /* full image height                  */

    uint16_t pyramid_index[3];      /* pyramid scale index                */
    int16_t  pyramid_offset_col[3]; /* offset of the pyramid image from   */
    int16_t  pyramid_offset_row[3]; /*     the full image                 */
    uint16_t pyramid_ratio_col[3];  /* scale factor of the full image to  */
    uint16_t pyramid_ratio_row[3];  /*     pyramid image, lshifted by 8   */

    uint16_t roi_start_col[3];      /* placement of ROI in pyramid image, */
    uint16_t roi_start_row[3];      /*     in pyramid image coordinates   */
    uint16_t roi_width[3];
    uint16_t roi_height[3];
    uint8_t  roi_total;             /* number of ROIs                     */
    uint8_t  roi_index;             /* roi index[0/1/2] to be used        */
} amba_od_rois_config_t;

/* to step 5 */
typedef struct {
    uint32_t msg_type;              /* AMBANET_MSG_TYPE_OD_GROUPING       */
    uint32_t group_id[MAX_NUM_CATEGORIES];
} amba_od_grouping_t;

/* to tracker */
typedef struct {
    uint32_t msg_type;
    uint32_t fps;
    uint32_t frame_width;
    uint32_t frame_height;
    uint32_t score_threshold;
    uint32_t test;
} amba_od_tracker_config_t;

/* to od_3dbox */
typedef struct {
    uint32_t msg_type;              /* AMBANET_MSG_TYPE_3D_CAMERA         */
    uint32_t camera_id;             /* ID of the camera                   */
    FLOAT    intrinsic[9];          /* intrinsic 3x3 matrix               */
    FLOAT    extrinsic_R[3];        /* orientation: yaw, pitch, roll      */
    FLOAT    extrinsic_T[3];        /* position: x, y, z                  */
} amba_3d_camera_config_t;

/* AmbaOD output data structure */
typedef struct {
    uint32_t type;                  /* AMBA_OD_OUTPUT_TYPE                */
    uint32_t length;                /* sizeof(amba_od_out_t)              */
    uint32_t num_objects;           /* size of object list                */
    rptr32_t objects_offset;        /* relative pointer to object list    */
    uint32_t num_proposals;         /* size of proposal list              */
    rptr32_t proposals_offset;      /* relative pointer to proposal list  */
    uint32_t frame_num;             /* frame_num of the current frame     */
    uint32_t capture_time;          /* capture time of the frame          */
    uint32_t errorcode;             /* Error code issued by the task.     */
    ptr32_t  class_name;            /* phy addr of class-name list        */
    uint16_t camera_id;             /* camera ID                          */
#define AMBAOD_TYPE_GENERAL     0
#define AMBAOD_TYPE_BARRIER     1
#define AMBAOD_TYPE_POLE        2
    uint16_t network_type;          /* network type                       */
    uint8_t  major_version[32];     /* version string of ambaod           */
    uint8_t  minor_version[32];     /* version string of classifier       */
    ptr64_t  class_name_p64;        /* class-name addr, for RTOS only     */
    uint8_t  padding[128-116];      /* padding to 128 bytes               */
} amba_od_out_t;
#define AC_CVAPI_CNN_OBJECT_T_VERSION           (1U)    // This shall be updated also when data pointed to are changed
#define AC_CVAPI_CNN_OBJECT_T_VER_BACK_0        (0U)    // Back compatibility version

typedef struct {
    uint32_t score;                 /* confidence score                   */
    uint32_t field;                 /* field ID, assigned by step1        */
    uint32_t class;                 /* class ID, assigned by step4        */
    uint32_t track;                 /* track ID, assigned by tracker      */
    int32_t  bb_start_row;
    int32_t  bb_start_col;
    uint32_t bb_height_m1;
    uint32_t bb_width_m1;
} amba_od_candidate_t;

/* AmbaOD KeyPoint-RCNN output data structure */
typedef struct {
    uint32_t type;                  /* AMBA_KP_OUTPUT_TYPE                */
    uint32_t length;                /* sizeof(amba_kp_out_t)              */
    uint32_t num_objects;           /* size of object list                */
    rptr32_t objects_offset;        /* relative pointer to object list    */
    uint32_t frame_num;             /* frame_num of the current frame     */
    uint32_t capture_time;          /* capture time of the frame          */
    uint32_t errorcode;             /* Error code issued by the task.     */
    uint32_t ambaod_out_address;    /* address of amba_od_out             */
    uint8_t  padding[96];
} amba_kp_out_t;

typedef struct {
    uint16_t coord[96];             /* ALIGN_32(AMBANET_KP_NUM) * 3       */
    uint32_t bbox_id;               /* index of corresponding step5 bbox  */
} amba_kp_candidate_t;

/* AmbaOD 3D bbox output data structure */
typedef struct {
    uint32_t type;                  /* AMBA_3D_OUTPUT_TYPE                */
    uint32_t length;                /* sizeof(amba_3d_out_t)              */
    uint32_t num_objects;           /* size of object list                */
    rptr32_t objects_offset;        /* relative pointer to object list    */
    uint32_t frame_num;             /* frame_num of the current frame     */
    uint32_t capture_time;          /* capture time of the frame          */
    uint32_t errorcode;             /* Error code issued by the task.     */
    ptr32_t  class_name;            /* phy addr of class-name list        */
    uint16_t camera_id;             /* camera ID                          */
    uint16_t padding;               /* padding                            */
} amba_3d_out_t;
#define AC_CVAPI_CNN_OBJECT_3D_T_VERSION        (1U)    // This shall be updated also when data pointed to are changed
#define AC_CVAPI_CNN_OBJECT_3D_T_VER_BACK_0     (0U)    // Previous version

typedef struct {
    amba_od_candidate_t od_info;    /* 2d bbox info from step5            */
    uint32_t pixel_corners[8][2];
    uint32_t orientation_2D[2][2];
    FLOAT    distance;              /* distance from camera               */
    FLOAT    theta;
    FLOAT    center[3];
    FLOAT    corners[8][3];
    FLOAT    loc_3d[8][3];
    FLOAT    orientation_3D[2][3];
} amba_3d_candidate_t;

typedef struct {
    uint32_t dim_shift;             /* shift value for dimention output   */
    uint32_t oren_shift;            /* shift value for orentation output  */
    uint32_t num_objects;           /* size of object list                */
    rptr32_t objects_offset;        /* relative pointer to object list    */
    uint32_t roi_index;             /* roi index                          */
} amba_od_step8_out_t;

typedef struct {
    int16_t dimension[32];          /* bbox dimension   */
    int16_t confidence[32];         /* bbox confidence  */
    int16_t orientation[32];        /* bbox orientation */
} amba_od_step8_candidate_t;

typedef struct {
    uint32_t type;                  /* AMBA_IS_OUTPUT_TYPE               */
    uint32_t length;                /* sizeof(amba_is_out_t)             */
    uint32_t num_objects;
    uint32_t list_offset_width;     /* list address of mask width        */
    uint32_t list_offset_height;    /* list address of mask height       */
    uint32_t list_offset_address;   /* list address of mask buffer addr  */
    uint32_t ambaod_out_address;    /* address of amba_od_out            */
    uint16_t camera_id;
    uint8_t  padding[98];           /* pad to 128-byte aligned           */
} amba_is_out_t;
#define AC_CVAPI_CNN_INST_T_VERSION             (1U)
#define AC_CVAPI_CNN_INST_T_VER_BACK_0          (0U)

/*******************************************************************************
 *                               INPUT BUFFER
 * Step1: one input
 *     nameL IDSP_RX
 *
 *******************************************************************************
 *                              OUTPUT BUFFER
 * Step5 & Tracker: one output
 *     name: AMBA_OD_OUTPUT_NAME
 *   struct: amba_od_out_t
 *
 * Step6: one output
 *     name: AMBA_MASK_OUTPUT_NAME
 *   struct: amba_mask_out_t
 *
 *******************************************************************************
 *                              CVTABLE LABELS
 * Tracker:
 * AMBA_TRACKER_PT_LABEL: set tracker as pass-through, default is ON.
 *
 *******************************************************************************
 *                       SYSFLOW CONFIGURATION SYNTAX
 *
 * For step5:
 * 0: number of ROIs
 *       input[2*N+0]: output from step3 of ROI N
 *       input[2*N+1]: output from step4 of ROI N
 *
 ******************************************************************************/

#endif  /* CVAPI_AMBA_OD_INTERFACE_H_FILE */
