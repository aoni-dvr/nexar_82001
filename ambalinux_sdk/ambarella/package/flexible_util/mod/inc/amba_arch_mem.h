/*
 * amba_arch_mem.h
 *
 * History:
 *	2015/07/21 - [Jian Tang] created file
 *
 * Copyright (c) 2016 Ambarella, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __AMBA_ARCH_MEM_H__
#define __AMBA_ARCH_MEM_H__

// #include <config.h>
#ifndef CONFIG_AMBARELLA_MAX_CHANNEL_NUM
#define CONFIG_AMBARELLA_MAX_CHANNEL_NUM  (1)
#endif

/**
 * IAV / DSP MEMORY Layout:
 *      It's same as the DSP buffer layout in "/amboot/include/dsp.h".
 *
 *       +----------------------+ <--- IDSP_RAM_START
 *       | DSP_BSB_SIZE         |
 *       +----------------------+
 *       | DSP_INT_BSB_SIZE     |
 *       +----------------------+
 *       | DSP_IAVRSVD_SIZE     |
 *       +----------------------+
 *       | DSP_FASTDATA_SIZE    |
 *       +----------------------+
 *       | DSP_FASTAUDIO_SIZE   |
 *       +----------------------+
 *       | DSP_BUFFER_SIZE      |
 *       +----------------------+
 *       | DSP_CMD_BUF_SIZE     |
 *       +----------------------+
 *       | DSP_MSG_BUF_SIZE     |
 *       +----------------------+
 *       | DSP_BSH_SIZE         |
 *       +----------------------+
 *       | DSP_LOG_SIZE         |
 *       +----------------------+
 *       | DSP_UCODE_SIZE       |
 *       +----------------------+
 *
 */

/*
 * Note:
 *   1. "start" means the first byte of physical address.
 *   2. "base" means the first byte of virtual address.
 *   3. DSP_INT_BSB_SIZE, DSP_BSB_SIZE ,DSP_IAVRSVD_SIZE, DSP_FASTAUDIO_SIZE are specified by menuconfig.
 *   4. DSP_IAVRSVD_SIZE is the size of memory reserved for IAV drivers.
 *   5. DSP_FASTDATA_SIZE is the size of memory reserved for Fastboot data.
 *      If FastData is disabled, DSP_FASTDATA_SIZE should be set zero, or not defined.
 *      It's used for store dsp_status/vin_video_format in amboot.
 *   6. DSP_FASTAUDIO_SIZE is the size of memory reserved for FastAudio.
 *      If FastAudio is disabled, DSP_FASTAUDIO_SIZE should be set zero, or not defined.
 *   7. DSP_BSH_SIZE is the size of memory for storing BIT_STREAM_HDR.
 */

/* IAV_MARGIN_SIZE is the memory margin size for IAV driver excluding IMG/BSB.
 * It is used to decide the reserved memory for IAV driver.
 * Normally below condition should be satisfied.
 * IAV_MARGIN_SIZE + IAV_DRAM_IMG + IAV_DRAM_BSB + IAV_DRAM_INT_BSB >= IAV_DRAM_MAX
 */


#ifndef DBGBUS_BASE
#define DBGBUS_BASE			(0xed000000)
#endif

#ifndef VIN_BASE_OFFSET
#define VIN_BASE_OFFSET		(0x1C0000)
#endif

#ifndef SECTION_SEL_REG_OFFSET
#define SECTION_SEL_REG_OFFSET	(0x1C8000)
#endif

#ifndef SECTION_RST_REG_OFFSET
#define SECTION_RST_REG_OFFSET	(0x1C801C)
#endif

#ifndef SECTION_SEL_REG_OFFSET
#define SECTION_SEL_REG_OFFSET	(0x1C8000)
#endif

#ifndef IAV_ROI_NUM_FOR_IPB_FRAMES
#define IAV_ROI_NUM_FOR_IPB_FRAMES		1
#endif

/* DSP buffer size */
#ifndef AMBCMA_DEFAULT_DSP_BUFFER_SIZE
#define AMBCMA_DEFAULT_DSP_BUFFER_SIZE		(0xF000000)
#endif

#ifndef IAV_MEM_INTRA_PB_SIZE
#define IAV_MEM_INTRA_PB_SIZE	0
#endif

#ifndef IAV_MEM_USR_SIZE
#define IAV_MEM_USR_SIZE		0
#endif

