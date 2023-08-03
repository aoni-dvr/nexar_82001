////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Ambarella International LP
////////////////////////////////////////////////////////////////////////////////
// This file and its contents ("Software") are protected by intellectual
// property rights including, without limitation, U.S. and/or foreign
// copyrights. This Software is also the confidential and proprietary
// information of Ambarella International LP and its licensors. You may not use,
// reproduce, disclose, distribute, modify, or otherwise prepare derivative
// works of this Software or any portion thereof except pursuant to a signed
// license agreement or nondisclosure agreement with Ambarella International LP
// or its authorized affiliates. In the absence of such an agreement, you agree
// to promptly notify and return this Software to Ambarella International LP.
//
// THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
// INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
// MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
////////////////////////////////////////////////////////////////////////////////
#ifndef flexidag_openseg_prim_split_4_H
#define flexidag_openseg_prim_split_4_H

#define vas_gen_header_version                  	7

/* dag name */
#define flexidag_openseg_prim_split_4_DAG_NAME 	"flexidag_openseg_prim_split_4"

/* VDG file info */
#define flexidag_openseg_prim_split_4_vdg_name 	"flexidag_openseg_split_4.vdg"

/* VDG DAG memory info */
#define flexidag_openseg_prim_split_4_byte_offset 	4294967295	/* 0xffffffff */
#define flexidag_openseg_prim_split_4_byte_size 	320	/* 0x00000140 */
#define flexidag_openseg_prim_split_4_preferred_dram_xfer_size 	1

/* flexidag_openseg_prim_split_4 VMEM info */
#define flexidag_openseg_prim_split_4_use_ping_pong_vmem 	0
#define flexidag_openseg_prim_split_4_VMEM_start 	4096	/* 0x00001000 */
#define flexidag_openseg_prim_split_4_VMEM_end 	487340	/* 0x00076fac */
#define flexidag_openseg_prim_split_4_image_start 	371808	/* 0x0005ac60 */
#define flexidag_openseg_prim_split_4_image_size 	115852	/* 0x0001c48c */
#define flexidag_openseg_prim_split_4_dagbin_start 	487340	/* 0x00076fac */

/* flexidag_openseg_prim_split_4 DAG info */
#define flexidag_openseg_prim_split_4_estimated_cycles 	490812
#define flexidag_openseg_prim_split_4_ddi_byte_size 	0	/* 0x00000000 */

/*******************************************************************************
 * DAG byte offset info for HMB/SMB/VCB and their fields
 * in DAG 'flexidag_openseg_prim_split_4' in source file 'pre_split3_flexidag_openseg_split_1.vas'
 ******************************************************************************/
