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
#ifndef flexidag_openseg_prim_split_4_H
#define flexidag_openseg_prim_split_4_H

#define vas_gen_header_version                  	7

/* dag name */
#define flexidag_openseg_prim_split_4_DAG_NAME  	"flexidag_openseg_prim_split_4"

/* VDG file info */
#define flexidag_openseg_prim_split_4_vdg_name  	"flexidag_openseg_split_4.vdg"

/* VDG DAG memory info */
#define flexidag_openseg_prim_split_4_byte_offset 	4294967295	/* 0xffffffff */
#define flexidag_openseg_prim_split_4_byte_size 	2080	/* 0x00000820 */
#define flexidag_openseg_prim_split_4_preferred_dram_xfer_size 	1

/* flexidag_openseg_prim_split_4 VMEM info */
#define flexidag_openseg_prim_split_4_use_ping_pong_vmem 	0
#define flexidag_openseg_prim_split_4_VMEM_start 	4096	/* 0x00001000 */
#define flexidag_openseg_prim_split_4_VMEM_end  	2068924	/* 0x001f91bc */
#define flexidag_openseg_prim_split_4_image_start 	1918944	/* 0x001d47e0 */
#define flexidag_openseg_prim_split_4_image_size 	152060	/* 0x000251fc */
#define flexidag_openseg_prim_split_4_dagbin_start 	2068924	/* 0x001f91bc */

/* flexidag_openseg_prim_split_4 DAG info */
#define flexidag_openseg_prim_split_4_estimated_cycles 	629581

/*******************************************************************************
 * DAG byte offset info for HMB/SMB/VCB and their fields
 * in DAG 'flexidag_openseg_prim_split_4' in source file 'flexidag_openseg_split_4.vas'
 ******************************************************************************/
/* flexidag_openseg_prim_split_4 HMB_input PeleeNet__stage_3_dense_block_4__concat */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_4__concat_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_4__concat"
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_4__concat_is_main_input_output 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_4__concat_is_constant 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_4__concat_has_init_data 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_4__concat_is_variable_scalar 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_4__concat_ddi_byte_size 	0	/* 0x00000000 */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_4__concat_ddi_byte_offset 	4294967295	/* 0xffffffff */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_4__concat_is_bitvector 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_4__concat_is_rlz 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_4__concat_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_4__concat_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_4__concat_byte_offset 	0	/* 0x00000000 */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_4__concat_vbase_byte_offset 	4	/* 0x00000004 */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_4__concat_dbase_byte_offset 	28	/* 0x0000001c */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_4__concat_dpitchm1_byte_offset 	32	/* 0x00000020 */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_4__concat_dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_4__concat_vwidth_minus_one_byte_offset 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_4__concat_vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_4__concat_vheight_minus_one_byte_offset 	18	/* 0x00000012 */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_4__concat_vheight_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_4__concat_drotate_bit_offset 	198	/* 0x000000c6 */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_4__concat_hflip_bit_offset 	199	/* 0x000000c7 */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_4__concat_vflip_bit_offset 	200	/* 0x000000c8 */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_4__concat_dflip_bit_offset 	201	/* 0x000000c9 */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_4__concat_pflip_bit_offset 	202	/* 0x000000ca */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_4__concat_W 	80
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_4__concat_H 	32
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_4__concat_D 	416
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_4__concat_P 	1
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_4__concat_data_num_bytes 	1
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_4__concat_denable 	1
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_4__concat_dpitch_num_bytes 	32
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_4__concat_dram_format 	3
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_4__concat_vp_interleave_mode 	2
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_4__concat_tile_width 	4
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_4__concat_tile_height 	1
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_4__concat_vmem_buffer_num_bytes 	332800
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_4__concat_dram_size_num_bytes 	1064960
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_4__concat_sign 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_4__concat_datasize 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_4__concat_expoffset 	5
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_4__concat_expbits 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_4__concat_drotate 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_4__concat_hflip 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_4__concat_vflip 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_4__concat_dflip 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_4__concat_pflip 	0

/* flexidag_openseg_prim_split_4 SMB_input __pvcn_140004_ */
#define flexidag_openseg_prim_split_4___pvcn_140004__cnngen_demangled_name 	"__pvcn_140004_"
#define flexidag_openseg_prim_split_4___pvcn_140004__is_constant 	1
#define flexidag_openseg_prim_split_4___pvcn_140004__has_init_data 	1
#define flexidag_openseg_prim_split_4___pvcn_140004__is_variable_scalar 	0
#define flexidag_openseg_prim_split_4___pvcn_140004__is_bitvector 	0
#define flexidag_openseg_prim_split_4___pvcn_140004__is_rlz 	0
#define flexidag_openseg_prim_split_4___pvcn_140004__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_4___pvcn_140004__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_4___pvcn_140004__byte_offset 	40	/* 0x00000028 */
#define flexidag_openseg_prim_split_4___pvcn_140004__vbase_byte_offset 	44	/* 0x0000002c */
#define flexidag_openseg_prim_split_4___pvcn_140004__dbase_byte_offset 	68	/* 0x00000044 */
#define flexidag_openseg_prim_split_4___pvcn_140004__dpitchm1_byte_offset 	72	/* 0x00000048 */
#define flexidag_openseg_prim_split_4___pvcn_140004__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_4___pvcn_140004__W 	16824
#define flexidag_openseg_prim_split_4___pvcn_140004__H 	1
#define flexidag_openseg_prim_split_4___pvcn_140004__D 	1
#define flexidag_openseg_prim_split_4___pvcn_140004__P 	1
#define flexidag_openseg_prim_split_4___pvcn_140004__data_num_bytes 	1
#define flexidag_openseg_prim_split_4___pvcn_140004__denable 	1
#define flexidag_openseg_prim_split_4___pvcn_140004__dpitch_num_bytes 	16832
#define flexidag_openseg_prim_split_4___pvcn_140004__dram_format 	0
#define flexidag_openseg_prim_split_4___pvcn_140004__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_4___pvcn_140004__tile_width 	1
#define flexidag_openseg_prim_split_4___pvcn_140004__tile_height 	1
#define flexidag_openseg_prim_split_4___pvcn_140004__vmem_buffer_num_bytes 	16824
#define flexidag_openseg_prim_split_4___pvcn_140004__dram_size_num_bytes 	16832

/* flexidag_openseg_prim_split_4 SMB_input __pvcn_140007_ */
#define flexidag_openseg_prim_split_4___pvcn_140007__cnngen_demangled_name 	"__pvcn_140007_"
#define flexidag_openseg_prim_split_4___pvcn_140007__is_constant 	1
#define flexidag_openseg_prim_split_4___pvcn_140007__has_init_data 	1
#define flexidag_openseg_prim_split_4___pvcn_140007__is_variable_scalar 	0
#define flexidag_openseg_prim_split_4___pvcn_140007__is_bitvector 	0
#define flexidag_openseg_prim_split_4___pvcn_140007__is_rlz 	0
#define flexidag_openseg_prim_split_4___pvcn_140007__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_4___pvcn_140007__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_4___pvcn_140007__byte_offset 	76	/* 0x0000004c */
#define flexidag_openseg_prim_split_4___pvcn_140007__vbase_byte_offset 	80	/* 0x00000050 */
#define flexidag_openseg_prim_split_4___pvcn_140007__dbase_byte_offset 	104	/* 0x00000068 */
#define flexidag_openseg_prim_split_4___pvcn_140007__dpitchm1_byte_offset 	108	/* 0x0000006c */
#define flexidag_openseg_prim_split_4___pvcn_140007__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_4___pvcn_140007__W 	16620
#define flexidag_openseg_prim_split_4___pvcn_140007__H 	1
#define flexidag_openseg_prim_split_4___pvcn_140007__D 	1
#define flexidag_openseg_prim_split_4___pvcn_140007__P 	1
#define flexidag_openseg_prim_split_4___pvcn_140007__data_num_bytes 	1
#define flexidag_openseg_prim_split_4___pvcn_140007__denable 	1
#define flexidag_openseg_prim_split_4___pvcn_140007__dpitch_num_bytes 	16640
#define flexidag_openseg_prim_split_4___pvcn_140007__dram_format 	0
#define flexidag_openseg_prim_split_4___pvcn_140007__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_4___pvcn_140007__tile_width 	1
#define flexidag_openseg_prim_split_4___pvcn_140007__tile_height 	1
#define flexidag_openseg_prim_split_4___pvcn_140007__vmem_buffer_num_bytes 	16620
#define flexidag_openseg_prim_split_4___pvcn_140007__dram_size_num_bytes 	16640

