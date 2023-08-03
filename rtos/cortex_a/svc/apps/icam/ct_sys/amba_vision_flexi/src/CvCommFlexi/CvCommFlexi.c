/**
*  @file CvCommFlexi.c
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
*   @details CV communication (The communication between cvtask and app) for flexidag
*
*/

#include "CvCommFlexi.h"

#define ARM_LOG_CCF           "CCF"

/* State of Feeding and run task */
#define STATE_IDLE            (0)   // Ready to feed data
#define STATE_DATA_READY      (1)   // Ready to call AmbaCV
#define STATE_DATA_ACK        (2)   // AmbaCV done

#define FLAG(x)               (1U << x)
#define ALIGN4K(x)            (((x) + 4095U) & 0xFFFFF000U)

/*---------------------------------------------------------------------------*\
 * Message Queue
\*---------------------------------------------------------------------------*/
typedef struct {
  UINT32    BufIdx;
  char      FolderName[MAX_CCF_PATH_LEN];
  UINT32    Flag;
  UINT32    ProcessingTime;
} CCF_MQ_ITEM_s;

/*---------------------------------------------------------------------------*\
 * Buffer
\*---------------------------------------------------------------------------*/
typedef struct {
  UINT32 Buf[AVB_OFFSET_NUM];
} AVB_MEM_BUF_CTRL_s;

typedef struct {
  flexidag_memblk_t   Buf[MAX_CCF_INPUT_IO_NUM];
  flexidag_memblk_t   AvbBuf[MAX_CCF_INPUT_IO_NUM];  // Advance batch mode
  char                FolderName[MAX_CCF_PATH_LEN];
  UINT32              Flag;
} CCF_FLEXI_IN_BUF_CTRL_s;

typedef struct {
  UINT32              Wp;               // Write index
  UINT32              AvblNum;          // Available buffer number
  UINT32              MaxAvblNum;       // The maximum available buffer number
  UINT32              OutputNum;
  flexidag_memblk_t   Buf[MAX_CCF_OUTPUT_BUF_DEPTH][MAX_CCF_OUTPUT_IO_NUM];
  ArmMutex_t          Mutex;            // Protect AvblNum
} CCF_FLEXI_OUT_BUF_CTRL_s;

typedef struct {
  UINT32                   PoolId;      // Mem Pool Id
  flexidag_memblk_t        Bin;
  flexidag_memblk_t        Total;       // Mem Pool: State + Temp + In + Out
  flexidag_memblk_t        State;
  flexidag_memblk_t        Temp;
  CCF_FLEXI_IN_BUF_CTRL_s  In;
  CCF_FLEXI_OUT_BUF_CTRL_s Out;
} CCF_FLEXI_BUF_s;

/*---------------------------------------------------------------------------*\
 * CvCommFlexi
\*---------------------------------------------------------------------------*/
typedef struct {
  /* Config */
  UINT32                    Mode;
  UINT32                    InputIoNum;
  UINT32                    LogCnt;
  UINT32                    BatchNum;

  /* Run Task */
  ArmTask_t                 RunTsk;
  ArmEventFlag_t            EventFlag;

  /* Callback Task */
  ArmMsgQueue_t             CBMsgQueue;
  ArmTask_t                 CBTsk;
  CCF_OUT_CALLBACK_f        OutCallback[MAX_CCF_OUT_CB];
  CCF_OUTPUT_s              OutData;

  /* Flexidag Handle */
  AMBA_CV_FLEXIDAG_HANDLE_s Handle;

  /* Flexidag Buffers */
  CCF_FLEXI_BUF_s           FlexiBuf;
} CCF_CTRL_s;

/* CvCommFlexi Controller */
static CCF_CTRL_s g_CCFCtrl[MAX_CCF_SLOT] = {0};

/*---------------------------------------------------------------------------*\
 * Static functions
\*---------------------------------------------------------------------------*/
static ArmTaskRet_t CvCommFlexiRunEntry(ArmTaskEntryArg_t Slot);
static ArmTaskRet_t CvCommFlexiCBEntry(ArmTaskEntryArg_t Slot);

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: MAX
 *
 *  @Description:: Return maximum value between input a and b
 *
 *  @Input      ::
 *     a:          The input a
 *     b:          The input b
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    UINT32:      The maximum value between a and b
\*---------------------------------------------------------------------------*/
static inline UINT32 MAX(UINT32 a, UINT32 b)
{
  UINT32 tmp;

  if (a > b) {
      tmp = a;
  } else {
      tmp = b;
  }

  return tmp;
}

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: GetAvailableOutputBufIdx
 *
 *  @Description:: Get available output buffer index
 *
 *  @Input      ::
 *     Slot:       The slot id
 *
 *  @Output     ::
 *     pIndex:     The output buffer index
 *
 *  @Return     ::
 *     UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*---------------------------------------------------------------------------*/
static UINT32 GetAvailableOutputBufIdx(UINT32 Slot, UINT32 *pIndex)
{
  UINT32 Rval = ARM_OK;
  CCF_FLEXI_OUT_BUF_CTRL_s *pOut = &g_CCFCtrl[Slot].FlexiBuf.Out;
  UINT32 WaitTime = 10; // 10ms

  /* 1. Sanity check */
  if (pIndex == NULL) {
    ArmLog_ERR(ARM_LOG_CCF, "## pIndex is null", 0U, 0U);
    Rval = ARM_NG;
  }

  /* 2. Get available OutputBuf */
  do {
    (void) ArmMutex_Take(&pOut->Mutex);

    if (pOut->AvblNum > 0) {
      pOut->AvblNum--;
      *pIndex = pOut->Wp;
      pOut->Wp ++;
      pOut->Wp %= pOut->MaxAvblNum;

      (void) ArmMutex_Give(&pOut->Mutex);
      break;
    } else {
      (void) ArmMutex_Give(&pOut->Mutex);
      // ArmLog_WARN(ARM_LOG_CCF, "Warning!! Cannot get available out buffer, wait %ums", WaitTime, 0U);
      (void) ArmTask_Sleep(WaitTime);
    }
  } while (Rval == ARM_OK);

  return Rval;
}

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: FreeOutputBufIdx
 *
 *  @Description:: Free output buffer index
 *
 *  @Input      ::
 *     Slot:       The slot id
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*---------------------------------------------------------------------------*/
static UINT32 FreeOutputBufIdx(UINT32 Slot)
{
  UINT32 Rval = ARM_OK;
  CCF_FLEXI_OUT_BUF_CTRL_s *pOut = &g_CCFCtrl[Slot].FlexiBuf.Out;

  /* 1. Free OutputBuf index */
  if (Rval == ARM_OK) {
    (void) ArmMutex_Take(&pOut->Mutex);

    if (pOut->AvblNum >= pOut->MaxAvblNum) {
      ArmLog_DBG(ARM_LOG_CCF, "## can't free buffer due to incorrect AvblNum %u", pOut->AvblNum, 0U);
      Rval = ARM_NG;
    } else {
      pOut->AvblNum++;
    }

    (void) ArmMutex_Give(&pOut->Mutex);
  }

  return Rval;
}

