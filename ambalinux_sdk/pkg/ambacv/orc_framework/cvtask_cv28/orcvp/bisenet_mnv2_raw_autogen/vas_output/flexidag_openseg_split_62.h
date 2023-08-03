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
#ifndef flexidag_openseg_prim_split_62_H
#define flexidag_openseg_prim_split_62_H

#define vas_gen_header_version                  	7

/* dag name */
#define flexidag_openseg_prim_split_62_DAG_NAME 	"flexidag_openseg_prim_split_62"

/* VDG file info */
#define flexidag_openseg_prim_split_62_vdg_name 	"flexidag_openseg_split_62.vdg"

/* VDG DAG memory info */
#define flexidag_openseg_prim_split_62_byte_offset 	4294967295	/* 0xffffffff */
#define flexidag_openseg_prim_split_62_byte_size 	260	/* 0x00000104 */
#define flexidag_openseg_prim_split_62_preferred_dram_xfer_size 	1

/* flexidag_openseg_prim_split_62 VMEM info */
#define flexidag_openseg_prim_split_62_use_ping_pong_vmem 	0
#define flexidag_openseg_prim_split_62_VMEM_start 	4096	/* 0x00001000 */
#define flexidag_openseg_prim_split_62_VMEM_end 	477856	/* 0x00074aa0 */
#define flexidag_openseg_prim_split_62_image_start 	477856	/* 0x00074aa0 */
#define flexidag_openseg_prim_split_62_image_size 	260	/* 0x00000104 */
#define flexidag_openseg_prim_split_62_dagbin_start 	477856	/* 0x00074aa0 */

/* flexidag_openseg_prim_split_62 DAG info */
#define flexidag_openseg_prim_split_62_estimated_cycles 	500912
#define flexidag_openseg_prim_split_62_ddi_byte_size 	0	/* 0x00000000 */

/*******************************************************************************
 * DAG byte offset info for HMB/SMB/VCB and their fields
 * in DAG 'flexidag_openseg_prim_split_62' in source file 'pre_split28_flexidag_openseg_split_23.vas'
 ******************************************************************************/
