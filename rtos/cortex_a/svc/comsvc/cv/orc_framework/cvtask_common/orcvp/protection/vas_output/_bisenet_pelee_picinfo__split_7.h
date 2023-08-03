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
#ifndef bisenet_pelee_picinfo__prim_split_7_H
#define bisenet_pelee_picinfo__prim_split_7_H

#define vas_gen_header_version                      5

/* dag name */
#define bisenet_pelee_picinfo__prim_split_7_DAG_NAME    "bisenet_pelee_picinfo__prim_split_7"

/* VDG file info */
#define bisenet_pelee_picinfo__prim_split_7_vdg_name    "bisenet_pelee_picinfo__split_7.vdg"

/* VDG DAG memory info */
#define bisenet_pelee_picinfo__prim_split_7_byte_offset     340 /* 0x00000154 */
#define bisenet_pelee_picinfo__prim_split_7_byte_size   540 /* 0x0000021c */
#define bisenet_pelee_picinfo__prim_split_7_preferred_dram_xfer_size    1

/* bisenet_pelee_picinfo__prim_split_7 VMEM info */
#define bisenet_pelee_picinfo__prim_split_7_use_ping_pong_vmem  0
#define bisenet_pelee_picinfo__prim_split_7_VMEM_start  4096    /* 0x00001000 */
#define bisenet_pelee_picinfo__prim_split_7_VMEM_end    2045612 /* 0x001f36ac */
#define bisenet_pelee_picinfo__prim_split_7_image_start     2014928 /* 0x001ebed0 */
#define bisenet_pelee_picinfo__prim_split_7_image_size  31224   /* 0x000079f8 */
#define bisenet_pelee_picinfo__prim_split_7_dagbin_start    2045612 /* 0x001f36ac */

/*******************************************************************************
 * DAG byte offset info for HMB/SMB/VCB and their fields
 * in DAG 'bisenet_pelee_picinfo__prim_split_7' in source file 'bisenet_pelee_picinfo__split_7.vas'
 ******************************************************************************/
/* bisenet_pelee_picinfo__prim_split_7 HMB_input ResizeBilinear */
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_cnngen_demangled_name    "ResizeBilinear"
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_is_constant  0
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_ddi_byte_size    0   /* 0x00000000 */
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_ddi_byte_offset  4294967295  /* 0xffffffff */
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_byte_offset  0   /* 0x00000000 */
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_vbase_byte_offset    4   /* 0x00000004 */
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_dbase_byte_offset    28  /* 0x0000001c */
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_dpitchm1_byte_offset     32  /* 0x00000020 */
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_dpitchm1_bsize   16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_vwidth_minus_one_byte_offset     16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_vwidth_minus_one_bsize   16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_vheight_minus_one_byte_offset    18  /* 0x00000012 */
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_vheight_minus_one_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_drotate_bit_offset   198 /* 0x000000c6 */
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_hflip_bit_offset     199 /* 0x000000c7 */
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_vflip_bit_offset     200 /* 0x000000c8 */
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_dflip_bit_offset     201 /* 0x000000c9 */
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_pflip_bit_offset     202 /* 0x000000ca */
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_W    160
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_H    64
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_D    256
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_P    1
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_data_num_bytes   2
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_dpitch_num_bytes     32
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_dram_format  3
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_vp_interleave_mode   2
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_tile_width   4
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_tile_height  1
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_vmem_buffer_num_bytes    53248
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_dram_size_num_bytes  5242880
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_sign     0
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_datasize     1
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_expoffset    12
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_expbits  0
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_drotate  0
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_hflip    0
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_vflip    0
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_dflip    0
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_pflip    0

