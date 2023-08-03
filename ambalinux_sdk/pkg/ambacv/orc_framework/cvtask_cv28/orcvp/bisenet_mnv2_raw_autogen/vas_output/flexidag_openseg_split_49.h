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
#ifndef flexidag_openseg_prim_split_49_H
#define flexidag_openseg_prim_split_49_H

#define vas_gen_header_version                  	7

/* dag name */
#define flexidag_openseg_prim_split_49_DAG_NAME 	"flexidag_openseg_prim_split_49"

/* VDG file info */
#define flexidag_openseg_prim_split_49_vdg_name 	"flexidag_openseg_split_49.vdg"

/* VDG DAG memory info */
#define flexidag_openseg_prim_split_49_byte_offset 	4294967295	/* 0xffffffff */
#define flexidag_openseg_prim_split_49_byte_size 	264	/* 0x00000108 */
#define flexidag_openseg_prim_split_49_preferred_dram_xfer_size 	1

/* flexidag_openseg_prim_split_49 VMEM info */
#define flexidag_openseg_prim_split_49_use_ping_pong_vmem 	0
#define flexidag_openseg_prim_split_49_VMEM_start 	4096	/* 0x00001000 */
#define flexidag_openseg_prim_split_49_VMEM_end 	500512	/* 0x0007a320 */
#define flexidag_openseg_prim_split_49_image_start 	500512	/* 0x0007a320 */
#define flexidag_openseg_prim_split_49_image_size 	264	/* 0x00000108 */
#define flexidag_openseg_prim_split_49_dagbin_start 	500512	/* 0x0007a320 */

/* flexidag_openseg_prim_split_49 DAG info */
#define flexidag_openseg_prim_split_49_estimated_cycles 	941232
#define flexidag_openseg_prim_split_49_ddi_byte_size 	0	/* 0x00000000 */

/*******************************************************************************
 * DAG byte offset info for HMB/SMB/VCB and their fields
 * in DAG 'flexidag_openseg_prim_split_49' in source file 'pre_split28_flexidag_openseg_split_10.vas'
 ******************************************************************************/
/* flexidag_openseg_prim_split_49 HMB_input concat_1 */
#define flexidag_openseg_prim_split_49_concat_1_cnngen_demangled_name 	"concat_1"
#define flexidag_openseg_prim_split_49_concat_1_is_main_input_output 	0
#define flexidag_openseg_prim_split_49_concat_1_is_constant 	0
#define flexidag_openseg_prim_split_49_concat_1_has_init_data 	0
#define flexidag_openseg_prim_split_49_concat_1_is_variable_scalar 	0
#define flexidag_openseg_prim_split_49_concat_1_ddi_byte_size 	0	/* 0x00000000 */
#define flexidag_openseg_prim_split_49_concat_1_ddi_byte_offset 	4294967295	/* 0xffffffff */
#define flexidag_openseg_prim_split_49_concat_1_is_bitvector 	0
#define flexidag_openseg_prim_split_49_concat_1_is_rlz 	0
#define flexidag_openseg_prim_split_49_concat_1_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_49_concat_1_byte_offset 	0	/* 0x00000000 */
#define flexidag_openseg_prim_split_49_concat_1_vbase_byte_offset 	4	/* 0x00000004 */
#define flexidag_openseg_prim_split_49_concat_1_dbase_byte_offset 	28	/* 0x0000001c */
#define flexidag_openseg_prim_split_49_concat_1_dpitchm1_byte_offset 	32	/* 0x00000020 */
#define flexidag_openseg_prim_split_49_concat_1_dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_49_concat_1_vwidth_minus_one_byte_offset 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_49_concat_1_vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_49_concat_1_vheight_minus_one_byte_offset 	18	/* 0x00000012 */
#define flexidag_openseg_prim_split_49_concat_1_vheight_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_49_concat_1_drotate_bit_offset 	198	/* 0x000000c6 */
#define flexidag_openseg_prim_split_49_concat_1_hflip_bit_offset 	199	/* 0x000000c7 */
#define flexidag_openseg_prim_split_49_concat_1_vflip_bit_offset 	200	/* 0x000000c8 */
#define flexidag_openseg_prim_split_49_concat_1_dflip_bit_offset 	201	/* 0x000000c9 */
#define flexidag_openseg_prim_split_49_concat_1_pflip_bit_offset 	202	/* 0x000000ca */
#define flexidag_openseg_prim_split_49_concat_1_W 	160
#define flexidag_openseg_prim_split_49_concat_1_H 	64
#define flexidag_openseg_prim_split_49_concat_1_D 	1024
#define flexidag_openseg_prim_split_49_concat_1_P 	1
#define flexidag_openseg_prim_split_49_concat_1_data_num_bytes 	2
#define flexidag_openseg_prim_split_49_concat_1_denable 	1
#define flexidag_openseg_prim_split_49_concat_1_dpitch_num_bytes 	64
#define flexidag_openseg_prim_split_49_concat_1_dram_format 	3
#define flexidag_openseg_prim_split_49_concat_1_vp_interleave_mode 	2
#define flexidag_openseg_prim_split_49_concat_1_tile_width 	1
#define flexidag_openseg_prim_split_49_concat_1_tile_height 	1
#define flexidag_openseg_prim_split_49_concat_1_vmem_buffer_num_bytes 	135168
#define flexidag_openseg_prim_split_49_concat_1_dram_size_num_bytes 	20971520
#define flexidag_openseg_prim_split_49_concat_1_sign 	0
#define flexidag_openseg_prim_split_49_concat_1_datasize 	1
#define flexidag_openseg_prim_split_49_concat_1_expoffset 	13
#define flexidag_openseg_prim_split_49_concat_1_expbits 	0
#define flexidag_openseg_prim_split_49_concat_1_drotate 	0
#define flexidag_openseg_prim_split_49_concat_1_hflip 	0
#define flexidag_openseg_prim_split_49_concat_1_vflip 	0
#define flexidag_openseg_prim_split_49_concat_1_dflip 	0
#define flexidag_openseg_prim_split_49_concat_1_pflip 	0
#define flexidag_openseg_prim_split_49_concat_1_trailing_extra_elements 	0

