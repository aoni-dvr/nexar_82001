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
#ifndef flexidag_openseg_prim_split_3_H
#define flexidag_openseg_prim_split_3_H

#define vas_gen_header_version                  	7

/* dag name */
#define flexidag_openseg_prim_split_3_DAG_NAME  	"flexidag_openseg_prim_split_3"

/* VDG file info */
#define flexidag_openseg_prim_split_3_vdg_name  	"flexidag_openseg_split_3.vdg"

/* VDG DAG memory info */
#define flexidag_openseg_prim_split_3_byte_offset 	4294967295	/* 0xffffffff */
#define flexidag_openseg_prim_split_3_byte_size 	2080	/* 0x00000820 */
#define flexidag_openseg_prim_split_3_preferred_dram_xfer_size 	1

/* flexidag_openseg_prim_split_3 VMEM info */
#define flexidag_openseg_prim_split_3_use_ping_pong_vmem 	0
#define flexidag_openseg_prim_split_3_VMEM_start 	4096	/* 0x00001000 */
#define flexidag_openseg_prim_split_3_VMEM_end  	2066996	/* 0x001f8a34 */
#define flexidag_openseg_prim_split_3_image_start 	1939296	/* 0x001d9760 */
#define flexidag_openseg_prim_split_3_image_size 	129780	/* 0x0001faf4 */
#define flexidag_openseg_prim_split_3_dagbin_start 	2066996	/* 0x001f8a34 */

/* flexidag_openseg_prim_split_3 DAG info */
#define flexidag_openseg_prim_split_3_estimated_cycles 	521713

/*******************************************************************************
 * DAG byte offset info for HMB/SMB/VCB and their fields
 * in DAG 'flexidag_openseg_prim_split_3' in source file 'flexidag_openseg_split_3.vas'
 ******************************************************************************/
/* flexidag_openseg_prim_split_3 HMB_input PeleeNet__stage_3_dense_block_1__concat */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_1__concat_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_1__concat"
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_1__concat_is_main_input_output 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_1__concat_is_constant 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_1__concat_has_init_data 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_1__concat_is_variable_scalar 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_1__concat_ddi_byte_size 	0	/* 0x00000000 */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_1__concat_ddi_byte_offset 	4294967295	/* 0xffffffff */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_1__concat_is_bitvector 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_1__concat_is_rlz 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_1__concat_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_1__concat_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_1__concat_byte_offset 	0	/* 0x00000000 */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_1__concat_vbase_byte_offset 	4	/* 0x00000004 */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_1__concat_dbase_byte_offset 	28	/* 0x0000001c */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_1__concat_dpitchm1_byte_offset 	32	/* 0x00000020 */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_1__concat_dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_1__concat_vwidth_minus_one_byte_offset 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_1__concat_vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_1__concat_vheight_minus_one_byte_offset 	18	/* 0x00000012 */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_1__concat_vheight_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_1__concat_drotate_bit_offset 	198	/* 0x000000c6 */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_1__concat_hflip_bit_offset 	199	/* 0x000000c7 */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_1__concat_vflip_bit_offset 	200	/* 0x000000c8 */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_1__concat_dflip_bit_offset 	201	/* 0x000000c9 */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_1__concat_pflip_bit_offset 	202	/* 0x000000ca */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_1__concat_W 	80
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_1__concat_H 	32
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_1__concat_D 	320
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_1__concat_P 	1
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_1__concat_data_num_bytes 	1
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_1__concat_denable 	1
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_1__concat_dpitch_num_bytes 	32
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_1__concat_dram_format 	3
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_1__concat_vp_interleave_mode 	2
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_1__concat_tile_width 	4
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_1__concat_tile_height 	1
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_1__concat_vmem_buffer_num_bytes 	312320
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_1__concat_dram_size_num_bytes 	819200
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_1__concat_sign 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_1__concat_datasize 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_1__concat_expoffset 	5
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_1__concat_expbits 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_1__concat_drotate 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_1__concat_hflip 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_1__concat_vflip 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_1__concat_dflip 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_1__concat_pflip 	0

/* flexidag_openseg_prim_split_3 SMB_input __pvcn_130004_ */
#define flexidag_openseg_prim_split_3___pvcn_130004__cnngen_demangled_name 	"__pvcn_130004_"
#define flexidag_openseg_prim_split_3___pvcn_130004__is_constant 	1
#define flexidag_openseg_prim_split_3___pvcn_130004__has_init_data 	1
#define flexidag_openseg_prim_split_3___pvcn_130004__is_variable_scalar 	0
#define flexidag_openseg_prim_split_3___pvcn_130004__is_bitvector 	0
#define flexidag_openseg_prim_split_3___pvcn_130004__is_rlz 	0
#define flexidag_openseg_prim_split_3___pvcn_130004__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_3___pvcn_130004__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_3___pvcn_130004__byte_offset 	40	/* 0x00000028 */
#define flexidag_openseg_prim_split_3___pvcn_130004__vbase_byte_offset 	44	/* 0x0000002c */
#define flexidag_openseg_prim_split_3___pvcn_130004__dbase_byte_offset 	68	/* 0x00000044 */
#define flexidag_openseg_prim_split_3___pvcn_130004__dpitchm1_byte_offset 	72	/* 0x00000048 */
#define flexidag_openseg_prim_split_3___pvcn_130004__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_3___pvcn_130004__W 	12852
#define flexidag_openseg_prim_split_3___pvcn_130004__H 	1
#define flexidag_openseg_prim_split_3___pvcn_130004__D 	1
#define flexidag_openseg_prim_split_3___pvcn_130004__P 	1
#define flexidag_openseg_prim_split_3___pvcn_130004__data_num_bytes 	1
#define flexidag_openseg_prim_split_3___pvcn_130004__denable 	1
#define flexidag_openseg_prim_split_3___pvcn_130004__dpitch_num_bytes 	12864
#define flexidag_openseg_prim_split_3___pvcn_130004__dram_format 	0
#define flexidag_openseg_prim_split_3___pvcn_130004__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_3___pvcn_130004__tile_width 	1
#define flexidag_openseg_prim_split_3___pvcn_130004__tile_height 	1
#define flexidag_openseg_prim_split_3___pvcn_130004__vmem_buffer_num_bytes 	12852
#define flexidag_openseg_prim_split_3___pvcn_130004__dram_size_num_bytes 	12864

/* flexidag_openseg_prim_split_3 SMB_input __pvcn_130007_ */
#define flexidag_openseg_prim_split_3___pvcn_130007__cnngen_demangled_name 	"__pvcn_130007_"
#define flexidag_openseg_prim_split_3___pvcn_130007__is_constant 	1
#define flexidag_openseg_prim_split_3___pvcn_130007__has_init_data 	1
#define flexidag_openseg_prim_split_3___pvcn_130007__is_variable_scalar 	0
#define flexidag_openseg_prim_split_3___pvcn_130007__is_bitvector 	0
#define flexidag_openseg_prim_split_3___pvcn_130007__is_rlz 	0
#define flexidag_openseg_prim_split_3___pvcn_130007__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_3___pvcn_130007__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_3___pvcn_130007__byte_offset 	76	/* 0x0000004c */
#define flexidag_openseg_prim_split_3___pvcn_130007__vbase_byte_offset 	80	/* 0x00000050 */
#define flexidag_openseg_prim_split_3___pvcn_130007__dbase_byte_offset 	104	/* 0x00000068 */
#define flexidag_openseg_prim_split_3___pvcn_130007__dpitchm1_byte_offset 	108	/* 0x0000006c */
#define flexidag_openseg_prim_split_3___pvcn_130007__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_3___pvcn_130007__W 	12716
#define flexidag_openseg_prim_split_3___pvcn_130007__H 	1
#define flexidag_openseg_prim_split_3___pvcn_130007__D 	1
#define flexidag_openseg_prim_split_3___pvcn_130007__P 	1
#define flexidag_openseg_prim_split_3___pvcn_130007__data_num_bytes 	1
#define flexidag_openseg_prim_split_3___pvcn_130007__denable 	1
#define flexidag_openseg_prim_split_3___pvcn_130007__dpitch_num_bytes 	12736
#define flexidag_openseg_prim_split_3___pvcn_130007__dram_format 	0
#define flexidag_openseg_prim_split_3___pvcn_130007__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_3___pvcn_130007__tile_width 	1
#define flexidag_openseg_prim_split_3___pvcn_130007__tile_height 	1
#define flexidag_openseg_prim_split_3___pvcn_130007__vmem_buffer_num_bytes 	12716
#define flexidag_openseg_prim_split_3___pvcn_130007__dram_size_num_bytes 	12736

