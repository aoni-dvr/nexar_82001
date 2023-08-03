/* CV5 DSP error code header file
Commit: 866f87a68201ebaf124b7b0b3c0c277a4bc392e9
Last Changed Commit: 4978828b2545fb9d82430ebfc9f8a5effb1f8a53
Last Changed Date: 2022-11-05 05:15:38 -0700
*/
/**
 * @file ucode_error.h
 *
 *  Copyright (c) 2021 Ambarella International LP
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

#define ERR_CODE_MOD_MM 0u
#define ERR_CODE_MOD_API 1u
#define ERR_CODE_MOD_MSG 2u
#define ERR_CODE_MOD_BOOT 3u
#define ERR_CODE_MOD_IENG 4u
#define ERR_CODE_MOD_VPROC 5u
//\#define ERR_CODE_MOD_IDSPDRV 6u
#define ERR_CODE_MOD_VIN 7u
#define ERR_CODE_MOD_PREV 8u
#define ERR_CODE_MOD_PROC_HL 9u
#define ERR_CODE_MOD_VOUT 10u
#define ERR_CODE_MOD_ENC 11u
#define ERR_CODE_MOD_ENG0 12u
#define ERR_CODE_MOD_PROXY 13u
#define ERR_CODE_MOD_DEC 14u
#define ERR_CODE_MOD_NUM 15u

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
#define MM_ERR_REQ_FB_FATAL_0_MSG  "MM:: req_fb(0x%04x) fatal 0: pic_struct=%d last_fb_id=0x%08x top_used_cnt=%d top_pending=%d\n"

#define MM_ERR_REQ_FB_FATAL_1_CODE 0x00004u
#define MM_ERR_REQ_FB_FATAL_1_MSG  "MM:: req_fb(0x%04x) fatal 1: pic_struct=%d last_fb_id=0x%08x bot_used_cnt=%d bot_pending=%d\n"

#define MM_ERR_REQ_FB_FATAL_2_CODE 0x00005u
#define MM_ERR_REQ_FB_FATAL_2_MSG  "MM:: req_fb() fatal 2: fbp_id=0x%04x fb_idx=%d fb_num_total=%d\n"

#define MM_ERR_REQ_FB_FATAL_3_CODE 0x00006u
#define MM_ERR_REQ_FB_FATAL_3_MSG  "MM:: req_fb() fatal 3: fb_idx=0x%04x top_used_cnt=%d bot_used_cnt=%d top_pending=%d bot_pending=%d\n"

#define MM_ERR_REG_FB_FATAL_CODE 0x00007u
#define MM_ERR_REG_FB_FATAL_MSG  "MM:: reg_fb() fatal: fb_id=0x%08x, top_used_cnt=%d bot_used_cnt=%d\n"

#define MM_ERR_REL_FB_FATAL_0_CODE 0x00008u
#define MM_ERR_REL_FB_FATAL_0_MSG  "MM:: rel_fb() fatal: fb_id=0x%08x top_used_cnt=%d top_pending=%d bot_used_cnt=%d bot_pending=%d\n"

#define MM_ERR_REL_FB_FATAL_1_CODE 0x00009u
#define MM_ERR_REL_FB_FATAL_1_MSG  "MM:: rel_fb() fatal: fb_id=0x%08x, fb_num_used=%d\n"

#define MM_ERR_INC_FB_FATAL_CODE 0x0000au
#define MM_ERR_INC_FB_FATAL_MSG  "MM:: inc_fb() fatal: fb_id=0x%08x top_used_cnt=%d top_pending=%d bot_used_cnt=%d bot_pending=%d \n"

#define MM_ERR_DEC_FB_FATAL_CODE 0x0000bu
#define MM_ERR_DEC_FB_FATAL_MSG  "MM:: dec_fb() fatal: fb_id=0x%08x top_used_cnt=%d top_pending=%d bot_used_cnt=%d bot_pending=%d \n"

#define MM_ERR_REQ_FB_INVALID_FB_TYPE_CODE 0x0000cu
#define MM_ERR_REQ_FB_INVALID_FB_TYPE_MSG  "MM:: req_fb(): fbp_id=0x%04x invalid fb_type=%d\n"

#define MM_ERR_REG_FB_INVALID_FB_TYPE_CODE 0x0000du
#define MM_ERR_REG_FB_INVALID_FB_TYPE_MSG  "MM:: reg_fb(): fb_id=0x%08x invalid fb_type=%d\n"

#define MM_ERR_REL_FB_INVALID_FB_TYPE_CODE 0x0000eu
#define MM_ERR_REL_FB_INVALID_FB_TYPE_MSG  "MM:: rel_fb(): fb_id=0x%08x invalid fb_type=%d\n"

#define MM_ERR_GET_FB_INVALID_FB_TYPE_CODE 0x0000fu
#define MM_ERR_GET_FB_INVALID_FB_TYPE_MSG  "MM:: get_fb(): fb_id=0x%08x invalid fb_type=%d\n"

#define MM_ERR_SET_FB_EXT_FAILED_CODE 0x00010u
#define MM_ERR_SET_FB_EXT_FAILED_MSG  "MM:: set_fb_ext() failed: frm_blk_size=%d > fb_size=%d\n"

#define MM_ERR_SET_FB_EXT_YUV_Y_CODE 0x00011u
#define MM_ERR_SET_FB_EXT_YUV_Y_MSG  "MM:: set_fb_ext_y_uv() failed: lu_blk_size=%d > fb_size_y=%d\n"

#define MM_ERR_SET_FB_EXT_YUV_UV_CODE 0x00012u
#define MM_ERR_SET_FB_EXT_YUV_UV_MSG  "MM:: set_fb_ext_y_uv() failed: ch_blk_size=%d > fb_size_uv=%d\n"

#define MM_ERR_FB_ID_NOT_A_FRAME_CODE 0x00013u
#define MM_ERR_FB_ID_NOT_A_FRAME_MSG  "MM:: fb_id=0x%08x is not a frame\n"

#define MM_ERR_GET_FB_AUX_FAILED_CODE 0x00014u
#define MM_ERR_GET_FB_AUX_FAILED_MSG  "get_fb_aux() failed, aux_width=0, fb_id=0x%08x, p_aux_buf=0x%08x\n"

#define MM_ERR_INVALID_FBP_TYPE_CODE 0x00015u
#define MM_ERR_INVALID_FBP_TYPE_MSG  "MM:: check_fbp_id(0x%04x) fatal: invalid fbp type!\n"

#define MM_ERR_INVALID_FBP_INDEX_CODE 0x00016u
#define MM_ERR_INVALID_FBP_INDEX_MSG  "MM:: check_fbp_id() failed: invalid fbp_id=0x%08x, fbp_index(%d) is over limit (%d)\n"

#define MM_ERR_INVALID_MFBP_TYPE_CODE 0x00017u
#define MM_ERR_INVALID_MFBP_TYPE_MSG  "MM:: check_mfbp_id(0x%04x) fatal: invalid mfbp type!\n"

#define MM_ERR_INVALID_MFBP_INDEX_CODE 0x00018u
#define MM_ERR_INVALID_MFBP_INDEX_MSG  "MM:: check_mfbp_id() failed: invalid mfbp_id=0x%08x, mfbp_index(%d) is over limit (%d)\n"

#define MM_ERR_INVALID_IF_EXT_CODE 0x00019u
#define MM_ERR_INVALID_IF_EXT_MSG  "MM:: fbp_id=0x%04x, invalid if_ext=%d\n"

#define MM_ERR_INVALID_IF_TILED_CODE 0x0001au
#define MM_ERR_INVALID_IF_TILED_MSG  "MM:: invalid if_tiled=%d\n"

#define MM_ERR_FBP_USED_CODE 0x0001bu
#define MM_ERR_FBP_USED_MSG  "MM:: create_fbp() failed: fbp_used(%d) >= max_fbp_num(%d)\n"

#define MM_ERR_FBP_INF_CODE 0x0001cu
#define MM_ERR_FBP_INF_MSG  "MM:: create_fbp() failed: fbp_inf_used(%d) + img_inf_max(%d) > max_imginf_num(%d)\n"

#define MM_ERR_FBUF_USED_CODE 0x0001du
#define MM_ERR_FBUF_USED_MSG  "MM:: create_fbp() failed: allow_ext=%d fbuf_used(%d) + frm_num_max(%d) >= max_fb_num(%d)\n"

#define MM_ERR_FRM_NUM_MAX_CODE 0x0001eu
#define MM_ERR_FRM_NUM_MAX_MSG  "MM:: create_fbp() failed: frm_num_max=%d >= MAX_FB_NUM_IN_FBP %d\n"

#define MM_ERR_INVALID_MEM_PAR_ID_CODE 0x0001fu
#define MM_ERR_INVALID_MEM_PAR_ID_MSG  "MM:: init_fbp_w_par() failed: fbp_id=0x%04x, invalid mem_par_id=0x%04x\n"

#define MM_ERR_INVALID_FRM_BLK_SIZE_CODE 0x00020u
#define MM_ERR_INVALID_FRM_BLK_SIZE_MSG  "MM:: init_fbp failed: fbp_id=0x%04x ch_fmt=%d lu_blk_size=%d ch_blk_size=%d aux_blk_size=%d\n"

#define MM_ERR_INVALID_CHROMA_FORMAT_CODE 0x00021u
#define MM_ERR_INVALID_CHROMA_FORMAT_MSG  "MM:: init_fbp_w_buf() failed: invalid fbp_id, tiled format only supports 420 but %d\n"

#define MM_ERR_INVALID_FB_NUM_TOTAL_CODE 0x00022u
#define MM_ERR_INVALID_FB_NUM_TOTAL_MSG  "MM:: init_fbp_w_buf() failed: tiled format fb_num_total=0\n"

#define MM_ERR_INVALID_BUF_WIDTH_CODE 0x00023u
#define MM_ERR_INVALID_BUF_WIDTH_MSG  "MM:: set_cur_fbp_imginfo_y_uv(0x%04x) failed: buf_width=%d lu_img_width=%d lu_img_offset_x=%d\n"

#define MM_ERR_INVALID_BUF_HEIGHT_CODE 0x00024u
#define MM_ERR_INVALID_BUF_HEIGHT_MSG  "MM:: set_cur_fbp_imginfo_y_uv(0x%04x) failed: buf_height=%d lu_img_height=%d lu_img_offset_y=%d\n"

#define MM_ERR_INVALID_IMG_INF_IDX_CODE 0x00025u
#define MM_ERR_INVALID_IMG_INF_IDX_MSG  "MM:: set_fbp_imginfo() failed, img_inf_idx(%d) >= max(%d)\n"

#define MM_ERR_MFBP_USED_CODE 0x00026u
#define MM_ERR_MFBP_USED_MSG  "MM:: create_mfbp() failed: mfbp_used(%d) >= max_mfbp_num(%d)\n"

#define MM_ERR_MFBP_INF_CODE 0x00027u
#define MM_ERR_MFBP_INF_MSG  "MM:: create_mfbp() failed: mfbp_inf_used(%d) + img_inf_max(%d) > max_mimginf_num(%d)\n"

#define MM_ERR_MFBUF_USED_CODE 0x00028u
#define MM_ERR_MFBUF_USED_MSG  "MM:: create_mfbp() failed: allow_ext=%d mfbuf_used(%d) + frm_num_max(%d) >= max_mfb_num(%d)\n"

#define MM_ERR_MFRM_BLK_SIZE_CODE 0x00029u
#define MM_ERR_MFRM_BLK_SIZE_MSG  "MM:: init_mfbp_w_buf failed: fbp_id=0x%04x ch_fmt=%d lu_blk_size=%d\n"

#define MM_ERR_INVALID_MCB_NUM_CODE 0x0002au
#define MM_ERR_INVALID_MCB_NUM_MSG  "MM:: create_mcbl() failed: mcb_num=%d > max=%d\n"

#define MM_ERR_INVALID_MCB_USED_CODE 0x0002bu
#define MM_ERR_INVALID_MCB_USED_MSG  "MM:: create_mcbl() failed: max_mcbl_num=%d mcbl_used=%d\n"

#define MM_ERR_INVALID_MCB_USED_NUM_CODE 0x0002cu
#define MM_ERR_INVALID_MCB_USED_NUM_MSG  "MM:: create_mcbl() failed: max_mcb_num=%d > mcb_used=%d + max=%d\n"

#define MM_ERR_INVALID_MCBL_SIZE_CODE 0x0002du
#define MM_ERR_INVALID_MCBL_SIZE_MSG  "MM:: init_mcbl_with_bu() failed, mcb_size =%d mcb_num=%d >= buf_size=%d\n"

#define MM_ERR_POP_MCB_FAILED_CODE 0x0002eu
#define MM_ERR_POP_MCB_FAILED_MSG  "MM:: pop_mcb() failed, mcb_used=%d mcb_num=%d mcb_head=0x%02x\n"

#define MM_ERR_PUSH_MCB_FAILED_CODE 0x0002fu
#define MM_ERR_PUSH_MCB_FAILED_MSG  "MM:: push_mcb() failed, mcbl_id=0x%04x, mcb_used=%d mcb_id=0x%02x\n"

#define MM_ERR_INVALID_MCBL_ID_CODE 0x00030u
#define MM_ERR_INVALID_MCBL_ID_MSG  "MM:: mcbl_id is MCBL_ID_INVALID!\n"

#define MM_ERR_INVALID_MCB_ID_TO_FREE_CODE 0x00031u
#define MM_ERR_INVALID_MCB_ID_TO_FREE_MSG  "MM:: mcb_id_to_free is NULL!\n"

#define MM_ERR_INVALID_MEM_PAR_TYPE_CODE 0x00032u
#define MM_ERR_INVALID_MEM_PAR_TYPE_MSG  "MM:: invalid mem_type=%d\n"

#define MM_ERR_INVALID_ALIGNMENT_CODE 0x00033u
#define MM_ERR_INVALID_ALIGNMENT_MSG  "MM:: alignment must be 4B aligned but alignment=%d\n"

#define MM_ERR_INVALID_SIZE_CODE 0x00034u
#define MM_ERR_INVALID_SIZE_MSG  "MM:: size must be 4B aligned but size=%d\n"

#define MM_ERR_TOO_MANY_PAR_CODE 0x00035u
#define MM_ERR_TOO_MANY_PAR_MSG  "MM:: par_type=%d, too many partitions, par_last_used=%d max_par_num=%d!\n"

#define MM_ERR_NO_MORE_SMEM_CODE 0x00036u
#define MM_ERR_NO_MORE_SMEM_MSG  "MM:: no more smem, size_aligned=%d next_off=0x%08x smem_base=0x%08x, end=0x%08x\n"

#define MM_ERR_NO_MORE_DRAM_CODE 0x00037u
#define MM_ERR_NO_MORE_DRAM_MSG  "MM:: no more dram, size_aligned=%d next_off=0x%08x dram_base=0x%08x, end=0x%08x\n"

#define MM_ERR_INVALID_MEM_PAR_CODE 0x00038u
#define MM_ERR_INVALID_MEM_PAR_MSG  "MM:: invalid mem par 0x%04x par_idx=%d par_used=%d\n"

#define MM_ERR_INVALID_SUP_SUB_PAR_CODE 0x00039u
#define MM_ERR_INVALID_SUP_SUB_PAR_MSG  "MM:: invalid sup/sub par 0x%04x\n"

#define MM_ERR_UNKNOWN_PAR_LEVEL_CODE 0x0003au
#define MM_ERR_UNKNOWN_PAR_LEVEL_MSG  "MM:: unknown par_level=%d, par_id=0x%x\n"

#define MM_ERR_TOO_MANY_SUP_PAR_CODE 0x0003bu
#define MM_ERR_TOO_MANY_SUP_PAR_MSG  "MM:: too many super partitions(%d) >= max=%d\n"

#define MM_ERR_TOO_MANY_SUB_PAR_CODE 0x0003cu
#define MM_ERR_TOO_MANY_SUB_PAR_MSG  "MM:: too many sub partitions, used(%d) + sub_par_max(%d) > max_sub_par_num(%d)\n"

#define MM_ERR_TOO_MANY_SUP_SUB_PAR_CODE 0x0003du
#define MM_ERR_TOO_MANY_SUP_SUB_PAR_MSG  "MM:: sup_par_id=0x%04x, idx=%d: too many sub partitions(%d) >= max(%d)\n"

#define MM_ERR_INVALID_SUP_PAR_ID_CODE 0x0003eu
#define MM_ERR_INVALID_SUP_PAR_ID_MSG  "MM:: (0x%04x) is not a valid sup_par_id (sup_par_used=0x%04x)!\n"

#define MM_ERR_SUB_PAR_OVER_LIMIT_CODE 0x0003fu
#define MM_ERR_SUB_PAR_OVER_LIMIT_MSG  "MM:: total sub partition (0x%08x+0x%08x) is over the superpartition limit=(0x%08x)\n"

#define MM_ERR_INVALID_MEM_PAR_LEVEL_CODE 0x00040u
#define MM_ERR_INVALID_MEM_PAR_LEVEL_MSG  "MM:: alloc on sub_par_id (0x%04x) only supported on sub-partition but =0x%04x\n"

#define MM_ERR_SUB_PAR_SIZE_CODE 0x00041u
#define MM_ERR_SUB_PAR_SIZE_MSG  "MM:: alloc on sub_par_id (0x%04x) failed: used=%d + size=%d > total=%d\n"

#define MM_ERR_INVALID_WIDTH_PITCH_CODE 0x00042u
#define MM_ERR_INVALID_WIDTH_PITCH_MSG  "MM:: width=%d need to be less than pitch=%d\n"

#define MM_ERR_INVALID_DBP_USED_CODE 0x00043u
#define MM_ERR_INVALID_DBP_USED_MSG  "MM:: dbp_used(%d) >= MAX_DBP_NUM(%d)\n"

#define MM_ERR_INVALID_DBUF_PITCH_CODE 0x00044u
#define MM_ERR_INVALID_DBUF_PITCH_MSG  "MM:: dbuf_pitch=%d need to be 32B aligned\n"

#define MM_ERR_INVALID_DBUF_WIDTH_CODE 0x00045u
#define MM_ERR_INVALID_DBUF_WIDTH_MSG  "MM:: width=%d need to be less than pitch=%d\n"

#define MM_ERR_INVALID_CBUF_USED_CODE 0x00046u
#define MM_ERR_INVALID_CBUF_USED_MSG  "MM::  maximum cirbuf %d reached!\n"

#define MM_ERR_INVALID_CBUF_SIZE_CODE 0x00047u
#define MM_ERR_INVALID_CBUF_SIZE_MSG  "MM:: cbuf_size(%d) need to be multiple of dma_width(%d)\n"

#define MM_ERR_INVALID_READER_USED_CODE 0x00048u
#define MM_ERR_INVALID_READER_USED_MSG  "MM:: reader_used=%d >= MAX_CIRBUF_READER(%d)\n"

#define MM_ERR_INVALID_READER_ID_CODE 0x00049u
#define MM_ERR_INVALID_READER_ID_MSG  "MM:: cbuf_id=%08x reader_id=%d, rdr_used=%d\n"

#define MM_ERR_CBUF_RD_NUM_BYTES_CODE 0x0004au
#define MM_ERR_CBUF_RD_NUM_BYTES_MSG  "MM:: cbuf_id=%08x num_bytes=%d, num_bytes[%d]=%d\n"

#define MM_ERR_INVALID_BDT_USED_CODE 0x0004bu
#define MM_ERR_INVALID_BDT_USED_MSG  "MM:: bdt_used(%d) >= max_bdt_num(%d)\n"

#define MM_ERR_INVALID_BD_NUM_CODE 0x0004cu
#define MM_ERR_INVALID_BD_NUM_MSG  "MM:: bd_used(%d) + bd_mum(%d) > max_bd_num(%d)\n"

#define MM_ERR_INVALID_PADDED_WIDTH_CODE 0x0004du
#define MM_ERR_INVALID_PADDED_WIDTH_MSG  "MM:: bd_id=0x%08x SWIDTH_2_SPITCH(%d) > padded_width(%d)\n"

#define MM_ERR_INVALID_HEAP_SIZE_CODE 0x0004eu
#define MM_ERR_INVALID_HEAP_SIZE_MSG  "MM:: alloc_heap() failed, aligned_size=%d + used=%d < HEAP_SIZE=%d\n"

#define MM_ERR_UNDEFINED_AES_SYMBOL_CODE 0x0004fu
#define MM_ERR_UNDEFINED_AES_SYMBOL_MSG  "MM:: AES symbol should be defined! \n"

#define MM_ERR_AES_FAILED_EXIT_CODE 0x00050u
#define MM_ERR_AES_FAILED_EXIT_MSG  "MM:: aes_main: failed exit=%d\n"

#define MM_ERR_AES_LEN_ALIGN_CODE 0x00051u
#define MM_ERR_AES_LEN_ALIGN_MSG  "length %d is not 16 align\n"

#define MM_ERR_STACK_OVERFLOW_CODE 0x00052u
#define MM_ERR_STACK_OVERFLOW_MSG  "stack overflow, bottom:0x%x 4-word:0x%08x 0x%08x 0x%08x 0x%08x\n"

#define MM_ERR_UTEST_CODE 0x00053u
#define MM_ERR_UTEST_MSG  "MM:: utest error arg0:%d arg1:%d arg2:%d arg3:%d arg4:%d\n"

#define MM_ERR_SMEM_2_DRAM_ALIGN_CODE 0x00054u
#define MM_ERR_SMEM_2_DRAM_ALIGN_MSG                                           \
  "smem_2_dram_copy_one2many: smem_addr (0x%x) does not align to 16 || "       \
  "out_width (%d) is not a multiple of 4 || out_pitch (%d) is not a multiple " \
  "of 32\n"

#define MM_ERR_BAD_BDT_ID_CODE 0x00055u
#define MM_ERR_BAD_BDT_ID_MSG                                          \
  "Detected bad buffer descriptor table ID 0x%x, debugging value1:%u " \
  "value2:%u value3:%u value4:%u\n"

#define MM_ERR_WRITE_DRAM_OUTOF_RANGE_CODE 0x00056u
#define MM_ERR_WRITE_DRAM_OUTOF_RANGE_MSG  "MM:: write dram out of range address 0x%x, size %d\n"

#define MM_ERR_NUM 0x57u

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
#define IENG_ERR_UNKNOWN_CODE 0x40000u
#define IENG_ERR_UNKNOWN_MSG "IENG: unknown error arg0:%d arg1:%d arg2:%d arg3:%d arg4:%d\n"

#define IENG_ERR_ADV_IDSP_JOB_Q_CODE 0x40001u
#define IENG_ERR_ADV_IDSP_JOB_Q_MSG  "IENG: advance_idsp_job_q(0x%04x) failed to advance job queue!\n"

#define IENG_ERR_IENG_INIT_CODE 0x40002u
#define IENG_ERR_IENG_INIT_MSG "IENG: idspeng_init used=%u allocated=%u\n"

#define IENG_ERR_CHK_STAT_DEFS_CODE 0x40003u
#define IENG_ERR_CHK_STAT_DEFS_MSG "IENG: ieng_check_static_defs static defines fail check %u %u %u %u %u\n"

#define IENG_ERR_GET_SYNC_CNT_CODE 0x40004u
#define IENG_ERR_GET_SYNC_CNT_MSG "IENG: ieng_get_sync_cnt sync_cnt_idx_logical:%u >= %u\n"

#define IENG_ERR_PROC_JOB_SECT_CODE 0x40005u
#define IENG_ERR_PROC_JOB_SECT_MSG "IENG: ieng_proc_job_sect invalid sect_no:%u\n"

#define IENG_ERR_PROC_JOB_SECT_PEND_CODE 0x40006u
#define IENG_ERR_PROC_JOB_SECT_PEND_MSG "IENG: ieng_proc_job_sect_pending invalid sect_no:%u\n"

#define IENG_ERR_SET_SYNC_CNT_CODE 0x40007u
#define IENG_ERR_SET_SYNC_CNT_MSG "IENG: ieng_set_sync_cnt(0x%08x) p_sync_cnt_idx_logical:%u sync_cnt_val:%u ready_to_issue:%u cur_sync_val:%u == 0u\n"

#define IENG_ERR_SMEM_ADDR_CODE 0x40008u
#define IENG_ERR_SMEM_ADDR_MSG "IENG: invalid smem address:0x%x min:0x%x max:0x%x\n"

#define IENG_ERR_DIRECT_DRAM_SMEM_CODE 0x40009u
#define IENG_ERR_DIRECT_DRAM_SMEM_MSG "IENG: ieng_prog_smem_in(0x%08x) sreg=%u sbase=0x%08x " \
                                      "needs to aligned to %u for direct-dram input!\n"

#define IENG_ERR_FLY_JOB_CODE 0x4000au
#define IENG_ERR_FLY_JOB_MSG "IENG: job_id=0x%08x invalid free=%u fly job list is full\n"

#define IENG_ERR_NULL_POINTER_CODE 0x4000bu
#define IENG_ERR_NULL_POINTER_MSG "IENG: unexpected null pointer\n"

#define IENG_ERR_INIT_SREG_CODE 0x4000cu
#define IENG_ERR_INIT_SREG_MSG "IENG: sreg:%u must be initialized to zero\n"

#define IENG_ERR_SEC_CMD_CODE 0x4000du
#define IENG_ERR_SEC_CMD_MSG "IENG: section:%u bank:%u increasing busy indicator but the busy indicator has been set\n"

#define IENG_ERR_BAD_VALUE_CODE 0x4000eu
#define IENG_ERR_BAD_VALUE_MSG "IENG: detected a bad value:%u debugging value1:%u value2:%u value3:%u value4:%u\n"

#define IENG_ERR_IDSP_LD_STR_CODE 0x4000fu
#define IENG_ERR_IDSP_LD_STR_MSG "IENG: run out of mcb for idsp load streams index:%u used:%u max:%u\n"

#define IENG_ERR_NUM 0x10u

extern const char * const ieng_err_code[IENG_ERR_NUM];

/*****************************************************************************
 * module_idx 5 ERR_CODE_MOD_VPROC
 *****************************************************************************/
