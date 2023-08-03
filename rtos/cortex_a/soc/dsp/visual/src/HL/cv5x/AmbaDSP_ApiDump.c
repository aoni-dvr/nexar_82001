/**
*  @file AmbaDSP_ApiDump.c
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
*  @details DSP API dump
*
*/

#include "AmbaDSP_Log.h"
#include "AmbaDSP_ApiDump.h"
#include "AmbaDSP_ContextUtility.h"

static Dsp_DumpApi_t *pDspApiDumpFunc = NULL;

/* Main */
static void dump_MainInit(const AMBA_DSP_SYS_CONFIG_s *pDspSysConfig)
{
    if (pDspSysConfig != NULL) {
        AmbaLL_LogUInt5(" MainInit State[%d]", pDspSysConfig->SysState, 0U, 0U, 0U, 0U);
        AmbaLL_LogUInt5("          DbgAddr[0x%X%X] DbgSz[0x%X]",
                    DSP_GetU64Msb((UINT64)pDspSysConfig->DebugLogDataAreaAddr),
                    DSP_GetU64Lsb((UINT64)pDspSysConfig->DebugLogDataAreaAddr),
                    pDspSysConfig->DebugLogDataAreaSize, 0U, 0U);
        AmbaLL_LogUInt5("          WorkAddr[0x%X%X] WorkSz[0x%X] VpMsgCtrl[0x%X%X]",
                    DSP_GetU64Msb((UINT64)pDspSysConfig->WorkAreaAddr),
                    DSP_GetU64Lsb((UINT64)pDspSysConfig->WorkAreaAddr),
                    pDspSysConfig->WorkAreaSize,
                    DSP_GetU64Msb((UINT64)pDspSysConfig->VpMsgCtrlBufAddr),
                    DSP_GetU64Lsb((UINT64)pDspSysConfig->VpMsgCtrlBufAddr));
    }
}

static void dump_MainGetDefaultSysCfg(const AMBA_DSP_SYS_CONFIG_s *pDspSysConfig)
{
    ULONG ULAddr = 0U;

    if (pDspSysConfig != NULL) {
        dsp_osal_typecast(&ULAddr, &pDspSysConfig);
    } else {
        ULAddr = 0U;
    }
    AmbaLL_LogUInt5(" MainGetDefaultCfg pDspSysConfig[0x%X%X]",
                DSP_GetU64Msb((UINT64)ULAddr), DSP_GetU64Lsb((UINT64)ULAddr), 0U, 0U, 0U);
}

static void dump_MainSetWorkArea(const ULONG WorkAreaAddr, const UINT32 WorkSize)
{

    AmbaLL_LogUInt5(" MainSetWorkArea WorkAddr[0x%X%X] WorkSz[0x%X]",
                DSP_GetU64Msb((UINT64)WorkAreaAddr),
                DSP_GetU64Lsb((UINT64)WorkAreaAddr), WorkSize, 0U, 0U);
}

static void dump_MainGetDspVerInfo(const AMBA_DSP_VERSION_INFO_s *pInfo)
{
    ULONG ULAddr = 0U;

    if (pInfo != NULL) {
        dsp_osal_typecast(&ULAddr, &pInfo);
    } else {
        ULAddr = 0U;
    }
    AmbaLL_LogUInt5(" MainGetDspVerInfo pInfo[0x%X%X]",
                DSP_GetU64Msb((UINT64)ULAddr), DSP_GetU64Lsb((UINT64)ULAddr), 0U, 0U, 0U);
}

static void dump_MainMsgParseEntry(const UINT32 EntryArg)
{
    AmbaLL_LogUInt5(" MsgParseEntry EntryArg[0x%X]", EntryArg, 0U, 0U, 0U, 0U);
}

static void dump_MainWaitVinInterrupt(const UINT32 Flag, const UINT32 *pActualFlag, const UINT32 TimeOut)
{
    ULONG ULAddr = 0U;

    if (pActualFlag != NULL) {
        dsp_osal_typecast(&ULAddr, &pActualFlag);
    } else {
        ULAddr = 0U;
    }
    AmbaLL_LogUInt5(" MainWaitVinInterrupt Flag[0x%x] *pActualFlag[0x%X%X] TimeOut[%d]",
                Flag, DSP_GetU64Msb((UINT64)ULAddr), DSP_GetU64Lsb((UINT64)ULAddr), TimeOut, 0U);
}

static void dump_MainWaitVoutInterrupt(const UINT32 Flag, const UINT32 *pActualFlag, const UINT32 TimeOut)
{
    ULONG ULAddr = 0U;

    if (pActualFlag != NULL) {
        dsp_osal_typecast(&ULAddr, &pActualFlag);
    } else {
        ULAddr = 0U;
    }
    AmbaLL_LogUInt5(" MainWaitVoutInterrupt Flag[0x%x] *pActualFlag[0x%X%X] TimeOut[%d]",
                Flag, DSP_GetU64Msb((UINT64)ULAddr), DSP_GetU64Lsb((UINT64)ULAddr), TimeOut, 0U);
}

static void dump_MainWaitFlag(const UINT32 Flag, const UINT32 *pActualFlag, const UINT32 TimeOut)
{
    ULONG ULAddr = 0U;

    if (pActualFlag != NULL) {
        dsp_osal_typecast(&ULAddr, &pActualFlag);
    } else {
        ULAddr = 0U;
    }
    AmbaLL_LogUInt5(" MainWaitFlag Flag[0x%x] *pActualFlag[0x%X%X] TimeOut[%d]",
                Flag, DSP_GetU64Msb((UINT64)ULAddr), DSP_GetU64Lsb((UINT64)ULAddr), TimeOut, 0U);
}

static inline void dump_MainRescLimitLv(const AMBA_DSP_LIVEVIEW_RESOURCE_s *pLvRes,
                                        const AMBA_DSP_LIVEVIEW_RESOURCE_EX_s *pLvResEx)
{
    UINT16 i, j;
    ULONG ULAddr;

    AmbaLL_LogUInt5(" ResLv Pipe[%d] LD[%d] MaxVinBit[0x%X] MaxViewNum[%d] MaxVirtVinBit[0x%X]",
            pLvRes->VideoPipe, pLvRes->LowDelayMode, pLvRes->MaxVinBit, pLvRes->MaxViewZoneNum, pLvRes->MaxVirtVinBit);
    for (i=0U; i<AMBA_DSP_MAX_VIN_NUM; i++) {
        AmbaLL_LogUInt5("       VinId[%d]",
                    i, 0U, 0U, 0U, 0U);
        AmbaLL_LogUInt5("         MaxVinVirtChanOutputNum[%d] MaxVinW[%d] MaxVinH[%d] MaxVinBootTout[%d] MaxVinTout[%d]",
                    pLvRes->MaxVinVirtChanOutputNum[i][0U], pLvRes->MaxVinVirtChanWidth[i][0U], pLvRes->MaxVinVirtChanHeight[i][0U],
                    pLvRes->MaxVinBootTimeout[i], pLvRes->MaxVinTimeout[i]);
        AmbaLL_LogUInt5("         MaxR2YDelay[%d] Capab[0x%X] EmbdWin[%d %d] ShrtFrmThrd[%d]", pLvRes->MaxRaw2YuvDelay[i], pLvResEx->MaxVinCapability[i],
                                                                                pLvResEx->MaxVinEmbdDataWidth[i], pLvResEx->MaxVinEmbdDataHeight[i], pLvResEx->MaxVinShortFrameThreshold[i]);
        AmbaLL_LogUInt5("         RawBuf.AllocType[%d] BufNum[%d] Compressed[%d] BaseAddr[0x%X%X]",
                    pLvRes->RawBuf[i][0U].AllocType, pLvRes->RawBuf[i][0U].BufNum, pLvRes->RawBuf[i][0U].Buf.Compressed,
                    DSP_GetU64Msb((UINT64)pLvRes->RawBuf[i][0U].Buf.BaseAddr),
                    DSP_GetU64Lsb((UINT64)pLvRes->RawBuf[i][0U].Buf.BaseAddr));
        AmbaLL_LogUInt5("                Pitch[%d] OfstX[%d %d] Win[%d %d]",
                    pLvRes->RawBuf[i][0U].Buf.Pitch,
                    pLvRes->RawBuf[i][0U].Buf.Window.OffsetX, pLvRes->RawBuf[i][0U].Buf.Window.OffsetY,
                    pLvRes->RawBuf[i][0U].Buf.Window.Width, pLvRes->RawBuf[i][0U].Buf.Window.Height);
        if (pLvRes->RawBuf[i][0U].pRawBufTbl != NULL) {
            dsp_osal_typecast(&ULAddr, &pLvRes->RawBuf[i][0U].pRawBufTbl);
        } else {
            ULAddr = 0U;
        }
        if ((pLvRes->RawBuf[i][0U].AllocType == ALLOC_EXTERNAL_DISTINCT) && (ULAddr > 0U)) {
            for (j = 0U; j < pLvRes->RawBuf[i][0U].BufNum; j++) {
                AmbaLL_LogUInt5("                TblIdx[%d] Addr[0x%X%X]", j,
                        DSP_GetU64Msb((UINT64)pLvRes->RawBuf[i][0U].pRawBufTbl[j]),
                        DSP_GetU64Lsb((UINT64)pLvRes->RawBuf[i][0U].pRawBufTbl[j]), 0U, 0U);
            }
        } else {
            AmbaLL_LogUInt5("                pRawBufTbl[NULL]", 0U, 0U, 0U, 0U, 0U);
        }
        AmbaLL_LogUInt5("         RawBuf.AuxBuf Compressed[%d] BaseAddr[0x%X%X] Pitch[%d]",
                    pLvRes->RawBuf[i][0U].AuxBuf.Compressed,
                    DSP_GetU64Msb((UINT64)pLvRes->RawBuf[i][0U].AuxBuf.BaseAddr),
                    DSP_GetU64Lsb((UINT64)pLvRes->RawBuf[i][0U].AuxBuf.BaseAddr),
                    pLvRes->RawBuf[i][0U].AuxBuf.Pitch, 0U);
        AmbaLL_LogUInt5("                Ofst[%d %d] Win[%d %d]",
                    pLvRes->RawBuf[i][0U].AuxBuf.Window.OffsetX, pLvRes->RawBuf[i][0U].AuxBuf.Window.OffsetY,
                    pLvRes->RawBuf[i][0U].AuxBuf.Window.Width, pLvRes->RawBuf[i][0U].AuxBuf.Window.Height, 0U);
        if (pLvRes->RawBuf[i][0U].pAuxBufTbl != NULL) {
            dsp_osal_typecast(&ULAddr, &pLvRes->RawBuf[i][0U].pAuxBufTbl);
        } else {
            ULAddr = 0U;
        }
        if ((pLvRes->RawBuf[i][0U].AllocType == ALLOC_EXTERNAL_DISTINCT) && (ULAddr > 0U)) {
            for (j = 0U; j < pLvRes->RawBuf[i][0U].BufNum; j++) {
                AmbaLL_LogUInt5("                TblIdx[%d] Addr[0x%X%X]", j,
                            DSP_GetU64Msb((UINT64)pLvRes->RawBuf[i][0U].pAuxBufTbl[j]),
                            DSP_GetU64Lsb((UINT64)pLvRes->RawBuf[i][0U].pAuxBufTbl[j]), 0U, 0U);
            }
        } else {
            AmbaLL_LogUInt5("                pAuxBufTbl[NULL]", 0U, 0U, 0U, 0U, 0U);
        }
        AmbaLL_LogUInt5("         DefRaw Format[%d]", pLvRes->DefaultRawFormat[i], 0U, 0U, 0U, 0U);
        AmbaLL_LogUInt5("           Compressed[%d] BaseAddr[0x%X%X] Pitch[%d]",
                    pLvRes->DefaultRawBuf[i][0U].Compressed,
                    DSP_GetU64Msb((UINT64)pLvRes->DefaultRawBuf[i][0U].BaseAddr),
                    DSP_GetU64Lsb((UINT64)pLvRes->DefaultRawBuf[i][0U].BaseAddr),
                    pLvRes->DefaultRawBuf[i][0U].Pitch, 0U);
        AmbaLL_LogUInt5("           Ofst[%d %d] Win[%d %d]",
                    pLvRes->DefaultRawBuf[i][0U].Window.OffsetX, pLvRes->DefaultRawBuf[i][0U].Window.OffsetY,
                    pLvRes->DefaultRawBuf[i][0U].Window.Width, pLvRes->DefaultRawBuf[i][0U].Window.Height, 0U);
        AmbaLL_LogUInt5("           Aux.Compressed[%d] BaseAddr[0x%X%X] Pitch[%d]",
                    pLvRes->DefaultAuxRawBuf[i][0U].Compressed,
                    DSP_GetU64Msb((UINT64)pLvRes->DefaultAuxRawBuf[i][0U].BaseAddr),
                    DSP_GetU64Lsb((UINT64)pLvRes->DefaultAuxRawBuf[i][0U].BaseAddr),
                    pLvRes->DefaultAuxRawBuf[i][0U].Pitch, 0U);
        AmbaLL_LogUInt5("               Ofst[%d %d] Win[%d %d]",
                    pLvRes->DefaultAuxRawBuf[i][0U].Window.OffsetX, pLvRes->DefaultAuxRawBuf[i][0U].Window.OffsetY,
                    pLvRes->DefaultAuxRawBuf[i][0U].Window.Width, pLvRes->DefaultAuxRawBuf[i][0U].Window.Height, 0U);
    }

    for (i=0U; i<AMBA_DSP_MAX_VIRT_VIN_NUM; i++) {
        if (1U == DSP_GetU16Bit(pLvRes->MaxVirtVinBit, i, 1U)) {
            AmbaLL_LogUInt5("       VirtVinId[%d]", i, 0U, 0U, 0U, 0U);
            AmbaLL_LogUInt5("         MaxVinVirtChanOutputNum[%d] MaxVinWin[%d %d]",
                        pLvRes->MaxVirtVinOutputNum[i], pLvRes->MaxVirtVinWidth[i], pLvRes->MaxVirtVinHeight[i], 0U, 0U);
        }
    }

    for (i=0U; i<AMBA_DSP_MAX_VIEWZONE_NUM; i++) {
        AmbaLL_LogUInt5("       ViewZoneId[%d] DefIsoCfg CtxIdx[%d] CfgIdx[%d] CfgAddr[0x%X%X]", i,
                    pLvRes->DefaultRawIsoConfigAddr[i].CtxIndex,
                    pLvRes->DefaultRawIsoConfigAddr[i].CfgIndex,
                    DSP_GetU64Msb((UINT64)pLvRes->DefaultRawIsoConfigAddr[i].CfgAddress),
                    DSP_GetU64Lsb((UINT64)pLvRes->DefaultRawIsoConfigAddr[i].CfgAddress));
        AmbaLL_LogUInt5("                     HdrCfgIdx[%d] MaxWarpDma[%d] MaxWarpWaitLine[%d %d]",
                    pLvRes->DefaultRawIsoConfigAddr[i].HdrCfgIndex,
                    pLvRes->MaxWarpDma[i], pLvRes->MaxWarpWaitLineLuma[i], pLvRes->MaxWarpWaitLineChroma[i], 0U);
        AmbaLL_LogUInt5("                     MaxHier[%d %d] MaxHorWarpComp[%d] PpBldW[%d]",
                    pLvRes->MaxHierWidth[i], pLvRes->MaxHierHeight[i],
                    pLvResEx->MaxHorWarpCompensation[i], pLvResEx->MaxPostBlendWidth[i], 0U);
        AmbaLL_LogUInt5("                     MaxInputWin[%d %d]", pLvResEx->MaxViewZoneInputWidth[i], pLvResEx->MaxViewZoneInputHeight[i], 0U, 0U, 0U);
#ifdef SUPPORT_IDSP_AFFINITY
        AmbaLL_LogUInt5("                     MaxAff[0x%X]",
                    DSP_GetU16Bit(pLvResEx->MaxDspAffinity[i], IDSP_AFFINITY_CORE_BIT_IDX, IDSP_AFFINITY_CORE_LEN), 0U, 0U, 0U, 0U);
#endif
    }
}

static void dump_MainResourceLimit(const AMBA_DSP_RESOURCE_s *pResource)
{
    if (pResource != NULL) {
        UINT16 i;
        const AMBA_DSP_LIVEVIEW_RESOURCE_s *pLvRes = &pResource->LiveviewResource;
        const AMBA_DSP_ENCODE_RESOURCE_s *pEncRes = &pResource->EncodeResource;
        const AMBA_DSP_STILL_RESOURCE_s *pStlRes = &pResource->StillResource;
        const AMBA_DSP_DECODE_RESOURCE_s *pDecRes = &pResource->DecodeResource;
        const AMBA_DSP_DISPLAY_RESOURCE_s *pDispRes = &pResource->DisplayResource;
        const AMBA_DSP_EXTMEM_ENC_RESOURCE_s *pExtMemRes = &pResource->ExtMemEncResource;
        const AMBA_DSP_LIVEVIEW_RESOURCE_EX_s *pLvResEx = &pResource->LiveviewResourceEx;

        dump_MainRescLimitLv(pLvRes, pLvResEx);

        AmbaLL_LogUInt5(" ResEnc StrmNum[%d]", pEncRes->MaxEncodeStream, 0U, 0U, 0U, 0U);
        for (i = 0U; i<AMBA_DSP_MAX_STREAM_NUM; i++) {
            AmbaLL_LogUInt5("        StrmIdx[%d] MaxGopM[%d] MaxRecon[%d] MaxFmt[0x%X]",
                    i, pEncRes->MaxGopM[i], pEncRes->MaxExtraReconNum[i],
                    pEncRes->MaxStrmFmt[i], 0U);
            AmbaLL_LogUInt5("                    MaxSR[%d] MaxSRHevc[%d] MaxAff[0x%X]",
                    pEncRes->MaxSearchRange[i], pEncRes->MaxSearchRangeHevc[i], pEncRes->MaxAffinity[i], 0U, 0U);
        }

        AmbaLL_LogUInt5(" ResStl MaxProcFmt[0x%X] MaxMainWin[%d %d]",
                pStlRes->MaxProcessFormat, pStlRes->MaxMainWidth,
                pStlRes->MaxMainHeight, 0U, 0U);
        for (i = 0U; i<AMBA_DSP_MAX_VIN_NUM; i++) {
            AmbaLL_LogUInt5("        [%d] MaxVinWin[%d %d]",
                    i, pStlRes->MaxVinWidth[i], pStlRes->MaxVinHeight[i], 0U, 0U);
        }
        AmbaLL_LogUInt5("        MaxRawInputWin[%d %d] MaxYuvInputWin[%d %d]",
                    pStlRes->MaxRawInputWidth, pStlRes->MaxRawInputHeight,
                    pStlRes->MaxYuvInputWidth, pStlRes->MaxYuvInputHeight, 0U);
        AmbaLL_LogUInt5("        MaxYuvEncWin[%d %d]",
                    pStlRes->MaxYuvEncWidth, pStlRes->MaxYuvEncHeight, 0U, 0U, 0U);

        AmbaLL_LogUInt5(" ResDec StrmNum[%d] MaxFrameWin[%d %d] MaxBitRate[%d] MaxRatioOfGopNM[%d]",
                pDecRes->MaxDecodeStreamNum, pDecRes->MaxFrameWidth,
                pDecRes->MaxFrameHeight, pDecRes->MaxBitRate, pDecRes->MaxRatioOfGopNM);
        AmbaLL_LogUInt5("        MaxVideoPlaneWin[%d %d] BackwardTrickPlayMode[%d] MaxPicWin[%d %d]",
                pDecRes->MaxVideoPlaneWidth, pDecRes->MaxVideoPlaneHeight,
                pDecRes->BackwardTrickPlayMode, pDecRes->MaxPicWidth, pDecRes->MaxPicHeight);
        if (pDecRes->MaxDecodeStreamNum > 0U) {
            for (i = 0U; i < pDecRes->MaxDecodeStreamNum; i++) {
                AmbaLL_LogUInt5("        MaxStrmFmt[%d] MaxSr[%d]",
                        pDecRes->MaxStrmFmt[i], pDecRes->MaxSearchRange[i], 0U, 0U, 0U);
            }
        }

        AmbaLL_LogUInt5(" ResDisp", 0U, 0U, 0U, 0U, 0U);
        for (i = 0U; i < AMBA_DSP_MAX_VOUT_NUM; i++) {
            AmbaLL_LogUInt5("        VoutIdx[%d] MaxVoutWidth[%d] MaxOsdBufWidth[%d] MaxVirtChan[%d]",
                    i, pDispRes->MaxVoutWidth[i], pDispRes->MaxOsdBufWidth[i], pDispRes->MaxVirtChanNum[i], 0U);
        }

        AmbaLL_LogUInt5(" ResExtMem", 0U, 0U, 0U, 0U, 0U);
        for (i = 0U; i < AMBA_DSP_MAX_STREAM_NUM; i++) {
            AmbaLL_LogUInt5("        StrmIdx[%d] MaxEncWin[%d %d]",
                    i, pExtMemRes->MaxEncWidth[i], pExtMemRes->MaxEncHeight[i], 0U, 0U);
        }
    }
}

