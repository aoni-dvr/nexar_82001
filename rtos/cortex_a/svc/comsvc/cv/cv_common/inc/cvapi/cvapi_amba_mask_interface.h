/*******************************************************************************/
/*  Copyright 2018 Ambarella Inc.                                              */
/*  Author: Joey Li                                                            */
/*  Email:  jli@ambarella.com                                                  */
/*******************************************************************************/

#ifndef CVAPI_AMBA_MASK_INTERFACE_H_FILE
#define CVAPI_AMBA_MASK_INTERFACE_H_FILE

#include "ambint.h"

#define AMBA_MASK_OUTPUT_NAME           "AMBANET_OUTPUT"

#define AMBA_MASK_CONFIG_MSG_TYPE       0x1000

#define AMBANET_MSG_TYPE_LICENSE        0xAB001000U

/* config message format */
typedef struct {
    uint32_t msg_type;              /* AMBA_MASK_CONFIG_MSG_TYPE         */
    uint32_t msg_length;            /* total length of the message       */
    uint32_t image_pyramid_index;   /* level of idsp image pyramid       */
    uint32_t num_categories;        /* number of categories              */

    /* positional information for intput ROI */
    uint16_t roi_start_col;         /* start column of roi               */
    uint16_t roi_start_row;         /* start row of roi                  */
    uint16_t roi_width;             /* roi width                         */
    uint16_t roi_height;            /* roi height                        */

    /* positional information for output segamentation mask */
    uint16_t buff_width;            /* output buffer width               */
    uint16_t buff_height;           /* output buffer height              */
    uint16_t mask_start_col;        /* start column of seg mask          */
    uint16_t mask_start_row;        /* start row of seg mask             */
    uint16_t mask_width;            /* mask width                        */
    uint16_t mask_height;           /* mask height                       */
    uint16_t mask_min_val;          /* minimum value in the mask         */
    uint16_t mask_max_val;          /* maximum value in the mask         */
} amba_mask_config_t;

/* output data format */
typedef struct {
    uint32_t type;                  /* AMBA_MASK_OUTPUT_TYPE             */
    uint32_t length;                /* sizeof(amba_segnet_out_t)         */
    ptr64_t  buff_addr;             /* mask buffer virtual address       */
    uint16_t buff_width;            /* buff width                        */
    uint16_t buff_height;           /* buff height                       */
    uint16_t mask_start_col;        /* start column of seg mask          */
    uint16_t mask_start_row;        /* start row of seg mask             */
    uint16_t mask_width;            /* mask width                        */
    uint16_t mask_height;           /* mask height                       */
    uint8_t  padding[100];          /* pad to 128-byte aligned           */
} amba_mask_out_t;

#endif  /* CVAPI_AMBA_MASK_INTERFACE_H_FILE */
