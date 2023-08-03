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
#ifndef flexidag_openseg_prim_split_7_H
#define flexidag_openseg_prim_split_7_H

#define vas_gen_header_version                  	7

/* dag name */
#define flexidag_openseg_prim_split_7_DAG_NAME  	"flexidag_openseg_prim_split_7"

/* VDG file info */
#define flexidag_openseg_prim_split_7_vdg_name  	"flexidag_openseg_split_7.vdg"

/* VDG DAG memory info */
#define flexidag_openseg_prim_split_7_byte_offset 	4294967295	/* 0xffffffff */
#define flexidag_openseg_prim_split_7_byte_size 	604	/* 0x0000025c */
#define flexidag_openseg_prim_split_7_preferred_dram_xfer_size 	1

/* flexidag_openseg_prim_split_7 VMEM info */
#define flexidag_openseg_prim_split_7_use_ping_pong_vmem 	0
#define flexidag_openseg_prim_split_7_VMEM_start 	4096	/* 0x00001000 */
#define flexidag_openseg_prim_split_7_VMEM_end  	2058864	/* 0x001f6a70 */
#define flexidag_openseg_prim_split_7_image_start 	1826912	/* 0x001be060 */
#define flexidag_openseg_prim_split_7_image_size 	232556	/* 0x00038c6c */
#define flexidag_openseg_prim_split_7_dagbin_start 	2058864	/* 0x001f6a70 */

/* flexidag_openseg_prim_split_7 DAG info */
#define flexidag_openseg_prim_split_7_estimated_cycles 	29089840

/*******************************************************************************
 * DAG byte offset info for HMB/SMB/VCB and their fields
 * in DAG 'flexidag_openseg_prim_split_7' in source file 'flexidag_openseg_split_7.vas'
 ******************************************************************************/
/* flexidag_openseg_prim_split_7 HMB_input concat */
#define flexidag_openseg_prim_split_7_concat_cnngen_demangled_name 	"concat"
#define flexidag_openseg_prim_split_7_concat_is_main_input_output 	0
#define flexidag_openseg_prim_split_7_concat_is_constant 	0
#define flexidag_openseg_prim_split_7_concat_has_init_data 	0
#define flexidag_openseg_prim_split_7_concat_is_variable_scalar 	0
#define flexidag_openseg_prim_split_7_concat_ddi_byte_size 	0	/* 0x00000000 */
#define flexidag_openseg_prim_split_7_concat_ddi_byte_offset 	4294967295	/* 0xffffffff */
#define flexidag_openseg_prim_split_7_concat_is_bitvector 	0
#define flexidag_openseg_prim_split_7_concat_is_rlz 	0
#define flexidag_openseg_prim_split_7_concat_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_7_concat_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_7_concat_byte_offset 	0	/* 0x00000000 */
#define flexidag_openseg_prim_split_7_concat_vbase_byte_offset 	4	/* 0x00000004 */
#define flexidag_openseg_prim_split_7_concat_dbase_byte_offset 	28	/* 0x0000001c */
#define flexidag_openseg_prim_split_7_concat_dpitchm1_byte_offset 	32	/* 0x00000020 */
#define flexidag_openseg_prim_split_7_concat_dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_7_concat_vwidth_minus_one_byte_offset 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_7_concat_vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_7_concat_vheight_minus_one_byte_offset 	18	/* 0x00000012 */
#define flexidag_openseg_prim_split_7_concat_vheight_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_7_concat_drotate_bit_offset 	198	/* 0x000000c6 */
#define flexidag_openseg_prim_split_7_concat_hflip_bit_offset 	199	/* 0x000000c7 */
#define flexidag_openseg_prim_split_7_concat_vflip_bit_offset 	200	/* 0x000000c8 */
#define flexidag_openseg_prim_split_7_concat_dflip_bit_offset 	201	/* 0x000000c9 */
#define flexidag_openseg_prim_split_7_concat_pflip_bit_offset 	202	/* 0x000000ca */
#define flexidag_openseg_prim_split_7_concat_W  	160
#define flexidag_openseg_prim_split_7_concat_H  	64
#define flexidag_openseg_prim_split_7_concat_D  	768
#define flexidag_openseg_prim_split_7_concat_P  	1
#define flexidag_openseg_prim_split_7_concat_data_num_bytes 	2
#define flexidag_openseg_prim_split_7_concat_denable 	1
#define flexidag_openseg_prim_split_7_concat_dpitch_num_bytes 	32
#define flexidag_openseg_prim_split_7_concat_dram_format 	3
#define flexidag_openseg_prim_split_7_concat_vp_interleave_mode 	2
#define flexidag_openseg_prim_split_7_concat_tile_width 	4
#define flexidag_openseg_prim_split_7_concat_tile_height 	1
#define flexidag_openseg_prim_split_7_concat_vmem_buffer_num_bytes 	110592
#define flexidag_openseg_prim_split_7_concat_dram_size_num_bytes 	15728640
#define flexidag_openseg_prim_split_7_concat_sign 	0
#define flexidag_openseg_prim_split_7_concat_datasize 	1
#define flexidag_openseg_prim_split_7_concat_expoffset 	13
#define flexidag_openseg_prim_split_7_concat_expbits 	0
#define flexidag_openseg_prim_split_7_concat_drotate 	0
#define flexidag_openseg_prim_split_7_concat_hflip 	0
#define flexidag_openseg_prim_split_7_concat_vflip 	0
#define flexidag_openseg_prim_split_7_concat_dflip 	0
#define flexidag_openseg_prim_split_7_concat_pflip 	0