static void dump_GetCmprRawBufInfo(const UINT16 Width, const UINT16 CmprRate, const UINT16 *pRawWidth, const UINT16 *pRawPitch)
{
(void)pRawWidth;
(void)pRawPitch;
    AmbaLL_LogUInt5(" GetCmprRawBufInfo W[%d] Rate[%d]", Width, CmprRate, 0U, 0U, 0U);
}

static void dump_ParLoadConfig(const UINT32 Enable, const UINT32 Data)
{
    AmbaLL_LogUInt5(" ParLoadCfg Enb[%d] Data[%d]", Enable, Data, 0U, 0U, 0U);
}

static void dump_ParLoadRegionUnlock(const UINT16 RegionIdx, const UINT16 SubRegionIdx)
{
    AmbaLL_LogUInt5(" ParLoadRegionUnlock Idx[%d] SubIdx[%d]", RegionIdx, SubRegionIdx, 0U, 0U, 0U);
}

static void dump_CalcEncMvBufInfo(const UINT16 Width, const UINT16 Height, const UINT32 Option, const UINT32 *pBufSize)
{
(void)pBufSize;
    AmbaLL_LogUInt5(" CalcEncMvBufInfo Win[%dx%d]] Opt[%d]", Width, Height, Option, 0U, 0U);
}

static void dump_MainGetBufInfo(const UINT32 Type, const AMBA_DSP_BUF_INFO_s *pBufInfo)
{
(void)pBufInfo;
    AmbaLL_LogUInt5(" MainGetBufInfo Type[%d]", Type, 0U, 0U, 0U, 0U);
}

static void dump_SliceCfgCalc(const UINT16 Id,
                              const AMBA_DSP_WINDOW_DIMENSION_s *pIn,
                              const AMBA_DSP_WINDOW_DIMENSION_s *pOut,
                              const AMBA_DSP_SLICE_CFG_s *pLvSliceCfg)
{
(void)pLvSliceCfg;
    AmbaLL_LogUInt5(" SliceCfgCalc Id[0x%X] In[%d %d] Out[%d %d]", Id, pIn->Width, pIn->Height, pOut->Width, pOut->Height);
}

static inline void dump_LvVzPymdBuf(const AMBA_DSP_LIVEVIEW_VIEWZONE_CFG_s* pViewZoneCfg)
{
    UINT32 j;
    ULONG ULAddr;

    if (pViewZoneCfg->pPyramidBuf != NULL) {
        if (pViewZoneCfg->pPyramidBuf->pYuvBufTbl != NULL) {
            dsp_osal_typecast(&ULAddr, &pViewZoneCfg->pPyramidBuf->pYuvBufTbl);
        } else {
            ULAddr = 0U;
        }
        AmbaLL_LogUInt5("             Alloc[%d] BufNum[%d] BufTbl[0x%X%X]",
                pViewZoneCfg->pPyramidBuf->AllocType,
                pViewZoneCfg->pPyramidBuf->BufNum,
                DSP_GetU64Msb((UINT64)ULAddr), DSP_GetU64Lsb((UINT64)ULAddr), 0U);
        AmbaLL_LogUInt5("             YuvBuf.DataFmt[%d] BaseAddrY[0x%X%X] BaseAddrUV[0x%X%X]",
                pViewZoneCfg->pPyramidBuf->YuvBuf.DataFmt,
                DSP_GetU64Msb((UINT64)pViewZoneCfg->pPyramidBuf->YuvBuf.BaseAddrY),
                DSP_GetU64Lsb((UINT64)pViewZoneCfg->pPyramidBuf->YuvBuf.BaseAddrY),
                DSP_GetU64Msb((UINT64)pViewZoneCfg->pPyramidBuf->YuvBuf.BaseAddrUV),
                DSP_GetU64Lsb((UINT64)pViewZoneCfg->pPyramidBuf->YuvBuf.BaseAddrUV));
        AmbaLL_LogUInt5("                    Pitch[%d] Width[%d] Height[%d]",
                pViewZoneCfg->pPyramidBuf->YuvBuf.Pitch, pViewZoneCfg->pPyramidBuf->YuvBuf.Window.Width,
                pViewZoneCfg->pPyramidBuf->YuvBuf.Window.Height, 0U, 0U);
        if ((pViewZoneCfg->pPyramidBuf->AllocType == ALLOC_EXTERNAL_DISTINCT) && (ULAddr > 0U)) {
            for (j = 0; j < pViewZoneCfg->pPyramidBuf->BufNum; j++) {
                AmbaLL_LogUInt5("               TblIdx[%d] Addr[0x%X%X]", j,
                        DSP_GetU64Msb((UINT64)pViewZoneCfg->pPyramidBuf->pYuvBufTbl[j]),
                        DSP_GetU64Lsb((UINT64)pViewZoneCfg->pPyramidBuf->pYuvBufTbl[j]), 0U, 0U);
            }
        }
    } else {
        AmbaLL_LogUInt5("       pViewZoneCfg->pPyramidBuf[NULL]", 0U, 0U, 0U, 0U, 0U);
    }
}

static inline void dump_LvVzLndtBuf(const AMBA_DSP_LIVEVIEW_VIEWZONE_CFG_s* pViewZoneCfg)
{
    UINT32 j;
    ULONG ULAddr;

    if (pViewZoneCfg->pLaneDetectBuf != NULL) {
        if (pViewZoneCfg->pLaneDetectBuf->pYuvBufTbl != NULL) {
            dsp_osal_typecast(&ULAddr, &pViewZoneCfg->pLaneDetectBuf->pYuvBufTbl);
        } else {
            ULAddr = 0U;
        }
        AmbaLL_LogUInt5("             Alloc[%d] BufNum[%d] BufTbl[0x%X%X]",
                pViewZoneCfg->pLaneDetectBuf->AllocType,
                pViewZoneCfg->pLaneDetectBuf->BufNum,
                DSP_GetU64Msb((UINT64)ULAddr), DSP_GetU64Lsb((UINT64)ULAddr), 0U);
        AmbaLL_LogUInt5("             YuvBuf.DataFmt[%d] BaseAddrY[0x%X%X] BaseAddrUV[0x%X%X]",
                pViewZoneCfg->pLaneDetectBuf->YuvBuf.DataFmt,
                DSP_GetU64Msb((UINT64)pViewZoneCfg->pLaneDetectBuf->YuvBuf.BaseAddrY),
                DSP_GetU64Lsb((UINT64)pViewZoneCfg->pLaneDetectBuf->YuvBuf.BaseAddrY),
                DSP_GetU64Msb((UINT64)pViewZoneCfg->pLaneDetectBuf->YuvBuf.BaseAddrUV),
                DSP_GetU64Lsb((UINT64)pViewZoneCfg->pLaneDetectBuf->YuvBuf.BaseAddrUV));
        AmbaLL_LogUInt5("                    Pitch[%d] Width[%d] Height[%d]",
                pViewZoneCfg->pLaneDetectBuf->YuvBuf.Pitch, pViewZoneCfg->pLaneDetectBuf->YuvBuf.Window.Width,
                pViewZoneCfg->pLaneDetectBuf->YuvBuf.Window.Height, 0U, 0U);
        if ((pViewZoneCfg->pLaneDetectBuf->AllocType == ALLOC_EXTERNAL_DISTINCT) && (ULAddr > 0U)) {
            for (j = 0; j < pViewZoneCfg->pLaneDetectBuf->BufNum; j++) {
                AmbaLL_LogUInt5("               TblIdx[%d] Addr[0x%X%X]", j,
                        DSP_GetU64Msb((UINT64)pViewZoneCfg->pLaneDetectBuf->pYuvBufTbl[j]),
                        DSP_GetU64Lsb((UINT64)pViewZoneCfg->pLaneDetectBuf->pYuvBufTbl[j]), 0U, 0U);
            }
        }
    } else {
        AmbaLL_LogUInt5("       pViewZoneCfg->pLaneDetectBuf[NULL]", 0U, 0U, 0U, 0U, 0U);
    }
}

static inline void dump_LvVzMainY12Buf(const AMBA_DSP_LIVEVIEW_VIEWZONE_CFG_s* pViewZoneCfg)
{
    UINT32 j;
    ULONG ULAddr;

    if (pViewZoneCfg->pMainY12Buf != NULL) {
        AmbaLL_LogUInt5("           MainY12", 0U, 0U, 0U, 0U, 0U);
        if (pViewZoneCfg->pMainY12Buf ->pYuvBufTbl != NULL) {
            dsp_osal_typecast(&ULAddr, &pViewZoneCfg->pMainY12Buf ->pYuvBufTbl);
        } else {
            ULAddr = 0U;
        }
        AmbaLL_LogUInt5("             Alloc[%d] BufNum[%d] BufTbl[0x%X%X]",
                                    pViewZoneCfg->pMainY12Buf ->AllocType,
                                    pViewZoneCfg->pMainY12Buf ->BufNum,
                                    DSP_GetU64Msb((UINT64)ULAddr),
                                    DSP_GetU64Lsb((UINT64)ULAddr), 0U);
        AmbaLL_LogUInt5("             YuvBuf.DataFmt[%d] BaseAddrY[0x%X%X] BaseAddrUV[0x%X%X]",
                pViewZoneCfg->pMainY12Buf ->YuvBuf.DataFmt,
                DSP_GetU64Msb((UINT64)pViewZoneCfg->pMainY12Buf ->YuvBuf.BaseAddrY),
                DSP_GetU64Lsb((UINT64)pViewZoneCfg->pMainY12Buf ->YuvBuf.BaseAddrY),
                DSP_GetU64Msb((UINT64)pViewZoneCfg->pMainY12Buf ->YuvBuf.BaseAddrUV),
                DSP_GetU64Lsb((UINT64)pViewZoneCfg->pMainY12Buf ->YuvBuf.BaseAddrUV));
        AmbaLL_LogUInt5("                    Pitch[%d] Width[%d] Height[%d]",
                pViewZoneCfg->pMainY12Buf ->YuvBuf.Pitch, pViewZoneCfg->pMainY12Buf ->YuvBuf.Window.Width,
                pViewZoneCfg->pMainY12Buf ->YuvBuf.Window.Height, 0U, 0U);
        if ((pViewZoneCfg->pMainY12Buf ->AllocType == ALLOC_EXTERNAL_DISTINCT) && (ULAddr > 0U)) {
            for (j = 0U; j < pViewZoneCfg->pMainY12Buf->BufNum; j++) {
                AmbaLL_LogUInt5("               TblIdx[%d] Addr[0x%X%X]",
                        j, DSP_GetU64Msb((UINT64)pViewZoneCfg->pMainY12Buf->pYuvBufTbl[j]),
                        DSP_GetU64Lsb((UINT64)pViewZoneCfg->pMainY12Buf->pYuvBufTbl[j]), 0U, 0U);
            }
        }
    } else {
        AmbaLL_LogUInt5("       pViewZoneCfg->pMainY12Buf[NULL]", 0U, 0U, 0U, 0U, 0U);
    }
}

static inline void dump_LvVzMainIrBuf(const AMBA_DSP_LIVEVIEW_VIEWZONE_CFG_s* pViewZoneCfg)
{
    UINT32 j;
    ULONG ULAddr;

    if (pViewZoneCfg->pMainIrBuf != NULL) {
        AmbaLL_LogUInt5("           MainIr", 0U, 0U, 0U, 0U, 0U);
        if (pViewZoneCfg->pMainIrBuf ->pYuvBufTbl != NULL) {
            dsp_osal_typecast(&ULAddr, &pViewZoneCfg->pMainIrBuf ->pYuvBufTbl);
        } else {
            ULAddr = 0U;
        }
        AmbaLL_LogUInt5("             Alloc[%d] BufNum[%d] BufTbl[0x%X%X]",
                                    pViewZoneCfg->pMainIrBuf ->AllocType,
                                    pViewZoneCfg->pMainIrBuf ->BufNum,
                                    DSP_GetU64Msb((UINT64)ULAddr),
                                    DSP_GetU64Lsb((UINT64)ULAddr), 0U);
        AmbaLL_LogUInt5("             YuvBuf.DataFmt[%d] BaseAddrY[0x%X%X] BaseAddrUV[0x%X%X]",
                pViewZoneCfg->pMainIrBuf ->YuvBuf.DataFmt,
                DSP_GetU64Msb((UINT64)pViewZoneCfg->pMainIrBuf ->YuvBuf.BaseAddrY),
                DSP_GetU64Lsb((UINT64)pViewZoneCfg->pMainIrBuf ->YuvBuf.BaseAddrY),
                DSP_GetU64Msb((UINT64)pViewZoneCfg->pMainIrBuf ->YuvBuf.BaseAddrUV),
                DSP_GetU64Lsb((UINT64)pViewZoneCfg->pMainIrBuf ->YuvBuf.BaseAddrUV));
        AmbaLL_LogUInt5("                    Pitch[%d] Width[%d] Height[%d]",
                pViewZoneCfg->pMainIrBuf ->YuvBuf.Pitch, pViewZoneCfg->pMainIrBuf ->YuvBuf.Window.Width,
                pViewZoneCfg->pMainIrBuf ->YuvBuf.Window.Height, 0U, 0U);
        if ((pViewZoneCfg->pMainIrBuf ->AllocType == ALLOC_EXTERNAL_DISTINCT) && (ULAddr > 0U)) {
            for (j = 0U; j < pViewZoneCfg->pMainIrBuf->BufNum; j++) {
                AmbaLL_LogUInt5("               TblIdx[%d] Addr[0x%X%X]",
                        j, DSP_GetU64Msb((UINT64)pViewZoneCfg->pMainIrBuf->pYuvBufTbl[j]),
                        DSP_GetU64Lsb((UINT64)pViewZoneCfg->pMainIrBuf->pYuvBufTbl[j]), 0U, 0U);
            }
        }
    } else {
        AmbaLL_LogUInt5("       pViewZoneCfg->pMainIrBuf[NULL]", 0U, 0U, 0U, 0U, 0U);
    }
}

static inline void dump_LiveviewVZCfg(const UINT16 NumViewZone,
                                      const AMBA_DSP_LIVEVIEW_VIEWZONE_CFG_s* pViewZoneCfg)
{
    UINT32 i, j;
    UINT32 MuxSelSrc, MuxSelHorDec, MuxSelVerDec;

    for (i = 0; i < NumViewZone; i++) {
        AmbaLL_LogUInt5("         ViewId[%d] VinId[0x%X] VinSrc[%d] IsVirt[0x%X] VirtChId[0x%X]",
                pViewZoneCfg[i].ViewZoneId, pViewZoneCfg[i].VinId,
                pViewZoneCfg[i].VinSource, pViewZoneCfg[i].IsVirtChan, pViewZoneCfg[i].VinVirtChId);
        MuxSelSrc = DSP_GetBit(pViewZoneCfg[i].InputMuxSelCfg, VZ_IN_MUXSEL_SRC_BIT_IDX, VZ_IN_MUXSEL_SRC_LEN);
        MuxSelHorDec = DSP_GetBit(pViewZoneCfg[i].InputMuxSelCfg, VZ_IN_MUXSEL_HOR_DEC_BIT_IDX, VZ_IN_MUXSEL_DEC_LEN);
        MuxSelVerDec = DSP_GetBit(pViewZoneCfg[i].InputMuxSelCfg, VZ_IN_MUXSEL_VER_DEC_BIT_IDX, VZ_IN_MUXSEL_DEC_LEN);
        AmbaLL_LogUInt5("         Deci[%d] DramU[%d] MuxSel[%d %d %d]",
                pViewZoneCfg[i].VinDecimation, pViewZoneCfg[i].DramUtilityPipe,
                MuxSelSrc, MuxSelHorDec, MuxSelVerDec);
        AmbaLL_LogUInt5("         ExHwarp[0x%X]", pViewZoneCfg[i].ExtraHorWarp, 0U, 0U, 0U, 0U);
#ifdef SUPPORT_IDSP_AFFINITY
        AmbaLL_LogUInt5("         Aff[0x%X]",
                DSP_GetU16Bit(pViewZoneCfg[i].IdspAffinity, IDSP_AFFINITY_CORE_BIT_IDX, IDSP_AFFINITY_CORE_LEN),
                0U, 0U, 0U, 0U);
#endif
        AmbaLL_LogUInt5("           ROI[%d %d %d %d]",
                pViewZoneCfg[i].VinRoi.Width, pViewZoneCfg[i].VinRoi.Height,
                pViewZoneCfg[i].VinRoi.OffsetX, pViewZoneCfg[i].VinRoi.OffsetY, 0U);
        AmbaLL_LogUInt5("           MainWin[%d %d] Rot[%d] ExpNum[0x%X] HdrH[%d]",
                pViewZoneCfg[i].MainWidth, pViewZoneCfg[i].MainHeight, pViewZoneCfg[i].RotateFlip,
                pViewZoneCfg[i].HdrExposureNum, pViewZoneCfg[i].HdrBlendHieght);
        AmbaLL_LogUInt5("           MctfDisable[%d] MctsDisable[%d] LCE[%d]",
                pViewZoneCfg[i].MctfDisable, pViewZoneCfg[i].MctsDisable,
                pViewZoneCfg[i].LinearCE, 0U, 0U);
        AmbaLL_LogUInt5("           Pymd[0x%X] IsPolySqrt[0x%X] PolyWin[%dx%d] Tag[0x%X]",
                pViewZoneCfg[i].Pyramid.HierBit, pViewZoneCfg[i].Pyramid.IsPolySqrt2,
                pViewZoneCfg[i].Pyramid.HierPolyWidth, pViewZoneCfg[i].Pyramid.HierPolyHeight, pViewZoneCfg[i].Pyramid.HierTag);
        for (j=0; j< AMBA_DSP_MAX_HIER_NUM; j++) {
            AmbaLL_LogUInt5("             HierCropWin[%d] [%dx%d] [%d %d]",
                    j, pViewZoneCfg[i].Pyramid.HierCropWindow[j].Width, pViewZoneCfg[i].Pyramid.HierCropWindow[j].Height,
                    pViewZoneCfg[i].Pyramid.HierCropWindow[j].OffsetX, pViewZoneCfg[i].Pyramid.HierCropWindow[j].OffsetY);
        }
        dump_LvVzPymdBuf(&pViewZoneCfg[i]);

        AmbaLL_LogUInt5("           Lndt CropWin[%dx%d] [%d %d]",
                pViewZoneCfg[i].LaneDetect.CropWindow.Width, pViewZoneCfg[i].LaneDetect.CropWindow.Height,
                pViewZoneCfg[i].LaneDetect.CropWindow.OffsetX, pViewZoneCfg[i].LaneDetect.CropWindow.OffsetY, 0U);
        AmbaLL_LogUInt5("                HierSrc[%d] Win[%d %d]",
                pViewZoneCfg[i].LaneDetect.HierSource, pViewZoneCfg[i].LaneDetect.CropWindow.Width,
                pViewZoneCfg[i].LaneDetect.CropWindow.Height, 0U, 0U);
        dump_LvVzLndtBuf(&pViewZoneCfg[i]);

        dump_LvVzMainY12Buf(&pViewZoneCfg[i]);

        dump_LvVzMainIrBuf(&pViewZoneCfg[i]);
    }
}

