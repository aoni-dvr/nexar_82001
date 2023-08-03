/**
*  @file AmbaIP_Monitor.c
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
*  @details Amba IP monitor
*
*/
#include "AmbaTypes.h"
#include "Amba_IPMonitor_Def.h"
#include "Amba_IPMonitor.h"
#include "AmbaMisraFix.h"
#include "AmbaIPC.h"
#include "AmbaWrap.h"
#include "AmbaPrint.h"
#include "AmbaKAL.h"
#include "AmbaErrorCode.h"
#include "AmbaUtility.h"

#define CRC_CMPR_VIA_SHADOW

#ifdef CRC_CMPR_VIA_SHADOW
#include "AmbaSafety.h"
#endif

#ifdef CRC_CMPR_VIA_SHADOW
#define IPMONADJ_SHADOW_BASE              (SHADOWBASE_IPMONADJ)
#define IPMONADJ_SHADOW_SIZE              (0x4000U)
#define IPMONADJ_SHADOW_CRC_QUEUE_DEPTH   (512U)

#define IPMONAE_SHADOW_BASE               (SHADOWBASE_IPMONAE)
#define IPMONAE_SHADOW_SIZE               (0x4000U)
#define IPMONAE_SHADOW_QUEUE_DEPTH        (128U)

#define IPMONAWB_SHADOW_BASE              (SHADOWBASE_IPMONAWB)
#define IPMONAWB_SHADOW_SIZE              (0x4000U)
#define IPMONAWB_SHADOW_QUEUE_DEPTH       (256U)


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

    UINT32 AdjWP:16;
    UINT32 AdjRP:16;

    AMBA_IP_MONITOR_ADJ_CMPR_s AdjResult[IPMONADJ_SHADOW_CRC_QUEUE_DEPTH];
} IPMONADJ_SHADOW_s;

typedef struct {
    UINT32 A53State:2;
    UINT32 A53Rsvd:30;

    UINT32 R52State:2;
    UINT32 R52Rsvd:30;

    UINT32 BufState:2;
    UINT32 BufEmpty:1;
    UINT32 BufRsvd:29;

    UINT32 AwbWP:16;
    UINT32 AwbRP:16;

    AMBA_IP_MONITOR_AWB_CMPR_s AwbResult[IPMONAWB_SHADOW_QUEUE_DEPTH];
} IPMONAWB_SHADOW_s;

typedef struct {
    UINT32 A53State:2;
    UINT32 A53Rsvd:30;

    UINT32 R52State:2;
    UINT32 R52Rsvd:30;

    UINT32 BufState:2;
    UINT32 BufEmpty:1;
    UINT32 BufRsvd:29;

    UINT32 AeWP:16;
    UINT32 AeRP:16;

    AMBA_IP_MONITOR_AE_CMPR_s AeResult[IPMONAE_SHADOW_QUEUE_DEPTH];
} IPMONAE_SHADOW_s;



static IPMONADJ_SHADOW_s *pIpMonShadowADJ;
static IPMONAE_SHADOW_s *pIpMonShadowAE;
static IPMONAWB_SHADOW_s *pIpMonShadowAWB;

#define IPMON_IPC_MTX_TIMEOUT  (10U)
static inline UINT32 IpMon_IPCShadowLock(void)
{
    UINT32 Rval = IPMON_ERR_NONE;

    Rval = AmbaIPC_MutexTake(AMBA_IPC_MUTEX_IPMON, IPMON_IPC_MTX_TIMEOUT);
    if (Rval != IPMON_ERR_NONE) {
        AmbaPrint_PrintUInt5("[IpMon] MutexTake Fail[0x%X]", Rval, 0U, 0U, 0U, 0U);
    }
    return Rval;
}

static inline void IpMon_IPCShadowUnLock(void)
{
    UINT32 Rval = IPMON_ERR_NONE;

    Rval = AmbaIPC_MutexGive(AMBA_IPC_MUTEX_IPMON);
    if (Rval != IPMON_ERR_NONE) {
        AmbaPrint_PrintUInt5("[IpMon] MutexGive Fail[0x%X]", Rval, 0U, 0U, 0U, 0U);
    }
}
#endif

typedef struct {
    UINT32 IpcInited:1;
    UINT32 CfgMaskInited:1;
    UINT32 Rsvd:31;
} IP_MON_STAT_s;

static AMBA_IPC_HANDLE IpMonIpc GNU_SECTION_NOZEROINIT;

static UINT32 IpMonCfgMask[IP_MON_MODULE_MAX_NUM] GNU_SECTION_NOZEROINIT;