#define VPROC_ERR_UNKNOWN_CODE 0x50000u
#define VPROC_ERR_UNKNOWN_MSG "VPROC: unknown error arg0:%d arg1:%d arg2:%d arg3:%d arg4:%d\n"

#define VPROC_ERR_CHAN_ID_CODE 0x50001u
#define VPROC_ERR_CHAN_ID_MSG "VPROC: invalid channel_id:%u num_of_chan:%u\n"

#define VPROC_ERR_OP_Q_TYPE_CODE 0x50002u
#define VPROC_ERR_OP_Q_TYPE_MSG "VPROC[chan_%d]: vproc_op_q invalid q_type:%d\n"

#define VPROC_ERR_OP_Q_MODE_CODE 0x50003u
#define VPROC_ERR_OP_Q_MODE_MSG "VPROC[chan_%d]: vproc_op_q invalid op_mode:%d\n"

#define VPROC_ERR_UPDATE_DATA_CODE 0x50004u
#define VPROC_ERR_UPDATE_DATA_MSG "VPROC[chan_%d]: invalid VPROC_LWQ storage_type:%d\n"

#define VPROC_ERR_INPUT_FID_CODE 0x50005u
#define VPROC_ERR_INPUT_FID_MSG                                             \
  "VPROC[chan_%u]: vproc_check_input_fbs input_format:%u is_ce_enabled:%u " \
  "pic_fid:0x%08x me_pic_fid2:0x%08x invalid\n"

#define VPROC_ERR_RAW_SIZE_CODE 0x50006u
#define VPROC_ERR_RAW_SIZE_MSG                                               \
  "VPROC[chan_%d]: vproc_check_raw_setup raw buffer overflow WxH max:%dx%d " \
  "actual:%dx%d check dummy_window_width and dummy_window_height with "      \
  "raw_width_max and raw_height_max\n"

#define VPROC_ERR_1EXP_SIZE_CODE 0x50007u
#define VPROC_ERR_1EXP_SIZE_MSG "VPROC[chan_%d]: vproc_check_raw_setup raw single " \
                                "exposure buffer overflow WxH max:%dx%d actual:%dx%d\n"

#define VPROC_ERR_CE_SIZE_CODE 0x50008u
#define VPROC_ERR_CE_SIZE_MSG "VPROC[chan_%d]: vproc_check_pic_setup ce buffer " \
                              "overflow WxH max:%dx%d actual:%dx%d\n"

#define VPROC_ERR_WH0_SIZE_CODE 0x50009u
#define VPROC_ERR_WH0_SIZE_MSG "VPROC[chan_%d]: vproc_check_cfa_setup W0xH0 buffer " \
                               "overflow max WxH:%dx%d actual WxH:%dx%d\n"

#define VPROC_ERR_PRE_WARP_SIZE_CODE 0x5000au
#define VPROC_ERR_PRE_WARP_SIZE_MSG "VPROC[chan_%d]: vproc_check_pre_warp_setup pre_warp luma " \
                                    "buffer overflow max WxH:%dx%d actual WxH:%dx%d\n"

#define VPROC_ERR_NOT_ENOUGH_SMEM_CODE 0x5000bu
#define VPROC_ERR_NOT_ENOUGH_SMEM_MSG "VPROC[chan_%d]: not enough smem, short %d\n"

#define VPROC_ERR_NOT_ENOUGH_DRAM_CODE 0x5000cu
#define VPROC_ERR_NOT_ENOUGH_DRAM_MSG "VPROC[chan_%d]: not enough dram, short %d\n"

#define VPROC_ERR_MAIN_SIZE_CODE 0x5000du
#define VPROC_ERR_MAIN_SIZE_MSG "VPROC[chan_%d]: vproc_pic_setup_sanity_check - " \
                                "main buffer overflow, WxH max(%dx%d), actual(%dx%d)\n"

#define VPROC_ERR_OUT_PIN_CODE 0x5000eu
#define VPROC_ERR_OUT_PIN_MSG "VPROC: vproc_get_output_pin_addr invalid argument " \
                              "dsp_fp_type:%u bind_msg:%u src_fp_num:%u src_ch_num:%u src_str_num:%u\n"

#define VPROC_ERR_IN_Q_ID_CODE 0x5000fu
#define VPROC_ERR_IN_Q_ID_MSG "VPROC: vproc_get_input_q_id invalid argument " \
                              "bind_msg:%u ch_num:%u str_num:%u\n"

#define VPROC_ERR_REQ_EXT_BUF_CODE 0x50010u
#define VPROC_ERR_REQ_EXT_BUF_MSG "VPROC[chan_%d]: vproc_req_ext_mem_buf invalid frame buffer! " \
                                  "ext_mem_type_id:%d\n"

#define VPROC_ERR_DZ_ZOOM_FAC_CODE 0x50011u
#define VPROC_ERR_DZ_ZOOM_FAC_MSG "VPROC[chan_%u]: vproc_dzoom_check " \
                                  "invalid zoom factor zoom_x:0x%x zoom_y:0x%x\n"

#define VPROC_ERR_DZ_DUMMY_WIN_CODE 0x50012u
#define VPROC_ERR_DZ_DUMMY_WIN_MSG                                           \
  "VPROC[chan_%u]: vproc_dzoom_check is_raw_compressed=%u is_ce_enabled=%u " \
  "dummy_window_x_left=%u should be 2^%u-aligned\n"


#define VPROC_ERR_PIN_OUT_DECI_CODE 0x50013u
#define VPROC_ERR_PIN_OUT_DECI_MSG "VPROC[chan_%u]: vproc_deci_rate_api " \
                                   "invalid stream_id:%u\n"

#define VPROC_ERR_GRP_INFO_CODE 0x50014u
#define VPROC_ERR_GRP_INFO_MSG "VPROC[chan_%u]: vproc_load_grp_info " \
                               "gpr_id:%d mismatch info->grp_id:%d\n"

#define VPROC_ERR_JID_Y2Y_CBS_CODE 0x50015u
#define VPROC_ERR_JID_Y2Y_CBS_MSG "VPROC[chan_%d]: y2y callback start Invalid job_src_id:%d!\n"

#define VPROC_ERR_JID_Y2Y_LDCBS_CODE 0x50016u
#define VPROC_ERR_JID_Y2Y_LDCBS_MSG "VPROC[chan_%d]: y2y ldelay callback start Invalid job_src_id:%d!\n"

#define VPROC_ERR_JID_Y2Y_LDLCBS_CODE 0x50017u
#define VPROC_ERR_JID_Y2Y_LDLCBS_MSG "VPROC[chan_%d]: y2y ldelay last row callback start Invalid job_src_id:%d!\n"

#define VPROC_ERR_JID_Y2Y_CBI_CODE 0x50018u
#define VPROC_ERR_JID_Y2Y_CBI_MSG "VPROC[chan_%d]: y2y callback issue Invalid job_src_id:%d!\n"

#define VPROC_ERR_JID_Y2Y_CBF_CODE 0x50019u
#define VPROC_ERR_JID_Y2Y_CBF_MSG "VPROC[chan_%d]: y2y callback finish Invalid job_src_id:%d!\n"

#define VPROC_ERR_TILEIDX_Y2Y_CODE 0x5001au
#define VPROC_ERR_TILEIDX_Y2Y_MSG "VPROC[chan_%d]: vproc_y2y_prep_ppl_idsp_cfg_fbs " \
                                  "tile_idx:%d not supported!\n"

#define VPROC_ERR_DQ_FULL_CODE 0x5001bu
#define VPROC_ERR_DQ_FULL_MSG "VPROC[chan_%d]: data Q full!\n"

#define VPROC_ERR_MAX_TILENUM_Y2Y_CODE 0x5001cu
#define VPROC_ERR_MAX_TILENUM_Y2Y_MSG "VPROC[chan_%d]: vproc_y2y_prep_ppl_data " \
                                      "total_tiles:%d > max_warp_tile_x_num=%d\n"

#define VPROC_ERR_Y2Y_DQ_EMPTY_CODE 0x5001du
#define VPROC_ERR_Y2Y_DQ_EMPTY_MSG "VPROC[chan_%d]: vproc_y2y_prep_idsp_jobs input Q empty\n"

#define VPROC_ERR_Y2Y_PPL_TYPE_CODE 0x5001eu
#define VPROC_ERR_Y2Y_PPL_TYPE_MSG "VPROC[chan_%d]: vproc_y2y_proc_job, " \
                                   "expect y2y_ppl_type:%d, but:%d\n"

#define VPROC_ERR_TS_Q_EMPTY_CODE 0x5001fu
#define VPROC_ERR_TS_Q_EMPTY_MSG "VPROC[chan_%d]: vproc_y2y_proc_job c2y_done_timestamp " \
                                 "queue empty when read!\n"

#define VPROC_ERR_PRC_JQ_EMPTY_CODE 0x50020u
#define VPROC_ERR_PRC_JQ_EMPTY_MSG "VPROC[chan_%d]: data Q empty!\n"

#define VPROC_ERR_MAIN_ME_FBID_CODE 0x50021u
#define VPROC_ERR_MAIN_ME_FBID_MSG "VPROC[chan_%d]: vproc_y2y_prep_sec3_fbs " \
                                   "main_fb_id:0x%x, me_fb_id:0x%x\n"

#define VPROC_ERR_MAIN_OUT_DRAM_CODE 0x50022u
#define VPROC_ERR_MAIN_OUT_DRAM_MSG "veffect no main_str output to dram TBD \n"

#define VPROC_ERR_VEMAIN_STR_FBID_CODE 0x50023u
#define VPROC_ERR_VEMAIN_STR_FBID_MSG "VPROC[chan_%d]: cap_seq_no:%d, passthrough ppl main_str " \
                                      "veffect invalid fid pic_fid:0x%x, me_fid:0x%x\n"

#define VPROC_ERR_MAIN_STR_VOUT0_CODE 0x50024u
#define VPROC_ERR_MAIN_STR_VOUT0_MSG "VPROC[chan_%d]: main_str out_dest:%d, to VOUT0\n"

#define VPROC_ERR_MAIN_STR_VOUT1_CODE 0x50025u
#define VPROC_ERR_MAIN_STR_VOUT1_MSG "VPROC[chan_%d]: main_str out_dest:%d, to VOUT1\n"

