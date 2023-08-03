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
#ifndef flexidag_openseg_prim_split_8_H
#define flexidag_openseg_prim_split_8_H

#define vas_gen_header_version                  	7

/* dag name */
#define flexidag_openseg_prim_split_8_DAG_NAME  	"flexidag_openseg_prim_split_8"

/* VDG file info */
#define flexidag_openseg_prim_split_8_vdg_name  	"flexidag_openseg_split_8.vdg"

/* VDG DAG memory info */
#define flexidag_openseg_prim_split_8_byte_offset 	4294967295	/* 0xffffffff */
#define flexidag_openseg_prim_split_8_byte_size 	248	/* 0x000000f8 */
#define flexidag_openseg_prim_split_8_preferred_dram_xfer_size 	1

/* flexidag_openseg_prim_split_8 VMEM info */
#define flexidag_openseg_prim_split_8_use_ping_pong_vmem 	0
#define flexidag_openseg_prim_split_8_VMEM_start 	4096	/* 0x00001000 */
#define flexidag_openseg_prim_split_8_VMEM_end  	1181480	/* 0x00120728 */
#define flexidag_openseg_prim_split_8_image_start 	1150768	/* 0x00118f30 */
#define flexidag_openseg_prim_split_8_image_size 	30960	/* 0x000078f0 */
#define flexidag_openseg_prim_split_8_dagbin_start 	1181480	/* 0x00120728 */

/* flexidag_openseg_prim_split_8 DAG info */
#define flexidag_openseg_prim_split_8_estimated_cycles 	655887

/*******************************************************************************
 * DAG byte offset info for HMB/SMB/VCB and their fields
 * in DAG 'flexidag_openseg_prim_split_8' in source file 'flexidag_openseg_split_8.vas'
 ******************************************************************************/
/* flexidag_openseg_prim_split_8 HMB_input concat_1_1522_ */
#define flexidag_openseg_prim_split_8_concat_1_1522__cnngen_demangled_name 	"concat_1_1522_"
#define flexidag_openseg_prim_split_8_concat_1_1522__is_main_input_output 	0
#define flexidag_openseg_prim_split_8_concat_1_1522__is_constant 	0
#define flexidag_openseg_prim_split_8_concat_1_1522__has_init_data 	0
#define flexidag_openseg_prim_split_8_concat_1_1522__is_variable_scalar 	0
#define flexidag_openseg_prim_split_8_concat_1_1522__ddi_byte_size 	0	/* 0x00000000 */
#define flexidag_openseg_prim_split_8_concat_1_1522__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define flexidag_openseg_prim_split_8_concat_1_1522__is_bitvector 	0
#define flexidag_openseg_prim_split_8_concat_1_1522__is_rlz 	0
#define flexidag_openseg_prim_split_8_concat_1_1522__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_8_concat_1_1522__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_8_concat_1_1522__byte_offset 	0	/* 0x00000000 */
#define flexidag_openseg_prim_split_8_concat_1_1522__vbase_byte_offset 	4	/* 0x00000004 */
#define flexidag_openseg_prim_split_8_concat_1_1522__dbase_byte_offset 	28	/* 0x0000001c */
#define flexidag_openseg_prim_split_8_concat_1_1522__dpitchm1_byte_offset 	32	/* 0x00000020 */
#define flexidag_openseg_prim_split_8_concat_1_1522__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_8_concat_1_1522__vwidth_minus_one_byte_offset 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_8_concat_1_1522__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_8_concat_1_1522__vheight_minus_one_byte_offset 	18	/* 0x00000012 */
#define flexidag_openseg_prim_split_8_concat_1_1522__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_8_concat_1_1522__drotate_bit_offset 	198	/* 0x000000c6 */
#define flexidag_openseg_prim_split_8_concat_1_1522__hflip_bit_offset 	199	/* 0x000000c7 */
#define flexidag_openseg_prim_split_8_concat_1_1522__vflip_bit_offset 	200	/* 0x000000c8 */
#define flexidag_openseg_prim_split_8_concat_1_1522__dflip_bit_offset 	201	/* 0x000000c9 */
#define flexidag_openseg_prim_split_8_concat_1_1522__pflip_bit_offset 	202	/* 0x000000ca */
#define flexidag_openseg_prim_split_8_concat_1_1522__W 	81
#define flexidag_openseg_prim_split_8_concat_1_1522__H 	64
#define flexidag_openseg_prim_split_8_concat_1_1522__D 	1024
#define flexidag_openseg_prim_split_8_concat_1_1522__P 	1
#define flexidag_openseg_prim_split_8_concat_1_1522__data_num_bytes 	2
#define flexidag_openseg_prim_split_8_concat_1_1522__denable 	1
#define flexidag_openseg_prim_split_8_concat_1_1522__dpitch_num_bytes 	32
#define flexidag_openseg_prim_split_8_concat_1_1522__dram_format 	3
#define flexidag_openseg_prim_split_8_concat_1_1522__vp_interleave_mode 	2
#define flexidag_openseg_prim_split_8_concat_1_1522__tile_width 	4
#define flexidag_openseg_prim_split_8_concat_1_1522__tile_height 	1
#define flexidag_openseg_prim_split_8_concat_1_1522__vmem_buffer_num_bytes 	1130496
#define flexidag_openseg_prim_split_8_concat_1_1522__dram_size_num_bytes 	11534336
#define flexidag_openseg_prim_split_8_concat_1_1522__sign 	0
#define flexidag_openseg_prim_split_8_concat_1_1522__datasize 	1
#define flexidag_openseg_prim_split_8_concat_1_1522__expoffset 	13
#define flexidag_openseg_prim_split_8_concat_1_1522__expbits 	0
#define flexidag_openseg_prim_split_8_concat_1_1522__drotate 	0
#define flexidag_openseg_prim_split_8_concat_1_1522__hflip 	0
#define flexidag_openseg_prim_split_8_concat_1_1522__vflip 	0
#define flexidag_openseg_prim_split_8_concat_1_1522__dflip 	0
#define flexidag_openseg_prim_split_8_concat_1_1522__pflip 	0

