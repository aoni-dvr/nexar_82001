/**
 *  @file SvcRtspServer.c
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
 *  @details rtsp server module
 *
 */

#include "AmbaTypes.h"
#include <AmbaKAL.h>
#include "AmbaPrint.h"
#include "NetStack.h"
#include "AmbaSvcWrap.h"

#if defined(CONFIG_NETX_ENET)
#include "NetXStack.h"
//#include <nx_api.h>
#include <nx_bsd.h>
#elif defined(CONFIG_LWIP_ENET)
#define LWIP_PATCH_MISRA   // for misra depress
#include "lwip/sockets.h"
#endif
#include "SvcRtspServer.h"
#include "SvcStreamMgr.h"
#include "AmbaUtility.h"
#include "SvcMediaSubsession.h"
#include "AmbaWrap.h"
#ifndef ERROR
#define ERROR   -1
#endif

static SVC_RTSP_SERVER_s* RtspServerPtr = NULL;
static AMBA_KAL_MUTEX_t    RTSPServerMemoryPoolMutex;

#define MEM_POOL_SDP_LINE_BUF_NUM   8
#define MEM_POOL_SDP_LINE_BUF_SIZE  0x400
static UINT8 MemoryPoolSdpLinePtr[MEM_POOL_SDP_LINE_BUF_NUM*MEM_POOL_SDP_LINE_BUF_SIZE];
static UINT8 MemoryPoolSdpLineIndex[MEM_POOL_SDP_LINE_BUF_NUM];

#define MEM_POOL_VPS_SPS_PPS_BUF_NUM   8
#define MEM_POOL_VPS_SPS_PPS_BUF_SIZE  0x40
static UINT8 MemoryPoolVpsSpsPpsPtr[MEM_POOL_VPS_SPS_PPS_BUF_NUM*MEM_POOL_VPS_SPS_PPS_BUF_SIZE];
static UINT8 MemoryPoolVpsSpsPpsIndex[MEM_POOL_VPS_SPS_PPS_BUF_NUM];

#define MEM_POOL_RESPONSE_BUF_NUM   2U
#define MEM_POOL_RESPONSE_BUF_SIZE  0x800U
static UINT8 MemoryPoolResponsePtr[MEM_POOL_RESPONSE_BUF_NUM*MEM_POOL_RESPONSE_BUF_SIZE];
static UINT8 MemoryPoolResponseIndex[MEM_POOL_RESPONSE_BUF_NUM];

static SVC_RTSP_SERVER_MEM_POOL_s RTSPServerMemPool[3] = {
    [SVC_RTSP_POOL_SDP_LINE_BUF] = {
        .Number     = MEM_POOL_SDP_LINE_BUF_NUM,
        .Size       = MEM_POOL_SDP_LINE_BUF_SIZE,
        .pPtr       = &MemoryPoolSdpLinePtr[0],
        .pIndex     = &MemoryPoolSdpLineIndex[0],
    },
    [SVC_RTSP_POOL_VPS_SPS_PPS_BUF] = {
        .Number     = MEM_POOL_VPS_SPS_PPS_BUF_NUM,
        .Size       = MEM_POOL_VPS_SPS_PPS_BUF_SIZE,
        .pPtr       = &MemoryPoolVpsSpsPpsPtr[0],
        .pIndex     = &MemoryPoolVpsSpsPpsIndex[0],
    },
    [SVC_RTSP_POOL_RESPONSE_BUF] = {
        .Number     = MEM_POOL_RESPONSE_BUF_NUM,
        .Size       = MEM_POOL_RESPONSE_BUF_SIZE,
        .pPtr       = &MemoryPoolResponsePtr[0],
        .pIndex     = &MemoryPoolResponseIndex[0],
    },
};

ULONG ALIGN_32(ULONG x)
{
    return (((x) + 31U) & ~31UL);
}

static SVC_RTSP_CLNTSESSION_s* LookupClientBySd(const SVC_RTSP_SERVER_s* pSrv, INT32 Sd)
{
    UINT32 i;
    SVC_RTSP_CLNTSESSION_s* ClntList = NULL;

    for (i = 0; i < pSrv->MaxClient ; i++) {
        if (Sd == pSrv->ClntList[i].Sd) {
            ClntList = &pSrv->ClntList[i];
        }
    }
    return ClntList;
}

static INT32 LookUpWatchTable(const SVC_RTSP_SERVER_s* pSrv, INT32 Sd)
{
    UINT32 i;
    INT32 Rval = -1;

    for (i = 0U; i < SVC_RTSP_MAX_IO_WATCHER; i++) {
        if (Sd == pSrv->WatchTable[i].Sd) {
            Rval = (INT32)i;
        }
    }
    return Rval;
}

static inline UINT32 TimeTickDiff(UINT32 Current, UINT32 Last)
{
    UINT32 Diff;
    if (Last > Current) {
        Diff = Current + (0xffffffffu - Last) + 1U;
    } else {
        Diff = Current - Last;
    }
    return Diff;
}

static void CheckClientTimeOut(const SVC_RTSP_SERVER_s* pSrv)
{
    UINT32 i, CurrentTime, LastTime, Diff, SysTickCnt;
    UINT32 Rval;

    (void)AmbaKAL_GetSysTickCount(&SysTickCnt);
    CurrentTime = SysTickCnt;
    for (i = 0U; i < pSrv->MaxClient ; i++) {
        if (pSrv->ClntList[i].Sd == -1) {
            continue;
        }
        LastTime = RtspClnt_GetLstActTime(&pSrv->ClntList[i]);
        Diff = TimeTickDiff(CurrentTime, LastTime);
        if (Diff > SVC_RTSP_TIMEOUT_MS) {
            AmbaPrint_PrintUInt5("[RTSP Server] client timeout, disconnect it", \
                            0U, 0U, 0U, 0U, 0U);
            Rval = RtspClnt_Release(&pSrv->ClntList[i]);
            if (Rval != OK) {
                AmbaPrint_PrintUInt5("[RTSP Server] release failed", \
                                0U, 0U, 0U, 0U, 0U);
            }
        }
    }
}

