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
#ifndef mnet_ssd_adas_flex_pic_prim_split_3_H
#define mnet_ssd_adas_flex_pic_prim_split_3_H

#define vas_gen_header_version                  	7

/* dag name */
#define mnet_ssd_adas_flex_pic_prim_split_3_DAG_NAME 	"mnet_ssd_adas_flex_pic_prim_split_3"

/* VDG file info */
#define mnet_ssd_adas_flex_pic_prim_split_3_vdg_name 	"mnet_ssd_adas_flex_pic_split_3.vdg"

/* VDG DAG memory info */
#define mnet_ssd_adas_flex_pic_prim_split_3_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_3_byte_size 	2316	/* 0x0000090c */
#define mnet_ssd_adas_flex_pic_prim_split_3_preferred_dram_xfer_size 	1

/* mnet_ssd_adas_flex_pic_prim_split_3 VMEM info */
#define mnet_ssd_adas_flex_pic_prim_split_3_use_ping_pong_vmem 	0
#define mnet_ssd_adas_flex_pic_prim_split_3_VMEM_start 	4096	/* 0x00001000 */
#define mnet_ssd_adas_flex_pic_prim_split_3_VMEM_end 	513868	/* 0x0007d74c */
#define mnet_ssd_adas_flex_pic_prim_split_3_image_start 	495200	/* 0x00078e60 */
#define mnet_ssd_adas_flex_pic_prim_split_3_image_size 	20984	/* 0x000051f8 */
#define mnet_ssd_adas_flex_pic_prim_split_3_dagbin_start 	513868	/* 0x0007d74c */

/* mnet_ssd_adas_flex_pic_prim_split_3 DAG info */
#define mnet_ssd_adas_flex_pic_prim_split_3_estimated_cycles 	364195
#define mnet_ssd_adas_flex_pic_prim_split_3_ddi_byte_size 	0	/* 0x00000000 */

/*******************************************************************************
 * DAG byte offset info for HMB/SMB/VCB and their fields
 * in DAG 'mnet_ssd_adas_flex_pic_prim_split_3' in source file 'pre_split3_mnet_ssd_adas_flex_pic_split_0.vas'
 ******************************************************************************/
/* mnet_ssd_adas_flex_pic_prim_split_3 SMB_input __pvcn_529_ */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_529__cnngen_demangled_name 	"__pvcn_529_"
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_529__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_529__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_529__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_529__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_529__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_529__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_529__byte_offset 	12	/* 0x0000000c */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_529__vbase_byte_offset 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_529__dbase_byte_offset 	40	/* 0x00000028 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_529__dpitchm1_byte_offset 	44	/* 0x0000002c */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_529__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_529__W 	15120
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_529__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_529__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_529__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_529__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_529__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_529__dpitch_num_bytes 	15168
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_529__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_529__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_529__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_529__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_529__vmem_buffer_num_bytes 	15120
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_529__dram_size_num_bytes 	15168

/* mnet_ssd_adas_flex_pic_prim_split_3 SMB_input __pvcn_530_ */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_530__cnngen_demangled_name 	"__pvcn_530_"
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_530__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_530__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_530__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_530__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_530__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_530__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_530__byte_offset 	48	/* 0x00000030 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_530__vbase_byte_offset 	52	/* 0x00000034 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_530__dbase_byte_offset 	76	/* 0x0000004c */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_530__dpitchm1_byte_offset 	80	/* 0x00000050 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_530__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_530__W 	1192
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_530__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_530__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_530__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_530__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_530__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_530__dpitch_num_bytes 	1216
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_530__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_530__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_530__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_530__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_530__vmem_buffer_num_bytes 	1192
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_530__dram_size_num_bytes 	1216

/* mnet_ssd_adas_flex_pic_prim_split_3 SMB_input __pvcn_547_ */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_547__cnngen_demangled_name 	"__pvcn_547_"
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_547__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_547__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_547__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_547__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_547__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_547__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_547__byte_offset 	100	/* 0x00000064 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_547__vbase_byte_offset 	104	/* 0x00000068 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_547__dbase_byte_offset 	128	/* 0x00000080 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_547__dpitchm1_byte_offset 	132	/* 0x00000084 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_547__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_547__W 	96
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_547__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_547__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_547__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_547__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_547__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_547__dpitch_num_bytes 	128
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_547__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_547__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_547__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_547__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_547__vmem_buffer_num_bytes 	96
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_547__dram_size_num_bytes 	128

/* mnet_ssd_adas_flex_pic_prim_split_3 SMB_input __pvcn_548_ */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_548__cnngen_demangled_name 	"__pvcn_548_"
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_548__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_548__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_548__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_548__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_548__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_548__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_548__byte_offset 	140	/* 0x0000008c */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_548__vbase_byte_offset 	144	/* 0x00000090 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_548__dbase_byte_offset 	168	/* 0x000000a8 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_548__dpitchm1_byte_offset 	172	/* 0x000000ac */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_548__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_548__W 	68
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_548__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_548__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_548__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_548__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_548__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_548__dpitch_num_bytes 	128
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_548__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_548__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_548__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_548__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_548__vmem_buffer_num_bytes 	68
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_548__dram_size_num_bytes 	128

/* mnet_ssd_adas_flex_pic_prim_split_3 SMB_input __pvcn_558_ */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_558__cnngen_demangled_name 	"__pvcn_558_"
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_558__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_558__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_558__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_558__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_558__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_558__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_558__byte_offset 	180	/* 0x000000b4 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_558__vbase_byte_offset 	184	/* 0x000000b8 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_558__dbase_byte_offset 	208	/* 0x000000d0 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_558__dpitchm1_byte_offset 	212	/* 0x000000d4 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_558__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_558__W 	96
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_558__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_558__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_558__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_558__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_558__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_558__dpitch_num_bytes 	128
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_558__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_558__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_558__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_558__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_558__vmem_buffer_num_bytes 	96
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_558__dram_size_num_bytes 	128

/* mnet_ssd_adas_flex_pic_prim_split_3 SMB_input __pvcn_559_ */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_559__cnngen_demangled_name 	"__pvcn_559_"
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_559__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_559__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_559__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_559__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_559__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_559__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_559__byte_offset 	220	/* 0x000000dc */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_559__vbase_byte_offset 	224	/* 0x000000e0 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_559__dbase_byte_offset 	248	/* 0x000000f8 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_559__dpitchm1_byte_offset 	252	/* 0x000000fc */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_559__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_559__W 	68
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_559__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_559__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_559__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_559__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_559__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_559__dpitch_num_bytes 	128
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_559__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_559__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_559__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_559__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_559__vmem_buffer_num_bytes 	68
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_559__dram_size_num_bytes 	128

