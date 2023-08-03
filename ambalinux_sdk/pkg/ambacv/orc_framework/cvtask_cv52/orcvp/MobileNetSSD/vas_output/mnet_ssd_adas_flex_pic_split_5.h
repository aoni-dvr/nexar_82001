////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Ambarella, Inc.
////////////////////////////////////////////////////////////////////////////////
// This file and its contents ("Software") are protected
// by intellectual property rights including, without limitation, U.S.
// and/or foreign copyrights.  This Software is also the confidential
// and proprietary information of Ambarella, Inc. and its licensors.
// You may not use, reproduce, disclose, distribute, modify, or
// otherwise prepare derivative works of this Software or any portion
// thereof except pursuant to a signed license agreement or
// nondisclosure agreement with Ambarella, Inc. or its authorized
// affiliates.  In the absence of such an agreement, you agree to
// promptly notify and return this Software to Ambarella, Inc.
//
// THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL AMBARELLA, INC. OR ITS
// AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.
////////////////////////////////////////////////////////////////////////////////
#ifndef mnet_ssd_adas_flex_pic_prim_split_5_H
#define mnet_ssd_adas_flex_pic_prim_split_5_H

#define vas_gen_header_version                  	7

/* dag name */
#define mnet_ssd_adas_flex_pic_prim_split_5_DAG_NAME 	"mnet_ssd_adas_flex_pic_prim_split_5"

/* VDG file info */
#define mnet_ssd_adas_flex_pic_prim_split_5_vdg_name 	"mnet_ssd_adas_flex_pic_split_5.vdg"

/* VDG DAG memory info */
#define mnet_ssd_adas_flex_pic_prim_split_5_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_5_byte_size 	1276	/* 0x000004fc */
#define mnet_ssd_adas_flex_pic_prim_split_5_preferred_dram_xfer_size 	1

/* mnet_ssd_adas_flex_pic_prim_split_5 VMEM info */
#define mnet_ssd_adas_flex_pic_prim_split_5_use_ping_pong_vmem 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_VMEM_start 	4096	/* 0x00001000 */
#define mnet_ssd_adas_flex_pic_prim_split_5_VMEM_end 	2053888	/* 0x001f5700 */
#define mnet_ssd_adas_flex_pic_prim_split_5_image_start 	88560	/* 0x000159f0 */
#define mnet_ssd_adas_flex_pic_prim_split_5_image_size 	1966604	/* 0x001e020c */
#define mnet_ssd_adas_flex_pic_prim_split_5_dagbin_start 	2053888	/* 0x001f5700 */

/* mnet_ssd_adas_flex_pic_prim_split_5 DAG info */
#define mnet_ssd_adas_flex_pic_prim_split_5_estimated_cycles 	3079585

/*******************************************************************************
 * DAG byte offset info for HMB/SMB/VCB and their fields
 * in DAG 'mnet_ssd_adas_flex_pic_prim_split_5' in source file 'mnet_ssd_adas_flex_pic_split_5.vas'
 ******************************************************************************/
/* mnet_ssd_adas_flex_pic_prim_split_5 HMB_input relu7_1 */
#define mnet_ssd_adas_flex_pic_prim_split_5_relu7_1_cnngen_demangled_name 	"relu7_1"
#define mnet_ssd_adas_flex_pic_prim_split_5_relu7_1_is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_relu7_1_is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_relu7_1_has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_relu7_1_is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_relu7_1_ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_5_relu7_1_ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_5_relu7_1_is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_relu7_1_is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_relu7_1_is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_relu7_1_is_twelve_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_relu7_1_byte_offset 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_5_relu7_1_vbase_byte_offset 	4	/* 0x00000004 */
#define mnet_ssd_adas_flex_pic_prim_split_5_relu7_1_dbase_byte_offset 	28	/* 0x0000001c */
#define mnet_ssd_adas_flex_pic_prim_split_5_relu7_1_dpitchm1_byte_offset 	32	/* 0x00000020 */
#define mnet_ssd_adas_flex_pic_prim_split_5_relu7_1_dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5_relu7_1_vwidth_minus_one_byte_offset 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5_relu7_1_vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5_relu7_1_vheight_minus_one_byte_offset 	18	/* 0x00000012 */
#define mnet_ssd_adas_flex_pic_prim_split_5_relu7_1_vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5_relu7_1_drotate_bit_offset 	198	/* 0x000000c6 */
#define mnet_ssd_adas_flex_pic_prim_split_5_relu7_1_hflip_bit_offset 	199	/* 0x000000c7 */
#define mnet_ssd_adas_flex_pic_prim_split_5_relu7_1_vflip_bit_offset 	200	/* 0x000000c8 */
#define mnet_ssd_adas_flex_pic_prim_split_5_relu7_1_dflip_bit_offset 	201	/* 0x000000c9 */
#define mnet_ssd_adas_flex_pic_prim_split_5_relu7_1_pflip_bit_offset 	202	/* 0x000000ca */
#define mnet_ssd_adas_flex_pic_prim_split_5_relu7_1_W 	10
#define mnet_ssd_adas_flex_pic_prim_split_5_relu7_1_H 	10
#define mnet_ssd_adas_flex_pic_prim_split_5_relu7_1_D 	256
#define mnet_ssd_adas_flex_pic_prim_split_5_relu7_1_P 	1
#define mnet_ssd_adas_flex_pic_prim_split_5_relu7_1_data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_5_relu7_1_denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_5_relu7_1_dpitch_num_bytes 	32
#define mnet_ssd_adas_flex_pic_prim_split_5_relu7_1_dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_5_relu7_1_vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_5_relu7_1_tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_5_relu7_1_tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_5_relu7_1_vmem_buffer_num_bytes 	36864
#define mnet_ssd_adas_flex_pic_prim_split_5_relu7_1_dram_size_num_bytes 	40960
#define mnet_ssd_adas_flex_pic_prim_split_5_relu7_1_sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_relu7_1_datasize 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_relu7_1_expoffset 	6
#define mnet_ssd_adas_flex_pic_prim_split_5_relu7_1_expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_relu7_1_drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_relu7_1_hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_relu7_1_vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_relu7_1_dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_relu7_1_pflip 	0

