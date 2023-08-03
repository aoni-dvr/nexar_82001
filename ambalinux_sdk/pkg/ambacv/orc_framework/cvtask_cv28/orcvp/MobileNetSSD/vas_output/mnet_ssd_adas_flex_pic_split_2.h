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
#ifndef mnet_ssd_adas_flex_pic_prim_split_2_H
#define mnet_ssd_adas_flex_pic_prim_split_2_H

#define vas_gen_header_version                  	7

/* dag name */
#define mnet_ssd_adas_flex_pic_prim_split_2_DAG_NAME 	"mnet_ssd_adas_flex_pic_prim_split_2"

/* VDG file info */
#define mnet_ssd_adas_flex_pic_prim_split_2_vdg_name 	"mnet_ssd_adas_flex_pic_split_2.vdg"

/* VDG DAG memory info */
#define mnet_ssd_adas_flex_pic_prim_split_2_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_2_byte_size 	1844	/* 0x00000734 */
#define mnet_ssd_adas_flex_pic_prim_split_2_preferred_dram_xfer_size 	1

/* mnet_ssd_adas_flex_pic_prim_split_2 VMEM info */
#define mnet_ssd_adas_flex_pic_prim_split_2_use_ping_pong_vmem 	0
#define mnet_ssd_adas_flex_pic_prim_split_2_VMEM_start 	4096	/* 0x00001000 */
#define mnet_ssd_adas_flex_pic_prim_split_2_VMEM_end 	510072	/* 0x0007c878 */
#define mnet_ssd_adas_flex_pic_prim_split_2_image_start 	499296	/* 0x00079e60 */
#define mnet_ssd_adas_flex_pic_prim_split_2_image_size 	12620	/* 0x0000314c */
#define mnet_ssd_adas_flex_pic_prim_split_2_dagbin_start 	510072	/* 0x0007c878 */

/* mnet_ssd_adas_flex_pic_prim_split_2 DAG info */
#define mnet_ssd_adas_flex_pic_prim_split_2_estimated_cycles 	2145050
#define mnet_ssd_adas_flex_pic_prim_split_2_ddi_byte_size 	0	/* 0x00000000 */

/*******************************************************************************
 * DAG byte offset info for HMB/SMB/VCB and their fields
 * in DAG 'mnet_ssd_adas_flex_pic_prim_split_2' in source file 'pre_split2_mnet_ssd_adas_flex_pic_split_0.vas'
 ******************************************************************************/
