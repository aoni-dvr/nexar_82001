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
#ifndef bisenet_pelee_picinfo__prim_split_3_H
#define bisenet_pelee_picinfo__prim_split_3_H

#define vas_gen_header_version                      5

/* dag name */
#define bisenet_pelee_picinfo__prim_split_3_DAG_NAME    "bisenet_pelee_picinfo__prim_split_3"

/* VDG file info */
#define bisenet_pelee_picinfo__prim_split_3_vdg_name    "bisenet_pelee_picinfo__split_3.vdg"

/* VDG DAG memory info */
#define bisenet_pelee_picinfo__prim_split_3_byte_offset     340 /* 0x00000154 */
#define bisenet_pelee_picinfo__prim_split_3_byte_size   1948    /* 0x0000079c */
#define bisenet_pelee_picinfo__prim_split_3_preferred_dram_xfer_size    1

/* bisenet_pelee_picinfo__prim_split_3 VMEM info */
#define bisenet_pelee_picinfo__prim_split_3_use_ping_pong_vmem  0
#define bisenet_pelee_picinfo__prim_split_3_VMEM_start  4096    /* 0x00001000 */
#define bisenet_pelee_picinfo__prim_split_3_VMEM_end    2068380 /* 0x001f8f9c */
#define bisenet_pelee_picinfo__prim_split_3_image_start     1942528 /* 0x001da400 */
#define bisenet_pelee_picinfo__prim_split_3_image_size  127800  /* 0x0001f338 */
#define bisenet_pelee_picinfo__prim_split_3_dagbin_start    2068380 /* 0x001f8f9c */

/*******************************************************************************
 * DAG byte offset info for HMB/SMB/VCB and their fields
 * in DAG 'bisenet_pelee_picinfo__prim_split_3' in source file 'bisenet_pelee_picinfo__split_3.vas'
 ******************************************************************************/
/* bisenet_pelee_picinfo__prim_split_3 HMB_input PeleeNet__stage_3_dense_block_1__concat */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_1__concat_cnngen_demangled_name   "PeleeNet__stage_3_dense_block_1__concat"
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_1__concat_is_constant     0
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_1__concat_ddi_byte_size   0   /* 0x00000000 */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_1__concat_ddi_byte_offset     4294967295  /* 0xffffffff */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_1__concat_byte_offset     0   /* 0x00000000 */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_1__concat_vbase_byte_offset   4   /* 0x00000004 */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_1__concat_dbase_byte_offset   28  /* 0x0000001c */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_1__concat_dpitchm1_byte_offset    32  /* 0x00000020 */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_1__concat_dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_1__concat_vwidth_minus_one_byte_offset    16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_1__concat_vwidth_minus_one_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_1__concat_vheight_minus_one_byte_offset   18  /* 0x00000012 */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_1__concat_vheight_minus_one_bsize     16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_1__concat_drotate_bit_offset  198 /* 0x000000c6 */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_1__concat_hflip_bit_offset    199 /* 0x000000c7 */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_1__concat_vflip_bit_offset    200 /* 0x000000c8 */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_1__concat_dflip_bit_offset    201 /* 0x000000c9 */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_1__concat_pflip_bit_offset    202 /* 0x000000ca */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_1__concat_W   80
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_1__concat_H   32
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_1__concat_D   320
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_1__concat_P   1
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_1__concat_data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_1__concat_dpitch_num_bytes    32
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_1__concat_dram_format     3
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_1__concat_vp_interleave_mode  2
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_1__concat_tile_width  4
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_1__concat_tile_height     1
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_1__concat_vmem_buffer_num_bytes   153600
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_1__concat_dram_size_num_bytes     819200
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_1__concat_sign    0
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_1__concat_datasize    0
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_1__concat_expoffset   5
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_1__concat_expbits     0
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_1__concat_drotate     0
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_1__concat_hflip   0
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_1__concat_vflip   0
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_1__concat_dflip   0
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_1__concat_pflip   0

/* bisenet_pelee_picinfo__prim_split_3 SMB_input __pvcn_585_ */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_585__cnngen_demangled_name   "__pvcn_585_"
#define bisenet_pelee_picinfo__prim_split_3___pvcn_585__is_constant     1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_585__byte_offset     40  /* 0x00000028 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_585__vbase_byte_offset   44  /* 0x0000002c */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_585__dbase_byte_offset   68  /* 0x00000044 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_585__dpitchm1_byte_offset    72  /* 0x00000048 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_585__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_585__W   25568
#define bisenet_pelee_picinfo__prim_split_3___pvcn_585__H   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_585__D   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_585__P   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_585__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_585__dpitch_num_bytes    25568
#define bisenet_pelee_picinfo__prim_split_3___pvcn_585__dram_format     0
#define bisenet_pelee_picinfo__prim_split_3___pvcn_585__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_3___pvcn_585__tile_width  1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_585__tile_height     1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_585__vmem_buffer_num_bytes   25568

