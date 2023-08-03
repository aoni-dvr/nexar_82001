////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Ambarella, Inc.
////////////////////////////////////////////////////////////////////////////////
// This file and its contents ("Software") are protected
// by intellectual property rights including, without limitation, U.S.
// and/or foreign copyrights.  This Software is also the confidential
// and proprietary information of Ambarella, Inc. and its licensors.
// You may not use, reproduce, disclose, distribute, modify, or
// otherwise prepare derivative works of this Software or any portion
// thereof except pursuant to a signed license agreement or
// nondisclosure agreement with Ambarella, Inc. or its authorized
// affiliates.  In the absence of such an agreement, you agree to
// promptly notify and return this Software to Ambarella, Inc.
//
// THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL AMBARELLA, INC. OR ITS
// AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.
////////////////////////////////////////////////////////////////////////////////
#ifndef flexidag_openseg_prim_split_2_H
#define flexidag_openseg_prim_split_2_H

#define vas_gen_header_version                  	7

/* dag name */
#define flexidag_openseg_prim_split_2_DAG_NAME  	"flexidag_openseg_prim_split_2"

/* VDG file info */
#define flexidag_openseg_prim_split_2_vdg_name  	"flexidag_openseg_split_2.vdg"

/* VDG DAG memory info */
#define flexidag_openseg_prim_split_2_byte_offset 	4294967295	/* 0xffffffff */
#define flexidag_openseg_prim_split_2_byte_size 	2540	/* 0x000009ec */
#define flexidag_openseg_prim_split_2_preferred_dram_xfer_size 	1

/* flexidag_openseg_prim_split_2 VMEM info */
#define flexidag_openseg_prim_split_2_use_ping_pong_vmem 	0
#define flexidag_openseg_prim_split_2_VMEM_start 	4096	/* 0x00001000 */
#define flexidag_openseg_prim_split_2_VMEM_end  	2071784	/* 0x001f9ce8 */
#define flexidag_openseg_prim_split_2_image_start 	1953584	/* 0x001dcf30 */
#define flexidag_openseg_prim_split_2_image_size 	120740	/* 0x0001d7a4 */
#define flexidag_openseg_prim_split_2_dagbin_start 	2071784	/* 0x001f9ce8 */

/* flexidag_openseg_prim_split_2 DAG info */
#define flexidag_openseg_prim_split_2_estimated_cycles 	848068

/*******************************************************************************
 * DAG byte offset info for HMB/SMB/VCB and their fields
 * in DAG 'flexidag_openseg_prim_split_2' in source file 'flexidag_openseg_split_2.vas'
 ******************************************************************************/
/* flexidag_openseg_prim_split_2 HMB_input PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_cnngen_demangled_name 	"PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool"
#define flexidag_openseg_prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_is_main_input_output 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_is_constant 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_has_init_data 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_is_variable_scalar 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_ddi_byte_size 	0	/* 0x00000000 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_ddi_byte_offset 	4294967295	/* 0xffffffff */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_is_bitvector 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_is_rlz 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_byte_offset 	0	/* 0x00000000 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_vbase_byte_offset 	4	/* 0x00000004 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_dbase_byte_offset 	28	/* 0x0000001c */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_dpitchm1_byte_offset 	32	/* 0x00000020 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_vwidth_minus_one_byte_offset 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_vheight_minus_one_byte_offset 	18	/* 0x00000012 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_vheight_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_drotate_bit_offset 	198	/* 0x000000c6 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_hflip_bit_offset 	199	/* 0x000000c7 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_vflip_bit_offset 	200	/* 0x000000c8 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_dflip_bit_offset 	201	/* 0x000000c9 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_pflip_bit_offset 	202	/* 0x000000ca */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_W 	80
#define flexidag_openseg_prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_H 	32
#define flexidag_openseg_prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_D 	256
#define flexidag_openseg_prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_P 	1
#define flexidag_openseg_prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_data_num_bytes 	1
#define flexidag_openseg_prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_denable 	1
#define flexidag_openseg_prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_dpitch_num_bytes 	96
#define flexidag_openseg_prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_dram_format 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_vp_interleave_mode 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_tile_width 	4
#define flexidag_openseg_prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_tile_height 	1
#define flexidag_openseg_prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_vmem_buffer_num_bytes 	659456
#define flexidag_openseg_prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_dram_size_num_bytes 	786432
#define flexidag_openseg_prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_sign 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_datasize 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_expoffset 	6
#define flexidag_openseg_prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_expbits 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_drotate 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_hflip 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_vflip 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_dflip 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_pflip 	0

/* flexidag_openseg_prim_split_2 SMB_input __pvcn_120004_ */
#define flexidag_openseg_prim_split_2___pvcn_120004__cnngen_demangled_name 	"__pvcn_120004_"
#define flexidag_openseg_prim_split_2___pvcn_120004__is_constant 	1
#define flexidag_openseg_prim_split_2___pvcn_120004__has_init_data 	1
#define flexidag_openseg_prim_split_2___pvcn_120004__is_variable_scalar 	0
#define flexidag_openseg_prim_split_2___pvcn_120004__is_bitvector 	0
#define flexidag_openseg_prim_split_2___pvcn_120004__is_rlz 	0
#define flexidag_openseg_prim_split_2___pvcn_120004__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_120004__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_120004__byte_offset 	48	/* 0x00000030 */
#define flexidag_openseg_prim_split_2___pvcn_120004__vbase_byte_offset 	52	/* 0x00000034 */
#define flexidag_openseg_prim_split_2___pvcn_120004__dbase_byte_offset 	76	/* 0x0000004c */
#define flexidag_openseg_prim_split_2___pvcn_120004__dpitchm1_byte_offset 	80	/* 0x00000050 */
#define flexidag_openseg_prim_split_2___pvcn_120004__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2___pvcn_120004__W 	22020
#define flexidag_openseg_prim_split_2___pvcn_120004__H 	1
#define flexidag_openseg_prim_split_2___pvcn_120004__D 	1
#define flexidag_openseg_prim_split_2___pvcn_120004__P 	1
#define flexidag_openseg_prim_split_2___pvcn_120004__data_num_bytes 	1
#define flexidag_openseg_prim_split_2___pvcn_120004__denable 	1
#define flexidag_openseg_prim_split_2___pvcn_120004__dpitch_num_bytes 	22048
#define flexidag_openseg_prim_split_2___pvcn_120004__dram_format 	0
#define flexidag_openseg_prim_split_2___pvcn_120004__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_2___pvcn_120004__tile_width 	1
#define flexidag_openseg_prim_split_2___pvcn_120004__tile_height 	1
#define flexidag_openseg_prim_split_2___pvcn_120004__vmem_buffer_num_bytes 	22020
#define flexidag_openseg_prim_split_2___pvcn_120004__dram_size_num_bytes 	22048

/* flexidag_openseg_prim_split_2 SMB_input __pvcn_120007_ */
#define flexidag_openseg_prim_split_2___pvcn_120007__cnngen_demangled_name 	"__pvcn_120007_"
#define flexidag_openseg_prim_split_2___pvcn_120007__is_constant 	1
#define flexidag_openseg_prim_split_2___pvcn_120007__has_init_data 	1
#define flexidag_openseg_prim_split_2___pvcn_120007__is_variable_scalar 	0
#define flexidag_openseg_prim_split_2___pvcn_120007__is_bitvector 	0
#define flexidag_openseg_prim_split_2___pvcn_120007__is_rlz 	0
#define flexidag_openseg_prim_split_2___pvcn_120007__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_120007__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_120007__byte_offset 	88	/* 0x00000058 */
#define flexidag_openseg_prim_split_2___pvcn_120007__vbase_byte_offset 	92	/* 0x0000005c */
#define flexidag_openseg_prim_split_2___pvcn_120007__dbase_byte_offset 	116	/* 0x00000074 */
#define flexidag_openseg_prim_split_2___pvcn_120007__dpitchm1_byte_offset 	120	/* 0x00000078 */
#define flexidag_openseg_prim_split_2___pvcn_120007__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2___pvcn_120007__W 	21948
#define flexidag_openseg_prim_split_2___pvcn_120007__H 	1
#define flexidag_openseg_prim_split_2___pvcn_120007__D 	1
#define flexidag_openseg_prim_split_2___pvcn_120007__P 	1
#define flexidag_openseg_prim_split_2___pvcn_120007__data_num_bytes 	1
#define flexidag_openseg_prim_split_2___pvcn_120007__denable 	1
#define flexidag_openseg_prim_split_2___pvcn_120007__dpitch_num_bytes 	21952
#define flexidag_openseg_prim_split_2___pvcn_120007__dram_format 	0
#define flexidag_openseg_prim_split_2___pvcn_120007__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_2___pvcn_120007__tile_width 	1
#define flexidag_openseg_prim_split_2___pvcn_120007__tile_height 	1
#define flexidag_openseg_prim_split_2___pvcn_120007__vmem_buffer_num_bytes 	21948
#define flexidag_openseg_prim_split_2___pvcn_120007__dram_size_num_bytes 	21952