/* flexidag_openseg_prim_split_4 HMB_input __pvcn_80000002_ */
#define flexidag_openseg_prim_split_4___pvcn_80000002__cnngen_demangled_name 	"__pvcn_80000002_"
#define flexidag_openseg_prim_split_4___pvcn_80000002__is_main_input_output 	0
#define flexidag_openseg_prim_split_4___pvcn_80000002__is_constant 	0
#define flexidag_openseg_prim_split_4___pvcn_80000002__has_init_data 	0
#define flexidag_openseg_prim_split_4___pvcn_80000002__is_variable_scalar 	0
#define flexidag_openseg_prim_split_4___pvcn_80000002__ddi_byte_size 	0	/* 0x00000000 */
#define flexidag_openseg_prim_split_4___pvcn_80000002__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define flexidag_openseg_prim_split_4___pvcn_80000002__is_bitvector 	0
#define flexidag_openseg_prim_split_4___pvcn_80000002__is_rlz 	0
#define flexidag_openseg_prim_split_4___pvcn_80000002__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_4___pvcn_80000002__byte_offset 	0	/* 0x00000000 */
#define flexidag_openseg_prim_split_4___pvcn_80000002__vbase_byte_offset 	4	/* 0x00000004 */
#define flexidag_openseg_prim_split_4___pvcn_80000002__dbase_byte_offset 	28	/* 0x0000001c */
#define flexidag_openseg_prim_split_4___pvcn_80000002__dpitchm1_byte_offset 	32	/* 0x00000020 */
#define flexidag_openseg_prim_split_4___pvcn_80000002__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_4___pvcn_80000002__vwidth_minus_one_byte_offset 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_4___pvcn_80000002__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_4___pvcn_80000002__vheight_minus_one_byte_offset 	18	/* 0x00000012 */
#define flexidag_openseg_prim_split_4___pvcn_80000002__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_4___pvcn_80000002__drotate_bit_offset 	198	/* 0x000000c6 */
#define flexidag_openseg_prim_split_4___pvcn_80000002__hflip_bit_offset 	199	/* 0x000000c7 */
#define flexidag_openseg_prim_split_4___pvcn_80000002__vflip_bit_offset 	200	/* 0x000000c8 */
#define flexidag_openseg_prim_split_4___pvcn_80000002__dflip_bit_offset 	201	/* 0x000000c9 */
#define flexidag_openseg_prim_split_4___pvcn_80000002__pflip_bit_offset 	202	/* 0x000000ca */
#define flexidag_openseg_prim_split_4___pvcn_80000002__W 	160
#define flexidag_openseg_prim_split_4___pvcn_80000002__H 	64
#define flexidag_openseg_prim_split_4___pvcn_80000002__D 	128
#define flexidag_openseg_prim_split_4___pvcn_80000002__P 	1
#define flexidag_openseg_prim_split_4___pvcn_80000002__data_num_bytes 	2
#define flexidag_openseg_prim_split_4___pvcn_80000002__denable 	1
#define flexidag_openseg_prim_split_4___pvcn_80000002__dpitch_num_bytes 	64
#define flexidag_openseg_prim_split_4___pvcn_80000002__dram_format 	3
#define flexidag_openseg_prim_split_4___pvcn_80000002__vp_interleave_mode 	2
#define flexidag_openseg_prim_split_4___pvcn_80000002__tile_width 	4
#define flexidag_openseg_prim_split_4___pvcn_80000002__tile_height 	1
#define flexidag_openseg_prim_split_4___pvcn_80000002__vmem_buffer_num_bytes 	32768
#define flexidag_openseg_prim_split_4___pvcn_80000002__dram_size_num_bytes 	2621440
#define flexidag_openseg_prim_split_4___pvcn_80000002__sign 	0
#define flexidag_openseg_prim_split_4___pvcn_80000002__datasize 	1
#define flexidag_openseg_prim_split_4___pvcn_80000002__expoffset 	12
#define flexidag_openseg_prim_split_4___pvcn_80000002__expbits 	0
#define flexidag_openseg_prim_split_4___pvcn_80000002__drotate 	0
#define flexidag_openseg_prim_split_4___pvcn_80000002__hflip 	0
#define flexidag_openseg_prim_split_4___pvcn_80000002__vflip 	0
#define flexidag_openseg_prim_split_4___pvcn_80000002__dflip 	0
#define flexidag_openseg_prim_split_4___pvcn_80000002__pflip 	0
#define flexidag_openseg_prim_split_4___pvcn_80000002__trailing_extra_elements 	0

