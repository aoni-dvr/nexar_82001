/**
 *  @file AmbaBLD_NAND.c
 *
 *  Copyright (c) 2021 Ambarella International LP
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
 *  @details Dram-Training API
 */


#include <AmbaBLD.h>
#include <AmbaMisraFix.h>
#include <AmbaCSL_DDRC.h>
#include <AmbaRTSL_Cache.h>

#if defined(CONFIG_ENABLE_NAND_BOOT) || defined(CONFIG_ENABLE_SPINAND_BOOT) || defined(CONFIG_ENABLE_EMMC_BOOT)
#include "AmbaNAND_Def.h"
#elif defined(CONFIG_ENABLE_SPINOR_BOOT)
#include "AmbaSpiNOR_Def.h"
#endif

extern void BLD_CmdDramReadTrain(void);
extern void BLD_CmdDramWriteTrain(void);

extern void *BL2_getAmbaTmpBuffer(void);
extern void BL2_UpdateDdrc(void);

/*  For CV5x, we use system scratchpad SRAM
 *  to pass training complete info after 2-nd boot from BST -> BL2.
 *
 *  At 1-st time boot, when DRAM is not yet trained,
 *  the main training parameters would be stored in DRAM,
 *  and the pointer to that address area would be pass to BLD through scratchpad register.
 */
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
#define BL2BINBUF 0x20E0030000UL
#endif

/** Get offset of storage to store DDRC parameters. */
static INT32 BLD_get_ddrc_param_offset(UINT32 *ParamSize)
{
#if defined(CONFIG_ENABLE_NAND_BOOT)
    extern const AMBA_NAND_DEV_INFO_s AmbaNAND_DevInfo;
    if (ParamSize != NULL) {
        *ParamSize = AmbaNAND_DevInfo.MainByteSize;
    }
    return 1u * (AmbaNAND_DevInfo.BlockPageSize-1U) * AmbaNAND_DevInfo.MainByteSize;

#elif defined(CONFIG_ENABLE_SPINAND_BOOT)
    extern const AMBA_SPINAND_DEV_INFO_s AmbaSpiNAND_DevInfo;;
    if (ParamSize != NULL) {
        *ParamSize = AmbaSpiNAND_DevInfo.MainByteSize;
    }
    return 1u * (AmbaSpiNAND_DevInfo.BlockPageSize-1U) * AmbaSpiNAND_DevInfo.MainByteSize;

#elif defined(CONFIG_ENABLE_SPINOR_BOOT)
    extern const AMBA_NORSPI_DEV_INFO_s AmbaNORSPI_DevInfo;; /* Pointer to external NOR device information */
    if (ParamSize != NULL) {
        *ParamSize = 2*AmbaNORSPI_DevInfo.PageSize;
    }
    return (AmbaNORSPI_DevInfo.EraseBlockSize - (2*AmbaNORSPI_DevInfo.PageSize));   // Last two pages of first block

#elif defined(CONFIG_ENABLE_EMMC_BOOT)
    if (ParamSize != NULL) {
        *ParamSize = 512u;
    }
    return ((AMBA_NAND_BOOTSTRAP_CODE_SPACE_SIZE / 512u) - 1u) * 512u;

#else
#error Unknown boot device
    return 0;
#endif
}

