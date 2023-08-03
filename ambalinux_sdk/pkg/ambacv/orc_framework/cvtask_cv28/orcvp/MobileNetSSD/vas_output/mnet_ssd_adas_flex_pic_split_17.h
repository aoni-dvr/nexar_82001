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
#ifndef mnet_ssd_adas_flex_pic_prim_split_17_H
#define mnet_ssd_adas_flex_pic_prim_split_17_H

#define vas_gen_header_version                  	7

/* dag name */
#define mnet_ssd_adas_flex_pic_prim_split_17_DAG_NAME 	"mnet_ssd_adas_flex_pic_prim_split_17"

/* VDG file info */
#define mnet_ssd_adas_flex_pic_prim_split_17_vdg_name 	"mnet_ssd_adas_flex_pic_split_17.vdg"

/* VDG DAG memory info */
#define mnet_ssd_adas_flex_pic_prim_split_17_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_17_byte_size 	752	/* 0x000002f0 */
#define mnet_ssd_adas_flex_pic_prim_split_17_preferred_dram_xfer_size 	1

/* mnet_ssd_adas_flex_pic_prim_split_17 VMEM info */
#define mnet_ssd_adas_flex_pic_prim_split_17_use_ping_pong_vmem 	0
#define mnet_ssd_adas_flex_pic_prim_split_17_VMEM_start 	4096	/* 0x00001000 */
#define mnet_ssd_adas_flex_pic_prim_split_17_VMEM_end 	508476	/* 0x0007c23c */
#define mnet_ssd_adas_flex_pic_prim_split_17_image_start 	256288	/* 0x0003e920 */
#define mnet_ssd_adas_flex_pic_prim_split_17_image_size 	252940	/* 0x0003dc0c */
#define mnet_ssd_adas_flex_pic_prim_split_17_dagbin_start 	508476	/* 0x0007c23c */

/* mnet_ssd_adas_flex_pic_prim_split_17 DAG info */
#define mnet_ssd_adas_flex_pic_prim_split_17_estimated_cycles 	108717
#define mnet_ssd_adas_flex_pic_prim_split_17_ddi_byte_size 	0	/* 0x00000000 */

/*******************************************************************************
 * DAG byte offset info for HMB/SMB/VCB and their fields
 * in DAG 'mnet_ssd_adas_flex_pic_prim_split_17' in source file 'pre_split17_mnet_ssd_adas_flex_pic_split_0.vas'
 ******************************************************************************/