/* flexidag_openseg_prim_split_3 SMB_input __pvcn_130005_ */
#define flexidag_openseg_prim_split_3___pvcn_130005__cnngen_demangled_name 	"__pvcn_130005_"
#define flexidag_openseg_prim_split_3___pvcn_130005__is_constant 	1
#define flexidag_openseg_prim_split_3___pvcn_130005__has_init_data 	1
#define flexidag_openseg_prim_split_3___pvcn_130005__is_variable_scalar 	0
#define flexidag_openseg_prim_split_3___pvcn_130005__is_bitvector 	0
#define flexidag_openseg_prim_split_3___pvcn_130005__is_rlz 	0
#define flexidag_openseg_prim_split_3___pvcn_130005__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_3___pvcn_130005__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_3___pvcn_130005__byte_offset 	116	/* 0x00000074 */
#define flexidag_openseg_prim_split_3___pvcn_130005__vbase_byte_offset 	120	/* 0x00000078 */
#define flexidag_openseg_prim_split_3___pvcn_130005__dbase_byte_offset 	144	/* 0x00000090 */
#define flexidag_openseg_prim_split_3___pvcn_130005__dpitchm1_byte_offset 	148	/* 0x00000094 */
#define flexidag_openseg_prim_split_3___pvcn_130005__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_3___pvcn_130005__W 	544
#define flexidag_openseg_prim_split_3___pvcn_130005__H 	1
#define flexidag_openseg_prim_split_3___pvcn_130005__D 	1
#define flexidag_openseg_prim_split_3___pvcn_130005__P 	1
#define flexidag_openseg_prim_split_3___pvcn_130005__data_num_bytes 	1
#define flexidag_openseg_prim_split_3___pvcn_130005__denable 	1
#define flexidag_openseg_prim_split_3___pvcn_130005__dpitch_num_bytes 	544
#define flexidag_openseg_prim_split_3___pvcn_130005__dram_format 	0
#define flexidag_openseg_prim_split_3___pvcn_130005__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_3___pvcn_130005__tile_width 	1
#define flexidag_openseg_prim_split_3___pvcn_130005__tile_height 	1
#define flexidag_openseg_prim_split_3___pvcn_130005__vmem_buffer_num_bytes 	544
#define flexidag_openseg_prim_split_3___pvcn_130005__dram_size_num_bytes 	544

/* flexidag_openseg_prim_split_3 SMB_input __pvcn_130008_ */
#define flexidag_openseg_prim_split_3___pvcn_130008__cnngen_demangled_name 	"__pvcn_130008_"
#define flexidag_openseg_prim_split_3___pvcn_130008__is_constant 	1
#define flexidag_openseg_prim_split_3___pvcn_130008__has_init_data 	1
#define flexidag_openseg_prim_split_3___pvcn_130008__is_variable_scalar 	0
#define flexidag_openseg_prim_split_3___pvcn_130008__is_bitvector 	0
#define flexidag_openseg_prim_split_3___pvcn_130008__is_rlz 	0
#define flexidag_openseg_prim_split_3___pvcn_130008__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_3___pvcn_130008__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_3___pvcn_130008__byte_offset 	156	/* 0x0000009c */
#define flexidag_openseg_prim_split_3___pvcn_130008__vbase_byte_offset 	160	/* 0x000000a0 */
#define flexidag_openseg_prim_split_3___pvcn_130008__dbase_byte_offset 	184	/* 0x000000b8 */
#define flexidag_openseg_prim_split_3___pvcn_130008__dpitchm1_byte_offset 	188	/* 0x000000bc */
#define flexidag_openseg_prim_split_3___pvcn_130008__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_3___pvcn_130008__W 	544
#define flexidag_openseg_prim_split_3___pvcn_130008__H 	1
#define flexidag_openseg_prim_split_3___pvcn_130008__D 	1
#define flexidag_openseg_prim_split_3___pvcn_130008__P 	1
#define flexidag_openseg_prim_split_3___pvcn_130008__data_num_bytes 	1
#define flexidag_openseg_prim_split_3___pvcn_130008__denable 	1
#define flexidag_openseg_prim_split_3___pvcn_130008__dpitch_num_bytes 	544
#define flexidag_openseg_prim_split_3___pvcn_130008__dram_format 	0
#define flexidag_openseg_prim_split_3___pvcn_130008__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_3___pvcn_130008__tile_width 	1
#define flexidag_openseg_prim_split_3___pvcn_130008__tile_height 	1
#define flexidag_openseg_prim_split_3___pvcn_130008__vmem_buffer_num_bytes 	544
#define flexidag_openseg_prim_split_3___pvcn_130008__dram_size_num_bytes 	544

/* flexidag_openseg_prim_split_3 SMB_input __pvcn_1160_ */
#define flexidag_openseg_prim_split_3___pvcn_1160__cnngen_demangled_name 	"__pvcn_1160_"
#define flexidag_openseg_prim_split_3___pvcn_1160__is_constant 	1
#define flexidag_openseg_prim_split_3___pvcn_1160__has_init_data 	1
#define flexidag_openseg_prim_split_3___pvcn_1160__is_variable_scalar 	0
#define flexidag_openseg_prim_split_3___pvcn_1160__is_bitvector 	0
#define flexidag_openseg_prim_split_3___pvcn_1160__is_rlz 	0
#define flexidag_openseg_prim_split_3___pvcn_1160__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_3___pvcn_1160__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_3___pvcn_1160__byte_offset 	196	/* 0x000000c4 */
#define flexidag_openseg_prim_split_3___pvcn_1160__vbase_byte_offset 	200	/* 0x000000c8 */
#define flexidag_openseg_prim_split_3___pvcn_1160__dbase_byte_offset 	224	/* 0x000000e0 */
#define flexidag_openseg_prim_split_3___pvcn_1160__dpitchm1_byte_offset 	228	/* 0x000000e4 */
#define flexidag_openseg_prim_split_3___pvcn_1160__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_3___pvcn_1160__W 	5660
#define flexidag_openseg_prim_split_3___pvcn_1160__H 	1
#define flexidag_openseg_prim_split_3___pvcn_1160__D 	1
#define flexidag_openseg_prim_split_3___pvcn_1160__P 	1
#define flexidag_openseg_prim_split_3___pvcn_1160__data_num_bytes 	1
#define flexidag_openseg_prim_split_3___pvcn_1160__denable 	1
#define flexidag_openseg_prim_split_3___pvcn_1160__dpitch_num_bytes 	5664
#define flexidag_openseg_prim_split_3___pvcn_1160__dram_format 	0
#define flexidag_openseg_prim_split_3___pvcn_1160__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_3___pvcn_1160__tile_width 	1
#define flexidag_openseg_prim_split_3___pvcn_1160__tile_height 	1
#define flexidag_openseg_prim_split_3___pvcn_1160__vmem_buffer_num_bytes 	5660
#define flexidag_openseg_prim_split_3___pvcn_1160__dram_size_num_bytes 	5664

/* flexidag_openseg_prim_split_3 SMB_input __pvcn_1161_ */
#define flexidag_openseg_prim_split_3___pvcn_1161__cnngen_demangled_name 	"__pvcn_1161_"
#define flexidag_openseg_prim_split_3___pvcn_1161__is_constant 	1
#define flexidag_openseg_prim_split_3___pvcn_1161__has_init_data 	1
#define flexidag_openseg_prim_split_3___pvcn_1161__is_variable_scalar 	0
#define flexidag_openseg_prim_split_3___pvcn_1161__is_bitvector 	0
#define flexidag_openseg_prim_split_3___pvcn_1161__is_rlz 	0
#define flexidag_openseg_prim_split_3___pvcn_1161__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_3___pvcn_1161__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_3___pvcn_1161__byte_offset 	236	/* 0x000000ec */
#define flexidag_openseg_prim_split_3___pvcn_1161__vbase_byte_offset 	240	/* 0x000000f0 */
#define flexidag_openseg_prim_split_3___pvcn_1161__dbase_byte_offset 	264	/* 0x00000108 */
#define flexidag_openseg_prim_split_3___pvcn_1161__dpitchm1_byte_offset 	268	/* 0x0000010c */
#define flexidag_openseg_prim_split_3___pvcn_1161__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_3___pvcn_1161__W 	136
#define flexidag_openseg_prim_split_3___pvcn_1161__H 	1
#define flexidag_openseg_prim_split_3___pvcn_1161__D 	1
#define flexidag_openseg_prim_split_3___pvcn_1161__P 	1
#define flexidag_openseg_prim_split_3___pvcn_1161__data_num_bytes 	1
#define flexidag_openseg_prim_split_3___pvcn_1161__denable 	1
#define flexidag_openseg_prim_split_3___pvcn_1161__dpitch_num_bytes 	160
#define flexidag_openseg_prim_split_3___pvcn_1161__dram_format 	0
#define flexidag_openseg_prim_split_3___pvcn_1161__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_3___pvcn_1161__tile_width 	1
#define flexidag_openseg_prim_split_3___pvcn_1161__tile_height 	1
#define flexidag_openseg_prim_split_3___pvcn_1161__vmem_buffer_num_bytes 	136
#define flexidag_openseg_prim_split_3___pvcn_1161__dram_size_num_bytes 	160

