////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019 Ambarella, Inc.
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
#define flexidag_openseg_prim_split_2_byte_offset 	356	/* 0x00000164 */
#define flexidag_openseg_prim_split_2_byte_size 	2420	/* 0x00000974 */
#define flexidag_openseg_prim_split_2_preferred_dram_xfer_size 	1

/* flexidag_openseg_prim_split_2 VMEM info */
#define flexidag_openseg_prim_split_2_use_ping_pong_vmem 	0
#define flexidag_openseg_prim_split_2_VMEM_start 	4096	/* 0x00001000 */
#define flexidag_openseg_prim_split_2_VMEM_end  	2075512	/* 0x001fab78 */
#define flexidag_openseg_prim_split_2_image_start 	1920128	/* 0x001d4c80 */
#define flexidag_openseg_prim_split_2_image_size 	157804	/* 0x0002686c */
#define flexidag_openseg_prim_split_2_dagbin_start 	2075512	/* 0x001fab78 */

/* flexidag_openseg_prim_split_2 DAG info */
#define flexidag_openseg_prim_split_2_estimated_cycles 	624492

/*******************************************************************************
 * DAG byte offset info for HMB/SMB/VCB and their fields
 * in DAG 'flexidag_openseg_prim_split_2' in source file 'flexidag_openseg_split_2.vas'
 ******************************************************************************/
/* flexidag_openseg_prim_split_2 HMB_input PeleeNet__stage_3_dense_block_0__concat */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__concat_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_0__concat"
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__concat_is_main_input_output 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__concat_is_constant 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__concat_ddi_byte_size 	0	/* 0x00000000 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__concat_ddi_byte_offset 	4294967295	/* 0xffffffff */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__concat_is_bitvector 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__concat_is_rlz 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__concat_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__concat_byte_offset 	0	/* 0x00000000 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__concat_vbase_byte_offset 	4	/* 0x00000004 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__concat_dbase_byte_offset 	28	/* 0x0000001c */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__concat_dpitchm1_byte_offset 	32	/* 0x00000020 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__concat_dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__concat_vwidth_minus_one_byte_offset 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__concat_vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__concat_vheight_minus_one_byte_offset 	18	/* 0x00000012 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__concat_vheight_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__concat_drotate_bit_offset 	198	/* 0x000000c6 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__concat_hflip_bit_offset 	199	/* 0x000000c7 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__concat_vflip_bit_offset 	200	/* 0x000000c8 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__concat_dflip_bit_offset 	201	/* 0x000000c9 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__concat_pflip_bit_offset 	202	/* 0x000000ca */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__concat_W 	80
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__concat_H 	32
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__concat_D 	288
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__concat_P 	1
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__concat_data_num_bytes 	1
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__concat_dpitch_num_bytes 	32
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__concat_dram_format 	3
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__concat_vp_interleave_mode 	2
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__concat_tile_width 	4
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__concat_tile_height 	1
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__concat_vmem_buffer_num_bytes 	188928
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__concat_dram_size_num_bytes 	737280
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__concat_sign 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__concat_datasize 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__concat_expoffset 	5
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__concat_expbits 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__concat_drotate 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__concat_hflip 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__concat_vflip 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__concat_dflip 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_0__concat_pflip 	0

/* flexidag_openseg_prim_split_2 SMB_input __pvcn_824_ */
#define flexidag_openseg_prim_split_2___pvcn_824__cnngen_demangled_name 	"__pvcn_824_"
#define flexidag_openseg_prim_split_2___pvcn_824__is_constant 	1
#define flexidag_openseg_prim_split_2___pvcn_824__is_bitvector 	0
#define flexidag_openseg_prim_split_2___pvcn_824__is_rlz 	0
#define flexidag_openseg_prim_split_2___pvcn_824__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_824__byte_offset 	40	/* 0x00000028 */
#define flexidag_openseg_prim_split_2___pvcn_824__vbase_byte_offset 	44	/* 0x0000002c */
#define flexidag_openseg_prim_split_2___pvcn_824__dbase_byte_offset 	68	/* 0x00000044 */
#define flexidag_openseg_prim_split_2___pvcn_824__dpitchm1_byte_offset 	72	/* 0x00000048 */
#define flexidag_openseg_prim_split_2___pvcn_824__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2___pvcn_824__W 	23076
#define flexidag_openseg_prim_split_2___pvcn_824__H 	1
#define flexidag_openseg_prim_split_2___pvcn_824__D 	1
#define flexidag_openseg_prim_split_2___pvcn_824__P 	1
#define flexidag_openseg_prim_split_2___pvcn_824__data_num_bytes 	1
#define flexidag_openseg_prim_split_2___pvcn_824__dpitch_num_bytes 	23104
#define flexidag_openseg_prim_split_2___pvcn_824__dram_format 	0
#define flexidag_openseg_prim_split_2___pvcn_824__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_2___pvcn_824__tile_width 	1
#define flexidag_openseg_prim_split_2___pvcn_824__tile_height 	1
#define flexidag_openseg_prim_split_2___pvcn_824__vmem_buffer_num_bytes 	23076

/* flexidag_openseg_prim_split_2 SMB_input __pvcn_825_ */
#define flexidag_openseg_prim_split_2___pvcn_825__cnngen_demangled_name 	"__pvcn_825_"
#define flexidag_openseg_prim_split_2___pvcn_825__is_constant 	1
#define flexidag_openseg_prim_split_2___pvcn_825__is_bitvector 	0
#define flexidag_openseg_prim_split_2___pvcn_825__is_rlz 	0
#define flexidag_openseg_prim_split_2___pvcn_825__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_825__byte_offset 	76	/* 0x0000004c */
#define flexidag_openseg_prim_split_2___pvcn_825__vbase_byte_offset 	80	/* 0x00000050 */
#define flexidag_openseg_prim_split_2___pvcn_825__dbase_byte_offset 	104	/* 0x00000068 */
#define flexidag_openseg_prim_split_2___pvcn_825__dpitchm1_byte_offset 	108	/* 0x0000006c */
#define flexidag_openseg_prim_split_2___pvcn_825__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2___pvcn_825__W 	640
#define flexidag_openseg_prim_split_2___pvcn_825__H 	1
#define flexidag_openseg_prim_split_2___pvcn_825__D 	1
#define flexidag_openseg_prim_split_2___pvcn_825__P 	1
#define flexidag_openseg_prim_split_2___pvcn_825__data_num_bytes 	1
#define flexidag_openseg_prim_split_2___pvcn_825__dpitch_num_bytes 	640
#define flexidag_openseg_prim_split_2___pvcn_825__dram_format 	0
#define flexidag_openseg_prim_split_2___pvcn_825__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_2___pvcn_825__tile_width 	1
#define flexidag_openseg_prim_split_2___pvcn_825__tile_height 	1
#define flexidag_openseg_prim_split_2___pvcn_825__vmem_buffer_num_bytes 	640

/* flexidag_openseg_prim_split_2 SMB_input __pvcn_828_ */
#define flexidag_openseg_prim_split_2___pvcn_828__cnngen_demangled_name 	"__pvcn_828_"
#define flexidag_openseg_prim_split_2___pvcn_828__is_constant 	1
#define flexidag_openseg_prim_split_2___pvcn_828__is_bitvector 	0
#define flexidag_openseg_prim_split_2___pvcn_828__is_rlz 	0
#define flexidag_openseg_prim_split_2___pvcn_828__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_828__byte_offset 	116	/* 0x00000074 */
#define flexidag_openseg_prim_split_2___pvcn_828__vbase_byte_offset 	120	/* 0x00000078 */
#define flexidag_openseg_prim_split_2___pvcn_828__dbase_byte_offset 	144	/* 0x00000090 */
#define flexidag_openseg_prim_split_2___pvcn_828__dpitchm1_byte_offset 	148	/* 0x00000094 */
#define flexidag_openseg_prim_split_2___pvcn_828__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2___pvcn_828__W 	5752
#define flexidag_openseg_prim_split_2___pvcn_828__H 	1
#define flexidag_openseg_prim_split_2___pvcn_828__D 	1
#define flexidag_openseg_prim_split_2___pvcn_828__P 	1
#define flexidag_openseg_prim_split_2___pvcn_828__data_num_bytes 	1
#define flexidag_openseg_prim_split_2___pvcn_828__dpitch_num_bytes 	5760
#define flexidag_openseg_prim_split_2___pvcn_828__dram_format 	0
#define flexidag_openseg_prim_split_2___pvcn_828__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_2___pvcn_828__tile_width 	1
#define flexidag_openseg_prim_split_2___pvcn_828__tile_height 	1
#define flexidag_openseg_prim_split_2___pvcn_828__vmem_buffer_num_bytes 	5752