/* bisenet_pelee_picinfo__prim_split_3 SMB_input __pvcn_586_ */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_586__cnngen_demangled_name   "__pvcn_586_"
#define bisenet_pelee_picinfo__prim_split_3___pvcn_586__is_constant     1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_586__byte_offset     76  /* 0x0000004c */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_586__vbase_byte_offset   80  /* 0x00000050 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_586__dbase_byte_offset   104 /* 0x00000068 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_586__dpitchm1_byte_offset    108 /* 0x0000006c */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_586__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_586__W   640
#define bisenet_pelee_picinfo__prim_split_3___pvcn_586__H   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_586__D   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_586__P   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_586__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_586__dpitch_num_bytes    640
#define bisenet_pelee_picinfo__prim_split_3___pvcn_586__dram_format     0
#define bisenet_pelee_picinfo__prim_split_3___pvcn_586__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_3___pvcn_586__tile_width  1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_586__tile_height     1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_586__vmem_buffer_num_bytes   640

/* bisenet_pelee_picinfo__prim_split_3 SMB_input __pvcn_589_ */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_589__cnngen_demangled_name   "__pvcn_589_"
#define bisenet_pelee_picinfo__prim_split_3___pvcn_589__is_constant     1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_589__byte_offset     116 /* 0x00000074 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_589__vbase_byte_offset   120 /* 0x00000078 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_589__dbase_byte_offset   144 /* 0x00000090 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_589__dpitchm1_byte_offset    148 /* 0x00000094 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_589__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_589__W   5784
#define bisenet_pelee_picinfo__prim_split_3___pvcn_589__H   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_589__D   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_589__P   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_589__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_589__dpitch_num_bytes    5792
#define bisenet_pelee_picinfo__prim_split_3___pvcn_589__dram_format     0
#define bisenet_pelee_picinfo__prim_split_3___pvcn_589__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_3___pvcn_589__tile_width  1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_589__tile_height     1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_589__vmem_buffer_num_bytes   5784

/* bisenet_pelee_picinfo__prim_split_3 SMB_input __pvcn_590_ */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_590__cnngen_demangled_name   "__pvcn_590_"
#define bisenet_pelee_picinfo__prim_split_3___pvcn_590__is_constant     1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_590__byte_offset     156 /* 0x0000009c */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_590__vbase_byte_offset   160 /* 0x000000a0 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_590__dbase_byte_offset   184 /* 0x000000b8 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_590__dpitchm1_byte_offset    188 /* 0x000000bc */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_590__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_590__W   80
#define bisenet_pelee_picinfo__prim_split_3___pvcn_590__H   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_590__D   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_590__P   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_590__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_590__dpitch_num_bytes    96
#define bisenet_pelee_picinfo__prim_split_3___pvcn_590__dram_format     0
#define bisenet_pelee_picinfo__prim_split_3___pvcn_590__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_3___pvcn_590__tile_width  1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_590__tile_height     1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_590__vmem_buffer_num_bytes   80

/* bisenet_pelee_picinfo__prim_split_3 SMB_input __pvcn_593_ */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_593__cnngen_demangled_name   "__pvcn_593_"
#define bisenet_pelee_picinfo__prim_split_3___pvcn_593__is_constant     1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_593__byte_offset     196 /* 0x000000c4 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_593__vbase_byte_offset   200 /* 0x000000c8 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_593__dbase_byte_offset   224 /* 0x000000e0 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_593__dpitchm1_byte_offset    228 /* 0x000000e4 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_593__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_593__W   5660
#define bisenet_pelee_picinfo__prim_split_3___pvcn_593__H   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_593__D   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_593__P   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_593__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_593__dpitch_num_bytes    5664
#define bisenet_pelee_picinfo__prim_split_3___pvcn_593__dram_format     0
#define bisenet_pelee_picinfo__prim_split_3___pvcn_593__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_3___pvcn_593__tile_width  1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_593__tile_height     1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_593__vmem_buffer_num_bytes   5660