/* bisenet_pelee_picinfo__prim_split_7 HMB_input ResizeBilinear_1 */
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_1_cnngen_demangled_name  "ResizeBilinear_1"
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_1_is_constant    0
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_1_ddi_byte_size  0   /* 0x00000000 */
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_1_ddi_byte_offset    4294967295  /* 0xffffffff */
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_1_byte_offset    36  /* 0x00000024 */
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_1_vbase_byte_offset  40  /* 0x00000028 */
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_1_dbase_byte_offset  64  /* 0x00000040 */
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_1_dpitchm1_byte_offset   68  /* 0x00000044 */
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_1_dpitchm1_bsize     16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_1_vwidth_minus_one_byte_offset   52  /* 0x00000034 */
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_1_vwidth_minus_one_bsize     16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_1_vheight_minus_one_byte_offset  54  /* 0x00000036 */
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_1_vheight_minus_one_bsize    16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_1_drotate_bit_offset     486 /* 0x000001e6 */
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_1_hflip_bit_offset   487 /* 0x000001e7 */
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_1_vflip_bit_offset   488 /* 0x000001e8 */
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_1_dflip_bit_offset   489 /* 0x000001e9 */
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_1_pflip_bit_offset   490 /* 0x000001ea */
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_1_W  160
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_1_H  64
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_1_D  512
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_1_P  1
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_1_data_num_bytes     2
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_1_dpitch_num_bytes   32
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_1_dram_format    3
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_1_vp_interleave_mode     2
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_1_tile_width     4
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_1_tile_height    1
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_1_vmem_buffer_num_bytes  106496
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_1_dram_size_num_bytes    10485760
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_1_sign   0
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_1_datasize   1
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_1_expoffset  12
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_1_expbits    0
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_1_drotate    0
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_1_hflip  0
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_1_vflip  0
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_1_dflip  0
#define bisenet_pelee_picinfo__prim_split_7_ResizeBilinear_1_pflip  0

/* bisenet_pelee_picinfo__prim_split_7 HMB_input Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2 */
#define bisenet_pelee_picinfo__prim_split_7_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_cnngen_demangled_name     "Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2"
#define bisenet_pelee_picinfo__prim_split_7_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_is_constant   0
#define bisenet_pelee_picinfo__prim_split_7_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_ddi_byte_size     0   /* 0x00000000 */
#define bisenet_pelee_picinfo__prim_split_7_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_ddi_byte_offset   4294967295  /* 0xffffffff */
#define bisenet_pelee_picinfo__prim_split_7_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_byte_offset   76  /* 0x0000004c */
#define bisenet_pelee_picinfo__prim_split_7_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_vbase_byte_offset     80  /* 0x00000050 */
#define bisenet_pelee_picinfo__prim_split_7_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_dbase_byte_offset     104 /* 0x00000068 */
#define bisenet_pelee_picinfo__prim_split_7_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_dpitchm1_byte_offset  108 /* 0x0000006c */
#define bisenet_pelee_picinfo__prim_split_7_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_dpitchm1_bsize    16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_7_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_vwidth_minus_one_byte_offset  92  /* 0x0000005c */
#define bisenet_pelee_picinfo__prim_split_7_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_vwidth_minus_one_bsize    16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_7_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_vheight_minus_one_byte_offset     94  /* 0x0000005e */
#define bisenet_pelee_picinfo__prim_split_7_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_vheight_minus_one_bsize   16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_7_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_drotate_bit_offset    806 /* 0x00000326 */
#define bisenet_pelee_picinfo__prim_split_7_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_hflip_bit_offset  807 /* 0x00000327 */
#define bisenet_pelee_picinfo__prim_split_7_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_vflip_bit_offset  808 /* 0x00000328 */
#define bisenet_pelee_picinfo__prim_split_7_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_dflip_bit_offset  809 /* 0x00000329 */
#define bisenet_pelee_picinfo__prim_split_7_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_pflip_bit_offset  810 /* 0x0000032a */
#define bisenet_pelee_picinfo__prim_split_7_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_W     160
#define bisenet_pelee_picinfo__prim_split_7_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_H     64
#define bisenet_pelee_picinfo__prim_split_7_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_D     256
#define bisenet_pelee_picinfo__prim_split_7_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_P     1
#define bisenet_pelee_picinfo__prim_split_7_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_data_num_bytes    2
#define bisenet_pelee_picinfo__prim_split_7_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_dpitch_num_bytes  32
#define bisenet_pelee_picinfo__prim_split_7_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_dram_format   3
#define bisenet_pelee_picinfo__prim_split_7_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_vp_interleave_mode    2
#define bisenet_pelee_picinfo__prim_split_7_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_tile_width    4
#define bisenet_pelee_picinfo__prim_split_7_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_tile_height   1
#define bisenet_pelee_picinfo__prim_split_7_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_vmem_buffer_num_bytes     36864
#define bisenet_pelee_picinfo__prim_split_7_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_dram_size_num_bytes   5242880
#define bisenet_pelee_picinfo__prim_split_7_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_sign  0
#define bisenet_pelee_picinfo__prim_split_7_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_datasize  1
#define bisenet_pelee_picinfo__prim_split_7_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_expoffset     12
#define bisenet_pelee_picinfo__prim_split_7_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_expbits   0
#define bisenet_pelee_picinfo__prim_split_7_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_drotate   0
#define bisenet_pelee_picinfo__prim_split_7_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_hflip     0
#define bisenet_pelee_picinfo__prim_split_7_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_vflip     0
#define bisenet_pelee_picinfo__prim_split_7_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_dflip     0
#define bisenet_pelee_picinfo__prim_split_7_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_pflip     0

