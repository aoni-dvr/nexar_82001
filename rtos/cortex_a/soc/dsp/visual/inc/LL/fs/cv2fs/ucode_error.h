/* CV2 DSP error code header file
Revision: 43821
Last Changed Rev: 43504
Last Changed Date: 2021-11-02 18:35:26 -0700 (Tue, 02 Nov 2021)
*/
/**
 * @file ucode_error.h
 *
 *  Copyright (c) 2020 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 * @brief Error code message header file
 *
 * History:
 *    2019/02/27 - [Chin-Chang Yang] created the file.
 **/

#ifndef UCODE_ERROR_H_
#define UCODE_ERROR_H_

#define ERR_CODE_MOD_MM          0x0u
#define ERR_CODE_MOD_API         0x1u
#define ERR_CODE_MOD_MSG         0x2u
#define ERR_CODE_MOD_BOOT        0x3u
#define ERR_CODE_MOD_IENG        0x4u
#define ERR_CODE_MOD_VPROC       0x5u
//\#define ERR_CODE_MOD_IDSPDRV 0x6u
#define ERR_CODE_MOD_VIN         0x7u
#define ERR_CODE_MOD_PREV        0x8u
#define ERR_CODE_MOD_PROC_HL     0x9u
#define ERR_CODE_MOD_VOUT        0xAu
#define ERR_CODE_MOD_ENC         0xBu
#define ERR_CODE_MOD_ENG0        0xCu
#define ERR_CODE_MOD_PROXY       0xDu
#define ERR_CODE_MOD_DEC         0xEu
#define ERR_CODE_IDSP_HW_TIMEOUT 0xFu
#define ERR_CODE_MOD_NUM         0x10u

#include "idspdrv_error.h"

/*****************************************************************************
 * module_idx 0 ERR_CODE_MOD_MM
 *****************************************************************************/
#define MM_ERR_UNKNOWN_CODE 0x0u
#define MM_ERR_UNKNOWN_MSG "MM:: unknown error arg0:%d arg1:%d arg2:%d arg3:%d arg4:%d\n"

#define MM_ERR_TILED_FMT_CODE 0x00001u
#define MM_ERR_TILED_FMT_MSG  "MM:: TBD: does not support tiled format!\n"

#define MM_ERR_INVALID_FB_ID_CODE 0x00002u
#define MM_ERR_INVALID_FB_ID_MSG  "MM:: invalid fb_id=0x%08x\n"

#define MM_ERR_REQ_FB_FATAL_0_CODE 0x00003u
#define MM_ERR_REQ_FB_FATAL_0_MSG  "MM:: req_fb(0x%04x) fatal 0: pic_struct=%d last_fb_id=0x%08x used_cnt=%d pending=%d\n"

#define MM_ERR_REQ_FB_FATAL_1_CODE 0x00004u
#define MM_ERR_REQ_FB_FATAL_1_MSG  "MM:: req_fb() fatal 2: fbp_id=0x%04x fb_idx=%d fb_num_total=%d\n"

#define MM_ERR_REQ_FB_FATAL_2_CODE 0x00005u
#define MM_ERR_REQ_FB_FATAL_2_MSG  "MM:: req_fb() fatal 3: fb_idx=0x%04x top_used_cnt=%d bot_used_cnt=%d top_pending=%d bot_pending=%d\n"

#define MM_ERR_FB_USED_CNT_CODE 0x00006u
#define MM_ERR_FB_USED_CNT_MSG  "MM:: rel_fb() fatal: fb_id=0x%08x top_used_cnt=%d top_pending=%d bot_used_cnt=%d bot_pending=%d\n"

#define MM_ERR_FB_NUM_USED_CODE 0x00007u
#define MM_ERR_FB_NUM_USED_MSG  "MM:: rel_fb() fatal: fb_id=0x%08x, fb_num_used=%d\n"

#define MM_ERR_INC_FB_FATAL_CODE 0x00008u
#define MM_ERR_INC_FB_FATAL_MSG  "MM:: inc_fb() fatal: fb_id=0x%08x top_used_cnt=%d top_pending=%d bot_used_cnt=%d bot_pending=%d \n"

#define MM_ERR_DEC_FB_FATAL_CODE 0x00009u
#define MM_ERR_DEC_FB_FATAL_MSG  "MM:: dec_fb() fatal: fb_id=0x%08x top_used_cnt=%d top_pending=%d bot_used_cnt=%d bot_pending=%d \n"

#define MM_ERR_FB_INVALID_FB_TYPE_CODE 0x0000au
#define MM_ERR_FB_INVALID_FB_TYPE_MSG  "MM:: fbp/fb id=0x%04x invalid fb_type=%d\n"

#define MM_ERR_SET_FB_EXT_FAILED_CODE 0x0000bu
#define MM_ERR_SET_FB_EXT_FAILED_MSG  "MM:: set_fb_ext() failed: blk_size=%d > fb_size=%d\n"

#define MM_ERR_FB_ID_NOT_A_FRAME_CODE 0x0000cu
#define MM_ERR_FB_ID_NOT_A_FRAME_MSG  "MM:: fb_id=0x%08x is not a frame\n"

#define MM_ERR_GET_FB_AUX_FAILED_CODE 0x0000du
#define MM_ERR_GET_FB_AUX_FAILED_MSG  "get_fb_aux() failed, aux_width=0, fb_id=0x%08x, p_aux_buf=0x%08x\n"

#define MM_ERR_INVALID_FBP_TYPE_CODE 0x0000eu
#define MM_ERR_INVALID_FBP_TYPE_MSG  "MM:: check_fbp_id(0x%04x) fatal: invalid fbp type!\n"

#define MM_ERR_INVALID_FBP_INDEX_CODE 0x0000fu
#define MM_ERR_INVALID_FBP_INDEX_MSG  "MM:: check_fbp_id() failed: invalid fbp_id=0x%08x, fbp_index(%d) is over limit (%d)\n"

#define MM_ERR_INVALID_IF_EXT_CODE 0x00010u
#define MM_ERR_INVALID_IF_EXT_MSG  "MM:: fbp_id=0x%04x, invalid if_ext=%d\n"

#define MM_ERR_INVALID_IF_TILED_CODE 0x00011u
#define MM_ERR_INVALID_IF_TILED_MSG  "MM:: invalid if_tiled=%d\n"

#define MM_ERR_FBP_USED_CODE 0x00012u
#define MM_ERR_FBP_USED_MSG  "MM:: create_fbp() failed: fbp_used(%d) >= max_fbp_num(%d)\n"

#define MM_ERR_FBP_INF_CODE 0x00013u
#define MM_ERR_FBP_INF_MSG  "MM:: create_fbp() failed: fbp_inf_used(%d) + img_inf_max(%d) > max_imginf_num(%d)\n"

#define MM_ERR_FBUF_USED_CODE 0x00014u
#define MM_ERR_FBUF_USED_MSG  "MM:: create_fbp() failed: allow_ext=%d fbuf_used(%d) + frm_num_max(%d) >= max_fb_num(%d)\n"

#define MM_ERR_FRM_NUM_MAX_CODE 0x00015u
#define MM_ERR_FRM_NUM_MAX_MSG  "MM:: create_fbp() failed: frm_num_max=%d >= MAX_FB_NUM_IN_FBP %d\n"

#define MM_ERR_INVALID_MEM_PAR_ID_CODE 0x00016u
#define MM_ERR_INVALID_MEM_PAR_ID_MSG  "MM:: init_fbp_w_par() failed: fbp_id=0x%04x, invalid mem_par_id=0x%04x\n"

#define MM_ERR_INVALID_FRM_BLK_SIZE_CODE 0x00017u
#define MM_ERR_INVALID_FRM_BLK_SIZE_MSG  "MM:: init_fbp failed: fbp_id=0x%04x ch_fmt=%d lu_blk_size=%d ch_blk_size=%d aux_blk_size=%d\n"

#define MM_ERR_INVALID_CHROMA_FORMAT_CODE 0x00018u
#define MM_ERR_INVALID_CHROMA_FORMAT_MSG  "MM:: init_fbp_w_buf() failed: invalid fbp_id, tiled format only supports 420 but %d\n"

#define MM_ERR_INVALID_FB_NUM_TOTAL_CODE 0x00019u
#define MM_ERR_INVALID_FB_NUM_TOTAL_MSG  "MM:: init_fbp_w_buf() failed: tiled format fb_num_total=0\n"

#define MM_ERR_INVALID_BUF_WIDTH_CODE 0x0001au
#define MM_ERR_INVALID_BUF_WIDTH_MSG  "MM:: set_cur_fbp_imginfo_y_uv(0x%04x) failed: buf_width=%d lu_img_width=%d lu_img_offset_x=%d\n"

#define MM_ERR_INVALID_BUF_HEIGHT_CODE 0x0001bu
#define MM_ERR_INVALID_BUF_HEIGHT_MSG  "MM:: set_cur_fbp_imginfo_y_uv(0x%04x) failed: buf_height=%d lu_img_height=%d lu_img_offset_y=%d\n"

