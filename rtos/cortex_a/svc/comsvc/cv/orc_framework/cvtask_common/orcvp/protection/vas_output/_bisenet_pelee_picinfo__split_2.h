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
#ifndef bisenet_pelee_picinfo__prim_split_2_H
#define bisenet_pelee_picinfo__prim_split_2_H

#define vas_gen_header_version                      5

/* dag name */
#define bisenet_pelee_picinfo__prim_split_2_DAG_NAME    "bisenet_pelee_picinfo__prim_split_2"

/* VDG file info */
#define bisenet_pelee_picinfo__prim_split_2_vdg_name    "bisenet_pelee_picinfo__split_2.vdg"

/* VDG DAG memory info */
#define bisenet_pelee_picinfo__prim_split_2_byte_offset     340 /* 0x00000154 */
#define bisenet_pelee_picinfo__prim_split_2_byte_size   1968    /* 0x000007b0 */
#define bisenet_pelee_picinfo__prim_split_2_preferred_dram_xfer_size    1

/* bisenet_pelee_picinfo__prim_split_2 VMEM info */
#define bisenet_pelee_picinfo__prim_split_2_use_ping_pong_vmem  0
#define bisenet_pelee_picinfo__prim_split_2_VMEM_start  4096    /* 0x00001000 */
#define bisenet_pelee_picinfo__prim_split_2_VMEM_end    2074032 /* 0x001fa5b0 */
#define bisenet_pelee_picinfo__prim_split_2_image_start     1688320 /* 0x0019c300 */
#define bisenet_pelee_picinfo__prim_split_2_image_size  387680  /* 0x0005ea60 */
#define bisenet_pelee_picinfo__prim_split_2_dagbin_start    2074032 /* 0x001fa5b0 */

/*******************************************************************************
 * DAG byte offset info for HMB/SMB/VCB and their fields
 * in DAG 'bisenet_pelee_picinfo__prim_split_2' in source file 'bisenet_pelee_picinfo__split_2.vas'
 ******************************************************************************/
/* bisenet_pelee_picinfo__prim_split_2 HMB_input PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool */
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_cnngen_demangled_name     "PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool"
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_is_constant   0
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_ddi_byte_size     0   /* 0x00000000 */
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_ddi_byte_offset   4294967295  /* 0xffffffff */
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_byte_offset   0   /* 0x00000000 */
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_vbase_byte_offset     4   /* 0x00000004 */
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_dbase_byte_offset     28  /* 0x0000001c */
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_dpitchm1_byte_offset  32  /* 0x00000020 */
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_dpitchm1_bsize    16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_vwidth_minus_one_byte_offset  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_vwidth_minus_one_bsize    16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_vheight_minus_one_byte_offset     18  /* 0x00000012 */
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_vheight_minus_one_bsize   16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_drotate_bit_offset    198 /* 0x000000c6 */
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_hflip_bit_offset  199 /* 0x000000c7 */
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_vflip_bit_offset  200 /* 0x000000c8 */
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_dflip_bit_offset  201 /* 0x000000c9 */
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_pflip_bit_offset  202 /* 0x000000ca */
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_W     80
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_H     32
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_D     256
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_P     1
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_data_num_bytes    1
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_dpitch_num_bytes  32
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_dram_format   3
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_vp_interleave_mode    2
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_tile_width    4
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_tile_height   1
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_vmem_buffer_num_bytes     86016
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_dram_size_num_bytes   655360
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_sign  0
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_datasize  0
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_expoffset     5
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_expbits   0
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_drotate   0
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_hflip     0
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_vflip     0
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_dflip     0
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_pflip     0

/* bisenet_pelee_picinfo__prim_split_2 HMB_input __vcn_957_ */
#define bisenet_pelee_picinfo__prim_split_2___vcn_957__cnngen_demangled_name    "__vcn_957_"
#define bisenet_pelee_picinfo__prim_split_2___vcn_957__is_constant  0
#define bisenet_pelee_picinfo__prim_split_2___vcn_957__ddi_byte_size    0   /* 0x00000000 */
#define bisenet_pelee_picinfo__prim_split_2___vcn_957__ddi_byte_offset  4294967295  /* 0xffffffff */
#define bisenet_pelee_picinfo__prim_split_2___vcn_957__byte_offset  40  /* 0x00000028 */
#define bisenet_pelee_picinfo__prim_split_2___vcn_957__vbase_byte_offset    44  /* 0x0000002c */
#define bisenet_pelee_picinfo__prim_split_2___vcn_957__dbase_byte_offset    68  /* 0x00000044 */
#define bisenet_pelee_picinfo__prim_split_2___vcn_957__dpitchm1_byte_offset     72  /* 0x00000048 */
#define bisenet_pelee_picinfo__prim_split_2___vcn_957__dpitchm1_bsize   16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_2___vcn_957__vwidth_minus_one_byte_offset     56  /* 0x00000038 */
#define bisenet_pelee_picinfo__prim_split_2___vcn_957__vwidth_minus_one_bsize   16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_2___vcn_957__vheight_minus_one_byte_offset    58  /* 0x0000003a */
#define bisenet_pelee_picinfo__prim_split_2___vcn_957__vheight_minus_one_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_2___vcn_957__drotate_bit_offset   518 /* 0x00000206 */
#define bisenet_pelee_picinfo__prim_split_2___vcn_957__hflip_bit_offset     519 /* 0x00000207 */
#define bisenet_pelee_picinfo__prim_split_2___vcn_957__vflip_bit_offset     520 /* 0x00000208 */
#define bisenet_pelee_picinfo__prim_split_2___vcn_957__dflip_bit_offset     521 /* 0x00000209 */
#define bisenet_pelee_picinfo__prim_split_2___vcn_957__pflip_bit_offset     522 /* 0x0000020a */
#define bisenet_pelee_picinfo__prim_split_2___vcn_957__W    80
#define bisenet_pelee_picinfo__prim_split_2___vcn_957__H    32
#define bisenet_pelee_picinfo__prim_split_2___vcn_957__D    256
#define bisenet_pelee_picinfo__prim_split_2___vcn_957__P    1
#define bisenet_pelee_picinfo__prim_split_2___vcn_957__data_num_bytes   1
#define bisenet_pelee_picinfo__prim_split_2___vcn_957__dpitch_num_bytes     32
#define bisenet_pelee_picinfo__prim_split_2___vcn_957__dram_format  3
#define bisenet_pelee_picinfo__prim_split_2___vcn_957__vp_interleave_mode   2
#define bisenet_pelee_picinfo__prim_split_2___vcn_957__tile_width   4
#define bisenet_pelee_picinfo__prim_split_2___vcn_957__tile_height  1
#define bisenet_pelee_picinfo__prim_split_2___vcn_957__vmem_buffer_num_bytes    40960
#define bisenet_pelee_picinfo__prim_split_2___vcn_957__dram_size_num_bytes  655360
#define bisenet_pelee_picinfo__prim_split_2___vcn_957__sign     0
#define bisenet_pelee_picinfo__prim_split_2___vcn_957__datasize     0
#define bisenet_pelee_picinfo__prim_split_2___vcn_957__expoffset    5
#define bisenet_pelee_picinfo__prim_split_2___vcn_957__expbits  0
#define bisenet_pelee_picinfo__prim_split_2___vcn_957__drotate  0
#define bisenet_pelee_picinfo__prim_split_2___vcn_957__hflip    0
#define bisenet_pelee_picinfo__prim_split_2___vcn_957__vflip    0
#define bisenet_pelee_picinfo__prim_split_2___vcn_957__dflip    0
#define bisenet_pelee_picinfo__prim_split_2___vcn_957__pflip    0

