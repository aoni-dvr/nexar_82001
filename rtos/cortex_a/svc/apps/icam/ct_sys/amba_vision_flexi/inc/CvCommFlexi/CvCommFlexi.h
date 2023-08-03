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
#include "ArmMem.h"
#include "ArmMemPool.h"

/* Daemon IO header */
#include "DaemonIO.h"

/* Pic-info mode */
#define MAX_YUV_SIZE                  (0x870000U)  // 8MB for 4096 x 2160

/* Advance batch mode */
#define AVB_OFFSET_NUM                (256U)

/* Flexidag mode */
#define CCF_MODE_RAW                  (0U)
#define CCF_MODE_PIC_INFO             (1U)
#define MAX_CCF_MODE                  (CCF_MODE_PIC_INFO + 1)

/* Buffer management */
#define MAX_CCF_INPUT_IO_NUM          (FLEXIDAG_MAX_OUTPUTS)
#define MAX_CCF_OUTPUT_IO_NUM         (FLEXIDAG_MAX_OUTPUTS)
#define MAX_CCF_OUTPUT_BUF_DEPTH      (32U)
#define DEFAULT_CCF_OUTPUT_BUF_DEPTH  (8U)

/* CCF settings */
#define MAX_CCF_OUT_CB                (3U)
#define MAX_CCF_SLOT                  (4U)
#define MAX_CCF_MSG_QUEUE_NUM         (16U)
#define MAX_CCF_PATH_LEN              (256)
#define DEFAULT_CCF_LOG_CNT           (3U)

/* CCF task settings */
#define CCF_RUN_TASK_STACK_SIZE       (0x10000U)
#define CCF_RUN_TASK_PRIORITY         (75U)
#define CCF_RUN_TASK_CORE_SEL         ARM_TASK_CORE0

#define CCF_CB_TASK_STACK_SIZE        (0x10000U)
#define CCF_CB_TASK_PRIORITY          (77U)
#define CCF_CB_TASK_CORE_SEL          ARM_TASK_CORE0

typedef INT32 (*CCF_OUT_CALLBACK_f)(UINT32 Slot, void *pEvnetData);  // The struct of pEvnetData is CCF_OUTPUT_s

typedef struct {
  UINT32 Slot;
  UINT32 Mode;
  UINT32 BatchNum;
  char   BinPath[MAX_CCF_PATH_LEN];
  UINT32 OutBufDepth;
} CCF_CREATE_CFG_s;

typedef struct {
  UINT32 Num;
  UINT32 BatchNum;
  UINT32 Size[MAX_CCF_INPUT_IO_NUM];
  UINT32 Addr[MAX_CCF_INPUT_IO_NUM];
  UINT32 Pitch[MAX_CCF_INPUT_IO_NUM];
} CCF_RAW_PAYLOAD_s;

typedef struct {
  UINT32 Addr;
  UINT32 PyramidScale;
  UINT32 Pitch;
  UINT32 OffX;
  UINT32 OffY;
  UINT32 DataW;
  UINT32 DataH;
  UINT32 RoiWidth;
  UINT32 RoiHeight;
} CCF_PICINFO_s;

typedef struct {
  UINT32 Num;
  UINT32 BatchNum;
  CCF_PICINFO_s PicInfo[MAX_CCF_INPUT_IO_NUM];
} CCF_PICINFO_PAYLOAD_s;

typedef struct {
  CCF_RAW_PAYLOAD_s     *pRaw;      // For CCF_MODE_RAW
  CCF_PICINFO_PAYLOAD_s *pPicInfo;  // For CCF_MODE_PIC_INFO
  char                  *OutFolderName;
  UINT32                Flag;
} CCF_PAYLOAD_s;

typedef struct {
  UINT32             NumOfIo;
  flexidag_memblk_t  Io[8];
  char               FolderName[MAX_CCF_PATH_LEN];
  UINT32             Flag;
  UINT32             ProcessingTime;
} CCF_OUTPUT_s;

typedef struct {
  UINT32             UUID;
  amba_roi_config_t  Config;
} CCF_ROI_CFG_s;

/*---------------------------------------------------------------------------*\
 * CvCommFlexi APIs
\*---------------------------------------------------------------------------*/
UINT32 CvCommFlexi_Create(CCF_CREATE_CFG_s CreateCfg);
UINT32 CvCommFlexi_Delete(UINT32 Slot);
UINT32 CvCommFlexi_FeedPayload(UINT32 Slot, CCF_PAYLOAD_s Payload);
UINT32 CvCommFlexi_RegisterOutCallback(UINT32 Slot, CCF_OUT_CALLBACK_f OutCallback);
UINT32 CvCommFlexi_UnregisterOutCallback(UINT32 Slot, CCF_OUT_CALLBACK_f OutCallback);
UINT32 CvCommFlexi_ConfigRoi(UINT32 Slot, CCF_ROI_CFG_s RoiConfig);
UINT32 CvCommFlexi_DumpLog(UINT32 Slot, const char *pPath);

#endif  /* CVCOMM_FLEXI_H */
