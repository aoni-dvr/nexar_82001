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
#ifndef mnet_ssd_adas_flex_pic_prim_split_13_H
#define mnet_ssd_adas_flex_pic_prim_split_13_H

#define vas_gen_header_version                  	7

/* dag name */
#define mnet_ssd_adas_flex_pic_prim_split_13_DAG_NAME 	"mnet_ssd_adas_flex_pic_prim_split_13"

/* VDG file info */
#define mnet_ssd_adas_flex_pic_prim_split_13_vdg_name 	"mnet_ssd_adas_flex_pic_split_13.vdg"

/* VDG DAG memory info */
#define mnet_ssd_adas_flex_pic_prim_split_13_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_13_byte_size 	1284	/* 0x00000504 */
#define mnet_ssd_adas_flex_pic_prim_split_13_preferred_dram_xfer_size 	1

/* mnet_ssd_adas_flex_pic_prim_split_13 VMEM info */
#define mnet_ssd_adas_flex_pic_prim_split_13_use_ping_pong_vmem 	0
#define mnet_ssd_adas_flex_pic_prim_split_13_VMEM_start 	4096	/* 0x00001000 */
#define mnet_ssd_adas_flex_pic_prim_split_13_VMEM_end 	513036	/* 0x0007d40c */
#define mnet_ssd_adas_flex_pic_prim_split_13_image_start 	262432	/* 0x00040120 */
#define mnet_ssd_adas_flex_pic_prim_split_13_image_size 	251888	/* 0x0003d7f0 */
#define mnet_ssd_adas_flex_pic_prim_split_13_dagbin_start 	513036	/* 0x0007d40c */

/* mnet_ssd_adas_flex_pic_prim_split_13 DAG info */
#define mnet_ssd_adas_flex_pic_prim_split_13_estimated_cycles 	132357
#define mnet_ssd_adas_flex_pic_prim_split_13_ddi_byte_size 	0	/* 0x00000000 */

/*******************************************************************************
 * DAG byte offset info for HMB/SMB/VCB and their fields
 * in DAG 'mnet_ssd_adas_flex_pic_prim_split_13' in source file 'pre_split13_mnet_ssd_adas_flex_pic_split_0.vas'
 ******************************************************************************/