/* mnet_ssd_adas_flex_pic_prim_split_2 HMB_input __pvcn_387_ */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_387__cnngen_demangled_name 	"__pvcn_387_"
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_387__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_387__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_387__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_387__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_387__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_387__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_387__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_387__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_387__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_387__byte_offset 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_387__vbase_byte_offset 	4	/* 0x00000004 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_387__dbase_byte_offset 	28	/* 0x0000001c */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_387__dpitchm1_byte_offset 	32	/* 0x00000020 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_387__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_387__vwidth_minus_one_byte_offset 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_387__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_387__vheight_minus_one_byte_offset 	18	/* 0x00000012 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_387__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_387__drotate_bit_offset 	198	/* 0x000000c6 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_387__hflip_bit_offset 	199	/* 0x000000c7 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_387__vflip_bit_offset 	200	/* 0x000000c8 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_387__dflip_bit_offset 	201	/* 0x000000c9 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_387__pflip_bit_offset 	202	/* 0x000000ca */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_387__W 	75
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_387__H 	75
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_387__D 	8
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_387__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_387__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_387__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_387__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_387__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_387__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_387__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_387__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_387__vmem_buffer_num_bytes 	20480
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_387__dram_size_num_bytes 	96000
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_387__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_387__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_387__expoffset 	13
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_387__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_387__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_387__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_387__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_387__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_387__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_387__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_2 HMB_input __pvcn_398_ */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_398__cnngen_demangled_name 	"__pvcn_398_"
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_398__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_398__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_398__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_398__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_398__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_398__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_398__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_398__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_398__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_398__byte_offset 	40	/* 0x00000028 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_398__vbase_byte_offset 	44	/* 0x0000002c */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_398__dbase_byte_offset 	68	/* 0x00000044 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_398__dpitchm1_byte_offset 	72	/* 0x00000048 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_398__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_398__vwidth_minus_one_byte_offset 	56	/* 0x00000038 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_398__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_398__vheight_minus_one_byte_offset 	58	/* 0x0000003a */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_398__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_398__drotate_bit_offset 	518	/* 0x00000206 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_398__hflip_bit_offset 	519	/* 0x00000207 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_398__vflip_bit_offset 	520	/* 0x00000208 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_398__dflip_bit_offset 	521	/* 0x00000209 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_398__pflip_bit_offset 	522	/* 0x0000020a */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_398__W 	75
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_398__H 	75
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_398__D 	8
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_398__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_398__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_398__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_398__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_398__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_398__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_398__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_398__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_398__vmem_buffer_num_bytes 	20480
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_398__dram_size_num_bytes 	96000
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_398__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_398__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_398__expoffset 	13
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_398__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_398__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_398__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_398__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_398__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_398__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_398__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_2 HMB_input __pvcn_409_ */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_409__cnngen_demangled_name 	"__pvcn_409_"
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_409__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_409__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_409__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_409__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_409__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_409__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_409__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_409__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_409__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_409__byte_offset 	80	/* 0x00000050 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_409__vbase_byte_offset 	84	/* 0x00000054 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_409__dbase_byte_offset 	108	/* 0x0000006c */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_409__dpitchm1_byte_offset 	112	/* 0x00000070 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_409__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_409__vwidth_minus_one_byte_offset 	96	/* 0x00000060 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_409__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_409__vheight_minus_one_byte_offset 	98	/* 0x00000062 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_409__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_409__drotate_bit_offset 	838	/* 0x00000346 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_409__hflip_bit_offset 	839	/* 0x00000347 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_409__vflip_bit_offset 	840	/* 0x00000348 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_409__dflip_bit_offset 	841	/* 0x00000349 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_409__pflip_bit_offset 	842	/* 0x0000034a */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_409__W 	75
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_409__H 	75
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_409__D 	6
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_409__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_409__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_409__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_409__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_409__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_409__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_409__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_409__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_409__vmem_buffer_num_bytes 	15360
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_409__dram_size_num_bytes 	72000
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_409__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_409__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_409__expoffset 	13
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_409__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_409__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_409__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_409__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_409__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_409__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_409__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_2 HMB_input __pvcn_376_ */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_376__cnngen_demangled_name 	"__pvcn_376_"
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_376__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_376__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_376__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_376__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_376__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_376__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_376__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_376__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_376__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_376__byte_offset 	120	/* 0x00000078 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_376__vbase_byte_offset 	124	/* 0x0000007c */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_376__dbase_byte_offset 	148	/* 0x00000094 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_376__dpitchm1_byte_offset 	152	/* 0x00000098 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_376__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_376__vwidth_minus_one_byte_offset 	136	/* 0x00000088 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_376__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_376__vheight_minus_one_byte_offset 	138	/* 0x0000008a */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_376__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_376__drotate_bit_offset 	1158	/* 0x00000486 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_376__hflip_bit_offset 	1159	/* 0x00000487 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_376__vflip_bit_offset 	1160	/* 0x00000488 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_376__dflip_bit_offset 	1161	/* 0x00000489 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_376__pflip_bit_offset 	1162	/* 0x0000048a */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_376__W 	75
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_376__H 	75
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_376__D 	7
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_376__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_376__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_376__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_376__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_376__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_376__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_376__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_376__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_376__vmem_buffer_num_bytes 	17920
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_376__dram_size_num_bytes 	84000
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_376__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_376__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_376__expoffset 	13
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_376__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_376__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_376__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_376__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_376__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_376__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_376__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_2 HMB_input __pvcn_354_ */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_354__cnngen_demangled_name 	"__pvcn_354_"
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_354__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_354__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_354__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_354__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_354__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_354__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_354__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_354__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_354__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_354__byte_offset 	160	/* 0x000000a0 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_354__vbase_byte_offset 	164	/* 0x000000a4 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_354__dbase_byte_offset 	188	/* 0x000000bc */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_354__dpitchm1_byte_offset 	192	/* 0x000000c0 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_354__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_354__vwidth_minus_one_byte_offset 	176	/* 0x000000b0 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_354__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_354__vheight_minus_one_byte_offset 	178	/* 0x000000b2 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_354__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_354__drotate_bit_offset 	1478	/* 0x000005c6 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_354__hflip_bit_offset 	1479	/* 0x000005c7 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_354__vflip_bit_offset 	1480	/* 0x000005c8 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_354__dflip_bit_offset 	1481	/* 0x000005c9 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_354__pflip_bit_offset 	1482	/* 0x000005ca */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_354__W 	75
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_354__H 	75
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_354__D 	11
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_354__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_354__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_354__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_354__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_354__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_354__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_354__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_354__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_354__vmem_buffer_num_bytes 	28160
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_354__dram_size_num_bytes 	132000
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_354__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_354__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_354__expoffset 	13
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_354__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_354__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_354__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_354__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_354__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_354__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_354__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_2 HMB_input __pvcn_365_ */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_365__cnngen_demangled_name 	"__pvcn_365_"
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_365__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_365__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_365__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_365__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_365__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_365__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_365__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_365__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_365__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_365__byte_offset 	200	/* 0x000000c8 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_365__vbase_byte_offset 	204	/* 0x000000cc */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_365__dbase_byte_offset 	228	/* 0x000000e4 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_365__dpitchm1_byte_offset 	232	/* 0x000000e8 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_365__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_365__vwidth_minus_one_byte_offset 	216	/* 0x000000d8 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_365__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_365__vheight_minus_one_byte_offset 	218	/* 0x000000da */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_365__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_365__drotate_bit_offset 	1798	/* 0x00000706 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_365__hflip_bit_offset 	1799	/* 0x00000707 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_365__vflip_bit_offset 	1800	/* 0x00000708 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_365__dflip_bit_offset 	1801	/* 0x00000709 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_365__pflip_bit_offset 	1802	/* 0x0000070a */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_365__W 	75
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_365__H 	75
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_365__D 	9
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_365__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_365__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_365__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_365__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_365__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_365__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_365__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_365__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_365__vmem_buffer_num_bytes 	11520
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_365__dram_size_num_bytes 	108000
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_365__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_365__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_365__expoffset 	13
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_365__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_365__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_365__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_365__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_365__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_365__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_365__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_2 HMB_input __pvcn_332_ */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_332__cnngen_demangled_name 	"__pvcn_332_"
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_332__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_332__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_332__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_332__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_332__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_332__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_332__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_332__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_332__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_332__byte_offset 	240	/* 0x000000f0 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_332__vbase_byte_offset 	244	/* 0x000000f4 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_332__dbase_byte_offset 	268	/* 0x0000010c */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_332__dpitchm1_byte_offset 	272	/* 0x00000110 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_332__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_332__vwidth_minus_one_byte_offset 	256	/* 0x00000100 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_332__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_332__vheight_minus_one_byte_offset 	258	/* 0x00000102 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_332__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_332__drotate_bit_offset 	2118	/* 0x00000846 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_332__hflip_bit_offset 	2119	/* 0x00000847 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_332__vflip_bit_offset 	2120	/* 0x00000848 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_332__dflip_bit_offset 	2121	/* 0x00000849 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_332__pflip_bit_offset 	2122	/* 0x0000084a */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_332__W 	75
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_332__H 	75
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_332__D 	8
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_332__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_332__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_332__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_332__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_332__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_332__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_332__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_332__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_332__vmem_buffer_num_bytes 	10240
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_332__dram_size_num_bytes 	96000
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_332__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_332__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_332__expoffset 	13
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_332__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_332__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_332__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_332__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_332__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_332__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_332__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_2 HMB_input __pvcn_343_ */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_343__cnngen_demangled_name 	"__pvcn_343_"
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_343__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_343__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_343__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_343__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_343__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_343__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_343__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_343__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_343__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_343__byte_offset 	276	/* 0x00000114 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_343__vbase_byte_offset 	280	/* 0x00000118 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_343__dbase_byte_offset 	304	/* 0x00000130 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_343__dpitchm1_byte_offset 	308	/* 0x00000134 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_343__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_343__vwidth_minus_one_byte_offset 	292	/* 0x00000124 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_343__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_343__vheight_minus_one_byte_offset 	294	/* 0x00000126 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_343__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_343__drotate_bit_offset 	2406	/* 0x00000966 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_343__hflip_bit_offset 	2407	/* 0x00000967 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_343__vflip_bit_offset 	2408	/* 0x00000968 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_343__dflip_bit_offset 	2409	/* 0x00000969 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_343__pflip_bit_offset 	2410	/* 0x0000096a */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_343__W 	75
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_343__H 	75
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_343__D 	7
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_343__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_343__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_343__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_343__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_343__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_343__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_343__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_343__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_343__vmem_buffer_num_bytes 	8960
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_343__dram_size_num_bytes 	84000
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_343__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_343__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_343__expoffset 	13
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_343__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_343__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_343__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_343__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_343__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_343__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_343__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_2 SMB_input __pvcn_434_ */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_434__cnngen_demangled_name 	"__pvcn_434_"
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_434__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_434__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_434__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_434__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_434__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_434__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_434__byte_offset 	324	/* 0x00000144 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_434__vbase_byte_offset 	328	/* 0x00000148 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_434__dbase_byte_offset 	352	/* 0x00000160 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_434__dpitchm1_byte_offset 	356	/* 0x00000164 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_434__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_434__W 	7540
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_434__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_434__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_434__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_434__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_434__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_434__dpitch_num_bytes 	7552
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_434__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_434__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_434__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_434__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_434__vmem_buffer_num_bytes 	7540
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_434__dram_size_num_bytes 	7552

/* mnet_ssd_adas_flex_pic_prim_split_2 SMB_input __pvcn_435_ */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_435__cnngen_demangled_name 	"__pvcn_435_"
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_435__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_435__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_435__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_435__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_435__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_435__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_435__byte_offset 	364	/* 0x0000016c */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_435__vbase_byte_offset 	368	/* 0x00000170 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_435__dbase_byte_offset 	392	/* 0x00000188 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_435__dpitchm1_byte_offset 	396	/* 0x0000018c */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_435__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_435__W 	728
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_435__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_435__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_435__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_435__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_435__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_435__dpitch_num_bytes 	768
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_435__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_435__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_435__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_435__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_435__vmem_buffer_num_bytes 	728
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_435__dram_size_num_bytes 	768