/* mnet_ssd_adas_flex_pic_prim_split_5 SMB_input __pvcn_150004_ */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150004__cnngen_demangled_name 	"__pvcn_150004_"
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150004__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150004__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150004__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150004__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150004__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150004__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150004__is_twelve_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150004__byte_offset 	40	/* 0x00000028 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150004__vbase_byte_offset 	44	/* 0x0000002c */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150004__dbase_byte_offset 	68	/* 0x00000044 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150004__dpitchm1_byte_offset 	72	/* 0x00000048 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150004__dpitchm1_bsize 	32	/* 0x00000020 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150004__W 	589824
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150004__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150004__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150004__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150004__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150004__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150004__dpitch_num_bytes 	589824
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150004__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150004__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150004__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150004__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150004__vmem_buffer_num_bytes 	589824
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150004__dram_size_num_bytes 	589824

/* mnet_ssd_adas_flex_pic_prim_split_5 SMB_input __pvcn_150007_ */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150007__cnngen_demangled_name 	"__pvcn_150007_"
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150007__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150007__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150007__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150007__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150007__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150007__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150007__is_twelve_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150007__byte_offset 	76	/* 0x0000004c */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150007__vbase_byte_offset 	80	/* 0x00000050 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150007__dbase_byte_offset 	104	/* 0x00000068 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150007__dpitchm1_byte_offset 	108	/* 0x0000006c */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150007__dpitchm1_bsize 	32	/* 0x00000020 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150007__W 	589824
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150007__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150007__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150007__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150007__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150007__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150007__dpitch_num_bytes 	589824
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150007__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150007__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150007__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150007__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150007__vmem_buffer_num_bytes 	589824
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150007__dram_size_num_bytes 	589824

/* mnet_ssd_adas_flex_pic_prim_split_5 SMB_input __pvcn_150005_ */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150005__cnngen_demangled_name 	"__pvcn_150005_"
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150005__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150005__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150005__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150005__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150005__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150005__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150005__is_twelve_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150005__byte_offset 	116	/* 0x00000074 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150005__vbase_byte_offset 	120	/* 0x00000078 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150005__dbase_byte_offset 	144	/* 0x00000090 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150005__dpitchm1_byte_offset 	148	/* 0x00000094 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150005__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150005__W 	2176
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150005__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150005__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150005__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150005__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150005__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150005__dpitch_num_bytes 	2176
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150005__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150005__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150005__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150005__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150005__vmem_buffer_num_bytes 	2176
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150005__dram_size_num_bytes 	2176

/* mnet_ssd_adas_flex_pic_prim_split_5 SMB_input __pvcn_150008_ */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150008__cnngen_demangled_name 	"__pvcn_150008_"
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150008__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150008__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150008__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150008__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150008__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150008__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150008__is_twelve_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150008__byte_offset 	156	/* 0x0000009c */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150008__vbase_byte_offset 	160	/* 0x000000a0 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150008__dbase_byte_offset 	184	/* 0x000000b8 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150008__dpitchm1_byte_offset 	188	/* 0x000000bc */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150008__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150008__W 	2176
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150008__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150008__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150008__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150008__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150008__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150008__dpitch_num_bytes 	2176
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150008__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150008__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150008__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150008__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150008__vmem_buffer_num_bytes 	2176
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150008__dram_size_num_bytes 	2176

/* mnet_ssd_adas_flex_pic_prim_split_5 SMB_input __pvcn_2561_ */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2561__cnngen_demangled_name 	"__pvcn_2561_"
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2561__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2561__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2561__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2561__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2561__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2561__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2561__is_twelve_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2561__byte_offset 	196	/* 0x000000c4 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2561__vbase_byte_offset 	200	/* 0x000000c8 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2561__dbase_byte_offset 	224	/* 0x000000e0 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2561__dpitchm1_byte_offset 	228	/* 0x000000e4 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2561__dpitchm1_bsize 	32	/* 0x00000020 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2561__W 	99328
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2561__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2561__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2561__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2561__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2561__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2561__dpitch_num_bytes 	99328
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2561__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2561__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2561__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2561__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2561__vmem_buffer_num_bytes 	99328
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2561__dram_size_num_bytes 	99328

/* mnet_ssd_adas_flex_pic_prim_split_5 SMB_input __pvcn_2562_ */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2562__cnngen_demangled_name 	"__pvcn_2562_"
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2562__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2562__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2562__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2562__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2562__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2562__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2562__is_twelve_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2562__byte_offset 	236	/* 0x000000ec */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2562__vbase_byte_offset 	240	/* 0x000000f0 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2562__dbase_byte_offset 	264	/* 0x00000108 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2562__dpitchm1_byte_offset 	268	/* 0x0000010c */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2562__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2562__W 	1784
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2562__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2562__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2562__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2562__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2562__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2562__dpitch_num_bytes 	1792
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2562__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2562__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2562__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2562__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2562__vmem_buffer_num_bytes 	1784
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2562__dram_size_num_bytes 	1792

/* mnet_ssd_adas_flex_pic_prim_split_5 SMB_input __pvcn_2565_ */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2565__cnngen_demangled_name 	"__pvcn_2565_"
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2565__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2565__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2565__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2565__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2565__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2565__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2565__is_twelve_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2565__byte_offset 	276	/* 0x00000114 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2565__vbase_byte_offset 	280	/* 0x00000118 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2565__dbase_byte_offset 	304	/* 0x00000130 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2565__dpitchm1_byte_offset 	308	/* 0x00000134 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2565__dpitchm1_bsize 	32	/* 0x00000020 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2565__W 	294912
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2565__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2565__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2565__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2565__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2565__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2565__dpitch_num_bytes 	294912
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2565__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2565__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2565__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2565__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2565__vmem_buffer_num_bytes 	294912
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2565__dram_size_num_bytes 	294912

/* mnet_ssd_adas_flex_pic_prim_split_5 SMB_input __pvcn_2566_ */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2566__cnngen_demangled_name 	"__pvcn_2566_"
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2566__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2566__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2566__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2566__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2566__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2566__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2566__is_twelve_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2566__byte_offset 	316	/* 0x0000013c */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2566__vbase_byte_offset 	320	/* 0x00000140 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2566__dbase_byte_offset 	344	/* 0x00000158 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2566__dpitchm1_byte_offset 	348	/* 0x0000015c */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2566__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2566__W 	2176
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2566__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2566__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2566__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2566__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2566__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2566__dpitch_num_bytes 	2176
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2566__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2566__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2566__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2566__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2566__vmem_buffer_num_bytes 	2176
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2566__dram_size_num_bytes 	2176

/* mnet_ssd_adas_flex_pic_prim_split_5 SMB_input __pvcn_2568_ */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2568__cnngen_demangled_name 	"__pvcn_2568_"
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2568__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2568__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2568__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2568__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2568__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2568__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2568__is_twelve_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2568__byte_offset 	356	/* 0x00000164 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2568__vbase_byte_offset 	360	/* 0x00000168 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2568__dbase_byte_offset 	384	/* 0x00000180 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2568__dpitchm1_byte_offset 	388	/* 0x00000184 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2568__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2568__W 	49664
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2568__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2568__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2568__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2568__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2568__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2568__dpitch_num_bytes 	49664
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2568__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2568__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2568__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2568__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2568__vmem_buffer_num_bytes 	49664
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2568__dram_size_num_bytes 	49664

/* mnet_ssd_adas_flex_pic_prim_split_5 SMB_input __pvcn_2569_ */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2569__cnngen_demangled_name 	"__pvcn_2569_"
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2569__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2569__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2569__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2569__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2569__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2569__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2569__is_twelve_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2569__byte_offset 	396	/* 0x0000018c */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2569__vbase_byte_offset 	400	/* 0x00000190 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2569__dbase_byte_offset 	424	/* 0x000001a8 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2569__dpitchm1_byte_offset 	428	/* 0x000001ac */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2569__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2569__W 	1712
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2569__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2569__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2569__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2569__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2569__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2569__dpitch_num_bytes 	1728
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2569__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2569__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2569__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2569__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2569__vmem_buffer_num_bytes 	1712
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2569__dram_size_num_bytes 	1728

