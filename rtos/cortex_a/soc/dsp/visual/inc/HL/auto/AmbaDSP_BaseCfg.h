/**
*  @file AmbaDSP_BaseCfg.h
*
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
*  @details Definitions & Constants for Base config
*
*/

#ifndef AMBA_DSP_BASE_CONFIG_H
#define AMBA_DSP_BASE_CONFIG_H

#include "AmbaDSP_Buffers.h"

typedef struct {
    /* General */
#define BASE_CFG_CACHE_DRAM_IDX (0U)
#define BASE_CFG_CACHE_SMEM_IDX (1U)
    UINT32 FbpCache:4;
    UINT32 MFbpCache:4;
    UINT32 FbCache:4;
    UINT32 MFbCache:4;
    UINT32 ImgInfCache:4;
    UINT32 MImgInfCache:4;
    UINT32 CacheRsvd:8;

    /* IENG */
    UINT8  LdStrNum[3U];
    UINT8  LdStrRsvd;

    /* Vin */
    UINT8  VinNum;
    UINT8  VirtVinNum;
    UINT8  MaxVinPinNum;
    UINT32 VinBitMask;
    UINT32 VinCeBitMask;
    UINT32 VinYuvBitMask;
    UINT16 FrmBufNum[DSP_VIN_MAX_NUM];

    /* Vout */
    UINT8  VoutBitMask;
    UINT8  VoutRsvd[3U];

    /* Vproc */
    UINT32 VprocResHdr:1;
    UINT32 VprocResLndt:1;
    UINT32 VprocResTileMode:1;
    UINT32 VprocResEfct:1;
    UINT32 VprocResMaxC2YTileNum:8;
    UINT32 VprocResMaxTileNum:8; //Warp
    UINT32 VprocResMaxEfctCopyNum:8;
    UINT32 VprocExtMemAllocMode:1; //0:FB allocated by uCode. 1:FB allocated by ARM
    UINT32 VprocResHiso:1;
    UINT32 VprocFlagRsvd:2;

    UINT8  MaxVprocNum;
    UINT8  MaxVprocEfctComm0FrmBufNum;
    UINT8  MaxVprocEfctComm1FrmBufNum;
    UINT8  MaxVprocEfctMainFrmBufNum;

    UINT8  MaxVprocEfctMainMeFrmBufNum;
    UINT8  MaxVprocEfctPipFrmBufNum;
    UINT8  MaxVprocEfctPipMeFrmBufNum;
    UINT8  VprocRsvd;

    UINT32 VprocCompOutBitMask;

    UINT32 VprocHdrBitMask;

    UINT32 VprocMipiYuyvBitMask;

    UINT8  VprocC2YFrmBufNum[AMBA_DSP_MAX_VIEWZONE_NUM];
    UINT8  VprocMainFrmBufNum[AMBA_DSP_MAX_VIEWZONE_NUM];
    UINT8  VprocMainMeFrmBufNum[AMBA_DSP_MAX_VIEWZONE_NUM];
    UINT8  VprocPrevAFrmBufNum[AMBA_DSP_MAX_VIEWZONE_NUM];
    UINT8  VprocPrevAMeFrmBufNum[AMBA_DSP_MAX_VIEWZONE_NUM];
    UINT8  VprocPrevBFrmBufNum[AMBA_DSP_MAX_VIEWZONE_NUM];
    UINT8  VprocPrevBMeFrmBufNum[AMBA_DSP_MAX_VIEWZONE_NUM];
    UINT8  VprocPrevCFrmBufNum[AMBA_DSP_MAX_VIEWZONE_NUM];
    UINT8  VprocPrevCMeFrmBufNum[AMBA_DSP_MAX_VIEWZONE_NUM];
    UINT8  VprocLndtFrmBufNum[AMBA_DSP_MAX_VIEWZONE_NUM];
    UINT8  VprocHierFrmBufNum[AMBA_DSP_MAX_VIEWZONE_NUM];
    UINT8  VprocExtMemBufNum[AMBA_DSP_MAX_VIEWZONE_NUM][NUM_VPROC_EXT_MEM_TYPE];
    UINT16 VprocHisoW0Max;
    UINT16 VprocHisoSec2Max;
    UINT16 VprocHisoSec4Max;
    UINT16 VprocHisoSec5Max;
    UINT16 VprocHisoSec6Max;
    UINT16 VprocHisoSec7Max;
    UINT16 VprocHisoSec9Max;
    UINT16 VprocHisoColRepMax;

    /* Enc */
    UINT8  EncNum;
    UINT8  EncRsvd[3U];
    UINT8  ReconNum[AMBA_DSP_MAX_STREAM_NUM];

    /* Dec */
    UINT8  DecNum;
    UINT8  DecFmt;  /* Bitwise B[0]AVC B[1]HEVC B[2]Jpeg */
    UINT8  DecRsvd[2U];
    UINT32 IsDuplexMode:1;
    UINT32 DecBitRsvd:31;

    /* Misc */
} DSP_BASE_CFG_s;

