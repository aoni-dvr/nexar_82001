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
#ifndef flexidag_openseg_prim_split_65_H
#define flexidag_openseg_prim_split_65_H

#define vas_gen_header_version                  	7

/* dag name */
#define flexidag_openseg_prim_split_65_DAG_NAME 	"flexidag_openseg_prim_split_65"

/* VDG file info */
#define flexidag_openseg_prim_split_65_vdg_name 	"flexidag_openseg_split_65.vdg"

/* VDG DAG memory info */
#define flexidag_openseg_prim_split_65_byte_offset 	4294967295	/* 0xffffffff */
#define flexidag_openseg_prim_split_65_byte_size 	260	/* 0x00000104 */
#define flexidag_openseg_prim_split_65_preferred_dram_xfer_size 	1

/* flexidag_openseg_prim_split_65 VMEM info */
#define flexidag_openseg_prim_split_65_use_ping_pong_vmem 	0
#define flexidag_openseg_prim_split_65_VMEM_start 	4096	/* 0x00001000 */
#define flexidag_openseg_prim_split_65_VMEM_end 	477856	/* 0x00074aa0 */
#define flexidag_openseg_prim_split_65_image_start 	477856	/* 0x00074aa0 */
#define flexidag_openseg_prim_split_65_image_size 	260	/* 0x00000104 */
#define flexidag_openseg_prim_split_65_dagbin_start 	477856	/* 0x00074aa0 */

/* flexidag_openseg_prim_split_65 DAG info */
#define flexidag_openseg_prim_split_65_estimated_cycles 	500912
#define flexidag_openseg_prim_split_65_ddi_byte_size 	0	/* 0x00000000 */

/*******************************************************************************
 * DAG byte offset info for HMB/SMB/VCB and their fields
 * in DAG 'flexidag_openseg_prim_split_65' in source file 'pre_split28_flexidag_openseg_split_26.vas'
 ******************************************************************************/
