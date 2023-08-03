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
#ifndef bisenet_pelee_picinfo__prim_split_4_H
#define bisenet_pelee_picinfo__prim_split_4_H

#define vas_gen_header_version                      5

/* dag name */
#define bisenet_pelee_picinfo__prim_split_4_DAG_NAME    "bisenet_pelee_picinfo__prim_split_4"

/* VDG file info */
#define bisenet_pelee_picinfo__prim_split_4_vdg_name    "bisenet_pelee_picinfo__split_4.vdg"

/* VDG DAG memory info */
#define bisenet_pelee_picinfo__prim_split_4_byte_offset     340 /* 0x00000154 */
#define bisenet_pelee_picinfo__prim_split_4_byte_size   1948    /* 0x0000079c */
#define bisenet_pelee_picinfo__prim_split_4_preferred_dram_xfer_size    1

/* bisenet_pelee_picinfo__prim_split_4 VMEM info */
#define bisenet_pelee_picinfo__prim_split_4_use_ping_pong_vmem  0
#define bisenet_pelee_picinfo__prim_split_4_VMEM_start  4096    /* 0x00001000 */
#define bisenet_pelee_picinfo__prim_split_4_VMEM_end    2072868 /* 0x001fa124 */
#define bisenet_pelee_picinfo__prim_split_4_image_start     1924736 /* 0x001d5e80 */
#define bisenet_pelee_picinfo__prim_split_4_image_size  150080  /* 0x00024a40 */
#define bisenet_pelee_picinfo__prim_split_4_dagbin_start    2072868 /* 0x001fa124 */

/*******************************************************************************
 * DAG byte offset info for HMB/SMB/VCB and their fields
 * in DAG 'bisenet_pelee_picinfo__prim_split_4' in source file 'bisenet_pelee_picinfo__split_4.vas'
 ******************************************************************************/
/* bisenet_pelee_picinfo__prim_split_4 HMB_input PeleeNet__stage_3_dense_block_4__concat */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_4__concat_cnngen_demangled_name   "PeleeNet__stage_3_dense_block_4__concat"
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_4__concat_is_constant     0
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_4__concat_ddi_byte_size   0   /* 0x00000000 */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_4__concat_ddi_byte_offset     4294967295  /* 0xffffffff */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_4__concat_byte_offset     0   /* 0x00000000 */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_4__concat_vbase_byte_offset   4   /* 0x00000004 */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_4__concat_dbase_byte_offset   28  /* 0x0000001c */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_4__concat_dpitchm1_byte_offset    32  /* 0x00000020 */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_4__concat_dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_4__concat_vwidth_minus_one_byte_offset    16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_4__concat_vwidth_minus_one_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_4__concat_vheight_minus_one_byte_offset   18  /* 0x00000012 */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_4__concat_vheight_minus_one_bsize     16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_4__concat_drotate_bit_offset  198 /* 0x000000c6 */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_4__concat_hflip_bit_offset    199 /* 0x000000c7 */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_4__concat_vflip_bit_offset    200 /* 0x000000c8 */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_4__concat_dflip_bit_offset    201 /* 0x000000c9 */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_4__concat_pflip_bit_offset    202 /* 0x000000ca */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_4__concat_W   80
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_4__concat_H   32
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_4__concat_D   416
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_4__concat_P   1
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_4__concat_data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_4__concat_dpitch_num_bytes    32
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_4__concat_dram_format     3
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_4__concat_vp_interleave_mode  2
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_4__concat_tile_width  4
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_4__concat_tile_height     1
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_4__concat_vmem_buffer_num_bytes   332800
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_4__concat_dram_size_num_bytes     1064960
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_4__concat_sign    0
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_4__concat_datasize    0
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_4__concat_expoffset   5
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_4__concat_expbits     0
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_4__concat_drotate     0
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_4__concat_hflip   0
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_4__concat_vflip   0
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_4__concat_dflip   0
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_4__concat_pflip   0

/* bisenet_pelee_picinfo__prim_split_4 SMB_input __pvcn_639_ */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_639__cnngen_demangled_name   "__pvcn_639_"
#define bisenet_pelee_picinfo__prim_split_4___pvcn_639__is_constant     1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_639__byte_offset     40  /* 0x00000028 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_639__vbase_byte_offset   44  /* 0x0000002c */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_639__dbase_byte_offset   68  /* 0x00000044 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_639__dpitchm1_byte_offset    72  /* 0x00000048 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_639__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_639__W   33444
#define bisenet_pelee_picinfo__prim_split_4___pvcn_639__H   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_639__D   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_639__P   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_639__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_639__dpitch_num_bytes    33472
#define bisenet_pelee_picinfo__prim_split_4___pvcn_639__dram_format     0
#define bisenet_pelee_picinfo__prim_split_4___pvcn_639__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_4___pvcn_639__tile_width  1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_639__tile_height     1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_639__vmem_buffer_num_bytes   33444

