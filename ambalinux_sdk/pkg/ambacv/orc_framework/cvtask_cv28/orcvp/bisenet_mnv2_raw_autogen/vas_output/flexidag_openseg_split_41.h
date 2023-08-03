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
#ifndef flexidag_openseg_prim_split_41_H
#define flexidag_openseg_prim_split_41_H

#define vas_gen_header_version                  	7

/* dag name */
#define flexidag_openseg_prim_split_41_DAG_NAME 	"flexidag_openseg_prim_split_41"

/* VDG file info */
#define flexidag_openseg_prim_split_41_vdg_name 	"flexidag_openseg_split_41.vdg"

/* VDG DAG memory info */
#define flexidag_openseg_prim_split_41_byte_offset 	4294967295	/* 0xffffffff */
#define flexidag_openseg_prim_split_41_byte_size 	352	/* 0x00000160 */
#define flexidag_openseg_prim_split_41_preferred_dram_xfer_size 	1

/* flexidag_openseg_prim_split_41 VMEM info */
#define flexidag_openseg_prim_split_41_use_ping_pong_vmem 	0
#define flexidag_openseg_prim_split_41_VMEM_start 	4096	/* 0x00001000 */
#define flexidag_openseg_prim_split_41_VMEM_end 	511584	/* 0x0007ce60 */
#define flexidag_openseg_prim_split_41_image_start 	511584	/* 0x0007ce60 */
#define flexidag_openseg_prim_split_41_image_size 	352	/* 0x00000160 */
#define flexidag_openseg_prim_split_41_dagbin_start 	511584	/* 0x0007ce60 */

/* flexidag_openseg_prim_split_41 DAG info */
#define flexidag_openseg_prim_split_41_estimated_cycles 	1004336
#define flexidag_openseg_prim_split_41_ddi_byte_size 	0	/* 0x00000000 */

/*******************************************************************************
 * DAG byte offset info for HMB/SMB/VCB and their fields
 * in DAG 'flexidag_openseg_prim_split_41' in source file 'pre_split28_flexidag_openseg_split_2.vas'
 ******************************************************************************/
