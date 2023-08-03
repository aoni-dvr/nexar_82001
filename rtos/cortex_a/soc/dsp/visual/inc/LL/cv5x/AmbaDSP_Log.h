/**
*  @file AmbaDSP_Log.h
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
*  @details Definitions for SSP logging Module.
*
*/

#ifndef AMBA_DSP_LOG_H
#define AMBA_DSP_LOG_H


#include "AmbaTypes.h"

#define LL_LOG_ARGNUM       (3U)
#define LL_LOG_ELEM_NUM     (1024U)
#define LL_LOG_MSG_LEN      (128U)

/* LL Log basic element */
typedef struct {
    UINT32      Time; /* logging time */
    UINT32      Type; /* logging type see @ref AMBALL_LOG_TYPE_e */
    const char  *Str; /* log buffer */
    UINT32      Args[LL_LOG_ARGNUM]; /* log buffer */
} LL_LogElem_t;

/* LL Log object */
typedef struct {
    LL_LogElem_t    *Log; /* log element buffer pointer */
    UINT32          LogNum; /* max log element num */
    UINT32          LogIdx; /* current log idx */
    UINT32          LogMode; /* U32 flag indicated logging mode */
    UINT32          PrintMode; /* U32 flag indicated print mode*/
    UINT32          Enable; /* 0:not init, 1: disable, 2:enable*/
} LL_Log_s;

/**
 * A type to specify different kind of log for LL_Log module.\n
 * Use bitwise flag as the type identifier, this allow multiple log type and(or) together.\n
 * These type also allow users to print/dump different logs in runtime.
 */
                                                  //num   //dec
#define AMBALLLOG_TYPE_ERR          0x00000001U   //1     1
#define AMBALLLOG_TYPE_MSG          0x00000002U   //2     2
#define AMBALLLOG_TYPE_DBG          0x00000004U   //3     4
#define AMBALLLOG_TYPE_INIT         0x00000008U   //4     8
#define AMBALLLOG_TYPE_BUFFER_W     0x00000010U   //5     16
#define AMBALLLOG_TYPE_BUFFER_R     0x00000020U   //6     32
#define AMBALLLOG_TYPE_CMD          0x00000040U   //7     64
#define AMBALLLOG_TYPE_CMD_ASYNC    0x00000080U   //8     128
#define AMBALLLOG_TYPE_CMD_GRP      0x00000100U   //9     256
#define AMBALLLOG_TYPE_CMD_DEF      0x00000200U   //10    512
#define AMBALLLOG_TYPE_ISR          0x00000400U   //11    1024
#define AMBALLLOG_TYPE_VIDEO_H264   0x00000800U   //12    2048
#define AMBALLLOG_TYPE_VIDEO_HEVC   0x00001000U   //13    4096
#define AMBALLLOG_TYPE_VIDEO_MJPG   0x00002000U   //14    8192
#define AMBALLLOG_TYPE_STILL        0x00004000U   //15    16384
#define AMBALLLOG_TYPE_JPEG         0x00008000U   //16    32678
#define AMBALLLOG_TYPE_AAA_STL      0x00010000U   //17    65536
#define AMBALLLOG_TYPE_AAA_VID      0x00020000U   //18    131072
#define AMBALLLOG_TYPE_AAA_VIN      0x00040000U   //19    262144
#define AMBALLLOG_TYPE_AAA_HYB      0x00080000U   //20    524288
#define AMBALLLOG_TYPE_PTS          0x00100000U   //21    1048576
#define AMBALLLOG_TYPE_WRAPPER      0x00200000U   //22    2097152
#define AMBALLLOG_TYPE_MSG_HEAD     0x00400000U   //23    4194304
#define AMBALLLOG_TYPE_INT          0x00800000U   //24    8388608
#define AMBALLLOG_TYPE_HYBRID       0x01000000U   //25    16777216
#define AMBALLLOG_TYPE_BATCH        0x02000000U   //26    33554432
#define AMBALLLOG_TYPE_VIN_INT      0x04000000U   //27    67108864
#define AMBALLLOG_TYPE_IN_LOCKED    0x08000000U   //28    134217728

/**
 *  Moule ID for AmbaPrint
 */
#define AMBA_SSP_PRINT_MODULE_ID       ((UINT16)(SSP_ERR_BASE >> 16U))

/**
 * Write AmbaLL_Log
 * @param [in] Type  Log type flag @ref AMBALL_LOG_TYPE_e
 * @param [in] Str   The string is going to log
 * @param [in] Var0  Variable0
 * @param [in] Var1  Variable1
 * @param [in] Var2  Variable2
 * @return none
 */
void AmbaLL_Log(UINT32 Type, const char *Str, UINT32 Var0, UINT32 Var1, UINT32 Var2);

/**
 * Init, assign buffer for AmbaLL_Log, if LogAddr is NULL,\n
 * will use default settings.
 * @param [in] LogAddr  Log buffer address, NULL to use code space
 * @param [out] LogNum   Log Buffer  element number
 * @return 0 - OK, other - ErrorCode
 */
//UINT32 AmbaLL_LogInit(LL_LogElem_t *LogAddr, UINT32 LogNum);

/**
 * Enable / Disable the AmbaLL_Log
 * @param [in] Enable  0/1
 * @return 0 - OK, other - ErrorCode
 */