/* flexidag_openseg_prim_split_3 SMB_input __pvcn_1163_ */
#define flexidag_openseg_prim_split_3___pvcn_1163__cnngen_demangled_name 	"__pvcn_1163_"
#define flexidag_openseg_prim_split_3___pvcn_1163__is_constant 	1
#define flexidag_openseg_prim_split_3___pvcn_1163__has_init_data 	1
#define flexidag_openseg_prim_split_3___pvcn_1163__is_variable_scalar 	0
#define flexidag_openseg_prim_split_3___pvcn_1163__is_bitvector 	0
#define flexidag_openseg_prim_split_3___pvcn_1163__is_rlz 	0
#define flexidag_openseg_prim_split_3___pvcn_1163__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_3___pvcn_1163__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_3___pvcn_1163__byte_offset 	276	/* 0x00000114 */
#define flexidag_openseg_prim_split_3___pvcn_1163__vbase_byte_offset 	280	/* 0x00000118 */
#define flexidag_openseg_prim_split_3___pvcn_1163__dbase_byte_offset 	304	/* 0x00000130 */
#define flexidag_openseg_prim_split_3___pvcn_1163__dpitchm1_byte_offset 	308	/* 0x00000134 */
#define flexidag_openseg_prim_split_3___pvcn_1163__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_3___pvcn_1163__W 	1436
#define flexidag_openseg_prim_split_3___pvcn_1163__H 	1
#define flexidag_openseg_prim_split_3___pvcn_1163__D 	1
#define flexidag_openseg_prim_split_3___pvcn_1163__P 	1
#define flexidag_openseg_prim_split_3___pvcn_1163__data_num_bytes 	1
#define flexidag_openseg_prim_split_3___pvcn_1163__denable 	1
#define flexidag_openseg_prim_split_3___pvcn_1163__dpitch_num_bytes 	1440
#define flexidag_openseg_prim_split_3___pvcn_1163__dram_format 	0
#define flexidag_openseg_prim_split_3___pvcn_1163__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_3___pvcn_1163__tile_width 	1
#define flexidag_openseg_prim_split_3___pvcn_1163__tile_height 	1
#define flexidag_openseg_prim_split_3___pvcn_1163__vmem_buffer_num_bytes 	1436
#define flexidag_openseg_prim_split_3___pvcn_1163__dram_size_num_bytes 	1440

/* flexidag_openseg_prim_split_3 SMB_input __pvcn_1164_ */
#define flexidag_openseg_prim_split_3___pvcn_1164__cnngen_demangled_name 	"__pvcn_1164_"
#define flexidag_openseg_prim_split_3___pvcn_1164__is_constant 	1
#define flexidag_openseg_prim_split_3___pvcn_1164__has_init_data 	1
#define flexidag_openseg_prim_split_3___pvcn_1164__is_variable_scalar 	0
#define flexidag_openseg_prim_split_3___pvcn_1164__is_bitvector 	0
#define flexidag_openseg_prim_split_3___pvcn_1164__is_rlz 	0
#define flexidag_openseg_prim_split_3___pvcn_1164__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_3___pvcn_1164__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_3___pvcn_1164__byte_offset 	316	/* 0x0000013c */
#define flexidag_openseg_prim_split_3___pvcn_1164__vbase_byte_offset 	320	/* 0x00000140 */
#define flexidag_openseg_prim_split_3___pvcn_1164__dbase_byte_offset 	344	/* 0x00000158 */
#define flexidag_openseg_prim_split_3___pvcn_1164__dpitchm1_byte_offset 	348	/* 0x0000015c */
#define flexidag_openseg_prim_split_3___pvcn_1164__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_3___pvcn_1164__W 	136
#define flexidag_openseg_prim_split_3___pvcn_1164__H 	1
#define flexidag_openseg_prim_split_3___pvcn_1164__D 	1
#define flexidag_openseg_prim_split_3___pvcn_1164__P 	1
#define flexidag_openseg_prim_split_3___pvcn_1164__data_num_bytes 	1
#define flexidag_openseg_prim_split_3___pvcn_1164__denable 	1
#define flexidag_openseg_prim_split_3___pvcn_1164__dpitch_num_bytes 	160
#define flexidag_openseg_prim_split_3___pvcn_1164__dram_format 	0
#define flexidag_openseg_prim_split_3___pvcn_1164__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_3___pvcn_1164__tile_width 	1
#define flexidag_openseg_prim_split_3___pvcn_1164__tile_height 	1
#define flexidag_openseg_prim_split_3___pvcn_1164__vmem_buffer_num_bytes 	136
#define flexidag_openseg_prim_split_3___pvcn_1164__dram_size_num_bytes 	160

/* flexidag_openseg_prim_split_3 SMB_input __pvcn_1167_ */
#define flexidag_openseg_prim_split_3___pvcn_1167__cnngen_demangled_name 	"__pvcn_1167_"
#define flexidag_openseg_prim_split_3___pvcn_1167__is_constant 	1
#define flexidag_openseg_prim_split_3___pvcn_1167__has_init_data 	1
#define flexidag_openseg_prim_split_3___pvcn_1167__is_variable_scalar 	0
#define flexidag_openseg_prim_split_3___pvcn_1167__is_bitvector 	0
#define flexidag_openseg_prim_split_3___pvcn_1167__is_rlz 	0
#define flexidag_openseg_prim_split_3___pvcn_1167__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_3___pvcn_1167__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_3___pvcn_1167__byte_offset 	356	/* 0x00000164 */
#define flexidag_openseg_prim_split_3___pvcn_1167__vbase_byte_offset 	360	/* 0x00000168 */
#define flexidag_openseg_prim_split_3___pvcn_1167__dbase_byte_offset 	384	/* 0x00000180 */
#define flexidag_openseg_prim_split_3___pvcn_1167__dpitchm1_byte_offset 	388	/* 0x00000184 */
#define flexidag_openseg_prim_split_3___pvcn_1167__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_3___pvcn_1167__W 	5784
#define flexidag_openseg_prim_split_3___pvcn_1167__H 	1
#define flexidag_openseg_prim_split_3___pvcn_1167__D 	1
#define flexidag_openseg_prim_split_3___pvcn_1167__P 	1
#define flexidag_openseg_prim_split_3___pvcn_1167__data_num_bytes 	1
#define flexidag_openseg_prim_split_3___pvcn_1167__denable 	1
#define flexidag_openseg_prim_split_3___pvcn_1167__dpitch_num_bytes 	5792
#define flexidag_openseg_prim_split_3___pvcn_1167__dram_format 	0
#define flexidag_openseg_prim_split_3___pvcn_1167__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_3___pvcn_1167__tile_width 	1
#define flexidag_openseg_prim_split_3___pvcn_1167__tile_height 	1
#define flexidag_openseg_prim_split_3___pvcn_1167__vmem_buffer_num_bytes 	5784
#define flexidag_openseg_prim_split_3___pvcn_1167__dram_size_num_bytes 	5792

/* flexidag_openseg_prim_split_3 SMB_input __pvcn_1168_ */
#define flexidag_openseg_prim_split_3___pvcn_1168__cnngen_demangled_name 	"__pvcn_1168_"
#define flexidag_openseg_prim_split_3___pvcn_1168__is_constant 	1
#define flexidag_openseg_prim_split_3___pvcn_1168__has_init_data 	1
#define flexidag_openseg_prim_split_3___pvcn_1168__is_variable_scalar 	0
#define flexidag_openseg_prim_split_3___pvcn_1168__is_bitvector 	0
#define flexidag_openseg_prim_split_3___pvcn_1168__is_rlz 	0
#define flexidag_openseg_prim_split_3___pvcn_1168__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_3___pvcn_1168__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_3___pvcn_1168__byte_offset 	396	/* 0x0000018c */
#define flexidag_openseg_prim_split_3___pvcn_1168__vbase_byte_offset 	400	/* 0x00000190 */
#define flexidag_openseg_prim_split_3___pvcn_1168__dbase_byte_offset 	424	/* 0x000001a8 */
#define flexidag_openseg_prim_split_3___pvcn_1168__dpitchm1_byte_offset 	428	/* 0x000001ac */
#define flexidag_openseg_prim_split_3___pvcn_1168__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_3___pvcn_1168__W 	136
#define flexidag_openseg_prim_split_3___pvcn_1168__H 	1
#define flexidag_openseg_prim_split_3___pvcn_1168__D 	1
#define flexidag_openseg_prim_split_3___pvcn_1168__P 	1
#define flexidag_openseg_prim_split_3___pvcn_1168__data_num_bytes 	1
#define flexidag_openseg_prim_split_3___pvcn_1168__denable 	1
#define flexidag_openseg_prim_split_3___pvcn_1168__dpitch_num_bytes 	160
#define flexidag_openseg_prim_split_3___pvcn_1168__dram_format 	0
#define flexidag_openseg_prim_split_3___pvcn_1168__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_3___pvcn_1168__tile_width 	1
#define flexidag_openseg_prim_split_3___pvcn_1168__tile_height 	1
#define flexidag_openseg_prim_split_3___pvcn_1168__vmem_buffer_num_bytes 	136
#define flexidag_openseg_prim_split_3___pvcn_1168__dram_size_num_bytes 	160

/* flexidag_openseg_prim_split_3 SMB_input __pvcn_1174_ */
#define flexidag_openseg_prim_split_3___pvcn_1174__cnngen_demangled_name 	"__pvcn_1174_"
#define flexidag_openseg_prim_split_3___pvcn_1174__is_constant 	1
#define flexidag_openseg_prim_split_3___pvcn_1174__has_init_data 	1
#define flexidag_openseg_prim_split_3___pvcn_1174__is_variable_scalar 	0
#define flexidag_openseg_prim_split_3___pvcn_1174__is_bitvector 	0
#define flexidag_openseg_prim_split_3___pvcn_1174__is_rlz 	0
#define flexidag_openseg_prim_split_3___pvcn_1174__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_3___pvcn_1174__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_3___pvcn_1174__byte_offset 	436	/* 0x000001b4 */
#define flexidag_openseg_prim_split_3___pvcn_1174__vbase_byte_offset 	440	/* 0x000001b8 */
#define flexidag_openseg_prim_split_3___pvcn_1174__dbase_byte_offset 	464	/* 0x000001d0 */
#define flexidag_openseg_prim_split_3___pvcn_1174__dpitchm1_byte_offset 	468	/* 0x000001d4 */
#define flexidag_openseg_prim_split_3___pvcn_1174__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_3___pvcn_1174__W 	28200
#define flexidag_openseg_prim_split_3___pvcn_1174__H 	1
#define flexidag_openseg_prim_split_3___pvcn_1174__D 	1
#define flexidag_openseg_prim_split_3___pvcn_1174__P 	1
#define flexidag_openseg_prim_split_3___pvcn_1174__data_num_bytes 	1
#define flexidag_openseg_prim_split_3___pvcn_1174__denable 	1
#define flexidag_openseg_prim_split_3___pvcn_1174__dpitch_num_bytes 	28224
#define flexidag_openseg_prim_split_3___pvcn_1174__dram_format 	0
#define flexidag_openseg_prim_split_3___pvcn_1174__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_3___pvcn_1174__tile_width 	1
#define flexidag_openseg_prim_split_3___pvcn_1174__tile_height 	1
#define flexidag_openseg_prim_split_3___pvcn_1174__vmem_buffer_num_bytes 	28200
#define flexidag_openseg_prim_split_3___pvcn_1174__dram_size_num_bytes 	28224