/* flexidag_openseg_prim_split_62 HMB_input concat_1_580000236_ */
#define flexidag_openseg_prim_split_62_concat_1_580000236__cnngen_demangled_name 	"concat_1_580000236_"
#define flexidag_openseg_prim_split_62_concat_1_580000236__is_main_input_output 	0
#define flexidag_openseg_prim_split_62_concat_1_580000236__is_constant 	0
#define flexidag_openseg_prim_split_62_concat_1_580000236__has_init_data 	0
#define flexidag_openseg_prim_split_62_concat_1_580000236__is_variable_scalar 	0
#define flexidag_openseg_prim_split_62_concat_1_580000236__ddi_byte_size 	0	/* 0x00000000 */
#define flexidag_openseg_prim_split_62_concat_1_580000236__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define flexidag_openseg_prim_split_62_concat_1_580000236__is_bitvector 	0
#define flexidag_openseg_prim_split_62_concat_1_580000236__is_rlz 	0
#define flexidag_openseg_prim_split_62_concat_1_580000236__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_62_concat_1_580000236__byte_offset 	0	/* 0x00000000 */
#define flexidag_openseg_prim_split_62_concat_1_580000236__vbase_byte_offset 	4	/* 0x00000004 */
#define flexidag_openseg_prim_split_62_concat_1_580000236__dbase_byte_offset 	28	/* 0x0000001c */
#define flexidag_openseg_prim_split_62_concat_1_580000236__dpitchm1_byte_offset 	32	/* 0x00000020 */
#define flexidag_openseg_prim_split_62_concat_1_580000236__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_62_concat_1_580000236__vwidth_minus_one_byte_offset 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_62_concat_1_580000236__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_62_concat_1_580000236__vheight_minus_one_byte_offset 	18	/* 0x00000012 */
#define flexidag_openseg_prim_split_62_concat_1_580000236__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_62_concat_1_580000236__drotate_bit_offset 	198	/* 0x000000c6 */
#define flexidag_openseg_prim_split_62_concat_1_580000236__hflip_bit_offset 	199	/* 0x000000c7 */
#define flexidag_openseg_prim_split_62_concat_1_580000236__vflip_bit_offset 	200	/* 0x000000c8 */
#define flexidag_openseg_prim_split_62_concat_1_580000236__dflip_bit_offset 	201	/* 0x000000c9 */
#define flexidag_openseg_prim_split_62_concat_1_580000236__pflip_bit_offset 	202	/* 0x000000ca */
#define flexidag_openseg_prim_split_62_concat_1_580000236__W 	160
#define flexidag_openseg_prim_split_62_concat_1_580000236__H 	64
#define flexidag_openseg_prim_split_62_concat_1_580000236__D 	342
#define flexidag_openseg_prim_split_62_concat_1_580000236__P 	1
#define flexidag_openseg_prim_split_62_concat_1_580000236__data_num_bytes 	2
#define flexidag_openseg_prim_split_62_concat_1_580000236__denable 	1
#define flexidag_openseg_prim_split_62_concat_1_580000236__dpitch_num_bytes 	64
#define flexidag_openseg_prim_split_62_concat_1_580000236__dram_format 	3
#define flexidag_openseg_prim_split_62_concat_1_580000236__vp_interleave_mode 	2
#define flexidag_openseg_prim_split_62_concat_1_580000236__tile_width 	4
#define flexidag_openseg_prim_split_62_concat_1_580000236__tile_height 	1
#define flexidag_openseg_prim_split_62_concat_1_580000236__vmem_buffer_num_bytes 	109440
#define flexidag_openseg_prim_split_62_concat_1_580000236__dram_size_num_bytes 	7004160
#define flexidag_openseg_prim_split_62_concat_1_580000236__sign 	0
#define flexidag_openseg_prim_split_62_concat_1_580000236__datasize 	1
#define flexidag_openseg_prim_split_62_concat_1_580000236__expoffset 	13
#define flexidag_openseg_prim_split_62_concat_1_580000236__expbits 	0
#define flexidag_openseg_prim_split_62_concat_1_580000236__drotate 	0
#define flexidag_openseg_prim_split_62_concat_1_580000236__hflip 	0
#define flexidag_openseg_prim_split_62_concat_1_580000236__vflip 	0
#define flexidag_openseg_prim_split_62_concat_1_580000236__dflip 	0
#define flexidag_openseg_prim_split_62_concat_1_580000236__pflip 	0
#define flexidag_openseg_prim_split_62_concat_1_580000236__trailing_extra_elements 	0