/* flexidag_openseg_prim_split_4 SMB_input __pvcn_140005_ */
#define flexidag_openseg_prim_split_4___pvcn_140005__cnngen_demangled_name 	"__pvcn_140005_"
#define flexidag_openseg_prim_split_4___pvcn_140005__is_constant 	1
#define flexidag_openseg_prim_split_4___pvcn_140005__has_init_data 	1
#define flexidag_openseg_prim_split_4___pvcn_140005__is_variable_scalar 	0
#define flexidag_openseg_prim_split_4___pvcn_140005__is_bitvector 	0
#define flexidag_openseg_prim_split_4___pvcn_140005__is_rlz 	0
#define flexidag_openseg_prim_split_4___pvcn_140005__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_4___pvcn_140005__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_4___pvcn_140005__byte_offset 	116	/* 0x00000074 */
#define flexidag_openseg_prim_split_4___pvcn_140005__vbase_byte_offset 	120	/* 0x00000078 */
#define flexidag_openseg_prim_split_4___pvcn_140005__dbase_byte_offset 	144	/* 0x00000090 */
#define flexidag_openseg_prim_split_4___pvcn_140005__dpitchm1_byte_offset 	148	/* 0x00000094 */
#define flexidag_openseg_prim_split_4___pvcn_140005__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_4___pvcn_140005__W 	544
#define flexidag_openseg_prim_split_4___pvcn_140005__H 	1
#define flexidag_openseg_prim_split_4___pvcn_140005__D 	1
#define flexidag_openseg_prim_split_4___pvcn_140005__P 	1
#define flexidag_openseg_prim_split_4___pvcn_140005__data_num_bytes 	1
#define flexidag_openseg_prim_split_4___pvcn_140005__denable 	1
#define flexidag_openseg_prim_split_4___pvcn_140005__dpitch_num_bytes 	544
#define flexidag_openseg_prim_split_4___pvcn_140005__dram_format 	0
#define flexidag_openseg_prim_split_4___pvcn_140005__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_4___pvcn_140005__tile_width 	1
#define flexidag_openseg_prim_split_4___pvcn_140005__tile_height 	1
#define flexidag_openseg_prim_split_4___pvcn_140005__vmem_buffer_num_bytes 	544
#define flexidag_openseg_prim_split_4___pvcn_140005__dram_size_num_bytes 	544

/* flexidag_openseg_prim_split_4 SMB_input __pvcn_140008_ */
#define flexidag_openseg_prim_split_4___pvcn_140008__cnngen_demangled_name 	"__pvcn_140008_"
#define flexidag_openseg_prim_split_4___pvcn_140008__is_constant 	1
#define flexidag_openseg_prim_split_4___pvcn_140008__has_init_data 	1
#define flexidag_openseg_prim_split_4___pvcn_140008__is_variable_scalar 	0
#define flexidag_openseg_prim_split_4___pvcn_140008__is_bitvector 	0
#define flexidag_openseg_prim_split_4___pvcn_140008__is_rlz 	0
#define flexidag_openseg_prim_split_4___pvcn_140008__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_4___pvcn_140008__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_4___pvcn_140008__byte_offset 	156	/* 0x0000009c */
#define flexidag_openseg_prim_split_4___pvcn_140008__vbase_byte_offset 	160	/* 0x000000a0 */
#define flexidag_openseg_prim_split_4___pvcn_140008__dbase_byte_offset 	184	/* 0x000000b8 */
#define flexidag_openseg_prim_split_4___pvcn_140008__dpitchm1_byte_offset 	188	/* 0x000000bc */
#define flexidag_openseg_prim_split_4___pvcn_140008__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_4___pvcn_140008__W 	544
#define flexidag_openseg_prim_split_4___pvcn_140008__H 	1
#define flexidag_openseg_prim_split_4___pvcn_140008__D 	1
#define flexidag_openseg_prim_split_4___pvcn_140008__P 	1
#define flexidag_openseg_prim_split_4___pvcn_140008__data_num_bytes 	1
#define flexidag_openseg_prim_split_4___pvcn_140008__denable 	1
#define flexidag_openseg_prim_split_4___pvcn_140008__dpitch_num_bytes 	544
#define flexidag_openseg_prim_split_4___pvcn_140008__dram_format 	0
#define flexidag_openseg_prim_split_4___pvcn_140008__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_4___pvcn_140008__tile_width 	1
#define flexidag_openseg_prim_split_4___pvcn_140008__tile_height 	1
#define flexidag_openseg_prim_split_4___pvcn_140008__vmem_buffer_num_bytes 	544
#define flexidag_openseg_prim_split_4___pvcn_140008__dram_size_num_bytes 	544

/* flexidag_openseg_prim_split_4 SMB_input __pvcn_1214_ */
#define flexidag_openseg_prim_split_4___pvcn_1214__cnngen_demangled_name 	"__pvcn_1214_"
#define flexidag_openseg_prim_split_4___pvcn_1214__is_constant 	1
#define flexidag_openseg_prim_split_4___pvcn_1214__has_init_data 	1
#define flexidag_openseg_prim_split_4___pvcn_1214__is_variable_scalar 	0
#define flexidag_openseg_prim_split_4___pvcn_1214__is_bitvector 	0
#define flexidag_openseg_prim_split_4___pvcn_1214__is_rlz 	0
#define flexidag_openseg_prim_split_4___pvcn_1214__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_4___pvcn_1214__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_4___pvcn_1214__byte_offset 	196	/* 0x000000c4 */
#define flexidag_openseg_prim_split_4___pvcn_1214__vbase_byte_offset 	200	/* 0x000000c8 */
#define flexidag_openseg_prim_split_4___pvcn_1214__dbase_byte_offset 	224	/* 0x000000e0 */
#define flexidag_openseg_prim_split_4___pvcn_1214__dpitchm1_byte_offset 	228	/* 0x000000e4 */
#define flexidag_openseg_prim_split_4___pvcn_1214__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_4___pvcn_1214__W 	5732
#define flexidag_openseg_prim_split_4___pvcn_1214__H 	1
#define flexidag_openseg_prim_split_4___pvcn_1214__D 	1
#define flexidag_openseg_prim_split_4___pvcn_1214__P 	1
#define flexidag_openseg_prim_split_4___pvcn_1214__data_num_bytes 	1
#define flexidag_openseg_prim_split_4___pvcn_1214__denable 	1
#define flexidag_openseg_prim_split_4___pvcn_1214__dpitch_num_bytes 	5760
#define flexidag_openseg_prim_split_4___pvcn_1214__dram_format 	0
#define flexidag_openseg_prim_split_4___pvcn_1214__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_4___pvcn_1214__tile_width 	1
#define flexidag_openseg_prim_split_4___pvcn_1214__tile_height 	1
#define flexidag_openseg_prim_split_4___pvcn_1214__vmem_buffer_num_bytes 	5732
#define flexidag_openseg_prim_split_4___pvcn_1214__dram_size_num_bytes 	5760

/* flexidag_openseg_prim_split_4 SMB_input __pvcn_1215_ */
#define flexidag_openseg_prim_split_4___pvcn_1215__cnngen_demangled_name 	"__pvcn_1215_"
#define flexidag_openseg_prim_split_4___pvcn_1215__is_constant 	1
#define flexidag_openseg_prim_split_4___pvcn_1215__has_init_data 	1
#define flexidag_openseg_prim_split_4___pvcn_1215__is_variable_scalar 	0
#define flexidag_openseg_prim_split_4___pvcn_1215__is_bitvector 	0
#define flexidag_openseg_prim_split_4___pvcn_1215__is_rlz 	0
#define flexidag_openseg_prim_split_4___pvcn_1215__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_4___pvcn_1215__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_4___pvcn_1215__byte_offset 	236	/* 0x000000ec */
#define flexidag_openseg_prim_split_4___pvcn_1215__vbase_byte_offset 	240	/* 0x000000f0 */
#define flexidag_openseg_prim_split_4___pvcn_1215__dbase_byte_offset 	264	/* 0x00000108 */
#define flexidag_openseg_prim_split_4___pvcn_1215__dpitchm1_byte_offset 	268	/* 0x0000010c */
#define flexidag_openseg_prim_split_4___pvcn_1215__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_4___pvcn_1215__W 	144
#define flexidag_openseg_prim_split_4___pvcn_1215__H 	1
#define flexidag_openseg_prim_split_4___pvcn_1215__D 	1
#define flexidag_openseg_prim_split_4___pvcn_1215__P 	1
#define flexidag_openseg_prim_split_4___pvcn_1215__data_num_bytes 	1
#define flexidag_openseg_prim_split_4___pvcn_1215__denable 	1
#define flexidag_openseg_prim_split_4___pvcn_1215__dpitch_num_bytes 	160
#define flexidag_openseg_prim_split_4___pvcn_1215__dram_format 	0
#define flexidag_openseg_prim_split_4___pvcn_1215__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_4___pvcn_1215__tile_width 	1
#define flexidag_openseg_prim_split_4___pvcn_1215__tile_height 	1
#define flexidag_openseg_prim_split_4___pvcn_1215__vmem_buffer_num_bytes 	144
#define flexidag_openseg_prim_split_4___pvcn_1215__dram_size_num_bytes 	160