/* flexidag_openseg_prim_split_3 SMB_input __pvcn_1175_ */
#define flexidag_openseg_prim_split_3___pvcn_1175__cnngen_demangled_name 	"__pvcn_1175_"
#define flexidag_openseg_prim_split_3___pvcn_1175__is_constant 	1
#define flexidag_openseg_prim_split_3___pvcn_1175__has_init_data 	1
#define flexidag_openseg_prim_split_3___pvcn_1175__is_variable_scalar 	0
#define flexidag_openseg_prim_split_3___pvcn_1175__is_bitvector 	0
#define flexidag_openseg_prim_split_3___pvcn_1175__is_rlz 	0
#define flexidag_openseg_prim_split_3___pvcn_1175__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_3___pvcn_1175__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_3___pvcn_1175__byte_offset 	476	/* 0x000001dc */
#define flexidag_openseg_prim_split_3___pvcn_1175__vbase_byte_offset 	480	/* 0x000001e0 */
#define flexidag_openseg_prim_split_3___pvcn_1175__dbase_byte_offset 	504	/* 0x000001f8 */
#define flexidag_openseg_prim_split_3___pvcn_1175__dpitchm1_byte_offset 	508	/* 0x000001fc */
#define flexidag_openseg_prim_split_3___pvcn_1175__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_3___pvcn_1175__W 	1088
#define flexidag_openseg_prim_split_3___pvcn_1175__H 	1
#define flexidag_openseg_prim_split_3___pvcn_1175__D 	1
#define flexidag_openseg_prim_split_3___pvcn_1175__P 	1
#define flexidag_openseg_prim_split_3___pvcn_1175__data_num_bytes 	1
#define flexidag_openseg_prim_split_3___pvcn_1175__denable 	1
#define flexidag_openseg_prim_split_3___pvcn_1175__dpitch_num_bytes 	1088
#define flexidag_openseg_prim_split_3___pvcn_1175__dram_format 	0
#define flexidag_openseg_prim_split_3___pvcn_1175__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_3___pvcn_1175__tile_width 	1
#define flexidag_openseg_prim_split_3___pvcn_1175__tile_height 	1
#define flexidag_openseg_prim_split_3___pvcn_1175__vmem_buffer_num_bytes 	1088
#define flexidag_openseg_prim_split_3___pvcn_1175__dram_size_num_bytes 	1088

/* flexidag_openseg_prim_split_3 SMB_input __pvcn_1178_ */
#define flexidag_openseg_prim_split_3___pvcn_1178__cnngen_demangled_name 	"__pvcn_1178_"
#define flexidag_openseg_prim_split_3___pvcn_1178__is_constant 	1
#define flexidag_openseg_prim_split_3___pvcn_1178__has_init_data 	1
#define flexidag_openseg_prim_split_3___pvcn_1178__is_variable_scalar 	0
#define flexidag_openseg_prim_split_3___pvcn_1178__is_bitvector 	0
#define flexidag_openseg_prim_split_3___pvcn_1178__is_rlz 	0
#define flexidag_openseg_prim_split_3___pvcn_1178__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_3___pvcn_1178__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_3___pvcn_1178__byte_offset 	516	/* 0x00000204 */
#define flexidag_openseg_prim_split_3___pvcn_1178__vbase_byte_offset 	520	/* 0x00000208 */
#define flexidag_openseg_prim_split_3___pvcn_1178__dbase_byte_offset 	544	/* 0x00000220 */
#define flexidag_openseg_prim_split_3___pvcn_1178__dpitchm1_byte_offset 	548	/* 0x00000224 */
#define flexidag_openseg_prim_split_3___pvcn_1178__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_3___pvcn_1178__W 	5748
#define flexidag_openseg_prim_split_3___pvcn_1178__H 	1
#define flexidag_openseg_prim_split_3___pvcn_1178__D 	1
#define flexidag_openseg_prim_split_3___pvcn_1178__P 	1
#define flexidag_openseg_prim_split_3___pvcn_1178__data_num_bytes 	1
#define flexidag_openseg_prim_split_3___pvcn_1178__denable 	1
#define flexidag_openseg_prim_split_3___pvcn_1178__dpitch_num_bytes 	5760
#define flexidag_openseg_prim_split_3___pvcn_1178__dram_format 	0
#define flexidag_openseg_prim_split_3___pvcn_1178__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_3___pvcn_1178__tile_width 	1
#define flexidag_openseg_prim_split_3___pvcn_1178__tile_height 	1
#define flexidag_openseg_prim_split_3___pvcn_1178__vmem_buffer_num_bytes 	5748
#define flexidag_openseg_prim_split_3___pvcn_1178__dram_size_num_bytes 	5760

/* flexidag_openseg_prim_split_3 SMB_input __pvcn_1179_ */
#define flexidag_openseg_prim_split_3___pvcn_1179__cnngen_demangled_name 	"__pvcn_1179_"
#define flexidag_openseg_prim_split_3___pvcn_1179__is_constant 	1
#define flexidag_openseg_prim_split_3___pvcn_1179__has_init_data 	1
#define flexidag_openseg_prim_split_3___pvcn_1179__is_variable_scalar 	0
#define flexidag_openseg_prim_split_3___pvcn_1179__is_bitvector 	0
#define flexidag_openseg_prim_split_3___pvcn_1179__is_rlz 	0
#define flexidag_openseg_prim_split_3___pvcn_1179__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_3___pvcn_1179__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_3___pvcn_1179__byte_offset 	556	/* 0x0000022c */
#define flexidag_openseg_prim_split_3___pvcn_1179__vbase_byte_offset 	560	/* 0x00000230 */
#define flexidag_openseg_prim_split_3___pvcn_1179__dbase_byte_offset 	584	/* 0x00000248 */
#define flexidag_openseg_prim_split_3___pvcn_1179__dpitchm1_byte_offset 	588	/* 0x0000024c */
#define flexidag_openseg_prim_split_3___pvcn_1179__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_3___pvcn_1179__W 	144
#define flexidag_openseg_prim_split_3___pvcn_1179__H 	1
#define flexidag_openseg_prim_split_3___pvcn_1179__D 	1
#define flexidag_openseg_prim_split_3___pvcn_1179__P 	1
#define flexidag_openseg_prim_split_3___pvcn_1179__data_num_bytes 	1
#define flexidag_openseg_prim_split_3___pvcn_1179__denable 	1
#define flexidag_openseg_prim_split_3___pvcn_1179__dpitch_num_bytes 	160
#define flexidag_openseg_prim_split_3___pvcn_1179__dram_format 	0
#define flexidag_openseg_prim_split_3___pvcn_1179__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_3___pvcn_1179__tile_width 	1
#define flexidag_openseg_prim_split_3___pvcn_1179__tile_height 	1
#define flexidag_openseg_prim_split_3___pvcn_1179__vmem_buffer_num_bytes 	144
#define flexidag_openseg_prim_split_3___pvcn_1179__dram_size_num_bytes 	160

/* flexidag_openseg_prim_split_3 SMB_input __pvcn_1181_ */
#define flexidag_openseg_prim_split_3___pvcn_1181__cnngen_demangled_name 	"__pvcn_1181_"
#define flexidag_openseg_prim_split_3___pvcn_1181__is_constant 	1
#define flexidag_openseg_prim_split_3___pvcn_1181__has_init_data 	1
#define flexidag_openseg_prim_split_3___pvcn_1181__is_variable_scalar 	0
#define flexidag_openseg_prim_split_3___pvcn_1181__is_bitvector 	0
#define flexidag_openseg_prim_split_3___pvcn_1181__is_rlz 	0
#define flexidag_openseg_prim_split_3___pvcn_1181__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_3___pvcn_1181__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_3___pvcn_1181__byte_offset 	596	/* 0x00000254 */
#define flexidag_openseg_prim_split_3___pvcn_1181__vbase_byte_offset 	600	/* 0x00000258 */
#define flexidag_openseg_prim_split_3___pvcn_1181__dbase_byte_offset 	624	/* 0x00000270 */
#define flexidag_openseg_prim_split_3___pvcn_1181__dpitchm1_byte_offset 	628	/* 0x00000274 */
#define flexidag_openseg_prim_split_3___pvcn_1181__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_3___pvcn_1181__W 	1440
#define flexidag_openseg_prim_split_3___pvcn_1181__H 	1
#define flexidag_openseg_prim_split_3___pvcn_1181__D 	1
#define flexidag_openseg_prim_split_3___pvcn_1181__P 	1
#define flexidag_openseg_prim_split_3___pvcn_1181__data_num_bytes 	1
#define flexidag_openseg_prim_split_3___pvcn_1181__denable 	1
#define flexidag_openseg_prim_split_3___pvcn_1181__dpitch_num_bytes 	1440
#define flexidag_openseg_prim_split_3___pvcn_1181__dram_format 	0
#define flexidag_openseg_prim_split_3___pvcn_1181__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_3___pvcn_1181__tile_width 	1
#define flexidag_openseg_prim_split_3___pvcn_1181__tile_height 	1
#define flexidag_openseg_prim_split_3___pvcn_1181__vmem_buffer_num_bytes 	1440
#define flexidag_openseg_prim_split_3___pvcn_1181__dram_size_num_bytes 	1440

