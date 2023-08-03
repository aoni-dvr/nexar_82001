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
#ifndef bisenet_pelee_picinfo__prim_split_6_H
#define bisenet_pelee_picinfo__prim_split_6_H

#define vas_gen_header_version                      5

/* dag name */
#define bisenet_pelee_picinfo__prim_split_6_DAG_NAME    "bisenet_pelee_picinfo__prim_split_6"

/* VDG file info */
#define bisenet_pelee_picinfo__prim_split_6_vdg_name    "bisenet_pelee_picinfo__split_6.vdg"

/* VDG DAG memory info */
#define bisenet_pelee_picinfo__prim_split_6_byte_offset     340 /* 0x00000154 */
#define bisenet_pelee_picinfo__prim_split_6_byte_size   1396    /* 0x00000574 */
#define bisenet_pelee_picinfo__prim_split_6_preferred_dram_xfer_size    1

/* bisenet_pelee_picinfo__prim_split_6 VMEM info */
#define bisenet_pelee_picinfo__prim_split_6_use_ping_pong_vmem  0
#define bisenet_pelee_picinfo__prim_split_6_VMEM_start  4096    /* 0x00001000 */
#define bisenet_pelee_picinfo__prim_split_6_VMEM_end    2069448 /* 0x001f93c8 */
#define bisenet_pelee_picinfo__prim_split_6_image_start     2022692 /* 0x001edd24 */
#define bisenet_pelee_picinfo__prim_split_6_image_size  48152   /* 0x0000bc18 */
#define bisenet_pelee_picinfo__prim_split_6_dagbin_start    2069448 /* 0x001f93c8 */

/*******************************************************************************
 * DAG byte offset info for HMB/SMB/VCB and their fields
 * in DAG 'bisenet_pelee_picinfo__prim_split_6' in source file 'bisenet_pelee_picinfo__split_6.vas'
 ******************************************************************************/
/* bisenet_pelee_picinfo__prim_split_6 HMB_input PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool */
#define bisenet_pelee_picinfo__prim_split_6_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_cnngen_demangled_name     "PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool"
#define bisenet_pelee_picinfo__prim_split_6_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_is_constant   0
#define bisenet_pelee_picinfo__prim_split_6_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_ddi_byte_size     0   /* 0x00000000 */
#define bisenet_pelee_picinfo__prim_split_6_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_ddi_byte_offset   4294967295  /* 0xffffffff */
#define bisenet_pelee_picinfo__prim_split_6_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_byte_offset   0   /* 0x00000000 */
#define bisenet_pelee_picinfo__prim_split_6_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_vbase_byte_offset     4   /* 0x00000004 */
#define bisenet_pelee_picinfo__prim_split_6_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_dbase_byte_offset     28  /* 0x0000001c */
#define bisenet_pelee_picinfo__prim_split_6_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_dpitchm1_byte_offset  32  /* 0x00000020 */
#define bisenet_pelee_picinfo__prim_split_6_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_dpitchm1_bsize    16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_6_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_vwidth_minus_one_byte_offset  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_6_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_vwidth_minus_one_bsize    16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_6_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_vheight_minus_one_byte_offset     18  /* 0x00000012 */
#define bisenet_pelee_picinfo__prim_split_6_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_vheight_minus_one_bsize   16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_6_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_drotate_bit_offset    198 /* 0x000000c6 */
#define bisenet_pelee_picinfo__prim_split_6_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_hflip_bit_offset  199 /* 0x000000c7 */
#define bisenet_pelee_picinfo__prim_split_6_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_vflip_bit_offset  200 /* 0x000000c8 */
#define bisenet_pelee_picinfo__prim_split_6_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_dflip_bit_offset  201 /* 0x000000c9 */
#define bisenet_pelee_picinfo__prim_split_6_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_pflip_bit_offset  202 /* 0x000000ca */
#define bisenet_pelee_picinfo__prim_split_6_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_W     80
#define bisenet_pelee_picinfo__prim_split_6_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_H     32
#define bisenet_pelee_picinfo__prim_split_6_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_D     256
#define bisenet_pelee_picinfo__prim_split_6_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_P     1
#define bisenet_pelee_picinfo__prim_split_6_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_data_num_bytes    1
#define bisenet_pelee_picinfo__prim_split_6_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_dpitch_num_bytes  32
#define bisenet_pelee_picinfo__prim_split_6_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_dram_format   3
#define bisenet_pelee_picinfo__prim_split_6_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_vp_interleave_mode    2
#define bisenet_pelee_picinfo__prim_split_6_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_tile_width    4
#define bisenet_pelee_picinfo__prim_split_6_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_tile_height   1
#define bisenet_pelee_picinfo__prim_split_6_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_vmem_buffer_num_bytes     634880
#define bisenet_pelee_picinfo__prim_split_6_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_dram_size_num_bytes   655360
#define bisenet_pelee_picinfo__prim_split_6_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_sign  0
#define bisenet_pelee_picinfo__prim_split_6_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_datasize  0
#define bisenet_pelee_picinfo__prim_split_6_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_expoffset     5
#define bisenet_pelee_picinfo__prim_split_6_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_expbits   0
#define bisenet_pelee_picinfo__prim_split_6_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_drotate   0
#define bisenet_pelee_picinfo__prim_split_6_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_hflip     0
#define bisenet_pelee_picinfo__prim_split_6_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_vflip     0
#define bisenet_pelee_picinfo__prim_split_6_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_dflip     0
#define bisenet_pelee_picinfo__prim_split_6_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_pflip     0