/* mnet_ssd_adas_flex_pic_prim_split_5 SMB_input __pvcn_2572_ */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2572__cnngen_demangled_name 	"__pvcn_2572_"
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2572__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2572__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2572__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2572__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2572__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2572__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2572__is_twelve_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2572__byte_offset 	436	/* 0x000001b4 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2572__vbase_byte_offset 	440	/* 0x000001b8 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2572__dbase_byte_offset 	464	/* 0x000001d0 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2572__dpitchm1_byte_offset 	468	/* 0x000001d4 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2572__dpitchm1_bsize 	32	/* 0x00000020 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2572__W 	294912
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2572__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2572__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2572__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2572__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2572__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2572__dpitch_num_bytes 	294912
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2572__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2572__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2572__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2572__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2572__vmem_buffer_num_bytes 	294912
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2572__dram_size_num_bytes 	294912

/* mnet_ssd_adas_flex_pic_prim_split_5 SMB_input __pvcn_2573_ */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2573__cnngen_demangled_name 	"__pvcn_2573_"
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2573__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2573__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2573__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2573__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2573__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2573__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2573__is_twelve_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2573__byte_offset 	476	/* 0x000001dc */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2573__vbase_byte_offset 	480	/* 0x000001e0 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2573__dbase_byte_offset 	504	/* 0x000001f8 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2573__dpitchm1_byte_offset 	508	/* 0x000001fc */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2573__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2573__W 	2176
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2573__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2573__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2573__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2573__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2573__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2573__dpitch_num_bytes 	2176
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2573__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2573__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2573__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2573__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2573__vmem_buffer_num_bytes 	2176
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_2573__dram_size_num_bytes 	2176

/* mnet_ssd_adas_flex_pic_prim_split_5 SMB_input __pvcn_150012_ */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150012__cnngen_demangled_name 	"__pvcn_150012_"
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150012__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150012__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150012__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150012__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150012__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150012__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150012__is_twelve_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150012__byte_offset 	516	/* 0x00000204 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150012__vbase_byte_offset 	520	/* 0x00000208 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150012__dbase_byte_offset 	544	/* 0x00000220 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150012__dpitchm1_byte_offset 	548	/* 0x00000224 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150012__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150012__W 	16640
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150012__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150012__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150012__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150012__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150012__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150012__dpitch_num_bytes 	16640
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150012__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150012__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150012__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150012__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150012__vmem_buffer_num_bytes 	16640
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150012__dram_size_num_bytes 	16640

/* mnet_ssd_adas_flex_pic_prim_split_5 SMB_input __pvcn_150015_ */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150015__cnngen_demangled_name 	"__pvcn_150015_"
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150015__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150015__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150015__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150015__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150015__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150015__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150015__is_twelve_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150015__byte_offset 	556	/* 0x0000022c */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150015__vbase_byte_offset 	560	/* 0x00000230 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150015__dbase_byte_offset 	584	/* 0x00000248 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150015__dpitchm1_byte_offset 	588	/* 0x0000024c */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150015__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150015__W 	16640
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150015__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150015__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150015__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150015__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150015__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150015__dpitch_num_bytes 	16640
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150015__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150015__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150015__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150015__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150015__vmem_buffer_num_bytes 	16640
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150015__dram_size_num_bytes 	16640

/* mnet_ssd_adas_flex_pic_prim_split_5 VCB conv7_2_____bn___muli___124_____bn_____scale_mul_____scale___relu7_2 */
#define mnet_ssd_adas_flex_pic_prim_split_5_conv7_2_____bn___muli___124_____bn_____scale_mul_____scale___relu7_2_cnngen_demangled_name 	"conv7_2_____bn___muli___124_____bn_____scale_mul_____scale___relu7_2"
#define mnet_ssd_adas_flex_pic_prim_split_5_conv7_2_____bn___muli___124_____bn_____scale_mul_____scale___relu7_2_is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_conv7_2_____bn___muli___124_____bn_____scale_mul_____scale___relu7_2_is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_conv7_2_____bn___muli___124_____bn_____scale_mul_____scale___relu7_2_is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_conv7_2_____bn___muli___124_____bn_____scale_mul_____scale___relu7_2_is_twelve_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_conv7_2_____bn___muli___124_____bn_____scale_mul_____scale___relu7_2_byte_offset 	596	/* 0x00000254 */
#define mnet_ssd_adas_flex_pic_prim_split_5_conv7_2_____bn___muli___124_____bn_____scale_mul_____scale___relu7_2_vbase_byte_offset 	600	/* 0x00000258 */
#define mnet_ssd_adas_flex_pic_prim_split_5_conv7_2_____bn___muli___124_____bn_____scale_mul_____scale___relu7_2_vmem_buffer_num_bytes 	20480

/* mnet_ssd_adas_flex_pic_prim_split_5 SMB_input __pvcn_150013_ */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150013__cnngen_demangled_name 	"__pvcn_150013_"
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150013__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150013__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150013__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150013__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150013__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150013__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150013__is_twelve_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150013__byte_offset 	628	/* 0x00000274 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150013__vbase_byte_offset 	632	/* 0x00000278 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150013__dbase_byte_offset 	656	/* 0x00000290 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150013__dpitchm1_byte_offset 	660	/* 0x00000294 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150013__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150013__W 	692
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150013__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150013__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150013__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150013__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150013__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150013__dpitch_num_bytes 	704
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150013__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150013__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150013__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150013__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150013__vmem_buffer_num_bytes 	692
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150013__dram_size_num_bytes 	704

/* mnet_ssd_adas_flex_pic_prim_split_5 SMB_input __pvcn_150016_ */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150016__cnngen_demangled_name 	"__pvcn_150016_"
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150016__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150016__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150016__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150016__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150016__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150016__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150016__is_twelve_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150016__byte_offset 	668	/* 0x0000029c */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150016__vbase_byte_offset 	672	/* 0x000002a0 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150016__dbase_byte_offset 	696	/* 0x000002b8 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150016__dpitchm1_byte_offset 	700	/* 0x000002bc */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150016__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150016__W 	692
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150016__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150016__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150016__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150016__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150016__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150016__dpitch_num_bytes 	704
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150016__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150016__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150016__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150016__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150016__vmem_buffer_num_bytes 	692
#define mnet_ssd_adas_flex_pic_prim_split_5___pvcn_150016__dram_size_num_bytes 	704