/* flexidag_openseg_prim_split_7 HMB_input Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1 */
#define flexidag_openseg_prim_split_7_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_cnngen_demangled_name 	"Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1"
#define flexidag_openseg_prim_split_7_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_is_main_input_output 	0
#define flexidag_openseg_prim_split_7_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_is_constant 	0
#define flexidag_openseg_prim_split_7_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_has_init_data 	0
#define flexidag_openseg_prim_split_7_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_is_variable_scalar 	0
#define flexidag_openseg_prim_split_7_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_ddi_byte_size 	0	/* 0x00000000 */
#define flexidag_openseg_prim_split_7_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_ddi_byte_offset 	4294967295	/* 0xffffffff */
#define flexidag_openseg_prim_split_7_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_is_bitvector 	0
#define flexidag_openseg_prim_split_7_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_is_rlz 	0
#define flexidag_openseg_prim_split_7_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_7_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_7_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_byte_offset 	40	/* 0x00000028 */
#define flexidag_openseg_prim_split_7_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_vbase_byte_offset 	44	/* 0x0000002c */
#define flexidag_openseg_prim_split_7_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_dbase_byte_offset 	68	/* 0x00000044 */
#define flexidag_openseg_prim_split_7_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_dpitchm1_byte_offset 	72	/* 0x00000048 */
#define flexidag_openseg_prim_split_7_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_7_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_vwidth_minus_one_byte_offset 	56	/* 0x00000038 */
#define flexidag_openseg_prim_split_7_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_7_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_vheight_minus_one_byte_offset 	58	/* 0x0000003a */
#define flexidag_openseg_prim_split_7_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_vheight_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_7_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_drotate_bit_offset 	518	/* 0x00000206 */
#define flexidag_openseg_prim_split_7_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_hflip_bit_offset 	519	/* 0x00000207 */
#define flexidag_openseg_prim_split_7_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_vflip_bit_offset 	520	/* 0x00000208 */
#define flexidag_openseg_prim_split_7_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_dflip_bit_offset 	521	/* 0x00000209 */
#define flexidag_openseg_prim_split_7_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_pflip_bit_offset 	522	/* 0x0000020a */
#define flexidag_openseg_prim_split_7_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_W 	320
#define flexidag_openseg_prim_split_7_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_H 	128
#define flexidag_openseg_prim_split_7_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_D 	128
#define flexidag_openseg_prim_split_7_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_P 	1
#define flexidag_openseg_prim_split_7_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_data_num_bytes 	1
#define flexidag_openseg_prim_split_7_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_denable 	1
#define flexidag_openseg_prim_split_7_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_dpitch_num_bytes 	32
#define flexidag_openseg_prim_split_7_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_dram_format 	3
#define flexidag_openseg_prim_split_7_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_vp_interleave_mode 	2
#define flexidag_openseg_prim_split_7_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_tile_width 	4
#define flexidag_openseg_prim_split_7_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_tile_height 	1
#define flexidag_openseg_prim_split_7_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_vmem_buffer_num_bytes 	655360
#define flexidag_openseg_prim_split_7_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_dram_size_num_bytes 	5242880
#define flexidag_openseg_prim_split_7_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_sign 	0
#define flexidag_openseg_prim_split_7_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_datasize 	0
#define flexidag_openseg_prim_split_7_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_expoffset 	5
#define flexidag_openseg_prim_split_7_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_expbits 	0
#define flexidag_openseg_prim_split_7_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_drotate 	0
#define flexidag_openseg_prim_split_7_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_hflip 	0
#define flexidag_openseg_prim_split_7_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_vflip 	0
#define flexidag_openseg_prim_split_7_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_dflip 	0
#define flexidag_openseg_prim_split_7_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_pflip 	0