/* mnet_ssd_adas_flex_pic_prim_split_3 SMB_input __pvcn_569_ */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_569__cnngen_demangled_name 	"__pvcn_569_"
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_569__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_569__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_569__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_569__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_569__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_569__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_569__byte_offset 	260	/* 0x00000104 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_569__vbase_byte_offset 	264	/* 0x00000108 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_569__dbase_byte_offset 	288	/* 0x00000120 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_569__dpitchm1_byte_offset 	292	/* 0x00000124 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_569__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_569__W 	96
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_569__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_569__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_569__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_569__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_569__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_569__dpitch_num_bytes 	128
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_569__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_569__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_569__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_569__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_569__vmem_buffer_num_bytes 	96
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_569__dram_size_num_bytes 	128

/* mnet_ssd_adas_flex_pic_prim_split_3 SMB_input __pvcn_570_ */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_570__cnngen_demangled_name 	"__pvcn_570_"
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_570__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_570__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_570__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_570__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_570__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_570__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_570__byte_offset 	300	/* 0x0000012c */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_570__vbase_byte_offset 	304	/* 0x00000130 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_570__dbase_byte_offset 	328	/* 0x00000148 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_570__dpitchm1_byte_offset 	332	/* 0x0000014c */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_570__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_570__W 	68
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_570__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_570__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_570__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_570__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_570__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_570__dpitch_num_bytes 	128
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_570__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_570__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_570__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_570__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_570__vmem_buffer_num_bytes 	68
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_570__dram_size_num_bytes 	128

/* mnet_ssd_adas_flex_pic_prim_split_3 SMB_input __pvcn_580_ */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_580__cnngen_demangled_name 	"__pvcn_580_"
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_580__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_580__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_580__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_580__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_580__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_580__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_580__byte_offset 	340	/* 0x00000154 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_580__vbase_byte_offset 	344	/* 0x00000158 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_580__dbase_byte_offset 	368	/* 0x00000170 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_580__dpitchm1_byte_offset 	372	/* 0x00000174 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_580__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_580__W 	96
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_580__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_580__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_580__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_580__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_580__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_580__dpitch_num_bytes 	128
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_580__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_580__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_580__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_580__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_580__vmem_buffer_num_bytes 	96
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_580__dram_size_num_bytes 	128

/* mnet_ssd_adas_flex_pic_prim_split_3 SMB_input __pvcn_581_ */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_581__cnngen_demangled_name 	"__pvcn_581_"
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_581__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_581__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_581__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_581__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_581__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_581__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_581__byte_offset 	380	/* 0x0000017c */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_581__vbase_byte_offset 	384	/* 0x00000180 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_581__dbase_byte_offset 	408	/* 0x00000198 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_581__dpitchm1_byte_offset 	412	/* 0x0000019c */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_581__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_581__W 	72
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_581__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_581__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_581__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_581__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_581__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_581__dpitch_num_bytes 	128
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_581__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_581__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_581__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_581__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_581__vmem_buffer_num_bytes 	72
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_581__dram_size_num_bytes 	128

/* mnet_ssd_adas_flex_pic_prim_split_3 SMB_input __pvcn_591_ */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_591__cnngen_demangled_name 	"__pvcn_591_"
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_591__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_591__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_591__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_591__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_591__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_591__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_591__byte_offset 	420	/* 0x000001a4 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_591__vbase_byte_offset 	424	/* 0x000001a8 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_591__dbase_byte_offset 	448	/* 0x000001c0 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_591__dpitchm1_byte_offset 	452	/* 0x000001c4 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_591__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_591__W 	96
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_591__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_591__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_591__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_591__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_591__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_591__dpitch_num_bytes 	128
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_591__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_591__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_591__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_591__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_591__vmem_buffer_num_bytes 	96
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_591__dram_size_num_bytes 	128

/* mnet_ssd_adas_flex_pic_prim_split_3 SMB_input __pvcn_592_ */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_592__cnngen_demangled_name 	"__pvcn_592_"
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_592__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_592__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_592__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_592__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_592__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_592__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_592__byte_offset 	460	/* 0x000001cc */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_592__vbase_byte_offset 	464	/* 0x000001d0 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_592__dbase_byte_offset 	488	/* 0x000001e8 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_592__dpitchm1_byte_offset 	492	/* 0x000001ec */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_592__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_592__W 	68
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_592__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_592__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_592__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_592__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_592__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_592__dpitch_num_bytes 	128
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_592__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_592__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_592__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_592__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_592__vmem_buffer_num_bytes 	68
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_592__dram_size_num_bytes 	128

/* mnet_ssd_adas_flex_pic_prim_split_3 SMB_input __pvcn_602_ */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_602__cnngen_demangled_name 	"__pvcn_602_"
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_602__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_602__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_602__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_602__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_602__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_602__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_602__byte_offset 	500	/* 0x000001f4 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_602__vbase_byte_offset 	504	/* 0x000001f8 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_602__dbase_byte_offset 	528	/* 0x00000210 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_602__dpitchm1_byte_offset 	532	/* 0x00000214 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_602__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_602__W 	100
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_602__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_602__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_602__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_602__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_602__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_602__dpitch_num_bytes 	128
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_602__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_602__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_602__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_602__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_602__vmem_buffer_num_bytes 	100
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_602__dram_size_num_bytes 	128

/* mnet_ssd_adas_flex_pic_prim_split_3 SMB_input __pvcn_603_ */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_603__cnngen_demangled_name 	"__pvcn_603_"
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_603__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_603__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_603__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_603__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_603__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_603__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_603__byte_offset 	540	/* 0x0000021c */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_603__vbase_byte_offset 	544	/* 0x00000220 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_603__dbase_byte_offset 	568	/* 0x00000238 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_603__dpitchm1_byte_offset 	572	/* 0x0000023c */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_603__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_603__W 	72
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_603__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_603__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_603__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_603__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_603__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_603__dpitch_num_bytes 	128
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_603__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_603__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_603__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_603__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_603__vmem_buffer_num_bytes 	72
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_603__dram_size_num_bytes 	128

/* mnet_ssd_adas_flex_pic_prim_split_3 SMB_input __pvcn_613_ */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_613__cnngen_demangled_name 	"__pvcn_613_"
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_613__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_613__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_613__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_613__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_613__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_613__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_613__byte_offset 	580	/* 0x00000244 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_613__vbase_byte_offset 	584	/* 0x00000248 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_613__dbase_byte_offset 	608	/* 0x00000260 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_613__dpitchm1_byte_offset 	612	/* 0x00000264 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_613__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_613__W 	104
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_613__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_613__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_613__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_613__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_613__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_613__dpitch_num_bytes 	128
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_613__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_613__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_613__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_613__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_613__vmem_buffer_num_bytes 	104
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_613__dram_size_num_bytes 	128

/* mnet_ssd_adas_flex_pic_prim_split_3 SMB_input __pvcn_614_ */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_614__cnngen_demangled_name 	"__pvcn_614_"
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_614__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_614__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_614__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_614__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_614__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_614__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_614__byte_offset 	620	/* 0x0000026c */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_614__vbase_byte_offset 	624	/* 0x00000270 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_614__dbase_byte_offset 	648	/* 0x00000288 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_614__dpitchm1_byte_offset 	652	/* 0x0000028c */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_614__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_614__W 	76
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_614__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_614__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_614__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_614__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_614__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_614__dpitch_num_bytes 	128
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_614__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_614__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_614__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_614__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_614__vmem_buffer_num_bytes 	76
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_614__dram_size_num_bytes 	128

