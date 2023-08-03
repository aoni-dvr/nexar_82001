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
#ifndef mnet_ssd_adas_flex_pic_prim_split_19_H
#define mnet_ssd_adas_flex_pic_prim_split_19_H

#define vas_gen_header_version                  	7

/* dag name */
#define mnet_ssd_adas_flex_pic_prim_split_19_DAG_NAME 	"mnet_ssd_adas_flex_pic_prim_split_19"

/* VDG file info */
#define mnet_ssd_adas_flex_pic_prim_split_19_vdg_name 	"mnet_ssd_adas_flex_pic_split_19.vdg"

/* VDG DAG memory info */
#define mnet_ssd_adas_flex_pic_prim_split_19_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_19_byte_size 	1284	/* 0x00000504 */
#define mnet_ssd_adas_flex_pic_prim_split_19_preferred_dram_xfer_size 	1

/* mnet_ssd_adas_flex_pic_prim_split_19 VMEM info */
#define mnet_ssd_adas_flex_pic_prim_split_19_use_ping_pong_vmem 	0
#define mnet_ssd_adas_flex_pic_prim_split_19_VMEM_start 	4096	/* 0x00001000 */
#define mnet_ssd_adas_flex_pic_prim_split_19_VMEM_end 	512364	/* 0x0007d16c */
#define mnet_ssd_adas_flex_pic_prim_split_19_image_start 	260512	/* 0x0003f9a0 */
#define mnet_ssd_adas_flex_pic_prim_split_19_image_size 	253136	/* 0x0003dcd0 */
#define mnet_ssd_adas_flex_pic_prim_split_19_dagbin_start 	512364	/* 0x0007d16c */

/* mnet_ssd_adas_flex_pic_prim_split_19 DAG info */
#define mnet_ssd_adas_flex_pic_prim_split_19_estimated_cycles 	131984
#define mnet_ssd_adas_flex_pic_prim_split_19_ddi_byte_size 	0	/* 0x00000000 */

/*******************************************************************************
 * DAG byte offset info for HMB/SMB/VCB and their fields
 * in DAG 'mnet_ssd_adas_flex_pic_prim_split_19' in source file 'pre_split19_mnet_ssd_adas_flex_pic_split_0.vas'
 ******************************************************************************/
