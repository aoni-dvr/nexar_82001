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
#ifndef mnet_ssd_adas_flex_pic_prim_split_25_H
#define mnet_ssd_adas_flex_pic_prim_split_25_H

#define vas_gen_header_version                  	7

/* dag name */
#define mnet_ssd_adas_flex_pic_prim_split_25_DAG_NAME 	"mnet_ssd_adas_flex_pic_prim_split_25"

/* VDG file info */
#define mnet_ssd_adas_flex_pic_prim_split_25_vdg_name 	"mnet_ssd_adas_flex_pic_split_25.vdg"

/* VDG DAG memory info */
#define mnet_ssd_adas_flex_pic_prim_split_25_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_25_byte_size 	2376	/* 0x00000948 */
#define mnet_ssd_adas_flex_pic_prim_split_25_preferred_dram_xfer_size 	1

/* mnet_ssd_adas_flex_pic_prim_split_25 VMEM info */
#define mnet_ssd_adas_flex_pic_prim_split_25_use_ping_pong_vmem 	0
#define mnet_ssd_adas_flex_pic_prim_split_25_VMEM_start 	4096	/* 0x00001000 */
#define mnet_ssd_adas_flex_pic_prim_split_25_VMEM_end 	511592	/* 0x0007ce68 */
#define mnet_ssd_adas_flex_pic_prim_split_25_image_start 	488672	/* 0x000774e0 */
#define mnet_ssd_adas_flex_pic_prim_split_25_image_size 	25296	/* 0x000062d0 */
#define mnet_ssd_adas_flex_pic_prim_split_25_dagbin_start 	511592	/* 0x0007ce68 */

/* mnet_ssd_adas_flex_pic_prim_split_25 DAG info */
#define mnet_ssd_adas_flex_pic_prim_split_25_estimated_cycles 	53811
#define mnet_ssd_adas_flex_pic_prim_split_25_ddi_byte_size 	0	/* 0x00000000 */

/*******************************************************************************
 * DAG byte offset info for HMB/SMB/VCB and their fields
 * in DAG 'mnet_ssd_adas_flex_pic_prim_split_25' in source file 'pre_split24_mnet_ssd_adas_flex_pic_split_0.vas'
 ******************************************************************************/
