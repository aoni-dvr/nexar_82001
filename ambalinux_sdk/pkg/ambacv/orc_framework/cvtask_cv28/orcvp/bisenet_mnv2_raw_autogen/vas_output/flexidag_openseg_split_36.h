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
#ifndef flexidag_openseg_prim_split_36_H
#define flexidag_openseg_prim_split_36_H

#define vas_gen_header_version                  	7

/* dag name */
#define flexidag_openseg_prim_split_36_DAG_NAME 	"flexidag_openseg_prim_split_36"

/* VDG file info */
#define flexidag_openseg_prim_split_36_vdg_name 	"flexidag_openseg_split_36.vdg"

/* VDG DAG memory info */
#define flexidag_openseg_prim_split_36_byte_offset 	4294967295	/* 0xffffffff */
#define flexidag_openseg_prim_split_36_byte_size 	200	/* 0x000000c8 */
#define flexidag_openseg_prim_split_36_preferred_dram_xfer_size 	1

/* flexidag_openseg_prim_split_36 VMEM info */
#define flexidag_openseg_prim_split_36_use_ping_pong_vmem 	0
#define flexidag_openseg_prim_split_36_VMEM_start 	4096	/* 0x00001000 */
#define flexidag_openseg_prim_split_36_VMEM_end 	499836	/* 0x0007a07c */
#define flexidag_openseg_prim_split_36_image_start 	499820	/* 0x0007a06c */
#define flexidag_openseg_prim_split_36_image_size 	216	/* 0x000000d8 */
#define flexidag_openseg_prim_split_36_dagbin_start 	499836	/* 0x0007a07c */

/* flexidag_openseg_prim_split_36 DAG info */
#define flexidag_openseg_prim_split_36_estimated_cycles 	655408
#define flexidag_openseg_prim_split_36_ddi_byte_size 	0	/* 0x00000000 */

/*******************************************************************************
 * DAG byte offset info for HMB/SMB/VCB and their fields
 * in DAG 'flexidag_openseg_prim_split_36' in source file 'pre_split25_flexidag_openseg_split_0.vas'
 ******************************************************************************/
/* flexidag_openseg_prim_split_36 HMB_input Mul_2 */
#define flexidag_openseg_prim_split_36_Mul_2_cnngen_demangled_name 	"Mul_2"
#define flexidag_openseg_prim_split_36_Mul_2_is_main_input_output 	0
#define flexidag_openseg_prim_split_36_Mul_2_is_constant 	0
#define flexidag_openseg_prim_split_36_Mul_2_has_init_data 	0
#define flexidag_openseg_prim_split_36_Mul_2_is_variable_scalar 	0
#define flexidag_openseg_prim_split_36_Mul_2_ddi_byte_size 	0	/* 0x00000000 */
#define flexidag_openseg_prim_split_36_Mul_2_ddi_byte_offset 	4294967295	/* 0xffffffff */
#define flexidag_openseg_prim_split_36_Mul_2_is_bitvector 	0
#define flexidag_openseg_prim_split_36_Mul_2_is_rlz 	0
#define flexidag_openseg_prim_split_36_Mul_2_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_36_Mul_2_byte_offset 	0	/* 0x00000000 */
#define flexidag_openseg_prim_split_36_Mul_2_vbase_byte_offset 	4	/* 0x00000004 */
#define flexidag_openseg_prim_split_36_Mul_2_dbase_byte_offset 	28	/* 0x0000001c */
#define flexidag_openseg_prim_split_36_Mul_2_dpitchm1_byte_offset 	32	/* 0x00000020 */
#define flexidag_openseg_prim_split_36_Mul_2_dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_36_Mul_2_vwidth_minus_one_byte_offset 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_36_Mul_2_vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_36_Mul_2_vheight_minus_one_byte_offset 	18	/* 0x00000012 */
#define flexidag_openseg_prim_split_36_Mul_2_vheight_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_36_Mul_2_drotate_bit_offset 	198	/* 0x000000c6 */
#define flexidag_openseg_prim_split_36_Mul_2_hflip_bit_offset 	199	/* 0x000000c7 */
#define flexidag_openseg_prim_split_36_Mul_2_vflip_bit_offset 	200	/* 0x000000c8 */
#define flexidag_openseg_prim_split_36_Mul_2_dflip_bit_offset 	201	/* 0x000000c9 */
#define flexidag_openseg_prim_split_36_Mul_2_pflip_bit_offset 	202	/* 0x000000ca */
#define flexidag_openseg_prim_split_36_Mul_2_W 	40
#define flexidag_openseg_prim_split_36_Mul_2_H 	16
#define flexidag_openseg_prim_split_36_Mul_2_D 	512
#define flexidag_openseg_prim_split_36_Mul_2_P 	1
#define flexidag_openseg_prim_split_36_Mul_2_data_num_bytes 	2
#define flexidag_openseg_prim_split_36_Mul_2_denable 	1
#define flexidag_openseg_prim_split_36_Mul_2_dpitch_num_bytes 	64
#define flexidag_openseg_prim_split_36_Mul_2_dram_format 	3
#define flexidag_openseg_prim_split_36_Mul_2_vp_interleave_mode 	2
#define flexidag_openseg_prim_split_36_Mul_2_tile_width 	4
#define flexidag_openseg_prim_split_36_Mul_2_tile_height 	1
#define flexidag_openseg_prim_split_36_Mul_2_vmem_buffer_num_bytes 	165900
#define flexidag_openseg_prim_split_36_Mul_2_dram_size_num_bytes 	655360
#define flexidag_openseg_prim_split_36_Mul_2_sign 	0
#define flexidag_openseg_prim_split_36_Mul_2_datasize 	1
#define flexidag_openseg_prim_split_36_Mul_2_expoffset 	13
#define flexidag_openseg_prim_split_36_Mul_2_expbits 	0
#define flexidag_openseg_prim_split_36_Mul_2_drotate 	0
#define flexidag_openseg_prim_split_36_Mul_2_hflip 	0
#define flexidag_openseg_prim_split_36_Mul_2_vflip 	0
#define flexidag_openseg_prim_split_36_Mul_2_dflip 	0
#define flexidag_openseg_prim_split_36_Mul_2_pflip 	0
#define flexidag_openseg_prim_split_36_Mul_2_trailing_extra_elements 	0

