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
#ifndef mnet_ssd_adas_flex_pic_prim_split_6_H
#define mnet_ssd_adas_flex_pic_prim_split_6_H

#define vas_gen_header_version                  	7

/* dag name */
#define mnet_ssd_adas_flex_pic_prim_split_6_DAG_NAME 	"mnet_ssd_adas_flex_pic_prim_split_6"

/* VDG file info */
#define mnet_ssd_adas_flex_pic_prim_split_6_vdg_name 	"mnet_ssd_adas_flex_pic_split_6.vdg"

/* VDG DAG memory info */
#define mnet_ssd_adas_flex_pic_prim_split_6_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_6_byte_size 	2164	/* 0x00000874 */
#define mnet_ssd_adas_flex_pic_prim_split_6_preferred_dram_xfer_size 	1

/* mnet_ssd_adas_flex_pic_prim_split_6 VMEM info */
#define mnet_ssd_adas_flex_pic_prim_split_6_use_ping_pong_vmem 	0
#define mnet_ssd_adas_flex_pic_prim_split_6_VMEM_start 	4096	/* 0x00001000 */
#define mnet_ssd_adas_flex_pic_prim_split_6_VMEM_end 	513620	/* 0x0007d654 */
#define mnet_ssd_adas_flex_pic_prim_split_6_image_start 	475872	/* 0x000742e0 */
#define mnet_ssd_adas_flex_pic_prim_split_6_image_size 	39912	/* 0x00009be8 */
#define mnet_ssd_adas_flex_pic_prim_split_6_dagbin_start 	513620	/* 0x0007d654 */

/* mnet_ssd_adas_flex_pic_prim_split_6 DAG info */
#define mnet_ssd_adas_flex_pic_prim_split_6_estimated_cycles 	148602
#define mnet_ssd_adas_flex_pic_prim_split_6_ddi_byte_size 	0	/* 0x00000000 */

/*******************************************************************************
 * DAG byte offset info for HMB/SMB/VCB and their fields
 * in DAG 'mnet_ssd_adas_flex_pic_prim_split_6' in source file 'pre_split6_mnet_ssd_adas_flex_pic_split_0.vas'
 ******************************************************************************/
/* mnet_ssd_adas_flex_pic_prim_split_6 HMB_input __pvcn_755_ */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_755__cnngen_demangled_name 	"__pvcn_755_"
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_755__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_755__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_755__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_755__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_755__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_755__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_755__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_755__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_755__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_755__byte_offset 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_755__vbase_byte_offset 	4	/* 0x00000004 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_755__dbase_byte_offset 	28	/* 0x0000001c */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_755__dpitchm1_byte_offset 	32	/* 0x00000020 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_755__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_755__vwidth_minus_one_byte_offset 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_755__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_755__vheight_minus_one_byte_offset 	18	/* 0x00000012 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_755__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_755__drotate_bit_offset 	198	/* 0x000000c6 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_755__hflip_bit_offset 	199	/* 0x000000c7 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_755__vflip_bit_offset 	200	/* 0x000000c8 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_755__dflip_bit_offset 	201	/* 0x000000c9 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_755__pflip_bit_offset 	202	/* 0x000000ca */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_755__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_755__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_755__D 	30
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_755__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_755__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_755__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_755__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_755__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_755__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_755__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_755__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_755__vmem_buffer_num_bytes 	19200
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_755__dram_size_num_bytes 	91200
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_755__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_755__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_755__expoffset 	13
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_755__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_755__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_755__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_755__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_755__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_755__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_755__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_6 HMB_input __pvcn_756_ */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_756__cnngen_demangled_name 	"__pvcn_756_"
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_756__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_756__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_756__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_756__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_756__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_756__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_756__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_756__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_756__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_756__byte_offset 	36	/* 0x00000024 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_756__vbase_byte_offset 	40	/* 0x00000028 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_756__dbase_byte_offset 	64	/* 0x00000040 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_756__dpitchm1_byte_offset 	68	/* 0x00000044 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_756__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_756__vwidth_minus_one_byte_offset 	52	/* 0x00000034 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_756__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_756__vheight_minus_one_byte_offset 	54	/* 0x00000036 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_756__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_756__drotate_bit_offset 	486	/* 0x000001e6 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_756__hflip_bit_offset 	487	/* 0x000001e7 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_756__vflip_bit_offset 	488	/* 0x000001e8 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_756__dflip_bit_offset 	489	/* 0x000001e9 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_756__pflip_bit_offset 	490	/* 0x000001ea */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_756__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_756__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_756__D 	31
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_756__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_756__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_756__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_756__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_756__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_756__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_756__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_756__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_756__vmem_buffer_num_bytes 	19840
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_756__dram_size_num_bytes 	94240
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_756__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_756__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_756__expoffset 	13
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_756__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_756__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_756__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_756__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_756__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_756__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_756__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_6 HMB_input __pvcn_719_ */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_719__cnngen_demangled_name 	"__pvcn_719_"
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_719__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_719__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_719__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_719__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_719__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_719__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_719__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_719__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_719__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_719__byte_offset 	72	/* 0x00000048 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_719__vbase_byte_offset 	76	/* 0x0000004c */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_719__dbase_byte_offset 	100	/* 0x00000064 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_719__dpitchm1_byte_offset 	104	/* 0x00000068 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_719__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_719__vwidth_minus_one_byte_offset 	88	/* 0x00000058 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_719__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_719__vheight_minus_one_byte_offset 	90	/* 0x0000005a */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_719__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_719__drotate_bit_offset 	774	/* 0x00000306 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_719__hflip_bit_offset 	775	/* 0x00000307 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_719__vflip_bit_offset 	776	/* 0x00000308 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_719__dflip_bit_offset 	777	/* 0x00000309 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_719__pflip_bit_offset 	778	/* 0x0000030a */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_719__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_719__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_719__D 	6
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_719__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_719__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_719__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_719__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_719__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_719__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_719__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_719__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_719__vmem_buffer_num_bytes 	9600
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_719__dram_size_num_bytes 	18240
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_719__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_719__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_719__expoffset 	13
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_719__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_719__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_719__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_719__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_719__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_719__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_719__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_6 HMB_input __pvcn_757_ */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_757__cnngen_demangled_name 	"__pvcn_757_"
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_757__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_757__is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_757__has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_757__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_757__ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_757__ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_757__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_757__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_757__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_757__byte_offset 	112	/* 0x00000070 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_757__vbase_byte_offset 	116	/* 0x00000074 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_757__dbase_byte_offset 	140	/* 0x0000008c */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_757__dpitchm1_byte_offset 	144	/* 0x00000090 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_757__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_757__vwidth_minus_one_byte_offset 	128	/* 0x00000080 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_757__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_757__vheight_minus_one_byte_offset 	130	/* 0x00000082 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_757__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_757__drotate_bit_offset 	1094	/* 0x00000446 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_757__hflip_bit_offset 	1095	/* 0x00000447 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_757__vflip_bit_offset 	1096	/* 0x00000448 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_757__dflip_bit_offset 	1097	/* 0x00000449 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_757__pflip_bit_offset 	1098	/* 0x0000044a */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_757__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_757__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_757__D 	61
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_757__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_757__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_757__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_757__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_757__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_757__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_757__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_757__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_757__vmem_buffer_num_bytes 	19520
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_757__dram_size_num_bytes 	185440
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_757__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_757__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_757__expoffset 	13
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_757__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_757__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_757__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_757__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_757__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_757__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_757__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_6 SMB_input __pvcn_762_ */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_762__cnngen_demangled_name 	"__pvcn_762_"
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_762__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_762__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_762__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_762__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_762__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_762__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_762__byte_offset 	164	/* 0x000000a4 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_762__vbase_byte_offset 	168	/* 0x000000a8 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_762__dbase_byte_offset 	192	/* 0x000000c0 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_762__dpitchm1_byte_offset 	196	/* 0x000000c4 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_762__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_762__W 	30444
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_762__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_762__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_762__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_762__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_762__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_762__dpitch_num_bytes 	30464
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_762__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_762__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_762__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_762__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_762__vmem_buffer_num_bytes 	30444
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_762__dram_size_num_bytes 	30464

/* mnet_ssd_adas_flex_pic_prim_split_6 SMB_input __pvcn_763_ */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_763__cnngen_demangled_name 	"__pvcn_763_"
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_763__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_763__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_763__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_763__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_763__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_763__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_763__byte_offset 	204	/* 0x000000cc */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_763__vbase_byte_offset 	208	/* 0x000000d0 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_763__dbase_byte_offset 	232	/* 0x000000e8 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_763__dpitchm1_byte_offset 	236	/* 0x000000ec */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_763__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_763__W 	1304
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_763__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_763__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_763__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_763__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_763__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_763__dpitch_num_bytes 	1344
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_763__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_763__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_763__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_763__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_763__vmem_buffer_num_bytes 	1304
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_763__dram_size_num_bytes 	1344

