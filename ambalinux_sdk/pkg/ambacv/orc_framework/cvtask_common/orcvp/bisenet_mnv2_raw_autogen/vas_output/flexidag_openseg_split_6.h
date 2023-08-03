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
#ifndef flexidag_openseg_prim_split_6_H
#define flexidag_openseg_prim_split_6_H

#define vas_gen_header_version                  	7

/* dag name */
#define flexidag_openseg_prim_split_6_DAG_NAME  	"flexidag_openseg_prim_split_6"

/* VDG file info */
#define flexidag_openseg_prim_split_6_vdg_name  	"flexidag_openseg_split_6.vdg"

/* VDG DAG memory info */
#define flexidag_openseg_prim_split_6_byte_offset 	356	/* 0x00000164 */
#define flexidag_openseg_prim_split_6_byte_size 	324	/* 0x00000144 */
#define flexidag_openseg_prim_split_6_preferred_dram_xfer_size 	1

/* flexidag_openseg_prim_split_6 VMEM info */
#define flexidag_openseg_prim_split_6_use_ping_pong_vmem 	0
#define flexidag_openseg_prim_split_6_VMEM_start 	4096	/* 0x00001000 */
#define flexidag_openseg_prim_split_6_VMEM_end  	1722028	/* 0x001a46ac */
#define flexidag_openseg_prim_split_6_image_start 	1691344	/* 0x0019ced0 */
#define flexidag_openseg_prim_split_6_image_size 	31008	/* 0x00007920 */
#define flexidag_openseg_prim_split_6_dagbin_start 	1722028	/* 0x001a46ac */

/* flexidag_openseg_prim_split_6 DAG info */
#define flexidag_openseg_prim_split_6_estimated_cycles 	2097631

/*******************************************************************************
 * DAG byte offset info for HMB/SMB/VCB and their fields
 * in DAG 'flexidag_openseg_prim_split_6' in source file 'flexidag_openseg_split_6.vas'
 ******************************************************************************/
/* flexidag_openseg_prim_split_6 HMB_input concat_1 */
#define flexidag_openseg_prim_split_6_concat_1_cnngen_demangled_name 	"concat_1"
#define flexidag_openseg_prim_split_6_concat_1_is_main_input_output 	0
#define flexidag_openseg_prim_split_6_concat_1_is_constant 	0
#define flexidag_openseg_prim_split_6_concat_1_ddi_byte_size 	0	/* 0x00000000 */
#define flexidag_openseg_prim_split_6_concat_1_ddi_byte_offset 	4294967295	/* 0xffffffff */
#define flexidag_openseg_prim_split_6_concat_1_is_bitvector 	0
#define flexidag_openseg_prim_split_6_concat_1_is_rlz 	0
#define flexidag_openseg_prim_split_6_concat_1_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_6_concat_1_byte_offset 	0	/* 0x00000000 */
#define flexidag_openseg_prim_split_6_concat_1_vbase_byte_offset 	4	/* 0x00000004 */
#define flexidag_openseg_prim_split_6_concat_1_dbase_byte_offset 	28	/* 0x0000001c */
#define flexidag_openseg_prim_split_6_concat_1_dpitchm1_byte_offset 	32	/* 0x00000020 */
#define flexidag_openseg_prim_split_6_concat_1_dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_6_concat_1_vwidth_minus_one_byte_offset 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_6_concat_1_vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_6_concat_1_vheight_minus_one_byte_offset 	18	/* 0x00000012 */
#define flexidag_openseg_prim_split_6_concat_1_vheight_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_6_concat_1_drotate_bit_offset 	198	/* 0x000000c6 */
#define flexidag_openseg_prim_split_6_concat_1_hflip_bit_offset 	199	/* 0x000000c7 */
#define flexidag_openseg_prim_split_6_concat_1_vflip_bit_offset 	200	/* 0x000000c8 */
#define flexidag_openseg_prim_split_6_concat_1_dflip_bit_offset 	201	/* 0x000000c9 */
#define flexidag_openseg_prim_split_6_concat_1_pflip_bit_offset 	202	/* 0x000000ca */
#define flexidag_openseg_prim_split_6_concat_1_W 	160
#define flexidag_openseg_prim_split_6_concat_1_H 	64
#define flexidag_openseg_prim_split_6_concat_1_D 	1024
#define flexidag_openseg_prim_split_6_concat_1_P 	1
#define flexidag_openseg_prim_split_6_concat_1_data_num_bytes 	2
#define flexidag_openseg_prim_split_6_concat_1_dpitch_num_bytes 	32
#define flexidag_openseg_prim_split_6_concat_1_dram_format 	3
#define flexidag_openseg_prim_split_6_concat_1_vp_interleave_mode 	2
#define flexidag_openseg_prim_split_6_concat_1_tile_width 	4
#define flexidag_openseg_prim_split_6_concat_1_tile_height 	1
#define flexidag_openseg_prim_split_6_concat_1_vmem_buffer_num_bytes 	212992
#define flexidag_openseg_prim_split_6_concat_1_dram_size_num_bytes 	20971520
#define flexidag_openseg_prim_split_6_concat_1_sign 	0
#define flexidag_openseg_prim_split_6_concat_1_datasize 	1
#define flexidag_openseg_prim_split_6_concat_1_expoffset 	13
#define flexidag_openseg_prim_split_6_concat_1_expbits 	0
#define flexidag_openseg_prim_split_6_concat_1_drotate 	0
#define flexidag_openseg_prim_split_6_concat_1_hflip 	0
#define flexidag_openseg_prim_split_6_concat_1_vflip 	0
#define flexidag_openseg_prim_split_6_concat_1_dflip 	0
#define flexidag_openseg_prim_split_6_concat_1_pflip 	0