static inline UINT32 IpcCmdProcOnCfgBitMask(const IPMON_IPC_CMD_s *pIpcCmd)
{
    UINT32 Rval = IPMON_ERR_NONE;
    const IPMON_CMD_CFG_BIT_MASK_s *pCfgMask = NULL;
    AmbaPrint_PrintUInt5("IpcCmdProcOnCfgBitMask ModuledID:%X, BitMask:%X    [pIpcCmd]", pIpcCmd->Data[0], pIpcCmd->Data[1], 0U, 0U, 0U);
    if (pIpcCmd != NULL) {
        AmbaMisra_TypeCast(&pCfgMask, &pIpcCmd);
        if (pCfgMask->ModuleId == (IPMONADJ_ERR_BASE >> 16U)) {
            AmbaPrint_PrintUInt5("1,IpcCmdProcOnCfgBitMask ModuledID:%X, BitMask:%X    [pCfgMask]", pCfgMask->ModuleId, pCfgMask->BitMask, 0U, 0U, 0U);
            IpMonCfgMask[0U] = pCfgMask->BitMask;
        } else if (pCfgMask->ModuleId == (IPMONAE_ERR_BASE >> 16U)) {
            AmbaPrint_PrintUInt5("2,IpcCmdProcOnCfgBitMask ModuledID:%X, BitMask:%X    [pCfgMask]", pCfgMask->ModuleId, pCfgMask->BitMask, 0U, 0U, 0U);
            IpMonCfgMask[1U] = pCfgMask->BitMask;
        } else if (pCfgMask->ModuleId == (IPMONAWB_ERR_BASE >> 16U)) {
            AmbaPrint_PrintUInt5("3,IpcCmdProcOnCfgBitMask ModuledID:%X, BitMask:%X    [pCfgMask]", pCfgMask->ModuleId, pCfgMask->BitMask, 0U, 0U, 0U);
            IpMonCfgMask[2U] = pCfgMask->BitMask;
        } else {
            // DO NOTHING
        }
        AmbaPrint_PrintUInt5("IpcCmdProcOnCfgBitMask ModuledID:%X, BitMask:%X    [pCfgMask], %d,%d,%d", pCfgMask->ModuleId, pCfgMask->BitMask, IpMonCfgMask[0U], IpMonCfgMask[1U], IpMonCfgMask[2U]);
    }

    return Rval;
}

static inline UINT32 IpMon_IpcCmdProc(const IPMON_IPC_CMD_s *pIpcCmd)
{
    UINT32 Rval = IPMON_ERR_NONE;

    if (pIpcCmd != NULL) {
        switch (pIpcCmd->Code) {
        case IPMON_IPC_CMD_CFG_BIT_MASK:
        {
            Rval = IpcCmdProcOnCfgBitMask(pIpcCmd);
        }
            break;
        case IPMON_IPC_CMD_STATE_INITED:
        case IPMON_IPC_CMD_CRC_CMPR:
        case IPMON_IPC_CMD_ERR_NOTIFY:
        default :
        {
            AmbaPrint_PrintUInt5("[IpMon] Unknown IpcCmd[0x%X]", pIpcCmd->Code, 0U, 0U, 0U, 0U);
        }
            break;
        }
    } else {
        Rval = IPMON_ERR_0000;
    }

    return Rval;
}

static INT32 IpMonIPC_MsgHdlr(AMBA_IPC_HANDLE  IpcHandle, AMBA_IPC_MSG_CTRL_s *pMsgCtrl)
{
    UINT32 Rval = OK;
    IPMON_IPC_CMD_s IpcCmd = {0U};
    AmbaMisra_TouchUnused(&IpcHandle);
// (void)IpcHandle;
    AmbaMisra_TouchUnused(pMsgCtrl);

    if (sizeof(IPMON_IPC_CMD_s) >= pMsgCtrl->Length) {
        Rval = AmbaWrap_memcpy(&IpcCmd, pMsgCtrl->pMsgData, pMsgCtrl->Length);
        if (Rval != IPMON_ERR_NONE) {
            Rval = IPMON_ERR_0002;
            AmbaPrint_PrintUInt5("[IpMon] IpcCmd memcpy fail[0x%X]", Rval, 0U, 0U, 0U, 0U);
        } else {
            Rval = IpMon_IpcCmdProc(&IpcCmd);
        }
    } else {
        Rval = IPMON_ERR_0001;
        AmbaPrint_PrintUInt5("[IpMon] IpcCmd Size exceed Resv[%d] In[%d]", sizeof(IPMON_IPC_CMD_s), pMsgCtrl->Length, 0U, 0U, 0U);
    }

    return (INT32)Rval;
}