/* flexidag_openseg_prim_split_4 SMB_input __pvcn_1217_ */
#define flexidag_openseg_prim_split_4___pvcn_1217__cnngen_demangled_name 	"__pvcn_1217_"
#define flexidag_openseg_prim_split_4___pvcn_1217__is_constant 	1
#define flexidag_openseg_prim_split_4___pvcn_1217__has_init_data 	1
#define flexidag_openseg_prim_split_4___pvcn_1217__is_variable_scalar 	0
#define flexidag_openseg_prim_split_4___pvcn_1217__is_bitvector 	0
#define flexidag_openseg_prim_split_4___pvcn_1217__is_rlz 	0
#define flexidag_openseg_prim_split_4___pvcn_1217__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_4___pvcn_1217__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_4___pvcn_1217__byte_offset 	276	/* 0x00000114 */
#define flexidag_openseg_prim_split_4___pvcn_1217__vbase_byte_offset 	280	/* 0x00000118 */
#define flexidag_openseg_prim_split_4___pvcn_1217__dbase_byte_offset 	304	/* 0x00000130 */
#define flexidag_openseg_prim_split_4___pvcn_1217__dpitchm1_byte_offset 	308	/* 0x00000134 */
#define flexidag_openseg_prim_split_4___pvcn_1217__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_4___pvcn_1217__W 	1420
#define flexidag_openseg_prim_split_4___pvcn_1217__H 	1
#define flexidag_openseg_prim_split_4___pvcn_1217__D 	1
#define flexidag_openseg_prim_split_4___pvcn_1217__P 	1
#define flexidag_openseg_prim_split_4___pvcn_1217__data_num_bytes 	1
#define flexidag_openseg_prim_split_4___pvcn_1217__denable 	1
#define flexidag_openseg_prim_split_4___pvcn_1217__dpitch_num_bytes 	1440
#define flexidag_openseg_prim_split_4___pvcn_1217__dram_format 	0
#define flexidag_openseg_prim_split_4___pvcn_1217__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_4___pvcn_1217__tile_width 	1
#define flexidag_openseg_prim_split_4___pvcn_1217__tile_height 	1
#define flexidag_openseg_prim_split_4___pvcn_1217__vmem_buffer_num_bytes 	1420
#define flexidag_openseg_prim_split_4___pvcn_1217__dram_size_num_bytes 	1440

/* flexidag_openseg_prim_split_4 SMB_input __pvcn_1218_ */
#define flexidag_openseg_prim_split_4___pvcn_1218__cnngen_demangled_name 	"__pvcn_1218_"
#define flexidag_openseg_prim_split_4___pvcn_1218__is_constant 	1
#define flexidag_openseg_prim_split_4___pvcn_1218__has_init_data 	1
#define flexidag_openseg_prim_split_4___pvcn_1218__is_variable_scalar 	0
#define flexidag_openseg_prim_split_4___pvcn_1218__is_bitvector 	0
#define flexidag_openseg_prim_split_4___pvcn_1218__is_rlz 	0
#define flexidag_openseg_prim_split_4___pvcn_1218__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_4___pvcn_1218__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_4___pvcn_1218__byte_offset 	316	/* 0x0000013c */
#define flexidag_openseg_prim_split_4___pvcn_1218__vbase_byte_offset 	320	/* 0x00000140 */
#define flexidag_openseg_prim_split_4___pvcn_1218__dbase_byte_offset 	344	/* 0x00000158 */
#define flexidag_openseg_prim_split_4___pvcn_1218__dpitchm1_byte_offset 	348	/* 0x0000015c */
#define flexidag_openseg_prim_split_4___pvcn_1218__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_4___pvcn_1218__W 	144
#define flexidag_openseg_prim_split_4___pvcn_1218__H 	1
#define flexidag_openseg_prim_split_4___pvcn_1218__D 	1
#define flexidag_openseg_prim_split_4___pvcn_1218__P 	1
#define flexidag_openseg_prim_split_4___pvcn_1218__data_num_bytes 	1
#define flexidag_openseg_prim_split_4___pvcn_1218__denable 	1
#define flexidag_openseg_prim_split_4___pvcn_1218__dpitch_num_bytes 	160
#define flexidag_openseg_prim_split_4___pvcn_1218__dram_format 	0
#define flexidag_openseg_prim_split_4___pvcn_1218__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_4___pvcn_1218__tile_width 	1
#define flexidag_openseg_prim_split_4___pvcn_1218__tile_height 	1
#define flexidag_openseg_prim_split_4___pvcn_1218__vmem_buffer_num_bytes 	144
#define flexidag_openseg_prim_split_4___pvcn_1218__dram_size_num_bytes 	160

/* flexidag_openseg_prim_split_4 SMB_input __pvcn_1221_ */
#define flexidag_openseg_prim_split_4___pvcn_1221__cnngen_demangled_name 	"__pvcn_1221_"
#define flexidag_openseg_prim_split_4___pvcn_1221__is_constant 	1
#define flexidag_openseg_prim_split_4___pvcn_1221__has_init_data 	1
#define flexidag_openseg_prim_split_4___pvcn_1221__is_variable_scalar 	0
#define flexidag_openseg_prim_split_4___pvcn_1221__is_bitvector 	0
#define flexidag_openseg_prim_split_4___pvcn_1221__is_rlz 	0
#define flexidag_openseg_prim_split_4___pvcn_1221__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_4___pvcn_1221__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_4___pvcn_1221__byte_offset 	356	/* 0x00000164 */
#define flexidag_openseg_prim_split_4___pvcn_1221__vbase_byte_offset 	360	/* 0x00000168 */
#define flexidag_openseg_prim_split_4___pvcn_1221__dbase_byte_offset 	384	/* 0x00000180 */
#define flexidag_openseg_prim_split_4___pvcn_1221__dpitchm1_byte_offset 	388	/* 0x00000184 */
#define flexidag_openseg_prim_split_4___pvcn_1221__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_4___pvcn_1221__W 	5756
#define flexidag_openseg_prim_split_4___pvcn_1221__H 	1
#define flexidag_openseg_prim_split_4___pvcn_1221__D 	1
#define flexidag_openseg_prim_split_4___pvcn_1221__P 	1
#define flexidag_openseg_prim_split_4___pvcn_1221__data_num_bytes 	1
#define flexidag_openseg_prim_split_4___pvcn_1221__denable 	1
#define flexidag_openseg_prim_split_4___pvcn_1221__dpitch_num_bytes 	5760
#define flexidag_openseg_prim_split_4___pvcn_1221__dram_format 	0
#define flexidag_openseg_prim_split_4___pvcn_1221__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_4___pvcn_1221__tile_width 	1
#define flexidag_openseg_prim_split_4___pvcn_1221__tile_height 	1
#define flexidag_openseg_prim_split_4___pvcn_1221__vmem_buffer_num_bytes 	5756
#define flexidag_openseg_prim_split_4___pvcn_1221__dram_size_num_bytes 	5760

/* flexidag_openseg_prim_split_4 SMB_input __pvcn_1222_ */
#define flexidag_openseg_prim_split_4___pvcn_1222__cnngen_demangled_name 	"__pvcn_1222_"
#define flexidag_openseg_prim_split_4___pvcn_1222__is_constant 	1
#define flexidag_openseg_prim_split_4___pvcn_1222__has_init_data 	1
#define flexidag_openseg_prim_split_4___pvcn_1222__is_variable_scalar 	0
#define flexidag_openseg_prim_split_4___pvcn_1222__is_bitvector 	0
#define flexidag_openseg_prim_split_4___pvcn_1222__is_rlz 	0
#define flexidag_openseg_prim_split_4___pvcn_1222__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_4___pvcn_1222__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_4___pvcn_1222__byte_offset 	396	/* 0x0000018c */
#define flexidag_openseg_prim_split_4___pvcn_1222__vbase_byte_offset 	400	/* 0x00000190 */
#define flexidag_openseg_prim_split_4___pvcn_1222__dbase_byte_offset 	424	/* 0x000001a8 */
#define flexidag_openseg_prim_split_4___pvcn_1222__dpitchm1_byte_offset 	428	/* 0x000001ac */
#define flexidag_openseg_prim_split_4___pvcn_1222__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_4___pvcn_1222__W 	144
#define flexidag_openseg_prim_split_4___pvcn_1222__H 	1
#define flexidag_openseg_prim_split_4___pvcn_1222__D 	1
#define flexidag_openseg_prim_split_4___pvcn_1222__P 	1
#define flexidag_openseg_prim_split_4___pvcn_1222__data_num_bytes 	1
#define flexidag_openseg_prim_split_4___pvcn_1222__denable 	1
#define flexidag_openseg_prim_split_4___pvcn_1222__dpitch_num_bytes 	160
#define flexidag_openseg_prim_split_4___pvcn_1222__dram_format 	0
#define flexidag_openseg_prim_split_4___pvcn_1222__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_4___pvcn_1222__tile_width 	1
#define flexidag_openseg_prim_split_4___pvcn_1222__tile_height 	1
#define flexidag_openseg_prim_split_4___pvcn_1222__vmem_buffer_num_bytes 	144
#define flexidag_openseg_prim_split_4___pvcn_1222__dram_size_num_bytes 	160

/* flexidag_openseg_prim_split_4 SMB_input __pvcn_1228_ */
#define flexidag_openseg_prim_split_4___pvcn_1228__cnngen_demangled_name 	"__pvcn_1228_"
#define flexidag_openseg_prim_split_4___pvcn_1228__is_constant 	1
#define flexidag_openseg_prim_split_4___pvcn_1228__has_init_data 	1
#define flexidag_openseg_prim_split_4___pvcn_1228__is_variable_scalar 	0
#define flexidag_openseg_prim_split_4___pvcn_1228__is_bitvector 	0
#define flexidag_openseg_prim_split_4___pvcn_1228__is_rlz 	0
#define flexidag_openseg_prim_split_4___pvcn_1228__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_4___pvcn_1228__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_4___pvcn_1228__byte_offset 	436	/* 0x000001b4 */
#define flexidag_openseg_prim_split_4___pvcn_1228__vbase_byte_offset 	440	/* 0x000001b8 */
#define flexidag_openseg_prim_split_4___pvcn_1228__dbase_byte_offset 	464	/* 0x000001d0 */
#define flexidag_openseg_prim_split_4___pvcn_1228__dpitchm1_byte_offset 	468	/* 0x000001d4 */
#define flexidag_openseg_prim_split_4___pvcn_1228__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_4___pvcn_1228__W 	35508
#define flexidag_openseg_prim_split_4___pvcn_1228__H 	1
#define flexidag_openseg_prim_split_4___pvcn_1228__D 	1
#define flexidag_openseg_prim_split_4___pvcn_1228__P 	1
#define flexidag_openseg_prim_split_4___pvcn_1228__data_num_bytes 	1
#define flexidag_openseg_prim_split_4___pvcn_1228__denable 	1
#define flexidag_openseg_prim_split_4___pvcn_1228__dpitch_num_bytes 	35520
#define flexidag_openseg_prim_split_4___pvcn_1228__dram_format 	0
#define flexidag_openseg_prim_split_4___pvcn_1228__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_4___pvcn_1228__tile_width 	1
#define flexidag_openseg_prim_split_4___pvcn_1228__tile_height 	1
#define flexidag_openseg_prim_split_4___pvcn_1228__vmem_buffer_num_bytes 	35508
#define flexidag_openseg_prim_split_4___pvcn_1228__dram_size_num_bytes 	35520

