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
#ifndef flexidag_openseg_prim_split_1_H
#define flexidag_openseg_prim_split_1_H

#define vas_gen_header_version                  	7

/* dag name */
#define flexidag_openseg_prim_split_1_DAG_NAME 	"flexidag_openseg_prim_split_1"

/* VDG file info */
#define flexidag_openseg_prim_split_1_vdg_name 	"flexidag_openseg_split_1.vdg"

/* VDG DAG memory info */
#define flexidag_openseg_prim_split_1_byte_offset 	4294967295	/* 0xffffffff */
#define flexidag_openseg_prim_split_1_byte_size 	288	/* 0x00000120 */
#define flexidag_openseg_prim_split_1_preferred_dram_xfer_size 	1

/* flexidag_openseg_prim_split_1 VMEM info */
#define flexidag_openseg_prim_split_1_use_ping_pong_vmem 	0
#define flexidag_openseg_prim_split_1_VMEM_start 	4096	/* 0x00001000 */
#define flexidag_openseg_prim_split_1_VMEM_end 	98400	/* 0x00018060 */
#define flexidag_openseg_prim_split_1_image_start 	98400	/* 0x00018060 */
#define flexidag_openseg_prim_split_1_image_size 	288	/* 0x00000120 */
#define flexidag_openseg_prim_split_1_dagbin_start 	98400	/* 0x00018060 */

/* flexidag_openseg_prim_split_1 DAG info */
#define flexidag_openseg_prim_split_1_estimated_cycles 	1966128
#define flexidag_openseg_prim_split_1_ddi_byte_size 	0	/* 0x00000000 */

/*******************************************************************************
 * DAG byte offset info for HMB/SMB/VCB and their fields
 * in DAG 'flexidag_openseg_prim_split_1' in source file 'pre_split1_flexidag_openseg_split_0.vas'
 ******************************************************************************/
/* flexidag_openseg_prim_split_1 HMB_input Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm___muli___440___1 */
#define flexidag_openseg_prim_split_1_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm___muli___440___1_cnngen_demangled_name 	"Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm___muli___440___1"
#define flexidag_openseg_prim_split_1_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm___muli___440___1_is_main_input_output 	0
#define flexidag_openseg_prim_split_1_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm___muli___440___1_is_constant 	0
#define flexidag_openseg_prim_split_1_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm___muli___440___1_has_init_data 	0
#define flexidag_openseg_prim_split_1_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm___muli___440___1_is_variable_scalar 	0
#define flexidag_openseg_prim_split_1_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm___muli___440___1_ddi_byte_size 	0	/* 0x00000000 */
#define flexidag_openseg_prim_split_1_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm___muli___440___1_ddi_byte_offset 	4294967295	/* 0xffffffff */
#define flexidag_openseg_prim_split_1_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm___muli___440___1_is_bitvector 	0
#define flexidag_openseg_prim_split_1_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm___muli___440___1_is_rlz 	0
#define flexidag_openseg_prim_split_1_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm___muli___440___1_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm___muli___440___1_byte_offset 	0	/* 0x00000000 */
#define flexidag_openseg_prim_split_1_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm___muli___440___1_vbase_byte_offset 	4	/* 0x00000004 */
#define flexidag_openseg_prim_split_1_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm___muli___440___1_dbase_byte_offset 	28	/* 0x0000001c */
#define flexidag_openseg_prim_split_1_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm___muli___440___1_dpitchm1_byte_offset 	32	/* 0x00000020 */
#define flexidag_openseg_prim_split_1_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm___muli___440___1_dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm___muli___440___1_vwidth_minus_one_byte_offset 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm___muli___440___1_vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm___muli___440___1_vheight_minus_one_byte_offset 	18	/* 0x00000012 */
#define flexidag_openseg_prim_split_1_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm___muli___440___1_vheight_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm___muli___440___1_drotate_bit_offset 	198	/* 0x000000c6 */
#define flexidag_openseg_prim_split_1_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm___muli___440___1_hflip_bit_offset 	199	/* 0x000000c7 */
#define flexidag_openseg_prim_split_1_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm___muli___440___1_vflip_bit_offset 	200	/* 0x000000c8 */
#define flexidag_openseg_prim_split_1_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm___muli___440___1_dflip_bit_offset 	201	/* 0x000000c9 */
#define flexidag_openseg_prim_split_1_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm___muli___440___1_pflip_bit_offset 	202	/* 0x000000ca */
#define flexidag_openseg_prim_split_1_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm___muli___440___1_W 	640
#define flexidag_openseg_prim_split_1_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm___muli___440___1_H 	256
#define flexidag_openseg_prim_split_1_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm___muli___440___1_D 	96
#define flexidag_openseg_prim_split_1_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm___muli___440___1_P 	1
#define flexidag_openseg_prim_split_1_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm___muli___440___1_data_num_bytes 	1
#define flexidag_openseg_prim_split_1_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm___muli___440___1_denable 	1
#define flexidag_openseg_prim_split_1_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm___muli___440___1_dpitch_num_bytes 	64
#define flexidag_openseg_prim_split_1_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm___muli___440___1_dram_format 	3
#define flexidag_openseg_prim_split_1_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm___muli___440___1_vp_interleave_mode 	2
#define flexidag_openseg_prim_split_1_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm___muli___440___1_tile_width 	4
#define flexidag_openseg_prim_split_1_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm___muli___440___1_tile_height 	1
#define flexidag_openseg_prim_split_1_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm___muli___440___1_vmem_buffer_num_bytes 	49152
#define flexidag_openseg_prim_split_1_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm___muli___440___1_dram_size_num_bytes 	15728640
#define flexidag_openseg_prim_split_1_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm___muli___440___1_sign 	1
#define flexidag_openseg_prim_split_1_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm___muli___440___1_datasize 	0
#define flexidag_openseg_prim_split_1_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm___muli___440___1_expoffset 	5
#define flexidag_openseg_prim_split_1_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm___muli___440___1_expbits 	0
#define flexidag_openseg_prim_split_1_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm___muli___440___1_drotate 	0
#define flexidag_openseg_prim_split_1_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm___muli___440___1_hflip 	0
#define flexidag_openseg_prim_split_1_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm___muli___440___1_vflip 	0
#define flexidag_openseg_prim_split_1_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm___muli___440___1_dflip 	0
#define flexidag_openseg_prim_split_1_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm___muli___440___1_pflip 	0
#define flexidag_openseg_prim_split_1_Conv__Conv2D___BiasAdd___BatchNorm__FusedBatchNorm___muli___7___BatchNorm__FusedBatchNorm___PeleeNet__stem_block__stem_block_conv0__Conv2D___BatchNorm__FusedBatchNorm___muli___440___1_trailing_extra_elements 	0

