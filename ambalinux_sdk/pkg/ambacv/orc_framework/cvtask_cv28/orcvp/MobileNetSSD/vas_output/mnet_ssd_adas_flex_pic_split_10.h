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
#ifndef mnet_ssd_adas_flex_pic_prim_split_10_H
#define mnet_ssd_adas_flex_pic_prim_split_10_H

#define vas_gen_header_version                  	7

/* dag name */
#define mnet_ssd_adas_flex_pic_prim_split_10_DAG_NAME 	"mnet_ssd_adas_flex_pic_prim_split_10"

/* VDG file info */
#define mnet_ssd_adas_flex_pic_prim_split_10_vdg_name 	"mnet_ssd_adas_flex_pic_split_10.vdg"

/* VDG DAG memory info */
#define mnet_ssd_adas_flex_pic_prim_split_10_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_10_byte_size 	1132	/* 0x0000046c */
#define mnet_ssd_adas_flex_pic_prim_split_10_preferred_dram_xfer_size 	1

/* mnet_ssd_adas_flex_pic_prim_split_10 VMEM info */
#define mnet_ssd_adas_flex_pic_prim_split_10_use_ping_pong_vmem 	0
#define mnet_ssd_adas_flex_pic_prim_split_10_VMEM_start 	4096	/* 0x00001000 */
#define mnet_ssd_adas_flex_pic_prim_split_10_VMEM_end 	510204	/* 0x0007c8fc */
#define mnet_ssd_adas_flex_pic_prim_split_10_image_start 	385248	/* 0x0005e0e0 */
#define mnet_ssd_adas_flex_pic_prim_split_10_image_size 	126088	/* 0x0001ec88 */
#define mnet_ssd_adas_flex_pic_prim_split_10_dagbin_start 	510204	/* 0x0007c8fc */

/* mnet_ssd_adas_flex_pic_prim_split_10 DAG info */
#define mnet_ssd_adas_flex_pic_prim_split_10_estimated_cycles 	65709
#define mnet_ssd_adas_flex_pic_prim_split_10_ddi_byte_size 	0	/* 0x00000000 */

/*******************************************************************************
 * DAG byte offset info for HMB/SMB/VCB and their fields
 * in DAG 'mnet_ssd_adas_flex_pic_prim_split_10' in source file 'pre_split10_mnet_ssd_adas_flex_pic_split_0.vas'
 ******************************************************************************/