/* flexidag_openseg_prim_split_62 HMB_input concat_1 */
#define flexidag_openseg_prim_split_62_concat_1_cnngen_demangled_name 	"concat_1"
#define flexidag_openseg_prim_split_62_concat_1_is_main_input_output 	0
#define flexidag_openseg_prim_split_62_concat_1_is_constant 	0
#define flexidag_openseg_prim_split_62_concat_1_has_init_data 	0
#define flexidag_openseg_prim_split_62_concat_1_is_variable_scalar 	0
#define flexidag_openseg_prim_split_62_concat_1_ddi_byte_size 	0	/* 0x00000000 */
#define flexidag_openseg_prim_split_62_concat_1_ddi_byte_offset 	4294967295	/* 0xffffffff */
#define flexidag_openseg_prim_split_62_concat_1_is_bitvector 	0
#define flexidag_openseg_prim_split_62_concat_1_is_rlz 	0
#define flexidag_openseg_prim_split_62_concat_1_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_62_concat_1_byte_offset 	36	/* 0x00000024 */
#define flexidag_openseg_prim_split_62_concat_1_vbase_byte_offset 	40	/* 0x00000028 */
#define flexidag_openseg_prim_split_62_concat_1_dbase_byte_offset 	64	/* 0x00000040 */
#define flexidag_openseg_prim_split_62_concat_1_dpitchm1_byte_offset 	68	/* 0x00000044 */
#define flexidag_openseg_prim_split_62_concat_1_dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_62_concat_1_vwidth_minus_one_byte_offset 	52	/* 0x00000034 */
#define flexidag_openseg_prim_split_62_concat_1_vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_62_concat_1_vheight_minus_one_byte_offset 	54	/* 0x00000036 */
#define flexidag_openseg_prim_split_62_concat_1_vheight_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_62_concat_1_drotate_bit_offset 	486	/* 0x000001e6 */
#define flexidag_openseg_prim_split_62_concat_1_hflip_bit_offset 	487	/* 0x000001e7 */
#define flexidag_openseg_prim_split_62_concat_1_vflip_bit_offset 	488	/* 0x000001e8 */
#define flexidag_openseg_prim_split_62_concat_1_dflip_bit_offset 	489	/* 0x000001e9 */
#define flexidag_openseg_prim_split_62_concat_1_pflip_bit_offset 	490	/* 0x000001ea */
#define flexidag_openseg_prim_split_62_concat_1_W 	160
#define flexidag_openseg_prim_split_62_concat_1_H 	64
#define flexidag_openseg_prim_split_62_concat_1_D 	1024
#define flexidag_openseg_prim_split_62_concat_1_P 	1
#define flexidag_openseg_prim_split_62_concat_1_data_num_bytes 	2
#define flexidag_openseg_prim_split_62_concat_1_denable 	1
#define flexidag_openseg_prim_split_62_concat_1_dpitch_num_bytes 	64
#define flexidag_openseg_prim_split_62_concat_1_dram_format 	3
#define flexidag_openseg_prim_split_62_concat_1_vp_interleave_mode 	2
#define flexidag_openseg_prim_split_62_concat_1_tile_width 	1
#define flexidag_openseg_prim_split_62_concat_1_tile_height 	1
#define flexidag_openseg_prim_split_62_concat_1_vmem_buffer_num_bytes 	266240
#define flexidag_openseg_prim_split_62_concat_1_dram_size_num_bytes 	20971520
#define flexidag_openseg_prim_split_62_concat_1_sign 	0
#define flexidag_openseg_prim_split_62_concat_1_datasize 	1
#define flexidag_openseg_prim_split_62_concat_1_expoffset 	13
#define flexidag_openseg_prim_split_62_concat_1_expbits 	0
#define flexidag_openseg_prim_split_62_concat_1_drotate 	0
#define flexidag_openseg_prim_split_62_concat_1_hflip 	0
#define flexidag_openseg_prim_split_62_concat_1_vflip 	0
#define flexidag_openseg_prim_split_62_concat_1_dflip 	0
#define flexidag_openseg_prim_split_62_concat_1_pflip 	0
#define flexidag_openseg_prim_split_62_concat_1_trailing_extra_elements 	0