/* mnet_ssd_adas_flex_pic_prim_split_2 SMB_input __pvcn_447_ */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_447__cnngen_demangled_name 	"__pvcn_447_"
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_447__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_447__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_447__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_447__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_447__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_447__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_447__byte_offset 	404	/* 0x00000194 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_447__vbase_byte_offset 	408	/* 0x00000198 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_447__dbase_byte_offset 	432	/* 0x000001b0 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_447__dpitchm1_byte_offset 	436	/* 0x000001b4 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_447__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_447__W 	212
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_447__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_447__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_447__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_447__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_447__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_447__dpitch_num_bytes 	256
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_447__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_447__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_447__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_447__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_447__vmem_buffer_num_bytes 	212
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_447__dram_size_num_bytes 	256

/* mnet_ssd_adas_flex_pic_prim_split_2 SMB_input __pvcn_448_ */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_448__cnngen_demangled_name 	"__pvcn_448_"
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_448__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_448__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_448__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_448__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_448__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_448__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_448__byte_offset 	444	/* 0x000001bc */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_448__vbase_byte_offset 	448	/* 0x000001c0 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_448__dbase_byte_offset 	472	/* 0x000001d8 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_448__dpitchm1_byte_offset 	476	/* 0x000001dc */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_448__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_448__W 	76
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_448__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_448__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_448__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_448__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_448__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_448__dpitch_num_bytes 	128
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_448__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_448__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_448__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_448__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_448__vmem_buffer_num_bytes 	76
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_448__dram_size_num_bytes 	128