/* flexidag_openseg_prim_split_6 SMB_input __pvcn_1237_ */
#define flexidag_openseg_prim_split_6___pvcn_1237__cnngen_demangled_name 	"__pvcn_1237_"
#define flexidag_openseg_prim_split_6___pvcn_1237__is_constant 	1
#define flexidag_openseg_prim_split_6___pvcn_1237__is_bitvector 	0
#define flexidag_openseg_prim_split_6___pvcn_1237__is_rlz 	0
#define flexidag_openseg_prim_split_6___pvcn_1237__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_6___pvcn_1237__byte_offset 	40	/* 0x00000028 */
#define flexidag_openseg_prim_split_6___pvcn_1237__vbase_byte_offset 	44	/* 0x0000002c */
#define flexidag_openseg_prim_split_6___pvcn_1237__dbase_byte_offset 	68	/* 0x00000044 */
#define flexidag_openseg_prim_split_6___pvcn_1237__dpitchm1_byte_offset 	72	/* 0x00000048 */
#define flexidag_openseg_prim_split_6___pvcn_1237__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_6___pvcn_1237__W 	30652
#define flexidag_openseg_prim_split_6___pvcn_1237__H 	1
#define flexidag_openseg_prim_split_6___pvcn_1237__D 	1
#define flexidag_openseg_prim_split_6___pvcn_1237__P 	1
#define flexidag_openseg_prim_split_6___pvcn_1237__data_num_bytes 	1
#define flexidag_openseg_prim_split_6___pvcn_1237__dpitch_num_bytes 	30656
#define flexidag_openseg_prim_split_6___pvcn_1237__dram_format 	0
#define flexidag_openseg_prim_split_6___pvcn_1237__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_6___pvcn_1237__tile_width 	1
#define flexidag_openseg_prim_split_6___pvcn_1237__tile_height 	1
#define flexidag_openseg_prim_split_6___pvcn_1237__vmem_buffer_num_bytes 	30652

/* flexidag_openseg_prim_split_6 SMB_input __pvcn_1238_ */
#define flexidag_openseg_prim_split_6___pvcn_1238__cnngen_demangled_name 	"__pvcn_1238_"
#define flexidag_openseg_prim_split_6___pvcn_1238__is_constant 	1
#define flexidag_openseg_prim_split_6___pvcn_1238__is_bitvector 	0
#define flexidag_openseg_prim_split_6___pvcn_1238__is_rlz 	0
#define flexidag_openseg_prim_split_6___pvcn_1238__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_6___pvcn_1238__byte_offset 	80	/* 0x00000050 */
#define flexidag_openseg_prim_split_6___pvcn_1238__vbase_byte_offset 	84	/* 0x00000054 */
#define flexidag_openseg_prim_split_6___pvcn_1238__dbase_byte_offset 	108	/* 0x0000006c */
#define flexidag_openseg_prim_split_6___pvcn_1238__dpitchm1_byte_offset 	112	/* 0x00000070 */
#define flexidag_openseg_prim_split_6___pvcn_1238__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_6___pvcn_1238__W 	32
#define flexidag_openseg_prim_split_6___pvcn_1238__H 	1
#define flexidag_openseg_prim_split_6___pvcn_1238__D 	1
#define flexidag_openseg_prim_split_6___pvcn_1238__P 	1
#define flexidag_openseg_prim_split_6___pvcn_1238__data_num_bytes 	1
#define flexidag_openseg_prim_split_6___pvcn_1238__dpitch_num_bytes 	32
#define flexidag_openseg_prim_split_6___pvcn_1238__dram_format 	0
#define flexidag_openseg_prim_split_6___pvcn_1238__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_6___pvcn_1238__tile_width 	1
#define flexidag_openseg_prim_split_6___pvcn_1238__tile_height 	1
#define flexidag_openseg_prim_split_6___pvcn_1238__vmem_buffer_num_bytes 	32

