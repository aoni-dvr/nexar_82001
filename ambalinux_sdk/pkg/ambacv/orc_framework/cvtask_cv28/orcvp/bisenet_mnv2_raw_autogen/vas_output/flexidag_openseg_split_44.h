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
#ifndef flexidag_openseg_prim_split_44_H
#define flexidag_openseg_prim_split_44_H

#define vas_gen_header_version                  	7

/* dag name */
#define flexidag_openseg_prim_split_44_DAG_NAME 	"flexidag_openseg_prim_split_44"

/* VDG file info */
#define flexidag_openseg_prim_split_44_vdg_name 	"flexidag_openseg_split_44.vdg"

/* VDG DAG memory info */
#define flexidag_openseg_prim_split_44_byte_offset 	4294967295	/* 0xffffffff */
#define flexidag_openseg_prim_split_44_byte_size 	260	/* 0x00000104 */
#define flexidag_openseg_prim_split_44_preferred_dram_xfer_size 	1

/* flexidag_openseg_prim_split_44 VMEM info */
#define flexidag_openseg_prim_split_44_use_ping_pong_vmem 	0
#define flexidag_openseg_prim_split_44_VMEM_start 	4096	/* 0x00001000 */
#define flexidag_openseg_prim_split_44_VMEM_end 	477856	/* 0x00074aa0 */
#define flexidag_openseg_prim_split_44_image_start 	477856	/* 0x00074aa0 */
#define flexidag_openseg_prim_split_44_image_size 	260	/* 0x00000104 */
#define flexidag_openseg_prim_split_44_dagbin_start 	477856	/* 0x00074aa0 */

/* flexidag_openseg_prim_split_44 DAG info */
#define flexidag_openseg_prim_split_44_estimated_cycles 	500912
#define flexidag_openseg_prim_split_44_ddi_byte_size 	0	/* 0x00000000 */

/*******************************************************************************
 * DAG byte offset info for HMB/SMB/VCB and their fields
 * in DAG 'flexidag_openseg_prim_split_44' in source file 'pre_split28_flexidag_openseg_split_5.vas'
 ******************************************************************************/