/* flexidag_openseg_prim_split_3 SMB_input __pvcn_1182_ */
#define flexidag_openseg_prim_split_3___pvcn_1182__cnngen_demangled_name 	"__pvcn_1182_"
#define flexidag_openseg_prim_split_3___pvcn_1182__is_constant 	1
#define flexidag_openseg_prim_split_3___pvcn_1182__has_init_data 	1
#define flexidag_openseg_prim_split_3___pvcn_1182__is_variable_scalar 	0
#define flexidag_openseg_prim_split_3___pvcn_1182__is_bitvector 	0
#define flexidag_openseg_prim_split_3___pvcn_1182__is_rlz 	0
#define flexidag_openseg_prim_split_3___pvcn_1182__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_3___pvcn_1182__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_3___pvcn_1182__byte_offset 	636	/* 0x0000027c */
#define flexidag_openseg_prim_split_3___pvcn_1182__vbase_byte_offset 	640	/* 0x00000280 */
#define flexidag_openseg_prim_split_3___pvcn_1182__dbase_byte_offset 	664	/* 0x00000298 */
#define flexidag_openseg_prim_split_3___pvcn_1182__dpitchm1_byte_offset 	668	/* 0x0000029c */
#define flexidag_openseg_prim_split_3___pvcn_1182__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_3___pvcn_1182__W 	144
#define flexidag_openseg_prim_split_3___pvcn_1182__H 	1
#define flexidag_openseg_prim_split_3___pvcn_1182__D 	1
#define flexidag_openseg_prim_split_3___pvcn_1182__P 	1
#define flexidag_openseg_prim_split_3___pvcn_1182__data_num_bytes 	1
#define flexidag_openseg_prim_split_3___pvcn_1182__denable 	1
#define flexidag_openseg_prim_split_3___pvcn_1182__dpitch_num_bytes 	160
#define flexidag_openseg_prim_split_3___pvcn_1182__dram_format 	0
#define flexidag_openseg_prim_split_3___pvcn_1182__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_3___pvcn_1182__tile_width 	1
#define flexidag_openseg_prim_split_3___pvcn_1182__tile_height 	1
#define flexidag_openseg_prim_split_3___pvcn_1182__vmem_buffer_num_bytes 	144
#define flexidag_openseg_prim_split_3___pvcn_1182__dram_size_num_bytes 	160

/* flexidag_openseg_prim_split_3 SMB_input __pvcn_1185_ */
#define flexidag_openseg_prim_split_3___pvcn_1185__cnngen_demangled_name 	"__pvcn_1185_"
#define flexidag_openseg_prim_split_3___pvcn_1185__is_constant 	1
#define flexidag_openseg_prim_split_3___pvcn_1185__has_init_data 	1
#define flexidag_openseg_prim_split_3___pvcn_1185__is_variable_scalar 	0
#define flexidag_openseg_prim_split_3___pvcn_1185__is_bitvector 	0
#define flexidag_openseg_prim_split_3___pvcn_1185__is_rlz 	0
#define flexidag_openseg_prim_split_3___pvcn_1185__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_3___pvcn_1185__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_3___pvcn_1185__byte_offset 	676	/* 0x000002a4 */
#define flexidag_openseg_prim_split_3___pvcn_1185__vbase_byte_offset 	680	/* 0x000002a8 */
#define flexidag_openseg_prim_split_3___pvcn_1185__dbase_byte_offset 	704	/* 0x000002c0 */
#define flexidag_openseg_prim_split_3___pvcn_1185__dpitchm1_byte_offset 	708	/* 0x000002c4 */
#define flexidag_openseg_prim_split_3___pvcn_1185__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_3___pvcn_1185__W 	5736
#define flexidag_openseg_prim_split_3___pvcn_1185__H 	1
#define flexidag_openseg_prim_split_3___pvcn_1185__D 	1
#define flexidag_openseg_prim_split_3___pvcn_1185__P 	1
#define flexidag_openseg_prim_split_3___pvcn_1185__data_num_bytes 	1
#define flexidag_openseg_prim_split_3___pvcn_1185__denable 	1
#define flexidag_openseg_prim_split_3___pvcn_1185__dpitch_num_bytes 	5760
#define flexidag_openseg_prim_split_3___pvcn_1185__dram_format 	0
#define flexidag_openseg_prim_split_3___pvcn_1185__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_3___pvcn_1185__tile_width 	1
#define flexidag_openseg_prim_split_3___pvcn_1185__tile_height 	1
#define flexidag_openseg_prim_split_3___pvcn_1185__vmem_buffer_num_bytes 	5736
#define flexidag_openseg_prim_split_3___pvcn_1185__dram_size_num_bytes 	5760

/* flexidag_openseg_prim_split_3 SMB_input __pvcn_1186_ */
#define flexidag_openseg_prim_split_3___pvcn_1186__cnngen_demangled_name 	"__pvcn_1186_"
#define flexidag_openseg_prim_split_3___pvcn_1186__is_constant 	1
#define flexidag_openseg_prim_split_3___pvcn_1186__has_init_data 	1
#define flexidag_openseg_prim_split_3___pvcn_1186__is_variable_scalar 	0
#define flexidag_openseg_prim_split_3___pvcn_1186__is_bitvector 	0
#define flexidag_openseg_prim_split_3___pvcn_1186__is_rlz 	0
#define flexidag_openseg_prim_split_3___pvcn_1186__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_3___pvcn_1186__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_3___pvcn_1186__byte_offset 	716	/* 0x000002cc */
#define flexidag_openseg_prim_split_3___pvcn_1186__vbase_byte_offset 	720	/* 0x000002d0 */
#define flexidag_openseg_prim_split_3___pvcn_1186__dbase_byte_offset 	744	/* 0x000002e8 */
#define flexidag_openseg_prim_split_3___pvcn_1186__dpitchm1_byte_offset 	748	/* 0x000002ec */
#define flexidag_openseg_prim_split_3___pvcn_1186__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_3___pvcn_1186__W 	136
#define flexidag_openseg_prim_split_3___pvcn_1186__H 	1
#define flexidag_openseg_prim_split_3___pvcn_1186__D 	1
#define flexidag_openseg_prim_split_3___pvcn_1186__P 	1
#define flexidag_openseg_prim_split_3___pvcn_1186__data_num_bytes 	1
#define flexidag_openseg_prim_split_3___pvcn_1186__denable 	1
#define flexidag_openseg_prim_split_3___pvcn_1186__dpitch_num_bytes 	160
#define flexidag_openseg_prim_split_3___pvcn_1186__dram_format 	0
#define flexidag_openseg_prim_split_3___pvcn_1186__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_3___pvcn_1186__tile_width 	1
#define flexidag_openseg_prim_split_3___pvcn_1186__tile_height 	1
#define flexidag_openseg_prim_split_3___pvcn_1186__vmem_buffer_num_bytes 	136
#define flexidag_openseg_prim_split_3___pvcn_1186__dram_size_num_bytes 	160

/* flexidag_openseg_prim_split_3 SMB_input __pvcn_1192_ */
#define flexidag_openseg_prim_split_3___pvcn_1192__cnngen_demangled_name 	"__pvcn_1192_"
#define flexidag_openseg_prim_split_3___pvcn_1192__is_constant 	1
#define flexidag_openseg_prim_split_3___pvcn_1192__has_init_data 	1
#define flexidag_openseg_prim_split_3___pvcn_1192__is_variable_scalar 	0
#define flexidag_openseg_prim_split_3___pvcn_1192__is_bitvector 	0
#define flexidag_openseg_prim_split_3___pvcn_1192__is_rlz 	0
#define flexidag_openseg_prim_split_3___pvcn_1192__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_3___pvcn_1192__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_3___pvcn_1192__byte_offset 	756	/* 0x000002f4 */
#define flexidag_openseg_prim_split_3___pvcn_1192__vbase_byte_offset 	760	/* 0x000002f8 */
#define flexidag_openseg_prim_split_3___pvcn_1192__dbase_byte_offset 	784	/* 0x00000310 */
#define flexidag_openseg_prim_split_3___pvcn_1192__dpitchm1_byte_offset 	788	/* 0x00000314 */
#define flexidag_openseg_prim_split_3___pvcn_1192__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_3___pvcn_1192__W 	30664
#define flexidag_openseg_prim_split_3___pvcn_1192__H 	1
#define flexidag_openseg_prim_split_3___pvcn_1192__D 	1
#define flexidag_openseg_prim_split_3___pvcn_1192__P 	1
#define flexidag_openseg_prim_split_3___pvcn_1192__data_num_bytes 	1
#define flexidag_openseg_prim_split_3___pvcn_1192__denable 	1
#define flexidag_openseg_prim_split_3___pvcn_1192__dpitch_num_bytes 	30688
#define flexidag_openseg_prim_split_3___pvcn_1192__dram_format 	0
#define flexidag_openseg_prim_split_3___pvcn_1192__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_3___pvcn_1192__tile_width 	1
#define flexidag_openseg_prim_split_3___pvcn_1192__tile_height 	1
#define flexidag_openseg_prim_split_3___pvcn_1192__vmem_buffer_num_bytes 	30664
#define flexidag_openseg_prim_split_3___pvcn_1192__dram_size_num_bytes 	30688