/* bisenet_pelee_picinfo__prim_split_4 SMB_input __pvcn_640_ */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_640__cnngen_demangled_name   "__pvcn_640_"
#define bisenet_pelee_picinfo__prim_split_4___pvcn_640__is_constant     1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_640__byte_offset     76  /* 0x0000004c */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_640__vbase_byte_offset   80  /* 0x00000050 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_640__dbase_byte_offset   104 /* 0x00000068 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_640__dpitchm1_byte_offset    108 /* 0x0000006c */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_640__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_640__W   640
#define bisenet_pelee_picinfo__prim_split_4___pvcn_640__H   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_640__D   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_640__P   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_640__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_640__dpitch_num_bytes    640
#define bisenet_pelee_picinfo__prim_split_4___pvcn_640__dram_format     0
#define bisenet_pelee_picinfo__prim_split_4___pvcn_640__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_4___pvcn_640__tile_width  1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_640__tile_height     1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_640__vmem_buffer_num_bytes   640

/* bisenet_pelee_picinfo__prim_split_4 SMB_input __pvcn_643_ */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_643__cnngen_demangled_name   "__pvcn_643_"
#define bisenet_pelee_picinfo__prim_split_4___pvcn_643__is_constant     1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_643__byte_offset     116 /* 0x00000074 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_643__vbase_byte_offset   120 /* 0x00000078 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_643__dbase_byte_offset   144 /* 0x00000090 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_643__dpitchm1_byte_offset    148 /* 0x00000094 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_643__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_643__W   5756
#define bisenet_pelee_picinfo__prim_split_4___pvcn_643__H   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_643__D   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_643__P   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_643__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_643__dpitch_num_bytes    5760
#define bisenet_pelee_picinfo__prim_split_4___pvcn_643__dram_format     0
#define bisenet_pelee_picinfo__prim_split_4___pvcn_643__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_4___pvcn_643__tile_width  1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_643__tile_height     1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_643__vmem_buffer_num_bytes   5756

/* bisenet_pelee_picinfo__prim_split_4 SMB_input __pvcn_644_ */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_644__cnngen_demangled_name   "__pvcn_644_"
#define bisenet_pelee_picinfo__prim_split_4___pvcn_644__is_constant     1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_644__byte_offset     156 /* 0x0000009c */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_644__vbase_byte_offset   160 /* 0x000000a0 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_644__dbase_byte_offset   184 /* 0x000000b8 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_644__dpitchm1_byte_offset    188 /* 0x000000bc */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_644__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_644__W   88
#define bisenet_pelee_picinfo__prim_split_4___pvcn_644__H   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_644__D   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_644__P   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_644__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_644__dpitch_num_bytes    96
#define bisenet_pelee_picinfo__prim_split_4___pvcn_644__dram_format     0
#define bisenet_pelee_picinfo__prim_split_4___pvcn_644__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_4___pvcn_644__tile_width  1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_644__tile_height     1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_644__vmem_buffer_num_bytes   88

/* bisenet_pelee_picinfo__prim_split_4 SMB_input __pvcn_647_ */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_647__cnngen_demangled_name   "__pvcn_647_"
#define bisenet_pelee_picinfo__prim_split_4___pvcn_647__is_constant     1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_647__byte_offset     196 /* 0x000000c4 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_647__vbase_byte_offset   200 /* 0x000000c8 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_647__dbase_byte_offset   224 /* 0x000000e0 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_647__dpitchm1_byte_offset    228 /* 0x000000e4 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_647__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_647__W   5732
#define bisenet_pelee_picinfo__prim_split_4___pvcn_647__H   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_647__D   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_647__P   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_647__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_647__dpitch_num_bytes    5760
#define bisenet_pelee_picinfo__prim_split_4___pvcn_647__dram_format     0
#define bisenet_pelee_picinfo__prim_split_4___pvcn_647__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_4___pvcn_647__tile_width  1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_647__tile_height     1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_647__vmem_buffer_num_bytes   5732

