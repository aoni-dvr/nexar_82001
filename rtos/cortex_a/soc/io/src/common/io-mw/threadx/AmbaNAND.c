/**
 *  @file AmbaNAND.c
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
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
 *  @details NAND Control APIs
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"

#include "AmbaNAND.h"
#include "AmbaNAND_Ctrl.h"
#include "AmbaNAND_OP.h"

#include "AmbaRTSL_Cache.h"
#include "AmbaRTSL_FIO.h"
#include "AmbaRTSL_NAND.h"
#include "AmbaRTSL_NAND_Ctrl.h"
#include "AmbaCSL_FIO.h"
#include "AmbaCSL_NAND.h"

/*
 * Definitions for EventFlag
 */
#define AMBA_FIO_NAND_CMD_DONE_FLAG     0x1U
#define DISABLE_COPYBACK

#define AMBA_FIO_EVENT_MASK (AMBA_FIO_NAND_CMD_DONE_FLAG)

typedef struct  {
    AMBA_KAL_EVENT_FLAG_t   EventFlag;  /* Event Flags */
    AMBA_KAL_MUTEX_t        Mutex;      /* Mutex */
} AMBA_NAND_CTRL_s;

static AMBA_NAND_CTRL_s AmbaNAND_Ctrl = {0};    /* NAND Management Structure */
static AMBA_KAL_MUTEX_t PseudoCB_Mutex;

static void NAND_FioNandCmdIsr(void);
static void AmbaNAND_HookApi(const AMBA_NAND_CONFIG_s *pNandConfig);

/*
 *  @RoutineName:: AmbaNAND_Lock
 *
 *  @Description:: NAND Lock
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaNAND_Lock(void)
{
    if(AmbaNandOp_PreAccessNotify != NULL) {
        AmbaNandOp_PreAccessNotify();
#if defined(CONFIG_ENABLE_AMBALINK) && defined(CONFIG_THREADX)
    } else {
        extern void AmbaIPC_NandLock(void);
        AmbaIPC_NandLock();
#endif
    }

    return AmbaKAL_MutexTake(&AmbaNAND_Ctrl.Mutex, 0xFFFFFFFFU);
}

/*
 *  @RoutineName:: AmbaNAND_Unlock
 *
 *  @Description:: Unlock NAND
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaNAND_Unlock(void)
{
    UINT32 Rval = AmbaKAL_MutexGive(&AmbaNAND_Ctrl.Mutex);

    if(AmbaNandOp_PostAccessNotify != NULL) {
        AmbaNandOp_PostAccessNotify();
#if defined(CONFIG_ENABLE_AMBALINK) && defined(CONFIG_THREADX)
    } else {
        extern void AmbaIPC_NandUnlock(void);
        (void)AmbaIPC_NandUnlock();
#endif
    }

    return Rval;
}

/*
 *  @RoutineName:: AmbaNAND_Init
 *
 *  @Description:: Initialize NAND data structure.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : OK(0)/NG(-1)
 */