/* flexidag_openseg_prim_split_8 SMB_input __pvcn_1532_ */
#define flexidag_openseg_prim_split_8___pvcn_1532__cnngen_demangled_name 	"__pvcn_1532_"
#define flexidag_openseg_prim_split_8___pvcn_1532__is_constant 	1
#define flexidag_openseg_prim_split_8___pvcn_1532__has_init_data 	1
#define flexidag_openseg_prim_split_8___pvcn_1532__is_variable_scalar 	0
#define flexidag_openseg_prim_split_8___pvcn_1532__is_bitvector 	0
#define flexidag_openseg_prim_split_8___pvcn_1532__is_rlz 	0
#define flexidag_openseg_prim_split_8___pvcn_1532__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_8___pvcn_1532__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_8___pvcn_1532__byte_offset 	36	/* 0x00000024 */
#define flexidag_openseg_prim_split_8___pvcn_1532__vbase_byte_offset 	40	/* 0x00000028 */
#define flexidag_openseg_prim_split_8___pvcn_1532__dbase_byte_offset 	64	/* 0x00000040 */
#define flexidag_openseg_prim_split_8___pvcn_1532__dpitchm1_byte_offset 	68	/* 0x00000044 */
#define flexidag_openseg_prim_split_8___pvcn_1532__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_8___pvcn_1532__W 	30652
#define flexidag_openseg_prim_split_8___pvcn_1532__H 	1
#define flexidag_openseg_prim_split_8___pvcn_1532__D 	1
#define flexidag_openseg_prim_split_8___pvcn_1532__P 	1
#define flexidag_openseg_prim_split_8___pvcn_1532__data_num_bytes 	1
#define flexidag_openseg_prim_split_8___pvcn_1532__denable 	1
#define flexidag_openseg_prim_split_8___pvcn_1532__dpitch_num_bytes 	30656
#define flexidag_openseg_prim_split_8___pvcn_1532__dram_format 	0
#define flexidag_openseg_prim_split_8___pvcn_1532__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_8___pvcn_1532__tile_width 	1
#define flexidag_openseg_prim_split_8___pvcn_1532__tile_height 	1
#define flexidag_openseg_prim_split_8___pvcn_1532__vmem_buffer_num_bytes 	30652
#define flexidag_openseg_prim_split_8___pvcn_1532__dram_size_num_bytes 	30656

