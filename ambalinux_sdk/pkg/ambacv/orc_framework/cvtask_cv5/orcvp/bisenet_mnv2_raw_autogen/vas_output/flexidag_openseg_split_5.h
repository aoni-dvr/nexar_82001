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
#ifndef flexidag_openseg_prim_split_5_H
#define flexidag_openseg_prim_split_5_H

#define vas_gen_header_version                  	7

/* dag name */
#define flexidag_openseg_prim_split_5_DAG_NAME  	"flexidag_openseg_prim_split_5"

/* VDG file info */
#define flexidag_openseg_prim_split_5_vdg_name  	"flexidag_openseg_split_5.vdg"

/* VDG DAG memory info */
#define flexidag_openseg_prim_split_5_byte_offset 	4294967295	/* 0xffffffff */
#define flexidag_openseg_prim_split_5_byte_size 	2404	/* 0x00000964 */
#define flexidag_openseg_prim_split_5_preferred_dram_xfer_size 	1

/* flexidag_openseg_prim_split_5 VMEM info */
#define flexidag_openseg_prim_split_5_use_ping_pong_vmem 	0
#define flexidag_openseg_prim_split_5_VMEM_start 	4096	/* 0x00001000 */
#define flexidag_openseg_prim_split_5_VMEM_end  	1962568	/* 0x001df248 */
#define flexidag_openseg_prim_split_5_image_start 	1633184	/* 0x0018eba0 */
#define flexidag_openseg_prim_split_5_image_size 	331788	/* 0x0005100c */
#define flexidag_openseg_prim_split_5_dagbin_start 	1962568	/* 0x001df248 */

/* flexidag_openseg_prim_split_5 DAG info */
#define flexidag_openseg_prim_split_5_estimated_cycles 	9954384

/*******************************************************************************
 * DAG byte offset info for HMB/SMB/VCB and their fields
 * in DAG 'flexidag_openseg_prim_split_5' in source file 'flexidag_openseg_split_5.vas'
 ******************************************************************************/
/* flexidag_openseg_prim_split_5 HMB_input PeleeNet__stage_3_dense_block_7__concat */
#define flexidag_openseg_prim_split_5_PeleeNet__stage_3_dense_block_7__concat_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_7__concat"
#define flexidag_openseg_prim_split_5_PeleeNet__stage_3_dense_block_7__concat_is_main_input_output 	0
#define flexidag_openseg_prim_split_5_PeleeNet__stage_3_dense_block_7__concat_is_constant 	0
#define flexidag_openseg_prim_split_5_PeleeNet__stage_3_dense_block_7__concat_has_init_data 	0
#define flexidag_openseg_prim_split_5_PeleeNet__stage_3_dense_block_7__concat_is_variable_scalar 	0
#define flexidag_openseg_prim_split_5_PeleeNet__stage_3_dense_block_7__concat_ddi_byte_size 	0	/* 0x00000000 */
#define flexidag_openseg_prim_split_5_PeleeNet__stage_3_dense_block_7__concat_ddi_byte_offset 	4294967295	/* 0xffffffff */
#define flexidag_openseg_prim_split_5_PeleeNet__stage_3_dense_block_7__concat_is_bitvector 	0
#define flexidag_openseg_prim_split_5_PeleeNet__stage_3_dense_block_7__concat_is_rlz 	0
#define flexidag_openseg_prim_split_5_PeleeNet__stage_3_dense_block_7__concat_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_5_PeleeNet__stage_3_dense_block_7__concat_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_5_PeleeNet__stage_3_dense_block_7__concat_byte_offset 	0	/* 0x00000000 */
#define flexidag_openseg_prim_split_5_PeleeNet__stage_3_dense_block_7__concat_vbase_byte_offset 	4	/* 0x00000004 */
#define flexidag_openseg_prim_split_5_PeleeNet__stage_3_dense_block_7__concat_dbase_byte_offset 	28	/* 0x0000001c */
#define flexidag_openseg_prim_split_5_PeleeNet__stage_3_dense_block_7__concat_dpitchm1_byte_offset 	32	/* 0x00000020 */
#define flexidag_openseg_prim_split_5_PeleeNet__stage_3_dense_block_7__concat_dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_5_PeleeNet__stage_3_dense_block_7__concat_vwidth_minus_one_byte_offset 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_5_PeleeNet__stage_3_dense_block_7__concat_vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_5_PeleeNet__stage_3_dense_block_7__concat_vheight_minus_one_byte_offset 	18	/* 0x00000012 */
#define flexidag_openseg_prim_split_5_PeleeNet__stage_3_dense_block_7__concat_vheight_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_5_PeleeNet__stage_3_dense_block_7__concat_drotate_bit_offset 	198	/* 0x000000c6 */
#define flexidag_openseg_prim_split_5_PeleeNet__stage_3_dense_block_7__concat_hflip_bit_offset 	199	/* 0x000000c7 */
#define flexidag_openseg_prim_split_5_PeleeNet__stage_3_dense_block_7__concat_vflip_bit_offset 	200	/* 0x000000c8 */
#define flexidag_openseg_prim_split_5_PeleeNet__stage_3_dense_block_7__concat_dflip_bit_offset 	201	/* 0x000000c9 */
#define flexidag_openseg_prim_split_5_PeleeNet__stage_3_dense_block_7__concat_pflip_bit_offset 	202	/* 0x000000ca */
#define flexidag_openseg_prim_split_5_PeleeNet__stage_3_dense_block_7__concat_W 	80
#define flexidag_openseg_prim_split_5_PeleeNet__stage_3_dense_block_7__concat_H 	32
#define flexidag_openseg_prim_split_5_PeleeNet__stage_3_dense_block_7__concat_D 	512
#define flexidag_openseg_prim_split_5_PeleeNet__stage_3_dense_block_7__concat_P 	1
#define flexidag_openseg_prim_split_5_PeleeNet__stage_3_dense_block_7__concat_data_num_bytes 	1
#define flexidag_openseg_prim_split_5_PeleeNet__stage_3_dense_block_7__concat_denable 	1
#define flexidag_openseg_prim_split_5_PeleeNet__stage_3_dense_block_7__concat_dpitch_num_bytes 	32
#define flexidag_openseg_prim_split_5_PeleeNet__stage_3_dense_block_7__concat_dram_format 	3
#define flexidag_openseg_prim_split_5_PeleeNet__stage_3_dense_block_7__concat_vp_interleave_mode 	2
#define flexidag_openseg_prim_split_5_PeleeNet__stage_3_dense_block_7__concat_tile_width 	4
#define flexidag_openseg_prim_split_5_PeleeNet__stage_3_dense_block_7__concat_tile_height 	1
#define flexidag_openseg_prim_split_5_PeleeNet__stage_3_dense_block_7__concat_vmem_buffer_num_bytes 	335872
#define flexidag_openseg_prim_split_5_PeleeNet__stage_3_dense_block_7__concat_dram_size_num_bytes 	1310720
#define flexidag_openseg_prim_split_5_PeleeNet__stage_3_dense_block_7__concat_sign 	0
#define flexidag_openseg_prim_split_5_PeleeNet__stage_3_dense_block_7__concat_datasize 	0
#define flexidag_openseg_prim_split_5_PeleeNet__stage_3_dense_block_7__concat_expoffset 	5
#define flexidag_openseg_prim_split_5_PeleeNet__stage_3_dense_block_7__concat_expbits 	0
#define flexidag_openseg_prim_split_5_PeleeNet__stage_3_dense_block_7__concat_drotate 	0
#define flexidag_openseg_prim_split_5_PeleeNet__stage_3_dense_block_7__concat_hflip 	0
#define flexidag_openseg_prim_split_5_PeleeNet__stage_3_dense_block_7__concat_vflip 	0
#define flexidag_openseg_prim_split_5_PeleeNet__stage_3_dense_block_7__concat_dflip 	0
#define flexidag_openseg_prim_split_5_PeleeNet__stage_3_dense_block_7__concat_pflip 	0

/* flexidag_openseg_prim_split_5 SMB_input __pvcn_150004_ */
#define flexidag_openseg_prim_split_5___pvcn_150004__cnngen_demangled_name 	"__pvcn_150004_"
#define flexidag_openseg_prim_split_5___pvcn_150004__is_constant 	1
#define flexidag_openseg_prim_split_5___pvcn_150004__has_init_data 	1
#define flexidag_openseg_prim_split_5___pvcn_150004__is_variable_scalar 	0
#define flexidag_openseg_prim_split_5___pvcn_150004__is_bitvector 	0
#define flexidag_openseg_prim_split_5___pvcn_150004__is_rlz 	0
#define flexidag_openseg_prim_split_5___pvcn_150004__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_5___pvcn_150004__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_5___pvcn_150004__byte_offset 	40	/* 0x00000028 */
#define flexidag_openseg_prim_split_5___pvcn_150004__vbase_byte_offset 	44	/* 0x0000002c */
#define flexidag_openseg_prim_split_5___pvcn_150004__dbase_byte_offset 	68	/* 0x00000044 */
#define flexidag_openseg_prim_split_5___pvcn_150004__dpitchm1_byte_offset 	72	/* 0x00000048 */
#define flexidag_openseg_prim_split_5___pvcn_150004__dpitchm1_bsize 	32	/* 0x00000020 */
#define flexidag_openseg_prim_split_5___pvcn_150004__W 	81208
#define flexidag_openseg_prim_split_5___pvcn_150004__H 	1
#define flexidag_openseg_prim_split_5___pvcn_150004__D 	1
#define flexidag_openseg_prim_split_5___pvcn_150004__P 	1
#define flexidag_openseg_prim_split_5___pvcn_150004__data_num_bytes 	1
#define flexidag_openseg_prim_split_5___pvcn_150004__denable 	1
#define flexidag_openseg_prim_split_5___pvcn_150004__dpitch_num_bytes 	81216
#define flexidag_openseg_prim_split_5___pvcn_150004__dram_format 	0
#define flexidag_openseg_prim_split_5___pvcn_150004__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_5___pvcn_150004__tile_width 	1
#define flexidag_openseg_prim_split_5___pvcn_150004__tile_height 	1
#define flexidag_openseg_prim_split_5___pvcn_150004__vmem_buffer_num_bytes 	81208
#define flexidag_openseg_prim_split_5___pvcn_150004__dram_size_num_bytes 	81216