#define MM_ERR_INVALID_IMG_INF_IDX_CODE 0x0001cu
#define MM_ERR_INVALID_IMG_INF_IDX_MSG  "MM:: set_fbp_imginfo() failed, img_inf_idx(%d) >= max(%d)\n"

#define MM_ERR_MFRM_BLK_SIZE_CODE 0x0001du
#define MM_ERR_MFRM_BLK_SIZE_MSG  "MM:: init_mfbp_w_buf failed: fbp_id=0x%04x ch_fmt=%d lu_blk_size=%d\n"

#define MM_ERR_INVALID_MCB_NUM_CODE 0x0001eu
#define MM_ERR_INVALID_MCB_NUM_MSG  "MM:: create_mcbl() failed: mcb_num=%d > max=%d\n"

#define MM_ERR_INVALID_MCB_USED_CODE 0x0001fu
#define MM_ERR_INVALID_MCB_USED_MSG  "MM:: create_mcbl() failed: max_mcbl_num=%d mcbl_used=%d\n"

#define MM_ERR_INVALID_MCB_USED_NUM_CODE 0x00020u
#define MM_ERR_INVALID_MCB_USED_NUM_MSG  "MM:: create_mcbl() failed: max_mcb_num=%d > mcb_used=%d + max=%d\n"

#define MM_ERR_INVALID_MCBL_SIZE_CODE 0x00021u
#define MM_ERR_INVALID_MCBL_SIZE_MSG  "MM:: init_mcbl_with_bu() failed, mcb_size =%d mcb_num=%d >= buf_size=%d\n"

#define MM_ERR_POP_MCB_FAILED_CODE 0x00022u
#define MM_ERR_POP_MCB_FAILED_MSG  "MM:: pop_mcb() failed, mcb_used=%d mcb_num=%d mcb_head=0x%02x\n"

#define MM_ERR_PUSH_MCB_FAILED_CODE 0x00023u
#define MM_ERR_PUSH_MCB_FAILED_MSG  "MM:: push_mcb() failed, mcbl_id=0x%04x, mcb_used=%d mcb_id=0x%02x\n"

#define MM_ERR_INVALID_MCBL_ID_CODE 0x00024u
#define MM_ERR_INVALID_MCBL_ID_MSG  "MM:: mcbl_id is MCBL_ID_INVALID!\n"

#define MM_ERR_INVALID_MCB_ID_TO_FREE_CODE 0x00025u
#define MM_ERR_INVALID_MCB_ID_TO_FREE_MSG  "MM:: mcb_id_to_free is NULL!\n"

#define MM_ERR_INVALID_MEM_PAR_TYPE_CODE 0x00026u
#define MM_ERR_INVALID_MEM_PAR_TYPE_MSG  "MM:: invalid mem_type=%d\n"

#define MM_ERR_INVALID_ALIGNMENT_CODE 0x00027u
#define MM_ERR_INVALID_ALIGNMENT_MSG  "MM:: alignment must be 4B aligned but alignment=%d\n"

#define MM_ERR_INVALID_SIZE_CODE 0x00028u
#define MM_ERR_INVALID_SIZE_MSG  "MM:: size must be 4B aligned but size=%d\n"

#define MM_ERR_TOO_MANY_PAR_CODE 0x00029u
#define MM_ERR_TOO_MANY_PAR_MSG  "MM:: par_type=%d, too many partitions, par_last_used=%d max_par_num=%d!\n"

#define MM_ERR_NO_MORE_SMEM_CODE 0x0002au
#define MM_ERR_NO_MORE_SMEM_MSG  "MM:: no more smem, size_aligned=%d next_off=0x%08x smem_base=0x%08x, end=0x%08x\n"

#define MM_ERR_NO_MORE_DRAM_CODE 0x0002bu
#define MM_ERR_NO_MORE_DRAM_MSG  "MM:: no more dram, size_aligned=%d next_off=0x%08x dram_base=0x%08x, end=0x%08x\n"

#define MM_ERR_INVALID_MEM_PAR_CODE 0x0002cu
#define MM_ERR_INVALID_MEM_PAR_MSG  "MM:: invalid mem par 0x%04x par_idx=%d par_used=%d\n"

#define MM_ERR_INVALID_SUP_SUB_PAR_CODE 0x0002du
#define MM_ERR_INVALID_SUP_SUB_PAR_MSG  "MM:: invalid sup/sub par 0x%04x\n"

#define MM_ERR_UNKNOWN_PAR_LEVEL_CODE 0x0002eu
#define MM_ERR_UNKNOWN_PAR_LEVEL_MSG  "MM:: unknown par_level=%d, par_id=0x%x\n"

#define MM_ERR_TOO_MANY_SUP_PAR_CODE 0x0002fu
#define MM_ERR_TOO_MANY_SUP_PAR_MSG  "MM:: too many super partitions(%d) >= max=%d\n"

#define MM_ERR_TOO_MANY_SUB_PAR_CODE 0x00030u
#define MM_ERR_TOO_MANY_SUB_PAR_MSG  "MM:: too many sub partitions, used(%d) + sub_par_max(%d) > max_sub_par_num(%d)\n"

#define MM_ERR_TOO_MANY_SUP_SUB_PAR_CODE 0x00031u
#define MM_ERR_TOO_MANY_SUP_SUB_PAR_MSG  "MM:: sup_par_id=0x%04x, idx=%d: too many sub partitions(%d) >= max(%d)\n"

#define MM_ERR_INVALID_SUP_PAR_ID_CODE 0x00032u
#define MM_ERR_INVALID_SUP_PAR_ID_MSG  "MM:: (0x%04x) is not a valid sup_par_id (sup_par_used=0x%04x)!\n"

#define MM_ERR_SUB_PAR_OVER_LIMIT_CODE 0x00033u
#define MM_ERR_SUB_PAR_OVER_LIMIT_MSG  "MM:: total sub partition (0x%08x+0x%08x) is over the superpartition limit=(0x%08x)\n"

#define MM_ERR_INVALID_MEM_PAR_LEVEL_CODE 0x00034u
#define MM_ERR_INVALID_MEM_PAR_LEVEL_MSG  "MM:: alloc on sub_par_id (0x%04x) only supported on sub-partition but =0x%04x\n"

#define MM_ERR_SUB_PAR_SIZE_CODE 0x00035u
#define MM_ERR_SUB_PAR_SIZE_MSG  "MM:: alloc on sub_par_id (0x%04x) failed: used=%d + size=%d > total=%d\n"

#define MM_ERR_INVALID_WIDTH_PITCH_CODE 0x00036u
#define MM_ERR_INVALID_WIDTH_PITCH_MSG  "MM:: width=%d need to be less than pitch=%d\n"

#define MM_ERR_INVALID_DBP_USED_CODE 0x00037u
#define MM_ERR_INVALID_DBP_USED_MSG  "MM:: dbp_used(%d) >= MAX_DBP_NUM(%d)\n"

#define MM_ERR_INVALID_DBUF_PITCH_CODE 0x00038u
#define MM_ERR_INVALID_DBUF_PITCH_MSG  "MM:: dbuf_pitch=%d need to be 32B aligned\n"

#define MM_ERR_INVALID_DBUF_WIDTH_CODE 0x00039u
#define MM_ERR_INVALID_DBUF_WIDTH_MSG  "MM:: width=%d need to be less than pitch=%d\n"

#define MM_ERR_INVALID_CBUF_USED_CODE 0x0003au
#define MM_ERR_INVALID_CBUF_USED_MSG  "MM::  maximum cirbuf %d reached!\n"

#define MM_ERR_INVALID_CBUF_SIZE_CODE 0x0003bu
#define MM_ERR_INVALID_CBUF_SIZE_MSG  "MM:: cbuf_size(%d) need to be multiple of dma_width(%d)\n"

#define MM_ERR_INVALID_READER_USED_CODE 0x0003cu
#define MM_ERR_INVALID_READER_USED_MSG  "MM:: reader_used=%d >= MAX_CIRBUF_READER(%d)\n"

#define MM_ERR_INVALID_READER_ID_CODE 0x0003du
#define MM_ERR_INVALID_READER_ID_MSG  "MM:: cbuf_id=%08x reader_id=%d, rdr_used=%d\n"

#define MM_ERR_CBUF_RD_NUM_BYTES_CODE 0x0003eu
#define MM_ERR_CBUF_RD_NUM_BYTES_MSG  "MM:: cbuf_id=%08x num_bytes=%d, num_bytes[%d]=%d\n"

#define MM_ERR_INVALID_BDT_USED_CODE 0x0003fu
#define MM_ERR_INVALID_BDT_USED_MSG  "MM:: bdt_used(%d) >= max_bdt_num(%d)\n"

#define MM_ERR_INVALID_BD_NUM_CODE 0x00040u
#define MM_ERR_INVALID_BD_NUM_MSG  "MM:: bd_used(%d) + bd_mum(%d) > max_bd_num(%d)\n"

#define MM_ERR_INVALID_PADDED_WIDTH_CODE 0x00041u
#define MM_ERR_INVALID_PADDED_WIDTH_MSG  "MM:: bd_id=0x%08x SWIDTH_2_SPITCH(%d) > padded_width(%d)\n"

