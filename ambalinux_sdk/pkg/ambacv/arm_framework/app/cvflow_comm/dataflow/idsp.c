/**
*  @file idsp.c
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
*   @details Example to exchange data between RTOS and linux
*
*/

/*---------------------------------------------------------------------------*\
 * [Description]
 *   RTOS -> Linux : The data struct is memio_source_recv_picinfo_t
 *   Linux -> RTOS : The data struct is memio_sink_send_out_t
\*---------------------------------------------------------------------------*/

#include "idsp.h"
#include "cvapi_ambacv_flexidag.h"
#include "AmbaFlexidagIO.h"

#define ARM_LOG_IDSP      "IDSP"

typedef struct {
    UINT32                    task_run;
    UINT32                    ipc_ch;
    ArmTask_t                 task;
} IDSP_CTRL_s;

static IDSP_CTRL_s g_IdspCtrl[MAX_SLOT_NUM] = {0};

/*---------------------------------------------------------------------------*\
 * static functions
\*---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: idsp_feeder_thd
 *
 *  @Description:: The thread to get idsp input fro RTOS
 *
 *  @Input      ::
 *     EntryArg:   The thread arg
 *
 *  @Output     :: None
 *
 *  @Return     :: None
\*-----------------------------------------------------------------------------------------------*/
static ArmTaskRet_t idsp_feeder_thd(ArmTaskEntryArg_t EntryArg)
{
    memio_source_recv_picinfo_t PicInfoData = {0};
    unsigned int ret, getlen;
    UINT32 *pSlot = (UINT32 *)EntryArg;   // EntryArg = pointer to SlotId
    UINT32 Slot = *pSlot;
    IDSP_CTRL_s *pCtrl;

    if (Slot < MAX_SLOT_NUM) {
        pCtrl = &g_IdspCtrl[Slot];

        while (pCtrl->task_run != 0) {
            /* 1. Get picinfo data (memio_source_recv_picinfo_t) from IPC */
            ret = AmbaIPC_FlexidagIO_GetInput(pCtrl->ipc_ch, &PicInfoData, &getlen);
            if ((ret == 0) && (getlen == sizeof(memio_source_recv_picinfo_t))) {

                /* 2. Feed picinfo data to CvAlgo (We pass IDSP_CTRL_s* by pUserdata) */
                ret = CtCvAlgoWrapper_FeedPicinfo(Slot, &PicInfoData, pCtrl);
                if (ret != ARM_OK) {
                    ArmLog_ERR(ARM_LOG_IDSP, "## CtCvAlgoWrapper_FeedPicinfo() fail", 0U, 0U);
                }

            } else {
                ArmLog_ERR(ARM_LOG_IDSP, "## fail to AmbaIPC_FlexidagIO_GetInput() ret %u, getlen %u", ret, getlen);
                break;
            }
        }
    }

    ArmLog_DBG(ARM_LOG_IDSP, "Feeder thread terminate");
    return ARM_TASK_NULL_RET;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: idsp_sink_cb
 *
 *  @Description:: The callback to send output result to RTOS
 *
 *  @Input      ::
 *    Event:       The callback event
 *    pEventData:  The CvAlgo output result
 *
 *  @Output     :: None
 *
 *  @Return     :: None
\*-----------------------------------------------------------------------------------------------*/
static UINT32 idsp_sink_cb(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    UINT32 Rval = ARM_OK;
    memio_sink_send_out_t sinkout = {0};
    IDSP_CTRL_s *pCtrl;

    if (Event == CALLBACK_EVENT_OUTPUT) {

        /* 1. Sanity check for parameters */
        if (pEventData == NULL) {
            ArmLog_ERR(ARM_LOG_IDSP, "## idsp_sink_cb fail (pEventData is null)", 0U, 0U);
            Rval = ARM_NG;
        } else {
            if (pEventData->pOutput == NULL) {
                ArmLog_ERR(ARM_LOG_IDSP, "## idsp_sink_cb fail (pOutput is null)", 0U, 0U);
                Rval = ARM_NG;
            }
            if (pEventData->pUserData == NULL) {  // pUserData should be the pointer to IDSP_CTRL_s
                ArmLog_ERR(ARM_LOG_IDSP, "## idsp_sink_cb fail (pUserData is null)", 0U, 0U);
                Rval = ARM_NG;
            }
        }

        /* 2. Send output to RTOS via AmbaIPC_FlexidagIO  */
        if ((Rval == ARM_OK) && (pEventData != NULL) && (pEventData->pOutput != NULL)) {
            // Pack output result to memio_sink_send_out_t format
            sinkout.num_of_io = pEventData->pOutput->num_of_buf;
            for (unsigned int i = 0; i < sinkout.num_of_io; i++) {
                sinkout.io[i].addr = pEventData->pOutput->buf[i].buffer_daddr;
                sinkout.io[i].size = pEventData->pOutput->buf[i].buffer_size;
            }

            pCtrl = (IDSP_CTRL_s *)pEventData->pUserData;
            Rval = AmbaIPC_FlexidagIO_SetResult(pCtrl->ipc_ch, (void *)&sinkout, sizeof(memio_sink_send_out_t));
            if (Rval != 0) {
                ArmLog_ERR(ARM_LOG_IDSP, "## fail to AmbaIPC_FlexidagIO_SetResult()");
            }
        }

    }
    return Rval;
}

/*---------------------------------------------------------------------------*\
 * idsp APIs
\*---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: idsp_start
 *
 *  @Description:: Start to run idsp feeder
 *
 *  @Input      ::
 *    slot:        The Slot id for CtCvAlgoWrapper
 *    ipc_ch:      The IPC channel for AmbaIPC_FlexidagIO
 *
 *  @Output     :: None
 *
 *  @Return     :: None
\*-----------------------------------------------------------------------------------------------*/
void idsp_start(UINT32 slot, UINT32 ipc_ch)
{
    int ret = 0;
    UINT32 Rval = ARM_OK;
    static UINT32  SlotId[MAX_SLOT_NUM] = {0};

    /* 1. Init IPC channel */
    ret = AmbaIPC_FlexidagIO_Init(ipc_ch);
    if (ret != 0) {
        ArmLog_ERR(ARM_LOG_IDSP, "  ## fail to init IPC channel %u for slot %u", ipc_ch, slot);
        Rval = ARM_NG;
    } else {
        g_IdspCtrl[slot].task_run = 1;
        g_IdspCtrl[slot].ipc_ch = ipc_ch;
        ArmLog_DBG(ARM_LOG_IDSP, "Slot[%u]: AmbaIPC_FlexidagIO_Init done (IPC_CH = %u)", slot, ipc_ch);
    }

    /* 2. Register callback */
    if (Rval == ARM_OK) {
        Rval = CtCvAlgoWrapper_RegCb(slot, 0 /*register*/, idsp_sink_cb);
        if (Rval != ARM_OK) {
            ArmLog_ERR(ARM_LOG_IDSP, "## CtCvAlgoWrapper_RegCb fail", 0U, 0U);
            Rval = ARM_NG;
        }
    }

    if (Rval == ARM_OK) {
        /* 3. Create task to recieve yuv data and feed it to CvCommFlexi */
        char TskName[16] = "IdspFeed_XX";
        ARM_TASK_CREATE_ARG_s  TskArg = {0};

        TskName[9]  = '0' + slot/10;
        TskName[10] = '0' + slot%10;

        SlotId[slot] = slot;
        TskArg.EntryFunction = idsp_feeder_thd;
        TskArg.EntryArg      = (ArmTaskEntryArg_t) &SlotId[slot];

        Rval = ArmTask_Create(&g_IdspCtrl[slot].task, TskName, &TskArg);
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: idsp_stop
 *
 *  @Description:: Stop to run idsp feeder
 *
 *  @Input      ::
 *    slot:        The Slot id for CtCvAlgoWrapper
 *
 *  @Output     :: None
 *
 *  @Return     :: None
\*-----------------------------------------------------------------------------------------------*/
void idsp_stop(uint32_t slot)
{
    UINT32 Rval = ARM_OK;

    /* 1. Unregister callback */
    Rval = CtCvAlgoWrapper_RegCb(slot, 1 /*unregister*/, idsp_sink_cb);
    if (Rval != ARM_OK) {
        ArmLog_ERR(ARM_LOG_IDSP, "## CtCvAlgoWrapper_RegCb fail", 0U, 0U);
    }

    /* 2. Delete feed thread */
    g_IdspCtrl[slot].task_run = 0;
    Rval = ArmTask_Delete(&g_IdspCtrl[slot].task);
    if (Rval != ARM_OK) {
        ArmLog_ERR(ARM_LOG_IDSP, "## ArmTask_Delete fail", 0U, 0U);
    }

    /* 3. Deinit AmbaIPC_FlexidagIO */
    AmbaIPC_FlexidagIO_Deinit(g_IdspCtrl[slot].ipc_ch);
}