/* flexidag_openseg_prim_split_8 SMB_input __pvcn_1533_ */
#define flexidag_openseg_prim_split_8___pvcn_1533__cnngen_demangled_name 	"__pvcn_1533_"
#define flexidag_openseg_prim_split_8___pvcn_1533__is_constant 	1
#define flexidag_openseg_prim_split_8___pvcn_1533__has_init_data 	1
#define flexidag_openseg_prim_split_8___pvcn_1533__is_variable_scalar 	0
#define flexidag_openseg_prim_split_8___pvcn_1533__is_bitvector 	0
#define flexidag_openseg_prim_split_8___pvcn_1533__is_rlz 	0
#define flexidag_openseg_prim_split_8___pvcn_1533__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_8___pvcn_1533__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_8___pvcn_1533__byte_offset 	72	/* 0x00000048 */
#define flexidag_openseg_prim_split_8___pvcn_1533__vbase_byte_offset 	76	/* 0x0000004c */
#define flexidag_openseg_prim_split_8___pvcn_1533__dbase_byte_offset 	100	/* 0x00000064 */
#define flexidag_openseg_prim_split_8___pvcn_1533__dpitchm1_byte_offset 	104	/* 0x00000068 */
#define flexidag_openseg_prim_split_8___pvcn_1533__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_8___pvcn_1533__W 	60
#define flexidag_openseg_prim_split_8___pvcn_1533__H 	1
#define flexidag_openseg_prim_split_8___pvcn_1533__D 	1
#define flexidag_openseg_prim_split_8___pvcn_1533__P 	1
#define flexidag_openseg_prim_split_8___pvcn_1533__data_num_bytes 	1
#define flexidag_openseg_prim_split_8___pvcn_1533__denable 	1
#define flexidag_openseg_prim_split_8___pvcn_1533__dpitch_num_bytes 	64
#define flexidag_openseg_prim_split_8___pvcn_1533__dram_format 	0
#define flexidag_openseg_prim_split_8___pvcn_1533__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_8___pvcn_1533__tile_width 	1
#define flexidag_openseg_prim_split_8___pvcn_1533__tile_height 	1
#define flexidag_openseg_prim_split_8___pvcn_1533__vmem_buffer_num_bytes 	60
#define flexidag_openseg_prim_split_8___pvcn_1533__dram_size_num_bytes 	64