/* mnet_ssd_adas_flex_pic_prim_split_5 HMB_output conv7_2_mbox_conf___conv2i___127______8_1_____bn___muli___130_____bn_____scale_mul_____scale___loc___conv2i___175___ */
#define mnet_ssd_adas_flex_pic_prim_split_5_conv7_2_mbox_conf___conv2i___127______8_1_____bn___muli___130_____bn_____scale_mul_____scale___loc___conv2i___175____cnngen_demangled_name 	"conv7_2_mbox_conf___conv2i___127______8_1_____bn___muli___130_____bn_____scale_mul_____scale___loc___conv2i___175___"
#define mnet_ssd_adas_flex_pic_prim_split_5_conv7_2_mbox_conf___conv2i___127______8_1_____bn___muli___130_____bn_____scale_mul_____scale___loc___conv2i___175____is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_conv7_2_mbox_conf___conv2i___127______8_1_____bn___muli___130_____bn_____scale_mul_____scale___loc___conv2i___175____is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_conv7_2_mbox_conf___conv2i___127______8_1_____bn___muli___130_____bn_____scale_mul_____scale___loc___conv2i___175____is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_conv7_2_mbox_conf___conv2i___127______8_1_____bn___muli___130_____bn_____scale_mul_____scale___loc___conv2i___175____is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_conv7_2_mbox_conf___conv2i___127______8_1_____bn___muli___130_____bn_____scale_mul_____scale___loc___conv2i___175____is_twelve_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_conv7_2_mbox_conf___conv2i___127______8_1_____bn___muli___130_____bn_____scale_mul_____scale___loc___conv2i___175____byte_offset 	788	/* 0x00000314 */
#define mnet_ssd_adas_flex_pic_prim_split_5_conv7_2_mbox_conf___conv2i___127______8_1_____bn___muli___130_____bn_____scale_mul_____scale___loc___conv2i___175____vbase_byte_offset 	792	/* 0x00000318 */
#define mnet_ssd_adas_flex_pic_prim_split_5_conv7_2_mbox_conf___conv2i___127______8_1_____bn___muli___130_____bn_____scale_mul_____scale___loc___conv2i___175____dbase_byte_offset 	816	/* 0x00000330 */
#define mnet_ssd_adas_flex_pic_prim_split_5_conv7_2_mbox_conf___conv2i___127______8_1_____bn___muli___130_____bn_____scale_mul_____scale___loc___conv2i___175____dpitchm1_byte_offset 	820	/* 0x00000334 */
#define mnet_ssd_adas_flex_pic_prim_split_5_conv7_2_mbox_conf___conv2i___127______8_1_____bn___muli___130_____bn_____scale_mul_____scale___loc___conv2i___175____dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5_conv7_2_mbox_conf___conv2i___127______8_1_____bn___muli___130_____bn_____scale_mul_____scale___loc___conv2i___175____vwidth_minus_one_byte_offset 	804	/* 0x00000324 */
#define mnet_ssd_adas_flex_pic_prim_split_5_conv7_2_mbox_conf___conv2i___127______8_1_____bn___muli___130_____bn_____scale_mul_____scale___loc___conv2i___175____vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5_conv7_2_mbox_conf___conv2i___127______8_1_____bn___muli___130_____bn_____scale_mul_____scale___loc___conv2i___175____vheight_minus_one_byte_offset 	806	/* 0x00000326 */
#define mnet_ssd_adas_flex_pic_prim_split_5_conv7_2_mbox_conf___conv2i___127______8_1_____bn___muli___130_____bn_____scale_mul_____scale___loc___conv2i___175____vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5_conv7_2_mbox_conf___conv2i___127______8_1_____bn___muli___130_____bn_____scale_mul_____scale___loc___conv2i___175____drotate_bit_offset 	6502	/* 0x00001966 */
#define mnet_ssd_adas_flex_pic_prim_split_5_conv7_2_mbox_conf___conv2i___127______8_1_____bn___muli___130_____bn_____scale_mul_____scale___loc___conv2i___175____hflip_bit_offset 	6503	/* 0x00001967 */
#define mnet_ssd_adas_flex_pic_prim_split_5_conv7_2_mbox_conf___conv2i___127______8_1_____bn___muli___130_____bn_____scale_mul_____scale___loc___conv2i___175____vflip_bit_offset 	6504	/* 0x00001968 */
#define mnet_ssd_adas_flex_pic_prim_split_5_conv7_2_mbox_conf___conv2i___127______8_1_____bn___muli___130_____bn_____scale_mul_____scale___loc___conv2i___175____dflip_bit_offset 	6505	/* 0x00001969 */
#define mnet_ssd_adas_flex_pic_prim_split_5_conv7_2_mbox_conf___conv2i___127______8_1_____bn___muli___130_____bn_____scale_mul_____scale___loc___conv2i___175____pflip_bit_offset 	6506	/* 0x0000196a */
#define mnet_ssd_adas_flex_pic_prim_split_5_conv7_2_mbox_conf___conv2i___127______8_1_____bn___muli___130_____bn_____scale_mul_____scale___loc___conv2i___175____W 	5
#define mnet_ssd_adas_flex_pic_prim_split_5_conv7_2_mbox_conf___conv2i___127______8_1_____bn___muli___130_____bn_____scale_mul_____scale___loc___conv2i___175____H 	5
#define mnet_ssd_adas_flex_pic_prim_split_5_conv7_2_mbox_conf___conv2i___127______8_1_____bn___muli___130_____bn_____scale_mul_____scale___loc___conv2i___175____D 	194
#define mnet_ssd_adas_flex_pic_prim_split_5_conv7_2_mbox_conf___conv2i___127______8_1_____bn___muli___130_____bn_____scale_mul_____scale___loc___conv2i___175____P 	1
#define mnet_ssd_adas_flex_pic_prim_split_5_conv7_2_mbox_conf___conv2i___127______8_1_____bn___muli___130_____bn_____scale_mul_____scale___loc___conv2i___175____data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_5_conv7_2_mbox_conf___conv2i___127______8_1_____bn___muli___130_____bn_____scale_mul_____scale___loc___conv2i___175____denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_5_conv7_2_mbox_conf___conv2i___127______8_1_____bn___muli___130_____bn_____scale_mul_____scale___loc___conv2i___175____dpitch_num_bytes 	32
#define mnet_ssd_adas_flex_pic_prim_split_5_conv7_2_mbox_conf___conv2i___127______8_1_____bn___muli___130_____bn_____scale_mul_____scale___loc___conv2i___175____dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_5_conv7_2_mbox_conf___conv2i___127______8_1_____bn___muli___130_____bn_____scale_mul_____scale___loc___conv2i___175____vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_5_conv7_2_mbox_conf___conv2i___127______8_1_____bn___muli___130_____bn_____scale_mul_____scale___loc___conv2i___175____tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_5_conv7_2_mbox_conf___conv2i___127______8_1_____bn___muli___130_____bn_____scale_mul_____scale___loc___conv2i___175____tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_5_conv7_2_mbox_conf___conv2i___127______8_1_____bn___muli___130_____bn_____scale_mul_____scale___loc___conv2i___175____vmem_buffer_num_bytes 	7760
#define mnet_ssd_adas_flex_pic_prim_split_5_conv7_2_mbox_conf___conv2i___127______8_1_____bn___muli___130_____bn_____scale_mul_____scale___loc___conv2i___175____dram_size_num_bytes 	7776
#define mnet_ssd_adas_flex_pic_prim_split_5_conv7_2_mbox_conf___conv2i___127______8_1_____bn___muli___130_____bn_____scale_mul_____scale___loc___conv2i___175____sign 	1
#define mnet_ssd_adas_flex_pic_prim_split_5_conv7_2_mbox_conf___conv2i___127______8_1_____bn___muli___130_____bn_____scale_mul_____scale___loc___conv2i___175____datasize 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_conv7_2_mbox_conf___conv2i___127______8_1_____bn___muli___130_____bn_____scale_mul_____scale___loc___conv2i___175____expoffset 	4
#define mnet_ssd_adas_flex_pic_prim_split_5_conv7_2_mbox_conf___conv2i___127______8_1_____bn___muli___130_____bn_____scale_mul_____scale___loc___conv2i___175____expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_conv7_2_mbox_conf___conv2i___127______8_1_____bn___muli___130_____bn_____scale_mul_____scale___loc___conv2i___175____drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_conv7_2_mbox_conf___conv2i___127______8_1_____bn___muli___130_____bn_____scale_mul_____scale___loc___conv2i___175____hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_conv7_2_mbox_conf___conv2i___127______8_1_____bn___muli___130_____bn_____scale_mul_____scale___loc___conv2i___175____vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_conv7_2_mbox_conf___conv2i___127______8_1_____bn___muli___130_____bn_____scale_mul_____scale___loc___conv2i___175____dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_conv7_2_mbox_conf___conv2i___127______8_1_____bn___muli___130_____bn_____scale_mul_____scale___loc___conv2i___175____pflip 	0