/* flexidag_openseg_prim_split_2 SMB_input __pvcn_120005_ */
#define flexidag_openseg_prim_split_2___pvcn_120005__cnngen_demangled_name 	"__pvcn_120005_"
#define flexidag_openseg_prim_split_2___pvcn_120005__is_constant 	1
#define flexidag_openseg_prim_split_2___pvcn_120005__has_init_data 	1
#define flexidag_openseg_prim_split_2___pvcn_120005__is_variable_scalar 	0
#define flexidag_openseg_prim_split_2___pvcn_120005__is_bitvector 	0
#define flexidag_openseg_prim_split_2___pvcn_120005__is_rlz 	0
#define flexidag_openseg_prim_split_2___pvcn_120005__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_120005__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_120005__byte_offset 	128	/* 0x00000080 */
#define flexidag_openseg_prim_split_2___pvcn_120005__vbase_byte_offset 	132	/* 0x00000084 */
#define flexidag_openseg_prim_split_2___pvcn_120005__dbase_byte_offset 	156	/* 0x0000009c */
#define flexidag_openseg_prim_split_2___pvcn_120005__dpitchm1_byte_offset 	160	/* 0x000000a0 */
#define flexidag_openseg_prim_split_2___pvcn_120005__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2___pvcn_120005__W 	720
#define flexidag_openseg_prim_split_2___pvcn_120005__H 	1
#define flexidag_openseg_prim_split_2___pvcn_120005__D 	1
#define flexidag_openseg_prim_split_2___pvcn_120005__P 	1
#define flexidag_openseg_prim_split_2___pvcn_120005__data_num_bytes 	1
#define flexidag_openseg_prim_split_2___pvcn_120005__denable 	1
#define flexidag_openseg_prim_split_2___pvcn_120005__dpitch_num_bytes 	736
#define flexidag_openseg_prim_split_2___pvcn_120005__dram_format 	0
#define flexidag_openseg_prim_split_2___pvcn_120005__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_2___pvcn_120005__tile_width 	1
#define flexidag_openseg_prim_split_2___pvcn_120005__tile_height 	1
#define flexidag_openseg_prim_split_2___pvcn_120005__vmem_buffer_num_bytes 	720
#define flexidag_openseg_prim_split_2___pvcn_120005__dram_size_num_bytes 	736

/* flexidag_openseg_prim_split_2 SMB_input __pvcn_120008_ */
#define flexidag_openseg_prim_split_2___pvcn_120008__cnngen_demangled_name 	"__pvcn_120008_"
#define flexidag_openseg_prim_split_2___pvcn_120008__is_constant 	1
#define flexidag_openseg_prim_split_2___pvcn_120008__has_init_data 	1
#define flexidag_openseg_prim_split_2___pvcn_120008__is_variable_scalar 	0
#define flexidag_openseg_prim_split_2___pvcn_120008__is_bitvector 	0
#define flexidag_openseg_prim_split_2___pvcn_120008__is_rlz 	0
#define flexidag_openseg_prim_split_2___pvcn_120008__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_120008__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_120008__byte_offset 	168	/* 0x000000a8 */
#define flexidag_openseg_prim_split_2___pvcn_120008__vbase_byte_offset 	172	/* 0x000000ac */
#define flexidag_openseg_prim_split_2___pvcn_120008__dbase_byte_offset 	196	/* 0x000000c4 */
#define flexidag_openseg_prim_split_2___pvcn_120008__dpitchm1_byte_offset 	200	/* 0x000000c8 */
#define flexidag_openseg_prim_split_2___pvcn_120008__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2___pvcn_120008__W 	720
#define flexidag_openseg_prim_split_2___pvcn_120008__H 	1
#define flexidag_openseg_prim_split_2___pvcn_120008__D 	1
#define flexidag_openseg_prim_split_2___pvcn_120008__P 	1
#define flexidag_openseg_prim_split_2___pvcn_120008__data_num_bytes 	1
#define flexidag_openseg_prim_split_2___pvcn_120008__denable 	1
#define flexidag_openseg_prim_split_2___pvcn_120008__dpitch_num_bytes 	736
#define flexidag_openseg_prim_split_2___pvcn_120008__dram_format 	0
#define flexidag_openseg_prim_split_2___pvcn_120008__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_2___pvcn_120008__tile_width 	1
#define flexidag_openseg_prim_split_2___pvcn_120008__tile_height 	1
#define flexidag_openseg_prim_split_2___pvcn_120008__vmem_buffer_num_bytes 	720
#define flexidag_openseg_prim_split_2___pvcn_120008__dram_size_num_bytes 	736

/* flexidag_openseg_prim_split_2 SMB_input __pvcn_1098_ */
#define flexidag_openseg_prim_split_2___pvcn_1098__cnngen_demangled_name 	"__pvcn_1098_"
#define flexidag_openseg_prim_split_2___pvcn_1098__is_constant 	1
#define flexidag_openseg_prim_split_2___pvcn_1098__has_init_data 	1
#define flexidag_openseg_prim_split_2___pvcn_1098__is_variable_scalar 	0
#define flexidag_openseg_prim_split_2___pvcn_1098__is_bitvector 	0
#define flexidag_openseg_prim_split_2___pvcn_1098__is_rlz 	0
#define flexidag_openseg_prim_split_2___pvcn_1098__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_1098__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_1098__byte_offset 	208	/* 0x000000d0 */
#define flexidag_openseg_prim_split_2___pvcn_1098__vbase_byte_offset 	212	/* 0x000000d4 */
#define flexidag_openseg_prim_split_2___pvcn_1098__dbase_byte_offset 	236	/* 0x000000ec */
#define flexidag_openseg_prim_split_2___pvcn_1098__dpitchm1_byte_offset 	240	/* 0x000000f0 */
#define flexidag_openseg_prim_split_2___pvcn_1098__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2___pvcn_1098__W 	4
#define flexidag_openseg_prim_split_2___pvcn_1098__H 	1
#define flexidag_openseg_prim_split_2___pvcn_1098__D 	1
#define flexidag_openseg_prim_split_2___pvcn_1098__P 	1
#define flexidag_openseg_prim_split_2___pvcn_1098__data_num_bytes 	2
#define flexidag_openseg_prim_split_2___pvcn_1098__denable 	1
#define flexidag_openseg_prim_split_2___pvcn_1098__dpitch_num_bytes 	32
#define flexidag_openseg_prim_split_2___pvcn_1098__dram_format 	0
#define flexidag_openseg_prim_split_2___pvcn_1098__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_2___pvcn_1098__tile_width 	1
#define flexidag_openseg_prim_split_2___pvcn_1098__tile_height 	1
#define flexidag_openseg_prim_split_2___pvcn_1098__vmem_buffer_num_bytes 	8
#define flexidag_openseg_prim_split_2___pvcn_1098__dram_size_num_bytes 	32

/* flexidag_openseg_prim_split_2 SMB_input __pvcn_1099_ */
#define flexidag_openseg_prim_split_2___pvcn_1099__cnngen_demangled_name 	"__pvcn_1099_"
#define flexidag_openseg_prim_split_2___pvcn_1099__is_constant 	1
#define flexidag_openseg_prim_split_2___pvcn_1099__has_init_data 	1
#define flexidag_openseg_prim_split_2___pvcn_1099__is_variable_scalar 	0
#define flexidag_openseg_prim_split_2___pvcn_1099__is_bitvector 	0
#define flexidag_openseg_prim_split_2___pvcn_1099__is_rlz 	0
#define flexidag_openseg_prim_split_2___pvcn_1099__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_1099__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_1099__byte_offset 	248	/* 0x000000f8 */
#define flexidag_openseg_prim_split_2___pvcn_1099__vbase_byte_offset 	252	/* 0x000000fc */
#define flexidag_openseg_prim_split_2___pvcn_1099__dbase_byte_offset 	276	/* 0x00000114 */
#define flexidag_openseg_prim_split_2___pvcn_1099__dpitchm1_byte_offset 	280	/* 0x00000118 */
#define flexidag_openseg_prim_split_2___pvcn_1099__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2___pvcn_1099__W 	5
#define flexidag_openseg_prim_split_2___pvcn_1099__H 	1
#define flexidag_openseg_prim_split_2___pvcn_1099__D 	1
#define flexidag_openseg_prim_split_2___pvcn_1099__P 	1
#define flexidag_openseg_prim_split_2___pvcn_1099__data_num_bytes 	2
#define flexidag_openseg_prim_split_2___pvcn_1099__denable 	1
#define flexidag_openseg_prim_split_2___pvcn_1099__dpitch_num_bytes 	32
#define flexidag_openseg_prim_split_2___pvcn_1099__dram_format 	0
#define flexidag_openseg_prim_split_2___pvcn_1099__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_2___pvcn_1099__tile_width 	1
#define flexidag_openseg_prim_split_2___pvcn_1099__tile_height 	1
#define flexidag_openseg_prim_split_2___pvcn_1099__vmem_buffer_num_bytes 	12
#define flexidag_openseg_prim_split_2___pvcn_1099__dram_size_num_bytes 	32