typedef struct {
    UINT16 AikParNum;
    UINT16 DramParNum;

    UINT16 SmemParNum;
    UINT16 SubParNum;

    UINT16 SupParNum;
    UINT16 FbpNum;

    UINT16 FbNum;
    UINT16 DbpNum;

    UINT16 CBufNum;
    UINT16 BdtNum;

    UINT16 BdNum;
    UINT16 ImgInfNum;

    UINT16 ExtFbNum;
    UINT16 McblNum;

    UINT16 McbNum;
    UINT16 MbufParSize;

    UINT16 MbufSize;
    UINT16 MFbpNum;

    UINT16 MFbSize;
    UINT16 ExtMFbNum;

    UINT16 MImgInfNum;
    UINT16 OrccodeMsgNum;
} DSP_BASE_PARAM_s;

#define DSP_MEM_AREA_VIN_BIT        (0U)
#define DSP_MEM_AREA_VPROC_COMM_BIT (1U)
#define DSP_MEM_AREA_VPROC_BIT      (2U)
#define DSP_MEM_AREA_ENC_BIT        (3U)
#define DSP_MEM_AREA_DEC_BIT        (4U)
#define DSP_MEM_AREA_MISC_BIT       (5U)
#define DSP_MEM_AREA_NUM_BIT        (6U)
#define DSP_MEM_AREA_ALL_MASK       (0x3FU) // ((1<<DSP_MEM_AREA_ALL_MASK) - 1)
typedef struct {
    /* Vin */
    UINT32 VinBitMask;
    UINT32 VinCeBitMask;
    UINT8  VinCmpr[DSP_VIN_MAX_NUM];
    UINT16 VinMaxCapOutWidth[DSP_VIN_MAX_NUM]; //include CFA/YUV
    UINT16 VinMaxCapOutHeight[DSP_VIN_MAX_NUM]; //include CFA/YUV
    UINT16 VinMaxCeOutWidth[DSP_VIN_MAX_NUM];
    UINT16 VinMaxCeOutHeight[DSP_VIN_MAX_NUM];
    UINT8  CapOutBufNum[DSP_VIN_MAX_NUM];
    UINT8  CeOutBufNum[DSP_VIN_MAX_NUM];

    /* Vproc */
    UINT32 VprocResHdr:1;
    UINT32 VprocResEfct:1;
    UINT32 VprocComm0Fmt:2;
    UINT32 VprocComm1Fmt:2;
    UINT32 VprocFlagRsvd:26;

    UINT8  MaxVprocNum;
    UINT8  MaxVprocEfctComm0FrmBufNum;
    UINT8  MaxVprocEfctComm1FrmBufNum;
    UINT8  MaxVprocEfctMainFrmBufNum;

    UINT8  MaxVprocEfctMainMeFrmBufNum;
    UINT8  MaxVprocEfctPipFrmBufNum;
    UINT8  MaxVprocEfctPipMeFrmBufNum;
    UINT8  VprocRsvd;

    UINT16 MaxComm0Width;
    UINT16 MaxComm0Height;

    UINT16 MaxComm1Width;
    UINT16 MaxComm1Height;

    UINT16 MaxEfctMainWidth;
    UINT16 MaxEfctMainHeight;

    UINT16 MaxEfctPipWidth;
    UINT16 MaxEfctPipHeight;

    UINT32 VprocBitMask;

    UINT32 VprocCompOutBitMask;

    UINT32 VprocHdrBitMask;

    UINT32 VprocMipiYuyvBitMask;

    UINT8  VprocC2YFrmBufNum[AMBA_DSP_MAX_VIEWZONE_NUM];

    UINT16 MaxC2YWidth[AMBA_DSP_MAX_VIEWZONE_NUM]; //PreWarp
    UINT16 MaxC2YHeight[AMBA_DSP_MAX_VIEWZONE_NUM];

    UINT16 MaxW0[AMBA_DSP_MAX_VIEWZONE_NUM]; //W0
    UINT16 MaxH0[AMBA_DSP_MAX_VIEWZONE_NUM];

    UINT8  VprocMainFrmBufNum[AMBA_DSP_MAX_VIEWZONE_NUM];
    UINT8  VprocMainMeFrmBufNum[AMBA_DSP_MAX_VIEWZONE_NUM];

    UINT16 MaxMainWidth[AMBA_DSP_MAX_VIEWZONE_NUM];
    UINT16 MaxMainHeight[AMBA_DSP_MAX_VIEWZONE_NUM];

    UINT8  VprocPrevAFrmBufNum[AMBA_DSP_MAX_VIEWZONE_NUM];
    UINT8  VprocPrevAMeFrmBufNum[AMBA_DSP_MAX_VIEWZONE_NUM];

    UINT16 MaxPrevAWidth[AMBA_DSP_MAX_VIEWZONE_NUM];
    UINT16 MaxPrevAHeight[AMBA_DSP_MAX_VIEWZONE_NUM];

    UINT8  VprocPrevBFrmBufNum[AMBA_DSP_MAX_VIEWZONE_NUM];
    UINT8  VprocPrevBMeFrmBufNum[AMBA_DSP_MAX_VIEWZONE_NUM];

    UINT16 MaxPrevBWidth[AMBA_DSP_MAX_VIEWZONE_NUM];
    UINT16 MaxPrevBHeight[AMBA_DSP_MAX_VIEWZONE_NUM];

    UINT8  VprocPrevCFrmBufNum[AMBA_DSP_MAX_VIEWZONE_NUM];
    UINT8  VprocPrevCMeFrmBufNum[AMBA_DSP_MAX_VIEWZONE_NUM];

    UINT16 MaxPrevCWidth[AMBA_DSP_MAX_VIEWZONE_NUM];
    UINT16 MaxPrevCHeight[AMBA_DSP_MAX_VIEWZONE_NUM];

    UINT8  VprocLndtFrmBufNum[AMBA_DSP_MAX_VIEWZONE_NUM];

    UINT16 MaxLndtWidth[AMBA_DSP_MAX_VIEWZONE_NUM];
    UINT16 MaxLndtHeight[AMBA_DSP_MAX_VIEWZONE_NUM];

    UINT8  VprocHierFrmBufNum[AMBA_DSP_MAX_VIEWZONE_NUM];

    UINT16 MaxHierBufWidth[AMBA_DSP_MAX_VIEWZONE_NUM];
    UINT16 MaxHierBufHeight[AMBA_DSP_MAX_VIEWZONE_NUM];

    /* Enc */
    UINT8  EncNum;
    UINT8  EncRsvd[3U];
    UINT8  ReconNum[AMBA_DSP_MAX_STREAM_NUM];
    UINT16 MaxEncWidth[AMBA_DSP_MAX_STREAM_NUM];
    UINT16 MaxEncHeight[AMBA_DSP_MAX_STREAM_NUM]; // include extra height
    UINT32 PjpgSize;
    UINT32 AvcPjpgSize;

    /* Dec */
    UINT8  DecNum;
    UINT8  DecFmt;  /* Bitwise B[0]AVC B[1]HEVC B[2]Jpeg */
    UINT8  DecMaxDpb;
    UINT8  DecRsvd[1U];
    UINT16 DecMaxWidth;
    UINT16 DecMaxHeight;

    /* Misc */
} DSP_DRAM_CFG_s;

