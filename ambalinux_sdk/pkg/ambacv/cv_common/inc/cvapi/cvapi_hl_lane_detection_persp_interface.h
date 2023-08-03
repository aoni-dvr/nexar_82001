#ifndef CVAPI_HL_LANE_DETECTION_PERSP_INTERFACE_H_FILE
#define CVAPI_HL_LANE_DETECTION_PERSP_INTERFACE_H_FILE

#include "cvapi_ll_lane_detection_persp_interface.h"

#define HL_LANE_MAXNUM_LANE                 30
#define HL_LANE_MAXNUM_POINT                150
#define HL_LOC_MAX_PNT                      6000
#define HL_MAX_BBX_NUM                      30
#define CALIB_CONFIG_MSG_TYPE               2
#define HL_LANE_STEP                        1.0
#define HL_LANE_CONFIDENCE_BAD_VALUE        -1.0
#define INIT_HL_MSG                         0x0189
#define IONAME_HLLDPERSP_OUT                "HLLDPERSP_OUT"
#define IONAME_HLLDPERSP_LOC_OUT            "HLLDPERSP_LOC_OUT"
#define HL_LOC_MAX_LANE_MARKERS             1500
#define HL_LOC_MAX_CURBS                    1000
#define HL_LOC_MAX_FREESPACE                3500

/*! \brief The maximum number features objects in the message. */
#define AC_CVAPI_LD_NUM_FEATURES            (50)

typedef enum {
    LD_CONFIG_CHN_ID        = 0,
    LD_CONFIG_SEGNET        = 1,
    LD_CONFIG_LANE_COLOR    = 2,
    LD_CONFIG_LANE_TYPE     = 3,
    LD_CONFIG_LANE_INSTANCE = 4,
    LD_CONFIG_OD            = 5,
    LD_CONFIG_DSI           = 6,
    LD_CONFIG_MSG           = 7
} LD_CONFIG_TYPE;

typedef enum {
    LDP_ADAS_DEPARTURE_NONE = -1,
	LDP_ADAS_DEPARTURE_LEFT = 0,
	LDP_ADAS_DEPARTURE_RIGHT = 1
} LDP_ADAS_DEPARTURE_TYPE;

typedef struct hl_departure_s {
    double distance_lr[2];			        //distance vector from lane marking
    LDP_ADAS_DEPARTURE_TYPE car_departure;	//departure status
} hl_departure_t;
#define AC_CVAPI_HL_DEPARTURE_T_VERSION      (0U)

typedef struct point2D_s {
    uint16_t x;
    uint16_t y;
} point2D_t;

typedef struct point3D_s {
   float x;
   float y;
   float z;
} point3D_t;

typedef struct hl_od_bbx_s {
    uint16_t    cat;
    uint16_t    fid;
    uint32_t    track;                 /* track ID, assigned by tracker */
    uint32_t    score;
    uint16_t    x;
    uint16_t    y;
    uint16_t    w;
    uint16_t    h;
    uint16_t    is_inside_lane;        // FIXME
    uint16_t    pt_world_is_valid;
    point3D_t   pt_world;
} hl_od_bbx_t;

typedef struct hl_od_bbx_list_s {
    uint32_t    msg_code;
    uint32_t    source;
    uint32_t    capture_time;
    uint32_t    frame_num;
    uint32_t    num_bbx;
    hl_od_bbx_t bbx[HL_MAX_BBX_NUM];    
} hl_od_bbx_list_t;
#define AC_CVAPI_OD_BBX_T_VERSION      (0U)

// confidence for filtered lane
typedef struct hl_confidence_s {
	uint32_t size;
	float 	 data[HL_LANE_MAXNUM_POINT];
} hl_confidence_t;
#define AC_CVAPI_HL_CONFIDENCE_T_VERSION      (0U)

typedef struct lane_detection_persp_calib_data_s {
  float     ku;
  float     kv;
  float     u0;
  float     v0;
  uint32_t  width;
  uint32_t  height;
  float     position_xyz[3];
  float     orientation_rpy[3];
  float     baseline;
} lane_detection_persp_calib_data_t;

typedef struct lane_detection_persp_calib_config_msg_s {
  lane_detection_persp_config_msg_header_t  header;
  lane_detection_persp_calib_data_t         calib_data;
} lane_detection_persp_calib_config_msg_t;

typedef struct line3D_s {
   uint32_t    num_point;
   point3D_t   pts3d[HL_LANE_MAXNUM_POINT];
} line3D_t;
#define AC_CVAPI_LINE_3D_T_VERSION      (0U)

typedef struct line2D_s {
   uint32_t    num_point;
   point2D_t   pts2d[HL_LANE_MAXNUM_POINT];
} line2D_t;
#define AC_CVAPI_LINE_2D_T_VERSION      (0U)

