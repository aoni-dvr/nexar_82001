/**
*  @file CvCommFlexi.h
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
*  @details CV communication (The communication between cvtask and app) for flexidag
*
*/

#ifndef CVCOMM_FLEXI_H
#define CVCOMM_FLEXI_H

/* AmbaCV header */
#include "cvapi_ambacv_flexidag.h"
#include "cvapi_idsp_interface.h"
#include "cvapi_memio_interface.h"
#include "cvapi_svccvalgo_memio_interface.h"
#include "idsp_roi_msg.h"

/* ArmUtil header */
#include "ArmLog.h"
#include "ArmErrCode.h"
#include "ArmStdC.h"
#include "ArmFIO.h"
#include "ArmTask.h"
#include "ArmEventFlag.h"
#include "ArmMutex.h"
#include "ArmMsgQueue.h"
#include "ArmMemPool.h"

/* CCF settings */
#define MAX_CCF_BUF_DEPTH             (32U)
#define MAX_CCF_NAME_LEN              (32U)
#define MAX_CCF_MSG_QUEUE_NUM         (16U)
#define MAX_CCF_OUT_CB_NUM            (3U)
#define DEFAULT_CCF_LOG_CNT           (3U)

/* CCF state */
#define CCF_STATE_PREOPEN             (0U)
#define CCF_STATE_OPENED              (1U)
#define CCF_STATE_INITED              (2U)

/* CCF task settings */
#define DEFAULT_TSK_STACK_SIZE        (0x10000U)
#define DEFAULT_TSK_PRIORITY          (75U)
#define DEFAULT_TSK_CORE_SEL          ARM_TASK_CORE0

/*---------------------------------------------------------------------------*\
 * Flexidag buffer control
\*---------------------------------------------------------------------------*/
typedef struct {
    UINT32                  Wp;               /* Write index*/
    UINT32                  AvblNum;          /* Available buffer number */
    UINT32                  MaxAvblNum;       /* The maximum available buffer number */
    AMBA_CV_FLEXIDAG_IO_s   Buf[MAX_CCF_BUF_DEPTH];
    ArmMutex_t              Mutex;            /* Protect AvblNum */
} CCF_FLEXI_BUF_CTRL_s;

typedef struct {
    flexidag_memblk_t         State;
    flexidag_memblk_t         Temp;
    AMBA_CV_FLEXIDAG_IO_s     *pIn;
    CCF_FLEXI_BUF_CTRL_s      Out;
    CCF_FLEXI_BUF_CTRL_s      In;
} CCF_FLEXI_BUF_s;

/*---------------------------------------------------------------------------*\
 * Message queue items
\*---------------------------------------------------------------------------*/
typedef struct {
    AMBA_CV_FLEXIDAG_IO_s  *pIn;
    AMBA_CV_FLEXIDAG_IO_s  *pOut;
    void                   *pUserData;
    void                   *pCvAlgoHdlr;
    AMBA_CV_FLEXIDAG_IO_s  *pInternal;
    UINT32                 IsInternalInBuf;
    UINT32                 IsInternalOutBuf;
} CCF_RUN_MQ_ITEM_s;

typedef struct {
    UINT32                 Token;
    AMBA_CV_FLEXIDAG_IO_s  *pOut;
    void                   *pUserData;
    void                   *pCvAlgoHdlr;
    AMBA_CV_FLEXIDAG_IO_s  *pInternal;
    UINT32                 IsInternalInBuf;
    UINT32                 IsInternalOutBuf;
} CCF_CB_MQ_ITEM_s;

/*---------------------------------------------------------------------------*\
 * Output callback
\*---------------------------------------------------------------------------*/
typedef struct {
    AMBA_CV_FLEXIDAG_IO_s  *pOut;
    void                   *pUserData;
    void                   *pCvAlgoHdlr;
    AMBA_CV_FLEXIDAG_IO_s  *pInternal;
} CCF_OUTPUT_s;

typedef void (*CCF_OUT_CALLBACK_f)(const CCF_OUTPUT_s *pEvnetData);

/*---------------------------------------------------------------------------*\
 * CvCommFlexi handler
\*---------------------------------------------------------------------------*/
typedef struct {
    char    RunTask[MAX_CCF_NAME_LEN];
    char    CBTask[MAX_CCF_NAME_LEN];
    char    RunMsgQ[MAX_CCF_NAME_LEN];
    char    CBMsgQ[MAX_CCF_NAME_LEN];
    char    InBufMtx[MAX_CCF_NAME_LEN];
    char    OutBufMtx[MAX_CCF_NAME_LEN];
} CCF_RESOURCE_NAME_s;

typedef struct {
    UINT32               StackSz;
    UINT32               Priority;
    UINT32               CoreSel;
} CCF_TSK_CTRL_s;

typedef struct {
    ArmTask_t            Tsk;
    flexidag_memblk_t    StackBuf;
    CCF_TSK_CTRL_s       Ctrl;
} CCF_TSK_s;