/* bisenet_pelee_picinfo__prim_split_3 SMB_input __pvcn_594_ */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_594__cnngen_demangled_name   "__pvcn_594_"
#define bisenet_pelee_picinfo__prim_split_3___pvcn_594__is_constant     1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_594__byte_offset     236 /* 0x000000ec */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_594__vbase_byte_offset   240 /* 0x000000f0 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_594__dbase_byte_offset   264 /* 0x00000108 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_594__dpitchm1_byte_offset    268 /* 0x0000010c */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_594__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_594__W   80
#define bisenet_pelee_picinfo__prim_split_3___pvcn_594__H   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_594__D   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_594__P   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_594__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_594__dpitch_num_bytes    96
#define bisenet_pelee_picinfo__prim_split_3___pvcn_594__dram_format     0
#define bisenet_pelee_picinfo__prim_split_3___pvcn_594__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_3___pvcn_594__tile_width  1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_594__tile_height     1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_594__vmem_buffer_num_bytes   80

/* bisenet_pelee_picinfo__prim_split_3 SMB_input __pvcn_596_ */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_596__cnngen_demangled_name   "__pvcn_596_"
#define bisenet_pelee_picinfo__prim_split_3___pvcn_596__is_constant     1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_596__byte_offset     276 /* 0x00000114 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_596__vbase_byte_offset   280 /* 0x00000118 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_596__dbase_byte_offset   304 /* 0x00000130 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_596__dpitchm1_byte_offset    308 /* 0x00000134 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_596__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_596__W   1436
#define bisenet_pelee_picinfo__prim_split_3___pvcn_596__H   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_596__D   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_596__P   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_596__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_596__dpitch_num_bytes    1440
#define bisenet_pelee_picinfo__prim_split_3___pvcn_596__dram_format     0
#define bisenet_pelee_picinfo__prim_split_3___pvcn_596__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_3___pvcn_596__tile_width  1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_596__tile_height     1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_596__vmem_buffer_num_bytes   1436

/* bisenet_pelee_picinfo__prim_split_3 SMB_input __pvcn_597_ */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_597__cnngen_demangled_name   "__pvcn_597_"
#define bisenet_pelee_picinfo__prim_split_3___pvcn_597__is_constant     1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_597__byte_offset     316 /* 0x0000013c */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_597__vbase_byte_offset   320 /* 0x00000140 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_597__dbase_byte_offset   344 /* 0x00000158 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_597__dpitchm1_byte_offset    348 /* 0x0000015c */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_597__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_597__W   80
#define bisenet_pelee_picinfo__prim_split_3___pvcn_597__H   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_597__D   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_597__P   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_597__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_597__dpitch_num_bytes    96
#define bisenet_pelee_picinfo__prim_split_3___pvcn_597__dram_format     0
#define bisenet_pelee_picinfo__prim_split_3___pvcn_597__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_3___pvcn_597__tile_width  1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_597__tile_height     1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_597__vmem_buffer_num_bytes   80

/* bisenet_pelee_picinfo__prim_split_3 SMB_input __pvcn_603_ */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_603__cnngen_demangled_name   "__pvcn_603_"
#define bisenet_pelee_picinfo__prim_split_3___pvcn_603__is_constant     1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_603__byte_offset     356 /* 0x00000164 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_603__vbase_byte_offset   360 /* 0x00000168 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_603__dbase_byte_offset   384 /* 0x00000180 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_603__dpitchm1_byte_offset    388 /* 0x00000184 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_603__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_603__W   28200
#define bisenet_pelee_picinfo__prim_split_3___pvcn_603__H   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_603__D   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_603__P   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_603__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_603__dpitch_num_bytes    28224
#define bisenet_pelee_picinfo__prim_split_3___pvcn_603__dram_format     0
#define bisenet_pelee_picinfo__prim_split_3___pvcn_603__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_3___pvcn_603__tile_width  1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_603__tile_height     1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_603__vmem_buffer_num_bytes   28200

/* bisenet_pelee_picinfo__prim_split_3 SMB_input __pvcn_604_ */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_604__cnngen_demangled_name   "__pvcn_604_"
#define bisenet_pelee_picinfo__prim_split_3___pvcn_604__is_constant     1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_604__byte_offset     396 /* 0x0000018c */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_604__vbase_byte_offset   400 /* 0x00000190 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_604__dbase_byte_offset   424 /* 0x000001a8 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_604__dpitchm1_byte_offset    428 /* 0x000001ac */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_604__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_604__W   640
#define bisenet_pelee_picinfo__prim_split_3___pvcn_604__H   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_604__D   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_604__P   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_604__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_604__dpitch_num_bytes    640
#define bisenet_pelee_picinfo__prim_split_3___pvcn_604__dram_format     0
#define bisenet_pelee_picinfo__prim_split_3___pvcn_604__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_3___pvcn_604__tile_width  1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_604__tile_height     1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_604__vmem_buffer_num_bytes   640

