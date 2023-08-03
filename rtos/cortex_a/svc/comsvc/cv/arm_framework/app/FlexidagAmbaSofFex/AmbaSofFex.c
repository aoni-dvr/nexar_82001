
#include "cvapi_flexidag_ambasoffex.h"
#ifdef CONFIG_BUILD_CV_THREADX
#include "AmbaTypes.h"
#include "AmbaFS.h"
#include <AmbaPrint.h>
#include <AmbaMisraFix.h>
#include <AmbaWrap.h>
#include "AmbaUtility.h"
#include "AmbaMMU.h"
#include "AmbaCache.h"
#endif

static void SofFex_CheckRval(UINT32 Rval, const char *pFuncName, const char *pCallerName)
{
    if (Rval != 0U) {
        AmbaPrint_PrintStr5("Error %s() call %s() Fail", pCallerName, pFuncName, NULL, NULL, NULL);
    }
}

// Neon code
void Do16bytesLessThanCmp(UINT8 *pDataA, UINT8 *pDataB, UINT8 *pCmpOut);

UINT32 SofFex_InitBriefDescPosLUT(UINT32 ImagePitch, UINT32 DimIndex, SOF_FEX_FD_HANDLE_s *pHandler)
{
    static const INT32 BriefAy[BRIEF_PAIRS_NUM] = {10,0,1,14,13,10,0,10,0,-1,0,-5,1,0,-10,-13,3,-3,-6,-1,
                                                   4,-10,6,-3,-3,-2,-4,-6,6,-10,-3,-2,6,-10,-3,10,7,2,13,0,5,-13,0,-4,-4,3,-3,6,0,-10,
                                                   10,-14,7,-10,-3,-4,-6,-4,5,10,-10,6,1,-10,3,3,-7,3,-14,-5,14,3,-3,-3,4,-6,-1,0,-4,
                                                   1,7,-4,-5,-13,0,-10,5,5,-4,0,-3,-2,-3,0,0,10,3,0,-13,10,-7,-3,5,-4,-4,-7,4,14,-4,5,
                                                   -3,14,-5,10,-2,-4,0,-13,5,5,-10,3,5,4,0,0,3,-13,-4,0,0,7,10,-5,3,-10,14,-13,2,-4,2,
                                                   10,10,5,-2,-1,-10,-3,-3,-7,-3,10,-7,-7,-5,-10,1,4,-14,-1,-3,-10,-3,4,3,4,-7,0,-7,
                                                   7,-1,-7,3,-13,5,6,-7,10,5,-13,2,-3,-7,-4,0,14,3,-5,-7,3,6,-14,0,-10,-5,-3,-6,0,-7,
                                                   -2,-7,-10,3,10,-3,-3,2,-3,-6,-4,-5,6,-6,-7,-2,10,10,-5,-3,-5,4,7,0,10,-10,-10,6,10,
                                                   -6,0,-4,0,0,-3,-10,-3,-4,4,3,-1,3,-7,5,0,-4,4,0,6,6,-14,5,0,6,-7,-6,-13
                                                  };
    static const INT32 BriefAx[BRIEF_PAIRS_NUM] = {0,14,-3,0,5,-10,-10,0,-10,3,-7,-13,3,7,-10,-5,-1,6,3,3,
                                                   -10,-4,-3,1,-6,2,10,3,-3,-4,-1,2,-3,10,-6,-10,7,2,-5,-14,13,-5,-4,10,-10,-6,-6,3,
                                                   -10,10,4,0,0,-4,-1,10,-3,10,-5,10,4,-3,-3,4,-1,-6,0,6,0,-13,0,-6,1,1,0,-3,-3,-14,
                                                   -10,3,0,-10,-13,5,10,0,-5,-5,0,-10,6,2,-1,-7,14,4,1,-7,-5,-10,0,-1,-13,10,10,7,10,
                                                   0,0,5,6,0,13,0,-2,10,-4,-5,13,-5,4,-6,13,0,-10,7,6,5,10,-10,-14,7,-4,-5,-6,10,0,-5,
                                                   2,0,2,0,-4,-5,-2,3,10,6,1,0,1,10,0,-7,5,0,3,-10,0,3,6,-4,1,0,1,10,7,7,-7,7,-3,0,-6,5,
                                                   -5,-3,-7,-4,5,-5,-2,6,7,0,-10,0,1,13,-7,1,3,0,14,10,-13,6,-3,4,0,2,7,-4,-1,4,-1,1,
                                                   -2,-6,-3,10,-5,3,3,7,2,10,4,13,6,-13,10,-7,10,0,-4,0,3,-4,-3,14,0,-10,-4,6,10,-6,
                                                   10,-10,6,-3,-1,0,-13,-14,0,10,-10,-3,-3,0,-5,4,-3,7,3,5
                                                  };
    static const INT32 BriefBy[BRIEF_PAIRS_NUM] = {-3,-7,7,-5,-4,0,4,-5,0,-10,4,7,13,-2,13,4,-10,3,13,3,10,
                                                   -5,10,-1,-5,-5,3,0,10,3,-7,7,-10,5,-13,6,-6,-2,-10,-4,6,-4,4,6,-10,0,-13,3,-3,-3,10,
                                                   4,-3,0,10,-1,-10,0,-14,-3,-3,-2,4,6,4,-3,-13,-6,0,-13,0,5,7,-13,7,10,-4,10,-2,5,0,10,
                                                   3,1,0,0,-6,-10,10,0,10,-10,1,0,-4,-6,-10,4,7,3,3,2,-2,-6,-4,13,3,3,0,-3,-1,-5,3,0,-3,4,
                                                   0,-5,3,0,-7,7,10,3,1,3,0,-13,5,7,-13,13,3,0,-7,-4,3,-10,-7,-14,14,5,-10,0,-7,-5,-5,-3,
                                                   0,-1,13,-7,-2,-3,-7,4,2,-3,3,-6,-2,-1,-10,0,1,10,-10,-1,-3,-4,-5,5,-6,0,0,7,1,13,-3,
                                                   -3,0,13,-3,1,0,-4,-3,-5,5,-4,0,-3,-1,-4,-7,-7,13,4,-3,5,4,-14,4,4,0,-2,-6,7,-14,10,10,
                                                   -5,-3,14,5,-5,-5,-5,-6,10,7,3,-13,-10,-10,-3,-7,-7,-7,6,-5,13,-10,-3,-5,0,0,3,5,0,0,0,
                                                   3,-5,-4,-10,5,14,-3,-1,10,7,-5,2,4,4
                                                  };
    static const INT32 BriefBx[BRIEF_PAIRS_NUM] = {6,7,7,-5,-10,-10,10,13,10,4,10,0,-5,-2,5,0,4,1,-5,-1,-4,5,
                                                   10,-3,5,5,-1,7,-10,-1,-7,7,4,-5,5,3,3,-2,4,10,3,-10,-10,-3,-10,4,-5,-1,-1,1,-10,-10,
                                                   -1,4,10,3,10,14,0,6,-1,2,-10,3,10,1,-5,3,-7,5,4,5,-7,5,-7,10,-10,0,2,5,-4,4,-6,-3,14,
                                                   14,3,10,-10,4,-10,4,3,-14,0,-3,4,-10,0,-6,-6,2,2,-3,0,-5,1,-6,4,6,-3,5,-1,7,-6,10,-10,
                                                   -5,-1,4,0,0,4,1,-3,-1,-7,-5,-5,7,-5,-5,6,10,7,10,-1,0,0,0,0,5,4,-14,7,-5,5,-1,-7,3,-5,
                                                   0,2,-1,-7,0,-2,6,-6,3,-2,-3,-4,-7,3,10,-4,-3,6,0,13,-13,3,-4,14,0,-3,-5,-1,1,4,5,-1,3,
                                                   -4,0,1,-13,13,0,10,-1,-3,-10,7,0,-5,-10,-1,-13,0,0,-10,-10,7,2,-3,0,0,-4,4,-13,6,0,13,
                                                   13,13,-5,-3,0,7,6,-5,10,4,-1,7,7,-7,3,-13,-5,-10,-6,-13,14,-4,-1,13,4,10,-14,6,-5,10,
                                                   4,5,0,-1,-3,10,-7,-13,-2,0,0
                                                  };
    UINT32 RetVal = 0U;
    UINT32 Idex;

    for (Idex = 0U; Idex < BRIEF_PAIRS_NUM; Idex++) {
        pHandler->HarrisBuff[DimIndex].BriefDesc_APosLUT[Idex] = (BriefAy[Idex]*(INT32)ImagePitch) + BriefAx[Idex];
        pHandler->HarrisBuff[DimIndex].BriefDesc_BPosLUT[Idex] = (BriefBy[Idex]*(INT32)ImagePitch) + BriefBx[Idex];
    }

    return RetVal;
}

