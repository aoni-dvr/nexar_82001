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
#ifndef bisenet_pelee_picinfo__prim_split_5_H
#define bisenet_pelee_picinfo__prim_split_5_H

#define vas_gen_header_version                      5

/* dag name */
#define bisenet_pelee_picinfo__prim_split_5_DAG_NAME    "bisenet_pelee_picinfo__prim_split_5"

/* VDG file info */
#define bisenet_pelee_picinfo__prim_split_5_vdg_name    "bisenet_pelee_picinfo__split_5.vdg"

/* VDG DAG memory info */
#define bisenet_pelee_picinfo__prim_split_5_byte_offset     340 /* 0x00000154 */
#define bisenet_pelee_picinfo__prim_split_5_byte_size   2300    /* 0x000008fc */
#define bisenet_pelee_picinfo__prim_split_5_preferred_dram_xfer_size    1

/* bisenet_pelee_picinfo__prim_split_5 VMEM info */
#define bisenet_pelee_picinfo__prim_split_5_use_ping_pong_vmem  0
#define bisenet_pelee_picinfo__prim_split_5_VMEM_start  4096    /* 0x00001000 */
#define bisenet_pelee_picinfo__prim_split_5_VMEM_end    2073820 /* 0x001fa4dc */
#define bisenet_pelee_picinfo__prim_split_5_image_start     1744908 /* 0x001aa00c */
#define bisenet_pelee_picinfo__prim_split_5_image_size  331212  /* 0x00050dcc */
#define bisenet_pelee_picinfo__prim_split_5_dagbin_start    2073820 /* 0x001fa4dc */

/*******************************************************************************
 * DAG byte offset info for HMB/SMB/VCB and their fields
 * in DAG 'bisenet_pelee_picinfo__prim_split_5' in source file 'bisenet_pelee_picinfo__split_5.vas'
 ******************************************************************************/
/* bisenet_pelee_picinfo__prim_split_5 HMB_input PeleeNet__stage_3_dense_block_7__concat */
#define bisenet_pelee_picinfo__prim_split_5_PeleeNet__stage_3_dense_block_7__concat_cnngen_demangled_name   "PeleeNet__stage_3_dense_block_7__concat"
#define bisenet_pelee_picinfo__prim_split_5_PeleeNet__stage_3_dense_block_7__concat_is_constant     0
#define bisenet_pelee_picinfo__prim_split_5_PeleeNet__stage_3_dense_block_7__concat_ddi_byte_size   0   /* 0x00000000 */
#define bisenet_pelee_picinfo__prim_split_5_PeleeNet__stage_3_dense_block_7__concat_ddi_byte_offset     4294967295  /* 0xffffffff */
#define bisenet_pelee_picinfo__prim_split_5_PeleeNet__stage_3_dense_block_7__concat_byte_offset     0   /* 0x00000000 */
#define bisenet_pelee_picinfo__prim_split_5_PeleeNet__stage_3_dense_block_7__concat_vbase_byte_offset   4   /* 0x00000004 */
#define bisenet_pelee_picinfo__prim_split_5_PeleeNet__stage_3_dense_block_7__concat_dbase_byte_offset   28  /* 0x0000001c */
#define bisenet_pelee_picinfo__prim_split_5_PeleeNet__stage_3_dense_block_7__concat_dpitchm1_byte_offset    32  /* 0x00000020 */
#define bisenet_pelee_picinfo__prim_split_5_PeleeNet__stage_3_dense_block_7__concat_dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_5_PeleeNet__stage_3_dense_block_7__concat_vwidth_minus_one_byte_offset    16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_5_PeleeNet__stage_3_dense_block_7__concat_vwidth_minus_one_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_5_PeleeNet__stage_3_dense_block_7__concat_vheight_minus_one_byte_offset   18  /* 0x00000012 */
#define bisenet_pelee_picinfo__prim_split_5_PeleeNet__stage_3_dense_block_7__concat_vheight_minus_one_bsize     16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_5_PeleeNet__stage_3_dense_block_7__concat_drotate_bit_offset  198 /* 0x000000c6 */
#define bisenet_pelee_picinfo__prim_split_5_PeleeNet__stage_3_dense_block_7__concat_hflip_bit_offset    199 /* 0x000000c7 */
#define bisenet_pelee_picinfo__prim_split_5_PeleeNet__stage_3_dense_block_7__concat_vflip_bit_offset    200 /* 0x000000c8 */
#define bisenet_pelee_picinfo__prim_split_5_PeleeNet__stage_3_dense_block_7__concat_dflip_bit_offset    201 /* 0x000000c9 */
#define bisenet_pelee_picinfo__prim_split_5_PeleeNet__stage_3_dense_block_7__concat_pflip_bit_offset    202 /* 0x000000ca */
#define bisenet_pelee_picinfo__prim_split_5_PeleeNet__stage_3_dense_block_7__concat_W   80
#define bisenet_pelee_picinfo__prim_split_5_PeleeNet__stage_3_dense_block_7__concat_H   32
#define bisenet_pelee_picinfo__prim_split_5_PeleeNet__stage_3_dense_block_7__concat_D   512
#define bisenet_pelee_picinfo__prim_split_5_PeleeNet__stage_3_dense_block_7__concat_P   1
#define bisenet_pelee_picinfo__prim_split_5_PeleeNet__stage_3_dense_block_7__concat_data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_5_PeleeNet__stage_3_dense_block_7__concat_dpitch_num_bytes    32
#define bisenet_pelee_picinfo__prim_split_5_PeleeNet__stage_3_dense_block_7__concat_dram_format     3
#define bisenet_pelee_picinfo__prim_split_5_PeleeNet__stage_3_dense_block_7__concat_vp_interleave_mode  2
#define bisenet_pelee_picinfo__prim_split_5_PeleeNet__stage_3_dense_block_7__concat_tile_width  4
#define bisenet_pelee_picinfo__prim_split_5_PeleeNet__stage_3_dense_block_7__concat_tile_height     1
#define bisenet_pelee_picinfo__prim_split_5_PeleeNet__stage_3_dense_block_7__concat_vmem_buffer_num_bytes   172032
#define bisenet_pelee_picinfo__prim_split_5_PeleeNet__stage_3_dense_block_7__concat_dram_size_num_bytes     1310720
#define bisenet_pelee_picinfo__prim_split_5_PeleeNet__stage_3_dense_block_7__concat_sign    0
#define bisenet_pelee_picinfo__prim_split_5_PeleeNet__stage_3_dense_block_7__concat_datasize    0
#define bisenet_pelee_picinfo__prim_split_5_PeleeNet__stage_3_dense_block_7__concat_expoffset   5
#define bisenet_pelee_picinfo__prim_split_5_PeleeNet__stage_3_dense_block_7__concat_expbits     0
#define bisenet_pelee_picinfo__prim_split_5_PeleeNet__stage_3_dense_block_7__concat_drotate     0
#define bisenet_pelee_picinfo__prim_split_5_PeleeNet__stage_3_dense_block_7__concat_hflip   0
#define bisenet_pelee_picinfo__prim_split_5_PeleeNet__stage_3_dense_block_7__concat_vflip   0
#define bisenet_pelee_picinfo__prim_split_5_PeleeNet__stage_3_dense_block_7__concat_dflip   0
#define bisenet_pelee_picinfo__prim_split_5_PeleeNet__stage_3_dense_block_7__concat_pflip   0