#define VPROC_ERR_Y2YS5_FBID_CODE 0x50026u
#define VPROC_ERR_Y2YS5_FBID_MSG "VPROC[chan_%d]: vproc_y2y_prep_sec5_fbs " \
                                 "pip_fb_id:0x%x, pip_me_fb_id:%d\n"

#define VPROC_ERR_Y2YS6_FBID_CODE 0x50027u
#define VPROC_ERR_Y2YS6_FBID_MSG "VPROC[chan_%d]: vproc_y2y_prep_sec6_fbs " \
                                 "3rd_str_fb_id:0x%x\n"

#define VPROC_ERR_Y2YS7_FBID_CODE 0x50028u
#define VPROC_ERR_Y2YS7_FBID_MSG "VPROC[chan_%d]: vproc_y2y_prep_sec7_fbs " \
                                 "4th_str_fb_id:0x%x\n"

#define VPROC_ERR_Y2YS17_Y_OVF_CODE 0x50029u
#define VPROC_ERR_Y2YS17_Y_OVF_MSG "VPROC[chan_%d]: vproc_y2y_prep_sec17_dram_str " \
                                   "buf_base_y:0x%x overflow, FB base:0x%x pitch:%d height:%d " \
                                   "check HIER size in CMD_VPROC_CONFIG, CMD_VPROC_SET_EXT_MEM, " \
                                   "or CMD_VPROC_SETUP\n"

#define VPROC_ERR_Y2YS17_UV_OVF_CODE 0x5002au
#define VPROC_ERR_Y2YS17_UV_OVF_MSG "VPROC[chan_%d]: vproc_y2y_prep_sec17_dram_str " \
                                    "buf_base_uv:0x%x overflow, FB base:0x%x pitch:%d height:%d " \
                                    "check HIER size in CMD_VPROC_CONFIG, CMD_VPROC_SET_EXT_MEM, " \
                                    "or CMD_VPROC_SETUP\n"

#define VPROC_ERR_VEPIP_STR_FBID_CODE 0x5002bu
#define VPROC_ERR_VEPIP_STR_FBID_MSG "VPROC[chan_%d]: cap_seq_no:%d, passthrough ppl pip_str " \
                                     "veffect invalid fid pic_fid:0x%x, me_fid:0x%x\n"

#define VPROC_ERR_PIP_STR_VOUT0_CODE 0x5002cu
#define VPROC_ERR_PIP_STR_VOUT0_MSG "VPROC[chan_%d]: pip_str out_dest:%d, to VOUT0\n"

#define VPROC_ERR_PIP_STR_VOUT1_CODE 0x5002du
#define VPROC_ERR_PIP_STR_VOUT1_MSG "VPROC[chan_%d]: pip_str out_dest:%d, to VOUT1\n"

#define VPROC_ERR_VE3RD_STR_FBID_CODE 0x5002eu
#define VPROC_ERR_VE3RD_STR_FBID_MSG "VPROC[chan_%d]: cap_seq_no:%d, passthrough ppl 3rd_str " \
                                     "veffect invalid fid pic_fid:0x%x\n"

#define VPROC_ERR_3RD_STR_PIN_CODE 0x5002fu
#define VPROC_ERR_3RD_STR_PIN_MSG "VPROC[chan_%d]: 3rd_str out_dest:%d, to PIN\n"

#define VPROC_ERR_3RD_STR_VOUT0_CODE 0x50030u
#define VPROC_ERR_3RD_STR_VOUT0_MSG "VPROC[chan_%d]: 3rd_str out_dest:%d, to VOUT0\n"

#define VPROC_ERR_VE4TH_STR_FBID_CODE 0x50031u
#define VPROC_ERR_VE4TH_STR_FBID_MSG "VPROC[chan_%d]: cap_seq_no:%d, passthrough ppl 4th_str " \
                                     "veffect invalid fid pic_fid:0x%x\n"

#define VPROC_ERR_4TH_STR_PIN_CODE 0x50032u
#define VPROC_ERR_4TH_STR_PIN_MSG "VPROC[chan_%d]: 4th_str out_dest:%d, to PIN\n"

#define VPROC_ERR_4TH_STR_VOUT1_CODE 0x50033u
#define VPROC_ERR_4TH_STR_VOUT1_MSG "VPROC[chan_%d]: 4th_str out_dest:%d, to VOUT1\n"

#define VPROC_ERR_MAX_TILE_NUM_CODE 0x50034u
#define VPROC_ERR_MAX_TILE_NUM_MSG "VPROC: vproc_flow_max_cfg_cmd max c2y tile num:%ux%u " \
                                   "max warp tile num:%ux%u VPROC_MAX_TILES_PER_FRAME:%u\n"

#define VPROC_ERR_GROUP_NUM_CODE 0x50035u
#define VPROC_ERR_GROUP_NUM_MSG  "VPROC:: num_vproc_groups:%d >VPROC_MAX_GRP_NUM:%d"

#define VPROC_ERR_UNHANDLED_CMD_CODE 0x50036u
#define VPROC_ERR_UNHANDLED_CMD_MSG "VPROC:: unhandled VPROC command:0x%08x\n"

#define VPROC_ERR_INPUT_FORMAT_CODE 0x50037u
#define VPROC_ERR_INPUT_FORMAT_MSG "VPROC[chan_%u]: vproc_rm_init invalid input_format:%u\n"

#define VPROC_ERR_PROC_IDX_CODE 0x50038u
#define VPROC_ERR_PROC_IDX_MSG "VPROC[chan_%d]: vproc_send_first_flow_idsp_job proc order " \
                               "check fail proc_idx curr:%d prev:%d\n"

#define VPROC_ERR_Y2Y_FIRST_FLOW_CODE 0x50039u
#define VPROC_ERR_Y2Y_FIRST_FLOW_MSG "VPROC[chan_%d]: vproc_update_first_flow_idsp_job "\
                                     "y2y first flow is not supported\n"

#define VPROC_ERR_IDSP_JOB_Q_C2Y_V_CODE 0x5003au
#define VPROC_ERR_IDSP_JOB_Q_C2Y_V_MSG "VPROC[chan_%d]: vproc_send_c2y_idsp_job: "\
                                       "IDSP_JOB_Q_C2Y_V full, Q length:%d, "\
                                       "c2y_job_cnt:%d@%u\n"

#define VPROC_ERR_C2Y_JOB_CNT_CODE 0x5003bu
#define VPROC_ERR_C2Y_JOB_CNT_MSG "VPROC[chan_%d]: vproc_send_c2y_idsp_job: "\
                                  "c2y_job_cnt:%d, but job_q_cb->lwq_cb.rd_items:0\n"

#define VPROC_ERR_IDSP_JOB_Q_FIN_V_CODE 0x5003cu
#define VPROC_ERR_IDSP_JOB_Q_FIN_V_MSG "VPROC[chan_%d]: vproc_send_y2y_idsp_job: "\
                                       "IDSP_JOB_Q_FIN_V full, Q length:%d, "\
                                       "y2y_job_cnt:%d@%u\n"

#define VPROC_ERR_JOB_CNT_CODE 0x5003du
#define VPROC_ERR_JOB_CNT_MSG \
  "VPROC[chan_%d]: job_cnt:%d, but job_q_cb->lwq_cb.rd_items:0\n"

#define VPROC_ERR_ORC_MSG_CHAN_ID_CODE 0x5003eu
#define VPROC_ERR_ORC_MSG_CHAN_ID_MSG "VPROC[chan_%d]: vproc_proc_orc_msgs "\
                                      "job_chan_id mismatch, job_chan_id:%d\n"

#define VPROC_ERR_ORC_MSG_SRC_ID_CODE 0x5003fu
#define VPROC_ERR_ORC_MSG_SRC_ID_MSG "VPROC[chan_%d]: vproc_proc_orc_msgs "\
                                     "job_src_id:%d mismatch,must be:%d\n"

#define VPROC_ERR_ORC_MSG_PASS_ID_CODE 0x50040u
#define VPROC_ERR_ORC_MSG_PASS_ID_MSG "VPROC[chan_%d]: vproc_proc_orc_msgs "\
                                      "invalid pass_id:%d idsp_job_id:0x%08x\n"

#define VPROC_ERR_ORC_MSG_ID_CODE 0x50041u
#define VPROC_ERR_ORC_MSG_ID_MSG "VPROC[chan_%d]: vproc_proc_orc_msgs "\
                                 "orc_msg.orc_msg_code:0x%08x\n"

#define VPROC_ERR_VP_MSG_Q_CODE 0x50042u
#define VPROC_ERR_VP_MSG_Q_MSG "VPROC[chan_%d]: vproc_send_msg_vp_hier_lane_out "\
                               "vp_msg_q full! rd:%d wr:%d max_num_msg:%d\n"

#define VPROC_ERR_VP_MSG_SYNC_CODE 0x50043u
#define VPROC_ERR_VP_MSG_SYNC_MSG "VPROC[chan_%d]: vproc_send_msg_vp_hier_lane_out "\
                                  "DSP_SYNC_COUNTER_IDSP_VP_MSG curr:%d max:%d\n"

#define VPROC_ERR_VP_MSG_TS_Q_CODE 0x50044u
#define VPROC_ERR_VP_MSG_TS_Q_MSG   "VPROC[chan_%d]: vproc_prep_send_msg_vp_hier_lane_out "\
                                    "c2y_done_timestamp queue empty when read! "\
                                    "check c2y and y2y ob processing order\n"

#define VPROC_ERR_VE_COPY_FBP_CODE 0x50045u
#define VPROC_ERR_VE_COPY_FBP_MSG "VPROC[chan_%d]: veffect_prep_copy_ppl_data "\
                                  "strm_%d run out of fbp_id(0x%0x) buf\n"

#define VPROC_ERR_VE_COPY_CR_FID_CODE 0x50046u
#define VPROC_ERR_VE_COPY_CR_FID_MSG "VPROC[chan_%d]: veffect_prep_copy_ppl_data "\
                                     "run out of copy_cr_cache_fid\n"

#define VPROC_ERR_VE_COPY_IN_DQ_CODE 0x50047u
#define VPROC_ERR_VE_COPY_IN_DQ_MSG "VPROC[chan_%d]: veffect_prep_copy_ppl_data "\
                                     "strm_%d input data Q overflow "\
                                     "effect_copy_data_in_cnt:%d, Q rd_items:%d\n"

#define VPROC_ERR_VE_COPY_DATA_CODE 0x50048u
#define VPROC_ERR_VE_COPY_DATA_MSG "VPROC[chan_%d]: veffect_prep_copy_jobs expect "\
                                    "%d jobs, but only %d copy_data prepared"

#define VPROC_ERR_VE_COPY_DQ_CODE 0x50049u
#define VPROC_ERR_VE_COPY_DQ_MSG "VPROC[chan_%d]: veffect_prep_copy_jobs "\
                                  "Y2Y input data Q empty\n"

#define VPROC_ERR_VE_COPY_DTYPE_CODE 0x5004au
#define VPROC_ERR_VE_COPY_DTYPE_MSG "VPROC[chan_%d]: veffect_prep_copy_jobs "\
                                    "data ppl_type mismatch, expect:%d, but:%d\n"

#define VPROC_ERR_VE_BLEND_FBP_CODE 0x5004bu
#define VPROC_ERR_VE_BLEND_FBP_MSG "VPROC[chan_%d]: veffect_prep_blend_ppl_data "\
                                  "strm_%d run out of fbp_id(0x%0x) buf\n"

#define VPROC_ERR_VE_BLEND_IN_DQ_CODE 0x5004cu
#define VPROC_ERR_VE_BLEND_IN_DQ_MSG "VPROC[chan_%d]: veffect_prep_blend_ppl_data "\
                                      "strm_%d input data Q overflow "\
                                      "effect_blend_data_in_cnt:%d, Q rd_items:%d\n"

#define VPROC_ERR_VE_BLEND_FID_CODE 0x5004du
#define VPROC_ERR_VE_BLEND_FID_MSG "VPROC[chan_%d]: veffect_prep_mctf_blend_dram_bds "\
                                   "Invalid blend_fid, buf_idx:%d\n"

#define VPROC_ERR_VE_BLEND_DATA_CODE 0x5004eu
#define VPROC_ERR_VE_BLEND_DATA_MSG "VPROC[chan_%d]: veffect_prep_blend_jobs expect "\
                                    "%d jobs, but only %d blend_data prepared"

#define VPROC_ERR_VE_BLEND_DQ_CODE 0x5004fu
#define VPROC_ERR_VE_BLEND_DQ_MSG "VPROC[chan_%d]: veffect_prep_blend_jobs "\
                                  "Y2Y input data Q empty\n"

#define VPROC_ERR_VE_BLEND_DTYPE_CODE 0x50050u
#define VPROC_ERR_VE_BLEND_DTYPE_MSG "VPROC[chan_%d]: veffect_prep_blend_jobs "\
                                     "data ppl_type mismatch, expect:%d, but:%d\n"

#define VPROC_ERR_SEND_BLEND_WAIT_CODE 0x50051u
#define VPROC_ERR_SEND_BLEND_WAIT_MSG "VPROC[chan_%d]: veffect_send_all_blend_jobs: "\
                                      "blend_job_cnt:%d < num_of_jobs:%d after "\
                                      "orc_sleep %d times@%u\n"

#define VPROC_ERR_SEND_BLEND_IENG_CODE 0x50052u
#define VPROC_ERR_SEND_BLEND_IENG_MSG "VPROC[chan_%d]: veffect_send_all_blend_jobs: "\
                                      "IENG IDSP_JOB_Q_FIN_V Q full, Q length:%d, "\
                                      "blend_job_cnt:%d\n"

#define VPROC_ERR_SEND_COPY_WAIT_CODE 0x50053u
#define VPROC_ERR_SEND_COPY_WAIT_MSG "VPROC[chan_%d]: veffect_send_all_copy_jobs: "\
                                      "copy_job_cnt:%d < num_of_jobs:%d after "\
                                      "orc_sleep %d times@%u\n"

#define VPROC_ERR_SEND_COPY_IENG_CODE 0x50054u
#define VPROC_ERR_SEND_COPY_IENG_MSG "VPROC[chan_%d]: veffect_send_all_copy_jobs: "\
                                      "IENG IDSP_JOB_Q_FIN_V Q full, Q length:%d, "\
                                      "copy_job_cnt:%d\n"

#define VPROC_ERR_PROC_COPY_Q_CODE 0x50055u
#define VPROC_ERR_PROC_COPY_Q_MSG "VPROC[chan_%d]: veffect_proc_copy_job data Q empty!\n"

#define VPROC_ERR_PROC_COPY_DTYPE_CODE 0x50056u
#define VPROC_ERR_PROC_COPY_DTYPE_MSG "VPROC[chan_%d]: veffect_proc_copy_job, "\
                                      "expect y2y_ppl_type:%d, but:%d\n"

#define VPROC_ERR_PROC_BLEND_Q_CODE 0x50057u
#define VPROC_ERR_PROC_BLEND_Q_MSG "VPROC[chan_%d]: veffect_proc_blend_job data Q empty!\n"

#define VPROC_ERR_PROC_BLEND_DTYPE_CODE 0x50058u
#define VPROC_ERR_PROC_BLEND_DTYPE_MSG "VPROC[chan_%d]: veffect_proc_blend_job, "\
                                      "expect y2y_ppl_type:%d, but:%d\n"

#define VPROC_ERR_PROC_BLEND_FIDS_CODE 0x50059u
#define VPROC_ERR_PROC_BLEND_FIDS_MSG "VPROC[chan_%d]: veffect_proc_blend_job "\
                                      "cap_seq_no:%u, strm_%d veffect invalid fid "\
                                      "pic_fid:0x%x, me_fid:0x%x\n"

#define VPROC_ERR_PROC_BLEND_FID_CODE 0x5005au
#define VPROC_ERR_PROC_BLEND_FID_MSG "VPROC[chan_%d]: veffect_proc_blend_job "\
                                      "cap_seq_no:%u, strm_%d veffect invalid fid "\
                                      "pic_fid:0x%x\n"

#define VPROC_ERR_VE_3RD_OUT_DEST_CODE 0x5005bu
#define VPROC_ERR_VE_3RD_OUT_DEST_MSG "VPROC[chan_%d]: veffect_proc_blend_job "\
                                      "3rd_str out_dest:%d, to %d\n"

#define VPROC_ERR_VE_4TH_OUT_DEST_CODE 0x5005cu
#define VPROC_ERR_VE_4TH_OUT_DEST_MSG "VPROC[chan_%d]: veffect_proc_blend_job "\
                                      "4th_str out_dest:%d, to %d\n"

#define VPROC_ERR_PREV_INS_FID_CODE 0x5005du
#define VPROC_ERR_PREV_INS_FID_MSG "PREV[%d]: vproc_prev_ins_new_pic invalid fid\n"

#define VPROC_ERR_PREV_INS_DISP_FID_CODE 0x5005eu
#define VPROC_ERR_PREV_INS_DISP_FID_MSG "PREV[%d]: vproc_prev_ins_new_pic state:%d "\
                                        "pic_info_q_rd:%d, disp_fid:0x%x\n"

#define VPROC_ERR_PREV_INS_VOUT_ID_CODE 0x5005fu
#define VPROC_ERR_PREV_INS_VOUT_ID_MSG "PREV[%d]: vproc_prev_ins_new_pic p_prev->vout_id:%d\n"

#define VPROC_ERR_PREV_SETUP_SRC_CODE 0x50060u
#define VPROC_ERR_PREV_SETUP_SRC_MSG "PREV[%d]: vproc_prev_setup_vout_input "\
                                     "source is not VPROC! src:%d\n"

#define VPROC_ERR_PREV_STOP_SRC_CODE 0x50061u
#define VPROC_ERR_PREV_STOP_SRC_MSG "PREV[%d]: vproc_prev_stop_vout_input "\
                                     "source is not VPROC! src:%d\n"

#define VPROC_ERR_PREV_PREP_SRC_CODE 0x50062u
#define VPROC_ERR_PREV_PREP_SRC_MSG "PREV[%d]: vproc_prev_prep_vout_input "\
                                     "source is not VPROC! src:%d\n"