static inline void dump_LiveviewStrmCfg(const UINT16 NumYUVStream,
                                        const AMBA_DSP_LIVEVIEW_STREAM_CFG_s *pYUVStrmCfg)
{
    UINT32 i, j, k;

    for (i = 0; i < NumYUVStream; i++) {
        AmbaLL_LogUInt5("         StrmId[%d] Purpose[0x%X] VoutDest[0x%X] EncDest[0x%X]",
                pYUVStrmCfg[i].StreamId, pYUVStrmCfg[i].Purpose,
                pYUVStrmCfg[i].DestVout, pYUVStrmCfg[i].DestEnc, 0U);
        AmbaLL_LogUInt5("           Win[%d %d] MaxWin[%d %d]",
                pYUVStrmCfg[i].Width, pYUVStrmCfg[i].Height,
                pYUVStrmCfg[i].MaxWidth, pYUVStrmCfg[i].MaxHeight, 0U);
        AmbaLL_LogUInt5("           Alloc[%d] BufNum[%d]",
                pYUVStrmCfg[i].StreamBuf.AllocType, pYUVStrmCfg[i].StreamBuf.BufNum, 0U, 0U, 0U);
        AmbaLL_LogUInt5("           YuvBuf.DataFmt[%d] BaseAddrY[0x%X%X] BaseAddrUV[0x%X%X]",
                pYUVStrmCfg[i].StreamBuf.YuvBuf.DataFmt,
                DSP_GetU64Msb((UINT64)pYUVStrmCfg[i].StreamBuf.YuvBuf.BaseAddrY),
                DSP_GetU64Lsb((UINT64)pYUVStrmCfg[i].StreamBuf.YuvBuf.BaseAddrY),
                DSP_GetU64Msb((UINT64)pYUVStrmCfg[i].StreamBuf.YuvBuf.BaseAddrUV),
                DSP_GetU64Lsb((UINT64)pYUVStrmCfg[i].StreamBuf.YuvBuf.BaseAddrUV));
        AmbaLL_LogUInt5("                  Pitch[%d] Width[%d] Height[%d]",
                pYUVStrmCfg[i].StreamBuf.YuvBuf.Pitch, pYUVStrmCfg[i].StreamBuf.YuvBuf.Window.Width,
                pYUVStrmCfg[i].StreamBuf.YuvBuf.Window.Height, 0U, 0U);
        AmbaLL_LogUInt5("           NumChan[%d]", pYUVStrmCfg[i].NumChan, 0U, 0U, 0U, 0U);

        if (pYUVStrmCfg[i].pChanCfg != NULL) {
            for (j = 0; j < pYUVStrmCfg[i].NumChan; j++) {
                const AMBA_DSP_LIVEVIEW_CHANNEL_WINDOW_s *pLvChanCfg = &pYUVStrmCfg[i].pChanCfg[j];

                AmbaLL_LogUInt5("             [%d] ViewId[%d] Rot[%d] BldNum[%d]",
                        j, pLvChanCfg->ViewZoneId, pLvChanCfg->RotateFlip, pLvChanCfg->BlendNum, 0U);
                AmbaLL_LogUInt5("                  ROI[%d %d %d %d]",
                        pLvChanCfg->ROI.Width, pLvChanCfg->ROI.Height,
                        pLvChanCfg->ROI.OffsetX, pLvChanCfg->ROI.OffsetY, 0U);
                AmbaLL_LogUInt5("                  Win[%d %d %d %d]",
                        pLvChanCfg->Window.Width, pLvChanCfg->Window.Height,
                        pLvChanCfg->Window.OffsetX, pLvChanCfg->Window.OffsetY, 0U);
                if (pLvChanCfg->LumaAlphaTable > 0U) {
                    const AMBA_DSP_BUF_s *pBldBuf;

                    dsp_osal_typecast(&pBldBuf, &pLvChanCfg->LumaAlphaTable);
                    for (k = 0; k < pLvChanCfg->BlendNum; k++) {
                        AmbaLL_LogUInt5("                  Bld[%d] Addr[0x%X%X] P[%d]", k,
                                DSP_GetU64Msb((UINT64)pBldBuf[k].BaseAddr),
                                DSP_GetU64Lsb((UINT64)pBldBuf[k].BaseAddr), pBldBuf[k].Pitch, 0U);
                        AmbaLL_LogUInt5("                    Win[%d %d %d %d]",
                                pBldBuf[k].Window.Width, pBldBuf[k].Window.Height,
                                pBldBuf[k].Window.OffsetX, pBldBuf[k].Window.OffsetY, 0U);
                    }
                }
            }
        } else {
            AmbaLL_LogUInt5("       pYUVStrmCfg[%d].pChanCfg[NULL]", i, 0U, 0U, 0U, 0U);
        }
        AmbaLL_LogUInt5("           MaxChanBitMask[0x%X] MaxChanNum[%d]", pYUVStrmCfg[i].MaxChanBitMask, pYUVStrmCfg[i].MaxChanNum, 0U, 0U, 0U);
        if (pYUVStrmCfg[i].pMaxChanWin != NULL) {
            for (j = 0; j < AMBA_DSP_MAX_VIEWZONE_NUM; j++) {
                if (1U == DSP_GetBit(pYUVStrmCfg[i].MaxChanBitMask, j, 1U)) {
                    const AMBA_DSP_WINDOW_DIMENSION_s *pWin = &pYUVStrmCfg[i].pMaxChanWin[j];

                    AmbaLL_LogUInt5("             ViewId[%d] MaxWin[%d %d]",
                            j, pWin->Width, pWin->Height, 0U, 0U);
                }
            }
        } else {
            AmbaLL_LogUInt5("       pYUVStrmCfg[%d].pMaxChanWin[NULL]", i, 0U, 0U, 0U, 0U);
        }
        AmbaLL_LogUInt5("           DestDeciRate[%d]", pYUVStrmCfg[i].DestDeciRate, 0U, 0U, 0U, 0U);
    }
}

/* Liveview */
static void dump_LiveviewConfig(const UINT16 NumViewZone,
                                const AMBA_DSP_LIVEVIEW_VIEWZONE_CFG_s* pViewZoneCfg,
                                const UINT16 NumYUVStream,
                                const AMBA_DSP_LIVEVIEW_STREAM_CFG_s *pYUVStrmCfg)
{
    AmbaLL_LogUInt5(" LvCfg NumView[%d]", NumViewZone, 0U, 0U, 0U, 0U);

    if (pViewZoneCfg != NULL) {
        dump_LiveviewVZCfg(NumViewZone, pViewZoneCfg);
    } else {
        AmbaLL_LogUInt5("       pViewZoneCfg[NULL]", 0U, 0U, 0U, 0U, 0U);
    }

    AmbaLL_LogUInt5("       NumYuvStrm[%d]", NumYUVStream, 0U, 0U, 0U, 0U);

    if (pYUVStrmCfg != NULL) {
        dump_LiveviewStrmCfg(NumYUVStream, pYUVStrmCfg);
    } else {
        AmbaLL_LogUInt5("       pYUVStrmCfg[NULL]", 0U, 0U, 0U, 0U, 0U);
    }
}

static void dump_LiveviewUpdateConfig(const UINT16 NumYuvStream,
                                      const AMBA_DSP_LIVEVIEW_STREAM_CFG_s *pYuvStrmCfg,
                                      const UINT64 *pAttachedRawSeq)
{
    UINT32 i, j, k;

    //FIXME MISRA
    (void)pAttachedRawSeq;

    AmbaLL_LogUInt5(" LvCfgUpdate NumYuvStream[%d]", NumYuvStream, 0U, 0U, 0U, 0U);
    if (pYuvStrmCfg != NULL) {
        for (i = 0; i < NumYuvStream; i++) {
            AmbaLL_LogUInt5("         StrmId[%d] Purpose[0x%X] VoutDest[0x%X] EncDest[0x%X]",
                    pYuvStrmCfg[i].StreamId, pYuvStrmCfg[i].Purpose,
                    pYuvStrmCfg[i].DestVout, pYuvStrmCfg[i].DestEnc, 0U);
            AmbaLL_LogUInt5("           Win[%d %d] MaxWin[%d %d]",
                    pYuvStrmCfg[i].Width, pYuvStrmCfg[i].Height,
                    pYuvStrmCfg[i].MaxWidth, pYuvStrmCfg[i].MaxHeight, 0U);
            AmbaLL_LogUInt5("           Alloc[%d] BufNum[%d]",
                    pYuvStrmCfg[i].StreamBuf.AllocType, pYuvStrmCfg[i].StreamBuf.BufNum, 0U, 0U, 0U);
            AmbaLL_LogUInt5("           YuvBuf.DataFmt[%d] BaseAddrY[0x%X%X] BaseAddrUV[0x%X%X]",
                    pYuvStrmCfg[i].StreamBuf.YuvBuf.DataFmt,
                    DSP_GetU64Msb((UINT64)pYuvStrmCfg[i].StreamBuf.YuvBuf.BaseAddrY),
                    DSP_GetU64Lsb((UINT64)pYuvStrmCfg[i].StreamBuf.YuvBuf.BaseAddrY),
                    DSP_GetU64Msb((UINT64)pYuvStrmCfg[i].StreamBuf.YuvBuf.BaseAddrUV),
                    DSP_GetU64Lsb((UINT64)pYuvStrmCfg[i].StreamBuf.YuvBuf.BaseAddrUV));
            AmbaLL_LogUInt5("                  Pitch[%d] Width[%d] Height[%d]",
                    pYuvStrmCfg[i].StreamBuf.YuvBuf.Pitch, pYuvStrmCfg[i].StreamBuf.YuvBuf.Window.Width,
                    pYuvStrmCfg[i].StreamBuf.YuvBuf.Window.Height, 0U, 0U);
            AmbaLL_LogUInt5("           NumChan[%d]", pYuvStrmCfg[i].NumChan, 0U, 0U, 0U, 0U);

            if (pYuvStrmCfg[i].pChanCfg != NULL) {
                for (j = 0; j < pYuvStrmCfg[i].NumChan; j++) {
                    const AMBA_DSP_LIVEVIEW_CHANNEL_WINDOW_s *pLvChanCfg = &pYuvStrmCfg[i].pChanCfg[j];

                    AmbaLL_LogUInt5("             [%d] ViewId[%d] Rot[%d] BldNum[%d]",
                            j, pLvChanCfg->ViewZoneId, pLvChanCfg->RotateFlip,
                            pLvChanCfg->BlendNum, 0U);
                    AmbaLL_LogUInt5("                  ROI[%d %d %d %d]",
                            pLvChanCfg->ROI.Width, pLvChanCfg->ROI.Height,
                            pLvChanCfg->ROI.OffsetX, pLvChanCfg->ROI.OffsetY, 0U);
                    AmbaLL_LogUInt5("                  Win[%d %d %d %d]",
                            pLvChanCfg->Window.Width, pLvChanCfg->Window.Height,
                            pLvChanCfg->Window.OffsetX, pLvChanCfg->Window.OffsetY, 0U);

                    if (pLvChanCfg->LumaAlphaTable > 0U) {
                        const AMBA_DSP_BUF_s *pBldBuf;

                        dsp_osal_typecast(&pBldBuf, &pLvChanCfg->LumaAlphaTable);
                        for (k=0U; k<pLvChanCfg->BlendNum; k++) {
                            AmbaLL_LogUInt5("                  Bld[%d] Addr[0x%X%X] P[%d]", k,
                                    DSP_GetU64Msb((UINT64)pBldBuf[k].BaseAddr),
                                    DSP_GetU64Lsb((UINT64)pBldBuf[k].BaseAddr), pBldBuf[k].Pitch, 0U);
                            AmbaLL_LogUInt5("                    Win[%d %d %d %d]",
                                    pBldBuf[k].Window.Width, pBldBuf[k].Window.Height,
                                    pBldBuf[k].Window.OffsetX, pBldBuf[k].Window.OffsetY, 0U);
                        }
                    }
                }
            } else {
                AmbaLL_LogUInt5("             ppYUVStrmCfg[%d].pChanCfg[NULL]", i, 0U, 0U, 0U, 0U);
            }
            AmbaLL_LogUInt5("           MaxChanBitMask[0x%X] MaxChanNum[%d]", pYuvStrmCfg[i].MaxChanBitMask, pYuvStrmCfg[i].MaxChanNum, 0U, 0U, 0U);
            if (pYuvStrmCfg[i].pMaxChanWin != NULL) {
                for (j = 0; j < AMBA_DSP_MAX_VIEWZONE_NUM; j++) {
                    if (1U == DSP_GetBit(pYuvStrmCfg[i].MaxChanBitMask, j, 1U)) {
                        const AMBA_DSP_WINDOW_DIMENSION_s *pWin = &pYuvStrmCfg[i].pMaxChanWin[j];

                        AmbaLL_LogUInt5("             ViewId[%d] MaxWin[%d %d]",
                                j, pWin->Width, pWin->Height, 0U, 0U);
                    }
                }
            } else {
                AmbaLL_LogUInt5("       pYUVStrmCfg[%d].pMaxChanWin[NULL]", i, 0U, 0U, 0U, 0U);
            }
            AmbaLL_LogUInt5("           DestDeciRate[%d]", pYuvStrmCfg[i].DestDeciRate, 0U, 0U, 0U, 0U);
        }
    } else {
        AmbaLL_LogUInt5("             pYuvStrmCfg[NULL]", 0U, 0U, 0U, 0U, 0U);
    }
}

static void dump_LiveviewCtrl(const UINT16 NumViewZone,
                              const UINT16 *pViewZoneId,
                              const UINT8* pEnable)
{
    UINT32 i;

    AmbaLL_LogUInt5(" LvCtrl NumView[%d]", NumViewZone, 0U, 0U, 0U, 0U);
    if ((pViewZoneId != NULL) && (pEnable != NULL)) {
        for (i = 0; i < NumViewZone; i++) {
            AmbaLL_LogUInt5("         ViewId[%d] Enable[%d]",
                    pViewZoneId[i], pEnable[i], 0U, 0U, 0U);
        }
    } else {
        AmbaLL_LogUInt5("        pViewZoneId[NULL] pEnable[NULL]", 0U, 0U, 0U, 0U, 0U);
    }
}

static void dump_LiveviewUpdateIsoCfg(const UINT16 NumViewZone,
                                      const AMBA_DSP_LIVEVIEW_ISOCFG_CTRL_s *pIsoCfgCtrl,
                                      const UINT64 *pAttachedRawSeq)
{
    UINT32 i;

    //FIXME MISRA
    (void)pAttachedRawSeq;

    AmbaLL_LogUInt5(" LvIsoCfgUpdate NumView[%d]", NumViewZone, 0U, 0U, 0U, 0U);
    if (pIsoCfgCtrl != NULL) {
        for (i = 0; i < NumViewZone; i++) {
            AmbaLL_LogUInt5("                ViewId[%d] CtxIdx[0x%X] CfgIdx[0x%X] CfgAddr[0x%X%X]",
                      pIsoCfgCtrl[i].ViewZoneId,
                      pIsoCfgCtrl[i].CtxIndex, pIsoCfgCtrl[i].CfgIndex,
                      DSP_GetU64Msb((UINT64)pIsoCfgCtrl[i].CfgAddress),
                      DSP_GetU64Lsb((UINT64)pIsoCfgCtrl[i].CfgAddress));
            AmbaLL_LogUInt5("                HdrCfgIdx[0x%X]",
                      pIsoCfgCtrl[i].HdrCfgIndex, 0U, 0U, 0U, 0U);
        }
    } else {
        AmbaLL_LogUInt5("                pIsoCfgCtrl[NULL]", 0U, 0U, 0U, 0U, 0U);
    }
}

static void dump_LiveviewUpdateVinCfg(const UINT16 VinId,
                                      const UINT16 SubChNum,
                                      const AMBA_DSP_VIN_SUB_CHAN_s *pSubCh,
                                      const AMBA_DSP_LIVEVIEW_VINCFG_CTRL_s *pLvVinCfgCtrl,
                                      const UINT64 *pAttachedRawSeq)
{
    UINT32 i;

    //FIXME MISRA
    (void)pAttachedRawSeq;

    AmbaLL_LogUInt5(" LvVinCfgUpdate VinId[%d] SubChNum[%d]", VinId, SubChNum, 0U, 0U, 0U);
    for (i = 0; i < SubChNum; i++) {
        if ((pSubCh != NULL) && (pLvVinCfgCtrl != NULL)) {
            AmbaLL_LogUInt5("                Idx[%d] Virt[%d]", pSubCh[i].Index, pSubCh[i].IsVirtual, 0U, 0U, 0U);
            AmbaLL_LogUInt5("                  EnVinCompand[%d] CompandAddr[0x%X%X] DecompandAddr[0x%X%X]",
                        pLvVinCfgCtrl[i].EnaVinCompand,
                        DSP_GetU64Msb((UINT64)pLvVinCfgCtrl[i].VinCompandTableAddr),
                        DSP_GetU64Lsb((UINT64)pLvVinCfgCtrl[i].VinCompandTableAddr),
                        DSP_GetU64Msb((UINT64)pLvVinCfgCtrl[i].VinDeCompandTableAddr),
                        DSP_GetU64Lsb((UINT64)pLvVinCfgCtrl[i].VinDeCompandTableAddr));
        }
    }
}

