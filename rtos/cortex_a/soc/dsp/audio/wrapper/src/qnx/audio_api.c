/**
 *  @file dsp_api.c
 *
 *  Copyright (c) 2020 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights includiERR_IMPL, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDIERR_IMPL, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRIERR_IMPLEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDIERR_IMPL, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDIERR_IMPL NEGLIGENCE OR OTHERWISE)
 *  ARISIERR_IMPL IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Audio APIs
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdint.h>
#include <assert.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include "AmbaKAL.h"
#include "AmbaTypes.h"
#include "AmbaAudio_AENC.h"
#include "AmbaAudio_ADEC.h"
#include "AmbaAudio_AIN.h"
#include "AmbaAudio_AOUT.h"
#include "AmbaAudio_AacEnc.h"
#include "AmbaAudio_AacDec.h"
//#include "dsp_wrapper_osal.h"

/************ AENC APIs **************/
UINT32 AmbaAENC_QueryBufSize (AMBA_AUDIO_ENC_CREATE_INFO_s *pInfo)
{
    (void)pInfo;
    return ERR_IMPL;
}
UINT32 AmbaAENC_CreateResource(const AMBA_AUDIO_ENC_CREATE_INFO_s *pInfo, const AMBA_AENC_BUF_INFO_s *pCachedInfo, UINT32 **pHdlr)
{
    (void)pInfo;
    (void)pCachedInfo;
    (void)pHdlr;
    return ERR_IMPL;
}
UINT32 AmbaAENC_DeleteResource(const UINT32 *pHdlr)
{
    (void)pHdlr;
    return ERR_IMPL;
}
UINT32 AmbaAENC_ProcEnc(UINT32 *pHdlr)
{
    (void)pHdlr;
    return ERR_IMPL;
}
UINT32 AmbaAENC_Start(const UINT32 *pHdlr, UINT32 FadeInTime)
{
    (void)pHdlr;
    (void)FadeInTime;
    return ERR_IMPL;
}
UINT32 AmbaAENC_Stop(const UINT32 *pHdlr, UINT32 FadeOutTime)
{
    (void)pHdlr;
    (void)FadeOutTime;
    return ERR_IMPL;
}
UINT32 AmbaAENC_RegisterCallBackFunc(const UINT32 *pHdlr, AMBA_AENC_CB_HDLR_s *pCbHdlr, UINT32 UseTickNum, UINT32 Criteria)
{
    (void)pHdlr;
    (void)pCbHdlr;
    (void)UseTickNum;
    (void)Criteria;
    return ERR_IMPL;
}
UINT32 AmbaAENC_DeRegisterCallBackFunc(const UINT32 *pHdlr, const AMBA_AENC_CB_HDLR_s *pCbHdlr)
{
    (void)pHdlr;
    (void)pCbHdlr;
    return ERR_IMPL;
}
UINT32 AmbaAENC_OpenIoNode(const UINT32 *pHdlr, const AMBA_AENC_CB_HDLR_s *pCbHdlr)
{
    (void)pHdlr;
    (void)pCbHdlr;
    return ERR_IMPL;
}
UINT32 AmbaAENC_CloseIoNode(const UINT32 *pHdlr, const AMBA_AENC_CB_HDLR_s *pCbHdlr)
{
    (void)pHdlr;
    (void)pCbHdlr;
    return ERR_IMPL;
}
UINT32 AmbaAENC_SetUpBsBuffer(const UINT32 *pHdlr, UINT32 *pBsAddr, UINT32 BsBufSize)
{
    (void)pHdlr;
    (void)pBsAddr;
    (void)BsBufSize;
    return ERR_IMPL;
}
UINT32 AmbaAENC_ConfigEventHdlr(const UINT32 *pHdlr, UINT32 EventId, UINT32 MaxNumHandler, AMBA_AENC_EVENT_HANDLER_f *pEventHandlers)
{
    (void)pHdlr;
    (void)EventId;
    (void)MaxNumHandler;
    (void)pEventHandlers;
    return ERR_IMPL;
}
UINT32 AmbaAENC_RegisterEventHdlr(const UINT32 *pHdlr, UINT32 EventId, AMBA_AENC_EVENT_HANDLER_f EventHandler)
{
    (void)pHdlr;
    (void)EventId;
    (void)EventHandler;
    return ERR_IMPL;
}
UINT32 AmbaAENC_DeRegisterEventHdlr(const UINT32 *pHdlr, UINT32 EventId, const AMBA_AENC_EVENT_HANDLER_f EventHandler)
{
    (void)pHdlr;
    (void)EventId;
    (void)EventHandler;
    return ERR_IMPL;
}
UINT32 AmbaAENC_GetPlugInEncConfig(const UINT32 *pHdlr, void *pPlugInConfig)
{
    (void)pHdlr;
    (void)pPlugInConfig;
    return ERR_IMPL;
}
UINT32 AmbaAENC_InstallPlugInEnc(const UINT32 *pHdlr, const AMBA_AENC_PLUGIN_ENC_CS_s *pPlugInCs)
{
    (void)pHdlr;
    (void)pPlugInCs;
    return ERR_IMPL;
}
UINT32 AmbaAENC_UpdatePlugInEnc(const UINT32 *pHdlr, const void *pPlugInConfig)
{
    (void)pHdlr;
    (void)pPlugInConfig;
    return ERR_IMPL;
}