/*---------------------------------------------------------------------------*\
 * Flexidag related functions
\*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: FlexidagSchdrStart
 *
 *  @Description:: FlexiDAG scheduler start
 *
 *  @Input      :: None
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*---------------------------------------------------------------------------*/
static UINT32 FlexidagSchdrStart(void)
{
  static UINT32 SchdrFlag = 0U;
  UINT32 Rval = ARM_OK;

  if(SchdrFlag == 0U) {
#ifndef CONFIG_THREADX
    /* Unix like */
    AMBA_CV_FLEXIDAG_SCHDR_CFG_s cfg = {0};
    cfg.cpu_map           = 0xD;
    cfg.log_level         = LVL_DEBUG;
    Rval = AmbaCV_FlexidagSchdrStart(&cfg);
#else
    /* Rtos */
    AMBA_CV_STANDALONE_SCHDR_CFG_s cfg = {0};
    cfg.flexidag_slot_num = MAX_CCF_SLOT;
    cfg.cavalry_slot_num  = 0;
    cfg.cpu_map           = 0xE;
    cfg.log_level         = LVL_DEBUG;
#ifdef CONFIG_CV_EXTERNAL_MSG_THREAD
    cfg.flag              = (INT32) (SCHDR_FASTBOOT | SCHDR_NO_INTERNAL_THREAD);
#else
    cfg.flag              = (INT32) SCHDR_FASTBOOT;
#endif
    cfg.flag              = 0;
    cfg.boot_cfg          = 0;
    Rval = AmbaCV_StandAloneSchdrStart(&cfg);
#endif
    if (Rval == 0U) {
      SchdrFlag = 1U;
      Rval = ARM_OK;
    } else {
      ArmLog_ERR(ARM_LOG_CCF, "## FlexidagSchdrStart fail (Rval 0x%x)", Rval, 0U);
      Rval = ARM_NG;
    }
  }

  return Rval;
}