/* flexidag_openseg_prim_split_62 HMB_output concat_1_580000240_ */
#define flexidag_openseg_prim_split_62_concat_1_580000240__cnngen_demangled_name 	"concat_1_580000240_"
#define flexidag_openseg_prim_split_62_concat_1_580000240__is_main_input_output 	0
#define flexidag_openseg_prim_split_62_concat_1_580000240__is_bitvector 	0
#define flexidag_openseg_prim_split_62_concat_1_580000240__is_rlz 	0
#define flexidag_openseg_prim_split_62_concat_1_580000240__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_62_concat_1_580000240__byte_offset 	92	/* 0x0000005c */
#define flexidag_openseg_prim_split_62_concat_1_580000240__vbase_byte_offset 	96	/* 0x00000060 */
#define flexidag_openseg_prim_split_62_concat_1_580000240__dbase_byte_offset 	120	/* 0x00000078 */
#define flexidag_openseg_prim_split_62_concat_1_580000240__dpitchm1_byte_offset 	124	/* 0x0000007c */
#define flexidag_openseg_prim_split_62_concat_1_580000240__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_62_concat_1_580000240__vwidth_minus_one_byte_offset 	108	/* 0x0000006c */
#define flexidag_openseg_prim_split_62_concat_1_580000240__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_62_concat_1_580000240__vheight_minus_one_byte_offset 	110	/* 0x0000006e */
#define flexidag_openseg_prim_split_62_concat_1_580000240__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_62_concat_1_580000240__drotate_bit_offset 	934	/* 0x000003a6 */
#define flexidag_openseg_prim_split_62_concat_1_580000240__hflip_bit_offset 	935	/* 0x000003a7 */
#define flexidag_openseg_prim_split_62_concat_1_580000240__vflip_bit_offset 	936	/* 0x000003a8 */
#define flexidag_openseg_prim_split_62_concat_1_580000240__dflip_bit_offset 	937	/* 0x000003a9 */
#define flexidag_openseg_prim_split_62_concat_1_580000240__pflip_bit_offset 	938	/* 0x000003aa */
#define flexidag_openseg_prim_split_62_concat_1_580000240__W 	18
#define flexidag_openseg_prim_split_62_concat_1_580000240__H 	64
#define flexidag_openseg_prim_split_62_concat_1_580000240__D 	342
#define flexidag_openseg_prim_split_62_concat_1_580000240__P 	1
#define flexidag_openseg_prim_split_62_concat_1_580000240__data_num_bytes 	2
#define flexidag_openseg_prim_split_62_concat_1_580000240__denable 	1
#define flexidag_openseg_prim_split_62_concat_1_580000240__dpitch_num_bytes 	64
#define flexidag_openseg_prim_split_62_concat_1_580000240__dram_format 	3
#define flexidag_openseg_prim_split_62_concat_1_580000240__vp_interleave_mode 	2
#define flexidag_openseg_prim_split_62_concat_1_580000240__tile_width 	4
#define flexidag_openseg_prim_split_62_concat_1_580000240__tile_height 	1
#define flexidag_openseg_prim_split_62_concat_1_580000240__vmem_buffer_num_bytes 	10944
#define flexidag_openseg_prim_split_62_concat_1_580000240__dram_size_num_bytes 	1050624
#define flexidag_openseg_prim_split_62_concat_1_580000240__sign 	0
#define flexidag_openseg_prim_split_62_concat_1_580000240__datasize 	1
#define flexidag_openseg_prim_split_62_concat_1_580000240__expoffset 	13
#define flexidag_openseg_prim_split_62_concat_1_580000240__expbits 	0
#define flexidag_openseg_prim_split_62_concat_1_580000240__drotate 	0
#define flexidag_openseg_prim_split_62_concat_1_580000240__hflip 	0
#define flexidag_openseg_prim_split_62_concat_1_580000240__vflip 	0
#define flexidag_openseg_prim_split_62_concat_1_580000240__dflip 	0
#define flexidag_openseg_prim_split_62_concat_1_580000240__pflip 	0
#define flexidag_openseg_prim_split_62_concat_1_580000240__trailing_extra_elements 	0

