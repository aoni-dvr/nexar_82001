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
#ifndef CONFIG_QNX
#include "AmbaRTSL_Cache.h"
#endif
#include "AmbaRTSL_PLL.h"
#include "AmbaRTSL_FIO.h"
#include "AmbaRTSL_NAND.h"
#include "AmbaRTSL_NAND_OP.h"
#include "AmbaRTSL_NAND_Ctrl.h"
#include "AmbaCSL_FIO.h"
#include "AmbaCSL_NAND.h"

AMBA_RTSL_NAND_CTRL_s AmbaRTSL_NandCtrl;

static void AmbaRTSL_NandHookApi(void);

#ifdef CONFIG_QNX
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
#define AmbaRTSL_PllGetCortex1Clk() (528000000U)
#else
#define AmbaRTSL_PllGetCortexClk()  (756000000U)
#endif
#endif

/*
 *  @RoutineName:: AmbaRTSL_NandInit
 *
 *  @Description:: Initialize the NAND flash controller
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
UINT32 AmbaRTSL_NandInit(void)
{
    if (0U != AmbaWrap_memset(&AmbaRTSL_NandCtrl, 0, sizeof(AMBA_RTSL_NAND_CTRL_s))) { /* Misrac */ };

    if (0U != (AmbaRTSL_FioIsRandomReadMode())) {
        AmbaRTSL_FioReset();
        AmbaCSL_FioDmaFifoModeEnable(); /* Enable DMA Mode for FIO-DMA FIFO */
    }
    AmbaCSL_FioSelectParallelNand();/* Use parallel NAND interface */
    AmbaCSL_FioInterruptEnable();   /* Enable CMD done & ECC interrupt */

    AmbaRTSL_NandReset();           /* NAND Device Reset */

    AmbaRTSL_NandHookApi();

    return OK;
}

/*
 *  @RoutineName:: AmbaRTSL_FioIsRandomMode
 *
 *  @Description:: Check in random read mode or fifo mode.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *                 1: Random read mode. 0: FIFO mode
 */
UINT32 AmbaRTSL_FioIsRandomReadMode(void)
{
    /* Check random read mode */
    return AmbaCSL_FioIsRandomReadMode();
}