/* mnet_ssd_adas_flex_pic_prim_split_6 SMB_input __pvcn_787_ */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_787__cnngen_demangled_name 	"__pvcn_787_"
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_787__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_787__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_787__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_787__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_787__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_787__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_787__byte_offset 	244	/* 0x000000f4 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_787__vbase_byte_offset 	248	/* 0x000000f8 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_787__dbase_byte_offset 	272	/* 0x00000110 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_787__dpitchm1_byte_offset 	276	/* 0x00000114 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_787__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_787__W 	320
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_787__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_787__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_787__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_787__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_787__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_787__dpitch_num_bytes 	320
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_787__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_787__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_787__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_787__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_787__vmem_buffer_num_bytes 	320
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_787__dram_size_num_bytes 	320

/* mnet_ssd_adas_flex_pic_prim_split_6 SMB_input __pvcn_788_ */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_788__cnngen_demangled_name 	"__pvcn_788_"
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_788__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_788__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_788__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_788__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_788__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_788__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_788__byte_offset 	284	/* 0x0000011c */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_788__vbase_byte_offset 	288	/* 0x00000120 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_788__dbase_byte_offset 	312	/* 0x00000138 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_788__dpitchm1_byte_offset 	316	/* 0x0000013c */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_788__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_788__W 	208
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_788__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_788__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_788__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_788__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_788__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_788__dpitch_num_bytes 	256
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_788__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_788__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_788__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_788__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_788__vmem_buffer_num_bytes 	208
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_788__dram_size_num_bytes 	256

/* mnet_ssd_adas_flex_pic_prim_split_6 SMB_input __pvcn_798_ */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_798__cnngen_demangled_name 	"__pvcn_798_"
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_798__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_798__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_798__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_798__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_798__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_798__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_798__byte_offset 	324	/* 0x00000144 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_798__vbase_byte_offset 	328	/* 0x00000148 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_798__dbase_byte_offset 	352	/* 0x00000160 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_798__dpitchm1_byte_offset 	356	/* 0x00000164 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_798__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_798__W 	332
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_798__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_798__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_798__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_798__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_798__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_798__dpitch_num_bytes 	384
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_798__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_798__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_798__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_798__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_798__vmem_buffer_num_bytes 	332
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_798__dram_size_num_bytes 	384

/* mnet_ssd_adas_flex_pic_prim_split_6 SMB_input __pvcn_799_ */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_799__cnngen_demangled_name 	"__pvcn_799_"
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_799__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_799__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_799__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_799__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_799__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_799__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_799__byte_offset 	364	/* 0x0000016c */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_799__vbase_byte_offset 	368	/* 0x00000170 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_799__dbase_byte_offset 	392	/* 0x00000188 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_799__dpitchm1_byte_offset 	396	/* 0x0000018c */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_799__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_799__W 	216
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_799__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_799__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_799__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_799__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_799__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_799__dpitch_num_bytes 	256
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_799__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_799__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_799__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_799__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_799__vmem_buffer_num_bytes 	216
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_799__dram_size_num_bytes 	256

