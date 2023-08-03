/**
*  @file AmbaDSP_Monitor.c
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
*  @details Amba DSP monitor
*
*/

#include "AmbaDSP_Monitor.h"
#include "AmbaDSP.h"
#include "AmbaMisraFix.h"
#include "AmbaIPC.h"
#include "AmbaWrap.h"
#include "AmbaPrint.h"
#include "AmbaKAL.h"
#include "AmbaErrorCode.h"
#include "AmbaDSP_Buffers.h"
#include "AmbaSafety.h"
#include "dsp_priv_api.h"

#ifndef CONFIG_DISABLE_SHADOW_MEMORY_WITH_DSP_MONITOR
#define DSP_MONITOR_WITH_SHADOW
#endif

//============================================================== YUVOUTSRC
#define DSP_VPROC_PIN_PREVC     (0U)
#define DSP_VPROC_PIN_PREVA     (1U)
#define DSP_VPROC_PIN_PREVB     (2U)
#define DSP_VPROC_PIN_MAIN      (3U)
#define DSP_VPROC_PIN_NUM       (4U)

#ifdef DSP_MONITOR_WITH_SHADOW
//============================================================== DSP_MONITOR_WITH_SHADOW

//#define SHADOWBASE_DSPMON       CONFIG_SHADOW_SHARED_MEM_ADDR + 0x8000 - 0xC000
//#define ASSERT_SHADOW_BASE                                      0x8000 + 512B
//#define YUVOUTSRC_SHADOW_BASE                                   0x8200 + 528B
//#define DSPMON_SHADOW_BASE                                      0x8410 + 10256B
//#define DSPMON_SHADOW_END                                       0xAC20, 0x13E0 = 5088B left

#define ASSERT_SHADOW_BASE              (SHADOWBASE_DSPMON)
#define YUVOUTSRC_SHADOW_BASE           (ASSERT_SHADOW_BASE    + 512)
#define DSPMON_SHADOW_BASE              (YUVOUTSRC_SHADOW_BASE + 528)
#define DSPMON_SHADOW_END               (DSPMON_SHADOW_BASE + 10256)
#define DSPMON_SHADOW_RSVD              (DSPMON_SHADOW_END - DSPMON_SHADOW_BASE)
#define DSPMON_SHADOW_SIZE              (0x4000)
#define DSPMON_SHADOW_CRC_QUEUE_DEPTH   (512U)

//typedef struct {
//    UINT32 ModuleId;
//    UINT32 Crc0;
//    UINT32 Crc1;
//    UINT64 UUID;
//} AMBA_DSP_MONITOR_CRC_CMPR_s;

typedef struct {
//#define SHADOW_STATE_INVALID    (0U)
#define SHADOW_STATE_INITED     (1U)
#define SHADOW_STATE_READY      (2U)
    UINT32 A53State:2;
    UINT32 A53Rsvd:30;

    UINT32 R52State:2;
    UINT32 R52Rsvd:30;

    UINT32 BufState:2;
    UINT32 BufEmpty:1;
    UINT32 BufRsvd:29;

    UINT32 CrcWP:16;
    UINT32 CrcRP:16;            //16B

    AMBA_DSP_MONITOR_CRC_CMPR_s Crc[DSPMON_SHADOW_CRC_QUEUE_DEPTH]; //20*512B=10240B
} DSPMON_SHADOW_s;

static DSPMON_SHADOW_s *pDspMonShadow;

#define DSPMON_IPC_MTX_TIMEOUT  (10U)


//============================================================== ASSERT
//typedef struct
//{
//  UINT32 module;        // 4B
//  UINT32 file_name;     // 8B
//  UINT32 file_line;     // 12B
//  UINT32 prn_str;       // 16B
//  UINT32 prn_arg1;      // 20B
//  UINT32 prn_arg2;      // 24B
//  UINT32 prn_arg3;      // 28B
//  UINT32 prn_arg4;      // 32B
//  UINT32 prn_arg5;      // 36B
//
//} assert_info_t;