enum  {LDP_LEFT, LDP_RIGHT, LDP_MULTI_LEFT, LDP_MULTI_RIGHT, LDP_NUM_EGO_LANE};
enum  {LDP_FILT_CENTER,LDP_FILT_LEFT,LDP_FILT_RIGHT,LDP_NUM_FILT};

typedef enum {
    LDP_MARKER_UNKNOWN = 0,
	LDP_MARKER_SOLID = 1,
	LDP_MARKER_DASHED = 2,
	LDP_MARKER_DOTS = 3,
	LDP_MARKER_SIZE = 4
} LDP_MARKER_TYPE;

typedef enum {
    LDP_COLOR_UNKNOWN = 0,
	LDP_COLOR_MARKER_WHITE = 1,
	LDP_COLOR_MARKER_YELLOW = 2
} LDP_COLOR_MARKER_TYPE;

typedef struct line_attribute_s {
    LDP_MARKER_TYPE         marker_type;
    LDP_COLOR_MARKER_TYPE   marker_color;
} line_attribute_t;
#define AC_CVAPI_LINE_ATTRIBUTE_T_VERSION      (0U)

//TODO confidence structure
typedef struct hl_lane_detection_persp_ego_output_s {
    int                 mask[LDP_NUM_EGO_LANE];
    line2D_t            lane2d[LDP_NUM_EGO_LANE];
    line3D_t            lane3d[LDP_NUM_EGO_LANE];
    line3D_t            ll_cc_pts[LDP_NUM_EGO_LANE];
    line_attribute_t    attribute[LDP_NUM_EGO_LANE];    
} hl_lane_detection_persp_ego_output_t;
#define AC_CVAPI_LANE_DETECTION_PERSP_EGO_OUTPUT_T_VERSION      (0U + AC_CVAPI_LINE_2D_T_VERSION + \
                                                                      AC_CVAPI_LINE_3D_T_VERSION + \
                                                                      AC_CVAPI_LINE_ATTRIBUTE_T_VERSION)
typedef struct hl_ego_filtered_s {
    hl_confidence_t		confidence[LDP_NUM_FILT];	// lane confidence
    int                 mask[LDP_NUM_FILT];			// mask valid lane	
    line2D_t 	        lane2d[LDP_NUM_FILT];       // 2d lane
    line3D_t 	        lane3d[LDP_NUM_FILT];      	// 3d lane
    line_attribute_t 	attribute[LDP_NUM_FILT];    
} hl_ego_filtered_t;
#define AC_CVAPI_EGO_FILTERED_T_VERSION      (0U + AC_CVAPI_HL_CONFIDENCE_T_VERSION + \
                                                   AC_CVAPI_LINE_2D_T_VERSION + \
                                                   AC_CVAPI_LINE_3D_T_VERSION + \
                                                   AC_CVAPI_LINE_ATTRIBUTE_T_VERSION)

typedef struct hl_lane_detection_persp_output_s { 
    uint32_t            frame_no;
    uint32_t            errcode;
    uint32_t            num_roi;
    lane_detection_persp_roi_param_t roi_param[NUM_ROI];
    int                 debug_ll;
    uint32_t            img_w;
    uint32_t            img_h;
    uint32_t            num_lane;
    line2D_t            lane2d[HL_LANE_MAXNUM_LANE];
    line3D_t            lane3d[HL_LANE_MAXNUM_LANE];
    hl_lane_detection_persp_ego_output_t ego_out;   
    hl_ego_filtered_t   ego_out_filt;
    hl_od_bbx_list_t    od_list;
    int                 od_enabled;
    hl_departure_t      departure_info;
    hl_ego_filtered_t   ego_out_filt_guess; // FIXME  
} hl_lane_detection_persp_output_t;
// This version MUST match, somehow, with the AC_CV_LANE_DETECTION_T_VERSION from AmbaComm
#define AC_CVAPI_LANE_DETECTION_T_VERSION      (0U + AC_CVAPI_LANE_DETECTION_PERSP_ROI_PARAM_T_VERSION + \
                                                     AC_CVAPI_LINE_2D_T_VERSION + \
                                                     AC_CVAPI_LINE_3D_T_VERSION + \
                                                     AC_CVAPI_LANE_DETECTION_PERSP_EGO_OUTPUT_T_VERSION + \
                                                     AC_CVAPI_EGO_FILTERED_T_VERSION + \
                                                     AC_CVAPI_OD_BBX_T_VERSION + \
                                                     AC_CVAPI_HL_DEPARTURE_T_VERSION)

typedef struct hl_lane_detection_persp_adas_output_s {
    uint32_t            frame_no;           // frame number
    uint32_t            errcode;			// error code
    hl_ego_filtered_t   ego_out_filt;		// lane filtered 
    hl_departure_t      departure_info;  	// ldw
} hl_lane_detection_persp_adas_output_t;

