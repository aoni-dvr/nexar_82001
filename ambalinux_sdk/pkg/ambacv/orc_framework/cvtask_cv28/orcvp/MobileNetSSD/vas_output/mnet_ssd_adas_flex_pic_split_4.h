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
#ifndef mnet_ssd_adas_flex_pic_prim_split_4_H
#define mnet_ssd_adas_flex_pic_prim_split_4_H

#define vas_gen_header_version                  	7

/* dag name */
#define mnet_ssd_adas_flex_pic_prim_split_4_DAG_NAME 	"mnet_ssd_adas_flex_pic_prim_split_4"

/* VDG file info */
#define mnet_ssd_adas_flex_pic_prim_split_4_vdg_name 	"mnet_ssd_adas_flex_pic_split_4.vdg"

/* VDG DAG memory info */
#define mnet_ssd_adas_flex_pic_prim_split_4_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_4_byte_size 	1756	/* 0x000006dc */
#define mnet_ssd_adas_flex_pic_prim_split_4_preferred_dram_xfer_size 	1

/* mnet_ssd_adas_flex_pic_prim_split_4 VMEM info */
#define mnet_ssd_adas_flex_pic_prim_split_4_use_ping_pong_vmem 	0
#define mnet_ssd_adas_flex_pic_prim_split_4_VMEM_start 	4096	/* 0x00001000 */
#define mnet_ssd_adas_flex_pic_prim_split_4_VMEM_end 	510260	/* 0x0007c934 */
#define mnet_ssd_adas_flex_pic_prim_split_4_image_start 	509792	/* 0x0007c760 */
#define mnet_ssd_adas_flex_pic_prim_split_4_image_size 	2224	/* 0x000008b0 */
#define mnet_ssd_adas_flex_pic_prim_split_4_dagbin_start 	510260	/* 0x0007c934 */

/* mnet_ssd_adas_flex_pic_prim_split_4 DAG info */
#define mnet_ssd_adas_flex_pic_prim_split_4_estimated_cycles 	28257
#define mnet_ssd_adas_flex_pic_prim_split_4_ddi_byte_size 	0	/* 0x00000000 */

/*******************************************************************************
 * DAG byte offset info for HMB/SMB/VCB and their fields
 * in DAG 'mnet_ssd_adas_flex_pic_prim_split_4' in source file 'pre_split4_mnet_ssd_adas_flex_pic_split_0.vas'
 ******************************************************************************/
