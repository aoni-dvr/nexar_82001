/**
 *  @file SvcCvFlowAlgoUtil.c
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
 *  @details Implementation of SVC CV Algo Utility
 *
 */

#include "AmbaTypes.h"
#include "AmbaDef.h"

#include "AmbaDSP.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaNAND.h"
#include "AmbaUtility.h"
#include "AmbaWrap.h"
#include "AmbaCache.h"
#include "AmbaFS.h"
#include "AmbaMMU.h"
#include "AmbaDRAMC.h"
#include "AmbaMemProt.h"

#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcNvm.h"
#include "SvcResCfg.h"
#include "SvcMem.h"

#include "SvcCvAlgo.h"
#include "SvcCvFlow.h"
#include "SvcCvFlowUtil.h"
#include "SvcCvFlowAlgoUtil.h"

#include "cvapi_ambacv_flexidag.h"

/**
* Pack buffer address and size to flexidag memory block structure
* @param [in] pModuleName user defined string to be printed
* @param [in] pAddr buffer address
* @param [in] Size buffer size
* @param [out] pMemBlk pointer to flexidag memory block
*/
void SvcCvFlow_PackFdagMemBlkInfo(const char *pModuleName, void *pAddr, UINT32 Size, flexidag_memblk_t *pMemBlk)
{
    ULONG Vaddr, Paddr = 0U;
    pMemBlk->buffer_cacheable = 1;
    AmbaMisra_TypeCast(&Vaddr, &pAddr);
    AmbaMisra_TypeCast(&pMemBlk->pBuffer, &Vaddr);
    pMemBlk->buffer_size = Size;
    if (SVC_OK != SvcMem_VirtToPhys(Vaddr, &Paddr)) {
        SvcLog_NG(pModuleName, "SvcCvFlow_BufInfo2FdagMemBlk error", 0U, 0U);
    }
    pMemBlk->buffer_daddr = Paddr;
    pMemBlk->buffer_caddr = Paddr;

    #if defined(CONFIG_THREADX) && defined(CONFIG_ATT_MAP) && defined(AMBA_DRAM_CLIENT_ORCVP)
    {
        ULONG  CliAddr;

        if (AmbaMemProt_QueryIntmdAddr(Paddr, AMBA_DRAM_CLIENT_ORCVP, &CliAddr) == SVC_OK) {
            pMemBlk->buffer_caddr = CliAddr;
        }
    }
    #endif

    AmbaMisra_TouchUnused(pAddr);
}

/**
* Load binary for algorithm
* @param [in] pModuleName user defined string to be printed
* @param [in] pLoadInfo loading information
* @param [out] pAlgoOp pointer to algorithm operation structure
* @return 0-OK, 1-NG
*/
UINT32 SvcCvFlow_LoadAlgoBin(const char *pModuleName, const SVC_CV_FLOW_LOAD_INFO_s *pLoadInfo, SVC_CV_FLOW_ALGO_OP_INFO_s *pAlgoOp)
{
    UINT32 RetVal = SVC_OK;
    UINT32 i;
    const char *pFileName;
    SVC_CV_FLOW_BUF_INFO_s BufInfo;

    if ((NULL != pLoadInfo) && (NULL != pAlgoOp)) {
        if (pLoadInfo->NumBin <= SVC_CV_FLOW_MAX_BIN_NUM) {
            for (i = 0; i < pLoadInfo->NumBin; i++) {
                pFileName = pLoadInfo->pFileName[i];
                if (pLoadInfo->BinType[i] == SVC_CV_FLOW_FLEXI_BIN) {
                    if (i < SVC_CV_FLOW_MAX_ALGO_FLEXI_BIN) {
                        if (0 == SvcWrap_strcmp("NULL", pFileName)) {
                            continue;
                        } else {
                            RetVal = SvcCvFlow_AttachFlexiBin(pFileName, pLoadInfo->StorageType, &BufInfo);
                            if (SVC_OK == RetVal) {
                               SvcCvFlow_PackFdagMemBlkInfo(pModuleName, BufInfo.pAddr, BufInfo.Size, &pAlgoOp->FlexiBinBuf[i]);
                               pAlgoOp->FlexiBinNum++;
                               pAlgoOp->FlexiBinBits |= ((UINT32)1U << i);
                               //SvcLog_DBG(pModuleName, "FlexiBinNum(%d) FlexiBinBits(0x%x)", pOpInfo->FlexiBinNum, pOpInfo->FlexiBinBits);
                            }
                        }
                    } else {
                        SvcLog_NG(pModuleName, "Exceed max FlexiBin", i, 0U);
                    }
                } else if (pLoadInfo->BinType[i] == SVC_CV_FLOW_LICENSE_BIN) {
                    RetVal = SvcCvFlow_LoadBinFile(pFileName, pLoadInfo->StorageType, &pAlgoOp->LcsBinBuf);
                } else if (pLoadInfo->BinType[i] == SVC_CV_FLOW_EXT_BIN) {
                    if (i < SVC_CV_FLOW_MAX_ALGO_EXT_BIN) {
                        RetVal = SvcCvFlow_LoadBinFile(pFileName, pLoadInfo->StorageType, &pAlgoOp->ExtBinBuf[i]);
                        if (SVC_OK == RetVal) {
                            pAlgoOp->ExtBinNum++;
                        }
                    } else {
                        SvcLog_NG(pModuleName, "Exceed max ExtBin", i, 0U);
                    }
                } else {
                    /* Do nothing */
                }
            }
        }else {
            SvcLog_NG(pModuleName, "pLoadInfo->NumBin (%d) > SVC_CV_FLOW_MAX_BIN_NUM", pLoadInfo->NumBin, 0U);
        }
    } else {
        SvcLog_NG(pModuleName, "LoadAlgoBin invalid param", 0U, 0U);
    }

    return RetVal;
}