/* flexidag_openseg_prim_split_62 HMB_output concat_1_580000241_ */
#define flexidag_openseg_prim_split_62_concat_1_580000241__cnngen_demangled_name 	"concat_1_580000241_"
#define flexidag_openseg_prim_split_62_concat_1_580000241__is_main_input_output 	0
#define flexidag_openseg_prim_split_62_concat_1_580000241__is_bitvector 	0
#define flexidag_openseg_prim_split_62_concat_1_580000241__is_rlz 	0
#define flexidag_openseg_prim_split_62_concat_1_580000241__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_62_concat_1_580000241__byte_offset 	144	/* 0x00000090 */
#define flexidag_openseg_prim_split_62_concat_1_580000241__vbase_byte_offset 	148	/* 0x00000094 */
#define flexidag_openseg_prim_split_62_concat_1_580000241__dbase_byte_offset 	172	/* 0x000000ac */
#define flexidag_openseg_prim_split_62_concat_1_580000241__dpitchm1_byte_offset 	176	/* 0x000000b0 */
#define flexidag_openseg_prim_split_62_concat_1_580000241__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_62_concat_1_580000241__vwidth_minus_one_byte_offset 	160	/* 0x000000a0 */
#define flexidag_openseg_prim_split_62_concat_1_580000241__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_62_concat_1_580000241__vheight_minus_one_byte_offset 	162	/* 0x000000a2 */
#define flexidag_openseg_prim_split_62_concat_1_580000241__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_62_concat_1_580000241__drotate_bit_offset 	1350	/* 0x00000546 */
#define flexidag_openseg_prim_split_62_concat_1_580000241__hflip_bit_offset 	1351	/* 0x00000547 */
#define flexidag_openseg_prim_split_62_concat_1_580000241__vflip_bit_offset 	1352	/* 0x00000548 */
#define flexidag_openseg_prim_split_62_concat_1_580000241__dflip_bit_offset 	1353	/* 0x00000549 */
#define flexidag_openseg_prim_split_62_concat_1_580000241__pflip_bit_offset 	1354	/* 0x0000054a */
#define flexidag_openseg_prim_split_62_concat_1_580000241__W 	160
#define flexidag_openseg_prim_split_62_concat_1_580000241__H 	64
#define flexidag_openseg_prim_split_62_concat_1_580000241__D 	340
#define flexidag_openseg_prim_split_62_concat_1_580000241__P 	1
#define flexidag_openseg_prim_split_62_concat_1_580000241__data_num_bytes 	2
#define flexidag_openseg_prim_split_62_concat_1_580000241__denable 	1
#define flexidag_openseg_prim_split_62_concat_1_580000241__dpitch_num_bytes 	64
#define flexidag_openseg_prim_split_62_concat_1_580000241__dram_format 	3
#define flexidag_openseg_prim_split_62_concat_1_580000241__vp_interleave_mode 	2
#define flexidag_openseg_prim_split_62_concat_1_580000241__tile_width 	4
#define flexidag_openseg_prim_split_62_concat_1_580000241__tile_height 	1
#define flexidag_openseg_prim_split_62_concat_1_580000241__vmem_buffer_num_bytes 	87040
#define flexidag_openseg_prim_split_62_concat_1_580000241__dram_size_num_bytes 	6963200
#define flexidag_openseg_prim_split_62_concat_1_580000241__sign 	0
#define flexidag_openseg_prim_split_62_concat_1_580000241__datasize 	1
#define flexidag_openseg_prim_split_62_concat_1_580000241__expoffset 	13
#define flexidag_openseg_prim_split_62_concat_1_580000241__expbits 	0
#define flexidag_openseg_prim_split_62_concat_1_580000241__drotate 	0
#define flexidag_openseg_prim_split_62_concat_1_580000241__hflip 	0
#define flexidag_openseg_prim_split_62_concat_1_580000241__vflip 	0
#define flexidag_openseg_prim_split_62_concat_1_580000241__dflip 	0
#define flexidag_openseg_prim_split_62_concat_1_580000241__pflip 	0
#define flexidag_openseg_prim_split_62_concat_1_580000241__trailing_extra_elements 	0

/* flexidag_openseg_prim_split_62 SMB_output __vas_created_terminate__v0 */
#define flexidag_openseg_prim_split_62___vas_created_terminate__v0_cnngen_demangled_name 	"__vas_created_terminate__v0"
#define flexidag_openseg_prim_split_62___vas_created_terminate__v0_is_main_input_output 	0
#define flexidag_openseg_prim_split_62___vas_created_terminate__v0_is_bitvector 	1
#define flexidag_openseg_prim_split_62___vas_created_terminate__v0_is_rlz 	0
#define flexidag_openseg_prim_split_62___vas_created_terminate__v0_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_62___vas_created_terminate__v0_byte_offset 	184	/* 0x000000b8 */
#define flexidag_openseg_prim_split_62___vas_created_terminate__v0_vbase_byte_offset 	188	/* 0x000000bc */
#define flexidag_openseg_prim_split_62___vas_created_terminate__v0_W 	256
#define flexidag_openseg_prim_split_62___vas_created_terminate__v0_H 	3
#define flexidag_openseg_prim_split_62___vas_created_terminate__v0_D 	1
#define flexidag_openseg_prim_split_62___vas_created_terminate__v0_P 	1
#define flexidag_openseg_prim_split_62___vas_created_terminate__v0_data_num_bytes 	1
#define flexidag_openseg_prim_split_62___vas_created_terminate__v0_denable 	0
#define flexidag_openseg_prim_split_62___vas_created_terminate__v0_dram_format 	0
#define flexidag_openseg_prim_split_62___vas_created_terminate__v0_vp_interleave_mode 	1
#define flexidag_openseg_prim_split_62___vas_created_terminate__v0_tile_width 	1
#define flexidag_openseg_prim_split_62___vas_created_terminate__v0_tile_height 	1
#define flexidag_openseg_prim_split_62___vas_created_terminate__v0_vmem_buffer_num_bytes 	96


#endif /* flexidag_openseg_prim_split_62_H */