/* bisenet_pelee_picinfo__prim_split_3 SMB_input __pvcn_607_ */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_607__cnngen_demangled_name   "__pvcn_607_"
#define bisenet_pelee_picinfo__prim_split_3___pvcn_607__is_constant     1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_607__byte_offset     436 /* 0x000001b4 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_607__vbase_byte_offset   440 /* 0x000001b8 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_607__dbase_byte_offset   464 /* 0x000001d0 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_607__dpitchm1_byte_offset    468 /* 0x000001d4 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_607__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_607__W   5736
#define bisenet_pelee_picinfo__prim_split_3___pvcn_607__H   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_607__D   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_607__P   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_607__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_607__dpitch_num_bytes    5760
#define bisenet_pelee_picinfo__prim_split_3___pvcn_607__dram_format     0
#define bisenet_pelee_picinfo__prim_split_3___pvcn_607__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_3___pvcn_607__tile_width  1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_607__tile_height     1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_607__vmem_buffer_num_bytes   5736

/* bisenet_pelee_picinfo__prim_split_3 SMB_input __pvcn_608_ */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_608__cnngen_demangled_name   "__pvcn_608_"
#define bisenet_pelee_picinfo__prim_split_3___pvcn_608__is_constant     1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_608__byte_offset     476 /* 0x000001dc */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_608__vbase_byte_offset   480 /* 0x000001e0 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_608__dbase_byte_offset   504 /* 0x000001f8 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_608__dpitchm1_byte_offset    508 /* 0x000001fc */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_608__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_608__W   80
#define bisenet_pelee_picinfo__prim_split_3___pvcn_608__H   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_608__D   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_608__P   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_608__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_608__dpitch_num_bytes    96
#define bisenet_pelee_picinfo__prim_split_3___pvcn_608__dram_format     0
#define bisenet_pelee_picinfo__prim_split_3___pvcn_608__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_3___pvcn_608__tile_width  1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_608__tile_height     1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_608__vmem_buffer_num_bytes   80

/* bisenet_pelee_picinfo__prim_split_3 SMB_input __pvcn_611_ */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_611__cnngen_demangled_name   "__pvcn_611_"
#define bisenet_pelee_picinfo__prim_split_3___pvcn_611__is_constant     1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_611__byte_offset     516 /* 0x00000204 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_611__vbase_byte_offset   520 /* 0x00000208 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_611__dbase_byte_offset   544 /* 0x00000220 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_611__dpitchm1_byte_offset    548 /* 0x00000224 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_611__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_611__W   5748
#define bisenet_pelee_picinfo__prim_split_3___pvcn_611__H   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_611__D   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_611__P   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_611__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_611__dpitch_num_bytes    5760
#define bisenet_pelee_picinfo__prim_split_3___pvcn_611__dram_format     0
#define bisenet_pelee_picinfo__prim_split_3___pvcn_611__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_3___pvcn_611__tile_width  1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_611__tile_height     1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_611__vmem_buffer_num_bytes   5748

/* bisenet_pelee_picinfo__prim_split_3 SMB_input __pvcn_612_ */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_612__cnngen_demangled_name   "__pvcn_612_"
#define bisenet_pelee_picinfo__prim_split_3___pvcn_612__is_constant     1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_612__byte_offset     556 /* 0x0000022c */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_612__vbase_byte_offset   560 /* 0x00000230 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_612__dbase_byte_offset   584 /* 0x00000248 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_612__dpitchm1_byte_offset    588 /* 0x0000024c */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_612__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_612__W   88
#define bisenet_pelee_picinfo__prim_split_3___pvcn_612__H   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_612__D   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_612__P   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_612__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_612__dpitch_num_bytes    96
#define bisenet_pelee_picinfo__prim_split_3___pvcn_612__dram_format     0
#define bisenet_pelee_picinfo__prim_split_3___pvcn_612__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_3___pvcn_612__tile_width  1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_612__tile_height     1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_612__vmem_buffer_num_bytes   88

