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
#ifndef flexidag_openseg_prim_split_1_H
#define flexidag_openseg_prim_split_1_H

#define vas_gen_header_version                  	7

/* dag name */
#define flexidag_openseg_prim_split_1_DAG_NAME  	"flexidag_openseg_prim_split_1"

/* VDG file info */
#define flexidag_openseg_prim_split_1_vdg_name  	"flexidag_openseg_split_1.vdg"

/* VDG DAG memory info */
#define flexidag_openseg_prim_split_1_byte_offset 	4294967295	/* 0xffffffff */
#define flexidag_openseg_prim_split_1_byte_size 	4048	/* 0x00000fd0 */
#define flexidag_openseg_prim_split_1_preferred_dram_xfer_size 	1

/* flexidag_openseg_prim_split_1 VMEM info */
#define flexidag_openseg_prim_split_1_use_ping_pong_vmem 	0
#define flexidag_openseg_prim_split_1_VMEM_start 	4096	/* 0x00001000 */
#define flexidag_openseg_prim_split_1_VMEM_end  	2072944	/* 0x001fa170 */
#define flexidag_openseg_prim_split_1_image_start 	1952928	/* 0x001dcca0 */
#define flexidag_openseg_prim_split_1_image_size 	124064	/* 0x0001e4a0 */
#define flexidag_openseg_prim_split_1_dagbin_start 	2072944	/* 0x001fa170 */

/* flexidag_openseg_prim_split_1 DAG info */
#define flexidag_openseg_prim_split_1_estimated_cycles 	21228619

/*******************************************************************************
 * DAG byte offset info for HMB/SMB/VCB and their fields
 * in DAG 'flexidag_openseg_prim_split_1' in source file 'flexidag_openseg_split_1.vas'
 ******************************************************************************/
/* flexidag_openseg_prim_split_1 HMB_input PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool_cnngen_demangled_name 	"PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool"
#define flexidag_openseg_prim_split_1_PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool_is_main_input_output 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool_is_constant 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool_has_init_data 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool_is_variable_scalar 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool_ddi_byte_size 	0	/* 0x00000000 */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool_ddi_byte_offset 	4294967295	/* 0xffffffff */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool_is_bitvector 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool_is_rlz 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool_byte_offset 	0	/* 0x00000000 */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool_vbase_byte_offset 	4	/* 0x00000004 */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool_dbase_byte_offset 	28	/* 0x0000001c */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool_dpitchm1_byte_offset 	32	/* 0x00000020 */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool_dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool_vwidth_minus_one_byte_offset 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool_vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool_vheight_minus_one_byte_offset 	18	/* 0x00000012 */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool_vheight_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool_drotate_bit_offset 	198	/* 0x000000c6 */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool_hflip_bit_offset 	199	/* 0x000000c7 */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool_vflip_bit_offset 	200	/* 0x000000c8 */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool_dflip_bit_offset 	201	/* 0x000000c9 */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool_pflip_bit_offset 	202	/* 0x000000ca */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool_W 	160
#define flexidag_openseg_prim_split_1_PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool_H 	64
#define flexidag_openseg_prim_split_1_PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool_D 	128
#define flexidag_openseg_prim_split_1_PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool_P 	1
#define flexidag_openseg_prim_split_1_PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool_data_num_bytes 	1
#define flexidag_openseg_prim_split_1_PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool_denable 	1
#define flexidag_openseg_prim_split_1_PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool_dpitch_num_bytes 	32
#define flexidag_openseg_prim_split_1_PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool_dram_format 	3
#define flexidag_openseg_prim_split_1_PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool_vp_interleave_mode 	2
#define flexidag_openseg_prim_split_1_PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool_tile_width 	4
#define flexidag_openseg_prim_split_1_PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool_tile_height 	1
#define flexidag_openseg_prim_split_1_PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool_vmem_buffer_num_bytes 	83968
#define flexidag_openseg_prim_split_1_PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool_dram_size_num_bytes 	1310720
#define flexidag_openseg_prim_split_1_PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool_sign 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool_datasize 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool_expoffset 	5
#define flexidag_openseg_prim_split_1_PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool_expbits 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool_drotate 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool_hflip 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool_vflip 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool_dflip 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_1_transition_layer__transition_layer_avgpool__AvgPool_pflip 	0

/* flexidag_openseg_prim_split_1 SMB_input __pvcn_110004_ */
#define flexidag_openseg_prim_split_1___pvcn_110004__cnngen_demangled_name 	"__pvcn_110004_"
#define flexidag_openseg_prim_split_1___pvcn_110004__is_constant 	1
#define flexidag_openseg_prim_split_1___pvcn_110004__has_init_data 	1
#define flexidag_openseg_prim_split_1___pvcn_110004__is_variable_scalar 	0
#define flexidag_openseg_prim_split_1___pvcn_110004__is_bitvector 	0
#define flexidag_openseg_prim_split_1___pvcn_110004__is_rlz 	0
#define flexidag_openseg_prim_split_1___pvcn_110004__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_110004__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_110004__byte_offset 	40	/* 0x00000028 */
#define flexidag_openseg_prim_split_1___pvcn_110004__vbase_byte_offset 	44	/* 0x0000002c */
#define flexidag_openseg_prim_split_1___pvcn_110004__dbase_byte_offset 	68	/* 0x00000044 */
#define flexidag_openseg_prim_split_1___pvcn_110004__dpitchm1_byte_offset 	72	/* 0x00000048 */
#define flexidag_openseg_prim_split_1___pvcn_110004__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1___pvcn_110004__W 	2888
#define flexidag_openseg_prim_split_1___pvcn_110004__H 	1
#define flexidag_openseg_prim_split_1___pvcn_110004__D 	1
#define flexidag_openseg_prim_split_1___pvcn_110004__P 	1
#define flexidag_openseg_prim_split_1___pvcn_110004__data_num_bytes 	1
#define flexidag_openseg_prim_split_1___pvcn_110004__denable 	1
#define flexidag_openseg_prim_split_1___pvcn_110004__dpitch_num_bytes 	2912
#define flexidag_openseg_prim_split_1___pvcn_110004__dram_format 	0
#define flexidag_openseg_prim_split_1___pvcn_110004__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_1___pvcn_110004__tile_width 	1
#define flexidag_openseg_prim_split_1___pvcn_110004__tile_height 	1
#define flexidag_openseg_prim_split_1___pvcn_110004__vmem_buffer_num_bytes 	2888
#define flexidag_openseg_prim_split_1___pvcn_110004__dram_size_num_bytes 	2912

/* flexidag_openseg_prim_split_1 SMB_input __pvcn_110007_ */
#define flexidag_openseg_prim_split_1___pvcn_110007__cnngen_demangled_name 	"__pvcn_110007_"
#define flexidag_openseg_prim_split_1___pvcn_110007__is_constant 	1
#define flexidag_openseg_prim_split_1___pvcn_110007__has_init_data 	1
#define flexidag_openseg_prim_split_1___pvcn_110007__is_variable_scalar 	0
#define flexidag_openseg_prim_split_1___pvcn_110007__is_bitvector 	0
#define flexidag_openseg_prim_split_1___pvcn_110007__is_rlz 	0
#define flexidag_openseg_prim_split_1___pvcn_110007__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_110007__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_110007__byte_offset 	76	/* 0x0000004c */
#define flexidag_openseg_prim_split_1___pvcn_110007__vbase_byte_offset 	80	/* 0x00000050 */
#define flexidag_openseg_prim_split_1___pvcn_110007__dbase_byte_offset 	104	/* 0x00000068 */
#define flexidag_openseg_prim_split_1___pvcn_110007__dpitchm1_byte_offset 	108	/* 0x0000006c */
#define flexidag_openseg_prim_split_1___pvcn_110007__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1___pvcn_110007__W 	2888
#define flexidag_openseg_prim_split_1___pvcn_110007__H 	1
#define flexidag_openseg_prim_split_1___pvcn_110007__D 	1
#define flexidag_openseg_prim_split_1___pvcn_110007__P 	1
#define flexidag_openseg_prim_split_1___pvcn_110007__data_num_bytes 	1
#define flexidag_openseg_prim_split_1___pvcn_110007__denable 	1
#define flexidag_openseg_prim_split_1___pvcn_110007__dpitch_num_bytes 	2912
#define flexidag_openseg_prim_split_1___pvcn_110007__dram_format 	0
#define flexidag_openseg_prim_split_1___pvcn_110007__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_1___pvcn_110007__tile_width 	1
#define flexidag_openseg_prim_split_1___pvcn_110007__tile_height 	1
#define flexidag_openseg_prim_split_1___pvcn_110007__vmem_buffer_num_bytes 	2888
#define flexidag_openseg_prim_split_1___pvcn_110007__dram_size_num_bytes 	2912

/* flexidag_openseg_prim_split_1 SMB_input __pvcn_110005_ */
#define flexidag_openseg_prim_split_1___pvcn_110005__cnngen_demangled_name 	"__pvcn_110005_"
#define flexidag_openseg_prim_split_1___pvcn_110005__is_constant 	1
#define flexidag_openseg_prim_split_1___pvcn_110005__has_init_data 	1
#define flexidag_openseg_prim_split_1___pvcn_110005__is_variable_scalar 	0
#define flexidag_openseg_prim_split_1___pvcn_110005__is_bitvector 	0
#define flexidag_openseg_prim_split_1___pvcn_110005__is_rlz 	0
#define flexidag_openseg_prim_split_1___pvcn_110005__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_110005__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_110005__byte_offset 	116	/* 0x00000074 */
#define flexidag_openseg_prim_split_1___pvcn_110005__vbase_byte_offset 	120	/* 0x00000078 */
#define flexidag_openseg_prim_split_1___pvcn_110005__dbase_byte_offset 	144	/* 0x00000090 */
#define flexidag_openseg_prim_split_1___pvcn_110005__dpitchm1_byte_offset 	148	/* 0x00000094 */
#define flexidag_openseg_prim_split_1___pvcn_110005__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1___pvcn_110005__W 	272
#define flexidag_openseg_prim_split_1___pvcn_110005__H 	1
#define flexidag_openseg_prim_split_1___pvcn_110005__D 	1
#define flexidag_openseg_prim_split_1___pvcn_110005__P 	1
#define flexidag_openseg_prim_split_1___pvcn_110005__data_num_bytes 	1
#define flexidag_openseg_prim_split_1___pvcn_110005__denable 	1
#define flexidag_openseg_prim_split_1___pvcn_110005__dpitch_num_bytes 	288
#define flexidag_openseg_prim_split_1___pvcn_110005__dram_format 	0
#define flexidag_openseg_prim_split_1___pvcn_110005__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_1___pvcn_110005__tile_width 	1
#define flexidag_openseg_prim_split_1___pvcn_110005__tile_height 	1
#define flexidag_openseg_prim_split_1___pvcn_110005__vmem_buffer_num_bytes 	272
#define flexidag_openseg_prim_split_1___pvcn_110005__dram_size_num_bytes 	288

/* flexidag_openseg_prim_split_1 SMB_input __pvcn_110008_ */
#define flexidag_openseg_prim_split_1___pvcn_110008__cnngen_demangled_name 	"__pvcn_110008_"
#define flexidag_openseg_prim_split_1___pvcn_110008__is_constant 	1
#define flexidag_openseg_prim_split_1___pvcn_110008__has_init_data 	1
#define flexidag_openseg_prim_split_1___pvcn_110008__is_variable_scalar 	0
#define flexidag_openseg_prim_split_1___pvcn_110008__is_bitvector 	0
#define flexidag_openseg_prim_split_1___pvcn_110008__is_rlz 	0
#define flexidag_openseg_prim_split_1___pvcn_110008__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_110008__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_110008__byte_offset 	156	/* 0x0000009c */
#define flexidag_openseg_prim_split_1___pvcn_110008__vbase_byte_offset 	160	/* 0x000000a0 */
#define flexidag_openseg_prim_split_1___pvcn_110008__dbase_byte_offset 	184	/* 0x000000b8 */
#define flexidag_openseg_prim_split_1___pvcn_110008__dpitchm1_byte_offset 	188	/* 0x000000bc */
#define flexidag_openseg_prim_split_1___pvcn_110008__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1___pvcn_110008__W 	272
#define flexidag_openseg_prim_split_1___pvcn_110008__H 	1
#define flexidag_openseg_prim_split_1___pvcn_110008__D 	1
#define flexidag_openseg_prim_split_1___pvcn_110008__P 	1
#define flexidag_openseg_prim_split_1___pvcn_110008__data_num_bytes 	1
#define flexidag_openseg_prim_split_1___pvcn_110008__denable 	1
#define flexidag_openseg_prim_split_1___pvcn_110008__dpitch_num_bytes 	288
#define flexidag_openseg_prim_split_1___pvcn_110008__dram_format 	0
#define flexidag_openseg_prim_split_1___pvcn_110008__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_1___pvcn_110008__tile_width 	1
#define flexidag_openseg_prim_split_1___pvcn_110008__tile_height 	1
#define flexidag_openseg_prim_split_1___pvcn_110008__vmem_buffer_num_bytes 	272
#define flexidag_openseg_prim_split_1___pvcn_110008__dram_size_num_bytes 	288

/* flexidag_openseg_prim_split_1 SMB_input __pvcn_939_ */
#define flexidag_openseg_prim_split_1___pvcn_939__cnngen_demangled_name 	"__pvcn_939_"
#define flexidag_openseg_prim_split_1___pvcn_939__is_constant 	1
#define flexidag_openseg_prim_split_1___pvcn_939__has_init_data 	1
#define flexidag_openseg_prim_split_1___pvcn_939__is_variable_scalar 	0
#define flexidag_openseg_prim_split_1___pvcn_939__is_bitvector 	0
#define flexidag_openseg_prim_split_1___pvcn_939__is_rlz 	0
#define flexidag_openseg_prim_split_1___pvcn_939__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_939__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_939__byte_offset 	196	/* 0x000000c4 */
#define flexidag_openseg_prim_split_1___pvcn_939__vbase_byte_offset 	200	/* 0x000000c8 */
#define flexidag_openseg_prim_split_1___pvcn_939__dbase_byte_offset 	224	/* 0x000000e0 */
#define flexidag_openseg_prim_split_1___pvcn_939__dpitchm1_byte_offset 	228	/* 0x000000e4 */
#define flexidag_openseg_prim_split_1___pvcn_939__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1___pvcn_939__W 	3216
#define flexidag_openseg_prim_split_1___pvcn_939__H 	1
#define flexidag_openseg_prim_split_1___pvcn_939__D 	1
#define flexidag_openseg_prim_split_1___pvcn_939__P 	1
#define flexidag_openseg_prim_split_1___pvcn_939__data_num_bytes 	1
#define flexidag_openseg_prim_split_1___pvcn_939__denable 	1
#define flexidag_openseg_prim_split_1___pvcn_939__dpitch_num_bytes 	3232
#define flexidag_openseg_prim_split_1___pvcn_939__dram_format 	0
#define flexidag_openseg_prim_split_1___pvcn_939__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_1___pvcn_939__tile_width 	1
#define flexidag_openseg_prim_split_1___pvcn_939__tile_height 	1
#define flexidag_openseg_prim_split_1___pvcn_939__vmem_buffer_num_bytes 	3216
#define flexidag_openseg_prim_split_1___pvcn_939__dram_size_num_bytes 	3232

/* flexidag_openseg_prim_split_1 SMB_input __pvcn_940_ */
#define flexidag_openseg_prim_split_1___pvcn_940__cnngen_demangled_name 	"__pvcn_940_"
#define flexidag_openseg_prim_split_1___pvcn_940__is_constant 	1
#define flexidag_openseg_prim_split_1___pvcn_940__has_init_data 	1
#define flexidag_openseg_prim_split_1___pvcn_940__is_variable_scalar 	0
#define flexidag_openseg_prim_split_1___pvcn_940__is_bitvector 	0
#define flexidag_openseg_prim_split_1___pvcn_940__is_rlz 	0
#define flexidag_openseg_prim_split_1___pvcn_940__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_940__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_940__byte_offset 	236	/* 0x000000ec */
#define flexidag_openseg_prim_split_1___pvcn_940__vbase_byte_offset 	240	/* 0x000000f0 */
#define flexidag_openseg_prim_split_1___pvcn_940__dbase_byte_offset 	264	/* 0x00000108 */
#define flexidag_openseg_prim_split_1___pvcn_940__dpitchm1_byte_offset 	268	/* 0x0000010c */
#define flexidag_openseg_prim_split_1___pvcn_940__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1___pvcn_940__W 	136
#define flexidag_openseg_prim_split_1___pvcn_940__H 	1
#define flexidag_openseg_prim_split_1___pvcn_940__D 	1
#define flexidag_openseg_prim_split_1___pvcn_940__P 	1
#define flexidag_openseg_prim_split_1___pvcn_940__data_num_bytes 	1
#define flexidag_openseg_prim_split_1___pvcn_940__denable 	1
#define flexidag_openseg_prim_split_1___pvcn_940__dpitch_num_bytes 	160
#define flexidag_openseg_prim_split_1___pvcn_940__dram_format 	0
#define flexidag_openseg_prim_split_1___pvcn_940__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_1___pvcn_940__tile_width 	1
#define flexidag_openseg_prim_split_1___pvcn_940__tile_height 	1
#define flexidag_openseg_prim_split_1___pvcn_940__vmem_buffer_num_bytes 	136
#define flexidag_openseg_prim_split_1___pvcn_940__dram_size_num_bytes 	160