/* bisenet_pelee_picinfo__prim_split_2 HMB_input Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___5___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm */
#define bisenet_pelee_picinfo__prim_split_2_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___5___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm_cnngen_demangled_name    "Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___5___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm"
#define bisenet_pelee_picinfo__prim_split_2_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___5___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm_is_constant  0
#define bisenet_pelee_picinfo__prim_split_2_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___5___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm_ddi_byte_size    0   /* 0x00000000 */
#define bisenet_pelee_picinfo__prim_split_2_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___5___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm_ddi_byte_offset  4294967295  /* 0xffffffff */
#define bisenet_pelee_picinfo__prim_split_2_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___5___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm_byte_offset  80  /* 0x00000050 */
#define bisenet_pelee_picinfo__prim_split_2_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___5___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm_vbase_byte_offset    84  /* 0x00000054 */
#define bisenet_pelee_picinfo__prim_split_2_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___5___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm_dbase_byte_offset    108 /* 0x0000006c */
#define bisenet_pelee_picinfo__prim_split_2_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___5___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm_dpitchm1_byte_offset     112 /* 0x00000070 */
#define bisenet_pelee_picinfo__prim_split_2_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___5___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm_dpitchm1_bsize   16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_2_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___5___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm_vwidth_minus_one_byte_offset     96  /* 0x00000060 */
#define bisenet_pelee_picinfo__prim_split_2_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___5___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm_vwidth_minus_one_bsize   16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_2_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___5___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm_vheight_minus_one_byte_offset    98  /* 0x00000062 */
#define bisenet_pelee_picinfo__prim_split_2_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___5___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm_vheight_minus_one_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_2_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___5___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm_drotate_bit_offset   838 /* 0x00000346 */
#define bisenet_pelee_picinfo__prim_split_2_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___5___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm_hflip_bit_offset     839 /* 0x00000347 */
#define bisenet_pelee_picinfo__prim_split_2_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___5___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm_vflip_bit_offset     840 /* 0x00000348 */
#define bisenet_pelee_picinfo__prim_split_2_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___5___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm_dflip_bit_offset     841 /* 0x00000349 */
#define bisenet_pelee_picinfo__prim_split_2_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___5___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm_pflip_bit_offset     842 /* 0x0000034a */
#define bisenet_pelee_picinfo__prim_split_2_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___5___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm_W    640
#define bisenet_pelee_picinfo__prim_split_2_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___5___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm_H    256
#define bisenet_pelee_picinfo__prim_split_2_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___5___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm_D    96
#define bisenet_pelee_picinfo__prim_split_2_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___5___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm_P    1
#define bisenet_pelee_picinfo__prim_split_2_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___5___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm_data_num_bytes   1
#define bisenet_pelee_picinfo__prim_split_2_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___5___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm_dpitch_num_bytes     32
#define bisenet_pelee_picinfo__prim_split_2_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___5___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm_dram_format  3
#define bisenet_pelee_picinfo__prim_split_2_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___5___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm_vp_interleave_mode   2
#define bisenet_pelee_picinfo__prim_split_2_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___5___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm_tile_width   4
#define bisenet_pelee_picinfo__prim_split_2_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___5___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm_tile_height  1
#define bisenet_pelee_picinfo__prim_split_2_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___5___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm_vmem_buffer_num_bytes    13824
#define bisenet_pelee_picinfo__prim_split_2_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___5___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm_dram_size_num_bytes  15728640
#define bisenet_pelee_picinfo__prim_split_2_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___5___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm_sign     1
#define bisenet_pelee_picinfo__prim_split_2_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___5___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm_datasize     0
#define bisenet_pelee_picinfo__prim_split_2_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___5___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm_expoffset    4
#define bisenet_pelee_picinfo__prim_split_2_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___5___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm_expbits  0
#define bisenet_pelee_picinfo__prim_split_2_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___5___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm_drotate  0
#define bisenet_pelee_picinfo__prim_split_2_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___5___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm_hflip    0
#define bisenet_pelee_picinfo__prim_split_2_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___5___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm_vflip    0
#define bisenet_pelee_picinfo__prim_split_2_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___5___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm_dflip    0
#define bisenet_pelee_picinfo__prim_split_2_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___5___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm_pflip    0

