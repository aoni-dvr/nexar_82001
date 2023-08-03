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
#ifndef mnet_ssd_adas_flex_pic_prim_split_15_H
#define mnet_ssd_adas_flex_pic_prim_split_15_H

#define vas_gen_header_version                  	7

/* dag name */
#define mnet_ssd_adas_flex_pic_prim_split_15_DAG_NAME 	"mnet_ssd_adas_flex_pic_prim_split_15"

/* VDG file info */
#define mnet_ssd_adas_flex_pic_prim_split_15_vdg_name 	"mnet_ssd_adas_flex_pic_split_15.vdg"

/* VDG DAG memory info */
#define mnet_ssd_adas_flex_pic_prim_split_15_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_15_byte_size 	2132	/* 0x00000854 */
#define mnet_ssd_adas_flex_pic_prim_split_15_preferred_dram_xfer_size 	1

/* mnet_ssd_adas_flex_pic_prim_split_15 VMEM info */
#define mnet_ssd_adas_flex_pic_prim_split_15_use_ping_pong_vmem 	0
#define mnet_ssd_adas_flex_pic_prim_split_15_VMEM_start 	4096	/* 0x00001000 */
#define mnet_ssd_adas_flex_pic_prim_split_15_VMEM_end 	509360	/* 0x0007c5b0 */
#define mnet_ssd_adas_flex_pic_prim_split_15_image_start 	497120	/* 0x000795e0 */
#define mnet_ssd_adas_flex_pic_prim_split_15_image_size 	14372	/* 0x00003824 */
#define mnet_ssd_adas_flex_pic_prim_split_15_dagbin_start 	509360	/* 0x0007c5b0 */

/* mnet_ssd_adas_flex_pic_prim_split_15 DAG info */
#define mnet_ssd_adas_flex_pic_prim_split_15_estimated_cycles 	75445
#define mnet_ssd_adas_flex_pic_prim_split_15_ddi_byte_size 	0	/* 0x00000000 */

/*******************************************************************************
 * DAG byte offset info for HMB/SMB/VCB and their fields
 * in DAG 'mnet_ssd_adas_flex_pic_prim_split_15' in source file 'pre_split15_mnet_ssd_adas_flex_pic_split_0.vas'
 ******************************************************************************/
