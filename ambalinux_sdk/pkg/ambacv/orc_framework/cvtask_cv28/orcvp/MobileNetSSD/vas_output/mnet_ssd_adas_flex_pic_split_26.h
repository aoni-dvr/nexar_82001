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
#ifndef mnet_ssd_adas_flex_pic_prim_split_26_H
#define mnet_ssd_adas_flex_pic_prim_split_26_H

#define vas_gen_header_version                  	7

/* dag name */
#define mnet_ssd_adas_flex_pic_prim_split_26_DAG_NAME 	"mnet_ssd_adas_flex_pic_prim_split_26"

/* VDG file info */
#define mnet_ssd_adas_flex_pic_prim_split_26_vdg_name 	"mnet_ssd_adas_flex_pic_split_26.vdg"

/* VDG DAG memory info */
#define mnet_ssd_adas_flex_pic_prim_split_26_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_26_byte_size 	1396	/* 0x00000574 */
#define mnet_ssd_adas_flex_pic_prim_split_26_preferred_dram_xfer_size 	1

/* mnet_ssd_adas_flex_pic_prim_split_26 VMEM info */
#define mnet_ssd_adas_flex_pic_prim_split_26_use_ping_pong_vmem 	0
#define mnet_ssd_adas_flex_pic_prim_split_26_VMEM_start 	4096	/* 0x00001000 */
#define mnet_ssd_adas_flex_pic_prim_split_26_VMEM_end 	414272	/* 0x00065240 */
#define mnet_ssd_adas_flex_pic_prim_split_26_image_start 	414272	/* 0x00065240 */
#define mnet_ssd_adas_flex_pic_prim_split_26_image_size 	1396	/* 0x00000574 */
#define mnet_ssd_adas_flex_pic_prim_split_26_dagbin_start 	414272	/* 0x00065240 */

/* mnet_ssd_adas_flex_pic_prim_split_26 DAG info */
#define mnet_ssd_adas_flex_pic_prim_split_26_estimated_cycles 	15408
#define mnet_ssd_adas_flex_pic_prim_split_26_ddi_byte_size 	0	/* 0x00000000 */

/*******************************************************************************
 * DAG byte offset info for HMB/SMB/VCB and their fields
 * in DAG 'mnet_ssd_adas_flex_pic_prim_split_26' in source file 'pre_split25_mnet_ssd_adas_flex_pic_split_0.vas'
 ******************************************************************************/
