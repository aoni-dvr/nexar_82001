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
#ifndef mnet_ssd_adas_flex_pic_prim_split_16_H
#define mnet_ssd_adas_flex_pic_prim_split_16_H

#define vas_gen_header_version                  	7

/* dag name */
#define mnet_ssd_adas_flex_pic_prim_split_16_DAG_NAME 	"mnet_ssd_adas_flex_pic_prim_split_16"

/* VDG file info */
#define mnet_ssd_adas_flex_pic_prim_split_16_vdg_name 	"mnet_ssd_adas_flex_pic_split_16.vdg"

/* VDG DAG memory info */
#define mnet_ssd_adas_flex_pic_prim_split_16_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_16_byte_size 	1476	/* 0x000005c4 */
#define mnet_ssd_adas_flex_pic_prim_split_16_preferred_dram_xfer_size 	1

/* mnet_ssd_adas_flex_pic_prim_split_16 VMEM info */
#define mnet_ssd_adas_flex_pic_prim_split_16_use_ping_pong_vmem 	0
#define mnet_ssd_adas_flex_pic_prim_split_16_VMEM_start 	4096	/* 0x00001000 */
#define mnet_ssd_adas_flex_pic_prim_split_16_VMEM_end 	508768	/* 0x0007c360 */
#define mnet_ssd_adas_flex_pic_prim_split_16_image_start 	508768	/* 0x0007c360 */
#define mnet_ssd_adas_flex_pic_prim_split_16_image_size 	1476	/* 0x000005c4 */
#define mnet_ssd_adas_flex_pic_prim_split_16_dagbin_start 	508768	/* 0x0007c360 */

/* mnet_ssd_adas_flex_pic_prim_split_16 DAG info */
#define mnet_ssd_adas_flex_pic_prim_split_16_estimated_cycles 	52339
#define mnet_ssd_adas_flex_pic_prim_split_16_ddi_byte_size 	0	/* 0x00000000 */

/*******************************************************************************
 * DAG byte offset info for HMB/SMB/VCB and their fields
 * in DAG 'mnet_ssd_adas_flex_pic_prim_split_16' in source file 'pre_split16_mnet_ssd_adas_flex_pic_split_0.vas'
 ******************************************************************************/