/* flexidag_openseg_prim_split_7 SMB_input __pvcn_170004_ */
#define flexidag_openseg_prim_split_7___pvcn_170004__cnngen_demangled_name 	"__pvcn_170004_"
#define flexidag_openseg_prim_split_7___pvcn_170004__is_constant 	1
#define flexidag_openseg_prim_split_7___pvcn_170004__has_init_data 	1
#define flexidag_openseg_prim_split_7___pvcn_170004__is_variable_scalar 	0
#define flexidag_openseg_prim_split_7___pvcn_170004__is_bitvector 	0
#define flexidag_openseg_prim_split_7___pvcn_170004__is_rlz 	0
#define flexidag_openseg_prim_split_7___pvcn_170004__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_7___pvcn_170004__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_7___pvcn_170004__byte_offset 	80	/* 0x00000050 */
#define flexidag_openseg_prim_split_7___pvcn_170004__vbase_byte_offset 	84	/* 0x00000054 */
#define flexidag_openseg_prim_split_7___pvcn_170004__dbase_byte_offset 	108	/* 0x0000006c */
#define flexidag_openseg_prim_split_7___pvcn_170004__dpitchm1_byte_offset 	112	/* 0x00000070 */
#define flexidag_openseg_prim_split_7___pvcn_170004__dpitchm1_bsize 	32	/* 0x00000020 */
#define flexidag_openseg_prim_split_7___pvcn_170004__W 	113672
#define flexidag_openseg_prim_split_7___pvcn_170004__H 	1
#define flexidag_openseg_prim_split_7___pvcn_170004__D 	1
#define flexidag_openseg_prim_split_7___pvcn_170004__P 	1
#define flexidag_openseg_prim_split_7___pvcn_170004__data_num_bytes 	1
#define flexidag_openseg_prim_split_7___pvcn_170004__denable 	1
#define flexidag_openseg_prim_split_7___pvcn_170004__dpitch_num_bytes 	113696
#define flexidag_openseg_prim_split_7___pvcn_170004__dram_format 	0
#define flexidag_openseg_prim_split_7___pvcn_170004__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_7___pvcn_170004__tile_width 	1
#define flexidag_openseg_prim_split_7___pvcn_170004__tile_height 	1
#define flexidag_openseg_prim_split_7___pvcn_170004__vmem_buffer_num_bytes 	113672
#define flexidag_openseg_prim_split_7___pvcn_170004__dram_size_num_bytes 	113696

/* flexidag_openseg_prim_split_7 SMB_input __pvcn_170007_ */
#define flexidag_openseg_prim_split_7___pvcn_170007__cnngen_demangled_name 	"__pvcn_170007_"
#define flexidag_openseg_prim_split_7___pvcn_170007__is_constant 	1
#define flexidag_openseg_prim_split_7___pvcn_170007__has_init_data 	1
#define flexidag_openseg_prim_split_7___pvcn_170007__is_variable_scalar 	0
#define flexidag_openseg_prim_split_7___pvcn_170007__is_bitvector 	0
#define flexidag_openseg_prim_split_7___pvcn_170007__is_rlz 	0
#define flexidag_openseg_prim_split_7___pvcn_170007__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_7___pvcn_170007__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_7___pvcn_170007__byte_offset 	116	/* 0x00000074 */
#define flexidag_openseg_prim_split_7___pvcn_170007__vbase_byte_offset 	120	/* 0x00000078 */
#define flexidag_openseg_prim_split_7___pvcn_170007__dbase_byte_offset 	144	/* 0x00000090 */
#define flexidag_openseg_prim_split_7___pvcn_170007__dpitchm1_byte_offset 	148	/* 0x00000094 */
#define flexidag_openseg_prim_split_7___pvcn_170007__dpitchm1_bsize 	32	/* 0x00000020 */
#define flexidag_openseg_prim_split_7___pvcn_170007__W 	114440
#define flexidag_openseg_prim_split_7___pvcn_170007__H 	1
#define flexidag_openseg_prim_split_7___pvcn_170007__D 	1
#define flexidag_openseg_prim_split_7___pvcn_170007__P 	1
#define flexidag_openseg_prim_split_7___pvcn_170007__data_num_bytes 	1
#define flexidag_openseg_prim_split_7___pvcn_170007__denable 	1
#define flexidag_openseg_prim_split_7___pvcn_170007__dpitch_num_bytes 	114464
#define flexidag_openseg_prim_split_7___pvcn_170007__dram_format 	0
#define flexidag_openseg_prim_split_7___pvcn_170007__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_7___pvcn_170007__tile_width 	1
#define flexidag_openseg_prim_split_7___pvcn_170007__tile_height 	1
#define flexidag_openseg_prim_split_7___pvcn_170007__vmem_buffer_num_bytes 	114440
#define flexidag_openseg_prim_split_7___pvcn_170007__dram_size_num_bytes 	114464