#define VPROC_ERR_PREV_MODE_CODE 0x50063u
#define VPROC_ERR_PREV_MODE_MSG "PREV[%d]: unknown preview mode\n"

#define VPROC_ERR_PREV_STOP_FID_CODE 0x50064u
#define VPROC_ERR_PREV_STOP_FID_MSG "PREV[%d]: vproc_prev_stop_vout_input "\
                                    "stat:%d pic_info_q_rd:%d, disp_fid:0x%x\n"

#define VPROC_ERR_PREV_NEXT_FID_CODE 0x50065u
#define VPROC_ERR_PREV_NEXT_FID_MSG "PREV[%d]: vproc_prev_get_next_disp_fid next fid: 0x%x\n"

#define VPROC_ERR_VE_STRM_ID_CODE 0x50066u
#define VPROC_ERR_VE_STRM_ID_MSG "VPROC[chan_%d]: invalid strm_id:%d\n"

#define VPROC_ERR_VE_WR_IO_FBQ_CODE 0x50067u
#define VPROC_ERR_VE_WR_IO_FBQ_MSG "VPROC[chan_%d]: veffect_write_io_fbq is full\n"

#define VPROC_ERR_VE_PRIM_FBP_CODE 0x50068u
#define VPROC_ERR_VE_PRIM_FBP_MSG "VPROC[chan_%d]: run out of prim_fbp_id(0x%0x) buf\n"

#define VPROC_ERR_VE_SUB_FBP_CODE 0x50069u
#define VPROC_ERR_VE_SUB_FBP_MSG "VPROC[chan_%d]: run out of sub_fbp_id(0x%0x) buf\n"

#define VPROC_ERR_VE_PRIM_WORK_FBP_CODE 0x5006au
#define VPROC_ERR_VE_PRIM_WORK_FBP_MSG "VPROC[chan_%d]: run out of prim_output_work_fbp_id(0x%x) buf\n"

#define VPROC_ERR_VE_SUB_WORK_FBP_CODE 0x5006bu
#define VPROC_ERR_VE_SUB_WORK_FBP_MSG "VPROC[chan_%d]: run out of sub_output_work_fbp_id(0x%x) buf\n"

#define VPROC_ERR_VE_STAGING_FID_CODE 0x5006cu
#define VPROC_ERR_VE_STAGING_FID_MSG "VPROC[chan_%d_grp_%d]: strm_%d invalid staging_fid\n"

#define VPROC_ERR_VE_BUF_IDX_CODE 0x5006du
#define VPROC_ERR_VE_BUF_IDX_MSG "VPROC[chan_%d_grp_%d]: strm_%d more than %d buf_idx\n"

#define VPROC_ERR_VE_COPY_SIZE_CODE 0x5006eu
#define VPROC_ERR_VE_COPY_SIZE_MSG "VPROC[chan_%d]: Invalid copy_region size:%dx%d\n"

#define VPROC_ERR_VE_MAIN_SIZE_CODE 0x5006fu
#define VPROC_ERR_VE_MAIN_SIZE_MSG "VPROC[chan_%d]: VPROC_MAIN_STREAM(%d) does not support "\
                                   "runtime change size(%dx%d) vproc_state:%d\n"

#define VPROC_ERR_C2Y_DATA_Q_CODE 0x50070u
#define VPROC_ERR_C2Y_DATA_Q_MSG "VPROC[chan_%d]: vproc_c2y_enque_ppl_data data Q full!\n"

#define VPROC_ERR_C2Y_PREP_JOB_DQ_CODE 0x50071u
#define VPROC_ERR_C2Y_PREP_JOB_DQ_MSG "VPROC[chan_%d]: vproc_c2y_prep_job data Q empty\n"

#define VPROC_ERR_C2Y_PROC_JOB_DQ_CODE 0x50072u
#define VPROC_ERR_C2Y_PROC_JOB_DQ_MSG "VPROC[chan_%d]: vproc_c2y_proc_job data Q empty\n"

#define VPROC_ERR_C2Y_TS_Q_FULL_CODE 0x50073u
#define VPROC_ERR_C2Y_TS_Q_FULL_MSG "VPROC[chan_%d]: vproc_c2y_proc_job "\
                                    "c2y_done_time_stamp queue full!\n"

#define VPROC_ERR_CMD_CHAN_ID_CODE 0x50074u
#define VPROC_ERR_CMD_CHAN_ID_MSG "VPROC[chan_%d]: Invalid channel_id:%d\n"

#define VPROC_ERR_CMD_EXP_NUM_CODE 0x50075u
#define VPROC_ERR_CMD_EXP_NUM_MSG \
  "VPROC[chan_%d]: Invalid num_of_exp:0, check CMD_VPROC_SETUP\n"

#define VPROC_ERR_PREV_A_SZ_CODE 0x50076u
#define VPROC_ERR_PREV_A_SZ_MSG "VPROC[chan_%d]: vproc_init_prev_a_setting - "\
                                "prev_a cur size(%dx%d) invalid\n"

#define VPROC_ERR_PREV_A_SZ_MAX_CODE 0x50077u
#define VPROC_ERR_PREV_A_SZ_MAX_MSG "VPROC[chan_%d]: vproc_init_prev_a_setting - "\
                                    "prev_a cur size(%dx%d) >= max size(%dx%d)\n"

#define VPROC_ERR_PREV_B_SZ_CODE 0x50078u
#define VPROC_ERR_PREV_B_SZ_MSG "VPROC[chan_%d]: vproc_init_prev_b_setting - "\
                                "prev_b cur size(%dx%d) invalid\n"

#define VPROC_ERR_PREV_B_SZ_MAX_CODE 0x50079u
#define VPROC_ERR_PREV_B_SZ_MAX_MSG "VPROC[chan_%d]: vproc_init_prev_b_setting - "\
                                    "prev_b cur size(%dx%d) >= max size(%dx%d)\n"

#define VPROC_ERR_PREV_C_SZ_CODE 0x5007au
#define VPROC_ERR_PREV_C_SZ_MSG "VPROC[chan_%d]: vproc_init_prev_c_setting - "\
                                "prev_c cur size(%dx%d) invalid\n"

#define VPROC_ERR_PREV_C_SZ_MAX_CODE 0x5007bu
#define VPROC_ERR_PREV_C_SZ_MAX_MSG "VPROC[chan_%d]: vproc_init_prev_c_setting - "\
                                    "prev_c cur size(%dx%d) >= max size(%dx%d)\n"

#define VPROC_ERR_PREV_D_SZ_CODE 0x5007cu
#define VPROC_ERR_PREV_D_SZ_MSG "VPROC[chan_%d]: vproc_init_prev_d_setting - "\
                                "lane_det cur size(%dx%d) invalid\n"

#define VPROC_ERR_PREV_D_SZ_MAX_CODE 0x5007du
#define VPROC_ERR_PREV_D_SZ_MAX_MSG "VPROC[chan_%d]: vproc_init_prev_d_setting - "\
                                    "lane_det cur size(%dx%d) >= max size(%dx%d)\n"

#define VPROC_ERR_HIER_SZ_CODE 0x5007eu
#define VPROC_ERR_HIER_SZ_MSG "VPROC[chan_%d]: vproc_init_hier_setting - "\
                              "hier_poly cur size(%dx%d) invalid\n"

#define VPROC_ERR_HIER_SZ_MAX_CODE 0x5007fu
#define VPROC_ERR_HIER_SZ_MAX_MSG "VPROC[chan_%d]: vproc_init_hier_setting - "\
                                  "hier_poly cur size(%dx%d) >= max size(%dx%d)\n"

#define VPROC_ERR_LDELAY_C2Y_CODE 0x50080u
#define VPROC_ERR_LDELAY_C2Y_MSG "VPROC[chan_%u]: vproc_proc_vproc_setup "\
                                 "is_c2y_dram_sync_to_warp:%u is_li_enabled:%u "\
                                 "must be enabled\n"

#define VPROC_ERR_LDELAY_MCTF_CODE 0x50081u
#define VPROC_ERR_LDELAY_MCTF_MSG "VPROC[chan_%u]: vproc_proc_vproc_setup "\
                                  "is_c2y_dram_sync_to_warp:%u is_mctf_enabled:%u "\
                                  "must be enabled\n"

#define VPROC_ERR_C2Y_TILE_NUM_CODE 0x50082u
#define VPROC_ERR_C2Y_TILE_NUM_MSG "VPROC[chan_%u]: c2y tile num:%ux%u must be <= %u\n"

#define VPROC_ERR_Y2Y_TILE_NUM_CODE 0x50083u
#define VPROC_ERR_Y2Y_TILE_NUM_MSG "VPROC[chan_%u]: warp tile num:%ux%u must be <= %u\n"

#define VPROC_ERR_ROI_TAG_NUM_CODE 0x50084u
#define VPROC_ERR_ROI_TAG_NUM_MSG "VPROC[chan_%d]: vproc_proc_vproc_img_pyramid_setup: "\
                                  "roi tag buffer full!\n"

#define VPROC_ERR_GRP_CMD_NUM_CODE 0x50085u
#define VPROC_ERR_GRP_CMD_NUM_MSG "VPROC[chan_%u]: vproc_proc_vproc_grp_cmd "\
                                  "without cmds in buffer\n"

#define VPROC_ERR_GRP_CMD_SYS_CODE 0x50086u
#define VPROC_ERR_GRP_CMD_SYS_MSG "VPROC[chan_%u]: vproc_proc_vproc_grp_cmd "\
                                  "is_sent_from_sys:%u should always be true\n"

#define VPROC_ERR_GRP_CMD_CHAN_CODE 0x50087u
#define VPROC_ERR_GRP_CMD_CHAN_MSG "VPROC[chan_%u]: vproc_proc_vproc_grp_cmd "\
                                  "invalid channel_id(cmd):%u not allowed to "\
                                  "send group command self channel\n"

#define VPROC_ERR_ROI_START_COL_CODE 0x50088u
#define VPROC_ERR_ROI_START_COL_MSG "VPROC[chan_%d]: vproc_proc_vproc_lane_det_setup "\
                                    "start_col:%d>=img_width:%d\n"

#define VPROC_ERR_ROI_START_ROW_CODE 0x50089u
#define VPROC_ERR_ROI_START_ROW_MSG "VPROC[chan_%d]: vproc_proc_vproc_lane_det_setup "\
                                    "start_rol:%d>=img_height:%d\n"

#define VPROC_ERR_PREV_ID_CODE 0x5008au
#define VPROC_ERR_PREV_ID_MSG "VPROC[chan_%d]:: Invalid prev_id: %d\n"

#define VPROC_ERR_PIN_OUT_DECI_M_CODE 0x5008bu
#define VPROC_ERR_PIN_OUT_DECI_M_MSG "VPROC[chan_%u]: vproc_proc_vproc_pin_out_deci "\
                                     "invalid output_repeat_ratio:0x%x (M:0)\n"

#define VPROC_ERR_PIN_OUT_DECI_N_CODE 0x5008cu
#define VPROC_ERR_PIN_OUT_DECI_N_MSG "VPROC[chan_%u]: vproc_proc_vproc_pin_out_deci "\
                                     "invalid output_repeat_ratio:0x%x (N:0)\n"

#define VPROC_ERR_EXT_MEM_TYPE_CODE 0x5008du
#define VPROC_ERR_EXT_MEM_TYPE_MSG "VPROC[chan_%d]: vproc_proc_vproc_set_ext_mem "\
                                   "Invalid ext buf type:%d\n"

#define VPROC_ERR_EXT_BUF_ADDR_CODE 0x5008eu
#define VPROC_ERR_EXT_BUF_ADDR_MSG "VPROC[chan_%d]: vproc_proc_vproc_set_ext_mem "\
                                   "null buf_addr!\n"

#define VPROC_ERR_VE_GRP_ID_CODE 0x5008fu
#define VPROC_ERR_VE_GRP_ID_MSG "VPROC:: vproc_proc_multi_stream_pp: effect_grp_id:%d "\
                                "VEFFECT_MAX_GRP_NUM:%d\n"

#define VPROC_ERR_VE_FINAL_BUF_ID_CODE 0x50090u
#define VPROC_ERR_VE_FINAL_BUF_ID_MSG "VPROC:: vproc_proc_multi_stream_pp: "\
                                      "final_output_buf_id:%d >= 255\n"

#define VPROC_ERR_VE_INPUT_NUMS_CODE 0x50091u
#define VPROC_ERR_VE_INPUT_NUMS_MSG "VPROC:: vproc_proc_multi_stream_pp: num_of_inputs:%d > %d\n"

#define VPROC_ERR_VE_STRM_ID_MIS_CODE 0x50092u
#define VPROC_ERR_VE_STRM_ID_MIS_MSG "VPROC[chan_%d]: vproc_proc_multi_stream_pp, %d-th "\
                                     "pp_strm_cntl strm_id:%d mismatch, cmd->str_id:%d\n"

#define VPROC_ERR_VE_OUT_BUF_ID_CODE 0x50093u
#define VPROC_ERR_VE_OUT_BUF_ID_MSG "VPROC[chan_%d]: vproc_proc_multi_stream_pp: "\
                                    "output_buf_id:%d, max_num_io_fbq:%d\n"

#define VPROC_ERR_VE_CHAN_ID_CODE 0x50094u
#define VPROC_ERR_VE_CHAN_ID_MSG "VPROC[chan_%d]: vproc_proc_multi_stream_pp: "\
                                 "input_channel_num:%d, num_of_chan:%d\n"

#define VPROC_ERR_VE_CH_STRM_ID_CODE 0x50095u
#define VPROC_ERR_VE_CH_STRM_ID_MSG "VPROC[chan_%d]: vproc_proc_multi_stream_pp: "\
                                    "input_stream_num:%d, VPROC_STREAM_NUMS:%d\n"

#define VPROC_ERR_VE_CHROMA_FMT_CODE 0x50096u
#define VPROC_ERR_VE_CHROMA_FMT_MSG "VPROC[chan_%d]: vproc_proc_set_effect_buf_img_sz: "\
                                    "invalid cmd->ch_fmt:%d, ch_fmt%d!\n"

#define VPROC_ERR_VE_PRIM_FBP_ID_CODE 0x50097u
#define VPROC_ERR_VE_PRIM_FBP_ID_MSG "VPROC[chan_%d]: vproc_proc_set_effect_buf_img_sz "\
                                     "invalid prim_fbp_id(0x%x)!\n"

#define VPROC_ERR_GRP_CH_NUM_CODE 0x50098u
#define VPROC_ERR_GRP_CH_NUM_MSG "VPROC[grp_%d]: vproc_proc_multi_chan_proc_order "\
                                  "cmd->num_of_vproc_channel:%d, vproc_cb->num_of_chan:%d\n"

#define VPROC_ERR_GRP_NUM_CODE 0x50099u
#define VPROC_ERR_GRP_NUM_MSG "VPROC:: vproc_proc_set_proc_grping "\
                              "num_of_grp:%d != vproc_cb->num_of_vproc_grp:%d\n"

#define VPROC_ERR_GRPING_CH_NUM_CODE 0x5009au
#define VPROC_ERR_GRPING_CH_NUM_MSG "VPROC[grp_%d]: vproc_proc_set_proc_grping "\
                                    "num_vprocs_per_group:%d, vproc_cb->vproc_num_of_chan:%d\n"

#define VPROC_ERR_BAT_PROC_ORDER_CODE 0x5009bu
#define VPROC_ERR_BAT_PROC_ORDER_MSG "VPROC[chan_%d]: CMD_VPROC_MULTI_CHAN_PROC_ORDER(0x%x) "\
                                     "MUST BE placed in master channel batch cmd set, "\
                                     "proc_idx:%d\n"

#define VPROC_ERR_BAT_PROC_GRPING_CODE 0x5009cu
#define VPROC_ERR_BAT_PROC_GRPING_MSG "VPROC[chan_%d]: CMD_VPROC_MULTI_CHAN_PROC_ORDER(0x%x) "\
                                      "MUST BE placed in master channel batch cmd set, "\
                                      "proc_idx:%d\n"

#define VPROC_ERR_SMEM_WIN_WIDTH_CODE 0x5009du
#define VPROC_ERR_SMEM_WIN_WIDTH_MSG "VPROC[chan_%d]: SMEM_WIN width:%d out of range, "\
                                     "padded_width:%d\n"

#define VPROC_ERR_VE_BLEND_CR_FID_CODE 0x5009eu
#define VPROC_ERR_VE_BLEND_CR_FID_MSG "VPROC[chan_%d]: veffect_prep_blend_ppl_data "\
                                     "run out of blend_cr_cache_fid\n"

#define VPROC_ERR_BAD_VALUE_CODE 0x5009fu
#define VPROC_ERR_BAD_VALUE_MSG                                            \
  "VPROC[chan_%u]: detected a bad value:%u debugging value1:%u value2:%u " \
  "value3:%u\n"

#define VPROC_ERR_NULL_IK_ADDR_CODE 0x500a0u
#define VPROC_ERR_NULL_IK_ADDR_MSG \
  "VPROC[chan_%u]: null ik addr, check CMD_VPROC_IK_CONFIG\n"

#define VPROC_ERR_INVALID_FB_ID_CODE 0x500a1u
#define VPROC_ERR_INVALID_FB_ID_MSG \
  "VPROC[chan_%u]: invalid frame buffer ID, fbp_id:0x%x\n"

#define VPROC_ERR_ABNORMAL_TILE_CODE 0x500a2u
#define VPROC_ERR_ABNORMAL_TILE_MSG \
  "VPROC[chan_%u]: detected abnormal tile passed:%u failed:%u\n"

#define VPROC_ERR_SETUP_CMD_CODE 0x500a3u
#define VPROC_ERR_SETUP_CMD_MSG \
  "VPROC[chan_%u]: bad setup command %u %u %u %u\n"

#define VPROC_ERR_GRP_CH_RT_CHG_CODE 0x500a4u
#define VPROC_ERR_GRP_CH_RT_CHG_MSG                  \
  "VPROC[grp_%d]: vproc_proc_multi_chan_proc_order " \
  "the runtime change must be sent "                 \
  "after the last chan and before the 1st chan\n"

#define VPROC_ERR_GRP_CH_TIMEOUT_CODE 0x500a5u
#define VPROC_ERR_GRP_CH_TIMEOUT_MSG "VPROC[grp_%d]: vproc_proc_multi_chan_proc_order "\
                                     "the runtime change is timeout\n"