/* flexidag_openseg_prim_split_44 HMB_input concat_1_580000128_ */
#define flexidag_openseg_prim_split_44_concat_1_580000128__cnngen_demangled_name 	"concat_1_580000128_"
#define flexidag_openseg_prim_split_44_concat_1_580000128__is_main_input_output 	0
#define flexidag_openseg_prim_split_44_concat_1_580000128__is_constant 	0
#define flexidag_openseg_prim_split_44_concat_1_580000128__has_init_data 	0
#define flexidag_openseg_prim_split_44_concat_1_580000128__is_variable_scalar 	0
#define flexidag_openseg_prim_split_44_concat_1_580000128__ddi_byte_size 	0	/* 0x00000000 */
#define flexidag_openseg_prim_split_44_concat_1_580000128__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define flexidag_openseg_prim_split_44_concat_1_580000128__is_bitvector 	0
#define flexidag_openseg_prim_split_44_concat_1_580000128__is_rlz 	0
#define flexidag_openseg_prim_split_44_concat_1_580000128__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_44_concat_1_580000128__byte_offset 	0	/* 0x00000000 */
#define flexidag_openseg_prim_split_44_concat_1_580000128__vbase_byte_offset 	4	/* 0x00000004 */
#define flexidag_openseg_prim_split_44_concat_1_580000128__dbase_byte_offset 	28	/* 0x0000001c */
#define flexidag_openseg_prim_split_44_concat_1_580000128__dpitchm1_byte_offset 	32	/* 0x00000020 */
#define flexidag_openseg_prim_split_44_concat_1_580000128__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_44_concat_1_580000128__vwidth_minus_one_byte_offset 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_44_concat_1_580000128__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_44_concat_1_580000128__vheight_minus_one_byte_offset 	18	/* 0x00000012 */
#define flexidag_openseg_prim_split_44_concat_1_580000128__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_44_concat_1_580000128__drotate_bit_offset 	198	/* 0x000000c6 */
#define flexidag_openseg_prim_split_44_concat_1_580000128__hflip_bit_offset 	199	/* 0x000000c7 */
#define flexidag_openseg_prim_split_44_concat_1_580000128__vflip_bit_offset 	200	/* 0x000000c8 */
#define flexidag_openseg_prim_split_44_concat_1_580000128__dflip_bit_offset 	201	/* 0x000000c9 */
#define flexidag_openseg_prim_split_44_concat_1_580000128__pflip_bit_offset 	202	/* 0x000000ca */
#define flexidag_openseg_prim_split_44_concat_1_580000128__W 	160
#define flexidag_openseg_prim_split_44_concat_1_580000128__H 	64
#define flexidag_openseg_prim_split_44_concat_1_580000128__D 	342
#define flexidag_openseg_prim_split_44_concat_1_580000128__P 	1
#define flexidag_openseg_prim_split_44_concat_1_580000128__data_num_bytes 	2
#define flexidag_openseg_prim_split_44_concat_1_580000128__denable 	1
#define flexidag_openseg_prim_split_44_concat_1_580000128__dpitch_num_bytes 	64
#define flexidag_openseg_prim_split_44_concat_1_580000128__dram_format 	3
#define flexidag_openseg_prim_split_44_concat_1_580000128__vp_interleave_mode 	2
#define flexidag_openseg_prim_split_44_concat_1_580000128__tile_width 	4
#define flexidag_openseg_prim_split_44_concat_1_580000128__tile_height 	1
#define flexidag_openseg_prim_split_44_concat_1_580000128__vmem_buffer_num_bytes 	109440
#define flexidag_openseg_prim_split_44_concat_1_580000128__dram_size_num_bytes 	7004160
#define flexidag_openseg_prim_split_44_concat_1_580000128__sign 	0
#define flexidag_openseg_prim_split_44_concat_1_580000128__datasize 	1
#define flexidag_openseg_prim_split_44_concat_1_580000128__expoffset 	13
#define flexidag_openseg_prim_split_44_concat_1_580000128__expbits 	0
#define flexidag_openseg_prim_split_44_concat_1_580000128__drotate 	0
#define flexidag_openseg_prim_split_44_concat_1_580000128__hflip 	0
#define flexidag_openseg_prim_split_44_concat_1_580000128__vflip 	0
#define flexidag_openseg_prim_split_44_concat_1_580000128__dflip 	0
#define flexidag_openseg_prim_split_44_concat_1_580000128__pflip 	0
#define flexidag_openseg_prim_split_44_concat_1_580000128__trailing_extra_elements 	0

