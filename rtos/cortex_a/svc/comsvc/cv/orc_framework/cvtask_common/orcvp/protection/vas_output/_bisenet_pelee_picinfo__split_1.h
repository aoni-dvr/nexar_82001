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
#ifndef bisenet_pelee_picinfo__prim_split_1_H
#define bisenet_pelee_picinfo__prim_split_1_H

#define vas_gen_header_version                      5

/* dag name */
#define bisenet_pelee_picinfo__prim_split_1_DAG_NAME    "bisenet_pelee_picinfo__prim_split_1"

/* VDG file info */
#define bisenet_pelee_picinfo__prim_split_1_vdg_name    "bisenet_pelee_picinfo__split_1.vdg"

/* VDG DAG memory info */
#define bisenet_pelee_picinfo__prim_split_1_byte_offset     340 /* 0x00000154 */
#define bisenet_pelee_picinfo__prim_split_1_byte_size   3164    /* 0x00000c5c */
#define bisenet_pelee_picinfo__prim_split_1_preferred_dram_xfer_size    1

/* bisenet_pelee_picinfo__prim_split_1 VMEM info */
#define bisenet_pelee_picinfo__prim_split_1_use_ping_pong_vmem  0
#define bisenet_pelee_picinfo__prim_split_1_VMEM_start  4096    /* 0x00001000 */
#define bisenet_pelee_picinfo__prim_split_1_VMEM_end    2070340 /* 0x001f9744 */
#define bisenet_pelee_picinfo__prim_split_1_image_start     1967872 /* 0x001e0700 */
#define bisenet_pelee_picinfo__prim_split_1_image_size  105632  /* 0x00019ca0 */
#define bisenet_pelee_picinfo__prim_split_1_dagbin_start    2070340 /* 0x001f9744 */

/*******************************************************************************
 * DAG byte offset info for HMB/SMB/VCB and their fields
 * in DAG 'bisenet_pelee_picinfo__prim_split_1' in source file 'bisenet_pelee_picinfo__split_1.vas'
 ******************************************************************************/
/* bisenet_pelee_picinfo__prim_split_1 HMB_input PeleeNet__stage_2_dense_block_0__concat */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_0__concat_cnngen_demangled_name   "PeleeNet__stage_2_dense_block_0__concat"
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_0__concat_is_constant     0
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_0__concat_ddi_byte_size   0   /* 0x00000000 */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_0__concat_ddi_byte_offset     4294967295  /* 0xffffffff */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_0__concat_byte_offset     0   /* 0x00000000 */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_0__concat_vbase_byte_offset   4   /* 0x00000004 */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_0__concat_dbase_byte_offset   28  /* 0x0000001c */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_0__concat_dpitchm1_byte_offset    32  /* 0x00000020 */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_0__concat_dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_0__concat_vwidth_minus_one_byte_offset    16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_0__concat_vwidth_minus_one_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_0__concat_vheight_minus_one_byte_offset   18  /* 0x00000012 */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_0__concat_vheight_minus_one_bsize     16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_0__concat_drotate_bit_offset  198 /* 0x000000c6 */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_0__concat_hflip_bit_offset    199 /* 0x000000c7 */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_0__concat_vflip_bit_offset    200 /* 0x000000c8 */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_0__concat_dflip_bit_offset    201 /* 0x000000c9 */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_0__concat_pflip_bit_offset    202 /* 0x000000ca */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_0__concat_W   160
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_0__concat_H   64
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_0__concat_D   160
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_0__concat_P   1
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_0__concat_data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_0__concat_dpitch_num_bytes    32
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_0__concat_dram_format     3
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_0__concat_vp_interleave_mode  2
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_0__concat_tile_width  4
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_0__concat_tile_height     1
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_0__concat_vmem_buffer_num_bytes   309760
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_0__concat_dram_size_num_bytes     1638400
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_0__concat_sign    0
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_0__concat_datasize    0
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_0__concat_expoffset   5
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_0__concat_expbits     0
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_0__concat_drotate     0
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_0__concat_hflip   0
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_0__concat_vflip   0
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_0__concat_dflip   0
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_0__concat_pflip   0

/* bisenet_pelee_picinfo__prim_split_1 SMB_input __pvcn_428_ */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_428__cnngen_demangled_name   "__pvcn_428_"
#define bisenet_pelee_picinfo__prim_split_1___pvcn_428__is_constant     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_428__byte_offset     40  /* 0x00000028 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_428__vbase_byte_offset   44  /* 0x0000002c */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_428__dbase_byte_offset   68  /* 0x00000044 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_428__dpitchm1_byte_offset    72  /* 0x00000048 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_428__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_428__W   7316
#define bisenet_pelee_picinfo__prim_split_1___pvcn_428__H   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_428__D   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_428__P   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_428__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_428__dpitch_num_bytes    7328
#define bisenet_pelee_picinfo__prim_split_1___pvcn_428__dram_format     0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_428__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_428__tile_width  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_428__tile_height     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_428__vmem_buffer_num_bytes   7316

/* bisenet_pelee_picinfo__prim_split_1 SMB_input __pvcn_429_ */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_429__cnngen_demangled_name   "__pvcn_429_"
#define bisenet_pelee_picinfo__prim_split_1___pvcn_429__is_constant     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_429__byte_offset     76  /* 0x0000004c */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_429__vbase_byte_offset   80  /* 0x00000050 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_429__dbase_byte_offset   104 /* 0x00000068 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_429__dpitchm1_byte_offset    108 /* 0x0000006c */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_429__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_429__W   320
#define bisenet_pelee_picinfo__prim_split_1___pvcn_429__H   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_429__D   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_429__P   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_429__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_429__dpitch_num_bytes    320
#define bisenet_pelee_picinfo__prim_split_1___pvcn_429__dram_format     0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_429__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_429__tile_width  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_429__tile_height     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_429__vmem_buffer_num_bytes   320

/* bisenet_pelee_picinfo__prim_split_1 SMB_input __pvcn_432_ */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_432__cnngen_demangled_name   "__pvcn_432_"
#define bisenet_pelee_picinfo__prim_split_1___pvcn_432__is_constant     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_432__byte_offset     116 /* 0x00000074 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_432__vbase_byte_offset   120 /* 0x00000078 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_432__dbase_byte_offset   144 /* 0x00000090 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_432__dpitchm1_byte_offset    148 /* 0x00000094 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_432__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_432__W   3252
#define bisenet_pelee_picinfo__prim_split_1___pvcn_432__H   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_432__D   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_432__P   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_432__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_432__dpitch_num_bytes    3264
#define bisenet_pelee_picinfo__prim_split_1___pvcn_432__dram_format     0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_432__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_432__tile_width  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_432__tile_height     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_432__vmem_buffer_num_bytes   3252