/* flexidag_openseg_prim_split_4 SMB_input __pvcn_1229_ */
#define flexidag_openseg_prim_split_4___pvcn_1229__cnngen_demangled_name 	"__pvcn_1229_"
#define flexidag_openseg_prim_split_4___pvcn_1229__is_constant 	1
#define flexidag_openseg_prim_split_4___pvcn_1229__has_init_data 	1
#define flexidag_openseg_prim_split_4___pvcn_1229__is_variable_scalar 	0
#define flexidag_openseg_prim_split_4___pvcn_1229__is_bitvector 	0
#define flexidag_openseg_prim_split_4___pvcn_1229__is_rlz 	0
#define flexidag_openseg_prim_split_4___pvcn_1229__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_4___pvcn_1229__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_4___pvcn_1229__byte_offset 	476	/* 0x000001dc */
#define flexidag_openseg_prim_split_4___pvcn_1229__vbase_byte_offset 	480	/* 0x000001e0 */
#define flexidag_openseg_prim_split_4___pvcn_1229__dbase_byte_offset 	504	/* 0x000001f8 */
#define flexidag_openseg_prim_split_4___pvcn_1229__dpitchm1_byte_offset 	508	/* 0x000001fc */
#define flexidag_openseg_prim_split_4___pvcn_1229__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_4___pvcn_1229__W 	1088
#define flexidag_openseg_prim_split_4___pvcn_1229__H 	1
#define flexidag_openseg_prim_split_4___pvcn_1229__D 	1
#define flexidag_openseg_prim_split_4___pvcn_1229__P 	1
#define flexidag_openseg_prim_split_4___pvcn_1229__data_num_bytes 	1
#define flexidag_openseg_prim_split_4___pvcn_1229__denable 	1
#define flexidag_openseg_prim_split_4___pvcn_1229__dpitch_num_bytes 	1088
#define flexidag_openseg_prim_split_4___pvcn_1229__dram_format 	0
#define flexidag_openseg_prim_split_4___pvcn_1229__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_4___pvcn_1229__tile_width 	1
#define flexidag_openseg_prim_split_4___pvcn_1229__tile_height 	1
#define flexidag_openseg_prim_split_4___pvcn_1229__vmem_buffer_num_bytes 	1088
#define flexidag_openseg_prim_split_4___pvcn_1229__dram_size_num_bytes 	1088

/* flexidag_openseg_prim_split_4 SMB_input __pvcn_1232_ */
#define flexidag_openseg_prim_split_4___pvcn_1232__cnngen_demangled_name 	"__pvcn_1232_"
#define flexidag_openseg_prim_split_4___pvcn_1232__is_constant 	1
#define flexidag_openseg_prim_split_4___pvcn_1232__has_init_data 	1
#define flexidag_openseg_prim_split_4___pvcn_1232__is_variable_scalar 	0
#define flexidag_openseg_prim_split_4___pvcn_1232__is_bitvector 	0
#define flexidag_openseg_prim_split_4___pvcn_1232__is_rlz 	0
#define flexidag_openseg_prim_split_4___pvcn_1232__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_4___pvcn_1232__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_4___pvcn_1232__byte_offset 	516	/* 0x00000204 */
#define flexidag_openseg_prim_split_4___pvcn_1232__vbase_byte_offset 	520	/* 0x00000208 */
#define flexidag_openseg_prim_split_4___pvcn_1232__dbase_byte_offset 	544	/* 0x00000220 */
#define flexidag_openseg_prim_split_4___pvcn_1232__dpitchm1_byte_offset 	548	/* 0x00000224 */
#define flexidag_openseg_prim_split_4___pvcn_1232__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_4___pvcn_1232__W 	5704
#define flexidag_openseg_prim_split_4___pvcn_1232__H 	1
#define flexidag_openseg_prim_split_4___pvcn_1232__D 	1
#define flexidag_openseg_prim_split_4___pvcn_1232__P 	1
#define flexidag_openseg_prim_split_4___pvcn_1232__data_num_bytes 	1
#define flexidag_openseg_prim_split_4___pvcn_1232__denable 	1
#define flexidag_openseg_prim_split_4___pvcn_1232__dpitch_num_bytes 	5728
#define flexidag_openseg_prim_split_4___pvcn_1232__dram_format 	0
#define flexidag_openseg_prim_split_4___pvcn_1232__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_4___pvcn_1232__tile_width 	1
#define flexidag_openseg_prim_split_4___pvcn_1232__tile_height 	1
#define flexidag_openseg_prim_split_4___pvcn_1232__vmem_buffer_num_bytes 	5704
#define flexidag_openseg_prim_split_4___pvcn_1232__dram_size_num_bytes 	5728

/* flexidag_openseg_prim_split_4 SMB_input __pvcn_1233_ */
#define flexidag_openseg_prim_split_4___pvcn_1233__cnngen_demangled_name 	"__pvcn_1233_"
#define flexidag_openseg_prim_split_4___pvcn_1233__is_constant 	1
#define flexidag_openseg_prim_split_4___pvcn_1233__has_init_data 	1
#define flexidag_openseg_prim_split_4___pvcn_1233__is_variable_scalar 	0
#define flexidag_openseg_prim_split_4___pvcn_1233__is_bitvector 	0
#define flexidag_openseg_prim_split_4___pvcn_1233__is_rlz 	0
#define flexidag_openseg_prim_split_4___pvcn_1233__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_4___pvcn_1233__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_4___pvcn_1233__byte_offset 	556	/* 0x0000022c */
#define flexidag_openseg_prim_split_4___pvcn_1233__vbase_byte_offset 	560	/* 0x00000230 */
#define flexidag_openseg_prim_split_4___pvcn_1233__dbase_byte_offset 	584	/* 0x00000248 */
#define flexidag_openseg_prim_split_4___pvcn_1233__dpitchm1_byte_offset 	588	/* 0x0000024c */
#define flexidag_openseg_prim_split_4___pvcn_1233__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_4___pvcn_1233__W 	136
#define flexidag_openseg_prim_split_4___pvcn_1233__H 	1
#define flexidag_openseg_prim_split_4___pvcn_1233__D 	1
#define flexidag_openseg_prim_split_4___pvcn_1233__P 	1
#define flexidag_openseg_prim_split_4___pvcn_1233__data_num_bytes 	1
#define flexidag_openseg_prim_split_4___pvcn_1233__denable 	1
#define flexidag_openseg_prim_split_4___pvcn_1233__dpitch_num_bytes 	160
#define flexidag_openseg_prim_split_4___pvcn_1233__dram_format 	0
#define flexidag_openseg_prim_split_4___pvcn_1233__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_4___pvcn_1233__tile_width 	1
#define flexidag_openseg_prim_split_4___pvcn_1233__tile_height 	1
#define flexidag_openseg_prim_split_4___pvcn_1233__vmem_buffer_num_bytes 	136
#define flexidag_openseg_prim_split_4___pvcn_1233__dram_size_num_bytes 	160

/* flexidag_openseg_prim_split_4 SMB_input __pvcn_1235_ */
#define flexidag_openseg_prim_split_4___pvcn_1235__cnngen_demangled_name 	"__pvcn_1235_"
#define flexidag_openseg_prim_split_4___pvcn_1235__is_constant 	1
#define flexidag_openseg_prim_split_4___pvcn_1235__has_init_data 	1
#define flexidag_openseg_prim_split_4___pvcn_1235__is_variable_scalar 	0
#define flexidag_openseg_prim_split_4___pvcn_1235__is_bitvector 	0
#define flexidag_openseg_prim_split_4___pvcn_1235__is_rlz 	0
#define flexidag_openseg_prim_split_4___pvcn_1235__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_4___pvcn_1235__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_4___pvcn_1235__byte_offset 	596	/* 0x00000254 */
#define flexidag_openseg_prim_split_4___pvcn_1235__vbase_byte_offset 	600	/* 0x00000258 */
#define flexidag_openseg_prim_split_4___pvcn_1235__dbase_byte_offset 	624	/* 0x00000270 */
#define flexidag_openseg_prim_split_4___pvcn_1235__dpitchm1_byte_offset 	628	/* 0x00000274 */
#define flexidag_openseg_prim_split_4___pvcn_1235__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_4___pvcn_1235__W 	1376
#define flexidag_openseg_prim_split_4___pvcn_1235__H 	1
#define flexidag_openseg_prim_split_4___pvcn_1235__D 	1
#define flexidag_openseg_prim_split_4___pvcn_1235__P 	1
#define flexidag_openseg_prim_split_4___pvcn_1235__data_num_bytes 	1
#define flexidag_openseg_prim_split_4___pvcn_1235__denable 	1
#define flexidag_openseg_prim_split_4___pvcn_1235__dpitch_num_bytes 	1376
#define flexidag_openseg_prim_split_4___pvcn_1235__dram_format 	0
#define flexidag_openseg_prim_split_4___pvcn_1235__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_4___pvcn_1235__tile_width 	1
#define flexidag_openseg_prim_split_4___pvcn_1235__tile_height 	1
#define flexidag_openseg_prim_split_4___pvcn_1235__vmem_buffer_num_bytes 	1376
#define flexidag_openseg_prim_split_4___pvcn_1235__dram_size_num_bytes 	1376

