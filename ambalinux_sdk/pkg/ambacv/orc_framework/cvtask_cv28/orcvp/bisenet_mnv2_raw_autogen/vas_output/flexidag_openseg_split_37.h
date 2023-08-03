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
#ifndef flexidag_openseg_prim_split_37_H
#define flexidag_openseg_prim_split_37_H

#define vas_gen_header_version                  	7

/* dag name */
#define flexidag_openseg_prim_split_37_DAG_NAME 	"flexidag_openseg_prim_split_37"

/* VDG file info */
#define flexidag_openseg_prim_split_37_vdg_name 	"flexidag_openseg_split_37.vdg"

/* VDG DAG memory info */
#define flexidag_openseg_prim_split_37_byte_offset 	4294967295	/* 0xffffffff */
#define flexidag_openseg_prim_split_37_byte_size 	200	/* 0x000000c8 */
#define flexidag_openseg_prim_split_37_preferred_dram_xfer_size 	1

/* flexidag_openseg_prim_split_37 VMEM info */
#define flexidag_openseg_prim_split_37_use_ping_pong_vmem 	0
#define flexidag_openseg_prim_split_37_VMEM_start 	4096	/* 0x00001000 */
#define flexidag_openseg_prim_split_37_VMEM_end 	403552	/* 0x00062860 */
#define flexidag_openseg_prim_split_37_image_start 	403552	/* 0x00062860 */
#define flexidag_openseg_prim_split_37_image_size 	200	/* 0x000000c8 */
#define flexidag_openseg_prim_split_37_dagbin_start 	403552	/* 0x00062860 */

/* flexidag_openseg_prim_split_37 DAG info */
#define flexidag_openseg_prim_split_37_estimated_cycles 	983088
#define flexidag_openseg_prim_split_37_ddi_byte_size 	0	/* 0x00000000 */

/*******************************************************************************
 * DAG byte offset info for HMB/SMB/VCB and their fields
 * in DAG 'flexidag_openseg_prim_split_37' in source file 'pre_split26_flexidag_openseg_split_0.vas'
 ******************************************************************************/