/************ ADEC APIs **************/
UINT32 AmbaADEC_QueryBufSize (AMBA_AUDIO_DEC_CREATE_INFO_s *pInfo)
{
    (void)pInfo;
    return ERR_IMPL;
}
UINT32 AmbaADEC_CreateResource(const AMBA_AUDIO_DEC_CREATE_INFO_s *pInfo, const AMBA_ADEC_BUF_INFO_s *pCachedInfo, UINT32 **pHdlr)
{
    (void)pInfo;
    (void)pCachedInfo;
    (void)pHdlr;
    return ERR_IMPL;
}
UINT32 AmbaADEC_DeleteResource(const UINT32 *pHdlr)
{
    (void)pHdlr;
    return ERR_IMPL;
}
UINT32 AmbaADEC_ProcDec(const UINT32 *pHdlr, UINT32 PreFillFrameNum)
{
    (void)pHdlr;
    (void)PreFillFrameNum;
    return ERR_IMPL;
}
UINT32 AmbaADEC_Start(const UINT32 *pHdlr)
{
    (void)pHdlr;
    return ERR_IMPL;
}
UINT32 AmbaADEC_Stop(const UINT32 *pHdlr, UINT32 Eos)
{
    (void)pHdlr;
    (void)Eos;
    return ERR_IMPL;
}
UINT32 AmbaADEC_RegisterCallBackFunc(const UINT32 *pHdlr, AMBA_ADEC_CB_HDLR_s *pCbHdlr)
{
    (void)pHdlr;
    (void)pCbHdlr;
    return ERR_IMPL;
}
UINT32 AmbaADEC_DeRegisterCallBackFunc(const UINT32 *pHdlr, const AMBA_ADEC_CB_HDLR_s *pCbHdlr)
{
    (void)pHdlr;
    (void)pCbHdlr;
    return ERR_IMPL;
}
UINT32 AmbaADEC_OpenIoNode(const UINT32 *pHdlr, const AMBA_ADEC_CB_HDLR_s *pCbHdlr)
{
    (void)pHdlr;
    (void)pCbHdlr;
    return ERR_IMPL;
}
UINT32 AmbaADEC_CloseIoNode(const UINT32 *pHdlr,const  AMBA_ADEC_CB_HDLR_s *pCbHdlr)
{
    (void)pHdlr;
    (void)pCbHdlr;
    return ERR_IMPL;
}
UINT32 AmbaADEC_SetUpBsBuffer(const UINT32 *pHdlr, UINT8 *pBsAddr, UINT32 BsBufSize)
{
    (void)pHdlr;
    (void)pBsAddr;
    (void)BsBufSize;
    return ERR_IMPL;
}
UINT32 AmbaADEC_UpdateBsBufWp(const UINT32 *pHdlr, UINT8 *pBsAddr, UINT32 UpdateSize)
{
    (void)pHdlr;
    (void)pBsAddr;
    (void)UpdateSize;
    return ERR_IMPL;
}
UINT32 AmbaADEC_ConfigEventHdlr(const UINT32 *pHdlr, UINT32 EventId, UINT32 MaxNumHandler, AMBA_ADEC_EVENT_HANDLER_f *pEventHandlers)
{
    (void)pHdlr;
    (void)EventId;
    (void)MaxNumHandler;
    (void)pEventHandlers;
    return ERR_IMPL;
}
UINT32 AmbaADEC_RegisterEventHdlr(const UINT32 *pHdlr, UINT32 EventId, AMBA_ADEC_EVENT_HANDLER_f EventHandler)
{
    (void)pHdlr;
    (void)EventId;
    (void)EventHandler;
    return ERR_IMPL;
}
UINT32 AmbaADEC_DeRegisterEventHdlr(const UINT32 *pHdlr, UINT32 EventId, AMBA_ADEC_EVENT_HANDLER_f EventHandler)
{
    (void)pHdlr;
    (void)EventId;
    (void)EventHandler;
    return ERR_IMPL;
}
UINT32 AmbaADEC_InstallPlugInDec(const UINT32 *pHdlr, const AMBA_ADEC_PLUGIN_DEC_CS_s *pPlugInCs)
{
    (void)pHdlr;
    (void)pPlugInCs;
    return ERR_IMPL;
}
UINT32 AmbaADEC_GetPlugInDecConfig(const UINT32 *pHdlr, void *pPlugInSelf)
{
    (void)pHdlr;
    (void)pPlugInSelf;
    return ERR_IMPL;
}
UINT32 AmbaADEC_UpdatePlugInDec(const UINT32 *pHdlr, const void *pPlugInSelf)
{
    (void)pHdlr;
    (void)pPlugInSelf;
    return ERR_IMPL;
}