#ifndef IAV_MEM_MV_SIZE
#define IAV_MEM_MV_SIZE		0
#endif

#ifndef IAV_MV_STREAM_NUM
#define IAV_MV_STREAM_NUM	0
#endif

#ifndef CONFIG_PIC_STATISTICS_DUMP_STREAM_NUM
#define CONFIG_PIC_STATISTICS_DUMP_STREAM_NUM	0
#endif

#ifndef IAV_MEM_OVERLAY_SIZE
#define IAV_MEM_OVERLAY_SIZE			0
#endif

#ifndef IAV_MEM_BLUR_SIZE
#define IAV_MEM_BLUR_SIZE				0
#endif

#ifndef IAV_MEM_IMG_SBP_SIZE
#define IAV_MEM_IMG_SBP_SIZE			0
#endif

#ifndef IAV_MEM_IMG_NN_BF_SIZE
#define IAV_MEM_IMG_NN_BF_SIZE	0
#endif

#ifndef IAV_MEM_IMG_IK_CFG_SIZE
#define IAV_MEM_IMG_IK_CFG_SIZE		(0x500000 * CONFIG_AMBARELLA_MAX_CHANNEL_NUM)
#endif

#if (IAV_MEM_IMG_IK_CFG_SIZE == 0)
#undef IAV_MEM_IMG_IK_CFG_SIZE
#define IAV_MEM_IMG_IK_CFG_SIZE		(0x500000 * CONFIG_AMBARELLA_MAX_CHANNEL_NUM)
#endif

#ifndef IAV_MEM_ROI_MATRIX_SIZE
#define IAV_MEM_ROI_MATRIX_SIZE		0
#endif

#ifndef CONFIG_AMBARELLA_IAV_DRAM_WARP_MEM
#define CONFIG_AMBARELLA_IAV_DRAM_WARP_MEM		0
#endif

#ifndef DSP_INT_BSB_SIZE
#define DSP_INT_BSB_SIZE	0
#endif

#ifndef DSP_BSB_SIZE
#define DSP_BSB_SIZE		0
#endif

#ifndef IAV_EXTRA_RAW_SIZE
#define IAV_EXTRA_RAW_SIZE		0
#endif

#ifndef IAV_MEM_TFC_SIZE
#define IAV_MEM_TFC_SIZE		0
#endif

#ifndef IAV_MEM_DIS_CTB_STAT_SIZE
#define IAV_MEM_DIS_CTB_STAT_SIZE		0
#endif

#ifndef CONFIG_AMBARELLA_MAX_VIN_WIDTH
#define CONFIG_AMBARELLA_MAX_VIN_WIDTH		(4096)
#endif

#ifndef CONFIG_AMBARELLA_MAX_VIN_HEIGHT
#define CONFIG_AMBARELLA_MAX_VIN_HEIGHT		(4096)
#endif

#ifndef CONFIG_AMBARELLA_IAV_DRAM_DECODE_ONLY

#define	IAV_MASK_RSVD_SIZE	(((7 << 20) + (512 << 10)) * CONFIG_AMBARELLA_MAX_CHANNEL_NUM)

#define	IAV_PIC_STAT_RSVD_SIZE	((64 << 10) * CONFIG_PIC_STATISTICS_DUMP_STREAM_NUM)

#ifndef CONFIG_AMBARELLA_IAV_ROI_PBG

#if (IAV_ROI_NUM_FOR_IPB_FRAMES == 1)
#define IAV_MARGIN_SIZE		((16 << 20) + (512 << 10) + IAV_MEM_USR_SIZE \
	+ IAV_PIC_STAT_RSVD_SIZE + IAV_MEM_MV_SIZE * IAV_MV_STREAM_NUM + IAV_MASK_RSVD_SIZE)
#elif (IAV_ROI_NUM_FOR_IPB_FRAMES == 2)
#define IAV_MARGIN_SIZE		((21 << 20) + (512 << 10) + IAV_MEM_USR_SIZE \
	+ IAV_PIC_STAT_RSVD_SIZE + IAV_MEM_MV_SIZE * IAV_MV_STREAM_NUM + IAV_MASK_RSVD_SIZE)