#define NEON_LOAD_BYTES (16U)
#define NEON_LOAD_TIMES (BRIEF_PAIRS_NUM/NEON_LOAD_BYTES)
static UINT32 SofFex_ProcBriefDesc(const UINT8 *pImgData, UINT32 ImagePitch,
                                   UINT16 XPos, UINT16 YPos,
                                   const INT32 PosALUT[BRIEF_PAIRS_NUM], const INT32 PosBLUT[BRIEF_PAIRS_NUM],
                                   const UINT8 pDesc[CV_FEX_DESCRIPTOR_SIZE])
{
    UINT32 RetVal = 0U;
    UINT32 SampleIdx;
    UINT8 pAList[NEON_LOAD_BYTES];
    UINT8 pBList[NEON_LOAD_BYTES];
    INT32 ImgIdx;
    UINT32 NeonOpIdx;

    SampleIdx = 0U;
    ImgIdx = ((INT32)YPos*(INT32)ImagePitch) + (INT32)XPos;
    // Each time compare 16 pairs of pixel
    for (NeonOpIdx = 0; NeonOpIdx < NEON_LOAD_TIMES; NeonOpIdx++) {
        UINT8 AB_Compare[NEON_LOAD_BYTES];
        UINT16 *pCurDescpPtr;
        UINT16 BitIdx;
        UINT32 Idex;

        for (Idex = 0; Idex < NEON_LOAD_BYTES; Idex++) {
            INT32 ShiftImgIdx;
            ShiftImgIdx = ImgIdx + PosALUT[SampleIdx + Idex];
            pAList[Idex] = pImgData[(UINT32)ShiftImgIdx];
            ShiftImgIdx = ImgIdx + PosBLUT[SampleIdx + Idex];
            pBList[Idex] = pImgData[(UINT32)ShiftImgIdx];
        }
        SampleIdx += NEON_LOAD_BYTES;
        Do16bytesLessThanCmp(pAList, pBList, AB_Compare);

        // 16 pairs of results take up 16 bits(2 bytes)
        {
            const void *p;
            p = &(pDesc[NeonOpIdx*2U]);
            SofFex_CheckRval(AmbaWrap_memcpy(&pCurDescpPtr, &p, sizeof(UINT16*)), "AmbaWrap_memcpy", __func__);
        }

        *pCurDescpPtr = 0U;
        for (BitIdx = 0U; BitIdx < NEON_LOAD_BYTES; BitIdx++) {
            if (AB_Compare[BitIdx] > 0U) {
                *pCurDescpPtr = (*pCurDescpPtr) | (UINT16)(0x0001UL<<BitIdx);
            }
        }
    }

    return RetVal;
}