/* flexidag_openseg_prim_split_37 HMB_input ResizeBilinear */
#define flexidag_openseg_prim_split_37_ResizeBilinear_cnngen_demangled_name 	"ResizeBilinear"
#define flexidag_openseg_prim_split_37_ResizeBilinear_is_main_input_output 	0
#define flexidag_openseg_prim_split_37_ResizeBilinear_is_constant 	0
#define flexidag_openseg_prim_split_37_ResizeBilinear_has_init_data 	0
#define flexidag_openseg_prim_split_37_ResizeBilinear_is_variable_scalar 	0
#define flexidag_openseg_prim_split_37_ResizeBilinear_ddi_byte_size 	0	/* 0x00000000 */
#define flexidag_openseg_prim_split_37_ResizeBilinear_ddi_byte_offset 	4294967295	/* 0xffffffff */
#define flexidag_openseg_prim_split_37_ResizeBilinear_is_bitvector 	0
#define flexidag_openseg_prim_split_37_ResizeBilinear_is_rlz 	0
#define flexidag_openseg_prim_split_37_ResizeBilinear_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_37_ResizeBilinear_byte_offset 	0	/* 0x00000000 */
#define flexidag_openseg_prim_split_37_ResizeBilinear_vbase_byte_offset 	4	/* 0x00000004 */
#define flexidag_openseg_prim_split_37_ResizeBilinear_dbase_byte_offset 	28	/* 0x0000001c */
#define flexidag_openseg_prim_split_37_ResizeBilinear_dpitchm1_byte_offset 	32	/* 0x00000020 */
#define flexidag_openseg_prim_split_37_ResizeBilinear_dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_37_ResizeBilinear_vwidth_minus_one_byte_offset 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_37_ResizeBilinear_vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_37_ResizeBilinear_vheight_minus_one_byte_offset 	18	/* 0x00000012 */
#define flexidag_openseg_prim_split_37_ResizeBilinear_vheight_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_37_ResizeBilinear_drotate_bit_offset 	198	/* 0x000000c6 */
#define flexidag_openseg_prim_split_37_ResizeBilinear_hflip_bit_offset 	199	/* 0x000000c7 */
#define flexidag_openseg_prim_split_37_ResizeBilinear_vflip_bit_offset 	200	/* 0x000000c8 */
#define flexidag_openseg_prim_split_37_ResizeBilinear_dflip_bit_offset 	201	/* 0x000000c9 */
#define flexidag_openseg_prim_split_37_ResizeBilinear_pflip_bit_offset 	202	/* 0x000000ca */
#define flexidag_openseg_prim_split_37_ResizeBilinear_W 	160
#define flexidag_openseg_prim_split_37_ResizeBilinear_H 	64
#define flexidag_openseg_prim_split_37_ResizeBilinear_D 	256
#define flexidag_openseg_prim_split_37_ResizeBilinear_P 	1
#define flexidag_openseg_prim_split_37_ResizeBilinear_data_num_bytes 	2
#define flexidag_openseg_prim_split_37_ResizeBilinear_denable 	1
#define flexidag_openseg_prim_split_37_ResizeBilinear_dpitch_num_bytes 	64
#define flexidag_openseg_prim_split_37_ResizeBilinear_dram_format 	3
#define flexidag_openseg_prim_split_37_ResizeBilinear_vp_interleave_mode 	2
#define flexidag_openseg_prim_split_37_ResizeBilinear_tile_width 	1
#define flexidag_openseg_prim_split_37_ResizeBilinear_tile_height 	1
#define flexidag_openseg_prim_split_37_ResizeBilinear_vmem_buffer_num_bytes 	66560
#define flexidag_openseg_prim_split_37_ResizeBilinear_dram_size_num_bytes 	5242880
#define flexidag_openseg_prim_split_37_ResizeBilinear_sign 	0
#define flexidag_openseg_prim_split_37_ResizeBilinear_datasize 	1
#define flexidag_openseg_prim_split_37_ResizeBilinear_expoffset 	13
#define flexidag_openseg_prim_split_37_ResizeBilinear_expbits 	0
#define flexidag_openseg_prim_split_37_ResizeBilinear_drotate 	0
#define flexidag_openseg_prim_split_37_ResizeBilinear_hflip 	0
#define flexidag_openseg_prim_split_37_ResizeBilinear_vflip 	0
#define flexidag_openseg_prim_split_37_ResizeBilinear_dflip 	0
#define flexidag_openseg_prim_split_37_ResizeBilinear_pflip 	0
#define flexidag_openseg_prim_split_37_ResizeBilinear_trailing_extra_elements 	0