/* bisenet_pelee_picinfo__prim_split_5 SMB_input __pvcn_693_ */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_693__cnngen_demangled_name   "__pvcn_693_"
#define bisenet_pelee_picinfo__prim_split_5___pvcn_693__is_constant     1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_693__byte_offset     36  /* 0x00000024 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_693__vbase_byte_offset   40  /* 0x00000028 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_693__dbase_byte_offset   64  /* 0x00000040 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_693__dpitchm1_byte_offset    68  /* 0x00000044 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_693__dpitchm1_bsize  32  /* 0x00000020 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_693__W   162500
#define bisenet_pelee_picinfo__prim_split_5___pvcn_693__H   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_693__D   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_693__P   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_693__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_693__dpitch_num_bytes    162528
#define bisenet_pelee_picinfo__prim_split_5___pvcn_693__dram_format     0
#define bisenet_pelee_picinfo__prim_split_5___pvcn_693__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_5___pvcn_693__tile_width  1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_693__tile_height     1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_693__vmem_buffer_num_bytes   162500

/* bisenet_pelee_picinfo__prim_split_5 SMB_input __pvcn_694_ */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_694__cnngen_demangled_name   "__pvcn_694_"
#define bisenet_pelee_picinfo__prim_split_5___pvcn_694__is_constant     1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_694__byte_offset     72  /* 0x00000048 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_694__vbase_byte_offset   76  /* 0x0000004c */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_694__dbase_byte_offset   100 /* 0x00000064 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_694__dpitchm1_byte_offset    104 /* 0x00000068 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_694__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_694__W   2560
#define bisenet_pelee_picinfo__prim_split_5___pvcn_694__H   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_694__D   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_694__P   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_694__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_694__dpitch_num_bytes    2560
#define bisenet_pelee_picinfo__prim_split_5___pvcn_694__dram_format     0
#define bisenet_pelee_picinfo__prim_split_5___pvcn_694__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_5___pvcn_694__tile_width  1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_694__tile_height     1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_694__vmem_buffer_num_bytes   2560

/* bisenet_pelee_picinfo__prim_split_5 SMB_input __pvcn_715_ */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_715__cnngen_demangled_name   "__pvcn_715_"
#define bisenet_pelee_picinfo__prim_split_5___pvcn_715__is_constant     1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_715__byte_offset     112 /* 0x00000070 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_715__vbase_byte_offset   116 /* 0x00000074 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_715__dbase_byte_offset   140 /* 0x0000008c */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_715__dpitchm1_byte_offset    144 /* 0x00000090 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_715__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_715__W   532
#define bisenet_pelee_picinfo__prim_split_5___pvcn_715__H   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_715__D   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_715__P   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_715__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_715__dpitch_num_bytes    544
#define bisenet_pelee_picinfo__prim_split_5___pvcn_715__dram_format     0
#define bisenet_pelee_picinfo__prim_split_5___pvcn_715__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_5___pvcn_715__tile_width  1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_715__tile_height     1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_715__vmem_buffer_num_bytes   532

/* bisenet_pelee_picinfo__prim_split_5 SMB_input __pvcn_716_ */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_716__cnngen_demangled_name   "__pvcn_716_"
#define bisenet_pelee_picinfo__prim_split_5___pvcn_716__is_constant     1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_716__byte_offset     152 /* 0x00000098 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_716__vbase_byte_offset   156 /* 0x0000009c */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_716__dbase_byte_offset   180 /* 0x000000b4 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_716__dpitchm1_byte_offset    184 /* 0x000000b8 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_716__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_716__W   176
#define bisenet_pelee_picinfo__prim_split_5___pvcn_716__H   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_716__D   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_716__P   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_716__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_716__dpitch_num_bytes    192
#define bisenet_pelee_picinfo__prim_split_5___pvcn_716__dram_format     0
#define bisenet_pelee_picinfo__prim_split_5___pvcn_716__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_5___pvcn_716__tile_width  1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_716__tile_height     1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_716__vmem_buffer_num_bytes   176