/* mnet_ssd_adas_flex_pic_prim_split_10 HMB_input __pvcn_1073_ */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1073__cnngen_demangled_name 	"__pvcn_1073_"
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1073__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1073__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1073__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1073__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1073__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1073__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1073__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1073__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1073__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1073__byte_offset 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1073__vbase_byte_offset 	4	/* 0x00000004 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1073__dbase_byte_offset 	28	/* 0x0000001c */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1073__dpitchm1_byte_offset 	32	/* 0x00000020 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1073__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1073__vwidth_minus_one_byte_offset 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1073__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1073__vheight_minus_one_byte_offset 	18	/* 0x00000012 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1073__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1073__drotate_bit_offset 	198	/* 0x000000c6 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1073__hflip_bit_offset 	199	/* 0x000000c7 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1073__vflip_bit_offset 	200	/* 0x000000c8 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1073__dflip_bit_offset 	201	/* 0x000000c9 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1073__pflip_bit_offset 	202	/* 0x000000ca */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1073__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1073__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1073__D 	21
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1073__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1073__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1073__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1073__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1073__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1073__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1073__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1073__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1073__vmem_buffer_num_bytes 	16128
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1073__dram_size_num_bytes 	19168
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1073__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1073__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1073__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1073__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1073__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1073__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1073__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1073__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1073__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1073__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_10 HMB_input __pvcn_963_ */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_963__cnngen_demangled_name 	"__pvcn_963_"
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_963__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_963__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_963__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_963__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_963__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_963__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_963__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_963__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_963__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_963__byte_offset 	40	/* 0x00000028 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_963__vbase_byte_offset 	44	/* 0x0000002c */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_963__dbase_byte_offset 	68	/* 0x00000044 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_963__dpitchm1_byte_offset 	72	/* 0x00000048 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_963__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_963__vwidth_minus_one_byte_offset 	56	/* 0x00000038 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_963__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_963__vheight_minus_one_byte_offset 	58	/* 0x0000003a */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_963__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_963__drotate_bit_offset 	518	/* 0x00000206 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_963__hflip_bit_offset 	519	/* 0x00000207 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_963__vflip_bit_offset 	520	/* 0x00000208 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_963__dflip_bit_offset 	521	/* 0x00000209 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_963__pflip_bit_offset 	522	/* 0x0000020a */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_963__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_963__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_963__D 	24
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_963__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_963__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_963__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_963__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_963__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_963__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_963__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_963__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_963__vmem_buffer_num_bytes 	18432
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_963__dram_size_num_bytes 	21888
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_963__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_963__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_963__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_963__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_963__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_963__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_963__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_963__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_963__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_963__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_10 HMB_input __pvcn_996_ */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_996__cnngen_demangled_name 	"__pvcn_996_"
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_996__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_996__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_996__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_996__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_996__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_996__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_996__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_996__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_996__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_996__byte_offset 	76	/* 0x0000004c */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_996__vbase_byte_offset 	80	/* 0x00000050 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_996__dbase_byte_offset 	104	/* 0x00000068 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_996__dpitchm1_byte_offset 	108	/* 0x0000006c */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_996__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_996__vwidth_minus_one_byte_offset 	92	/* 0x0000005c */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_996__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_996__vheight_minus_one_byte_offset 	94	/* 0x0000005e */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_996__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_996__drotate_bit_offset 	806	/* 0x00000326 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_996__hflip_bit_offset 	807	/* 0x00000327 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_996__vflip_bit_offset 	808	/* 0x00000328 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_996__dflip_bit_offset 	809	/* 0x00000329 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_996__pflip_bit_offset 	810	/* 0x0000032a */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_996__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_996__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_996__D 	23
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_996__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_996__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_996__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_996__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_996__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_996__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_996__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_996__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_996__vmem_buffer_num_bytes 	17664
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_996__dram_size_num_bytes 	20992
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_996__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_996__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_996__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_996__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_996__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_996__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_996__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_996__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_996__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_996__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_10 HMB_input __pvcn_1084_ */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1084__cnngen_demangled_name 	"__pvcn_1084_"
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1084__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1084__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1084__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1084__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1084__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1084__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1084__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1084__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1084__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1084__byte_offset 	116	/* 0x00000074 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1084__vbase_byte_offset 	120	/* 0x00000078 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1084__dbase_byte_offset 	144	/* 0x00000090 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1084__dpitchm1_byte_offset 	148	/* 0x00000094 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1084__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1084__vwidth_minus_one_byte_offset 	132	/* 0x00000084 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1084__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1084__vheight_minus_one_byte_offset 	134	/* 0x00000086 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1084__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1084__drotate_bit_offset 	1126	/* 0x00000466 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1084__hflip_bit_offset 	1127	/* 0x00000467 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1084__vflip_bit_offset 	1128	/* 0x00000468 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1084__dflip_bit_offset 	1129	/* 0x00000469 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1084__pflip_bit_offset 	1130	/* 0x0000046a */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1084__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1084__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1084__D 	14
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1084__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1084__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1084__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1084__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1084__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1084__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1084__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1084__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1084__vmem_buffer_num_bytes 	10752
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1084__dram_size_num_bytes 	12768
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1084__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1084__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1084__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1084__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1084__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1084__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1084__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1084__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1084__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1084__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_10 HMB_input __pvcn_1029_ */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1029__cnngen_demangled_name 	"__pvcn_1029_"
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1029__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1029__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1029__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1029__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1029__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1029__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1029__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1029__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1029__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1029__byte_offset 	156	/* 0x0000009c */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1029__vbase_byte_offset 	160	/* 0x000000a0 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1029__dbase_byte_offset 	184	/* 0x000000b8 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1029__dpitchm1_byte_offset 	188	/* 0x000000bc */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1029__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1029__vwidth_minus_one_byte_offset 	172	/* 0x000000ac */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1029__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1029__vheight_minus_one_byte_offset 	174	/* 0x000000ae */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1029__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1029__drotate_bit_offset 	1446	/* 0x000005a6 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1029__hflip_bit_offset 	1447	/* 0x000005a7 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1029__vflip_bit_offset 	1448	/* 0x000005a8 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1029__dflip_bit_offset 	1449	/* 0x000005a9 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1029__pflip_bit_offset 	1450	/* 0x000005aa */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1029__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1029__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1029__D 	23
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1029__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1029__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1029__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1029__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1029__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1029__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1029__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1029__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1029__vmem_buffer_num_bytes 	17664
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1029__dram_size_num_bytes 	20992
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1029__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1029__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1029__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1029__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1029__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1029__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1029__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1029__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1029__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1029__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_10 HMB_input __pvcn_985_ */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_985__cnngen_demangled_name 	"__pvcn_985_"
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_985__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_985__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_985__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_985__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_985__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_985__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_985__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_985__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_985__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_985__byte_offset 	196	/* 0x000000c4 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_985__vbase_byte_offset 	200	/* 0x000000c8 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_985__dbase_byte_offset 	224	/* 0x000000e0 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_985__dpitchm1_byte_offset 	228	/* 0x000000e4 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_985__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_985__vwidth_minus_one_byte_offset 	212	/* 0x000000d4 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_985__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_985__vheight_minus_one_byte_offset 	214	/* 0x000000d6 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_985__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_985__drotate_bit_offset 	1766	/* 0x000006e6 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_985__hflip_bit_offset 	1767	/* 0x000006e7 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_985__vflip_bit_offset 	1768	/* 0x000006e8 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_985__dflip_bit_offset 	1769	/* 0x000006e9 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_985__pflip_bit_offset 	1770	/* 0x000006ea */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_985__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_985__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_985__D 	22
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_985__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_985__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_985__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_985__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_985__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_985__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_985__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_985__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_985__vmem_buffer_num_bytes 	16896
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_985__dram_size_num_bytes 	20064
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_985__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_985__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_985__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_985__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_985__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_985__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_985__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_985__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_985__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_985__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_10 HMB_input __pvcn_1051_ */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1051__cnngen_demangled_name 	"__pvcn_1051_"
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1051__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1051__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1051__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1051__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1051__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1051__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1051__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1051__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1051__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1051__byte_offset 	236	/* 0x000000ec */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1051__vbase_byte_offset 	240	/* 0x000000f0 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1051__dbase_byte_offset 	264	/* 0x00000108 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1051__dpitchm1_byte_offset 	268	/* 0x0000010c */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1051__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1051__vwidth_minus_one_byte_offset 	252	/* 0x000000fc */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1051__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1051__vheight_minus_one_byte_offset 	254	/* 0x000000fe */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1051__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1051__drotate_bit_offset 	2086	/* 0x00000826 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1051__hflip_bit_offset 	2087	/* 0x00000827 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1051__vflip_bit_offset 	2088	/* 0x00000828 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1051__dflip_bit_offset 	2089	/* 0x00000829 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1051__pflip_bit_offset 	2090	/* 0x0000082a */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1051__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1051__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1051__D 	24
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1051__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1051__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1051__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1051__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1051__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1051__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1051__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1051__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1051__vmem_buffer_num_bytes 	18432
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1051__dram_size_num_bytes 	21888
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1051__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1051__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1051__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1051__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1051__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1051__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1051__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1051__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1051__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1051__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_10 HMB_input __pvcn_1062_ */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1062__cnngen_demangled_name 	"__pvcn_1062_"
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1062__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1062__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1062__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1062__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1062__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1062__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1062__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1062__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1062__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1062__byte_offset 	276	/* 0x00000114 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1062__vbase_byte_offset 	280	/* 0x00000118 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1062__dbase_byte_offset 	304	/* 0x00000130 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1062__dpitchm1_byte_offset 	308	/* 0x00000134 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1062__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1062__vwidth_minus_one_byte_offset 	292	/* 0x00000124 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1062__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1062__vheight_minus_one_byte_offset 	294	/* 0x00000126 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1062__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1062__drotate_bit_offset 	2406	/* 0x00000966 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1062__hflip_bit_offset 	2407	/* 0x00000967 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1062__vflip_bit_offset 	2408	/* 0x00000968 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1062__dflip_bit_offset 	2409	/* 0x00000969 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1062__pflip_bit_offset 	2410	/* 0x0000096a */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1062__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1062__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1062__D 	22
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1062__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1062__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1062__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1062__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1062__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1062__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1062__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1062__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1062__vmem_buffer_num_bytes 	16896
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1062__dram_size_num_bytes 	20064
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1062__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1062__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1062__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1062__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1062__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1062__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1062__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1062__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1062__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1062__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_10 HMB_input __pvcn_1040_ */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1040__cnngen_demangled_name 	"__pvcn_1040_"
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1040__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1040__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1040__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1040__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1040__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1040__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1040__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1040__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1040__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1040__byte_offset 	316	/* 0x0000013c */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1040__vbase_byte_offset 	320	/* 0x00000140 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1040__dbase_byte_offset 	344	/* 0x00000158 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1040__dpitchm1_byte_offset 	348	/* 0x0000015c */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1040__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1040__vwidth_minus_one_byte_offset 	332	/* 0x0000014c */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1040__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1040__vheight_minus_one_byte_offset 	334	/* 0x0000014e */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1040__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1040__drotate_bit_offset 	2726	/* 0x00000aa6 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1040__hflip_bit_offset 	2727	/* 0x00000aa7 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1040__vflip_bit_offset 	2728	/* 0x00000aa8 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1040__dflip_bit_offset 	2729	/* 0x00000aa9 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1040__pflip_bit_offset 	2730	/* 0x00000aaa */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1040__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1040__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1040__D 	24
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1040__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1040__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1040__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1040__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1040__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1040__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1040__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1040__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1040__vmem_buffer_num_bytes 	18432
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1040__dram_size_num_bytes 	21888
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1040__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1040__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1040__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1040__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1040__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1040__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1040__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1040__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1040__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1040__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_10 HMB_input __pvcn_1007_ */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1007__cnngen_demangled_name 	"__pvcn_1007_"
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1007__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1007__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1007__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1007__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1007__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1007__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1007__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1007__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1007__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1007__byte_offset 	356	/* 0x00000164 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1007__vbase_byte_offset 	360	/* 0x00000168 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1007__dbase_byte_offset 	384	/* 0x00000180 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1007__dpitchm1_byte_offset 	388	/* 0x00000184 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1007__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1007__vwidth_minus_one_byte_offset 	372	/* 0x00000174 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1007__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1007__vheight_minus_one_byte_offset 	374	/* 0x00000176 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1007__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1007__drotate_bit_offset 	3046	/* 0x00000be6 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1007__hflip_bit_offset 	3047	/* 0x00000be7 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1007__vflip_bit_offset 	3048	/* 0x00000be8 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1007__dflip_bit_offset 	3049	/* 0x00000be9 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1007__pflip_bit_offset 	3050	/* 0x00000bea */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1007__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1007__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1007__D 	18
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1007__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1007__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1007__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1007__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1007__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1007__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1007__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1007__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1007__vmem_buffer_num_bytes 	13824
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1007__dram_size_num_bytes 	16416
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1007__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1007__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1007__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1007__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1007__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1007__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1007__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1007__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1007__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1007__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_10 HMB_input __pvcn_1018_ */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1018__cnngen_demangled_name 	"__pvcn_1018_"
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1018__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1018__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1018__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1018__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1018__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1018__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1018__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1018__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1018__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1018__byte_offset 	396	/* 0x0000018c */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1018__vbase_byte_offset 	400	/* 0x00000190 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1018__dbase_byte_offset 	424	/* 0x000001a8 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1018__dpitchm1_byte_offset 	428	/* 0x000001ac */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1018__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1018__vwidth_minus_one_byte_offset 	412	/* 0x0000019c */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1018__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1018__vheight_minus_one_byte_offset 	414	/* 0x0000019e */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1018__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1018__drotate_bit_offset 	3366	/* 0x00000d26 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1018__hflip_bit_offset 	3367	/* 0x00000d27 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1018__vflip_bit_offset 	3368	/* 0x00000d28 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1018__dflip_bit_offset 	3369	/* 0x00000d29 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1018__pflip_bit_offset 	3370	/* 0x00000d2a */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1018__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1018__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1018__D 	20
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1018__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1018__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1018__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1018__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1018__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1018__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1018__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1018__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1018__vmem_buffer_num_bytes 	15360
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1018__dram_size_num_bytes 	18240
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1018__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1018__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1018__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1018__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1018__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1018__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1018__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1018__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1018__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1018__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_10 HMB_input __pvcn_974_ */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_974__cnngen_demangled_name 	"__pvcn_974_"
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_974__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_974__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_974__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_974__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_974__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_974__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_974__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_974__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_974__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_974__byte_offset 	436	/* 0x000001b4 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_974__vbase_byte_offset 	440	/* 0x000001b8 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_974__dbase_byte_offset 	464	/* 0x000001d0 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_974__dpitchm1_byte_offset 	468	/* 0x000001d4 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_974__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_974__vwidth_minus_one_byte_offset 	452	/* 0x000001c4 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_974__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_974__vheight_minus_one_byte_offset 	454	/* 0x000001c6 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_974__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_974__drotate_bit_offset 	3686	/* 0x00000e66 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_974__hflip_bit_offset 	3687	/* 0x00000e67 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_974__vflip_bit_offset 	3688	/* 0x00000e68 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_974__dflip_bit_offset 	3689	/* 0x00000e69 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_974__pflip_bit_offset 	3690	/* 0x00000e6a */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_974__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_974__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_974__D 	21
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_974__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_974__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_974__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_974__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_974__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_974__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_974__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_974__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_974__vmem_buffer_num_bytes 	8064
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_974__dram_size_num_bytes 	19168
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_974__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_974__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_974__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_974__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_974__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_974__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_974__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_974__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_974__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_974__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_10 SMB_input __pvcn_1117_ */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1117__cnngen_demangled_name 	"__pvcn_1117_"
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1117__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1117__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1117__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1117__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1117__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1117__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1117__byte_offset 	484	/* 0x000001e4 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1117__vbase_byte_offset 	488	/* 0x000001e8 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1117__dbase_byte_offset 	512	/* 0x00000200 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1117__dpitchm1_byte_offset 	516	/* 0x00000204 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1117__dpitchm1_bsize 	32	/* 0x00000020 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1117__W 	122352
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1117__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1117__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1117__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1117__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1117__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1117__dpitch_num_bytes 	122368
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1117__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1117__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1117__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1117__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1117__vmem_buffer_num_bytes 	122352
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1117__dram_size_num_bytes 	122368

/* mnet_ssd_adas_flex_pic_prim_split_10 SMB_input __pvcn_1118_ */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1118__cnngen_demangled_name 	"__pvcn_1118_"
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1118__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1118__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1118__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1118__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1118__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1118__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1118__byte_offset 	524	/* 0x0000020c */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1118__vbase_byte_offset 	528	/* 0x00000210 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1118__dbase_byte_offset 	552	/* 0x00000228 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1118__dpitchm1_byte_offset 	556	/* 0x0000022c */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1118__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1118__W 	2604
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1118__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1118__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1118__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1118__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1118__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1118__dpitch_num_bytes 	2624
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1118__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1118__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1118__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1118__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1118__vmem_buffer_num_bytes 	2604
#define mnet_ssd_adas_flex_pic_prim_split_10___pvcn_1118__dram_size_num_bytes 	2624