static void dump_LiveviewUpdatePymdCfg(const UINT16 NumViewZone,
                                       const UINT16 *pViewZoneId,
                                       const AMBA_DSP_LIVEVIEW_PYRAMID_s *pPyramid,
                                       const AMBA_DSP_LIVEVIEW_YUV_BUF_s *pPyramidBuf,
                                       const UINT64 *pAttachedRawSeq)
{
    UINT32 i, j;

    //FIXME MISRA
    (void)pAttachedRawSeq;

    AmbaLL_LogUInt5(" LvPymdBufUpdate NumView[%d]", NumViewZone, 0U, 0U, 0U, 0U);
    if ((pViewZoneId != NULL) && (pPyramid != NULL)) {
        for (i = 0; i < NumViewZone; i++) {
            AmbaLL_LogUInt5("           ViewId[%d]",
                                        pViewZoneId[i], 0U, 0U, 0U, 0U);
            AmbaLL_LogUInt5("               Pymd[0x%X] IsPolySqrt[0x%X] PolyWin[%dx%d] Tag[0x%X]",
                                        pPyramid[i].HierBit, pPyramid[i].IsPolySqrt2, pPyramid[i].HierPolyWidth, pPyramid[i].HierPolyHeight, pPyramid[i].HierTag);

            if (pPyramidBuf != NULL) {
                ULONG ULAddr;

                if (pPyramidBuf[i].pYuvBufTbl != NULL) {
                    dsp_osal_typecast(&ULAddr, &pPyramidBuf[i].pYuvBufTbl);
                } else {
                    ULAddr = 0U;
                }
                AmbaLL_LogUInt5("                 Alloc[%d] BufNum[%d] BufTbl[0x%X%X]",
                        pPyramidBuf[i].AllocType,
                        pPyramidBuf[i].BufNum,
                        DSP_GetU64Msb((UINT64)ULAddr), DSP_GetU64Lsb((UINT64)ULAddr), 0U);
                AmbaLL_LogUInt5("                 YuvBuf.DataFmt[%d] BaseAddrY[0x%X%X] BaseAddrUV[0x%X%X]",
                        pPyramidBuf[i].YuvBuf.DataFmt,
                        DSP_GetU64Msb((UINT64)pPyramidBuf[i].YuvBuf.BaseAddrY),
                        DSP_GetU64Lsb((UINT64)pPyramidBuf[i].YuvBuf.BaseAddrY),
                        DSP_GetU64Msb((UINT64)pPyramidBuf[i].YuvBuf.BaseAddrUV),
                        DSP_GetU64Lsb((UINT64)pPyramidBuf[i].YuvBuf.BaseAddrUV));
                AmbaLL_LogUInt5("                        Pitch[%d] Width[%d] Height[%d]",
                        pPyramidBuf[i].YuvBuf.Pitch, pPyramidBuf[i].YuvBuf.Window.Width,
                        pPyramidBuf[i].YuvBuf.Window.Height, 0U, 0U);
                if ((pPyramidBuf[i].AllocType == ALLOC_EXTERNAL_DISTINCT) && (ULAddr > 0U)) {
                    for (j = 0; j < pPyramidBuf[i].BufNum; j++) {
                        AmbaLL_LogUInt5("                   TblIdx[%d] Addr[0x%X%X]", j,
                                DSP_GetU64Msb((UINT64)pPyramidBuf[i].pYuvBufTbl[j]),
                                DSP_GetU64Lsb((UINT64)pPyramidBuf[i].pYuvBufTbl[j]), 0U, 0U);
                    }
                }
            } else {
                AmbaLL_LogUInt5("                 pPyramidBuf[NULL]", 0U, 0U, 0U, 0U, 0U);
            }
        }
    } else {
        AmbaLL_LogUInt5("                 pViewZoneId[NULL] pPyramid[NULL]", 0U, 0U, 0U, 0U, 0U);
    }
}

static void dump_LiveviewConfigVinCapture(const UINT16 VinId,
                                          const UINT16 SubChNum,
                                          const AMBA_DSP_VIN_SUB_CHAN_CFG_s *pSubChCfg)
{
    UINT32 i, j;

    AmbaLL_LogUInt5(" LvCfgVinCap VinId[0x%X] SubChNum[%d]", VinId, SubChNum, 0U, 0U, 0U);
    for (i = 0; i < SubChNum; i++) {
        if (pSubChCfg != NULL) {
            AmbaLL_LogUInt5("             Idx[0x%X] Virt[%d] Opt[%d] CatNum[%d] IntcNum[%d]",
                    pSubChCfg[i].SubChan.Index, pSubChCfg[i].SubChan.IsVirtual,
                    pSubChCfg[i].Option, pSubChCfg[i].ConCatNum, pSubChCfg[i].IntcNum);
            AmbaLL_LogUInt5("               Win[%d %d %d %d]",
                    pSubChCfg[i].CaptureWindow.Width, pSubChCfg[i].CaptureWindow.Height,
                    pSubChCfg[i].CaptureWindow.OffsetX, pSubChCfg[i].CaptureWindow.OffsetY, 0U);
            AmbaLL_LogUInt5("               TDNum[%d]", pSubChCfg[i].TDNum, 0U, 0U, 0U, 0U);
            if (pSubChCfg[i].pTDFrmNum != NULL) {
                for (j = 0; j < pSubChCfg[i].TDNum; j++) {
                    AmbaLL_LogUInt5("                 [%d] FrmNum[%d]", j, pSubChCfg[i].pTDFrmNum[j], 0U, 0U, 0U);
                }
            }
        }
    }
}

static void dump_LiveviewConfigVinPost(const UINT8 Type, const UINT16 VinId)
{
    AmbaLL_LogUInt5(" LvCfgVinPost VinId[%d] Type[%d]", VinId, Type, 0U, 0U, 0U);
}

static void dump_LiveviewFeedRawData(const UINT16 NumViewZone,
                                     const UINT16 *pViewZoneId,
                                     const AMBA_DSP_LIVEVIEW_EXT_RAW_s *pExtBuf)
{
    UINT32 i;
    UINT32 SeqMsb, SeqLsb, PtsMsb, PtsLsb;

    AmbaLL_LogUInt5(" LvFeedRaw NumView[%d]", NumViewZone, 0U, 0U, 0U, 0U);
    if ((pViewZoneId != NULL) && (pExtBuf != NULL)) {
        for (i = 0; i < NumViewZone; i++) {
            AmbaLL_LogUInt5("           ViewId[%d] VinId[0x%X] IsLast[%d]", pViewZoneId[i], pExtBuf[i].VinId, pExtBuf[i].IsLast, 0U, 0U);
            SeqMsb = DSP_GetU64Msb(pExtBuf[i].CapSequence);
            SeqLsb = DSP_GetU64Lsb(pExtBuf[i].CapSequence);
            PtsMsb = DSP_GetU64Msb(pExtBuf[i].CapPts);
            PtsLsb = DSP_GetU64Lsb(pExtBuf[i].CapPts);
            AmbaLL_LogUInt5("             CapSeq[0x%X%X] Pts[0x%X%X]", SeqMsb, SeqLsb, PtsMsb, PtsLsb, 0U);
            AmbaLL_LogUInt5("             Raw [0x%X%X] Cmpr[%d] P[%d] Win[%dx%d]",
                        DSP_GetU64Msb((UINT64)pExtBuf[i].RawBuffer.BaseAddr),
                        DSP_GetU64Lsb((UINT64)pExtBuf[i].RawBuffer.BaseAddr),
                        pExtBuf[i].RawBuffer.Compressed, pExtBuf[i].RawBuffer.Pitch, 0U);
            AmbaLL_LogUInt5("             Win[%dx%d]", pExtBuf[i].RawBuffer.Window.Width, pExtBuf[i].RawBuffer.Window.Height, 0U, 0U, 0U);
            AmbaLL_LogUInt5("             Ce  [0x%X%X] Cmpr[%d] P[%d] Win[%dx%d]",
                        DSP_GetU64Msb((UINT64)pExtBuf[i].CeBuffer.BaseAddr),
                        DSP_GetU64Lsb((UINT64)pExtBuf[i].CeBuffer.BaseAddr),
                        pExtBuf[i].CeBuffer.Compressed, pExtBuf[i].CeBuffer.Pitch, 0U);
            AmbaLL_LogUInt5("             Win[%dx%d]", pExtBuf[i].CeBuffer.Window.Width, pExtBuf[i].CeBuffer.Window.Height, 0U, 0U, 0U);
        }
    } else {
        AmbaLL_LogUInt5("           pViewZoneId[NULL] pExtBuf[NULL]", 0U, 0U, 0U, 0U, 0U);
    }
}

static void dump_LiveviewFeedYuvData(const UINT16 NumViewZone,
                                     const UINT16 *pViewZoneId,
                                     const AMBA_DSP_EXT_YUV_BUF_s *pExtYuvBuf)
{
    UINT32 i;

    AmbaLL_LogUInt5(" LvFeedYuv NumView[%d]", NumViewZone, 0U, 0U, 0U, 0U);
    if ((pViewZoneId != NULL) && (pExtYuvBuf != NULL)) {
        for (i = 0; i < NumViewZone; i++) {
            AmbaLL_LogUInt5("           ViewId[%d] IsLast[%d]",
                    pViewZoneId[i], pExtYuvBuf[i].IsLast, 0U, 0U, 0U);
            AmbaLL_LogUInt5("             Yuv [0x%X%X 0x%X%X] P[%d]",
                    DSP_GetU64Msb((UINT64)pExtYuvBuf[i].ExtYuvBuf.BaseAddrY),
                    DSP_GetU64Lsb((UINT64)pExtYuvBuf[i].ExtYuvBuf.BaseAddrY),
                    DSP_GetU64Msb((UINT64)pExtYuvBuf[i].ExtYuvBuf.BaseAddrUV),
                    DSP_GetU64Lsb((UINT64)pExtYuvBuf[i].ExtYuvBuf.BaseAddrUV),
                    pExtYuvBuf[i].ExtYuvBuf.Pitch);
            AmbaLL_LogUInt5("             Fmt[%d] Win[%dx%d]",
                    pExtYuvBuf[i].ExtYuvBuf.DataFmt,
                    pExtYuvBuf[i].ExtYuvBuf.Window.Width, pExtYuvBuf[i].ExtYuvBuf.Window.Height, 0U, 0U);
        }
    } else {
        AmbaLL_LogUInt5("           pViewZoneId[NULL] pExtBuf[NULL]", 0U, 0U, 0U, 0U, 0U);
    }
}

static void dump_LiveviewGetIDspCfg(const UINT16 ViewZoneId, const ULONG *CfgAddr)
{
    (void)CfgAddr;
    AmbaLL_LogUInt5(" LvGetIdspCfg ViewId[0x%X]", ViewZoneId, 0U, 0U, 0U, 0U);
}

static void dump_LiveviewSidebandUpdate(const UINT16 ViewZoneId, const UINT8 NumBand, const ULONG *pSidebandBufAddr)
{
    UINT8 i;

    AmbaLL_LogUInt5(" LvSideBand ViewId[%d] NumBand[%d]", ViewZoneId, NumBand, 0U, 0U, 0U);
    if (pSidebandBufAddr != NULL) {
        for (i = 0U; i < NumBand; i++) {
            AmbaLL_LogUInt5("             Idx[%d] SideBandAddr[0x%X%X]", i,
                    DSP_GetU64Msb((UINT64)pSidebandBufAddr[i]),
                    DSP_GetU64Lsb((UINT64)pSidebandBufAddr[i]), 0U, 0U);
        }
    }
}

static void dump_LiveviewYuvStreamSync(const UINT16 YuvStrmIdx,
                                       const AMBA_DSP_LIVEVIEW_SYNC_CTRL_s *pYuvStrmSyncCtrl,
                                       const UINT32 *pSyncJobId,
                                       const UINT64 *pAttachedRawSeq)
{
    //FIXME MISRA
    (void)pSyncJobId;
    (void)pAttachedRawSeq;

    AmbaLL_LogUInt5(" LvYuvStrmSync Idx[%d]", YuvStrmIdx, 0U, 0U, 0U, 0U);
    if (pYuvStrmSyncCtrl != NULL) {
        AmbaLL_LogUInt5("               Mask[0x%X] Opt[%d]", pYuvStrmSyncCtrl->TargetViewZoneMask, pYuvStrmSyncCtrl->Opt, 0U, 0U, 0U);
    }
}

static void dump_LiveviewUpdateGeoCfg(const UINT16 ViewZoneId,
                                      const AMBA_DSP_LIVEVIEW_GEOCFG_CTRL_s *pGeoCfgCtrl,
                                      const UINT64 *pAttachedRawSeq)
{
    //FIXME MISRA
    (void)pAttachedRawSeq;

    AmbaLL_LogUInt5(" LvUpdateGeoCfg ViewId[%d]", ViewZoneId, 0U, 0U, 0U, 0U);
    if (pGeoCfgCtrl != NULL) {
        AmbaLL_LogUInt5("                CfgAddr[0x%X%X] CfgId[%d]",
            DSP_GetU64Msb((UINT64)pGeoCfgCtrl->GeoCfgAddr),
            DSP_GetU64Lsb((UINT64)pGeoCfgCtrl->GeoCfgAddr), pGeoCfgCtrl->GeoCfgId, 0U, 0U);
    }
}

static void dump_LiveviewUpdateVZSrcCfg(const UINT16 NumViewZone,
                                        const AMBA_DSP_LIVEVIEW_VZ_SRC_CFG_s *pVzSrcCfg,
                                        const UINT64 *pAttachedRawSeq)
{
    UINT32 i;

    //FIXME MISRA
    (void)pAttachedRawSeq;

    AmbaLL_LogUInt5(" LvUpdateVZSrc NumView[%d]", NumViewZone, 0U, 0U, 0U, 0U);
    if (pVzSrcCfg != NULL) {
        for (i = 0; i < NumViewZone; i++) {
            AmbaLL_LogUInt5("           ViewId[%d] VinSource[%d]",
                    pVzSrcCfg[i].ViewZoneId, pVzSrcCfg[i].VinSource, 0U, 0U, 0U);
            AmbaLL_LogUInt5("             VinId [%d] VinVirtChId[%d] IsVirtChan[%d] Deci[%d]",
                    pVzSrcCfg[i].VinId, pVzSrcCfg[i].VinVirtChId, pVzSrcCfg[i].IsVirtChan, pVzSrcCfg[i].VinDecimation, 0U);
            AmbaLL_LogUInt5("             VinRoi[%d, %d, %d, %d]",
                    pVzSrcCfg[i].VinRoi.OffsetX, pVzSrcCfg[i].VinRoi.OffsetY,
                    pVzSrcCfg[i].VinRoi.Width, pVzSrcCfg[i].VinRoi.Height, 0U);
        }
    } else {
        AmbaLL_LogUInt5("           pVzSrcCfg[NULL]", 0U, 0U, 0U, 0U, 0U);
    }
}

static void dump_LiveviewSliceCfg(const UINT16 ViewZoneId,
                                  const AMBA_DSP_LIVEVIEW_SLICE_CFG_s *pLvSliceCfg)
{
    AmbaLL_LogUInt5(" LvSliceCfg ViewId[%d]", ViewZoneId, 0U, 0U, 0U, 0U);
    if (pLvSliceCfg != NULL) {
        AmbaLL_LogUInt5("            ColNum[%d] RowNum[%d] EncSyncOpt[%d]",
                pLvSliceCfg->SliceNumCol, pLvSliceCfg->SliceNumRow, pLvSliceCfg->EncSyncOpt, 0U, 0U);
        AmbaLL_LogUInt5("            WaitLine[%d %d] OverLap[%d] VinDrag[%d]",
                pLvSliceCfg->WarpLumaWaitLine, pLvSliceCfg->WarpChromaWaitLine, pLvSliceCfg->WarpOverLap, pLvSliceCfg->VinDragLine, 0U);
    } else {
        AmbaLL_LogUInt5("            pLvSliceCfg[NULL]", 0U, 0U, 0U, 0U, 0U);
    }
}

static void dump_LiveviewSliceCfgCalc(const UINT16 ViewZoneId,
                                      const AMBA_DSP_LIVEVIEW_SLICE_CFG_s *pLvSliceCfg)
{
(void)pLvSliceCfg;
    AmbaLL_LogUInt5(" LvSliceCfgCalc ViewId[0x%X]", ViewZoneId, 0U, 0U, 0U, 0U);
}

static void dump_LiveviewVZPostponeCfg(const UINT16 ViewZoneId, const AMBA_DSP_LV_VZ_POSTPONE_CFG_s *pVzPostPoneCfg)
{
    AmbaLL_LogUInt5(" LvVZPostPoneCfg ViewId[%d]", ViewZoneId, 0U, 0U, 0U, 0U);
    if (pVzPostPoneCfg != NULL) {
        AmbaLL_LogUInt5("                 StageId[%d] Dly[%d]",
                pVzPostPoneCfg->StageId, pVzPostPoneCfg->DelayTimeIn10xMs, 0U, 0U, 0U);
    } else {
        AmbaLL_LogUInt5("                 pVzPostPoneCfg[NULL]", 0U, 0U, 0U, 0U, 0U);
    }
}

static void dump_LiveviewUpdateVinState(const UINT16 NumVin, const AMBA_DSP_LV_VIN_STATE_s *pVinState)
{
    UINT16 i;

    AmbaLL_LogUInt5(" UpdateVinState NumVin[%d]", NumVin, 0U, 0U, 0U, 0U);
    if (pVinState != NULL) {
        for (i = 0U; i < NumVin; i++) {
            AmbaLL_LogUInt5("             [%u]",
                    i, 0U, 0U, 0U, 0U);
            AmbaLL_LogUInt5("                 VinId[%u] SubChan[%u %u] State[%u] Operation[%u]",
                    pVinState[i].VinId,
                    pVinState[i].SubChan.Index,
                    pVinState[i].SubChan.IsVirtual,
                    pVinState[i].State,
                    pVinState[i].Operation);
        }
    } else {
        AmbaLL_LogUInt5("                 pVinLostCfg[NULL]", 0U, 0U, 0U, 0U, 0U);
    }
}

static void dump_LiveviewSlowShutterCtrl(const UINT16 NumViewZone, const UINT16* pViewZoneId, const UINT32* pRatio)
{
    UINT16 i;

    AmbaLL_LogUInt5(" SlowShutterCtrl NumViewZone[%d]", NumViewZone, 0U, 0U, 0U, 0U);
    if (pViewZoneId != NULL) {
        for (i = 0U; i < NumViewZone; i++) {
            AmbaLL_LogUInt5("             [%u]",
                    i, 0U, 0U, 0U, 0U);
            AmbaLL_LogUInt5("                 ViewZoneId[%d] Ratio[%d]", pViewZoneId[i], pRatio[i], 0U, 0U, 0U);
        }
    } else {
        AmbaLL_LogUInt5("                 pViewZoneId[NULL]", 0U, 0U, 0U, 0U, 0U);
    }
}

static void dump_LiveviewDropRepeatCtrl(const UINT16 NumViewZone, const UINT16* pViewZoneId, const AMBA_DSP_LIVEVIEW_DROP_REPEAT_s* pDropRptCfg)
{
    UINT16 i;

    AmbaLL_LogUInt5(" DropRepeatCtrl NumViewZone[%d]", NumViewZone, 0U, 0U, 0U, 0U);
    if (pViewZoneId != NULL) {
        for (i = 0U; i < NumViewZone; i++) {
            AmbaLL_LogUInt5("             [%u]",
                    i, 0U, 0U, 0U, 0U);
            AmbaLL_LogUInt5("                 ViewZoneId[%d] Method[%d] Opt[%d]", pViewZoneId[i], pDropRptCfg[i].Method, pDropRptCfg[i].Option, 0U, 0U);
        }
    } else {
        AmbaLL_LogUInt5("                 pViewZoneId[NULL]", 0U, 0U, 0U, 0U, 0U);
    }
}

