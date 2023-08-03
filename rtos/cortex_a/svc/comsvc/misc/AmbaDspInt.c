/**
 *  @file AmbaDspInt.c
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
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Ambarella dsp interrupt
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"

#include "AmbaDSP.h"
#include "AmbaSYS.h"

#include "AmbaDSP_Event.h"

#include "AmbaDspInt.h"

#ifndef AMBAWRAP_H
#include "AmbaWrap.h"
#endif

#define OK_UL  ((UINT32) 0U)
#define NG_UL  ((UINT32) 1U)

//#define AMBA_DSP_INT_DEBUG_PRINT
#ifdef AMBA_DSP_INT_DEBUG_PRINT
#ifndef AMBA_PRINT_H
#include "AmbaPrint.h"
#endif
#endif

typedef struct /*_AMBA_DSP_INT_MEM_CTX_s_*/ {
    AMBA_DSP_INT_MEM_ADDR      Data;
    void                       *pVoid;
    char                       *pChar;
    const void                 *pCvoid;
    AMBA_DSP_RAW_DATA_RDY_s    *pRaw;
#ifdef AMBA_DSP_INT_DEBUG_PRINT
    AMBA_DSP_INT_PORT_s        *pPort;
#endif
} AMBA_DSP_INT_MEM_CTX_s;

typedef struct /*_AMBA_DSP_INT_MEM_s_*/ {
    AMBA_DSP_INT_MEM_CTX_s    Ctx;
} AMBA_DSP_INT_MEM_s;

static void AmbaDspInt_Task(UINT32 Param);
static void *AmbaDspInt_TaskEx(void *pParam);

static AMBA_DSP_INT_s AmbaDspInt = { .Magic = 0U, .pList = NULL };
static UINT32 AmbaDspInt_DummyFlag[AMBA_DSP_INT_TYPE_TOTAL] = {1U, 1U};

static UINT64 GNU_SECTION_NOZEROINIT AmbaDspInt_RawCapSeq[AMBA_NUM_VIN_CHANNEL];
static AMBA_DSP_INT_INFO_s GNU_SECTION_NOZEROINIT AmbaDspInt_Info;

/**
 *  DSP interrupt convert value to pointer
 *  @param [out] pParam1 pointer to dest parameter
 *  @param [in] Param2 src parameter
 *  @note this function is intended for internal use only
 */
static void AmbaDspInt_MemV2P(void *pParam1, const AMBA_DSP_INT_MEM_ADDR Param2)
{
    UINT32 FuncRetCode;

    FuncRetCode = AmbaWrap_memcpy(pParam1, &Param2, sizeof(AMBA_DSP_INT_MEM_ADDR));
    if (FuncRetCode != 0UL) {
        /* */
    }
}

/**
 *  DSP interrupt convert pointer to pointer
 *  @param [out] pParam1 pointer to dest parameter
 *  @param [in] pParam2 pointer to src parameter
 *  @note this function is intended for internal use only
 */
static void AmbaDspInt_MemP2P(void *pParam1, const void *pParam2)
{
    UINT32 FuncRetCode;

    FuncRetCode = AmbaWrap_memcpy(pParam1, &pParam2, sizeof(void *));
    if (FuncRetCode != 0UL) {
        /* */
    }
}

/**
 *  DSP interrupt convert pointer to value
 *  @param [out] pParam1 pointer to dest parameter
 *  @param [in] pParam2 pointer to src parameter
 *  @note this function is intended for internal use only
 */
static void AmbaDspInt_MemP2V(void *pParam1, const void *pParam2)
{
    UINT32 FuncRetCode;

    FuncRetCode = AmbaWrap_memcpy(pParam1, &pParam2, sizeof(void *));
    if (FuncRetCode != 0UL) {
        /* */
    }
}

/**
 * DSP interrupt task initialize function
 * @param [in] Priority priority for DSP interrupt task
 * @param [in] CoreInclusion Core param for setting SMP affinity
 * @return ErrorCode OK_UL(0)/NG_UL(1)
 *
 */
UINT32 AmbaDspInt_Init(UINT32 Priority, UINT32 CoreInclusion)
{
    UINT32 RetCode;

    RetCode = AmbaDspInt_SysInit();
    if (RetCode == OK_UL) {
        RetCode = AmbaDspInt_TaskCreate(Priority, CoreInclusion);
    }

    return RetCode;
}

/**
 * DSP interrupt task raw rdy
 * @param [in] pData pointer to the raw data ready information
 * @return error code
 */
