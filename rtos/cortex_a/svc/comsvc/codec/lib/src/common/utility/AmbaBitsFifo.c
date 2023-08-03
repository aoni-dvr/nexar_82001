/**
 *  @file AmbaBitsFifo.c
 *
 * Copyright (c) [2020] Ambarella International LP
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
 * This file includes sample code and is only for internal testing and evaluation.  If you
 * distribute this sample code (whether in source, object, or binary code form), it will be
 * without any warranty or indemnity protection from Ambarella International LP or its affiliates.
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
 *
 *  @details amba bitstream fifo functions
 *
 */

#include "AmbaDef.h"
#include "AmbaMisraFix.h"
#include "AmbaDSP.h"
#include "SvcFIFO.h"
#include "SvcStream.h"
#include "SvcIso.h"
#include "SvcFormat.h"
#include "AmbaCodecCom.h"
#include "AmbaSvcWrap.h"
#include "AmbaBitsFifo.h"

#define BFIFO_NUM               (32U)
#define SVC_ENTRY_NUM_PER_BFIFO     (1024U)
#define MAX_PENDING_NUM         (BFIFO_NUM * (128U))

typedef struct {
    const SVC_FIFO_HDLR_s   *pHdlr;
    UINT8                   EventId;
} AMBA_BFIFO_MSG_s;

typedef struct {
    UINT8                   IsCreate;
    SVC_FIFO_HDLR_s         *pHdlr;
    ULONG                   BufAddr;
} AMBA_BFIFO_INFO_s;

typedef struct {
    AMBA_KAL_TASK_t         Task;
    AMBA_KAL_MSG_QUEUE_t    MsgQue;
    AMBA_BFIFO_INFO_s       Info[BFIFO_NUM];
} AMBA_BFIFO_s;

static AMBA_BFIFO_s BFifo GNU_SECTION_NOZEROINIT;

static void* BitsFifoTask(void* EntryArg)
{
    UINT32            Err, LoopU = 1U;
    AMBA_BFIFO_MSG_s  Msg;

    AmbaMisra_TouchUnused(EntryArg);
    AmbaMisra_TouchUnused(&LoopU);
    AmbaPrint_ModulePrintUInt5(AMBA_CODEC_PRINT_MODULE_ID, "BitsFifoTask created", 0U, 0U, 0U, 0U, 0U);

    while (LoopU == 1U) {
        Err = AmbaKAL_MsgQueueReceive(&(BFifo.MsgQue), &Msg, AMBA_KAL_WAIT_FOREVER);
        if (Err != KAL_ERR_NONE) {
            AmbaPrint_ModulePrintUInt5(AMBA_CODEC_PRINT_MODULE_ID, "AmbaKAL_MsgQueueReceive failed (%u)", Err, 0U, 0U, 0U, 0U);
            LoopU = 0U;
        } else {
            Err = SvcFIFO_ProcessEvent(Msg.pHdlr, Msg.EventId);
            if (Err != CODEC_OK) {
                AmbaPrint_ModulePrintUInt5(AMBA_CODEC_PRINT_MODULE_ID, "SvcFIFO_ProcessEvent failed (%u)", Err, 0U, 0U, 0U, 0U);
            }
        }
    }

    return NULL;
}

static UINT32 BitsFifoEventHandler(const SVC_FIFO_HDLR_s *pHdlr, UINT8 EventId, SVC_FIFO_CALLBACK_INFO_s *pInfo)
{
    UINT32            Rval = CODEC_OK, Err;
    AMBA_BFIFO_MSG_s  Msg;

    AmbaMisra_TouchUnused(pInfo);

    Msg.pHdlr     = pHdlr;
    Msg.EventId   = EventId;

    Err = AmbaKAL_MsgQueueSend(&(BFifo.MsgQue), &Msg, AMBA_KAL_NO_WAIT);
    if (Err != KAL_ERR_NONE) {
        AmbaPrint_ModulePrintUInt5(AMBA_CODEC_PRINT_MODULE_ID, "AmbaKAL_MsgQueueSend failed (%u)", Err, 0U, 0U, 0U, 0U);
    }

    return Rval;
}