/* bisenet_pelee_picinfo__prim_split_6 SMB_input __pvcn_803_ */
#define bisenet_pelee_picinfo__prim_split_6___pvcn_803__cnngen_demangled_name   "__pvcn_803_"
#define bisenet_pelee_picinfo__prim_split_6___pvcn_803__is_constant     1
#define bisenet_pelee_picinfo__prim_split_6___pvcn_803__byte_offset     64  /* 0x00000040 */
#define bisenet_pelee_picinfo__prim_split_6___pvcn_803__vbase_byte_offset   68  /* 0x00000044 */
#define bisenet_pelee_picinfo__prim_split_6___pvcn_803__dbase_byte_offset   92  /* 0x0000005c */
#define bisenet_pelee_picinfo__prim_split_6___pvcn_803__dpitchm1_byte_offset    96  /* 0x00000060 */
#define bisenet_pelee_picinfo__prim_split_6___pvcn_803__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_6___pvcn_803__W   43968
#define bisenet_pelee_picinfo__prim_split_6___pvcn_803__H   1
#define bisenet_pelee_picinfo__prim_split_6___pvcn_803__D   1
#define bisenet_pelee_picinfo__prim_split_6___pvcn_803__P   1
#define bisenet_pelee_picinfo__prim_split_6___pvcn_803__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_6___pvcn_803__dpitch_num_bytes    43968
#define bisenet_pelee_picinfo__prim_split_6___pvcn_803__dram_format     0
#define bisenet_pelee_picinfo__prim_split_6___pvcn_803__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_6___pvcn_803__tile_width  1
#define bisenet_pelee_picinfo__prim_split_6___pvcn_803__tile_height     1
#define bisenet_pelee_picinfo__prim_split_6___pvcn_803__vmem_buffer_num_bytes   43968