/* flexidag_openseg_prim_split_37 HMB_input ResizeBilinear_1 */
#define flexidag_openseg_prim_split_37_ResizeBilinear_1_cnngen_demangled_name 	"ResizeBilinear_1"
#define flexidag_openseg_prim_split_37_ResizeBilinear_1_is_main_input_output 	0
#define flexidag_openseg_prim_split_37_ResizeBilinear_1_is_constant 	0
#define flexidag_openseg_prim_split_37_ResizeBilinear_1_has_init_data 	0
#define flexidag_openseg_prim_split_37_ResizeBilinear_1_is_variable_scalar 	0
#define flexidag_openseg_prim_split_37_ResizeBilinear_1_ddi_byte_size 	0	/* 0x00000000 */
#define flexidag_openseg_prim_split_37_ResizeBilinear_1_ddi_byte_offset 	4294967295	/* 0xffffffff */
#define flexidag_openseg_prim_split_37_ResizeBilinear_1_is_bitvector 	0
#define flexidag_openseg_prim_split_37_ResizeBilinear_1_is_rlz 	0
#define flexidag_openseg_prim_split_37_ResizeBilinear_1_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_37_ResizeBilinear_1_byte_offset 	36	/* 0x00000024 */
#define flexidag_openseg_prim_split_37_ResizeBilinear_1_vbase_byte_offset 	40	/* 0x00000028 */
#define flexidag_openseg_prim_split_37_ResizeBilinear_1_dbase_byte_offset 	64	/* 0x00000040 */
#define flexidag_openseg_prim_split_37_ResizeBilinear_1_dpitchm1_byte_offset 	68	/* 0x00000044 */
#define flexidag_openseg_prim_split_37_ResizeBilinear_1_dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_37_ResizeBilinear_1_vwidth_minus_one_byte_offset 	52	/* 0x00000034 */
#define flexidag_openseg_prim_split_37_ResizeBilinear_1_vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_37_ResizeBilinear_1_vheight_minus_one_byte_offset 	54	/* 0x00000036 */
#define flexidag_openseg_prim_split_37_ResizeBilinear_1_vheight_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_37_ResizeBilinear_1_drotate_bit_offset 	486	/* 0x000001e6 */
#define flexidag_openseg_prim_split_37_ResizeBilinear_1_hflip_bit_offset 	487	/* 0x000001e7 */
#define flexidag_openseg_prim_split_37_ResizeBilinear_1_vflip_bit_offset 	488	/* 0x000001e8 */
#define flexidag_openseg_prim_split_37_ResizeBilinear_1_dflip_bit_offset 	489	/* 0x000001e9 */
#define flexidag_openseg_prim_split_37_ResizeBilinear_1_pflip_bit_offset 	490	/* 0x000001ea */
#define flexidag_openseg_prim_split_37_ResizeBilinear_1_W 	160
#define flexidag_openseg_prim_split_37_ResizeBilinear_1_H 	64
#define flexidag_openseg_prim_split_37_ResizeBilinear_1_D 	512
#define flexidag_openseg_prim_split_37_ResizeBilinear_1_P 	1
#define flexidag_openseg_prim_split_37_ResizeBilinear_1_data_num_bytes 	2
#define flexidag_openseg_prim_split_37_ResizeBilinear_1_denable 	1
#define flexidag_openseg_prim_split_37_ResizeBilinear_1_dpitch_num_bytes 	64
#define flexidag_openseg_prim_split_37_ResizeBilinear_1_dram_format 	3
#define flexidag_openseg_prim_split_37_ResizeBilinear_1_vp_interleave_mode 	2
#define flexidag_openseg_prim_split_37_ResizeBilinear_1_tile_width 	1
#define flexidag_openseg_prim_split_37_ResizeBilinear_1_tile_height 	1
#define flexidag_openseg_prim_split_37_ResizeBilinear_1_vmem_buffer_num_bytes 	133120
#define flexidag_openseg_prim_split_37_ResizeBilinear_1_dram_size_num_bytes 	10485760
#define flexidag_openseg_prim_split_37_ResizeBilinear_1_sign 	0
#define flexidag_openseg_prim_split_37_ResizeBilinear_1_datasize 	1
#define flexidag_openseg_prim_split_37_ResizeBilinear_1_expoffset 	13
#define flexidag_openseg_prim_split_37_ResizeBilinear_1_expbits 	0
#define flexidag_openseg_prim_split_37_ResizeBilinear_1_drotate 	0
#define flexidag_openseg_prim_split_37_ResizeBilinear_1_hflip 	0
#define flexidag_openseg_prim_split_37_ResizeBilinear_1_vflip 	0
#define flexidag_openseg_prim_split_37_ResizeBilinear_1_dflip 	0
#define flexidag_openseg_prim_split_37_ResizeBilinear_1_pflip 	0
#define flexidag_openseg_prim_split_37_ResizeBilinear_1_trailing_extra_elements 	0