/* flexidag_openseg_prim_split_41 HMB_input concat_1_580000105_ */
#define flexidag_openseg_prim_split_41_concat_1_580000105__cnngen_demangled_name 	"concat_1_580000105_"
#define flexidag_openseg_prim_split_41_concat_1_580000105__is_main_input_output 	0
#define flexidag_openseg_prim_split_41_concat_1_580000105__is_constant 	0
#define flexidag_openseg_prim_split_41_concat_1_580000105__has_init_data 	0
#define flexidag_openseg_prim_split_41_concat_1_580000105__is_variable_scalar 	0
#define flexidag_openseg_prim_split_41_concat_1_580000105__ddi_byte_size 	0	/* 0x00000000 */
#define flexidag_openseg_prim_split_41_concat_1_580000105__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define flexidag_openseg_prim_split_41_concat_1_580000105__is_bitvector 	0
#define flexidag_openseg_prim_split_41_concat_1_580000105__is_rlz 	0
#define flexidag_openseg_prim_split_41_concat_1_580000105__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_41_concat_1_580000105__byte_offset 	0	/* 0x00000000 */
#define flexidag_openseg_prim_split_41_concat_1_580000105__vbase_byte_offset 	4	/* 0x00000004 */
#define flexidag_openseg_prim_split_41_concat_1_580000105__dbase_byte_offset 	28	/* 0x0000001c */
#define flexidag_openseg_prim_split_41_concat_1_580000105__dpitchm1_byte_offset 	32	/* 0x00000020 */
#define flexidag_openseg_prim_split_41_concat_1_580000105__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_41_concat_1_580000105__vwidth_minus_one_byte_offset 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_41_concat_1_580000105__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_41_concat_1_580000105__vheight_minus_one_byte_offset 	18	/* 0x00000012 */
#define flexidag_openseg_prim_split_41_concat_1_580000105__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_41_concat_1_580000105__drotate_bit_offset 	198	/* 0x000000c6 */
#define flexidag_openseg_prim_split_41_concat_1_580000105__hflip_bit_offset 	199	/* 0x000000c7 */
#define flexidag_openseg_prim_split_41_concat_1_580000105__vflip_bit_offset 	200	/* 0x000000c8 */
#define flexidag_openseg_prim_split_41_concat_1_580000105__dflip_bit_offset 	201	/* 0x000000c9 */
#define flexidag_openseg_prim_split_41_concat_1_580000105__pflip_bit_offset 	202	/* 0x000000ca */
#define flexidag_openseg_prim_split_41_concat_1_580000105__W 	160
#define flexidag_openseg_prim_split_41_concat_1_580000105__H 	64
#define flexidag_openseg_prim_split_41_concat_1_580000105__D 	342
#define flexidag_openseg_prim_split_41_concat_1_580000105__P 	1
#define flexidag_openseg_prim_split_41_concat_1_580000105__data_num_bytes 	2
#define flexidag_openseg_prim_split_41_concat_1_580000105__denable 	1
#define flexidag_openseg_prim_split_41_concat_1_580000105__dpitch_num_bytes 	64
#define flexidag_openseg_prim_split_41_concat_1_580000105__dram_format 	3
#define flexidag_openseg_prim_split_41_concat_1_580000105__vp_interleave_mode 	2
#define flexidag_openseg_prim_split_41_concat_1_580000105__tile_width 	4
#define flexidag_openseg_prim_split_41_concat_1_580000105__tile_height 	1
#define flexidag_openseg_prim_split_41_concat_1_580000105__vmem_buffer_num_bytes 	109440
#define flexidag_openseg_prim_split_41_concat_1_580000105__dram_size_num_bytes 	7004160
#define flexidag_openseg_prim_split_41_concat_1_580000105__sign 	0
#define flexidag_openseg_prim_split_41_concat_1_580000105__datasize 	1
#define flexidag_openseg_prim_split_41_concat_1_580000105__expoffset 	13
#define flexidag_openseg_prim_split_41_concat_1_580000105__expbits 	0
#define flexidag_openseg_prim_split_41_concat_1_580000105__drotate 	0
#define flexidag_openseg_prim_split_41_concat_1_580000105__hflip 	0
#define flexidag_openseg_prim_split_41_concat_1_580000105__vflip 	0
#define flexidag_openseg_prim_split_41_concat_1_580000105__dflip 	0
#define flexidag_openseg_prim_split_41_concat_1_580000105__pflip 	0
#define flexidag_openseg_prim_split_41_concat_1_580000105__trailing_extra_elements 	0