/* bisenet_pelee_picinfo__prim_split_7 SMB_input __pvcn_1018_ */
#define bisenet_pelee_picinfo__prim_split_7___pvcn_1018__cnngen_demangled_name  "__pvcn_1018_"
#define bisenet_pelee_picinfo__prim_split_7___pvcn_1018__is_constant    1
#define bisenet_pelee_picinfo__prim_split_7___pvcn_1018__byte_offset    116 /* 0x00000074 */
#define bisenet_pelee_picinfo__prim_split_7___pvcn_1018__vbase_byte_offset  120 /* 0x00000078 */
#define bisenet_pelee_picinfo__prim_split_7___pvcn_1018__dbase_byte_offset  144 /* 0x00000090 */
#define bisenet_pelee_picinfo__prim_split_7___pvcn_1018__dpitchm1_byte_offset   148 /* 0x00000094 */
#define bisenet_pelee_picinfo__prim_split_7___pvcn_1018__dpitchm1_bsize     16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_7___pvcn_1018__W  30652
#define bisenet_pelee_picinfo__prim_split_7___pvcn_1018__H  1
#define bisenet_pelee_picinfo__prim_split_7___pvcn_1018__D  1
#define bisenet_pelee_picinfo__prim_split_7___pvcn_1018__P  1
#define bisenet_pelee_picinfo__prim_split_7___pvcn_1018__data_num_bytes     1
#define bisenet_pelee_picinfo__prim_split_7___pvcn_1018__dpitch_num_bytes   30656
#define bisenet_pelee_picinfo__prim_split_7___pvcn_1018__dram_format    0
#define bisenet_pelee_picinfo__prim_split_7___pvcn_1018__vp_interleave_mode     0
#define bisenet_pelee_picinfo__prim_split_7___pvcn_1018__tile_width     1
#define bisenet_pelee_picinfo__prim_split_7___pvcn_1018__tile_height    1
#define bisenet_pelee_picinfo__prim_split_7___pvcn_1018__vmem_buffer_num_bytes  30652

/* bisenet_pelee_picinfo__prim_split_7 SMB_input __pvcn_1019_ */
#define bisenet_pelee_picinfo__prim_split_7___pvcn_1019__cnngen_demangled_name  "__pvcn_1019_"
#define bisenet_pelee_picinfo__prim_split_7___pvcn_1019__is_constant    1
#define bisenet_pelee_picinfo__prim_split_7___pvcn_1019__byte_offset    156 /* 0x0000009c */
#define bisenet_pelee_picinfo__prim_split_7___pvcn_1019__vbase_byte_offset  160 /* 0x000000a0 */
#define bisenet_pelee_picinfo__prim_split_7___pvcn_1019__dbase_byte_offset  184 /* 0x000000b8 */
#define bisenet_pelee_picinfo__prim_split_7___pvcn_1019__dpitchm1_byte_offset   188 /* 0x000000bc */
#define bisenet_pelee_picinfo__prim_split_7___pvcn_1019__dpitchm1_bsize     16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_7___pvcn_1019__W  32
#define bisenet_pelee_picinfo__prim_split_7___pvcn_1019__H  1
#define bisenet_pelee_picinfo__prim_split_7___pvcn_1019__D  1
#define bisenet_pelee_picinfo__prim_split_7___pvcn_1019__P  1
#define bisenet_pelee_picinfo__prim_split_7___pvcn_1019__data_num_bytes     1
#define bisenet_pelee_picinfo__prim_split_7___pvcn_1019__dpitch_num_bytes   32
#define bisenet_pelee_picinfo__prim_split_7___pvcn_1019__dram_format    0
#define bisenet_pelee_picinfo__prim_split_7___pvcn_1019__vp_interleave_mode     0
#define bisenet_pelee_picinfo__prim_split_7___pvcn_1019__tile_width     1
#define bisenet_pelee_picinfo__prim_split_7___pvcn_1019__tile_height    1
#define bisenet_pelee_picinfo__prim_split_7___pvcn_1019__vmem_buffer_num_bytes  32

