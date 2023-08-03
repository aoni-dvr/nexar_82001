/*******************************************************************************/
/*  Copyright 2020 Ambarella Inc.                                              */
/*  Author: Joey Li                                                            */
/*  Email:  jli@ambarella.com                                                  */
/*******************************************************************************/

#ifndef CVAPI_AMBANET_INTERFACE_H_FILE
#define CVAPI_AMBANET_INTERFACE_H_FILE

#include "ambint.h"

#if (defined(__linux__) || defined(ORCVIS))
#define FLOAT       float
#endif

#ifndef AMBANET_MSG_TYPE_LICENSE
#define AMBANET_MSG_TYPE_LICENSE        0xAB001000U
#endif

#define AMBANET_OUTPUT_NAME             "AMBANET_OUTPUT"
#define AMBANET_MSG_UNINITIALIZED       0x0

#define AMBANET_MSG_TYPE_ROI            0x4001

/* generic ROI configration */
typedef struct {
        uint32_t msg_type;              /* AMBANET_MSG_TYPE_ROI               */
        uint16_t roi_index;             /* index of this ROI                  */
        uint16_t frame_width;           /* full image width                   */
        uint16_t frame_height;          /* full image height                  */ 
        uint16_t pyramid_index;         /* pyramid scale index                */
        int16_t  pyramid_start_col;     /* offset of the pyramid image from   */
        int16_t  pyramid_start_row;     /*     the full image                 */
        uint16_t pyramid_ratio_col;     /* scale factor of the full image to  */
        uint16_t pyramid_ratio_row;     /*     pyramid image, lshifted by 8   */

        uint16_t roi_start_col;         /* placement of ROI in pyramid image, */
        uint16_t roi_start_row;         /*     in pyramid image coordinates   */
        uint16_t roi_width;
        uint16_t roi_height;
} ambanet_roi_config_t;

#endif  /* CVAPI_AMBANET_INTERFACE_H_FILE */