/* flexidag_openseg_prim_split_8 HMB_output Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1526_ */
#define flexidag_openseg_prim_split_8_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1526__cnngen_demangled_name 	"Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1526_"
#define flexidag_openseg_prim_split_8_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1526__is_main_input_output 	0
#define flexidag_openseg_prim_split_8_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1526__is_bitvector 	0
#define flexidag_openseg_prim_split_8_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1526__is_rlz 	0
#define flexidag_openseg_prim_split_8_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1526__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_8_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1526__is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_8_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1526__byte_offset 	132	/* 0x00000084 */
#define flexidag_openseg_prim_split_8_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1526__vbase_byte_offset 	136	/* 0x00000088 */
#define flexidag_openseg_prim_split_8_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1526__dbase_byte_offset 	160	/* 0x000000a0 */
#define flexidag_openseg_prim_split_8_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1526__dpitchm1_byte_offset 	164	/* 0x000000a4 */
#define flexidag_openseg_prim_split_8_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1526__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_8_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1526__vwidth_minus_one_byte_offset 	148	/* 0x00000094 */
#define flexidag_openseg_prim_split_8_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1526__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_8_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1526__vheight_minus_one_byte_offset 	150	/* 0x00000096 */
#define flexidag_openseg_prim_split_8_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1526__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_8_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1526__drotate_bit_offset 	1254	/* 0x000004e6 */
#define flexidag_openseg_prim_split_8_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1526__hflip_bit_offset 	1255	/* 0x000004e7 */
#define flexidag_openseg_prim_split_8_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1526__vflip_bit_offset 	1256	/* 0x000004e8 */
#define flexidag_openseg_prim_split_8_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1526__dflip_bit_offset 	1257	/* 0x000004e9 */
#define flexidag_openseg_prim_split_8_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1526__pflip_bit_offset 	1258	/* 0x000004ea */
#define flexidag_openseg_prim_split_8_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1526__W 	80
#define flexidag_openseg_prim_split_8_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1526__H 	64
#define flexidag_openseg_prim_split_8_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1526__D 	5
#define flexidag_openseg_prim_split_8_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1526__P 	1
#define flexidag_openseg_prim_split_8_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1526__data_num_bytes 	1
#define flexidag_openseg_prim_split_8_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1526__denable 	1
#define flexidag_openseg_prim_split_8_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1526__dpitch_num_bytes 	32
#define flexidag_openseg_prim_split_8_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1526__dram_format 	3
#define flexidag_openseg_prim_split_8_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1526__vp_interleave_mode 	2
#define flexidag_openseg_prim_split_8_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1526__tile_width 	4
#define flexidag_openseg_prim_split_8_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1526__tile_height 	1
#define flexidag_openseg_prim_split_8_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1526__vmem_buffer_num_bytes 	16080
#define flexidag_openseg_prim_split_8_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1526__dram_size_num_bytes 	25600
#define flexidag_openseg_prim_split_8_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1526__sign 	0
#define flexidag_openseg_prim_split_8_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1526__datasize 	0
#define flexidag_openseg_prim_split_8_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1526__expoffset 	6
#define flexidag_openseg_prim_split_8_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1526__expbits 	0
#define flexidag_openseg_prim_split_8_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1526__drotate 	0
#define flexidag_openseg_prim_split_8_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1526__hflip 	0
#define flexidag_openseg_prim_split_8_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1526__vflip 	0
#define flexidag_openseg_prim_split_8_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1526__dflip 	0
#define flexidag_openseg_prim_split_8_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1526__pflip 	0

/* flexidag_openseg_prim_split_8 SMB_output split_8__vas_created_terminate__v0 */
#define flexidag_openseg_prim_split_8_split_8__vas_created_terminate__v0_cnngen_demangled_name 	"split_8__vas_created_terminate__v0"
#define flexidag_openseg_prim_split_8_split_8__vas_created_terminate__v0_is_main_input_output 	0
#define flexidag_openseg_prim_split_8_split_8__vas_created_terminate__v0_is_bitvector 	1
#define flexidag_openseg_prim_split_8_split_8__vas_created_terminate__v0_is_rlz 	0
#define flexidag_openseg_prim_split_8_split_8__vas_created_terminate__v0_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_8_split_8__vas_created_terminate__v0_is_twelve_bit_packed 	0
#define flexidag_openseg_prim_split_8_split_8__vas_created_terminate__v0_byte_offset 	172	/* 0x000000ac */
#define flexidag_openseg_prim_split_8_split_8__vas_created_terminate__v0_vbase_byte_offset 	176	/* 0x000000b0 */
#define flexidag_openseg_prim_split_8_split_8__vas_created_terminate__v0_W 	256
#define flexidag_openseg_prim_split_8_split_8__vas_created_terminate__v0_H 	3
#define flexidag_openseg_prim_split_8_split_8__vas_created_terminate__v0_D 	1
#define flexidag_openseg_prim_split_8_split_8__vas_created_terminate__v0_P 	1
#define flexidag_openseg_prim_split_8_split_8__vas_created_terminate__v0_data_num_bytes 	1
#define flexidag_openseg_prim_split_8_split_8__vas_created_terminate__v0_denable 	0
#define flexidag_openseg_prim_split_8_split_8__vas_created_terminate__v0_dram_format 	0
#define flexidag_openseg_prim_split_8_split_8__vas_created_terminate__v0_vp_interleave_mode 	1
#define flexidag_openseg_prim_split_8_split_8__vas_created_terminate__v0_tile_width 	1
#define flexidag_openseg_prim_split_8_split_8__vas_created_terminate__v0_tile_height 	1
#define flexidag_openseg_prim_split_8_split_8__vas_created_terminate__v0_vmem_buffer_num_bytes 	96


#endif /* flexidag_openseg_prim_split_8_H */
