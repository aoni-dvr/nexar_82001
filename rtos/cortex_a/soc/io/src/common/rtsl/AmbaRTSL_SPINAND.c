/*
 * Copyright (c) 2020 Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella International LP.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaMisraFix.h"
#include "AmbaWrap.h"

#include "AmbaRTSL_PLL.h"
#include "AmbaCSL_NAND.h"
#include "AmbaCSL_SPINAND.h"
#include "AmbaRTSL_SPINAND.h"
#include "AmbaRTSL_NAND_OP.h"
#include "AmbaRTSL_NAND_Ctrl.h"
#include "AmbaRTSL_NAND_BBM.h"
#include "AmbaCSL_FIO.h"
#include "AmbaRTSL_FIO.h"

#ifdef CONFIG_QNX
#define AmbaRTSL_PllGetCortexClk() (756000000U)
#if defined (CONFIG_SOC_CV2FS)|| defined (CONFIG_SOC_CV22FS)
#define AmbaRTSL_PllGetCortex1Clk() (756000000U)
#endif
#endif

static void AmbaRTSL_SpiNandHookApi(void);

/**
 *  AmbaRTSL_NandInit - Initialize the NAND flash controller
 *  @return error code
 */
UINT32 AmbaRTSL_SpiNandInit(void)
{
    //AmbaRTSL_FioReset();
    AmbaCSL_FioDmaFifoModeEnable(); /* Enable DMA Mode for FIO-DMA FIFO */
    AmbaCSL_FioInterruptEnable();   /* Enable CMD done & ECC interrupt */

    AmbaRTSL_SpiNandHookApi();

    return OK;
}

/**
 *  AmbaRTSL_SpiNand_DisbaleOnDevECC -
 *  @return error code
 */
UINT32 AmbaRTSL_SpiNand_ConfigOnDevECC(void)
{
    const AMBA_SPINAND_DEV_INFO_s *pNandDevInfo = AmbaRTSL_SpiNandDevInfo;
    UINT8 Status;
    UINT32 RetVal;

    RetVal = AmbaRTSL_SpiNandReset();
    if (RetVal == OK) {
        /* Unlock BP0~2 From Device */
        AmbaRTSL_SpiNandGetFeature(0xA0U, &Status);
        if (Status != 0U) {
            AmbaRTSL_SpiNandSetFeature(0xA0U, 0);
        }

        /* Disable the ECC Feature on Device */
        if (pNandDevInfo->InternalECC == AMBA_SPINAND_DISABLE_ONDEVICE_ECC) {
            AmbaRTSL_SpiNandGetFeature(0xB0U, &Status);
            if ((Status & 0x10U) != 0U) {
                Status &= 0xefU;
                AmbaRTSL_SpiNandSetFeature(0xB0U, Status);
            }
        }
    }

    return RetVal;
}

/**
 *  GetSpiNandCycleCount -
 *  @return
 */
static UINT8 GetSpiNandCycleCount(UINT32 Val0)
{
    UINT32 ValU32;
    UINT8 ValU8;

    ValU32 = (((Val0) > (1U)) ? (Val0) : (1U));

    if (AmbaWrap_memcpy(&ValU8, &ValU32, sizeof(ValU8)) != OK) { /* Do nothing */ };
    ValU8 -= 1U;

    return ValU8;
}

/**
 *  AmbaRTSL_SpiNandSetTiming - Set timing parameters for the SpiNAND flash device
 *  @param[in] pNandDevInfo pointer to NAND flash device information
 *  @return error code
 */