/* flexidag_openseg_prim_split_4 SMB_input __pvcn_1236_ */
#define flexidag_openseg_prim_split_4___pvcn_1236__cnngen_demangled_name 	"__pvcn_1236_"
#define flexidag_openseg_prim_split_4___pvcn_1236__is_constant 	1
#define flexidag_openseg_prim_split_4___pvcn_1236__has_init_data 	1
#define flexidag_openseg_prim_split_4___pvcn_1236__is_variable_scalar 	0
#define flexidag_openseg_prim_split_4___pvcn_1236__is_bitvector 	0
#define flexidag_openseg_prim_split_4___pvcn_1236__is_rlz 	0
#define flexidag_openseg_prim_split_4___pvcn_1236__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_4___pvcn_1236__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_4___pvcn_1236__byte_offset 	636	/* 0x0000027c */
#define flexidag_openseg_prim_split_4___pvcn_1236__vbase_byte_offset 	640	/* 0x00000280 */
#define flexidag_openseg_prim_split_4___pvcn_1236__dbase_byte_offset 	664	/* 0x00000298 */
#define flexidag_openseg_prim_split_4___pvcn_1236__dpitchm1_byte_offset 	668	/* 0x0000029c */
#define flexidag_openseg_prim_split_4___pvcn_1236__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_4___pvcn_1236__W 	136
#define flexidag_openseg_prim_split_4___pvcn_1236__H 	1
#define flexidag_openseg_prim_split_4___pvcn_1236__D 	1
#define flexidag_openseg_prim_split_4___pvcn_1236__P 	1
#define flexidag_openseg_prim_split_4___pvcn_1236__data_num_bytes 	1
#define flexidag_openseg_prim_split_4___pvcn_1236__denable 	1
#define flexidag_openseg_prim_split_4___pvcn_1236__dpitch_num_bytes 	160
#define flexidag_openseg_prim_split_4___pvcn_1236__dram_format 	0
#define flexidag_openseg_prim_split_4___pvcn_1236__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_4___pvcn_1236__tile_width 	1
#define flexidag_openseg_prim_split_4___pvcn_1236__tile_height 	1
#define flexidag_openseg_prim_split_4___pvcn_1236__vmem_buffer_num_bytes 	136
#define flexidag_openseg_prim_split_4___pvcn_1236__dram_size_num_bytes 	160

/* flexidag_openseg_prim_split_4 SMB_input __pvcn_1239_ */
#define flexidag_openseg_prim_split_4___pvcn_1239__cnngen_demangled_name 	"__pvcn_1239_"
#define flexidag_openseg_prim_split_4___pvcn_1239__is_constant 	1
#define flexidag_openseg_prim_split_4___pvcn_1239__has_init_data 	1
#define flexidag_openseg_prim_split_4___pvcn_1239__is_variable_scalar 	0
#define flexidag_openseg_prim_split_4___pvcn_1239__is_bitvector 	0
#define flexidag_openseg_prim_split_4___pvcn_1239__is_rlz 	0
#define flexidag_openseg_prim_split_4___pvcn_1239__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_4___pvcn_1239__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_4___pvcn_1239__byte_offset 	676	/* 0x000002a4 */
#define flexidag_openseg_prim_split_4___pvcn_1239__vbase_byte_offset 	680	/* 0x000002a8 */
#define flexidag_openseg_prim_split_4___pvcn_1239__dbase_byte_offset 	704	/* 0x000002c0 */
#define flexidag_openseg_prim_split_4___pvcn_1239__dpitchm1_byte_offset 	708	/* 0x000002c4 */
#define flexidag_openseg_prim_split_4___pvcn_1239__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_4___pvcn_1239__W 	5772
#define flexidag_openseg_prim_split_4___pvcn_1239__H 	1
#define flexidag_openseg_prim_split_4___pvcn_1239__D 	1
#define flexidag_openseg_prim_split_4___pvcn_1239__P 	1
#define flexidag_openseg_prim_split_4___pvcn_1239__data_num_bytes 	1
#define flexidag_openseg_prim_split_4___pvcn_1239__denable 	1
#define flexidag_openseg_prim_split_4___pvcn_1239__dpitch_num_bytes 	5792
#define flexidag_openseg_prim_split_4___pvcn_1239__dram_format 	0
#define flexidag_openseg_prim_split_4___pvcn_1239__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_4___pvcn_1239__tile_width 	1
#define flexidag_openseg_prim_split_4___pvcn_1239__tile_height 	1
#define flexidag_openseg_prim_split_4___pvcn_1239__vmem_buffer_num_bytes 	5772
#define flexidag_openseg_prim_split_4___pvcn_1239__dram_size_num_bytes 	5792

/* flexidag_openseg_prim_split_4 SMB_input __pvcn_1240_ */
#define flexidag_openseg_prim_split_4___pvcn_1240__cnngen_demangled_name 	"__pvcn_1240_"
#define flexidag_openseg_prim_split_4___pvcn_1240__is_constant 	1
#define flexidag_openseg_prim_split_4___pvcn_1240__has_init_data 	1
#define flexidag_openseg_prim_split_4___pvcn_1240__is_variable_scalar 	0
#define flexidag_openseg_prim_split_4___pvcn_1240__is_bitvector 	0
#define flexidag_openseg_prim_split_4___pvcn_1240__is_rlz 	0
#define flexidag_openseg_prim_split_4___pvcn_1240__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_4___pvcn_1240__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_4___pvcn_1240__byte_offset 	716	/* 0x000002cc */
#define flexidag_openseg_prim_split_4___pvcn_1240__vbase_byte_offset 	720	/* 0x000002d0 */
#define flexidag_openseg_prim_split_4___pvcn_1240__dbase_byte_offset 	744	/* 0x000002e8 */
#define flexidag_openseg_prim_split_4___pvcn_1240__dpitchm1_byte_offset 	748	/* 0x000002ec */
#define flexidag_openseg_prim_split_4___pvcn_1240__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_4___pvcn_1240__W 	144
#define flexidag_openseg_prim_split_4___pvcn_1240__H 	1
#define flexidag_openseg_prim_split_4___pvcn_1240__D 	1
#define flexidag_openseg_prim_split_4___pvcn_1240__P 	1
#define flexidag_openseg_prim_split_4___pvcn_1240__data_num_bytes 	1
#define flexidag_openseg_prim_split_4___pvcn_1240__denable 	1
#define flexidag_openseg_prim_split_4___pvcn_1240__dpitch_num_bytes 	160
#define flexidag_openseg_prim_split_4___pvcn_1240__dram_format 	0
#define flexidag_openseg_prim_split_4___pvcn_1240__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_4___pvcn_1240__tile_width 	1
#define flexidag_openseg_prim_split_4___pvcn_1240__tile_height 	1
#define flexidag_openseg_prim_split_4___pvcn_1240__vmem_buffer_num_bytes 	144
#define flexidag_openseg_prim_split_4___pvcn_1240__dram_size_num_bytes 	160

/* flexidag_openseg_prim_split_4 SMB_input __pvcn_1246_ */
#define flexidag_openseg_prim_split_4___pvcn_1246__cnngen_demangled_name 	"__pvcn_1246_"
#define flexidag_openseg_prim_split_4___pvcn_1246__is_constant 	1
#define flexidag_openseg_prim_split_4___pvcn_1246__has_init_data 	1
#define flexidag_openseg_prim_split_4___pvcn_1246__is_variable_scalar 	0
#define flexidag_openseg_prim_split_4___pvcn_1246__is_bitvector 	0
#define flexidag_openseg_prim_split_4___pvcn_1246__is_rlz 	0
#define flexidag_openseg_prim_split_4___pvcn_1246__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_4___pvcn_1246__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_4___pvcn_1246__byte_offset 	756	/* 0x000002f4 */
#define flexidag_openseg_prim_split_4___pvcn_1246__vbase_byte_offset 	760	/* 0x000002f8 */
#define flexidag_openseg_prim_split_4___pvcn_1246__dbase_byte_offset 	784	/* 0x00000310 */
#define flexidag_openseg_prim_split_4___pvcn_1246__dpitchm1_byte_offset 	788	/* 0x00000314 */
#define flexidag_openseg_prim_split_4___pvcn_1246__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_4___pvcn_1246__W 	37900
#define flexidag_openseg_prim_split_4___pvcn_1246__H 	1
#define flexidag_openseg_prim_split_4___pvcn_1246__D 	1
#define flexidag_openseg_prim_split_4___pvcn_1246__P 	1
#define flexidag_openseg_prim_split_4___pvcn_1246__data_num_bytes 	1
#define flexidag_openseg_prim_split_4___pvcn_1246__denable 	1
#define flexidag_openseg_prim_split_4___pvcn_1246__dpitch_num_bytes 	37920
#define flexidag_openseg_prim_split_4___pvcn_1246__dram_format 	0
#define flexidag_openseg_prim_split_4___pvcn_1246__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_4___pvcn_1246__tile_width 	1
#define flexidag_openseg_prim_split_4___pvcn_1246__tile_height 	1
#define flexidag_openseg_prim_split_4___pvcn_1246__vmem_buffer_num_bytes 	37900
#define flexidag_openseg_prim_split_4___pvcn_1246__dram_size_num_bytes 	37920