/* flexidag_openseg_prim_split_2 SMB_input __pvcn_829_ */
#define flexidag_openseg_prim_split_2___pvcn_829__cnngen_demangled_name 	"__pvcn_829_"
#define flexidag_openseg_prim_split_2___pvcn_829__is_constant 	1
#define flexidag_openseg_prim_split_2___pvcn_829__is_bitvector 	0
#define flexidag_openseg_prim_split_2___pvcn_829__is_rlz 	0
#define flexidag_openseg_prim_split_2___pvcn_829__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_829__byte_offset 	156	/* 0x0000009c */
#define flexidag_openseg_prim_split_2___pvcn_829__vbase_byte_offset 	160	/* 0x000000a0 */
#define flexidag_openseg_prim_split_2___pvcn_829__dbase_byte_offset 	184	/* 0x000000b8 */
#define flexidag_openseg_prim_split_2___pvcn_829__dpitchm1_byte_offset 	188	/* 0x000000bc */
#define flexidag_openseg_prim_split_2___pvcn_829__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2___pvcn_829__W 	80
#define flexidag_openseg_prim_split_2___pvcn_829__H 	1
#define flexidag_openseg_prim_split_2___pvcn_829__D 	1
#define flexidag_openseg_prim_split_2___pvcn_829__P 	1
#define flexidag_openseg_prim_split_2___pvcn_829__data_num_bytes 	1
#define flexidag_openseg_prim_split_2___pvcn_829__dpitch_num_bytes 	96
#define flexidag_openseg_prim_split_2___pvcn_829__dram_format 	0
#define flexidag_openseg_prim_split_2___pvcn_829__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_2___pvcn_829__tile_width 	1
#define flexidag_openseg_prim_split_2___pvcn_829__tile_height 	1
#define flexidag_openseg_prim_split_2___pvcn_829__vmem_buffer_num_bytes 	80

/* flexidag_openseg_prim_split_2 SMB_input __pvcn_832_ */
#define flexidag_openseg_prim_split_2___pvcn_832__cnngen_demangled_name 	"__pvcn_832_"
#define flexidag_openseg_prim_split_2___pvcn_832__is_constant 	1
#define flexidag_openseg_prim_split_2___pvcn_832__is_bitvector 	0
#define flexidag_openseg_prim_split_2___pvcn_832__is_rlz 	0
#define flexidag_openseg_prim_split_2___pvcn_832__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_832__byte_offset 	196	/* 0x000000c4 */
#define flexidag_openseg_prim_split_2___pvcn_832__vbase_byte_offset 	200	/* 0x000000c8 */
#define flexidag_openseg_prim_split_2___pvcn_832__dbase_byte_offset 	224	/* 0x000000e0 */
#define flexidag_openseg_prim_split_2___pvcn_832__dpitchm1_byte_offset 	228	/* 0x000000e4 */
#define flexidag_openseg_prim_split_2___pvcn_832__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2___pvcn_832__W 	5740
#define flexidag_openseg_prim_split_2___pvcn_832__H 	1
#define flexidag_openseg_prim_split_2___pvcn_832__D 	1
#define flexidag_openseg_prim_split_2___pvcn_832__P 	1
#define flexidag_openseg_prim_split_2___pvcn_832__data_num_bytes 	1
#define flexidag_openseg_prim_split_2___pvcn_832__dpitch_num_bytes 	5760
#define flexidag_openseg_prim_split_2___pvcn_832__dram_format 	0
#define flexidag_openseg_prim_split_2___pvcn_832__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_2___pvcn_832__tile_width 	1
#define flexidag_openseg_prim_split_2___pvcn_832__tile_height 	1
#define flexidag_openseg_prim_split_2___pvcn_832__vmem_buffer_num_bytes 	5740

/* flexidag_openseg_prim_split_2 SMB_input __pvcn_833_ */
#define flexidag_openseg_prim_split_2___pvcn_833__cnngen_demangled_name 	"__pvcn_833_"
#define flexidag_openseg_prim_split_2___pvcn_833__is_constant 	1
#define flexidag_openseg_prim_split_2___pvcn_833__is_bitvector 	0
#define flexidag_openseg_prim_split_2___pvcn_833__is_rlz 	0
#define flexidag_openseg_prim_split_2___pvcn_833__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_833__byte_offset 	236	/* 0x000000ec */
#define flexidag_openseg_prim_split_2___pvcn_833__vbase_byte_offset 	240	/* 0x000000f0 */
#define flexidag_openseg_prim_split_2___pvcn_833__dbase_byte_offset 	264	/* 0x00000108 */
#define flexidag_openseg_prim_split_2___pvcn_833__dpitchm1_byte_offset 	268	/* 0x0000010c */
#define flexidag_openseg_prim_split_2___pvcn_833__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2___pvcn_833__W 	88
#define flexidag_openseg_prim_split_2___pvcn_833__H 	1
#define flexidag_openseg_prim_split_2___pvcn_833__D 	1
#define flexidag_openseg_prim_split_2___pvcn_833__P 	1
#define flexidag_openseg_prim_split_2___pvcn_833__data_num_bytes 	1
#define flexidag_openseg_prim_split_2___pvcn_833__dpitch_num_bytes 	96
#define flexidag_openseg_prim_split_2___pvcn_833__dram_format 	0
#define flexidag_openseg_prim_split_2___pvcn_833__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_2___pvcn_833__tile_width 	1
#define flexidag_openseg_prim_split_2___pvcn_833__tile_height 	1
#define flexidag_openseg_prim_split_2___pvcn_833__vmem_buffer_num_bytes 	88

/* flexidag_openseg_prim_split_2 SMB_input __pvcn_835_ */
#define flexidag_openseg_prim_split_2___pvcn_835__cnngen_demangled_name 	"__pvcn_835_"
#define flexidag_openseg_prim_split_2___pvcn_835__is_constant 	1
#define flexidag_openseg_prim_split_2___pvcn_835__is_bitvector 	0
#define flexidag_openseg_prim_split_2___pvcn_835__is_rlz 	0
#define flexidag_openseg_prim_split_2___pvcn_835__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_835__byte_offset 	276	/* 0x00000114 */
#define flexidag_openseg_prim_split_2___pvcn_835__vbase_byte_offset 	280	/* 0x00000118 */
#define flexidag_openseg_prim_split_2___pvcn_835__dbase_byte_offset 	304	/* 0x00000130 */
#define flexidag_openseg_prim_split_2___pvcn_835__dpitchm1_byte_offset 	308	/* 0x00000134 */
#define flexidag_openseg_prim_split_2___pvcn_835__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2___pvcn_835__W 	1408
#define flexidag_openseg_prim_split_2___pvcn_835__H 	1
#define flexidag_openseg_prim_split_2___pvcn_835__D 	1
#define flexidag_openseg_prim_split_2___pvcn_835__P 	1
#define flexidag_openseg_prim_split_2___pvcn_835__data_num_bytes 	1
#define flexidag_openseg_prim_split_2___pvcn_835__dpitch_num_bytes 	1408
#define flexidag_openseg_prim_split_2___pvcn_835__dram_format 	0
#define flexidag_openseg_prim_split_2___pvcn_835__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_2___pvcn_835__tile_width 	1
#define flexidag_openseg_prim_split_2___pvcn_835__tile_height 	1
#define flexidag_openseg_prim_split_2___pvcn_835__vmem_buffer_num_bytes 	1408

/* flexidag_openseg_prim_split_2 SMB_input __pvcn_836_ */
#define flexidag_openseg_prim_split_2___pvcn_836__cnngen_demangled_name 	"__pvcn_836_"
#define flexidag_openseg_prim_split_2___pvcn_836__is_constant 	1
#define flexidag_openseg_prim_split_2___pvcn_836__is_bitvector 	0
#define flexidag_openseg_prim_split_2___pvcn_836__is_rlz 	0
#define flexidag_openseg_prim_split_2___pvcn_836__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_836__byte_offset 	316	/* 0x0000013c */
#define flexidag_openseg_prim_split_2___pvcn_836__vbase_byte_offset 	320	/* 0x00000140 */
#define flexidag_openseg_prim_split_2___pvcn_836__dbase_byte_offset 	344	/* 0x00000158 */
#define flexidag_openseg_prim_split_2___pvcn_836__dpitchm1_byte_offset 	348	/* 0x0000015c */
#define flexidag_openseg_prim_split_2___pvcn_836__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2___pvcn_836__W 	80
#define flexidag_openseg_prim_split_2___pvcn_836__H 	1
#define flexidag_openseg_prim_split_2___pvcn_836__D 	1
#define flexidag_openseg_prim_split_2___pvcn_836__P 	1
#define flexidag_openseg_prim_split_2___pvcn_836__data_num_bytes 	1
#define flexidag_openseg_prim_split_2___pvcn_836__dpitch_num_bytes 	96
#define flexidag_openseg_prim_split_2___pvcn_836__dram_format 	0
#define flexidag_openseg_prim_split_2___pvcn_836__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_2___pvcn_836__tile_width 	1
#define flexidag_openseg_prim_split_2___pvcn_836__tile_height 	1
#define flexidag_openseg_prim_split_2___pvcn_836__vmem_buffer_num_bytes 	80