/* bisenet_pelee_picinfo__prim_split_5 SMB_input __pvcn_720_ */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_720__cnngen_demangled_name   "__pvcn_720_"
#define bisenet_pelee_picinfo__prim_split_5___pvcn_720__is_constant     1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_720__byte_offset     192 /* 0x000000c0 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_720__vbase_byte_offset   196 /* 0x000000c4 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_720__dbase_byte_offset   220 /* 0x000000dc */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_720__dpitchm1_byte_offset    224 /* 0x000000e0 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_720__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_720__W   532
#define bisenet_pelee_picinfo__prim_split_5___pvcn_720__H   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_720__D   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_720__P   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_720__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_720__dpitch_num_bytes    544
#define bisenet_pelee_picinfo__prim_split_5___pvcn_720__dram_format     0
#define bisenet_pelee_picinfo__prim_split_5___pvcn_720__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_5___pvcn_720__tile_width  1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_720__tile_height     1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_720__vmem_buffer_num_bytes   532

/* bisenet_pelee_picinfo__prim_split_5 SMB_input __pvcn_721_ */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_721__cnngen_demangled_name   "__pvcn_721_"
#define bisenet_pelee_picinfo__prim_split_5___pvcn_721__is_constant     1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_721__byte_offset     232 /* 0x000000e8 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_721__vbase_byte_offset   236 /* 0x000000ec */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_721__dbase_byte_offset   260 /* 0x00000104 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_721__dpitchm1_byte_offset    264 /* 0x00000108 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_721__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_721__W   176
#define bisenet_pelee_picinfo__prim_split_5___pvcn_721__H   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_721__D   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_721__P   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_721__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_721__dpitch_num_bytes    192
#define bisenet_pelee_picinfo__prim_split_5___pvcn_721__dram_format     0
#define bisenet_pelee_picinfo__prim_split_5___pvcn_721__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_5___pvcn_721__tile_width  1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_721__tile_height     1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_721__vmem_buffer_num_bytes   176

/* bisenet_pelee_picinfo__prim_split_5 SMB_input __pvcn_725_ */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_725__cnngen_demangled_name   "__pvcn_725_"
#define bisenet_pelee_picinfo__prim_split_5___pvcn_725__is_constant     1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_725__byte_offset     272 /* 0x00000110 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_725__vbase_byte_offset   276 /* 0x00000114 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_725__dbase_byte_offset   300 /* 0x0000012c */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_725__dpitchm1_byte_offset    304 /* 0x00000130 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_725__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_725__W   532
#define bisenet_pelee_picinfo__prim_split_5___pvcn_725__H   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_725__D   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_725__P   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_725__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_725__dpitch_num_bytes    544
#define bisenet_pelee_picinfo__prim_split_5___pvcn_725__dram_format     0
#define bisenet_pelee_picinfo__prim_split_5___pvcn_725__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_5___pvcn_725__tile_width  1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_725__tile_height     1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_725__vmem_buffer_num_bytes   532

/* bisenet_pelee_picinfo__prim_split_5 SMB_input __pvcn_726_ */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_726__cnngen_demangled_name   "__pvcn_726_"
#define bisenet_pelee_picinfo__prim_split_5___pvcn_726__is_constant     1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_726__byte_offset     312 /* 0x00000138 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_726__vbase_byte_offset   316 /* 0x0000013c */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_726__dbase_byte_offset   340 /* 0x00000154 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_726__dpitchm1_byte_offset    344 /* 0x00000158 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_726__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_726__W   176
#define bisenet_pelee_picinfo__prim_split_5___pvcn_726__H   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_726__D   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_726__P   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_726__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_726__dpitch_num_bytes    192
#define bisenet_pelee_picinfo__prim_split_5___pvcn_726__dram_format     0
#define bisenet_pelee_picinfo__prim_split_5___pvcn_726__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_5___pvcn_726__tile_width  1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_726__tile_height     1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_726__vmem_buffer_num_bytes   176

/* bisenet_pelee_picinfo__prim_split_5 SMB_input __pvcn_730_ */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_730__cnngen_demangled_name   "__pvcn_730_"
#define bisenet_pelee_picinfo__prim_split_5___pvcn_730__is_constant     1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_730__byte_offset     352 /* 0x00000160 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_730__vbase_byte_offset   356 /* 0x00000164 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_730__dbase_byte_offset   380 /* 0x0000017c */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_730__dpitchm1_byte_offset    384 /* 0x00000180 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_730__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_730__W   532
#define bisenet_pelee_picinfo__prim_split_5___pvcn_730__H   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_730__D   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_730__P   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_730__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_730__dpitch_num_bytes    544
#define bisenet_pelee_picinfo__prim_split_5___pvcn_730__dram_format     0
#define bisenet_pelee_picinfo__prim_split_5___pvcn_730__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_5___pvcn_730__tile_width  1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_730__tile_height     1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_730__vmem_buffer_num_bytes   532

/* bisenet_pelee_picinfo__prim_split_5 SMB_input __pvcn_731_ */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_731__cnngen_demangled_name   "__pvcn_731_"
#define bisenet_pelee_picinfo__prim_split_5___pvcn_731__is_constant     1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_731__byte_offset     392 /* 0x00000188 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_731__vbase_byte_offset   396 /* 0x0000018c */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_731__dbase_byte_offset   420 /* 0x000001a4 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_731__dpitchm1_byte_offset    424 /* 0x000001a8 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_731__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_731__W   176
#define bisenet_pelee_picinfo__prim_split_5___pvcn_731__H   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_731__D   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_731__P   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_731__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_731__dpitch_num_bytes    192
#define bisenet_pelee_picinfo__prim_split_5___pvcn_731__dram_format     0
#define bisenet_pelee_picinfo__prim_split_5___pvcn_731__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_5___pvcn_731__tile_width  1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_731__tile_height     1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_731__vmem_buffer_num_bytes   176

