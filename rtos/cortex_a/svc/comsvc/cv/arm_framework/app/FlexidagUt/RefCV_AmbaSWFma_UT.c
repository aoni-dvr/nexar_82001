/*
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
*/
#if defined(CONFIG_BUILD_CV_THREADX)
#include "AmbaIntrinsics.h"
#endif
#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaINT.h"
#include "AmbaPrint.h"
#include "AmbaUtility.h"
#include "AmbaMisraFix.h"
#include "AmbaMMU.h"
#include "AmbaFS.h"
#include "AmbaWrap.h"
#include "AmbaCache.h"
#include "AmbaShell.h"
#include "cvapi_ambacv_flexidag.h"
#include "cvapi_flexidag_unittest.h"
#include "cvapi_cavalry.h"
#include "cvapi_sw_fma_interface.h"

#define DC_S NULL
#define DC_U 0U

#if defined(CONFIG_QNX)
static INT32 mmemcmp(const void *s1, const void *s2, SIZE_t len)
{
    const unsigned char *s = s1;
    const unsigned char *d = s2;
    unsigned char sc;
    unsigned char dc;

    while (len--) {
        sc = *s++;
        dc = *d++;
        if (sc - dc)
            return (sc - dc);
    }

    return 0;
}
#endif


static INT32 Fma_Memcmp(const UINT8 *s1, const UINT8 *s2, SIZE_t n)
{
    INT32 value;
    const void* ps1;
    const void* ps2;
    (void)AmbaWrap_memcpy(&ps1, &s1, sizeof(void*));
    (void)AmbaWrap_memcpy(&ps2, &s2, sizeof(void*));
    //#if defined(CONFIG_QNX)
    //value = mmemcmp(ps1, ps2, n);
    //#else
    (void)AmbaWrap_memcmp(ps1, ps2, n, &value);
    //#endif
    return value;
}