#define VPROC_ERR_IDSP_TIMEOUT_CODE 0x500a6u
#define VPROC_ERR_IDSP_TIMEOUT_MSG                                    \
  "VPROC[chan_%u]: idsp time out! cap_seq_no:%u job_pass_step_id:%u " \
  "job_id:0x%x hw_duration:%u\n"

#define VPROC_ERR_FLOW_MAX_CFG_CODE 0x500a7u
#define VPROC_ERR_FLOW_MAX_CFG_MSG \
  "VPROC: CMD_DSP_VPROC_FLOW_MAX_CFG failed check! check index:%u value:%u\n"

#define VPROC_ERR_OSD_BLEND_CMD_CODE 0x500a8u
#define VPROC_ERR_OSD_BLEND_CMD_MSG \
  "VPROC[chan_%u]: CMD_VPROC_OSD_BLEND bad osd blend command %u %u %u %u\n"

#define VPROC_ERR_MISMATCH_IK_IF_VER_CODE 0x500a9u
#define VPROC_ERR_MISMATCH_IK_IF_VER_MSG \
  "VPROC[chan_%u]: IK interface version mismatches %u %u\n"

#define VPROC_ERR_OSD_INSERT_CMD_CODE 0x500aau
#define VPROC_ERR_OSD_INSERT_CMD_MSG \
  "VPROC[chan_%u]: CMD_VPROC_OSD_INSERT bad osd insert command %u %u %u %u\n"

#define VPROC_ERR_IDSP_FLOW_INFO_CODE 0x500abu
#define VPROC_ERR_IDSP_FLOW_INFO_MSG \
  "VPROC[chan_%u]: IDSP FLOW INFO failed check! %u %u %u %u\n"

#define VPROC_ERR_BOUNDING_CHECK_CODE 0x500acu
#define VPROC_ERR_BOUNDING_CHECK_MSG \
  "VPROC: bounding check fail (silicon_version:%d): vproc throughput (K):%d over limit:%d (K) - margin:%d\n"

#define VPROC_ERR_INVALID_REGION_CODE 0x500adu
#define VPROC_ERR_INVALID_REGION_MSG \
  "VPROC[chan_%u]: Invalid region off_x %d off_y %d width %d height %d\n"

#define VPROC_ERR_STA_BUF_CFG_NUM_CODE 0x500aeu
#define VPROC_ERR_STA_BUF_CFG_NUM_MSG "VPROC:Invalid staging buf cfg num %d, it should be no bigger than %d\n"

#define VPROC_ERR_BLUR_PM_CODE 0x500afu
#define VPROC_ERR_BLUR_PM_MSG \
  "VPROC: CMD_VPROC_BLUR_PM_PP bad blur pm command %u %u %u %u %u\n"

#define VPROC_ERR_NUM 0xb0u


extern const char * const vproc_err_code[VPROC_ERR_NUM];


/*****************************************************************************
 * module_idx 7 ERR_CODE_MOD_VIN
 *****************************************************************************/
#define VIN_ERR_UNKNOWN_CODE            0x70000u
#define VIN_ERR_UNKNOWN_MSG             "VIN: unknown error arg0:%d arg1:%d arg2:%d arg3:%d arg4:%d\n"

#define EC_VIN_API                      0x70001u
#define EM_VIN_API                      "VIN[%d]: API ERROR! cmd_code=0x%08x\n"

#define EC_VIN_BAD_VIN_THREAD_ID        0x70002u
#define EM_VIN_BAD_VIN_THREAD_ID        "VIN: invalid vin_thread_id:%d!\n"

#define EC_VIN_NULL_CMD_INFO_TBL        0x70003u
#define EM_VIN_NULL_CMD_INFO_TBL        "VIN: null cmd_info_tbl!\n"

#define EC_VIN_ASSERT_CHECK_AES         0x70004u
#define EM_VIN_ASSERT_CHECK_AES         "VIN: assert check aes encrypt/decrypt if the same\n"

#define EC_VIN_SC_CHECK_FAIL            0x70005u
#define EM_VIN_SC_CHECK_FAIL            "VIN: vin_main() sc error\n"

#define EC_VIN_MISMATCH_VIN_ID          0x70006u
#define EM_VIN_MISMATCH_VIN_ID          "VIN: mismatch vin_id(%d:%d)\n"

#define EC_VIN_BAD_YUV_ENC_FBP_ID       0x70007u
#define EM_VIN_BAD_YUV_ENC_FBP_ID       "VIN[%d]: invalid vin_yuv_enc_fbp_id!\n"

#define EC_VIN_BAD_DEFAULT_RAW_FB_ID    0x70008u
#define EM_VIN_BAD_DEFAULT_RAW_FB_ID    "VIN[%d]:: invalid default_raw_img_fb_id(0x%x)\n"

#define EC_VIN_BAD_DEFAULT_CE_FB_ID     0x70009u
#define EM_VIN_BAD_DEFAULT_CE_FB_ID     "VIN[%d]: invalid default_ce_img_fb_id(0x%x)\n"

#define EC_VIN_BAD_SDBD_INFO_FOV_ID     0x7000au
#define EM_VIN_BAD_SDBD_INFO_FOV_ID     "VIN[%d]: invalid sideband_info fov_id:%d max_fov_num:%d\n"

#define EC_VIN_NULL_CMD_CHAN_DADDR      0x7000bu
#define EM_VIN_NULL_CMD_CHAN_DADDR      "VIN: null cmd_dram_addr:0x%x!\n"

#define EC_VIN_SHORT_OF_DRAM            0x7000cu
#define EM_VIN_SHORT_OF_DRAM            "VIN: not enough dram, short %d\n"

#define EC_VIN_SHORT_OF_SMEM            0x7000du
#define EM_VIN_SHORT_OF_SMEM            "VIN: not enough smem, short %d\n"

#define EC_VIN_CAP_RM_INIT_NOT_SET      0x7000eu
#define EM_VIN_CAP_RM_INIT_NOT_SET      "VIN[%d]: is_vin_cap_rm_init:%d should be set!\n"

#define EC_VIN_BAD_FB                   0x7000fu
#define EM_VIN_BAD_FB                   "VIN[%d]: invalid vin fb!\n"

#define EC_VIN_OP_FAIL                  0x70010u
#define EM_VIN_OP_FAIL                  "VIN: operation fail, result:%d\n"

#define EC_VIN_YUV422_OUT_NOT_EN        0x70011u
#define EM_VIN_YUV422_OUT_NOT_EN        "VIN[%d]: is_yuv422_out_enabled:%d should be set!\n"

#define EC_VIN_SHORT_OF_DATA_SMEM       0x70012u
#define EM_VIN_SHORT_OF_DATA_SMEM       "VIN: not enough data smem, short %d\n"

#define EC_VIN_CAP_RM_INIT_FLAG         0x70013u
#define EM_VIN_CAP_RM_INIT_FLAG         "VIN: is_vin_cap_rm_init:%d should be zero!\n"

#define EC_VIN_MISMATCH_IDSP_STR_ID     0x70014u
#define EM_VIN_MISMATCH_IDSP_STR_ID     "VIN: mismatch idsp_str_id(%d:%d)\n"

#define EC_VIN_NULL_FOV_FBP_IDS         0x70015u
#define EM_VIN_NULL_FOV_FBP_IDS         "VIN[%d]: null vin_fov_fbp_ids!\n"

#define EC_VIN_BAD_VIN_FOV_FB_ID        0x70016u
#define EM_VIN_BAD_VIN_FOV_FB_ID        "VIN: invalid vin_fov_fb_id!\n"

#define EC_VIN_SEND_RAW_STATUS_VP_Q     0x70017u
#define EM_VIN_SEND_RAW_STATUS_VP_Q     "VIN[%d]: invalid send_raw_status_vp_q flag!\n"

#define EC_VIN_BAD_LUMA_OUT_FB          0x70018u
#define EM_VIN_BAD_LUMA_OUT_FB          "VIN[%d]: invalid yuv422 luma out fb!\n"

#define EC_VIN_BAD_CHROMA_OUT_FB        0x70019u
#define EM_VIN_BAD_CHROMA_OUT_FB        "VIN[%d]: invalid yuv422 chroma out fb!\n"

#define EC_VIN_BAD_CE_OUT_FB            0x7001au
#define EM_VIN_BAD_CE_OUT_FB            "VIN[%d]: invalid ce out fb!\n"

#define EC_VIN_BAD_AUX_OUT_FB           0x7001bu
#define EM_VIN_BAD_AUX_OUT_FB           "VIN[%d]: invalid aux out fb!\n"

#define EC_VIN_WRONG_PIC_STRM           0x7001cu
#define EM_VIN_WRONG_PIC_STRM           "VIN[%d]: wrong pic strm err:%d!\n"

#define EC_VIN_BAD_INT_OUT_FB           0x7001du
#define EM_VIN_BAD_INT_OUT_FB           "VIN[%d]: invalid internal out fb!\n"

#define EC_VIN_NON_ZERO_RESET_CNT       0x7001eu
#define EM_VIN_NON_ZERO_RESET_CNT       "VIN: non-zero vin_reset_cnt:%d!\n"

#define EC_VIN_BAD_PREV_RAW_IMG_FB_ID   0x7001fu
#define EM_VIN_BAD_PREV_RAW_IMG_FB_ID   "VIN[%d]: invalid prev_raw_img_fb_id!\n"

#define EC_VIN_BAD_PREV_CE_IMG_FB_ID    0x70020u
#define EM_VIN_BAD_PREV_CE_IMG_FB_ID    "VIN[%d]: invalid prev_ce_img_fb_id!\n"

#define EC_VIN_NULL_DATA_Q_OUT          0x70021u
#define EM_VIN_NULL_DATA_Q_OUT          "VIN[%d]: null data_q_out! Check CMD_DSP_CONFIG vin[0-5]_output_pins or CMD_VIN_START output_dest\n"

#define EC_VIN_MISMATCH_BATCH_ADDR      0x70022u
#define EM_VIN_MISMATCH_BATCH_ADDR      "VIN[%d]: mismatch batch_cmd_set_addr(cache):0x%x and batch_cmd_set_addr(cap_info):0x%x\n"

#define EC_VIN_MISMATCH_BATCH_ID        0x70023u
#define EM_VIN_MISMATCH_BATCH_ID        "VIN[%d]: mismatch batch_cmd_set_id(cache):0x%x and batch_cmd_set_id(cap_info):0x%x\n"

#define EC_VIN_MISMATCH_BATCH_SZ        0x70024u
#define EM_VIN_MISMATCH_BATCH_SZ        "VIN[%d]: mismatch batch_cmd_set_size(cache):%d and batch_cmd_set_size(cap_info):%d\n"

#define EC_VIN_BAD_PIC_REPEAT_CNT       0x70025u
#define EM_VIN_BAD_PIC_REPEAT_CNT       "VIN[%d]: invalid pic repeat_cnt:0x%x m:%d n:%d!\n"

#define EC_VIN_BAD_SRC_FP_NUM           0x70026u
#define EM_VIN_BAD_SRC_FP_NUM           "VIN: invalid src_fp_num(vin_id):%d max_vin_num:%d\n"

#define EC_VIN_BAD_VPROC_BINDING_NUM    0x70027u
#define EM_VIN_BAD_VPROC_BINDING_NUM    "VIN: vin_vproc_binding_num:%d > vproc_num:%d! Check vproc num in CMD_DSP_CONFIG or CMD_DSP_BINDING_CFG num!\n"

#define EC_VIN_BAD_ENC_BINDING_NUM      0x70028u
#define EM_VIN_BAD_ENC_BINDING_NUM      "VIN: vin_enc_binding_num:%d > enc_num:%d! Check enc num in CMD_DSP_CONFIG or CMD_DSP_BINDING_CFG num!\n"

#define EC_VIN_BAD_ENC_BIND_SRC_CH_NUM  0x70029u
#define EM_VIN_BAD_ENC_BIND_SRC_CH_NUM  "VIN: vin->enc channel num:%d\n"

#define EC_VIN_BAD_DST_FP_TYPE          0x7002au
#define EM_VIN_BAD_DST_FP_TYPE          "VIN: invalid dst_fp_type:%d\n"

#define EC_VIN_BAD_BIND_MSG             0x7002bu
#define EM_VIN_BAD_BIND_MSG             "VIN: vin has not assigned a out msg q yet!\n"

#define EC_VIN_BAD_BIND_SRC_CH_NUM      0x7002cu
#define EM_VIN_BAD_BIND_SRC_CH_NUM      "VIN: data_q_out src_ch_num (%d) >= data_q_out_num (%d) !\n"

#define EC_VIN_NULL_CE_FOV_FBP_IDS      0x7002du
#define EM_VIN_NULL_CE_FOV_FBP_IDS      "VIN[%d]: null vin chroma fov ids!\n"

#define EC_VIN_BAD_ENC_YUV_OUT_FB_ID    0x7002eu
#define EM_VIN_BAD_ENC_YUV_OUT_FB_ID    "VIN[%d]: invalid vin->enc yuv out_fb_id:0x%x\n"

#define EC_VIN_BAD_ENC_ME_OUT_FB_ID     0x7002fu
#define EM_VIN_BAD_ENC_ME_OUT_FB_ID     "VIN[%d]: invalid vin->enc me out_fb_id_me:0x%x\n"

#define EC_VIN_RUN_OUT_EXT_YUV_FB       0x70030u
#define EM_VIN_RUN_OUT_EXT_YUV_FB       "VIN[%u]: vin_yuv422_req_ext_output_buf run out ext_mem FB\n"

#define EC_VIN_WRONG_DRAM               0x70031u
#define EM_VIN_WRONG_DRAM               "VIN: wrong dram, daddr 0x%x\n"

#define VIN_ERR_NUM 0x32u

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
/* misrac: identifier length should less then 31 */

#define HLP_ERRC_UNKNOWN                0x90000u
#define HLP_ERRM_UNKNOWN                "HL_PROC: unknown error arg0:%d arg1:%d arg2:%d arg3:%d arg4:%d\n"
#define HLP_ERRC_STR_DMA_PADDR_NULL     0x90001u
#define HLP_ERRM_STR_DMA_PADDR_NULL     "HL_PROC[chan_%d]: null strs_dma_params_addr!\n"
#define HLP_ERRC_WRONG_DMA_WH           0x90002u
#define HLP_ERRM_WRONG_DMA_WH           "HL_PROC[chan_%d]: abnormal DMA WxH:%dx%d!\n"
#define HLP_ERRC_BLENDSBUF_TARIN_ROT    0x90003u
#define HLP_ERRM_BLENDSBUF_TARIN_ROT    "HL_PROC[chan_%d]: BLEND SBUF tar input with rotation is not supported\n"
#define HLP_ERRC_BLENDSBUF_REFIN_ROT    0x90004u
#define HLP_ERRM_BLENDSBUF_REFIN_ROT    "HL_PROC[chan_%d]: BLEND SBUF ref input with rotation is not supported\n"
#define HLP_ERRC_INVALID_SEC_ID         0x90005u
#define HLP_ERRM_INVALID_SEC_ID         "HL_PROC[chan_%d]:Invalid idsp sec_id:%d\n"
#define HLP_ERRC_NS_MCTF_TAR_SBUF       0x90006u
#define HLP_ERRM_NS_MCTF_TAR_SBUF       "prepare_mctf_smem_str Not support Target SBUF mode now\n"
#define HLP_ERRC_MAXITEMS_POWER2        0x90007u
#define HLP_ERRM_MAXITEMS_POWER2        "HL_PROC[chan_%d]: HL_PROC_LWQ requires power of 2 max_items\n"
#define HLP_ERRC_INVALID_LWQ_STORAGE    0x90008u
#define HLP_ERRM_INVALID_LWQ_STORAGE    "HL_PROC[chan_%d]: hl_proc_lwq_init invalid HL_PROC_LWQ storage_type:%d\n"
#define HLP_ERRC_LWQ_FULL               0x90009u
#define HLP_ERRM_LWQ_FULL               "HL_PROC[chan_%d]: hl_proc_lwq_wr HL_PROC_LWQ is full\n"
#define HLP_ERRC_WRITEQ_LWQ_FULL        0x9000au
#define HLP_ERRM_WRITEQ_LWQ_FULL        "HL_PROC[chan_%d]: hl_proc_write_queue HL_PROC_LWQ is full\n"
#define HLP_ERRC_WRITEQ_STORAGE_TYPE    0x9000bu
#define HLP_ERRM_WRITEQ_STORAGE_TYPE    "HL_PROC[chan_%d]: write queue Invalid HL_PROC_LWQ storage type:%d\n"
#define HLP_ERRC_PEEKQ_STORAGE_TYPE     0x9000cu
#define HLP_ERRM_PEEKQ_STORAGE_TYPE     "HL_PROC[chan_%d]: peek queue Invalid HL_PROC_LWQ storage type:%d\n"
#define HLP_ERRC_PEEKPROCQ_STORAGE_T    0x9000du
#define HLP_ERRM_PEEKPROCQ_STORAGE_T    "HL_PROC[chan_%d]: hl_proc_peek_proc_queue: Invalid HL_PROC_LWQ storage type:%d\n"
#define HLP_ERRC_READQ_STORAGE_T        0x9000eu
#define HLP_ERRM_READQ_STORAGE_T        "HL_PROC[chan_%d]: Invalid HL_PROC_LWQ storage type:%d\n"
#define HLP_ERRC_PROCQ_STARGE_T         0x9000fu
#define HLP_ERRM_PROCQ_STARGE_T         "HL_PROC[chan_%d]: proc queue Invalid HL_PROC_LWQ storage type:%d\n"
#define HLP_ERRC_UPDATEQ_STORAGE_T      0x90010u
#define HLP_ERRM_UPDATEQ_STORAGE_T      "HL_PROC[chan_%d]: update queue Invalid HL_PROC_LWQ storage type:%d\n"
#define HLP_ERRC_REQEXT_RUNOUTFB        0x90011u
#define HLP_ERRM_REQEXT_RUNOUTFB        "HL_PROC[chan_%d]: run out ext_mem FB\n"
#define HLP_ERRC_OUTOF_MFB              0x90012u
#define HLP_ERRM_OUTOF_MFB              "HL_PROC[chan_%d]: run out ext_mem FB\n"
#define HLP_ERRC_SETEXTM_BUFPITCH       0x90013u
#define HLP_ERRM_SETEXTM_BUFPITCH       "HL_PROC[chan_%d]: hl_proc_set_ext_mem() buf_width:%d buf_pitch:%d is too small!\n"
#define HLP_ERRC_SETEXTM_MAX_BUFS       0x90014u
#define HLP_ERRM_SETEXTM_MAX_BUFS       "HL_PROC[chan_%d]: hl_proc_set_ext_mem() brand new allocation mode, num_frm_buf:%d is too large, max:%d\n"
#define HLP_ERRC_SETEXTM_INVALID_FBP    0x90015u
#define HLP_ERRM_SETEXTM_INVALID_FBP    "HL_PROC[chan_%d]: hl_proc_set_ext_mem() invalid frame buffer pool\n"
#define HLP_ERRC_SETEXTM_APPEND_FBS     0x90016u
#define HLP_ERRM_SETEXTM_APPEND_FBS     "HL_PROC[chan_%d]: hl_proc_set_ext_mem() append mode, num_frm_buf:%d is too large, max:%d, used:%d, free:%d\n"
#define HLP_ERRC_SETEXTM_NS_ALLOCMODE   0x90017u
#define HLP_ERRM_SETEXTM_NS_ALLOCMODE   "HL_PROC[chan_%d]: hl_proc_set_ext_mem() allocation_mode:%d is not supported\n"
#define HLP_ERRC_SETEXTM_NS_ALLOCTYPE   0x90018u
#define HLP_ERRM_SETEXTM_NS_ALLOCTYPE   "HL_PROC[chan_%d]: hl_proc_set_ext_mem() allocation_type:%d is not supported\n"
#define HLP_ERRC_NOIMP_SBUF             0x90019u
#define HLP_ERRM_NOIMP_SBUF             "HL_PROC: hl_proc_ppl_out_str_cfg_init() smem sbuf mode not implemented!\n"
#define HLP_ERRC_MISMATCH_MEM_SZ        0x9001au
#define HLP_ERRM_MISMATCH_MEM_SZ        "HL_PROC: Mismatch memoey size!\n"
#define HLP_ERRC_DIAG_CHECK             0x9001bu
#define HLP_ERRM_DIAG_CHECK             "diag check failed - arg0:%d arg1:%d arg2:%d arg3:%d arg4:%d \n"
#define HLP_ERRC_INVALID_CR_BASE        0x9001cu
#define HLP_ERRM_INVALID_CR_BASE        "HL_PROC[chan_%d]: hl_proc_idsp_prep_sec%d_bb: cr_base NULL\n"
#define HLP_ERRC_OUTPUT_RATIO           0x9001du
#define HLP_ERRM_OUTPUT_RATIO          "HL_PROC: Output ratio (%d/%d) needs to be less than %d\n"