/* mnet_ssd_adas_flex_pic_prim_split_15 HMB_input __pvcn_1626_ */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1626__cnngen_demangled_name 	"__pvcn_1626_"
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1626__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1626__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1626__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1626__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1626__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1626__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1626__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1626__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1626__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1626__byte_offset 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1626__vbase_byte_offset 	4	/* 0x00000004 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1626__dbase_byte_offset 	28	/* 0x0000001c */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1626__dpitchm1_byte_offset 	32	/* 0x00000020 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1626__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1626__vwidth_minus_one_byte_offset 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1626__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1626__vheight_minus_one_byte_offset 	18	/* 0x00000012 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1626__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1626__drotate_bit_offset 	198	/* 0x000000c6 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1626__hflip_bit_offset 	199	/* 0x000000c7 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1626__vflip_bit_offset 	200	/* 0x000000c8 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1626__dflip_bit_offset 	201	/* 0x000000c9 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1626__pflip_bit_offset 	202	/* 0x000000ca */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1626__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1626__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1626__D 	512
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1626__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1626__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1626__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1626__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1626__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1626__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1626__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1626__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1626__vmem_buffer_num_bytes 	16384
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1626__dram_size_num_bytes 	466944
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1626__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1626__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1626__expoffset 	13
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1626__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1626__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1626__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1626__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1626__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1626__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1626__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_15 SMB_input __pvcn_1668_ */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1668__cnngen_demangled_name 	"__pvcn_1668_"
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1668__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1668__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1668__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1668__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1668__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1668__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1668__byte_offset 	48	/* 0x00000030 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1668__vbase_byte_offset 	52	/* 0x00000034 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1668__dbase_byte_offset 	76	/* 0x0000004c */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1668__dpitchm1_byte_offset 	80	/* 0x00000050 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1668__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1668__W 	468
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1668__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1668__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1668__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1668__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1668__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1668__dpitch_num_bytes 	512
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1668__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1668__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1668__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1668__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1668__vmem_buffer_num_bytes 	468
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1668__dram_size_num_bytes 	512

/* mnet_ssd_adas_flex_pic_prim_split_15 SMB_input __pvcn_1669_ */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1669__cnngen_demangled_name 	"__pvcn_1669_"
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1669__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1669__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1669__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1669__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1669__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1669__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1669__byte_offset 	88	/* 0x00000058 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1669__vbase_byte_offset 	92	/* 0x0000005c */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1669__dbase_byte_offset 	116	/* 0x00000074 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1669__dpitchm1_byte_offset 	120	/* 0x00000078 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1669__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1669__W 	336
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1669__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1669__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1669__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1669__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1669__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1669__dpitch_num_bytes 	384
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1669__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1669__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1669__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1669__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1669__vmem_buffer_num_bytes 	336
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1669__dram_size_num_bytes 	384

/* mnet_ssd_adas_flex_pic_prim_split_15 SMB_input __pvcn_1679_ */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1679__cnngen_demangled_name 	"__pvcn_1679_"
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1679__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1679__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1679__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1679__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1679__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1679__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1679__byte_offset 	128	/* 0x00000080 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1679__vbase_byte_offset 	132	/* 0x00000084 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1679__dbase_byte_offset 	156	/* 0x0000009c */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1679__dpitchm1_byte_offset 	160	/* 0x000000a0 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1679__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1679__W 	552
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1679__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1679__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1679__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1679__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1679__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1679__dpitch_num_bytes 	576
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1679__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1679__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1679__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1679__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1679__vmem_buffer_num_bytes 	552
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1679__dram_size_num_bytes 	576

/* mnet_ssd_adas_flex_pic_prim_split_15 SMB_input __pvcn_1680_ */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1680__cnngen_demangled_name 	"__pvcn_1680_"
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1680__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1680__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1680__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1680__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1680__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1680__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1680__byte_offset 	168	/* 0x000000a8 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1680__vbase_byte_offset 	172	/* 0x000000ac */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1680__dbase_byte_offset 	196	/* 0x000000c4 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1680__dpitchm1_byte_offset 	200	/* 0x000000c8 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1680__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1680__W 	344
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1680__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1680__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1680__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1680__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1680__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1680__dpitch_num_bytes 	384
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1680__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1680__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1680__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1680__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1680__vmem_buffer_num_bytes 	344
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1680__dram_size_num_bytes 	384

/* mnet_ssd_adas_flex_pic_prim_split_15 SMB_input __pvcn_1690_ */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1690__cnngen_demangled_name 	"__pvcn_1690_"
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1690__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1690__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1690__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1690__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1690__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1690__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1690__byte_offset 	208	/* 0x000000d0 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1690__vbase_byte_offset 	212	/* 0x000000d4 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1690__dbase_byte_offset 	236	/* 0x000000ec */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1690__dpitchm1_byte_offset 	240	/* 0x000000f0 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1690__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1690__W 	524
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1690__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1690__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1690__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1690__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1690__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1690__dpitch_num_bytes 	576
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1690__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1690__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1690__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1690__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1690__vmem_buffer_num_bytes 	524
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1690__dram_size_num_bytes 	576

/* mnet_ssd_adas_flex_pic_prim_split_15 SMB_input __pvcn_1691_ */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1691__cnngen_demangled_name 	"__pvcn_1691_"
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1691__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1691__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1691__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1691__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1691__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1691__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1691__byte_offset 	248	/* 0x000000f8 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1691__vbase_byte_offset 	252	/* 0x000000fc */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1691__dbase_byte_offset 	276	/* 0x00000114 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1691__dpitchm1_byte_offset 	280	/* 0x00000118 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1691__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1691__W 	336
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1691__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1691__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1691__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1691__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1691__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1691__dpitch_num_bytes 	384
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1691__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1691__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1691__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1691__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1691__vmem_buffer_num_bytes 	336
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1691__dram_size_num_bytes 	384

/* mnet_ssd_adas_flex_pic_prim_split_15 SMB_input __pvcn_1701_ */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1701__cnngen_demangled_name 	"__pvcn_1701_"
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1701__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1701__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1701__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1701__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1701__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1701__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1701__byte_offset 	288	/* 0x00000120 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1701__vbase_byte_offset 	292	/* 0x00000124 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1701__dbase_byte_offset 	316	/* 0x0000013c */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1701__dpitchm1_byte_offset 	320	/* 0x00000140 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1701__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1701__W 	512
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1701__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1701__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1701__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1701__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1701__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1701__dpitch_num_bytes 	512
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1701__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1701__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1701__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1701__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1701__vmem_buffer_num_bytes 	512
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1701__dram_size_num_bytes 	512

/* mnet_ssd_adas_flex_pic_prim_split_15 SMB_input __pvcn_1702_ */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1702__cnngen_demangled_name 	"__pvcn_1702_"
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1702__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1702__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1702__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1702__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1702__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1702__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1702__byte_offset 	328	/* 0x00000148 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1702__vbase_byte_offset 	332	/* 0x0000014c */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1702__dbase_byte_offset 	356	/* 0x00000164 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1702__dpitchm1_byte_offset 	360	/* 0x00000168 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1702__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1702__W 	336
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1702__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1702__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1702__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1702__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1702__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1702__dpitch_num_bytes 	384
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1702__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1702__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1702__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1702__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1702__vmem_buffer_num_bytes 	336
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1702__dram_size_num_bytes 	384

/* mnet_ssd_adas_flex_pic_prim_split_15 SMB_input __pvcn_1712_ */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1712__cnngen_demangled_name 	"__pvcn_1712_"
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1712__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1712__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1712__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1712__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1712__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1712__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1712__byte_offset 	368	/* 0x00000170 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1712__vbase_byte_offset 	372	/* 0x00000174 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1712__dbase_byte_offset 	396	/* 0x0000018c */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1712__dpitchm1_byte_offset 	400	/* 0x00000190 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1712__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1712__W 	568
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1712__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1712__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1712__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1712__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1712__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1712__dpitch_num_bytes 	576
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1712__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1712__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1712__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1712__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1712__vmem_buffer_num_bytes 	568
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1712__dram_size_num_bytes 	576

/* mnet_ssd_adas_flex_pic_prim_split_15 SMB_input __pvcn_1713_ */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1713__cnngen_demangled_name 	"__pvcn_1713_"
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1713__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1713__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1713__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1713__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1713__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1713__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1713__byte_offset 	408	/* 0x00000198 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1713__vbase_byte_offset 	412	/* 0x0000019c */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1713__dbase_byte_offset 	436	/* 0x000001b4 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1713__dpitchm1_byte_offset 	440	/* 0x000001b8 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1713__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1713__W 	360
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1713__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1713__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1713__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1713__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1713__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1713__dpitch_num_bytes 	384
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1713__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1713__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1713__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1713__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1713__vmem_buffer_num_bytes 	360
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1713__dram_size_num_bytes 	384

/* mnet_ssd_adas_flex_pic_prim_split_15 SMB_input __pvcn_1723_ */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1723__cnngen_demangled_name 	"__pvcn_1723_"
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1723__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1723__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1723__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1723__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1723__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1723__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1723__byte_offset 	448	/* 0x000001c0 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1723__vbase_byte_offset 	452	/* 0x000001c4 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1723__dbase_byte_offset 	476	/* 0x000001dc */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1723__dpitchm1_byte_offset 	480	/* 0x000001e0 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1723__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1723__W 	552
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1723__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1723__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1723__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1723__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1723__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1723__dpitch_num_bytes 	576
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1723__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1723__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1723__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1723__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1723__vmem_buffer_num_bytes 	552
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1723__dram_size_num_bytes 	576

/* mnet_ssd_adas_flex_pic_prim_split_15 SMB_input __pvcn_1724_ */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1724__cnngen_demangled_name 	"__pvcn_1724_"
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1724__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1724__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1724__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1724__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1724__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1724__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1724__byte_offset 	488	/* 0x000001e8 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1724__vbase_byte_offset 	492	/* 0x000001ec */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1724__dbase_byte_offset 	516	/* 0x00000204 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1724__dpitchm1_byte_offset 	520	/* 0x00000208 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1724__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1724__W 	352
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1724__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1724__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1724__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1724__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1724__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1724__dpitch_num_bytes 	384
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1724__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1724__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1724__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1724__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1724__vmem_buffer_num_bytes 	352
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1724__dram_size_num_bytes 	384

/* mnet_ssd_adas_flex_pic_prim_split_15 SMB_input __pvcn_1734_ */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1734__cnngen_demangled_name 	"__pvcn_1734_"
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1734__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1734__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1734__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1734__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1734__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1734__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1734__byte_offset 	528	/* 0x00000210 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1734__vbase_byte_offset 	532	/* 0x00000214 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1734__dbase_byte_offset 	556	/* 0x0000022c */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1734__dpitchm1_byte_offset 	560	/* 0x00000230 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1734__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1734__W 	540
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1734__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1734__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1734__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1734__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1734__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1734__dpitch_num_bytes 	576
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1734__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1734__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1734__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1734__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1734__vmem_buffer_num_bytes 	540
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1734__dram_size_num_bytes 	576

/* mnet_ssd_adas_flex_pic_prim_split_15 SMB_input __pvcn_1735_ */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1735__cnngen_demangled_name 	"__pvcn_1735_"
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1735__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1735__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1735__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1735__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1735__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1735__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1735__byte_offset 	568	/* 0x00000238 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1735__vbase_byte_offset 	572	/* 0x0000023c */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1735__dbase_byte_offset 	596	/* 0x00000254 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1735__dpitchm1_byte_offset 	600	/* 0x00000258 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1735__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1735__W 	340
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1735__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1735__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1735__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1735__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1735__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1735__dpitch_num_bytes 	384
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1735__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1735__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1735__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1735__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1735__vmem_buffer_num_bytes 	340
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1735__dram_size_num_bytes 	384

/* mnet_ssd_adas_flex_pic_prim_split_15 SMB_input __pvcn_1745_ */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1745__cnngen_demangled_name 	"__pvcn_1745_"
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1745__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1745__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1745__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1745__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1745__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1745__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1745__byte_offset 	608	/* 0x00000260 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1745__vbase_byte_offset 	612	/* 0x00000264 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1745__dbase_byte_offset 	636	/* 0x0000027c */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1745__dpitchm1_byte_offset 	640	/* 0x00000280 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1745__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1745__W 	552
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1745__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1745__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1745__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1745__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1745__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1745__dpitch_num_bytes 	576
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1745__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1745__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1745__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1745__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1745__vmem_buffer_num_bytes 	552
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1745__dram_size_num_bytes 	576

/* mnet_ssd_adas_flex_pic_prim_split_15 SMB_input __pvcn_1746_ */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1746__cnngen_demangled_name 	"__pvcn_1746_"
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1746__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1746__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1746__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1746__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1746__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1746__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1746__byte_offset 	648	/* 0x00000288 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1746__vbase_byte_offset 	652	/* 0x0000028c */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1746__dbase_byte_offset 	676	/* 0x000002a4 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1746__dpitchm1_byte_offset 	680	/* 0x000002a8 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1746__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1746__W 	348
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1746__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1746__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1746__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1746__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1746__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1746__dpitch_num_bytes 	384
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1746__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1746__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1746__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1746__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1746__vmem_buffer_num_bytes 	348
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1746__dram_size_num_bytes 	384

/* mnet_ssd_adas_flex_pic_prim_split_15 SMB_input __pvcn_1756_ */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1756__cnngen_demangled_name 	"__pvcn_1756_"
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1756__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1756__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1756__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1756__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1756__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1756__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1756__byte_offset 	688	/* 0x000002b0 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1756__vbase_byte_offset 	692	/* 0x000002b4 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1756__dbase_byte_offset 	716	/* 0x000002cc */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1756__dpitchm1_byte_offset 	720	/* 0x000002d0 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1756__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1756__W 	552
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1756__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1756__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1756__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1756__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1756__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1756__dpitch_num_bytes 	576
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1756__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1756__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1756__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1756__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1756__vmem_buffer_num_bytes 	552
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1756__dram_size_num_bytes 	576

/* mnet_ssd_adas_flex_pic_prim_split_15 SMB_input __pvcn_1757_ */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1757__cnngen_demangled_name 	"__pvcn_1757_"
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1757__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1757__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1757__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1757__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1757__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1757__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1757__byte_offset 	728	/* 0x000002d8 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1757__vbase_byte_offset 	732	/* 0x000002dc */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1757__dbase_byte_offset 	756	/* 0x000002f4 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1757__dpitchm1_byte_offset 	760	/* 0x000002f8 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1757__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1757__W 	360
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1757__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1757__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1757__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1757__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1757__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1757__dpitch_num_bytes 	384
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1757__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1757__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1757__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1757__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1757__vmem_buffer_num_bytes 	360
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1757__dram_size_num_bytes 	384

/* mnet_ssd_adas_flex_pic_prim_split_15 SMB_input __pvcn_1767_ */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1767__cnngen_demangled_name 	"__pvcn_1767_"
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1767__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1767__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1767__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1767__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1767__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1767__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1767__byte_offset 	768	/* 0x00000300 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1767__vbase_byte_offset 	772	/* 0x00000304 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1767__dbase_byte_offset 	796	/* 0x0000031c */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1767__dpitchm1_byte_offset 	800	/* 0x00000320 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1767__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1767__W 	540
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1767__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1767__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1767__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1767__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1767__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1767__dpitch_num_bytes 	576
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1767__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1767__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1767__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1767__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1767__vmem_buffer_num_bytes 	540
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1767__dram_size_num_bytes 	576

/* mnet_ssd_adas_flex_pic_prim_split_15 SMB_input __pvcn_1768_ */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1768__cnngen_demangled_name 	"__pvcn_1768_"
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1768__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1768__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1768__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1768__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1768__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1768__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1768__byte_offset 	808	/* 0x00000328 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1768__vbase_byte_offset 	812	/* 0x0000032c */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1768__dbase_byte_offset 	836	/* 0x00000344 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1768__dpitchm1_byte_offset 	840	/* 0x00000348 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1768__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1768__W 	344
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1768__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1768__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1768__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1768__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1768__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1768__dpitch_num_bytes 	384
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1768__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1768__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1768__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1768__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1768__vmem_buffer_num_bytes 	344
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1768__dram_size_num_bytes 	384

/* mnet_ssd_adas_flex_pic_prim_split_15 SMB_input __pvcn_1778_ */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1778__cnngen_demangled_name 	"__pvcn_1778_"
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1778__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1778__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1778__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1778__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1778__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1778__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1778__byte_offset 	848	/* 0x00000350 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1778__vbase_byte_offset 	852	/* 0x00000354 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1778__dbase_byte_offset 	876	/* 0x0000036c */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1778__dpitchm1_byte_offset 	880	/* 0x00000370 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1778__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1778__W 	564
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1778__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1778__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1778__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1778__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1778__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1778__dpitch_num_bytes 	576
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1778__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1778__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1778__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1778__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1778__vmem_buffer_num_bytes 	564
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1778__dram_size_num_bytes 	576

/* mnet_ssd_adas_flex_pic_prim_split_15 SMB_input __pvcn_1779_ */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1779__cnngen_demangled_name 	"__pvcn_1779_"
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1779__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1779__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1779__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1779__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1779__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1779__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1779__byte_offset 	888	/* 0x00000378 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1779__vbase_byte_offset 	892	/* 0x0000037c */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1779__dbase_byte_offset 	916	/* 0x00000394 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1779__dpitchm1_byte_offset 	920	/* 0x00000398 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1779__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1779__W 	360
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1779__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1779__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1779__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1779__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1779__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1779__dpitch_num_bytes 	384
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1779__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1779__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1779__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1779__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1779__vmem_buffer_num_bytes 	360
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1779__dram_size_num_bytes 	384

/* mnet_ssd_adas_flex_pic_prim_split_15 SMB_input __pvcn_1789_ */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1789__cnngen_demangled_name 	"__pvcn_1789_"
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1789__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1789__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1789__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1789__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1789__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1789__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1789__byte_offset 	928	/* 0x000003a0 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1789__vbase_byte_offset 	932	/* 0x000003a4 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1789__dbase_byte_offset 	956	/* 0x000003bc */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1789__dpitchm1_byte_offset 	960	/* 0x000003c0 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1789__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1789__W 	552
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1789__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1789__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1789__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1789__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1789__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1789__dpitch_num_bytes 	576
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1789__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1789__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1789__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1789__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1789__vmem_buffer_num_bytes 	552
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1789__dram_size_num_bytes 	576

/* mnet_ssd_adas_flex_pic_prim_split_15 SMB_input __pvcn_1790_ */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1790__cnngen_demangled_name 	"__pvcn_1790_"
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1790__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1790__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1790__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1790__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1790__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1790__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1790__byte_offset 	968	/* 0x000003c8 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1790__vbase_byte_offset 	972	/* 0x000003cc */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1790__dbase_byte_offset 	996	/* 0x000003e4 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1790__dpitchm1_byte_offset 	1000	/* 0x000003e8 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1790__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1790__W 	344
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1790__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1790__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1790__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1790__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1790__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1790__dpitch_num_bytes 	384
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1790__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1790__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1790__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1790__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1790__vmem_buffer_num_bytes 	344
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1790__dram_size_num_bytes 	384

/* mnet_ssd_adas_flex_pic_prim_split_15 SMB_input __pvcn_1800_ */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1800__cnngen_demangled_name 	"__pvcn_1800_"
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1800__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1800__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1800__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1800__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1800__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1800__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1800__byte_offset 	1008	/* 0x000003f0 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1800__vbase_byte_offset 	1012	/* 0x000003f4 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1800__dbase_byte_offset 	1036	/* 0x0000040c */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1800__dpitchm1_byte_offset 	1040	/* 0x00000410 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1800__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1800__W 	564
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1800__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1800__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1800__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1800__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1800__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1800__dpitch_num_bytes 	576
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1800__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1800__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1800__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1800__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1800__vmem_buffer_num_bytes 	564
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1800__dram_size_num_bytes 	576

/* mnet_ssd_adas_flex_pic_prim_split_15 SMB_input __pvcn_1801_ */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1801__cnngen_demangled_name 	"__pvcn_1801_"
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1801__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1801__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1801__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1801__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1801__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1801__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1801__byte_offset 	1048	/* 0x00000418 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1801__vbase_byte_offset 	1052	/* 0x0000041c */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1801__dbase_byte_offset 	1076	/* 0x00000434 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1801__dpitchm1_byte_offset 	1080	/* 0x00000438 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1801__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1801__W 	352
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1801__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1801__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1801__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1801__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1801__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1801__dpitch_num_bytes 	384
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1801__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1801__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1801__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1801__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1801__vmem_buffer_num_bytes 	352
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1801__dram_size_num_bytes 	384

/* mnet_ssd_adas_flex_pic_prim_split_15 SMB_input __pvcn_1811_ */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1811__cnngen_demangled_name 	"__pvcn_1811_"
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1811__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1811__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1811__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1811__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1811__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1811__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1811__byte_offset 	1088	/* 0x00000440 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1811__vbase_byte_offset 	1092	/* 0x00000444 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1811__dbase_byte_offset 	1116	/* 0x0000045c */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1811__dpitchm1_byte_offset 	1120	/* 0x00000460 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1811__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1811__W 	416
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1811__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1811__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1811__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1811__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1811__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1811__dpitch_num_bytes 	448
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1811__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1811__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1811__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1811__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1811__vmem_buffer_num_bytes 	416
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1811__dram_size_num_bytes 	448

/* mnet_ssd_adas_flex_pic_prim_split_15 SMB_input __pvcn_1812_ */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1812__cnngen_demangled_name 	"__pvcn_1812_"
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1812__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1812__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1812__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1812__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1812__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1812__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1812__byte_offset 	1128	/* 0x00000468 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1812__vbase_byte_offset 	1132	/* 0x0000046c */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1812__dbase_byte_offset 	1156	/* 0x00000484 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1812__dpitchm1_byte_offset 	1160	/* 0x00000488 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1812__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1812__W 	272
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1812__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1812__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1812__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1812__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1812__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1812__dpitch_num_bytes 	320
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1812__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1812__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1812__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1812__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1812__vmem_buffer_num_bytes 	272
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1812__dram_size_num_bytes 	320

/* mnet_ssd_adas_flex_pic_prim_split_15 VCB conv5_3__sep_____bn_conv5_3__sep__scale_mul___muli___62_____bn_conv5_3__sep__scale_mul_____scale___muli___352 */
#define mnet_ssd_adas_flex_pic_prim_split_15_conv5_3__sep_____bn_conv5_3__sep__scale_mul___muli___62_____bn_conv5_3__sep__scale_mul_____scale___muli___352_cnngen_demangled_name 	"conv5_3__sep_____bn_conv5_3__sep__scale_mul___muli___62_____bn_conv5_3__sep__scale_mul_____scale___muli___352"
#define mnet_ssd_adas_flex_pic_prim_split_15_conv5_3__sep_____bn_conv5_3__sep__scale_mul___muli___62_____bn_conv5_3__sep__scale_mul_____scale___muli___352_is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_15_conv5_3__sep_____bn_conv5_3__sep__scale_mul___muli___62_____bn_conv5_3__sep__scale_mul_____scale___muli___352_is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_15_conv5_3__sep_____bn_conv5_3__sep__scale_mul___muli___62_____bn_conv5_3__sep__scale_mul_____scale___muli___352_is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_15_conv5_3__sep_____bn_conv5_3__sep__scale_mul___muli___62_____bn_conv5_3__sep__scale_mul_____scale___muli___352_byte_offset 	1216	/* 0x000004c0 */
#define mnet_ssd_adas_flex_pic_prim_split_15_conv5_3__sep_____bn_conv5_3__sep__scale_mul___muli___62_____bn_conv5_3__sep__scale_mul_____scale___muli___352_vbase_byte_offset 	1224	/* 0x000004c8 */
#define mnet_ssd_adas_flex_pic_prim_split_15_conv5_3__sep_____bn_conv5_3__sep__scale_mul___muli___62_____bn_conv5_3__sep__scale_mul_____scale___muli___352_vmem_buffer_num_bytes 	122880