/* flexidag_openseg_prim_split_2 SMB_input __pvcn_1116_ */
#define flexidag_openseg_prim_split_2___pvcn_1116__cnngen_demangled_name 	"__pvcn_1116_"
#define flexidag_openseg_prim_split_2___pvcn_1116__is_constant 	1
#define flexidag_openseg_prim_split_2___pvcn_1116__has_init_data 	1
#define flexidag_openseg_prim_split_2___pvcn_1116__is_variable_scalar 	0
#define flexidag_openseg_prim_split_2___pvcn_1116__is_bitvector 	0
#define flexidag_openseg_prim_split_2___pvcn_1116__is_rlz 	0
#define flexidag_openseg_prim_split_2___pvcn_1116__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_1116__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_1116__byte_offset 	336	/* 0x00000150 */
#define flexidag_openseg_prim_split_2___pvcn_1116__vbase_byte_offset 	340	/* 0x00000154 */
#define flexidag_openseg_prim_split_2___pvcn_1116__dbase_byte_offset 	364	/* 0x0000016c */
#define flexidag_openseg_prim_split_2___pvcn_1116__dpitchm1_byte_offset 	368	/* 0x00000170 */
#define flexidag_openseg_prim_split_2___pvcn_1116__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2___pvcn_1116__W 	4
#define flexidag_openseg_prim_split_2___pvcn_1116__H 	1
#define flexidag_openseg_prim_split_2___pvcn_1116__D 	1
#define flexidag_openseg_prim_split_2___pvcn_1116__P 	1
#define flexidag_openseg_prim_split_2___pvcn_1116__data_num_bytes 	4
#define flexidag_openseg_prim_split_2___pvcn_1116__denable 	1
#define flexidag_openseg_prim_split_2___pvcn_1116__dpitch_num_bytes 	32
#define flexidag_openseg_prim_split_2___pvcn_1116__dram_format 	0
#define flexidag_openseg_prim_split_2___pvcn_1116__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_2___pvcn_1116__tile_width 	1
#define flexidag_openseg_prim_split_2___pvcn_1116__tile_height 	1
#define flexidag_openseg_prim_split_2___pvcn_1116__vmem_buffer_num_bytes 	16
#define flexidag_openseg_prim_split_2___pvcn_1116__dram_size_num_bytes 	32

/* flexidag_openseg_prim_split_2 SMB_input __pvcn_1120_ */
#define flexidag_openseg_prim_split_2___pvcn_1120__cnngen_demangled_name 	"__pvcn_1120_"
#define flexidag_openseg_prim_split_2___pvcn_1120__is_constant 	1
#define flexidag_openseg_prim_split_2___pvcn_1120__has_init_data 	1
#define flexidag_openseg_prim_split_2___pvcn_1120__is_variable_scalar 	0
#define flexidag_openseg_prim_split_2___pvcn_1120__is_bitvector 	0
#define flexidag_openseg_prim_split_2___pvcn_1120__is_rlz 	0
#define flexidag_openseg_prim_split_2___pvcn_1120__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_1120__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_1120__byte_offset 	388	/* 0x00000184 */
#define flexidag_openseg_prim_split_2___pvcn_1120__vbase_byte_offset 	392	/* 0x00000188 */
#define flexidag_openseg_prim_split_2___pvcn_1120__dbase_byte_offset 	416	/* 0x000001a0 */
#define flexidag_openseg_prim_split_2___pvcn_1120__dpitchm1_byte_offset 	420	/* 0x000001a4 */
#define flexidag_openseg_prim_split_2___pvcn_1120__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2___pvcn_1120__W 	20804
#define flexidag_openseg_prim_split_2___pvcn_1120__H 	1
#define flexidag_openseg_prim_split_2___pvcn_1120__D 	1
#define flexidag_openseg_prim_split_2___pvcn_1120__P 	1
#define flexidag_openseg_prim_split_2___pvcn_1120__data_num_bytes 	1
#define flexidag_openseg_prim_split_2___pvcn_1120__denable 	1
#define flexidag_openseg_prim_split_2___pvcn_1120__dpitch_num_bytes 	20832
#define flexidag_openseg_prim_split_2___pvcn_1120__dram_format 	0
#define flexidag_openseg_prim_split_2___pvcn_1120__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_2___pvcn_1120__tile_width 	1
#define flexidag_openseg_prim_split_2___pvcn_1120__tile_height 	1
#define flexidag_openseg_prim_split_2___pvcn_1120__vmem_buffer_num_bytes 	20804
#define flexidag_openseg_prim_split_2___pvcn_1120__dram_size_num_bytes 	20832

/* flexidag_openseg_prim_split_2 SMB_input __pvcn_1121_ */
#define flexidag_openseg_prim_split_2___pvcn_1121__cnngen_demangled_name 	"__pvcn_1121_"
#define flexidag_openseg_prim_split_2___pvcn_1121__is_constant 	1
#define flexidag_openseg_prim_split_2___pvcn_1121__has_init_data 	1
#define flexidag_openseg_prim_split_2___pvcn_1121__is_variable_scalar 	0
#define flexidag_openseg_prim_split_2___pvcn_1121__is_bitvector 	0
#define flexidag_openseg_prim_split_2___pvcn_1121__is_rlz 	0
#define flexidag_openseg_prim_split_2___pvcn_1121__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_1121__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_1121__byte_offset 	428	/* 0x000001ac */
#define flexidag_openseg_prim_split_2___pvcn_1121__vbase_byte_offset 	432	/* 0x000001b0 */
#define flexidag_openseg_prim_split_2___pvcn_1121__dbase_byte_offset 	456	/* 0x000001c8 */
#define flexidag_openseg_prim_split_2___pvcn_1121__dpitchm1_byte_offset 	460	/* 0x000001cc */
#define flexidag_openseg_prim_split_2___pvcn_1121__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2___pvcn_1121__W 	1088
#define flexidag_openseg_prim_split_2___pvcn_1121__H 	1
#define flexidag_openseg_prim_split_2___pvcn_1121__D 	1
#define flexidag_openseg_prim_split_2___pvcn_1121__P 	1
#define flexidag_openseg_prim_split_2___pvcn_1121__data_num_bytes 	1
#define flexidag_openseg_prim_split_2___pvcn_1121__denable 	1
#define flexidag_openseg_prim_split_2___pvcn_1121__dpitch_num_bytes 	1088
#define flexidag_openseg_prim_split_2___pvcn_1121__dram_format 	0
#define flexidag_openseg_prim_split_2___pvcn_1121__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_2___pvcn_1121__tile_width 	1
#define flexidag_openseg_prim_split_2___pvcn_1121__tile_height 	1
#define flexidag_openseg_prim_split_2___pvcn_1121__vmem_buffer_num_bytes 	1088
#define flexidag_openseg_prim_split_2___pvcn_1121__dram_size_num_bytes 	1088

/* flexidag_openseg_prim_split_2 SMB_input __pvcn_1124_ */
#define flexidag_openseg_prim_split_2___pvcn_1124__cnngen_demangled_name 	"__pvcn_1124_"
#define flexidag_openseg_prim_split_2___pvcn_1124__is_constant 	1
#define flexidag_openseg_prim_split_2___pvcn_1124__has_init_data 	1
#define flexidag_openseg_prim_split_2___pvcn_1124__is_variable_scalar 	0
#define flexidag_openseg_prim_split_2___pvcn_1124__is_bitvector 	0
#define flexidag_openseg_prim_split_2___pvcn_1124__is_rlz 	0
#define flexidag_openseg_prim_split_2___pvcn_1124__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_1124__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_1124__byte_offset 	468	/* 0x000001d4 */
#define flexidag_openseg_prim_split_2___pvcn_1124__vbase_byte_offset 	472	/* 0x000001d8 */
#define flexidag_openseg_prim_split_2___pvcn_1124__dbase_byte_offset 	496	/* 0x000001f0 */
#define flexidag_openseg_prim_split_2___pvcn_1124__dpitchm1_byte_offset 	500	/* 0x000001f4 */
#define flexidag_openseg_prim_split_2___pvcn_1124__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2___pvcn_1124__W 	5736
#define flexidag_openseg_prim_split_2___pvcn_1124__H 	1
#define flexidag_openseg_prim_split_2___pvcn_1124__D 	1
#define flexidag_openseg_prim_split_2___pvcn_1124__P 	1
#define flexidag_openseg_prim_split_2___pvcn_1124__data_num_bytes 	1
#define flexidag_openseg_prim_split_2___pvcn_1124__denable 	1
#define flexidag_openseg_prim_split_2___pvcn_1124__dpitch_num_bytes 	5760
#define flexidag_openseg_prim_split_2___pvcn_1124__dram_format 	0
#define flexidag_openseg_prim_split_2___pvcn_1124__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_2___pvcn_1124__tile_width 	1
#define flexidag_openseg_prim_split_2___pvcn_1124__tile_height 	1
#define flexidag_openseg_prim_split_2___pvcn_1124__vmem_buffer_num_bytes 	5736
#define flexidag_openseg_prim_split_2___pvcn_1124__dram_size_num_bytes 	5760

