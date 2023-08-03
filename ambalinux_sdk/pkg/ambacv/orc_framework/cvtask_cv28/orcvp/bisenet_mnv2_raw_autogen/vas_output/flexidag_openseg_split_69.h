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
#ifndef flexidag_openseg_prim_split_69_H
#define flexidag_openseg_prim_split_69_H

#define vas_gen_header_version                  	7

/* dag name */
#define flexidag_openseg_prim_split_69_DAG_NAME 	"flexidag_openseg_prim_split_69"

/* VDG file info */
#define flexidag_openseg_prim_split_69_vdg_name 	"flexidag_openseg_split_69.vdg"

/* VDG DAG memory info */
#define flexidag_openseg_prim_split_69_byte_offset 	4294967295	/* 0xffffffff */
#define flexidag_openseg_prim_split_69_byte_size 	392	/* 0x00000188 */
#define flexidag_openseg_prim_split_69_preferred_dram_xfer_size 	1

/* flexidag_openseg_prim_split_69 VMEM info */
#define flexidag_openseg_prim_split_69_use_ping_pong_vmem 	0
#define flexidag_openseg_prim_split_69_VMEM_start 	4096	/* 0x00001000 */
#define flexidag_openseg_prim_split_69_VMEM_end 	255908	/* 0x0003e7a4 */
#define flexidag_openseg_prim_split_69_image_start 	255828	/* 0x0003e754 */
#define flexidag_openseg_prim_split_69_image_size 	472	/* 0x000001d8 */
#define flexidag_openseg_prim_split_69_dagbin_start 	255908	/* 0x0003e7a4 */

/* flexidag_openseg_prim_split_69 DAG info */
#define flexidag_openseg_prim_split_69_estimated_cycles 	51249
#define flexidag_openseg_prim_split_69_ddi_byte_size 	0	/* 0x00000000 */

/*******************************************************************************
 * DAG byte offset info for HMB/SMB/VCB and their fields
 * in DAG 'flexidag_openseg_prim_split_69' in source file 'pre_split30_flexidag_openseg_split_0.vas'
 ******************************************************************************/
/* flexidag_openseg_prim_split_69 HMB_input Add */
#define flexidag_openseg_prim_split_69_Add_cnngen_demangled_name 	"Add"
#define flexidag_openseg_prim_split_69_Add_is_main_input_output 	0
#define flexidag_openseg_prim_split_69_Add_is_constant 	0
#define flexidag_openseg_prim_split_69_Add_has_init_data 	0
#define flexidag_openseg_prim_split_69_Add_is_variable_scalar 	0
#define flexidag_openseg_prim_split_69_Add_ddi_byte_size 	0	/* 0x00000000 */
#define flexidag_openseg_prim_split_69_Add_ddi_byte_offset 	4294967295	/* 0xffffffff */
#define flexidag_openseg_prim_split_69_Add_is_bitvector 	0
#define flexidag_openseg_prim_split_69_Add_is_rlz 	0
#define flexidag_openseg_prim_split_69_Add_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_69_Add_byte_offset 	0	/* 0x00000000 */
#define flexidag_openseg_prim_split_69_Add_vbase_byte_offset 	4	/* 0x00000004 */
#define flexidag_openseg_prim_split_69_Add_dbase_byte_offset 	28	/* 0x0000001c */
#define flexidag_openseg_prim_split_69_Add_dpitchm1_byte_offset 	32	/* 0x00000020 */
#define flexidag_openseg_prim_split_69_Add_dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_69_Add_vwidth_minus_one_byte_offset 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_69_Add_vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_69_Add_vheight_minus_one_byte_offset 	18	/* 0x00000012 */
#define flexidag_openseg_prim_split_69_Add_vheight_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_69_Add_drotate_bit_offset 	198	/* 0x000000c6 */
#define flexidag_openseg_prim_split_69_Add_hflip_bit_offset 	199	/* 0x000000c7 */
#define flexidag_openseg_prim_split_69_Add_vflip_bit_offset 	200	/* 0x000000c8 */
#define flexidag_openseg_prim_split_69_Add_dflip_bit_offset 	201	/* 0x000000c9 */
#define flexidag_openseg_prim_split_69_Add_pflip_bit_offset 	202	/* 0x000000ca */
#define flexidag_openseg_prim_split_69_Add_W 	160
#define flexidag_openseg_prim_split_69_Add_H 	64
#define flexidag_openseg_prim_split_69_Add_D 	5
#define flexidag_openseg_prim_split_69_Add_P 	1
#define flexidag_openseg_prim_split_69_Add_data_num_bytes 	1
#define flexidag_openseg_prim_split_69_Add_denable 	1
#define flexidag_openseg_prim_split_69_Add_dpitch_num_bytes 	64
#define flexidag_openseg_prim_split_69_Add_dram_format 	3
#define flexidag_openseg_prim_split_69_Add_vp_interleave_mode 	2
#define flexidag_openseg_prim_split_69_Add_tile_width 	2
#define flexidag_openseg_prim_split_69_Add_tile_height 	1
#define flexidag_openseg_prim_split_69_Add_vmem_buffer_num_bytes 	26276
#define flexidag_openseg_prim_split_69_Add_dram_size_num_bytes 	51200
#define flexidag_openseg_prim_split_69_Add_sign 	0
#define flexidag_openseg_prim_split_69_Add_datasize 	0
#define flexidag_openseg_prim_split_69_Add_expoffset 	5
#define flexidag_openseg_prim_split_69_Add_expbits 	0
#define flexidag_openseg_prim_split_69_Add_drotate 	0
#define flexidag_openseg_prim_split_69_Add_hflip 	0
#define flexidag_openseg_prim_split_69_Add_vflip 	0
#define flexidag_openseg_prim_split_69_Add_dflip 	0
#define flexidag_openseg_prim_split_69_Add_pflip 	0
#define flexidag_openseg_prim_split_69_Add_trailing_extra_elements 	0