static inline UINT32 IpMon_InitIpc(void)
{
    UINT32 Rval = IPMON_ERR_NONE;
    // static const char IpMonName[] = "ipmon";
    INT32 IpcErr;
    IPMON_IPC_CMD_s IpcCmd;
    UINT32 IpcCmdSize;

    Rval = AmbaWrap_memset(&IpMonIpc, 0, sizeof(AMBA_IPC_HANDLE));
    if (Rval == IPMON_ERR_NONE) {
        IpMonIpc = AmbaIPC_Alloc(RPMSG_DEV_AMBA, "ipmon", IpMonIPC_MsgHdlr);
        if (IpMonIpc == NULL) {
            AmbaPrint_PrintStr5("AmbaIPC_Alloc[%s] failed", "ipmon", NULL, NULL, NULL, NULL);
        } else {
            IpcErr = AmbaIPC_RegisterChannel(IpMonIpc, NULL);
            if (IpcErr != 0) {
                AmbaPrint_PrintUInt5("AmbaIPC_RegisterChannel fail[0x%X]", (UINT32)IpcErr, 0U, 0U, 0U, 0U);
                Rval = IPMON_ERR_0002;
            } else {
                Rval = AmbaWrap_memset(&IpcCmd, 0, sizeof(IPMON_IPC_CMD_s));
                if (Rval == IPMON_ERR_NONE) {
                    IpcCmdSize = 0U;
                    IpcCmd.Code = IPMON_IPC_CMD_STATE_INITED;
                    IpcCmdSize += sizeof(IpcCmd.Code);
                    IpcErr = AmbaIPC_Send(IpMonIpc, &IpcCmd, (INT32)IpcCmdSize);
                    if (IpcErr != 0) {
                        AmbaPrint_PrintUInt5("[IpMon] IPCSend[0x%X] fail[0x%X]", IpcCmd.Code, (UINT32)IpcErr, 0U, 0U, 0U);
                        Rval = IPMON_ERR_0002;
                    }
                }
            }
        }
    }

    return Rval;
}

#ifdef CRC_CMPR_VIA_SHADOW
static inline UINT32 IpMon_InitShdow(void)
{
    UINT32 Rval = IPMON_ERR_NONE;
    UINT32 Addr = IPMONADJ_SHADOW_BASE;
    UINT32 AddrAE = IPMONAE_SHADOW_BASE;
    UINT32 AddrAWB = IPMONAWB_SHADOW_BASE;
    Rval= IpMon_IPCShadowLock();
    if (Rval == IPMON_ERR_NONE) {
        AmbaMisra_TypeCast(&pIpMonShadowADJ, &Addr);
        if (pIpMonShadowADJ->BufState != SHADOW_STATE_INITED) {
            pIpMonShadowADJ->AdjWP = (UINT16)(IPMONADJ_SHADOW_CRC_QUEUE_DEPTH - 1U);
            pIpMonShadowADJ->AdjRP = (UINT16)(IPMONADJ_SHADOW_CRC_QUEUE_DEPTH - 1U);
            pIpMonShadowADJ->BufEmpty = (UINT8)0x1U;
            pIpMonShadowADJ->BufState = SHADOW_STATE_INITED;
            //AmbaPrint_PrintUInt5("[IpMon] IpMonShadow BufState Init", 0U, 0U, 0U, 0U, 0U);
        }
        pIpMonShadowADJ->A53State = SHADOW_STATE_INITED;

        AmbaMisra_TypeCast(&pIpMonShadowAE, &AddrAE);
        if (pIpMonShadowAE->BufState != SHADOW_STATE_INITED) {
            pIpMonShadowAE->AeWP = ((UINT16)IPMONAE_SHADOW_QUEUE_DEPTH - (UINT16)1);
            pIpMonShadowAE->AeRP = ((UINT16)IPMONAE_SHADOW_QUEUE_DEPTH - (UINT16)1);
            pIpMonShadowAE->BufEmpty = (UINT8)0x1U;
            pIpMonShadowAE->BufState = SHADOW_STATE_INITED;
            //AmbaPrint_PrintUInt5("[IpMon] IpMonShadow BufState Init", 0U, 0U, 0U, 0U, 0U);
        }
        pIpMonShadowAE->A53State = SHADOW_STATE_INITED;

        AmbaMisra_TypeCast(&pIpMonShadowAWB, &AddrAWB);
        if (pIpMonShadowAWB->BufState != SHADOW_STATE_INITED) {
            pIpMonShadowAWB->AwbWP = (UINT16)(IPMONAWB_SHADOW_QUEUE_DEPTH - 1U);
            pIpMonShadowAWB->AwbRP = (UINT16)(IPMONAWB_SHADOW_QUEUE_DEPTH - 1U);
            pIpMonShadowAWB->BufEmpty = (UINT8)0x1U;
            pIpMonShadowAWB->BufState = SHADOW_STATE_INITED;
            //AmbaPrint_PrintUInt5("[IpMon] IpMonShadow BufState Init", 0U, 0U, 0U, 0U, 0U);
        }
        pIpMonShadowAWB->A53State = SHADOW_STATE_INITED;


        IpMon_IPCShadowUnLock();
        // AmbaPrint_PrintUInt5("[IpMon] IpMonShadow buffer Req[%d] Resv[%d] %d", sizeof(IPMONADJ_SHADOW_s), IPMONADJ_SHADOW_SIZE, sizeof(AMBA_IP_MONITOR_ADJ_CMPR_s), 0U, 0U);
        // AmbaPrint_PrintUInt5("[IpMonAE]  IpMonAEShadow  buffer Req[%d] Resv[%d] %d", sizeof(IPMONADJ_SHADOW_s), IPMONAE_SHADOW_SIZE, sizeof(AMBA_IP_MONITOR_ADJ_CMPR_s), 0U, 0U);
        // AmbaPrint_PrintUInt5("[IpMonAWB] IpMonAWBShadow buffer Req[%d] Resv[%d] %d", sizeof(IPMONADJ_SHADOW_s), IPMONAWB_SHADOW_SIZE, sizeof(AMBA_IP_MONITOR_ADJ_CMPR_s), 0U, 0U);


    }

    return Rval;
}
#endif