UINT32 SofFex_GetProcVPInstIdx(const SOF_FEX_FD_HANDLE_s *pHandler,
                               const memio_source_recv_picinfo_t *pInPicInfo,
                               UINT32 InPicInfoOctaveIndex,
                               UINT32 RoiEnable, const roi_t *pRoi,
                               UINT32 *pOutProcVPInstIdx)
{
    UINT32 Ret = 1U;
    UINT16 Width;
    UINT16 Height;
    UINT32 Idex;

    if ( (RoiEnable == 1U) || (pInPicInfo == NULL) ) {
        Width = pRoi->m_width_m1 + 1U;
        Height = pRoi->m_height_m1 + 1U;
    } else {
        Width = pInPicInfo->pic_info.pyramid.half_octave[InPicInfoOctaveIndex].roi_width_m1 + 1U;
        Height = pInPicInfo->pic_info.pyramid.half_octave[InPicInfoOctaveIndex].roi_height_m1 + 1U;
    }

    for (Idex = 0; Idex < pHandler->RegisterHdlrNum; Idex++) {
        if ( (Width == pHandler->ProcWidth[Idex]) && (Height == pHandler->ProcHeight[Idex]) ) {
            *pOutProcVPInstIdx = Idex;
            Ret = 0U;
        }
    }

    return Ret;
}

static UINT32 PicInfoToRawInfo(const SOF_FEX_FD_HANDLE_s *pHandler, UINT32 ChID,
                               const memio_source_recv_picinfo_t *pInPicInfo, UINT32 InPicInfoOctaveIndex,
                               UINT32 ProcVPInstIdx,
                               UINT32 ConfigRoiEnable, const roi_t *pConfigRoi,
                               memio_source_recv_raw_t *pOutRawInfo)
{
    UINT32 Rval = 0U;
    UINT32 OctaveCtrl;
    UINT32 ImagePitch;
    UINT32 BasePicInfoAddr;
    UINT32 RelativeYAddr = 0U;
    INT16  AbsoluteStartX, AbsoluteStartY;

    SofFex_CheckRval(AmbaWrap_memcpy(&OctaveCtrl,
                                     &pInPicInfo->pic_info.pyramid.half_octave[InPicInfoOctaveIndex].ctrl, sizeof(UINT32)), "AmbaWrap_memcpy", __func__);

    ImagePitch = (OctaveCtrl & 0xFFFF0000U);
    ImagePitch >>= 16U;

    SofFex_CheckRval(AmbaWrap_memset(pOutRawInfo, 0, sizeof(memio_source_recv_raw_t)), "AmbaWrap_memset", __func__);
    pOutRawInfo->size = ImagePitch*pHandler->ProcHeight[ProcVPInstIdx];
    pOutRawInfo->pitch = ImagePitch;

    AmbaMisra_TypeCast32(&BasePicInfoAddr, &pInPicInfo);
    if ( ChID == VP_SOF_FEX_LEFT ) {
        RelativeYAddr = pInPicInfo->pic_info.rpLumaLeft[InPicInfoOctaveIndex];
    } else if ( ChID == VP_SOF_FEX_RIGHT ) {
        RelativeYAddr = pInPicInfo->pic_info.rpLumaRight[InPicInfoOctaveIndex];
    } else {
        Rval = 1U;
    }

    AbsoluteStartX = pInPicInfo->pic_info.pyramid.half_octave[InPicInfoOctaveIndex].roi_start_col;
    AbsoluteStartY = pInPicInfo->pic_info.pyramid.half_octave[InPicInfoOctaveIndex].roi_start_row;

    if ( ConfigRoiEnable == 1U ) {
        AbsoluteStartX += (INT16)pConfigRoi->m_start_col;
        AbsoluteStartY += (INT16)pConfigRoi->m_start_row;
    }

    // Offset data by given ROI
    {
        UINT8 *pU8Ptr;
        AmbaMisra_TypeCast32(&pU8Ptr, &RelativeYAddr);
        pU8Ptr = &pU8Ptr[(ImagePitch*(UINT32)AbsoluteStartY) + (UINT32)AbsoluteStartX];
        AmbaMisra_TypeCast32(&RelativeYAddr, &pU8Ptr);
    }

    pOutRawInfo->addr = BasePicInfoAddr + RelativeYAddr;

    // Write cache data to main memory
    {
        ULONG Addr;

        AmbaMisra_TypeCast32(&Addr, &pOutRawInfo);
        (void) AmbaCache_DataClean(Addr, sizeof(memio_source_recv_raw_t));
        (void) AmbaCache_DataClean(pOutRawInfo->addr, pOutRawInfo->size);
    }

    return Rval;
}

static UINT32 HarrisCornerDet_Process_VP(
    REF_FD_HANDLE_s* pHandler,
    const memio_source_recv_raw_t* pIn,
    const UINT8* pOutHarrisBitDet,
    const UINT8* pOutHarrisResponse)
{
    UINT32 Rval;
    ULONG Addr, Paddr;

    {
        void* pInPtr[8U];
        void* pOutPtr[8U];
        UINT32 InSz[8U] = {0U};
        UINT32 InNum = 1U;
        UINT32 OutNum = 2U;

        AmbaMisra_TypeCast32(&pInPtr[0U], &pIn);
        AmbaMisra_TypeCast32(&pOutPtr[0U], &pOutHarrisBitDet);
        AmbaMisra_TypeCast32(&pOutPtr[1U], &pOutHarrisResponse);
        InSz[0U] = SOF_FEX_ALIGN128(sizeof(memio_source_recv_raw_t));

        Rval = RefCVUtil_FlexiDagProc(pHandler, pInPtr, InSz, InNum,
                                      pOutPtr, OutNum);
    }

    AmbaMisra_TypeCast32(&Addr, &pOutHarrisBitDet);
    (void) AmbaMMU_VirtToPhys(Addr, &Paddr);
    (void) AmbaCache_DataInvalidate(Paddr, pHandler->FDGenHandle.mem_req.flexidag_output_buffer_size[0U]);

    AmbaMisra_TypeCast32(&Addr, &pOutHarrisResponse);
    (void) AmbaMMU_VirtToPhys(Addr, &Paddr);
    (void) AmbaCache_DataInvalidate(Paddr, pHandler->FDGenHandle.mem_req.flexidag_output_buffer_size[1U]);

    return Rval;
}