/* flexidag_openseg_prim_split_44 HMB_input concat_1 */
#define flexidag_openseg_prim_split_44_concat_1_cnngen_demangled_name 	"concat_1"
#define flexidag_openseg_prim_split_44_concat_1_is_main_input_output 	0
#define flexidag_openseg_prim_split_44_concat_1_is_constant 	0
#define flexidag_openseg_prim_split_44_concat_1_has_init_data 	0
#define flexidag_openseg_prim_split_44_concat_1_is_variable_scalar 	0
#define flexidag_openseg_prim_split_44_concat_1_ddi_byte_size 	0	/* 0x00000000 */
#define flexidag_openseg_prim_split_44_concat_1_ddi_byte_offset 	4294967295	/* 0xffffffff */
#define flexidag_openseg_prim_split_44_concat_1_is_bitvector 	0
#define flexidag_openseg_prim_split_44_concat_1_is_rlz 	0
#define flexidag_openseg_prim_split_44_concat_1_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_44_concat_1_byte_offset 	36	/* 0x00000024 */
#define flexidag_openseg_prim_split_44_concat_1_vbase_byte_offset 	40	/* 0x00000028 */
#define flexidag_openseg_prim_split_44_concat_1_dbase_byte_offset 	64	/* 0x00000040 */
#define flexidag_openseg_prim_split_44_concat_1_dpitchm1_byte_offset 	68	/* 0x00000044 */
#define flexidag_openseg_prim_split_44_concat_1_dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_44_concat_1_vwidth_minus_one_byte_offset 	52	/* 0x00000034 */
#define flexidag_openseg_prim_split_44_concat_1_vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_44_concat_1_vheight_minus_one_byte_offset 	54	/* 0x00000036 */
#define flexidag_openseg_prim_split_44_concat_1_vheight_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_44_concat_1_drotate_bit_offset 	486	/* 0x000001e6 */
#define flexidag_openseg_prim_split_44_concat_1_hflip_bit_offset 	487	/* 0x000001e7 */
#define flexidag_openseg_prim_split_44_concat_1_vflip_bit_offset 	488	/* 0x000001e8 */
#define flexidag_openseg_prim_split_44_concat_1_dflip_bit_offset 	489	/* 0x000001e9 */
#define flexidag_openseg_prim_split_44_concat_1_pflip_bit_offset 	490	/* 0x000001ea */
#define flexidag_openseg_prim_split_44_concat_1_W 	160
#define flexidag_openseg_prim_split_44_concat_1_H 	64
#define flexidag_openseg_prim_split_44_concat_1_D 	1024
#define flexidag_openseg_prim_split_44_concat_1_P 	1
#define flexidag_openseg_prim_split_44_concat_1_data_num_bytes 	2
#define flexidag_openseg_prim_split_44_concat_1_denable 	1
#define flexidag_openseg_prim_split_44_concat_1_dpitch_num_bytes 	64
#define flexidag_openseg_prim_split_44_concat_1_dram_format 	3
#define flexidag_openseg_prim_split_44_concat_1_vp_interleave_mode 	2
#define flexidag_openseg_prim_split_44_concat_1_tile_width 	1
#define flexidag_openseg_prim_split_44_concat_1_tile_height 	1
#define flexidag_openseg_prim_split_44_concat_1_vmem_buffer_num_bytes 	266240
#define flexidag_openseg_prim_split_44_concat_1_dram_size_num_bytes 	20971520
#define flexidag_openseg_prim_split_44_concat_1_sign 	0
#define flexidag_openseg_prim_split_44_concat_1_datasize 	1
#define flexidag_openseg_prim_split_44_concat_1_expoffset 	13
#define flexidag_openseg_prim_split_44_concat_1_expbits 	0
#define flexidag_openseg_prim_split_44_concat_1_drotate 	0
#define flexidag_openseg_prim_split_44_concat_1_hflip 	0
#define flexidag_openseg_prim_split_44_concat_1_vflip 	0
#define flexidag_openseg_prim_split_44_concat_1_dflip 	0
#define flexidag_openseg_prim_split_44_concat_1_pflip 	0
#define flexidag_openseg_prim_split_44_concat_1_trailing_extra_elements 	0

