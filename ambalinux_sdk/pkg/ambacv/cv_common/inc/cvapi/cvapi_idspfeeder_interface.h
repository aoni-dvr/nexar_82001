/*
 * Copyright (c) 2017-2018 Ambarella, Inc.
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign copyrights.
 * This Software is also the confidential and proprietary information of
 * Ambarella, Inc. and its licensors.  You may not use, reproduce, disclose,
 * distribute, modify, or otherwise prepare derivative works of this Software
 * or any portion thereof except pursuant to a signed license agreement or
 * nondisclosure agreement with Ambarella, Inc. or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and
 * return this Software to Ambarella, Inc.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef CVAPI_IDSPFEEDER_INTERFACE_H_FILE
#define CVAPI_IDSPFEEDER_INTERFACE_H_FILE

#include "ambint.h"

#define MAX_NUM_FOV             16
#define IDSP_FEEDER_IDSP_INIT   0xF0000001U
#define IDSP_FEEDER_STREAM_CTL  0xF0000003U


// Macro_Patch
// Remove Comment to Compile the Patch that provides Config Message with ID = IDSP_FEEDER_MSG_EXTRA_ID for IDSP Feeder to set the Frame Number Offset
//#define IDSP_FEEDER_COMPILE_PATCH_FRAMENUMOFFSET 1

#define IDSP_FEEDER_MSG_EXTRA_ID 0xF0000005U

#if 0 // this is for reference only
typedef struct
{
	uint32_t  base_daddr;              /* base daddre of Message FIFO */
	uint32_t  max_num_msg;             /* maximum number of messages */
	uint32_t  read_index;              /* read index */
	uint32_t  write_index;             /* write index */
	uint32_t  reserved[12];
} dsp_msg_q_info_t;
#endif


typedef struct {
	uint32_t  fov_id;                 /* fov for this frame list */
	uint32_t  frame_list_table_addr;  /* address for frame list table */
	uint32_t  frame_list_table_size;  /* size of frame list table */
	uint32_t  frame_buffer_length;    /* length of each frame buffer */
	uint32_t  ld_list_table_addr;     /* address for ld list table */
	uint32_t  ld_list_table_size;     /* size of ld list table */
	uint32_t  ld_buffer_length;       /* length of each ld buffer */
	uint32_t  meta_list_table_addr;   /* address for metadata list table */
	uint32_t  meta_list_table_size;   /* size of metadata list table */
	uint32_t  meta_buffer_length;     /* length of each metadata buffer */
} vp_frame_list_descriptor_t;

typedef struct {
	uint32_t  message_id;             /* IDSP_FEEDER_IDSP_INIT */
	uint32_t  param_size;             /* In bytes */
	uint32_t  async_cmd_queue_addr;   /* async cmd queue address */
	uint32_t  async_cmd_queue_size;   /* async cmd queue size */
	uint32_t  vp_msg_queue_cb_addr;   /* --> dsp_msg_q_info_t for vp */
	uint32_t  num_fov;                /* number of input fov */
	vp_frame_list_descriptor_t frame_list_descriptor[MAX_NUM_FOV];
} idspfeeder_idsp_init_msg_t;

typedef struct {
	uint32_t  message_id;             /*  IDSP_FEEDER_STREAM_CTL */
	uint32_t  param_size;              /* In bytes */
#define IDSPFEEDER_STREAMING_CTRL_STOP  0
#define IDSPFEEDER_STREAMING_CTRL_START 1
#define IDSPFEEDER_STREAMING_CTRL_NEXT  2
	uint32_t  ctrl_op;
} idspfeeder_streaming_ctrl_msg_t;






#ifdef IDSP_FEEDER_COMPILE_PATCH_FRAMENUMOFFSET

typedef struct {
	uint32_t  message_id;             /*  IDSP_FEEDER_STREAM_CTL */
	uint32_t  frame_offset;
} idspfeeder_extra_config_msg_t;

#pragma message ( "Compiling " __FILE__ " with Frame Num Offset Patch" )

#endif



#endif  /* !CVAPI_IDSPFEEDER_INTERFACE_H_FILE */