UINT32 AmbaIP_MonitorInit(void)
{
    static IP_MON_STAT_s IpMonStat = {0};
    UINT32 Rval = IPMON_ERR_NONE;

    if (IpMonStat.IpcInited == (UINT8)0U) {
        Rval = IpMon_InitIpc();
        if (Rval == IPMON_ERR_NONE) {
            IpMonStat.IpcInited = (UINT8)1U;
        }

        if (Rval == IPMON_ERR_NONE) {
            if (IpMonStat.CfgMaskInited == (UINT8)0U) {
                Rval = AmbaWrap_memset(&IpMonCfgMask, 0, sizeof(UINT32)*IP_MON_MODULE_MAX_NUM);
                if (Rval == IPMON_ERR_NONE) {
                    IpMonStat.CfgMaskInited = (UINT8)1U;
                }
            }
        }

#ifdef CRC_CMPR_VIA_SHADOW
        if (Rval == IPMON_ERR_NONE) {
            Rval = IpMon_InitShdow();
        }
#endif
    }

    return Rval;
}

UINT32 AmbaIP_GetMonitorCfg(UINT32 ModuleId, AMBA_IP_MONITOR_CONFIG_s *pMonCfg)
{
    UINT32 Rval = IPMON_ERR_NONE;

    if (pMonCfg == NULL) {
        Rval = IPMON_ERR_0000;
    } else {
        if (ModuleId == (IPMONADJ_ERR_BASE >> 16U)) {
            pMonCfg->BitMask = IpMonCfgMask[0U];
        } else if (ModuleId == (IPMONAE_ERR_BASE >> 16U)) {
            pMonCfg->BitMask = IpMonCfgMask[1U];
        } else if (ModuleId == (IPMONAWB_ERR_BASE >> 16U)) {
            pMonCfg->BitMask = IpMonCfgMask[2U];
        } else {
            Rval = IPMON_ERR_0001;
        }
    }

    return Rval;
}