static void* AmbaRTSPServerEntry(void* Input)
{
    INT32               Status,ServSd;
    INT32               Sdi;
    struct sockaddr_in  ServAddr;/* Echo server address */
    struct sockaddr_in  ClientAddr = {0U};
    struct timeval      Timeout;
    UINT32              Clientlen;
#if defined(CONFIG_NETX_ENET)
    INT32               IntClientLen;
#else
    UINT32              IntClientLen;
#endif
    INT32               Index;
    UINT32              MonitorPeriodStart, SysTickCnt;  /* To make sure we scan all client within every RTSP TIMEOUT period */
    UINT32              Rval, Err, ActFlag;
    struct sockaddr *   pSockAddr;
    struct sockaddr_in const *pServAddr;
    struct sockaddr_in const *pClientAddr;
    UINT32              KeepLoop = 1U;

    SVC_RTSP_SERVER_s* pSrv;

    AmbaMisra_TouchUnused(Input);
    AmbaMisra_TypeCast(&pSrv, &Input);

    /* Create BSD TCP Server Socket */
    ServSd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ServSd == -1) {
        AmbaPrint_PrintUInt5("AmbaRTSPServerEntry: RTSP Server socket create fail", \
                        0U, 0U, 0U, 0U, 0U);
        Rval = AmbaKAL_EventFlagSet(&(pSrv->EventFlag), RTSP_SERVER_FLAG_SOCKET_FAIL);
        if (Rval != KAL_ERR_NONE) {
            AmbaPrint_PrintUInt5("AmbaKAL_EventFlagSet Error: RTSP_SERVER_FLAG_SOCKET_FAIL", \
                            0U, 0U, 0U, 0U, 0U);
        }
    } else {
        AmbaPrint_PrintUInt5("[RTSP Server] Socket created", \
                        0U, 0U, 0U, 0U, 0U);
        /* Now fill server side information */
        pSrv->Port = SVC_RTSP_SERVER_DEFAULT_PORT;
        if (AmbaWrap_memset(&ServAddr, 0, sizeof(ServAddr))!= 0U) { }

        ServAddr.sin_family = PF_INET;
        ServAddr.sin_addr.s_addr = 0;
    #if defined(CONFIG_NETX_ENET)
        ServAddr.sin_port = (UINT16)pSrv->Port;
    #else
        ServAddr.sin_port = htons((UINT16)pSrv->Port);
    #endif

        Clientlen = sizeof(struct sockaddr);
        /* Bind this server socket */
        pServAddr = &ServAddr;
        AmbaMisra_TypeCast(&pSockAddr, &pServAddr);
        Status = bind(ServSd, pSockAddr, (INT32)sizeof(ServAddr));
        if (Status < 0) {
            AmbaPrint_PrintUInt5("Error: RTSP Server Socket Bind \n", \
                           0U, 0U, 0U, 0U, 0U);
            (void)AmbaKAL_EventFlagSet(&(pSrv->EventFlag), RTSP_SERVER_FLAG_SOCKET_FAIL);
        } else {
            FD_ZERO(&pSrv->MasterList);
            FD_ZERO(&pSrv->ReadReady);
            FD_SET(ServSd,&pSrv->MasterList);
            pSrv->MaxSd = ServSd;

            /* Now listen for any client connections for this server socket */
            Status = listen(ServSd,5);
            if (Status < 0) {
                AmbaPrint_PrintUInt5("Error: RTSP Server Socket Listen", \
                                0U, 0U, 0U, 0U, 0U);
                (void)AmbaKAL_EventFlagSet(&(pSrv->EventFlag), RTSP_SERVER_FLAG_SOCKET_FAIL);
            } else {
                /* Notify socket done to task create API */
                (void)AmbaKAL_EventFlagSet(&(pSrv->EventFlag), RTSP_SERVER_FLAG_SOCKET_DONE);

                (void)AmbaKAL_GetSysTickCount(&SysTickCnt);
                MonitorPeriodStart = SysTickCnt;

                /* Loop to create and establish server connections.  */
                while (0U < KeepLoop) {
                    AmbaMisra_TouchUnused(&KeepLoop);

                    Err = AmbaKAL_EventFlagGet(&(pSrv->EventFlag),
                                                RTSP_SERVER_FLAG_START,
                                                AMBA_KAL_FLAGS_ALL,
                                                AMBA_KAL_FLAGS_CLEAR_NONE,
                                                &ActFlag,
                                                KAL_WAIT_FOREVER);
                    if (Err != OK) {
                        AmbaPrint_PrintUInt5("Error: RTSP Server wait RTSP_SERVER_FLAG_START failed %u", Err, 0U, 0U, 0U, 0U);
                    }

                    pSrv->ReadReady = pSrv->MasterList;
                    Timeout.tv_sec = SVC_RTSP_TIMEOUT_S;
                    Timeout.tv_usec = 0;

                    Status = select(pSrv->MaxSd + 1,&pSrv->ReadReady, NULL, NULL, &Timeout);
                    if (Status == ERROR) {
                        continue;
                    } else if (Status == 0) {
                        CheckClientTimeOut(pSrv);

                        (void)AmbaKAL_GetSysTickCount(&SysTickCnt);
                        MonitorPeriodStart = SysTickCnt;
                        continue;
                    } else {
                        // Do nothing
                    }

                    if (pSrv->RunningFlag == 0U) {
                        continue;
                    }

                    Sdi = 0;
                    while (Sdi < (pSrv->MaxSd + 1)) {
                        if (0 == FD_ISSET(Sdi, &pSrv->ReadReady)) {
                            Sdi++;
                            continue;
                        }

                        if (pSrv->RunningFlag == 0U) {
                            break;
                        } else if (Sdi == ServSd) {
                            INT32 newConnectSd;

                            pClientAddr = &ClientAddr;
                            AmbaMisra_TypeCast(&pSockAddr, &pClientAddr);
                        #if defined(CONFIG_NETX_ENET)
                            IntClientLen = (INT32)Clientlen;
                            newConnectSd = accept(ServSd, pSockAddr, &IntClientLen);
                        #else
                            IntClientLen = Clientlen;
                            newConnectSd = accept(ServSd, pSockAddr, &IntClientLen);
                        #endif
                            /* Add this new connection to our socket descriptor list */
                            FD_SET(newConnectSd,&pSrv->MasterList);
                            pSrv->MaxSd = (newConnectSd > pSrv->MaxSd) ? newConnectSd : pSrv->MaxSd;

                            /* Create new RTSP client session */
                            (void)RtspClnt_Create(pSrv, newConnectSd, ClientAddr);
                            Sdi++;
                            continue;
                        } else {
                            Index = LookUpWatchTable(pSrv, Sdi);
                            if (Index >= 0) {
                                pSrv->WatchTable[Index].func(Sdi, pSrv->WatchTable[Index].Ctx);
                            } else {
                                SVC_RTSP_CLNTSESSION_s* clnt = LookupClientBySd(pSrv, Sdi);
                                if (NULL == clnt) {
                                    AmbaPrint_PrintUInt5("Can't find corresponding rtspClientSession for incoming request(Sd=%u)!! Should NOT happen", \
                                                    (UINT32)Sdi, 0U, 0U, 0U, 0U);
                                    (void)SvcRtspServer_ClearSd(pSrv, Sdi);
                                } else {
                                    if (pSrv->RunningFlag > 0U) {
                                        RtspClnt_IncomingHandler(clnt);
                                    }
                                }
                            }
                        }
                        Sdi++;
                    }

                    (void)AmbaKAL_GetSysTickCount(&SysTickCnt);
                    if (TimeTickDiff(SysTickCnt, MonitorPeriodStart) > SVC_RTSP_TIMEOUT_MS) {
                        CheckClientTimeOut(pSrv);

                        (void)AmbaKAL_GetSysTickCount(&SysTickCnt);
                        MonitorPeriodStart = SysTickCnt;
                    }
                }
            }
        }

    #if defined(CONFIG_NETX_ENET)
        if (soc_close(ServSd) != 0) {
            /* do nothing */
        }
    #else
        if (close(ServSd) != 0) {
            /* do nothing */
        }
    #endif
    }

    return NULL;
}