/* bisenet_pelee_picinfo__prim_split_5 SMB_input __pvcn_735_ */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_735__cnngen_demangled_name   "__pvcn_735_"
#define bisenet_pelee_picinfo__prim_split_5___pvcn_735__is_constant     1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_735__byte_offset     432 /* 0x000001b0 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_735__vbase_byte_offset   436 /* 0x000001b4 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_735__dbase_byte_offset   460 /* 0x000001cc */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_735__dpitchm1_byte_offset    464 /* 0x000001d0 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_735__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_735__W   532
#define bisenet_pelee_picinfo__prim_split_5___pvcn_735__H   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_735__D   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_735__P   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_735__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_735__dpitch_num_bytes    544
#define bisenet_pelee_picinfo__prim_split_5___pvcn_735__dram_format     0
#define bisenet_pelee_picinfo__prim_split_5___pvcn_735__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_5___pvcn_735__tile_width  1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_735__tile_height     1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_735__vmem_buffer_num_bytes   532

/* bisenet_pelee_picinfo__prim_split_5 SMB_input __pvcn_736_ */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_736__cnngen_demangled_name   "__pvcn_736_"
#define bisenet_pelee_picinfo__prim_split_5___pvcn_736__is_constant     1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_736__byte_offset     472 /* 0x000001d8 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_736__vbase_byte_offset   476 /* 0x000001dc */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_736__dbase_byte_offset   500 /* 0x000001f4 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_736__dpitchm1_byte_offset    504 /* 0x000001f8 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_736__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_736__W   176
#define bisenet_pelee_picinfo__prim_split_5___pvcn_736__H   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_736__D   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_736__P   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_736__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_736__dpitch_num_bytes    192
#define bisenet_pelee_picinfo__prim_split_5___pvcn_736__dram_format     0
#define bisenet_pelee_picinfo__prim_split_5___pvcn_736__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_5___pvcn_736__tile_width  1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_736__tile_height     1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_736__vmem_buffer_num_bytes   176

/* bisenet_pelee_picinfo__prim_split_5 SMB_input __pvcn_740_ */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_740__cnngen_demangled_name   "__pvcn_740_"
#define bisenet_pelee_picinfo__prim_split_5___pvcn_740__is_constant     1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_740__byte_offset     512 /* 0x00000200 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_740__vbase_byte_offset   516 /* 0x00000204 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_740__dbase_byte_offset   540 /* 0x0000021c */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_740__dpitchm1_byte_offset    544 /* 0x00000220 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_740__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_740__W   532
#define bisenet_pelee_picinfo__prim_split_5___pvcn_740__H   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_740__D   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_740__P   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_740__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_740__dpitch_num_bytes    544
#define bisenet_pelee_picinfo__prim_split_5___pvcn_740__dram_format     0
#define bisenet_pelee_picinfo__prim_split_5___pvcn_740__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_5___pvcn_740__tile_width  1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_740__tile_height     1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_740__vmem_buffer_num_bytes   532

/* bisenet_pelee_picinfo__prim_split_5 SMB_input __pvcn_741_ */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_741__cnngen_demangled_name   "__pvcn_741_"
#define bisenet_pelee_picinfo__prim_split_5___pvcn_741__is_constant     1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_741__byte_offset     552 /* 0x00000228 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_741__vbase_byte_offset   556 /* 0x0000022c */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_741__dbase_byte_offset   580 /* 0x00000244 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_741__dpitchm1_byte_offset    584 /* 0x00000248 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_741__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_741__W   176
#define bisenet_pelee_picinfo__prim_split_5___pvcn_741__H   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_741__D   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_741__P   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_741__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_741__dpitch_num_bytes    192
#define bisenet_pelee_picinfo__prim_split_5___pvcn_741__dram_format     0
#define bisenet_pelee_picinfo__prim_split_5___pvcn_741__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_5___pvcn_741__tile_width  1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_741__tile_height     1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_741__vmem_buffer_num_bytes   176

/* bisenet_pelee_picinfo__prim_split_5 SMB_input __pvcn_745_ */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_745__cnngen_demangled_name   "__pvcn_745_"
#define bisenet_pelee_picinfo__prim_split_5___pvcn_745__is_constant     1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_745__byte_offset     592 /* 0x00000250 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_745__vbase_byte_offset   596 /* 0x00000254 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_745__dbase_byte_offset   620 /* 0x0000026c */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_745__dpitchm1_byte_offset    624 /* 0x00000270 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_745__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_745__W   532
#define bisenet_pelee_picinfo__prim_split_5___pvcn_745__H   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_745__D   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_745__P   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_745__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_745__dpitch_num_bytes    544
#define bisenet_pelee_picinfo__prim_split_5___pvcn_745__dram_format     0
#define bisenet_pelee_picinfo__prim_split_5___pvcn_745__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_5___pvcn_745__tile_width  1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_745__tile_height     1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_745__vmem_buffer_num_bytes   532

/* bisenet_pelee_picinfo__prim_split_5 SMB_input __pvcn_746_ */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_746__cnngen_demangled_name   "__pvcn_746_"
#define bisenet_pelee_picinfo__prim_split_5___pvcn_746__is_constant     1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_746__byte_offset     632 /* 0x00000278 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_746__vbase_byte_offset   636 /* 0x0000027c */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_746__dbase_byte_offset   660 /* 0x00000294 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_746__dpitchm1_byte_offset    664 /* 0x00000298 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_746__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_746__W   176
#define bisenet_pelee_picinfo__prim_split_5___pvcn_746__H   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_746__D   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_746__P   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_746__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_746__dpitch_num_bytes    192
#define bisenet_pelee_picinfo__prim_split_5___pvcn_746__dram_format     0
#define bisenet_pelee_picinfo__prim_split_5___pvcn_746__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_5___pvcn_746__tile_width  1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_746__tile_height     1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_746__vmem_buffer_num_bytes   176

