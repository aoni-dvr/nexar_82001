/*******************************************************************************/
/*  Copyright 2020 Ambarella Inc.                                              */
/*  Author: Joey Li                                                            */
/*  Email:  jli@ambarella.com                                                  */
/*******************************************************************************/

#ifndef CVAPI_AMBAOD_H_FILE
#define CVAPI_AMBAOD_H_FILE

#include "ambint.h"
#define FLOAT float

/******************************************************************************/
/*                      data structure for detections                         */ 
/******************************************************************************/
/* 2D Bounding Box */
#define AMBA_OD_OUTPUT_NAME             "AMBANET_OUTPUT"
#define AMBA_OD_OUTPUT_TYPE             0x6F626474 /* "obdt" */
#define AMBA_OD_OUTPUT_VERSION          0x0104
typedef struct {
        uint32_t type;                  /* AMBA_OD_OUTPUT_TYPE                */
        uint32_t version;               /* version of this data struct        */
        uint32_t num_objects;           /* size of object list                */
        rptr32_t objects_offset;        /* relative pointer to object list    */
        uint8_t  score_frac_bits;       /* quantization level of bbox score   */
        uint8_t  reserved[7];           /* reserved, should be zero           */
        uint32_t frame_num;             /* frame_num of the current frame     */
        uint32_t capture_time;          /* capture time of the frame          */
        uint32_t errorcode;             /* Error code issued by the task.     */
        ptr32_t  class_name;            /* phy addr of class-name list        */
        uint16_t camera_id;             /* camera ID                          */
#define AMBAOD_TYPE_GENERAL             0
#define AMBAOD_TYPE_BARRIER             1
#define AMBAOD_TYPE_POLE                2
#define AMBAOD_TYPE_SR_ADAS             3
#define AMBAOD_TYPE_BARRIER_AND_POLE    4
        uint16_t network_type;          /* network type                       */
        uint8_t  major_version[32];     /* version string of ambaod           */
        uint32_t minor_version_addr;    /* version string of classifier       */
        uint32_t reserved1[10];         /* reserved, should be zero           */
        ptr64_t  class_name_p64;        /* class-name addr, for RTOS only     */
} amba_od_out_t;

typedef struct {
        uint32_t score;                 /* confidence, [0, 2^score_frac_bit)  */
        uint8_t  field;                 /* field ID                           */
        uint8_t  reserved[3];
        uint32_t clsId;                 /* class ID                           */
        uint32_t track;                 /* track ID, assigned by tracker      */
        int32_t  bb_start_row;
        int32_t  bb_start_col;
        uint32_t bb_height_m1;
        uint32_t bb_width_m1;
} amba_od_candidate_t;

/* KeyPoint */
#define AMBA_KP_OUTPUT_NAME             "AMBANET_OUTPUT"
#define AMBA_KP_OUTPUT_TYPE             0x6F646B70 /* "odkp" */
#define AMBA_KP_OUTPUT_VERSION          0x0100
typedef struct {
        uint32_t type;                  /* AMBA_KP_OUTPUT_TYPE                */
        uint32_t version;               /* version of this data struct        */
        uint32_t num_objects;           /* size of object list                */
        rptr32_t objects_offset;        /* relative pointer to object list    */
        uint32_t frame_num;             /* frame_num of the current frame     */
        uint32_t capture_time;          /* capture time of the frame          */
        uint32_t errorcode;             /* Error code issued by the task.     */
        ptr32_t  ambaod_out_address;    /* address of amba_od_out             */
        uint16_t camera_id;             /* camera ID                          */
        uint16_t reserved;              /* reserved                           */
        ptr64_t  ambaod_out_p64;        /* amba_od_out addr, for RTOS only    */
        uint8_t  padding[128-44];       /* padding to 128 bytes               */
} amba_kp_out_t;

typedef struct {
#define AMBANET_KP_NUM                  17
        uint16_t coord[96];             /* ALIGN_32(AMBANET_KP_NUM) * 3       */
        uint32_t bbox_id;               /* index of corresponding step5 bbox  */
} amba_kp_candidate_t;