/* flexidag_openseg_prim_split_3 SMB_input __pvcn_1193_ */
#define flexidag_openseg_prim_split_3___pvcn_1193__cnngen_demangled_name 	"__pvcn_1193_"
#define flexidag_openseg_prim_split_3___pvcn_1193__is_constant 	1
#define flexidag_openseg_prim_split_3___pvcn_1193__has_init_data 	1
#define flexidag_openseg_prim_split_3___pvcn_1193__is_variable_scalar 	0
#define flexidag_openseg_prim_split_3___pvcn_1193__is_bitvector 	0
#define flexidag_openseg_prim_split_3___pvcn_1193__is_rlz 	0
#define flexidag_openseg_prim_split_3___pvcn_1193__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_3___pvcn_1193__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_3___pvcn_1193__byte_offset 	796	/* 0x0000031c */
#define flexidag_openseg_prim_split_3___pvcn_1193__vbase_byte_offset 	800	/* 0x00000320 */
#define flexidag_openseg_prim_split_3___pvcn_1193__dbase_byte_offset 	824	/* 0x00000338 */
#define flexidag_openseg_prim_split_3___pvcn_1193__dpitchm1_byte_offset 	828	/* 0x0000033c */
#define flexidag_openseg_prim_split_3___pvcn_1193__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_3___pvcn_1193__W 	1088
#define flexidag_openseg_prim_split_3___pvcn_1193__H 	1
#define flexidag_openseg_prim_split_3___pvcn_1193__D 	1
#define flexidag_openseg_prim_split_3___pvcn_1193__P 	1
#define flexidag_openseg_prim_split_3___pvcn_1193__data_num_bytes 	1
#define flexidag_openseg_prim_split_3___pvcn_1193__denable 	1
#define flexidag_openseg_prim_split_3___pvcn_1193__dpitch_num_bytes 	1088
#define flexidag_openseg_prim_split_3___pvcn_1193__dram_format 	0
#define flexidag_openseg_prim_split_3___pvcn_1193__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_3___pvcn_1193__tile_width 	1
#define flexidag_openseg_prim_split_3___pvcn_1193__tile_height 	1
#define flexidag_openseg_prim_split_3___pvcn_1193__vmem_buffer_num_bytes 	1088
#define flexidag_openseg_prim_split_3___pvcn_1193__dram_size_num_bytes 	1088

/* flexidag_openseg_prim_split_3 SMB_input __pvcn_1196_ */
#define flexidag_openseg_prim_split_3___pvcn_1196__cnngen_demangled_name 	"__pvcn_1196_"
#define flexidag_openseg_prim_split_3___pvcn_1196__is_constant 	1
#define flexidag_openseg_prim_split_3___pvcn_1196__has_init_data 	1
#define flexidag_openseg_prim_split_3___pvcn_1196__is_variable_scalar 	0
#define flexidag_openseg_prim_split_3___pvcn_1196__is_bitvector 	0
#define flexidag_openseg_prim_split_3___pvcn_1196__is_rlz 	0
#define flexidag_openseg_prim_split_3___pvcn_1196__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_3___pvcn_1196__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_3___pvcn_1196__byte_offset 	836	/* 0x00000344 */
#define flexidag_openseg_prim_split_3___pvcn_1196__vbase_byte_offset 	840	/* 0x00000348 */
#define flexidag_openseg_prim_split_3___pvcn_1196__dbase_byte_offset 	864	/* 0x00000360 */
#define flexidag_openseg_prim_split_3___pvcn_1196__dpitchm1_byte_offset 	868	/* 0x00000364 */
#define flexidag_openseg_prim_split_3___pvcn_1196__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_3___pvcn_1196__W 	5740
#define flexidag_openseg_prim_split_3___pvcn_1196__H 	1
#define flexidag_openseg_prim_split_3___pvcn_1196__D 	1
#define flexidag_openseg_prim_split_3___pvcn_1196__P 	1
#define flexidag_openseg_prim_split_3___pvcn_1196__data_num_bytes 	1
#define flexidag_openseg_prim_split_3___pvcn_1196__denable 	1
#define flexidag_openseg_prim_split_3___pvcn_1196__dpitch_num_bytes 	5760
#define flexidag_openseg_prim_split_3___pvcn_1196__dram_format 	0
#define flexidag_openseg_prim_split_3___pvcn_1196__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_3___pvcn_1196__tile_width 	1
#define flexidag_openseg_prim_split_3___pvcn_1196__tile_height 	1
#define flexidag_openseg_prim_split_3___pvcn_1196__vmem_buffer_num_bytes 	5740
#define flexidag_openseg_prim_split_3___pvcn_1196__dram_size_num_bytes 	5760

/* flexidag_openseg_prim_split_3 SMB_input __pvcn_1197_ */
#define flexidag_openseg_prim_split_3___pvcn_1197__cnngen_demangled_name 	"__pvcn_1197_"
#define flexidag_openseg_prim_split_3___pvcn_1197__is_constant 	1
#define flexidag_openseg_prim_split_3___pvcn_1197__has_init_data 	1
#define flexidag_openseg_prim_split_3___pvcn_1197__is_variable_scalar 	0
#define flexidag_openseg_prim_split_3___pvcn_1197__is_bitvector 	0
#define flexidag_openseg_prim_split_3___pvcn_1197__is_rlz 	0
#define flexidag_openseg_prim_split_3___pvcn_1197__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_3___pvcn_1197__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_3___pvcn_1197__byte_offset 	876	/* 0x0000036c */
#define flexidag_openseg_prim_split_3___pvcn_1197__vbase_byte_offset 	880	/* 0x00000370 */
#define flexidag_openseg_prim_split_3___pvcn_1197__dbase_byte_offset 	904	/* 0x00000388 */
#define flexidag_openseg_prim_split_3___pvcn_1197__dpitchm1_byte_offset 	908	/* 0x0000038c */
#define flexidag_openseg_prim_split_3___pvcn_1197__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_3___pvcn_1197__W 	144
#define flexidag_openseg_prim_split_3___pvcn_1197__H 	1
#define flexidag_openseg_prim_split_3___pvcn_1197__D 	1
#define flexidag_openseg_prim_split_3___pvcn_1197__P 	1
#define flexidag_openseg_prim_split_3___pvcn_1197__data_num_bytes 	1
#define flexidag_openseg_prim_split_3___pvcn_1197__denable 	1
#define flexidag_openseg_prim_split_3___pvcn_1197__dpitch_num_bytes 	160
#define flexidag_openseg_prim_split_3___pvcn_1197__dram_format 	0
#define flexidag_openseg_prim_split_3___pvcn_1197__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_3___pvcn_1197__tile_width 	1
#define flexidag_openseg_prim_split_3___pvcn_1197__tile_height 	1
#define flexidag_openseg_prim_split_3___pvcn_1197__vmem_buffer_num_bytes 	144
#define flexidag_openseg_prim_split_3___pvcn_1197__dram_size_num_bytes 	160

/* flexidag_openseg_prim_split_3 SMB_input __pvcn_1199_ */
#define flexidag_openseg_prim_split_3___pvcn_1199__cnngen_demangled_name 	"__pvcn_1199_"
#define flexidag_openseg_prim_split_3___pvcn_1199__is_constant 	1
#define flexidag_openseg_prim_split_3___pvcn_1199__has_init_data 	1
#define flexidag_openseg_prim_split_3___pvcn_1199__is_variable_scalar 	0
#define flexidag_openseg_prim_split_3___pvcn_1199__is_bitvector 	0
#define flexidag_openseg_prim_split_3___pvcn_1199__is_rlz 	0
#define flexidag_openseg_prim_split_3___pvcn_1199__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_3___pvcn_1199__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_3___pvcn_1199__byte_offset 	916	/* 0x00000394 */
#define flexidag_openseg_prim_split_3___pvcn_1199__vbase_byte_offset 	920	/* 0x00000398 */
#define flexidag_openseg_prim_split_3___pvcn_1199__dbase_byte_offset 	944	/* 0x000003b0 */
#define flexidag_openseg_prim_split_3___pvcn_1199__dpitchm1_byte_offset 	948	/* 0x000003b4 */
#define flexidag_openseg_prim_split_3___pvcn_1199__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_3___pvcn_1199__W 	1416
#define flexidag_openseg_prim_split_3___pvcn_1199__H 	1
#define flexidag_openseg_prim_split_3___pvcn_1199__D 	1
#define flexidag_openseg_prim_split_3___pvcn_1199__P 	1
#define flexidag_openseg_prim_split_3___pvcn_1199__data_num_bytes 	1
#define flexidag_openseg_prim_split_3___pvcn_1199__denable 	1
#define flexidag_openseg_prim_split_3___pvcn_1199__dpitch_num_bytes 	1440
#define flexidag_openseg_prim_split_3___pvcn_1199__dram_format 	0
#define flexidag_openseg_prim_split_3___pvcn_1199__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_3___pvcn_1199__tile_width 	1
#define flexidag_openseg_prim_split_3___pvcn_1199__tile_height 	1
#define flexidag_openseg_prim_split_3___pvcn_1199__vmem_buffer_num_bytes 	1416
#define flexidag_openseg_prim_split_3___pvcn_1199__dram_size_num_bytes 	1440