/* flexidag_openseg_prim_split_5 SMB_input __pvcn_150007_ */
#define flexidag_openseg_prim_split_5___pvcn_150007__cnngen_demangled_name 	"__pvcn_150007_"
#define flexidag_openseg_prim_split_5___pvcn_150007__is_constant 	1
#define flexidag_openseg_prim_split_5___pvcn_150007__has_init_data 	1
#define flexidag_openseg_prim_split_5___pvcn_150007__is_variable_scalar 	0
#define flexidag_openseg_prim_split_5___pvcn_150007__is_bitvector 	0
#define flexidag_openseg_prim_split_5___pvcn_150007__is_rlz 	0
#define flexidag_openseg_prim_split_5___pvcn_150007__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_5___pvcn_150007__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_5___pvcn_150007__byte_offset 	76	/* 0x0000004c */
#define flexidag_openseg_prim_split_5___pvcn_150007__vbase_byte_offset 	80	/* 0x00000050 */
#define flexidag_openseg_prim_split_5___pvcn_150007__dbase_byte_offset 	104	/* 0x00000068 */
#define flexidag_openseg_prim_split_5___pvcn_150007__dpitchm1_byte_offset 	108	/* 0x0000006c */
#define flexidag_openseg_prim_split_5___pvcn_150007__dpitchm1_bsize 	32	/* 0x00000020 */
#define flexidag_openseg_prim_split_5___pvcn_150007__W 	81292
#define flexidag_openseg_prim_split_5___pvcn_150007__H 	1
#define flexidag_openseg_prim_split_5___pvcn_150007__D 	1
#define flexidag_openseg_prim_split_5___pvcn_150007__P 	1
#define flexidag_openseg_prim_split_5___pvcn_150007__data_num_bytes 	1
#define flexidag_openseg_prim_split_5___pvcn_150007__denable 	1
#define flexidag_openseg_prim_split_5___pvcn_150007__dpitch_num_bytes 	81312
#define flexidag_openseg_prim_split_5___pvcn_150007__dram_format 	0
#define flexidag_openseg_prim_split_5___pvcn_150007__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_5___pvcn_150007__tile_width 	1
#define flexidag_openseg_prim_split_5___pvcn_150007__tile_height 	1
#define flexidag_openseg_prim_split_5___pvcn_150007__vmem_buffer_num_bytes 	81292
#define flexidag_openseg_prim_split_5___pvcn_150007__dram_size_num_bytes 	81312

/* flexidag_openseg_prim_split_5 SMB_input __pvcn_150005_ */
#define flexidag_openseg_prim_split_5___pvcn_150005__cnngen_demangled_name 	"__pvcn_150005_"
#define flexidag_openseg_prim_split_5___pvcn_150005__is_constant 	1
#define flexidag_openseg_prim_split_5___pvcn_150005__has_init_data 	1
#define flexidag_openseg_prim_split_5___pvcn_150005__is_variable_scalar 	0
#define flexidag_openseg_prim_split_5___pvcn_150005__is_bitvector 	0
#define flexidag_openseg_prim_split_5___pvcn_150005__is_rlz 	0
#define flexidag_openseg_prim_split_5___pvcn_150005__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_5___pvcn_150005__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_5___pvcn_150005__byte_offset 	116	/* 0x00000074 */
#define flexidag_openseg_prim_split_5___pvcn_150005__vbase_byte_offset 	120	/* 0x00000078 */
#define flexidag_openseg_prim_split_5___pvcn_150005__dbase_byte_offset 	144	/* 0x00000090 */
#define flexidag_openseg_prim_split_5___pvcn_150005__dpitchm1_byte_offset 	148	/* 0x00000094 */
#define flexidag_openseg_prim_split_5___pvcn_150005__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_5___pvcn_150005__W 	2176
#define flexidag_openseg_prim_split_5___pvcn_150005__H 	1
#define flexidag_openseg_prim_split_5___pvcn_150005__D 	1
#define flexidag_openseg_prim_split_5___pvcn_150005__P 	1
#define flexidag_openseg_prim_split_5___pvcn_150005__data_num_bytes 	1
#define flexidag_openseg_prim_split_5___pvcn_150005__denable 	1
#define flexidag_openseg_prim_split_5___pvcn_150005__dpitch_num_bytes 	2176
#define flexidag_openseg_prim_split_5___pvcn_150005__dram_format 	0
#define flexidag_openseg_prim_split_5___pvcn_150005__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_5___pvcn_150005__tile_width 	1
#define flexidag_openseg_prim_split_5___pvcn_150005__tile_height 	1
#define flexidag_openseg_prim_split_5___pvcn_150005__vmem_buffer_num_bytes 	2176
#define flexidag_openseg_prim_split_5___pvcn_150005__dram_size_num_bytes 	2176

/* flexidag_openseg_prim_split_5 SMB_input __pvcn_150008_ */
#define flexidag_openseg_prim_split_5___pvcn_150008__cnngen_demangled_name 	"__pvcn_150008_"
#define flexidag_openseg_prim_split_5___pvcn_150008__is_constant 	1
#define flexidag_openseg_prim_split_5___pvcn_150008__has_init_data 	1
#define flexidag_openseg_prim_split_5___pvcn_150008__is_variable_scalar 	0
#define flexidag_openseg_prim_split_5___pvcn_150008__is_bitvector 	0
#define flexidag_openseg_prim_split_5___pvcn_150008__is_rlz 	0
#define flexidag_openseg_prim_split_5___pvcn_150008__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_5___pvcn_150008__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_5___pvcn_150008__byte_offset 	156	/* 0x0000009c */
#define flexidag_openseg_prim_split_5___pvcn_150008__vbase_byte_offset 	160	/* 0x000000a0 */
#define flexidag_openseg_prim_split_5___pvcn_150008__dbase_byte_offset 	184	/* 0x000000b8 */
#define flexidag_openseg_prim_split_5___pvcn_150008__dpitchm1_byte_offset 	188	/* 0x000000bc */
#define flexidag_openseg_prim_split_5___pvcn_150008__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_5___pvcn_150008__W 	2176
#define flexidag_openseg_prim_split_5___pvcn_150008__H 	1
#define flexidag_openseg_prim_split_5___pvcn_150008__D 	1
#define flexidag_openseg_prim_split_5___pvcn_150008__P 	1
#define flexidag_openseg_prim_split_5___pvcn_150008__data_num_bytes 	1
#define flexidag_openseg_prim_split_5___pvcn_150008__denable 	1
#define flexidag_openseg_prim_split_5___pvcn_150008__dpitch_num_bytes 	2176
#define flexidag_openseg_prim_split_5___pvcn_150008__dram_format 	0
#define flexidag_openseg_prim_split_5___pvcn_150008__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_5___pvcn_150008__tile_width 	1
#define flexidag_openseg_prim_split_5___pvcn_150008__tile_height 	1
#define flexidag_openseg_prim_split_5___pvcn_150008__vmem_buffer_num_bytes 	2176
#define flexidag_openseg_prim_split_5___pvcn_150008__dram_size_num_bytes 	2176

/* flexidag_openseg_prim_split_5 SMB_input __pvcn_1295_ */
#define flexidag_openseg_prim_split_5___pvcn_1295__cnngen_demangled_name 	"__pvcn_1295_"
#define flexidag_openseg_prim_split_5___pvcn_1295__is_constant 	1
#define flexidag_openseg_prim_split_5___pvcn_1295__has_init_data 	1
#define flexidag_openseg_prim_split_5___pvcn_1295__is_variable_scalar 	0
#define flexidag_openseg_prim_split_5___pvcn_1295__is_bitvector 	0
#define flexidag_openseg_prim_split_5___pvcn_1295__is_rlz 	0
#define flexidag_openseg_prim_split_5___pvcn_1295__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_5___pvcn_1295__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_5___pvcn_1295__byte_offset 	196	/* 0x000000c4 */
#define flexidag_openseg_prim_split_5___pvcn_1295__vbase_byte_offset 	200	/* 0x000000c8 */
#define flexidag_openseg_prim_split_5___pvcn_1295__dbase_byte_offset 	224	/* 0x000000e0 */
#define flexidag_openseg_prim_split_5___pvcn_1295__dpitchm1_byte_offset 	228	/* 0x000000e4 */
#define flexidag_openseg_prim_split_5___pvcn_1295__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_5___pvcn_1295__W 	512
#define flexidag_openseg_prim_split_5___pvcn_1295__H 	1
#define flexidag_openseg_prim_split_5___pvcn_1295__D 	1
#define flexidag_openseg_prim_split_5___pvcn_1295__P 	1
#define flexidag_openseg_prim_split_5___pvcn_1295__data_num_bytes 	1
#define flexidag_openseg_prim_split_5___pvcn_1295__denable 	1
#define flexidag_openseg_prim_split_5___pvcn_1295__dpitch_num_bytes 	512
#define flexidag_openseg_prim_split_5___pvcn_1295__dram_format 	0
#define flexidag_openseg_prim_split_5___pvcn_1295__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_5___pvcn_1295__tile_width 	1
#define flexidag_openseg_prim_split_5___pvcn_1295__tile_height 	1
#define flexidag_openseg_prim_split_5___pvcn_1295__vmem_buffer_num_bytes 	512
#define flexidag_openseg_prim_split_5___pvcn_1295__dram_size_num_bytes 	512