/* mnet_ssd_adas_flex_pic_prim_split_6 SMB_input __pvcn_809_ */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_809__cnngen_demangled_name 	"__pvcn_809_"
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_809__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_809__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_809__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_809__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_809__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_809__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_809__byte_offset 	404	/* 0x00000194 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_809__vbase_byte_offset 	408	/* 0x00000198 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_809__dbase_byte_offset 	432	/* 0x000001b0 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_809__dpitchm1_byte_offset 	436	/* 0x000001b4 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_809__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_809__W 	304
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_809__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_809__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_809__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_809__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_809__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_809__dpitch_num_bytes 	320
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_809__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_809__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_809__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_809__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_809__vmem_buffer_num_bytes 	304
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_809__dram_size_num_bytes 	320

/* mnet_ssd_adas_flex_pic_prim_split_6 SMB_input __pvcn_810_ */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_810__cnngen_demangled_name 	"__pvcn_810_"
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_810__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_810__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_810__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_810__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_810__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_810__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_810__byte_offset 	444	/* 0x000001bc */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_810__vbase_byte_offset 	448	/* 0x000001c0 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_810__dbase_byte_offset 	472	/* 0x000001d8 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_810__dpitchm1_byte_offset 	476	/* 0x000001dc */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_810__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_810__W 	216
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_810__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_810__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_810__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_810__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_810__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_810__dpitch_num_bytes 	256
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_810__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_810__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_810__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_810__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_810__vmem_buffer_num_bytes 	216
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_810__dram_size_num_bytes 	256

/* mnet_ssd_adas_flex_pic_prim_split_6 SMB_input __pvcn_820_ */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_820__cnngen_demangled_name 	"__pvcn_820_"
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_820__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_820__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_820__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_820__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_820__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_820__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_820__byte_offset 	484	/* 0x000001e4 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_820__vbase_byte_offset 	488	/* 0x000001e8 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_820__dbase_byte_offset 	512	/* 0x00000200 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_820__dpitchm1_byte_offset 	516	/* 0x00000204 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_820__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_820__W 	308
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_820__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_820__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_820__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_820__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_820__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_820__dpitch_num_bytes 	320
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_820__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_820__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_820__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_820__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_820__vmem_buffer_num_bytes 	308
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_820__dram_size_num_bytes 	320

/* mnet_ssd_adas_flex_pic_prim_split_6 SMB_input __pvcn_821_ */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_821__cnngen_demangled_name 	"__pvcn_821_"
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_821__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_821__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_821__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_821__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_821__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_821__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_821__byte_offset 	524	/* 0x0000020c */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_821__vbase_byte_offset 	528	/* 0x00000210 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_821__dbase_byte_offset 	552	/* 0x00000228 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_821__dpitchm1_byte_offset 	556	/* 0x0000022c */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_821__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_821__W 	212
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_821__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_821__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_821__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_821__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_821__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_821__dpitch_num_bytes 	256
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_821__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_821__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_821__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_821__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_821__vmem_buffer_num_bytes 	212
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_821__dram_size_num_bytes 	256

/* mnet_ssd_adas_flex_pic_prim_split_6 SMB_input __pvcn_831_ */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_831__cnngen_demangled_name 	"__pvcn_831_"
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_831__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_831__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_831__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_831__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_831__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_831__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_831__byte_offset 	564	/* 0x00000234 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_831__vbase_byte_offset 	568	/* 0x00000238 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_831__dbase_byte_offset 	592	/* 0x00000250 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_831__dpitchm1_byte_offset 	596	/* 0x00000254 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_831__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_831__W 	308
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_831__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_831__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_831__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_831__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_831__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_831__dpitch_num_bytes 	320
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_831__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_831__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_831__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_831__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_831__vmem_buffer_num_bytes 	308
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_831__dram_size_num_bytes 	320

/* mnet_ssd_adas_flex_pic_prim_split_6 SMB_input __pvcn_832_ */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_832__cnngen_demangled_name 	"__pvcn_832_"
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_832__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_832__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_832__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_832__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_832__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_832__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_832__byte_offset 	604	/* 0x0000025c */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_832__vbase_byte_offset 	608	/* 0x00000260 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_832__dbase_byte_offset 	632	/* 0x00000278 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_832__dpitchm1_byte_offset 	636	/* 0x0000027c */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_832__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_832__W 	204
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_832__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_832__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_832__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_832__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_832__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_832__dpitch_num_bytes 	256
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_832__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_832__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_832__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_832__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_832__vmem_buffer_num_bytes 	204
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_832__dram_size_num_bytes 	256

/* mnet_ssd_adas_flex_pic_prim_split_6 SMB_input __pvcn_842_ */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_842__cnngen_demangled_name 	"__pvcn_842_"
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_842__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_842__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_842__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_842__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_842__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_842__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_842__byte_offset 	644	/* 0x00000284 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_842__vbase_byte_offset 	648	/* 0x00000288 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_842__dbase_byte_offset 	672	/* 0x000002a0 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_842__dpitchm1_byte_offset 	676	/* 0x000002a4 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_842__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_842__W 	304
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_842__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_842__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_842__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_842__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_842__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_842__dpitch_num_bytes 	320
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_842__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_842__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_842__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_842__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_842__vmem_buffer_num_bytes 	304
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_842__dram_size_num_bytes 	320

/* mnet_ssd_adas_flex_pic_prim_split_6 SMB_input __pvcn_843_ */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_843__cnngen_demangled_name 	"__pvcn_843_"
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_843__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_843__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_843__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_843__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_843__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_843__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_843__byte_offset 	684	/* 0x000002ac */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_843__vbase_byte_offset 	688	/* 0x000002b0 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_843__dbase_byte_offset 	712	/* 0x000002c8 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_843__dpitchm1_byte_offset 	716	/* 0x000002cc */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_843__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_843__W 	204
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_843__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_843__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_843__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_843__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_843__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_843__dpitch_num_bytes 	256
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_843__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_843__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_843__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_843__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_843__vmem_buffer_num_bytes 	204
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_843__dram_size_num_bytes 	256