/* flexidag_openseg_prim_split_1 SMB_input __pvcn_942_ */
#define flexidag_openseg_prim_split_1___pvcn_942__cnngen_demangled_name 	"__pvcn_942_"
#define flexidag_openseg_prim_split_1___pvcn_942__is_constant 	1
#define flexidag_openseg_prim_split_1___pvcn_942__has_init_data 	1
#define flexidag_openseg_prim_split_1___pvcn_942__is_variable_scalar 	0
#define flexidag_openseg_prim_split_1___pvcn_942__is_bitvector 	0
#define flexidag_openseg_prim_split_1___pvcn_942__is_rlz 	0
#define flexidag_openseg_prim_split_1___pvcn_942__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_942__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_942__byte_offset 	276	/* 0x00000114 */
#define flexidag_openseg_prim_split_1___pvcn_942__vbase_byte_offset 	280	/* 0x00000118 */
#define flexidag_openseg_prim_split_1___pvcn_942__dbase_byte_offset 	304	/* 0x00000130 */
#define flexidag_openseg_prim_split_1___pvcn_942__dpitchm1_byte_offset 	308	/* 0x00000134 */
#define flexidag_openseg_prim_split_1___pvcn_942__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1___pvcn_942__W 	1648
#define flexidag_openseg_prim_split_1___pvcn_942__H 	1
#define flexidag_openseg_prim_split_1___pvcn_942__D 	1
#define flexidag_openseg_prim_split_1___pvcn_942__P 	1
#define flexidag_openseg_prim_split_1___pvcn_942__data_num_bytes 	1
#define flexidag_openseg_prim_split_1___pvcn_942__denable 	1
#define flexidag_openseg_prim_split_1___pvcn_942__dpitch_num_bytes 	1664
#define flexidag_openseg_prim_split_1___pvcn_942__dram_format 	0
#define flexidag_openseg_prim_split_1___pvcn_942__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_1___pvcn_942__tile_width 	1
#define flexidag_openseg_prim_split_1___pvcn_942__tile_height 	1
#define flexidag_openseg_prim_split_1___pvcn_942__vmem_buffer_num_bytes 	1648
#define flexidag_openseg_prim_split_1___pvcn_942__dram_size_num_bytes 	1664

/* flexidag_openseg_prim_split_1 SMB_input __pvcn_943_ */
#define flexidag_openseg_prim_split_1___pvcn_943__cnngen_demangled_name 	"__pvcn_943_"
#define flexidag_openseg_prim_split_1___pvcn_943__is_constant 	1
#define flexidag_openseg_prim_split_1___pvcn_943__has_init_data 	1
#define flexidag_openseg_prim_split_1___pvcn_943__is_variable_scalar 	0
#define flexidag_openseg_prim_split_1___pvcn_943__is_bitvector 	0
#define flexidag_openseg_prim_split_1___pvcn_943__is_rlz 	0
#define flexidag_openseg_prim_split_1___pvcn_943__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_943__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_943__byte_offset 	316	/* 0x0000013c */
#define flexidag_openseg_prim_split_1___pvcn_943__vbase_byte_offset 	320	/* 0x00000140 */
#define flexidag_openseg_prim_split_1___pvcn_943__dbase_byte_offset 	344	/* 0x00000158 */
#define flexidag_openseg_prim_split_1___pvcn_943__dpitchm1_byte_offset 	348	/* 0x0000015c */
#define flexidag_openseg_prim_split_1___pvcn_943__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1___pvcn_943__W 	136
#define flexidag_openseg_prim_split_1___pvcn_943__H 	1
#define flexidag_openseg_prim_split_1___pvcn_943__D 	1
#define flexidag_openseg_prim_split_1___pvcn_943__P 	1
#define flexidag_openseg_prim_split_1___pvcn_943__data_num_bytes 	1
#define flexidag_openseg_prim_split_1___pvcn_943__denable 	1
#define flexidag_openseg_prim_split_1___pvcn_943__dpitch_num_bytes 	160
#define flexidag_openseg_prim_split_1___pvcn_943__dram_format 	0
#define flexidag_openseg_prim_split_1___pvcn_943__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_1___pvcn_943__tile_width 	1
#define flexidag_openseg_prim_split_1___pvcn_943__tile_height 	1
#define flexidag_openseg_prim_split_1___pvcn_943__vmem_buffer_num_bytes 	136
#define flexidag_openseg_prim_split_1___pvcn_943__dram_size_num_bytes 	160

/* flexidag_openseg_prim_split_1 SMB_input __pvcn_946_ */
#define flexidag_openseg_prim_split_1___pvcn_946__cnngen_demangled_name 	"__pvcn_946_"
#define flexidag_openseg_prim_split_1___pvcn_946__is_constant 	1
#define flexidag_openseg_prim_split_1___pvcn_946__has_init_data 	1
#define flexidag_openseg_prim_split_1___pvcn_946__is_variable_scalar 	0
#define flexidag_openseg_prim_split_1___pvcn_946__is_bitvector 	0
#define flexidag_openseg_prim_split_1___pvcn_946__is_rlz 	0
#define flexidag_openseg_prim_split_1___pvcn_946__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_946__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_946__byte_offset 	356	/* 0x00000164 */
#define flexidag_openseg_prim_split_1___pvcn_946__vbase_byte_offset 	360	/* 0x00000168 */
#define flexidag_openseg_prim_split_1___pvcn_946__dbase_byte_offset 	384	/* 0x00000180 */
#define flexidag_openseg_prim_split_1___pvcn_946__dpitchm1_byte_offset 	388	/* 0x00000184 */
#define flexidag_openseg_prim_split_1___pvcn_946__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1___pvcn_946__W 	3236
#define flexidag_openseg_prim_split_1___pvcn_946__H 	1
#define flexidag_openseg_prim_split_1___pvcn_946__D 	1
#define flexidag_openseg_prim_split_1___pvcn_946__P 	1
#define flexidag_openseg_prim_split_1___pvcn_946__data_num_bytes 	1
#define flexidag_openseg_prim_split_1___pvcn_946__denable 	1
#define flexidag_openseg_prim_split_1___pvcn_946__dpitch_num_bytes 	3264
#define flexidag_openseg_prim_split_1___pvcn_946__dram_format 	0
#define flexidag_openseg_prim_split_1___pvcn_946__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_1___pvcn_946__tile_width 	1
#define flexidag_openseg_prim_split_1___pvcn_946__tile_height 	1
#define flexidag_openseg_prim_split_1___pvcn_946__vmem_buffer_num_bytes 	3236
#define flexidag_openseg_prim_split_1___pvcn_946__dram_size_num_bytes 	3264

/* flexidag_openseg_prim_split_1 SMB_input __pvcn_947_ */
#define flexidag_openseg_prim_split_1___pvcn_947__cnngen_demangled_name 	"__pvcn_947_"
#define flexidag_openseg_prim_split_1___pvcn_947__is_constant 	1
#define flexidag_openseg_prim_split_1___pvcn_947__has_init_data 	1
#define flexidag_openseg_prim_split_1___pvcn_947__is_variable_scalar 	0
#define flexidag_openseg_prim_split_1___pvcn_947__is_bitvector 	0
#define flexidag_openseg_prim_split_1___pvcn_947__is_rlz 	0
#define flexidag_openseg_prim_split_1___pvcn_947__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_947__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_947__byte_offset 	396	/* 0x0000018c */
#define flexidag_openseg_prim_split_1___pvcn_947__vbase_byte_offset 	400	/* 0x00000190 */
#define flexidag_openseg_prim_split_1___pvcn_947__dbase_byte_offset 	424	/* 0x000001a8 */
#define flexidag_openseg_prim_split_1___pvcn_947__dpitchm1_byte_offset 	428	/* 0x000001ac */
#define flexidag_openseg_prim_split_1___pvcn_947__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1___pvcn_947__W 	144
#define flexidag_openseg_prim_split_1___pvcn_947__H 	1
#define flexidag_openseg_prim_split_1___pvcn_947__D 	1
#define flexidag_openseg_prim_split_1___pvcn_947__P 	1
#define flexidag_openseg_prim_split_1___pvcn_947__data_num_bytes 	1
#define flexidag_openseg_prim_split_1___pvcn_947__denable 	1
#define flexidag_openseg_prim_split_1___pvcn_947__dpitch_num_bytes 	160
#define flexidag_openseg_prim_split_1___pvcn_947__dram_format 	0
#define flexidag_openseg_prim_split_1___pvcn_947__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_1___pvcn_947__tile_width 	1
#define flexidag_openseg_prim_split_1___pvcn_947__tile_height 	1
#define flexidag_openseg_prim_split_1___pvcn_947__vmem_buffer_num_bytes 	144
#define flexidag_openseg_prim_split_1___pvcn_947__dram_size_num_bytes 	160

/* flexidag_openseg_prim_split_1 SMB_input __pvcn_953_ */
#define flexidag_openseg_prim_split_1___pvcn_953__cnngen_demangled_name 	"__pvcn_953_"
#define flexidag_openseg_prim_split_1___pvcn_953__is_constant 	1
#define flexidag_openseg_prim_split_1___pvcn_953__has_init_data 	1
#define flexidag_openseg_prim_split_1___pvcn_953__is_variable_scalar 	0
#define flexidag_openseg_prim_split_1___pvcn_953__is_bitvector 	0
#define flexidag_openseg_prim_split_1___pvcn_953__is_rlz 	0
#define flexidag_openseg_prim_split_1___pvcn_953__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_953__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_953__byte_offset 	436	/* 0x000001b4 */
#define flexidag_openseg_prim_split_1___pvcn_953__vbase_byte_offset 	440	/* 0x000001b8 */
#define flexidag_openseg_prim_split_1___pvcn_953__dbase_byte_offset 	464	/* 0x000001d0 */
#define flexidag_openseg_prim_split_1___pvcn_953__dpitchm1_byte_offset 	468	/* 0x000001d4 */
#define flexidag_openseg_prim_split_1___pvcn_953__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1___pvcn_953__W 	7316
#define flexidag_openseg_prim_split_1___pvcn_953__H 	1
#define flexidag_openseg_prim_split_1___pvcn_953__D 	1
#define flexidag_openseg_prim_split_1___pvcn_953__P 	1
#define flexidag_openseg_prim_split_1___pvcn_953__data_num_bytes 	1
#define flexidag_openseg_prim_split_1___pvcn_953__denable 	1
#define flexidag_openseg_prim_split_1___pvcn_953__dpitch_num_bytes 	7328
#define flexidag_openseg_prim_split_1___pvcn_953__dram_format 	0
#define flexidag_openseg_prim_split_1___pvcn_953__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_1___pvcn_953__tile_width 	1
#define flexidag_openseg_prim_split_1___pvcn_953__tile_height 	1
#define flexidag_openseg_prim_split_1___pvcn_953__vmem_buffer_num_bytes 	7316
#define flexidag_openseg_prim_split_1___pvcn_953__dram_size_num_bytes 	7328

/* flexidag_openseg_prim_split_1 SMB_input __pvcn_954_ */
#define flexidag_openseg_prim_split_1___pvcn_954__cnngen_demangled_name 	"__pvcn_954_"
#define flexidag_openseg_prim_split_1___pvcn_954__is_constant 	1
#define flexidag_openseg_prim_split_1___pvcn_954__has_init_data 	1
#define flexidag_openseg_prim_split_1___pvcn_954__is_variable_scalar 	0
#define flexidag_openseg_prim_split_1___pvcn_954__is_bitvector 	0
#define flexidag_openseg_prim_split_1___pvcn_954__is_rlz 	0
#define flexidag_openseg_prim_split_1___pvcn_954__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_954__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_954__byte_offset 	476	/* 0x000001dc */
#define flexidag_openseg_prim_split_1___pvcn_954__vbase_byte_offset 	480	/* 0x000001e0 */
#define flexidag_openseg_prim_split_1___pvcn_954__dbase_byte_offset 	504	/* 0x000001f8 */
#define flexidag_openseg_prim_split_1___pvcn_954__dpitchm1_byte_offset 	508	/* 0x000001fc */
#define flexidag_openseg_prim_split_1___pvcn_954__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1___pvcn_954__W 	544
#define flexidag_openseg_prim_split_1___pvcn_954__H 	1
#define flexidag_openseg_prim_split_1___pvcn_954__D 	1
#define flexidag_openseg_prim_split_1___pvcn_954__P 	1
#define flexidag_openseg_prim_split_1___pvcn_954__data_num_bytes 	1
#define flexidag_openseg_prim_split_1___pvcn_954__denable 	1
#define flexidag_openseg_prim_split_1___pvcn_954__dpitch_num_bytes 	544
#define flexidag_openseg_prim_split_1___pvcn_954__dram_format 	0
#define flexidag_openseg_prim_split_1___pvcn_954__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_1___pvcn_954__tile_width 	1
#define flexidag_openseg_prim_split_1___pvcn_954__tile_height 	1
#define flexidag_openseg_prim_split_1___pvcn_954__vmem_buffer_num_bytes 	544
#define flexidag_openseg_prim_split_1___pvcn_954__dram_size_num_bytes 	544

/* flexidag_openseg_prim_split_1 SMB_input __pvcn_957_ */
#define flexidag_openseg_prim_split_1___pvcn_957__cnngen_demangled_name 	"__pvcn_957_"
#define flexidag_openseg_prim_split_1___pvcn_957__is_constant 	1
#define flexidag_openseg_prim_split_1___pvcn_957__has_init_data 	1
#define flexidag_openseg_prim_split_1___pvcn_957__is_variable_scalar 	0
#define flexidag_openseg_prim_split_1___pvcn_957__is_bitvector 	0
#define flexidag_openseg_prim_split_1___pvcn_957__is_rlz 	0
#define flexidag_openseg_prim_split_1___pvcn_957__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_957__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_957__byte_offset 	516	/* 0x00000204 */
#define flexidag_openseg_prim_split_1___pvcn_957__vbase_byte_offset 	520	/* 0x00000208 */
#define flexidag_openseg_prim_split_1___pvcn_957__dbase_byte_offset 	544	/* 0x00000220 */
#define flexidag_openseg_prim_split_1___pvcn_957__dpitchm1_byte_offset 	548	/* 0x00000224 */
#define flexidag_openseg_prim_split_1___pvcn_957__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1___pvcn_957__W 	3264
#define flexidag_openseg_prim_split_1___pvcn_957__H 	1
#define flexidag_openseg_prim_split_1___pvcn_957__D 	1
#define flexidag_openseg_prim_split_1___pvcn_957__P 	1
#define flexidag_openseg_prim_split_1___pvcn_957__data_num_bytes 	1
#define flexidag_openseg_prim_split_1___pvcn_957__denable 	1
#define flexidag_openseg_prim_split_1___pvcn_957__dpitch_num_bytes 	3264
#define flexidag_openseg_prim_split_1___pvcn_957__dram_format 	0
#define flexidag_openseg_prim_split_1___pvcn_957__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_1___pvcn_957__tile_width 	1
#define flexidag_openseg_prim_split_1___pvcn_957__tile_height 	1
#define flexidag_openseg_prim_split_1___pvcn_957__vmem_buffer_num_bytes 	3264
#define flexidag_openseg_prim_split_1___pvcn_957__dram_size_num_bytes 	3264

/* flexidag_openseg_prim_split_1 SMB_input __pvcn_958_ */
#define flexidag_openseg_prim_split_1___pvcn_958__cnngen_demangled_name 	"__pvcn_958_"
#define flexidag_openseg_prim_split_1___pvcn_958__is_constant 	1
#define flexidag_openseg_prim_split_1___pvcn_958__has_init_data 	1
#define flexidag_openseg_prim_split_1___pvcn_958__is_variable_scalar 	0
#define flexidag_openseg_prim_split_1___pvcn_958__is_bitvector 	0
#define flexidag_openseg_prim_split_1___pvcn_958__is_rlz 	0
#define flexidag_openseg_prim_split_1___pvcn_958__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_958__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_958__byte_offset 	556	/* 0x0000022c */
#define flexidag_openseg_prim_split_1___pvcn_958__vbase_byte_offset 	560	/* 0x00000230 */
#define flexidag_openseg_prim_split_1___pvcn_958__dbase_byte_offset 	584	/* 0x00000248 */
#define flexidag_openseg_prim_split_1___pvcn_958__dpitchm1_byte_offset 	588	/* 0x0000024c */
#define flexidag_openseg_prim_split_1___pvcn_958__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1___pvcn_958__W 	144
#define flexidag_openseg_prim_split_1___pvcn_958__H 	1
#define flexidag_openseg_prim_split_1___pvcn_958__D 	1
#define flexidag_openseg_prim_split_1___pvcn_958__P 	1
#define flexidag_openseg_prim_split_1___pvcn_958__data_num_bytes 	1
#define flexidag_openseg_prim_split_1___pvcn_958__denable 	1
#define flexidag_openseg_prim_split_1___pvcn_958__dpitch_num_bytes 	160
#define flexidag_openseg_prim_split_1___pvcn_958__dram_format 	0
#define flexidag_openseg_prim_split_1___pvcn_958__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_1___pvcn_958__tile_width 	1
#define flexidag_openseg_prim_split_1___pvcn_958__tile_height 	1
#define flexidag_openseg_prim_split_1___pvcn_958__vmem_buffer_num_bytes 	144
#define flexidag_openseg_prim_split_1___pvcn_958__dram_size_num_bytes 	160