/* mnet_ssd_adas_flex_pic_prim_split_26 HMB_input __pvcn_2420_ */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2420__cnngen_demangled_name 	"__pvcn_2420_"
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2420__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2420__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2420__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2420__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2420__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2420__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2420__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2420__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2420__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2420__byte_offset 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2420__vbase_byte_offset 	4	/* 0x00000004 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2420__dbase_byte_offset 	28	/* 0x0000001c */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2420__dpitchm1_byte_offset 	32	/* 0x00000020 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2420__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2420__vwidth_minus_one_byte_offset 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2420__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2420__vheight_minus_one_byte_offset 	18	/* 0x00000012 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2420__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2420__drotate_bit_offset 	198	/* 0x000000c6 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2420__hflip_bit_offset 	199	/* 0x000000c7 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2420__vflip_bit_offset 	200	/* 0x000000c8 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2420__dflip_bit_offset 	201	/* 0x000000c9 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2420__pflip_bit_offset 	202	/* 0x000000ca */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2420__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2420__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2420__D 	72
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2420__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2420__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2420__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2420__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2420__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2420__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2420__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2420__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2420__vmem_buffer_num_bytes 	14400
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2420__dram_size_num_bytes 	17280
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2420__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2420__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2420__expoffset 	12
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2420__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2420__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2420__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2420__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2420__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2420__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2420__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_26 HMB_input __pvcn_2442_ */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2442__cnngen_demangled_name 	"__pvcn_2442_"
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2442__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2442__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2442__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2442__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2442__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2442__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2442__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2442__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2442__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2442__byte_offset 	40	/* 0x00000028 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2442__vbase_byte_offset 	44	/* 0x0000002c */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2442__dbase_byte_offset 	68	/* 0x00000044 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2442__dpitchm1_byte_offset 	72	/* 0x00000048 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2442__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2442__vwidth_minus_one_byte_offset 	56	/* 0x00000038 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2442__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2442__vheight_minus_one_byte_offset 	58	/* 0x0000003a */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2442__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2442__drotate_bit_offset 	518	/* 0x00000206 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2442__hflip_bit_offset 	519	/* 0x00000207 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2442__vflip_bit_offset 	520	/* 0x00000208 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2442__dflip_bit_offset 	521	/* 0x00000209 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2442__pflip_bit_offset 	522	/* 0x0000020a */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2442__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2442__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2442__D 	70
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2442__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2442__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2442__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2442__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2442__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2442__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2442__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2442__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2442__vmem_buffer_num_bytes 	14000
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2442__dram_size_num_bytes 	16800
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2442__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2442__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2442__expoffset 	12
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2442__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2442__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2442__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2442__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2442__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2442__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2442__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_26 HMB_input __pvcn_2453_ */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2453__cnngen_demangled_name 	"__pvcn_2453_"
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2453__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2453__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2453__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2453__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2453__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2453__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2453__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2453__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2453__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2453__byte_offset 	80	/* 0x00000050 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2453__vbase_byte_offset 	84	/* 0x00000054 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2453__dbase_byte_offset 	108	/* 0x0000006c */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2453__dpitchm1_byte_offset 	112	/* 0x00000070 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2453__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2453__vwidth_minus_one_byte_offset 	96	/* 0x00000060 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2453__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2453__vheight_minus_one_byte_offset 	98	/* 0x00000062 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2453__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2453__drotate_bit_offset 	838	/* 0x00000346 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2453__hflip_bit_offset 	839	/* 0x00000347 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2453__vflip_bit_offset 	840	/* 0x00000348 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2453__dflip_bit_offset 	841	/* 0x00000349 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2453__pflip_bit_offset 	842	/* 0x0000034a */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2453__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2453__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2453__D 	74
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2453__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2453__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2453__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2453__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2453__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2453__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2453__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2453__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2453__vmem_buffer_num_bytes 	14800
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2453__dram_size_num_bytes 	17760
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2453__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2453__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2453__expoffset 	12
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2453__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2453__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2453__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2453__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2453__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2453__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2453__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_26 HMB_input __pvcn_2519_ */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2519__cnngen_demangled_name 	"__pvcn_2519_"
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2519__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2519__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2519__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2519__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2519__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2519__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2519__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2519__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2519__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2519__byte_offset 	120	/* 0x00000078 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2519__vbase_byte_offset 	124	/* 0x0000007c */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2519__dbase_byte_offset 	148	/* 0x00000094 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2519__dpitchm1_byte_offset 	152	/* 0x00000098 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2519__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2519__vwidth_minus_one_byte_offset 	136	/* 0x00000088 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2519__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2519__vheight_minus_one_byte_offset 	138	/* 0x0000008a */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2519__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2519__drotate_bit_offset 	1158	/* 0x00000486 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2519__hflip_bit_offset 	1159	/* 0x00000487 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2519__vflip_bit_offset 	1160	/* 0x00000488 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2519__dflip_bit_offset 	1161	/* 0x00000489 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2519__pflip_bit_offset 	1162	/* 0x0000048a */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2519__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2519__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2519__D 	6
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2519__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2519__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2519__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2519__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2519__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2519__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2519__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2519__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2519__vmem_buffer_num_bytes 	1440
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2519__dram_size_num_bytes 	1440
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2519__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2519__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2519__expoffset 	12
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2519__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2519__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2519__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2519__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2519__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2519__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2519__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_26 HMB_input __pvcn_2497_ */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2497__cnngen_demangled_name 	"__pvcn_2497_"
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2497__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2497__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2497__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2497__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2497__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2497__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2497__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2497__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2497__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2497__byte_offset 	160	/* 0x000000a0 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2497__vbase_byte_offset 	164	/* 0x000000a4 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2497__dbase_byte_offset 	188	/* 0x000000bc */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2497__dpitchm1_byte_offset 	192	/* 0x000000c0 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2497__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2497__vwidth_minus_one_byte_offset 	176	/* 0x000000b0 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2497__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2497__vheight_minus_one_byte_offset 	178	/* 0x000000b2 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2497__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2497__drotate_bit_offset 	1478	/* 0x000005c6 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2497__hflip_bit_offset 	1479	/* 0x000005c7 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2497__vflip_bit_offset 	1480	/* 0x000005c8 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2497__dflip_bit_offset 	1481	/* 0x000005c9 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2497__pflip_bit_offset 	1482	/* 0x000005ca */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2497__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2497__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2497__D 	73
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2497__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2497__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2497__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2497__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2497__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2497__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2497__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2497__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2497__vmem_buffer_num_bytes 	14600
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2497__dram_size_num_bytes 	17536
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2497__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2497__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2497__expoffset 	12
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2497__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2497__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2497__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2497__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2497__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2497__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2497__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_26 HMB_input __pvcn_2475_ */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2475__cnngen_demangled_name 	"__pvcn_2475_"
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2475__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2475__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2475__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2475__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2475__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2475__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2475__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2475__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2475__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2475__byte_offset 	200	/* 0x000000c8 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2475__vbase_byte_offset 	204	/* 0x000000cc */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2475__dbase_byte_offset 	228	/* 0x000000e4 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2475__dpitchm1_byte_offset 	232	/* 0x000000e8 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2475__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2475__vwidth_minus_one_byte_offset 	216	/* 0x000000d8 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2475__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2475__vheight_minus_one_byte_offset 	218	/* 0x000000da */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2475__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2475__drotate_bit_offset 	1798	/* 0x00000706 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2475__hflip_bit_offset 	1799	/* 0x00000707 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2475__vflip_bit_offset 	1800	/* 0x00000708 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2475__dflip_bit_offset 	1801	/* 0x00000709 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2475__pflip_bit_offset 	1802	/* 0x0000070a */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2475__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2475__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2475__D 	76
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2475__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2475__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2475__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2475__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2475__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2475__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2475__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2475__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2475__vmem_buffer_num_bytes 	15200
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2475__dram_size_num_bytes 	18240
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2475__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2475__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2475__expoffset 	12
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2475__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2475__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2475__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2475__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2475__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2475__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2475__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_26 HMB_input __pvcn_2508_ */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2508__cnngen_demangled_name 	"__pvcn_2508_"
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2508__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2508__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2508__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2508__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2508__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2508__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2508__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2508__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2508__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2508__byte_offset 	240	/* 0x000000f0 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2508__vbase_byte_offset 	244	/* 0x000000f4 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2508__dbase_byte_offset 	268	/* 0x0000010c */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2508__dpitchm1_byte_offset 	272	/* 0x00000110 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2508__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2508__vwidth_minus_one_byte_offset 	256	/* 0x00000100 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2508__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2508__vheight_minus_one_byte_offset 	258	/* 0x00000102 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2508__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2508__drotate_bit_offset 	2118	/* 0x00000846 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2508__hflip_bit_offset 	2119	/* 0x00000847 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2508__vflip_bit_offset 	2120	/* 0x00000848 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2508__dflip_bit_offset 	2121	/* 0x00000849 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2508__pflip_bit_offset 	2122	/* 0x0000084a */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2508__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2508__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2508__D 	70
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2508__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2508__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2508__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2508__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2508__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2508__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2508__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2508__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2508__vmem_buffer_num_bytes 	14000
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2508__dram_size_num_bytes 	16800
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2508__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2508__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2508__expoffset 	12
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2508__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2508__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2508__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2508__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2508__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2508__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2508__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_26 HMB_input __pvcn_2464_ */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2464__cnngen_demangled_name 	"__pvcn_2464_"
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2464__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2464__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2464__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2464__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2464__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2464__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2464__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2464__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2464__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2464__byte_offset 	280	/* 0x00000118 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2464__vbase_byte_offset 	284	/* 0x0000011c */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2464__dbase_byte_offset 	308	/* 0x00000134 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2464__dpitchm1_byte_offset 	312	/* 0x00000138 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2464__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2464__vwidth_minus_one_byte_offset 	296	/* 0x00000128 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2464__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2464__vheight_minus_one_byte_offset 	298	/* 0x0000012a */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2464__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2464__drotate_bit_offset 	2438	/* 0x00000986 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2464__hflip_bit_offset 	2439	/* 0x00000987 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2464__vflip_bit_offset 	2440	/* 0x00000988 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2464__dflip_bit_offset 	2441	/* 0x00000989 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2464__pflip_bit_offset 	2442	/* 0x0000098a */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2464__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2464__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2464__D 	72
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2464__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2464__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2464__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2464__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2464__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2464__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2464__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2464__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2464__vmem_buffer_num_bytes 	14400
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2464__dram_size_num_bytes 	17280
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2464__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2464__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2464__expoffset 	12
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2464__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2464__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2464__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2464__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2464__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2464__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2464__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_26 HMB_input __pvcn_2431_ */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2431__cnngen_demangled_name 	"__pvcn_2431_"
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2431__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2431__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2431__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2431__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2431__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2431__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2431__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2431__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2431__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2431__byte_offset 	320	/* 0x00000140 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2431__vbase_byte_offset 	324	/* 0x00000144 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2431__dbase_byte_offset 	348	/* 0x0000015c */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2431__dpitchm1_byte_offset 	352	/* 0x00000160 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2431__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2431__vwidth_minus_one_byte_offset 	336	/* 0x00000150 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2431__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2431__vheight_minus_one_byte_offset 	338	/* 0x00000152 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2431__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2431__drotate_bit_offset 	2758	/* 0x00000ac6 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2431__hflip_bit_offset 	2759	/* 0x00000ac7 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2431__vflip_bit_offset 	2760	/* 0x00000ac8 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2431__dflip_bit_offset 	2761	/* 0x00000ac9 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2431__pflip_bit_offset 	2762	/* 0x00000aca */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2431__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2431__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2431__D 	73
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2431__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2431__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2431__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2431__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2431__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2431__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2431__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2431__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2431__vmem_buffer_num_bytes 	14600
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2431__dram_size_num_bytes 	17536
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2431__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2431__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2431__expoffset 	12
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2431__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2431__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2431__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2431__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2431__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2431__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2431__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_26 HMB_input __pvcn_2486_ */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2486__cnngen_demangled_name 	"__pvcn_2486_"
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2486__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2486__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2486__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2486__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2486__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2486__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2486__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2486__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2486__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2486__byte_offset 	360	/* 0x00000168 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2486__vbase_byte_offset 	364	/* 0x0000016c */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2486__dbase_byte_offset 	388	/* 0x00000184 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2486__dpitchm1_byte_offset 	392	/* 0x00000188 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2486__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2486__vwidth_minus_one_byte_offset 	376	/* 0x00000178 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2486__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2486__vheight_minus_one_byte_offset 	378	/* 0x0000017a */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2486__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2486__drotate_bit_offset 	3078	/* 0x00000c06 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2486__hflip_bit_offset 	3079	/* 0x00000c07 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2486__vflip_bit_offset 	3080	/* 0x00000c08 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2486__dflip_bit_offset 	3081	/* 0x00000c09 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2486__pflip_bit_offset 	3082	/* 0x00000c0a */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2486__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2486__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2486__D 	71
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2486__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2486__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2486__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2486__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2486__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2486__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2486__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2486__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2486__vmem_buffer_num_bytes 	14200
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2486__dram_size_num_bytes 	17056
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2486__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2486__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2486__expoffset 	12
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2486__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2486__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2486__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2486__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2486__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2486__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2486__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_26 HMB_input __pvcn_2365_ */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2365__cnngen_demangled_name 	"__pvcn_2365_"
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2365__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2365__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2365__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2365__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2365__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2365__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2365__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2365__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2365__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2365__byte_offset 	400	/* 0x00000190 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2365__vbase_byte_offset 	404	/* 0x00000194 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2365__dbase_byte_offset 	428	/* 0x000001ac */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2365__dpitchm1_byte_offset 	432	/* 0x000001b0 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2365__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2365__vwidth_minus_one_byte_offset 	416	/* 0x000001a0 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2365__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2365__vheight_minus_one_byte_offset 	418	/* 0x000001a2 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2365__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2365__drotate_bit_offset 	3398	/* 0x00000d46 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2365__hflip_bit_offset 	3399	/* 0x00000d47 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2365__vflip_bit_offset 	3400	/* 0x00000d48 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2365__dflip_bit_offset 	3401	/* 0x00000d49 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2365__pflip_bit_offset 	3402	/* 0x00000d4a */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2365__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2365__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2365__D 	78
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2365__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2365__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2365__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2365__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2365__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2365__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2365__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2365__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2365__vmem_buffer_num_bytes 	15600
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2365__dram_size_num_bytes 	18720
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2365__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2365__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2365__expoffset 	12
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2365__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2365__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2365__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2365__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2365__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2365__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2365__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_26 HMB_input __pvcn_2409_ */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2409__cnngen_demangled_name 	"__pvcn_2409_"
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2409__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2409__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2409__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2409__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2409__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2409__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2409__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2409__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2409__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2409__byte_offset 	436	/* 0x000001b4 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2409__vbase_byte_offset 	440	/* 0x000001b8 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2409__dbase_byte_offset 	464	/* 0x000001d0 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2409__dpitchm1_byte_offset 	468	/* 0x000001d4 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2409__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2409__vwidth_minus_one_byte_offset 	452	/* 0x000001c4 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2409__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2409__vheight_minus_one_byte_offset 	454	/* 0x000001c6 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2409__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2409__drotate_bit_offset 	3686	/* 0x00000e66 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2409__hflip_bit_offset 	3687	/* 0x00000e67 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2409__vflip_bit_offset 	3688	/* 0x00000e68 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2409__dflip_bit_offset 	3689	/* 0x00000e69 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2409__pflip_bit_offset 	3690	/* 0x00000e6a */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2409__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2409__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2409__D 	70
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2409__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2409__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2409__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2409__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2409__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2409__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2409__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2409__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2409__vmem_buffer_num_bytes 	14000
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2409__dram_size_num_bytes 	16800
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2409__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2409__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2409__expoffset 	12
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2409__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2409__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2409__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2409__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2409__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2409__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2409__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_26 HMB_input __pvcn_2398_ */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2398__cnngen_demangled_name 	"__pvcn_2398_"
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2398__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2398__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2398__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2398__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2398__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2398__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2398__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2398__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2398__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2398__byte_offset 	476	/* 0x000001dc */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2398__vbase_byte_offset 	480	/* 0x000001e0 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2398__dbase_byte_offset 	504	/* 0x000001f8 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2398__dpitchm1_byte_offset 	508	/* 0x000001fc */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2398__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2398__vwidth_minus_one_byte_offset 	492	/* 0x000001ec */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2398__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2398__vheight_minus_one_byte_offset 	494	/* 0x000001ee */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2398__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2398__drotate_bit_offset 	4006	/* 0x00000fa6 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2398__hflip_bit_offset 	4007	/* 0x00000fa7 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2398__vflip_bit_offset 	4008	/* 0x00000fa8 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2398__dflip_bit_offset 	4009	/* 0x00000fa9 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2398__pflip_bit_offset 	4010	/* 0x00000faa */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2398__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2398__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2398__D 	75
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2398__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2398__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2398__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2398__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2398__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2398__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2398__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2398__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2398__vmem_buffer_num_bytes 	15000
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2398__dram_size_num_bytes 	18016
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2398__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2398__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2398__expoffset 	12
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2398__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2398__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2398__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2398__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2398__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2398__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2398__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_26 HMB_input __pvcn_2387_ */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2387__cnngen_demangled_name 	"__pvcn_2387_"
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2387__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2387__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2387__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2387__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2387__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2387__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2387__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2387__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2387__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2387__byte_offset 	516	/* 0x00000204 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2387__vbase_byte_offset 	520	/* 0x00000208 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2387__dbase_byte_offset 	544	/* 0x00000220 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2387__dpitchm1_byte_offset 	548	/* 0x00000224 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2387__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2387__vwidth_minus_one_byte_offset 	532	/* 0x00000214 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2387__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2387__vheight_minus_one_byte_offset 	534	/* 0x00000216 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2387__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2387__drotate_bit_offset 	4326	/* 0x000010e6 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2387__hflip_bit_offset 	4327	/* 0x000010e7 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2387__vflip_bit_offset 	4328	/* 0x000010e8 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2387__dflip_bit_offset 	4329	/* 0x000010e9 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2387__pflip_bit_offset 	4330	/* 0x000010ea */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2387__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2387__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2387__D 	75
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2387__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2387__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2387__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2387__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2387__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2387__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2387__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2387__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2387__vmem_buffer_num_bytes 	15000
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2387__dram_size_num_bytes 	18016
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2387__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2387__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2387__expoffset 	12
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2387__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2387__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2387__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2387__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2387__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2387__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2387__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_26 HMB_input __pvcn_2376_ */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2376__cnngen_demangled_name 	"__pvcn_2376_"
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2376__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2376__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2376__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2376__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2376__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2376__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2376__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2376__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2376__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2376__byte_offset 	556	/* 0x0000022c */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2376__vbase_byte_offset 	560	/* 0x00000230 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2376__dbase_byte_offset 	584	/* 0x00000248 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2376__dpitchm1_byte_offset 	588	/* 0x0000024c */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2376__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2376__vwidth_minus_one_byte_offset 	572	/* 0x0000023c */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2376__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2376__vheight_minus_one_byte_offset 	574	/* 0x0000023e */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2376__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2376__drotate_bit_offset 	4646	/* 0x00001226 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2376__hflip_bit_offset 	4647	/* 0x00001227 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2376__vflip_bit_offset 	4648	/* 0x00001228 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2376__dflip_bit_offset 	4649	/* 0x00001229 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2376__pflip_bit_offset 	4650	/* 0x0000122a */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2376__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2376__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2376__D 	69
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2376__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2376__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2376__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2376__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2376__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2376__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2376__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2376__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2376__vmem_buffer_num_bytes 	13800
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2376__dram_size_num_bytes 	16576
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2376__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2376__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2376__expoffset 	12
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2376__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2376__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2376__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2376__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2376__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2376__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2376__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_26 HMB_output __pvcn_2359_ */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2359__cnngen_demangled_name 	"__pvcn_2359_"
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2359__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2359__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2359__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2359__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2359__byte_offset 	608	/* 0x00000260 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2359__vbase_byte_offset 	612	/* 0x00000264 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2359__dbase_byte_offset 	636	/* 0x0000027c */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2359__dpitchm1_byte_offset 	640	/* 0x00000280 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2359__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2359__vwidth_minus_one_byte_offset 	624	/* 0x00000270 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2359__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2359__vheight_minus_one_byte_offset 	626	/* 0x00000272 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2359__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2359__drotate_bit_offset 	5062	/* 0x000013c6 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2359__hflip_bit_offset 	5063	/* 0x000013c7 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2359__vflip_bit_offset 	5064	/* 0x000013c8 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2359__dflip_bit_offset 	5065	/* 0x000013c9 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2359__pflip_bit_offset 	5066	/* 0x000013ca */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2359__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2359__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2359__D 	78
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2359__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2359__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2359__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2359__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2359__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2359__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2359__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2359__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2359__vmem_buffer_num_bytes 	15600
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2359__dram_size_num_bytes 	18720
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2359__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2359__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2359__expoffset 	15
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2359__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2359__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2359__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2359__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2359__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2359__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2359__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_26 HMB_output __pvcn_2370_ */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2370__cnngen_demangled_name 	"__pvcn_2370_"
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2370__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2370__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2370__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2370__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2370__byte_offset 	656	/* 0x00000290 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2370__vbase_byte_offset 	660	/* 0x00000294 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2370__dbase_byte_offset 	684	/* 0x000002ac */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2370__dpitchm1_byte_offset 	688	/* 0x000002b0 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2370__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2370__vwidth_minus_one_byte_offset 	672	/* 0x000002a0 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2370__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2370__vheight_minus_one_byte_offset 	674	/* 0x000002a2 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2370__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2370__drotate_bit_offset 	5446	/* 0x00001546 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2370__hflip_bit_offset 	5447	/* 0x00001547 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2370__vflip_bit_offset 	5448	/* 0x00001548 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2370__dflip_bit_offset 	5449	/* 0x00001549 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2370__pflip_bit_offset 	5450	/* 0x0000154a */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2370__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2370__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2370__D 	69
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2370__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2370__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2370__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2370__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2370__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2370__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2370__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2370__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2370__vmem_buffer_num_bytes 	13800
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2370__dram_size_num_bytes 	16576
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2370__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2370__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2370__expoffset 	15
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2370__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2370__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2370__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2370__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2370__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2370__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2370__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_26 HMB_output __pvcn_2381_ */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2381__cnngen_demangled_name 	"__pvcn_2381_"
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2381__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2381__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2381__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2381__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2381__byte_offset 	704	/* 0x000002c0 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2381__vbase_byte_offset 	708	/* 0x000002c4 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2381__dbase_byte_offset 	732	/* 0x000002dc */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2381__dpitchm1_byte_offset 	736	/* 0x000002e0 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2381__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2381__vwidth_minus_one_byte_offset 	720	/* 0x000002d0 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2381__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2381__vheight_minus_one_byte_offset 	722	/* 0x000002d2 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2381__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2381__drotate_bit_offset 	5830	/* 0x000016c6 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2381__hflip_bit_offset 	5831	/* 0x000016c7 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2381__vflip_bit_offset 	5832	/* 0x000016c8 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2381__dflip_bit_offset 	5833	/* 0x000016c9 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2381__pflip_bit_offset 	5834	/* 0x000016ca */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2381__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2381__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2381__D 	75
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2381__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2381__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2381__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2381__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2381__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2381__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2381__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2381__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2381__vmem_buffer_num_bytes 	15000
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2381__dram_size_num_bytes 	18016
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2381__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2381__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2381__expoffset 	15
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2381__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2381__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2381__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2381__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2381__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2381__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2381__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_26 HMB_output __pvcn_2392_ */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2392__cnngen_demangled_name 	"__pvcn_2392_"
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2392__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2392__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2392__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2392__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2392__byte_offset 	752	/* 0x000002f0 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2392__vbase_byte_offset 	756	/* 0x000002f4 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2392__dbase_byte_offset 	780	/* 0x0000030c */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2392__dpitchm1_byte_offset 	784	/* 0x00000310 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2392__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2392__vwidth_minus_one_byte_offset 	768	/* 0x00000300 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2392__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2392__vheight_minus_one_byte_offset 	770	/* 0x00000302 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2392__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2392__drotate_bit_offset 	6214	/* 0x00001846 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2392__hflip_bit_offset 	6215	/* 0x00001847 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2392__vflip_bit_offset 	6216	/* 0x00001848 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2392__dflip_bit_offset 	6217	/* 0x00001849 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2392__pflip_bit_offset 	6218	/* 0x0000184a */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2392__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2392__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2392__D 	75
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2392__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2392__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2392__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2392__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2392__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2392__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2392__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2392__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2392__vmem_buffer_num_bytes 	15000
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2392__dram_size_num_bytes 	18016
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2392__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2392__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2392__expoffset 	15
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2392__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2392__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2392__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2392__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2392__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2392__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2392__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_26 HMB_output __pvcn_2403_ */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2403__cnngen_demangled_name 	"__pvcn_2403_"
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2403__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2403__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2403__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2403__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2403__byte_offset 	800	/* 0x00000320 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2403__vbase_byte_offset 	804	/* 0x00000324 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2403__dbase_byte_offset 	828	/* 0x0000033c */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2403__dpitchm1_byte_offset 	832	/* 0x00000340 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2403__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2403__vwidth_minus_one_byte_offset 	816	/* 0x00000330 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2403__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2403__vheight_minus_one_byte_offset 	818	/* 0x00000332 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2403__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2403__drotate_bit_offset 	6598	/* 0x000019c6 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2403__hflip_bit_offset 	6599	/* 0x000019c7 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2403__vflip_bit_offset 	6600	/* 0x000019c8 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2403__dflip_bit_offset 	6601	/* 0x000019c9 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2403__pflip_bit_offset 	6602	/* 0x000019ca */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2403__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2403__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2403__D 	70
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2403__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2403__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2403__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2403__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2403__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2403__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2403__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2403__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2403__vmem_buffer_num_bytes 	14000
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2403__dram_size_num_bytes 	16800
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2403__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2403__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2403__expoffset 	15
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2403__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2403__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2403__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2403__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2403__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2403__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2403__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_26 HMB_output __pvcn_2414_ */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2414__cnngen_demangled_name 	"__pvcn_2414_"
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2414__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2414__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2414__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2414__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2414__byte_offset 	848	/* 0x00000350 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2414__vbase_byte_offset 	852	/* 0x00000354 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2414__dbase_byte_offset 	876	/* 0x0000036c */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2414__dpitchm1_byte_offset 	880	/* 0x00000370 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2414__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2414__vwidth_minus_one_byte_offset 	864	/* 0x00000360 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2414__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2414__vheight_minus_one_byte_offset 	866	/* 0x00000362 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2414__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2414__drotate_bit_offset 	6982	/* 0x00001b46 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2414__hflip_bit_offset 	6983	/* 0x00001b47 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2414__vflip_bit_offset 	6984	/* 0x00001b48 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2414__dflip_bit_offset 	6985	/* 0x00001b49 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2414__pflip_bit_offset 	6986	/* 0x00001b4a */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2414__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2414__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2414__D 	72
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2414__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2414__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2414__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2414__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2414__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2414__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2414__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2414__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2414__vmem_buffer_num_bytes 	14400
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2414__dram_size_num_bytes 	17280
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2414__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2414__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2414__expoffset 	15
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2414__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2414__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2414__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2414__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2414__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2414__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2414__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_26 HMB_output __pvcn_2425_ */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2425__cnngen_demangled_name 	"__pvcn_2425_"
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2425__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2425__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2425__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2425__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2425__byte_offset 	896	/* 0x00000380 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2425__vbase_byte_offset 	900	/* 0x00000384 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2425__dbase_byte_offset 	924	/* 0x0000039c */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2425__dpitchm1_byte_offset 	928	/* 0x000003a0 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2425__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2425__vwidth_minus_one_byte_offset 	912	/* 0x00000390 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2425__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2425__vheight_minus_one_byte_offset 	914	/* 0x00000392 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2425__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2425__drotate_bit_offset 	7366	/* 0x00001cc6 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2425__hflip_bit_offset 	7367	/* 0x00001cc7 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2425__vflip_bit_offset 	7368	/* 0x00001cc8 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2425__dflip_bit_offset 	7369	/* 0x00001cc9 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2425__pflip_bit_offset 	7370	/* 0x00001cca */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2425__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2425__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2425__D 	73
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2425__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2425__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2425__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2425__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2425__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2425__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2425__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2425__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2425__vmem_buffer_num_bytes 	14600
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2425__dram_size_num_bytes 	17536
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2425__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2425__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2425__expoffset 	15
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2425__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2425__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2425__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2425__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2425__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2425__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2425__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_26 HMB_output __pvcn_2436_ */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2436__cnngen_demangled_name 	"__pvcn_2436_"
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2436__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2436__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2436__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2436__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2436__byte_offset 	944	/* 0x000003b0 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2436__vbase_byte_offset 	948	/* 0x000003b4 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2436__dbase_byte_offset 	972	/* 0x000003cc */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2436__dpitchm1_byte_offset 	976	/* 0x000003d0 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2436__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2436__vwidth_minus_one_byte_offset 	960	/* 0x000003c0 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2436__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2436__vheight_minus_one_byte_offset 	962	/* 0x000003c2 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2436__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2436__drotate_bit_offset 	7750	/* 0x00001e46 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2436__hflip_bit_offset 	7751	/* 0x00001e47 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2436__vflip_bit_offset 	7752	/* 0x00001e48 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2436__dflip_bit_offset 	7753	/* 0x00001e49 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2436__pflip_bit_offset 	7754	/* 0x00001e4a */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2436__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2436__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2436__D 	70
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2436__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2436__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2436__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2436__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2436__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2436__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2436__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2436__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2436__vmem_buffer_num_bytes 	14000
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2436__dram_size_num_bytes 	16800
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2436__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2436__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2436__expoffset 	15
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2436__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2436__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2436__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2436__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2436__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2436__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2436__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_26 HMB_output __pvcn_2447_ */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2447__cnngen_demangled_name 	"__pvcn_2447_"
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2447__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2447__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2447__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2447__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2447__byte_offset 	992	/* 0x000003e0 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2447__vbase_byte_offset 	996	/* 0x000003e4 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2447__dbase_byte_offset 	1020	/* 0x000003fc */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2447__dpitchm1_byte_offset 	1024	/* 0x00000400 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2447__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2447__vwidth_minus_one_byte_offset 	1008	/* 0x000003f0 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2447__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2447__vheight_minus_one_byte_offset 	1010	/* 0x000003f2 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2447__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2447__drotate_bit_offset 	8134	/* 0x00001fc6 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2447__hflip_bit_offset 	8135	/* 0x00001fc7 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2447__vflip_bit_offset 	8136	/* 0x00001fc8 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2447__dflip_bit_offset 	8137	/* 0x00001fc9 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2447__pflip_bit_offset 	8138	/* 0x00001fca */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2447__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2447__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2447__D 	74
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2447__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2447__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2447__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2447__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2447__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2447__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2447__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2447__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2447__vmem_buffer_num_bytes 	14800
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2447__dram_size_num_bytes 	17760
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2447__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2447__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2447__expoffset 	15
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2447__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2447__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2447__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2447__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2447__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2447__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2447__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_26 HMB_output __pvcn_2458_ */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2458__cnngen_demangled_name 	"__pvcn_2458_"
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2458__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2458__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2458__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2458__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2458__byte_offset 	1040	/* 0x00000410 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2458__vbase_byte_offset 	1044	/* 0x00000414 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2458__dbase_byte_offset 	1068	/* 0x0000042c */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2458__dpitchm1_byte_offset 	1072	/* 0x00000430 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2458__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2458__vwidth_minus_one_byte_offset 	1056	/* 0x00000420 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2458__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2458__vheight_minus_one_byte_offset 	1058	/* 0x00000422 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2458__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2458__drotate_bit_offset 	8518	/* 0x00002146 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2458__hflip_bit_offset 	8519	/* 0x00002147 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2458__vflip_bit_offset 	8520	/* 0x00002148 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2458__dflip_bit_offset 	8521	/* 0x00002149 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2458__pflip_bit_offset 	8522	/* 0x0000214a */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2458__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2458__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2458__D 	72
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2458__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2458__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2458__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2458__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2458__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2458__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2458__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2458__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2458__vmem_buffer_num_bytes 	14400
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2458__dram_size_num_bytes 	17280
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2458__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2458__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2458__expoffset 	15
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2458__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2458__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2458__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2458__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2458__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2458__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2458__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_26 HMB_output __pvcn_2469_ */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2469__cnngen_demangled_name 	"__pvcn_2469_"
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2469__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2469__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2469__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2469__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2469__byte_offset 	1088	/* 0x00000440 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2469__vbase_byte_offset 	1092	/* 0x00000444 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2469__dbase_byte_offset 	1116	/* 0x0000045c */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2469__dpitchm1_byte_offset 	1120	/* 0x00000460 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2469__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2469__vwidth_minus_one_byte_offset 	1104	/* 0x00000450 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2469__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2469__vheight_minus_one_byte_offset 	1106	/* 0x00000452 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2469__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2469__drotate_bit_offset 	8902	/* 0x000022c6 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2469__hflip_bit_offset 	8903	/* 0x000022c7 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2469__vflip_bit_offset 	8904	/* 0x000022c8 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2469__dflip_bit_offset 	8905	/* 0x000022c9 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2469__pflip_bit_offset 	8906	/* 0x000022ca */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2469__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2469__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2469__D 	76
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2469__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2469__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2469__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2469__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2469__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2469__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2469__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2469__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2469__vmem_buffer_num_bytes 	15200
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2469__dram_size_num_bytes 	18240
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2469__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2469__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2469__expoffset 	15
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2469__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2469__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2469__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2469__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2469__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2469__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2469__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_26 HMB_output __pvcn_2480_ */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2480__cnngen_demangled_name 	"__pvcn_2480_"
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2480__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2480__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2480__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2480__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2480__byte_offset 	1136	/* 0x00000470 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2480__vbase_byte_offset 	1140	/* 0x00000474 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2480__dbase_byte_offset 	1164	/* 0x0000048c */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2480__dpitchm1_byte_offset 	1168	/* 0x00000490 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2480__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2480__vwidth_minus_one_byte_offset 	1152	/* 0x00000480 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2480__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2480__vheight_minus_one_byte_offset 	1154	/* 0x00000482 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2480__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2480__drotate_bit_offset 	9286	/* 0x00002446 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2480__hflip_bit_offset 	9287	/* 0x00002447 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2480__vflip_bit_offset 	9288	/* 0x00002448 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2480__dflip_bit_offset 	9289	/* 0x00002449 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2480__pflip_bit_offset 	9290	/* 0x0000244a */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2480__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2480__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2480__D 	71
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2480__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2480__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2480__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2480__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2480__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2480__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2480__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2480__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2480__vmem_buffer_num_bytes 	14200
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2480__dram_size_num_bytes 	17056
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2480__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2480__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2480__expoffset 	15
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2480__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2480__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2480__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2480__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2480__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2480__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2480__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_26 HMB_output __pvcn_2491_ */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2491__cnngen_demangled_name 	"__pvcn_2491_"
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2491__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2491__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2491__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2491__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2491__byte_offset 	1184	/* 0x000004a0 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2491__vbase_byte_offset 	1188	/* 0x000004a4 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2491__dbase_byte_offset 	1212	/* 0x000004bc */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2491__dpitchm1_byte_offset 	1216	/* 0x000004c0 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2491__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2491__vwidth_minus_one_byte_offset 	1200	/* 0x000004b0 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2491__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2491__vheight_minus_one_byte_offset 	1202	/* 0x000004b2 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2491__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2491__drotate_bit_offset 	9670	/* 0x000025c6 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2491__hflip_bit_offset 	9671	/* 0x000025c7 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2491__vflip_bit_offset 	9672	/* 0x000025c8 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2491__dflip_bit_offset 	9673	/* 0x000025c9 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2491__pflip_bit_offset 	9674	/* 0x000025ca */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2491__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2491__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2491__D 	73
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2491__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2491__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2491__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2491__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2491__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2491__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2491__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2491__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2491__vmem_buffer_num_bytes 	14600
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2491__dram_size_num_bytes 	17536
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2491__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2491__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2491__expoffset 	15
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2491__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2491__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2491__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2491__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2491__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2491__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2491__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_26 HMB_output __pvcn_2502_ */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2502__cnngen_demangled_name 	"__pvcn_2502_"
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2502__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2502__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2502__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2502__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2502__byte_offset 	1232	/* 0x000004d0 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2502__vbase_byte_offset 	1236	/* 0x000004d4 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2502__dbase_byte_offset 	1260	/* 0x000004ec */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2502__dpitchm1_byte_offset 	1264	/* 0x000004f0 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2502__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2502__vwidth_minus_one_byte_offset 	1248	/* 0x000004e0 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2502__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2502__vheight_minus_one_byte_offset 	1250	/* 0x000004e2 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2502__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2502__drotate_bit_offset 	10054	/* 0x00002746 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2502__hflip_bit_offset 	10055	/* 0x00002747 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2502__vflip_bit_offset 	10056	/* 0x00002748 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2502__dflip_bit_offset 	10057	/* 0x00002749 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2502__pflip_bit_offset 	10058	/* 0x0000274a */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2502__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2502__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2502__D 	70
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2502__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2502__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2502__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2502__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2502__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2502__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2502__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2502__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2502__vmem_buffer_num_bytes 	14000
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2502__dram_size_num_bytes 	16800
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2502__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2502__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2502__expoffset 	15
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2502__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2502__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2502__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2502__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2502__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2502__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2502__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_26 HMB_output __pvcn_2513_ */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2513__cnngen_demangled_name 	"__pvcn_2513_"
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2513__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2513__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2513__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2513__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2513__byte_offset 	1280	/* 0x00000500 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2513__vbase_byte_offset 	1284	/* 0x00000504 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2513__dbase_byte_offset 	1308	/* 0x0000051c */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2513__dpitchm1_byte_offset 	1312	/* 0x00000520 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2513__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2513__vwidth_minus_one_byte_offset 	1296	/* 0x00000510 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2513__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2513__vheight_minus_one_byte_offset 	1298	/* 0x00000512 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2513__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2513__drotate_bit_offset 	10438	/* 0x000028c6 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2513__hflip_bit_offset 	10439	/* 0x000028c7 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2513__vflip_bit_offset 	10440	/* 0x000028c8 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2513__dflip_bit_offset 	10441	/* 0x000028c9 */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2513__pflip_bit_offset 	10442	/* 0x000028ca */
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2513__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2513__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2513__D 	6
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2513__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2513__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2513__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2513__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2513__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2513__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2513__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2513__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2513__vmem_buffer_num_bytes 	1440
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2513__dram_size_num_bytes 	1440
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2513__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2513__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2513__expoffset 	15
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2513__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2513__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2513__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2513__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2513__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2513__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___pvcn_2513__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_26 SMB_output __vas_created_terminate__v0 */
#define mnet_ssd_adas_flex_pic_prim_split_26___vas_created_terminate__v0_cnngen_demangled_name 	"__vas_created_terminate__v0"
#define mnet_ssd_adas_flex_pic_prim_split_26___vas_created_terminate__v0_is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___vas_created_terminate__v0_is_bitvector 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___vas_created_terminate__v0_is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___vas_created_terminate__v0_is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___vas_created_terminate__v0_byte_offset 	1320	/* 0x00000528 */
#define mnet_ssd_adas_flex_pic_prim_split_26___vas_created_terminate__v0_vbase_byte_offset 	1324	/* 0x0000052c */
#define mnet_ssd_adas_flex_pic_prim_split_26___vas_created_terminate__v0_W 	256
#define mnet_ssd_adas_flex_pic_prim_split_26___vas_created_terminate__v0_H 	3
#define mnet_ssd_adas_flex_pic_prim_split_26___vas_created_terminate__v0_D 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___vas_created_terminate__v0_P 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___vas_created_terminate__v0_data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___vas_created_terminate__v0_denable 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___vas_created_terminate__v0_dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_26___vas_created_terminate__v0_vp_interleave_mode 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___vas_created_terminate__v0_tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___vas_created_terminate__v0_tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_26___vas_created_terminate__v0_vmem_buffer_num_bytes 	96


#endif /* mnet_ssd_adas_flex_pic_prim_split_26_H */