/* mnet_ssd_adas_flex_pic_prim_split_3 SMB_input __pvcn_624_ */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_624__cnngen_demangled_name 	"__pvcn_624_"
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_624__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_624__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_624__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_624__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_624__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_624__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_624__byte_offset 	660	/* 0x00000294 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_624__vbase_byte_offset 	664	/* 0x00000298 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_624__dbase_byte_offset 	688	/* 0x000002b0 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_624__dpitchm1_byte_offset 	692	/* 0x000002b4 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_624__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_624__W 	100
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_624__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_624__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_624__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_624__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_624__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_624__dpitch_num_bytes 	128
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_624__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_624__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_624__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_624__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_624__vmem_buffer_num_bytes 	100
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_624__dram_size_num_bytes 	128

/* mnet_ssd_adas_flex_pic_prim_split_3 SMB_input __pvcn_625_ */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_625__cnngen_demangled_name 	"__pvcn_625_"
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_625__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_625__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_625__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_625__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_625__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_625__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_625__byte_offset 	700	/* 0x000002bc */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_625__vbase_byte_offset 	704	/* 0x000002c0 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_625__dbase_byte_offset 	728	/* 0x000002d8 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_625__dpitchm1_byte_offset 	732	/* 0x000002dc */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_625__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_625__W 	72
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_625__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_625__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_625__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_625__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_625__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_625__dpitch_num_bytes 	128
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_625__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_625__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_625__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_625__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_625__vmem_buffer_num_bytes 	72
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_625__dram_size_num_bytes 	128

/* mnet_ssd_adas_flex_pic_prim_split_3 SMB_input __pvcn_635_ */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_635__cnngen_demangled_name 	"__pvcn_635_"
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_635__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_635__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_635__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_635__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_635__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_635__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_635__byte_offset 	740	/* 0x000002e4 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_635__vbase_byte_offset 	744	/* 0x000002e8 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_635__dbase_byte_offset 	768	/* 0x00000300 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_635__dpitchm1_byte_offset 	772	/* 0x00000304 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_635__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_635__W 	96
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_635__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_635__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_635__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_635__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_635__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_635__dpitch_num_bytes 	128
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_635__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_635__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_635__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_635__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_635__vmem_buffer_num_bytes 	96
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_635__dram_size_num_bytes 	128

/* mnet_ssd_adas_flex_pic_prim_split_3 SMB_input __pvcn_636_ */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_636__cnngen_demangled_name 	"__pvcn_636_"
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_636__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_636__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_636__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_636__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_636__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_636__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_636__byte_offset 	780	/* 0x0000030c */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_636__vbase_byte_offset 	784	/* 0x00000310 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_636__dbase_byte_offset 	808	/* 0x00000328 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_636__dpitchm1_byte_offset 	812	/* 0x0000032c */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_636__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_636__W 	72
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_636__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_636__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_636__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_636__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_636__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_636__dpitch_num_bytes 	128
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_636__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_636__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_636__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_636__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_636__vmem_buffer_num_bytes 	72
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_636__dram_size_num_bytes 	128

/* mnet_ssd_adas_flex_pic_prim_split_3 SMB_input __pvcn_646_ */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_646__cnngen_demangled_name 	"__pvcn_646_"
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_646__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_646__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_646__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_646__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_646__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_646__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_646__byte_offset 	820	/* 0x00000334 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_646__vbase_byte_offset 	824	/* 0x00000338 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_646__dbase_byte_offset 	848	/* 0x00000350 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_646__dpitchm1_byte_offset 	852	/* 0x00000354 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_646__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_646__W 	96
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_646__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_646__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_646__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_646__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_646__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_646__dpitch_num_bytes 	128
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_646__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_646__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_646__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_646__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_646__vmem_buffer_num_bytes 	96
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_646__dram_size_num_bytes 	128

/* mnet_ssd_adas_flex_pic_prim_split_3 SMB_input __pvcn_647_ */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_647__cnngen_demangled_name 	"__pvcn_647_"
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_647__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_647__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_647__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_647__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_647__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_647__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_647__byte_offset 	860	/* 0x0000035c */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_647__vbase_byte_offset 	864	/* 0x00000360 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_647__dbase_byte_offset 	888	/* 0x00000378 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_647__dpitchm1_byte_offset 	892	/* 0x0000037c */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_647__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_647__W 	68
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_647__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_647__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_647__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_647__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_647__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_647__dpitch_num_bytes 	128
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_647__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_647__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_647__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_647__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_647__vmem_buffer_num_bytes 	68
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_647__dram_size_num_bytes 	128

/* mnet_ssd_adas_flex_pic_prim_split_3 SMB_input __pvcn_657_ */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_657__cnngen_demangled_name 	"__pvcn_657_"
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_657__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_657__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_657__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_657__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_657__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_657__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_657__byte_offset 	900	/* 0x00000384 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_657__vbase_byte_offset 	904	/* 0x00000388 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_657__dbase_byte_offset 	928	/* 0x000003a0 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_657__dpitchm1_byte_offset 	932	/* 0x000003a4 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_657__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_657__W 	96
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_657__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_657__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_657__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_657__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_657__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_657__dpitch_num_bytes 	128
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_657__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_657__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_657__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_657__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_657__vmem_buffer_num_bytes 	96
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_657__dram_size_num_bytes 	128

/* mnet_ssd_adas_flex_pic_prim_split_3 SMB_input __pvcn_658_ */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_658__cnngen_demangled_name 	"__pvcn_658_"
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_658__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_658__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_658__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_658__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_658__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_658__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_658__byte_offset 	940	/* 0x000003ac */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_658__vbase_byte_offset 	944	/* 0x000003b0 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_658__dbase_byte_offset 	968	/* 0x000003c8 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_658__dpitchm1_byte_offset 	972	/* 0x000003cc */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_658__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_658__W 	68
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_658__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_658__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_658__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_658__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_658__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_658__dpitch_num_bytes 	128
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_658__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_658__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_658__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_658__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_658__vmem_buffer_num_bytes 	68
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_658__dram_size_num_bytes 	128