/* flexidag_openseg_prim_split_5 SMB_input __pvcn_1296_ */
#define flexidag_openseg_prim_split_5___pvcn_1296__cnngen_demangled_name 	"__pvcn_1296_"
#define flexidag_openseg_prim_split_5___pvcn_1296__is_constant 	1
#define flexidag_openseg_prim_split_5___pvcn_1296__has_init_data 	1
#define flexidag_openseg_prim_split_5___pvcn_1296__is_variable_scalar 	0
#define flexidag_openseg_prim_split_5___pvcn_1296__is_bitvector 	0
#define flexidag_openseg_prim_split_5___pvcn_1296__is_rlz 	0
#define flexidag_openseg_prim_split_5___pvcn_1296__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_5___pvcn_1296__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_5___pvcn_1296__byte_offset 	236	/* 0x000000ec */
#define flexidag_openseg_prim_split_5___pvcn_1296__vbase_byte_offset 	240	/* 0x000000f0 */
#define flexidag_openseg_prim_split_5___pvcn_1296__dbase_byte_offset 	264	/* 0x00000108 */
#define flexidag_openseg_prim_split_5___pvcn_1296__dpitchm1_byte_offset 	268	/* 0x0000010c */
#define flexidag_openseg_prim_split_5___pvcn_1296__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_5___pvcn_1296__W 	360
#define flexidag_openseg_prim_split_5___pvcn_1296__H 	1
#define flexidag_openseg_prim_split_5___pvcn_1296__D 	1
#define flexidag_openseg_prim_split_5___pvcn_1296__P 	1
#define flexidag_openseg_prim_split_5___pvcn_1296__data_num_bytes 	1
#define flexidag_openseg_prim_split_5___pvcn_1296__denable 	1
#define flexidag_openseg_prim_split_5___pvcn_1296__dpitch_num_bytes 	384
#define flexidag_openseg_prim_split_5___pvcn_1296__dram_format 	0
#define flexidag_openseg_prim_split_5___pvcn_1296__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_5___pvcn_1296__tile_width 	1
#define flexidag_openseg_prim_split_5___pvcn_1296__tile_height 	1
#define flexidag_openseg_prim_split_5___pvcn_1296__vmem_buffer_num_bytes 	360
#define flexidag_openseg_prim_split_5___pvcn_1296__dram_size_num_bytes 	384

/* flexidag_openseg_prim_split_5 SMB_input __pvcn_1302_ */
#define flexidag_openseg_prim_split_5___pvcn_1302__cnngen_demangled_name 	"__pvcn_1302_"
#define flexidag_openseg_prim_split_5___pvcn_1302__is_constant 	1
#define flexidag_openseg_prim_split_5___pvcn_1302__has_init_data 	1
#define flexidag_openseg_prim_split_5___pvcn_1302__is_variable_scalar 	0
#define flexidag_openseg_prim_split_5___pvcn_1302__is_bitvector 	0
#define flexidag_openseg_prim_split_5___pvcn_1302__is_rlz 	0
#define flexidag_openseg_prim_split_5___pvcn_1302__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_5___pvcn_1302__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_5___pvcn_1302__byte_offset 	276	/* 0x00000114 */
#define flexidag_openseg_prim_split_5___pvcn_1302__vbase_byte_offset 	280	/* 0x00000118 */
#define flexidag_openseg_prim_split_5___pvcn_1302__dbase_byte_offset 	304	/* 0x00000130 */
#define flexidag_openseg_prim_split_5___pvcn_1302__dpitchm1_byte_offset 	308	/* 0x00000134 */
#define flexidag_openseg_prim_split_5___pvcn_1302__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_5___pvcn_1302__W 	512
#define flexidag_openseg_prim_split_5___pvcn_1302__H 	1
#define flexidag_openseg_prim_split_5___pvcn_1302__D 	1
#define flexidag_openseg_prim_split_5___pvcn_1302__P 	1
#define flexidag_openseg_prim_split_5___pvcn_1302__data_num_bytes 	1
#define flexidag_openseg_prim_split_5___pvcn_1302__denable 	1
#define flexidag_openseg_prim_split_5___pvcn_1302__dpitch_num_bytes 	512
#define flexidag_openseg_prim_split_5___pvcn_1302__dram_format 	0
#define flexidag_openseg_prim_split_5___pvcn_1302__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_5___pvcn_1302__tile_width 	1
#define flexidag_openseg_prim_split_5___pvcn_1302__tile_height 	1
#define flexidag_openseg_prim_split_5___pvcn_1302__vmem_buffer_num_bytes 	512
#define flexidag_openseg_prim_split_5___pvcn_1302__dram_size_num_bytes 	512

/* flexidag_openseg_prim_split_5 SMB_input __pvcn_1303_ */
#define flexidag_openseg_prim_split_5___pvcn_1303__cnngen_demangled_name 	"__pvcn_1303_"
#define flexidag_openseg_prim_split_5___pvcn_1303__is_constant 	1
#define flexidag_openseg_prim_split_5___pvcn_1303__has_init_data 	1
#define flexidag_openseg_prim_split_5___pvcn_1303__is_variable_scalar 	0
#define flexidag_openseg_prim_split_5___pvcn_1303__is_bitvector 	0
#define flexidag_openseg_prim_split_5___pvcn_1303__is_rlz 	0
#define flexidag_openseg_prim_split_5___pvcn_1303__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_5___pvcn_1303__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_5___pvcn_1303__byte_offset 	316	/* 0x0000013c */
#define flexidag_openseg_prim_split_5___pvcn_1303__vbase_byte_offset 	320	/* 0x00000140 */
#define flexidag_openseg_prim_split_5___pvcn_1303__dbase_byte_offset 	344	/* 0x00000158 */
#define flexidag_openseg_prim_split_5___pvcn_1303__dpitchm1_byte_offset 	348	/* 0x0000015c */
#define flexidag_openseg_prim_split_5___pvcn_1303__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_5___pvcn_1303__W 	360
#define flexidag_openseg_prim_split_5___pvcn_1303__H 	1
#define flexidag_openseg_prim_split_5___pvcn_1303__D 	1
#define flexidag_openseg_prim_split_5___pvcn_1303__P 	1
#define flexidag_openseg_prim_split_5___pvcn_1303__data_num_bytes 	1
#define flexidag_openseg_prim_split_5___pvcn_1303__denable 	1
#define flexidag_openseg_prim_split_5___pvcn_1303__dpitch_num_bytes 	384
#define flexidag_openseg_prim_split_5___pvcn_1303__dram_format 	0
#define flexidag_openseg_prim_split_5___pvcn_1303__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_5___pvcn_1303__tile_width 	1
#define flexidag_openseg_prim_split_5___pvcn_1303__tile_height 	1
#define flexidag_openseg_prim_split_5___pvcn_1303__vmem_buffer_num_bytes 	360
#define flexidag_openseg_prim_split_5___pvcn_1303__dram_size_num_bytes 	384

/* flexidag_openseg_prim_split_5 SMB_input __pvcn_1309_ */
#define flexidag_openseg_prim_split_5___pvcn_1309__cnngen_demangled_name 	"__pvcn_1309_"
#define flexidag_openseg_prim_split_5___pvcn_1309__is_constant 	1
#define flexidag_openseg_prim_split_5___pvcn_1309__has_init_data 	1
#define flexidag_openseg_prim_split_5___pvcn_1309__is_variable_scalar 	0
#define flexidag_openseg_prim_split_5___pvcn_1309__is_bitvector 	0
#define flexidag_openseg_prim_split_5___pvcn_1309__is_rlz 	0
#define flexidag_openseg_prim_split_5___pvcn_1309__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_5___pvcn_1309__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_5___pvcn_1309__byte_offset 	356	/* 0x00000164 */
#define flexidag_openseg_prim_split_5___pvcn_1309__vbase_byte_offset 	360	/* 0x00000168 */
#define flexidag_openseg_prim_split_5___pvcn_1309__dbase_byte_offset 	384	/* 0x00000180 */
#define flexidag_openseg_prim_split_5___pvcn_1309__dpitchm1_byte_offset 	388	/* 0x00000184 */
#define flexidag_openseg_prim_split_5___pvcn_1309__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_5___pvcn_1309__W 	512
#define flexidag_openseg_prim_split_5___pvcn_1309__H 	1
#define flexidag_openseg_prim_split_5___pvcn_1309__D 	1
#define flexidag_openseg_prim_split_5___pvcn_1309__P 	1
#define flexidag_openseg_prim_split_5___pvcn_1309__data_num_bytes 	1
#define flexidag_openseg_prim_split_5___pvcn_1309__denable 	1
#define flexidag_openseg_prim_split_5___pvcn_1309__dpitch_num_bytes 	512
#define flexidag_openseg_prim_split_5___pvcn_1309__dram_format 	0
#define flexidag_openseg_prim_split_5___pvcn_1309__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_5___pvcn_1309__tile_width 	1
#define flexidag_openseg_prim_split_5___pvcn_1309__tile_height 	1
#define flexidag_openseg_prim_split_5___pvcn_1309__vmem_buffer_num_bytes 	512
#define flexidag_openseg_prim_split_5___pvcn_1309__dram_size_num_bytes 	512

/* flexidag_openseg_prim_split_5 SMB_input __pvcn_1310_ */
#define flexidag_openseg_prim_split_5___pvcn_1310__cnngen_demangled_name 	"__pvcn_1310_"
#define flexidag_openseg_prim_split_5___pvcn_1310__is_constant 	1
#define flexidag_openseg_prim_split_5___pvcn_1310__has_init_data 	1
#define flexidag_openseg_prim_split_5___pvcn_1310__is_variable_scalar 	0
#define flexidag_openseg_prim_split_5___pvcn_1310__is_bitvector 	0
#define flexidag_openseg_prim_split_5___pvcn_1310__is_rlz 	0
#define flexidag_openseg_prim_split_5___pvcn_1310__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_5___pvcn_1310__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_5___pvcn_1310__byte_offset 	396	/* 0x0000018c */
#define flexidag_openseg_prim_split_5___pvcn_1310__vbase_byte_offset 	400	/* 0x00000190 */
#define flexidag_openseg_prim_split_5___pvcn_1310__dbase_byte_offset 	424	/* 0x000001a8 */
#define flexidag_openseg_prim_split_5___pvcn_1310__dpitchm1_byte_offset 	428	/* 0x000001ac */
#define flexidag_openseg_prim_split_5___pvcn_1310__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_5___pvcn_1310__W 	360
#define flexidag_openseg_prim_split_5___pvcn_1310__H 	1
#define flexidag_openseg_prim_split_5___pvcn_1310__D 	1
#define flexidag_openseg_prim_split_5___pvcn_1310__P 	1
#define flexidag_openseg_prim_split_5___pvcn_1310__data_num_bytes 	1
#define flexidag_openseg_prim_split_5___pvcn_1310__denable 	1
#define flexidag_openseg_prim_split_5___pvcn_1310__dpitch_num_bytes 	384
#define flexidag_openseg_prim_split_5___pvcn_1310__dram_format 	0
#define flexidag_openseg_prim_split_5___pvcn_1310__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_5___pvcn_1310__tile_width 	1
#define flexidag_openseg_prim_split_5___pvcn_1310__tile_height 	1
#define flexidag_openseg_prim_split_5___pvcn_1310__vmem_buffer_num_bytes 	360
#define flexidag_openseg_prim_split_5___pvcn_1310__dram_size_num_bytes 	384