/* mnet_ssd_adas_flex_pic_prim_split_13 HMB_input __pvcn_1403_ */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1403__cnngen_demangled_name 	"__pvcn_1403_"
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1403__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1403__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1403__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1403__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1403__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1403__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1403__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1403__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1403__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1403__byte_offset 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1403__vbase_byte_offset 	4	/* 0x00000004 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1403__dbase_byte_offset 	28	/* 0x0000001c */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1403__dpitchm1_byte_offset 	32	/* 0x00000020 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1403__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1403__vwidth_minus_one_byte_offset 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1403__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1403__vheight_minus_one_byte_offset 	18	/* 0x00000012 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1403__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1403__drotate_bit_offset 	198	/* 0x000000c6 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1403__hflip_bit_offset 	199	/* 0x000000c7 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1403__vflip_bit_offset 	200	/* 0x000000c8 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1403__dflip_bit_offset 	201	/* 0x000000c9 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1403__pflip_bit_offset 	202	/* 0x000000ca */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1403__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1403__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1403__D 	37
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1403__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1403__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1403__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1403__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1403__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1403__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1403__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1403__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1403__vmem_buffer_num_bytes 	14208
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1403__dram_size_num_bytes 	33760
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1403__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1403__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1403__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1403__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1403__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1403__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1403__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1403__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1403__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1403__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_13 HMB_input __pvcn_1348_ */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1348__cnngen_demangled_name 	"__pvcn_1348_"
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1348__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1348__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1348__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1348__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1348__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1348__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1348__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1348__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1348__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1348__byte_offset 	40	/* 0x00000028 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1348__vbase_byte_offset 	44	/* 0x0000002c */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1348__dbase_byte_offset 	68	/* 0x00000044 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1348__dpitchm1_byte_offset 	72	/* 0x00000048 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1348__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1348__vwidth_minus_one_byte_offset 	56	/* 0x00000038 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1348__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1348__vheight_minus_one_byte_offset 	58	/* 0x0000003a */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1348__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1348__drotate_bit_offset 	518	/* 0x00000206 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1348__hflip_bit_offset 	519	/* 0x00000207 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1348__vflip_bit_offset 	520	/* 0x00000208 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1348__dflip_bit_offset 	521	/* 0x00000209 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1348__pflip_bit_offset 	522	/* 0x0000020a */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1348__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1348__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1348__D 	37
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1348__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1348__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1348__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1348__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1348__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1348__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1348__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1348__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1348__vmem_buffer_num_bytes 	14208
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1348__dram_size_num_bytes 	33760
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1348__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1348__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1348__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1348__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1348__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1348__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1348__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1348__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1348__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1348__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_13 HMB_input __pvcn_1425_ */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1425__cnngen_demangled_name 	"__pvcn_1425_"
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1425__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1425__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1425__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1425__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1425__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1425__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1425__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1425__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1425__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1425__byte_offset 	80	/* 0x00000050 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1425__vbase_byte_offset 	84	/* 0x00000054 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1425__dbase_byte_offset 	108	/* 0x0000006c */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1425__dpitchm1_byte_offset 	112	/* 0x00000070 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1425__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1425__vwidth_minus_one_byte_offset 	96	/* 0x00000060 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1425__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1425__vheight_minus_one_byte_offset 	98	/* 0x00000062 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1425__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1425__drotate_bit_offset 	838	/* 0x00000346 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1425__hflip_bit_offset 	839	/* 0x00000347 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1425__vflip_bit_offset 	840	/* 0x00000348 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1425__dflip_bit_offset 	841	/* 0x00000349 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1425__pflip_bit_offset 	842	/* 0x0000034a */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1425__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1425__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1425__D 	39
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1425__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1425__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1425__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1425__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1425__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1425__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1425__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1425__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1425__vmem_buffer_num_bytes 	14976
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1425__dram_size_num_bytes 	35584
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1425__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1425__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1425__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1425__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1425__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1425__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1425__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1425__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1425__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1425__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_13 HMB_input __pvcn_1392_ */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1392__cnngen_demangled_name 	"__pvcn_1392_"
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1392__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1392__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1392__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1392__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1392__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1392__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1392__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1392__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1392__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1392__byte_offset 	120	/* 0x00000078 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1392__vbase_byte_offset 	124	/* 0x0000007c */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1392__dbase_byte_offset 	148	/* 0x00000094 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1392__dpitchm1_byte_offset 	152	/* 0x00000098 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1392__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1392__vwidth_minus_one_byte_offset 	136	/* 0x00000088 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1392__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1392__vheight_minus_one_byte_offset 	138	/* 0x0000008a */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1392__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1392__drotate_bit_offset 	1158	/* 0x00000486 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1392__hflip_bit_offset 	1159	/* 0x00000487 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1392__vflip_bit_offset 	1160	/* 0x00000488 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1392__dflip_bit_offset 	1161	/* 0x00000489 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1392__pflip_bit_offset 	1162	/* 0x0000048a */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1392__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1392__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1392__D 	40
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1392__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1392__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1392__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1392__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1392__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1392__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1392__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1392__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1392__vmem_buffer_num_bytes 	15360
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1392__dram_size_num_bytes 	36480
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1392__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1392__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1392__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1392__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1392__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1392__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1392__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1392__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1392__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1392__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_13 HMB_input __pvcn_1436_ */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1436__cnngen_demangled_name 	"__pvcn_1436_"
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1436__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1436__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1436__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1436__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1436__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1436__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1436__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1436__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1436__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1436__byte_offset 	160	/* 0x000000a0 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1436__vbase_byte_offset 	164	/* 0x000000a4 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1436__dbase_byte_offset 	188	/* 0x000000bc */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1436__dpitchm1_byte_offset 	192	/* 0x000000c0 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1436__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1436__vwidth_minus_one_byte_offset 	176	/* 0x000000b0 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1436__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1436__vheight_minus_one_byte_offset 	178	/* 0x000000b2 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1436__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1436__drotate_bit_offset 	1478	/* 0x000005c6 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1436__hflip_bit_offset 	1479	/* 0x000005c7 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1436__vflip_bit_offset 	1480	/* 0x000005c8 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1436__dflip_bit_offset 	1481	/* 0x000005c9 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1436__pflip_bit_offset 	1482	/* 0x000005ca */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1436__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1436__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1436__D 	14
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1436__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1436__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1436__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1436__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1436__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1436__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1436__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1436__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1436__vmem_buffer_num_bytes 	8064
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1436__dram_size_num_bytes 	12768
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1436__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1436__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1436__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1436__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1436__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1436__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1436__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1436__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1436__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1436__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_13 HMB_input __pvcn_1414_ */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1414__cnngen_demangled_name 	"__pvcn_1414_"
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1414__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1414__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1414__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1414__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1414__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1414__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1414__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1414__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1414__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1414__byte_offset 	200	/* 0x000000c8 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1414__vbase_byte_offset 	204	/* 0x000000cc */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1414__dbase_byte_offset 	228	/* 0x000000e4 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1414__dpitchm1_byte_offset 	232	/* 0x000000e8 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1414__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1414__vwidth_minus_one_byte_offset 	216	/* 0x000000d8 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1414__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1414__vheight_minus_one_byte_offset 	218	/* 0x000000da */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1414__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1414__drotate_bit_offset 	1798	/* 0x00000706 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1414__hflip_bit_offset 	1799	/* 0x00000707 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1414__vflip_bit_offset 	1800	/* 0x00000708 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1414__dflip_bit_offset 	1801	/* 0x00000709 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1414__pflip_bit_offset 	1802	/* 0x0000070a */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1414__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1414__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1414__D 	38
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1414__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1414__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1414__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1414__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1414__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1414__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1414__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1414__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1414__vmem_buffer_num_bytes 	14592
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1414__dram_size_num_bytes 	34656
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1414__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1414__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1414__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1414__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1414__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1414__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1414__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1414__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1414__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1414__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_13 HMB_input __pvcn_1293_ */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1293__cnngen_demangled_name 	"__pvcn_1293_"
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1293__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1293__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1293__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1293__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1293__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1293__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1293__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1293__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1293__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1293__byte_offset 	240	/* 0x000000f0 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1293__vbase_byte_offset 	244	/* 0x000000f4 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1293__dbase_byte_offset 	268	/* 0x0000010c */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1293__dpitchm1_byte_offset 	272	/* 0x00000110 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1293__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1293__vwidth_minus_one_byte_offset 	256	/* 0x00000100 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1293__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1293__vheight_minus_one_byte_offset 	258	/* 0x00000102 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1293__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1293__drotate_bit_offset 	2118	/* 0x00000846 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1293__hflip_bit_offset 	2119	/* 0x00000847 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1293__vflip_bit_offset 	2120	/* 0x00000848 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1293__dflip_bit_offset 	2121	/* 0x00000849 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1293__pflip_bit_offset 	2122	/* 0x0000084a */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1293__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1293__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1293__D 	38
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1293__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1293__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1293__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1293__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1293__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1293__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1293__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1293__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1293__vmem_buffer_num_bytes 	14592
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1293__dram_size_num_bytes 	34656
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1293__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1293__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1293__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1293__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1293__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1293__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1293__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1293__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1293__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1293__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_13 HMB_input __pvcn_1381_ */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1381__cnngen_demangled_name 	"__pvcn_1381_"
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1381__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1381__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1381__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1381__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1381__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1381__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1381__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1381__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1381__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1381__byte_offset 	276	/* 0x00000114 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1381__vbase_byte_offset 	280	/* 0x00000118 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1381__dbase_byte_offset 	304	/* 0x00000130 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1381__dpitchm1_byte_offset 	308	/* 0x00000134 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1381__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1381__vwidth_minus_one_byte_offset 	292	/* 0x00000124 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1381__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1381__vheight_minus_one_byte_offset 	294	/* 0x00000126 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1381__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1381__drotate_bit_offset 	2406	/* 0x00000966 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1381__hflip_bit_offset 	2407	/* 0x00000967 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1381__vflip_bit_offset 	2408	/* 0x00000968 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1381__dflip_bit_offset 	2409	/* 0x00000969 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1381__pflip_bit_offset 	2410	/* 0x0000096a */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1381__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1381__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1381__D 	38
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1381__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1381__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1381__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1381__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1381__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1381__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1381__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1381__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1381__vmem_buffer_num_bytes 	7296
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1381__dram_size_num_bytes 	34656
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1381__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1381__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1381__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1381__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1381__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1381__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1381__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1381__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1381__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1381__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_13 HMB_input __pvcn_1359_ */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1359__cnngen_demangled_name 	"__pvcn_1359_"
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1359__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1359__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1359__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1359__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1359__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1359__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1359__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1359__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1359__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1359__byte_offset 	316	/* 0x0000013c */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1359__vbase_byte_offset 	320	/* 0x00000140 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1359__dbase_byte_offset 	344	/* 0x00000158 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1359__dpitchm1_byte_offset 	348	/* 0x0000015c */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1359__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1359__vwidth_minus_one_byte_offset 	332	/* 0x0000014c */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1359__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1359__vheight_minus_one_byte_offset 	334	/* 0x0000014e */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1359__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1359__drotate_bit_offset 	2726	/* 0x00000aa6 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1359__hflip_bit_offset 	2727	/* 0x00000aa7 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1359__vflip_bit_offset 	2728	/* 0x00000aa8 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1359__dflip_bit_offset 	2729	/* 0x00000aa9 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1359__pflip_bit_offset 	2730	/* 0x00000aaa */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1359__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1359__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1359__D 	37
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1359__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1359__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1359__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1359__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1359__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1359__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1359__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1359__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1359__vmem_buffer_num_bytes 	7104
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1359__dram_size_num_bytes 	33760
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1359__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1359__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1359__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1359__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1359__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1359__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1359__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1359__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1359__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1359__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_13 HMB_input __pvcn_1370_ */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1370__cnngen_demangled_name 	"__pvcn_1370_"
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1370__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1370__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1370__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1370__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1370__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1370__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1370__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1370__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1370__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1370__byte_offset 	356	/* 0x00000164 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1370__vbase_byte_offset 	360	/* 0x00000168 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1370__dbase_byte_offset 	384	/* 0x00000180 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1370__dpitchm1_byte_offset 	388	/* 0x00000184 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1370__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1370__vwidth_minus_one_byte_offset 	372	/* 0x00000174 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1370__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1370__vheight_minus_one_byte_offset 	374	/* 0x00000176 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1370__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1370__drotate_bit_offset 	3046	/* 0x00000be6 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1370__hflip_bit_offset 	3047	/* 0x00000be7 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1370__vflip_bit_offset 	3048	/* 0x00000be8 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1370__dflip_bit_offset 	3049	/* 0x00000be9 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1370__pflip_bit_offset 	3050	/* 0x00000bea */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1370__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1370__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1370__D 	43
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1370__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1370__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1370__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1370__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1370__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1370__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1370__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1370__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1370__vmem_buffer_num_bytes 	8256
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1370__dram_size_num_bytes 	39232
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1370__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1370__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1370__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1370__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1370__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1370__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1370__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1370__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1370__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1370__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_13 HMB_input __pvcn_1337_ */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1337__cnngen_demangled_name 	"__pvcn_1337_"
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1337__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1337__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1337__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1337__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1337__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1337__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1337__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1337__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1337__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1337__byte_offset 	396	/* 0x0000018c */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1337__vbase_byte_offset 	400	/* 0x00000190 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1337__dbase_byte_offset 	424	/* 0x000001a8 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1337__dpitchm1_byte_offset 	428	/* 0x000001ac */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1337__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1337__vwidth_minus_one_byte_offset 	412	/* 0x0000019c */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1337__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1337__vheight_minus_one_byte_offset 	414	/* 0x0000019e */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1337__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1337__drotate_bit_offset 	3366	/* 0x00000d26 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1337__hflip_bit_offset 	3367	/* 0x00000d27 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1337__vflip_bit_offset 	3368	/* 0x00000d28 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1337__dflip_bit_offset 	3369	/* 0x00000d29 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1337__pflip_bit_offset 	3370	/* 0x00000d2a */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1337__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1337__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1337__D 	38
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1337__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1337__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1337__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1337__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1337__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1337__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1337__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1337__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1337__vmem_buffer_num_bytes 	7296
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1337__dram_size_num_bytes 	34656
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1337__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1337__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1337__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1337__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1337__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1337__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1337__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1337__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1337__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1337__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_13 HMB_input __pvcn_1315_ */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1315__cnngen_demangled_name 	"__pvcn_1315_"
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1315__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1315__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1315__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1315__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1315__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1315__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1315__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1315__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1315__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1315__byte_offset 	436	/* 0x000001b4 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1315__vbase_byte_offset 	440	/* 0x000001b8 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1315__dbase_byte_offset 	464	/* 0x000001d0 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1315__dpitchm1_byte_offset 	468	/* 0x000001d4 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1315__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1315__vwidth_minus_one_byte_offset 	452	/* 0x000001c4 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1315__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1315__vheight_minus_one_byte_offset 	454	/* 0x000001c6 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1315__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1315__drotate_bit_offset 	3686	/* 0x00000e66 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1315__hflip_bit_offset 	3687	/* 0x00000e67 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1315__vflip_bit_offset 	3688	/* 0x00000e68 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1315__dflip_bit_offset 	3689	/* 0x00000e69 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1315__pflip_bit_offset 	3690	/* 0x00000e6a */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1315__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1315__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1315__D 	41
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1315__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1315__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1315__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1315__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1315__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1315__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1315__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1315__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1315__vmem_buffer_num_bytes 	7872
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1315__dram_size_num_bytes 	37408
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1315__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1315__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1315__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1315__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1315__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1315__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1315__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1315__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1315__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1315__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_13 HMB_input __pvcn_1326_ */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1326__cnngen_demangled_name 	"__pvcn_1326_"
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1326__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1326__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1326__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1326__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1326__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1326__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1326__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1326__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1326__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1326__byte_offset 	476	/* 0x000001dc */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1326__vbase_byte_offset 	480	/* 0x000001e0 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1326__dbase_byte_offset 	504	/* 0x000001f8 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1326__dpitchm1_byte_offset 	508	/* 0x000001fc */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1326__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1326__vwidth_minus_one_byte_offset 	492	/* 0x000001ec */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1326__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1326__vheight_minus_one_byte_offset 	494	/* 0x000001ee */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1326__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1326__drotate_bit_offset 	4006	/* 0x00000fa6 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1326__hflip_bit_offset 	4007	/* 0x00000fa7 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1326__vflip_bit_offset 	4008	/* 0x00000fa8 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1326__dflip_bit_offset 	4009	/* 0x00000fa9 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1326__pflip_bit_offset 	4010	/* 0x00000faa */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1326__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1326__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1326__D 	36
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1326__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1326__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1326__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1326__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1326__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1326__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1326__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1326__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1326__vmem_buffer_num_bytes 	6912
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1326__dram_size_num_bytes 	32832
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1326__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1326__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1326__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1326__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1326__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1326__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1326__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1326__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1326__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1326__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_13 HMB_input __pvcn_1304_ */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1304__cnngen_demangled_name 	"__pvcn_1304_"
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1304__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1304__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1304__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1304__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1304__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1304__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1304__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1304__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1304__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1304__byte_offset 	516	/* 0x00000204 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1304__vbase_byte_offset 	520	/* 0x00000208 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1304__dbase_byte_offset 	544	/* 0x00000220 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1304__dpitchm1_byte_offset 	548	/* 0x00000224 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1304__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1304__vwidth_minus_one_byte_offset 	532	/* 0x00000214 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1304__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1304__vheight_minus_one_byte_offset 	534	/* 0x00000216 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1304__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1304__drotate_bit_offset 	4326	/* 0x000010e6 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1304__hflip_bit_offset 	4327	/* 0x000010e7 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1304__vflip_bit_offset 	4328	/* 0x000010e8 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1304__dflip_bit_offset 	4329	/* 0x000010e9 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1304__pflip_bit_offset 	4330	/* 0x000010ea */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1304__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1304__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1304__D 	36
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1304__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1304__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1304__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1304__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1304__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1304__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1304__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1304__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1304__vmem_buffer_num_bytes 	6912
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1304__dram_size_num_bytes 	32832
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1304__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1304__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1304__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1304__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1304__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1304__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1304__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1304__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1304__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1304__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_13 SMB_input __pvcn_1473_ */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1473__cnngen_demangled_name 	"__pvcn_1473_"
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1473__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1473__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1473__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1473__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1473__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1473__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1473__byte_offset 	564	/* 0x00000234 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1473__vbase_byte_offset 	568	/* 0x00000238 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1473__dbase_byte_offset 	592	/* 0x00000250 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1473__dpitchm1_byte_offset 	596	/* 0x00000254 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1473__dpitchm1_bsize 	32	/* 0x00000020 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1473__W 	247744
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1473__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1473__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1473__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1473__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1473__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1473__dpitch_num_bytes 	247744
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1473__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1473__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1473__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1473__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1473__vmem_buffer_num_bytes 	247744
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1473__dram_size_num_bytes 	247744

/* mnet_ssd_adas_flex_pic_prim_split_13 SMB_input __pvcn_1474_ */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1474__cnngen_demangled_name 	"__pvcn_1474_"
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1474__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1474__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1474__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1474__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1474__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1474__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1474__byte_offset 	604	/* 0x0000025c */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1474__vbase_byte_offset 	608	/* 0x00000260 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1474__dbase_byte_offset 	632	/* 0x00000278 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1474__dpitchm1_byte_offset 	636	/* 0x0000027c */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1474__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1474__W 	2860
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1474__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1474__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1474__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1474__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1474__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1474__dpitch_num_bytes 	2880
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1474__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1474__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1474__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1474__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1474__vmem_buffer_num_bytes 	2860
#define mnet_ssd_adas_flex_pic_prim_split_13___pvcn_1474__dram_size_num_bytes 	2880