/* flexidag_openseg_prim_split_69 SMB_input __pvcn_1190_ */
#define flexidag_openseg_prim_split_69___pvcn_1190__cnngen_demangled_name 	"__pvcn_1190_"
#define flexidag_openseg_prim_split_69___pvcn_1190__is_constant 	1
#define flexidag_openseg_prim_split_69___pvcn_1190__has_init_data 	1
#define flexidag_openseg_prim_split_69___pvcn_1190__is_variable_scalar 	0
#define flexidag_openseg_prim_split_69___pvcn_1190__is_bitvector 	0
#define flexidag_openseg_prim_split_69___pvcn_1190__is_rlz 	0
#define flexidag_openseg_prim_split_69___pvcn_1190__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_69___pvcn_1190__byte_offset 	36	/* 0x00000024 */
#define flexidag_openseg_prim_split_69___pvcn_1190__vbase_byte_offset 	40	/* 0x00000028 */
#define flexidag_openseg_prim_split_69___pvcn_1190__dbase_byte_offset 	64	/* 0x00000040 */
#define flexidag_openseg_prim_split_69___pvcn_1190__dpitchm1_byte_offset 	68	/* 0x00000044 */
#define flexidag_openseg_prim_split_69___pvcn_1190__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_69___pvcn_1190__W 	4
#define flexidag_openseg_prim_split_69___pvcn_1190__H 	1
#define flexidag_openseg_prim_split_69___pvcn_1190__D 	1
#define flexidag_openseg_prim_split_69___pvcn_1190__P 	1
#define flexidag_openseg_prim_split_69___pvcn_1190__data_num_bytes 	4
#define flexidag_openseg_prim_split_69___pvcn_1190__denable 	1
#define flexidag_openseg_prim_split_69___pvcn_1190__dpitch_num_bytes 	64
#define flexidag_openseg_prim_split_69___pvcn_1190__dram_format 	0
#define flexidag_openseg_prim_split_69___pvcn_1190__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_69___pvcn_1190__tile_width 	1
#define flexidag_openseg_prim_split_69___pvcn_1190__tile_height 	1
#define flexidag_openseg_prim_split_69___pvcn_1190__vmem_buffer_num_bytes 	16
#define flexidag_openseg_prim_split_69___pvcn_1190__dram_size_num_bytes 	64

