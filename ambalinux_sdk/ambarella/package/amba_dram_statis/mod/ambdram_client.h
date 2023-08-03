/*
 * kernel/private/drivers/ambdram_statis/ambdram_client.h
 *
 * History:
 *	2020/06/08 - [Bingliang Hu]
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

#ifndef _AMBARELLA_DRAM_CLIENT_H
#define _AMBARELLA_DRAM_CLIENT_H

#define ARCH_PLAT_VER_5 (5)
#define ARCH_PLAT_VER_6 (6)
#define ARCH_PLAT_VER_7 (7)

#define AMBA_DRAM_STAT_NEW_FORMAT

#if defined(AMBA_DRAM_STAT_NEW_FORMAT)
enum {
	DRAM_CLIENT_GROUP_CORTEX	= 0,
	DRAM_CLIENT_GROUP_DSP,
	DRAM_CLIENT_GROUP_VPU,
	DRAM_CLIENT_GROUP_GPU,
	DRAM_CLIENT_GROUP_HSM,
	DRAM_CLIENT_GROUP_PCIE,
	DRAM_CLIENT_GROUP_FEX,
	DRAM_CLIENT_GROUP_FMA,
	DRAM_CLIENT_GROUP_PERIPHERAL,
	DRAM_CLIENT_GROUP_OTHER,
	DRAM_CLIENT_GROUP_NUM,
};

const char *G_dram_group_name[] = {
	[DRAM_CLIENT_GROUP_CORTEX]	    = "Cortex",
	[DRAM_CLIENT_GROUP_DSP]		    = "DSP",
	[DRAM_CLIENT_GROUP_VPU]		    = "VPU",
	[DRAM_CLIENT_GROUP_GPU]		    = "GPU",
	[DRAM_CLIENT_GROUP_HSM]		    = "HSM",
	[DRAM_CLIENT_GROUP_PCIE]	    = "PCIE",
	[DRAM_CLIENT_GROUP_FEX]         = "FEX",
	[DRAM_CLIENT_GROUP_FMA]         = "FMA",
	[DRAM_CLIENT_GROUP_PERIPHERAL]  = "PERIPHERAL",
	[DRAM_CLIENT_GROUP_OTHER]	    = "Other",
};
#else
enum {
	DRAM_CLIENT_GROUP_CORTEX	= 0,
	DRAM_CLIENT_GROUP_DSP,
	DRAM_CLIENT_GROUP_VP,
	DRAM_CLIENT_GROUP_GDMA,
	DRAM_CLIENT_GROUP_ENET,
	DRAM_CLIENT_GROUP_OTHER,
	DRAM_CLIENT_GROUP_NUM,
};

const char *G_dram_group_name[] = {
	[DRAM_CLIENT_GROUP_CORTEX]	= "Cortex",
	[DRAM_CLIENT_GROUP_DSP]		= "DSP",
	[DRAM_CLIENT_GROUP_VP]		= "VP",
	[DRAM_CLIENT_GROUP_GDMA]	= "GDMA",
	[DRAM_CLIENT_GROUP_ENET]	= "ENET",
	[DRAM_CLIENT_GROUP_OTHER]	= "Other",
};
#endif

struct dram_client {
	const char *name;
	u32 group;
};


#if defined(ambarella_soc_cv2)

#define ARCH_PLAT_VER	(ARCH_PLAT_VER_5)
struct dram_client G_dram_client[] = {
	{"AXI0",	DRAM_CLIENT_GROUP_CORTEX},
	{"ARM_DMA0",	DRAM_CLIENT_GROUP_OTHER},
	{"ARM_DMA1",	DRAM_CLIENT_GROUP_OTHER},
	{"ENET",	DRAM_CLIENT_GROUP_ENET},
	{"FDMA",	DRAM_CLIENT_GROUP_OTHER},
	{"CANC0",	DRAM_CLIENT_GROUP_OTHER},
	{"CANC1",	DRAM_CLIENT_GROUP_OTHER},
	{"GDMA",	DRAM_CLIENT_GROUP_GDMA},
	{"SDXC0",	DRAM_CLIENT_GROUP_OTHER},
	{"SDXC1",	DRAM_CLIENT_GROUP_OTHER},
	{"USB20(DEV)",	DRAM_CLIENT_GROUP_OTHER},
	{"USB20(HOST)",	DRAM_CLIENT_GROUP_OTHER},
	{"ORCME",	DRAM_CLIENT_GROUP_DSP},
	{"ORCCODE",	DRAM_CLIENT_GROUP_DSP},
	{"ORCVP",	DRAM_CLIENT_GROUP_VP},
	{"ORCL2CACHE",	DRAM_CLIENT_GROUP_VP},
	{"SMEM",	DRAM_CLIENT_GROUP_DSP},
	{"VMEM0",	DRAM_CLIENT_GROUP_VP},
	{"FEX",		DRAM_CLIENT_GROUP_VP},
	{"BMEM",	DRAM_CLIENT_GROUP_VP},
};

#elif defined(ambarella_soc_cv22)

#define ARCH_PLAT_VER	(ARCH_PLAT_VER_5)
struct dram_client G_dram_client[] = {
	{"AXI0",	DRAM_CLIENT_GROUP_CORTEX},
	{"ARM_DMA0",	DRAM_CLIENT_GROUP_OTHER},
	{"ARM_DMA1",	DRAM_CLIENT_GROUP_OTHER},
	{"ENET",	DRAM_CLIENT_GROUP_ENET},
	{"FDMA",	DRAM_CLIENT_GROUP_OTHER},
	{"CANC0",	DRAM_CLIENT_GROUP_OTHER},
	{"GDMA",	DRAM_CLIENT_GROUP_GDMA},
	{"SDXC0",	DRAM_CLIENT_GROUP_OTHER},
	{"SDXC1",	DRAM_CLIENT_GROUP_OTHER},
	{"USB20(DEV)",	DRAM_CLIENT_GROUP_OTHER},
	{"USB20(HOST)",	DRAM_CLIENT_GROUP_OTHER},
	{"ORCME",	DRAM_CLIENT_GROUP_DSP},
	{"ORCCODE",	DRAM_CLIENT_GROUP_DSP},
	{"ORCVP",	DRAM_CLIENT_GROUP_VP},
	{"ORCL2CACHE",	DRAM_CLIENT_GROUP_VP},
	{"SMEM",	DRAM_CLIENT_GROUP_DSP},
	{"VMEM0",	DRAM_CLIENT_GROUP_VP},
};

#elif defined(ambarella_soc_cv25) || defined(ambarella_soc_cv28)

#define ARCH_PLAT_VER	(ARCH_PLAT_VER_5)
struct dram_client G_dram_client[] = {
	{"AXI0",	DRAM_CLIENT_GROUP_CORTEX},
	{"ARM_DMA0",	DRAM_CLIENT_GROUP_OTHER},
	{"ARM_DMA1",	DRAM_CLIENT_GROUP_OTHER},
	{"ENET",	DRAM_CLIENT_GROUP_ENET},
	{"FDMA",	DRAM_CLIENT_GROUP_OTHER},
	{"CANC0",	DRAM_CLIENT_GROUP_OTHER},
	{"GDMA",	DRAM_CLIENT_GROUP_GDMA},
	{"SDXC0",	DRAM_CLIENT_GROUP_OTHER},
	{"SDXC1",	DRAM_CLIENT_GROUP_OTHER},
	{"SDXC2",	DRAM_CLIENT_GROUP_OTHER},
	{"USB20(DEV)",	DRAM_CLIENT_GROUP_OTHER},
	{"USB20(HOST)",	DRAM_CLIENT_GROUP_OTHER},
	{"ORCME",	DRAM_CLIENT_GROUP_DSP},
	{"ORCCODE",	DRAM_CLIENT_GROUP_DSP},
	{"ORCVP",	DRAM_CLIENT_GROUP_VP},
	{"ORCL2CACHE",	DRAM_CLIENT_GROUP_VP},
	{"SMEM",	DRAM_CLIENT_GROUP_DSP},
	{"VMEM0",	DRAM_CLIENT_GROUP_VP},
};

#elif defined(ambarella_soc_s6lm)

#define ARCH_PLAT_VER	(ARCH_PLAT_VER_5)
struct dram_client G_dram_client[] = {
	{"AXI0",	DRAM_CLIENT_GROUP_CORTEX},
	{"ARM_DMA0",	DRAM_CLIENT_GROUP_OTHER},
	{"ARM_DMA1",	DRAM_CLIENT_GROUP_OTHER},
	{"ENET",	DRAM_CLIENT_GROUP_ENET},
	{"FDMA",	DRAM_CLIENT_GROUP_OTHER},
	{"GDMA",	DRAM_CLIENT_GROUP_GDMA},
	{"SDXC0",	DRAM_CLIENT_GROUP_OTHER},
	{"SDXC1",	DRAM_CLIENT_GROUP_OTHER},
	{"SDXC2",	DRAM_CLIENT_GROUP_OTHER},
	{"USB20(DEV)",	DRAM_CLIENT_GROUP_OTHER},
	{"USB20(HOST)",	DRAM_CLIENT_GROUP_OTHER},
	{"ORCME",	DRAM_CLIENT_GROUP_DSP},
	{"ORCCODE",	DRAM_CLIENT_GROUP_DSP},
	{"SMEM",	DRAM_CLIENT_GROUP_DSP},
};

#elif defined(ambarella_soc_cv5)

#define ARCH_PLAT_VER	(ARCH_PLAT_VER_6)
struct dram_client G_dram_client[] = {
	{"AXI0",	DRAM_CLIENT_GROUP_CORTEX},
	{"AXI1",	DRAM_CLIENT_GROUP_CORTEX},
	{"L2CACHE",	DRAM_CLIENT_GROUP_VP},
	{"USB3",	DRAM_CLIENT_GROUP_OTHER},
	{"PCIE",	DRAM_CLIENT_GROUP_OTHER},
	{"ENET0",	DRAM_CLIENT_GROUP_ENET},
	{"ENET1",	DRAM_CLIENT_GROUP_ENET},
	{"FDMA",	DRAM_CLIENT_GROUP_OTHER},
	{"SDAXI0",	DRAM_CLIENT_GROUP_OTHER},
	{"SDAXI1",	DRAM_CLIENT_GROUP_OTHER},
	{"SDAHB0",	DRAM_CLIENT_GROUP_OTHER},
	{"USB(DEV)",	DRAM_CLIENT_GROUP_OTHER},
	{"ARM_DMA0",	DRAM_CLIENT_GROUP_OTHER},
	{"ARM_DMA1",	DRAM_CLIENT_GROUP_OTHER},
	{"CANC0",	DRAM_CLIENT_GROUP_OTHER},
	{"GDMA",	DRAM_CLIENT_GROUP_GDMA},
	{"ORCME0",	DRAM_CLIENT_GROUP_DSP},
	{"ORCME1",	DRAM_CLIENT_GROUP_DSP},
	{"ORCCODE0",	DRAM_CLIENT_GROUP_DSP},
	{"ORCCODE1",	DRAM_CLIENT_GROUP_DSP},
	{"ORCVP",	DRAM_CLIENT_GROUP_VP},
	{"SMEM_WR",	DRAM_CLIENT_GROUP_DSP},
	{"SMEM_RD",	DRAM_CLIENT_GROUP_DSP},
	{"VMEM0",	DRAM_CLIENT_GROUP_VP},
	{"DBSE",	DRAM_CLIENT_GROUP_VP},
};

#elif defined(ambarella_soc_cv3)

#define ARCH_PLAT_VER	(ARCH_PLAT_VER_7)
#if defined(AMBA_DRAM_STAT_NEW_FORMAT)
struct dram_client G_dram_client[] = {
	{"CORTEX0",	            DRAM_CLIENT_GROUP_CORTEX},
	{"CORTEX1",	            DRAM_CLIENT_GROUP_CORTEX},
	{"CORTEX2",	            DRAM_CLIENT_GROUP_CORTEX},
	{"CORTEX3",	            DRAM_CLIENT_GROUP_CORTEX},
	{"USB3H0",	            DRAM_CLIENT_GROUP_OTHER},
	{"PCIE0",	            DRAM_CLIENT_GROUP_PCIE},
	{"PCIE1_pcierc",        DRAM_CLIENT_GROUP_PCIE},
	{"PCIE1_piceep",        DRAM_CLIENT_GROUP_PCIE},
	{"GPU",		            DRAM_CLIENT_GROUP_GPU},
	{"ENET_enet0",	        DRAM_CLIENT_GROUP_OTHER},
	{"ENET_enet1",	        DRAM_CLIENT_GROUP_OTHER},
	{"ENET_enet2",	        DRAM_CLIENT_GROUP_OTHER},
	{"ENET_enet3",	        DRAM_CLIENT_GROUP_OTHER},
	{"ENET_usb2d",	        DRAM_CLIENT_GROUP_OTHER},
	{"ENET_fdma",	        DRAM_CLIENT_GROUP_OTHER},
	{"PERIPHLS0_sdxc0",	    DRAM_CLIENT_GROUP_PERIPHERAL},
	{"PERIPHLS0_sdxc1",	    DRAM_CLIENT_GROUP_PERIPHERAL},
	{"PERIPHLS0_sdxc2",	    DRAM_CLIENT_GROUP_PERIPHERAL},
	{"PERIPHLS1_armdma0",	DRAM_CLIENT_GROUP_PERIPHERAL},
	{"PERIPHLS1_armdma1",	DRAM_CLIENT_GROUP_PERIPHERAL},
	{"PERIPHLS1_armdma2",	DRAM_CLIENT_GROUP_PERIPHERAL},
	{"PERIPHLS1_canc0",	    DRAM_CLIENT_GROUP_PERIPHERAL},
	{"PERIPHLS1_canc1",	    DRAM_CLIENT_GROUP_PERIPHERAL},
	{"GDMA_gdma0",	        DRAM_CLIENT_GROUP_OTHER},
	{"GDMA_gdma1",	        DRAM_CLIENT_GROUP_OTHER},
	{"GDMA_gdma2",	        DRAM_CLIENT_GROUP_OTHER},
	{"GDMA_gdma3",	        DRAM_CLIENT_GROUP_OTHER},
	{"GDMA_dbse",	        DRAM_CLIENT_GROUP_OTHER},
	{"ORCME0_eorc0",	    DRAM_CLIENT_GROUP_DSP},
	{"ORCME0_dorc0",	    DRAM_CLIENT_GROUP_DSP},
	{"ORCME1_eorc1",        DRAM_CLIENT_GROUP_DSP},
	{"ORCME1_dorc1",        DRAM_CLIENT_GROUP_DSP},
	{"ORCCODE_corc",	    DRAM_CLIENT_GROUP_DSP},
	{"ORCCODE_iorcv0",	    DRAM_CLIENT_GROUP_DSP},
	{"ORCCODE_iorcv1",	    DRAM_CLIENT_GROUP_DSP},
	{"ORCCODE_iorc0",	    DRAM_CLIENT_GROUP_DSP},
	{"ORCCODE_iorc1",	    DRAM_CLIENT_GROUP_DSP},
	{"NVP0MAXI_nvp0cmem",	DRAM_CLIENT_GROUP_VPU},
	{"NVP0MAXI_nvp0l2c",	DRAM_CLIENT_GROUP_VPU},
	{"NVP0VMEM",	        DRAM_CLIENT_GROUP_VPU},
	{"NVP1MAXI_nvp1cmem",	DRAM_CLIENT_GROUP_VPU},
	{"NVP1MAXI_nvp1l2c",	DRAM_CLIENT_GROUP_VPU},
	{"NVP1VMEM",	        DRAM_CLIENT_GROUP_VPU},
	{"NVP2MAXI_nvp2cmem",	DRAM_CLIENT_GROUP_VPU},
	{"NVP2MAXI_nvp2l2c",	DRAM_CLIENT_GROUP_VPU},
	{"NVP2VMEM",	        DRAM_CLIENT_GROUP_VPU},
	{"NVP3MAXI_nvp3cmem",	DRAM_CLIENT_GROUP_VPU},
	{"NVP3MAXI_nvp3l2c",	DRAM_CLIENT_GROUP_VPU},
	{"NVP3VMEM",	        DRAM_CLIENT_GROUP_VPU},
	{"NVP4MAXI_nvp4cmem",	DRAM_CLIENT_GROUP_VPU},
	{"NVP4MAXI_nvp4l2c",	DRAM_CLIENT_GROUP_VPU},
	{"NVP4VMEM",	        DRAM_CLIENT_GROUP_VPU},
	{"NVP5MAXI_nvp5cmem",	DRAM_CLIENT_GROUP_VPU},
	{"NVP5MAXI_nvp5l2c",	DRAM_CLIENT_GROUP_VPU},
	{"NVP5VMEM",	        DRAM_CLIENT_GROUP_VPU},
	{"GVP0MAXI_gvp0cmem",	DRAM_CLIENT_GROUP_VPU},
	{"GVP0MAXI_gvp0l2c",	DRAM_CLIENT_GROUP_VPU},
	{"GVP0VMEM",	        DRAM_CLIENT_GROUP_VPU},
	{"GVP1MAXI_gvp1cmem",	DRAM_CLIENT_GROUP_VPU},
	{"GVP1MAXI_gvp1l2c",	DRAM_CLIENT_GROUP_VPU},
	{"GVP1VMEM",	        DRAM_CLIENT_GROUP_VPU},
	{"FEXMAXI_fex0cmem",	DRAM_CLIENT_GROUP_FEX},
	{"FEXMAXI_fex1cmem",	DRAM_CLIENT_GROUP_FEX},
	{"FEXMAXI_fex0l2c",	    DRAM_CLIENT_GROUP_FEX},
	{"FEXMAXI_fex1l2c",	    DRAM_CLIENT_GROUP_FEX},
	{"FEXDMA_fex0dma",	    DRAM_CLIENT_GROUP_FEX},
	{"FEXDMA_fex1dma",	    DRAM_CLIENT_GROUP_FEX},
	{"FMAMAXI_fma0cmem",	DRAM_CLIENT_GROUP_FMA},
	{"FMAMAXI_fma1cmem",	DRAM_CLIENT_GROUP_FMA},
	{"FMAMAXI_fma0l2c",	    DRAM_CLIENT_GROUP_FMA},
	{"FMAMAXI_fma1l2c",	    DRAM_CLIENT_GROUP_FMA},
	{"FMABMEM_fma0bmem",	DRAM_CLIENT_GROUP_FMA},
	{"FMABMEM_fma1bmem",	DRAM_CLIENT_GROUP_FMA},
	{"SWMAXI_swcmem",	    DRAM_CLIENT_GROUP_OTHER},
	{"SWMAXI_swcl2c",	    DRAM_CLIENT_GROUP_OTHER},
	{"HSM_hsml2c",		    DRAM_CLIENT_GROUP_HSM},
	{"HSM_hsmdma",		    DRAM_CLIENT_GROUP_HSM},
	{"HSM_hsmcmem",		    DRAM_CLIENT_GROUP_HSM},
	{"HSM_hsmsmc",		    DRAM_CLIENT_GROUP_HSM},
	{"SMEM_WR",	            DRAM_CLIENT_GROUP_DSP},
	{"SMEM_RD",	            DRAM_CLIENT_GROUP_DSP},
};
#else
struct dram_client G_dram_client[] = {
	{"AXI0",	            DRAM_CLIENT_GROUP_CORTEX},
	{"AXI1",	            DRAM_CLIENT_GROUP_CORTEX},
	{"AXI2",	            DRAM_CLIENT_GROUP_CORTEX},
	{"AXI3",	            DRAM_CLIENT_GROUP_CORTEX},
	{"USB3",	            DRAM_CLIENT_GROUP_OTHER},
	{"PCIE0",	            DRAM_CLIENT_GROUP_OTHER},
	{"PCIE1_pcierc",        DRAM_CLIENT_GROUP_OTHER},
	{"PCIE1_piceep",        DRAM_CLIENT_GROUP_OTHER},
	{"GPU",		            DRAM_CLIENT_GROUP_OTHER},
	{"ENET_enet0",	        DRAM_CLIENT_GROUP_ENET},
	{"ENET_enet1",	        DRAM_CLIENT_GROUP_ENET},
	{"ENET_enet2",	        DRAM_CLIENT_GROUP_ENET},
	{"ENET_enet3",	        DRAM_CLIENT_GROUP_ENET},
	{"ENET_usb2d",	        DRAM_CLIENT_GROUP_ENET},
	{"ENET_fdma",	        DRAM_CLIENT_GROUP_ENET},
	{"PERIPHLS0_sdxc0",	    DRAM_CLIENT_GROUP_OTHER},
	{"PERIPHLS0_sdxc1",	    DRAM_CLIENT_GROUP_OTHER},
	{"PERIPHLS0_sdxc2",	    DRAM_CLIENT_GROUP_OTHER},
	{"PERIPHLS1_armdma0",	DRAM_CLIENT_GROUP_OTHER},
	{"PERIPHLS1_armdma1",	DRAM_CLIENT_GROUP_OTHER},
	{"PERIPHLS1_armdma2",	DRAM_CLIENT_GROUP_OTHER},
	{"PERIPHLS1_canc0",	    DRAM_CLIENT_GROUP_OTHER},
	{"PERIPHLS1_canc1",	    DRAM_CLIENT_GROUP_OTHER},
	{"GDMA_gdma0",	        DRAM_CLIENT_GROUP_GDMA},
	{"GDMA_gdma1",	        DRAM_CLIENT_GROUP_GDMA},
	{"GDMA_gdma2",	        DRAM_CLIENT_GROUP_GDMA},
	{"GDMA_gdma3",	        DRAM_CLIENT_GROUP_GDMA},
	{"GDMA_dbse",	        DRAM_CLIENT_GROUP_GDMA},
	{"ORCME0_eorc0",	    DRAM_CLIENT_GROUP_DSP},
	{"ORCME0_dorc0",	    DRAM_CLIENT_GROUP_DSP},
	{"ORCME1_eorc1",        DRAM_CLIENT_GROUP_DSP},
	{"ORCME1_dorc1",        DRAM_CLIENT_GROUP_DSP},
	{"ORCCODE_corc",	    DRAM_CLIENT_GROUP_DSP},
	{"ORCCODE_iorcv0",	    DRAM_CLIENT_GROUP_DSP},
	{"ORCCODE_iorcv1",	    DRAM_CLIENT_GROUP_DSP},
	{"ORCCODE_iorc0",	    DRAM_CLIENT_GROUP_DSP},
	{"ORCCODE_iorc1",	    DRAM_CLIENT_GROUP_DSP},
	{"NVP0MAXI_nvp0cmem",	DRAM_CLIENT_GROUP_VP},
	{"NVP0MAXI_nvp0l2c",	DRAM_CLIENT_GROUP_VP},
	{"NVP0VMEM",	        DRAM_CLIENT_GROUP_VP},
	{"NVP1MAXI_nvp1cmem",	DRAM_CLIENT_GROUP_VP},
	{"NVP1MAXI_nvp1l2c",	DRAM_CLIENT_GROUP_VP},
	{"NVP1VMEM",	        DRAM_CLIENT_GROUP_VP},
	{"NVP2MAXI_nvp2cmem",	DRAM_CLIENT_GROUP_VP},
	{"NVP2MAXI_nvp2l2c",	DRAM_CLIENT_GROUP_VP},
	{"NVP2VMEM",	        DRAM_CLIENT_GROUP_VP},
	{"NVP3MAXI_nvp3cmem",	DRAM_CLIENT_GROUP_VP},
	{"NVP3MAXI_nvp3l2c",	DRAM_CLIENT_GROUP_VP},
	{"NVP3VMEM",	        DRAM_CLIENT_GROUP_VP},
	{"NVP4MAXI_nvp4cmem",	DRAM_CLIENT_GROUP_VP},
	{"NVP4MAXI_nvp4l2c",	DRAM_CLIENT_GROUP_VP},
	{"NVP4VMEM",	        DRAM_CLIENT_GROUP_VP},
	{"NVP5MAXI_nvp5cmem",	DRAM_CLIENT_GROUP_VP},
	{"NVP5MAXI_nvp5l2c",	DRAM_CLIENT_GROUP_VP},
	{"NVP5VMEM",	        DRAM_CLIENT_GROUP_VP},
	{"GVP0MAXI_gvp0cmem",	DRAM_CLIENT_GROUP_VP},
	{"GVP0MAXI_gvp0l2c",	DRAM_CLIENT_GROUP_VP},
	{"GVP0VMEM",	        DRAM_CLIENT_GROUP_VP},
	{"GVP1MAXI_gvp1cmem",	DRAM_CLIENT_GROUP_VP},
	{"GVP1MAXI_gvp1l2c",	DRAM_CLIENT_GROUP_VP},
	{"GVP1VMEM",	        DRAM_CLIENT_GROUP_VP},
	{"FEXMAXI_fex0cmem",	DRAM_CLIENT_GROUP_VP},
	{"FEXMAXI_fex1cmem",	DRAM_CLIENT_GROUP_VP},
	{"FEXMAXI_fex0l2c",	    DRAM_CLIENT_GROUP_VP},
	{"FEXMAXI_fex1l2c",	    DRAM_CLIENT_GROUP_VP},
	{"FEXDMA_fex0dma",	    DRAM_CLIENT_GROUP_VP},
	{"FEXDMA_fex1dma",	    DRAM_CLIENT_GROUP_VP},
	{"FMAMAXI_fma0cmem",	DRAM_CLIENT_GROUP_VP},
	{"FMAMAXI_fma1cmem",	DRAM_CLIENT_GROUP_VP},
	{"FMAMAXI_fma0l2c",	    DRAM_CLIENT_GROUP_VP},
	{"FMAMAXI_fma1l2c",	    DRAM_CLIENT_GROUP_VP},
	{"FMABMEM_fma0bmem",	DRAM_CLIENT_GROUP_VP},
	{"FMABMEM_fma1bmem",	DRAM_CLIENT_GROUP_VP},
	{"SWMAXI_swcmem",	    DRAM_CLIENT_GROUP_VP},
	{"SWMAXI_swcl2c",	    DRAM_CLIENT_GROUP_VP},
	{"HSM_hsml2c",		    DRAM_CLIENT_GROUP_OTHER},
	{"HSM_hsmdma",		    DRAM_CLIENT_GROUP_OTHER},
	{"HSM_hsmcmem",		    DRAM_CLIENT_GROUP_OTHER},
	{"HSM_hsmsmc",		    DRAM_CLIENT_GROUP_OTHER},
	{"SMEM_WR",	            DRAM_CLIENT_GROUP_DSP},
	{"SMEM_RD",	            DRAM_CLIENT_GROUP_DSP},
};
#endif

#else

#error "Unknown chip"

#endif

#endif //_AMBARELLA_DRAM_CLIENT_H
