/**
*  @file SvcCvAlgo.h
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
*   @details Header of SvcCvAlgo
*
*/

#ifndef SVC_CV_ALGO_H
#define SVC_CV_ALGO_H

#include "AmbaTypes.h"
#include "cvapi_ambacv_flexidag.h"
#include "CvCommFlexi.h"


/* Returned Value */
#define CVALGO_ERR_INVALID_API      (0xFFFFDEADU)
#define CVALGO_OK                   (0x0U)
#define CVALGO_NG                   (0xFFFFFFFFU)

/* State */
#define CVALGO_STATE_PREOPEN        (0U)
#define CVALGO_STATE_OPENED         (1U)
#define CVALGO_STATE_INITED         (2U)

/* Control Type */
#define CTRL_TYPE_ROI               (1U)        // param = amba_roi_config_t
#define CTRL_TYPE_DMSG              (2U)        // param = NULL
#define CTRL_TYPE_SEND_SECRET       (3U)        // param = 16 bytes arm_secret data
#define CTRL_TYPE_USER_BASE         (128U)

/* Callback Event */
#define CALLBACK_EVENT_FREE_INPUT   (0U)
#define CALLBACK_EVENT_OUTPUT       (1U)
#define CALLBACK_EVENT_USER_BASE    (128U)

/* Settings */
#define MAX_ALGO_FD                 (16U)
#define MAX_CALLBACK_NUM            (4U)

typedef struct SVC_CV_ALGO_OBJ SVC_CV_ALGO_OBJ_s;

typedef struct {
    AMBA_CV_FLEXIDAG_IO_s           *pOutput;                  ///< out:  The outpur data
    void                            *pUserData;                ///< out:  The user attached data
    void                            *pExtOutput;               ///< out:  User defined extend output data. NULL to ignore
} SVC_CV_ALGO_OUTPUT_s;

typedef UINT32 (*SVC_CV_ALGO_CALLBACK_f)(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData);

typedef struct {
    SVC_CV_ALGO_OBJ_s               *pAlgoObj;                  ///< The algo object
    UINT32                          MemPoolId;                  ///< Mempool ID
    UINT32                          State;                      ///< 0:Pre-open 1:Opened
    UINT32                          NumFD;                      ///< Flexidag number
    CCF_HANDLE_s                    FDs[MAX_ALGO_FD];           ///< Used Flexidags in this algo
    flexidag_memblk_t               *pAlgoBuf;                  ///< Pointer to the allocated total buffer
    void                            *pAlgoCtrl;                 ///< Pointer to proprietary cvalgo control structure
    SVC_CV_ALGO_CALLBACK_f          Callback[MAX_CALLBACK_NUM]; ///< Callback functions
} SVC_CV_ALGO_HANDLE_s;

typedef struct {
    SVC_CV_ALGO_OBJ_s               *pAlgoObj;                      ///< in:  The algo object
    UINT32                          TotalReqBufSz;                  ///< out: Total Required buffer size
    void                            *pExtQueryCfg;                  ///< in:  User defined extend query config. NULL to ignore
} SVC_CV_ALGO_QUERY_CFG_s;

typedef struct {
    UINT32                          NumFD;                          ///< in:  The given flexidag number
    flexidag_memblk_t               *pBin[MAX_ALGO_FD];             ///< in:  Buffers with loaded flexibin
    flexidag_memblk_t               *pAlgoBuf;                      ///< in:  The given algo buffer
    UINT32                          OutputNum;                      ///< out: The output number of this algo
    UINT32                          OutputSz[FLEXIDAG_MAX_OUTPUTS]; ///< out: The size of each output (array with OutputNum)
    void                            *pExtCreateCfg;                 ///< in:  User defined extend create config. NULL to ignore
} SVC_CV_ALGO_CREATE_CFG_s;

typedef struct {
    void                            *pExtDeleteCfg;            ///< in:  User defined extend delete config. NULL to ignore
} SVC_CV_ALGO_DELETE_CFG_s;

typedef struct {
    AMBA_CV_FLEXIDAG_IO_s           *pIn;                      ///< in:  The input data
    AMBA_CV_FLEXIDAG_IO_s           *pOut;                     ///< in:  The output buffer
    void                            *pUserData;                ///< in:  The hooked user data
    void                            *pExtFeedCfg;              ///< in:  User defined extend feed config. NULL to ignore
} SVC_CV_ALGO_FEED_CFG_s;

typedef struct {
    UINT32                          CtrlType;                  ///< in:  The control type
    void                            *pCtrlParam;               ///< in:  The control parameter
    void                            *pExtCtrlCfg;              ///< in:  User defined extend control config. NULL to ignore
} SVC_CV_ALGO_CTRL_CFG_s;

typedef struct {
    UINT32                          Mode;                      ///< in:  0:register 1:unregister
    SVC_CV_ALGO_CALLBACK_f          Callback;                  ///< in:  The callback function
    void                            *pExtRegcbCfg;             ///< in:  User defined extend regcb config. NULL to ignore
} SVC_CV_ALGO_REGCB_CFG_s;

struct SVC_CV_ALGO_OBJ {
    char                Name[32];
    UINT32              (*Query)(SVC_CV_ALGO_HANDLE_s* pHdlr, SVC_CV_ALGO_QUERY_CFG_s *pCfg);
    UINT32              (*Create)(SVC_CV_ALGO_HANDLE_s* pHdlr, SVC_CV_ALGO_CREATE_CFG_s *pCfg);
    UINT32              (*Delete)(SVC_CV_ALGO_HANDLE_s* pHdlr, const SVC_CV_ALGO_DELETE_CFG_s *pCfg);
    UINT32              (*Feed)(SVC_CV_ALGO_HANDLE_s* pHdlr, const SVC_CV_ALGO_FEED_CFG_s *pCfg);
    UINT32              (*Control)(SVC_CV_ALGO_HANDLE_s* pHdlr, const SVC_CV_ALGO_CTRL_CFG_s *pCfg);
} ;

typedef struct {
    struct SVC_CV_ALGO_OBJ Obj;
} SvcCvAlgo_JustToFixMisra;


UINT32 SvcCvAlgo_Query(SVC_CV_ALGO_HANDLE_s* pHdlr, SVC_CV_ALGO_QUERY_CFG_s *pCfg);
UINT32 SvcCvAlgo_Create(SVC_CV_ALGO_HANDLE_s* pHdlr, SVC_CV_ALGO_CREATE_CFG_s *pCfg);
UINT32 SvcCvAlgo_Delete(SVC_CV_ALGO_HANDLE_s* pHdlr, const SVC_CV_ALGO_DELETE_CFG_s *pCfg);
UINT32 SvcCvAlgo_Feed(SVC_CV_ALGO_HANDLE_s* pHdlr, const SVC_CV_ALGO_FEED_CFG_s *pCfg);
UINT32 SvcCvAlgo_Control(SVC_CV_ALGO_HANDLE_s* pHdlr, const SVC_CV_ALGO_CTRL_CFG_s *pCfg);
UINT32 SvcCvAlgo_RegCallback(SVC_CV_ALGO_HANDLE_s* pHdlr, const SVC_CV_ALGO_REGCB_CFG_s *pCfg);

#endif /* SVC_CV_ALGO_H */