static UINT32 HarrisCornerDet_GetResult(
    REF_FD_HANDLE_s* pHandler, UINT8** pOutHarrisBitDet, UINT8** pOutHarrisResponse)
{
    UINT32 Rval;
    AMBA_CV_FLEXIDAG_IO_s InBuf, OutBuf;

    Rval = RefCVUtil_FlexiDagGetResult(pHandler, &InBuf, &OutBuf);

    AmbaMisra_TypeCast32(pOutHarrisBitDet, &(OutBuf.buf[0U].pBuffer));
    AmbaMisra_TypeCast32(pOutHarrisResponse, &(OutBuf.buf[1U].pBuffer));

    return Rval;
}

static UINT32 HarrisBitMapToKeyPointFmt(const UINT8 *pInHarrisBitMap, const UINT16 *pHarrResp,
                                        const memio_source_recv_raw_t* pImg,
                                        UINT32 ImgWidth, UINT32 ImgHeight, UINT32 BitMapPitch,
                                        UINT32 BriefRadius,
                                        const INT32* const pBriefPosLUT[2U],
                                        UINT8 *pOutKeyPointCount,
                                        SOF_FEX_KEYPOINT_SCORE_s pOutKeyPoint[][CV_FEX_MAX_KEYPOINTS],
                                        const UINT8 pOutKeyPointDesc[][CV_FEX_MAX_KEYPOINTS][CV_FEX_DESCRIPTOR_SIZE])
{
    UINT32 Ret = 0U;
    UINT32 BitMapWidth;
    UINT32 Idex, J;
    UINT32 WidthBnd = ImgWidth-BriefRadius;
    UINT32 HeightBnd = ImgHeight-BriefRadius;
    UINT32 BucketIdx;
    UINT32 BucketWidth = ImgWidth >> 3U;
    UINT32 BucketHeight = ImgHeight >> 3U;

    BitMapWidth = ImgWidth/8U;

    // Set key point number to 0 for all the buckets
    for (BucketIdx = 0U; BucketIdx < CV_FEX_MAX_BUCKETS; BucketIdx++) {
        pOutKeyPointCount[BucketIdx] = 0U;
    }

    for (Idex = 0U; Idex < ImgHeight; Idex++) {
        const UINT8* pHarrisBitMapPtr = &(pInHarrisBitMap[(Idex*BitMapPitch)]);
        for (J = 0U; J < BitMapWidth; J++) {
            if ( *pHarrisBitMapPtr != 0U ) {
                UINT32 BitPos;
                UINT32 IfCorner;
                for (BitPos = 0U; BitPos < 8U; BitPos++) {
                    IfCorner = (UINT32)(*pHarrisBitMapPtr) >> BitPos;
                    IfCorner &= 0x1U;
                    if ( IfCorner == 1U ) {
                        UINT32 X = (J*8U) + BitPos;
                        UINT32 Y = Idex;

                        // To calculate brief descriptor, corners are restricted to be located in image with margin = BriefRadius
                        if ( ((X >= BriefRadius) && (X < WidthBnd)) &&
                             ((Y >= BriefRadius) && (Y < HeightBnd)) ) {
                            UINT32 BucketPosI;
                            UINT32 BucketPosJ;

                            BucketPosI = Y / BucketHeight;
                            BucketPosJ = X / BucketWidth;

                            BucketIdx = (BucketPosI*8U) + BucketPosJ;

                            if ( pOutKeyPointCount[BucketIdx] != CV_FEX_MAX_KEYPOINTS ) {
                                UINT32 KeyPointIdx = pOutKeyPointCount[BucketIdx];
                                UINT32 KeyPointScore;
                                const UINT8 *pImgData;

                                // transfer to 14.2 format
                                pOutKeyPoint[BucketIdx][KeyPointIdx].X = (UINT16)(0xFFFFU & (X << 2U));
                                pOutKeyPoint[BucketIdx][KeyPointIdx].Y = (UINT16)(0xFFFFU & (Y << 2U));

                                KeyPointScore = pHarrResp[(Y*ImgWidth)+X];
                                KeyPointScore = KeyPointScore << 2U;
                                pOutKeyPoint[BucketIdx][KeyPointIdx].Score = KeyPointScore;

                                pOutKeyPointCount[BucketIdx] += 1U;

                                AmbaMisra_TypeCast32(&pImgData, &pImg->addr);
                                (void) SofFex_ProcBriefDesc(pImgData, pImg->pitch, (UINT16)X, (UINT16)Y,
                                                            pBriefPosLUT[0U], pBriefPosLUT[1U],
                                                            pOutKeyPointDesc[BucketIdx][KeyPointIdx]);
                            }
                        }
                    }
                }
            }
            pHarrisBitMapPtr++;
        }
    }

    return Ret;
}