/* bisenet_pelee_picinfo__prim_split_4 SMB_input __pvcn_648_ */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_648__cnngen_demangled_name   "__pvcn_648_"
#define bisenet_pelee_picinfo__prim_split_4___pvcn_648__is_constant     1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_648__byte_offset     236 /* 0x000000ec */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_648__vbase_byte_offset   240 /* 0x000000f0 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_648__dbase_byte_offset   264 /* 0x00000108 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_648__dpitchm1_byte_offset    268 /* 0x0000010c */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_648__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_648__W   88
#define bisenet_pelee_picinfo__prim_split_4___pvcn_648__H   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_648__D   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_648__P   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_648__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_648__dpitch_num_bytes    96
#define bisenet_pelee_picinfo__prim_split_4___pvcn_648__dram_format     0
#define bisenet_pelee_picinfo__prim_split_4___pvcn_648__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_4___pvcn_648__tile_width  1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_648__tile_height     1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_648__vmem_buffer_num_bytes   88

/* bisenet_pelee_picinfo__prim_split_4 SMB_input __pvcn_650_ */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_650__cnngen_demangled_name   "__pvcn_650_"
#define bisenet_pelee_picinfo__prim_split_4___pvcn_650__is_constant     1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_650__byte_offset     276 /* 0x00000114 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_650__vbase_byte_offset   280 /* 0x00000118 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_650__dbase_byte_offset   304 /* 0x00000130 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_650__dpitchm1_byte_offset    308 /* 0x00000134 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_650__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_650__W   1420
#define bisenet_pelee_picinfo__prim_split_4___pvcn_650__H   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_650__D   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_650__P   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_650__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_650__dpitch_num_bytes    1440
#define bisenet_pelee_picinfo__prim_split_4___pvcn_650__dram_format     0
#define bisenet_pelee_picinfo__prim_split_4___pvcn_650__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_4___pvcn_650__tile_width  1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_650__tile_height     1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_650__vmem_buffer_num_bytes   1420

/* bisenet_pelee_picinfo__prim_split_4 SMB_input __pvcn_651_ */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_651__cnngen_demangled_name   "__pvcn_651_"
#define bisenet_pelee_picinfo__prim_split_4___pvcn_651__is_constant     1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_651__byte_offset     316 /* 0x0000013c */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_651__vbase_byte_offset   320 /* 0x00000140 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_651__dbase_byte_offset   344 /* 0x00000158 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_651__dpitchm1_byte_offset    348 /* 0x0000015c */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_651__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_651__W   88
#define bisenet_pelee_picinfo__prim_split_4___pvcn_651__H   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_651__D   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_651__P   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_651__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_651__dpitch_num_bytes    96
#define bisenet_pelee_picinfo__prim_split_4___pvcn_651__dram_format     0
#define bisenet_pelee_picinfo__prim_split_4___pvcn_651__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_4___pvcn_651__tile_width  1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_651__tile_height     1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_651__vmem_buffer_num_bytes   88

/* bisenet_pelee_picinfo__prim_split_4 SMB_input __pvcn_657_ */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_657__cnngen_demangled_name   "__pvcn_657_"
#define bisenet_pelee_picinfo__prim_split_4___pvcn_657__is_constant     1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_657__byte_offset     356 /* 0x00000164 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_657__vbase_byte_offset   360 /* 0x00000168 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_657__dbase_byte_offset   384 /* 0x00000180 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_657__dpitchm1_byte_offset    388 /* 0x00000184 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_657__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_657__W   35508
#define bisenet_pelee_picinfo__prim_split_4___pvcn_657__H   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_657__D   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_657__P   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_657__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_657__dpitch_num_bytes    35520
#define bisenet_pelee_picinfo__prim_split_4___pvcn_657__dram_format     0
#define bisenet_pelee_picinfo__prim_split_4___pvcn_657__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_4___pvcn_657__tile_width  1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_657__tile_height     1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_657__vmem_buffer_num_bytes   35508

/* bisenet_pelee_picinfo__prim_split_4 SMB_input __pvcn_658_ */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_658__cnngen_demangled_name   "__pvcn_658_"
#define bisenet_pelee_picinfo__prim_split_4___pvcn_658__is_constant     1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_658__byte_offset     396 /* 0x0000018c */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_658__vbase_byte_offset   400 /* 0x00000190 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_658__dbase_byte_offset   424 /* 0x000001a8 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_658__dpitchm1_byte_offset    428 /* 0x000001ac */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_658__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_658__W   640
#define bisenet_pelee_picinfo__prim_split_4___pvcn_658__H   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_658__D   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_658__P   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_658__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_658__dpitch_num_bytes    640
#define bisenet_pelee_picinfo__prim_split_4___pvcn_658__dram_format     0
#define bisenet_pelee_picinfo__prim_split_4___pvcn_658__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_4___pvcn_658__tile_width  1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_658__tile_height     1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_658__vmem_buffer_num_bytes   640

