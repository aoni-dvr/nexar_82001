/**
 *  @file AmbaDSP_ArmComm.h
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
 *  @details Definitions & constants for the APIs of ARM-DSP communication
 *
 */

#ifndef AMBA_DSP_ARM_COMM_H
#define AMBA_DSP_ARM_COMM_H

#include <AmbaVer.h>
#include "AmbaDSP.h"
#include "AmbaDSP_ArmComm_Def.h"
#include "AmbaDSP_uCode.h"
#include "dsp_osal.h"

#define SUPPORT_DSP_ASYNC_ENCMSG
#define SUPPORT_ORC_ASSERT
//#define SUPPORT_DSP_SET_RAW_CAP_CNT
//#define SUPPORT_DSP_DEC_BATCH
//#define SUPPORT_DSP_SET_IDSP_DBG_CMD

/* Algo protection period */
#define ALGO_PROTECT_PERIOD_10SEC       ((12288U*10U)*1000U) //default generate protection key every 10sec, ucode rsv 30bits

/* CmdProtocol Relative */
#define MAX_BATCH_CMD_SET_DEPTH         (1U)  // BatchCmdSet for IsoCfgUpdate
#ifdef USE_FLEXIBLE_BATCH_CMD_POOL
#define MAX_BATCH_CMD_POOL_NUM          (130U) // Maximal achievable protocol delay = 130/2 = 65, minumum = 10
#else
#define MAX_BATCH_CMD_POOL_NUM          (10U) // Max possible protocol delay
#endif
#define MAX_BATCH_CMD_NUM               (13U) // IsoCfgUpdate(Fixed at FirstSlot and replace by NopCmd default)/Warp/CA/PpStrm/PrevSetup + SlaveGrp[7] + CmdEnd
#define MIN_BATCH_CMD_NUM               (2U)  // IsoCfgUpdate + CmdEnd
#define MAX_BATCH_CMD_END               (0xDEADBEEFU)
#define BATCH_ISOCFGCMD_SLOT_IDX        (0x0U)
#define BATCH_CMD_START_NUM             (0x1U) //first CmdSlot for IsoCfg all the time
#define MAX_GROUP_STRM_NUM              (AMBA_DSP_MAX_YUVSTRM_NUM) // Max Effect YuvStrm
#define MAX_GROUP_CMD_POOL_NUM          (8U)  // Max possible protocol delay
#define MAX_GROUP_CMD_SUBJOB_NUM        (10U) // Max SubJob : 8Chan+2Vout includes Master
#define MAX_GROUP_CMD_NUM               (3U)  // VPROC : Warp/CAWarp/PrevSetup ; VOUT : OsdSetup/VideoSetup
#define CMD_SIZE_IN_BYTE                (128U) //128 byte
#define CMD_SIZE_IN_WORD                (32U) //128 byte = 32 word
#define MAX_SIDEBAND_BUF_NUM            (8U)  // maximum number of Sideband Buffer
#define MAX_SIDEBAND_BUF_ARRAY_NUM      (MAX_TOKEN_ARRAY)  // 4U:maximum number of Sideband Buffer Array
#define DSP_BUF_ALIGNMENT               (AMBA_CACHE_LINE_SIZE)

#define DSP_ARM_ISR_TASK_PRIORITY       (1U)
#define DSP_ARM_ISR_TASK_STACK_SIZE     (0x1000U)
#define DSP_ARM_ISR_TASK_CORE_SELECTION (0x1U)

#define DEFAULT_MSGBUF_OVERFLOW_ASSERT  (5U)    //MsgBuf overflow threshold: remaining msg space < 5

//extern AMBA_DSP_VERSION_INFO_s AmbaDSP_UCodeVersionInfo;

#define DSP_HEADER_U32_PAD_NUM  ((128U - sizeof(dsp_header_cmd_t))/4U)
typedef struct {
    dsp_header_cmd_t  Contents;
    UINT32 pad[DSP_HEADER_U32_PAD_NUM];
} AMBA_DSP_HEADER_CMD_s;