/* flexidag_openseg_prim_split_6 VCB concat_1_1227_ */
#define flexidag_openseg_prim_split_6_concat_1_1227__cnngen_demangled_name 	"concat_1_1227_"
#define flexidag_openseg_prim_split_6_concat_1_1227__is_bitvector 	0
#define flexidag_openseg_prim_split_6_concat_1_1227__is_rlz 	0
#define flexidag_openseg_prim_split_6_concat_1_1227__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_6_concat_1_1227__byte_offset 	136	/* 0x00000088 */
#define flexidag_openseg_prim_split_6_concat_1_1227__vbase_byte_offset 	144	/* 0x00000090 */
#define flexidag_openseg_prim_split_6_concat_1_1227__vmem_buffer_num_bytes 	1310720

/* flexidag_openseg_prim_split_6 HMB_output concat_1_1232_ */
#define flexidag_openseg_prim_split_6_concat_1_1232__cnngen_demangled_name 	"concat_1_1232_"
#define flexidag_openseg_prim_split_6_concat_1_1232__is_main_input_output 	0
#define flexidag_openseg_prim_split_6_concat_1_1232__is_bitvector 	0
#define flexidag_openseg_prim_split_6_concat_1_1232__is_rlz 	0
#define flexidag_openseg_prim_split_6_concat_1_1232__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_6_concat_1_1232__byte_offset 	180	/* 0x000000b4 */
#define flexidag_openseg_prim_split_6_concat_1_1232__vbase_byte_offset 	184	/* 0x000000b8 */
#define flexidag_openseg_prim_split_6_concat_1_1232__dbase_byte_offset 	208	/* 0x000000d0 */
#define flexidag_openseg_prim_split_6_concat_1_1232__dpitchm1_byte_offset 	212	/* 0x000000d4 */
#define flexidag_openseg_prim_split_6_concat_1_1232__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_6_concat_1_1232__vwidth_minus_one_byte_offset 	196	/* 0x000000c4 */
#define flexidag_openseg_prim_split_6_concat_1_1232__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_6_concat_1_1232__vheight_minus_one_byte_offset 	198	/* 0x000000c6 */
#define flexidag_openseg_prim_split_6_concat_1_1232__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_6_concat_1_1232__drotate_bit_offset 	1638	/* 0x00000666 */
#define flexidag_openseg_prim_split_6_concat_1_1232__hflip_bit_offset 	1639	/* 0x00000667 */
#define flexidag_openseg_prim_split_6_concat_1_1232__vflip_bit_offset 	1640	/* 0x00000668 */
#define flexidag_openseg_prim_split_6_concat_1_1232__dflip_bit_offset 	1641	/* 0x00000669 */
#define flexidag_openseg_prim_split_6_concat_1_1232__pflip_bit_offset 	1642	/* 0x0000066a */
#define flexidag_openseg_prim_split_6_concat_1_1232__W 	81
#define flexidag_openseg_prim_split_6_concat_1_1232__H 	64
#define flexidag_openseg_prim_split_6_concat_1_1232__D 	1024
#define flexidag_openseg_prim_split_6_concat_1_1232__P 	1
#define flexidag_openseg_prim_split_6_concat_1_1232__data_num_bytes 	2
#define flexidag_openseg_prim_split_6_concat_1_1232__dpitch_num_bytes 	32
#define flexidag_openseg_prim_split_6_concat_1_1232__dram_format 	3
#define flexidag_openseg_prim_split_6_concat_1_1232__vp_interleave_mode 	2
#define flexidag_openseg_prim_split_6_concat_1_1232__tile_width 	4
#define flexidag_openseg_prim_split_6_concat_1_1232__tile_height 	1
#define flexidag_openseg_prim_split_6_concat_1_1232__vmem_buffer_num_bytes 	147456
#define flexidag_openseg_prim_split_6_concat_1_1232__dram_size_num_bytes 	11534336
#define flexidag_openseg_prim_split_6_concat_1_1232__sign 	0
#define flexidag_openseg_prim_split_6_concat_1_1232__datasize 	1
#define flexidag_openseg_prim_split_6_concat_1_1232__expoffset 	13
#define flexidag_openseg_prim_split_6_concat_1_1232__expbits 	0
#define flexidag_openseg_prim_split_6_concat_1_1232__drotate 	0
#define flexidag_openseg_prim_split_6_concat_1_1232__hflip 	0
#define flexidag_openseg_prim_split_6_concat_1_1232__vflip 	0
#define flexidag_openseg_prim_split_6_concat_1_1232__dflip 	0
#define flexidag_openseg_prim_split_6_concat_1_1232__pflip 	0