/* mnet_ssd_adas_flex_pic_prim_split_17 HMB_input __pvcn_1832_ */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1832__cnngen_demangled_name 	"__pvcn_1832_"
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1832__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1832__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1832__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1832__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1832__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1832__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1832__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1832__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1832__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1832__byte_offset 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1832__vbase_byte_offset 	4	/* 0x00000004 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1832__dbase_byte_offset 	28	/* 0x0000001c */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1832__dpitchm1_byte_offset 	32	/* 0x00000020 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1832__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1832__vwidth_minus_one_byte_offset 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1832__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1832__vheight_minus_one_byte_offset 	18	/* 0x00000012 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1832__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1832__drotate_bit_offset 	198	/* 0x000000c6 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1832__hflip_bit_offset 	199	/* 0x000000c7 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1832__vflip_bit_offset 	200	/* 0x000000c8 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1832__dflip_bit_offset 	201	/* 0x000000c9 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1832__pflip_bit_offset 	202	/* 0x000000ca */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1832__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1832__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1832__D 	78
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1832__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1832__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1832__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1832__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1832__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1832__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1832__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1832__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1832__vmem_buffer_num_bytes 	29952
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1832__dram_size_num_bytes 	71136
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1832__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1832__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1832__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1832__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1832__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1832__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1832__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1832__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1832__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1832__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_17 HMB_input __pvcn_1835_ */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1835__cnngen_demangled_name 	"__pvcn_1835_"
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1835__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1835__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1835__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1835__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1835__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1835__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1835__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1835__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1835__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1835__byte_offset 	40	/* 0x00000028 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1835__vbase_byte_offset 	44	/* 0x0000002c */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1835__dbase_byte_offset 	68	/* 0x00000044 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1835__dpitchm1_byte_offset 	72	/* 0x00000048 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1835__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1835__vwidth_minus_one_byte_offset 	56	/* 0x00000038 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1835__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1835__vheight_minus_one_byte_offset 	58	/* 0x0000003a */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1835__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1835__drotate_bit_offset 	518	/* 0x00000206 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1835__hflip_bit_offset 	519	/* 0x00000207 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1835__vflip_bit_offset 	520	/* 0x00000208 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1835__dflip_bit_offset 	521	/* 0x00000209 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1835__pflip_bit_offset 	522	/* 0x0000020a */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1835__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1835__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1835__D 	76
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1835__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1835__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1835__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1835__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1835__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1835__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1835__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1835__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1835__vmem_buffer_num_bytes 	29184
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1835__dram_size_num_bytes 	69312
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1835__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1835__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1835__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1835__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1835__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1835__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1835__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1835__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1835__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1835__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_17 HMB_input __pvcn_1830_ */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1830__cnngen_demangled_name 	"__pvcn_1830_"
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1830__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1830__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1830__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1830__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1830__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1830__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1830__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1830__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1830__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1830__byte_offset 	80	/* 0x00000050 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1830__vbase_byte_offset 	84	/* 0x00000054 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1830__dbase_byte_offset 	108	/* 0x0000006c */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1830__dpitchm1_byte_offset 	112	/* 0x00000070 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1830__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1830__vwidth_minus_one_byte_offset 	96	/* 0x00000060 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1830__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1830__vheight_minus_one_byte_offset 	98	/* 0x00000062 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1830__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1830__drotate_bit_offset 	838	/* 0x00000346 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1830__hflip_bit_offset 	839	/* 0x00000347 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1830__vflip_bit_offset 	840	/* 0x00000348 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1830__dflip_bit_offset 	841	/* 0x00000349 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1830__pflip_bit_offset 	842	/* 0x0000034a */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1830__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1830__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1830__D 	71
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1830__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1830__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1830__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1830__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1830__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1830__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1830__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1830__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1830__vmem_buffer_num_bytes 	27264
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1830__dram_size_num_bytes 	64768
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1830__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1830__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1830__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1830__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1830__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1830__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1830__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1830__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1830__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1830__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_17 HMB_input __pvcn_1833_ */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1833__cnngen_demangled_name 	"__pvcn_1833_"
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1833__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1833__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1833__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1833__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1833__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1833__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1833__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1833__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1833__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1833__byte_offset 	116	/* 0x00000074 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1833__vbase_byte_offset 	120	/* 0x00000078 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1833__dbase_byte_offset 	144	/* 0x00000090 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1833__dpitchm1_byte_offset 	148	/* 0x00000094 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1833__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1833__vwidth_minus_one_byte_offset 	132	/* 0x00000084 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1833__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1833__vheight_minus_one_byte_offset 	134	/* 0x00000086 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1833__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1833__drotate_bit_offset 	1126	/* 0x00000466 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1833__hflip_bit_offset 	1127	/* 0x00000467 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1833__vflip_bit_offset 	1128	/* 0x00000468 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1833__dflip_bit_offset 	1129	/* 0x00000469 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1833__pflip_bit_offset 	1130	/* 0x0000046a */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1833__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1833__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1833__D 	73
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1833__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1833__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1833__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1833__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1833__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1833__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1833__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1833__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1833__vmem_buffer_num_bytes 	14016
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1833__dram_size_num_bytes 	66592
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1833__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1833__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1833__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1833__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1833__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1833__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1833__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1833__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1833__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1833__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_17 HMB_input __pvcn_1834_ */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1834__cnngen_demangled_name 	"__pvcn_1834_"
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1834__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1834__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1834__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1834__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1834__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1834__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1834__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1834__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1834__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1834__byte_offset 	156	/* 0x0000009c */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1834__vbase_byte_offset 	160	/* 0x000000a0 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1834__dbase_byte_offset 	184	/* 0x000000b8 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1834__dpitchm1_byte_offset 	188	/* 0x000000bc */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1834__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1834__vwidth_minus_one_byte_offset 	172	/* 0x000000ac */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1834__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1834__vheight_minus_one_byte_offset 	174	/* 0x000000ae */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1834__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1834__drotate_bit_offset 	1446	/* 0x000005a6 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1834__hflip_bit_offset 	1447	/* 0x000005a7 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1834__vflip_bit_offset 	1448	/* 0x000005a8 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1834__dflip_bit_offset 	1449	/* 0x000005a9 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1834__pflip_bit_offset 	1450	/* 0x000005aa */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1834__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1834__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1834__D 	76
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1834__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1834__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1834__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1834__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1834__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1834__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1834__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1834__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1834__vmem_buffer_num_bytes 	14592
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1834__dram_size_num_bytes 	69312
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1834__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1834__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1834__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1834__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1834__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1834__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1834__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1834__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1834__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1834__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_17 HMB_input __pvcn_1836_ */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1836__cnngen_demangled_name 	"__pvcn_1836_"
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1836__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1836__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1836__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1836__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1836__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1836__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1836__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1836__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1836__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1836__byte_offset 	196	/* 0x000000c4 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1836__vbase_byte_offset 	200	/* 0x000000c8 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1836__dbase_byte_offset 	224	/* 0x000000e0 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1836__dpitchm1_byte_offset 	228	/* 0x000000e4 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1836__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1836__vwidth_minus_one_byte_offset 	212	/* 0x000000d4 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1836__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1836__vheight_minus_one_byte_offset 	214	/* 0x000000d6 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1836__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1836__drotate_bit_offset 	1766	/* 0x000006e6 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1836__hflip_bit_offset 	1767	/* 0x000006e7 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1836__vflip_bit_offset 	1768	/* 0x000006e8 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1836__dflip_bit_offset 	1769	/* 0x000006e9 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1836__pflip_bit_offset 	1770	/* 0x000006ea */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1836__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1836__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1836__D 	67
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1836__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1836__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1836__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1836__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1836__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1836__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1836__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1836__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1836__vmem_buffer_num_bytes 	12864
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1836__dram_size_num_bytes 	61120
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1836__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1836__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1836__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1836__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1836__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1836__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1836__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1836__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1836__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1836__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_17 HMB_input __pvcn_1831_ */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1831__cnngen_demangled_name 	"__pvcn_1831_"
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1831__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1831__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1831__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1831__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1831__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1831__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1831__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1831__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1831__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1831__byte_offset 	236	/* 0x000000ec */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1831__vbase_byte_offset 	240	/* 0x000000f0 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1831__dbase_byte_offset 	264	/* 0x00000108 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1831__dpitchm1_byte_offset 	268	/* 0x0000010c */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1831__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1831__vwidth_minus_one_byte_offset 	252	/* 0x000000fc */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1831__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1831__vheight_minus_one_byte_offset 	254	/* 0x000000fe */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1831__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1831__drotate_bit_offset 	2086	/* 0x00000826 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1831__hflip_bit_offset 	2087	/* 0x00000827 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1831__vflip_bit_offset 	2088	/* 0x00000828 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1831__dflip_bit_offset 	2089	/* 0x00000829 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1831__pflip_bit_offset 	2090	/* 0x0000082a */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1831__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1831__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1831__D 	71
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1831__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1831__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1831__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1831__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1831__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1831__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1831__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1831__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1831__vmem_buffer_num_bytes 	13632
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1831__dram_size_num_bytes 	64768
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1831__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1831__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1831__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1831__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1831__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1831__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1831__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1831__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1831__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1831__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_17 SMB_input __pvcn_1844_ */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1844__cnngen_demangled_name 	"__pvcn_1844_"
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1844__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1844__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1844__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1844__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1844__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1844__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1844__byte_offset 	284	/* 0x0000011c */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1844__vbase_byte_offset 	288	/* 0x00000120 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1844__dbase_byte_offset 	312	/* 0x00000138 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1844__dpitchm1_byte_offset 	316	/* 0x0000013c */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1844__dpitchm1_bsize 	32	/* 0x00000020 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1844__W 	249340
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1844__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1844__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1844__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1844__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1844__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1844__dpitch_num_bytes 	249344
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1844__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1844__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1844__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1844__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1844__vmem_buffer_num_bytes 	249340
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1844__dram_size_num_bytes 	249344

/* mnet_ssd_adas_flex_pic_prim_split_17 SMB_input __pvcn_1845_ */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1845__cnngen_demangled_name 	"__pvcn_1845_"
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1845__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1845__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1845__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1845__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1845__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1845__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1845__byte_offset 	324	/* 0x00000144 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1845__vbase_byte_offset 	328	/* 0x00000148 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1845__dbase_byte_offset 	352	/* 0x00000160 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1845__dpitchm1_byte_offset 	356	/* 0x00000164 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1845__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1845__W 	2848
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1845__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1845__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1845__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1845__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1845__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1845__dpitch_num_bytes 	2880
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1845__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1845__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1845__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1845__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1845__vmem_buffer_num_bytes 	2848
#define mnet_ssd_adas_flex_pic_prim_split_17___pvcn_1845__dram_size_num_bytes 	2880

/* mnet_ssd_adas_flex_pic_prim_split_17 VCB conv5_4__dw_____bn_conv5_4__dw__scale_mul___muli___65_____bn_conv5_4__dw__scale_mul_____scale___muli___353 */
#define mnet_ssd_adas_flex_pic_prim_split_17_conv5_4__dw_____bn_conv5_4__dw__scale_mul___muli___65_____bn_conv5_4__dw__scale_mul_____scale___muli___353_cnngen_demangled_name 	"conv5_4__dw_____bn_conv5_4__dw__scale_mul___muli___65_____bn_conv5_4__dw__scale_mul_____scale___muli___353"
#define mnet_ssd_adas_flex_pic_prim_split_17_conv5_4__dw_____bn_conv5_4__dw__scale_mul___muli___65_____bn_conv5_4__dw__scale_mul_____scale___muli___353_is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_17_conv5_4__dw_____bn_conv5_4__dw__scale_mul___muli___65_____bn_conv5_4__dw__scale_mul_____scale___muli___353_is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_17_conv5_4__dw_____bn_conv5_4__dw__scale_mul___muli___65_____bn_conv5_4__dw__scale_mul_____scale___muli___353_is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_17_conv5_4__dw_____bn_conv5_4__dw__scale_mul___muli___65_____bn_conv5_4__dw__scale_mul_____scale___muli___353_byte_offset 	588	/* 0x0000024c */
#define mnet_ssd_adas_flex_pic_prim_split_17_conv5_4__dw_____bn_conv5_4__dw__scale_mul___muli___65_____bn_conv5_4__dw__scale_mul_____scale___muli___353_vbase_byte_offset 	596	/* 0x00000254 */
#define mnet_ssd_adas_flex_pic_prim_split_17_conv5_4__dw_____bn_conv5_4__dw__scale_mul___muli___65_____bn_conv5_4__dw__scale_mul_____scale___muli___353_vmem_buffer_num_bytes 	61440