/* mnet_ssd_adas_flex_pic_prim_split_2 SMB_input __pvcn_455_ */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_455__cnngen_demangled_name 	"__pvcn_455_"
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_455__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_455__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_455__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_455__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_455__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_455__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_455__byte_offset 	484	/* 0x000001e4 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_455__vbase_byte_offset 	488	/* 0x000001e8 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_455__dbase_byte_offset 	512	/* 0x00000200 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_455__dpitchm1_byte_offset 	516	/* 0x00000204 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_455__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_455__W 	208
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_455__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_455__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_455__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_455__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_455__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_455__dpitch_num_bytes 	256
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_455__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_455__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_455__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_455__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_455__vmem_buffer_num_bytes 	208
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_455__dram_size_num_bytes 	256

/* mnet_ssd_adas_flex_pic_prim_split_2 SMB_input __pvcn_456_ */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_456__cnngen_demangled_name 	"__pvcn_456_"
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_456__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_456__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_456__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_456__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_456__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_456__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_456__byte_offset 	524	/* 0x0000020c */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_456__vbase_byte_offset 	528	/* 0x00000210 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_456__dbase_byte_offset 	552	/* 0x00000228 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_456__dpitchm1_byte_offset 	556	/* 0x0000022c */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_456__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_456__W 	76
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_456__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_456__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_456__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_456__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_456__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_456__dpitch_num_bytes 	128
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_456__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_456__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_456__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_456__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_456__vmem_buffer_num_bytes 	76
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_456__dram_size_num_bytes 	128