/* flexidag_openseg_prim_split_69 SMB_input __pvcn_1192_ */
#define flexidag_openseg_prim_split_69___pvcn_1192__cnngen_demangled_name 	"__pvcn_1192_"
#define flexidag_openseg_prim_split_69___pvcn_1192__is_constant 	1
#define flexidag_openseg_prim_split_69___pvcn_1192__has_init_data 	1
#define flexidag_openseg_prim_split_69___pvcn_1192__is_variable_scalar 	0
#define flexidag_openseg_prim_split_69___pvcn_1192__is_bitvector 	0
#define flexidag_openseg_prim_split_69___pvcn_1192__is_rlz 	0
#define flexidag_openseg_prim_split_69___pvcn_1192__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_69___pvcn_1192__byte_offset 	72	/* 0x00000048 */
#define flexidag_openseg_prim_split_69___pvcn_1192__vbase_byte_offset 	76	/* 0x0000004c */
#define flexidag_openseg_prim_split_69___pvcn_1192__dbase_byte_offset 	100	/* 0x00000064 */
#define flexidag_openseg_prim_split_69___pvcn_1192__dpitchm1_byte_offset 	104	/* 0x00000068 */
#define flexidag_openseg_prim_split_69___pvcn_1192__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_69___pvcn_1192__W 	20
#define flexidag_openseg_prim_split_69___pvcn_1192__H 	1
#define flexidag_openseg_prim_split_69___pvcn_1192__D 	1
#define flexidag_openseg_prim_split_69___pvcn_1192__P 	1
#define flexidag_openseg_prim_split_69___pvcn_1192__data_num_bytes 	1
#define flexidag_openseg_prim_split_69___pvcn_1192__denable 	1
#define flexidag_openseg_prim_split_69___pvcn_1192__dpitch_num_bytes 	64
#define flexidag_openseg_prim_split_69___pvcn_1192__dram_format 	0
#define flexidag_openseg_prim_split_69___pvcn_1192__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_69___pvcn_1192__tile_width 	1
#define flexidag_openseg_prim_split_69___pvcn_1192__tile_height 	1
#define flexidag_openseg_prim_split_69___pvcn_1192__vmem_buffer_num_bytes 	20
#define flexidag_openseg_prim_split_69___pvcn_1192__dram_size_num_bytes 	64

/* flexidag_openseg_prim_split_69 SMB_input __pvcn_1193_ */
#define flexidag_openseg_prim_split_69___pvcn_1193__cnngen_demangled_name 	"__pvcn_1193_"
#define flexidag_openseg_prim_split_69___pvcn_1193__is_constant 	1
#define flexidag_openseg_prim_split_69___pvcn_1193__has_init_data 	1
#define flexidag_openseg_prim_split_69___pvcn_1193__is_variable_scalar 	0
#define flexidag_openseg_prim_split_69___pvcn_1193__is_bitvector 	0
#define flexidag_openseg_prim_split_69___pvcn_1193__is_rlz 	0
#define flexidag_openseg_prim_split_69___pvcn_1193__is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_69___pvcn_1193__byte_offset 	112	/* 0x00000070 */
#define flexidag_openseg_prim_split_69___pvcn_1193__vbase_byte_offset 	116	/* 0x00000074 */
#define flexidag_openseg_prim_split_69___pvcn_1193__dbase_byte_offset 	140	/* 0x0000008c */
#define flexidag_openseg_prim_split_69___pvcn_1193__dpitchm1_byte_offset 	144	/* 0x00000090 */
#define flexidag_openseg_prim_split_69___pvcn_1193__dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_69___pvcn_1193__W 	44
#define flexidag_openseg_prim_split_69___pvcn_1193__H 	1
#define flexidag_openseg_prim_split_69___pvcn_1193__D 	1
#define flexidag_openseg_prim_split_69___pvcn_1193__P 	1
#define flexidag_openseg_prim_split_69___pvcn_1193__data_num_bytes 	1
#define flexidag_openseg_prim_split_69___pvcn_1193__denable 	1
#define flexidag_openseg_prim_split_69___pvcn_1193__dpitch_num_bytes 	64
#define flexidag_openseg_prim_split_69___pvcn_1193__dram_format 	0
#define flexidag_openseg_prim_split_69___pvcn_1193__vp_interleave_mode 	0
#define flexidag_openseg_prim_split_69___pvcn_1193__tile_width 	1
#define flexidag_openseg_prim_split_69___pvcn_1193__tile_height 	1
#define flexidag_openseg_prim_split_69___pvcn_1193__vmem_buffer_num_bytes 	44
#define flexidag_openseg_prim_split_69___pvcn_1193__dram_size_num_bytes 	64