/* mnet_ssd_adas_flex_pic_prim_split_10 VCB conv4_2__dw_____bn_conv4_2__dw__scale_mul___muli___41_____bn_conv4_2__dw__scale_mul_____scale___muli___345 */
#define mnet_ssd_adas_flex_pic_prim_split_10_conv4_2__dw_____bn_conv4_2__dw__scale_mul___muli___41_____bn_conv4_2__dw__scale_mul_____scale___muli___345_cnngen_demangled_name 	"conv4_2__dw_____bn_conv4_2__dw__scale_mul___muli___41_____bn_conv4_2__dw__scale_mul_____scale___muli___345"
#define mnet_ssd_adas_flex_pic_prim_split_10_conv4_2__dw_____bn_conv4_2__dw__scale_mul___muli___41_____bn_conv4_2__dw__scale_mul_____scale___muli___345_is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_10_conv4_2__dw_____bn_conv4_2__dw__scale_mul___muli___41_____bn_conv4_2__dw__scale_mul_____scale___muli___345_is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_10_conv4_2__dw_____bn_conv4_2__dw__scale_mul___muli___41_____bn_conv4_2__dw__scale_mul_____scale___muli___345_is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_10_conv4_2__dw_____bn_conv4_2__dw__scale_mul___muli___41_____bn_conv4_2__dw__scale_mul_____scale___muli___345_byte_offset 	968	/* 0x000003c8 */
#define mnet_ssd_adas_flex_pic_prim_split_10_conv4_2__dw_____bn_conv4_2__dw__scale_mul___muli___41_____bn_conv4_2__dw__scale_mul_____scale___muli___345_vbase_byte_offset 	976	/* 0x000003d0 */
#define mnet_ssd_adas_flex_pic_prim_split_10_conv4_2__dw_____bn_conv4_2__dw__scale_mul___muli___41_____bn_conv4_2__dw__scale_mul_____scale___muli___345_vmem_buffer_num_bytes 	90112