#if defined (CONFIG_SOC_CV5)|| defined (CONFIG_SOC_CV52)
void AmbaRTSL_SpiNandSetTiming(const AMBA_SPINAND_DEV_INFO_s *pSpiNandDevInfo)
{
#define MAX_SPINAND_CLOCK (100000000U)

    UINT32 NandClkFreq; /* Unit: MHz */
    UINT8 tCLQV;
    UINT32 Divider;
    AMBA_SPINAND_TIMING_CTRL_s NandTiming = {0};

    (void)AmbaRTSL_PllSetNandClk(533000000U);

    NandClkFreq = AmbaRTSL_PllGetNandClk() / 1000000U;

    Divider = GetRoundUpValU32(AmbaRTSL_PllGetNandClk(), MAX_SPINAND_CLOCK);

    NandTiming.Timing0.tCLL = GetSpiNandCycleCount(GetRoundUpValU32(Divider, 2U));
    NandTiming.Timing0.tCLH = GetSpiNandCycleCount(Divider / 2U);

    if (((((UINT32)NandTiming.Timing0.tCLL + 1U) * 100U) / NandClkFreq) > pSpiNandDevInfo->tCLL) {
        NandTiming.Timing0.tCLL++;
    }
    if (((((UINT32) NandTiming.Timing0.tCLH + 1U) * 100U) / NandClkFreq) > pSpiNandDevInfo->tCLL) {
        NandTiming.Timing0.tCLH++;
    }

    NandTiming.Timing0.tCS  = GetSpiNandCycleCount(GetRoundUpValU32(NandClkFreq * pSpiNandDevInfo->tCS, 1000U));
    if (NandTiming.Timing0.tCS < 0x14U) {
        NandTiming.Timing0.tCS = 0x14U;
    }

    //NandTiming.Timing0.tCLQV  = GetSpiNandCycleCount(GetRoundUpValU32(NandClkFreq * ((UINT32) pSpiNandDevInfo->tCLQV + 7U), 1000U));
    NandTiming.Timing0.tCLQV  = GetSpiNandCycleCount(GetRoundUpValU32((UINT32) (NandClkFreq * (((UINT32) pSpiNandDevInfo->tCLQV * 10U) + 81U)), 10000U));

    tCLQV = (UINT8)(((NandTiming.Timing0.tCLL + NandTiming.Timing0.tCLH + 2U) * 2U) + NandTiming.Timing0.tCLL + 1U) - 1U;
    if(NandTiming.Timing0.tCLQV > tCLQV) {
        /* __asm__ __volatile__ ("B ."); */
    }

    NandTiming.Timing1.tCHSL = GetSpiNandCycleCount(GetRoundUpValU32(NandClkFreq * pSpiNandDevInfo->tCHSL, 1000U));
    NandTiming.Timing1.tSLCH = GetSpiNandCycleCount(GetRoundUpValU32(NandClkFreq * pSpiNandDevInfo->tSLCH, 1000U));
    NandTiming.Timing1.tCHSH = GetSpiNandCycleCount(GetRoundUpValU32(NandClkFreq * pSpiNandDevInfo->tCHS, 1000U));
    NandTiming.Timing1.tSHC  = GetSpiNandCycleCount(GetRoundUpValU32(NandClkFreq * pSpiNandDevInfo->tSHC, 1000U));

    NandTiming.Timing2.tHHQX = GetSpiNandCycleCount(GetRoundDownValU32(NandClkFreq * pSpiNandDevInfo->tHHQX, 1000U));
    NandTiming.Timing2.tWPS  = GetSpiNandCycleCount(GetRoundUpValU32(NandClkFreq * pSpiNandDevInfo->tWPS, 1000U));
    NandTiming.Timing2.tWPH  = GetSpiNandCycleCount(GetRoundUpValU32(NandClkFreq * pSpiNandDevInfo->tWPH, 1000U));

    AmbaCSL_SpiNandSetTiming(&NandTiming);
}
#else
void AmbaRTSL_SpiNandSetTiming(const AMBA_SPINAND_DEV_INFO_s *pSpiNandDevInfo)
{
    UINT32 NandClkFreq; /* Unit: MHz */
    UINT8 tCLQV;
    AMBA_SPINAND_TIMING_CTRL_s NandTiming= {0};

    NandClkFreq = AmbaRTSL_PllGetNandClk() / 1000000U;

    NandTiming.Timing0.tCLH = GetSpiNandCycleCount(GetRoundUpValU32(NandClkFreq * pSpiNandDevInfo->tCLH, 1000U));
    NandTiming.Timing0.tCLL = GetSpiNandCycleCount(GetRoundUpValU32(NandClkFreq * pSpiNandDevInfo->tCLL, 1000U));
    if ((NandTiming.Timing0.tCLH <= 1U) || (NandTiming.Timing0.tCLL <= 1U)) {
        NandTiming.Timing0.tCLL = 2U;
        NandTiming.Timing0.tCLH = 2U;
    }

    NandTiming.Timing0.tCS  = GetSpiNandCycleCount(GetRoundUpValU32(NandClkFreq * pSpiNandDevInfo->tCS, 1000U));
    if (NandTiming.Timing0.tCS < 0x14U) {
        NandTiming.Timing0.tCS = 0x14U;
    }

    NandTiming.Timing0.tCLQV  = GetSpiNandCycleCount(GetRoundUpValU32(NandClkFreq * ((UINT32) pSpiNandDevInfo->tCLQV + 7U), 1000U));
    tCLQV = (UINT8)(((NandTiming.Timing0.tCLL + NandTiming.Timing0.tCLH + 2U) * 2U) + NandTiming.Timing0.tCLL + 1U) - 1U;
    if(NandTiming.Timing0.tCLQV > tCLQV) {
        /* __asm__ __volatile__ ("B ."); */
    }

    NandTiming.Timing1.tCHSL = GetSpiNandCycleCount(GetRoundUpValU32(NandClkFreq * pSpiNandDevInfo->tCHSL, 1000U));
    NandTiming.Timing1.tSLCH = GetSpiNandCycleCount(GetRoundUpValU32(NandClkFreq * pSpiNandDevInfo->tSLCH, 1000U));
    NandTiming.Timing1.tCHS  = GetSpiNandCycleCount(GetRoundUpValU32(NandClkFreq * pSpiNandDevInfo->tCHS, 1000U));
    NandTiming.Timing1.tSHC  = GetSpiNandCycleCount(GetRoundUpValU32(NandClkFreq * pSpiNandDevInfo->tSHC, 1000U));

    NandTiming.Timing2.tHHQX = GetSpiNandCycleCount(GetRoundDownValU32(NandClkFreq * pSpiNandDevInfo->tHHQX, 1000U));
    NandTiming.Timing2.tWPS  = GetSpiNandCycleCount(GetRoundUpValU32(NandClkFreq * pSpiNandDevInfo->tWPS, 1000U));
    NandTiming.Timing2.tWPH  = GetSpiNandCycleCount(GetRoundUpValU32(NandClkFreq * pSpiNandDevInfo->tWPH, 1000U));

    AmbaCSL_SpiNandSetTiming(&NandTiming);
}
#endif