/* mnet_ssd_adas_flex_pic_prim_split_4 HMB_input __pvcn_692_ */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_692__cnngen_demangled_name 	"__pvcn_692_"
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_692__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_692__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_692__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_692__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_692__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_692__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_692__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_692__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_692__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_692__byte_offset 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_692__vbase_byte_offset 	4	/* 0x00000004 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_692__dbase_byte_offset 	28	/* 0x0000001c */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_692__dpitchm1_byte_offset 	32	/* 0x00000020 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_692__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_692__vwidth_minus_one_byte_offset 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_692__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_692__vheight_minus_one_byte_offset 	18	/* 0x00000012 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_692__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_692__drotate_bit_offset 	198	/* 0x000000c6 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_692__hflip_bit_offset 	199	/* 0x000000c7 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_692__vflip_bit_offset 	200	/* 0x000000c8 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_692__dflip_bit_offset 	201	/* 0x000000c9 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_692__pflip_bit_offset 	202	/* 0x000000ca */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_692__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_692__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_692__D 	7
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_692__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_692__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_692__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_692__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_692__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_692__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_692__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_692__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_692__vmem_buffer_num_bytes 	17920
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_692__dram_size_num_bytes 	21280
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_692__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_692__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_692__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_692__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_692__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_692__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_692__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_692__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_692__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_692__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_4 HMB_input __pvcn_681_ */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_681__cnngen_demangled_name 	"__pvcn_681_"
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_681__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_681__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_681__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_681__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_681__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_681__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_681__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_681__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_681__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_681__byte_offset 	40	/* 0x00000028 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_681__vbase_byte_offset 	44	/* 0x0000002c */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_681__dbase_byte_offset 	68	/* 0x00000044 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_681__dpitchm1_byte_offset 	72	/* 0x00000048 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_681__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_681__vwidth_minus_one_byte_offset 	56	/* 0x00000038 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_681__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_681__vheight_minus_one_byte_offset 	58	/* 0x0000003a */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_681__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_681__drotate_bit_offset 	518	/* 0x00000206 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_681__hflip_bit_offset 	519	/* 0x00000207 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_681__vflip_bit_offset 	520	/* 0x00000208 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_681__dflip_bit_offset 	521	/* 0x00000209 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_681__pflip_bit_offset 	522	/* 0x0000020a */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_681__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_681__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_681__D 	9
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_681__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_681__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_681__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_681__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_681__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_681__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_681__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_681__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_681__vmem_buffer_num_bytes 	23040
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_681__dram_size_num_bytes 	27360
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_681__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_681__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_681__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_681__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_681__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_681__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_681__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_681__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_681__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_681__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_4 HMB_input __pvcn_626_ */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_626__cnngen_demangled_name 	"__pvcn_626_"
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_626__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_626__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_626__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_626__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_626__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_626__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_626__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_626__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_626__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_626__byte_offset 	80	/* 0x00000050 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_626__vbase_byte_offset 	84	/* 0x00000054 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_626__dbase_byte_offset 	108	/* 0x0000006c */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_626__dpitchm1_byte_offset 	112	/* 0x00000070 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_626__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_626__vwidth_minus_one_byte_offset 	96	/* 0x00000060 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_626__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_626__vheight_minus_one_byte_offset 	98	/* 0x00000062 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_626__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_626__drotate_bit_offset 	838	/* 0x00000346 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_626__hflip_bit_offset 	839	/* 0x00000347 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_626__vflip_bit_offset 	840	/* 0x00000348 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_626__dflip_bit_offset 	841	/* 0x00000349 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_626__pflip_bit_offset 	842	/* 0x0000034a */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_626__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_626__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_626__D 	8
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_626__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_626__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_626__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_626__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_626__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_626__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_626__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_626__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_626__vmem_buffer_num_bytes 	10240
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_626__dram_size_num_bytes 	24320
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_626__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_626__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_626__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_626__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_626__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_626__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_626__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_626__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_626__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_626__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_4 HMB_input __pvcn_604_ */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_604__cnngen_demangled_name 	"__pvcn_604_"
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_604__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_604__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_604__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_604__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_604__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_604__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_604__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_604__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_604__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_604__byte_offset 	120	/* 0x00000078 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_604__vbase_byte_offset 	124	/* 0x0000007c */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_604__dbase_byte_offset 	148	/* 0x00000094 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_604__dpitchm1_byte_offset 	152	/* 0x00000098 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_604__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_604__vwidth_minus_one_byte_offset 	136	/* 0x00000088 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_604__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_604__vheight_minus_one_byte_offset 	138	/* 0x0000008a */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_604__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_604__drotate_bit_offset 	1158	/* 0x00000486 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_604__hflip_bit_offset 	1159	/* 0x00000487 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_604__vflip_bit_offset 	1160	/* 0x00000488 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_604__dflip_bit_offset 	1161	/* 0x00000489 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_604__pflip_bit_offset 	1162	/* 0x0000048a */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_604__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_604__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_604__D 	8
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_604__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_604__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_604__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_604__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_604__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_604__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_604__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_604__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_604__vmem_buffer_num_bytes 	10240
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_604__dram_size_num_bytes 	24320
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_604__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_604__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_604__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_604__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_604__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_604__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_604__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_604__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_604__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_604__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_4 HMB_input __vcn_2726_ */
#define mnet_ssd_adas_flex_pic_prim_split_4___vcn_2726__cnngen_demangled_name 	"__vcn_2726_"
#define mnet_ssd_adas_flex_pic_prim_split_4___vcn_2726__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___vcn_2726__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___vcn_2726__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___vcn_2726__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___vcn_2726__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_4___vcn_2726__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_4___vcn_2726__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___vcn_2726__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___vcn_2726__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___vcn_2726__byte_offset 	160	/* 0x000000a0 */
#define mnet_ssd_adas_flex_pic_prim_split_4___vcn_2726__vbase_byte_offset 	164	/* 0x000000a4 */
#define mnet_ssd_adas_flex_pic_prim_split_4___vcn_2726__dbase_byte_offset 	188	/* 0x000000bc */
#define mnet_ssd_adas_flex_pic_prim_split_4___vcn_2726__dpitchm1_byte_offset 	192	/* 0x000000c0 */
#define mnet_ssd_adas_flex_pic_prim_split_4___vcn_2726__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___vcn_2726__vwidth_minus_one_byte_offset 	176	/* 0x000000b0 */
#define mnet_ssd_adas_flex_pic_prim_split_4___vcn_2726__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___vcn_2726__vheight_minus_one_byte_offset 	178	/* 0x000000b2 */
#define mnet_ssd_adas_flex_pic_prim_split_4___vcn_2726__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___vcn_2726__drotate_bit_offset 	1478	/* 0x000005c6 */
#define mnet_ssd_adas_flex_pic_prim_split_4___vcn_2726__hflip_bit_offset 	1479	/* 0x000005c7 */
#define mnet_ssd_adas_flex_pic_prim_split_4___vcn_2726__vflip_bit_offset 	1480	/* 0x000005c8 */
#define mnet_ssd_adas_flex_pic_prim_split_4___vcn_2726__dflip_bit_offset 	1481	/* 0x000005c9 */
#define mnet_ssd_adas_flex_pic_prim_split_4___vcn_2726__pflip_bit_offset 	1482	/* 0x000005ca */
#define mnet_ssd_adas_flex_pic_prim_split_4___vcn_2726__W 	75
#define mnet_ssd_adas_flex_pic_prim_split_4___vcn_2726__H 	75
#define mnet_ssd_adas_flex_pic_prim_split_4___vcn_2726__D 	128
#define mnet_ssd_adas_flex_pic_prim_split_4___vcn_2726__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___vcn_2726__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___vcn_2726__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___vcn_2726__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_4___vcn_2726__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_4___vcn_2726__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_4___vcn_2726__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_4___vcn_2726__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___vcn_2726__vmem_buffer_num_bytes 	189440
#define mnet_ssd_adas_flex_pic_prim_split_4___vcn_2726__dram_size_num_bytes 	768000
#define mnet_ssd_adas_flex_pic_prim_split_4___vcn_2726__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___vcn_2726__datasize 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___vcn_2726__expoffset 	6
#define mnet_ssd_adas_flex_pic_prim_split_4___vcn_2726__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___vcn_2726__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___vcn_2726__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___vcn_2726__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___vcn_2726__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___vcn_2726__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___vcn_2726__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_4 HMB_input __pvcn_659_ */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_659__cnngen_demangled_name 	"__pvcn_659_"
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_659__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_659__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_659__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_659__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_659__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_659__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_659__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_659__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_659__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_659__byte_offset 	200	/* 0x000000c8 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_659__vbase_byte_offset 	204	/* 0x000000cc */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_659__dbase_byte_offset 	228	/* 0x000000e4 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_659__dpitchm1_byte_offset 	232	/* 0x000000e8 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_659__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_659__vwidth_minus_one_byte_offset 	216	/* 0x000000d8 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_659__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_659__vheight_minus_one_byte_offset 	218	/* 0x000000da */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_659__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_659__drotate_bit_offset 	1798	/* 0x00000706 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_659__hflip_bit_offset 	1799	/* 0x00000707 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_659__vflip_bit_offset 	1800	/* 0x00000708 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_659__dflip_bit_offset 	1801	/* 0x00000709 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_659__pflip_bit_offset 	1802	/* 0x0000070a */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_659__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_659__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_659__D 	7
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_659__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_659__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_659__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_659__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_659__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_659__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_659__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_659__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_659__vmem_buffer_num_bytes 	8960
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_659__dram_size_num_bytes 	21280
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_659__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_659__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_659__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_659__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_659__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_659__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_659__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_659__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_659__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_659__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_4 HMB_input __pvcn_670_ */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_670__cnngen_demangled_name 	"__pvcn_670_"
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_670__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_670__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_670__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_670__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_670__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_670__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_670__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_670__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_670__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_670__byte_offset 	240	/* 0x000000f0 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_670__vbase_byte_offset 	244	/* 0x000000f4 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_670__dbase_byte_offset 	268	/* 0x0000010c */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_670__dpitchm1_byte_offset 	272	/* 0x00000110 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_670__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_670__vwidth_minus_one_byte_offset 	256	/* 0x00000100 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_670__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_670__vheight_minus_one_byte_offset 	258	/* 0x00000102 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_670__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_670__drotate_bit_offset 	2118	/* 0x00000846 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_670__hflip_bit_offset 	2119	/* 0x00000847 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_670__vflip_bit_offset 	2120	/* 0x00000848 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_670__dflip_bit_offset 	2121	/* 0x00000849 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_670__pflip_bit_offset 	2122	/* 0x0000084a */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_670__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_670__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_670__D 	8
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_670__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_670__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_670__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_670__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_670__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_670__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_670__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_670__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_670__vmem_buffer_num_bytes 	10240
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_670__dram_size_num_bytes 	24320
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_670__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_670__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_670__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_670__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_670__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_670__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_670__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_670__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_670__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_670__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_4 HMB_input __pvcn_648_ */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_648__cnngen_demangled_name 	"__pvcn_648_"
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_648__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_648__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_648__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_648__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_648__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_648__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_648__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_648__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_648__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_648__byte_offset 	280	/* 0x00000118 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_648__vbase_byte_offset 	284	/* 0x0000011c */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_648__dbase_byte_offset 	308	/* 0x00000134 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_648__dpitchm1_byte_offset 	312	/* 0x00000138 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_648__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_648__vwidth_minus_one_byte_offset 	296	/* 0x00000128 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_648__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_648__vheight_minus_one_byte_offset 	298	/* 0x0000012a */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_648__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_648__drotate_bit_offset 	2438	/* 0x00000986 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_648__hflip_bit_offset 	2439	/* 0x00000987 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_648__vflip_bit_offset 	2440	/* 0x00000988 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_648__dflip_bit_offset 	2441	/* 0x00000989 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_648__pflip_bit_offset 	2442	/* 0x0000098a */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_648__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_648__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_648__D 	7
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_648__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_648__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_648__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_648__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_648__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_648__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_648__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_648__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_648__vmem_buffer_num_bytes 	8960
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_648__dram_size_num_bytes 	21280
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_648__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_648__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_648__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_648__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_648__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_648__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_648__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_648__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_648__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_648__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_4 HMB_input __pvcn_637_ */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_637__cnngen_demangled_name 	"__pvcn_637_"
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_637__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_637__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_637__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_637__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_637__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_637__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_637__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_637__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_637__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_637__byte_offset 	320	/* 0x00000140 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_637__vbase_byte_offset 	324	/* 0x00000144 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_637__dbase_byte_offset 	348	/* 0x0000015c */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_637__dpitchm1_byte_offset 	352	/* 0x00000160 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_637__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_637__vwidth_minus_one_byte_offset 	336	/* 0x00000150 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_637__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_637__vheight_minus_one_byte_offset 	338	/* 0x00000152 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_637__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_637__drotate_bit_offset 	2758	/* 0x00000ac6 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_637__hflip_bit_offset 	2759	/* 0x00000ac7 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_637__vflip_bit_offset 	2760	/* 0x00000ac8 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_637__dflip_bit_offset 	2761	/* 0x00000ac9 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_637__pflip_bit_offset 	2762	/* 0x00000aca */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_637__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_637__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_637__D 	8
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_637__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_637__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_637__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_637__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_637__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_637__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_637__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_637__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_637__vmem_buffer_num_bytes 	10240
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_637__dram_size_num_bytes 	24320
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_637__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_637__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_637__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_637__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_637__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_637__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_637__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_637__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_637__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_637__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_4 HMB_input __pvcn_571_ */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_571__cnngen_demangled_name 	"__pvcn_571_"
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_571__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_571__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_571__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_571__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_571__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_571__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_571__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_571__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_571__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_571__byte_offset 	360	/* 0x00000168 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_571__vbase_byte_offset 	364	/* 0x0000016c */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_571__dbase_byte_offset 	388	/* 0x00000184 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_571__dpitchm1_byte_offset 	392	/* 0x00000188 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_571__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_571__vwidth_minus_one_byte_offset 	376	/* 0x00000178 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_571__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_571__vheight_minus_one_byte_offset 	378	/* 0x0000017a */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_571__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_571__drotate_bit_offset 	3078	/* 0x00000c06 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_571__hflip_bit_offset 	3079	/* 0x00000c07 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_571__vflip_bit_offset 	3080	/* 0x00000c08 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_571__dflip_bit_offset 	3081	/* 0x00000c09 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_571__pflip_bit_offset 	3082	/* 0x00000c0a */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_571__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_571__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_571__D 	7
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_571__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_571__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_571__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_571__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_571__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_571__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_571__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_571__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_571__vmem_buffer_num_bytes 	8960
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_571__dram_size_num_bytes 	21280
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_571__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_571__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_571__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_571__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_571__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_571__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_571__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_571__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_571__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_571__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_4 HMB_input __pvcn_615_ */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_615__cnngen_demangled_name 	"__pvcn_615_"
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_615__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_615__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_615__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_615__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_615__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_615__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_615__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_615__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_615__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_615__byte_offset 	400	/* 0x00000190 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_615__vbase_byte_offset 	404	/* 0x00000194 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_615__dbase_byte_offset 	428	/* 0x000001ac */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_615__dpitchm1_byte_offset 	432	/* 0x000001b0 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_615__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_615__vwidth_minus_one_byte_offset 	416	/* 0x000001a0 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_615__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_615__vheight_minus_one_byte_offset 	418	/* 0x000001a2 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_615__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_615__drotate_bit_offset 	3398	/* 0x00000d46 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_615__hflip_bit_offset 	3399	/* 0x00000d47 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_615__vflip_bit_offset 	3400	/* 0x00000d48 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_615__dflip_bit_offset 	3401	/* 0x00000d49 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_615__pflip_bit_offset 	3402	/* 0x00000d4a */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_615__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_615__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_615__D 	9
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_615__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_615__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_615__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_615__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_615__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_615__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_615__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_615__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_615__vmem_buffer_num_bytes 	11520
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_615__dram_size_num_bytes 	27360
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_615__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_615__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_615__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_615__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_615__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_615__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_615__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_615__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_615__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_615__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_4 HMB_input __pvcn_560_ */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_560__cnngen_demangled_name 	"__pvcn_560_"
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_560__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_560__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_560__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_560__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_560__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_560__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_560__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_560__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_560__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_560__byte_offset 	440	/* 0x000001b8 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_560__vbase_byte_offset 	444	/* 0x000001bc */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_560__dbase_byte_offset 	468	/* 0x000001d4 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_560__dpitchm1_byte_offset 	472	/* 0x000001d8 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_560__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_560__vwidth_minus_one_byte_offset 	456	/* 0x000001c8 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_560__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_560__vheight_minus_one_byte_offset 	458	/* 0x000001ca */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_560__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_560__drotate_bit_offset 	3718	/* 0x00000e86 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_560__hflip_bit_offset 	3719	/* 0x00000e87 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_560__vflip_bit_offset 	3720	/* 0x00000e88 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_560__dflip_bit_offset 	3721	/* 0x00000e89 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_560__pflip_bit_offset 	3722	/* 0x00000e8a */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_560__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_560__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_560__D 	7
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_560__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_560__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_560__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_560__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_560__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_560__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_560__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_560__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_560__vmem_buffer_num_bytes 	8960
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_560__dram_size_num_bytes 	21280
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_560__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_560__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_560__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_560__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_560__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_560__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_560__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_560__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_560__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_560__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_4 HMB_input __pvcn_593_ */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_593__cnngen_demangled_name 	"__pvcn_593_"
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_593__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_593__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_593__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_593__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_593__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_593__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_593__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_593__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_593__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_593__byte_offset 	480	/* 0x000001e0 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_593__vbase_byte_offset 	484	/* 0x000001e4 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_593__dbase_byte_offset 	508	/* 0x000001fc */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_593__dpitchm1_byte_offset 	512	/* 0x00000200 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_593__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_593__vwidth_minus_one_byte_offset 	496	/* 0x000001f0 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_593__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_593__vheight_minus_one_byte_offset 	498	/* 0x000001f2 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_593__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_593__drotate_bit_offset 	4038	/* 0x00000fc6 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_593__hflip_bit_offset 	4039	/* 0x00000fc7 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_593__vflip_bit_offset 	4040	/* 0x00000fc8 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_593__dflip_bit_offset 	4041	/* 0x00000fc9 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_593__pflip_bit_offset 	4042	/* 0x00000fca */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_593__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_593__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_593__D 	7
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_593__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_593__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_593__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_593__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_593__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_593__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_593__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_593__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_593__vmem_buffer_num_bytes 	8960
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_593__dram_size_num_bytes 	21280
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_593__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_593__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_593__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_593__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_593__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_593__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_593__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_593__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_593__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_593__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_4 HMB_input __pvcn_582_ */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_582__cnngen_demangled_name 	"__pvcn_582_"
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_582__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_582__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_582__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_582__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_582__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_582__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_582__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_582__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_582__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_582__byte_offset 	520	/* 0x00000208 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_582__vbase_byte_offset 	524	/* 0x0000020c */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_582__dbase_byte_offset 	548	/* 0x00000224 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_582__dpitchm1_byte_offset 	552	/* 0x00000228 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_582__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_582__vwidth_minus_one_byte_offset 	536	/* 0x00000218 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_582__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_582__vheight_minus_one_byte_offset 	538	/* 0x0000021a */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_582__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_582__drotate_bit_offset 	4358	/* 0x00001106 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_582__hflip_bit_offset 	4359	/* 0x00001107 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_582__vflip_bit_offset 	4360	/* 0x00001108 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_582__dflip_bit_offset 	4361	/* 0x00001109 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_582__pflip_bit_offset 	4362	/* 0x0000110a */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_582__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_582__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_582__D 	8
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_582__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_582__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_582__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_582__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_582__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_582__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_582__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_582__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_582__vmem_buffer_num_bytes 	10240
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_582__dram_size_num_bytes 	24320
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_582__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_582__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_582__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_582__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_582__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_582__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_582__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_582__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_582__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_582__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_4 HMB_input __pvcn_549_ */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_549__cnngen_demangled_name 	"__pvcn_549_"
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_549__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_549__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_549__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_549__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_549__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_549__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_549__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_549__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_549__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_549__byte_offset 	560	/* 0x00000230 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_549__vbase_byte_offset 	564	/* 0x00000234 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_549__dbase_byte_offset 	588	/* 0x0000024c */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_549__dpitchm1_byte_offset 	592	/* 0x00000250 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_549__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_549__vwidth_minus_one_byte_offset 	576	/* 0x00000240 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_549__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_549__vheight_minus_one_byte_offset 	578	/* 0x00000242 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_549__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_549__drotate_bit_offset 	4678	/* 0x00001246 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_549__hflip_bit_offset 	4679	/* 0x00001247 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_549__vflip_bit_offset 	4680	/* 0x00001248 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_549__dflip_bit_offset 	4681	/* 0x00001249 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_549__pflip_bit_offset 	4682	/* 0x0000124a */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_549__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_549__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_549__D 	7
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_549__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_549__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_549__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_549__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_549__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_549__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_549__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_549__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_549__vmem_buffer_num_bytes 	8960
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_549__dram_size_num_bytes 	21280
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_549__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_549__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_549__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_549__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_549__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_549__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_549__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_549__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_549__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_549__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_4 SMB_input __pvcn_701_ */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_701__cnngen_demangled_name 	"__pvcn_701_"
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_701__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_701__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_701__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_701__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_701__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_701__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_701__byte_offset 	596	/* 0x00000254 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_701__vbase_byte_offset 	600	/* 0x00000258 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_701__dbase_byte_offset 	624	/* 0x00000270 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_701__dpitchm1_byte_offset 	628	/* 0x00000274 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_701__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_701__W 	88
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_701__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_701__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_701__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_701__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_701__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_701__dpitch_num_bytes 	128
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_701__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_701__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_701__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_701__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_701__vmem_buffer_num_bytes 	88
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_701__dram_size_num_bytes 	128

/* mnet_ssd_adas_flex_pic_prim_split_4 SMB_input __pvcn_702_ */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_702__cnngen_demangled_name 	"__pvcn_702_"
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_702__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_702__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_702__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_702__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_702__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_702__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_702__byte_offset 	636	/* 0x0000027c */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_702__vbase_byte_offset 	640	/* 0x00000280 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_702__dbase_byte_offset 	664	/* 0x00000298 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_702__dpitchm1_byte_offset 	668	/* 0x0000029c */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_702__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_702__W 	68
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_702__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_702__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_702__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_702__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_702__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_702__dpitch_num_bytes 	128
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_702__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_702__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_702__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_702__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_702__vmem_buffer_num_bytes 	68
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_702__dram_size_num_bytes 	128