/* bisenet_pelee_picinfo__prim_split_5 SMB_input __pvcn_750_ */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_750__cnngen_demangled_name   "__pvcn_750_"
#define bisenet_pelee_picinfo__prim_split_5___pvcn_750__is_constant     1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_750__byte_offset     672 /* 0x000002a0 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_750__vbase_byte_offset   676 /* 0x000002a4 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_750__dbase_byte_offset   700 /* 0x000002bc */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_750__dpitchm1_byte_offset    704 /* 0x000002c0 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_750__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_750__W   532
#define bisenet_pelee_picinfo__prim_split_5___pvcn_750__H   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_750__D   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_750__P   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_750__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_750__dpitch_num_bytes    544
#define bisenet_pelee_picinfo__prim_split_5___pvcn_750__dram_format     0
#define bisenet_pelee_picinfo__prim_split_5___pvcn_750__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_5___pvcn_750__tile_width  1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_750__tile_height     1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_750__vmem_buffer_num_bytes   532

/* bisenet_pelee_picinfo__prim_split_5 SMB_input __pvcn_751_ */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_751__cnngen_demangled_name   "__pvcn_751_"
#define bisenet_pelee_picinfo__prim_split_5___pvcn_751__is_constant     1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_751__byte_offset     712 /* 0x000002c8 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_751__vbase_byte_offset   716 /* 0x000002cc */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_751__dbase_byte_offset   740 /* 0x000002e4 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_751__dpitchm1_byte_offset    744 /* 0x000002e8 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_751__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_751__W   176
#define bisenet_pelee_picinfo__prim_split_5___pvcn_751__H   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_751__D   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_751__P   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_751__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_751__dpitch_num_bytes    192
#define bisenet_pelee_picinfo__prim_split_5___pvcn_751__dram_format     0
#define bisenet_pelee_picinfo__prim_split_5___pvcn_751__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_5___pvcn_751__tile_width  1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_751__tile_height     1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_751__vmem_buffer_num_bytes   176

/* bisenet_pelee_picinfo__prim_split_5 SMB_input __pvcn_769_ */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_769__cnngen_demangled_name   "__pvcn_769_"
#define bisenet_pelee_picinfo__prim_split_5___pvcn_769__is_constant     1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_769__byte_offset     776 /* 0x00000308 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_769__vbase_byte_offset   780 /* 0x0000030c */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_769__dbase_byte_offset   804 /* 0x00000324 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_769__dpitchm1_byte_offset    808 /* 0x00000328 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_769__dpitchm1_bsize  32  /* 0x00000020 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_769__W   152648
#define bisenet_pelee_picinfo__prim_split_5___pvcn_769__H   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_769__D   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_769__P   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_769__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_769__dpitch_num_bytes    152672
#define bisenet_pelee_picinfo__prim_split_5___pvcn_769__dram_format     0
#define bisenet_pelee_picinfo__prim_split_5___pvcn_769__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_5___pvcn_769__tile_width  1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_769__tile_height     1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_769__vmem_buffer_num_bytes   152648

/* bisenet_pelee_picinfo__prim_split_5 SMB_input __pvcn_770_ */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_770__cnngen_demangled_name   "__pvcn_770_"
#define bisenet_pelee_picinfo__prim_split_5___pvcn_770__is_constant     1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_770__byte_offset     816 /* 0x00000330 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_770__vbase_byte_offset   820 /* 0x00000334 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_770__dbase_byte_offset   844 /* 0x0000034c */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_770__dpitchm1_byte_offset    848 /* 0x00000350 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_770__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_770__W   1408
#define bisenet_pelee_picinfo__prim_split_5___pvcn_770__H   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_770__D   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_770__P   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_770__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_770__dpitch_num_bytes    1408
#define bisenet_pelee_picinfo__prim_split_5___pvcn_770__dram_format     0
#define bisenet_pelee_picinfo__prim_split_5___pvcn_770__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_5___pvcn_770__tile_width  1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_770__tile_height     1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_770__vmem_buffer_num_bytes   1408

/* bisenet_pelee_picinfo__prim_split_5 SMB_input __pvcn_773_ */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_773__cnngen_demangled_name   "__pvcn_773_"
#define bisenet_pelee_picinfo__prim_split_5___pvcn_773__is_constant     1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_773__byte_offset     856 /* 0x00000358 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_773__vbase_byte_offset   860 /* 0x0000035c */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_773__dbase_byte_offset   888 /* 0x00000378 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_773__dpitchm1_byte_offset    892 /* 0x0000037c */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_773__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_773__W   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_773__H   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_773__D   512
#define bisenet_pelee_picinfo__prim_split_5___pvcn_773__P   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_773__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_773__dpitch_num_bytes    32
#define bisenet_pelee_picinfo__prim_split_5___pvcn_773__dram_format     0
#define bisenet_pelee_picinfo__prim_split_5___pvcn_773__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_5___pvcn_773__tile_width  1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_773__tile_height     1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_773__vmem_buffer_num_bytes   2048

/* bisenet_pelee_picinfo__prim_split_5 SMB_input __pvcn_774_ */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_774__cnngen_demangled_name   "__pvcn_774_"
#define bisenet_pelee_picinfo__prim_split_5___pvcn_774__is_constant     1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_774__byte_offset     900 /* 0x00000384 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_774__vbase_byte_offset   904 /* 0x00000388 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_774__dbase_byte_offset   932 /* 0x000003a4 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_774__dpitchm1_byte_offset    936 /* 0x000003a8 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_774__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_774__W   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_774__H   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_774__D   512
#define bisenet_pelee_picinfo__prim_split_5___pvcn_774__P   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_774__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_774__dpitch_num_bytes    32
#define bisenet_pelee_picinfo__prim_split_5___pvcn_774__dram_format     0
#define bisenet_pelee_picinfo__prim_split_5___pvcn_774__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_5___pvcn_774__tile_width  1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_774__tile_height     1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_774__vmem_buffer_num_bytes   2048