/* mnet_ssd_adas_flex_pic_prim_split_13 VCB conv5_2__dw_____bn_conv5_2__dw__scale_mul___muli___53_____bn_conv5_2__dw__scale_mul_____scale___muli___349 */
#define mnet_ssd_adas_flex_pic_prim_split_13_conv5_2__dw_____bn_conv5_2__dw__scale_mul___muli___53_____bn_conv5_2__dw__scale_mul_____scale___muli___349_cnngen_demangled_name 	"conv5_2__dw_____bn_conv5_2__dw__scale_mul___muli___53_____bn_conv5_2__dw__scale_mul_____scale___muli___349"
#define mnet_ssd_adas_flex_pic_prim_split_13_conv5_2__dw_____bn_conv5_2__dw__scale_mul___muli___53_____bn_conv5_2__dw__scale_mul_____scale___muli___349_is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_13_conv5_2__dw_____bn_conv5_2__dw__scale_mul___muli___53_____bn_conv5_2__dw__scale_mul_____scale___muli___349_is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_13_conv5_2__dw_____bn_conv5_2__dw__scale_mul___muli___53_____bn_conv5_2__dw__scale_mul_____scale___muli___349_is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_13_conv5_2__dw_____bn_conv5_2__dw__scale_mul___muli___53_____bn_conv5_2__dw__scale_mul_____scale___muli___349_byte_offset 	1120	/* 0x00000460 */
#define mnet_ssd_adas_flex_pic_prim_split_13_conv5_2__dw_____bn_conv5_2__dw__scale_mul___muli___53_____bn_conv5_2__dw__scale_mul_____scale___muli___349_vbase_byte_offset 	1128	/* 0x00000468 */
#define mnet_ssd_adas_flex_pic_prim_split_13_conv5_2__dw_____bn_conv5_2__dw__scale_mul___muli___53_____bn_conv5_2__dw__scale_mul_____scale___muli___349_vmem_buffer_num_bytes 	61440