/* flexidag_openseg_prim_split_1 SMB_input __pvcn_960_ */
#define flexidag_openseg_prim_split_1___pvcn_960__cnngen_demangled_name 	"__pvcn_960_"
#define flexidag_openseg_prim_split_1___pvcn_960__is_constant 	1
#define flexidag_openseg_prim_split_1___pvcn_960__has_init_data 	1
#define flexidag_openseg_prim_split_1___pvcn_960__is_variable_scalar 	0
#define flexidag_openseg_prim_split_1___pvcn_960__is_bitvector 	0
#define flexidag_openseg_prim_split_1___pvcn_960__is_rlz 	0
#define flexidag_openseg_prim_split_1___pvcn_960__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_960__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_960__byte_offset 	596	/* 0x00000254 */
#define flexidag_openseg_prim_split_1___pvcn_960__vbase_byte_offset 	600	/* 0x00000258 */
#define flexidag_openseg_prim_split_1___pvcn_960__dbase_byte_offset 	624	/* 0x00000270 */
#define flexidag_openseg_prim_split_1___pvcn_960__dpitchm1_byte_offset 	628	/* 0x00000274 */
#define flexidag_openseg_prim_split_1___pvcn_960__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1___pvcn_960__W 	1620
#define flexidag_openseg_prim_split_1___pvcn_960__H 	1
#define flexidag_openseg_prim_split_1___pvcn_960__D 	1
#define flexidag_openseg_prim_split_1___pvcn_960__P 	1
#define flexidag_openseg_prim_split_1___pvcn_960__data_num_bytes 	1
#define flexidag_openseg_prim_split_1___pvcn_960__denable 	1
#define flexidag_openseg_prim_split_1___pvcn_960__dpitch_num_bytes 	1632
#define flexidag_openseg_prim_split_1___pvcn_960__dram_format 	0
#define flexidag_openseg_prim_split_1___pvcn_960__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_1___pvcn_960__tile_width 	1
#define flexidag_openseg_prim_split_1___pvcn_960__tile_height 	1
#define flexidag_openseg_prim_split_1___pvcn_960__vmem_buffer_num_bytes 	1620
#define flexidag_openseg_prim_split_1___pvcn_960__dram_size_num_bytes 	1632

/* flexidag_openseg_prim_split_1 SMB_input __pvcn_961_ */
#define flexidag_openseg_prim_split_1___pvcn_961__cnngen_demangled_name 	"__pvcn_961_"
#define flexidag_openseg_prim_split_1___pvcn_961__is_constant 	1
#define flexidag_openseg_prim_split_1___pvcn_961__has_init_data 	1
#define flexidag_openseg_prim_split_1___pvcn_961__is_variable_scalar 	0
#define flexidag_openseg_prim_split_1___pvcn_961__is_bitvector 	0
#define flexidag_openseg_prim_split_1___pvcn_961__is_rlz 	0
#define flexidag_openseg_prim_split_1___pvcn_961__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_961__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_961__byte_offset 	636	/* 0x0000027c */
#define flexidag_openseg_prim_split_1___pvcn_961__vbase_byte_offset 	640	/* 0x00000280 */
#define flexidag_openseg_prim_split_1___pvcn_961__dbase_byte_offset 	664	/* 0x00000298 */
#define flexidag_openseg_prim_split_1___pvcn_961__dpitchm1_byte_offset 	668	/* 0x0000029c */
#define flexidag_openseg_prim_split_1___pvcn_961__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1___pvcn_961__W 	144
#define flexidag_openseg_prim_split_1___pvcn_961__H 	1
#define flexidag_openseg_prim_split_1___pvcn_961__D 	1
#define flexidag_openseg_prim_split_1___pvcn_961__P 	1
#define flexidag_openseg_prim_split_1___pvcn_961__data_num_bytes 	1
#define flexidag_openseg_prim_split_1___pvcn_961__denable 	1
#define flexidag_openseg_prim_split_1___pvcn_961__dpitch_num_bytes 	160
#define flexidag_openseg_prim_split_1___pvcn_961__dram_format 	0
#define flexidag_openseg_prim_split_1___pvcn_961__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_1___pvcn_961__tile_width 	1
#define flexidag_openseg_prim_split_1___pvcn_961__tile_height 	1
#define flexidag_openseg_prim_split_1___pvcn_961__vmem_buffer_num_bytes 	144
#define flexidag_openseg_prim_split_1___pvcn_961__dram_size_num_bytes 	160

/* flexidag_openseg_prim_split_1 SMB_input __pvcn_964_ */
#define flexidag_openseg_prim_split_1___pvcn_964__cnngen_demangled_name 	"__pvcn_964_"
#define flexidag_openseg_prim_split_1___pvcn_964__is_constant 	1
#define flexidag_openseg_prim_split_1___pvcn_964__has_init_data 	1
#define flexidag_openseg_prim_split_1___pvcn_964__is_variable_scalar 	0
#define flexidag_openseg_prim_split_1___pvcn_964__is_bitvector 	0
#define flexidag_openseg_prim_split_1___pvcn_964__is_rlz 	0
#define flexidag_openseg_prim_split_1___pvcn_964__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_964__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_964__byte_offset 	676	/* 0x000002a4 */
#define flexidag_openseg_prim_split_1___pvcn_964__vbase_byte_offset 	680	/* 0x000002a8 */
#define flexidag_openseg_prim_split_1___pvcn_964__dbase_byte_offset 	704	/* 0x000002c0 */
#define flexidag_openseg_prim_split_1___pvcn_964__dpitchm1_byte_offset 	708	/* 0x000002c4 */
#define flexidag_openseg_prim_split_1___pvcn_964__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1___pvcn_964__W 	3252
#define flexidag_openseg_prim_split_1___pvcn_964__H 	1
#define flexidag_openseg_prim_split_1___pvcn_964__D 	1
#define flexidag_openseg_prim_split_1___pvcn_964__P 	1
#define flexidag_openseg_prim_split_1___pvcn_964__data_num_bytes 	1
#define flexidag_openseg_prim_split_1___pvcn_964__denable 	1
#define flexidag_openseg_prim_split_1___pvcn_964__dpitch_num_bytes 	3264
#define flexidag_openseg_prim_split_1___pvcn_964__dram_format 	0
#define flexidag_openseg_prim_split_1___pvcn_964__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_1___pvcn_964__tile_width 	1
#define flexidag_openseg_prim_split_1___pvcn_964__tile_height 	1
#define flexidag_openseg_prim_split_1___pvcn_964__vmem_buffer_num_bytes 	3252
#define flexidag_openseg_prim_split_1___pvcn_964__dram_size_num_bytes 	3264

/* flexidag_openseg_prim_split_1 SMB_input __pvcn_965_ */
#define flexidag_openseg_prim_split_1___pvcn_965__cnngen_demangled_name 	"__pvcn_965_"
#define flexidag_openseg_prim_split_1___pvcn_965__is_constant 	1
#define flexidag_openseg_prim_split_1___pvcn_965__has_init_data 	1
#define flexidag_openseg_prim_split_1___pvcn_965__is_variable_scalar 	0
#define flexidag_openseg_prim_split_1___pvcn_965__is_bitvector 	0
#define flexidag_openseg_prim_split_1___pvcn_965__is_rlz 	0
#define flexidag_openseg_prim_split_1___pvcn_965__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_965__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_965__byte_offset 	716	/* 0x000002cc */
#define flexidag_openseg_prim_split_1___pvcn_965__vbase_byte_offset 	720	/* 0x000002d0 */
#define flexidag_openseg_prim_split_1___pvcn_965__dbase_byte_offset 	744	/* 0x000002e8 */
#define flexidag_openseg_prim_split_1___pvcn_965__dpitchm1_byte_offset 	748	/* 0x000002ec */
#define flexidag_openseg_prim_split_1___pvcn_965__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1___pvcn_965__W 	144
#define flexidag_openseg_prim_split_1___pvcn_965__H 	1
#define flexidag_openseg_prim_split_1___pvcn_965__D 	1
#define flexidag_openseg_prim_split_1___pvcn_965__P 	1
#define flexidag_openseg_prim_split_1___pvcn_965__data_num_bytes 	1
#define flexidag_openseg_prim_split_1___pvcn_965__denable 	1
#define flexidag_openseg_prim_split_1___pvcn_965__dpitch_num_bytes 	160
#define flexidag_openseg_prim_split_1___pvcn_965__dram_format 	0
#define flexidag_openseg_prim_split_1___pvcn_965__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_1___pvcn_965__tile_width 	1
#define flexidag_openseg_prim_split_1___pvcn_965__tile_height 	1
#define flexidag_openseg_prim_split_1___pvcn_965__vmem_buffer_num_bytes 	144
#define flexidag_openseg_prim_split_1___pvcn_965__dram_size_num_bytes 	160

/* flexidag_openseg_prim_split_1 SMB_input __pvcn_971_ */
#define flexidag_openseg_prim_split_1___pvcn_971__cnngen_demangled_name 	"__pvcn_971_"
#define flexidag_openseg_prim_split_1___pvcn_971__is_constant 	1
#define flexidag_openseg_prim_split_1___pvcn_971__has_init_data 	1
#define flexidag_openseg_prim_split_1___pvcn_971__is_variable_scalar 	0
#define flexidag_openseg_prim_split_1___pvcn_971__is_bitvector 	0
#define flexidag_openseg_prim_split_1___pvcn_971__is_rlz 	0
#define flexidag_openseg_prim_split_1___pvcn_971__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_971__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_971__byte_offset 	756	/* 0x000002f4 */
#define flexidag_openseg_prim_split_1___pvcn_971__vbase_byte_offset 	760	/* 0x000002f8 */
#define flexidag_openseg_prim_split_1___pvcn_971__dbase_byte_offset 	784	/* 0x00000310 */
#define flexidag_openseg_prim_split_1___pvcn_971__dpitchm1_byte_offset 	788	/* 0x00000314 */
#define flexidag_openseg_prim_split_1___pvcn_971__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1___pvcn_971__W 	8652
#define flexidag_openseg_prim_split_1___pvcn_971__H 	1
#define flexidag_openseg_prim_split_1___pvcn_971__D 	1
#define flexidag_openseg_prim_split_1___pvcn_971__P 	1
#define flexidag_openseg_prim_split_1___pvcn_971__data_num_bytes 	1
#define flexidag_openseg_prim_split_1___pvcn_971__denable 	1
#define flexidag_openseg_prim_split_1___pvcn_971__dpitch_num_bytes 	8672
#define flexidag_openseg_prim_split_1___pvcn_971__dram_format 	0
#define flexidag_openseg_prim_split_1___pvcn_971__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_1___pvcn_971__tile_width 	1
#define flexidag_openseg_prim_split_1___pvcn_971__tile_height 	1
#define flexidag_openseg_prim_split_1___pvcn_971__vmem_buffer_num_bytes 	8652
#define flexidag_openseg_prim_split_1___pvcn_971__dram_size_num_bytes 	8672

/* flexidag_openseg_prim_split_1 SMB_input __pvcn_972_ */
#define flexidag_openseg_prim_split_1___pvcn_972__cnngen_demangled_name 	"__pvcn_972_"
#define flexidag_openseg_prim_split_1___pvcn_972__is_constant 	1
#define flexidag_openseg_prim_split_1___pvcn_972__has_init_data 	1
#define flexidag_openseg_prim_split_1___pvcn_972__is_variable_scalar 	0
#define flexidag_openseg_prim_split_1___pvcn_972__is_bitvector 	0
#define flexidag_openseg_prim_split_1___pvcn_972__is_rlz 	0
#define flexidag_openseg_prim_split_1___pvcn_972__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_972__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_972__byte_offset 	796	/* 0x0000031c */
#define flexidag_openseg_prim_split_1___pvcn_972__vbase_byte_offset 	800	/* 0x00000320 */
#define flexidag_openseg_prim_split_1___pvcn_972__dbase_byte_offset 	824	/* 0x00000338 */
#define flexidag_openseg_prim_split_1___pvcn_972__dpitchm1_byte_offset 	828	/* 0x0000033c */
#define flexidag_openseg_prim_split_1___pvcn_972__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1___pvcn_972__W 	544
#define flexidag_openseg_prim_split_1___pvcn_972__H 	1
#define flexidag_openseg_prim_split_1___pvcn_972__D 	1
#define flexidag_openseg_prim_split_1___pvcn_972__P 	1
#define flexidag_openseg_prim_split_1___pvcn_972__data_num_bytes 	1
#define flexidag_openseg_prim_split_1___pvcn_972__denable 	1
#define flexidag_openseg_prim_split_1___pvcn_972__dpitch_num_bytes 	544
#define flexidag_openseg_prim_split_1___pvcn_972__dram_format 	0
#define flexidag_openseg_prim_split_1___pvcn_972__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_1___pvcn_972__tile_width 	1
#define flexidag_openseg_prim_split_1___pvcn_972__tile_height 	1
#define flexidag_openseg_prim_split_1___pvcn_972__vmem_buffer_num_bytes 	544
#define flexidag_openseg_prim_split_1___pvcn_972__dram_size_num_bytes 	544

/* flexidag_openseg_prim_split_1 SMB_input __pvcn_975_ */
#define flexidag_openseg_prim_split_1___pvcn_975__cnngen_demangled_name 	"__pvcn_975_"
#define flexidag_openseg_prim_split_1___pvcn_975__is_constant 	1
#define flexidag_openseg_prim_split_1___pvcn_975__has_init_data 	1
#define flexidag_openseg_prim_split_1___pvcn_975__is_variable_scalar 	0
#define flexidag_openseg_prim_split_1___pvcn_975__is_bitvector 	0
#define flexidag_openseg_prim_split_1___pvcn_975__is_rlz 	0
#define flexidag_openseg_prim_split_1___pvcn_975__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_975__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_975__byte_offset 	836	/* 0x00000344 */
#define flexidag_openseg_prim_split_1___pvcn_975__vbase_byte_offset 	840	/* 0x00000348 */
#define flexidag_openseg_prim_split_1___pvcn_975__dbase_byte_offset 	864	/* 0x00000360 */
#define flexidag_openseg_prim_split_1___pvcn_975__dpitchm1_byte_offset 	868	/* 0x00000364 */
#define flexidag_openseg_prim_split_1___pvcn_975__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1___pvcn_975__W 	3196
#define flexidag_openseg_prim_split_1___pvcn_975__H 	1
#define flexidag_openseg_prim_split_1___pvcn_975__D 	1
#define flexidag_openseg_prim_split_1___pvcn_975__P 	1
#define flexidag_openseg_prim_split_1___pvcn_975__data_num_bytes 	1
#define flexidag_openseg_prim_split_1___pvcn_975__denable 	1
#define flexidag_openseg_prim_split_1___pvcn_975__dpitch_num_bytes 	3200
#define flexidag_openseg_prim_split_1___pvcn_975__dram_format 	0
#define flexidag_openseg_prim_split_1___pvcn_975__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_1___pvcn_975__tile_width 	1
#define flexidag_openseg_prim_split_1___pvcn_975__tile_height 	1
#define flexidag_openseg_prim_split_1___pvcn_975__vmem_buffer_num_bytes 	3196
#define flexidag_openseg_prim_split_1___pvcn_975__dram_size_num_bytes 	3200

/* flexidag_openseg_prim_split_1 SMB_input __pvcn_976_ */
#define flexidag_openseg_prim_split_1___pvcn_976__cnngen_demangled_name 	"__pvcn_976_"
#define flexidag_openseg_prim_split_1___pvcn_976__is_constant 	1
#define flexidag_openseg_prim_split_1___pvcn_976__has_init_data 	1
#define flexidag_openseg_prim_split_1___pvcn_976__is_variable_scalar 	0
#define flexidag_openseg_prim_split_1___pvcn_976__is_bitvector 	0
#define flexidag_openseg_prim_split_1___pvcn_976__is_rlz 	0
#define flexidag_openseg_prim_split_1___pvcn_976__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_976__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_976__byte_offset 	876	/* 0x0000036c */
#define flexidag_openseg_prim_split_1___pvcn_976__vbase_byte_offset 	880	/* 0x00000370 */
#define flexidag_openseg_prim_split_1___pvcn_976__dbase_byte_offset 	904	/* 0x00000388 */
#define flexidag_openseg_prim_split_1___pvcn_976__dpitchm1_byte_offset 	908	/* 0x0000038c */
#define flexidag_openseg_prim_split_1___pvcn_976__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1___pvcn_976__W 	144
#define flexidag_openseg_prim_split_1___pvcn_976__H 	1
#define flexidag_openseg_prim_split_1___pvcn_976__D 	1
#define flexidag_openseg_prim_split_1___pvcn_976__P 	1
#define flexidag_openseg_prim_split_1___pvcn_976__data_num_bytes 	1
#define flexidag_openseg_prim_split_1___pvcn_976__denable 	1
#define flexidag_openseg_prim_split_1___pvcn_976__dpitch_num_bytes 	160
#define flexidag_openseg_prim_split_1___pvcn_976__dram_format 	0
#define flexidag_openseg_prim_split_1___pvcn_976__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_1___pvcn_976__tile_width 	1
#define flexidag_openseg_prim_split_1___pvcn_976__tile_height 	1
#define flexidag_openseg_prim_split_1___pvcn_976__vmem_buffer_num_bytes 	144
#define flexidag_openseg_prim_split_1___pvcn_976__dram_size_num_bytes 	160