/**
 *  AmbaRTSL_SpiNandConfig - NAND software configurations
 *  @param[in] pNandConfig pointer to NAND software configurations
 *  @return error code
 */
UINT32 AmbaRTSL_SpiNandConfig(AMBA_SPINAND_CONFIG_s *pNandConfig)
{
    AMBA_NAND_CTRL_REG_s NandCtrl = {0};
    AMBA_SPINAND_CTRL_REG_s SCtrl = {0};
    AMBA_SPINAND_DEV_INFO_s *pNandDevInfo = NULL;
    AMBA_PARTITION_CONFIG_s *pUserPartConfig = NULL;
    UINT32 MainByteSize, SpareByteSize;
    UINT32 NumEccBit, k;
    UINT32 RetVal = SPINAND_ERR_NONE;
    UINT32 Tmp32;
    UINT8  Tmp8;

    AmbaMisra_TouchUnused(pNandConfig);
    AmbaMisra_TouchUnused(pNandDevInfo);

    if ((pNandConfig == NULL) || (pNandConfig->pNandDevInfo == NULL) || (pNandConfig->pUserPartConfig == NULL)) {
        RetVal = SPINAND_ERR_ARG;  /* wrong parameters */
    } else {

        /* Select NandController to Spinand */
        AmbaCSL_FioSelectSpiNand();

        pNandDevInfo    = pNandConfig->pNandDevInfo;
        pUserPartConfig = pNandConfig->pUserPartConfig;
        SpareByteSize   = pNandDevInfo->SpareByteSize;
        MainByteSize    = pNandDevInfo->MainByteSize;
        k               = MainByteSize / 512U;

        AmbaRTSL_NandCtrl.NandInfo.BlockPageSize  = pNandDevInfo->BlockPageSize;
        AmbaRTSL_NandCtrl.NandInfo.MainByteSize   = pNandDevInfo->MainByteSize;
        AmbaRTSL_NandCtrl.NandInfo.PlaneAddrMask  = 0;
        AmbaRTSL_NandCtrl.NandInfo.PlaneBlockSize = pNandDevInfo->PlaneBlockSize;
        AmbaRTSL_NandCtrl.NandInfo.SpareByteSize  = pNandDevInfo->SpareByteSize;
        AmbaRTSL_NandCtrl.NandInfo.TotalPlanes    = pNandDevInfo->TotalPlanes;
        AmbaRTSL_NandCtrl.NandInfo.TotalZones     = pNandDevInfo->TotalZones;
        AmbaRTSL_NandCtrl.NandInfo.ZoneBlockSize  = pNandDevInfo->ZoneBlockSize;

        AmbaRTSL_NandCtrl.DevInfo.pSpiNandDevInfo = pNandDevInfo;          /* save the pointer to NAND Device Information */
        AmbaRTSL_NandCtrl.pSysPartConfig  = pNandConfig->pSysPartConfig;   /* pointer to System partition configurations */
        AmbaRTSL_NandCtrl.pUserPartConfig = pUserPartConfig;               /* save the pointer to User Partition Configurations */

        /* Set timing parameters for the NAND flash device */
        AmbaRTSL_SpiNandSetTiming(pNandDevInfo);

        if (MainByteSize > 2048U) {
            AmbaCSL_NandSetPageSize4KB();
        } else {
            AmbaCSL_NandDisablePageSize4KB();
        }

        if (pNandDevInfo->ChipSize > AMBA_NAND_SIZE_8G_BITS) {
            AmbaCSL_NandSetChipSizeOver8Gb();
        }
#if 0
        if (SpareByteSize < (k * 16U)) {
            /* BBM: small page (OOB size < 64 Bytes; Page size <= 512 Bytes), check the 5th Byte */
            AmbaRTSL_NandCtrl.BadBlkMarkOffset   = 5U;  /* the offset of Bad Block Mark */
            AmbaRTSL_NandCtrl.BbtSignatureOffset = 0U;  /* the offset of BBT Signature */
            AmbaRTSL_NandCtrl.BbtVersionOffset   = 12U; /* the offset of BBT Version */
            return NG;      /* Small page does not support multi-bits BCH */
        }
#endif
        /* BBM: large page (OOB size >= 64 Bytes; Page size >= 2K Bytes), check the 0th Byte */
        AmbaRTSL_NandCtrl.BadBlkMarkOffset = 0;        /* the offset of Bad Block Mark */

        if (pNandDevInfo->InternalECC == AMBA_SPINAND_DISABLE_ONDEVICE_ECC) {
            if (SpareByteSize >= (32U * k)) {
                NumEccBit = 8U;  /* Bose-Chaudhuri-Hocquenghem (BCH) error correction (8-bit) */
            } else {
                NumEccBit = 6U;  /* Bose-Chaudhuri-Hocquenghem (BCH) error correction (6-bit) */
            }
        } else {
            NumEccBit = 0;
        }

        if (SpareByteSize >= (32U * k)) {
            AmbaRTSL_NandCtrl.BbtSignatureOffset = 2U;  /* the offset of BBT Signature */
            AmbaRTSL_NandCtrl.BbtVersionOffset   = 8U;  /* the offset of BBT Version */
            AmbaCSL_NandEnableSpare2xArea();
        } else {
            AmbaRTSL_NandCtrl.BbtSignatureOffset = 1U;  /* the offset of BBT Signature */
            AmbaRTSL_NandCtrl.BbtVersionOffset   = 18U; /* the offset of BBT Version */
            AmbaCSL_NandDisableSpare2xArea();
        }

        /* 1 - Column Address in 2 Cycles for 2KB/page, 0 - for 512B/page */
        NandCtrl.PageAddr3Cycle = 1U;     /* 1 - Page Address in 3 Cycles */

        Tmp32 = pNandDevInfo->ChipSize;
        if (AmbaWrap_memcpy(&Tmp8, &Tmp32, sizeof(Tmp8)) != OK) { /* Do nothing */ };
        NandCtrl.ChipSize = Tmp8;

        //NandCtrl.Addr33_32 = (pNandDevInfo->ChipSize > AMBA_NAND_SIZE_4G_BITS) ? 1U : 0U;
        NandCtrl.WriteWithAutoStatusUpdate = 0; /* 1 - Clear CmdCode when command is complete done */

        AmbaCSL_NandSetCtrlReg(NandCtrl);

        /* Set the timeout value to SpiNand Controller */
        SCtrl.MaxCmdLoop   = 0x3ffffU;
        SCtrl.FlowCtrl     = 0;
        SCtrl.SpiClockMode = 0;

        if (AmbaWrap_memcpy(&Tmp8, &pNandDevInfo->PlaneSelect, sizeof(Tmp8)) != OK) { /* Do nothing */ };
        SCtrl.PlaneSelect  = Tmp8;
        AmbaCSL_SpiNandSetCtrlReg(SCtrl);

        /* Enable Bose-Chaudhuri-Hocquenghem (BCH) error correction (6-bit/8-bit) */
        if (NumEccBit != 0U) {
            AmbaCSL_NandEnableBCH(NumEccBit);
        } else {
            AmbaCSL_NandDisableBCH(MainByteSize, pNandDevInfo->SpareByteSize);
        }

        if (AmbaWrap_memcpy(&AmbaRTSL_NandCtrl.NandCtrlRegVal, &NandCtrl, sizeof(AmbaRTSL_NandCtrl.NandCtrlRegVal)) != OK) { /* Do nothing */ };

        AmbaRTSL_NandCtrl.NumEccBit = NumEccBit;

        /* Block size in Byte */
        AmbaRTSL_NandCtrl.BlkByteSize = MainByteSize * pNandDevInfo->BlockPageSize;

        /* Total number of blocks */
        AmbaRTSL_NandCtrl.TotalNumBlk = pNandDevInfo->TotalPlanes * pNandDevInfo->PlaneBlockSize;

        /* Number of Pages for BST */
        AmbaRTSL_NandCtrl.BstPageCount = GetRoundUpValU32(AMBA_NAND_BOOTSTRAP_CODE_SPACE_SIZE, MainByteSize);

        /* Number of Pages for System Partition Table */
        AmbaRTSL_NandCtrl.SysPtblPageCount = GetRoundUpValU32((UINT32)(sizeof(AMBA_SYS_PARTITION_TABLE_s) & 0xffffffffU), MainByteSize);

        /* Number of Pages for User Partition Table */
        AmbaRTSL_NandCtrl.UserPtblPageCount = GetRoundUpValU32((UINT32)(sizeof(AMBA_USER_PARTITION_TABLE_s) & 0xffffffffU), MainByteSize);

        /* Number of Pages for Vendor Specific Data */
        k = pUserPartConfig[AMBA_USER_PARTITION_PTB].ByteCount;
        AmbaRTSL_NandCtrl.VendorDataPageCount = GetRoundUpValU32(k, MainByteSize);

        /* Number of Pages for BBT: Use 2-bit Mark per block, 1 Byte holds Marks of 4 Blocks */
        AmbaRTSL_NandCtrl.BbtPageCount = GetRoundUpValU32(GetRoundUpValU32(AmbaRTSL_NandCtrl.TotalNumBlk, 4U), MainByteSize);
    }
    return RetVal;
}