/* flexidag_openseg_prim_split_2 SMB_input __pvcn_842_ */
#define flexidag_openseg_prim_split_2___pvcn_842__cnngen_demangled_name 	"__pvcn_842_"
#define flexidag_openseg_prim_split_2___pvcn_842__is_constant 	1
#define flexidag_openseg_prim_split_2___pvcn_842__is_bitvector 	0
#define flexidag_openseg_prim_split_2___pvcn_842__is_rlz 	0
#define flexidag_openseg_prim_split_2___pvcn_842__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_842__byte_offset 	356	/* 0x00000164 */
#define flexidag_openseg_prim_split_2___pvcn_842__vbase_byte_offset 	360	/* 0x00000168 */
#define flexidag_openseg_prim_split_2___pvcn_842__dbase_byte_offset 	384	/* 0x00000180 */
#define flexidag_openseg_prim_split_2___pvcn_842__dpitchm1_byte_offset 	388	/* 0x00000184 */
#define flexidag_openseg_prim_split_2___pvcn_842__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2___pvcn_842__W 	25568
#define flexidag_openseg_prim_split_2___pvcn_842__H 	1
#define flexidag_openseg_prim_split_2___pvcn_842__D 	1
#define flexidag_openseg_prim_split_2___pvcn_842__P 	1
#define flexidag_openseg_prim_split_2___pvcn_842__data_num_bytes 	1
#define flexidag_openseg_prim_split_2___pvcn_842__dpitch_num_bytes 	25568
#define flexidag_openseg_prim_split_2___pvcn_842__dram_format 	0
#define flexidag_openseg_prim_split_2___pvcn_842__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_2___pvcn_842__tile_width 	1
#define flexidag_openseg_prim_split_2___pvcn_842__tile_height 	1
#define flexidag_openseg_prim_split_2___pvcn_842__vmem_buffer_num_bytes 	25568

/* flexidag_openseg_prim_split_2 SMB_input __pvcn_843_ */
#define flexidag_openseg_prim_split_2___pvcn_843__cnngen_demangled_name 	"__pvcn_843_"
#define flexidag_openseg_prim_split_2___pvcn_843__is_constant 	1
#define flexidag_openseg_prim_split_2___pvcn_843__is_bitvector 	0
#define flexidag_openseg_prim_split_2___pvcn_843__is_rlz 	0
#define flexidag_openseg_prim_split_2___pvcn_843__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_843__byte_offset 	396	/* 0x0000018c */
#define flexidag_openseg_prim_split_2___pvcn_843__vbase_byte_offset 	400	/* 0x00000190 */
#define flexidag_openseg_prim_split_2___pvcn_843__dbase_byte_offset 	424	/* 0x000001a8 */
#define flexidag_openseg_prim_split_2___pvcn_843__dpitchm1_byte_offset 	428	/* 0x000001ac */
#define flexidag_openseg_prim_split_2___pvcn_843__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2___pvcn_843__W 	640
#define flexidag_openseg_prim_split_2___pvcn_843__H 	1
#define flexidag_openseg_prim_split_2___pvcn_843__D 	1
#define flexidag_openseg_prim_split_2___pvcn_843__P 	1
#define flexidag_openseg_prim_split_2___pvcn_843__data_num_bytes 	1
#define flexidag_openseg_prim_split_2___pvcn_843__dpitch_num_bytes 	640
#define flexidag_openseg_prim_split_2___pvcn_843__dram_format 	0
#define flexidag_openseg_prim_split_2___pvcn_843__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_2___pvcn_843__tile_width 	1
#define flexidag_openseg_prim_split_2___pvcn_843__tile_height 	1
#define flexidag_openseg_prim_split_2___pvcn_843__vmem_buffer_num_bytes 	640

/* flexidag_openseg_prim_split_2 SMB_input __pvcn_848_ */
#define flexidag_openseg_prim_split_2___pvcn_848__cnngen_demangled_name 	"__pvcn_848_"
#define flexidag_openseg_prim_split_2___pvcn_848__is_constant 	1
#define flexidag_openseg_prim_split_2___pvcn_848__is_bitvector 	0
#define flexidag_openseg_prim_split_2___pvcn_848__is_rlz 	0
#define flexidag_openseg_prim_split_2___pvcn_848__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_848__byte_offset 	448	/* 0x000001c0 */
#define flexidag_openseg_prim_split_2___pvcn_848__vbase_byte_offset 	452	/* 0x000001c4 */
#define flexidag_openseg_prim_split_2___pvcn_848__dbase_byte_offset 	476	/* 0x000001dc */
#define flexidag_openseg_prim_split_2___pvcn_848__dpitchm1_byte_offset 	480	/* 0x000001e0 */
#define flexidag_openseg_prim_split_2___pvcn_848__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2___pvcn_848__W 	5784
#define flexidag_openseg_prim_split_2___pvcn_848__H 	1
#define flexidag_openseg_prim_split_2___pvcn_848__D 	1
#define flexidag_openseg_prim_split_2___pvcn_848__P 	1
#define flexidag_openseg_prim_split_2___pvcn_848__data_num_bytes 	1
#define flexidag_openseg_prim_split_2___pvcn_848__dpitch_num_bytes 	5792
#define flexidag_openseg_prim_split_2___pvcn_848__dram_format 	0
#define flexidag_openseg_prim_split_2___pvcn_848__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_2___pvcn_848__tile_width 	1
#define flexidag_openseg_prim_split_2___pvcn_848__tile_height 	1
#define flexidag_openseg_prim_split_2___pvcn_848__vmem_buffer_num_bytes 	5784

/* flexidag_openseg_prim_split_2 SMB_input __pvcn_849_ */
#define flexidag_openseg_prim_split_2___pvcn_849__cnngen_demangled_name 	"__pvcn_849_"
#define flexidag_openseg_prim_split_2___pvcn_849__is_constant 	1
#define flexidag_openseg_prim_split_2___pvcn_849__is_bitvector 	0
#define flexidag_openseg_prim_split_2___pvcn_849__is_rlz 	0
#define flexidag_openseg_prim_split_2___pvcn_849__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_849__byte_offset 	488	/* 0x000001e8 */
#define flexidag_openseg_prim_split_2___pvcn_849__vbase_byte_offset 	492	/* 0x000001ec */
#define flexidag_openseg_prim_split_2___pvcn_849__dbase_byte_offset 	516	/* 0x00000204 */
#define flexidag_openseg_prim_split_2___pvcn_849__dpitchm1_byte_offset 	520	/* 0x00000208 */
#define flexidag_openseg_prim_split_2___pvcn_849__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2___pvcn_849__W 	80
#define flexidag_openseg_prim_split_2___pvcn_849__H 	1
#define flexidag_openseg_prim_split_2___pvcn_849__D 	1
#define flexidag_openseg_prim_split_2___pvcn_849__P 	1
#define flexidag_openseg_prim_split_2___pvcn_849__data_num_bytes 	1
#define flexidag_openseg_prim_split_2___pvcn_849__dpitch_num_bytes 	96
#define flexidag_openseg_prim_split_2___pvcn_849__dram_format 	0
#define flexidag_openseg_prim_split_2___pvcn_849__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_2___pvcn_849__tile_width 	1
#define flexidag_openseg_prim_split_2___pvcn_849__tile_height 	1
#define flexidag_openseg_prim_split_2___pvcn_849__vmem_buffer_num_bytes 	80

/* flexidag_openseg_prim_split_2 SMB_input __pvcn_852_ */
#define flexidag_openseg_prim_split_2___pvcn_852__cnngen_demangled_name 	"__pvcn_852_"
#define flexidag_openseg_prim_split_2___pvcn_852__is_constant 	1
#define flexidag_openseg_prim_split_2___pvcn_852__is_bitvector 	0
#define flexidag_openseg_prim_split_2___pvcn_852__is_rlz 	0
#define flexidag_openseg_prim_split_2___pvcn_852__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_852__byte_offset 	528	/* 0x00000210 */
#define flexidag_openseg_prim_split_2___pvcn_852__vbase_byte_offset 	532	/* 0x00000214 */
#define flexidag_openseg_prim_split_2___pvcn_852__dbase_byte_offset 	556	/* 0x0000022c */
#define flexidag_openseg_prim_split_2___pvcn_852__dpitchm1_byte_offset 	560	/* 0x00000230 */
#define flexidag_openseg_prim_split_2___pvcn_852__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2___pvcn_852__W 	5660
#define flexidag_openseg_prim_split_2___pvcn_852__H 	1
#define flexidag_openseg_prim_split_2___pvcn_852__D 	1
#define flexidag_openseg_prim_split_2___pvcn_852__P 	1
#define flexidag_openseg_prim_split_2___pvcn_852__data_num_bytes 	1
#define flexidag_openseg_prim_split_2___pvcn_852__dpitch_num_bytes 	5664
#define flexidag_openseg_prim_split_2___pvcn_852__dram_format 	0
#define flexidag_openseg_prim_split_2___pvcn_852__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_2___pvcn_852__tile_width 	1
#define flexidag_openseg_prim_split_2___pvcn_852__tile_height 	1
#define flexidag_openseg_prim_split_2___pvcn_852__vmem_buffer_num_bytes 	5660