/* bisenet_pelee_picinfo__prim_split_7 VCB concat */
#define bisenet_pelee_picinfo__prim_split_7_concat_cnngen_demangled_name    "concat"
#define bisenet_pelee_picinfo__prim_split_7_concat_byte_offset  196 /* 0x000000c4 */
#define bisenet_pelee_picinfo__prim_split_7_concat_vbase_byte_offset    204 /* 0x000000cc */
#define bisenet_pelee_picinfo__prim_split_7_concat_vmem_buffer_num_bytes    208896

/* bisenet_pelee_picinfo__prim_split_7 VCB concat_1 */
#define bisenet_pelee_picinfo__prim_split_7_concat_1_cnngen_demangled_name  "concat_1"
#define bisenet_pelee_picinfo__prim_split_7_concat_1_byte_offset    300 /* 0x0000012c */
#define bisenet_pelee_picinfo__prim_split_7_concat_1_vbase_byte_offset  308 /* 0x00000134 */
#define bisenet_pelee_picinfo__prim_split_7_concat_1_vmem_buffer_num_bytes  229376

/* bisenet_pelee_picinfo__prim_split_7 VCB concat_1_1008_ */
#define bisenet_pelee_picinfo__prim_split_7_concat_1_1008__cnngen_demangled_name    "concat_1_1008_"
#define bisenet_pelee_picinfo__prim_split_7_concat_1_1008__byte_offset  352 /* 0x00000160 */
#define bisenet_pelee_picinfo__prim_split_7_concat_1_1008__vbase_byte_offset    360 /* 0x00000168 */
#define bisenet_pelee_picinfo__prim_split_7_concat_1_1008__vmem_buffer_num_bytes    1310720