/* bisenet_pelee_picinfo__prim_split_5 SMB_input __pvcn_776_ */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_776__cnngen_demangled_name   "__pvcn_776_"
#define bisenet_pelee_picinfo__prim_split_5___pvcn_776__is_constant     1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_776__byte_offset     944 /* 0x000003b0 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_776__vbase_byte_offset   948 /* 0x000003b4 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_776__dbase_byte_offset   972 /* 0x000003cc */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_776__dpitchm1_byte_offset    976 /* 0x000003d0 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_776__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_776__W   4
#define bisenet_pelee_picinfo__prim_split_5___pvcn_776__H   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_776__D   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_776__P   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_776__data_num_bytes  2
#define bisenet_pelee_picinfo__prim_split_5___pvcn_776__dpitch_num_bytes    32
#define bisenet_pelee_picinfo__prim_split_5___pvcn_776__dram_format     0
#define bisenet_pelee_picinfo__prim_split_5___pvcn_776__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_5___pvcn_776__tile_width  1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_776__tile_height     1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_776__vmem_buffer_num_bytes   8

/* bisenet_pelee_picinfo__prim_split_5 SMB_input __pvcn_777_ */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_777__cnngen_demangled_name   "__pvcn_777_"
#define bisenet_pelee_picinfo__prim_split_5___pvcn_777__is_constant     1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_777__byte_offset     984 /* 0x000003d8 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_777__vbase_byte_offset   988 /* 0x000003dc */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_777__dbase_byte_offset   1012    /* 0x000003f4 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_777__dpitchm1_byte_offset    1016    /* 0x000003f8 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_777__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_777__W   5
#define bisenet_pelee_picinfo__prim_split_5___pvcn_777__H   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_777__D   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_777__P   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_777__data_num_bytes  2
#define bisenet_pelee_picinfo__prim_split_5___pvcn_777__dpitch_num_bytes    32
#define bisenet_pelee_picinfo__prim_split_5___pvcn_777__dram_format     0
#define bisenet_pelee_picinfo__prim_split_5___pvcn_777__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_5___pvcn_777__tile_width  1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_777__tile_height     1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_777__vmem_buffer_num_bytes   12

/* bisenet_pelee_picinfo__prim_split_5 SMB_input __pvcn_798_ */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_798__cnngen_demangled_name   "__pvcn_798_"
#define bisenet_pelee_picinfo__prim_split_5___pvcn_798__is_constant     1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_798__byte_offset     1096    /* 0x00000448 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_798__vbase_byte_offset   1100    /* 0x0000044c */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_798__dbase_byte_offset   1124    /* 0x00000464 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_798__dpitchm1_byte_offset    1128    /* 0x00000468 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_798__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_798__W   4
#define bisenet_pelee_picinfo__prim_split_5___pvcn_798__H   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_798__D   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_798__P   1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_798__data_num_bytes  4
#define bisenet_pelee_picinfo__prim_split_5___pvcn_798__dpitch_num_bytes    32
#define bisenet_pelee_picinfo__prim_split_5___pvcn_798__dram_format     0
#define bisenet_pelee_picinfo__prim_split_5___pvcn_798__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_5___pvcn_798__tile_width  1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_798__tile_height     1
#define bisenet_pelee_picinfo__prim_split_5___pvcn_798__vmem_buffer_num_bytes   16

/* bisenet_pelee_picinfo__prim_split_5 VCB PeleeNet__stage_3_transition_layer__transition_layer_avgpool__AvgPool */
#define bisenet_pelee_picinfo__prim_split_5_PeleeNet__stage_3_transition_layer__transition_layer_avgpool__AvgPool_cnngen_demangled_name     "PeleeNet__stage_3_transition_layer__transition_layer_avgpool__AvgPool"
#define bisenet_pelee_picinfo__prim_split_5_PeleeNet__stage_3_transition_layer__transition_layer_avgpool__AvgPool_byte_offset   1136    /* 0x00000470 */
#define bisenet_pelee_picinfo__prim_split_5_PeleeNet__stage_3_transition_layer__transition_layer_avgpool__AvgPool_vbase_byte_offset     1140    /* 0x00000474 */
#define bisenet_pelee_picinfo__prim_split_5_PeleeNet__stage_3_transition_layer__transition_layer_avgpool__AvgPool_vmem_buffer_num_bytes     397312

/* bisenet_pelee_picinfo__prim_split_5 VCB PeleeNet__stage_3_transition_layer__transition_layer_conv0__Conv2D___BatchNorm__FusedBatchNorm___Relu */
#define bisenet_pelee_picinfo__prim_split_5_PeleeNet__stage_3_transition_layer__transition_layer_conv0__Conv2D___BatchNorm__FusedBatchNorm___Relu_cnngen_demangled_name     "PeleeNet__stage_3_transition_layer__transition_layer_conv0__Conv2D___BatchNorm__FusedBatchNorm___Relu"
#define bisenet_pelee_picinfo__prim_split_5_PeleeNet__stage_3_transition_layer__transition_layer_conv0__Conv2D___BatchNorm__FusedBatchNorm___Relu_byte_offset   1184    /* 0x000004a0 */
#define bisenet_pelee_picinfo__prim_split_5_PeleeNet__stage_3_transition_layer__transition_layer_conv0__Conv2D___BatchNorm__FusedBatchNorm___Relu_vbase_byte_offset     1192    /* 0x000004a8 */
#define bisenet_pelee_picinfo__prim_split_5_PeleeNet__stage_3_transition_layer__transition_layer_conv0__Conv2D___BatchNorm__FusedBatchNorm___Relu_vmem_buffer_num_bytes     348160