/* mnet_ssd_adas_flex_pic_prim_split_19 HMB_input __pvcn_2000_ */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2000__cnngen_demangled_name 	"__pvcn_2000_"
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2000__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2000__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2000__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2000__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2000__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2000__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2000__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2000__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2000__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2000__byte_offset 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2000__vbase_byte_offset 	4	/* 0x00000004 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2000__dbase_byte_offset 	28	/* 0x0000001c */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2000__dpitchm1_byte_offset 	32	/* 0x00000020 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2000__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2000__vwidth_minus_one_byte_offset 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2000__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2000__vheight_minus_one_byte_offset 	18	/* 0x00000012 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2000__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2000__drotate_bit_offset 	198	/* 0x000000c6 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2000__hflip_bit_offset 	199	/* 0x000000c7 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2000__vflip_bit_offset 	200	/* 0x000000c8 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2000__dflip_bit_offset 	201	/* 0x000000c9 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2000__pflip_bit_offset 	202	/* 0x000000ca */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2000__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2000__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2000__D 	35
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2000__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2000__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2000__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2000__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2000__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2000__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2000__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2000__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2000__vmem_buffer_num_bytes 	13440
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2000__dram_size_num_bytes 	31936
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2000__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2000__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2000__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2000__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2000__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2000__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2000__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2000__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2000__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2000__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_19 HMB_input __pvcn_1912_ */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1912__cnngen_demangled_name 	"__pvcn_1912_"
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1912__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1912__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1912__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1912__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1912__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1912__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1912__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1912__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1912__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1912__byte_offset 	40	/* 0x00000028 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1912__vbase_byte_offset 	44	/* 0x0000002c */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1912__dbase_byte_offset 	68	/* 0x00000044 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1912__dpitchm1_byte_offset 	72	/* 0x00000048 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1912__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1912__vwidth_minus_one_byte_offset 	56	/* 0x00000038 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1912__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1912__vheight_minus_one_byte_offset 	58	/* 0x0000003a */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1912__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1912__drotate_bit_offset 	518	/* 0x00000206 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1912__hflip_bit_offset 	519	/* 0x00000207 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1912__vflip_bit_offset 	520	/* 0x00000208 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1912__dflip_bit_offset 	521	/* 0x00000209 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1912__pflip_bit_offset 	522	/* 0x0000020a */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1912__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1912__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1912__D 	36
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1912__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1912__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1912__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1912__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1912__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1912__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1912__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1912__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1912__vmem_buffer_num_bytes 	13824
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1912__dram_size_num_bytes 	32832
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1912__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1912__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1912__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1912__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1912__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1912__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1912__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1912__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1912__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1912__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_19 HMB_input __pvcn_2022_ */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2022__cnngen_demangled_name 	"__pvcn_2022_"
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2022__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2022__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2022__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2022__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2022__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2022__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2022__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2022__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2022__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2022__byte_offset 	80	/* 0x00000050 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2022__vbase_byte_offset 	84	/* 0x00000054 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2022__dbase_byte_offset 	108	/* 0x0000006c */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2022__dpitchm1_byte_offset 	112	/* 0x00000070 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2022__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2022__vwidth_minus_one_byte_offset 	96	/* 0x00000060 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2022__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2022__vheight_minus_one_byte_offset 	98	/* 0x00000062 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2022__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2022__drotate_bit_offset 	838	/* 0x00000346 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2022__hflip_bit_offset 	839	/* 0x00000347 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2022__vflip_bit_offset 	840	/* 0x00000348 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2022__dflip_bit_offset 	841	/* 0x00000349 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2022__pflip_bit_offset 	842	/* 0x0000034a */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2022__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2022__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2022__D 	23
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2022__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2022__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2022__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2022__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2022__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2022__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2022__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2022__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2022__vmem_buffer_num_bytes 	8832
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2022__dram_size_num_bytes 	20992
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2022__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2022__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2022__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2022__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2022__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2022__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2022__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2022__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2022__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2022__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_19 HMB_input __pvcn_1901_ */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1901__cnngen_demangled_name 	"__pvcn_1901_"
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1901__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1901__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1901__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1901__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1901__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1901__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1901__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1901__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1901__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1901__byte_offset 	120	/* 0x00000078 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1901__vbase_byte_offset 	124	/* 0x0000007c */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1901__dbase_byte_offset 	148	/* 0x00000094 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1901__dpitchm1_byte_offset 	152	/* 0x00000098 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1901__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1901__vwidth_minus_one_byte_offset 	136	/* 0x00000088 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1901__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1901__vheight_minus_one_byte_offset 	138	/* 0x0000008a */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1901__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1901__drotate_bit_offset 	1158	/* 0x00000486 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1901__hflip_bit_offset 	1159	/* 0x00000487 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1901__vflip_bit_offset 	1160	/* 0x00000488 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1901__dflip_bit_offset 	1161	/* 0x00000489 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1901__pflip_bit_offset 	1162	/* 0x0000048a */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1901__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1901__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1901__D 	40
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1901__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1901__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1901__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1901__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1901__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1901__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1901__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1901__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1901__vmem_buffer_num_bytes 	15360
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1901__dram_size_num_bytes 	36480
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1901__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1901__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1901__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1901__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1901__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1901__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1901__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1901__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1901__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1901__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_19 HMB_input __pvcn_2011_ */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2011__cnngen_demangled_name 	"__pvcn_2011_"
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2011__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2011__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2011__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2011__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2011__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2011__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2011__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2011__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2011__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2011__byte_offset 	160	/* 0x000000a0 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2011__vbase_byte_offset 	164	/* 0x000000a4 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2011__dbase_byte_offset 	188	/* 0x000000bc */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2011__dpitchm1_byte_offset 	192	/* 0x000000c0 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2011__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2011__vwidth_minus_one_byte_offset 	176	/* 0x000000b0 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2011__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2011__vheight_minus_one_byte_offset 	178	/* 0x000000b2 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2011__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2011__drotate_bit_offset 	1478	/* 0x000005c6 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2011__hflip_bit_offset 	1479	/* 0x000005c7 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2011__vflip_bit_offset 	1480	/* 0x000005c8 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2011__dflip_bit_offset 	1481	/* 0x000005c9 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2011__pflip_bit_offset 	1482	/* 0x000005ca */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2011__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2011__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2011__D 	40
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2011__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2011__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2011__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2011__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2011__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2011__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2011__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2011__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2011__vmem_buffer_num_bytes 	15360
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2011__dram_size_num_bytes 	36480
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2011__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2011__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2011__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2011__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2011__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2011__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2011__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2011__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2011__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2011__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_19 HMB_input __pvcn_1989_ */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1989__cnngen_demangled_name 	"__pvcn_1989_"
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1989__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1989__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1989__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1989__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1989__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1989__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1989__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1989__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1989__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1989__byte_offset 	200	/* 0x000000c8 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1989__vbase_byte_offset 	204	/* 0x000000cc */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1989__dbase_byte_offset 	228	/* 0x000000e4 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1989__dpitchm1_byte_offset 	232	/* 0x000000e8 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1989__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1989__vwidth_minus_one_byte_offset 	216	/* 0x000000d8 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1989__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1989__vheight_minus_one_byte_offset 	218	/* 0x000000da */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1989__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1989__drotate_bit_offset 	1798	/* 0x00000706 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1989__hflip_bit_offset 	1799	/* 0x00000707 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1989__vflip_bit_offset 	1800	/* 0x00000708 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1989__dflip_bit_offset 	1801	/* 0x00000709 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1989__pflip_bit_offset 	1802	/* 0x0000070a */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1989__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1989__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1989__D 	36
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1989__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1989__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1989__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1989__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1989__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1989__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1989__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1989__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1989__vmem_buffer_num_bytes 	13824
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1989__dram_size_num_bytes 	32832
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1989__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1989__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1989__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1989__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1989__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1989__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1989__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1989__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1989__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1989__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_19 HMB_input __pvcn_1978_ */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1978__cnngen_demangled_name 	"__pvcn_1978_"
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1978__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1978__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1978__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1978__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1978__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1978__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1978__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1978__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1978__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1978__byte_offset 	240	/* 0x000000f0 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1978__vbase_byte_offset 	244	/* 0x000000f4 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1978__dbase_byte_offset 	268	/* 0x0000010c */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1978__dpitchm1_byte_offset 	272	/* 0x00000110 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1978__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1978__vwidth_minus_one_byte_offset 	256	/* 0x00000100 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1978__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1978__vheight_minus_one_byte_offset 	258	/* 0x00000102 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1978__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1978__drotate_bit_offset 	2118	/* 0x00000846 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1978__hflip_bit_offset 	2119	/* 0x00000847 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1978__vflip_bit_offset 	2120	/* 0x00000848 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1978__dflip_bit_offset 	2121	/* 0x00000849 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1978__pflip_bit_offset 	2122	/* 0x0000084a */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1978__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1978__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1978__D 	37
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1978__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1978__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1978__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1978__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1978__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1978__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1978__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1978__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1978__vmem_buffer_num_bytes 	14208
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1978__dram_size_num_bytes 	33760
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1978__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1978__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1978__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1978__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1978__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1978__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1978__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1978__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1978__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1978__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_19 HMB_input __pvcn_1967_ */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1967__cnngen_demangled_name 	"__pvcn_1967_"
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1967__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1967__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1967__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1967__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1967__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1967__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1967__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1967__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1967__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1967__byte_offset 	280	/* 0x00000118 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1967__vbase_byte_offset 	284	/* 0x0000011c */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1967__dbase_byte_offset 	308	/* 0x00000134 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1967__dpitchm1_byte_offset 	312	/* 0x00000138 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1967__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1967__vwidth_minus_one_byte_offset 	296	/* 0x00000128 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1967__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1967__vheight_minus_one_byte_offset 	298	/* 0x0000012a */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1967__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1967__drotate_bit_offset 	2438	/* 0x00000986 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1967__hflip_bit_offset 	2439	/* 0x00000987 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1967__vflip_bit_offset 	2440	/* 0x00000988 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1967__dflip_bit_offset 	2441	/* 0x00000989 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1967__pflip_bit_offset 	2442	/* 0x0000098a */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1967__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1967__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1967__D 	41
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1967__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1967__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1967__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1967__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1967__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1967__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1967__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1967__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1967__vmem_buffer_num_bytes 	7872
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1967__dram_size_num_bytes 	37408
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1967__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1967__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1967__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1967__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1967__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1967__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1967__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1967__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1967__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1967__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_19 HMB_input __pvcn_1945_ */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1945__cnngen_demangled_name 	"__pvcn_1945_"
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1945__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1945__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1945__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1945__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1945__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1945__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1945__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1945__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1945__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1945__byte_offset 	320	/* 0x00000140 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1945__vbase_byte_offset 	324	/* 0x00000144 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1945__dbase_byte_offset 	348	/* 0x0000015c */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1945__dpitchm1_byte_offset 	352	/* 0x00000160 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1945__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1945__vwidth_minus_one_byte_offset 	336	/* 0x00000150 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1945__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1945__vheight_minus_one_byte_offset 	338	/* 0x00000152 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1945__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1945__drotate_bit_offset 	2758	/* 0x00000ac6 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1945__hflip_bit_offset 	2759	/* 0x00000ac7 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1945__vflip_bit_offset 	2760	/* 0x00000ac8 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1945__dflip_bit_offset 	2761	/* 0x00000ac9 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1945__pflip_bit_offset 	2762	/* 0x00000aca */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1945__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1945__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1945__D 	40
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1945__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1945__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1945__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1945__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1945__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1945__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1945__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1945__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1945__vmem_buffer_num_bytes 	7680
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1945__dram_size_num_bytes 	36480
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1945__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1945__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1945__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1945__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1945__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1945__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1945__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1945__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1945__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1945__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_19 HMB_input __pvcn_1956_ */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1956__cnngen_demangled_name 	"__pvcn_1956_"
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1956__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1956__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1956__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1956__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1956__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1956__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1956__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1956__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1956__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1956__byte_offset 	360	/* 0x00000168 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1956__vbase_byte_offset 	364	/* 0x0000016c */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1956__dbase_byte_offset 	388	/* 0x00000184 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1956__dpitchm1_byte_offset 	392	/* 0x00000188 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1956__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1956__vwidth_minus_one_byte_offset 	376	/* 0x00000178 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1956__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1956__vheight_minus_one_byte_offset 	378	/* 0x0000017a */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1956__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1956__drotate_bit_offset 	3078	/* 0x00000c06 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1956__hflip_bit_offset 	3079	/* 0x00000c07 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1956__vflip_bit_offset 	3080	/* 0x00000c08 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1956__dflip_bit_offset 	3081	/* 0x00000c09 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1956__pflip_bit_offset 	3082	/* 0x00000c0a */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1956__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1956__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1956__D 	37
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1956__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1956__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1956__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1956__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1956__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1956__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1956__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1956__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1956__vmem_buffer_num_bytes 	7104
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1956__dram_size_num_bytes 	33760
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1956__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1956__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1956__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1956__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1956__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1956__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1956__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1956__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1956__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1956__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_19 HMB_input __pvcn_1923_ */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1923__cnngen_demangled_name 	"__pvcn_1923_"
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1923__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1923__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1923__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1923__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1923__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1923__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1923__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1923__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1923__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1923__byte_offset 	400	/* 0x00000190 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1923__vbase_byte_offset 	404	/* 0x00000194 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1923__dbase_byte_offset 	428	/* 0x000001ac */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1923__dpitchm1_byte_offset 	432	/* 0x000001b0 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1923__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1923__vwidth_minus_one_byte_offset 	416	/* 0x000001a0 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1923__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1923__vheight_minus_one_byte_offset 	418	/* 0x000001a2 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1923__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1923__drotate_bit_offset 	3398	/* 0x00000d46 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1923__hflip_bit_offset 	3399	/* 0x00000d47 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1923__vflip_bit_offset 	3400	/* 0x00000d48 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1923__dflip_bit_offset 	3401	/* 0x00000d49 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1923__pflip_bit_offset 	3402	/* 0x00000d4a */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1923__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1923__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1923__D 	37
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1923__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1923__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1923__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1923__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1923__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1923__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1923__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1923__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1923__vmem_buffer_num_bytes 	7104
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1923__dram_size_num_bytes 	33760
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1923__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1923__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1923__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1923__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1923__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1923__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1923__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1923__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1923__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1923__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_19 HMB_input __pvcn_1934_ */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1934__cnngen_demangled_name 	"__pvcn_1934_"
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1934__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1934__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1934__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1934__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1934__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1934__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1934__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1934__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1934__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1934__byte_offset 	440	/* 0x000001b8 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1934__vbase_byte_offset 	444	/* 0x000001bc */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1934__dbase_byte_offset 	468	/* 0x000001d4 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1934__dpitchm1_byte_offset 	472	/* 0x000001d8 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1934__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1934__vwidth_minus_one_byte_offset 	456	/* 0x000001c8 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1934__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1934__vheight_minus_one_byte_offset 	458	/* 0x000001ca */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1934__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1934__drotate_bit_offset 	3718	/* 0x00000e86 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1934__hflip_bit_offset 	3719	/* 0x00000e87 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1934__vflip_bit_offset 	3720	/* 0x00000e88 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1934__dflip_bit_offset 	3721	/* 0x00000e89 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1934__pflip_bit_offset 	3722	/* 0x00000e8a */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1934__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1934__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1934__D 	36
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1934__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1934__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1934__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1934__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1934__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1934__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1934__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1934__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1934__vmem_buffer_num_bytes 	6912
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1934__dram_size_num_bytes 	32832
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1934__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1934__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1934__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1934__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1934__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1934__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1934__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1934__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1934__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1934__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_19 HMB_input __pvcn_1879_ */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1879__cnngen_demangled_name 	"__pvcn_1879_"
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1879__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1879__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1879__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1879__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1879__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1879__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1879__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1879__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1879__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1879__byte_offset 	480	/* 0x000001e0 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1879__vbase_byte_offset 	484	/* 0x000001e4 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1879__dbase_byte_offset 	508	/* 0x000001fc */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1879__dpitchm1_byte_offset 	512	/* 0x00000200 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1879__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1879__vwidth_minus_one_byte_offset 	496	/* 0x000001f0 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1879__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1879__vheight_minus_one_byte_offset 	498	/* 0x000001f2 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1879__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1879__drotate_bit_offset 	4038	/* 0x00000fc6 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1879__hflip_bit_offset 	4039	/* 0x00000fc7 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1879__vflip_bit_offset 	4040	/* 0x00000fc8 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1879__dflip_bit_offset 	4041	/* 0x00000fc9 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1879__pflip_bit_offset 	4042	/* 0x00000fca */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1879__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1879__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1879__D 	37
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1879__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1879__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1879__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1879__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1879__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1879__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1879__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1879__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1879__vmem_buffer_num_bytes 	7104
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1879__dram_size_num_bytes 	33760
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1879__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1879__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1879__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1879__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1879__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1879__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1879__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1879__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1879__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1879__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_19 HMB_input __pvcn_1890_ */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1890__cnngen_demangled_name 	"__pvcn_1890_"
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1890__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1890__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1890__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1890__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1890__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1890__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1890__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1890__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1890__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1890__byte_offset 	516	/* 0x00000204 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1890__vbase_byte_offset 	520	/* 0x00000208 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1890__dbase_byte_offset 	544	/* 0x00000220 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1890__dpitchm1_byte_offset 	548	/* 0x00000224 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1890__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1890__vwidth_minus_one_byte_offset 	532	/* 0x00000214 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1890__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1890__vheight_minus_one_byte_offset 	534	/* 0x00000216 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1890__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1890__drotate_bit_offset 	4326	/* 0x000010e6 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1890__hflip_bit_offset 	4327	/* 0x000010e7 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1890__vflip_bit_offset 	4328	/* 0x000010e8 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1890__dflip_bit_offset 	4329	/* 0x000010e9 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1890__pflip_bit_offset 	4330	/* 0x000010ea */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1890__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1890__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1890__D 	37
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1890__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1890__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1890__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1890__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1890__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1890__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1890__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1890__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1890__vmem_buffer_num_bytes 	7104
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1890__dram_size_num_bytes 	33760
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1890__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1890__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1890__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1890__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1890__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1890__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1890__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1890__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1890__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_1890__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_19 SMB_input __pvcn_2059_ */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2059__cnngen_demangled_name 	"__pvcn_2059_"
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2059__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2059__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2059__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2059__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2059__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2059__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2059__byte_offset 	564	/* 0x00000234 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2059__vbase_byte_offset 	568	/* 0x00000238 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2059__dbase_byte_offset 	592	/* 0x00000250 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2059__dpitchm1_byte_offset 	596	/* 0x00000254 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2059__dpitchm1_bsize 	32	/* 0x00000020 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2059__W 	248996
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2059__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2059__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2059__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2059__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2059__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2059__dpitch_num_bytes 	249024
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2059__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2059__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2059__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2059__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2059__vmem_buffer_num_bytes 	248996
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2059__dram_size_num_bytes 	249024

/* mnet_ssd_adas_flex_pic_prim_split_19 SMB_input __pvcn_2060_ */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2060__cnngen_demangled_name 	"__pvcn_2060_"
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2060__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2060__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2060__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2060__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2060__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2060__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2060__byte_offset 	604	/* 0x0000025c */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2060__vbase_byte_offset 	608	/* 0x00000260 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2060__dbase_byte_offset 	632	/* 0x00000278 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2060__dpitchm1_byte_offset 	636	/* 0x0000027c */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2060__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2060__W 	2856
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2060__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2060__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2060__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2060__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2060__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2060__dpitch_num_bytes 	2880
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2060__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2060__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2060__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2060__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2060__vmem_buffer_num_bytes 	2856
#define mnet_ssd_adas_flex_pic_prim_split_19___pvcn_2060__dram_size_num_bytes 	2880