/* flexidag_openseg_prim_split_44 HMB_output concat_1_580000132_ */
#define flexidag_openseg_prim_split_44_concat_1_580000132__cnngen_demangled_name 	"concat_1_580000132_"
#define flexidag_openseg_prim_split_44_concat_1_580000132__is_main_input_output 	0
#define flexidag_openseg_prim_split_44_concat_1_580000132__is_bitvector 	0
#define flexidag_openseg_prim_split_44_concat_1_580000132__is_rlz 	0
#define flexidag_openseg_prim_split_44_concat_1_580000132__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_44_concat_1_580000132__byte_offset 	92	/* 0x0000005c */
#define flexidag_openseg_prim_split_44_concat_1_580000132__vbase_byte_offset 	96	/* 0x00000060 */
#define flexidag_openseg_prim_split_44_concat_1_580000132__dbase_byte_offset 	120	/* 0x00000078 */
#define flexidag_openseg_prim_split_44_concat_1_580000132__dpitchm1_byte_offset 	124	/* 0x0000007c */
#define flexidag_openseg_prim_split_44_concat_1_580000132__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_44_concat_1_580000132__vwidth_minus_one_byte_offset 	108	/* 0x0000006c */
#define flexidag_openseg_prim_split_44_concat_1_580000132__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_44_concat_1_580000132__vheight_minus_one_byte_offset 	110	/* 0x0000006e */
#define flexidag_openseg_prim_split_44_concat_1_580000132__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_44_concat_1_580000132__drotate_bit_offset 	934	/* 0x000003a6 */
#define flexidag_openseg_prim_split_44_concat_1_580000132__hflip_bit_offset 	935	/* 0x000003a7 */
#define flexidag_openseg_prim_split_44_concat_1_580000132__vflip_bit_offset 	936	/* 0x000003a8 */
#define flexidag_openseg_prim_split_44_concat_1_580000132__dflip_bit_offset 	937	/* 0x000003a9 */
#define flexidag_openseg_prim_split_44_concat_1_580000132__pflip_bit_offset 	938	/* 0x000003aa */
#define flexidag_openseg_prim_split_44_concat_1_580000132__W 	18
#define flexidag_openseg_prim_split_44_concat_1_580000132__H 	64
#define flexidag_openseg_prim_split_44_concat_1_580000132__D 	342
#define flexidag_openseg_prim_split_44_concat_1_580000132__P 	1
#define flexidag_openseg_prim_split_44_concat_1_580000132__data_num_bytes 	2
#define flexidag_openseg_prim_split_44_concat_1_580000132__denable 	1
#define flexidag_openseg_prim_split_44_concat_1_580000132__dpitch_num_bytes 	64
#define flexidag_openseg_prim_split_44_concat_1_580000132__dram_format 	3
#define flexidag_openseg_prim_split_44_concat_1_580000132__vp_interleave_mode 	2
#define flexidag_openseg_prim_split_44_concat_1_580000132__tile_width 	4
#define flexidag_openseg_prim_split_44_concat_1_580000132__tile_height 	1
#define flexidag_openseg_prim_split_44_concat_1_580000132__vmem_buffer_num_bytes 	10944
#define flexidag_openseg_prim_split_44_concat_1_580000132__dram_size_num_bytes 	1050624
#define flexidag_openseg_prim_split_44_concat_1_580000132__sign 	0
#define flexidag_openseg_prim_split_44_concat_1_580000132__datasize 	1
#define flexidag_openseg_prim_split_44_concat_1_580000132__expoffset 	13
#define flexidag_openseg_prim_split_44_concat_1_580000132__expbits 	0
#define flexidag_openseg_prim_split_44_concat_1_580000132__drotate 	0
#define flexidag_openseg_prim_split_44_concat_1_580000132__hflip 	0
#define flexidag_openseg_prim_split_44_concat_1_580000132__vflip 	0
#define flexidag_openseg_prim_split_44_concat_1_580000132__dflip 	0
#define flexidag_openseg_prim_split_44_concat_1_580000132__pflip 	0
#define flexidag_openseg_prim_split_44_concat_1_580000132__trailing_extra_elements 	0