/* mnet_ssd_adas_flex_pic_prim_split_25 HMB_input __pvcn_2281_ */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2281__cnngen_demangled_name 	"__pvcn_2281_"
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2281__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2281__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2281__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2281__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2281__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2281__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2281__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2281__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2281__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2281__byte_offset 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2281__vbase_byte_offset 	4	/* 0x00000004 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2281__dbase_byte_offset 	28	/* 0x0000001c */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2281__dpitchm1_byte_offset 	32	/* 0x00000020 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2281__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2281__vwidth_minus_one_byte_offset 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2281__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2281__vheight_minus_one_byte_offset 	18	/* 0x00000012 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2281__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2281__drotate_bit_offset 	198	/* 0x000000c6 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2281__hflip_bit_offset 	199	/* 0x000000c7 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2281__vflip_bit_offset 	200	/* 0x000000c8 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2281__dflip_bit_offset 	201	/* 0x000000c9 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2281__pflip_bit_offset 	202	/* 0x000000ca */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2281__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2281__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2281__D 	512
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2281__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2281__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2281__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2281__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2281__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2281__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2281__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2281__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2281__vmem_buffer_num_bytes 	102400
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2281__dram_size_num_bytes 	122880
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2281__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2281__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2281__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2281__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2281__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2281__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2281__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2281__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2281__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2281__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_25 HMB_input __pvcn_2292_ */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2292__cnngen_demangled_name 	"__pvcn_2292_"
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2292__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2292__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2292__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2292__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2292__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2292__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2292__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2292__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2292__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2292__byte_offset 	36	/* 0x00000024 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2292__vbase_byte_offset 	40	/* 0x00000028 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2292__dbase_byte_offset 	64	/* 0x00000040 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2292__dpitchm1_byte_offset 	68	/* 0x00000044 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2292__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2292__vwidth_minus_one_byte_offset 	52	/* 0x00000034 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2292__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2292__vheight_minus_one_byte_offset 	54	/* 0x00000036 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2292__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2292__drotate_bit_offset 	486	/* 0x000001e6 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2292__hflip_bit_offset 	487	/* 0x000001e7 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2292__vflip_bit_offset 	488	/* 0x000001e8 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2292__dflip_bit_offset 	489	/* 0x000001e9 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2292__pflip_bit_offset 	490	/* 0x000001ea */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2292__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2292__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2292__D 	512
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2292__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2292__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2292__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2292__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2292__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2292__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2292__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2292__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2292__vmem_buffer_num_bytes 	102400
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2292__dram_size_num_bytes 	122880
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2292__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2292__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2292__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2292__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2292__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2292__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2292__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2292__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2292__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2292__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_25 SMB_input __pvcn_2363_ */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2363__cnngen_demangled_name 	"__pvcn_2363_"
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2363__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2363__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2363__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2363__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2363__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2363__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2363__byte_offset 	88	/* 0x00000058 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2363__vbase_byte_offset 	92	/* 0x0000005c */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2363__dbase_byte_offset 	116	/* 0x00000074 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2363__dpitchm1_byte_offset 	120	/* 0x00000078 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2363__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2363__W 	1060
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2363__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2363__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2363__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2363__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2363__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2363__dpitch_num_bytes 	1088
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2363__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2363__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2363__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2363__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2363__vmem_buffer_num_bytes 	1060
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2363__dram_size_num_bytes 	1088

/* mnet_ssd_adas_flex_pic_prim_split_25 SMB_input __pvcn_2364_ */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2364__cnngen_demangled_name 	"__pvcn_2364_"
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2364__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2364__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2364__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2364__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2364__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2364__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2364__byte_offset 	128	/* 0x00000080 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2364__vbase_byte_offset 	132	/* 0x00000084 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2364__dbase_byte_offset 	156	/* 0x0000009c */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2364__dpitchm1_byte_offset 	160	/* 0x000000a0 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2364__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2364__W 	672
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2364__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2364__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2364__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2364__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2364__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2364__dpitch_num_bytes 	704
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2364__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2364__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2364__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2364__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2364__vmem_buffer_num_bytes 	672
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2364__dram_size_num_bytes 	704

/* mnet_ssd_adas_flex_pic_prim_split_25 SMB_input __pvcn_2374_ */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2374__cnngen_demangled_name 	"__pvcn_2374_"
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2374__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2374__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2374__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2374__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2374__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2374__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2374__byte_offset 	168	/* 0x000000a8 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2374__vbase_byte_offset 	172	/* 0x000000ac */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2374__dbase_byte_offset 	196	/* 0x000000c4 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2374__dpitchm1_byte_offset 	200	/* 0x000000c8 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2374__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2374__W 	900
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2374__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2374__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2374__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2374__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2374__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2374__dpitch_num_bytes 	960
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2374__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2374__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2374__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2374__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2374__vmem_buffer_num_bytes 	900
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2374__dram_size_num_bytes 	960

/* mnet_ssd_adas_flex_pic_prim_split_25 SMB_input __pvcn_2375_ */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2375__cnngen_demangled_name 	"__pvcn_2375_"
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2375__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2375__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2375__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2375__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2375__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2375__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2375__byte_offset 	208	/* 0x000000d0 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2375__vbase_byte_offset 	212	/* 0x000000d4 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2375__dbase_byte_offset 	236	/* 0x000000ec */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2375__dpitchm1_byte_offset 	240	/* 0x000000f0 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2375__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2375__W 	596
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2375__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2375__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2375__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2375__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2375__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2375__dpitch_num_bytes 	640
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2375__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2375__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2375__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2375__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2375__vmem_buffer_num_bytes 	596
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2375__dram_size_num_bytes 	640

/* mnet_ssd_adas_flex_pic_prim_split_25 SMB_input __pvcn_2385_ */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2385__cnngen_demangled_name 	"__pvcn_2385_"
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2385__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2385__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2385__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2385__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2385__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2385__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2385__byte_offset 	248	/* 0x000000f8 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2385__vbase_byte_offset 	252	/* 0x000000fc */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2385__dbase_byte_offset 	276	/* 0x00000114 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2385__dpitchm1_byte_offset 	280	/* 0x00000118 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2385__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2385__W 	1044
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2385__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2385__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2385__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2385__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2385__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2385__dpitch_num_bytes 	1088
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2385__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2385__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2385__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2385__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2385__vmem_buffer_num_bytes 	1044
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2385__dram_size_num_bytes 	1088

/* mnet_ssd_adas_flex_pic_prim_split_25 SMB_input __pvcn_2386_ */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2386__cnngen_demangled_name 	"__pvcn_2386_"
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2386__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2386__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2386__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2386__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2386__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2386__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2386__byte_offset 	288	/* 0x00000120 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2386__vbase_byte_offset 	292	/* 0x00000124 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2386__dbase_byte_offset 	316	/* 0x0000013c */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2386__dpitchm1_byte_offset 	320	/* 0x00000140 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2386__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2386__W 	664
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2386__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2386__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2386__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2386__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2386__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2386__dpitch_num_bytes 	704
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2386__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2386__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2386__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2386__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2386__vmem_buffer_num_bytes 	664
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2386__dram_size_num_bytes 	704

/* mnet_ssd_adas_flex_pic_prim_split_25 SMB_input __pvcn_2396_ */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2396__cnngen_demangled_name 	"__pvcn_2396_"
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2396__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2396__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2396__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2396__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2396__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2396__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2396__byte_offset 	328	/* 0x00000148 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2396__vbase_byte_offset 	332	/* 0x0000014c */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2396__dbase_byte_offset 	356	/* 0x00000164 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2396__dpitchm1_byte_offset 	360	/* 0x00000168 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2396__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2396__W 	1016
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2396__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2396__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2396__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2396__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2396__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2396__dpitch_num_bytes 	1024
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2396__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2396__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2396__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2396__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2396__vmem_buffer_num_bytes 	1016
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2396__dram_size_num_bytes 	1024

/* mnet_ssd_adas_flex_pic_prim_split_25 SMB_input __pvcn_2397_ */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2397__cnngen_demangled_name 	"__pvcn_2397_"
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2397__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2397__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2397__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2397__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2397__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2397__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2397__byte_offset 	368	/* 0x00000170 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2397__vbase_byte_offset 	372	/* 0x00000174 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2397__dbase_byte_offset 	396	/* 0x0000018c */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2397__dpitchm1_byte_offset 	400	/* 0x00000190 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2397__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2397__W 	660
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2397__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2397__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2397__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2397__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2397__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2397__dpitch_num_bytes 	704
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2397__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2397__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2397__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2397__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2397__vmem_buffer_num_bytes 	660
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2397__dram_size_num_bytes 	704

/* mnet_ssd_adas_flex_pic_prim_split_25 SMB_input __pvcn_2407_ */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2407__cnngen_demangled_name 	"__pvcn_2407_"
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2407__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2407__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2407__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2407__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2407__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2407__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2407__byte_offset 	408	/* 0x00000198 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2407__vbase_byte_offset 	412	/* 0x0000019c */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2407__dbase_byte_offset 	436	/* 0x000001b4 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2407__dpitchm1_byte_offset 	440	/* 0x000001b8 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2407__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2407__W 	932
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2407__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2407__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2407__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2407__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2407__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2407__dpitch_num_bytes 	960
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2407__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2407__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2407__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2407__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2407__vmem_buffer_num_bytes 	932
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2407__dram_size_num_bytes 	960

/* mnet_ssd_adas_flex_pic_prim_split_25 SMB_input __pvcn_2408_ */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2408__cnngen_demangled_name 	"__pvcn_2408_"
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2408__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2408__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2408__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2408__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2408__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2408__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2408__byte_offset 	448	/* 0x000001c0 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2408__vbase_byte_offset 	452	/* 0x000001c4 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2408__dbase_byte_offset 	476	/* 0x000001dc */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2408__dpitchm1_byte_offset 	480	/* 0x000001e0 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2408__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2408__W 	604
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2408__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2408__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2408__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2408__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2408__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2408__dpitch_num_bytes 	640
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2408__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2408__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2408__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2408__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2408__vmem_buffer_num_bytes 	604
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2408__dram_size_num_bytes 	640

/* mnet_ssd_adas_flex_pic_prim_split_25 SMB_input __pvcn_2418_ */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2418__cnngen_demangled_name 	"__pvcn_2418_"
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2418__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2418__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2418__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2418__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2418__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2418__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2418__byte_offset 	488	/* 0x000001e8 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2418__vbase_byte_offset 	492	/* 0x000001ec */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2418__dbase_byte_offset 	516	/* 0x00000204 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2418__dpitchm1_byte_offset 	520	/* 0x00000208 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2418__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2418__W 	912
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2418__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2418__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2418__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2418__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2418__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2418__dpitch_num_bytes 	960
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2418__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2418__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2418__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2418__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2418__vmem_buffer_num_bytes 	912
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2418__dram_size_num_bytes 	960

/* mnet_ssd_adas_flex_pic_prim_split_25 SMB_input __pvcn_2419_ */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2419__cnngen_demangled_name 	"__pvcn_2419_"
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2419__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2419__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2419__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2419__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2419__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2419__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2419__byte_offset 	528	/* 0x00000210 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2419__vbase_byte_offset 	532	/* 0x00000214 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2419__dbase_byte_offset 	556	/* 0x0000022c */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2419__dpitchm1_byte_offset 	560	/* 0x00000230 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2419__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2419__W 	612
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2419__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2419__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2419__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2419__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2419__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2419__dpitch_num_bytes 	640
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2419__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2419__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2419__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2419__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2419__vmem_buffer_num_bytes 	612
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2419__dram_size_num_bytes 	640

/* mnet_ssd_adas_flex_pic_prim_split_25 SMB_input __pvcn_2429_ */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2429__cnngen_demangled_name 	"__pvcn_2429_"
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2429__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2429__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2429__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2429__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2429__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2429__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2429__byte_offset 	568	/* 0x00000238 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2429__vbase_byte_offset 	572	/* 0x0000023c */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2429__dbase_byte_offset 	596	/* 0x00000254 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2429__dpitchm1_byte_offset 	600	/* 0x00000258 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2429__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2429__W 	972
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2429__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2429__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2429__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2429__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2429__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2429__dpitch_num_bytes 	1024
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2429__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2429__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2429__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2429__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2429__vmem_buffer_num_bytes 	972
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2429__dram_size_num_bytes 	1024

/* mnet_ssd_adas_flex_pic_prim_split_25 SMB_input __pvcn_2430_ */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2430__cnngen_demangled_name 	"__pvcn_2430_"
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2430__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2430__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2430__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2430__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2430__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2430__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2430__byte_offset 	608	/* 0x00000260 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2430__vbase_byte_offset 	612	/* 0x00000264 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2430__dbase_byte_offset 	636	/* 0x0000027c */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2430__dpitchm1_byte_offset 	640	/* 0x00000280 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2430__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2430__W 	656
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2430__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2430__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2430__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2430__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2430__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2430__dpitch_num_bytes 	704
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2430__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2430__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2430__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2430__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2430__vmem_buffer_num_bytes 	656
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2430__dram_size_num_bytes 	704

/* mnet_ssd_adas_flex_pic_prim_split_25 SMB_input __pvcn_2440_ */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2440__cnngen_demangled_name 	"__pvcn_2440_"
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2440__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2440__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2440__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2440__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2440__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2440__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2440__byte_offset 	648	/* 0x00000288 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2440__vbase_byte_offset 	652	/* 0x0000028c */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2440__dbase_byte_offset 	676	/* 0x000002a4 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2440__dpitchm1_byte_offset 	680	/* 0x000002a8 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2440__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2440__W 	932
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2440__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2440__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2440__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2440__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2440__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2440__dpitch_num_bytes 	960
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2440__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2440__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2440__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2440__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2440__vmem_buffer_num_bytes 	932
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2440__dram_size_num_bytes 	960

/* mnet_ssd_adas_flex_pic_prim_split_25 SMB_input __pvcn_2441_ */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2441__cnngen_demangled_name 	"__pvcn_2441_"
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2441__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2441__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2441__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2441__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2441__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2441__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2441__byte_offset 	688	/* 0x000002b0 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2441__vbase_byte_offset 	692	/* 0x000002b4 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2441__dbase_byte_offset 	716	/* 0x000002cc */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2441__dpitchm1_byte_offset 	720	/* 0x000002d0 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2441__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2441__W 	604
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2441__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2441__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2441__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2441__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2441__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2441__dpitch_num_bytes 	640
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2441__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2441__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2441__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2441__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2441__vmem_buffer_num_bytes 	604
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2441__dram_size_num_bytes 	640

/* mnet_ssd_adas_flex_pic_prim_split_25 SMB_input __pvcn_2451_ */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2451__cnngen_demangled_name 	"__pvcn_2451_"
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2451__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2451__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2451__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2451__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2451__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2451__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2451__byte_offset 	728	/* 0x000002d8 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2451__vbase_byte_offset 	732	/* 0x000002dc */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2451__dbase_byte_offset 	756	/* 0x000002f4 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2451__dpitchm1_byte_offset 	760	/* 0x000002f8 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2451__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2451__W 	1012
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2451__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2451__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2451__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2451__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2451__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2451__dpitch_num_bytes 	1024
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2451__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2451__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2451__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2451__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2451__vmem_buffer_num_bytes 	1012
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2451__dram_size_num_bytes 	1024

/* mnet_ssd_adas_flex_pic_prim_split_25 SMB_input __pvcn_2452_ */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2452__cnngen_demangled_name 	"__pvcn_2452_"
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2452__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2452__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2452__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2452__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2452__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2452__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2452__byte_offset 	768	/* 0x00000300 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2452__vbase_byte_offset 	772	/* 0x00000304 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2452__dbase_byte_offset 	796	/* 0x0000031c */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2452__dpitchm1_byte_offset 	800	/* 0x00000320 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2452__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2452__W 	656
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2452__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2452__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2452__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2452__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2452__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2452__dpitch_num_bytes 	704
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2452__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2452__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2452__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2452__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2452__vmem_buffer_num_bytes 	656
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2452__dram_size_num_bytes 	704

/* mnet_ssd_adas_flex_pic_prim_split_25 SMB_input __pvcn_2462_ */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2462__cnngen_demangled_name 	"__pvcn_2462_"
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2462__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2462__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2462__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2462__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2462__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2462__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2462__byte_offset 	808	/* 0x00000328 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2462__vbase_byte_offset 	812	/* 0x0000032c */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2462__dbase_byte_offset 	836	/* 0x00000344 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2462__dpitchm1_byte_offset 	840	/* 0x00000348 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2462__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2462__W 	1012
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2462__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2462__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2462__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2462__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2462__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2462__dpitch_num_bytes 	1024
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2462__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2462__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2462__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2462__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2462__vmem_buffer_num_bytes 	1012
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2462__dram_size_num_bytes 	1024

/* mnet_ssd_adas_flex_pic_prim_split_25 SMB_input __pvcn_2463_ */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2463__cnngen_demangled_name 	"__pvcn_2463_"
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2463__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2463__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2463__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2463__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2463__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2463__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2463__byte_offset 	848	/* 0x00000350 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2463__vbase_byte_offset 	852	/* 0x00000354 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2463__dbase_byte_offset 	876	/* 0x0000036c */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2463__dpitchm1_byte_offset 	880	/* 0x00000370 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2463__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2463__W 	648
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2463__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2463__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2463__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2463__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2463__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2463__dpitch_num_bytes 	704
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2463__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2463__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2463__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2463__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2463__vmem_buffer_num_bytes 	648
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2463__dram_size_num_bytes 	704

/* mnet_ssd_adas_flex_pic_prim_split_25 SMB_input __pvcn_2473_ */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2473__cnngen_demangled_name 	"__pvcn_2473_"
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2473__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2473__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2473__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2473__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2473__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2473__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2473__byte_offset 	888	/* 0x00000378 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2473__vbase_byte_offset 	892	/* 0x0000037c */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2473__dbase_byte_offset 	916	/* 0x00000394 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2473__dpitchm1_byte_offset 	920	/* 0x00000398 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2473__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2473__W 	1000
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2473__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2473__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2473__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2473__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2473__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2473__dpitch_num_bytes 	1024
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2473__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2473__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2473__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2473__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2473__vmem_buffer_num_bytes 	1000
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2473__dram_size_num_bytes 	1024

/* mnet_ssd_adas_flex_pic_prim_split_25 SMB_input __pvcn_2474_ */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2474__cnngen_demangled_name 	"__pvcn_2474_"
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2474__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2474__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2474__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2474__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2474__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2474__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2474__byte_offset 	928	/* 0x000003a0 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2474__vbase_byte_offset 	932	/* 0x000003a4 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2474__dbase_byte_offset 	956	/* 0x000003bc */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2474__dpitchm1_byte_offset 	960	/* 0x000003c0 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2474__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2474__W 	664
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2474__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2474__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2474__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2474__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2474__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2474__dpitch_num_bytes 	704
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2474__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2474__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2474__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2474__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2474__vmem_buffer_num_bytes 	664
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2474__dram_size_num_bytes 	704

/* mnet_ssd_adas_flex_pic_prim_split_25 SMB_input __pvcn_2484_ */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2484__cnngen_demangled_name 	"__pvcn_2484_"
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2484__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2484__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2484__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2484__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2484__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2484__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2484__byte_offset 	968	/* 0x000003c8 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2484__vbase_byte_offset 	972	/* 0x000003cc */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2484__dbase_byte_offset 	996	/* 0x000003e4 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2484__dpitchm1_byte_offset 	1000	/* 0x000003e8 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2484__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2484__W 	1008
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2484__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2484__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2484__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2484__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2484__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2484__dpitch_num_bytes 	1024
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2484__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2484__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2484__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2484__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2484__vmem_buffer_num_bytes 	1008
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2484__dram_size_num_bytes 	1024

/* mnet_ssd_adas_flex_pic_prim_split_25 SMB_input __pvcn_2485_ */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2485__cnngen_demangled_name 	"__pvcn_2485_"
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2485__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2485__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2485__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2485__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2485__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2485__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2485__byte_offset 	1008	/* 0x000003f0 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2485__vbase_byte_offset 	1012	/* 0x000003f4 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2485__dbase_byte_offset 	1036	/* 0x0000040c */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2485__dpitchm1_byte_offset 	1040	/* 0x00000410 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2485__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2485__W 	640
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2485__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2485__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2485__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2485__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2485__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2485__dpitch_num_bytes 	640
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2485__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2485__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2485__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2485__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2485__vmem_buffer_num_bytes 	640
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2485__dram_size_num_bytes 	640

/* mnet_ssd_adas_flex_pic_prim_split_25 SMB_input __pvcn_2495_ */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2495__cnngen_demangled_name 	"__pvcn_2495_"
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2495__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2495__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2495__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2495__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2495__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2495__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2495__byte_offset 	1048	/* 0x00000418 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2495__vbase_byte_offset 	1052	/* 0x0000041c */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2495__dbase_byte_offset 	1076	/* 0x00000434 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2495__dpitchm1_byte_offset 	1080	/* 0x00000438 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2495__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2495__W 	1020
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2495__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2495__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2495__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2495__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2495__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2495__dpitch_num_bytes 	1024
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2495__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2495__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2495__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2495__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2495__vmem_buffer_num_bytes 	1020
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2495__dram_size_num_bytes 	1024

/* mnet_ssd_adas_flex_pic_prim_split_25 SMB_input __pvcn_2496_ */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2496__cnngen_demangled_name 	"__pvcn_2496_"
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2496__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2496__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2496__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2496__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2496__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2496__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2496__byte_offset 	1088	/* 0x00000440 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2496__vbase_byte_offset 	1092	/* 0x00000444 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2496__dbase_byte_offset 	1116	/* 0x0000045c */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2496__dpitchm1_byte_offset 	1120	/* 0x00000460 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2496__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2496__W 	656
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2496__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2496__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2496__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2496__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2496__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2496__dpitch_num_bytes 	704
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2496__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2496__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2496__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2496__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2496__vmem_buffer_num_bytes 	656
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2496__dram_size_num_bytes 	704

/* mnet_ssd_adas_flex_pic_prim_split_25 SMB_input __pvcn_2506_ */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2506__cnngen_demangled_name 	"__pvcn_2506_"
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2506__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2506__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2506__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2506__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2506__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2506__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2506__byte_offset 	1128	/* 0x00000468 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2506__vbase_byte_offset 	1132	/* 0x0000046c */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2506__dbase_byte_offset 	1156	/* 0x00000484 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2506__dpitchm1_byte_offset 	1160	/* 0x00000488 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2506__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2506__W 	984
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2506__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2506__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2506__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2506__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2506__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2506__dpitch_num_bytes 	1024
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2506__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2506__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2506__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2506__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2506__vmem_buffer_num_bytes 	984
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2506__dram_size_num_bytes 	1024

/* mnet_ssd_adas_flex_pic_prim_split_25 SMB_input __pvcn_2507_ */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2507__cnngen_demangled_name 	"__pvcn_2507_"
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2507__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2507__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2507__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2507__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2507__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2507__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2507__byte_offset 	1168	/* 0x00000490 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2507__vbase_byte_offset 	1172	/* 0x00000494 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2507__dbase_byte_offset 	1196	/* 0x000004ac */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2507__dpitchm1_byte_offset 	1200	/* 0x000004b0 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2507__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2507__W 	640
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2507__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2507__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2507__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2507__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2507__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2507__dpitch_num_bytes 	640
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2507__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2507__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2507__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2507__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2507__vmem_buffer_num_bytes 	640
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2507__dram_size_num_bytes 	640

/* mnet_ssd_adas_flex_pic_prim_split_25 SMB_input __pvcn_2517_ */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2517__cnngen_demangled_name 	"__pvcn_2517_"
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2517__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2517__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2517__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2517__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2517__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2517__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2517__byte_offset 	1208	/* 0x000004b8 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2517__vbase_byte_offset 	1212	/* 0x000004bc */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2517__dbase_byte_offset 	1236	/* 0x000004d4 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2517__dpitchm1_byte_offset 	1240	/* 0x000004d8 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2517__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2517__W 	84
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2517__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2517__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2517__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2517__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2517__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2517__dpitch_num_bytes 	128
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2517__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2517__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2517__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2517__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2517__vmem_buffer_num_bytes 	84
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2517__dram_size_num_bytes 	128

/* mnet_ssd_adas_flex_pic_prim_split_25 SMB_input __pvcn_2518_ */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2518__cnngen_demangled_name 	"__pvcn_2518_"
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2518__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2518__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2518__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2518__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2518__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2518__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2518__byte_offset 	1248	/* 0x000004e0 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2518__vbase_byte_offset 	1252	/* 0x000004e4 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2518__dbase_byte_offset 	1276	/* 0x000004fc */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2518__dpitchm1_byte_offset 	1280	/* 0x00000500 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2518__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2518__W 	60
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2518__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2518__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2518__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2518__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2518__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2518__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2518__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2518__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2518__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2518__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2518__vmem_buffer_num_bytes 	60
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2518__dram_size_num_bytes 	64

/* mnet_ssd_adas_flex_pic_prim_split_25 VCB conv5_6__sep_____bn_conv5_6__sep__scale_mul___muli___82_____bn_conv5_6__sep__scale_mul_____scale___muli___359 */
#define mnet_ssd_adas_flex_pic_prim_split_25_conv5_6__sep_____bn_conv5_6__sep__scale_mul___muli___82_____bn_conv5_6__sep__scale_mul_____scale___muli___359_cnngen_demangled_name 	"conv5_6__sep_____bn_conv5_6__sep__scale_mul___muli___82_____bn_conv5_6__sep__scale_mul_____scale___muli___359"
#define mnet_ssd_adas_flex_pic_prim_split_25_conv5_6__sep_____bn_conv5_6__sep__scale_mul___muli___82_____bn_conv5_6__sep__scale_mul_____scale___muli___359_is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_25_conv5_6__sep_____bn_conv5_6__sep__scale_mul___muli___82_____bn_conv5_6__sep__scale_mul_____scale___muli___359_is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_25_conv5_6__sep_____bn_conv5_6__sep__scale_mul___muli___82_____bn_conv5_6__sep__scale_mul_____scale___muli___359_is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_25_conv5_6__sep_____bn_conv5_6__sep__scale_mul___muli___82_____bn_conv5_6__sep__scale_mul_____scale___muli___359_byte_offset 	1404	/* 0x0000057c */
#define mnet_ssd_adas_flex_pic_prim_split_25_conv5_6__sep_____bn_conv5_6__sep__scale_mul___muli___82_____bn_conv5_6__sep__scale_mul_____scale___muli___359_vbase_byte_offset 	1412	/* 0x00000584 */
#define mnet_ssd_adas_flex_pic_prim_split_25_conv5_6__sep_____bn_conv5_6__sep__scale_mul___muli___82_____bn_conv5_6__sep__scale_mul_____scale___muli___359_vmem_buffer_num_bytes 	81920