#define MM_ERR_INVALID_HEAP_SIZE_CODE 0x00042u
#define MM_ERR_INVALID_HEAP_SIZE_MSG  "MM:: alloc_heap() failed, aligned_size=%d + used=%d < HEAP_SIZE=%d\n"

#define MM_ERR_UNDEFINED_AES_SYMBOL_CODE 0x00043u
#define MM_ERR_UNDEFINED_AES_SYMBOL_MSG  "MM:: AES symbol should be defined! \n"

#define MM_ERR_AES_FAILED_EXIT_CODE 0x00044u
#define MM_ERR_AES_FAILED_EXIT_MSG  "MM:: aes_main: failed exit=%d\n"

#define MM_ERR_AES_LEN_ALIGN_CODE 0x00045u
#define MM_ERR_AES_LEN_ALIGN_MSG  "length %d is not 16 align\n"

#define MM_ERR_STACK_OVERFLOW_CODE 0x00046u
#define MM_ERR_STACK_OVERFLOW_MSG  "stack overflow, bottom:0x%x 4-word:0x%08x 0x%08x 0x%08x 0x%08x\n"

#define MM_ERR_UTEST_CODE 0x00047u
#define MM_ERR_UTEST_MSG  "MM:: utest error arg0:%d arg1:%d arg2:%d arg3:%d arg4:%d\n"

#define MM_ERR_SMEM_2_DRAM_ALIGN_CODE 0x00048u
#define MM_ERR_SMEM_2_DRAM_ALIGN_MSG                                           \
  "smem_2_dram_copy_one2many: smem_addr (0x%x) does not align to 16 || "       \
  "out_width (%d) is not a multiple of 4 || out_pitch (%d) is not a multiple " \
  "of 32\n"

#define MM_ERR_BAD_BDT_ID_CODE 0x00049u
#define MM_ERR_BAD_BDT_ID_MSG                                          \
  "Detected bad buffer descriptor table ID 0x%x, debugging value1:%u " \
  "value2:%u value3:%u value4:%u\n"

#define MM_ERR_WRITE_DRAM_OUTOF_RANGE_CODE 0x0004au
#define MM_ERR_WRITE_DRAM_OUTOF_RANGE_MSG  "MM:: write dram out of range address 0x%x, size %d\n"

#define MM_ERR_NUM 0x4bu

extern const char * const mm_err_code[MM_ERR_NUM];

/*****************************************************************************
 * module_idx 1 ERR_CODE_MOD_API
 *****************************************************************************/
#define API_ERR_UNKNOWN_CODE 0x10000u
#define API_ERR_UNKNOWN_MSG  "API:: unknown error arg0:%d arg1:%d arg2:%d arg3:%d arg4:%d\n"

#define API_ERR_INTER_MSG_HOLD_MODE_CODE 0x10001u
#define API_ERR_INTER_MSG_HOLD_MODE_MSG  "API:: internal_msg_holder_mode=%d is not valid.\n"

#define API_ERR_DSP_INIT_DATA_CODE 0x10002u
#define API_ERR_DSP_INIT_DATA_MSG  "API:: init_dsp_init_data failed, msg base_daddr=0x%08x max_num_msg=%d.\n"

#define API_ERR_INVALID_DSP_FP_TYPE_CODE 0x10003u
#define API_ERR_INVALID_DSP_FP_TYPE_MSG  "API:: invalid dsp_fp_type::%d\n"

#define API_ERR_WITHOUT_MUTEX_CODE 0x10004u
#define API_ERR_WITHOUT_MUTEX_MSG  "API:: accessing critical section without holding mutex\n"

#define API_ERR_INVALID_MSG_INFO_TBL_CODE 0x10005u
#define API_ERR_INVALID_MSG_INFO_TBL_MSG  "API:: invalid msg_info_tbl[%d]==NULL(0x%x).\n"

#define API_ERR_INVALID_CMD_API_ID_CODE 0x10006u
#define API_ERR_INVALID_CMD_API_ID_MSG  "API:: cmd_api_id=%d out of range.\n"

#define API_ERR_INVALID_CMD_INFO_TBL_CODE 0x10007u
#define API_ERR_INVALID_CMD_INFO_TBL_MSG  "API:: invalid cmd_info_tbl[%d]=0x%x cmd_dram_addr=0x%x.\n"

#define API_ERR_MSG_QUEUE_FULL_CODE 0x10008u
#define API_ERR_MSG_QUEUE_FULL_MSG  "API:: msg queue full. rd=%d wr=%d max_num_msg=%d\n"

#define API_ERR_INVALID_MSG_API_ID_CODE 0x10009u
#define API_ERR_INVALID_MSG_API_ID_MSG  "API:: msg_api_id=%d out of range.\n"

#define API_ERR_INVALID_CNT_VAL_CODE 0x1000au
#define API_ERR_INVALID_CNT_VAL_MSG  "API:: cmd_api_id=%d wrong counter_value %d.\n"

#define API_ERR_TIME_OUT_CODE 0x1000bu
#define API_ERR_TIME_OUT_MSG  "API:: time out, cmd_api_id=%d.\n"

#define API_ERR_HEADER_CMD_CODE_CODE 0x1000cu
#define API_ERR_HEADER_CMD_CODE_MSG  "API:: invalid header command_code=0x%08x, CMD_DSP_HEADER=0x%08x\n"

#define API_ERR_INVALID_CMD_SEQ_CODE 0x1000du
#define API_ERR_INVALID_CMD_SEQ_MSG  "API:: wrong cmd_seq 0x%x prev_seq 0x%x\n"

#define API_ERR_TOO_MANY_ENTRIES_CODE 0x1000eu
#define API_ERR_TOO_MANY_ENTRIES_MSG  "API:: too many outstanding entries=%d > %d\n"

#define API_ERR_INVALID_DSP_CMD_CODE_CODE 0x1000fu
#define API_ERR_INVALID_DSP_CMD_CODE_MSG  "API:: unsupport DSP command.code=0x%08x\n"

#define API_ERR_BAD_ECHO_CMD_PIDX_CODE 0x10010u
#define API_ERR_BAD_ECHO_CMD_PIDX_MSG  "API:: invalid echo cmd cache idx, idx:max=%d:%d\n"

#define API_ERR_NUM 0x11u

extern const char * const api_err_code[API_ERR_NUM];

/*****************************************************************************
 * module_idx 2 ERR_CODE_MOD_MSG
 *****************************************************************************/
#define MSG_ERR_UNKNOWN_CODE 0x20000u
#define MSG_ERR_UNKNOWN_MSG  "MSG:: unknown error arg0:%d arg1:%d arg2:%d arg3:%d arg4:%d\n"

#define MSG_ERR_INVALID_ORC_ALL_CODE 0x20001u
#define MSG_ERR_INVALID_ORC_ALL_MSG  "MSG:: msg_q_id(0x%04x), orc_all is zero!\n"

#define MSG_ERR_Q_TYPE_ORC_ALL_CODE 0x20002u
#define MSG_ERR_Q_TYPE_ORC_ALL_MSG  "MSG:: queue_type=%d and orc_all=%d are not allowed!\n"

#define MSG_ERR_Q_TYPE_ENTRY_SIZE_CODE 0x20003u
#define MSG_ERR_Q_TYPE_ENTRY_SIZE_MSG  "MSG:: invalid input queue_type=%d entry_size=%d entry_num=%d\n"

#define MSG_ERR_NO_MORE_QUEUES_CODE 0x20004u
#define MSG_ERR_NO_MORE_QUEUES_MSG  "MSG:: no more queues next_q=%d max_qs=%d orc_all=%u! Check max_orccode_msg_qs or max_orc_all_msg_qs in CMD_DSP_CONFIG\n"

#define MSG_ERR_INVALID_MSG_Q_IDX_CODE 0x20005u
#define MSG_ERR_INVALID_MSG_Q_IDX_MSG  "MSG:: msg_q_id(0x%04x): invalid msg_q_idx=%d >= %d\n"

#define MSG_ERR_QUEUE_IS_FULL_CODE 0x20006u
#define MSG_ERR_QUEUE_IS_FULL_MSG  "MSG:: msg_q_id(0x%04x): queue is full =%d >= %d\n"

#define MSG_ERR_NUM 0x7u

extern const char * const msg_err_code[MSG_ERR_NUM];

/*****************************************************************************
 * module_idx 3 ERR_CODE_MOD_BOOT
 *****************************************************************************/
#define BOOT_ERR_UNKNOWN_CODE 0x30000u
#define BOOT_ERR_UNKNOWN_MSG  "BOOT:: unknown error arg0:%d arg1:%d arg2:%d arg3:%d arg4:%d\n"

#define BOOT_ERR_DSP_DEBUG_DSIZE_CODE 0x30001u
#define BOOT_ERR_DSP_DEBUG_DSIZE_MSG  "BOOT:: init_data.dsp_debug_dsize=%d is not multiple of %d.\n"

#define BOOT_ERR_CHIP_ID_DADDR_CODE 0x30002u
#define BOOT_ERR_CHIP_ID_DADDR_MSG  "BOOT:: init_data->is_prt_opt=1 but init_data->chip_id_daddr 0x%x ??? \n"