/* flexidag_openseg_prim_split_2 SMB_input __pvcn_1125_ */
#define flexidag_openseg_prim_split_2___pvcn_1125__cnngen_demangled_name 	"__pvcn_1125_"
#define flexidag_openseg_prim_split_2___pvcn_1125__is_constant 	1
#define flexidag_openseg_prim_split_2___pvcn_1125__has_init_data 	1
#define flexidag_openseg_prim_split_2___pvcn_1125__is_variable_scalar 	0
#define flexidag_openseg_prim_split_2___pvcn_1125__is_bitvector 	0
#define flexidag_openseg_prim_split_2___pvcn_1125__is_rlz 	0
#define flexidag_openseg_prim_split_2___pvcn_1125__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_1125__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_1125__byte_offset 	508	/* 0x000001fc */
#define flexidag_openseg_prim_split_2___pvcn_1125__vbase_byte_offset 	512	/* 0x00000200 */
#define flexidag_openseg_prim_split_2___pvcn_1125__dbase_byte_offset 	536	/* 0x00000218 */
#define flexidag_openseg_prim_split_2___pvcn_1125__dpitchm1_byte_offset 	540	/* 0x0000021c */
#define flexidag_openseg_prim_split_2___pvcn_1125__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2___pvcn_1125__W 	136
#define flexidag_openseg_prim_split_2___pvcn_1125__H 	1
#define flexidag_openseg_prim_split_2___pvcn_1125__D 	1
#define flexidag_openseg_prim_split_2___pvcn_1125__P 	1
#define flexidag_openseg_prim_split_2___pvcn_1125__data_num_bytes 	1
#define flexidag_openseg_prim_split_2___pvcn_1125__denable 	1
#define flexidag_openseg_prim_split_2___pvcn_1125__dpitch_num_bytes 	160
#define flexidag_openseg_prim_split_2___pvcn_1125__dram_format 	0
#define flexidag_openseg_prim_split_2___pvcn_1125__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_2___pvcn_1125__tile_width 	1
#define flexidag_openseg_prim_split_2___pvcn_1125__tile_height 	1
#define flexidag_openseg_prim_split_2___pvcn_1125__vmem_buffer_num_bytes 	136
#define flexidag_openseg_prim_split_2___pvcn_1125__dram_size_num_bytes 	160

/* flexidag_openseg_prim_split_2 SMB_input __pvcn_1127_ */
#define flexidag_openseg_prim_split_2___pvcn_1127__cnngen_demangled_name 	"__pvcn_1127_"
#define flexidag_openseg_prim_split_2___pvcn_1127__is_constant 	1
#define flexidag_openseg_prim_split_2___pvcn_1127__has_init_data 	1
#define flexidag_openseg_prim_split_2___pvcn_1127__is_variable_scalar 	0
#define flexidag_openseg_prim_split_2___pvcn_1127__is_bitvector 	0
#define flexidag_openseg_prim_split_2___pvcn_1127__is_rlz 	0
#define flexidag_openseg_prim_split_2___pvcn_1127__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_1127__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_1127__byte_offset 	548	/* 0x00000224 */
#define flexidag_openseg_prim_split_2___pvcn_1127__vbase_byte_offset 	552	/* 0x00000228 */
#define flexidag_openseg_prim_split_2___pvcn_1127__dbase_byte_offset 	576	/* 0x00000240 */
#define flexidag_openseg_prim_split_2___pvcn_1127__dpitchm1_byte_offset 	580	/* 0x00000244 */
#define flexidag_openseg_prim_split_2___pvcn_1127__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2___pvcn_1127__W 	1452
#define flexidag_openseg_prim_split_2___pvcn_1127__H 	1
#define flexidag_openseg_prim_split_2___pvcn_1127__D 	1
#define flexidag_openseg_prim_split_2___pvcn_1127__P 	1
#define flexidag_openseg_prim_split_2___pvcn_1127__data_num_bytes 	1
#define flexidag_openseg_prim_split_2___pvcn_1127__denable 	1
#define flexidag_openseg_prim_split_2___pvcn_1127__dpitch_num_bytes 	1472
#define flexidag_openseg_prim_split_2___pvcn_1127__dram_format 	0
#define flexidag_openseg_prim_split_2___pvcn_1127__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_2___pvcn_1127__tile_width 	1
#define flexidag_openseg_prim_split_2___pvcn_1127__tile_height 	1
#define flexidag_openseg_prim_split_2___pvcn_1127__vmem_buffer_num_bytes 	1452
#define flexidag_openseg_prim_split_2___pvcn_1127__dram_size_num_bytes 	1472

/* flexidag_openseg_prim_split_2 SMB_input __pvcn_1128_ */
#define flexidag_openseg_prim_split_2___pvcn_1128__cnngen_demangled_name 	"__pvcn_1128_"
#define flexidag_openseg_prim_split_2___pvcn_1128__is_constant 	1
#define flexidag_openseg_prim_split_2___pvcn_1128__has_init_data 	1
#define flexidag_openseg_prim_split_2___pvcn_1128__is_variable_scalar 	0
#define flexidag_openseg_prim_split_2___pvcn_1128__is_bitvector 	0
#define flexidag_openseg_prim_split_2___pvcn_1128__is_rlz 	0
#define flexidag_openseg_prim_split_2___pvcn_1128__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_1128__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_1128__byte_offset 	588	/* 0x0000024c */
#define flexidag_openseg_prim_split_2___pvcn_1128__vbase_byte_offset 	592	/* 0x00000250 */
#define flexidag_openseg_prim_split_2___pvcn_1128__dbase_byte_offset 	616	/* 0x00000268 */
#define flexidag_openseg_prim_split_2___pvcn_1128__dpitchm1_byte_offset 	620	/* 0x0000026c */
#define flexidag_openseg_prim_split_2___pvcn_1128__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2___pvcn_1128__W 	136
#define flexidag_openseg_prim_split_2___pvcn_1128__H 	1
#define flexidag_openseg_prim_split_2___pvcn_1128__D 	1
#define flexidag_openseg_prim_split_2___pvcn_1128__P 	1
#define flexidag_openseg_prim_split_2___pvcn_1128__data_num_bytes 	1
#define flexidag_openseg_prim_split_2___pvcn_1128__denable 	1
#define flexidag_openseg_prim_split_2___pvcn_1128__dpitch_num_bytes 	160
#define flexidag_openseg_prim_split_2___pvcn_1128__dram_format 	0
#define flexidag_openseg_prim_split_2___pvcn_1128__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_2___pvcn_1128__tile_width 	1
#define flexidag_openseg_prim_split_2___pvcn_1128__tile_height 	1
#define flexidag_openseg_prim_split_2___pvcn_1128__vmem_buffer_num_bytes 	136
#define flexidag_openseg_prim_split_2___pvcn_1128__dram_size_num_bytes 	160

/* flexidag_openseg_prim_split_2 SMB_input __pvcn_1131_ */
#define flexidag_openseg_prim_split_2___pvcn_1131__cnngen_demangled_name 	"__pvcn_1131_"
#define flexidag_openseg_prim_split_2___pvcn_1131__is_constant 	1
#define flexidag_openseg_prim_split_2___pvcn_1131__has_init_data 	1
#define flexidag_openseg_prim_split_2___pvcn_1131__is_variable_scalar 	0
#define flexidag_openseg_prim_split_2___pvcn_1131__is_bitvector 	0
#define flexidag_openseg_prim_split_2___pvcn_1131__is_rlz 	0
#define flexidag_openseg_prim_split_2___pvcn_1131__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_1131__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_1131__byte_offset 	628	/* 0x00000274 */
#define flexidag_openseg_prim_split_2___pvcn_1131__vbase_byte_offset 	632	/* 0x00000278 */
#define flexidag_openseg_prim_split_2___pvcn_1131__dbase_byte_offset 	656	/* 0x00000290 */
#define flexidag_openseg_prim_split_2___pvcn_1131__dpitchm1_byte_offset 	660	/* 0x00000294 */
#define flexidag_openseg_prim_split_2___pvcn_1131__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2___pvcn_1131__W 	5780
#define flexidag_openseg_prim_split_2___pvcn_1131__H 	1
#define flexidag_openseg_prim_split_2___pvcn_1131__D 	1
#define flexidag_openseg_prim_split_2___pvcn_1131__P 	1
#define flexidag_openseg_prim_split_2___pvcn_1131__data_num_bytes 	1
#define flexidag_openseg_prim_split_2___pvcn_1131__denable 	1
#define flexidag_openseg_prim_split_2___pvcn_1131__dpitch_num_bytes 	5792
#define flexidag_openseg_prim_split_2___pvcn_1131__dram_format 	0
#define flexidag_openseg_prim_split_2___pvcn_1131__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_2___pvcn_1131__tile_width 	1
#define flexidag_openseg_prim_split_2___pvcn_1131__tile_height 	1
#define flexidag_openseg_prim_split_2___pvcn_1131__vmem_buffer_num_bytes 	5780
#define flexidag_openseg_prim_split_2___pvcn_1131__dram_size_num_bytes 	5792

