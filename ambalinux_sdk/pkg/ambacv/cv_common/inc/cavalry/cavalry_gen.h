/*
 * Copyright (c) 2018-2018 Ambarella, Inc.
 * 2018/08/17 - [Monica Yang] created file
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign copyrights.
 * This Software is also the confidential and proprietary information of
 * Ambarella, Inc. and its licensors.  You may not use, reproduce, disclose,
 * distribute, modify, or otherwise prepare derivative works of this Software
 * or any portion thereof except pursuant to a signed license agreement or
 * nondisclosure agreement with Ambarella, Inc. or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and
 * return this Software to Ambarella, Inc.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __CAVALRY_GEN_H__
#define __CAVALRY_GEN_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define CAVALRY_GEN_VERSION		(0x02010007)
#define CAVALRY_FILENAME_MAX		(128)
#define CAVALRY_VAR_NAME_MAX		(128)
#define CAVALRY_IO_NAME_MAX		(512)
#define CAVALRY_IO_DEMNGL_NAME_MAX		(512)
#define CAVALRY_IO_PARENT_NAME_MAX		(64)
#define CAVALRY_VPROC_VAR_NAME_MAX		(32)
#define CAVALRY_VPROC_SMB_NAME_MAX		(16)
#define CAVALRY_PATH_MAX			(2048)
#define CAVALRY_LINE_LEN_MAX		(1024)

/* cavalry_gen will set invalid value if not found data_format */
typedef enum datasize_type_s {
	DATASIZE_8_BITS = 0,
	DATASIZE_16_BITS = 1,
	DATASIZE_32_BITS = 2,
	DATASIZE_64_BITS = 3,
	DATASIZE_INVALID = 0xFF,
} datasize_type_t;

/* header info */
typedef struct cavalry_gen_header_s {
	uint32_t version_info;
	uint32_t version_hash;
	uint32_t dvi_num;
} cavalry_gen_header_t;

/*per dvi descriptor */
typedef struct dvi_desc_s {
	uint8_t dvi_id;
	uint8_t vproc_id;
	uint8_t dvi_ppv;
	uint8_t reserve_dvi;
	uint32_t dvi_img_vaddr;
	uint32_t dvi_img_size;
	uint32_t dvi_dag_vaddr;
	uint32_t input_num;
	uint32_t output_num;
	uint32_t dvi_pkg_size;
	char dag_name[CAVALRY_VAR_NAME_MAX];
} dvi_desc_t;

/*per port descriptor (HMB_input HMB_output) */
typedef struct io_descriptor_s{
	uint32_t port_dim_p;
	uint32_t port_dim_d;
	uint32_t port_dim_h;
	uint32_t port_dim_w;
	uint32_t port_pitch;		/* dpitch_num_bytes */
	uint32_t port_pitch_offset;
	uint32_t port_pitch_bsize: 6;
	uint32_t port_dram_format: 4;
	uint32_t port_dim_bitvector: 1;
	uint32_t port_drotate: 1;
	uint32_t port_hflip: 1;
	uint32_t port_vflip: 1;
	uint32_t port_dflip: 1;
	uint32_t port_pflip: 1;
	uint32_t reserve_port: 7;
	uint32_t port_slice_total_num : 4;
	uint32_t port_slice_seq : 4;
	uint32_t port_is_main_io: 1;
	uint32_t port_slice_byte_offset;
	char port_slice_parent_name[CAVALRY_IO_PARENT_NAME_MAX];

	uint32_t port_drotate_bit_offset;
	uint32_t port_hflip_bit_offset;
	uint32_t port_vflip_bit_offset;
	uint32_t port_dflip_bit_offset;
	uint32_t port_pflip_bit_offset;

	uint8_t port_data_sign;	/*data format: sign, datasize, expoffset, expbits*/
	uint8_t port_data_size;
	int8_t port_data_expoffset;
	uint8_t port_data_expbits;

	uint32_t port_size;
	uint32_t port_byte_offset;	/* dbase_byte_offset */
	char port_name[CAVALRY_IO_NAME_MAX];
	char port_demangled_name[CAVALRY_IO_DEMNGL_NAME_MAX];
} io_descriptor_t;

/* vproc descriptors */
typedef struct vproc_desc_s {
	uint32_t var_num;
	uint32_t smb_num;
	uint32_t vproc_pkg_size;
}vproc_desc_t;

/* functional parameters */
typedef struct func_variable_s{
	char var_name[CAVALRY_VPROC_VAR_NAME_MAX];
	uint32_t var_boffset;
	uint32_t var_bsize;
} func_variable_t;

/* SMB descriptor */
typedef struct smb_descriptor_s{
	char smb_name[CAVALRY_VPROC_SMB_NAME_MAX];
	uint32_t vbase_byte_offset;
} smb_descriptor_t;


#ifdef __cplusplus
}
#endif

#endif