/* mnet_ssd_adas_flex_pic_prim_split_19 VCB conv5_5__dw_____bn_conv5_5__dw__scale_mul___muli___71_____bn_conv5_5__dw__scale_mul_____scale___muli___355 */
#define mnet_ssd_adas_flex_pic_prim_split_19_conv5_5__dw_____bn_conv5_5__dw__scale_mul___muli___71_____bn_conv5_5__dw__scale_mul_____scale___muli___355_cnngen_demangled_name 	"conv5_5__dw_____bn_conv5_5__dw__scale_mul___muli___71_____bn_conv5_5__dw__scale_mul_____scale___muli___355"
#define mnet_ssd_adas_flex_pic_prim_split_19_conv5_5__dw_____bn_conv5_5__dw__scale_mul___muli___71_____bn_conv5_5__dw__scale_mul_____scale___muli___355_is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_19_conv5_5__dw_____bn_conv5_5__dw__scale_mul___muli___71_____bn_conv5_5__dw__scale_mul_____scale___muli___355_is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_19_conv5_5__dw_____bn_conv5_5__dw__scale_mul___muli___71_____bn_conv5_5__dw__scale_mul_____scale___muli___355_is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_19_conv5_5__dw_____bn_conv5_5__dw__scale_mul___muli___71_____bn_conv5_5__dw__scale_mul_____scale___muli___355_byte_offset 	1120	/* 0x00000460 */
#define mnet_ssd_adas_flex_pic_prim_split_19_conv5_5__dw_____bn_conv5_5__dw__scale_mul___muli___71_____bn_conv5_5__dw__scale_mul_____scale___muli___355_vbase_byte_offset 	1128	/* 0x00000468 */
#define mnet_ssd_adas_flex_pic_prim_split_19_conv5_5__dw_____bn_conv5_5__dw__scale_mul___muli___71_____bn_conv5_5__dw__scale_mul_____scale___muli___355_vmem_buffer_num_bytes 	61440