//#define DBG_SHADOW_LOCK
static inline UINT32 IpMon_CrcWrite(const AMBA_IP_MONITOR_ADJ_CMPR_s *pAdjCmpr)
{
    UINT32 Rval = IPMON_ERR_NONE;
#ifdef CRC_CMPR_VIA_SHADOW
    UINT16 NextWp;
#ifdef DBG_SHADOW_LOCK
    UINT32 TimeS, TimeE, TimeSpend;
#endif

#ifdef DBG_SHADOW_LOCK
    Rval = AmbaKAL_GetSysTickCount(&TimeS); AmbaMisra_TouchUnused(&Rval);
#endif

    Rval = IpMon_IPCShadowLock();
    if (Rval == IPMON_ERR_NONE) {
        /* Check */
        if (pIpMonShadowADJ->A53State == SHADOW_STATE_INITED) {
            pIpMonShadowADJ->A53State = SHADOW_STATE_READY;
        }

        if ((pIpMonShadowADJ->A53State != SHADOW_STATE_READY) ||
            (pIpMonShadowADJ->BufState != SHADOW_STATE_INITED) ||
            (pIpMonShadowADJ->R52State != SHADOW_STATE_READY)) {
            Rval = IPMON_ERR_0003;
        } else {
            NextWp = (pIpMonShadowADJ->AdjWP + 1U) % IPMONADJ_SHADOW_CRC_QUEUE_DEPTH;

            if (pIpMonShadowADJ->BufEmpty != (UINT8)0x1U) {
                if (pIpMonShadowADJ->AdjWP == pIpMonShadowADJ->AdjRP) {
                    Rval = IPMON_ERR_0002;
                    AmbaPrint_PrintUInt5("[IpMon] CrcQ Full, Sz[%d]", IPMONADJ_SHADOW_CRC_QUEUE_DEPTH, 0U, 0U, 0U, 0U);
                }
            }

            if (Rval == IPMON_ERR_NONE) {
                pIpMonShadowADJ->AdjResult[NextWp].ModuleId = pAdjCmpr->ModuleId;
                pIpMonShadowADJ->AdjResult[NextWp].Result = pAdjCmpr->Result;
                pIpMonShadowADJ->AdjResult[NextWp].UUID = pAdjCmpr->UUID;
                pIpMonShadowADJ->AdjWP = NextWp;
                pIpMonShadowADJ->BufEmpty = (UINT8)0x0U;
            }
        }
        IpMon_IPCShadowUnLock();
    }
#ifdef DBG_SHADOW_LOCK
    Rval = AmbaKAL_GetSysTickCount(&TimeE); AmbaMisra_TouchUnused(&Rval);
    if (TimeE >= TimeS) {
        TimeSpend = TimeE - TimeS;
    } else {
        TimeSpend = ((0xFFFFFFFFU - TimeS) + 1U) + TimeE;
    }
    AmbaPrint_PrintUInt5("[IpMon] TimeSpend[%d]", TimeSpend, 0U, 0U, 0U, 0U);
#endif

#else
    INT32 IpcErr;
    IPMON_CMD_CRC_CMPR_s IpcCmd = {0U};

    Rval = AmbaWrap_memset(&IpcCmd, 0, sizeof(IPMON_CMD_CRC_CMPR_s));
    if (Rval == IPMON_ERR_NONE) {
        IpcCmd.Code = IPMON_IPC_CMD_CRC_CMPR;
        IpcCmd.Crc0 = pAdjCmpr->Crc0;
        IpcCmd.Crc1 = pAdjCmpr->Crc1;
        IpcCmd.UUID = pAdjCmpr->UUID;
        IpcErr = AmbaIPC_Send(IpMonIpc, &IpcCmd, (INT32)sizeof(IPMON_CMD_CRC_CMPR_s));
        if (IpcErr != 0) {
            AmbaPrint_PrintUInt5("[IpMon] IPCSend[0x%X] fail[0x%X]", IpcCmd.Code, (UINT32)IpcErr, 0U, 0U, 0U);
            Rval = IPMON_ERR_0002;
        }
    }
#endif

    return Rval;
}

static inline UINT32 IpMon_AeWrite(const AMBA_IP_MONITOR_AE_CMPR_s *pAeCmpr)
{
    UINT32 Rval = IPMON_ERR_NONE;
#ifdef CRC_CMPR_VIA_SHADOW
    UINT16 NextWp;
#ifdef DBG_SHADOW_LOCK
    UINT32 TimeS, TimeE, TimeSpend;
#endif

#ifdef DBG_SHADOW_LOCK
    Rval = AmbaKAL_GetSysTickCount(&TimeS); AmbaMisra_TouchUnused(&Rval);
#endif

    Rval = IpMon_IPCShadowLock();
    if (Rval == IPMON_ERR_NONE) {
        /* Check */
        if (pIpMonShadowAE->A53State == SHADOW_STATE_INITED) {
            pIpMonShadowAE->A53State = SHADOW_STATE_READY;
        }

        if ((pIpMonShadowAE->A53State != SHADOW_STATE_READY) ||
            (pIpMonShadowAE->BufState != SHADOW_STATE_INITED) ||
            (pIpMonShadowAE->R52State != SHADOW_STATE_READY)) {
            Rval = IPMON_ERR_0003;
        } else {
            NextWp = (pIpMonShadowAE->AeWP + 1U) % IPMONAE_SHADOW_QUEUE_DEPTH;

            if (pIpMonShadowAE->BufEmpty != (UINT8)0x1U) {
                if (pIpMonShadowAE->AeWP == pIpMonShadowAE->AeRP) {
                    Rval = IPMON_ERR_0002;
                    AmbaPrint_PrintUInt5("[IpMon] CrcQ Full, Sz[%d]", IPMONAE_SHADOW_QUEUE_DEPTH, 0U, 0U, 0U, 0U);
                }
            }

            if (Rval == IPMON_ERR_NONE) {
                pIpMonShadowAE->AeResult[NextWp].ModuleId = pAeCmpr->ModuleId;
                pIpMonShadowAE->AeResult[NextWp].Result = pAeCmpr->Result;
                pIpMonShadowAE->AeResult[NextWp].UUID = pAeCmpr->UUID;
                pIpMonShadowAE->AeWP = NextWp;
                pIpMonShadowAE->BufEmpty = (UINT8)0x0U;
            }
        }
        IpMon_IPCShadowUnLock();
    }
#ifdef DBG_SHADOW_LOCK
    Rval = AmbaKAL_GetSysTickCount(&TimeE); AmbaMisra_TouchUnused(&Rval);
    if (TimeE >= TimeS) {
        TimeSpend = TimeE - TimeS;
    } else {
        TimeSpend = ((0xFFFFFFFFU - TimeS) + 1U) + TimeE;
    }
    AmbaPrint_PrintUInt5("[IpMon] TimeSpend[%d]", TimeSpend, 0U, 0U, 0U, 0U);
#endif

#else
    INT32 IpcErr;
    IPMON_CMD_CRC_CMPR_s IpcCmd = {0U};

    Rval = AmbaWrap_memset(&IpcCmd, 0, sizeof(IPMON_CMD_CRC_CMPR_s));
    if (Rval == IPMON_ERR_NONE) {
        IpcCmd.Code = IPMON_IPC_CMD_CRC_CMPR;
        IpcCmd.Crc0 = pCrcCmpr->Crc0;
        IpcCmd.Crc1 = pCrcCmpr->Crc1;
        IpcCmd.UUID = pCrcCmpr->UUID;
        IpcErr = AmbaIPC_Send(IpMonIpc, &IpcCmd, (INT32)sizeof(IPMON_CMD_CRC_CMPR_s));
        if (IpcErr != 0) {
            AmbaPrint_PrintUInt5("[IpMon] IPCSend[0x%X] fail[0x%X]", IpcCmd.Code, (UINT32)IpcErr, 0U, 0U, 0U);
            Rval = IPMON_ERR_0002;
        }
    }
#endif

    return Rval;
}