/* StillCapture */
static void dump_DataCapCfg(const UINT16 CapInstance, const AMBA_DSP_DATACAP_CFG_s *pDataCapCfg)
{
    if (pDataCapCfg != NULL) {
        const AMBA_DSP_BUF_s *pDspBuf;
        ULONG ULAddr;
        UINT16 i;

        AmbaLL_LogUInt5(" DataCapCfg Inst[%d] Type[%d] AuxData[%d] Idx[0x%X]",
                    CapInstance,
                    pDataCapCfg->CapDataType,
                    pDataCapCfg->AuxDataNeeded,
                    pDataCapCfg->Index, 0U);
        AmbaLL_LogUInt5("            Alloc[%d] Num[%d] OverFlowCtrl[%d] CmprRate[%d] CmptRate[%d]",
                    pDataCapCfg->AllocType, pDataCapCfg->BufNum,
                    pDataCapCfg->OverFlowCtrl, pDataCapCfg->CmprRate, pDataCapCfg->CmptRate);
        AmbaLL_LogUInt5("            HdrExposureNum[0x%X]",
                    pDataCapCfg->HdrExposureNum, 0U, 0U, 0U, 0U);
        pDspBuf = &(pDataCapCfg->DataBuf);
        AmbaLL_LogUInt5("            Data Buf Addr[0x%X%X] P[%d]",
                    DSP_GetU64Msb((UINT64)pDspBuf->BaseAddr),
                    DSP_GetU64Lsb((UINT64)pDspBuf->BaseAddr), pDspBuf->Pitch, 0U, 0U);
        AmbaLL_LogUInt5("                 Win[%d %d %d %d]",
                    pDspBuf->Window.Width, pDspBuf->Window.Height,
                    pDspBuf->Window.OffsetX, pDspBuf->Window.OffsetY, 0U);
        if (pDataCapCfg->pBufTbl != NULL) {
            dsp_osal_typecast(&ULAddr, &pDataCapCfg->pBufTbl);
        } else {
            ULAddr = 0U;
        }
        if ((pDataCapCfg->AllocType == ALLOC_EXTERNAL_DISTINCT) && (ULAddr > 0U)) {
            for (i = 0; i < pDataCapCfg->BufNum; i++) {
                AmbaLL_LogUInt5("                 TblIdx[%d] Addr[0x%X%X]", i,
                        DSP_GetU64Msb((UINT64)pDataCapCfg->pBufTbl[i]),
                        DSP_GetU64Lsb((UINT64)pDataCapCfg->pBufTbl[i]), 0U, 0U);
            }
        }
        pDspBuf = &(pDataCapCfg->AuxDataBuf);
        AmbaLL_LogUInt5("            AuxData Buf Addr[0x%X%X] P[%d]",
                    DSP_GetU64Msb((UINT64)pDspBuf->BaseAddr),
                    DSP_GetU64Lsb((UINT64)pDspBuf->BaseAddr), pDspBuf->Pitch, 0U, 0U);
        AmbaLL_LogUInt5("                    Win[%d %d %d %d]",
                    pDspBuf->Window.Width, pDspBuf->Window.Height,
                    pDspBuf->Window.OffsetX, pDspBuf->Window.OffsetY, 0U);
        if (pDataCapCfg->pAuxBufTbl != NULL) {
            dsp_osal_typecast(&ULAddr, &pDataCapCfg->pAuxBufTbl);
        } else {
            ULAddr = 0U;
        }
        if ((pDataCapCfg->AllocType == ALLOC_EXTERNAL_DISTINCT) && (ULAddr > 0U)) {
            for (i = 0; i < pDataCapCfg->BufNum; i++) {
                AmbaLL_LogUInt5("                 TblIdx[%d] Addr[0x%X%X]", i,
                            DSP_GetU64Msb((UINT64)pDataCapCfg->pAuxBufTbl[i]),
                            DSP_GetU64Lsb((UINT64)pDataCapCfg->pAuxBufTbl[i]), 0U, 0U);
            }
        }
    }
}

static void dump_UpdateCapBuffer(const UINT16 CapInstance, const AMBA_DSP_DATACAP_BUF_CFG_s *pCapBuf, const UINT64 *pAttachedRawSeq)
{
    UINT16 i;
    ULONG ULAddr;

(void)pAttachedRawSeq;
    AmbaLL_LogUInt5(" UpdateCapBuffer Inst[%d]", CapInstance, 0U, 0U, 0U, 0U);

    if (pCapBuf != NULL) {
        AmbaLL_LogUInt5("                 Num[%d] Alloc[%d] BufAddr[0x%X%X]",
                    pCapBuf->BufNum, pCapBuf->AllocType,
                    DSP_GetU64Msb((UINT64)pCapBuf->BufAddr),
                    DSP_GetU64Lsb((UINT64)pCapBuf->BufAddr), 0U);
        AmbaLL_LogUInt5("                 AuxBufAddr[0x%X%X]",
                    DSP_GetU64Msb((UINT64)pCapBuf->AuxBufAddr),
                    DSP_GetU64Lsb((UINT64)pCapBuf->AuxBufAddr), 0U, 0U, 0U);
        if (pCapBuf->pBufTbl != NULL) {
            dsp_osal_typecast(&ULAddr, &pCapBuf->pBufTbl);
        } else {
            ULAddr = 0U;
        }
        if ((pCapBuf->AllocType == ALLOC_EXTERNAL_DISTINCT) && (ULAddr > 0U)) {
            for (i = 0; i < pCapBuf->BufNum; i++) {
                AmbaLL_LogUInt5("                 Buf TblIdx[%d] Addr[0x%X%X]", i,
                    DSP_GetU64Msb((UINT64)pCapBuf->pBufTbl[i]),
                    DSP_GetU64Lsb((UINT64)pCapBuf->pBufTbl[i]), 0U, 0U);
            }
        }

        if (pCapBuf->pAuxBufTbl != NULL) {
            dsp_osal_typecast(&ULAddr, &pCapBuf->pAuxBufTbl);
        } else {
            ULAddr = 0U;
        }
        if ((pCapBuf->AllocType == ALLOC_EXTERNAL_DISTINCT) && (ULAddr > 0U)) {
            for (i = 0; i < pCapBuf->BufNum; i++) {
                AmbaLL_LogUInt5("                 AuxBuf TblIdx[%d] Addr[0x%X%X]", i,
                    DSP_GetU64Msb((UINT64)pCapBuf->pAuxBufTbl[i]),
                    DSP_GetU64Lsb((UINT64)pCapBuf->pAuxBufTbl[i]), 0U, 0U);
            }
        }
    }
}

static void dump_DataCapCtrl(const UINT16 NumCapInstance,
                             const UINT16 *pCapInstance,
                             const AMBA_DSP_DATACAP_CTRL_s *pDataCapCtrl,
                             const UINT64 *pAttachedRawSeq)
{
    UINT16 i;

(void)pAttachedRawSeq;
    AmbaLL_LogUInt5(" DataCapCtrl NumInst[%d]", NumCapInstance, 0U, 0U, 0U, 0U);
    if ((pCapInstance != NULL) && (pDataCapCtrl != NULL)) {
        for (i = 0U; i<NumCapInstance; i++) {
            AmbaLL_LogUInt5("             [%d] InstIdx[%d] CapNum[%d]", i, pCapInstance[i], pDataCapCtrl[i].CapNum, 0U, 0U);
        }
    }
}

static void dump_StillYuv2Yuv(const AMBA_DSP_YUV_IMG_BUF_s *pYuvIn,
                              const AMBA_DSP_YUV_IMG_BUF_s *pYuvOut,
                              const AMBA_DSP_ISOCFG_CTRL_s *pIsoCfg,
                              const UINT32 Opt,
                              const UINT64 *pAttachedRawSeq)
{
(void)pAttachedRawSeq;

    if ((pYuvIn != NULL) && (pYuvOut != NULL)) {
        AmbaLL_LogUInt5(" StlY2Y Opt[%d]", Opt, 0U, 0U, 0U, 0U);
        AmbaLL_LogUInt5("        YuvIn DataFmt[%d] BaseAddrY[0x%X%X] BaseAddrUV[0x%X%X]",
                                 pYuvIn->DataFmt,
                                 DSP_GetU64Msb((UINT64)pYuvIn->BaseAddrY), DSP_GetU64Lsb((UINT64)pYuvIn->BaseAddrY),
                                 DSP_GetU64Msb((UINT64)pYuvIn->BaseAddrUV), DSP_GetU64Lsb((UINT64)pYuvIn->BaseAddrUV));
        AmbaLL_LogUInt5("              Pitch[%d] Window.Width[%d] Window.Height[%d]",
                                 pYuvIn->Pitch, pYuvIn->Window.Width, pYuvIn->Window.Height, 0U, 0U);
        AmbaLL_LogUInt5("        YuvOut DataFmt[%d] BaseAddrY[0x%X%X] BaseAddrUV[0x%X%X]",
                                 pYuvOut->DataFmt,
                                 DSP_GetU64Msb((UINT64)pYuvOut->BaseAddrY), DSP_GetU64Lsb((UINT64)pYuvOut->BaseAddrY),
                                 DSP_GetU64Msb((UINT64)pYuvOut->BaseAddrUV), DSP_GetU64Lsb((UINT64)pYuvOut->BaseAddrUV));
        AmbaLL_LogUInt5("              Pitch[%d] Window.Width[%d] Window.Height[%d]",
                                 pYuvOut->Pitch, pYuvOut->Window.Width, pYuvOut->Window.Height, 0U, 0U);
        if ((Opt == 0U) && (pIsoCfg != NULL)) {
            AmbaLL_LogUInt5("        IsoCfg CtxIndex[0x%X] CfgIndex[0x%X] CfgAddress[0x%X%X] HdrCfgIndex[0x%X]",
                                     pIsoCfg->CtxIndex, pIsoCfg->CfgIndex,
                                     DSP_GetU64Msb((UINT64)pIsoCfg->CfgAddress),
                                     DSP_GetU64Lsb((UINT64)pIsoCfg->CfgAddress), pIsoCfg->HdrCfgIndex);
        }
    }
}

static void dump_StillEncodeCtrl(const UINT16 StreamIdx, const AMBA_DSP_STLENC_CTRL_s *pStlEncCtrl, const UINT64 *pAttachedRawSeq)
{
(void)pAttachedRawSeq;

    AmbaLL_LogUInt5(" StlEnc StreamIdx[%d]", StreamIdx, 0U, 0U, 0U, 0U);
    if (pStlEncCtrl != NULL) {
        AmbaLL_LogUInt5("        QTblAddr[0x%X%X] Qlvl[%d] EncWidth[%d] EncHeight[%d]",
                                 DSP_GetU64Msb((UINT64)pStlEncCtrl->QTblAddr), DSP_GetU64Lsb((UINT64)pStlEncCtrl->QTblAddr),
                                 pStlEncCtrl->QualityLevel, pStlEncCtrl->EncWidth, pStlEncCtrl->EncHeight);
        AmbaLL_LogUInt5("        TargetBitRate[%d] Tolerance[%d] RateCurvPoints[%d] RateCurvAddr[0x%X%X]",
                                 pStlEncCtrl->TargetBitRate, pStlEncCtrl->Tolerance, pStlEncCtrl->RateCurvPoints,
                                 DSP_GetU64Msb((UINT64)pStlEncCtrl->RateCurvAddr), DSP_GetU64Lsb((UINT64)pStlEncCtrl->RateCurvAddr));
        AmbaLL_LogUInt5("        MaxEncLoop[%d] RotateFlip[%d] BitsBufAddr[0x%X%X] BitsBufSize[%d]",
                                 pStlEncCtrl->MaxEncLoop, pStlEncCtrl->RotateFlip,
                                 DSP_GetU64Msb((UINT64)pStlEncCtrl->BitsBufAddr), DSP_GetU64Lsb((UINT64)pStlEncCtrl->BitsBufAddr),
                                 pStlEncCtrl->BitsBufSize);
        AmbaLL_LogUInt5("        Yuv DataFmt[%d] BaseAddrY[0x%X%X] BaseAddrUV[0x%X%X]",
                                 pStlEncCtrl->YuvBuf.DataFmt,
                                 DSP_GetU64Msb((UINT64)pStlEncCtrl->YuvBuf.BaseAddrY), DSP_GetU64Lsb((UINT64)pStlEncCtrl->YuvBuf.BaseAddrY),
                                 DSP_GetU64Msb((UINT64)pStlEncCtrl->YuvBuf.BaseAddrUV), DSP_GetU64Lsb((UINT64)pStlEncCtrl->YuvBuf.BaseAddrUV));
        AmbaLL_LogUInt5("              Pitch[%d] Window.Width[%d] Window.Height[%d]",
                                 pStlEncCtrl->YuvBuf.Pitch, pStlEncCtrl->YuvBuf.Window.Width, pStlEncCtrl->YuvBuf.Window.Height, 0U, 0U);
    }
}

static void dump_CalcStillYuvExtBufSize(const UINT16 StreamIdx, const UINT16 BufType, const UINT16 *pBufPitch, const UINT32 *pBufUnitSize)
{
(void)pBufPitch;
(void)pBufUnitSize;

    AmbaLL_LogUInt5(" CalcStlYuvExtBufSize StreamIdx[%d] BufType[%d]", StreamIdx, BufType, 0U, 0U, 0U);
}

static void dump_StillRaw2Yuv(const AMBA_DSP_RAW_BUF_s *pRawIn,
                              const AMBA_DSP_BUF_s *pAuxBufIn,
                              const AMBA_DSP_YUV_IMG_BUF_s *pYuvOut,
                              const AMBA_DSP_ISOCFG_CTRL_s *pIsoCfg,
                              const UINT32 Opt,
                              const UINT64 *pAttachedRawSeq)
{
(void)pAttachedRawSeq;
(void)Opt;

    if ((pRawIn != NULL) && (pYuvOut != NULL)) {
        UINT32 CeBaseAddr = 0U;

        AmbaLL_LogUInt5(" StlR2Y Opt[%d]", Opt, 0U, 0U, 0U, 0U);
        AmbaLL_LogUInt5("        RawIn Compressed[%d] RawBaseAddr[0x%X%X] CeBaseAddr[0x%X]",
                                 pRawIn->Compressed,
                                 DSP_GetU64Msb((UINT64)pRawIn->BaseAddr), DSP_GetU64Lsb((UINT64)pRawIn->BaseAddr), CeBaseAddr, 0U);
        AmbaLL_LogUInt5("              Pitch[%d] Window.Width[%d] Window.Height[%d]",
                                 pRawIn->Pitch, pRawIn->Window.Width, pRawIn->Window.Height, 0U, 0U);
        if (pAuxBufIn != NULL) {
            AmbaLL_LogUInt5("        AuxIn RawBaseAddr[0x%X%X] Pitch[%d] Window.Width[%d] Window.Height[%d]",
                                 DSP_GetU64Msb((UINT64)pAuxBufIn->BaseAddr), DSP_GetU64Lsb((UINT64)pAuxBufIn->BaseAddr),
                                 pAuxBufIn->Pitch, pAuxBufIn->Window.Width, pAuxBufIn->Window.Height);
        }
        AmbaLL_LogUInt5("        YuvOut DataFmt[%d] BaseAddrY[0x%X%X] BaseAddrUV[0x%X%X]",
                                 pYuvOut->DataFmt,
                                 DSP_GetU64Msb((UINT64)pYuvOut->BaseAddrY), DSP_GetU64Lsb((UINT64)pYuvOut->BaseAddrY),
                                 DSP_GetU64Msb((UINT64)pYuvOut->BaseAddrUV), DSP_GetU64Lsb((UINT64)pYuvOut->BaseAddrUV));
        AmbaLL_LogUInt5("              Pitch[%d] Window.Width[%d] Window.Height[%d]",
                                 pYuvOut->Pitch, pYuvOut->Window.Width, pYuvOut->Window.Height, 0U, 0U);
        if (pIsoCfg != NULL) {
            AmbaLL_LogUInt5("        IsoCfg CtxIndex[0x%X] CfgIndex[0x%X] CfgAddress[0x%X%X] HdrCfgIndex[0x%X]",
                                 pIsoCfg->CtxIndex, pIsoCfg->CfgIndex,
                                 DSP_GetU64Msb((UINT64)pIsoCfg->CfgAddress), DSP_GetU64Lsb((UINT64)pIsoCfg->CfgAddress),
                                 pIsoCfg->HdrCfgIndex);
        }
    } else {
        AmbaLL_LogUInt5("dump_StillRaw2Yuv error", 0U, 0U, 0U, 0U, 0U);
    }
}

static void dump_StillRaw2Raw(const AMBA_DSP_RAW_BUF_s *pRawIn,
                              const AMBA_DSP_BUF_s *pAuxBufIn,
                              const AMBA_DSP_RAW_BUF_s *pRawOut,
                              const AMBA_DSP_ISOCFG_CTRL_s *pIsoCfg,
                              const UINT32 Opt,
                              const UINT64 *pAttachedRawSeq)
{
(void)pAttachedRawSeq;
(void)Opt;

    if ((pRawIn != NULL) && (pRawOut != NULL)) {
        UINT32 CeBaseAddr = 0U;

        AmbaLL_LogUInt5(" StlR2R Opt[%d]", Opt, 0U, 0U, 0U, 0U);
        AmbaLL_LogUInt5("        RawIn Compressed[%d] RawBaseAddr[0x%X%X] CeBaseAddr[0x%X]",
                                 pRawIn->Compressed,
                                 DSP_GetU64Msb((UINT64)pRawIn->BaseAddr), DSP_GetU64Lsb((UINT64)pRawIn->BaseAddr),
                                 CeBaseAddr, 0U);
        AmbaLL_LogUInt5("              Pitch[%d] Window.Width[%d] Window.Height[%d]",
                                 pRawIn->Pitch, pRawIn->Window.Width, pRawIn->Window.Height, 0U, 0U);
        if (pAuxBufIn != NULL) {
            AmbaLL_LogUInt5("        AuxIn RawBaseAddr[0x%X%X] Pitch[%d] Window.Width[%d] Window.Height[%d]",
                                 DSP_GetU64Msb((UINT64)pAuxBufIn->BaseAddr), DSP_GetU64Lsb((UINT64)pAuxBufIn->BaseAddr),
                                 pAuxBufIn->Pitch, pAuxBufIn->Window.Width, pAuxBufIn->Window.Height);
        }
        AmbaLL_LogUInt5("        RawOut Compressed[%d] RawBaseAddr[0x%X%X]",
                                 pRawOut->Compressed,
                                 DSP_GetU64Msb((UINT64)pRawOut->BaseAddr), DSP_GetU64Lsb((UINT64)pRawOut->BaseAddr), 0U, 0U);
        AmbaLL_LogUInt5("              Pitch[%d] Window.Width[%d] Window.Height[%d]",
                                 pRawOut->Pitch, pRawOut->Window.Width, pRawOut->Window.Height, 0U, 0U);
        if (pIsoCfg != NULL) {
            AmbaLL_LogUInt5("        IsoCfg CtxIndex[0x%X] CfgIndex[0x%X] CfgAddress[0x%X%X] HdrCfgIndex[0x%X]",
                                 pIsoCfg->CtxIndex, pIsoCfg->CfgIndex,
                                 DSP_GetU64Msb((UINT64)pIsoCfg->CfgAddress),
                                 DSP_GetU64Lsb((UINT64)pIsoCfg->CfgAddress), pIsoCfg->HdrCfgIndex);
        }
    } else {
        AmbaLL_LogUInt5("dump_StillRaw2Raw error", 0U, 0U, 0U, 0U, 0U);
    }
}