/* flexidag_openseg_prim_split_41 HMB_input concat_1 */
#define flexidag_openseg_prim_split_41_concat_1_cnngen_demangled_name 	"concat_1"
#define flexidag_openseg_prim_split_41_concat_1_is_main_input_output 	0
#define flexidag_openseg_prim_split_41_concat_1_is_constant 	0
#define flexidag_openseg_prim_split_41_concat_1_has_init_data 	0
#define flexidag_openseg_prim_split_41_concat_1_is_variable_scalar 	0
#define flexidag_openseg_prim_split_41_concat_1_ddi_byte_size 	0	/* 0x00000000 */
#define flexidag_openseg_prim_split_41_concat_1_ddi_byte_offset 	4294967295	/* 0xffffffff */
#define flexidag_openseg_prim_split_41_concat_1_is_bitvector 	0
#define flexidag_openseg_prim_split_41_concat_1_is_rlz 	0
#define flexidag_openseg_prim_split_41_concat_1_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_41_concat_1_byte_offset 	36	/* 0x00000024 */
#define flexidag_openseg_prim_split_41_concat_1_vbase_byte_offset 	40	/* 0x00000028 */
#define flexidag_openseg_prim_split_41_concat_1_dbase_byte_offset 	64	/* 0x00000040 */
#define flexidag_openseg_prim_split_41_concat_1_dpitchm1_byte_offset 	68	/* 0x00000044 */
#define flexidag_openseg_prim_split_41_concat_1_dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_41_concat_1_vwidth_minus_one_byte_offset 	52	/* 0x00000034 */
#define flexidag_openseg_prim_split_41_concat_1_vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_41_concat_1_vheight_minus_one_byte_offset 	54	/* 0x00000036 */
#define flexidag_openseg_prim_split_41_concat_1_vheight_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_41_concat_1_drotate_bit_offset 	486	/* 0x000001e6 */
#define flexidag_openseg_prim_split_41_concat_1_hflip_bit_offset 	487	/* 0x000001e7 */
#define flexidag_openseg_prim_split_41_concat_1_vflip_bit_offset 	488	/* 0x000001e8 */
#define flexidag_openseg_prim_split_41_concat_1_dflip_bit_offset 	489	/* 0x000001e9 */
#define flexidag_openseg_prim_split_41_concat_1_pflip_bit_offset 	490	/* 0x000001ea */
#define flexidag_openseg_prim_split_41_concat_1_W 	160
#define flexidag_openseg_prim_split_41_concat_1_H 	64
#define flexidag_openseg_prim_split_41_concat_1_D 	1024
#define flexidag_openseg_prim_split_41_concat_1_P 	1
#define flexidag_openseg_prim_split_41_concat_1_data_num_bytes 	2
#define flexidag_openseg_prim_split_41_concat_1_denable 	1
#define flexidag_openseg_prim_split_41_concat_1_dpitch_num_bytes 	64
#define flexidag_openseg_prim_split_41_concat_1_dram_format 	3
#define flexidag_openseg_prim_split_41_concat_1_vp_interleave_mode 	2
#define flexidag_openseg_prim_split_41_concat_1_tile_width 	1
#define flexidag_openseg_prim_split_41_concat_1_tile_height 	1
#define flexidag_openseg_prim_split_41_concat_1_vmem_buffer_num_bytes 	36864
#define flexidag_openseg_prim_split_41_concat_1_dram_size_num_bytes 	20971520
#define flexidag_openseg_prim_split_41_concat_1_sign 	0
#define flexidag_openseg_prim_split_41_concat_1_datasize 	1
#define flexidag_openseg_prim_split_41_concat_1_expoffset 	13
#define flexidag_openseg_prim_split_41_concat_1_expbits 	0
#define flexidag_openseg_prim_split_41_concat_1_drotate 	0
#define flexidag_openseg_prim_split_41_concat_1_hflip 	0
#define flexidag_openseg_prim_split_41_concat_1_vflip 	0
#define flexidag_openseg_prim_split_41_concat_1_dflip 	0
#define flexidag_openseg_prim_split_41_concat_1_pflip 	0
#define flexidag_openseg_prim_split_41_concat_1_trailing_extra_elements 	0

/* flexidag_openseg_prim_split_41 HMB_output concat_1_580000109_ */
#define flexidag_openseg_prim_split_41_concat_1_580000109__cnngen_demangled_name 	"concat_1_580000109_"
#define flexidag_openseg_prim_split_41_concat_1_580000109__is_main_input_output 	0
#define flexidag_openseg_prim_split_41_concat_1_580000109__is_bitvector 	0
#define flexidag_openseg_prim_split_41_concat_1_580000109__is_rlz 	0
#define flexidag_openseg_prim_split_41_concat_1_580000109__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_41_concat_1_580000109__byte_offset 	92	/* 0x0000005c */
#define flexidag_openseg_prim_split_41_concat_1_580000109__vbase_byte_offset 	96	/* 0x00000060 */
#define flexidag_openseg_prim_split_41_concat_1_580000109__dbase_byte_offset 	120	/* 0x00000078 */
#define flexidag_openseg_prim_split_41_concat_1_580000109__dpitchm1_byte_offset 	124	/* 0x0000007c */
#define flexidag_openseg_prim_split_41_concat_1_580000109__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_41_concat_1_580000109__vwidth_minus_one_byte_offset 	108	/* 0x0000006c */
#define flexidag_openseg_prim_split_41_concat_1_580000109__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_41_concat_1_580000109__vheight_minus_one_byte_offset 	110	/* 0x0000006e */
#define flexidag_openseg_prim_split_41_concat_1_580000109__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_41_concat_1_580000109__drotate_bit_offset 	934	/* 0x000003a6 */
#define flexidag_openseg_prim_split_41_concat_1_580000109__hflip_bit_offset 	935	/* 0x000003a7 */
#define flexidag_openseg_prim_split_41_concat_1_580000109__vflip_bit_offset 	936	/* 0x000003a8 */
#define flexidag_openseg_prim_split_41_concat_1_580000109__dflip_bit_offset 	937	/* 0x000003a9 */
#define flexidag_openseg_prim_split_41_concat_1_580000109__pflip_bit_offset 	938	/* 0x000003aa */
#define flexidag_openseg_prim_split_41_concat_1_580000109__W 	18
#define flexidag_openseg_prim_split_41_concat_1_580000109__H 	64
#define flexidag_openseg_prim_split_41_concat_1_580000109__D 	342
#define flexidag_openseg_prim_split_41_concat_1_580000109__P 	1
#define flexidag_openseg_prim_split_41_concat_1_580000109__data_num_bytes 	2
#define flexidag_openseg_prim_split_41_concat_1_580000109__denable 	1
#define flexidag_openseg_prim_split_41_concat_1_580000109__dpitch_num_bytes 	64
#define flexidag_openseg_prim_split_41_concat_1_580000109__dram_format 	3
#define flexidag_openseg_prim_split_41_concat_1_580000109__vp_interleave_mode 	2
#define flexidag_openseg_prim_split_41_concat_1_580000109__tile_width 	4
#define flexidag_openseg_prim_split_41_concat_1_580000109__tile_height 	1
#define flexidag_openseg_prim_split_41_concat_1_580000109__vmem_buffer_num_bytes 	10944
#define flexidag_openseg_prim_split_41_concat_1_580000109__dram_size_num_bytes 	1050624
#define flexidag_openseg_prim_split_41_concat_1_580000109__sign 	0
#define flexidag_openseg_prim_split_41_concat_1_580000109__datasize 	1
#define flexidag_openseg_prim_split_41_concat_1_580000109__expoffset 	13
#define flexidag_openseg_prim_split_41_concat_1_580000109__expbits 	0
#define flexidag_openseg_prim_split_41_concat_1_580000109__drotate 	0
#define flexidag_openseg_prim_split_41_concat_1_580000109__hflip 	0
#define flexidag_openseg_prim_split_41_concat_1_580000109__vflip 	0
#define flexidag_openseg_prim_split_41_concat_1_580000109__dflip 	0
#define flexidag_openseg_prim_split_41_concat_1_580000109__pflip 	0
#define flexidag_openseg_prim_split_41_concat_1_580000109__trailing_extra_elements 	0