/* flexidag_openseg_prim_split_69 VCB ResizeBilinear_2 */
#define flexidag_openseg_prim_split_69_ResizeBilinear_2_cnngen_demangled_name 	"ResizeBilinear_2"
#define flexidag_openseg_prim_split_69_ResizeBilinear_2_is_bitvector 	0
#define flexidag_openseg_prim_split_69_ResizeBilinear_2_is_rlz 	0
#define flexidag_openseg_prim_split_69_ResizeBilinear_2_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_69_ResizeBilinear_2_byte_offset 	164	/* 0x000000a4 */
#define flexidag_openseg_prim_split_69_ResizeBilinear_2_vbase_byte_offset 	172	/* 0x000000ac */
#define flexidag_openseg_prim_split_69_ResizeBilinear_2_vmem_buffer_num_bytes 	102480

/* flexidag_openseg_prim_split_69 VCB logits__Conv2D___BiasAdd */
#define flexidag_openseg_prim_split_69_logits__Conv2D___BiasAdd_cnngen_demangled_name 	"logits__Conv2D___BiasAdd"
#define flexidag_openseg_prim_split_69_logits__Conv2D___BiasAdd_is_bitvector 	0
#define flexidag_openseg_prim_split_69_logits__Conv2D___BiasAdd_is_rlz 	0
#define flexidag_openseg_prim_split_69_logits__Conv2D___BiasAdd_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_69_logits__Conv2D___BiasAdd_byte_offset 	212	/* 0x000000d4 */
#define flexidag_openseg_prim_split_69_logits__Conv2D___BiasAdd_vbase_byte_offset 	220	/* 0x000000dc */
#define flexidag_openseg_prim_split_69_logits__Conv2D___BiasAdd_vmem_buffer_num_bytes 	102400

