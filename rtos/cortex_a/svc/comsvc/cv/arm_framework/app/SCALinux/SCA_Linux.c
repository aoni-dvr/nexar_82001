/**
 *  @file SCA_OpenOD.c
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
 *  @details Implementation of SCA OpenOD
 *
 */
#include "AmbaTypes.h"
#include "cvapi_memio_interface.h"
#include "cvapi_ambacv_flexidag.h"
#include "idsp_roi_msg.h"
#include "AmbaFS.h"
#include "AmbaPrint.h"
#include "AmbaMisraFix.h"
#include "AmbaWrap.h"
#include "AmbaUtility.h"
#include "AmbaMMU.h"
#include "AmbaCache.h"
#include "AmbaNVM_Partition.h"
#include "SvcCvAlgo.h"
#include "cvapi_svccvalgo_linux.h"

#define SCA_LINUX_MAX_INSTANCE (4U)

#define SCA_LINUX_INSTANCE0     (0U)
#define SCA_LINUX_INSTANCE1     (1U)
#define SCA_LINUX_INSTANCE2     (2U)
#define SCA_LINUX_INSTANCE3     (3U)


#ifdef CONFIG_AMBALINK_BOOT_OS
#include "AmbaIPC_FlexidagIO.h"
#endif

//static void CCFCbHdlrFD0(const CCF_OUTPUT_s *pEvnetData)

static SVC_CV_ALGO_HANDLE_s* pHdlrList[SCA_LINUX_MAX_INSTANCE];

static UINT32 CCFCbHdlrFD(UINT32 Inst, UINT32 Channel, const void *pData, UINT32 Len)
{
    const SVC_CV_ALGO_HANDLE_s* pHdlr;
    UINT32 I;
    static SVC_CV_ALGO_OUTPUT_s  SvcCvAlgoOut;
    static AMBA_CV_FLEXIDAG_IO_s Out;
    static memio_sink_send_out_t* pSinkOut;

    AmbaMisra_TypeCast(&pHdlr, &pHdlrList[Inst]);
    AmbaMisra_TypeCast(&pSinkOut, &pData);

    //AmbaPrint_PrintUInt5("CCFCbHdlrFD, Inst = %d Channel = %d", Inst, Channel, 0U, 0U, 0U);
    if (Len == sizeof(memio_sink_send_out_t)) {
        Out.num_of_buf = pSinkOut->num_of_io;
        for (UINT32 i = 0; i < Out.num_of_buf; i++) {
            AmbaMisra_TypeCast32(&Out.buf[i].pBuffer, &pSinkOut->io[i].addr);
            Out.buf[i].buffer_daddr     = pSinkOut->io[i].addr;
            Out.buf[i].buffer_size      = pSinkOut->io[i].size;
            Out.buf[i].buffer_cacheable = 1;
        }

        SvcCvAlgoOut.pOutput    = &Out;
        SvcCvAlgoOut.pUserData  = &pSinkOut->cvtask_frameset_id;
        SvcCvAlgoOut.pExtOutput = NULL;

        for (I=0 ; I<MAX_CALLBACK_NUM ; I++) {
            if (pHdlr->Callback[I] != NULL) {
                (void)pHdlr->Callback[I](CALLBACK_EVENT_FREE_INPUT, &SvcCvAlgoOut);
                (void)pHdlr->Callback[I](CALLBACK_EVENT_OUTPUT, &SvcCvAlgoOut);
            }
        }
    } else {
        AmbaPrint_PrintStr5("## AmbaIPC_FlexidagIO_GetResult() fail GetLen", NULL, NULL, NULL, NULL, NULL);
    }

    (void)Channel;
    (void)Len;
    return CVALGO_OK;
}

static UINT32 CCFCbHdlrFD0(UINT32 Channel, const void *pData, UINT32 Len)
{
    return CCFCbHdlrFD(SCA_LINUX_INSTANCE0, Channel, pData, Len);
}

static UINT32 CCFCbHdlrFD1(UINT32 Channel, const void *pData, UINT32 Len)
{
    return CCFCbHdlrFD(SCA_LINUX_INSTANCE1, Channel, pData, Len);
}

