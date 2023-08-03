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
#ifndef flexidag_openseg_prim_split_39_H
#define flexidag_openseg_prim_split_39_H

#define vas_gen_header_version                  	7

/* dag name */
#define flexidag_openseg_prim_split_39_DAG_NAME 	"flexidag_openseg_prim_split_39"

/* VDG file info */
#define flexidag_openseg_prim_split_39_vdg_name 	"flexidag_openseg_split_39.vdg"

/* VDG DAG memory info */
#define flexidag_openseg_prim_split_39_byte_offset 	4294967295	/* 0xffffffff */
#define flexidag_openseg_prim_split_39_byte_size 	348	/* 0x0000015c */
#define flexidag_openseg_prim_split_39_preferred_dram_xfer_size 	1

/* flexidag_openseg_prim_split_39 VMEM info */
#define flexidag_openseg_prim_split_39_use_ping_pong_vmem 	0
#define flexidag_openseg_prim_split_39_VMEM_start 	4096	/* 0x00001000 */
#define flexidag_openseg_prim_split_39_VMEM_end 	499808	/* 0x0007a060 */
#define flexidag_openseg_prim_split_39_image_start 	499808	/* 0x0007a060 */
#define flexidag_openseg_prim_split_39_image_size 	348	/* 0x0000015c */
#define flexidag_openseg_prim_split_39_dagbin_start 	499808	/* 0x0007a060 */

/* flexidag_openseg_prim_split_39 DAG info */
#define flexidag_openseg_prim_split_39_estimated_cycles 	1507376
#define flexidag_openseg_prim_split_39_ddi_byte_size 	0	/* 0x00000000 */

/*******************************************************************************
 * DAG byte offset info for HMB/SMB/VCB and their fields
 * in DAG 'flexidag_openseg_prim_split_39' in source file 'pre_split28_flexidag_openseg_split_0.vas'
 ******************************************************************************/
/* flexidag_openseg_prim_split_39 HMB_input concat_1 */
#define flexidag_openseg_prim_split_39_concat_1_cnngen_demangled_name 	"concat_1"
#define flexidag_openseg_prim_split_39_concat_1_is_main_input_output 	0
#define flexidag_openseg_prim_split_39_concat_1_is_constant 	0
#define flexidag_openseg_prim_split_39_concat_1_has_init_data 	0
#define flexidag_openseg_prim_split_39_concat_1_is_variable_scalar 	0
#define flexidag_openseg_prim_split_39_concat_1_ddi_byte_size 	0	/* 0x00000000 */
#define flexidag_openseg_prim_split_39_concat_1_ddi_byte_offset 	4294967295	/* 0xffffffff */
#define flexidag_openseg_prim_split_39_concat_1_is_bitvector 	0
#define flexidag_openseg_prim_split_39_concat_1_is_rlz 	0
#define flexidag_openseg_prim_split_39_concat_1_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_39_concat_1_byte_offset 	0	/* 0x00000000 */
#define flexidag_openseg_prim_split_39_concat_1_vbase_byte_offset 	4	/* 0x00000004 */
#define flexidag_openseg_prim_split_39_concat_1_dbase_byte_offset 	28	/* 0x0000001c */
#define flexidag_openseg_prim_split_39_concat_1_dpitchm1_byte_offset 	32	/* 0x00000020 */
#define flexidag_openseg_prim_split_39_concat_1_dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_39_concat_1_vwidth_minus_one_byte_offset 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_39_concat_1_vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_39_concat_1_vheight_minus_one_byte_offset 	18	/* 0x00000012 */
#define flexidag_openseg_prim_split_39_concat_1_vheight_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_39_concat_1_drotate_bit_offset 	198	/* 0x000000c6 */
#define flexidag_openseg_prim_split_39_concat_1_hflip_bit_offset 	199	/* 0x000000c7 */
#define flexidag_openseg_prim_split_39_concat_1_vflip_bit_offset 	200	/* 0x000000c8 */
#define flexidag_openseg_prim_split_39_concat_1_dflip_bit_offset 	201	/* 0x000000c9 */
#define flexidag_openseg_prim_split_39_concat_1_pflip_bit_offset 	202	/* 0x000000ca */
#define flexidag_openseg_prim_split_39_concat_1_W 	160
#define flexidag_openseg_prim_split_39_concat_1_H 	64
#define flexidag_openseg_prim_split_39_concat_1_D 	1024
#define flexidag_openseg_prim_split_39_concat_1_P 	1
#define flexidag_openseg_prim_split_39_concat_1_data_num_bytes 	2
#define flexidag_openseg_prim_split_39_concat_1_denable 	1
#define flexidag_openseg_prim_split_39_concat_1_dpitch_num_bytes 	64
#define flexidag_openseg_prim_split_39_concat_1_dram_format 	3
#define flexidag_openseg_prim_split_39_concat_1_vp_interleave_mode 	2
#define flexidag_openseg_prim_split_39_concat_1_tile_width 	1
#define flexidag_openseg_prim_split_39_concat_1_tile_height 	1
#define flexidag_openseg_prim_split_39_concat_1_vmem_buffer_num_bytes 	135168
#define flexidag_openseg_prim_split_39_concat_1_dram_size_num_bytes 	20971520
#define flexidag_openseg_prim_split_39_concat_1_sign 	0
#define flexidag_openseg_prim_split_39_concat_1_datasize 	1
#define flexidag_openseg_prim_split_39_concat_1_expoffset 	13
#define flexidag_openseg_prim_split_39_concat_1_expbits 	0
#define flexidag_openseg_prim_split_39_concat_1_drotate 	0
#define flexidag_openseg_prim_split_39_concat_1_hflip 	0
#define flexidag_openseg_prim_split_39_concat_1_vflip 	0
#define flexidag_openseg_prim_split_39_concat_1_dflip 	0
#define flexidag_openseg_prim_split_39_concat_1_pflip 	0
#define flexidag_openseg_prim_split_39_concat_1_trailing_extra_elements 	0