typedef struct {
    dsp_msg_t           *pMsgFifo;      /* pointer to the message fifo */
    dsp_msg_q_info_t    *pMsgFifoCtrl;  /* pointer to the message fifo ctrl */
    UINT32              *pCmdReqRptr;   /* Cmd request message read pointer */
    UINT32              DspMsgRp;       /* DspDrv keep MsgRp(copy from dsp_msg_q_info_t.read_ptr) use refer to it.
                                         * due to HL need buffer align for dram operation, if we use read_ptr directly.
                                         * DspDrv may read a over-written value,
                                         * so we use our own Rp and write back to dsp_msg_q_info_t.read_ptr */
    UINT32              UnReadMsgNum;   /* Unread message number: used to detect buffer overflow */
    UINT32              DspMsgLastPtr;  /* When we start parsing all message buffers,
                                         * DspDrv decides the last index that needs to be read (copy from *pCmdReqRptr)
                                         * Make sure all messages are in the correct order */
} AMBA_DSP_MSG_BUF_CTRL_s;

typedef struct {
    UINT32                  *pCmdQueueBaseAddr;     /* pointer to the base physical address of command queue in dsp_init_data_t */
    UINT32                  *CurCmdSeqNo;           /* current command sequence number */
    UINT32                  PrevCmdSeqNo;           /* previous command sequence number received from DSP */
    UINT8                   SyncCounter;            /* sync counter for command request */

    UINT8                   MaxNumCmds;             /* maximum number of commands per Buffer */
    UINT8                   MaxNumCmdBuf;           /* maximum number of Command Buffer */
    UINT8                   ReadPtr;                /* pointer to the read command buffer */
    UINT8                   WritePtr;               /* pointer to the write command buffer */
    dsp_cmd_t               *pCmdBufBaseAddr;       /* pointer to the base address of command buffer */
    dsp_cmd_t               *pGroupCmdBufBaseAddr;  /* pointer to the base address of group command buffer */
    UINT8                   Id;                     /* Id */
    AMBA_DSP_MSG_BUF_CTRL_s MsgBufCtrl;             /* Sync message buffer ctrl */
    UINT64                  IntFlagCounter;         /* Counter of VDSP ISR */
    UINT32                  CmdLock;                /* Lock for writing SendInputData cmd, each bit indicates VinId */
} AMBA_DSP_SYNC_CMD_BUF_CTRL_s;

typedef struct {
    UINT8                   SyncCounter;            /* sync counter for command request */
    UINT8                   MaxNumCmds;             /* maximum number of commands per Buffer */
    dsp_cmd_t               *pCmdBufBaseAddr;       /* pointer to Async command buffer */
    AMBA_DSP_MSG_BUF_CTRL_s MsgBufCtrl;             /* Async message buffer ctrl */
} AMBA_DSP_ASYNC_CMD_BUF_CTRL_s;

extern const UINT8 SyncCmdBufCtrlIdMap[AMBA_DSP_NUM_CMD_WRITE];

/**
 * Inital function of ArmComm
 * Must be checked before using all variables on the ArmComm
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 ArmCommInit(void);

/**
 * Write a command into the default command buffer
 * In case that default command buffer is full,
 * write the command to the synccommand buffer
 * @param [in] SysState System boot mode
 * @param [in] pCmdData Pointer to DSP command data
 * @param [in] CmdDataSize Command data size in byte
 * @return 0 - OK, others - ErrorCode
 */
UINT32 WriteDefaultCmdBuf(UINT8 SysState, const void *pCmdData, UINT32 CmdDataSize);

/**
 * Write a command into the async command buffer
 * @param [in] pCmdData Pointer to DSP command data
 * @param [in] CmdDataSize Command data size in byte
 * @return 0 - OK, others - ErrorCode
 */
UINT32 WriteAsyncCmdBuf(const void *pCmdData, UINT32 CmdDataSize);

/**
 * Write a command into the sync command buffer
 * @param [in] pCmdBufCtrl Pointer to the command buffer control block
 * @param [in] pCmdData Pointer to DSP command data
 * @param [in] CmdDataSize Command data size in byte
 * @return 0 - OK, others - ErrorCode
 */