/* flexidag_openseg_prim_split_5 SMB_input __pvcn_1316_ */
#define flexidag_openseg_prim_split_5___pvcn_1316__cnngen_demangled_name 	"__pvcn_1316_"
#define flexidag_openseg_prim_split_5___pvcn_1316__is_constant 	1
#define flexidag_openseg_prim_split_5___pvcn_1316__has_init_data 	1
#define flexidag_openseg_prim_split_5___pvcn_1316__is_variable_scalar 	0
#define flexidag_openseg_prim_split_5___pvcn_1316__is_bitvector 	0
#define flexidag_openseg_prim_split_5___pvcn_1316__is_rlz 	0
#define flexidag_openseg_prim_split_5___pvcn_1316__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_5___pvcn_1316__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_5___pvcn_1316__byte_offset 	436	/* 0x000001b4 */
#define flexidag_openseg_prim_split_5___pvcn_1316__vbase_byte_offset 	440	/* 0x000001b8 */
#define flexidag_openseg_prim_split_5___pvcn_1316__dbase_byte_offset 	464	/* 0x000001d0 */
#define flexidag_openseg_prim_split_5___pvcn_1316__dpitchm1_byte_offset 	468	/* 0x000001d4 */
#define flexidag_openseg_prim_split_5___pvcn_1316__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_5___pvcn_1316__W 	512
#define flexidag_openseg_prim_split_5___pvcn_1316__H 	1
#define flexidag_openseg_prim_split_5___pvcn_1316__D 	1
#define flexidag_openseg_prim_split_5___pvcn_1316__P 	1
#define flexidag_openseg_prim_split_5___pvcn_1316__data_num_bytes 	1
#define flexidag_openseg_prim_split_5___pvcn_1316__denable 	1
#define flexidag_openseg_prim_split_5___pvcn_1316__dpitch_num_bytes 	512
#define flexidag_openseg_prim_split_5___pvcn_1316__dram_format 	0
#define flexidag_openseg_prim_split_5___pvcn_1316__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_5___pvcn_1316__tile_width 	1
#define flexidag_openseg_prim_split_5___pvcn_1316__tile_height 	1
#define flexidag_openseg_prim_split_5___pvcn_1316__vmem_buffer_num_bytes 	512
#define flexidag_openseg_prim_split_5___pvcn_1316__dram_size_num_bytes 	512

/* flexidag_openseg_prim_split_5 SMB_input __pvcn_1317_ */
#define flexidag_openseg_prim_split_5___pvcn_1317__cnngen_demangled_name 	"__pvcn_1317_"
#define flexidag_openseg_prim_split_5___pvcn_1317__is_constant 	1
#define flexidag_openseg_prim_split_5___pvcn_1317__has_init_data 	1
#define flexidag_openseg_prim_split_5___pvcn_1317__is_variable_scalar 	0
#define flexidag_openseg_prim_split_5___pvcn_1317__is_bitvector 	0
#define flexidag_openseg_prim_split_5___pvcn_1317__is_rlz 	0
#define flexidag_openseg_prim_split_5___pvcn_1317__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_5___pvcn_1317__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_5___pvcn_1317__byte_offset 	476	/* 0x000001dc */
#define flexidag_openseg_prim_split_5___pvcn_1317__vbase_byte_offset 	480	/* 0x000001e0 */
#define flexidag_openseg_prim_split_5___pvcn_1317__dbase_byte_offset 	504	/* 0x000001f8 */
#define flexidag_openseg_prim_split_5___pvcn_1317__dpitchm1_byte_offset 	508	/* 0x000001fc */
#define flexidag_openseg_prim_split_5___pvcn_1317__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_5___pvcn_1317__W 	360
#define flexidag_openseg_prim_split_5___pvcn_1317__H 	1
#define flexidag_openseg_prim_split_5___pvcn_1317__D 	1
#define flexidag_openseg_prim_split_5___pvcn_1317__P 	1
#define flexidag_openseg_prim_split_5___pvcn_1317__data_num_bytes 	1
#define flexidag_openseg_prim_split_5___pvcn_1317__denable 	1
#define flexidag_openseg_prim_split_5___pvcn_1317__dpitch_num_bytes 	384
#define flexidag_openseg_prim_split_5___pvcn_1317__dram_format 	0
#define flexidag_openseg_prim_split_5___pvcn_1317__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_5___pvcn_1317__tile_width 	1
#define flexidag_openseg_prim_split_5___pvcn_1317__tile_height 	1
#define flexidag_openseg_prim_split_5___pvcn_1317__vmem_buffer_num_bytes 	360
#define flexidag_openseg_prim_split_5___pvcn_1317__dram_size_num_bytes 	384

/* flexidag_openseg_prim_split_5 SMB_input __pvcn_1323_ */
#define flexidag_openseg_prim_split_5___pvcn_1323__cnngen_demangled_name 	"__pvcn_1323_"
#define flexidag_openseg_prim_split_5___pvcn_1323__is_constant 	1
#define flexidag_openseg_prim_split_5___pvcn_1323__has_init_data 	1
#define flexidag_openseg_prim_split_5___pvcn_1323__is_variable_scalar 	0
#define flexidag_openseg_prim_split_5___pvcn_1323__is_bitvector 	0
#define flexidag_openseg_prim_split_5___pvcn_1323__is_rlz 	0
#define flexidag_openseg_prim_split_5___pvcn_1323__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_5___pvcn_1323__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_5___pvcn_1323__byte_offset 	516	/* 0x00000204 */
#define flexidag_openseg_prim_split_5___pvcn_1323__vbase_byte_offset 	520	/* 0x00000208 */
#define flexidag_openseg_prim_split_5___pvcn_1323__dbase_byte_offset 	544	/* 0x00000220 */
#define flexidag_openseg_prim_split_5___pvcn_1323__dpitchm1_byte_offset 	548	/* 0x00000224 */
#define flexidag_openseg_prim_split_5___pvcn_1323__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_5___pvcn_1323__W 	512
#define flexidag_openseg_prim_split_5___pvcn_1323__H 	1
#define flexidag_openseg_prim_split_5___pvcn_1323__D 	1
#define flexidag_openseg_prim_split_5___pvcn_1323__P 	1
#define flexidag_openseg_prim_split_5___pvcn_1323__data_num_bytes 	1
#define flexidag_openseg_prim_split_5___pvcn_1323__denable 	1
#define flexidag_openseg_prim_split_5___pvcn_1323__dpitch_num_bytes 	512
#define flexidag_openseg_prim_split_5___pvcn_1323__dram_format 	0
#define flexidag_openseg_prim_split_5___pvcn_1323__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_5___pvcn_1323__tile_width 	1
#define flexidag_openseg_prim_split_5___pvcn_1323__tile_height 	1
#define flexidag_openseg_prim_split_5___pvcn_1323__vmem_buffer_num_bytes 	512
#define flexidag_openseg_prim_split_5___pvcn_1323__dram_size_num_bytes 	512

/* flexidag_openseg_prim_split_5 SMB_input __pvcn_1324_ */
#define flexidag_openseg_prim_split_5___pvcn_1324__cnngen_demangled_name 	"__pvcn_1324_"
#define flexidag_openseg_prim_split_5___pvcn_1324__is_constant 	1
#define flexidag_openseg_prim_split_5___pvcn_1324__has_init_data 	1
#define flexidag_openseg_prim_split_5___pvcn_1324__is_variable_scalar 	0
#define flexidag_openseg_prim_split_5___pvcn_1324__is_bitvector 	0
#define flexidag_openseg_prim_split_5___pvcn_1324__is_rlz 	0
#define flexidag_openseg_prim_split_5___pvcn_1324__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_5___pvcn_1324__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_5___pvcn_1324__byte_offset 	556	/* 0x0000022c */
#define flexidag_openseg_prim_split_5___pvcn_1324__vbase_byte_offset 	560	/* 0x00000230 */
#define flexidag_openseg_prim_split_5___pvcn_1324__dbase_byte_offset 	584	/* 0x00000248 */
#define flexidag_openseg_prim_split_5___pvcn_1324__dpitchm1_byte_offset 	588	/* 0x0000024c */
#define flexidag_openseg_prim_split_5___pvcn_1324__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_5___pvcn_1324__W 	360
#define flexidag_openseg_prim_split_5___pvcn_1324__H 	1
#define flexidag_openseg_prim_split_5___pvcn_1324__D 	1
#define flexidag_openseg_prim_split_5___pvcn_1324__P 	1
#define flexidag_openseg_prim_split_5___pvcn_1324__data_num_bytes 	1
#define flexidag_openseg_prim_split_5___pvcn_1324__denable 	1
#define flexidag_openseg_prim_split_5___pvcn_1324__dpitch_num_bytes 	384
#define flexidag_openseg_prim_split_5___pvcn_1324__dram_format 	0
#define flexidag_openseg_prim_split_5___pvcn_1324__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_5___pvcn_1324__tile_width 	1
#define flexidag_openseg_prim_split_5___pvcn_1324__tile_height 	1
#define flexidag_openseg_prim_split_5___pvcn_1324__vmem_buffer_num_bytes 	360
#define flexidag_openseg_prim_split_5___pvcn_1324__dram_size_num_bytes 	384

/* flexidag_openseg_prim_split_5 SMB_input __pvcn_1330_ */
#define flexidag_openseg_prim_split_5___pvcn_1330__cnngen_demangled_name 	"__pvcn_1330_"
#define flexidag_openseg_prim_split_5___pvcn_1330__is_constant 	1
#define flexidag_openseg_prim_split_5___pvcn_1330__has_init_data 	1
#define flexidag_openseg_prim_split_5___pvcn_1330__is_variable_scalar 	0
#define flexidag_openseg_prim_split_5___pvcn_1330__is_bitvector 	0
#define flexidag_openseg_prim_split_5___pvcn_1330__is_rlz 	0
#define flexidag_openseg_prim_split_5___pvcn_1330__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_5___pvcn_1330__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_5___pvcn_1330__byte_offset 	596	/* 0x00000254 */
#define flexidag_openseg_prim_split_5___pvcn_1330__vbase_byte_offset 	600	/* 0x00000258 */
#define flexidag_openseg_prim_split_5___pvcn_1330__dbase_byte_offset 	624	/* 0x00000270 */
#define flexidag_openseg_prim_split_5___pvcn_1330__dpitchm1_byte_offset 	628	/* 0x00000274 */
#define flexidag_openseg_prim_split_5___pvcn_1330__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_5___pvcn_1330__W 	512
#define flexidag_openseg_prim_split_5___pvcn_1330__H 	1
#define flexidag_openseg_prim_split_5___pvcn_1330__D 	1
#define flexidag_openseg_prim_split_5___pvcn_1330__P 	1
#define flexidag_openseg_prim_split_5___pvcn_1330__data_num_bytes 	1
#define flexidag_openseg_prim_split_5___pvcn_1330__denable 	1
#define flexidag_openseg_prim_split_5___pvcn_1330__dpitch_num_bytes 	512
#define flexidag_openseg_prim_split_5___pvcn_1330__dram_format 	0
#define flexidag_openseg_prim_split_5___pvcn_1330__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_5___pvcn_1330__tile_width 	1
#define flexidag_openseg_prim_split_5___pvcn_1330__tile_height 	1
#define flexidag_openseg_prim_split_5___pvcn_1330__vmem_buffer_num_bytes 	512
#define flexidag_openseg_prim_split_5___pvcn_1330__dram_size_num_bytes 	512