/* flexidag_openseg_prim_split_1 SMB_input __pvcn_978_ */
#define flexidag_openseg_prim_split_1___pvcn_978__cnngen_demangled_name 	"__pvcn_978_"
#define flexidag_openseg_prim_split_1___pvcn_978__is_constant 	1
#define flexidag_openseg_prim_split_1___pvcn_978__has_init_data 	1
#define flexidag_openseg_prim_split_1___pvcn_978__is_variable_scalar 	0
#define flexidag_openseg_prim_split_1___pvcn_978__is_bitvector 	0
#define flexidag_openseg_prim_split_1___pvcn_978__is_rlz 	0
#define flexidag_openseg_prim_split_1___pvcn_978__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_978__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_978__byte_offset 	916	/* 0x00000394 */
#define flexidag_openseg_prim_split_1___pvcn_978__vbase_byte_offset 	920	/* 0x00000398 */
#define flexidag_openseg_prim_split_1___pvcn_978__dbase_byte_offset 	944	/* 0x000003b0 */
#define flexidag_openseg_prim_split_1___pvcn_978__dpitchm1_byte_offset 	948	/* 0x000003b4 */
#define flexidag_openseg_prim_split_1___pvcn_978__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1___pvcn_978__W 	1632
#define flexidag_openseg_prim_split_1___pvcn_978__H 	1
#define flexidag_openseg_prim_split_1___pvcn_978__D 	1
#define flexidag_openseg_prim_split_1___pvcn_978__P 	1
#define flexidag_openseg_prim_split_1___pvcn_978__data_num_bytes 	1
#define flexidag_openseg_prim_split_1___pvcn_978__denable 	1
#define flexidag_openseg_prim_split_1___pvcn_978__dpitch_num_bytes 	1632
#define flexidag_openseg_prim_split_1___pvcn_978__dram_format 	0
#define flexidag_openseg_prim_split_1___pvcn_978__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_1___pvcn_978__tile_width 	1
#define flexidag_openseg_prim_split_1___pvcn_978__tile_height 	1
#define flexidag_openseg_prim_split_1___pvcn_978__vmem_buffer_num_bytes 	1632
#define flexidag_openseg_prim_split_1___pvcn_978__dram_size_num_bytes 	1632

/* flexidag_openseg_prim_split_1 SMB_input __pvcn_979_ */
#define flexidag_openseg_prim_split_1___pvcn_979__cnngen_demangled_name 	"__pvcn_979_"
#define flexidag_openseg_prim_split_1___pvcn_979__is_constant 	1
#define flexidag_openseg_prim_split_1___pvcn_979__has_init_data 	1
#define flexidag_openseg_prim_split_1___pvcn_979__is_variable_scalar 	0
#define flexidag_openseg_prim_split_1___pvcn_979__is_bitvector 	0
#define flexidag_openseg_prim_split_1___pvcn_979__is_rlz 	0
#define flexidag_openseg_prim_split_1___pvcn_979__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_979__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_979__byte_offset 	956	/* 0x000003bc */
#define flexidag_openseg_prim_split_1___pvcn_979__vbase_byte_offset 	960	/* 0x000003c0 */
#define flexidag_openseg_prim_split_1___pvcn_979__dbase_byte_offset 	984	/* 0x000003d8 */
#define flexidag_openseg_prim_split_1___pvcn_979__dpitchm1_byte_offset 	988	/* 0x000003dc */
#define flexidag_openseg_prim_split_1___pvcn_979__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1___pvcn_979__W 	144
#define flexidag_openseg_prim_split_1___pvcn_979__H 	1
#define flexidag_openseg_prim_split_1___pvcn_979__D 	1
#define flexidag_openseg_prim_split_1___pvcn_979__P 	1
#define flexidag_openseg_prim_split_1___pvcn_979__data_num_bytes 	1
#define flexidag_openseg_prim_split_1___pvcn_979__denable 	1
#define flexidag_openseg_prim_split_1___pvcn_979__dpitch_num_bytes 	160
#define flexidag_openseg_prim_split_1___pvcn_979__dram_format 	0
#define flexidag_openseg_prim_split_1___pvcn_979__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_1___pvcn_979__tile_width 	1
#define flexidag_openseg_prim_split_1___pvcn_979__tile_height 	1
#define flexidag_openseg_prim_split_1___pvcn_979__vmem_buffer_num_bytes 	144
#define flexidag_openseg_prim_split_1___pvcn_979__dram_size_num_bytes 	160

/* flexidag_openseg_prim_split_1 SMB_input __pvcn_982_ */
#define flexidag_openseg_prim_split_1___pvcn_982__cnngen_demangled_name 	"__pvcn_982_"
#define flexidag_openseg_prim_split_1___pvcn_982__is_constant 	1
#define flexidag_openseg_prim_split_1___pvcn_982__has_init_data 	1
#define flexidag_openseg_prim_split_1___pvcn_982__is_variable_scalar 	0
#define flexidag_openseg_prim_split_1___pvcn_982__is_bitvector 	0
#define flexidag_openseg_prim_split_1___pvcn_982__is_rlz 	0
#define flexidag_openseg_prim_split_1___pvcn_982__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_982__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_982__byte_offset 	996	/* 0x000003e4 */
#define flexidag_openseg_prim_split_1___pvcn_982__vbase_byte_offset 	1000	/* 0x000003e8 */
#define flexidag_openseg_prim_split_1___pvcn_982__dbase_byte_offset 	1024	/* 0x00000400 */
#define flexidag_openseg_prim_split_1___pvcn_982__dpitchm1_byte_offset 	1028	/* 0x00000404 */
#define flexidag_openseg_prim_split_1___pvcn_982__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1___pvcn_982__W 	3236
#define flexidag_openseg_prim_split_1___pvcn_982__H 	1
#define flexidag_openseg_prim_split_1___pvcn_982__D 	1
#define flexidag_openseg_prim_split_1___pvcn_982__P 	1
#define flexidag_openseg_prim_split_1___pvcn_982__data_num_bytes 	1
#define flexidag_openseg_prim_split_1___pvcn_982__denable 	1
#define flexidag_openseg_prim_split_1___pvcn_982__dpitch_num_bytes 	3264
#define flexidag_openseg_prim_split_1___pvcn_982__dram_format 	0
#define flexidag_openseg_prim_split_1___pvcn_982__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_1___pvcn_982__tile_width 	1
#define flexidag_openseg_prim_split_1___pvcn_982__tile_height 	1
#define flexidag_openseg_prim_split_1___pvcn_982__vmem_buffer_num_bytes 	3236
#define flexidag_openseg_prim_split_1___pvcn_982__dram_size_num_bytes 	3264

/* flexidag_openseg_prim_split_1 SMB_input __pvcn_983_ */
#define flexidag_openseg_prim_split_1___pvcn_983__cnngen_demangled_name 	"__pvcn_983_"
#define flexidag_openseg_prim_split_1___pvcn_983__is_constant 	1
#define flexidag_openseg_prim_split_1___pvcn_983__has_init_data 	1
#define flexidag_openseg_prim_split_1___pvcn_983__is_variable_scalar 	0
#define flexidag_openseg_prim_split_1___pvcn_983__is_bitvector 	0
#define flexidag_openseg_prim_split_1___pvcn_983__is_rlz 	0
#define flexidag_openseg_prim_split_1___pvcn_983__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_983__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_983__byte_offset 	1036	/* 0x0000040c */
#define flexidag_openseg_prim_split_1___pvcn_983__vbase_byte_offset 	1040	/* 0x00000410 */
#define flexidag_openseg_prim_split_1___pvcn_983__dbase_byte_offset 	1064	/* 0x00000428 */
#define flexidag_openseg_prim_split_1___pvcn_983__dpitchm1_byte_offset 	1068	/* 0x0000042c */
#define flexidag_openseg_prim_split_1___pvcn_983__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1___pvcn_983__W 	144
#define flexidag_openseg_prim_split_1___pvcn_983__H 	1
#define flexidag_openseg_prim_split_1___pvcn_983__D 	1
#define flexidag_openseg_prim_split_1___pvcn_983__P 	1
#define flexidag_openseg_prim_split_1___pvcn_983__data_num_bytes 	1
#define flexidag_openseg_prim_split_1___pvcn_983__denable 	1
#define flexidag_openseg_prim_split_1___pvcn_983__dpitch_num_bytes 	160
#define flexidag_openseg_prim_split_1___pvcn_983__dram_format 	0
#define flexidag_openseg_prim_split_1___pvcn_983__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_1___pvcn_983__tile_width 	1
#define flexidag_openseg_prim_split_1___pvcn_983__tile_height 	1
#define flexidag_openseg_prim_split_1___pvcn_983__vmem_buffer_num_bytes 	144
#define flexidag_openseg_prim_split_1___pvcn_983__dram_size_num_bytes 	160

/* flexidag_openseg_prim_split_1 SMB_input __pvcn_991_ */
#define flexidag_openseg_prim_split_1___pvcn_991__cnngen_demangled_name 	"__pvcn_991_"
#define flexidag_openseg_prim_split_1___pvcn_991__is_constant 	1
#define flexidag_openseg_prim_split_1___pvcn_991__has_init_data 	1
#define flexidag_openseg_prim_split_1___pvcn_991__is_variable_scalar 	0
#define flexidag_openseg_prim_split_1___pvcn_991__is_bitvector 	0
#define flexidag_openseg_prim_split_1___pvcn_991__is_rlz 	0
#define flexidag_openseg_prim_split_1___pvcn_991__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_991__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_991__byte_offset 	1088	/* 0x00000440 */
#define flexidag_openseg_prim_split_1___pvcn_991__vbase_byte_offset 	1092	/* 0x00000444 */
#define flexidag_openseg_prim_split_1___pvcn_991__dbase_byte_offset 	1116	/* 0x0000045c */
#define flexidag_openseg_prim_split_1___pvcn_991__dpitchm1_byte_offset 	1120	/* 0x00000460 */
#define flexidag_openseg_prim_split_1___pvcn_991__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1___pvcn_991__W 	10056
#define flexidag_openseg_prim_split_1___pvcn_991__H 	1
#define flexidag_openseg_prim_split_1___pvcn_991__D 	1
#define flexidag_openseg_prim_split_1___pvcn_991__P 	1
#define flexidag_openseg_prim_split_1___pvcn_991__data_num_bytes 	1
#define flexidag_openseg_prim_split_1___pvcn_991__denable 	1
#define flexidag_openseg_prim_split_1___pvcn_991__dpitch_num_bytes 	10080
#define flexidag_openseg_prim_split_1___pvcn_991__dram_format 	0
#define flexidag_openseg_prim_split_1___pvcn_991__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_1___pvcn_991__tile_width 	1
#define flexidag_openseg_prim_split_1___pvcn_991__tile_height 	1
#define flexidag_openseg_prim_split_1___pvcn_991__vmem_buffer_num_bytes 	10056
#define flexidag_openseg_prim_split_1___pvcn_991__dram_size_num_bytes 	10080

/* flexidag_openseg_prim_split_1 SMB_input __pvcn_992_ */
#define flexidag_openseg_prim_split_1___pvcn_992__cnngen_demangled_name 	"__pvcn_992_"
#define flexidag_openseg_prim_split_1___pvcn_992__is_constant 	1
#define flexidag_openseg_prim_split_1___pvcn_992__has_init_data 	1
#define flexidag_openseg_prim_split_1___pvcn_992__is_variable_scalar 	0
#define flexidag_openseg_prim_split_1___pvcn_992__is_bitvector 	0
#define flexidag_openseg_prim_split_1___pvcn_992__is_rlz 	0
#define flexidag_openseg_prim_split_1___pvcn_992__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_992__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_992__byte_offset 	1128	/* 0x00000468 */
#define flexidag_openseg_prim_split_1___pvcn_992__vbase_byte_offset 	1132	/* 0x0000046c */
#define flexidag_openseg_prim_split_1___pvcn_992__dbase_byte_offset 	1156	/* 0x00000484 */
#define flexidag_openseg_prim_split_1___pvcn_992__dpitchm1_byte_offset 	1160	/* 0x00000488 */
#define flexidag_openseg_prim_split_1___pvcn_992__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1___pvcn_992__W 	544
#define flexidag_openseg_prim_split_1___pvcn_992__H 	1
#define flexidag_openseg_prim_split_1___pvcn_992__D 	1
#define flexidag_openseg_prim_split_1___pvcn_992__P 	1
#define flexidag_openseg_prim_split_1___pvcn_992__data_num_bytes 	1
#define flexidag_openseg_prim_split_1___pvcn_992__denable 	1
#define flexidag_openseg_prim_split_1___pvcn_992__dpitch_num_bytes 	544
#define flexidag_openseg_prim_split_1___pvcn_992__dram_format 	0
#define flexidag_openseg_prim_split_1___pvcn_992__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_1___pvcn_992__tile_width 	1
#define flexidag_openseg_prim_split_1___pvcn_992__tile_height 	1
#define flexidag_openseg_prim_split_1___pvcn_992__vmem_buffer_num_bytes 	544
#define flexidag_openseg_prim_split_1___pvcn_992__dram_size_num_bytes 	544

/* flexidag_openseg_prim_split_1 SMB_input __pvcn_995_ */
#define flexidag_openseg_prim_split_1___pvcn_995__cnngen_demangled_name 	"__pvcn_995_"
#define flexidag_openseg_prim_split_1___pvcn_995__is_constant 	1
#define flexidag_openseg_prim_split_1___pvcn_995__has_init_data 	1
#define flexidag_openseg_prim_split_1___pvcn_995__is_variable_scalar 	0
#define flexidag_openseg_prim_split_1___pvcn_995__is_bitvector 	0
#define flexidag_openseg_prim_split_1___pvcn_995__is_rlz 	0
#define flexidag_openseg_prim_split_1___pvcn_995__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_995__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_995__byte_offset 	1168	/* 0x00000490 */
#define flexidag_openseg_prim_split_1___pvcn_995__vbase_byte_offset 	1172	/* 0x00000494 */
#define flexidag_openseg_prim_split_1___pvcn_995__dbase_byte_offset 	1196	/* 0x000004ac */
#define flexidag_openseg_prim_split_1___pvcn_995__dpitchm1_byte_offset 	1200	/* 0x000004b0 */
#define flexidag_openseg_prim_split_1___pvcn_995__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1___pvcn_995__W 	3252
#define flexidag_openseg_prim_split_1___pvcn_995__H 	1
#define flexidag_openseg_prim_split_1___pvcn_995__D 	1
#define flexidag_openseg_prim_split_1___pvcn_995__P 	1
#define flexidag_openseg_prim_split_1___pvcn_995__data_num_bytes 	1
#define flexidag_openseg_prim_split_1___pvcn_995__denable 	1
#define flexidag_openseg_prim_split_1___pvcn_995__dpitch_num_bytes 	3264
#define flexidag_openseg_prim_split_1___pvcn_995__dram_format 	0
#define flexidag_openseg_prim_split_1___pvcn_995__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_1___pvcn_995__tile_width 	1
#define flexidag_openseg_prim_split_1___pvcn_995__tile_height 	1
#define flexidag_openseg_prim_split_1___pvcn_995__vmem_buffer_num_bytes 	3252
#define flexidag_openseg_prim_split_1___pvcn_995__dram_size_num_bytes 	3264

/* flexidag_openseg_prim_split_1 SMB_input __pvcn_996_ */
#define flexidag_openseg_prim_split_1___pvcn_996__cnngen_demangled_name 	"__pvcn_996_"
#define flexidag_openseg_prim_split_1___pvcn_996__is_constant 	1
#define flexidag_openseg_prim_split_1___pvcn_996__has_init_data 	1
#define flexidag_openseg_prim_split_1___pvcn_996__is_variable_scalar 	0
#define flexidag_openseg_prim_split_1___pvcn_996__is_bitvector 	0
#define flexidag_openseg_prim_split_1___pvcn_996__is_rlz 	0
#define flexidag_openseg_prim_split_1___pvcn_996__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_996__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_996__byte_offset 	1208	/* 0x000004b8 */
#define flexidag_openseg_prim_split_1___pvcn_996__vbase_byte_offset 	1212	/* 0x000004bc */
#define flexidag_openseg_prim_split_1___pvcn_996__dbase_byte_offset 	1236	/* 0x000004d4 */
#define flexidag_openseg_prim_split_1___pvcn_996__dpitchm1_byte_offset 	1240	/* 0x000004d8 */
#define flexidag_openseg_prim_split_1___pvcn_996__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1___pvcn_996__W 	144
#define flexidag_openseg_prim_split_1___pvcn_996__H 	1
#define flexidag_openseg_prim_split_1___pvcn_996__D 	1
#define flexidag_openseg_prim_split_1___pvcn_996__P 	1
#define flexidag_openseg_prim_split_1___pvcn_996__data_num_bytes 	1
#define flexidag_openseg_prim_split_1___pvcn_996__denable 	1
#define flexidag_openseg_prim_split_1___pvcn_996__dpitch_num_bytes 	160
#define flexidag_openseg_prim_split_1___pvcn_996__dram_format 	0
#define flexidag_openseg_prim_split_1___pvcn_996__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_1___pvcn_996__tile_width 	1
#define flexidag_openseg_prim_split_1___pvcn_996__tile_height 	1
#define flexidag_openseg_prim_split_1___pvcn_996__vmem_buffer_num_bytes 	144
#define flexidag_openseg_prim_split_1___pvcn_996__dram_size_num_bytes 	160