/* mnet_ssd_adas_flex_pic_prim_split_13 HMB_output conv5_2__sep_____bn_conv5_2__sep__scale_mul___muli___56_____bn_conv5_2__sep__scale_mul_____scale___relu5_2__sep */
#define mnet_ssd_adas_flex_pic_prim_split_13_conv5_2__sep_____bn_conv5_2__sep__scale_mul___muli___56_____bn_conv5_2__sep__scale_mul_____scale___relu5_2__sep_cnngen_demangled_name 	"conv5_2__sep_____bn_conv5_2__sep__scale_mul___muli___56_____bn_conv5_2__sep__scale_mul_____scale___relu5_2__sep"
#define mnet_ssd_adas_flex_pic_prim_split_13_conv5_2__sep_____bn_conv5_2__sep__scale_mul___muli___56_____bn_conv5_2__sep__scale_mul_____scale___relu5_2__sep_is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_13_conv5_2__sep_____bn_conv5_2__sep__scale_mul___muli___56_____bn_conv5_2__sep__scale_mul_____scale___relu5_2__sep_is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_13_conv5_2__sep_____bn_conv5_2__sep__scale_mul___muli___56_____bn_conv5_2__sep__scale_mul_____scale___relu5_2__sep_is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_13_conv5_2__sep_____bn_conv5_2__sep__scale_mul___muli___56_____bn_conv5_2__sep__scale_mul_____scale___relu5_2__sep_is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_13_conv5_2__sep_____bn_conv5_2__sep__scale_mul___muli___56_____bn_conv5_2__sep__scale_mul_____scale___relu5_2__sep_byte_offset 	1168	/* 0x00000490 */
#define mnet_ssd_adas_flex_pic_prim_split_13_conv5_2__sep_____bn_conv5_2__sep__scale_mul___muli___56_____bn_conv5_2__sep__scale_mul_____scale___relu5_2__sep_vbase_byte_offset 	1172	/* 0x00000494 */
#define mnet_ssd_adas_flex_pic_prim_split_13_conv5_2__sep_____bn_conv5_2__sep__scale_mul___muli___56_____bn_conv5_2__sep__scale_mul_____scale___relu5_2__sep_dbase_byte_offset 	1196	/* 0x000004ac */
#define mnet_ssd_adas_flex_pic_prim_split_13_conv5_2__sep_____bn_conv5_2__sep__scale_mul___muli___56_____bn_conv5_2__sep__scale_mul_____scale___relu5_2__sep_dpitchm1_byte_offset 	1200	/* 0x000004b0 */
#define mnet_ssd_adas_flex_pic_prim_split_13_conv5_2__sep_____bn_conv5_2__sep__scale_mul___muli___56_____bn_conv5_2__sep__scale_mul_____scale___relu5_2__sep_dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_13_conv5_2__sep_____bn_conv5_2__sep__scale_mul___muli___56_____bn_conv5_2__sep__scale_mul_____scale___relu5_2__sep_vwidth_minus_one_byte_offset 	1184	/* 0x000004a0 */
#define mnet_ssd_adas_flex_pic_prim_split_13_conv5_2__sep_____bn_conv5_2__sep__scale_mul___muli___56_____bn_conv5_2__sep__scale_mul_____scale___relu5_2__sep_vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_13_conv5_2__sep_____bn_conv5_2__sep__scale_mul___muli___56_____bn_conv5_2__sep__scale_mul_____scale___relu5_2__sep_vheight_minus_one_byte_offset 	1186	/* 0x000004a2 */
#define mnet_ssd_adas_flex_pic_prim_split_13_conv5_2__sep_____bn_conv5_2__sep__scale_mul___muli___56_____bn_conv5_2__sep__scale_mul_____scale___relu5_2__sep_vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_13_conv5_2__sep_____bn_conv5_2__sep__scale_mul___muli___56_____bn_conv5_2__sep__scale_mul_____scale___relu5_2__sep_drotate_bit_offset 	9542	/* 0x00002546 */
#define mnet_ssd_adas_flex_pic_prim_split_13_conv5_2__sep_____bn_conv5_2__sep__scale_mul___muli___56_____bn_conv5_2__sep__scale_mul_____scale___relu5_2__sep_hflip_bit_offset 	9543	/* 0x00002547 */
#define mnet_ssd_adas_flex_pic_prim_split_13_conv5_2__sep_____bn_conv5_2__sep__scale_mul___muli___56_____bn_conv5_2__sep__scale_mul_____scale___relu5_2__sep_vflip_bit_offset 	9544	/* 0x00002548 */
#define mnet_ssd_adas_flex_pic_prim_split_13_conv5_2__sep_____bn_conv5_2__sep__scale_mul___muli___56_____bn_conv5_2__sep__scale_mul_____scale___relu5_2__sep_dflip_bit_offset 	9545	/* 0x00002549 */
#define mnet_ssd_adas_flex_pic_prim_split_13_conv5_2__sep_____bn_conv5_2__sep__scale_mul___muli___56_____bn_conv5_2__sep__scale_mul_____scale___relu5_2__sep_pflip_bit_offset 	9546	/* 0x0000254a */
#define mnet_ssd_adas_flex_pic_prim_split_13_conv5_2__sep_____bn_conv5_2__sep__scale_mul___muli___56_____bn_conv5_2__sep__scale_mul_____scale___relu5_2__sep_W 	19
#define mnet_ssd_adas_flex_pic_prim_split_13_conv5_2__sep_____bn_conv5_2__sep__scale_mul___muli___56_____bn_conv5_2__sep__scale_mul_____scale___relu5_2__sep_H 	19
#define mnet_ssd_adas_flex_pic_prim_split_13_conv5_2__sep_____bn_conv5_2__sep__scale_mul___muli___56_____bn_conv5_2__sep__scale_mul_____scale___relu5_2__sep_D 	512
#define mnet_ssd_adas_flex_pic_prim_split_13_conv5_2__sep_____bn_conv5_2__sep__scale_mul___muli___56_____bn_conv5_2__sep__scale_mul_____scale___relu5_2__sep_P 	1
#define mnet_ssd_adas_flex_pic_prim_split_13_conv5_2__sep_____bn_conv5_2__sep__scale_mul___muli___56_____bn_conv5_2__sep__scale_mul_____scale___relu5_2__sep_data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_13_conv5_2__sep_____bn_conv5_2__sep__scale_mul___muli___56_____bn_conv5_2__sep__scale_mul_____scale___relu5_2__sep_denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_13_conv5_2__sep_____bn_conv5_2__sep__scale_mul___muli___56_____bn_conv5_2__sep__scale_mul_____scale___relu5_2__sep_dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_13_conv5_2__sep_____bn_conv5_2__sep__scale_mul___muli___56_____bn_conv5_2__sep__scale_mul_____scale___relu5_2__sep_dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_13_conv5_2__sep_____bn_conv5_2__sep__scale_mul___muli___56_____bn_conv5_2__sep__scale_mul_____scale___relu5_2__sep_vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_13_conv5_2__sep_____bn_conv5_2__sep__scale_mul___muli___56_____bn_conv5_2__sep__scale_mul_____scale___relu5_2__sep_tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_13_conv5_2__sep_____bn_conv5_2__sep__scale_mul___muli___56_____bn_conv5_2__sep__scale_mul_____scale___relu5_2__sep_tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_13_conv5_2__sep_____bn_conv5_2__sep__scale_mul___muli___56_____bn_conv5_2__sep__scale_mul_____scale___relu5_2__sep_vmem_buffer_num_bytes 	49152
#define mnet_ssd_adas_flex_pic_prim_split_13_conv5_2__sep_____bn_conv5_2__sep__scale_mul___muli___56_____bn_conv5_2__sep__scale_mul_____scale___relu5_2__sep_dram_size_num_bytes 	233472
#define mnet_ssd_adas_flex_pic_prim_split_13_conv5_2__sep_____bn_conv5_2__sep__scale_mul___muli___56_____bn_conv5_2__sep__scale_mul_____scale___relu5_2__sep_sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_13_conv5_2__sep_____bn_conv5_2__sep__scale_mul___muli___56_____bn_conv5_2__sep__scale_mul_____scale___relu5_2__sep_datasize 	0
#define mnet_ssd_adas_flex_pic_prim_split_13_conv5_2__sep_____bn_conv5_2__sep__scale_mul___muli___56_____bn_conv5_2__sep__scale_mul_____scale___relu5_2__sep_expoffset 	8
#define mnet_ssd_adas_flex_pic_prim_split_13_conv5_2__sep_____bn_conv5_2__sep__scale_mul___muli___56_____bn_conv5_2__sep__scale_mul_____scale___relu5_2__sep_expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_13_conv5_2__sep_____bn_conv5_2__sep__scale_mul___muli___56_____bn_conv5_2__sep__scale_mul_____scale___relu5_2__sep_drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_13_conv5_2__sep_____bn_conv5_2__sep__scale_mul___muli___56_____bn_conv5_2__sep__scale_mul_____scale___relu5_2__sep_hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_13_conv5_2__sep_____bn_conv5_2__sep__scale_mul___muli___56_____bn_conv5_2__sep__scale_mul_____scale___relu5_2__sep_vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_13_conv5_2__sep_____bn_conv5_2__sep__scale_mul___muli___56_____bn_conv5_2__sep__scale_mul_____scale___relu5_2__sep_dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_13_conv5_2__sep_____bn_conv5_2__sep__scale_mul___muli___56_____bn_conv5_2__sep__scale_mul_____scale___relu5_2__sep_pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_13_conv5_2__sep_____bn_conv5_2__sep__scale_mul___muli___56_____bn_conv5_2__sep__scale_mul_____scale___relu5_2__sep_trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_13 SMB_output __vas_created_terminate__v0 */
#define mnet_ssd_adas_flex_pic_prim_split_13___vas_created_terminate__v0_cnngen_demangled_name 	"__vas_created_terminate__v0"
#define mnet_ssd_adas_flex_pic_prim_split_13___vas_created_terminate__v0_is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___vas_created_terminate__v0_is_bitvector 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___vas_created_terminate__v0_is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___vas_created_terminate__v0_is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___vas_created_terminate__v0_byte_offset 	1208	/* 0x000004b8 */
#define mnet_ssd_adas_flex_pic_prim_split_13___vas_created_terminate__v0_vbase_byte_offset 	1212	/* 0x000004bc */
#define mnet_ssd_adas_flex_pic_prim_split_13___vas_created_terminate__v0_W 	256
#define mnet_ssd_adas_flex_pic_prim_split_13___vas_created_terminate__v0_H 	3
#define mnet_ssd_adas_flex_pic_prim_split_13___vas_created_terminate__v0_D 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___vas_created_terminate__v0_P 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___vas_created_terminate__v0_data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___vas_created_terminate__v0_denable 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___vas_created_terminate__v0_dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_13___vas_created_terminate__v0_vp_interleave_mode 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___vas_created_terminate__v0_tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___vas_created_terminate__v0_tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_13___vas_created_terminate__v0_vmem_buffer_num_bytes 	96


#endif /* mnet_ssd_adas_flex_pic_prim_split_13_H */