/* flexidag_openseg_prim_split_5 SMB_input __pvcn_1331_ */
#define flexidag_openseg_prim_split_5___pvcn_1331__cnngen_demangled_name 	"__pvcn_1331_"
#define flexidag_openseg_prim_split_5___pvcn_1331__is_constant 	1
#define flexidag_openseg_prim_split_5___pvcn_1331__has_init_data 	1
#define flexidag_openseg_prim_split_5___pvcn_1331__is_variable_scalar 	0
#define flexidag_openseg_prim_split_5___pvcn_1331__is_bitvector 	0
#define flexidag_openseg_prim_split_5___pvcn_1331__is_rlz 	0
#define flexidag_openseg_prim_split_5___pvcn_1331__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_5___pvcn_1331__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_5___pvcn_1331__byte_offset 	636	/* 0x0000027c */
#define flexidag_openseg_prim_split_5___pvcn_1331__vbase_byte_offset 	640	/* 0x00000280 */
#define flexidag_openseg_prim_split_5___pvcn_1331__dbase_byte_offset 	664	/* 0x00000298 */
#define flexidag_openseg_prim_split_5___pvcn_1331__dpitchm1_byte_offset 	668	/* 0x0000029c */
#define flexidag_openseg_prim_split_5___pvcn_1331__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_5___pvcn_1331__W 	360
#define flexidag_openseg_prim_split_5___pvcn_1331__H 	1
#define flexidag_openseg_prim_split_5___pvcn_1331__D 	1
#define flexidag_openseg_prim_split_5___pvcn_1331__P 	1
#define flexidag_openseg_prim_split_5___pvcn_1331__data_num_bytes 	1
#define flexidag_openseg_prim_split_5___pvcn_1331__denable 	1
#define flexidag_openseg_prim_split_5___pvcn_1331__dpitch_num_bytes 	384
#define flexidag_openseg_prim_split_5___pvcn_1331__dram_format 	0
#define flexidag_openseg_prim_split_5___pvcn_1331__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_5___pvcn_1331__tile_width 	1
#define flexidag_openseg_prim_split_5___pvcn_1331__tile_height 	1
#define flexidag_openseg_prim_split_5___pvcn_1331__vmem_buffer_num_bytes 	360
#define flexidag_openseg_prim_split_5___pvcn_1331__dram_size_num_bytes 	384

/* flexidag_openseg_prim_split_5 SMB_input __pvcn_1337_ */
#define flexidag_openseg_prim_split_5___pvcn_1337__cnngen_demangled_name 	"__pvcn_1337_"
#define flexidag_openseg_prim_split_5___pvcn_1337__is_constant 	1
#define flexidag_openseg_prim_split_5___pvcn_1337__has_init_data 	1
#define flexidag_openseg_prim_split_5___pvcn_1337__is_variable_scalar 	0
#define flexidag_openseg_prim_split_5___pvcn_1337__is_bitvector 	0
#define flexidag_openseg_prim_split_5___pvcn_1337__is_rlz 	0
#define flexidag_openseg_prim_split_5___pvcn_1337__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_5___pvcn_1337__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_5___pvcn_1337__byte_offset 	676	/* 0x000002a4 */
#define flexidag_openseg_prim_split_5___pvcn_1337__vbase_byte_offset 	680	/* 0x000002a8 */
#define flexidag_openseg_prim_split_5___pvcn_1337__dbase_byte_offset 	704	/* 0x000002c0 */
#define flexidag_openseg_prim_split_5___pvcn_1337__dpitchm1_byte_offset 	708	/* 0x000002c4 */
#define flexidag_openseg_prim_split_5___pvcn_1337__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_5___pvcn_1337__W 	512
#define flexidag_openseg_prim_split_5___pvcn_1337__H 	1
#define flexidag_openseg_prim_split_5___pvcn_1337__D 	1
#define flexidag_openseg_prim_split_5___pvcn_1337__P 	1
#define flexidag_openseg_prim_split_5___pvcn_1337__data_num_bytes 	1
#define flexidag_openseg_prim_split_5___pvcn_1337__denable 	1
#define flexidag_openseg_prim_split_5___pvcn_1337__dpitch_num_bytes 	512
#define flexidag_openseg_prim_split_5___pvcn_1337__dram_format 	0
#define flexidag_openseg_prim_split_5___pvcn_1337__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_5___pvcn_1337__tile_width 	1
#define flexidag_openseg_prim_split_5___pvcn_1337__tile_height 	1
#define flexidag_openseg_prim_split_5___pvcn_1337__vmem_buffer_num_bytes 	512
#define flexidag_openseg_prim_split_5___pvcn_1337__dram_size_num_bytes 	512

/* flexidag_openseg_prim_split_5 SMB_input __pvcn_1338_ */
#define flexidag_openseg_prim_split_5___pvcn_1338__cnngen_demangled_name 	"__pvcn_1338_"
#define flexidag_openseg_prim_split_5___pvcn_1338__is_constant 	1
#define flexidag_openseg_prim_split_5___pvcn_1338__has_init_data 	1
#define flexidag_openseg_prim_split_5___pvcn_1338__is_variable_scalar 	0
#define flexidag_openseg_prim_split_5___pvcn_1338__is_bitvector 	0
#define flexidag_openseg_prim_split_5___pvcn_1338__is_rlz 	0
#define flexidag_openseg_prim_split_5___pvcn_1338__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_5___pvcn_1338__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_5___pvcn_1338__byte_offset 	716	/* 0x000002cc */
#define flexidag_openseg_prim_split_5___pvcn_1338__vbase_byte_offset 	720	/* 0x000002d0 */
#define flexidag_openseg_prim_split_5___pvcn_1338__dbase_byte_offset 	744	/* 0x000002e8 */
#define flexidag_openseg_prim_split_5___pvcn_1338__dpitchm1_byte_offset 	748	/* 0x000002ec */
#define flexidag_openseg_prim_split_5___pvcn_1338__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_5___pvcn_1338__W 	360
#define flexidag_openseg_prim_split_5___pvcn_1338__H 	1
#define flexidag_openseg_prim_split_5___pvcn_1338__D 	1
#define flexidag_openseg_prim_split_5___pvcn_1338__P 	1
#define flexidag_openseg_prim_split_5___pvcn_1338__data_num_bytes 	1
#define flexidag_openseg_prim_split_5___pvcn_1338__denable 	1
#define flexidag_openseg_prim_split_5___pvcn_1338__dpitch_num_bytes 	384
#define flexidag_openseg_prim_split_5___pvcn_1338__dram_format 	0
#define flexidag_openseg_prim_split_5___pvcn_1338__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_5___pvcn_1338__tile_width 	1
#define flexidag_openseg_prim_split_5___pvcn_1338__tile_height 	1
#define flexidag_openseg_prim_split_5___pvcn_1338__vmem_buffer_num_bytes 	360
#define flexidag_openseg_prim_split_5___pvcn_1338__dram_size_num_bytes 	384

/* flexidag_openseg_prim_split_5 SMB_input __pvcn_1344_ */
#define flexidag_openseg_prim_split_5___pvcn_1344__cnngen_demangled_name 	"__pvcn_1344_"
#define flexidag_openseg_prim_split_5___pvcn_1344__is_constant 	1
#define flexidag_openseg_prim_split_5___pvcn_1344__has_init_data 	1
#define flexidag_openseg_prim_split_5___pvcn_1344__is_variable_scalar 	0
#define flexidag_openseg_prim_split_5___pvcn_1344__is_bitvector 	0
#define flexidag_openseg_prim_split_5___pvcn_1344__is_rlz 	0
#define flexidag_openseg_prim_split_5___pvcn_1344__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_5___pvcn_1344__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_5___pvcn_1344__byte_offset 	756	/* 0x000002f4 */
#define flexidag_openseg_prim_split_5___pvcn_1344__vbase_byte_offset 	760	/* 0x000002f8 */
#define flexidag_openseg_prim_split_5___pvcn_1344__dbase_byte_offset 	784	/* 0x00000310 */
#define flexidag_openseg_prim_split_5___pvcn_1344__dpitchm1_byte_offset 	788	/* 0x00000314 */
#define flexidag_openseg_prim_split_5___pvcn_1344__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_5___pvcn_1344__W 	512
#define flexidag_openseg_prim_split_5___pvcn_1344__H 	1
#define flexidag_openseg_prim_split_5___pvcn_1344__D 	1
#define flexidag_openseg_prim_split_5___pvcn_1344__P 	1
#define flexidag_openseg_prim_split_5___pvcn_1344__data_num_bytes 	1
#define flexidag_openseg_prim_split_5___pvcn_1344__denable 	1
#define flexidag_openseg_prim_split_5___pvcn_1344__dpitch_num_bytes 	512
#define flexidag_openseg_prim_split_5___pvcn_1344__dram_format 	0
#define flexidag_openseg_prim_split_5___pvcn_1344__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_5___pvcn_1344__tile_width 	1
#define flexidag_openseg_prim_split_5___pvcn_1344__tile_height 	1
#define flexidag_openseg_prim_split_5___pvcn_1344__vmem_buffer_num_bytes 	512
#define flexidag_openseg_prim_split_5___pvcn_1344__dram_size_num_bytes 	512