UINT32 SvcRtspServer_GetInitDefaultCfg(SVC_RTSPSERVER_INIT_CFG_s *pDefaultCfg)
{
    SVC_STREAM_MGR_INIT_CFG_s StreamerCfg;
    UINT32 Rval = OK;

    if (pDefaultCfg == NULL) {
        AmbaPrint_PrintUInt5("SvcRtspServer_GetInitDefaultCfg: Invalid input", \
                        0U, 0U, 0U, 0U, 0U);
        Rval = ERR_NA;
    } else {
        if (AmbaWrap_memset(pDefaultCfg, 0, sizeof(SVC_RTSPSERVER_INIT_CFG_s))!= 0U) { }
        if (SvcRtspStrmMgrGetInitDefaultCfg(&StreamerCfg) != OK) {
            AmbaPrint_PrintUInt5("SvcRtspServer_GetInitDefaultCfg: AmpStreamerMgr_GetInitDefaultCfg fail", \
                            0U, 0U, 0U, 0U, 0U);
            Rval = ERR_NA;
        } else {

            pDefaultCfg->TaskInfo.Priority = 63;
            pDefaultCfg->TaskInfo.StackSize = SVC_RTSP_SERVER_STACK_SIZE;
            pDefaultCfg->TaskInfo.CoreSelection = SVC_RTSP_SERVER_DEFAULT_CORE;
            pDefaultCfg->GuardSize = SVC_RTSP_SERVER_GUARD_SIZE;
            pDefaultCfg->NumMaxClient = SVC_RTSP_MAX_CLIENT_SESSION;
            pDefaultCfg->NumMaxMediaSession = SVC_RTSP_MAX_MEDIA_SESSION;
            pDefaultCfg->NumMaxStreamer = StreamerCfg.NumMaxStreamer;
            pDefaultCfg->NumMaxActiveLiveGroup = StreamerCfg.NumMaxActiveLiveGroup;
            pDefaultCfg->MemoryPoolAddr = NULL;
            pDefaultCfg->MemoryPoolSize = pDefaultCfg->TaskInfo.StackSize + pDefaultCfg->GuardSize + ALIGN_32(sizeof(SVC_RTSP_SERVER_s))
                    + (pDefaultCfg->NumMaxClient * sizeof(SVC_RTSP_CLNTSESSION_s))
                    + (pDefaultCfg->NumMaxMediaSession * sizeof(MEDIA_SESSION_s))
                    + (pDefaultCfg->NumMaxStreamer * sizeof(MEDIA_SUBSESSION_s))
                    + ALIGN_32(StreamerCfg.MemoryPoolSize)
                    + SVC_RTSP_SERVER_BYTE_POOL_SIZE;
        }
    }
    return Rval;
}

UINT32 SvcRtspServer_GetRequiredMemorySize(SVC_RTSPSERVER_INIT_CFG_s const *pCfg, UINT32 *pMemSize)
{
    SVC_STREAM_MGR_INIT_CFG_s StreamerCfg;
    UINT32 StreamMgrMemorySize;
    UINT32 Rval = ERR_NA;

    if ((NULL == pCfg) || (NULL == pMemSize)) {
        AmbaPrint_PrintUInt5("SvcRtspServer_GetRequiredMemorySize: Invalid input", \
                        0U, 0U, 0U, 0U, 0U);
        Rval = ERR_NA;
    } else {
        if (SvcRtspStrmMgrGetInitDefaultCfg(&StreamerCfg) != OK) {
            AmbaPrint_PrintUInt5("SvcRtspServer_GetRequiredMemorySize: AmpStreamerMgr_GetInitDefaultCfg fail", \
                            0U, 0U, 0U, 0U, 0U);
            Rval = ERR_NA;
        } else {
            StreamerCfg.NumMaxStreamer = pCfg->NumMaxStreamer;
            StreamerCfg.NumMaxActiveLiveGroup = pCfg->NumMaxActiveLiveGroup;
            Rval = SvcRtspStrmMgrGetRequiredMemorySize(&StreamerCfg, &StreamMgrMemorySize);
            if (Rval == OK) {
                *pMemSize = pCfg->TaskInfo.StackSize + pCfg->GuardSize + ALIGN_32(sizeof(SVC_RTSP_SERVER_s))
                        + (pCfg->NumMaxClient * sizeof(SVC_RTSP_CLNTSESSION_s))
                        + (pCfg->NumMaxMediaSession * sizeof(MEDIA_SESSION_s))
                        + (pCfg->NumMaxStreamer * sizeof(MEDIA_SUBSESSION_s))
                        + ALIGN_32(StreamMgrMemorySize)
                        + SVC_RTSP_SERVER_BYTE_POOL_SIZE;
                Rval = OK;
            }
        }
    }

    return Rval;
}

/**
* AmbaLink memory init.
* @return OK/ERR_NA.
*/
UINT32 SvcRtspServer_MemPool_Init(void)
{
    UINT32 i;
    UINT32 Status;
    UINT32 Rval = OK;
    char pRtspServerMemoryPoolMtxName[] = "RTSPServerMemoryPool";
    static UINT8 RTSPServerMemPoolInit = 0;

    if(1U == RTSPServerMemPoolInit) {
        Rval = OK;
    } else {
        Status = AmbaKAL_MutexCreate(&RTSPServerMemoryPoolMutex, pRtspServerMemoryPoolMtxName);
        if (OK != Status) {
            AmbaPrint_PrintUInt5("RTSPServer: RTSPServerMemoryPool mutex create fail (%d)", Status, 0U, 0U, 0U, 0U);
            Rval = ERR_NA;
        } else {

            (void)AmbaKAL_MutexTake(&RTSPServerMemoryPoolMutex, AMBA_KAL_WAIT_FOREVER);
            for(i = 0; i < SVC_RTSP_POOL_MAX; i++) {
                if (AmbaWrap_memset(RTSPServerMemPool[i].pIndex, 0, RTSPServerMemPool[i].Number)!= 0U) { }
            }
            (void)AmbaKAL_MutexGive(&RTSPServerMemoryPoolMutex);
            RTSPServerMemPoolInit = 1U;
        }
    }

    return Rval;
}