/* mnet_ssd_adas_flex_pic_prim_split_19 HMB_output conv5_5__sep_____bn_conv5_5__sep__scale_mul___muli___74_____bn_conv5_5__sep__scale_mul_____scale___relu5_5__sep */
#define mnet_ssd_adas_flex_pic_prim_split_19_conv5_5__sep_____bn_conv5_5__sep__scale_mul___muli___74_____bn_conv5_5__sep__scale_mul_____scale___relu5_5__sep_cnngen_demangled_name 	"conv5_5__sep_____bn_conv5_5__sep__scale_mul___muli___74_____bn_conv5_5__sep__scale_mul_____scale___relu5_5__sep"
#define mnet_ssd_adas_flex_pic_prim_split_19_conv5_5__sep_____bn_conv5_5__sep__scale_mul___muli___74_____bn_conv5_5__sep__scale_mul_____scale___relu5_5__sep_is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_19_conv5_5__sep_____bn_conv5_5__sep__scale_mul___muli___74_____bn_conv5_5__sep__scale_mul_____scale___relu5_5__sep_is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_19_conv5_5__sep_____bn_conv5_5__sep__scale_mul___muli___74_____bn_conv5_5__sep__scale_mul_____scale___relu5_5__sep_is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_19_conv5_5__sep_____bn_conv5_5__sep__scale_mul___muli___74_____bn_conv5_5__sep__scale_mul_____scale___relu5_5__sep_is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_19_conv5_5__sep_____bn_conv5_5__sep__scale_mul___muli___74_____bn_conv5_5__sep__scale_mul_____scale___relu5_5__sep_byte_offset 	1168	/* 0x00000490 */
#define mnet_ssd_adas_flex_pic_prim_split_19_conv5_5__sep_____bn_conv5_5__sep__scale_mul___muli___74_____bn_conv5_5__sep__scale_mul_____scale___relu5_5__sep_vbase_byte_offset 	1172	/* 0x00000494 */
#define mnet_ssd_adas_flex_pic_prim_split_19_conv5_5__sep_____bn_conv5_5__sep__scale_mul___muli___74_____bn_conv5_5__sep__scale_mul_____scale___relu5_5__sep_dbase_byte_offset 	1196	/* 0x000004ac */
#define mnet_ssd_adas_flex_pic_prim_split_19_conv5_5__sep_____bn_conv5_5__sep__scale_mul___muli___74_____bn_conv5_5__sep__scale_mul_____scale___relu5_5__sep_dpitchm1_byte_offset 	1200	/* 0x000004b0 */
#define mnet_ssd_adas_flex_pic_prim_split_19_conv5_5__sep_____bn_conv5_5__sep__scale_mul___muli___74_____bn_conv5_5__sep__scale_mul_____scale___relu5_5__sep_dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_19_conv5_5__sep_____bn_conv5_5__sep__scale_mul___muli___74_____bn_conv5_5__sep__scale_mul_____scale___relu5_5__sep_vwidth_minus_one_byte_offset 	1184	/* 0x000004a0 */
#define mnet_ssd_adas_flex_pic_prim_split_19_conv5_5__sep_____bn_conv5_5__sep__scale_mul___muli___74_____bn_conv5_5__sep__scale_mul_____scale___relu5_5__sep_vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_19_conv5_5__sep_____bn_conv5_5__sep__scale_mul___muli___74_____bn_conv5_5__sep__scale_mul_____scale___relu5_5__sep_vheight_minus_one_byte_offset 	1186	/* 0x000004a2 */
#define mnet_ssd_adas_flex_pic_prim_split_19_conv5_5__sep_____bn_conv5_5__sep__scale_mul___muli___74_____bn_conv5_5__sep__scale_mul_____scale___relu5_5__sep_vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_19_conv5_5__sep_____bn_conv5_5__sep__scale_mul___muli___74_____bn_conv5_5__sep__scale_mul_____scale___relu5_5__sep_drotate_bit_offset 	9542	/* 0x00002546 */
#define mnet_ssd_adas_flex_pic_prim_split_19_conv5_5__sep_____bn_conv5_5__sep__scale_mul___muli___74_____bn_conv5_5__sep__scale_mul_____scale___relu5_5__sep_hflip_bit_offset 	9543	/* 0x00002547 */
#define mnet_ssd_adas_flex_pic_prim_split_19_conv5_5__sep_____bn_conv5_5__sep__scale_mul___muli___74_____bn_conv5_5__sep__scale_mul_____scale___relu5_5__sep_vflip_bit_offset 	9544	/* 0x00002548 */
#define mnet_ssd_adas_flex_pic_prim_split_19_conv5_5__sep_____bn_conv5_5__sep__scale_mul___muli___74_____bn_conv5_5__sep__scale_mul_____scale___relu5_5__sep_dflip_bit_offset 	9545	/* 0x00002549 */
#define mnet_ssd_adas_flex_pic_prim_split_19_conv5_5__sep_____bn_conv5_5__sep__scale_mul___muli___74_____bn_conv5_5__sep__scale_mul_____scale___relu5_5__sep_pflip_bit_offset 	9546	/* 0x0000254a */
#define mnet_ssd_adas_flex_pic_prim_split_19_conv5_5__sep_____bn_conv5_5__sep__scale_mul___muli___74_____bn_conv5_5__sep__scale_mul_____scale___relu5_5__sep_W 	19
#define mnet_ssd_adas_flex_pic_prim_split_19_conv5_5__sep_____bn_conv5_5__sep__scale_mul___muli___74_____bn_conv5_5__sep__scale_mul_____scale___relu5_5__sep_H 	19
#define mnet_ssd_adas_flex_pic_prim_split_19_conv5_5__sep_____bn_conv5_5__sep__scale_mul___muli___74_____bn_conv5_5__sep__scale_mul_____scale___relu5_5__sep_D 	512
#define mnet_ssd_adas_flex_pic_prim_split_19_conv5_5__sep_____bn_conv5_5__sep__scale_mul___muli___74_____bn_conv5_5__sep__scale_mul_____scale___relu5_5__sep_P 	1
#define mnet_ssd_adas_flex_pic_prim_split_19_conv5_5__sep_____bn_conv5_5__sep__scale_mul___muli___74_____bn_conv5_5__sep__scale_mul_____scale___relu5_5__sep_data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_19_conv5_5__sep_____bn_conv5_5__sep__scale_mul___muli___74_____bn_conv5_5__sep__scale_mul_____scale___relu5_5__sep_denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_19_conv5_5__sep_____bn_conv5_5__sep__scale_mul___muli___74_____bn_conv5_5__sep__scale_mul_____scale___relu5_5__sep_dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_19_conv5_5__sep_____bn_conv5_5__sep__scale_mul___muli___74_____bn_conv5_5__sep__scale_mul_____scale___relu5_5__sep_dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_19_conv5_5__sep_____bn_conv5_5__sep__scale_mul___muli___74_____bn_conv5_5__sep__scale_mul_____scale___relu5_5__sep_vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_19_conv5_5__sep_____bn_conv5_5__sep__scale_mul___muli___74_____bn_conv5_5__sep__scale_mul_____scale___relu5_5__sep_tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_19_conv5_5__sep_____bn_conv5_5__sep__scale_mul___muli___74_____bn_conv5_5__sep__scale_mul_____scale___relu5_5__sep_tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_19_conv5_5__sep_____bn_conv5_5__sep__scale_mul___muli___74_____bn_conv5_5__sep__scale_mul_____scale___relu5_5__sep_vmem_buffer_num_bytes 	49152
#define mnet_ssd_adas_flex_pic_prim_split_19_conv5_5__sep_____bn_conv5_5__sep__scale_mul___muli___74_____bn_conv5_5__sep__scale_mul_____scale___relu5_5__sep_dram_size_num_bytes 	233472
#define mnet_ssd_adas_flex_pic_prim_split_19_conv5_5__sep_____bn_conv5_5__sep__scale_mul___muli___74_____bn_conv5_5__sep__scale_mul_____scale___relu5_5__sep_sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_19_conv5_5__sep_____bn_conv5_5__sep__scale_mul___muli___74_____bn_conv5_5__sep__scale_mul_____scale___relu5_5__sep_datasize 	0
#define mnet_ssd_adas_flex_pic_prim_split_19_conv5_5__sep_____bn_conv5_5__sep__scale_mul___muli___74_____bn_conv5_5__sep__scale_mul_____scale___relu5_5__sep_expoffset 	6
#define mnet_ssd_adas_flex_pic_prim_split_19_conv5_5__sep_____bn_conv5_5__sep__scale_mul___muli___74_____bn_conv5_5__sep__scale_mul_____scale___relu5_5__sep_expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_19_conv5_5__sep_____bn_conv5_5__sep__scale_mul___muli___74_____bn_conv5_5__sep__scale_mul_____scale___relu5_5__sep_drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_19_conv5_5__sep_____bn_conv5_5__sep__scale_mul___muli___74_____bn_conv5_5__sep__scale_mul_____scale___relu5_5__sep_hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_19_conv5_5__sep_____bn_conv5_5__sep__scale_mul___muli___74_____bn_conv5_5__sep__scale_mul_____scale___relu5_5__sep_vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_19_conv5_5__sep_____bn_conv5_5__sep__scale_mul___muli___74_____bn_conv5_5__sep__scale_mul_____scale___relu5_5__sep_dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_19_conv5_5__sep_____bn_conv5_5__sep__scale_mul___muli___74_____bn_conv5_5__sep__scale_mul_____scale___relu5_5__sep_pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_19_conv5_5__sep_____bn_conv5_5__sep__scale_mul___muli___74_____bn_conv5_5__sep__scale_mul_____scale___relu5_5__sep_trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_19 SMB_output __vas_created_terminate__v0 */
#define mnet_ssd_adas_flex_pic_prim_split_19___vas_created_terminate__v0_cnngen_demangled_name 	"__vas_created_terminate__v0"
#define mnet_ssd_adas_flex_pic_prim_split_19___vas_created_terminate__v0_is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___vas_created_terminate__v0_is_bitvector 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___vas_created_terminate__v0_is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___vas_created_terminate__v0_is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___vas_created_terminate__v0_byte_offset 	1208	/* 0x000004b8 */
#define mnet_ssd_adas_flex_pic_prim_split_19___vas_created_terminate__v0_vbase_byte_offset 	1212	/* 0x000004bc */
#define mnet_ssd_adas_flex_pic_prim_split_19___vas_created_terminate__v0_W 	256
#define mnet_ssd_adas_flex_pic_prim_split_19___vas_created_terminate__v0_H 	3
#define mnet_ssd_adas_flex_pic_prim_split_19___vas_created_terminate__v0_D 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___vas_created_terminate__v0_P 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___vas_created_terminate__v0_data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___vas_created_terminate__v0_denable 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___vas_created_terminate__v0_dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_19___vas_created_terminate__v0_vp_interleave_mode 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___vas_created_terminate__v0_tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___vas_created_terminate__v0_tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_19___vas_created_terminate__v0_vmem_buffer_num_bytes 	96


#endif /* mnet_ssd_adas_flex_pic_prim_split_19_H */