/* flexidag_openseg_prim_split_1 SMB_input __pvcn_998_ */
#define flexidag_openseg_prim_split_1___pvcn_998__cnngen_demangled_name 	"__pvcn_998_"
#define flexidag_openseg_prim_split_1___pvcn_998__is_constant 	1
#define flexidag_openseg_prim_split_1___pvcn_998__has_init_data 	1
#define flexidag_openseg_prim_split_1___pvcn_998__is_variable_scalar 	0
#define flexidag_openseg_prim_split_1___pvcn_998__is_bitvector 	0
#define flexidag_openseg_prim_split_1___pvcn_998__is_rlz 	0
#define flexidag_openseg_prim_split_1___pvcn_998__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_998__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_998__byte_offset 	1248	/* 0x000004e0 */
#define flexidag_openseg_prim_split_1___pvcn_998__vbase_byte_offset 	1252	/* 0x000004e4 */
#define flexidag_openseg_prim_split_1___pvcn_998__dbase_byte_offset 	1276	/* 0x000004fc */
#define flexidag_openseg_prim_split_1___pvcn_998__dpitchm1_byte_offset 	1280	/* 0x00000500 */
#define flexidag_openseg_prim_split_1___pvcn_998__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1___pvcn_998__W 	1596
#define flexidag_openseg_prim_split_1___pvcn_998__H 	1
#define flexidag_openseg_prim_split_1___pvcn_998__D 	1
#define flexidag_openseg_prim_split_1___pvcn_998__P 	1
#define flexidag_openseg_prim_split_1___pvcn_998__data_num_bytes 	1
#define flexidag_openseg_prim_split_1___pvcn_998__denable 	1
#define flexidag_openseg_prim_split_1___pvcn_998__dpitch_num_bytes 	1600
#define flexidag_openseg_prim_split_1___pvcn_998__dram_format 	0
#define flexidag_openseg_prim_split_1___pvcn_998__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_1___pvcn_998__tile_width 	1
#define flexidag_openseg_prim_split_1___pvcn_998__tile_height 	1
#define flexidag_openseg_prim_split_1___pvcn_998__vmem_buffer_num_bytes 	1596
#define flexidag_openseg_prim_split_1___pvcn_998__dram_size_num_bytes 	1600

/* flexidag_openseg_prim_split_1 SMB_input __pvcn_999_ */
#define flexidag_openseg_prim_split_1___pvcn_999__cnngen_demangled_name 	"__pvcn_999_"
#define flexidag_openseg_prim_split_1___pvcn_999__is_constant 	1
#define flexidag_openseg_prim_split_1___pvcn_999__has_init_data 	1
#define flexidag_openseg_prim_split_1___pvcn_999__is_variable_scalar 	0
#define flexidag_openseg_prim_split_1___pvcn_999__is_bitvector 	0
#define flexidag_openseg_prim_split_1___pvcn_999__is_rlz 	0
#define flexidag_openseg_prim_split_1___pvcn_999__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_999__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_999__byte_offset 	1288	/* 0x00000508 */
#define flexidag_openseg_prim_split_1___pvcn_999__vbase_byte_offset 	1292	/* 0x0000050c */
#define flexidag_openseg_prim_split_1___pvcn_999__dbase_byte_offset 	1316	/* 0x00000524 */
#define flexidag_openseg_prim_split_1___pvcn_999__dpitchm1_byte_offset 	1320	/* 0x00000528 */
#define flexidag_openseg_prim_split_1___pvcn_999__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1___pvcn_999__W 	144
#define flexidag_openseg_prim_split_1___pvcn_999__H 	1
#define flexidag_openseg_prim_split_1___pvcn_999__D 	1
#define flexidag_openseg_prim_split_1___pvcn_999__P 	1
#define flexidag_openseg_prim_split_1___pvcn_999__data_num_bytes 	1
#define flexidag_openseg_prim_split_1___pvcn_999__denable 	1
#define flexidag_openseg_prim_split_1___pvcn_999__dpitch_num_bytes 	160
#define flexidag_openseg_prim_split_1___pvcn_999__dram_format 	0
#define flexidag_openseg_prim_split_1___pvcn_999__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_1___pvcn_999__tile_width 	1
#define flexidag_openseg_prim_split_1___pvcn_999__tile_height 	1
#define flexidag_openseg_prim_split_1___pvcn_999__vmem_buffer_num_bytes 	144
#define flexidag_openseg_prim_split_1___pvcn_999__dram_size_num_bytes 	160

/* flexidag_openseg_prim_split_1 SMB_input __pvcn_1002_ */
#define flexidag_openseg_prim_split_1___pvcn_1002__cnngen_demangled_name 	"__pvcn_1002_"
#define flexidag_openseg_prim_split_1___pvcn_1002__is_constant 	1
#define flexidag_openseg_prim_split_1___pvcn_1002__has_init_data 	1
#define flexidag_openseg_prim_split_1___pvcn_1002__is_variable_scalar 	0
#define flexidag_openseg_prim_split_1___pvcn_1002__is_bitvector 	0
#define flexidag_openseg_prim_split_1___pvcn_1002__is_rlz 	0
#define flexidag_openseg_prim_split_1___pvcn_1002__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_1002__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_1002__byte_offset 	1328	/* 0x00000530 */
#define flexidag_openseg_prim_split_1___pvcn_1002__vbase_byte_offset 	1332	/* 0x00000534 */
#define flexidag_openseg_prim_split_1___pvcn_1002__dbase_byte_offset 	1356	/* 0x0000054c */
#define flexidag_openseg_prim_split_1___pvcn_1002__dpitchm1_byte_offset 	1360	/* 0x00000550 */
#define flexidag_openseg_prim_split_1___pvcn_1002__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1___pvcn_1002__W 	3284
#define flexidag_openseg_prim_split_1___pvcn_1002__H 	1
#define flexidag_openseg_prim_split_1___pvcn_1002__D 	1
#define flexidag_openseg_prim_split_1___pvcn_1002__P 	1
#define flexidag_openseg_prim_split_1___pvcn_1002__data_num_bytes 	1
#define flexidag_openseg_prim_split_1___pvcn_1002__denable 	1
#define flexidag_openseg_prim_split_1___pvcn_1002__dpitch_num_bytes 	3296
#define flexidag_openseg_prim_split_1___pvcn_1002__dram_format 	0
#define flexidag_openseg_prim_split_1___pvcn_1002__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_1___pvcn_1002__tile_width 	1
#define flexidag_openseg_prim_split_1___pvcn_1002__tile_height 	1
#define flexidag_openseg_prim_split_1___pvcn_1002__vmem_buffer_num_bytes 	3284
#define flexidag_openseg_prim_split_1___pvcn_1002__dram_size_num_bytes 	3296

/* flexidag_openseg_prim_split_1 SMB_input __pvcn_1003_ */
#define flexidag_openseg_prim_split_1___pvcn_1003__cnngen_demangled_name 	"__pvcn_1003_"
#define flexidag_openseg_prim_split_1___pvcn_1003__is_constant 	1
#define flexidag_openseg_prim_split_1___pvcn_1003__has_init_data 	1
#define flexidag_openseg_prim_split_1___pvcn_1003__is_variable_scalar 	0
#define flexidag_openseg_prim_split_1___pvcn_1003__is_bitvector 	0
#define flexidag_openseg_prim_split_1___pvcn_1003__is_rlz 	0
#define flexidag_openseg_prim_split_1___pvcn_1003__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_1003__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_1003__byte_offset 	1368	/* 0x00000558 */
#define flexidag_openseg_prim_split_1___pvcn_1003__vbase_byte_offset 	1372	/* 0x0000055c */
#define flexidag_openseg_prim_split_1___pvcn_1003__dbase_byte_offset 	1396	/* 0x00000574 */
#define flexidag_openseg_prim_split_1___pvcn_1003__dpitchm1_byte_offset 	1400	/* 0x00000578 */
#define flexidag_openseg_prim_split_1___pvcn_1003__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1___pvcn_1003__W 	136
#define flexidag_openseg_prim_split_1___pvcn_1003__H 	1
#define flexidag_openseg_prim_split_1___pvcn_1003__D 	1
#define flexidag_openseg_prim_split_1___pvcn_1003__P 	1
#define flexidag_openseg_prim_split_1___pvcn_1003__data_num_bytes 	1
#define flexidag_openseg_prim_split_1___pvcn_1003__denable 	1
#define flexidag_openseg_prim_split_1___pvcn_1003__dpitch_num_bytes 	160
#define flexidag_openseg_prim_split_1___pvcn_1003__dram_format 	0
#define flexidag_openseg_prim_split_1___pvcn_1003__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_1___pvcn_1003__tile_width 	1
#define flexidag_openseg_prim_split_1___pvcn_1003__tile_height 	1
#define flexidag_openseg_prim_split_1___pvcn_1003__vmem_buffer_num_bytes 	136
#define flexidag_openseg_prim_split_1___pvcn_1003__dram_size_num_bytes 	160

/* flexidag_openseg_prim_split_1 SMB_input __pvcn_110012_ */
#define flexidag_openseg_prim_split_1___pvcn_110012__cnngen_demangled_name 	"__pvcn_110012_"
#define flexidag_openseg_prim_split_1___pvcn_110012__is_constant 	1
#define flexidag_openseg_prim_split_1___pvcn_110012__has_init_data 	1
#define flexidag_openseg_prim_split_1___pvcn_110012__is_variable_scalar 	0
#define flexidag_openseg_prim_split_1___pvcn_110012__is_bitvector 	0
#define flexidag_openseg_prim_split_1___pvcn_110012__is_rlz 	0
#define flexidag_openseg_prim_split_1___pvcn_110012__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_110012__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_110012__byte_offset 	1408	/* 0x00000580 */
#define flexidag_openseg_prim_split_1___pvcn_110012__vbase_byte_offset 	1412	/* 0x00000584 */
#define flexidag_openseg_prim_split_1___pvcn_110012__dbase_byte_offset 	1436	/* 0x0000059c */
#define flexidag_openseg_prim_split_1___pvcn_110012__dpitchm1_byte_offset 	1440	/* 0x000005a0 */
#define flexidag_openseg_prim_split_1___pvcn_110012__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1___pvcn_110012__W 	23200
#define flexidag_openseg_prim_split_1___pvcn_110012__H 	1
#define flexidag_openseg_prim_split_1___pvcn_110012__D 	1
#define flexidag_openseg_prim_split_1___pvcn_110012__P 	1
#define flexidag_openseg_prim_split_1___pvcn_110012__data_num_bytes 	1
#define flexidag_openseg_prim_split_1___pvcn_110012__denable 	1
#define flexidag_openseg_prim_split_1___pvcn_110012__dpitch_num_bytes 	23200
#define flexidag_openseg_prim_split_1___pvcn_110012__dram_format 	0
#define flexidag_openseg_prim_split_1___pvcn_110012__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_1___pvcn_110012__tile_width 	1
#define flexidag_openseg_prim_split_1___pvcn_110012__tile_height 	1
#define flexidag_openseg_prim_split_1___pvcn_110012__vmem_buffer_num_bytes 	23200
#define flexidag_openseg_prim_split_1___pvcn_110012__dram_size_num_bytes 	23200

/* flexidag_openseg_prim_split_1 SMB_input __pvcn_110015_ */
#define flexidag_openseg_prim_split_1___pvcn_110015__cnngen_demangled_name 	"__pvcn_110015_"
#define flexidag_openseg_prim_split_1___pvcn_110015__is_constant 	1
#define flexidag_openseg_prim_split_1___pvcn_110015__has_init_data 	1
#define flexidag_openseg_prim_split_1___pvcn_110015__is_variable_scalar 	0
#define flexidag_openseg_prim_split_1___pvcn_110015__is_bitvector 	0
#define flexidag_openseg_prim_split_1___pvcn_110015__is_rlz 	0
#define flexidag_openseg_prim_split_1___pvcn_110015__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_110015__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_110015__byte_offset 	1448	/* 0x000005a8 */
#define flexidag_openseg_prim_split_1___pvcn_110015__vbase_byte_offset 	1452	/* 0x000005ac */
#define flexidag_openseg_prim_split_1___pvcn_110015__dbase_byte_offset 	1476	/* 0x000005c4 */
#define flexidag_openseg_prim_split_1___pvcn_110015__dpitchm1_byte_offset 	1480	/* 0x000005c8 */
#define flexidag_openseg_prim_split_1___pvcn_110015__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1___pvcn_110015__W 	23136
#define flexidag_openseg_prim_split_1___pvcn_110015__H 	1
#define flexidag_openseg_prim_split_1___pvcn_110015__D 	1
#define flexidag_openseg_prim_split_1___pvcn_110015__P 	1
#define flexidag_openseg_prim_split_1___pvcn_110015__data_num_bytes 	1
#define flexidag_openseg_prim_split_1___pvcn_110015__denable 	1
#define flexidag_openseg_prim_split_1___pvcn_110015__dpitch_num_bytes 	23136
#define flexidag_openseg_prim_split_1___pvcn_110015__dram_format 	0
#define flexidag_openseg_prim_split_1___pvcn_110015__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_1___pvcn_110015__tile_width 	1
#define flexidag_openseg_prim_split_1___pvcn_110015__tile_height 	1
#define flexidag_openseg_prim_split_1___pvcn_110015__vmem_buffer_num_bytes 	23136
#define flexidag_openseg_prim_split_1___pvcn_110015__dram_size_num_bytes 	23136

/* flexidag_openseg_prim_split_1 SMB_input __pvcn_110013_ */
#define flexidag_openseg_prim_split_1___pvcn_110013__cnngen_demangled_name 	"__pvcn_110013_"
#define flexidag_openseg_prim_split_1___pvcn_110013__is_constant 	1
#define flexidag_openseg_prim_split_1___pvcn_110013__has_init_data 	1
#define flexidag_openseg_prim_split_1___pvcn_110013__is_variable_scalar 	0
#define flexidag_openseg_prim_split_1___pvcn_110013__is_bitvector 	0
#define flexidag_openseg_prim_split_1___pvcn_110013__is_rlz 	0
#define flexidag_openseg_prim_split_1___pvcn_110013__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_110013__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_110013__byte_offset 	1488	/* 0x000005d0 */
#define flexidag_openseg_prim_split_1___pvcn_110013__vbase_byte_offset 	1492	/* 0x000005d4 */
#define flexidag_openseg_prim_split_1___pvcn_110013__dbase_byte_offset 	1516	/* 0x000005ec */
#define flexidag_openseg_prim_split_1___pvcn_110013__dpitchm1_byte_offset 	1520	/* 0x000005f0 */
#define flexidag_openseg_prim_split_1___pvcn_110013__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1___pvcn_110013__W 	1088
#define flexidag_openseg_prim_split_1___pvcn_110013__H 	1
#define flexidag_openseg_prim_split_1___pvcn_110013__D 	1
#define flexidag_openseg_prim_split_1___pvcn_110013__P 	1
#define flexidag_openseg_prim_split_1___pvcn_110013__data_num_bytes 	1
#define flexidag_openseg_prim_split_1___pvcn_110013__denable 	1
#define flexidag_openseg_prim_split_1___pvcn_110013__dpitch_num_bytes 	1088
#define flexidag_openseg_prim_split_1___pvcn_110013__dram_format 	0
#define flexidag_openseg_prim_split_1___pvcn_110013__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_1___pvcn_110013__tile_width 	1
#define flexidag_openseg_prim_split_1___pvcn_110013__tile_height 	1
#define flexidag_openseg_prim_split_1___pvcn_110013__vmem_buffer_num_bytes 	1088
#define flexidag_openseg_prim_split_1___pvcn_110013__dram_size_num_bytes 	1088

/* flexidag_openseg_prim_split_1 SMB_input __pvcn_110016_ */
#define flexidag_openseg_prim_split_1___pvcn_110016__cnngen_demangled_name 	"__pvcn_110016_"
#define flexidag_openseg_prim_split_1___pvcn_110016__is_constant 	1
#define flexidag_openseg_prim_split_1___pvcn_110016__has_init_data 	1
#define flexidag_openseg_prim_split_1___pvcn_110016__is_variable_scalar 	0
#define flexidag_openseg_prim_split_1___pvcn_110016__is_bitvector 	0
#define flexidag_openseg_prim_split_1___pvcn_110016__is_rlz 	0
#define flexidag_openseg_prim_split_1___pvcn_110016__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_110016__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_110016__byte_offset 	1528	/* 0x000005f8 */
#define flexidag_openseg_prim_split_1___pvcn_110016__vbase_byte_offset 	1532	/* 0x000005fc */
#define flexidag_openseg_prim_split_1___pvcn_110016__dbase_byte_offset 	1556	/* 0x00000614 */
#define flexidag_openseg_prim_split_1___pvcn_110016__dpitchm1_byte_offset 	1560	/* 0x00000618 */
#define flexidag_openseg_prim_split_1___pvcn_110016__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1___pvcn_110016__W 	1088
#define flexidag_openseg_prim_split_1___pvcn_110016__H 	1
#define flexidag_openseg_prim_split_1___pvcn_110016__D 	1
#define flexidag_openseg_prim_split_1___pvcn_110016__P 	1
#define flexidag_openseg_prim_split_1___pvcn_110016__data_num_bytes 	1
#define flexidag_openseg_prim_split_1___pvcn_110016__denable 	1
#define flexidag_openseg_prim_split_1___pvcn_110016__dpitch_num_bytes 	1088
#define flexidag_openseg_prim_split_1___pvcn_110016__dram_format 	0
#define flexidag_openseg_prim_split_1___pvcn_110016__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_1___pvcn_110016__tile_width 	1
#define flexidag_openseg_prim_split_1___pvcn_110016__tile_height 	1
#define flexidag_openseg_prim_split_1___pvcn_110016__vmem_buffer_num_bytes 	1088
#define flexidag_openseg_prim_split_1___pvcn_110016__dram_size_num_bytes 	1088