/* flexidag_openseg_prim_split_6 HMB_output Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1231_ */
#define flexidag_openseg_prim_split_6_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1231__cnngen_demangled_name 	"Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1231_"
#define flexidag_openseg_prim_split_6_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1231__is_main_input_output 	0
#define flexidag_openseg_prim_split_6_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1231__is_bitvector 	0
#define flexidag_openseg_prim_split_6_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1231__is_rlz 	0
#define flexidag_openseg_prim_split_6_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1231__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_6_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1231__byte_offset 	236	/* 0x000000ec */
#define flexidag_openseg_prim_split_6_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1231__vbase_byte_offset 	240	/* 0x000000f0 */
#define flexidag_openseg_prim_split_6_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1231__dbase_byte_offset 	264	/* 0x00000108 */
#define flexidag_openseg_prim_split_6_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1231__dpitchm1_byte_offset 	268	/* 0x0000010c */
#define flexidag_openseg_prim_split_6_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1231__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_6_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1231__vwidth_minus_one_byte_offset 	252	/* 0x000000fc */
#define flexidag_openseg_prim_split_6_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1231__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_6_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1231__vheight_minus_one_byte_offset 	254	/* 0x000000fe */
#define flexidag_openseg_prim_split_6_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1231__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_6_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1231__drotate_bit_offset 	2086	/* 0x00000826 */
#define flexidag_openseg_prim_split_6_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1231__hflip_bit_offset 	2087	/* 0x00000827 */
#define flexidag_openseg_prim_split_6_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1231__vflip_bit_offset 	2088	/* 0x00000828 */
#define flexidag_openseg_prim_split_6_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1231__dflip_bit_offset 	2089	/* 0x00000829 */
#define flexidag_openseg_prim_split_6_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1231__pflip_bit_offset 	2090	/* 0x0000082a */
#define flexidag_openseg_prim_split_6_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1231__W 	80
#define flexidag_openseg_prim_split_6_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1231__H 	64
#define flexidag_openseg_prim_split_6_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1231__D 	5
#define flexidag_openseg_prim_split_6_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1231__P 	1
#define flexidag_openseg_prim_split_6_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1231__data_num_bytes 	1
#define flexidag_openseg_prim_split_6_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1231__dpitch_num_bytes 	32
#define flexidag_openseg_prim_split_6_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1231__dram_format 	3
#define flexidag_openseg_prim_split_6_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1231__vp_interleave_mode 	2
#define flexidag_openseg_prim_split_6_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1231__tile_width 	4
#define flexidag_openseg_prim_split_6_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1231__tile_height 	1
#define flexidag_openseg_prim_split_6_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1231__vmem_buffer_num_bytes 	16080
#define flexidag_openseg_prim_split_6_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1231__dram_size_num_bytes 	25600
#define flexidag_openseg_prim_split_6_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1231__sign 	0
#define flexidag_openseg_prim_split_6_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1231__datasize 	0
#define flexidag_openseg_prim_split_6_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1231__expoffset 	6
#define flexidag_openseg_prim_split_6_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1231__expbits 	0
#define flexidag_openseg_prim_split_6_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1231__drotate 	0
#define flexidag_openseg_prim_split_6_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1231__hflip 	0
#define flexidag_openseg_prim_split_6_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1231__vflip 	0
#define flexidag_openseg_prim_split_6_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1231__dflip 	0
#define flexidag_openseg_prim_split_6_Conv_5__Conv2D___BiasAdd___BatchNorm_5__FusedBatchNorm___muli___156___BatchNorm_5__FusedBatchNorm___Relu_3_1231__pflip 	0


#endif /* flexidag_openseg_prim_split_6_H */