/* flexidag_openseg_prim_split_65 HMB_input concat_1_580000254_ */
#define flexidag_openseg_prim_split_65_concat_1_580000254__cnngen_demangled_name 	"concat_1_580000254_"
#define flexidag_openseg_prim_split_65_concat_1_580000254__is_main_input_output 	0
#define flexidag_openseg_prim_split_65_concat_1_580000254__is_constant 	0
#define flexidag_openseg_prim_split_65_concat_1_580000254__has_init_data 	0
#define flexidag_openseg_prim_split_65_concat_1_580000254__is_variable_scalar 	0
#define flexidag_openseg_prim_split_65_concat_1_580000254__ddi_byte_size 	0	/* 0x00000000 */
#define flexidag_openseg_prim_split_65_concat_1_580000254__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define flexidag_openseg_prim_split_65_concat_1_580000254__is_bitvector 	0
#define flexidag_openseg_prim_split_65_concat_1_580000254__is_rlz 	0
#define flexidag_openseg_prim_split_65_concat_1_580000254__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_65_concat_1_580000254__byte_offset 	0	/* 0x00000000 */
#define flexidag_openseg_prim_split_65_concat_1_580000254__vbase_byte_offset 	4	/* 0x00000004 */
#define flexidag_openseg_prim_split_65_concat_1_580000254__dbase_byte_offset 	28	/* 0x0000001c */
#define flexidag_openseg_prim_split_65_concat_1_580000254__dpitchm1_byte_offset 	32	/* 0x00000020 */
#define flexidag_openseg_prim_split_65_concat_1_580000254__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_65_concat_1_580000254__vwidth_minus_one_byte_offset 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_65_concat_1_580000254__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_65_concat_1_580000254__vheight_minus_one_byte_offset 	18	/* 0x00000012 */
#define flexidag_openseg_prim_split_65_concat_1_580000254__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_65_concat_1_580000254__drotate_bit_offset 	198	/* 0x000000c6 */
#define flexidag_openseg_prim_split_65_concat_1_580000254__hflip_bit_offset 	199	/* 0x000000c7 */
#define flexidag_openseg_prim_split_65_concat_1_580000254__vflip_bit_offset 	200	/* 0x000000c8 */
#define flexidag_openseg_prim_split_65_concat_1_580000254__dflip_bit_offset 	201	/* 0x000000c9 */
#define flexidag_openseg_prim_split_65_concat_1_580000254__pflip_bit_offset 	202	/* 0x000000ca */
#define flexidag_openseg_prim_split_65_concat_1_580000254__W 	160
#define flexidag_openseg_prim_split_65_concat_1_580000254__H 	64
#define flexidag_openseg_prim_split_65_concat_1_580000254__D 	342
#define flexidag_openseg_prim_split_65_concat_1_580000254__P 	1
#define flexidag_openseg_prim_split_65_concat_1_580000254__data_num_bytes 	2
#define flexidag_openseg_prim_split_65_concat_1_580000254__denable 	1
#define flexidag_openseg_prim_split_65_concat_1_580000254__dpitch_num_bytes 	64
#define flexidag_openseg_prim_split_65_concat_1_580000254__dram_format 	3
#define flexidag_openseg_prim_split_65_concat_1_580000254__vp_interleave_mode 	2
#define flexidag_openseg_prim_split_65_concat_1_580000254__tile_width 	4
#define flexidag_openseg_prim_split_65_concat_1_580000254__tile_height 	1
#define flexidag_openseg_prim_split_65_concat_1_580000254__vmem_buffer_num_bytes 	109440
#define flexidag_openseg_prim_split_65_concat_1_580000254__dram_size_num_bytes 	7004160
#define flexidag_openseg_prim_split_65_concat_1_580000254__sign 	0
#define flexidag_openseg_prim_split_65_concat_1_580000254__datasize 	1
#define flexidag_openseg_prim_split_65_concat_1_580000254__expoffset 	13
#define flexidag_openseg_prim_split_65_concat_1_580000254__expbits 	0
#define flexidag_openseg_prim_split_65_concat_1_580000254__drotate 	0
#define flexidag_openseg_prim_split_65_concat_1_580000254__hflip 	0
#define flexidag_openseg_prim_split_65_concat_1_580000254__vflip 	0
#define flexidag_openseg_prim_split_65_concat_1_580000254__dflip 	0
#define flexidag_openseg_prim_split_65_concat_1_580000254__pflip 	0
#define flexidag_openseg_prim_split_65_concat_1_580000254__trailing_extra_elements 	0