/* mnet_ssd_adas_flex_pic_prim_split_5 VCB relu8_1 */
#define mnet_ssd_adas_flex_pic_prim_split_5_relu8_1_cnngen_demangled_name 	"relu8_1"
#define mnet_ssd_adas_flex_pic_prim_split_5_relu8_1_is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_relu8_1_is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_relu8_1_is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_relu8_1_is_twelve_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_relu8_1_byte_offset 	840	/* 0x00000348 */
#define mnet_ssd_adas_flex_pic_prim_split_5_relu8_1_vbase_byte_offset 	844	/* 0x0000034c */
#define mnet_ssd_adas_flex_pic_prim_split_5_relu8_1_vmem_buffer_num_bytes 	5120

/* mnet_ssd_adas_flex_pic_prim_split_5 VCB conv8_2_____bn___muli___134_____bn_____scale_mul_____scale___relu8_2 */
#define mnet_ssd_adas_flex_pic_prim_split_5_conv8_2_____bn___muli___134_____bn_____scale_mul_____scale___relu8_2_cnngen_demangled_name 	"conv8_2_____bn___muli___134_____bn_____scale_mul_____scale___relu8_2"
#define mnet_ssd_adas_flex_pic_prim_split_5_conv8_2_____bn___muli___134_____bn_____scale_mul_____scale___relu8_2_is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_conv8_2_____bn___muli___134_____bn_____scale_mul_____scale___relu8_2_is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_conv8_2_____bn___muli___134_____bn_____scale_mul_____scale___relu8_2_is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_conv8_2_____bn___muli___134_____bn_____scale_mul_____scale___relu8_2_is_twelve_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_conv8_2_____bn___muli___134_____bn_____scale_mul_____scale___relu8_2_byte_offset 	884	/* 0x00000374 */
#define mnet_ssd_adas_flex_pic_prim_split_5_conv8_2_____bn___muli___134_____bn_____scale_mul_____scale___relu8_2_vbase_byte_offset 	888	/* 0x00000378 */
#define mnet_ssd_adas_flex_pic_prim_split_5_conv8_2_____bn___muli___134_____bn_____scale_mul_____scale___relu8_2_vmem_buffer_num_bytes 	3072