/* bisenet_pelee_picinfo__prim_split_4 SMB_input __pvcn_661_ */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_661__cnngen_demangled_name   "__pvcn_661_"
#define bisenet_pelee_picinfo__prim_split_4___pvcn_661__is_constant     1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_661__byte_offset     436 /* 0x000001b4 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_661__vbase_byte_offset   440 /* 0x000001b8 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_661__dbase_byte_offset   464 /* 0x000001d0 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_661__dpitchm1_byte_offset    468 /* 0x000001d4 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_661__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_661__W   5772
#define bisenet_pelee_picinfo__prim_split_4___pvcn_661__H   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_661__D   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_661__P   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_661__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_661__dpitch_num_bytes    5792
#define bisenet_pelee_picinfo__prim_split_4___pvcn_661__dram_format     0
#define bisenet_pelee_picinfo__prim_split_4___pvcn_661__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_4___pvcn_661__tile_width  1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_661__tile_height     1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_661__vmem_buffer_num_bytes   5772

/* bisenet_pelee_picinfo__prim_split_4 SMB_input __pvcn_662_ */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_662__cnngen_demangled_name   "__pvcn_662_"
#define bisenet_pelee_picinfo__prim_split_4___pvcn_662__is_constant     1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_662__byte_offset     476 /* 0x000001dc */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_662__vbase_byte_offset   480 /* 0x000001e0 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_662__dbase_byte_offset   504 /* 0x000001f8 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_662__dpitchm1_byte_offset    508 /* 0x000001fc */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_662__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_662__W   88
#define bisenet_pelee_picinfo__prim_split_4___pvcn_662__H   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_662__D   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_662__P   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_662__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_662__dpitch_num_bytes    96
#define bisenet_pelee_picinfo__prim_split_4___pvcn_662__dram_format     0
#define bisenet_pelee_picinfo__prim_split_4___pvcn_662__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_4___pvcn_662__tile_width  1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_662__tile_height     1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_662__vmem_buffer_num_bytes   88

/* bisenet_pelee_picinfo__prim_split_4 SMB_input __pvcn_665_ */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_665__cnngen_demangled_name   "__pvcn_665_"
#define bisenet_pelee_picinfo__prim_split_4___pvcn_665__is_constant     1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_665__byte_offset     516 /* 0x00000204 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_665__vbase_byte_offset   520 /* 0x00000208 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_665__dbase_byte_offset   544 /* 0x00000220 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_665__dpitchm1_byte_offset    548 /* 0x00000224 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_665__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_665__W   5704
#define bisenet_pelee_picinfo__prim_split_4___pvcn_665__H   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_665__D   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_665__P   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_665__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_665__dpitch_num_bytes    5728
#define bisenet_pelee_picinfo__prim_split_4___pvcn_665__dram_format     0
#define bisenet_pelee_picinfo__prim_split_4___pvcn_665__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_4___pvcn_665__tile_width  1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_665__tile_height     1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_665__vmem_buffer_num_bytes   5704

/* bisenet_pelee_picinfo__prim_split_4 SMB_input __pvcn_666_ */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_666__cnngen_demangled_name   "__pvcn_666_"
#define bisenet_pelee_picinfo__prim_split_4___pvcn_666__is_constant     1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_666__byte_offset     556 /* 0x0000022c */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_666__vbase_byte_offset   560 /* 0x00000230 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_666__dbase_byte_offset   584 /* 0x00000248 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_666__dpitchm1_byte_offset    588 /* 0x0000024c */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_666__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_666__W   80
#define bisenet_pelee_picinfo__prim_split_4___pvcn_666__H   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_666__D   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_666__P   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_666__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_666__dpitch_num_bytes    96
#define bisenet_pelee_picinfo__prim_split_4___pvcn_666__dram_format     0
#define bisenet_pelee_picinfo__prim_split_4___pvcn_666__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_4___pvcn_666__tile_width  1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_666__tile_height     1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_666__vmem_buffer_num_bytes   80

