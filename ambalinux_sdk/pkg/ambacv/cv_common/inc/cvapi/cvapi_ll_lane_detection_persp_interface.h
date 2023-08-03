#ifndef CVAPI_LL_LANE_DETECTION_PERSP_INTERFACE_H_FILE
#define CVAPI_LL_LANE_DETECTION_PERSP_INTERFACE_H_FILE

#define NUM_ROI           4
#define ROI_W             960
#define ROI_H             100
#define ROI_H_S           1080
#define CC_MAX_W          96
#define CC_W_BYTES        (2*ALIGN_32_BYTE(CC_MAX_W))
#define ALIGN_32_BYTE(x)  ((((x) + 31) >> 5) << 5)
#define INIT_LL_MSG       0x0190

#ifndef __VAS__

#define ROI_CONFIG_MSG_TYPE 1

// SegNet values related to output masks from AmbaNet/SegNet
typedef enum {
    LD_SEGNET_CLASS_BACKGROUND   = 0,
    LD_SEGNET_CLASS_ROAD         = 1,
    LD_SEGNET_CLASS_CURB         = 2,
    LD_SEGNET_CLASS_LANE_MARKING = 3
} LD_SEGNET_CLASS;

typedef enum {
    LD_LL_SEGENT_OUT_ID_LANE_MARKING  = 0,
    LD_LL_SEGENT_OUT_ID_CURB          = 1,
    LD_LL_SEGENT_OUT_ID_ROAD          = 2
} LD_LL_SEGENT_OUT_ID;

// MarcoNet values related to output masks
typedef enum {
    LD_MARCONET_CLASS_LEFT         = 0,
    LD_MARCONET_CLASS_RIGHT        = 1,
    LD_MARCONET_CLASS_MULTI_LEFT   = 2,
    LD_MARCONET_CLASS_MULTI_RIGHT  = 3
} LD_MARCONET_CLASS;

typedef enum {
    LD_LL_MARCONET_OUT_ID_LEFT        = 0,
    LD_LL_MARCONET_OUT_ID_RIGHT       = 1,
    LD_LL_MARCONET_OUT_ID_MULTILEFT   = 2,
    LD_LL_MARCONET_OUT_ID_MULTIRIGHT  = 3
} LD_LL_MARCONET_OUT_ID;

typedef enum {
    LD_LL_NET_SEGNET = 0,
    LD_LL_NET_MARCONET
} LD_LL_NET_TYPE;

typedef struct lane_detection_persp_roi_param_s {
    uint32_t  scale;
    int32_t   start_col;
    int32_t   start_row;
    uint32_t  w;
    uint32_t  h;
} lane_detection_persp_roi_param_t;
#define AC_CVAPI_LANE_DETECTION_PERSP_ROI_PARAM_T_VERSION      (0U)

typedef struct lane_detection_persp_config_msg_header_s {
    uint32_t msg_type;
} lane_detection_persp_config_msg_header_t;

typedef struct lane_detection_persp_rois_config_msg_s {
    lane_detection_persp_config_msg_header_t  header;
    lane_detection_persp_roi_param_t          roi_param[NUM_ROI];
    lane_detection_persp_roi_param_t          ground_roi_param;
} lane_detection_persp_rois_config_msg_t;

typedef struct ll_lane_roi_output_s
{
    //uint16_t  line_cc[3*ALIGN_32_BYTE(CC_MAX_W)*ROI_H];
    //uint16_t  dots_cc[3*ALIGN_32_BYTE(CC_MAX_W)*ROI_H];
    uint16_t    merge_cc[3*ALIGN_32_BYTE(CC_MAX_W)*2*ROI_H];
} ll_lane_roi_output_t;

typedef struct ll_lane_detection_persp_output_s
{
    ll_lane_roi_output_t roi_output[NUM_ROI];
} ll_lane_detection_persp_output_t;

//segnet
typedef struct ll_seg_lane_roi_output_s
{
    //uint16_t  line_cc[3*ALIGN_32_BYTE(CC_MAX_W)*ROI_H];
    //uint16_t  dots_cc[3*ALIGN_32_BYTE(CC_MAX_W)*ROI_H];
    uint16_t    merge_cc[3*ALIGN_32_BYTE(CC_MAX_W)*ROI_H_S];
} ll_seg_lane_roi_output_t;

typedef struct ll_seg_lane_detection_persp_output_s
{
    ll_seg_lane_roi_output_t roi_output[NUM_ROI];
} ll_seg_lane_detection_persp_output_t;

typedef struct ll_lane_detection_persp_config_msg_s {
    uint32_t    msg_type;
    int         net_type;
    int         use_curbs;
    int         use_freespace;
} ll_lane_detection_persp_config_msg_t;

#endif

#endif
