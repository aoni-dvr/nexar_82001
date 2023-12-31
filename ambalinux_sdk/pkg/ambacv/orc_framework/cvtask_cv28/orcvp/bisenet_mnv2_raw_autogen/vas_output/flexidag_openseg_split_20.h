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
#ifndef flexidag_openseg_prim_split_20_H
#define flexidag_openseg_prim_split_20_H

#define vas_gen_header_version                  	7

/* dag name */
#define flexidag_openseg_prim_split_20_DAG_NAME 	"flexidag_openseg_prim_split_20"

/* VDG file info */
#define flexidag_openseg_prim_split_20_vdg_name 	"flexidag_openseg_split_20.vdg"

/* VDG DAG memory info */
#define flexidag_openseg_prim_split_20_byte_offset 	4294967295	/* 0xffffffff */
#define flexidag_openseg_prim_split_20_byte_size 	420	/* 0x000001a4 */
#define flexidag_openseg_prim_split_20_preferred_dram_xfer_size 	1

/* flexidag_openseg_prim_split_20 VMEM info */
#define flexidag_openseg_prim_split_20_use_ping_pong_vmem 	0
#define flexidag_openseg_prim_split_20_VMEM_start 	4096	/* 0x00001000 */
#define flexidag_openseg_prim_split_20_VMEM_end 	475980	/* 0x0007434c */
#define flexidag_openseg_prim_split_20_image_start 	470112	/* 0x00072c60 */
#define flexidag_openseg_prim_split_20_image_size 	6288	/* 0x00001890 */
#define flexidag_openseg_prim_split_20_dagbin_start 	475980	/* 0x0007434c */

/* flexidag_openseg_prim_split_20 DAG info */
#define flexidag_openseg_prim_split_20_estimated_cycles 	74378
#define flexidag_openseg_prim_split_20_ddi_byte_size 	0	/* 0x00000000 */

/*******************************************************************************
 * DAG byte offset info for HMB/SMB/VCB and their fields
 * in DAG 'flexidag_openseg_prim_split_20' in source file 'pre_split15_flexidag_openseg_split_4.vas'
 ******************************************************************************/
/* flexidag_openseg_prim_split_20 HMB_input PeleeNet__stage_3_dense_block_1__concat______muli___472 */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_1__concat______muli___472_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_1__concat______muli___472"
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_1__concat______muli___472_is_main_input_output 	0
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_1__concat______muli___472_is_constant 	0
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_1__concat______muli___472_has_init_data 	0
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_1__concat______muli___472_is_variable_scalar 	0
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_1__concat______muli___472_ddi_byte_size 	0	/* 0x00000000 */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_1__concat______muli___472_ddi_byte_offset 	4294967295	/* 0xffffffff */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_1__concat______muli___472_is_bitvector 	0
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_1__concat______muli___472_is_rlz 	0
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_1__concat______muli___472_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_1__concat______muli___472_byte_offset 	0	/* 0x00000000 */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_1__concat______muli___472_vbase_byte_offset 	4	/* 0x00000004 */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_1__concat______muli___472_dbase_byte_offset 	28	/* 0x0000001c */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_1__concat______muli___472_dpitchm1_byte_offset 	32	/* 0x00000020 */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_1__concat______muli___472_dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_1__concat______muli___472_vwidth_minus_one_byte_offset 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_1__concat______muli___472_vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_1__concat______muli___472_vheight_minus_one_byte_offset 	18	/* 0x00000012 */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_1__concat______muli___472_vheight_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_1__concat______muli___472_drotate_bit_offset 	198	/* 0x000000c6 */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_1__concat______muli___472_hflip_bit_offset 	199	/* 0x000000c7 */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_1__concat______muli___472_vflip_bit_offset 	200	/* 0x000000c8 */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_1__concat______muli___472_dflip_bit_offset 	201	/* 0x000000c9 */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_1__concat______muli___472_pflip_bit_offset 	202	/* 0x000000ca */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_1__concat______muli___472_W 	80
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_1__concat______muli___472_H 	32
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_1__concat______muli___472_D 	320
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_1__concat______muli___472_P 	1
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_1__concat______muli___472_data_num_bytes 	1
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_1__concat______muli___472_denable 	1
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_1__concat______muli___472_dpitch_num_bytes 	64
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_1__concat______muli___472_dram_format 	3
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_1__concat______muli___472_vp_interleave_mode 	2
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_1__concat______muli___472_tile_width 	4
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_1__concat______muli___472_tile_height 	1
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_1__concat______muli___472_vmem_buffer_num_bytes 	20480
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_1__concat______muli___472_dram_size_num_bytes 	819200
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_1__concat______muli___472_sign 	0
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_1__concat______muli___472_datasize 	0
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_1__concat______muli___472_expoffset 	5
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_1__concat______muli___472_expbits 	0
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_1__concat______muli___472_drotate 	0
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_1__concat______muli___472_hflip 	0
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_1__concat______muli___472_vflip 	0
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_1__concat______muli___472_dflip 	0
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_1__concat______muli___472_pflip 	0
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_1__concat______muli___472_trailing_extra_elements 	0