static UINT32 HarrisCornerDet_Process_Ch(SOF_FEX_FD_HANDLE_s* pHandler,
        UINT32 ChID, UINT32 ProcVPInstIdx, const memio_source_recv_raw_t* pIn)
{
    UINT32 Rval = 0U;
    UINT8 *pOutHarrisBitDet;
    UINT8 *pOutHarrisResponse;
    const UINT16 *pOutHarrisResponseU16;

    if ( ProcVPInstIdx >= pHandler->RegisterHdlrNum ) {
        Rval = 1U;
    } else {
        UINT32 Width, Height, DetBitPitch;
        const INT32 *pBriefPosLUT[2U] = {
            pHandler->HarrisBuff[ProcVPInstIdx].BriefDesc_APosLUT,
            pHandler->HarrisBuff[ProcVPInstIdx].BriefDesc_BPosLUT
        };

        Width = pHandler->ProcWidth[ProcVPInstIdx];
        Height = pHandler->ProcHeight[ProcVPInstIdx];
        DetBitPitch = SOF_FEX_ALIGN32(Width/8U);

        Rval |= HarrisCornerDet_Process_VP(pHandler->pHarrisHandler[ProcVPInstIdx], pIn,
                                           pHandler->HarrisBuff[ProcVPInstIdx].pAlignedBitOutMapPtr,
                                           pHandler->HarrisBuff[ProcVPInstIdx].pAlignedRespMapPtr);

        Rval |= HarrisCornerDet_GetResult(
                    pHandler->pHarrisHandler[ProcVPInstIdx], &pOutHarrisBitDet, &pOutHarrisResponse);

        AmbaMisra_TypeCast32(&pOutHarrisResponseU16, &pOutHarrisResponse);

        Rval |= HarrisBitMapToKeyPointFmt(pOutHarrisBitDet, pOutHarrisResponseU16,
                                          pIn,
                                          Width, Height, DetBitPitch,
                                          BRIEF_RADIUS,
                                          pBriefPosLUT,
                                          pHandler->HarrisBuff[ProcVPInstIdx].OutKeypointsCount[ChID],
                                          pHandler->HarrisBuff[ProcVPInstIdx].OutKeyPoints[ChID],
                                          (const UINT8 (*)[CV_FEX_MAX_KEYPOINTS][CV_FEX_DESCRIPTOR_SIZE])pHandler->HarrisBuff[ProcVPInstIdx].OutDescriptors[ChID]);
    }

    return Rval;
}

static UINT32 LKOpticalFlow_Process_VP(
    REF_FD_HANDLE_s* pHandler,
    const memio_source_recv_raw_t* pInLast,
    const memio_source_recv_raw_t* pInCur,
    const UINT8* pOutDxDy,
    const UINT8* pOutInvalidCount)
{
    UINT32 Rval;
    ULONG Addr, Paddr;

    {
        void* pInPtr[8U];
        void* pOutPtr[8U];
        UINT32 InSz[8U] = {0U};
        UINT32 InNum = 2U;
        UINT32 OutNum = 2U;

        AmbaMisra_TypeCast32(&pInPtr[0U], &pInLast);
        AmbaMisra_TypeCast32(&pInPtr[1U], &pInCur);
        AmbaMisra_TypeCast32(&pOutPtr[0U], &pOutDxDy);
        AmbaMisra_TypeCast32(&pOutPtr[1U], &pOutInvalidCount);
        InSz[0U] = SOF_FEX_ALIGN128(sizeof(memio_source_recv_raw_t));
        InSz[1U] = SOF_FEX_ALIGN128(sizeof(memio_source_recv_raw_t));

        AmbaMisra_TypeCast32(&Addr, &pOutDxDy);
        (void) AmbaMMU_VirtToPhys(Addr, &Paddr);
        (void) AmbaCache_DataInvalidate(Paddr, pHandler->FDGenHandle.mem_req.flexidag_output_buffer_size[0U]);

        AmbaMisra_TypeCast32(&Addr, &pOutInvalidCount);
        (void) AmbaMMU_VirtToPhys(Addr, &Paddr);
        (void) AmbaCache_DataInvalidate(Paddr, pHandler->FDGenHandle.mem_req.flexidag_output_buffer_size[1U]);

        Rval = RefCVUtil_FlexiDagProc(pHandler, pInPtr, InSz, InNum,
                                      pOutPtr, OutNum);
    }

    return Rval;
}

static UINT32 LKOpticalFlow_GetResult(
    REF_FD_HANDLE_s* pHandler, UINT8** pOutDxDy, UINT8** pOutInvalidCount)
{
    UINT32 Rval;
    AMBA_CV_FLEXIDAG_IO_s InBuf, OutBuf;

    Rval = RefCVUtil_FlexiDagGetResult(pHandler, &InBuf, &OutBuf);

    AmbaMisra_TypeCast32(pOutDxDy, &(OutBuf.buf[0U].pBuffer));
    AmbaMisra_TypeCast32(pOutInvalidCount, &(OutBuf.buf[1U].pBuffer));

    return Rval;
}

static UINT32 LKOpticalFlow_GetInvalidCount(const UINT8 *pInvalidCountBuffer,
        UINT32 *pInvalidCount)
{
    UINT32 BlockIdx;
    const UINT8 *pInvalidCountPtr;
    const UINT16 *pInvalidCountPtrU16;

    pInvalidCountPtr = pInvalidCountBuffer;
    AmbaMisra_TypeCast32(&pInvalidCountPtrU16, &pInvalidCountPtr);

    *pInvalidCount = 0U;
    for (BlockIdx = 0U; BlockIdx < LK_INVALID_ARRAY_SIZE; BlockIdx++) {
        *pInvalidCount = *pInvalidCount + pInvalidCountPtrU16[BlockIdx];
    }
    return 0U;
}