/* mnet_ssd_adas_flex_pic_prim_split_3 SMB_input __pvcn_668_ */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_668__cnngen_demangled_name 	"__pvcn_668_"
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_668__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_668__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_668__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_668__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_668__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_668__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_668__byte_offset 	980	/* 0x000003d4 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_668__vbase_byte_offset 	984	/* 0x000003d8 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_668__dbase_byte_offset 	1008	/* 0x000003f0 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_668__dpitchm1_byte_offset 	1012	/* 0x000003f4 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_668__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_668__W 	104
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_668__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_668__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_668__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_668__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_668__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_668__dpitch_num_bytes 	128
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_668__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_668__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_668__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_668__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_668__vmem_buffer_num_bytes 	104
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_668__dram_size_num_bytes 	128

/* mnet_ssd_adas_flex_pic_prim_split_3 SMB_input __pvcn_669_ */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_669__cnngen_demangled_name 	"__pvcn_669_"
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_669__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_669__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_669__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_669__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_669__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_669__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_669__byte_offset 	1020	/* 0x000003fc */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_669__vbase_byte_offset 	1024	/* 0x00000400 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_669__dbase_byte_offset 	1048	/* 0x00000418 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_669__dpitchm1_byte_offset 	1052	/* 0x0000041c */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_669__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_669__W 	72
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_669__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_669__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_669__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_669__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_669__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_669__dpitch_num_bytes 	128
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_669__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_669__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_669__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_669__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_669__vmem_buffer_num_bytes 	72
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_669__dram_size_num_bytes 	128

/* mnet_ssd_adas_flex_pic_prim_split_3 SMB_input __pvcn_679_ */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_679__cnngen_demangled_name 	"__pvcn_679_"
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_679__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_679__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_679__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_679__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_679__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_679__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_679__byte_offset 	1060	/* 0x00000424 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_679__vbase_byte_offset 	1064	/* 0x00000428 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_679__dbase_byte_offset 	1088	/* 0x00000440 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_679__dpitchm1_byte_offset 	1092	/* 0x00000444 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_679__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_679__W 	96
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_679__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_679__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_679__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_679__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_679__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_679__dpitch_num_bytes 	128
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_679__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_679__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_679__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_679__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_679__vmem_buffer_num_bytes 	96
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_679__dram_size_num_bytes 	128

/* mnet_ssd_adas_flex_pic_prim_split_3 SMB_input __pvcn_680_ */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_680__cnngen_demangled_name 	"__pvcn_680_"
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_680__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_680__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_680__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_680__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_680__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_680__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_680__byte_offset 	1100	/* 0x0000044c */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_680__vbase_byte_offset 	1104	/* 0x00000450 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_680__dbase_byte_offset 	1128	/* 0x00000468 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_680__dpitchm1_byte_offset 	1132	/* 0x0000046c */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_680__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_680__W 	76
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_680__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_680__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_680__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_680__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_680__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_680__dpitch_num_bytes 	128
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_680__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_680__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_680__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_680__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_680__vmem_buffer_num_bytes 	76
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_680__dram_size_num_bytes 	128

/* mnet_ssd_adas_flex_pic_prim_split_3 SMB_input __pvcn_690_ */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_690__cnngen_demangled_name 	"__pvcn_690_"
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_690__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_690__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_690__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_690__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_690__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_690__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_690__byte_offset 	1140	/* 0x00000474 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_690__vbase_byte_offset 	1144	/* 0x00000478 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_690__dbase_byte_offset 	1168	/* 0x00000490 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_690__dpitchm1_byte_offset 	1172	/* 0x00000494 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_690__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_690__W 	96
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_690__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_690__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_690__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_690__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_690__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_690__dpitch_num_bytes 	128
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_690__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_690__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_690__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_690__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_690__vmem_buffer_num_bytes 	96
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_690__dram_size_num_bytes 	128

/* mnet_ssd_adas_flex_pic_prim_split_3 SMB_input __pvcn_691_ */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_691__cnngen_demangled_name 	"__pvcn_691_"
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_691__is_constant 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_691__has_init_data 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_691__is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_691__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_691__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_691__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_691__byte_offset 	1180	/* 0x0000049c */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_691__vbase_byte_offset 	1184	/* 0x000004a0 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_691__dbase_byte_offset 	1208	/* 0x000004b8 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_691__dpitchm1_byte_offset 	1212	/* 0x000004bc */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_691__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_691__W 	68
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_691__H 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_691__D 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_691__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_691__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_691__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_691__dpitch_num_bytes 	128
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_691__dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_691__vp_interleave_mode 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_691__tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_691__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_691__vmem_buffer_num_bytes 	68
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_691__dram_size_num_bytes 	128