/* mnet_ssd_adas_flex_pic_prim_split_6 SMB_input __pvcn_853_ */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_853__cnngen_demangled_name 	"__pvcn_853_"
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_853__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_853__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_853__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_853__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_853__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_853__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_853__byte_offset 	724	/* 0x000002d4 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_853__vbase_byte_offset 	728	/* 0x000002d8 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_853__dbase_byte_offset 	752	/* 0x000002f0 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_853__dpitchm1_byte_offset 	756	/* 0x000002f4 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_853__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_853__W 	320
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_853__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_853__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_853__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_853__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_853__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_853__dpitch_num_bytes 	320
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_853__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_853__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_853__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_853__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_853__vmem_buffer_num_bytes 	320
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_853__dram_size_num_bytes 	320

/* mnet_ssd_adas_flex_pic_prim_split_6 SMB_input __pvcn_854_ */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_854__cnngen_demangled_name 	"__pvcn_854_"
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_854__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_854__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_854__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_854__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_854__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_854__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_854__byte_offset 	764	/* 0x000002fc */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_854__vbase_byte_offset 	768	/* 0x00000300 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_854__dbase_byte_offset 	792	/* 0x00000318 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_854__dpitchm1_byte_offset 	796	/* 0x0000031c */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_854__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_854__W 	208
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_854__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_854__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_854__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_854__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_854__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_854__dpitch_num_bytes 	256
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_854__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_854__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_854__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_854__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_854__vmem_buffer_num_bytes 	208
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_854__dram_size_num_bytes 	256

/* mnet_ssd_adas_flex_pic_prim_split_6 SMB_input __pvcn_864_ */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_864__cnngen_demangled_name 	"__pvcn_864_"
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_864__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_864__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_864__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_864__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_864__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_864__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_864__byte_offset 	804	/* 0x00000324 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_864__vbase_byte_offset 	808	/* 0x00000328 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_864__dbase_byte_offset 	832	/* 0x00000340 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_864__dpitchm1_byte_offset 	836	/* 0x00000344 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_864__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_864__W 	320
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_864__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_864__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_864__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_864__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_864__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_864__dpitch_num_bytes 	320
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_864__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_864__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_864__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_864__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_864__vmem_buffer_num_bytes 	320
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_864__dram_size_num_bytes 	320

/* mnet_ssd_adas_flex_pic_prim_split_6 SMB_input __pvcn_865_ */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_865__cnngen_demangled_name 	"__pvcn_865_"
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_865__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_865__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_865__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_865__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_865__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_865__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_865__byte_offset 	844	/* 0x0000034c */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_865__vbase_byte_offset 	848	/* 0x00000350 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_865__dbase_byte_offset 	872	/* 0x00000368 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_865__dpitchm1_byte_offset 	876	/* 0x0000036c */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_865__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_865__W 	216
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_865__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_865__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_865__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_865__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_865__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_865__dpitch_num_bytes 	256
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_865__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_865__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_865__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_865__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_865__vmem_buffer_num_bytes 	216
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_865__dram_size_num_bytes 	256

/* mnet_ssd_adas_flex_pic_prim_split_6 SMB_input __pvcn_875_ */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_875__cnngen_demangled_name 	"__pvcn_875_"
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_875__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_875__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_875__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_875__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_875__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_875__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_875__byte_offset 	884	/* 0x00000374 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_875__vbase_byte_offset 	888	/* 0x00000378 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_875__dbase_byte_offset 	912	/* 0x00000390 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_875__dpitchm1_byte_offset 	916	/* 0x00000394 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_875__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_875__W 	308
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_875__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_875__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_875__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_875__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_875__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_875__dpitch_num_bytes 	320
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_875__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_875__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_875__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_875__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_875__vmem_buffer_num_bytes 	308
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_875__dram_size_num_bytes 	320

/* mnet_ssd_adas_flex_pic_prim_split_6 SMB_input __pvcn_876_ */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_876__cnngen_demangled_name 	"__pvcn_876_"
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_876__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_876__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_876__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_876__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_876__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_876__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_876__byte_offset 	924	/* 0x0000039c */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_876__vbase_byte_offset 	928	/* 0x000003a0 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_876__dbase_byte_offset 	952	/* 0x000003b8 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_876__dpitchm1_byte_offset 	956	/* 0x000003bc */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_876__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_876__W 	204
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_876__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_876__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_876__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_876__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_876__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_876__dpitch_num_bytes 	256
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_876__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_876__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_876__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_876__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_876__vmem_buffer_num_bytes 	204
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_876__dram_size_num_bytes 	256

/* mnet_ssd_adas_flex_pic_prim_split_6 SMB_input __pvcn_886_ */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_886__cnngen_demangled_name 	"__pvcn_886_"
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_886__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_886__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_886__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_886__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_886__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_886__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_886__byte_offset 	964	/* 0x000003c4 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_886__vbase_byte_offset 	968	/* 0x000003c8 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_886__dbase_byte_offset 	992	/* 0x000003e0 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_886__dpitchm1_byte_offset 	996	/* 0x000003e4 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_886__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_886__W 	296
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_886__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_886__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_886__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_886__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_886__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_886__dpitch_num_bytes 	320
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_886__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_886__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_886__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_886__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_886__vmem_buffer_num_bytes 	296
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_886__dram_size_num_bytes 	320

/* mnet_ssd_adas_flex_pic_prim_split_6 SMB_input __pvcn_887_ */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_887__cnngen_demangled_name 	"__pvcn_887_"
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_887__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_887__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_887__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_887__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_887__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_887__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_887__byte_offset 	1004	/* 0x000003ec */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_887__vbase_byte_offset 	1008	/* 0x000003f0 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_887__dbase_byte_offset 	1032	/* 0x00000408 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_887__dpitchm1_byte_offset 	1036	/* 0x0000040c */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_887__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_887__W 	196
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_887__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_887__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_887__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_887__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_887__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_887__dpitch_num_bytes 	256
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_887__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_887__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_887__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_887__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_887__vmem_buffer_num_bytes 	196
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_887__dram_size_num_bytes 	256