/* bisenet_pelee_picinfo__prim_split_4 SMB_input __pvcn_668_ */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_668__cnngen_demangled_name   "__pvcn_668_"
#define bisenet_pelee_picinfo__prim_split_4___pvcn_668__is_constant     1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_668__byte_offset     596 /* 0x00000254 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_668__vbase_byte_offset   600 /* 0x00000258 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_668__dbase_byte_offset   624 /* 0x00000270 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_668__dpitchm1_byte_offset    628 /* 0x00000274 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_668__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_668__W   1376
#define bisenet_pelee_picinfo__prim_split_4___pvcn_668__H   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_668__D   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_668__P   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_668__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_668__dpitch_num_bytes    1376
#define bisenet_pelee_picinfo__prim_split_4___pvcn_668__dram_format     0
#define bisenet_pelee_picinfo__prim_split_4___pvcn_668__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_4___pvcn_668__tile_width  1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_668__tile_height     1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_668__vmem_buffer_num_bytes   1376

/* bisenet_pelee_picinfo__prim_split_4 SMB_input __pvcn_669_ */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_669__cnngen_demangled_name   "__pvcn_669_"
#define bisenet_pelee_picinfo__prim_split_4___pvcn_669__is_constant     1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_669__byte_offset     636 /* 0x0000027c */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_669__vbase_byte_offset   640 /* 0x00000280 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_669__dbase_byte_offset   664 /* 0x00000298 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_669__dpitchm1_byte_offset    668 /* 0x0000029c */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_669__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_669__W   80
#define bisenet_pelee_picinfo__prim_split_4___pvcn_669__H   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_669__D   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_669__P   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_669__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_669__dpitch_num_bytes    96
#define bisenet_pelee_picinfo__prim_split_4___pvcn_669__dram_format     0
#define bisenet_pelee_picinfo__prim_split_4___pvcn_669__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_4___pvcn_669__tile_width  1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_669__tile_height     1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_669__vmem_buffer_num_bytes   80

/* bisenet_pelee_picinfo__prim_split_4 SMB_input __pvcn_675_ */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_675__cnngen_demangled_name   "__pvcn_675_"
#define bisenet_pelee_picinfo__prim_split_4___pvcn_675__is_constant     1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_675__byte_offset     676 /* 0x000002a4 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_675__vbase_byte_offset   680 /* 0x000002a8 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_675__dbase_byte_offset   704 /* 0x000002c0 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_675__dpitchm1_byte_offset    708 /* 0x000002c4 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_675__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_675__W   37900
#define bisenet_pelee_picinfo__prim_split_4___pvcn_675__H   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_675__D   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_675__P   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_675__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_675__dpitch_num_bytes    37920
#define bisenet_pelee_picinfo__prim_split_4___pvcn_675__dram_format     0
#define bisenet_pelee_picinfo__prim_split_4___pvcn_675__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_4___pvcn_675__tile_width  1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_675__tile_height     1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_675__vmem_buffer_num_bytes   37900

/* bisenet_pelee_picinfo__prim_split_4 SMB_input __pvcn_676_ */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_676__cnngen_demangled_name   "__pvcn_676_"
#define bisenet_pelee_picinfo__prim_split_4___pvcn_676__is_constant     1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_676__byte_offset     716 /* 0x000002cc */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_676__vbase_byte_offset   720 /* 0x000002d0 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_676__dbase_byte_offset   744 /* 0x000002e8 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_676__dpitchm1_byte_offset    748 /* 0x000002ec */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_676__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_676__W   640
#define bisenet_pelee_picinfo__prim_split_4___pvcn_676__H   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_676__D   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_676__P   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_676__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_676__dpitch_num_bytes    640
#define bisenet_pelee_picinfo__prim_split_4___pvcn_676__dram_format     0
#define bisenet_pelee_picinfo__prim_split_4___pvcn_676__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_4___pvcn_676__tile_width  1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_676__tile_height     1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_676__vmem_buffer_num_bytes   640

/* bisenet_pelee_picinfo__prim_split_4 SMB_input __pvcn_679_ */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_679__cnngen_demangled_name   "__pvcn_679_"
#define bisenet_pelee_picinfo__prim_split_4___pvcn_679__is_constant     1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_679__byte_offset     756 /* 0x000002f4 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_679__vbase_byte_offset   760 /* 0x000002f8 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_679__dbase_byte_offset   784 /* 0x00000310 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_679__dpitchm1_byte_offset    788 /* 0x00000314 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_679__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_679__W   5680
#define bisenet_pelee_picinfo__prim_split_4___pvcn_679__H   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_679__D   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_679__P   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_679__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_679__dpitch_num_bytes    5696
#define bisenet_pelee_picinfo__prim_split_4___pvcn_679__dram_format     0
#define bisenet_pelee_picinfo__prim_split_4___pvcn_679__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_4___pvcn_679__tile_width  1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_679__tile_height     1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_679__vmem_buffer_num_bytes   5680