/* flexidag_openseg_prim_split_49 VCB concat_1_580000159_ */
#define flexidag_openseg_prim_split_49_concat_1_580000159__cnngen_demangled_name 	"concat_1_580000159_"
#define flexidag_openseg_prim_split_49_concat_1_580000159__is_bitvector 	0
#define flexidag_openseg_prim_split_49_concat_1_580000159__is_rlz 	0
#define flexidag_openseg_prim_split_49_concat_1_580000159__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_49_concat_1_580000159__byte_offset 	56	/* 0x00000038 */
#define flexidag_openseg_prim_split_49_concat_1_580000159__vbase_byte_offset 	64	/* 0x00000040 */
#define flexidag_openseg_prim_split_49_concat_1_580000159__vmem_buffer_num_bytes 	328320

/* flexidag_openseg_prim_split_49 HMB_output concat_1_580000163_ */
#define flexidag_openseg_prim_split_49_concat_1_580000163__cnngen_demangled_name 	"concat_1_580000163_"
#define flexidag_openseg_prim_split_49_concat_1_580000163__is_main_input_output 	0
#define flexidag_openseg_prim_split_49_concat_1_580000163__is_bitvector 	0
#define flexidag_openseg_prim_split_49_concat_1_580000163__is_rlz 	0
#define flexidag_openseg_prim_split_49_concat_1_580000163__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_49_concat_1_580000163__byte_offset 	96	/* 0x00000060 */
#define flexidag_openseg_prim_split_49_concat_1_580000163__vbase_byte_offset 	100	/* 0x00000064 */
#define flexidag_openseg_prim_split_49_concat_1_580000163__dbase_byte_offset 	124	/* 0x0000007c */
#define flexidag_openseg_prim_split_49_concat_1_580000163__dpitchm1_byte_offset 	128	/* 0x00000080 */
#define flexidag_openseg_prim_split_49_concat_1_580000163__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_49_concat_1_580000163__vwidth_minus_one_byte_offset 	112	/* 0x00000070 */
#define flexidag_openseg_prim_split_49_concat_1_580000163__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_49_concat_1_580000163__vheight_minus_one_byte_offset 	114	/* 0x00000072 */
#define flexidag_openseg_prim_split_49_concat_1_580000163__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_49_concat_1_580000163__drotate_bit_offset 	966	/* 0x000003c6 */
#define flexidag_openseg_prim_split_49_concat_1_580000163__hflip_bit_offset 	967	/* 0x000003c7 */
#define flexidag_openseg_prim_split_49_concat_1_580000163__vflip_bit_offset 	968	/* 0x000003c8 */
#define flexidag_openseg_prim_split_49_concat_1_580000163__dflip_bit_offset 	969	/* 0x000003c9 */
#define flexidag_openseg_prim_split_49_concat_1_580000163__pflip_bit_offset 	970	/* 0x000003ca */
#define flexidag_openseg_prim_split_49_concat_1_580000163__W 	18
#define flexidag_openseg_prim_split_49_concat_1_580000163__H 	64
#define flexidag_openseg_prim_split_49_concat_1_580000163__D 	342
#define flexidag_openseg_prim_split_49_concat_1_580000163__P 	1
#define flexidag_openseg_prim_split_49_concat_1_580000163__data_num_bytes 	2
#define flexidag_openseg_prim_split_49_concat_1_580000163__denable 	1
#define flexidag_openseg_prim_split_49_concat_1_580000163__dpitch_num_bytes 	64
#define flexidag_openseg_prim_split_49_concat_1_580000163__dram_format 	3
#define flexidag_openseg_prim_split_49_concat_1_580000163__vp_interleave_mode 	2
#define flexidag_openseg_prim_split_49_concat_1_580000163__tile_width 	4
#define flexidag_openseg_prim_split_49_concat_1_580000163__tile_height 	1
#define flexidag_openseg_prim_split_49_concat_1_580000163__vmem_buffer_num_bytes 	10944
#define flexidag_openseg_prim_split_49_concat_1_580000163__dram_size_num_bytes 	1050624
#define flexidag_openseg_prim_split_49_concat_1_580000163__sign 	0
#define flexidag_openseg_prim_split_49_concat_1_580000163__datasize 	1
#define flexidag_openseg_prim_split_49_concat_1_580000163__expoffset 	13
#define flexidag_openseg_prim_split_49_concat_1_580000163__expbits 	0
#define flexidag_openseg_prim_split_49_concat_1_580000163__drotate 	0
#define flexidag_openseg_prim_split_49_concat_1_580000163__hflip 	0
#define flexidag_openseg_prim_split_49_concat_1_580000163__vflip 	0
#define flexidag_openseg_prim_split_49_concat_1_580000163__dflip 	0
#define flexidag_openseg_prim_split_49_concat_1_580000163__pflip 	0
#define flexidag_openseg_prim_split_49_concat_1_580000163__trailing_extra_elements 	0