/* flexidag_openseg_prim_split_5 SMB_input __pvcn_1345_ */
#define flexidag_openseg_prim_split_5___pvcn_1345__cnngen_demangled_name 	"__pvcn_1345_"
#define flexidag_openseg_prim_split_5___pvcn_1345__is_constant 	1
#define flexidag_openseg_prim_split_5___pvcn_1345__has_init_data 	1
#define flexidag_openseg_prim_split_5___pvcn_1345__is_variable_scalar 	0
#define flexidag_openseg_prim_split_5___pvcn_1345__is_bitvector 	0
#define flexidag_openseg_prim_split_5___pvcn_1345__is_rlz 	0
#define flexidag_openseg_prim_split_5___pvcn_1345__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_5___pvcn_1345__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_5___pvcn_1345__byte_offset 	796	/* 0x0000031c */
#define flexidag_openseg_prim_split_5___pvcn_1345__vbase_byte_offset 	800	/* 0x00000320 */
#define flexidag_openseg_prim_split_5___pvcn_1345__dbase_byte_offset 	824	/* 0x00000338 */
#define flexidag_openseg_prim_split_5___pvcn_1345__dpitchm1_byte_offset 	828	/* 0x0000033c */
#define flexidag_openseg_prim_split_5___pvcn_1345__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_5___pvcn_1345__W 	360
#define flexidag_openseg_prim_split_5___pvcn_1345__H 	1
#define flexidag_openseg_prim_split_5___pvcn_1345__D 	1
#define flexidag_openseg_prim_split_5___pvcn_1345__P 	1
#define flexidag_openseg_prim_split_5___pvcn_1345__data_num_bytes 	1
#define flexidag_openseg_prim_split_5___pvcn_1345__denable 	1
#define flexidag_openseg_prim_split_5___pvcn_1345__dpitch_num_bytes 	384
#define flexidag_openseg_prim_split_5___pvcn_1345__dram_format 	0
#define flexidag_openseg_prim_split_5___pvcn_1345__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_5___pvcn_1345__tile_width 	1
#define flexidag_openseg_prim_split_5___pvcn_1345__tile_height 	1
#define flexidag_openseg_prim_split_5___pvcn_1345__vmem_buffer_num_bytes 	360
#define flexidag_openseg_prim_split_5___pvcn_1345__dram_size_num_bytes 	384

/* flexidag_openseg_prim_split_5 SMB_input __pvcn_150012_ */
#define flexidag_openseg_prim_split_5___pvcn_150012__cnngen_demangled_name 	"__pvcn_150012_"
#define flexidag_openseg_prim_split_5___pvcn_150012__is_constant 	1
#define flexidag_openseg_prim_split_5___pvcn_150012__has_init_data 	1
#define flexidag_openseg_prim_split_5___pvcn_150012__is_variable_scalar 	0
#define flexidag_openseg_prim_split_5___pvcn_150012__is_bitvector 	0
#define flexidag_openseg_prim_split_5___pvcn_150012__is_rlz 	0
#define flexidag_openseg_prim_split_5___pvcn_150012__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_5___pvcn_150012__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_5___pvcn_150012__byte_offset 	848	/* 0x00000350 */
#define flexidag_openseg_prim_split_5___pvcn_150012__vbase_byte_offset 	852	/* 0x00000354 */
#define flexidag_openseg_prim_split_5___pvcn_150012__dbase_byte_offset 	876	/* 0x0000036c */
#define flexidag_openseg_prim_split_5___pvcn_150012__dpitchm1_byte_offset 	880	/* 0x00000370 */
#define flexidag_openseg_prim_split_5___pvcn_150012__dpitchm1_bsize 	32	/* 0x00000020 */
#define flexidag_openseg_prim_split_5___pvcn_150012__W 	76044
#define flexidag_openseg_prim_split_5___pvcn_150012__H 	1
#define flexidag_openseg_prim_split_5___pvcn_150012__D 	1
#define flexidag_openseg_prim_split_5___pvcn_150012__P 	1
#define flexidag_openseg_prim_split_5___pvcn_150012__data_num_bytes 	1
#define flexidag_openseg_prim_split_5___pvcn_150012__denable 	1
#define flexidag_openseg_prim_split_5___pvcn_150012__dpitch_num_bytes 	76064
#define flexidag_openseg_prim_split_5___pvcn_150012__dram_format 	0
#define flexidag_openseg_prim_split_5___pvcn_150012__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_5___pvcn_150012__tile_width 	1
#define flexidag_openseg_prim_split_5___pvcn_150012__tile_height 	1
#define flexidag_openseg_prim_split_5___pvcn_150012__vmem_buffer_num_bytes 	76044
#define flexidag_openseg_prim_split_5___pvcn_150012__dram_size_num_bytes 	76064

/* flexidag_openseg_prim_split_5 SMB_input __pvcn_150015_ */
#define flexidag_openseg_prim_split_5___pvcn_150015__cnngen_demangled_name 	"__pvcn_150015_"
#define flexidag_openseg_prim_split_5___pvcn_150015__is_constant 	1
#define flexidag_openseg_prim_split_5___pvcn_150015__has_init_data 	1
#define flexidag_openseg_prim_split_5___pvcn_150015__is_variable_scalar 	0
#define flexidag_openseg_prim_split_5___pvcn_150015__is_bitvector 	0
#define flexidag_openseg_prim_split_5___pvcn_150015__is_rlz 	0
#define flexidag_openseg_prim_split_5___pvcn_150015__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_5___pvcn_150015__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_5___pvcn_150015__byte_offset 	888	/* 0x00000378 */
#define flexidag_openseg_prim_split_5___pvcn_150015__vbase_byte_offset 	892	/* 0x0000037c */
#define flexidag_openseg_prim_split_5___pvcn_150015__dbase_byte_offset 	916	/* 0x00000394 */
#define flexidag_openseg_prim_split_5___pvcn_150015__dpitchm1_byte_offset 	920	/* 0x00000398 */
#define flexidag_openseg_prim_split_5___pvcn_150015__dpitchm1_bsize 	32	/* 0x00000020 */
#define flexidag_openseg_prim_split_5___pvcn_150015__W 	76612
#define flexidag_openseg_prim_split_5___pvcn_150015__H 	1
#define flexidag_openseg_prim_split_5___pvcn_150015__D 	1
#define flexidag_openseg_prim_split_5___pvcn_150015__P 	1
#define flexidag_openseg_prim_split_5___pvcn_150015__data_num_bytes 	1
#define flexidag_openseg_prim_split_5___pvcn_150015__denable 	1
#define flexidag_openseg_prim_split_5___pvcn_150015__dpitch_num_bytes 	76640
#define flexidag_openseg_prim_split_5___pvcn_150015__dram_format 	0
#define flexidag_openseg_prim_split_5___pvcn_150015__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_5___pvcn_150015__tile_width 	1
#define flexidag_openseg_prim_split_5___pvcn_150015__tile_height 	1
#define flexidag_openseg_prim_split_5___pvcn_150015__vmem_buffer_num_bytes 	76612
#define flexidag_openseg_prim_split_5___pvcn_150015__dram_size_num_bytes 	76640

/* flexidag_openseg_prim_split_5 SMB_input __pvcn_150013_ */
#define flexidag_openseg_prim_split_5___pvcn_150013__cnngen_demangled_name 	"__pvcn_150013_"
#define flexidag_openseg_prim_split_5___pvcn_150013__is_constant 	1
#define flexidag_openseg_prim_split_5___pvcn_150013__has_init_data 	1
#define flexidag_openseg_prim_split_5___pvcn_150013__is_variable_scalar 	0
#define flexidag_openseg_prim_split_5___pvcn_150013__is_bitvector 	0
#define flexidag_openseg_prim_split_5___pvcn_150013__is_rlz 	0
#define flexidag_openseg_prim_split_5___pvcn_150013__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_5___pvcn_150013__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_5___pvcn_150013__byte_offset 	928	/* 0x000003a0 */
#define flexidag_openseg_prim_split_5___pvcn_150013__vbase_byte_offset 	932	/* 0x000003a4 */
#define flexidag_openseg_prim_split_5___pvcn_150013__dbase_byte_offset 	956	/* 0x000003bc */
#define flexidag_openseg_prim_split_5___pvcn_150013__dpitchm1_byte_offset 	960	/* 0x000003c0 */
#define flexidag_openseg_prim_split_5___pvcn_150013__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_5___pvcn_150013__W 	1440
#define flexidag_openseg_prim_split_5___pvcn_150013__H 	1
#define flexidag_openseg_prim_split_5___pvcn_150013__D 	1
#define flexidag_openseg_prim_split_5___pvcn_150013__P 	1
#define flexidag_openseg_prim_split_5___pvcn_150013__data_num_bytes 	1
#define flexidag_openseg_prim_split_5___pvcn_150013__denable 	1
#define flexidag_openseg_prim_split_5___pvcn_150013__dpitch_num_bytes 	1440
#define flexidag_openseg_prim_split_5___pvcn_150013__dram_format 	0
#define flexidag_openseg_prim_split_5___pvcn_150013__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_5___pvcn_150013__tile_width 	1
#define flexidag_openseg_prim_split_5___pvcn_150013__tile_height 	1
#define flexidag_openseg_prim_split_5___pvcn_150013__vmem_buffer_num_bytes 	1440
#define flexidag_openseg_prim_split_5___pvcn_150013__dram_size_num_bytes 	1440