#define BOOT_ERR_DELTA_AUDIO_TICKS_CODE 0x30003u
#define BOOT_ERR_DELTA_AUDIO_TICKS_MSG  "BOOT:: init_data->is_prt_opt=1 but init_data->delta_audio_ticks 0x%x \n"

#define BOOT_ERR_DSP_PROF_ID_CODE 0x30004u
#define BOOT_ERR_DSP_PROF_ID_MSG  "BOOT:: TBD -- dsp_prof_id=%d\n"

#define BOOT_ERR_FIRST_DEF_CMD_CODE 0x30005u
#define BOOT_ERR_FIRST_DEF_CMD_MSG  "BOOT: First default command must be 0x%08x but 0x%08x\n"

#define BOOT_ERR_ORC_MSG_T_SIZE_CODE 0x30006u
#define BOOT_ERR_ORC_MSG_T_SIZE_MSG  "BOOT:: orc_msg_t=%d > ENG0_MSG_ENTRY_SIZE=%d !\n"

#define BOOT_ERR_CALC_SBLK_FAILED_CODE 0x30007u
#define BOOT_ERR_CALC_SBLK_FAILED_MSG  "BOOT:: calc_sblk: failed to get sblk for %u"

#define BOOT_ERR_UNKNOWN_VALUES_CODE 0x30008u
#define BOOT_ERR_UNKNOWN_VALUES_MSG  "BOOT:: unknown values: dram_intlv=%d page_val=%d\n"

#define BOOT_ERR_INVALID_IN_IDX_CODE 0x30009u
#define BOOT_ERR_INVALID_IN_IDX_MSG  "BOOT:: flow_tag(%d) in_idx=%d q_in_num=%d\n"

#define BOOT_ERR_INVALID_OUT_IDX_CODE 0x3000au
#define BOOT_ERR_INVALID_OUT_IDX_MSG  "BOOT:: flow_tag(%d) out_idx=%d q_out_num=%d\n"

#define BOOT_ERR_NUM 0xbu

extern const char * const boot_err_code[BOOT_ERR_NUM];

/*****************************************************************************
 * module_idx 4 ERR_CODE_MOD_IENG
 *****************************************************************************/
/* IENG unknown error: only shown in unit tests assertion */
#define IENG_ERR_UNKNOWN_CODE 0x40000u
#define IENG_ERR_UNKNOWN_MSG \
  "IENG: unknown error arg0:%d arg1:%d arg2:%d arg3:%d arg4:%d\n"

/* IENG bad value error: runtime unexpected value */
#define IENG_ERR_BAD_VALUE_CODE 0x40001u
#define IENG_ERR_BAD_VALUE_MSG                                             \
  "IENG: detected a bad value:%u debugging value1:%u value2:%u value3:%u " \
  "value4:%u\n"

/* IENG IDSP load stream error: too many idsp load streams */
#define IENG_ERR_IDSP_LD_STR_CODE 0x40002u
#define IENG_ERR_IDSP_LD_STR_MSG \
  "IENG: run out of mcb for idsp load streams index:%u used:%u max:%u\n"

#define IENG_ERR_NUM 0x3u

extern const char * const ieng_err_code[IENG_ERR_NUM];

/*****************************************************************************
 * module_idx 5 ERR_CODE_MOD_VPROC
 *****************************************************************************/
#define VPROC_ERR_UNKNOWN_CODE 0x50000u
#define VPROC_ERR_UNKNOWN_MSG "VPROC: unknown error arg0:%d arg1:%d arg2:%d arg3:%d arg4:%d\n"

/* VPROC bad value error: runtime unexpected value */
#define VPROC_ERR_BAD_VALUE_CODE 0x50001u
#define VPROC_ERR_BAD_VALUE_MSG "VPROC: bad value error arg0:0x%x arg1:0x%x arg2:0x%x arg3:0x%x arg4:0x%x\n"

/* VPROC cmds error */
#define VPROC_ERR_SETUP_CMD_CODE 0x50002u
#define VPROC_ERR_SETUP_CMD_MSG \
"VPROC: vproc_proc_vproc_setup error arg0:0x%x arg1:0x%x arg2:0x%x arg3:0x%x arg4:0x%x\n"

#define VPROC_ERR_FLOW_MAX_CFG_CODE 0x50003u
#define VPROC_ERR_FLOW_MAX_CFG_MSG \
  "VPROC: CMD_DSP_VPROC_FLOW_MAX_CFG failed check! check index:%u value:%u\n"

#define VPROC_ERR_CFG_CMD_CODE 0x50004u
#define VPROC_ERR_CFG_CMD_MSG \
  "VPROC[chan_%u]: CMD_VPROC_CONFIG failed check! check index:%u value:%u\n"

#define VPROC_ERR_IMG_PYR_CMD_CODE 0x50005u
#define VPROC_ERR_IMG_PYR_CMD_MSG \
"VPROC: vproc_proc_img_pyramid_setup error arg0:0x%x arg1:0x%x arg2:0x%x arg3:0x%x arg4:0x%x\n"

#define VPROC_ERR_PREV_CMD_CODE 0x50006u
#define VPROC_ERR_PREV_CMD_MSG \
"VPROC: vproc_proc_vproc_prev_setup error arg0:0x%x arg1:0x%x arg2:0x%x arg3:0x%x arg4:0x%x\n"

#define VPROC_ERR_WARP_CMD_CODE 0x50007u
#define VPROC_ERR_WARP_CMD_MSG \
  "VPROC[chan_%u]: CMD_VPROC_WARP_GROUP_UPDATE failed check! check index:%u value:%u\n"

#define VPROC_ERR_MULT_STRM_PP_CODE 0x50008u
#define VPROC_ERR_MULT_STRM_PP_MSG                                          \
"VPROC: vproc_proc_multi_stream_pp error arg0:0x%x arg1:0x%x arg2:0x%x arg3:0x%x arg4:0x%x\n"

#define VPROC_ERR_SET_EF_BUF_IMG_CODE 0x50009u
#define VPROC_ERR_SET_EF_BUF_IMG_MSG                                        \
"VPROC: vproc_proc_set_effect_buf_img_sz error arg0:0x%x arg1:0x%x arg2:0x%x arg3:0x%x arg4:0x%x\n"

#define VPROC_ERR_VPROC_GRPING_CODE 0x5000au
#define VPROC_ERR_VPROC_GRPING_MSG                                           \
"VPROC: vproc_proc_set_vproc_grping error arg0:0x%x arg1:0x%x arg2:0x%x arg3:0x%x arg4:0x%x\n"

#define VPROC_ERR_MC_PROC_ORDER_CODE 0x5000bu
#define VPROC_ERR_MC_PROC_ORDER_MSG                                         \
"VPROC: vproc_proc_multi_chan_proc_order error arg0:0x%x arg1:0x%x arg2:0x%x arg3:0x%x arg4:0x%x\n"

#define VPROC_ERR_SET_EXT_MEM_CODE 0x5000cu
#define VPROC_ERR_SET_EXT_MEM_MSG                                            \
"VPROC: vproc_proc_vproc_set_ext_mem error arg0:0x%x arg1:0x%x arg2:0x%x arg3:0x%x arg4:0x%x\n"

#define VPROC_ERR_PIN_OUT_DECI_CODE 0x5000du
#define VPROC_ERR_PIN_OUT_DECI_MSG                                           \
"VPROC: vproc_proc_vproc_pin_out_deci error arg0:0x%x arg1:0x%x arg2:0x%x arg3:0x%x arg4:0x%x\n"

#define VPROC_ERR_LN_DEC_SETUP_CODE 0x5000eu
#define VPROC_ERR_LN_DEC_SETUP_MSG                                           \
"VPROC: vproc_proc_vproc_lane_det_setup error arg0:0x%x arg1:0x%x arg2:0x%x arg3:0x%x arg4:0x%x\n"

#define VPROC_ERR_GRP_CMD_CODE 0x5000fu
#define VPROC_ERR_GRP_CMD_MSG                                                \
"VPROC: vproc_proc_vproc_grp_cmd error arg0:0x%x arg1:0x%x arg2:0x%x arg3:0x%x arg4:0x%x\n"

#define VPROC_ERR_BOUNDING_CHECK_CODE 0x50010u
#define VPROC_ERR_BOUNDING_CHECK_MSG \
  "VPROC: bounding check fail (silicon_version:%d): vproc throughput (K):%d over limit:%d (K) - margin:%d (%)\n"

#define VPROC_ERR_INPUT_FID_CODE 0x50011u
#define VPROC_ERR_INPUT_FID_MSG                                             \
  "VPROC[chan_%u]: vproc_check_input_fbs input_format:%u is_ce_enabled:%u " \
  "pic_fid:0x%08x me_pic_fid2:0x%08x invalid\n"

#define VPROC_ERR_NOT_ENOUGH_SMEM_CODE 0x50012u
#define VPROC_ERR_NOT_ENOUGH_SMEM_MSG "VPROC[chan_%d]: not enough smem, short %d\n"

#define VPROC_ERR_NOT_ENOUGH_DRAM_CODE 0x50013u
#define VPROC_ERR_NOT_ENOUGH_DRAM_MSG "VPROC[chan_%d]: not enough dram, short %d\n"