/* flexidag_openseg_prim_split_20 HMB_input PeleeNet__stage_3_dense_block_2__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_2__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu"
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_main_input_output 	0
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_constant 	0
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_has_init_data 	0
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_variable_scalar 	0
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_ddi_byte_size 	0	/* 0x00000000 */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_ddi_byte_offset 	4294967295	/* 0xffffffff */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_bitvector 	0
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_rlz 	0
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_byte_offset 	40	/* 0x00000028 */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_vbase_byte_offset 	44	/* 0x0000002c */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_dbase_byte_offset 	68	/* 0x00000044 */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_dpitchm1_byte_offset 	72	/* 0x00000048 */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_vwidth_minus_one_byte_offset 	56	/* 0x00000038 */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_vheight_minus_one_byte_offset 	58	/* 0x0000003a */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_vheight_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_drotate_bit_offset 	518	/* 0x00000206 */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_hflip_bit_offset 	519	/* 0x00000207 */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_vflip_bit_offset 	520	/* 0x00000208 */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_dflip_bit_offset 	521	/* 0x00000209 */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_pflip_bit_offset 	522	/* 0x0000020a */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_W 	80
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_H 	32
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_D 	16
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_P 	1
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_data_num_bytes 	1
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_denable 	1
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_dpitch_num_bytes 	64
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_format 	3
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_vp_interleave_mode 	2
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_tile_width 	4
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_tile_height 	1
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_vmem_buffer_num_bytes 	20480
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_dram_size_num_bytes 	40960
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_sign 	0
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_datasize 	0
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_expoffset 	5
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_expbits 	0
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_drotate 	0
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_hflip 	0
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_vflip 	0
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_dflip 	0
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_pflip 	0
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_right_2__Conv2D___BatchNorm__FusedBatchNorm___Relu_trailing_extra_elements 	0

/* flexidag_openseg_prim_split_20 HMB_input PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___402 */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___402_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___402"
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___402_is_main_input_output 	0
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___402_is_constant 	0
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___402_has_init_data 	0
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___402_is_variable_scalar 	0
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___402_ddi_byte_size 	0	/* 0x00000000 */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___402_ddi_byte_offset 	4294967295	/* 0xffffffff */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___402_is_bitvector 	0
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___402_is_rlz 	0
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___402_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___402_byte_offset 	80	/* 0x00000050 */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___402_vbase_byte_offset 	84	/* 0x00000054 */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___402_dbase_byte_offset 	108	/* 0x0000006c */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___402_dpitchm1_byte_offset 	112	/* 0x00000070 */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___402_dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___402_vwidth_minus_one_byte_offset 	96	/* 0x00000060 */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___402_vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___402_vheight_minus_one_byte_offset 	98	/* 0x00000062 */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___402_vheight_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___402_drotate_bit_offset 	838	/* 0x00000346 */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___402_hflip_bit_offset 	839	/* 0x00000347 */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___402_vflip_bit_offset 	840	/* 0x00000348 */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___402_dflip_bit_offset 	841	/* 0x00000349 */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___402_pflip_bit_offset 	842	/* 0x0000034a */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___402_W 	80
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___402_H 	32
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___402_D 	64
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___402_P 	1
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___402_data_num_bytes 	1
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___402_denable 	1
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___402_dpitch_num_bytes 	64
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___402_dram_format 	3
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___402_vp_interleave_mode 	2
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___402_tile_width 	4
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___402_tile_height 	1
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___402_vmem_buffer_num_bytes 	40960
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___402_dram_size_num_bytes 	163840
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___402_sign 	1
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___402_datasize 	0
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___402_expoffset 	5
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___402_expbits 	0
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___402_drotate 	0
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___402_hflip 	0
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___402_vflip 	0
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___402_dflip 	0
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___402_pflip 	0
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_left_0__Conv2D___BatchNorm__FusedBatchNorm___right_0__Conv2D___crop___402_trailing_extra_elements 	0