/* flexidag_openseg_prim_split_2 SMB_input __pvcn_853_ */
#define flexidag_openseg_prim_split_2___pvcn_853__cnngen_demangled_name 	"__pvcn_853_"
#define flexidag_openseg_prim_split_2___pvcn_853__is_constant 	1
#define flexidag_openseg_prim_split_2___pvcn_853__is_bitvector 	0
#define flexidag_openseg_prim_split_2___pvcn_853__is_rlz 	0
#define flexidag_openseg_prim_split_2___pvcn_853__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_853__byte_offset 	568	/* 0x00000238 */
#define flexidag_openseg_prim_split_2___pvcn_853__vbase_byte_offset 	572	/* 0x0000023c */
#define flexidag_openseg_prim_split_2___pvcn_853__dbase_byte_offset 	596	/* 0x00000254 */
#define flexidag_openseg_prim_split_2___pvcn_853__dpitchm1_byte_offset 	600	/* 0x00000258 */
#define flexidag_openseg_prim_split_2___pvcn_853__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2___pvcn_853__W 	80
#define flexidag_openseg_prim_split_2___pvcn_853__H 	1
#define flexidag_openseg_prim_split_2___pvcn_853__D 	1
#define flexidag_openseg_prim_split_2___pvcn_853__P 	1
#define flexidag_openseg_prim_split_2___pvcn_853__data_num_bytes 	1
#define flexidag_openseg_prim_split_2___pvcn_853__dpitch_num_bytes 	96
#define flexidag_openseg_prim_split_2___pvcn_853__dram_format 	0
#define flexidag_openseg_prim_split_2___pvcn_853__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_2___pvcn_853__tile_width 	1
#define flexidag_openseg_prim_split_2___pvcn_853__tile_height 	1
#define flexidag_openseg_prim_split_2___pvcn_853__vmem_buffer_num_bytes 	80

/* flexidag_openseg_prim_split_2 SMB_input __pvcn_855_ */
#define flexidag_openseg_prim_split_2___pvcn_855__cnngen_demangled_name 	"__pvcn_855_"
#define flexidag_openseg_prim_split_2___pvcn_855__is_constant 	1
#define flexidag_openseg_prim_split_2___pvcn_855__is_bitvector 	0
#define flexidag_openseg_prim_split_2___pvcn_855__is_rlz 	0
#define flexidag_openseg_prim_split_2___pvcn_855__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_855__byte_offset 	608	/* 0x00000260 */
#define flexidag_openseg_prim_split_2___pvcn_855__vbase_byte_offset 	612	/* 0x00000264 */
#define flexidag_openseg_prim_split_2___pvcn_855__dbase_byte_offset 	636	/* 0x0000027c */
#define flexidag_openseg_prim_split_2___pvcn_855__dpitchm1_byte_offset 	640	/* 0x00000280 */
#define flexidag_openseg_prim_split_2___pvcn_855__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2___pvcn_855__W 	1436
#define flexidag_openseg_prim_split_2___pvcn_855__H 	1
#define flexidag_openseg_prim_split_2___pvcn_855__D 	1
#define flexidag_openseg_prim_split_2___pvcn_855__P 	1
#define flexidag_openseg_prim_split_2___pvcn_855__data_num_bytes 	1
#define flexidag_openseg_prim_split_2___pvcn_855__dpitch_num_bytes 	1440
#define flexidag_openseg_prim_split_2___pvcn_855__dram_format 	0
#define flexidag_openseg_prim_split_2___pvcn_855__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_2___pvcn_855__tile_width 	1
#define flexidag_openseg_prim_split_2___pvcn_855__tile_height 	1
#define flexidag_openseg_prim_split_2___pvcn_855__vmem_buffer_num_bytes 	1436

/* flexidag_openseg_prim_split_2 SMB_input __pvcn_856_ */
#define flexidag_openseg_prim_split_2___pvcn_856__cnngen_demangled_name 	"__pvcn_856_"
#define flexidag_openseg_prim_split_2___pvcn_856__is_constant 	1
#define flexidag_openseg_prim_split_2___pvcn_856__is_bitvector 	0
#define flexidag_openseg_prim_split_2___pvcn_856__is_rlz 	0
#define flexidag_openseg_prim_split_2___pvcn_856__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_856__byte_offset 	648	/* 0x00000288 */
#define flexidag_openseg_prim_split_2___pvcn_856__vbase_byte_offset 	652	/* 0x0000028c */
#define flexidag_openseg_prim_split_2___pvcn_856__dbase_byte_offset 	676	/* 0x000002a4 */
#define flexidag_openseg_prim_split_2___pvcn_856__dpitchm1_byte_offset 	680	/* 0x000002a8 */
#define flexidag_openseg_prim_split_2___pvcn_856__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2___pvcn_856__W 	80
#define flexidag_openseg_prim_split_2___pvcn_856__H 	1
#define flexidag_openseg_prim_split_2___pvcn_856__D 	1
#define flexidag_openseg_prim_split_2___pvcn_856__P 	1
#define flexidag_openseg_prim_split_2___pvcn_856__data_num_bytes 	1
#define flexidag_openseg_prim_split_2___pvcn_856__dpitch_num_bytes 	96
#define flexidag_openseg_prim_split_2___pvcn_856__dram_format 	0
#define flexidag_openseg_prim_split_2___pvcn_856__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_2___pvcn_856__tile_width 	1
#define flexidag_openseg_prim_split_2___pvcn_856__tile_height 	1
#define flexidag_openseg_prim_split_2___pvcn_856__vmem_buffer_num_bytes 	80

/* flexidag_openseg_prim_split_2 SMB_input __pvcn_862_ */
#define flexidag_openseg_prim_split_2___pvcn_862__cnngen_demangled_name 	"__pvcn_862_"
#define flexidag_openseg_prim_split_2___pvcn_862__is_constant 	1
#define flexidag_openseg_prim_split_2___pvcn_862__is_bitvector 	0
#define flexidag_openseg_prim_split_2___pvcn_862__is_rlz 	0
#define flexidag_openseg_prim_split_2___pvcn_862__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_862__byte_offset 	688	/* 0x000002b0 */
#define flexidag_openseg_prim_split_2___pvcn_862__vbase_byte_offset 	692	/* 0x000002b4 */
#define flexidag_openseg_prim_split_2___pvcn_862__dbase_byte_offset 	716	/* 0x000002cc */
#define flexidag_openseg_prim_split_2___pvcn_862__dpitchm1_byte_offset 	720	/* 0x000002d0 */
#define flexidag_openseg_prim_split_2___pvcn_862__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2___pvcn_862__W 	28200
#define flexidag_openseg_prim_split_2___pvcn_862__H 	1
#define flexidag_openseg_prim_split_2___pvcn_862__D 	1
#define flexidag_openseg_prim_split_2___pvcn_862__P 	1
#define flexidag_openseg_prim_split_2___pvcn_862__data_num_bytes 	1
#define flexidag_openseg_prim_split_2___pvcn_862__dpitch_num_bytes 	28224
#define flexidag_openseg_prim_split_2___pvcn_862__dram_format 	0
#define flexidag_openseg_prim_split_2___pvcn_862__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_2___pvcn_862__tile_width 	1
#define flexidag_openseg_prim_split_2___pvcn_862__tile_height 	1
#define flexidag_openseg_prim_split_2___pvcn_862__vmem_buffer_num_bytes 	28200

