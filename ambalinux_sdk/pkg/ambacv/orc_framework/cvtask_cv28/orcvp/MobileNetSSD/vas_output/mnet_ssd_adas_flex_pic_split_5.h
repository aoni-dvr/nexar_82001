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
#ifndef mnet_ssd_adas_flex_pic_prim_split_5_H
#define mnet_ssd_adas_flex_pic_prim_split_5_H

#define vas_gen_header_version                  	7

/* dag name */
#define mnet_ssd_adas_flex_pic_prim_split_5_DAG_NAME 	"mnet_ssd_adas_flex_pic_prim_split_5"

/* VDG file info */
#define mnet_ssd_adas_flex_pic_prim_split_5_vdg_name 	"mnet_ssd_adas_flex_pic_split_5.vdg"

/* VDG DAG memory info */
#define mnet_ssd_adas_flex_pic_prim_split_5_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_5_byte_size 	1380	/* 0x00000564 */
#define mnet_ssd_adas_flex_pic_prim_split_5_preferred_dram_xfer_size 	1

/* mnet_ssd_adas_flex_pic_prim_split_5 VMEM info */
#define mnet_ssd_adas_flex_pic_prim_split_5_use_ping_pong_vmem 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_VMEM_start 	4096	/* 0x00001000 */
#define mnet_ssd_adas_flex_pic_prim_split_5_VMEM_end 	499552	/* 0x00079f60 */
#define mnet_ssd_adas_flex_pic_prim_split_5_image_start 	499552	/* 0x00079f60 */
#define mnet_ssd_adas_flex_pic_prim_split_5_image_size 	1380	/* 0x00000564 */
#define mnet_ssd_adas_flex_pic_prim_split_5_dagbin_start 	499552	/* 0x00079f60 */

/* mnet_ssd_adas_flex_pic_prim_split_5 DAG info */
#define mnet_ssd_adas_flex_pic_prim_split_5_estimated_cycles 	59095
#define mnet_ssd_adas_flex_pic_prim_split_5_ddi_byte_size 	0	/* 0x00000000 */

/*******************************************************************************
 * DAG byte offset info for HMB/SMB/VCB and their fields
 * in DAG 'mnet_ssd_adas_flex_pic_prim_split_5' in source file 'pre_split5_mnet_ssd_adas_flex_pic_split_0.vas'
 ******************************************************************************/