/**
* AmbaBitsFifo Init
* @param [in]  pInit init config
* @return ErrorCode
*/
UINT32 AmbaBitsFifo_Init(const AMBA_BFIFO_INIT_s *pInit)
{
    UINT32        Err, Rval = CODEC_OK;
    UINT32        RSize = pInit->BufSize;
    ULONG         BaseAddr = pInit->BufAddr;
    static UINT32 Init = 0U;

    AmbaMisra_TouchUnused(&Rval);

    if (Init == 0U) {
        Err = AmbaWrap_memset(&(BFifo), 0, sizeof(BFifo));
        if (Err != CODEC_OK) {
            AmbaPrint_ModulePrintUInt5(AMBA_CODEC_PRINT_MODULE_ID, "AmbaWrap_memset failed (%u)", Err, 0U, 0U, 0U, 0U);
        }

        /* input parameters check */
        if (Rval == CODEC_OK) {
            UINT32 Size;

            Err = AmbaBitsFifo_EvalMemSize(&Size);
            if (Err == CODEC_OK) {
                if (Size > RSize) {
                    AmbaPrint_ModulePrintUInt5(AMBA_CODEC_PRINT_MODULE_ID, "Buffer size is too small (%u/%u)", RSize, Size, 0U, 0U, 0U);
                    Rval = CODEC_ERROR_ARG;
                }
            } else {
                AmbaPrint_ModulePrintUInt5(AMBA_CODEC_PRINT_MODULE_ID, "AmbaBitsFifo_EvalMemSize failed (%u)", Err, 0U, 0U, 0U, 0U);
                Rval = CODEC_ERROR_GENERAL_ERROR;
            }
        }

        /* create msg queue for fifo process task */
        if (Rval == CODEC_OK) {
#define BFIFO_MSG_NUM (512U)
            static char              Name[] = "BitsFifo_Que";
            static AMBA_BFIFO_MSG_s  MsgBuf[BFIFO_MSG_NUM] GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;

            Err = AmbaKAL_MsgQueueCreate(&(BFifo.MsgQue), Name, (UINT32)sizeof(AMBA_BFIFO_MSG_s), MsgBuf, (UINT32)sizeof(MsgBuf));
            if (Err != KAL_ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(AMBA_CODEC_PRINT_MODULE_ID, "AmbaKAL_MsgQueueCreate failed (%u)", Err, 0U, 0U, 0U, 0U);
                Rval = CODEC_ERROR_GENERAL_ERROR;
            }
        }

        /* create fifo process task */
        if (Rval == CODEC_OK) {
#define BFIFO_STACK_SIZE (0x6000U)
            static UINT8     StackBuf[BFIFO_STACK_SIZE] GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;
            static char      Name[] = "BitsFifo_Task";

            Err = AmbaKAL_TaskCreate(&(BFifo.Task), Name, pInit->TaskPriority,
                                    BitsFifoTask, NULL, StackBuf, BFIFO_STACK_SIZE, 0U);
            if (Err != KAL_ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(AMBA_CODEC_PRINT_MODULE_ID, "AmbaKAL_TaskCreate failed (%u)", Err, 0U, 0U, 0U, 0U);
                Rval = CODEC_ERROR_GENERAL_ERROR;
            }

            if (Rval == CODEC_OK) {
                Err = AmbaKAL_TaskSetSmpAffinity(&(BFifo.Task), pInit->TaskCpuBits);
                if (Err != KAL_ERR_NONE) {
                    AmbaPrint_ModulePrintUInt5(AMBA_CODEC_PRINT_MODULE_ID, "AmbaKAL_TaskSetSmpAffinity failed (%u)", Err, 0U, 0U, 0U, 0U);
                    Rval = CODEC_ERROR_GENERAL_ERROR;
                }
            }

            if (Rval == CODEC_OK) {
                Err = AmbaKAL_TaskResume(&(BFifo.Task));
                if (Err != KAL_ERR_NONE) {
                    AmbaPrint_ModulePrintUInt5(AMBA_CODEC_PRINT_MODULE_ID, "AmbaKAL_TaskResume failed (%u)", Err, 0U, 0U, 0U, 0U);
                    Rval = CODEC_ERROR_GENERAL_ERROR;
                }
            }
        }

        /* init fifo */
        if (Rval == CODEC_OK) {
            SVC_FIFO_INIT_CFG_s  InitCfg;
            UINT32               Size;

            Err = SvcFIFO_GetInitDefaultCfg(&InitCfg);
            if (Err != CODEC_OK) {
                AmbaPrint_ModulePrintUInt5(AMBA_CODEC_PRINT_MODULE_ID, "SvcFIFO_GetInitDefaultCfg failed (%u)", Err, 0U, 0U, 0U, 0U);
            }

            Err = SvcFIFO_GetInitBufferSize(BFIFO_NUM, MAX_PENDING_NUM, &Size);
            if (Err != CODEC_OK) {
                AmbaPrint_ModulePrintUInt5(AMBA_CODEC_PRINT_MODULE_ID, "SvcFIFO_GetInitBufferSize failed (%u)", Err, 0U, 0U, 0U, 0U);
                Rval = CODEC_ERROR_GENERAL_ERROR;
            } else {
                if (RSize < Size) {
                    AmbaPrint_ModulePrintUInt5(AMBA_CODEC_PRINT_MODULE_ID, "BufSize is not enough (%u/%u)", RSize, Size, 0U, 0U, 0U);
                    Rval = CODEC_ERROR_ARG;
                }
            }

            if (Rval == CODEC_OK) {
                InitCfg.MemoryPoolSize     = Size;
                InitCfg.NumMaxFifo         = BFIFO_NUM;
                InitCfg.NumMaxPendingEvent = MAX_PENDING_NUM;
                InitCfg.CbEvent            = BitsFifoEventHandler;
                AmbaMisra_TypeCast(&InitCfg.MemoryPoolAddr, &BaseAddr);

                Err = SvcFIFO_Init(&InitCfg);
                if (Err != CODEC_OK) {
                    AmbaPrint_ModulePrintUInt5(AMBA_CODEC_PRINT_MODULE_ID, "SvcFIFO_Init failed (%u)", Err, 0U, 0U, 0U, 0U);
                    Rval = CODEC_ERROR_GENERAL_ERROR;
                }
            }

            RSize    -= Size;
            BaseAddr += Size;
        }

        /* fifo element buffer */
        if (Rval == CODEC_OK) {
            UINT32 i, Size;

            Size  = (UINT32)sizeof(SVC_FIFO_BITS_DESC_s) * SVC_ENTRY_NUM_PER_BFIFO;
            Size  = GetAlignedValU32(Size, (UINT32)AMBA_CACHE_LINE_SIZE);

            for (i = 0U; i < BFIFO_NUM; i++) {
                if (RSize >= Size) {
                    BFifo.Info[i].BufAddr = BaseAddr;
                    RSize    -= Size;
                    BaseAddr += Size;
                } else {
                    AmbaPrint_ModulePrintUInt5(AMBA_CODEC_PRINT_MODULE_ID, "BufSize is not enough (%u/%u)", RSize, Size, 0U, 0U, 0U);
                    Rval = CODEC_ERROR_ARG;
                    break;
                }
            }
        }

        if (Rval == CODEC_OK) {
            Init = 1U;
        }
    }

    return Rval;
}