#define VPROC_ERR_GRP_INFO_CODE 0x50014u
#define VPROC_ERR_GRP_INFO_MSG "VPROC[chan_%u]: vproc_load_grp_info " \
                               "gpr_id:%d mismatch info->grp_id:%d\n"

#define VPROC_ERR_BAT_PROC_ORDER_CODE 0x50015u
#define VPROC_ERR_BAT_PROC_ORDER_MSG "VPROC[chan_%d]: CMD_VPROC_MULTI_CHAN_PROC_ORDER(0x%x) "\
                                     "MUST BE placed in master channel batch cmd set, "\
                                     "proc_idx:%d\n"

#define VPROC_ERR_SMEM_WIN_WIDTH_CODE 0x50016u
#define VPROC_ERR_SMEM_WIN_WIDTH_MSG "VPROC[chan_%d]: SMEM_WIN width:%d out of range, "\
                                     "padded_width:%d\n"

#define VPROC_ERR_NULL_IK_ADDR_CODE 0x50017u
#define VPROC_ERR_NULL_IK_ADDR_MSG \
  "VPROC[chan_%u]: null ik addr, check CMD_VPROC_IK_CONFIG\n"

#define VPROC_ERR_INVALID_FB_ID_CODE 0x50018u
#define VPROC_ERR_INVALID_FB_ID_MSG \
  "VPROC[chan_%u]: invalid frame buffer ID, fbp_id:0x%x\n"

#define VPROC_ERR_ABNORMAL_TILE_CODE 0x50019u
#define VPROC_ERR_ABNORMAL_TILE_MSG \
  "VPROC[chan_%u]: detected abnormal tile passed:%u failed:%u\n"

#define VPROC_ERR_UNHANDLED_CMD_CODE 0x5001au
#define VPROC_ERR_UNHANDLED_CMD_MSG "VPROC:: unhandled VPROC command:0x%08x\n"

#define VPROC_ERR_IN_Q_ID_CODE 0x5001bu
#define VPROC_ERR_IN_Q_ID_MSG "VPROC: vproc_get_input_q_id invalid argument " \
                              "fp_num:%u bind_msg:%u ch_num:%u str_num:%u\n"

#define VPROC_ERR_OUT_PIN_CODE 0x5001cu
#define VPROC_ERR_OUT_PIN_MSG "VPROC: vproc_get_output_pin_addr invalid argument " \
                              "dsp_fp_type:%u bind_msg:%u src_fp_num:%u src_ch_num:%u src_str_num:%u\n"


#define VPROC_ERR_NUM 0x1du

extern const char * const vproc_err_code[VPROC_ERR_NUM];


/*****************************************************************************
 * module_idx 7 ERR_CODE_MOD_VIN
 *****************************************************************************/
#define EM_VIN_ERR_CODE                 "VIN[%d]: ERROR! err_code=0x%08x\n"

#define VIN_ERR_UNKNOWN_CODE            0x70000u
#define VIN_ERR_UNKNOWN_MSG             "VIN: unknown error arg0:%d arg1:%d arg2:%d arg3:%d arg4:%d\n"

#define EC_VIN_API                      0x70001u
#define EM_VIN_API                      "VIN[%d]: API ERROR! cmd_code=0x%08x\n"

#define EC_VIN_BAD_VIN_THREAD_ID        0x70002u
#define EM_VIN_BAD_VIN_THREAD_ID        EM_VIN_ERR_CODE //\\"VIN: invalid vin_thread_id:%d!\n"

#define EC_VIN_NULL_CMD_INFO_TBL        0x70003u
#define EM_VIN_NULL_CMD_INFO_TBL        EM_VIN_ERR_CODE //\\"VIN: null cmd_info_tbl!\n"

#define EC_VIN_ASSERT_CHECK_AES         0x70004u
#define EM_VIN_ASSERT_CHECK_AES         "VIN: assert check aes encrypt/decrypt if the same\n"

#define EC_VIN_SC_CHECK_FAIL            0x70005u
#define EM_VIN_SC_CHECK_FAIL            "VIN: vin_main() sc error\n"

#define EC_VIN_MISMATCH_VIN_ID          0x70006u
#define EM_VIN_MISMATCH_VIN_ID          "VIN: mismatch vin_id(%d:%d)\n"

#define EC_VIN_BAD_YUV_ENC_FBP_ID       0x70007u
#define EM_VIN_BAD_YUV_ENC_FBP_ID       EM_VIN_ERR_CODE //\\"VIN[%d]: invalid vin_yuv_enc_fbp_id!\n"

#define EC_VIN_BAD_FB_ID                0x70008u
#define EM_VIN_BAD_FB_ID                "VIN[%d]:: invalid fb_id(0x%x)\n"

#define EC_VIN_BAD_SDBD_INFO_FOV_ID     0x70009u
#define EM_VIN_BAD_SDBD_INFO_FOV_ID     EM_VIN_ERR_CODE //\\"VIN[%d]: invalid sideband_info fov_id:%d max_fov_num:%d\n"

#define EC_VIN_NULL_CMD_CHAN_DADDR      0x7000au
#define EM_VIN_NULL_CMD_CHAN_DADDR      EM_VIN_ERR_CODE //\\"VIN: null cmd_dram_addr:0x%x!\n"

#define EC_VIN_SHORT_OF_DRAM            0x7000bu
#define EM_VIN_SHORT_OF_DRAM            "VIN: not enough dram, short %d\n"

#define EC_VIN_SHORT_OF_SMEM            0x7000cu
#define EM_VIN_SHORT_OF_SMEM            "VIN: not enough smem, short %d\n"

#define EC_VIN_CAP_RM_INIT_NOT_SET      0x7000du
#define EM_VIN_CAP_RM_INIT_NOT_SET      "VIN[%d]: is_vin_cap_rm_init:%d should be set!\n"

#define EC_VIN_SENSOR_RAW_OUT_NOT_EN    0x7000eu
#define EM_VIN_SENSOR_RAW_OUT_NOT_EN    "VIN[%d]: is_sensor_raw_out_enabled:%d should be set!\n"

#define EC_VIN_CE_OUT_NOT_EN            0x7000fu
#define EM_VIN_CE_OUT_NOT_EN            "VIN[%d]: is_contrast_enhance_out_enabled:%d should be set!\n"

#define EC_VIN_YUV422_OUT_NOT_EN        0x70010u
#define EM_VIN_YUV422_OUT_NOT_EN        EM_VIN_ERR_CODE //\\"VIN[%d]: is_yuv422_out_enabled:%d should be set!\n"

#define EC_VIN_SHORT_OF_DATA_SMEM       0x70011u
#define EM_VIN_SHORT_OF_DATA_SMEM       "VIN: not enough data smem, short %d\n"

#define EC_VIN_MISMATCH_IDSP_STR_ID     0x70012u
#define EM_VIN_MISMATCH_IDSP_STR_ID     EM_VIN_ERR_CODE //\\"VIN: mismatch idsp_str_id(%d:%d)\n"

#define EC_VIN_NULL_FOV_FBP_IDS         0x70013u
#define EM_VIN_NULL_FOV_FBP_IDS         EM_VIN_ERR_CODE //\\"VIN[%d]: null vin fov_fbp_ids!\n"

#define EC_VIN_NON_ZERO_RESET_CNT       0x70014u
#define EM_VIN_NON_ZERO_RESET_CNT       EM_VIN_ERR_CODE //\\"VIN: non-zero vin_reset_cnt:%d!\n"

#define EC_VIN_NULL_DATA_Q_OUT          0x70015u
#define EM_VIN_NULL_DATA_Q_OUT          EM_VIN_ERR_CODE //\\"VIN[%d]: null data_q_out! Check CMD_DSP_CONFIG vin[0-5]_output_pins or CMD_VIN_START output_dest\n"

#define EC_VIN_MISMATCH_BATCH_ADDR      0x70016u
#define EM_VIN_MISMATCH_BATCH_ADDR      EM_VIN_ERR_CODE //\\"VIN[%d]: mismatch batch_cmd_set_addr(cache):0x%x and batch_cmd_set_addr(cap_info):0x%x\n"

#define EC_VIN_MISMATCH_BATCH_ID        0x70017u
#define EM_VIN_MISMATCH_BATCH_ID        EM_VIN_ERR_CODE //\\"VIN[%d]: mismatch batch_cmd_set_id(cache):0x%x and batch_cmd_set_id(cap_info):0x%x\n"

#define EC_VIN_MISMATCH_BATCH_SZ        0x70018u
#define EM_VIN_MISMATCH_BATCH_SZ        EM_VIN_ERR_CODE //\\"VIN[%d]: mismatch batch_cmd_set_size(cache):%d and batch_cmd_set_size(cap_info):%d\n"

#define EC_VIN_BAD_PIC_REPEAT_CNT       0x70019u
#define EM_VIN_BAD_PIC_REPEAT_CNT       "VIN[%d]: invalid pic repeat_cnt:0x%x m:%d n:%d!\n"