UINT32 SofFex_HarrisCornerDetProc(SOF_FEX_FD_HANDLE_s *pHandler, const AMBA_CV_IMG_BUF_s *pImgInfo)
{
    UINT32 Rval = 0U;
    const AMBA_CV_SPUFEX_CFG_s *pConfig;
    AMBA_CV_FEX_DATA_s *pFexData;
    UINT32 FexDataPtr;
    UINT32 Ptr;
    UINT32 OctaveIdx;

    pConfig = &pHandler->Config;
    pFexData = &pHandler->OutFexData;
    AmbaMisra_TypeCast32(&FexDataPtr, &pFexData);

#if defined(CONFIG_SOC_CV2)
    pFexData->Reserved = 0U;
#else
    // Other chipset follow CV2FS format
    pFexData->NumHalfOctaves = 0U;
#endif

    for (OctaveIdx = 0U; OctaveIdx < MAX_HALF_OCTAVES; OctaveIdx++) {
        UINT32 OctaveCtrl;
        UINT32 OctaveDisable;
        UINT32 ProcVPInstIdx;
        UINT32 Proc = 0U;

        const UINT8 *pOutKeypointsCountPtr;
        const SOF_FEX_KEYPOINT_SCORE_s *pOutKeypointsPtr;
        const UINT8 *pOutDescriptorsPtr;

        SofFex_CheckRval(AmbaWrap_memcpy(&OctaveCtrl,
                                         &pImgInfo->pBuffer->pic_info.pyramid.half_octave[OctaveIdx].ctrl, sizeof(UINT32)), "AmbaWrap_memcpy", __func__);
        OctaveDisable = OctaveCtrl & 0x1U;

        SofFex_CheckRval(AmbaWrap_memset(&pFexData->PrimaryList[OctaveIdx], 0, sizeof(AMBA_CV_FEX_FEATURE_LIST_s)), "AmbaWrap_memset", __func__);
        SofFex_CheckRval(AmbaWrap_memset(&pFexData->SecondaryList[OctaveIdx], 0, sizeof(AMBA_CV_FEX_FEATURE_LIST_s)), "AmbaWrap_memset", __func__);
        pFexData->PrimaryList[OctaveIdx].HalfOctaveId = OctaveIdx;
        pFexData->SecondaryList[OctaveIdx].HalfOctaveId = OctaveIdx;

        if ( OctaveDisable == 0U ) {
            if ( ((pConfig->FexScaleCfg[OctaveIdx].Enable == CV_FEX_ENABLE_ALL) ||
                  (pConfig->FexScaleCfg[OctaveIdx].Enable == CV_FEX_ENABLE_RIGHT)) ||
                 (pConfig->FexScaleCfg[OctaveIdx].Enable == CV_FEX_ENABLE_LEFT) ) {
                UINT32 GetProcVPInstIdxRet;
                const roi_t Roi = pConfig->FexScaleCfg[OctaveIdx].Roi;

                GetProcVPInstIdxRet = SofFex_GetProcVPInstIdx(pHandler, pImgInfo->pBuffer,
                                      OctaveIdx,
                                      pConfig->FexScaleCfg[OctaveIdx].RoiEnable,
                                      &Roi,
                                      &ProcVPInstIdx);
                if ( GetProcVPInstIdxRet == 0U ) {
                    memio_source_recv_raw_t *pRawIn;

                    AmbaMisra_TypeCast32(&pRawIn, &pHandler->HarrisBuff[ProcVPInstIdx].pAlignedRecvRawBuffPtr);

                    if ( (pConfig->FexScaleCfg[OctaveIdx].Enable == CV_FEX_ENABLE_ALL) ||
                         (pConfig->FexScaleCfg[OctaveIdx].Enable == CV_FEX_ENABLE_RIGHT) ) {
                        // Do harris corner on right(primary) image
                        (void) PicInfoToRawInfo(pHandler, VP_SOF_FEX_RIGHT, pImgInfo->pBuffer,
                                                OctaveIdx, ProcVPInstIdx, pConfig->FexScaleCfg[OctaveIdx].RoiEnable, &Roi,
                                                pRawIn);
                        Rval |= HarrisCornerDet_Process_Ch(pHandler, VP_SOF_FEX_RIGHT, ProcVPInstIdx, pRawIn);

                        pOutKeypointsCountPtr = pHandler->HarrisBuff[ProcVPInstIdx].OutKeypointsCount[VP_SOF_FEX_RIGHT];
                        pOutKeypointsPtr = pHandler->HarrisBuff[ProcVPInstIdx].OutKeyPoints[VP_SOF_FEX_RIGHT][0U];
                        pOutDescriptorsPtr = pHandler->HarrisBuff[ProcVPInstIdx].OutDescriptors[VP_SOF_FEX_RIGHT][0U][0U];

                        pFexData->PrimaryList[OctaveIdx].Enable = 1U;
                        AmbaMisra_TypeCast32(&Ptr, &pOutKeypointsCountPtr);
                        pFexData->PrimaryList[OctaveIdx].KeypointsCountOffset = Ptr - FexDataPtr;
                        AmbaMisra_TypeCast32(&Ptr, &pOutKeypointsPtr);
                        pFexData->PrimaryList[OctaveIdx].KeypointsOffset = Ptr - FexDataPtr;
                        AmbaMisra_TypeCast32(&Ptr, &pOutDescriptorsPtr);
                        pFexData->PrimaryList[OctaveIdx].DescriptorsOffset = Ptr - FexDataPtr;

                        Proc = 1U;
                    }
                    if ( (pConfig->FexScaleCfg[OctaveIdx].Enable == CV_FEX_ENABLE_ALL) ||
                         (pConfig->FexScaleCfg[OctaveIdx].Enable == CV_FEX_ENABLE_LEFT) ) {
                        INT16 StartCol;
                        INT16 StartRow;
                        roi_t LeftRoi = pConfig->FexScaleCfg[OctaveIdx].Roi;

                        StartCol = (INT16)Roi.m_start_col + pConfig->FexScaleCfg[OctaveIdx].SecondaryRoiColOffset;
                        StartRow = (INT16)Roi.m_start_row + pConfig->FexScaleCfg[OctaveIdx].SecondaryRoiRowOffset;
                        LeftRoi.m_start_col = (UINT16)StartCol;
                        LeftRoi.m_start_row = (UINT16)StartRow;

                        // Do harris corner on left(secondary) image
                        (void) PicInfoToRawInfo(pHandler, VP_SOF_FEX_LEFT, pImgInfo->pBuffer,
                                                OctaveIdx, ProcVPInstIdx, pConfig->FexScaleCfg[OctaveIdx].RoiEnable, &LeftRoi,
                                                pRawIn);

                        Rval |= HarrisCornerDet_Process_Ch(pHandler, VP_SOF_FEX_LEFT, ProcVPInstIdx, pRawIn);

                        pOutKeypointsCountPtr = pHandler->HarrisBuff[ProcVPInstIdx].OutKeypointsCount[VP_SOF_FEX_LEFT];
                        pOutKeypointsPtr = pHandler->HarrisBuff[ProcVPInstIdx].OutKeyPoints[VP_SOF_FEX_LEFT][0U];
                        pOutDescriptorsPtr = pHandler->HarrisBuff[ProcVPInstIdx].OutDescriptors[VP_SOF_FEX_LEFT][0U][0U];

                        pFexData->SecondaryList[OctaveIdx].Enable = 1U;
                        AmbaMisra_TypeCast32(&Ptr, &pOutKeypointsCountPtr);
                        pFexData->SecondaryList[OctaveIdx].KeypointsCountOffset = Ptr - FexDataPtr;
                        AmbaMisra_TypeCast32(&Ptr, &pOutKeypointsPtr);
                        pFexData->SecondaryList[OctaveIdx].KeypointsOffset = Ptr - FexDataPtr;
                        AmbaMisra_TypeCast32(&Ptr, &pOutDescriptorsPtr);
                        pFexData->SecondaryList[OctaveIdx].DescriptorsOffset = Ptr - FexDataPtr;

                        Proc = 1U;
                    }
#ifdef CONFIG_SOC_CV2
                    AmbaMisra_TouchUnused(&Proc);
#else
                    if ( Proc == 1U ) {
                        pFexData->NumHalfOctaves++;
                    }
#endif
                }
            }
        }
    }

    return Rval;
}