/************ AIN APIs **************/
UINT32 AmbaAIN_QueryBufSize(AMBA_AIN_IO_CREATE_INFO_s *pInfo)
{
    (void)pInfo;
    return ERR_IMPL;
}
UINT32 AmbaAIN_CreateResource(const AMBA_AIN_IO_CREATE_INFO_s *pInfo,
                              const AMBA_AIN_BUF_INFO_s *pCachedInfo,
                              const AMBA_AIN_BUF_INFO_s *pNonCachedInfo,
                              UINT32 **pHdlr)
{
    (void)pInfo;
    (void)pCachedInfo;
    (void)pNonCachedInfo;
    (void)pHdlr;
    return ERR_IMPL;
}
UINT32 AmbaAIN_DeleteResource(const UINT32 *pHdlr)
{
    (void)pHdlr;
    return ERR_IMPL;
}
UINT32 AmbaAIN_ProcDMA(const UINT32 *pHdlr, UINT32 *pLof)
{
    (void)pHdlr;
    (void)pLof;
    return ERR_IMPL;
}
UINT32 AmbaAIN_Prepare(const UINT32 *pHdlr)
{
    (void)pHdlr;
    return ERR_IMPL;
}
UINT32 AmbaAIN_Stop(const UINT32 *pHdlr)
{
    (void)pHdlr;
    return ERR_IMPL;
}
UINT32 AmbaAIN_RegisterCallBackFunc(const UINT32 *pHdlr, AMBA_AIN_CB_HDLR_s *pCbHdlr)
{
    (void)pHdlr;
    (void)pCbHdlr;
    return ERR_IMPL;
}
UINT32 AmbaAIN_DeRegisterCallBackFunc(const UINT32 *pHdlr, const AMBA_AIN_CB_HDLR_s *pCbHdlr)
{
    (void)pHdlr;
    (void)pCbHdlr;
    return ERR_IMPL;
}
UINT32 AmbaAIN_OpenIoNode(const UINT32 *pHdlr, const AMBA_AIN_CB_HDLR_s *pCbHdlr)
{
    (void)pHdlr;
    (void)pCbHdlr;
    return ERR_IMPL;
}
UINT32 AmbaAIN_CloseIoNode(const UINT32 *pHdlr, const AMBA_AIN_CB_HDLR_s *pCbHdlr)
{
    (void)pHdlr;
    (void)pCbHdlr;
    return ERR_IMPL;
}
UINT32 AmbaAIN_ConfigEventHdlr(const UINT32 *pHdlr, UINT32 EventId,
                               UINT32 MaxNumHandler,
                               AMBA_AIN_EVENT_HANDLER_f *pEventHandlers)
{
    (void)pHdlr;
    (void)EventId;
    (void)MaxNumHandler;
    (void)pEventHandlers;
    return ERR_IMPL;
}
UINT32 AmbaAIN_RegisterEventHdlr(const UINT32 *pHdlr, UINT32 EventId,
                                 AMBA_AIN_EVENT_HANDLER_f EventHandler)
{
    (void)pHdlr;
    (void)EventId;
    (void)EventHandler;
    return ERR_IMPL;
}
UINT32 AmbaAIN_DeRegisterEventHdlr(const UINT32 *pHdlr, UINT32 EventId,
                                   AMBA_AIN_EVENT_HANDLER_f EventHandler)
{
    (void)pHdlr;
    (void)EventId;
    (void)EventHandler;
    return ERR_IMPL;
}

