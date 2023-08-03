/**
*  @file SvcCalibModBpc.c
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
*  @details C file for SVC Module Storage - BPC
*
*/


#include "AmbaTypes.h"
#include "AmbaMisraFix.h"
#include "AmbaKAL.h"
#include "AmbaWrap.h"
#include "AmbaUtility.h"
#include "AmbaShell.h"
#include "AmbaEEPROM.h"

#include "AmbaCameraModule.h"

#ifdef CONFIG_BUILD_FOSS_LZ4
#include "AmbaLZ4_IF.h"
#define SVC_MODULE_LZ4_SEGMENT_SIZE (32768)
#endif

/* framework */
#include "SvcErrCode.h"
#include "SvcWrap.h"
#include "SvcLog.h"
#include "SvcCmd.h"

#include "SvcCalibMgr.h"
#include "SvcCalibModMgr.h"

static UINT32 SvcModule_BpcMemAddrComp(const UINT8 *pVal0, const UINT8 *pVal1);
static UINT32 SvcModule_BpcMemAddrDiff(const UINT8 *pVal0, const UINT8 *pVal1);
static UINT32 SvcModule_BpcMemQry(UINT32 *pMemSize);
static UINT32 SvcModule_BpcDataGet(UINT8 *pShadowBuf, UINT32 ShadowBufSize, UINT8 *pWorkBuf, UINT32 WorkBufSize, SVC_MODULE_BPC_TBL_DATA_s *pDataInfo);
static UINT32 SvcModule_BpcDataSet(UINT8 *pShadowBuf, UINT32 ShadowBufSize, SVC_MODULE_BPC_TBL_DATA_s *pDataInfo, UINT32 *pBufUsed);

#define SVC_LOG_MOD_BPC "STGMOD_BPC"
#define PRN_MOD_BPC_LOG        { SVC_WRAP_PRINT_s CalibModBpcPrint; AmbaSvcWrap_MisraMemset(&(CalibModBpcPrint), 0, sizeof(CalibModBpcPrint)); CalibModBpcPrint.Argc --; CalibModBpcPrint.pStrFmt =
#define PRN_MOD_BPC_ARG_UINT32 ; CalibModBpcPrint.Argc ++; CalibModBpcPrint.Argv[CalibModBpcPrint.Argc].Uint64   = (UINT64)((
#define PRN_MOD_BPC_ARG_POST   ))
#define PRN_MOD_BPC_NG         ; CalibModBpcPrint.Argc ++; SvcModule_BpcPrintLog(SVC_LOG_CAL_NG , &(CalibModBpcPrint)); }

static void SvcModule_BpcPrintLog(UINT32 LogLevel, SVC_WRAP_PRINT_s *pPrint)
{
    if (pPrint != NULL) {
        switch (LogLevel) {
        case SVC_LOG_CAL_OK :
            pPrint->pProc = SvcLog_OK;
            break;
        case SVC_LOG_CAL_NG :
            pPrint->pProc = SvcLog_NG;
            break;
        default :
            pPrint->pProc = SvcLog_DBG;
            break;
        }

        SvcWrap_Print(SVC_LOG_MOD_BPC, pPrint);
    }
}

static UINT32 SvcModule_BpcMemAddrComp(const UINT8 *pVal0, const UINT8 *pVal1)
{
    UINT32 RetVal = 0U;

#ifdef CONFIG_ARM64
    UINT64 Val_A = 0U, Val_B = 0U;
#else
    UINT32 Val_A = 0U, Val_B = 0U;
#endif

    AmbaMisra_TypeCast(&(Val_A), &(pVal0));
    AmbaMisra_TypeCast(&(Val_B), &(pVal1));

    if (Val_A >= Val_B) {
        RetVal = 1U;
    }

    return RetVal;
}