/*---------------------------------------------------------------------------*\
 *  @RoutineName:: OpenAndInitFlexidag
 *
 *  @Description:: Open and init FlexiDAG with memory allocation
 *                 (AmbaCV_FlexidagOpen() -> AmbaCV_FlexidagInit())
 *
 *  @Input      ::
 *    pCcfCfg:     Pointer to CCF_CREATE_CFG_s struct
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*---------------------------------------------------------------------------*/
static UINT32 OpenAndInitFlexidag(CCF_CREATE_CFG_s* pCcfCfg)
{
  UINT32 Rval = ARM_OK;
  CCF_FLEXI_BUF_s *pBuf = &g_CCFCtrl[pCcfCfg->Slot].FlexiBuf;
  AMBA_CV_FLEXIDAG_HANDLE_s *pHandle = &g_CCFCtrl[pCcfCfg->Slot].Handle;
  INT32 BinSize = 0;
  AMBA_CV_FLEXIDAG_LOG_MSG_s set;

  /* Update the batch number to g_CCFCtrl */
  g_CCFCtrl[pCcfCfg->Slot].BatchNum = pCcfCfg->BatchNum;

  /* 0. Increace orc log buffer */
  set.flexidag_msg_entry = 8U;
  set.cvtask_msg_entry = 4U;
  set.arm_cpu_map = 0xFU;
  set.arm_log_entry = 0U;
  set.orc_log_entry = 2048U;

  AmbaCV_FlexidagSetParamSet(pHandle, FLEXIDAG_PARAMSET_LOG_MSG, (void *)&set, sizeof(AMBA_CV_FLEXIDAG_LOG_MSG_s));

  /* 1. Allocate memory for flexibin */
  if (Rval == ARM_OK) {
    (void) AmbaCV_UtilityFileSize(pCcfCfg->BinPath, &BinSize);
    Rval = DaemonIO_AllocateCma((UINT32) BinSize, &pBuf->Bin);

    if (ARM_NG == Rval) {
      UINT32 UsedSize = 0, FreeSize = 0;
      DaemonIO_GetCmaInfo(&UsedSize, &FreeSize);
      ArmLog_ERR(ARM_LOG_CCF, "## Request size %uB(%uMB) for flexibin", BinSize, BinSize/(1024*1024));
    }
  }

  /* 2. Load flexibin */
  if (Rval == ARM_OK) {
    (void) AmbaCV_UtilityFileLoad(pCcfCfg->BinPath, &pBuf->Bin);
  }

  /* 3. Open flexidag (call AmbaCV_FlexidagOpen())  */
  if (Rval == ARM_OK) {
    Rval = AmbaCV_FlexidagOpen(&pBuf->Bin, pHandle);

    if(Rval != 0U) {
      ArmLog_ERR(ARM_LOG_CCF, "## AmbaCV_FlexidagOpen fail (Slot %u, Rval 0x%x)", pCcfCfg->Slot, Rval);
      Rval = ARM_NG;
    } else {
      Rval = ARM_OK;
    }
  }

  /* 4. Allocate memory and create memory pool */
  {
    UINT32 TotalSize = 0U, Temp;

    /* State */
    TotalSize += ALIGN4K(pHandle->mem_req.flexidag_state_buffer_size);

    /* Temp */
    TotalSize += ALIGN4K(pHandle->mem_req.flexidag_temp_buffer_size);

    /* In */
    Temp = ALIGN4K(MAX(sizeof(memio_source_recv_picinfo_t), sizeof(memio_source_recv_raw_t)));

    if (pCcfCfg->BatchNum > 1) {
      Temp += ALIGN4K(sizeof(AVB_MEM_BUF_CTRL_s));
    }

    Temp *= MAX_CCF_INPUT_IO_NUM;
    TotalSize += ALIGN4K(Temp);

    /* Out */
    Temp = 0U;

    for (UINT32 i = 0U; i < pCcfCfg->OutBufDepth; i++) {
      for (UINT32 j = 0U; j < pHandle->mem_req.flexidag_num_outputs; j++) {
        Temp += ALIGN4K(pHandle->mem_req.flexidag_output_buffer_size[j]);
      }
    }

    TotalSize += ALIGN4K(Temp);
    Rval = DaemonIO_AllocateCma(TotalSize, &pBuf->Total);

    if (Rval == ARM_NG) {
      UINT32 UsedSize = 0, FreeSize = 0;
      DaemonIO_GetCmaInfo(&UsedSize, &FreeSize);
      ArmLog_ERR(ARM_LOG_CCF, "## Request size %uB(%uMB) for flexidag", TotalSize, TotalSize/(1024*1024));
    }

    if (Rval == ARM_OK) {
      Rval = ArmMemPool_Create(&pBuf->Total, &pBuf->PoolId);
    }
  }

  /* 5. Allocate memory for State, Temp, In and Out from memory pool */
  if (Rval == ARM_OK) {     // State
    if (pHandle->mem_req.flexidag_state_buffer_size != 0U) {
      Rval = ArmMemPool_Allocate(pBuf->PoolId, pHandle->mem_req.flexidag_state_buffer_size, &pBuf->State);
    } else {
      (void) ArmStdC_memset(&pBuf->State, 0, sizeof(flexidag_memblk_t));
    }
  }

  if (Rval == ARM_OK) {     // Temp
    if (pHandle->mem_req.flexidag_temp_buffer_size != 0U) {
      Rval = ArmMemPool_Allocate(pBuf->PoolId, pHandle->mem_req.flexidag_temp_buffer_size, &pBuf->Temp);
    } else {
      (void) ArmStdC_memset(&pBuf->Temp, 0, sizeof(flexidag_memblk_t));
    }
  }

  if (Rval == ARM_OK) {     // Input
    UINT32 InBufSize = MAX(sizeof(memio_source_recv_picinfo_t), sizeof(memio_source_recv_raw_t));

    for (UINT32 i = 0U; i < MAX_CCF_INPUT_IO_NUM; i++) {
      if (Rval == ARM_OK) {
        Rval = ArmMemPool_Allocate(pBuf->PoolId, InBufSize, &pBuf->In.Buf[i]);
      }

      if (pCcfCfg->BatchNum > 1) {
        if (Rval == ARM_OK) {
          Rval = ArmMemPool_Allocate(pBuf->PoolId, sizeof(AVB_MEM_BUF_CTRL_s), &pBuf->In.AvbBuf[i]);
        }
      }
    }
  }

  if (Rval == ARM_OK) {     // Out
    pBuf->Out.OutputNum  = pHandle->mem_req.flexidag_num_outputs;
    pBuf->Out.Wp         = 0U;
    pBuf->Out.AvblNum    = pCcfCfg->OutBufDepth;
    pBuf->Out.MaxAvblNum = pCcfCfg->OutBufDepth;

    for (UINT32 i = 0U; i < pBuf->Out.MaxAvblNum; i++) {
      for (UINT32 j = 0U; j < pBuf->Out.OutputNum; j++) {
        if (Rval == ARM_OK) {
          Rval = Rval = ArmMemPool_Allocate(pBuf->PoolId, pHandle->mem_req.flexidag_output_buffer_size[j], &pBuf->Out.Buf[i][j]);
        }
      }
    }
  }

  /* 6. Initialize flexidag (call AmbaCV_FlexidagInit()) */
  if (Rval == ARM_OK) {
    AMBA_CV_FLEXIDAG_INIT_s init = {0};

    init.state_buf = pBuf->State;
    init.temp_buf  = pBuf->Temp;
    Rval = AmbaCV_FlexidagInit(pHandle, &init);
    if(Rval != 0U) {
      ArmLog_ERR(ARM_LOG_CCF, "## AmbaCV_FlexidagInit fail (Slot %u, Rval 0x%x)", pCcfCfg->Slot, Rval);
      Rval = ARM_NG;
    } else {
      Rval = ARM_OK;
    }
  }

  return Rval;
}

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: CloseFlexidag
 *
 *  @Description:: Close FlexiDAG and release memory allocation
 *                 (AmbaCV_FlexidagClose())
 *
 *  @Input      ::
 *    Slot:        The slot id
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*---------------------------------------------------------------------------*/
static UINT32 CloseFlexidag(UINT32 Slot)
{
  UINT32 Rval = ARM_OK;
  CCF_CTRL_s *pCtrl = &g_CCFCtrl[Slot];
  CCF_FLEXI_BUF_s *pBuf = &g_CCFCtrl[Slot].FlexiBuf;
  AMBA_CV_FLEXIDAG_HANDLE_s *pHandle = &g_CCFCtrl[Slot].Handle;

  /* 1. Close flexidag */
  if (Rval == ARM_OK) {
    Rval = AmbaCV_FlexidagClose(pHandle);
    if(Rval != 0U) {
      ArmLog_ERR(ARM_LOG_CCF, "## AmbaCV_FlexidagClose fail (Slot %u, Rval 0x%x)", Slot, Rval);
      Rval = ARM_NG;
    } else {
      Rval = ARM_OK;
    }
  }

  /* 2. Free memory for Bin, State, Temp, In and Out */
  if (Rval == ARM_OK) {     // Bin
      Rval = DaemonIO_FreeCma(&pBuf->Bin);
  }

  if (Rval == ARM_OK) {     // State
    if (pBuf->State.buffer_size != 0U) {
      Rval = ArmMemPool_Free(pBuf->PoolId, &pBuf->State);
    }
  }

  if (Rval == ARM_OK) {     // Temp
    if (pBuf->Temp.buffer_size != 0U) {
      Rval = ArmMemPool_Free(pBuf->PoolId, &pBuf->Temp);
    }
  }

  if (Rval == ARM_OK) {     // Input
    for (UINT32 i = 0U; i < MAX_CCF_INPUT_IO_NUM; i++) {
      if (Rval == ARM_OK) {
        Rval = ArmMemPool_Free(pBuf->PoolId, &pBuf->In.Buf[i]);
      }

      if (pCtrl->BatchNum > 1) {
        if (Rval == ARM_OK) {
          Rval = ArmMemPool_Free(pBuf->PoolId, &pBuf->In.AvbBuf[i]);
        }
      }
    }
  }

  if (Rval == ARM_OK) {     // Out
    for (UINT32 i = 0U; i < pBuf->Out.AvblNum; i++) {
      for (UINT32 j = 0U; j < pBuf->Out.OutputNum; j++) {
        if (Rval == ARM_OK) {
          Rval = ArmMemPool_Free(pBuf->PoolId, &pBuf->Out.Buf[i][j]);
        }
      }
    }
  }

  if (Rval == ARM_OK) {
    Rval = ArmMemPool_Delete(pBuf->PoolId);
  }

  if (Rval == ARM_OK) {
    Rval = DaemonIO_FreeCma(&pBuf->Total);
  }

  if (Rval != ARM_OK) {
    ArmLog_ERR(ARM_LOG_CCF, "## CloseFlexidag() fail");
  }

  return Rval;
}

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: RunFlexidag
 *
 *  @Description:: Run flexidag one time
 *                 (AmbaCV_FlexidagRun())
 *
 *  @Input      ::
 *    Slot:        The slot id
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*---------------------------------------------------------------------------*/
static UINT32 RunFlexidag(UINT32 Slot)
{
  UINT32 Rval = ARM_OK, OutBufIdx = 0U;
  CCF_CTRL_s                  *pCtrl   = &g_CCFCtrl[Slot];
  CCF_FLEXI_BUF_s             *pBuf    = &g_CCFCtrl[Slot].FlexiBuf;
  AMBA_CV_FLEXIDAG_HANDLE_s   *pHandle = &g_CCFCtrl[Slot].Handle;
  AMBA_CV_FLEXIDAG_IO_s       in_buf   = {0};
  AMBA_CV_FLEXIDAG_IO_s       out_buf  = {0};
  AMBA_CV_FLEXIDAG_RUN_INFO_s run_info = {0};
  CCF_MQ_ITEM_s               MsgQueueItem = {0};

  /* 1. Prepare IN buffer (AMBA_CV_FLEXIDAG_IO_s) */
  if (Rval == ARM_OK) {
    if (pCtrl->BatchNum > 1) {  // Advance batch mode
      UINT32 offset_idx = 0;
      UINT32 offset_cnt = 0;
      UINT32* offset_ptr = NULL;
      UINT32 BaseAddr[MAX_CCF_INPUT_IO_NUM] = {0};

      in_buf.num_of_buf = pCtrl->InputIoNum / pCtrl->BatchNum * 2;  // Input ports will be double in advance batch mode

      for (UINT32 i = 0U; i < pCtrl->InputIoNum; i++) {
        /* Fill directly if i < in_buf.num_of_buf */
        if (i < (in_buf.num_of_buf / 2)) {
          BaseAddr[i] = ((memio_source_recv_raw_t*) (pBuf->In.Buf[i].pBuffer))->addr;
          ((memio_source_recv_raw_t*) (pBuf->In.Buf[i].pBuffer))->addr = 0U;  // Set the base address to be zero
          in_buf.buf[i] = pBuf->In.Buf[i];

          /* Cache clean */
          if (in_buf.buf[i].buffer_cacheable == 1) {
            (void) ArmMem_CacheClean(&in_buf.buf[i]);
          }
        }

        /* Update address offsets */
        offset_idx = i % (in_buf.num_of_buf/2);
        offset_cnt = i / (in_buf.num_of_buf/2);
        offset_ptr = (UINT32*) pBuf->In.AvbBuf[offset_idx].pBuffer;
        *(offset_ptr + offset_cnt) = (offset_cnt == 0) ? BaseAddr[i] : ((memio_source_recv_raw_t*) (pBuf->In.Buf[i].pBuffer))->addr;

        /* Fill directly if i >= (pCtrl->InputIoNum - in_buf.num_of_buf/2) */
        if (i >= (pCtrl->InputIoNum - in_buf.num_of_buf/2)) {
          ((memio_source_recv_raw_t*) (pBuf->In.Buf[i].pBuffer))->addr = pBuf->In.AvbBuf[offset_idx].buffer_daddr;
          in_buf.buf[in_buf.num_of_buf/2 + offset_idx] = pBuf->In.Buf[i];

          /* Cache clean */
          if (in_buf.buf[in_buf.num_of_buf/2 + offset_idx].buffer_cacheable == 1) {
            (void) ArmMem_CacheClean(&in_buf.buf[in_buf.num_of_buf/2 + offset_idx]);
          }

          if (pBuf->In.AvbBuf[offset_idx].buffer_cacheable == 1) {
            (void) ArmMem_CacheClean(&pBuf->In.AvbBuf[offset_idx]);
          }
        }
      }
    } else {
      in_buf.num_of_buf = pCtrl->InputIoNum;

      for (UINT32 i = 0U; i < in_buf.num_of_buf; i++) {
        in_buf.buf[i] = pBuf->In.Buf[i];

        /* Cache clean */
        if (in_buf.buf[i].buffer_cacheable == 1) {
          (void) ArmMem_CacheClean(&in_buf.buf[i]);
        }
      }
    }
  }

  /* 2. Get avaliable output buffer index */
  if (Rval == ARM_OK) {
    Rval = GetAvailableOutputBufIdx(Slot, &OutBufIdx);
  }

  /* 3. Prepare OUT buffer (AMBA_CV_FLEXIDAG_IO_s) */
  if (Rval == ARM_OK) {
    out_buf.num_of_buf = pHandle->mem_req.flexidag_num_outputs;

    for (UINT32 i = 0U; i < out_buf.num_of_buf; i++) {
      out_buf.buf[i] = pBuf->Out.Buf[OutBufIdx][i];

      // If flexidag output includes some padding bytes, it will be random number. memset as 0.
      (void) ArmStdC_memset((void *)out_buf.buf[i].pBuffer, 0, out_buf.buf[i].buffer_size);

      /* Cache clean */
      if (out_buf.buf[i].buffer_cacheable == 1) {
        (void) ArmMem_CacheClean(&out_buf.buf[i]);
      }
    }
  }

  /* 4. Call AmbaCV_FlexidagRun() */
  if (Rval == ARM_OK) {
    Rval = AmbaCV_FlexidagRun(pHandle, &in_buf, &out_buf, &run_info);
    if (Rval != 0U) {
      ArmLog_ERR(ARM_LOG_CCF, "## AmbaCV_FlexidagRun fail (Slot %u, Rval 0x%x)", Slot, Rval);
      Rval = ARM_NG;
    }
  }

  /* Cache invalid */
  if (Rval == ARM_OK) {
    for (UINT32 i = 0U; i < out_buf.num_of_buf; i++) {
      if (out_buf.buf[i].buffer_cacheable == 1) {
        (void) ArmMem_Invalid(&out_buf.buf[i]);
      }
    }
  }

  /* 5. Send msg to callback task */
  if (Rval == ARM_OK) {
    MsgQueueItem.BufIdx = OutBufIdx;
    MsgQueueItem.Flag   = pBuf->In.Flag;
    if (run_info.start_time > run_info.end_time) {
      MsgQueueItem.ProcessingTime = 0;
    } else {
      MsgQueueItem.ProcessingTime = run_info.end_time - run_info.start_time;
    }
    if (0 != pBuf->In.FolderName[0]) {
      strcpy(MsgQueueItem.FolderName, pBuf->In.FolderName);
    }
    Rval = ArmMsgQueue_Send(&pCtrl->CBMsgQueue, (void *)&MsgQueueItem);
  }

  /* 6. Print performance log */
  if (Rval == ARM_OK) {
    if (pCtrl->LogCnt != 0U) {
      UINT32 diff;
      ArmLog_DBG(ARM_LOG_CCF, "", 0U, 0U);
      ArmLog_DBG(ARM_LOG_CCF, "--------------------------------------------", 0U, 0U);
      ArmLog_DBG(ARM_LOG_CCF, "[FlexiDag%u]RetCode                %u", Slot, run_info.overall_retcode);
      diff = run_info.api_end_time - run_info.api_start_time;
      ArmLog_DBG(ARM_LOG_CCF, "[FlexiDag%u]Processing time(+API)  %uus", Slot, diff);
      diff = run_info.end_time - run_info.start_time;
      ArmLog_DBG(ARM_LOG_CCF, "[FlexiDag%u]Processing time        %uus", Slot, diff);
      ArmLog_DBG(ARM_LOG_CCF, "--------------------------------------------", 0U, 0U);

      pCtrl->LogCnt --;
    }
  }

  return Rval;
}