/* flexidag_openseg_prim_split_7 VCB __pvcn_1395_ */
#define flexidag_openseg_prim_split_7___pvcn_1395__cnngen_demangled_name 	"__pvcn_1395_"
#define flexidag_openseg_prim_split_7___pvcn_1395__is_bitvector 	0
#define flexidag_openseg_prim_split_7___pvcn_1395__is_rlz 	0
#define flexidag_openseg_prim_split_7___pvcn_1395__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_7___pvcn_1395__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_7___pvcn_1395__byte_offset 	156	/* 0x0000009c */
#define flexidag_openseg_prim_split_7___pvcn_1395__vbase_byte_offset 	164	/* 0x000000a4 */
#define flexidag_openseg_prim_split_7___pvcn_1395__vmem_buffer_num_bytes 	663552

/* flexidag_openseg_prim_split_7 SMB_input __pvcn_170005_ */
#define flexidag_openseg_prim_split_7___pvcn_170005__cnngen_demangled_name 	"__pvcn_170005_"
#define flexidag_openseg_prim_split_7___pvcn_170005__is_constant 	1
#define flexidag_openseg_prim_split_7___pvcn_170005__has_init_data 	1
#define flexidag_openseg_prim_split_7___pvcn_170005__is_variable_scalar 	0
#define flexidag_openseg_prim_split_7___pvcn_170005__is_bitvector 	0
#define flexidag_openseg_prim_split_7___pvcn_170005__is_rlz 	0
#define flexidag_openseg_prim_split_7___pvcn_170005__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_7___pvcn_170005__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_7___pvcn_170005__byte_offset 	192	/* 0x000000c0 */
#define flexidag_openseg_prim_split_7___pvcn_170005__vbase_byte_offset 	196	/* 0x000000c4 */
#define flexidag_openseg_prim_split_7___pvcn_170005__dbase_byte_offset 	220	/* 0x000000dc */
#define flexidag_openseg_prim_split_7___pvcn_170005__dpitchm1_byte_offset 	224	/* 0x000000e0 */
#define flexidag_openseg_prim_split_7___pvcn_170005__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_7___pvcn_170005__W 	1920
#define flexidag_openseg_prim_split_7___pvcn_170005__H 	1
#define flexidag_openseg_prim_split_7___pvcn_170005__D 	1
#define flexidag_openseg_prim_split_7___pvcn_170005__P 	1
#define flexidag_openseg_prim_split_7___pvcn_170005__data_num_bytes 	1
#define flexidag_openseg_prim_split_7___pvcn_170005__denable 	1
#define flexidag_openseg_prim_split_7___pvcn_170005__dpitch_num_bytes 	1920
#define flexidag_openseg_prim_split_7___pvcn_170005__dram_format 	0
#define flexidag_openseg_prim_split_7___pvcn_170005__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_7___pvcn_170005__tile_width 	1
#define flexidag_openseg_prim_split_7___pvcn_170005__tile_height 	1
#define flexidag_openseg_prim_split_7___pvcn_170005__vmem_buffer_num_bytes 	1920
#define flexidag_openseg_prim_split_7___pvcn_170005__dram_size_num_bytes 	1920