/* mnet_ssd_adas_flex_pic_prim_split_2 SMB_input __pvcn_463_ */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_463__cnngen_demangled_name 	"__pvcn_463_"
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_463__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_463__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_463__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_463__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_463__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_463__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_463__byte_offset 	564	/* 0x00000234 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_463__vbase_byte_offset 	568	/* 0x00000238 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_463__dbase_byte_offset 	592	/* 0x00000250 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_463__dpitchm1_byte_offset 	596	/* 0x00000254 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_463__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_463__W 	192
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_463__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_463__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_463__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_463__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_463__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_463__dpitch_num_bytes 	192
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_463__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_463__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_463__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_463__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_463__vmem_buffer_num_bytes 	192
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_463__dram_size_num_bytes 	192

/* mnet_ssd_adas_flex_pic_prim_split_2 SMB_input __pvcn_464_ */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_464__cnngen_demangled_name 	"__pvcn_464_"
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_464__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_464__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_464__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_464__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_464__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_464__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_464__byte_offset 	604	/* 0x0000025c */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_464__vbase_byte_offset 	608	/* 0x00000260 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_464__dbase_byte_offset 	632	/* 0x00000278 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_464__dpitchm1_byte_offset 	636	/* 0x0000027c */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_464__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_464__W 	76
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_464__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_464__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_464__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_464__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_464__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_464__dpitch_num_bytes 	128
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_464__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_464__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_464__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_464__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_464__vmem_buffer_num_bytes 	76
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_464__dram_size_num_bytes 	128

/* mnet_ssd_adas_flex_pic_prim_split_2 SMB_input __pvcn_471_ */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_471__cnngen_demangled_name 	"__pvcn_471_"
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_471__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_471__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_471__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_471__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_471__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_471__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_471__byte_offset 	644	/* 0x00000284 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_471__vbase_byte_offset 	648	/* 0x00000288 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_471__dbase_byte_offset 	672	/* 0x000002a0 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_471__dpitchm1_byte_offset 	676	/* 0x000002a4 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_471__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_471__W 	204
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_471__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_471__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_471__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_471__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_471__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_471__dpitch_num_bytes 	256
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_471__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_471__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_471__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_471__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_471__vmem_buffer_num_bytes 	204
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_471__dram_size_num_bytes 	256

/* mnet_ssd_adas_flex_pic_prim_split_2 SMB_input __pvcn_472_ */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_472__cnngen_demangled_name 	"__pvcn_472_"
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_472__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_472__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_472__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_472__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_472__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_472__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_472__byte_offset 	684	/* 0x000002ac */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_472__vbase_byte_offset 	688	/* 0x000002b0 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_472__dbase_byte_offset 	712	/* 0x000002c8 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_472__dpitchm1_byte_offset 	716	/* 0x000002cc */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_472__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_472__W 	76
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_472__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_472__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_472__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_472__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_472__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_472__dpitch_num_bytes 	128
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_472__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_472__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_472__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_472__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_472__vmem_buffer_num_bytes 	76
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_472__dram_size_num_bytes 	128

/* mnet_ssd_adas_flex_pic_prim_split_2 SMB_input __pvcn_479_ */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_479__cnngen_demangled_name 	"__pvcn_479_"
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_479__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_479__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_479__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_479__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_479__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_479__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_479__byte_offset 	724	/* 0x000002d4 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_479__vbase_byte_offset 	728	/* 0x000002d8 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_479__dbase_byte_offset 	752	/* 0x000002f0 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_479__dpitchm1_byte_offset 	756	/* 0x000002f4 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_479__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_479__W 	212
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_479__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_479__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_479__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_479__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_479__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_479__dpitch_num_bytes 	256
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_479__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_479__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_479__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_479__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_479__vmem_buffer_num_bytes 	212
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_479__dram_size_num_bytes 	256

/* mnet_ssd_adas_flex_pic_prim_split_2 SMB_input __pvcn_480_ */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_480__cnngen_demangled_name 	"__pvcn_480_"
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_480__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_480__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_480__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_480__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_480__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_480__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_480__byte_offset 	764	/* 0x000002fc */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_480__vbase_byte_offset 	768	/* 0x00000300 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_480__dbase_byte_offset 	792	/* 0x00000318 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_480__dpitchm1_byte_offset 	796	/* 0x0000031c */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_480__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_480__W 	80
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_480__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_480__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_480__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_480__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_480__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_480__dpitch_num_bytes 	128
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_480__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_480__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_480__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_480__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_480__vmem_buffer_num_bytes 	80
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_480__dram_size_num_bytes 	128