/* bisenet_pelee_picinfo__prim_split_1 SMB_input __pvcn_433_ */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_433__cnngen_demangled_name   "__pvcn_433_"
#define bisenet_pelee_picinfo__prim_split_1___pvcn_433__is_constant     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_433__byte_offset     156 /* 0x0000009c */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_433__vbase_byte_offset   160 /* 0x000000a0 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_433__dbase_byte_offset   184 /* 0x000000b8 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_433__dpitchm1_byte_offset    188 /* 0x000000bc */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_433__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_433__W   88
#define bisenet_pelee_picinfo__prim_split_1___pvcn_433__H   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_433__D   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_433__P   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_433__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_433__dpitch_num_bytes    96
#define bisenet_pelee_picinfo__prim_split_1___pvcn_433__dram_format     0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_433__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_433__tile_width  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_433__tile_height     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_433__vmem_buffer_num_bytes   88

/* bisenet_pelee_picinfo__prim_split_1 SMB_input __pvcn_436_ */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_436__cnngen_demangled_name   "__pvcn_436_"
#define bisenet_pelee_picinfo__prim_split_1___pvcn_436__is_constant     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_436__byte_offset     196 /* 0x000000c4 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_436__vbase_byte_offset   200 /* 0x000000c8 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_436__dbase_byte_offset   224 /* 0x000000e0 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_436__dpitchm1_byte_offset    228 /* 0x000000e4 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_436__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_436__W   3264
#define bisenet_pelee_picinfo__prim_split_1___pvcn_436__H   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_436__D   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_436__P   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_436__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_436__dpitch_num_bytes    3264
#define bisenet_pelee_picinfo__prim_split_1___pvcn_436__dram_format     0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_436__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_436__tile_width  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_436__tile_height     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_436__vmem_buffer_num_bytes   3264

/* bisenet_pelee_picinfo__prim_split_1 SMB_input __pvcn_437_ */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_437__cnngen_demangled_name   "__pvcn_437_"
#define bisenet_pelee_picinfo__prim_split_1___pvcn_437__is_constant     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_437__byte_offset     236 /* 0x000000ec */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_437__vbase_byte_offset   240 /* 0x000000f0 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_437__dbase_byte_offset   264 /* 0x00000108 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_437__dpitchm1_byte_offset    268 /* 0x0000010c */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_437__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_437__W   88
#define bisenet_pelee_picinfo__prim_split_1___pvcn_437__H   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_437__D   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_437__P   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_437__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_437__dpitch_num_bytes    96
#define bisenet_pelee_picinfo__prim_split_1___pvcn_437__dram_format     0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_437__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_437__tile_width  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_437__tile_height     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_437__vmem_buffer_num_bytes   88

/* bisenet_pelee_picinfo__prim_split_1 SMB_input __pvcn_439_ */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_439__cnngen_demangled_name   "__pvcn_439_"
#define bisenet_pelee_picinfo__prim_split_1___pvcn_439__is_constant     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_439__byte_offset     276 /* 0x00000114 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_439__vbase_byte_offset   280 /* 0x00000118 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_439__dbase_byte_offset   304 /* 0x00000130 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_439__dpitchm1_byte_offset    308 /* 0x00000134 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_439__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_439__W   1620
#define bisenet_pelee_picinfo__prim_split_1___pvcn_439__H   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_439__D   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_439__P   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_439__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_439__dpitch_num_bytes    1632
#define bisenet_pelee_picinfo__prim_split_1___pvcn_439__dram_format     0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_439__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_439__tile_width  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_439__tile_height     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_439__vmem_buffer_num_bytes   1620

/* bisenet_pelee_picinfo__prim_split_1 SMB_input __pvcn_440_ */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_440__cnngen_demangled_name   "__pvcn_440_"
#define bisenet_pelee_picinfo__prim_split_1___pvcn_440__is_constant     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_440__byte_offset     316 /* 0x0000013c */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_440__vbase_byte_offset   320 /* 0x00000140 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_440__dbase_byte_offset   344 /* 0x00000158 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_440__dpitchm1_byte_offset    348 /* 0x0000015c */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_440__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_440__W   88
#define bisenet_pelee_picinfo__prim_split_1___pvcn_440__H   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_440__D   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_440__P   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_440__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_440__dpitch_num_bytes    96
#define bisenet_pelee_picinfo__prim_split_1___pvcn_440__dram_format     0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_440__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_440__tile_width  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_440__tile_height     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_440__vmem_buffer_num_bytes   88

/* bisenet_pelee_picinfo__prim_split_1 SMB_input __pvcn_446_ */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_446__cnngen_demangled_name   "__pvcn_446_"
#define bisenet_pelee_picinfo__prim_split_1___pvcn_446__is_constant     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_446__byte_offset     356 /* 0x00000164 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_446__vbase_byte_offset   360 /* 0x00000168 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_446__dbase_byte_offset   384 /* 0x00000180 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_446__dpitchm1_byte_offset    388 /* 0x00000184 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_446__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_446__W   8652
#define bisenet_pelee_picinfo__prim_split_1___pvcn_446__H   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_446__D   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_446__P   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_446__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_446__dpitch_num_bytes    8672
#define bisenet_pelee_picinfo__prim_split_1___pvcn_446__dram_format     0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_446__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_446__tile_width  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_446__tile_height     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_446__vmem_buffer_num_bytes   8652

/* bisenet_pelee_picinfo__prim_split_1 SMB_input __pvcn_447_ */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_447__cnngen_demangled_name   "__pvcn_447_"
#define bisenet_pelee_picinfo__prim_split_1___pvcn_447__is_constant     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_447__byte_offset     396 /* 0x0000018c */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_447__vbase_byte_offset   400 /* 0x00000190 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_447__dbase_byte_offset   424 /* 0x000001a8 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_447__dpitchm1_byte_offset    428 /* 0x000001ac */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_447__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_447__W   320
#define bisenet_pelee_picinfo__prim_split_1___pvcn_447__H   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_447__D   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_447__P   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_447__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_447__dpitch_num_bytes    320
#define bisenet_pelee_picinfo__prim_split_1___pvcn_447__dram_format     0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_447__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_447__tile_width  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_447__tile_height     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_447__vmem_buffer_num_bytes   320

/* bisenet_pelee_picinfo__prim_split_1 SMB_input __pvcn_450_ */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_450__cnngen_demangled_name   "__pvcn_450_"
#define bisenet_pelee_picinfo__prim_split_1___pvcn_450__is_constant     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_450__byte_offset     436 /* 0x000001b4 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_450__vbase_byte_offset   440 /* 0x000001b8 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_450__dbase_byte_offset   464 /* 0x000001d0 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_450__dpitchm1_byte_offset    468 /* 0x000001d4 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_450__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_450__W   3236
#define bisenet_pelee_picinfo__prim_split_1___pvcn_450__H   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_450__D   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_450__P   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_450__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_450__dpitch_num_bytes    3264
#define bisenet_pelee_picinfo__prim_split_1___pvcn_450__dram_format     0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_450__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_450__tile_width  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_450__tile_height     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_450__vmem_buffer_num_bytes   3236