/* flexidag_openseg_prim_split_7 SMB_input __pvcn_170008_ */
#define flexidag_openseg_prim_split_7___pvcn_170008__cnngen_demangled_name 	"__pvcn_170008_"
#define flexidag_openseg_prim_split_7___pvcn_170008__is_constant 	1
#define flexidag_openseg_prim_split_7___pvcn_170008__has_init_data 	1
#define flexidag_openseg_prim_split_7___pvcn_170008__is_variable_scalar 	0
#define flexidag_openseg_prim_split_7___pvcn_170008__is_bitvector 	0
#define flexidag_openseg_prim_split_7___pvcn_170008__is_rlz 	0
#define flexidag_openseg_prim_split_7___pvcn_170008__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_7___pvcn_170008__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_7___pvcn_170008__byte_offset 	232	/* 0x000000e8 */
#define flexidag_openseg_prim_split_7___pvcn_170008__vbase_byte_offset 	236	/* 0x000000ec */
#define flexidag_openseg_prim_split_7___pvcn_170008__dbase_byte_offset 	260	/* 0x00000104 */
#define flexidag_openseg_prim_split_7___pvcn_170008__dpitchm1_byte_offset 	264	/* 0x00000108 */
#define flexidag_openseg_prim_split_7___pvcn_170008__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_7___pvcn_170008__W 	1920
#define flexidag_openseg_prim_split_7___pvcn_170008__H 	1
#define flexidag_openseg_prim_split_7___pvcn_170008__D 	1
#define flexidag_openseg_prim_split_7___pvcn_170008__P 	1
#define flexidag_openseg_prim_split_7___pvcn_170008__data_num_bytes 	1
#define flexidag_openseg_prim_split_7___pvcn_170008__denable 	1
#define flexidag_openseg_prim_split_7___pvcn_170008__dpitch_num_bytes 	1920
#define flexidag_openseg_prim_split_7___pvcn_170008__dram_format 	0
#define flexidag_openseg_prim_split_7___pvcn_170008__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_7___pvcn_170008__tile_width 	1
#define flexidag_openseg_prim_split_7___pvcn_170008__tile_height 	1
#define flexidag_openseg_prim_split_7___pvcn_170008__vmem_buffer_num_bytes 	1920
#define flexidag_openseg_prim_split_7___pvcn_170008__dram_size_num_bytes 	1920

/* flexidag_openseg_prim_split_7 VCB concat_1 */
#define flexidag_openseg_prim_split_7_concat_1_cnngen_demangled_name 	"concat_1"
#define flexidag_openseg_prim_split_7_concat_1_is_bitvector 	0
#define flexidag_openseg_prim_split_7_concat_1_is_rlz 	0
#define flexidag_openseg_prim_split_7_concat_1_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_7_concat_1_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_7_concat_1_byte_offset 	384	/* 0x00000180 */
#define flexidag_openseg_prim_split_7_concat_1_vbase_byte_offset 	392	/* 0x00000188 */
#define flexidag_openseg_prim_split_7_concat_1_vmem_buffer_num_bytes 	229376