#define HLP_ERR_NUM 0x1eu

extern const char * const hl_proc_err_code[HLP_ERR_NUM];

/*****************************************************************************
 * module_idx 10 ERR_CODE_MOD_VOUT
 *****************************************************************************/
#define VOUT_EC_UNKNOWN 0xa0000u
#define VOUT_EM_UNKNOWN "VOUT: unknown error arg0:%d arg1:%d arg2:%d arg3:%d arg4:%d\n"

#define VOUT_EC_VOUTA_DISP_NOT_IDLE 0xa0001u
#define VOUT_EM_VOUTA_DISP_NOT_IDLE "VOUT-A: display section should be idle before config\n"

#define VOUT_EC_VOUTA_NG_SETUP_DISP_VAL 0xa0002u
#define VOUT_EM_VOUTA_NG_SETUP_DISP_VAL "VOUT-A: unexpected setup_display value 0x%x\n"

#define VOUT_EC_VOUTA_NG_DEF_IMG_Y_ADDR 0xa0003u
#define VOUT_EM_VOUTA_NG_DEF_IMG_Y_ADDR "setupVoutDefaultImg() Invalid default_image_y_addr 0x%x, vout_id=%d\n"

#define VOUT_EC_VOUT_SRC_WHEN_SETUP 0xa0004u
#define VOUT_EM_VOUT_SRC_WHEN_SETUP "vout_setup_video_source() Invalid Vout source %d. vout_id=%d\n"

#define VOUT_EC_INVALID_PREV_VOUTA_SRC 0xa0005u
#define VOUT_EM_INVALID_PREV_VOUTA_SRC "VOUT: Invalid previous VoutA source %d\n"

#define VOUT_EC_INVALID_NEW_VOUTA_SRC 0xa0006u
#define VOUT_EM_INVALID_NEW_VOUTA_SRC "VOUT: Invalid new VoutA source %d\n"

#define VOUT_EC_VOUT_SRC_WHEN_PREP 0xa0007u
#define VOUT_EM_VOUT_SRC_WHEN_PREP "VOUT: Invalid Vout source %d. vout_id=%d\n"

#define VOUT_EC_WRONG_CURRENT_VOUT_ID 0xa0008u
#define VOUT_EM_WRONG_CURRENT_VOUT_ID "VOUT: context_switch_to_vout() Wrong current vout id %d ! \n"

#define VOUT_EC_WRONG_NEXT_VOUT_ID 0xa0009u
#define VOUT_EM_WRONG_NEXT_VOUT_ID "VOUT: context_switch_to_vout() Wrong next vout id ! \n"

#define VOUT_EC_WRONG_COUNTER_VAL 0xa000au
#define VOUT_EM_WRONG_COUNTER_VAL "VOUT: vout_wait_for_vin_sync() wrong counter_value: %d. \n"

#define VOUT_EC_NEXT_FRM_A_TOO_LARGE 0xa000bu
#define VOUT_EM_NEXT_FRM_A_TOO_LARGE "VOUT:: vout_handle_multi_vout() next_frm_a:%u >= frm_rate_a_ticks:%u, vout_ts[0]:%u curr_time:%u\n"

#define VOUT_EC_NEXT_FRM_B_TOO_LARGE 0xa000cu
#define VOUT_EM_NEXT_FRM_B_TOO_LARGE "VOUT:: vout_handle_multi_vout() next_frm_b:%u >= frm_rate_b_ticks:%u, vout_ts[1]:%u curr_time:%u\n"

#define VOUT_EC_VOUTB_DISP_NOT_IDLE 0xa000du
#define VOUT_EM_VOUTB_DISP_NOT_IDLE "VOUT-B: display section should be idle before config\n"

#define VOUT_EC_VOUTB_NG_SETUP_DISP_VAL 0xa000eu
#define VOUT_EM_VOUTB_NG_SETUP_DISP_VAL "VOUT-B: unexpected setup_display value 0x%x\n"

#define VOUT_EC_VOUTB_NG_DEF_IMG_Y_ADDR 0xa000fu
#define VOUT_EM_VOUTB_NG_DEF_IMG_Y_ADDR "Invalid default_image_y_addr 0x%x\n"

#define VOUT_EC_VOUTB_SRC_WHEN_SETUP 0xa0010u
#define VOUT_EM_VOUTB_SRC_WHEN_SETUP "Invalid VoutB source %d\n"

#define VOUT_EC_INVALID_PREV_VOUTB_SRC 0xa0011u
#define VOUT_EM_INVALID_PREV_VOUTB_SRC "VOUT-B: Invalid previous VoutB source %d\n"

#define VOUT_EC_INVALID_NEW_VOUTB_SRC 0xa0012u
#define VOUT_EM_INVALID_NEW_VOUTB_SRC "VOUT-B: Invalid new VoutB source %d\n"

#define VOUT_EC_VOUTB_SRC_WHEN_PREP 0xa0013u
#define VOUT_EM_VOUTB_SRC_WHEN_PREP "VOUT-B: Invalid VoutB source %d\n"

#define VOUT_EC_WRITE_CMD_Q_SBASE_NULL 0xa0014u
#define VOUT_EM_WRITE_CMD_Q_SBASE_NULL "VOUT-(%d) vout_write_dsp_cmd_q dsp_cmd_q_sbase is NULL\n"

#define VOUT_EC_READ_CMD_Q_SBASE_NULL 0xa0015u
#define VOUT_EM_READ_CMD_Q_SBASE_NULL "VOUT: vout_read_dsp_cmd_q dsp_cmd_q_sbase is NULL, vout_id:%d\n"

#define VOUT_EC_INVALID_VOUT_ID_IN_CMD 0xa0016u
#define VOUT_EM_INVALID_VOUT_ID_IN_CMD "VOUT: vout_dispatch_dsp_cmd() Invalid vout_id=%d in command\n"

#define VOUT_EC_MIXER_CMD_VID_NG 0xa0017u
#define VOUT_EM_MIXER_CMD_VID_NG "VOUT:: procVoutMixerSetuptCmd() mixer_cmd->vout_id:%d not match with g_vout->vout_id:%d\n"

#define VOUT_EC_VIDEO_CMD_VID_NG 0xa0018u
#define VOUT_EM_VIDEO_CMD_VID_NG "VOUT:: procVoutVideoSetuptCmd() video_cmd->vout_id:%d not match with g_vout->vout_id:%d\n"

#define VOUT_EC_OSD_CMD_VID_NG 0xa0019u
#define VOUT_EM_OSD_CMD_VID_NG "VOUT:: procVoutOsdSetuptCmd() osd_cmd->vout_id:%d not match with g_vout->vout_id:%d\n"

#define VOUT_EC_OSD_BUF_CMD_VID_NG 0xa001au
#define VOUT_EM_OSD_BUF_CMD_VID_NG "VOUT:: procVoutOsdBufSetuptCmd() osd_buf_cmd->vout_id:%d not match with g_vout->vout_id:%d\n"

#define VOUT_EC_OSD_CLUT_CMD_VID_NG 0xa001bu
#define VOUT_EM_OSD_CLUT_CMD_VID_NG "VOUT:: procVoutOsdClutSetuptCmd() osd_clut_cmd->vout_id:%d not match with g_vout->vout_id:%d\n"

#define VOUT_EC_DISP_CMD_VID_NG 0xa001cu
#define VOUT_EM_DISP_CMD_VID_NG "VOUT:: procVoutDisplaySetuptCmd() disp_cmd->vout_id:%d not match with g_vout->vout_id:%d\n"

#define VOUT_EC_RESET_CMD_VID_NG 0xa001du
#define VOUT_EM_RESET_CMD_VID_NG "VOUT:: procVoutResetCmd() reset_cmd->vout_id:%d not match with g_vout->vout_id:%d\n"

#define VOUT_EC_CSC_CMD_VID_NG 0xa001eu
#define VOUT_EM_CSC_CMD_VID_NG "VOUT:: procVoutDisplayCscSetuptCmd() csc_cmd->vout_id:%d not match with g_vout->vout_id:%d\n"

#define VOUT_EC_OUTPUT_MODE_CMD_VID_NG 0xa001fu
#define VOUT_EM_OUTPUT_MODE_CMD_VID_NG "VOUT:: procVoutDigitalOutputModeSetupCmd() output_mode_cmd->vout_id:%d not match with g_vout->vout_id:%d\n"

#define VOUT_EC_GAMMA_SETUP_CMD_VID_NG 0xa0020u
#define VOUT_EM_GAMMA_SETUP_CMD_VID_NG "VOUT:: procVoutAGammaSetupCmd() cmd->vout_id:%d not match with g_vout->vout_id:%d\n"

#define VOUT_EC_VOUTA_NG_CSC_TYPE 0xa0021u
#define VOUT_EM_VOUTA_NG_CSC_TYPE "VOUT-A: unsupported csc type %d\n"

#define VOUT_EC_VOUTB_NG_CSC_TYPE 0xa0022u
#define VOUT_EM_VOUTB_NG_CSC_TYPE "VOUT-B: unsupported csc type %d\n"

#define VOUT_EC_NG_CMD_FROM_VOUT_CMD 0xa0023u
#define VOUT_EM_NG_CMD_FROM_VOUT_CMD "VOUT:: Unknown cmd_code=0x%08x, (vout_id=%d)\n"

#define VOUT_EC_NG_NUM_CMDS_IN_BUF 0xa0024u
#define VOUT_EM_NG_NUM_CMDS_IN_BUF "VOUT: vout_proc_vout_pic_info() invalid num_cmds_in_buf\n"

#define VOUT_EC_NG_CMD_FROM_PIC_INFO 0xa0025u
#define VOUT_EM_NG_CMD_FROM_PIC_INFO "VOUT:: Unknown vout cmd_code=0x%08x, (vout_id=%d)\n"

// Used on vout side
#define VOUT_EC_PREV_INS_FID 0xa0026u
#define VOUT_EM_PREV_INS_FID "VOUT[%d]: vout_ins_pic_info invalid fid dst_vout_id:%d\n"

#define VOUT_EC_PREV_INS_DISP_FID 0xa0027u
#define VOUT_EM_PREV_INS_DISP_FID "VOUT: vproc_prev_ins_new_pic state:%d "\
                                        "pic_info_q_rd:%d, disp_fid:0x%x\n"

#define VOUT_EC_PREV_INS_VOUT_ID 0xa0028u
#define VOUT_EM_PREV_INS_VOUT_ID "VOUT: vproc_prev_ins_new_pic p_prev->vout_id:%d\n"

#define VOUT_EC_PREV_SETUP_SRC 0xa0029u
#define VOUT_EM_PREV_SETUP_SRC "VOUT: vproc_prev_setup_vout_input "\
                                     "source is not VPROC! src:%d\n"

#define VOUT_EC_PREV_STOP_SRC 0xa002au
#define VOUT_EM_PREV_STOP_SRC "VOUT[%d]: vout_stop_input_from_prev "\
                                     "input is not started\n"

#define VOUT_EC_PREV_PREP_SRC 0xa002bu
#define VOUT_EM_PREV_PREP_SRC "VOUT: vproc_prev_prep_vout_input "\
                                     "source is not VPROC! src:%d\n"

#define VOUT_EC_PREV_MODE 0xa002cu
#define VOUT_EM_PREV_MODE "VOUT[%d]: unknown preview mode\n"

#define VOUT_EC_PREV_STOP_FID 0xa002du
#define VOUT_EM_PREV_STOP_FID "VOUT[%d]: vproc_prev_stop_vout_input "\
                                    "dst_vout_id:%d stat:%d disp_fid:0x%x\n"

#define VOUT_EC_PREV_NEXT_FID 0xa002eu
#define VOUT_EM_PREV_NEXT_FID "VOUT[%d]: vproc_prev_get_next_disp_fid next fid: 0x%x\n"

#define VOUT_EC_CH_ID_EXCEED_LMT 0xa002fu
#define VOUT_EM_CH_ID_EXCEED_LMT "VOUT: get_vout_input_q_id channel_id %d exceeds VOUT_MAX_CHANNELS %d\n"

#define VOUT_EC_NOT_ENOUGH_DRAM 0xa0030u
#define VOUT_EM_NOT_ENOUGH_DRAM  "VOUT: not enough dram, short %d\n"

#define VOUT_EC_DISP_CMD_DISPLAY_C 0xa0031u
#define VOUT_EM_DISP_CMD_DISPLAY_C "VOUT:: procVoutDisplaySetuptCmd() g_vout->vout_id:%d not support Display C\n"

#define VOUT_EC_NOT_ENOUGH_SMEM 0xa0032u
#define VOUT_EM_NOT_ENOUGH_SMEM  "VOUT: not enough smem, short %d\n"

#define VOUT_EC_WRONG_VOUT_BIT_MASK 0xa0033u
#define VOUT_EM_WRONG_VOUT_BIT_MASK "VOUT: initMultiVoutCB(): Wrong VOUT_BIT_MASK\n"

#define VOUT_EC_NG_SELECT_CHAN 0xa0034u
#define VOUT_EM_NG_SELECT_CHAN "VOUT: selected_chan=%d active_chans=%d\n"

#define VOUT_EC_QUEUE_EMPTY 0xa0035u
#define VOUT_EM_QUEUE_EMPTY "VOUT: queue empty!\n"

#define VOUT_EC_QUEUE_FULL 0xa0036u
#define VOUT_EM_QUEUE_FULL "VOUT: queue full!\n"

#define VOUT_EC_ACTIVATE_CHAN 0xa0037u
#define VOUT_EM_ACTIVATE_CHAN "VOUT: activate chan %d but max_virtual_chan_num=%d !\n"

#define VOUT_ERR_NUM 0x38u

extern const char * const vout_err_code[VOUT_ERR_NUM];

/*****************************************************************************
 * module_idx 11 ERR_CODE_MOD_ENC
 *****************************************************************************/
#define ENC_ERRC_ENC_CFG_DADDR_INVALID 0xb0000u
#define ENC_ERRM_ENC_CFG_DADDR_INVALID  "ENC_BOOT: hlenc_load_encoder_config() enc_cfg_daddr is NULL\n"

#define ENC_ERRC_MAX_NUM_ENC_INVALID 0xb0001u
#define ENC_ERRM_MAX_NUM_ENC_INVALID  "ENC_BOOT: create_encoders() max_num_enc (%d) is over limit (%d)\n"

#define ENC_ERRC_BDTMGR_POOL_SIZE_INVALID 0xb0002u
#define ENC_ERRM_BDTMGR_POOL_SIZE_INVALID  "ENC_BOOT: create_encoders() Need to expand bdtmgr_t definition\n"

#define ENC_ERRC_SMEM_BUF_OVERFLOW 0xb0003u
#define ENC_ERRM_SMEM_BUF_OVERFLOW  "ENC_BOOT: create_encoder_dram_smem_(): smem buffer overflow allocated=%d, used=%d\n"

#define ENC_ERRC_IN_Q_CHAN_NUM_INVALID 0xb0004u
#define ENC_ERRM_IN_Q_CHAN_NUM_INVALID  "ENC: hlenc_get_input_q_id() invalid ch_num=%d enc_stream_num=%d\n"

#define ENC_ERRC_OUT_Q_CHAN_NUM_INVALID 0xb0005u
#define ENC_ERRM_OUT_Q_CHAN_NUM_INVALID  "ENC: hlenc_get_output_pin_addr() invalid ch_num=%d enc_stream_num=%d\n"