/* flexidag_openseg_prim_split_41 VCB concat_1_580000110_ */
#define flexidag_openseg_prim_split_41_concat_1_580000110__cnngen_demangled_name 	"concat_1_580000110_"
#define flexidag_openseg_prim_split_41_concat_1_580000110__is_bitvector 	0
#define flexidag_openseg_prim_split_41_concat_1_580000110__is_rlz 	0
#define flexidag_openseg_prim_split_41_concat_1_580000110__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_41_concat_1_580000110__byte_offset 	144	/* 0x00000090 */
#define flexidag_openseg_prim_split_41_concat_1_580000110__vbase_byte_offset 	152	/* 0x00000098 */
#define flexidag_openseg_prim_split_41_concat_1_580000110__vmem_buffer_num_bytes 	328320

/* flexidag_openseg_prim_split_41 HMB_output concat_1_580000114_ */
#define flexidag_openseg_prim_split_41_concat_1_580000114__cnngen_demangled_name 	"concat_1_580000114_"
#define flexidag_openseg_prim_split_41_concat_1_580000114__is_main_input_output 	0
#define flexidag_openseg_prim_split_41_concat_1_580000114__is_bitvector 	0
#define flexidag_openseg_prim_split_41_concat_1_580000114__is_rlz 	0
#define flexidag_openseg_prim_split_41_concat_1_580000114__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_41_concat_1_580000114__byte_offset 	184	/* 0x000000b8 */
#define flexidag_openseg_prim_split_41_concat_1_580000114__vbase_byte_offset 	188	/* 0x000000bc */
#define flexidag_openseg_prim_split_41_concat_1_580000114__dbase_byte_offset 	212	/* 0x000000d4 */
#define flexidag_openseg_prim_split_41_concat_1_580000114__dpitchm1_byte_offset 	216	/* 0x000000d8 */
#define flexidag_openseg_prim_split_41_concat_1_580000114__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_41_concat_1_580000114__vwidth_minus_one_byte_offset 	200	/* 0x000000c8 */
#define flexidag_openseg_prim_split_41_concat_1_580000114__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_41_concat_1_580000114__vheight_minus_one_byte_offset 	202	/* 0x000000ca */
#define flexidag_openseg_prim_split_41_concat_1_580000114__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_41_concat_1_580000114__drotate_bit_offset 	1670	/* 0x00000686 */
#define flexidag_openseg_prim_split_41_concat_1_580000114__hflip_bit_offset 	1671	/* 0x00000687 */
#define flexidag_openseg_prim_split_41_concat_1_580000114__vflip_bit_offset 	1672	/* 0x00000688 */
#define flexidag_openseg_prim_split_41_concat_1_580000114__dflip_bit_offset 	1673	/* 0x00000689 */
#define flexidag_openseg_prim_split_41_concat_1_580000114__pflip_bit_offset 	1674	/* 0x0000068a */
#define flexidag_openseg_prim_split_41_concat_1_580000114__W 	18
#define flexidag_openseg_prim_split_41_concat_1_580000114__H 	64
#define flexidag_openseg_prim_split_41_concat_1_580000114__D 	342
#define flexidag_openseg_prim_split_41_concat_1_580000114__P 	1
#define flexidag_openseg_prim_split_41_concat_1_580000114__data_num_bytes 	2
#define flexidag_openseg_prim_split_41_concat_1_580000114__denable 	1
#define flexidag_openseg_prim_split_41_concat_1_580000114__dpitch_num_bytes 	64
#define flexidag_openseg_prim_split_41_concat_1_580000114__dram_format 	3
#define flexidag_openseg_prim_split_41_concat_1_580000114__vp_interleave_mode 	2
#define flexidag_openseg_prim_split_41_concat_1_580000114__tile_width 	4
#define flexidag_openseg_prim_split_41_concat_1_580000114__tile_height 	1
#define flexidag_openseg_prim_split_41_concat_1_580000114__vmem_buffer_num_bytes 	10944
#define flexidag_openseg_prim_split_41_concat_1_580000114__dram_size_num_bytes 	1050624
#define flexidag_openseg_prim_split_41_concat_1_580000114__sign 	0
#define flexidag_openseg_prim_split_41_concat_1_580000114__datasize 	1
#define flexidag_openseg_prim_split_41_concat_1_580000114__expoffset 	13
#define flexidag_openseg_prim_split_41_concat_1_580000114__expbits 	0
#define flexidag_openseg_prim_split_41_concat_1_580000114__drotate 	0
#define flexidag_openseg_prim_split_41_concat_1_580000114__hflip 	0
#define flexidag_openseg_prim_split_41_concat_1_580000114__vflip 	0
#define flexidag_openseg_prim_split_41_concat_1_580000114__dflip 	0
#define flexidag_openseg_prim_split_41_concat_1_580000114__pflip 	0
#define flexidag_openseg_prim_split_41_concat_1_580000114__trailing_extra_elements 	0