/* flexidag_openseg_prim_split_20 SMB_input __pvcn_921_ */
#define flexidag_openseg_prim_split_20___pvcn_921__cnngen_demangled_name 	"__pvcn_921_"
#define flexidag_openseg_prim_split_20___pvcn_921__is_constant 	1
#define flexidag_openseg_prim_split_20___pvcn_921__has_init_data 	1
#define flexidag_openseg_prim_split_20___pvcn_921__is_variable_scalar 	0
#define flexidag_openseg_prim_split_20___pvcn_921__is_bitvector 	0
#define flexidag_openseg_prim_split_20___pvcn_921__is_rlz 	0
#define flexidag_openseg_prim_split_20___pvcn_921__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_20___pvcn_921__byte_offset 	116	/* 0x00000074 */
#define flexidag_openseg_prim_split_20___pvcn_921__vbase_byte_offset 	120	/* 0x00000078 */
#define flexidag_openseg_prim_split_20___pvcn_921__dbase_byte_offset 	144	/* 0x00000090 */
#define flexidag_openseg_prim_split_20___pvcn_921__dpitchm1_byte_offset 	148	/* 0x00000094 */
#define flexidag_openseg_prim_split_20___pvcn_921__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_20___pvcn_921__W 	5772
#define flexidag_openseg_prim_split_20___pvcn_921__H 	1
#define flexidag_openseg_prim_split_20___pvcn_921__D 	1
#define flexidag_openseg_prim_split_20___pvcn_921__P 	1
#define flexidag_openseg_prim_split_20___pvcn_921__data_num_bytes 	1
#define flexidag_openseg_prim_split_20___pvcn_921__denable 	1
#define flexidag_openseg_prim_split_20___pvcn_921__dpitch_num_bytes 	5824
#define flexidag_openseg_prim_split_20___pvcn_921__dram_format 	0
#define flexidag_openseg_prim_split_20___pvcn_921__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_20___pvcn_921__tile_width 	1
#define flexidag_openseg_prim_split_20___pvcn_921__tile_height 	1
#define flexidag_openseg_prim_split_20___pvcn_921__vmem_buffer_num_bytes 	5772
#define flexidag_openseg_prim_split_20___pvcn_921__dram_size_num_bytes 	5824

/* flexidag_openseg_prim_split_20 SMB_input __pvcn_922_ */
#define flexidag_openseg_prim_split_20___pvcn_922__cnngen_demangled_name 	"__pvcn_922_"
#define flexidag_openseg_prim_split_20___pvcn_922__is_constant 	1
#define flexidag_openseg_prim_split_20___pvcn_922__has_init_data 	1
#define flexidag_openseg_prim_split_20___pvcn_922__is_variable_scalar 	0
#define flexidag_openseg_prim_split_20___pvcn_922__is_bitvector 	0
#define flexidag_openseg_prim_split_20___pvcn_922__is_rlz 	0
#define flexidag_openseg_prim_split_20___pvcn_922__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_20___pvcn_922__byte_offset 	156	/* 0x0000009c */
#define flexidag_openseg_prim_split_20___pvcn_922__vbase_byte_offset 	160	/* 0x000000a0 */
#define flexidag_openseg_prim_split_20___pvcn_922__dbase_byte_offset 	184	/* 0x000000b8 */
#define flexidag_openseg_prim_split_20___pvcn_922__dpitchm1_byte_offset 	188	/* 0x000000bc */
#define flexidag_openseg_prim_split_20___pvcn_922__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_20___pvcn_922__W 	96
#define flexidag_openseg_prim_split_20___pvcn_922__H 	1
#define flexidag_openseg_prim_split_20___pvcn_922__D 	1
#define flexidag_openseg_prim_split_20___pvcn_922__P 	1
#define flexidag_openseg_prim_split_20___pvcn_922__data_num_bytes 	1
#define flexidag_openseg_prim_split_20___pvcn_922__denable 	1
#define flexidag_openseg_prim_split_20___pvcn_922__dpitch_num_bytes 	128
#define flexidag_openseg_prim_split_20___pvcn_922__dram_format 	0
#define flexidag_openseg_prim_split_20___pvcn_922__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_20___pvcn_922__tile_width 	1
#define flexidag_openseg_prim_split_20___pvcn_922__tile_height 	1
#define flexidag_openseg_prim_split_20___pvcn_922__vmem_buffer_num_bytes 	96
#define flexidag_openseg_prim_split_20___pvcn_922__dram_size_num_bytes 	128