/************ AOUT APIs **************/
UINT32 AmbaAOUT_QueryBufSize(AMBA_AOUT_IO_CREATE_INFO_s *pInfo)
{
    (void)pInfo;
    return ERR_IMPL;
}
UINT32 AmbaAOUT_CreateResource(const AMBA_AOUT_IO_CREATE_INFO_s *pInfo,
                               const AMBA_AOUT_BUF_INFO_s *pCachedInfo,
                               const AMBA_AOUT_BUF_INFO_s *pNonCachedInfo,
                               UINT32 **pHdlr)
{
    (void)pInfo;
    (void)pCachedInfo;
    (void)pNonCachedInfo;
    (void)pHdlr;
    return ERR_IMPL;
}
UINT32 AmbaAOUT_DeleteResource(const UINT32 *pHdlr)
{
    (void)pHdlr;
    return ERR_IMPL;
}
UINT32 AmbaAOUT_Prepare(const UINT32 *pHdlr)
{
    (void)pHdlr;
    return ERR_IMPL;
}
UINT32 AmbaAOUT_ProcDMA(const UINT32 *pHdlr, UINT32 *pLof)
{
    (void)pHdlr;
    (void)pLof;
    return ERR_IMPL;
}
UINT32 AmbaAOUT_Stop(const UINT32 *pHdlr)
{
    (void)pHdlr;
    return ERR_IMPL;
}
UINT32 AmbaAOUT_StopWithLof(const UINT32 *pHdlr)
{
    (void)pHdlr;
    return ERR_IMPL;
}
UINT32 AmbaAOUT_Pause(const UINT32 *pHdlr)
{
    (void)pHdlr;
    return ERR_IMPL;
}
UINT32 AmbaAOUT_RegisterCallBackFunc(const UINT32 *pHdlr, AMBA_AOUT_CB_HDLR_s *pCbHdlr)
{
    (void)pHdlr;
    (void)pCbHdlr;
    return ERR_IMPL;
}
UINT32 AmbaAOUT_DeRegisterCallBackFunc(const UINT32 *pHdlr, const AMBA_AOUT_CB_HDLR_s *pCbHdlr)
{
    (void)pHdlr;
    (void)pCbHdlr;
    return ERR_IMPL;
}
UINT32 AmbaAOUT_OpenIoNode(const UINT32 *pHdlr, const AMBA_AOUT_CB_HDLR_s *pCbHdlr)
{
    (void)pHdlr;
    (void)pCbHdlr;
    return ERR_IMPL;
}
UINT32 AmbaAOUT_CloseIoNode(const UINT32 *pHdlr, const AMBA_AOUT_CB_HDLR_s *pCbHdlr)
{
    (void)pHdlr;
    (void)pCbHdlr;
    return ERR_IMPL;
}
UINT32 AmbaAOUT_ConfigEventHdlr(const UINT32 *pHdlr, UINT32 EventId,
                                UINT32 MaxNumHandler,
                                AMBA_AOUT_EVENT_HANDLER_f *pEventHandlers)
{
    (void)pHdlr;
    (void)EventId;
    (void)MaxNumHandler;
    (void)pEventHandlers;
    return ERR_IMPL;
}
UINT32 AmbaAOUT_RegisterEventHdlr(const UINT32 *pHdlr, UINT32 EventId,
                                  AMBA_AOUT_EVENT_HANDLER_f EventHandler)
{
    (void)pHdlr;
    (void)EventId;
    (void)EventHandler;
    return ERR_IMPL;
}
UINT32 AmbaAOUT_DeRegisterEventHdlr(const UINT32 *pHdlr, UINT32 EventId,
                                    AMBA_AOUT_EVENT_HANDLER_f EventHandler)
{
    (void)pHdlr;
    (void)EventId;
    (void)EventHandler;
    return ERR_IMPL;
}

/************ AAC ENC APIs **************/
INT32 aacenc_get_mem_size(UINT32 max_ch_num, UINT32 max_sample_freq, UINT32 max_bitrate, UINT32 mode)
{
    (void)max_ch_num;
    (void)max_sample_freq;
    (void)max_bitrate;
    (void)mode;
    return 0;
}
void aacenc_setup(au_aacenc_config_t *pAacEncConfig)
{
    (void)pAacEncConfig;
}
void aacenc_open(au_aacenc_config_t *pAacEncConfig)
{
    (void)pAacEncConfig;
}
void aacenc_encode(au_aacenc_config_t *pAacEncConfig)
{
    (void)pAacEncConfig;
}
void AmbaLibAacEnc_GetVerInfo(AMBA_VerInfo_s *pVerInfo)
{
    (void)pVerInfo;
}

/************ AAC DEC APIs **************/
void aacdec_setup(au_aacdec_config_t *pAacDecConfig)
{
    (void)pAacDecConfig;
}
void aacdec_open(au_aacdec_config_t *pAacDecConfig)
{
    (void)pAacDecConfig;
}
void aacdec_decode(au_aacdec_config_t *pAacDecConfig)
{
    (void)pAacDecConfig;
}
void AmbaLibAacDec_GetVerInfo(AMBA_VerInfo_s *pVerInfo)
{
    (void)pVerInfo;
}
