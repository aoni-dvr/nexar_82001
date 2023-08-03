/**
 *  @file Util_ReadCvoutHdlr.h
 *
 * Copyright (c) 2021 Ambarella International LP
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
 *  @details Header file of the utility for read cvout
 *
 */

#ifndef UTIL_READ_CVOUT_HDLR_H
#define UTIL_READ_CVOUT_HDLR_H

#include "RefFlow_Common.h"
#include "AmbaOD_2DBbx.h"
#include "AmbaOD_3DBbx.h"
#include "AmbaSEG_BufferDef.h"
#include "AmbaFS.h"

typedef struct {
    RF_LOG_OD_2ND_DATA_HEADER_s Header;
    RF_LOG_OD_2ND_DATA_s Data[AMBA_OD_2DBBX_MAX_BBX_NUM];
    AMBA_OD_3DBBX_s ThreeDBbx[AMBA_OD_2DBBX_MAX_BBX_NUM];
} UTIL_NN3D_CVOUT_s;

typedef struct {
    RF_LOG_OD_2ND_DATA_HEADER_s Header;
    RF_LOG_OD_2ND_DATA_s Data[AMBA_OD_2DBBX_MAX_BBX_NUM];
    RF_LOG_OD_2ND_OBJ_SEG_INFO MkBbx[AMBA_OD_2DBBX_MAX_BBX_NUM];
    UINT8* Addr[AMBA_OD_2DBBX_MAX_BBX_NUM];
} UTIL_NNMK_CVOUT_s;

UINT32 UtilReadCvoutHdlr_GetNexOD(AMBA_OD_2DBBX_LIST_s* pBbxList, RF_LOG_OD_2DBBOX_HEADER_s* pODHeader, AMBA_FS_FILE* fp, UINT32* frameIdx, UINT32* EndFileFlag);
UINT32 UtilReadCvoutHdlr_GetNexSeg(AMBA_SEG_BUF_INFO_s* pSegInfo, UINT32 SegBufFmt, UINT32 SegBufSizeLimit, AMBA_FS_FILE* fp, UINT32* frameIdx, UINT32* EndFileFlag);

UINT32 UtilReadCvoutHdlr_GetNex3D(AMBA_FS_FILE *pFp, UTIL_NN3D_CVOUT_s *pNN3DOut, UINT32 *pFrameIdx, UINT32* EndFileFlag);
UINT32 UtilReadCvoutHdlr_GetNexMk(AMBA_FS_FILE *pFp, UINT8 *pBuf, UINT32 MkBufFmt, UINT32 MaxBufSize, UINT32 *pUsedBufSize, UTIL_NNMK_CVOUT_s *pNNMkOut, UINT32 *pFrameIdx, UINT32* EndFileFlag);


UINT32 UtilReadCvoutHdlr_RunLengthEncode(const UINT8* pBufIn, UINT32 Width, UINT32 Height, UINT32 Pitch, UINT8* pBufEnc, UINT32 BufEncSize, UINT32* EncLength);
UINT32 UtilReadCvoutHdlr_RunLengthDecode(const UINT8 *pBufEnc, UINT32 BufEncSize, UINT8* pBufDec, UINT32 BufDecSize, UINT32* DecLength);

UINT32 UtilReadCvoutHdlr_Parsing2DBbox(UINT32 DataFmt, const UINT8* pPayload, UINT32 PayloadSize,
                                       RF_LOG_OD_2DBBOX_HEADER_s* pBbx2dHeader,
                                       AMBA_OD_2DBBX_s Bbx2d[AMBA_OD_2DBBX_MAX_BBX_NUM]);

UINT32 UtilReadCvoutHdlr_Parsing3DBbox(UINT32 DataFmt, const UINT8* pPayload, UINT32 PayloadSize,
                                       RF_LOG_OD_2ND_DATA_HEADER_s* pBbx3dHeader,
                                       RF_LOG_OD_2ND_DATA_s Bbx3dData[AMBA_OD_2DBBX_MAX_BBX_NUM],
                                       AMBA_OD_3DBBX_s Bbx3d[AMBA_OD_2DBBX_MAX_BBX_NUM]);

UINT32 UtilReadCvoutHdlr_ParsingMK(UINT32 DataFmt, const UINT8* pPayload, UINT32 PayloadSize,
                                   RF_LOG_OD_2ND_DATA_HEADER_s* pMkHeader,
                                   RF_LOG_OD_2ND_DATA_s MkData[AMBA_OD_2DBBX_MAX_BBX_NUM],
                                   RF_LOG_OD_2ND_OBJ_SEG_INFO MkInfo[AMBA_OD_2DBBX_MAX_BBX_NUM],
                                   UINT8* pMk[AMBA_OD_2DBBX_MAX_BBX_NUM],
                                   UINT8* MkWrkBuf, UINT32 MkWrkBufSize);

UINT32 UtilReadCvoutHdlr_ParsingSeg(UINT32 DataFmt, const UINT8* pPayload, UINT32 PayloadSize,
                                    RF_LOG_SEG_BUF_HEADER_s* pSegHeader,
                                    UINT8* pSegBuf, UINT32 SegBufSize);

#endif