#elif (IAV_ROI_NUM_FOR_IPB_FRAMES == 3)
#define IAV_MARGIN_SIZE		((26 << 20) + (512 << 10) + IAV_MEM_USR_SIZE \
	+ IAV_PIC_STAT_RSVD_SIZE + IAV_MEM_MV_SIZE * IAV_MV_STREAM_NUM + IAV_MASK_RSVD_SIZE)
#elif (IAV_ROI_NUM_FOR_IPB_FRAMES == 0)
#define IAV_MARGIN_SIZE		((13 << 20) + (512 << 10) + IAV_MEM_USR_SIZE \
	+ IAV_PIC_STAT_RSVD_SIZE + IAV_MEM_MV_SIZE * IAV_MV_STREAM_NUM + IAV_MASK_RSVD_SIZE)
#endif

#else

#if (IAV_ROI_NUM_FOR_IPB_FRAMES == 1)
#define IAV_MARGIN_SIZE		((19 << 20) + (512 << 10) + IAV_MEM_USR_SIZE \
	+ IAV_PIC_STAT_RSVD_SIZE + IAV_MEM_MV_SIZE * IAV_MV_STREAM_NUM + IAV_MASK_RSVD_SIZE)
#elif (IAV_ROI_NUM_FOR_IPB_FRAMES == 2)
#define IAV_MARGIN_SIZE		((24 << 20) + (512 << 10) + IAV_MEM_USR_SIZE \
	+ IAV_PIC_STAT_RSVD_SIZE + IAV_MEM_MV_SIZE * IAV_MV_STREAM_NUM + IAV_MASK_RSVD_SIZE)
#elif (IAV_ROI_NUM_FOR_IPB_FRAMES == 3)
#define IAV_MARGIN_SIZE		((29 << 20) + (512 << 10) + IAV_MEM_USR_SIZE \
	+ IAV_PIC_STAT_RSVD_SIZE + IAV_MEM_MV_SIZE * IAV_MV_STREAM_NUM + IAV_MASK_RSVD_SIZE)
#elif (IAV_ROI_NUM_FOR_IPB_FRAMES == 0)
#define IAV_MARGIN_SIZE		((16 << 20) + (512 << 10) + IAV_MEM_USR_SIZE \
	+ IAV_PIC_STAT_RSVD_SIZE + IAV_MEM_MV_SIZE * IAV_MV_STREAM_NUM + IAV_MASK_RSVD_SIZE)
#endif

#endif

#else

#define IAV_MARGIN_SIZE		0

#endif

#ifndef CONFIG_AMBARELLA_IAV_DRAM_DECODE_ONLY
#define IK_BUF_SIZE				(IAV_MEM_IMG_IK_CFG_SIZE)
#define DSP_SBP_SIZE			(IAV_MEM_IMG_SBP_SIZE)
#define DSP_NN_BF_SIZE			(IAV_MEM_IMG_NN_BF_SIZE)
#define IAV_IMGRSVD_SIZE		(1 << 20)

/**
* IAV_PART_IMG Layout:
*       +--------------------+
*       | IK_BUF_SIZE        |
*       +--------------------+
*       | DSP_SBP_SIZE       |
*       +--------------------+
*       | DSP_NN_BF_SIZE     |
*       +--------------------+
*       | IAV_IMGRSVD_SIZE   |
*       +--------------------+
*/
#define DSP_IMG_SIZE			(IK_BUF_SIZE + DSP_SBP_SIZE + DSP_NN_BF_SIZE)
#define TOTAL_IMG_SIZE			(DSP_IMG_SIZE + IAV_IMGRSVD_SIZE)

#ifndef DSP_IAVRSVD_SIZE
#define DSP_IAVRSVD_SIZE	0x0A00000
#endif

#if (DSP_IAVRSVD_SIZE < (TOTAL_IMG_SIZE + IAV_MARGIN_SIZE))
#undef DSP_IAVRSVD_SIZE
#define DSP_IAVRSVD_SIZE	(TOTAL_IMG_SIZE + IAV_MARGIN_SIZE)
//#  error "Reserved IAV driver memory size must be larger than 16MB."
#endif

#else

#define TOTAL_IMG_SIZE		0
#undef DSP_IAVRSVD_SIZE
#define DSP_IAVRSVD_SIZE		0

#endif

#define DSP_UCODE_SIZE			(5 << 20)
#ifdef CONFIG_AMBARELLA_DSP_LOG_SIZE
#define DSP_LOG_SIZE			CONFIG_AMBARELLA_DSP_LOG_SIZE
#else
#define DSP_LOG_SIZE			(512 << 10)
#endif
#define DSP_BSH_SIZE			(16 << 10)