/* mnet_ssd_adas_flex_pic_prim_split_3 HMB_input conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw */
#define mnet_ssd_adas_flex_pic_prim_split_3_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_cnngen_demangled_name 	"conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw"
#define mnet_ssd_adas_flex_pic_prim_split_3_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_3_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_is_constant 	0
#define mnet_ssd_adas_flex_pic_prim_split_3_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_has_init_data 	0
#define mnet_ssd_adas_flex_pic_prim_split_3_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_is_variable_scalar 	0
#define mnet_ssd_adas_flex_pic_prim_split_3_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_ddi_byte_size 	0	/* 0x00000000 */
#define mnet_ssd_adas_flex_pic_prim_split_3_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_ddi_byte_offset 	4294967295	/* 0xffffffff */
#define mnet_ssd_adas_flex_pic_prim_split_3_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_3_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_3_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_3_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_byte_offset 	1220	/* 0x000004c4 */
#define mnet_ssd_adas_flex_pic_prim_split_3_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_vbase_byte_offset 	1224	/* 0x000004c8 */
#define mnet_ssd_adas_flex_pic_prim_split_3_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_dbase_byte_offset 	1248	/* 0x000004e0 */
#define mnet_ssd_adas_flex_pic_prim_split_3_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_dpitchm1_byte_offset 	1252	/* 0x000004e4 */
#define mnet_ssd_adas_flex_pic_prim_split_3_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_vwidth_minus_one_byte_offset 	1236	/* 0x000004d4 */
#define mnet_ssd_adas_flex_pic_prim_split_3_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_vheight_minus_one_byte_offset 	1238	/* 0x000004d6 */
#define mnet_ssd_adas_flex_pic_prim_split_3_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_drotate_bit_offset 	9958	/* 0x000026e6 */
#define mnet_ssd_adas_flex_pic_prim_split_3_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_hflip_bit_offset 	9959	/* 0x000026e7 */
#define mnet_ssd_adas_flex_pic_prim_split_3_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_vflip_bit_offset 	9960	/* 0x000026e8 */
#define mnet_ssd_adas_flex_pic_prim_split_3_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_dflip_bit_offset 	9961	/* 0x000026e9 */
#define mnet_ssd_adas_flex_pic_prim_split_3_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_pflip_bit_offset 	9962	/* 0x000026ea */
#define mnet_ssd_adas_flex_pic_prim_split_3_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_W 	75
#define mnet_ssd_adas_flex_pic_prim_split_3_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_H 	75
#define mnet_ssd_adas_flex_pic_prim_split_3_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_D 	128
#define mnet_ssd_adas_flex_pic_prim_split_3_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_P 	1
#define mnet_ssd_adas_flex_pic_prim_split_3_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_3_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_3_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_3_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_3_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_3_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_3_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_3_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_vmem_buffer_num_bytes 	83968
#define mnet_ssd_adas_flex_pic_prim_split_3_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_dram_size_num_bytes 	768000
#define mnet_ssd_adas_flex_pic_prim_split_3_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_3_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_datasize 	0
#define mnet_ssd_adas_flex_pic_prim_split_3_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_expoffset 	6
#define mnet_ssd_adas_flex_pic_prim_split_3_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_3_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_3_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_3_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_3_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_3_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_3_conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_3 VCB __pvcn_531_ */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_531__cnngen_demangled_name 	"__pvcn_531_"
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_531__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_531__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_531__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_531__byte_offset 	1272	/* 0x000004f8 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_531__vbase_byte_offset 	1280	/* 0x00000500 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_531__vmem_buffer_num_bytes 	81920

/* mnet_ssd_adas_flex_pic_prim_split_3 VCB conv3_1__sep_____bn_conv3_1__sep__scale_mul___muli___26_____bn_conv3_1__sep__scale_mul_____scale___muli___340 */
#define mnet_ssd_adas_flex_pic_prim_split_3_conv3_1__sep_____bn_conv3_1__sep__scale_mul___muli___26_____bn_conv3_1__sep__scale_mul_____scale___muli___340_cnngen_demangled_name 	"conv3_1__sep_____bn_conv3_1__sep__scale_mul___muli___26_____bn_conv3_1__sep__scale_mul_____scale___muli___340"
#define mnet_ssd_adas_flex_pic_prim_split_3_conv3_1__sep_____bn_conv3_1__sep__scale_mul___muli___26_____bn_conv3_1__sep__scale_mul_____scale___muli___340_is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_3_conv3_1__sep_____bn_conv3_1__sep__scale_mul___muli___26_____bn_conv3_1__sep__scale_mul_____scale___muli___340_is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_3_conv3_1__sep_____bn_conv3_1__sep__scale_mul___muli___26_____bn_conv3_1__sep__scale_mul_____scale___muli___340_is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_3_conv3_1__sep_____bn_conv3_1__sep__scale_mul___muli___26_____bn_conv3_1__sep__scale_mul_____scale___muli___340_byte_offset 	1352	/* 0x00000548 */
#define mnet_ssd_adas_flex_pic_prim_split_3_conv3_1__sep_____bn_conv3_1__sep__scale_mul___muli___26_____bn_conv3_1__sep__scale_mul_____scale___muli___340_vbase_byte_offset 	1360	/* 0x00000550 */
#define mnet_ssd_adas_flex_pic_prim_split_3_conv3_1__sep_____bn_conv3_1__sep__scale_mul___muli___26_____bn_conv3_1__sep__scale_mul_____scale___muli___340_vmem_buffer_num_bytes 	189440