/* mnet_ssd_adas_flex_pic_prim_split_5 HMB_output conv8_2_mbox_conf___conv2i___137______9_1_____bn___muli___140_____bn_____scale_mul_____scale___loc___conv2i___177___ */
#define mnet_ssd_adas_flex_pic_prim_split_5_conv8_2_mbox_conf___conv2i___137______9_1_____bn___muli___140_____bn_____scale_mul_____scale___loc___conv2i___177____cnngen_demangled_name 	"conv8_2_mbox_conf___conv2i___137______9_1_____bn___muli___140_____bn_____scale_mul_____scale___loc___conv2i___177___"
#define mnet_ssd_adas_flex_pic_prim_split_5_conv8_2_mbox_conf___conv2i___137______9_1_____bn___muli___140_____bn_____scale_mul_____scale___loc___conv2i___177____is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_conv8_2_mbox_conf___conv2i___137______9_1_____bn___muli___140_____bn_____scale_mul_____scale___loc___conv2i___177____is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_conv8_2_mbox_conf___conv2i___137______9_1_____bn___muli___140_____bn_____scale_mul_____scale___loc___conv2i___177____is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_conv8_2_mbox_conf___conv2i___137______9_1_____bn___muli___140_____bn_____scale_mul_____scale___loc___conv2i___177____is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_conv8_2_mbox_conf___conv2i___137______9_1_____bn___muli___140_____bn_____scale_mul_____scale___loc___conv2i___177____is_twelve_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_conv8_2_mbox_conf___conv2i___137______9_1_____bn___muli___140_____bn_____scale_mul_____scale___loc___conv2i___177____byte_offset 	928	/* 0x000003a0 */
#define mnet_ssd_adas_flex_pic_prim_split_5_conv8_2_mbox_conf___conv2i___137______9_1_____bn___muli___140_____bn_____scale_mul_____scale___loc___conv2i___177____vbase_byte_offset 	932	/* 0x000003a4 */
#define mnet_ssd_adas_flex_pic_prim_split_5_conv8_2_mbox_conf___conv2i___137______9_1_____bn___muli___140_____bn_____scale_mul_____scale___loc___conv2i___177____dbase_byte_offset 	956	/* 0x000003bc */
#define mnet_ssd_adas_flex_pic_prim_split_5_conv8_2_mbox_conf___conv2i___137______9_1_____bn___muli___140_____bn_____scale_mul_____scale___loc___conv2i___177____dpitchm1_byte_offset 	960	/* 0x000003c0 */
#define mnet_ssd_adas_flex_pic_prim_split_5_conv8_2_mbox_conf___conv2i___137______9_1_____bn___muli___140_____bn_____scale_mul_____scale___loc___conv2i___177____dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5_conv8_2_mbox_conf___conv2i___137______9_1_____bn___muli___140_____bn_____scale_mul_____scale___loc___conv2i___177____vwidth_minus_one_byte_offset 	944	/* 0x000003b0 */
#define mnet_ssd_adas_flex_pic_prim_split_5_conv8_2_mbox_conf___conv2i___137______9_1_____bn___muli___140_____bn_____scale_mul_____scale___loc___conv2i___177____vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5_conv8_2_mbox_conf___conv2i___137______9_1_____bn___muli___140_____bn_____scale_mul_____scale___loc___conv2i___177____vheight_minus_one_byte_offset 	946	/* 0x000003b2 */
#define mnet_ssd_adas_flex_pic_prim_split_5_conv8_2_mbox_conf___conv2i___137______9_1_____bn___muli___140_____bn_____scale_mul_____scale___loc___conv2i___177____vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5_conv8_2_mbox_conf___conv2i___137______9_1_____bn___muli___140_____bn_____scale_mul_____scale___loc___conv2i___177____drotate_bit_offset 	7622	/* 0x00001dc6 */
#define mnet_ssd_adas_flex_pic_prim_split_5_conv8_2_mbox_conf___conv2i___137______9_1_____bn___muli___140_____bn_____scale_mul_____scale___loc___conv2i___177____hflip_bit_offset 	7623	/* 0x00001dc7 */
#define mnet_ssd_adas_flex_pic_prim_split_5_conv8_2_mbox_conf___conv2i___137______9_1_____bn___muli___140_____bn_____scale_mul_____scale___loc___conv2i___177____vflip_bit_offset 	7624	/* 0x00001dc8 */
#define mnet_ssd_adas_flex_pic_prim_split_5_conv8_2_mbox_conf___conv2i___137______9_1_____bn___muli___140_____bn_____scale_mul_____scale___loc___conv2i___177____dflip_bit_offset 	7625	/* 0x00001dc9 */
#define mnet_ssd_adas_flex_pic_prim_split_5_conv8_2_mbox_conf___conv2i___137______9_1_____bn___muli___140_____bn_____scale_mul_____scale___loc___conv2i___177____pflip_bit_offset 	7626	/* 0x00001dca */
#define mnet_ssd_adas_flex_pic_prim_split_5_conv8_2_mbox_conf___conv2i___137______9_1_____bn___muli___140_____bn_____scale_mul_____scale___loc___conv2i___177____W 	3
#define mnet_ssd_adas_flex_pic_prim_split_5_conv8_2_mbox_conf___conv2i___137______9_1_____bn___muli___140_____bn_____scale_mul_____scale___loc___conv2i___177____H 	3
#define mnet_ssd_adas_flex_pic_prim_split_5_conv8_2_mbox_conf___conv2i___137______9_1_____bn___muli___140_____bn_____scale_mul_____scale___loc___conv2i___177____D 	194
#define mnet_ssd_adas_flex_pic_prim_split_5_conv8_2_mbox_conf___conv2i___137______9_1_____bn___muli___140_____bn_____scale_mul_____scale___loc___conv2i___177____P 	1
#define mnet_ssd_adas_flex_pic_prim_split_5_conv8_2_mbox_conf___conv2i___137______9_1_____bn___muli___140_____bn_____scale_mul_____scale___loc___conv2i___177____data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_5_conv8_2_mbox_conf___conv2i___137______9_1_____bn___muli___140_____bn_____scale_mul_____scale___loc___conv2i___177____denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_5_conv8_2_mbox_conf___conv2i___137______9_1_____bn___muli___140_____bn_____scale_mul_____scale___loc___conv2i___177____dpitch_num_bytes 	32
#define mnet_ssd_adas_flex_pic_prim_split_5_conv8_2_mbox_conf___conv2i___137______9_1_____bn___muli___140_____bn_____scale_mul_____scale___loc___conv2i___177____dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_5_conv8_2_mbox_conf___conv2i___137______9_1_____bn___muli___140_____bn_____scale_mul_____scale___loc___conv2i___177____vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_5_conv8_2_mbox_conf___conv2i___137______9_1_____bn___muli___140_____bn_____scale_mul_____scale___loc___conv2i___177____tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_5_conv8_2_mbox_conf___conv2i___137______9_1_____bn___muli___140_____bn_____scale_mul_____scale___loc___conv2i___177____tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_5_conv8_2_mbox_conf___conv2i___137______9_1_____bn___muli___140_____bn_____scale_mul_____scale___loc___conv2i___177____vmem_buffer_num_bytes 	2328
#define mnet_ssd_adas_flex_pic_prim_split_5_conv8_2_mbox_conf___conv2i___137______9_1_____bn___muli___140_____bn_____scale_mul_____scale___loc___conv2i___177____dram_size_num_bytes 	2336
#define mnet_ssd_adas_flex_pic_prim_split_5_conv8_2_mbox_conf___conv2i___137______9_1_____bn___muli___140_____bn_____scale_mul_____scale___loc___conv2i___177____sign 	1
#define mnet_ssd_adas_flex_pic_prim_split_5_conv8_2_mbox_conf___conv2i___137______9_1_____bn___muli___140_____bn_____scale_mul_____scale___loc___conv2i___177____datasize 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_conv8_2_mbox_conf___conv2i___137______9_1_____bn___muli___140_____bn_____scale_mul_____scale___loc___conv2i___177____expoffset 	4
#define mnet_ssd_adas_flex_pic_prim_split_5_conv8_2_mbox_conf___conv2i___137______9_1_____bn___muli___140_____bn_____scale_mul_____scale___loc___conv2i___177____expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_conv8_2_mbox_conf___conv2i___137______9_1_____bn___muli___140_____bn_____scale_mul_____scale___loc___conv2i___177____drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_conv8_2_mbox_conf___conv2i___137______9_1_____bn___muli___140_____bn_____scale_mul_____scale___loc___conv2i___177____hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_conv8_2_mbox_conf___conv2i___137______9_1_____bn___muli___140_____bn_____scale_mul_____scale___loc___conv2i___177____vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_conv8_2_mbox_conf___conv2i___137______9_1_____bn___muli___140_____bn_____scale_mul_____scale___loc___conv2i___177____dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_conv8_2_mbox_conf___conv2i___137______9_1_____bn___muli___140_____bn_____scale_mul_____scale___loc___conv2i___177____pflip 	0

/* mnet_ssd_adas_flex_pic_prim_split_5 VCB relu9_1 */
#define mnet_ssd_adas_flex_pic_prim_split_5_relu9_1_cnngen_demangled_name 	"relu9_1"
#define mnet_ssd_adas_flex_pic_prim_split_5_relu9_1_is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_relu9_1_is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_relu9_1_is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_relu9_1_is_twelve_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_relu9_1_byte_offset 	980	/* 0x000003d4 */
#define mnet_ssd_adas_flex_pic_prim_split_5_relu9_1_vbase_byte_offset 	984	/* 0x000003d8 */
#define mnet_ssd_adas_flex_pic_prim_split_5_relu9_1_vmem_buffer_num_bytes 	1536

/* mnet_ssd_adas_flex_pic_prim_split_5 VCB conv9_2_____bn___muli___144_____bn_____scale_mul_____scale___relu9_2 */
#define mnet_ssd_adas_flex_pic_prim_split_5_conv9_2_____bn___muli___144_____bn_____scale_mul_____scale___relu9_2_cnngen_demangled_name 	"conv9_2_____bn___muli___144_____bn_____scale_mul_____scale___relu9_2"
#define mnet_ssd_adas_flex_pic_prim_split_5_conv9_2_____bn___muli___144_____bn_____scale_mul_____scale___relu9_2_is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_conv9_2_____bn___muli___144_____bn_____scale_mul_____scale___relu9_2_is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_conv9_2_____bn___muli___144_____bn_____scale_mul_____scale___relu9_2_is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_conv9_2_____bn___muli___144_____bn_____scale_mul_____scale___relu9_2_is_twelve_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_conv9_2_____bn___muli___144_____bn_____scale_mul_____scale___relu9_2_byte_offset 	1024	/* 0x00000400 */
#define mnet_ssd_adas_flex_pic_prim_split_5_conv9_2_____bn___muli___144_____bn_____scale_mul_____scale___relu9_2_vbase_byte_offset 	1028	/* 0x00000404 */
#define mnet_ssd_adas_flex_pic_prim_split_5_conv9_2_____bn___muli___144_____bn_____scale_mul_____scale___relu9_2_vmem_buffer_num_bytes 	4096