/* bisenet_pelee_picinfo__prim_split_3 SMB_input __pvcn_614_ */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_614__cnngen_demangled_name   "__pvcn_614_"
#define bisenet_pelee_picinfo__prim_split_3___pvcn_614__is_constant     1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_614__byte_offset     596 /* 0x00000254 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_614__vbase_byte_offset   600 /* 0x00000258 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_614__dbase_byte_offset   624 /* 0x00000270 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_614__dpitchm1_byte_offset    628 /* 0x00000274 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_614__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_614__W   1440
#define bisenet_pelee_picinfo__prim_split_3___pvcn_614__H   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_614__D   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_614__P   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_614__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_614__dpitch_num_bytes    1440
#define bisenet_pelee_picinfo__prim_split_3___pvcn_614__dram_format     0
#define bisenet_pelee_picinfo__prim_split_3___pvcn_614__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_3___pvcn_614__tile_width  1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_614__tile_height     1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_614__vmem_buffer_num_bytes   1440

/* bisenet_pelee_picinfo__prim_split_3 SMB_input __pvcn_615_ */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_615__cnngen_demangled_name   "__pvcn_615_"
#define bisenet_pelee_picinfo__prim_split_3___pvcn_615__is_constant     1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_615__byte_offset     636 /* 0x0000027c */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_615__vbase_byte_offset   640 /* 0x00000280 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_615__dbase_byte_offset   664 /* 0x00000298 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_615__dpitchm1_byte_offset    668 /* 0x0000029c */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_615__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_615__W   88
#define bisenet_pelee_picinfo__prim_split_3___pvcn_615__H   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_615__D   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_615__P   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_615__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_615__dpitch_num_bytes    96
#define bisenet_pelee_picinfo__prim_split_3___pvcn_615__dram_format     0
#define bisenet_pelee_picinfo__prim_split_3___pvcn_615__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_3___pvcn_615__tile_width  1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_615__tile_height     1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_615__vmem_buffer_num_bytes   88

/* bisenet_pelee_picinfo__prim_split_3 SMB_input __pvcn_621_ */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_621__cnngen_demangled_name   "__pvcn_621_"
#define bisenet_pelee_picinfo__prim_split_3___pvcn_621__is_constant     1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_621__byte_offset     676 /* 0x000002a4 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_621__vbase_byte_offset   680 /* 0x000002a8 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_621__dbase_byte_offset   704 /* 0x000002c0 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_621__dpitchm1_byte_offset    708 /* 0x000002c4 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_621__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_621__W   30664
#define bisenet_pelee_picinfo__prim_split_3___pvcn_621__H   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_621__D   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_621__P   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_621__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_621__dpitch_num_bytes    30688
#define bisenet_pelee_picinfo__prim_split_3___pvcn_621__dram_format     0
#define bisenet_pelee_picinfo__prim_split_3___pvcn_621__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_3___pvcn_621__tile_width  1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_621__tile_height     1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_621__vmem_buffer_num_bytes   30664

/* bisenet_pelee_picinfo__prim_split_3 SMB_input __pvcn_622_ */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_622__cnngen_demangled_name   "__pvcn_622_"
#define bisenet_pelee_picinfo__prim_split_3___pvcn_622__is_constant     1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_622__byte_offset     716 /* 0x000002cc */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_622__vbase_byte_offset   720 /* 0x000002d0 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_622__dbase_byte_offset   744 /* 0x000002e8 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_622__dpitchm1_byte_offset    748 /* 0x000002ec */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_622__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_622__W   640
#define bisenet_pelee_picinfo__prim_split_3___pvcn_622__H   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_622__D   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_622__P   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_622__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_622__dpitch_num_bytes    640
#define bisenet_pelee_picinfo__prim_split_3___pvcn_622__dram_format     0
#define bisenet_pelee_picinfo__prim_split_3___pvcn_622__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_3___pvcn_622__tile_width  1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_622__tile_height     1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_622__vmem_buffer_num_bytes   640

/* bisenet_pelee_picinfo__prim_split_3 SMB_input __pvcn_625_ */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_625__cnngen_demangled_name   "__pvcn_625_"
#define bisenet_pelee_picinfo__prim_split_3___pvcn_625__is_constant     1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_625__byte_offset     756 /* 0x000002f4 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_625__vbase_byte_offset   760 /* 0x000002f8 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_625__dbase_byte_offset   784 /* 0x00000310 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_625__dpitchm1_byte_offset    788 /* 0x00000314 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_625__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_625__W   5780
#define bisenet_pelee_picinfo__prim_split_3___pvcn_625__H   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_625__D   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_625__P   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_625__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_625__dpitch_num_bytes    5792
#define bisenet_pelee_picinfo__prim_split_3___pvcn_625__dram_format     0
#define bisenet_pelee_picinfo__prim_split_3___pvcn_625__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_3___pvcn_625__tile_width  1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_625__tile_height     1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_625__vmem_buffer_num_bytes   5780