/* flexidag_openseg_prim_split_2 SMB_input __pvcn_863_ */
#define flexidag_openseg_prim_split_2___pvcn_863__cnngen_demangled_name 	"__pvcn_863_"
#define flexidag_openseg_prim_split_2___pvcn_863__is_constant 	1
#define flexidag_openseg_prim_split_2___pvcn_863__is_bitvector 	0
#define flexidag_openseg_prim_split_2___pvcn_863__is_rlz 	0
#define flexidag_openseg_prim_split_2___pvcn_863__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_863__byte_offset 	728	/* 0x000002d8 */
#define flexidag_openseg_prim_split_2___pvcn_863__vbase_byte_offset 	732	/* 0x000002dc */
#define flexidag_openseg_prim_split_2___pvcn_863__dbase_byte_offset 	756	/* 0x000002f4 */
#define flexidag_openseg_prim_split_2___pvcn_863__dpitchm1_byte_offset 	760	/* 0x000002f8 */
#define flexidag_openseg_prim_split_2___pvcn_863__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2___pvcn_863__W 	640
#define flexidag_openseg_prim_split_2___pvcn_863__H 	1
#define flexidag_openseg_prim_split_2___pvcn_863__D 	1
#define flexidag_openseg_prim_split_2___pvcn_863__P 	1
#define flexidag_openseg_prim_split_2___pvcn_863__data_num_bytes 	1
#define flexidag_openseg_prim_split_2___pvcn_863__dpitch_num_bytes 	640
#define flexidag_openseg_prim_split_2___pvcn_863__dram_format 	0
#define flexidag_openseg_prim_split_2___pvcn_863__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_2___pvcn_863__tile_width 	1
#define flexidag_openseg_prim_split_2___pvcn_863__tile_height 	1
#define flexidag_openseg_prim_split_2___pvcn_863__vmem_buffer_num_bytes 	640

/* flexidag_openseg_prim_split_2 SMB_input __pvcn_866_ */
#define flexidag_openseg_prim_split_2___pvcn_866__cnngen_demangled_name 	"__pvcn_866_"
#define flexidag_openseg_prim_split_2___pvcn_866__is_constant 	1
#define flexidag_openseg_prim_split_2___pvcn_866__is_bitvector 	0
#define flexidag_openseg_prim_split_2___pvcn_866__is_rlz 	0
#define flexidag_openseg_prim_split_2___pvcn_866__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_866__byte_offset 	768	/* 0x00000300 */
#define flexidag_openseg_prim_split_2___pvcn_866__vbase_byte_offset 	772	/* 0x00000304 */
#define flexidag_openseg_prim_split_2___pvcn_866__dbase_byte_offset 	796	/* 0x0000031c */
#define flexidag_openseg_prim_split_2___pvcn_866__dpitchm1_byte_offset 	800	/* 0x00000320 */
#define flexidag_openseg_prim_split_2___pvcn_866__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2___pvcn_866__W 	5736
#define flexidag_openseg_prim_split_2___pvcn_866__H 	1
#define flexidag_openseg_prim_split_2___pvcn_866__D 	1
#define flexidag_openseg_prim_split_2___pvcn_866__P 	1
#define flexidag_openseg_prim_split_2___pvcn_866__data_num_bytes 	1
#define flexidag_openseg_prim_split_2___pvcn_866__dpitch_num_bytes 	5760
#define flexidag_openseg_prim_split_2___pvcn_866__dram_format 	0
#define flexidag_openseg_prim_split_2___pvcn_866__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_2___pvcn_866__tile_width 	1
#define flexidag_openseg_prim_split_2___pvcn_866__tile_height 	1
#define flexidag_openseg_prim_split_2___pvcn_866__vmem_buffer_num_bytes 	5736

/* flexidag_openseg_prim_split_2 SMB_input __pvcn_867_ */
#define flexidag_openseg_prim_split_2___pvcn_867__cnngen_demangled_name 	"__pvcn_867_"
#define flexidag_openseg_prim_split_2___pvcn_867__is_constant 	1
#define flexidag_openseg_prim_split_2___pvcn_867__is_bitvector 	0
#define flexidag_openseg_prim_split_2___pvcn_867__is_rlz 	0
#define flexidag_openseg_prim_split_2___pvcn_867__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_867__byte_offset 	808	/* 0x00000328 */
#define flexidag_openseg_prim_split_2___pvcn_867__vbase_byte_offset 	812	/* 0x0000032c */
#define flexidag_openseg_prim_split_2___pvcn_867__dbase_byte_offset 	836	/* 0x00000344 */
#define flexidag_openseg_prim_split_2___pvcn_867__dpitchm1_byte_offset 	840	/* 0x00000348 */
#define flexidag_openseg_prim_split_2___pvcn_867__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2___pvcn_867__W 	80
#define flexidag_openseg_prim_split_2___pvcn_867__H 	1
#define flexidag_openseg_prim_split_2___pvcn_867__D 	1
#define flexidag_openseg_prim_split_2___pvcn_867__P 	1
#define flexidag_openseg_prim_split_2___pvcn_867__data_num_bytes 	1
#define flexidag_openseg_prim_split_2___pvcn_867__dpitch_num_bytes 	96
#define flexidag_openseg_prim_split_2___pvcn_867__dram_format 	0
#define flexidag_openseg_prim_split_2___pvcn_867__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_2___pvcn_867__tile_width 	1
#define flexidag_openseg_prim_split_2___pvcn_867__tile_height 	1
#define flexidag_openseg_prim_split_2___pvcn_867__vmem_buffer_num_bytes 	80

/* flexidag_openseg_prim_split_2 SMB_input __pvcn_870_ */
#define flexidag_openseg_prim_split_2___pvcn_870__cnngen_demangled_name 	"__pvcn_870_"
#define flexidag_openseg_prim_split_2___pvcn_870__is_constant 	1
#define flexidag_openseg_prim_split_2___pvcn_870__is_bitvector 	0
#define flexidag_openseg_prim_split_2___pvcn_870__is_rlz 	0
#define flexidag_openseg_prim_split_2___pvcn_870__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_870__byte_offset 	848	/* 0x00000350 */
#define flexidag_openseg_prim_split_2___pvcn_870__vbase_byte_offset 	852	/* 0x00000354 */
#define flexidag_openseg_prim_split_2___pvcn_870__dbase_byte_offset 	876	/* 0x0000036c */
#define flexidag_openseg_prim_split_2___pvcn_870__dpitchm1_byte_offset 	880	/* 0x00000370 */
#define flexidag_openseg_prim_split_2___pvcn_870__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2___pvcn_870__W 	5748
#define flexidag_openseg_prim_split_2___pvcn_870__H 	1
#define flexidag_openseg_prim_split_2___pvcn_870__D 	1
#define flexidag_openseg_prim_split_2___pvcn_870__P 	1
#define flexidag_openseg_prim_split_2___pvcn_870__data_num_bytes 	1
#define flexidag_openseg_prim_split_2___pvcn_870__dpitch_num_bytes 	5760
#define flexidag_openseg_prim_split_2___pvcn_870__dram_format 	0
#define flexidag_openseg_prim_split_2___pvcn_870__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_2___pvcn_870__tile_width 	1
#define flexidag_openseg_prim_split_2___pvcn_870__tile_height 	1
#define flexidag_openseg_prim_split_2___pvcn_870__vmem_buffer_num_bytes 	5748

/* flexidag_openseg_prim_split_2 SMB_input __pvcn_871_ */
#define flexidag_openseg_prim_split_2___pvcn_871__cnngen_demangled_name 	"__pvcn_871_"
#define flexidag_openseg_prim_split_2___pvcn_871__is_constant 	1
#define flexidag_openseg_prim_split_2___pvcn_871__is_bitvector 	0
#define flexidag_openseg_prim_split_2___pvcn_871__is_rlz 	0
#define flexidag_openseg_prim_split_2___pvcn_871__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_871__byte_offset 	888	/* 0x00000378 */
#define flexidag_openseg_prim_split_2___pvcn_871__vbase_byte_offset 	892	/* 0x0000037c */
#define flexidag_openseg_prim_split_2___pvcn_871__dbase_byte_offset 	916	/* 0x00000394 */
#define flexidag_openseg_prim_split_2___pvcn_871__dpitchm1_byte_offset 	920	/* 0x00000398 */
#define flexidag_openseg_prim_split_2___pvcn_871__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2___pvcn_871__W 	88
#define flexidag_openseg_prim_split_2___pvcn_871__H 	1
#define flexidag_openseg_prim_split_2___pvcn_871__D 	1
#define flexidag_openseg_prim_split_2___pvcn_871__P 	1
#define flexidag_openseg_prim_split_2___pvcn_871__data_num_bytes 	1
#define flexidag_openseg_prim_split_2___pvcn_871__dpitch_num_bytes 	96
#define flexidag_openseg_prim_split_2___pvcn_871__dram_format 	0
#define flexidag_openseg_prim_split_2___pvcn_871__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_2___pvcn_871__tile_width 	1
#define flexidag_openseg_prim_split_2___pvcn_871__tile_height 	1
#define flexidag_openseg_prim_split_2___pvcn_871__vmem_buffer_num_bytes 	88