/* VideoEncode */
static void dump_VideoEncConfig(const UINT16 NumStream,
                                const UINT16 *pStreamIdx,
                                const AMBA_DSP_VIDEO_ENC_STRM_CONFIG_s *pStreamConfig)
{
    UINT32 i;

    AmbaLL_LogUInt5(" VdoEncCfg NumStrm[%d]", NumStream, 0U, 0U, 0U, 0U);
    if ((pStreamIdx != NULL) && (pStreamConfig != NULL)) {
        for (i = 0U; i<NumStream; i++) {
            UINT8 IntraRefreshQpAdj8 = 0U;

            (void)dsp_osal_memcpy(&IntraRefreshQpAdj8, &pStreamConfig[i].EncConfig.IntraRefreshCtrl.IntraRefreshQpAdj, sizeof(UINT8));
            AmbaLL_LogUInt5("         StrmId[%d] Fmt[%d] SrcId[%d] IntCap[%d] Aff[0x%X]",
                                        pStreamIdx[i], pStreamConfig[i].CodingFmt,
                                        pStreamConfig[i].SrcYuvStreamId,
                                        pStreamConfig[i].IntervalCapture, pStreamConfig[i].Affinity);
            AmbaLL_LogUInt5("           Win[%d %d %d %d] Rot[%d]",
                                        pStreamConfig[i].Window.Width, pStreamConfig[i].Window.Height,
                                        pStreamConfig[i].Window.OffsetX,
                                        pStreamConfig[i].Window.OffsetY, pStreamConfig[i].Rotate);
            AmbaLL_LogUInt5("           Frate[%d %d %d]",
                                        pStreamConfig[i].FrameRate.Interlace,
                                        pStreamConfig[i].FrameRate.TimeScale,
                                        pStreamConfig[i].FrameRate.NumUnitsInTick, 0U, 0U);
            AmbaLL_LogUInt5("           EncConfig ProfileIDC[%d] LevelIDC[%d] IsCabac[%d] NumPRef[%d] NumBRef[%d]",
                                        pStreamConfig[i].EncConfig.ProfileIDC,
                                        pStreamConfig[i].EncConfig.LevelIDC,
                                        pStreamConfig[i].EncConfig.IsCabac,
                                        pStreamConfig[i].EncConfig.NumPRef,
                                        pStreamConfig[i].EncConfig.NumBRef);
            AmbaLL_LogUInt5("           EncConfig GOPStruct[%d] M[%d] N[%d] IdrInterval[%d] FirstGOPStartB[%d]",
                                        pStreamConfig[i].EncConfig.GOPCfg.GOPStruct,
                                        pStreamConfig[i].EncConfig.GOPCfg.M,
                                        pStreamConfig[i].EncConfig.GOPCfg.N,
                                        pStreamConfig[i].EncConfig.GOPCfg.IdrInterval,
                                        pStreamConfig[i].EncConfig.FirstGOPStartB);
            AmbaLL_LogUInt5("           EncConfig Deblock EnableLoopFilter[%d] LoopFilterAlpha[%d] LoopFilterBeta[%d]",
                                        pStreamConfig[i].EncConfig.DeblockFilter.EnableLoopFilter,
                                        pStreamConfig[i].EncConfig.DeblockFilter.LoopFilterAlpha,
                                        pStreamConfig[i].EncConfig.DeblockFilter.LoopFilterBeta, 0U, 0U);
            AmbaLL_LogUInt5("           EncConfig FrameCrop CroppingFlag[%d] CropLeftOffset[%d] CropRightOffset[%d] CropTopOffset[%d] CropBottomOffset[%d]",
                                        pStreamConfig[i].EncConfig.FrameCrop.CroppingFlag,
                                        pStreamConfig[i].EncConfig.FrameCrop.CropLeftOffset,
                                        pStreamConfig[i].EncConfig.FrameCrop.CropRightOffset,
                                        pStreamConfig[i].EncConfig.FrameCrop.CropTopOffset,
                                        pStreamConfig[i].EncConfig.FrameCrop.CropBottomOffset);
            AmbaLL_LogUInt5("           EncConfig Vui VuiEnable[%d] AspectRatioInfoPresentFlag[%d] OverscanInfoPresentFlag[%d] OverscanAppropriateFlag[%d] VideoSignalTypePresentFlag[%d]",
                                        pStreamConfig[i].EncConfig.Vui.VuiEnable,
                                        pStreamConfig[i].EncConfig.Vui.AspectRatioInfoPresentFlag,
                                        pStreamConfig[i].EncConfig.Vui.OverscanInfoPresentFlag,
                                        pStreamConfig[i].EncConfig.Vui.OverscanAppropriateFlag,
                                        pStreamConfig[i].EncConfig.Vui.VideoSignalTypePresentFlag);
            AmbaLL_LogUInt5("           EncConfig Vui VideoFullRangeFlag[%d] ColourDescriptionPresentFlag[%d] ChromaLocInfoPresentFlag[%d] TimingInfoPresentFlag[%d] FixedFrameRateFlag[%d]",
                                        pStreamConfig[i].EncConfig.Vui.VideoFullRangeFlag,
                                        pStreamConfig[i].EncConfig.Vui.ColourDescriptionPresentFlag,
                                        pStreamConfig[i].EncConfig.Vui.ChromaLocInfoPresentFlag,
                                        pStreamConfig[i].EncConfig.Vui.TimingInfoPresentFlag,
                                        pStreamConfig[i].EncConfig.Vui.FixedFrameRateFlag);
            AmbaLL_LogUInt5("           EncConfig Vui NalHrdParametersPresentFlag[%d] VclHrdParametersPresentFlag[%d] LowDelayHrdFlag[%d] PicStructPresentFlag[%d] BitstreamRestrictionFlag[%d]",
                                        pStreamConfig[i].EncConfig.Vui.NalHrdParametersPresentFlag,
                                        pStreamConfig[i].EncConfig.Vui.VclHrdParametersPresentFlag,
                                        pStreamConfig[i].EncConfig.Vui.LowDelayHrdFlag,
                                        pStreamConfig[i].EncConfig.Vui.PicStructPresentFlag,
                                        pStreamConfig[i].EncConfig.Vui.BitstreamRestrictionFlag);
            AmbaLL_LogUInt5("           EncConfig Vui MotionVectorsOverPicBoundariesFlag[%d] AspectRatioIdc[%d] SarWidth[%d] SarHeight[%d] VideoFormat[%d]",
                                        pStreamConfig[i].EncConfig.Vui.MotionVectorsOverPicBoundariesFlag,
                                        pStreamConfig[i].EncConfig.Vui.AspectRatioIdc,
                                        pStreamConfig[i].EncConfig.Vui.SarWidth,
                                        pStreamConfig[i].EncConfig.Vui.SarHeight,
                                        pStreamConfig[i].EncConfig.Vui.VideoFormat);
            AmbaLL_LogUInt5("           EncConfig Vui ColourPrimaries[%d] TransferCharacteristics[%d] MatrixCoefficients[%d] ChromaSampleLocTypeTopField[%d] ChromaSampleLocTypeBottomField[%d]",
                                        pStreamConfig[i].EncConfig.Vui.ColourPrimaries,
                                        pStreamConfig[i].EncConfig.Vui.TransferCharacteristics,
                                        pStreamConfig[i].EncConfig.Vui.MatrixCoefficients,
                                        pStreamConfig[i].EncConfig.Vui.ChromaSampleLocTypeTopField,
                                        pStreamConfig[i].EncConfig.Vui.ChromaSampleLocTypeBottomField);
            AmbaLL_LogUInt5("           EncConfig Vui VbrCbpRate[%d] Log2MaxMvLengthHorizontal[%d] Log2MaxMvLengthVertical[%d] NumReorderFrames[%d] MaxDecFrameBuffering[%d]",
                                        pStreamConfig[i].EncConfig.Vui.VbrCbpRate,
                                        pStreamConfig[i].EncConfig.Vui.Log2MaxMvLengthHorizontal,
                                        pStreamConfig[i].EncConfig.Vui.Log2MaxMvLengthVertical,
                                        pStreamConfig[i].EncConfig.Vui.NumReorderFrames,
                                        pStreamConfig[i].EncConfig.Vui.MaxDecFrameBuffering);
            AmbaLL_LogUInt5("           EncConfig Vui MaxBytesPerPicDenom[%d] MaxBitsPerMbDenom[%d]",
                                        pStreamConfig[i].EncConfig.Vui.MaxBytesPerPicDenom,
                                        pStreamConfig[i].EncConfig.Vui.MaxBitsPerMbDenom, 0U, 0U, 0U);
            AmbaLL_LogUInt5("           EncConfig BitRateCfg BitRateCtrl[%d] QualityLevel[%d] BitRate[%d] CPBSize[%d] VBRComplexLvl[%d]",
                                        pStreamConfig[i].EncConfig.BitRateCfg.BitRateCtrl,
                                        pStreamConfig[i].EncConfig.BitRateCfg.QualityLevel,
                                        pStreamConfig[i].EncConfig.BitRateCfg.BitRate,
                                        pStreamConfig[i].EncConfig.BitRateCfg.CPBSize,
                                        pStreamConfig[i].EncConfig.BitRateCfg.VBRComplexLvl);
            AmbaLL_LogUInt5("           EncConfig BitRateCfg VBRPercent[%d] VBRMinRatio[%d] VBRMaxRatio[%d] RcPMul[%d] VBRComplexLvl[%d]",
                                        pStreamConfig[i].EncConfig.BitRateCfg.VBRPercent,
                                        pStreamConfig[i].EncConfig.BitRateCfg.VBRMinRatio,
                                        pStreamConfig[i].EncConfig.BitRateCfg.VBRMaxRatio,
                                        pStreamConfig[i].EncConfig.BitRateCfg.RcPMul, 0U);
            AmbaLL_LogUInt5("           EncConfig BitsBufAddr[0x%X%X] BitsBufSize[%d]",
                                        DSP_GetU64Msb((UINT64)pStreamConfig[i].EncConfig.BitsBufAddr),
                                        DSP_GetU64Lsb((UINT64)pStreamConfig[i].EncConfig.BitsBufAddr),
                                        pStreamConfig[i].EncConfig.BitsBufSize, 0U, 0U);
            AmbaLL_LogUInt5("           EncConfig NumSlice[%d] NumTile[%d] NumExtraRecon[%d]",
                                        pStreamConfig[i].EncConfig.NumSlice,
                                        pStreamConfig[i].EncConfig.NumTile,
                                        pStreamConfig[i].EncConfig.NumExtraRecon, 0U, 0U);
            AmbaLL_LogUInt5("           EncConfig IntraRefreshCtrl Cycle[%d] Mode[%d] Frame[%d] QpAdj[%d] MaxDecodeDelay[%d]",
                                        pStreamConfig[i].EncConfig.IntraRefreshCtrl.IntraRefreshCycle,
                                        pStreamConfig[i].EncConfig.IntraRefreshCtrl.IntraRefreshMode,
                                        pStreamConfig[i].EncConfig.IntraRefreshCtrl.IntraRefreshFrame,
                                        IntraRefreshQpAdj8,
                                        pStreamConfig[i].EncConfig.IntraRefreshCtrl.IntraRefreshMaxDecodeDelay);
            AmbaLL_LogUInt5("           EncConfig PicOrderCntType[0x%X] SPSPPSHeaderInsertMode[%d] AUDMode[%d] NonRefPIntvl[%d] Opt[0x%X]",
                                        pStreamConfig[i].EncConfig.PicOrderCntType,
                                        pStreamConfig[i].EncConfig.SPSPPSHeaderInsertMode,
                                        pStreamConfig[i].EncConfig.AUDMode,
                                        pStreamConfig[i].EncConfig.NonRefPIntvl,
                                        pStreamConfig[i].EncConfig.EncOption);
            AmbaLL_LogUInt5("           EncConfig QuantMatrixAddr[0x%X%X]",
                                        DSP_GetU64Msb((UINT64)pStreamConfig[i].EncConfig.QuantMatrixAddr),
                                        DSP_GetU64Lsb((UINT64)pStreamConfig[i].EncConfig.QuantMatrixAddr), 0U, 0U, 0U);
            AmbaLL_LogUInt5("           EncConfig JpgBitRateCtrl QualityLevel[%d] TargetBitRate[%d] Tolerance[%d] RateCurvePoints[%d] MaxEncLoop[%d]",
                                        pStreamConfig[i].EncConfig.JpgBitRateCtrl.QualityLevel,
                                        pStreamConfig[i].EncConfig.JpgBitRateCtrl.TargetBitRate,
                                        pStreamConfig[i].EncConfig.JpgBitRateCtrl.Tolerance,
                                        pStreamConfig[i].EncConfig.JpgBitRateCtrl.RateCurvePoints,
                                        pStreamConfig[i].EncConfig.JpgBitRateCtrl.MaxEncLoop);
            AmbaLL_LogUInt5("           EncConfig JpgBitRateCtrl RateCurvAddr[0x%X%X]",
                                        DSP_GetU64Msb((UINT64)pStreamConfig[i].EncConfig.JpgBitRateCtrl.RateCurvAddr),
                                        DSP_GetU64Lsb((UINT64)pStreamConfig[i].EncConfig.JpgBitRateCtrl.RateCurvAddr),
                                        0U, 0U, 0U);
        }
    } else {
        AmbaLL_LogUInt5("         pStreamIdx[NULL] pStreamConfig[NULL]", 0U, 0U, 0U, 0U, 0U);
    }
}

static void dump_VideoEncStart(const UINT16 NumStream,
                               const UINT16 *pStreamIdx,
                               const AMBA_DSP_VIDEO_ENC_START_CONFIG_s *pStartConfig,
                               const UINT64 *pAttachedRawSeq)
{
    UINT32 i;

    //FIXME MISRA
    (void)pAttachedRawSeq;

    AmbaLL_LogUInt5(" VdoEncStart NumStrm[%d]", NumStream, 0U, 0U, 0U, 0U);
    if ((pStreamIdx != NULL) && (pStartConfig != NULL)) {
        for (i = 0U; i<NumStream; i++) {
            AmbaLL_LogUInt5("         StrmId[%d] EncDur[%d] EncSkip[%d]",
                    pStreamIdx[i], pStartConfig[i].EncDuration,
                    pStartConfig[i].EncSkipFrame, 0U, 0U);
        }
    } else {
        AmbaLL_LogUInt5("         pStreamIdx[NULL] pStreamConfig[NULL]", 0U, 0U, 0U, 0U, 0U);
    }
}

static void dump_VideoEncStop(const UINT16 NumStream,
                              const UINT16 *pStreamIdx,
                              const UINT8 *pStopOption,
                              const UINT64 *pAttachedRawSeq)
{
    UINT32 i;

    //FIXME MISRA
    (void)pAttachedRawSeq;

    AmbaLL_LogUInt5(" VdoEncStop NumStrm[%d]", NumStream, 0U, 0U, 0U, 0U);
    if ((pStreamIdx != NULL) && (pStopOption != NULL)) {
        for (i = 0U; i<NumStream; i++) {
            AmbaLL_LogUInt5("         StrmId[%d] Opt[%d]",
                    pStreamIdx[i], pStopOption[i], 0U, 0U, 0U);
        }
    } else {
        AmbaLL_LogUInt5("            pStreamIdx[NULL] pStopOption[NULL] ",
                0U, 0U, 0U, 0U, 0U);
    }
}

static void dump_VideoEncControlFrameRate(const UINT16 NumStream,
                                          const UINT16 *pStreamIdx,
                                          const UINT32 *pDivisor,
                                          const UINT64 *pAttachedRawSeq)
{
    UINT32 i;

    //FIXME MISRA
    (void)pAttachedRawSeq;

    AmbaLL_LogUInt5(" VideoEncControlFrameRate NumStrm[%d]", NumStream, 0U, 0U, 0U, 0U);
    if ((pStreamIdx != NULL) && (pDivisor != NULL)) {
        for (i = 0U; i<NumStream; i++) {
            AmbaLL_LogUInt5("         StrmId[%d] Divisor[%d]",
                    pStreamIdx[i], pDivisor[i], 0U, 0U, 0U);
        }
    } else {
        AmbaLL_LogUInt5("         pStreamIdx[NULL] pDivisor[NULL] ", 0U, 0U, 0U, 0U, 0U);
    }
}

static void dump_VideoEncControlRepeatDrop(const UINT16 NumStream,
                                           const UINT16* pStreamIdx,
                                           const AMBA_DSP_VIDEO_REPEAT_DROP_CONFIG_s *pRepeatDropCfg,
                                           const UINT64* pAttachedRawSeq)
{
    UINT32 i;

    //FIXME MISRA
    (void)pAttachedRawSeq;

    AmbaLL_LogUInt5(" VideoEncControlRepeatDrop NumStrm[%d]", NumStream, 0U, 0U, 0U, 0U);
    if ((pStreamIdx != NULL) && (pRepeatDropCfg != NULL)) {
        for (i = 0U; i<NumStream; i++) {
            AmbaLL_LogUInt5("         StrmId[%d] RepeatDropCfg.Count[%d] RepeatDropCfg.DropRepeatOption[%d]",
                    pStreamIdx[i], pRepeatDropCfg[i].Count, pRepeatDropCfg[i].DropRepeatOption, 0U, 0U);
        }
    } else {
        AmbaLL_LogUInt5("         pStreamIdx[NULL] pRepeatDropCfg[NULL] ", 0U, 0U, 0U, 0U, 0U);
    }
}