static inline UINT32 IpMon_AwbWrite(const AMBA_IP_MONITOR_AWB_CMPR_s *pAwbCmpr)
{
    UINT32 Rval = IPMON_ERR_NONE;
#ifdef CRC_CMPR_VIA_SHADOW
    UINT16 NextWp;
#ifdef DBG_SHADOW_LOCK
    UINT32 TimeS, TimeE, TimeSpend;
#endif

#ifdef DBG_SHADOW_LOCK
    Rval = AmbaKAL_GetSysTickCount(&TimeS); AmbaMisra_TouchUnused(&Rval);
#endif

    Rval = IpMon_IPCShadowLock();
    if (Rval == IPMON_ERR_NONE) {
        /* Check */
        if (pIpMonShadowAWB->A53State == SHADOW_STATE_INITED) {
            pIpMonShadowAWB->A53State = SHADOW_STATE_READY;
        }

        if ((pIpMonShadowAWB->A53State != SHADOW_STATE_READY) ||
            (pIpMonShadowAWB->BufState != SHADOW_STATE_INITED) ||
            (pIpMonShadowAWB->R52State != SHADOW_STATE_READY)) {
            Rval = IPMON_ERR_0003;
        } else {
            NextWp = (pIpMonShadowAWB->AwbWP + 1U) % IPMONAWB_SHADOW_QUEUE_DEPTH;

            if (pIpMonShadowAWB->BufEmpty != (UINT8)0x1U) {
                if (pIpMonShadowAWB->AwbWP == pIpMonShadowAWB->AwbRP) {
                    Rval = IPMON_ERR_0002;
                    AmbaPrint_PrintUInt5("[IpMon] CrcQ Full, Sz[%d]", IPMONAWB_SHADOW_QUEUE_DEPTH, 0U, 0U, 0U, 0U);
                }
            }

            if (Rval == IPMON_ERR_NONE) {
                pIpMonShadowAWB->AwbResult[NextWp].ModuleId = pAwbCmpr->ModuleId;
                pIpMonShadowAWB->AwbResult[NextWp].Result = pAwbCmpr->Result;
                pIpMonShadowAWB->AwbResult[NextWp].UUID = pAwbCmpr->UUID;
                pIpMonShadowAWB->AwbWP = NextWp;
                pIpMonShadowAWB->BufEmpty = (UINT8)0x0U;
            }
        }
        IpMon_IPCShadowUnLock();
    }
#ifdef DBG_SHADOW_LOCK
    Rval = AmbaKAL_GetSysTickCount(&TimeE); AmbaMisra_TouchUnused(&Rval);
    if (TimeE >= TimeS) {
        TimeSpend = TimeE - TimeS;
    } else {
        TimeSpend = ((0xFFFFFFFFU - TimeS) + 1U) + TimeE;
    }
    AmbaPrint_PrintUInt5("[IpMon] TimeSpend[%d]", TimeSpend, 0U, 0U, 0U, 0U);
#endif

#else
    INT32 IpcErr;
    IPMON_CMD_CRC_CMPR_s IpcCmd = {0U};

    Rval = AmbaWrap_memset(&IpcCmd, 0, sizeof(IPMON_CMD_CRC_CMPR_s));
    if (Rval == IPMON_ERR_NONE) {
        IpcCmd.Code = IPMON_IPC_CMD_CRC_CMPR;
        IpcCmd.Crc0 = pCrcCmpr->Crc0;
        IpcCmd.Crc1 = pCrcCmpr->Crc1;
        IpcCmd.UUID = pCrcCmpr->UUID;
        IpcErr = AmbaIPC_Send(IpMonIpc, &IpcCmd, (INT32)sizeof(IPMON_CMD_CRC_CMPR_s));
        if (IpcErr != 0) {
            AmbaPrint_PrintUInt5("[IpMon] IPCSend[0x%X] fail[0x%X]", IpcCmd.Code, (UINT32)IpcErr, 0U, 0U, 0U);
            Rval = IPMON_ERR_0002;
        }
    }
#endif

    return Rval;
}