/* flexidag_openseg_prim_split_41 HMB_output concat_1_580000115_ */
#define flexidag_openseg_prim_split_41_concat_1_580000115__cnngen_demangled_name 	"concat_1_580000115_"
#define flexidag_openseg_prim_split_41_concat_1_580000115__is_main_input_output 	0
#define flexidag_openseg_prim_split_41_concat_1_580000115__is_bitvector 	0
#define flexidag_openseg_prim_split_41_concat_1_580000115__is_rlz 	0
#define flexidag_openseg_prim_split_41_concat_1_580000115__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_41_concat_1_580000115__byte_offset 	236	/* 0x000000ec */
#define flexidag_openseg_prim_split_41_concat_1_580000115__vbase_byte_offset 	240	/* 0x000000f0 */
#define flexidag_openseg_prim_split_41_concat_1_580000115__dbase_byte_offset 	264	/* 0x00000108 */
#define flexidag_openseg_prim_split_41_concat_1_580000115__dpitchm1_byte_offset 	268	/* 0x0000010c */
#define flexidag_openseg_prim_split_41_concat_1_580000115__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_41_concat_1_580000115__vwidth_minus_one_byte_offset 	252	/* 0x000000fc */
#define flexidag_openseg_prim_split_41_concat_1_580000115__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_41_concat_1_580000115__vheight_minus_one_byte_offset 	254	/* 0x000000fe */
#define flexidag_openseg_prim_split_41_concat_1_580000115__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_41_concat_1_580000115__drotate_bit_offset 	2086	/* 0x00000826 */
#define flexidag_openseg_prim_split_41_concat_1_580000115__hflip_bit_offset 	2087	/* 0x00000827 */
#define flexidag_openseg_prim_split_41_concat_1_580000115__vflip_bit_offset 	2088	/* 0x00000828 */
#define flexidag_openseg_prim_split_41_concat_1_580000115__dflip_bit_offset 	2089	/* 0x00000829 */
#define flexidag_openseg_prim_split_41_concat_1_580000115__pflip_bit_offset 	2090	/* 0x0000082a */
#define flexidag_openseg_prim_split_41_concat_1_580000115__W 	160
#define flexidag_openseg_prim_split_41_concat_1_580000115__H 	64
#define flexidag_openseg_prim_split_41_concat_1_580000115__D 	340
#define flexidag_openseg_prim_split_41_concat_1_580000115__P 	1
#define flexidag_openseg_prim_split_41_concat_1_580000115__data_num_bytes 	2
#define flexidag_openseg_prim_split_41_concat_1_580000115__denable 	1
#define flexidag_openseg_prim_split_41_concat_1_580000115__dpitch_num_bytes 	64
#define flexidag_openseg_prim_split_41_concat_1_580000115__dram_format 	3
#define flexidag_openseg_prim_split_41_concat_1_580000115__vp_interleave_mode 	2
#define flexidag_openseg_prim_split_41_concat_1_580000115__tile_width 	4
#define flexidag_openseg_prim_split_41_concat_1_580000115__tile_height 	1
#define flexidag_openseg_prim_split_41_concat_1_580000115__vmem_buffer_num_bytes 	10880
#define flexidag_openseg_prim_split_41_concat_1_580000115__dram_size_num_bytes 	6963200
#define flexidag_openseg_prim_split_41_concat_1_580000115__sign 	0
#define flexidag_openseg_prim_split_41_concat_1_580000115__datasize 	1
#define flexidag_openseg_prim_split_41_concat_1_580000115__expoffset 	13
#define flexidag_openseg_prim_split_41_concat_1_580000115__expbits 	0
#define flexidag_openseg_prim_split_41_concat_1_580000115__drotate 	0
#define flexidag_openseg_prim_split_41_concat_1_580000115__hflip 	0
#define flexidag_openseg_prim_split_41_concat_1_580000115__vflip 	0
#define flexidag_openseg_prim_split_41_concat_1_580000115__dflip 	0
#define flexidag_openseg_prim_split_41_concat_1_580000115__pflip 	0
#define flexidag_openseg_prim_split_41_concat_1_580000115__trailing_extra_elements 	0