/**
* Load binary of SCA object
* @param [in] pModuleName user defined string to be printed
* @param [in] pAlgoInfo algorithm information
* @param [out] pAlgoOp pointer to algorithm operation structure
* @return 0-OK, 1-NG
*/
UINT32 SvcCvFlow_LoadAlgo(const char *pModuleName, const SVC_CV_FLOW_ALGO_INFO_s *pAlgoInfo, SVC_CV_FLOW_ALGO_OP_INFO_s *pAlgoOp)
{
    UINT32 RetVal = SVC_OK;
    UINT32 i, Num;
    const char *pFileName;
    SVC_CV_FLOW_BUF_INFO_s BufInfo;

    if ((NULL != pAlgoInfo) && (NULL != pAlgoOp)) {
        /* Attach FlexiBin */
        for (i = 0; i < SVC_CV_FLOW_MAX_ALGO_FLEXI_BIN; i++) {
            if (0U < (pAlgoInfo->FlexiBinBits & ((UINT32)1U << i))) {
                pFileName = pAlgoInfo->FlexiBinList[i];
                RetVal = SvcCvFlow_AttachFlexiBin(pFileName, SVC_CV_FLOW_STORAGE_TYPE_ROMFS, &BufInfo);
                if (SVC_OK == RetVal) {
                    SvcCvFlow_PackFdagMemBlkInfo(pModuleName, BufInfo.pAddr, BufInfo.Size, &pAlgoOp->FlexiBinBuf[i]);
                    pAlgoOp->FlexiBinNum++;
                    pAlgoOp->FlexiBinBits |= ((UINT32)1U << i);
                    //SvcLog_DBG(pModuleName, "FlexiBinNum(%d) FlexiBinBits(0x%x)", pAlgoOp->FlexiBinNum, pAlgoOp->FlexiBinBits);
                }
            }
        }

        /* Load License bin */
        Num = pAlgoInfo->NumLcsBin;
        for (i = 0; i < Num; i++) {
            pFileName = pAlgoInfo->LcsBinList[i];
            RetVal = SvcCvFlow_LoadBinFile(pFileName, SVC_CV_FLOW_STORAGE_TYPE_ROMFS, &pAlgoOp->LcsBinBuf);
        }

        /* Load extra bin */
        Num = pAlgoInfo->NumExtBin;
        for (i = 0; i < Num; i++) {
            pFileName = pAlgoInfo->ExtBinList[i];
            RetVal = SvcCvFlow_LoadBinFile(pFileName, SVC_CV_FLOW_STORAGE_TYPE_ROMFS, &pAlgoOp->ExtBinBuf[i]);
            if (SVC_OK == RetVal) {
                pAlgoOp->ExtBinNum++;
            }
        }
    } else {
        SvcLog_NG(pModuleName, "LoadAlgo invalid param", 0U, 0U);
    }

    return RetVal;
}

/**
* Query SCA object
* @param [in] pModuleName user defined string to be printed
* @param [in] pAlgoInfo algorithm information
* @param [in] pAlgoCfg configuration of algorithm
* @param [out] pAlgoOp pointer to algorithm operation structure
* @return 0-OK, 1-NG
*/
UINT32 SvcCvFlow_QueryAlgo(const char *pModuleName, const SVC_CV_FLOW_ALGO_INFO_s *pAlgoInfo, SVC_CV_FLOW_ALGO_QUERY_CFG_s *pAlgoCfg, SVC_CV_FLOW_ALGO_OP_INFO_s *pAlgoOp)
{
    UINT32 RetVal = SVC_OK;
    SVC_CV_ALGO_QUERY_CFG_s QueryCfg;
    SVC_CV_FLOW_BUF_INFO_s BufInfo;

    AmbaMisra_TouchUnused(pAlgoCfg);

    if ((NULL != pAlgoInfo) && (NULL != pAlgoCfg) && (NULL != pAlgoOp)) {
        if (NULL != pAlgoInfo->pAlgoObj) {
            /* Query */
            RetVal = AmbaWrap_memset(&QueryCfg, 0, sizeof(SVC_CV_ALGO_QUERY_CFG_s));
            if (SVC_OK != RetVal) {
                SvcLog_NG(pModuleName, "memset QueryCfg failed", 0U, 0U);
            }

            QueryCfg.pAlgoObj     = pAlgoInfo->pAlgoObj;
            QueryCfg.pExtQueryCfg = pAlgoCfg->pExtCfg;
            RetVal = SvcCvAlgo_Query(&pAlgoOp->AlgoHandle, &QueryCfg);
            if (SVC_OK == RetVal) {
                RetVal = SvcCvFlow_AllocWorkBuf(QueryCfg.TotalReqBufSz, &BufInfo);
                if (SVC_OK == RetVal) {
                    SvcCvFlow_PackFdagMemBlkInfo(pModuleName, BufInfo.pAddr, BufInfo.Size, &pAlgoOp->AlgoBuf);
                } else {
                    SvcLog_NG(pModuleName, "Alloc Algo buf failed", 0U, 0U);
                }
            } else {
                SvcLog_NG(pModuleName, "Algo Query error(0x%X)", RetVal, 0U);
            }
        } else {
            SvcLog_NG(pModuleName, "AlgoObj NULL", 0U, 0U);
        }
    } else {
        SvcLog_NG(pModuleName, "QueryAlgo invalid param", 0U, 0U);
    }

    return RetVal;
}