/* bisenet_pelee_picinfo__prim_split_2 SMB_input __pvcn_981_ */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_981__cnngen_demangled_name   "__pvcn_981_"
#define bisenet_pelee_picinfo__prim_split_2___pvcn_981__is_constant     1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_981__byte_offset     116 /* 0x00000074 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_981__vbase_byte_offset   120 /* 0x00000078 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_981__dbase_byte_offset   144 /* 0x00000090 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_981__dpitchm1_byte_offset    148 /* 0x00000094 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_981__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_981__W   20804
#define bisenet_pelee_picinfo__prim_split_2___pvcn_981__H   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_981__D   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_981__P   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_981__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_981__dpitch_num_bytes    20832
#define bisenet_pelee_picinfo__prim_split_2___pvcn_981__dram_format     0
#define bisenet_pelee_picinfo__prim_split_2___pvcn_981__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_2___pvcn_981__tile_width  1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_981__tile_height     1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_981__vmem_buffer_num_bytes   20804

/* bisenet_pelee_picinfo__prim_split_2 SMB_input __pvcn_983_ */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_983__cnngen_demangled_name   "__pvcn_983_"
#define bisenet_pelee_picinfo__prim_split_2___pvcn_983__is_constant     1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_983__byte_offset     156 /* 0x0000009c */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_983__vbase_byte_offset   160 /* 0x000000a0 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_983__dbase_byte_offset   184 /* 0x000000b8 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_983__dpitchm1_byte_offset    188 /* 0x000000bc */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_983__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_983__W   20804
#define bisenet_pelee_picinfo__prim_split_2___pvcn_983__H   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_983__D   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_983__P   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_983__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_983__dpitch_num_bytes    20832
#define bisenet_pelee_picinfo__prim_split_2___pvcn_983__dram_format     0
#define bisenet_pelee_picinfo__prim_split_2___pvcn_983__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_2___pvcn_983__tile_width  1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_983__tile_height     1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_983__vmem_buffer_num_bytes   20804

/* bisenet_pelee_picinfo__prim_split_2 SMB_input __pvcn_982_ */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_982__cnngen_demangled_name   "__pvcn_982_"
#define bisenet_pelee_picinfo__prim_split_2___pvcn_982__is_constant     1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_982__byte_offset     196 /* 0x000000c4 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_982__vbase_byte_offset   200 /* 0x000000c8 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_982__dbase_byte_offset   224 /* 0x000000e0 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_982__dpitchm1_byte_offset    228 /* 0x000000e4 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_982__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_982__W   640
#define bisenet_pelee_picinfo__prim_split_2___pvcn_982__H   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_982__D   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_982__P   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_982__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_982__dpitch_num_bytes    640
#define bisenet_pelee_picinfo__prim_split_2___pvcn_982__dram_format     0
#define bisenet_pelee_picinfo__prim_split_2___pvcn_982__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_2___pvcn_982__tile_width  1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_982__tile_height     1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_982__vmem_buffer_num_bytes   640

/* bisenet_pelee_picinfo__prim_split_2 SMB_input __pvcn_984_ */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_984__cnngen_demangled_name   "__pvcn_984_"
#define bisenet_pelee_picinfo__prim_split_2___pvcn_984__is_constant     1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_984__byte_offset     236 /* 0x000000ec */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_984__vbase_byte_offset   240 /* 0x000000f0 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_984__dbase_byte_offset   264 /* 0x00000108 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_984__dpitchm1_byte_offset    268 /* 0x0000010c */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_984__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_984__W   640
#define bisenet_pelee_picinfo__prim_split_2___pvcn_984__H   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_984__D   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_984__P   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_984__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_984__dpitch_num_bytes    640
#define bisenet_pelee_picinfo__prim_split_2___pvcn_984__dram_format     0
#define bisenet_pelee_picinfo__prim_split_2___pvcn_984__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_2___pvcn_984__tile_width  1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_984__tile_height     1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_984__vmem_buffer_num_bytes   640

/* bisenet_pelee_picinfo__prim_split_2 SMB_input __pvcn_553_ */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_553__cnngen_demangled_name   "__pvcn_553_"
#define bisenet_pelee_picinfo__prim_split_2___pvcn_553__is_constant     1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_553__byte_offset     276 /* 0x00000114 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_553__vbase_byte_offset   280 /* 0x00000118 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_553__dbase_byte_offset   304 /* 0x00000130 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_553__dpitchm1_byte_offset    308 /* 0x00000134 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_553__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_553__W   5780
#define bisenet_pelee_picinfo__prim_split_2___pvcn_553__H   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_553__D   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_553__P   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_553__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_553__dpitch_num_bytes    5792
#define bisenet_pelee_picinfo__prim_split_2___pvcn_553__dram_format     0
#define bisenet_pelee_picinfo__prim_split_2___pvcn_553__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_2___pvcn_553__tile_width  1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_553__tile_height     1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_553__vmem_buffer_num_bytes   5780

/* bisenet_pelee_picinfo__prim_split_2 SMB_input __pvcn_554_ */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_554__cnngen_demangled_name   "__pvcn_554_"
#define bisenet_pelee_picinfo__prim_split_2___pvcn_554__is_constant     1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_554__byte_offset     316 /* 0x0000013c */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_554__vbase_byte_offset   320 /* 0x00000140 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_554__dbase_byte_offset   344 /* 0x00000158 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_554__dpitchm1_byte_offset    348 /* 0x0000015c */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_554__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_554__W   88
#define bisenet_pelee_picinfo__prim_split_2___pvcn_554__H   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_554__D   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_554__P   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_554__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_554__dpitch_num_bytes    96
#define bisenet_pelee_picinfo__prim_split_2___pvcn_554__dram_format     0
#define bisenet_pelee_picinfo__prim_split_2___pvcn_554__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_2___pvcn_554__tile_width  1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_554__tile_height     1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_554__vmem_buffer_num_bytes   88