UINT32 AmbaDspInt_Raw(const void *pData)
{
    AMBA_DSP_INT_MEM_s MemInfo;

    MemInfo.Ctx.pCvoid = pData;
    AmbaDspInt_MemP2P(&(MemInfo.Ctx.pRaw), MemInfo.Ctx.pCvoid);
    if (MemInfo.Ctx.pRaw->VinId < AMBA_NUM_VIN_CHANNEL) {
        AmbaDspInt_RawCapSeq[MemInfo.Ctx.pRaw->VinId] = MemInfo.Ctx.pRaw->CapSequence;
    }

    return OK;
}

/**
 * DSP interrupt reset
 * @note this function is intended for internal use only
 */
static void AmbaDspInt_Reset(void)
{
    UINT32 i;

    for (i = 0U; i < AMBA_NUM_VIN_CHANNEL; i++) {
        AmbaDspInt_RawCapSeq[i] = 0xFFFFFFFFFFFFFFFFULL;
        AmbaDspInt_Info.Vin.RawCapSeq[i] = 0U;
    }
}

/**
 * DSP Interrupt Task Ex
 * @param [in] pParam pointer to parameter
 * @note this function is intended for internal use only
 */
static void *AmbaDspInt_TaskEx(void *pParam)
{
    AMBA_DSP_INT_MEM_s MemInfo;

    MemInfo.Ctx.pVoid = pParam;
    AmbaDspInt_MemP2V(&(MemInfo.Ctx.Data), MemInfo.Ctx.pVoid);
    AmbaDspInt_Task((UINT32) MemInfo.Ctx.Data);

    return NULL;
}

/**
 * DSP Interrupt Task
 * @param [in] Param interrupt type parameter
 * @return none
 */
