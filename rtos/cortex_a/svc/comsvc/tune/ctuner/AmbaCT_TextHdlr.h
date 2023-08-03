/**
 *  @file AmbaCT_TextHdlr.h
 *
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
 */
#ifndef CT_TEXT_HDLR_H
#define CT_TEXT_HDLR_H
#include "AmbaTypes.h"
#include "AmbaCalib_Def.h"

typedef enum {
    AMBA_CT_TYPE_VIG = 0,
    AMBA_CT_TYPE_1D_VIG,
    AMBA_CT_TYPE_BPC,
    AMBA_CT_TYPE_AVM,
    AMBA_CT_TYPE_EMIRROR,
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV2FS)
    AMBA_CT_TYPE_STEREO,
#endif
    AMBA_CT_TYPE_LDC,
    AMBA_CT_TYPE_LDCC,
    AMBA_CT_TYPE_OC,
    AMBA_CT_TYPE_OCCB,
    AMBA_CT_TYPE_CA,
    AMBA_CT_TYPE_MAX,
} AMBA_CT_TYPE_e;

typedef struct {
    void *pTunerWorkingBuf;
    SIZE_t TunerWorkingBufSize;
    struct {
        UINT32 (*FeedRawFunc)(SIZE_t RawBufSize, UINT32 *pPitch, AMBA_CAL_ROI_s *pValidArea, UINT16 *pRaw);
    } Vig;
    struct {
        void *pCalibWorkingBuf;
        SIZE_t CalibWorkingBufSize;
    } Vig1d;
    struct {
        void *pCalibWorkingBuf;
        SIZE_t CalibWorkingBufSize;
    } Avm;
    struct {
        void *pCalibWorkingBuf;
        SIZE_t CalibWorkingBufSize;
        UINT32 (*CbFeedMaskFunc)(UINT32 CamId, UINT32 *pWidth, UINT32 *pHeight, UINT8 *pTable);
    } Emirror;
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV2FS)
    struct {
        void *pCalibWorkingBuf;
        SIZE_t CalibWorkingBufSize;
    } Stereo;
#endif
    struct {
        void *pCalibWorkingBuf;
        SIZE_t CalibWorkingBufSize;
    } Ldc;
    struct {
        void *pPatDetWorkingBuf;
        SIZE_t PatDetWorkingBufSize;
        void *pOrganizeCBCornerWorkingBuf;
        SIZE_t OrganizeCBCornerWorkingBufSize;
        void *pOCFinderWorkingBuf;
        SIZE_t OCFinderWorkingBufSize;
    } OcCb;

    struct {
        void *pCalibWorkingBuf;
        SIZE_t CalibWorkingBufSize;
    } Oc;

    struct {
        void *pCalibWorkingBuf;
        SIZE_t CalibWorkingBufSize;
    } Ldcc;

    struct {
        void *pCalibWorkingBuf;
        SIZE_t CalibWorkingBufSize;
    } Ca;
    struct {
        void *pCalibWorkingBuf;
        SIZE_t CalibWorkingBufSize;
    } Bpc;
} AMBA_CT_INITIAL_CONFIG_s;

typedef struct {
    struct {
        struct {
            AMBA_CAL_SIZE_s MaxImageSize;
            UINT32 Resolution;
        } Vig1d;
        struct {
            AMBA_CAL_SIZE_s MaxImageSize;
            UINT32 Resolution;
        } Bpc;
    } Type;
} AMBA_CT_CONFIG_s;
UINT32 AmbaCT_Cfg(AMBA_CT_TYPE_e Type, const AMBA_CT_CONFIG_s *pCfg);
UINT32 AmbaCT_GetBufSize(AMBA_CT_TYPE_e Type, SIZE_t *pSize);
UINT32 AmbaCT_Init(AMBA_CT_TYPE_e Type, const AMBA_CT_INITIAL_CONFIG_s *pInitCfg);
UINT32 AmbaCT_Load(const char *pTunerFileName);
UINT32 AmbaCT_Execute(void);
UINT32 AmbaCT_DumpCalibData(void);
#endif
