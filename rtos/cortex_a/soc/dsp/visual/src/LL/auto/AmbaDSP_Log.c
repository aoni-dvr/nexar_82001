/**
 *  @file AmbaDSP_Log.c
 *
 *  Copyright (c) 2020 Ambarella International LP
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
*  @details SSP logging Module
*
*/

#include "AmbaDSP.h"
#include "AmbaDSP_Log.h"
#include "AmbaDSP_uCode.h"
#include "AmbaDSP_CommonAPI.h"
#include "dsp_osal.h"

static LL_LogElem_t LogBuffer[LL_LOG_ELEM_NUM] GNU_SECTION_NOZEROINIT;

static LL_Log_s LL_Log = {
        .Log = LogBuffer,
        .LogNum = LL_LOG_ELEM_NUM,
        .LogIdx = 0,
        .LogMode = 0,
        .PrintMode = 0,
        .Enable = 0,
};

static UINT32 AmbaLL_LogInit(LL_LogElem_t *LogAddr, UINT32 LogNum)
{
    ULONG ULAddr;

    if (LL_Log.Enable == (UINT8)0) {
        if (LogAddr != NULL) {
            LL_Log.Log = LogAddr;
            LL_Log.LogNum = LogNum;
        }

        LL_Log.LogMode = ((UINT32)AMBALLLOG_TYPE_ERR |
                          (UINT32)AMBALLLOG_TYPE_MSG |
                          (UINT32)AMBALLLOG_TYPE_DBG |
                          (UINT32)AMBALLLOG_TYPE_INIT |
                          (UINT32)AMBALLLOG_TYPE_BUFFER_W |
                          (UINT32)AMBALLLOG_TYPE_BUFFER_R |
                          (UINT32)AMBALLLOG_TYPE_CMD |
                          (UINT32)AMBALLLOG_TYPE_CMD_ASYNC |
                          (UINT32)AMBALLLOG_TYPE_CMD_GRP |
                          (UINT32)AMBALLLOG_TYPE_CMD_DEF |
                          (UINT32)AMBALLLOG_TYPE_MSG_HEAD |
                          (UINT32)AMBALLLOG_TYPE_ISR);
        LL_Log.PrintMode = ((UINT32)AMBALLLOG_TYPE_ERR |
                            (UINT32)AMBALLLOG_TYPE_MSG |
                            (UINT32)AMBALLLOG_TYPE_WRAPPER);

        LL_Log.Enable = 2U;

        dsp_osal_typecast(&ULAddr, &LL_Log.Log);
        AmbaLL_LogUInt5("       LogBuf 0x%X%X LogNum %d LogMode 0x%X PrintMode 0x%X",
            DSP_GetU64Msb((UINT64)ULAddr), DSP_GetU64Lsb((UINT64)ULAddr),
            LL_Log.LogNum, LL_Log.LogMode, LL_Log.PrintMode);
    } else {
        AmbaLL_LogStr5("%s Inited", __func__, NULL, NULL, NULL, NULL);
    }

    return OK;
}

void AmbaLL_Log(UINT32 Type, const char* Str, UINT32 Var0, UINT32 Var1, UINT32 Var2)
{
    osal_spinlock_t Spinlock;
    UINT32 Rval = OK;
    UINT32 Enable = LL_Log.Enable;

    if (Enable == 0U) {
        AmbaLL_LogStr5("%s auto init", __func__, NULL, NULL, NULL, NULL);
        Rval = AmbaLL_LogInit(NULL, 0U);
        Enable = LL_Log.Enable;
    }

    if ((Enable == 2U) && (Rval == OK)) {
        if ((Type & LL_Log.LogMode) > 0U) {
            LL_LogElem_t *Msg;
            UINT32 Time = 0U;
            UINT32 Idx;
            UINT32 *pArgs;

            Rval = dsp_osal_get_sys_tick(&Time);
            AmbaMisra_TouchUnused(&Rval);

            /* Get current DSP log index */
             if ((Type & AMBALLLOG_TYPE_IN_LOCKED) == 0U) {
                /* Don't need to lock again when it is already in SpinLocked state */
                Rval = dsp_osal_spinlock(&Spinlock);
                AmbaMisra_TouchUnused(&Rval);
            }
            Idx = LL_Log.LogIdx;
            LL_Log.LogIdx += 1U;
            if ((Type & AMBALLLOG_TYPE_IN_LOCKED) == 0U) {
                Rval = dsp_osal_spinunlock(&Spinlock);
                AmbaMisra_TouchUnused(&Rval);
            }
            Idx = Idx % (LL_Log.LogNum);

            /* saving log */
            Msg = &LL_Log.Log[Idx];
            pArgs = Msg->Args;
            Msg->Type = Type;
            Msg->Time = Time;
            Msg->Str = Str;
            pArgs[0] = Var0;
            pArgs[1] = Var1;
            pArgs[2] = Var2;
        }
        if ((Type & LL_Log.PrintMode) > 0U) {
            /* call console print */
            AmbaLL_LogUInt5(Str, Var0, Var1, Var2, 0U, 0U);
        }
    } else {
        // DO NOTHING
    }

    return;
}