/** Doing DRAM training */
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
static void BL2_doDramTrain(void)
{
    UINT32 DramTrainingState;

    DramTrainingState = AmbaCSL_DdrcGetTrainScratchPad(0U);

    if ((MAGIC_CODE_DRAM_TRAIN_COMPLETION != DramTrainingState) &&
        (MAGIC_CODE_DRAM_TRAIN_FAILURE != DramTrainingState)) {
        DramTrainingState = (DramTrainingState >> DRAM_TRAIN_PROC_OPMODE_OFST);
        if ( DramTrainingState == 0U ) {
#ifdef CONFIG_AUTO_DRAM_TRAINING
#if !defined(CONFIG_BST_DRAM_TRAINING)
            AmbaCSL_DdrcSetTrainScratchPad(0U, (UINT32)DRAM_TRAIN_OPMODE_BOTH_RW_DONE << DRAM_TRAIN_PROC_OPMODE_OFST);
            BL2_DramTraining();
            BL2_CmdStoreTrainingResult((UINT32)DRAM_TRAIN_OPMODE_BOTH_RW_DONE << DRAM_TRAIN_PROC_OPMODE_OFST);
#endif
#endif
        } else if ( DramTrainingState == DRAM_TRAIN_OPMODE_BOTH_RW_DONE ) {
            /* DRAM_TRAIN_OPMODE_RW_DONE */
        } else {
            BL2_DramTrainFail();
        }
    }
}
#else
static void BL2_doDramTrain(void)
{
    UINT32 DramTrainingState;

    DramTrainingState = AmbaCSL_DdrcGetTrainScratchPad(0U);

    if ((MAGIC_CODE_DRAM_TRAIN_COMPLETION != DramTrainingState) &&
        (MAGIC_CODE_DRAM_TRAIN_FAILURE != DramTrainingState)) {
        DramTrainingState = (DramTrainingState >> DRAM_TRAIN_PROC_OPMODE_OFST);
        if ( DramTrainingState == 0U ) {
            /* DRAM_TRAIN_OPMODE_READ */
            AmbaCSL_DdrcSetTrainScratchPad(0U, (UINT32)DRAM_TRAIN_OPMODE_READ_DONE << DRAM_TRAIN_PROC_OPMODE_OFST);
            BLD_CmdDramReadTrain();
        } else if ( DramTrainingState == DRAM_TRAIN_OPMODE_READ_DONE ) {
            /* DRAM_TRAIN_OPMODE_READ_DONE, DRAM_TRAIN_OPMODE_WRITE */
            AmbaCSL_DdrcSetTrainScratchPad(0U, (UINT32)DRAM_TRAIN_OPMODE_BOTH_RW_DONE << DRAM_TRAIN_PROC_OPMODE_OFST);
            BLD_CmdDramWriteTrain();
        } else if ( DramTrainingState == DRAM_TRAIN_OPMODE_BOTH_RW_DONE ) {
            /* DRAM_TRAIN_OPMODE_WRITE_DONE */
#if defined(CONFIG_SOC_CV28)
            // At this stage we write ddrc trained parameters to system scratchpad and pass down to BLD
            BL2_CmdStoreTrainingResult((UINT32)DRAM_TRAIN_OPMODE_BOTH_RW_DONE);
#endif
        } else {
            /* DramTrainingState unknown */
        }
    }
}
#endif

/** Called by BL2 to check training results. */
void BL2_CheckTrainingResult(int (*ambarella_read_dramc_param)(unsigned int offset, void *buff, unsigned int size))
{
    UINT32 offset;
    UINT32 page_size = 2048u;
    UINT32 DramTrainingState;
    UINT32 *pBuffer;
    INT32 retVal;
    UINT32 doTrain = 0u;
    void *buff = BL2_getAmbaTmpBuffer();
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    const UINT8 *pData = (UINT8*)BL2BINBUF;
#endif

    if (ambarella_read_dramc_param != NULL) {
        offset = BLD_get_ddrc_param_offset(&page_size);

        retVal = ambarella_read_dramc_param(offset, buff, page_size);
        if (retVal != 0) {
            doTrain = 1u;
        } else {
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
            AmbaMisra_TypeCast64(&pBuffer, &pData);
#else
            AmbaMisra_TypeCast64(&pBuffer, &buff);
#endif
            DramTrainingState = pBuffer[DRAM_TRAIN_PARAM_OFST_FLAG];
            (void)AmbaRTSL_CacheFlushDataPtr(&DramTrainingState, sizeof(UINT32));
            if (MAGIC_CODE_DRAM_TRAIN_COMPLETION != DramTrainingState) {
                doTrain = 1u;
            } else {
                BL2_UpdateDdrc();
            }
        }

        if (doTrain == 1u) {
            BL2_doDramTrain();
        }
    }
}


/*
 * ATF do not link libwrap_std.a
 * So make a simple weak to use libc from ATF.
 */
void *memcpy(void *dest, const void *src, size_t n);
UINT32 AmbaWrap_memcpy(void *pDst, const void *pSrc, SIZE_t num) __attribute__((weak));

UINT32 AmbaWrap_memcpy(void *pDst, const void *pSrc, SIZE_t num)
{
    (void) memcpy(pDst, pSrc, num);

    return 0u;
}

void *memset(void *ptr, INT32 v, SIZE_t n);
UINT32 AmbaWrap_memset(void *ptr, INT32 v, SIZE_t n) __attribute__((weak));

UINT32 AmbaWrap_memset(void *ptr, INT32 v, SIZE_t n)
{
    UINT32 err = 0U;

    if (ptr == NULL) {
        err = 1U;
    } else {
        memset(ptr, v, n);
    }

    return err;
}