/* flexidag_openseg_prim_split_2 SMB_input __pvcn_1132_ */
#define flexidag_openseg_prim_split_2___pvcn_1132__cnngen_demangled_name 	"__pvcn_1132_"
#define flexidag_openseg_prim_split_2___pvcn_1132__is_constant 	1
#define flexidag_openseg_prim_split_2___pvcn_1132__has_init_data 	1
#define flexidag_openseg_prim_split_2___pvcn_1132__is_variable_scalar 	0
#define flexidag_openseg_prim_split_2___pvcn_1132__is_bitvector 	0
#define flexidag_openseg_prim_split_2___pvcn_1132__is_rlz 	0
#define flexidag_openseg_prim_split_2___pvcn_1132__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_1132__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_1132__byte_offset 	668	/* 0x0000029c */
#define flexidag_openseg_prim_split_2___pvcn_1132__vbase_byte_offset 	672	/* 0x000002a0 */
#define flexidag_openseg_prim_split_2___pvcn_1132__dbase_byte_offset 	696	/* 0x000002b8 */
#define flexidag_openseg_prim_split_2___pvcn_1132__dpitchm1_byte_offset 	700	/* 0x000002bc */
#define flexidag_openseg_prim_split_2___pvcn_1132__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2___pvcn_1132__W 	144
#define flexidag_openseg_prim_split_2___pvcn_1132__H 	1
#define flexidag_openseg_prim_split_2___pvcn_1132__D 	1
#define flexidag_openseg_prim_split_2___pvcn_1132__P 	1
#define flexidag_openseg_prim_split_2___pvcn_1132__data_num_bytes 	1
#define flexidag_openseg_prim_split_2___pvcn_1132__denable 	1
#define flexidag_openseg_prim_split_2___pvcn_1132__dpitch_num_bytes 	160
#define flexidag_openseg_prim_split_2___pvcn_1132__dram_format 	0
#define flexidag_openseg_prim_split_2___pvcn_1132__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_2___pvcn_1132__tile_width 	1
#define flexidag_openseg_prim_split_2___pvcn_1132__tile_height 	1
#define flexidag_openseg_prim_split_2___pvcn_1132__vmem_buffer_num_bytes 	144
#define flexidag_openseg_prim_split_2___pvcn_1132__dram_size_num_bytes 	160

/* flexidag_openseg_prim_split_2 SMB_input __pvcn_1138_ */
#define flexidag_openseg_prim_split_2___pvcn_1138__cnngen_demangled_name 	"__pvcn_1138_"
#define flexidag_openseg_prim_split_2___pvcn_1138__is_constant 	1
#define flexidag_openseg_prim_split_2___pvcn_1138__has_init_data 	1
#define flexidag_openseg_prim_split_2___pvcn_1138__is_variable_scalar 	0
#define flexidag_openseg_prim_split_2___pvcn_1138__is_bitvector 	0
#define flexidag_openseg_prim_split_2___pvcn_1138__is_rlz 	0
#define flexidag_openseg_prim_split_2___pvcn_1138__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_1138__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_1138__byte_offset 	708	/* 0x000002c4 */
#define flexidag_openseg_prim_split_2___pvcn_1138__vbase_byte_offset 	712	/* 0x000002c8 */
#define flexidag_openseg_prim_split_2___pvcn_1138__dbase_byte_offset 	736	/* 0x000002e0 */
#define flexidag_openseg_prim_split_2___pvcn_1138__dpitchm1_byte_offset 	740	/* 0x000002e4 */
#define flexidag_openseg_prim_split_2___pvcn_1138__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2___pvcn_1138__W 	23076
#define flexidag_openseg_prim_split_2___pvcn_1138__H 	1
#define flexidag_openseg_prim_split_2___pvcn_1138__D 	1
#define flexidag_openseg_prim_split_2___pvcn_1138__P 	1
#define flexidag_openseg_prim_split_2___pvcn_1138__data_num_bytes 	1
#define flexidag_openseg_prim_split_2___pvcn_1138__denable 	1
#define flexidag_openseg_prim_split_2___pvcn_1138__dpitch_num_bytes 	23104
#define flexidag_openseg_prim_split_2___pvcn_1138__dram_format 	0
#define flexidag_openseg_prim_split_2___pvcn_1138__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_2___pvcn_1138__tile_width 	1
#define flexidag_openseg_prim_split_2___pvcn_1138__tile_height 	1
#define flexidag_openseg_prim_split_2___pvcn_1138__vmem_buffer_num_bytes 	23076
#define flexidag_openseg_prim_split_2___pvcn_1138__dram_size_num_bytes 	23104

/* flexidag_openseg_prim_split_2 SMB_input __pvcn_1139_ */
#define flexidag_openseg_prim_split_2___pvcn_1139__cnngen_demangled_name 	"__pvcn_1139_"
#define flexidag_openseg_prim_split_2___pvcn_1139__is_constant 	1
#define flexidag_openseg_prim_split_2___pvcn_1139__has_init_data 	1
#define flexidag_openseg_prim_split_2___pvcn_1139__is_variable_scalar 	0
#define flexidag_openseg_prim_split_2___pvcn_1139__is_bitvector 	0
#define flexidag_openseg_prim_split_2___pvcn_1139__is_rlz 	0
#define flexidag_openseg_prim_split_2___pvcn_1139__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_1139__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_1139__byte_offset 	748	/* 0x000002ec */
#define flexidag_openseg_prim_split_2___pvcn_1139__vbase_byte_offset 	752	/* 0x000002f0 */
#define flexidag_openseg_prim_split_2___pvcn_1139__dbase_byte_offset 	776	/* 0x00000308 */
#define flexidag_openseg_prim_split_2___pvcn_1139__dpitchm1_byte_offset 	780	/* 0x0000030c */
#define flexidag_openseg_prim_split_2___pvcn_1139__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2___pvcn_1139__W 	1088
#define flexidag_openseg_prim_split_2___pvcn_1139__H 	1
#define flexidag_openseg_prim_split_2___pvcn_1139__D 	1
#define flexidag_openseg_prim_split_2___pvcn_1139__P 	1
#define flexidag_openseg_prim_split_2___pvcn_1139__data_num_bytes 	1
#define flexidag_openseg_prim_split_2___pvcn_1139__denable 	1
#define flexidag_openseg_prim_split_2___pvcn_1139__dpitch_num_bytes 	1088
#define flexidag_openseg_prim_split_2___pvcn_1139__dram_format 	0
#define flexidag_openseg_prim_split_2___pvcn_1139__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_2___pvcn_1139__tile_width 	1
#define flexidag_openseg_prim_split_2___pvcn_1139__tile_height 	1
#define flexidag_openseg_prim_split_2___pvcn_1139__vmem_buffer_num_bytes 	1088
#define flexidag_openseg_prim_split_2___pvcn_1139__dram_size_num_bytes 	1088

/* flexidag_openseg_prim_split_2 SMB_input __pvcn_1142_ */
#define flexidag_openseg_prim_split_2___pvcn_1142__cnngen_demangled_name 	"__pvcn_1142_"
#define flexidag_openseg_prim_split_2___pvcn_1142__is_constant 	1
#define flexidag_openseg_prim_split_2___pvcn_1142__has_init_data 	1
#define flexidag_openseg_prim_split_2___pvcn_1142__is_variable_scalar 	0
#define flexidag_openseg_prim_split_2___pvcn_1142__is_bitvector 	0
#define flexidag_openseg_prim_split_2___pvcn_1142__is_rlz 	0
#define flexidag_openseg_prim_split_2___pvcn_1142__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_1142__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_1142__byte_offset 	788	/* 0x00000314 */
#define flexidag_openseg_prim_split_2___pvcn_1142__vbase_byte_offset 	792	/* 0x00000318 */
#define flexidag_openseg_prim_split_2___pvcn_1142__dbase_byte_offset 	816	/* 0x00000330 */
#define flexidag_openseg_prim_split_2___pvcn_1142__dpitchm1_byte_offset 	820	/* 0x00000334 */
#define flexidag_openseg_prim_split_2___pvcn_1142__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2___pvcn_1142__W 	5740
#define flexidag_openseg_prim_split_2___pvcn_1142__H 	1
#define flexidag_openseg_prim_split_2___pvcn_1142__D 	1
#define flexidag_openseg_prim_split_2___pvcn_1142__P 	1
#define flexidag_openseg_prim_split_2___pvcn_1142__data_num_bytes 	1
#define flexidag_openseg_prim_split_2___pvcn_1142__denable 	1
#define flexidag_openseg_prim_split_2___pvcn_1142__dpitch_num_bytes 	5760
#define flexidag_openseg_prim_split_2___pvcn_1142__dram_format 	0
#define flexidag_openseg_prim_split_2___pvcn_1142__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_2___pvcn_1142__tile_width 	1
#define flexidag_openseg_prim_split_2___pvcn_1142__tile_height 	1
#define flexidag_openseg_prim_split_2___pvcn_1142__vmem_buffer_num_bytes 	5740
#define flexidag_openseg_prim_split_2___pvcn_1142__dram_size_num_bytes 	5760