UINT32 AmbaIP_MonitorAdjCmpr(const AMBA_IP_MONITOR_ADJ_CMPR_s *pAdjCmpr)
{
    UINT32 Rval = IPMON_ERR_NONE;

    if (pAdjCmpr == NULL) {
        Rval = IPMON_ERR_0000;
    } else {
        Rval = IpMon_CrcWrite(pAdjCmpr);
    }

    return Rval;
}

UINT32 AmbaIP_MonitorAeCmpr(const AMBA_IP_MONITOR_AE_CMPR_s *pAeCmpr)
{
    UINT32 Rval = IPMON_ERR_NONE;

    if (pAeCmpr == NULL) {
        Rval = IPMON_ERR_0000;
    } else {
        Rval = IpMon_AeWrite(pAeCmpr);
    }

    return Rval;
}

UINT32 AmbaIP_MonitorAwbCmpr(const AMBA_IP_MONITOR_AWB_CMPR_s *pAwbCmpr)
{
    UINT32 Rval = IPMON_ERR_NONE;

    if (pAwbCmpr == NULL) {
        Rval = IPMON_ERR_0000;
    } else {
        Rval = IpMon_AwbWrite(pAwbCmpr);
    }

    return Rval;
}


UINT32 AmbaIP_MonitorErrorNotify(const AMBA_IP_MONITOR_ERR_NOTIFY_s *pErrNotify)
{
    UINT32 Rval = IPMON_ERR_NONE;
    INT32 IpcErr;
    IPMON_CMD_ERR_NOTIFY_s IpcCmd = {0U};

    if (pErrNotify == NULL) {
        Rval = IPMON_ERR_0000;
    } else {
        Rval = AmbaWrap_memset(&IpcCmd, 0, sizeof(IPMON_CMD_ERR_NOTIFY_s));
        if (Rval == IPMON_ERR_NONE) {
            IpcCmd.Code = IPMON_IPC_CMD_ERR_NOTIFY;
            IpcCmd.ModuleId = pErrNotify->ModuleId;
            IpcCmd.InstId = pErrNotify->InstId;
            IpcCmd.ApiId = pErrNotify->ApiId;
            IpcCmd.ErrorId = pErrNotify->ErrorId;
            IpcErr = AmbaIPC_Send(IpMonIpc, &IpcCmd, (INT32)sizeof(IPMON_CMD_ERR_NOTIFY_s));
            if (IpcErr != 0) {
                AmbaPrint_PrintUInt5("[IpMon] IPCSend[0x%X] fail[0x%X]", IpcCmd.Code, (UINT32)IpcErr, 0U, 0U, 0U);
                Rval = IPMON_ERR_0002;
            }
        }
    }

    return Rval;
}