/**
 *  AmbaRTSL_SpiNandWaitCmdDone - Wait for NAND command done
 */
static void AmbaRTSL_SpiNandWaitCmdDone(void)
{
#if defined(CONFIG_SOC_CV2FS)
    UINT32 TimeOut = (AmbaRTSL_PllGetCortex1Clk() / 10000U) * 5000U;
#else
    UINT32 TimeOut = (AmbaRTSL_PllGetCortexClk() / 10000U) * 5000U;
#endif
    for (UINT32 i = 0; i < TimeOut; i++) {
        /* Wait for NAND command done */
        if (AmbaCSL_FioDoneStatus() != 0U) {
            break;
        }
    }
    AmbaCSL_FioClearIrqStatus();   /* clear NAND interrupt status */
}

/**
 *  AmbaRTSL_SpiNandReadID - To retrieve the ONFI signature or JEDEC manufacturer ID and device ID
 *  @param[out] pDeviceID ID byte sequence
 *  @return error code
 */
UINT32 AmbaRTSL_SpiNandReadID(UINT8 *pDeviceID)
{
    AmbaCSL_SpiNandSendReadIdCmd();
    AmbaRTSL_SpiNandWaitCmdDone();      /* Wait for SpiNAND command done */
    AmbaCSL_SpiNandGetIdResponse(pDeviceID);
    return OK;
}