/* 3D bounding box */
#define AMBA_3D_OUTPUT_NAME             "AMBANET_OUTPUT"
#define AMBA_3D_OUTPUT_TYPE             0x6F645164 /* "od3d" */
#define AMBA_3D_OUTPUT_VERSION          0x0101
typedef struct {
        uint32_t type;                  /* AMBA_3D_OUTPUT_TYPE                */
        uint32_t version;               /* version of this data struct        */
        uint32_t num_objects;           /* size of object list                */
        rptr32_t objects_offset;        /* relative pointer to object list    */
        uint32_t frame_num;             /* frame_num of the current frame     */
        uint32_t capture_time;          /* capture time of the frame          */
        uint32_t errorcode;             /* Error code issued by the task.     */
        ptr32_t  class_name;            /* phy addr of class-name list        */
        uint16_t camera_id;             /* camera ID                          */
        uint16_t score_frac_bits;       /* quantization level of bbox score   */
        ptr64_t  ambaod_out_p64;        /* amba_od_out addr, for RTOS only    */
        uint8_t  padding[128-44];       /* padding to 128 bytes               */
} amba_3d_out_t;

typedef struct {
        amba_od_candidate_t od_info;    /* 2d bbox info from step5            */
        uint32_t pixel_corners[8][2];
        uint32_t orientation_2D[2][2];
        float    distance;              /* distance from camera               */
        FLOAT    theta;
        FLOAT    center[3];
        FLOAT    corners[8][3];
        FLOAT    loc_3d[8][3];
        FLOAT    orientation_3D[2][3];
} amba_3d_candidate_t;

/* Instance Segmentation */
#define AMBA_IS_OUTPUT_NAME             "AMBANET_OUTPUT"
#define AMBA_IS_SCORE_OUTPUT_NAME       "AMBANET_IS_SCORE_OUTPUT"
#define AMBA_IS_OUTPUT_TYPE             0x6F646973 /* "odis" */
#define AMBA_IS_OUTPUT_VERSION          0x0101
typedef struct {
        uint32_t type;                  /* AMBA_IS_OUTPUT_TYPE                */
        uint32_t version;               /* version of this data struct        */
        uint32_t num_objects;
        uint32_t list_offset_width;     /* list address of mask width         */
        uint32_t list_offset_height;    /* list address of mask height        */
        rptr32_t list_offset_address;   /* list address of mask buffer addr   */
        ptr32_t  ambaod_out_address;    /* address of amba_od_out             */
        uint16_t camera_id;             /* camera ID                          */
        uint16_t network_type;          /* network type                       */
        ptr64_t  ambaod_out_p64;        /* amba_od_out addr, for RTOS only    */
        uint8_t  padding[128-40];       /* padding to 128 bytes               */
} amba_is_out_t;

/* Tracker VP part */
#define AMBA_TR_OUTPUT_NAME            "AMBA_ODV37_STEP9_OUTPUT"
#define AMBA_TR_OUTPUT_TYPE             0x6F647472 /* "odtr" */
#define AMBA_TR_OUTPUT_VERSION          0x0100
typedef struct {
        uint32_t type;                  /* AMBA_TR_OUTPUT_TYPE              */
        uint32_t version;               /* version of this data struct      */
        uint32_t fm_shift;              /* shift value for dimention output */
        uint32_t num_objects;           /* size of object list              */
        rptr32_t objects_offset;        /* relative pointer to object list  */
        uint32_t roi_index;             /* roi index                        */
} amba_odv37_step9_out_t;

/******************************************************************************/
/*                  data structure for configure messages                     */ 
/******************************************************************************/
/* 
 * This data structure is for a list of messgae putting together sequentially
 * For a consumer to access a message list pointed by "base":
 *
 *      ambaod_cfg_msglist_t *pMsglist = (ambaod_cfg_msglist_t*) base;
 *      for (int i = 0; i < pMsglist->num_msgs; i++) {
 *              uint8_t *pMsg = base + pMsgList->offset[i];
 *              // process message pointed by @pMsg
 *      }
 *
 */