/* mnet_ssd_adas_flex_pic_prim_split_6 SMB_input __pvcn_897_ */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_897__cnngen_demangled_name 	"__pvcn_897_"
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_897__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_897__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_897__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_897__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_897__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_897__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_897__byte_offset 	1044	/* 0x00000414 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_897__vbase_byte_offset 	1048	/* 0x00000418 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_897__dbase_byte_offset 	1072	/* 0x00000430 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_897__dpitchm1_byte_offset 	1076	/* 0x00000434 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_897__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_897__W 	288
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_897__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_897__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_897__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_897__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_897__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_897__dpitch_num_bytes 	320
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_897__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_897__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_897__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_897__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_897__vmem_buffer_num_bytes 	288
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_897__dram_size_num_bytes 	320

/* mnet_ssd_adas_flex_pic_prim_split_6 SMB_input __pvcn_898_ */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_898__cnngen_demangled_name 	"__pvcn_898_"
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_898__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_898__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_898__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_898__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_898__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_898__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_898__byte_offset 	1084	/* 0x0000043c */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_898__vbase_byte_offset 	1088	/* 0x00000440 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_898__dbase_byte_offset 	1112	/* 0x00000458 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_898__dpitchm1_byte_offset 	1116	/* 0x0000045c */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_898__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_898__W 	196
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_898__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_898__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_898__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_898__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_898__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_898__dpitch_num_bytes 	256
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_898__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_898__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_898__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_898__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_898__vmem_buffer_num_bytes 	196
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_898__dram_size_num_bytes 	256

/* mnet_ssd_adas_flex_pic_prim_split_6 SMB_input __pvcn_908_ */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_908__cnngen_demangled_name 	"__pvcn_908_"
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_908__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_908__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_908__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_908__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_908__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_908__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_908__byte_offset 	1124	/* 0x00000464 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_908__vbase_byte_offset 	1128	/* 0x00000468 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_908__dbase_byte_offset 	1152	/* 0x00000480 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_908__dpitchm1_byte_offset 	1156	/* 0x00000484 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_908__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_908__W 	184
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_908__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_908__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_908__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_908__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_908__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_908__dpitch_num_bytes 	192
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_908__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_908__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_908__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_908__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_908__vmem_buffer_num_bytes 	184
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_908__dram_size_num_bytes 	192

/* mnet_ssd_adas_flex_pic_prim_split_6 SMB_input __pvcn_909_ */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_909__cnngen_demangled_name 	"__pvcn_909_"
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_909__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_909__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_909__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_909__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_909__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_909__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_909__byte_offset 	1164	/* 0x0000048c */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_909__vbase_byte_offset 	1168	/* 0x00000490 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_909__dbase_byte_offset 	1192	/* 0x000004a8 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_909__dpitchm1_byte_offset 	1196	/* 0x000004ac */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_909__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_909__W 	128
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_909__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_909__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_909__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_909__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_909__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_909__dpitch_num_bytes 	128
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_909__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_909__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_909__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_909__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_909__vmem_buffer_num_bytes 	128
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_909__dram_size_num_bytes 	128

/* mnet_ssd_adas_flex_pic_prim_split_6 VCB conv3_2__dw_____bn_conv3_2__dw__scale_mul___muli___29_____bn_conv3_2__dw__scale_mul_____scale___muli___341 */
#define mnet_ssd_adas_flex_pic_prim_split_6_conv3_2__dw_____bn_conv3_2__dw__scale_mul___muli___29_____bn_conv3_2__dw__scale_mul_____scale___muli___341_cnngen_demangled_name 	"conv3_2__dw_____bn_conv3_2__dw__scale_mul___muli___29_____bn_conv3_2__dw__scale_mul_____scale___muli___341"
#define mnet_ssd_adas_flex_pic_prim_split_6_conv3_2__dw_____bn_conv3_2__dw__scale_mul___muli___29_____bn_conv3_2__dw__scale_mul_____scale___muli___341_is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_6_conv3_2__dw_____bn_conv3_2__dw__scale_mul___muli___29_____bn_conv3_2__dw__scale_mul_____scale___muli___341_is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_6_conv3_2__dw_____bn_conv3_2__dw__scale_mul___muli___29_____bn_conv3_2__dw__scale_mul_____scale___muli___341_is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_6_conv3_2__dw_____bn_conv3_2__dw__scale_mul___muli___29_____bn_conv3_2__dw__scale_mul_____scale___muli___341_byte_offset 	1316	/* 0x00000524 */
#define mnet_ssd_adas_flex_pic_prim_split_6_conv3_2__dw_____bn_conv3_2__dw__scale_mul___muli___29_____bn_conv3_2__dw__scale_mul_____scale___muli___341_vbase_byte_offset 	1324	/* 0x0000052c */
#define mnet_ssd_adas_flex_pic_prim_split_6_conv3_2__dw_____bn_conv3_2__dw__scale_mul___muli___29_____bn_conv3_2__dw__scale_mul_____scale___muli___341_vmem_buffer_num_bytes 	30720