/* bisenet_pelee_picinfo__prim_split_6 SMB_input __pvcn_804_ */
#define bisenet_pelee_picinfo__prim_split_6___pvcn_804__cnngen_demangled_name   "__pvcn_804_"
#define bisenet_pelee_picinfo__prim_split_6___pvcn_804__is_constant     1
#define bisenet_pelee_picinfo__prim_split_6___pvcn_804__byte_offset     104 /* 0x00000068 */
#define bisenet_pelee_picinfo__prim_split_6___pvcn_804__vbase_byte_offset   108 /* 0x0000006c */
#define bisenet_pelee_picinfo__prim_split_6___pvcn_804__dbase_byte_offset   132 /* 0x00000084 */
#define bisenet_pelee_picinfo__prim_split_6___pvcn_804__dpitchm1_byte_offset    136 /* 0x00000088 */
#define bisenet_pelee_picinfo__prim_split_6___pvcn_804__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_6___pvcn_804__W   704
#define bisenet_pelee_picinfo__prim_split_6___pvcn_804__H   1
#define bisenet_pelee_picinfo__prim_split_6___pvcn_804__D   1
#define bisenet_pelee_picinfo__prim_split_6___pvcn_804__P   1
#define bisenet_pelee_picinfo__prim_split_6___pvcn_804__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_6___pvcn_804__dpitch_num_bytes    704
#define bisenet_pelee_picinfo__prim_split_6___pvcn_804__dram_format     0
#define bisenet_pelee_picinfo__prim_split_6___pvcn_804__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_6___pvcn_804__tile_width  1
#define bisenet_pelee_picinfo__prim_split_6___pvcn_804__tile_height     1
#define bisenet_pelee_picinfo__prim_split_6___pvcn_804__vmem_buffer_num_bytes   704

/* bisenet_pelee_picinfo__prim_split_6 SMB_input __pvcn_807_ */
#define bisenet_pelee_picinfo__prim_split_6___pvcn_807__cnngen_demangled_name   "__pvcn_807_"
#define bisenet_pelee_picinfo__prim_split_6___pvcn_807__is_constant     1
#define bisenet_pelee_picinfo__prim_split_6___pvcn_807__byte_offset     144 /* 0x00000090 */
#define bisenet_pelee_picinfo__prim_split_6___pvcn_807__vbase_byte_offset   148 /* 0x00000094 */
#define bisenet_pelee_picinfo__prim_split_6___pvcn_807__dbase_byte_offset   176 /* 0x000000b0 */
#define bisenet_pelee_picinfo__prim_split_6___pvcn_807__dpitchm1_byte_offset    180 /* 0x000000b4 */
#define bisenet_pelee_picinfo__prim_split_6___pvcn_807__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_6___pvcn_807__W   1
#define bisenet_pelee_picinfo__prim_split_6___pvcn_807__H   1
#define bisenet_pelee_picinfo__prim_split_6___pvcn_807__D   256
#define bisenet_pelee_picinfo__prim_split_6___pvcn_807__P   1
#define bisenet_pelee_picinfo__prim_split_6___pvcn_807__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_6___pvcn_807__dpitch_num_bytes    32
#define bisenet_pelee_picinfo__prim_split_6___pvcn_807__dram_format     0
#define bisenet_pelee_picinfo__prim_split_6___pvcn_807__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_6___pvcn_807__tile_width  1
#define bisenet_pelee_picinfo__prim_split_6___pvcn_807__tile_height     1
#define bisenet_pelee_picinfo__prim_split_6___pvcn_807__vmem_buffer_num_bytes   1024