#define AMBAOD_CFG_MSGLIST_TYPE         0x1000
#define AMBAOD_CFG_MSGLIST_VER          0x0001
typedef struct {
        uint16_t type;          // AMBAOD_CFG_MSGLIST_TYPE
        uint16_t version;       // AMBAOD_CFG_MSGLIST_VER
        uint32_t reserved0[4];  // reserved, must be zero
        uint16_t length;        // total length of ENTIRE congregated message
        uint16_t num_msgs;      // number of messages
        uint8_t  reserved1[4];  // reserved, must be zero
        uint32_t offset[24];    // offset of each message pointer, max 24 mesgs
} ambaod_cfg_msglist_t;


#define AMBAOD_CFG_ROI_TYPE             0x1001
#define AMBAOD_CFG_ROI_VER              0x0001
typedef struct {
        /* type, version, target_frame, common for all message */
        uint16_t type;          // AMBAOD_CFG_ROI_TYPE
        uint16_t version;       // AMBAOD_CFG_ROI_VER
        uint32_t target_frame;  // target frame in where the msg takes effect

        uint8_t  roi_id;        // the ID of the ROI
        uint8_t  pyramid_scale; // pyramid scale of the ROI 
        uint8_t  reserved0[2];  // reserved, must be zero

        /* info aobut the input ROI */
        uint16_t rx,ry,rw,rh;   // topleft corner, widht, heigh of the ROI

        /* info about the output mask, only needed for segmentation network */
        uint16_t mx,my,mw,mh;   // topleft corner, widht, heigh of the mask
        uint16_t canvas_width;  // overall canvas width
        uint16_t canvas_height; // overall canvas height
        uint8_t  reserved1[32]; // reserved, must be zero
} ambaod_cfg_roi_t;

#define AMBAOD_CFG_GROUP_TYPE           0x1002
#define AMBAOD_CFG_GROUP_VER            0x0001
typedef struct {
        /* type, version, target_frame, common for all message */
        uint16_t type;          // AMBAOD_CFG_GROUP_TYPE
        uint16_t version;       // AMBAOD_CFG_GROUP_VER
        uint32_t target_frame;  // target frame in where the msg takes effect
        uint8_t  num_classes;   // number of classes
        uint8_t  reserved0[3];  // reserved, must be zero
        uint8_t  map[52];       // the map for each classes
} ambaod_cfg_group_t;

#define AMBAOD_CFG_CAMCALIB_TYPE        0x1003
#define AMBAOD_CFG_CAMCALIB_VER         0x0001
typedef struct {
        /* type, version, target_frame, common for all message */
        uint16_t type;          // AMBAOD_CFG_CAMCALIB_TYPE
        uint16_t version;       // AMBAOD_CFG_CAMCALIB_VER
        uint32_t target_frame;  // target frame in where the msg takes effect
        uint32_t camera_id;     // ID of the camera
        float    intrinsic[9];  // intrinsic 3x3 matrix
        float    extrinsic_R[3];// orientation: yaw, pitch, roll
        float    extrinsic_T[3];// position: x, y, z
} ambaod_cfg_camcalib_t;

#define AMBAOD_CFG_NMS_TYPE             0x1004
#define AMBAOD_CFG_NMS_VER              0x0001
/* 
 * NMS info for each class, up to 14 entries in one message
 * if more than 14 entries are needs, multiple message can be used
 * class_id is the ID of the class
 * Threshold will be normalized to [0,1] by division of 256
 * To disable IOU, set iou_threshold to zero
 * To disable IOM, set iom_threshold to zero
 */
typedef struct {
        uint8_t  class_id;
        uint8_t  iou_threshold;
        uint8_t  iom_threshold;
        uint8_t  score_threshold;
} ambaod_cfg_nms_info_t;

typedef struct {
        /* type, version, target_frame, common for all message */
        uint16_t type;          // AMBAOD_CFG_NMS_TYPE
        uint16_t version;       // AMBAOD_CFG_NMS_VER
        uint32_t target_frame;  // target frame in where the msg takes effect
        uint8_t  num_classes;   // number of classes
        uint8_t  reserved0[3];  // reserved, must be zero
        ambaod_cfg_nms_info_t nms[13];
} ambaod_cfg_nms_t;

#endif  /* CVAPI_AMBAOD_H_FILE */