//typedef struct {
//    assert_info_t   Info;
//    UINT8           Rsvd[28];        // Must allocate 64 bytes align
//} AMBA_DSP_ASSERT_INFO_s;

//#define DSP_MAX_NUM_ASSERT_INFO (8U) // 8 threads's assert info
//static AMBA_DSP_ASSERT_INFO_s DspMon_AssertInfo[DSP_MAX_NUM_ASSERT_INFO];     //64*8B=512B
//static AMBA_DSP_ASSERT_INFO_s   *pDspMon_AssertInfo;

//============================================================== YUVOUTSRC

//AMBA_DSP_MONITOR_YUV_SRC_INFO_s DspMon_YuvOutStrmSrc[DSP_VPROC_PIN_NUM];    //(4*32+4)*4=528B
static AMBA_DSP_MONITOR_YUV_SRC_INFO_s *pDspMon_YuvOutStrmSrc;

#else
//============================================================== YUVOUTSRC
#define DSPMON_IPC_CMD_CFG_YUVSRC      (0x00000106U)    //internal event
typedef struct {
    UINT32 Code;
    UINT32 YuvOutIdx;
    AMBA_DSP_MONITOR_YUV_SRC_INFO_s YuvSrcInfo;
} DSPMON_CMD_CFG_YUV_SRC_s;

AMBA_DSP_MONITOR_YUV_SRC_INFO_s DspMon_YuvOutStrmSrc[DSP_VPROC_PIN_NUM];    //(4*32+4)*4=528B
static AMBA_DSP_MONITOR_YUV_SRC_INFO_s *pDspMon_YuvOutStrmSrc = &DspMon_YuvOutStrmSrc[0];
#endif

#ifdef DSP_MONITOR_WITH_SHADOW
static inline UINT32 DspMon_IPCShadowLock(void)
{
    UINT32 Rval;

    Rval = AmbaIPC_MutexTake(AMBA_IPC_MUTEX_DSPMON, DSPMON_IPC_MTX_TIMEOUT);
    if (Rval != DSPMON_ERR_NONE) {
        AmbaPrint_PrintUInt5("[DspMon] MutexTake Fail[0x%X]", Rval, 0U, 0U, 0U, 0U);
    }
    return Rval;
}

static inline void DspMon_IPCShadowUnLock(void)
{
    UINT32 Rval;

    Rval = AmbaIPC_MutexGive(AMBA_IPC_MUTEX_DSPMON);
    if (Rval != DSPMON_ERR_NONE) {
        AmbaPrint_PrintUInt5("[DspMon] MutexGive Fail[0x%X]", Rval, 0U, 0U, 0U, 0U);
    }
}
#endif

typedef struct {
    UINT32 IpcInited:1;
    UINT32 CfgMaskInited:1;
    UINT32 Rsvd:31;
} DSP_MON_STAT_s;

static AMBA_IPC_HANDLE DspMonIpc GNU_SECTION_NOZEROINIT;

static UINT32 DspMonCfgMask[DSP_MON_MODULE_MAX_NUM] GNU_SECTION_NOZEROINIT;

static inline UINT32 IpcCmdProcOnCfgBitMask(const DSPMON_IPC_CMD_s *pIpcCmd)
{
    UINT32 Rval = DSPMON_ERR_NONE;
    const DSPMON_CMD_CFG_BIT_MASK_s *pCfgMask = NULL;

    if (pIpcCmd != NULL) {
        AmbaMisra_TypeCast(&pCfgMask, &pIpcCmd);
        if (pCfgMask->ModuleId == (DSPMON_ERR_BASE >> 16U)) {
            DspMonCfgMask[0U] = pCfgMask->BitMask;
        } else if (pCfgMask->ModuleId == (SSP_ERR_BASE >> 16U)) {
            DspMonCfgMask[1U] = pCfgMask->BitMask;
        } else if (pCfgMask->ModuleId == (IK_ERR_BASE >> 16U)) {
            DspMonCfgMask[2U] = pCfgMask->BitMask;
        } else {
            // DO NOTHING
        }
    }

    return Rval;
}