/* bisenet_pelee_picinfo__prim_split_5 VCB Mean_1_pre_trans */
#define bisenet_pelee_picinfo__prim_split_5_Mean_1_pre_trans_cnngen_demangled_name  "Mean_1_pre_trans"
#define bisenet_pelee_picinfo__prim_split_5_Mean_1_pre_trans_byte_offset    1500    /* 0x000005dc */
#define bisenet_pelee_picinfo__prim_split_5_Mean_1_pre_trans_vbase_byte_offset  1504    /* 0x000005e0 */
#define bisenet_pelee_picinfo__prim_split_5_Mean_1_pre_trans_vmem_buffer_num_bytes  512

/* bisenet_pelee_picinfo__prim_split_5 VCB __pvcn_252_ */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_252__cnngen_demangled_name   "__pvcn_252_"
#define bisenet_pelee_picinfo__prim_split_5___pvcn_252__byte_offset     1536    /* 0x00000600 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_252__vbase_byte_offset   1540    /* 0x00000604 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_252__vmem_buffer_num_bytes   512

/* bisenet_pelee_picinfo__prim_split_5 VCB Conv_4__Conv2D___dotib___389 */
#define bisenet_pelee_picinfo__prim_split_5_Conv_4__Conv2D___dotib___389_cnngen_demangled_name  "Conv_4__Conv2D___dotib___389"
#define bisenet_pelee_picinfo__prim_split_5_Conv_4__Conv2D___dotib___389_byte_offset    1572    /* 0x00000624 */
#define bisenet_pelee_picinfo__prim_split_5_Conv_4__Conv2D___dotib___389_vbase_byte_offset  1576    /* 0x00000628 */
#define bisenet_pelee_picinfo__prim_split_5_Conv_4__Conv2D___dotib___389_vmem_buffer_num_bytes  512

/* bisenet_pelee_picinfo__prim_split_5 VCB __pvcn_786_ */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_786__cnngen_demangled_name   "__pvcn_786_"
#define bisenet_pelee_picinfo__prim_split_5___pvcn_786__byte_offset     1780    /* 0x000006f4 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_786__vbase_byte_offset   1784    /* 0x000006f8 */
#define bisenet_pelee_picinfo__prim_split_5___pvcn_786__vmem_buffer_num_bytes   6144

/* bisenet_pelee_picinfo__prim_split_5 VCB Sigmoid_1___exp___138 */
#define bisenet_pelee_picinfo__prim_split_5_Sigmoid_1___exp___138_cnngen_demangled_name     "Sigmoid_1___exp___138"
#define bisenet_pelee_picinfo__prim_split_5_Sigmoid_1___exp___138_byte_offset   1820    /* 0x0000071c */
#define bisenet_pelee_picinfo__prim_split_5_Sigmoid_1___exp___138_vbase_byte_offset     1824    /* 0x00000720 */
#define bisenet_pelee_picinfo__prim_split_5_Sigmoid_1___exp___138_vmem_buffer_num_bytes     4096

/* bisenet_pelee_picinfo__prim_split_5 VCB Sigmoid_1___addi___139 */
#define bisenet_pelee_picinfo__prim_split_5_Sigmoid_1___addi___139_cnngen_demangled_name    "Sigmoid_1___addi___139"
#define bisenet_pelee_picinfo__prim_split_5_Sigmoid_1___addi___139_byte_offset  1864    /* 0x00000748 */
#define bisenet_pelee_picinfo__prim_split_5_Sigmoid_1___addi___139_vbase_byte_offset    1868    /* 0x0000074c */
#define bisenet_pelee_picinfo__prim_split_5_Sigmoid_1___addi___139_vmem_buffer_num_bytes    6144

/* bisenet_pelee_picinfo__prim_split_5 VCB Sigmoid_1___mul___141 */
#define bisenet_pelee_picinfo__prim_split_5_Sigmoid_1___mul___141_cnngen_demangled_name     "Sigmoid_1___mul___141"
#define bisenet_pelee_picinfo__prim_split_5_Sigmoid_1___mul___141_byte_offset   1936    /* 0x00000790 */
#define bisenet_pelee_picinfo__prim_split_5_Sigmoid_1___mul___141_vbase_byte_offset     1940    /* 0x00000794 */
#define bisenet_pelee_picinfo__prim_split_5_Sigmoid_1___mul___141_vmem_buffer_num_bytes     4096

/* bisenet_pelee_picinfo__prim_split_5 VCB Sigmoid_1 */
#define bisenet_pelee_picinfo__prim_split_5_Sigmoid_1_cnngen_demangled_name     "Sigmoid_1"
#define bisenet_pelee_picinfo__prim_split_5_Sigmoid_1_byte_offset   2012    /* 0x000007dc */
#define bisenet_pelee_picinfo__prim_split_5_Sigmoid_1_vbase_byte_offset     2016    /* 0x000007e0 */
#define bisenet_pelee_picinfo__prim_split_5_Sigmoid_1_vmem_buffer_num_bytes     10240

/* bisenet_pelee_picinfo__prim_split_5 VCB Mul_1 */
#define bisenet_pelee_picinfo__prim_split_5_Mul_1_cnngen_demangled_name     "Mul_1"
#define bisenet_pelee_picinfo__prim_split_5_Mul_1_byte_offset   2052    /* 0x00000804 */
#define bisenet_pelee_picinfo__prim_split_5_Mul_1_vbase_byte_offset     2056    /* 0x00000808 */
#define bisenet_pelee_picinfo__prim_split_5_Mul_1_vmem_buffer_num_bytes     331776