/* flexidag_openseg_prim_split_4 SMB_input __pvcn_1247_ */
#define flexidag_openseg_prim_split_4___pvcn_1247__cnngen_demangled_name 	"__pvcn_1247_"
#define flexidag_openseg_prim_split_4___pvcn_1247__is_constant 	1
#define flexidag_openseg_prim_split_4___pvcn_1247__has_init_data 	1
#define flexidag_openseg_prim_split_4___pvcn_1247__is_variable_scalar 	0
#define flexidag_openseg_prim_split_4___pvcn_1247__is_bitvector 	0
#define flexidag_openseg_prim_split_4___pvcn_1247__is_rlz 	0
#define flexidag_openseg_prim_split_4___pvcn_1247__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_4___pvcn_1247__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_4___pvcn_1247__byte_offset 	796	/* 0x0000031c */
#define flexidag_openseg_prim_split_4___pvcn_1247__vbase_byte_offset 	800	/* 0x00000320 */
#define flexidag_openseg_prim_split_4___pvcn_1247__dbase_byte_offset 	824	/* 0x00000338 */
#define flexidag_openseg_prim_split_4___pvcn_1247__dpitchm1_byte_offset 	828	/* 0x0000033c */
#define flexidag_openseg_prim_split_4___pvcn_1247__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_4___pvcn_1247__W 	1088
#define flexidag_openseg_prim_split_4___pvcn_1247__H 	1
#define flexidag_openseg_prim_split_4___pvcn_1247__D 	1
#define flexidag_openseg_prim_split_4___pvcn_1247__P 	1
#define flexidag_openseg_prim_split_4___pvcn_1247__data_num_bytes 	1
#define flexidag_openseg_prim_split_4___pvcn_1247__denable 	1
#define flexidag_openseg_prim_split_4___pvcn_1247__dpitch_num_bytes 	1088
#define flexidag_openseg_prim_split_4___pvcn_1247__dram_format 	0
#define flexidag_openseg_prim_split_4___pvcn_1247__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_4___pvcn_1247__tile_width 	1
#define flexidag_openseg_prim_split_4___pvcn_1247__tile_height 	1
#define flexidag_openseg_prim_split_4___pvcn_1247__vmem_buffer_num_bytes 	1088
#define flexidag_openseg_prim_split_4___pvcn_1247__dram_size_num_bytes 	1088

/* flexidag_openseg_prim_split_4 SMB_input __pvcn_1250_ */
#define flexidag_openseg_prim_split_4___pvcn_1250__cnngen_demangled_name 	"__pvcn_1250_"
#define flexidag_openseg_prim_split_4___pvcn_1250__is_constant 	1
#define flexidag_openseg_prim_split_4___pvcn_1250__has_init_data 	1
#define flexidag_openseg_prim_split_4___pvcn_1250__is_variable_scalar 	0
#define flexidag_openseg_prim_split_4___pvcn_1250__is_bitvector 	0
#define flexidag_openseg_prim_split_4___pvcn_1250__is_rlz 	0
#define flexidag_openseg_prim_split_4___pvcn_1250__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_4___pvcn_1250__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_4___pvcn_1250__byte_offset 	836	/* 0x00000344 */
#define flexidag_openseg_prim_split_4___pvcn_1250__vbase_byte_offset 	840	/* 0x00000348 */
#define flexidag_openseg_prim_split_4___pvcn_1250__dbase_byte_offset 	864	/* 0x00000360 */
#define flexidag_openseg_prim_split_4___pvcn_1250__dpitchm1_byte_offset 	868	/* 0x00000364 */
#define flexidag_openseg_prim_split_4___pvcn_1250__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_4___pvcn_1250__W 	5716
#define flexidag_openseg_prim_split_4___pvcn_1250__H 	1
#define flexidag_openseg_prim_split_4___pvcn_1250__D 	1
#define flexidag_openseg_prim_split_4___pvcn_1250__P 	1
#define flexidag_openseg_prim_split_4___pvcn_1250__data_num_bytes 	1
#define flexidag_openseg_prim_split_4___pvcn_1250__denable 	1
#define flexidag_openseg_prim_split_4___pvcn_1250__dpitch_num_bytes 	5728
#define flexidag_openseg_prim_split_4___pvcn_1250__dram_format 	0
#define flexidag_openseg_prim_split_4___pvcn_1250__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_4___pvcn_1250__tile_width 	1
#define flexidag_openseg_prim_split_4___pvcn_1250__tile_height 	1
#define flexidag_openseg_prim_split_4___pvcn_1250__vmem_buffer_num_bytes 	5716
#define flexidag_openseg_prim_split_4___pvcn_1250__dram_size_num_bytes 	5728

/* flexidag_openseg_prim_split_4 SMB_input __pvcn_1251_ */
#define flexidag_openseg_prim_split_4___pvcn_1251__cnngen_demangled_name 	"__pvcn_1251_"
#define flexidag_openseg_prim_split_4___pvcn_1251__is_constant 	1
#define flexidag_openseg_prim_split_4___pvcn_1251__has_init_data 	1
#define flexidag_openseg_prim_split_4___pvcn_1251__is_variable_scalar 	0
#define flexidag_openseg_prim_split_4___pvcn_1251__is_bitvector 	0
#define flexidag_openseg_prim_split_4___pvcn_1251__is_rlz 	0
#define flexidag_openseg_prim_split_4___pvcn_1251__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_4___pvcn_1251__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_4___pvcn_1251__byte_offset 	876	/* 0x0000036c */
#define flexidag_openseg_prim_split_4___pvcn_1251__vbase_byte_offset 	880	/* 0x00000370 */
#define flexidag_openseg_prim_split_4___pvcn_1251__dbase_byte_offset 	904	/* 0x00000388 */
#define flexidag_openseg_prim_split_4___pvcn_1251__dpitchm1_byte_offset 	908	/* 0x0000038c */
#define flexidag_openseg_prim_split_4___pvcn_1251__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_4___pvcn_1251__W 	144
#define flexidag_openseg_prim_split_4___pvcn_1251__H 	1
#define flexidag_openseg_prim_split_4___pvcn_1251__D 	1
#define flexidag_openseg_prim_split_4___pvcn_1251__P 	1
#define flexidag_openseg_prim_split_4___pvcn_1251__data_num_bytes 	1
#define flexidag_openseg_prim_split_4___pvcn_1251__denable 	1
#define flexidag_openseg_prim_split_4___pvcn_1251__dpitch_num_bytes 	160
#define flexidag_openseg_prim_split_4___pvcn_1251__dram_format 	0
#define flexidag_openseg_prim_split_4___pvcn_1251__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_4___pvcn_1251__tile_width 	1
#define flexidag_openseg_prim_split_4___pvcn_1251__tile_height 	1
#define flexidag_openseg_prim_split_4___pvcn_1251__vmem_buffer_num_bytes 	144
#define flexidag_openseg_prim_split_4___pvcn_1251__dram_size_num_bytes 	160

/* flexidag_openseg_prim_split_4 SMB_input __pvcn_1253_ */
#define flexidag_openseg_prim_split_4___pvcn_1253__cnngen_demangled_name 	"__pvcn_1253_"
#define flexidag_openseg_prim_split_4___pvcn_1253__is_constant 	1
#define flexidag_openseg_prim_split_4___pvcn_1253__has_init_data 	1
#define flexidag_openseg_prim_split_4___pvcn_1253__is_variable_scalar 	0
#define flexidag_openseg_prim_split_4___pvcn_1253__is_bitvector 	0
#define flexidag_openseg_prim_split_4___pvcn_1253__is_rlz 	0
#define flexidag_openseg_prim_split_4___pvcn_1253__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_4___pvcn_1253__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_4___pvcn_1253__byte_offset 	916	/* 0x00000394 */
#define flexidag_openseg_prim_split_4___pvcn_1253__vbase_byte_offset 	920	/* 0x00000398 */
#define flexidag_openseg_prim_split_4___pvcn_1253__dbase_byte_offset 	944	/* 0x000003b0 */
#define flexidag_openseg_prim_split_4___pvcn_1253__dpitchm1_byte_offset 	948	/* 0x000003b4 */
#define flexidag_openseg_prim_split_4___pvcn_1253__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_4___pvcn_1253__W 	1436
#define flexidag_openseg_prim_split_4___pvcn_1253__H 	1
#define flexidag_openseg_prim_split_4___pvcn_1253__D 	1
#define flexidag_openseg_prim_split_4___pvcn_1253__P 	1
#define flexidag_openseg_prim_split_4___pvcn_1253__data_num_bytes 	1
#define flexidag_openseg_prim_split_4___pvcn_1253__denable 	1
#define flexidag_openseg_prim_split_4___pvcn_1253__dpitch_num_bytes 	1440
#define flexidag_openseg_prim_split_4___pvcn_1253__dram_format 	0
#define flexidag_openseg_prim_split_4___pvcn_1253__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_4___pvcn_1253__tile_width 	1
#define flexidag_openseg_prim_split_4___pvcn_1253__tile_height 	1
#define flexidag_openseg_prim_split_4___pvcn_1253__vmem_buffer_num_bytes 	1436
#define flexidag_openseg_prim_split_4___pvcn_1253__dram_size_num_bytes 	1440