/* bisenet_pelee_picinfo__prim_split_6 SMB_input __pvcn_808_ */
#define bisenet_pelee_picinfo__prim_split_6___pvcn_808__cnngen_demangled_name   "__pvcn_808_"
#define bisenet_pelee_picinfo__prim_split_6___pvcn_808__is_constant     1
#define bisenet_pelee_picinfo__prim_split_6___pvcn_808__byte_offset     188 /* 0x000000bc */
#define bisenet_pelee_picinfo__prim_split_6___pvcn_808__vbase_byte_offset   192 /* 0x000000c0 */
#define bisenet_pelee_picinfo__prim_split_6___pvcn_808__dbase_byte_offset   220 /* 0x000000dc */
#define bisenet_pelee_picinfo__prim_split_6___pvcn_808__dpitchm1_byte_offset    224 /* 0x000000e0 */
#define bisenet_pelee_picinfo__prim_split_6___pvcn_808__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_6___pvcn_808__W   1
#define bisenet_pelee_picinfo__prim_split_6___pvcn_808__H   1
#define bisenet_pelee_picinfo__prim_split_6___pvcn_808__D   256
#define bisenet_pelee_picinfo__prim_split_6___pvcn_808__P   1
#define bisenet_pelee_picinfo__prim_split_6___pvcn_808__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_6___pvcn_808__dpitch_num_bytes    32
#define bisenet_pelee_picinfo__prim_split_6___pvcn_808__dram_format     0
#define bisenet_pelee_picinfo__prim_split_6___pvcn_808__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_6___pvcn_808__tile_width  1
#define bisenet_pelee_picinfo__prim_split_6___pvcn_808__tile_height     1
#define bisenet_pelee_picinfo__prim_split_6___pvcn_808__vmem_buffer_num_bytes   1024

/* bisenet_pelee_picinfo__prim_split_6 SMB_input __pvcn_810_ */
#define bisenet_pelee_picinfo__prim_split_6___pvcn_810__cnngen_demangled_name   "__pvcn_810_"
#define bisenet_pelee_picinfo__prim_split_6___pvcn_810__is_constant     1
#define bisenet_pelee_picinfo__prim_split_6___pvcn_810__byte_offset     232 /* 0x000000e8 */
#define bisenet_pelee_picinfo__prim_split_6___pvcn_810__vbase_byte_offset   236 /* 0x000000ec */
#define bisenet_pelee_picinfo__prim_split_6___pvcn_810__dbase_byte_offset   260 /* 0x00000104 */
#define bisenet_pelee_picinfo__prim_split_6___pvcn_810__dpitchm1_byte_offset    264 /* 0x00000108 */
#define bisenet_pelee_picinfo__prim_split_6___pvcn_810__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_6___pvcn_810__W   4
#define bisenet_pelee_picinfo__prim_split_6___pvcn_810__H   1
#define bisenet_pelee_picinfo__prim_split_6___pvcn_810__D   1
#define bisenet_pelee_picinfo__prim_split_6___pvcn_810__P   1
#define bisenet_pelee_picinfo__prim_split_6___pvcn_810__data_num_bytes  2
#define bisenet_pelee_picinfo__prim_split_6___pvcn_810__dpitch_num_bytes    32
#define bisenet_pelee_picinfo__prim_split_6___pvcn_810__dram_format     0
#define bisenet_pelee_picinfo__prim_split_6___pvcn_810__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_6___pvcn_810__tile_width  1
#define bisenet_pelee_picinfo__prim_split_6___pvcn_810__tile_height     1
#define bisenet_pelee_picinfo__prim_split_6___pvcn_810__vmem_buffer_num_bytes   8

/* bisenet_pelee_picinfo__prim_split_6 SMB_input __pvcn_811_ */
#define bisenet_pelee_picinfo__prim_split_6___pvcn_811__cnngen_demangled_name   "__pvcn_811_"
#define bisenet_pelee_picinfo__prim_split_6___pvcn_811__is_constant     1
#define bisenet_pelee_picinfo__prim_split_6___pvcn_811__byte_offset     272 /* 0x00000110 */
#define bisenet_pelee_picinfo__prim_split_6___pvcn_811__vbase_byte_offset   276 /* 0x00000114 */
#define bisenet_pelee_picinfo__prim_split_6___pvcn_811__dbase_byte_offset   300 /* 0x0000012c */
#define bisenet_pelee_picinfo__prim_split_6___pvcn_811__dpitchm1_byte_offset    304 /* 0x00000130 */
#define bisenet_pelee_picinfo__prim_split_6___pvcn_811__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_6___pvcn_811__W   5
#define bisenet_pelee_picinfo__prim_split_6___pvcn_811__H   1
#define bisenet_pelee_picinfo__prim_split_6___pvcn_811__D   1
#define bisenet_pelee_picinfo__prim_split_6___pvcn_811__P   1
#define bisenet_pelee_picinfo__prim_split_6___pvcn_811__data_num_bytes  2
#define bisenet_pelee_picinfo__prim_split_6___pvcn_811__dpitch_num_bytes    32
#define bisenet_pelee_picinfo__prim_split_6___pvcn_811__dram_format     0
#define bisenet_pelee_picinfo__prim_split_6___pvcn_811__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_6___pvcn_811__tile_width  1
#define bisenet_pelee_picinfo__prim_split_6___pvcn_811__tile_height     1
#define bisenet_pelee_picinfo__prim_split_6___pvcn_811__vmem_buffer_num_bytes   12