#define ENC_ERRC_OUT_Q_BIND_TYPE_INVALID 0xb0006u
#define ENC_ERRM_OUT_Q_BIND_TYPE_INVALID  "ENC: encoder doesn't support this bind: dst_fp_type=%d bind_msg=%d data_q_out_num=%d\n"

#define ENC_ERRC_CMD_Q_CHAN_NUM_INVALID 0xb0007u
#define ENC_ERRM_CMD_Q_CHAN_NUM_INVALID  "ENC: hlenc_dispatch_dsp_cmd() invalid channel_id=%d enc_stream_num=%d\n"

#define ENC_ERRC_SBUF_DESC_IDX_INVALID 0xb0008u
#define ENC_ERRM_SBUF_DESC_IDX_INVALID  "ENC: memd_calc_smem_buffer_size coding_type=%d invalid smem_index=%d\n"

#define ENC_ERRC_ENC_CMEM_GP_OVERFLOW 0xb0009u
#define ENC_ERRM_ENC_CMEM_GP_OVERFLOW  "ENC: cmem_gp overflow! cmem_pos_gp=%d > ENC_CMEM_GP_SIZE=%d\n"

#define ENC_ERRC_ENC_CMEM_POS_NOT_ZERO 0xb000au
#define ENC_ERRM_ENC_CMEM_POS_NOT_ZERO  "ENC: cmem_pos gp=%d rc=%d doesn't go back to 0, boot=%d\n"

#define ENC_ERRC_MEMCOPY_SIZE_INVALID 0xb000bu
#define ENC_ERRM_MEMCOPY_SIZE_INVALID  "ENC: hlenc_memcpy(): size=%d must be multiple of 4\n"

#define ENC_ERRC_SET_DBUF_DESC_TYPE_INVALID 0xb000cu
#define ENC_ERRM_SET_DBUF_DESC_TYPE_INVALID  "ENC: hlenc_memd_set_buf_desc_dram_one_imgbuf: Illegal luma_chroma (%d)!\n"

#define ENC_ERRC_SET_DBUF_DESC_DPITCH_INVALID 0xb000du
#define ENC_ERRM_SET_DBUF_DESC_DPITCH_INVALID  "ENC: hlenc_memd_set_buf_desc_dram_one_imgbuf: Illegal external dpitch (%d)!\n"

#define ENC_ERRC_UNKNOWN_CODING_TYPE 0xb000eu
#define ENC_ERRM_UNKNOWN_CODING_TYPE  "ENC: unknown coding_type=%d\n"

#define ENC_ERRC_INCORRECT_PROXY_CABAC_STATE 0xb000fu
#define ENC_ERRM_INCORRECT_PROXY_CABAC_STATE  "ENC: Incorrect proxy cabac state, h264/HEVC avai %d/%d\n"

#define ENC_ERRC_BDTNOTE_INVALID 0xb0010u
#define ENC_ERRM_BDTNOTE_INVALID  "BDTMGR: node_idx(=%d) > pool_size(=%d)\n"

#define ENC_ERRC_NOT_SUPPORT_B_LT 0xb0011u
#define ENC_ERRM_NOT_SUPPORT_B_LT  "hlenc_get_new_lt_frm_idx: Not support to encode B frame as LT\n"

#define ENC_ERRC_HIERB_GOP_INVALID 0xb0012u
#define ENC_ERRM_HIERB_GOP_INVALID  "hlenc_core_update_gop_for_hierb(): Error: Invalid GOP (M, N) (%d, %d) values for Hier B. N must be an integer mulitple of M\n"

#define ENC_ERRC_HIERP_GOP_INVALID 0xb0013u
#define ENC_ERRM_HIERP_GOP_INVALID  "hlenc_core_update_gop_for_hierp(): ERROR! improper gop_m (%d) for HierP\n"

#define ENC_ERRC_FAST_SEEK_LTR_INVALID 0xb0014u
#define ENC_ERRM_FAST_SEEK_LTR_INVALID  "hlenc_core_update_gop_for_fast_seek(): ERROR! long term reference must be enabled\n"

#define ENC_ERRC_POC_FALLBACK_INVALID 0xb0015u
#define ENC_ERRM_POC_FALLBACK_INVALID  "hlenc_core_set_pic_property_reorder(): ERROR! improper POC_FALLBACK_REDUCTION and POC_FALLBACK_THRESHOLD\n"

#define ENC_ERRC_UNKNOWN_ENC_STATE 0xb0016u
#define ENC_ERRM_UNKNOWN_ENC_STATE  "hlenc_core_check_enc_state unknown enc_state=%d\n"

#define ENC_ERRC_FBN_FULL 0xb0017u
#define ENC_ERRM_FBN_FULL  "buffer full. count=%d max=%d\n"

#define ENC_ERRC_FBN_FB_ID_INVALID 0xb0018u
#define ENC_ERRM_FBN_FB_ID_INVALID  "hlenc_fbmgr_store_picture_to_queue(): Got invalid fb_id. pic=0x%x me1=0x%x\n"

#define ENC_ERRC_FBN_FREE_NODE_INVALID 0xb0019u
#define ENC_ERRM_FBN_FREE_NODE_INVALID  "MMCO: ERROR! dpb full but can't find buffer to release\n"

#define ENC_ERRC_FBN_LTR_MMCO6_INVALID 0xb001au
#define ENC_ERRM_FBN_LTR_MMCO6_INVALID  "hlenc_fbmgr_find_node_to_release: ERROR! MMCO6 != 1 for curr pic when a LTR is being released!\n"

#define ENC_ERRC_FBN_COUNT_OVER_DBP 0xb001bu
#define ENC_ERRM_FBN_COUNT_OVER_DBP  "MMCO: Error: freed count(=%d) is more than dpb_fullness(=%d)\n"

#define ENC_ERRC_FBN_COUNT_INVALID 0xb001cu
#define ENC_ERRM_FBN_COUNT_INVALID  "check if any memory leakage on frame buffer\n"

#define ENC_ERRC_DBP_FULL 0xb001du
#define ENC_ERRM_DBP_FULL  "MMCO: DPB full. check DPB_SIZE=%d\n"

#define ENC_ERRC_RECON_FB_ID_INVALID 0xb001eu
#define ENC_ERRM_RECON_FB_ID_INVALID  "Recon buffers are all in use\n"

#define ENC_ERRC_MMCO_PHASE_INVALID 0xb001fu
#define ENC_ERRM_MMCO_PHASE_INVALID  "hlenc_fbmgr_do_buffer_management() unsupport phase=%d\n"

#define ENC_ERRC_UNKNOWN_PIC_TYPE 0xb0020u
#define ENC_ERRM_UNKNOWN_PIC_TYPE  "MEMD: unknown HL pict_type=%d\n"

#define ENC_ERRC_UNSUPPORT_CODING_TYPE 0xb0021u
#define ENC_ERRM_UNSUPPORT_CODING_TYPE  "not ready to support coding_type=%d\n"

#define ENC_ERRC_REF_NUM_INVALID 0xb0022u
#define ENC_ERRM_REF_NUM_INVALID  "invalid number of references %d\n"

#define ENC_ERRC_DBUF_DESC_IDX_INVALID 0xb0023u
#define ENC_ERRM_DBUF_DESC_IDX_INVALID  "MEMD: memd_calc_dram_buffer_size coding_type=%d invalid dram_index=%d\n"

#define ENC_ERRC_PP_SMEM_SIZE_INVALID 0xb0024u
#define ENC_ERRM_PP_SMEM_SIZE_INVALID  "MEMD: allcated smem buffer is not enough. coding_type %d is_shared %d, Need %d but only allocated %d\n"

#define ENC_ERRC_RPLR_LTR_0 0xb0025u
#define ENC_ERRM_RPLR_LTR_0  "RPLR: hlenc_rplr_reorder_list(): ERROR! sequence has no long-term reference enabled\n"

#define ENC_ERRC_RPLR_LTR_1 0xb0026u
#define ENC_ERRM_RPLR_LTR_1  "RPLR: hlenc_rplr_reorder_list(): ERROR! long-term reference unsupported for interlaced sequences\n"

#define ENC_ERRC_RPLR_LTR_2 0xb0027u
#define ENC_ERRM_RPLR_LTR_2  "RPLR: hlenc_rplr_reorder_list(): ERROR! lt%d_reorder_ref_idx should smaller than listXsize\n"

#define ENC_ERRC_RPLR_LTR_3 0xb0028u
#define ENC_ERRM_RPLR_LTR_3  "RPLR: Not supported B frame with a long term ref in List1[0]\n"

#define ENC_ERRC_RPLR_PIC_DIFF_INVALID 0xb0029u
#define ENC_ERRM_RPLR_PIC_DIFF_INVALID  "RPLR: hlenc_rplr_get_diff_pic_num_minus1(): ERROR! diff_pic_num <= 0\n"

#define ENC_ERRC_RPLR_REORDER_CNT_INVALID 0xb002au
#define ENC_ERRM_RPLR_REORDER_CNT_INVALID  "RPLR: hlenc_rplr_set_avc_sh_header(): ERROR! (reorder_counter >= NUM_REFS_MAX) should not happen! rplr_sh_syntax array size is too small.\n"

#define ENC_ERRC_AVC_SH_PIC_DIFF_INVALID 0xb002bu
#define ENC_ERRM_AVC_SH_PIC_DIFF_INVALID  "RPLR: hlenc_rplr_prepare_avc_sh_syntax(): ERROR! rplr_sh_syntax_coded has 6 bits but not enough for abs_diff_pic_num_minus1 >= 64. list=%d ref=%d (abs_diff_pic_num_minus1+1)=%d\n"

#define ENC_ERRC_DBP_REF_NUM_INVALID 0xb002cu
#define ENC_ERRM_DBP_REF_NUM_INVALID  "RPLR: ERROR! too many references (%d)) in dpb(=%d)!\n"

#define ENC_ERRC_API_0 0xb002du
#define ENC_ERRM_API_0  "ENC: bad value error arg0:0x%x arg1:0x%x arg2:0x%x arg3:0x%x arg4:0x%x\n"

#define ENC_ERRC_API_1 0xb002eu
#define ENC_ERRM_API_1  "bits_fifo_size is not multiple of %d aligned or info_fifo_size is not multiple of %d aligned\n"

#define ENC_ERRC_API_2 0xb002fu
#define ENC_ERRM_API_2  "Unsupported gop_m=%d for hier-b gop"

#define ENC_ERRC_API_3 0xb0030u
#define ENC_ERRM_API_3  "Error: Invalid GOP (M, N) (%d, %d) values for Hier B. N must be an integer mulitple of M"

#define ENC_ERRC_API_4 0xb0031u
#define ENC_ERRM_API_4  "Unsupported gop_m=%d for hier-p gop\n"

#define ENC_ERRC_API_5 0xb0032u
#define ENC_ERRM_API_5  "fast_seek_interval=%d is not dividable by gop_m=%d for fast seek gop\n"

#define ENC_ERRC_API_6 0xb0033u
#define ENC_ERRM_API_6  "gop_n=%d smaller than fast_seek_interval=%d\n"

#define ENC_ERRC_API_7 0xb0034u
#define ENC_ERRM_API_7  "Too small numRef_P=%d for GOP_SIMPLE\n"

#define ENC_ERRC_API_8 0xb0035u
#define ENC_ERRM_API_8  "Unsupported intra refresh type=%d\n"

#define ENC_ERRC_API_9 0xb0036u
#define ENC_ERRM_API_9  "mvdump_dpitch must be a multiple of 32\n"

#define ENC_ERRC_API_10 0xb0037u
#define ENC_ERRM_API_10  "invalid stat info size\n"

#define ENC_ERRC_API_11 0xb0038u
#define ENC_ERRM_API_11  "stat info size must be multiple of %d\n"

#define ENC_ERRC_API_12 0xb0039u
#define ENC_ERRM_API_12  "max stat info index must be no greater than %d\n"

#define ENC_ERRC_API_13 0xb003au
#define ENC_ERRM_API_13  "ERROR: enc_stop_method must be 0 if start_method is 0\n"

#define ENC_ERRC_API_14 0xb003bu
#define ENC_ERRM_API_14  "ERROR: check size: seq_par_t=%d jpeg_seq_t=%d\n"

#define ENC_ERRC_API_15 0xb003cu
#define ENC_ERRM_API_15  "not support command.code=0x%08x in state=%d\n"

#define ENC_ERRC_RC_0 0xb003du
#define ENC_ERRM_RC_0  "cmem_rc running out! cmem_pos_rc=%d > ENC_CMEM_RC_SIZE(=%d)\n"

#define ENC_ERRC_RC_1 0xb003eu
#define ENC_ERRM_RC_1  "Did not recognize LevelIDC %d\n"

#define ENC_ERRC_RC_2 0xb003fu
#define ENC_ERRM_RC_2  "ERROR: wrong pic type for svct\n"

#define ENC_ERRC_RC_3 0xb0040u
#define ENC_ERRM_RC_3  "HardwareCategory_16 Error: need implement Qmul_4~Qmul_15 for rc_pic_type=%d\n"

#define ENC_ERRC_RC_4 0xb0041u
#define ENC_ERRM_RC_4  "frm_rc_cntl/frm_rc_cntl may have changed frequencies; need to figure out how to compbine with layers\n"

#define ENC_ERRC_RC_5 0xb0042u
#define ENC_ERRM_RC_5  "Complexity overflow\n"

#define ENC_ERRC_RC_6 0xb0043u
#define ENC_ERRM_RC_6  "transition_rc: not implement with 16 hardware yet!\n"

#define ENC_ERRC_RC_7 0xb0044u
#define ENC_ERRM_RC_7  "RC ERROR: act_mem_struct->act_cnt only handle up to 65535 mb\n"

#define ENC_ERRC_RC_8 0xb0045u
#define ENC_ERRM_RC_8  "hlenc_rc_create_resource: rc_data_smem_base = 0!\n"

#define ENC_ERRC_RC_9 0xb0046u
#define ENC_ERRM_RC_9  "unknown RC enable mode=%d\n"

#define ENC_ERRC_RC_10 0xb0047u
#define ENC_ERRM_RC_10  "Wrong level number %d\n"

#define ENC_ERRC_RC_11 0xb0048u
#define ENC_ERRM_RC_11  "simple RC only support VBR_MODE=3\n"

#define ENC_ERRC_RC_12 0xb0049u
#define ENC_ERRM_RC_12  "Wrong picture_type %d\n"

#define ENC_ERRC_RC_13 0xb004au
#define ENC_ERRM_RC_13  "find_max_qp: mod_pic_type is not supported\n"

#define ENC_ERRC_RC_14 0xb004bu
#define ENC_ERRM_RC_14  "find_min_qp: mod_pic_type is not supported\n"

#define ENC_ERRC_INVALID_CHIP_ID 0xb004cu
#define ENC_ERRM_INVALID_CHIP_ID  "invalid chip ID\n"

#define ENC_ERRC_VDSP_BOUNDING_CHECK 0xb004du
#define ENC_ERRM_VDSP_BOUNDING_CHECK  "VDSP bounding check fail (silicon_version:%d): enc + dec throughput (K):(%d + %d) over limit %d (K) + margin:%d\n"

#define ENC_ERRC_INVALID_AU_TYPE 0xb004eu
#define ENC_ERRM_INVALID_AU_TYPE  "invalid au type\n"

#define ENC_ERRC_INVALID_STREAM_DUMMY_LATENCY 0xb004fu
#define ENC_ERRM_INVALID_STREAM_DUMMY_LATENCY "Invalid stream dummy latency %d, should be smaller than %d\n"

#define ENC_ERRC_INVALID_ENC_SYNC_CMD_ADDR  0xb0050u
#define ENC_ERRM_INVALID_ENC_SYNC_CMD_ADDR  "Invalid enc sync cmd addr 0x%x\n"

#define ENC_ERR_NUM 0x51u

extern const char * const enc_err_code[ENC_ERR_NUM];

/*****************************************************************************
 * module_idx 12 ERR_CODE_MOD_ENG0
 *****************************************************************************/
#define ENG0_ERRC_UNKNOWN 0xc0000u
#define ENG0_ERRM_UNKNOWN "ENG0: unknown error arg0:%d arg1:%d arg2:%d arg3:%d arg4:%d\n"

#define ENG0_ERRC_NULL_PTR 0xc0001u
#define ENG0_ERRM_NULL_PTR "ENG0: pointer is NULL\n"

#define ENG0_ERRC_NOT_SUPPORTED 0xc0002u
#define ENG0_ERRM_NOT_SUPPORTED "ENG0: not supported or unknown type/case.\n"

#define ENG0_ERRC_RPLR_MAX_ELE 0xc0003u
#define ENG0_ERRM_RPLR_MAX_ELE "ENG0: rplr L0 index=%d >= RPLR_MAX_ELEMENT!!\n"

#define ENG0_ERRC_MMCO_MAX_ELE 0xc0004u
#define ENG0_ERRM_MMCO_MAX_ELE "ENG0: mmco index=%d >= MMCO_MAX_ELEMENT!!\n"

#define ENG0_ERRC_CMEM_GP_SIZE 0xc0005u
#define ENG0_ERRM_CMEM_GP_SIZE "ENG0: overflow! cmem_gp_pos=%d > ENG0_CMEM_GP_SIZE(=%d)\n"

#define ENG0_ERRC_NAL_REF_IDC 0xc0006u
#define ENG0_ERRM_NAL_REF_IDC "ENG0: nal_reference_idc should be in 0-3 but got %d\n"

#define ENG0_ERRC_NAL_UNIT_TYPE 0xc0007u
#define ENG0_ERRM_NAL_UNIT_TYPE "ENG0: nal_unit_type should be in 0-10 (H264) or 0-9, 16-21, 32-40 (HEVC) but got %d\n"

#define ENG0_ERRC_MAX_RBSP_SIZE 0xc0008u
#define ENG0_ERRM_MAX_RBSP_SIZE "ENG0: MAX_RBSP_SIZE(=%d) is too small. rbsp_size=%d\n"

#define ENG0_ERRC_FORBIDDEN_BIT 0xc0009u
#define ENG0_ERRM_FORBIDDEN_BIT "ENG0: forbidden_bit == 0\n"