/* flexidag_openseg_prim_split_36 SMB_input __pvcn_1142_ */
#define flexidag_openseg_prim_split_36___pvcn_1142__cnngen_demangled_name 	"__pvcn_1142_"
#define flexidag_openseg_prim_split_36___pvcn_1142__is_constant 	1
#define flexidag_openseg_prim_split_36___pvcn_1142__has_init_data 	1
#define flexidag_openseg_prim_split_36___pvcn_1142__is_variable_scalar 	0
#define flexidag_openseg_prim_split_36___pvcn_1142__is_bitvector 	0
#define flexidag_openseg_prim_split_36___pvcn_1142__is_rlz 	0
#define flexidag_openseg_prim_split_36___pvcn_1142__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_36___pvcn_1142__byte_offset 	36	/* 0x00000024 */
#define flexidag_openseg_prim_split_36___pvcn_1142__vbase_byte_offset 	40	/* 0x00000028 */
#define flexidag_openseg_prim_split_36___pvcn_1142__dbase_byte_offset 	64	/* 0x00000040 */
#define flexidag_openseg_prim_split_36___pvcn_1142__dpitchm1_byte_offset 	68	/* 0x00000044 */
#define flexidag_openseg_prim_split_36___pvcn_1142__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_36___pvcn_1142__W 	4
#define flexidag_openseg_prim_split_36___pvcn_1142__H 	1
#define flexidag_openseg_prim_split_36___pvcn_1142__D 	1
#define flexidag_openseg_prim_split_36___pvcn_1142__P 	1
#define flexidag_openseg_prim_split_36___pvcn_1142__data_num_bytes 	4
#define flexidag_openseg_prim_split_36___pvcn_1142__denable 	1
#define flexidag_openseg_prim_split_36___pvcn_1142__dpitch_num_bytes 	64
#define flexidag_openseg_prim_split_36___pvcn_1142__dram_format 	0
#define flexidag_openseg_prim_split_36___pvcn_1142__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_36___pvcn_1142__tile_width 	1
#define flexidag_openseg_prim_split_36___pvcn_1142__tile_height 	1
#define flexidag_openseg_prim_split_36___pvcn_1142__vmem_buffer_num_bytes 	16
#define flexidag_openseg_prim_split_36___pvcn_1142__dram_size_num_bytes 	64