static UINT32 AmbaNAND_Init(void)
{
    UINT32 RetVal = NAND_ERR_NONE;
    static char AmbaPseudoCBMutexName[16] = "PseudoCB_Mutex";

    /* Create Event Flag */
    if (AmbaKAL_EventFlagCreate(&AmbaNAND_Ctrl.EventFlag, NULL) != OK) {
        RetVal = NAND_ERR_OS_API_FAIL;  /* should never happen ! */
    } else {
        /* Create Mutex */
        if (AmbaKAL_MutexCreate(&AmbaNAND_Ctrl.Mutex, NULL) != OK) {
            RetVal = NAND_ERR_OS_API_FAIL;  /* should never happen ! */
        } else {
            if (AmbaKAL_MutexCreate(&PseudoCB_Mutex, AmbaPseudoCBMutexName) != OK) {
                RetVal = NAND_ERR_OS_API_FAIL;  /* should never happen ! */
            } else {
                AmbaRTSL_FioInit();
                (void)AmbaRTSL_NandInit();

                AmbaRTSL_FioInitInt();
                AmbaRTSL_FioNandCmdIsrCallBack = NAND_FioNandCmdIsr; /* Call back function when NAND Command done */
            }
        }
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaNAND_ReadID
 *
 *  @Description:: Read device ID
 *
 *  @Input      ::
 *      NumReadCycle: number of cycles
 *      pDeviceID:    pointer to the buffer of Device ID
 *      TimeOut:      Time out value
 *
 *  @Output     ::
 *      pDeviceID: pointer to the Device ID
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaNAND_ReadID(UINT8 NumReadCycle, UINT8 *pDeviceID, UINT32 TimeOut)
{
    UINT32 RetVal;
    UINT32 ActualFlags = 0U;

    /*
     * Take the Mutex
     */
    if (AmbaNAND_Lock() != OK) {
        RetVal = NAND_ERR_OS_API_FAIL;
    } else {
        RetVal = AmbaNAND_Reset(TimeOut);
        if (RetVal == NAND_ERR_NONE) {

            (void )AmbaKAL_EventFlagClear(&AmbaNAND_Ctrl.EventFlag, AMBA_FIO_NAND_CMD_DONE_FLAG);
            AmbaRTSL_NandSendReadIdCmd(NumReadCycle);

            /* wait for Command Done: Event Flag ! */
            RetVal = AmbaKAL_EventFlagGet(&AmbaNAND_Ctrl.EventFlag, AMBA_FIO_NAND_CMD_DONE_FLAG,
                                          0x1U, 0x1U, &ActualFlags, TimeOut);
            if (RetVal == NAND_ERR_NONE) {
                AmbaRTSL_NandGetReadIdResponse(NumReadCycle, pDeviceID);
            }

            /*
             * Release the Mutex
             */
            (void) AmbaNAND_Unlock();
        }
    }
    return RetVal;
}

/*
 *  @RoutineName:: AmbaNAND_Config
 *
 *  @Description:: NAND software configurations
 *
 *  @Input      ::
 *      pNandConfig: pointer to NAND software configurations
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : OK(0)/NG(-1)
 */
UINT32 AmbaNAND_Config(AMBA_NAND_CONFIG_s *pNandConfig)
{
    UINT32 RetVal = NAND_ERR_NONE;
    RetVal = AmbaNAND_Init();
    if (RetVal == NAND_ERR_NONE) {
        AmbaNAND_HookApi(pNandConfig);
        (void)AmbaNAND_Lock();
        RetVal = AmbaNAND_Reset(1000);
        (void)AmbaNAND_Unlock();
    }

    if (RetVal == NAND_ERR_NONE) {
        (void) AmbaRTSL_NandConfig(pNandConfig);
        /* Init BBT and System/User Partition Tables */
        RetVal = AmbaNAND_InitPtbBbt(1000);
    }

    if (RetVal == NAND_ERR_NONE) {
        RetVal = AmbaNAND_LoadNvmRomFileTable();   /* Load NAND ROM File Table */
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaNAND_GetDevInfo
 *
 *  @Description:: get the pointer to current NAND device information
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      AMBA_NAND_DEV_INFO_s * : the pointer to current NAND device information
 */
AMBA_NAND_DEV_INFO_s *AmbaNAND_GetDevInfo(void)
{
    return AmbaRTSL_NandDevInfo;
}

AMBA_NAND_COMMON_INFO_s *AmbaNAND_GetCommonInfo(void)
{
    return AmbaRTSL_NandCommonInfo;
}

/*
 *  @RoutineName:: AmbaNAND_Reset
 *
 *  @Description:: Reset NAND data structure.
 *
 *  @Input      ::
 *      TimeOut:   The timeout value
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaNAND_Reset(UINT32 TimeOut)
{
    UINT32 ActualFlags = 0U;
    UINT32 RetVal = NAND_ERR_NONE;

    (void) AmbaKAL_EventFlagClear(&AmbaNAND_Ctrl.EventFlag, AMBA_FIO_NAND_CMD_DONE_FLAG);

    AmbaRTSL_NandSendResetCmd();

    if (OK != AmbaKAL_EventFlagGet(&AmbaNAND_Ctrl.EventFlag, AMBA_FIO_NAND_CMD_DONE_FLAG,
                                   0x1U, 0x1U, &ActualFlags, TimeOut)) {
        RetVal = NAND_ERR_IO_FAIL;
    }

    return RetVal;
}
#if 0
/*
 *  @RoutineName:: AmbaNAND_ReadStatus
 *
 *  @Description:: Get Nand current status.
 *
 *  @Input      ::
 *      pStatus: pointer to the Status
 *
 *  @Output     ::
 *      pStatus: pointer to the Status
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
INT32 AmbaNAND_ReadStatus(AMBA_NAND_STATUS_u *pStatus, UINT32 TimeOut)
{
    UINT32 ActualFlags = 0U;
    INT32 RetVal;

    /*
     * Take the Mutex
     */
    if (AmbaNAND_Lock() != OK) {
        RetVal = NG;
    } else {

        (void) AmbaKAL_EventFlagClear(&AmbaNAND_Ctrl.EventFlag, AMBA_FIO_NAND_CMD_DONE_FLAG);

        AmbaRTSL_NandSendReadStatusCmd();

        RetVal = AmbaKAL_EventFlagGet(&AmbaNAND_Ctrl.EventFlag, AMBA_FIO_NAND_CMD_DONE_FLAG, 0x1U, 0x1U, &ActualFlags, TimeOut);
        if (RetVal == OK) {
            AmbaRTSL_NandGetCmdResponse((UINT8*)pStatus);
        }

        /*
         * Release the Mutex
         */
        (void) AmbaNAND_Unlock();
    }
    return RetVal;
}
#endif
/*
 *  @RoutineName:: AmbaNAND_Read
 *
 *  @Description:: Read data from NAND flash
 *
 *  @Input      ::
 *      PageAddr:  The first page address to read
 *      NumPage:   Number of pages to read
 *      pMainBuf:  pointer to DRAM buffer for main area data
 *      pSpareBuf: pointer to DRAM buffer for spare area data
 *      TimeOut:   The timeout value
 *
 *  @Output     ::
 *      pMainBuf:  pointer to main area data
 *      pSpareBuf: pointer to spare area data
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaNAND_Read(UINT32 PageAddr, UINT32 NumPage, UINT8 *pMainBuf, UINT8 *pSpareBuf, UINT32 TimeOut)
{
    UINT32 RetVal;
    const AMBA_RTSL_FDMA_CTRL_s *pFdmaCtrl = &AmbaRTSL_FioCtrl.FdmaCtrl;
    UINT32 ActualFlags = 0U;

    if ((NumPage == 0U) || ((pMainBuf == NULL) && (pSpareBuf == NULL))) {
        RetVal = NAND_ERR_ARG;  /* wrong parameter */
    } else {
        /*
         * Take the Mutex
         */
        if (AmbaNAND_Lock() != OK) {
            RetVal = NAND_ERR_OS_API_FAIL;
        } else {
            (void) AmbaKAL_EventFlagClear(&AmbaNAND_Ctrl.EventFlag, AMBA_FIO_EVENT_MASK );

            RetVal = AmbaRTSL_NandReadStart(PageAddr, NumPage, pMainBuf, pSpareBuf);
            if (RetVal == NAND_ERR_NONE) {
                if (OK != AmbaKAL_EventFlagGet(&AmbaNAND_Ctrl.EventFlag, AMBA_FIO_EVENT_MASK,
                                               0x1U, 0x1U, &ActualFlags, TimeOut)) {
                    RetVal = NAND_ERR_IO_FAIL;
                }
            }

            if (RetVal == NAND_ERR_NONE) {
                (void)AmbaRTSL_CacheInvalDataPtr(pFdmaCtrl->pMainBuf, pFdmaCtrl->MainByteCount);
                (void)AmbaRTSL_CacheInvalDataPtr(pFdmaCtrl->pSpareBuf, pFdmaCtrl->SpareByteCount);
                RetVal = AmbaRTSL_NandCheckDeviceStatus(NumPage);
            }

            if (RetVal == NAND_ERR_NONE) {
                if ((pMainBuf != NULL) && (pMainBuf != pFdmaCtrl->pMainBuf)) {
                    if (AmbaWrap_memcpy(pMainBuf, pFdmaCtrl->pMainBuf, pFdmaCtrl->MainByteCount) != OK) { /* Do nothing */ };
                }

                if ((pSpareBuf != NULL) && (pSpareBuf != pFdmaCtrl->pSpareBuf)) {
                    if (AmbaWrap_memcpy(pSpareBuf, pFdmaCtrl->pSpareBuf, pFdmaCtrl->SpareByteCount) != OK) { /* Do nothing */ };
                }
            } else {
                (void) AmbaNAND_Reset(5000);
            }

            /*
             * Release the Mutex
             */
            (void) AmbaNAND_Unlock();
        }
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaNAND_Program
 *
 *  @Description:: Perform NAND write data cmd flow setup
 *
 *  @Input      ::
 *      PageAddr:  The first page address to write
 *      NumPage:   Number of pages to write
 *      pMainBuf:  pointer to DRAM buffer for main area data
 *      pSpareBuf: pointer to DRAM buffer for spare area data
 *      TimeOut:   The timeout value
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaNAND_Program(UINT32 PageAddr, UINT32 NumPage, UINT8 *pMainBuf, UINT8 *pSpareBuf, UINT32 TimeOut)
{
    UINT32 RetVal;
    UINT32 ActualFlags = 0U;

    if ((NumPage == 0U) || ((pMainBuf == NULL) && (pSpareBuf == NULL))) {
        RetVal = NAND_ERR_ARG;  /* wrong parameter */
    } else {

        /*
         * Take the Mutex
         */
        if (AmbaNAND_Lock() != OK) {
            RetVal = NAND_ERR_OS_API_FAIL;
        } else {
            AmbaCSL_NandDisableWriteProtect();

            (void) AmbaKAL_EventFlagClear(&AmbaNAND_Ctrl.EventFlag, AMBA_FIO_EVENT_MASK);
            AmbaRTSL_NandProgramStart(PageAddr, NumPage, pMainBuf, pSpareBuf);

            RetVal = AmbaKAL_EventFlagGet(&AmbaNAND_Ctrl.EventFlag, AMBA_FIO_EVENT_MASK,
                                          0x1U, 0x1U, &ActualFlags, TimeOut);
            if (RetVal == OK) {
                RetVal = AmbaRTSL_NandCheckDeviceStatus(NumPage);
            }

            AmbaCSL_NandEnableWriteProtect();

            /*
             * Release the Mutex
             */
            (void) AmbaNAND_Unlock();
        }
    }

    return RetVal;
}

/*
 *  @RoutineName:: NandPseudoCB
 *
 *  @Description:: Copyback by read and program command.
 *
 *  @Input      ::
 *          UINT32 BlockFrom    : Copy from
 *          UINT32 Page         : Page to copied
 *          UINT32 BlockTo      : Copy to
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
static UINT32 AmbaNAND_PseudoCB(UINT32 DestPageAddr, UINT32 SrcPageAddr, UINT32 TimeOut)
{
    /* Work Buffer for 1 Block.  */
    static UINT8 PseudoCBBufMain[64 * 2 * 1024]
    GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;

    static UINT8 PseudoCBBufSpare[64 * 128]
    GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;

    UINT32 RetVal;

    if (AmbaKAL_MutexTake(&PseudoCB_Mutex, 0xFFFFFFFFU) != KAL_ERR_NONE) {
        RetVal = NAND_ERR_OS_API_FAIL;
    } else {
        RetVal = AmbaNAND_Read(SrcPageAddr, 1U, PseudoCBBufMain, PseudoCBBufSpare, TimeOut);
        if (RetVal == NAND_ERR_IO_FAIL) {
            RetVal = NAND_ERR_READ;
        }

        if (RetVal == NAND_ERR_NONE) {
            RetVal = AmbaNAND_Program(DestPageAddr, 1U, PseudoCBBufMain, PseudoCBBufSpare, TimeOut);
            if (RetVal == NAND_ERR_IO_FAIL) {
                RetVal = NAND_ERR_PROGRAM;
            }
        }

        (void)AmbaKAL_MutexGive(&PseudoCB_Mutex);
    }
    return RetVal;
}

/*
 *  @RoutineName:: AmbaNAND_CopyBack
 *
 *  @Description:: Copy data from source page to destinaiton page
 *
 *  @Input      ::
 *      DestPageAddr: Destination page address
 *      SrcPageAddr: Source page address
 *      TimeOut:     Time out value
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaNAND_CopyBack(UINT32 DestPageAddr, UINT32 SrcPageAddr, UINT32 TimeOut)
{
#if !defined(DISABLE_COPYBACK)
    AMBA_NAND_DEV_INFO_s *pNandDevInfo = AmbaRTSL_NandDevInfo;
    UINT32 MainByteSize = 0;
    UINT32 ActualFlags = 0U;
    INT32 RetVal = OK;

    if (pNandDevInfo == NULL) {
        RetVal = -1;
    } else {
        AmbaCSL_NandDisableWriteProtect();

        MainByteSize = pNandDevInfo->MainByteSize;
        (void) AmbaKAL_EventFlagClear(&AmbaNAND_Ctrl.EventFlag, AMBA_FIO_NAND_CMD_DONE_FLAG);
        AmbaCSL_NandSendCopyBackCmd(SrcPageAddr * MainByteSize, DestPageAddr * MainByteSize);

        RetVal = AmbaKAL_EventFlagGet(&AmbaNAND_Ctrl.EventFlag, AMBA_FIO_NAND_CMD_DONE_FLAG, 0x1U, 0x1U, &ActualFlags, TimeOut);
        if (RetVal != OK) {
            return RetVal;
        }

        AmbaCSL_NandGetCmdResponse((UINT8 *) & (AmbaRTSL_NandCtrl.Status[0]));

        AmbaCSL_NandEnableWriteProtect();

        return AmbaRTSL_NandCtrl.Status[0].Bits[0].LastCmdFailed ? -1 : OK;
    }
#else
    return AmbaNAND_PseudoCB(DestPageAddr, SrcPageAddr, TimeOut);
#endif
}

/*
 *  @RoutineName:: AmbaNAND_EraseBlock
 *
 *  @Description:: To erase the block of data identified by the block address parameter
 *
 *  @Input      ::
 *      BlkAddr: Block address
 *      TimeOut: Time out value
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaNAND_EraseBlock(UINT32 BlkAddr, UINT32 TimeOut)
{
    UINT32 ActualFlags = 0U;
    UINT32 RetVal;

    /*
     * Take the Mutex
     */
    if (AmbaNAND_Lock() != OK) {
        RetVal = NAND_ERR_OS_API_FAIL;
    } else {
        AmbaCSL_NandDisableWriteProtect();

        (void) AmbaKAL_EventFlagClear(&AmbaNAND_Ctrl.EventFlag, AMBA_FIO_NAND_CMD_DONE_FLAG);
        AmbaCSL_NandSendBlockEraseCmd(BlkAddr * AmbaRTSL_NandCtrl.BlkByteSize);

        RetVal = AmbaKAL_EventFlagGet(&AmbaNAND_Ctrl.EventFlag, AMBA_FIO_NAND_CMD_DONE_FLAG, 0x1U, 0x1U, &ActualFlags, TimeOut);
        if (RetVal == KAL_ERR_NONE) {
            AmbaCSL_NandGetCmdResponse((UINT8 *) & (AmbaRTSL_NandCtrl.Status[0]));
            if (AmbaRTSL_NandCtrl.Status[0].Bits[0].LastCmdFailed == 1U) {
                RetVal = NAND_ERR_IO_FAIL;
            } else {
                RetVal = NAND_ERR_NONE;
            }
        } else {
            RetVal = NAND_ERR_NONE;
        }

        AmbaCSL_NandEnableWriteProtect();

        /*
         * Release the Mutex
         */
        (void) AmbaNAND_Unlock();
    }

    return RetVal;
}

/*
 *  @RoutineName:: NAND_FioNandCmdIsr
 *
 *  @Description:: NAND command done ISR
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
/* disable NEON registers usage in ISR */
#pragma GCC push_options
#pragma GCC target("general-regs-only")

static void NAND_FioNandCmdIsr(void)
{
    (void) AmbaKAL_EventFlagSet(&AmbaNAND_Ctrl.EventFlag, AMBA_FIO_NAND_CMD_DONE_FLAG);
}
#pragma GCC pop_options

static void AmbaNAND_HookApi(const AMBA_NAND_CONFIG_s *pNandConfig)
{
    AmbaNandOp_Copyback   = AmbaNAND_CopyBack;
    AmbaNandOp_EraseBlock = AmbaNAND_EraseBlock;
    AmbaNandOp_Read       = AmbaNAND_Read;
    AmbaNandOp_Program    = AmbaNAND_Program;

    AmbaNandOp_PreAccessNotify  = pNandConfig->AmbaNandPreAccessNotify;
    AmbaNandOp_PostAccessNotify = pNandConfig->AmbaNandPostAccessNotify;
}