/* flexidag_openseg_prim_split_65 HMB_input concat_1 */
#define flexidag_openseg_prim_split_65_concat_1_cnngen_demangled_name 	"concat_1"
#define flexidag_openseg_prim_split_65_concat_1_is_main_input_output 	0
#define flexidag_openseg_prim_split_65_concat_1_is_constant 	0
#define flexidag_openseg_prim_split_65_concat_1_has_init_data 	0
#define flexidag_openseg_prim_split_65_concat_1_is_variable_scalar 	0
#define flexidag_openseg_prim_split_65_concat_1_ddi_byte_size 	0	/* 0x00000000 */
#define flexidag_openseg_prim_split_65_concat_1_ddi_byte_offset 	4294967295	/* 0xffffffff */
#define flexidag_openseg_prim_split_65_concat_1_is_bitvector 	0
#define flexidag_openseg_prim_split_65_concat_1_is_rlz 	0
#define flexidag_openseg_prim_split_65_concat_1_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_65_concat_1_byte_offset 	36	/* 0x00000024 */
#define flexidag_openseg_prim_split_65_concat_1_vbase_byte_offset 	40	/* 0x00000028 */
#define flexidag_openseg_prim_split_65_concat_1_dbase_byte_offset 	64	/* 0x00000040 */
#define flexidag_openseg_prim_split_65_concat_1_dpitchm1_byte_offset 	68	/* 0x00000044 */
#define flexidag_openseg_prim_split_65_concat_1_dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_65_concat_1_vwidth_minus_one_byte_offset 	52	/* 0x00000034 */
#define flexidag_openseg_prim_split_65_concat_1_vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_65_concat_1_vheight_minus_one_byte_offset 	54	/* 0x00000036 */
#define flexidag_openseg_prim_split_65_concat_1_vheight_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_65_concat_1_drotate_bit_offset 	486	/* 0x000001e6 */
#define flexidag_openseg_prim_split_65_concat_1_hflip_bit_offset 	487	/* 0x000001e7 */
#define flexidag_openseg_prim_split_65_concat_1_vflip_bit_offset 	488	/* 0x000001e8 */
#define flexidag_openseg_prim_split_65_concat_1_dflip_bit_offset 	489	/* 0x000001e9 */
#define flexidag_openseg_prim_split_65_concat_1_pflip_bit_offset 	490	/* 0x000001ea */
#define flexidag_openseg_prim_split_65_concat_1_W 	160
#define flexidag_openseg_prim_split_65_concat_1_H 	64
#define flexidag_openseg_prim_split_65_concat_1_D 	1024
#define flexidag_openseg_prim_split_65_concat_1_P 	1
#define flexidag_openseg_prim_split_65_concat_1_data_num_bytes 	2
#define flexidag_openseg_prim_split_65_concat_1_denable 	1
#define flexidag_openseg_prim_split_65_concat_1_dpitch_num_bytes 	64
#define flexidag_openseg_prim_split_65_concat_1_dram_format 	3
#define flexidag_openseg_prim_split_65_concat_1_vp_interleave_mode 	2
#define flexidag_openseg_prim_split_65_concat_1_tile_width 	1
#define flexidag_openseg_prim_split_65_concat_1_tile_height 	1
#define flexidag_openseg_prim_split_65_concat_1_vmem_buffer_num_bytes 	266240
#define flexidag_openseg_prim_split_65_concat_1_dram_size_num_bytes 	20971520
#define flexidag_openseg_prim_split_65_concat_1_sign 	0
#define flexidag_openseg_prim_split_65_concat_1_datasize 	1
#define flexidag_openseg_prim_split_65_concat_1_expoffset 	13
#define flexidag_openseg_prim_split_65_concat_1_expbits 	0
#define flexidag_openseg_prim_split_65_concat_1_drotate 	0
#define flexidag_openseg_prim_split_65_concat_1_hflip 	0
#define flexidag_openseg_prim_split_65_concat_1_vflip 	0
#define flexidag_openseg_prim_split_65_concat_1_dflip 	0
#define flexidag_openseg_prim_split_65_concat_1_pflip 	0
#define flexidag_openseg_prim_split_65_concat_1_trailing_extra_elements 	0