/* flexidag_openseg_prim_split_4 HMB_input Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1 */
#define flexidag_openseg_prim_split_4_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_cnngen_demangled_name 	"Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1"
#define flexidag_openseg_prim_split_4_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_is_main_input_output 	0
#define flexidag_openseg_prim_split_4_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_is_constant 	0
#define flexidag_openseg_prim_split_4_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_has_init_data 	0
#define flexidag_openseg_prim_split_4_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_is_variable_scalar 	0
#define flexidag_openseg_prim_split_4_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_ddi_byte_size 	0	/* 0x00000000 */
#define flexidag_openseg_prim_split_4_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_ddi_byte_offset 	4294967295	/* 0xffffffff */
#define flexidag_openseg_prim_split_4_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_is_bitvector 	0
#define flexidag_openseg_prim_split_4_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_is_rlz 	0
#define flexidag_openseg_prim_split_4_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_4_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_byte_offset 	40	/* 0x00000028 */
#define flexidag_openseg_prim_split_4_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_vbase_byte_offset 	44	/* 0x0000002c */
#define flexidag_openseg_prim_split_4_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_dbase_byte_offset 	68	/* 0x00000044 */
#define flexidag_openseg_prim_split_4_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_dpitchm1_byte_offset 	72	/* 0x00000048 */
#define flexidag_openseg_prim_split_4_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_4_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_vwidth_minus_one_byte_offset 	56	/* 0x00000038 */
#define flexidag_openseg_prim_split_4_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_4_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_vheight_minus_one_byte_offset 	58	/* 0x0000003a */
#define flexidag_openseg_prim_split_4_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_vheight_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_4_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_drotate_bit_offset 	518	/* 0x00000206 */
#define flexidag_openseg_prim_split_4_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_hflip_bit_offset 	519	/* 0x00000207 */
#define flexidag_openseg_prim_split_4_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_vflip_bit_offset 	520	/* 0x00000208 */
#define flexidag_openseg_prim_split_4_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_dflip_bit_offset 	521	/* 0x00000209 */
#define flexidag_openseg_prim_split_4_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_pflip_bit_offset 	522	/* 0x0000020a */
#define flexidag_openseg_prim_split_4_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_W 	320
#define flexidag_openseg_prim_split_4_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_H 	128
#define flexidag_openseg_prim_split_4_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_D 	128
#define flexidag_openseg_prim_split_4_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_P 	1
#define flexidag_openseg_prim_split_4_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_data_num_bytes 	1
#define flexidag_openseg_prim_split_4_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_denable 	1
#define flexidag_openseg_prim_split_4_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_dpitch_num_bytes 	64
#define flexidag_openseg_prim_split_4_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_dram_format 	3
#define flexidag_openseg_prim_split_4_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_vp_interleave_mode 	2
#define flexidag_openseg_prim_split_4_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_tile_width 	4
#define flexidag_openseg_prim_split_4_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_tile_height 	1
#define flexidag_openseg_prim_split_4_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_vmem_buffer_num_bytes 	171008
#define flexidag_openseg_prim_split_4_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_dram_size_num_bytes 	5242880
#define flexidag_openseg_prim_split_4_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_sign 	0
#define flexidag_openseg_prim_split_4_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_datasize 	0
#define flexidag_openseg_prim_split_4_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_expoffset 	5
#define flexidag_openseg_prim_split_4_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_expbits 	0
#define flexidag_openseg_prim_split_4_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_drotate 	0
#define flexidag_openseg_prim_split_4_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_hflip 	0
#define flexidag_openseg_prim_split_4_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_vflip 	0
#define flexidag_openseg_prim_split_4_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_dflip 	0
#define flexidag_openseg_prim_split_4_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_pflip 	0
#define flexidag_openseg_prim_split_4_Conv_1__Conv2D___BiasAdd___BatchNorm_1__FusedBatchNorm___muli___10___BatchNorm_1__FusedBatchNorm___Relu_1_trailing_extra_elements 	0

/* flexidag_openseg_prim_split_4 SMB_input __pvcn_80000004_ */
#define flexidag_openseg_prim_split_4___pvcn_80000004__cnngen_demangled_name 	"__pvcn_80000004_"
#define flexidag_openseg_prim_split_4___pvcn_80000004__is_constant 	1
#define flexidag_openseg_prim_split_4___pvcn_80000004__has_init_data 	1
#define flexidag_openseg_prim_split_4___pvcn_80000004__is_variable_scalar 	0
#define flexidag_openseg_prim_split_4___pvcn_80000004__is_bitvector 	0
#define flexidag_openseg_prim_split_4___pvcn_80000004__is_rlz 	0
#define flexidag_openseg_prim_split_4___pvcn_80000004__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_4___pvcn_80000004__byte_offset 	76	/* 0x0000004c */
#define flexidag_openseg_prim_split_4___pvcn_80000004__vbase_byte_offset 	80	/* 0x00000050 */
#define flexidag_openseg_prim_split_4___pvcn_80000004__dbase_byte_offset 	104	/* 0x00000068 */
#define flexidag_openseg_prim_split_4___pvcn_80000004__dpitchm1_byte_offset 	108	/* 0x0000006c */
#define flexidag_openseg_prim_split_4___pvcn_80000004__dpitchm1_bsize 	32	/* 0x00000020 */
#define flexidag_openseg_prim_split_4___pvcn_80000004__W 	114380
#define flexidag_openseg_prim_split_4___pvcn_80000004__H 	1
#define flexidag_openseg_prim_split_4___pvcn_80000004__D 	1
#define flexidag_openseg_prim_split_4___pvcn_80000004__P 	1
#define flexidag_openseg_prim_split_4___pvcn_80000004__data_num_bytes 	1
#define flexidag_openseg_prim_split_4___pvcn_80000004__denable 	1
#define flexidag_openseg_prim_split_4___pvcn_80000004__dpitch_num_bytes 	114432
#define flexidag_openseg_prim_split_4___pvcn_80000004__dram_format 	0
#define flexidag_openseg_prim_split_4___pvcn_80000004__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_4___pvcn_80000004__tile_width 	1
#define flexidag_openseg_prim_split_4___pvcn_80000004__tile_height 	1
#define flexidag_openseg_prim_split_4___pvcn_80000004__vmem_buffer_num_bytes 	114380
#define flexidag_openseg_prim_split_4___pvcn_80000004__dram_size_num_bytes 	114432