UINT32 AmbaLL_LogEnable(UINT8 Enable)
{
    LL_Log.Enable = Enable;
    return OK;
}

void AmbaLL_LogShow(UINT32 Entry)
{
    UINT32 IdxStart, IdxMax, IdxLog, PrintedLine, i;
    UINT32 StrLen;
    UINT32 Idx;
    UINT32 OrigEna;
    char StrBuffer[LL_LOG_MSG_LEN];
    UINT32 Rval;

    Rval = dsp_osal_memset(StrBuffer, 0, sizeof(char)*LL_LOG_MSG_LEN);
    LL_PrintErrLine(Rval, __func__, __LINE__);

    if (LL_Log.Log == NULL) {
        AmbaLL_LogStr5("%s not init", __func__, NULL, NULL, NULL, NULL);
    } else {
        OrigEna = LL_Log.Enable;
        LL_Log.Enable = 1;
        IdxMax = LL_Log.LogNum;
        IdxLog = LL_Log.LogIdx % IdxMax;
        if (Entry > IdxMax) {
            Idx = IdxMax;
        } else {
            Idx = Entry;
        }

        if (LL_Log.LogIdx < IdxMax) {
            IdxStart = 0;
            if (Entry > IdxLog) {
                Idx = IdxLog;
            }
        } else if (IdxLog > Idx) {
            IdxStart = IdxLog - Idx;
        } else {
            IdxStart = (IdxMax - Idx) + IdxLog;
        }
        AmbaLL_LogUInt5("Entry[%d] Log.LogIdx[%d] IdxLog[%d] Idx[%d] IdxStart[%d]",
                        Entry, LL_Log.LogIdx, IdxLog, Idx, IdxStart);
        dsp_osal_print_flush();

        PrintedLine = 0;

        for(i = 0; i < Idx; i++) {
            const LL_LogElem_t *Msg = &LL_Log.Log[IdxStart];
            const UINT32* pArgs = Msg->Args;

            if (Msg->Str == NULL) {
                continue;
            } else {
                StrLen = dsp_osal_str_print_u32(StrBuffer,
                                                LL_LOG_MSG_LEN,
                                                "[%d]", 1, &Msg->Time);
                Rval = dsp_osal_str_append(&StrBuffer[StrLen], LL_LOG_MSG_LEN, Msg->Str);
                LL_PrintErrLine(Rval, __func__, __LINE__);
                AmbaLL_LogUInt5(StrBuffer, pArgs[0], pArgs[1], pArgs[2], 0U, 0U);
                if ((PrintedLine % 256U) == 0U) {
                    dsp_osal_print_flush();
                }
            }

            IdxStart = (IdxStart + 1U) % IdxMax;
            PrintedLine++;
        }
        LL_Log.Enable = OrigEna;
    }

    return;
}

UINT32 AmbaLL_LogSetMode(UINT32 LogMode, UINT32 PrintMode)
{
    LL_Log.LogMode = LogMode;
    LL_Log.PrintMode = PrintMode;

    return OK;
}

UINT32 AmbaLL_LogGetMode(UINT32 *LogMode, UINT32 *PrintMode)
{
    UINT32 Rval = OK;

    if ((LogMode == NULL) || (PrintMode == NULL)) {
        Rval = DSP_ERR_0000;
    } else {
        *LogMode = LL_Log.LogMode;
        *PrintMode = LL_Log.PrintMode;
    }

    return Rval;
}

#define MAX_MSG_NUM_PER_CAT     32U/*16*/            // 4bit + 2 bit
#define MSG_HASH_BUCK_NUM       32U/*8*/              /* (11 * MAX_CMD_NUM_PER_CAT) / 32 +2 */
static UINT32 MsgLog[MSG_HASH_BUCK_NUM] = {0};      /* Command log hash bit flag */

UINT32 MsgCodeHash(UINT32 MsgCode)
{
    UINT32 Buck0, Buck1, Buck2;

    Buck0 = ((MsgCode >> (UINT32)24) & (UINT32)(0xF));      //0~15  16(Bit[24:27])
    Buck1 = ((MsgCode >> (UINT32)12) & (UINT32)(0x3));      //0~3    4(Bit[12:13])
    Buck2 = (MsgCode & (UINT32)(0xF));                      //0~15  16(Bit[0:3])

    //AmbaPrint("[%s] MsgCode = 0x%X b0 = %u b1 = %u b2 = %u", __func__, MsgCode, Buck0, Buck1, Buck2);
    return (Buck0 << 5U) + (Buck1 << 4U) + Buck2;
}