/* mnet_ssd_adas_flex_pic_prim_split_25 HMB_output __pvcn_2365_ */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2365__cnngen_demangled_name 	"__pvcn_2365_"
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2365__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2365__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2365__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2365__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2365__byte_offset 	1476	/* 0x000005c4 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2365__vbase_byte_offset 	1480	/* 0x000005c8 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2365__dbase_byte_offset 	1504	/* 0x000005e0 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2365__dpitchm1_byte_offset 	1508	/* 0x000005e4 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2365__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2365__vwidth_minus_one_byte_offset 	1492	/* 0x000005d4 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2365__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2365__vheight_minus_one_byte_offset 	1494	/* 0x000005d6 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2365__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2365__drotate_bit_offset 	12006	/* 0x00002ee6 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2365__hflip_bit_offset 	12007	/* 0x00002ee7 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2365__vflip_bit_offset 	12008	/* 0x00002ee8 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2365__dflip_bit_offset 	12009	/* 0x00002ee9 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2365__pflip_bit_offset 	12010	/* 0x00002eea */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2365__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2365__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2365__D 	78
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2365__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2365__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2365__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2365__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2365__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2365__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2365__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2365__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2365__vmem_buffer_num_bytes 	15600
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2365__dram_size_num_bytes 	18720
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2365__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2365__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2365__expoffset 	12
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2365__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2365__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2365__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2365__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2365__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2365__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2365__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_25 HMB_output __pvcn_2376_ */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2376__cnngen_demangled_name 	"__pvcn_2376_"
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2376__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2376__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2376__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2376__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2376__byte_offset 	1532	/* 0x000005fc */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2376__vbase_byte_offset 	1536	/* 0x00000600 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2376__dbase_byte_offset 	1560	/* 0x00000618 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2376__dpitchm1_byte_offset 	1564	/* 0x0000061c */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2376__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2376__vwidth_minus_one_byte_offset 	1548	/* 0x0000060c */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2376__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2376__vheight_minus_one_byte_offset 	1550	/* 0x0000060e */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2376__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2376__drotate_bit_offset 	12454	/* 0x000030a6 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2376__hflip_bit_offset 	12455	/* 0x000030a7 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2376__vflip_bit_offset 	12456	/* 0x000030a8 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2376__dflip_bit_offset 	12457	/* 0x000030a9 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2376__pflip_bit_offset 	12458	/* 0x000030aa */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2376__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2376__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2376__D 	69
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2376__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2376__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2376__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2376__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2376__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2376__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2376__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2376__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2376__vmem_buffer_num_bytes 	13800
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2376__dram_size_num_bytes 	16576
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2376__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2376__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2376__expoffset 	12
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2376__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2376__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2376__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2376__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2376__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2376__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2376__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_25 HMB_output __pvcn_2387_ */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2387__cnngen_demangled_name 	"__pvcn_2387_"
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2387__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2387__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2387__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2387__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2387__byte_offset 	1588	/* 0x00000634 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2387__vbase_byte_offset 	1592	/* 0x00000638 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2387__dbase_byte_offset 	1616	/* 0x00000650 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2387__dpitchm1_byte_offset 	1620	/* 0x00000654 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2387__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2387__vwidth_minus_one_byte_offset 	1604	/* 0x00000644 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2387__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2387__vheight_minus_one_byte_offset 	1606	/* 0x00000646 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2387__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2387__drotate_bit_offset 	12902	/* 0x00003266 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2387__hflip_bit_offset 	12903	/* 0x00003267 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2387__vflip_bit_offset 	12904	/* 0x00003268 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2387__dflip_bit_offset 	12905	/* 0x00003269 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2387__pflip_bit_offset 	12906	/* 0x0000326a */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2387__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2387__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2387__D 	75
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2387__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2387__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2387__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2387__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2387__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2387__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2387__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2387__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2387__vmem_buffer_num_bytes 	15000
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2387__dram_size_num_bytes 	18016
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2387__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2387__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2387__expoffset 	12
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2387__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2387__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2387__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2387__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2387__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2387__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2387__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_25 HMB_output __pvcn_2398_ */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2398__cnngen_demangled_name 	"__pvcn_2398_"
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2398__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2398__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2398__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2398__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2398__byte_offset 	1644	/* 0x0000066c */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2398__vbase_byte_offset 	1648	/* 0x00000670 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2398__dbase_byte_offset 	1672	/* 0x00000688 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2398__dpitchm1_byte_offset 	1676	/* 0x0000068c */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2398__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2398__vwidth_minus_one_byte_offset 	1660	/* 0x0000067c */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2398__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2398__vheight_minus_one_byte_offset 	1662	/* 0x0000067e */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2398__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2398__drotate_bit_offset 	13350	/* 0x00003426 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2398__hflip_bit_offset 	13351	/* 0x00003427 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2398__vflip_bit_offset 	13352	/* 0x00003428 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2398__dflip_bit_offset 	13353	/* 0x00003429 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2398__pflip_bit_offset 	13354	/* 0x0000342a */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2398__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2398__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2398__D 	75
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2398__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2398__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2398__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2398__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2398__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2398__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2398__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2398__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2398__vmem_buffer_num_bytes 	15000
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2398__dram_size_num_bytes 	18016
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2398__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2398__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2398__expoffset 	12
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2398__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2398__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2398__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2398__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2398__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2398__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2398__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_25 HMB_output __pvcn_2409_ */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2409__cnngen_demangled_name 	"__pvcn_2409_"
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2409__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2409__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2409__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2409__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2409__byte_offset 	1700	/* 0x000006a4 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2409__vbase_byte_offset 	1704	/* 0x000006a8 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2409__dbase_byte_offset 	1728	/* 0x000006c0 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2409__dpitchm1_byte_offset 	1732	/* 0x000006c4 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2409__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2409__vwidth_minus_one_byte_offset 	1716	/* 0x000006b4 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2409__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2409__vheight_minus_one_byte_offset 	1718	/* 0x000006b6 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2409__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2409__drotate_bit_offset 	13798	/* 0x000035e6 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2409__hflip_bit_offset 	13799	/* 0x000035e7 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2409__vflip_bit_offset 	13800	/* 0x000035e8 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2409__dflip_bit_offset 	13801	/* 0x000035e9 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2409__pflip_bit_offset 	13802	/* 0x000035ea */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2409__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2409__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2409__D 	70
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2409__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2409__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2409__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2409__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2409__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2409__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2409__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2409__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2409__vmem_buffer_num_bytes 	14000
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2409__dram_size_num_bytes 	16800
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2409__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2409__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2409__expoffset 	12
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2409__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2409__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2409__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2409__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2409__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2409__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2409__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_25 HMB_output __pvcn_2420_ */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2420__cnngen_demangled_name 	"__pvcn_2420_"
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2420__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2420__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2420__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2420__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2420__byte_offset 	1756	/* 0x000006dc */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2420__vbase_byte_offset 	1760	/* 0x000006e0 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2420__dbase_byte_offset 	1784	/* 0x000006f8 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2420__dpitchm1_byte_offset 	1788	/* 0x000006fc */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2420__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2420__vwidth_minus_one_byte_offset 	1772	/* 0x000006ec */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2420__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2420__vheight_minus_one_byte_offset 	1774	/* 0x000006ee */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2420__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2420__drotate_bit_offset 	14246	/* 0x000037a6 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2420__hflip_bit_offset 	14247	/* 0x000037a7 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2420__vflip_bit_offset 	14248	/* 0x000037a8 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2420__dflip_bit_offset 	14249	/* 0x000037a9 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2420__pflip_bit_offset 	14250	/* 0x000037aa */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2420__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2420__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2420__D 	72
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2420__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2420__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2420__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2420__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2420__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2420__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2420__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2420__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2420__vmem_buffer_num_bytes 	14400
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2420__dram_size_num_bytes 	17280
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2420__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2420__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2420__expoffset 	12
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2420__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2420__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2420__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2420__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2420__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2420__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2420__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_25 HMB_output __pvcn_2431_ */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2431__cnngen_demangled_name 	"__pvcn_2431_"
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2431__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2431__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2431__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2431__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2431__byte_offset 	1812	/* 0x00000714 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2431__vbase_byte_offset 	1816	/* 0x00000718 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2431__dbase_byte_offset 	1840	/* 0x00000730 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2431__dpitchm1_byte_offset 	1844	/* 0x00000734 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2431__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2431__vwidth_minus_one_byte_offset 	1828	/* 0x00000724 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2431__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2431__vheight_minus_one_byte_offset 	1830	/* 0x00000726 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2431__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2431__drotate_bit_offset 	14694	/* 0x00003966 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2431__hflip_bit_offset 	14695	/* 0x00003967 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2431__vflip_bit_offset 	14696	/* 0x00003968 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2431__dflip_bit_offset 	14697	/* 0x00003969 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2431__pflip_bit_offset 	14698	/* 0x0000396a */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2431__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2431__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2431__D 	73
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2431__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2431__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2431__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2431__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2431__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2431__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2431__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2431__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2431__vmem_buffer_num_bytes 	14600
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2431__dram_size_num_bytes 	17536
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2431__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2431__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2431__expoffset 	12
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2431__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2431__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2431__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2431__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2431__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2431__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2431__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_25 HMB_output __pvcn_2442_ */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2442__cnngen_demangled_name 	"__pvcn_2442_"
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2442__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2442__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2442__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2442__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2442__byte_offset 	1868	/* 0x0000074c */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2442__vbase_byte_offset 	1872	/* 0x00000750 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2442__dbase_byte_offset 	1896	/* 0x00000768 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2442__dpitchm1_byte_offset 	1900	/* 0x0000076c */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2442__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2442__vwidth_minus_one_byte_offset 	1884	/* 0x0000075c */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2442__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2442__vheight_minus_one_byte_offset 	1886	/* 0x0000075e */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2442__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2442__drotate_bit_offset 	15142	/* 0x00003b26 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2442__hflip_bit_offset 	15143	/* 0x00003b27 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2442__vflip_bit_offset 	15144	/* 0x00003b28 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2442__dflip_bit_offset 	15145	/* 0x00003b29 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2442__pflip_bit_offset 	15146	/* 0x00003b2a */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2442__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2442__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2442__D 	70
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2442__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2442__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2442__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2442__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2442__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2442__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2442__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2442__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2442__vmem_buffer_num_bytes 	14000
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2442__dram_size_num_bytes 	16800
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2442__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2442__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2442__expoffset 	12
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2442__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2442__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2442__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2442__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2442__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2442__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2442__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_25 HMB_output __pvcn_2453_ */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2453__cnngen_demangled_name 	"__pvcn_2453_"
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2453__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2453__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2453__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2453__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2453__byte_offset 	1924	/* 0x00000784 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2453__vbase_byte_offset 	1928	/* 0x00000788 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2453__dbase_byte_offset 	1952	/* 0x000007a0 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2453__dpitchm1_byte_offset 	1956	/* 0x000007a4 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2453__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2453__vwidth_minus_one_byte_offset 	1940	/* 0x00000794 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2453__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2453__vheight_minus_one_byte_offset 	1942	/* 0x00000796 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2453__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2453__drotate_bit_offset 	15590	/* 0x00003ce6 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2453__hflip_bit_offset 	15591	/* 0x00003ce7 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2453__vflip_bit_offset 	15592	/* 0x00003ce8 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2453__dflip_bit_offset 	15593	/* 0x00003ce9 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2453__pflip_bit_offset 	15594	/* 0x00003cea */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2453__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2453__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2453__D 	74
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2453__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2453__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2453__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2453__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2453__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2453__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2453__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2453__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2453__vmem_buffer_num_bytes 	14800
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2453__dram_size_num_bytes 	17760
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2453__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2453__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2453__expoffset 	12
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2453__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2453__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2453__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2453__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2453__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2453__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2453__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_25 HMB_output __pvcn_2464_ */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2464__cnngen_demangled_name 	"__pvcn_2464_"
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2464__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2464__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2464__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2464__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2464__byte_offset 	1980	/* 0x000007bc */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2464__vbase_byte_offset 	1984	/* 0x000007c0 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2464__dbase_byte_offset 	2008	/* 0x000007d8 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2464__dpitchm1_byte_offset 	2012	/* 0x000007dc */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2464__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2464__vwidth_minus_one_byte_offset 	1996	/* 0x000007cc */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2464__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2464__vheight_minus_one_byte_offset 	1998	/* 0x000007ce */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2464__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2464__drotate_bit_offset 	16038	/* 0x00003ea6 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2464__hflip_bit_offset 	16039	/* 0x00003ea7 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2464__vflip_bit_offset 	16040	/* 0x00003ea8 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2464__dflip_bit_offset 	16041	/* 0x00003ea9 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2464__pflip_bit_offset 	16042	/* 0x00003eaa */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2464__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2464__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2464__D 	72
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2464__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2464__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2464__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2464__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2464__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2464__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2464__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2464__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2464__vmem_buffer_num_bytes 	14400
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2464__dram_size_num_bytes 	17280
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2464__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2464__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2464__expoffset 	12
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2464__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2464__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2464__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2464__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2464__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2464__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2464__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_25 HMB_output __pvcn_2475_ */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2475__cnngen_demangled_name 	"__pvcn_2475_"
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2475__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2475__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2475__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2475__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2475__byte_offset 	2036	/* 0x000007f4 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2475__vbase_byte_offset 	2040	/* 0x000007f8 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2475__dbase_byte_offset 	2064	/* 0x00000810 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2475__dpitchm1_byte_offset 	2068	/* 0x00000814 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2475__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2475__vwidth_minus_one_byte_offset 	2052	/* 0x00000804 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2475__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2475__vheight_minus_one_byte_offset 	2054	/* 0x00000806 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2475__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2475__drotate_bit_offset 	16486	/* 0x00004066 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2475__hflip_bit_offset 	16487	/* 0x00004067 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2475__vflip_bit_offset 	16488	/* 0x00004068 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2475__dflip_bit_offset 	16489	/* 0x00004069 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2475__pflip_bit_offset 	16490	/* 0x0000406a */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2475__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2475__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2475__D 	76
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2475__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2475__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2475__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2475__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2475__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2475__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2475__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2475__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2475__vmem_buffer_num_bytes 	15200
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2475__dram_size_num_bytes 	18240
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2475__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2475__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2475__expoffset 	12
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2475__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2475__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2475__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2475__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2475__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2475__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2475__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_25 HMB_output __pvcn_2486_ */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2486__cnngen_demangled_name 	"__pvcn_2486_"
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2486__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2486__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2486__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2486__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2486__byte_offset 	2092	/* 0x0000082c */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2486__vbase_byte_offset 	2096	/* 0x00000830 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2486__dbase_byte_offset 	2120	/* 0x00000848 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2486__dpitchm1_byte_offset 	2124	/* 0x0000084c */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2486__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2486__vwidth_minus_one_byte_offset 	2108	/* 0x0000083c */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2486__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2486__vheight_minus_one_byte_offset 	2110	/* 0x0000083e */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2486__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2486__drotate_bit_offset 	16934	/* 0x00004226 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2486__hflip_bit_offset 	16935	/* 0x00004227 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2486__vflip_bit_offset 	16936	/* 0x00004228 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2486__dflip_bit_offset 	16937	/* 0x00004229 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2486__pflip_bit_offset 	16938	/* 0x0000422a */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2486__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2486__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2486__D 	71
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2486__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2486__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2486__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2486__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2486__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2486__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2486__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2486__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2486__vmem_buffer_num_bytes 	14200
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2486__dram_size_num_bytes 	17056
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2486__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2486__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2486__expoffset 	12
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2486__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2486__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2486__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2486__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2486__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2486__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2486__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_25 HMB_output __pvcn_2497_ */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2497__cnngen_demangled_name 	"__pvcn_2497_"
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2497__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2497__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2497__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2497__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2497__byte_offset 	2148	/* 0x00000864 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2497__vbase_byte_offset 	2152	/* 0x00000868 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2497__dbase_byte_offset 	2176	/* 0x00000880 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2497__dpitchm1_byte_offset 	2180	/* 0x00000884 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2497__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2497__vwidth_minus_one_byte_offset 	2164	/* 0x00000874 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2497__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2497__vheight_minus_one_byte_offset 	2166	/* 0x00000876 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2497__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2497__drotate_bit_offset 	17382	/* 0x000043e6 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2497__hflip_bit_offset 	17383	/* 0x000043e7 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2497__vflip_bit_offset 	17384	/* 0x000043e8 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2497__dflip_bit_offset 	17385	/* 0x000043e9 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2497__pflip_bit_offset 	17386	/* 0x000043ea */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2497__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2497__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2497__D 	73
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2497__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2497__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2497__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2497__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2497__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2497__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2497__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2497__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2497__vmem_buffer_num_bytes 	14600
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2497__dram_size_num_bytes 	17536
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2497__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2497__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2497__expoffset 	12
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2497__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2497__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2497__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2497__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2497__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2497__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2497__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_25 HMB_output __pvcn_2508_ */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2508__cnngen_demangled_name 	"__pvcn_2508_"
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2508__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2508__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2508__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2508__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2508__byte_offset 	2204	/* 0x0000089c */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2508__vbase_byte_offset 	2208	/* 0x000008a0 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2508__dbase_byte_offset 	2232	/* 0x000008b8 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2508__dpitchm1_byte_offset 	2236	/* 0x000008bc */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2508__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2508__vwidth_minus_one_byte_offset 	2220	/* 0x000008ac */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2508__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2508__vheight_minus_one_byte_offset 	2222	/* 0x000008ae */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2508__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2508__drotate_bit_offset 	17830	/* 0x000045a6 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2508__hflip_bit_offset 	17831	/* 0x000045a7 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2508__vflip_bit_offset 	17832	/* 0x000045a8 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2508__dflip_bit_offset 	17833	/* 0x000045a9 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2508__pflip_bit_offset 	17834	/* 0x000045aa */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2508__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2508__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2508__D 	70
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2508__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2508__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2508__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2508__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2508__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2508__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2508__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2508__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2508__vmem_buffer_num_bytes 	6720
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2508__dram_size_num_bytes 	16800
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2508__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2508__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2508__expoffset 	12
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2508__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2508__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2508__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2508__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2508__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2508__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2508__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_25 HMB_output __pvcn_2519_ */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2519__cnngen_demangled_name 	"__pvcn_2519_"
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2519__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2519__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2519__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2519__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2519__byte_offset 	2260	/* 0x000008d4 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2519__vbase_byte_offset 	2264	/* 0x000008d8 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2519__dbase_byte_offset 	2288	/* 0x000008f0 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2519__dpitchm1_byte_offset 	2292	/* 0x000008f4 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2519__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2519__vwidth_minus_one_byte_offset 	2276	/* 0x000008e4 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2519__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2519__vheight_minus_one_byte_offset 	2278	/* 0x000008e6 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2519__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2519__drotate_bit_offset 	18278	/* 0x00004766 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2519__hflip_bit_offset 	18279	/* 0x00004767 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2519__vflip_bit_offset 	18280	/* 0x00004768 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2519__dflip_bit_offset 	18281	/* 0x00004769 */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2519__pflip_bit_offset 	18282	/* 0x0000476a */
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2519__W 	10
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2519__H 	10
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2519__D 	6
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2519__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2519__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2519__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2519__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2519__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2519__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2519__tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2519__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2519__vmem_buffer_num_bytes 	1440
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2519__dram_size_num_bytes 	1440
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2519__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2519__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2519__expoffset 	12
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2519__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2519__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2519__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2519__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2519__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2519__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___pvcn_2519__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_25 SMB_output __vas_created_terminate__v0 */
#define mnet_ssd_adas_flex_pic_prim_split_25___vas_created_terminate__v0_cnngen_demangled_name 	"__vas_created_terminate__v0"
#define mnet_ssd_adas_flex_pic_prim_split_25___vas_created_terminate__v0_is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___vas_created_terminate__v0_is_bitvector 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___vas_created_terminate__v0_is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___vas_created_terminate__v0_is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___vas_created_terminate__v0_byte_offset 	2300	/* 0x000008fc */
#define mnet_ssd_adas_flex_pic_prim_split_25___vas_created_terminate__v0_vbase_byte_offset 	2304	/* 0x00000900 */
#define mnet_ssd_adas_flex_pic_prim_split_25___vas_created_terminate__v0_W 	256
#define mnet_ssd_adas_flex_pic_prim_split_25___vas_created_terminate__v0_H 	3
#define mnet_ssd_adas_flex_pic_prim_split_25___vas_created_terminate__v0_D 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___vas_created_terminate__v0_P 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___vas_created_terminate__v0_data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___vas_created_terminate__v0_denable 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___vas_created_terminate__v0_dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_25___vas_created_terminate__v0_vp_interleave_mode 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___vas_created_terminate__v0_tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___vas_created_terminate__v0_tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_25___vas_created_terminate__v0_vmem_buffer_num_bytes 	96


#endif /* mnet_ssd_adas_flex_pic_prim_split_25_H */