/* flexidag_openseg_prim_split_49 HMB_output concat_1_580000164_ */
#define flexidag_openseg_prim_split_49_concat_1_580000164__cnngen_demangled_name 	"concat_1_580000164_"
#define flexidag_openseg_prim_split_49_concat_1_580000164__is_main_input_output 	0
#define flexidag_openseg_prim_split_49_concat_1_580000164__is_bitvector 	0
#define flexidag_openseg_prim_split_49_concat_1_580000164__is_rlz 	0
#define flexidag_openseg_prim_split_49_concat_1_580000164__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_49_concat_1_580000164__byte_offset 	148	/* 0x00000094 */
#define flexidag_openseg_prim_split_49_concat_1_580000164__vbase_byte_offset 	152	/* 0x00000098 */
#define flexidag_openseg_prim_split_49_concat_1_580000164__dbase_byte_offset 	176	/* 0x000000b0 */
#define flexidag_openseg_prim_split_49_concat_1_580000164__dpitchm1_byte_offset 	180	/* 0x000000b4 */
#define flexidag_openseg_prim_split_49_concat_1_580000164__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_49_concat_1_580000164__vwidth_minus_one_byte_offset 	164	/* 0x000000a4 */
#define flexidag_openseg_prim_split_49_concat_1_580000164__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_49_concat_1_580000164__vheight_minus_one_byte_offset 	166	/* 0x000000a6 */
#define flexidag_openseg_prim_split_49_concat_1_580000164__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_49_concat_1_580000164__drotate_bit_offset 	1382	/* 0x00000566 */
#define flexidag_openseg_prim_split_49_concat_1_580000164__hflip_bit_offset 	1383	/* 0x00000567 */
#define flexidag_openseg_prim_split_49_concat_1_580000164__vflip_bit_offset 	1384	/* 0x00000568 */
#define flexidag_openseg_prim_split_49_concat_1_580000164__dflip_bit_offset 	1385	/* 0x00000569 */
#define flexidag_openseg_prim_split_49_concat_1_580000164__pflip_bit_offset 	1386	/* 0x0000056a */
#define flexidag_openseg_prim_split_49_concat_1_580000164__W 	160
#define flexidag_openseg_prim_split_49_concat_1_580000164__H 	64
#define flexidag_openseg_prim_split_49_concat_1_580000164__D 	342
#define flexidag_openseg_prim_split_49_concat_1_580000164__P 	1
#define flexidag_openseg_prim_split_49_concat_1_580000164__data_num_bytes 	2
#define flexidag_openseg_prim_split_49_concat_1_580000164__denable 	1
#define flexidag_openseg_prim_split_49_concat_1_580000164__dpitch_num_bytes 	64
#define flexidag_openseg_prim_split_49_concat_1_580000164__dram_format 	3
#define flexidag_openseg_prim_split_49_concat_1_580000164__vp_interleave_mode 	2
#define flexidag_openseg_prim_split_49_concat_1_580000164__tile_width 	4
#define flexidag_openseg_prim_split_49_concat_1_580000164__tile_height 	1
#define flexidag_openseg_prim_split_49_concat_1_580000164__vmem_buffer_num_bytes 	21888
#define flexidag_openseg_prim_split_49_concat_1_580000164__dram_size_num_bytes 	7004160
#define flexidag_openseg_prim_split_49_concat_1_580000164__sign 	0
#define flexidag_openseg_prim_split_49_concat_1_580000164__datasize 	1
#define flexidag_openseg_prim_split_49_concat_1_580000164__expoffset 	13
#define flexidag_openseg_prim_split_49_concat_1_580000164__expbits 	0
#define flexidag_openseg_prim_split_49_concat_1_580000164__drotate 	0
#define flexidag_openseg_prim_split_49_concat_1_580000164__hflip 	0
#define flexidag_openseg_prim_split_49_concat_1_580000164__vflip 	0
#define flexidag_openseg_prim_split_49_concat_1_580000164__dflip 	0
#define flexidag_openseg_prim_split_49_concat_1_580000164__pflip 	0
#define flexidag_openseg_prim_split_49_concat_1_580000164__trailing_extra_elements 	0