/* flexidag_openseg_prim_split_1 HMB_output PeleeNet__stem_block__stem_block_conv0__Relu______muli___443 */
#define flexidag_openseg_prim_split_1_PeleeNet__stem_block__stem_block_conv0__Relu______muli___443_cnngen_demangled_name 	"PeleeNet__stem_block__stem_block_conv0__Relu______muli___443"
#define flexidag_openseg_prim_split_1_PeleeNet__stem_block__stem_block_conv0__Relu______muli___443_is_main_input_output 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stem_block__stem_block_conv0__Relu______muli___443_is_bitvector 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stem_block__stem_block_conv0__Relu______muli___443_is_rlz 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stem_block__stem_block_conv0__Relu______muli___443_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stem_block__stem_block_conv0__Relu______muli___443_byte_offset 	88	/* 0x00000058 */
#define flexidag_openseg_prim_split_1_PeleeNet__stem_block__stem_block_conv0__Relu______muli___443_vbase_byte_offset 	92	/* 0x0000005c */
#define flexidag_openseg_prim_split_1_PeleeNet__stem_block__stem_block_conv0__Relu______muli___443_dbase_byte_offset 	116	/* 0x00000074 */
#define flexidag_openseg_prim_split_1_PeleeNet__stem_block__stem_block_conv0__Relu______muli___443_dpitchm1_byte_offset 	120	/* 0x00000078 */
#define flexidag_openseg_prim_split_1_PeleeNet__stem_block__stem_block_conv0__Relu______muli___443_dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1_PeleeNet__stem_block__stem_block_conv0__Relu______muli___443_vwidth_minus_one_byte_offset 	104	/* 0x00000068 */
#define flexidag_openseg_prim_split_1_PeleeNet__stem_block__stem_block_conv0__Relu______muli___443_vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1_PeleeNet__stem_block__stem_block_conv0__Relu______muli___443_vheight_minus_one_byte_offset 	106	/* 0x0000006a */
#define flexidag_openseg_prim_split_1_PeleeNet__stem_block__stem_block_conv0__Relu______muli___443_vheight_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1_PeleeNet__stem_block__stem_block_conv0__Relu______muli___443_drotate_bit_offset 	902	/* 0x00000386 */
#define flexidag_openseg_prim_split_1_PeleeNet__stem_block__stem_block_conv0__Relu______muli___443_hflip_bit_offset 	903	/* 0x00000387 */
#define flexidag_openseg_prim_split_1_PeleeNet__stem_block__stem_block_conv0__Relu______muli___443_vflip_bit_offset 	904	/* 0x00000388 */
#define flexidag_openseg_prim_split_1_PeleeNet__stem_block__stem_block_conv0__Relu______muli___443_dflip_bit_offset 	905	/* 0x00000389 */
#define flexidag_openseg_prim_split_1_PeleeNet__stem_block__stem_block_conv0__Relu______muli___443_pflip_bit_offset 	906	/* 0x0000038a */
#define flexidag_openseg_prim_split_1_PeleeNet__stem_block__stem_block_conv0__Relu______muli___443_W 	640
#define flexidag_openseg_prim_split_1_PeleeNet__stem_block__stem_block_conv0__Relu______muli___443_H 	256
#define flexidag_openseg_prim_split_1_PeleeNet__stem_block__stem_block_conv0__Relu______muli___443_D 	32
#define flexidag_openseg_prim_split_1_PeleeNet__stem_block__stem_block_conv0__Relu______muli___443_P 	1
#define flexidag_openseg_prim_split_1_PeleeNet__stem_block__stem_block_conv0__Relu______muli___443_data_num_bytes 	1
#define flexidag_openseg_prim_split_1_PeleeNet__stem_block__stem_block_conv0__Relu______muli___443_denable 	1
#define flexidag_openseg_prim_split_1_PeleeNet__stem_block__stem_block_conv0__Relu______muli___443_dpitch_num_bytes 	64
#define flexidag_openseg_prim_split_1_PeleeNet__stem_block__stem_block_conv0__Relu______muli___443_dram_format 	3
#define flexidag_openseg_prim_split_1_PeleeNet__stem_block__stem_block_conv0__Relu______muli___443_vp_interleave_mode 	2
#define flexidag_openseg_prim_split_1_PeleeNet__stem_block__stem_block_conv0__Relu______muli___443_tile_width 	4
#define flexidag_openseg_prim_split_1_PeleeNet__stem_block__stem_block_conv0__Relu______muli___443_tile_height 	1
#define flexidag_openseg_prim_split_1_PeleeNet__stem_block__stem_block_conv0__Relu______muli___443_vmem_buffer_num_bytes 	12288
#define flexidag_openseg_prim_split_1_PeleeNet__stem_block__stem_block_conv0__Relu______muli___443_dram_size_num_bytes 	5242880
#define flexidag_openseg_prim_split_1_PeleeNet__stem_block__stem_block_conv0__Relu______muli___443_sign 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stem_block__stem_block_conv0__Relu______muli___443_datasize 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stem_block__stem_block_conv0__Relu______muli___443_expoffset 	6
#define flexidag_openseg_prim_split_1_PeleeNet__stem_block__stem_block_conv0__Relu______muli___443_expbits 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stem_block__stem_block_conv0__Relu______muli___443_drotate 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stem_block__stem_block_conv0__Relu______muli___443_hflip 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stem_block__stem_block_conv0__Relu______muli___443_vflip 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stem_block__stem_block_conv0__Relu______muli___443_dflip 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stem_block__stem_block_conv0__Relu______muli___443_pflip 	0
#define flexidag_openseg_prim_split_1_PeleeNet__stem_block__stem_block_conv0__Relu______muli___443_trailing_extra_elements 	0