static inline UINT32 DspMon_IpcCmdProc(const DSPMON_IPC_CMD_s *pIpcCmd)
{
    UINT32 Rval = DSPMON_ERR_NONE;

    if (pIpcCmd != NULL) {
        switch (pIpcCmd->Code) {
        case DSPMON_IPC_CMD_CFG_BIT_MASK:
        {
            Rval = IpcCmdProcOnCfgBitMask(pIpcCmd);
        }
            break;
        case DSPMON_IPC_CMD_STATE_INITED:
        case DSPMON_IPC_CMD_CRC_CMPR:
        case DSPMON_IPC_CMD_ERR_NOTIFY:
        default :
        {
            AmbaPrint_PrintUInt5("[DspMon] Unknown IpcCmd[0x%X]", pIpcCmd->Code, 0U, 0U, 0U, 0U);
        }
            break;
        }
    } else {
        Rval = DSPMON_ERR_0000;
    }

    return Rval;
}

static INT32 DspMonIPC_MsgHdlr(AMBA_IPC_HANDLE IpcHandle, AMBA_IPC_MSG_CTRL_s *pMsgCtrl)
{
    UINT32 Rval = OK;
    DSPMON_IPC_CMD_s IpcCmd = {0U};
    AmbaMisra_TouchUnused(&IpcHandle);
    AmbaMisra_TouchUnused(pMsgCtrl);

    if (sizeof(DSPMON_IPC_CMD_s) >= pMsgCtrl->Length) {
        Rval = AmbaWrap_memcpy(&IpcCmd, pMsgCtrl->pMsgData, pMsgCtrl->Length);
        if (Rval != DSPMON_ERR_NONE) {
            Rval = DSPMON_ERR_0002;
            AmbaPrint_PrintUInt5("[DspMon] IpcCmd memcpy fail[0x%X]", Rval, 0U, 0U, 0U, 0U);
        } else {
            Rval = DspMon_IpcCmdProc(&IpcCmd);
        }
    } else {
        Rval = DSPMON_ERR_0001;
        AmbaPrint_PrintUInt5("[DspMon] IpcCmd Size exceed Resv[%d] In[%d]", sizeof(DSPMON_IPC_CMD_s), pMsgCtrl->Length, 0U, 0U, 0U);
    }

    return (INT32)Rval;
}

static inline UINT32 DspMon_InitIpc(void)
{
    UINT32 Rval;
    static const char DspMonName[32] = "dspmon";
    INT32 IpcErr;
    DSPMON_IPC_CMD_s IpcCmd;
    UINT32 IpcCmdSize;

    Rval = AmbaWrap_memset(&DspMonIpc, 0, sizeof(AMBA_IPC_HANDLE));
    if (Rval == DSPMON_ERR_NONE) {
        DspMonIpc = AmbaIPC_Alloc(RPMSG_DEV_AMBA, DspMonName, DspMonIPC_MsgHdlr);
        if (DspMonIpc == NULL) {
            AmbaPrint_PrintStr5("AmbaIPC_Alloc[%s] failed", DspMonName, NULL, NULL, NULL, NULL);
        } else {
            IpcErr = AmbaIPC_RegisterChannel(DspMonIpc, NULL);
            if (IpcErr != 0) {
                AmbaPrint_PrintUInt5("AmbaIPC_RegisterChannel fail[0x%X]", (UINT32)IpcErr, 0U, 0U, 0U, 0U);
                Rval = DSPMON_ERR_0002;
            } else {
                Rval = AmbaWrap_memset(&IpcCmd, 0, sizeof(DSPMON_IPC_CMD_s));
                if (Rval == DSPMON_ERR_NONE) {
                    IpcCmdSize = 0U;
                    IpcCmd.Code = DSPMON_IPC_CMD_STATE_INITED;
                    IpcCmdSize += sizeof(IpcCmd.Code);
                    IpcErr = AmbaIPC_Send(DspMonIpc, &IpcCmd, (INT32)IpcCmdSize);
                    if (IpcErr != 0) {
                        AmbaPrint_PrintUInt5("[DspMon] IPCSend[0x%X] fail[0x%X]", IpcCmd.Code, (UINT32)IpcErr, 0U, 0U, 0U);
                        Rval = DSPMON_ERR_0002;
                    }
                }
            }
        }
    }

    return Rval;
}