/* mnet_ssd_adas_flex_pic_prim_split_16 HMB_input __pvcn_1802_ */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1802__cnngen_demangled_name 	"__pvcn_1802_"
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1802__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1802__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1802__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1802__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1802__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1802__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1802__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1802__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1802__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1802__byte_offset 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1802__vbase_byte_offset 	4	/* 0x00000004 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1802__dbase_byte_offset 	28	/* 0x0000001c */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1802__dpitchm1_byte_offset 	32	/* 0x00000020 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1802__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1802__vwidth_minus_one_byte_offset 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1802__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1802__vheight_minus_one_byte_offset 	18	/* 0x00000012 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1802__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1802__drotate_bit_offset 	198	/* 0x000000c6 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1802__hflip_bit_offset 	199	/* 0x000000c7 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1802__vflip_bit_offset 	200	/* 0x000000c8 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1802__dflip_bit_offset 	201	/* 0x000000c9 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1802__pflip_bit_offset 	202	/* 0x000000ca */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1802__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1802__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1802__D 	38
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1802__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1802__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1802__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1802__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1802__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1802__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1802__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1802__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1802__vmem_buffer_num_bytes 	29184
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1802__dram_size_num_bytes 	34656
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1802__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1802__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1802__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1802__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1802__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1802__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1802__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1802__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1802__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1802__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_16 HMB_input __pvcn_1758_ */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1758__cnngen_demangled_name 	"__pvcn_1758_"
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1758__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1758__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1758__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1758__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1758__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1758__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1758__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1758__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1758__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1758__byte_offset 	40	/* 0x00000028 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1758__vbase_byte_offset 	44	/* 0x0000002c */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1758__dbase_byte_offset 	68	/* 0x00000044 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1758__dpitchm1_byte_offset 	72	/* 0x00000048 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1758__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1758__vwidth_minus_one_byte_offset 	56	/* 0x00000038 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1758__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1758__vheight_minus_one_byte_offset 	58	/* 0x0000003a */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1758__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1758__drotate_bit_offset 	518	/* 0x00000206 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1758__hflip_bit_offset 	519	/* 0x00000207 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1758__vflip_bit_offset 	520	/* 0x00000208 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1758__dflip_bit_offset 	521	/* 0x00000209 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1758__pflip_bit_offset 	522	/* 0x0000020a */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1758__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1758__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1758__D 	40
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1758__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1758__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1758__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1758__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1758__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1758__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1758__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1758__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1758__vmem_buffer_num_bytes 	30720
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1758__dram_size_num_bytes 	36480
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1758__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1758__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1758__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1758__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1758__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1758__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1758__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1758__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1758__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1758__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_16 HMB_input __pvcn_1813_ */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1813__cnngen_demangled_name 	"__pvcn_1813_"
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1813__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1813__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1813__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1813__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1813__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1813__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1813__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1813__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1813__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1813__byte_offset 	80	/* 0x00000050 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1813__vbase_byte_offset 	84	/* 0x00000054 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1813__dbase_byte_offset 	108	/* 0x0000006c */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1813__dpitchm1_byte_offset 	112	/* 0x00000070 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1813__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1813__vwidth_minus_one_byte_offset 	96	/* 0x00000060 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1813__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1813__vheight_minus_one_byte_offset 	98	/* 0x00000062 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1813__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1813__drotate_bit_offset 	838	/* 0x00000346 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1813__hflip_bit_offset 	839	/* 0x00000347 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1813__vflip_bit_offset 	840	/* 0x00000348 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1813__dflip_bit_offset 	841	/* 0x00000349 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1813__pflip_bit_offset 	842	/* 0x0000034a */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1813__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1813__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1813__D 	29
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1813__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1813__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1813__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1813__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1813__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1813__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1813__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1813__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1813__vmem_buffer_num_bytes 	22272
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1813__dram_size_num_bytes 	26464
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1813__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1813__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1813__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1813__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1813__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1813__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1813__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1813__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1813__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1813__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_16 HMB_input __pvcn_1791_ */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1791__cnngen_demangled_name 	"__pvcn_1791_"
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1791__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1791__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1791__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1791__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1791__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1791__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1791__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1791__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1791__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1791__byte_offset 	120	/* 0x00000078 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1791__vbase_byte_offset 	124	/* 0x0000007c */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1791__dbase_byte_offset 	148	/* 0x00000094 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1791__dpitchm1_byte_offset 	152	/* 0x00000098 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1791__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1791__vwidth_minus_one_byte_offset 	136	/* 0x00000088 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1791__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1791__vheight_minus_one_byte_offset 	138	/* 0x0000008a */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1791__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1791__drotate_bit_offset 	1158	/* 0x00000486 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1791__hflip_bit_offset 	1159	/* 0x00000487 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1791__vflip_bit_offset 	1160	/* 0x00000488 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1791__dflip_bit_offset 	1161	/* 0x00000489 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1791__pflip_bit_offset 	1162	/* 0x0000048a */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1791__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1791__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1791__D 	36
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1791__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1791__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1791__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1791__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1791__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1791__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1791__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1791__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1791__vmem_buffer_num_bytes 	27648
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1791__dram_size_num_bytes 	32832
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1791__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1791__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1791__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1791__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1791__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1791__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1791__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1791__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1791__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1791__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_16 HMB_input __pvcn_1670_ */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1670__cnngen_demangled_name 	"__pvcn_1670_"
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1670__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1670__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1670__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1670__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1670__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1670__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1670__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1670__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1670__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1670__byte_offset 	160	/* 0x000000a0 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1670__vbase_byte_offset 	164	/* 0x000000a4 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1670__dbase_byte_offset 	188	/* 0x000000bc */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1670__dpitchm1_byte_offset 	192	/* 0x000000c0 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1670__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1670__vwidth_minus_one_byte_offset 	176	/* 0x000000b0 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1670__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1670__vheight_minus_one_byte_offset 	178	/* 0x000000b2 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1670__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1670__drotate_bit_offset 	1478	/* 0x000005c6 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1670__hflip_bit_offset 	1479	/* 0x000005c7 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1670__vflip_bit_offset 	1480	/* 0x000005c8 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1670__dflip_bit_offset 	1481	/* 0x000005c9 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1670__pflip_bit_offset 	1482	/* 0x000005ca */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1670__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1670__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1670__D 	35
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1670__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1670__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1670__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1670__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1670__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1670__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1670__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1670__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1670__vmem_buffer_num_bytes 	26880
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1670__dram_size_num_bytes 	31936
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1670__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1670__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1670__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1670__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1670__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1670__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1670__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1670__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1670__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1670__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_16 HMB_input __pvcn_1780_ */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1780__cnngen_demangled_name 	"__pvcn_1780_"
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1780__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1780__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1780__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1780__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1780__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1780__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1780__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1780__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1780__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1780__byte_offset 	196	/* 0x000000c4 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1780__vbase_byte_offset 	200	/* 0x000000c8 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1780__dbase_byte_offset 	224	/* 0x000000e0 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1780__dpitchm1_byte_offset 	228	/* 0x000000e4 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1780__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1780__vwidth_minus_one_byte_offset 	212	/* 0x000000d4 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1780__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1780__vheight_minus_one_byte_offset 	214	/* 0x000000d6 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1780__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1780__drotate_bit_offset 	1766	/* 0x000006e6 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1780__hflip_bit_offset 	1767	/* 0x000006e7 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1780__vflip_bit_offset 	1768	/* 0x000006e8 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1780__dflip_bit_offset 	1769	/* 0x000006e9 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1780__pflip_bit_offset 	1770	/* 0x000006ea */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1780__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1780__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1780__D 	40
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1780__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1780__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1780__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1780__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1780__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1780__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1780__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1780__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1780__vmem_buffer_num_bytes 	30720
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1780__dram_size_num_bytes 	36480
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1780__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1780__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1780__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1780__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1780__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1780__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1780__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1780__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1780__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1780__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_16 HMB_input __pvcn_1692_ */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1692__cnngen_demangled_name 	"__pvcn_1692_"
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1692__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1692__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1692__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1692__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1692__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1692__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1692__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1692__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1692__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1692__byte_offset 	236	/* 0x000000ec */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1692__vbase_byte_offset 	240	/* 0x000000f0 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1692__dbase_byte_offset 	264	/* 0x00000108 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1692__dpitchm1_byte_offset 	268	/* 0x0000010c */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1692__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1692__vwidth_minus_one_byte_offset 	252	/* 0x000000fc */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1692__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1692__vheight_minus_one_byte_offset 	254	/* 0x000000fe */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1692__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1692__drotate_bit_offset 	2086	/* 0x00000826 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1692__hflip_bit_offset 	2087	/* 0x00000827 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1692__vflip_bit_offset 	2088	/* 0x00000828 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1692__dflip_bit_offset 	2089	/* 0x00000829 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1692__pflip_bit_offset 	2090	/* 0x0000082a */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1692__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1692__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1692__D 	36
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1692__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1692__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1692__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1692__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1692__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1692__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1692__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1692__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1692__vmem_buffer_num_bytes 	27648
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1692__dram_size_num_bytes 	32832
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1692__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1692__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1692__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1692__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1692__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1692__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1692__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1692__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1692__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1692__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_16 HMB_input __pvcn_1769_ */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1769__cnngen_demangled_name 	"__pvcn_1769_"
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1769__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1769__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1769__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1769__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1769__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1769__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1769__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1769__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1769__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1769__byte_offset 	276	/* 0x00000114 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1769__vbase_byte_offset 	280	/* 0x00000118 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1769__dbase_byte_offset 	304	/* 0x00000130 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1769__dpitchm1_byte_offset 	308	/* 0x00000134 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1769__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1769__vwidth_minus_one_byte_offset 	292	/* 0x00000124 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1769__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1769__vheight_minus_one_byte_offset 	294	/* 0x00000126 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1769__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1769__drotate_bit_offset 	2406	/* 0x00000966 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1769__hflip_bit_offset 	2407	/* 0x00000967 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1769__vflip_bit_offset 	2408	/* 0x00000968 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1769__dflip_bit_offset 	2409	/* 0x00000969 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1769__pflip_bit_offset 	2410	/* 0x0000096a */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1769__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1769__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1769__D 	36
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1769__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1769__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1769__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1769__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1769__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1769__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1769__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1769__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1769__vmem_buffer_num_bytes 	27648
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1769__dram_size_num_bytes 	32832
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1769__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1769__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1769__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1769__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1769__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1769__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1769__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1769__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1769__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1769__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_16 HMB_input __pvcn_1747_ */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1747__cnngen_demangled_name 	"__pvcn_1747_"
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1747__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1747__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1747__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1747__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1747__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1747__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1747__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1747__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1747__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1747__byte_offset 	316	/* 0x0000013c */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1747__vbase_byte_offset 	320	/* 0x00000140 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1747__dbase_byte_offset 	344	/* 0x00000158 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1747__dpitchm1_byte_offset 	348	/* 0x0000015c */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1747__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1747__vwidth_minus_one_byte_offset 	332	/* 0x0000014c */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1747__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1747__vheight_minus_one_byte_offset 	334	/* 0x0000014e */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1747__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1747__drotate_bit_offset 	2726	/* 0x00000aa6 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1747__hflip_bit_offset 	2727	/* 0x00000aa7 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1747__vflip_bit_offset 	2728	/* 0x00000aa8 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1747__dflip_bit_offset 	2729	/* 0x00000aa9 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1747__pflip_bit_offset 	2730	/* 0x00000aaa */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1747__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1747__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1747__D 	37
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1747__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1747__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1747__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1747__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1747__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1747__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1747__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1747__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1747__vmem_buffer_num_bytes 	14208
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1747__dram_size_num_bytes 	33760
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1747__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1747__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1747__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1747__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1747__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1747__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1747__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1747__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1747__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1747__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_16 HMB_input __pvcn_1736_ */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1736__cnngen_demangled_name 	"__pvcn_1736_"
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1736__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1736__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1736__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1736__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1736__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1736__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1736__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1736__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1736__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1736__byte_offset 	356	/* 0x00000164 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1736__vbase_byte_offset 	360	/* 0x00000168 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1736__dbase_byte_offset 	384	/* 0x00000180 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1736__dpitchm1_byte_offset 	388	/* 0x00000184 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1736__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1736__vwidth_minus_one_byte_offset 	372	/* 0x00000174 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1736__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1736__vheight_minus_one_byte_offset 	374	/* 0x00000176 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1736__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1736__drotate_bit_offset 	3046	/* 0x00000be6 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1736__hflip_bit_offset 	3047	/* 0x00000be7 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1736__vflip_bit_offset 	3048	/* 0x00000be8 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1736__dflip_bit_offset 	3049	/* 0x00000be9 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1736__pflip_bit_offset 	3050	/* 0x00000bea */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1736__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1736__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1736__D 	36
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1736__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1736__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1736__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1736__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1736__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1736__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1736__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1736__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1736__vmem_buffer_num_bytes 	13824
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1736__dram_size_num_bytes 	32832
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1736__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1736__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1736__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1736__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1736__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1736__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1736__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1736__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1736__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1736__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_16 HMB_input __pvcn_1725_ */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1725__cnngen_demangled_name 	"__pvcn_1725_"
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1725__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1725__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1725__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1725__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1725__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1725__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1725__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1725__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1725__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1725__byte_offset 	396	/* 0x0000018c */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1725__vbase_byte_offset 	400	/* 0x00000190 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1725__dbase_byte_offset 	424	/* 0x000001a8 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1725__dpitchm1_byte_offset 	428	/* 0x000001ac */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1725__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1725__vwidth_minus_one_byte_offset 	412	/* 0x0000019c */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1725__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1725__vheight_minus_one_byte_offset 	414	/* 0x0000019e */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1725__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1725__drotate_bit_offset 	3366	/* 0x00000d26 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1725__hflip_bit_offset 	3367	/* 0x00000d27 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1725__vflip_bit_offset 	3368	/* 0x00000d28 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1725__dflip_bit_offset 	3369	/* 0x00000d29 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1725__pflip_bit_offset 	3370	/* 0x00000d2a */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1725__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1725__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1725__D 	38
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1725__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1725__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1725__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1725__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1725__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1725__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1725__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1725__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1725__vmem_buffer_num_bytes 	14592
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1725__dram_size_num_bytes 	34656
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1725__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1725__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1725__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1725__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1725__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1725__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1725__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1725__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1725__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1725__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_16 HMB_input __pvcn_1714_ */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1714__cnngen_demangled_name 	"__pvcn_1714_"
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1714__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1714__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1714__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1714__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1714__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1714__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1714__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1714__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1714__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1714__byte_offset 	436	/* 0x000001b4 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1714__vbase_byte_offset 	440	/* 0x000001b8 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1714__dbase_byte_offset 	464	/* 0x000001d0 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1714__dpitchm1_byte_offset 	468	/* 0x000001d4 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1714__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1714__vwidth_minus_one_byte_offset 	452	/* 0x000001c4 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1714__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1714__vheight_minus_one_byte_offset 	454	/* 0x000001c6 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1714__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1714__drotate_bit_offset 	3686	/* 0x00000e66 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1714__hflip_bit_offset 	3687	/* 0x00000e67 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1714__vflip_bit_offset 	3688	/* 0x00000e68 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1714__dflip_bit_offset 	3689	/* 0x00000e69 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1714__pflip_bit_offset 	3690	/* 0x00000e6a */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1714__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1714__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1714__D 	40
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1714__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1714__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1714__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1714__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1714__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1714__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1714__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1714__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1714__vmem_buffer_num_bytes 	15360
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1714__dram_size_num_bytes 	36480
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1714__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1714__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1714__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1714__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1714__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1714__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1714__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1714__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1714__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1714__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_16 HMB_input __pvcn_1703_ */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1703__cnngen_demangled_name 	"__pvcn_1703_"
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1703__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1703__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1703__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1703__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1703__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1703__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1703__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1703__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1703__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1703__byte_offset 	476	/* 0x000001dc */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1703__vbase_byte_offset 	480	/* 0x000001e0 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1703__dbase_byte_offset 	504	/* 0x000001f8 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1703__dpitchm1_byte_offset 	508	/* 0x000001fc */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1703__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1703__vwidth_minus_one_byte_offset 	492	/* 0x000001ec */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1703__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1703__vheight_minus_one_byte_offset 	494	/* 0x000001ee */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1703__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1703__drotate_bit_offset 	4006	/* 0x00000fa6 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1703__hflip_bit_offset 	4007	/* 0x00000fa7 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1703__vflip_bit_offset 	4008	/* 0x00000fa8 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1703__dflip_bit_offset 	4009	/* 0x00000fa9 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1703__pflip_bit_offset 	4010	/* 0x00000faa */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1703__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1703__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1703__D 	35
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1703__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1703__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1703__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1703__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1703__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1703__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1703__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1703__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1703__vmem_buffer_num_bytes 	13440
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1703__dram_size_num_bytes 	31936
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1703__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1703__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1703__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1703__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1703__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1703__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1703__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1703__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1703__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1703__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_16 HMB_input __pvcn_1681_ */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1681__cnngen_demangled_name 	"__pvcn_1681_"
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1681__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1681__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1681__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1681__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1681__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1681__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1681__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1681__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1681__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1681__byte_offset 	516	/* 0x00000204 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1681__vbase_byte_offset 	520	/* 0x00000208 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1681__dbase_byte_offset 	544	/* 0x00000220 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1681__dpitchm1_byte_offset 	548	/* 0x00000224 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1681__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1681__vwidth_minus_one_byte_offset 	532	/* 0x00000214 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1681__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1681__vheight_minus_one_byte_offset 	534	/* 0x00000216 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1681__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1681__drotate_bit_offset 	4326	/* 0x000010e6 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1681__hflip_bit_offset 	4327	/* 0x000010e7 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1681__vflip_bit_offset 	4328	/* 0x000010e8 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1681__dflip_bit_offset 	4329	/* 0x000010e9 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1681__pflip_bit_offset 	4330	/* 0x000010ea */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1681__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1681__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1681__D 	36
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1681__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1681__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1681__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1681__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1681__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1681__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1681__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1681__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1681__vmem_buffer_num_bytes 	13824
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1681__dram_size_num_bytes 	32832
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1681__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1681__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1681__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1681__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1681__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1681__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1681__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1681__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1681__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1681__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_16 HMB_output __pvcn_1830_ */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1830__cnngen_demangled_name 	"__pvcn_1830_"
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1830__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1830__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1830__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1830__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1830__byte_offset 	1072	/* 0x00000430 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1830__vbase_byte_offset 	1076	/* 0x00000434 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1830__dbase_byte_offset 	1100	/* 0x0000044c */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1830__dpitchm1_byte_offset 	1104	/* 0x00000450 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1830__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1830__vwidth_minus_one_byte_offset 	1088	/* 0x00000440 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1830__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1830__vheight_minus_one_byte_offset 	1090	/* 0x00000442 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1830__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1830__drotate_bit_offset 	8774	/* 0x00002246 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1830__hflip_bit_offset 	8775	/* 0x00002247 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1830__vflip_bit_offset 	8776	/* 0x00002248 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1830__dflip_bit_offset 	8777	/* 0x00002249 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1830__pflip_bit_offset 	8778	/* 0x0000224a */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1830__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1830__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1830__D 	71
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1830__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1830__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1830__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1830__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1830__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1830__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1830__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1830__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1830__vmem_buffer_num_bytes 	27264
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1830__dram_size_num_bytes 	64768
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1830__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1830__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1830__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1830__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1830__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1830__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1830__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1830__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1830__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1830__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_16 HMB_output __pvcn_1831_ */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1831__cnngen_demangled_name 	"__pvcn_1831_"
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1831__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1831__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1831__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1831__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1831__byte_offset 	1120	/* 0x00000460 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1831__vbase_byte_offset 	1124	/* 0x00000464 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1831__dbase_byte_offset 	1148	/* 0x0000047c */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1831__dpitchm1_byte_offset 	1152	/* 0x00000480 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1831__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1831__vwidth_minus_one_byte_offset 	1136	/* 0x00000470 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1831__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1831__vheight_minus_one_byte_offset 	1138	/* 0x00000472 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1831__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1831__drotate_bit_offset 	9158	/* 0x000023c6 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1831__hflip_bit_offset 	9159	/* 0x000023c7 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1831__vflip_bit_offset 	9160	/* 0x000023c8 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1831__dflip_bit_offset 	9161	/* 0x000023c9 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1831__pflip_bit_offset 	9162	/* 0x000023ca */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1831__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1831__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1831__D 	71
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1831__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1831__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1831__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1831__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1831__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1831__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1831__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1831__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1831__vmem_buffer_num_bytes 	27264
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1831__dram_size_num_bytes 	64768
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1831__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1831__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1831__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1831__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1831__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1831__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1831__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1831__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1831__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1831__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_16 HMB_output __pvcn_1832_ */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1832__cnngen_demangled_name 	"__pvcn_1832_"
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1832__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1832__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1832__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1832__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1832__byte_offset 	1168	/* 0x00000490 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1832__vbase_byte_offset 	1172	/* 0x00000494 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1832__dbase_byte_offset 	1196	/* 0x000004ac */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1832__dpitchm1_byte_offset 	1200	/* 0x000004b0 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1832__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1832__vwidth_minus_one_byte_offset 	1184	/* 0x000004a0 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1832__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1832__vheight_minus_one_byte_offset 	1186	/* 0x000004a2 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1832__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1832__drotate_bit_offset 	9542	/* 0x00002546 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1832__hflip_bit_offset 	9543	/* 0x00002547 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1832__vflip_bit_offset 	9544	/* 0x00002548 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1832__dflip_bit_offset 	9545	/* 0x00002549 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1832__pflip_bit_offset 	9546	/* 0x0000254a */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1832__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1832__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1832__D 	78
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1832__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1832__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1832__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1832__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1832__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1832__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1832__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1832__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1832__vmem_buffer_num_bytes 	29952
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1832__dram_size_num_bytes 	71136
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1832__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1832__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1832__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1832__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1832__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1832__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1832__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1832__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1832__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1832__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_16 HMB_output __pvcn_1833_ */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1833__cnngen_demangled_name 	"__pvcn_1833_"
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1833__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1833__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1833__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1833__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1833__byte_offset 	1216	/* 0x000004c0 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1833__vbase_byte_offset 	1220	/* 0x000004c4 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1833__dbase_byte_offset 	1244	/* 0x000004dc */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1833__dpitchm1_byte_offset 	1248	/* 0x000004e0 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1833__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1833__vwidth_minus_one_byte_offset 	1232	/* 0x000004d0 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1833__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1833__vheight_minus_one_byte_offset 	1234	/* 0x000004d2 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1833__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1833__drotate_bit_offset 	9926	/* 0x000026c6 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1833__hflip_bit_offset 	9927	/* 0x000026c7 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1833__vflip_bit_offset 	9928	/* 0x000026c8 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1833__dflip_bit_offset 	9929	/* 0x000026c9 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1833__pflip_bit_offset 	9930	/* 0x000026ca */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1833__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1833__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1833__D 	73
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1833__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1833__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1833__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1833__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1833__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1833__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1833__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1833__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1833__vmem_buffer_num_bytes 	28032
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1833__dram_size_num_bytes 	66592
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1833__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1833__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1833__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1833__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1833__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1833__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1833__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1833__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1833__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1833__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_16 HMB_output __pvcn_1834_ */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1834__cnngen_demangled_name 	"__pvcn_1834_"
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1834__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1834__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1834__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1834__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1834__byte_offset 	1264	/* 0x000004f0 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1834__vbase_byte_offset 	1268	/* 0x000004f4 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1834__dbase_byte_offset 	1292	/* 0x0000050c */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1834__dpitchm1_byte_offset 	1296	/* 0x00000510 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1834__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1834__vwidth_minus_one_byte_offset 	1280	/* 0x00000500 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1834__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1834__vheight_minus_one_byte_offset 	1282	/* 0x00000502 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1834__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1834__drotate_bit_offset 	10310	/* 0x00002846 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1834__hflip_bit_offset 	10311	/* 0x00002847 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1834__vflip_bit_offset 	10312	/* 0x00002848 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1834__dflip_bit_offset 	10313	/* 0x00002849 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1834__pflip_bit_offset 	10314	/* 0x0000284a */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1834__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1834__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1834__D 	76
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1834__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1834__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1834__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1834__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1834__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1834__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1834__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1834__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1834__vmem_buffer_num_bytes 	29184
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1834__dram_size_num_bytes 	69312
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1834__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1834__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1834__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1834__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1834__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1834__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1834__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1834__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1834__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1834__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_16 HMB_output __pvcn_1835_ */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1835__cnngen_demangled_name 	"__pvcn_1835_"
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1835__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1835__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1835__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1835__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1835__byte_offset 	1312	/* 0x00000520 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1835__vbase_byte_offset 	1316	/* 0x00000524 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1835__dbase_byte_offset 	1340	/* 0x0000053c */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1835__dpitchm1_byte_offset 	1344	/* 0x00000540 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1835__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1835__vwidth_minus_one_byte_offset 	1328	/* 0x00000530 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1835__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1835__vheight_minus_one_byte_offset 	1330	/* 0x00000532 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1835__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1835__drotate_bit_offset 	10694	/* 0x000029c6 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1835__hflip_bit_offset 	10695	/* 0x000029c7 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1835__vflip_bit_offset 	10696	/* 0x000029c8 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1835__dflip_bit_offset 	10697	/* 0x000029c9 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1835__pflip_bit_offset 	10698	/* 0x000029ca */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1835__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1835__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1835__D 	76
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1835__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1835__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1835__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1835__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1835__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1835__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1835__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1835__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1835__vmem_buffer_num_bytes 	29184
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1835__dram_size_num_bytes 	69312
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1835__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1835__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1835__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1835__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1835__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1835__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1835__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1835__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1835__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1835__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_16 HMB_output __pvcn_1836_ */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1836__cnngen_demangled_name 	"__pvcn_1836_"
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1836__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1836__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1836__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1836__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1836__byte_offset 	1360	/* 0x00000550 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1836__vbase_byte_offset 	1364	/* 0x00000554 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1836__dbase_byte_offset 	1388	/* 0x0000056c */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1836__dpitchm1_byte_offset 	1392	/* 0x00000570 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1836__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1836__vwidth_minus_one_byte_offset 	1376	/* 0x00000560 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1836__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1836__vheight_minus_one_byte_offset 	1378	/* 0x00000562 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1836__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1836__drotate_bit_offset 	11078	/* 0x00002b46 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1836__hflip_bit_offset 	11079	/* 0x00002b47 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1836__vflip_bit_offset 	11080	/* 0x00002b48 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1836__dflip_bit_offset 	11081	/* 0x00002b49 */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1836__pflip_bit_offset 	11082	/* 0x00002b4a */
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1836__W 	19
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1836__H 	19
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1836__D 	67
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1836__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1836__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1836__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1836__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1836__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1836__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1836__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1836__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1836__vmem_buffer_num_bytes 	25728
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1836__dram_size_num_bytes 	61120
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1836__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1836__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1836__expoffset 	14
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1836__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1836__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1836__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1836__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1836__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1836__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___pvcn_1836__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_16 SMB_output __vas_created_terminate__v0 */
#define mnet_ssd_adas_flex_pic_prim_split_16___vas_created_terminate__v0_cnngen_demangled_name 	"__vas_created_terminate__v0"
#define mnet_ssd_adas_flex_pic_prim_split_16___vas_created_terminate__v0_is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___vas_created_terminate__v0_is_bitvector 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___vas_created_terminate__v0_is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___vas_created_terminate__v0_is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___vas_created_terminate__v0_byte_offset 	1400	/* 0x00000578 */
#define mnet_ssd_adas_flex_pic_prim_split_16___vas_created_terminate__v0_vbase_byte_offset 	1404	/* 0x0000057c */
#define mnet_ssd_adas_flex_pic_prim_split_16___vas_created_terminate__v0_W 	256
#define mnet_ssd_adas_flex_pic_prim_split_16___vas_created_terminate__v0_H 	3
#define mnet_ssd_adas_flex_pic_prim_split_16___vas_created_terminate__v0_D 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___vas_created_terminate__v0_P 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___vas_created_terminate__v0_data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___vas_created_terminate__v0_denable 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___vas_created_terminate__v0_dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_16___vas_created_terminate__v0_vp_interleave_mode 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___vas_created_terminate__v0_tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___vas_created_terminate__v0_tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_16___vas_created_terminate__v0_vmem_buffer_num_bytes 	96


#endif /* mnet_ssd_adas_flex_pic_prim_split_16_H */