/* flexidag_openseg_prim_split_7 HMB_output concat_1_1522_ */
#define flexidag_openseg_prim_split_7_concat_1_1522__cnngen_demangled_name 	"concat_1_1522_"
#define flexidag_openseg_prim_split_7_concat_1_1522__is_main_input_output 	0
#define flexidag_openseg_prim_split_7_concat_1_1522__is_bitvector 	0
#define flexidag_openseg_prim_split_7_concat_1_1522__is_rlz 	0
#define flexidag_openseg_prim_split_7_concat_1_1522__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_7_concat_1_1522__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_7_concat_1_1522__byte_offset 	436	/* 0x000001b4 */
#define flexidag_openseg_prim_split_7_concat_1_1522__vbase_byte_offset 	440	/* 0x000001b8 */
#define flexidag_openseg_prim_split_7_concat_1_1522__dbase_byte_offset 	464	/* 0x000001d0 */
#define flexidag_openseg_prim_split_7_concat_1_1522__dpitchm1_byte_offset 	468	/* 0x000001d4 */
#define flexidag_openseg_prim_split_7_concat_1_1522__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_7_concat_1_1522__vwidth_minus_one_byte_offset 	452	/* 0x000001c4 */
#define flexidag_openseg_prim_split_7_concat_1_1522__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_7_concat_1_1522__vheight_minus_one_byte_offset 	454	/* 0x000001c6 */
#define flexidag_openseg_prim_split_7_concat_1_1522__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_7_concat_1_1522__drotate_bit_offset 	3686	/* 0x00000e66 */
#define flexidag_openseg_prim_split_7_concat_1_1522__hflip_bit_offset 	3687	/* 0x00000e67 */
#define flexidag_openseg_prim_split_7_concat_1_1522__vflip_bit_offset 	3688	/* 0x00000e68 */
#define flexidag_openseg_prim_split_7_concat_1_1522__dflip_bit_offset 	3689	/* 0x00000e69 */
#define flexidag_openseg_prim_split_7_concat_1_1522__pflip_bit_offset 	3690	/* 0x00000e6a */
#define flexidag_openseg_prim_split_7_concat_1_1522__W 	81
#define flexidag_openseg_prim_split_7_concat_1_1522__H 	64
#define flexidag_openseg_prim_split_7_concat_1_1522__D 	1024
#define flexidag_openseg_prim_split_7_concat_1_1522__P 	1
#define flexidag_openseg_prim_split_7_concat_1_1522__data_num_bytes 	2
#define flexidag_openseg_prim_split_7_concat_1_1522__denable 	1
#define flexidag_openseg_prim_split_7_concat_1_1522__dpitch_num_bytes 	32
#define flexidag_openseg_prim_split_7_concat_1_1522__dram_format 	3
#define flexidag_openseg_prim_split_7_concat_1_1522__vp_interleave_mode 	2
#define flexidag_openseg_prim_split_7_concat_1_1522__tile_width 	4
#define flexidag_openseg_prim_split_7_concat_1_1522__tile_height 	1
#define flexidag_openseg_prim_split_7_concat_1_1522__vmem_buffer_num_bytes 	81920
#define flexidag_openseg_prim_split_7_concat_1_1522__dram_size_num_bytes 	11534336
#define flexidag_openseg_prim_split_7_concat_1_1522__sign 	0
#define flexidag_openseg_prim_split_7_concat_1_1522__datasize 	1
#define flexidag_openseg_prim_split_7_concat_1_1522__expoffset 	13
#define flexidag_openseg_prim_split_7_concat_1_1522__expbits 	0
#define flexidag_openseg_prim_split_7_concat_1_1522__drotate 	0
#define flexidag_openseg_prim_split_7_concat_1_1522__hflip 	0
#define flexidag_openseg_prim_split_7_concat_1_1522__vflip 	0
#define flexidag_openseg_prim_split_7_concat_1_1522__dflip 	0
#define flexidag_openseg_prim_split_7_concat_1_1522__pflip 	0