/* flexidag_openseg_prim_split_65 HMB_output concat_1_580000258_ */
#define flexidag_openseg_prim_split_65_concat_1_580000258__cnngen_demangled_name 	"concat_1_580000258_"
#define flexidag_openseg_prim_split_65_concat_1_580000258__is_main_input_output 	0
#define flexidag_openseg_prim_split_65_concat_1_580000258__is_bitvector 	0
#define flexidag_openseg_prim_split_65_concat_1_580000258__is_rlz 	0
#define flexidag_openseg_prim_split_65_concat_1_580000258__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_65_concat_1_580000258__byte_offset 	92	/* 0x0000005c */
#define flexidag_openseg_prim_split_65_concat_1_580000258__vbase_byte_offset 	96	/* 0x00000060 */
#define flexidag_openseg_prim_split_65_concat_1_580000258__dbase_byte_offset 	120	/* 0x00000078 */
#define flexidag_openseg_prim_split_65_concat_1_580000258__dpitchm1_byte_offset 	124	/* 0x0000007c */
#define flexidag_openseg_prim_split_65_concat_1_580000258__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_65_concat_1_580000258__vwidth_minus_one_byte_offset 	108	/* 0x0000006c */
#define flexidag_openseg_prim_split_65_concat_1_580000258__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_65_concat_1_580000258__vheight_minus_one_byte_offset 	110	/* 0x0000006e */
#define flexidag_openseg_prim_split_65_concat_1_580000258__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_65_concat_1_580000258__drotate_bit_offset 	934	/* 0x000003a6 */
#define flexidag_openseg_prim_split_65_concat_1_580000258__hflip_bit_offset 	935	/* 0x000003a7 */
#define flexidag_openseg_prim_split_65_concat_1_580000258__vflip_bit_offset 	936	/* 0x000003a8 */
#define flexidag_openseg_prim_split_65_concat_1_580000258__dflip_bit_offset 	937	/* 0x000003a9 */
#define flexidag_openseg_prim_split_65_concat_1_580000258__pflip_bit_offset 	938	/* 0x000003aa */
#define flexidag_openseg_prim_split_65_concat_1_580000258__W 	17
#define flexidag_openseg_prim_split_65_concat_1_580000258__H 	64
#define flexidag_openseg_prim_split_65_concat_1_580000258__D 	342
#define flexidag_openseg_prim_split_65_concat_1_580000258__P 	1
#define flexidag_openseg_prim_split_65_concat_1_580000258__data_num_bytes 	2
#define flexidag_openseg_prim_split_65_concat_1_580000258__denable 	1
#define flexidag_openseg_prim_split_65_concat_1_580000258__dpitch_num_bytes 	64
#define flexidag_openseg_prim_split_65_concat_1_580000258__dram_format 	3
#define flexidag_openseg_prim_split_65_concat_1_580000258__vp_interleave_mode 	2
#define flexidag_openseg_prim_split_65_concat_1_580000258__tile_width 	4
#define flexidag_openseg_prim_split_65_concat_1_580000258__tile_height 	1
#define flexidag_openseg_prim_split_65_concat_1_580000258__vmem_buffer_num_bytes 	10944
#define flexidag_openseg_prim_split_65_concat_1_580000258__dram_size_num_bytes 	1050624
#define flexidag_openseg_prim_split_65_concat_1_580000258__sign 	0
#define flexidag_openseg_prim_split_65_concat_1_580000258__datasize 	1
#define flexidag_openseg_prim_split_65_concat_1_580000258__expoffset 	13
#define flexidag_openseg_prim_split_65_concat_1_580000258__expbits 	0
#define flexidag_openseg_prim_split_65_concat_1_580000258__drotate 	0
#define flexidag_openseg_prim_split_65_concat_1_580000258__hflip 	0
#define flexidag_openseg_prim_split_65_concat_1_580000258__vflip 	0
#define flexidag_openseg_prim_split_65_concat_1_580000258__dflip 	0
#define flexidag_openseg_prim_split_65_concat_1_580000258__pflip 	0
#define flexidag_openseg_prim_split_65_concat_1_580000258__trailing_extra_elements 	0