#ifdef DSP_MONITOR_WITH_SHADOW
static inline UINT32 DspMon_InitShdow(void)
{
    UINT32 Rval;
    ULONG ULAddr = DSPMON_SHADOW_BASE;

    Rval = DspMon_IPCShadowLock();
    if (Rval == DSPMON_ERR_NONE) {
        AmbaMisra_TypeCast(&pDspMonShadow, &ULAddr);
        if (pDspMonShadow->BufState != SHADOW_STATE_INITED) {
            pDspMonShadow->CrcWP = (UINT16)(DSPMON_SHADOW_CRC_QUEUE_DEPTH - 1U);
            pDspMonShadow->CrcRP = (UINT16)(DSPMON_SHADOW_CRC_QUEUE_DEPTH - 1U);
            pDspMonShadow->BufEmpty = (UINT8)0x1U;
            pDspMonShadow->BufState = SHADOW_STATE_INITED;
            //AmbaPrint_PrintUInt5("[DspMon] DspMonShadow BufState Init", 0U, 0U, 0U, 0U, 0U);
        }
        pDspMonShadow->A53State = SHADOW_STATE_INITED;
        DspMon_IPCShadowUnLock();
        AmbaPrint_PrintUInt5("[DspMon] DspMonShadow buffer Req[%d] Resv[%d]",
                sizeof(DSPMON_SHADOW_s), (DSPMON_SHADOW_RSVD), 0U, 0U, 0U);

    }

    return Rval;
}
#endif

static inline UINT32 DspMon_CrcWrite(const AMBA_DSP_MONITOR_CRC_CMPR_s *pCrcCmpr)
{
    UINT32 Rval;
#ifdef DSP_MONITOR_WITH_SHADOW
    UINT16 NextWp;
#ifdef DBG_SHADOW_LOCK
    UINT32 TimeS, TimeE, TimeSpend;
#endif

#ifdef DBG_SHADOW_LOCK
    Rval = AmbaKAL_GetSysTickCount(&TimeS); AmbaMisra_TouchUnused(&Rval);
#endif

    Rval = DspMon_IPCShadowLock();
    if (Rval == DSPMON_ERR_NONE) {
        /* Check */
        if (pDspMonShadow->A53State == SHADOW_STATE_INITED) {
            pDspMonShadow->A53State = SHADOW_STATE_READY;
        }

        if ((pDspMonShadow->A53State != SHADOW_STATE_READY) ||
            (pDspMonShadow->BufState != SHADOW_STATE_INITED) ||
            (pDspMonShadow->R52State != SHADOW_STATE_READY)) {
            Rval = DSPMON_ERR_0003;
        } else {
            NextWp = (pDspMonShadow->CrcWP + 1U) % DSPMON_SHADOW_CRC_QUEUE_DEPTH;

            if (pDspMonShadow->BufEmpty != (UINT8)0x1U) {
                if (pDspMonShadow->CrcWP == pDspMonShadow->CrcRP) {
                    Rval = DSPMON_ERR_0002;
                    AmbaPrint_PrintUInt5("[DspMon] CrcQ Full, Sz[%d]",
                            DSPMON_SHADOW_CRC_QUEUE_DEPTH, 0U, 0U, 0U, 0U);
                }
            }

            if (Rval == DSPMON_ERR_NONE) {
                pDspMonShadow->Crc[NextWp].ModuleId = pCrcCmpr->ModuleId;
                pDspMonShadow->Crc[NextWp].Crc0 = pCrcCmpr->Crc0;
                pDspMonShadow->Crc[NextWp].Crc1= pCrcCmpr->Crc1;
                pDspMonShadow->Crc[NextWp].UUID = pCrcCmpr->UUID;
                pDspMonShadow->CrcWP = NextWp;
                pDspMonShadow->BufEmpty = (UINT8)0x0U;
            }
        }
        DspMon_IPCShadowUnLock();
    }
#ifdef DBG_SHADOW_LOCK
    Rval = AmbaKAL_GetSysTickCount(&TimeE); AmbaMisra_TouchUnused(&Rval);
    if (TimeE >= TimeS) {
        TimeSpend = TimeE - TimeS;
    } else {
        TimeSpend = ((0xFFFFFFFFU - TimeS) + 1U) + TimeE;
    }
    AmbaPrint_PrintUInt5("[DspMon] TimeSpend[%d]", TimeSpend, 0U, 0U, 0U, 0U);
#endif

#else
    INT32 IpcErr;
    DSPMON_CMD_CRC_CMPR_s IpcCmd = {0U};

    Rval = AmbaWrap_memset(&IpcCmd, 0, sizeof(DSPMON_CMD_CRC_CMPR_s));
    if (Rval == DSPMON_ERR_NONE) {
        IpcCmd.Code = DSPMON_IPC_CMD_CRC_CMPR;
        IpcCmd.ModuleId = pCrcCmpr->ModuleId;
        IpcCmd.Crc0 = pCrcCmpr->Crc0;
        IpcCmd.Crc1 = pCrcCmpr->Crc1;
        IpcCmd.UUID = pCrcCmpr->UUID;
        IpcErr = AmbaIPC_Send(DspMonIpc, &IpcCmd, (INT32)sizeof(DSPMON_CMD_CRC_CMPR_s));
        if (IpcErr != 0) {
            AmbaPrint_PrintUInt5("[DspMon] IPCSend[0x%X] fail[0x%X]", IpcCmd.Code, (UINT32)IpcErr, 0U, 0U, 0U);
            Rval = DSPMON_ERR_0002;
        }
    }
#endif

    return Rval;
}