/* bisenet_pelee_picinfo__prim_split_6 SMB_input __pvcn_828_ */
#define bisenet_pelee_picinfo__prim_split_6___pvcn_828__cnngen_demangled_name   "__pvcn_828_"
#define bisenet_pelee_picinfo__prim_split_6___pvcn_828__is_constant     1
#define bisenet_pelee_picinfo__prim_split_6___pvcn_828__byte_offset     360 /* 0x00000168 */
#define bisenet_pelee_picinfo__prim_split_6___pvcn_828__vbase_byte_offset   364 /* 0x0000016c */
#define bisenet_pelee_picinfo__prim_split_6___pvcn_828__dbase_byte_offset   388 /* 0x00000184 */
#define bisenet_pelee_picinfo__prim_split_6___pvcn_828__dpitchm1_byte_offset    392 /* 0x00000188 */
#define bisenet_pelee_picinfo__prim_split_6___pvcn_828__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_6___pvcn_828__W   4
#define bisenet_pelee_picinfo__prim_split_6___pvcn_828__H   1
#define bisenet_pelee_picinfo__prim_split_6___pvcn_828__D   1
#define bisenet_pelee_picinfo__prim_split_6___pvcn_828__P   1
#define bisenet_pelee_picinfo__prim_split_6___pvcn_828__data_num_bytes  4
#define bisenet_pelee_picinfo__prim_split_6___pvcn_828__dpitch_num_bytes    32
#define bisenet_pelee_picinfo__prim_split_6___pvcn_828__dram_format     0
#define bisenet_pelee_picinfo__prim_split_6___pvcn_828__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_6___pvcn_828__tile_width  1
#define bisenet_pelee_picinfo__prim_split_6___pvcn_828__tile_height     1
#define bisenet_pelee_picinfo__prim_split_6___pvcn_828__vmem_buffer_num_bytes   16

/* bisenet_pelee_picinfo__prim_split_6 VCB PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_986_ */
#define bisenet_pelee_picinfo__prim_split_6_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_986__cnngen_demangled_name    "PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_986_"
#define bisenet_pelee_picinfo__prim_split_6_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_986__byte_offset  416 /* 0x000001a0 */
#define bisenet_pelee_picinfo__prim_split_6_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_986__vbase_byte_offset    420 /* 0x000001a4 */
#define bisenet_pelee_picinfo__prim_split_6_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_986__vmem_buffer_num_bytes    329228

/* bisenet_pelee_picinfo__prim_split_6 VCB Mean_pre_trans___dotib___64 */
#define bisenet_pelee_picinfo__prim_split_6_Mean_pre_trans___dotib___64_cnngen_demangled_name   "Mean_pre_trans___dotib___64"
#define bisenet_pelee_picinfo__prim_split_6_Mean_pre_trans___dotib___64_byte_offset     436 /* 0x000001b4 */
#define bisenet_pelee_picinfo__prim_split_6_Mean_pre_trans___dotib___64_vbase_byte_offset   440 /* 0x000001b8 */
#define bisenet_pelee_picinfo__prim_split_6_Mean_pre_trans___dotib___64_vmem_buffer_num_bytes   512