/*
 *  @RoutineName:: AmbaRTSL_NandWaitCmdDone
 *
 *  @Description:: Wait for NAND command done
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
static void AmbaRTSL_NandWaitCmdDone(void)
{
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
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

/*
 *  @RoutineName:: AmbaRTSL_NandReadID
 *
 *  @Description:: To retrieve the ONFI signature or JEDEC manufacturer ID and device ID
 *
 *  @Input      ::
 *      NumReadCycle: number of read cycles
 *
 *  @Output     ::
 *      pDeviceID: ID byte sequence (at most five bytes)
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaRTSL_NandReadID(UINT32 NumReadCycle, UINT8 *pDeviceID)
{
    UINT32 RetVal = NAND_ERR_NONE;

    if ((NumReadCycle == 0U) || (NumReadCycle > 5U) || (pDeviceID == NULL)) {
        RetVal = NAND_ERR_ARG;  /* wrong parameters */
    } else {
        AmbaCSL_NandSendReadIdCmd(NumReadCycle);
        AmbaRTSL_NandWaitCmdDone();
        AmbaCSL_NandGetReadIdResponse(NumReadCycle, pDeviceID);
    }
    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_NandConfig
 *
 *  @Description:: NAND software configurations
 *
 *  @Input      ::
 *      pNandConfig: pointer to NAND software configurations
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaRTSL_NandConfig(AMBA_NAND_CONFIG_s *pNandConfig)
{
    UINT32 RetVal = NAND_ERR_NONE;
    AMBA_NAND_CTRL_REG_s NandCtrl = {0U};
    AMBA_NAND_DEV_INFO_s *pNandDevInfo;
    AMBA_PARTITION_CONFIG_s *pUserPartConfig;
    UINT32 SpareByteSize;
    UINT32 MainByteSize;
    UINT32 NumEccBit, k, Tmp32;
    UINT8  Tmp8 = 0;

    AmbaMisra_TouchUnused(pNandConfig);

    if ((pNandConfig == NULL) || (pNandConfig->pNandDevInfo == NULL) || (pNandConfig->pUserPartConfig == NULL)) {
        RetVal = NAND_ERR_ARG;  /* wrong parameters */
    } else {

        pNandDevInfo    = pNandConfig->pNandDevInfo;
        pUserPartConfig = pNandConfig->pUserPartConfig;
        SpareByteSize   = pNandDevInfo->SpareByteSize;
        MainByteSize    = pNandDevInfo->MainByteSize;

        AmbaRTSL_NandCtrl.NandInfo.BlockPageSize  = pNandDevInfo->BlockPageSize;
        AmbaRTSL_NandCtrl.NandInfo.MainByteSize   = pNandDevInfo->MainByteSize;
        AmbaRTSL_NandCtrl.NandInfo.PlaneAddrMask  = pNandDevInfo->PlaneAddrMask;
        AmbaRTSL_NandCtrl.NandInfo.PlaneBlockSize = pNandDevInfo->PlaneBlockSize;
        AmbaRTSL_NandCtrl.NandInfo.SpareByteSize  = pNandDevInfo->SpareByteSize;
        AmbaRTSL_NandCtrl.NandInfo.TotalPlanes    = pNandDevInfo->TotalPlanes;
        AmbaRTSL_NandCtrl.NandInfo.TotalZones     = pNandDevInfo->TotalZones;
        AmbaRTSL_NandCtrl.NandInfo.ZoneBlockSize  = pNandDevInfo->ZoneBlockSize;

        AmbaRTSL_NandCtrl.DevInfo.pNandDevInfo    = pNandDevInfo;         /* save the pointer to NAND Device Information */

        AmbaRTSL_NandCtrl.pSysPartConfig  = pNandConfig->pSysPartConfig;   /* pointer to System partition configurations */
        AmbaRTSL_NandCtrl.pUserPartConfig = pUserPartConfig;               /* save the pointer to User Partition Configurations */
        AmbaRTSL_NandSetTiming(pNandDevInfo);       /* Set timing parameters for the NAND flash device */

        if (MainByteSize > 2048U) {
            AmbaCSL_NandSetPageSize4KB();
        } else {
            AmbaCSL_NandDisablePageSize4KB();
        }

        if (pNandDevInfo->ChipSize > AMBA_NAND_SIZE_8G_BITS) {
            AmbaCSL_NandSetChipSizeOver8Gb();
        }

        k = MainByteSize / 512U;
#if 0
        if (SpareByteSize < (k * 16U)) {
            /* BBM: small page (OOB size < 64 Bytes; Page size <= 512 Bytes), check the 5th Byte */
            AmbaRTSL_NandCtrl.BadBlkMarkOffset   = 5;  /* the offset of Bad Block Mark */
            AmbaRTSL_NandCtrl.BbtSignatureOffset = 0;  /* the offset of BBT Signature */
            AmbaRTSL_NandCtrl.BbtVersionOffset   = 12; /* the offset of BBT Version */
            return NG;      /* Small page does not support multi-bits BCH */
        }
#endif
        /* BBM: large page (OOB size >= 64 Bytes; Page size >= 2K Bytes), check the 0th Byte */
        AmbaRTSL_NandCtrl.BadBlkMarkOffset = 0;        /* the offset of Bad Block Mark */

        if (SpareByteSize >= (32U * k)) {
            NumEccBit = 8U;  /* Bose-Chaudhuri-Hocquenghem (BCH) error correction (8-bit) */
            AmbaRTSL_NandCtrl.BbtSignatureOffset = 2U;  /* the offset of BBT Signature */
            AmbaRTSL_NandCtrl.BbtVersionOffset   = 8U;  /* the offset of BBT Version */
            AmbaCSL_NandEnableSpare2xArea();
        } else {
            NumEccBit = 6U;  /* Bose-Chaudhuri-Hocquenghem (BCH) error correction (6-bit) */
            AmbaRTSL_NandCtrl.BbtSignatureOffset = 1U;  /* the offset of BBT Signature */
            AmbaRTSL_NandCtrl.BbtVersionOffset   = 18U; /* the offset of BBT Version */
        }

        Tmp32 = pNandDevInfo->ChipSize & AMBA_NAND_SIZE_8G_BITS;
        if (0U != AmbaWrap_memcpy(&Tmp8, &Tmp32, sizeof(Tmp8))) { /* Misrac */ };
        NandCtrl.ChipSize                  = Tmp8;

        NandCtrl.PageAddr3Cycle            = 1U;     /* 1 - Page Address in 3 Cycles */
        NandCtrl.Addr33_32                 = (pNandDevInfo->ChipSize > AMBA_NAND_SIZE_4G_BITS) ? 1U : 0U;
        NandCtrl.WriteWithAutoStatusUpdate = 1U;
        NandCtrl.WriteProtectEnable        = 1U;

        AmbaCSL_NandSetCtrlReg(NandCtrl);

        /* Enable Bose-Chaudhuri-Hocquenghem (BCH) error correction (6-bit/8-bit) */
        AmbaCSL_NandEnableBCH(NumEccBit);

        AmbaMisra_TypeCast32(&AmbaRTSL_NandCtrl.NandCtrlRegVal, &NandCtrl);

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
        AmbaRTSL_NandCtrl.BbtPageCount = GetRoundUpValU32(GetRoundUpValU32(AmbaRTSL_NandCtrl.TotalNumBlk, 4), MainByteSize);
    }
    return RetVal;
}