static void AmbaDspInt_Task(UINT32 Param)
{
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 DspIntVinFlag = 0U;
    UINT32 DspIntVoutFlag = 0U;
    UINT32 ActualFlags;

    AMBA_DSP_INT_PORT_s *pPort;

    UINT32 SofDtsCarry[AMBA_NUM_VIN_CHANNEL];
    UINT32 EofDtsCarry[AMBA_NUM_VIN_CHANNEL];
    UINT32 VoutDtsCarry[AMBA_NUM_VOUT_CHANNEL];

    UINT32 LastSofDts[AMBA_NUM_VIN_CHANNEL];
    UINT32 LastEofDts[AMBA_NUM_VIN_CHANNEL];
    UINT32 LastVoutDts[AMBA_NUM_VOUT_CHANNEL];

    UINT32 Dts;

    UINT64 SofPts[AMBA_NUM_VIN_CHANNEL];
    UINT64 EofPts[AMBA_NUM_VIN_CHANNEL];
    UINT64 VoutPts[AMBA_NUM_VOUT_CHANNEL];

    static const UINT32 AmbaDspInt_VinEofFlag[AMBA_NUM_VIN_CHANNEL] = {
#if defined(AMBA_DSP_VIN0_EOF)
        AMBA_DSP_VIN0_EOF,
#endif
#if defined(AMBA_DSP_VIN1_EOF)
        AMBA_DSP_VIN1_EOF,
#endif
#if defined(AMBA_DSP_VIN2_EOF)
        AMBA_DSP_VIN2_EOF,
#endif
#if defined(AMBA_DSP_VIN3_EOF)
        AMBA_DSP_VIN3_EOF,
#endif
#if defined(AMBA_DSP_VIN4_EOF)
        AMBA_DSP_VIN4_EOF,
#endif
#if defined(AMBA_DSP_VIN5_EOF)
        AMBA_DSP_VIN5_EOF,
#endif
#if defined(AMBA_DSP_VIN6_EOF)
        AMBA_DSP_VIN6_EOF,
#endif
#if defined(AMBA_DSP_VIN7_EOF)
        AMBA_DSP_VIN7_EOF,
#endif
#if defined(AMBA_DSP_VIN8_EOF)
        AMBA_DSP_VIN8_EOF,
#endif
#if defined(AMBA_DSP_VIN9_EOF)
        AMBA_DSP_VIN9_EOF,
#endif
#if defined(AMBA_DSP_VIN10_EOF)
        AMBA_DSP_VIN10_EOF,
#endif
#if defined(AMBA_DSP_VIN11_EOF)
        AMBA_DSP_VIN11_EOF,
#endif
#if defined(AMBA_DSP_VIN12_EOF)
        AMBA_DSP_VIN12_EOF,
#endif
#if defined(AMBA_DSP_VIN13_EOF)
        AMBA_DSP_VIN13_EOF,
#endif
    };
    static const UINT32 AmbaDspInt_VinSofFlag[AMBA_NUM_VIN_CHANNEL] = {
#if defined(AMBA_DSP_VIN0_SOF)
        AMBA_DSP_VIN0_SOF,
#endif
#if defined(AMBA_DSP_VIN1_SOF)
        AMBA_DSP_VIN1_SOF,
#endif
#if defined(AMBA_DSP_VIN2_SOF)
        AMBA_DSP_VIN2_SOF,
#endif
#if defined(AMBA_DSP_VIN3_SOF)
        AMBA_DSP_VIN3_SOF,
#endif
#if defined(AMBA_DSP_VIN4_SOF)
        AMBA_DSP_VIN4_SOF,
#endif
#if defined(AMBA_DSP_VIN5_SOF)
        AMBA_DSP_VIN5_SOF,
#endif
#if defined(AMBA_DSP_VIN6_SOF)
        AMBA_DSP_VIN6_SOF,
#endif
#if defined(AMBA_DSP_VIN7_SOF)
        AMBA_DSP_VIN7_SOF,
#endif
#if defined(AMBA_DSP_VIN8_SOF)
        AMBA_DSP_VIN8_SOF,
#endif
#if defined(AMBA_DSP_VIN9_SOF)
        AMBA_DSP_VIN9_SOF,
#endif
#if defined(AMBA_DSP_VIN10_SOF)
        AMBA_DSP_VIN10_SOF,
#endif
#if defined(AMBA_DSP_VIN11_SOF)
        AMBA_DSP_VIN11_SOF,
#endif
#if defined(AMBA_DSP_VIN12_SOF)
        AMBA_DSP_VIN12_SOF,
#endif
#if defined(AMBA_DSP_VIN13_SOF)
        AMBA_DSP_VIN13_SOF,
#endif
    };
    static const UINT32 AmbaDspInt_VoutFlag[AMBA_NUM_VOUT_CHANNEL] = {
#if defined(AMBA_DSP_VOUT0_INT)
        AMBA_DSP_VOUT0_INT,
#endif
#if defined(AMBA_DSP_VOUT1_INT)
        AMBA_DSP_VOUT1_INT,
#endif
#if defined(AMBA_DSP_VOUT2_INT)
        AMBA_DSP_VOUT2_INT,
#endif
    };

#if defined(AMBA_DSP_VIN0_SOF)
    DspIntVinFlag |= (AMBA_DSP_VIN0_SOF | AMBA_DSP_VIN0_EOF);
#endif
#if defined(AMBA_DSP_VIN1_SOF)
    DspIntVinFlag |= (AMBA_DSP_VIN1_SOF | AMBA_DSP_VIN1_EOF);
#endif
#if defined(AMBA_DSP_VIN2_SOF)
    DspIntVinFlag |= (AMBA_DSP_VIN2_SOF | AMBA_DSP_VIN2_EOF);
#endif
#if defined(AMBA_DSP_VIN3_SOF)
    DspIntVinFlag |= (AMBA_DSP_VIN3_SOF | AMBA_DSP_VIN3_EOF);
#endif
#if defined(AMBA_DSP_VIN4_SOF)
    DspIntVinFlag |= (AMBA_DSP_VIN4_SOF | AMBA_DSP_VIN4_EOF);
#endif
#if defined(AMBA_DSP_VIN5_SOF)
    DspIntVinFlag |= (AMBA_DSP_VIN5_SOF | AMBA_DSP_VIN5_EOF);
#endif
#if defined(AMBA_DSP_VIN6_SOF)
    DspIntVinFlag |= (AMBA_DSP_VIN6_SOF | AMBA_DSP_VIN6_EOF);
#endif
#if defined(AMBA_DSP_VIN7_SOF)
    DspIntVinFlag |= (AMBA_DSP_VIN7_SOF | AMBA_DSP_VIN7_EOF);
#endif
#if defined(AMBA_DSP_VIN8_SOF)
    DspIntVinFlag |= (AMBA_DSP_VIN8_SOF | AMBA_DSP_VIN8_EOF);
#endif
#if defined(AMBA_DSP_VIN9_SOF)
    DspIntVinFlag |= (AMBA_DSP_VIN9_SOF | AMBA_DSP_VIN9_EOF);
#endif
#if defined(AMBA_DSP_VIN10_SOF)
    DspIntVinFlag |= (AMBA_DSP_VIN10_SOF | AMBA_DSP_VIN10_EOF);
#endif
#if defined(AMBA_DSP_VIN11_SOF)
    DspIntVinFlag |= (AMBA_DSP_VIN11_SOF | AMBA_DSP_VIN11_EOF);
#endif
#if defined(AMBA_DSP_VIN12_SOF)
    DspIntVinFlag |= (AMBA_DSP_VIN12_SOF | AMBA_DSP_VIN12_EOF);
#endif
#if defined(AMBA_DSP_VIN13_SOF)
    DspIntVinFlag |= (AMBA_DSP_VIN13_SOF | AMBA_DSP_VIN13_EOF);
#endif

#if defined(AMBA_DSP_VOUT0_INT)
    DspIntVoutFlag |= AMBA_DSP_VOUT0_INT;
#endif
#if defined(AMBA_DSP_VOUT1_INT)
    DspIntVoutFlag |= AMBA_DSP_VOUT1_INT;
#endif
#if defined(AMBA_DSP_VOUT2_INT)
    DspIntVoutFlag |= AMBA_DSP_VOUT2_INT;
#endif

    /* dts reset */
    for (i = 0U; i < AMBA_NUM_VIN_CHANNEL; i++) {
        /* carry reset */
        SofDtsCarry[i] = 0U;
        EofDtsCarry[i] = 0U;
        /* last reset */
        LastSofDts[i] = 0U;
        LastEofDts[i] = 0U;
        /* pts reset */
        SofPts[i] = 0ULL;
        EofPts[i] = 0ULL;
    }
    for (i = 0U; i < AMBA_NUM_VOUT_CHANNEL; i++) {
        /* carry reset */
        VoutDtsCarry[i] = 0U;
        /* last reset */
        LastVoutDts[i] = 0U;
        /* pts reset */
        VoutPts[i] = 0ULL;
    }

    /* dsp interrupt reset */
    AmbaDspInt_Reset();

    while (AmbaDspInt_DummyFlag[Param] > 0U) {
        /*
         *  current status of ssp:
         *  vin/vout is in different group of event flag
         */
        if (Param == (UINT32) AMBA_DSP_INT_TYPE_VIN) {
            /* vin int ssp api */
            FuncRetCode = AmbaDSP_MainWaitVinInterrupt(DspIntVinFlag, &ActualFlags, 0xFFFFU);
        } else {
            /* vout int ssp api */
            FuncRetCode = AmbaDSP_MainWaitVoutInterrupt(DspIntVoutFlag, &ActualFlags, 0xFFFFU);
        }

        if (FuncRetCode == KAL_ERR_NONE) {
            if (ActualFlags > 0U) {
                /* orc timer get */
                (void) AmbaSYS_GetOrcTimer(&Dts);
                /* int type */
                if (Param == (UINT32) AMBA_DSP_INT_TYPE_VIN) {
                    /* vin int */
                    for (i = 0U; i < AMBA_NUM_VIN_CHANNEL; i++) {
                        /* eof */
                        if ((ActualFlags & AmbaDspInt_VinEofFlag[i]) > 0U) {
                            /* eof dts carry? */
                            if (LastEofDts[i] > Dts) {
                                EofDtsCarry[i]++;
                            }
                            /* eof pts */
                            EofPts[i] = (((UINT64) EofDtsCarry[i]) << 32ULL) | ((UINT64) Dts);
                            /* eof last dts update */
                            LastEofDts[i] = Dts;
                            /* raw cap seq update */
                            if (AmbaDspInt_RawCapSeq[i] != 0xFFFFFFFFFFFFFFFFULL) {
                                AmbaDspInt_Info.Vin.RawCapSeq[i] = (UINT32) ((AmbaDspInt_RawCapSeq[i] + 1ULL) % 0xFFFFFFFFULL);
                            } else {
                                AmbaDspInt_Info.Vin.RawCapSeq[i] = 0U;
                            }
                        }
                        /* sof */
                        if ((ActualFlags & AmbaDspInt_VinSofFlag[i]) > 0U) {
                            /* sof dts carry? */
                            if (LastSofDts[i] > Dts) {
                                SofDtsCarry[i]++;
                            }
                            /* sof pts */
                            SofPts[i] = (((UINT64) SofDtsCarry[i]) << 32ULL) | ((UINT64) Dts);
                            /* sof last dts update */
                            LastSofDts[i] = Dts;
                        }
                    }
                } else {
                    /* vout int */
                    for (i = 0U; i < AMBA_NUM_VOUT_CHANNEL; i++) {
                        if ((ActualFlags & AmbaDspInt_VoutFlag[i]) > 0U) {
                            /* vout dts carry? */
                            if (LastVoutDts[i] > Dts) {
                                VoutDtsCarry[i]++;
                            }
                            /* vout pts */
                            VoutPts[i] = (((UINT64) VoutDtsCarry[i]) << 32ULL) | ((UINT64) Dts);
                            /* vout last dts update */
                            LastVoutDts[i] = Dts;
                        }
                    }
                }
                /* list mutex take */
                FuncRetCode = AmbaKAL_MutexTake(&(AmbaDspInt.Mutex), AMBA_KAL_WAIT_FOREVER);
                if(FuncRetCode == KAL_ERR_NONE) {
                    /* port list get */
                    pPort = AmbaDspInt.pList;
                    while (pPort != NULL) {
                        /* valid check */
                        if (pPort->Magic != 0xCafeU) {
                            /* invalid */
                            break;
                        }
                        /* event send */
                        if (Param == (UINT32) AMBA_DSP_INT_TYPE_VIN) {
                            /* vin int */
                            for (i = 0U; i < AMBA_NUM_VIN_CHANNEL; i++) {
                                /* eof */
                                if ((ActualFlags & AmbaDspInt_VinEofFlag[i]) > 0U) {
                                    /* eof pts */
                                    AmbaDspInt_Info.Pts.Eof[i] = EofPts[i];
                                    pPort->Pts.Eof[i] = EofPts[i];
                                }
                                /* sof */
                                if ((ActualFlags & AmbaDspInt_VinSofFlag[i]) > 0U) {
                                    /* sof pts */
                                    AmbaDspInt_Info.Pts.Sof[i] = SofPts[i];
                                    pPort->Pts.Sof[i] = SofPts[i];
                                }
                            }
                            /* vin cb */
                            if (pPort->pFunc != NULL) {
                                pPort->pFunc(ActualFlags, &AmbaDspInt_Info);
                            }
                            /* vin: bit[0:15] */
                            (void) AmbaKAL_EventFlagSet(&(pPort->Event), ActualFlags);
                        } else {
                            /* vout int */
                            for (i = 0U; i < AMBA_NUM_VOUT_CHANNEL; i++) {
                                /* vout */
                                if ((ActualFlags & AmbaDspInt_VoutFlag[i]) > 0U) {
                                    /* vout pts */
                                    AmbaDspInt_Info.Pts.Vout[i] = VoutPts[i];
                                    pPort->Pts.Vout[i] = VoutPts[i];
                                }
                            }
                            /* vout cb */
                            if (pPort->pFunc != NULL) {
                                pPort->pFunc(ActualFlags << AMBA_DSP_INT_VOUT_FLAG_SHIFT, &AmbaDspInt_Info);
                            }
                            /* vout: bit[16:31] */
                            (void) AmbaKAL_EventFlagSet(&(pPort->Event), (ActualFlags << AMBA_DSP_INT_VOUT_FLAG_SHIFT));
                        }
                        /* next */
                        pPort = pPort->Link.Down;
                    }
                    /* list mutex give */
                    (void) AmbaKAL_MutexGive(&(AmbaDspInt.Mutex));
                }
            }
        } else {
            /* avoid busy block if there are errors */
            (void)AmbaKAL_TaskSleep(5U);
        }
    }
}