/* flexidag_openseg_prim_split_3 SMB_input __pvcn_1200_ */
#define flexidag_openseg_prim_split_3___pvcn_1200__cnngen_demangled_name 	"__pvcn_1200_"
#define flexidag_openseg_prim_split_3___pvcn_1200__is_constant 	1
#define flexidag_openseg_prim_split_3___pvcn_1200__has_init_data 	1
#define flexidag_openseg_prim_split_3___pvcn_1200__is_variable_scalar 	0
#define flexidag_openseg_prim_split_3___pvcn_1200__is_bitvector 	0
#define flexidag_openseg_prim_split_3___pvcn_1200__is_rlz 	0
#define flexidag_openseg_prim_split_3___pvcn_1200__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_3___pvcn_1200__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_3___pvcn_1200__byte_offset 	956	/* 0x000003bc */
#define flexidag_openseg_prim_split_3___pvcn_1200__vbase_byte_offset 	960	/* 0x000003c0 */
#define flexidag_openseg_prim_split_3___pvcn_1200__dbase_byte_offset 	984	/* 0x000003d8 */
#define flexidag_openseg_prim_split_3___pvcn_1200__dpitchm1_byte_offset 	988	/* 0x000003dc */
#define flexidag_openseg_prim_split_3___pvcn_1200__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_3___pvcn_1200__W 	144
#define flexidag_openseg_prim_split_3___pvcn_1200__H 	1
#define flexidag_openseg_prim_split_3___pvcn_1200__D 	1
#define flexidag_openseg_prim_split_3___pvcn_1200__P 	1
#define flexidag_openseg_prim_split_3___pvcn_1200__data_num_bytes 	1
#define flexidag_openseg_prim_split_3___pvcn_1200__denable 	1
#define flexidag_openseg_prim_split_3___pvcn_1200__dpitch_num_bytes 	160
#define flexidag_openseg_prim_split_3___pvcn_1200__dram_format 	0
#define flexidag_openseg_prim_split_3___pvcn_1200__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_3___pvcn_1200__tile_width 	1
#define flexidag_openseg_prim_split_3___pvcn_1200__tile_height 	1
#define flexidag_openseg_prim_split_3___pvcn_1200__vmem_buffer_num_bytes 	144
#define flexidag_openseg_prim_split_3___pvcn_1200__dram_size_num_bytes 	160

/* flexidag_openseg_prim_split_3 SMB_input __pvcn_1203_ */
#define flexidag_openseg_prim_split_3___pvcn_1203__cnngen_demangled_name 	"__pvcn_1203_"
#define flexidag_openseg_prim_split_3___pvcn_1203__is_constant 	1
#define flexidag_openseg_prim_split_3___pvcn_1203__has_init_data 	1
#define flexidag_openseg_prim_split_3___pvcn_1203__is_variable_scalar 	0
#define flexidag_openseg_prim_split_3___pvcn_1203__is_bitvector 	0
#define flexidag_openseg_prim_split_3___pvcn_1203__is_rlz 	0
#define flexidag_openseg_prim_split_3___pvcn_1203__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_3___pvcn_1203__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_3___pvcn_1203__byte_offset 	996	/* 0x000003e4 */
#define flexidag_openseg_prim_split_3___pvcn_1203__vbase_byte_offset 	1000	/* 0x000003e8 */
#define flexidag_openseg_prim_split_3___pvcn_1203__dbase_byte_offset 	1024	/* 0x00000400 */
#define flexidag_openseg_prim_split_3___pvcn_1203__dpitchm1_byte_offset 	1028	/* 0x00000404 */
#define flexidag_openseg_prim_split_3___pvcn_1203__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_3___pvcn_1203__W 	5780
#define flexidag_openseg_prim_split_3___pvcn_1203__H 	1
#define flexidag_openseg_prim_split_3___pvcn_1203__D 	1
#define flexidag_openseg_prim_split_3___pvcn_1203__P 	1
#define flexidag_openseg_prim_split_3___pvcn_1203__data_num_bytes 	1
#define flexidag_openseg_prim_split_3___pvcn_1203__denable 	1
#define flexidag_openseg_prim_split_3___pvcn_1203__dpitch_num_bytes 	5792
#define flexidag_openseg_prim_split_3___pvcn_1203__dram_format 	0
#define flexidag_openseg_prim_split_3___pvcn_1203__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_3___pvcn_1203__tile_width 	1
#define flexidag_openseg_prim_split_3___pvcn_1203__tile_height 	1
#define flexidag_openseg_prim_split_3___pvcn_1203__vmem_buffer_num_bytes 	5780
#define flexidag_openseg_prim_split_3___pvcn_1203__dram_size_num_bytes 	5792

/* flexidag_openseg_prim_split_3 SMB_input __pvcn_1204_ */
#define flexidag_openseg_prim_split_3___pvcn_1204__cnngen_demangled_name 	"__pvcn_1204_"
#define flexidag_openseg_prim_split_3___pvcn_1204__is_constant 	1
#define flexidag_openseg_prim_split_3___pvcn_1204__has_init_data 	1
#define flexidag_openseg_prim_split_3___pvcn_1204__is_variable_scalar 	0
#define flexidag_openseg_prim_split_3___pvcn_1204__is_bitvector 	0
#define flexidag_openseg_prim_split_3___pvcn_1204__is_rlz 	0
#define flexidag_openseg_prim_split_3___pvcn_1204__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_3___pvcn_1204__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_3___pvcn_1204__byte_offset 	1036	/* 0x0000040c */
#define flexidag_openseg_prim_split_3___pvcn_1204__vbase_byte_offset 	1040	/* 0x00000410 */
#define flexidag_openseg_prim_split_3___pvcn_1204__dbase_byte_offset 	1064	/* 0x00000428 */
#define flexidag_openseg_prim_split_3___pvcn_1204__dpitchm1_byte_offset 	1068	/* 0x0000042c */
#define flexidag_openseg_prim_split_3___pvcn_1204__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_3___pvcn_1204__W 	144
#define flexidag_openseg_prim_split_3___pvcn_1204__H 	1
#define flexidag_openseg_prim_split_3___pvcn_1204__D 	1
#define flexidag_openseg_prim_split_3___pvcn_1204__P 	1
#define flexidag_openseg_prim_split_3___pvcn_1204__data_num_bytes 	1
#define flexidag_openseg_prim_split_3___pvcn_1204__denable 	1
#define flexidag_openseg_prim_split_3___pvcn_1204__dpitch_num_bytes 	160
#define flexidag_openseg_prim_split_3___pvcn_1204__dram_format 	0
#define flexidag_openseg_prim_split_3___pvcn_1204__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_3___pvcn_1204__tile_width 	1
#define flexidag_openseg_prim_split_3___pvcn_1204__tile_height 	1
#define flexidag_openseg_prim_split_3___pvcn_1204__vmem_buffer_num_bytes 	144
#define flexidag_openseg_prim_split_3___pvcn_1204__dram_size_num_bytes 	160

/* flexidag_openseg_prim_split_3 VCB PeleeNet__stage_3_dense_block_2__concat */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_2__concat_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_2__concat"
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_2__concat_is_bitvector 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_2__concat_is_rlz 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_2__concat_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_2__concat_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_2__concat_byte_offset 	1076	/* 0x00000434 */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_2__concat_vbase_byte_offset 	1084	/* 0x0000043c */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_2__concat_vmem_buffer_num_bytes 	281600

/* flexidag_openseg_prim_split_3 VCB PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_130001_ */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_130001__cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_130001_"
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_130001__is_bitvector 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_130001__is_rlz 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_130001__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_130001__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_130001__byte_offset 	1128	/* 0x00000468 */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_130001__vbase_byte_offset 	1136	/* 0x00000470 */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_130001__vmem_buffer_num_bytes 	41984

/* flexidag_openseg_prim_split_3 VCB PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_130002_ */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_130002__cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_130002_"
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_130002__is_bitvector 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_130002__is_rlz 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_130002__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_130002__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_130002__byte_offset 	1172	/* 0x00000494 */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_130002__vbase_byte_offset 	1180	/* 0x0000049c */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_130002__vmem_buffer_num_bytes 	41984

/* flexidag_openseg_prim_split_3 VCB PeleeNet__stage_3_dense_block_2__conv_right_0__Relu */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_2__conv_right_0__Relu_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_2__conv_right_0__Relu"
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_2__conv_right_0__Relu_is_bitvector 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_2__conv_right_0__Relu_is_rlz 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_2__conv_right_0__Relu_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_2__conv_right_0__Relu_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_2__conv_right_0__Relu_byte_offset 	1208	/* 0x000004b8 */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_2__conv_right_0__Relu_vbase_byte_offset 	1216	/* 0x000004c0 */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_2__conv_right_0__Relu_vmem_buffer_num_bytes 	56320

