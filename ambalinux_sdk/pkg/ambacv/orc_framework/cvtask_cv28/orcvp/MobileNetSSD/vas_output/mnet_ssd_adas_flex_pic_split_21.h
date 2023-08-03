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
#ifndef mnet_ssd_adas_flex_pic_prim_split_21_H
#define mnet_ssd_adas_flex_pic_prim_split_21_H

#define vas_gen_header_version                  	7

/* dag name */
#define mnet_ssd_adas_flex_pic_prim_split_21_DAG_NAME 	"mnet_ssd_adas_flex_pic_prim_split_21"

/* VDG file info */
#define mnet_ssd_adas_flex_pic_prim_split_21_vdg_name 	"mnet_ssd_adas_flex_pic_split_21.vdg"

/* VDG DAG memory info */
#define mnet_ssd_adas_flex_pic_prim_split_21_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_21_byte_size 	1452	/* 0x000005ac */
#define mnet_ssd_adas_flex_pic_prim_split_21_preferred_dram_xfer_size 	1

/* mnet_ssd_adas_flex_pic_prim_split_21 VMEM info */
#define mnet_ssd_adas_flex_pic_prim_split_21_use_ping_pong_vmem 	0
#define mnet_ssd_adas_flex_pic_prim_split_21_VMEM_start 	4096	/* 0x00001000 */
#define mnet_ssd_adas_flex_pic_prim_split_21_VMEM_end 	234024	/* 0x00039228 */
#define mnet_ssd_adas_flex_pic_prim_split_21_image_start 	234024	/* 0x00039228 */
#define mnet_ssd_adas_flex_pic_prim_split_21_image_size 	1452	/* 0x000005ac */
#define mnet_ssd_adas_flex_pic_prim_split_21_dagbin_start 	234024	/* 0x00039228 */

/* mnet_ssd_adas_flex_pic_prim_split_21 DAG info */
#define mnet_ssd_adas_flex_pic_prim_split_21_estimated_cycles 	8668
#define mnet_ssd_adas_flex_pic_prim_split_21_ddi_byte_size 	0	/* 0x00000000 */

/*******************************************************************************
 * DAG byte offset info for HMB/SMB/VCB and their fields
 * in DAG 'mnet_ssd_adas_flex_pic_prim_split_21' in source file 'pre_split21_mnet_ssd_adas_flex_pic_split_0.vas'
 ******************************************************************************/