/* flexidag_openseg_prim_split_4 SMB_input __pvcn_1254_ */
#define flexidag_openseg_prim_split_4___pvcn_1254__cnngen_demangled_name 	"__pvcn_1254_"
#define flexidag_openseg_prim_split_4___pvcn_1254__is_constant 	1
#define flexidag_openseg_prim_split_4___pvcn_1254__has_init_data 	1
#define flexidag_openseg_prim_split_4___pvcn_1254__is_variable_scalar 	0
#define flexidag_openseg_prim_split_4___pvcn_1254__is_bitvector 	0
#define flexidag_openseg_prim_split_4___pvcn_1254__is_rlz 	0
#define flexidag_openseg_prim_split_4___pvcn_1254__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_4___pvcn_1254__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_4___pvcn_1254__byte_offset 	956	/* 0x000003bc */
#define flexidag_openseg_prim_split_4___pvcn_1254__vbase_byte_offset 	960	/* 0x000003c0 */
#define flexidag_openseg_prim_split_4___pvcn_1254__dbase_byte_offset 	984	/* 0x000003d8 */
#define flexidag_openseg_prim_split_4___pvcn_1254__dpitchm1_byte_offset 	988	/* 0x000003dc */
#define flexidag_openseg_prim_split_4___pvcn_1254__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_4___pvcn_1254__W 	136
#define flexidag_openseg_prim_split_4___pvcn_1254__H 	1
#define flexidag_openseg_prim_split_4___pvcn_1254__D 	1
#define flexidag_openseg_prim_split_4___pvcn_1254__P 	1
#define flexidag_openseg_prim_split_4___pvcn_1254__data_num_bytes 	1
#define flexidag_openseg_prim_split_4___pvcn_1254__denable 	1
#define flexidag_openseg_prim_split_4___pvcn_1254__dpitch_num_bytes 	160
#define flexidag_openseg_prim_split_4___pvcn_1254__dram_format 	0
#define flexidag_openseg_prim_split_4___pvcn_1254__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_4___pvcn_1254__tile_width 	1
#define flexidag_openseg_prim_split_4___pvcn_1254__tile_height 	1
#define flexidag_openseg_prim_split_4___pvcn_1254__vmem_buffer_num_bytes 	136
#define flexidag_openseg_prim_split_4___pvcn_1254__dram_size_num_bytes 	160

/* flexidag_openseg_prim_split_4 SMB_input __pvcn_1257_ */
#define flexidag_openseg_prim_split_4___pvcn_1257__cnngen_demangled_name 	"__pvcn_1257_"
#define flexidag_openseg_prim_split_4___pvcn_1257__is_constant 	1
#define flexidag_openseg_prim_split_4___pvcn_1257__has_init_data 	1
#define flexidag_openseg_prim_split_4___pvcn_1257__is_variable_scalar 	0
#define flexidag_openseg_prim_split_4___pvcn_1257__is_bitvector 	0
#define flexidag_openseg_prim_split_4___pvcn_1257__is_rlz 	0
#define flexidag_openseg_prim_split_4___pvcn_1257__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_4___pvcn_1257__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_4___pvcn_1257__byte_offset 	996	/* 0x000003e4 */
#define flexidag_openseg_prim_split_4___pvcn_1257__vbase_byte_offset 	1000	/* 0x000003e8 */
#define flexidag_openseg_prim_split_4___pvcn_1257__dbase_byte_offset 	1024	/* 0x00000400 */
#define flexidag_openseg_prim_split_4___pvcn_1257__dpitchm1_byte_offset 	1028	/* 0x00000404 */
#define flexidag_openseg_prim_split_4___pvcn_1257__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_4___pvcn_1257__W 	5680
#define flexidag_openseg_prim_split_4___pvcn_1257__H 	1
#define flexidag_openseg_prim_split_4___pvcn_1257__D 	1
#define flexidag_openseg_prim_split_4___pvcn_1257__P 	1
#define flexidag_openseg_prim_split_4___pvcn_1257__data_num_bytes 	1
#define flexidag_openseg_prim_split_4___pvcn_1257__denable 	1
#define flexidag_openseg_prim_split_4___pvcn_1257__dpitch_num_bytes 	5696
#define flexidag_openseg_prim_split_4___pvcn_1257__dram_format 	0
#define flexidag_openseg_prim_split_4___pvcn_1257__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_4___pvcn_1257__tile_width 	1
#define flexidag_openseg_prim_split_4___pvcn_1257__tile_height 	1
#define flexidag_openseg_prim_split_4___pvcn_1257__vmem_buffer_num_bytes 	5680
#define flexidag_openseg_prim_split_4___pvcn_1257__dram_size_num_bytes 	5696

/* flexidag_openseg_prim_split_4 SMB_input __pvcn_1258_ */
#define flexidag_openseg_prim_split_4___pvcn_1258__cnngen_demangled_name 	"__pvcn_1258_"
#define flexidag_openseg_prim_split_4___pvcn_1258__is_constant 	1
#define flexidag_openseg_prim_split_4___pvcn_1258__has_init_data 	1
#define flexidag_openseg_prim_split_4___pvcn_1258__is_variable_scalar 	0
#define flexidag_openseg_prim_split_4___pvcn_1258__is_bitvector 	0
#define flexidag_openseg_prim_split_4___pvcn_1258__is_rlz 	0
#define flexidag_openseg_prim_split_4___pvcn_1258__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_4___pvcn_1258__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_4___pvcn_1258__byte_offset 	1036	/* 0x0000040c */
#define flexidag_openseg_prim_split_4___pvcn_1258__vbase_byte_offset 	1040	/* 0x00000410 */
#define flexidag_openseg_prim_split_4___pvcn_1258__dbase_byte_offset 	1064	/* 0x00000428 */
#define flexidag_openseg_prim_split_4___pvcn_1258__dpitchm1_byte_offset 	1068	/* 0x0000042c */
#define flexidag_openseg_prim_split_4___pvcn_1258__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_4___pvcn_1258__W 	144
#define flexidag_openseg_prim_split_4___pvcn_1258__H 	1
#define flexidag_openseg_prim_split_4___pvcn_1258__D 	1
#define flexidag_openseg_prim_split_4___pvcn_1258__P 	1
#define flexidag_openseg_prim_split_4___pvcn_1258__data_num_bytes 	1
#define flexidag_openseg_prim_split_4___pvcn_1258__denable 	1
#define flexidag_openseg_prim_split_4___pvcn_1258__dpitch_num_bytes 	160
#define flexidag_openseg_prim_split_4___pvcn_1258__dram_format 	0
#define flexidag_openseg_prim_split_4___pvcn_1258__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_4___pvcn_1258__tile_width 	1
#define flexidag_openseg_prim_split_4___pvcn_1258__tile_height 	1
#define flexidag_openseg_prim_split_4___pvcn_1258__vmem_buffer_num_bytes 	144
#define flexidag_openseg_prim_split_4___pvcn_1258__dram_size_num_bytes 	160

/* flexidag_openseg_prim_split_4 VCB PeleeNet__stage_3_dense_block_5__concat */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_5__concat_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_5__concat"
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_5__concat_is_bitvector 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_5__concat_is_rlz 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_5__concat_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_5__concat_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_5__concat_byte_offset 	1076	/* 0x00000434 */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_5__concat_vbase_byte_offset 	1084	/* 0x0000043c */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_5__concat_vmem_buffer_num_bytes 	293888

/* flexidag_openseg_prim_split_4 VCB PeleeNet__stage_3_dense_block_5__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_140001_ */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_5__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_140001__cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_5__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_140001_"
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_5__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_140001__is_bitvector 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_5__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_140001__is_rlz 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_5__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_140001__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_5__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_140001__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_5__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_140001__byte_offset 	1128	/* 0x00000468 */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_5__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_140001__vbase_byte_offset 	1136	/* 0x00000470 */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_5__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_140001__vmem_buffer_num_bytes 	21504

/* flexidag_openseg_prim_split_4 VCB PeleeNet__stage_3_dense_block_5__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_140002_ */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_5__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_140002__cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_5__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_140002_"
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_5__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_140002__is_bitvector 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_5__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_140002__is_rlz 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_5__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_140002__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_5__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_140002__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_5__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_140002__byte_offset 	1172	/* 0x00000494 */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_5__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_140002__vbase_byte_offset 	1180	/* 0x0000049c */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_5__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_140002__vmem_buffer_num_bytes 	21504

/* flexidag_openseg_prim_split_4 VCB PeleeNet__stage_3_dense_block_5__conv_right_0__Relu */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_5__conv_right_0__Relu_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_5__conv_right_0__Relu"
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_5__conv_right_0__Relu_is_bitvector 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_5__conv_right_0__Relu_is_rlz 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_5__conv_right_0__Relu_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_5__conv_right_0__Relu_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_5__conv_right_0__Relu_byte_offset 	1208	/* 0x000004b8 */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_5__conv_right_0__Relu_vbase_byte_offset 	1216	/* 0x000004c0 */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_5__conv_right_0__Relu_vmem_buffer_num_bytes 	43264