static UINT32 CCFCbHdlrFD2(UINT32 Channel, const void *pData, UINT32 Len)
{
    return CCFCbHdlrFD(SCA_LINUX_INSTANCE2, Channel, pData, Len);
}

static UINT32 CCFCbHdlrFD3(UINT32 Channel, const void *pData, UINT32 Len)
{
    return CCFCbHdlrFD(SCA_LINUX_INSTANCE3, Channel, pData, Len);
}

static UINT32 Linux_Query(SVC_CV_ALGO_HANDLE_s* Hdlr, SVC_CV_ALGO_QUERY_CFG_s *Cfg)
{
    // assign obj
    Hdlr->pAlgoObj = Cfg->pAlgoObj;
    (void)Cfg;
    return CVALGO_OK;
}

static UINT32 Linux_Control(SVC_CV_ALGO_HANDLE_s* Hdlr, const SVC_CV_ALGO_CTRL_CFG_s *Cfg)
{

    (void)Hdlr;
    (void)Cfg;

    return CVALGO_OK;
}

static UINT32 Linux_Create(SVC_CV_ALGO_HANDLE_s* Hdlr, SVC_CV_ALGO_CREATE_CFG_s *Cfg)
{
    UINT32 Ret;
    UINT32 AmbaIPCChannel;
    const SVC_CV_ALGO_LINUX_EX_CFG_s *pExCfg;
    static AMBA_IPC_FLEXIDAGIO_RESULT_f CCFCbHdlrList[SCA_LINUX_MAX_INSTANCE] = {
        CCFCbHdlrFD0,
        CCFCbHdlrFD1,
        CCFCbHdlrFD2,
        CCFCbHdlrFD3,
    };

    AmbaMisra_TypeCast(&pExCfg, &Cfg->pExtCreateCfg);
    AmbaIPCChannel = pExCfg->AmbaIPCChannel;
    AmbaMisra_TypeCast(&pHdlrList[AmbaIPCChannel], &Hdlr);
    //AmbaPrint_PrintUInt5("Linux_Create, AmbaIPCChannel = %d", AmbaIPCChannel, 0U, 0U, 0U, 0U);

    Ret = AmbaIPC_FlexidagIO_GetResult_SetCB(AmbaIPCChannel, CCFCbHdlrList[AmbaIPCChannel]);

    (void)Cfg;
    (void)Hdlr;
    return Ret;
}

static UINT32 Linux_Delete(SVC_CV_ALGO_HANDLE_s* Hdlr, const SVC_CV_ALGO_DELETE_CFG_s *Cfg) //V
{
    (void)Hdlr;
    (void)Cfg;

    return CVALGO_OK;
}

static UINT32 Linux_Feed(SVC_CV_ALGO_HANDLE_s* Hdlr, const SVC_CV_ALGO_FEED_CFG_s *Cfg)
{
#ifdef CONFIG_AMBALINK_BOOT_OS
    UINT32 Ret;
    const SVC_CV_ALGO_LINUX_EX_CFG_s *pExCfg;

    (void)Hdlr;
    AmbaMisra_TypeCast(&pExCfg, &Cfg->pExtFeedCfg);
    //AmbaPrint_PrintUInt5("Linux_Feed, AmbaIPCChannel = %d", pExCfg->AmbaIPCChannel, 0U, 0U, 0U, 0U);

    Ret = AmbaIPC_FlexidagIO_SetInput(pExCfg->AmbaIPCChannel, Cfg->pIn->buf[0].pBuffer, pExCfg->ValidDataSize);
    if (Ret != FLEXIDAGIO_OK) {
        AmbaPrint_PrintStr5("%s, AmbaIPC_FlexidagIO_SetInput() 0 failed!", __func__, NULL, NULL, NULL, NULL);
    }

#else
    (void)Hdlr;
    (void)Cfg;
#endif

    return CVALGO_OK;
}

SVC_CV_ALGO_OBJ_s LinuxAlgoObj = {
    .Query = Linux_Query,
    .Create = Linux_Create,
    .Delete = Linux_Delete,
    .Feed = Linux_Feed,
    .Control = Linux_Control
};