/* bisenet_pelee_picinfo__prim_split_5 VCB Mean_2_pre_trans */
#define bisenet_pelee_picinfo__prim_split_5_Mean_2_pre_trans_cnngen_demangled_name  "Mean_2_pre_trans"
#define bisenet_pelee_picinfo__prim_split_5_Mean_2_pre_trans_byte_offset    2132    /* 0x00000854 */
#define bisenet_pelee_picinfo__prim_split_5_Mean_2_pre_trans_vbase_byte_offset  2136    /* 0x00000858 */
#define bisenet_pelee_picinfo__prim_split_5_Mean_2_pre_trans_vmem_buffer_num_bytes  512

/* bisenet_pelee_picinfo__prim_split_5 VCB Mul_2 */
#define bisenet_pelee_picinfo__prim_split_5_Mul_2_cnngen_demangled_name     "Mul_2"
#define bisenet_pelee_picinfo__prim_split_5_Mul_2_byte_offset   2168    /* 0x00000878 */
#define bisenet_pelee_picinfo__prim_split_5_Mul_2_vbase_byte_offset     2176    /* 0x00000880 */
#define bisenet_pelee_picinfo__prim_split_5_Mul_2_vmem_buffer_num_bytes     122892

/* bisenet_pelee_picinfo__prim_split_5 HMB_output ResizeBilinear_1 */
#define bisenet_pelee_picinfo__prim_split_5_ResizeBilinear_1_cnngen_demangled_name  "ResizeBilinear_1"
#define bisenet_pelee_picinfo__prim_split_5_ResizeBilinear_1_byte_offset    2212    /* 0x000008a4 */
#define bisenet_pelee_picinfo__prim_split_5_ResizeBilinear_1_vbase_byte_offset  2216    /* 0x000008a8 */
#define bisenet_pelee_picinfo__prim_split_5_ResizeBilinear_1_dbase_byte_offset  2240    /* 0x000008c0 */
#define bisenet_pelee_picinfo__prim_split_5_ResizeBilinear_1_dpitchm1_byte_offset   2244    /* 0x000008c4 */
#define bisenet_pelee_picinfo__prim_split_5_ResizeBilinear_1_dpitchm1_bsize     16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_5_ResizeBilinear_1_vwidth_minus_one_byte_offset   2228    /* 0x000008b4 */
#define bisenet_pelee_picinfo__prim_split_5_ResizeBilinear_1_vwidth_minus_one_bsize     16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_5_ResizeBilinear_1_vheight_minus_one_byte_offset  2230    /* 0x000008b6 */
#define bisenet_pelee_picinfo__prim_split_5_ResizeBilinear_1_vheight_minus_one_bsize    16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_5_ResizeBilinear_1_drotate_bit_offset     17894   /* 0x000045e6 */
#define bisenet_pelee_picinfo__prim_split_5_ResizeBilinear_1_hflip_bit_offset   17895   /* 0x000045e7 */
#define bisenet_pelee_picinfo__prim_split_5_ResizeBilinear_1_vflip_bit_offset   17896   /* 0x000045e8 */
#define bisenet_pelee_picinfo__prim_split_5_ResizeBilinear_1_dflip_bit_offset   17897   /* 0x000045e9 */
#define bisenet_pelee_picinfo__prim_split_5_ResizeBilinear_1_pflip_bit_offset   17898   /* 0x000045ea */
#define bisenet_pelee_picinfo__prim_split_5_ResizeBilinear_1_W  160
#define bisenet_pelee_picinfo__prim_split_5_ResizeBilinear_1_H  64
#define bisenet_pelee_picinfo__prim_split_5_ResizeBilinear_1_D  512
#define bisenet_pelee_picinfo__prim_split_5_ResizeBilinear_1_P  1
#define bisenet_pelee_picinfo__prim_split_5_ResizeBilinear_1_data_num_bytes     2
#define bisenet_pelee_picinfo__prim_split_5_ResizeBilinear_1_dpitch_num_bytes   32
#define bisenet_pelee_picinfo__prim_split_5_ResizeBilinear_1_dram_format    3
#define bisenet_pelee_picinfo__prim_split_5_ResizeBilinear_1_vp_interleave_mode     2
#define bisenet_pelee_picinfo__prim_split_5_ResizeBilinear_1_tile_width     4
#define bisenet_pelee_picinfo__prim_split_5_ResizeBilinear_1_tile_height    1
#define bisenet_pelee_picinfo__prim_split_5_ResizeBilinear_1_vmem_buffer_num_bytes  335872
#define bisenet_pelee_picinfo__prim_split_5_ResizeBilinear_1_dram_size_num_bytes    10485760
#define bisenet_pelee_picinfo__prim_split_5_ResizeBilinear_1_sign   0
#define bisenet_pelee_picinfo__prim_split_5_ResizeBilinear_1_datasize   1
#define bisenet_pelee_picinfo__prim_split_5_ResizeBilinear_1_expoffset  12
#define bisenet_pelee_picinfo__prim_split_5_ResizeBilinear_1_expbits    0
#define bisenet_pelee_picinfo__prim_split_5_ResizeBilinear_1_drotate    0
#define bisenet_pelee_picinfo__prim_split_5_ResizeBilinear_1_hflip  0
#define bisenet_pelee_picinfo__prim_split_5_ResizeBilinear_1_vflip  0
#define bisenet_pelee_picinfo__prim_split_5_ResizeBilinear_1_dflip  0
#define bisenet_pelee_picinfo__prim_split_5_ResizeBilinear_1_pflip  0


#endif /* bisenet_pelee_picinfo__prim_split_5_H */