/* mnet_ssd_adas_flex_pic_prim_split_4 SMB_input __pvcn_712_ */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_712__cnngen_demangled_name 	"__pvcn_712_"
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_712__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_712__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_712__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_712__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_712__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_712__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_712__byte_offset 	676	/* 0x000002a4 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_712__vbase_byte_offset 	680	/* 0x000002a8 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_712__dbase_byte_offset 	704	/* 0x000002c0 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_712__dpitchm1_byte_offset 	708	/* 0x000002c4 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_712__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_712__W 	96
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_712__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_712__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_712__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_712__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_712__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_712__dpitch_num_bytes 	128
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_712__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_712__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_712__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_712__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_712__vmem_buffer_num_bytes 	96
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_712__dram_size_num_bytes 	128

/* mnet_ssd_adas_flex_pic_prim_split_4 SMB_input __pvcn_713_ */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_713__cnngen_demangled_name 	"__pvcn_713_"
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_713__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_713__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_713__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_713__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_713__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_713__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_713__byte_offset 	716	/* 0x000002cc */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_713__vbase_byte_offset 	720	/* 0x000002d0 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_713__dbase_byte_offset 	744	/* 0x000002e8 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_713__dpitchm1_byte_offset 	748	/* 0x000002ec */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_713__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_713__W 	72
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_713__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_713__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_713__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_713__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_713__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_713__dpitch_num_bytes 	128
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_713__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_713__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_713__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_713__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_713__vmem_buffer_num_bytes 	72
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_713__dram_size_num_bytes 	128

