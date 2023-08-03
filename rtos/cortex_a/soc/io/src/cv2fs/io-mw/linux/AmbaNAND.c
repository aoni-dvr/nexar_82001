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
/*
#include "AmbaRTSL_Cache.h"
#include "AmbaRTSL_FIO.h"
#include "AmbaRTSL_NAND.h"
#include "AmbaRTSL_NAND_Ctrl.h"
#include "AmbaCSL_FIO.h"
#include "AmbaCSL_NAND.h"
*/
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

static AMBA_NAND_CTRL_s _AmbaNAND_Ctrl = {0};    /* NAND Management Structure */

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
    }

    return AmbaKAL_MutexTake(&_AmbaNAND_Ctrl.Mutex, 0xFFFFFFFFUL);
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
    UINT32 Rval = AmbaKAL_MutexGive(&_AmbaNAND_Ctrl.Mutex);

    if(AmbaNandOp_PostAccessNotify != NULL) {
        AmbaNandOp_PostAccessNotify();
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
    (void) NumReadCycle;
    (void) pDeviceID;

    /*
     * Take the Mutex
     */
    if (AmbaNAND_Lock() != OK) {
        RetVal = NAND_ERR_OS_API_FAIL;
    } else {
        RetVal = AmbaNAND_Reset(TimeOut);
        if (RetVal == NAND_ERR_NONE) {

            (void )AmbaKAL_EventFlagClear(&_AmbaNAND_Ctrl.EventFlag, AMBA_FIO_NAND_CMD_DONE_FLAG);
            //AmbaRTSL_NandSendReadIdCmd(NumReadCycle);

            /* wait for Command Done: Event Flag ! */
            RetVal = AmbaKAL_EventFlagGet(&_AmbaNAND_Ctrl.EventFlag, AMBA_FIO_NAND_CMD_DONE_FLAG,
                                          0x1U, 0x1U, &ActualFlags, TimeOut);
            if (RetVal == NAND_ERR_NONE) {
                //AmbaRTSL_NandGetReadIdResponse(NumReadCycle, pDeviceID);
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
    (void) pNandConfig;

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
    return 0;
}

AMBA_NAND_COMMON_INFO_s *AmbaNAND_GetCommonInfo(void)
{
    return 0;
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

    (void) AmbaKAL_EventFlagClear(&_AmbaNAND_Ctrl.EventFlag, AMBA_FIO_NAND_CMD_DONE_FLAG);

    //AmbaRTSL_NandSendResetCmd();

    if (OK != AmbaKAL_EventFlagGet(&_AmbaNAND_Ctrl.EventFlag, AMBA_FIO_NAND_CMD_DONE_FLAG,
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

        (void) AmbaKAL_EventFlagClear(&_AmbaNAND_Ctrl.EventFlag, AMBA_FIO_NAND_CMD_DONE_FLAG);

        AmbaRTSL_NandSendReadStatusCmd();

        RetVal = AmbaKAL_EventFlagGet(&_AmbaNAND_Ctrl.EventFlag, AMBA_FIO_NAND_CMD_DONE_FLAG, 0x1U, 0x1U, &ActualFlags, TimeOut);
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
    UINT32 RetVal = NAND_ERR_ARG;
    (void) PageAddr;
    (void) NumPage;
    (void) pMainBuf;
    (void) pSpareBuf;
    (void) TimeOut;

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
    UINT32 RetVal = NAND_ERR_NONE;
    (void) PageAddr;
    (void) NumPage;
    (void) pMainBuf;
    (void) pSpareBuf;
    (void) TimeOut;

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

    RetVal = AmbaNAND_Read(SrcPageAddr, 1, PseudoCBBufMain, PseudoCBBufSpare, TimeOut);
    if (RetVal == NAND_ERR_NONE) {
        RetVal = AmbaNAND_Program(DestPageAddr, 1, PseudoCBBufMain, PseudoCBBufSpare, TimeOut);
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
        (void) AmbaKAL_EventFlagClear(&_AmbaNAND_Ctrl.EventFlag, AMBA_FIO_NAND_CMD_DONE_FLAG);
        AmbaCSL_NandSendCopyBackCmd(SrcPageAddr * MainByteSize, DestPageAddr * MainByteSize);

        RetVal = AmbaKAL_EventFlagGet(&_AmbaNAND_Ctrl.EventFlag, AMBA_FIO_NAND_CMD_DONE_FLAG, 0x1U, 0x1U, &ActualFlags, TimeOut);
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
    (void) BlkAddr;
    /*
     * Take the Mutex
     */
    if (AmbaNAND_Lock() != OK) {
        RetVal = NAND_ERR_OS_API_FAIL;
    } else {
        //AmbaCSL_NandDisableWriteProtect();

        (void) AmbaKAL_EventFlagClear(&_AmbaNAND_Ctrl.EventFlag, AMBA_FIO_NAND_CMD_DONE_FLAG);
        //AmbaCSL_NandSendBlockEraseCmd(BlkAddr * AmbaRTSL_NandCtrl.BlkByteSize);

        RetVal = AmbaKAL_EventFlagGet(&_AmbaNAND_Ctrl.EventFlag, AMBA_FIO_NAND_CMD_DONE_FLAG, 0x1U, 0x1U, &ActualFlags, TimeOut);
        if (RetVal == KAL_ERR_NONE) {
            RetVal = NAND_ERR_NONE;
        } else {
            RetVal = NAND_ERR_NONE;
        }

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