/* flexidag_openseg_prim_split_3 VCB PeleeNet__stage_3_dense_block_2__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_2__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_2__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu"
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_2__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_bitvector 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_2__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_rlz 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_2__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_2__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_2__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_byte_offset 	1256	/* 0x000004e8 */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_2__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vbase_byte_offset 	1264	/* 0x000004f0 */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_2__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vmem_buffer_num_bytes 	14080

/* flexidag_openseg_prim_split_3 VCB PeleeNet__stage_3_dense_block_2__conv_left_0__Relu */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_2__conv_left_0__Relu_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_2__conv_left_0__Relu"
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_2__conv_left_0__Relu_is_bitvector 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_2__conv_left_0__Relu_is_rlz 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_2__conv_left_0__Relu_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_2__conv_left_0__Relu_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_2__conv_left_0__Relu_byte_offset 	1324	/* 0x0000052c */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_2__conv_left_0__Relu_vbase_byte_offset 	1332	/* 0x00000534 */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_2__conv_left_0__Relu_vmem_buffer_num_bytes 	28160

/* flexidag_openseg_prim_split_3 VCB PeleeNet__stage_3_dense_block_3__concat */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_3__concat_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_3__concat"
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_3__concat_is_bitvector 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_3__concat_is_rlz 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_3__concat_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_3__concat_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_3__concat_byte_offset 	1400	/* 0x00000578 */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_3__concat_vbase_byte_offset 	1408	/* 0x00000580 */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_3__concat_vmem_buffer_num_bytes 	380928

/* flexidag_openseg_prim_split_3 VCB PeleeNet__stage_3_dense_block_3__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_3__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_3__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm"
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_3__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_is_bitvector 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_3__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_is_rlz 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_3__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_3__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_3__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_byte_offset 	1472	/* 0x000005c0 */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_3__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_vbase_byte_offset 	1480	/* 0x000005c8 */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_3__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_vmem_buffer_num_bytes 	165888

/* flexidag_openseg_prim_split_3 VCB PeleeNet__stage_3_dense_block_3__conv_right_0__Relu */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_3__conv_right_0__Relu_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_3__conv_right_0__Relu"
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_3__conv_right_0__Relu_is_bitvector 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_3__conv_right_0__Relu_is_rlz 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_3__conv_right_0__Relu_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_3__conv_right_0__Relu_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_3__conv_right_0__Relu_byte_offset 	1524	/* 0x000005f4 */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_3__conv_right_0__Relu_vbase_byte_offset 	1532	/* 0x000005fc */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_3__conv_right_0__Relu_vmem_buffer_num_bytes 	43264

/* flexidag_openseg_prim_split_3 VCB PeleeNet__stage_3_dense_block_3__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_3__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_3__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu"
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_3__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_bitvector 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_3__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_rlz 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_3__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_3__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_3__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_byte_offset 	1572	/* 0x00000624 */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_3__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vbase_byte_offset 	1580	/* 0x0000062c */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_3__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vmem_buffer_num_bytes 	28032

/* flexidag_openseg_prim_split_3 VCB PeleeNet__stage_3_dense_block_3__conv_left_0__Relu */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_3__conv_left_0__Relu_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_3__conv_left_0__Relu"
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_3__conv_left_0__Relu_is_bitvector 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_3__conv_left_0__Relu_is_rlz 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_3__conv_left_0__Relu_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_3__conv_left_0__Relu_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_3__conv_left_0__Relu_byte_offset 	1644	/* 0x0000066c */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_3__conv_left_0__Relu_vbase_byte_offset 	1652	/* 0x00000674 */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_3__conv_left_0__Relu_vmem_buffer_num_bytes 	28160

/* flexidag_openseg_prim_split_3 HMB_output PeleeNet__stage_3_dense_block_4__concat */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__concat_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_4__concat"
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__concat_is_main_input_output 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__concat_is_bitvector 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__concat_is_rlz 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__concat_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__concat_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__concat_byte_offset 	1700	/* 0x000006a4 */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__concat_vbase_byte_offset 	1704	/* 0x000006a8 */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__concat_dbase_byte_offset 	1728	/* 0x000006c0 */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__concat_dpitchm1_byte_offset 	1732	/* 0x000006c4 */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__concat_dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__concat_vwidth_minus_one_byte_offset 	1716	/* 0x000006b4 */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__concat_vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__concat_vheight_minus_one_byte_offset 	1718	/* 0x000006b6 */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__concat_vheight_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__concat_drotate_bit_offset 	13798	/* 0x000035e6 */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__concat_hflip_bit_offset 	13799	/* 0x000035e7 */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__concat_vflip_bit_offset 	13800	/* 0x000035e8 */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__concat_dflip_bit_offset 	13801	/* 0x000035e9 */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__concat_pflip_bit_offset 	13802	/* 0x000035ea */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__concat_W 	80
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__concat_H 	32
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__concat_D 	416
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__concat_P 	1
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__concat_data_num_bytes 	1
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__concat_denable 	1
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__concat_dpitch_num_bytes 	32
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__concat_dram_format 	3
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__concat_vp_interleave_mode 	2
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__concat_tile_width 	4
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__concat_tile_height 	1
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__concat_vmem_buffer_num_bytes 	272896
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__concat_dram_size_num_bytes 	1064960
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__concat_sign 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__concat_datasize 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__concat_expoffset 	5
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__concat_expbits 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__concat_drotate 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__concat_hflip 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__concat_vflip 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__concat_dflip 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__concat_pflip 	0

/* flexidag_openseg_prim_split_3 VCB PeleeNet__stage_3_dense_block_4__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_4__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm"
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_is_bitvector 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_is_rlz 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_byte_offset 	1772	/* 0x000006ec */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_vbase_byte_offset 	1780	/* 0x000006f4 */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___BatchNorm__FusedBatchNorm_vmem_buffer_num_bytes 	124928

/* flexidag_openseg_prim_split_3 VCB PeleeNet__stage_3_dense_block_4__conv_right_0__Relu */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__conv_right_0__Relu_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_4__conv_right_0__Relu"
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__conv_right_0__Relu_is_bitvector 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__conv_right_0__Relu_is_rlz 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__conv_right_0__Relu_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__conv_right_0__Relu_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__conv_right_0__Relu_byte_offset 	1824	/* 0x00000720 */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__conv_right_0__Relu_vbase_byte_offset 	1832	/* 0x00000728 */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__conv_right_0__Relu_vmem_buffer_num_bytes 	43264

/* flexidag_openseg_prim_split_3 VCB PeleeNet__stage_3_dense_block_4__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_4__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu"
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_bitvector 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_rlz 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_byte_offset 	1872	/* 0x00000750 */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vbase_byte_offset 	1880	/* 0x00000758 */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vmem_buffer_num_bytes 	28032

/* flexidag_openseg_prim_split_3 VCB PeleeNet__stage_3_dense_block_4__conv_left_0__Relu */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__conv_left_0__Relu_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_4__conv_left_0__Relu"
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__conv_left_0__Relu_is_bitvector 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__conv_left_0__Relu_is_rlz 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__conv_left_0__Relu_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__conv_left_0__Relu_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__conv_left_0__Relu_byte_offset 	1944	/* 0x00000798 */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__conv_left_0__Relu_vbase_byte_offset 	1952	/* 0x000007a0 */
#define flexidag_openseg_prim_split_3_PeleeNet__stage_3_dense_block_4__conv_left_0__Relu_vmem_buffer_num_bytes 	43264

/* flexidag_openseg_prim_split_3 SMB_output split_3__vas_created_terminate__v0 */
#define flexidag_openseg_prim_split_3_split_3__vas_created_terminate__v0_cnngen_demangled_name 	"split_3__vas_created_terminate__v0"
#define flexidag_openseg_prim_split_3_split_3__vas_created_terminate__v0_is_main_input_output 	0
#define flexidag_openseg_prim_split_3_split_3__vas_created_terminate__v0_is_bitvector 	1
#define flexidag_openseg_prim_split_3_split_3__vas_created_terminate__v0_is_rlz 	0
#define flexidag_openseg_prim_split_3_split_3__vas_created_terminate__v0_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_3_split_3__vas_created_terminate__v0_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_3_split_3__vas_created_terminate__v0_byte_offset 	2004	/* 0x000007d4 */
#define flexidag_openseg_prim_split_3_split_3__vas_created_terminate__v0_vbase_byte_offset 	2008	/* 0x000007d8 */
#define flexidag_openseg_prim_split_3_split_3__vas_created_terminate__v0_W 	256
#define flexidag_openseg_prim_split_3_split_3__vas_created_terminate__v0_H 	3
#define flexidag_openseg_prim_split_3_split_3__vas_created_terminate__v0_D 	1
#define flexidag_openseg_prim_split_3_split_3__vas_created_terminate__v0_P 	1
#define flexidag_openseg_prim_split_3_split_3__vas_created_terminate__v0_data_num_bytes 	1
#define flexidag_openseg_prim_split_3_split_3__vas_created_terminate__v0_denable 	0
#define flexidag_openseg_prim_split_3_split_3__vas_created_terminate__v0_dram_format 	0
#define flexidag_openseg_prim_split_3_split_3__vas_created_terminate__v0_vp_interleave_mode 	1
#define flexidag_openseg_prim_split_3_split_3__vas_created_terminate__v0_tile_width 	1
#define flexidag_openseg_prim_split_3_split_3__vas_created_terminate__v0_tile_height 	1
#define flexidag_openseg_prim_split_3_split_3__vas_created_terminate__v0_vmem_buffer_num_bytes 	96


#endif /* flexidag_openseg_prim_split_3_H */
