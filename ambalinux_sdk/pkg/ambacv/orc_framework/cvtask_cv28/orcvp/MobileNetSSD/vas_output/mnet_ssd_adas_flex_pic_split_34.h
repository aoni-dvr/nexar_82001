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
#ifndef mnet_ssd_adas_flex_pic_prim_split_34_H
#define mnet_ssd_adas_flex_pic_prim_split_34_H

#define vas_gen_header_version                  	7

/* dag name */
#define mnet_ssd_adas_flex_pic_prim_split_34_DAG_NAME 	"mnet_ssd_adas_flex_pic_prim_split_34"

/* VDG file info */
#define mnet_ssd_adas_flex_pic_prim_split_34_vdg_name 	"mnet_ssd_adas_flex_pic_split_34.vdg"

/* VDG DAG memory info */
#define mnet_ssd_adas_flex_pic_prim_split_34_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_34_byte_size 	252	/* 0x000000fc */
#define mnet_ssd_adas_flex_pic_prim_split_34_preferred_dram_xfer_size 	1

/* mnet_ssd_adas_flex_pic_prim_split_34 VMEM info */
#define mnet_ssd_adas_flex_pic_prim_split_34_use_ping_pong_vmem 	0
#define mnet_ssd_adas_flex_pic_prim_split_34_VMEM_start 	4096	/* 0x00001000 */
#define mnet_ssd_adas_flex_pic_prim_split_34_VMEM_end 	322848	/* 0x0004ed20 */
#define mnet_ssd_adas_flex_pic_prim_split_34_image_start 	27232	/* 0x00006a60 */
#define mnet_ssd_adas_flex_pic_prim_split_34_image_size 	295868	/* 0x000483bc */
#define mnet_ssd_adas_flex_pic_prim_split_34_dagbin_start 	322848	/* 0x0004ed20 */

/* mnet_ssd_adas_flex_pic_prim_split_34 DAG info */
#define mnet_ssd_adas_flex_pic_prim_split_34_estimated_cycles 	18923
#define mnet_ssd_adas_flex_pic_prim_split_34_ddi_byte_size 	0	/* 0x00000000 */

/*******************************************************************************
 * DAG byte offset info for HMB/SMB/VCB and their fields
 * in DAG 'mnet_ssd_adas_flex_pic_prim_split_34' in source file 'pre_split29_mnet_ssd_adas_flex_pic_split_2.vas'
 ******************************************************************************/
/* mnet_ssd_adas_flex_pic_prim_split_34 HMB_input relu7_1 */
#define mnet_ssd_adas_flex_pic_prim_split_34_relu7_1_cnngen_demangled_name 	"relu7_1"
#define mnet_ssd_adas_flex_pic_prim_split_34_relu7_1_is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_34_relu7_1_is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_34_relu7_1_has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_34_relu7_1_is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_34_relu7_1_ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_34_relu7_1_ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_34_relu7_1_is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_34_relu7_1_is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_34_relu7_1_is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_34_relu7_1_byte_offset 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_34_relu7_1_vbase_byte_offset 	4	/* 0x00000004 */
#define mnet_ssd_adas_flex_pic_prim_split_34_relu7_1_dbase_byte_offset 	28	/* 0x0000001c */
#define mnet_ssd_adas_flex_pic_prim_split_34_relu7_1_dpitchm1_byte_offset 	32	/* 0x00000020 */
#define mnet_ssd_adas_flex_pic_prim_split_34_relu7_1_dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_34_relu7_1_vwidth_minus_one_byte_offset 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_34_relu7_1_vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_34_relu7_1_vheight_minus_one_byte_offset 	18	/* 0x00000012 */
#define mnet_ssd_adas_flex_pic_prim_split_34_relu7_1_vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_34_relu7_1_drotate_bit_offset 	198	/* 0x000000c6 */
#define mnet_ssd_adas_flex_pic_prim_split_34_relu7_1_hflip_bit_offset 	199	/* 0x000000c7 */
#define mnet_ssd_adas_flex_pic_prim_split_34_relu7_1_vflip_bit_offset 	200	/* 0x000000c8 */
#define mnet_ssd_adas_flex_pic_prim_split_34_relu7_1_dflip_bit_offset 	201	/* 0x000000c9 */
#define mnet_ssd_adas_flex_pic_prim_split_34_relu7_1_pflip_bit_offset 	202	/* 0x000000ca */
#define mnet_ssd_adas_flex_pic_prim_split_34_relu7_1_W 	10
#define mnet_ssd_adas_flex_pic_prim_split_34_relu7_1_H 	10
#define mnet_ssd_adas_flex_pic_prim_split_34_relu7_1_D 	256
#define mnet_ssd_adas_flex_pic_prim_split_34_relu7_1_P 	1
#define mnet_ssd_adas_flex_pic_prim_split_34_relu7_1_data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_34_relu7_1_denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_34_relu7_1_dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_34_relu7_1_dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_34_relu7_1_vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_34_relu7_1_tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_34_relu7_1_tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_34_relu7_1_vmem_buffer_num_bytes 	18432
#define mnet_ssd_adas_flex_pic_prim_split_34_relu7_1_dram_size_num_bytes 	30720
#define mnet_ssd_adas_flex_pic_prim_split_34_relu7_1_sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_34_relu7_1_datasize 	0
#define mnet_ssd_adas_flex_pic_prim_split_34_relu7_1_expoffset 	6
#define mnet_ssd_adas_flex_pic_prim_split_34_relu7_1_expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_34_relu7_1_drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_34_relu7_1_hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_34_relu7_1_vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_34_relu7_1_dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_34_relu7_1_pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_34_relu7_1_trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_34 SMB_input __pvcn_2609_ */
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2609__cnngen_demangled_name 	"__pvcn_2609_"
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2609__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2609__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2609__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2609__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2609__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2609__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2609__byte_offset 	36	/* 0x00000024 */
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2609__vbase_byte_offset 	40	/* 0x00000028 */
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2609__dbase_byte_offset 	64	/* 0x00000040 */
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2609__dpitchm1_byte_offset 	68	/* 0x00000044 */
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2609__dpitchm1_bsize 	32	/* 0x00000020 */
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2609__W 	294912
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2609__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2609__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2609__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2609__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2609__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2609__dpitch_num_bytes 	294912
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2609__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2609__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2609__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2609__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2609__vmem_buffer_num_bytes 	294912
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2609__dram_size_num_bytes 	294912