/* MSG size is 128 bytes, total is 128 MSG + STATUS MSG for each port.
 * For fixed MSG buffer, two ports size: 128 * 32 * 2 = 8 KB
 * For ring MSG buffer, two ports size: 128 * 129 * 2 = 32 KB + 256 B
 * The default way for MSG is using ring buffer. If want to use fixed
 * buffer, please change "DSP_PORT_MSG_LOG2_ITEM_NUM" to 0.
 */
#define DSP_PORT_FIXED_MSG_BUF_SIZE		(4 << 10)
#define DSP_PORT_MSG_LOG2_ITEM_NUM	(7)
#define DSP_PORT_MSG_ITEM_NUM		(1U << DSP_PORT_MSG_LOG2_ITEM_NUM)
#define DSP_PORT_MSG_ITEM_NUM_THRESHOLD	(DSP_PORT_MSG_ITEM_NUM - 4)
#define DSP_PORT_RING_MSG_BUF_SIZE		((DSP_PORT_MSG_ITEM_NUM + 1) * 128)
#if (DSP_PORT_MSG_LOG2_ITEM_NUM > 0)
#define DSP_PORT_MSG_SIZE		DSP_PORT_RING_MSG_BUF_SIZE
#else
#define DSP_PORT_MSG_SIZE		DSP_PORT_FIXED_MSG_BUF_SIZE
#endif
#define DSP_MSG_BUF_SIZE		(DSP_PORT_MSG_SIZE << 1)

/* CMD size is 128 bytes, total is 31 CMD + header for each port.
 * For two ports: 128 * 32 * 2 = 8 KB */
#define DSP_PORT_CMD_SIZE		(4 << 10)
#define DSP_CMD_BUF_SIZE		(DSP_PORT_CMD_SIZE * 2)

/* Default CMD size is 128 bytes, total is 31 CMD + header for each port. */
#define DSP_DEF_CMD_BUF_SIZE		(DSP_PORT_CMD_SIZE)

#ifndef DSP_FASTAUDIO_SIZE
#define DSP_FASTAUDIO_SIZE	0
#endif

#ifndef DSP_FASTDATA_SIZE
#define DSP_FASTDATA_SIZE	0
#endif

#if defined(CONFIG_ARCH_CV22) || defined(CONFIG_ARCH_CV2) || defined(CONFIG_ARCH_CV25) || defined (CONFIG_ARCH_CV28)
#if ((CV_RAM_START - IDSP_RAM_START) < ((64 << 20) + (DSP_IAVRSVD_SIZE)))
#error "Too small RAM for DSP."
#endif
#define DSP_DRAM_SIZE		((CV_RAM_START - IDSP_RAM_START) - \
						DSP_UCODE_SIZE - \
						DSP_LOG_SIZE - \
						DSP_BSH_SIZE - \
						DSP_MSG_BUF_SIZE - \
						DSP_CMD_BUF_SIZE - \
						DSP_DEF_CMD_BUF_SIZE - \
						DSP_FASTDATA_SIZE - \
						DSP_FASTAUDIO_SIZE - \
						DSP_IAVRSVD_SIZE - \
						DSP_INT_BSB_SIZE - \
						DSP_BSB_SIZE)
#else
#define DSP_DRAM_SIZE		(IAV_MEM_DRAM_SIZE -\
						DSP_UCODE_SIZE - \
						DSP_LOG_SIZE - \
						DSP_BSH_SIZE - \
						DSP_MSG_BUF_SIZE - \
						DSP_CMD_BUF_SIZE - \
						DSP_DEF_CMD_BUF_SIZE - \
						DSP_FASTDATA_SIZE - \
						DSP_FASTAUDIO_SIZE - \
						DSP_IAVRSVD_SIZE - \
						DSP_INT_BSB_SIZE - \
						DSP_BSB_SIZE)
#endif