/* flexidag_openseg_prim_split_7 HMB_output concat_1_1527_ */
#define flexidag_openseg_prim_split_7_concat_1_1527__cnngen_demangled_name 	"concat_1_1527_"
#define flexidag_openseg_prim_split_7_concat_1_1527__is_main_input_output 	0
#define flexidag_openseg_prim_split_7_concat_1_1527__is_bitvector 	0
#define flexidag_openseg_prim_split_7_concat_1_1527__is_rlz 	0
#define flexidag_openseg_prim_split_7_concat_1_1527__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_7_concat_1_1527__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_7_concat_1_1527__byte_offset 	488	/* 0x000001e8 */
#define flexidag_openseg_prim_split_7_concat_1_1527__vbase_byte_offset 	492	/* 0x000001ec */
#define flexidag_openseg_prim_split_7_concat_1_1527__dbase_byte_offset 	516	/* 0x00000204 */
#define flexidag_openseg_prim_split_7_concat_1_1527__dpitchm1_byte_offset 	520	/* 0x00000208 */
#define flexidag_openseg_prim_split_7_concat_1_1527__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_7_concat_1_1527__vwidth_minus_one_byte_offset 	504	/* 0x000001f8 */
#define flexidag_openseg_prim_split_7_concat_1_1527__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_7_concat_1_1527__vheight_minus_one_byte_offset 	506	/* 0x000001fa */
#define flexidag_openseg_prim_split_7_concat_1_1527__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_7_concat_1_1527__drotate_bit_offset 	4102	/* 0x00001006 */
#define flexidag_openseg_prim_split_7_concat_1_1527__hflip_bit_offset 	4103	/* 0x00001007 */
#define flexidag_openseg_prim_split_7_concat_1_1527__vflip_bit_offset 	4104	/* 0x00001008 */
#define flexidag_openseg_prim_split_7_concat_1_1527__dflip_bit_offset 	4105	/* 0x00001009 */
#define flexidag_openseg_prim_split_7_concat_1_1527__pflip_bit_offset 	4106	/* 0x0000100a */
#define flexidag_openseg_prim_split_7_concat_1_1527__W 	81
#define flexidag_openseg_prim_split_7_concat_1_1527__H 	64
#define flexidag_openseg_prim_split_7_concat_1_1527__D 	1024
#define flexidag_openseg_prim_split_7_concat_1_1527__P 	1
#define flexidag_openseg_prim_split_7_concat_1_1527__data_num_bytes 	2
#define flexidag_openseg_prim_split_7_concat_1_1527__denable 	1
#define flexidag_openseg_prim_split_7_concat_1_1527__dpitch_num_bytes 	32
#define flexidag_openseg_prim_split_7_concat_1_1527__dram_format 	3
#define flexidag_openseg_prim_split_7_concat_1_1527__vp_interleave_mode 	2
#define flexidag_openseg_prim_split_7_concat_1_1527__tile_width 	4
#define flexidag_openseg_prim_split_7_concat_1_1527__tile_height 	1
#define flexidag_openseg_prim_split_7_concat_1_1527__vmem_buffer_num_bytes 	81920
#define flexidag_openseg_prim_split_7_concat_1_1527__dram_size_num_bytes 	11534336
#define flexidag_openseg_prim_split_7_concat_1_1527__sign 	0
#define flexidag_openseg_prim_split_7_concat_1_1527__datasize 	1
#define flexidag_openseg_prim_split_7_concat_1_1527__expoffset 	13
#define flexidag_openseg_prim_split_7_concat_1_1527__expbits 	0
#define flexidag_openseg_prim_split_7_concat_1_1527__drotate 	0
#define flexidag_openseg_prim_split_7_concat_1_1527__hflip 	0
#define flexidag_openseg_prim_split_7_concat_1_1527__vflip 	0
#define flexidag_openseg_prim_split_7_concat_1_1527__dflip 	0
#define flexidag_openseg_prim_split_7_concat_1_1527__pflip 	0

/* flexidag_openseg_prim_split_7 SMB_output split_7__vas_created_terminate__v0 */
#define flexidag_openseg_prim_split_7_split_7__vas_created_terminate__v0_cnngen_demangled_name 	"split_7__vas_created_terminate__v0"
#define flexidag_openseg_prim_split_7_split_7__vas_created_terminate__v0_is_main_input_output 	0
#define flexidag_openseg_prim_split_7_split_7__vas_created_terminate__v0_is_bitvector 	1
#define flexidag_openseg_prim_split_7_split_7__vas_created_terminate__v0_is_rlz 	0
#define flexidag_openseg_prim_split_7_split_7__vas_created_terminate__v0_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_7_split_7__vas_created_terminate__v0_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_7_split_7__vas_created_terminate__v0_byte_offset 	528	/* 0x00000210 */
#define flexidag_openseg_prim_split_7_split_7__vas_created_terminate__v0_vbase_byte_offset 	532	/* 0x00000214 */
#define flexidag_openseg_prim_split_7_split_7__vas_created_terminate__v0_W 	256
#define flexidag_openseg_prim_split_7_split_7__vas_created_terminate__v0_H 	3
#define flexidag_openseg_prim_split_7_split_7__vas_created_terminate__v0_D 	1
#define flexidag_openseg_prim_split_7_split_7__vas_created_terminate__v0_P 	1
#define flexidag_openseg_prim_split_7_split_7__vas_created_terminate__v0_data_num_bytes 	1
#define flexidag_openseg_prim_split_7_split_7__vas_created_terminate__v0_denable 	0
#define flexidag_openseg_prim_split_7_split_7__vas_created_terminate__v0_dram_format 	0
#define flexidag_openseg_prim_split_7_split_7__vas_created_terminate__v0_vp_interleave_mode 	1
#define flexidag_openseg_prim_split_7_split_7__vas_created_terminate__v0_tile_width 	1
#define flexidag_openseg_prim_split_7_split_7__vas_created_terminate__v0_tile_height 	1
#define flexidag_openseg_prim_split_7_split_7__vas_created_terminate__v0_vmem_buffer_num_bytes 	96


#endif /* flexidag_openseg_prim_split_7_H */