/* flexidag_openseg_prim_split_1 SMB_input __pvcn_1028_ */
#define flexidag_openseg_prim_split_1___pvcn_1028__cnngen_demangled_name 	"__pvcn_1028_"
#define flexidag_openseg_prim_split_1___pvcn_1028__is_constant 	1
#define flexidag_openseg_prim_split_1___pvcn_1028__has_init_data 	1
#define flexidag_openseg_prim_split_1___pvcn_1028__is_variable_scalar 	0
#define flexidag_openseg_prim_split_1___pvcn_1028__is_bitvector 	0
#define flexidag_openseg_prim_split_1___pvcn_1028__is_rlz 	0
#define flexidag_openseg_prim_split_1___pvcn_1028__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_1028__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_1028__byte_offset 	1568	/* 0x00000620 */
#define flexidag_openseg_prim_split_1___pvcn_1028__vbase_byte_offset 	1572	/* 0x00000624 */
#define flexidag_openseg_prim_split_1___pvcn_1028__dbase_byte_offset 	1596	/* 0x0000063c */
#define flexidag_openseg_prim_split_1___pvcn_1028__dpitchm1_byte_offset 	1600	/* 0x00000640 */
#define flexidag_openseg_prim_split_1___pvcn_1028__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1___pvcn_1028__W 	240
#define flexidag_openseg_prim_split_1___pvcn_1028__H 	1
#define flexidag_openseg_prim_split_1___pvcn_1028__D 	1
#define flexidag_openseg_prim_split_1___pvcn_1028__P 	1
#define flexidag_openseg_prim_split_1___pvcn_1028__data_num_bytes 	1
#define flexidag_openseg_prim_split_1___pvcn_1028__denable 	1
#define flexidag_openseg_prim_split_1___pvcn_1028__dpitch_num_bytes 	256
#define flexidag_openseg_prim_split_1___pvcn_1028__dram_format 	0
#define flexidag_openseg_prim_split_1___pvcn_1028__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_1___pvcn_1028__tile_width 	1
#define flexidag_openseg_prim_split_1___pvcn_1028__tile_height 	1
#define flexidag_openseg_prim_split_1___pvcn_1028__vmem_buffer_num_bytes 	240
#define flexidag_openseg_prim_split_1___pvcn_1028__dram_size_num_bytes 	256

/* flexidag_openseg_prim_split_1 SMB_input __pvcn_1029_ */
#define flexidag_openseg_prim_split_1___pvcn_1029__cnngen_demangled_name 	"__pvcn_1029_"
#define flexidag_openseg_prim_split_1___pvcn_1029__is_constant 	1
#define flexidag_openseg_prim_split_1___pvcn_1029__has_init_data 	1
#define flexidag_openseg_prim_split_1___pvcn_1029__is_variable_scalar 	0
#define flexidag_openseg_prim_split_1___pvcn_1029__is_bitvector 	0
#define flexidag_openseg_prim_split_1___pvcn_1029__is_rlz 	0
#define flexidag_openseg_prim_split_1___pvcn_1029__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_1029__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_1029__byte_offset 	1608	/* 0x00000648 */
#define flexidag_openseg_prim_split_1___pvcn_1029__vbase_byte_offset 	1612	/* 0x0000064c */
#define flexidag_openseg_prim_split_1___pvcn_1029__dbase_byte_offset 	1636	/* 0x00000664 */
#define flexidag_openseg_prim_split_1___pvcn_1029__dpitchm1_byte_offset 	1640	/* 0x00000668 */
#define flexidag_openseg_prim_split_1___pvcn_1029__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1___pvcn_1029__W 	184
#define flexidag_openseg_prim_split_1___pvcn_1029__H 	1
#define flexidag_openseg_prim_split_1___pvcn_1029__D 	1
#define flexidag_openseg_prim_split_1___pvcn_1029__P 	1
#define flexidag_openseg_prim_split_1___pvcn_1029__data_num_bytes 	1
#define flexidag_openseg_prim_split_1___pvcn_1029__denable 	1
#define flexidag_openseg_prim_split_1___pvcn_1029__dpitch_num_bytes 	192
#define flexidag_openseg_prim_split_1___pvcn_1029__dram_format 	0
#define flexidag_openseg_prim_split_1___pvcn_1029__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_1___pvcn_1029__tile_width 	1
#define flexidag_openseg_prim_split_1___pvcn_1029__tile_height 	1
#define flexidag_openseg_prim_split_1___pvcn_1029__vmem_buffer_num_bytes 	184
#define flexidag_openseg_prim_split_1___pvcn_1029__dram_size_num_bytes 	192

/* flexidag_openseg_prim_split_1 SMB_input __pvcn_1035_ */
#define flexidag_openseg_prim_split_1___pvcn_1035__cnngen_demangled_name 	"__pvcn_1035_"
#define flexidag_openseg_prim_split_1___pvcn_1035__is_constant 	1
#define flexidag_openseg_prim_split_1___pvcn_1035__has_init_data 	1
#define flexidag_openseg_prim_split_1___pvcn_1035__is_variable_scalar 	0
#define flexidag_openseg_prim_split_1___pvcn_1035__is_bitvector 	0
#define flexidag_openseg_prim_split_1___pvcn_1035__is_rlz 	0
#define flexidag_openseg_prim_split_1___pvcn_1035__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_1035__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_1035__byte_offset 	1648	/* 0x00000670 */
#define flexidag_openseg_prim_split_1___pvcn_1035__vbase_byte_offset 	1652	/* 0x00000674 */
#define flexidag_openseg_prim_split_1___pvcn_1035__dbase_byte_offset 	1676	/* 0x0000068c */
#define flexidag_openseg_prim_split_1___pvcn_1035__dpitchm1_byte_offset 	1680	/* 0x00000690 */
#define flexidag_openseg_prim_split_1___pvcn_1035__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1___pvcn_1035__W 	240
#define flexidag_openseg_prim_split_1___pvcn_1035__H 	1
#define flexidag_openseg_prim_split_1___pvcn_1035__D 	1
#define flexidag_openseg_prim_split_1___pvcn_1035__P 	1
#define flexidag_openseg_prim_split_1___pvcn_1035__data_num_bytes 	1
#define flexidag_openseg_prim_split_1___pvcn_1035__denable 	1
#define flexidag_openseg_prim_split_1___pvcn_1035__dpitch_num_bytes 	256
#define flexidag_openseg_prim_split_1___pvcn_1035__dram_format 	0
#define flexidag_openseg_prim_split_1___pvcn_1035__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_1___pvcn_1035__tile_width 	1
#define flexidag_openseg_prim_split_1___pvcn_1035__tile_height 	1
#define flexidag_openseg_prim_split_1___pvcn_1035__vmem_buffer_num_bytes 	240
#define flexidag_openseg_prim_split_1___pvcn_1035__dram_size_num_bytes 	256

/* flexidag_openseg_prim_split_1 SMB_input __pvcn_1036_ */
#define flexidag_openseg_prim_split_1___pvcn_1036__cnngen_demangled_name 	"__pvcn_1036_"
#define flexidag_openseg_prim_split_1___pvcn_1036__is_constant 	1
#define flexidag_openseg_prim_split_1___pvcn_1036__has_init_data 	1
#define flexidag_openseg_prim_split_1___pvcn_1036__is_variable_scalar 	0
#define flexidag_openseg_prim_split_1___pvcn_1036__is_bitvector 	0
#define flexidag_openseg_prim_split_1___pvcn_1036__is_rlz 	0
#define flexidag_openseg_prim_split_1___pvcn_1036__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_1036__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_1036__byte_offset 	1688	/* 0x00000698 */
#define flexidag_openseg_prim_split_1___pvcn_1036__vbase_byte_offset 	1692	/* 0x0000069c */
#define flexidag_openseg_prim_split_1___pvcn_1036__dbase_byte_offset 	1716	/* 0x000006b4 */
#define flexidag_openseg_prim_split_1___pvcn_1036__dpitchm1_byte_offset 	1720	/* 0x000006b8 */
#define flexidag_openseg_prim_split_1___pvcn_1036__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1___pvcn_1036__W 	184
#define flexidag_openseg_prim_split_1___pvcn_1036__H 	1
#define flexidag_openseg_prim_split_1___pvcn_1036__D 	1
#define flexidag_openseg_prim_split_1___pvcn_1036__P 	1
#define flexidag_openseg_prim_split_1___pvcn_1036__data_num_bytes 	1
#define flexidag_openseg_prim_split_1___pvcn_1036__denable 	1
#define flexidag_openseg_prim_split_1___pvcn_1036__dpitch_num_bytes 	192
#define flexidag_openseg_prim_split_1___pvcn_1036__dram_format 	0
#define flexidag_openseg_prim_split_1___pvcn_1036__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_1___pvcn_1036__tile_width 	1
#define flexidag_openseg_prim_split_1___pvcn_1036__tile_height 	1
#define flexidag_openseg_prim_split_1___pvcn_1036__vmem_buffer_num_bytes 	184
#define flexidag_openseg_prim_split_1___pvcn_1036__dram_size_num_bytes 	192

/* flexidag_openseg_prim_split_1 SMB_input __pvcn_1042_ */
#define flexidag_openseg_prim_split_1___pvcn_1042__cnngen_demangled_name 	"__pvcn_1042_"
#define flexidag_openseg_prim_split_1___pvcn_1042__is_constant 	1
#define flexidag_openseg_prim_split_1___pvcn_1042__has_init_data 	1
#define flexidag_openseg_prim_split_1___pvcn_1042__is_variable_scalar 	0
#define flexidag_openseg_prim_split_1___pvcn_1042__is_bitvector 	0
#define flexidag_openseg_prim_split_1___pvcn_1042__is_rlz 	0
#define flexidag_openseg_prim_split_1___pvcn_1042__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_1042__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_1042__byte_offset 	1728	/* 0x000006c0 */
#define flexidag_openseg_prim_split_1___pvcn_1042__vbase_byte_offset 	1732	/* 0x000006c4 */
#define flexidag_openseg_prim_split_1___pvcn_1042__dbase_byte_offset 	1756	/* 0x000006dc */
#define flexidag_openseg_prim_split_1___pvcn_1042__dpitchm1_byte_offset 	1760	/* 0x000006e0 */
#define flexidag_openseg_prim_split_1___pvcn_1042__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1___pvcn_1042__W 	240
#define flexidag_openseg_prim_split_1___pvcn_1042__H 	1
#define flexidag_openseg_prim_split_1___pvcn_1042__D 	1
#define flexidag_openseg_prim_split_1___pvcn_1042__P 	1
#define flexidag_openseg_prim_split_1___pvcn_1042__data_num_bytes 	1
#define flexidag_openseg_prim_split_1___pvcn_1042__denable 	1
#define flexidag_openseg_prim_split_1___pvcn_1042__dpitch_num_bytes 	256
#define flexidag_openseg_prim_split_1___pvcn_1042__dram_format 	0
#define flexidag_openseg_prim_split_1___pvcn_1042__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_1___pvcn_1042__tile_width 	1
#define flexidag_openseg_prim_split_1___pvcn_1042__tile_height 	1
#define flexidag_openseg_prim_split_1___pvcn_1042__vmem_buffer_num_bytes 	240
#define flexidag_openseg_prim_split_1___pvcn_1042__dram_size_num_bytes 	256

/* flexidag_openseg_prim_split_1 SMB_input __pvcn_1043_ */
#define flexidag_openseg_prim_split_1___pvcn_1043__cnngen_demangled_name 	"__pvcn_1043_"
#define flexidag_openseg_prim_split_1___pvcn_1043__is_constant 	1
#define flexidag_openseg_prim_split_1___pvcn_1043__has_init_data 	1
#define flexidag_openseg_prim_split_1___pvcn_1043__is_variable_scalar 	0
#define flexidag_openseg_prim_split_1___pvcn_1043__is_bitvector 	0
#define flexidag_openseg_prim_split_1___pvcn_1043__is_rlz 	0
#define flexidag_openseg_prim_split_1___pvcn_1043__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_1043__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_1043__byte_offset 	1768	/* 0x000006e8 */
#define flexidag_openseg_prim_split_1___pvcn_1043__vbase_byte_offset 	1772	/* 0x000006ec */
#define flexidag_openseg_prim_split_1___pvcn_1043__dbase_byte_offset 	1796	/* 0x00000704 */
#define flexidag_openseg_prim_split_1___pvcn_1043__dpitchm1_byte_offset 	1800	/* 0x00000708 */
#define flexidag_openseg_prim_split_1___pvcn_1043__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1___pvcn_1043__W 	184
#define flexidag_openseg_prim_split_1___pvcn_1043__H 	1
#define flexidag_openseg_prim_split_1___pvcn_1043__D 	1
#define flexidag_openseg_prim_split_1___pvcn_1043__P 	1
#define flexidag_openseg_prim_split_1___pvcn_1043__data_num_bytes 	1
#define flexidag_openseg_prim_split_1___pvcn_1043__denable 	1
#define flexidag_openseg_prim_split_1___pvcn_1043__dpitch_num_bytes 	192
#define flexidag_openseg_prim_split_1___pvcn_1043__dram_format 	0
#define flexidag_openseg_prim_split_1___pvcn_1043__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_1___pvcn_1043__tile_width 	1
#define flexidag_openseg_prim_split_1___pvcn_1043__tile_height 	1
#define flexidag_openseg_prim_split_1___pvcn_1043__vmem_buffer_num_bytes 	184
#define flexidag_openseg_prim_split_1___pvcn_1043__dram_size_num_bytes 	192

/* flexidag_openseg_prim_split_1 SMB_input __pvcn_1049_ */
#define flexidag_openseg_prim_split_1___pvcn_1049__cnngen_demangled_name 	"__pvcn_1049_"
#define flexidag_openseg_prim_split_1___pvcn_1049__is_constant 	1
#define flexidag_openseg_prim_split_1___pvcn_1049__has_init_data 	1
#define flexidag_openseg_prim_split_1___pvcn_1049__is_variable_scalar 	0
#define flexidag_openseg_prim_split_1___pvcn_1049__is_bitvector 	0
#define flexidag_openseg_prim_split_1___pvcn_1049__is_rlz 	0
#define flexidag_openseg_prim_split_1___pvcn_1049__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_1049__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_1049__byte_offset 	1808	/* 0x00000710 */
#define flexidag_openseg_prim_split_1___pvcn_1049__vbase_byte_offset 	1812	/* 0x00000714 */
#define flexidag_openseg_prim_split_1___pvcn_1049__dbase_byte_offset 	1836	/* 0x0000072c */
#define flexidag_openseg_prim_split_1___pvcn_1049__dpitchm1_byte_offset 	1840	/* 0x00000730 */
#define flexidag_openseg_prim_split_1___pvcn_1049__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1___pvcn_1049__W 	240
#define flexidag_openseg_prim_split_1___pvcn_1049__H 	1
#define flexidag_openseg_prim_split_1___pvcn_1049__D 	1
#define flexidag_openseg_prim_split_1___pvcn_1049__P 	1
#define flexidag_openseg_prim_split_1___pvcn_1049__data_num_bytes 	1
#define flexidag_openseg_prim_split_1___pvcn_1049__denable 	1
#define flexidag_openseg_prim_split_1___pvcn_1049__dpitch_num_bytes 	256
#define flexidag_openseg_prim_split_1___pvcn_1049__dram_format 	0
#define flexidag_openseg_prim_split_1___pvcn_1049__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_1___pvcn_1049__tile_width 	1
#define flexidag_openseg_prim_split_1___pvcn_1049__tile_height 	1
#define flexidag_openseg_prim_split_1___pvcn_1049__vmem_buffer_num_bytes 	240
#define flexidag_openseg_prim_split_1___pvcn_1049__dram_size_num_bytes 	256

/* flexidag_openseg_prim_split_1 SMB_input __pvcn_1050_ */
#define flexidag_openseg_prim_split_1___pvcn_1050__cnngen_demangled_name 	"__pvcn_1050_"
#define flexidag_openseg_prim_split_1___pvcn_1050__is_constant 	1
#define flexidag_openseg_prim_split_1___pvcn_1050__has_init_data 	1
#define flexidag_openseg_prim_split_1___pvcn_1050__is_variable_scalar 	0
#define flexidag_openseg_prim_split_1___pvcn_1050__is_bitvector 	0
#define flexidag_openseg_prim_split_1___pvcn_1050__is_rlz 	0
#define flexidag_openseg_prim_split_1___pvcn_1050__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_1050__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_1050__byte_offset 	1848	/* 0x00000738 */
#define flexidag_openseg_prim_split_1___pvcn_1050__vbase_byte_offset 	1852	/* 0x0000073c */
#define flexidag_openseg_prim_split_1___pvcn_1050__dbase_byte_offset 	1876	/* 0x00000754 */
#define flexidag_openseg_prim_split_1___pvcn_1050__dpitchm1_byte_offset 	1880	/* 0x00000758 */
#define flexidag_openseg_prim_split_1___pvcn_1050__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1___pvcn_1050__W 	184
#define flexidag_openseg_prim_split_1___pvcn_1050__H 	1
#define flexidag_openseg_prim_split_1___pvcn_1050__D 	1
#define flexidag_openseg_prim_split_1___pvcn_1050__P 	1
#define flexidag_openseg_prim_split_1___pvcn_1050__data_num_bytes 	1
#define flexidag_openseg_prim_split_1___pvcn_1050__denable 	1
#define flexidag_openseg_prim_split_1___pvcn_1050__dpitch_num_bytes 	192
#define flexidag_openseg_prim_split_1___pvcn_1050__dram_format 	0
#define flexidag_openseg_prim_split_1___pvcn_1050__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_1___pvcn_1050__tile_width 	1
#define flexidag_openseg_prim_split_1___pvcn_1050__tile_height 	1
#define flexidag_openseg_prim_split_1___pvcn_1050__vmem_buffer_num_bytes 	184
#define flexidag_openseg_prim_split_1___pvcn_1050__dram_size_num_bytes 	192