/* bisenet_pelee_picinfo__prim_split_1 SMB_input __pvcn_451_ */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_451__cnngen_demangled_name   "__pvcn_451_"
#define bisenet_pelee_picinfo__prim_split_1___pvcn_451__is_constant     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_451__byte_offset     476 /* 0x000001dc */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_451__vbase_byte_offset   480 /* 0x000001e0 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_451__dbase_byte_offset   504 /* 0x000001f8 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_451__dpitchm1_byte_offset    508 /* 0x000001fc */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_451__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_451__W   88
#define bisenet_pelee_picinfo__prim_split_1___pvcn_451__H   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_451__D   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_451__P   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_451__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_451__dpitch_num_bytes    96
#define bisenet_pelee_picinfo__prim_split_1___pvcn_451__dram_format     0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_451__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_451__tile_width  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_451__tile_height     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_451__vmem_buffer_num_bytes   88

/* bisenet_pelee_picinfo__prim_split_1 SMB_input __pvcn_454_ */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_454__cnngen_demangled_name   "__pvcn_454_"
#define bisenet_pelee_picinfo__prim_split_1___pvcn_454__is_constant     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_454__byte_offset     516 /* 0x00000204 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_454__vbase_byte_offset   520 /* 0x00000208 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_454__dbase_byte_offset   544 /* 0x00000220 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_454__dpitchm1_byte_offset    548 /* 0x00000224 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_454__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_454__W   3196
#define bisenet_pelee_picinfo__prim_split_1___pvcn_454__H   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_454__D   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_454__P   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_454__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_454__dpitch_num_bytes    3200
#define bisenet_pelee_picinfo__prim_split_1___pvcn_454__dram_format     0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_454__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_454__tile_width  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_454__tile_height     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_454__vmem_buffer_num_bytes   3196

/* bisenet_pelee_picinfo__prim_split_1 SMB_input __pvcn_455_ */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_455__cnngen_demangled_name   "__pvcn_455_"
#define bisenet_pelee_picinfo__prim_split_1___pvcn_455__is_constant     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_455__byte_offset     556 /* 0x0000022c */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_455__vbase_byte_offset   560 /* 0x00000230 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_455__dbase_byte_offset   584 /* 0x00000248 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_455__dpitchm1_byte_offset    588 /* 0x0000024c */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_455__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_455__W   88
#define bisenet_pelee_picinfo__prim_split_1___pvcn_455__H   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_455__D   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_455__P   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_455__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_455__dpitch_num_bytes    96
#define bisenet_pelee_picinfo__prim_split_1___pvcn_455__dram_format     0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_455__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_455__tile_width  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_455__tile_height     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_455__vmem_buffer_num_bytes   88

/* bisenet_pelee_picinfo__prim_split_1 SMB_input __pvcn_457_ */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_457__cnngen_demangled_name   "__pvcn_457_"
#define bisenet_pelee_picinfo__prim_split_1___pvcn_457__is_constant     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_457__byte_offset     596 /* 0x00000254 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_457__vbase_byte_offset   600 /* 0x00000258 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_457__dbase_byte_offset   624 /* 0x00000270 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_457__dpitchm1_byte_offset    628 /* 0x00000274 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_457__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_457__W   1632
#define bisenet_pelee_picinfo__prim_split_1___pvcn_457__H   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_457__D   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_457__P   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_457__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_457__dpitch_num_bytes    1632
#define bisenet_pelee_picinfo__prim_split_1___pvcn_457__dram_format     0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_457__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_457__tile_width  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_457__tile_height     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_457__vmem_buffer_num_bytes   1632

/* bisenet_pelee_picinfo__prim_split_1 SMB_input __pvcn_458_ */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_458__cnngen_demangled_name   "__pvcn_458_"
#define bisenet_pelee_picinfo__prim_split_1___pvcn_458__is_constant     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_458__byte_offset     636 /* 0x0000027c */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_458__vbase_byte_offset   640 /* 0x00000280 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_458__dbase_byte_offset   664 /* 0x00000298 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_458__dpitchm1_byte_offset    668 /* 0x0000029c */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_458__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_458__W   88
#define bisenet_pelee_picinfo__prim_split_1___pvcn_458__H   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_458__D   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_458__P   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_458__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_458__dpitch_num_bytes    96
#define bisenet_pelee_picinfo__prim_split_1___pvcn_458__dram_format     0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_458__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_458__tile_width  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_458__tile_height     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_458__vmem_buffer_num_bytes   88

/* bisenet_pelee_picinfo__prim_split_1 SMB_input __pvcn_466_ */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_466__cnngen_demangled_name   "__pvcn_466_"
#define bisenet_pelee_picinfo__prim_split_1___pvcn_466__is_constant     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_466__byte_offset     688 /* 0x000002b0 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_466__vbase_byte_offset   692 /* 0x000002b4 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_466__dbase_byte_offset   716 /* 0x000002cc */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_466__dpitchm1_byte_offset    720 /* 0x000002d0 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_466__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_466__W   10056
#define bisenet_pelee_picinfo__prim_split_1___pvcn_466__H   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_466__D   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_466__P   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_466__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_466__dpitch_num_bytes    10080
#define bisenet_pelee_picinfo__prim_split_1___pvcn_466__dram_format     0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_466__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_466__tile_width  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_466__tile_height     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_466__vmem_buffer_num_bytes   10056

/* bisenet_pelee_picinfo__prim_split_1 SMB_input __pvcn_467_ */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_467__cnngen_demangled_name   "__pvcn_467_"
#define bisenet_pelee_picinfo__prim_split_1___pvcn_467__is_constant     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_467__byte_offset     728 /* 0x000002d8 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_467__vbase_byte_offset   732 /* 0x000002dc */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_467__dbase_byte_offset   756 /* 0x000002f4 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_467__dpitchm1_byte_offset    760 /* 0x000002f8 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_467__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_467__W   320
#define bisenet_pelee_picinfo__prim_split_1___pvcn_467__H   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_467__D   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_467__P   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_467__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_467__dpitch_num_bytes    320
#define bisenet_pelee_picinfo__prim_split_1___pvcn_467__dram_format     0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_467__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_467__tile_width  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_467__tile_height     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_467__vmem_buffer_num_bytes   320

/* bisenet_pelee_picinfo__prim_split_1 SMB_input __pvcn_470_ */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_470__cnngen_demangled_name   "__pvcn_470_"
#define bisenet_pelee_picinfo__prim_split_1___pvcn_470__is_constant     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_470__byte_offset     768 /* 0x00000300 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_470__vbase_byte_offset   772 /* 0x00000304 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_470__dbase_byte_offset   796 /* 0x0000031c */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_470__dpitchm1_byte_offset    800 /* 0x00000320 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_470__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_470__W   3284
#define bisenet_pelee_picinfo__prim_split_1___pvcn_470__H   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_470__D   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_470__P   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_470__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_470__dpitch_num_bytes    3296
#define bisenet_pelee_picinfo__prim_split_1___pvcn_470__dram_format     0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_470__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_470__tile_width  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_470__tile_height     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_470__vmem_buffer_num_bytes   3284