/* bisenet_pelee_picinfo__prim_split_2 SMB_input __pvcn_557_ */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_557__cnngen_demangled_name   "__pvcn_557_"
#define bisenet_pelee_picinfo__prim_split_2___pvcn_557__is_constant     1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_557__byte_offset     356 /* 0x00000164 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_557__vbase_byte_offset   360 /* 0x00000168 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_557__dbase_byte_offset   384 /* 0x00000180 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_557__dpitchm1_byte_offset    388 /* 0x00000184 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_557__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_557__W   5736
#define bisenet_pelee_picinfo__prim_split_2___pvcn_557__H   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_557__D   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_557__P   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_557__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_557__dpitch_num_bytes    5760
#define bisenet_pelee_picinfo__prim_split_2___pvcn_557__dram_format     0
#define bisenet_pelee_picinfo__prim_split_2___pvcn_557__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_2___pvcn_557__tile_width  1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_557__tile_height     1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_557__vmem_buffer_num_bytes   5736

/* bisenet_pelee_picinfo__prim_split_2 SMB_input __pvcn_558_ */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_558__cnngen_demangled_name   "__pvcn_558_"
#define bisenet_pelee_picinfo__prim_split_2___pvcn_558__is_constant     1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_558__byte_offset     396 /* 0x0000018c */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_558__vbase_byte_offset   400 /* 0x00000190 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_558__dbase_byte_offset   424 /* 0x000001a8 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_558__dpitchm1_byte_offset    428 /* 0x000001ac */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_558__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_558__W   80
#define bisenet_pelee_picinfo__prim_split_2___pvcn_558__H   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_558__D   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_558__P   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_558__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_558__dpitch_num_bytes    96
#define bisenet_pelee_picinfo__prim_split_2___pvcn_558__dram_format     0
#define bisenet_pelee_picinfo__prim_split_2___pvcn_558__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_2___pvcn_558__tile_width  1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_558__tile_height     1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_558__vmem_buffer_num_bytes   80

/* bisenet_pelee_picinfo__prim_split_2 SMB_input __pvcn_560_ */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_560__cnngen_demangled_name   "__pvcn_560_"
#define bisenet_pelee_picinfo__prim_split_2___pvcn_560__is_constant     1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_560__byte_offset     436 /* 0x000001b4 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_560__vbase_byte_offset   440 /* 0x000001b8 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_560__dbase_byte_offset   464 /* 0x000001d0 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_560__dpitchm1_byte_offset    468 /* 0x000001d4 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_560__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_560__W   1452
#define bisenet_pelee_picinfo__prim_split_2___pvcn_560__H   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_560__D   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_560__P   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_560__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_560__dpitch_num_bytes    1472
#define bisenet_pelee_picinfo__prim_split_2___pvcn_560__dram_format     0
#define bisenet_pelee_picinfo__prim_split_2___pvcn_560__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_2___pvcn_560__tile_width  1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_560__tile_height     1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_560__vmem_buffer_num_bytes   1452

/* bisenet_pelee_picinfo__prim_split_2 SMB_input __pvcn_561_ */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_561__cnngen_demangled_name   "__pvcn_561_"
#define bisenet_pelee_picinfo__prim_split_2___pvcn_561__is_constant     1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_561__byte_offset     476 /* 0x000001dc */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_561__vbase_byte_offset   480 /* 0x000001e0 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_561__dbase_byte_offset   504 /* 0x000001f8 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_561__dpitchm1_byte_offset    508 /* 0x000001fc */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_561__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_561__W   80
#define bisenet_pelee_picinfo__prim_split_2___pvcn_561__H   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_561__D   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_561__P   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_561__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_561__dpitch_num_bytes    96
#define bisenet_pelee_picinfo__prim_split_2___pvcn_561__dram_format     0
#define bisenet_pelee_picinfo__prim_split_2___pvcn_561__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_2___pvcn_561__tile_width  1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_561__tile_height     1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_561__vmem_buffer_num_bytes   80

/* bisenet_pelee_picinfo__prim_split_2 SMB_input __pvcn_567_ */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_567__cnngen_demangled_name   "__pvcn_567_"
#define bisenet_pelee_picinfo__prim_split_2___pvcn_567__is_constant     1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_567__byte_offset     516 /* 0x00000204 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_567__vbase_byte_offset   520 /* 0x00000208 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_567__dbase_byte_offset   544 /* 0x00000220 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_567__dpitchm1_byte_offset    548 /* 0x00000224 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_567__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_567__W   23076
#define bisenet_pelee_picinfo__prim_split_2___pvcn_567__H   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_567__D   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_567__P   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_567__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_567__dpitch_num_bytes    23104
#define bisenet_pelee_picinfo__prim_split_2___pvcn_567__dram_format     0
#define bisenet_pelee_picinfo__prim_split_2___pvcn_567__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_2___pvcn_567__tile_width  1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_567__tile_height     1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_567__vmem_buffer_num_bytes   23076

/* bisenet_pelee_picinfo__prim_split_2 SMB_input __pvcn_568_ */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_568__cnngen_demangled_name   "__pvcn_568_"
#define bisenet_pelee_picinfo__prim_split_2___pvcn_568__is_constant     1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_568__byte_offset     556 /* 0x0000022c */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_568__vbase_byte_offset   560 /* 0x00000230 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_568__dbase_byte_offset   584 /* 0x00000248 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_568__dpitchm1_byte_offset    588 /* 0x0000024c */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_568__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_568__W   640
#define bisenet_pelee_picinfo__prim_split_2___pvcn_568__H   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_568__D   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_568__P   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_568__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_568__dpitch_num_bytes    640
#define bisenet_pelee_picinfo__prim_split_2___pvcn_568__dram_format     0
#define bisenet_pelee_picinfo__prim_split_2___pvcn_568__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_2___pvcn_568__tile_width  1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_568__tile_height     1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_568__vmem_buffer_num_bytes   640