/* flexidag_openseg_prim_split_37 HMB_output concat */
#define flexidag_openseg_prim_split_37_concat_cnngen_demangled_name 	"concat"
#define flexidag_openseg_prim_split_37_concat_is_main_input_output 	0
#define flexidag_openseg_prim_split_37_concat_is_bitvector 	0
#define flexidag_openseg_prim_split_37_concat_is_rlz 	0
#define flexidag_openseg_prim_split_37_concat_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_37_concat_byte_offset 	84	/* 0x00000054 */
#define flexidag_openseg_prim_split_37_concat_vbase_byte_offset 	88	/* 0x00000058 */
#define flexidag_openseg_prim_split_37_concat_dbase_byte_offset 	112	/* 0x00000070 */
#define flexidag_openseg_prim_split_37_concat_dpitchm1_byte_offset 	116	/* 0x00000074 */
#define flexidag_openseg_prim_split_37_concat_dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_37_concat_vwidth_minus_one_byte_offset 	100	/* 0x00000064 */
#define flexidag_openseg_prim_split_37_concat_vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_37_concat_vheight_minus_one_byte_offset 	102	/* 0x00000066 */
#define flexidag_openseg_prim_split_37_concat_vheight_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_37_concat_drotate_bit_offset 	870	/* 0x00000366 */
#define flexidag_openseg_prim_split_37_concat_hflip_bit_offset 	871	/* 0x00000367 */
#define flexidag_openseg_prim_split_37_concat_vflip_bit_offset 	872	/* 0x00000368 */
#define flexidag_openseg_prim_split_37_concat_dflip_bit_offset 	873	/* 0x00000369 */
#define flexidag_openseg_prim_split_37_concat_pflip_bit_offset 	874	/* 0x0000036a */
#define flexidag_openseg_prim_split_37_concat_W 	160
#define flexidag_openseg_prim_split_37_concat_H 	64
#define flexidag_openseg_prim_split_37_concat_D 	768
#define flexidag_openseg_prim_split_37_concat_P 	1
#define flexidag_openseg_prim_split_37_concat_data_num_bytes 	2
#define flexidag_openseg_prim_split_37_concat_denable 	1
#define flexidag_openseg_prim_split_37_concat_dpitch_num_bytes 	64
#define flexidag_openseg_prim_split_37_concat_dram_format 	3
#define flexidag_openseg_prim_split_37_concat_vp_interleave_mode 	2
#define flexidag_openseg_prim_split_37_concat_tile_width 	1
#define flexidag_openseg_prim_split_37_concat_tile_height 	1
#define flexidag_openseg_prim_split_37_concat_vmem_buffer_num_bytes 	199680
#define flexidag_openseg_prim_split_37_concat_dram_size_num_bytes 	15728640
#define flexidag_openseg_prim_split_37_concat_sign 	0
#define flexidag_openseg_prim_split_37_concat_datasize 	1
#define flexidag_openseg_prim_split_37_concat_expoffset 	13
#define flexidag_openseg_prim_split_37_concat_expbits 	0
#define flexidag_openseg_prim_split_37_concat_drotate 	0
#define flexidag_openseg_prim_split_37_concat_hflip 	0
#define flexidag_openseg_prim_split_37_concat_vflip 	0
#define flexidag_openseg_prim_split_37_concat_dflip 	0
#define flexidag_openseg_prim_split_37_concat_pflip 	0
#define flexidag_openseg_prim_split_37_concat_trailing_extra_elements 	0

/* flexidag_openseg_prim_split_37 SMB_output __vas_created_terminate__v0 */
#define flexidag_openseg_prim_split_37___vas_created_terminate__v0_cnngen_demangled_name 	"__vas_created_terminate__v0"
#define flexidag_openseg_prim_split_37___vas_created_terminate__v0_is_main_input_output 	0
#define flexidag_openseg_prim_split_37___vas_created_terminate__v0_is_bitvector 	1
#define flexidag_openseg_prim_split_37___vas_created_terminate__v0_is_rlz 	0
#define flexidag_openseg_prim_split_37___vas_created_terminate__v0_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_37___vas_created_terminate__v0_byte_offset 	124	/* 0x0000007c */
#define flexidag_openseg_prim_split_37___vas_created_terminate__v0_vbase_byte_offset 	128	/* 0x00000080 */
#define flexidag_openseg_prim_split_37___vas_created_terminate__v0_W 	256
#define flexidag_openseg_prim_split_37___vas_created_terminate__v0_H 	3
#define flexidag_openseg_prim_split_37___vas_created_terminate__v0_D 	1
#define flexidag_openseg_prim_split_37___vas_created_terminate__v0_P 	1
#define flexidag_openseg_prim_split_37___vas_created_terminate__v0_data_num_bytes 	1
#define flexidag_openseg_prim_split_37___vas_created_terminate__v0_denable 	0
#define flexidag_openseg_prim_split_37___vas_created_terminate__v0_dram_format 	0
#define flexidag_openseg_prim_split_37___vas_created_terminate__v0_vp_interleave_mode 	1
#define flexidag_openseg_prim_split_37___vas_created_terminate__v0_tile_width 	1
#define flexidag_openseg_prim_split_37___vas_created_terminate__v0_tile_height 	1
#define flexidag_openseg_prim_split_37___vas_created_terminate__v0_vmem_buffer_num_bytes 	96


#endif /* flexidag_openseg_prim_split_37_H */