/* bisenet_pelee_picinfo__prim_split_1 SMB_input __pvcn_471_ */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_471__cnngen_demangled_name   "__pvcn_471_"
#define bisenet_pelee_picinfo__prim_split_1___pvcn_471__is_constant     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_471__byte_offset     808 /* 0x00000328 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_471__vbase_byte_offset   812 /* 0x0000032c */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_471__dbase_byte_offset   836 /* 0x00000344 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_471__dpitchm1_byte_offset    840 /* 0x00000348 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_471__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_471__W   80
#define bisenet_pelee_picinfo__prim_split_1___pvcn_471__H   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_471__D   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_471__P   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_471__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_471__dpitch_num_bytes    96
#define bisenet_pelee_picinfo__prim_split_1___pvcn_471__dram_format     0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_471__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_471__tile_width  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_471__tile_height     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_471__vmem_buffer_num_bytes   80

/* bisenet_pelee_picinfo__prim_split_1 SMB_input __pvcn_474_ */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_474__cnngen_demangled_name   "__pvcn_474_"
#define bisenet_pelee_picinfo__prim_split_1___pvcn_474__is_constant     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_474__byte_offset     848 /* 0x00000350 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_474__vbase_byte_offset   852 /* 0x00000354 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_474__dbase_byte_offset   876 /* 0x0000036c */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_474__dpitchm1_byte_offset    880 /* 0x00000370 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_474__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_474__W   3252
#define bisenet_pelee_picinfo__prim_split_1___pvcn_474__H   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_474__D   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_474__P   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_474__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_474__dpitch_num_bytes    3264
#define bisenet_pelee_picinfo__prim_split_1___pvcn_474__dram_format     0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_474__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_474__tile_width  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_474__tile_height     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_474__vmem_buffer_num_bytes   3252

/* bisenet_pelee_picinfo__prim_split_1 SMB_input __pvcn_475_ */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_475__cnngen_demangled_name   "__pvcn_475_"
#define bisenet_pelee_picinfo__prim_split_1___pvcn_475__is_constant     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_475__byte_offset     888 /* 0x00000378 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_475__vbase_byte_offset   892 /* 0x0000037c */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_475__dbase_byte_offset   916 /* 0x00000394 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_475__dpitchm1_byte_offset    920 /* 0x00000398 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_475__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_475__W   88
#define bisenet_pelee_picinfo__prim_split_1___pvcn_475__H   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_475__D   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_475__P   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_475__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_475__dpitch_num_bytes    96
#define bisenet_pelee_picinfo__prim_split_1___pvcn_475__dram_format     0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_475__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_475__tile_width  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_475__tile_height     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_475__vmem_buffer_num_bytes   88

/* bisenet_pelee_picinfo__prim_split_1 SMB_input __pvcn_477_ */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_477__cnngen_demangled_name   "__pvcn_477_"
#define bisenet_pelee_picinfo__prim_split_1___pvcn_477__is_constant     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_477__byte_offset     928 /* 0x000003a0 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_477__vbase_byte_offset   932 /* 0x000003a4 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_477__dbase_byte_offset   956 /* 0x000003bc */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_477__dpitchm1_byte_offset    960 /* 0x000003c0 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_477__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_477__W   1596
#define bisenet_pelee_picinfo__prim_split_1___pvcn_477__H   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_477__D   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_477__P   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_477__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_477__dpitch_num_bytes    1600
#define bisenet_pelee_picinfo__prim_split_1___pvcn_477__dram_format     0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_477__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_477__tile_width  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_477__tile_height     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_477__vmem_buffer_num_bytes   1596

/* bisenet_pelee_picinfo__prim_split_1 SMB_input __pvcn_478_ */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_478__cnngen_demangled_name   "__pvcn_478_"
#define bisenet_pelee_picinfo__prim_split_1___pvcn_478__is_constant     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_478__byte_offset     968 /* 0x000003c8 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_478__vbase_byte_offset   972 /* 0x000003cc */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_478__dbase_byte_offset   996 /* 0x000003e4 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_478__dpitchm1_byte_offset    1000    /* 0x000003e8 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_478__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_478__W   88
#define bisenet_pelee_picinfo__prim_split_1___pvcn_478__H   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_478__D   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_478__P   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_478__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_478__dpitch_num_bytes    96
#define bisenet_pelee_picinfo__prim_split_1___pvcn_478__dram_format     0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_478__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_478__tile_width  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_478__tile_height     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_478__vmem_buffer_num_bytes   88

/* bisenet_pelee_picinfo__prim_split_1 SMB_input __pvcn_484_ */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_484__cnngen_demangled_name   "__pvcn_484_"
#define bisenet_pelee_picinfo__prim_split_1___pvcn_484__is_constant     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_484__byte_offset     1008    /* 0x000003f0 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_484__vbase_byte_offset   1012    /* 0x000003f4 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_484__dbase_byte_offset   1036    /* 0x0000040c */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_484__dpitchm1_byte_offset    1040    /* 0x00000410 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_484__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_484__W   46336
#define bisenet_pelee_picinfo__prim_split_1___pvcn_484__H   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_484__D   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_484__P   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_484__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_484__dpitch_num_bytes    46336
#define bisenet_pelee_picinfo__prim_split_1___pvcn_484__dram_format     0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_484__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_484__tile_width  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_484__tile_height     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_484__vmem_buffer_num_bytes   46336

/* bisenet_pelee_picinfo__prim_split_1 SMB_input __pvcn_485_ */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_485__cnngen_demangled_name   "__pvcn_485_"
#define bisenet_pelee_picinfo__prim_split_1___pvcn_485__is_constant     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_485__byte_offset     1048    /* 0x00000418 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_485__vbase_byte_offset   1052    /* 0x0000041c */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_485__dbase_byte_offset   1076    /* 0x00000434 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_485__dpitchm1_byte_offset    1080    /* 0x00000438 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_485__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_485__W   1280
#define bisenet_pelee_picinfo__prim_split_1___pvcn_485__H   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_485__D   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_485__P   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_485__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_485__dpitch_num_bytes    1280
#define bisenet_pelee_picinfo__prim_split_1___pvcn_485__dram_format     0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_485__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_485__tile_width  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_485__tile_height     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_485__vmem_buffer_num_bytes   1280

/* bisenet_pelee_picinfo__prim_split_1 SMB_input __pvcn_498_ */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_498__cnngen_demangled_name   "__pvcn_498_"
#define bisenet_pelee_picinfo__prim_split_1___pvcn_498__is_constant     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_498__byte_offset     1088    /* 0x00000440 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_498__vbase_byte_offset   1092    /* 0x00000444 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_498__dbase_byte_offset   1116    /* 0x0000045c */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_498__dpitchm1_byte_offset    1120    /* 0x00000460 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_498__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_498__W   256
#define bisenet_pelee_picinfo__prim_split_1___pvcn_498__H   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_498__D   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_498__P   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_498__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_498__dpitch_num_bytes    256
#define bisenet_pelee_picinfo__prim_split_1___pvcn_498__dram_format     0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_498__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_498__tile_width  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_498__tile_height     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_498__vmem_buffer_num_bytes   256