/* mnet_ssd_adas_flex_pic_prim_split_10 HMB_output conv4_2__sep_____bn_conv4_2__sep__scale_mul___muli___44_____bn_conv4_2__sep__scale_mul_____scale___relu4_2__sep */
#define mnet_ssd_adas_flex_pic_prim_split_10_conv4_2__sep_____bn_conv4_2__sep__scale_mul___muli___44_____bn_conv4_2__sep__scale_mul_____scale___relu4_2__sep_cnngen_demangled_name 	"conv4_2__sep_____bn_conv4_2__sep__scale_mul___muli___44_____bn_conv4_2__sep__scale_mul_____scale___relu4_2__sep"
#define mnet_ssd_adas_flex_pic_prim_split_10_conv4_2__sep_____bn_conv4_2__sep__scale_mul___muli___44_____bn_conv4_2__sep__scale_mul_____scale___relu4_2__sep_is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_10_conv4_2__sep_____bn_conv4_2__sep__scale_mul___muli___44_____bn_conv4_2__sep__scale_mul_____scale___relu4_2__sep_is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_10_conv4_2__sep_____bn_conv4_2__sep__scale_mul___muli___44_____bn_conv4_2__sep__scale_mul_____scale___relu4_2__sep_is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_10_conv4_2__sep_____bn_conv4_2__sep__scale_mul___muli___44_____bn_conv4_2__sep__scale_mul_____scale___relu4_2__sep_is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_10_conv4_2__sep_____bn_conv4_2__sep__scale_mul___muli___44_____bn_conv4_2__sep__scale_mul_____scale___relu4_2__sep_byte_offset 	1016	/* 0x000003f8 */
#define mnet_ssd_adas_flex_pic_prim_split_10_conv4_2__sep_____bn_conv4_2__sep__scale_mul___muli___44_____bn_conv4_2__sep__scale_mul_____scale___relu4_2__sep_vbase_byte_offset 	1020	/* 0x000003fc */
#define mnet_ssd_adas_flex_pic_prim_split_10_conv4_2__sep_____bn_conv4_2__sep__scale_mul___muli___44_____bn_conv4_2__sep__scale_mul_____scale___relu4_2__sep_dbase_byte_offset 	1044	/* 0x00000414 */
#define mnet_ssd_adas_flex_pic_prim_split_10_conv4_2__sep_____bn_conv4_2__sep__scale_mul___muli___44_____bn_conv4_2__sep__scale_mul_____scale___relu4_2__sep_dpitchm1_byte_offset 	1048	/* 0x00000418 */
#define mnet_ssd_adas_flex_pic_prim_split_10_conv4_2__sep_____bn_conv4_2__sep__scale_mul___muli___44_____bn_conv4_2__sep__scale_mul_____scale___relu4_2__sep_dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_10_conv4_2__sep_____bn_conv4_2__sep__scale_mul___muli___44_____bn_conv4_2__sep__scale_mul_____scale___relu4_2__sep_vwidth_minus_one_byte_offset 	1032	/* 0x00000408 */
#define mnet_ssd_adas_flex_pic_prim_split_10_conv4_2__sep_____bn_conv4_2__sep__scale_mul___muli___44_____bn_conv4_2__sep__scale_mul_____scale___relu4_2__sep_vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_10_conv4_2__sep_____bn_conv4_2__sep__scale_mul___muli___44_____bn_conv4_2__sep__scale_mul_____scale___relu4_2__sep_vheight_minus_one_byte_offset 	1034	/* 0x0000040a */
#define mnet_ssd_adas_flex_pic_prim_split_10_conv4_2__sep_____bn_conv4_2__sep__scale_mul___muli___44_____bn_conv4_2__sep__scale_mul_____scale___relu4_2__sep_vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_10_conv4_2__sep_____bn_conv4_2__sep__scale_mul___muli___44_____bn_conv4_2__sep__scale_mul_____scale___relu4_2__sep_drotate_bit_offset 	8326	/* 0x00002086 */
#define mnet_ssd_adas_flex_pic_prim_split_10_conv4_2__sep_____bn_conv4_2__sep__scale_mul___muli___44_____bn_conv4_2__sep__scale_mul_____scale___relu4_2__sep_hflip_bit_offset 	8327	/* 0x00002087 */
#define mnet_ssd_adas_flex_pic_prim_split_10_conv4_2__sep_____bn_conv4_2__sep__scale_mul___muli___44_____bn_conv4_2__sep__scale_mul_____scale___relu4_2__sep_vflip_bit_offset 	8328	/* 0x00002088 */
#define mnet_ssd_adas_flex_pic_prim_split_10_conv4_2__sep_____bn_conv4_2__sep__scale_mul___muli___44_____bn_conv4_2__sep__scale_mul_____scale___relu4_2__sep_dflip_bit_offset 	8329	/* 0x00002089 */
#define mnet_ssd_adas_flex_pic_prim_split_10_conv4_2__sep_____bn_conv4_2__sep__scale_mul___muli___44_____bn_conv4_2__sep__scale_mul_____scale___relu4_2__sep_pflip_bit_offset 	8330	/* 0x0000208a */
#define mnet_ssd_adas_flex_pic_prim_split_10_conv4_2__sep_____bn_conv4_2__sep__scale_mul___muli___44_____bn_conv4_2__sep__scale_mul_____scale___relu4_2__sep_W 	19
#define mnet_ssd_adas_flex_pic_prim_split_10_conv4_2__sep_____bn_conv4_2__sep__scale_mul___muli___44_____bn_conv4_2__sep__scale_mul_____scale___relu4_2__sep_H 	19
#define mnet_ssd_adas_flex_pic_prim_split_10_conv4_2__sep_____bn_conv4_2__sep__scale_mul___muli___44_____bn_conv4_2__sep__scale_mul_____scale___relu4_2__sep_D 	512
#define mnet_ssd_adas_flex_pic_prim_split_10_conv4_2__sep_____bn_conv4_2__sep__scale_mul___muli___44_____bn_conv4_2__sep__scale_mul_____scale___relu4_2__sep_P 	1
#define mnet_ssd_adas_flex_pic_prim_split_10_conv4_2__sep_____bn_conv4_2__sep__scale_mul___muli___44_____bn_conv4_2__sep__scale_mul_____scale___relu4_2__sep_data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_10_conv4_2__sep_____bn_conv4_2__sep__scale_mul___muli___44_____bn_conv4_2__sep__scale_mul_____scale___relu4_2__sep_denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_10_conv4_2__sep_____bn_conv4_2__sep__scale_mul___muli___44_____bn_conv4_2__sep__scale_mul_____scale___relu4_2__sep_dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_10_conv4_2__sep_____bn_conv4_2__sep__scale_mul___muli___44_____bn_conv4_2__sep__scale_mul_____scale___relu4_2__sep_dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_10_conv4_2__sep_____bn_conv4_2__sep__scale_mul___muli___44_____bn_conv4_2__sep__scale_mul_____scale___relu4_2__sep_vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_10_conv4_2__sep_____bn_conv4_2__sep__scale_mul___muli___44_____bn_conv4_2__sep__scale_mul_____scale___relu4_2__sep_tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_10_conv4_2__sep_____bn_conv4_2__sep__scale_mul___muli___44_____bn_conv4_2__sep__scale_mul_____scale___relu4_2__sep_tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_10_conv4_2__sep_____bn_conv4_2__sep__scale_mul___muli___44_____bn_conv4_2__sep__scale_mul_____scale___relu4_2__sep_vmem_buffer_num_bytes 	102400
#define mnet_ssd_adas_flex_pic_prim_split_10_conv4_2__sep_____bn_conv4_2__sep__scale_mul___muli___44_____bn_conv4_2__sep__scale_mul_____scale___relu4_2__sep_dram_size_num_bytes 	233472
#define mnet_ssd_adas_flex_pic_prim_split_10_conv4_2__sep_____bn_conv4_2__sep__scale_mul___muli___44_____bn_conv4_2__sep__scale_mul_____scale___relu4_2__sep_sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_10_conv4_2__sep_____bn_conv4_2__sep__scale_mul___muli___44_____bn_conv4_2__sep__scale_mul_____scale___relu4_2__sep_datasize 	0
#define mnet_ssd_adas_flex_pic_prim_split_10_conv4_2__sep_____bn_conv4_2__sep__scale_mul___muli___44_____bn_conv4_2__sep__scale_mul_____scale___relu4_2__sep_expoffset 	8
#define mnet_ssd_adas_flex_pic_prim_split_10_conv4_2__sep_____bn_conv4_2__sep__scale_mul___muli___44_____bn_conv4_2__sep__scale_mul_____scale___relu4_2__sep_expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_10_conv4_2__sep_____bn_conv4_2__sep__scale_mul___muli___44_____bn_conv4_2__sep__scale_mul_____scale___relu4_2__sep_drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_10_conv4_2__sep_____bn_conv4_2__sep__scale_mul___muli___44_____bn_conv4_2__sep__scale_mul_____scale___relu4_2__sep_hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_10_conv4_2__sep_____bn_conv4_2__sep__scale_mul___muli___44_____bn_conv4_2__sep__scale_mul_____scale___relu4_2__sep_vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_10_conv4_2__sep_____bn_conv4_2__sep__scale_mul___muli___44_____bn_conv4_2__sep__scale_mul_____scale___relu4_2__sep_dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_10_conv4_2__sep_____bn_conv4_2__sep__scale_mul___muli___44_____bn_conv4_2__sep__scale_mul_____scale___relu4_2__sep_pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_10_conv4_2__sep_____bn_conv4_2__sep__scale_mul___muli___44_____bn_conv4_2__sep__scale_mul_____scale___relu4_2__sep_trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_10 SMB_output __vas_created_terminate__v0 */
#define mnet_ssd_adas_flex_pic_prim_split_10___vas_created_terminate__v0_cnngen_demangled_name 	"__vas_created_terminate__v0"
#define mnet_ssd_adas_flex_pic_prim_split_10___vas_created_terminate__v0_is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___vas_created_terminate__v0_is_bitvector 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___vas_created_terminate__v0_is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___vas_created_terminate__v0_is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___vas_created_terminate__v0_byte_offset 	1056	/* 0x00000420 */
#define mnet_ssd_adas_flex_pic_prim_split_10___vas_created_terminate__v0_vbase_byte_offset 	1060	/* 0x00000424 */
#define mnet_ssd_adas_flex_pic_prim_split_10___vas_created_terminate__v0_W 	256
#define mnet_ssd_adas_flex_pic_prim_split_10___vas_created_terminate__v0_H 	3
#define mnet_ssd_adas_flex_pic_prim_split_10___vas_created_terminate__v0_D 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___vas_created_terminate__v0_P 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___vas_created_terminate__v0_data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___vas_created_terminate__v0_denable 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___vas_created_terminate__v0_dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_10___vas_created_terminate__v0_vp_interleave_mode 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___vas_created_terminate__v0_tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___vas_created_terminate__v0_tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_10___vas_created_terminate__v0_vmem_buffer_num_bytes 	96


#endif /* mnet_ssd_adas_flex_pic_prim_split_10_H */