/* mnet_ssd_adas_flex_pic_prim_split_3 HMB_output __pvcn_549_ */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_549__cnngen_demangled_name 	"__pvcn_549_"
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_549__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_549__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_549__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_549__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_549__byte_offset 	1424	/* 0x00000590 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_549__vbase_byte_offset 	1428	/* 0x00000594 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_549__dbase_byte_offset 	1452	/* 0x000005ac */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_549__dpitchm1_byte_offset 	1456	/* 0x000005b0 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_549__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_549__vwidth_minus_one_byte_offset 	1440	/* 0x000005a0 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_549__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_549__vheight_minus_one_byte_offset 	1442	/* 0x000005a2 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_549__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_549__drotate_bit_offset 	11590	/* 0x00002d46 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_549__hflip_bit_offset 	11591	/* 0x00002d47 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_549__vflip_bit_offset 	11592	/* 0x00002d48 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_549__dflip_bit_offset 	11593	/* 0x00002d49 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_549__pflip_bit_offset 	11594	/* 0x00002d4a */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_549__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_549__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_549__D 	7
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_549__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_549__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_549__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_549__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_549__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_549__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_549__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_549__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_549__vmem_buffer_num_bytes 	4480
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_549__dram_size_num_bytes 	21280
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_549__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_549__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_549__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_549__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_549__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_549__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_549__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_549__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_549__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_549__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_3 HMB_output __pvcn_560_ */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_560__cnngen_demangled_name 	"__pvcn_560_"
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_560__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_560__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_560__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_560__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_560__byte_offset 	1480	/* 0x000005c8 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_560__vbase_byte_offset 	1484	/* 0x000005cc */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_560__dbase_byte_offset 	1508	/* 0x000005e4 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_560__dpitchm1_byte_offset 	1512	/* 0x000005e8 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_560__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_560__vwidth_minus_one_byte_offset 	1496	/* 0x000005d8 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_560__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_560__vheight_minus_one_byte_offset 	1498	/* 0x000005da */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_560__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_560__drotate_bit_offset 	12038	/* 0x00002f06 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_560__hflip_bit_offset 	12039	/* 0x00002f07 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_560__vflip_bit_offset 	12040	/* 0x00002f08 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_560__dflip_bit_offset 	12041	/* 0x00002f09 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_560__pflip_bit_offset 	12042	/* 0x00002f0a */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_560__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_560__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_560__D 	7
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_560__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_560__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_560__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_560__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_560__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_560__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_560__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_560__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_560__vmem_buffer_num_bytes 	4480
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_560__dram_size_num_bytes 	21280
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_560__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_560__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_560__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_560__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_560__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_560__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_560__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_560__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_560__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_560__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_3 HMB_output __pvcn_571_ */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_571__cnngen_demangled_name 	"__pvcn_571_"
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_571__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_571__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_571__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_571__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_571__byte_offset 	1536	/* 0x00000600 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_571__vbase_byte_offset 	1540	/* 0x00000604 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_571__dbase_byte_offset 	1564	/* 0x0000061c */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_571__dpitchm1_byte_offset 	1568	/* 0x00000620 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_571__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_571__vwidth_minus_one_byte_offset 	1552	/* 0x00000610 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_571__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_571__vheight_minus_one_byte_offset 	1554	/* 0x00000612 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_571__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_571__drotate_bit_offset 	12486	/* 0x000030c6 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_571__hflip_bit_offset 	12487	/* 0x000030c7 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_571__vflip_bit_offset 	12488	/* 0x000030c8 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_571__dflip_bit_offset 	12489	/* 0x000030c9 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_571__pflip_bit_offset 	12490	/* 0x000030ca */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_571__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_571__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_571__D 	7
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_571__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_571__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_571__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_571__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_571__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_571__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_571__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_571__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_571__vmem_buffer_num_bytes 	4480
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_571__dram_size_num_bytes 	21280
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_571__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_571__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_571__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_571__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_571__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_571__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_571__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_571__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_571__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_571__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_3 HMB_output __pvcn_582_ */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_582__cnngen_demangled_name 	"__pvcn_582_"
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_582__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_582__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_582__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_582__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_582__byte_offset 	1592	/* 0x00000638 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_582__vbase_byte_offset 	1596	/* 0x0000063c */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_582__dbase_byte_offset 	1620	/* 0x00000654 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_582__dpitchm1_byte_offset 	1624	/* 0x00000658 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_582__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_582__vwidth_minus_one_byte_offset 	1608	/* 0x00000648 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_582__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_582__vheight_minus_one_byte_offset 	1610	/* 0x0000064a */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_582__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_582__drotate_bit_offset 	12934	/* 0x00003286 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_582__hflip_bit_offset 	12935	/* 0x00003287 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_582__vflip_bit_offset 	12936	/* 0x00003288 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_582__dflip_bit_offset 	12937	/* 0x00003289 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_582__pflip_bit_offset 	12938	/* 0x0000328a */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_582__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_582__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_582__D 	8
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_582__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_582__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_582__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_582__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_582__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_582__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_582__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_582__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_582__vmem_buffer_num_bytes 	5120
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_582__dram_size_num_bytes 	24320
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_582__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_582__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_582__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_582__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_582__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_582__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_582__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_582__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_582__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_582__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_3 HMB_output __pvcn_593_ */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_593__cnngen_demangled_name 	"__pvcn_593_"
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_593__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_593__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_593__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_593__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_593__byte_offset 	1648	/* 0x00000670 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_593__vbase_byte_offset 	1652	/* 0x00000674 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_593__dbase_byte_offset 	1676	/* 0x0000068c */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_593__dpitchm1_byte_offset 	1680	/* 0x00000690 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_593__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_593__vwidth_minus_one_byte_offset 	1664	/* 0x00000680 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_593__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_593__vheight_minus_one_byte_offset 	1666	/* 0x00000682 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_593__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_593__drotate_bit_offset 	13382	/* 0x00003446 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_593__hflip_bit_offset 	13383	/* 0x00003447 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_593__vflip_bit_offset 	13384	/* 0x00003448 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_593__dflip_bit_offset 	13385	/* 0x00003449 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_593__pflip_bit_offset 	13386	/* 0x0000344a */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_593__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_593__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_593__D 	7
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_593__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_593__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_593__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_593__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_593__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_593__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_593__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_593__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_593__vmem_buffer_num_bytes 	4480
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_593__dram_size_num_bytes 	21280
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_593__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_593__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_593__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_593__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_593__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_593__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_593__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_593__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_593__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_593__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_3 HMB_output __pvcn_604_ */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_604__cnngen_demangled_name 	"__pvcn_604_"
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_604__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_604__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_604__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_604__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_604__byte_offset 	1704	/* 0x000006a8 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_604__vbase_byte_offset 	1708	/* 0x000006ac */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_604__dbase_byte_offset 	1732	/* 0x000006c4 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_604__dpitchm1_byte_offset 	1736	/* 0x000006c8 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_604__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_604__vwidth_minus_one_byte_offset 	1720	/* 0x000006b8 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_604__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_604__vheight_minus_one_byte_offset 	1722	/* 0x000006ba */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_604__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_604__drotate_bit_offset 	13830	/* 0x00003606 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_604__hflip_bit_offset 	13831	/* 0x00003607 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_604__vflip_bit_offset 	13832	/* 0x00003608 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_604__dflip_bit_offset 	13833	/* 0x00003609 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_604__pflip_bit_offset 	13834	/* 0x0000360a */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_604__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_604__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_604__D 	8
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_604__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_604__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_604__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_604__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_604__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_604__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_604__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_604__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_604__vmem_buffer_num_bytes 	5120
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_604__dram_size_num_bytes 	24320
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_604__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_604__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_604__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_604__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_604__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_604__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_604__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_604__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_604__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_604__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_3 HMB_output __pvcn_615_ */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_615__cnngen_demangled_name 	"__pvcn_615_"
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_615__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_615__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_615__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_615__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_615__byte_offset 	1760	/* 0x000006e0 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_615__vbase_byte_offset 	1764	/* 0x000006e4 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_615__dbase_byte_offset 	1788	/* 0x000006fc */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_615__dpitchm1_byte_offset 	1792	/* 0x00000700 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_615__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_615__vwidth_minus_one_byte_offset 	1776	/* 0x000006f0 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_615__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_615__vheight_minus_one_byte_offset 	1778	/* 0x000006f2 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_615__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_615__drotate_bit_offset 	14278	/* 0x000037c6 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_615__hflip_bit_offset 	14279	/* 0x000037c7 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_615__vflip_bit_offset 	14280	/* 0x000037c8 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_615__dflip_bit_offset 	14281	/* 0x000037c9 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_615__pflip_bit_offset 	14282	/* 0x000037ca */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_615__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_615__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_615__D 	9
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_615__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_615__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_615__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_615__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_615__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_615__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_615__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_615__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_615__vmem_buffer_num_bytes 	5760
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_615__dram_size_num_bytes 	27360
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_615__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_615__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_615__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_615__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_615__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_615__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_615__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_615__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_615__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_615__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_3 HMB_output __pvcn_626_ */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_626__cnngen_demangled_name 	"__pvcn_626_"
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_626__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_626__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_626__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_626__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_626__byte_offset 	1816	/* 0x00000718 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_626__vbase_byte_offset 	1820	/* 0x0000071c */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_626__dbase_byte_offset 	1844	/* 0x00000734 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_626__dpitchm1_byte_offset 	1848	/* 0x00000738 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_626__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_626__vwidth_minus_one_byte_offset 	1832	/* 0x00000728 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_626__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_626__vheight_minus_one_byte_offset 	1834	/* 0x0000072a */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_626__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_626__drotate_bit_offset 	14726	/* 0x00003986 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_626__hflip_bit_offset 	14727	/* 0x00003987 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_626__vflip_bit_offset 	14728	/* 0x00003988 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_626__dflip_bit_offset 	14729	/* 0x00003989 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_626__pflip_bit_offset 	14730	/* 0x0000398a */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_626__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_626__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_626__D 	8
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_626__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_626__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_626__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_626__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_626__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_626__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_626__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_626__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_626__vmem_buffer_num_bytes 	5120
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_626__dram_size_num_bytes 	24320
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_626__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_626__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_626__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_626__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_626__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_626__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_626__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_626__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_626__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_626__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_3 HMB_output __pvcn_637_ */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_637__cnngen_demangled_name 	"__pvcn_637_"
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_637__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_637__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_637__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_637__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_637__byte_offset 	1872	/* 0x00000750 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_637__vbase_byte_offset 	1876	/* 0x00000754 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_637__dbase_byte_offset 	1900	/* 0x0000076c */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_637__dpitchm1_byte_offset 	1904	/* 0x00000770 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_637__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_637__vwidth_minus_one_byte_offset 	1888	/* 0x00000760 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_637__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_637__vheight_minus_one_byte_offset 	1890	/* 0x00000762 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_637__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_637__drotate_bit_offset 	15174	/* 0x00003b46 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_637__hflip_bit_offset 	15175	/* 0x00003b47 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_637__vflip_bit_offset 	15176	/* 0x00003b48 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_637__dflip_bit_offset 	15177	/* 0x00003b49 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_637__pflip_bit_offset 	15178	/* 0x00003b4a */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_637__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_637__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_637__D 	8
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_637__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_637__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_637__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_637__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_637__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_637__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_637__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_637__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_637__vmem_buffer_num_bytes 	2560
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_637__dram_size_num_bytes 	24320
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_637__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_637__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_637__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_637__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_637__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_637__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_637__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_637__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_637__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_637__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_3 HMB_output __pvcn_648_ */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_648__cnngen_demangled_name 	"__pvcn_648_"
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_648__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_648__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_648__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_648__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_648__byte_offset 	1928	/* 0x00000788 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_648__vbase_byte_offset 	1932	/* 0x0000078c */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_648__dbase_byte_offset 	1956	/* 0x000007a4 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_648__dpitchm1_byte_offset 	1960	/* 0x000007a8 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_648__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_648__vwidth_minus_one_byte_offset 	1944	/* 0x00000798 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_648__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_648__vheight_minus_one_byte_offset 	1946	/* 0x0000079a */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_648__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_648__drotate_bit_offset 	15622	/* 0x00003d06 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_648__hflip_bit_offset 	15623	/* 0x00003d07 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_648__vflip_bit_offset 	15624	/* 0x00003d08 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_648__dflip_bit_offset 	15625	/* 0x00003d09 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_648__pflip_bit_offset 	15626	/* 0x00003d0a */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_648__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_648__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_648__D 	7
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_648__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_648__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_648__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_648__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_648__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_648__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_648__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_648__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_648__vmem_buffer_num_bytes 	2240
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_648__dram_size_num_bytes 	21280
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_648__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_648__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_648__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_648__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_648__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_648__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_648__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_648__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_648__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_648__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_3 HMB_output __pvcn_659_ */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_659__cnngen_demangled_name 	"__pvcn_659_"
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_659__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_659__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_659__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_659__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_659__byte_offset 	1984	/* 0x000007c0 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_659__vbase_byte_offset 	1988	/* 0x000007c4 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_659__dbase_byte_offset 	2012	/* 0x000007dc */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_659__dpitchm1_byte_offset 	2016	/* 0x000007e0 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_659__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_659__vwidth_minus_one_byte_offset 	2000	/* 0x000007d0 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_659__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_659__vheight_minus_one_byte_offset 	2002	/* 0x000007d2 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_659__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_659__drotate_bit_offset 	16070	/* 0x00003ec6 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_659__hflip_bit_offset 	16071	/* 0x00003ec7 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_659__vflip_bit_offset 	16072	/* 0x00003ec8 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_659__dflip_bit_offset 	16073	/* 0x00003ec9 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_659__pflip_bit_offset 	16074	/* 0x00003eca */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_659__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_659__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_659__D 	7
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_659__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_659__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_659__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_659__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_659__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_659__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_659__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_659__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_659__vmem_buffer_num_bytes 	2240
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_659__dram_size_num_bytes 	21280
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_659__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_659__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_659__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_659__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_659__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_659__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_659__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_659__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_659__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_659__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_3 HMB_output __pvcn_670_ */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_670__cnngen_demangled_name 	"__pvcn_670_"
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_670__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_670__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_670__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_670__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_670__byte_offset 	2040	/* 0x000007f8 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_670__vbase_byte_offset 	2044	/* 0x000007fc */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_670__dbase_byte_offset 	2068	/* 0x00000814 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_670__dpitchm1_byte_offset 	2072	/* 0x00000818 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_670__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_670__vwidth_minus_one_byte_offset 	2056	/* 0x00000808 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_670__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_670__vheight_minus_one_byte_offset 	2058	/* 0x0000080a */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_670__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_670__drotate_bit_offset 	16518	/* 0x00004086 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_670__hflip_bit_offset 	16519	/* 0x00004087 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_670__vflip_bit_offset 	16520	/* 0x00004088 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_670__dflip_bit_offset 	16521	/* 0x00004089 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_670__pflip_bit_offset 	16522	/* 0x0000408a */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_670__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_670__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_670__D 	8
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_670__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_670__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_670__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_670__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_670__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_670__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_670__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_670__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_670__vmem_buffer_num_bytes 	2560
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_670__dram_size_num_bytes 	24320
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_670__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_670__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_670__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_670__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_670__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_670__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_670__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_670__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_670__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_670__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_3 HMB_output __pvcn_681_ */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_681__cnngen_demangled_name 	"__pvcn_681_"
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_681__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_681__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_681__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_681__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_681__byte_offset 	2096	/* 0x00000830 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_681__vbase_byte_offset 	2100	/* 0x00000834 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_681__dbase_byte_offset 	2124	/* 0x0000084c */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_681__dpitchm1_byte_offset 	2128	/* 0x00000850 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_681__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_681__vwidth_minus_one_byte_offset 	2112	/* 0x00000840 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_681__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_681__vheight_minus_one_byte_offset 	2114	/* 0x00000842 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_681__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_681__drotate_bit_offset 	16966	/* 0x00004246 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_681__hflip_bit_offset 	16967	/* 0x00004247 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_681__vflip_bit_offset 	16968	/* 0x00004248 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_681__dflip_bit_offset 	16969	/* 0x00004249 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_681__pflip_bit_offset 	16970	/* 0x0000424a */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_681__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_681__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_681__D 	9
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_681__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_681__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_681__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_681__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_681__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_681__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_681__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_681__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_681__vmem_buffer_num_bytes 	2880
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_681__dram_size_num_bytes 	27360
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_681__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_681__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_681__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_681__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_681__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_681__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_681__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_681__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_681__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_681__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_3 HMB_output __pvcn_692_ */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_692__cnngen_demangled_name 	"__pvcn_692_"
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_692__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_692__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_692__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_692__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_692__byte_offset 	2152	/* 0x00000868 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_692__vbase_byte_offset 	2156	/* 0x0000086c */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_692__dbase_byte_offset 	2180	/* 0x00000884 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_692__dpitchm1_byte_offset 	2184	/* 0x00000888 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_692__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_692__vwidth_minus_one_byte_offset 	2168	/* 0x00000878 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_692__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_692__vheight_minus_one_byte_offset 	2170	/* 0x0000087a */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_692__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_692__drotate_bit_offset 	17414	/* 0x00004406 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_692__hflip_bit_offset 	17415	/* 0x00004407 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_692__vflip_bit_offset 	17416	/* 0x00004408 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_692__dflip_bit_offset 	17417	/* 0x00004409 */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_692__pflip_bit_offset 	17418	/* 0x0000440a */
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_692__W 	38
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_692__H 	38
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_692__D 	7
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_692__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_692__data_num_bytes 	2
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_692__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_692__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_692__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_692__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_692__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_692__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_692__vmem_buffer_num_bytes 	2240
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_692__dram_size_num_bytes 	21280
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_692__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_692__datasize 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_692__expoffset 	11
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_692__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_692__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_692__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_692__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_692__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_692__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___pvcn_692__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_3 HMB_output __vcn_2726_ */
#define mnet_ssd_adas_flex_pic_prim_split_3___vcn_2726__cnngen_demangled_name 	"__vcn_2726_"
#define mnet_ssd_adas_flex_pic_prim_split_3___vcn_2726__is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___vcn_2726__is_bitvector 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___vcn_2726__is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___vcn_2726__is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___vcn_2726__byte_offset 	2200	/* 0x00000898 */
#define mnet_ssd_adas_flex_pic_prim_split_3___vcn_2726__vbase_byte_offset 	2204	/* 0x0000089c */
#define mnet_ssd_adas_flex_pic_prim_split_3___vcn_2726__dbase_byte_offset 	2228	/* 0x000008b4 */
#define mnet_ssd_adas_flex_pic_prim_split_3___vcn_2726__dpitchm1_byte_offset 	2232	/* 0x000008b8 */
#define mnet_ssd_adas_flex_pic_prim_split_3___vcn_2726__dpitchm1_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___vcn_2726__vwidth_minus_one_byte_offset 	2216	/* 0x000008a8 */
#define mnet_ssd_adas_flex_pic_prim_split_3___vcn_2726__vwidth_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___vcn_2726__vheight_minus_one_byte_offset 	2218	/* 0x000008aa */
#define mnet_ssd_adas_flex_pic_prim_split_3___vcn_2726__vheight_minus_one_bsize 	16	/* 0x00000010 */
#define mnet_ssd_adas_flex_pic_prim_split_3___vcn_2726__drotate_bit_offset 	17798	/* 0x00004586 */
#define mnet_ssd_adas_flex_pic_prim_split_3___vcn_2726__hflip_bit_offset 	17799	/* 0x00004587 */
#define mnet_ssd_adas_flex_pic_prim_split_3___vcn_2726__vflip_bit_offset 	17800	/* 0x00004588 */
#define mnet_ssd_adas_flex_pic_prim_split_3___vcn_2726__dflip_bit_offset 	17801	/* 0x00004589 */
#define mnet_ssd_adas_flex_pic_prim_split_3___vcn_2726__pflip_bit_offset 	17802	/* 0x0000458a */
#define mnet_ssd_adas_flex_pic_prim_split_3___vcn_2726__W 	75
#define mnet_ssd_adas_flex_pic_prim_split_3___vcn_2726__H 	75
#define mnet_ssd_adas_flex_pic_prim_split_3___vcn_2726__D 	128
#define mnet_ssd_adas_flex_pic_prim_split_3___vcn_2726__P 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___vcn_2726__data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___vcn_2726__denable 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___vcn_2726__dpitch_num_bytes 	64
#define mnet_ssd_adas_flex_pic_prim_split_3___vcn_2726__dram_format 	3
#define mnet_ssd_adas_flex_pic_prim_split_3___vcn_2726__vp_interleave_mode 	2
#define mnet_ssd_adas_flex_pic_prim_split_3___vcn_2726__tile_width 	4
#define mnet_ssd_adas_flex_pic_prim_split_3___vcn_2726__tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___vcn_2726__vmem_buffer_num_bytes 	81920
#define mnet_ssd_adas_flex_pic_prim_split_3___vcn_2726__dram_size_num_bytes 	768000
#define mnet_ssd_adas_flex_pic_prim_split_3___vcn_2726__sign 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___vcn_2726__datasize 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___vcn_2726__expoffset 	6
#define mnet_ssd_adas_flex_pic_prim_split_3___vcn_2726__expbits 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___vcn_2726__drotate 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___vcn_2726__hflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___vcn_2726__vflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___vcn_2726__dflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___vcn_2726__pflip 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___vcn_2726__trailing_extra_elements 	0