/* flexidag_openseg_prim_split_2 SMB_input __pvcn_873_ */
#define flexidag_openseg_prim_split_2___pvcn_873__cnngen_demangled_name 	"__pvcn_873_"
#define flexidag_openseg_prim_split_2___pvcn_873__is_constant 	1
#define flexidag_openseg_prim_split_2___pvcn_873__is_bitvector 	0
#define flexidag_openseg_prim_split_2___pvcn_873__is_rlz 	0
#define flexidag_openseg_prim_split_2___pvcn_873__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_873__byte_offset 	928	/* 0x000003a0 */
#define flexidag_openseg_prim_split_2___pvcn_873__vbase_byte_offset 	932	/* 0x000003a4 */
#define flexidag_openseg_prim_split_2___pvcn_873__dbase_byte_offset 	956	/* 0x000003bc */
#define flexidag_openseg_prim_split_2___pvcn_873__dpitchm1_byte_offset 	960	/* 0x000003c0 */
#define flexidag_openseg_prim_split_2___pvcn_873__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2___pvcn_873__W 	1440
#define flexidag_openseg_prim_split_2___pvcn_873__H 	1
#define flexidag_openseg_prim_split_2___pvcn_873__D 	1
#define flexidag_openseg_prim_split_2___pvcn_873__P 	1
#define flexidag_openseg_prim_split_2___pvcn_873__data_num_bytes 	1
#define flexidag_openseg_prim_split_2___pvcn_873__dpitch_num_bytes 	1440
#define flexidag_openseg_prim_split_2___pvcn_873__dram_format 	0
#define flexidag_openseg_prim_split_2___pvcn_873__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_2___pvcn_873__tile_width 	1
#define flexidag_openseg_prim_split_2___pvcn_873__tile_height 	1
#define flexidag_openseg_prim_split_2___pvcn_873__vmem_buffer_num_bytes 	1440

/* flexidag_openseg_prim_split_2 SMB_input __pvcn_874_ */
#define flexidag_openseg_prim_split_2___pvcn_874__cnngen_demangled_name 	"__pvcn_874_"
#define flexidag_openseg_prim_split_2___pvcn_874__is_constant 	1
#define flexidag_openseg_prim_split_2___pvcn_874__is_bitvector 	0
#define flexidag_openseg_prim_split_2___pvcn_874__is_rlz 	0
#define flexidag_openseg_prim_split_2___pvcn_874__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_874__byte_offset 	968	/* 0x000003c8 */
#define flexidag_openseg_prim_split_2___pvcn_874__vbase_byte_offset 	972	/* 0x000003cc */
#define flexidag_openseg_prim_split_2___pvcn_874__dbase_byte_offset 	996	/* 0x000003e4 */
#define flexidag_openseg_prim_split_2___pvcn_874__dpitchm1_byte_offset 	1000	/* 0x000003e8 */
#define flexidag_openseg_prim_split_2___pvcn_874__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2___pvcn_874__W 	88
#define flexidag_openseg_prim_split_2___pvcn_874__H 	1
#define flexidag_openseg_prim_split_2___pvcn_874__D 	1
#define flexidag_openseg_prim_split_2___pvcn_874__P 	1
#define flexidag_openseg_prim_split_2___pvcn_874__data_num_bytes 	1
#define flexidag_openseg_prim_split_2___pvcn_874__dpitch_num_bytes 	96
#define flexidag_openseg_prim_split_2___pvcn_874__dram_format 	0
#define flexidag_openseg_prim_split_2___pvcn_874__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_2___pvcn_874__tile_width 	1
#define flexidag_openseg_prim_split_2___pvcn_874__tile_height 	1
#define flexidag_openseg_prim_split_2___pvcn_874__vmem_buffer_num_bytes 	88

/* flexidag_openseg_prim_split_2 SMB_input __pvcn_880_ */
#define flexidag_openseg_prim_split_2___pvcn_880__cnngen_demangled_name 	"__pvcn_880_"
#define flexidag_openseg_prim_split_2___pvcn_880__is_constant 	1
#define flexidag_openseg_prim_split_2___pvcn_880__is_bitvector 	0
#define flexidag_openseg_prim_split_2___pvcn_880__is_rlz 	0
#define flexidag_openseg_prim_split_2___pvcn_880__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_880__byte_offset 	1008	/* 0x000003f0 */
#define flexidag_openseg_prim_split_2___pvcn_880__vbase_byte_offset 	1012	/* 0x000003f4 */
#define flexidag_openseg_prim_split_2___pvcn_880__dbase_byte_offset 	1036	/* 0x0000040c */
#define flexidag_openseg_prim_split_2___pvcn_880__dpitchm1_byte_offset 	1040	/* 0x00000410 */
#define flexidag_openseg_prim_split_2___pvcn_880__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2___pvcn_880__W 	30664
#define flexidag_openseg_prim_split_2___pvcn_880__H 	1
#define flexidag_openseg_prim_split_2___pvcn_880__D 	1
#define flexidag_openseg_prim_split_2___pvcn_880__P 	1
#define flexidag_openseg_prim_split_2___pvcn_880__data_num_bytes 	1
#define flexidag_openseg_prim_split_2___pvcn_880__dpitch_num_bytes 	30688
#define flexidag_openseg_prim_split_2___pvcn_880__dram_format 	0
#define flexidag_openseg_prim_split_2___pvcn_880__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_2___pvcn_880__tile_width 	1
#define flexidag_openseg_prim_split_2___pvcn_880__tile_height 	1
#define flexidag_openseg_prim_split_2___pvcn_880__vmem_buffer_num_bytes 	30664

/* flexidag_openseg_prim_split_2 SMB_input __pvcn_881_ */
#define flexidag_openseg_prim_split_2___pvcn_881__cnngen_demangled_name 	"__pvcn_881_"
#define flexidag_openseg_prim_split_2___pvcn_881__is_constant 	1
#define flexidag_openseg_prim_split_2___pvcn_881__is_bitvector 	0
#define flexidag_openseg_prim_split_2___pvcn_881__is_rlz 	0
#define flexidag_openseg_prim_split_2___pvcn_881__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_881__byte_offset 	1048	/* 0x00000418 */
#define flexidag_openseg_prim_split_2___pvcn_881__vbase_byte_offset 	1052	/* 0x0000041c */
#define flexidag_openseg_prim_split_2___pvcn_881__dbase_byte_offset 	1076	/* 0x00000434 */
#define flexidag_openseg_prim_split_2___pvcn_881__dpitchm1_byte_offset 	1080	/* 0x00000438 */
#define flexidag_openseg_prim_split_2___pvcn_881__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2___pvcn_881__W 	640
#define flexidag_openseg_prim_split_2___pvcn_881__H 	1
#define flexidag_openseg_prim_split_2___pvcn_881__D 	1
#define flexidag_openseg_prim_split_2___pvcn_881__P 	1
#define flexidag_openseg_prim_split_2___pvcn_881__data_num_bytes 	1
#define flexidag_openseg_prim_split_2___pvcn_881__dpitch_num_bytes 	640
#define flexidag_openseg_prim_split_2___pvcn_881__dram_format 	0
#define flexidag_openseg_prim_split_2___pvcn_881__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_2___pvcn_881__tile_width 	1
#define flexidag_openseg_prim_split_2___pvcn_881__tile_height 	1
#define flexidag_openseg_prim_split_2___pvcn_881__vmem_buffer_num_bytes 	640

/* flexidag_openseg_prim_split_2 SMB_input __pvcn_886_ */
#define flexidag_openseg_prim_split_2___pvcn_886__cnngen_demangled_name 	"__pvcn_886_"
#define flexidag_openseg_prim_split_2___pvcn_886__is_constant 	1
#define flexidag_openseg_prim_split_2___pvcn_886__is_bitvector 	0
#define flexidag_openseg_prim_split_2___pvcn_886__is_rlz 	0
#define flexidag_openseg_prim_split_2___pvcn_886__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_886__byte_offset 	1100	/* 0x0000044c */
#define flexidag_openseg_prim_split_2___pvcn_886__vbase_byte_offset 	1104	/* 0x00000450 */
#define flexidag_openseg_prim_split_2___pvcn_886__dbase_byte_offset 	1128	/* 0x00000468 */
#define flexidag_openseg_prim_split_2___pvcn_886__dpitchm1_byte_offset 	1132	/* 0x0000046c */
#define flexidag_openseg_prim_split_2___pvcn_886__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2___pvcn_886__W 	5780
#define flexidag_openseg_prim_split_2___pvcn_886__H 	1
#define flexidag_openseg_prim_split_2___pvcn_886__D 	1
#define flexidag_openseg_prim_split_2___pvcn_886__P 	1
#define flexidag_openseg_prim_split_2___pvcn_886__data_num_bytes 	1
#define flexidag_openseg_prim_split_2___pvcn_886__dpitch_num_bytes 	5792
#define flexidag_openseg_prim_split_2___pvcn_886__dram_format 	0
#define flexidag_openseg_prim_split_2___pvcn_886__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_2___pvcn_886__tile_width 	1
#define flexidag_openseg_prim_split_2___pvcn_886__tile_height 	1
#define flexidag_openseg_prim_split_2___pvcn_886__vmem_buffer_num_bytes 	5780