/* bisenet_pelee_picinfo__prim_split_4 SMB_input __pvcn_680_ */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_680__cnngen_demangled_name   "__pvcn_680_"
#define bisenet_pelee_picinfo__prim_split_4___pvcn_680__is_constant     1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_680__byte_offset     796 /* 0x0000031c */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_680__vbase_byte_offset   800 /* 0x00000320 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_680__dbase_byte_offset   824 /* 0x00000338 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_680__dpitchm1_byte_offset    828 /* 0x0000033c */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_680__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_680__W   88
#define bisenet_pelee_picinfo__prim_split_4___pvcn_680__H   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_680__D   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_680__P   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_680__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_680__dpitch_num_bytes    96
#define bisenet_pelee_picinfo__prim_split_4___pvcn_680__dram_format     0
#define bisenet_pelee_picinfo__prim_split_4___pvcn_680__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_4___pvcn_680__tile_width  1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_680__tile_height     1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_680__vmem_buffer_num_bytes   88

/* bisenet_pelee_picinfo__prim_split_4 SMB_input __pvcn_683_ */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_683__cnngen_demangled_name   "__pvcn_683_"
#define bisenet_pelee_picinfo__prim_split_4___pvcn_683__is_constant     1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_683__byte_offset     836 /* 0x00000344 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_683__vbase_byte_offset   840 /* 0x00000348 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_683__dbase_byte_offset   864 /* 0x00000360 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_683__dpitchm1_byte_offset    868 /* 0x00000364 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_683__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_683__W   5716
#define bisenet_pelee_picinfo__prim_split_4___pvcn_683__H   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_683__D   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_683__P   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_683__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_683__dpitch_num_bytes    5728
#define bisenet_pelee_picinfo__prim_split_4___pvcn_683__dram_format     0
#define bisenet_pelee_picinfo__prim_split_4___pvcn_683__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_4___pvcn_683__tile_width  1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_683__tile_height     1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_683__vmem_buffer_num_bytes   5716

/* bisenet_pelee_picinfo__prim_split_4 SMB_input __pvcn_684_ */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_684__cnngen_demangled_name   "__pvcn_684_"
#define bisenet_pelee_picinfo__prim_split_4___pvcn_684__is_constant     1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_684__byte_offset     876 /* 0x0000036c */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_684__vbase_byte_offset   880 /* 0x00000370 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_684__dbase_byte_offset   904 /* 0x00000388 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_684__dpitchm1_byte_offset    908 /* 0x0000038c */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_684__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_684__W   88
#define bisenet_pelee_picinfo__prim_split_4___pvcn_684__H   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_684__D   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_684__P   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_684__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_684__dpitch_num_bytes    96
#define bisenet_pelee_picinfo__prim_split_4___pvcn_684__dram_format     0
#define bisenet_pelee_picinfo__prim_split_4___pvcn_684__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_4___pvcn_684__tile_width  1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_684__tile_height     1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_684__vmem_buffer_num_bytes   88

/* bisenet_pelee_picinfo__prim_split_4 SMB_input __pvcn_686_ */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_686__cnngen_demangled_name   "__pvcn_686_"
#define bisenet_pelee_picinfo__prim_split_4___pvcn_686__is_constant     1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_686__byte_offset     916 /* 0x00000394 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_686__vbase_byte_offset   920 /* 0x00000398 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_686__dbase_byte_offset   944 /* 0x000003b0 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_686__dpitchm1_byte_offset    948 /* 0x000003b4 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_686__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_686__W   1436
#define bisenet_pelee_picinfo__prim_split_4___pvcn_686__H   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_686__D   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_686__P   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_686__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_686__dpitch_num_bytes    1440
#define bisenet_pelee_picinfo__prim_split_4___pvcn_686__dram_format     0
#define bisenet_pelee_picinfo__prim_split_4___pvcn_686__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_4___pvcn_686__tile_width  1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_686__tile_height     1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_686__vmem_buffer_num_bytes   1436

/* bisenet_pelee_picinfo__prim_split_4 SMB_input __pvcn_687_ */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_687__cnngen_demangled_name   "__pvcn_687_"
#define bisenet_pelee_picinfo__prim_split_4___pvcn_687__is_constant     1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_687__byte_offset     956 /* 0x000003bc */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_687__vbase_byte_offset   960 /* 0x000003c0 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_687__dbase_byte_offset   984 /* 0x000003d8 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_687__dpitchm1_byte_offset    988 /* 0x000003dc */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_687__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_4___pvcn_687__W   80
#define bisenet_pelee_picinfo__prim_split_4___pvcn_687__H   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_687__D   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_687__P   1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_687__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_687__dpitch_num_bytes    96
#define bisenet_pelee_picinfo__prim_split_4___pvcn_687__dram_format     0
#define bisenet_pelee_picinfo__prim_split_4___pvcn_687__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_4___pvcn_687__tile_width  1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_687__tile_height     1
#define bisenet_pelee_picinfo__prim_split_4___pvcn_687__vmem_buffer_num_bytes   80