/**
 *  AmbaRTSL_SpiNandWriteEnable - Put the target in its default power-up state and in SDR data interface
 *  @return error code
 */
static void AmbaRTSL_SpiNandWriteEnable(void)
{
    AmbaCSL_SpiNandSendWriteEnable();
    AmbaRTSL_SpiNandWaitCmdDone();      /* Wait for SpiNAND command done */
}

#if 0
/**
 *  AmbaRTSL_SpiNandWriteEnable - Put the target in its default power-up state and in SDR data interface
 */
static void AmbaRTSL_SpiNandWriteDisable(void)
{
    AmbaCSL_SpiNandSendWriteDisable();
    AmbaRTSL_SpiNandWaitCmdDone();      /* Wait for SpiNAND command done */
}
#endif

/**
 *  AmbaRTSL_SpiNandReset - Put the target in its default power-up state and in SDR data interface
 *  @return error code
 */
UINT32 AmbaRTSL_SpiNandReset(void)
{
    UINT8 Status;
    UINT32 RetVal = SPINAND_ERR_NONE;
#if defined(CONFIG_SOC_CV2FS)
    UINT32 TimeOut = (AmbaRTSL_PllGetCortex1Clk() / 10000U) * 5000U;
#else
    UINT32 TimeOut = (AmbaRTSL_PllGetCortexClk() / 10000U) * 5000U;
#endif
    AmbaCSL_SpiNandSendResetCmd();      /* send the command */
    AmbaRTSL_SpiNandWaitCmdDone();      /* Wait for SpiNAND command done */

    do {
        AmbaRTSL_SpiNandGetFeature(0xC0U, &Status);
        TimeOut --;
        if (TimeOut == 0U) {
            RetVal = SPINAND_ERR_IO_FAIL;
            break;
        }
    } while((Status & 0x1U) != 0U);

    return RetVal;
}

/**
 *  AmbaRTSL_SpiNandEraseBlockStart -
 *  @param[in] BlkAddr
 */