UINT32 dsp_mon_init(void)
{
    static DSP_MON_STAT_s DspMonStat = {0};
    UINT32 Rval = DSPMON_ERR_NONE;

    if (DspMonStat.IpcInited == (UINT8)0U) {
        Rval = DspMon_InitIpc();
        if (Rval == DSPMON_ERR_NONE) {
            DspMonStat.IpcInited = (UINT8)1U;
        }

        if (Rval == DSPMON_ERR_NONE) {
            if (DspMonStat.CfgMaskInited == (UINT8)0U) {
                Rval = AmbaWrap_memset(&DspMonCfgMask[0], 0, sizeof(UINT32)*DSP_MON_MODULE_MAX_NUM);
                if (Rval == DSPMON_ERR_NONE) {
                    DspMonStat.CfgMaskInited = (UINT8)1U;
                }
            }
        }

#ifdef DSP_MONITOR_WITH_SHADOW
        if (Rval == DSPMON_ERR_NONE) {
            Rval = DspMon_InitShdow();
        }
#endif
    }

    return Rval;
}

UINT32 dsp_mon_get_cfg(UINT32 ModuleId, AMBA_DSP_MONITOR_CONFIG_s *pMonCfg)
{
    UINT32 Rval = DSPMON_ERR_NONE;

    if (pMonCfg == NULL) {
        Rval = DSPMON_ERR_0000;
    } else {
        if (ModuleId == (DSPMON_ERR_BASE >> 16U)) {
            pMonCfg->BitMask = DspMonCfgMask[0U];
        } else if (ModuleId == (SSP_ERR_BASE >> 16U)) {
            pMonCfg->BitMask = DspMonCfgMask[1U];
        } else if (ModuleId == (IK_ERR_BASE >> 16U)) {
            pMonCfg->BitMask = DspMonCfgMask[2U];
        } else {
            Rval = DSPMON_ERR_0001;
        }
    }

    return Rval;
}

//#define DBG_SHADOW_LOCK
UINT32 dsp_mon_crc_cmpr(const AMBA_DSP_MONITOR_CRC_CMPR_s *pCrcCmpr)
{
    UINT32 Rval = DSPMON_ERR_NONE;

    if (pCrcCmpr == NULL) {
        Rval = DSPMON_ERR_0000;
    } else {
        Rval = DspMon_CrcWrite(pCrcCmpr);
    }

    return Rval;
}