UINT32 WriteCmdBuf(AMBA_DSP_SYNC_CMD_BUF_CTRL_s *pCmdBufCtrl, const void *pCmdData, UINT32 CmdDataSize);

/**
 * Write a command into the group command buffer
 * @param [in] pCmdBufCtrl Pointer to the command buffer control block
 * @param [in] pCmdData Pointer to DSP command data
 * @param [in] CmdDataSize Command data size in byte
 * @return 0 - OK, others - ErrorCode
 */
UINT32 WriteGroupCmdBuf(const AMBA_DSP_SYNC_CMD_BUF_CTRL_s *pCmdBufCtrl, const void *pCmdData, UINT32 CmdDataSize);

/**
 * Copy group commands to sync command buffer
 * @param [in] pCmdBufCtrl Pointer to the command buffer control block
 * @return 0 - OK, others - ErrorCode
 */
//extern UINT32 SendGroupCmdBuf(AMBA_DSP_SYNC_CMD_BUF_CTRL_s *pCmdBufCtrl);

extern UINT32 SetupDSPInitData(const AMBA_DSP_SYS_CONFIG_s *pDspSysConfig, ULONG *pChipInfoAddr, UINT8 ParLoadEnable);

/**
* DSP boot entry function
* @param [in]  pDspSysConfig point to dsp sys config buffer
* @param [in/out]  ChipInfoAddr point for chip info buffer address
* @param [in]  MonitorMask DspMonitor mask
* @param [in]  IdspBinShare share idsp binary
* @return ErrorCode
*/
extern UINT32 DSP_Bootup(const AMBA_DSP_SYS_CONFIG_s *pDspSysConfig,
                         ULONG *pChipInfoAddr,
                         UINT32 MonitorMask,
                         UINT8 ParLoadEnable,
                         UINT8 IdspBinShare);

/**
* Update DSP working buffer
* @param [in]  WorkAreaAddr buffer address
* @param [in]  WorkAreaSize buffer size
* @return ErrorCode
*/
extern UINT32 DSP_UpdateWorkBuffer(ULONG WorkAreaAddr, UINT32 WorkAreaSize);

/**
 * Send sync command to DSP
 * This function is called inside ISR
 * @param [in] CmdBufID Sync command buffer ID
 * @return 0 - OK, others - ErrorCode
 */
//extern void DSP_SendSyncCmd(UINT8 CmdBufID);

//extern UINT32 DSP_WriteCmd(DSP_CMD_WRITE_TYPE_e CmdType, UINT32 BufType, void *pCmdData, UINT32 CmdDataSize);

/**
 * Query certain cmd exist in CmdQ or not, and return CmdAddr
 * @param [in] WriteMode The mode of write to buffer
 * @param [in] CmdCode Cmd code to be searched
 * @param [in/out] CmdAddr Cmd buffer address
 * @param [in] Val0 search condition #0
 * @param [in] Val1 search condition #1
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdQuery(UINT8 WriteMode, UINT32 CmdCode, ULONG *pCmdAddr, UINT32 Val0, UINT32 Val1);

/**
 * Write command into the corresponding buffer
 * This is an API for command wrapper
 * @param [in] WriteMode The mode of write to buffer
 * @param [in] pCmdData Pointer to DSP command data
 * @param [in] CmdDataSize Command data size in Byte
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdSend(UINT8 WriteMode, const void *pCmdData, UINT32 CmdDataSize);

extern UINT32 AmbaLL_CmdSendSetupInitCrc(UINT8 WriteMode, const void *pCmdData, UINT32 CmdDataSize);

/**
 * Process buffered commands
 * This is an API for AmbaDSP_FSM
 * @param [in] GroupWrite The type of command to be processed
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 DSP_ProcessBufCmds(UINT8 GroupWrite);

/**
 * Print dsp init data information
 * This is an API for the debugging via test comamnd
 */
extern void DSP_DumpDspInitDataInfo(void);

/**
 * Print dsp default command data information
 */
//extern void DSP_DumpCmdDefaultBuffer(void);

/**
 * Print dsp command buffer information
 * @param [in] CmdType The type of command to be printed
 * @param [in] Detail 1/0, whether to print buffer details
 */