/* bisenet_pelee_picinfo__prim_split_7 HMB_output concat_1_1013_ */
#define bisenet_pelee_picinfo__prim_split_7_concat_1_1013__cnngen_demangled_name    "concat_1_1013_"
#define bisenet_pelee_picinfo__prim_split_7_concat_1_1013__byte_offset  396 /* 0x0000018c */
#define bisenet_pelee_picinfo__prim_split_7_concat_1_1013__vbase_byte_offset    400 /* 0x00000190 */
#define bisenet_pelee_picinfo__prim_split_7_concat_1_1013__dbase_byte_offset    424 /* 0x000001a8 */
#define bisenet_pelee_picinfo__prim_split_7_concat_1_1013__dpitchm1_byte_offset     428 /* 0x000001ac */
#define bisenet_pelee_picinfo__prim_split_7_concat_1_1013__dpitchm1_bsize   16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_7_concat_1_1013__vwidth_minus_one_byte_offset     412 /* 0x0000019c */
#define bisenet_pelee_picinfo__prim_split_7_concat_1_1013__vwidth_minus_one_bsize   16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_7_concat_1_1013__vheight_minus_one_byte_offset    414 /* 0x0000019e */
#define bisenet_pelee_picinfo__prim_split_7_concat_1_1013__vheight_minus_one_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_7_concat_1_1013__drotate_bit_offset   3366    /* 0x00000d26 */
#define bisenet_pelee_picinfo__prim_split_7_concat_1_1013__hflip_bit_offset     3367    /* 0x00000d27 */
#define bisenet_pelee_picinfo__prim_split_7_concat_1_1013__vflip_bit_offset     3368    /* 0x00000d28 */
#define bisenet_pelee_picinfo__prim_split_7_concat_1_1013__dflip_bit_offset     3369    /* 0x00000d29 */
#define bisenet_pelee_picinfo__prim_split_7_concat_1_1013__pflip_bit_offset     3370    /* 0x00000d2a */
#define bisenet_pelee_picinfo__prim_split_7_concat_1_1013__W    81
#define bisenet_pelee_picinfo__prim_split_7_concat_1_1013__H    64
#define bisenet_pelee_picinfo__prim_split_7_concat_1_1013__D    1024
#define bisenet_pelee_picinfo__prim_split_7_concat_1_1013__P    1
#define bisenet_pelee_picinfo__prim_split_7_concat_1_1013__data_num_bytes   2
#define bisenet_pelee_picinfo__prim_split_7_concat_1_1013__dpitch_num_bytes     32
#define bisenet_pelee_picinfo__prim_split_7_concat_1_1013__dram_format  3
#define bisenet_pelee_picinfo__prim_split_7_concat_1_1013__vp_interleave_mode   2
#define bisenet_pelee_picinfo__prim_split_7_concat_1_1013__tile_width   4
#define bisenet_pelee_picinfo__prim_split_7_concat_1_1013__tile_height  1
#define bisenet_pelee_picinfo__prim_split_7_concat_1_1013__vmem_buffer_num_bytes    49152
#define bisenet_pelee_picinfo__prim_split_7_concat_1_1013__dram_size_num_bytes  11534336
#define bisenet_pelee_picinfo__prim_split_7_concat_1_1013__sign     0
#define bisenet_pelee_picinfo__prim_split_7_concat_1_1013__datasize     1
#define bisenet_pelee_picinfo__prim_split_7_concat_1_1013__expoffset    12
#define bisenet_pelee_picinfo__prim_split_7_concat_1_1013__expbits  0
#define bisenet_pelee_picinfo__prim_split_7_concat_1_1013__drotate  0
#define bisenet_pelee_picinfo__prim_split_7_concat_1_1013__hflip    0
#define bisenet_pelee_picinfo__prim_split_7_concat_1_1013__vflip    0
#define bisenet_pelee_picinfo__prim_split_7_concat_1_1013__dflip    0
#define bisenet_pelee_picinfo__prim_split_7_concat_1_1013__pflip    0