UINT32 SofFex_LKOpticalFlowProc(SOF_FEX_FD_HANDLE_s *pHandler, const AMBA_CV_IMG_BUF_s *pImgInfo)
{
    UINT32 Rval = 0U;
    const AMBA_CV_SPUFEX_CFG_s *pConfig;
    AMBA_CV_SPU_DATA_s *pSofData;
    UINT32 SofDataPtr;
    UINT32 Ptr;
    UINT32 OctaveIdx;

    pConfig = &pHandler->Config;
    pSofData = &pHandler->OutSofData;
    AmbaMisra_TypeCast32(&SofDataPtr, &pSofData);

    for (OctaveIdx = 0U; OctaveIdx < MAX_HALF_OCTAVES; OctaveIdx++) {
        UINT32 OctaveCtrl;
        UINT32 OctaveDisable;
        UINT32 ProcVPInstIdx;
        AMBA_CV_SPU_SCALE_OUT_s *pScale;

        SofFex_CheckRval(AmbaWrap_memcpy(&OctaveCtrl,
                                         &pImgInfo->pBuffer->pic_info.pyramid.half_octave[OctaveIdx].ctrl, sizeof(UINT32)), "AmbaWrap_memcpy", __func__);
        OctaveDisable = OctaveCtrl & 0x1U;

        pScale = &pSofData->Scales[OctaveIdx];
        if ( (OctaveDisable == 0U) && (pConfig->SpuScaleCfg[OctaveIdx].Enable == 1U) ) {
            memio_source_recv_raw_t *pLKOpcFwRawLastIn;
            memio_source_recv_raw_t *pLKOpcFwRawCurIn;
            UINT32 GetProcVPInstIdxRet;
            const roi_t Roi = pConfig->SpuScaleCfg[OctaveIdx].Roi;

            GetProcVPInstIdxRet = SofFex_GetProcVPInstIdx(pHandler, pImgInfo->pBuffer,
                                  OctaveIdx,
                                  pConfig->FexScaleCfg[OctaveIdx].RoiEnable,
                                  &Roi,
                                  &ProcVPInstIdx);

            if ( GetProcVPInstIdxRet == 0U ) {
                UINT8* pOutDxDy;
                UINT8* pOutInvalidCount;
                UINT32 ScaleRval = 0U;
                INT32  AbsRoiStartRow;
                INT32  AbsRoiStartCol;

                AmbaMisra_TypeCast32(&pLKOpcFwRawLastIn,
                                     &pHandler->LKOpcFwBuff[ProcVPInstIdx].pAlignedLastRecvRawPtr);
                AmbaMisra_TypeCast32(&pLKOpcFwRawCurIn,
                                     &pHandler->LKOpcFwBuff[ProcVPInstIdx].pAlignedCurRecvRawPtr);

                (void) PicInfoToRawInfo(pHandler, VP_SOF_FEX_RIGHT, pImgInfo->pBuffer,
                                        OctaveIdx, ProcVPInstIdx,
                                        pConfig->SpuScaleCfg[OctaveIdx].RoiEnable,
                                        &Roi,
                                        pLKOpcFwRawLastIn);
                (void) PicInfoToRawInfo(pHandler, VP_SOF_FEX_LEFT, pImgInfo->pBuffer,
                                        OctaveIdx, ProcVPInstIdx,
                                        pConfig->SpuScaleCfg[OctaveIdx].RoiEnable,
                                        &Roi,
                                        pLKOpcFwRawCurIn);

                ScaleRval |= LKOpticalFlow_Process_VP(
                                 pHandler->pLKOpcFlowHandler[ProcVPInstIdx],
                                 pLKOpcFwRawLastIn, pLKOpcFwRawCurIn,
                                 pHandler->LKOpcFwBuff[ProcVPInstIdx].pAlignedDxDyPtr,
                                 pHandler->LKOpcFwBuff[ProcVPInstIdx].pAlignedInvalidCountPtr);

                ScaleRval |= LKOpticalFlow_GetResult(pHandler->pLKOpcFlowHandler[ProcVPInstIdx],
                                                     &pOutDxDy, &pOutInvalidCount);

                ScaleRval |= LKOpticalFlow_GetInvalidCount(pOutInvalidCount,
                             &pScale->InvalidDisparities);

                pScale->Status = (INT32)ScaleRval;

                AbsRoiStartRow = pImgInfo->pBuffer->pic_info.pyramid.half_octave[OctaveIdx].roi_start_row;
                AbsRoiStartCol = pImgInfo->pBuffer->pic_info.pyramid.half_octave[OctaveIdx].roi_start_col;

                if ( pConfig->SpuScaleCfg[OctaveIdx].RoiEnable != 0U ) {
                    AbsRoiStartRow += (INT32)pConfig->SpuScaleCfg[OctaveIdx].Roi.m_start_row;
                    AbsRoiStartCol += (INT32)pConfig->SpuScaleCfg[OctaveIdx].Roi.m_start_col;
                }

                pScale->DisparityHeight = pHandler->ProcHeight[ProcVPInstIdx];
                pScale->DisparityWidth = pHandler->ProcWidth[ProcVPInstIdx];
#if defined(CONFIG_SOC_CV2)
                pScale->DisparityPitch = SOF_FEX_ALIGN32((UINT32)pHandler->ProcWidth[ProcVPInstIdx]*2U);
#else
                pScale->DisparityWidthInBytes = (UINT32)pHandler->ProcWidth[ProcVPInstIdx]*2U;
                pScale->DisparityPitch = SOF_FEX_ALIGN32(pScale->DisparityWidthInBytes);
#endif
                pScale->BufSize = pScale->DisparityPitch*pScale->DisparityHeight;
                if ( pConfig->SpuScaleCfg[OctaveIdx].RoiEnable != 0U ) {
                    pScale->RoiStartRow = pConfig->SpuScaleCfg[OctaveIdx].Roi.m_start_row;
                    pScale->RoiStartCol = pConfig->SpuScaleCfg[OctaveIdx].Roi.m_start_col;
                } else {
                    pScale->RoiStartRow = 0U;
                    pScale->RoiStartCol = 0U;
                }
                pScale->RoiAbsoluteStartRow = (UINT32)AbsRoiStartRow;
                pScale->RoiAbsoluteStartCol = (UINT32)AbsRoiStartCol;
                pScale->DisparityBpp = 16U;
                pScale->DisparityQm = 5U;
                pScale->DisparityQf = 3U;

#if defined(CONFIG_SOC_CV2)
                pScale->Reserved_0 = 0U;
                pScale->Reserved_1 = 0U;
                pScale->Reserved_2 = 0U;
                pScale->Reserved_3 = 0U;
#else
                // Other chipset follow CV2FS format
                pScale->Reserved_0 = 0U;
                pScale->Reserved_1 = 0U;
                pScale->Reserved_2 = 0U;
                pScale->Reserved_3 = 0U;
                pScale->Reserved_4 = 0U;
                pScale->Reserved_5 = 0U;
                pScale->Reserved_6 = 0U;
#endif

                AmbaMisra_TypeCast32(&Ptr, &pOutDxDy);
                pScale->DisparityMapOffset = Ptr - SofDataPtr;
            }
        } else {
            // Clean up non-used structure
            SofFex_CheckRval(AmbaWrap_memset(pScale, 0, sizeof(AMBA_CV_SPU_SCALE_OUT_s)), "AmbaWrap_memset", __func__);
            // Set to status to invalid
            pScale->Status = -1;
        }
    }

    return Rval;
}