/* bisenet_pelee_picinfo__prim_split_6 VCB PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_991_ */
#define bisenet_pelee_picinfo__prim_split_6_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_991__cnngen_demangled_name    "PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_991_"
#define bisenet_pelee_picinfo__prim_split_6_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_991__byte_offset  508 /* 0x000001fc */
#define bisenet_pelee_picinfo__prim_split_6_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_991__vbase_byte_offset    512 /* 0x00000200 */
#define bisenet_pelee_picinfo__prim_split_6_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_991__vmem_buffer_num_bytes    329228

/* bisenet_pelee_picinfo__prim_split_6 VCB Mean_pre_trans */
#define bisenet_pelee_picinfo__prim_split_6_Mean_pre_trans_cnngen_demangled_name    "Mean_pre_trans"
#define bisenet_pelee_picinfo__prim_split_6_Mean_pre_trans_byte_offset  568 /* 0x00000238 */
#define bisenet_pelee_picinfo__prim_split_6_Mean_pre_trans_vbase_byte_offset    572 /* 0x0000023c */
#define bisenet_pelee_picinfo__prim_split_6_Mean_pre_trans_vmem_buffer_num_bytes    256

/* bisenet_pelee_picinfo__prim_split_6 VCB __pvcn_260_ */
#define bisenet_pelee_picinfo__prim_split_6___pvcn_260__cnngen_demangled_name   "__pvcn_260_"
#define bisenet_pelee_picinfo__prim_split_6___pvcn_260__byte_offset     604 /* 0x0000025c */
#define bisenet_pelee_picinfo__prim_split_6___pvcn_260__vbase_byte_offset   608 /* 0x00000260 */
#define bisenet_pelee_picinfo__prim_split_6___pvcn_260__vmem_buffer_num_bytes   256

/* bisenet_pelee_picinfo__prim_split_6 VCB Conv_3__Conv2D___dotib___391 */
#define bisenet_pelee_picinfo__prim_split_6_Conv_3__Conv2D___dotib___391_cnngen_demangled_name  "Conv_3__Conv2D___dotib___391"
#define bisenet_pelee_picinfo__prim_split_6_Conv_3__Conv2D___dotib___391_byte_offset    640 /* 0x00000280 */
#define bisenet_pelee_picinfo__prim_split_6_Conv_3__Conv2D___dotib___391_vbase_byte_offset  644 /* 0x00000284 */
#define bisenet_pelee_picinfo__prim_split_6_Conv_3__Conv2D___dotib___391_vmem_buffer_num_bytes  256

/* bisenet_pelee_picinfo__prim_split_6 VCB __pvcn_820_ */
#define bisenet_pelee_picinfo__prim_split_6___pvcn_820__cnngen_demangled_name   "__pvcn_820_"
#define bisenet_pelee_picinfo__prim_split_6___pvcn_820__byte_offset     848 /* 0x00000350 */
#define bisenet_pelee_picinfo__prim_split_6___pvcn_820__vbase_byte_offset   852 /* 0x00000354 */
#define bisenet_pelee_picinfo__prim_split_6___pvcn_820__vmem_buffer_num_bytes   3072

/* bisenet_pelee_picinfo__prim_split_6 VCB Sigmoid___exp___71 */
#define bisenet_pelee_picinfo__prim_split_6_Sigmoid___exp___71_cnngen_demangled_name    "Sigmoid___exp___71"
#define bisenet_pelee_picinfo__prim_split_6_Sigmoid___exp___71_byte_offset  888 /* 0x00000378 */
#define bisenet_pelee_picinfo__prim_split_6_Sigmoid___exp___71_vbase_byte_offset    892 /* 0x0000037c */
#define bisenet_pelee_picinfo__prim_split_6_Sigmoid___exp___71_vmem_buffer_num_bytes    2048