/* flexidag_openseg_prim_split_4 VCB PeleeNet__stage_3_dense_block_5__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_5__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_5__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu"
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_5__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_bitvector 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_5__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_rlz 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_5__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_5__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_5__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_byte_offset 	1256	/* 0x000004e8 */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_5__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vbase_byte_offset 	1264	/* 0x000004f0 */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_5__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vmem_buffer_num_bytes 	28032

/* flexidag_openseg_prim_split_4 VCB PeleeNet__stage_3_dense_block_5__conv_left_0__Relu */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_5__conv_left_0__Relu_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_5__conv_left_0__Relu"
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_5__conv_left_0__Relu_is_bitvector 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_5__conv_left_0__Relu_is_rlz 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_5__conv_left_0__Relu_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_5__conv_left_0__Relu_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_5__conv_left_0__Relu_byte_offset 	1324	/* 0x0000052c */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_5__conv_left_0__Relu_vbase_byte_offset 	1332	/* 0x00000534 */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_5__conv_left_0__Relu_vmem_buffer_num_bytes 	43264

/* flexidag_openseg_prim_split_4 VCB PeleeNet__stage_3_dense_block_6__concat */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_6__concat_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_6__concat"
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_6__concat_is_bitvector 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_6__concat_is_rlz 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_6__concat_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_6__concat_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_6__concat_byte_offset 	1400	/* 0x00000578 */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_6__concat_vbase_byte_offset 	1408	/* 0x00000580 */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_6__concat_vmem_buffer_num_bytes 	384000

/* flexidag_openseg_prim_split_4 VCB PeleeNet__stage_3_dense_block_6__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_6__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_6__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm"
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_6__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_is_bitvector 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_6__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_is_rlz 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_6__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_6__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_6__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_byte_offset 	1472	/* 0x000005c0 */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_6__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_vbase_byte_offset 	1480	/* 0x000005c8 */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_6__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_vmem_buffer_num_bytes 	83968

/* flexidag_openseg_prim_split_4 VCB PeleeNet__stage_3_dense_block_6__conv_right_0__Relu */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_6__conv_right_0__Relu_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_6__conv_right_0__Relu"
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_6__conv_right_0__Relu_is_bitvector 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_6__conv_right_0__Relu_is_rlz 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_6__conv_right_0__Relu_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_6__conv_right_0__Relu_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_6__conv_right_0__Relu_byte_offset 	1524	/* 0x000005f4 */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_6__conv_right_0__Relu_vbase_byte_offset 	1532	/* 0x000005fc */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_6__conv_right_0__Relu_vmem_buffer_num_bytes 	28160

/* flexidag_openseg_prim_split_4 VCB PeleeNet__stage_3_dense_block_6__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_6__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_6__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu"
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_6__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_bitvector 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_6__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_rlz 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_6__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_6__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_6__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_byte_offset 	1572	/* 0x00000624 */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_6__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vbase_byte_offset 	1580	/* 0x0000062c */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_6__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vmem_buffer_num_bytes 	14080

/* flexidag_openseg_prim_split_4 VCB PeleeNet__stage_3_dense_block_6__conv_left_0__Relu */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_6__conv_left_0__Relu_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_6__conv_left_0__Relu"
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_6__conv_left_0__Relu_is_bitvector 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_6__conv_left_0__Relu_is_rlz 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_6__conv_left_0__Relu_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_6__conv_left_0__Relu_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_6__conv_left_0__Relu_byte_offset 	1644	/* 0x0000066c */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_6__conv_left_0__Relu_vbase_byte_offset 	1652	/* 0x00000674 */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_6__conv_left_0__Relu_vmem_buffer_num_bytes 	43264

/* flexidag_openseg_prim_split_4 HMB_output PeleeNet__stage_3_dense_block_7__concat */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__concat_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_7__concat"
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__concat_is_main_input_output 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__concat_is_bitvector 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__concat_is_rlz 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__concat_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__concat_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__concat_byte_offset 	1700	/* 0x000006a4 */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__concat_vbase_byte_offset 	1704	/* 0x000006a8 */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__concat_dbase_byte_offset 	1728	/* 0x000006c0 */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__concat_dpitchm1_byte_offset 	1732	/* 0x000006c4 */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__concat_dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__concat_vwidth_minus_one_byte_offset 	1716	/* 0x000006b4 */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__concat_vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__concat_vheight_minus_one_byte_offset 	1718	/* 0x000006b6 */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__concat_vheight_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__concat_drotate_bit_offset 	13798	/* 0x000035e6 */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__concat_hflip_bit_offset 	13799	/* 0x000035e7 */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__concat_vflip_bit_offset 	13800	/* 0x000035e8 */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__concat_dflip_bit_offset 	13801	/* 0x000035e9 */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__concat_pflip_bit_offset 	13802	/* 0x000035ea */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__concat_W 	80
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__concat_H 	32
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__concat_D 	512
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__concat_P 	1
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__concat_data_num_bytes 	1
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__concat_denable 	1
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__concat_dpitch_num_bytes 	32
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__concat_dram_format 	3
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__concat_vp_interleave_mode 	2
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__concat_tile_width 	4
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__concat_tile_height 	1
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__concat_vmem_buffer_num_bytes 	344064
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__concat_dram_size_num_bytes 	1310720
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__concat_sign 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__concat_datasize 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__concat_expoffset 	5
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__concat_expbits 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__concat_drotate 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__concat_hflip 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__concat_vflip 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__concat_dflip 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__concat_pflip 	0

/* flexidag_openseg_prim_split_4 VCB PeleeNet__stage_3_dense_block_7__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_7__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm"
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_is_bitvector 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_is_rlz 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_byte_offset 	1772	/* 0x000006ec */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_vbase_byte_offset 	1780	/* 0x000006f4 */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_vmem_buffer_num_bytes 	124928

/* flexidag_openseg_prim_split_4 VCB PeleeNet__stage_3_dense_block_7__conv_right_0__Relu */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__conv_right_0__Relu_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_7__conv_right_0__Relu"
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__conv_right_0__Relu_is_bitvector 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__conv_right_0__Relu_is_rlz 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__conv_right_0__Relu_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__conv_right_0__Relu_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__conv_right_0__Relu_byte_offset 	1824	/* 0x00000720 */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__conv_right_0__Relu_vbase_byte_offset 	1832	/* 0x00000728 */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__conv_right_0__Relu_vmem_buffer_num_bytes 	43264

/* flexidag_openseg_prim_split_4 VCB PeleeNet__stage_3_dense_block_7__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_7__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu"
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_bitvector 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_rlz 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_byte_offset 	1872	/* 0x00000750 */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vbase_byte_offset 	1880	/* 0x00000758 */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vmem_buffer_num_bytes 	21504

/* flexidag_openseg_prim_split_4 VCB PeleeNet__stage_3_dense_block_7__conv_left_0__Relu */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__conv_left_0__Relu_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_7__conv_left_0__Relu"
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__conv_left_0__Relu_is_bitvector 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__conv_left_0__Relu_is_rlz 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__conv_left_0__Relu_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__conv_left_0__Relu_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__conv_left_0__Relu_byte_offset 	1944	/* 0x00000798 */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__conv_left_0__Relu_vbase_byte_offset 	1952	/* 0x000007a0 */
#define flexidag_openseg_prim_split_4_PeleeNet__stage_3_dense_block_7__conv_left_0__Relu_vmem_buffer_num_bytes 	43264

/* flexidag_openseg_prim_split_4 SMB_output split_4__vas_created_terminate__v0 */
#define flexidag_openseg_prim_split_4_split_4__vas_created_terminate__v0_cnngen_demangled_name 	"split_4__vas_created_terminate__v0"
#define flexidag_openseg_prim_split_4_split_4__vas_created_terminate__v0_is_main_input_output 	0
#define flexidag_openseg_prim_split_4_split_4__vas_created_terminate__v0_is_bitvector 	1
#define flexidag_openseg_prim_split_4_split_4__vas_created_terminate__v0_is_rlz 	0
#define flexidag_openseg_prim_split_4_split_4__vas_created_terminate__v0_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_4_split_4__vas_created_terminate__v0_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_4_split_4__vas_created_terminate__v0_byte_offset 	2004	/* 0x000007d4 */
#define flexidag_openseg_prim_split_4_split_4__vas_created_terminate__v0_vbase_byte_offset 	2008	/* 0x000007d8 */
#define flexidag_openseg_prim_split_4_split_4__vas_created_terminate__v0_W 	256
#define flexidag_openseg_prim_split_4_split_4__vas_created_terminate__v0_H 	3
#define flexidag_openseg_prim_split_4_split_4__vas_created_terminate__v0_D 	1
#define flexidag_openseg_prim_split_4_split_4__vas_created_terminate__v0_P 	1
#define flexidag_openseg_prim_split_4_split_4__vas_created_terminate__v0_data_num_bytes 	1
#define flexidag_openseg_prim_split_4_split_4__vas_created_terminate__v0_denable 	0
#define flexidag_openseg_prim_split_4_split_4__vas_created_terminate__v0_dram_format 	0
#define flexidag_openseg_prim_split_4_split_4__vas_created_terminate__v0_vp_interleave_mode 	1
#define flexidag_openseg_prim_split_4_split_4__vas_created_terminate__v0_tile_width 	1
#define flexidag_openseg_prim_split_4_split_4__vas_created_terminate__v0_tile_height 	1
#define flexidag_openseg_prim_split_4_split_4__vas_created_terminate__v0_vmem_buffer_num_bytes 	96


#endif /* flexidag_openseg_prim_split_4_H */