/*---------------------------------------------------------------------------*\
 * CvCommFlexi resource functions
\*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: CreateCtrlResource
 *
 *  @Description:: Create CvComm controller resource
 *                 (mutex -> eventflag -> MsgQ -> run/cb tasks)
 *
 *  @Input      ::
 *     Slot:       The slot id
 *     Mode:       The flexidag mode : RAW(0)/PICINFO(1)
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*---------------------------------------------------------------------------*/
static UINT32 CreateCtrlResource(UINT32 Slot, UINT32 Mode)
{
  UINT32 Rval = ARM_OK;
  static UINT32  SlotId[MAX_CCF_SLOT] = {0};
  CCF_CTRL_s *pCtrl = &g_CCFCtrl[Slot];

  SlotId[Slot] = Slot;

  /* 1. Init CCF_CTRL_s variable */
  pCtrl->Mode       = Mode;
  pCtrl->InputIoNum = 0U;
  pCtrl->LogCnt     = DEFAULT_CCF_LOG_CNT;
  for (UINT32 Idx = 0U; Idx < MAX_CCF_OUT_CB; Idx++) {
    pCtrl->OutCallback[Idx] = NULL;
  }
  (void) ArmStdC_memset(&pCtrl->OutData, 0, sizeof(CCF_OUTPUT_s));

  /* 2. Create mutex */
  if (Rval == ARM_OK) {
    char MutexName[16] = "CCFBufMut_XX";

    MutexName[10] = '0' + Slot/10;
    MutexName[11] = '0' + Slot%10;
    Rval = ArmMutex_Create(&pCtrl->FlexiBuf.Out.Mutex, MutexName);
  }

  /* 3. Create eventflag */
  if (Rval == ARM_OK) {
    char EventFlagName[16] = "CCFStateFlag_XX";

    EventFlagName[13] = '0' + Slot/10;
    EventFlagName[14] = '0' + Slot%10;
    Rval = ArmEventFlag_Create(&pCtrl->EventFlag, EventFlagName);
  }

  /* 4. Create message queue */
  if (Rval == ARM_OK) {
    static CCF_MQ_ITEM_s g_MsgQueueBuf[MAX_CCF_SLOT][MAX_CCF_MSG_QUEUE_NUM];
    char MsgQName[16] = "CCFMsgQ_XX";

    MsgQName[8] = '0' + Slot/10;
    MsgQName[9] = '0' + Slot%10;
    Rval = ArmMsgQueue_Create(&pCtrl->CBMsgQueue, MsgQName, sizeof(CCF_MQ_ITEM_s), MAX_CCF_MSG_QUEUE_NUM, (void *)&g_MsgQueueBuf[Slot][0]);
  }

  /* 5. Create run and callback task */
  if (Rval == ARM_OK) {
    static UINT8   g_RunTskStackBuf[MAX_CCF_SLOT][CCF_RUN_TASK_STACK_SIZE];
    char TskName[16] = "CCFRun_XX";
    ARM_TASK_CREATE_ARG_s  TskArg = {0};

    TskName[7] = '0' + Slot/10;
    TskName[8] = '0' + Slot%10;
    TskArg.TaskPriority  = CCF_RUN_TASK_PRIORITY;
    TskArg.EntryFunction = CvCommFlexiRunEntry;
    TskArg.EntryArg      = (ArmTaskEntryArg_t) &SlotId[Slot];
    TskArg.pStackBase    = &g_RunTskStackBuf[Slot][0];
    TskArg.StackByteSize = CCF_RUN_TASK_STACK_SIZE;
    TskArg.CoreSel       = CCF_RUN_TASK_CORE_SEL;

    Rval = ArmTask_Create(&pCtrl->RunTsk, TskName, &TskArg);
  }
  if (Rval == ARM_OK) {
    static UINT8   g_CBTskStackBuf[MAX_CCF_SLOT][CCF_CB_TASK_STACK_SIZE];
    char TskName[16] = "CCFCB_XX";
    ARM_TASK_CREATE_ARG_s  TskArg = {0};

    TskName[6] = '0' + Slot/10;
    TskName[7] = '0' + Slot%10;
    TskArg.TaskPriority  = CCF_CB_TASK_PRIORITY;
    TskArg.EntryFunction = CvCommFlexiCBEntry;
    TskArg.EntryArg      = (ArmTaskEntryArg_t) &SlotId[Slot];
    TskArg.pStackBase    = &g_CBTskStackBuf[Slot][0];
    TskArg.StackByteSize = CCF_CB_TASK_STACK_SIZE;
    TskArg.CoreSel       = CCF_CB_TASK_CORE_SEL;

    Rval = ArmTask_Create(&pCtrl->CBTsk, TskName, &TskArg);
  }

  if (Rval == ARM_OK) {
    ArmLog_DBG(ARM_LOG_CCF, "Create resource for Slot %u successfully", Slot, 0U);
  }

  return Rval;
}