/* flexidag_openseg_prim_split_5 SMB_input __pvcn_150016_ */
#define flexidag_openseg_prim_split_5___pvcn_150016__cnngen_demangled_name 	"__pvcn_150016_"
#define flexidag_openseg_prim_split_5___pvcn_150016__is_constant 	1
#define flexidag_openseg_prim_split_5___pvcn_150016__has_init_data 	1
#define flexidag_openseg_prim_split_5___pvcn_150016__is_variable_scalar 	0
#define flexidag_openseg_prim_split_5___pvcn_150016__is_bitvector 	0
#define flexidag_openseg_prim_split_5___pvcn_150016__is_rlz 	0
#define flexidag_openseg_prim_split_5___pvcn_150016__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_5___pvcn_150016__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_5___pvcn_150016__byte_offset 	968	/* 0x000003c8 */
#define flexidag_openseg_prim_split_5___pvcn_150016__vbase_byte_offset 	972	/* 0x000003cc */
#define flexidag_openseg_prim_split_5___pvcn_150016__dbase_byte_offset 	996	/* 0x000003e4 */
#define flexidag_openseg_prim_split_5___pvcn_150016__dpitchm1_byte_offset 	1000	/* 0x000003e8 */
#define flexidag_openseg_prim_split_5___pvcn_150016__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_5___pvcn_150016__W 	1440
#define flexidag_openseg_prim_split_5___pvcn_150016__H 	1
#define flexidag_openseg_prim_split_5___pvcn_150016__D 	1
#define flexidag_openseg_prim_split_5___pvcn_150016__P 	1
#define flexidag_openseg_prim_split_5___pvcn_150016__data_num_bytes 	1
#define flexidag_openseg_prim_split_5___pvcn_150016__denable 	1
#define flexidag_openseg_prim_split_5___pvcn_150016__dpitch_num_bytes 	1440
#define flexidag_openseg_prim_split_5___pvcn_150016__dram_format 	0
#define flexidag_openseg_prim_split_5___pvcn_150016__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_5___pvcn_150016__tile_width 	1
#define flexidag_openseg_prim_split_5___pvcn_150016__tile_height 	1
#define flexidag_openseg_prim_split_5___pvcn_150016__vmem_buffer_num_bytes 	1440
#define flexidag_openseg_prim_split_5___pvcn_150016__dram_size_num_bytes 	1440

/* flexidag_openseg_prim_split_5 SMB_input __pvcn_1365_ */
#define flexidag_openseg_prim_split_5___pvcn_1365__cnngen_demangled_name 	"__pvcn_1365_"
#define flexidag_openseg_prim_split_5___pvcn_1365__is_constant 	1
#define flexidag_openseg_prim_split_5___pvcn_1365__has_init_data 	1
#define flexidag_openseg_prim_split_5___pvcn_1365__is_variable_scalar 	0
#define flexidag_openseg_prim_split_5___pvcn_1365__is_bitvector 	0
#define flexidag_openseg_prim_split_5___pvcn_1365__is_rlz 	0
#define flexidag_openseg_prim_split_5___pvcn_1365__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_5___pvcn_1365__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_5___pvcn_1365__byte_offset 	1008	/* 0x000003f0 */
#define flexidag_openseg_prim_split_5___pvcn_1365__vbase_byte_offset 	1012	/* 0x000003f4 */
#define flexidag_openseg_prim_split_5___pvcn_1365__dbase_byte_offset 	1036	/* 0x0000040c */
#define flexidag_openseg_prim_split_5___pvcn_1365__dpitchm1_byte_offset 	1040	/* 0x00000410 */
#define flexidag_openseg_prim_split_5___pvcn_1365__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_5___pvcn_1365__W 	4
#define flexidag_openseg_prim_split_5___pvcn_1365__H 	1
#define flexidag_openseg_prim_split_5___pvcn_1365__D 	1
#define flexidag_openseg_prim_split_5___pvcn_1365__P 	1
#define flexidag_openseg_prim_split_5___pvcn_1365__data_num_bytes 	2
#define flexidag_openseg_prim_split_5___pvcn_1365__denable 	1
#define flexidag_openseg_prim_split_5___pvcn_1365__dpitch_num_bytes 	32
#define flexidag_openseg_prim_split_5___pvcn_1365__dram_format 	0
#define flexidag_openseg_prim_split_5___pvcn_1365__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_5___pvcn_1365__tile_width 	1
#define flexidag_openseg_prim_split_5___pvcn_1365__tile_height 	1
#define flexidag_openseg_prim_split_5___pvcn_1365__vmem_buffer_num_bytes 	8
#define flexidag_openseg_prim_split_5___pvcn_1365__dram_size_num_bytes 	32

/* flexidag_openseg_prim_split_5 SMB_input __pvcn_1366_ */
#define flexidag_openseg_prim_split_5___pvcn_1366__cnngen_demangled_name 	"__pvcn_1366_"
#define flexidag_openseg_prim_split_5___pvcn_1366__is_constant 	1
#define flexidag_openseg_prim_split_5___pvcn_1366__has_init_data 	1
#define flexidag_openseg_prim_split_5___pvcn_1366__is_variable_scalar 	0
#define flexidag_openseg_prim_split_5___pvcn_1366__is_bitvector 	0
#define flexidag_openseg_prim_split_5___pvcn_1366__is_rlz 	0
#define flexidag_openseg_prim_split_5___pvcn_1366__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_5___pvcn_1366__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_5___pvcn_1366__byte_offset 	1048	/* 0x00000418 */
#define flexidag_openseg_prim_split_5___pvcn_1366__vbase_byte_offset 	1052	/* 0x0000041c */
#define flexidag_openseg_prim_split_5___pvcn_1366__dbase_byte_offset 	1076	/* 0x00000434 */
#define flexidag_openseg_prim_split_5___pvcn_1366__dpitchm1_byte_offset 	1080	/* 0x00000438 */
#define flexidag_openseg_prim_split_5___pvcn_1366__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_5___pvcn_1366__W 	5
#define flexidag_openseg_prim_split_5___pvcn_1366__H 	1
#define flexidag_openseg_prim_split_5___pvcn_1366__D 	1
#define flexidag_openseg_prim_split_5___pvcn_1366__P 	1
#define flexidag_openseg_prim_split_5___pvcn_1366__data_num_bytes 	2
#define flexidag_openseg_prim_split_5___pvcn_1366__denable 	1
#define flexidag_openseg_prim_split_5___pvcn_1366__dpitch_num_bytes 	32
#define flexidag_openseg_prim_split_5___pvcn_1366__dram_format 	0
#define flexidag_openseg_prim_split_5___pvcn_1366__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_5___pvcn_1366__tile_width 	1
#define flexidag_openseg_prim_split_5___pvcn_1366__tile_height 	1
#define flexidag_openseg_prim_split_5___pvcn_1366__vmem_buffer_num_bytes 	12
#define flexidag_openseg_prim_split_5___pvcn_1366__dram_size_num_bytes 	32

/* flexidag_openseg_prim_split_5 VCB PeleeNet__stage_3_transition_layer__transition_layer_conv0__Conv2D___BatchNorm__FusedBatchNorm___Relu */
#define flexidag_openseg_prim_split_5_PeleeNet__stage_3_transition_layer__transition_layer_conv0__Conv2D___BatchNorm__FusedBatchNorm___Relu_cnngen_demangled_name 	"PeleeNet__stage_3_transition_layer__transition_layer_conv0__Conv2D___BatchNorm__FusedBatchNorm___Relu"
#define flexidag_openseg_prim_split_5_PeleeNet__stage_3_transition_layer__transition_layer_conv0__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_bitvector 	0
#define flexidag_openseg_prim_split_5_PeleeNet__stage_3_transition_layer__transition_layer_conv0__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_rlz 	0
#define flexidag_openseg_prim_split_5_PeleeNet__stage_3_transition_layer__transition_layer_conv0__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_5_PeleeNet__stage_3_transition_layer__transition_layer_conv0__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_5_PeleeNet__stage_3_transition_layer__transition_layer_conv0__Conv2D___BatchNorm__FusedBatchNorm___Relu_byte_offset 	1148	/* 0x0000047c */
#define flexidag_openseg_prim_split_5_PeleeNet__stage_3_transition_layer__transition_layer_conv0__Conv2D___BatchNorm__FusedBatchNorm___Relu_vbase_byte_offset 	1156	/* 0x00000484 */
#define flexidag_openseg_prim_split_5_PeleeNet__stage_3_transition_layer__transition_layer_conv0__Conv2D___BatchNorm__FusedBatchNorm___Relu_vmem_buffer_num_bytes 	516096

/* flexidag_openseg_prim_split_5 VCB PeleeNet__stage_3_transition_layer__transition_layer_avgpool__AvgPool */
#define flexidag_openseg_prim_split_5_PeleeNet__stage_3_transition_layer__transition_layer_avgpool__AvgPool_cnngen_demangled_name 	"PeleeNet__stage_3_transition_layer__transition_layer_avgpool__AvgPool"
#define flexidag_openseg_prim_split_5_PeleeNet__stage_3_transition_layer__transition_layer_avgpool__AvgPool_is_bitvector 	0
#define flexidag_openseg_prim_split_5_PeleeNet__stage_3_transition_layer__transition_layer_avgpool__AvgPool_is_rlz 	0
#define flexidag_openseg_prim_split_5_PeleeNet__stage_3_transition_layer__transition_layer_avgpool__AvgPool_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_5_PeleeNet__stage_3_transition_layer__transition_layer_avgpool__AvgPool_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_5_PeleeNet__stage_3_transition_layer__transition_layer_avgpool__AvgPool_byte_offset 	1240	/* 0x000004d8 */
#define flexidag_openseg_prim_split_5_PeleeNet__stage_3_transition_layer__transition_layer_avgpool__AvgPool_vbase_byte_offset 	1244	/* 0x000004dc */
#define flexidag_openseg_prim_split_5_PeleeNet__stage_3_transition_layer__transition_layer_avgpool__AvgPool_vmem_buffer_num_bytes 	397312

/* flexidag_openseg_prim_split_5 VCB __pvcn_682_ */
#define flexidag_openseg_prim_split_5___pvcn_682__cnngen_demangled_name 	"__pvcn_682_"
#define flexidag_openseg_prim_split_5___pvcn_682__is_bitvector 	0
#define flexidag_openseg_prim_split_5___pvcn_682__is_rlz 	0
#define flexidag_openseg_prim_split_5___pvcn_682__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_5___pvcn_682__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_5___pvcn_682__byte_offset 	1612	/* 0x0000064c */
#define flexidag_openseg_prim_split_5___pvcn_682__vbase_byte_offset 	1616	/* 0x00000650 */
#define flexidag_openseg_prim_split_5___pvcn_682__vmem_buffer_num_bytes 	512

/* flexidag_openseg_prim_split_5 VCB Conv_4__Conv2D___dotib___386 */
#define flexidag_openseg_prim_split_5_Conv_4__Conv2D___dotib___386_cnngen_demangled_name 	"Conv_4__Conv2D___dotib___386"
#define flexidag_openseg_prim_split_5_Conv_4__Conv2D___dotib___386_is_bitvector 	0
#define flexidag_openseg_prim_split_5_Conv_4__Conv2D___dotib___386_is_rlz 	0
#define flexidag_openseg_prim_split_5_Conv_4__Conv2D___dotib___386_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_5_Conv_4__Conv2D___dotib___386_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_5_Conv_4__Conv2D___dotib___386_byte_offset 	1640	/* 0x00000668 */
#define flexidag_openseg_prim_split_5_Conv_4__Conv2D___dotib___386_vbase_byte_offset 	1644	/* 0x0000066c */
#define flexidag_openseg_prim_split_5_Conv_4__Conv2D___dotib___386_vmem_buffer_num_bytes 	516