/* flexidag_openseg_prim_split_2 SMB_input __pvcn_887_ */
#define flexidag_openseg_prim_split_2___pvcn_887__cnngen_demangled_name 	"__pvcn_887_"
#define flexidag_openseg_prim_split_2___pvcn_887__is_constant 	1
#define flexidag_openseg_prim_split_2___pvcn_887__is_bitvector 	0
#define flexidag_openseg_prim_split_2___pvcn_887__is_rlz 	0
#define flexidag_openseg_prim_split_2___pvcn_887__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2___pvcn_887__byte_offset 	1140	/* 0x00000474 */
#define flexidag_openseg_prim_split_2___pvcn_887__vbase_byte_offset 	1144	/* 0x00000478 */
#define flexidag_openseg_prim_split_2___pvcn_887__dbase_byte_offset 	1168	/* 0x00000490 */
#define flexidag_openseg_prim_split_2___pvcn_887__dpitchm1_byte_offset 	1172	/* 0x00000494 */
#define flexidag_openseg_prim_split_2___pvcn_887__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2___pvcn_887__W 	88
#define flexidag_openseg_prim_split_2___pvcn_887__H 	1
#define flexidag_openseg_prim_split_2___pvcn_887__D 	1
#define flexidag_openseg_prim_split_2___pvcn_887__P 	1
#define flexidag_openseg_prim_split_2___pvcn_887__data_num_bytes 	1
#define flexidag_openseg_prim_split_2___pvcn_887__dpitch_num_bytes 	96
#define flexidag_openseg_prim_split_2___pvcn_887__dram_format 	0
#define flexidag_openseg_prim_split_2___pvcn_887__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_2___pvcn_887__tile_width 	1
#define flexidag_openseg_prim_split_2___pvcn_887__tile_height 	1
#define flexidag_openseg_prim_split_2___pvcn_887__vmem_buffer_num_bytes 	88

/* flexidag_openseg_prim_split_2 VCB PeleeNet__stage_3_dense_block_1__concat */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__concat_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_1__concat"
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__concat_is_bitvector 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__concat_is_rlz 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__concat_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__concat_byte_offset 	1180	/* 0x0000049c */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__concat_vbase_byte_offset 	1188	/* 0x000004a4 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__concat_vmem_buffer_num_bytes 	312320

/* flexidag_openseg_prim_split_2 VCB PeleeNet__stage_3_dense_block_1__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_1__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm"
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_is_bitvector 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_is_rlz 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_byte_offset 	1232	/* 0x000004d0 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_vbase_byte_offset 	1240	/* 0x000004d8 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_vmem_buffer_num_bytes 	247808

/* flexidag_openseg_prim_split_2 VCB PeleeNet__stage_3_dense_block_1__conv_left_0__Relu */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__conv_left_0__Relu_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_1__conv_left_0__Relu"
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__conv_left_0__Relu_is_bitvector 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__conv_left_0__Relu_is_rlz 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__conv_left_0__Relu_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__conv_left_0__Relu_byte_offset 	1284	/* 0x00000504 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__conv_left_0__Relu_vbase_byte_offset 	1292	/* 0x0000050c */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__conv_left_0__Relu_vmem_buffer_num_bytes 	56320

/* flexidag_openseg_prim_split_2 VCB PeleeNet__stage_3_dense_block_1__conv_right_0__Relu */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__conv_right_0__Relu_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_1__conv_right_0__Relu"
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__conv_right_0__Relu_is_bitvector 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__conv_right_0__Relu_is_rlz 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__conv_right_0__Relu_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__conv_right_0__Relu_byte_offset 	1356	/* 0x0000054c */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__conv_right_0__Relu_vbase_byte_offset 	1364	/* 0x00000554 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__conv_right_0__Relu_vmem_buffer_num_bytes 	43264

/* flexidag_openseg_prim_split_2 VCB PeleeNet__stage_3_dense_block_1__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_1__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu"
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_bitvector 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_rlz 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_byte_offset 	1404	/* 0x0000057c */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vbase_byte_offset 	1412	/* 0x00000584 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_1__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vmem_buffer_num_bytes 	21504

/* flexidag_openseg_prim_split_2 VCB PeleeNet__stage_3_dense_block_2__concat */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_2__concat_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_2__concat"
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_2__concat_is_bitvector 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_2__concat_is_rlz 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_2__concat_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_2__concat_byte_offset 	1480	/* 0x000005c8 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_2__concat_vbase_byte_offset 	1488	/* 0x000005d0 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_2__concat_vmem_buffer_num_bytes 	281600

/* flexidag_openseg_prim_split_2 VCB PeleeNet__stage_3_dense_block_2__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_2__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_2__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm"
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_2__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_is_bitvector 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_2__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_is_rlz 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_2__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_2__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_byte_offset 	1552	/* 0x00000610 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_2__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_vbase_byte_offset 	1560	/* 0x00000618 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_2__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_vmem_buffer_num_bytes 	43008

/* flexidag_openseg_prim_split_2 VCB PeleeNet__stage_3_dense_block_2__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___muli___488___9 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_2__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___muli___488___9_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_2__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___muli___488___9"
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_2__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___muli___488___9_is_bitvector 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_2__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___muli___488___9_is_rlz 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_2__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___muli___488___9_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_2__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___muli___488___9_byte_offset 	1596	/* 0x0000063c */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_2__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___muli___488___9_vbase_byte_offset 	1604	/* 0x00000644 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_2__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___muli___488___9_vmem_buffer_num_bytes 	43008

/* flexidag_openseg_prim_split_2 VCB PeleeNet__stage_3_dense_block_2__conv_left_0__Relu */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_2__conv_left_0__Relu_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_2__conv_left_0__Relu"
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_2__conv_left_0__Relu_is_bitvector 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_2__conv_left_0__Relu_is_rlz 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_2__conv_left_0__Relu_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_2__conv_left_0__Relu_byte_offset 	1648	/* 0x00000670 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_2__conv_left_0__Relu_vbase_byte_offset 	1656	/* 0x00000678 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_2__conv_left_0__Relu_vmem_buffer_num_bytes 	28160

/* flexidag_openseg_prim_split_2 VCB PeleeNet__stage_3_dense_block_2__conv_right_0__Relu */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_2__conv_right_0__Relu_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_2__conv_right_0__Relu"
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_2__conv_right_0__Relu_is_bitvector 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_2__conv_right_0__Relu_is_rlz 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_2__conv_right_0__Relu_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_2__conv_right_0__Relu_byte_offset 	1720	/* 0x000006b8 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_2__conv_right_0__Relu_vbase_byte_offset 	1728	/* 0x000006c0 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_2__conv_right_0__Relu_vmem_buffer_num_bytes 	28160

/* flexidag_openseg_prim_split_2 VCB PeleeNet__stage_3_dense_block_2__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_2__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_2__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu"
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_2__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_bitvector 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_2__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_rlz 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_2__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_2__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_byte_offset 	1768	/* 0x000006e8 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_2__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vbase_byte_offset 	1776	/* 0x000006f0 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_2__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vmem_buffer_num_bytes 	14080

/* flexidag_openseg_prim_split_2 VCB PeleeNet__stage_3_dense_block_3__concat */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_3__concat_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_3__concat"
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_3__concat_is_bitvector 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_3__concat_is_rlz 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_3__concat_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_3__concat_byte_offset 	1824	/* 0x00000720 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_3__concat_vbase_byte_offset 	1832	/* 0x00000728 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_3__concat_vmem_buffer_num_bytes 	153600

/* flexidag_openseg_prim_split_2 VCB PeleeNet__stage_3_dense_block_3__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_3__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_3__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm"
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_3__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_is_bitvector 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_3__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_is_rlz 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_3__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_3__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_byte_offset 	1896	/* 0x00000768 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_3__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_vbase_byte_offset 	1904	/* 0x00000770 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_3__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_vmem_buffer_num_bytes 	165888

/* flexidag_openseg_prim_split_2 VCB PeleeNet__stage_3_dense_block_3__conv_left_0__Relu */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_3__conv_left_0__Relu_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_3__conv_left_0__Relu"
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_3__conv_left_0__Relu_is_bitvector 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_3__conv_left_0__Relu_is_rlz 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_3__conv_left_0__Relu_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_3__conv_left_0__Relu_byte_offset 	1948	/* 0x0000079c */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_3__conv_left_0__Relu_vbase_byte_offset 	1956	/* 0x000007a4 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_3__conv_left_0__Relu_vmem_buffer_num_bytes 	28160

/* flexidag_openseg_prim_split_2 VCB PeleeNet__stage_3_dense_block_3__conv_right_0__Relu */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_3__conv_right_0__Relu_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_3__conv_right_0__Relu"
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_3__conv_right_0__Relu_is_bitvector 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_3__conv_right_0__Relu_is_rlz 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_3__conv_right_0__Relu_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_3__conv_right_0__Relu_byte_offset 	2020	/* 0x000007e4 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_3__conv_right_0__Relu_vbase_byte_offset 	2028	/* 0x000007ec */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_3__conv_right_0__Relu_vmem_buffer_num_bytes 	43264