typedef struct hl_ld_persp_output_s { 
    uint32_t frame_no;
    uint32_t errcode;
    hl_lane_detection_persp_ego_output_t ego_out;   
    hl_ego_filtered_t ego_out_filt;
} hl_ld_persp_output_t;

typedef enum {
    LDP_FEATURE_LANE_MARKING,
    LDP_FEATURE_CURB,
    LDP_FEATURE_FREE_SPACE
} LDP_FEATURE_TYPE;

typedef enum {
    NO_COLOR   = 0,
    WHITE      = 1,
    YELLOW     = 2,
    RED        = 3,
    BLUE       = 4
} LDP_LANE_COLOR;

typedef enum {
    NO_TYPE      = 0,
    SOLID        = 1,
    DASH         = 2,
    DOUBLE_SOLID = 3,
    DOUBLE_DASH  = 4
} LDP_LANE_TYPE;

typedef enum {
    NO_INSTANCE    = 0,
    DRIVING_LEFT   = 1,
    DRIVING_RIGHT  = 2,
    ADJACENT_LEFT  = 3,
    ADJACENT_RIGHT = 4,
    CENTER         = 5
} LDP_LANE_INSTANCE;

typedef struct hl_ld_persp_lane_info_s {
    LDP_LANE_COLOR    color;
    LDP_LANE_TYPE     type;
    LDP_LANE_INSTANCE instance;
} hl_ld_persp_lane_info_t;
#define AC_CVAPI_LD_PERSP_LANE_INFO_T_VERSION      (0U)


typedef struct hl_ld_persp_loc_feature_s {
    LDP_FEATURE_TYPE        type;       // lane markings, curbs, etc.
    hl_ld_persp_lane_info_t lane_info;  // additional categories if type is lane
    uint16_t                id;         // id of its connected component
    uint16_t                padding;
    float                   disparity;  // disparity of point if dsi is used else is -1
    point2D_t               uv;         // uv coordinates
    point3D_t               xyz;        // xyz coordinates (camera/body)
} hl_ld_persp_loc_feature_t;
#define AC_CVAPI_LD_FEATURE_T_VERSION      (1U + AC_CVAPI_LD_PERSP_LANE_INFO_T_VERSION)

typedef struct hl_ld_persp_loc_output_s { 
    uint32_t                    frame_no;
    uint32_t                    errcode;
    uint32_t                    size;
    int8_t                      uv_scale;
    int8_t                      disparity_scale;
    uint16_t                    padding;
    hl_ld_persp_loc_feature_t   features[HL_LOC_MAX_PNT];
} hl_ld_persp_loc_output_t;
#define AC_CVAPI_FEATURES_T_VERSION         (0U + AC_CVAPI_LD_FEATURE_T_VERSION)

typedef struct {
    uint32_t                    frame_no;
    uint32_t                    errcode;
    uint16_t                    num_features; //<! The number of valid features in the array. Range [0, AC_CVAPI_LD_NUM_FEATURES).
    uint16_t                    padding;
    uint16_t                    cam_ids[AC_CVAPI_LD_NUM_FEATURES];
    hl_ld_persp_loc_output_t*   list[AC_CVAPI_LD_NUM_FEATURES];
} hl_ld_persp_loc_output_list_t;
#define AC_CVAPI_FEATURES_LIST_T_VERSION         (0U + AC_CVAPI_FEATURES_T_VERSION)

// Struct aligned to 32
typedef struct hl_lane_detection_persp_config_msg_s {
    uint32_t    msg_type;
    float       x;
    float       y;
    float       z;
    float       yaw;
    float       pitch;
    float       roll;
    float       u0;
    float       v0;
    float       ku;
    float       kv;
    float       baseline;
    int         hood;
    int         img_w;
    int         img_h;
    int         fps;
    int         use_segnet;
    int         use_od;
    int         use_dsi;
    int         use_curbs;
    int         use_freespace;
    int         use_kalman;
    int         use_canbus;
    int         update_calibration;
    int         features_out_body;
    float       car_width;
    int         spu_fex_scale;          // disparity
    int         spu_fex_roi_start_x;
    int         spu_fex_roi_start_y;
    int         spu_fex_roi_width;
    int         spu_fex_roi_height;
    int32_t     segnet_scale;           // segnet
    int32_t     segnet_buffer_width;
    int32_t     segnet_buffer_height;
    int32_t     segnet_roi_start_x;
    int32_t     segnet_roi_start_y;
    int32_t     segnet_roi_width;
    int32_t     segnet_roi_height;
    int32_t     skip_data_on_border_roi;
    int32_t     skip_data_min_size;
} hl_lane_detection_persp_config_msg_t;

#endif  /* CVAPI_HL_LANE_DETECTION_PERSP_INTERFACE_H_FILE */
