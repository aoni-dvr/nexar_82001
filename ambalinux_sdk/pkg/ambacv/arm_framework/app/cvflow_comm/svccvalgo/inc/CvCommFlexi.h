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

/**
 * @defgroup API_CvCommFlexi API for FlexiDAG communication
 * CvCommFlexi APIs are exported for the application code to run a FlexiDAG.
 */

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
    UINT32                  Wp;                      /*!< Write index*/
    UINT32                  AvblNum;                 /*!< The available buffer number */
    UINT32                  MaxAvblNum;              /*!< The maximum available buffer number */
    AMBA_CV_FLEXIDAG_IO_s   Buf[MAX_CCF_BUF_DEPTH];  /*!< The buffer */
    ArmMutex_t              Mutex;                   /*!< The mutex object */
} CCF_FLEXI_BUF_CTRL_s;

typedef struct {
    flexidag_memblk_t         State;  /*!< The state buffer of FlexiDAG */
    flexidag_memblk_t         Temp;   /*!< The temp buffer of FlexiDAG */
    AMBA_CV_FLEXIDAG_IO_s     *pIn;   /*!< The input buffer */
    CCF_FLEXI_BUF_CTRL_s      Out;    /*!< Buffer control for the output data */
    CCF_FLEXI_BUF_CTRL_s      In;     /*!< Buffer control for the input data */
} CCF_FLEXI_BUF_s;

/*---------------------------------------------------------------------------*\
 * Message queue items
\*---------------------------------------------------------------------------*/
typedef struct {
    AMBA_CV_FLEXIDAG_IO_s  *pIn;              /*!< The input buffer */
    AMBA_CV_FLEXIDAG_IO_s  *pOut;             /*!< The output buffer */
    void                   *pUserData;        /*!< The hooked user data */
    void                   *pCvAlgoHdlr;      /*!< The handler of the CV algorithm */
    AMBA_CV_FLEXIDAG_IO_s  *pInternal;        /*!< The internal buffer */
    UINT32                 IsInternalInBuf;   /*!< Flag for using internal input buffer */
    UINT32                 IsInternalOutBuf;  /*!< Flag for using internal output buffer */
} CCF_RUN_MQ_ITEM_s;

typedef struct {
    UINT32                 Token;             /*!< ID for getting the FlexiDAG run information */
    AMBA_CV_FLEXIDAG_IO_s  *pOut;             /*!< The output buffer */
    void                   *pUserData;        /*!< The hooked user data */
    void                   *pCvAlgoHdlr;      /*!< The handler of the CV algorithm */
    AMBA_CV_FLEXIDAG_IO_s  *pInternal;        /*!< The internal buffer */
    UINT32                 IsInternalInBuf;   /*!< Flag for using internal input buffer */
    UINT32                 IsInternalOutBuf;  /*!< Flag for using internal output buffer */
} CCF_CB_MQ_ITEM_s;

/*---------------------------------------------------------------------------*\
 * Output callback
\*---------------------------------------------------------------------------*/
typedef struct {
    AMBA_CV_FLEXIDAG_IO_s  *pOut;         /*!< The output buffer */
    void                   *pUserData;    /*!< The hooked user data */
    void                   *pCvAlgoHdlr;  /*!< The handler of the CV algorithm */
    AMBA_CV_FLEXIDAG_IO_s  *pInternal;    /*!< The internal buffer */
} CCF_OUTPUT_s;

typedef void (*CCF_OUT_CALLBACK_f)(const CCF_OUTPUT_s *pEvnetData);

/*---------------------------------------------------------------------------*\
 * CvCommFlexi handler
\*---------------------------------------------------------------------------*/
typedef struct {
    char    RunTask[MAX_CCF_NAME_LEN];    /*!< Name of the run task */
    char    CBTask[MAX_CCF_NAME_LEN];     /*!< Name of the callback task */
    char    RunMsgQ[MAX_CCF_NAME_LEN];    /*!< Name of the message queue for the run task */
    char    CBMsgQ[MAX_CCF_NAME_LEN];     /*!< Name of the message queue for the callback task */
    char    InBufMtx[MAX_CCF_NAME_LEN];   /*!< Name of the mutex for the input buffer */
    char    OutBufMtx[MAX_CCF_NAME_LEN];  /*!< Name of the mutex for the output buffer */
} CCF_RESOURCE_NAME_s;

typedef struct {
    UINT32               StackSz;   /*!< Stack size */
    UINT32               Priority;  /*!< Priority of the task */
    UINT32               CoreSel;   /*!< CPU binding */
} CCF_TSK_CTRL_s;