/* mnet_ssd_adas_flex_pic_prim_split_5 HMB_output conv9_2_mbox_conf___conv2i___147______10_1_____bn___muli___150_____bn_____scale_mul_____scale___loc___conv2i___179___ */
#define mnet_ssd_adas_flex_pic_prim_split_5_conv9_2_mbox_conf___conv2i___147______10_1_____bn___muli___150_____bn_____scale_mul_____scale___loc___conv2i___179____cnngen_demangled_name 	"conv9_2_mbox_conf___conv2i___147______10_1_____bn___muli___150_____bn_____scale_mul_____scale___loc___conv2i___179___"
#define mnet_ssd_adas_flex_pic_prim_split_5_conv9_2_mbox_conf___conv2i___147______10_1_____bn___muli___150_____bn_____scale_mul_____scale___loc___conv2i___179____is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_conv9_2_mbox_conf___conv2i___147______10_1_____bn___muli___150_____bn_____scale_mul_____scale___loc___conv2i___179____is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_conv9_2_mbox_conf___conv2i___147______10_1_____bn___muli___150_____bn_____scale_mul_____scale___loc___conv2i___179____is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_conv9_2_mbox_conf___conv2i___147______10_1_____bn___muli___150_____bn_____scale_mul_____scale___loc___conv2i___179____is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_conv9_2_mbox_conf___conv2i___147______10_1_____bn___muli___150_____bn_____scale_mul_____scale___loc___conv2i___179____is_twelve_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_conv9_2_mbox_conf___conv2i___147______10_1_____bn___muli___150_____bn_____scale_mul_____scale___loc___conv2i___179____byte_offset 	1056	/* 0x00000420 */
#define mnet_ssd_adas_flex_pic_prim_split_5_conv9_2_mbox_conf___conv2i___147______10_1_____bn___muli___150_____bn_____scale_mul_____scale___loc___conv2i___179____vbase_byte_offset 	1060	/* 0x00000424 */
#define mnet_ssd_adas_flex_pic_prim_split_5_conv9_2_mbox_conf___conv2i___147______10_1_____bn___muli___150_____bn_____scale_mul_____scale___loc___conv2i___179____dbase_byte_offset 	1084	/* 0x0000043c */
#define mnet_ssd_adas_flex_pic_prim_split_5_conv9_2_mbox_conf___conv2i___147______10_1_____bn___muli___150_____bn_____scale_mul_____scale___loc___conv2i___179____dpitchm1_byte_offset 	1088	/* 0x00000440 */
#define mnet_ssd_adas_flex_pic_prim_split_5_conv9_2_mbox_conf___conv2i___147______10_1_____bn___muli___150_____bn_____scale_mul_____scale___loc___conv2i___179____dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5_conv9_2_mbox_conf___conv2i___147______10_1_____bn___muli___150_____bn_____scale_mul_____scale___loc___conv2i___179____vwidth_minus_one_byte_offset 	1072	/* 0x00000430 */
#define mnet_ssd_adas_flex_pic_prim_split_5_conv9_2_mbox_conf___conv2i___147______10_1_____bn___muli___150_____bn_____scale_mul_____scale___loc___conv2i___179____vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5_conv9_2_mbox_conf___conv2i___147______10_1_____bn___muli___150_____bn_____scale_mul_____scale___loc___conv2i___179____vheight_minus_one_byte_offset 	1074	/* 0x00000432 */
#define mnet_ssd_adas_flex_pic_prim_split_5_conv9_2_mbox_conf___conv2i___147______10_1_____bn___muli___150_____bn_____scale_mul_____scale___loc___conv2i___179____vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5_conv9_2_mbox_conf___conv2i___147______10_1_____bn___muli___150_____bn_____scale_mul_____scale___loc___conv2i___179____drotate_bit_offset 	8646	/* 0x000021c6 */
#define mnet_ssd_adas_flex_pic_prim_split_5_conv9_2_mbox_conf___conv2i___147______10_1_____bn___muli___150_____bn_____scale_mul_____scale___loc___conv2i___179____hflip_bit_offset 	8647	/* 0x000021c7 */
#define mnet_ssd_adas_flex_pic_prim_split_5_conv9_2_mbox_conf___conv2i___147______10_1_____bn___muli___150_____bn_____scale_mul_____scale___loc___conv2i___179____vflip_bit_offset 	8648	/* 0x000021c8 */
#define mnet_ssd_adas_flex_pic_prim_split_5_conv9_2_mbox_conf___conv2i___147______10_1_____bn___muli___150_____bn_____scale_mul_____scale___loc___conv2i___179____dflip_bit_offset 	8649	/* 0x000021c9 */
#define mnet_ssd_adas_flex_pic_prim_split_5_conv9_2_mbox_conf___conv2i___147______10_1_____bn___muli___150_____bn_____scale_mul_____scale___loc___conv2i___179____pflip_bit_offset 	8650	/* 0x000021ca */
#define mnet_ssd_adas_flex_pic_prim_split_5_conv9_2_mbox_conf___conv2i___147______10_1_____bn___muli___150_____bn_____scale_mul_____scale___loc___conv2i___179____W 	2
#define mnet_ssd_adas_flex_pic_prim_split_5_conv9_2_mbox_conf___conv2i___147______10_1_____bn___muli___150_____bn_____scale_mul_____scale___loc___conv2i___179____H 	2
#define mnet_ssd_adas_flex_pic_prim_split_5_conv9_2_mbox_conf___conv2i___147______10_1_____bn___muli___150_____bn_____scale_mul_____scale___loc___conv2i___179____D 	130
#define mnet_ssd_adas_flex_pic_prim_split_5_conv9_2_mbox_conf___conv2i___147______10_1_____bn___muli___150_____bn_____scale_mul_____scale___loc___conv2i___179____P 	1
#define mnet_ssd_adas_flex_pic_prim_split_5_conv9_2_mbox_conf___conv2i___147______10_1_____bn___muli___150_____bn_____scale_mul_____scale___loc___conv2i___179____data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_5_conv9_2_mbox_conf___conv2i___147______10_1_____bn___muli___150_____bn_____scale_mul_____scale___loc___conv2i___179____denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_5_conv9_2_mbox_conf___conv2i___147______10_1_____bn___muli___150_____bn_____scale_mul_____scale___loc___conv2i___179____dpitch_num_bytes 	32
#define mnet_ssd_adas_flex_pic_prim_split_5_conv9_2_mbox_conf___conv2i___147______10_1_____bn___muli___150_____bn_____scale_mul_____scale___loc___conv2i___179____dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_5_conv9_2_mbox_conf___conv2i___147______10_1_____bn___muli___150_____bn_____scale_mul_____scale___loc___conv2i___179____vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_5_conv9_2_mbox_conf___conv2i___147______10_1_____bn___muli___150_____bn_____scale_mul_____scale___loc___conv2i___179____tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_5_conv9_2_mbox_conf___conv2i___147______10_1_____bn___muli___150_____bn_____scale_mul_____scale___loc___conv2i___179____tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_5_conv9_2_mbox_conf___conv2i___147______10_1_____bn___muli___150_____bn_____scale_mul_____scale___loc___conv2i___179____vmem_buffer_num_bytes 	2600
#define mnet_ssd_adas_flex_pic_prim_split_5_conv9_2_mbox_conf___conv2i___147______10_1_____bn___muli___150_____bn_____scale_mul_____scale___loc___conv2i___179____dram_size_num_bytes 	1056
#define mnet_ssd_adas_flex_pic_prim_split_5_conv9_2_mbox_conf___conv2i___147______10_1_____bn___muli___150_____bn_____scale_mul_____scale___loc___conv2i___179____sign 	1
#define mnet_ssd_adas_flex_pic_prim_split_5_conv9_2_mbox_conf___conv2i___147______10_1_____bn___muli___150_____bn_____scale_mul_____scale___loc___conv2i___179____datasize 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_conv9_2_mbox_conf___conv2i___147______10_1_____bn___muli___150_____bn_____scale_mul_____scale___loc___conv2i___179____expoffset 	4
#define mnet_ssd_adas_flex_pic_prim_split_5_conv9_2_mbox_conf___conv2i___147______10_1_____bn___muli___150_____bn_____scale_mul_____scale___loc___conv2i___179____expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_conv9_2_mbox_conf___conv2i___147______10_1_____bn___muli___150_____bn_____scale_mul_____scale___loc___conv2i___179____drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_conv9_2_mbox_conf___conv2i___147______10_1_____bn___muli___150_____bn_____scale_mul_____scale___loc___conv2i___179____hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_conv9_2_mbox_conf___conv2i___147______10_1_____bn___muli___150_____bn_____scale_mul_____scale___loc___conv2i___179____vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_conv9_2_mbox_conf___conv2i___147______10_1_____bn___muli___150_____bn_____scale_mul_____scale___loc___conv2i___179____dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_conv9_2_mbox_conf___conv2i___147______10_1_____bn___muli___150_____bn_____scale_mul_____scale___loc___conv2i___179____pflip 	0