/* mnet_ssd_adas_flex_pic_prim_split_5 HMB_input __pvcn_543_ */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_543__cnngen_demangled_name 	"__pvcn_543_"
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_543__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_543__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_543__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_543__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_543__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_543__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_543__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_543__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_543__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_543__byte_offset 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_543__vbase_byte_offset 	4	/* 0x00000004 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_543__dbase_byte_offset 	28	/* 0x0000001c */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_543__dpitchm1_byte_offset 	32	/* 0x00000020 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_543__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_543__vwidth_minus_one_byte_offset 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_543__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_543__vheight_minus_one_byte_offset 	18	/* 0x00000012 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_543__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_543__drotate_bit_offset 	198	/* 0x000000c6 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_543__hflip_bit_offset 	199	/* 0x000000c7 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_543__vflip_bit_offset 	200	/* 0x000000c8 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_543__dflip_bit_offset 	201	/* 0x000000c9 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_543__pflip_bit_offset 	202	/* 0x000000ca */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_543__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_543__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_543__D 	7
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_543__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_543__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_543__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_543__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_543__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_543__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_543__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_543__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_543__vmem_buffer_num_bytes 	17920
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_543__dram_size_num_bytes 	21280
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_543__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_543__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_543__expoffset 	13
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_543__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_543__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_543__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_543__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_543__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_543__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_543__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_5 HMB_input __pvcn_620_ */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_620__cnngen_demangled_name 	"__pvcn_620_"
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_620__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_620__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_620__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_620__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_620__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_620__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_620__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_620__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_620__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_620__byte_offset 	40	/* 0x00000028 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_620__vbase_byte_offset 	44	/* 0x0000002c */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_620__dbase_byte_offset 	68	/* 0x00000044 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_620__dpitchm1_byte_offset 	72	/* 0x00000048 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_620__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_620__vwidth_minus_one_byte_offset 	56	/* 0x00000038 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_620__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_620__vheight_minus_one_byte_offset 	58	/* 0x0000003a */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_620__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_620__drotate_bit_offset 	518	/* 0x00000206 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_620__hflip_bit_offset 	519	/* 0x00000207 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_620__vflip_bit_offset 	520	/* 0x00000208 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_620__dflip_bit_offset 	521	/* 0x00000209 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_620__pflip_bit_offset 	522	/* 0x0000020a */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_620__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_620__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_620__D 	8
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_620__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_620__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_620__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_620__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_620__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_620__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_620__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_620__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_620__vmem_buffer_num_bytes 	20480
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_620__dram_size_num_bytes 	24320
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_620__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_620__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_620__expoffset 	13
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_620__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_620__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_620__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_620__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_620__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_620__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_620__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_5 HMB_input __pvcn_686_ */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_686__cnngen_demangled_name 	"__pvcn_686_"
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_686__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_686__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_686__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_686__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_686__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_686__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_686__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_686__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_686__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_686__byte_offset 	80	/* 0x00000050 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_686__vbase_byte_offset 	84	/* 0x00000054 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_686__dbase_byte_offset 	108	/* 0x0000006c */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_686__dpitchm1_byte_offset 	112	/* 0x00000070 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_686__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_686__vwidth_minus_one_byte_offset 	96	/* 0x00000060 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_686__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_686__vheight_minus_one_byte_offset 	98	/* 0x00000062 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_686__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_686__drotate_bit_offset 	838	/* 0x00000346 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_686__hflip_bit_offset 	839	/* 0x00000347 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_686__vflip_bit_offset 	840	/* 0x00000348 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_686__dflip_bit_offset 	841	/* 0x00000349 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_686__pflip_bit_offset 	842	/* 0x0000034a */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_686__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_686__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_686__D 	7
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_686__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_686__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_686__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_686__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_686__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_686__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_686__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_686__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_686__vmem_buffer_num_bytes 	17920
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_686__dram_size_num_bytes 	21280
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_686__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_686__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_686__expoffset 	13
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_686__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_686__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_686__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_686__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_686__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_686__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_686__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_5 HMB_input __pvcn_725_ */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_725__cnngen_demangled_name 	"__pvcn_725_"
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_725__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_725__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_725__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_725__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_725__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_725__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_725__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_725__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_725__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_725__byte_offset 	120	/* 0x00000078 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_725__vbase_byte_offset 	124	/* 0x0000007c */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_725__dbase_byte_offset 	148	/* 0x00000094 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_725__dpitchm1_byte_offset 	152	/* 0x00000098 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_725__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_725__vwidth_minus_one_byte_offset 	136	/* 0x00000088 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_725__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_725__vheight_minus_one_byte_offset 	138	/* 0x0000008a */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_725__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_725__drotate_bit_offset 	1158	/* 0x00000486 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_725__hflip_bit_offset 	1159	/* 0x00000487 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_725__vflip_bit_offset 	1160	/* 0x00000488 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_725__dflip_bit_offset 	1161	/* 0x00000489 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_725__pflip_bit_offset 	1162	/* 0x0000048a */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_725__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_725__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_725__D 	6
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_725__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_725__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_725__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_725__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_725__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_725__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_725__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_725__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_725__vmem_buffer_num_bytes 	15360
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_725__dram_size_num_bytes 	18240
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_725__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_725__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_725__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_725__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_725__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_725__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_725__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_725__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_725__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_725__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_5 HMB_input __pvcn_675_ */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_675__cnngen_demangled_name 	"__pvcn_675_"
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_675__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_675__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_675__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_675__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_675__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_675__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_675__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_675__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_675__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_675__byte_offset 	160	/* 0x000000a0 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_675__vbase_byte_offset 	164	/* 0x000000a4 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_675__dbase_byte_offset 	188	/* 0x000000bc */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_675__dpitchm1_byte_offset 	192	/* 0x000000c0 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_675__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_675__vwidth_minus_one_byte_offset 	176	/* 0x000000b0 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_675__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_675__vheight_minus_one_byte_offset 	178	/* 0x000000b2 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_675__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_675__drotate_bit_offset 	1478	/* 0x000005c6 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_675__hflip_bit_offset 	1479	/* 0x000005c7 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_675__vflip_bit_offset 	1480	/* 0x000005c8 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_675__dflip_bit_offset 	1481	/* 0x000005c9 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_675__pflip_bit_offset 	1482	/* 0x000005ca */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_675__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_675__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_675__D 	9
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_675__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_675__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_675__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_675__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_675__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_675__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_675__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_675__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_675__vmem_buffer_num_bytes 	23040
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_675__dram_size_num_bytes 	27360
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_675__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_675__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_675__expoffset 	13
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_675__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_675__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_675__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_675__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_675__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_675__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_675__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_5 HMB_input __pvcn_714_ */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_714__cnngen_demangled_name 	"__pvcn_714_"
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_714__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_714__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_714__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_714__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_714__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_714__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_714__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_714__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_714__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_714__byte_offset 	200	/* 0x000000c8 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_714__vbase_byte_offset 	204	/* 0x000000cc */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_714__dbase_byte_offset 	228	/* 0x000000e4 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_714__dpitchm1_byte_offset 	232	/* 0x000000e8 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_714__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_714__vwidth_minus_one_byte_offset 	216	/* 0x000000d8 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_714__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_714__vheight_minus_one_byte_offset 	218	/* 0x000000da */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_714__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_714__drotate_bit_offset 	1798	/* 0x00000706 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_714__hflip_bit_offset 	1799	/* 0x00000707 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_714__vflip_bit_offset 	1800	/* 0x00000708 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_714__dflip_bit_offset 	1801	/* 0x00000709 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_714__pflip_bit_offset 	1802	/* 0x0000070a */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_714__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_714__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_714__D 	8
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_714__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_714__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_714__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_714__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_714__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_714__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_714__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_714__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_714__vmem_buffer_num_bytes 	20480
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_714__dram_size_num_bytes 	24320
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_714__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_714__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_714__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_714__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_714__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_714__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_714__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_714__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_714__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_714__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_5 HMB_input __pvcn_653_ */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_653__cnngen_demangled_name 	"__pvcn_653_"
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_653__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_653__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_653__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_653__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_653__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_653__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_653__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_653__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_653__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_653__byte_offset 	240	/* 0x000000f0 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_653__vbase_byte_offset 	244	/* 0x000000f4 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_653__dbase_byte_offset 	268	/* 0x0000010c */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_653__dpitchm1_byte_offset 	272	/* 0x00000110 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_653__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_653__vwidth_minus_one_byte_offset 	256	/* 0x00000100 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_653__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_653__vheight_minus_one_byte_offset 	258	/* 0x00000102 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_653__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_653__drotate_bit_offset 	2118	/* 0x00000846 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_653__hflip_bit_offset 	2119	/* 0x00000847 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_653__vflip_bit_offset 	2120	/* 0x00000848 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_653__dflip_bit_offset 	2121	/* 0x00000849 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_653__pflip_bit_offset 	2122	/* 0x0000084a */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_653__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_653__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_653__D 	7
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_653__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_653__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_653__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_653__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_653__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_653__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_653__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_653__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_653__vmem_buffer_num_bytes 	17920
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_653__dram_size_num_bytes 	21280
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_653__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_653__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_653__expoffset 	13
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_653__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_653__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_653__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_653__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_653__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_653__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_653__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_5 HMB_input __pvcn_664_ */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_664__cnngen_demangled_name 	"__pvcn_664_"
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_664__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_664__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_664__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_664__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_664__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_664__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_664__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_664__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_664__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_664__byte_offset 	280	/* 0x00000118 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_664__vbase_byte_offset 	284	/* 0x0000011c */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_664__dbase_byte_offset 	308	/* 0x00000134 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_664__dpitchm1_byte_offset 	312	/* 0x00000138 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_664__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_664__vwidth_minus_one_byte_offset 	296	/* 0x00000128 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_664__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_664__vheight_minus_one_byte_offset 	298	/* 0x0000012a */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_664__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_664__drotate_bit_offset 	2438	/* 0x00000986 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_664__hflip_bit_offset 	2439	/* 0x00000987 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_664__vflip_bit_offset 	2440	/* 0x00000988 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_664__dflip_bit_offset 	2441	/* 0x00000989 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_664__pflip_bit_offset 	2442	/* 0x0000098a */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_664__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_664__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_664__D 	8
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_664__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_664__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_664__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_664__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_664__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_664__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_664__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_664__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_664__vmem_buffer_num_bytes 	20480
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_664__dram_size_num_bytes 	24320
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_664__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_664__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_664__expoffset 	13
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_664__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_664__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_664__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_664__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_664__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_664__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_664__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_5 HMB_input __pvcn_609_ */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_609__cnngen_demangled_name 	"__pvcn_609_"
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_609__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_609__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_609__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_609__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_609__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_609__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_609__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_609__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_609__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_609__byte_offset 	320	/* 0x00000140 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_609__vbase_byte_offset 	324	/* 0x00000144 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_609__dbase_byte_offset 	348	/* 0x0000015c */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_609__dpitchm1_byte_offset 	352	/* 0x00000160 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_609__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_609__vwidth_minus_one_byte_offset 	336	/* 0x00000150 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_609__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_609__vheight_minus_one_byte_offset 	338	/* 0x00000152 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_609__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_609__drotate_bit_offset 	2758	/* 0x00000ac6 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_609__hflip_bit_offset 	2759	/* 0x00000ac7 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_609__vflip_bit_offset 	2760	/* 0x00000ac8 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_609__dflip_bit_offset 	2761	/* 0x00000ac9 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_609__pflip_bit_offset 	2762	/* 0x00000aca */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_609__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_609__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_609__D 	9
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_609__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_609__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_609__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_609__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_609__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_609__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_609__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_609__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_609__vmem_buffer_num_bytes 	23040
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_609__dram_size_num_bytes 	27360
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_609__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_609__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_609__expoffset 	13
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_609__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_609__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_609__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_609__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_609__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_609__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_609__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_5 HMB_input __pvcn_631_ */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_631__cnngen_demangled_name 	"__pvcn_631_"
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_631__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_631__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_631__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_631__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_631__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_631__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_631__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_631__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_631__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_631__byte_offset 	360	/* 0x00000168 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_631__vbase_byte_offset 	364	/* 0x0000016c */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_631__dbase_byte_offset 	388	/* 0x00000184 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_631__dpitchm1_byte_offset 	392	/* 0x00000188 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_631__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_631__vwidth_minus_one_byte_offset 	376	/* 0x00000178 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_631__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_631__vheight_minus_one_byte_offset 	378	/* 0x0000017a */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_631__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_631__drotate_bit_offset 	3078	/* 0x00000c06 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_631__hflip_bit_offset 	3079	/* 0x00000c07 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_631__vflip_bit_offset 	3080	/* 0x00000c08 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_631__dflip_bit_offset 	3081	/* 0x00000c09 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_631__pflip_bit_offset 	3082	/* 0x00000c0a */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_631__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_631__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_631__D 	8
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_631__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_631__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_631__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_631__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_631__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_631__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_631__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_631__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_631__vmem_buffer_num_bytes 	20480
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_631__dram_size_num_bytes 	24320
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_631__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_631__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_631__expoffset 	13
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_631__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_631__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_631__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_631__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_631__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_631__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_631__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_5 HMB_input __pvcn_587_ */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_587__cnngen_demangled_name 	"__pvcn_587_"
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_587__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_587__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_587__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_587__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_587__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_587__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_587__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_587__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_587__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_587__byte_offset 	400	/* 0x00000190 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_587__vbase_byte_offset 	404	/* 0x00000194 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_587__dbase_byte_offset 	428	/* 0x000001ac */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_587__dpitchm1_byte_offset 	432	/* 0x000001b0 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_587__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_587__vwidth_minus_one_byte_offset 	416	/* 0x000001a0 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_587__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_587__vheight_minus_one_byte_offset 	418	/* 0x000001a2 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_587__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_587__drotate_bit_offset 	3398	/* 0x00000d46 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_587__hflip_bit_offset 	3399	/* 0x00000d47 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_587__vflip_bit_offset 	3400	/* 0x00000d48 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_587__dflip_bit_offset 	3401	/* 0x00000d49 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_587__pflip_bit_offset 	3402	/* 0x00000d4a */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_587__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_587__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_587__D 	7
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_587__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_587__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_587__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_587__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_587__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_587__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_587__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_587__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_587__vmem_buffer_num_bytes 	17920
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_587__dram_size_num_bytes 	21280
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_587__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_587__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_587__expoffset 	13
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_587__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_587__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_587__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_587__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_587__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_587__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_587__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_5 HMB_input __pvcn_642_ */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_642__cnngen_demangled_name 	"__pvcn_642_"
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_642__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_642__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_642__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_642__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_642__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_642__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_642__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_642__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_642__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_642__byte_offset 	440	/* 0x000001b8 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_642__vbase_byte_offset 	444	/* 0x000001bc */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_642__dbase_byte_offset 	468	/* 0x000001d4 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_642__dpitchm1_byte_offset 	472	/* 0x000001d8 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_642__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_642__vwidth_minus_one_byte_offset 	456	/* 0x000001c8 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_642__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_642__vheight_minus_one_byte_offset 	458	/* 0x000001ca */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_642__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_642__drotate_bit_offset 	3718	/* 0x00000e86 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_642__hflip_bit_offset 	3719	/* 0x00000e87 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_642__vflip_bit_offset 	3720	/* 0x00000e88 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_642__dflip_bit_offset 	3721	/* 0x00000e89 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_642__pflip_bit_offset 	3722	/* 0x00000e8a */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_642__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_642__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_642__D 	7
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_642__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_642__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_642__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_642__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_642__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_642__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_642__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_642__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_642__vmem_buffer_num_bytes 	17920
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_642__dram_size_num_bytes 	21280
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_642__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_642__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_642__expoffset 	13
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_642__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_642__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_642__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_642__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_642__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_642__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_642__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_5 HMB_input __pvcn_598_ */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_598__cnngen_demangled_name 	"__pvcn_598_"
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_598__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_598__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_598__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_598__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_598__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_598__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_598__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_598__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_598__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_598__byte_offset 	480	/* 0x000001e0 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_598__vbase_byte_offset 	484	/* 0x000001e4 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_598__dbase_byte_offset 	508	/* 0x000001fc */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_598__dpitchm1_byte_offset 	512	/* 0x00000200 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_598__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_598__vwidth_minus_one_byte_offset 	496	/* 0x000001f0 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_598__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_598__vheight_minus_one_byte_offset 	498	/* 0x000001f2 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_598__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_598__drotate_bit_offset 	4038	/* 0x00000fc6 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_598__hflip_bit_offset 	4039	/* 0x00000fc7 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_598__vflip_bit_offset 	4040	/* 0x00000fc8 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_598__dflip_bit_offset 	4041	/* 0x00000fc9 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_598__pflip_bit_offset 	4042	/* 0x00000fca */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_598__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_598__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_598__D 	8
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_598__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_598__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_598__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_598__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_598__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_598__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_598__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_598__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_598__vmem_buffer_num_bytes 	20480
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_598__dram_size_num_bytes 	24320
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_598__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_598__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_598__expoffset 	13
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_598__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_598__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_598__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_598__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_598__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_598__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_598__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_5 HMB_input __pvcn_565_ */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_565__cnngen_demangled_name 	"__pvcn_565_"
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_565__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_565__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_565__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_565__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_565__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_565__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_565__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_565__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_565__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_565__byte_offset 	520	/* 0x00000208 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_565__vbase_byte_offset 	524	/* 0x0000020c */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_565__dbase_byte_offset 	548	/* 0x00000224 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_565__dpitchm1_byte_offset 	552	/* 0x00000228 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_565__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_565__vwidth_minus_one_byte_offset 	536	/* 0x00000218 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_565__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_565__vheight_minus_one_byte_offset 	538	/* 0x0000021a */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_565__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_565__drotate_bit_offset 	4358	/* 0x00001106 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_565__hflip_bit_offset 	4359	/* 0x00001107 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_565__vflip_bit_offset 	4360	/* 0x00001108 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_565__dflip_bit_offset 	4361	/* 0x00001109 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_565__pflip_bit_offset 	4362	/* 0x0000110a */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_565__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_565__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_565__D 	7
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_565__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_565__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_565__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_565__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_565__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_565__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_565__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_565__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_565__vmem_buffer_num_bytes 	17920
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_565__dram_size_num_bytes 	21280
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_565__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_565__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_565__expoffset 	13
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_565__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_565__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_565__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_565__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_565__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_565__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_565__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_5 HMB_input __pvcn_576_ */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_576__cnngen_demangled_name 	"__pvcn_576_"
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_576__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_576__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_576__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_576__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_576__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_576__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_576__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_576__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_576__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_576__byte_offset 	560	/* 0x00000230 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_576__vbase_byte_offset 	564	/* 0x00000234 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_576__dbase_byte_offset 	588	/* 0x0000024c */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_576__dpitchm1_byte_offset 	592	/* 0x00000250 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_576__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_576__vwidth_minus_one_byte_offset 	576	/* 0x00000240 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_576__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_576__vheight_minus_one_byte_offset 	578	/* 0x00000242 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_576__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_576__drotate_bit_offset 	4678	/* 0x00001246 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_576__hflip_bit_offset 	4679	/* 0x00001247 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_576__vflip_bit_offset 	4680	/* 0x00001248 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_576__dflip_bit_offset 	4681	/* 0x00001249 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_576__pflip_bit_offset 	4682	/* 0x0000124a */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_576__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_576__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_576__D 	8
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_576__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_576__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_576__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_576__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_576__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_576__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_576__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_576__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_576__vmem_buffer_num_bytes 	20480
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_576__dram_size_num_bytes 	24320
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_576__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_576__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_576__expoffset 	13
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_576__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_576__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_576__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_576__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_576__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_576__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_576__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_5 HMB_input __pvcn_554_ */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_554__cnngen_demangled_name 	"__pvcn_554_"
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_554__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_554__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_554__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_554__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_554__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_554__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_554__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_554__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_554__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_554__byte_offset 	600	/* 0x00000258 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_554__vbase_byte_offset 	604	/* 0x0000025c */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_554__dbase_byte_offset 	628	/* 0x00000274 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_554__dpitchm1_byte_offset 	632	/* 0x00000278 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_554__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_554__vwidth_minus_one_byte_offset 	616	/* 0x00000268 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_554__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_554__vheight_minus_one_byte_offset 	618	/* 0x0000026a */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_554__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_554__drotate_bit_offset 	4998	/* 0x00001386 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_554__hflip_bit_offset 	4999	/* 0x00001387 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_554__vflip_bit_offset 	5000	/* 0x00001388 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_554__dflip_bit_offset 	5001	/* 0x00001389 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_554__pflip_bit_offset 	5002	/* 0x0000138a */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_554__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_554__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_554__D 	7
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_554__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_554__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_554__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_554__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_554__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_554__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_554__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_554__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_554__vmem_buffer_num_bytes 	17920
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_554__dram_size_num_bytes 	21280
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_554__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_554__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_554__expoffset 	13
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_554__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_554__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_554__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_554__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_554__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_554__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_554__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_5 HMB_input __pvcn_703_ */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_703__cnngen_demangled_name 	"__pvcn_703_"
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_703__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_703__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_703__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_703__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_703__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_703__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_703__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_703__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_703__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_703__byte_offset 	640	/* 0x00000280 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_703__vbase_byte_offset 	644	/* 0x00000284 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_703__dbase_byte_offset 	668	/* 0x0000029c */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_703__dpitchm1_byte_offset 	672	/* 0x000002a0 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_703__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_703__vwidth_minus_one_byte_offset 	656	/* 0x00000290 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_703__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_703__vheight_minus_one_byte_offset 	658	/* 0x00000292 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_703__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_703__drotate_bit_offset 	5318	/* 0x000014c6 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_703__hflip_bit_offset 	5319	/* 0x000014c7 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_703__vflip_bit_offset 	5320	/* 0x000014c8 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_703__dflip_bit_offset 	5321	/* 0x000014c9 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_703__pflip_bit_offset 	5322	/* 0x000014ca */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_703__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_703__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_703__D 	7
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_703__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_703__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_703__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_703__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_703__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_703__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_703__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_703__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_703__vmem_buffer_num_bytes 	17920
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_703__dram_size_num_bytes 	21280
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_703__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_703__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_703__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_703__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_703__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_703__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_703__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_703__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_703__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_703__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_5 HMB_output __pvcn_719_ */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_719__cnngen_demangled_name 	"__pvcn_719_"
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_719__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_719__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_719__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_719__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_719__byte_offset 	760	/* 0x000002f8 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_719__vbase_byte_offset 	764	/* 0x000002fc */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_719__dbase_byte_offset 	788	/* 0x00000314 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_719__dpitchm1_byte_offset 	792	/* 0x00000318 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_719__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_719__vwidth_minus_one_byte_offset 	776	/* 0x00000308 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_719__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_719__vheight_minus_one_byte_offset 	778	/* 0x0000030a */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_719__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_719__drotate_bit_offset 	6278	/* 0x00001886 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_719__hflip_bit_offset 	6279	/* 0x00001887 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_719__vflip_bit_offset 	6280	/* 0x00001888 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_719__dflip_bit_offset 	6281	/* 0x00001889 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_719__pflip_bit_offset 	6282	/* 0x0000188a */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_719__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_719__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_719__D 	6
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_719__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_719__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_719__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_719__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_719__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_719__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_719__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_719__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_719__vmem_buffer_num_bytes 	11520
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_719__dram_size_num_bytes 	18240
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_719__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_719__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_719__expoffset 	13
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_719__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_719__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_719__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_719__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_719__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_719__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_719__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_5 HMB_output __pvcn_755_ */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_755__cnngen_demangled_name 	"__pvcn_755_"
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_755__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_755__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_755__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_755__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_755__byte_offset 	1168	/* 0x00000490 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_755__vbase_byte_offset 	1172	/* 0x00000494 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_755__dbase_byte_offset 	1196	/* 0x000004ac */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_755__dpitchm1_byte_offset 	1200	/* 0x000004b0 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_755__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_755__vwidth_minus_one_byte_offset 	1184	/* 0x000004a0 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_755__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_755__vheight_minus_one_byte_offset 	1186	/* 0x000004a2 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_755__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_755__drotate_bit_offset 	9542	/* 0x00002546 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_755__hflip_bit_offset 	9543	/* 0x00002547 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_755__vflip_bit_offset 	9544	/* 0x00002548 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_755__dflip_bit_offset 	9545	/* 0x00002549 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_755__pflip_bit_offset 	9546	/* 0x0000254a */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_755__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_755__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_755__D 	30
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_755__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_755__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_755__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_755__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_755__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_755__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_755__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_755__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_755__vmem_buffer_num_bytes 	38400
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_755__dram_size_num_bytes 	91200
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_755__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_755__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_755__expoffset 	13
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_755__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_755__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_755__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_755__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_755__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_755__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_755__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_5 HMB_output __pvcn_756_ */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_756__cnngen_demangled_name 	"__pvcn_756_"
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_756__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_756__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_756__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_756__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_756__byte_offset 	1216	/* 0x000004c0 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_756__vbase_byte_offset 	1220	/* 0x000004c4 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_756__dbase_byte_offset 	1244	/* 0x000004dc */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_756__dpitchm1_byte_offset 	1248	/* 0x000004e0 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_756__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_756__vwidth_minus_one_byte_offset 	1232	/* 0x000004d0 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_756__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_756__vheight_minus_one_byte_offset 	1234	/* 0x000004d2 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_756__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_756__drotate_bit_offset 	9926	/* 0x000026c6 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_756__hflip_bit_offset 	9927	/* 0x000026c7 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_756__vflip_bit_offset 	9928	/* 0x000026c8 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_756__dflip_bit_offset 	9929	/* 0x000026c9 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_756__pflip_bit_offset 	9930	/* 0x000026ca */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_756__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_756__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_756__D 	31
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_756__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_756__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_756__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_756__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_756__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_756__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_756__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_756__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_756__vmem_buffer_num_bytes 	39680
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_756__dram_size_num_bytes 	94240
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_756__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_756__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_756__expoffset 	13
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_756__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_756__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_756__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_756__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_756__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_756__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_756__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_5 HMB_output __pvcn_757_ */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_757__cnngen_demangled_name 	"__pvcn_757_"
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_757__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_757__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_757__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_757__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_757__byte_offset 	1264	/* 0x000004f0 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_757__vbase_byte_offset 	1268	/* 0x000004f4 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_757__dbase_byte_offset 	1292	/* 0x0000050c */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_757__dpitchm1_byte_offset 	1296	/* 0x00000510 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_757__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_757__vwidth_minus_one_byte_offset 	1280	/* 0x00000500 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_757__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_757__vheight_minus_one_byte_offset 	1282	/* 0x00000502 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_757__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_757__drotate_bit_offset 	10310	/* 0x00002846 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_757__hflip_bit_offset 	10311	/* 0x00002847 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_757__vflip_bit_offset 	10312	/* 0x00002848 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_757__dflip_bit_offset 	10313	/* 0x00002849 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_757__pflip_bit_offset 	10314	/* 0x0000284a */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_757__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_757__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_757__D 	61
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_757__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_757__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_757__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_757__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_757__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_757__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_757__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_757__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_757__vmem_buffer_num_bytes 	78080
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_757__dram_size_num_bytes 	185440
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_757__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_757__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_757__expoffset 	13
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_757__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_757__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_757__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_757__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_757__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_757__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_757__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_5 SMB_output __vas_created_terminate__v0 */
#define mnet_ssd_adas_flex_pic_prim_split_5___vas_created_terminate__v0_cnngen_demangled_name 	"__vas_created_terminate__v0"
#define mnet_ssd_adas_flex_pic_prim_split_5___vas_created_terminate__v0_is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___vas_created_terminate__v0_is_bitvector 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___vas_created_terminate__v0_is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___vas_created_terminate__v0_is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___vas_created_terminate__v0_byte_offset 	1304	/* 0x00000518 */
#define mnet_ssd_adas_flex_pic_prim_split_5___vas_created_terminate__v0_vbase_byte_offset 	1308	/* 0x0000051c */
#define mnet_ssd_adas_flex_pic_prim_split_5___vas_created_terminate__v0_W 	256
#define mnet_ssd_adas_flex_pic_prim_split_5___vas_created_terminate__v0_H 	3
#define mnet_ssd_adas_flex_pic_prim_split_5___vas_created_terminate__v0_D 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___vas_created_terminate__v0_P 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___vas_created_terminate__v0_data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___vas_created_terminate__v0_denable 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___vas_created_terminate__v0_dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___vas_created_terminate__v0_vp_interleave_mode 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___vas_created_terminate__v0_tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___vas_created_terminate__v0_tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___vas_created_terminate__v0_vmem_buffer_num_bytes 	96


#endif /* mnet_ssd_adas_flex_pic_prim_split_5_H */