/**
* evaluate memory size
* @param [out]  pSize Size
* @return ErrorCode
*/
UINT32 AmbaBitsFifo_EvalMemSize(UINT32 *pSize)
{
    UINT32  Err, Rval = CODEC_OK, Size, RSize = 0U;

    AmbaMisra_TouchUnused(&Rval);

    if (Rval == CODEC_OK) {
        Err = SvcFIFO_GetInitBufferSize(BFIFO_NUM, MAX_PENDING_NUM, &Size);
        if (Err != CODEC_OK) {
            AmbaPrint_ModulePrintUInt5(AMBA_CODEC_PRINT_MODULE_ID, "SvcFIFO_GetInitBufferSize failed (%u)", Err, 0U, 0U, 0U, 0U);
            Rval = CODEC_ERROR_GENERAL_ERROR;
        } else {
            RSize += Size;
        }
    }

    if (Rval == CODEC_OK) {
        Size  = (UINT32)sizeof(SVC_FIFO_BITS_DESC_s) * SVC_ENTRY_NUM_PER_BFIFO;
        Size  = GetAlignedValU32(Size, (UINT32)AMBA_CACHE_LINE_SIZE);
        Size *= BFIFO_NUM;

        RSize += Size;
    }

    if (Rval == CODEC_OK) {
        *pSize = RSize;
    }

    return Rval;
}