/* bisenet_pelee_picinfo__prim_split_2 SMB_input __pvcn_571_ */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_571__cnngen_demangled_name   "__pvcn_571_"
#define bisenet_pelee_picinfo__prim_split_2___pvcn_571__is_constant     1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_571__byte_offset     596 /* 0x00000254 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_571__vbase_byte_offset   600 /* 0x00000258 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_571__dbase_byte_offset   624 /* 0x00000270 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_571__dpitchm1_byte_offset    628 /* 0x00000274 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_571__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_571__W   5752
#define bisenet_pelee_picinfo__prim_split_2___pvcn_571__H   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_571__D   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_571__P   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_571__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_571__dpitch_num_bytes    5760
#define bisenet_pelee_picinfo__prim_split_2___pvcn_571__dram_format     0
#define bisenet_pelee_picinfo__prim_split_2___pvcn_571__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_2___pvcn_571__tile_width  1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_571__tile_height     1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_571__vmem_buffer_num_bytes   5752

/* bisenet_pelee_picinfo__prim_split_2 SMB_input __pvcn_572_ */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_572__cnngen_demangled_name   "__pvcn_572_"
#define bisenet_pelee_picinfo__prim_split_2___pvcn_572__is_constant     1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_572__byte_offset     636 /* 0x0000027c */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_572__vbase_byte_offset   640 /* 0x00000280 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_572__dbase_byte_offset   664 /* 0x00000298 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_572__dpitchm1_byte_offset    668 /* 0x0000029c */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_572__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_572__W   80
#define bisenet_pelee_picinfo__prim_split_2___pvcn_572__H   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_572__D   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_572__P   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_572__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_572__dpitch_num_bytes    96
#define bisenet_pelee_picinfo__prim_split_2___pvcn_572__dram_format     0
#define bisenet_pelee_picinfo__prim_split_2___pvcn_572__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_2___pvcn_572__tile_width  1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_572__tile_height     1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_572__vmem_buffer_num_bytes   80

/* bisenet_pelee_picinfo__prim_split_2 SMB_input __pvcn_575_ */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_575__cnngen_demangled_name   "__pvcn_575_"
#define bisenet_pelee_picinfo__prim_split_2___pvcn_575__is_constant     1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_575__byte_offset     676 /* 0x000002a4 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_575__vbase_byte_offset   680 /* 0x000002a8 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_575__dbase_byte_offset   704 /* 0x000002c0 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_575__dpitchm1_byte_offset    708 /* 0x000002c4 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_575__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_575__W   5740
#define bisenet_pelee_picinfo__prim_split_2___pvcn_575__H   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_575__D   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_575__P   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_575__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_575__dpitch_num_bytes    5760
#define bisenet_pelee_picinfo__prim_split_2___pvcn_575__dram_format     0
#define bisenet_pelee_picinfo__prim_split_2___pvcn_575__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_2___pvcn_575__tile_width  1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_575__tile_height     1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_575__vmem_buffer_num_bytes   5740

/* bisenet_pelee_picinfo__prim_split_2 SMB_input __pvcn_576_ */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_576__cnngen_demangled_name   "__pvcn_576_"
#define bisenet_pelee_picinfo__prim_split_2___pvcn_576__is_constant     1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_576__byte_offset     716 /* 0x000002cc */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_576__vbase_byte_offset   720 /* 0x000002d0 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_576__dbase_byte_offset   744 /* 0x000002e8 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_576__dpitchm1_byte_offset    748 /* 0x000002ec */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_576__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_576__W   88
#define bisenet_pelee_picinfo__prim_split_2___pvcn_576__H   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_576__D   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_576__P   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_576__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_576__dpitch_num_bytes    96
#define bisenet_pelee_picinfo__prim_split_2___pvcn_576__dram_format     0
#define bisenet_pelee_picinfo__prim_split_2___pvcn_576__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_2___pvcn_576__tile_width  1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_576__tile_height     1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_576__vmem_buffer_num_bytes   88

/* bisenet_pelee_picinfo__prim_split_2 SMB_input __pvcn_578_ */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_578__cnngen_demangled_name   "__pvcn_578_"
#define bisenet_pelee_picinfo__prim_split_2___pvcn_578__is_constant     1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_578__byte_offset     756 /* 0x000002f4 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_578__vbase_byte_offset   760 /* 0x000002f8 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_578__dbase_byte_offset   784 /* 0x00000310 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_578__dpitchm1_byte_offset    788 /* 0x00000314 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_578__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_578__W   1408
#define bisenet_pelee_picinfo__prim_split_2___pvcn_578__H   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_578__D   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_578__P   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_578__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_578__dpitch_num_bytes    1408
#define bisenet_pelee_picinfo__prim_split_2___pvcn_578__dram_format     0
#define bisenet_pelee_picinfo__prim_split_2___pvcn_578__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_2___pvcn_578__tile_width  1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_578__tile_height     1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_578__vmem_buffer_num_bytes   1408

/* bisenet_pelee_picinfo__prim_split_2 SMB_input __pvcn_579_ */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_579__cnngen_demangled_name   "__pvcn_579_"
#define bisenet_pelee_picinfo__prim_split_2___pvcn_579__is_constant     1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_579__byte_offset     796 /* 0x0000031c */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_579__vbase_byte_offset   800 /* 0x00000320 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_579__dbase_byte_offset   824 /* 0x00000338 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_579__dpitchm1_byte_offset    828 /* 0x0000033c */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_579__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_579__W   80
#define bisenet_pelee_picinfo__prim_split_2___pvcn_579__H   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_579__D   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_579__P   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_579__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_579__dpitch_num_bytes    96
#define bisenet_pelee_picinfo__prim_split_2___pvcn_579__dram_format     0
#define bisenet_pelee_picinfo__prim_split_2___pvcn_579__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_2___pvcn_579__tile_width  1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_579__tile_height     1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_579__vmem_buffer_num_bytes   80