/**
 * DSP Interrupt Port List Check
 * @param [in] pPort pointer to interrput port
 * @return ErrorCode OK_UL(0)/NG_UL(1)
 */
static UINT32 AmbaDspInt_PortListCheck(const AMBA_DSP_INT_PORT_s *pPort)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 FindFlag = 0U;

    const AMBA_DSP_INT_PORT_s *pPortList;

    /* mutex take */
    FuncRetCode = AmbaKAL_MutexTake(&(AmbaDspInt.Mutex), AMBA_KAL_WAIT_FOREVER);
    if (FuncRetCode == KAL_ERR_NONE) {
        /* port list get */
        pPortList = AmbaDspInt.pList;
        /* search port by address */
        while (pPortList != NULL) {
            /* compare */
            if (pPortList == pPort) {
                FindFlag = 1U;
                break;
            }
            /* next port */
            pPortList = pPortList->Link.Down;
        }
        /* mutex give */
        (void) AmbaKAL_MutexGive(&(AmbaDspInt.Mutex));
    }

    /* not found? */
    if (FindFlag == 0U) {
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 * DSP Interrupt Open Ex
 * @param [in] pPort pointer to dsp interrupt port
 * @param [in] Priority pripority
 * @param [in] pFunc callback function
 * @return ErrorCode OK_UL(0)/NG_UL(1)
 */
UINT32 AmbaDspInt_OpenEx(AMBA_DSP_INT_PORT_s *pPort, UINT32 Priority, AMBA_DSP_INT_CB_f pFunc)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    AMBA_DSP_INT_PORT_s *pList;
#ifdef AMBA_DSP_INT_DEBUG_PRINT
    AMBA_DSP_INT_MEM_s MemInfo;
#endif
    if (AmbaDspInt.Magic != 0xCafeU) {
        /* init not yet */
        if (pPort != NULL) {
            /* force to reset */
            pPort->Magic = 0U;
        }
        RetCode = NG_UL;
    }

    if ((RetCode == OK_UL) && (pPort != NULL)) {
        /* list check */
        FuncRetCode = AmbaDspInt_PortListCheck(pPort);
        if (FuncRetCode == OK_UL) {
            /* already in list, re-open? */
            RetCode = NG_UL;
        } else {
            /* not in list, force to reset */
            pPort->Magic = 0U;
        }
    }

    if ((RetCode == OK_UL) && (pPort != NULL) && (pPort->Magic != 0xCafeU)) {
        /* list mutex take */
        FuncRetCode = AmbaKAL_MutexTake(&(AmbaDspInt.Mutex), AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            /* port event create */
            FuncRetCode = AmbaKAL_EventFlagCreate(&(pPort->Event), NULL);
            if (FuncRetCode == KAL_ERR_NONE) {
                /* link */
                pPort->Link.Up = NULL;
                pPort->Link.Down = NULL;
                /* magic */
                pPort->Magic = 0xCafeU;
                pPort->Priority = Priority;
                pPort->pFunc = pFunc;
#ifdef AMBA_DSP_INT_DEBUG_PRINT
                MemInfo.Ctx.pPort = pPort;
                AmbaDspInt_MemP2V(&(MemInfo.Ctx.Data), MemInfo.Ctx.pPort);
                AmbaPrint_PrintUInt5("port 0x%08lx", (UINT32) MemInfo.Ctx.Data, 0U, 0U, 0U, 0U);
                AmbaPrint_PrintUInt5("priority 0x%08lx", MemInfo.Ctx.pPort->Priority, 0U, 0U, 0U, 0U);
#endif
                /* list get */
                pList = AmbaDspInt.pList;
                if (pList == NULL) {
                    /* root */
                    AmbaDspInt.pList = pPort;
#ifdef AMBA_DSP_INT_DEBUG_PRINT
                    MemInfo.Ctx.pPort = AmbaDspInt.pList;
                    AmbaDspInt_MemP2V(&(MemInfo.Ctx.Data), MemInfo.Ctx.pPort);
                    AmbaPrint_PrintUInt5("root 0x%08lx", (UINT32) MemInfo.Ctx.Data, 0U, 0U, 0U, 0U);
#endif
                } else {
                    /* root check */
                    if (pList->Priority > pPort->Priority) {
                        /* root add */
                        AmbaDspInt.pList = pPort;
                        pPort->Link.Down = pList;
                        pList->Link.Up = pPort;
#ifdef AMBA_DSP_INT_DEBUG_PRINT
                        MemInfo.Ctx.pPort = AmbaDspInt.pList;
                        AmbaDspInt_MemP2V(&(MemInfo.Ctx.Data), MemInfo.Ctx.pPort);
                        AmbaPrint_PrintUInt5("root add 0x%08lx", (UINT32) MemInfo.Ctx.Data, 0U, 0U, 0U, 0U);
                        MemInfo.Ctx.pPort = pPort->Link.Down;
                        AmbaDspInt_MemP2V(&(MemInfo.Ctx.Data), MemInfo.Ctx.pPort);
                        AmbaPrint_PrintUInt5("next 0x%08lx", (UINT32) MemInfo.Ctx.Data, 0U, 0U, 0U, 0U);
#endif
                    } else {
                        /* list find (priority) */
                        while ((pList->Priority <= pPort->Priority) && (pList->Link.Down != NULL)) {
                            pList = pList->Link.Down;
                        }
                        /* port add */
                        if ((pList->Link.Down == NULL) && (pList->Priority <= pPort->Priority)) {
                            /* tailer */
                            pList->Link.Down = pPort;
                            pPort->Link.Up = pList;
#ifdef AMBA_DSP_INT_DEBUG_PRINT
                            MemInfo.Ctx.pPort = pList->Link.Down;
                            AmbaDspInt_MemP2V(&(MemInfo.Ctx.Data), MemInfo.Ctx.pPort);
                            AmbaPrint_PrintUInt5("tailer add 0x%08lx", (UINT32) MemInfo.Ctx.Data, 0U, 0U, 0U, 0U);
                            MemInfo.Ctx.pPort = pList;
                            AmbaDspInt_MemP2V(&(MemInfo.Ctx.Data), MemInfo.Ctx.pPort);
                            AmbaPrint_PrintUInt5("previous 0x%08lx", (UINT32) MemInfo.Ctx.Data, 0U, 0U, 0U, 0U);
#endif
                        } else {
                            /* add */
                            pPort->Link.Up = pList->Link.Up;
                            pPort->Link.Up->Link.Down = pPort;
                            pPort->Link.Down = pList;
                            pList->Link.Up = pPort;
#ifdef AMBA_DSP_INT_DEBUG_PRINT
                            MemInfo.Ctx.pPort = pList->Link.Up;
                            AmbaDspInt_MemP2V(&(MemInfo.Ctx.Data), MemInfo.Ctx.pPort);
                            AmbaPrint_PrintUInt5("list add 0x%08lx", (UINT32) MemInfo.Ctx.Data, 0U, 0U, 0U, 0U);
                            MemInfo.Ctx.pPort = pPort->Link.Up;
                            AmbaDspInt_MemP2V(&(MemInfo.Ctx.Data), MemInfo.Ctx.pPort);
                            AmbaPrint_PrintUInt5("previous 0x%08lx", (UINT32) MemInfo.Ctx.Data, 0U, 0U, 0U, 0U);
                            MemInfo.Ctx.pPort = pPort->Link.Down;
                            AmbaDspInt_MemP2V(&(MemInfo.Ctx.Data), MemInfo.Ctx.pPort);
                            AmbaPrint_PrintUInt5("next 0x%08lx", (UINT32) MemInfo.Ctx.Data, 0U, 0U, 0U, 0U);
#endif
                        }
                    }
                }
            } else {
                /* event fail */
                RetCode = NG_UL;
            }
            /* list mutex give */
            (void) AmbaKAL_MutexGive(&(AmbaDspInt.Mutex));
        }
    } else {
        /* null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 * DSP Interrupt Open
 * @param [in] pPort pointer to dsp interrupt port
 * @return ErrorCode OK_UL(0)/NG_UL(1)
 */
UINT32 AmbaDspInt_Open(AMBA_DSP_INT_PORT_s *pPort)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    FuncRetCode = AmbaDspInt_OpenEx(pPort, 0xFFU, NULL);
    if (FuncRetCode != OK_UL) {
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 * DSP Interrupt Mutex Take
 * @param [in] pPort pointer to dsp interrupt port
 * @param [in] Flag waiting event flag
 * @param [in] pActualFlag pointer to received actual flag
 * @param [in] Timeout timeout setting
 * @return ErrorCode OK_UL(0)/NG_UL(1)
 */
UINT32 AmbaDspInt_Take(AMBA_DSP_INT_PORT_s *pPort, UINT32 Flag, UINT32 *pActualFlag, UINT32 Timeout)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    if ((pPort != NULL) && (pPort->Magic == 0xCafeU)) {
        FuncRetCode = AmbaKAL_EventFlagGet(&(pPort->Event), Flag, AMBA_KAL_FLAGS_ANY, AMBA_KAL_FLAGS_CLEAR_AUTO, pActualFlag, Timeout);
        if (FuncRetCode != KAL_ERR_NONE) {
            RetCode = NG_UL;
        }
    } else {
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 * DSP Interrupt Mutex Give
 * @param [in] pPort pointer to dsp interrupt port
 * @param [in] Flags mutex event flag
 * @return ErrorCode OK_UL(0)/NG_UL(1)
 */
UINT32 AmbaDspInt_AuxGive(AMBA_DSP_INT_PORT_s *pPort, UINT32 Flags)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    const AMBA_DSP_INT_PORT_s *pList;

    if ((pPort != NULL) && (pPort->Magic == 0xCafeU)) {
        /* list mutex take */
        FuncRetCode = AmbaKAL_MutexTake(&(AmbaDspInt.Mutex), AMBA_KAL_WAIT_FOREVER);
        if(FuncRetCode == KAL_ERR_NONE) {
            /* port list get */
            pList = AmbaDspInt.pList;
            while ((pList != NULL) && (pList->Magic == 0xCafeU)) {
                /* port check */
                if (pList == pPort) {
                    /* event give */
                    (void) AmbaKAL_EventFlagSet(&(pPort->Event), Flags);
                    break;
                }
                /* next */
                pList = pList->Link.Down;
            }
            /* list mutex give */
            (void) AmbaKAL_MutexGive(&(AmbaDspInt.Mutex));
        } else {
            /* mutex fail */
            RetCode = NG_UL;
        }
    } else {
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 * DSP Interrupt Close
 * @param [in] pPort pointer to dsp interrupt port
 * @return ErrorCode OK_UL(0)/NG_UL(1)
 */
UINT32 AmbaDspInt_Close(AMBA_DSP_INT_PORT_s *pPort)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    if ((pPort != NULL) && (pPort->Magic == 0xCafeU)) {
        /* list mutex take */
        FuncRetCode = AmbaKAL_MutexTake(&(AmbaDspInt.Mutex), AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            if (AmbaDspInt.pList != NULL) {
                /* up link check */
                if (pPort->Link.Up == NULL) {
                    /* root */
                    AmbaDspInt.pList = pPort->Link.Down;
                } else {
                    /* linker */
                    pPort->Link.Up->Link.Down = pPort->Link.Down;
                }
                /* down link check */
                if (pPort->Link.Down != NULL) {
                    pPort->Link.Down->Link.Up = pPort->Link.Up;
                }

                /* magic clear */
                pPort->Magic = 0U;

                /* event delete */
                (void) AmbaKAL_EventFlagDelete(&(pPort->Event));
            }

            /* list mutex give */
            (void) AmbaKAL_MutexGive(&(AmbaDspInt.Mutex));
        } else {
            /* list mutex fail */
            RetCode = NG_UL;
        }
    } else {
        /* null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 * DSP interrupt initialize function
 * @return ErrorCode OK_UL(0)/NG_UL(1)
 */
UINT32 AmbaDspInt_SysInit(void)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    if (AmbaDspInt.Magic != 0xCafeU) {
        /* mutex create */
        FuncRetCode = AmbaKAL_MutexCreate(&(AmbaDspInt.Mutex), NULL);
        if (FuncRetCode != KAL_ERR_NONE) {
            RetCode = NG_UL;
        }

        /* magic update */
        AmbaDspInt.Magic = 0xCafeU;
    }

    for (i = 0U; i < (UINT32) AMBA_DSP_INT_TYPE_TOTAL; i++) {
        AmbaDspInt_DummyFlag[i] = 1U;
    }

    return RetCode;
}

/**
 * DSP interrupt task creation
 * @param [in] Priority priority for DSP interrupt task
 * @param [in] CoreInclusion Core param for setting SMP affinity
 * @return ErrorCode OK_UL(0)/NG_UL(1)
 */
UINT32 AmbaDspInt_TaskCreate(UINT32 Priority, UINT32 CoreInclusion)
{
#define AMBA_DSP_INT_TASK_STACK_SIZE    0xA000
    static AMBA_KAL_TASK_t AmbaDspInt_TaskId[AMBA_DSP_INT_TYPE_TOTAL];
    static UINT8 GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE AmbaDspInt_TaskStack[AMBA_DSP_INT_TYPE_TOTAL][AMBA_DSP_INT_TASK_STACK_SIZE];
    static const char AmbaDspInt_VinTaskName[] = "AmbaDspIntVin";
    static const char AmbaDspInt_VoutTaskName[] = "AmbaDspIntVout";
    static const char * const AmbaDspInt_TaskName[AMBA_DSP_INT_TYPE_TOTAL] = { AmbaDspInt_VinTaskName, AmbaDspInt_VoutTaskName };

    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    if (AmbaDspInt.Magic == 0xCafeU) {
        for (i = 0U; i < (UINT32) AMBA_DSP_INT_TYPE_TOTAL; i++) {
            /* task create */
            {
                AMBA_DSP_INT_MEM_s MemInfo;

                /* arg */
                MemInfo.Ctx.Data = i;
                AmbaDspInt_MemV2P(&(MemInfo.Ctx.pVoid), MemInfo.Ctx.Data);
                /* name */
                AmbaDspInt_MemP2P(&(MemInfo.Ctx.pChar), AmbaDspInt_TaskName[i]);
                /* task */
                FuncRetCode = AmbaKAL_TaskCreate(&(AmbaDspInt_TaskId[i]),
                                                 MemInfo.Ctx.pChar,
                                                 Priority,
                                                 AmbaDspInt_TaskEx,
                                                 MemInfo.Ctx.pVoid,
                                                 AmbaDspInt_TaskStack[i],
                                                 AMBA_DSP_INT_TASK_STACK_SIZE,
                                                 AMBA_KAL_DONT_START);
            }
            if (FuncRetCode == OK) {
                /* core */
                FuncRetCode = AmbaKAL_TaskSetSmpAffinity(&(AmbaDspInt_TaskId[i]), CoreInclusion);
                if (FuncRetCode != KAL_ERR_NONE) {
                    RetCode = NG_UL;
                }
                /* resume */
                FuncRetCode = AmbaKAL_TaskResume(&(AmbaDspInt_TaskId[i]));
                if (FuncRetCode != KAL_ERR_NONE) {
                    RetCode = NG_UL;
                }
            } else {
                RetCode = NG_UL;
            }
        }
    }

    return RetCode;
}