/* mnet_ssd_adas_flex_pic_prim_split_3 SMB_output __vas_created_terminate__v0 */
#define mnet_ssd_adas_flex_pic_prim_split_3___vas_created_terminate__v0_cnngen_demangled_name 	"__vas_created_terminate__v0"
#define mnet_ssd_adas_flex_pic_prim_split_3___vas_created_terminate__v0_is_main_input_output 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___vas_created_terminate__v0_is_bitvector 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___vas_created_terminate__v0_is_rlz 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___vas_created_terminate__v0_is_ten_bit_packed 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___vas_created_terminate__v0_byte_offset 	2240	/* 0x000008c0 */
#define mnet_ssd_adas_flex_pic_prim_split_3___vas_created_terminate__v0_vbase_byte_offset 	2244	/* 0x000008c4 */
#define mnet_ssd_adas_flex_pic_prim_split_3___vas_created_terminate__v0_W 	256
#define mnet_ssd_adas_flex_pic_prim_split_3___vas_created_terminate__v0_H 	3
#define mnet_ssd_adas_flex_pic_prim_split_3___vas_created_terminate__v0_D 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___vas_created_terminate__v0_P 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___vas_created_terminate__v0_data_num_bytes 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___vas_created_terminate__v0_denable 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___vas_created_terminate__v0_dram_format 	0
#define mnet_ssd_adas_flex_pic_prim_split_3___vas_created_terminate__v0_vp_interleave_mode 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___vas_created_terminate__v0_tile_width 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___vas_created_terminate__v0_tile_height 	1
#define mnet_ssd_adas_flex_pic_prim_split_3___vas_created_terminate__v0_vmem_buffer_num_bytes 	96


#endif /* mnet_ssd_adas_flex_pic_prim_split_3_H */