/* flexidag_openseg_prim_split_4 SMB_input __pvcn_80000007_ */
#define flexidag_openseg_prim_split_4___pvcn_80000007__cnngen_demangled_name 	"__pvcn_80000007_"
#define flexidag_openseg_prim_split_4___pvcn_80000007__is_constant 	1
#define flexidag_openseg_prim_split_4___pvcn_80000007__has_init_data 	1
#define flexidag_openseg_prim_split_4___pvcn_80000007__is_variable_scalar 	0
#define flexidag_openseg_prim_split_4___pvcn_80000007__is_bitvector 	0
#define flexidag_openseg_prim_split_4___pvcn_80000007__is_rlz 	0
#define flexidag_openseg_prim_split_4___pvcn_80000007__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_4___pvcn_80000007__byte_offset 	112	/* 0x00000070 */
#define flexidag_openseg_prim_split_4___pvcn_80000007__vbase_byte_offset 	116	/* 0x00000074 */
#define flexidag_openseg_prim_split_4___pvcn_80000007__dbase_byte_offset 	140	/* 0x0000008c */
#define flexidag_openseg_prim_split_4___pvcn_80000007__dpitchm1_byte_offset 	144	/* 0x00000090 */
#define flexidag_openseg_prim_split_4___pvcn_80000007__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_4___pvcn_80000007__W 	1152
#define flexidag_openseg_prim_split_4___pvcn_80000007__H 	1
#define flexidag_openseg_prim_split_4___pvcn_80000007__D 	1
#define flexidag_openseg_prim_split_4___pvcn_80000007__P 	1
#define flexidag_openseg_prim_split_4___pvcn_80000007__data_num_bytes 	1
#define flexidag_openseg_prim_split_4___pvcn_80000007__denable 	1
#define flexidag_openseg_prim_split_4___pvcn_80000007__dpitch_num_bytes 	1152
#define flexidag_openseg_prim_split_4___pvcn_80000007__dram_format 	0
#define flexidag_openseg_prim_split_4___pvcn_80000007__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_4___pvcn_80000007__tile_width 	1
#define flexidag_openseg_prim_split_4___pvcn_80000007__tile_height 	1
#define flexidag_openseg_prim_split_4___pvcn_80000007__vmem_buffer_num_bytes 	1152
#define flexidag_openseg_prim_split_4___pvcn_80000007__dram_size_num_bytes 	1152