UINT32 AmbaLL_LogEnable(UINT8 Enable);

/**
 * Set AmbaLL_Log attribute function
 * @param [in] LogMode   To specify which type of log is going to log @ref AMBALL_LOG_TYPE_e
 * @param [in] PrintMode To specify which type of log is going to print @ref AMBALL_LOG_TYPE_e
 * @return 0 - OK, other - ErrorCode
 */
UINT32 AmbaLL_LogSetMode(UINT32 LogMode, UINT32 PrintMode);

/**
 * Get AmbaLL_Log attribute function
 * @param [out] *LogMode   A valid pointer to get current log mode @ref AMBALL_LOG_TYPE_e
 * @param [out] *PrintMode A valid pointer to get current print mode @ref AMBALL_LOG_TYPE_e
 * @return 0 - OK, other - ErrorCode
 */
UINT32 AmbaLL_LogGetMode(UINT32 *LogMode, UINT32 *PrintMode);

/**
 * Prints AmbaLL_Log entries on the screen.
 * @param [in] Entry
 */
void AmbaLL_LogShow(UINT32 Entry);

/**
 * Get a command/message log bit of input command code
 * @param [in] CmdCode String of a command code
 * @return Command log bit, 1 - Enable the log, 0 - Disable the log
 */
UINT32 DSP_ShowCmdMsgLog(UINT32 CmdCode);

/**
 * Setup the message log bit of input command category
 * @param [in] CmdCat Target command category to be enabled/disabled message log
 * @param [in] On Enable bit: 1 - enable the log; 0 - disable the log
 */
void DSP_SetMsgLogCat(UINT32 CmdCat, UINT8 On);

/**
 * Setup whether a specific command will be logged with its parameters
 * The input is usually UINT32 command code which comes from code.
 * @param [in] StrCmdCode String of a command code starting with 0x
 * @param [in] On Enable bit: 1 - enable the log; 0 - disable the log
 * @return 0 - OK, other - ErrorCode
 */
//UINT32 DSP_SetMsgLog(const char *StrCmdCode, UINT8 On);

/**
 * Hash function that maps a message code to a index of CmdLog
 * @param [in] MsgCode Command code of a DSP command
 * @return 0 - OK, other - ErrorCode
 */
extern UINT32 MsgCodeHash(UINT32 MsgCode);

/**
 * Setup a command/message log bit based on the index derived from the hash function
 * @param [in] Idx Index of CmdLog, which represents a specific command
 * @param [in] Val Enable bit: 1 - enable the log; 0 - disable the log
 */
extern void SetMsgLogBit(UINT32 Idx, UINT8 Val);

/**
 * Write to AmbaPrint buffer using AMBA_SSP_PRINT_MODULE_ID module id
 * @param [in] pFmt  The string is going to log
 * @param [in] pArg1 String variable 1
 * @param [in] pArg2 String variable 2
 * @param [in] pArg3 String variable 3
 * @param [in] pArg4 String variable 4
 * @param [in] pArg5 String variable 5
 * @return none
 */
void AmbaLL_LogStr5(const char *pFmt, const char *pArg1, const char *pArg2, const char *pArg3, const char *pArg4, const char *pArg5);

/**
 * Write to AmbaPrint buffer using AMBA_SSP_PRINT_MODULE_ID module id
 * @param [in] pFmt The string is going to log
 * @param [in] Arg1 Unsigned int variable 1
 * @param [in] Arg2 Unsigned int variable 2
 * @param [in] Arg3 Unsigned int variable 3
 * @param [in] Arg4 Unsigned int variable 4
 * @param [in] Arg5 Unsigned int variable 5
 * @return none
 */
void AmbaLL_LogUInt5(const char *pFmt, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5);

/**
 * Write to AmbaPrint buffer using AMBA_SSP_PRINT_MODULE_ID module id
 * @param [in] pFmt  The string is going to log
 * @param [in] pArg1 Int variable 1
 * @param [in] pArg2 Int variable 2
 * @param [in] pArg3 Int variable 3
 * @param [in] pArg4 Int variable 4
 * @param [in] pArg5 Int variable 5
 * @return none
 */
//void AmbaLL_LogInt5(const char *pFmt, INT32 Arg1, INT32 Arg2, INT32 Arg3, INT32 Arg4, INT32 Arg5);

/**
 * Write warning log to AmbaPrint buffer using AMBA_SSP_PRINT_MODULE_ID module id
 * @param [in] WarnType  The wraning type of log
 * @param [in] pFmt  The string is going to log
 * @param [in] pArg1 Unsigned variable 1
 * @param [in] pArg2 Unsigned variable 2
 * @param [in] pArg3 Unsigned variable 3
 * @param [in] pArg4 Unsigned variable 4
 * @param [in] pArg5 Unsigned variable 5
 * @return none
 */
#define AMBALLLOG_WARN_BATCHQ_FULL      (0U)
#define AMBALLLOG_WARN_FRMCNT_FULL      (1U)
#define AMBALLLOG_WARN_SEND_ENCINFO     (2U)
#define AMBALLLOG_WARN_NUM              (3U)
void AmbaLL_LogWarnUInt5(const UINT16 WarnType, const char *pFmt, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5);

#endif  /* AMBA_SSP_LOG_H */
