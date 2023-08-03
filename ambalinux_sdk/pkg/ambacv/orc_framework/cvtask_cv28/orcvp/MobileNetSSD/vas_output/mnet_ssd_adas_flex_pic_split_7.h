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
#ifndef mnet_ssd_adas_flex_pic_prim_split_7_H
#define mnet_ssd_adas_flex_pic_prim_split_7_H

#define vas_gen_header_version                  	7

/* dag name */
#define mnet_ssd_adas_flex_pic_prim_split_7_DAG_NAME 	"mnet_ssd_adas_flex_pic_prim_split_7"

/* VDG file info */
#define mnet_ssd_adas_flex_pic_prim_split_7_vdg_name 	"mnet_ssd_adas_flex_pic_split_7.vdg"

/* VDG DAG memory info */
#define mnet_ssd_adas_flex_pic_prim_split_7_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_7_byte_size 	1276	/* 0x000004fc */
#define mnet_ssd_adas_flex_pic_prim_split_7_preferred_dram_xfer_size 	1

/* mnet_ssd_adas_flex_pic_prim_split_7 VMEM info */
#define mnet_ssd_adas_flex_pic_prim_split_7_use_ping_pong_vmem 	0
#define mnet_ssd_adas_flex_pic_prim_split_7_VMEM_start 	4096	/* 0x00001000 */
#define mnet_ssd_adas_flex_pic_prim_split_7_VMEM_end 	512352	/* 0x0007d160 */
#define mnet_ssd_adas_flex_pic_prim_split_7_image_start 	512352	/* 0x0007d160 */
#define mnet_ssd_adas_flex_pic_prim_split_7_image_size 	1276	/* 0x000004fc */
#define mnet_ssd_adas_flex_pic_prim_split_7_dagbin_start 	512352	/* 0x0007d160 */

/* mnet_ssd_adas_flex_pic_prim_split_7 DAG info */
#define mnet_ssd_adas_flex_pic_prim_split_7_estimated_cycles 	94897
#define mnet_ssd_adas_flex_pic_prim_split_7_ddi_byte_size 	0	/* 0x00000000 */

/*******************************************************************************
 * DAG byte offset info for HMB/SMB/VCB and their fields
 * in DAG 'mnet_ssd_adas_flex_pic_prim_split_7' in source file 'pre_split7_mnet_ssd_adas_flex_pic_split_0.vas'
 ******************************************************************************/