/* flexidag_openseg_prim_split_2 SMB_input __pvcn_1143_ */
#define flexidag_openseg_prim_split_2___pvcn_1143__cnngen_demangled_name 	"__pvcn_1143_"
#define flexidag_openseg_prim_split_2___pvcn_1143__is_constant 	1
#define flexidag_openseg_prim_split_2___pvcn_1143__has_init_data 	1
#define flexidag_openseg_prim_split_2___pvcn_1143__is_variable_scalar 	0
#define flexidag_openseg_prim_split_2___pvcn_1143__is_bitvector 	0
#define flexidag_openseg_prim_split_2___pvcn_1143__is_rlz 	0
#define flexidag_openseg_prim_split_2___pvcn_1143__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_1143__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_1143__byte_offset 	828	/* 0x0000033c */
#define flexidag_openseg_prim_split_2___pvcn_1143__vbase_byte_offset 	832	/* 0x00000340 */
#define flexidag_openseg_prim_split_2___pvcn_1143__dbase_byte_offset 	856	/* 0x00000358 */
#define flexidag_openseg_prim_split_2___pvcn_1143__dpitchm1_byte_offset 	860	/* 0x0000035c */
#define flexidag_openseg_prim_split_2___pvcn_1143__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2___pvcn_1143__W 	144
#define flexidag_openseg_prim_split_2___pvcn_1143__H 	1
#define flexidag_openseg_prim_split_2___pvcn_1143__D 	1
#define flexidag_openseg_prim_split_2___pvcn_1143__P 	1
#define flexidag_openseg_prim_split_2___pvcn_1143__data_num_bytes 	1
#define flexidag_openseg_prim_split_2___pvcn_1143__denable 	1
#define flexidag_openseg_prim_split_2___pvcn_1143__dpitch_num_bytes 	160
#define flexidag_openseg_prim_split_2___pvcn_1143__dram_format 	0
#define flexidag_openseg_prim_split_2___pvcn_1143__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_2___pvcn_1143__tile_width 	1
#define flexidag_openseg_prim_split_2___pvcn_1143__tile_height 	1
#define flexidag_openseg_prim_split_2___pvcn_1143__vmem_buffer_num_bytes 	144
#define flexidag_openseg_prim_split_2___pvcn_1143__dram_size_num_bytes 	160

/* flexidag_openseg_prim_split_2 SMB_input __pvcn_1145_ */
#define flexidag_openseg_prim_split_2___pvcn_1145__cnngen_demangled_name 	"__pvcn_1145_"
#define flexidag_openseg_prim_split_2___pvcn_1145__is_constant 	1
#define flexidag_openseg_prim_split_2___pvcn_1145__has_init_data 	1
#define flexidag_openseg_prim_split_2___pvcn_1145__is_variable_scalar 	0
#define flexidag_openseg_prim_split_2___pvcn_1145__is_bitvector 	0
#define flexidag_openseg_prim_split_2___pvcn_1145__is_rlz 	0
#define flexidag_openseg_prim_split_2___pvcn_1145__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_1145__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_1145__byte_offset 	868	/* 0x00000364 */
#define flexidag_openseg_prim_split_2___pvcn_1145__vbase_byte_offset 	872	/* 0x00000368 */
#define flexidag_openseg_prim_split_2___pvcn_1145__dbase_byte_offset 	896	/* 0x00000380 */
#define flexidag_openseg_prim_split_2___pvcn_1145__dpitchm1_byte_offset 	900	/* 0x00000384 */
#define flexidag_openseg_prim_split_2___pvcn_1145__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2___pvcn_1145__W 	1408
#define flexidag_openseg_prim_split_2___pvcn_1145__H 	1
#define flexidag_openseg_prim_split_2___pvcn_1145__D 	1
#define flexidag_openseg_prim_split_2___pvcn_1145__P 	1
#define flexidag_openseg_prim_split_2___pvcn_1145__data_num_bytes 	1
#define flexidag_openseg_prim_split_2___pvcn_1145__denable 	1
#define flexidag_openseg_prim_split_2___pvcn_1145__dpitch_num_bytes 	1408
#define flexidag_openseg_prim_split_2___pvcn_1145__dram_format 	0
#define flexidag_openseg_prim_split_2___pvcn_1145__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_2___pvcn_1145__tile_width 	1
#define flexidag_openseg_prim_split_2___pvcn_1145__tile_height 	1
#define flexidag_openseg_prim_split_2___pvcn_1145__vmem_buffer_num_bytes 	1408
#define flexidag_openseg_prim_split_2___pvcn_1145__dram_size_num_bytes 	1408

/* flexidag_openseg_prim_split_2 SMB_input __pvcn_1146_ */
#define flexidag_openseg_prim_split_2___pvcn_1146__cnngen_demangled_name 	"__pvcn_1146_"
#define flexidag_openseg_prim_split_2___pvcn_1146__is_constant 	1
#define flexidag_openseg_prim_split_2___pvcn_1146__has_init_data 	1
#define flexidag_openseg_prim_split_2___pvcn_1146__is_variable_scalar 	0
#define flexidag_openseg_prim_split_2___pvcn_1146__is_bitvector 	0
#define flexidag_openseg_prim_split_2___pvcn_1146__is_rlz 	0
#define flexidag_openseg_prim_split_2___pvcn_1146__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_1146__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_1146__byte_offset 	908	/* 0x0000038c */
#define flexidag_openseg_prim_split_2___pvcn_1146__vbase_byte_offset 	912	/* 0x00000390 */
#define flexidag_openseg_prim_split_2___pvcn_1146__dbase_byte_offset 	936	/* 0x000003a8 */
#define flexidag_openseg_prim_split_2___pvcn_1146__dpitchm1_byte_offset 	940	/* 0x000003ac */
#define flexidag_openseg_prim_split_2___pvcn_1146__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2___pvcn_1146__W 	136
#define flexidag_openseg_prim_split_2___pvcn_1146__H 	1
#define flexidag_openseg_prim_split_2___pvcn_1146__D 	1
#define flexidag_openseg_prim_split_2___pvcn_1146__P 	1
#define flexidag_openseg_prim_split_2___pvcn_1146__data_num_bytes 	1
#define flexidag_openseg_prim_split_2___pvcn_1146__denable 	1
#define flexidag_openseg_prim_split_2___pvcn_1146__dpitch_num_bytes 	160
#define flexidag_openseg_prim_split_2___pvcn_1146__dram_format 	0
#define flexidag_openseg_prim_split_2___pvcn_1146__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_2___pvcn_1146__tile_width 	1
#define flexidag_openseg_prim_split_2___pvcn_1146__tile_height 	1
#define flexidag_openseg_prim_split_2___pvcn_1146__vmem_buffer_num_bytes 	136
#define flexidag_openseg_prim_split_2___pvcn_1146__dram_size_num_bytes 	160

/* flexidag_openseg_prim_split_2 SMB_input __pvcn_1149_ */
#define flexidag_openseg_prim_split_2___pvcn_1149__cnngen_demangled_name 	"__pvcn_1149_"
#define flexidag_openseg_prim_split_2___pvcn_1149__is_constant 	1
#define flexidag_openseg_prim_split_2___pvcn_1149__has_init_data 	1
#define flexidag_openseg_prim_split_2___pvcn_1149__is_variable_scalar 	0
#define flexidag_openseg_prim_split_2___pvcn_1149__is_bitvector 	0
#define flexidag_openseg_prim_split_2___pvcn_1149__is_rlz 	0
#define flexidag_openseg_prim_split_2___pvcn_1149__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_1149__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_1149__byte_offset 	948	/* 0x000003b4 */
#define flexidag_openseg_prim_split_2___pvcn_1149__vbase_byte_offset 	952	/* 0x000003b8 */
#define flexidag_openseg_prim_split_2___pvcn_1149__dbase_byte_offset 	976	/* 0x000003d0 */
#define flexidag_openseg_prim_split_2___pvcn_1149__dpitchm1_byte_offset 	980	/* 0x000003d4 */
#define flexidag_openseg_prim_split_2___pvcn_1149__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2___pvcn_1149__W 	5752
#define flexidag_openseg_prim_split_2___pvcn_1149__H 	1
#define flexidag_openseg_prim_split_2___pvcn_1149__D 	1
#define flexidag_openseg_prim_split_2___pvcn_1149__P 	1
#define flexidag_openseg_prim_split_2___pvcn_1149__data_num_bytes 	1
#define flexidag_openseg_prim_split_2___pvcn_1149__denable 	1
#define flexidag_openseg_prim_split_2___pvcn_1149__dpitch_num_bytes 	5760
#define flexidag_openseg_prim_split_2___pvcn_1149__dram_format 	0
#define flexidag_openseg_prim_split_2___pvcn_1149__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_2___pvcn_1149__tile_width 	1
#define flexidag_openseg_prim_split_2___pvcn_1149__tile_height 	1
#define flexidag_openseg_prim_split_2___pvcn_1149__vmem_buffer_num_bytes 	5752
#define flexidag_openseg_prim_split_2___pvcn_1149__dram_size_num_bytes 	5760

/* flexidag_openseg_prim_split_2 SMB_input __pvcn_1150_ */
#define flexidag_openseg_prim_split_2___pvcn_1150__cnngen_demangled_name 	"__pvcn_1150_"
#define flexidag_openseg_prim_split_2___pvcn_1150__is_constant 	1
#define flexidag_openseg_prim_split_2___pvcn_1150__has_init_data 	1
#define flexidag_openseg_prim_split_2___pvcn_1150__is_variable_scalar 	0
#define flexidag_openseg_prim_split_2___pvcn_1150__is_bitvector 	0
#define flexidag_openseg_prim_split_2___pvcn_1150__is_rlz 	0
#define flexidag_openseg_prim_split_2___pvcn_1150__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_1150__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_1150__byte_offset 	988	/* 0x000003dc */
#define flexidag_openseg_prim_split_2___pvcn_1150__vbase_byte_offset 	992	/* 0x000003e0 */
#define flexidag_openseg_prim_split_2___pvcn_1150__dbase_byte_offset 	1016	/* 0x000003f8 */
#define flexidag_openseg_prim_split_2___pvcn_1150__dpitchm1_byte_offset 	1020	/* 0x000003fc */
#define flexidag_openseg_prim_split_2___pvcn_1150__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2___pvcn_1150__W 	136
#define flexidag_openseg_prim_split_2___pvcn_1150__H 	1
#define flexidag_openseg_prim_split_2___pvcn_1150__D 	1
#define flexidag_openseg_prim_split_2___pvcn_1150__P 	1
#define flexidag_openseg_prim_split_2___pvcn_1150__data_num_bytes 	1
#define flexidag_openseg_prim_split_2___pvcn_1150__denable 	1
#define flexidag_openseg_prim_split_2___pvcn_1150__dpitch_num_bytes 	160
#define flexidag_openseg_prim_split_2___pvcn_1150__dram_format 	0
#define flexidag_openseg_prim_split_2___pvcn_1150__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_2___pvcn_1150__tile_width 	1
#define flexidag_openseg_prim_split_2___pvcn_1150__tile_height 	1
#define flexidag_openseg_prim_split_2___pvcn_1150__vmem_buffer_num_bytes 	136
#define flexidag_openseg_prim_split_2___pvcn_1150__dram_size_num_bytes 	160