/* flexidag_openseg_prim_split_1 HMB_output Relu */
#define flexidag_openseg_prim_split_1_Relu_cnngen_demangled_name 	"Relu"
#define flexidag_openseg_prim_split_1_Relu_is_main_input_output 	0
#define flexidag_openseg_prim_split_1_Relu_is_bitvector 	0
#define flexidag_openseg_prim_split_1_Relu_is_rlz 	0
#define flexidag_openseg_prim_split_1_Relu_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1_Relu_byte_offset 	172	/* 0x000000ac */
#define flexidag_openseg_prim_split_1_Relu_vbase_byte_offset 	176	/* 0x000000b0 */
#define flexidag_openseg_prim_split_1_Relu_dbase_byte_offset 	200	/* 0x000000c8 */
#define flexidag_openseg_prim_split_1_Relu_dpitchm1_byte_offset 	204	/* 0x000000cc */
#define flexidag_openseg_prim_split_1_Relu_dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1_Relu_vwidth_minus_one_byte_offset 	188	/* 0x000000bc */
#define flexidag_openseg_prim_split_1_Relu_vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1_Relu_vheight_minus_one_byte_offset 	190	/* 0x000000be */
#define flexidag_openseg_prim_split_1_Relu_vheight_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_1_Relu_drotate_bit_offset 	1574	/* 0x00000626 */
#define flexidag_openseg_prim_split_1_Relu_hflip_bit_offset 	1575	/* 0x00000627 */
#define flexidag_openseg_prim_split_1_Relu_vflip_bit_offset 	1576	/* 0x00000628 */
#define flexidag_openseg_prim_split_1_Relu_dflip_bit_offset 	1577	/* 0x00000629 */
#define flexidag_openseg_prim_split_1_Relu_pflip_bit_offset 	1578	/* 0x0000062a */
#define flexidag_openseg_prim_split_1_Relu_W 	640
#define flexidag_openseg_prim_split_1_Relu_H 	256
#define flexidag_openseg_prim_split_1_Relu_D 	64
#define flexidag_openseg_prim_split_1_Relu_P 	1
#define flexidag_openseg_prim_split_1_Relu_data_num_bytes 	1
#define flexidag_openseg_prim_split_1_Relu_denable 	1
#define flexidag_openseg_prim_split_1_Relu_dpitch_num_bytes 	64
#define flexidag_openseg_prim_split_1_Relu_dram_format 	3
#define flexidag_openseg_prim_split_1_Relu_vp_interleave_mode 	2
#define flexidag_openseg_prim_split_1_Relu_tile_width 	4
#define flexidag_openseg_prim_split_1_Relu_tile_height 	1
#define flexidag_openseg_prim_split_1_Relu_vmem_buffer_num_bytes 	32768
#define flexidag_openseg_prim_split_1_Relu_dram_size_num_bytes 	10485760
#define flexidag_openseg_prim_split_1_Relu_sign 	0
#define flexidag_openseg_prim_split_1_Relu_datasize 	0
#define flexidag_openseg_prim_split_1_Relu_expoffset 	5
#define flexidag_openseg_prim_split_1_Relu_expbits 	0
#define flexidag_openseg_prim_split_1_Relu_drotate 	0
#define flexidag_openseg_prim_split_1_Relu_hflip 	0
#define flexidag_openseg_prim_split_1_Relu_vflip 	0
#define flexidag_openseg_prim_split_1_Relu_dflip 	0
#define flexidag_openseg_prim_split_1_Relu_pflip 	0
#define flexidag_openseg_prim_split_1_Relu_trailing_extra_elements 	0