#define DSP_BW_AREA_VIN_BIT         (0U)
#define DSP_BW_AREA_VPROC_BIT       (1U)
#define DSP_BW_AREA_ENC_BIT         (3U)
#define DSP_BW_AREA_VOUT_BIT        (2U)
#define DSP_BW_AREA_EFCT_BIT        (4U)
#define DSP_BW_AREA_DEC_BIT         (5U)
#define DSP_BW_AREA_MISC_BIT        (6U)
#define DSP_BW_AREA_NUM_BIT         (7U)
#define DSP_BW_AREA_ALL_MASK        (0x7FU) // ((1<<DSP_BW_AREA_NUM_BIT) - 1)
typedef struct {
    /* Vin */
    UINT32 VinBitMask;
    UINT32 VinCeBitMask;
    UINT8  VinCmpr[DSP_VIN_MAX_NUM];
    UINT16 VinCapOutWidth[DSP_VIN_MAX_NUM]; //include CFA/YUV
    UINT16 VinCapOutHeight[DSP_VIN_MAX_NUM]; //include CFA/YUV
    UINT16 VinCeOutWidth[DSP_VIN_MAX_NUM];
    UINT16 VinCeOutHeight[DSP_VIN_MAX_NUM];
    UINT16 VinFrmRateFPSx10[AMBA_DSP_MAX_VIEWZONE_NUM];

    /* Vproc */
    UINT32 VprocBitMask;
    UINT16 MainWidth[AMBA_DSP_MAX_VIEWZONE_NUM];
    UINT16 MainHeight[AMBA_DSP_MAX_VIEWZONE_NUM];
    UINT16 PrevAWidth[AMBA_DSP_MAX_VIEWZONE_NUM];
    UINT16 PrevAHeight[AMBA_DSP_MAX_VIEWZONE_NUM];
    UINT16 PrevBWidth[AMBA_DSP_MAX_VIEWZONE_NUM];
    UINT16 PrevBHeight[AMBA_DSP_MAX_VIEWZONE_NUM];
    UINT16 PrevCWidth[AMBA_DSP_MAX_VIEWZONE_NUM];
    UINT16 PrevCHeight[AMBA_DSP_MAX_VIEWZONE_NUM];
    UINT16 LndtWidth[AMBA_DSP_MAX_VIEWZONE_NUM];
    UINT16 LndtHeight[AMBA_DSP_MAX_VIEWZONE_NUM];
    UINT16 HierBufWidth[AMBA_DSP_MAX_VIEWZONE_NUM];
    UINT16 HierBufHeight[AMBA_DSP_MAX_VIEWZONE_NUM];
    UINT16 VprocFrmRateFPSx10[AMBA_DSP_MAX_VIEWZONE_NUM];

    /* Vout */
    UINT8  VoutBitMask;
    UINT8  VoutRsvd[3U];
    UINT16 VoutVideoWidth[AMBA_DSP_MAX_VOUT_NUM];
    UINT16 VoutVideoHeight[AMBA_DSP_MAX_VOUT_NUM];
    UINT16 VoutFrmRateFPSx10[AMBA_DSP_MAX_VOUT_NUM];

    /* Enc */
    UINT8  EncNum;
    UINT8  EncRsvd[3U];
    UINT16 EncWidth[AMBA_DSP_MAX_STREAM_NUM];
    UINT16 EncHeight[AMBA_DSP_MAX_STREAM_NUM];
    UINT16 EncFrmRateFPSx10[AMBA_DSP_MAX_VOUT_NUM];

    /* Dec */

    /* Efct */
    UINT8  CopyNum;
    UINT8  BldNum;
    UINT8  RfctRsvd[2U];

    UINT16 CopyWidth[AMBA_DSP_MAX_YUVSTRM_VIEW_NUM];
    UINT16 CopyHeight[AMBA_DSP_MAX_YUVSTRM_VIEW_NUM];
    UINT16 BldWidth[AMBA_DSP_MAX_YUVSTRM_VIEW_NUM];
    UINT16 BldHeight[AMBA_DSP_MAX_YUVSTRM_VIEW_NUM];

    /* Misc */
} DSP_DRAM_BW_CFG_s;

extern UINT32 HL_PrepareBaseParam(const DSP_BASE_CFG_s *pBaseCfg, DSP_BASE_PARAM_s *pBaseParam);
extern UINT32 HL_CalcWorkingBuffer(UINT32 AOI, const DSP_DRAM_CFG_s *pDramCfg, UINT32 *pDspWork);
extern UINT32 HL_CalcDramBandWidth(UINT32 AOI, const DSP_DRAM_BW_CFG_s *pDramBwCfg, const UINT32 *pBandWidth);


#endif /* AMBA_DSP_BASE_CONFIG_H */