/**
* AmbaBitsFifo Create
* @param [in]  pCreate create info
* @param [out]  ppHdlr fifo handler
* @return ErrorCode
*/
UINT32 AmbaBitsFifo_Create(const AMBA_BFIFO_CREATE_s *pCreate, SVC_FIFO_HDLR_s **ppHdlr)
{
    UINT32            Err, Rval = CODEC_OK, FifoId = 0U, i, IsFind = 0U;
    SVC_FIFO_CFG_s    FifoCfg;

    AmbaMisra_TouchUnused(&Rval);

    /* find available fifo */
    if (Rval == CODEC_OK) {
        for (i = 0U; i < BFIFO_NUM; i++) {
            if (BFifo.Info[i].IsCreate == 0U) {
                FifoId = i;
                IsFind = 1U;
                break;
            }
        }

        if (IsFind == 0U) {
            AmbaPrint_ModulePrintUInt5(AMBA_CODEC_PRINT_MODULE_ID, "No fifo is available", 0U, 0U, 0U, 0U, 0U);
            Rval = CODEC_ERROR_ARG;
        }
    }

    /* create fifo */
    if (Rval == CODEC_OK) {
        Err = SvcFIFO_GetDefaultCfg(&FifoCfg);
        if (Err != CODEC_OK) {
            AmbaPrint_ModulePrintUInt5(AMBA_CODEC_PRINT_MODULE_ID, "SvcFIFO_GetDefaultCfg failed (%u)", Err, 0U, 0U, 0U, 0U);
        }

        FifoCfg.NumEntries = SVC_ENTRY_NUM_PER_BFIFO;
        AmbaMisra_TypeCast(&(FifoCfg.BufferAddr), &(BFifo.Info[FifoId].BufAddr));
        FifoCfg.BufferSize = GetAlignedValU32(((UINT32)sizeof(SVC_FIFO_BITS_DESC_s) * SVC_ENTRY_NUM_PER_BFIFO), (UINT32)AMBA_CACHE_LINE_SIZE);
        FifoCfg.BaseHdlr   = pCreate->pBaseHdlr;
        FifoCfg.CbEvent    = pCreate->CbEvent;
        FifoCfg.BaseCfg    = pCreate->BaseCfg;
        FifoCfg.VirtualCfg = pCreate->VirtualCfg;

        Err = SvcFIFO_Create(&FifoCfg, &(BFifo.Info[FifoId].pHdlr));
        if (Err != CODEC_OK) {
            AmbaPrint_ModulePrintUInt5(AMBA_CODEC_PRINT_MODULE_ID, "SvcFIFO_Create failed (%u)", Err, 0U, 0U, 0U, 0U);
            Rval = CODEC_ERROR_GENERAL_ERROR;
        }
    }

    if (Rval == CODEC_OK) {
        BFifo.Info[FifoId].IsCreate = 1U;
        *ppHdlr = BFifo.Info[FifoId].pHdlr;
    }

    return Rval;
}

/**
* AmbaBitsFifo delete
* @param [in]  pHdlr fifo handler
* @return ErrorCode
*/
UINT32 AmbaBitsFifo_Delete(SVC_FIFO_HDLR_s *pHdlr)
{
    UINT32  Err, Rval = CODEC_ERROR_ARG, i, FifoId = 0U;

    AmbaMisra_TouchUnused(&Rval);
    AmbaMisra_TouchUnused(pHdlr);

    /* find the fifo id */
    for (i = 0U; i < BFIFO_NUM; i++) {
        if (BFifo.Info[i].pHdlr == pHdlr) {
            FifoId = i;

            /* pass misra rule */
            AmbaMisra_TouchUnused(&FifoId);
            if (BFifo.Info[i].IsCreate == 0U) {
                AmbaPrint_ModulePrintUInt5(AMBA_CODEC_PRINT_MODULE_ID, "the fifo is already deleted (%u)", i, 0U, 0U, 0U, 0U);
            } else {
                Rval = CODEC_OK;
            }
            break;
        }
    }


    if (Rval != CODEC_OK) {
        AmbaPrint_ModulePrintUInt5(AMBA_CODEC_PRINT_MODULE_ID, "Cannot find the corresponded fifo", 0U, 0U, 0U, 0U, 0U);
    }

    /* delete fifo */
    if ((Rval == CODEC_OK) && (FifoId < BFIFO_NUM)) {
        Err = SvcFIFO_Delete(BFifo.Info[FifoId].pHdlr);
        if (Err != CODEC_OK) {
            AmbaPrint_ModulePrintUInt5(AMBA_CODEC_PRINT_MODULE_ID, "SvcFIFO_Delete failed (%u)", Err, 0U, 0U, 0U, 0U);
            Rval = CODEC_ERROR_GENERAL_ERROR;
        }
    }

    if ((Rval == CODEC_OK) && (FifoId < BFIFO_NUM)) {
        BFifo.Info[FifoId].pHdlr     = NULL;
        BFifo.Info[FifoId].IsCreate  = 0U;
    }

    return Rval;
}