/* flexidag_openseg_prim_split_1 SMB_input __pvcn_1056_ */
#define flexidag_openseg_prim_split_1___pvcn_1056__cnngen_demangled_name 	"__pvcn_1056_"
#define flexidag_openseg_prim_split_1___pvcn_1056__is_constant 	1
#define flexidag_openseg_prim_split_1___pvcn_1056__has_init_data 	1
#define flexidag_openseg_prim_split_1___pvcn_1056__is_variable_scalar 	0
#define flexidag_openseg_prim_split_1___pvcn_1056__is_bitvector 	0
#define flexidag_openseg_prim_split_1___pvcn_1056__is_rlz 	0
#define flexidag_openseg_prim_split_1___pvcn_1056__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_1056__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_1056__byte_offset 	1888	/* 0x00000760 */
#define flexidag_openseg_prim_split_1___pvcn_1056__vbase_byte_offset 	1892	/* 0x00000764 */
#define flexidag_openseg_prim_split_1___pvcn_1056__dbase_byte_offset 	1916	/* 0x0000077c */
#define flexidag_openseg_prim_split_1___pvcn_1056__dpitchm1_byte_offset 	1920	/* 0x00000780 */
#define flexidag_openseg_prim_split_1___pvcn_1056__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1___pvcn_1056__W 	240
#define flexidag_openseg_prim_split_1___pvcn_1056__H 	1
#define flexidag_openseg_prim_split_1___pvcn_1056__D 	1
#define flexidag_openseg_prim_split_1___pvcn_1056__P 	1
#define flexidag_openseg_prim_split_1___pvcn_1056__data_num_bytes 	1
#define flexidag_openseg_prim_split_1___pvcn_1056__denable 	1
#define flexidag_openseg_prim_split_1___pvcn_1056__dpitch_num_bytes 	256
#define flexidag_openseg_prim_split_1___pvcn_1056__dram_format 	0
#define flexidag_openseg_prim_split_1___pvcn_1056__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_1___pvcn_1056__tile_width 	1
#define flexidag_openseg_prim_split_1___pvcn_1056__tile_height 	1
#define flexidag_openseg_prim_split_1___pvcn_1056__vmem_buffer_num_bytes 	240
#define flexidag_openseg_prim_split_1___pvcn_1056__dram_size_num_bytes 	256

/* flexidag_openseg_prim_split_1 SMB_input __pvcn_1057_ */
#define flexidag_openseg_prim_split_1___pvcn_1057__cnngen_demangled_name 	"__pvcn_1057_"
#define flexidag_openseg_prim_split_1___pvcn_1057__is_constant 	1
#define flexidag_openseg_prim_split_1___pvcn_1057__has_init_data 	1
#define flexidag_openseg_prim_split_1___pvcn_1057__is_variable_scalar 	0
#define flexidag_openseg_prim_split_1___pvcn_1057__is_bitvector 	0
#define flexidag_openseg_prim_split_1___pvcn_1057__is_rlz 	0
#define flexidag_openseg_prim_split_1___pvcn_1057__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_1057__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_1057__byte_offset 	1928	/* 0x00000788 */
#define flexidag_openseg_prim_split_1___pvcn_1057__vbase_byte_offset 	1932	/* 0x0000078c */
#define flexidag_openseg_prim_split_1___pvcn_1057__dbase_byte_offset 	1956	/* 0x000007a4 */
#define flexidag_openseg_prim_split_1___pvcn_1057__dpitchm1_byte_offset 	1960	/* 0x000007a8 */
#define flexidag_openseg_prim_split_1___pvcn_1057__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1___pvcn_1057__W 	184
#define flexidag_openseg_prim_split_1___pvcn_1057__H 	1
#define flexidag_openseg_prim_split_1___pvcn_1057__D 	1
#define flexidag_openseg_prim_split_1___pvcn_1057__P 	1
#define flexidag_openseg_prim_split_1___pvcn_1057__data_num_bytes 	1
#define flexidag_openseg_prim_split_1___pvcn_1057__denable 	1
#define flexidag_openseg_prim_split_1___pvcn_1057__dpitch_num_bytes 	192
#define flexidag_openseg_prim_split_1___pvcn_1057__dram_format 	0
#define flexidag_openseg_prim_split_1___pvcn_1057__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_1___pvcn_1057__tile_width 	1
#define flexidag_openseg_prim_split_1___pvcn_1057__tile_height 	1
#define flexidag_openseg_prim_split_1___pvcn_1057__vmem_buffer_num_bytes 	184
#define flexidag_openseg_prim_split_1___pvcn_1057__dram_size_num_bytes 	192

/* flexidag_openseg_prim_split_1 SMB_input __pvcn_1063_ */
#define flexidag_openseg_prim_split_1___pvcn_1063__cnngen_demangled_name 	"__pvcn_1063_"
#define flexidag_openseg_prim_split_1___pvcn_1063__is_constant 	1
#define flexidag_openseg_prim_split_1___pvcn_1063__has_init_data 	1
#define flexidag_openseg_prim_split_1___pvcn_1063__is_variable_scalar 	0
#define flexidag_openseg_prim_split_1___pvcn_1063__is_bitvector 	0
#define flexidag_openseg_prim_split_1___pvcn_1063__is_rlz 	0
#define flexidag_openseg_prim_split_1___pvcn_1063__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_1063__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_1063__byte_offset 	1968	/* 0x000007b0 */
#define flexidag_openseg_prim_split_1___pvcn_1063__vbase_byte_offset 	1972	/* 0x000007b4 */
#define flexidag_openseg_prim_split_1___pvcn_1063__dbase_byte_offset 	1996	/* 0x000007cc */
#define flexidag_openseg_prim_split_1___pvcn_1063__dpitchm1_byte_offset 	2000	/* 0x000007d0 */
#define flexidag_openseg_prim_split_1___pvcn_1063__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1___pvcn_1063__W 	240
#define flexidag_openseg_prim_split_1___pvcn_1063__H 	1
#define flexidag_openseg_prim_split_1___pvcn_1063__D 	1
#define flexidag_openseg_prim_split_1___pvcn_1063__P 	1
#define flexidag_openseg_prim_split_1___pvcn_1063__data_num_bytes 	1
#define flexidag_openseg_prim_split_1___pvcn_1063__denable 	1
#define flexidag_openseg_prim_split_1___pvcn_1063__dpitch_num_bytes 	256
#define flexidag_openseg_prim_split_1___pvcn_1063__dram_format 	0
#define flexidag_openseg_prim_split_1___pvcn_1063__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_1___pvcn_1063__tile_width 	1
#define flexidag_openseg_prim_split_1___pvcn_1063__tile_height 	1
#define flexidag_openseg_prim_split_1___pvcn_1063__vmem_buffer_num_bytes 	240
#define flexidag_openseg_prim_split_1___pvcn_1063__dram_size_num_bytes 	256

/* flexidag_openseg_prim_split_1 SMB_input __pvcn_1064_ */
#define flexidag_openseg_prim_split_1___pvcn_1064__cnngen_demangled_name 	"__pvcn_1064_"
#define flexidag_openseg_prim_split_1___pvcn_1064__is_constant 	1
#define flexidag_openseg_prim_split_1___pvcn_1064__has_init_data 	1
#define flexidag_openseg_prim_split_1___pvcn_1064__is_variable_scalar 	0
#define flexidag_openseg_prim_split_1___pvcn_1064__is_bitvector 	0
#define flexidag_openseg_prim_split_1___pvcn_1064__is_rlz 	0
#define flexidag_openseg_prim_split_1___pvcn_1064__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_1064__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_1064__byte_offset 	2008	/* 0x000007d8 */
#define flexidag_openseg_prim_split_1___pvcn_1064__vbase_byte_offset 	2012	/* 0x000007dc */
#define flexidag_openseg_prim_split_1___pvcn_1064__dbase_byte_offset 	2036	/* 0x000007f4 */
#define flexidag_openseg_prim_split_1___pvcn_1064__dpitchm1_byte_offset 	2040	/* 0x000007f8 */
#define flexidag_openseg_prim_split_1___pvcn_1064__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1___pvcn_1064__W 	184
#define flexidag_openseg_prim_split_1___pvcn_1064__H 	1
#define flexidag_openseg_prim_split_1___pvcn_1064__D 	1
#define flexidag_openseg_prim_split_1___pvcn_1064__P 	1
#define flexidag_openseg_prim_split_1___pvcn_1064__data_num_bytes 	1
#define flexidag_openseg_prim_split_1___pvcn_1064__denable 	1
#define flexidag_openseg_prim_split_1___pvcn_1064__dpitch_num_bytes 	192
#define flexidag_openseg_prim_split_1___pvcn_1064__dram_format 	0
#define flexidag_openseg_prim_split_1___pvcn_1064__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_1___pvcn_1064__tile_width 	1
#define flexidag_openseg_prim_split_1___pvcn_1064__tile_height 	1
#define flexidag_openseg_prim_split_1___pvcn_1064__vmem_buffer_num_bytes 	184
#define flexidag_openseg_prim_split_1___pvcn_1064__dram_size_num_bytes 	192

/* flexidag_openseg_prim_split_1 SMB_input __pvcn_1070_ */
#define flexidag_openseg_prim_split_1___pvcn_1070__cnngen_demangled_name 	"__pvcn_1070_"
#define flexidag_openseg_prim_split_1___pvcn_1070__is_constant 	1
#define flexidag_openseg_prim_split_1___pvcn_1070__has_init_data 	1
#define flexidag_openseg_prim_split_1___pvcn_1070__is_variable_scalar 	0
#define flexidag_openseg_prim_split_1___pvcn_1070__is_bitvector 	0
#define flexidag_openseg_prim_split_1___pvcn_1070__is_rlz 	0
#define flexidag_openseg_prim_split_1___pvcn_1070__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_1070__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_1070__byte_offset 	2048	/* 0x00000800 */
#define flexidag_openseg_prim_split_1___pvcn_1070__vbase_byte_offset 	2052	/* 0x00000804 */
#define flexidag_openseg_prim_split_1___pvcn_1070__dbase_byte_offset 	2076	/* 0x0000081c */
#define flexidag_openseg_prim_split_1___pvcn_1070__dpitchm1_byte_offset 	2080	/* 0x00000820 */
#define flexidag_openseg_prim_split_1___pvcn_1070__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1___pvcn_1070__W 	240
#define flexidag_openseg_prim_split_1___pvcn_1070__H 	1
#define flexidag_openseg_prim_split_1___pvcn_1070__D 	1
#define flexidag_openseg_prim_split_1___pvcn_1070__P 	1
#define flexidag_openseg_prim_split_1___pvcn_1070__data_num_bytes 	1
#define flexidag_openseg_prim_split_1___pvcn_1070__denable 	1
#define flexidag_openseg_prim_split_1___pvcn_1070__dpitch_num_bytes 	256
#define flexidag_openseg_prim_split_1___pvcn_1070__dram_format 	0
#define flexidag_openseg_prim_split_1___pvcn_1070__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_1___pvcn_1070__tile_width 	1
#define flexidag_openseg_prim_split_1___pvcn_1070__tile_height 	1
#define flexidag_openseg_prim_split_1___pvcn_1070__vmem_buffer_num_bytes 	240
#define flexidag_openseg_prim_split_1___pvcn_1070__dram_size_num_bytes 	256

/* flexidag_openseg_prim_split_1 SMB_input __pvcn_1071_ */
#define flexidag_openseg_prim_split_1___pvcn_1071__cnngen_demangled_name 	"__pvcn_1071_"
#define flexidag_openseg_prim_split_1___pvcn_1071__is_constant 	1
#define flexidag_openseg_prim_split_1___pvcn_1071__has_init_data 	1
#define flexidag_openseg_prim_split_1___pvcn_1071__is_variable_scalar 	0
#define flexidag_openseg_prim_split_1___pvcn_1071__is_bitvector 	0
#define flexidag_openseg_prim_split_1___pvcn_1071__is_rlz 	0
#define flexidag_openseg_prim_split_1___pvcn_1071__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_1071__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_1071__byte_offset 	2088	/* 0x00000828 */
#define flexidag_openseg_prim_split_1___pvcn_1071__vbase_byte_offset 	2092	/* 0x0000082c */
#define flexidag_openseg_prim_split_1___pvcn_1071__dbase_byte_offset 	2116	/* 0x00000844 */
#define flexidag_openseg_prim_split_1___pvcn_1071__dpitchm1_byte_offset 	2120	/* 0x00000848 */
#define flexidag_openseg_prim_split_1___pvcn_1071__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1___pvcn_1071__W 	184
#define flexidag_openseg_prim_split_1___pvcn_1071__H 	1
#define flexidag_openseg_prim_split_1___pvcn_1071__D 	1
#define flexidag_openseg_prim_split_1___pvcn_1071__P 	1
#define flexidag_openseg_prim_split_1___pvcn_1071__data_num_bytes 	1
#define flexidag_openseg_prim_split_1___pvcn_1071__denable 	1
#define flexidag_openseg_prim_split_1___pvcn_1071__dpitch_num_bytes 	192
#define flexidag_openseg_prim_split_1___pvcn_1071__dram_format 	0
#define flexidag_openseg_prim_split_1___pvcn_1071__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_1___pvcn_1071__tile_width 	1
#define flexidag_openseg_prim_split_1___pvcn_1071__tile_height 	1
#define flexidag_openseg_prim_split_1___pvcn_1071__vmem_buffer_num_bytes 	184
#define flexidag_openseg_prim_split_1___pvcn_1071__dram_size_num_bytes 	192

/* flexidag_openseg_prim_split_1 SMB_input __pvcn_1077_ */
#define flexidag_openseg_prim_split_1___pvcn_1077__cnngen_demangled_name 	"__pvcn_1077_"
#define flexidag_openseg_prim_split_1___pvcn_1077__is_constant 	1
#define flexidag_openseg_prim_split_1___pvcn_1077__has_init_data 	1
#define flexidag_openseg_prim_split_1___pvcn_1077__is_variable_scalar 	0
#define flexidag_openseg_prim_split_1___pvcn_1077__is_bitvector 	0
#define flexidag_openseg_prim_split_1___pvcn_1077__is_rlz 	0
#define flexidag_openseg_prim_split_1___pvcn_1077__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_1077__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_1077__byte_offset 	2128	/* 0x00000850 */
#define flexidag_openseg_prim_split_1___pvcn_1077__vbase_byte_offset 	2132	/* 0x00000854 */
#define flexidag_openseg_prim_split_1___pvcn_1077__dbase_byte_offset 	2156	/* 0x0000086c */
#define flexidag_openseg_prim_split_1___pvcn_1077__dpitchm1_byte_offset 	2160	/* 0x00000870 */
#define flexidag_openseg_prim_split_1___pvcn_1077__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1___pvcn_1077__W 	240
#define flexidag_openseg_prim_split_1___pvcn_1077__H 	1
#define flexidag_openseg_prim_split_1___pvcn_1077__D 	1
#define flexidag_openseg_prim_split_1___pvcn_1077__P 	1
#define flexidag_openseg_prim_split_1___pvcn_1077__data_num_bytes 	1
#define flexidag_openseg_prim_split_1___pvcn_1077__denable 	1
#define flexidag_openseg_prim_split_1___pvcn_1077__dpitch_num_bytes 	256
#define flexidag_openseg_prim_split_1___pvcn_1077__dram_format 	0
#define flexidag_openseg_prim_split_1___pvcn_1077__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_1___pvcn_1077__tile_width 	1
#define flexidag_openseg_prim_split_1___pvcn_1077__tile_height 	1
#define flexidag_openseg_prim_split_1___pvcn_1077__vmem_buffer_num_bytes 	240
#define flexidag_openseg_prim_split_1___pvcn_1077__dram_size_num_bytes 	256

/* flexidag_openseg_prim_split_1 SMB_input __pvcn_1078_ */
#define flexidag_openseg_prim_split_1___pvcn_1078__cnngen_demangled_name 	"__pvcn_1078_"
#define flexidag_openseg_prim_split_1___pvcn_1078__is_constant 	1
#define flexidag_openseg_prim_split_1___pvcn_1078__has_init_data 	1
#define flexidag_openseg_prim_split_1___pvcn_1078__is_variable_scalar 	0
#define flexidag_openseg_prim_split_1___pvcn_1078__is_bitvector 	0
#define flexidag_openseg_prim_split_1___pvcn_1078__is_rlz 	0
#define flexidag_openseg_prim_split_1___pvcn_1078__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_1078__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1___pvcn_1078__byte_offset 	2168	/* 0x00000878 */
#define flexidag_openseg_prim_split_1___pvcn_1078__vbase_byte_offset 	2172	/* 0x0000087c */
#define flexidag_openseg_prim_split_1___pvcn_1078__dbase_byte_offset 	2196	/* 0x00000894 */
#define flexidag_openseg_prim_split_1___pvcn_1078__dpitchm1_byte_offset 	2200	/* 0x00000898 */
#define flexidag_openseg_prim_split_1___pvcn_1078__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1___pvcn_1078__W 	184
#define flexidag_openseg_prim_split_1___pvcn_1078__H 	1
#define flexidag_openseg_prim_split_1___pvcn_1078__D 	1
#define flexidag_openseg_prim_split_1___pvcn_1078__P 	1
#define flexidag_openseg_prim_split_1___pvcn_1078__data_num_bytes 	1
#define flexidag_openseg_prim_split_1___pvcn_1078__denable 	1
#define flexidag_openseg_prim_split_1___pvcn_1078__dpitch_num_bytes 	192
#define flexidag_openseg_prim_split_1___pvcn_1078__dram_format 	0
#define flexidag_openseg_prim_split_1___pvcn_1078__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_1___pvcn_1078__tile_width 	1
#define flexidag_openseg_prim_split_1___pvcn_1078__tile_height 	1
#define flexidag_openseg_prim_split_1___pvcn_1078__vmem_buffer_num_bytes 	184
#define flexidag_openseg_prim_split_1___pvcn_1078__dram_size_num_bytes 	192