static void _SaveBin(UINT8 *Buf, const char* Fn, const UINT32 Size)
{
    UINT32 Ret;
    AMBA_FS_FILE *Fp;
    UINT32 OpRes;

    Ret = AmbaFS_FileOpen(Fn, "wb", &Fp);
    if(Ret !=  0U) {
        AmbaPrint_PrintUInt5("_WriteOutputFile AmbaFS_FileOpen fail ", 0U, 0U, 0U, 0U, 0U);
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileWrite(Buf, 1U, Size, Fp, &OpRes);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("_WriteOutputFile AmbaFS_FileWrite fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileClose(Fp);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("_WriteOutputFile AmbaFS_FileClose fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
}

static void _DumpSWFmaResult(AMBA_FMA_RESULT_s *pFmaResult, const char* OutputPath1, const char* OutputPath2)
{
    UINT8 *pChar;
    pChar = (UINT8*)&pFmaResult->Idx[0];
    _SaveBin(pChar, OutputPath1, sizeof(pFmaResult->Idx));
    pChar = (UINT8*)&pFmaResult->MinDist[0];
    _SaveBin(pChar, OutputPath2, sizeof(pFmaResult->MinDist));
}

static void RefCV_AmbaSWFma_UT_Cmp(UINT32 Algo, const AMBA_FMA_RESULT_s *pFmaResult)
{
#define FILE_NAME_OUTPUT_LENGTH 128UL
    AMBA_FS_FILE *FdIn1 = NULL;
    UINT32 Ret;
#if defined(CONFIG_BUILD_CV_THREADX)
    char FnGdnIdx[FILE_NAME_OUTPUT_LENGTH] = "c:\\idx_out.bin";
    char FnGdnMDst[FILE_NAME_OUTPUT_LENGTH] = "c:\\min_out.bin";
    char FnGdnIdx1[FILE_NAME_OUTPUT_LENGTH] = "c:\\idx1_out.bin";
    char FnGdnMDst1[FILE_NAME_OUTPUT_LENGTH] = "c:\\min1_out.bin";
    char FnGdnIdx2[FILE_NAME_OUTPUT_LENGTH] = "c:\\idx2_out.bin";
    char FnGdnMDst2[FILE_NAME_OUTPUT_LENGTH] = "c:\\min2_out.bin";
#elif defined(CONFIG_QNX)
    char FnGdnIdx[FILE_NAME_OUTPUT_LENGTH] = "sd0/idx_out.bin";
    char FnGdnMDst[FILE_NAME_OUTPUT_LENGTH] = "sd0/min_out.bin";
    char FnGdnIdx1[FILE_NAME_OUTPUT_LENGTH] = "sd0/idx1_out.bin";
    char FnGdnMDst1[FILE_NAME_OUTPUT_LENGTH] = "sd0/min1_out.bin";
    char FnGdnIdx2[FILE_NAME_OUTPUT_LENGTH] = "sd0/idx2_out.bin";
    char FnGdnMDst2[FILE_NAME_OUTPUT_LENGTH] = "sd0/min2_out.bin";
#else
    char FnGdnIdx[FILE_NAME_OUTPUT_LENGTH] = "idx_out.bin";
    char FnGdnMDst[FILE_NAME_OUTPUT_LENGTH] = "min_out.bin";
    char FnGdnIdx1[FILE_NAME_OUTPUT_LENGTH] = "idx1_out.bin";
    char FnGdnMDst1[FILE_NAME_OUTPUT_LENGTH] = "min1_out.bin";
    char FnGdnIdx2[FILE_NAME_OUTPUT_LENGTH] = "idx2_out.bin";
    char FnGdnMDst2[FILE_NAME_OUTPUT_LENGTH] = "min2_out.bin";


#endif

    static UINT8 FmaGdnIdxData[4096] = {0};
    static UINT8 FmaGdnMDstData[2048] = {0};
    const UINT8 *pMisraCU8, *pMisraCU81;
    UINT32 NumSuccess;
    INT32 MDstRet = 0, IdxRet = 0;


    if(Algo == 0U) {
        Ret = AmbaFS_FileOpen(FnGdnIdx, "rb", &FdIn1);
    } else if(Algo == 1U) {
        Ret = AmbaFS_FileOpen(FnGdnIdx1, "rb", &FdIn1);
    } else if(Algo == 2U) {
        Ret = AmbaFS_FileOpen(FnGdnIdx2, "rb", &FdIn1);
    } else {
        AmbaPrint_PrintUInt5("Algo(%d) ERROR !", Algo, DC_U, DC_U, DC_U, DC_U);
        Ret = AMBA_FS_ERR_ARG;
    }
    if (Ret == AMBA_FS_ERR_NONE) {
        Ret = AmbaFS_FileRead(FmaGdnIdxData, sizeof(UINT8), sizeof(FmaGdnIdxData), FdIn1, &NumSuccess);
        Ret = AmbaFS_FileClose(FdIn1);
    } else {
        AmbaPrint_PrintStr5("AmbaFS_FileOpen(Idx) Fail", DC_S, DC_S, DC_S, DC_S, DC_S);
    }


    if(Algo == 0U) {
        Ret = AmbaFS_FileOpen(FnGdnMDst, "rb", &FdIn1);
    } else if(Algo == 1U) {
        Ret = AmbaFS_FileOpen(FnGdnMDst1, "rb", &FdIn1);
    } else if(Algo == 2U) {
        Ret = AmbaFS_FileOpen(FnGdnMDst2, "rb", &FdIn1);
    } else {
        AmbaPrint_PrintUInt5("Algo(%d) ERROR !", Algo, DC_U, DC_U, DC_U, DC_U);
        Ret = AMBA_FS_ERR_ARG;
    }
    if (Ret == AMBA_FS_ERR_NONE) {
        Ret = AmbaFS_FileRead(FmaGdnMDstData, sizeof(UINT8), sizeof(FmaGdnMDstData), FdIn1, &NumSuccess);
        Ret = AmbaFS_FileClose(FdIn1);
    } else {
        AmbaPrint_PrintStr5("AmbaFS_FileOpen(MDst) Fail", DC_S, DC_S, DC_S, DC_S, DC_S);
    }

    if (Ret == AMBA_FS_ERR_NONE) {
        AmbaMisra_TypeCast32(&pMisraCU8, &FmaGdnMDstData);
        AmbaMisra_TypeCast32(&pMisraCU81, &pFmaResult->MinDist[0]);
        MDstRet = Fma_Memcmp(pMisraCU8, pMisraCU81, sizeof(FmaGdnMDstData));
        AmbaMisra_TypeCast32(&pMisraCU8, &FmaGdnIdxData);
        AmbaMisra_TypeCast32(&pMisraCU81, &pFmaResult->Idx[0]);
        IdxRet = Fma_Memcmp(pMisraCU8, pMisraCU81, sizeof(FmaGdnIdxData));
        //MDstRet = memcmp(FmaGdnMDstData, (UINT8*)&FmaResult.MinDist[0], sizeof(FmaGdnMDstData));
        //IdxRet = memcmp(FmaGdnIdxData, (UINT8*)&FmaResult.Idx[0], sizeof(FmaGdnIdxData));

    } else {
        AmbaPrint_PrintStr5("Compare open Fail", DC_S, DC_S, DC_S, DC_S, DC_S);
    }

    if ((MDstRet == 0) && (IdxRet == 0)) {
        AmbaPrint_PrintStr5("============>  AmbaSWFma_UT  test PASS", DC_S, DC_S, DC_S, DC_S, DC_S);
    } else {
        AmbaPrint_PrintUInt5("============>  AmbaSWFma_UT  test ERROR , MDstRet:%d, IdxRet:%d ", (UINT32)MDstRet, (UINT32)IdxRet, DC_U, DC_U, DC_U);
    }
}

static const AMBA_FMA_HANDLE_s *CastFmaHdlr2Const(AMBA_FMA_HANDLE_s *pHdlr)
{
    const AMBA_FMA_HANDLE_s *pRval;
    AmbaMisra_TouchUnused(pHdlr);
    (void) AmbaWrap_memcpy(&pRval, &pHdlr, sizeof(pRval));
    return pRval;
}

static void RefCV_AmbaSWFma_UT_Run(const char* InputPath1, const char* InputPath2, const char* OutputPath1, const char* OutputPath2, UINT32 Algo)
{
    AMBA_FS_FILE *FdIn1 = NULL;
    static UINT8 FmaIn1Buf[1925120 ] = {0};
    static UINT8 FmaIn2Buf[1925120 ] = {0};
    static AMBA_FMA_HANDLE_s HandlerSwFma = {0};
    static const AMBA_FMA_HANDLE_s* HdlrSwFma;
    UINT32 NumSuccess;
    const AMBA_CV_FEX_DATA_s *pFlexStartAddr, *pFlexStartAddr1;
    UINT8 *pTemp;
    AMBA_FMA_CFG_s FmaCfg;
    AMBA_FMA_RESULT_s FmaResult;
    UINT32 Ret;
    UINT32 timeS = 0, timeE = 0;
    const UINT8* pPtrU32;
    UINT32 MisraCU32;
    HdlrSwFma = CastFmaHdlr2Const(&HandlerSwFma);
    FmaCfg.MatchAlgo = (UINT8)Algo;
    FmaCfg.DistTh = 6;

    AmbaPrint_PrintStr5("RefCV_AmbaSWFma_UT_Run START ! ", DC_S, DC_S, DC_S, DC_S, DC_S);
    AmbaMisra_TouchUnused(&OutputPath1);
    AmbaMisra_TouchUnused(&OutputPath2);

    Ret = AmbaFS_FileOpen(InputPath1, "rb", &FdIn1);
    if (Ret == AMBA_FS_ERR_NONE) {
        Ret = AmbaFS_FileRead(FmaIn1Buf, sizeof(UINT8), sizeof(FmaIn1Buf), FdIn1, &NumSuccess);
        if (Ret != AMBA_FS_ERR_NONE) {
            AmbaPrint_PrintStr5("AmbaFS_FileRead(%s) Fail", InputPath1, DC_S, DC_S, DC_S, DC_S);
        }
        Ret = AmbaFS_FileClose(FdIn1);
        if (Ret != AMBA_FS_ERR_NONE) {
            AmbaPrint_PrintStr5("AmbaFS_FileClose(%s) Fail", InputPath1, DC_S, DC_S, DC_S, DC_S);
        }
    } else {
        AmbaPrint_PrintStr5("AmbaFS_FileOpen(%s) Fail", InputPath1, DC_S, DC_S, DC_S, DC_S);
    }
    pPtrU32 = &FmaIn1Buf[0];
    AmbaMisra_TypeCast32(&pFlexStartAddr, &pPtrU32);
    //pFlexStartAddr = (AMBA_CV_FEX_DATA_s*)&FmaIn1Buf[0];

    Ret = AmbaFS_FileOpen(InputPath2, "rb", &FdIn1);
    if (Ret == AMBA_FS_ERR_NONE) {
        Ret = AmbaFS_FileRead(FmaIn2Buf, sizeof(UINT8), sizeof(FmaIn2Buf), FdIn1, &NumSuccess);
        if (Ret != AMBA_FS_ERR_NONE) {
            AmbaPrint_PrintStr5("AmbaFS_FileRead(%s) Fail", InputPath1, DC_S, DC_S, DC_S, DC_S);
        }
        Ret = AmbaFS_FileClose(FdIn1);
        if (Ret != AMBA_FS_ERR_NONE) {
            AmbaPrint_PrintStr5("AmbaFS_FileClose(%s) Fail", InputPath1, DC_S, DC_S, DC_S, DC_S);
        }
    } else {
        AmbaPrint_PrintStr5("AmbaFS_FileOpen(%s) Fail", InputPath2, DC_S, DC_S, DC_S, DC_S);
    }
    pPtrU32 = &FmaIn2Buf[0];
    AmbaMisra_TypeCast32(&pFlexStartAddr1, &pPtrU32);
    //pFlexStartAddr1 = (AMBA_CV_FEX_DATA_s*)&FmaIn2Buf[0];

    AmbaMisra_TypeCast32(&MisraCU32, &pFlexStartAddr);
    MisraCU32 = MisraCU32 + pFlexStartAddr->PrimaryList[0].DescriptorsOffset;
    AmbaMisra_TypeCast32(&pTemp, &MisraCU32);

    //pTemp = (UINT8*)(((UINT32)pFlexStartAddr)+pFlexStartAddr1->PrimaryList[0].DescriptorsOffset);
    (void)AmbaSWFma_Open(HdlrSwFma);
    (void)AmbaSWFma_Init(HdlrSwFma);
    (void)AmbaSWFma_Cfg(HdlrSwFma, &FmaCfg);
    (void)AmbaSWFma_Process(HdlrSwFma, pTemp);

    AmbaMisra_TypeCast32(&MisraCU32, &pFlexStartAddr1);
    MisraCU32 = MisraCU32 + pFlexStartAddr1->PrimaryList[0].DescriptorsOffset;
    AmbaMisra_TypeCast32(&pTemp, &MisraCU32);

    //pTemp = (UINT8*)(((UINT32)pFlexStartAddr1)+pFlexStartAddr1->PrimaryList[0].DescriptorsOffset);
    (void)AmbaKAL_GetSysTickCount(&timeS);
    (void)AmbaSWFma_Process(HdlrSwFma, pTemp);
    (void)AmbaKAL_GetSysTickCount(&timeE);
    (void)AmbaSWFma_GetResult(HdlrSwFma, &FmaResult);
    AmbaPrint_PrintUInt5("process time %d", timeE - timeS, DC_U, DC_U, DC_U,DC_U);
    _DumpSWFmaResult(&FmaResult, OutputPath1, OutputPath2);

    (void)AmbaSWFma_Close(HdlrSwFma);
    RefCV_AmbaSWFma_UT_Cmp(Algo, &FmaResult);
}


void RefCV_AmbaSWFma_UT(const char* InputPath1, const char* InputPath2, const char* OutputPath1, const char* OutputPath2, UINT32 Algo)
{

    RefCV_AmbaSWFma_UT_Run(InputPath1, InputPath2, OutputPath1, OutputPath2, Algo);
}