void SetMsgLogBit(UINT32 Idx, UINT8 Val)
{
    UINT32 IntIdx = Idx >> 5;

    if (IntIdx >= (UINT32)MSG_HASH_BUCK_NUM) {
        AmbaLL_LogUInt5("SetMsgLogBit IntIdx %d too large", IntIdx, 0U, 0U, 0U, 0U);
    } else {
        if (Val > (UINT8)0) {
            MsgLog[IntIdx] |= (UINT32)1 << (Idx % (UINT32)32);
        } else {
            MsgLog[IntIdx] &= ~((UINT32)1 << (Idx % (UINT32)32));
        }
    }
}

#if 0
UINT32 DSP_SetMsgLog(const char *StrCmdCode, UINT8 On)
{
    UINT32 Rval;
    UINT32 Data;

    Rval = dsp_osal_str_to_u32(StrCmdCode, &Data);
    LL_PrintErrLine(Rval, __func__, __LINE__);
    Rval = SetMsgLogBit(MsgCodeHash(Data), On);

    return Rval;
}
#endif

void DSP_SetMsgLogCat(UINT32 CmdCat, UINT8 On)
{
    UINT32 Idx;

    if ((CmdCat >  (UINT32)CAT_DSP_HEADER) && (CmdCat <=  (UINT32)CAT_POSTPROC)) {
        UINT32 Start   = (CmdCat -  (UINT32)CAT_DSP_CFG) * (UINT32)MAX_MSG_NUM_PER_CAT;
        UINT32 End     = Start + (UINT32)MAX_MSG_NUM_PER_CAT;
        for (Idx = Start; Idx <= End; Idx++) {
            SetMsgLogBit(Idx, On);
        }
    } else {
        AmbaLL_LogUInt5("Unknown command category [%d]", CmdCat, 0U, 0U, 0U, 0U);
    }
}

/**
 * Get a command/message log bit based on the index derived from the hash function
 * This function is called inside DSP_ShowCmdMsgLog
 * @param [in] Idx Index of CmdLog, which represents a specific command
 * @return Command log bit, 1 - Enable the log, 0 - Disable the log
 */
static inline UINT32 GetCmdMsgLogBit(UINT32 Idx)
{
    UINT32 IntIdx = (Idx >> 5U);
    return 1U & (MsgLog[IntIdx] >> (Idx & (32U - 1U)));
}

UINT32 DSP_ShowCmdMsgLog(UINT32 CmdCode)
{
    UINT32 Rval;

    if(GetCmdMsgLogBit(MsgCodeHash(CmdCode)) == 0U) {
        Rval = 0U;
    } else {
        Rval = 1U;
    }
    return Rval;
}

void AmbaLL_LogStr5(const char *pFmt, const char *pArg1, const char *pArg2, const char *pArg3, const char *pArg4, const char *pArg5)
{
    UINT16 ModuleId = AMBA_SSP_PRINT_MODULE_ID;
    dsp_osal_module_printS5(ModuleId, pFmt, pArg1, pArg2, pArg3, pArg4, pArg5);
}

void AmbaLL_LogUInt5(const char *pFmt, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5)
{
    UINT16 ModuleId = AMBA_SSP_PRINT_MODULE_ID;
    dsp_osal_module_printU5(ModuleId, pFmt, Arg1, Arg2, Arg3, Arg4, Arg5);
}
#if 0
void AmbaLL_LogInt5(const char *pFmt, INT32 Arg1, INT32 Arg2, INT32 Arg3, INT32 Arg4, INT32 Arg5)
{
    UINT16 ModuleId = AMBA_SSP_PRINT_MODULE_ID;
    dsp_osal_module_printI5(ModuleId, pFmt, Arg1, Arg2, Arg3, Arg4, Arg5);
}
#endif

void AmbaLL_LogWarnUInt5(const UINT16 WarnType, const char *pFmt, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5)
{
#define MAX_PRINT_WARN_CNT    (10U)
static UINT8 LL_PrintWarnCnt[AMBALLLOG_WARN_NUM] = {
    [AMBALLLOG_WARN_BATCHQ_FULL] = MAX_PRINT_WARN_CNT,
    [AMBALLLOG_WARN_FRMCNT_FULL] = MAX_PRINT_WARN_CNT,
    [AMBALLLOG_WARN_SEND_ENCINFO] = MAX_PRINT_WARN_CNT,
};
    if (WarnType < AMBALLLOG_WARN_NUM) {
        if (LL_PrintWarnCnt[WarnType] > 0U) {
            LL_PrintWarnCnt[WarnType]--;
            AmbaLL_LogUInt5(pFmt, Arg1, Arg2, Arg3, Arg4, Arg5);
        }
    }
}