#define DSP_BSB_START			(IDSP_RAM_START)
#define DSP_INT_BSB_START		(DSP_BSB_START + DSP_BSB_SIZE)
#define DSP_IAVRSVD_START		(DSP_INT_BSB_START + DSP_INT_BSB_SIZE)
#define DSP_FASTDATA_START		(DSP_IAVRSVD_START + DSP_IAVRSVD_SIZE)
#define DSP_FASTAUDIO_START		(DSP_FASTDATA_START + DSP_FASTDATA_SIZE)
#define DSP_DRAM_START			(DSP_FASTAUDIO_START + DSP_FASTAUDIO_SIZE)
#define DSP_DEF_CMD_BUF_START	(DSP_DRAM_START + DSP_DRAM_SIZE)
#define DSP_CMD_BUF_START		(DSP_DEF_CMD_BUF_START + DSP_DEF_CMD_BUF_SIZE)
#define DSP_MSG_BUF_START		(DSP_CMD_BUF_START + DSP_CMD_BUF_SIZE)
#define DSP_BSH_START			(DSP_MSG_BUF_START + DSP_MSG_BUF_SIZE)
#define DSP_LOG_START			(DSP_BSH_START + DSP_BSH_SIZE)
#define DSP_UCODE_START			(DSP_LOG_START + DSP_LOG_SIZE)

#ifdef CONFIG_DSP_LOG_START_IAVMEM
#define DSP_LOG_AREA_PHYS		(DSP_LOG_START)
#endif	// CONFIG_DSP_LOG_START_IAVMEM

/*
 * layout for ucode in memory:
 *
 *       +----------------------+ <--- DSP_UCODE_START
 *       | ORCCODE       (3 MB) |
 *       +----------------------+
 *       | ORCME       (640 KB) |
 *       +----------------------+
 *       | ORCMDXF     (256 KB) |
 *       +----------------------+
 *       | DEFAULT BIN (512 KB) |
 *       +----------------------+ <--- Optional for fast boot
 *       | DEFAULT MCTF (16 KB) |
 *       +----------------------+ <--- Chip ID
 *       | CHIP ID        (4 B) |
 *       +----------------------+
 *       | RESERVED       (4 B) |
 *       +----------------------+ <--- dsp init data
 *       | DSP INIT DATA (128 B)|
 *       +----------------------+
 *       | DSP ASSERT MSG (16 B)|
 *       +----------------------+ <--- unique id
 *       | UNIQUE ID (32 B)     |
 *       +----------------------+ <--- wafer id
 *       | WAFER ID (8 B)       |
 *       +----------------------+
 */

#define DSP_CODE_SIZE						(3 * MB)
#define DSP_ME_SIZE							(640 * KB)
#define DSP_MDXF_SIZE						(256 * KB)
#define DSP_BINARY_SIZE						(512 * KB)
#define DSP_MCTF_SIZE						(16 * KB)
#define DSP_INIT_DATA_SIZE					(128)
#define DSP_ASSERT_MSG_SIZE				(16)
#define DSP_UNIQUE_ID_SIZE					(32)
#define DSP_WAFER_ID_SIZE					(8)

/* based on DSP_UCODE_START */
#define DSP_CODE_MEMORY_OFFSET				(0)
#define DSP_CODE_SILICON_VERSION_OFFSET		(DSP_CODE_MEMORY_OFFSET + 0x64)
#define UCODE_DSP_INIT_DATA_PTR_OFFSET		(DSP_CODE_MEMORY_OFFSET + 0x68)
#define DSP_ME_MEMORY_OFFSET				(DSP_CODE_MEMORY_OFFSET + DSP_CODE_SIZE)
#define DSP_MDXF_MEMORY_OFFSET				(DSP_ME_MEMORY_OFFSET + DSP_ME_SIZE)
#define DSP_BINARY_DATA_MEMORY_OFFSET		(DSP_MDXF_MEMORY_OFFSET + DSP_MDXF_SIZE)
#define DSP_MCTF_OFFSET						(DSP_BINARY_DATA_MEMORY_OFFSET + DSP_BINARY_SIZE)
#define DSP_CHIP_ID_OFFSET					(DSP_MCTF_OFFSET + DSP_MCTF_SIZE)
#define DSP_INIT_DATA_OFFSET				(DSP_CHIP_ID_OFFSET + 4 + 4)
#define DSP_ASSERT_MSG_OFFSET				(DSP_INIT_DATA_OFFSET + DSP_INIT_DATA_SIZE)
#define DSP_UNIQUE_ID_OFFSET				(DSP_ASSERT_MSG_OFFSET + DSP_ASSERT_MSG_SIZE)
#define DSP_WAFER_ID_OFFSET					(DSP_UNIQUE_ID_OFFSET + DSP_UNIQUE_ID_SIZE)