/*---------------------------------------------------------------------------*\
 *  @RoutineName:: DeleteCtrlResource
 *
 *  @Description:: Delete CvComm controller resource
 *                 (run/cb tasks -> MsgQ -> eventflag -> mutex)
 *
 *  @Input      ::
 *     Slot:       The slot id
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*---------------------------------------------------------------------------*/
static UINT32 DeleteCtrlResource(UINT32 Slot)
{
  UINT32 Rval = ARM_OK;
  CCF_CTRL_s *pCtrl = &g_CCFCtrl[Slot];

  /* 1. Clear all callback */
  for (UINT32 Idx = 0U; Idx < MAX_CCF_OUT_CB; Idx++) {
    pCtrl->OutCallback[Idx] = NULL;
  }

  /* 2. Delete run and callback task */
  if (Rval == ARM_OK) {
    Rval = ArmTask_Delete(&pCtrl->RunTsk);
  }
  if (Rval == ARM_OK) {
    Rval = ArmTask_Delete(&pCtrl->CBTsk);
  }

  /* 3. Delete message queue */
  if (Rval == ARM_OK) {
    char MsgQName[16] = "CCFMsgQ_XX";

    MsgQName[8] = '0' + Slot/10;
    MsgQName[9] = '0' + Slot%10;
    Rval = ArmMsgQueue_Delete(&pCtrl->CBMsgQueue, MsgQName);
  }

  /* 4. Delete eventflag */
  if (Rval == ARM_OK) {
    Rval = ArmEventFlag_Delete(&pCtrl->EventFlag);
  }

  /* 5. Delete mutex */
  if (Rval == ARM_OK) {
    Rval = ArmMutex_Delete(&pCtrl->FlexiBuf.Out.Mutex);
  }

  return Rval;
}


/*---------------------------------------------------------------------------*\
 * CvCommFlexi task entry
\*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: CvCommFlexiRunEntry
 *
 *  @Description:: The task to run flexidag
 *
 *  @Input      ::
 *     EntryArg:   pointer to slot id (UINT32 *)
 *
 *  @Output     :: None
 *
 *  @Return     :: None
\*---------------------------------------------------------------------------*/
static ArmTaskRet_t CvCommFlexiRunEntry(ArmTaskEntryArg_t EntryArg)
{
  UINT32 Rval = ARM_OK;
  CCF_CTRL_s *pCtrl;
  UINT32 *pSlot = (UINT32 *)EntryArg;
  UINT32 Slot = *pSlot;

  ArmLog_DBG(ARM_LOG_CCF, "Create CvCommFlexi Run task (Slot = %u)", Slot, 0U);

  if (Slot < MAX_CCF_SLOT) {
    pCtrl = &g_CCFCtrl[Slot];

    while (1) {
      /* 1. Set state to IDLE */
      if (Rval == ARM_OK) {
        Rval = ArmEventFlag_Set(&pCtrl->EventFlag, FLAG(STATE_IDLE));
      }

      /* 2. Wait for DATA_READY state */
      if (Rval == ARM_OK) {
        UINT32 ActualFlags = 0U;
        Rval = ArmEventFlag_Wait(&pCtrl->EventFlag, FLAG(STATE_DATA_READY), ARM_EF_AND_CLEAR, &ActualFlags);
      }

      /* 3. Run FlexiDag */
      if (Rval == ARM_OK) {
        Rval = RunFlexidag(Slot);
      }

      /* 4. Set state to DATA_ACK */
      if (Rval == ARM_OK) {
        Rval = ArmEventFlag_Set(&pCtrl->EventFlag, FLAG(STATE_DATA_ACK));
      }
    }
  } else {
    ArmLog_ERR(ARM_LOG_CCF,"## CvCommFlexi task shutdown because of invalid Slot %u", Slot, 0U);
  }

  ArmLog_DBG(ARM_LOG_CCF, "CvCommFlexi Run task terminate (Slot = %u)", Slot, 0U);

  return ARM_TASK_NULL_RET;
}