/* mnet_ssd_adas_flex_pic_prim_split_21 HMB_input __pvcn_2166_ */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2166__cnngen_demangled_name 	"__pvcn_2166_"
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2166__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2166__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2166__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2166__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2166__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2166__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2166__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2166__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2166__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2166__byte_offset 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2166__vbase_byte_offset 	4	/* 0x00000004 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2166__dbase_byte_offset 	28	/* 0x0000001c */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2166__dpitchm1_byte_offset 	32	/* 0x00000020 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2166__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2166__vwidth_minus_one_byte_offset 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2166__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2166__vheight_minus_one_byte_offset 	18	/* 0x00000012 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2166__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2166__drotate_bit_offset 	198	/* 0x000000c6 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2166__hflip_bit_offset 	199	/* 0x000000c7 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2166__vflip_bit_offset 	200	/* 0x000000c8 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2166__dflip_bit_offset 	201	/* 0x000000c9 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2166__pflip_bit_offset 	202	/* 0x000000ca */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2166__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2166__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2166__D 	40
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2166__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2166__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2166__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2166__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2166__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2166__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2166__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2166__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2166__vmem_buffer_num_bytes 	8000
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2166__dram_size_num_bytes 	9600
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2166__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2166__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2166__expoffset 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2166__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2166__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2166__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2166__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2166__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2166__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2166__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_21 HMB_input __pvcn_2243_ */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2243__cnngen_demangled_name 	"__pvcn_2243_"
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2243__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2243__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2243__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2243__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2243__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2243__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2243__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2243__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2243__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2243__byte_offset 	40	/* 0x00000028 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2243__vbase_byte_offset 	44	/* 0x0000002c */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2243__dbase_byte_offset 	68	/* 0x00000044 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2243__dpitchm1_byte_offset 	72	/* 0x00000048 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2243__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2243__vwidth_minus_one_byte_offset 	56	/* 0x00000038 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2243__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2243__vheight_minus_one_byte_offset 	58	/* 0x0000003a */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2243__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2243__drotate_bit_offset 	518	/* 0x00000206 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2243__hflip_bit_offset 	519	/* 0x00000207 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2243__vflip_bit_offset 	520	/* 0x00000208 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2243__dflip_bit_offset 	521	/* 0x00000209 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2243__pflip_bit_offset 	522	/* 0x0000020a */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2243__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2243__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2243__D 	15
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2243__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2243__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2243__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2243__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2243__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2243__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2243__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2243__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2243__vmem_buffer_num_bytes 	3000
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2243__dram_size_num_bytes 	3616
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2243__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2243__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2243__expoffset 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2243__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2243__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2243__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2243__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2243__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2243__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2243__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_21 HMB_input __pvcn_2177_ */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2177__cnngen_demangled_name 	"__pvcn_2177_"
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2177__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2177__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2177__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2177__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2177__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2177__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2177__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2177__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2177__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2177__byte_offset 	80	/* 0x00000050 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2177__vbase_byte_offset 	84	/* 0x00000054 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2177__dbase_byte_offset 	108	/* 0x0000006c */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2177__dpitchm1_byte_offset 	112	/* 0x00000070 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2177__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2177__vwidth_minus_one_byte_offset 	96	/* 0x00000060 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2177__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2177__vheight_minus_one_byte_offset 	98	/* 0x00000062 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2177__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2177__drotate_bit_offset 	838	/* 0x00000346 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2177__hflip_bit_offset 	839	/* 0x00000347 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2177__vflip_bit_offset 	840	/* 0x00000348 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2177__dflip_bit_offset 	841	/* 0x00000349 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2177__pflip_bit_offset 	842	/* 0x0000034a */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2177__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2177__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2177__D 	37
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2177__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2177__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2177__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2177__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2177__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2177__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2177__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2177__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2177__vmem_buffer_num_bytes 	7400
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2177__dram_size_num_bytes 	8896
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2177__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2177__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2177__expoffset 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2177__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2177__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2177__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2177__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2177__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2177__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2177__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_21 HMB_input __pvcn_2188_ */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2188__cnngen_demangled_name 	"__pvcn_2188_"
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2188__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2188__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2188__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2188__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2188__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2188__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2188__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2188__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2188__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2188__byte_offset 	120	/* 0x00000078 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2188__vbase_byte_offset 	124	/* 0x0000007c */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2188__dbase_byte_offset 	148	/* 0x00000094 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2188__dpitchm1_byte_offset 	152	/* 0x00000098 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2188__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2188__vwidth_minus_one_byte_offset 	136	/* 0x00000088 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2188__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2188__vheight_minus_one_byte_offset 	138	/* 0x0000008a */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2188__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2188__drotate_bit_offset 	1158	/* 0x00000486 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2188__hflip_bit_offset 	1159	/* 0x00000487 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2188__vflip_bit_offset 	1160	/* 0x00000488 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2188__dflip_bit_offset 	1161	/* 0x00000489 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2188__pflip_bit_offset 	1162	/* 0x0000048a */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2188__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2188__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2188__D 	36
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2188__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2188__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2188__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2188__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2188__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2188__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2188__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2188__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2188__vmem_buffer_num_bytes 	7200
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2188__dram_size_num_bytes 	8640
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2188__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2188__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2188__expoffset 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2188__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2188__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2188__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2188__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2188__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2188__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2188__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_21 HMB_input __pvcn_2155_ */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2155__cnngen_demangled_name 	"__pvcn_2155_"
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2155__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2155__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2155__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2155__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2155__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2155__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2155__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2155__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2155__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2155__byte_offset 	160	/* 0x000000a0 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2155__vbase_byte_offset 	164	/* 0x000000a4 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2155__dbase_byte_offset 	188	/* 0x000000bc */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2155__dpitchm1_byte_offset 	192	/* 0x000000c0 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2155__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2155__vwidth_minus_one_byte_offset 	176	/* 0x000000b0 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2155__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2155__vheight_minus_one_byte_offset 	178	/* 0x000000b2 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2155__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2155__drotate_bit_offset 	1478	/* 0x000005c6 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2155__hflip_bit_offset 	1479	/* 0x000005c7 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2155__vflip_bit_offset 	1480	/* 0x000005c8 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2155__dflip_bit_offset 	1481	/* 0x000005c9 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2155__pflip_bit_offset 	1482	/* 0x000005ca */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2155__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2155__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2155__D 	41
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2155__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2155__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2155__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2155__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2155__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2155__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2155__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2155__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2155__vmem_buffer_num_bytes 	8200
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2155__dram_size_num_bytes 	9856
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2155__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2155__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2155__expoffset 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2155__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2155__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2155__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2155__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2155__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2155__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2155__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_21 HMB_input __pvcn_2210_ */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2210__cnngen_demangled_name 	"__pvcn_2210_"
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2210__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2210__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2210__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2210__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2210__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2210__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2210__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2210__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2210__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2210__byte_offset 	200	/* 0x000000c8 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2210__vbase_byte_offset 	204	/* 0x000000cc */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2210__dbase_byte_offset 	228	/* 0x000000e4 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2210__dpitchm1_byte_offset 	232	/* 0x000000e8 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2210__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2210__vwidth_minus_one_byte_offset 	216	/* 0x000000d8 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2210__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2210__vheight_minus_one_byte_offset 	218	/* 0x000000da */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2210__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2210__drotate_bit_offset 	1798	/* 0x00000706 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2210__hflip_bit_offset 	1799	/* 0x00000707 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2210__vflip_bit_offset 	1800	/* 0x00000708 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2210__dflip_bit_offset 	1801	/* 0x00000709 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2210__pflip_bit_offset 	1802	/* 0x0000070a */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2210__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2210__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2210__D 	37
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2210__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2210__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2210__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2210__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2210__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2210__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2210__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2210__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2210__vmem_buffer_num_bytes 	7400
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2210__dram_size_num_bytes 	8896
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2210__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2210__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2210__expoffset 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2210__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2210__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2210__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2210__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2210__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2210__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2210__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_21 HMB_input __pvcn_2221_ */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2221__cnngen_demangled_name 	"__pvcn_2221_"
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2221__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2221__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2221__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2221__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2221__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2221__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2221__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2221__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2221__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2221__byte_offset 	240	/* 0x000000f0 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2221__vbase_byte_offset 	244	/* 0x000000f4 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2221__dbase_byte_offset 	268	/* 0x0000010c */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2221__dpitchm1_byte_offset 	272	/* 0x00000110 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2221__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2221__vwidth_minus_one_byte_offset 	256	/* 0x00000100 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2221__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2221__vheight_minus_one_byte_offset 	258	/* 0x00000102 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2221__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2221__drotate_bit_offset 	2118	/* 0x00000846 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2221__hflip_bit_offset 	2119	/* 0x00000847 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2221__vflip_bit_offset 	2120	/* 0x00000848 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2221__dflip_bit_offset 	2121	/* 0x00000849 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2221__pflip_bit_offset 	2122	/* 0x0000084a */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2221__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2221__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2221__D 	40
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2221__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2221__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2221__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2221__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2221__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2221__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2221__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2221__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2221__vmem_buffer_num_bytes 	8000
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2221__dram_size_num_bytes 	9600
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2221__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2221__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2221__expoffset 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2221__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2221__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2221__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2221__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2221__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2221__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2221__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_21 HMB_input __pvcn_2144_ */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2144__cnngen_demangled_name 	"__pvcn_2144_"
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2144__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2144__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2144__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2144__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2144__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2144__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2144__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2144__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2144__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2144__byte_offset 	280	/* 0x00000118 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2144__vbase_byte_offset 	284	/* 0x0000011c */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2144__dbase_byte_offset 	308	/* 0x00000134 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2144__dpitchm1_byte_offset 	312	/* 0x00000138 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2144__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2144__vwidth_minus_one_byte_offset 	296	/* 0x00000128 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2144__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2144__vheight_minus_one_byte_offset 	298	/* 0x0000012a */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2144__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2144__drotate_bit_offset 	2438	/* 0x00000986 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2144__hflip_bit_offset 	2439	/* 0x00000987 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2144__vflip_bit_offset 	2440	/* 0x00000988 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2144__dflip_bit_offset 	2441	/* 0x00000989 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2144__pflip_bit_offset 	2442	/* 0x0000098a */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2144__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2144__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2144__D 	39
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2144__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2144__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2144__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2144__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2144__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2144__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2144__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2144__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2144__vmem_buffer_num_bytes 	7800
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2144__dram_size_num_bytes 	9376
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2144__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2144__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2144__expoffset 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2144__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2144__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2144__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2144__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2144__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2144__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2144__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_21 HMB_input __pvcn_2199_ */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2199__cnngen_demangled_name 	"__pvcn_2199_"
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2199__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2199__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2199__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2199__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2199__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2199__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2199__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2199__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2199__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2199__byte_offset 	320	/* 0x00000140 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2199__vbase_byte_offset 	324	/* 0x00000144 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2199__dbase_byte_offset 	348	/* 0x0000015c */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2199__dpitchm1_byte_offset 	352	/* 0x00000160 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2199__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2199__vwidth_minus_one_byte_offset 	336	/* 0x00000150 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2199__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2199__vheight_minus_one_byte_offset 	338	/* 0x00000152 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2199__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2199__drotate_bit_offset 	2758	/* 0x00000ac6 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2199__hflip_bit_offset 	2759	/* 0x00000ac7 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2199__vflip_bit_offset 	2760	/* 0x00000ac8 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2199__dflip_bit_offset 	2761	/* 0x00000ac9 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2199__pflip_bit_offset 	2762	/* 0x00000aca */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2199__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2199__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2199__D 	41
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2199__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2199__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2199__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2199__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2199__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2199__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2199__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2199__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2199__vmem_buffer_num_bytes 	8200
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2199__dram_size_num_bytes 	9856
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2199__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2199__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2199__expoffset 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2199__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2199__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2199__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2199__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2199__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2199__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2199__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_21 HMB_input conv5_5_mbox_conf___conv2i___76______loc___conv2i___136______muli___370___1 */
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136______muli___370___1_cnngen_demangled_name 	"conv5_5_mbox_conf___conv2i___76______loc___conv2i___136______muli___370___1"
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136______muli___370___1_is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136______muli___370___1_is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136______muli___370___1_has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136______muli___370___1_is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136______muli___370___1_ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136______muli___370___1_ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136______muli___370___1_is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136______muli___370___1_is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136______muli___370___1_is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136______muli___370___1_byte_offset 	360	/* 0x00000168 */
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136______muli___370___1_vbase_byte_offset 	364	/* 0x0000016c */
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136______muli___370___1_dbase_byte_offset 	388	/* 0x00000184 */
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136______muli___370___1_dpitchm1_byte_offset 	392	/* 0x00000188 */
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136______muli___370___1_dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136______muli___370___1_vwidth_minus_one_byte_offset 	376	/* 0x00000178 */
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136______muli___370___1_vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136______muli___370___1_vheight_minus_one_byte_offset 	378	/* 0x0000017a */
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136______muli___370___1_vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136______muli___370___1_drotate_bit_offset 	3078	/* 0x00000c06 */
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136______muli___370___1_hflip_bit_offset 	3079	/* 0x00000c07 */
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136______muli___370___1_vflip_bit_offset 	3080	/* 0x00000c08 */
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136______muli___370___1_dflip_bit_offset 	3081	/* 0x00000c09 */
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136______muli___370___1_pflip_bit_offset 	3082	/* 0x00000c0a */
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136______muli___370___1_W 	19
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136______muli___370___1_H 	19
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136______muli___370___1_D 	33
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136______muli___370___1_P 	1
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136______muli___370___1_data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136______muli___370___1_denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136______muli___370___1_dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136______muli___370___1_dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136______muli___370___1_vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136______muli___370___1_tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136______muli___370___1_tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136______muli___370___1_vmem_buffer_num_bytes 	12936
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136______muli___370___1_dram_size_num_bytes 	15072
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136______muli___370___1_sign 	1
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136______muli___370___1_datasize 	0
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136______muli___370___1_expoffset 	3
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136______muli___370___1_expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136______muli___370___1_drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136______muli___370___1_hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136______muli___370___1_vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136______muli___370___1_dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136______muli___370___1_pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136______muli___370___1_trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_21 HMB_input __pvcn_2232_ */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2232__cnngen_demangled_name 	"__pvcn_2232_"
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2232__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2232__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2232__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2232__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2232__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2232__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2232__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2232__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2232__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2232__byte_offset 	400	/* 0x00000190 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2232__vbase_byte_offset 	404	/* 0x00000194 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2232__dbase_byte_offset 	428	/* 0x000001ac */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2232__dpitchm1_byte_offset 	432	/* 0x000001b0 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2232__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2232__vwidth_minus_one_byte_offset 	416	/* 0x000001a0 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2232__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2232__vheight_minus_one_byte_offset 	418	/* 0x000001a2 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2232__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2232__drotate_bit_offset 	3398	/* 0x00000d46 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2232__hflip_bit_offset 	3399	/* 0x00000d47 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2232__vflip_bit_offset 	3400	/* 0x00000d48 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2232__dflip_bit_offset 	3401	/* 0x00000d49 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2232__pflip_bit_offset 	3402	/* 0x00000d4a */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2232__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2232__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2232__D 	36
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2232__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2232__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2232__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2232__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2232__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2232__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2232__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2232__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2232__vmem_buffer_num_bytes 	7200
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2232__dram_size_num_bytes 	8640
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2232__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2232__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2232__expoffset 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2232__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2232__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2232__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2232__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2232__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2232__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2232__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_21 HMB_input __pvcn_2133_ */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2133__cnngen_demangled_name 	"__pvcn_2133_"
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2133__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2133__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2133__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2133__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2133__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2133__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2133__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2133__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2133__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2133__byte_offset 	440	/* 0x000001b8 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2133__vbase_byte_offset 	444	/* 0x000001bc */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2133__dbase_byte_offset 	468	/* 0x000001d4 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2133__dpitchm1_byte_offset 	472	/* 0x000001d8 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2133__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2133__vwidth_minus_one_byte_offset 	456	/* 0x000001c8 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2133__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2133__vheight_minus_one_byte_offset 	458	/* 0x000001ca */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2133__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2133__drotate_bit_offset 	3718	/* 0x00000e86 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2133__hflip_bit_offset 	3719	/* 0x00000e87 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2133__vflip_bit_offset 	3720	/* 0x00000e88 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2133__dflip_bit_offset 	3721	/* 0x00000e89 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2133__pflip_bit_offset 	3722	/* 0x00000e8a */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2133__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2133__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2133__D 	35
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2133__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2133__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2133__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2133__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2133__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2133__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2133__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2133__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2133__vmem_buffer_num_bytes 	7000
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2133__dram_size_num_bytes 	8416
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2133__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2133__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2133__expoffset 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2133__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2133__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2133__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2133__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2133__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2133__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2133__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_21 HMB_input __pvcn_2122_ */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2122__cnngen_demangled_name 	"__pvcn_2122_"
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2122__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2122__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2122__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2122__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2122__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2122__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2122__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2122__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2122__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2122__byte_offset 	480	/* 0x000001e0 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2122__vbase_byte_offset 	484	/* 0x000001e4 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2122__dbase_byte_offset 	508	/* 0x000001fc */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2122__dpitchm1_byte_offset 	512	/* 0x00000200 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2122__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2122__vwidth_minus_one_byte_offset 	496	/* 0x000001f0 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2122__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2122__vheight_minus_one_byte_offset 	498	/* 0x000001f2 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2122__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2122__drotate_bit_offset 	4038	/* 0x00000fc6 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2122__hflip_bit_offset 	4039	/* 0x00000fc7 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2122__vflip_bit_offset 	4040	/* 0x00000fc8 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2122__dflip_bit_offset 	4041	/* 0x00000fc9 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2122__pflip_bit_offset 	4042	/* 0x00000fca */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2122__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2122__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2122__D 	36
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2122__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2122__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2122__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2122__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2122__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2122__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2122__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2122__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2122__vmem_buffer_num_bytes 	7200
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2122__dram_size_num_bytes 	8640
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2122__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2122__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2122__expoffset 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2122__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2122__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2122__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2122__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2122__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2122__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2122__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_21 HMB_input __pvcn_2111_ */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2111__cnngen_demangled_name 	"__pvcn_2111_"
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2111__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2111__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2111__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2111__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2111__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2111__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2111__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2111__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2111__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2111__byte_offset 	520	/* 0x00000208 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2111__vbase_byte_offset 	524	/* 0x0000020c */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2111__dbase_byte_offset 	548	/* 0x00000224 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2111__dpitchm1_byte_offset 	552	/* 0x00000228 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2111__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2111__vwidth_minus_one_byte_offset 	536	/* 0x00000218 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2111__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2111__vheight_minus_one_byte_offset 	538	/* 0x0000021a */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2111__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2111__drotate_bit_offset 	4358	/* 0x00001106 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2111__hflip_bit_offset 	4359	/* 0x00001107 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2111__vflip_bit_offset 	4360	/* 0x00001108 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2111__dflip_bit_offset 	4361	/* 0x00001109 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2111__pflip_bit_offset 	4362	/* 0x0000110a */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2111__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2111__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2111__D 	40
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2111__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2111__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2111__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2111__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2111__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2111__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2111__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2111__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2111__vmem_buffer_num_bytes 	8000
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2111__dram_size_num_bytes 	9600
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2111__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2111__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2111__expoffset 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2111__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2111__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2111__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2111__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2111__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2111__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2111__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_21 HMB_input __pvcn_2100_ */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2100__cnngen_demangled_name 	"__pvcn_2100_"
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2100__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2100__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2100__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2100__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2100__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2100__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2100__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2100__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2100__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2100__byte_offset 	560	/* 0x00000230 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2100__vbase_byte_offset 	564	/* 0x00000234 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2100__dbase_byte_offset 	588	/* 0x0000024c */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2100__dpitchm1_byte_offset 	592	/* 0x00000250 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2100__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2100__vwidth_minus_one_byte_offset 	576	/* 0x00000240 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2100__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2100__vheight_minus_one_byte_offset 	578	/* 0x00000242 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2100__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2100__drotate_bit_offset 	4678	/* 0x00001246 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2100__hflip_bit_offset 	4679	/* 0x00001247 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2100__vflip_bit_offset 	4680	/* 0x00001248 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2100__dflip_bit_offset 	4681	/* 0x00001249 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2100__pflip_bit_offset 	4682	/* 0x0000124a */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2100__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2100__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2100__D 	39
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2100__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2100__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2100__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2100__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2100__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2100__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2100__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2100__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2100__vmem_buffer_num_bytes 	7800
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2100__dram_size_num_bytes 	9376
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2100__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2100__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2100__expoffset 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2100__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2100__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2100__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2100__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2100__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2100__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2100__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_21 HMB_output __pvcn_2094_ */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2094__cnngen_demangled_name 	"__pvcn_2094_"
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2094__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2094__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2094__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2094__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2094__byte_offset 	608	/* 0x00000260 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2094__vbase_byte_offset 	612	/* 0x00000264 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2094__dbase_byte_offset 	636	/* 0x0000027c */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2094__dpitchm1_byte_offset 	640	/* 0x00000280 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2094__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2094__vwidth_minus_one_byte_offset 	624	/* 0x00000270 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2094__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2094__vheight_minus_one_byte_offset 	626	/* 0x00000272 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2094__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2094__drotate_bit_offset 	5062	/* 0x000013c6 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2094__hflip_bit_offset 	5063	/* 0x000013c7 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2094__vflip_bit_offset 	5064	/* 0x000013c8 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2094__dflip_bit_offset 	5065	/* 0x000013c9 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2094__pflip_bit_offset 	5066	/* 0x000013ca */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2094__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2094__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2094__D 	39
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2094__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2094__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2094__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2094__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2094__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2094__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2094__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2094__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2094__vmem_buffer_num_bytes 	7800
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2094__dram_size_num_bytes 	9376
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2094__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2094__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2094__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2094__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2094__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2094__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2094__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2094__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2094__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2094__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_21 HMB_output __pvcn_2105_ */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2105__cnngen_demangled_name 	"__pvcn_2105_"
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2105__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2105__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2105__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2105__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2105__byte_offset 	656	/* 0x00000290 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2105__vbase_byte_offset 	660	/* 0x00000294 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2105__dbase_byte_offset 	684	/* 0x000002ac */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2105__dpitchm1_byte_offset 	688	/* 0x000002b0 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2105__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2105__vwidth_minus_one_byte_offset 	672	/* 0x000002a0 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2105__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2105__vheight_minus_one_byte_offset 	674	/* 0x000002a2 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2105__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2105__drotate_bit_offset 	5446	/* 0x00001546 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2105__hflip_bit_offset 	5447	/* 0x00001547 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2105__vflip_bit_offset 	5448	/* 0x00001548 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2105__dflip_bit_offset 	5449	/* 0x00001549 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2105__pflip_bit_offset 	5450	/* 0x0000154a */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2105__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2105__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2105__D 	40
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2105__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2105__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2105__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2105__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2105__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2105__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2105__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2105__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2105__vmem_buffer_num_bytes 	8000
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2105__dram_size_num_bytes 	9600
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2105__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2105__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2105__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2105__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2105__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2105__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2105__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2105__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2105__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2105__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_21 HMB_output __pvcn_2116_ */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2116__cnngen_demangled_name 	"__pvcn_2116_"
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2116__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2116__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2116__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2116__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2116__byte_offset 	704	/* 0x000002c0 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2116__vbase_byte_offset 	708	/* 0x000002c4 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2116__dbase_byte_offset 	732	/* 0x000002dc */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2116__dpitchm1_byte_offset 	736	/* 0x000002e0 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2116__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2116__vwidth_minus_one_byte_offset 	720	/* 0x000002d0 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2116__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2116__vheight_minus_one_byte_offset 	722	/* 0x000002d2 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2116__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2116__drotate_bit_offset 	5830	/* 0x000016c6 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2116__hflip_bit_offset 	5831	/* 0x000016c7 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2116__vflip_bit_offset 	5832	/* 0x000016c8 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2116__dflip_bit_offset 	5833	/* 0x000016c9 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2116__pflip_bit_offset 	5834	/* 0x000016ca */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2116__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2116__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2116__D 	36
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2116__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2116__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2116__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2116__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2116__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2116__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2116__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2116__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2116__vmem_buffer_num_bytes 	7200
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2116__dram_size_num_bytes 	8640
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2116__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2116__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2116__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2116__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2116__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2116__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2116__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2116__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2116__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2116__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_21 HMB_output __pvcn_2127_ */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2127__cnngen_demangled_name 	"__pvcn_2127_"
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2127__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2127__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2127__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2127__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2127__byte_offset 	752	/* 0x000002f0 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2127__vbase_byte_offset 	756	/* 0x000002f4 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2127__dbase_byte_offset 	780	/* 0x0000030c */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2127__dpitchm1_byte_offset 	784	/* 0x00000310 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2127__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2127__vwidth_minus_one_byte_offset 	768	/* 0x00000300 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2127__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2127__vheight_minus_one_byte_offset 	770	/* 0x00000302 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2127__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2127__drotate_bit_offset 	6214	/* 0x00001846 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2127__hflip_bit_offset 	6215	/* 0x00001847 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2127__vflip_bit_offset 	6216	/* 0x00001848 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2127__dflip_bit_offset 	6217	/* 0x00001849 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2127__pflip_bit_offset 	6218	/* 0x0000184a */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2127__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2127__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2127__D 	35
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2127__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2127__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2127__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2127__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2127__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2127__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2127__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2127__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2127__vmem_buffer_num_bytes 	7000
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2127__dram_size_num_bytes 	8416
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2127__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2127__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2127__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2127__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2127__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2127__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2127__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2127__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2127__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2127__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_21 HMB_output __pvcn_2138_ */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2138__cnngen_demangled_name 	"__pvcn_2138_"
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2138__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2138__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2138__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2138__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2138__byte_offset 	800	/* 0x00000320 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2138__vbase_byte_offset 	804	/* 0x00000324 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2138__dbase_byte_offset 	828	/* 0x0000033c */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2138__dpitchm1_byte_offset 	832	/* 0x00000340 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2138__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2138__vwidth_minus_one_byte_offset 	816	/* 0x00000330 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2138__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2138__vheight_minus_one_byte_offset 	818	/* 0x00000332 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2138__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2138__drotate_bit_offset 	6598	/* 0x000019c6 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2138__hflip_bit_offset 	6599	/* 0x000019c7 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2138__vflip_bit_offset 	6600	/* 0x000019c8 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2138__dflip_bit_offset 	6601	/* 0x000019c9 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2138__pflip_bit_offset 	6602	/* 0x000019ca */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2138__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2138__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2138__D 	39
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2138__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2138__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2138__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2138__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2138__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2138__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2138__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2138__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2138__vmem_buffer_num_bytes 	7800
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2138__dram_size_num_bytes 	9376
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2138__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2138__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2138__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2138__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2138__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2138__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2138__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2138__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2138__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2138__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_21 HMB_output __pvcn_2149_ */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2149__cnngen_demangled_name 	"__pvcn_2149_"
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2149__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2149__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2149__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2149__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2149__byte_offset 	848	/* 0x00000350 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2149__vbase_byte_offset 	852	/* 0x00000354 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2149__dbase_byte_offset 	876	/* 0x0000036c */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2149__dpitchm1_byte_offset 	880	/* 0x00000370 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2149__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2149__vwidth_minus_one_byte_offset 	864	/* 0x00000360 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2149__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2149__vheight_minus_one_byte_offset 	866	/* 0x00000362 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2149__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2149__drotate_bit_offset 	6982	/* 0x00001b46 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2149__hflip_bit_offset 	6983	/* 0x00001b47 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2149__vflip_bit_offset 	6984	/* 0x00001b48 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2149__dflip_bit_offset 	6985	/* 0x00001b49 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2149__pflip_bit_offset 	6986	/* 0x00001b4a */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2149__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2149__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2149__D 	41
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2149__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2149__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2149__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2149__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2149__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2149__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2149__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2149__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2149__vmem_buffer_num_bytes 	8200
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2149__dram_size_num_bytes 	9856
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2149__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2149__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2149__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2149__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2149__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2149__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2149__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2149__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2149__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2149__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_21 HMB_output __pvcn_2160_ */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2160__cnngen_demangled_name 	"__pvcn_2160_"
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2160__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2160__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2160__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2160__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2160__byte_offset 	896	/* 0x00000380 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2160__vbase_byte_offset 	900	/* 0x00000384 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2160__dbase_byte_offset 	924	/* 0x0000039c */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2160__dpitchm1_byte_offset 	928	/* 0x000003a0 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2160__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2160__vwidth_minus_one_byte_offset 	912	/* 0x00000390 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2160__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2160__vheight_minus_one_byte_offset 	914	/* 0x00000392 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2160__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2160__drotate_bit_offset 	7366	/* 0x00001cc6 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2160__hflip_bit_offset 	7367	/* 0x00001cc7 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2160__vflip_bit_offset 	7368	/* 0x00001cc8 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2160__dflip_bit_offset 	7369	/* 0x00001cc9 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2160__pflip_bit_offset 	7370	/* 0x00001cca */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2160__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2160__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2160__D 	40
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2160__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2160__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2160__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2160__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2160__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2160__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2160__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2160__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2160__vmem_buffer_num_bytes 	8000
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2160__dram_size_num_bytes 	9600
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2160__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2160__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2160__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2160__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2160__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2160__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2160__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2160__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2160__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2160__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_21 HMB_output __pvcn_2171_ */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2171__cnngen_demangled_name 	"__pvcn_2171_"
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2171__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2171__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2171__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2171__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2171__byte_offset 	944	/* 0x000003b0 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2171__vbase_byte_offset 	948	/* 0x000003b4 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2171__dbase_byte_offset 	972	/* 0x000003cc */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2171__dpitchm1_byte_offset 	976	/* 0x000003d0 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2171__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2171__vwidth_minus_one_byte_offset 	960	/* 0x000003c0 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2171__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2171__vheight_minus_one_byte_offset 	962	/* 0x000003c2 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2171__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2171__drotate_bit_offset 	7750	/* 0x00001e46 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2171__hflip_bit_offset 	7751	/* 0x00001e47 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2171__vflip_bit_offset 	7752	/* 0x00001e48 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2171__dflip_bit_offset 	7753	/* 0x00001e49 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2171__pflip_bit_offset 	7754	/* 0x00001e4a */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2171__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2171__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2171__D 	37
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2171__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2171__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2171__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2171__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2171__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2171__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2171__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2171__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2171__vmem_buffer_num_bytes 	7400
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2171__dram_size_num_bytes 	8896
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2171__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2171__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2171__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2171__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2171__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2171__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2171__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2171__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2171__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2171__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_21 HMB_output __pvcn_2182_ */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2182__cnngen_demangled_name 	"__pvcn_2182_"
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2182__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2182__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2182__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2182__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2182__byte_offset 	992	/* 0x000003e0 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2182__vbase_byte_offset 	996	/* 0x000003e4 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2182__dbase_byte_offset 	1020	/* 0x000003fc */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2182__dpitchm1_byte_offset 	1024	/* 0x00000400 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2182__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2182__vwidth_minus_one_byte_offset 	1008	/* 0x000003f0 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2182__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2182__vheight_minus_one_byte_offset 	1010	/* 0x000003f2 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2182__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2182__drotate_bit_offset 	8134	/* 0x00001fc6 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2182__hflip_bit_offset 	8135	/* 0x00001fc7 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2182__vflip_bit_offset 	8136	/* 0x00001fc8 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2182__dflip_bit_offset 	8137	/* 0x00001fc9 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2182__pflip_bit_offset 	8138	/* 0x00001fca */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2182__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2182__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2182__D 	36
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2182__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2182__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2182__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2182__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2182__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2182__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2182__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2182__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2182__vmem_buffer_num_bytes 	7200
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2182__dram_size_num_bytes 	8640
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2182__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2182__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2182__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2182__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2182__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2182__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2182__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2182__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2182__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2182__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_21 HMB_output __pvcn_2193_ */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2193__cnngen_demangled_name 	"__pvcn_2193_"
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2193__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2193__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2193__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2193__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2193__byte_offset 	1040	/* 0x00000410 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2193__vbase_byte_offset 	1044	/* 0x00000414 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2193__dbase_byte_offset 	1068	/* 0x0000042c */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2193__dpitchm1_byte_offset 	1072	/* 0x00000430 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2193__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2193__vwidth_minus_one_byte_offset 	1056	/* 0x00000420 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2193__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2193__vheight_minus_one_byte_offset 	1058	/* 0x00000422 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2193__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2193__drotate_bit_offset 	8518	/* 0x00002146 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2193__hflip_bit_offset 	8519	/* 0x00002147 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2193__vflip_bit_offset 	8520	/* 0x00002148 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2193__dflip_bit_offset 	8521	/* 0x00002149 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2193__pflip_bit_offset 	8522	/* 0x0000214a */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2193__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2193__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2193__D 	41
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2193__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2193__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2193__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2193__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2193__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2193__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2193__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2193__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2193__vmem_buffer_num_bytes 	8200
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2193__dram_size_num_bytes 	9856
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2193__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2193__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2193__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2193__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2193__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2193__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2193__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2193__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2193__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2193__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_21 HMB_output __pvcn_2204_ */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2204__cnngen_demangled_name 	"__pvcn_2204_"
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2204__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2204__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2204__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2204__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2204__byte_offset 	1088	/* 0x00000440 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2204__vbase_byte_offset 	1092	/* 0x00000444 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2204__dbase_byte_offset 	1116	/* 0x0000045c */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2204__dpitchm1_byte_offset 	1120	/* 0x00000460 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2204__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2204__vwidth_minus_one_byte_offset 	1104	/* 0x00000450 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2204__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2204__vheight_minus_one_byte_offset 	1106	/* 0x00000452 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2204__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2204__drotate_bit_offset 	8902	/* 0x000022c6 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2204__hflip_bit_offset 	8903	/* 0x000022c7 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2204__vflip_bit_offset 	8904	/* 0x000022c8 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2204__dflip_bit_offset 	8905	/* 0x000022c9 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2204__pflip_bit_offset 	8906	/* 0x000022ca */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2204__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2204__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2204__D 	37
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2204__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2204__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2204__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2204__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2204__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2204__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2204__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2204__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2204__vmem_buffer_num_bytes 	7400
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2204__dram_size_num_bytes 	8896
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2204__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2204__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2204__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2204__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2204__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2204__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2204__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2204__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2204__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2204__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_21 HMB_output __pvcn_2215_ */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2215__cnngen_demangled_name 	"__pvcn_2215_"
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2215__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2215__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2215__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2215__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2215__byte_offset 	1136	/* 0x00000470 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2215__vbase_byte_offset 	1140	/* 0x00000474 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2215__dbase_byte_offset 	1164	/* 0x0000048c */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2215__dpitchm1_byte_offset 	1168	/* 0x00000490 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2215__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2215__vwidth_minus_one_byte_offset 	1152	/* 0x00000480 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2215__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2215__vheight_minus_one_byte_offset 	1154	/* 0x00000482 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2215__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2215__drotate_bit_offset 	9286	/* 0x00002446 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2215__hflip_bit_offset 	9287	/* 0x00002447 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2215__vflip_bit_offset 	9288	/* 0x00002448 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2215__dflip_bit_offset 	9289	/* 0x00002449 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2215__pflip_bit_offset 	9290	/* 0x0000244a */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2215__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2215__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2215__D 	40
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2215__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2215__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2215__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2215__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2215__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2215__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2215__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2215__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2215__vmem_buffer_num_bytes 	8000
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2215__dram_size_num_bytes 	9600
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2215__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2215__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2215__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2215__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2215__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2215__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2215__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2215__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2215__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2215__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_21 HMB_output __pvcn_2226_ */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2226__cnngen_demangled_name 	"__pvcn_2226_"
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2226__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2226__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2226__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2226__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2226__byte_offset 	1184	/* 0x000004a0 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2226__vbase_byte_offset 	1188	/* 0x000004a4 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2226__dbase_byte_offset 	1212	/* 0x000004bc */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2226__dpitchm1_byte_offset 	1216	/* 0x000004c0 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2226__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2226__vwidth_minus_one_byte_offset 	1200	/* 0x000004b0 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2226__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2226__vheight_minus_one_byte_offset 	1202	/* 0x000004b2 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2226__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2226__drotate_bit_offset 	9670	/* 0x000025c6 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2226__hflip_bit_offset 	9671	/* 0x000025c7 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2226__vflip_bit_offset 	9672	/* 0x000025c8 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2226__dflip_bit_offset 	9673	/* 0x000025c9 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2226__pflip_bit_offset 	9674	/* 0x000025ca */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2226__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2226__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2226__D 	36
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2226__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2226__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2226__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2226__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2226__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2226__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2226__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2226__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2226__vmem_buffer_num_bytes 	7200
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2226__dram_size_num_bytes 	8640
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2226__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2226__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2226__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2226__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2226__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2226__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2226__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2226__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2226__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2226__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_21 HMB_output __pvcn_2237_ */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2237__cnngen_demangled_name 	"__pvcn_2237_"
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2237__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2237__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2237__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2237__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2237__byte_offset 	1232	/* 0x000004d0 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2237__vbase_byte_offset 	1236	/* 0x000004d4 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2237__dbase_byte_offset 	1260	/* 0x000004ec */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2237__dpitchm1_byte_offset 	1264	/* 0x000004f0 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2237__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2237__vwidth_minus_one_byte_offset 	1248	/* 0x000004e0 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2237__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2237__vheight_minus_one_byte_offset 	1250	/* 0x000004e2 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2237__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2237__drotate_bit_offset 	10054	/* 0x00002746 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2237__hflip_bit_offset 	10055	/* 0x00002747 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2237__vflip_bit_offset 	10056	/* 0x00002748 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2237__dflip_bit_offset 	10057	/* 0x00002749 */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2237__pflip_bit_offset 	10058	/* 0x0000274a */
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2237__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2237__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2237__D 	15
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2237__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2237__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2237__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2237__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2237__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2237__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2237__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2237__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2237__vmem_buffer_num_bytes 	2160
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2237__dram_size_num_bytes 	3616
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2237__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2237__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2237__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2237__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2237__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2237__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2237__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2237__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2237__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___pvcn_2237__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_21 HMB_output conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___321 */
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___321_cnngen_demangled_name 	"conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___321"
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___321_is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___321_is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___321_is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___321_is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___321_byte_offset 	1284	/* 0x00000504 */
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___321_vbase_byte_offset 	1288	/* 0x00000508 */
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___321_dbase_byte_offset 	1312	/* 0x00000520 */
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___321_dpitchm1_byte_offset 	1316	/* 0x00000524 */
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___321_dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___321_vwidth_minus_one_byte_offset 	1300	/* 0x00000514 */
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___321_vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___321_vheight_minus_one_byte_offset 	1302	/* 0x00000516 */
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___321_vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___321_drotate_bit_offset 	10470	/* 0x000028e6 */
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___321_hflip_bit_offset 	10471	/* 0x000028e7 */
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___321_vflip_bit_offset 	10472	/* 0x000028e8 */
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___321_dflip_bit_offset 	10473	/* 0x000028e9 */
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___321_pflip_bit_offset 	10474	/* 0x000028ea */
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___321_W 	19
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___321_H 	19
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___321_D 	12
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___321_P 	1
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___321_data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___321_denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___321_dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___321_dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___321_vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___321_tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___321_tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___321_vmem_buffer_num_bytes 	4704
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___321_dram_size_num_bytes 	14592
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___321_sign 	1
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___321_datasize 	0
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___321_expoffset 	3
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___321_expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___321_drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___321_hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___321_vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___321_dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___321_pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___321_trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_21 HMB_output conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___320 */
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___320_cnngen_demangled_name 	"conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___320"
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___320_is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___320_is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___320_is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___320_is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___320_byte_offset 	1336	/* 0x00000538 */
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___320_vbase_byte_offset 	1340	/* 0x0000053c */
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___320_dbase_byte_offset 	1364	/* 0x00000554 */
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___320_dpitchm1_byte_offset 	1368	/* 0x00000558 */
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___320_dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___320_vwidth_minus_one_byte_offset 	1352	/* 0x00000548 */
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___320_vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___320_vheight_minus_one_byte_offset 	1354	/* 0x0000054a */
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___320_vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___320_drotate_bit_offset 	10886	/* 0x00002a86 */
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___320_hflip_bit_offset 	10887	/* 0x00002a87 */
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___320_vflip_bit_offset 	10888	/* 0x00002a88 */
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___320_dflip_bit_offset 	10889	/* 0x00002a89 */
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___320_pflip_bit_offset 	10890	/* 0x00002a8a */
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___320_W 	19
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___320_H 	19
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___320_D 	21
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___320_P 	1
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___320_data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___320_denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___320_dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___320_dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___320_vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___320_tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___320_tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___320_vmem_buffer_num_bytes 	8232
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___320_dram_size_num_bytes 	25536
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___320_sign 	1
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___320_datasize 	0
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___320_expoffset 	3
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___320_expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___320_drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___320_hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___320_vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___320_dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___320_pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_21_conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___320_trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_21 SMB_output __vas_created_terminate__v0 */
#define mnet_ssd_adas_flex_pic_prim_split_21___vas_created_terminate__v0_cnngen_demangled_name 	"__vas_created_terminate__v0"
#define mnet_ssd_adas_flex_pic_prim_split_21___vas_created_terminate__v0_is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___vas_created_terminate__v0_is_bitvector 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___vas_created_terminate__v0_is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___vas_created_terminate__v0_is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___vas_created_terminate__v0_byte_offset 	1376	/* 0x00000560 */
#define mnet_ssd_adas_flex_pic_prim_split_21___vas_created_terminate__v0_vbase_byte_offset 	1380	/* 0x00000564 */
#define mnet_ssd_adas_flex_pic_prim_split_21___vas_created_terminate__v0_W 	256
#define mnet_ssd_adas_flex_pic_prim_split_21___vas_created_terminate__v0_H 	3
#define mnet_ssd_adas_flex_pic_prim_split_21___vas_created_terminate__v0_D 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___vas_created_terminate__v0_P 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___vas_created_terminate__v0_data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___vas_created_terminate__v0_denable 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___vas_created_terminate__v0_dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_21___vas_created_terminate__v0_vp_interleave_mode 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___vas_created_terminate__v0_tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___vas_created_terminate__v0_tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_21___vas_created_terminate__v0_vmem_buffer_num_bytes 	96


#endif /* mnet_ssd_adas_flex_pic_prim_split_21_H */