/* flexidag_openseg_prim_split_39 VCB concat_1_580000087_ */
#define flexidag_openseg_prim_split_39_concat_1_580000087__cnngen_demangled_name 	"concat_1_580000087_"
#define flexidag_openseg_prim_split_39_concat_1_580000087__is_bitvector 	0
#define flexidag_openseg_prim_split_39_concat_1_580000087__is_rlz 	0
#define flexidag_openseg_prim_split_39_concat_1_580000087__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_39_concat_1_580000087__byte_offset 	56	/* 0x00000038 */
#define flexidag_openseg_prim_split_39_concat_1_580000087__vbase_byte_offset 	64	/* 0x00000040 */
#define flexidag_openseg_prim_split_39_concat_1_580000087__vmem_buffer_num_bytes 	109440

/* flexidag_openseg_prim_split_39 HMB_output concat_1_580000000_ */
#define flexidag_openseg_prim_split_39_concat_1_580000000__cnngen_demangled_name 	"concat_1_580000000_"
#define flexidag_openseg_prim_split_39_concat_1_580000000__is_main_input_output 	0
#define flexidag_openseg_prim_split_39_concat_1_580000000__is_bitvector 	0
#define flexidag_openseg_prim_split_39_concat_1_580000000__is_rlz 	0
#define flexidag_openseg_prim_split_39_concat_1_580000000__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_39_concat_1_580000000__byte_offset 	80	/* 0x00000050 */
#define flexidag_openseg_prim_split_39_concat_1_580000000__vbase_byte_offset 	84	/* 0x00000054 */
#define flexidag_openseg_prim_split_39_concat_1_580000000__dbase_byte_offset 	108	/* 0x0000006c */
#define flexidag_openseg_prim_split_39_concat_1_580000000__dpitchm1_byte_offset 	112	/* 0x00000070 */
#define flexidag_openseg_prim_split_39_concat_1_580000000__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_39_concat_1_580000000__vwidth_minus_one_byte_offset 	96	/* 0x00000060 */
#define flexidag_openseg_prim_split_39_concat_1_580000000__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_39_concat_1_580000000__vheight_minus_one_byte_offset 	98	/* 0x00000062 */
#define flexidag_openseg_prim_split_39_concat_1_580000000__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_39_concat_1_580000000__drotate_bit_offset 	838	/* 0x00000346 */
#define flexidag_openseg_prim_split_39_concat_1_580000000__hflip_bit_offset 	839	/* 0x00000347 */
#define flexidag_openseg_prim_split_39_concat_1_580000000__vflip_bit_offset 	840	/* 0x00000348 */
#define flexidag_openseg_prim_split_39_concat_1_580000000__dflip_bit_offset 	841	/* 0x00000349 */
#define flexidag_openseg_prim_split_39_concat_1_580000000__pflip_bit_offset 	842	/* 0x0000034a */
#define flexidag_openseg_prim_split_39_concat_1_580000000__W 	17
#define flexidag_openseg_prim_split_39_concat_1_580000000__H 	64
#define flexidag_openseg_prim_split_39_concat_1_580000000__D 	1024
#define flexidag_openseg_prim_split_39_concat_1_580000000__P 	1
#define flexidag_openseg_prim_split_39_concat_1_580000000__data_num_bytes 	2
#define flexidag_openseg_prim_split_39_concat_1_580000000__denable 	1
#define flexidag_openseg_prim_split_39_concat_1_580000000__dpitch_num_bytes 	64
#define flexidag_openseg_prim_split_39_concat_1_580000000__dram_format 	3
#define flexidag_openseg_prim_split_39_concat_1_580000000__vp_interleave_mode 	2
#define flexidag_openseg_prim_split_39_concat_1_580000000__tile_width 	4
#define flexidag_openseg_prim_split_39_concat_1_580000000__tile_height 	1
#define flexidag_openseg_prim_split_39_concat_1_580000000__vmem_buffer_num_bytes 	32768
#define flexidag_openseg_prim_split_39_concat_1_580000000__dram_size_num_bytes 	3145728
#define flexidag_openseg_prim_split_39_concat_1_580000000__sign 	0
#define flexidag_openseg_prim_split_39_concat_1_580000000__datasize 	1
#define flexidag_openseg_prim_split_39_concat_1_580000000__expoffset 	13
#define flexidag_openseg_prim_split_39_concat_1_580000000__expbits 	0
#define flexidag_openseg_prim_split_39_concat_1_580000000__drotate 	0
#define flexidag_openseg_prim_split_39_concat_1_580000000__hflip 	0
#define flexidag_openseg_prim_split_39_concat_1_580000000__vflip 	0
#define flexidag_openseg_prim_split_39_concat_1_580000000__dflip 	0
#define flexidag_openseg_prim_split_39_concat_1_580000000__pflip 	0
#define flexidag_openseg_prim_split_39_concat_1_580000000__trailing_extra_elements 	0