/* mnet_ssd_adas_flex_pic_prim_split_2 SMB_input __pvcn_487_ */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_487__cnngen_demangled_name 	"__pvcn_487_"
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_487__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_487__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_487__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_487__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_487__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_487__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_487__byte_offset 	804	/* 0x00000324 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_487__vbase_byte_offset 	808	/* 0x00000328 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_487__dbase_byte_offset 	832	/* 0x00000340 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_487__dpitchm1_byte_offset 	836	/* 0x00000344 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_487__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_487__W 	228
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_487__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_487__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_487__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_487__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_487__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_487__dpitch_num_bytes 	256
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_487__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_487__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_487__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_487__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_487__vmem_buffer_num_bytes 	228
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_487__dram_size_num_bytes 	256

/* mnet_ssd_adas_flex_pic_prim_split_2 SMB_input __pvcn_488_ */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_488__cnngen_demangled_name 	"__pvcn_488_"
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_488__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_488__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_488__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_488__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_488__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_488__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_488__byte_offset 	844	/* 0x0000034c */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_488__vbase_byte_offset 	848	/* 0x00000350 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_488__dbase_byte_offset 	872	/* 0x00000368 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_488__dpitchm1_byte_offset 	876	/* 0x0000036c */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_488__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_488__W 	84
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_488__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_488__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_488__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_488__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_488__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_488__dpitch_num_bytes 	128
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_488__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_488__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_488__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_488__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_488__vmem_buffer_num_bytes 	84
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_488__dram_size_num_bytes 	128

/* mnet_ssd_adas_flex_pic_prim_split_2 SMB_input __pvcn_495_ */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_495__cnngen_demangled_name 	"__pvcn_495_"
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_495__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_495__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_495__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_495__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_495__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_495__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_495__byte_offset 	884	/* 0x00000374 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_495__vbase_byte_offset 	888	/* 0x00000378 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_495__dbase_byte_offset 	912	/* 0x00000390 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_495__dpitchm1_byte_offset 	916	/* 0x00000394 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_495__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_495__W 	196
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_495__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_495__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_495__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_495__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_495__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_495__dpitch_num_bytes 	256
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_495__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_495__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_495__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_495__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_495__vmem_buffer_num_bytes 	196
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_495__dram_size_num_bytes 	256

/* mnet_ssd_adas_flex_pic_prim_split_2 SMB_input __pvcn_496_ */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_496__cnngen_demangled_name 	"__pvcn_496_"
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_496__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_496__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_496__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_496__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_496__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_496__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_496__byte_offset 	924	/* 0x0000039c */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_496__vbase_byte_offset 	928	/* 0x000003a0 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_496__dbase_byte_offset 	952	/* 0x000003b8 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_496__dpitchm1_byte_offset 	956	/* 0x000003bc */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_496__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_496__W 	72
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_496__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_496__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_496__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_496__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_496__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_496__dpitch_num_bytes 	128
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_496__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_496__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_496__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_496__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_496__vmem_buffer_num_bytes 	72
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_496__dram_size_num_bytes 	128

/* mnet_ssd_adas_flex_pic_prim_split_2 SMB_input __pvcn_503_ */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_503__cnngen_demangled_name 	"__pvcn_503_"
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_503__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_503__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_503__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_503__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_503__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_503__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_503__byte_offset 	964	/* 0x000003c4 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_503__vbase_byte_offset 	968	/* 0x000003c8 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_503__dbase_byte_offset 	992	/* 0x000003e0 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_503__dpitchm1_byte_offset 	996	/* 0x000003e4 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_503__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_503__W 	192
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_503__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_503__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_503__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_503__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_503__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_503__dpitch_num_bytes 	192
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_503__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_503__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_503__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_503__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_503__vmem_buffer_num_bytes 	192
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_503__dram_size_num_bytes 	192

/* mnet_ssd_adas_flex_pic_prim_split_2 SMB_input __pvcn_504_ */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_504__cnngen_demangled_name 	"__pvcn_504_"
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_504__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_504__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_504__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_504__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_504__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_504__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_504__byte_offset 	1004	/* 0x000003ec */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_504__vbase_byte_offset 	1008	/* 0x000003f0 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_504__dbase_byte_offset 	1032	/* 0x00000408 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_504__dpitchm1_byte_offset 	1036	/* 0x0000040c */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_504__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_504__W 	76
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_504__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_504__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_504__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_504__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_504__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_504__dpitch_num_bytes 	128
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_504__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_504__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_504__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_504__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_504__vmem_buffer_num_bytes 	76
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_504__dram_size_num_bytes 	128