/* flexidag_openseg_prim_split_1 SMB_output __vas_created_terminate__v0 */
#define flexidag_openseg_prim_split_1___vas_created_terminate__v0_cnngen_demangled_name 	"__vas_created_terminate__v0"
#define flexidag_openseg_prim_split_1___vas_created_terminate__v0_is_main_input_output 	0
#define flexidag_openseg_prim_split_1___vas_created_terminate__v0_is_bitvector 	1
#define flexidag_openseg_prim_split_1___vas_created_terminate__v0_is_rlz 	0
#define flexidag_openseg_prim_split_1___vas_created_terminate__v0_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_1___vas_created_terminate__v0_byte_offset 	212	/* 0x000000d4 */
#define flexidag_openseg_prim_split_1___vas_created_terminate__v0_vbase_byte_offset 	216	/* 0x000000d8 */
#define flexidag_openseg_prim_split_1___vas_created_terminate__v0_W 	256
#define flexidag_openseg_prim_split_1___vas_created_terminate__v0_H 	3
#define flexidag_openseg_prim_split_1___vas_created_terminate__v0_D 	1
#define flexidag_openseg_prim_split_1___vas_created_terminate__v0_P 	1
#define flexidag_openseg_prim_split_1___vas_created_terminate__v0_data_num_bytes 	1
#define flexidag_openseg_prim_split_1___vas_created_terminate__v0_denable 	0
#define flexidag_openseg_prim_split_1___vas_created_terminate__v0_dram_format 	0
#define flexidag_openseg_prim_split_1___vas_created_terminate__v0_vp_interleave_mode 	1
#define flexidag_openseg_prim_split_1___vas_created_terminate__v0_tile_width 	1
#define flexidag_openseg_prim_split_1___vas_created_terminate__v0_tile_height 	1
#define flexidag_openseg_prim_split_1___vas_created_terminate__v0_vmem_buffer_num_bytes 	96


#endif /* flexidag_openseg_prim_split_1_H */