/* mnet_ssd_adas_flex_pic_prim_split_6 VCB conv3_2__sep_____bn_conv3_2__sep__scale_mul___muli___32_____bn_conv3_2__sep__scale_mul_____scale___relu3_2__sep */
#define mnet_ssd_adas_flex_pic_prim_split_6_conv3_2__sep_____bn_conv3_2__sep__scale_mul___muli___32_____bn_conv3_2__sep__scale_mul_____scale___relu3_2__sep_cnngen_demangled_name 	"conv3_2__sep_____bn_conv3_2__sep__scale_mul___muli___32_____bn_conv3_2__sep__scale_mul_____scale___relu3_2__sep"
#define mnet_ssd_adas_flex_pic_prim_split_6_conv3_2__sep_____bn_conv3_2__sep__scale_mul___muli___32_____bn_conv3_2__sep__scale_mul_____scale___relu3_2__sep_is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_6_conv3_2__sep_____bn_conv3_2__sep__scale_mul___muli___32_____bn_conv3_2__sep__scale_mul_____scale___relu3_2__sep_is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_6_conv3_2__sep_____bn_conv3_2__sep__scale_mul___muli___32_____bn_conv3_2__sep__scale_mul_____scale___relu3_2__sep_is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_6_conv3_2__sep_____bn_conv3_2__sep__scale_mul___muli___32_____bn_conv3_2__sep__scale_mul_____scale___relu3_2__sep_byte_offset 	1364	/* 0x00000554 */
#define mnet_ssd_adas_flex_pic_prim_split_6_conv3_2__sep_____bn_conv3_2__sep__scale_mul___muli___32_____bn_conv3_2__sep__scale_mul_____scale___relu3_2__sep_vbase_byte_offset 	1372	/* 0x0000055c */
#define mnet_ssd_adas_flex_pic_prim_split_6_conv3_2__sep_____bn_conv3_2__sep__scale_mul___muli___32_____bn_conv3_2__sep__scale_mul_____scale___relu3_2__sep_vmem_buffer_num_bytes 	286720