#define DSP_DRAM_CODE_START					(DSP_UCODE_START + DSP_CODE_MEMORY_OFFSET)
#define UCODE_DSP_INIT_DATA_PTR				(DSP_UCODE_START + UCODE_DSP_INIT_DATA_PTR_OFFSET)
#define DSP_DRAM_ME_START					(DSP_UCODE_START + DSP_ME_MEMORY_OFFSET)
#define DSP_DRAM_MDXF_START					(DSP_UCODE_START + DSP_MDXF_MEMORY_OFFSET)
#define DSP_BINARY_DATA_START				(DSP_UCODE_START + DSP_BINARY_DATA_MEMORY_OFFSET)
#define DSP_CHIP_ID_START					(DSP_UCODE_START + DSP_CHIP_ID_OFFSET)
#define DSP_INIT_DATA_START					(DSP_UCODE_START + DSP_INIT_DATA_OFFSET)
#define DSP_ASSERT_MSG_START				(DSP_UCODE_START + DSP_ASSERT_MSG_OFFSET)
#define DSP_UNIQUE_ID_START				(DSP_UCODE_START + DSP_UNIQUE_ID_OFFSET)
#define DSP_WAFER_ID_START					(DSP_UCODE_START + DSP_WAFER_ID_OFFSET)

#define DSP_IDSP_BASE_OFFSET				(0x11801C) // based on DSP_DRAM_START
#define DSP_IDSP_BASE						(DSP_DRAM_START + DSP_IDSP_BASE_OFFSET)
//#define DSP_ORC_BASE						(get_ambarella_apb_virt() + 0x11FF00)

/*
 * layout for DSP_FASTDATA in memory:
 *
 *       +-------------------------+ <--- DSP_FASTDATA_START
 *       | DSP_STATUS       (4 B)  |
 *       +-------------------------+ <--- vin_video_format structure
 *       | VIN_VIDEO_FORMAT (128 B)|
 *       +-------------------------+ <--- vin_dsp_config structure
 *       | VIN_DSP_CONFIG  (128 B) |
 *       +-------------------------+ <--- DSP_ENC_CFG structure * IPCAM_RECORD_MAX_NUM_ENC
 *       | ENC_CONFIG_DRAM (16*8 B)|
 *       +-------------------------+
 */

/* store DSP_FASTDATA in amboot, restore it after enter Linux IAV */
#define DSP_STATUS_STORE_SIZE				(4)
#define DSP_VIN_VIDEO_FORMAT_STORE_SIZE		(128)
#define DSP_VIN_CONFIG_STORE_SIZE			(128)

#define DSP_STATUS_STORE_OFFSET				(0)
#define DSP_VIN_VIDEO_FORMAT_STORE_OFFSET	(DSP_STATUS_STORE_OFFSET + DSP_STATUS_STORE_SIZE)
#define DSP_VIN_CONFIG_STORE_OFFSET			(DSP_VIN_VIDEO_FORMAT_STORE_OFFSET + DSP_VIN_VIDEO_FORMAT_STORE_SIZE)
#define DSP_ENC_CFG_OFFSET					(DSP_VIN_CONFIG_STORE_OFFSET + DSP_VIN_CONFIG_STORE_SIZE)

#define DSP_STATUS_STORE_START				(DSP_FASTDATA_START + DSP_STATUS_STORE_OFFSET)
#define DSP_VIN_VIDEO_FORMAT_STORE_START	(DSP_FASTDATA_START + DSP_VIN_VIDEO_FORMAT_STORE_OFFSET)
#define DSP_VIN_CONFIG_STORE_START			(DSP_FASTDATA_START + DSP_VIN_CONFIG_STORE_OFFSET)
#define DSP_ENC_CFG_START					(DSP_FASTDATA_START + DSP_ENC_CFG_OFFSET)
#define DSP_FASTDATA_INVALID				(0xFF)


/* layout of IAV_PART_DSP_RSV */
#define DSP_RSV_BUF_SIZE					(DSP_CMD_BUF_SIZE + DSP_MSG_BUF_SIZE)

#define DSP_CMD_BUF_OFFSET					(0)
#define DSP_MSG_BUF_OFFSET					(DSP_CMD_BUF_SIZE)

#endif	// __AMBA_ARCH_MEM_H__