/* flexidag_openseg_prim_split_4 HMB_output __pvcn_1152_ */
#define flexidag_openseg_prim_split_4___pvcn_1152__cnngen_demangled_name 	"__pvcn_1152_"
#define flexidag_openseg_prim_split_4___pvcn_1152__is_main_input_output 	0
#define flexidag_openseg_prim_split_4___pvcn_1152__is_bitvector 	0
#define flexidag_openseg_prim_split_4___pvcn_1152__is_rlz 	0
#define flexidag_openseg_prim_split_4___pvcn_1152__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_4___pvcn_1152__byte_offset 	152	/* 0x00000098 */
#define flexidag_openseg_prim_split_4___pvcn_1152__vbase_byte_offset 	156	/* 0x0000009c */
#define flexidag_openseg_prim_split_4___pvcn_1152__dbase_byte_offset 	180	/* 0x000000b4 */
#define flexidag_openseg_prim_split_4___pvcn_1152__dpitchm1_byte_offset 	184	/* 0x000000b8 */
#define flexidag_openseg_prim_split_4___pvcn_1152__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_4___pvcn_1152__vwidth_minus_one_byte_offset 	168	/* 0x000000a8 */
#define flexidag_openseg_prim_split_4___pvcn_1152__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_4___pvcn_1152__vheight_minus_one_byte_offset 	170	/* 0x000000aa */
#define flexidag_openseg_prim_split_4___pvcn_1152__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_4___pvcn_1152__drotate_bit_offset 	1414	/* 0x00000586 */
#define flexidag_openseg_prim_split_4___pvcn_1152__hflip_bit_offset 	1415	/* 0x00000587 */
#define flexidag_openseg_prim_split_4___pvcn_1152__vflip_bit_offset 	1416	/* 0x00000588 */
#define flexidag_openseg_prim_split_4___pvcn_1152__dflip_bit_offset 	1417	/* 0x00000589 */
#define flexidag_openseg_prim_split_4___pvcn_1152__pflip_bit_offset 	1418	/* 0x0000058a */
#define flexidag_openseg_prim_split_4___pvcn_1152__W 	160
#define flexidag_openseg_prim_split_4___pvcn_1152__H 	64
#define flexidag_openseg_prim_split_4___pvcn_1152__D 	256
#define flexidag_openseg_prim_split_4___pvcn_1152__P 	1
#define flexidag_openseg_prim_split_4___pvcn_1152__data_num_bytes 	2
#define flexidag_openseg_prim_split_4___pvcn_1152__denable 	1
#define flexidag_openseg_prim_split_4___pvcn_1152__dpitch_num_bytes 	64
#define flexidag_openseg_prim_split_4___pvcn_1152__dram_format 	3
#define flexidag_openseg_prim_split_4___pvcn_1152__vp_interleave_mode 	2
#define flexidag_openseg_prim_split_4___pvcn_1152__tile_width 	4
#define flexidag_openseg_prim_split_4___pvcn_1152__tile_height 	1
#define flexidag_openseg_prim_split_4___pvcn_1152__vmem_buffer_num_bytes 	163840
#define flexidag_openseg_prim_split_4___pvcn_1152__dram_size_num_bytes 	5242880
#define flexidag_openseg_prim_split_4___pvcn_1152__sign 	0
#define flexidag_openseg_prim_split_4___pvcn_1152__datasize 	1
#define flexidag_openseg_prim_split_4___pvcn_1152__expoffset 	12
#define flexidag_openseg_prim_split_4___pvcn_1152__expbits 	0
#define flexidag_openseg_prim_split_4___pvcn_1152__drotate 	0
#define flexidag_openseg_prim_split_4___pvcn_1152__hflip 	0
#define flexidag_openseg_prim_split_4___pvcn_1152__vflip 	0
#define flexidag_openseg_prim_split_4___pvcn_1152__dflip 	0
#define flexidag_openseg_prim_split_4___pvcn_1152__pflip 	0
#define flexidag_openseg_prim_split_4___pvcn_1152__trailing_extra_elements 	0

/* flexidag_openseg_prim_split_4 SMB_output __vas_created_terminate__v0 */
#define flexidag_openseg_prim_split_4___vas_created_terminate__v0_cnngen_demangled_name 	"__vas_created_terminate__v0"
#define flexidag_openseg_prim_split_4___vas_created_terminate__v0_is_main_input_output 	0
#define flexidag_openseg_prim_split_4___vas_created_terminate__v0_is_bitvector 	1
#define flexidag_openseg_prim_split_4___vas_created_terminate__v0_is_rlz 	0
#define flexidag_openseg_prim_split_4___vas_created_terminate__v0_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_4___vas_created_terminate__v0_byte_offset 	244	/* 0x000000f4 */
#define flexidag_openseg_prim_split_4___vas_created_terminate__v0_vbase_byte_offset 	248	/* 0x000000f8 */
#define flexidag_openseg_prim_split_4___vas_created_terminate__v0_W 	256
#define flexidag_openseg_prim_split_4___vas_created_terminate__v0_H 	3
#define flexidag_openseg_prim_split_4___vas_created_terminate__v0_D 	1
#define flexidag_openseg_prim_split_4___vas_created_terminate__v0_P 	1
#define flexidag_openseg_prim_split_4___vas_created_terminate__v0_data_num_bytes 	1
#define flexidag_openseg_prim_split_4___vas_created_terminate__v0_denable 	0
#define flexidag_openseg_prim_split_4___vas_created_terminate__v0_dram_format 	0
#define flexidag_openseg_prim_split_4___vas_created_terminate__v0_vp_interleave_mode 	1
#define flexidag_openseg_prim_split_4___vas_created_terminate__v0_tile_width 	1
#define flexidag_openseg_prim_split_4___vas_created_terminate__v0_tile_height 	1
#define flexidag_openseg_prim_split_4___vas_created_terminate__v0_vmem_buffer_num_bytes 	96


#endif /* flexidag_openseg_prim_split_4_H */
