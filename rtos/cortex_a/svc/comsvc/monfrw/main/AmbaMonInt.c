/**
 *  @file AmbaMonInt.c
 *
 *  Copyright (c) [2020] Ambarella International LP
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
 *  @details Amba Monitor Interrupt
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaDSP.h"
#include "AmbaVIN_Def.h"
#include "AmbaSYS.h"

#include "AmbaDspInt.h"

#include "AmbaMonFramework.h"
#include "AmbaMonDef.h"

#include "AmbaMonVin.h"
#include "AmbaMonVout.h"

#include "AmbaMonInt.h"
#include "AmbaMonInt_Platform.h"

#define OK_UL  ((UINT32) 0U)
#define NG_UL  ((UINT32) 1U)

typedef union /*_AMBA_MON_INT_MEM_u_*/ {
    AMBA_MON_MAIN_MEM_ADDR     Data;
    void                       *pVoid;
    const void                 *pCvoid;
    AMBA_MON_VIN_ISR_RDY_s     *pVinIsrRdy;
    AMBA_MON_VOUT_ISR_RDY_s    *pVoutIsrRdy;
} AMBA_MON_INT_MEM_u;

typedef struct /*_AMBA_MON_INT_MEM_s_*/ {
    AMBA_MON_INT_MEM_u    Ctx;
} AMBA_MON_INT_MEM_s;

static AMBA_KAL_TASK_t GNU_SECTION_NOZEROINIT AmbaMonIntTaskId;
static void AmbaMonInt_Task(UINT32 Param);
static void *AmbaMonInt_TaskEx(void *pParam);

static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaMonInt_Ready;
static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaMonInt_Go;
static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaMonInt_Ack;

static UINT32 AmbaMonInt_Cmd = (UINT32) AMBA_MON_INT_TASK_CMD_STOP;

static AMBA_DSP_INT_PORT_s AmbaMonDspInt_Port = {0};
#define AMBA_MON_INT_PUSH_EVENT_FLAG    0x80000000U

/**
 *  @private
 *  Amba monitor vin/vout interrupt create
 *  @param[in] Priority task priority id
 *  @param[in] CoreInclusion cpu core inclusion (bits)
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonInt_Create(UINT32 Priority, UINT32 CoreInclusion)
{
#define AMBA_MON_INT_TASK_STACK_SIZE    0xA000
    static UINT8 GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE AmbaMonIntTaskStack[AMBA_MON_INT_TASK_STACK_SIZE];
    static char AmbaMonIntTaskName[] = "MonitorInt";

    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaMonInt_Ready, NULL, 0U);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaMonInt_Go, NULL, 0U);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaMonInt_Ack, NULL, 0U);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }

    if (RetCode == OK_UL) {
        AmbaMonPrintStr(S_PRINT_FLAG_MSG, "amba mon create dsp interrupt task");
        {
            AMBA_MON_INT_MEM_s MemInfo;
            MemInfo.Ctx.Data = 0U;
            FuncRetCode = AmbaKAL_TaskCreate(&AmbaMonIntTaskId,
                                             AmbaMonIntTaskName,
                                             Priority,
                                             AmbaMonInt_TaskEx,
                                             MemInfo.Ctx.pVoid,
                                             AmbaMonIntTaskStack,
                                             AMBA_MON_INT_TASK_STACK_SIZE,
                                             AMBA_KAL_DONT_START);
        }
        if (FuncRetCode == KAL_ERR_NONE) {
            FuncRetCode = AmbaKAL_TaskSetSmpAffinity(&AmbaMonIntTaskId, CoreInclusion);
            if (FuncRetCode != KAL_ERR_NONE) {
                RetCode = NG_UL;
            }
            FuncRetCode = AmbaKAL_TaskResume(&AmbaMonIntTaskId);
            if (FuncRetCode != KAL_ERR_NONE) {
                RetCode = NG_UL;
            }
        } else {
            RetCode = NG_UL;
        }
    }

    return RetCode;
}

/**
 *  @private
 *  Amba monitor vin/vout interrupt delete
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonInt_Delete(void)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    FuncRetCode = AmbaKAL_TaskTerminate(&AmbaMonIntTaskId);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    FuncRetCode = AmbaKAL_TaskDelete(&AmbaMonIntTaskId);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }

    FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaMonInt_Ready);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaMonInt_Go);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaMonInt_Ack);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba monitor vin/vout interrupt active
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonInt_Active(void)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    if (AmbaMonInt_Cmd == (UINT32) AMBA_MON_INT_TASK_CMD_STOP) {
        /* ready take */
        FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaMonInt_Ready, AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            /* cmd start */
            AmbaMonInt_Cmd = (UINT32) AMBA_MON_INT_TASK_CMD_START;
            /* go release */
            FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaMonInt_Go);
            if (FuncRetCode == KAL_ERR_NONE) {
                /* ack take */
                FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaMonInt_Ack, AMBA_KAL_WAIT_FOREVER);
                if (FuncRetCode != KAL_ERR_NONE) {
                    /* ack fail */
                    RetCode = NG_UL;
                }
            } else {
                /* go fail */
                RetCode = NG_UL;
            }
        } else {
            /* not ready */
            RetCode = NG_UL;
        }
    }

    return RetCode;
}