/* flexidag_openseg_prim_split_41 SMB_output __vas_created_terminate__v0 */
#define flexidag_openseg_prim_split_41___vas_created_terminate__v0_cnngen_demangled_name 	"__vas_created_terminate__v0"
#define flexidag_openseg_prim_split_41___vas_created_terminate__v0_is_main_input_output 	0
#define flexidag_openseg_prim_split_41___vas_created_terminate__v0_is_bitvector 	1
#define flexidag_openseg_prim_split_41___vas_created_terminate__v0_is_rlz 	0
#define flexidag_openseg_prim_split_41___vas_created_terminate__v0_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_41___vas_created_terminate__v0_byte_offset 	276	/* 0x00000114 */
#define flexidag_openseg_prim_split_41___vas_created_terminate__v0_vbase_byte_offset 	280	/* 0x00000118 */
#define flexidag_openseg_prim_split_41___vas_created_terminate__v0_W 	256
#define flexidag_openseg_prim_split_41___vas_created_terminate__v0_H 	3
#define flexidag_openseg_prim_split_41___vas_created_terminate__v0_D 	1
#define flexidag_openseg_prim_split_41___vas_created_terminate__v0_P 	1
#define flexidag_openseg_prim_split_41___vas_created_terminate__v0_data_num_bytes 	1
#define flexidag_openseg_prim_split_41___vas_created_terminate__v0_denable 	0
#define flexidag_openseg_prim_split_41___vas_created_terminate__v0_dram_format 	0
#define flexidag_openseg_prim_split_41___vas_created_terminate__v0_vp_interleave_mode 	1
#define flexidag_openseg_prim_split_41___vas_created_terminate__v0_tile_width 	1
#define flexidag_openseg_prim_split_41___vas_created_terminate__v0_tile_height 	1
#define flexidag_openseg_prim_split_41___vas_created_terminate__v0_vmem_buffer_num_bytes 	96


#endif /* flexidag_openseg_prim_split_41_H */