/* bisenet_pelee_picinfo__prim_split_1 SMB_input __pvcn_499_ */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_499__cnngen_demangled_name   "__pvcn_499_"
#define bisenet_pelee_picinfo__prim_split_1___pvcn_499__is_constant     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_499__byte_offset     1128    /* 0x00000468 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_499__vbase_byte_offset   1132    /* 0x0000046c */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_499__dbase_byte_offset   1156    /* 0x00000484 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_499__dpitchm1_byte_offset    1160    /* 0x00000488 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_499__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_499__W   88
#define bisenet_pelee_picinfo__prim_split_1___pvcn_499__H   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_499__D   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_499__P   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_499__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_499__dpitch_num_bytes    96
#define bisenet_pelee_picinfo__prim_split_1___pvcn_499__dram_format     0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_499__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_499__tile_width  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_499__tile_height     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_499__vmem_buffer_num_bytes   88

/* bisenet_pelee_picinfo__prim_split_1 SMB_input __pvcn_503_ */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_503__cnngen_demangled_name   "__pvcn_503_"
#define bisenet_pelee_picinfo__prim_split_1___pvcn_503__is_constant     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_503__byte_offset     1168    /* 0x00000490 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_503__vbase_byte_offset   1172    /* 0x00000494 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_503__dbase_byte_offset   1196    /* 0x000004ac */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_503__dpitchm1_byte_offset    1200    /* 0x000004b0 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_503__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_503__W   256
#define bisenet_pelee_picinfo__prim_split_1___pvcn_503__H   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_503__D   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_503__P   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_503__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_503__dpitch_num_bytes    256
#define bisenet_pelee_picinfo__prim_split_1___pvcn_503__dram_format     0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_503__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_503__tile_width  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_503__tile_height     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_503__vmem_buffer_num_bytes   256

/* bisenet_pelee_picinfo__prim_split_1 SMB_input __pvcn_504_ */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_504__cnngen_demangled_name   "__pvcn_504_"
#define bisenet_pelee_picinfo__prim_split_1___pvcn_504__is_constant     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_504__byte_offset     1208    /* 0x000004b8 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_504__vbase_byte_offset   1212    /* 0x000004bc */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_504__dbase_byte_offset   1236    /* 0x000004d4 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_504__dpitchm1_byte_offset    1240    /* 0x000004d8 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_504__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_504__W   88
#define bisenet_pelee_picinfo__prim_split_1___pvcn_504__H   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_504__D   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_504__P   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_504__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_504__dpitch_num_bytes    96
#define bisenet_pelee_picinfo__prim_split_1___pvcn_504__dram_format     0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_504__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_504__tile_width  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_504__tile_height     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_504__vmem_buffer_num_bytes   88

/* bisenet_pelee_picinfo__prim_split_1 SMB_input __pvcn_508_ */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_508__cnngen_demangled_name   "__pvcn_508_"
#define bisenet_pelee_picinfo__prim_split_1___pvcn_508__is_constant     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_508__byte_offset     1248    /* 0x000004e0 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_508__vbase_byte_offset   1252    /* 0x000004e4 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_508__dbase_byte_offset   1276    /* 0x000004fc */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_508__dpitchm1_byte_offset    1280    /* 0x00000500 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_508__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_508__W   256
#define bisenet_pelee_picinfo__prim_split_1___pvcn_508__H   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_508__D   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_508__P   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_508__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_508__dpitch_num_bytes    256
#define bisenet_pelee_picinfo__prim_split_1___pvcn_508__dram_format     0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_508__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_508__tile_width  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_508__tile_height     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_508__vmem_buffer_num_bytes   256

/* bisenet_pelee_picinfo__prim_split_1 SMB_input __pvcn_509_ */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_509__cnngen_demangled_name   "__pvcn_509_"
#define bisenet_pelee_picinfo__prim_split_1___pvcn_509__is_constant     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_509__byte_offset     1288    /* 0x00000508 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_509__vbase_byte_offset   1292    /* 0x0000050c */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_509__dbase_byte_offset   1316    /* 0x00000524 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_509__dpitchm1_byte_offset    1320    /* 0x00000528 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_509__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_509__W   88
#define bisenet_pelee_picinfo__prim_split_1___pvcn_509__H   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_509__D   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_509__P   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_509__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_509__dpitch_num_bytes    96
#define bisenet_pelee_picinfo__prim_split_1___pvcn_509__dram_format     0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_509__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_509__tile_width  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_509__tile_height     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_509__vmem_buffer_num_bytes   88

/* bisenet_pelee_picinfo__prim_split_1 SMB_input __pvcn_513_ */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_513__cnngen_demangled_name   "__pvcn_513_"
#define bisenet_pelee_picinfo__prim_split_1___pvcn_513__is_constant     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_513__byte_offset     1328    /* 0x00000530 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_513__vbase_byte_offset   1332    /* 0x00000534 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_513__dbase_byte_offset   1356    /* 0x0000054c */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_513__dpitchm1_byte_offset    1360    /* 0x00000550 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_513__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_513__W   256
#define bisenet_pelee_picinfo__prim_split_1___pvcn_513__H   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_513__D   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_513__P   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_513__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_513__dpitch_num_bytes    256
#define bisenet_pelee_picinfo__prim_split_1___pvcn_513__dram_format     0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_513__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_513__tile_width  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_513__tile_height     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_513__vmem_buffer_num_bytes   256

/* bisenet_pelee_picinfo__prim_split_1 SMB_input __pvcn_514_ */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_514__cnngen_demangled_name   "__pvcn_514_"
#define bisenet_pelee_picinfo__prim_split_1___pvcn_514__is_constant     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_514__byte_offset     1368    /* 0x00000558 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_514__vbase_byte_offset   1372    /* 0x0000055c */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_514__dbase_byte_offset   1396    /* 0x00000574 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_514__dpitchm1_byte_offset    1400    /* 0x00000578 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_514__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_514__W   88
#define bisenet_pelee_picinfo__prim_split_1___pvcn_514__H   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_514__D   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_514__P   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_514__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_514__dpitch_num_bytes    96
#define bisenet_pelee_picinfo__prim_split_1___pvcn_514__dram_format     0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_514__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_514__tile_width  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_514__tile_height     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_514__vmem_buffer_num_bytes   88

/* bisenet_pelee_picinfo__prim_split_1 SMB_input __pvcn_518_ */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_518__cnngen_demangled_name   "__pvcn_518_"
#define bisenet_pelee_picinfo__prim_split_1___pvcn_518__is_constant     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_518__byte_offset     1408    /* 0x00000580 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_518__vbase_byte_offset   1412    /* 0x00000584 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_518__dbase_byte_offset   1436    /* 0x0000059c */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_518__dpitchm1_byte_offset    1440    /* 0x000005a0 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_518__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_518__W   256
#define bisenet_pelee_picinfo__prim_split_1___pvcn_518__H   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_518__D   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_518__P   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_518__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_518__dpitch_num_bytes    256
#define bisenet_pelee_picinfo__prim_split_1___pvcn_518__dram_format     0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_518__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_518__tile_width  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_518__tile_height     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_518__vmem_buffer_num_bytes   256

