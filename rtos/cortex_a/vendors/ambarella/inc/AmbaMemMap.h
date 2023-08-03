/**
 *  @file AmbaMemMap.h
 *
 *  Copyright (c) 2020 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Definitions & Constants for system memory map
 *
 */

#ifndef AMBA_MEM_MAP_H
#define AMBA_MEM_MAP_H

/* Physical address space: 4GB */
/* 0x00000000 to 0xdffdffff: DRAM */
/* |-> 0x00000000 to 0x00003fff: Reserved for system private data */
/* |-> 0x00004000 to 0xdffdffff: Non-reserved */
/* 0xdffe0000 to 0xffffffff: MMIO */

/* Virtual address space: 4GB */
/* 0x00000000 to 0xdffdffff: Non-reserved */
/* 0xdffe0000 to 0xffffffff: Reserved for system */
/* |-> 0xdffe0000 to 0xffdfffff: Reserved for MMIO */
/* |-> 0xfffe0000 to 0xfffe3fff: Reserved for system private data mapping */
/* |-> 0xfffe4000 to 0xfffeffff: Reserved */
/* |-> 0xffff0000 to 0xffffffff: Reserved for exception vectors */

#if defined(CONFIG_ARM64) && (CONFIG_DDR_SIZE > 0xffe00000U)
#define AMBA_DRAM_LAST_ADDR             0x1000000000ULL // Total 64GB, Reserved and Exception are at 0xffffe0000 and 0xfffff0000
#else
#define AMBA_DRAM_LAST_ADDR             0x100000000ULL  // Total 4GB, Reserved and Exception are at 0xfffe0000 and 0xffff0000
#endif

/*
 *  Following definition is the address allocation for reserved system private data.
 */
#define AMBA_DRAM_RESERVED_PHYS_ADDR    0x00000000U
#define AMBA_DRAM_RESERVED_VIRT_ADDR    (AMBA_DRAM_LAST_ADDR - 0x20000U)    //0xfffe0000U
#define AMBA_DRAM_RESERVED_SIZE         0x00004000U

/*
 *  Following definition is the address allocation for exception vectors.
 */
#define AMBA_DRAM_EXCEPTION_VIRT_ADDR   (AMBA_DRAM_LAST_ADDR - 0x10000U)    //0xffff0000U
#define AMBA_DRAM_EXCEPTION_SIZE        0x00010000U

/*
 *  Following definitions are for reserved system memory allocation. All aligned to cache-line size.
 */
#define AMBA_RAM_DDR_RTT_CALIBRATION_OFFSET             0x0000U      /* 256 bytes. Required by DDR SDRAM initialization sequence. */
#define AMBA_RAM_JUMP_TO_KERNEL_START_OFFSET            0x0000U      /* 16 bytes. For the 2nd/3rd/4th core bootup. (overlapped) */
/* 0x0000 to 0x00FF is written by DDR controller at DDR SDRAM initialization stage. */
/* 0x0000 to 0x000F is then filled by software for the first instruction code of all the other processors. */
#define AMBA_RAM_MAGIC_CODE_OFFSET                      0x0100U      /* 32 to 256 bytes. Two magic code and boot parameters */
#define AMBA_RAM_BOOTSTRAP_OFFSET                       0x0200U      /* 4096 bytes. A copy from SRAM. */
/* 0x1200 to 0x13FF is currently reserved as guard band. */
#define AMBA_RAM_NVM_SYS_PARTITION_TABLE_OFFSET         0x1400U      /* 2048 bytes. Aligned to 512-byte boundary for NAND. */
#define AMBA_RAM_NVM_USER_PARTITION_TABLE_OFFSET        0x1c00U      /* 2048 bytes. Aligned to 512-byte boundary for NAND. */
#define AMBA_RAM_NVM_PRIMARY_BAD_BLOCK_TABLE_OFFSET     0x2400U      /* 2048 bytes. Aligned to 512-byte boundary for NAND. */
#define AMBA_RAM_APPLICATION_SPECIFIC_OFFSET            0x2c00U      /* 1024 bytes. Reserved for boot time profiling and multi-purpose application. */
/* 0x3000 to 0x3FFF is currently reserved. */

/*
 *  Following definitions are for Magic Codes
 */
#define AMBA_MAGIC_CODE_SIZE                            0x10U
#define AMBA_RAM_MAGIC_CODE_VIRT_ADDR                   ((AMBA_DRAM_LAST_ADDR - (UINT32) CONFIG_FWPROG_SYS_LOADADDR) + AMBA_RAM_MAGIC_CODE_OFFSET)
#define AMBA_RAM_MAGIC_CODE_PHYS_ADDR                   (AMBA_DRAM_RESERVED_PHYS_ADDR + AMBA_RAM_MAGIC_CODE_OFFSET)
#define AMBA_ROM_MAGIC_CODE_VIRT_ADDR                   (AMBA_RAM_MAGIC_CODE_VIRT_ADDR + AMBA_MAGIC_CODE_SIZE)
#define AMBA_ROM_MAGIC_CODE_PHYS_ADDR                   (AMBA_RAM_MAGIC_CODE_PHYS_ADDR + AMBA_MAGIC_CODE_SIZE)

#define AMBA_RAM_SYS_PTB_VIRT_ADDR                      ((AMBA_DRAM_LAST_ADDR - (UINT32) CONFIG_FWPROG_SYS_LOADADDR) + AMBA_RAM_NVM_SYS_PARTITION_TABLE_OFFSET)
#define AMBA_RAM_USER_PTB_VIRT_ADDR                     ((AMBA_DRAM_LAST_ADDR - (UINT32) CONFIG_FWPROG_SYS_LOADADDR) + AMBA_RAM_NVM_USER_PARTITION_TABLE_OFFSET)

#endif /* AMBA_MEM_MAP_H */