UINT32 dsp_mon_heartbeat_cfg(const AMBA_DSP_MONITOR_HEARTBEAT_s *pHeartBeatCfg)
{
    UINT32 Rval = DSPMON_ERR_NONE;
    INT32 IpcErr;
    DSPMON_CMD_HEARTBEAT_s IpcCmd = {0U};
    UINT16 i;

    if (pHeartBeatCfg == NULL) {
        Rval = DSPMON_ERR_0000;
    } else {
        Rval = AmbaWrap_memset(&IpcCmd, 0, sizeof(DSPMON_CMD_HEARTBEAT_s));
        if (Rval == DSPMON_ERR_NONE) {
            IpcCmd.Code = DSPMON_IPC_CMD_CFG_HEARTBEAT;
            for (i = 0U; i < DSPMON_HEARTBEAT_NUM; i++) {
                IpcCmd.IntId[i] = pHeartBeatCfg->IntId[i];
                IpcCmd.Addr[i] = pHeartBeatCfg->Addr[i];
            }
            IpcErr = AmbaIPC_Send(DspMonIpc, &IpcCmd, (INT32)sizeof(DSPMON_CMD_HEARTBEAT_s));
            if (IpcErr != 0) {
                AmbaPrint_PrintUInt5("[DspMon] IPCSend[0x%X] fail[0x%X]", IpcCmd.Code, (UINT32)IpcErr, 0U, 0U, 0U);
                Rval = DSPMON_ERR_0002;
            }
        }
    }
    return Rval;
}

UINT32 dsp_mon_error_notify(const AMBA_DSP_MONITOR_ERR_NOTIFY_s *pErrNotify)
{
    UINT32 Rval = DSPMON_ERR_NONE;
    INT32 IpcErr;
    DSPMON_CMD_ERR_NOTIFY_s IpcCmd = {0U};

    if (pErrNotify == NULL) {
        Rval = DSPMON_ERR_0000;
    } else {
        Rval = AmbaWrap_memset(&IpcCmd, 0, sizeof(DSPMON_CMD_ERR_NOTIFY_s));
        if (Rval == DSPMON_ERR_NONE) {
            IpcCmd.Code = DSPMON_IPC_CMD_ERR_NOTIFY;
            IpcCmd.ModuleId = pErrNotify->ModuleId;
            IpcCmd.InstId = pErrNotify->InstId;
            IpcCmd.ApiId = pErrNotify->ApiId;
            IpcCmd.ErrorId = pErrNotify->ErrorId;
            IpcErr = AmbaIPC_Send(DspMonIpc, &IpcCmd, (INT32)sizeof(DSPMON_CMD_ERR_NOTIFY_s));
            if (IpcErr != 0) {
                AmbaPrint_PrintUInt5("[DspMon] IPCSend[0x%X] fail[0x%X]", IpcCmd.Code, (UINT32)IpcErr, 0U, 0U, 0U);
                Rval = DSPMON_ERR_0002;
            }
        }
    }
    return Rval;
}

#ifdef DSP_MONITOR_WITH_SHADOW
UINT32 DSPMon_GetAssertBufferAddr(ULONG *pU32Addr)
{
    ULONG ULAddr = ASSERT_SHADOW_BASE;

    AmbaMisra_TypeCast(pU32Addr, &ULAddr);
    return DSPMON_ERR_NONE;
}