/**
 *  @private
 *  Amba monitor vin/vout interrupt inactive
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonInt_Inactive(void)
{
    if (AmbaMonInt_Cmd == (UINT32) AMBA_MON_INT_TASK_CMD_START) {
        AmbaMonInt_Cmd = (UINT32) AMBA_MON_INT_TASK_CMD_STOP;
    }

    return OK_UL;
}

/**
 *  @private
 *  Amba monitor interrupt push
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonInt_Push(void)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    FuncRetCode = AmbaDspInt_AuxGive(&(AmbaMonDspInt_Port), AMBA_MON_INT_PUSH_EVENT_FLAG);
    if (FuncRetCode != OK_UL) {
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba monitor vin/vout interrupt idle wait
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonInt_IdleWait(void)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    /* ready take */
    FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaMonInt_Ready, AMBA_KAL_WAIT_FOREVER);
    if (FuncRetCode == KAL_ERR_NONE) {
        /* ready give */
        FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaMonInt_Ready);
        if (FuncRetCode != KAL_ERR_NONE) {
            /* ready give fail */
            RetCode = NG_UL;
        }
    } else {
        /* ready take fail */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba monitor vin/vout interrupt task ex
 *  @param[in] pParam pointer to parameter
 *  @note this function is intended for internal use only
 */
static void *AmbaMonInt_TaskEx(void *pParam)
{
    AMBA_MON_INT_MEM_s MemInfo;
    MemInfo.Ctx.pVoid = pParam;
    AmbaMonInt_Task((UINT32) MemInfo.Ctx.Data);
    return NULL;
}

/**
 *  Amba monitor vin/vout interrupt task
 *  @param[in] Param (unused)
 *  @note this function is intended for internal use only
 */
static void AmbaMonInt_Task(UINT32 Param)
{
    static UINT32 AmbaMonDspInt_DummyFlag = 1U;

    UINT8 FirstActive = 1U;

    UINT32 FuncRetCode;
    UINT32 ActualFlags;
    UINT32 i;

    UINT32 DspVinIntFlag = 0U;
    UINT32 DspVoutIntFlag = 0U;

    UINT32 DspIntFlag;

    AMBA_MON_INT_MEM_s MemInfo;
    AMBA_MON_VIN_ISR_RDY_s VinIsrRdy;
    AMBA_MON_VOUT_ISR_RDY_s VoutIsrRdy;

    AMBA_DSP_INT_PORT_s *pDspIntPort;

    UINT32 SofDtsCarry[AMBA_MON_NUM_VIN_CHANNEL];
    UINT32 EofDtsCarry[AMBA_MON_NUM_VIN_CHANNEL];
    UINT32 VoutDtsCarry[AMBA_MON_NUM_VOUT_CHANNEL];

    UINT32 Dts;

    UINT32 LastSofDts[AMBA_MON_NUM_VIN_CHANNEL];
    UINT32 LastEofDts[AMBA_MON_NUM_VIN_CHANNEL];
    UINT32 LastVoutDts[AMBA_MON_NUM_VOUT_CHANNEL];

    (void) Param;
    pDspIntPort = &(AmbaMonDspInt_Port);

    /* dts reset */
    for (i = 0U; i < AMBA_MON_NUM_VIN_CHANNEL; i++) {
        /* carry reset */
        SofDtsCarry[i] = 0U;
        EofDtsCarry[i] = 0U;
        /* last reset */
        LastSofDts[i] = 0U;
        LastEofDts[i] = 0U;
    }
    for (i = 0U; i < AMBA_MON_NUM_VOUT_CHANNEL; i++) {
        /* carry reset */
        VoutDtsCarry[i] = 0U;
        /* last reset */
        LastVoutDts[i] = 0U;
    }

    /* vin interrupt flag */
    for (i = 0U; i < AMBA_MON_NUM_VIN_CHANNEL; i++) {
        DspVinIntFlag |= (AmbaMonInt_VinFlag[i].Sof | AmbaMonInt_VinFlag[i].Eof);
    }

    /* vout interrupt flag */
    for (i = 0U; i < AMBA_MON_NUM_VOUT_CHANNEL; i++) {
        DspVoutIntFlag |= AmbaMonInt_VoutFlag[i];
    }

    /* total interrupt flag */
    DspIntFlag = DspVoutIntFlag | DspVinIntFlag | AMBA_MON_INT_PUSH_EVENT_FLAG;

    AmbaMonInt_Cmd = (UINT32) AMBA_MON_INT_TASK_CMD_STOP;

    while (AmbaMonDspInt_DummyFlag > 0U) {
        if (AmbaMonInt_Cmd == (UINT32) AMBA_MON_INT_TASK_CMD_STOP) {
            if (FirstActive == 1U) {
                FirstActive = 0U;
            } else {
                /* interrupt event close */
                FuncRetCode = AmbaDspInt_Close(pDspIntPort);
                if (FuncRetCode != OK_UL) {
                    AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: amba mon int task port close");
                }
            }
            /* ready give */
            FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaMonInt_Ready);
            if (FuncRetCode != KAL_ERR_NONE) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: amba mon int task idle");
            }
            /* go take */
            FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaMonInt_Go, AMBA_KAL_WAIT_FOREVER);
            if (FuncRetCode != KAL_ERR_NONE) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: amba mon int task go");
            }
            /* dts reset */
            for (i = 0U; i < AMBA_MON_NUM_VIN_CHANNEL; i++) {
                /* carry reset */
                SofDtsCarry[i] = 0U;
                EofDtsCarry[i] = 0U;
                /* last reset */
                LastSofDts[i] = 0U;
                LastEofDts[i] = 0U;
            }
            for (i = 0U; i < AMBA_MON_NUM_VOUT_CHANNEL; i++) {
                /* carry reset */
                VoutDtsCarry[i] = 0U;
                /* last reset */
                LastVoutDts[i] = 0U;
            }
            /* interrupt event open */
            FuncRetCode = AmbaDspInt_Open(pDspIntPort);
            if (FuncRetCode != OK_UL) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: amba mon int task port open");
            }
            /* ack */
            FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaMonInt_Ack);
            if (FuncRetCode != KAL_ERR_NONE) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: amba mon int task ack");
            }
        }

        /* interrupt even take */
        FuncRetCode = AmbaDspInt_Take(pDspIntPort, DspIntFlag, &ActualFlags, AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == OK_UL) {
            /* orc timer get */
            (void) AmbaSYS_GetOrcTimer(&Dts);
            /* vin int */
            if ((ActualFlags & DspVinIntFlag) > 0U) {
                for (i = 0U; i < AMBA_MON_NUM_VIN_CHANNEL; i++) {
                    /* vin id */
                    VinIsrRdy.VinId = i;
                    /* eof */
                    if ((ActualFlags & AmbaMonInt_VinFlag[i].Eof) > 0U) {
                        /* carry? */
                        if (LastEofDts[i] > Dts) {
                            EofDtsCarry[i]++;
                        }
                        /* dts */
                        VinIsrRdy.Dts = (((UINT64) EofDtsCarry[i]) << 32ULL) | ((UINT64) Dts);
                        VinIsrRdy.Pts = pDspIntPort->Pts.Eof[i];
                        MemInfo.Ctx.pVinIsrRdy = &VinIsrRdy;
                        FuncRetCode = AmbaMonVin_EofRdy(MemInfo.Ctx.pCvoid);
                        if (FuncRetCode != OK_UL) {
                            /* */
                        }
                        /* last dts update */
                        LastEofDts[i] = Dts;
                    }
                    /* sof */
                    if ((ActualFlags & AmbaMonInt_VinFlag[i].Sof) > 0U) {
                        /* carry? */
                        if (LastSofDts[i] > Dts) {
                            SofDtsCarry[i]++;
                        }
                        /* dts */
                        VinIsrRdy.Dts = (((UINT64) SofDtsCarry[i]) << 32ULL) | ((UINT64) Dts);
                        VinIsrRdy.Pts = pDspIntPort->Pts.Sof[i];
                        MemInfo.Ctx.pVinIsrRdy = &VinIsrRdy;
                        FuncRetCode = AmbaMonVin_SofRdy(MemInfo.Ctx.pCvoid);
                        if (FuncRetCode != OK_UL) {
                            /* */
                        }
                        /* last dts update */
                        LastSofDts[i] = Dts;
                    }
                }
            }
            /* vout int */
            if ((ActualFlags & DspVoutIntFlag) > 0U) {
                for (i = 0U; i < AMBA_MON_NUM_VOUT_CHANNEL; i++) {
                    /* vout id */
                    VoutIsrRdy.VoutId = i;
                    /* vout */
                    if ((ActualFlags & AmbaMonInt_VoutFlag[i]) > 0U) {
                        /* carry? */
                        if (LastVoutDts[i] > Dts) {
                            VoutDtsCarry[i]++;
                        }
                        /* dts */
                        VoutIsrRdy.Dts = (((UINT64) VoutDtsCarry[i]) << 32ULL) | ((UINT64) Dts);
                        VoutIsrRdy.Pts = pDspIntPort->Pts.Vout[i];
                        MemInfo.Ctx.pVoutIsrRdy = &VoutIsrRdy;
                        FuncRetCode = AmbaMonVout_IsrRdy(MemInfo.Ctx.pCvoid);
                        if (FuncRetCode != OK_UL) {
                            /* */
                        }
                        /* last dts updat */
                        LastVoutDts[i] = Dts;
                    }
                }
            }
        }
    }
}