/* bisenet_pelee_picinfo__prim_split_1 SMB_input __pvcn_519_ */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_519__cnngen_demangled_name   "__pvcn_519_"
#define bisenet_pelee_picinfo__prim_split_1___pvcn_519__is_constant     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_519__byte_offset     1448    /* 0x000005a8 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_519__vbase_byte_offset   1452    /* 0x000005ac */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_519__dbase_byte_offset   1476    /* 0x000005c4 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_519__dpitchm1_byte_offset    1480    /* 0x000005c8 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_519__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_519__W   88
#define bisenet_pelee_picinfo__prim_split_1___pvcn_519__H   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_519__D   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_519__P   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_519__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_519__dpitch_num_bytes    96
#define bisenet_pelee_picinfo__prim_split_1___pvcn_519__dram_format     0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_519__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_519__tile_width  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_519__tile_height     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_519__vmem_buffer_num_bytes   88

/* bisenet_pelee_picinfo__prim_split_1 SMB_input __pvcn_523_ */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_523__cnngen_demangled_name   "__pvcn_523_"
#define bisenet_pelee_picinfo__prim_split_1___pvcn_523__is_constant     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_523__byte_offset     1488    /* 0x000005d0 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_523__vbase_byte_offset   1492    /* 0x000005d4 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_523__dbase_byte_offset   1516    /* 0x000005ec */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_523__dpitchm1_byte_offset    1520    /* 0x000005f0 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_523__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_523__W   256
#define bisenet_pelee_picinfo__prim_split_1___pvcn_523__H   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_523__D   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_523__P   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_523__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_523__dpitch_num_bytes    256
#define bisenet_pelee_picinfo__prim_split_1___pvcn_523__dram_format     0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_523__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_523__tile_width  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_523__tile_height     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_523__vmem_buffer_num_bytes   256

/* bisenet_pelee_picinfo__prim_split_1 SMB_input __pvcn_524_ */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_524__cnngen_demangled_name   "__pvcn_524_"
#define bisenet_pelee_picinfo__prim_split_1___pvcn_524__is_constant     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_524__byte_offset     1528    /* 0x000005f8 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_524__vbase_byte_offset   1532    /* 0x000005fc */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_524__dbase_byte_offset   1556    /* 0x00000614 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_524__dpitchm1_byte_offset    1560    /* 0x00000618 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_524__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_524__W   88
#define bisenet_pelee_picinfo__prim_split_1___pvcn_524__H   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_524__D   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_524__P   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_524__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_524__dpitch_num_bytes    96
#define bisenet_pelee_picinfo__prim_split_1___pvcn_524__dram_format     0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_524__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_524__tile_width  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_524__tile_height     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_524__vmem_buffer_num_bytes   88

/* bisenet_pelee_picinfo__prim_split_1 SMB_input __pvcn_528_ */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_528__cnngen_demangled_name   "__pvcn_528_"
#define bisenet_pelee_picinfo__prim_split_1___pvcn_528__is_constant     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_528__byte_offset     1568    /* 0x00000620 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_528__vbase_byte_offset   1572    /* 0x00000624 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_528__dbase_byte_offset   1596    /* 0x0000063c */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_528__dpitchm1_byte_offset    1600    /* 0x00000640 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_528__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_528__W   256
#define bisenet_pelee_picinfo__prim_split_1___pvcn_528__H   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_528__D   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_528__P   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_528__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_528__dpitch_num_bytes    256
#define bisenet_pelee_picinfo__prim_split_1___pvcn_528__dram_format     0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_528__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_528__tile_width  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_528__tile_height     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_528__vmem_buffer_num_bytes   256

/* bisenet_pelee_picinfo__prim_split_1 SMB_input __pvcn_529_ */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_529__cnngen_demangled_name   "__pvcn_529_"
#define bisenet_pelee_picinfo__prim_split_1___pvcn_529__is_constant     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_529__byte_offset     1608    /* 0x00000648 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_529__vbase_byte_offset   1612    /* 0x0000064c */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_529__dbase_byte_offset   1636    /* 0x00000664 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_529__dpitchm1_byte_offset    1640    /* 0x00000668 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_529__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_529__W   88
#define bisenet_pelee_picinfo__prim_split_1___pvcn_529__H   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_529__D   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_529__P   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_529__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_529__dpitch_num_bytes    96
#define bisenet_pelee_picinfo__prim_split_1___pvcn_529__dram_format     0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_529__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_529__tile_width  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_529__tile_height     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_529__vmem_buffer_num_bytes   88

/* bisenet_pelee_picinfo__prim_split_1 SMB_input __pvcn_533_ */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_533__cnngen_demangled_name   "__pvcn_533_"
#define bisenet_pelee_picinfo__prim_split_1___pvcn_533__is_constant     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_533__byte_offset     1648    /* 0x00000670 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_533__vbase_byte_offset   1652    /* 0x00000674 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_533__dbase_byte_offset   1676    /* 0x0000068c */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_533__dpitchm1_byte_offset    1680    /* 0x00000690 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_533__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_533__W   256
#define bisenet_pelee_picinfo__prim_split_1___pvcn_533__H   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_533__D   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_533__P   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_533__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_533__dpitch_num_bytes    256
#define bisenet_pelee_picinfo__prim_split_1___pvcn_533__dram_format     0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_533__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_533__tile_width  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_533__tile_height     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_533__vmem_buffer_num_bytes   256

/* bisenet_pelee_picinfo__prim_split_1 SMB_input __pvcn_534_ */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_534__cnngen_demangled_name   "__pvcn_534_"
#define bisenet_pelee_picinfo__prim_split_1___pvcn_534__is_constant     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_534__byte_offset     1688    /* 0x00000698 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_534__vbase_byte_offset   1692    /* 0x0000069c */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_534__dbase_byte_offset   1716    /* 0x000006b4 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_534__dpitchm1_byte_offset    1720    /* 0x000006b8 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_534__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_1___pvcn_534__W   88
#define bisenet_pelee_picinfo__prim_split_1___pvcn_534__H   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_534__D   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_534__P   1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_534__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_534__dpitch_num_bytes    96
#define bisenet_pelee_picinfo__prim_split_1___pvcn_534__dram_format     0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_534__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_1___pvcn_534__tile_width  1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_534__tile_height     1
#define bisenet_pelee_picinfo__prim_split_1___pvcn_534__vmem_buffer_num_bytes   88