/* flexidag_openseg_prim_split_39 VCB concat_1_580000092_ */
#define flexidag_openseg_prim_split_39_concat_1_580000092__cnngen_demangled_name 	"concat_1_580000092_"
#define flexidag_openseg_prim_split_39_concat_1_580000092__is_bitvector 	0
#define flexidag_openseg_prim_split_39_concat_1_580000092__is_rlz 	0
#define flexidag_openseg_prim_split_39_concat_1_580000092__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_39_concat_1_580000092__byte_offset 	156	/* 0x0000009c */
#define flexidag_openseg_prim_split_39_concat_1_580000092__vbase_byte_offset 	164	/* 0x000000a4 */
#define flexidag_openseg_prim_split_39_concat_1_580000092__vmem_buffer_num_bytes 	109440

/* flexidag_openseg_prim_split_39 VCB concat_1_580000097_ */
#define flexidag_openseg_prim_split_39_concat_1_580000097__cnngen_demangled_name 	"concat_1_580000097_"
#define flexidag_openseg_prim_split_39_concat_1_580000097__is_bitvector 	0
#define flexidag_openseg_prim_split_39_concat_1_580000097__is_rlz 	0
#define flexidag_openseg_prim_split_39_concat_1_580000097__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_39_concat_1_580000097__byte_offset 	220	/* 0x000000dc */
#define flexidag_openseg_prim_split_39_concat_1_580000097__vbase_byte_offset 	228	/* 0x000000e4 */
#define flexidag_openseg_prim_split_39_concat_1_580000097__vmem_buffer_num_bytes 	108800

/* flexidag_openseg_prim_split_39 SMB_output __vas_created_terminate__v0 */
#define flexidag_openseg_prim_split_39___vas_created_terminate__v0_cnngen_demangled_name 	"__vas_created_terminate__v0"
#define flexidag_openseg_prim_split_39___vas_created_terminate__v0_is_main_input_output 	0
#define flexidag_openseg_prim_split_39___vas_created_terminate__v0_is_bitvector 	1
#define flexidag_openseg_prim_split_39___vas_created_terminate__v0_is_rlz 	0
#define flexidag_openseg_prim_split_39___vas_created_terminate__v0_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_39___vas_created_terminate__v0_byte_offset 	272	/* 0x00000110 */
#define flexidag_openseg_prim_split_39___vas_created_terminate__v0_vbase_byte_offset 	276	/* 0x00000114 */
#define flexidag_openseg_prim_split_39___vas_created_terminate__v0_W 	256
#define flexidag_openseg_prim_split_39___vas_created_terminate__v0_H 	3
#define flexidag_openseg_prim_split_39___vas_created_terminate__v0_D 	1
#define flexidag_openseg_prim_split_39___vas_created_terminate__v0_P 	1
#define flexidag_openseg_prim_split_39___vas_created_terminate__v0_data_num_bytes 	1
#define flexidag_openseg_prim_split_39___vas_created_terminate__v0_denable 	0
#define flexidag_openseg_prim_split_39___vas_created_terminate__v0_dram_format 	0
#define flexidag_openseg_prim_split_39___vas_created_terminate__v0_vp_interleave_mode 	1
#define flexidag_openseg_prim_split_39___vas_created_terminate__v0_tile_width 	1
#define flexidag_openseg_prim_split_39___vas_created_terminate__v0_tile_height 	1
#define flexidag_openseg_prim_split_39___vas_created_terminate__v0_vmem_buffer_num_bytes 	96


#endif /* flexidag_openseg_prim_split_39_H */
