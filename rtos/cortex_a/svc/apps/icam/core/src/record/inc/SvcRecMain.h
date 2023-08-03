/**
*  @file SvcRecMain.h
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
*  @details svc record main control
*
*/

#ifndef SVC_REC_MAIN_H
#define SVC_REC_MAIN_H

#if defined(CONFIG_ICAM_RECORD_USED)

#include "SvcEnc.h"
#include "AmbaRecFrwk.h"
#include "AmbaRscInf.h"
#include "AmbaRscVA.h"
#include "AmbaRscData.h"
#include "AmbaRbxInf.h"
#include "AmbaRbxMP4.h"
#include "AmbaRdtInf.h"
#include "AmbaRdtFile.h"
#include "AmbaRdtNet.h"
#include "AmbaRdtNmlEvt.h"
#include "AmbaRdtEmgEvt.h"
#include "AmbaRecMaster.h"

#define SVC_RCM_PMT_NLWR_SET    (0U)
#define SVC_RCM_PMT_NLWR_GET    (1U)
#define SVC_RCM_RBX_IN          (2U)
#define SVC_RCM_RDT_SRCIN       (3U)
#define SVC_RCM_RDT_BOXIN       (4U)
#define SVC_RCM_PMT_VDCODING    (5U)
#define SVC_RCM_PMT_AUCODING    (6U)
#define SVC_RCM_PMT_DACODING    (7U)
#define SVC_RCM_GET_VID_STATIS  (8U)
#define SVC_RCM_GET_AUD_STATIS  (9U)
#define SVC_RCM_GET_DAT_STATIS  (10U)
#define SVC_RCM_GET_ENC_STATUS  (11U)
#define SVC_RCM_SET_BLEND       (12U)
#define SVC_RCM_SET_SCH_VIDOFF  (13U)
#define SVC_RCM_BITSCMP_ON      (14U)
#define SVC_RCM_BITSCMP_OFF     (15U)
#define SVC_RCM_BITSCMP_INFO    (16U)
#define SVC_RCM_SHMOO_CTRL      (17U)

/* callback function of record monitor */
#define SVC_REC_MON_NCODE_QUEUE_EXCEP   (0x01U)

typedef void (*PFN_REC_MON_NOTIFY)(UINT32 NotifyCode, UINT32 StrmBits);
typedef void (*PFN_REC_CAP_THM)(UINT32 StrmBits);

typedef struct {
    AMBA_REC_EVAL_s              MiaInfo;

    UINT32                       RmaPriority;
    UINT32                       RmaCpuBits;

    AMBA_RMA_USR_CFG_s*          RmaCfg;

    UINT32                       NumRsc;
    AMBA_RSC_USR_CFG_s*          RscCfgArr[CONFIG_ICAM_MAX_RSC_PER_STRM];

    UINT32                       NumRbx;
    AMBA_RBX_USR_CFG_s*          RbxCfgArr[CONFIG_ICAM_MAX_RBX_PER_STRM];

    UINT32                       NumRdt;
    AMBA_RDT_USR_CFG_s*          RdtCfgArr[CONFIG_ICAM_MAX_RDT_PER_STRM];

    UINT32                       IsXcode;
    UINT32                       IsTimeLapse;
    UINT32                       AEncBits;

    AMBA_DSP_WINDOW_DIMENSION_s  EncWin;

    char                         Drive;
} SVC_REC_FWK_CFG_s;

/* MV information */
#define MAX_MV_NUM_PER_STRM     (8U)

typedef struct {
    UINT32  StreamId;
    UINT32  CtbColNum;
    UINT32  CtbRowNum;
    ULONG   IntraBufBase;
    UINT32  IntraBufPitch;
    ULONG   MVBufBase;
    UINT32  MVBufPitch;
} SVC_ENC_MV_DATA_s;

typedef void (*PFN_REC_MV_NOTIFY)(const void *pMVData);

typedef struct {
    UINT32             *pNumStrm;
    SVC_REC_FWK_CFG_s  *pFwkStrmCfg;
    UINT32             *pQpCtrlPriority;
    UINT32             *pQpCtrlCpuBits;
    UINT32             *pRecBlendPriority;
    UINT32             *pRecBlendCpuBits;
    UINT32             *pRecMonPriority;
    UINT32             *pRecMonCpuBits;
    UINT32             *pRecMVPriority;
    UINT32             *pRecMVCpuBits;
    PFN_REC_MV_NOTIFY  *ppfnMVNotify;
    UINT32             *pNumAEnc;
    UINT32             *pBitsCmpPriority;
    UINT32             *pBitsCmpCpuBits;
} SVC_REC_MAIN_INFO_s;

#define SVC_RCM_EVT_FSTATUS     (0UL)      /* AMBA_RDT_FSTATUS_INFO_s */
#define SVC_RCM_EVT_NUM         (1UL)
typedef void (*REC_EVENT_NOTIFY)(UINT32 Event, const void *pInfo);


void SvcRecMain_InfoGet(SVC_REC_MAIN_INFO_s *pInfo);

void SvcRecMain_Init(void);
void SvcRecMain_FwkLoad(PFN_REC_MON_NOTIFY pfnMonNotify, PFN_REC_CAP_THM pfnCapThm);
void SvcRecMain_FwkUnload(void);

void SvcRecMain_Start(UINT32 StartBits, UINT32 IsBootToRec);
void SvcRecMain_Stop(UINT32 StopBits, UINT32 IsEmgStop);

void SvcRecMain_Control(UINT32 ParamType,
                        UINT16 NumStrm,
                        const UINT16 *pStreamIdx,
                        const void* pNewVal);
void SvcRecMain_Dump(void);

void SvcRecMain_EventCbRegister(UINT32 StreamBits, UINT32 Event, REC_EVENT_NOTIFY pfnEvtNotify);
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
#include "../../../../cardv/rec/rec_dvr.h"
#include "../../../../main/src/config/inc/SvcUserPref.h"
UINT32 SvcRecMain_EvtStop(UINT32 StreamBits);
int SvcRecMain_SetDebug(int enable);
#endif

#endif

#endif  /* SVC_REC_MAIN_H */