/* flexidag_openseg_prim_split_2 VCB PeleeNet__stage_3_dense_block_3__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_3__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_3__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu"
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_3__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_bitvector 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_3__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_rlz 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_3__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_3__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_byte_offset 	2068	/* 0x00000814 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_3__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vbase_byte_offset 	2076	/* 0x0000081c */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_3__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vmem_buffer_num_bytes 	28032

/* flexidag_openseg_prim_split_2 HMB_output __vcn_1207_ */
#define flexidag_openseg_prim_split_2___vcn_1207__cnngen_demangled_name 	"__vcn_1207_"
#define flexidag_openseg_prim_split_2___vcn_1207__is_main_input_output 	0
#define flexidag_openseg_prim_split_2___vcn_1207__is_bitvector 	0
#define flexidag_openseg_prim_split_2___vcn_1207__is_rlz 	0
#define flexidag_openseg_prim_split_2___vcn_1207__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2___vcn_1207__byte_offset 	2136	/* 0x00000858 */
#define flexidag_openseg_prim_split_2___vcn_1207__vbase_byte_offset 	2140	/* 0x0000085c */
#define flexidag_openseg_prim_split_2___vcn_1207__dbase_byte_offset 	2164	/* 0x00000874 */
#define flexidag_openseg_prim_split_2___vcn_1207__dpitchm1_byte_offset 	2168	/* 0x00000878 */
#define flexidag_openseg_prim_split_2___vcn_1207__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2___vcn_1207__vwidth_minus_one_byte_offset 	2152	/* 0x00000868 */
#define flexidag_openseg_prim_split_2___vcn_1207__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2___vcn_1207__vheight_minus_one_byte_offset 	2154	/* 0x0000086a */
#define flexidag_openseg_prim_split_2___vcn_1207__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2___vcn_1207__drotate_bit_offset 	17286	/* 0x00004386 */
#define flexidag_openseg_prim_split_2___vcn_1207__hflip_bit_offset 	17287	/* 0x00004387 */
#define flexidag_openseg_prim_split_2___vcn_1207__vflip_bit_offset 	17288	/* 0x00004388 */
#define flexidag_openseg_prim_split_2___vcn_1207__dflip_bit_offset 	17289	/* 0x00004389 */
#define flexidag_openseg_prim_split_2___vcn_1207__pflip_bit_offset 	17290	/* 0x0000438a */
#define flexidag_openseg_prim_split_2___vcn_1207__W 	80
#define flexidag_openseg_prim_split_2___vcn_1207__H 	32
#define flexidag_openseg_prim_split_2___vcn_1207__D 	384
#define flexidag_openseg_prim_split_2___vcn_1207__P 	1
#define flexidag_openseg_prim_split_2___vcn_1207__data_num_bytes 	1
#define flexidag_openseg_prim_split_2___vcn_1207__dpitch_num_bytes 	32
#define flexidag_openseg_prim_split_2___vcn_1207__dram_format 	3
#define flexidag_openseg_prim_split_2___vcn_1207__vp_interleave_mode 	2
#define flexidag_openseg_prim_split_2___vcn_1207__tile_width 	4
#define flexidag_openseg_prim_split_2___vcn_1207__tile_height 	1
#define flexidag_openseg_prim_split_2___vcn_1207__vmem_buffer_num_bytes 	61440
#define flexidag_openseg_prim_split_2___vcn_1207__dram_size_num_bytes 	983040
#define flexidag_openseg_prim_split_2___vcn_1207__sign 	0
#define flexidag_openseg_prim_split_2___vcn_1207__datasize 	0
#define flexidag_openseg_prim_split_2___vcn_1207__expoffset 	5
#define flexidag_openseg_prim_split_2___vcn_1207__expbits 	0
#define flexidag_openseg_prim_split_2___vcn_1207__drotate 	0
#define flexidag_openseg_prim_split_2___vcn_1207__hflip 	0
#define flexidag_openseg_prim_split_2___vcn_1207__vflip 	0
#define flexidag_openseg_prim_split_2___vcn_1207__dflip 	0
#define flexidag_openseg_prim_split_2___vcn_1207__pflip 	0

/* flexidag_openseg_prim_split_2 VCB PeleeNet__stage_3_dense_block_4__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_4__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm"
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_is_bitvector 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_is_rlz 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_byte_offset 	2188	/* 0x0000088c */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_vbase_byte_offset 	2196	/* 0x00000894 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_vmem_buffer_num_bytes 	43008

/* flexidag_openseg_prim_split_2 HMB_output PeleeNet__stage_3_dense_block_4__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___muli___494___5 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___muli___494___5_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_4__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___muli___494___5"
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___muli___494___5_is_main_input_output 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___muli___494___5_is_bitvector 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___muli___494___5_is_rlz 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___muli___494___5_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___muli___494___5_byte_offset 	2232	/* 0x000008b8 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___muli___494___5_vbase_byte_offset 	2236	/* 0x000008bc */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___muli___494___5_dbase_byte_offset 	2260	/* 0x000008d4 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___muli___494___5_dpitchm1_byte_offset 	2264	/* 0x000008d8 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___muli___494___5_dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___muli___494___5_vwidth_minus_one_byte_offset 	2248	/* 0x000008c8 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___muli___494___5_vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___muli___494___5_vheight_minus_one_byte_offset 	2250	/* 0x000008ca */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___muli___494___5_vheight_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___muli___494___5_drotate_bit_offset 	18054	/* 0x00004686 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___muli___494___5_hflip_bit_offset 	18055	/* 0x00004687 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___muli___494___5_vflip_bit_offset 	18056	/* 0x00004688 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___muli___494___5_dflip_bit_offset 	18057	/* 0x00004689 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___muli___494___5_pflip_bit_offset 	18058	/* 0x0000468a */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___muli___494___5_W 	80
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___muli___494___5_H 	32
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___muli___494___5_D 	128
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___muli___494___5_P 	1
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___muli___494___5_data_num_bytes 	1
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___muli___494___5_dpitch_num_bytes 	32
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___muli___494___5_dram_format 	3
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___muli___494___5_vp_interleave_mode 	2
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___muli___494___5_tile_width 	4
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___muli___494___5_tile_height 	1
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___muli___494___5_vmem_buffer_num_bytes 	30720
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___muli___494___5_dram_size_num_bytes 	327680
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___muli___494___5_sign 	1
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___muli___494___5_datasize 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___muli___494___5_expoffset 	5
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___muli___494___5_expbits 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___muli___494___5_drotate 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___muli___494___5_hflip 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___muli___494___5_vflip 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___muli___494___5_dflip 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___muli___494___5_pflip 	0

/* flexidag_openseg_prim_split_2 VCB PeleeNet__stage_3_dense_block_4__conv_left_0__Relu */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_left_0__Relu_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_4__conv_left_0__Relu"
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_left_0__Relu_is_bitvector 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_left_0__Relu_is_rlz 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_left_0__Relu_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_left_0__Relu_byte_offset 	2284	/* 0x000008ec */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_left_0__Relu_vbase_byte_offset 	2292	/* 0x000008f4 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_left_0__Relu_vmem_buffer_num_bytes 	43264

/* flexidag_openseg_prim_split_2 HMB_output PeleeNet__stage_3_dense_block_4__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_4__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu"
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_main_input_output 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_bitvector 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_rlz 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_byte_offset 	2332	/* 0x0000091c */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vbase_byte_offset 	2336	/* 0x00000920 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_dbase_byte_offset 	2360	/* 0x00000938 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_dpitchm1_byte_offset 	2364	/* 0x0000093c */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vwidth_minus_one_byte_offset 	2348	/* 0x0000092c */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vheight_minus_one_byte_offset 	2350	/* 0x0000092e */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vheight_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_drotate_bit_offset 	18854	/* 0x000049a6 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_hflip_bit_offset 	18855	/* 0x000049a7 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vflip_bit_offset 	18856	/* 0x000049a8 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_dflip_bit_offset 	18857	/* 0x000049a9 */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_pflip_bit_offset 	18858	/* 0x000049aa */
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_W 	80
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_H 	32
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_D 	16
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_P 	1
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_data_num_bytes 	1
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_dpitch_num_bytes 	32
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_format 	3
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vp_interleave_mode 	2
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_tile_width 	4
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_tile_height 	1
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vmem_buffer_num_bytes 	10496
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_size_num_bytes 	40960
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_sign 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_datasize 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_expoffset 	5
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_expbits 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_drotate 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_hflip 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vflip 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_dflip 	0
#define flexidag_openseg_prim_split_2_PeleeNet__stage_3_dense_block_4__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_pflip 	0


#endif /* flexidag_openseg_prim_split_2_H */