/* flexidag_openseg_prim_split_49 SMB_output __vas_created_terminate__v0 */
#define flexidag_openseg_prim_split_49___vas_created_terminate__v0_cnngen_demangled_name 	"__vas_created_terminate__v0"
#define flexidag_openseg_prim_split_49___vas_created_terminate__v0_is_main_input_output 	0
#define flexidag_openseg_prim_split_49___vas_created_terminate__v0_is_bitvector 	1
#define flexidag_openseg_prim_split_49___vas_created_terminate__v0_is_rlz 	0
#define flexidag_openseg_prim_split_49___vas_created_terminate__v0_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_49___vas_created_terminate__v0_byte_offset 	188	/* 0x000000bc */
#define flexidag_openseg_prim_split_49___vas_created_terminate__v0_vbase_byte_offset 	192	/* 0x000000c0 */
#define flexidag_openseg_prim_split_49___vas_created_terminate__v0_W 	256
#define flexidag_openseg_prim_split_49___vas_created_terminate__v0_H 	3
#define flexidag_openseg_prim_split_49___vas_created_terminate__v0_D 	1
#define flexidag_openseg_prim_split_49___vas_created_terminate__v0_P 	1
#define flexidag_openseg_prim_split_49___vas_created_terminate__v0_data_num_bytes 	1
#define flexidag_openseg_prim_split_49___vas_created_terminate__v0_denable 	0
#define flexidag_openseg_prim_split_49___vas_created_terminate__v0_dram_format 	0
#define flexidag_openseg_prim_split_49___vas_created_terminate__v0_vp_interleave_mode 	1
#define flexidag_openseg_prim_split_49___vas_created_terminate__v0_tile_width 	1
#define flexidag_openseg_prim_split_49___vas_created_terminate__v0_tile_height 	1
#define flexidag_openseg_prim_split_49___vas_created_terminate__v0_vmem_buffer_num_bytes 	96


#endif /* flexidag_openseg_prim_split_49_H */
