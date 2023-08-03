/**
 *  @file AmbaBLD.h
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
 *  @details Definitions & Constants for Bootloader APIs
 *
 */

#ifndef AMBA_BLD_H
#define AMBA_BLD_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif

#ifndef AMBA_ERROR_CODE_H
#include "AmbaErrorCode.h"
#endif

#include "AmbaNVM_Partition.h"

#define BLD_ERR_NONE       OK
#define BLD_ERR_ARG        (BLD_ERR_BASE + 0x1U)
#define BLD_ERR_TIMEOUT    (BLD_ERR_BASE + 0x2U)
#define BLD_ERR_INFO       (BLD_ERR_BASE + 0x3U)

#ifndef CONFIG_SECURE_MEMORY_SIZE
#define CONFIG_SECURE_MEMORY_SIZE 0x0U
#endif

/* Byte offset of bootstrap binary */
#define BST_OFST_DRAM_HIGH_FREQ_PARAM           0xE00U   /* Offset: 512. Address to store dram high frequency parameters */

/* Magic codes used to stop dram training procedure */
#define MAGIC_CODE_DRAM_TRAIN_COMPLETION        0x61626D41U  /* Amba */
#define MAGIC_CODE_DRAM_TRAIN_FAILURE           0x416D6261U  /* abmA */

/* Definition for dram training procedure */
#define DRAM_TRAIN_PROC_RESULT_MASK             0xFFFFFFU
#define DRAM_TRAIN_PROC_OPMODE_OFST             24U

#define DRAM_TRAIN_OPMODE_AUTO_READ             0U
#define DRAM_TRAIN_OPMODE_AUTO_READ_DONE        1U
#define DRAM_TRAIN_OPMODE_AUTO_WRITE            2U
#define DRAM_TRAIN_OPMODE_AUTO_WRITE_DONE       3U
#define DRAM_TRAIN_OPMODE_READ                  4U
#define DRAM_TRAIN_OPMODE_READ_DONE             5U
#define DRAM_TRAIN_OPMODE_WRITE                 6U
#define DRAM_TRAIN_OPMODE_WRITE_DONE            7U
#define DRAM_TRAIN_OPMODE_FAIL                  8U
#define NUM_DRAM_TRAIN_OPMODE                   9U

/* Offsets of dram training parameters, these offsets must be the same with AmbaBST_UserConfig_A64.asm */
#define DRAM_TRAIN_PARAM_OFST_FLAG              0x0U
#define DRAM_TRAIN_PARAM_OFST_DQ_WDLY           0x1U
#define DRAM_TRAIN_PARAM_OFST_DQCA              0x2U
#define DRAM_TRAIN_PARAM_OFST_DLL0              0x3U
#define DRAM_TRAIN_PARAM_OFST_DLL1              0x4U
#define DRAM_TRAIN_PARAM_OFST_DLL2              0x5U
#define DRAM_TRAIN_PARAM_OFST_DLL3              0x6U
#define DRAM_TRAIN_PARAM_OFST_MR14              0x7U
#define DRAM_TRAIN_PARAM_OFST_FLAG_V            0x8U
#define DRAM_TRAIN_PARAM_OFST_DQSDLY            0x9U
#define DRAM_TRAIN_PARAM_OFST_STATE             0x10U
#define NUM_DRAM_TRAIN_PARAM                    (DRAM_TRAIN_PARAM_OFST_STATE + 1U)

/* Status flags used in DRAM_TRAIN_PARAM_OFST_FLAG */
#define DRAM_TRAIN_FLAG_FAIL                    0x80000000U

INT32 main(void);

/**
 *  Defined in AmbaBLD_NAND/emmc/spinor.c
 */
UINT32 BLD_InitBootDevice(void);
void BLD_LoadFirmware(void);
void BLD_EnterKernel(void);
void BLD_EraseNvm(void);
UINT32 BLD_EraseNvmPart(UINT32 IsUserPart, UINT32 PartID);
void BLD_LoadNvmPart(UINT32 PartFlag, UINT32 PartID, UINT8 *pDataBuf);
void BLD_WriteNvmPart(UINT32 PartFlag, UINT32 PartID, UINT32 DataSize, UINT8 *pDataBuf);
AMBA_SYS_PARTITION_TABLE_s *AmbaBLD_GetSysPartTable(void);
AMBA_USER_PARTITION_TABLE_s *AmbaBLD_GetUserPartTable(void);
INT32 main(void);

/**
 *  Defined in AmbaBLD_Shell.c
 */
void BLD_ShellStart(UINT32 ForceStartFlag);
void BLD_CheckTrainingResult(void);

#endif /* AMBA_BLD_H */
