/**
*  @file CtCvAlgoWrapper.h
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
*  @details The wrapper for SvcCvAlgo
*
*/

#ifndef CT_CVALGO_WRAPPER_H
#define CT_CVALGO_WRAPPER_H

/* SvcCvAlgo interface */
#include "SvcCvAlgo.h"

/* AmbaCV header */
#include "cvapi_ambacv_flexidag.h"
#include "cvapi_idsp_interface.h"
#include "cvapi_memio_interface.h"
#include "cvapi_svccvalgo_memio_interface.h"
#include "idsp_roi_msg.h"

/* Default Settings */
#define MAX_BUF_DEPTH          (4U)
#define MAX_SLOT_NUM           (4U)
#define MAX_PATH_LEN           (32U)

typedef struct {
  SVC_CV_ALGO_OBJ_s         *pCvAlgoObj;
  UINT32                    MemPoolId;
  UINT32                    NumFD;
  char                      Flexibin[MAX_ALGO_FD][MAX_PATH_LEN];
//  UINT32     LoadMode;    // From SD or ROMFS
} CT_CVALGO_WRAPPER_CREATE_CFG_s;

/*---------------------------------------------------------------------------*\
 * CtCvAlgoWrapper APIs
\*---------------------------------------------------------------------------*/
UINT32 CtCvAlgoWrapper_Create(UINT32 Slot, const CT_CVALGO_WRAPPER_CREATE_CFG_s *pCfg);
UINT32 CtCvAlgoWrapper_Delete(UINT32 Slot);

UINT32 CtCvAlgoWrapper_FeedRaw(UINT32 Slot, const memio_source_recv_multi_raw_t *pRaw, void *pUserData);
UINT32 CtCvAlgoWrapper_FeedPicinfo(UINT32 Slot, const memio_source_recv_picinfo_t *pPic, void *pUserData);
UINT32 CtCvAlgoWrapper_RegCb(UINT32 Slot, UINT32 Mode, SVC_CV_ALGO_CALLBACK_f Callback);
UINT32 CtCvAlgoWrapper_Control(UINT32 Slot, UINT32 CtrlType, void *pCtrlParam);

#endif  /* CT_CVALGO_WRAPPER_H */