typedef struct {
    /* Config */
    char                      FDName[MAX_CCF_NAME_LEN];
    UINT32                    State;       // 0:pre-open 1:Opened 2:Inited
    UINT32                    LogCnt;      // Print the performance log for the first N times
    UINT32                    InBufDepth;
    UINT32                    OutBufDepth;
    UINT32                    MemPoolId;

    /* Resources */
    CCF_RESOURCE_NAME_s       ResourceNames;
    CCF_TSK_s                 RunTsk;
    CCF_TSK_s                 CBTsk;

    ArmMsgQueue_t             RunMsgQ;
    CCF_RUN_MQ_ITEM_s         RunMsgQBuf[MAX_CCF_MSG_QUEUE_NUM];
    ArmMsgQueue_t             CBMsgQ;
    CCF_CB_MQ_ITEM_s          CBMsgQBuf[MAX_CCF_MSG_QUEUE_NUM];

    CCF_OUTPUT_s              OutData;
    CCF_OUT_CALLBACK_f        OutCallback[MAX_CCF_OUT_CB_NUM];

    /* Flexidag Handle */
    AMBA_CV_FLEXIDAG_HANDLE_s Handle;

    /* Flexidag Buffers */
    CCF_FLEXI_BUF_s           FlexiBuf;
} CCF_HANDLE_s;

/*---------------------------------------------------------------------------*\
 * CvCommFlexi configs
\*---------------------------------------------------------------------------*/
typedef struct {
    flexidag_memblk_t  *pBin;                           ///< in:  The buffer which contain loaded flexidag
    UINT32             MemPoolId;                       ///< in:  The memory pool id (allocate buffer from it)
    UINT32             InBufDepth;                      ///< in:  The required input buffer depth (0: no extra input buffer allocation)
    /* The allocated size is MAX(sizeof(memio_source_recv_picinfo_t), sizeof(memio_source_recv_raw_t)) * FLEXIDAG_MAX_OUTPUTS
       The purpose is to prepare AMBA_CV_FLEXIDAG_IO_s */
    UINT32             OutBufDepth;                     ///< in:  The required output buffer depth (0: no extra output buffer allocation)
    /* The allocated size is the output size of flexidag */
    CCF_TSK_CTRL_s     RunTskCtrl;                      ///< in:  The run task control
    CCF_TSK_CTRL_s     CBTskCtrl;                       ///< in:  The callback task control
    char               FDName[MAX_CCF_NAME_LEN];        ///< in:  The given flexidag name
} CCF_CREATE_CFG_s;

typedef struct {
    UINT32             OutputNum;                       ///< out: The output number of this felxidag
    UINT32             OutputSz[FLEXIDAG_MAX_OUTPUTS];  ///< out: The size of each output (array with OutputNum)
} CCF_OUTPUT_INFO_s;

typedef struct {
    AMBA_CV_FLEXIDAG_IO_s          *pIn;                ///< in:  The input data
    memio_source_recv_multi_raw_t  *pRaw;               ///< in:  The input data (CvCommFlexi will pack it to AMBA_CV_FLEXIDAG_IO_s automatically)
    memio_source_recv_picinfo_t    *pPic;               ///< in:  The input data (CvCommFlexi will pack it to AMBA_CV_FLEXIDAG_IO_s automatically)
    AMBA_CV_FLEXIDAG_IO_s          *pOut;               ///< in:  The output buffer (NULL: CvCommFlexi allocate output buffer automatically)
    void                           *pUserData;          ///< in:  The hooked user data
    void                           *pCvAlgoHdlr;        ///< in:  The cvalgo handler pointer
    AMBA_CV_FLEXIDAG_IO_s          *pInternal;          ///< in:  The cvalgo internal data pointer (Use to put the final output data of cvalgo)
} CCF_FEED_CFG_s;

typedef struct {
    UINT32                     Mode;            ///< in:  0:register 1:unregister
    CCF_OUT_CALLBACK_f         Callback;        ///< in:  The callback function
} CCF_REGCB_CFG_s;

typedef struct {
    UINT32                     UUID;            ///< in:  The UUID
    void                       *pMsg;           ///< in:  Buffer to the message
    UINT32                     MsgSize;         ///< in:  The length of the message
} CCF_SEND_MSG_CFG_s;

/*---------------------------------------------------------------------------*\
 * CvCommFlexi APIs
\*---------------------------------------------------------------------------*/
UINT32 CvCommFlexi_Create(CCF_HANDLE_s *pHdlr, const CCF_CREATE_CFG_s *pCfg, CCF_OUTPUT_INFO_s *pOutInfo);
UINT32 CvCommFlexi_Delete(CCF_HANDLE_s *pHdlr);

UINT32 CvCommFlexi_Feed(CCF_HANDLE_s *pHdlr, const CCF_FEED_CFG_s *pCfg);
UINT32 CvCommFlexi_RegCallback(CCF_HANDLE_s *pHdlr, const CCF_REGCB_CFG_s *pCfg);
UINT32 CvCommFlexi_SendMsg(const CCF_HANDLE_s *pHdlr, const CCF_SEND_MSG_CFG_s *pCfg);
UINT32 CvCommFlexi_SendPrivateMsg(const CCF_HANDLE_s *pHdlr, const CCF_SEND_MSG_CFG_s *pCfg);
UINT32 CvCommFlexi_DumpLog(const CCF_HANDLE_s *pHdlr);

#endif  /* CVCOMM_FLEXI_H */