/* bisenet_pelee_picinfo__prim_split_6 VCB Sigmoid___addi___72 */
#define bisenet_pelee_picinfo__prim_split_6_Sigmoid___addi___72_cnngen_demangled_name   "Sigmoid___addi___72"
#define bisenet_pelee_picinfo__prim_split_6_Sigmoid___addi___72_byte_offset     932 /* 0x000003a4 */
#define bisenet_pelee_picinfo__prim_split_6_Sigmoid___addi___72_vbase_byte_offset   936 /* 0x000003a8 */
#define bisenet_pelee_picinfo__prim_split_6_Sigmoid___addi___72_vmem_buffer_num_bytes   3072

/* bisenet_pelee_picinfo__prim_split_6 VCB Sigmoid___mul___74 */
#define bisenet_pelee_picinfo__prim_split_6_Sigmoid___mul___74_cnngen_demangled_name    "Sigmoid___mul___74"
#define bisenet_pelee_picinfo__prim_split_6_Sigmoid___mul___74_byte_offset  1004    /* 0x000003ec */
#define bisenet_pelee_picinfo__prim_split_6_Sigmoid___mul___74_vbase_byte_offset    1008    /* 0x000003f0 */
#define bisenet_pelee_picinfo__prim_split_6_Sigmoid___mul___74_vmem_buffer_num_bytes    2048

/* bisenet_pelee_picinfo__prim_split_6 VCB Sigmoid */
#define bisenet_pelee_picinfo__prim_split_6_Sigmoid_cnngen_demangled_name   "Sigmoid"
#define bisenet_pelee_picinfo__prim_split_6_Sigmoid_byte_offset     1080    /* 0x00000438 */
#define bisenet_pelee_picinfo__prim_split_6_Sigmoid_vbase_byte_offset   1084    /* 0x0000043c */
#define bisenet_pelee_picinfo__prim_split_6_Sigmoid_vmem_buffer_num_bytes   5120

/* bisenet_pelee_picinfo__prim_split_6 VCB PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_997_ */
#define bisenet_pelee_picinfo__prim_split_6_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_997__cnngen_demangled_name    "PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_997_"
#define bisenet_pelee_picinfo__prim_split_6_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_997__byte_offset  1128    /* 0x00000468 */
#define bisenet_pelee_picinfo__prim_split_6_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_997__vbase_byte_offset    1136    /* 0x00000470 */
#define bisenet_pelee_picinfo__prim_split_6_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_997__vmem_buffer_num_bytes    61952

/* bisenet_pelee_picinfo__prim_split_6 VCB PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_1002_ */
#define bisenet_pelee_picinfo__prim_split_6_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_1002__cnngen_demangled_name   "PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_1002_"
#define bisenet_pelee_picinfo__prim_split_6_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_1002__byte_offset     1200    /* 0x000004b0 */
#define bisenet_pelee_picinfo__prim_split_6_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_1002__vbase_byte_offset   1208    /* 0x000004b8 */
#define bisenet_pelee_picinfo__prim_split_6_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_1002__vmem_buffer_num_bytes   61952

/* bisenet_pelee_picinfo__prim_split_6 VCB Mul */
#define bisenet_pelee_picinfo__prim_split_6_Mul_cnngen_demangled_name   "Mul"
#define bisenet_pelee_picinfo__prim_split_6_Mul_byte_offset     1264    /* 0x000004f0 */
#define bisenet_pelee_picinfo__prim_split_6_Mul_vbase_byte_offset   1272    /* 0x000004f8 */
#define bisenet_pelee_picinfo__prim_split_6_Mul_vmem_buffer_num_bytes   252940

