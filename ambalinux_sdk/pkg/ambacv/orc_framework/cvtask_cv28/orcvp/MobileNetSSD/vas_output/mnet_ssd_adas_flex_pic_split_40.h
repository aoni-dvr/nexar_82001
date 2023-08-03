////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Ambarella International LP
////////////////////////////////////////////////////////////////////////////////
// This file and its contents ("Software") are protected by intellectual
// property rights including, without limitation, U.S. and/or foreign
// copyrights. This Software is also the confidential and proprietary
// information of Ambarella International LP and its licensors. You may not use,
// reproduce, disclose, distribute, modify, or otherwise prepare derivative
// works of this Software or any portion thereof except pursuant to a signed
// license agreement or nondisclosure agreement with Ambarella International LP
// or its authorized affiliates. In the absence of such an agreement, you agree
// to promptly notify and return this Software to Ambarella International LP.
//
// THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
// INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
// MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
////////////////////////////////////////////////////////////////////////////////
#ifndef mnet_ssd_adas_flex_pic_prim_split_40_H
#define mnet_ssd_adas_flex_pic_prim_split_40_H

#define vas_gen_header_version                  	7

/* dag name */
#define mnet_ssd_adas_flex_pic_prim_split_40_DAG_NAME 	"mnet_ssd_adas_flex_pic_prim_split_40"

/* VDG file info */
#define mnet_ssd_adas_flex_pic_prim_split_40_vdg_name 	"mnet_ssd_adas_flex_pic_split_40.vdg"

/* VDG DAG memory info */
#define mnet_ssd_adas_flex_pic_prim_split_40_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_40_byte_size 	168	/* 0x000000a8 */
#define mnet_ssd_adas_flex_pic_prim_split_40_preferred_dram_xfer_size 	1

/* mnet_ssd_adas_flex_pic_prim_split_40 VMEM info */
#define mnet_ssd_adas_flex_pic_prim_split_40_use_ping_pong_vmem 	0
#define mnet_ssd_adas_flex_pic_prim_split_40_VMEM_start 	4096	/* 0x00001000 */
#define mnet_ssd_adas_flex_pic_prim_split_40_VMEM_end 	53088	/* 0x0000cf60 */
#define mnet_ssd_adas_flex_pic_prim_split_40_image_start 	53088	/* 0x0000cf60 */
#define mnet_ssd_adas_flex_pic_prim_split_40_image_size 	168	/* 0x000000a8 */
#define mnet_ssd_adas_flex_pic_prim_split_40_dagbin_start 	53088	/* 0x0000cf60 */

/* mnet_ssd_adas_flex_pic_prim_split_40 DAG info */
#define mnet_ssd_adas_flex_pic_prim_split_40_estimated_cycles 	3403
#define mnet_ssd_adas_flex_pic_prim_split_40_ddi_byte_size 	0	/* 0x00000000 */

/*******************************************************************************
 * DAG byte offset info for HMB/SMB/VCB and their fields
 * in DAG 'mnet_ssd_adas_flex_pic_prim_split_40' in source file 'pre_split34_mnet_ssd_adas_flex_pic_split_0.vas'
 ******************************************************************************/
/* mnet_ssd_adas_flex_pic_prim_split_40 HMB_input mbox_conf_flatten_ */
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten__cnngen_demangled_name 	"mbox_conf_flatten_"
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten__byte_offset 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten__vbase_byte_offset 	4	/* 0x00000004 */
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten__dbase_byte_offset 	24	/* 0x00000018 */
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten__dpitchm1_byte_offset 	28	/* 0x0000001c */
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten__vwidth_minus_one_byte_offset 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten__drotate_bit_offset 	166	/* 0x000000a6 */
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten__hflip_bit_offset 	167	/* 0x000000a7 */
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten__vflip_bit_offset 	168	/* 0x000000a8 */
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten__dflip_bit_offset 	169	/* 0x000000a9 */
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten__pflip_bit_offset 	170	/* 0x000000aa */
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten__W 	13419
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten__vmem_buffer_num_bytes 	16128
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten__dram_size_num_bytes 	26848
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten__expoffset 	-8
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten__expbits 	4
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_40 HMB_output mbox_conf_flatten */
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten_cnngen_demangled_name 	"mbox_conf_flatten"
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten_is_main_input_output 	1
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten_is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten_is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten_is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten_byte_offset 	56	/* 0x00000038 */
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten_vbase_byte_offset 	60	/* 0x0000003c */
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten_dbase_byte_offset 	80	/* 0x00000050 */
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten_dpitchm1_byte_offset 	84	/* 0x00000054 */
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten_dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten_vwidth_minus_one_byte_offset 	72	/* 0x00000048 */
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten_vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten_drotate_bit_offset 	614	/* 0x00000266 */
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten_hflip_bit_offset 	615	/* 0x00000267 */
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten_vflip_bit_offset 	616	/* 0x00000268 */
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten_dflip_bit_offset 	617	/* 0x00000269 */
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten_pflip_bit_offset 	618	/* 0x0000026a */
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten_W 	13419
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten_H 	1
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten_D 	1
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten_P 	1
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten_data_num_bytes 	4
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten_denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten_dpitch_num_bytes 	53696
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten_dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten_vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten_tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten_tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten_vmem_buffer_num_bytes 	32768
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten_dram_size_num_bytes 	53696
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten_sign 	1
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten_datasize 	2
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten_expoffset 	0
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten_expbits 	7
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten_drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten_hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten_vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten_dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten_pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_40_mbox_conf_flatten_trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_40 SMB_output __vas_created_terminate__v0 */
#define mnet_ssd_adas_flex_pic_prim_split_40___vas_created_terminate__v0_cnngen_demangled_name 	"__vas_created_terminate__v0"
#define mnet_ssd_adas_flex_pic_prim_split_40___vas_created_terminate__v0_is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_40___vas_created_terminate__v0_is_bitvector 	1
#define mnet_ssd_adas_flex_pic_prim_split_40___vas_created_terminate__v0_is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_40___vas_created_terminate__v0_is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_40___vas_created_terminate__v0_byte_offset 	92	/* 0x0000005c */
#define mnet_ssd_adas_flex_pic_prim_split_40___vas_created_terminate__v0_vbase_byte_offset 	96	/* 0x00000060 */
#define mnet_ssd_adas_flex_pic_prim_split_40___vas_created_terminate__v0_W 	256
#define mnet_ssd_adas_flex_pic_prim_split_40___vas_created_terminate__v0_H 	3
#define mnet_ssd_adas_flex_pic_prim_split_40___vas_created_terminate__v0_D 	1
#define mnet_ssd_adas_flex_pic_prim_split_40___vas_created_terminate__v0_P 	1
#define mnet_ssd_adas_flex_pic_prim_split_40___vas_created_terminate__v0_data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_40___vas_created_terminate__v0_denable 	0
#define mnet_ssd_adas_flex_pic_prim_split_40___vas_created_terminate__v0_dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_40___vas_created_terminate__v0_vp_interleave_mode 	1
#define mnet_ssd_adas_flex_pic_prim_split_40___vas_created_terminate__v0_tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_40___vas_created_terminate__v0_tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_40___vas_created_terminate__v0_vmem_buffer_num_bytes 	96


#endif /* mnet_ssd_adas_flex_pic_prim_split_40_H */