/**
* Create SCA object
* @param [in] pModuleName user defined string to be printed
* @param [in] pAlgoInfo algorithm information
* @param [in] pAlgoCfg configuration of algorithm
* @param [out] pAlgoOp pointer to algorithm operation structure
* @return 0-OK, 1-NG
*/
UINT32 SvcCvFlow_CreateAlgo(const char *pModuleName, const SVC_CV_FLOW_ALGO_INFO_s *pAlgoInfo, SVC_CV_FLOW_ALGO_CREATE_CFG_s *pAlgoCfg, SVC_CV_FLOW_ALGO_OP_INFO_s *pAlgoOp)
{
    UINT32 RetVal = SVC_OK;
    UINT32 i, idx;
    SVC_CV_ALGO_CREATE_CFG_s CreateCfg;

    AmbaMisra_TouchUnused(pAlgoCfg);

    if ((NULL != pAlgoInfo) && (NULL != pAlgoCfg) && (NULL != pAlgoOp)) {
        RetVal = AmbaWrap_memset(&CreateCfg, 0, sizeof(SVC_CV_ALGO_CREATE_CFG_s));
        if (SVC_OK != RetVal) {
            SvcLog_NG(pModuleName, "memset CreateCfg failed", 0U, 0U);
        }

        CreateCfg.NumFD         = pAlgoOp->FlexiBinNum;
        CreateCfg.pAlgoBuf      = &pAlgoOp->AlgoBuf;
        CreateCfg.pExtCreateCfg = pAlgoCfg->pExtCfg;
        //SvcLog_DBG(pModuleName, "CreateCfg NumFD(%d)", CreateCfg.NumFD, 0U);
        for (idx = 0; idx < SVC_CV_FLOW_MAX_ALGO_FLEXI_BIN; idx++) {
            if (0U < (pAlgoOp->FlexiBinBits & ((UINT32)1U << idx))) {
                CreateCfg.pBin[idx] = &pAlgoOp->FlexiBinBuf[idx];
                //SvcLog_DBG(pModuleName, "CreateCfg FBin(%d)", idx, 0U);
            }
        }

        RetVal = SvcCvAlgo_Create(&pAlgoOp->AlgoHandle, &CreateCfg);
        if (SVC_OK == RetVal) {
            pAlgoOp->OutputNum = (UINT8) CreateCfg.OutputNum;
            for (i = 0; i < CreateCfg.OutputNum; i++) {
                pAlgoOp->OutputSize[i] = CreateCfg.OutputSz[i];
            }
        } else {
            SvcLog_NG(pModuleName, "Algo Create error(0x%X)", RetVal, 0U);
        }
    } else {
       SvcLog_NG(pModuleName, "CreateAlgo invalid param", 0U, 0U);
    }

    return RetVal;
}

/**
* Delete SCA object
* @param [in] pModuleName user defined string to be printed
* @param [in] pAlgoCfg configuration of algorithm
* @param [in] pAlgoOp pointer to algorithm operation structure
* @return 0-OK, 1-NG
*/
UINT32 SvcCvFlow_DeleteAlgo(const char *pModuleName, SVC_CV_FLOW_ALGO_DELETE_CFG_s *pAlgoCfg, SVC_CV_FLOW_ALGO_OP_INFO_s *pAlgoOp)
{
    UINT32 RetVal = SVC_OK;
    SVC_CV_ALGO_DELETE_CFG_s DelCfg;

    AmbaMisra_TouchUnused(pAlgoCfg);

    if ((NULL != pAlgoOp) && (NULL != pAlgoCfg)) {
        DelCfg.pExtDeleteCfg = pAlgoCfg->pExtCfg;
        RetVal = SvcCvAlgo_Delete(&pAlgoOp->AlgoHandle, &DelCfg);
        if (SVC_OK != RetVal) {
            SvcLog_NG(pModuleName, "Algo Delete error(0x%X)", RetVal, 0U);
        }
    } else {
        SvcLog_NG(pModuleName, "DeleteAlgo invalid param", 0U, 0U);
    }

    return RetVal;
}