UINT32 DspMon_SetYuvOutStrmSrc(UINT32 YuvOutIdx, const AMBA_DSP_MONITOR_YUV_SRC_INFO_s *pYuvSrcInfo)
{
    UINT32 Rval;

    Rval = AmbaIPC_MutexTake(AMBA_IPC_MUTEX_DSPMON, DSPMON_IPC_MTX_TIMEOUT);
    if (Rval == DSPMON_ERR_NONE) {
        if (YuvOutIdx < DSP_VPROC_PIN_NUM) {
            ULONG ULAddr = YUVOUTSRC_SHADOW_BASE;

            AmbaMisra_TypeCast(&pDspMon_YuvOutStrmSrc, &ULAddr);
            Rval = AmbaWrap_memcpy(&pDspMon_YuvOutStrmSrc[YuvOutIdx],
                                   pYuvSrcInfo,
                                   sizeof(AMBA_DSP_MONITOR_YUV_SRC_INFO_s));
            if (Rval != DSPMON_ERR_NONE) {
                AmbaPrint_PrintUInt5("[DspMon] IpcCmd memcpy fail[0x%X]", Rval, 0U, 0U, 0U, 0U);
                Rval = DSPMON_ERR_0002;
            } else {
                Rval = AmbaIPC_MutexGive(AMBA_IPC_MUTEX_DSPMON);
                if (Rval != DSPMON_ERR_NONE) {
                    AmbaPrint_PrintUInt5("[DspMon] AmbaIPC_MutexGive Fail[0x%X]", Rval, 0U, 0U, 0U, 0U);
                }
            }
        } else {
            Rval = DSPMON_ERR_0001;
        }
    } else {
        AmbaPrint_PrintUInt5("[DspMon] AmbaIPC_MutexTake Fail[0x%X]", Rval, 0U, 0U, 0U, 0U);
    }
    return Rval;
}

UINT32 DspMon_ResetYuvOutStrmSrc(void)
{
    UINT32 Rval;

    Rval = AmbaIPC_MutexTake(AMBA_IPC_MUTEX_DSPMON, DSPMON_IPC_MTX_TIMEOUT);
    if (Rval == DSPMON_ERR_NONE) {
        ULONG ULAddr = YUVOUTSRC_SHADOW_BASE;

        AmbaMisra_TypeCast(&pDspMon_YuvOutStrmSrc, &ULAddr);
        Rval = AmbaWrap_memset(pDspMon_YuvOutStrmSrc, 0, sizeof(AMBA_DSP_MONITOR_YUV_SRC_INFO_s)*DSP_VPROC_PIN_NUM);
        if (Rval != DSPMON_ERR_NONE) {
            AmbaPrint_PrintUInt5("[DspMon] pDspMon_YuvOutStrmSrcBitMask Fail[0x%X]", Rval, 0U, 0U, 0U, 0U);
        }
        AmbaMisra_TouchUnused(&Rval);
        Rval = AmbaIPC_MutexGive(AMBA_IPC_MUTEX_DSPMON);
        if (Rval != DSPMON_ERR_NONE) {
            AmbaPrint_PrintUInt5("[DspMon] AmbaIPC_MutexGive Fail[0x%X]", Rval, 0U, 0U, 0U, 0U);
        }
    } else {
        AmbaPrint_PrintUInt5("[DspMon] AmbaIPC_MutexTake Fail[0x%X]", Rval, 0U, 0U, 0U, 0U);
    }
    return Rval;
}

#else
UINT32 DspMon_SetYuvOutStrmSrc(UINT32 YuvOutIdx, const AMBA_DSP_MONITOR_YUV_SRC_INFO_s *pYuvSrcInfo)
{
    UINT32 Rval;
    if (YuvOutIdx < DSP_VPROC_PIN_NUM) {
        (void)AmbaWrap_memcpy(&pDspMon_YuvOutStrmSrc[YuvOutIdx],
                               pYuvSrcInfo,
                               sizeof(AMBA_DSP_MONITOR_YUV_SRC_INFO_s));
        Rval = DSPMON_ERR_NONE;
    } else {
        Rval = DSPMON_ERR_0001;
    }
    return Rval;
}

UINT32 DspMon_ResetYuvOutStrmSrc(void)
{
    UINT32 Rval;
    Rval = AmbaWrap_memset(pDspMon_YuvOutStrmSrc, 0, sizeof(AMBA_DSP_MONITOR_YUV_SRC_INFO_s)*DSP_VPROC_PIN_NUM);
    if (Rval != DSPMON_ERR_NONE) {
        AmbaPrint_PrintUInt5("[DspMon] pDspMon_YuvOutStrmSrcBitMask Fail[0x%X]", Rval, 0U, 0U, 0U, 0U);
    }
    return Rval;
}
#endif