/* mnet_ssd_adas_flex_pic_prim_split_6 HMB_output __pvcn_789_ */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_789__cnngen_demangled_name 	"__pvcn_789_"
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_789__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_789__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_789__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_789__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_789__byte_offset 	1432	/* 0x00000598 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_789__vbase_byte_offset 	1436	/* 0x0000059c */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_789__dbase_byte_offset 	1460	/* 0x000005b4 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_789__dpitchm1_byte_offset 	1464	/* 0x000005b8 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_789__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_789__vwidth_minus_one_byte_offset 	1448	/* 0x000005a8 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_789__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_789__vheight_minus_one_byte_offset 	1450	/* 0x000005aa */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_789__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_789__drotate_bit_offset 	11654	/* 0x00002d86 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_789__hflip_bit_offset 	11655	/* 0x00002d87 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_789__vflip_bit_offset 	11656	/* 0x00002d88 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_789__dflip_bit_offset 	11657	/* 0x00002d89 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_789__pflip_bit_offset 	11658	/* 0x00002d8a */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_789__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_789__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_789__D 	22
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_789__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_789__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_789__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_789__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_789__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_789__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_789__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_789__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_789__vmem_buffer_num_bytes 	7040
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_789__dram_size_num_bytes 	66880
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_789__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_789__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_789__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_789__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_789__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_789__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_789__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_789__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_789__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_789__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_6 HMB_output __pvcn_800_ */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_800__cnngen_demangled_name 	"__pvcn_800_"
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_800__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_800__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_800__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_800__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_800__byte_offset 	1488	/* 0x000005d0 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_800__vbase_byte_offset 	1492	/* 0x000005d4 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_800__dbase_byte_offset 	1516	/* 0x000005ec */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_800__dpitchm1_byte_offset 	1520	/* 0x000005f0 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_800__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_800__vwidth_minus_one_byte_offset 	1504	/* 0x000005e0 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_800__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_800__vheight_minus_one_byte_offset 	1506	/* 0x000005e2 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_800__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_800__drotate_bit_offset 	12102	/* 0x00002f46 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_800__hflip_bit_offset 	12103	/* 0x00002f47 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_800__vflip_bit_offset 	12104	/* 0x00002f48 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_800__dflip_bit_offset 	12105	/* 0x00002f49 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_800__pflip_bit_offset 	12106	/* 0x00002f4a */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_800__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_800__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_800__D 	24
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_800__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_800__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_800__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_800__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_800__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_800__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_800__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_800__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_800__vmem_buffer_num_bytes 	7680
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_800__dram_size_num_bytes 	72960
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_800__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_800__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_800__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_800__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_800__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_800__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_800__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_800__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_800__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_800__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_6 HMB_output __pvcn_811_ */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_811__cnngen_demangled_name 	"__pvcn_811_"
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_811__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_811__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_811__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_811__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_811__byte_offset 	1544	/* 0x00000608 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_811__vbase_byte_offset 	1548	/* 0x0000060c */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_811__dbase_byte_offset 	1572	/* 0x00000624 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_811__dpitchm1_byte_offset 	1576	/* 0x00000628 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_811__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_811__vwidth_minus_one_byte_offset 	1560	/* 0x00000618 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_811__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_811__vheight_minus_one_byte_offset 	1562	/* 0x0000061a */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_811__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_811__drotate_bit_offset 	12550	/* 0x00003106 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_811__hflip_bit_offset 	12551	/* 0x00003107 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_811__vflip_bit_offset 	12552	/* 0x00003108 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_811__dflip_bit_offset 	12553	/* 0x00003109 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_811__pflip_bit_offset 	12554	/* 0x0000310a */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_811__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_811__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_811__D 	24
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_811__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_811__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_811__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_811__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_811__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_811__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_811__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_811__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_811__vmem_buffer_num_bytes 	7680
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_811__dram_size_num_bytes 	72960
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_811__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_811__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_811__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_811__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_811__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_811__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_811__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_811__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_811__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_811__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_6 HMB_output __pvcn_822_ */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_822__cnngen_demangled_name 	"__pvcn_822_"
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_822__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_822__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_822__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_822__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_822__byte_offset 	1600	/* 0x00000640 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_822__vbase_byte_offset 	1604	/* 0x00000644 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_822__dbase_byte_offset 	1628	/* 0x0000065c */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_822__dpitchm1_byte_offset 	1632	/* 0x00000660 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_822__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_822__vwidth_minus_one_byte_offset 	1616	/* 0x00000650 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_822__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_822__vheight_minus_one_byte_offset 	1618	/* 0x00000652 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_822__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_822__drotate_bit_offset 	12998	/* 0x000032c6 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_822__hflip_bit_offset 	12999	/* 0x000032c7 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_822__vflip_bit_offset 	13000	/* 0x000032c8 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_822__dflip_bit_offset 	13001	/* 0x000032c9 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_822__pflip_bit_offset 	13002	/* 0x000032ca */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_822__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_822__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_822__D 	23
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_822__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_822__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_822__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_822__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_822__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_822__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_822__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_822__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_822__vmem_buffer_num_bytes 	7360
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_822__dram_size_num_bytes 	69920
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_822__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_822__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_822__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_822__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_822__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_822__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_822__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_822__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_822__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_822__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_6 HMB_output __pvcn_833_ */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_833__cnngen_demangled_name 	"__pvcn_833_"
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_833__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_833__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_833__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_833__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_833__byte_offset 	1656	/* 0x00000678 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_833__vbase_byte_offset 	1660	/* 0x0000067c */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_833__dbase_byte_offset 	1684	/* 0x00000694 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_833__dpitchm1_byte_offset 	1688	/* 0x00000698 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_833__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_833__vwidth_minus_one_byte_offset 	1672	/* 0x00000688 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_833__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_833__vheight_minus_one_byte_offset 	1674	/* 0x0000068a */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_833__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_833__drotate_bit_offset 	13446	/* 0x00003486 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_833__hflip_bit_offset 	13447	/* 0x00003487 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_833__vflip_bit_offset 	13448	/* 0x00003488 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_833__dflip_bit_offset 	13449	/* 0x00003489 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_833__pflip_bit_offset 	13450	/* 0x0000348a */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_833__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_833__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_833__D 	21
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_833__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_833__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_833__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_833__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_833__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_833__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_833__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_833__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_833__vmem_buffer_num_bytes 	6720
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_833__dram_size_num_bytes 	63840
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_833__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_833__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_833__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_833__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_833__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_833__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_833__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_833__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_833__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_833__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_6 HMB_output __pvcn_844_ */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_844__cnngen_demangled_name 	"__pvcn_844_"
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_844__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_844__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_844__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_844__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_844__byte_offset 	1712	/* 0x000006b0 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_844__vbase_byte_offset 	1716	/* 0x000006b4 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_844__dbase_byte_offset 	1740	/* 0x000006cc */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_844__dpitchm1_byte_offset 	1744	/* 0x000006d0 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_844__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_844__vwidth_minus_one_byte_offset 	1728	/* 0x000006c0 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_844__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_844__vheight_minus_one_byte_offset 	1730	/* 0x000006c2 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_844__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_844__drotate_bit_offset 	13894	/* 0x00003646 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_844__hflip_bit_offset 	13895	/* 0x00003647 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_844__vflip_bit_offset 	13896	/* 0x00003648 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_844__dflip_bit_offset 	13897	/* 0x00003649 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_844__pflip_bit_offset 	13898	/* 0x0000364a */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_844__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_844__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_844__D 	21
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_844__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_844__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_844__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_844__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_844__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_844__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_844__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_844__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_844__vmem_buffer_num_bytes 	6720
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_844__dram_size_num_bytes 	63840
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_844__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_844__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_844__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_844__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_844__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_844__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_844__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_844__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_844__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_844__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_6 HMB_output __pvcn_855_ */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_855__cnngen_demangled_name 	"__pvcn_855_"
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_855__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_855__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_855__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_855__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_855__byte_offset 	1768	/* 0x000006e8 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_855__vbase_byte_offset 	1772	/* 0x000006ec */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_855__dbase_byte_offset 	1796	/* 0x00000704 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_855__dpitchm1_byte_offset 	1800	/* 0x00000708 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_855__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_855__vwidth_minus_one_byte_offset 	1784	/* 0x000006f8 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_855__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_855__vheight_minus_one_byte_offset 	1786	/* 0x000006fa */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_855__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_855__drotate_bit_offset 	14342	/* 0x00003806 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_855__hflip_bit_offset 	14343	/* 0x00003807 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_855__vflip_bit_offset 	14344	/* 0x00003808 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_855__dflip_bit_offset 	14345	/* 0x00003809 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_855__pflip_bit_offset 	14346	/* 0x0000380a */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_855__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_855__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_855__D 	22
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_855__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_855__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_855__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_855__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_855__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_855__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_855__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_855__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_855__vmem_buffer_num_bytes 	7040
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_855__dram_size_num_bytes 	66880
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_855__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_855__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_855__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_855__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_855__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_855__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_855__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_855__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_855__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_855__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_6 HMB_output __pvcn_866_ */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_866__cnngen_demangled_name 	"__pvcn_866_"
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_866__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_866__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_866__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_866__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_866__byte_offset 	1824	/* 0x00000720 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_866__vbase_byte_offset 	1828	/* 0x00000724 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_866__dbase_byte_offset 	1852	/* 0x0000073c */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_866__dpitchm1_byte_offset 	1856	/* 0x00000740 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_866__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_866__vwidth_minus_one_byte_offset 	1840	/* 0x00000730 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_866__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_866__vheight_minus_one_byte_offset 	1842	/* 0x00000732 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_866__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_866__drotate_bit_offset 	14790	/* 0x000039c6 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_866__hflip_bit_offset 	14791	/* 0x000039c7 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_866__vflip_bit_offset 	14792	/* 0x000039c8 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_866__dflip_bit_offset 	14793	/* 0x000039c9 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_866__pflip_bit_offset 	14794	/* 0x000039ca */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_866__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_866__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_866__D 	24
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_866__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_866__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_866__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_866__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_866__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_866__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_866__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_866__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_866__vmem_buffer_num_bytes 	7680
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_866__dram_size_num_bytes 	72960
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_866__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_866__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_866__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_866__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_866__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_866__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_866__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_866__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_866__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_866__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_6 HMB_output __pvcn_877_ */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_877__cnngen_demangled_name 	"__pvcn_877_"
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_877__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_877__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_877__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_877__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_877__byte_offset 	1880	/* 0x00000758 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_877__vbase_byte_offset 	1884	/* 0x0000075c */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_877__dbase_byte_offset 	1908	/* 0x00000774 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_877__dpitchm1_byte_offset 	1912	/* 0x00000778 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_877__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_877__vwidth_minus_one_byte_offset 	1896	/* 0x00000768 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_877__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_877__vheight_minus_one_byte_offset 	1898	/* 0x0000076a */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_877__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_877__drotate_bit_offset 	15238	/* 0x00003b86 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_877__hflip_bit_offset 	15239	/* 0x00003b87 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_877__vflip_bit_offset 	15240	/* 0x00003b88 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_877__dflip_bit_offset 	15241	/* 0x00003b89 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_877__pflip_bit_offset 	15242	/* 0x00003b8a */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_877__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_877__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_877__D 	22
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_877__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_877__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_877__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_877__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_877__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_877__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_877__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_877__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_877__vmem_buffer_num_bytes 	7040
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_877__dram_size_num_bytes 	66880
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_877__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_877__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_877__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_877__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_877__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_877__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_877__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_877__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_877__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_877__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_6 HMB_output __pvcn_888_ */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_888__cnngen_demangled_name 	"__pvcn_888_"
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_888__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_888__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_888__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_888__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_888__byte_offset 	1936	/* 0x00000790 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_888__vbase_byte_offset 	1940	/* 0x00000794 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_888__dbase_byte_offset 	1964	/* 0x000007ac */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_888__dpitchm1_byte_offset 	1968	/* 0x000007b0 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_888__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_888__vwidth_minus_one_byte_offset 	1952	/* 0x000007a0 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_888__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_888__vheight_minus_one_byte_offset 	1954	/* 0x000007a2 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_888__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_888__drotate_bit_offset 	15686	/* 0x00003d46 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_888__hflip_bit_offset 	15687	/* 0x00003d47 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_888__vflip_bit_offset 	15688	/* 0x00003d48 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_888__dflip_bit_offset 	15689	/* 0x00003d49 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_888__pflip_bit_offset 	15690	/* 0x00003d4a */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_888__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_888__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_888__D 	20
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_888__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_888__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_888__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_888__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_888__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_888__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_888__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_888__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_888__vmem_buffer_num_bytes 	6400
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_888__dram_size_num_bytes 	60800
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_888__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_888__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_888__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_888__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_888__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_888__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_888__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_888__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_888__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_888__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_6 HMB_output __pvcn_899_ */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_899__cnngen_demangled_name 	"__pvcn_899_"
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_899__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_899__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_899__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_899__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_899__byte_offset 	1992	/* 0x000007c8 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_899__vbase_byte_offset 	1996	/* 0x000007cc */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_899__dbase_byte_offset 	2020	/* 0x000007e4 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_899__dpitchm1_byte_offset 	2024	/* 0x000007e8 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_899__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_899__vwidth_minus_one_byte_offset 	2008	/* 0x000007d8 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_899__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_899__vheight_minus_one_byte_offset 	2010	/* 0x000007da */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_899__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_899__drotate_bit_offset 	16134	/* 0x00003f06 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_899__hflip_bit_offset 	16135	/* 0x00003f07 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_899__vflip_bit_offset 	16136	/* 0x00003f08 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_899__dflip_bit_offset 	16137	/* 0x00003f09 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_899__pflip_bit_offset 	16138	/* 0x00003f0a */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_899__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_899__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_899__D 	20
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_899__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_899__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_899__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_899__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_899__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_899__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_899__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_899__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_899__vmem_buffer_num_bytes 	6400
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_899__dram_size_num_bytes 	60800
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_899__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_899__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_899__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_899__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_899__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_899__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_899__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_899__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_899__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_899__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_6 HMB_output __pvcn_910_ */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_910__cnngen_demangled_name 	"__pvcn_910_"
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_910__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_910__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_910__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_910__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_910__byte_offset 	2048	/* 0x00000800 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_910__vbase_byte_offset 	2052	/* 0x00000804 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_910__dbase_byte_offset 	2076	/* 0x0000081c */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_910__dpitchm1_byte_offset 	2080	/* 0x00000820 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_910__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_910__vwidth_minus_one_byte_offset 	2064	/* 0x00000810 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_910__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_910__vheight_minus_one_byte_offset 	2066	/* 0x00000812 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_910__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_910__drotate_bit_offset 	16582	/* 0x000040c6 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_910__hflip_bit_offset 	16583	/* 0x000040c7 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_910__vflip_bit_offset 	16584	/* 0x000040c8 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_910__dflip_bit_offset 	16585	/* 0x000040c9 */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_910__pflip_bit_offset 	16586	/* 0x000040ca */
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_910__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_910__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_910__D 	13
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_910__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_910__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_910__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_910__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_910__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_910__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_910__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_910__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_910__vmem_buffer_num_bytes 	8320
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_910__dram_size_num_bytes 	39520
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_910__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_910__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_910__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_910__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_910__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_910__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_910__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_910__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_910__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___pvcn_910__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_6 SMB_output __vas_created_terminate__v0 */
#define mnet_ssd_adas_flex_pic_prim_split_6___vas_created_terminate__v0_cnngen_demangled_name 	"__vas_created_terminate__v0"
#define mnet_ssd_adas_flex_pic_prim_split_6___vas_created_terminate__v0_is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___vas_created_terminate__v0_is_bitvector 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___vas_created_terminate__v0_is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___vas_created_terminate__v0_is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___vas_created_terminate__v0_byte_offset 	2088	/* 0x00000828 */
#define mnet_ssd_adas_flex_pic_prim_split_6___vas_created_terminate__v0_vbase_byte_offset 	2092	/* 0x0000082c */
#define mnet_ssd_adas_flex_pic_prim_split_6___vas_created_terminate__v0_W 	256
#define mnet_ssd_adas_flex_pic_prim_split_6___vas_created_terminate__v0_H 	3
#define mnet_ssd_adas_flex_pic_prim_split_6___vas_created_terminate__v0_D 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___vas_created_terminate__v0_P 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___vas_created_terminate__v0_data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___vas_created_terminate__v0_denable 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___vas_created_terminate__v0_dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_6___vas_created_terminate__v0_vp_interleave_mode 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___vas_created_terminate__v0_tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___vas_created_terminate__v0_tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_6___vas_created_terminate__v0_vmem_buffer_num_bytes 	96


#endif /* mnet_ssd_adas_flex_pic_prim_split_6_H */