void AmbaRTSL_SpiNandEraseBlockStart(UINT32 BlkAddr)
{
    const AMBA_SPINAND_DEV_INFO_s *pNandDevInfo = AmbaRTSL_SpiNandDevInfo;
    UINT64 SrcAddr;
    UINT32 AddrHi, AddrLow;

    if (pNandDevInfo->EraseAddrType == AMBA_SPINAND_ERASE_ADDRBY_BYTE) {
        SrcAddr = (UINT64)BlkAddr * AmbaRTSL_NandCtrl.BlkByteSize;
        AddrHi  = (UINT32)(SrcAddr >> 32U);
        AddrLow = (UINT32)(BlkAddr * AmbaRTSL_NandCtrl.BlkByteSize);
    } else if (pNandDevInfo->EraseAddrType == AMBA_SPINAND_ERASE_ADDRBY_PAGE) {
        SrcAddr = (UINT64)BlkAddr *  pNandDevInfo->BlockPageSize;
        AddrHi =  (UINT32)(SrcAddr >> 32U);
        AddrLow = (UINT32)(BlkAddr *  pNandDevInfo->BlockPageSize);
    } else {
        SrcAddr = (UINT64) BlkAddr;
        AddrHi  = (UINT32)(SrcAddr >> 32U);
        AddrLow = (UINT32) BlkAddr;
    }
    AmbaCSL_SpiNandSendBlockEraseCmd(AddrHi, AddrLow, pNandDevInfo->PageCycles);
}

/**
 *  AmbaRTSL_SpiNandEraseBlock - To erase the block of data identified by the block address parameter
 *  @param[in] BlkAddr
 *  @return error code
 */
UINT32 AmbaRTSL_SpiNandEraseBlock(UINT32 BlkAddr)
{
    UINT32 RetValue;
    const AMBA_SPINAND_DEV_INFO_s *pNandDevInfo = AmbaRTSL_SpiNandDevInfo;

    AmbaRTSL_SpiNandEraseBlockStart(BlkAddr);

    AmbaRTSL_SpiNandWaitCmdDone();      /* Wait for NAND command done */

    RetValue = AmbaRTSL_SpiNandCheckDeviceStatus(pNandDevInfo->BlockPageSize, AMBA_SPINAND_ERROR_ERASE_FAIL);

    return RetValue;
}

#if 0
/**
 *  AmbaRTSL_SpiNandReadStatus - send command 0xf to the SPI Nand status register.
 *  @param[out] pDeviceID ID byte sequence (at most five bytes)
 *  @return error code
 */
void AmbaRTSL_SpiNandReadStatus(UINT8 *pStatus)
{
    AmbaCSL_SpiNandSendGetFeature(0xC0U);
    AmbaRTSL_SpiNandWaitCmdDone();      /* Wait for NAND command done */
    AmbaCSL_SpiNandGetCustomCmdData(1U, pStatus);
}
#endif

/**
 *  AmbaRTSL_SpiNandSetFeature - send command 0xf to the SPI Nand status register
 *  @param[in] FeatureAddr
 *  @param[in] Value
 *  @return error code
 */
void AmbaRTSL_SpiNandSetFeature(UINT8 FeatureAddr, UINT8 Value)
{
    AmbaRTSL_SpiNandWriteEnable();

    AmbaCSL_SpiNandSendSetFeature(FeatureAddr, Value);
    AmbaRTSL_SpiNandWaitCmdDone();      /* Wait for NAND command done */
}

/**
 *  AmbaRTSL_SpiNandGetFeature - send command 0xf to the SPI Nand status register
 *  @param[in] FeatureAddr
 *  @param[out] pDeviceID ID byte sequence (at most five bytes)
 *  @return error code
 */
void AmbaRTSL_SpiNandGetFeature(UINT8 FeatureAddr, UINT8 *pStatus)
{
    AmbaCSL_SpiNandSendGetFeature(FeatureAddr);
    AmbaRTSL_SpiNandWaitCmdDone();      /* Wait for NAND command done */
    AmbaCSL_SpiNandGetCustomCmdData(1U, pStatus);
}

/**
 *  AmbaRTSL_SpiNandReadStart - Read pages from SpiNAND flash Start
 *  @param[in] PageAddr The first page address to read
 *  @param[in] NumPage Number of pages to read
 *  @param[out] pMainBuf pointer to DRAM buffer for main area data
 *  @param[out] pSpareBuf pointer to DRAM buffer for spare area data
 *  @return error code
 */
