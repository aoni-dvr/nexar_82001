/**
 *  @file SvcCvFlexidagIO.c
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
 *  @details Implementation of IPC Flexidag IO interface
 *
 */

#include "AmbaKAL.h"
#include "AmbaMisraFix.h"
#include "AmbaUtility.h"
#include "AmbaWrap.h"

#include "SvcLog.h"
#include "SvcErrCode.h"
#include "AmbaIPC_FlexidagIO.h"
#include "SvcCvFlexidagIO.h"

#define SVC_LOG_FDAG_IO         "FDAG_IO"

typedef struct {
    UINT32                        Use      :1;
    UINT32                        Init     :1;
    UINT32                        Reserved :30;
    AMBA_IPC_FLEXIDAGIO_CONFIG_f  ConfigCb;
} SVC_CV_FLEXI_IO_CTRL_s;

static SVC_CV_FLEXI_IO_CTRL_s  g_FlexiIOCtrl[SVC_CV_FLEXIDAG_IO_MAX_CHANNEL] GNU_SECTION_NOZEROINIT;
static AMBA_KAL_MUTEX_t FlexiIOMutex;
static UINT8 FlexiIOInit = 0U;
static UINT8 IPCReady;

static UINT32 SvcCv_FlexiIOCallback(UINT32 Channel, UINT32 OutType);
static UINT32 FlexiIO_Init(UINT32 Channel);

static void FlexiIO_MutexTake(void)
{
    if (KAL_ERR_NONE != AmbaKAL_MutexTake(&FlexiIOMutex, 5000)) {
        SvcLog_NG(SVC_LOG_FDAG_IO, "FlexiIO_MutexTake: timeout", 0U, 0U);
    }
}

static void FlexiIO_MutexGive(void)
{
    if (KAL_ERR_NONE != AmbaKAL_MutexGive(&FlexiIOMutex)) {
        SvcLog_NG(SVC_LOG_FDAG_IO, "FlexiIO_MutexGive: error", 0U, 0U);
    }
}

/**
* Initialize Flexidag IO interface
* @return 0-OK, 1-NG
*/
UINT32 SvcCv_FlexiIOInit(void)
{
    UINT32  Rval = SVC_OK;

    Rval = AmbaWrap_memset(&g_FlexiIOCtrl[0], 0, sizeof(SVC_CV_FLEXI_IO_CTRL_s) * SVC_CV_FLEXIDAG_IO_MAX_CHANNEL);
    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_FDAG_IO, "memset g_FlexiIOCtrl failed", 0U, 0U);
    }

    if (KAL_ERR_NONE != AmbaKAL_MutexCreate(&FlexiIOMutex, NULL)) {
        SvcLog_NG(SVC_LOG_FDAG_IO, "SvcCv_FlexiIOInit: Fail to MutexCreate", 0U, 0U);
        Rval = SVC_NG;
    }

    IPCReady = 0U;
    FlexiIOInit = 1U;

    return Rval;
}

/**
* De-initialize Flexidag IO interface
* @return 0-OK, 1-NG
*/
UINT32 SvcCv_FlexiIODeInit(void)
{
    UINT32  Rval = SVC_OK;

    if (KAL_ERR_NONE != AmbaKAL_MutexDelete(&FlexiIOMutex)) {
        SvcLog_NG(SVC_LOG_FDAG_IO, "SvcCv_FlexiIODeInit: Fail to MutexDelete", 0U, 0U);
        Rval = SVC_NG;
    }

    Rval = AmbaWrap_memset(&g_FlexiIOCtrl[0], 0, sizeof(SVC_CV_FLEXI_IO_CTRL_s) * SVC_CV_FLEXIDAG_IO_MAX_CHANNEL);
    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_FDAG_IO, "memset g_FlexiIOCtrl failed", 0U, 0U);
    }

    FlexiIOInit = 0U;

    return Rval;
}

/**
* Register a Flexidag IO channel
* @param [in] Channel Flexidag IO channel
* @param [in] ConfigCb callback that will be involked by remote IPC
* @return 0-OK, 1-NG
*/
UINT32 SvcCv_FlexiIORegister(UINT32 Channel, AMBA_IPC_FLEXIDAGIO_CONFIG_f ConfigCb)
{
    UINT32 Rval = SVC_OK;

    FlexiIO_MutexTake();

    if ((Channel < SVC_CV_FLEXIDAG_IO_MAX_CHANNEL) && (0U == g_FlexiIOCtrl[Channel].Use)) {
        g_FlexiIOCtrl[Channel].Use = 1U;
        g_FlexiIOCtrl[Channel].ConfigCb = ConfigCb;
        if (IPCReady == 1U) {
            Rval = FlexiIO_Init(Channel);
        }
    } else {
        Rval = SVC_NG;
        SvcLog_NG(SVC_LOG_FDAG_IO, "Fail to SvcCv_FlexiIORegister Channel(%u)", Channel, 0U);
    }

    FlexiIO_MutexGive();

    return Rval;
}