/* mnet_ssd_adas_flex_pic_prim_split_34 SMB_input __pvcn_2610_ */
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2610__cnngen_demangled_name 	"__pvcn_2610_"
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2610__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2610__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2610__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2610__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2610__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2610__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2610__byte_offset 	72	/* 0x00000048 */
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2610__vbase_byte_offset 	76	/* 0x0000004c */
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2610__dbase_byte_offset 	100	/* 0x00000064 */
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2610__dpitchm1_byte_offset 	104	/* 0x00000068 */
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2610__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2610__W 	704
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2610__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2610__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2610__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2610__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2610__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2610__dpitch_num_bytes 	704
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2610__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2610__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2610__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2610__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2610__vmem_buffer_num_bytes 	704
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2610__dram_size_num_bytes 	704

/* mnet_ssd_adas_flex_pic_prim_split_34 HMB_output __pvcn_2608_ */
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2608__cnngen_demangled_name 	"__pvcn_2608_"
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2608__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2608__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2608__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2608__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2608__byte_offset 	136	/* 0x00000088 */
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2608__vbase_byte_offset 	140	/* 0x0000008c */
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2608__dbase_byte_offset 	164	/* 0x000000a4 */
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2608__dpitchm1_byte_offset 	168	/* 0x000000a8 */
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2608__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2608__vwidth_minus_one_byte_offset 	152	/* 0x00000098 */
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2608__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2608__vheight_minus_one_byte_offset 	154	/* 0x0000009a */
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2608__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2608__drotate_bit_offset 	1286	/* 0x00000506 */
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2608__hflip_bit_offset 	1287	/* 0x00000507 */
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2608__vflip_bit_offset 	1288	/* 0x00000508 */
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2608__dflip_bit_offset 	1289	/* 0x00000509 */
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2608__pflip_bit_offset 	1290	/* 0x0000050a */
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2608__W 	5
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2608__H 	5
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2608__D 	128
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2608__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2608__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2608__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2608__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2608__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2608__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2608__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2608__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2608__vmem_buffer_num_bytes 	4608
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2608__dram_size_num_bytes 	5120
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2608__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2608__datasize 	0
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2608__expoffset 	6
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2608__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2608__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2608__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2608__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2608__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2608__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_34___pvcn_2608__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_34 SMB_output __vas_created_terminate__v0 */
#define mnet_ssd_adas_flex_pic_prim_split_34___vas_created_terminate__v0_cnngen_demangled_name 	"__vas_created_terminate__v0"
#define mnet_ssd_adas_flex_pic_prim_split_34___vas_created_terminate__v0_is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_34___vas_created_terminate__v0_is_bitvector 	1
#define mnet_ssd_adas_flex_pic_prim_split_34___vas_created_terminate__v0_is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_34___vas_created_terminate__v0_is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_34___vas_created_terminate__v0_byte_offset 	176	/* 0x000000b0 */
#define mnet_ssd_adas_flex_pic_prim_split_34___vas_created_terminate__v0_vbase_byte_offset 	180	/* 0x000000b4 */
#define mnet_ssd_adas_flex_pic_prim_split_34___vas_created_terminate__v0_W 	256
#define mnet_ssd_adas_flex_pic_prim_split_34___vas_created_terminate__v0_H 	3
#define mnet_ssd_adas_flex_pic_prim_split_34___vas_created_terminate__v0_D 	1
#define mnet_ssd_adas_flex_pic_prim_split_34___vas_created_terminate__v0_P 	1
#define mnet_ssd_adas_flex_pic_prim_split_34___vas_created_terminate__v0_data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_34___vas_created_terminate__v0_denable 	0
#define mnet_ssd_adas_flex_pic_prim_split_34___vas_created_terminate__v0_dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_34___vas_created_terminate__v0_vp_interleave_mode 	1
#define mnet_ssd_adas_flex_pic_prim_split_34___vas_created_terminate__v0_tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_34___vas_created_terminate__v0_tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_34___vas_created_terminate__v0_vmem_buffer_num_bytes 	96


#endif /* mnet_ssd_adas_flex_pic_prim_split_34_H */