/**
* Free the memory.
* @param [in]  Point to the memory buffer to be freed.
*/
void SvcRtspServer_MemPool_Free(UINT32 Num, void const *pPtr)
{
    UINT32 i,Find = 0U;
    SVC_RTSP_SERVER_MEM_POOL_s Tmp;
    ULONG PtrAddr;

    if(Num >= SVC_RTSP_POOL_MAX) {
        AmbaPrint_PrintUInt5("SvcRtspServer_MemPool_Free, Num(%d) is bigger than SVC_RTSP_POOL_MAX(%d)", Num, SVC_RTSP_POOL_MAX, 0U, 0U, 0U);
    } else {

        (void)AmbaKAL_MutexTake(&RTSPServerMemoryPoolMutex, AMBA_KAL_WAIT_FOREVER);
        Tmp.Number = RTSPServerMemPool[Num].Number;
        Tmp.Size = RTSPServerMemPool[Num].Size;
        Tmp.pPtr = RTSPServerMemPool[Num].pPtr;
        Tmp.pIndex = RTSPServerMemPool[Num].pIndex;
        for(i = 0U; i < Tmp.Number; i++) {
            if(&(Tmp.pPtr[i*Tmp.Size]) == pPtr) {
                Tmp.pIndex[i] = 0U;
                Find = 1U;
                break;
            }
        }
        (void)AmbaKAL_MutexGive(&RTSPServerMemoryPoolMutex);

        if(0U == Find) {
            AmbaMisra_TypeCast(&PtrAddr, &pPtr);
            // AmbaPrint_PrintUInt5("SvcRtspServer_MemPool_Free, Num(%d) pPtr(%x) free fail", Num, PtrAddr, 0U, 0U, 0U);
        }
    }
    return;
}

/**
* AmbaLink memory allocation.
* @param [in]  memory request size
* @return pointer to allocated memory buffer
*/
void *SvcRtspServer_MemPool_Malloc(UINT32 Num, UINT32 Size)
{
    UINT32 i,Find = 0U;
    SVC_RTSP_SERVER_MEM_POOL_s Tmp;
    void *pPtr = NULL;

    if(Num >= SVC_RTSP_POOL_MAX) {
        AmbaPrint_PrintUInt5("SvcRtspServer_MemPool_Malloc, Num(%d) is bigger than SVC_RTSP_POOL_MAX(%d)", Num, SVC_RTSP_POOL_MAX, 0U, 0U, 0U);
    } else {
        if(Size > RTSPServerMemPool[Num].Size) {
            AmbaPrint_PrintUInt5("SvcRtspServer_MemPool_Malloc, Size(%d) is bigger than AmbaLinkMemPool[%d].Size(%d) ",Size, Num, RTSPServerMemPool[Num].Size, 0U, 0U);
        } else {

            (void)AmbaKAL_MutexTake(&RTSPServerMemoryPoolMutex, AMBA_KAL_WAIT_FOREVER);
            Tmp.Number = RTSPServerMemPool[Num].Number;
            Tmp.Size = RTSPServerMemPool[Num].Size;
            Tmp.pPtr = RTSPServerMemPool[Num].pPtr;
            Tmp.pIndex = RTSPServerMemPool[Num].pIndex;

            for(i = 0U; i < Tmp.Number; i++) {
                if(0U == Tmp.pIndex[i]) {
                    pPtr = &(Tmp.pPtr[i*Tmp.Size]);
                    Tmp.pIndex[i] = 1U;
                    if (AmbaWrap_memset(pPtr, 0, Size)!= 0U) { }
                    Find = 1U;
                    break;
                }
            }
            (void)AmbaKAL_MutexGive(&RTSPServerMemoryPoolMutex);

            if(0U == Find) {
                AmbaPrint_PrintUInt5("SvcRtspServer_MemPool_Malloc, Num(%d) Malloc fail", Num, 0U, 0U, 0U, 0U);
            }
        }
    }

    return pPtr;
}

static UINT32 BiggerThan(UINT8 const *pAddrA, UINT8 const *pAddrB)
{
    ULONG AddrA, AddrB;
    UINT32 RetVal;

    AmbaMisra_TypeCast(&AddrA, &pAddrA);
    AmbaMisra_TypeCast(&AddrB, &pAddrB);

    if(AddrA > AddrB) {
        RetVal = 1U;
    } else {
        RetVal = 0U;
    }

    return RetVal;
}

static UINT8 * PtrAlign32(UINT8 const *pPtr)
{
    ULONG PtrAddr;
    UINT8 *pRetPtr;

    AmbaMisra_TypeCast(&PtrAddr, &pPtr);
    PtrAddr = ALIGN_32(PtrAddr);
    AmbaMisra_TypeCast(&pRetPtr, &PtrAddr);

    return pRetPtr;
}