/* bisenet_pelee_picinfo__prim_split_3 SMB_input __pvcn_626_ */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_626__cnngen_demangled_name   "__pvcn_626_"
#define bisenet_pelee_picinfo__prim_split_3___pvcn_626__is_constant     1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_626__byte_offset     796 /* 0x0000031c */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_626__vbase_byte_offset   800 /* 0x00000320 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_626__dbase_byte_offset   824 /* 0x00000338 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_626__dpitchm1_byte_offset    828 /* 0x0000033c */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_626__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_626__W   88
#define bisenet_pelee_picinfo__prim_split_3___pvcn_626__H   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_626__D   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_626__P   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_626__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_626__dpitch_num_bytes    96
#define bisenet_pelee_picinfo__prim_split_3___pvcn_626__dram_format     0
#define bisenet_pelee_picinfo__prim_split_3___pvcn_626__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_3___pvcn_626__tile_width  1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_626__tile_height     1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_626__vmem_buffer_num_bytes   88

/* bisenet_pelee_picinfo__prim_split_3 SMB_input __pvcn_629_ */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_629__cnngen_demangled_name   "__pvcn_629_"
#define bisenet_pelee_picinfo__prim_split_3___pvcn_629__is_constant     1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_629__byte_offset     836 /* 0x00000344 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_629__vbase_byte_offset   840 /* 0x00000348 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_629__dbase_byte_offset   864 /* 0x00000360 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_629__dpitchm1_byte_offset    868 /* 0x00000364 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_629__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_629__W   5740
#define bisenet_pelee_picinfo__prim_split_3___pvcn_629__H   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_629__D   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_629__P   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_629__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_629__dpitch_num_bytes    5760
#define bisenet_pelee_picinfo__prim_split_3___pvcn_629__dram_format     0
#define bisenet_pelee_picinfo__prim_split_3___pvcn_629__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_3___pvcn_629__tile_width  1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_629__tile_height     1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_629__vmem_buffer_num_bytes   5740

/* bisenet_pelee_picinfo__prim_split_3 SMB_input __pvcn_630_ */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_630__cnngen_demangled_name   "__pvcn_630_"
#define bisenet_pelee_picinfo__prim_split_3___pvcn_630__is_constant     1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_630__byte_offset     876 /* 0x0000036c */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_630__vbase_byte_offset   880 /* 0x00000370 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_630__dbase_byte_offset   904 /* 0x00000388 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_630__dpitchm1_byte_offset    908 /* 0x0000038c */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_630__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_630__W   88
#define bisenet_pelee_picinfo__prim_split_3___pvcn_630__H   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_630__D   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_630__P   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_630__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_630__dpitch_num_bytes    96
#define bisenet_pelee_picinfo__prim_split_3___pvcn_630__dram_format     0
#define bisenet_pelee_picinfo__prim_split_3___pvcn_630__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_3___pvcn_630__tile_width  1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_630__tile_height     1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_630__vmem_buffer_num_bytes   88

/* bisenet_pelee_picinfo__prim_split_3 SMB_input __pvcn_632_ */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_632__cnngen_demangled_name   "__pvcn_632_"
#define bisenet_pelee_picinfo__prim_split_3___pvcn_632__is_constant     1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_632__byte_offset     916 /* 0x00000394 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_632__vbase_byte_offset   920 /* 0x00000398 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_632__dbase_byte_offset   944 /* 0x000003b0 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_632__dpitchm1_byte_offset    948 /* 0x000003b4 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_632__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_632__W   1416
#define bisenet_pelee_picinfo__prim_split_3___pvcn_632__H   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_632__D   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_632__P   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_632__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_632__dpitch_num_bytes    1440
#define bisenet_pelee_picinfo__prim_split_3___pvcn_632__dram_format     0
#define bisenet_pelee_picinfo__prim_split_3___pvcn_632__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_3___pvcn_632__tile_width  1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_632__tile_height     1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_632__vmem_buffer_num_bytes   1416

/* bisenet_pelee_picinfo__prim_split_3 SMB_input __pvcn_633_ */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_633__cnngen_demangled_name   "__pvcn_633_"
#define bisenet_pelee_picinfo__prim_split_3___pvcn_633__is_constant     1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_633__byte_offset     956 /* 0x000003bc */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_633__vbase_byte_offset   960 /* 0x000003c0 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_633__dbase_byte_offset   984 /* 0x000003d8 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_633__dpitchm1_byte_offset    988 /* 0x000003dc */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_633__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_3___pvcn_633__W   88
#define bisenet_pelee_picinfo__prim_split_3___pvcn_633__H   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_633__D   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_633__P   1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_633__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_633__dpitch_num_bytes    96
#define bisenet_pelee_picinfo__prim_split_3___pvcn_633__dram_format     0
#define bisenet_pelee_picinfo__prim_split_3___pvcn_633__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_3___pvcn_633__tile_width  1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_633__tile_height     1
#define bisenet_pelee_picinfo__prim_split_3___pvcn_633__vmem_buffer_num_bytes   88