#define EC_VIN_BAD_SRC_FP_NUM           0x7001au
#define EM_VIN_BAD_SRC_FP_NUM           EM_VIN_ERR_CODE //\\"VIN: invalid src_fp_num(vin_id):%d vin_bit_mask:%d\n"

#define EC_VIN_BAD_VPROC_BINDING_NUM    0x7001bu
#define EM_VIN_BAD_VPROC_BINDING_NUM    EM_VIN_ERR_CODE //\\"VIN: vin_vproc_binding_num:%d > vproc_num:%d! Check vproc num in CMD_DSP_CONFIG or CMD_DSP_BINDING_CFG num!\n"

#define EC_VIN_BAD_ENC_BINDING_NUM      0x7001cu
#define EM_VIN_BAD_ENC_BINDING_NUM      EM_VIN_ERR_CODE //\\"VIN: vin_enc_binding_num:%d > enc_num:%d! Check enc num in CMD_DSP_CONFIG or CMD_DSP_BINDING_CFG num!\n"

#define EC_VIN_BAD_ENC_BIND_SRC_CH_NUM  0x7001du
#define EM_VIN_BAD_ENC_BIND_SRC_CH_NUM  EM_VIN_ERR_CODE //\\"VIN: vin->enc channel num:%d\n"

#define EC_VIN_BAD_DST_FP_TYPE          0x7001eu
#define EM_VIN_BAD_DST_FP_TYPE          EM_VIN_ERR_CODE //\\"VIN: invalid dst_fp_type:%d\n"

#define EC_VIN_BAD_BIND_MSG             0x7001fu
#define EM_VIN_BAD_BIND_MSG             EM_VIN_ERR_CODE //\\"VIN: vin has not assigned a out msg q yet!\n"

#define EC_VIN_BAD_BIND_SRC_CH_NUM      0x70020u
#define EM_VIN_BAD_BIND_SRC_CH_NUM      EM_VIN_ERR_CODE //\\"VIN: data_q_out src_ch_num (%d) >= data_q_out_num (%d) !\n"

#define EC_VIN_RUN_OUT_EXT_YUV_FB       0x70021u
#define EM_VIN_RUN_OUT_EXT_YUV_FB       EM_VIN_ERR_CODE //\\"VIN[%u]: vin_yuv422_req_ext_output_buf run out ext_mem FB\n"

#define EC_VIN_WRONG_DRAM               0x70022u
#define EM_VIN_WRONG_DRAM               "VIN: wrong dram, daddr 0x%x\n"

#define VIN_ERR_NUM 0x23u

extern const char * const vin_err_code[VIN_ERR_NUM];

/*****************************************************************************
 * module_idx 8 ERR_CODE_MOD_PREV
 *****************************************************************************/
#define PREV_ERR_UNKNOWN_CODE 0x80000u
#define PREV_ERR_UNKNOWN_MSG "PREV: unknown error arg0:%d arg1:%d arg2:%d arg3:%d arg4:%d\n"

#define PREV_ERR_NUM 0x1u

extern const char * const prev_err_code[PREV_ERR_NUM];

/*****************************************************************************
 * module_idx 9 ERR_CODE_MOD_PROC_HL
 *****************************************************************************/
/* HLP unknown error: only shown in unit tests assertion */
#define HLP_ERRC_UNKNOWN 0x90000u
#define HLP_ERRM_UNKNOWN \
  "HL_PROC: unknown error arg0:%d arg1:%d arg2:%d arg3:%d arg4:%d\n"

/* HLP bad value error: runtime unexpected value */
#define HLP_ERRC_BAD_VALUE 0x90001u
#define HLP_ERRM_BAD_VALUE                                                     \
  "HL_PROC: detected a bad value:%u debugging value1:%u value2:%u value3:%u  " \
  "value4:%u\n"

/* HLP write queue full error: data producer runs faster than consumer */
#define HLP_ERRC_WRITEQ_LWQ_FULL 0x90002u
#define HLP_ERRM_WRITEQ_LWQ_FULL \
  "HL_PROC[chan_%d]: unable to add data to a full queue\n"

/* HLP external memory buffer error: run out external memory frame buffers */
#define HLP_ERRC_REQEXT_RUNOUTFB 0x90003u
#define HLP_ERRM_REQEXT_RUNOUTFB \
  "HL_PROC[chan_%d]: run out external memory frame buffers\n"

/* HLP set external memory error: unexpected external memory parameter */
#define HLP_ERRC_SET_EXT_MEM 0x90004u
#define HLP_ERRM_SET_EXT_MEM                                                \
  "HL_PROC[chan_%d]: set external memory fatal error! value1:%u value2:%u " \
  "value3:%u value4:%u\n"

/* HLP compression config error: unexpected compression parameter */
#define HLP_ERRC_CMPR_CFG 0x90005u
#define HLP_ERRM_CMPR_CFG \
  "HL_PROC[chan_%d]: invalid compression config block_sz:%d mantissa:%d\n"

#define HLP_ERR_NUM 0x6u

extern const char* const hl_proc_err_code[HLP_ERR_NUM];

/*****************************************************************************
 * module_idx 10 ERR_CODE_MOD_VOUT
 *****************************************************************************/
#define ERR_CODE_INVALID_PREV_VOUT_SRC 0xa0000u
#define ERR_MSG_INVALID_PREV_VOUT_SRC "VOUT: Invalid previous Vout source %d\n"

#define ERR_CODE_INVALID_NEW_VOUT_SRC 0xa0001u
#define ERR_MSG_INVALID_NEW_VOUT_SRC "VOUT: Invalid new Vout source %d\n"

#define ERR_CODE_VOUT_SRC_WHEN_PREP 0xa0002u
#define ERR_MSG_VOUT_SRC_WHEN_PREP "VOUT: Invalid Vout source %d. vout_id=%d\n"

#define ERR_CODE_WRONG_NEXT_VOUT_ID 0xa0003u
#define ERR_MSG_WRONG_NEXT_VOUT_ID "VOUT: context_switch_to_vout() Wrong next vout id ! \n"

#define ERR_CODE_WRONG_COUNTER_VAL 0xa0004u
#define ERR_MSG_WRONG_COUNTER_VAL "VOUT: vout_wait_for_vin_sync() wrong counter_value: %d. \n"

#define ERR_CODE_NEXT_FRM_A_TOO_LARGE 0xa0005u
#define ERR_MSG_NEXT_FRM_A_TOO_LARGE "VOUT:: vout_handle_multi_vout() next_frm_a:%u >= frm_rate_a_ticks:%u, vout_ts[0]:%u curr_time:%u\n"

#define ERR_CODE_NEXT_FRM_B_TOO_LARGE 0xa0006u
#define ERR_MSG_NEXT_FRM_B_TOO_LARGE "VOUT:: vout_handle_multi_vout() next_frm_b:%u >= frm_rate_b_ticks:%u, vout_ts[1]:%u curr_time:%u\n"

#define ERR_CODE_WRITE_CMD_Q_SBASE_NULL 0xa0007u
#define ERR_MSG_WRITE_CMD_Q_SBASE_NULL "VOUT-(%d) vout_write_dsp_cmd_q dsp_cmd_q_sbase is NULL\n"

#define ERR_CODE_READ_CMD_Q_SBASE_NULL 0xa0008u
#define ERR_MSG_READ_CMD_Q_SBASE_NULL "VOUT: vout_read_dsp_cmd_q dsp_cmd_q_sbase is NULL, vout_id:%d\n"

#define ERR_CODE_INVALID_VOUT_ID_IN_CMD 0xa0009u
#define ERR_MSG_INVALID_VOUT_ID_IN_CMD "VOUT: vout_dispatch_dsp_cmd() Invalid vout_id=%d in command\n"

#define ERR_CODE_VOUT_CMD_VOUT_ID_NOT_MATCH 0xa000au
#define ERR_MSG_VOUT_CMD_VOUT_ID_NOT_MATCH "VOUT:: Command vout_id:%d not match with g_vout->vout_id:%d. cmd_code:0x%08x\n"

#define ERR_CODE_VOUT_UNSUPPORTED_CSC_TYPE 0xa000bu
#define ERR_MSG_VOUT_UNSUPPORTED_CSC_TYPE "VOUT: unsupported csc type:%d (vout_id=%d)\n"

#define ERR_CODE_UNKNOW_CMD_CODE_FROM_VOUT_CMD 0xa000cu
#define ERR_MSG_UNKNOW_CMD_CODE_FROM_VOUT_CMD "VOUT:: Unknown cmd_code=0x%08x, (vout_id=%d)\n"

#define ERR_CODE_INVALID_NUM_CMDS_IN_BUF 0xa000du
#define ERR_MSG_INVALID_NUM_CMDS_IN_BUF "VOUT: vout_proc_vout_pic_info() invalid num_cmds_in_buf\n"

#define VOUT_ERR_NUM 0xeu

extern const char * const vout_err_code[VOUT_ERR_NUM];

/*****************************************************************************
 * module_idx 11 ERR_CODE_MOD_ENC
 *****************************************************************************/
#define ENC_ERRC_BAD_VALUE 0xb0000u
#define ENC_ERRM_BAD_VALUE "ENC: bad value error arg0:0x%x arg1:0x%x arg2:0x%x arg3:0x%x arg4:0x%x\n"