/* flexidag_openseg_prim_split_65 HMB_output concat_1_580000259_ */
#define flexidag_openseg_prim_split_65_concat_1_580000259__cnngen_demangled_name 	"concat_1_580000259_"
#define flexidag_openseg_prim_split_65_concat_1_580000259__is_main_input_output 	0
#define flexidag_openseg_prim_split_65_concat_1_580000259__is_bitvector 	0
#define flexidag_openseg_prim_split_65_concat_1_580000259__is_rlz 	0
#define flexidag_openseg_prim_split_65_concat_1_580000259__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_65_concat_1_580000259__byte_offset 	144	/* 0x00000090 */
#define flexidag_openseg_prim_split_65_concat_1_580000259__vbase_byte_offset 	148	/* 0x00000094 */
#define flexidag_openseg_prim_split_65_concat_1_580000259__dbase_byte_offset 	172	/* 0x000000ac */
#define flexidag_openseg_prim_split_65_concat_1_580000259__dpitchm1_byte_offset 	176	/* 0x000000b0 */
#define flexidag_openseg_prim_split_65_concat_1_580000259__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_65_concat_1_580000259__vwidth_minus_one_byte_offset 	160	/* 0x000000a0 */
#define flexidag_openseg_prim_split_65_concat_1_580000259__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_65_concat_1_580000259__vheight_minus_one_byte_offset 	162	/* 0x000000a2 */
#define flexidag_openseg_prim_split_65_concat_1_580000259__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_65_concat_1_580000259__drotate_bit_offset 	1350	/* 0x00000546 */
#define flexidag_openseg_prim_split_65_concat_1_580000259__hflip_bit_offset 	1351	/* 0x00000547 */
#define flexidag_openseg_prim_split_65_concat_1_580000259__vflip_bit_offset 	1352	/* 0x00000548 */
#define flexidag_openseg_prim_split_65_concat_1_580000259__dflip_bit_offset 	1353	/* 0x00000549 */
#define flexidag_openseg_prim_split_65_concat_1_580000259__pflip_bit_offset 	1354	/* 0x0000054a */
#define flexidag_openseg_prim_split_65_concat_1_580000259__W 	160
#define flexidag_openseg_prim_split_65_concat_1_580000259__H 	64
#define flexidag_openseg_prim_split_65_concat_1_580000259__D 	340
#define flexidag_openseg_prim_split_65_concat_1_580000259__P 	1
#define flexidag_openseg_prim_split_65_concat_1_580000259__data_num_bytes 	2
#define flexidag_openseg_prim_split_65_concat_1_580000259__denable 	1
#define flexidag_openseg_prim_split_65_concat_1_580000259__dpitch_num_bytes 	64
#define flexidag_openseg_prim_split_65_concat_1_580000259__dram_format 	3
#define flexidag_openseg_prim_split_65_concat_1_580000259__vp_interleave_mode 	2
#define flexidag_openseg_prim_split_65_concat_1_580000259__tile_width 	4
#define flexidag_openseg_prim_split_65_concat_1_580000259__tile_height 	1
#define flexidag_openseg_prim_split_65_concat_1_580000259__vmem_buffer_num_bytes 	87040
#define flexidag_openseg_prim_split_65_concat_1_580000259__dram_size_num_bytes 	6963200
#define flexidag_openseg_prim_split_65_concat_1_580000259__sign 	0
#define flexidag_openseg_prim_split_65_concat_1_580000259__datasize 	1
#define flexidag_openseg_prim_split_65_concat_1_580000259__expoffset 	13
#define flexidag_openseg_prim_split_65_concat_1_580000259__expbits 	0
#define flexidag_openseg_prim_split_65_concat_1_580000259__drotate 	0
#define flexidag_openseg_prim_split_65_concat_1_580000259__hflip 	0
#define flexidag_openseg_prim_split_65_concat_1_580000259__vflip 	0
#define flexidag_openseg_prim_split_65_concat_1_580000259__dflip 	0
#define flexidag_openseg_prim_split_65_concat_1_580000259__pflip 	0
#define flexidag_openseg_prim_split_65_concat_1_580000259__trailing_extra_elements 	0

/* flexidag_openseg_prim_split_65 SMB_output __vas_created_terminate__v0 */
#define flexidag_openseg_prim_split_65___vas_created_terminate__v0_cnngen_demangled_name 	"__vas_created_terminate__v0"
#define flexidag_openseg_prim_split_65___vas_created_terminate__v0_is_main_input_output 	0
#define flexidag_openseg_prim_split_65___vas_created_terminate__v0_is_bitvector 	1
#define flexidag_openseg_prim_split_65___vas_created_terminate__v0_is_rlz 	0
#define flexidag_openseg_prim_split_65___vas_created_terminate__v0_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_65___vas_created_terminate__v0_byte_offset 	184	/* 0x000000b8 */
#define flexidag_openseg_prim_split_65___vas_created_terminate__v0_vbase_byte_offset 	188	/* 0x000000bc */
#define flexidag_openseg_prim_split_65___vas_created_terminate__v0_W 	256
#define flexidag_openseg_prim_split_65___vas_created_terminate__v0_H 	3
#define flexidag_openseg_prim_split_65___vas_created_terminate__v0_D 	1
#define flexidag_openseg_prim_split_65___vas_created_terminate__v0_P 	1
#define flexidag_openseg_prim_split_65___vas_created_terminate__v0_data_num_bytes 	1
#define flexidag_openseg_prim_split_65___vas_created_terminate__v0_denable 	0
#define flexidag_openseg_prim_split_65___vas_created_terminate__v0_dram_format 	0
#define flexidag_openseg_prim_split_65___vas_created_terminate__v0_vp_interleave_mode 	1
#define flexidag_openseg_prim_split_65___vas_created_terminate__v0_tile_width 	1
#define flexidag_openseg_prim_split_65___vas_created_terminate__v0_tile_height 	1
#define flexidag_openseg_prim_split_65___vas_created_terminate__v0_vmem_buffer_num_bytes 	96


#endif /* flexidag_openseg_prim_split_65_H */