/* bisenet_pelee_picinfo__prim_split_3 VCB PeleeNet__stage_3_dense_block_2__concat */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_2__concat_cnngen_demangled_name   "PeleeNet__stage_3_dense_block_2__concat"
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_2__concat_byte_offset     996 /* 0x000003e4 */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_2__concat_vbase_byte_offset   1004    /* 0x000003ec */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_2__concat_vmem_buffer_num_bytes   568832

/* bisenet_pelee_picinfo__prim_split_3 VCB PeleeNet__stage_3_dense_block_3__concat */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_3__concat_cnngen_demangled_name   "PeleeNet__stage_3_dense_block_3__concat"
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_3__concat_byte_offset     1032    /* 0x00000408 */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_3__concat_vbase_byte_offset   1040    /* 0x00000410 */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_3__concat_vmem_buffer_num_bytes   374784

/* bisenet_pelee_picinfo__prim_split_3 HMB_output PeleeNet__stage_3_dense_block_4__concat */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_4__concat_cnngen_demangled_name   "PeleeNet__stage_3_dense_block_4__concat"
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_4__concat_byte_offset     1068    /* 0x0000042c */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_4__concat_vbase_byte_offset   1072    /* 0x00000430 */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_4__concat_dbase_byte_offset   1096    /* 0x00000448 */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_4__concat_dpitchm1_byte_offset    1100    /* 0x0000044c */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_4__concat_dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_4__concat_vwidth_minus_one_byte_offset    1084    /* 0x0000043c */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_4__concat_vwidth_minus_one_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_4__concat_vheight_minus_one_byte_offset   1086    /* 0x0000043e */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_4__concat_vheight_minus_one_bsize     16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_4__concat_drotate_bit_offset  8742    /* 0x00002226 */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_4__concat_hflip_bit_offset    8743    /* 0x00002227 */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_4__concat_vflip_bit_offset    8744    /* 0x00002228 */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_4__concat_dflip_bit_offset    8745    /* 0x00002229 */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_4__concat_pflip_bit_offset    8746    /* 0x0000222a */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_4__concat_W   80
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_4__concat_H   32
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_4__concat_D   416
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_4__concat_P   1
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_4__concat_data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_4__concat_dpitch_num_bytes    32
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_4__concat_dram_format     3
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_4__concat_vp_interleave_mode  2
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_4__concat_tile_width  4
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_4__concat_tile_height     1
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_4__concat_vmem_buffer_num_bytes   139776
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_4__concat_dram_size_num_bytes     1064960
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_4__concat_sign    0
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_4__concat_datasize    0
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_4__concat_expoffset   5
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_4__concat_expbits     0
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_4__concat_drotate     0
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_4__concat_hflip   0
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_4__concat_vflip   0
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_4__concat_dflip   0
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_4__concat_pflip   0

/* bisenet_pelee_picinfo__prim_split_3 VCB PeleeNet__stage_3_dense_block_2__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_2__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_cnngen_demangled_name    "PeleeNet__stage_3_dense_block_2__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm"
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_2__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_byte_offset  1120    /* 0x00000460 */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_2__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_vbase_byte_offset    1128    /* 0x00000468 */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_2__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_vmem_buffer_num_bytes    83968

/* bisenet_pelee_picinfo__prim_split_3 VCB PeleeNet__stage_3_dense_block_2__conv_left_0__Relu */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_2__conv_left_0__Relu_cnngen_demangled_name    "PeleeNet__stage_3_dense_block_2__conv_left_0__Relu"
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_2__conv_left_0__Relu_byte_offset  1172    /* 0x00000494 */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_2__conv_left_0__Relu_vbase_byte_offset    1180    /* 0x0000049c */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_2__conv_left_0__Relu_vmem_buffer_num_bytes    56320

/* bisenet_pelee_picinfo__prim_split_3 VCB PeleeNet__stage_3_dense_block_2__conv_right_0__Relu */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_2__conv_right_0__Relu_cnngen_demangled_name   "PeleeNet__stage_3_dense_block_2__conv_right_0__Relu"
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_2__conv_right_0__Relu_byte_offset     1244    /* 0x000004dc */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_2__conv_right_0__Relu_vbase_byte_offset   1252    /* 0x000004e4 */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_2__conv_right_0__Relu_vmem_buffer_num_bytes   28160