/* flexidag_openseg_prim_split_36 HMB_output ResizeBilinear_1 */
#define flexidag_openseg_prim_split_36_ResizeBilinear_1_cnngen_demangled_name 	"ResizeBilinear_1"
#define flexidag_openseg_prim_split_36_ResizeBilinear_1_is_main_input_output 	0
#define flexidag_openseg_prim_split_36_ResizeBilinear_1_is_bitvector 	0
#define flexidag_openseg_prim_split_36_ResizeBilinear_1_is_rlz 	0
#define flexidag_openseg_prim_split_36_ResizeBilinear_1_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_36_ResizeBilinear_1_byte_offset 	84	/* 0x00000054 */
#define flexidag_openseg_prim_split_36_ResizeBilinear_1_vbase_byte_offset 	88	/* 0x00000058 */
#define flexidag_openseg_prim_split_36_ResizeBilinear_1_dbase_byte_offset 	112	/* 0x00000070 */
#define flexidag_openseg_prim_split_36_ResizeBilinear_1_dpitchm1_byte_offset 	116	/* 0x00000074 */
#define flexidag_openseg_prim_split_36_ResizeBilinear_1_dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_36_ResizeBilinear_1_vwidth_minus_one_byte_offset 	100	/* 0x00000064 */
#define flexidag_openseg_prim_split_36_ResizeBilinear_1_vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_36_ResizeBilinear_1_vheight_minus_one_byte_offset 	102	/* 0x00000066 */
#define flexidag_openseg_prim_split_36_ResizeBilinear_1_vheight_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_36_ResizeBilinear_1_drotate_bit_offset 	870	/* 0x00000366 */
#define flexidag_openseg_prim_split_36_ResizeBilinear_1_hflip_bit_offset 	871	/* 0x00000367 */
#define flexidag_openseg_prim_split_36_ResizeBilinear_1_vflip_bit_offset 	872	/* 0x00000368 */
#define flexidag_openseg_prim_split_36_ResizeBilinear_1_dflip_bit_offset 	873	/* 0x00000369 */
#define flexidag_openseg_prim_split_36_ResizeBilinear_1_pflip_bit_offset 	874	/* 0x0000036a */
#define flexidag_openseg_prim_split_36_ResizeBilinear_1_W 	160
#define flexidag_openseg_prim_split_36_ResizeBilinear_1_H 	64
#define flexidag_openseg_prim_split_36_ResizeBilinear_1_D 	512
#define flexidag_openseg_prim_split_36_ResizeBilinear_1_P 	1
#define flexidag_openseg_prim_split_36_ResizeBilinear_1_data_num_bytes 	2
#define flexidag_openseg_prim_split_36_ResizeBilinear_1_denable 	1
#define flexidag_openseg_prim_split_36_ResizeBilinear_1_dpitch_num_bytes 	64
#define flexidag_openseg_prim_split_36_ResizeBilinear_1_dram_format 	3
#define flexidag_openseg_prim_split_36_ResizeBilinear_1_vp_interleave_mode 	2
#define flexidag_openseg_prim_split_36_ResizeBilinear_1_tile_width 	1
#define flexidag_openseg_prim_split_36_ResizeBilinear_1_tile_height 	1
#define flexidag_openseg_prim_split_36_ResizeBilinear_1_vmem_buffer_num_bytes 	329728
#define flexidag_openseg_prim_split_36_ResizeBilinear_1_dram_size_num_bytes 	10485760
#define flexidag_openseg_prim_split_36_ResizeBilinear_1_sign 	0
#define flexidag_openseg_prim_split_36_ResizeBilinear_1_datasize 	1
#define flexidag_openseg_prim_split_36_ResizeBilinear_1_expoffset 	13
#define flexidag_openseg_prim_split_36_ResizeBilinear_1_expbits 	0
#define flexidag_openseg_prim_split_36_ResizeBilinear_1_drotate 	0
#define flexidag_openseg_prim_split_36_ResizeBilinear_1_hflip 	0
#define flexidag_openseg_prim_split_36_ResizeBilinear_1_vflip 	0
#define flexidag_openseg_prim_split_36_ResizeBilinear_1_dflip 	0
#define flexidag_openseg_prim_split_36_ResizeBilinear_1_pflip 	0
#define flexidag_openseg_prim_split_36_ResizeBilinear_1_trailing_extra_elements 	0

/* flexidag_openseg_prim_split_36 SMB_output __vas_created_terminate__v0 */
#define flexidag_openseg_prim_split_36___vas_created_terminate__v0_cnngen_demangled_name 	"__vas_created_terminate__v0"
#define flexidag_openseg_prim_split_36___vas_created_terminate__v0_is_main_input_output 	0
#define flexidag_openseg_prim_split_36___vas_created_terminate__v0_is_bitvector 	1
#define flexidag_openseg_prim_split_36___vas_created_terminate__v0_is_rlz 	0
#define flexidag_openseg_prim_split_36___vas_created_terminate__v0_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_36___vas_created_terminate__v0_byte_offset 	124	/* 0x0000007c */
#define flexidag_openseg_prim_split_36___vas_created_terminate__v0_vbase_byte_offset 	128	/* 0x00000080 */
#define flexidag_openseg_prim_split_36___vas_created_terminate__v0_W 	256
#define flexidag_openseg_prim_split_36___vas_created_terminate__v0_H 	3
#define flexidag_openseg_prim_split_36___vas_created_terminate__v0_D 	1
#define flexidag_openseg_prim_split_36___vas_created_terminate__v0_P 	1
#define flexidag_openseg_prim_split_36___vas_created_terminate__v0_data_num_bytes 	1
#define flexidag_openseg_prim_split_36___vas_created_terminate__v0_denable 	0
#define flexidag_openseg_prim_split_36___vas_created_terminate__v0_dram_format 	0
#define flexidag_openseg_prim_split_36___vas_created_terminate__v0_vp_interleave_mode 	1
#define flexidag_openseg_prim_split_36___vas_created_terminate__v0_tile_width 	1
#define flexidag_openseg_prim_split_36___vas_created_terminate__v0_tile_height 	1
#define flexidag_openseg_prim_split_36___vas_created_terminate__v0_vmem_buffer_num_bytes 	96


#endif /* flexidag_openseg_prim_split_36_H */