/* flexidag_openseg_prim_split_1 VCB PeleeNet__stage_2_dense_block_0__concat */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_0__concat_cnngen_demangled_name 	"PeleeNet__stage_2_dense_block_0__concat"
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_0__concat_is_bitvector 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_0__concat_is_rlz 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_0__concat_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_0__concat_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_0__concat_byte_offset 	2208	/* 0x000008a0 */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_0__concat_vbase_byte_offset 	2216	/* 0x000008a8 */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_0__concat_vmem_buffer_num_bytes 	158720

/* flexidag_openseg_prim_split_1 VCB PeleeNet__stage_2_dense_block_0__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_110001_ */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_0__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_110001__cnngen_demangled_name 	"PeleeNet__stage_2_dense_block_0__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_110001_"
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_0__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_110001__is_bitvector 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_0__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_110001__is_rlz 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_0__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_110001__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_0__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_110001__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_0__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_110001__byte_offset 	2260	/* 0x000008d4 */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_0__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_110001__vbase_byte_offset 	2268	/* 0x000008dc */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_0__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_110001__vmem_buffer_num_bytes 	20992

/* flexidag_openseg_prim_split_1 VCB PeleeNet__stage_2_dense_block_0__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_110002_ */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_0__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_110002__cnngen_demangled_name 	"PeleeNet__stage_2_dense_block_0__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_110002_"
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_0__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_110002__is_bitvector 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_0__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_110002__is_rlz 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_0__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_110002__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_0__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_110002__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_0__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_110002__byte_offset 	2304	/* 0x00000900 */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_0__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_110002__vbase_byte_offset 	2312	/* 0x00000908 */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_0__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_110002__vmem_buffer_num_bytes 	20992

/* flexidag_openseg_prim_split_1 VCB PeleeNet__stage_2_dense_block_0__conv_right_0__Relu */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_0__conv_right_0__Relu_cnngen_demangled_name 	"PeleeNet__stage_2_dense_block_0__conv_right_0__Relu"
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_0__conv_right_0__Relu_is_bitvector 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_0__conv_right_0__Relu_is_rlz 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_0__conv_right_0__Relu_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_0__conv_right_0__Relu_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_0__conv_right_0__Relu_byte_offset 	2340	/* 0x00000924 */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_0__conv_right_0__Relu_vbase_byte_offset 	2348	/* 0x0000092c */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_0__conv_right_0__Relu_vmem_buffer_num_bytes 	23168

/* flexidag_openseg_prim_split_1 VCB PeleeNet__stage_2_dense_block_0__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_0__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_cnngen_demangled_name 	"PeleeNet__stage_2_dense_block_0__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu"
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_0__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_bitvector 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_0__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_rlz 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_0__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_0__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_0__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_byte_offset 	2388	/* 0x00000954 */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_0__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vbase_byte_offset 	2396	/* 0x0000095c */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_0__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vmem_buffer_num_bytes 	17920

/* flexidag_openseg_prim_split_1 VCB PeleeNet__stage_2_dense_block_0__conv_left_0__Relu */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_0__conv_left_0__Relu_cnngen_demangled_name 	"PeleeNet__stage_2_dense_block_0__conv_left_0__Relu"
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_0__conv_left_0__Relu_is_bitvector 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_0__conv_left_0__Relu_is_rlz 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_0__conv_left_0__Relu_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_0__conv_left_0__Relu_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_0__conv_left_0__Relu_byte_offset 	2456	/* 0x00000998 */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_0__conv_left_0__Relu_vbase_byte_offset 	2464	/* 0x000009a0 */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_0__conv_left_0__Relu_vmem_buffer_num_bytes 	28160

/* flexidag_openseg_prim_split_1 VCB PeleeNet__stage_2_dense_block_1__concat */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_1__concat_cnngen_demangled_name 	"PeleeNet__stage_2_dense_block_1__concat"
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_1__concat_is_bitvector 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_1__concat_is_rlz 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_1__concat_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_1__concat_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_1__concat_byte_offset 	2532	/* 0x000009e4 */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_1__concat_vbase_byte_offset 	2540	/* 0x000009ec */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_1__concat_vmem_buffer_num_bytes 	187392

/* flexidag_openseg_prim_split_1 VCB PeleeNet__stage_2_dense_block_1__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_1__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_cnngen_demangled_name 	"PeleeNet__stage_2_dense_block_1__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm"
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_1__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_is_bitvector 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_1__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_is_rlz 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_1__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_1__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_1__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_byte_offset 	2604	/* 0x00000a2c */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_1__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_vbase_byte_offset 	2612	/* 0x00000a34 */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_1__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_vmem_buffer_num_bytes 	82944

/* flexidag_openseg_prim_split_1 VCB PeleeNet__stage_2_dense_block_1__conv_right_0__Relu */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_1__conv_right_0__Relu_cnngen_demangled_name 	"PeleeNet__stage_2_dense_block_1__conv_right_0__Relu"
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_1__conv_right_0__Relu_is_bitvector 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_1__conv_right_0__Relu_is_rlz 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_1__conv_right_0__Relu_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_1__conv_right_0__Relu_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_1__conv_right_0__Relu_byte_offset 	2656	/* 0x00000a60 */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_1__conv_right_0__Relu_vbase_byte_offset 	2664	/* 0x00000a68 */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_1__conv_right_0__Relu_vmem_buffer_num_bytes 	28160

/* flexidag_openseg_prim_split_1 VCB PeleeNet__stage_2_dense_block_1__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_1__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_cnngen_demangled_name 	"PeleeNet__stage_2_dense_block_1__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu"
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_1__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_bitvector 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_1__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_rlz 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_1__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_1__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_1__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_byte_offset 	2704	/* 0x00000a90 */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_1__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vbase_byte_offset 	2712	/* 0x00000a98 */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_1__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vmem_buffer_num_bytes 	27840

/* flexidag_openseg_prim_split_1 VCB PeleeNet__stage_2_dense_block_1__conv_left_0__Relu */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_1__conv_left_0__Relu_cnngen_demangled_name 	"PeleeNet__stage_2_dense_block_1__conv_left_0__Relu"
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_1__conv_left_0__Relu_is_bitvector 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_1__conv_left_0__Relu_is_rlz 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_1__conv_left_0__Relu_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_1__conv_left_0__Relu_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_1__conv_left_0__Relu_byte_offset 	2776	/* 0x00000ad8 */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_1__conv_left_0__Relu_vbase_byte_offset 	2784	/* 0x00000ae0 */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_1__conv_left_0__Relu_vmem_buffer_num_bytes 	28160

/* flexidag_openseg_prim_split_1 VCB PeleeNet__stage_2_dense_block_2__concat */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_2__concat_cnngen_demangled_name 	"PeleeNet__stage_2_dense_block_2__concat"
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_2__concat_is_bitvector 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_2__concat_is_rlz 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_2__concat_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_2__concat_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_2__concat_byte_offset 	2832	/* 0x00000b10 */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_2__concat_vbase_byte_offset 	2840	/* 0x00000b18 */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_2__concat_vmem_buffer_num_bytes 	250880

/* flexidag_openseg_prim_split_1 VCB PeleeNet__stage_2_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_cnngen_demangled_name 	"PeleeNet__stage_2_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm"
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_is_bitvector 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_is_rlz 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_byte_offset 	2904	/* 0x00000b58 */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_vbase_byte_offset 	2912	/* 0x00000b60 */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_vmem_buffer_num_bytes 	82944

/* flexidag_openseg_prim_split_1 VCB PeleeNet__stage_2_dense_block_2__conv_right_0__Relu */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_2__conv_right_0__Relu_cnngen_demangled_name 	"PeleeNet__stage_2_dense_block_2__conv_right_0__Relu"
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_2__conv_right_0__Relu_is_bitvector 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_2__conv_right_0__Relu_is_rlz 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_2__conv_right_0__Relu_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_2__conv_right_0__Relu_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_2__conv_right_0__Relu_byte_offset 	2956	/* 0x00000b8c */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_2__conv_right_0__Relu_vbase_byte_offset 	2964	/* 0x00000b94 */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_2__conv_right_0__Relu_vmem_buffer_num_bytes 	28160

/* flexidag_openseg_prim_split_1 VCB PeleeNet__stage_2_dense_block_2__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_2__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_cnngen_demangled_name 	"PeleeNet__stage_2_dense_block_2__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu"
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_2__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_bitvector 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_2__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_rlz 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_2__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_2__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_2__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_byte_offset 	3004	/* 0x00000bbc */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_2__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vbase_byte_offset 	3012	/* 0x00000bc4 */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_2__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vmem_buffer_num_bytes 	27840

/* flexidag_openseg_prim_split_1 VCB PeleeNet__stage_2_dense_block_2__conv_left_0__Relu */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_2__conv_left_0__Relu_cnngen_demangled_name 	"PeleeNet__stage_2_dense_block_2__conv_left_0__Relu"
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_2__conv_left_0__Relu_is_bitvector 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_2__conv_left_0__Relu_is_rlz 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_2__conv_left_0__Relu_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_2__conv_left_0__Relu_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_2__conv_left_0__Relu_byte_offset 	3076	/* 0x00000c04 */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_2__conv_left_0__Relu_vbase_byte_offset 	3084	/* 0x00000c0c */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_2__conv_left_0__Relu_vmem_buffer_num_bytes 	28160

/* flexidag_openseg_prim_split_1 VCB PeleeNet__stage_2_dense_block_3__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_3__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_cnngen_demangled_name 	"PeleeNet__stage_2_dense_block_3__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm"
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_3__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_is_bitvector 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_3__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_is_rlz 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_3__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_3__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_3__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_byte_offset 	3188	/* 0x00000c74 */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_3__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_vbase_byte_offset 	3196	/* 0x00000c7c */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_3__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_vmem_buffer_num_bytes 	62464

/* flexidag_openseg_prim_split_1 VCB PeleeNet__stage_2_dense_block_3__conv_right_0__Relu */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_3__conv_right_0__Relu_cnngen_demangled_name 	"PeleeNet__stage_2_dense_block_3__conv_right_0__Relu"
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_3__conv_right_0__Relu_is_bitvector 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_3__conv_right_0__Relu_is_rlz 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_3__conv_right_0__Relu_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_3__conv_right_0__Relu_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_3__conv_right_0__Relu_byte_offset 	3240	/* 0x00000ca8 */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_3__conv_right_0__Relu_vbase_byte_offset 	3248	/* 0x00000cb0 */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_3__conv_right_0__Relu_vmem_buffer_num_bytes 	28160

/* flexidag_openseg_prim_split_1 VCB PeleeNet__stage_2_dense_block_3__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_3__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_cnngen_demangled_name 	"PeleeNet__stage_2_dense_block_3__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu"
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_3__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_bitvector 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_3__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_rlz 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_3__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_3__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_3__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_byte_offset 	3288	/* 0x00000cd8 */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_3__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vbase_byte_offset 	3296	/* 0x00000ce0 */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_3__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vmem_buffer_num_bytes 	27840

/* flexidag_openseg_prim_split_1 VCB PeleeNet__stage_2_dense_block_3__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_3__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_cnngen_demangled_name 	"PeleeNet__stage_2_dense_block_3__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu"
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_3__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_bitvector 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_3__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_rlz 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_3__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_3__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_3__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_byte_offset 	3336	/* 0x00000d08 */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_3__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_vbase_byte_offset 	3344	/* 0x00000d10 */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_3__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_vmem_buffer_num_bytes 	15616

/* flexidag_openseg_prim_split_1 VCB PeleeNet__stage_2_dense_block_3__conv_left_0__Relu */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_3__conv_left_0__Relu_cnngen_demangled_name 	"PeleeNet__stage_2_dense_block_3__conv_left_0__Relu"
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_3__conv_left_0__Relu_is_bitvector 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_3__conv_left_0__Relu_is_rlz 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_3__conv_left_0__Relu_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_3__conv_left_0__Relu_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_3__conv_left_0__Relu_byte_offset 	3388	/* 0x00000d3c */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_3__conv_left_0__Relu_vbase_byte_offset 	3396	/* 0x00000d44 */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_3__conv_left_0__Relu_vmem_buffer_num_bytes 	23168

/* flexidag_openseg_prim_split_1 VCB PeleeNet__stage_2_dense_block_3__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_3__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_cnngen_demangled_name 	"PeleeNet__stage_2_dense_block_3__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu"
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_3__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_bitvector 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_3__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_rlz 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_3__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_3__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_3__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_byte_offset 	3436	/* 0x00000d6c */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_3__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vbase_byte_offset 	3444	/* 0x00000d74 */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_3__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vmem_buffer_num_bytes 	20992

/* flexidag_openseg_prim_split_1 VCB PeleeNet__stage_2_dense_block_3__concat */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_3__concat_cnngen_demangled_name 	"PeleeNet__stage_2_dense_block_3__concat"
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_3__concat_is_bitvector 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_3__concat_is_rlz 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_3__concat_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_3__concat_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_3__concat_byte_offset 	3516	/* 0x00000dbc */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_3__concat_vbase_byte_offset 	3524	/* 0x00000dc4 */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_dense_block_3__concat_vmem_buffer_num_bytes 	86016

/* flexidag_openseg_prim_split_1 VCB PeleeNet__stage_2_transition_layer__transition_layer_conv0__Conv2D___BatchNorm__FusedBatchNorm___Relu */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_conv0__Conv2D___BatchNorm__FusedBatchNorm___Relu_cnngen_demangled_name 	"PeleeNet__stage_2_transition_layer__transition_layer_conv0__Conv2D___BatchNorm__FusedBatchNorm___Relu"
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_conv0__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_bitvector 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_conv0__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_rlz 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_conv0__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_conv0__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_conv0__Conv2D___BatchNorm__FusedBatchNorm___Relu_byte_offset 	3552	/* 0x00000de0 */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_conv0__Conv2D___BatchNorm__FusedBatchNorm___Relu_vbase_byte_offset 	3560	/* 0x00000de8 */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_conv0__Conv2D___BatchNorm__FusedBatchNorm___Relu_vmem_buffer_num_bytes 	517120

/* flexidag_openseg_prim_split_1 HMB_output PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_cnngen_demangled_name 	"PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool"
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_is_main_input_output 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_is_bitvector 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_is_rlz 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_byte_offset 	3644	/* 0x00000e3c */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_vbase_byte_offset 	3648	/* 0x00000e40 */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_dbase_byte_offset 	3672	/* 0x00000e58 */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_dpitchm1_byte_offset 	3676	/* 0x00000e5c */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_vwidth_minus_one_byte_offset 	3660	/* 0x00000e4c */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_vheight_minus_one_byte_offset 	3662	/* 0x00000e4e */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_vheight_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_drotate_bit_offset 	29350	/* 0x000072a6 */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_hflip_bit_offset 	29351	/* 0x000072a7 */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_vflip_bit_offset 	29352	/* 0x000072a8 */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_dflip_bit_offset 	29353	/* 0x000072a9 */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_pflip_bit_offset 	29354	/* 0x000072aa */
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_W 	80
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_H 	32
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_D 	256
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_P 	1
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_data_num_bytes 	1
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_denable 	1
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_dpitch_num_bytes 	96
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_dram_format 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_vp_interleave_mode 	2
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_tile_width 	4
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_tile_height 	1
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_vmem_buffer_num_bytes 	40960
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_dram_size_num_bytes 	786432
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_sign 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_datasize 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_expoffset 	6
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_expbits 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_drotate 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_hflip 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_vflip 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_dflip 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_pflip 	0

/* flexidag_openseg_prim_split_1 SMB_output split_1__vas_created_terminate__v0 */
#define flexidag_openseg_prim_split_1_split_1__vas_created_terminate__v0_cnngen_demangled_name 	"split_1__vas_created_terminate__v0"
#define flexidag_openseg_prim_split_1_split_1__vas_created_terminate__v0_is_main_input_output 	0
#define flexidag_openseg_prim_split_1_split_1__vas_created_terminate__v0_is_bitvector 	1
#define flexidag_openseg_prim_split_1_split_1__vas_created_terminate__v0_is_rlz 	0
#define flexidag_openseg_prim_split_1_split_1__vas_created_terminate__v0_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1_split_1__vas_created_terminate__v0_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_1_split_1__vas_created_terminate__v0_byte_offset 	3972	/* 0x00000f84 */
#define flexidag_openseg_prim_split_1_split_1__vas_created_terminate__v0_vbase_byte_offset 	3976	/* 0x00000f88 */
#define flexidag_openseg_prim_split_1_split_1__vas_created_terminate__v0_W 	256
#define flexidag_openseg_prim_split_1_split_1__vas_created_terminate__v0_H 	3
#define flexidag_openseg_prim_split_1_split_1__vas_created_terminate__v0_D 	1
#define flexidag_openseg_prim_split_1_split_1__vas_created_terminate__v0_P 	1
#define flexidag_openseg_prim_split_1_split_1__vas_created_terminate__v0_data_num_bytes 	1
#define flexidag_openseg_prim_split_1_split_1__vas_created_terminate__v0_denable 	0
#define flexidag_openseg_prim_split_1_split_1__vas_created_terminate__v0_dram_format 	0
#define flexidag_openseg_prim_split_1_split_1__vas_created_terminate__v0_vp_interleave_mode 	1
#define flexidag_openseg_prim_split_1_split_1__vas_created_terminate__v0_tile_width 	1
#define flexidag_openseg_prim_split_1_split_1__vas_created_terminate__v0_tile_height 	1
#define flexidag_openseg_prim_split_1_split_1__vas_created_terminate__v0_vmem_buffer_num_bytes 	96


#endif /* flexidag_openseg_prim_split_1_H */