static void dump_VideoEncControlQuality(const UINT16 NumStream,
                                        const UINT16 *pStreamIdx,
                                        const AMBA_DSP_VIDEO_ENC_QUALITY_s *pQCtrl)
{
    UINT32 i;

    AmbaLL_LogUInt5(" VideoEncControlQuality NumStrm[%d]", NumStream, 0U, 0U, 0U, 0U);
    if ((pStreamIdx != NULL) && (pQCtrl != NULL)) {
        for (i = 0U; i<NumStream; i++) {
            AmbaLL_LogUInt5("         StrmId[%d] QCtrl.DemandIDR[%d] QCtrl.BitRateCtrl[%d] QCtrl.GOPCtrl[%d] QCtrl.QPCtrl[%d]",
                    pStreamIdx[i], pQCtrl[i].DemandIDR, pQCtrl[i].BitRateCtrl,
                    pQCtrl[i].GOPCtrl, pQCtrl[i].QPCtrl);
            AmbaLL_LogUInt5("                    QCtrl.BitRate[%d]",
                    pQCtrl[i].BitRate, 0U, 0U, 0U, 0U);
            AmbaLL_LogUInt5("                    QCtrl.GOPCfg.GOPStruct[%d] QCtrl.GOPCfg.M[%d] QCtrl.GOPCfg.N[%d] QCtrl.GOPCfg.IdrInterval[%d]",
                    pQCtrl[i].GOPCfg.GOPStruct, pQCtrl[i].GOPCfg.M, pQCtrl[i].GOPCfg.N,
                    pQCtrl[i].GOPCfg.IdrInterval, 0U);
            AmbaLL_LogUInt5("                    QpCfg.GOPCfg.MinI[%d] QCtrl.GOPCfg.MaxI[%d] QCtrl.GOPCfg.MinP[%d] QCtrl.GOPCfg.MaxP[%d]",
                    pQCtrl[i].QpCfg.QpMinIFrame, pQCtrl[i].QpCfg.QpMaxIFrame, pQCtrl[i].QpCfg.QpMinPFrame,
                    pQCtrl[i].QpCfg.QpMaxPFrame, 0U);
            AmbaLL_LogUInt5("                    QpCfg.GOPCfg.MinB[%d] QCtrl.GOPCfg.MaxB[%d] QCtrl.GOPCfg.ReduceI[%d] QCtrl.GOPCfg.ReduceP[%d] QCtrl.GOPCfg.AQP[%d]",
                    pQCtrl[i].QpCfg.QpMinBFrame, pQCtrl[i].QpCfg.QpMaxBFrame, pQCtrl[i].QpCfg.QpReduceIFrame,
                    pQCtrl[i].QpCfg.QpReducePFrame, pQCtrl[i].QpCfg.Aqp);
            AmbaLL_LogUInt5("                    QpCfg.RoiCfg.IFrmMapAddr[0x%X%X] QpCfg.RoiCfg.PFrmMapAddr[0x%X%X]",
                    DSP_GetU64Msb((UINT64)pQCtrl[i].RoiCfg.IFrmMapAddr),
                    DSP_GetU64Lsb((UINT64)pQCtrl[i].RoiCfg.IFrmMapAddr),
                    DSP_GetU64Msb((UINT64)pQCtrl[i].RoiCfg.PFrmMapAddr),
                    DSP_GetU64Lsb((UINT64)pQCtrl[i].RoiCfg.PFrmMapAddr), 0U);
            AmbaLL_LogUInt5("                    QpCfg.RoiCfg.BFrmMapAddr[0x%X%X] QpCfg.RoiCfg.AuxFrmMapAddr[0x%X%X]",
                    DSP_GetU64Msb((UINT64)pQCtrl[i].RoiCfg.BFrmMapAddr),
                    DSP_GetU64Lsb((UINT64)pQCtrl[i].RoiCfg.BFrmMapAddr),
                    DSP_GetU64Msb((UINT64)pQCtrl[i].RoiCfg.AuxFrmMapAddr),
                    DSP_GetU64Lsb((UINT64)pQCtrl[i].RoiCfg.AuxFrmMapAddr), 0U);
            AmbaLL_LogUInt5("                    QpCfg.RoiCfg.MdCatLUT[%d %d %d %d]",
                    pQCtrl[i].RoiCfg.MdCatLUT[0U],
                    pQCtrl[i].RoiCfg.MdCatLUT[1U],
                    pQCtrl[i].RoiCfg.MdCatLUT[2U],
                    pQCtrl[i].RoiCfg.MdCatLUT[3U], 0U);
            AmbaLL_LogUInt5("                    QpCfg.RoiCfg.MdCatLUT[%d %d %d %d]",
                    pQCtrl[i].RoiCfg.MdCatLUT[4U],
                    pQCtrl[i].RoiCfg.MdCatLUT[5U],
                    pQCtrl[i].RoiCfg.MdCatLUT[6U],
                    pQCtrl[i].RoiCfg.MdCatLUT[7U], 0U);
            AmbaLL_LogUInt5("                    QpCfg.RoiCfg.IntraBias[%d %d] QpCfg.RoiCfg.DirectBias[%d %d]",
                    pQCtrl[i].RoiCfg.IntraBias[0U],
                    pQCtrl[i].RoiCfg.IntraBias[1U],
                    pQCtrl[i].RoiCfg.DirectBias[0U],
                    pQCtrl[i].RoiCfg.DirectBias[1U], 0U);
        }
    } else {
        AmbaLL_LogUInt5("         pStreamIdx[NULL] pQCtrl[NULL] ", 0U, 0U, 0U, 0U, 0U);
    }
}

static void dump_VideoEncControlBlend(const UINT16 NumStream,
                                      const UINT16 *pStreamIdx,
                                      const AMBA_DSP_VIDEO_BLEND_CONFIG_s *pBlendCfg)
{
    UINT32 i;

    AmbaLL_LogUInt5(" VdoEncCtrlBld NumStrm[%d]", NumStream, 0U, 0U, 0U, 0U);
    if ((pStreamIdx != NULL) && (pBlendCfg != NULL)) {
        for (i = 0U; i<NumStream; i++) {
            AmbaLL_LogUInt5("         StrmIdx[%d] Idx[%d] En[%d] CLUT[%d]",
                    pStreamIdx[i], pBlendCfg[i].BlendAreaIdx,
                    pBlendCfg[i].Enable, pBlendCfg[i].EnableCLUT, 0U);
            AmbaLL_LogUInt5("         BldYuv Pitch[%d] AddrY[0x%X%X] AddrUV[0x%X%X]",
                    pBlendCfg[i].BlendYuvBuf.Pitch,
                    DSP_GetU64Msb((UINT64)pBlendCfg[i].BlendYuvBuf.BaseAddrY),
                    DSP_GetU64Lsb((UINT64)pBlendCfg[i].BlendYuvBuf.BaseAddrY),
                    DSP_GetU64Msb((UINT64)pBlendCfg[i].BlendYuvBuf.BaseAddrUV),
                    DSP_GetU64Lsb((UINT64)pBlendCfg[i].BlendYuvBuf.BaseAddrUV));
            AmbaLL_LogUInt5("                Fmt[%d] Win[%d %d %d %d]",
                    pBlendCfg[i].BlendYuvBuf.DataFmt,
                    pBlendCfg[i].BlendYuvBuf.Window.Width, pBlendCfg[i].BlendYuvBuf.Window.Height,
                    pBlendCfg[i].BlendYuvBuf.Window.OffsetX, pBlendCfg[i].BlendYuvBuf.Window.OffsetY);
            AmbaLL_LogUInt5("         AlphaYuv Pitch[%d] AddrY[0x%X%X] AddrUV[0x%X%X] ",
                    pBlendCfg[i].AlphaBuf.Pitch,
                    DSP_GetU64Msb((UINT64)pBlendCfg[i].AlphaBuf.BaseAddrY),
                    DSP_GetU64Lsb((UINT64)pBlendCfg[i].AlphaBuf.BaseAddrY),
                    DSP_GetU64Msb((UINT64)pBlendCfg[i].AlphaBuf.BaseAddrUV),
                    DSP_GetU64Lsb((UINT64)pBlendCfg[i].AlphaBuf.BaseAddrUV));
            AmbaLL_LogUInt5("                  Fmt[%d] Win[%d %d %d %d]",
                    pBlendCfg[i].AlphaBuf.DataFmt,
                    pBlendCfg[i].AlphaBuf.Window.Width, pBlendCfg[i].AlphaBuf.Window.Height,
                    pBlendCfg[i].AlphaBuf.Window.OffsetX, pBlendCfg[i].AlphaBuf.Window.OffsetY);
        }
    } else {
        AmbaLL_LogUInt5("               pStreamIdx[NULL] pBlendCfg[NULL] ", 0U, 0U, 0U, 0U, 0U);
    }
}

static void dump_VideoEncExecIntervalCap(const UINT16 NumStream,
                                         const UINT16 *pStreamIdx)
{
    UINT32 i;

    AmbaLL_LogUInt5(" VdoEncIntCap NumStrm[%d]", NumStream, 0U, 0U, 0U, 0U);
    if (pStreamIdx != NULL) {
        for (i = 0U; i<NumStream; i++) {
            AmbaLL_LogUInt5("              StrmId[%d]", pStreamIdx[i], 0U, 0U, 0U, 0U);
        }
    } else {
        AmbaLL_LogUInt5("              pStreamIdx[NULL]", 0U, 0U, 0U, 0U, 0U);
    }
}

static void dump_VideoEncFeedYuvData(const UINT16 NumStream,
                                     const UINT16 *pStreamIdx,
                                     const AMBA_DSP_EXT_YUV_BUF_s *pExtYuvBuf)
{
    UINT32 i;

    AmbaLL_LogUInt5(" VdoEncFeedYuv NumStrm[%d]", NumStream, 0U, 0U, 0U, 0U);
    if ((pStreamIdx != NULL) && (pExtYuvBuf != NULL)) {
        for (i = 0U; i<NumStream; i++) {
            AmbaLL_LogUInt5("               StrmId[%d] IsLast[%d]",
                    pStreamIdx[i], pExtYuvBuf[i].IsLast, 0U, 0U, 0U);
            AmbaLL_LogUInt5("               Yuv [0x%X%X 0x%X%X] P[%d]",
                    DSP_GetU64Msb((UINT64)pExtYuvBuf[i].ExtYuvBuf.BaseAddrY),
                    DSP_GetU64Lsb((UINT64)pExtYuvBuf[i].ExtYuvBuf.BaseAddrY),
                    DSP_GetU64Msb((UINT64)pExtYuvBuf[i].ExtYuvBuf.BaseAddrUV),
                    DSP_GetU64Lsb((UINT64)pExtYuvBuf[i].ExtYuvBuf.BaseAddrUV),
                    pExtYuvBuf[i].ExtYuvBuf.Pitch);
            AmbaLL_LogUInt5("               Fmt[%d] Win[%dx%d]",
                    pExtYuvBuf[i].ExtYuvBuf.DataFmt,
                    pExtYuvBuf[i].ExtYuvBuf.Window.Width,
                    pExtYuvBuf[i].ExtYuvBuf.Window.Height, 0U, 0U);
            if (pExtYuvBuf[i].pExtME1Buf != NULL) {
                AmbaLL_LogUInt5("               Me1 [0x%X%X] P[%d] Win[%dx%d]",
                    DSP_GetU64Msb((UINT64)pExtYuvBuf[i].pExtME1Buf->BaseAddr),
                    DSP_GetU64Lsb((UINT64)pExtYuvBuf[i].pExtME1Buf->BaseAddr),
                    pExtYuvBuf[i].pExtME1Buf->Pitch,
                    pExtYuvBuf[i].pExtME1Buf->Window.Width, pExtYuvBuf[i].pExtME1Buf->Window.Height);
            }
            if (pExtYuvBuf[i].pExtME0Buf != NULL) {
                AmbaLL_LogUInt5("               Me0 [0x%X%X] P[%d] Win[%dx%d]",
                    DSP_GetU64Msb((UINT64)pExtYuvBuf[i].pExtME0Buf->BaseAddr),
                    DSP_GetU64Lsb((UINT64)pExtYuvBuf[i].pExtME0Buf->BaseAddr),
                    pExtYuvBuf[i].pExtME0Buf->Pitch,
                    pExtYuvBuf[i].pExtME0Buf->Window.Width, pExtYuvBuf[i].pExtME0Buf->Window.Height);
            }
        }
    } else {
        AmbaLL_LogUInt5("              pStreamIdx[NULL] pExtYuvBuf[NULL]", 0U, 0U, 0U, 0U, 0U);
    }
}

static void dump_VideoEncGrpConfig(const AMBA_DSP_VIDEO_ENC_GROUP_s *pGrpCfg)
{
    UINT32 i;

    AmbaLL_LogUInt5(" VdoEncGrpCfg", 0U, 0U, 0U, 0U, 0U);
    if (pGrpCfg != NULL) {
        AmbaLL_LogUInt5("                GrpIdx[%d] Purpose[0x%X] NumStrm[%d] Stride[0x%X]", pGrpCfg->GrpIdx, pGrpCfg->Purpose, pGrpCfg->NumStrm, pGrpCfg->Stride, 0U);
        for (i = 0U; i<pGrpCfg->NumStrm; i++) {
            AmbaLL_LogUInt5("                  [%d] StrmId[%d]", i, pGrpCfg->pStrmIdx[i], 0U, 0U, 0U);
        }
    } else {
        AmbaLL_LogUInt5("                pGrpCfg[NULL]", 0U, 0U, 0U, 0U, 0U);
    }
}

static void dump_VideoEncMvConfig(const UINT16 StreamIdx, const AMBA_DSP_VIDEO_ENC_MV_CFG_s *pMvCfg)
{
    AmbaLL_LogUInt5(" VdoEncMvCfg StrmId[%d]", StreamIdx, 0U, 0U, 0U, 0U);
    if (pMvCfg != NULL) {
        AmbaLL_LogUInt5("             BufAddr[0x%X%X] BufSize[%d] UnitSize[%d] Opt[%d]",
                                      DSP_GetU64Msb((UINT64)pMvCfg->BufAddr),
                                      DSP_GetU64Lsb((UINT64)pMvCfg->BufAddr),
                                      pMvCfg->BufSize, pMvCfg->UnitSize, pMvCfg->Option);
    } else {
        AmbaLL_LogUInt5("             pMvCfg[NULL]", 0U, 0U, 0U, 0U, 0U);
    }
}

static void dump_VideoEncDescFmtConfig(const UINT16 StreamIdx, const UINT16 CatIdx, const UINT32 OptVal)
{
    AmbaLL_LogUInt5(" VideoEncDescFmtConfig StrmId[%d] CategoryIdx[%d] OptionValue[%d]", StreamIdx, CatIdx, OptVal, 0U, 0U);
}

/* Vout */
static void dump_VoutReset(const UINT8 VoutIdx)
{
    AmbaLL_LogUInt5(" VoutReset Idx[%d]", VoutIdx, 0U, 0U, 0U, 0U);
}

static void dump_VoutVideoConfig(const UINT8 VoutIdx, const AMBA_DSP_VOUT_VIDEO_CFG_s *pConfig)
{
    AmbaLL_LogUInt5(" VoutVdoCfg Idx[%d]", VoutIdx, 0U, 0U, 0U, 0U);
    if (pConfig != NULL) {
        AmbaLL_LogUInt5("            Win[%d %d %d %d] Src[%d]",
                                     pConfig->Window.Width, pConfig->Window.Height,
                                     pConfig->Window.OffsetX, pConfig->Window.OffsetY,
                                     pConfig->VideoSource);
        AmbaLL_LogUInt5("            DefImg Rep[%d] Pitch[%d] AddrY[0x%X%X]",
                                     pConfig->DefaultImgConfig.FieldRepeat, pConfig->DefaultImgConfig.Pitch,
                                     DSP_GetU64Msb((UINT64)pConfig->DefaultImgConfig.BaseAddrY),
                                     DSP_GetU64Lsb((UINT64)pConfig->DefaultImgConfig.BaseAddrY), 0U);
        AmbaLL_LogUInt5("            AddrUV[0x%X%X] RotFlip[%d] DataTransOpt[0x%X]",
                                     DSP_GetU64Msb((UINT64)pConfig->DefaultImgConfig.BaseAddrUV),
                                     DSP_GetU64Lsb((UINT64)pConfig->DefaultImgConfig.BaseAddrUV),
                                     pConfig->RotateFlip, pConfig->DataTransmitOption, 0U);
    } else {
        AmbaLL_LogUInt5("              pConfig[NULL]", 0U, 0U, 0U, 0U, 0U);
    }
}

static void dump_VoutVideoCtrl(const UINT8 VoutIdx,
                               const UINT8 Enable,
                               const UINT8 SyncWithVin,
                               const UINT64 *pAttachedRawSeq)
{

    //FIXME MISRA
    (void)pAttachedRawSeq;

    AmbaLL_LogUInt5(" VoutVdoCtrl Idx[%d] Enb[%d] SyncVin[%d]", VoutIdx, Enable, SyncWithVin, 0U, 0U);
}

static void dump_VoutOsdConfigBuf(const UINT8 VoutIdx, const AMBA_DSP_VOUT_OSD_BUF_CONFIG_s *pBufConfig)
{
    AmbaLL_LogUInt5(" VoutOsdCfg Idx[%d]", VoutIdx, 0U, 0U, 0U, 0U);
    if (pBufConfig != NULL) {
        AmbaLL_LogUInt5("            Addr[0x%X%X] DataFmt[%d] FieldRpt[%d] P[%d]",
                        DSP_GetU64Msb((UINT64)pBufConfig->BaseAddr),
                        DSP_GetU64Lsb((UINT64)pBufConfig->BaseAddr),
                        pBufConfig->DataFormat,
                        pBufConfig->FieldRepeat, pBufConfig->Pitch);
        AmbaLL_LogUInt5("            ClutAddr[0x%X%X]",
                        DSP_GetU64Msb((UINT64)pBufConfig->CLUTAddr),
                        DSP_GetU64Lsb((UINT64)pBufConfig->CLUTAddr), 0U, 0U, 0U);
        AmbaLL_LogUInt5("            InWin[%dx%d] Win[%dx%d]",
                        pBufConfig->InputWidth, pBufConfig->InputHeight,
                        pBufConfig->Window.Width, pBufConfig->Window.Height, 0U);
        AmbaLL_LogUInt5("            Swap[%d] PreMul[%d] GlbBld[%d] Trans[%d] TransColor[%d]",
                        pBufConfig->SwapByteEnable, pBufConfig->PremultipliedEnable,
                        pBufConfig->GlobalBlendEnable, pBufConfig->TransparentColorEnable,
                        pBufConfig->TransparentColor);
    } else {
        AmbaLL_LogUInt5("              pBufConfig[NULL]", 0U, 0U, 0U, 0U, 0U);
    }
}

static void dump_VoutOsdCtrl(const UINT8 VoutIdx,
                             const UINT8 Enable,
                             const UINT8 SyncWithVin,
                             const UINT64 *pAttachedRawSeq)
{

    //FIXME MISRA
    (void)pAttachedRawSeq;

    AmbaLL_LogUInt5(" VoutOsdCtrl Idx[%d] Enb[%d] SyncVin[%d]", VoutIdx, Enable, SyncWithVin, 0U, 0U);
}

static void dump_VoutDisplayConfig(const UINT8 VoutIdx, const AMBA_DSP_DISPLAY_CONFIG_s *pConfig)
{
    AmbaLL_LogUInt5(" VoutDispCfg Idx[%d]", VoutIdx, 0U, 0U, 0U, 0U);
    if (pConfig != NULL) {
        AmbaLL_LogUInt5("            VinVout[%d] VoutVout[%d] VinTarget[%d]", pConfig->VinVoutSyncDelay, pConfig->VoutSyncDelay, pConfig->VinTargetIndex, 0U, 0U);
    } else {
        AmbaLL_LogUInt5("              pConfig[NULL]", 0U, 0U, 0U, 0U, 0U);
    }
}

static void dump_VoutDisplayCtrl(const UINT8 VoutIdx)
{
    AmbaLL_LogUInt5(" VoutDispCtrl Idx[%d]", VoutIdx, 0U, 0U, 0U, 0U);
}

static void dump_VoutMixerConfig(const UINT8 VoutIdx, const AMBA_DSP_VOUT_MIXER_CONFIG_s *pConfig)
{
    AmbaLL_LogUInt5(" VoutMixerCfg Idx[%d]", VoutIdx, 0U, 0U, 0U, 0U);
    if (pConfig != NULL) {
        AmbaLL_LogUInt5("            ActiveWidth[%d] ActiveHeight[%d] VideoHorReverseEnable[%d] MixerColorFormat[0x%X]",
                pConfig->ActiveWidth, pConfig->ActiveHeight, pConfig->VideoHorReverseEnable,
                pConfig->MixerColorFormat, 0U);
        AmbaLL_LogUInt5("            Frate[%d %d %d]",
                pConfig->FrameRate.Interlace, pConfig->FrameRate.TimeScale, pConfig->FrameRate.NumUnitsInTick, 0U, 0U);
    } else {
        AmbaLL_LogUInt5("              pConfig[NULL]", 0U, 0U, 0U, 0U, 0U);
    }
}

static void dump_VoutMixerConfigBackColor(const UINT8 VoutIdx, const UINT32 BackColorYUV)
{
    AmbaLL_LogUInt5(" VoutMixerConfigBackColor Idx[%d] BackColorYUV[%x]",
            VoutIdx, BackColorYUV, 0U, 0U, 0U);
}