/* bisenet_pelee_picinfo__prim_split_3 VCB PeleeNet__stage_3_dense_block_2__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_2__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_cnngen_demangled_name  "PeleeNet__stage_3_dense_block_2__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu"
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_2__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_byte_offset    1292    /* 0x0000050c */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_2__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vbase_byte_offset  1300    /* 0x00000514 */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_2__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vmem_buffer_num_bytes  28160

/* bisenet_pelee_picinfo__prim_split_3 VCB PeleeNet__stage_3_dense_block_3__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_3__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_cnngen_demangled_name    "PeleeNet__stage_3_dense_block_3__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm"
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_3__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_byte_offset  1404    /* 0x0000057c */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_3__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_vbase_byte_offset    1412    /* 0x00000584 */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_3__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_vmem_buffer_num_bytes    165888

/* bisenet_pelee_picinfo__prim_split_3 VCB PeleeNet__stage_3_dense_block_3__conv_left_0__Relu */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_3__conv_left_0__Relu_cnngen_demangled_name    "PeleeNet__stage_3_dense_block_3__conv_left_0__Relu"
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_3__conv_left_0__Relu_byte_offset  1456    /* 0x000005b0 */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_3__conv_left_0__Relu_vbase_byte_offset    1464    /* 0x000005b8 */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_3__conv_left_0__Relu_vmem_buffer_num_bytes    28160

/* bisenet_pelee_picinfo__prim_split_3 VCB PeleeNet__stage_3_dense_block_3__conv_right_0__Relu */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_3__conv_right_0__Relu_cnngen_demangled_name   "PeleeNet__stage_3_dense_block_3__conv_right_0__Relu"
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_3__conv_right_0__Relu_byte_offset     1528    /* 0x000005f8 */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_3__conv_right_0__Relu_vbase_byte_offset   1536    /* 0x00000600 */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_3__conv_right_0__Relu_vmem_buffer_num_bytes   43264

/* bisenet_pelee_picinfo__prim_split_3 VCB PeleeNet__stage_3_dense_block_3__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_3__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_cnngen_demangled_name  "PeleeNet__stage_3_dense_block_3__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu"
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_3__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_byte_offset    1576    /* 0x00000628 */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_3__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vbase_byte_offset  1584    /* 0x00000630 */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_3__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vmem_buffer_num_bytes  28032

/* bisenet_pelee_picinfo__prim_split_3 VCB PeleeNet__stage_3_dense_block_4__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_4__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_cnngen_demangled_name    "PeleeNet__stage_3_dense_block_4__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm"
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_4__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_byte_offset  1668    /* 0x00000684 */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_4__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_vbase_byte_offset    1676    /* 0x0000068c */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_4__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_vmem_buffer_num_bytes    124928

/* bisenet_pelee_picinfo__prim_split_3 VCB PeleeNet__stage_3_dense_block_4__conv_left_0__Relu */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_4__conv_left_0__Relu_cnngen_demangled_name    "PeleeNet__stage_3_dense_block_4__conv_left_0__Relu"
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_4__conv_left_0__Relu_byte_offset  1720    /* 0x000006b8 */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_4__conv_left_0__Relu_vbase_byte_offset    1728    /* 0x000006c0 */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_4__conv_left_0__Relu_vmem_buffer_num_bytes    43264

/* bisenet_pelee_picinfo__prim_split_3 VCB PeleeNet__stage_3_dense_block_4__conv_right_0__Relu */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_4__conv_right_0__Relu_cnngen_demangled_name   "PeleeNet__stage_3_dense_block_4__conv_right_0__Relu"
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_4__conv_right_0__Relu_byte_offset     1792    /* 0x00000700 */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_4__conv_right_0__Relu_vbase_byte_offset   1800    /* 0x00000708 */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_4__conv_right_0__Relu_vmem_buffer_num_bytes   43264

/* bisenet_pelee_picinfo__prim_split_3 VCB PeleeNet__stage_3_dense_block_4__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_4__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_cnngen_demangled_name  "PeleeNet__stage_3_dense_block_4__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu"
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_4__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_byte_offset    1840    /* 0x00000730 */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_4__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vbase_byte_offset  1848    /* 0x00000738 */
#define bisenet_pelee_picinfo__prim_split_3_PeleeNet__stage_3_dense_block_4__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vmem_buffer_num_bytes  28032


#endif /* bisenet_pelee_picinfo__prim_split_3_H */