#define ENC_ERRC_ENC_CMEM_GP_OVERFLOW 0xb0001u
#define ENC_ERRM_ENC_CMEM_GP_OVERFLOW  "ENC: cmem_gp overflow! cmem_pos_gp=%d > ENC_CMEM_GP_SIZE=%d\n"

#define ENC_ERRC_UNKNOWN_CODING_TYPE 0xb0002u
#define ENC_ERRM_UNKNOWN_CODING_TYPE  "ENC: unknown coding_type=%d\n"

#define ENC_ERRC_API 0xb0003u
#define ENC_ERRM_API  "ENC: API ERROR! cmd_code=0x%08x\n"

#define ENC_ERRC_API_CHK 0xb0004u
#define ENC_ERRM_API_CHK  "enc_safety_chk: actual %d expect %d\n"

#define ENC_ERRC_RC_BAD_VALUE 0xb0005u
#define ENC_ERRM_RC_BAD_VALUE "RC: bad value error arg0:0x%x arg1:0x%x arg2:0x%x arg3:0x%x arg4:0x%x\n"

#define ENC_ERRC_RC_CMEM_RC_OVERFLOW 0xb0006u
#define ENC_ERRM_RC_CMEM_RC_OVERFLOW  "RC: cmem_rc running out! cmem_pos_rc=%d > ENC_CMEM_RC_SIZE(=%d)\n"

#define ENC_ERRC_VDSP_BOUNDING_CHECK 0xb0007u
#define ENC_ERRM_VDSP_BOUNDING_CHECK  "VDSP bounding check fail (silicon_version:%d): enc + dec throughput (K):(%d + %d) over limit %d (K) + margin:%d\n"

#define ENC_ERR_NUM 0x8u

extern const char * const enc_err_code[ENC_ERR_NUM];

/*****************************************************************************
 * module_idx 12 ERR_CODE_MOD_ENG0
 *****************************************************************************/
#define ENG0_ERRC_BAD_VALUE 0xc0000u
#define ENG0_ERRM_BAD_VALUE "ENG0: bad value error arg0:0x%x arg1:0x%x arg2:0x%x arg3:0x%x arg4:0x%x\n"

#define ENG0_ERRC_NOT_SUPPORTED 0xc0001u
#define ENG0_ERRM_NOT_SUPPORTED "ENG0: not supported or unknown type/case %d\n"

#define ENG0_ERRC_CMEM_GP_SIZE 0xc0002u
#define ENG0_ERRM_CMEM_GP_SIZE "ENG0: overflow! cmem_gp_pos=%d > ENG0_CMEM_GP_SIZE(=%d)\n"

#define ENG0_ERRC_BITS_INFO_SIZE 0xc0003u
#define ENG0_ERRM_BITS_INFO_SIZE "ENG0: User specified bits_info size is too big. The limit is %dx8191=%d\n"

#define ENG0_ERR_NUM 0x4u

extern const char * const eng0_err_code[ENG0_ERR_NUM];

/*****************************************************************************
 * module_idx 13 ERR_CODE_MOD_PROXY
 *****************************************************************************/
#define PROXY_ERRC_BAD_VALUE 0xd0000u
#define PROXY_ERRM_BAD_VALUE "PROXY: bad value error arg0:0x%x arg1:0x%x arg2:0x%x arg3:0x%x arg4:0x%x\n"

#define PROXY_ERRC_NOT_SUPPORTED 0xd0001u
#define PROXY_ERRM_NOT_SUPPORTED "PROXY: not supported or unknown type/case %d\n"

#define PROXY_ERRC_CMEM_GP_SIZE 0xd0002u
#define PROXY_ERRM_CMEM_GP_SIZE "PROXY: overflow! cmem_gp_pos=%d > PROXY_CMEM_GP_SIZE(=%d)\n"

#define PROXY_ERR_NUM 0x3u

extern const char * const proxy_err_code[PROXY_ERR_NUM];

/*****************************************************************************
 * module_idx 14 ERR_CODE_MOD_DEC
 *****************************************************************************/
#define DEC_ERR_UNKNOWN_CODE                 0xe0000u
#define DEC_ERR_UNKNOWN_MSG                  "DEC: unknown error arg0:%d arg1:%d arg2:%d arg3:%d arg4:%d\n"

#define DEC_ERR_CODE_MDXF_SHARE_SBUF_SHORT   0xe0001u
#define DEC_ERR_MSG_MDXF_SHARE_SBUF_SHORT    "DEC:: init_decoder failed, mdxf cmd/msg sbuf size(%d) is short\n"

#define DEC_ERR_CODE_SCHED_WORKSPACE_SHORT   0xe0002u
#define DEC_ERR_MSG_SCHED_WORKSPACE_SHORT    "DEC:: init_decoder failed, DEC_SCHED_WORKSPACE_SIZE(%d) < MAX(pic_info_t=%d, orc_msg_t=%d)\n"

#define DEC_ERR_CODE_INVALID_FP_NO           0xe0003u
#define DEC_ERR_MSG_INVALID_FP_NO            "DEC:: fp_type=%d, invalid fp_no=%d\n"

#define DEC_ERR_CODE_INVALID_MSG_Q           0xe0004u
#define DEC_ERR_MSG_INVALID_MSG_Q            "DEC:: fp_type=%d, invalid message queue, is_input=%d, ch_num=%d, str_num=%d\n"

#define DEC_ERR_CODE_INVALID_DATA_Q          0xe0005u
#define DEC_ERR_MSG_INVALID_DATA_Q           "DEC:: fp_type=%d, invalid data queue is_input=%d\n"

#define DEC_ERR_CODE_INVALID_OUT_PIN         0xe0006u
#define DEC_ERR_MSG_INVALID_OUT_PIN          "DEC:: fp_type=%d has no output pin. src_num=%d bind_msg=%d\n"

#define DEC_ERR_CODE_UNKNOWN_ORC_MSG         0xe0007u
#define DEC_ERR_MSG_UNKNOWN_ORC_MSG          "DEC:: fp_type=%d, unknown orc message 0x%x received\n"

#define DEC_ERR_CODE_UNKNOWN_DSP_CMD         0xe0008u
#define DEC_ERR_MSG_UNKNOWN_DSP_CMD          "DEC:: fp_type=%d, unknown dsp command 0x%x received\n"

#define DEC_ERR_CODE_DSP_CMD_WRONG_STATE     0xe0009u
#define DEC_ERR_MSG_DSP_CMD_WRONG_STATE      "DEC:: cmd_code=%d is received at wrong state, op_mode=%d\n"

#define DEC_ERR_CODE_PROXY_PIC_UNDERFLOW     0xe000au
#define DEC_ERR_MSG_PROXY_PIC_UNDERFLOW      "DEC:: no decoder proxy pending pictures when jobs is done\n"

#define DEC_ERR_FLOW_MAX_CFG_CODE            0xe000bu
#define DEC_ERR_FLOW_MAX_CFG_MSG             "DEC: CMD_DSP_DEC_FLOW_MAX_CFG failed check! check index:%u value:%u\n"

// avc decoder
#define DEC_ERR_CODE_PICS_IN_HCB_MISMATCH    0xe0010u
#define DEC_ERR_MSG_PICS_IN_HCB_MISMATCH     "AVCDEC:: AVCDEC_MAX_PICS_IN_HCB(%d) <>  PIC_NUM_IN_HCB_MAX(%d)\n"

#define DEC_ERR_CODE_SPS_STORAGE_NOT_ALIGN   0xe0011u
#define DEC_ERR_MSG_SPS_STORAGE_NOT_ALIGN    "AVCDEC:: sizeof(avcdec_sps_storage_t)=%d is not multiple of 32B\n" 

#define DEC_ERR_CODE_PPS_STORAGE_NOT_ALIGN   0xe0012u
#define DEC_ERR_MSG_PPS_STORAGE_NOT_ALIGN    "AVCDEC:: sizeof(avcdec_pps_storage_t)=%d is not multiple of 32B\n"

#define DEC_ERR_CODE_SH_STORAGE_NOT_ALIGN    0xe0013u
#define DEC_ERR_MSG_SH_STORAGE_NOT_ALIGN     "AVCDEC:: sizeof(avcdec_sh_storage_t)=%d is not multiple of 32B\n"

#define DEC_ERR_CODE_RECON_CMD_NOT_ALIGN     0xe0014u
#define DEC_ERR_MSG_RECON_CMD_NOT_ALIGN      "AVCDEC:: sizeof(avcdec_recon_cmd_t)=%d is not multiple of 32B\n"

#define DEC_ERR_CODE_RECON_MSG_NOT_ALIGN     0xe0015u
#define DEC_ERR_MSG_RECON_MSG_NOT_ALIGN      "AVCDEC:: sizeof(avcdec_recon_msg_t)=%d is not multiple of 32B\n"

#define DEC_ERR_CODE_AVCDEC_EXCEED_MAX_NUM   0xe0016u
#define DEC_ERR_MSG_AVCDEC_EXCEED_MAX_NUM    "AVCDEC:: create_avc_decoder() failed: avcdec_used=%d >= %d\n"