static void dump_VoutMixerConfigHighlightColor(const UINT8 VoutIdx,
                                               const UINT8 LumaThreshold,
                                               const UINT32 HighlightColorYUV)
{
    AmbaLL_LogUInt5(" VoutMixerConfigHighlightColor VoutIdx[%d] LumaThreshold[%d] HighlightColorYUV[%x]",
            VoutIdx, LumaThreshold, HighlightColorYUV, 0U, 0U);
}

static void dump_VoutMixerConfigCsc(const UINT8 VoutIdx, const UINT8 CscCtrl)
{
    AmbaLL_LogUInt5(" VoutMixerConfigCsc Idx[%d] CscCtrl[%x]",
            VoutIdx, CscCtrl, 0U, 0U, 0U);
}

static void dump_VoutMixerCscMatrixConfig(const UINT8 VoutIdx, const dsp_vout_csc_matrix_s *pCscMatrix)
{
(void)pCscMatrix;
    AmbaLL_LogUInt5(" VoutMixerCscMatrixConfig Idx[%d]",
            VoutIdx, 0U, 0U, 0U, 0U);
}

static void dump_VoutMixerCtrl(const UINT8 VoutIdx)
{
    AmbaLL_LogUInt5(" VoutMixerCtrl Idx[%d]",
            VoutIdx, 0U, 0U, 0U, 0U);
}

static void dump_VoutDisplayConfigGamma(const UINT8 VoutIdx, const ULONG TableAddr)
{
    AmbaLL_LogUInt5(" VoutDisplayConfigGamma Idx[%d] TableAddr[0x%X]",
            VoutIdx, (UINT32)TableAddr, 0U, 0U, 0U);
}

static void dump_VoutDisplayControlGamma(const UINT8 VoutIdx, const UINT8 Enable)
{
    AmbaLL_LogUInt5(" VoutDisplayControlGamma Idx[%d] Enable[%d]",
            VoutIdx, Enable, 0U, 0U, 0U);
}

static void dump_VoutConfigMixerBinding(const UINT8 NumVout,
                                        const UINT8 *pVoutIdx,
                                        const AMBA_DSP_VOUT_DATA_PATH_CFG_s *pConfig)
{
    UINT16 i;
    AmbaLL_LogUInt5(" VoutConfigMixerBinding NumVout[%d]",
            NumVout, 0U, 0U, 0U, 0U);
    if ((pVoutIdx != NULL) && (pConfig != NULL)) {
        for (i = 0U; i < NumVout; i++) {
            AmbaLL_LogUInt5("  VoutIdx[%d] UseMixer[%d] MixerIdx[%d] DisplayOpt[%d]",
                    pVoutIdx[i], pConfig[i].UseMixer, pConfig[i].MixerIdx, pConfig[i].DisplayOption, 0U);
        }
    } else {
        AmbaLL_LogUInt5("              pVoutIdx[NULL] pConfig[NULL]", 0U, 0U, 0U, 0U, 0U);
    }
}

static void dump_VoutDveConfig(const UINT8 VoutIdx, const UINT8 DevMode)
{
    AmbaLL_LogUInt5(" VoutDveCfg Idx[%d] Mode[%d]", VoutIdx, DevMode, 0U, 0U, 0U);
}

static void dump_VoutDveCtrl(const UINT8 VoutIdx)
{
    AmbaLL_LogUInt5(" VoutDveCtrl Idx[%d]", VoutIdx, 0U, 0U, 0U, 0U);
}

/* StillDecode */
static void dump_StillDecStart(const UINT16 StreamIdx, const AMBA_DSP_STLDEC_START_CONFIG_s *pDecConfig)
{
    AmbaLL_LogUInt5(" StillDecStart Idx[%d]", StreamIdx, 0U, 0U, 0U, 0U);
    if (pDecConfig != NULL) {
        AmbaLL_LogUInt5("   BitFmt[%d] ImgWidth[%d] BitAddr[0x%X%X] BitSize[%d]",
                         pDecConfig->BitsFormat,
                         pDecConfig->ImageWidth,
                         DSP_GetU64Msb((UINT64)pDecConfig->BitsAddr),
                         DSP_GetU64Lsb((UINT64)pDecConfig->BitsAddr),
                         pDecConfig->BitsSize);
        AmbaLL_LogUInt5("   YuvBufAddr[0x%X%X] YuvBufSz[%d] CmplOpt[0x%X]",
                         DSP_GetU64Msb((UINT64)pDecConfig->YuvBufAddr),
                         DSP_GetU64Lsb((UINT64)pDecConfig->YuvBufAddr),
                         pDecConfig->YuvBufSize, pDecConfig->ComplianceOption, 0U);
    } else {
        AmbaLL_LogUInt5("   pDecConfig[NULL]", 0U, 0U, 0U, 0U, 0U);
    }
}

/* VideoDecode */
static void dump_VideoDecConfig(const UINT16 MaxNumStream, const AMBA_DSP_VIDDEC_STREAM_CONFIG_s *pStreamConfig)
{
    UINT32 i;
    AmbaLL_LogUInt5(" VideoDecConfig MaxNumStream[%d]", MaxNumStream, 0U, 0U, 0U, 0U);
    if (pStreamConfig != NULL) {
        for (i = 0U; i<MaxNumStream; i++) {
            AmbaLL_LogUInt5("                StrmId[%d] ", pStreamConfig[i].StreamID, 0U, 0U, 0U, 0U);
            AmbaLL_LogUInt5("                BitFmt[%d] BitAddr[0x%X%X] BitSize[%d]",
                                pStreamConfig[i].BitsFormat,
                                DSP_GetU64Msb((UINT64)pStreamConfig[i].BitsBufAddr),
                                DSP_GetU64Lsb((UINT64)pStreamConfig[i].BitsBufAddr),
                                pStreamConfig[i].BitsBufSize, 0U);
            AmbaLL_LogUInt5("                MaxFrmW[%d] MaxFrmH[%d]",
                                pStreamConfig[i].MaxFrameWidth, pStreamConfig[i].MaxFrameHeight, 0U, 0U, 0U);
            AmbaLL_LogUInt5("                XcodeMode[%d] XcodeW[%d] XcodeH[%d]",
                                pStreamConfig[i].XcodeMode, pStreamConfig[i].XcodeWidth, pStreamConfig[i].XcodeHeight, 0U, 0U);
            AmbaLL_LogUInt5("                MaxBuffW[%d] MaxBuffH[%d]",
                                pStreamConfig[i].MaxVideoBufferWidth, pStreamConfig[i].MaxVideoBufferHeight, 0U, 0U, 0U);
            AmbaLL_LogUInt5("                Frate[%d %d %d] CmplOpt[0x%X]",
                                pStreamConfig[i].FrameRate.Interlace, pStreamConfig[i].FrameRate.TimeScale,
                                pStreamConfig[i].FrameRate.NumUnitsInTick, pStreamConfig[i].ComplianceOption, 0U);
        }
    } else {
        AmbaLL_LogUInt5("                pStreamConfig[NULL]", 0U, 0U, 0U, 0U, 0U);
    }
}

static void dump_VideoDecStart(const UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDDEC_START_CONFIG_s *pStartConfig)
{
    UINT32 i;
    AmbaLL_LogUInt5(" VideoDecStart NumStream[%d]", NumStream, 0U, 0U, 0U, 0U);
    if (pStartConfig != NULL) {
        for (i = 0U; i<NumStream; i++) {
            AmbaLL_LogUInt5("               StrmIdx[%d] PreLoadSize[%d] SpeedIdx[%d] Dir[%d]",
                    pStreamIdx[i], pStartConfig[i].PreloadDataSize, pStartConfig[i].SpeedIndex, pStartConfig[i].Direction, 0U);
            AmbaLL_LogUInt5("               PTS[0x%X%X]",
                    DSP_GetU64Msb((UINT64)pStartConfig[i].FirstDisplayPTS),
                    DSP_GetU64Lsb((UINT64)pStartConfig[i].FirstDisplayPTS), 0U, 0U, 0U);
        }
    } else {
        AmbaLL_LogUInt5("               pStartConfig[NULL]", 0U, 0U, 0U, 0U, 0U);
    }
}

static void dump_VideoDecPostCtrl(const UINT16 StreamIdx, const UINT16 NumPostCtrl, const AMBA_DSP_VIDDEC_POST_CTRL_s *pPostCtrl)
{
    UINT32 i;
    AmbaLL_LogUInt5(" VideoDecPostCtrl StrmIdx[%d] NumPostCtrl[%d]", StreamIdx, NumPostCtrl, 0U, 0U, 0U);
    if (pPostCtrl != NULL) {
        for (i = 0U; i<NumPostCtrl; i++) {
            AmbaLL_LogUInt5("                  InputWin[%d %d %d %d]",
                    pPostCtrl[i].InputWindow.Width, pPostCtrl[i].InputWindow.Height, pPostCtrl[i].InputWindow.OffsetX, pPostCtrl[i].InputWindow.OffsetY, 0U);
            AmbaLL_LogUInt5("                  TargetWin[%d %d %d %d] TgtRotFlp[%d]",
                    pPostCtrl[i].TargetWindow.Width, pPostCtrl[i].TargetWindow.Height, pPostCtrl[i].TargetWindow.OffsetX,
                    pPostCtrl[i].TargetWindow.OffsetY, pPostCtrl[i].TargetRotateFlip);
            AmbaLL_LogUInt5("                  VoutIdx[%d] VoutRotFlp[%d]", pPostCtrl[i].VoutIdx, pPostCtrl[i].VoutRotateFlip, 0U, 0U, 0U);
            AmbaLL_LogUInt5("                  VoutWin[%d %d %d %d]",
                    pPostCtrl[i].VoutWindow.Width, pPostCtrl[i].VoutWindow.Height, pPostCtrl[i].VoutWindow.OffsetX, pPostCtrl[i].VoutWindow.OffsetY, 0U);
        }
    } else {
        AmbaLL_LogUInt5("               pStartConfig[NULL]", 0U, 0U, 0U, 0U, 0U);
    }
}

static void dump_VideoDecBitsFifoUpdate(const UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDDEC_BITS_FIFO_s *pBitsFifo)
{
    UINT32 i;
    AmbaLL_LogUInt5(" VideoDecBitsFifoUpdate NumStream[%d]", NumStream, 0U, 0U, 0U, 0U);
    if (pBitsFifo != NULL) {
        for (i = 0U; i<NumStream; i++) {
            AmbaLL_LogUInt5("               StrmIdx[%d] StartAddr[0x%X%X] EndAddr[0x%X%X]", pStreamIdx[i],
                    DSP_GetU64Msb((UINT64)pBitsFifo[i].StartAddr), DSP_GetU64Lsb((UINT64)pBitsFifo[i].StartAddr),
                    DSP_GetU64Msb((UINT64)pBitsFifo[i].EndAddr), DSP_GetU64Lsb((UINT64)pBitsFifo[i].EndAddr));
        }
    } else {
        AmbaLL_LogUInt5("               pBitsFifo[NULL]", 0U, 0U, 0U, 0U, 0U);
    }
}

static void dump_VideoDecStop(const UINT16 NumStream, const UINT16 *pStreamIdx, const UINT8 *pShowLastFrame)
{
    UINT32 i;
    AmbaLL_LogUInt5(" VideoDecStop NumStream[%d]", NumStream, 0U, 0U, 0U, 0U);
    if (pShowLastFrame != NULL) {
        for (i = 0U; i<NumStream; i++) {
            AmbaLL_LogUInt5("               StrmIdx[%d] ShowLastFrame[%u]", pStreamIdx[i], pShowLastFrame[i], 0U, 0U, 0U);
        }
    } else {
        AmbaLL_LogUInt5("               pShowLastFrame[NULL]", 0U, 0U, 0U, 0U, 0U);
    }
}

static void dump_VideoDecTrickPlay(const UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDDEC_TRICKPLAY_s *pTrickPlay)
{
    UINT32 i;
    AmbaLL_LogUInt5(" VideoDecTrickPlay NumStream[%d]", NumStream, 0U, 0U, 0U, 0U);
    if (pTrickPlay != NULL) {
        for (i = 0U; i<NumStream; i++) {
            AmbaLL_LogUInt5("               StrmIdx[%d] Op[%u] ResumeDir[%u] ResumeSpeedIdx[%u]", pStreamIdx[i], pTrickPlay[i].Operation,
                                                                                                  pTrickPlay[i].ResumeDirection, pTrickPlay[i].ResumeSpeedIndex, 0U);
        }
    } else {
        AmbaLL_LogUInt5("               pTrickPlay[NULL]", 0U, 0U, 0U, 0U, 0U);
    }
}

void DumpApiReg(const UINT8 Enable, const UINT32 Mask)
{
    static Dsp_DumpApi_t DspApiDumpFunc = {
        /* Main */
        .pMainInit = dump_MainInit,
        .pMainGetDefaultSysCfg = dump_MainGetDefaultSysCfg,
        .pMainSetWorkArea = dump_MainSetWorkArea,
        .pMainGetDspVerInfo = dump_MainGetDspVerInfo,
        .pMainMsgParseEntry = dump_MainMsgParseEntry,
        .pMainWaitVinInterrupt = dump_MainWaitVinInterrupt,
        .pMainWaitVoutInterrupt = dump_MainWaitVoutInterrupt,
        .pMainWaitFlag = dump_MainWaitFlag,
        .pMainResourceLimit = dump_MainResourceLimit,
        .pGetCmprRawBufInfo = dump_GetCmprRawBufInfo,
        .pParLoadConfig = dump_ParLoadConfig,
        .pParLoadRegionUnlock = dump_ParLoadRegionUnlock,
        .pCalcEncMvBufInfo = dump_CalcEncMvBufInfo,
        .pMainGetBufInfo = dump_MainGetBufInfo,
        .pSliceCfgCalc = dump_SliceCfgCalc,

        /* Liveview */
        .pLiveviewConfig = dump_LiveviewConfig,
        .pLiveviewCtrl = dump_LiveviewCtrl,
        .pLiveviewUpdateConfig = dump_LiveviewUpdateConfig,
        .pLiveviewIsoConfigUpdate = dump_LiveviewUpdateIsoCfg,
        .pLiveviewUpdateVinCfg = dump_LiveviewUpdateVinCfg,
        .pLiveviewUpdatePymdCfg = dump_LiveviewUpdatePymdCfg,
        .pLiveviewConfigVinCapture = dump_LiveviewConfigVinCapture,
        .pLiveviewConfigVinPost = dump_LiveviewConfigVinPost,
        .pLiveviewFeedRawData = dump_LiveviewFeedRawData,
        .pLiveviewFeedYuvData = dump_LiveviewFeedYuvData,
        .pLiveviewGetIDspCfg = dump_LiveviewGetIDspCfg,
        .pLiveviewSidebandUpdate = dump_LiveviewSidebandUpdate,
        .pLiveviewYuvStreamSync = dump_LiveviewYuvStreamSync,
        .pLiveviewUpdateGeoCfg = dump_LiveviewUpdateGeoCfg,
        .pLiveviewUpdateVZSrcCfg = dump_LiveviewUpdateVZSrcCfg,
        .pLiveviewSliceCfg = dump_LiveviewSliceCfg,
        .pLiveviewSliceCfgCalc = dump_LiveviewSliceCfgCalc,
        .pLiveviewVZPostponeCfg = dump_LiveviewVZPostponeCfg,
        .pLiveviewUpdateVinState = dump_LiveviewUpdateVinState,
        .pLiveviewSlowShutterCtrl = dump_LiveviewSlowShutterCtrl,
        .pLiveviewDropRepeatCtrl = dump_LiveviewDropRepeatCtrl,

        /* StillCapture */
        .pDataCapCfg = dump_DataCapCfg,
        .pUpdateCapBuffer = dump_UpdateCapBuffer,
        .pDataCapCtrl = dump_DataCapCtrl,
        .pStillYuv2Yuv = dump_StillYuv2Yuv,
        .pStillEncodeCtrl = dump_StillEncodeCtrl,
        .pCalcStillYuvExtBufSize = dump_CalcStillYuvExtBufSize,
        .pStillRaw2Yuv = dump_StillRaw2Yuv,
        .pStillRaw2Raw = dump_StillRaw2Raw,

        /* VideoEncode */
        .pVideoEncConfig = dump_VideoEncConfig,
        .pVideoEncStart = dump_VideoEncStart,
        .pVideoEncStop = dump_VideoEncStop,
        .pVideoEncControlFrameRate = dump_VideoEncControlFrameRate,
        .pVideoEncControlRepeatDrop = dump_VideoEncControlRepeatDrop,
        .pVideoEncControlQuality = dump_VideoEncControlQuality,
        .pVideoEncControlBlend = dump_VideoEncControlBlend,
        .pVideoEncExecIntervalCap = dump_VideoEncExecIntervalCap,
        .pVideoEncFeedYuvData = dump_VideoEncFeedYuvData,
        .pVideoEncGrpConfig = dump_VideoEncGrpConfig,
        .pVideoEncMvConfig = dump_VideoEncMvConfig,
        .pVideoEncDescFmtConfig = dump_VideoEncDescFmtConfig,

        /* Vout */
        .pVoutReset = dump_VoutReset,
        .pVoutVideoConfig = dump_VoutVideoConfig,
        .pVoutVideoCtrl = dump_VoutVideoCtrl,
        .pVoutOsdConfigBuf = dump_VoutOsdConfigBuf,
        .pVoutOsdCtrl = dump_VoutOsdCtrl,
        .pVoutDisplayConfig = dump_VoutDisplayConfig,
        .pVoutDisplayCtrl = dump_VoutDisplayCtrl,
        .pVoutMixerConfig = dump_VoutMixerConfig,
        .pVoutMixerConfigBackColor = dump_VoutMixerConfigBackColor,
        .pVoutMixerConfigHighlightColor = dump_VoutMixerConfigHighlightColor,
        .pVoutMixerConfigCsc = dump_VoutMixerConfigCsc,
        .pVoutMixerCscMatrixConfig = dump_VoutMixerCscMatrixConfig,
        .pVoutMixerCtrl = dump_VoutMixerCtrl,
        .pVoutDisplayConfigGamma = dump_VoutDisplayConfigGamma,
        .pVoutDisplayControlGamma = dump_VoutDisplayControlGamma,
        .pVoutConfigMixerBinding = dump_VoutConfigMixerBinding,
        .pVoutDveConfig = dump_VoutDveConfig,
        .pVoutDveCtrl = dump_VoutDveCtrl,

        /* StillDecode */
        .pStillDecStart = dump_StillDecStart,

        /* VideoDecode */
        .pVideoDecConfig = dump_VideoDecConfig,
        .pVideoDecStart = dump_VideoDecStart,
        .pVideoDecPostCtrl = dump_VideoDecPostCtrl,
        .pVideoDecBitsFifoUpdate = dump_VideoDecBitsFifoUpdate,
        .pVideoDecStop = dump_VideoDecStop,
        .pVideoDecTrickPlay = dump_VideoDecTrickPlay,

    };

//FIXME, Misra
    (void)Mask;

    if (Enable == (UINT8)1) {
        pDspApiDumpFunc = &DspApiDumpFunc;
    } else {
        pDspApiDumpFunc = NULL;
    }
}

Dsp_DumpApi_t* AmbaDSP_GetDumpApiFunc(void){
    return pDspApiDumpFunc;
}