/* flexidag_openseg_prim_split_20 HMB_output PeleeNet__stage_3_dense_block_2__concat______muli___475 */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__concat______muli___475_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_2__concat______muli___475"
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__concat______muli___475_is_main_input_output 	0
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__concat______muli___475_is_bitvector 	0
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__concat______muli___475_is_rlz 	0
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__concat______muli___475_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__concat______muli___475_byte_offset 	196	/* 0x000000c4 */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__concat______muli___475_vbase_byte_offset 	200	/* 0x000000c8 */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__concat______muli___475_dbase_byte_offset 	224	/* 0x000000e0 */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__concat______muli___475_dpitchm1_byte_offset 	228	/* 0x000000e4 */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__concat______muli___475_dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__concat______muli___475_vwidth_minus_one_byte_offset 	212	/* 0x000000d4 */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__concat______muli___475_vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__concat______muli___475_vheight_minus_one_byte_offset 	214	/* 0x000000d6 */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__concat______muli___475_vheight_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__concat______muli___475_drotate_bit_offset 	1766	/* 0x000006e6 */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__concat______muli___475_hflip_bit_offset 	1767	/* 0x000006e7 */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__concat______muli___475_vflip_bit_offset 	1768	/* 0x000006e8 */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__concat______muli___475_dflip_bit_offset 	1769	/* 0x000006e9 */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__concat______muli___475_pflip_bit_offset 	1770	/* 0x000006ea */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__concat______muli___475_W 	80
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__concat______muli___475_H 	32
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__concat______muli___475_D 	352
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__concat______muli___475_P 	1
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__concat______muli___475_data_num_bytes 	1
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__concat______muli___475_denable 	1
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__concat______muli___475_dpitch_num_bytes 	64
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__concat______muli___475_dram_format 	3
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__concat______muli___475_vp_interleave_mode 	2
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__concat______muli___475_tile_width 	4
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__concat______muli___475_tile_height 	1
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__concat______muli___475_vmem_buffer_num_bytes 	225280
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__concat______muli___475_dram_size_num_bytes 	901120
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__concat______muli___475_sign 	0
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__concat______muli___475_datasize 	0
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__concat______muli___475_expoffset 	5
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__concat______muli___475_expbits 	0
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__concat______muli___475_drotate 	0
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__concat______muli___475_hflip 	0
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__concat______muli___475_vflip 	0
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__concat______muli___475_dflip 	0
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__concat______muli___475_pflip 	0
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__concat______muli___475_trailing_extra_elements 	0

/* flexidag_openseg_prim_split_20 VCB PeleeNet__stage_3_dense_block_2__conv_left_0__Relu */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_left_0__Relu_cnngen_demangled_name 	"PeleeNet__stage_3_dense_block_2__conv_left_0__Relu"
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_left_0__Relu_is_bitvector 	0
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_left_0__Relu_is_rlz 	0
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_left_0__Relu_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_left_0__Relu_byte_offset 	244	/* 0x000000f4 */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_left_0__Relu_vbase_byte_offset 	252	/* 0x000000fc */
#define flexidag_openseg_prim_split_20_PeleeNet__stage_3_dense_block_2__conv_left_0__Relu_vmem_buffer_num_bytes 	158720

/* flexidag_openseg_prim_split_20 SMB_output __vas_created_terminate__v0 */
#define flexidag_openseg_prim_split_20___vas_created_terminate__v0_cnngen_demangled_name 	"__vas_created_terminate__v0"
#define flexidag_openseg_prim_split_20___vas_created_terminate__v0_is_main_input_output 	0
#define flexidag_openseg_prim_split_20___vas_created_terminate__v0_is_bitvector 	1
#define flexidag_openseg_prim_split_20___vas_created_terminate__v0_is_rlz 	0
#define flexidag_openseg_prim_split_20___vas_created_terminate__v0_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_20___vas_created_terminate__v0_byte_offset 	344	/* 0x00000158 */
#define flexidag_openseg_prim_split_20___vas_created_terminate__v0_vbase_byte_offset 	348	/* 0x0000015c */
#define flexidag_openseg_prim_split_20___vas_created_terminate__v0_W 	256
#define flexidag_openseg_prim_split_20___vas_created_terminate__v0_H 	3
#define flexidag_openseg_prim_split_20___vas_created_terminate__v0_D 	1
#define flexidag_openseg_prim_split_20___vas_created_terminate__v0_P 	1
#define flexidag_openseg_prim_split_20___vas_created_terminate__v0_data_num_bytes 	1
#define flexidag_openseg_prim_split_20___vas_created_terminate__v0_denable 	0
#define flexidag_openseg_prim_split_20___vas_created_terminate__v0_dram_format 	0
#define flexidag_openseg_prim_split_20___vas_created_terminate__v0_vp_interleave_mode 	1
#define flexidag_openseg_prim_split_20___vas_created_terminate__v0_tile_width 	1
#define flexidag_openseg_prim_split_20___vas_created_terminate__v0_tile_height 	1
#define flexidag_openseg_prim_split_20___vas_created_terminate__v0_vmem_buffer_num_bytes 	96


#endif /* flexidag_openseg_prim_split_20_H */