#define DEC_ERR_CODE_ASCENDING_PTS_IN_BWD    0xe0017u
#define DEC_ERR_MSG_ASCENDING_PTS_IN_BWD     "AVCDEC:: poc=%d, wrong PTS in BWD, prev_pts_low=%u <= cur_pts=%u\n"

#define DEC_ERR_CODE_QUIT_ON_ERROR           0xe0018u
#define DEC_ERR_MSG_QUIT_ON_ERROR            "AVCDEC:: quits on error=0x%08x\n"

#define DEC_ERR_CODE_RECON_NOT_START_OF_PIC  0xe0019u
#define DEC_ERR_MSG_RECON_NOT_START_OF_PIC   "AVCDEC:: avcdec_ready_for_recon_pic. not a new picture, first_mb=%d, sh_idx=%d, frame_num=%d eop=%d\n"

#define DEC_ERR_CODE_RECON_PIC_IDX_MISMATCH  0xe001au
#define DEC_ERR_MSG_RECON_PIC_IDX_MISMATCH   "AVCDEC:: avcdec_ready_for_recon_pic. PPS.recon_idx(%d) <> picin_hcb_rd_idx(%d)\n"

#define DEC_ERR_CODE_RECON_SPS_INVALID       0xe001bu
#define DEC_ERR_MSG_RECON_SPS_INVALID        "AVCDEC:: avcdec_init_fbp() failed, SPS is not valid, pic_number_in_hcb=%d, first_recon_done=%d\n"

#define DEC_ERR_CODE_RECON_SPS_IDX_MISMATCH  0xe001cu
#define DEC_ERR_MSG_RECON_SPS_IDX_MISMATCH   "AVCDEC:: avcdec_init_fbp() failed, SPS.recon_idx(%d) <> pi_read_idx_in_hcb(%d)\n"

#define DEC_ERR_CODE_RECON_FB_SIZE_SHORT     0xe001du
#define DEC_ERR_MSG_RECON_FB_SIZE_SHORT      "AVCDEC:: avcdec_init_fbp() failed, img size %dx%d > buffer size %dx%d\n"

#define DEC_ERR_CODE_RECON_PPS_IDX_MISMATCH  0xe001eu
#define DEC_ERR_MSG_RECON_PPS_IDX_MISMATCH   "AVCDEC:: avcdec_alloc_recon_fb(): sh.pps_id(%d) <> pps.pps_id(%d)\n"

#define DEC_ERR_CODE_RECON_OUTPUT_FLD_PAIR   0xe001fu
#define DEC_ERR_MSG_RECON_OUTPUT_FLD_PAIR    "AVCDEC:: avcdec_proc_output(). output of field pairs\n"

#define DEC_ERR_CODE_RECON_DBP_SZ_OVERFLOW   0xe0020u
#define DEC_ERR_MSG_RECON_DBP_SZ_OVERFLOW    "AVCDEC:: recon discrete buffer pool size overflows, recon_cmd=%d, recon_msg=%d, mvdec_edge=%d\n"

#define DEC_ERR_CODE_RESIDUAL_FBUF           0xe0021u
#define DEC_ERR_MSG_RESIDUAL_FBUF            "AVCDEC:: frame buffer left after stopped. fbp_id=0x%04x, fb_num=%d\n"

#define DEC_ERR_CODE_BITS_FIFO_WP_MISMATCH   0xe0022u
#define DEC_ERR_MSG_BITS_FIFO_WP_MISMATCH    "AVCDEC:: bitsfifo update failed, cpb_wptr=0x%x != bits_fifo_start=0x%x\n"

#define DEC_ERR_CODE_BITS_FIFO_OVERFLOW      0xe0023u
#define DEC_ERR_MSG_BITS_FIFO_OVERFLOW       "AVCDEC:: bitsfifo update failed, bs_data_size=%d > cpb_emptiness=%d\n"

#define DEC_ERR_CODE_HCB_NOT_ALIGN           0xe0024u
#define DEC_ERR_MSG_HCB_NOT_ALIGN            "AVCDEC:: avcdec_init_cout(), cur_addr(0x%x)-d_base(0x%x) is not 128B aligned.\n"

#define DEC_ERR_CODE_GET_SH_IDX_MISMATCH     0xe0025u
#define DEC_ERR_MSG_GET_SH_IDX_MISMATCH      "AVCDEC:: avcdec_get_sh_fr_q() done, sh_idx=%d, sh_num=%d, frame_num=%d sh_saddr=0x%08x\n"

#define DEC_ERR_CODE_PUT_SH_IDX_MISMATCH     0xe0026u
#define DEC_ERR_MSG_PUT_SH_IDX_MISMATCH      "AVCDEC:: avcdec_put_sh_fr_q() done, sh_idx=%d, sh_num=%d, frame_num=%d sh_saddr=0x%08x\n"

#define DEC_ERR_CODE_FREE_DPB_IDX            0xe0027u        
#define DEC_ERR_MSG_FREE_DPB_IDX             "AVCDEC:: free_dpb_idx() failed. dpb_idx=%d\n"

#define DEC_ERR_CODE_GET_FB_FR_DPB           0xe0028u
#define DEC_ERR_MSG_GET_FB_FR_DPB            "AVCDEC:: get_fb_fr_dpb_idx() failed: could not find fb_id for dpb_idx=%d, dpb_panel=%08x, dpb_used=%d, ref=%d, ltref=%d\n"

#define DEC_ERR_CODE_GET_FRM_NUM_FR_DPB      0xe0029u
#define DEC_ERR_MSG_GET_FRM_NUM_FR_DPB       "AVCDEC:: get_frm_num_fr_dpb_idx() failed: could not find frame_num for dpb_idx=%d, dpb_panel=%08x, dpb_used=%d, ref=%d, ltref=%d\n"

#define DEC_ERR_CODE_INVALID_PICSTRUCT       0xe002au
#define DEC_ERR_MSG_INVALID_PICSTRUCT        "AVCDEC:: invalid pic_struct(%d)\n"

#define DEC_ERR_CODE_INVALID_IS_USED         0xe002bu
#define DEC_ERR_MSG_INVALID_IS_USED          "AVCDEC:: invalid is_used=%d\n"

#define DEC_ERR_CODE_OUT_DPB_UNPAIR          0xe002cu
#define DEC_ERR_MSG_OUT_DPB_UNPAIR           "AVCDEC:: output_one_frm_fr_dpb(), unpaired! is_used=%d, poc=%d, frm_fb_id=0x%04x \n"

#define DEC_ERR_CODE_SLIDING_WIN_IDR         0xe002du
#define DEC_ERR_MSG_SLIDING_WIN_IDR          "AVCDEC:: sliding_window_management() should not be done on IDR\n"

#define DEC_ERR_CODE_TESTBIN_CRC             0xe002eu
#define DEC_ERR_MSG_TESTBIN_CRC              "AVCDEC:: avcdec_load_testbin() CRC is not correct\n"

#define DEC_ERR_CODE_TESTBIN_FRM_LMT         0xe002fu
#define DEC_ERR_MSG_TESTBIN_FRM_LMT          "AVCDEC:: avcdec_load_testbin() test frame maximum is 6\n"
#define DEC_ERR_CODE_NUM 0x30u

extern const char * const dec_err_code[DEC_ERR_CODE_NUM];

/*****************************************************************************
 * module_idx 15 ERR_CODE_IDSP_HW_TIMEOUT
 *****************************************************************************/
#define IDSP_HW_TIMEOUT_CODE 0xF0000u
#define IDSP_HW_TIMEOUT_MSG                                     \
  "IDSP_HW_TIMEOUT: chan_%u cap_seq_no:%u job_pass_step_id:%u " \
  "job_id:0x%x hw_duration:%u\n"

#define IDSP_HW_TIMEOUT_ERR_NUM 0x1u

extern const char* const idsp_hw_timeout_err_code[IDSP_HW_TIMEOUT_ERR_NUM];

/*****************************************************************************
 * module_idx 16 ERR_CODE_ENC_HW
 *****************************************************************************/
#define ENC_HW_ERRC_ENC_0 0x100000u
#define ENC_HW_ERRM_ENC_0 "ENC_HW_TIMEOUT: MEMD HW is time out\n"

#define ENC_HW_ERR_NUM 0x1u

extern const char * const enc_hw_err_code[ENC_HW_ERR_NUM];

/*****************************************************************************
 * module_idx 17 ERR_CODE_ENC_CRC
 *****************************************************************************/
#define ENC_CRC_ERRC_ENC_0 0x110000u
#define ENC_CRC_ERRM_ENC_0 "ENC_CRC_MISMATCH: memd checksum mismatch: index=%d golden=0x%.8x got=0x%.8x\n\n"

#define ENC_CRC_ERRC_ENC_1 0x110001u
#define ENC_CRC_ERRM_ENC_1 "ENC_CRC_MISMATCH: code checksum not matched: golden=%p got=%p\n"

#define ENC_CRC_ERR_NUM 0x2u

extern const char * const enc_crc_err_code[ENC_CRC_ERR_NUM];

#endif /* UCODE_ERROR_H_ */