/* mnet_ssd_adas_flex_pic_prim_split_2 SMB_input __pvcn_511_ */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_511__cnngen_demangled_name 	"__pvcn_511_"
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_511__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_511__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_511__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_511__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_511__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_511__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_511__byte_offset 	1044	/* 0x00000414 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_511__vbase_byte_offset 	1048	/* 0x00000418 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_511__dbase_byte_offset 	1072	/* 0x00000430 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_511__dpitchm1_byte_offset 	1076	/* 0x00000434 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_511__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_511__W 	176
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_511__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_511__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_511__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_511__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_511__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_511__dpitch_num_bytes 	192
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_511__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_511__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_511__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_511__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_511__vmem_buffer_num_bytes 	176
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_511__dram_size_num_bytes 	192

/* mnet_ssd_adas_flex_pic_prim_split_2 SMB_input __pvcn_512_ */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_512__cnngen_demangled_name 	"__pvcn_512_"
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_512__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_512__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_512__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_512__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_512__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_512__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_512__byte_offset 	1084	/* 0x0000043c */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_512__vbase_byte_offset 	1088	/* 0x00000440 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_512__dbase_byte_offset 	1112	/* 0x00000458 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_512__dpitchm1_byte_offset 	1116	/* 0x0000045c */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_512__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_512__W 	72
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_512__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_512__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_512__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_512__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_512__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_512__dpitch_num_bytes 	128
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_512__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_512__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_512__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_512__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_512__vmem_buffer_num_bytes 	72
#define mnet_ssd_adas_flex_pic_prim_split_2___pvcn_512__dram_size_num_bytes 	128

/* mnet_ssd_adas_flex_pic_prim_split_2 VCB conv2_2__dw_____bn_conv2_2__dw__scale_mul___muli___17_____bn_conv2_2__dw__scale_mul_____scale___muli___337 */
#define mnet_ssd_adas_flex_pic_prim_split_2_conv2_2__dw_____bn_conv2_2__dw__scale_mul___muli___17_____bn_conv2_2__dw__scale_mul_____scale___muli___337_cnngen_demangled_name 	"conv2_2__dw_____bn_conv2_2__dw__scale_mul___muli___17_____bn_conv2_2__dw__scale_mul_____scale___muli___337"
#define mnet_ssd_adas_flex_pic_prim_split_2_conv2_2__dw_____bn_conv2_2__dw__scale_mul___muli___17_____bn_conv2_2__dw__scale_mul_____scale___muli___337_is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_2_conv2_2__dw_____bn_conv2_2__dw__scale_mul___muli___17_____bn_conv2_2__dw__scale_mul_____scale___muli___337_is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_2_conv2_2__dw_____bn_conv2_2__dw__scale_mul___muli___17_____bn_conv2_2__dw__scale_mul_____scale___muli___337_is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_2_conv2_2__dw_____bn_conv2_2__dw__scale_mul___muli___17_____bn_conv2_2__dw__scale_mul_____scale___muli___337_byte_offset 	1384	/* 0x00000568 */
#define mnet_ssd_adas_flex_pic_prim_split_2_conv2_2__dw_____bn_conv2_2__dw__scale_mul___muli___17_____bn_conv2_2__dw__scale_mul_____scale___muli___337_vbase_byte_offset 	1392	/* 0x00000570 */
#define mnet_ssd_adas_flex_pic_prim_split_2_conv2_2__dw_____bn_conv2_2__dw__scale_mul___muli___17_____bn_conv2_2__dw__scale_mul_____scale___muli___337_vmem_buffer_num_bytes 	65024

/* mnet_ssd_adas_flex_pic_prim_split_2 VCB conv2_2__sep_____bn_conv2_2__sep__scale_mul___muli___20_____bn_conv2_2__sep__scale_mul_____scale___relu2_2__sep */
#define mnet_ssd_adas_flex_pic_prim_split_2_conv2_2__sep_____bn_conv2_2__sep__scale_mul___muli___20_____bn_conv2_2__sep__scale_mul_____scale___relu2_2__sep_cnngen_demangled_name 	"conv2_2__sep_____bn_conv2_2__sep__scale_mul___muli___20_____bn_conv2_2__sep__scale_mul_____scale___relu2_2__sep"
#define mnet_ssd_adas_flex_pic_prim_split_2_conv2_2__sep_____bn_conv2_2__sep__scale_mul___muli___20_____bn_conv2_2__sep__scale_mul_____scale___relu2_2__sep_is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_2_conv2_2__sep_____bn_conv2_2__sep__scale_mul___muli___20_____bn_conv2_2__sep__scale_mul_____scale___relu2_2__sep_is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_2_conv2_2__sep_____bn_conv2_2__sep__scale_mul___muli___20_____bn_conv2_2__sep__scale_mul_____scale___relu2_2__sep_is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_2_conv2_2__sep_____bn_conv2_2__sep__scale_mul___muli___20_____bn_conv2_2__sep__scale_mul_____scale___relu2_2__sep_byte_offset 	1432	/* 0x00000598 */
#define mnet_ssd_adas_flex_pic_prim_split_2_conv2_2__sep_____bn_conv2_2__sep__scale_mul___muli___20_____bn_conv2_2__sep__scale_mul_____scale___relu2_2__sep_vbase_byte_offset 	1440	/* 0x000005a0 */
#define mnet_ssd_adas_flex_pic_prim_split_2_conv2_2__sep_____bn_conv2_2__sep__scale_mul___muli___20_____bn_conv2_2__sep__scale_mul_____scale___relu2_2__sep_vmem_buffer_num_bytes 	215040