/* flexidag_openseg_prim_split_2 VCB __pvcn_674_ */
#define flexidag_openseg_prim_split_2___pvcn_674__cnngen_demangled_name 	"__pvcn_674_"
#define flexidag_openseg_prim_split_2___pvcn_674__is_bitvector 	0
#define flexidag_openseg_prim_split_2___pvcn_674__is_rlz 	0
#define flexidag_openseg_prim_split_2___pvcn_674__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_674__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_674__byte_offset 	1080	/* 0x00000438 */
#define flexidag_openseg_prim_split_2___pvcn_674__vbase_byte_offset 	1084	/* 0x0000043c */
#define flexidag_openseg_prim_split_2___pvcn_674__vmem_buffer_num_bytes 	256

/* flexidag_openseg_prim_split_2 VCB Conv_3__Conv2D___dotib___388 */
#define flexidag_openseg_prim_split_2_Conv_3__Conv2D___dotib___388_cnngen_demangled_name 	"Conv_3__Conv2D___dotib___388"
#define flexidag_openseg_prim_split_2_Conv_3__Conv2D___dotib___388_is_bitvector 	0
#define flexidag_openseg_prim_split_2_Conv_3__Conv2D___dotib___388_is_rlz 	0
#define flexidag_openseg_prim_split_2_Conv_3__Conv2D___dotib___388_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2_Conv_3__Conv2D___dotib___388_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_2_Conv_3__Conv2D___dotib___388_byte_offset 	1108	/* 0x00000454 */
#define flexidag_openseg_prim_split_2_Conv_3__Conv2D___dotib___388_vbase_byte_offset 	1112	/* 0x00000458 */
#define flexidag_openseg_prim_split_2_Conv_3__Conv2D___dotib___388_vmem_buffer_num_bytes 	260

/* flexidag_openseg_prim_split_2 VCB __pvcn_1108_ */
#define flexidag_openseg_prim_split_2___pvcn_1108__cnngen_demangled_name 	"__pvcn_1108_"
#define flexidag_openseg_prim_split_2___pvcn_1108__is_bitvector 	0
#define flexidag_openseg_prim_split_2___pvcn_1108__is_rlz 	0
#define flexidag_openseg_prim_split_2___pvcn_1108__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_1108__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_1108__byte_offset 	1384	/* 0x00000568 */
#define flexidag_openseg_prim_split_2___pvcn_1108__vbase_byte_offset 	1392	/* 0x00000570 */
#define flexidag_openseg_prim_split_2___pvcn_1108__vmem_buffer_num_bytes 	1088

/* flexidag_openseg_prim_split_2 VCB Sigmoid___mul___76 */
#define flexidag_openseg_prim_split_2_Sigmoid___mul___76_cnngen_demangled_name 	"Sigmoid___mul___76"
#define flexidag_openseg_prim_split_2_Sigmoid___mul___76_is_bitvector 	0
#define flexidag_openseg_prim_split_2_Sigmoid___mul___76_is_rlz 	0
#define flexidag_openseg_prim_split_2_Sigmoid___mul___76_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2_Sigmoid___mul___76_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_2_Sigmoid___mul___76_byte_offset 	1540	/* 0x00000604 */
#define flexidag_openseg_prim_split_2_Sigmoid___mul___76_vbase_byte_offset 	1544	/* 0x00000608 */
#define flexidag_openseg_prim_split_2_Sigmoid___mul___76_vmem_buffer_num_bytes 	1024

/* flexidag_openseg_prim_split_2 VCB Sigmoid */
#define flexidag_openseg_prim_split_2_Sigmoid_cnngen_demangled_name 	"Sigmoid"
#define flexidag_openseg_prim_split_2_Sigmoid_is_bitvector 	0
#define flexidag_openseg_prim_split_2_Sigmoid_is_rlz 	0
#define flexidag_openseg_prim_split_2_Sigmoid_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2_Sigmoid_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_2_Sigmoid_byte_offset 	1620	/* 0x00000654 */
#define flexidag_openseg_prim_split_2_Sigmoid_vbase_byte_offset 	1624	/* 0x00000658 */
#define flexidag_openseg_prim_split_2_Sigmoid_vmem_buffer_num_bytes 	5120

/* flexidag_openseg_prim_split_2 VCB Mul */
#define flexidag_openseg_prim_split_2_Mul_cnngen_demangled_name 	"Mul"
#define flexidag_openseg_prim_split_2_Mul_is_bitvector 	0
#define flexidag_openseg_prim_split_2_Mul_is_rlz 	0
#define flexidag_openseg_prim_split_2_Mul_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2_Mul_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_2_Mul_byte_offset 	1660	/* 0x0000067c */
#define flexidag_openseg_prim_split_2_Mul_vbase_byte_offset 	1668	/* 0x00000684 */
#define flexidag_openseg_prim_split_2_Mul_vmem_buffer_num_bytes 	167948

/* flexidag_openseg_prim_split_2 HMB_output ResizeBilinear */
#define flexidag_openseg_prim_split_2_ResizeBilinear_cnngen_demangled_name 	"ResizeBilinear"
#define flexidag_openseg_prim_split_2_ResizeBilinear_is_main_input_output 	0
#define flexidag_openseg_prim_split_2_ResizeBilinear_is_bitvector 	0
#define flexidag_openseg_prim_split_2_ResizeBilinear_is_rlz 	0
#define flexidag_openseg_prim_split_2_ResizeBilinear_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2_ResizeBilinear_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_2_ResizeBilinear_byte_offset 	1704	/* 0x000006a8 */
#define flexidag_openseg_prim_split_2_ResizeBilinear_vbase_byte_offset 	1708	/* 0x000006ac */
#define flexidag_openseg_prim_split_2_ResizeBilinear_dbase_byte_offset 	1732	/* 0x000006c4 */
#define flexidag_openseg_prim_split_2_ResizeBilinear_dpitchm1_byte_offset 	1736	/* 0x000006c8 */
#define flexidag_openseg_prim_split_2_ResizeBilinear_dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2_ResizeBilinear_vwidth_minus_one_byte_offset 	1720	/* 0x000006b8 */
#define flexidag_openseg_prim_split_2_ResizeBilinear_vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2_ResizeBilinear_vheight_minus_one_byte_offset 	1722	/* 0x000006ba */
#define flexidag_openseg_prim_split_2_ResizeBilinear_vheight_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2_ResizeBilinear_drotate_bit_offset 	13830	/* 0x00003606 */
#define flexidag_openseg_prim_split_2_ResizeBilinear_hflip_bit_offset 	13831	/* 0x00003607 */
#define flexidag_openseg_prim_split_2_ResizeBilinear_vflip_bit_offset 	13832	/* 0x00003608 */
#define flexidag_openseg_prim_split_2_ResizeBilinear_dflip_bit_offset 	13833	/* 0x00003609 */
#define flexidag_openseg_prim_split_2_ResizeBilinear_pflip_bit_offset 	13834	/* 0x0000360a */
#define flexidag_openseg_prim_split_2_ResizeBilinear_W 	160
#define flexidag_openseg_prim_split_2_ResizeBilinear_H 	64
#define flexidag_openseg_prim_split_2_ResizeBilinear_D 	256
#define flexidag_openseg_prim_split_2_ResizeBilinear_P 	1
#define flexidag_openseg_prim_split_2_ResizeBilinear_data_num_bytes 	2
#define flexidag_openseg_prim_split_2_ResizeBilinear_denable 	1
#define flexidag_openseg_prim_split_2_ResizeBilinear_dpitch_num_bytes 	32
#define flexidag_openseg_prim_split_2_ResizeBilinear_dram_format 	3
#define flexidag_openseg_prim_split_2_ResizeBilinear_vp_interleave_mode 	2
#define flexidag_openseg_prim_split_2_ResizeBilinear_tile_width 	4
#define flexidag_openseg_prim_split_2_ResizeBilinear_tile_height 	2
#define flexidag_openseg_prim_split_2_ResizeBilinear_vmem_buffer_num_bytes 	344064
#define flexidag_openseg_prim_split_2_ResizeBilinear_dram_size_num_bytes 	5242880
#define flexidag_openseg_prim_split_2_ResizeBilinear_sign 	0
#define flexidag_openseg_prim_split_2_ResizeBilinear_datasize 	1
#define flexidag_openseg_prim_split_2_ResizeBilinear_expoffset 	13
#define flexidag_openseg_prim_split_2_ResizeBilinear_expbits 	0
#define flexidag_openseg_prim_split_2_ResizeBilinear_drotate 	0
#define flexidag_openseg_prim_split_2_ResizeBilinear_hflip 	0
#define flexidag_openseg_prim_split_2_ResizeBilinear_vflip 	0
#define flexidag_openseg_prim_split_2_ResizeBilinear_dflip 	0
#define flexidag_openseg_prim_split_2_ResizeBilinear_pflip 	0