UINT32 AmbaRTSL_SpiNandReadStart(UINT32 PageAddr, UINT32 NumPage, UINT8 *pMainBuf, UINT8 *pSpareBuf)
{
    const AMBA_SPINAND_DEV_INFO_s *pNandDevInfo = AmbaRTSL_SpiNandDevInfo;
    UINT64 SrcAddr;
    UINT32 AddrHi, AddrLow;
    UINT32 RetVal;
    AMBA_NAND_CTRL_REG_s NandCtrl = {0U};

    if ((NumPage == 0U) || ((pMainBuf == NULL) && (pSpareBuf == NULL))) {
        RetVal = SPINAND_ERR_ARG;  /* wrong parameter */
    } else {

        AmbaCSL_FioClearIrqStatus();

        AmbaMisra_TypeCast32(&NandCtrl, &AmbaRTSL_NandCtrl.NandCtrlRegVal);
        AmbaCSL_NandSetCtrlReg(NandCtrl);

        /* Enable Bose-Chaudhuri-Hocquenghem (BCH) error correction (6-bit/8-bit) */
        if (AmbaRTSL_NandCtrl.NumEccBit != 0U) {
            AmbaCSL_NandEnableBCH(AmbaRTSL_NandCtrl.NumEccBit);
        } else {
            AmbaCSL_NandDisableBCH(pNandDevInfo->MainByteSize, pNandDevInfo->SpareByteSize);
        }

        /* FDMA read data from FIFO to DRAM */
        RetVal = AmbaRTSL_FdmaRead(PageAddr, NumPage, pMainBuf, pSpareBuf);
        if (RetVal == OK) {
            /* Page Read command */
            SrcAddr = (UINT64)PageAddr * pNandDevInfo->MainByteSize;

            AddrHi  = (UINT32)(SrcAddr >> 32U);
            AddrLow = (UINT32) (PageAddr * pNandDevInfo->MainByteSize);

            AmbaCSL_SpiNandSetupReadCmd(pNandDevInfo->Read, pNandDevInfo->PageCycles,
                                        pNandDevInfo->ColumnCycles, AddrHi, AddrLow);
        }
    }
    return RetVal;
}

/**
 *  AmbaRTSL_SpiNandRead - Read pages from SpiNAND flash
 *  @param[in] PageAddr The first page address to read
 *  @param[in] NumPage Number of pages to read
 *  @param[in] pMainBuf pointer to DRAM buffer for main area data
 *  @param[in] pSpareBuf pointer to DRAM buffer for spare area data
 *  @return error code
 */