/* mnet_ssd_adas_flex_pic_prim_split_2 HMB_output conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw */
#define mnet_ssd_adas_flex_pic_prim_split_2_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_cnngen_demangled_name 	"conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw"
#define mnet_ssd_adas_flex_pic_prim_split_2_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_2_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_2_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_2_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_2_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_byte_offset 	1476	/* 0x000005c4 */
#define mnet_ssd_adas_flex_pic_prim_split_2_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_vbase_byte_offset 	1480	/* 0x000005c8 */
#define mnet_ssd_adas_flex_pic_prim_split_2_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_dbase_byte_offset 	1504	/* 0x000005e0 */
#define mnet_ssd_adas_flex_pic_prim_split_2_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_dpitchm1_byte_offset 	1508	/* 0x000005e4 */
#define mnet_ssd_adas_flex_pic_prim_split_2_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_2_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_vwidth_minus_one_byte_offset 	1492	/* 0x000005d4 */
#define mnet_ssd_adas_flex_pic_prim_split_2_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_2_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_vheight_minus_one_byte_offset 	1494	/* 0x000005d6 */
#define mnet_ssd_adas_flex_pic_prim_split_2_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_2_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_drotate_bit_offset 	12006	/* 0x00002ee6 */
#define mnet_ssd_adas_flex_pic_prim_split_2_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_hflip_bit_offset 	12007	/* 0x00002ee7 */
#define mnet_ssd_adas_flex_pic_prim_split_2_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_vflip_bit_offset 	12008	/* 0x00002ee8 */
#define mnet_ssd_adas_flex_pic_prim_split_2_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_dflip_bit_offset 	12009	/* 0x00002ee9 */
#define mnet_ssd_adas_flex_pic_prim_split_2_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_pflip_bit_offset 	12010	/* 0x00002eea */
#define mnet_ssd_adas_flex_pic_prim_split_2_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_W 	75
#define mnet_ssd_adas_flex_pic_prim_split_2_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_H 	75
#define mnet_ssd_adas_flex_pic_prim_split_2_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_D 	128
#define mnet_ssd_adas_flex_pic_prim_split_2_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_P 	1
#define mnet_ssd_adas_flex_pic_prim_split_2_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_2_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_2_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_2_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_2_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_2_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_2_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_2_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_vmem_buffer_num_bytes 	81920
#define mnet_ssd_adas_flex_pic_prim_split_2_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_dram_size_num_bytes 	768000
#define mnet_ssd_adas_flex_pic_prim_split_2_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_2_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_datasize 	0
#define mnet_ssd_adas_flex_pic_prim_split_2_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_expoffset 	6
#define mnet_ssd_adas_flex_pic_prim_split_2_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_2_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_2_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_2_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_2_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_2_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_2_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_2 SMB_output __vas_created_terminate__v0 */
#define mnet_ssd_adas_flex_pic_prim_split_2___vas_created_terminate__v0_cnngen_demangled_name 	"__vas_created_terminate__v0"
#define mnet_ssd_adas_flex_pic_prim_split_2___vas_created_terminate__v0_is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___vas_created_terminate__v0_is_bitvector 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___vas_created_terminate__v0_is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___vas_created_terminate__v0_is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___vas_created_terminate__v0_byte_offset 	1768	/* 0x000006e8 */
#define mnet_ssd_adas_flex_pic_prim_split_2___vas_created_terminate__v0_vbase_byte_offset 	1772	/* 0x000006ec */
#define mnet_ssd_adas_flex_pic_prim_split_2___vas_created_terminate__v0_W 	256
#define mnet_ssd_adas_flex_pic_prim_split_2___vas_created_terminate__v0_H 	3
#define mnet_ssd_adas_flex_pic_prim_split_2___vas_created_terminate__v0_D 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___vas_created_terminate__v0_P 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___vas_created_terminate__v0_data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___vas_created_terminate__v0_denable 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___vas_created_terminate__v0_dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_2___vas_created_terminate__v0_vp_interleave_mode 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___vas_created_terminate__v0_tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___vas_created_terminate__v0_tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_2___vas_created_terminate__v0_vmem_buffer_num_bytes 	96


#endif /* mnet_ssd_adas_flex_pic_prim_split_2_H */