/* mnet_ssd_adas_flex_pic_prim_split_5 HMB_output relu10_1 */
#define mnet_ssd_adas_flex_pic_prim_split_5_relu10_1_cnngen_demangled_name 	"relu10_1"
#define mnet_ssd_adas_flex_pic_prim_split_5_relu10_1_is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_relu10_1_is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_relu10_1_is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_relu10_1_is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_relu10_1_is_twelve_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_relu10_1_byte_offset 	1160	/* 0x00000488 */
#define mnet_ssd_adas_flex_pic_prim_split_5_relu10_1_vbase_byte_offset 	1164	/* 0x0000048c */
#define mnet_ssd_adas_flex_pic_prim_split_5_relu10_1_dbase_byte_offset 	1188	/* 0x000004a4 */
#define mnet_ssd_adas_flex_pic_prim_split_5_relu10_1_dpitchm1_byte_offset 	1192	/* 0x000004a8 */
#define mnet_ssd_adas_flex_pic_prim_split_5_relu10_1_dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5_relu10_1_vwidth_minus_one_byte_offset 	1176	/* 0x00000498 */
#define mnet_ssd_adas_flex_pic_prim_split_5_relu10_1_vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5_relu10_1_vheight_minus_one_byte_offset 	1178	/* 0x0000049a */
#define mnet_ssd_adas_flex_pic_prim_split_5_relu10_1_vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_5_relu10_1_drotate_bit_offset 	9478	/* 0x00002506 */
#define mnet_ssd_adas_flex_pic_prim_split_5_relu10_1_hflip_bit_offset 	9479	/* 0x00002507 */
#define mnet_ssd_adas_flex_pic_prim_split_5_relu10_1_vflip_bit_offset 	9480	/* 0x00002508 */
#define mnet_ssd_adas_flex_pic_prim_split_5_relu10_1_dflip_bit_offset 	9481	/* 0x00002509 */
#define mnet_ssd_adas_flex_pic_prim_split_5_relu10_1_pflip_bit_offset 	9482	/* 0x0000250a */
#define mnet_ssd_adas_flex_pic_prim_split_5_relu10_1_W 	2
#define mnet_ssd_adas_flex_pic_prim_split_5_relu10_1_H 	2
#define mnet_ssd_adas_flex_pic_prim_split_5_relu10_1_D 	64
#define mnet_ssd_adas_flex_pic_prim_split_5_relu10_1_P 	1
#define mnet_ssd_adas_flex_pic_prim_split_5_relu10_1_data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_5_relu10_1_denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_5_relu10_1_dpitch_num_bytes 	32
#define mnet_ssd_adas_flex_pic_prim_split_5_relu10_1_dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_5_relu10_1_vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_5_relu10_1_tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_5_relu10_1_tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_5_relu10_1_vmem_buffer_num_bytes 	512
#define mnet_ssd_adas_flex_pic_prim_split_5_relu10_1_dram_size_num_bytes 	512
#define mnet_ssd_adas_flex_pic_prim_split_5_relu10_1_sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_relu10_1_datasize 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_relu10_1_expoffset 	7
#define mnet_ssd_adas_flex_pic_prim_split_5_relu10_1_expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_relu10_1_drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_relu10_1_hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_relu10_1_vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_relu10_1_dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_relu10_1_pflip 	0

/* mnet_ssd_adas_flex_pic_prim_split_5 SMB_output split_5__vas_created_terminate__v0 */
#define mnet_ssd_adas_flex_pic_prim_split_5_split_5__vas_created_terminate__v0_cnngen_demangled_name 	"split_5__vas_created_terminate__v0"
#define mnet_ssd_adas_flex_pic_prim_split_5_split_5__vas_created_terminate__v0_is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_split_5__vas_created_terminate__v0_is_bitvector 	1
#define mnet_ssd_adas_flex_pic_prim_split_5_split_5__vas_created_terminate__v0_is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_split_5__vas_created_terminate__v0_is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_split_5__vas_created_terminate__v0_is_twelve_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_split_5__vas_created_terminate__v0_byte_offset 	1200	/* 0x000004b0 */
#define mnet_ssd_adas_flex_pic_prim_split_5_split_5__vas_created_terminate__v0_vbase_byte_offset 	1204	/* 0x000004b4 */
#define mnet_ssd_adas_flex_pic_prim_split_5_split_5__vas_created_terminate__v0_W 	256
#define mnet_ssd_adas_flex_pic_prim_split_5_split_5__vas_created_terminate__v0_H 	3
#define mnet_ssd_adas_flex_pic_prim_split_5_split_5__vas_created_terminate__v0_D 	1
#define mnet_ssd_adas_flex_pic_prim_split_5_split_5__vas_created_terminate__v0_P 	1
#define mnet_ssd_adas_flex_pic_prim_split_5_split_5__vas_created_terminate__v0_data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_5_split_5__vas_created_terminate__v0_denable 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_split_5__vas_created_terminate__v0_dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_5_split_5__vas_created_terminate__v0_vp_interleave_mode 	1
#define mnet_ssd_adas_flex_pic_prim_split_5_split_5__vas_created_terminate__v0_tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_5_split_5__vas_created_terminate__v0_tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_5_split_5__vas_created_terminate__v0_vmem_buffer_num_bytes 	96


#endif /* mnet_ssd_adas_flex_pic_prim_split_5_H */