/* bisenet_pelee_picinfo__prim_split_1 VCB PeleeNet__stage_2_dense_block_1__concat */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_1__concat_cnngen_demangled_name   "PeleeNet__stage_2_dense_block_1__concat"
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_1__concat_byte_offset     1728    /* 0x000006c0 */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_1__concat_vbase_byte_offset   1736    /* 0x000006c8 */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_1__concat_vmem_buffer_num_bytes   371712

/* bisenet_pelee_picinfo__prim_split_1 VCB PeleeNet__stage_2_dense_block_1__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_1__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_cnngen_demangled_name    "PeleeNet__stage_2_dense_block_1__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm"
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_1__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_byte_offset  1780    /* 0x000006f4 */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_1__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_vbase_byte_offset    1788    /* 0x000006fc */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_1__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_vmem_buffer_num_bytes    82944

/* bisenet_pelee_picinfo__prim_split_1 VCB PeleeNet__stage_2_dense_block_1__conv_left_0__Relu */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_1__conv_left_0__Relu_cnngen_demangled_name    "PeleeNet__stage_2_dense_block_1__conv_left_0__Relu"
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_1__conv_left_0__Relu_byte_offset  1832    /* 0x00000728 */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_1__conv_left_0__Relu_vbase_byte_offset    1840    /* 0x00000730 */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_1__conv_left_0__Relu_vmem_buffer_num_bytes    28160

/* bisenet_pelee_picinfo__prim_split_1 VCB PeleeNet__stage_2_dense_block_1__conv_right_0__Relu */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_1__conv_right_0__Relu_cnngen_demangled_name   "PeleeNet__stage_2_dense_block_1__conv_right_0__Relu"
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_1__conv_right_0__Relu_byte_offset     1904    /* 0x00000770 */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_1__conv_right_0__Relu_vbase_byte_offset   1912    /* 0x00000778 */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_1__conv_right_0__Relu_vmem_buffer_num_bytes   28160

/* bisenet_pelee_picinfo__prim_split_1 VCB PeleeNet__stage_2_dense_block_1__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_1__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_cnngen_demangled_name  "PeleeNet__stage_2_dense_block_1__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu"
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_1__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_byte_offset    1952    /* 0x000007a0 */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_1__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vbase_byte_offset  1960    /* 0x000007a8 */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_1__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vmem_buffer_num_bytes  27520

/* bisenet_pelee_picinfo__prim_split_1 VCB PeleeNet__stage_2_dense_block_2__concat */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_2__concat_cnngen_demangled_name   "PeleeNet__stage_2_dense_block_2__concat"
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_2__concat_byte_offset     2028    /* 0x000007ec */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_2__concat_vbase_byte_offset   2036    /* 0x000007f4 */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_2__concat_vmem_buffer_num_bytes   250880

/* bisenet_pelee_picinfo__prim_split_1 VCB PeleeNet__stage_2_dense_block_2__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_2__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_cnngen_demangled_name    "PeleeNet__stage_2_dense_block_2__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm"
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_2__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_byte_offset  2100    /* 0x00000834 */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_2__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_vbase_byte_offset    2108    /* 0x0000083c */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_2__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_vmem_buffer_num_bytes    82944

/* bisenet_pelee_picinfo__prim_split_1 VCB PeleeNet__stage_2_dense_block_2__conv_left_0__Relu */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_2__conv_left_0__Relu_cnngen_demangled_name    "PeleeNet__stage_2_dense_block_2__conv_left_0__Relu"
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_2__conv_left_0__Relu_byte_offset  2152    /* 0x00000868 */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_2__conv_left_0__Relu_vbase_byte_offset    2160    /* 0x00000870 */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_2__conv_left_0__Relu_vmem_buffer_num_bytes    28160

/* bisenet_pelee_picinfo__prim_split_1 VCB PeleeNet__stage_2_dense_block_2__conv_right_0__Relu */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_2__conv_right_0__Relu_cnngen_demangled_name   "PeleeNet__stage_2_dense_block_2__conv_right_0__Relu"
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_2__conv_right_0__Relu_byte_offset     2224    /* 0x000008b0 */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_2__conv_right_0__Relu_vbase_byte_offset   2232    /* 0x000008b8 */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_2__conv_right_0__Relu_vmem_buffer_num_bytes   28160

/* bisenet_pelee_picinfo__prim_split_1 VCB PeleeNet__stage_2_dense_block_2__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_2__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_cnngen_demangled_name  "PeleeNet__stage_2_dense_block_2__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu"
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_2__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_byte_offset    2272    /* 0x000008e0 */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_2__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vbase_byte_offset  2280    /* 0x000008e8 */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_2__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vmem_buffer_num_bytes  27520

/* bisenet_pelee_picinfo__prim_split_1 VCB PeleeNet__stage_2_dense_block_3__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_3__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_cnngen_demangled_name    "PeleeNet__stage_2_dense_block_3__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm"
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_3__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_byte_offset  2380    /* 0x0000094c */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_3__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_vbase_byte_offset    2388    /* 0x00000954 */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_3__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_vmem_buffer_num_bytes    62464

/* bisenet_pelee_picinfo__prim_split_1 VCB PeleeNet__stage_2_dense_block_3__conv_left_0__Relu */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_3__conv_left_0__Relu_cnngen_demangled_name    "PeleeNet__stage_2_dense_block_3__conv_left_0__Relu"
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_3__conv_left_0__Relu_byte_offset  2432    /* 0x00000980 */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_3__conv_left_0__Relu_vbase_byte_offset    2440    /* 0x00000988 */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_3__conv_left_0__Relu_vmem_buffer_num_bytes    23168

/* bisenet_pelee_picinfo__prim_split_1 VCB PeleeNet__stage_2_dense_block_3__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_3__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_cnngen_demangled_name   "PeleeNet__stage_2_dense_block_3__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu"
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_3__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_byte_offset     2480    /* 0x000009b0 */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_3__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vbase_byte_offset   2488    /* 0x000009b8 */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_3__conv_left_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vmem_buffer_num_bytes   20736

/* bisenet_pelee_picinfo__prim_split_1 VCB PeleeNet__stage_2_dense_block_3__conv_right_0__Relu */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_3__conv_right_0__Relu_cnngen_demangled_name   "PeleeNet__stage_2_dense_block_3__conv_right_0__Relu"
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_3__conv_right_0__Relu_byte_offset     2532    /* 0x000009e4 */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_3__conv_right_0__Relu_vbase_byte_offset   2540    /* 0x000009ec */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_3__conv_right_0__Relu_vmem_buffer_num_bytes   28160

/* bisenet_pelee_picinfo__prim_split_1 VCB PeleeNet__stage_2_dense_block_3__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_3__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_cnngen_demangled_name  "PeleeNet__stage_2_dense_block_3__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu"
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_3__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_byte_offset    2580    /* 0x00000a14 */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_3__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vbase_byte_offset  2588    /* 0x00000a1c */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_3__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vmem_buffer_num_bytes  27520