static UINT8 GetNandCycleCount(UINT32 Val0)
{
    UINT32 ValU32;
    UINT8 ValU8;

    ValU32 = (((Val0) > (1U)) ? (Val0) : (1U));

    if (0U != AmbaWrap_memcpy(&ValU8, &ValU32, sizeof(ValU8))) { /* Misrac */ };
    ValU8 -= 1U;

    return ValU8;
}

/*
 *  @RoutineName:: AmbaRTSL_NandSetTiming
 *
 *  @Description:: Set timing parameters for the NAND flash device
 *
 *  @Input      ::
 *      pNandDevInfo: pointer to NAND flash device information
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
void AmbaRTSL_NandSetTiming(const AMBA_NAND_DEV_INFO_s *pNandDevInfo)
{
    UINT32 NandClkFreq; /* Unit: MHz */
    AMBA_NAND_TIMING_CTRL_s NandTiming = {0U};
    UINT32 tREH = pNandDevInfo->tREH, tALH;
    UINT8 TMP;

    NandClkFreq = AmbaRTSL_PllGetNandClk() / 1000000U;

    if ((pNandDevInfo->tRP + pNandDevInfo->tREH) < pNandDevInfo->tRC) { /* tRP + tREH >= tRC */
        tREH = (UINT32)pNandDevInfo->tRC - pNandDevInfo->tRP;
    }

    tALH = (UINT32)GetMaxValU8((pNandDevInfo->tADL - pNandDevInfo->tWP), pNandDevInfo->tALH); /* tALH + tWP >= tADL */

    NandTiming.Timing0.tCLS = GetNandCycleCount(GetRoundUpValU32(NandClkFreq * pNandDevInfo->tCLS, 1000U));

    NandTiming.Timing0.tALS = GetNandCycleCount(GetRoundUpValU32(NandClkFreq * pNandDevInfo->tALS, 1000U));
    NandTiming.Timing0.tCS  = GetNandCycleCount(GetRoundUpValU32(NandClkFreq * pNandDevInfo->tCS,  1000U));
    NandTiming.Timing0.tDS  = GetNandCycleCount(GetRoundUpValU32(NandClkFreq * pNandDevInfo->tDS,  1000U));

    NandTiming.Timing1.tCLH = GetNandCycleCount(GetRoundUpValU32(NandClkFreq * pNandDevInfo->tCLH, 1000U));
    NandTiming.Timing1.tALH = GetNandCycleCount(GetRoundUpValU32(NandClkFreq * tALH, 1000U));
    NandTiming.Timing1.tCH  = GetNandCycleCount(GetRoundUpValU32(NandClkFreq * pNandDevInfo->tCH,  1000U));
    NandTiming.Timing1.tDH  = GetNandCycleCount(GetRoundUpValU32(NandClkFreq * pNandDevInfo->tDH,  1000U));

    NandTiming.Timing2.tWP  = GetNandCycleCount(GetRoundUpValU32(NandClkFreq * pNandDevInfo->tWP,  1000U));
    NandTiming.Timing2.tWH  = GetNandCycleCount(GetRoundUpValU32(NandClkFreq * pNandDevInfo->tWH,  1000U));
    NandTiming.Timing2.tWB  = GetNandCycleCount(GetRoundUpValU32(NandClkFreq * pNandDevInfo->tWB,  1000U));
    NandTiming.Timing2.tRR  = GetNandCycleCount(GetRoundUpValU32(NandClkFreq * pNandDevInfo->tRR,  1000U));

    NandTiming.Timing3.tRP  = GetNandCycleCount(GetRoundUpValU32(NandClkFreq * pNandDevInfo->tRP,  1000U));
    NandTiming.Timing3.tREH = GetNandCycleCount(GetRoundUpValU32(NandClkFreq * tREH, 1000U));
    NandTiming.Timing3.tRB  = GetNandCycleCount(GetRoundUpValU32(NandClkFreq * pNandDevInfo->tRB,  1000U));
    NandTiming.Timing3.tCEH = GetNandCycleCount(GetRoundUpValU32(NandClkFreq * pNandDevInfo->tCEH, 1000U));

    NandTiming.Timing4.tCLR = GetNandCycleCount(GetRoundUpValU32(NandClkFreq * pNandDevInfo->tCLR, 1000U));
    NandTiming.Timing4.tWHR = GetNandCycleCount(GetRoundUpValU32(NandClkFreq * pNandDevInfo->tWHR, 1000U));
    NandTiming.Timing4.tIR  = GetNandCycleCount(GetRoundUpValU32(NandClkFreq * pNandDevInfo->tIR,  1000U));

    NandTiming.Timing5.tWW  = GetNandCycleCount(GetRoundUpValU32(NandClkFreq * pNandDevInfo->tWW,  1000U));
    NandTiming.Timing5.tAR  = GetNandCycleCount(GetRoundUpValU32(NandClkFreq * pNandDevInfo->tAR,  1000U));

    NandTiming.Timing5.tRHZ = GetNandCycleCount(GetRoundDownValU32(NandClkFreq * pNandDevInfo->tRHZ, 1000U));

    NandTiming.Timing6.tCRL = GetNandCycleCount(GetRoundUpValU32(NandClkFreq * pNandDevInfo->tCRL, 1000U));
    NandTiming.Timing6.tADL = GetNandCycleCount(GetRoundUpValU32(NandClkFreq * pNandDevInfo->tADL, 1000U));
    NandTiming.Timing6.tRHW = GetNandCycleCount(GetRoundUpValU32(NandClkFreq * pNandDevInfo->tRHW, 1000U));

    TMP = (UINT8)(NandTiming.Timing3.tRP & 0xffU) + (UINT8)(NandTiming.Timing3.tREH & 0xffU);
    TMP += 1U;
    NandTiming.Timing4.tRDELAY = TMP;

    if (NandTiming.Timing5.tRHZ < NandTiming.Timing4.tRDELAY) {
        NandTiming.Timing4.tRDELAY = NandTiming.Timing5.tRHZ;
    }

    AmbaCSL_NandSetTiming(&NandTiming);
}