UINT32 Ipc_Test_Command(INT32 argc, char *const*argv)
{
    UINT32 rval = 0xFFFF;
    UINT32 RValU32 = 0U;

    AmbaPrint_PrintUInt5("Ipc_Test_Command", 0U, 0U, 0U, 0U, 0U);
    if(AmbaUtility_StringCompare(argv[2], "init", 4U) == 0) {
        RValU32 = AmbaIP_MonitorInit();
        if (RValU32 != 0U) {
            AmbaPrint_PrintUInt5("AmbaIP_MonitorInit fail[0x%X]", RValU32, 0U, 0U, 0U, 0U);
            AmbaPrint_Flush();
        }
    } else if(AmbaUtility_StringCompare(argv[2], "adj", 3U) == 0) {
        if (argc == 6) {
            UINT32 Temp;
            UINT32 UUID = 0U;
            AMBA_IP_MONITOR_ADJ_CMPR_s AdjCmpr = {0};
            (void)AmbaUtility_StringToUInt32(argv[3], &AdjCmpr.ModuleId);
            (void)AmbaUtility_StringToUInt32(argv[4], &Temp);
            (void)AmbaUtility_StringToUInt32(argv[5], &UUID);

            AdjCmpr.ModuleId = IPMONADJ_ERR_BASE;
            AdjCmpr.Result = (INT32)Temp;
            AdjCmpr.UUID = UUID;
            (void)AmbaIP_MonitorAdjCmpr(&AdjCmpr);
        }
        rval = 0;
    } else if(AmbaUtility_StringCompare(argv[2], "ae", 2U) == 0) {
        if (argc == 6) {
            UINT32 Temp;
            UINT32 UUID = 0U;
            AMBA_IP_MONITOR_AE_CMPR_s AeCmpr = {0};
            (void)AmbaUtility_StringToUInt32(argv[3], &AeCmpr.ModuleId);
            (void)AmbaUtility_StringToUInt32(argv[4], &Temp);
            (void)AmbaUtility_StringToUInt32(argv[5], &UUID);

            AeCmpr.ModuleId = IPMONAE_ERR_BASE;
            AeCmpr.Result = (INT32)Temp;
            AeCmpr.UUID = UUID;
            (void)AmbaIP_MonitorAeCmpr(&AeCmpr);
        }
        rval = 0;
    } else if(AmbaUtility_StringCompare(argv[2], "awb", 3U) == 0) {
        if (argc == 6) {
            UINT32 Temp;
            UINT32 UUID = 0U;
            AMBA_IP_MONITOR_AWB_CMPR_s AwbCmpr = {0};
            (void)AmbaUtility_StringToUInt32(argv[3], &AwbCmpr.ModuleId);
            (void)AmbaUtility_StringToUInt32(argv[4], &Temp);
            (void)AmbaUtility_StringToUInt32(argv[5], &UUID);

            AwbCmpr.ModuleId = IPMONAWB_ERR_BASE;

            AwbCmpr.UUID = UUID;
            AwbCmpr.Result = (INT32)Temp;
            (void)AmbaIP_MonitorAwbCmpr(&AwbCmpr);
        }
        rval = 0;
    // } else if(AmbaUtility_StringCompare(argv[2], "getconfig", 3U) == 0) {
    //   extern UINT32 AmbaIP_GetMonitorConfig(UINT32 ModuleId, AMBA_IP_MONITOR_CONFIG_s *pMonCfg);
    //   AMBA_IP_MONITOR_CONFIG_s MonCfg = {0};
    //   RValU32 |= AmbaIP_GetMonitorConfig((IPMONADJ_ERR_BASE>>16U), &MonCfg);
    //   AmbaPrint_PrintUInt5("ADJ MODULE:%X, BITMASK:%d", IPMONADJ_ERR_BASE, MonCfg.BitMask, 0U, 0U, 0U);

    //   RValU32 |= AmbaIP_GetMonitorConfig((IPMONAE_ERR_BASE>>16U), &MonCfg);
    //   AmbaPrint_PrintUInt5("AE MODULE:%X, BITMASK:%d", IPMONAE_ERR_BASE, MonCfg.BitMask, 0U, 0U, 0U);

    //   RValU32 |= AmbaIP_GetMonitorConfig((IPMONAWB_ERR_BASE>>16U), &MonCfg);
    //   AmbaPrint_PrintUInt5("AWB MODULE:%X, BITMASK:%d", IPMONAWB_ERR_BASE, MonCfg.BitMask, 0U, 0U, 0U);
    //   if(RValU32 != 0U){
    //       AmbaPrint_PrintUInt5("\timgproc -ipc getconfig ERROR XXXXXXXXX  %d", RValU32, 0U, 0U, 0U, 0U);
    //   }

    } else if(AmbaUtility_StringCompare(argv[2], "ap", 2U) == 0) {
        AmbaPrint_PrintUInt5( "\n", 0U, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("\timgproc -ipc init", 0U, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("\timgproc -ipc crc ModuleID CRC0 CRC1 UUID", 0U, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("\timgproc -ipc ae  ModuleID DGain0 ShutterTime0(*10000000) AgcGain0(*10000000) DGain1 ShutterTime1(*10000000) AgcGain1(*10000000) UUID", 0U, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("\timgproc -ipc awb ModuleID GainR0 GainG0 GainB0 GainR1 GainG1 GainB1 UUID", 0U, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("\timgproc -ipc getconfig", 0U, 0U, 0U, 0U, 0U);

        AmbaPrint_PrintUInt5( "\n", 0U, 0U, 0U, 0U, 0U);
        rval = 0;
    }else{
        //misrac
    }



    if(RValU32==0U){
        //
    }

    return rval;
}