/* bisenet_pelee_picinfo__prim_split_7 HMB_output Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___154___BatchNorm_5__FusedBatchNorm___Relu_3_1012_ */
#define bisenet_pelee_picinfo__prim_split_7_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___154___BatchNorm_5__FusedBatchNorm___Relu_3_1012__cnngen_demangled_name  "Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___154___BatchNorm_5__FusedBatchNorm___Relu_3_1012_"
#define bisenet_pelee_picinfo__prim_split_7_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___154___BatchNorm_5__FusedBatchNorm___Relu_3_1012__byte_offset    452 /* 0x000001c4 */
#define bisenet_pelee_picinfo__prim_split_7_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___154___BatchNorm_5__FusedBatchNorm___Relu_3_1012__vbase_byte_offset  456 /* 0x000001c8 */
#define bisenet_pelee_picinfo__prim_split_7_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___154___BatchNorm_5__FusedBatchNorm___Relu_3_1012__dbase_byte_offset  480 /* 0x000001e0 */
#define bisenet_pelee_picinfo__prim_split_7_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___154___BatchNorm_5__FusedBatchNorm___Relu_3_1012__dpitchm1_byte_offset   484 /* 0x000001e4 */
#define bisenet_pelee_picinfo__prim_split_7_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___154___BatchNorm_5__FusedBatchNorm___Relu_3_1012__dpitchm1_bsize     16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_7_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___154___BatchNorm_5__FusedBatchNorm___Relu_3_1012__vwidth_minus_one_byte_offset   468 /* 0x000001d4 */
#define bisenet_pelee_picinfo__prim_split_7_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___154___BatchNorm_5__FusedBatchNorm___Relu_3_1012__vwidth_minus_one_bsize     16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_7_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___154___BatchNorm_5__FusedBatchNorm___Relu_3_1012__vheight_minus_one_byte_offset  470 /* 0x000001d6 */
#define bisenet_pelee_picinfo__prim_split_7_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___154___BatchNorm_5__FusedBatchNorm___Relu_3_1012__vheight_minus_one_bsize    16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_7_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___154___BatchNorm_5__FusedBatchNorm___Relu_3_1012__drotate_bit_offset     3814    /* 0x00000ee6 */
#define bisenet_pelee_picinfo__prim_split_7_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___154___BatchNorm_5__FusedBatchNorm___Relu_3_1012__hflip_bit_offset   3815    /* 0x00000ee7 */
#define bisenet_pelee_picinfo__prim_split_7_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___154___BatchNorm_5__FusedBatchNorm___Relu_3_1012__vflip_bit_offset   3816    /* 0x00000ee8 */
#define bisenet_pelee_picinfo__prim_split_7_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___154___BatchNorm_5__FusedBatchNorm___Relu_3_1012__dflip_bit_offset   3817    /* 0x00000ee9 */
#define bisenet_pelee_picinfo__prim_split_7_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___154___BatchNorm_5__FusedBatchNorm___Relu_3_1012__pflip_bit_offset   3818    /* 0x00000eea */
#define bisenet_pelee_picinfo__prim_split_7_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___154___BatchNorm_5__FusedBatchNorm___Relu_3_1012__W  80
#define bisenet_pelee_picinfo__prim_split_7_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___154___BatchNorm_5__FusedBatchNorm___Relu_3_1012__H  64
#define bisenet_pelee_picinfo__prim_split_7_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___154___BatchNorm_5__FusedBatchNorm___Relu_3_1012__D  5
#define bisenet_pelee_picinfo__prim_split_7_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___154___BatchNorm_5__FusedBatchNorm___Relu_3_1012__P  1
#define bisenet_pelee_picinfo__prim_split_7_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___154___BatchNorm_5__FusedBatchNorm___Relu_3_1012__data_num_bytes     1
#define bisenet_pelee_picinfo__prim_split_7_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___154___BatchNorm_5__FusedBatchNorm___Relu_3_1012__dpitch_num_bytes   32
#define bisenet_pelee_picinfo__prim_split_7_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___154___BatchNorm_5__FusedBatchNorm___Relu_3_1012__dram_format    3
#define bisenet_pelee_picinfo__prim_split_7_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___154___BatchNorm_5__FusedBatchNorm___Relu_3_1012__vp_interleave_mode     2
#define bisenet_pelee_picinfo__prim_split_7_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___154___BatchNorm_5__FusedBatchNorm___Relu_3_1012__tile_width     4
#define bisenet_pelee_picinfo__prim_split_7_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___154___BatchNorm_5__FusedBatchNorm___Relu_3_1012__tile_height    1
#define bisenet_pelee_picinfo__prim_split_7_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___154___BatchNorm_5__FusedBatchNorm___Relu_3_1012__vmem_buffer_num_bytes  16080
#define bisenet_pelee_picinfo__prim_split_7_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___154___BatchNorm_5__FusedBatchNorm___Relu_3_1012__dram_size_num_bytes    25600
#define bisenet_pelee_picinfo__prim_split_7_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___154___BatchNorm_5__FusedBatchNorm___Relu_3_1012__sign   0
#define bisenet_pelee_picinfo__prim_split_7_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___154___BatchNorm_5__FusedBatchNorm___Relu_3_1012__datasize   0
#define bisenet_pelee_picinfo__prim_split_7_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___154___BatchNorm_5__FusedBatchNorm___Relu_3_1012__expoffset  5
#define bisenet_pelee_picinfo__prim_split_7_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___154___BatchNorm_5__FusedBatchNorm___Relu_3_1012__expbits    0
#define bisenet_pelee_picinfo__prim_split_7_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___154___BatchNorm_5__FusedBatchNorm___Relu_3_1012__drotate    0
#define bisenet_pelee_picinfo__prim_split_7_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___154___BatchNorm_5__FusedBatchNorm___Relu_3_1012__hflip  0
#define bisenet_pelee_picinfo__prim_split_7_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___154___BatchNorm_5__FusedBatchNorm___Relu_3_1012__vflip  0
#define bisenet_pelee_picinfo__prim_split_7_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___154___BatchNorm_5__FusedBatchNorm___Relu_3_1012__dflip  0
#define bisenet_pelee_picinfo__prim_split_7_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___154___BatchNorm_5__FusedBatchNorm___Relu_3_1012__pflip  0


#endif /* bisenet_pelee_picinfo__prim_split_7_H */
