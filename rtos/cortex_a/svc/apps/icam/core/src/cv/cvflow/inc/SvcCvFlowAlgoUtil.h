/**
 *  @file SvcCvFlowAlgoUtil.h
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
 *  @details Header of SvcCvFlow Utility of SVC CV Algo
 *
 */

#ifndef SVC_CV_FLOW_ALGO_UTIL_H
#define SVC_CV_FLOW_ALGO_UTIL_H

#include "SvcCvFlowUtil.h"

#define SVC_CV_FLOW_MAX_ALGO_PER_GRP            (5U)
#define SVC_CV_FLOW_MAX_ALGO_FLEXI_BIN          (10U)
#define SVC_CV_FLOW_MAX_ALGO_EXT_BIN            (1U)
#define SVC_CV_FLOW_MAX_OUTPUT                  (8U)    /* FLEXIDAG_MAX_OUTPUTS */

typedef struct {
    SVC_CV_ALGO_OBJ_s       *pAlgoObj;
    UINT8                   NumFlexiBin;
    UINT32                  FlexiBinBits;
    const char              *FlexiBinList[SVC_CV_FLOW_MAX_ALGO_FLEXI_BIN];
    UINT8                   NumLcsBin;
    const char              *LcsBinList[1];
    UINT8                   NumExtBin;
    UINT8                   ExtBinType[SVC_CV_FLOW_MAX_ALGO_EXT_BIN];
    const char              *ExtBinList[SVC_CV_FLOW_MAX_ALGO_EXT_BIN];
} SVC_CV_FLOW_ALGO_INFO_s;

typedef struct {
    UINT8                   NumAlgo;
    SVC_CV_FLOW_ALGO_INFO_s AlgoInfo[SVC_CV_FLOW_MAX_ALGO_PER_GRP];
} SVC_CV_FLOW_ALGO_GROUP_s;

typedef struct {
    SVC_CV_ALGO_HANDLE_s    AlgoHandle;
    flexidag_memblk_t       AlgoBuf;
    UINT8                   FlexiBinNum;
    UINT32                  FlexiBinBits;
    flexidag_memblk_t       FlexiBinBuf[SVC_CV_FLOW_MAX_ALGO_FLEXI_BIN];
    SVC_CV_FLOW_BUF_INFO_s  LcsBinBuf;
    UINT8                   OutputNum;
    UINT32                  OutputSize[SVC_CV_FLOW_MAX_OUTPUT];

    UINT8                   ExtBinNum;
    SVC_CV_FLOW_BUF_INFO_s  ExtBinBuf[SVC_CV_FLOW_MAX_ALGO_EXT_BIN];
} SVC_CV_FLOW_ALGO_OP_INFO_s;

typedef struct {
    void                    *pExtCfg;
} SVC_CV_FLOW_ALGO_QUERY_CFG_s;

typedef struct {
    void                    *pExtCfg;
} SVC_CV_FLOW_ALGO_CREATE_CFG_s;
typedef struct {
    void                    *pExtCfg;
} SVC_CV_FLOW_ALGO_DELETE_CFG_s;

void SvcCvFlow_PackFdagMemBlkInfo(const char *pModuleName, void *pAddr, UINT32 Size, flexidag_memblk_t *pMemBlk);

UINT32 SvcCvFlow_LoadAlgo(const char *pModuleName, const SVC_CV_FLOW_ALGO_INFO_s *pAlgoInfo, SVC_CV_FLOW_ALGO_OP_INFO_s *pAlgoOp);
UINT32 SvcCvFlow_QueryAlgo(const char *pModuleName, const SVC_CV_FLOW_ALGO_INFO_s *pAlgoInfo, SVC_CV_FLOW_ALGO_QUERY_CFG_s *pAlgoCfg, SVC_CV_FLOW_ALGO_OP_INFO_s *pAlgoOp);
UINT32 SvcCvFlow_CreateAlgo(const char *pModuleName, const SVC_CV_FLOW_ALGO_INFO_s *pAlgoInfo, SVC_CV_FLOW_ALGO_CREATE_CFG_s *pAlgoCfg, SVC_CV_FLOW_ALGO_OP_INFO_s *pAlgoOp);
UINT32 SvcCvFlow_DeleteAlgo(const char *pModuleName, SVC_CV_FLOW_ALGO_DELETE_CFG_s *pAlgoCfg, SVC_CV_FLOW_ALGO_OP_INFO_s *pAlgoOp);

UINT32 SvcCvFlow_LoadAlgoBin(const char *pModuleName, const SVC_CV_FLOW_LOAD_INFO_s *pLoadInfo, SVC_CV_FLOW_ALGO_OP_INFO_s *pAlgoOp);
#endif /* SVC_CV_FLOW_ALGO_UTIL_H */