UINT32 SvcRtspServer_Init(SVC_RTSPSERVER_INIT_CFG_s const *Cfg)
{
    UINT8 * pAddr;
    UINT8 const *pPoolEnd;
    UINT32 Er;
    UINT32 StreamerMemSize;
    SVC_RTSP_SERVER_s* RtspServer;
    SVC_STREAM_MGR_INIT_CFG_s StreamerCfg;
    static char  RtspFlagName[32U] = "Rtsp server";
    UINT32 Rval = OK;
    void const *pVoidPtr;

    if ((NULL == Cfg) || (NULL == Cfg->MemoryPoolAddr) || (0U == Cfg->MemoryPoolSize)) {
        AmbaPrint_PrintUInt5("SvcRtspServer_Init: Invalid input", \
                        0U, 0U, 0U, 0U, 0U);
        Rval = ERR_NA;
    } else {
        if (RtspServerPtr != NULL) {
            AmbaPrint_PrintUInt5("[RTSP Server] Already inited.", \
                            0U, 0U, 0U, 0U, 0U);
            Rval = OK;
        } else {

            pAddr = Cfg->MemoryPoolAddr;
            pPoolEnd = &(Cfg->MemoryPoolAddr[Cfg->MemoryPoolSize]);
            if (AmbaWrap_memset(pAddr, 0, Cfg->MemoryPoolSize)!= 0U) { }

            // addr points to remained buffer, allocate our rtspServer instance
            AmbaMisra_TypeCast(&RtspServer, &pAddr);
            pAddr = &(pAddr[sizeof(SVC_RTSP_SERVER_s)]);

            RtspServer->RunningFlag = 0U;
            RtspServer->TaskInfo = Cfg->TaskInfo;
            RtspServer->cbEvent = Cfg->cbEvent;
            RtspServer->cbPlayback = Cfg->cbPlayback;
            RtspServer->MaxClient = (UINT8)(Cfg->NumMaxClient);
            RtspServer->MaxMediaSession = (UINT16)Cfg->NumMaxMediaSession;
            RtspServer->MaxSubsession = (UINT16)Cfg->NumMaxStreamer;

            // allocate client session
            AmbaMisra_TypeCast(&(RtspServer->ClntList), &pAddr);
            pAddr = &pAddr[(RtspServer->MaxClient * sizeof(SVC_RTSP_CLNTSESSION_s))];

            if (1U == BiggerThan(pAddr, pPoolEnd)) {
                AmbaPrint_PrintUInt5("SvcRtspServer_Init: no enough memory space for client Session pool", \
                                0U, 0U, 0U, 0U, 0U);
                Rval = ERR_NA;
            } else {

                // allocate media session
                AmbaMisra_TypeCast(&(RtspServer->SessionList), &pAddr);
                pAddr = &pAddr[(RtspServer->MaxMediaSession * sizeof(MEDIA_SESSION_s))];
                if (1U == BiggerThan(pAddr, pPoolEnd)) {
                    AmbaPrint_PrintUInt5("SvcRtspServer_Init: no enough memory space for media Session pool", \
                                    0U, 0U, 0U, 0U, 0U);
                    Rval = ERR_NA;
                } else {

                    // allocate media subsession
                    AmbaMisra_TypeCast(&(RtspServer->SubSessionList), &pAddr);
                    pAddr = &pAddr[(RtspServer->MaxSubsession * sizeof(MEDIA_SUBSESSION_s))];
                    if (1U == BiggerThan(pAddr, pPoolEnd)) {
                        AmbaPrint_PrintStr5("SvcRtspServer_Init: no enough memory space for media subsession pool", NULL, NULL, NULL, NULL, NULL);
                        Rval = ERR_NA;
                    } else {

                        // allocate protection page, set all bytes=0xc4 on top of stack
                        RtspServer->GuardSize= Cfg->GuardSize;
                        RtspServer->GuardAddr = pAddr;
                        if(Cfg->GuardSize > 0U){
                            UINT32 i;
                            for (i = 0U; i < Cfg->GuardSize; i++){
                                pAddr[i] = 0xC4U;
                            }
                            pAddr = &pAddr[Cfg->GuardSize];
                        }

                        // allocate stack
                        RtspServer->Stack = PtrAlign32(pAddr);
                        pAddr = &RtspServer->Stack[Cfg->TaskInfo.StackSize];
                        if (1U == BiggerThan(pAddr, pPoolEnd)) {
                            AmbaPrint_PrintUInt5("SvcRtspServer_Init: no enough memory space for Stack", \
                                            0U, 0U, 0U, 0U, 0U);
                            Rval = ERR_NA;
                        } else {
                            // initialize streamerManager
                            Er = SvcRtspStrmMgrGetInitDefaultCfg(&StreamerCfg);
                            if (Er != OK) {
                                Rval = Er;
                            } else {
                                StreamerCfg.NumMaxStreamer = Cfg->NumMaxStreamer;
                                StreamerCfg.NumMaxActiveLiveGroup = Cfg->NumMaxActiveLiveGroup;

                                Er = SvcRtspStrmMgrGetRequiredMemorySize(&StreamerCfg, &StreamerMemSize);
                                if (Er != OK) {
                                    Rval = Er;
                                } else {
                                    StreamerCfg.cbEvent = Cfg->cbEvent;
                                    StreamerCfg.cbMediaInfo = Cfg->cbMediaInfo;
                                    StreamerCfg.cbStreamList = Cfg->cbStreamList;
                                    StreamerCfg.NumMaxStreamer = Cfg->NumMaxStreamer;
                                    StreamerCfg.NumMaxActiveLiveGroup = Cfg->NumMaxActiveLiveGroup;
                                    StreamerCfg.MemoryPoolAddr = pAddr;
                                    StreamerCfg.MemoryPoolSize = StreamerMemSize;

                                    Er = SvcRtspStrmMgrInit(&StreamerCfg);
                                    if (Er != OK) {
                                        AmbaPrint_PrintUInt5("SvcRtspServer_Init: AmpStreamerManager Init fail", \
                                                        0U, 0U, 0U, 0U, 0U);
                                        Rval = ERR_NA;
                                    } else {
                                        pAddr = &pAddr[StreamerMemSize];
                                        if (1U == BiggerThan(pAddr, pPoolEnd)) {
                                            AmbaPrint_PrintUInt5("SvcRtspServer_Init: no enough memory space for AmpStreamerManager", \
                                                            0U, 0U, 0U, 0U, 0U);
                                            (void)SvcRtspServer_Release();
                                            Rval = ERR_NA;
                                        } else {

                                            // Create Byte Pool, use the rest of the memory
                                            (void)SvcRtspServer_MemPool_Init();

                                            // Save rtspServer pointer for following operation
                                            RtspServerPtr = RtspServer;

                                            // init client session property
                                            pVoidPtr = SvcRtspServer_MemPool_Malloc(SVC_RTSP_POOL_RESPONSE_BUF, SvcRtspServer_GetBufferSize());
                                            AmbaMisra_TypeCast(&RtspServer->ResponseBuf, &pVoidPtr);
                                            if (NULL == RtspServer->ResponseBuf) {
                                                AmbaPrint_PrintStr5("SvcRtspServer_Init: malloc for response buffer fail", NULL, NULL, NULL, NULL, NULL);
                                                (void)SvcRtspServer_Release();
                                                Rval = ERR_NA;
                                            } else {
                                                UINT8 i;
                                                for (i = 0U; i < RtspServer->MaxClient; i++) {
                                                    /* All client use same responseBuf allocated in server(reduce memory requirement)
                                                                                         * because we always serve one client one time and send response in one shot
                                                                                         */
                                                    RtspServer->ClntList[i].ResponseBuf = RtspServer->ResponseBuf;
                                                    RtspServer->ClntList[i].Sd = -1;
                                                }

                                                // Create event flag
                                                Er = AmbaKAL_EventFlagCreate(&(RtspServer->EventFlag), RtspFlagName);
                                                if (Er != OK) {
                                                    AmbaPrint_PrintUInt5("[RTSP Server] Event flag create fail", \
                                                                    0U, 0U, 0U, 0U, 0U);
                                                    (void)SvcRtspServer_Release();
                                                    Rval = ERR_NA;
                                                } else {
                                                    Er = AmbaKAL_EventFlagClear(&(RtspServer->EventFlag), 0xFFFFFFFFU); // clear all
                                                    if (Er != OK) {
                                                        AmbaPrint_PrintUInt5("[RTSP Server] Event flag clear fail", \
                                                                        0U, 0U, 0U, 0U, 0U);
                                                        (void)SvcRtspServer_Release();
                                                        Rval = ERR_NA;
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return Rval;
}

UINT32 SvcRtspServer_Release(void)
{
    UINT32 Rval = OK;

    if (RtspServerPtr == NULL) {
        AmbaPrint_PrintUInt5("SvcRtspServer_Release: Rtsp server is not inited", \
                        0U, 0U, 0U, 0U, 0U);
        Rval = ERR_NA;
    } else {
        if (1U == RtspServerPtr->RunningFlag) {
            (void)AmbaKAL_TaskTerminate(&RtspServerPtr->Task);
            (void)AmbaKAL_TaskDelete(&RtspServerPtr->Task);
            RtspServerPtr->RunningFlag = 0;
        }
        (void)SvcStrmMgrRelease();
        (void)AmbaKAL_EventFlagDelete(&RtspServerPtr->EventFlag);
        RtspServerPtr = NULL;
    }
    return Rval;
}

UINT32 SvcRtspServer_Start(void)
{
    UINT32 Er;
    UINT32 ActFlag;
    SVC_RTSP_SERVER_s* RtspServer;
    UINT32 Rval = OK;
    char pRtspServerTaskName[] = "RTSPServerTask";
    ULONG RtspServerAddr;
    static UINT32 TaskCreated = 0U;

    if (RtspServerPtr == NULL) {
        AmbaPrint_PrintUInt5("SvcRtspServer_Start: Rtsp server has not been inited", \
                        0U, 0U, 0U, 0U, 0U);
        Rval = ERR_NA;
    } else {
        RtspServer = RtspServerPtr;
        if (TaskCreated == 1U) {
            if (RtspServer->RunningFlag == 0U) {
                RtspServer->RunningFlag = 1U;
                (void)AmbaKAL_EventFlagSet(&(RtspServer->EventFlag), RTSP_SERVER_FLAG_START);
            }
            Rval = OK;
        } else {

            // Create RTSPServer task
            AmbaMisra_TypeCast(&RtspServerAddr, &RtspServer);
            Er = AmbaKAL_TaskCreate(&RtspServer->Task, pRtspServerTaskName, RtspServer->TaskInfo.Priority, \
                 AmbaRTSPServerEntry, RtspServer, RtspServer->Stack, RtspServer->TaskInfo.StackSize, AMBA_KAL_AUTO_START);
            if (Er != OK) {
                AmbaPrint_PrintUInt5("SvcRtspServer_Start: RTSPServer Task create Fail!", \
                                0U, 0U, 0U, 0U, 0U);
                Rval = ERR_NA;
            } else {
                TaskCreated = 1U;
                // Wait socket creation/binding result (it could fail if wifi is not ready)
                Er = AmbaKAL_EventFlagGet(&(RtspServer->EventFlag),
                                            RTSP_SERVER_FLAG_SOCKET_FAIL | RTSP_SERVER_FLAG_SOCKET_DONE,
                                            AMBA_KAL_FLAGS_ANY,
                                            AMBA_KAL_FLAGS_CLEAR_AUTO,
                                            &ActFlag,
                                            2000);

                if (Er != OK) {
                    AmbaPrint_PrintUInt5("[RTSP Server] Timeout in waiting Rtsp Server Task socket binding", \
                                    0U, 0U, 0U, 0U, 0U);
                    Rval = ERR_NA;
                } else {

                    if (0U != (ActFlag & RTSP_SERVER_FLAG_SOCKET_DONE)) {
                        RtspServer->RunningFlag = 1;
                        (void)AmbaKAL_EventFlagSet(&(RtspServer->EventFlag), RTSP_SERVER_FLAG_START);
                        AmbaPrint_PrintUInt5("[RTSP Server] Start rtsp server success", \
                                        0U, 0U, 0U, 0U, 0U);
                        Rval = OK;
                    } else {
                        AmbaPrint_PrintUInt5("[RTSP Server] Start rtsp server fail", \
                                        0U, 0U, 0U, 0U, 0U);
                        Rval = ERR_NA;
                    }
                }
            }
        }
    }

    return Rval;
}

UINT32 SvcRtspServer_Stop(void)
{
    SVC_RTSP_SERVER_s* RtspServer;
    UINT32 Rval = OK;

    if (RtspServerPtr == NULL) {
        AmbaPrint_PrintUInt5("SvcRtspServer_Stop: Rtsp server has not been inited", \
                        0U, 0U, 0U, 0U, 0U);
        Rval = ERR_NA;
    } else {
        RtspServer = RtspServerPtr;
        if (0U == RtspServer->RunningFlag) {
            AmbaPrint_PrintUInt5("SvcRtspServer_Stop: Rtsp server is NOT running", \
                            0U, 0U, 0U, 0U, 0U);
            Rval = ERR_NA;
        } else {
            // (void)AmbaKAL_TaskTerminate(&RtspServerPtr->Task);
            // (void)AmbaKAL_TaskDelete(&RtspServerPtr->Task);
            (void)AmbaKAL_EventFlagClear(&(RtspServer->EventFlag), RTSP_SERVER_FLAG_START);
            RtspServer->RunningFlag = 0;
        }
    }
    return Rval;
}

UINT32 SvcRtspServer_Notify(UINT32 Notify, UINT32 Param1, UINT32 Param2)
{
    (VOID)Param1;
    (VOID)Param2;

    (void)SvcStrmMgrNotify(Notify);
    return 0U;
}


/*
 *  Here we divide the byte pool for responseBuf and MediaSession->sdp_line
 *  By doing so AppLib can enlarge the memory size assigned to RTSPServer when we run out space to store sdp_line/response
 */
UINT32 SvcRtspServer_GetBufferSize(void)
{
    return MEM_POOL_RESPONSE_BUF_SIZE;
}

SVC_RTSP_CLNTSESSION_s* SvcRtspServer_GetClientSlot(SVC_RTSP_SERVER_s const * pSrv)
{
    UINT32 i;
    SVC_RTSP_CLNTSESSION_s *pPtr = NULL;

    if (NULL != pSrv) {
        for (i=0; i < pSrv->MaxClient ; i++) {
            if (pSrv->ClntList[i].Sd == -1) {
                pPtr = &pSrv->ClntList[i];
            }
        }
    }

    return pPtr;
}

void* SvcRtspServer_GetSessionSlot(SVC_RTSP_SERVER_s const * pSrv)
{
    UINT32 i;
    MEDIA_SESSION_s *pSessionList;
    MEDIA_SESSION_s *pPtr = NULL;

    AmbaMisra_TypeCast(&pSessionList, &pSrv->SessionList);
    for (i = 0U; i < pSrv->MaxMediaSession ; i++) {
        if (0U == AmbaUtility_StringLength(pSessionList->StreamName)) {
            pPtr = pSessionList;
            break;
        }
        pSessionList = &pSessionList[1U];
    }
    return pPtr;
}

void* SvcRtspServer_GetSubsessionSlot(SVC_RTSP_SERVER_s const * pSrv)
{
    UINT32 i;
    MEDIA_SUBSESSION_s *pSubSession;
    MEDIA_SUBSESSION_s *pPtr = NULL;

    AmbaMisra_TypeCast(&pSubSession, &pSrv->SubSessionList);
    for (i = 0U; i < pSrv->MaxSubsession ; i++) {
        if (0U == pSubSession->CodecID) {
            pPtr = pSubSession;
        }
        pSubSession = &pSubSession[1U];
    }
    return pPtr;
}

void* SvcRtspServer_LookupMediaSession(SVC_RTSP_SERVER_s const *pSrv, const char* Name)
{
    UINT32 i;
    MEDIA_SESSION_s *pSession;
    MEDIA_SESSION_s *pPtr = NULL;

    AmbaMisra_TypeCast(&pSession, &pSrv->SessionList);
    for (i = 0U; i < pSrv->MaxMediaSession; i++) {
        if (AmbaUtility_StringCompare(pSession->StreamName, Name, AmbaUtility_StringLength(pSession->StreamName)) == 0) {
            pPtr = pSession;
            break;
        }
        pSession = &pSession[1U];
    }
    if(NULL == pPtr) {
        pPtr = SvcMediaSession_Create(pSrv, Name);
    }

    return pPtr;
}

UINT32 SvcRtspServer_GetPlaybackSessionCount(SVC_RTSP_SERVER_s const * pSrv)
{
    UINT32 i;
    UINT32 Num = 0U;
    MEDIA_SESSION_s *pSession;

    AmbaMisra_TypeCast(&pSession, &pSrv->SessionList);
    for (i = 0U; i < pSrv->MaxMediaSession; i++) {
        if(0U != AmbaUtility_StringLength(pSession->StreamName)) {
            if (AmbaUtility_StringCompare(pSession->StreamName, "live", 4U) != 0) {
                Num++;
            }
        }
        pSession = &pSession[1U];
    }
    return Num;
}

UINT32 SvcRtspServer_GetRTSP_URL(SVC_RTSP_SERVER_s const * pSrv, void const *pSession,
                              char* pBuf, UINT32 Len, UINT32 IpAddress)
{
    UINT32 Rval = OK;
    char IP[16] = { '\0'};
    MEDIA_SESSION_s* pMediaSession;

    AmbaMisra_TypeCast(&pMediaSession, &pSession);

    if ((NULL == pSrv) || (NULL == pMediaSession) || (NULL == pBuf) || (0U == Len)) {
        Rval = ERR_NA;
    } else {

        if (0U != IpAddress) {
            {
                if (AmbaWrap_memset(IP, 0, 16U)!= 0U) { }

                // (void)AmbaAdvSnPrint(IP, 16, "%u.%u.%u.%u", ((IpAddress & 0xFF000000U) >> 24U), ((IpAddress & 0x00FF0000U) >> 16U),
                //                     ((IpAddress & 0x0000FF00U) >> 8U), (IpAddress & 0x000000FFU));
                {
                    char    *StrBuf = IP;
                    UINT32  BufSize = 16U;
                    UINT32  CurStrLen, Temp;

                    AmbaUtility_StringAppend(StrBuf, BufSize, "");

                    CurStrLen = AmbaUtility_StringLength(StrBuf);
                    if (CurStrLen < BufSize) {
                        Temp = ((IpAddress & 0xFF000000UL) >> 24U);
                        (void)AmbaUtility_UInt64ToStr(&(StrBuf[CurStrLen]), BufSize - CurStrLen, (UINT64)Temp, 10U);
                    }
                    AmbaUtility_StringAppend(StrBuf, BufSize, ".");

                    CurStrLen = AmbaUtility_StringLength(StrBuf);
                    if (CurStrLen < BufSize) {
                        Temp = ((IpAddress & 0x00FF0000UL) >> 16U);
                        (void)AmbaUtility_UInt64ToStr(&(StrBuf[CurStrLen]), BufSize - CurStrLen, (UINT64)Temp, 10U);
                    }
                    AmbaUtility_StringAppend(StrBuf, BufSize, ".");

                    CurStrLen = AmbaUtility_StringLength(StrBuf);
                    if (CurStrLen < BufSize) {
                        Temp = ((IpAddress & 0x0000FF00UL) >> 8U);
                        (void)AmbaUtility_UInt64ToStr(&(StrBuf[CurStrLen]), BufSize - CurStrLen, (UINT64)Temp, 10U);
                    }
                    AmbaUtility_StringAppend(StrBuf, BufSize, ".");

                    CurStrLen = AmbaUtility_StringLength(StrBuf);
                    if (CurStrLen < BufSize) {
                        Temp = ((IpAddress & 0x000000FFUL));
                        (void)AmbaUtility_UInt64ToStr(&(StrBuf[CurStrLen]), BufSize - CurStrLen, (UINT64)Temp, 10U);
                    }
                    AmbaUtility_StringAppend(StrBuf, BufSize, "");
                }

                AmbaPrint_PrintStr5("%s, IP %s", __func__, IP, NULL, NULL, NULL);
            }
        } else {
            {
                if (AmbaWrap_memset(IP, 0, 16)!= 0U) { }

                // (void)AmbaAdvSnPrint(IP, 16, "192.168.42.1");
                AmbaUtility_StringAppend(IP, 16U, "192.168.42.1");
                AmbaPrint_PrintStr5("%s, IP %s", __func__, IP, NULL, NULL, NULL);
            }
        }

        if (pSrv->Port == SVC_RTSP_SERVER_DEFAULT_PORT) {
            {
                if (AmbaWrap_memset(pBuf, 0, Len)!= 0U) { }

                // (void)AmbaAdvSnPrint(pBuf, Len, "rtsp://%s/%s", IP, pMediaSession->StreamName);
                {
                    char    *StrBuf = pBuf;
                    UINT32  BufSize = Len;

                    AmbaUtility_StringAppend(StrBuf, BufSize, "rtsp://");
                    AmbaUtility_StringAppend(StrBuf, BufSize, (IP));
                    AmbaUtility_StringAppend(StrBuf, BufSize, "/");
                    AmbaUtility_StringAppend(StrBuf, BufSize, (pMediaSession->StreamName));
                    AmbaUtility_StringAppend(StrBuf, BufSize, "");
                }
                AmbaPrint_PrintStr5("%s, Buf %s", __func__, pBuf, NULL, NULL, NULL);
            }
        } else {
            {
                if (AmbaWrap_memset(pBuf, 0, Len)!= 0U) { }

                // (void)AmbaAdvSnPrint(pBuf, Len, "rtsp://%s:%u/%s", IP, pSrv->Port, pMediaSession->StreamName);
                {
                    char    *StrBuf = pBuf;
                    UINT32  BufSize = Len;
                    UINT32  CurStrLen;

                    AmbaUtility_StringAppend(StrBuf, BufSize, "rtsp://");
                    AmbaUtility_StringAppend(StrBuf, BufSize, (IP));
                    AmbaUtility_StringAppend(StrBuf, BufSize, ":");

                    CurStrLen = AmbaUtility_StringLength(StrBuf);
                    if (CurStrLen < BufSize) {
                        (void)AmbaUtility_UInt64ToStr(&(StrBuf[CurStrLen]), BufSize - CurStrLen, (UINT64)(pSrv->Port), 10U);
                    }
                    AmbaUtility_StringAppend(StrBuf, BufSize, "/");
                    AmbaUtility_StringAppend(StrBuf, BufSize, (pMediaSession->StreamName));
                    AmbaUtility_StringAppend(StrBuf, BufSize, "");
                }
                AmbaPrint_PrintStr5("%s, pBuf %s", __func__, pBuf, NULL, NULL, NULL);
            }
        }
    }
    AmbaMisra_TouchUnused(pMediaSession);

    return Rval;
}

UINT32 SvcRtspServer_RegWatcher(SVC_RTSP_SERVER_s* pSrv, INT32 Sd, watcher_cb EventFunc, void * pCtx)
{
    UINT32 i;
    UINT32 Rval = OK;

    if ((NULL == pSrv) || (NULL == EventFunc) || (Sd <= 0)) {
        AmbaPrint_PrintUInt5("SvcRtspServer_RegWatcher: Invalid input", \
                        0U, 0U, 0U, 0U, 0U);
        Rval = ERR_NA;
    } else {
        for (i=0; i < SVC_RTSP_MAX_IO_WATCHER; i++) {
            if (pSrv->WatchTable[i].Sd == 0) {
                pSrv->WatchTable[i].Sd = Sd;
                pSrv->WatchTable[i].func = EventFunc;
                pSrv->WatchTable[i].Ctx = pCtx;
                break;
            }
        }
        if (SVC_RTSP_MAX_IO_WATCHER == i) { //no available
            AmbaPrint_PrintUInt5("SvcRtspServer_RegWatcher: no availabe WatchTable slot", \
                            0U, 0U, 0U, 0U, 0U);
            Rval = ERR_NA;
        } else {
            /* Add this sd to our socket descriptor list */
            FD_SET(Sd, &pSrv->MasterList);
            pSrv->MaxSd = (Sd > pSrv->MaxSd) ? Sd : pSrv->MaxSd;
        }
    }

    return Rval;
}

UINT32 SvcRtspServer_UnregWatcher(SVC_RTSP_SERVER_s* pSrv, INT32 Sd)
{
    UINT32 i;
    UINT32 Rval = OK;
    if ((NULL == pSrv) || (Sd <= 0)) {
        AmbaPrint_PrintUInt5("SvcRtspServer_UnregWatcher: Invalid input", \
                        0U, 0U, 0U, 0U, 0U);
        Rval = ERR_NA;
    } else {

        for (i = 0U; i < SVC_RTSP_MAX_IO_WATCHER; i++) {
            if (pSrv->WatchTable[i].Sd == Sd) {
                pSrv->WatchTable[i].Sd = 0;
                break;
            }
        }
        if (i==SVC_RTSP_MAX_IO_WATCHER) { //no available
            AmbaPrint_PrintUInt5("SvcRtspServer_UnregWatcher: can't find watcher to unregister", \
                            0U, 0U, 0U, 0U, 0U);
            Rval = ERR_NA;
        } else {
            /* Remove this sd from our socket descriptor list */
            FD_CLR(Sd, &pSrv->MasterList);
            pSrv->MaxSd = (Sd == pSrv->MaxSd) ? (Sd - 1) : pSrv->MaxSd;
        }
    }

    return Rval;
}

UINT32 SvcRtspServer_ClearSd(SVC_RTSP_SERVER_s* pSrv, INT32 Sd)
{
    UINT32 Rval = OK;

    if ((NULL == pSrv) || (Sd <= 0)) {
        AmbaPrint_PrintUInt5("SvcRtspServer_ClearSd: Invalid input", \
                        0U, 0U, 0U, 0U, 0U);
        Rval = ERR_NA;
    } else {
        FD_CLR(Sd, &pSrv->MasterList);
        pSrv->MaxSd = (Sd == pSrv->MaxSd) ? (Sd - 1) : pSrv->MaxSd;
    #if defined(CONFIG_NETX_ENET)
        (void)soc_close(Sd);
    #else
        (void)close(Sd);
    #endif
    }
    return Rval;
}

UINT32 SvcRtspServer_PlaybackCmd(SVC_RTSP_SERVER_s const * pSrv, UINT32 Op, void const * pInfo)
{
    return pSrv->cbPlayback(NULL, Op, pInfo);
}

INT32 SvcRtspServer_ReportStatus(SVC_RTSP_SERVER_s const * pSrv, UINT32 Event, void const * pInfo)
{
    (void)pSrv->cbEvent(NULL, Event, pInfo);
    return 0;
}

UINT32 SvcRtspServer_ihtonl(UINT32 n)
{
    return ((n & 0xffU) << 24U)|
        ((n & 0xff00U) << 8U)|
        ((n & 0xff0000U) >> 8U)|
        ((n & 0xff000000U) >> 24U);
}

UINT32 SvcRtspServer_intohl(UINT32 n)
{
    return ((n & 0xffU) << 24U) |
           ((n & 0xff00U) << 8U) |
           ((n & 0xff0000U) >> 8U) |
           ((n & 0xff000000U) >> 24U);
}


UINT16 SvcRtspServer_ihtons(UINT16 n)
{
    return ((n & 0xffU) << 8U) | ((n & 0xff00U) >> 8U);
}

UINT16 SvcRtspServer_intohs(UINT16 n)
{
    return ((n & 0xffU) << 8U) | ((n & 0xff00U) >> 8U);
}