/* mnet_ssd_adas_flex_pic_prim_split_4 SMB_input __pvcn_723_ */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_723__cnngen_demangled_name 	"__pvcn_723_"
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_723__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_723__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_723__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_723__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_723__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_723__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_723__byte_offset 	756	/* 0x000002f4 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_723__vbase_byte_offset 	760	/* 0x000002f8 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_723__dbase_byte_offset 	784	/* 0x00000310 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_723__dpitchm1_byte_offset 	788	/* 0x00000314 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_723__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_723__W 	84
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_723__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_723__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_723__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_723__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_723__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_723__dpitch_num_bytes 	128
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_723__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_723__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_723__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_723__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_723__vmem_buffer_num_bytes 	84
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_723__dram_size_num_bytes 	128

/* mnet_ssd_adas_flex_pic_prim_split_4 SMB_input __pvcn_724_ */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_724__cnngen_demangled_name 	"__pvcn_724_"
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_724__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_724__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_724__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_724__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_724__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_724__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_724__byte_offset 	796	/* 0x0000031c */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_724__vbase_byte_offset 	800	/* 0x00000320 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_724__dbase_byte_offset 	824	/* 0x00000338 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_724__dpitchm1_byte_offset 	828	/* 0x0000033c */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_724__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_724__W 	60
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_724__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_724__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_724__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_724__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_724__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_724__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_724__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_724__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_724__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_724__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_724__vmem_buffer_num_bytes 	60
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_724__dram_size_num_bytes 	64