/*---------------------------------------------------------------------------*\
 *  @RoutineName:: CvCommFlexiCBEntry
 *
 *  @Description:: The task to run flexidag output callback
 *
 *  @Input      ::
 *     EntryArg:   pointer to slot id (UINT32 *)
 *
 *  @Output     :: None
 *
 *  @Return     :: None
\*---------------------------------------------------------------------------*/
static ArmTaskRet_t CvCommFlexiCBEntry(ArmTaskEntryArg_t EntryArg)
{
  UINT32 Rval = ARM_OK;
  CCF_CTRL_s *pCtrl;
  UINT32 *pSlot = (UINT32 *)EntryArg;
  UINT32 Slot = *pSlot;
  CCF_MQ_ITEM_s MsgQueueItem = {0};
  CCF_OUTPUT_s *pOutData;
  AMBA_CV_FLEXIDAG_HANDLE_s *pHandle;
  CCF_FLEXI_BUF_s *pBuf;

  ArmLog_DBG(ARM_LOG_CCF, "Create CvCommFlexi CB task (Slot = %u)", Slot, 0U);

  if (Slot < MAX_CCF_SLOT) {
    pCtrl    = &g_CCFCtrl[Slot];
    pHandle  = &g_CCFCtrl[Slot].Handle;
    pBuf     = &g_CCFCtrl[Slot].FlexiBuf;
    pOutData = &pCtrl->OutData;

    while (1) {
      /* 1. Get output index from MsgQueue */
      Rval = ArmMsgQueue_Recv(&pCtrl->CBMsgQueue, &MsgQueueItem);

      /* 2. Fill CCF_OUTPUT_s */
      if (Rval == ARM_OK) {
        pOutData->NumOfIo = pHandle->mem_req.flexidag_num_outputs;

        for (UINT32 i = 0U; i < pOutData->NumOfIo; i++) {
          pOutData->Io[i] = pBuf->Out.Buf[MsgQueueItem.BufIdx][i];
          // flexidag memory block size is 4k alignment.(Linux: 4k, RTOS: 64 )
          // the real output size depends on dags.
          pOutData->Io[i].buffer_size = pHandle->mem_req.flexidag_output_buffer_size[i];
        }

        if (0 != MsgQueueItem.FolderName[0]) {
          strcpy(pOutData->FolderName, MsgQueueItem.FolderName);
        }
        pOutData->Flag = MsgQueueItem.Flag;
        pOutData->ProcessingTime = MsgQueueItem.ProcessingTime;
      }

      /* 3. Call callback function */
      if (Rval == ARM_OK) {
        for (UINT32 Idx = 0U; Idx < MAX_CCF_OUT_CB; Idx++) {
          const CCF_OUT_CALLBACK_f *pCCFOutHdlr = &pCtrl->OutCallback[Idx];
          if ((*pCCFOutHdlr) == NULL) {
            continue;
          } else {
            (void)(*pCCFOutHdlr)(Slot, (void *)pOutData);
          }
        }
      }

      /* 4. Release the buffer */
      if (Rval == ARM_OK) {
        Rval = FreeOutputBufIdx(Slot);
      }
    }
  } else {
    ArmLog_ERR(ARM_LOG_CCF,"## CvCommFlexiCB task shutdown because of invalid Slot %u", Slot, 0U);
  }

  ArmLog_DBG(ARM_LOG_CCF, "CvCommFlexi CB task terminate (Slot = %u)", Slot, 0U);

  return ARM_TASK_NULL_RET;
}