/* flexidag_openseg_prim_split_69 HMB_output out_class */
#define flexidag_openseg_prim_split_69_out_class_cnngen_demangled_name 	"out_class"
#define flexidag_openseg_prim_split_69_out_class_is_main_input_output 	1
#define flexidag_openseg_prim_split_69_out_class_is_bitvector 	0
#define flexidag_openseg_prim_split_69_out_class_is_rlz 	0
#define flexidag_openseg_prim_split_69_out_class_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_69_out_class_byte_offset 	280	/* 0x00000118 */
#define flexidag_openseg_prim_split_69_out_class_vbase_byte_offset 	284	/* 0x0000011c */
#define flexidag_openseg_prim_split_69_out_class_dbase_byte_offset 	304	/* 0x00000130 */
#define flexidag_openseg_prim_split_69_out_class_dpitchm1_byte_offset 	308	/* 0x00000134 */
#define flexidag_openseg_prim_split_69_out_class_dpitchm1_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_69_out_class_vwidth_minus_one_byte_offset 	296	/* 0x00000128 */
#define flexidag_openseg_prim_split_69_out_class_vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_69_out_class_vheight_minus_one_byte_offset 	298	/* 0x0000012a */
#define flexidag_openseg_prim_split_69_out_class_vheight_minus_one_bsize 	16	/* 0x00000010 */
#define flexidag_openseg_prim_split_69_out_class_drotate_bit_offset 	2406	/* 0x00000966 */
#define flexidag_openseg_prim_split_69_out_class_hflip_bit_offset 	2407	/* 0x00000967 */
#define flexidag_openseg_prim_split_69_out_class_vflip_bit_offset 	2408	/* 0x00000968 */
#define flexidag_openseg_prim_split_69_out_class_dflip_bit_offset 	2409	/* 0x00000969 */
#define flexidag_openseg_prim_split_69_out_class_pflip_bit_offset 	2410	/* 0x0000096a */
#define flexidag_openseg_prim_split_69_out_class_W 	320
#define flexidag_openseg_prim_split_69_out_class_H 	128
#define flexidag_openseg_prim_split_69_out_class_D 	1
#define flexidag_openseg_prim_split_69_out_class_P 	1
#define flexidag_openseg_prim_split_69_out_class_data_num_bytes 	1
#define flexidag_openseg_prim_split_69_out_class_denable 	1
#define flexidag_openseg_prim_split_69_out_class_dpitch_num_bytes 	320
#define flexidag_openseg_prim_split_69_out_class_dram_format 	0
#define flexidag_openseg_prim_split_69_out_class_vp_interleave_mode 	0
#define flexidag_openseg_prim_split_69_out_class_tile_width 	1
#define flexidag_openseg_prim_split_69_out_class_tile_height 	1
#define flexidag_openseg_prim_split_69_out_class_vmem_buffer_num_bytes 	20480
#define flexidag_openseg_prim_split_69_out_class_dram_size_num_bytes 	40960
#define flexidag_openseg_prim_split_69_out_class_sign 	0
#define flexidag_openseg_prim_split_69_out_class_datasize 	0
#define flexidag_openseg_prim_split_69_out_class_expoffset 	0
#define flexidag_openseg_prim_split_69_out_class_expbits 	0
#define flexidag_openseg_prim_split_69_out_class_drotate 	0
#define flexidag_openseg_prim_split_69_out_class_hflip 	0
#define flexidag_openseg_prim_split_69_out_class_vflip 	0
#define flexidag_openseg_prim_split_69_out_class_dflip 	0
#define flexidag_openseg_prim_split_69_out_class_pflip 	0
#define flexidag_openseg_prim_split_69_out_class_trailing_extra_elements 	0

/* flexidag_openseg_prim_split_69 SMB_output __vas_created_terminate__v0 */
#define flexidag_openseg_prim_split_69___vas_created_terminate__v0_cnngen_demangled_name 	"__vas_created_terminate__v0"
#define flexidag_openseg_prim_split_69___vas_created_terminate__v0_is_main_input_output 	0
#define flexidag_openseg_prim_split_69___vas_created_terminate__v0_is_bitvector 	1
#define flexidag_openseg_prim_split_69___vas_created_terminate__v0_is_rlz 	0
#define flexidag_openseg_prim_split_69___vas_created_terminate__v0_is_ten_bit_packed 	0
#define flexidag_openseg_prim_split_69___vas_created_terminate__v0_byte_offset 	316	/* 0x0000013c */
#define flexidag_openseg_prim_split_69___vas_created_terminate__v0_vbase_byte_offset 	320	/* 0x00000140 */
#define flexidag_openseg_prim_split_69___vas_created_terminate__v0_W 	256
#define flexidag_openseg_prim_split_69___vas_created_terminate__v0_H 	3
#define flexidag_openseg_prim_split_69___vas_created_terminate__v0_D 	1
#define flexidag_openseg_prim_split_69___vas_created_terminate__v0_P 	1
#define flexidag_openseg_prim_split_69___vas_created_terminate__v0_data_num_bytes 	1
#define flexidag_openseg_prim_split_69___vas_created_terminate__v0_denable 	0
#define flexidag_openseg_prim_split_69___vas_created_terminate__v0_dram_format 	0
#define flexidag_openseg_prim_split_69___vas_created_terminate__v0_vp_interleave_mode 	1
#define flexidag_openseg_prim_split_69___vas_created_terminate__v0_tile_width 	1
#define flexidag_openseg_prim_split_69___vas_created_terminate__v0_tile_height 	1
#define flexidag_openseg_prim_split_69___vas_created_terminate__v0_vmem_buffer_num_bytes 	96


#endif /* flexidag_openseg_prim_split_69_H */
