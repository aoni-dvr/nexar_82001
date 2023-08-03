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
#ifdef CONFIG_BST_LARGE_SIZE
#define BST_MAX_SIZE                            0x4000U // 16K
#else
#define BST_MAX_SIZE                            0x2000U
#endif

#define BST_OFST_DRAM_HIGH_FREQ_PARAM           0x1c00U
#define DRAM_TRAIN_PARAM_OFST                   0x80U

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
#define DRAM_TRAIN_OPMODE_AUTO_CMD              9U
#define DRAM_TRAIN_OPMODE_AUTO_CMD_DONE         10U
#define DRAM_TRAIN_OPMODE_CMD                   11U
#define DRAM_TRAIN_OPMODE_CMD_DONE              12U
#define DRAM_TRAIN_OPMODE_BOTH_RW_DONE          13U      // Add for BL2 training stroage
#define NUM_DRAM_TRAIN_OPMODE                   14U

/* Offsets of dram training parameters, these offsets must be the same with AmbaBST_UserConfig_A64.asm */
#define DRAM_TRAIN_PARAM_OFST_FLAG             0x0U
#define DRAM_TRAIN_PARAM_OFST_CK_DELAY         0x1U
#define DRAM_TRAIN_PARAM_OFST_CA_DELAY_COARSE  0x2U
#define DRAM_TRAIN_PARAM_OFST_CKE_DELAY_COARSE 0x3U
#define DRAM_TRAIN_PARAM_OFST_RDDLY_DIE0       0x4U
#define DRAM_TRAIN_PARAM_OFST_RDDLY_DIE1       0x5U
#define DRAM_TRAIN_PARAM_OFST_RDVREF_0         0x6U
#define DRAM_TRAIN_PARAM_OFST_RDVREF_1         0x7U
#define DRAM_TRAIN_PARAM_OFST_WRVREF_0         0x8U
#define DRAM_TRAIN_PARAM_OFST_WRVREF_1         0x9U
#define DRAM_TRAIN_PARAM_OFST_B0_DLY0_D0       0xAU
#define DRAM_TRAIN_PARAM_OFST_B0_DLY1_D0       0xBU
#define DRAM_TRAIN_PARAM_OFST_B0_DLY2_D0       0xCU
#define DRAM_TRAIN_PARAM_OFST_B0_DLY3_D0       0xDU
#define DRAM_TRAIN_PARAM_OFST_B0_DLY4_D0       0xEU
#define DRAM_TRAIN_PARAM_OFST_B0_DLY5_D0       0xFU
#define DRAM_TRAIN_PARAM_OFST_B1_DLY0_D0       0x10U
#define DRAM_TRAIN_PARAM_OFST_B1_DLY1_D0       0x11U
#define DRAM_TRAIN_PARAM_OFST_B1_DLY2_D0       0x12U
#define DRAM_TRAIN_PARAM_OFST_B1_DLY3_D0       0x13U
#define DRAM_TRAIN_PARAM_OFST_B1_DLY4_D0       0x14U
#define DRAM_TRAIN_PARAM_OFST_B1_DLY5_D0       0x15U
#define DRAM_TRAIN_PARAM_OFST_B2_DLY0_D0       0x16U
#define DRAM_TRAIN_PARAM_OFST_B2_DLY1_D0       0x17U
#define DRAM_TRAIN_PARAM_OFST_B2_DLY2_D0       0x18U
#define DRAM_TRAIN_PARAM_OFST_B2_DLY3_D0       0x19U
#define DRAM_TRAIN_PARAM_OFST_B2_DLY4_D0       0x1AU
#define DRAM_TRAIN_PARAM_OFST_B2_DLY5_D0       0x1BU
#define DRAM_TRAIN_PARAM_OFST_B3_DLY0_D0       0x1CU
#define DRAM_TRAIN_PARAM_OFST_B3_DLY1_D0       0x1DU
#define DRAM_TRAIN_PARAM_OFST_B3_DLY2_D0       0x1EU
#define DRAM_TRAIN_PARAM_OFST_B3_DLY3_D0       0x1FU
#define DRAM_TRAIN_PARAM_OFST_B3_DLY4_D0       0x20U
#define DRAM_TRAIN_PARAM_OFST_B3_DLY5_D0       0x21U
#define DRAM_TRAIN_PARAM_OFST_B0_DLY0_D1       0x22U
#define DRAM_TRAIN_PARAM_OFST_B0_DLY1_D1       0x23U
#define DRAM_TRAIN_PARAM_OFST_B0_DLY2_D1       0x24U
#define DRAM_TRAIN_PARAM_OFST_B0_DLY3_D1       0x25U
#define DRAM_TRAIN_PARAM_OFST_B0_DLY4_D1       0x26U
#define DRAM_TRAIN_PARAM_OFST_B0_DLY5_D1       0x27U
#define DRAM_TRAIN_PARAM_OFST_B1_DLY0_D1       0x28U
#define DRAM_TRAIN_PARAM_OFST_B1_DLY1_D1       0x29U
#define DRAM_TRAIN_PARAM_OFST_B1_DLY2_D1       0x2AU
#define DRAM_TRAIN_PARAM_OFST_B1_DLY3_D1       0x2BU
#define DRAM_TRAIN_PARAM_OFST_B1_DLY4_D1       0x2CU
#define DRAM_TRAIN_PARAM_OFST_B1_DLY5_D1       0x2DU
#define DRAM_TRAIN_PARAM_OFST_B2_DLY0_D1       0x2EU
#define DRAM_TRAIN_PARAM_OFST_B2_DLY1_D1       0x2FU
#define DRAM_TRAIN_PARAM_OFST_B2_DLY2_D1       0x30U
#define DRAM_TRAIN_PARAM_OFST_B2_DLY3_D1       0x31U
#define DRAM_TRAIN_PARAM_OFST_B2_DLY4_D1       0x32U
#define DRAM_TRAIN_PARAM_OFST_B2_DLY5_D1       0x33U
#define DRAM_TRAIN_PARAM_OFST_B3_DLY0_D1       0x34U
#define DRAM_TRAIN_PARAM_OFST_B3_DLY1_D1       0x35U
#define DRAM_TRAIN_PARAM_OFST_B3_DLY2_D1       0x36U
#define DRAM_TRAIN_PARAM_OFST_B3_DLY3_D1       0x37U
#define DRAM_TRAIN_PARAM_OFST_B3_DLY4_D1       0x38U
#define DRAM_TRAIN_PARAM_OFST_B3_DLY5_D1       0x39U
#define DRAM_TRAIN_PARAM_OFST_MR30C0D0         0x3AU
#define DRAM_TRAIN_PARAM_OFST_MR30C0D1         0x3BU
#define DRAM_TRAIN_PARAM_OFST_MR30C1D0         0x3CU
#define DRAM_TRAIN_PARAM_OFST_MR30C1D1         0x3DU
#define DRAM_TRAIN_PARAM_OFST_MR12C0D0         0x3EU
#define DRAM_TRAIN_PARAM_OFST_MR12C0D1         0x3FU
#define DRAM_TRAIN_PARAM_OFST_MR12C1D0         0x40U
#define DRAM_TRAIN_PARAM_OFST_MR12C1D1         0x41U
#define DRAM_TRAIN_PARAM_OFST_MR14C0D0         0x42U
#define DRAM_TRAIN_PARAM_OFST_MR14C0D1         0x43U
#define DRAM_TRAIN_PARAM_OFST_MR14C1D0         0x44U
#define DRAM_TRAIN_PARAM_OFST_MR14C1D1         0x45U
#define DRAM_TRAIN_PARAM_OFST_MR15C0D0         0x46U
#define DRAM_TRAIN_PARAM_OFST_MR15C0D1         0x47U
#define DRAM_TRAIN_PARAM_OFST_MR15C1D0         0x48U
#define DRAM_TRAIN_PARAM_OFST_MR15C1D1         0x49U
#define DRAM_TRAIN_PARAM_OFST_WCK2DQC0D0       0x4AU
#define DRAM_TRAIN_PARAM_OFST_WCK2DQC1D0       0x4BU
#define DRAM_TRAIN_PARAM_OFST_WCK2DQC0D1       0x4CU
#define DRAM_TRAIN_PARAM_OFST_WCK2DQC1D1       0x4DU
#define DRAM_TRAIN_PARAM_OFST_STATE            0x4EU
#if defined(CONFIG_TEST_DRAM_SAVE_RESTORE)
#define DRAM_TRAIN_PARAM_OFST_RSTCOUNT         0x4FU
#define DRAM_TRAIN_PARAM_OFST_TOTALFAILCOUNT   0x50U
#define NUM_DRAM_TRAIN_PARAM_PER_HOST          (DRAM_TRAIN_PARAM_OFST_TOTALFAILCOUNT + 1U)
#else
#define NUM_DRAM_TRAIN_PARAM_PER_HOST          (DRAM_TRAIN_PARAM_OFST_STATE + 1U)
#endif

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
UINT32 BLD_WriteDdrcData(UINT8 *pDataBuf);
UINT32 BLD_LoadDdrcData(UINT8 *pDataBuf);
UINT32 BLD_EraseNvmPart(UINT32 IsUserPart, UINT32 PartID);
void BLD_LoadNvmPart(UINT32 PartFlag, UINT32 PartID, UINT8 *pDataBuf);
void BLD_WriteNvmPart(UINT32 PartFlag, UINT32 PartID, UINT32 DataSize, UINT8 *pDataBuf);
AMBA_SYS_PARTITION_TABLE_s *AmbaBLD_GetSysPartTable(void);
AMBA_USER_PARTITION_TABLE_s *AmbaBLD_GetUserPartTable(void);

/**
 *  Defined in AmbaBLD_Shell.c
 */
void AmbaBLD_BootCore1(UINT32 entry);
void BLD_ShellStart(UINT32 ForceStartFlag);
void BLD_CheckTrainingResult(void);
#if defined(CONFIG_TEST_DRAM_SAVE_RESTORE)
void BLD_CompareTrainingParameters(void);
#if defined(PEEP_DRAM_TRAINING_PARAM)
void BLD_PeepTrainingParameters(UINT32);
#endif
void BLD_CompareFunc(char*, volatile UINT32*, volatile UINT32*);
UINT32 BLD_Reset(void);
void BLD_DBG_printHEX(UINT32, char*);
#endif
#if defined(CONFIG_ATF_HAVE_BL2)
void *BL2_getAmbaTmpBuffer(void);
void BL2_DramTraining(void);
void BL2_UpdateDdrc(void);
void BL2_DramTrainFail(void);
void BL2_CmdStoreTrainingResult(UINT32 OpMode);
#endif

/**
 *  Defined in AmbaBLD_ChipVersion.c
 */
void BLD_ChipVersionCheck(void);

#endif /* AMBA_BLD_H */