/* bisenet_pelee_picinfo__prim_split_2 SMB_input __pvcn_833_ */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_833__cnngen_demangled_name   "__pvcn_833_"
#define bisenet_pelee_picinfo__prim_split_2___pvcn_833__is_constant     1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_833__byte_offset     836 /* 0x00000344 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_833__vbase_byte_offset   840 /* 0x00000348 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_833__dbase_byte_offset   864 /* 0x00000360 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_833__dpitchm1_byte_offset    868 /* 0x00000364 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_833__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_833__W   61628
#define bisenet_pelee_picinfo__prim_split_2___pvcn_833__H   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_833__D   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_833__P   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_833__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_833__dpitch_num_bytes    61632
#define bisenet_pelee_picinfo__prim_split_2___pvcn_833__dram_format     0
#define bisenet_pelee_picinfo__prim_split_2___pvcn_833__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_2___pvcn_833__tile_width  1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_833__tile_height     1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_833__vmem_buffer_num_bytes   61628

/* bisenet_pelee_picinfo__prim_split_2 SMB_input __pvcn_834_ */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_834__cnngen_demangled_name   "__pvcn_834_"
#define bisenet_pelee_picinfo__prim_split_2___pvcn_834__is_constant     1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_834__byte_offset     876 /* 0x0000036c */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_834__vbase_byte_offset   880 /* 0x00000370 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_834__dbase_byte_offset   904 /* 0x00000388 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_834__dpitchm1_byte_offset    908 /* 0x0000038c */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_834__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_834__W   704
#define bisenet_pelee_picinfo__prim_split_2___pvcn_834__H   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_834__D   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_834__P   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_834__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_834__dpitch_num_bytes    704
#define bisenet_pelee_picinfo__prim_split_2___pvcn_834__dram_format     0
#define bisenet_pelee_picinfo__prim_split_2___pvcn_834__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_2___pvcn_834__tile_width  1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_834__tile_height     1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_834__vmem_buffer_num_bytes   704

/* bisenet_pelee_picinfo__prim_split_2 SMB_input __pvcn_836_ */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_836__cnngen_demangled_name   "__pvcn_836_"
#define bisenet_pelee_picinfo__prim_split_2___pvcn_836__is_constant     1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_836__byte_offset     916 /* 0x00000394 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_836__vbase_byte_offset   920 /* 0x00000398 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_836__dbase_byte_offset   944 /* 0x000003b0 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_836__dpitchm1_byte_offset    948 /* 0x000003b4 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_836__dpitchm1_bsize  32  /* 0x00000020 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_836__W   228108
#define bisenet_pelee_picinfo__prim_split_2___pvcn_836__H   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_836__D   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_836__P   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_836__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_836__dpitch_num_bytes    228128
#define bisenet_pelee_picinfo__prim_split_2___pvcn_836__dram_format     0
#define bisenet_pelee_picinfo__prim_split_2___pvcn_836__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_2___pvcn_836__tile_width  1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_836__tile_height     1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_836__vmem_buffer_num_bytes   228108

/* bisenet_pelee_picinfo__prim_split_2 SMB_input __pvcn_837_ */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_837__cnngen_demangled_name   "__pvcn_837_"
#define bisenet_pelee_picinfo__prim_split_2___pvcn_837__is_constant     1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_837__byte_offset     956 /* 0x000003bc */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_837__vbase_byte_offset   960 /* 0x000003c0 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_837__dbase_byte_offset   984 /* 0x000003d8 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_837__dpitchm1_byte_offset    988 /* 0x000003dc */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_837__dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_837__W   2304
#define bisenet_pelee_picinfo__prim_split_2___pvcn_837__H   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_837__D   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_837__P   1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_837__data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_837__dpitch_num_bytes    2304
#define bisenet_pelee_picinfo__prim_split_2___pvcn_837__dram_format     0
#define bisenet_pelee_picinfo__prim_split_2___pvcn_837__vp_interleave_mode  0
#define bisenet_pelee_picinfo__prim_split_2___pvcn_837__tile_width  1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_837__tile_height     1
#define bisenet_pelee_picinfo__prim_split_2___pvcn_837__vmem_buffer_num_bytes   2304

/* bisenet_pelee_picinfo__prim_split_2 VCB PeleeNet__stage_3_dense_block_0__concat */
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_0__concat_cnngen_demangled_name   "PeleeNet__stage_3_dense_block_0__concat"
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_0__concat_byte_offset     996 /* 0x000003e4 */
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_0__concat_vbase_byte_offset   1004    /* 0x000003ec */
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_0__concat_vmem_buffer_num_bytes   230400