UINT32 AmbaRTSL_SpiNandRead(UINT32 PageAddr, UINT32 NumPage, UINT8 *pMainBuf, UINT8 *pSpareBuf)
{
    UINT32 RetVal = AmbaRTSL_SpiNandReadStart(PageAddr, NumPage, pMainBuf, pSpareBuf);
    const AMBA_RTSL_FDMA_CTRL_s *pFdmaCtrl = (AMBA_RTSL_FDMA_CTRL_s *) &AmbaRTSL_FioCtrl.FdmaCtrl;

    if (RetVal == OK) {
        /* Wait for NAND command done */
        AmbaRTSL_SpiNandWaitCmdDone();

        RetVal = AmbaRTSL_SpiNandCheckDeviceStatus(NumPage, AMBA_SPINAND_ERROR_UNCORRECTABLE_ECC);
        if (RetVal == OK) {
            /* AmbaRTSL_CacheInvalidateDataCacheRange(pFdmaCtrl->pMainBuf, pFdmaCtrl->MainByteCount);*/
            /* AmbaRTSL_CacheInvalidateDataCacheRange(pFdmaCtrl->pSpareBuf, pFdmaCtrl->SpareByteCount);*/

            /* Copy back through DMA if output buf is not 8-Byte aligned. */
            if ((pMainBuf != NULL) && (pMainBuf != pFdmaCtrl->pMainBuf)) {
                if (AmbaWrap_memcpy(pMainBuf, pFdmaCtrl->pMainBuf, pFdmaCtrl->MainByteCount) != OK) { /* Do nothing */ };
            }

            if ((pSpareBuf != NULL) && (pSpareBuf != pFdmaCtrl->pSpareBuf)) {
                if (AmbaWrap_memcpy(pSpareBuf, pFdmaCtrl->pSpareBuf, pFdmaCtrl->SpareByteCount) != OK) { /* Do nothing */ };
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaRTSL_SpiNandProgramStart - Program SpiNAND flash memory Start
 *  @param[in] PageAddr The first page address to read
 *  @param[in] NumPage Number of pages to read
 *  @param[in] pMainBuf pointer to DRAM buffer for main area data
 *  @param[in] pSpareBuf pointer to DRAM buffer for spare area data
 *  @return error code
 */
UINT32 AmbaRTSL_SpiNandProgramStart(UINT32 PageAddr, UINT32 NumPage, UINT8 *pMainBuf, UINT8 *pSpareBuf)
{
    const AMBA_SPINAND_DEV_INFO_s *pNandDevInfo = AmbaRTSL_SpiNandDevInfo;
    UINT64 SrcAddr;
    UINT32 AddrHi, AddrLow;
    AMBA_NAND_CTRL_REG_s NandCtrl = {0U};
    UINT32 RetVal;

    AmbaCSL_FioClearIrqStatus();

    AmbaMisra_TypeCast32(&NandCtrl, &AmbaRTSL_NandCtrl.NandCtrlRegVal);
    AmbaCSL_NandSetCtrlReg(NandCtrl);

    /* Enable Bose-Chaudhuri-Hocquenghem (BCH) error correction (6-bit/8-bit) */
    if(AmbaRTSL_NandCtrl.NumEccBit != 0U) {
        AmbaCSL_NandEnableBCH(AmbaRTSL_NandCtrl.NumEccBit);
    } else {
        AmbaCSL_NandDisableBCH(pNandDevInfo->MainByteSize, pNandDevInfo->SpareByteSize);
    }

    /* FDMA write data from DRAM to FIFO */
    RetVal = AmbaRTSL_FdmaWrite(PageAddr, NumPage, pMainBuf, pSpareBuf);
    if (RetVal == OK) {

        /* Page Program command */
        SrcAddr = (UINT64)PageAddr * pNandDevInfo->MainByteSize;
        AddrHi  = (UINT32)(SrcAddr >> 32U);
        AddrLow = (UINT32)PageAddr * pNandDevInfo->MainByteSize;

        AmbaCSL_SpiNandSetupProgramCmd(pNandDevInfo->Program, pNandDevInfo->PageCycles,
                                       pNandDevInfo->ColumnCycles, AddrHi, AddrLow);
    }
    return RetVal;
}

/**
 *  AmbaRTSL_SpiNandProgram - Program NAND flash memory
 *  @param[in] PageAddr The first page address to read
 *  @param[in] NumPage Number of pages to read
 *  @param[in] pMainBuf pointer to DRAM buffer for main area data
 *  @param[in] pSpareBuf pointer to DRAM buffer for spare area data
 *  @return error code
 */
UINT32 AmbaRTSL_SpiNandProgram(UINT32 PageAddr, UINT32 NumPage, UINT8 *pMainBuf, UINT8 *pSpareBuf)
{
    UINT32 RetVal;

    RetVal = AmbaRTSL_SpiNandProgramStart(PageAddr, NumPage, pMainBuf, pSpareBuf);
    if (RetVal == OK) {
        /* Wait for NAND command done */
        AmbaRTSL_SpiNandWaitCmdDone();

        RetVal = AmbaRTSL_SpiNandCheckDeviceStatus(NumPage, AMBA_SPINAND_ERROR_PROGRAM_FAIL);
    }

    return RetVal;
}

/**
 *  AmbaRTSL_NandCheckDeviceStatus - Check Nand Controller status after cmd done
 *  @return error code
 */
#define AMBA_SPINAND_IRQ_ERROR_PATTERN  0x6U
UINT32 AmbaRTSL_SpiNandCheckDeviceStatus(UINT32 NumPage, UINT8 ErrorPattern)
{
    UINT32 RetVal = SPINAND_ERR_NONE;

    AmbaMisra_TouchUnused(&NumPage);

    if (AmbaRTSL_FioCheckIrqStatus(AMBA_SPINAND_IRQ_ERROR_PATTERN) != 0) {
        RetVal = NAND_ERR_IO_FAIL;
    } else {
        AMBA_SPINAND_CMDSTATUS_REG_s Status;

        UINT32 StatusTmp = 0;

        AmbaCSL_NandGetCmdResponse((UINT8 *)&Status);
        AmbaMisra_TypeCast32(&StatusTmp, &Status);

        /* Check NAND cmd Response */
        if (AmbaRTSL_NandCtrl.NumEccBit != 0U) {
            if (ErrorPattern != AMBA_SPINAND_ERROR_UNCORRECTABLE_ECC) {
                if ((StatusTmp & ErrorPattern) != 0U) {
                    RetVal = NAND_ERR_ECC;
                }
            }

            if ((RetVal == SPINAND_ERR_NONE) &&
                (ErrorPattern == AMBA_SPINAND_ERROR_UNCORRECTABLE_ECC)) { /* Check ECC status */
                if (AmbaRTSL_FioCheckEccStatus() != 0) {
                    RetVal = NAND_ERR_ECC;
                }
            }
        } else {
            if (ErrorPattern == AMBA_SPINAND_ERROR_UNCORRECTABLE_ECC) {
                if ((Status.ECC & AMBA_SPINAND_ONDEVICE_ECC_ERR) != 0U) {
                    RetVal = NAND_ERR_ECC;
                }
            } else {
                if ((StatusTmp & ErrorPattern) != 0U) {
                    RetVal = NAND_ERR_IO_FAIL;
                }
            }
        }
    }

    AmbaCSL_FioClearIrqStatus();
    AmbaCSL_SpiNandClearStatus();

    return RetVal;
}

/**
 *  AmbaRTSL_SpiNandHookApi -
 */
static void AmbaRTSL_SpiNandHookApi(void)
{
    AmbaRTSL_NandOpCopyback   = NULL;
    AmbaRTSL_NandOpEraseBlock = AmbaRTSL_SpiNandEraseBlock;
    AmbaRTSL_NandOpRead       = AmbaRTSL_SpiNandRead;
    AmbaRTSL_NandOpProgram    = AmbaRTSL_SpiNandProgram;
}