/**
* Release a Flexidag IO channel
* @param [in] Channel Flexidag IO channel
* @return 0-OK, 1-NG
*/
UINT32 SvcCv_FlexiIORelease(UINT32 Channel)
{
    UINT32 Rval = SVC_OK;

    FlexiIO_MutexTake();

    if (Channel < SVC_CV_FLEXIDAG_IO_MAX_CHANNEL) {
        Rval = AmbaIPC_FlexidagIO_Deinit(Channel);
        if (Rval != FLEXIDAGIO_OK) {
            SvcLog_NG(SVC_LOG_FDAG_IO, "AmbaIPC_FlexidagIO_Deinit(%u) failed", Channel, 0U);
        }
        g_FlexiIOCtrl[Channel].Use = 0U;
        g_FlexiIOCtrl[Channel].Init = 0U;
    } else {
        Rval = SVC_NG;
        SvcLog_NG(SVC_LOG_FDAG_IO, "Fail to SvcCv_FlexiIORelease Channel(%u)", Channel, 0U);
    }

    FlexiIO_MutexGive();

    return Rval;
}


/**
* Get status of Flexidag IO channel
* @param [in] Channel Flexidag IO channel
* @return 1-Available, 0-Not available
*/
UINT32 SvcCv_FlexiIOStatusGet(UINT32 Channel)
{
    UINT32 Rval = 0U;

    if (FlexiIOInit == 1U) {
        FlexiIO_MutexTake();
        if (g_FlexiIOCtrl[Channel].Init == 1U) {
            Rval = 1U;
        }
        FlexiIO_MutexGive();
    } else {
        SvcLog_NG(SVC_LOG_FDAG_IO, "Not initialized", 0U, 0U);
    }

    return Rval;
}

/**
* Send the system status event to Flexidag IO module
* @param [in] Event System event
* @return 0-OK, 1-NG
*/
UINT32 SvcCv_FlexiIOSysEventNotify(UINT32 Event)
{
    UINT32 Rval = SVC_OK;
    UINT32 Channel = 0;

    if (FlexiIOInit == 1U) {
        FlexiIO_MutexTake();

        if (Event == SVC_CV_FLEXIDAG_IO_IPC_READY) {
            for (Channel = 0; Channel < SVC_CV_FLEXIDAG_IO_MAX_CHANNEL; Channel++) {
                if ((g_FlexiIOCtrl[Channel].Use == 1U) && (g_FlexiIOCtrl[Channel].Init == 0U)) {
                    Rval = FlexiIO_Init(Channel);
                }
            }

            IPCReady = 1U;
        } else {
            SvcLog_NG(SVC_LOG_FDAG_IO, "Invalid Event(0x%x)", Event, 0U);
        }

        FlexiIO_MutexGive();
    } else {
        SvcLog_NG(SVC_LOG_FDAG_IO, "Not initialized", 0U, 0U);
    }

    return Rval;
}

static UINT32 SvcCv_FlexiIOCallback(UINT32 Channel, UINT32 OutType)
{
    UINT32 Rval = SVC_OK;

    if (FlexiIOInit == 1U) {
        FlexiIO_MutexTake();
        if (g_FlexiIOCtrl[Channel].ConfigCb != NULL) {
            Rval = g_FlexiIOCtrl[Channel].ConfigCb(Channel, OutType);
        } else {
            SvcLog_NG(SVC_LOG_FDAG_IO, "ConfigCb = NULL", 0U, 0U);
        }

        FlexiIO_MutexGive();
    } else {
        SvcLog_NG(SVC_LOG_FDAG_IO, "Not initialized", 0U, 0U);
    }

    return Rval;
}

static UINT32 FlexiIO_Init(UINT32 Channel)
{
    UINT32 Rval;

    Rval = AmbaIPC_FlexidagIO_Init(Channel, SvcCv_FlexiIOCallback);
    if (Rval == FLEXIDAGIO_OK) {
        g_FlexiIOCtrl[Channel].Init = 1U;
        SvcLog_OK(SVC_LOG_FDAG_IO, "AmbaIPC_FlexidagIO_Init(%u)", Channel, 0U);
    } else {
        SvcLog_NG(SVC_LOG_FDAG_IO, "AmbaIPC_FlexidagIO_Init(%u) failed", Channel, 0U);
    }

    return Rval;
}