/*---------------------------------------------------------------------------*\
 * CvCommFlexi APIs
\*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*\
 *  @RoutineName:: CvCommFlexi_Create
 *
 *  @Description:: Create CvCommFlexi resource/task
 *
 *  @Input      ::
 *     CreateCfg:  CvComm flexidag create config
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*---------------------------------------------------------------------------*/
UINT32 CvCommFlexi_Create(CCF_CREATE_CFG_s CreateCfg)
{
  UINT32 Rval = ARM_OK;
  UINT32 OutBufDepth = 0U;

  /* 1. Sanity check for parameters */
  if (CreateCfg.Slot >= MAX_CCF_SLOT) {
    ArmLog_ERR(ARM_LOG_CCF, "## CvCommFlexi_Create fail (invalid Slot %u)", CreateCfg.Slot, 0U);
    Rval = ARM_NG;
  }
  if (CreateCfg.Mode >= MAX_CCF_MODE) {
    ArmLog_ERR(ARM_LOG_CCF, "## CvCommFlexi_Create fail (invalid Mode %u)", CreateCfg.Mode, 0U);
    Rval = ARM_NG;
  }
  if (ArmFIO_GetSize(CreateCfg.BinPath) <= 0) {
    ArmLog_STR(ARM_LOG_CCF, "## CvCommFlexi_Create fail (BIN %s does not exist)", CreateCfg.BinPath, NULL);
    Rval = ARM_NG;
  }

  /* 2. Calculate OutBufDepth */
  if (CreateCfg.OutBufDepth == 0U) {
    OutBufDepth = DEFAULT_CCF_OUTPUT_BUF_DEPTH;
  } else if (CreateCfg.OutBufDepth > MAX_CCF_OUTPUT_BUF_DEPTH) {
    OutBufDepth = MAX_CCF_OUTPUT_BUF_DEPTH;
  } else {
    OutBufDepth = CreateCfg.OutBufDepth;
  }

  /* 3. Trigger scheduler start */
  if (Rval == ARM_OK) {
    Rval = FlexidagSchdrStart();
  }

  /* 4. Open and init flexidag */
  if (Rval == ARM_OK) {
    Rval = OpenAndInitFlexidag(&CreateCfg);
  }

  /* 5. Create other resource (task, mutex, eventflag ...) */
  if (Rval == ARM_OK) {
    Rval = CreateCtrlResource(CreateCfg.Slot, CreateCfg.Mode);
  }

  return Rval;
}

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: CvCommFlexi_Delete
 *
 *  @Description:: Delete CvComm flexidag ctrl/buf resource
 *
 *  @Input      ::
 *     Slot:       The slot id
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*---------------------------------------------------------------------------*/
UINT32 CvCommFlexi_Delete(UINT32 Slot)
{
  UINT32 Rval = ARM_OK;

  /* 1. Sanity check for parameters */
  if (Slot >= MAX_CCF_SLOT) {
    ArmLog_ERR(ARM_LOG_CCF, "## CvCommFlexi_Delete fail (invalid Slot %u)", Slot, 0U);
    Rval = ARM_NG;
  }

  /* 2. Delete controller resource */
  if (Rval == ARM_OK) {
    DeleteCtrlResource(Slot);
  }

  /* 3. Delete Flexidag and buffer resource */
  if (Rval == ARM_OK) {
    CloseFlexidag(Slot);
  }

  /* 4. reset g_CCFCtrl */
  (void) ArmStdC_memset(&g_CCFCtrl[Slot], 0, sizeof(CCF_CTRL_s));

  return Rval;
}

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: CvCommFlexi_FeedPayload
 *
 *  @Description:: Feed payload by raw/pic_info format (blocking function)
 *                 The format MUST match to the CvCommFlexi mode
 *
 *  @Input      ::
 *     Slot:       The slot id
 *     Payload:    The given payload
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*---------------------------------------------------------------------------*/
UINT32 CvCommFlexi_FeedPayload(UINT32 Slot, const CCF_PAYLOAD_s Payload)
{
  UINT32 Rval = ARM_OK;
  CCF_CTRL_s *pCtrl;
  CCF_FLEXI_BUF_s *pBuf;

  /* 1. Sanity check for parameters */
  if (Slot >= MAX_CCF_SLOT) {
    ArmLog_ERR(ARM_LOG_CCF, "## CvCommFlexi_FeedPayload fail (invalid Slot %u)", Slot, 0U);
    Rval = ARM_NG;
  } else {  // check payload
    if (g_CCFCtrl[Slot].Mode == CCF_MODE_RAW && Payload.pRaw == NULL) {
      ArmLog_ERR(ARM_LOG_CCF, "## CvCommFlexi_FeedPayload fail (Mode is Raw but pRaw is Null)", 0U, 0U);
      Rval = ARM_NG;
    } else if (g_CCFCtrl[Slot].Mode == CCF_MODE_PIC_INFO && Payload.pPicInfo == NULL) {
      ArmLog_ERR(ARM_LOG_CCF, "## CvCommFlexi_FeedPayload fail (Mode is PICINFO but pPicInfo is Null)", 0U, 0U);
      Rval = ARM_NG;
    }
  }

  /* 2. Feed data */
  if (Rval == ARM_OK) {
    pCtrl = &g_CCFCtrl[Slot];
    pBuf = &g_CCFCtrl[Slot].FlexiBuf;

    /* 2-1. Wait for IDLE state */
    if (Rval == ARM_OK) {
      UINT32 ActualFlags = 0U;
      Rval = ArmEventFlag_Wait(&pCtrl->EventFlag, FLAG(STATE_IDLE), ARM_EF_AND_CLEAR, &ActualFlags);
    }

    /* 2-2. Copy data to CCF_FLEXI_IN_BUF_CTRL_s */
    if (Rval == ARM_OK) {
      if (pCtrl->Mode == CCF_MODE_RAW) {
        pCtrl->InputIoNum = Payload.pRaw->Num;
        pCtrl->BatchNum = Payload.pRaw->BatchNum;

        for (UINT32 i = 0U; i < Payload.pRaw->Num; i++) {
          memio_source_recv_raw_t rawTemp = {0};

          if (pCtrl->BatchNum > 1) {
            if (i < (Payload.pRaw->Num / pCtrl->BatchNum)) {
              rawTemp.batch_cnt = -1;
            } else {
              rawTemp.batch_cnt = pCtrl->BatchNum;
            }
          }

          rawTemp.addr  = Payload.pRaw->Addr[i];
          rawTemp.size  = Payload.pRaw->Size[i];
          rawTemp.pitch = Payload.pRaw->Pitch[i];
          // printf("[CCF] addr 0x%X size %u\n", rawTemp.addr, rawTemp.size);
          (void) ArmStdC_memcpy(pBuf->In.Buf[i].pBuffer, &rawTemp, sizeof(memio_source_recv_raw_t));
        }

        if (NULL != Payload.OutFolderName) {
          (void) ArmStdC_memset(pBuf->In.FolderName, 0, MAX_CCF_PATH_LEN);
          strcpy(pBuf->In.FolderName, Payload.OutFolderName);
          // printf("[CCF] folder name %s\n", pBuf->In.FolderName);
        }

        pBuf->In.Flag = Payload.Flag;
        // printf("[CCF]Payload.Flag %u\n", pBuf->In.Flag);

      } else if (pCtrl->Mode == CCF_MODE_PIC_INFO) {
        pCtrl->InputIoNum = Payload.pPicInfo->BatchNum;
        pCtrl->BatchNum = Payload.pPicInfo->BatchNum;
        CCF_PICINFO_s* pPicInfo = Payload.pPicInfo->PicInfo;

        for (UINT32 i = 0U; i < pCtrl->InputIoNum; i++) {
          UINT32 UpperIdx0 = 2*i;
          UINT32 UpperIdx1 = (2*i) + 1;

          /* Prepare for run */
          memio_source_recv_picinfo_t PicInfo;
          PicInfo.pic_info.rpLumaLeft[pPicInfo[UpperIdx0].PyramidScale] = pPicInfo[UpperIdx0].Addr;
          PicInfo.pic_info.rpLumaRight[pPicInfo[UpperIdx0].PyramidScale] = pPicInfo[UpperIdx0].Addr;
          PicInfo.pic_info.rpChromaLeft[pPicInfo[UpperIdx0].PyramidScale] = pPicInfo[UpperIdx1].Addr;
          PicInfo.pic_info.rpChromaRight[pPicInfo[UpperIdx0].PyramidScale] = pPicInfo[UpperIdx1].Addr;
          PicInfo.pic_info.pyramid.half_octave[pPicInfo[UpperIdx0].PyramidScale].ctrl.roi_pitch = (UINT16) pPicInfo[UpperIdx0].Pitch;
          PicInfo.pic_info.pyramid.half_octave[pPicInfo[UpperIdx0].PyramidScale].roi_width_m1 = (UINT16) pPicInfo[UpperIdx0].RoiWidth - 1U;
          PicInfo.pic_info.pyramid.half_octave[pPicInfo[UpperIdx0].PyramidScale].roi_height_m1 = (UINT16) pPicInfo[UpperIdx0].RoiHeight - 1U;
          (void) ArmStdC_memcpy(pBuf->In.Buf[i].pBuffer, &PicInfo, sizeof(memio_source_recv_picinfo_t));

          /* Calculate offset for picinfo */
          for (UINT32 j = 0U ; j < MAX_HALF_OCTAVES ; j++) {
            ((memio_source_recv_picinfo_t*) (pBuf->In.Buf[i].pBuffer))->pic_info.rpLumaLeft[j] -= pBuf->In.Buf[i].buffer_daddr;
            ((memio_source_recv_picinfo_t*) (pBuf->In.Buf[i].pBuffer))->pic_info.rpLumaRight[j] -= pBuf->In.Buf[i].buffer_daddr;
            ((memio_source_recv_picinfo_t*) (pBuf->In.Buf[i].pBuffer))->pic_info.rpChromaLeft[j] -= pBuf->In.Buf[i].buffer_daddr;
            ((memio_source_recv_picinfo_t*) (pBuf->In.Buf[i].pBuffer))->pic_info.rpChromaRight[j] -= pBuf->In.Buf[i].buffer_daddr;
          }
        }

        if (NULL != Payload.OutFolderName) {
          (void) ArmStdC_memset(pBuf->In.FolderName, 0, MAX_CCF_PATH_LEN);
          strcpy(pBuf->In.FolderName, Payload.OutFolderName);
          // printf("[CCF] folder name %s\n", pBuf->In.FolderName);
        }

        pBuf->In.Flag = Payload.Flag;
        // printf("[CCF]Payload.Flag %u\n", pBuf->In.Flag);
      }
    }

    /* 2-3. Set state to DATA_READY */
    if (Rval == ARM_OK) {
      Rval = ArmEventFlag_Set(&pCtrl->EventFlag, FLAG(STATE_DATA_READY));
    }

    /* 2-4. Wait for DATA_ACK state */
    if (Rval == ARM_OK) {
      UINT32 ActualFlags = 0U;
      Rval = ArmEventFlag_Wait(&pCtrl->EventFlag, FLAG(STATE_DATA_ACK), ARM_EF_AND_CLEAR, &ActualFlags);
    }
  }

  return Rval;
}

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: CvCommFlexi_RegisterOutCallback
 *
 *  @Description:: Register CvCommFlexi output callback
 *
 *  @Input      ::
 *    Slot:        The slot id
 *    OutCallback: Callback function
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*---------------------------------------------------------------------------*/
UINT32 CvCommFlexi_RegisterOutCallback(UINT32 Slot, CCF_OUT_CALLBACK_f OutCallback)
{
  UINT32 Rval = ARM_OK;
  CCF_CTRL_s *pCtrl;

  /* 1. Sanity check for parameters */
  if (Slot >= MAX_CCF_SLOT) {
    ArmLog_ERR(ARM_LOG_CCF, "## CvCommFlexi_RegisterOutCallback fail (invalid Slot %u)", Slot, 0U);
    Rval = ARM_NG;
  } else if (OutCallback == NULL) {
    ArmLog_ERR(ARM_LOG_CCF, "## CvCommFlexi_RegisterOutCallback fail (callback is null)", 0U, 0U);
    Rval = ARM_NG;
  }

  /* 2. Find an empty space to register */
  if (Rval == ARM_OK) {
    UINT32 Idx = 0U;
    pCtrl = &g_CCFCtrl[Slot];

    for (Idx = 0U; Idx < MAX_CCF_OUT_CB; Idx++) {
      if (pCtrl->OutCallback[Idx] == OutCallback) {
        ArmLog_DBG(ARM_LOG_CCF, "[Slot_%u] Already register callback to idx %u", Slot, Idx);
        break;
      } else if (pCtrl->OutCallback[Idx] == NULL) {
        ArmLog_DBG(ARM_LOG_CCF, "[Slot_%u] Register callback to idx %u", Slot, Idx);
        pCtrl->OutCallback[Idx] = OutCallback;
        break;
      }
    }

    if (MAX_CCF_OUT_CB == Idx) {
      ArmLog_ERR(ARM_LOG_CCF, "[Slot_%u] No more empty space to register callback", Slot, 0U);
      Rval = ARM_NG;
    }
  }

  return Rval;
}

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: CvCommFlexi_UnregisterOutCallback
 *
 *  @Description:: Unregister CvCommFlexi out callback
 *
 *  @Input      ::
 *    Slot:        The slot id
 *    OutCallback: Callback function
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*---------------------------------------------------------------------------*/
UINT32 CvCommFlexi_UnregisterOutCallback(UINT32 Slot, CCF_OUT_CALLBACK_f OutCallback)
{
  UINT32 Rval = ARM_OK;
  CCF_CTRL_s *pCtrl;

  /* 1. Sanity check for parameters */
  if (Slot >= MAX_CCF_SLOT) {
    ArmLog_ERR(ARM_LOG_CCF, "## CvCommFlexi_UnregisterOutCallback fail (invalid Slot %u)", Slot, 0U);
    Rval = ARM_NG;
  } else if (OutCallback == NULL) {
    ArmLog_ERR(ARM_LOG_CCF, "## CvCommFlexi_UnregisterOutCallback fail (callback is null)", 0U, 0U);
    Rval = ARM_NG;
  }

  /* 2. Find callback and remove it */
  if (Rval == ARM_OK) {
    pCtrl = &g_CCFCtrl[Slot];

    for (UINT32 Idx = 0U; Idx < MAX_CCF_OUT_CB; Idx++) {
      if (pCtrl->OutCallback[Idx] == OutCallback) {
        pCtrl->OutCallback[Idx] = NULL;
        ArmLog_DBG(ARM_LOG_CCF, "[Slot_%u] Remove callback idx %u", Slot, Idx);
        break;
      }
    }
  }

  return Rval;
}

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: CvCommFlexi_ConfigRoi
 *
 *  @Description:: Dynamically configure ROI settings
 *
 *  @Input      ::
 *    Slot:        The slot id
 *    RoiConfig:   ROI config
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*---------------------------------------------------------------------------*/
UINT32 CvCommFlexi_ConfigRoi(UINT32 Slot, CCF_ROI_CFG_s RoiConfig)
{
  UINT32 Rval = ARM_OK;
  UINT16 FlowId = 0;
  AMBA_CV_FLEXIDAG_HANDLE_s *pHandle;

  /* 1. Sanity check for parameters */
  if (Slot >= MAX_CCF_SLOT) {
    ArmLog_ERR(ARM_LOG_CCF, "## CvCommFlexi_ConfigRoi fail (invalid Slot %u)", Slot, 0U);
    Rval = ARM_NG;
  }

  /* 2. Get flow ID by UUID */
  if (Rval == ARM_OK) {
    pHandle = &g_CCFCtrl[Slot].Handle;

    Rval = AmbaCV_FlexidagGetFlowIdByUUID(pHandle, RoiConfig.UUID, &FlowId);
    if (Rval != 0U) {
      ArmLog_ERR(ARM_LOG_CCF,"## AmbaCV_FlexidagGetFlowIdByUUID fail (Rval = 0x%x)", Rval, 0U);
      Rval = ARM_NG;
    }
  }

  /* 3. Send message through Flow ID */
  if (Rval == ARM_OK) {
    AMBA_CV_FLEXIDAG_MSG_s Msg = {0};

    Msg.flow_id = FlowId;
    Msg.vpMessage = &RoiConfig.Config;
    Msg.length = sizeof(RoiConfig.Config);
    Rval = AmbaCV_FlexidagSendMsg(pHandle, &Msg);
    if (Rval != 0U) {
      ArmLog_ERR(ARM_LOG_CCF,"## AmbaCV_FlexidagSendMsg fail (Rval = 0x%x)", Rval, 0U);
      Rval = ARM_NG;
    }
  }

  return Rval;
}

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: CvCommFlexi_DumpLog
 *
 *  @Description:: Print logs to console
 *
 *  @Input      ::
 *    Slot:        The slot id
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*---------------------------------------------------------------------------*/
UINT32 CvCommFlexi_DumpLog(UINT32 Slot, const char *pPath)
{
  UINT32 Rval = ARM_OK;
  AMBA_CV_FLEXIDAG_HANDLE_s *pHandle;

  /* 1. Sanity check for parameters */
  if (Slot >= MAX_CCF_SLOT) {
    ArmLog_ERR(ARM_LOG_CCF, "## CvCommFlexi_ConfigRoi fail (invalid Slot %u)", Slot, 0U);
    Rval = ARM_NG;
  }

  /* 2. Dump log */
  if (Rval == ARM_OK) {
    pHandle = &g_CCFCtrl[Slot].Handle;

    if (NULL == pHandle->fd_handle) {
      ArmLog_ERR(ARM_LOG_CCF, "## CvCommFlexi_DumpLog fail, no FD handle on Slot%u", Slot, 0U);
      Rval = ARM_NG;
    } else {

      // CCFWrapper_ConPrintCtl(1U);

      if (ARM_OK == Rval) {
        if (NULL == pPath) {
          Rval = AmbaCV_FlexidagDumpLog(pHandle, NULL, FLEXILOG_CORE0 | FLEXILOG_VIS_CVTASK | FLEXILOG_ECHO_TO_CONSOLE);
        } else {
          Rval = AmbaCV_FlexidagDumpLog(pHandle, pPath, FLEXILOG_CORE0 | FLEXILOG_VIS_CVTASK);
        }
      }

      // CCFWrapper_ConPrintCtl(0U);
    }
  }

  return Rval;
}