/* bisenet_pelee_picinfo__prim_split_1 VCB PeleeNet__stage_2_dense_block_3__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_3__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_cnngen_demangled_name  "PeleeNet__stage_2_dense_block_3__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu"
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_3__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_byte_offset    2628    /* 0x00000a44 */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_3__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_vbase_byte_offset  2636    /* 0x00000a4c */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_3__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_vmem_buffer_num_bytes  15616

/* bisenet_pelee_picinfo__prim_split_1 VCB PeleeNet__stage_2_dense_block_3__concat */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_3__concat_cnngen_demangled_name   "PeleeNet__stage_2_dense_block_3__concat"
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_3__concat_byte_offset     2708    /* 0x00000a94 */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_3__concat_vbase_byte_offset   2716    /* 0x00000a9c */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_dense_block_3__concat_vmem_buffer_num_bytes   86016

/* bisenet_pelee_picinfo__prim_split_1 VCB PeleeNet__stage_2_transition_layer__transition_layer_conv0__Conv2D___BatchNorm__FusedBatchNorm___Relu */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_conv0__Conv2D___BatchNorm__FusedBatchNorm___Relu_cnngen_demangled_name     "PeleeNet__stage_2_transition_layer__transition_layer_conv0__Conv2D___BatchNorm__FusedBatchNorm___Relu"
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_conv0__Conv2D___BatchNorm__FusedBatchNorm___Relu_byte_offset   2756    /* 0x00000ac4 */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_conv0__Conv2D___BatchNorm__FusedBatchNorm___Relu_vbase_byte_offset     2764    /* 0x00000acc */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_conv0__Conv2D___BatchNorm__FusedBatchNorm___Relu_vmem_buffer_num_bytes     327680

/* bisenet_pelee_picinfo__prim_split_1 HMB_output PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_cnngen_demangled_name     "PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool"
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_byte_offset   2800    /* 0x00000af0 */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_vbase_byte_offset     2804    /* 0x00000af4 */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_dbase_byte_offset     2828    /* 0x00000b0c */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_dpitchm1_byte_offset  2832    /* 0x00000b10 */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_dpitchm1_bsize    16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_vwidth_minus_one_byte_offset  2816    /* 0x00000b00 */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_vwidth_minus_one_bsize    16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_vheight_minus_one_byte_offset     2818    /* 0x00000b02 */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_vheight_minus_one_bsize   16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_drotate_bit_offset    22598   /* 0x00005846 */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_hflip_bit_offset  22599   /* 0x00005847 */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_vflip_bit_offset  22600   /* 0x00005848 */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_dflip_bit_offset  22601   /* 0x00005849 */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_pflip_bit_offset  22602   /* 0x0000584a */
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_W     80
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_H     32
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_D     256
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_P     1
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_data_num_bytes    1
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_dpitch_num_bytes  32
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_dram_format   3
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_vp_interleave_mode    2
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_tile_width    4
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_tile_height   1
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_vmem_buffer_num_bytes     86016
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_dram_size_num_bytes   655360
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_sign  0
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_datasize  0
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_expoffset     5
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_expbits   0
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_drotate   0
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_hflip     0
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_vflip     0
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_dflip     0
#define bisenet_pelee_picinfo__prim_split_1_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_pflip     0

/* bisenet_pelee_picinfo__prim_split_1 HMB_output __vcn_957_ */
#define bisenet_pelee_picinfo__prim_split_1___vcn_957__cnngen_demangled_name    "__vcn_957_"
#define bisenet_pelee_picinfo__prim_split_1___vcn_957__byte_offset  3076    /* 0x00000c04 */
#define bisenet_pelee_picinfo__prim_split_1___vcn_957__vbase_byte_offset    3080    /* 0x00000c08 */
#define bisenet_pelee_picinfo__prim_split_1___vcn_957__dbase_byte_offset    3104    /* 0x00000c20 */
#define bisenet_pelee_picinfo__prim_split_1___vcn_957__dpitchm1_byte_offset     3108    /* 0x00000c24 */
#define bisenet_pelee_picinfo__prim_split_1___vcn_957__dpitchm1_bsize   16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_1___vcn_957__vwidth_minus_one_byte_offset     3092    /* 0x00000c14 */
#define bisenet_pelee_picinfo__prim_split_1___vcn_957__vwidth_minus_one_bsize   16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_1___vcn_957__vheight_minus_one_byte_offset    3094    /* 0x00000c16 */
#define bisenet_pelee_picinfo__prim_split_1___vcn_957__vheight_minus_one_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_1___vcn_957__drotate_bit_offset   24806   /* 0x000060e6 */
#define bisenet_pelee_picinfo__prim_split_1___vcn_957__hflip_bit_offset     24807   /* 0x000060e7 */
#define bisenet_pelee_picinfo__prim_split_1___vcn_957__vflip_bit_offset     24808   /* 0x000060e8 */
#define bisenet_pelee_picinfo__prim_split_1___vcn_957__dflip_bit_offset     24809   /* 0x000060e9 */
#define bisenet_pelee_picinfo__prim_split_1___vcn_957__pflip_bit_offset     24810   /* 0x000060ea */
#define bisenet_pelee_picinfo__prim_split_1___vcn_957__W    80
#define bisenet_pelee_picinfo__prim_split_1___vcn_957__H    32
#define bisenet_pelee_picinfo__prim_split_1___vcn_957__D    256
#define bisenet_pelee_picinfo__prim_split_1___vcn_957__P    1
#define bisenet_pelee_picinfo__prim_split_1___vcn_957__data_num_bytes   1
#define bisenet_pelee_picinfo__prim_split_1___vcn_957__dpitch_num_bytes     32
#define bisenet_pelee_picinfo__prim_split_1___vcn_957__dram_format  3
#define bisenet_pelee_picinfo__prim_split_1___vcn_957__vp_interleave_mode   2
#define bisenet_pelee_picinfo__prim_split_1___vcn_957__tile_width   4
#define bisenet_pelee_picinfo__prim_split_1___vcn_957__tile_height  1
#define bisenet_pelee_picinfo__prim_split_1___vcn_957__vmem_buffer_num_bytes    20480
#define bisenet_pelee_picinfo__prim_split_1___vcn_957__dram_size_num_bytes  655360
#define bisenet_pelee_picinfo__prim_split_1___vcn_957__sign     0
#define bisenet_pelee_picinfo__prim_split_1___vcn_957__datasize     0
#define bisenet_pelee_picinfo__prim_split_1___vcn_957__expoffset    5
#define bisenet_pelee_picinfo__prim_split_1___vcn_957__expbits  0
#define bisenet_pelee_picinfo__prim_split_1___vcn_957__drotate  0
#define bisenet_pelee_picinfo__prim_split_1___vcn_957__hflip    0
#define bisenet_pelee_picinfo__prim_split_1___vcn_957__vflip    0
#define bisenet_pelee_picinfo__prim_split_1___vcn_957__dflip    0
#define bisenet_pelee_picinfo__prim_split_1___vcn_957__pflip    0


#endif /* bisenet_pelee_picinfo__prim_split_1_H */