/* mnet_ssd_adas_flex_pic_prim_split_7 HMB_input __pvcn_877_ */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_877__cnngen_demangled_name 	"__pvcn_877_"
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_877__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_877__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_877__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_877__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_877__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_877__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_877__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_877__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_877__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_877__byte_offset 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_877__vbase_byte_offset 	4	/* 0x00000004 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_877__dbase_byte_offset 	28	/* 0x0000001c */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_877__dpitchm1_byte_offset 	32	/* 0x00000020 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_877__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_877__vwidth_minus_one_byte_offset 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_877__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_877__vheight_minus_one_byte_offset 	18	/* 0x00000012 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_877__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_877__drotate_bit_offset 	198	/* 0x000000c6 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_877__hflip_bit_offset 	199	/* 0x000000c7 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_877__vflip_bit_offset 	200	/* 0x000000c8 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_877__dflip_bit_offset 	201	/* 0x000000c9 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_877__pflip_bit_offset 	202	/* 0x000000ca */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_877__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_877__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_877__D 	22
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_877__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_877__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_877__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_877__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_877__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_877__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_877__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_877__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_877__vmem_buffer_num_bytes 	28160
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_877__dram_size_num_bytes 	66880
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_877__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_877__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_877__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_877__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_877__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_877__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_877__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_877__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_877__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_877__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_7 HMB_input __pvcn_888_ */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_888__cnngen_demangled_name 	"__pvcn_888_"
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_888__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_888__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_888__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_888__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_888__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_888__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_888__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_888__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_888__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_888__byte_offset 	40	/* 0x00000028 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_888__vbase_byte_offset 	44	/* 0x0000002c */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_888__dbase_byte_offset 	68	/* 0x00000044 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_888__dpitchm1_byte_offset 	72	/* 0x00000048 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_888__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_888__vwidth_minus_one_byte_offset 	56	/* 0x00000038 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_888__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_888__vheight_minus_one_byte_offset 	58	/* 0x0000003a */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_888__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_888__drotate_bit_offset 	518	/* 0x00000206 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_888__hflip_bit_offset 	519	/* 0x00000207 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_888__vflip_bit_offset 	520	/* 0x00000208 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_888__dflip_bit_offset 	521	/* 0x00000209 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_888__pflip_bit_offset 	522	/* 0x0000020a */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_888__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_888__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_888__D 	20
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_888__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_888__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_888__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_888__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_888__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_888__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_888__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_888__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_888__vmem_buffer_num_bytes 	25600
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_888__dram_size_num_bytes 	60800
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_888__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_888__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_888__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_888__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_888__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_888__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_888__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_888__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_888__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_888__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_7 HMB_input __pvcn_833_ */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_833__cnngen_demangled_name 	"__pvcn_833_"
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_833__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_833__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_833__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_833__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_833__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_833__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_833__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_833__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_833__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_833__byte_offset 	80	/* 0x00000050 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_833__vbase_byte_offset 	84	/* 0x00000054 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_833__dbase_byte_offset 	108	/* 0x0000006c */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_833__dpitchm1_byte_offset 	112	/* 0x00000070 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_833__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_833__vwidth_minus_one_byte_offset 	96	/* 0x00000060 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_833__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_833__vheight_minus_one_byte_offset 	98	/* 0x00000062 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_833__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_833__drotate_bit_offset 	838	/* 0x00000346 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_833__hflip_bit_offset 	839	/* 0x00000347 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_833__vflip_bit_offset 	840	/* 0x00000348 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_833__dflip_bit_offset 	841	/* 0x00000349 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_833__pflip_bit_offset 	842	/* 0x0000034a */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_833__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_833__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_833__D 	21
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_833__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_833__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_833__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_833__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_833__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_833__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_833__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_833__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_833__vmem_buffer_num_bytes 	26880
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_833__dram_size_num_bytes 	63840
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_833__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_833__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_833__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_833__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_833__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_833__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_833__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_833__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_833__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_833__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_7 HMB_input __pvcn_910_ */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_910__cnngen_demangled_name 	"__pvcn_910_"
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_910__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_910__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_910__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_910__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_910__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_910__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_910__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_910__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_910__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_910__byte_offset 	120	/* 0x00000078 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_910__vbase_byte_offset 	124	/* 0x0000007c */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_910__dbase_byte_offset 	148	/* 0x00000094 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_910__dpitchm1_byte_offset 	152	/* 0x00000098 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_910__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_910__vwidth_minus_one_byte_offset 	136	/* 0x00000088 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_910__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_910__vheight_minus_one_byte_offset 	138	/* 0x0000008a */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_910__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_910__drotate_bit_offset 	1158	/* 0x00000486 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_910__hflip_bit_offset 	1159	/* 0x00000487 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_910__vflip_bit_offset 	1160	/* 0x00000488 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_910__dflip_bit_offset 	1161	/* 0x00000489 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_910__pflip_bit_offset 	1162	/* 0x0000048a */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_910__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_910__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_910__D 	13
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_910__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_910__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_910__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_910__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_910__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_910__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_910__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_910__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_910__vmem_buffer_num_bytes 	33280
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_910__dram_size_num_bytes 	39520
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_910__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_910__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_910__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_910__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_910__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_910__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_910__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_910__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_910__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_910__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_7 HMB_input __pvcn_855_ */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_855__cnngen_demangled_name 	"__pvcn_855_"
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_855__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_855__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_855__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_855__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_855__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_855__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_855__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_855__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_855__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_855__byte_offset 	160	/* 0x000000a0 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_855__vbase_byte_offset 	164	/* 0x000000a4 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_855__dbase_byte_offset 	188	/* 0x000000bc */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_855__dpitchm1_byte_offset 	192	/* 0x000000c0 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_855__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_855__vwidth_minus_one_byte_offset 	176	/* 0x000000b0 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_855__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_855__vheight_minus_one_byte_offset 	178	/* 0x000000b2 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_855__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_855__drotate_bit_offset 	1478	/* 0x000005c6 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_855__hflip_bit_offset 	1479	/* 0x000005c7 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_855__vflip_bit_offset 	1480	/* 0x000005c8 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_855__dflip_bit_offset 	1481	/* 0x000005c9 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_855__pflip_bit_offset 	1482	/* 0x000005ca */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_855__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_855__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_855__D 	22
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_855__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_855__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_855__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_855__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_855__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_855__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_855__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_855__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_855__vmem_buffer_num_bytes 	28160
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_855__dram_size_num_bytes 	66880
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_855__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_855__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_855__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_855__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_855__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_855__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_855__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_855__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_855__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_855__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_7 HMB_input __pvcn_899_ */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_899__cnngen_demangled_name 	"__pvcn_899_"
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_899__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_899__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_899__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_899__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_899__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_899__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_899__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_899__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_899__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_899__byte_offset 	200	/* 0x000000c8 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_899__vbase_byte_offset 	204	/* 0x000000cc */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_899__dbase_byte_offset 	228	/* 0x000000e4 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_899__dpitchm1_byte_offset 	232	/* 0x000000e8 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_899__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_899__vwidth_minus_one_byte_offset 	216	/* 0x000000d8 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_899__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_899__vheight_minus_one_byte_offset 	218	/* 0x000000da */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_899__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_899__drotate_bit_offset 	1798	/* 0x00000706 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_899__hflip_bit_offset 	1799	/* 0x00000707 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_899__vflip_bit_offset 	1800	/* 0x00000708 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_899__dflip_bit_offset 	1801	/* 0x00000709 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_899__pflip_bit_offset 	1802	/* 0x0000070a */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_899__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_899__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_899__D 	20
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_899__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_899__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_899__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_899__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_899__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_899__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_899__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_899__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_899__vmem_buffer_num_bytes 	25600
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_899__dram_size_num_bytes 	60800
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_899__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_899__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_899__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_899__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_899__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_899__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_899__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_899__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_899__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_899__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_7 HMB_input __pvcn_866_ */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_866__cnngen_demangled_name 	"__pvcn_866_"
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_866__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_866__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_866__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_866__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_866__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_866__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_866__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_866__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_866__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_866__byte_offset 	240	/* 0x000000f0 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_866__vbase_byte_offset 	244	/* 0x000000f4 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_866__dbase_byte_offset 	268	/* 0x0000010c */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_866__dpitchm1_byte_offset 	272	/* 0x00000110 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_866__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_866__vwidth_minus_one_byte_offset 	256	/* 0x00000100 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_866__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_866__vheight_minus_one_byte_offset 	258	/* 0x00000102 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_866__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_866__drotate_bit_offset 	2118	/* 0x00000846 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_866__hflip_bit_offset 	2119	/* 0x00000847 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_866__vflip_bit_offset 	2120	/* 0x00000848 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_866__dflip_bit_offset 	2121	/* 0x00000849 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_866__pflip_bit_offset 	2122	/* 0x0000084a */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_866__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_866__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_866__D 	24
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_866__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_866__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_866__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_866__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_866__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_866__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_866__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_866__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_866__vmem_buffer_num_bytes 	30720
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_866__dram_size_num_bytes 	72960
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_866__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_866__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_866__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_866__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_866__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_866__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_866__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_866__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_866__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_866__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_7 HMB_input __pvcn_844_ */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_844__cnngen_demangled_name 	"__pvcn_844_"
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_844__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_844__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_844__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_844__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_844__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_844__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_844__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_844__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_844__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_844__byte_offset 	280	/* 0x00000118 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_844__vbase_byte_offset 	284	/* 0x0000011c */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_844__dbase_byte_offset 	308	/* 0x00000134 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_844__dpitchm1_byte_offset 	312	/* 0x00000138 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_844__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_844__vwidth_minus_one_byte_offset 	296	/* 0x00000128 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_844__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_844__vheight_minus_one_byte_offset 	298	/* 0x0000012a */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_844__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_844__drotate_bit_offset 	2438	/* 0x00000986 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_844__hflip_bit_offset 	2439	/* 0x00000987 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_844__vflip_bit_offset 	2440	/* 0x00000988 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_844__dflip_bit_offset 	2441	/* 0x00000989 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_844__pflip_bit_offset 	2442	/* 0x0000098a */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_844__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_844__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_844__D 	21
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_844__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_844__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_844__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_844__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_844__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_844__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_844__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_844__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_844__vmem_buffer_num_bytes 	26880
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_844__dram_size_num_bytes 	63840
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_844__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_844__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_844__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_844__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_844__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_844__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_844__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_844__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_844__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_844__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_7 HMB_input __pvcn_822_ */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_822__cnngen_demangled_name 	"__pvcn_822_"
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_822__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_822__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_822__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_822__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_822__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_822__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_822__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_822__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_822__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_822__byte_offset 	320	/* 0x00000140 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_822__vbase_byte_offset 	324	/* 0x00000144 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_822__dbase_byte_offset 	348	/* 0x0000015c */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_822__dpitchm1_byte_offset 	352	/* 0x00000160 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_822__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_822__vwidth_minus_one_byte_offset 	336	/* 0x00000150 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_822__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_822__vheight_minus_one_byte_offset 	338	/* 0x00000152 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_822__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_822__drotate_bit_offset 	2758	/* 0x00000ac6 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_822__hflip_bit_offset 	2759	/* 0x00000ac7 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_822__vflip_bit_offset 	2760	/* 0x00000ac8 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_822__dflip_bit_offset 	2761	/* 0x00000ac9 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_822__pflip_bit_offset 	2762	/* 0x00000aca */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_822__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_822__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_822__D 	23
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_822__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_822__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_822__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_822__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_822__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_822__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_822__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_822__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_822__vmem_buffer_num_bytes 	29440
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_822__dram_size_num_bytes 	69920
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_822__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_822__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_822__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_822__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_822__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_822__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_822__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_822__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_822__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_822__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_7 HMB_input __pvcn_811_ */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_811__cnngen_demangled_name 	"__pvcn_811_"
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_811__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_811__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_811__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_811__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_811__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_811__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_811__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_811__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_811__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_811__byte_offset 	360	/* 0x00000168 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_811__vbase_byte_offset 	364	/* 0x0000016c */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_811__dbase_byte_offset 	388	/* 0x00000184 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_811__dpitchm1_byte_offset 	392	/* 0x00000188 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_811__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_811__vwidth_minus_one_byte_offset 	376	/* 0x00000178 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_811__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_811__vheight_minus_one_byte_offset 	378	/* 0x0000017a */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_811__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_811__drotate_bit_offset 	3078	/* 0x00000c06 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_811__hflip_bit_offset 	3079	/* 0x00000c07 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_811__vflip_bit_offset 	3080	/* 0x00000c08 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_811__dflip_bit_offset 	3081	/* 0x00000c09 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_811__pflip_bit_offset 	3082	/* 0x00000c0a */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_811__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_811__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_811__D 	24
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_811__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_811__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_811__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_811__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_811__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_811__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_811__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_811__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_811__vmem_buffer_num_bytes 	30720
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_811__dram_size_num_bytes 	72960
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_811__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_811__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_811__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_811__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_811__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_811__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_811__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_811__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_811__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_811__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_7 HMB_input __pvcn_800_ */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_800__cnngen_demangled_name 	"__pvcn_800_"
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_800__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_800__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_800__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_800__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_800__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_800__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_800__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_800__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_800__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_800__byte_offset 	400	/* 0x00000190 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_800__vbase_byte_offset 	404	/* 0x00000194 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_800__dbase_byte_offset 	428	/* 0x000001ac */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_800__dpitchm1_byte_offset 	432	/* 0x000001b0 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_800__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_800__vwidth_minus_one_byte_offset 	416	/* 0x000001a0 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_800__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_800__vheight_minus_one_byte_offset 	418	/* 0x000001a2 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_800__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_800__drotate_bit_offset 	3398	/* 0x00000d46 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_800__hflip_bit_offset 	3399	/* 0x00000d47 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_800__vflip_bit_offset 	3400	/* 0x00000d48 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_800__dflip_bit_offset 	3401	/* 0x00000d49 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_800__pflip_bit_offset 	3402	/* 0x00000d4a */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_800__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_800__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_800__D 	24
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_800__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_800__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_800__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_800__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_800__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_800__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_800__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_800__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_800__vmem_buffer_num_bytes 	30720
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_800__dram_size_num_bytes 	72960
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_800__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_800__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_800__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_800__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_800__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_800__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_800__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_800__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_800__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_800__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_7 HMB_input __pvcn_789_ */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_789__cnngen_demangled_name 	"__pvcn_789_"
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_789__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_789__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_789__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_789__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_789__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_789__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_789__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_789__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_789__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_789__byte_offset 	440	/* 0x000001b8 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_789__vbase_byte_offset 	444	/* 0x000001bc */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_789__dbase_byte_offset 	468	/* 0x000001d4 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_789__dpitchm1_byte_offset 	472	/* 0x000001d8 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_789__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_789__vwidth_minus_one_byte_offset 	456	/* 0x000001c8 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_789__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_789__vheight_minus_one_byte_offset 	458	/* 0x000001ca */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_789__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_789__drotate_bit_offset 	3718	/* 0x00000e86 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_789__hflip_bit_offset 	3719	/* 0x00000e87 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_789__vflip_bit_offset 	3720	/* 0x00000e88 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_789__dflip_bit_offset 	3721	/* 0x00000e89 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_789__pflip_bit_offset 	3722	/* 0x00000e8a */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_789__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_789__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_789__D 	22
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_789__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_789__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_789__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_789__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_789__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_789__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_789__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_789__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_789__vmem_buffer_num_bytes 	28160
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_789__dram_size_num_bytes 	66880
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_789__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_789__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_789__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_789__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_789__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_789__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_789__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_789__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_789__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_789__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_7 HMB_output __pvcn_925_ */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_925__cnngen_demangled_name 	"__pvcn_925_"
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_925__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_925__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_925__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_925__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_925__byte_offset 	920	/* 0x00000398 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_925__vbase_byte_offset 	924	/* 0x0000039c */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_925__dbase_byte_offset 	948	/* 0x000003b4 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_925__dpitchm1_byte_offset 	952	/* 0x000003b8 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_925__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_925__vwidth_minus_one_byte_offset 	936	/* 0x000003a8 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_925__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_925__vheight_minus_one_byte_offset 	938	/* 0x000003aa */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_925__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_925__drotate_bit_offset 	7558	/* 0x00001d86 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_925__hflip_bit_offset 	7559	/* 0x00001d87 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_925__vflip_bit_offset 	7560	/* 0x00001d88 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_925__dflip_bit_offset 	7561	/* 0x00001d89 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_925__pflip_bit_offset 	7562	/* 0x00001d8a */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_925__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_925__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_925__D 	46
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_925__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_925__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_925__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_925__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_925__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_925__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_925__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_925__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_925__vmem_buffer_num_bytes 	29440
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_925__dram_size_num_bytes 	139840
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_925__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_925__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_925__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_925__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_925__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_925__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_925__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_925__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_925__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_925__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_7 HMB_output __pvcn_926_ */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_926__cnngen_demangled_name 	"__pvcn_926_"
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_926__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_926__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_926__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_926__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_926__byte_offset 	968	/* 0x000003c8 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_926__vbase_byte_offset 	972	/* 0x000003cc */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_926__dbase_byte_offset 	996	/* 0x000003e4 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_926__dpitchm1_byte_offset 	1000	/* 0x000003e8 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_926__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_926__vwidth_minus_one_byte_offset 	984	/* 0x000003d8 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_926__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_926__vheight_minus_one_byte_offset 	986	/* 0x000003da */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_926__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_926__drotate_bit_offset 	7942	/* 0x00001f06 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_926__hflip_bit_offset 	7943	/* 0x00001f07 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_926__vflip_bit_offset 	7944	/* 0x00001f08 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_926__dflip_bit_offset 	7945	/* 0x00001f09 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_926__pflip_bit_offset 	7946	/* 0x00001f0a */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_926__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_926__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_926__D 	47
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_926__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_926__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_926__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_926__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_926__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_926__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_926__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_926__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_926__vmem_buffer_num_bytes 	30080
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_926__dram_size_num_bytes 	142880
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_926__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_926__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_926__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_926__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_926__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_926__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_926__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_926__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_926__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_926__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_7 HMB_output __pvcn_927_ */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_927__cnngen_demangled_name 	"__pvcn_927_"
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_927__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_927__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_927__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_927__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_927__byte_offset 	1016	/* 0x000003f8 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_927__vbase_byte_offset 	1020	/* 0x000003fc */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_927__dbase_byte_offset 	1044	/* 0x00000414 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_927__dpitchm1_byte_offset 	1048	/* 0x00000418 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_927__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_927__vwidth_minus_one_byte_offset 	1032	/* 0x00000408 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_927__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_927__vheight_minus_one_byte_offset 	1034	/* 0x0000040a */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_927__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_927__drotate_bit_offset 	8326	/* 0x00002086 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_927__hflip_bit_offset 	8327	/* 0x00002087 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_927__vflip_bit_offset 	8328	/* 0x00002088 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_927__dflip_bit_offset 	8329	/* 0x00002089 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_927__pflip_bit_offset 	8330	/* 0x0000208a */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_927__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_927__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_927__D 	42
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_927__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_927__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_927__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_927__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_927__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_927__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_927__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_927__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_927__vmem_buffer_num_bytes 	26880
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_927__dram_size_num_bytes 	127680
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_927__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_927__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_927__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_927__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_927__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_927__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_927__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_927__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_927__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_927__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_7 HMB_output __pvcn_928_ */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_928__cnngen_demangled_name 	"__pvcn_928_"
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_928__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_928__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_928__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_928__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_928__byte_offset 	1064	/* 0x00000428 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_928__vbase_byte_offset 	1068	/* 0x0000042c */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_928__dbase_byte_offset 	1092	/* 0x00000444 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_928__dpitchm1_byte_offset 	1096	/* 0x00000448 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_928__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_928__vwidth_minus_one_byte_offset 	1080	/* 0x00000438 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_928__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_928__vheight_minus_one_byte_offset 	1082	/* 0x0000043a */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_928__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_928__drotate_bit_offset 	8710	/* 0x00002206 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_928__hflip_bit_offset 	8711	/* 0x00002207 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_928__vflip_bit_offset 	8712	/* 0x00002208 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_928__dflip_bit_offset 	8713	/* 0x00002209 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_928__pflip_bit_offset 	8714	/* 0x0000220a */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_928__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_928__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_928__D 	46
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_928__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_928__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_928__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_928__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_928__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_928__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_928__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_928__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_928__vmem_buffer_num_bytes 	29440
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_928__dram_size_num_bytes 	139840
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_928__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_928__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_928__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_928__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_928__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_928__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_928__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_928__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_928__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_928__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_7 HMB_output __pvcn_929_ */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_929__cnngen_demangled_name 	"__pvcn_929_"
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_929__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_929__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_929__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_929__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_929__byte_offset 	1112	/* 0x00000458 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_929__vbase_byte_offset 	1116	/* 0x0000045c */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_929__dbase_byte_offset 	1140	/* 0x00000474 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_929__dpitchm1_byte_offset 	1144	/* 0x00000478 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_929__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_929__vwidth_minus_one_byte_offset 	1128	/* 0x00000468 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_929__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_929__vheight_minus_one_byte_offset 	1130	/* 0x0000046a */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_929__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_929__drotate_bit_offset 	9094	/* 0x00002386 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_929__hflip_bit_offset 	9095	/* 0x00002387 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_929__vflip_bit_offset 	9096	/* 0x00002388 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_929__dflip_bit_offset 	9097	/* 0x00002389 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_929__pflip_bit_offset 	9098	/* 0x0000238a */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_929__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_929__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_929__D 	42
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_929__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_929__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_929__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_929__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_929__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_929__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_929__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_929__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_929__vmem_buffer_num_bytes 	26880
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_929__dram_size_num_bytes 	127680
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_929__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_929__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_929__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_929__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_929__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_929__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_929__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_929__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_929__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_929__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_7 HMB_output __pvcn_930_ */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_930__cnngen_demangled_name 	"__pvcn_930_"
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_930__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_930__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_930__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_930__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_930__byte_offset 	1160	/* 0x00000488 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_930__vbase_byte_offset 	1164	/* 0x0000048c */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_930__dbase_byte_offset 	1188	/* 0x000004a4 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_930__dpitchm1_byte_offset 	1192	/* 0x000004a8 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_930__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_930__vwidth_minus_one_byte_offset 	1176	/* 0x00000498 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_930__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_930__vheight_minus_one_byte_offset 	1178	/* 0x0000049a */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_930__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_930__drotate_bit_offset 	9478	/* 0x00002506 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_930__hflip_bit_offset 	9479	/* 0x00002507 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_930__vflip_bit_offset 	9480	/* 0x00002508 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_930__dflip_bit_offset 	9481	/* 0x00002509 */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_930__pflip_bit_offset 	9482	/* 0x0000250a */
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_930__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_930__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_930__D 	33
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_930__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_930__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_930__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_930__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_930__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_930__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_930__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_930__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_930__vmem_buffer_num_bytes 	21120
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_930__dram_size_num_bytes 	100320
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_930__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_930__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_930__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_930__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_930__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_930__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_930__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_930__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_930__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___pvcn_930__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_7 SMB_output __vas_created_terminate__v0 */
#define mnet_ssd_adas_flex_pic_prim_split_7___vas_created_terminate__v0_cnngen_demangled_name 	"__vas_created_terminate__v0"
#define mnet_ssd_adas_flex_pic_prim_split_7___vas_created_terminate__v0_is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___vas_created_terminate__v0_is_bitvector 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___vas_created_terminate__v0_is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___vas_created_terminate__v0_is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___vas_created_terminate__v0_byte_offset 	1200	/* 0x000004b0 */
#define mnet_ssd_adas_flex_pic_prim_split_7___vas_created_terminate__v0_vbase_byte_offset 	1204	/* 0x000004b4 */
#define mnet_ssd_adas_flex_pic_prim_split_7___vas_created_terminate__v0_W 	256
#define mnet_ssd_adas_flex_pic_prim_split_7___vas_created_terminate__v0_H 	3
#define mnet_ssd_adas_flex_pic_prim_split_7___vas_created_terminate__v0_D 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___vas_created_terminate__v0_P 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___vas_created_terminate__v0_data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___vas_created_terminate__v0_denable 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___vas_created_terminate__v0_dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_7___vas_created_terminate__v0_vp_interleave_mode 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___vas_created_terminate__v0_tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___vas_created_terminate__v0_tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_7___vas_created_terminate__v0_vmem_buffer_num_bytes 	96


#endif /* mnet_ssd_adas_flex_pic_prim_split_7_H */