#define ENG0_ERRC_START_CODE_PREF_LEN 0xc000au
#define ENG0_ERRM_START_CODE_PREF_LEN "ENG0: startcodeprefix_len = 3 or 4, but got %d\n"

#define ENG0_ERRC_ENC_ERROR 0xc000bu
#define ENG0_ERRM_ENC_ERROR "ENG0: encode ERROR! max_pic_num=%d abs_diff_pic_num_minus1=%d\n"

#define ENG0_ERRC_DEC_ERROR 0xc000cu
#define ENG0_ERRM_DEC_ERROR "ENG0: decode ERROR! max_pic_num=%d rplr_sh_syntax_coded_lsb7=%d\n"

#define ENG0_ERRC_Q_EMPTY 0xc000du
#define ENG0_ERRM_Q_EMPTY "ENG0: q empty!!!\n"

#define ENG0_ERRC_Q_FULL 0xc000eu
#define ENG0_ERRM_Q_FULL "ENG0: q fuill!!!\n"

#define ENG0_ERRC_FRM_JOB_IDX 0xc000fu
#define ENG0_ERRM_FRM_JOB_IDX "ENG0: Check frame_job_idx fail!!!\n"

#define ENG0_ERRC_SINGLE_CODEC_TYPE 0xc0010u
#define ENG0_ERRM_SINGLE_CODEC_TYPE "ENG0: Single codec type is allowed in sub q 0!\n"

#define ENG0_ERRC_DIFF_CODEC_GROUP 0xc0011u
#define ENG0_ERRM_DIFF_CODEC_GROUP "ENG0: Shouldn't have high priority jobs of different codec group!\n"

#define ENG0_ERRC_HANDLE_CODEC_SWITCH 0xc0012u
#define ENG0_ERRM_HANDLE_CODEC_SWITCH "ENG0: Should handle codec switch before exit!\n"

#define ENG0_ERRC_INVALID_FP_NUM 0xc0013u
#define ENG0_ERRM_INVALID_FP_NUM "ENG0: invalid fp_num=%d\n"

#define ENG0_ERRC_INVALID_CH_NUM 0xc0014u
#define ENG0_ERRM_INVALID_CH_NUM "ENG0: invalid ch_num=%d\n"

#define ENG0_ERRC_INVALID_STR_NUM 0xc0015u
#define ENG0_ERRM_INVALID_STR_NUM "ENG0: invalid str_num=%d > %d\n"

#define ENG0_ERRC_NO_DATA_Q_IN 0xc0016u
#define ENG0_ERRM_NO_DATA_Q_IN "ENG0: eng0 doesn't have data_q_in\n"

#define ENG0_ERRC_INVALID_DHT_DQT_ADDR 0xc0017u
#define ENG0_ERRM_INVALID_DHT_DQT_ADDR "ENG0: Invalid DHT addresses (0x%8x, 0x%8x) or dqt_daddr 0x%8x!\n"

#define ENG0_ERRC_NOT_ENOUGH_SMEM 0xc0018u
#define ENG0_ERRM_NOT_ENOUGH_SMEM "ENG0: not enough smem size\n"

#define ENG0_ERRC_BITS_INFO_SIZE 0xc0019u
#define ENG0_ERRM_BITS_INFO_SIZE "ENG0: User specified bits_info size is too big. The limit is %dx8191=%d\n"

#define ENG0_ERRC_SUFFIX_LEN 0xc001au
#define ENG0_ERRM_SUFFIX_LEN "ENG0: suffix_len = %d should < 32\n"

#define ENG0_ERRC_SE_LEN 0xc001bu
#define ENG0_ERRM_SE_LEN "ENG0: ((se->len-1) < 32) should < 32\n"

#define ENG0_ERRC_SLICE_HEIGHT 0xc001cu
#define ENG0_ERRM_SLICE_HEIGHT "ENG0: slice is small. need height >= 2 but got %d\n"

#define ENG0_ERRC_BITSTREAM_ENCODING 0xc001du
#define ENG0_ERRM_BITSTREAM_ENCODING "ENG0: bitstream encoding error. error_code=%d\n"

#define ENG0_ERRC_ENC_TYPE_NOT_MATCH 0xc001eu
#define ENG0_ERRM_ENC_TYPE_NOT_MATCH "ENG0: coding_type %u not match with cur_enc_type %d!\n"

#define ENG0_ERRC_ILLEGAL_START_END_SLICE_IDX 0xc001fu
#define ENG0_ERRM_ILLEGAL_START_END_SLICE_IDX "ENG0: Illegal eng0 start end slice idx\n"

#define ENG0_ERR_NUM 0x20u

extern const char * const eng0_err_code[ENG0_ERR_NUM];

/*****************************************************************************
 * module_idx 13 ERR_CODE_MOD_PROXY
 *****************************************************************************/
#define PROXY_ERRC_BOOT_0 0xd0000u
#define PROXY_ERRM_BOOT_0 "PROXY: sizeof(proxy_entry_dec_t)=%d > sizeof(proxy_entry_enc_t)=%d\n"

#define PROXY_ERRC_ENC_0 0xd0001u
#define PROXY_ERRM_ENC_0 "PROXY: overflow! cmem_pos_gp=%d > PROXY_CMEM_GP_SIZE(=%d)\n"

#define PROXY_ERRC_ENC_1 0xd0002u
#define PROXY_ERRM_ENC_1 "PROXY: queue full. count=%d wr=%d rd=%d\n"

#define PROXY_ERRC_ENC_2 0xd0003u
#define PROXY_ERRM_ENC_2 "PROXY: bit width definition in proxy_entry_t can't afford such a big number!\n"

#define PROXY_ERRC_ENC_3 0xd0004u
#define PROXY_ERRM_ENC_3 "PROXY: eng0 msg queue full after send. queue_size=%d curr_size=%d num_msgs_in_one_send=%d\n"

#define PROXY_ERRC_ENC_4 0xd0005u
#define PROXY_ERRM_ENC_4 "PROXY: proxy_process_slices: Illegal enc_type %u\n"

#define PROXY_ERRC_ENC_5 0xd0006u
#define PROXY_ERRM_ENC_5 "PROXY: proxy_save_cabac_job: cabac job queue is not empty\n"

#define PROXY_ERRC_ENC_6 0xd0007u
#define PROXY_ERRM_ENC_6 "PROXY: proxy_save_cabac_job: incorrect enc_type=%d\n"

#define PROXY_ERRC_ENC_7 0xd0008u
#define PROXY_ERRM_ENC_7 "PROXY: proxy_load_cabac_job: incorrect enc_type=%d\n"

#define PROXY_ERRC_ENC_8 0xd0009u
#define PROXY_ERRM_ENC_8 "PROXY: proxy_wait_memd_done: unknown enc_type=%d\n"

#define PROXY_ERRC_ENC_9 0xd000au
#define PROXY_ERRM_ENC_9 "PROXY: Slice_num should > 1 when lowdelay mode is MEMD_CABAC_LOWDELAY_SLICEBASED\n"

#define PROXY_ERRC_ENC_10 0xd000bu
#define PROXY_ERRM_ENC_10 "PROXY: Slice_num should be 1 when lowdelay mode is MEMD_CABAC_LOWDELAY_OFF\n"

#define PROXY_ERRC_ENC_11 0xd000cu
#define PROXY_ERRM_ENC_11 "PROXY: No cabac job in queue when handle cabac only job\n"

#define PROXY_ERRC_ENC_12 0xd000du
#define PROXY_ERRM_ENC_12 "PROXY: the memd is running un-expected\n"

#define PROXY_ERRC_ENC_13 0xd000eu
#define PROXY_ERRM_ENC_13 "PROXY: job qid is un-expected %d\n"

#define PROXY_ERR_NUM 0xfu

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

#define DEC_ERR_CODE_BOUNDING_CHECK_ERR      0xe000bu
#define DEC_ERR_MSG_BOUNDING_CHECK_ERR       "DEC: bounding check fail (silicon_version:%d): throughput (K):%d over limit %d (K) + margin:%d\n"

#define DEC_ERR_CODE_PROXY_QUE_SIZE_ERR      0xe000cu
#define DEC_ERR_MSG_PROXY_QUE_SIZE_ERR       "DEC: proxy q1 size %d q0 size %d\n"

#define DEC_ERR_HW_RSC_ERR                   0xe000du
#define DEC_ERR_MSG_HW_RSC_ERR               "DEC: CV7 only support 1 coding engine and 1 mdxf core\n"

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

#define DEC_ERR_CODE_INIT_NEI_CTX            0xe0050u
#define DEC_ERR_MSG_INIT_NEI_CTX             "AVCDEC:: avcdec_init_nei_ctx(): w_in_mbs=%d in window mode\n"

/* jpeg decode */
#define DEC_ERR_CODE_SMEM_INVALID            0xe002eu
#define DEC_ERR_MSG_SMEM_INVALID             "JPGDEC:: SMEM INVALID\n"

/* hevc decode */
#define DEC_ERR_CODE_HEVC_DPB_SHORTAGE       0xe002fu
#define DEC_ERR_MSG_HEVC_DPB_SHORTAGE        "HEVCDEC:: create_hevc_decoder() SMEM reserved for backing up DPB is not enough: size=%d\n"

#define DEC_ERR_CODE_HEVC_SIZE_MM            0xe0030u
#define DEC_ERR_MSG_HEVC_SIZE_MM             "HEVCDEC:: DEFINE_SIZE=%d sizeof(real_structure)=%d\n"

#define DEC_ERR_CODE_HEVC_MAX_CH_LMT         0xe0031u
#define DEC_ERR_MSG_HEVC_MAX_CH_LMT          "HEVC:: create_hevc_decoder() failed: hevcdec_used=%d >= %d\n\n"

#define DEC_ERR_CODE_HEVC_OPM_ERR            0xe0032u
#define DEC_ERR_MSG_HEVC_OPM_ERR             "HEVCDEC:: CMD:: CMD_DECODER_SETUP is rejected, opm=%d!\n"

#define DEC_ERR_CODE_HEVC_CPB_ALN_ERR        0xe0033u
#define DEC_ERR_MSG_HEVC_CPB_ALN_ERR         "HEVCDEC:: CMD:: CMD_DECODER_SETUP: cbp_size=%d _base=0x%08x,_limit=0x%08x, is not multiple of 64\n"

#define DEC_ERR_CODE_HEVC_HCB_ALN_ERR        0xe0034u
#define DEC_ERR_MSG_HEVC_HCB_ALN_ERR         "HEVCDEC:: CMD:: CMD_DECODER_SETUP: pjpeg_buf_size=%d is not multiple of 64\n"

#define DEC_ERR_CODE_HEVC_CMD_ERR            0xe0035u
#define DEC_ERR_MSG_HEVC_CMD_ERR             "HEVCDEC:: CMD:: CMD_DECODER_START:: received at wrong state, opm=%d!\n"

#define DEC_ERR_CODE_HEVC_CMD_ERR2           0xe0036u
#define DEC_ERR_MSG_HEVC_CMD_ERR2            "HEVCDEC:: CMD:: CMD_DECODER_BITSFIFO_UPDATE::received at wrong state op_mode=%d!\n"

#define DEC_ERR_CODE_HEVC_CMD_ERR3           0xe0037u
#define DEC_ERR_MSG_HEVC_CMD_ERR3            "HEVCDEC:: CMD:: CMD_DECODER_TRICKPLAY is rejected, opm=%d\n"

#define DEC_ERR_CODE_HEVC_CMD_ERR4           0xe0038u
#define DEC_ERR_MSG_HEVC_CMD_ERR4            "HEVCDEC:: CMD::Unkown cmd_code=0x%08x\n"

#define DEC_ERR_CODE_HEVC_INT_ERR            0xe0039u
#define DEC_ERR_MSG_HEVC_INT_ERR             "HEVCDEC:: send msg to Coding failed.\n"

#define DEC_ERR_CODE_HEVC_DPX_MOD_ERR        0xe003au
#define DEC_ERR_MSG_HEVC_DPX_MOD_ERR         "HEVCDEC:: hevcdec_sched() dpb_fm_num=%d expect=0\n"

#define DEC_ERR_CODE_HEVC_INT_ERR2           0xe003bu
#define DEC_ERR_MSG_HEVC_INT_ERR2            "HEVCDEC:: hevc_calc_ref_smem_info() ref_smem_par_id cannot be NULL\n"

#define DEC_ERR_CODE_HEVC_QUIT_BY_ERR        0xe003cu
#define DEC_ERR_MSG_HEVC_QUIT_BY_ERR         "HEVCDEC_ERR: Quit on error=0x%08x!\n"

#define DEC_ERR_CODE_HEVC_TILE_ERR           0xe003du
#define DEC_ERR_MSG_HEVC_TILE_ERR            "HEVCDEC:: hevc_dec_slice_data(): tile_info not available\n"

#define DEC_ERR_CODE_HEVC_INIT_ERR           0xe003eu
#define DEC_ERR_MSG_HEVC_INIT_ERR            "HEVCDEC:: hevc_init_cabac_ctx(): failed. SNCols_orccode_comm_smem=%d < %d\n"

#define DEC_ERR_CODE_HEVC_SL_IDX_ERR         0xe003fu
#define DEC_ERR_MSG_HEVC_SL_IDX_ERR          "HEVCDEC:: hevc_get_sh_fr_q() mismatched: hevc.sh_idx(%d) != sh.sh_idx(%d)"

#define DEC_ERR_CODE_HEVC_SL_OV_ERR          0xe0040u
#define DEC_ERR_MSG_HEVC_SL_OV_ERR           "HEVCDEC:: hevc_update_sh_write(): sh_num=%d is over HEVCDEC_SH_TOT_MAX(%d)\n"

#define DEC_ERR_CODE_HEVC_CIN_ALIGN_ERR      0xe0041u
#define DEC_ERR_MSG_HEVC_CIN_ALIGN_ERR       "HEVCDEC:: slice_daddr=0x%x != cbuf rptr=0x%x \n"

#define DEC_ERR_CODE_HEVC_CPB_UF_ERR         0xe0042u
#define DEC_ERR_MSG_HEVC_CPB_UF_ERR          "HEVCDEC:: hevc_dec_slice_data(): [xywu] TBD, error, CPB underflow\n"

#define DEC_ERR_CODE_HEVC_SPS_QUE_ERR        0xe0043u
#define DEC_ERR_MSG_HEVC_SPS_QUE_ERR         "HEVCDEC:: hevc_get_sps_rps_num_pics_fr_q(): not able to load num_negative_pics to temp buffer\n"

#define DEC_ERR_CODE_HEVC_NON_SUPP_ERR       0xe0044u
#define DEC_ERR_MSG_HEVC_NON_SUPP_ERR        "hevcGetList0EntryNum(): [TBD] lt rps is not supported by enc right now\n"

#define DEC_ERR_CODE_HEVC_PIC_CTRL_ERR       0xe0045u
#define DEC_ERR_MSG_HEVC_PIC_CTRL_ERR        "HEVCDEC:: hevc_remove_pic_fr_dpb(): the picture been retired should be used\n"

#define DEC_ERR_CODE_HEVC_DPB_EMP_ERR        0xe0046u
#define DEC_ERR_MSG_HEVC_DPB_EMP_ERR         "HEVCDEC:: hevc_get_smallest_poc_fr_dpb(): DPB is empty\n"

#define DEC_ERR_CODE_HEVC_ILG_USED_SIZE_ERR  0xe0047u
#define DEC_ERR_MSG_HEVC_ILG_USED_SIZE_ERR   "HEVCDEC:: hevc_dump_dpb(): illegal used_size\n"

#define DEC_ERR_CODE_HEVC_STOP_ERR           0xe0048u
#define DEC_ERR_MSG_HEVC_STOP_ERR            "HEVCDEC:: send msg to Coding failed.\n"

#define DEC_ERR_CODE_HEVC_SEG_ERR            0xe0049u
#define DEC_ERR_MSG_HEVC_SEG_ERR             "HEVCDEC:: only support 1 slice per segment for now, segment_h_in_slice=%d\n"

#define DEC_ERR_CODE_AMB_GOP_ERR             0xe004au
#define DEC_ERR_MSG_AMB_GOP_ERR              "DEC:: only support ver_sub 1 and 2 but %d\n"

#define DEC_ERR_CODE_MVEDGE_SIZE_ERR         0xe004bu
#define DEC_ERR_MSG_MVEDGE_SIZE_ERR          "HEVCDEC:: mvedge_sz_in_ctb_m1(0x%x) > scolmax(0x7fff)\n"

#define DEC_ERR_CODE_RES_DYN_ERR             0xe004cu
#define DEC_ERR_MSG_RES_DYN_ERR              "HEVCDEC:: hevc_init_fbp() failed. not yet support runtime resolution change\n"

#define DEC_ERR_CODE_PJPEG_SIZE_ERR          0xe004du
#define DEC_ERR_MSG_PJPEG_SIZE_ERR           "HEVCDEC:: PJPEG size over threshold\n"

#define DEC_ERR_CODE_OVER_HL_STACK_ERR       0xe004eu
#define DEC_ERR_MSG_OVER_HL_STACK_ERR        "HEVCDEC:: over decode hl stack size %d, used size %d\n"

#define DEC_ERR_CODE_SMEM_USAGE_ERR          0xe004fu
#define DEC_ERR_MSG_SMEM_USAGE_ERR           "DEC:: smem usage error, maybe not allocated \n"

#define DEC_ERR_CODE_HEVC_ALIGNBIT_ERR       0xe0050u
#define DEC_ERR_MSG_HEVC_ALIGNBIT_ERR        "HEVCDEC:: hevc_dec_slice_hdr(): alignment_bit_equal_to_one\n"

#define DEC_ERR_CODE_HEVC_SDECBUF_SHORT_ERR  0xe0051u
#define DEC_ERR_MSG_HEVC_SDECBUF_SHORT_ERR   "HEVCDEC:: hevc decI the buffer is shortage\n"

#define DEC_ERR_CODE_NUM 0x52u /* remove this comment if DEC_ERR_NUM has been updated */

extern const char * const dec_err_code[DEC_ERR_CODE_NUM];


#endif /* UCODE_ERROR_H_ */
