/**
  * @brief The TTC Estimation Output Interface 
  * @todo TODO: Move it in cv_common 
  * @note Corresponding CVTask is in `arm_framework/cvtask/ttc_estimation` and CVTask Name is `ttc_estimation` and the only output channel is `TTC_ESTIMATION_OUTPUT` 
  * @todo 
  * - [ ] Rename cvapi_fcw_interface.h 
  * - [ ] Look at spu_scale_config_t and spu_internal_scale_config_t 
  * 
  */
#ifndef CVAPI_FCW_INTERFACE_H
#define CVAPI_FCW_INTERFACE_H


#include <stdint.h>

#define FCW_MAX_TRACKED_OBJ 256

#define FCW_INVALID_TTC 0xFFFFFFFF
#define FCW_INVALID_OBJECT_ID 0xFFFFFFFF


#define FCW_WRT_LANE_DIST_SCALE_FACTOR 100

#define FCW_RESERVED_SIZE 1024

/**
  * @brief BBox Information propagated 
  * @todo Tracker Output Passthrough 
  */
typedef struct 
{
    uint32_t id;                ///< NOTE: Tracker ID, if 0xFFFFFFFF then invalid 

    
    int32_t  bb_start_row;
    int32_t  bb_start_col;
    uint32_t bb_height_m1;
    uint32_t bb_width_m1;
} od_bbox_t; 

//void print_od_bbox(const od_bbox_t* in); 


/**
  * @brief Warning Level 
  * @note Allowed values for warning_level
  */
enum 
{
    FCW_WARNING_NO=0,           /* No Warning, e.g. target vehicle - ego vehicle distance is increasing over time */
    FCW_WARNING_LOW,            /* Low Warning, e.g. target vehicle slowly approaching */
    FCW_WARNING_MID,            /* Mid Warning, e.g. target vehicle approaching at a moderate speed */
    FCW_WARNING_HIGH,           /* High Warning, e.g. target vehicle approaching fast */
    FCW_WARNING_NA              /* For some reasons it has not been possible to compute TTC */
}; 


/**
  * @brief It defines the possible values in fcw_tracked_object_t->lane_position field 
  * @note Allowed values for lane_position 
  */
enum 
{
    FCW_LANE_POS_EGO, 
    FCW_LANE_POS_LEFT, 
    FCW_LANE_POS_RIGHT, 
    FCW_LANE_POS_NOT_DEFINED 
}; 

/**
  * @brief Leading Status 
  * @note Allowed values for fcw_tracked_object_t->leading_status
  */
enum 
{
    FCW_LEADING_STATUS_NO_LEADING=0, 
    FCW_LEADING_STATUS_LEADING
}; 

/**
  * @brief Single Item Results 
  */
typedef struct 
{
    od_bbox_t bbox; 
    
    // TODO: [x] Rename to warning_level 
    uint32_t warning_level; 
    int32_t ttc; 
    
    // TODO: enum Ego Lane, left, right, not defined 
    uint32_t lane_position; 
    
    // NOTE: Used to identity the BBox selcted for TTC Computation by the internal selection policy 
    // Rename to Leading Status as enum LEADING / NOT LEADING 
    uint32_t leading_status; 
    
} fcw_tracked_object_t; 



/**
  * @brief The output on the `TTC_ESTIMATION_OUTPUT` channel 
  */
typedef struct 
{
    // Rename to data --> tracked_objects 
    fcw_tracked_object_t tracked_objects[FCW_MAX_TRACKED_OBJ];                     ///< List of Tracked BBox with TTC 
    // Rename n --> num_objects 
    uint32_t num_objects;                                                         ///< Number of Elements  
    
    char debug[FCW_RESERVED_SIZE];                                   ///< Reserved 
 
 // TODO: Rename fcw_estimation_output_t --> fcw_output_t
} fcw_output_t; 




#define FCW_CONFIG                     0x00000001

typedef struct fcw_header_s {
    uint32_t    m_message_type;         ///< NOTE: To be set FCW_CONFIG
} fcw_header_t;



#define FCW_FIELD_WS_BASE              (1 << 3)

/**
  * @brief This is the unique configuration message payload 
  * @note It is possible to selectivate change some fields 
  */
typedef struct fcw_config_s 
{
    fcw_header_t m_header; 
    //union config_msg_bit_field_t config_msg_bit_field; 
    
    uint32_t m_reserved_0[11]; 
        
    // NOTE: The offset to be applied to the LR Thresholds and RL Thresholds 
    float ws_offset; 
    
    // NOTE: BL and BR Coeff, valid [0, 100]
    uint32_t m_reserved_1; 
        
    // TODO: Set according to the defines 
    uint32_t m_field_mask; 
    
} fcw_config_t; 




#endif 