/* flexidag_openseg_prim_split_44 HMB_output concat_1_580000133_ */
#define flexidag_openseg_prim_split_44_concat_1_580000133__cnngen_demangled_name 	"concat_1_580000133_"
#define flexidag_openseg_prim_split_44_concat_1_580000133__is_main_input_output 	0
#define flexidag_openseg_prim_split_44_concat_1_580000133__is_bitvector 	0
#define flexidag_openseg_prim_split_44_concat_1_580000133__is_rlz 	0
#define flexidag_openseg_prim_split_44_concat_1_580000133__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_44_concat_1_580000133__byte_offset 	144	/* 0x00000090 */
#define flexidag_openseg_prim_split_44_concat_1_580000133__vbase_byte_offset 	148	/* 0x00000094 */
#define flexidag_openseg_prim_split_44_concat_1_580000133__dbase_byte_offset 	172	/* 0x000000ac */
#define flexidag_openseg_prim_split_44_concat_1_580000133__dpitchm1_byte_offset 	176	/* 0x000000b0 */
#define flexidag_openseg_prim_split_44_concat_1_580000133__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_44_concat_1_580000133__vwidth_minus_one_byte_offset 	160	/* 0x000000a0 */
#define flexidag_openseg_prim_split_44_concat_1_580000133__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_44_concat_1_580000133__vheight_minus_one_byte_offset 	162	/* 0x000000a2 */
#define flexidag_openseg_prim_split_44_concat_1_580000133__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_44_concat_1_580000133__drotate_bit_offset 	1350	/* 0x00000546 */
#define flexidag_openseg_prim_split_44_concat_1_580000133__hflip_bit_offset 	1351	/* 0x00000547 */
#define flexidag_openseg_prim_split_44_concat_1_580000133__vflip_bit_offset 	1352	/* 0x00000548 */
#define flexidag_openseg_prim_split_44_concat_1_580000133__dflip_bit_offset 	1353	/* 0x00000549 */
#define flexidag_openseg_prim_split_44_concat_1_580000133__pflip_bit_offset 	1354	/* 0x0000054a */
#define flexidag_openseg_prim_split_44_concat_1_580000133__W 	160
#define flexidag_openseg_prim_split_44_concat_1_580000133__H 	64
#define flexidag_openseg_prim_split_44_concat_1_580000133__D 	340
#define flexidag_openseg_prim_split_44_concat_1_580000133__P 	1
#define flexidag_openseg_prim_split_44_concat_1_580000133__data_num_bytes 	2
#define flexidag_openseg_prim_split_44_concat_1_580000133__denable 	1
#define flexidag_openseg_prim_split_44_concat_1_580000133__dpitch_num_bytes 	64
#define flexidag_openseg_prim_split_44_concat_1_580000133__dram_format 	3
#define flexidag_openseg_prim_split_44_concat_1_580000133__vp_interleave_mode 	2
#define flexidag_openseg_prim_split_44_concat_1_580000133__tile_width 	4
#define flexidag_openseg_prim_split_44_concat_1_580000133__tile_height 	1
#define flexidag_openseg_prim_split_44_concat_1_580000133__vmem_buffer_num_bytes 	87040
#define flexidag_openseg_prim_split_44_concat_1_580000133__dram_size_num_bytes 	6963200
#define flexidag_openseg_prim_split_44_concat_1_580000133__sign 	0
#define flexidag_openseg_prim_split_44_concat_1_580000133__datasize 	1
#define flexidag_openseg_prim_split_44_concat_1_580000133__expoffset 	13
#define flexidag_openseg_prim_split_44_concat_1_580000133__expbits 	0
#define flexidag_openseg_prim_split_44_concat_1_580000133__drotate 	0
#define flexidag_openseg_prim_split_44_concat_1_580000133__hflip 	0
#define flexidag_openseg_prim_split_44_concat_1_580000133__vflip 	0
#define flexidag_openseg_prim_split_44_concat_1_580000133__dflip 	0
#define flexidag_openseg_prim_split_44_concat_1_580000133__pflip 	0
#define flexidag_openseg_prim_split_44_concat_1_580000133__trailing_extra_elements 	0

/* flexidag_openseg_prim_split_44 SMB_output __vas_created_terminate__v0 */
#define flexidag_openseg_prim_split_44___vas_created_terminate__v0_cnngen_demangled_name 	"__vas_created_terminate__v0"
#define flexidag_openseg_prim_split_44___vas_created_terminate__v0_is_main_input_output 	0
#define flexidag_openseg_prim_split_44___vas_created_terminate__v0_is_bitvector 	1
#define flexidag_openseg_prim_split_44___vas_created_terminate__v0_is_rlz 	0
#define flexidag_openseg_prim_split_44___vas_created_terminate__v0_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_44___vas_created_terminate__v0_byte_offset 	184	/* 0x000000b8 */
#define flexidag_openseg_prim_split_44___vas_created_terminate__v0_vbase_byte_offset 	188	/* 0x000000bc */
#define flexidag_openseg_prim_split_44___vas_created_terminate__v0_W 	256
#define flexidag_openseg_prim_split_44___vas_created_terminate__v0_H 	3
#define flexidag_openseg_prim_split_44___vas_created_terminate__v0_D 	1
#define flexidag_openseg_prim_split_44___vas_created_terminate__v0_P 	1
#define flexidag_openseg_prim_split_44___vas_created_terminate__v0_data_num_bytes 	1
#define flexidag_openseg_prim_split_44___vas_created_terminate__v0_denable 	0
#define flexidag_openseg_prim_split_44___vas_created_terminate__v0_dram_format 	0
#define flexidag_openseg_prim_split_44___vas_created_terminate__v0_vp_interleave_mode 	1
#define flexidag_openseg_prim_split_44___vas_created_terminate__v0_tile_width 	1
#define flexidag_openseg_prim_split_44___vas_created_terminate__v0_tile_height 	1
#define flexidag_openseg_prim_split_44___vas_created_terminate__v0_vmem_buffer_num_bytes 	96


#endif /* flexidag_openseg_prim_split_44_H */