/* bisenet_pelee_picinfo__prim_split_2 HMB_output PeleeNet__stage_3_dense_block_1__concat */
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_1__concat_cnngen_demangled_name   "PeleeNet__stage_3_dense_block_1__concat"
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_1__concat_byte_offset     1032    /* 0x00000408 */
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_1__concat_vbase_byte_offset   1036    /* 0x0000040c */
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_1__concat_dbase_byte_offset   1060    /* 0x00000424 */
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_1__concat_dpitchm1_byte_offset    1064    /* 0x00000428 */
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_1__concat_dpitchm1_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_1__concat_vwidth_minus_one_byte_offset    1048    /* 0x00000418 */
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_1__concat_vwidth_minus_one_bsize  16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_1__concat_vheight_minus_one_byte_offset   1050    /* 0x0000041a */
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_1__concat_vheight_minus_one_bsize     16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_1__concat_drotate_bit_offset  8454    /* 0x00002106 */
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_1__concat_hflip_bit_offset    8455    /* 0x00002107 */
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_1__concat_vflip_bit_offset    8456    /* 0x00002108 */
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_1__concat_dflip_bit_offset    8457    /* 0x00002109 */
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_1__concat_pflip_bit_offset    8458    /* 0x0000210a */
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_1__concat_W   80
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_1__concat_H   32
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_1__concat_D   320
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_1__concat_P   1
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_1__concat_data_num_bytes  1
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_1__concat_dpitch_num_bytes    32
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_1__concat_dram_format     3
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_1__concat_vp_interleave_mode  2
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_1__concat_tile_width  4
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_1__concat_tile_height     1
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_1__concat_vmem_buffer_num_bytes   107520
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_1__concat_dram_size_num_bytes     819200
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_1__concat_sign    0
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_1__concat_datasize    0
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_1__concat_expoffset   5
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_1__concat_expbits     0
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_1__concat_drotate     0
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_1__concat_hflip   0
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_1__concat_vflip   0
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_1__concat_dflip   0
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_1__concat_pflip   0

/* bisenet_pelee_picinfo__prim_split_2 VCB Relu */
#define bisenet_pelee_picinfo__prim_split_2_Relu_cnngen_demangled_name  "Relu"
#define bisenet_pelee_picinfo__prim_split_2_Relu_byte_offset    1084    /* 0x0000043c */
#define bisenet_pelee_picinfo__prim_split_2_Relu_vbase_byte_offset  1092    /* 0x00000444 */
#define bisenet_pelee_picinfo__prim_split_2_Relu_vmem_buffer_num_bytes  166912

/* bisenet_pelee_picinfo__prim_split_2 VCB Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___8___BatchNorm_1__FusedBatchNorm___Relu_1 */
#define bisenet_pelee_picinfo__prim_split_2_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___8___BatchNorm_1__FusedBatchNorm___Relu_1_cnngen_demangled_name  "Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___8___BatchNorm_1__FusedBatchNorm___Relu_1"
#define bisenet_pelee_picinfo__prim_split_2_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___8___BatchNorm_1__FusedBatchNorm___Relu_1_byte_offset    1140    /* 0x00000474 */
#define bisenet_pelee_picinfo__prim_split_2_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___8___BatchNorm_1__FusedBatchNorm___Relu_1_vbase_byte_offset  1148    /* 0x0000047c */
#define bisenet_pelee_picinfo__prim_split_2_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___8___BatchNorm_1__FusedBatchNorm___Relu_1_vmem_buffer_num_bytes  245760

/* bisenet_pelee_picinfo__prim_split_2 VCB __pvcn_838_ */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_838__cnngen_demangled_name   "__pvcn_838_"
#define bisenet_pelee_picinfo__prim_split_2___pvcn_838__byte_offset     1196    /* 0x000004ac */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_838__vbase_byte_offset   1204    /* 0x000004b4 */
#define bisenet_pelee_picinfo__prim_split_2___pvcn_838__vmem_buffer_num_bytes   331776

/* bisenet_pelee_picinfo__prim_split_2 HMB_output Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2 */
#define bisenet_pelee_picinfo__prim_split_2_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_cnngen_demangled_name     "Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2"
#define bisenet_pelee_picinfo__prim_split_2_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_byte_offset   1240    /* 0x000004d8 */
#define bisenet_pelee_picinfo__prim_split_2_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_vbase_byte_offset     1244    /* 0x000004dc */
#define bisenet_pelee_picinfo__prim_split_2_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_dbase_byte_offset     1268    /* 0x000004f4 */
#define bisenet_pelee_picinfo__prim_split_2_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_dpitchm1_byte_offset  1272    /* 0x000004f8 */
#define bisenet_pelee_picinfo__prim_split_2_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_dpitchm1_bsize    16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_2_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_vwidth_minus_one_byte_offset  1256    /* 0x000004e8 */
#define bisenet_pelee_picinfo__prim_split_2_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_vwidth_minus_one_bsize    16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_2_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_vheight_minus_one_byte_offset     1258    /* 0x000004ea */
#define bisenet_pelee_picinfo__prim_split_2_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_vheight_minus_one_bsize   16  /* 0x00000010 */
#define bisenet_pelee_picinfo__prim_split_2_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_drotate_bit_offset    10118   /* 0x00002786 */
#define bisenet_pelee_picinfo__prim_split_2_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_hflip_bit_offset  10119   /* 0x00002787 */
#define bisenet_pelee_picinfo__prim_split_2_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_vflip_bit_offset  10120   /* 0x00002788 */
#define bisenet_pelee_picinfo__prim_split_2_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_dflip_bit_offset  10121   /* 0x00002789 */
#define bisenet_pelee_picinfo__prim_split_2_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_pflip_bit_offset  10122   /* 0x0000278a */
#define bisenet_pelee_picinfo__prim_split_2_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_W     160
#define bisenet_pelee_picinfo__prim_split_2_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_H     64
#define bisenet_pelee_picinfo__prim_split_2_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_D     256
#define bisenet_pelee_picinfo__prim_split_2_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_P     1
#define bisenet_pelee_picinfo__prim_split_2_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_data_num_bytes    2
#define bisenet_pelee_picinfo__prim_split_2_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_dpitch_num_bytes  32
#define bisenet_pelee_picinfo__prim_split_2_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_dram_format   3
#define bisenet_pelee_picinfo__prim_split_2_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_vp_interleave_mode    2
#define bisenet_pelee_picinfo__prim_split_2_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_tile_width    4
#define bisenet_pelee_picinfo__prim_split_2_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_tile_height   1
#define bisenet_pelee_picinfo__prim_split_2_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_vmem_buffer_num_bytes     12288
#define bisenet_pelee_picinfo__prim_split_2_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_dram_size_num_bytes   5242880
#define bisenet_pelee_picinfo__prim_split_2_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_sign  0
#define bisenet_pelee_picinfo__prim_split_2_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_datasize  1
#define bisenet_pelee_picinfo__prim_split_2_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_expoffset     12
#define bisenet_pelee_picinfo__prim_split_2_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_expbits   0
#define bisenet_pelee_picinfo__prim_split_2_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_drotate   0
#define bisenet_pelee_picinfo__prim_split_2_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_hflip     0
#define bisenet_pelee_picinfo__prim_split_2_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_vflip     0
#define bisenet_pelee_picinfo__prim_split_2_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_dflip     0
#define bisenet_pelee_picinfo__prim_split_2_Conv_2__Conv2D___BiasAdd___BatchNorm_2__FusedBatchNorm___muli___11___BatchNorm_2__FusedBatchNorm___Relu_2_pflip     0