/* mnet_ssd_adas_flex_pic_prim_split_15 HMB_output __pvcn_1670_ */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1670__cnngen_demangled_name 	"__pvcn_1670_"
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1670__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1670__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1670__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1670__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1670__byte_offset 	1288	/* 0x00000508 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1670__vbase_byte_offset 	1292	/* 0x0000050c */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1670__dbase_byte_offset 	1316	/* 0x00000524 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1670__dpitchm1_byte_offset 	1320	/* 0x00000528 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1670__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1670__vwidth_minus_one_byte_offset 	1304	/* 0x00000518 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1670__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1670__vheight_minus_one_byte_offset 	1306	/* 0x0000051a */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1670__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1670__drotate_bit_offset 	10502	/* 0x00002906 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1670__hflip_bit_offset 	10503	/* 0x00002907 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1670__vflip_bit_offset 	10504	/* 0x00002908 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1670__dflip_bit_offset 	10505	/* 0x00002909 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1670__pflip_bit_offset 	10506	/* 0x0000290a */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1670__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1670__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1670__D 	35
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1670__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1670__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1670__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1670__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1670__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1670__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1670__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1670__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1670__vmem_buffer_num_bytes 	26880
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1670__dram_size_num_bytes 	31936
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1670__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1670__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1670__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1670__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1670__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1670__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1670__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1670__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1670__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1670__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_15 HMB_output __pvcn_1681_ */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1681__cnngen_demangled_name 	"__pvcn_1681_"
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1681__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1681__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1681__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1681__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1681__byte_offset 	1344	/* 0x00000540 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1681__vbase_byte_offset 	1348	/* 0x00000544 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1681__dbase_byte_offset 	1372	/* 0x0000055c */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1681__dpitchm1_byte_offset 	1376	/* 0x00000560 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1681__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1681__vwidth_minus_one_byte_offset 	1360	/* 0x00000550 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1681__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1681__vheight_minus_one_byte_offset 	1362	/* 0x00000552 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1681__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1681__drotate_bit_offset 	10950	/* 0x00002ac6 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1681__hflip_bit_offset 	10951	/* 0x00002ac7 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1681__vflip_bit_offset 	10952	/* 0x00002ac8 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1681__dflip_bit_offset 	10953	/* 0x00002ac9 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1681__pflip_bit_offset 	10954	/* 0x00002aca */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1681__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1681__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1681__D 	36
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1681__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1681__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1681__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1681__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1681__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1681__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1681__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1681__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1681__vmem_buffer_num_bytes 	27648
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1681__dram_size_num_bytes 	32832
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1681__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1681__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1681__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1681__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1681__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1681__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1681__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1681__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1681__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1681__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_15 HMB_output __pvcn_1692_ */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1692__cnngen_demangled_name 	"__pvcn_1692_"
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1692__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1692__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1692__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1692__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1692__byte_offset 	1400	/* 0x00000578 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1692__vbase_byte_offset 	1404	/* 0x0000057c */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1692__dbase_byte_offset 	1428	/* 0x00000594 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1692__dpitchm1_byte_offset 	1432	/* 0x00000598 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1692__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1692__vwidth_minus_one_byte_offset 	1416	/* 0x00000588 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1692__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1692__vheight_minus_one_byte_offset 	1418	/* 0x0000058a */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1692__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1692__drotate_bit_offset 	11398	/* 0x00002c86 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1692__hflip_bit_offset 	11399	/* 0x00002c87 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1692__vflip_bit_offset 	11400	/* 0x00002c88 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1692__dflip_bit_offset 	11401	/* 0x00002c89 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1692__pflip_bit_offset 	11402	/* 0x00002c8a */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1692__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1692__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1692__D 	36
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1692__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1692__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1692__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1692__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1692__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1692__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1692__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1692__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1692__vmem_buffer_num_bytes 	27648
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1692__dram_size_num_bytes 	32832
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1692__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1692__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1692__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1692__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1692__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1692__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1692__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1692__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1692__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1692__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_15 HMB_output __pvcn_1703_ */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1703__cnngen_demangled_name 	"__pvcn_1703_"
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1703__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1703__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1703__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1703__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1703__byte_offset 	1456	/* 0x000005b0 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1703__vbase_byte_offset 	1460	/* 0x000005b4 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1703__dbase_byte_offset 	1484	/* 0x000005cc */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1703__dpitchm1_byte_offset 	1488	/* 0x000005d0 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1703__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1703__vwidth_minus_one_byte_offset 	1472	/* 0x000005c0 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1703__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1703__vheight_minus_one_byte_offset 	1474	/* 0x000005c2 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1703__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1703__drotate_bit_offset 	11846	/* 0x00002e46 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1703__hflip_bit_offset 	11847	/* 0x00002e47 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1703__vflip_bit_offset 	11848	/* 0x00002e48 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1703__dflip_bit_offset 	11849	/* 0x00002e49 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1703__pflip_bit_offset 	11850	/* 0x00002e4a */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1703__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1703__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1703__D 	35
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1703__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1703__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1703__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1703__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1703__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1703__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1703__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1703__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1703__vmem_buffer_num_bytes 	26880
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1703__dram_size_num_bytes 	31936
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1703__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1703__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1703__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1703__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1703__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1703__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1703__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1703__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1703__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1703__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_15 HMB_output __pvcn_1714_ */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1714__cnngen_demangled_name 	"__pvcn_1714_"
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1714__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1714__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1714__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1714__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1714__byte_offset 	1512	/* 0x000005e8 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1714__vbase_byte_offset 	1516	/* 0x000005ec */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1714__dbase_byte_offset 	1540	/* 0x00000604 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1714__dpitchm1_byte_offset 	1544	/* 0x00000608 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1714__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1714__vwidth_minus_one_byte_offset 	1528	/* 0x000005f8 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1714__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1714__vheight_minus_one_byte_offset 	1530	/* 0x000005fa */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1714__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1714__drotate_bit_offset 	12294	/* 0x00003006 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1714__hflip_bit_offset 	12295	/* 0x00003007 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1714__vflip_bit_offset 	12296	/* 0x00003008 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1714__dflip_bit_offset 	12297	/* 0x00003009 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1714__pflip_bit_offset 	12298	/* 0x0000300a */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1714__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1714__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1714__D 	40
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1714__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1714__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1714__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1714__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1714__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1714__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1714__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1714__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1714__vmem_buffer_num_bytes 	30720
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1714__dram_size_num_bytes 	36480
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1714__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1714__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1714__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1714__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1714__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1714__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1714__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1714__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1714__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1714__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_15 HMB_output __pvcn_1725_ */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1725__cnngen_demangled_name 	"__pvcn_1725_"
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1725__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1725__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1725__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1725__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1725__byte_offset 	1568	/* 0x00000620 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1725__vbase_byte_offset 	1572	/* 0x00000624 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1725__dbase_byte_offset 	1596	/* 0x0000063c */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1725__dpitchm1_byte_offset 	1600	/* 0x00000640 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1725__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1725__vwidth_minus_one_byte_offset 	1584	/* 0x00000630 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1725__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1725__vheight_minus_one_byte_offset 	1586	/* 0x00000632 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1725__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1725__drotate_bit_offset 	12742	/* 0x000031c6 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1725__hflip_bit_offset 	12743	/* 0x000031c7 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1725__vflip_bit_offset 	12744	/* 0x000031c8 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1725__dflip_bit_offset 	12745	/* 0x000031c9 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1725__pflip_bit_offset 	12746	/* 0x000031ca */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1725__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1725__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1725__D 	38
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1725__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1725__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1725__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1725__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1725__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1725__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1725__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1725__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1725__vmem_buffer_num_bytes 	29184
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1725__dram_size_num_bytes 	34656
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1725__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1725__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1725__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1725__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1725__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1725__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1725__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1725__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1725__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1725__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_15 HMB_output __pvcn_1736_ */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1736__cnngen_demangled_name 	"__pvcn_1736_"
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1736__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1736__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1736__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1736__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1736__byte_offset 	1624	/* 0x00000658 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1736__vbase_byte_offset 	1628	/* 0x0000065c */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1736__dbase_byte_offset 	1652	/* 0x00000674 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1736__dpitchm1_byte_offset 	1656	/* 0x00000678 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1736__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1736__vwidth_minus_one_byte_offset 	1640	/* 0x00000668 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1736__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1736__vheight_minus_one_byte_offset 	1642	/* 0x0000066a */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1736__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1736__drotate_bit_offset 	13190	/* 0x00003386 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1736__hflip_bit_offset 	13191	/* 0x00003387 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1736__vflip_bit_offset 	13192	/* 0x00003388 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1736__dflip_bit_offset 	13193	/* 0x00003389 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1736__pflip_bit_offset 	13194	/* 0x0000338a */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1736__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1736__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1736__D 	36
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1736__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1736__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1736__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1736__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1736__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1736__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1736__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1736__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1736__vmem_buffer_num_bytes 	27648
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1736__dram_size_num_bytes 	32832
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1736__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1736__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1736__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1736__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1736__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1736__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1736__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1736__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1736__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1736__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_15 HMB_output __pvcn_1747_ */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1747__cnngen_demangled_name 	"__pvcn_1747_"
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1747__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1747__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1747__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1747__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1747__byte_offset 	1680	/* 0x00000690 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1747__vbase_byte_offset 	1684	/* 0x00000694 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1747__dbase_byte_offset 	1708	/* 0x000006ac */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1747__dpitchm1_byte_offset 	1712	/* 0x000006b0 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1747__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1747__vwidth_minus_one_byte_offset 	1696	/* 0x000006a0 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1747__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1747__vheight_minus_one_byte_offset 	1698	/* 0x000006a2 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1747__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1747__drotate_bit_offset 	13638	/* 0x00003546 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1747__hflip_bit_offset 	13639	/* 0x00003547 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1747__vflip_bit_offset 	13640	/* 0x00003548 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1747__dflip_bit_offset 	13641	/* 0x00003549 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1747__pflip_bit_offset 	13642	/* 0x0000354a */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1747__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1747__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1747__D 	37
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1747__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1747__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1747__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1747__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1747__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1747__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1747__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1747__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1747__vmem_buffer_num_bytes 	28416
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1747__dram_size_num_bytes 	33760
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1747__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1747__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1747__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1747__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1747__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1747__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1747__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1747__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1747__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1747__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_15 HMB_output __pvcn_1758_ */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1758__cnngen_demangled_name 	"__pvcn_1758_"
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1758__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1758__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1758__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1758__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1758__byte_offset 	1736	/* 0x000006c8 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1758__vbase_byte_offset 	1740	/* 0x000006cc */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1758__dbase_byte_offset 	1764	/* 0x000006e4 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1758__dpitchm1_byte_offset 	1768	/* 0x000006e8 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1758__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1758__vwidth_minus_one_byte_offset 	1752	/* 0x000006d8 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1758__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1758__vheight_minus_one_byte_offset 	1754	/* 0x000006da */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1758__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1758__drotate_bit_offset 	14086	/* 0x00003706 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1758__hflip_bit_offset 	14087	/* 0x00003707 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1758__vflip_bit_offset 	14088	/* 0x00003708 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1758__dflip_bit_offset 	14089	/* 0x00003709 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1758__pflip_bit_offset 	14090	/* 0x0000370a */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1758__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1758__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1758__D 	40
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1758__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1758__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1758__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1758__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1758__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1758__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1758__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1758__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1758__vmem_buffer_num_bytes 	30720
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1758__dram_size_num_bytes 	36480
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1758__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1758__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1758__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1758__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1758__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1758__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1758__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1758__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1758__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1758__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_15 HMB_output __pvcn_1769_ */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1769__cnngen_demangled_name 	"__pvcn_1769_"
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1769__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1769__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1769__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1769__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1769__byte_offset 	1792	/* 0x00000700 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1769__vbase_byte_offset 	1796	/* 0x00000704 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1769__dbase_byte_offset 	1820	/* 0x0000071c */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1769__dpitchm1_byte_offset 	1824	/* 0x00000720 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1769__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1769__vwidth_minus_one_byte_offset 	1808	/* 0x00000710 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1769__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1769__vheight_minus_one_byte_offset 	1810	/* 0x00000712 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1769__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1769__drotate_bit_offset 	14534	/* 0x000038c6 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1769__hflip_bit_offset 	14535	/* 0x000038c7 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1769__vflip_bit_offset 	14536	/* 0x000038c8 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1769__dflip_bit_offset 	14537	/* 0x000038c9 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1769__pflip_bit_offset 	14538	/* 0x000038ca */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1769__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1769__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1769__D 	36
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1769__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1769__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1769__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1769__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1769__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1769__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1769__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1769__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1769__vmem_buffer_num_bytes 	27648
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1769__dram_size_num_bytes 	32832
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1769__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1769__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1769__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1769__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1769__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1769__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1769__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1769__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1769__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1769__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_15 HMB_output __pvcn_1780_ */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1780__cnngen_demangled_name 	"__pvcn_1780_"
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1780__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1780__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1780__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1780__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1780__byte_offset 	1848	/* 0x00000738 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1780__vbase_byte_offset 	1852	/* 0x0000073c */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1780__dbase_byte_offset 	1876	/* 0x00000754 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1780__dpitchm1_byte_offset 	1880	/* 0x00000758 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1780__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1780__vwidth_minus_one_byte_offset 	1864	/* 0x00000748 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1780__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1780__vheight_minus_one_byte_offset 	1866	/* 0x0000074a */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1780__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1780__drotate_bit_offset 	14982	/* 0x00003a86 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1780__hflip_bit_offset 	14983	/* 0x00003a87 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1780__vflip_bit_offset 	14984	/* 0x00003a88 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1780__dflip_bit_offset 	14985	/* 0x00003a89 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1780__pflip_bit_offset 	14986	/* 0x00003a8a */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1780__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1780__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1780__D 	40
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1780__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1780__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1780__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1780__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1780__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1780__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1780__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1780__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1780__vmem_buffer_num_bytes 	30720
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1780__dram_size_num_bytes 	36480
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1780__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1780__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1780__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1780__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1780__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1780__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1780__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1780__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1780__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1780__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_15 HMB_output __pvcn_1791_ */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1791__cnngen_demangled_name 	"__pvcn_1791_"
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1791__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1791__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1791__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1791__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1791__byte_offset 	1904	/* 0x00000770 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1791__vbase_byte_offset 	1908	/* 0x00000774 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1791__dbase_byte_offset 	1932	/* 0x0000078c */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1791__dpitchm1_byte_offset 	1936	/* 0x00000790 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1791__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1791__vwidth_minus_one_byte_offset 	1920	/* 0x00000780 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1791__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1791__vheight_minus_one_byte_offset 	1922	/* 0x00000782 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1791__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1791__drotate_bit_offset 	15430	/* 0x00003c46 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1791__hflip_bit_offset 	15431	/* 0x00003c47 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1791__vflip_bit_offset 	15432	/* 0x00003c48 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1791__dflip_bit_offset 	15433	/* 0x00003c49 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1791__pflip_bit_offset 	15434	/* 0x00003c4a */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1791__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1791__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1791__D 	36
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1791__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1791__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1791__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1791__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1791__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1791__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1791__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1791__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1791__vmem_buffer_num_bytes 	13824
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1791__dram_size_num_bytes 	32832
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1791__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1791__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1791__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1791__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1791__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1791__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1791__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1791__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1791__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1791__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_15 HMB_output __pvcn_1802_ */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1802__cnngen_demangled_name 	"__pvcn_1802_"
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1802__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1802__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1802__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1802__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1802__byte_offset 	1960	/* 0x000007a8 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1802__vbase_byte_offset 	1964	/* 0x000007ac */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1802__dbase_byte_offset 	1988	/* 0x000007c4 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1802__dpitchm1_byte_offset 	1992	/* 0x000007c8 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1802__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1802__vwidth_minus_one_byte_offset 	1976	/* 0x000007b8 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1802__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1802__vheight_minus_one_byte_offset 	1978	/* 0x000007ba */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1802__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1802__drotate_bit_offset 	15878	/* 0x00003e06 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1802__hflip_bit_offset 	15879	/* 0x00003e07 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1802__vflip_bit_offset 	15880	/* 0x00003e08 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1802__dflip_bit_offset 	15881	/* 0x00003e09 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1802__pflip_bit_offset 	15882	/* 0x00003e0a */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1802__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1802__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1802__D 	38
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1802__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1802__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1802__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1802__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1802__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1802__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1802__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1802__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1802__vmem_buffer_num_bytes 	14592
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1802__dram_size_num_bytes 	34656
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1802__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1802__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1802__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1802__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1802__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1802__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1802__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1802__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1802__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1802__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_15 HMB_output __pvcn_1813_ */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1813__cnngen_demangled_name 	"__pvcn_1813_"
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1813__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1813__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1813__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1813__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1813__byte_offset 	2016	/* 0x000007e0 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1813__vbase_byte_offset 	2020	/* 0x000007e4 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1813__dbase_byte_offset 	2044	/* 0x000007fc */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1813__dpitchm1_byte_offset 	2048	/* 0x00000800 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1813__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1813__vwidth_minus_one_byte_offset 	2032	/* 0x000007f0 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1813__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1813__vheight_minus_one_byte_offset 	2034	/* 0x000007f2 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1813__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1813__drotate_bit_offset 	16326	/* 0x00003fc6 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1813__hflip_bit_offset 	16327	/* 0x00003fc7 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1813__vflip_bit_offset 	16328	/* 0x00003fc8 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1813__dflip_bit_offset 	16329	/* 0x00003fc9 */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1813__pflip_bit_offset 	16330	/* 0x00003fca */
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1813__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1813__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1813__D 	29
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1813__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1813__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1813__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1813__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1813__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1813__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1813__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1813__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1813__vmem_buffer_num_bytes 	11136
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1813__dram_size_num_bytes 	26464
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1813__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1813__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1813__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1813__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1813__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1813__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1813__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1813__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1813__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___pvcn_1813__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_15 SMB_output __vas_created_terminate__v0 */
#define mnet_ssd_adas_flex_pic_prim_split_15___vas_created_terminate__v0_cnngen_demangled_name 	"__vas_created_terminate__v0"
#define mnet_ssd_adas_flex_pic_prim_split_15___vas_created_terminate__v0_is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___vas_created_terminate__v0_is_bitvector 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___vas_created_terminate__v0_is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___vas_created_terminate__v0_is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___vas_created_terminate__v0_byte_offset 	2056	/* 0x00000808 */
#define mnet_ssd_adas_flex_pic_prim_split_15___vas_created_terminate__v0_vbase_byte_offset 	2060	/* 0x0000080c */
#define mnet_ssd_adas_flex_pic_prim_split_15___vas_created_terminate__v0_W 	256
#define mnet_ssd_adas_flex_pic_prim_split_15___vas_created_terminate__v0_H 	3
#define mnet_ssd_adas_flex_pic_prim_split_15___vas_created_terminate__v0_D 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___vas_created_terminate__v0_P 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___vas_created_terminate__v0_data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___vas_created_terminate__v0_denable 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___vas_created_terminate__v0_dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_15___vas_created_terminate__v0_vp_interleave_mode 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___vas_created_terminate__v0_tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___vas_created_terminate__v0_tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_15___vas_created_terminate__v0_vmem_buffer_num_bytes 	96


#endif /* mnet_ssd_adas_flex_pic_prim_split_15_H */