/* bisenet_pelee_picinfo__prim_split_6 HMB_output ResizeBilinear */
#define bisenet_pelee_picinfo__prim_split_6_ResizeBilinear_cnngen_demangled_name    "ResizeBilinear"
#define bisenet_pelee_picinfo__prim_split_6_ResizeBilinear_byte_offset  1308    /* 0x0000051c */
#define bisenet_pelee_picinfo__prim_split_6_ResizeBilinear_vbase_byte_offset    1312    /* 0x00000520 */
#define bisenet_pelee_picinfo__prim_split_6_ResizeBilinear_dbase_byte_offset    1336    /* 0x00000538 */
#define bisenet_pelee_picinfo__prim_split_6_ResizeBilinear_dpitchm1_byte_offset     1340    /* 0x0000053c */
#define bisenet_pelee_picinfo__prim_split_6_ResizeBilinear_dpitchm1_bsize   16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_6_ResizeBilinear_vwidth_minus_one_byte_offset     1324    /* 0x0000052c */
#define bisenet_pelee_picinfo__prim_split_6_ResizeBilinear_vwidth_minus_one_bsize   16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_6_ResizeBilinear_vheight_minus_one_byte_offset    1326    /* 0x0000052e */
#define bisenet_pelee_picinfo__prim_split_6_ResizeBilinear_vheight_minus_one_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_6_ResizeBilinear_drotate_bit_offset   10662   /* 0x000029a6 */
#define bisenet_pelee_picinfo__prim_split_6_ResizeBilinear_hflip_bit_offset     10663   /* 0x000029a7 */
#define bisenet_pelee_picinfo__prim_split_6_ResizeBilinear_vflip_bit_offset     10664   /* 0x000029a8 */
#define bisenet_pelee_picinfo__prim_split_6_ResizeBilinear_dflip_bit_offset     10665   /* 0x000029a9 */
#define bisenet_pelee_picinfo__prim_split_6_ResizeBilinear_pflip_bit_offset     10666   /* 0x000029aa */
#define bisenet_pelee_picinfo__prim_split_6_ResizeBilinear_W    160
#define bisenet_pelee_picinfo__prim_split_6_ResizeBilinear_H    64
#define bisenet_pelee_picinfo__prim_split_6_ResizeBilinear_D    256
#define bisenet_pelee_picinfo__prim_split_6_ResizeBilinear_P    1
#define bisenet_pelee_picinfo__prim_split_6_ResizeBilinear_data_num_bytes   2
#define bisenet_pelee_picinfo__prim_split_6_ResizeBilinear_dpitch_num_bytes     32
#define bisenet_pelee_picinfo__prim_split_6_ResizeBilinear_dram_format  3
#define bisenet_pelee_picinfo__prim_split_6_ResizeBilinear_vp_interleave_mode   2
#define bisenet_pelee_picinfo__prim_split_6_ResizeBilinear_tile_width   4
#define bisenet_pelee_picinfo__prim_split_6_ResizeBilinear_tile_height  1
#define bisenet_pelee_picinfo__prim_split_6_ResizeBilinear_vmem_buffer_num_bytes    331776
#define bisenet_pelee_picinfo__prim_split_6_ResizeBilinear_dram_size_num_bytes  5242880
#define bisenet_pelee_picinfo__prim_split_6_ResizeBilinear_sign     0
#define bisenet_pelee_picinfo__prim_split_6_ResizeBilinear_datasize     1
#define bisenet_pelee_picinfo__prim_split_6_ResizeBilinear_expoffset    12
#define bisenet_pelee_picinfo__prim_split_6_ResizeBilinear_expbits  0
#define bisenet_pelee_picinfo__prim_split_6_ResizeBilinear_drotate  0
#define bisenet_pelee_picinfo__prim_split_6_ResizeBilinear_hflip    0
#define bisenet_pelee_picinfo__prim_split_6_ResizeBilinear_vflip    0
#define bisenet_pelee_picinfo__prim_split_6_ResizeBilinear_dflip    0
#define bisenet_pelee_picinfo__prim_split_6_ResizeBilinear_pflip    0


#endif /* bisenet_pelee_picinfo__prim_split_6_H */