typedef struct {
    ArmTask_t            Tsk;       /*!< Task ID */
    flexidag_memblk_t    StackBuf;  /*!< Task stack buffer */
    CCF_TSK_CTRL_s       Ctrl;      /*!< Task control information */
} CCF_TSK_s;

typedef struct {
    /* Config */
    char                      FDName[MAX_CCF_NAME_LEN];           /*!< The algorithm name */
    UINT32                    State;                              /*!< 0:Pre-open\n 1:Opened\n 2:Inited */
    UINT32                    LogCnt;                             /*!< Print the performance log for the first N times */
    UINT32                    InBufDepth;                         /*!< The input buffer depth */
    UINT32                    OutBufDepth;                        /*!< The output buffer depth */
    UINT32                    MemPoolId;                          /*!< The memory pool identifier */

    /* Resources */
    CCF_RESOURCE_NAME_s       ResourceNames;                      /*!< Resource name */
    CCF_TSK_s                 RunTsk;                             /*!< Information of the FlexiDAG run task */
    CCF_TSK_s                 CBTsk;                              /*!< Information of the FlexiDAG output callback task */

    ArmMsgQueue_t             RunMsgQ;                            /*!< The message queue descriptor for the run task */
    CCF_RUN_MQ_ITEM_s         RunMsgQBuf[MAX_CCF_MSG_QUEUE_NUM];  /*!< The message queue buffer for the run task */
    ArmMsgQueue_t             CBMsgQ;                             /*!< The message queue descriptor for the callback task */
    CCF_CB_MQ_ITEM_s          CBMsgQBuf[MAX_CCF_MSG_QUEUE_NUM];   /*!< The message queue buffer for the callback task */

    CCF_OUTPUT_s              OutData;                            /*!< The output data information */
    CCF_OUT_CALLBACK_f        OutCallback[MAX_CCF_OUT_CB_NUM];    /*!< Callback function */

    /* Flexidag Handle */
    AMBA_CV_FLEXIDAG_HANDLE_s Handle;                             /*!< The handle of the FlexiDAG object */

    /* Flexidag Buffers */
    CCF_FLEXI_BUF_s           FlexiBuf;                           /*!< The buffer of the FlexiDAG */
} CCF_HANDLE_s;

/*---------------------------------------------------------------------------*\
 * CvCommFlexi configs
\*---------------------------------------------------------------------------*/
typedef struct {
    flexidag_memblk_t  *pBin;                     /*!< [in]  The FlexiBIN buffer copies all CVTask (that runs on the VISORC) from the file */
    UINT32             MemPoolId;                 /*!< [in]  The memory pool id (allocate buffer from it) */
    UINT32             InBufDepth;                /*!< [in]  The required input buffer depth (0: no extra input buffer allocation)\n
       The allocated size is MAX(sizeof(memio_source_recv_picinfo_t), sizeof(memio_source_recv_raw_t)) * FLEXIDAG_MAX_OUTPUTS.\n
       The purpose is to prepare AMBA_CV_FLEXIDAG_IO_s. */
    UINT32             OutBufDepth;               /*!< [in]  The required output buffer depth (0: no extra output buffer allocation)\n
       The allocated size is the output size of FlexiDAG */
    CCF_TSK_CTRL_s     RunTskCtrl;                /*!< [in]  The run task control */
    CCF_TSK_CTRL_s     CBTskCtrl;                 /*!< [in]  The callback task control */
    char               FDName[MAX_CCF_NAME_LEN];  /*!< [in]  The given FlexiDAG name */
} CCF_CREATE_CFG_s;

typedef struct {
    UINT32             OutputNum;                       /*!< [out] The output number of this felxidag */
    UINT32             OutputSz[FLEXIDAG_MAX_OUTPUTS];  /*!< [out] The size of each output (array with OutputNum) */
} CCF_OUTPUT_INFO_s;

typedef struct {
    AMBA_CV_FLEXIDAG_IO_s          *pIn;                /*!< [in]  The input data */
    memio_source_recv_multi_raw_t  *pRaw;               /*!< [in]  The input data\n CvCommFlexi will pack it to AMBA_CV_FLEXIDAG_IO_s automatically. */
    memio_source_recv_picinfo_t    *pPic;               /*!< [in]  The input data\n CvCommFlexi will pack it to AMBA_CV_FLEXIDAG_IO_s automatically. */
    AMBA_CV_FLEXIDAG_IO_s          *pOut;               /*!< [in]  The output buffer\n NULL: CvCommFlexi allocates the output buffer automatically. */
    void                           *pUserData;          /*!< [in]  The hooked user data */
    void                           *pCvAlgoHdlr;        /*!< [in]  The cvalgo handler */
    AMBA_CV_FLEXIDAG_IO_s          *pInternal;          /*!< [in]  The cvalgo internal data\n Use to put the final output data of cvalgo. */
} CCF_FEED_CFG_s;