/* bisenet_pelee_picinfo__prim_split_4 VCB PeleeNet__stage_3_dense_block_5__concat */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_5__concat_cnngen_demangled_name   "PeleeNet__stage_3_dense_block_5__concat"
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_5__concat_byte_offset     996 /* 0x000003e4 */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_5__concat_vbase_byte_offset   1004    /* 0x000003ec */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_5__concat_vmem_buffer_num_bytes   293888

/* bisenet_pelee_picinfo__prim_split_4 VCB PeleeNet__stage_3_dense_block_6__concat */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_6__concat_cnngen_demangled_name   "PeleeNet__stage_3_dense_block_6__concat"
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_6__concat_byte_offset     1032    /* 0x00000408 */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_6__concat_vbase_byte_offset   1040    /* 0x00000410 */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_6__concat_vmem_buffer_num_bytes   384000

/* bisenet_pelee_picinfo__prim_split_4 HMB_output PeleeNet__stage_3_dense_block_7__concat */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_7__concat_cnngen_demangled_name   "PeleeNet__stage_3_dense_block_7__concat"
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_7__concat_byte_offset     1068    /* 0x0000042c */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_7__concat_vbase_byte_offset   1072    /* 0x00000430 */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_7__concat_dbase_byte_offset   1096    /* 0x00000448 */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_7__concat_dpitchm1_byte_offset    1100    /* 0x0000044c */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_7__concat_dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_7__concat_vwidth_minus_one_byte_offset    1084    /* 0x0000043c */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_7__concat_vwidth_minus_one_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_7__concat_vheight_minus_one_byte_offset   1086    /* 0x0000043e */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_7__concat_vheight_minus_one_bsize     16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_7__concat_drotate_bit_offset  8742    /* 0x00002226 */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_7__concat_hflip_bit_offset    8743    /* 0x00002227 */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_7__concat_vflip_bit_offset    8744    /* 0x00002228 */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_7__concat_dflip_bit_offset    8745    /* 0x00002229 */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_7__concat_pflip_bit_offset    8746    /* 0x0000222a */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_7__concat_W   80
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_7__concat_H   32
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_7__concat_D   512
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_7__concat_P   1
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_7__concat_data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_7__concat_dpitch_num_bytes    32
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_7__concat_dram_format     3
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_7__concat_vp_interleave_mode  2
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_7__concat_tile_width  4
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_7__concat_tile_height     1
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_7__concat_vmem_buffer_num_bytes   172032
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_7__concat_dram_size_num_bytes     1310720
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_7__concat_sign    0
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_7__concat_datasize    0
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_7__concat_expoffset   5
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_7__concat_expbits     0
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_7__concat_drotate     0
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_7__concat_hflip   0
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_7__concat_vflip   0
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_7__concat_dflip   0
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_7__concat_pflip   0

/* bisenet_pelee_picinfo__prim_split_4 VCB PeleeNet__stage_3_dense_block_5__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_5__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_cnngen_demangled_name    "PeleeNet__stage_3_dense_block_5__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm"
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_5__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_byte_offset  1120    /* 0x00000460 */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_5__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_vbase_byte_offset    1128    /* 0x00000468 */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_5__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_vmem_buffer_num_bytes    124928

/* bisenet_pelee_picinfo__prim_split_4 VCB PeleeNet__stage_3_dense_block_5__conv_left_0__Relu */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_5__conv_left_0__Relu_cnngen_demangled_name    "PeleeNet__stage_3_dense_block_5__conv_left_0__Relu"
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_5__conv_left_0__Relu_byte_offset  1172    /* 0x00000494 */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_5__conv_left_0__Relu_vbase_byte_offset    1180    /* 0x0000049c */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_5__conv_left_0__Relu_vmem_buffer_num_bytes    43264

/* bisenet_pelee_picinfo__prim_split_4 VCB PeleeNet__stage_3_dense_block_5__conv_right_0__Relu */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_5__conv_right_0__Relu_cnngen_demangled_name   "PeleeNet__stage_3_dense_block_5__conv_right_0__Relu"
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_5__conv_right_0__Relu_byte_offset     1244    /* 0x000004dc */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_5__conv_right_0__Relu_vbase_byte_offset   1252    /* 0x000004e4 */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_5__conv_right_0__Relu_vmem_buffer_num_bytes   43264