static UINT32 GetAlignedAddr(const UINT8 *pBufferStart,
                             UINT8 **pAlignedBufferStart)
{
    UINT32 AlignedAddr;
    UINT32 Addr;
    UINT32 Rval = 0U;

    AmbaMisra_TypeCast32(&Addr, &pBufferStart);
    AlignedAddr = SOF_FEX_ALIGN128(Addr);
    AmbaMisra_TypeCast32(pAlignedBufferStart, &AlignedAddr);

    return Rval;
}

UINT32 SofFex_SetWorkBuffAligned(SOF_FEX_FD_HANDLE_s *pHandler)
{
    UINT32 VP_InstIdx;

    {
        for (VP_InstIdx = 0U; VP_InstIdx < MAX_VP_SOF_FEX_INSTANCE; VP_InstIdx++) {
            (void) GetAlignedAddr(pHandler->HarrisBuff[VP_InstIdx].BitOutMap,
                                  &pHandler->HarrisBuff[VP_InstIdx].pAlignedBitOutMapPtr);
            (void) GetAlignedAddr(pHandler->HarrisBuff[VP_InstIdx].RespMap,
                                  &pHandler->HarrisBuff[VP_InstIdx].pAlignedRespMapPtr);
            (void) GetAlignedAddr(pHandler->HarrisBuff[VP_InstIdx].RecvRawBuff,
                                  &pHandler->HarrisBuff[VP_InstIdx].pAlignedRecvRawBuffPtr);

            (void) GetAlignedAddr(pHandler->LKOpcFwBuff[VP_InstIdx].LastRecvRawBuff,
                                  &pHandler->LKOpcFwBuff[VP_InstIdx].pAlignedLastRecvRawPtr);
            (void) GetAlignedAddr(pHandler->LKOpcFwBuff[VP_InstIdx].CurRecvRawBuff,
                                  &pHandler->LKOpcFwBuff[VP_InstIdx].pAlignedCurRecvRawPtr);

            (void) GetAlignedAddr(pHandler->LKOpcFwBuff[VP_InstIdx].DxDy,
                                  &pHandler->LKOpcFwBuff[VP_InstIdx].pAlignedDxDyPtr);
            (void) GetAlignedAddr(pHandler->LKOpcFwBuff[VP_InstIdx].InvalidCount,
                                  &pHandler->LKOpcFwBuff[VP_InstIdx].pAlignedInvalidCountPtr);
        }
    }

    return 0U;
}