/* flexidag_openseg_prim_split_5 VCB __pvcn_1375_ */
#define flexidag_openseg_prim_split_5___pvcn_1375__cnngen_demangled_name 	"__pvcn_1375_"
#define flexidag_openseg_prim_split_5___pvcn_1375__is_bitvector 	0
#define flexidag_openseg_prim_split_5___pvcn_1375__is_rlz 	0
#define flexidag_openseg_prim_split_5___pvcn_1375__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_5___pvcn_1375__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_5___pvcn_1375__byte_offset 	1916	/* 0x0000077c */
#define flexidag_openseg_prim_split_5___pvcn_1375__vbase_byte_offset 	1924	/* 0x00000784 */
#define flexidag_openseg_prim_split_5___pvcn_1375__vmem_buffer_num_bytes 	2176

/* flexidag_openseg_prim_split_5 VCB Sigmoid_1___mul___143 */
#define flexidag_openseg_prim_split_5_Sigmoid_1___mul___143_cnngen_demangled_name 	"Sigmoid_1___mul___143"
#define flexidag_openseg_prim_split_5_Sigmoid_1___mul___143_is_bitvector 	0
#define flexidag_openseg_prim_split_5_Sigmoid_1___mul___143_is_rlz 	0
#define flexidag_openseg_prim_split_5_Sigmoid_1___mul___143_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_5_Sigmoid_1___mul___143_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_5_Sigmoid_1___mul___143_byte_offset 	2072	/* 0x00000818 */
#define flexidag_openseg_prim_split_5_Sigmoid_1___mul___143_vbase_byte_offset 	2080	/* 0x00000820 */
#define flexidag_openseg_prim_split_5_Sigmoid_1___mul___143_vmem_buffer_num_bytes 	1084

/* flexidag_openseg_prim_split_5 VCB Sigmoid_1 */
#define flexidag_openseg_prim_split_5_Sigmoid_1_cnngen_demangled_name 	"Sigmoid_1"
#define flexidag_openseg_prim_split_5_Sigmoid_1_is_bitvector 	0
#define flexidag_openseg_prim_split_5_Sigmoid_1_is_rlz 	0
#define flexidag_openseg_prim_split_5_Sigmoid_1_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_5_Sigmoid_1_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_5_Sigmoid_1_byte_offset 	2156	/* 0x0000086c */
#define flexidag_openseg_prim_split_5_Sigmoid_1_vbase_byte_offset 	2164	/* 0x00000874 */
#define flexidag_openseg_prim_split_5_Sigmoid_1_vmem_buffer_num_bytes 	2176

/* flexidag_openseg_prim_split_5 VCB Mul_1 */
#define flexidag_openseg_prim_split_5_Mul_1_cnngen_demangled_name 	"Mul_1"
#define flexidag_openseg_prim_split_5_Mul_1_is_bitvector 	0
#define flexidag_openseg_prim_split_5_Mul_1_is_rlz 	0
#define flexidag_openseg_prim_split_5_Mul_1_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_5_Mul_1_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_5_Mul_1_byte_offset 	2200	/* 0x00000898 */
#define flexidag_openseg_prim_split_5_Mul_1_vbase_byte_offset 	2204	/* 0x0000089c */
#define flexidag_openseg_prim_split_5_Mul_1_vmem_buffer_num_bytes 	331776

/* flexidag_openseg_prim_split_5 VCB Mean_2_nhwc */
#define flexidag_openseg_prim_split_5_Mean_2_nhwc_cnngen_demangled_name 	"Mean_2_nhwc"
#define flexidag_openseg_prim_split_5_Mean_2_nhwc_is_bitvector 	0
#define flexidag_openseg_prim_split_5_Mean_2_nhwc_is_rlz 	0
#define flexidag_openseg_prim_split_5_Mean_2_nhwc_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_5_Mean_2_nhwc_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_5_Mean_2_nhwc_byte_offset 	2252	/* 0x000008cc */
#define flexidag_openseg_prim_split_5_Mean_2_nhwc_vbase_byte_offset 	2256	/* 0x000008d0 */
#define flexidag_openseg_prim_split_5_Mean_2_nhwc_vmem_buffer_num_bytes 	512

/* flexidag_openseg_prim_split_5 HMB_output Mul_2 */
#define flexidag_openseg_prim_split_5_Mul_2_cnngen_demangled_name 	"Mul_2"
#define flexidag_openseg_prim_split_5_Mul_2_is_main_input_output 	0
#define flexidag_openseg_prim_split_5_Mul_2_is_bitvector 	0
#define flexidag_openseg_prim_split_5_Mul_2_is_rlz 	0
#define flexidag_openseg_prim_split_5_Mul_2_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_5_Mul_2_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_5_Mul_2_byte_offset 	2288	/* 0x000008f0 */
#define flexidag_openseg_prim_split_5_Mul_2_vbase_byte_offset 	2292	/* 0x000008f4 */
#define flexidag_openseg_prim_split_5_Mul_2_dbase_byte_offset 	2316	/* 0x0000090c */
#define flexidag_openseg_prim_split_5_Mul_2_dpitchm1_byte_offset 	2320	/* 0x00000910 */
#define flexidag_openseg_prim_split_5_Mul_2_dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_5_Mul_2_vwidth_minus_one_byte_offset 	2304	/* 0x00000900 */
#define flexidag_openseg_prim_split_5_Mul_2_vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_5_Mul_2_vheight_minus_one_byte_offset 	2306	/* 0x00000902 */
#define flexidag_openseg_prim_split_5_Mul_2_vheight_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_5_Mul_2_drotate_bit_offset 	18502	/* 0x00004846 */
#define flexidag_openseg_prim_split_5_Mul_2_hflip_bit_offset 	18503	/* 0x00004847 */
#define flexidag_openseg_prim_split_5_Mul_2_vflip_bit_offset 	18504	/* 0x00004848 */
#define flexidag_openseg_prim_split_5_Mul_2_dflip_bit_offset 	18505	/* 0x00004849 */
#define flexidag_openseg_prim_split_5_Mul_2_pflip_bit_offset 	18506	/* 0x0000484a */
#define flexidag_openseg_prim_split_5_Mul_2_W   	40
#define flexidag_openseg_prim_split_5_Mul_2_H   	16
#define flexidag_openseg_prim_split_5_Mul_2_D   	512
#define flexidag_openseg_prim_split_5_Mul_2_P   	1
#define flexidag_openseg_prim_split_5_Mul_2_data_num_bytes 	2
#define flexidag_openseg_prim_split_5_Mul_2_denable 	1
#define flexidag_openseg_prim_split_5_Mul_2_dpitch_num_bytes 	32
#define flexidag_openseg_prim_split_5_Mul_2_dram_format 	3
#define flexidag_openseg_prim_split_5_Mul_2_vp_interleave_mode 	2
#define flexidag_openseg_prim_split_5_Mul_2_tile_width 	4
#define flexidag_openseg_prim_split_5_Mul_2_tile_height 	1
#define flexidag_openseg_prim_split_5_Mul_2_vmem_buffer_num_bytes 	40960
#define flexidag_openseg_prim_split_5_Mul_2_dram_size_num_bytes 	655360
#define flexidag_openseg_prim_split_5_Mul_2_sign 	0
#define flexidag_openseg_prim_split_5_Mul_2_datasize 	1
#define flexidag_openseg_prim_split_5_Mul_2_expoffset 	13
#define flexidag_openseg_prim_split_5_Mul_2_expbits 	0
#define flexidag_openseg_prim_split_5_Mul_2_drotate 	0
#define flexidag_openseg_prim_split_5_Mul_2_hflip 	0
#define flexidag_openseg_prim_split_5_Mul_2_vflip 	0
#define flexidag_openseg_prim_split_5_Mul_2_dflip 	0
#define flexidag_openseg_prim_split_5_Mul_2_pflip 	0

/* flexidag_openseg_prim_split_5 SMB_output split_5__vas_created_terminate__v0 */
#define flexidag_openseg_prim_split_5_split_5__vas_created_terminate__v0_cnngen_demangled_name 	"split_5__vas_created_terminate__v0"
#define flexidag_openseg_prim_split_5_split_5__vas_created_terminate__v0_is_main_input_output 	0
#define flexidag_openseg_prim_split_5_split_5__vas_created_terminate__v0_is_bitvector 	1
#define flexidag_openseg_prim_split_5_split_5__vas_created_terminate__v0_is_rlz 	0
#define flexidag_openseg_prim_split_5_split_5__vas_created_terminate__v0_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_5_split_5__vas_created_terminate__v0_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_5_split_5__vas_created_terminate__v0_byte_offset 	2328	/* 0x00000918 */
#define flexidag_openseg_prim_split_5_split_5__vas_created_terminate__v0_vbase_byte_offset 	2332	/* 0x0000091c */
#define flexidag_openseg_prim_split_5_split_5__vas_created_terminate__v0_W 	256
#define flexidag_openseg_prim_split_5_split_5__vas_created_terminate__v0_H 	3
#define flexidag_openseg_prim_split_5_split_5__vas_created_terminate__v0_D 	1
#define flexidag_openseg_prim_split_5_split_5__vas_created_terminate__v0_P 	1
#define flexidag_openseg_prim_split_5_split_5__vas_created_terminate__v0_data_num_bytes 	1
#define flexidag_openseg_prim_split_5_split_5__vas_created_terminate__v0_denable 	0
#define flexidag_openseg_prim_split_5_split_5__vas_created_terminate__v0_dram_format 	0
#define flexidag_openseg_prim_split_5_split_5__vas_created_terminate__v0_vp_interleave_mode 	1
#define flexidag_openseg_prim_split_5_split_5__vas_created_terminate__v0_tile_width 	1
#define flexidag_openseg_prim_split_5_split_5__vas_created_terminate__v0_tile_height 	1
#define flexidag_openseg_prim_split_5_split_5__vas_created_terminate__v0_vmem_buffer_num_bytes 	96


#endif /* flexidag_openseg_prim_split_5_H */