typedef struct {
    UINT32                     Mode;      /*!< [in]  0: Register 1:Unregister */
    CCF_OUT_CALLBACK_f         Callback;  /*!< [in]  The callback function */
} CCF_REGCB_CFG_s;

typedef struct {
    UINT32                     UUID;     /*!< [in]  Universally unique identifier(UUID) in the system flow table */
    void                       *pMsg;    /*!< [in]  Buffer for the message */
    UINT32                     MsgSize;  /*!< [in]  The length of the message */
} CCF_SEND_MSG_CFG_s;

/*---------------------------------------------------------------------------*\
 * CvCommFlexi APIs
\*---------------------------------------------------------------------------*/

/**
 * @addtogroup API_CvCommFlexi API for FlexiDAG communication
 * @{
 */

/**
 *  @brief       This function opens and initializes the FlexiDAG.
 *  @details     It creates two tasks (threads) to process the request of the FlexiDAG run.
 *               Next, it returns the FlexiDAG handle and the buffer requirements of the output results.
 *  @param[in]   pHdlr     The CvCommFlexi handler.
 *  @param[in]   pCfg      The creation config.
 *  @param[out]  pOutInfo  The output information.
 *  @retval      ARM_OK    Success.
 *  @retval      ARM_NG    Error.
 */
UINT32 CvCommFlexi_Create(CCF_HANDLE_s *pHdlr, const CCF_CREATE_CFG_s *pCfg, CCF_OUTPUT_INFO_s *pOutInfo);

/**
 *  @brief       This function closes the FlexiDAG and releases the resource.
 *  @param[in]   pHdlr     The CvCommFlexi handler.
 *  @retval      ARM_OK    Success.
 *  @retval      ARM_NG    Error.
 */
UINT32 CvCommFlexi_Delete(CCF_HANDLE_s *pHdlr);

/**
 *  @brief       This function feeds data to the FlexiDAG.
 *  @param[in]   pHdlr     The CvCommFlexi handler.
 *  @param[in]   pCfg      The feed config.
 *  @retval      ARM_OK    Success.
 *  @retval      ARM_NG    Error.
 */
UINT32 CvCommFlexi_Feed(CCF_HANDLE_s *pHdlr, const CCF_FEED_CFG_s *pCfg);

/**
 *  @brief       This function registers or unregisters the FlexiDAG output callback.
 *  @param[in]   pHdlr     The CvCommFlexi handler.
 *  @param[in]   pCfg      The callback config.
 *  @retval      ARM_OK    Success.
 *  @retval      ARM_NG    Error.
 */
UINT32 CvCommFlexi_RegCallback(CCF_HANDLE_s *pHdlr, const CCF_REGCB_CFG_s *pCfg);

/**
 *  @brief       This function sends the message to CVTask.
 *  @param[in]   pHdlr     The CvCommFlexi handler.
 *  @param[in]   pCfg      The send message config.
 *  @retval      ARM_OK    Success.
 *  @retval      ARM_NG    Error.
 */
UINT32 CvCommFlexi_SendMsg(const CCF_HANDLE_s *pHdlr, const CCF_SEND_MSG_CFG_s *pCfg);

/**
 *  @brief       This function sends the private message to CVTask.
 *  @param[in]   pHdlr     The CvCommFlexi handler.
 *  @param[in]   pCfg      The send message config.
 *  @retval      ARM_OK    Success.
 *  @retval      ARM_NG    Error.
 */
UINT32 CvCommFlexi_SendPrivateMsg(const CCF_HANDLE_s *pHdlr, const CCF_SEND_MSG_CFG_s *pCfg);

/**
 *  @brief       This function prints the CVTask log to the console.
 *  @param[in]   pHdlr     The CvCommFlexi handler.
 *  @retval      ARM_OK    Success.
 *  @retval      ARM_NG    Error.
 */
UINT32 CvCommFlexi_DumpLog(const CCF_HANDLE_s *pHdlr);

/** @} */
#endif  /* CVCOMM_FLEXI_H */