//extern void DSP_DumpCmdBuffer(UINT8 CmdType, UINT8 Detail);

/**
 * Clear default command buffer
 */
extern void DSP_ClrDefCfgBuffer(void);

/**
 * Check and log the default command buffer
 * Check from AmbaDSP_InitData.Data.default_config_daddr
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 DSP_CheckDefaultCmds(void);

/**
 * reset group command
 * @param [in] CmdBufID command type
 */
extern UINT32 DSP_ClearGroupCmdBuffer(UINT8 CmdBufID);

/**
 * Reset Command and Message buffers
 */
extern void DSP_ResetCmdMsgBuf(void);

/**
 * Inform the LL about the DSP suspend status
 * @param [in] Suspend TRUE/FALSE, indicating DSP suspend
 */
extern void DSP_Suspend(UINT32 Suspend);

/**
 * Set up uCode for resuming DSP
 * @param [in] SysState Target system state to be resumed
 * @return 0 - OK, others - ErrorCode
 */
//extern UINT32 DSP_ResumeSetupInitData(UINT8 SysState);

/**
 * Get DSP command category
 * @param [in] Cmd Target command
 * @param [out] pCat Pointer to command category
 * @return 0 - OK, others - ErrorCode
 */
extern void GET_DSP_CMD_CAT(UINT32 Cmd, UINT32 *pCat);

/**
 * Wait message event flag generated by DSP interrupt
 * This is called by AmbaDSP_MainWaitFlag
 * @param [in] Flag Target event flag
 * @param [out] ActualFlag Actual event flags
 * @param [in] TimeOut Wait time out
 * @return 0 - OK, other - ErrorCode
 */
UINT32 LL_WaitDspMsgFlag(UINT32 Flag, UINT32 *ActualFlag, UINT32 TimeOut);

/**
 * Read message from all message queues
 * This is called by AmbaDSP_MainMsgParseEntry
 * @param [in] EntryArg Entry arg
 * @return 0 - OK, other - ErrorCode
 */
UINT32 LL_MsgParseEntry(UINT32 EntryArg);

/**
 * Check DSP assertion and send the AMBA_DSP_EVENT_ERROR event
 */
void LL_CheckDspAssert(UINT8 IdspBinShare);

/**
 * Detect overflow warning in the message buffer
 * @param [in] pMsgBufCtrl Pointer to the message buffer control block
 * @param [in] Threshold Overflow warning threshold
 * @return 0 - OK, 1 - Overflow warning
 */
//UINT32 DSP_MsgOverflowDetection(const AMBA_DSP_MSG_BUF_CTRL_s *pMsgBufCtrl, const UINT32 Threshold);

void DSP_DspParLoadUnlock(UINT8 FullUnlock, UINT32 Mask);

extern UINT8 IsNormalWriteMode(UINT8 WriteMode);
extern UINT8 IsGroupWriteMode(UINT8 WriteMode);
#if defined (CONFIG_THREADX)
extern void AmbaVer_SetDspDspKernelLL(AMBA_VerInfo_s *pVerInfo);
#endif

#if defined (CONFIG_ENABLE_DSP_DIAG)
extern UINT32 DSP_GetFirstCmdAddr(UINT32 CmdBufIdx, ULONG *pCmdAddr);
extern UINT32 DSP_GetNextMsgAddr(UINT32 MsgBufIdx, ULONG *pMsgAddr);
extern UINT32 DSP_GetMsgPtrData(UINT32 MsgBufIdx, UINT32 *pMaxNumMsg, UINT32 **pReadPtr, UINT32 **pWritePtr);
#endif

/**
 * Check command buffer lock
 */
UINT32 DSP_CheckCmdBufLock(const UINT16 VinId);

#if defined (CONFIG_THREADX)
/**
 * Delete ArmCommIsrTask
 */
extern UINT32 DeleteArmCommIsrTask(void);
#endif

/**
 * Clear all unread messages
 */
extern void DSP_ClearAllUnreadMsg(void);

#endif  /* AMBA_DSP_ARM_COMM_H */