/* bisenet_pelee_picinfo__prim_split_4 VCB PeleeNet__stage_3_dense_block_5__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_5__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_cnngen_demangled_name  "PeleeNet__stage_3_dense_block_5__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu"
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_5__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_byte_offset    1292    /* 0x0000050c */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_5__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vbase_byte_offset  1300    /* 0x00000514 */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_5__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vmem_buffer_num_bytes  28032

/* bisenet_pelee_picinfo__prim_split_4 VCB PeleeNet__stage_3_dense_block_6__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_6__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_cnngen_demangled_name    "PeleeNet__stage_3_dense_block_6__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm"
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_6__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_byte_offset  1404    /* 0x0000057c */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_6__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_vbase_byte_offset    1412    /* 0x00000584 */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_6__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_vmem_buffer_num_bytes    165888

/* bisenet_pelee_picinfo__prim_split_4 VCB PeleeNet__stage_3_dense_block_6__conv_left_0__Relu */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_6__conv_left_0__Relu_cnngen_demangled_name    "PeleeNet__stage_3_dense_block_6__conv_left_0__Relu"
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_6__conv_left_0__Relu_byte_offset  1456    /* 0x000005b0 */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_6__conv_left_0__Relu_vbase_byte_offset    1464    /* 0x000005b8 */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_6__conv_left_0__Relu_vmem_buffer_num_bytes    43264

/* bisenet_pelee_picinfo__prim_split_4 VCB PeleeNet__stage_3_dense_block_6__conv_right_0__Relu */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_6__conv_right_0__Relu_cnngen_demangled_name   "PeleeNet__stage_3_dense_block_6__conv_right_0__Relu"
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_6__conv_right_0__Relu_byte_offset     1528    /* 0x000005f8 */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_6__conv_right_0__Relu_vbase_byte_offset   1536    /* 0x00000600 */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_6__conv_right_0__Relu_vmem_buffer_num_bytes   28160

/* bisenet_pelee_picinfo__prim_split_4 VCB PeleeNet__stage_3_dense_block_6__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_6__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_cnngen_demangled_name  "PeleeNet__stage_3_dense_block_6__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu"
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_6__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_byte_offset    1576    /* 0x00000628 */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_6__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vbase_byte_offset  1584    /* 0x00000630 */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_6__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vmem_buffer_num_bytes  28160

/* bisenet_pelee_picinfo__prim_split_4 VCB PeleeNet__stage_3_dense_block_7__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_7__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_cnngen_demangled_name    "PeleeNet__stage_3_dense_block_7__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm"
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_7__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_byte_offset  1668    /* 0x00000684 */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_7__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_vbase_byte_offset    1676    /* 0x0000068c */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_7__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_vmem_buffer_num_bytes    124928

/* bisenet_pelee_picinfo__prim_split_4 VCB PeleeNet__stage_3_dense_block_7__conv_left_0__Relu */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_7__conv_left_0__Relu_cnngen_demangled_name    "PeleeNet__stage_3_dense_block_7__conv_left_0__Relu"
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_7__conv_left_0__Relu_byte_offset  1720    /* 0x000006b8 */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_7__conv_left_0__Relu_vbase_byte_offset    1728    /* 0x000006c0 */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_7__conv_left_0__Relu_vmem_buffer_num_bytes    43264

/* bisenet_pelee_picinfo__prim_split_4 VCB PeleeNet__stage_3_dense_block_7__conv_right_0__Relu */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_7__conv_right_0__Relu_cnngen_demangled_name   "PeleeNet__stage_3_dense_block_7__conv_right_0__Relu"
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_7__conv_right_0__Relu_byte_offset     1792    /* 0x00000700 */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_7__conv_right_0__Relu_vbase_byte_offset   1800    /* 0x00000708 */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_7__conv_right_0__Relu_vmem_buffer_num_bytes   43264

/* bisenet_pelee_picinfo__prim_split_4 VCB PeleeNet__stage_3_dense_block_7__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_7__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_cnngen_demangled_name  "PeleeNet__stage_3_dense_block_7__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu"
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_7__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_byte_offset    1840    /* 0x00000730 */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_7__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vbase_byte_offset  1848    /* 0x00000738 */
#define bisenet_pelee_picinfo__prim_split_4_PeleeNet__stage_3_dense_block_7__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vmem_buffer_num_bytes  21504


#endif /* bisenet_pelee_picinfo__prim_split_4_H */