/*
 *  @RoutineName:: AmbaRTSL_NandReset
 *
 *  @Description:: Put the target in its default power-up state and in SDR data interface
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
void AmbaRTSL_NandReset(void)
{
    AmbaCSL_NandSendResetCmd();         /* send the command */
    AmbaRTSL_NandWaitCmdDone();         /* Wait for NAND command done */
}
#if 0
/*
 *  @RoutineName:: AmbaRTSL_NandReadStatus
 *
 *  @Description:: Retrieve a status value for the last command issued
 *
 *  @Input      :: none
 *
 *  @Output     ::
 *      pStatus: Pointer to nand device status
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaRTSL_NandReadStatus(AMBA_NAND_STATUS_u *pStatus)
{
    UINT32 RetVal = NAND_ERR_NONE;

    if (pStatus == NULL) {
        RetVal = NAND_ERR_ARG;  /* wrong parameters */
    } else {
        AmbaCSL_NandSendReadStatusCmd();    /* send the command */
        AmbaCSL_NandWaitCmdDone();          /* Wait for NAND command done */
        AmbaCSL_NandGetCmdResponse((UINT8 *) pStatus);
    }
    return RetVal;
}
#endif
/*
 *  @RoutineName:: AmbaRTSL_NandCopyback
 *
 *  @Description:: Read a page of data from one location and then move that data to a second location
 *
 *  @Input      ::
 *      SrcPageAddr:    Source page address
 *      DstPageAddr:    Destination page address
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaRTSL_NandCopyback(UINT32 SrcPageAddr, UINT32 DestPageAddr)
{
#if 0
    AMBA_NAND_DEV_INFO_s *pNandDevInfo = AmbaRTSL_NandDevInfo;
    UINT32 MainByteSize = pNandDevInfo->MainByteSize;

    AmbaCSL_NandDisableWriteProtect();

    AmbaCSL_NandSendCopyBackCmd((UINT64)SrcPageAddr * MainByteSize, (UINT64)DestPageAddr * MainByteSize);
    AmbaCSL_NandWaitCmdDone();      /* Wait for NAND command done */

    AmbaCSL_NandEnableWriteProtect();
