/**
 *  @file AmbaCV_Protect.c
 *
 *  Copyright (c) 2020 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  This file includes sample code and is only for internal testing and evaluation.  If you
 *  distribute this sample code (whether in source, object, or binary code form), it will be
 *  without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details AmbaCV Protect Utility APIs
 *
 */

#include "cvapi_protection.h"
#include "AmbaFS.h"
#include "AmbaCache.h"
#if defined(CONFIG_QNX) || defined(CONFIG_LINUX)
#include "AmbaDSP_Def.h"
#endif

#if !defined(CONFIG_QNX) && !defined(CONFIG_LINUX)
extern uint32_t get_protect_task_priority(void);
extern uint32_t get_protect_task_size(void);
extern uint32_t get_protect_task_stack_ptr(char **ppstack);
#endif

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS) || defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52) || defined(CONFIG_SVC_APPS_CVUT)
INT32 AmbaCV_ProtectInit(UINT32 cpu_map)
{
    (void) cpu_map;
    return (INT32) OK;
}

UINT32 AmbaCV_ProtectDeInit(void)
{
    return OK;
}

#else
static AMBA_KAL_TASK_t CvProtectTask;
static UINT32 Init = 0U;
static uint32_t cv_protect_loop;

static void* ProtectTaskEntry(void *EntryArg)
{
#if defined(CONFIG_QNX) || defined(CONFIG_LINUX)
    extern UINT32 AmbaDSP_GetProtectBufInfo(AMBA_DSP_PROTECT_s *pBuf);
    AMBA_DSP_PROTECT_s Buf;
    UINT64 Addr = 0UL;
    UINT64 Tmp;
    UINT8 *pBuf = NULL;
#else
    extern void HL_GetProtectBufInfo(ULONG *Addr, UINT32 *Size);
    ULONG Addr = 0U;
    ULONG Tmp;
#endif
    UINT32 Size = 0U;
    UINT32 PreTS = 0U;
    UINT32 LatestTS = 0x0U;
    UINT32 SentTS = 0x0U;
    const UINT32* LatestTSAddr;
    UINT32 SameTSCnt = 0U;
    //UINT32 Cnt = 0U;
    void *pAddr;
    UINT32 DlyTime = 1U;
    UINT32 ret = 0U;
    INT32 ret1 = 0;

    (void) EntryArg;
    AmbaMisra_TouchUnused(EntryArg);

    while (cv_protect_loop == 1U) {
#if defined(CONFIG_QNX) || defined(CONFIG_LINUX)
        AmbaDSP_GetProtectBufInfo(&Buf);
        pBuf = &Buf.Data[0];
        AmbaMisra_TypeCast(&Addr, &pBuf);
        Size = AMBA_DSP_PROTECT_DATA_LEN;
        (void) Size;
#else
        HL_GetProtectBufInfo(&Addr, &Size);
#endif
        if (Addr != 0U) {
            //Tmp = Addr+16U;
#if defined(CONFIG_THREADX)
            ret |=  AmbaCache_DataInvalidate(Addr, 64);
#endif
            Tmp = Addr+48U;
            AmbaMisra_TypeCast(&LatestTSAddr, &Tmp);
            LatestTS = *LatestTSAddr;
            //AmbaPrint_PrintUInt5("Call HL_GetProtectBufInfo(): LatestTS = %d ",LatestTS, 0U, 0U, 0U, 0U);

            if (PreTS!=LatestTS) {
                SameTSCnt=0;
            } else {
                SameTSCnt++;
            }
            if (SentTS != LatestTS) {
                if (SameTSCnt == 1U) {
                    // send to visorc
                    extern int32_t visorc_security_send_payload(void *vpPayload);

                    AmbaMisra_TypeCast(&pAddr, &Addr);
                    ret1 =  visorc_security_send_payload(pAddr);
                    DlyTime = 100U;
                    SentTS = LatestTS;
                    //AmbaPrint_PrintUInt5("Call visorc_security_send_payload(): SentTS = %d ",SentTS, 0U, 0U, 0U, 0U);
                }
            } else {
                // Cnt++;
            }
            PreTS = LatestTS;
        }
        //ShellPrintFunc("ProtectTaskEntry...\n");
        ret |=  AmbaKAL_TaskSleep(DlyTime);
    }
    (void) ret;
    (void) ret1;
    return NULL;
}

INT32 AmbaCV_ProtectInit(UINT32 cpu_map)
{
    char TaskName[] = "CV_Protect";
    UINT32 ret = 0U;

    if (Init == 0U) {
#if defined(CONFIG_QNX) || defined(CONFIG_LINUX)
        (void) cpu_map;
        cv_protect_loop = 1U;
        ret |= AmbaKAL_TaskCreate(&CvProtectTask, TaskName, 0U,
                                  ProtectTaskEntry, NULL,
                                  NULL, 0U, 0U);
        ret |= AmbaKAL_TaskResume(&CvProtectTask);
#else
        UINT32 stack_size, task_priority;
        char *pU8;
        void *pVoid;

        stack_size = get_protect_task_size();
        task_priority = get_protect_task_priority();

        ret |= get_protect_task_stack_ptr(&pU8);
        AmbaMisra_TypeCast(&pVoid, &pU8);
        cv_protect_loop = 1U;
        ret |= AmbaKAL_TaskCreate(&CvProtectTask, TaskName, task_priority,
                                  ProtectTaskEntry, NULL,
                                  pVoid, stack_size, 0U);
        ret |= AmbaKAL_TaskSetSmpAffinity(&CvProtectTask, cpu_map);
        ret |= AmbaKAL_TaskResume(&CvProtectTask);
#endif
        if(ret != 0U) {
            AmbaPrint_PrintUInt5("AmbaCV_ProtectInit(): ret %d", ret, 0U, 0U, 0U, 0U);
        }
        Init = 1U;
    }
    (void) ret;
    return (INT32) OK;

}

UINT32 AmbaCV_ProtectDeInit(void)
{
    UINT32 Rval;

#if !defined(CONFIG_LINUX)
    Rval = AmbaKAL_TaskSuspend(&CvProtectTask);
    if (Rval != KAL_ERR_NONE) {
        AmbaPrint_PrintUInt5("AmbaCV_ProtectDeInit(): AmbaKAL_TaskSuspend CvProtectTask fail return %d", Rval, 0U, 0U, 0U, 0U);
    } else
#endif
    {
        Rval = AmbaKAL_TaskTerminate(&(CvProtectTask));
        if (Rval != KAL_ERR_NONE) {
            AmbaPrint_PrintUInt5("AmbaCV_ProtectDeInit(): AmbaKAL_TaskTerminate CvProtectTask fail return %d", Rval, 0U, 0U, 0U, 0U);
        } else {
            Rval = AmbaKAL_TaskDelete(&(CvProtectTask));
            if (Rval != KAL_ERR_NONE) {
                AmbaPrint_PrintUInt5("AmbaCV_ProtectDeInit(): AmbaKAL_TaskDelete CvProtectTask fail return %d", Rval, 0U, 0U, 0U, 0U);
            } else {
                Init = 0U;
            }
        }
    }
    cv_protect_loop = 0U;
    return Rval;

}

#endif