static UINT32 SvcModule_BpcMemAddrDiff(const UINT8 *pVal0, const UINT8 *pVal1)
{
    UINT32 RetVal = 0U;

#ifdef CONFIG_ARM64
    UINT64 Val_A = 0U, Val_B = 0U, Diff;
#else
    UINT32 Val_A = 0U, Val_B = 0U, Diff;
#endif

    AmbaMisra_TypeCast(&(Val_A), &(pVal0));
    AmbaMisra_TypeCast(&(Val_B), &(pVal1));

    if (Val_A > Val_B) {
        Diff = Val_A - Val_B;
    } else {
        Diff = Val_B - Val_A;
    }
    RetVal = (UINT32)Diff;

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcModule_BpcMemQry
 *
 *  @Description:: query svc storage module bpc working memory size
 *
 *  @Input      :: None
 *
 *  @Output     ::
 *      pMemSize : the pointer of eeprom memory size
 *
 *  @Return     ::
 *     SVC_OK(0) : Success
 *     SVC_NG(1) : Failure
\*-----------------------------------------------------------------------------------------------*/
static UINT32 SvcModule_BpcMemQry(UINT32 *pMemSize)
{
    if (pMemSize != NULL) {
        *pMemSize = (UINT32) sizeof(BAD_PIXELS_s) + SVC_MODULE_ITEM_BPC_SIZE;
#ifdef CONFIG_BUILD_FOSS_LZ4
        {
            UINT32 PackLz4WorkSize = 0U;
            (void) AmbaLZ4_BlendGetTblEncWorkSize(&PackLz4WorkSize);
            if (PackLz4WorkSize > 0U) {
                *pMemSize += PackLz4WorkSize;
            }
        }
#endif
    }

    return SVC_OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcModule_BpcDataGet
 *
 *  @Description:: get svc storage module bpc data
 *
 *  @Input      ::
 * ShadowBufBase : the storage module bpc shadow base
 * ShadowBufSize : the storage module bpc shadow size
 *   WorkBufBase : the storage module bpc working base
 *   WorkBufSize : the storage module bpc working size
 *     pDataInfo : the pointer of data info
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     SVC_OK(0) : Success
 *     SVC_NG(1) : Failure
\*-----------------------------------------------------------------------------------------------*/
static UINT32 SvcModule_BpcDataGet(UINT8 *pShadowBuf, UINT32 ShadowBufSize, UINT8 *pWorkBuf, UINT32 WorkBufSize, SVC_MODULE_BPC_TBL_DATA_s *pDataInfo)
{
    UINT32 RetVal = SVC_OK, PRetVal;
    const BAD_PIXELS_s *pBpcHeader = NULL;

    AmbaMisra_TypeCast(&(pBpcHeader), &(pShadowBuf));

    if (pBpcHeader == NULL) {
        RetVal = SVC_NG;
        PRN_MOD_BPC_LOG "Fail to get bpc data - invalid buffer base!" PRN_MOD_BPC_NG
    } else if ( (UINT32) sizeof(BAD_PIXELS_s) > ShadowBufSize ) {
        RetVal = SVC_NG;
        PRN_MOD_BPC_LOG "Fail to get bpc data - invalid buffer size!" PRN_MOD_BPC_NG
    } else if (pDataInfo == NULL) {
        RetVal = SVC_NG;
        PRN_MOD_BPC_LOG "Fail to get bpc data - output data info should not null!" PRN_MOD_BPC_NG
    } else if (pDataInfo->pSbpTbl == NULL) {
        RetVal = SVC_NG;
        PRN_MOD_BPC_LOG "Fail to get bpc data - output sbp table should not null!" PRN_MOD_BPC_NG
    } else {
        PRetVal = (UINT32) sizeof(BAD_PIXELS_s) + pBpcHeader->PackedSize;
        if (PRetVal > ShadowBufSize) {
            RetVal = SVC_NG;
            PRN_MOD_BPC_LOG "Fail to get bpc data - module shadow size small than bpc header + data! 0x%x < 0x%x"
                PRN_MOD_BPC_ARG_UINT32 ShadowBufSize PRN_MOD_BPC_ARG_POST
                PRN_MOD_BPC_ARG_UINT32 PRetVal       PRN_MOD_BPC_ARG_POST
            PRN_MOD_BPC_NG
        } else if (pDataInfo->SbpTblSize < pBpcHeader->MaskSize) {
            RetVal = SVC_NG;
            PRN_MOD_BPC_LOG "Fail to get bpc data - output bpc data size is small mask size! 0x%x < 0x%x"
                PRN_MOD_BPC_ARG_UINT32 pDataInfo->SbpTblSize PRN_MOD_BPC_ARG_POST
                PRN_MOD_BPC_ARG_UINT32 pBpcHeader->MaskSize  PRN_MOD_BPC_ARG_POST
            PRN_MOD_BPC_NG
        } else {
            UINT32 BpcHeaderSize = (UINT32) sizeof(BAD_PIXELS_s);
            UINT8 *pBpcData      = &(pShadowBuf[BpcHeaderSize]);

#ifdef CONFIG_BUILD_FOSS_LZ4
            PRetVal = 0U;
            if (0U != AmbaLZ4_BlendGetTblEncWorkSize(&PRetVal)) {
                RetVal = SVC_NG;
                PRN_MOD_BPC_LOG "Fail to get bpc data - query lz4 working size fail!" PRN_MOD_BPC_NG
            } else if (PRetVal > WorkBufSize) {
                RetVal = SVC_NG;
                PRN_MOD_BPC_LOG "Fail to get bpc data - current work size < lz4 requested working size! 0x%x < 0x%x"
                    PRN_MOD_BPC_ARG_UINT32 WorkBufSize PRN_MOD_BPC_ARG_POST
                    PRN_MOD_BPC_ARG_UINT32 PRetVal     PRN_MOD_BPC_ARG_POST
                PRN_MOD_BPC_NG
            } else if (pWorkBuf == NULL) {
                RetVal = SVC_NG;
                PRN_MOD_BPC_LOG "Fail to get bpc data - current lz4 work buffer should not null" PRN_MOD_BPC_NG
            } else {
                UINT8 *pBpcUnPackedBuf;

                AmbaMisra_TypeCast(&(pBpcUnPackedBuf), &(pDataInfo->pSbpTbl));

                PRetVal = AmbaLZ4_TblDecodeSegment(1, pBpcData, &pBpcUnPackedBuf, SVC_MODULE_LZ4_SEGMENT_SIZE, pWorkBuf);
                if (PRetVal != 0U) {
                    RetVal = SVC_NG;
                    PRN_MOD_BPC_LOG "Fail to get bpc data - UnPack BPC data fail! RetVal: 0x%x"
                        PRN_MOD_BPC_ARG_UINT32 PRetVal PRN_MOD_BPC_ARG_POST
                    PRN_MOD_BPC_NG
                }
            }
#else
            RetVal = AmbaWrap_memcpy(pDataInfo->pSbpTbl, pBpcData, pBpcHeader->MaskSize);
            AmbaMisra_TouchUnused(pWorkBuf);
            AmbaMisra_TouchUnused(&WorkBufSize);
#endif

            if (RetVal == SVC_OK) {
                pDataInfo->CalibGeo.StartX               = pBpcHeader->CalibVinSensorGeo.StartX              ;
                pDataInfo->CalibGeo.StartY               = pBpcHeader->CalibVinSensorGeo.StartY              ;
                pDataInfo->CalibGeo.Width                = pBpcHeader->CalibVinSensorGeo.Width               ;
                pDataInfo->CalibGeo.Height               = pBpcHeader->CalibVinSensorGeo.Height              ;
                pDataInfo->CalibGeo.HSubSample.FactorDen = pBpcHeader->CalibVinSensorGeo.HSubSample.FactorDen;
                pDataInfo->CalibGeo.HSubSample.FactorNum = pBpcHeader->CalibVinSensorGeo.HSubSample.FactorNum;
                pDataInfo->CalibGeo.VSubSample.FactorDen = pBpcHeader->CalibVinSensorGeo.VSubSample.FactorDen;
                pDataInfo->CalibGeo.VSubSample.FactorNum = pBpcHeader->CalibVinSensorGeo.VSubSample.FactorNum;
                pDataInfo->SbpTblSize                    = pBpcHeader->MaskSize;
            }

            AmbaMisra_TouchUnused(pBpcData);
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcModule_BpcDataSet
 *
 *  @Description:: set svc storage module bpc data
 *
 *  @Input      ::
 * ShadowBufBase : the storage module bpc shadow base
 * ShadowBufSize : the storage module bpc shadow size
 *     pDataInfo : the pointer of data info
 *
 *  @Output     ::
 *    pBufUsed : the buffer used size
 *
 *  @Return     ::
 *     SVC_OK(0) : Success
 *     SVC_NG(1) : Failure
\*-----------------------------------------------------------------------------------------------*/
static UINT32 SvcModule_BpcDataSet(UINT8 *pShadowBuf, UINT32 ShadowBufSize, SVC_MODULE_BPC_TBL_DATA_s *pDataInfo, UINT32 *pBufUsed)
{
    UINT32 RetVal = SVC_OK, PRetVal;
    BAD_PIXELS_s *pBpcHeader = NULL;
    AmbaMisra_TypeCast(&(pBpcHeader), &(pShadowBuf));

    if (pBpcHeader == NULL) {
        RetVal = SVC_NG;
        PRN_MOD_BPC_LOG "Fail to set bpc data - invalid buffer base!" PRN_MOD_BPC_NG
    } else if ( (UINT32) sizeof(BAD_PIXELS_s) > ShadowBufSize ) {
        RetVal = SVC_NG;
        PRN_MOD_BPC_LOG "Fail to set bpc data - invalid buffer size!" PRN_MOD_BPC_NG
    } else if (pDataInfo == NULL) {
        RetVal = SVC_NG;
        PRN_MOD_BPC_LOG "Fail to set bpc data - input data info should not null!" PRN_MOD_BPC_NG
    } else if (pDataInfo->pSbpTbl == NULL) {
        RetVal = SVC_NG;
        PRN_MOD_BPC_LOG "Fail to set bpc data - input sbp table should not null!" PRN_MOD_BPC_NG
    } else if ((pDataInfo->SbpTblSize + (UINT32)sizeof(BAD_PIXELS_s)) > ShadowBufSize) {
        PRetVal = pDataInfo->SbpTblSize + (UINT32)sizeof(BAD_PIXELS_s);
        RetVal = SVC_NG;
        PRN_MOD_BPC_LOG "Fail to set bpc data - shadow buffer is not enough to service it! 0x%x < 0x%x"
            PRN_MOD_BPC_ARG_UINT32 ShadowBufSize PRN_MOD_BPC_ARG_POST
            PRN_MOD_BPC_ARG_UINT32 PRetVal       PRN_MOD_BPC_ARG_POST
        PRN_MOD_BPC_NG
    } else {
        UINT32 BpcHeaderSize = (UINT32) sizeof(BAD_PIXELS_s);
        UINT8 *pBpcData      = &(pShadowBuf[BpcHeaderSize]);
        UINT32 BpcDataSize   = pDataInfo->SbpTblSize;
        UINT8 *pBpcWork      = &(pBpcData[BpcDataSize]);
        UINT32 BpcWorkSize   = 0U;

        if (0U < SvcModule_BpcMemAddrComp(&(pShadowBuf[ShadowBufSize]), pBpcWork)) {
            UINT32 PackedSize = 0U;

            BpcWorkSize = ShadowBufSize - SvcModule_BpcMemAddrDiff(pBpcWork, pShadowBuf);

#ifdef CONFIG_BUILD_FOSS_LZ4
            PRetVal = 0U;
            if (0U != AmbaLZ4_BlendGetTblEncWorkSize(&PRetVal)) {
                RetVal = SVC_NG;
                PRN_MOD_BPC_LOG "Fail to set bpc data - query lz4 working size fail!" PRN_MOD_BPC_NG
            } else if (PRetVal > BpcWorkSize) {
                RetVal = SVC_NG;
                PRN_MOD_BPC_LOG "Fail to set bpc data - current work size < lz4 requested working size! 0x%x < 0x%x"
                    PRN_MOD_BPC_ARG_UINT32 BpcWorkSize PRN_MOD_BPC_ARG_POST
                    PRN_MOD_BPC_ARG_UINT32 PRetVal     PRN_MOD_BPC_ARG_POST
                PRN_MOD_BPC_NG
            } else {
                const UINT8 *pBpcSrcData;
                INT32 BpcSrcDataSize;
                INT8 *pBpcPackedBuf = NULL;
                INT32 BpcPackedBufSize, BpcPackedSize = 0;
                const INT16 *pWorkSpace;

                // Configure the bpc table data
                AmbaMisra_TypeCast(&(pBpcSrcData),    &(pDataInfo->pSbpTbl));
                AmbaMisra_TypeCast(&(BpcSrcDataSize), &(pDataInfo->SbpTblSize));

                // Configure packed buffer
                AmbaMisra_TypeCast(&(pBpcPackedBuf),    &(pBpcData));
                AmbaMisra_TypeCast(&(BpcPackedBufSize), &(BpcDataSize));

                AmbaMisra_TypeCast(&(pWorkSpace), &(pBpcWork));

                PRetVal = AmbaLZ4_TblEncodeSegment(1, &(pBpcSrcData), &BpcSrcDataSize, pBpcPackedBuf, BpcPackedBufSize, &BpcPackedSize, SVC_MODULE_LZ4_SEGMENT_SIZE, pWorkSpace);
                if (PRetVal != 0U) {
                    RetVal = SVC_NG;
                    PRN_MOD_BPC_LOG "Fail to set bpc data - pack bpc data by lz4 fail! RetVal: 0x%x"
                        PRN_MOD_BPC_ARG_UINT32 PRetVal PRN_MOD_BPC_ARG_POST
                    PRN_MOD_BPC_NG
                } else {
                    AmbaMisra_TypeCast(&(PackedSize), &(BpcPackedSize));
                }
            }
#else
            {
                const void *pSrcData;
                void *pDstData;

                AmbaMisra_TypeCast(&(pSrcData), &(pDataInfo->pSbpTbl));
                AmbaMisra_TypeCast(&(pDstData), &(pBpcData));

                if ((pSrcData == NULL) || (pDstData == NULL)) {
                    RetVal = SVC_NG;
                    PRN_MOD_BPC_LOG "Fail to set bpc data - invalid src/dst buffer address!" PRN_MOD_BPC_NG
                } else {
                    PackedSize = pDataInfo->SbpTblSize;
                    AmbaSvcWrap_MisraMemcpy(pDstData, pSrcData, PackedSize);
                }
            }
#endif

            if (RetVal == SVC_OK) {

                AmbaSvcWrap_MisraMemset(pBpcHeader, 0, sizeof(BAD_PIXELS_s));
                pBpcHeader->CalibVinSensorGeo.StartX               = pDataInfo->CalibGeo.StartX              ;
                pBpcHeader->CalibVinSensorGeo.StartY               = pDataInfo->CalibGeo.StartY              ;
                pBpcHeader->CalibVinSensorGeo.Width                = pDataInfo->CalibGeo.Width               ;
                pBpcHeader->CalibVinSensorGeo.Height               = pDataInfo->CalibGeo.Height              ;
                pBpcHeader->CalibVinSensorGeo.HSubSample.FactorDen = pDataInfo->CalibGeo.HSubSample.FactorDen;
                pBpcHeader->CalibVinSensorGeo.HSubSample.FactorNum = pDataInfo->CalibGeo.HSubSample.FactorNum;
                pBpcHeader->CalibVinSensorGeo.VSubSample.FactorDen = pDataInfo->CalibGeo.VSubSample.FactorDen;
                pBpcHeader->CalibVinSensorGeo.VSubSample.FactorNum = pDataInfo->CalibGeo.VSubSample.FactorNum;
                pBpcHeader->OBEnable                               = (UINT8) pDataInfo->EnableOB;
                pBpcHeader->MaskSize                               = pDataInfo->SbpTblSize;
                pBpcHeader->PackedSize                             = PackedSize;

                if (pBufUsed != NULL) {
                    *pBufUsed = pBpcHeader->PackedSize + ( (UINT32) sizeof(BAD_PIXELS_s) );
                }
            }

            AmbaMisra_TouchUnused(&BpcWorkSize);
        }

        AmbaMisra_TouchUnused(pDataInfo);
        AmbaMisra_TouchUnused(pBpcData);
        AmbaMisra_TouchUnused(pBpcWork);
        AmbaMisra_TouchUnused(&PRetVal);
    }

    return RetVal;
}

/**
 * Storage module bpc command entry
 *
 * @param [in] CmdID  the bpc command id
 * @param [in] pData  the pointer of data
 * @param [in] Param1 no.01 parameter
 * @param [in] Param2 no.02 parameter
 * @param [in] Param3 no.03 parameter
 * @param [in] Param4 no.04 parameter
 * @return ErrCode SVC_OK(0)/SVC_NG(1)
 */
UINT32 SvcModule_BpcCmdFunc(UINT32 CmdID, void *pData, void *pParam1, void *pParam2, void *pParam3, void *pParam4)
{
    UINT32 RetVal = SVC_OK;
    UINT32 *pU32Val = NULL;

    if (CmdID == SVC_MODULE_ITEM_CMD_MEM_QRY) {
        UINT32 *pMemSize;
        AmbaMisra_TypeCast(&(pMemSize), &(pData));
        RetVal = SvcModule_BpcMemQry(pMemSize);
    } else if (CmdID == SVC_MODULE_ITEM_CMD_GET) {
        SVC_MODULE_ITEM_DATA_INFO_s *pDataInfo;
        AmbaMisra_TypeCast(&(pDataInfo), &(pData));

        if (pDataInfo != NULL) {
            SVC_MODULE_BPC_TBL_DATA_s *pBpcTblData;
            UINT8 *pShadowBuf    = NULL;
            UINT32 ShadowBufSize = 0U;
            UINT8 *pWorkBuf    = NULL;
            UINT32 WorkBufSize = 0U;

            AmbaMisra_TypeCast(&(pBpcTblData), &(pDataInfo->pData));

            AmbaMisra_TypeCast(&(pShadowBuf), &(pParam1));
            AmbaMisra_TypeCast(&(pU32Val), &(pParam2));
            if (pU32Val != NULL) { ShadowBufSize = *pU32Val; }
            AmbaMisra_TypeCast(&(pWorkBuf), &(pParam3));
            AmbaMisra_TypeCast(&(pU32Val), &(pParam4));
            if (pU32Val != NULL) { WorkBufSize = *pU32Val; }

            RetVal = SvcModule_BpcDataGet(pShadowBuf, ShadowBufSize, pWorkBuf, WorkBufSize, pBpcTblData);
        }

        AmbaMisra_TouchUnused(pDataInfo);
    } else if (CmdID == SVC_MODULE_ITEM_CMD_SET) {
        SVC_MODULE_ITEM_DATA_INFO_s *pDataInfo;
        UINT32 *pBufUsed;
        AmbaMisra_TypeCast(&(pDataInfo), &(pData));
        AmbaMisra_TypeCast(&(pBufUsed), &(pParam3));

        if (pDataInfo != NULL) {
            SVC_MODULE_BPC_TBL_DATA_s *pBpcTblData;
            UINT8 *pShadowBuf    = NULL;
            UINT32 ShadowBufSize = 0U;

            AmbaMisra_TypeCast(&(pBpcTblData), &(pDataInfo->pData));
            AmbaMisra_TypeCast(&(pShadowBuf), &(pParam1));
            AmbaMisra_TypeCast(&(pU32Val), &(pParam2));
            if (pU32Val != NULL) { ShadowBufSize = *pU32Val; }

            RetVal = SvcModule_BpcDataSet(pShadowBuf, ShadowBufSize, pBpcTblData, pBufUsed);
        }

        AmbaMisra_TouchUnused(pDataInfo);
    } else {
        AmbaMisra_TouchUnused(pData);
        AmbaMisra_TouchUnused(pParam1);
        AmbaMisra_TouchUnused(pParam2);
        AmbaMisra_TouchUnused(pParam3);
        AmbaMisra_TouchUnused(pParam4);
        AmbaMisra_TouchUnused(pU32Val);
    }

    return RetVal;
}