/* mnet_ssd_adas_flex_pic_prim_split_17 HMB_output conv5_4__sep_____bn_conv5_4__sep__scale_mul___muli___68_____bn_conv5_4__sep__scale_mul_____scale___relu5_4__sep */
#define mnet_ssd_adas_flex_pic_prim_split_17_conv5_4__sep_____bn_conv5_4__sep__scale_mul___muli___68_____bn_conv5_4__sep__scale_mul_____scale___relu5_4__sep_cnngen_demangled_name 	"conv5_4__sep_____bn_conv5_4__sep__scale_mul___muli___68_____bn_conv5_4__sep__scale_mul_____scale___relu5_4__sep"
#define mnet_ssd_adas_flex_pic_prim_split_17_conv5_4__sep_____bn_conv5_4__sep__scale_mul___muli___68_____bn_conv5_4__sep__scale_mul_____scale___relu5_4__sep_is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_17_conv5_4__sep_____bn_conv5_4__sep__scale_mul___muli___68_____bn_conv5_4__sep__scale_mul_____scale___relu5_4__sep_is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_17_conv5_4__sep_____bn_conv5_4__sep__scale_mul___muli___68_____bn_conv5_4__sep__scale_mul_____scale___relu5_4__sep_is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_17_conv5_4__sep_____bn_conv5_4__sep__scale_mul___muli___68_____bn_conv5_4__sep__scale_mul_____scale___relu5_4__sep_is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_17_conv5_4__sep_____bn_conv5_4__sep__scale_mul___muli___68_____bn_conv5_4__sep__scale_mul_____scale___relu5_4__sep_byte_offset 	636	/* 0x0000027c */
#define mnet_ssd_adas_flex_pic_prim_split_17_conv5_4__sep_____bn_conv5_4__sep__scale_mul___muli___68_____bn_conv5_4__sep__scale_mul_____scale___relu5_4__sep_vbase_byte_offset 	640	/* 0x00000280 */
#define mnet_ssd_adas_flex_pic_prim_split_17_conv5_4__sep_____bn_conv5_4__sep__scale_mul___muli___68_____bn_conv5_4__sep__scale_mul_____scale___relu5_4__sep_dbase_byte_offset 	664	/* 0x00000298 */
#define mnet_ssd_adas_flex_pic_prim_split_17_conv5_4__sep_____bn_conv5_4__sep__scale_mul___muli___68_____bn_conv5_4__sep__scale_mul_____scale___relu5_4__sep_dpitchm1_byte_offset 	668	/* 0x0000029c */
#define mnet_ssd_adas_flex_pic_prim_split_17_conv5_4__sep_____bn_conv5_4__sep__scale_mul___muli___68_____bn_conv5_4__sep__scale_mul_____scale___relu5_4__sep_dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_17_conv5_4__sep_____bn_conv5_4__sep__scale_mul___muli___68_____bn_conv5_4__sep__scale_mul_____scale___relu5_4__sep_vwidth_minus_one_byte_offset 	652	/* 0x0000028c */
#define mnet_ssd_adas_flex_pic_prim_split_17_conv5_4__sep_____bn_conv5_4__sep__scale_mul___muli___68_____bn_conv5_4__sep__scale_mul_____scale___relu5_4__sep_vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_17_conv5_4__sep_____bn_conv5_4__sep__scale_mul___muli___68_____bn_conv5_4__sep__scale_mul_____scale___relu5_4__sep_vheight_minus_one_byte_offset 	654	/* 0x0000028e */
#define mnet_ssd_adas_flex_pic_prim_split_17_conv5_4__sep_____bn_conv5_4__sep__scale_mul___muli___68_____bn_conv5_4__sep__scale_mul_____scale___relu5_4__sep_vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_17_conv5_4__sep_____bn_conv5_4__sep__scale_mul___muli___68_____bn_conv5_4__sep__scale_mul_____scale___relu5_4__sep_drotate_bit_offset 	5286	/* 0x000014a6 */
#define mnet_ssd_adas_flex_pic_prim_split_17_conv5_4__sep_____bn_conv5_4__sep__scale_mul___muli___68_____bn_conv5_4__sep__scale_mul_____scale___relu5_4__sep_hflip_bit_offset 	5287	/* 0x000014a7 */
#define mnet_ssd_adas_flex_pic_prim_split_17_conv5_4__sep_____bn_conv5_4__sep__scale_mul___muli___68_____bn_conv5_4__sep__scale_mul_____scale___relu5_4__sep_vflip_bit_offset 	5288	/* 0x000014a8 */
#define mnet_ssd_adas_flex_pic_prim_split_17_conv5_4__sep_____bn_conv5_4__sep__scale_mul___muli___68_____bn_conv5_4__sep__scale_mul_____scale___relu5_4__sep_dflip_bit_offset 	5289	/* 0x000014a9 */
#define mnet_ssd_adas_flex_pic_prim_split_17_conv5_4__sep_____bn_conv5_4__sep__scale_mul___muli___68_____bn_conv5_4__sep__scale_mul_____scale___relu5_4__sep_pflip_bit_offset 	5290	/* 0x000014aa */
#define mnet_ssd_adas_flex_pic_prim_split_17_conv5_4__sep_____bn_conv5_4__sep__scale_mul___muli___68_____bn_conv5_4__sep__scale_mul_____scale___relu5_4__sep_W 	19
#define mnet_ssd_adas_flex_pic_prim_split_17_conv5_4__sep_____bn_conv5_4__sep__scale_mul___muli___68_____bn_conv5_4__sep__scale_mul_____scale___relu5_4__sep_H 	19
#define mnet_ssd_adas_flex_pic_prim_split_17_conv5_4__sep_____bn_conv5_4__sep__scale_mul___muli___68_____bn_conv5_4__sep__scale_mul_____scale___relu5_4__sep_D 	512
#define mnet_ssd_adas_flex_pic_prim_split_17_conv5_4__sep_____bn_conv5_4__sep__scale_mul___muli___68_____bn_conv5_4__sep__scale_mul_____scale___relu5_4__sep_P 	1
#define mnet_ssd_adas_flex_pic_prim_split_17_conv5_4__sep_____bn_conv5_4__sep__scale_mul___muli___68_____bn_conv5_4__sep__scale_mul_____scale___relu5_4__sep_data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_17_conv5_4__sep_____bn_conv5_4__sep__scale_mul___muli___68_____bn_conv5_4__sep__scale_mul_____scale___relu5_4__sep_denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_17_conv5_4__sep_____bn_conv5_4__sep__scale_mul___muli___68_____bn_conv5_4__sep__scale_mul_____scale___relu5_4__sep_dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_17_conv5_4__sep_____bn_conv5_4__sep__scale_mul___muli___68_____bn_conv5_4__sep__scale_mul_____scale___relu5_4__sep_dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_17_conv5_4__sep_____bn_conv5_4__sep__scale_mul___muli___68_____bn_conv5_4__sep__scale_mul_____scale___relu5_4__sep_vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_17_conv5_4__sep_____bn_conv5_4__sep__scale_mul___muli___68_____bn_conv5_4__sep__scale_mul_____scale___relu5_4__sep_tile_width 	2
#define mnet_ssd_adas_flex_pic_prim_split_17_conv5_4__sep_____bn_conv5_4__sep__scale_mul___muli___68_____bn_conv5_4__sep__scale_mul_____scale___relu5_4__sep_tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_17_conv5_4__sep_____bn_conv5_4__sep__scale_mul___muli___68_____bn_conv5_4__sep__scale_mul_____scale___relu5_4__sep_vmem_buffer_num_bytes 	49152
#define mnet_ssd_adas_flex_pic_prim_split_17_conv5_4__sep_____bn_conv5_4__sep__scale_mul___muli___68_____bn_conv5_4__sep__scale_mul_____scale___relu5_4__sep_dram_size_num_bytes 	233472
#define mnet_ssd_adas_flex_pic_prim_split_17_conv5_4__sep_____bn_conv5_4__sep__scale_mul___muli___68_____bn_conv5_4__sep__scale_mul_____scale___relu5_4__sep_sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_17_conv5_4__sep_____bn_conv5_4__sep__scale_mul___muli___68_____bn_conv5_4__sep__scale_mul_____scale___relu5_4__sep_datasize 	0
#define mnet_ssd_adas_flex_pic_prim_split_17_conv5_4__sep_____bn_conv5_4__sep__scale_mul___muli___68_____bn_conv5_4__sep__scale_mul_____scale___relu5_4__sep_expoffset 	8
#define mnet_ssd_adas_flex_pic_prim_split_17_conv5_4__sep_____bn_conv5_4__sep__scale_mul___muli___68_____bn_conv5_4__sep__scale_mul_____scale___relu5_4__sep_expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_17_conv5_4__sep_____bn_conv5_4__sep__scale_mul___muli___68_____bn_conv5_4__sep__scale_mul_____scale___relu5_4__sep_drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_17_conv5_4__sep_____bn_conv5_4__sep__scale_mul___muli___68_____bn_conv5_4__sep__scale_mul_____scale___relu5_4__sep_hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_17_conv5_4__sep_____bn_conv5_4__sep__scale_mul___muli___68_____bn_conv5_4__sep__scale_mul_____scale___relu5_4__sep_vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_17_conv5_4__sep_____bn_conv5_4__sep__scale_mul___muli___68_____bn_conv5_4__sep__scale_mul_____scale___relu5_4__sep_dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_17_conv5_4__sep_____bn_conv5_4__sep__scale_mul___muli___68_____bn_conv5_4__sep__scale_mul_____scale___relu5_4__sep_pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_17_conv5_4__sep_____bn_conv5_4__sep__scale_mul___muli___68_____bn_conv5_4__sep__scale_mul_____scale___relu5_4__sep_trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_17 SMB_output __vas_created_terminate__v0 */
#define mnet_ssd_adas_flex_pic_prim_split_17___vas_created_terminate__v0_cnngen_demangled_name 	"__vas_created_terminate__v0"
#define mnet_ssd_adas_flex_pic_prim_split_17___vas_created_terminate__v0_is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___vas_created_terminate__v0_is_bitvector 	1
#define mnet_ssd_adas_flex_pic_prim_split_17___vas_created_terminate__v0_is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___vas_created_terminate__v0_is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___vas_created_terminate__v0_byte_offset 	676	/* 0x000002a4 */
#define mnet_ssd_adas_flex_pic_prim_split_17___vas_created_terminate__v0_vbase_byte_offset 	680	/* 0x000002a8 */
#define mnet_ssd_adas_flex_pic_prim_split_17___vas_created_terminate__v0_W 	256
#define mnet_ssd_adas_flex_pic_prim_split_17___vas_created_terminate__v0_H 	3
#define mnet_ssd_adas_flex_pic_prim_split_17___vas_created_terminate__v0_D 	1
#define mnet_ssd_adas_flex_pic_prim_split_17___vas_created_terminate__v0_P 	1
#define mnet_ssd_adas_flex_pic_prim_split_17___vas_created_terminate__v0_data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_17___vas_created_terminate__v0_denable 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___vas_created_terminate__v0_dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_17___vas_created_terminate__v0_vp_interleave_mode 	1
#define mnet_ssd_adas_flex_pic_prim_split_17___vas_created_terminate__v0_tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_17___vas_created_terminate__v0_tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_17___vas_created_terminate__v0_vmem_buffer_num_bytes 	96


#endif /* mnet_ssd_adas_flex_pic_prim_split_17_H */