/* flexidag_openseg_prim_split_2 VCB PeleeNet__stage_3_dense_block_0__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_0__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm"
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_is_bitvector 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_is_rlz 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_byte_offset 	1796	/* 0x00000704 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_vbase_byte_offset 	1804	/* 0x0000070c */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_vmem_buffer_num_bytes 	83968

/* flexidag_openseg_prim_split_2 VCB PeleeNet__stage_3_dense_block_0__conv_right_0__Relu */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__conv_right_0__Relu_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_0__conv_right_0__Relu"
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__conv_right_0__Relu_is_bitvector 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__conv_right_0__Relu_is_rlz 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__conv_right_0__Relu_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__conv_right_0__Relu_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__conv_right_0__Relu_byte_offset 	1848	/* 0x00000738 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__conv_right_0__Relu_vbase_byte_offset 	1856	/* 0x00000740 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__conv_right_0__Relu_vmem_buffer_num_bytes 	56320

/* flexidag_openseg_prim_split_2 VCB PeleeNet__stage_3_dense_block_0__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_0__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu"
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_bitvector 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_rlz 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_byte_offset 	1896	/* 0x00000768 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vbase_byte_offset 	1904	/* 0x00000770 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vmem_buffer_num_bytes 	28288

/* flexidag_openseg_prim_split_2 VCB PeleeNet__stage_3_dense_block_0__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_0__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu"
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_bitvector 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_rlz 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_byte_offset 	1944	/* 0x00000798 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_vbase_byte_offset 	1952	/* 0x000007a0 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_vmem_buffer_num_bytes 	7680

/* flexidag_openseg_prim_split_2 VCB PeleeNet__stage_3_dense_block_0__conv_left_0__Relu */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__conv_left_0__Relu_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_0__conv_left_0__Relu"
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__conv_left_0__Relu_is_bitvector 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__conv_left_0__Relu_is_rlz 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__conv_left_0__Relu_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__conv_left_0__Relu_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__conv_left_0__Relu_byte_offset 	1996	/* 0x000007cc */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__conv_left_0__Relu_vbase_byte_offset 	2004	/* 0x000007d4 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__conv_left_0__Relu_vmem_buffer_num_bytes 	43264

/* flexidag_openseg_prim_split_2 VCB PeleeNet__stage_3_dense_block_0__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_0__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu"
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_bitvector 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_rlz 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_byte_offset 	2044	/* 0x000007fc */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vbase_byte_offset 	2052	/* 0x00000804 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vmem_buffer_num_bytes 	12800

/* flexidag_openseg_prim_split_2 VCB PeleeNet__stage_3_dense_block_0__concat */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__concat_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_0__concat"
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__concat_is_bitvector 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__concat_is_rlz 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__concat_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__concat_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__concat_byte_offset 	2124	/* 0x0000084c */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__concat_vbase_byte_offset 	2132	/* 0x00000854 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__concat_vmem_buffer_num_bytes 	207360

/* flexidag_openseg_prim_split_2 HMB_output PeleeNet__stage_3_dense_block_1__concat */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__concat_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_1__concat"
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__concat_is_main_input_output 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__concat_is_bitvector 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__concat_is_rlz 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__concat_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__concat_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__concat_byte_offset 	2160	/* 0x00000870 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__concat_vbase_byte_offset 	2164	/* 0x00000874 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__concat_dbase_byte_offset 	2188	/* 0x0000088c */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__concat_dpitchm1_byte_offset 	2192	/* 0x00000890 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__concat_dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__concat_vwidth_minus_one_byte_offset 	2176	/* 0x00000880 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__concat_vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__concat_vheight_minus_one_byte_offset 	2178	/* 0x00000882 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__concat_vheight_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__concat_drotate_bit_offset 	17478	/* 0x00004446 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__concat_hflip_bit_offset 	17479	/* 0x00004447 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__concat_vflip_bit_offset 	17480	/* 0x00004448 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__concat_dflip_bit_offset 	17481	/* 0x00004449 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__concat_pflip_bit_offset 	17482	/* 0x0000444a */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__concat_W 	80
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__concat_H 	32
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__concat_D 	320
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__concat_P 	1
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__concat_data_num_bytes 	1
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__concat_denable 	1
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__concat_dpitch_num_bytes 	32
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__concat_dram_format 	3
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__concat_vp_interleave_mode 	2
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__concat_tile_width 	4
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__concat_tile_height 	1
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__concat_vmem_buffer_num_bytes 	112640
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__concat_dram_size_num_bytes 	819200
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__concat_sign 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__concat_datasize 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__concat_expoffset 	5
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__concat_expbits 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__concat_drotate 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__concat_hflip 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__concat_vflip 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__concat_dflip 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__concat_pflip 	0

/* flexidag_openseg_prim_split_2 VCB PeleeNet__stage_3_dense_block_1__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_1__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm"
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_is_bitvector 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_is_rlz 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_byte_offset 	2232	/* 0x000008b8 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_vbase_byte_offset 	2240	/* 0x000008c0 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_vmem_buffer_num_bytes 	124928

/* flexidag_openseg_prim_split_2 VCB PeleeNet__stage_3_dense_block_1__conv_right_0__Relu */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__conv_right_0__Relu_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_1__conv_right_0__Relu"
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__conv_right_0__Relu_is_bitvector 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__conv_right_0__Relu_is_rlz 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__conv_right_0__Relu_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__conv_right_0__Relu_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__conv_right_0__Relu_byte_offset 	2284	/* 0x000008ec */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__conv_right_0__Relu_vbase_byte_offset 	2292	/* 0x000008f4 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__conv_right_0__Relu_vmem_buffer_num_bytes 	43264

/* flexidag_openseg_prim_split_2 VCB PeleeNet__stage_3_dense_block_1__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_1__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu"
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_bitvector 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_rlz 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_byte_offset 	2332	/* 0x0000091c */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vbase_byte_offset 	2340	/* 0x00000924 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vmem_buffer_num_bytes 	21504

/* flexidag_openseg_prim_split_2 VCB PeleeNet__stage_3_dense_block_1__conv_left_0__Relu */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__conv_left_0__Relu_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_1__conv_left_0__Relu"
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__conv_left_0__Relu_is_bitvector 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__conv_left_0__Relu_is_rlz 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__conv_left_0__Relu_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__conv_left_0__Relu_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__conv_left_0__Relu_byte_offset 	2404	/* 0x00000964 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__conv_left_0__Relu_vbase_byte_offset 	2412	/* 0x0000096c */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__conv_left_0__Relu_vmem_buffer_num_bytes 	28160

/* flexidag_openseg_prim_split_2 SMB_output split_2__vas_created_terminate__v0 */
#define flexidag_openseg_prim_split_2_split_2__vas_created_terminate__v0_cnngen_demangled_name 	"split_2__vas_created_terminate__v0"
#define flexidag_openseg_prim_split_2_split_2__vas_created_terminate__v0_is_main_input_output 	0
#define flexidag_openseg_prim_split_2_split_2__vas_created_terminate__v0_is_bitvector 	1
#define flexidag_openseg_prim_split_2_split_2__vas_created_terminate__v0_is_rlz 	0
#define flexidag_openseg_prim_split_2_split_2__vas_created_terminate__v0_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2_split_2__vas_created_terminate__v0_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_2_split_2__vas_created_terminate__v0_byte_offset 	2464	/* 0x000009a0 */
#define flexidag_openseg_prim_split_2_split_2__vas_created_terminate__v0_vbase_byte_offset 	2468	/* 0x000009a4 */
#define flexidag_openseg_prim_split_2_split_2__vas_created_terminate__v0_W 	256
#define flexidag_openseg_prim_split_2_split_2__vas_created_terminate__v0_H 	3
#define flexidag_openseg_prim_split_2_split_2__vas_created_terminate__v0_D 	1
#define flexidag_openseg_prim_split_2_split_2__vas_created_terminate__v0_P 	1
#define flexidag_openseg_prim_split_2_split_2__vas_created_terminate__v0_data_num_bytes 	1
#define flexidag_openseg_prim_split_2_split_2__vas_created_terminate__v0_denable 	0
#define flexidag_openseg_prim_split_2_split_2__vas_created_terminate__v0_dram_format 	0
#define flexidag_openseg_prim_split_2_split_2__vas_created_terminate__v0_vp_interleave_mode 	1
#define flexidag_openseg_prim_split_2_split_2__vas_created_terminate__v0_tile_width 	1
#define flexidag_openseg_prim_split_2_split_2__vas_created_terminate__v0_tile_height 	1
#define flexidag_openseg_prim_split_2_split_2__vas_created_terminate__v0_vmem_buffer_num_bytes 	96


#endif /* flexidag_openseg_prim_split_2_H */