#else
    (void)SrcPageAddr;
    (void)DestPageAddr;
#endif
    return 0;
}

/*
 *  @RoutineName:: AmbaRTSL_NandEraseBlock
 *
 *  @Description:: To erase the block of data identified by the block address parameter
 *
 *  @Input      ::
 *      BlkAddr:  Block address
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaRTSL_NandEraseBlock(UINT32 BlkAddr)
{
    UINT32 Rval;
    AmbaCSL_NandDisableWriteProtect();

    AmbaCSL_NandSendBlockEraseCmd(BlkAddr * AmbaRTSL_NandCtrl.BlkByteSize);
    AmbaRTSL_NandWaitCmdDone(); /* Wait for NAND command done */
    AmbaCSL_NandGetCmdResponse((UINT8 *) & (AmbaRTSL_NandCtrl.Status[0]));

    AmbaCSL_NandEnableWriteProtect();

    if (AmbaRTSL_NandCtrl.Status[0].Bits[0].LastCmdFailed != 0U) {
        Rval = NAND_ERR_IO_FAIL;
    } else {
        Rval = NAND_ERR_NONE;
    }
    return Rval;
}

/*
 *  @RoutineName:: AmbaRTSL_NandReadStart
 *
 *  @Description:: Read pages from NAND flash Start
 *
 *  @Input      ::
 *      PageAddr:  The first page address to read
 *      NumPage:   Number of pages to read
 *      pMainBuf:  pointer to DRAM buffer for main area data
 *      pSpareBuf: pointer to DRAM buffer for spare area data
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaRTSL_NandReadStart(UINT32 PageAddr, UINT32 NumPage, UINT8 *pMainBuf, UINT8 *pSpareBuf)
{
    UINT32 RetVal = NAND_ERR_NONE;
    AMBA_NAND_CMD_REG_s   NandCmdRegVal = {0};
    UINT64                SrcAddr, SrcShiftTmp;
    UINT32                Addr31_4 = 0;
    UINT8 Addr33_32 = 0;
    const AMBA_NAND_DEV_INFO_s *pNandDevInfo  =  AmbaRTSL_NandDevInfo;

    if ((NumPage == 0U) || ((pMainBuf == NULL) && (pSpareBuf == NULL)) ) {
        RetVal = NAND_ERR_ARG;  /* wrong parameter */
    } else {
        AMBA_NAND_CTRL_REG_s NandCtrl = {0U};

        AmbaCSL_FioClearIrqStatus(); /* clear NAND interrupt status */

        AmbaMisra_TypeCast32(&NandCtrl, &AmbaRTSL_NandCtrl.NandCtrlRegVal);
        AmbaCSL_NandSetCtrlReg(NandCtrl);
        /* Enable Bose-Chaudhuri-Hocquenghem (BCH) error correction (6-bit/8-bit) */
        AmbaCSL_NandEnableBCH(AmbaRTSL_NandCtrl.NumEccBit);

        /* FDMA read data from FIFO to DRAM */
        (void)AmbaRTSL_FdmaRead(PageAddr, NumPage, pMainBuf, pSpareBuf);

        SrcAddr   = (UINT64)PageAddr * pNandDevInfo->MainByteSize;

        SrcShiftTmp = SrcAddr >> 4U;
        AmbaMisra_TypeCast32(&Addr31_4, &SrcShiftTmp);

        SrcShiftTmp = SrcAddr >> 32U;
        if (0U != AmbaWrap_memcpy(&Addr33_32, &SrcShiftTmp, sizeof(Addr33_32))) { /* Misrac */ };

        AmbaCSL_NandSetAddress33_32(Addr33_32);

        NandCmdRegVal.Addr    = Addr31_4;

        NandCmdRegVal.CmdCode = AMBA_NAND_CMD_READ;

#ifndef USE_FIODMA_DESCRIPTORS
        /* Page Read command */
        AmbaCSL_NandSetCmdReg(NandCmdRegVal);
#endif
    }
    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_NandRead
 *
 *  @Description:: Read pages from NAND flash
 *
 *  @Input      ::
 *      PageAddr:  The first page address to read
 *      NumPage:   Number of pages to read
 *      pMainBuf:  pointer to DRAM buffer for main area data
 *      pSpareBuf: pointer to DRAM buffer for spare area data
 *
 *  @Output     ::
 *      pMainBuf:  pointer to main area data
 *      pSpareBuf: pointer to spare area data
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaRTSL_NandRead(UINT32 PageAddr, UINT32 NumPage, UINT8 *pMainBuf, UINT8 *pSpareBuf)
{
    UINT32 RetVal;
    const AMBA_RTSL_FDMA_CTRL_s *pFdmaCtrl = &AmbaRTSL_FioCtrl.FdmaCtrl;
    ULONG MainAddr = 0x0, SpareAddr = 0x0;

    RetVal = AmbaRTSL_NandReadStart(PageAddr, NumPage, pMainBuf, pSpareBuf);

    if (RetVal == OK) {
        /* Wait for NAND command done */
        AmbaRTSL_NandWaitCmdDone();

        /* Wait fo FDMA operation done */
        AmbaRTSL_FdmaGetStatus();

        if (AmbaRTSL_NandCheckDeviceStatus(NumPage) != OK) {
            RetVal = NAND_ERR_ECC;
        } else {

            AmbaMisra_TypeCast(&MainAddr,  &pFdmaCtrl->pMainBuf);
            AmbaMisra_TypeCast(&SpareAddr, &pFdmaCtrl->pSpareBuf);
#ifndef CONFIG_QNX
            (void)AmbaRTSL_CacheInvalData(MainAddr, pFdmaCtrl->MainByteCount);
            (void)AmbaRTSL_CacheInvalData(SpareAddr, pFdmaCtrl->SpareByteCount);
#endif
            /* Copy back through DMA if output buf is not 8-Byte aligned. */
            if ((pMainBuf != NULL) && (pMainBuf != pFdmaCtrl->pMainBuf)) {
                if (0U != AmbaWrap_memcpy(pMainBuf, pFdmaCtrl->pMainBuf, pFdmaCtrl->MainByteCount)) { /* Misrac */ };
            }

            if ((pSpareBuf != NULL) && (pSpareBuf != pFdmaCtrl->pSpareBuf)) {
                if (0U != AmbaWrap_memcpy(pSpareBuf, pFdmaCtrl->pSpareBuf, pFdmaCtrl->SpareByteCount)) { /* Misrac */ };
            }
        }
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_NandProgramStart
 *
 *  @Description:: Program NAND flash memory Start
 *
 *  @Input      ::
 *      PageAddr:  The first page address to write
 *      NumPage:   Number of pages to write
 *      pMainBuf:  pointer to DRAM buffer for main area data
 *      pSpareBuf: pointer to DRAM buffer for spare area data
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
void AmbaRTSL_NandProgramStart(UINT32 PageAddr, UINT32 NumPage, UINT8 *pMainBuf, UINT8 *pSpareBuf)
{
    AMBA_NAND_CMD_REG_s   NandCmdRegVal = {0};
    const AMBA_NAND_DEV_INFO_s *pNandDevInfo = AmbaRTSL_NandDevInfo;

    UINT64 SrcAddr, SrcShiftTmp;
    UINT32 Addr31_4 = 0;
    UINT8  Addr33_32 = 0;
    AMBA_NAND_CTRL_REG_s NandCtrl = {0U};

    AmbaCSL_FioClearIrqStatus();  /* clear NAND interrupt status */

    AmbaMisra_TypeCast32(&NandCtrl, &AmbaRTSL_NandCtrl.NandCtrlRegVal);
    AmbaCSL_NandSetCtrlReg(NandCtrl);

    /* Enable Bose-Chaudhuri-Hocquenghem (BCH) error correction (6-bit/8-bit) */
    AmbaCSL_NandEnableBCH(AmbaRTSL_NandCtrl.NumEccBit);

    AmbaCSL_NandDisableWriteProtect();

    /* FDMA write data from DRAM to FIFO */
    (void)AmbaRTSL_FdmaWrite(PageAddr, NumPage, pMainBuf, pSpareBuf);

    SrcAddr = (UINT64)PageAddr * pNandDevInfo->MainByteSize;

    SrcShiftTmp = SrcAddr >> 4U;
    AmbaMisra_TypeCast32(&Addr31_4, &SrcShiftTmp);

    SrcShiftTmp = SrcAddr >> 32U;
    if (0U != AmbaWrap_memcpy(&Addr33_32, &SrcShiftTmp, sizeof(Addr33_32))) { /* Misrac */ };

    AmbaCSL_NandSetAddress33_32(Addr33_32);

    NandCmdRegVal.Addr    = Addr31_4;

    NandCmdRegVal.CmdCode = AMBA_NAND_CMD_PAGE_PROGRAM;

#ifndef USE_FIODMA_DESCRIPTORS
    /* Page program command */
    AmbaCSL_NandSetCmdReg(NandCmdRegVal);
#endif
}

/*
 *  @RoutineName:: AmbaRTSL_NandProgram
 *
 *  @Description:: Program NAND flash memory
 *
 *  @Input      ::
 *      PageAddr:  The first page address to write
 *      NumPage:   Number of pages to write
 *      pMainBuf:  pointer to DRAM buffer for main area data
 *      pSpareBuf: pointer to DRAM buffer for spare area data
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaRTSL_NandProgram(UINT32 PageAddr, UINT32 NumPage, UINT8 *pMainBuf, UINT8 *pSpareBuf)
{
    UINT32 RetVal;

    AmbaRTSL_NandProgramStart(PageAddr, NumPage, pMainBuf, pSpareBuf);

    /* Wait for NAND command done */
    AmbaRTSL_NandWaitCmdDone();

    AmbaCSL_NandGetCmdResponse((UINT8 *) & (AmbaRTSL_NandCtrl.Status[0]));

    if (AmbaRTSL_NandCtrl.Status[0].Bits[0].LastCmdFailed != 0U) {
        RetVal = NAND_ERR_IO_FAIL;
    } else {
        RetVal = NAND_ERR_NONE;
    }

    /* Wait fo FDMA operation done */
    AmbaRTSL_FdmaGetStatus();

    if (AmbaRTSL_FioCheckEccStatus() != 0) {
        /* DEBUG_MSG("%s page:%d, pages: %d  fail\n", __func__, PageAddr, NumPage); */
        RetVal = NAND_ERR_ECC;
    }

    AmbaCSL_NandEnableWriteProtect();

    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_NandCheckDeviceStatus
 *
 *  @Description:: Check Nand Controller status after cmd done
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
//#define AMBA_BCH6_CODE_BYTE_COUNT    10
//#define AMBA_BCH8_CODE_BYTE_COUNT    13
UINT32 AmbaRTSL_NandCheckDeviceStatus(UINT32 NumPage)
{
    const AMBA_NAND_DEV_INFO_s *pNandDevInfo = AmbaRTSL_NandDevInfo;
    UINT32 RetVal = NAND_ERR_NONE;

    AmbaMisra_TouchUnused(&NumPage);

    if (pNandDevInfo == NULL) {
        RetVal = NAND_ERR_ARG;
    } else {
        /* Check NAND last cmd success or not */
        AmbaCSL_NandGetCmdResponse((UINT8 *) & (AmbaRTSL_NandCtrl.Status[0]));
        if ( AmbaRTSL_NandCtrl.Status[0].Bits[0].LastCmdFailed != 0U) {
            RetVal = NAND_ERR_IO_FAIL;
        } else {
            /* Check ECC status */
            if (AmbaRTSL_FioCheckEccStatus() != 0) {
#if 0
                UINT32 Sectors = pNandDevInfo->MainByteSize / 512U * NumPage;
                AMBA_RTSL_FDMA_CTRL_s *pFdmaCtrl = (AMBA_RTSL_FDMA_CTRL_s *) &AmbaRTSL_FioCtrl.FdmaCtrl;
                UINT32 BchCodeOffset = 6, BchCodeByteCount;
                UINT32 i, SecCnt;
                UINT32 SpareByteSize, MainByteSize;

                SpareByteSize = pNandDevInfo->SpareByteSize;
                MainByteSize  = pNandDevInfo->MainByteSize;

                /*
                 * If somthing wrong, check spare data to distinguish it's really fail or not
                 * If all data are 0xff, the BCH controller will report an error.
                 * This is a fake error.
                 */
                BchCodeByteCount = (SpareByteSize >= (32 * (MainByteSize / 512))) ? AMBA_BCH8_CODE_BYTE_COUNT : AMBA_BCH6_CODE_BYTE_COUNT;

                for (SecCnt = 0; SecCnt < Sectors; SecCnt++) {
                    UINT8 *pSpareBuf = &pFdmaCtrl->pSpareBuf[SecCnt * 16];
                    for (i = BchCodeOffset; i < BchCodeByteCount; i++) {
                        if (pSpareBuf[i] != 0xff) {
                            RetVal = NG;
                            break;
                        }
                        RetVal = OK;
                        return RetVal;
                    }
                }
#else
                RetVal = NAND_ERR_ECC;
#endif
            }
        }
    }
    return RetVal;
}

static void AmbaRTSL_NandHookApi(void)
{
    AmbaRTSL_NandOpCopyback   = AmbaRTSL_NandCopyback;
    AmbaRTSL_NandOpEraseBlock = AmbaRTSL_NandEraseBlock;
    AmbaRTSL_NandOpRead       = AmbaRTSL_NandRead;
    AmbaRTSL_NandOpProgram    = AmbaRTSL_NandProgram;
}