/* mnet_ssd_adas_flex_pic_prim_split_4 HMB_output __pvcn_543_ */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_543__cnngen_demangled_name 	"__pvcn_543_"
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_543__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_543__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_543__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_543__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_543__byte_offset 	848	/* 0x00000350 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_543__vbase_byte_offset 	852	/* 0x00000354 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_543__dbase_byte_offset 	876	/* 0x0000036c */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_543__dpitchm1_byte_offset 	880	/* 0x00000370 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_543__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_543__vwidth_minus_one_byte_offset 	864	/* 0x00000360 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_543__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_543__vheight_minus_one_byte_offset 	866	/* 0x00000362 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_543__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_543__drotate_bit_offset 	6982	/* 0x00001b46 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_543__hflip_bit_offset 	6983	/* 0x00001b47 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_543__vflip_bit_offset 	6984	/* 0x00001b48 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_543__dflip_bit_offset 	6985	/* 0x00001b49 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_543__pflip_bit_offset 	6986	/* 0x00001b4a */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_543__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_543__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_543__D 	7
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_543__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_543__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_543__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_543__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_543__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_543__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_543__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_543__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_543__vmem_buffer_num_bytes 	8960
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_543__dram_size_num_bytes 	21280
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_543__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_543__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_543__expoffset 	13
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_543__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_543__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_543__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_543__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_543__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_543__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_543__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_4 HMB_output __pvcn_554_ */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_554__cnngen_demangled_name 	"__pvcn_554_"
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_554__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_554__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_554__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_554__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_554__byte_offset 	896	/* 0x00000380 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_554__vbase_byte_offset 	900	/* 0x00000384 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_554__dbase_byte_offset 	924	/* 0x0000039c */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_554__dpitchm1_byte_offset 	928	/* 0x000003a0 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_554__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_554__vwidth_minus_one_byte_offset 	912	/* 0x00000390 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_554__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_554__vheight_minus_one_byte_offset 	914	/* 0x00000392 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_554__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_554__drotate_bit_offset 	7366	/* 0x00001cc6 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_554__hflip_bit_offset 	7367	/* 0x00001cc7 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_554__vflip_bit_offset 	7368	/* 0x00001cc8 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_554__dflip_bit_offset 	7369	/* 0x00001cc9 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_554__pflip_bit_offset 	7370	/* 0x00001cca */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_554__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_554__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_554__D 	7
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_554__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_554__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_554__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_554__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_554__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_554__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_554__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_554__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_554__vmem_buffer_num_bytes 	8960
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_554__dram_size_num_bytes 	21280
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_554__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_554__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_554__expoffset 	13
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_554__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_554__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_554__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_554__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_554__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_554__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_554__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_4 HMB_output __pvcn_565_ */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_565__cnngen_demangled_name 	"__pvcn_565_"
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_565__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_565__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_565__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_565__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_565__byte_offset 	944	/* 0x000003b0 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_565__vbase_byte_offset 	948	/* 0x000003b4 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_565__dbase_byte_offset 	972	/* 0x000003cc */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_565__dpitchm1_byte_offset 	976	/* 0x000003d0 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_565__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_565__vwidth_minus_one_byte_offset 	960	/* 0x000003c0 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_565__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_565__vheight_minus_one_byte_offset 	962	/* 0x000003c2 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_565__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_565__drotate_bit_offset 	7750	/* 0x00001e46 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_565__hflip_bit_offset 	7751	/* 0x00001e47 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_565__vflip_bit_offset 	7752	/* 0x00001e48 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_565__dflip_bit_offset 	7753	/* 0x00001e49 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_565__pflip_bit_offset 	7754	/* 0x00001e4a */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_565__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_565__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_565__D 	7
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_565__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_565__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_565__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_565__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_565__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_565__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_565__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_565__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_565__vmem_buffer_num_bytes 	8960
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_565__dram_size_num_bytes 	21280
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_565__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_565__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_565__expoffset 	13
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_565__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_565__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_565__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_565__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_565__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_565__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_565__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_4 HMB_output __pvcn_576_ */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_576__cnngen_demangled_name 	"__pvcn_576_"
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_576__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_576__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_576__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_576__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_576__byte_offset 	992	/* 0x000003e0 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_576__vbase_byte_offset 	996	/* 0x000003e4 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_576__dbase_byte_offset 	1020	/* 0x000003fc */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_576__dpitchm1_byte_offset 	1024	/* 0x00000400 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_576__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_576__vwidth_minus_one_byte_offset 	1008	/* 0x000003f0 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_576__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_576__vheight_minus_one_byte_offset 	1010	/* 0x000003f2 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_576__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_576__drotate_bit_offset 	8134	/* 0x00001fc6 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_576__hflip_bit_offset 	8135	/* 0x00001fc7 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_576__vflip_bit_offset 	8136	/* 0x00001fc8 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_576__dflip_bit_offset 	8137	/* 0x00001fc9 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_576__pflip_bit_offset 	8138	/* 0x00001fca */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_576__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_576__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_576__D 	8
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_576__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_576__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_576__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_576__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_576__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_576__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_576__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_576__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_576__vmem_buffer_num_bytes 	10240
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_576__dram_size_num_bytes 	24320
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_576__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_576__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_576__expoffset 	13
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_576__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_576__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_576__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_576__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_576__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_576__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_576__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_4 HMB_output __pvcn_587_ */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_587__cnngen_demangled_name 	"__pvcn_587_"
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_587__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_587__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_587__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_587__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_587__byte_offset 	1040	/* 0x00000410 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_587__vbase_byte_offset 	1044	/* 0x00000414 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_587__dbase_byte_offset 	1068	/* 0x0000042c */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_587__dpitchm1_byte_offset 	1072	/* 0x00000430 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_587__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_587__vwidth_minus_one_byte_offset 	1056	/* 0x00000420 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_587__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_587__vheight_minus_one_byte_offset 	1058	/* 0x00000422 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_587__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_587__drotate_bit_offset 	8518	/* 0x00002146 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_587__hflip_bit_offset 	8519	/* 0x00002147 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_587__vflip_bit_offset 	8520	/* 0x00002148 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_587__dflip_bit_offset 	8521	/* 0x00002149 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_587__pflip_bit_offset 	8522	/* 0x0000214a */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_587__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_587__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_587__D 	7
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_587__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_587__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_587__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_587__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_587__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_587__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_587__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_587__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_587__vmem_buffer_num_bytes 	8960
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_587__dram_size_num_bytes 	21280
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_587__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_587__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_587__expoffset 	13
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_587__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_587__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_587__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_587__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_587__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_587__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_587__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_4 HMB_output __pvcn_598_ */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_598__cnngen_demangled_name 	"__pvcn_598_"
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_598__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_598__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_598__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_598__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_598__byte_offset 	1088	/* 0x00000440 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_598__vbase_byte_offset 	1092	/* 0x00000444 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_598__dbase_byte_offset 	1116	/* 0x0000045c */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_598__dpitchm1_byte_offset 	1120	/* 0x00000460 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_598__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_598__vwidth_minus_one_byte_offset 	1104	/* 0x00000450 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_598__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_598__vheight_minus_one_byte_offset 	1106	/* 0x00000452 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_598__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_598__drotate_bit_offset 	8902	/* 0x000022c6 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_598__hflip_bit_offset 	8903	/* 0x000022c7 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_598__vflip_bit_offset 	8904	/* 0x000022c8 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_598__dflip_bit_offset 	8905	/* 0x000022c9 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_598__pflip_bit_offset 	8906	/* 0x000022ca */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_598__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_598__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_598__D 	8
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_598__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_598__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_598__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_598__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_598__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_598__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_598__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_598__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_598__vmem_buffer_num_bytes 	10240
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_598__dram_size_num_bytes 	24320
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_598__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_598__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_598__expoffset 	13
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_598__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_598__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_598__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_598__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_598__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_598__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_598__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_4 HMB_output __pvcn_609_ */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_609__cnngen_demangled_name 	"__pvcn_609_"
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_609__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_609__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_609__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_609__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_609__byte_offset 	1136	/* 0x00000470 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_609__vbase_byte_offset 	1140	/* 0x00000474 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_609__dbase_byte_offset 	1164	/* 0x0000048c */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_609__dpitchm1_byte_offset 	1168	/* 0x00000490 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_609__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_609__vwidth_minus_one_byte_offset 	1152	/* 0x00000480 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_609__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_609__vheight_minus_one_byte_offset 	1154	/* 0x00000482 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_609__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_609__drotate_bit_offset 	9286	/* 0x00002446 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_609__hflip_bit_offset 	9287	/* 0x00002447 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_609__vflip_bit_offset 	9288	/* 0x00002448 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_609__dflip_bit_offset 	9289	/* 0x00002449 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_609__pflip_bit_offset 	9290	/* 0x0000244a */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_609__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_609__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_609__D 	9
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_609__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_609__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_609__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_609__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_609__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_609__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_609__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_609__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_609__vmem_buffer_num_bytes 	11520
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_609__dram_size_num_bytes 	27360
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_609__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_609__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_609__expoffset 	13
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_609__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_609__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_609__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_609__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_609__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_609__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_609__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_4 HMB_output __pvcn_620_ */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_620__cnngen_demangled_name 	"__pvcn_620_"
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_620__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_620__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_620__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_620__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_620__byte_offset 	1184	/* 0x000004a0 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_620__vbase_byte_offset 	1188	/* 0x000004a4 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_620__dbase_byte_offset 	1212	/* 0x000004bc */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_620__dpitchm1_byte_offset 	1216	/* 0x000004c0 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_620__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_620__vwidth_minus_one_byte_offset 	1200	/* 0x000004b0 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_620__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_620__vheight_minus_one_byte_offset 	1202	/* 0x000004b2 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_620__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_620__drotate_bit_offset 	9670	/* 0x000025c6 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_620__hflip_bit_offset 	9671	/* 0x000025c7 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_620__vflip_bit_offset 	9672	/* 0x000025c8 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_620__dflip_bit_offset 	9673	/* 0x000025c9 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_620__pflip_bit_offset 	9674	/* 0x000025ca */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_620__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_620__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_620__D 	8
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_620__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_620__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_620__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_620__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_620__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_620__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_620__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_620__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_620__vmem_buffer_num_bytes 	10240
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_620__dram_size_num_bytes 	24320
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_620__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_620__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_620__expoffset 	13
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_620__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_620__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_620__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_620__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_620__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_620__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_620__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_4 HMB_output __pvcn_631_ */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_631__cnngen_demangled_name 	"__pvcn_631_"
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_631__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_631__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_631__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_631__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_631__byte_offset 	1232	/* 0x000004d0 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_631__vbase_byte_offset 	1236	/* 0x000004d4 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_631__dbase_byte_offset 	1260	/* 0x000004ec */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_631__dpitchm1_byte_offset 	1264	/* 0x000004f0 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_631__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_631__vwidth_minus_one_byte_offset 	1248	/* 0x000004e0 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_631__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_631__vheight_minus_one_byte_offset 	1250	/* 0x000004e2 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_631__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_631__drotate_bit_offset 	10054	/* 0x00002746 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_631__hflip_bit_offset 	10055	/* 0x00002747 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_631__vflip_bit_offset 	10056	/* 0x00002748 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_631__dflip_bit_offset 	10057	/* 0x00002749 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_631__pflip_bit_offset 	10058	/* 0x0000274a */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_631__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_631__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_631__D 	8
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_631__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_631__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_631__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_631__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_631__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_631__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_631__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_631__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_631__vmem_buffer_num_bytes 	10240
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_631__dram_size_num_bytes 	24320
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_631__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_631__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_631__expoffset 	13
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_631__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_631__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_631__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_631__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_631__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_631__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_631__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_4 HMB_output __pvcn_642_ */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_642__cnngen_demangled_name 	"__pvcn_642_"
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_642__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_642__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_642__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_642__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_642__byte_offset 	1280	/* 0x00000500 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_642__vbase_byte_offset 	1284	/* 0x00000504 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_642__dbase_byte_offset 	1308	/* 0x0000051c */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_642__dpitchm1_byte_offset 	1312	/* 0x00000520 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_642__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_642__vwidth_minus_one_byte_offset 	1296	/* 0x00000510 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_642__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_642__vheight_minus_one_byte_offset 	1298	/* 0x00000512 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_642__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_642__drotate_bit_offset 	10438	/* 0x000028c6 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_642__hflip_bit_offset 	10439	/* 0x000028c7 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_642__vflip_bit_offset 	10440	/* 0x000028c8 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_642__dflip_bit_offset 	10441	/* 0x000028c9 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_642__pflip_bit_offset 	10442	/* 0x000028ca */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_642__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_642__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_642__D 	7
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_642__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_642__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_642__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_642__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_642__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_642__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_642__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_642__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_642__vmem_buffer_num_bytes 	8960
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_642__dram_size_num_bytes 	21280
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_642__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_642__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_642__expoffset 	13
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_642__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_642__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_642__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_642__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_642__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_642__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_642__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_4 HMB_output __pvcn_653_ */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_653__cnngen_demangled_name 	"__pvcn_653_"
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_653__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_653__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_653__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_653__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_653__byte_offset 	1328	/* 0x00000530 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_653__vbase_byte_offset 	1332	/* 0x00000534 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_653__dbase_byte_offset 	1356	/* 0x0000054c */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_653__dpitchm1_byte_offset 	1360	/* 0x00000550 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_653__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_653__vwidth_minus_one_byte_offset 	1344	/* 0x00000540 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_653__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_653__vheight_minus_one_byte_offset 	1346	/* 0x00000542 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_653__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_653__drotate_bit_offset 	10822	/* 0x00002a46 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_653__hflip_bit_offset 	10823	/* 0x00002a47 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_653__vflip_bit_offset 	10824	/* 0x00002a48 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_653__dflip_bit_offset 	10825	/* 0x00002a49 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_653__pflip_bit_offset 	10826	/* 0x00002a4a */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_653__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_653__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_653__D 	7
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_653__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_653__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_653__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_653__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_653__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_653__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_653__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_653__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_653__vmem_buffer_num_bytes 	8960
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_653__dram_size_num_bytes 	21280
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_653__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_653__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_653__expoffset 	13
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_653__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_653__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_653__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_653__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_653__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_653__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_653__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_4 HMB_output __pvcn_664_ */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_664__cnngen_demangled_name 	"__pvcn_664_"
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_664__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_664__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_664__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_664__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_664__byte_offset 	1376	/* 0x00000560 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_664__vbase_byte_offset 	1380	/* 0x00000564 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_664__dbase_byte_offset 	1404	/* 0x0000057c */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_664__dpitchm1_byte_offset 	1408	/* 0x00000580 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_664__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_664__vwidth_minus_one_byte_offset 	1392	/* 0x00000570 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_664__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_664__vheight_minus_one_byte_offset 	1394	/* 0x00000572 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_664__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_664__drotate_bit_offset 	11206	/* 0x00002bc6 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_664__hflip_bit_offset 	11207	/* 0x00002bc7 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_664__vflip_bit_offset 	11208	/* 0x00002bc8 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_664__dflip_bit_offset 	11209	/* 0x00002bc9 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_664__pflip_bit_offset 	11210	/* 0x00002bca */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_664__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_664__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_664__D 	8
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_664__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_664__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_664__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_664__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_664__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_664__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_664__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_664__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_664__vmem_buffer_num_bytes 	10240
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_664__dram_size_num_bytes 	24320
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_664__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_664__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_664__expoffset 	13
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_664__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_664__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_664__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_664__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_664__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_664__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_664__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_4 HMB_output __pvcn_675_ */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_675__cnngen_demangled_name 	"__pvcn_675_"
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_675__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_675__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_675__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_675__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_675__byte_offset 	1424	/* 0x00000590 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_675__vbase_byte_offset 	1428	/* 0x00000594 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_675__dbase_byte_offset 	1452	/* 0x000005ac */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_675__dpitchm1_byte_offset 	1456	/* 0x000005b0 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_675__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_675__vwidth_minus_one_byte_offset 	1440	/* 0x000005a0 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_675__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_675__vheight_minus_one_byte_offset 	1442	/* 0x000005a2 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_675__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_675__drotate_bit_offset 	11590	/* 0x00002d46 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_675__hflip_bit_offset 	11591	/* 0x00002d47 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_675__vflip_bit_offset 	11592	/* 0x00002d48 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_675__dflip_bit_offset 	11593	/* 0x00002d49 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_675__pflip_bit_offset 	11594	/* 0x00002d4a */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_675__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_675__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_675__D 	9
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_675__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_675__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_675__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_675__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_675__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_675__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_675__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_675__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_675__vmem_buffer_num_bytes 	11520
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_675__dram_size_num_bytes 	27360
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_675__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_675__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_675__expoffset 	13
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_675__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_675__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_675__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_675__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_675__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_675__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_675__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_4 HMB_output __pvcn_686_ */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_686__cnngen_demangled_name 	"__pvcn_686_"
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_686__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_686__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_686__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_686__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_686__byte_offset 	1472	/* 0x000005c0 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_686__vbase_byte_offset 	1476	/* 0x000005c4 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_686__dbase_byte_offset 	1500	/* 0x000005dc */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_686__dpitchm1_byte_offset 	1504	/* 0x000005e0 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_686__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_686__vwidth_minus_one_byte_offset 	1488	/* 0x000005d0 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_686__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_686__vheight_minus_one_byte_offset 	1490	/* 0x000005d2 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_686__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_686__drotate_bit_offset 	11974	/* 0x00002ec6 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_686__hflip_bit_offset 	11975	/* 0x00002ec7 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_686__vflip_bit_offset 	11976	/* 0x00002ec8 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_686__dflip_bit_offset 	11977	/* 0x00002ec9 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_686__pflip_bit_offset 	11978	/* 0x00002eca */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_686__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_686__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_686__D 	7
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_686__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_686__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_686__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_686__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_686__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_686__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_686__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_686__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_686__vmem_buffer_num_bytes 	8960
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_686__dram_size_num_bytes 	21280
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_686__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_686__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_686__expoffset 	13
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_686__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_686__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_686__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_686__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_686__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_686__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_686__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_4 HMB_output __pvcn_703_ */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_703__cnngen_demangled_name 	"__pvcn_703_"
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_703__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_703__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_703__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_703__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_703__byte_offset 	1528	/* 0x000005f8 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_703__vbase_byte_offset 	1532	/* 0x000005fc */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_703__dbase_byte_offset 	1556	/* 0x00000614 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_703__dpitchm1_byte_offset 	1560	/* 0x00000618 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_703__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_703__vwidth_minus_one_byte_offset 	1544	/* 0x00000608 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_703__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_703__vheight_minus_one_byte_offset 	1546	/* 0x0000060a */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_703__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_703__drotate_bit_offset 	12422	/* 0x00003086 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_703__hflip_bit_offset 	12423	/* 0x00003087 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_703__vflip_bit_offset 	12424	/* 0x00003088 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_703__dflip_bit_offset 	12425	/* 0x00003089 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_703__pflip_bit_offset 	12426	/* 0x0000308a */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_703__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_703__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_703__D 	7
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_703__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_703__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_703__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_703__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_703__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_703__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_703__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_703__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_703__vmem_buffer_num_bytes 	8960
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_703__dram_size_num_bytes 	21280
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_703__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_703__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_703__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_703__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_703__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_703__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_703__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_703__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_703__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_703__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_4 HMB_output __pvcn_714_ */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_714__cnngen_demangled_name 	"__pvcn_714_"
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_714__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_714__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_714__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_714__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_714__byte_offset 	1584	/* 0x00000630 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_714__vbase_byte_offset 	1588	/* 0x00000634 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_714__dbase_byte_offset 	1612	/* 0x0000064c */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_714__dpitchm1_byte_offset 	1616	/* 0x00000650 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_714__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_714__vwidth_minus_one_byte_offset 	1600	/* 0x00000640 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_714__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_714__vheight_minus_one_byte_offset 	1602	/* 0x00000642 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_714__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_714__drotate_bit_offset 	12870	/* 0x00003246 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_714__hflip_bit_offset 	12871	/* 0x00003247 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_714__vflip_bit_offset 	12872	/* 0x00003248 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_714__dflip_bit_offset 	12873	/* 0x00003249 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_714__pflip_bit_offset 	12874	/* 0x0000324a */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_714__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_714__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_714__D 	8
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_714__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_714__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_714__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_714__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_714__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_714__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_714__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_714__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_714__vmem_buffer_num_bytes 	5120
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_714__dram_size_num_bytes 	24320
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_714__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_714__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_714__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_714__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_714__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_714__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_714__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_714__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_714__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_714__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_4 HMB_output __pvcn_725_ */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_725__cnngen_demangled_name 	"__pvcn_725_"
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_725__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_725__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_725__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_725__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_725__byte_offset 	1640	/* 0x00000668 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_725__vbase_byte_offset 	1644	/* 0x0000066c */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_725__dbase_byte_offset 	1668	/* 0x00000684 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_725__dpitchm1_byte_offset 	1672	/* 0x00000688 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_725__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_725__vwidth_minus_one_byte_offset 	1656	/* 0x00000678 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_725__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_725__vheight_minus_one_byte_offset 	1658	/* 0x0000067a */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_725__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_725__drotate_bit_offset 	13318	/* 0x00003406 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_725__hflip_bit_offset 	13319	/* 0x00003407 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_725__vflip_bit_offset 	13320	/* 0x00003408 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_725__dflip_bit_offset 	13321	/* 0x00003409 */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_725__pflip_bit_offset 	13322	/* 0x0000340a */
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_725__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_725__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_725__D 	6
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_725__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_725__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_725__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_725__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_725__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_725__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_725__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_725__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_725__vmem_buffer_num_bytes 	7680
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_725__dram_size_num_bytes 	18240
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_725__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_725__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_725__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_725__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_725__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_725__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_725__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_725__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_725__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___pvcn_725__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_4 SMB_output __vas_created_terminate__v0 */
#define mnet_ssd_adas_flex_pic_prim_split_4___vas_created_terminate__v0_cnngen_demangled_name 	"__vas_created_terminate__v0"
#define mnet_ssd_adas_flex_pic_prim_split_4___vas_created_terminate__v0_is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___vas_created_terminate__v0_is_bitvector 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___vas_created_terminate__v0_is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___vas_created_terminate__v0_is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___vas_created_terminate__v0_byte_offset 	1680	/* 0x00000690 */
#define mnet_ssd_adas_flex_pic_prim_split_4___vas_created_terminate__v0_vbase_byte_offset 	1684	/* 0x00000694 */
#define mnet_ssd_adas_flex_pic_prim_split_4___vas_created_terminate__v0_W 	256
#define mnet_ssd_adas_flex_pic_prim_split_4___vas_created_terminate__v0_H 	3
#define mnet_ssd_adas_flex_pic_prim_split_4___vas_created_terminate__v0_D 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___vas_created_terminate__v0_P 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___vas_created_terminate__v0_data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___vas_created_terminate__v0_denable 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___vas_created_terminate__v0_dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_4___vas_created_terminate__v0_vp_interleave_mode 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___vas_created_terminate__v0_tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___vas_created_terminate__v0_tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_4___vas_created_terminate__v0_vmem_buffer_num_bytes 	96


#endif /* mnet_ssd_adas_flex_pic_prim_split_4_H */