/* bisenet_pelee_picinfo__prim_split_2 VCB PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_971_ */
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_971__cnngen_demangled_name    "PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_971_"
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_971__byte_offset  1292    /* 0x0000050c */
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_971__vbase_byte_offset    1300    /* 0x00000514 */
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_971__vmem_buffer_num_bytes    24576

/* bisenet_pelee_picinfo__prim_split_2 VCB PeleeNet__stage_3_dense_block_0__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm */
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_0__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_cnngen_demangled_name    "PeleeNet__stage_3_dense_block_0__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm"
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_0__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_byte_offset  1316    /* 0x00000524 */
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_0__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_vbase_byte_offset    1324    /* 0x0000052c */
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_0__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_vmem_buffer_num_bytes    83968

/* bisenet_pelee_picinfo__prim_split_2 VCB PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_976_ */
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_976__cnngen_demangled_name    "PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_976_"
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_976__byte_offset  1392    /* 0x00000570 */
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_976__vbase_byte_offset    1400    /* 0x00000578 */
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_2_transition_layer__transition_layer_avgpool__AvgPool_976__vmem_buffer_num_bytes    36864

/* bisenet_pelee_picinfo__prim_split_2 VCB PeleeNet__stage_3_dense_block_0__conv_left_0__Relu */
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_0__conv_left_0__Relu_cnngen_demangled_name    "PeleeNet__stage_3_dense_block_0__conv_left_0__Relu"
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_0__conv_left_0__Relu_byte_offset  1456    /* 0x000005b0 */
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_0__conv_left_0__Relu_vbase_byte_offset    1464    /* 0x000005b8 */
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_0__conv_left_0__Relu_vmem_buffer_num_bytes    43264

/* bisenet_pelee_picinfo__prim_split_2 VCB PeleeNet__stage_3_dense_block_0__conv_right_0__Relu */
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_0__conv_right_0__Relu_cnngen_demangled_name   "PeleeNet__stage_3_dense_block_0__conv_right_0__Relu"
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_0__conv_right_0__Relu_byte_offset     1528    /* 0x000005f8 */
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_0__conv_right_0__Relu_vbase_byte_offset   1536    /* 0x00000600 */
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_0__conv_right_0__Relu_vmem_buffer_num_bytes   28160

/* bisenet_pelee_picinfo__prim_split_2 VCB PeleeNet__stage_3_dense_block_0__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu */
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_0__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_cnngen_demangled_name  "PeleeNet__stage_3_dense_block_0__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu"
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_0__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_byte_offset    1576    /* 0x00000628 */
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_0__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vbase_byte_offset  1584    /* 0x00000630 */
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_0__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vmem_buffer_num_bytes  14080

/* bisenet_pelee_picinfo__prim_split_2 VCB PeleeNet__stage_3_dense_block_1__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm */
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_1__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_cnngen_demangled_name    "PeleeNet__stage_3_dense_block_1__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm"
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_1__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_byte_offset  1688    /* 0x00000698 */
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_1__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_vbase_byte_offset    1696    /* 0x000006a0 */
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_1__conv_right_0__Conv2D___BatchNorm__FusedBatchNorm___left_0__Conv2D___BatchNorm__FusedBatchNorm_vmem_buffer_num_bytes    124928

/* bisenet_pelee_picinfo__prim_split_2 VCB PeleeNet__stage_3_dense_block_1__conv_left_0__Relu */
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_1__conv_left_0__Relu_cnngen_demangled_name    "PeleeNet__stage_3_dense_block_1__conv_left_0__Relu"
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_1__conv_left_0__Relu_byte_offset  1740    /* 0x000006cc */
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_1__conv_left_0__Relu_vbase_byte_offset    1748    /* 0x000006d4 */
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_1__conv_left_0__Relu_vmem_buffer_num_bytes    28160

/* bisenet_pelee_picinfo__prim_split_2 VCB PeleeNet__stage_3_dense_block_1__conv_right_0__Relu */
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_1__conv_right_0__Relu_cnngen_demangled_name   "PeleeNet__stage_3_dense_block_1__conv_right_0__Relu"
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_1__conv_right_0__Relu_byte_offset     1812    /* 0x00000714 */
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_1__conv_right_0__Relu_vbase_byte_offset   1820    /* 0x0000071c */
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_1__conv_right_0__Relu_vmem_buffer_num_bytes   43264

/* bisenet_pelee_picinfo__prim_split_2 VCB PeleeNet__stage_3_dense_block_1__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu */
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_1__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_cnngen_demangled_name  "PeleeNet__stage_3_dense_block_1__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu"
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_1__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_byte_offset    1860    /* 0x00000744 */
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_1__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vbase_byte_offset  1868    /* 0x0000074c */
#define bisenet_pelee_picinfo__prim_split_2_PeleeNet__stage_3_dense_block_1__conv_right_1__Conv2D___BatchNorm__FusedBatchNorm___Relu_vmem_buffer_num_bytes  21504


#endif /* bisenet_pelee_picinfo__prim_split_2_H */
