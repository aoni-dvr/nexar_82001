/**
*  @file SvcStillMux.c
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
*  @details svc boot device
*
*/

#include "AmbaKAL.h"
#include "AmbaMisraFix.h"
#include "AmbaUtility.h"
#include "AmbaFS.h"
#include "AmbaWrap.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaDSP_Event.h"
#include "AmbaVIN_Def.h"
#include "AmbaGDMA.h"
#include "Amba_ExifImg.h"

#include "SvcLog.h"
#include "SvcErrCode.h"
#include "SvcWrap.h"
#include "SvcResCfg.h"
#include "SvcDSP.h"
#include "SvcStill.h"
#include "SvcStillCap.h"
#include "SvcStillMux.h"
#include "SvcTask.h"
#include "AmbaDCF.h"
#include "AmbaExif.h"
#include "SvcImg.h"
#include "SvcUtil.h"
#include "SvcVinSrc.h"
#include "SvcStgMonitor.h"
#include "SvcPlat.h"
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
#include "custom.h"
#include "app_helper.h"
#include "msg_queue/msg_queue.h"
#include "platform.h"
#endif


#define JPEG_BUF_ALIGN_BITS  (15U)
#define JPEG_BUF_ALIGN_SIZE  (0x8000U) // 2^15 = 32768 = 0x8000

#if defined(CONFIG_ICAM_MAX_REC_STRM)
#if (CONFIG_ICAM_MAX_REC_STRM + AMBA_DSP_MAX_VOUT_NUM) > AMBA_DSP_MAX_VIN_NUM
#define SVC_MAX_EXIF_INFO_NUM SVC_MAX_PIV_STREAMS
#endif
#endif

#ifndef SVC_MAX_EXIF_INFO_NUM
#define SVC_MAX_EXIF_INFO_NUM   AMBA_DSP_MAX_VIN_NUM
#endif

#define MUX_AVAILABLE (1)

typedef struct {
    UINT16                 CapSeq;
    UINT16                 PicView;
    UINT16                 Width;
    UINT16                 Height;
    ULONG                  StartAddr;
    UINT32                 PicSize;
    UINT8                  Action;
#define SVC_STL_MUX_CREATE_FILE (0U)
#define SVC_STL_MUX_INPUT_PIC  (1U)
    UINT8                  PicType;
    UINT8                  CapType;
    UINT8                  StreamId;
    UINT16                 SensorIdx;
    UINT16                 AebIdx;
} SVC_STL_PIC_MSG_s;

typedef struct {
    SVC_TASK_CTRL_s             TaskCtrl;
    AMBA_KAL_MSG_QUEUE_t        PicQue;
    SVC_STL_PIC_MSG_s           PicQueBuf[SVC_STL_MUX_NUM_FILES * SVC_STL_NUM_VIEW];
    ULONG                       StillBufStart;
    UINT32                      StillBufSize;
    ULONG                       StillBufEnd;
    ULONG                       StillBufWp;
    UINT32                      StillBufAvail;
    ULONG                       VideoBufStart;
    UINT32                      VideoBufSize;
    ULONG                       VideoBufEnd;
    ULONG                       YuvBufStart;
    UINT32                      YuvBufSize;
    ULONG                       YuvBufEnd;
    ULONG                       RawBufStart;
    UINT32                      RawBufSize;
    ULONG                       RawBufEnd;
    char                        VidThmFileName[CONFIG_ICAM_MAX_REC_STRM][SVC_STL_MUX_MAX_FILENAME_LEN];
    UINT8                       VidThmFileNameLen[CONFIG_ICAM_MAX_REC_STRM];
    AMBA_EXIF_FORMATTER_INFO_s  ExifFormaterInfo;
    UINT8                       Makernote[AMBA_EXIF_MAKERNOTE_SIZE];
    EXIF_INFO_s                 ExifInfo[SVC_MAX_EXIF_INFO_NUM][SVC_CAP_MAX_SENSOR_PER_VIN][SVC_STL_MAX_AEB_NUM];
    UINT8                       ExifStreamId[SVC_STL_MUX_NUM_FILES];
    UINT16                      ExifSensorIdx[SVC_STL_MUX_NUM_FILES];
    UINT16                      ExifAebIdx[SVC_STL_MUX_NUM_FILES];
    UINT8                       ExifWp;
    UINT8                       ExifRp;
    char                        FileName[SVC_STL_MUX_NUM_FILES][SVC_STL_MUX_MAX_FILENAME_LEN];
    UINT8                       FileWp;
    UINT8                       FileRp;
    UINT8                       NumFile;
    AMBA_VFS_FILE_s             File[SVC_STL_MUX_NUM_FILES];
    UINT32                      PicSize[SVC_STL_NUM_VIEW];
    AMBA_KAL_EVENT_FLAG_t       Status;
    char                        StorageDrive;
} SVC_STL_MUX_CTRL_s;

static UINT8 StlMux_DebugOn = 0;
static UINT8 StlMux_Created = 0;
static SVC_STL_MUX_CTRL_s StlMux GNU_SECTION_NOZEROINIT;

static void StillMux_CreateFile(const SVC_STL_PIC_MSG_s *pMsg);
static void StillMux_SaveDataDirectly(const SVC_STL_PIC_MSG_s *pMsg);

/**
* query memory size needed for still mux task
* @param [in] pSetup pointer to configuration of still muxer task
* @param [out] pMemSize pointer to needed memory size
* @return 0-OK, 1-NG
*/
UINT32 SvcStillMux_QueryMem(const SVC_STL_MUX_SETUP_s *pSetup, UINT32 *pMemSize)
{
    *pMemSize = pSetup->MaxWidth * pSetup->MaxHeight;
    *pMemSize += pSetup->MaxScrW * pSetup->MaxScrH;
    *pMemSize += SVC_STL_THMB_W * (SVC_STL_THMB_W * (UINT32)pSetup->MaxScrH / (UINT32)pSetup->MaxScrW);
    *pMemSize = (*pMemSize * 3U) >> 1U; // 420
    *pMemSize = ((*pMemSize + JPEG_BUF_ALIGN_SIZE - 1U) >> JPEG_BUF_ALIGN_BITS) << JPEG_BUF_ALIGN_BITS;
    *pMemSize *= pSetup->NumFrameBuf;
    if (StlMux_DebugOn == 1U) {
        SvcLog_DBG(SVC_LOG_STL_MUX, "[SvcStillMux_QueryMem] w %u h %u", pSetup->MaxWidth, pSetup->MaxHeight);
    }
    SvcLog_OK(SVC_LOG_STL_MUX, "[SvcStillMux_QueryMem] memory size %u", *pMemSize, 0);
    return SVC_OK;
}

static void SetExif(UINT8 StreamId, UINT16 SensorIdx, UINT16 AebIdx)
{
    StlMux.ExifFormaterInfo.DeviceSetDescripSize      = StlMux.ExifInfo[StreamId][SensorIdx][AebIdx].PriImgDataSize;
    StlMux.ExifFormaterInfo.pDeviceSetDescrip         = StlMux.ExifInfo[StreamId][SensorIdx][AebIdx].PrivateImageData;
    StlMux.ExifFormaterInfo.PAAA.ExposureTimeNum      = StlMux.ExifInfo[StreamId][SensorIdx][AebIdx].ExposureTimeNum;
    StlMux.ExifFormaterInfo.PAAA.ExposureTimeDen      = StlMux.ExifInfo[StreamId][SensorIdx][AebIdx].ExposureTimeDen;
    StlMux.ExifFormaterInfo.PAAA.FNumberNum           = StlMux.ExifInfo[StreamId][SensorIdx][AebIdx].FNumberNum;
    StlMux.ExifFormaterInfo.PAAA.FNumberDen           = StlMux.ExifInfo[StreamId][SensorIdx][AebIdx].FNumberDen;
    StlMux.ExifFormaterInfo.PAAA.ExposureProgram      = StlMux.ExifInfo[StreamId][SensorIdx][AebIdx].ExposureProgram;
    StlMux.ExifFormaterInfo.PAAA.PhotoGraphicSentivity= StlMux.ExifInfo[StreamId][SensorIdx][AebIdx].IsoSpeedRating;
    StlMux.ExifFormaterInfo.PAAA.ShutterSpeedNum      = StlMux.ExifInfo[StreamId][SensorIdx][AebIdx].ShutterSpeedNum;
    StlMux.ExifFormaterInfo.PAAA.ShutterSpeedDen      = StlMux.ExifInfo[StreamId][SensorIdx][AebIdx].ShutterSpeedDen;
    StlMux.ExifFormaterInfo.PAAA.ApertureValueNum     = StlMux.ExifInfo[StreamId][SensorIdx][AebIdx].ApertureValueNum;
    StlMux.ExifFormaterInfo.PAAA.ApertureValueDen     = StlMux.ExifInfo[StreamId][SensorIdx][AebIdx].ApertureValueDen;
    StlMux.ExifFormaterInfo.PAAA.ExposureBiasValueNum = StlMux.ExifInfo[StreamId][SensorIdx][AebIdx].ExposureBiasValueNum;
    StlMux.ExifFormaterInfo.PAAA.ExposureBiasValueDen = StlMux.ExifInfo[StreamId][SensorIdx][AebIdx].ExposureBiasValueDen;
    StlMux.ExifFormaterInfo.PAAA.MaxApertureValueNum  = StlMux.ExifInfo[StreamId][SensorIdx][AebIdx].MaxApertureValueNum;
    StlMux.ExifFormaterInfo.PAAA.MaxApertureValueDen  = StlMux.ExifInfo[StreamId][SensorIdx][AebIdx].MaxApertureValueDen;
    StlMux.ExifFormaterInfo.PAAA.SubjectDistanceRange = StlMux.ExifInfo[StreamId][SensorIdx][AebIdx].SubjectDistanceRange;
    StlMux.ExifFormaterInfo.PAAA.SubjectDistanceNum   = StlMux.ExifInfo[StreamId][SensorIdx][AebIdx].SubjectDistanceNum;
    StlMux.ExifFormaterInfo.PAAA.SubjectDistanceDen   = StlMux.ExifInfo[StreamId][SensorIdx][AebIdx].SubjectDistanceDen;
    StlMux.ExifFormaterInfo.PAAA.MeteringMode         = StlMux.ExifInfo[StreamId][SensorIdx][AebIdx].MeteringMode;
    StlMux.ExifFormaterInfo.PAAA.LightSource          = StlMux.ExifInfo[StreamId][SensorIdx][AebIdx].LightSource;
    StlMux.ExifFormaterInfo.PAAA.Flash                = StlMux.ExifInfo[StreamId][SensorIdx][AebIdx].Flash;
    StlMux.ExifFormaterInfo.PAAA.FocalLengthNum       = StlMux.ExifInfo[StreamId][SensorIdx][AebIdx].FocalLengthNum;
    StlMux.ExifFormaterInfo.PAAA.FocalLengthDen       = StlMux.ExifInfo[StreamId][SensorIdx][AebIdx].FocalLengthDen;
    for (UINT32 i = 0U; i < 4U; i++) {
        StlMux.ExifFormaterInfo.PAAA.FlashpixVersion[i] = StlMux.ExifInfo[StreamId][SensorIdx][AebIdx].FlashPixVersion[i];
    }
    StlMux.ExifFormaterInfo.PAAA.ColorSpace           = StlMux.ExifInfo[StreamId][SensorIdx][AebIdx].ColorSpace;
    StlMux.ExifFormaterInfo.PAAA.ExposureIndexNum     = StlMux.ExifInfo[StreamId][SensorIdx][AebIdx].ExposureIndexNum;
    StlMux.ExifFormaterInfo.PAAA.ExposureIndexDen     = StlMux.ExifInfo[StreamId][SensorIdx][AebIdx].ExposureIndexDen;
    StlMux.ExifFormaterInfo.PAAA.SensingMethod        = StlMux.ExifInfo[StreamId][SensorIdx][AebIdx].SensingMethod;
    StlMux.ExifFormaterInfo.PAAA.FileSource           = StlMux.ExifInfo[StreamId][SensorIdx][AebIdx].FileSource;
    StlMux.ExifFormaterInfo.PAAA.SceneType            = StlMux.ExifInfo[StreamId][SensorIdx][AebIdx].SceneType;
    StlMux.ExifFormaterInfo.PAAA.CustomRendered       = StlMux.ExifInfo[StreamId][SensorIdx][AebIdx].CustomRendered;
    StlMux.ExifFormaterInfo.PAAA.ExposureMode         = StlMux.ExifInfo[StreamId][SensorIdx][AebIdx].ExposureMode;
    StlMux.ExifFormaterInfo.PAAA.WhiteBalance         = StlMux.ExifInfo[StreamId][SensorIdx][AebIdx].WhiteBalance;
    StlMux.ExifFormaterInfo.PAAA.DigitalZoomRatioNum  = StlMux.ExifInfo[StreamId][SensorIdx][AebIdx].DigitalZoomRatioNum;
    StlMux.ExifFormaterInfo.PAAA.DigitalZoomRatioDen  = StlMux.ExifInfo[StreamId][SensorIdx][AebIdx].DigitalZoomRatioDen;
    StlMux.ExifFormaterInfo.PAAA.FocalLength35mmFilm  = StlMux.ExifInfo[StreamId][SensorIdx][AebIdx].FocalLength35mmFilm;
    StlMux.ExifFormaterInfo.PAAA.SceneCaptureType     = StlMux.ExifInfo[StreamId][SensorIdx][AebIdx].SceneType;
    StlMux.ExifFormaterInfo.PAAA.GainControl          = StlMux.ExifInfo[StreamId][SensorIdx][AebIdx].GainControl;
    StlMux.ExifFormaterInfo.PAAA.Contrast             = StlMux.ExifInfo[StreamId][SensorIdx][AebIdx].Contrast;
    StlMux.ExifFormaterInfo.PAAA.Saturation           = StlMux.ExifInfo[StreamId][SensorIdx][AebIdx].Saturation;
    StlMux.ExifFormaterInfo.PAAA.Sharpness            = StlMux.ExifInfo[StreamId][SensorIdx][AebIdx].Sharpness;
}

/**
* set EXIF 3A content by specified FOV index
* @param [in] FovId index of fov
* @param [in] StreamId id of record stream
* @return 0-OK, 1-NG
*/
UINT32 SvcStillMux_SetExifByFov(UINT32 FovId, UINT8 StreamId)
{
    UINT32 VinID;
    UINT32 SensorIdx;
    UINT32 Rval, FovSrc;

    if (StreamId < (UINT32)SVC_MAX_EXIF_INFO_NUM) {
        Rval = SvcResCfg_GetSensorIdxOfFovIdx(FovId, &VinID, &SensorIdx);
        if (Rval == OK) {
            Rval = SvcResCfg_GetFovSrc(FovId, &FovSrc);
            if (Rval == OK) {
                if (FovSrc == SVC_VIN_SRC_YUV) {
                    Rval = AmbaWrap_memset(&(StlMux.ExifInfo[StreamId][0][0]), 0, sizeof(EXIF_INFO_s));
                    if (Rval != OK) {
                        SvcLog_NG(SVC_LOG_STL_MUX, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
                    }
#if defined(CONFIG_BUILD_IMGFRW_AAA) && defined(CONFIG_BUILD_COMSVC_MX_EXIF)
                } else {
                    EXIF_INFO_s ExifInfo;
                    COMPUTE_EXIF_PARAMS_s ComputeExif;
                    UINT32 AlgoId;
                    Rval = SvcImg_AlgoIdGet(VinID, SensorIdx, &AlgoId);
                    if (Rval == OK) {
                        ComputeExif.ChannelNo = AlgoId;
                        ComputeExif.Mode      = IMG_EXIF_PIV;
                        ComputeExif.AeIdx     = 0;
                        Amba_Exif_Compute_AAA_Exif(&ComputeExif);
                        Rval = Amba_Exif_Get_Exif_Info(0, &ExifInfo);
                        if (Rval == OK) {
                            Rval = AmbaWrap_memcpy(&(StlMux.ExifInfo[StreamId][0][0]),
                                                   &ExifInfo,
                                                   sizeof(EXIF_INFO_s));
                            if (Rval != OK) {
                                SvcLog_NG(SVC_LOG_STL_MUX, "[line %u] AmbaWrap_memcpy return 0x%x", __LINE__, Rval);
                            }
                        }
                    }
#endif
                }
            }
        }
    } else {
        SvcLog_NG(SVC_LOG_STL_MUX, "[SvcStillMux_SetExifByFov] StreamId %u >= SVC_MAX_EXIF_INFO_NUM %u", StreamId, SVC_MAX_EXIF_INFO_NUM);
        Rval = SVC_NG;
    }
    return Rval;
}

/**
* set EXIF 3A content by specified VIN index
* @param [in] VinId index of vin
* @param [in] SensorIdx index of sensor
* @param [in] SensorNum number of sensors
* @param [in] AebNum number of AEB
* @return 0-OK, 1-NG
*/
UINT32 SvcStillMux_SetExifByVin(UINT32 VinId, UINT32 SensorIdx, UINT32 SensorNum, UINT8 AebNum)
{
    UINT32 Rval, Src = 0;

    if (VinId < (UINT32)SVC_MAX_EXIF_INFO_NUM) {
        Rval = SvcResCfg_GetVinSrc(VinId, &Src);
        if (Rval == OK) {
            if (Src == SVC_VIN_SRC_YUV) {
                Rval = AmbaWrap_memset(&(StlMux.ExifInfo[VinId][SensorIdx][0]), 0, sizeof(EXIF_INFO_s) * AebNum);
                if (Rval != OK) {
                    SvcLog_NG(SVC_LOG_STL_MUX, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
                }
#ifdef CONFIG_BUILD_IMGFRW_AAA
            } else {
                UINT32 AlgoId;
                void *pExifInfo;
                Rval = SvcImg_AlgoIdGet(VinId, SensorNum, &AlgoId);
                if (Rval == OK) {
                    UINT8 AebIdx;
                    for (AebIdx = 0; AebIdx < AebNum; AebIdx ++) {
                        SvcImg_StillExif(VinId, SensorNum, &pExifInfo);
                        Rval = AmbaWrap_memcpy(&(StlMux.ExifInfo[VinId][SensorIdx][AebIdx]),
                                               pExifInfo,
                                               sizeof(EXIF_INFO_s));
                        if (Rval != OK) {
                            SvcLog_NG(SVC_LOG_STL_MUX, "[line %u] AmbaWrap_memcpy return 0x%x", __LINE__, Rval);
                        }
                    }
                    Rval = OK;
                }
#else
                AmbaMisra_TouchUnused(&SensorNum);
#endif
            }
        }
    } else {
        SvcLog_NG(SVC_LOG_STL_MUX, "[SvcStillMux_SetExifByVin] VinId %u >= SVC_MAX_EXIF_INFO_NUM %u", VinId, SVC_MAX_EXIF_INFO_NUM);
        Rval = SVC_NG;
    }
    return Rval;
}

/**
* set video thumbnail file name
* @param [in] RecStream id of record stream
* @param [in] pFileName pointer to file name
* @param [in] FileNameLength size of file name
* @return 0-OK, 1-NG
*/
UINT32 SvcStillMux_SetVidThmFileName(UINT8 RecStream, char *pFileName, UINT8 FileNameLength)
{
    UINT32 Rval;
    if (FileNameLength > (UINT8)SVC_STL_MUX_MAX_FILENAME_LEN) {
        SvcLog_NG(SVC_LOG_STL_MUX, "[SvcStillMux_SetVidThmFileName] FileNameLength %u > SVC_STL_MUX_MAX_FILENAME_LEN %u", FileNameLength, (UINT8)SVC_STL_MUX_MAX_FILENAME_LEN);
        Rval = SVC_NG;
    } else {
        Rval = AmbaWrap_memcpy(StlMux.VidThmFileName[RecStream], pFileName, FileNameLength);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_STL_MUX, "[line %u] AmbaWrap_memcpy return 0x%x", __LINE__, Rval);
        } else {
            AmbaMisra_TouchUnused(pFileName);
            StlMux.VidThmFileNameLen[RecStream] = FileNameLength;
        }
    }
    return Rval;
}

static UINT32 SvcStillMux_GetVidThmFileName(UINT8 RecStream, char *pFileName)
{
    UINT32 Rval;
    Rval = AmbaWrap_memcpy(pFileName, StlMux.VidThmFileName[RecStream], StlMux.VidThmFileNameLen[RecStream]);
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_STL_MUX, "[line %u] AmbaWrap_memcpy return 0x%x", __LINE__, Rval);
    }
    return SVC_OK;
}

static void WriteToStillBuf(ULONG Addr, UINT32 Size)
{
    UINT32 Rval;
    UINT8 *pSrc, *pDst;

    if (StlMux.StillBufAvail < Size) {
        SvcLog_NG(SVC_LOG_STL_MUX, "[WriteToStillBuf] size %u over avail %u. Please increase mux buffer size", Size, StlMux.StillBufAvail);
    } else {
        if ((StlMux.StillBufWp + (ULONG)Size - 1UL) > StlMux.StillBufEnd) {
            ULONG Temp1 = (StlMux.StillBufEnd - StlMux.StillBufWp) + 1UL;
            ULONG Addr1;
            UINT32 Temp = 0;
            if (Temp1 < 0xFFFFFFFFUL) {
                Temp = (UINT32)Temp1;
            } else {
                SvcStillLog("[WriteToStillBuf] ERR: Temp1 %p >= 0xFFFFFFFFUL", Temp1, 0, 0, 0, 0);
            }
            AmbaMisra_TypeCast(&pSrc, &Addr);
            AmbaMisra_TypeCast(&pDst, &StlMux.StillBufWp);
            if (StlMux_DebugOn == 1U) {
                SvcStillLog("[WriteToStillBuf] 1. copy from %p to %p size 0x%x", Addr, StlMux.StillBufWp, Temp, 0, 0);
            }
            Rval = SvcStillBufCopy(pDst, pSrc, Temp);
            if (Rval == SVC_OK) {
                if (StlMux_DebugOn == 1U) {
                    SvcLog_DBG(SVC_LOG_STL_MUX, "copy done", 0, 0);
                }
                Addr1 = Addr + Temp;
                Temp = Size - Temp;
                if (StlMux_DebugOn == 1U) {
                    SvcStillLog("[WriteToStillBuf] 2. copy from %p to %p size 0x%x", Addr1, StlMux.StillBufStart, Temp, 0, 0);
                }
                AmbaMisra_TypeCast(&pSrc, &Addr1);
                AmbaMisra_TypeCast(&pDst, &StlMux.StillBufStart);
                Rval = SvcStillBufCopy(pDst, pSrc, Temp);
                if (Rval == SVC_OK) {
                    if (StlMux_DebugOn == 1U) {
                        SvcLog_DBG(SVC_LOG_STL_MUX, "copy done", 0, 0);
                    }
                }
            }
        } else {
            AmbaMisra_TypeCast(&pSrc, &Addr);
            AmbaMisra_TypeCast(&pDst, &StlMux.StillBufWp);
            if (StlMux_DebugOn == 1U) {
                SvcStillLog("[WriteToStillBuf] copy from %p to %p size 0x%x", Addr, StlMux.StillBufWp, Size, 0, 0);
            }
            Rval = SvcStillBufCopy(pDst, pSrc, Size);
            if (Rval == SVC_OK) {
                if (StlMux_DebugOn == 1U) {
                    SvcLog_DBG(SVC_LOG_STL_MUX, "copy done", 0, 0);
                }
            }
        }
#if defined(CONFIG_AMBA_REC_GDMA_USED)
        Rval = AmbaGDMA_WaitAllCompletion(1000);
        if (Rval != SVC_OK) {
            SvcLog_NG(SVC_LOG_STL_MUX, "AmbaGDMA_WaitAllCompletion return 0x%x", Rval, 0);
        }
#endif
        AmbaMisra_TouchUnused(pSrc);
        if ((StlMux.StillBufWp + (ULONG)Size) > StlMux.StillBufEnd) {
            StlMux.StillBufWp = StlMux.StillBufStart + ((ULONG)Size - ((StlMux.StillBufEnd - StlMux.StillBufWp) + 1U));
        } else {
            StlMux.StillBufWp += (ULONG)Size;
        }
        StlMux.StillBufAvail -= Size;
        if (StlMux_DebugOn == 1U) {
            SvcLog_DBG(SVC_LOG_STL_MUX, "StillBufAvail %u", StlMux.StillBufAvail, 0);
        }
    }
}

static void WriteFromVideoBuf(ULONG PicAddr, UINT32 PicSize, UINT8 PicType)
{
    if (PicSize != 0U) {
        ULONG SrcBufEnd;
        ULONG DstStart = StlMux.StillBufWp;
        if ((PicType == SVC_STL_TYPE_LUMA) || (PicType == SVC_STL_TYPE_CHROMA)) {
            SrcBufEnd = StlMux.YuvBufEnd;
        } else {
            if ((PicType == SVC_STL_TYPE_RAWONLY) || (PicType == SVC_STL_TYPE_RAW) || (PicType == SVC_STL_TYPE_CLEAN_RAW) || (PicType == SVC_STL_TYPE_CE)) {
                SrcBufEnd = StlMux.RawBufEnd;
            } else {
                SrcBufEnd = StlMux.VideoBufEnd;
            }
        }
        if (StlMux_DebugOn == 1U) {
            SvcStillLog("[WriteFromVideoBuf] PicAddr + PicSize - 1U = %p, SrcBufEnd %p", PicAddr + (ULONG)PicSize - 1UL, SrcBufEnd, 0, 0, 0);
        }
        if ((PicAddr + PicSize - 1UL) > SrcBufEnd) {
            ULONG Size = (SrcBufEnd - PicAddr) + 1U;
            if (Size < 0xFFFFFFFFUL) {
                if (StlMux_DebugOn == 1U) {
                    SvcLog_DBG(SVC_LOG_STL_MUX, "write 0x%x", (UINT32)Size, 0);
                }
                WriteToStillBuf(PicAddr, (UINT32)Size);
                if (StlMux_DebugOn == 1U) {
                    SvcLog_DBG(SVC_LOG_STL_MUX, "write 0x%x", PicSize - (UINT32)Size, 0);
                }
                WriteToStillBuf(StlMux.VideoBufStart, PicSize - (UINT32)Size);
            } else {
                SvcStillLog("WriteFromVideoBuf ERR: Size %p", Size, 0, 0, 0, 0);
            }
        } else {
            WriteToStillBuf(PicAddr, PicSize);
        }
        if (pSvDataReady != NULL) {
            SVC_STILL_DATAREADY_s Info;
            Info.BufBase = StlMux.StillBufStart;
            Info.BufSize = StlMux.StillBufSize;
            Info.DataStart = DstStart;
            Info.DataSize = PicSize;
            Info.PicType = PicType;
            pSvDataReady(&Info);
        }
    }
    if ((PicType != SVC_STL_TYPE_LUMA) && (PicType != SVC_STL_TYPE_RAW)) { // For luma/raw data, set available after chroma/ce data saved.
        UINT32 Rval = AmbaKAL_EventFlagSet(&StlMux.Status, MUX_AVAILABLE);
        if (OK != Rval) {
            SvcLog_NG(SVC_LOG_STL_MUX, "Event flag set failed with 0x%x", Rval, 0U);
        }
    }
}

/**
* create picture file
* @param [in] PicType 0 - normal picture. 1 - video thumbnail. 2 - dump YUV.
* @param [in] CapType SVC_FRAME_TYPE_YUV - liveview yuv. SVC_FRAME_TYPE_RAW - still raw.
* @param [in] StreamId stream id or vin id
* @param [in] SensorIdx sensor id
* @param [in] CapSeq capture sequence for burst capture
* @param [in] AebIdx AEB index
* @return 0-OK, 1-NG
*/
UINT32 SvcStillMux_CreatePicFile(UINT8 PicType, UINT8 CapType, UINT16 StreamId, UINT16 SensorIdx, UINT16 CapSeq, UINT16 AebIdx)
{
    SVC_STL_PIC_MSG_s PicMsg;
    UINT32 Rval = AmbaWrap_memset(&PicMsg, 0, sizeof(PicMsg));
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_STL_MUX, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
    } else {
        if (PicType == SVC_STL_TYPE_NULL_WR) {
            Rval = OK;
        } else if (PicType == SVC_STL_TYPE_IDSP) {
            Rval = OK;
            PicMsg.PicType = PicType;
            PicMsg.CapType = CapType;
            PicMsg.StreamId = (UINT8)StreamId;
            StillMux_SaveDataDirectly(&PicMsg);
        } else {
            PicMsg.Action = SVC_STL_MUX_CREATE_FILE;
            PicMsg.CapSeq = CapSeq;
            PicMsg.PicType = PicType;
            PicMsg.CapType = CapType;
            PicMsg.StreamId = (UINT8)StreamId;
            PicMsg.SensorIdx = SensorIdx;
            PicMsg.AebIdx = AebIdx;
            Rval = AmbaKAL_MsgQueueSend(&StlMux.PicQue, &PicMsg, 1000);
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_STL_MUX, "[SvcStillMux_CreatePicFile] Message send failed with 0x%x", Rval, 0U);
            } else {
                if (StlMux_DebugOn == 1U) {
                    SvcLog_DBG(SVC_LOG_STL_MUX, "[SvcStillMux_CreatePicFile] PicType %u StreamId %u", PicType, StreamId);
                }
            }
        }
    }
    return Rval;
}

/**
* wait still mux available
* @return 0-OK, 1-NG
*/
UINT32 SvcStillMux_WaitAvail(void)
{
    UINT32 Rval, ActualFlags = 0;
    Rval = AmbaKAL_EventFlagGet(&StlMux.Status, MUX_AVAILABLE, AMBA_KAL_FLAGS_ALL, AMBA_KAL_FLAGS_CLEAR_NONE, &ActualFlags, 10000);
    if (OK != Rval) {
        SvcLog_NG(SVC_LOG_STL_MUX, "Event flag get failed with 0x%x", Rval, 0U);
    }
    return Rval;
}

/**
* input picture to still muxer task
* @param [in] pPicInput pointer to info block of picture
* @return 0-OK, 1-NG
*/
UINT32 SvcStillMux_InputPic(const SVC_STL_MUX_INPUT_s *pPicInput)
{
    SVC_STL_PIC_MSG_s PicMsg;
    UINT32 Rval = AmbaWrap_memset(&PicMsg, 0, sizeof(SVC_STL_PIC_MSG_s));
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_STL_MUX, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
    } else {
        Rval = AmbaKAL_EventFlagClear(&StlMux.Status, 0xFFFFFFFFU);
        if (OK != Rval) {
            SvcLog_NG(SVC_LOG_STL_MUX, "Event flag clear failed with 0x%x", Rval, 0U);
        } else {
            SvcStillCap_SetStatus(SVC_STL_MUX_BUSY);

            PicMsg.StartAddr        = pPicInput->PicRdy.StartAddr       ;
            PicMsg.PicSize          = pPicInput->PicRdy.PicSize         ;

            if (StlMux.StillBufSize < PicMsg.PicSize) {
                SvcLog_NG(SVC_LOG_STL_MUX, "[SvcStillMux_InputPic] PicSize %u larger than still buffer size %u", PicMsg.PicSize, StlMux.StillBufSize);
                Rval = SVC_NG;
            } else {
                PicMsg.PicView = pPicInput->PicView;
                PicMsg.CapSeq = pPicInput->RawSeq;
                PicMsg.Width = pPicInput->Width;
                PicMsg.Height = pPicInput->Height;
                SvcStillLog("[SvcStillMux_InputPic] StartAddr %p PicSize 0x%x", pPicInput->PicRdy.StartAddr, pPicInput->PicRdy.PicSize, 0, 0, 0);
                PicMsg.PicType = pPicInput->PicType;
                PicMsg.Action = SVC_STL_MUX_INPUT_PIC;
                //WriteFromVideoBuf(PicMsg.StartAddr, PicMsg.PicSize, PicMsg.PicType);
                Rval = AmbaKAL_MsgQueueSend(&StlMux.PicQue, &PicMsg, 1000);
                if (OK != Rval) {
                    SvcLog_NG(SVC_LOG_STL_MUX, "[SvcStillMux_InputPic] Message send failed with 0x%x", Rval, 0U);
                } else {
                    if (StlMux_DebugOn == 1U) {
                        SvcLog_DBG(SVC_LOG_STL_MUX, "[SvcStillMux_InputPic] PicType %u", PicMsg.PicType, 0U);
                    }
                }
            }
        }
    }
    return Rval;
}

static void CreateFileProcess(void)
{
    UINT32 Rval;
    SvcLog_DBG(SVC_LOG_STL_MUX, "AmbaVFS_Open() start", 0, 0);
    if (StlMux.NumFile < (UINT8)SVC_STL_MUX_NUM_FILES) {
        Rval = AmbaVFS_Open(StlMux.FileName[StlMux.FileWp], "w", 1U, &StlMux.File[StlMux.FileWp]);
    } else {
        SvcLog_NG(SVC_LOG_STL_MUX, "StlMux.NumFile %u SVC_STL_MUX_NUM_FILES %u", StlMux.NumFile, SVC_STL_MUX_NUM_FILES);
        Rval = SVC_NG;
    }
    if (OK != Rval) {
        SvcLog_NG(StlMux.FileName[StlMux.FileWp], "AmbaVFS_Open() failed. return 0x%x", Rval, 0);
    } else {
        SvcLog_OK(StlMux.FileName[StlMux.FileWp], "file created", 0, 0);
        if (StlMux.FileWp < ((UINT8)SVC_STL_MUX_NUM_FILES - 1U)) {
            StlMux.FileWp ++;
        } else {
            StlMux.FileWp = 0;
        }
        StlMux.NumFile ++;
    }
}

static void StillMux_CreateFile(const SVC_STL_PIC_MSG_s *pMsg)
{
    UINT32 Rval, Temp;
    const SVC_RES_CFG_s  *pResCfg = SvcResCfg_Get();
#if defined(CONFIG_SVC_ENABLE_VOUT_DEF_IMG)
    UINT32 DispNum = 0;
#else
    UINT32 DispNum = pResCfg->DispNum;
#endif
    if (StlMux_DebugOn == 1U) {
        SvcLog_DBG(SVC_LOG_STL_MUX, "StreamId %u SensorIdx %u", pMsg->StreamId, pMsg->SensorIdx);
        SvcLog_DBG(SVC_LOG_STL_MUX, "CapSeq %u AebIdx %u", pMsg->CapSeq, pMsg->AebIdx);
    }
    if ((pMsg->PicType == SVC_STL_TYPE_THM) || (pMsg->PicType == SVC_STL_TYPE_THMYUV)) {
        Rval = SvcStillMux_GetVidThmFileName(pMsg->StreamId - (UINT8)DispNum, StlMux.FileName[StlMux.FileWp]);
        if (pMsg->PicType == SVC_STL_TYPE_THMYUV) {
            ULONG StrLen = AmbaUtility_StringLength(StlMux.FileName[StlMux.FileWp]);
            if ((StrLen + 3UL) < SVC_STL_MUX_MAX_FILENAME_LEN) {
                UINT8 FileWpY = StlMux.FileWp;
                StlMux.FileName[StlMux.FileWp][StrLen] = '.';
                StlMux.FileName[StlMux.FileWp][StrLen + 1U] = 'y';
                StlMux.FileName[StlMux.FileWp][StrLen + 2U] = '\0';
                CreateFileProcess();
                Temp = AmbaWrap_memcpy(StlMux.FileName[StlMux.FileWp], StlMux.FileName[FileWpY], StrLen);
                if (Temp != OK) {
                    SvcLog_NG(SVC_LOG_STL_MUX, "[line %u] AmbaWrap_memcpy return 0x%x", __LINE__, Temp);
                } else {
                    StlMux.FileName[StlMux.FileWp][StrLen] = '.';
                    StlMux.FileName[StlMux.FileWp][StrLen + 1U] = 'u';
                    StlMux.FileName[StlMux.FileWp][StrLen + 2U] = 'v';
                    StlMux.FileName[StlMux.FileWp][StrLen + 3U] = '\0';
                }
            } else {
                SvcLog_NG(SVC_LOG_STL_MUX, "StillMux_CreateFile StrLen + 3 = %u, max %u", (UINT32)StrLen + 3U, (UINT32)SVC_STL_MUX_MAX_FILENAME_LEN);
            }
        }
    } else {
        SVC_STG_MONI_DRIVE_INFO_s DriveInfo;
        UINT8 StreamId = pMsg->StreamId;

        SvcLog_DBG(SVC_LOG_STL_MUX, "AmbaDCF_CreateFileName start", 0, 0);
        if ((pMsg->PicType != SVC_STL_TYPE_YUV) && (pMsg->PicType != SVC_STL_TYPE_THMYUV_PIV) && (pMsg->PicType != SVC_STL_TYPE_RAWONLY) && (pMsg->PicType != SVC_STL_TYPE_RAW) && (pMsg->PicType != SVC_STL_TYPE_CE)){
            StlMux.ExifStreamId[StlMux.ExifWp] = pMsg->StreamId;
            StlMux.ExifSensorIdx[StlMux.ExifWp] = pMsg->SensorIdx;
            StlMux.ExifAebIdx[StlMux.ExifWp] = pMsg->AebIdx;
            if (StlMux.ExifWp < ((UINT8)SVC_STL_MUX_NUM_FILES - 1U)) {
                StlMux.ExifWp ++;
            } else {
                StlMux.ExifWp = 0;
            }
        }
        Rval = SvcStgMonitor_GetDriveInfo(StlMux.StorageDrive, &DriveInfo);
        if (pMsg->CapType != SVC_FRAME_TYPE_RAW) {
            if (StreamId >= (UINT8)DispNum) {
                StreamId -= (UINT8)DispNum;
            }
        }
        if (Rval == SVC_OK) {
            if (pMsg->CapType == SVC_FRAME_TYPE_RAW) {
                Rval = AmbaDCF_CreateFileName(DriveInfo.DriveID, 2, AMBA_DCF_FILE_TYPE_IMAGE, StlMux.FileName[StlMux.FileWp], ((UINT32)StreamId * 10U) + pMsg->SensorIdx, pMsg->CapSeq);
            } else {
                Rval = AmbaDCF_CreateFileName(DriveInfo.DriveID, 2, AMBA_DCF_FILE_TYPE_IMAGE, StlMux.FileName[StlMux.FileWp], pMsg->SensorIdx, StreamId);
            }
        }
        if (Rval == OK) {
            SvcLog_DBG(StlMux.FileName[StlMux.FileWp], "AmbaDCF_CreateFileName done", 0, 0);

            if((pMsg->PicType == SVC_STL_TYPE_YUV) || (pMsg->PicType == SVC_STL_TYPE_THMYUV_PIV)
            || (pMsg->PicType == SVC_STL_TYPE_RAW) || (pMsg->PicType == SVC_STL_TYPE_CLEAN_RAW) || (pMsg->PicType == SVC_STL_TYPE_RAWONLY)
            || (pMsg->PicType == SVC_STL_TYPE_CE)) {
                ULONG StrLen = AmbaUtility_StringLength(StlMux.FileName[StlMux.FileWp]);
                if ((StrLen >= 3UL) && (StrLen <= SVC_STL_MUX_MAX_FILENAME_LEN)) {
                    if (pMsg->PicType == SVC_STL_TYPE_YUV) {
                        UINT8 FileWpY = StlMux.FileWp;
                        StlMux.FileName[StlMux.FileWp][StrLen - 3U] = 'y';
                        StlMux.FileName[StlMux.FileWp][StrLen - 2U] = '\0';
                        CreateFileProcess();
                        Temp = AmbaWrap_memcpy(StlMux.FileName[StlMux.FileWp], StlMux.FileName[FileWpY], StrLen);
                        if (Temp != OK) {
                            SvcLog_NG(SVC_LOG_STL_MUX, "[line %u] AmbaWrap_memcpy return 0x%x", __LINE__, Temp);
                        } else {
                            StlMux.FileName[StlMux.FileWp][StrLen - 3U] = 'u';
                            StlMux.FileName[StlMux.FileWp][StrLen - 2U] = 'v';
                            StlMux.FileName[StlMux.FileWp][StrLen - 1U] = '\0';
                        }
                    }
                    if (pMsg->PicType == SVC_STL_TYPE_THMYUV_PIV) {
                        if ((StrLen + 3UL) < SVC_STL_MUX_MAX_FILENAME_LEN) {
                            UINT8 FileWpY = StlMux.FileWp;
                            StlMux.FileName[StlMux.FileWp][StrLen - 3U] = 't';
                            StlMux.FileName[StlMux.FileWp][StrLen - 2U] = 'h';
                            StlMux.FileName[StlMux.FileWp][StrLen - 1U] = 'm';
                            StlMux.FileName[StlMux.FileWp][StrLen] = '.';
                            StlMux.FileName[StlMux.FileWp][StrLen + 1U] = 'y';
                            StlMux.FileName[StlMux.FileWp][StrLen + 2U] = '\0';
                            CreateFileProcess();
                            Temp = AmbaWrap_memcpy(StlMux.FileName[StlMux.FileWp], StlMux.FileName[FileWpY], StrLen);
                            if (Temp != OK) {
                                SvcLog_NG(SVC_LOG_STL_MUX, "[line %u] AmbaWrap_memcpy return 0x%x", __LINE__, Temp);
                            } else {
                                StlMux.FileName[StlMux.FileWp][StrLen - 3U] = 't';
                                StlMux.FileName[StlMux.FileWp][StrLen - 2U] = 'h';
                                StlMux.FileName[StlMux.FileWp][StrLen - 1U] = 'm';
                                StlMux.FileName[StlMux.FileWp][StrLen] = '.';
                                StlMux.FileName[StlMux.FileWp][StrLen + 1U] = 'u';
                                StlMux.FileName[StlMux.FileWp][StrLen + 2U] = 'v';
                                StlMux.FileName[StlMux.FileWp][StrLen + 3U] = '\0';
                            }
                        } else {
                            SvcLog_NG(SVC_LOG_STL_MUX, "StillMux_CreateFile StrLen + 3 = %u, max %u", (UINT32)StrLen + 3U, (UINT32)SVC_STL_MUX_MAX_FILENAME_LEN);
                        }
                    }
                    if ((pMsg->PicType == SVC_STL_TYPE_RAW) || (pMsg->PicType == SVC_STL_TYPE_RAWONLY)) {
                        StlMux.FileName[StlMux.FileWp][StrLen - 3U] = 'r';
                        StlMux.FileName[StlMux.FileWp][StrLen - 2U] = 'a';
                        StlMux.FileName[StlMux.FileWp][StrLen - 1U] = 'w';
                    }

                    if (pMsg->PicType == SVC_STL_TYPE_CLEAN_RAW) {
                        if ((StrLen + 6UL) < SVC_STL_MUX_MAX_FILENAME_LEN) {
                            StlMux.FileName[StlMux.FileWp][StrLen - 4U] = '_';
                            StlMux.FileName[StlMux.FileWp][StrLen - 3U] = 'c';
                            StlMux.FileName[StlMux.FileWp][StrLen - 2U] = 'l';
                            StlMux.FileName[StlMux.FileWp][StrLen - 1U] = 'e';
                            StlMux.FileName[StlMux.FileWp][StrLen     ] = 'a';
                            StlMux.FileName[StlMux.FileWp][StrLen + 1U] = 'n';
                            StlMux.FileName[StlMux.FileWp][StrLen + 2U] = '.';
                            StlMux.FileName[StlMux.FileWp][StrLen + 3U] = 'r';
                            StlMux.FileName[StlMux.FileWp][StrLen + 4U] = 'a';
                            StlMux.FileName[StlMux.FileWp][StrLen + 5U] = 'w';
                            StlMux.FileName[StlMux.FileWp][StrLen + 6U] = '\0';
                        } else {
                            SvcLog_NG(SVC_LOG_STL_MUX, "StillMux_CreateFile StrLen + 5 = %u, max %u", (UINT32)StrLen + 5U, (UINT32)SVC_STL_MUX_MAX_FILENAME_LEN);
                        }
                    }
                    if (pMsg->PicType == SVC_STL_TYPE_CE) {
                        StlMux.FileName[StlMux.FileWp][StrLen - 3U] = 'c';
                        StlMux.FileName[StlMux.FileWp][StrLen - 2U] = 'e';
                        StlMux.FileName[StlMux.FileWp][StrLen - 1U] = '\0';
                    }
                } else {
                    SvcStillLog("StillMux_CreateFile ERR: StrLen %p > SVC_STL_MUX_MAX_FILENAME_LEN %p", StrLen, SVC_STL_MUX_MAX_FILENAME_LEN, 0, 0, 0);
                }
            }
        }
    }
    if (OK != Rval) {
        SvcLog_NG(SVC_LOG_STL_MUX, "get file name failed. return 0x%x", Rval, 0);
    } else {
        if (pMsg->PicType != SVC_STL_TYPE_IDSP) {
            CreateFileProcess();
        }
    }
}

static void StillMux_CacheClean(void *pBuf, ULONG Size)
{
    if ((pBuf != NULL) && (Size > 0U)) {
#ifdef CONFIG_AMBA_VFS_GDMA_USED
        ULONG Addr;
        AmbaMisra_TypeCast(&Addr, &pBuf);
        if (0U != SvcPlat_CacheClean(Addr, Size)) {
            // SvcStillLog("[StillMux_CacheClean] proc SrcAddr %p cache invalid fail. if memory is non-cahce, ignore it.", SrcAddr, 0, 0, 0, 0);
        }
#endif
        AmbaMisra_TouchUnused(pBuf);
    }
}

static void StillMux_SavePictureDirectly(const SVC_STL_PIC_MSG_s *pMsg)
{
    UINT32 t1, t2, Rval, NumSuccess;
    ULONG BufStart, BufEnd, BufWp = StlMux.StillBufWp, *pBuf;
    WriteFromVideoBuf(pMsg->StartAddr, pMsg->PicSize, pMsg->PicType);
    BufStart = StlMux.StillBufStart;
    BufEnd = StlMux.StillBufEnd;
    Rval = AmbaKAL_GetSysTickCount(&t1);
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_STL_MUX, "AmbaKAL_GetSysTickCount failed! return 0x%x", Rval, 0U);
    }
    if (((BufWp + pMsg->PicSize) - 1U) > BufEnd) {
        ULONG Size = (BufEnd - BufWp) + 1U;
        AmbaMisra_TypeCast(&pBuf, &BufWp);
        StillMux_CacheClean(pBuf, Size);
        Rval = AmbaVFS_Write(pBuf, (UINT32)Size, 1U, &(StlMux.File[StlMux.FileRp]), &NumSuccess);
        if (Rval == SVC_OK) {
            AmbaMisra_TypeCast(&pBuf, &BufStart);
            StillMux_CacheClean(pBuf, (ULONG)pMsg->PicSize - Size);
            Rval = AmbaVFS_Write(pBuf, pMsg->PicSize - (UINT32)Size, 1U, &(StlMux.File[StlMux.FileRp]), &NumSuccess);
        }
    } else {
        AmbaMisra_TypeCast(&pBuf, &BufWp);
        StillMux_CacheClean(pBuf, (ULONG)pMsg->PicSize);
        Rval = AmbaVFS_Write(pBuf, pMsg->PicSize, 1U, &(StlMux.File[StlMux.FileRp]), &NumSuccess);
    }
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
    if (pMsg->PicType == SVC_STL_TYPE_LUMA) {
        StlMux.StillBufAvail += pMsg->PicSize;
        return;
    }
#endif
    if (Rval == SVC_OK) {
        Rval = AmbaVFS_Sync(&(StlMux.File[StlMux.FileRp]));
    }
    if (Rval == SVC_OK) {
        Rval = AmbaVFS_Close(&(StlMux.File[StlMux.FileRp]));
    }
    if (Rval == SVC_OK) {
        Rval = AmbaKAL_GetSysTickCount(&t2);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_STL_MUX, "AmbaKAL_GetSysTickCount failed! return 0x%x", Rval, 0U);
        }
        if (t2 >= t1) {
            SvcLog_OK(StlMux.FileName[StlMux.FileRp], "file closed, mux time %u ms", t2 - t1, 0);
        } else {
            SvcLog_OK(StlMux.FileName[StlMux.FileRp], "file closed, mux time %u ms", (0xFFFFFFFFU - t1) + t2, 0);
        }
        if (StlMux.FileRp < ((UINT8)SVC_STL_MUX_NUM_FILES - 1U)) {
            StlMux.FileRp ++;
        } else {
            StlMux.FileRp = 0;
        }
        StlMux.NumFile --;
        StlMux.StillBufAvail += pMsg->PicSize;
        if (StlMux_DebugOn == 1U) {
            SvcLog_DBG(SVC_LOG_STL_MUX, "StillBufAvail %u", StlMux.StillBufAvail, 0);
        }
    }
}

static void StillMux_SaveDataDirectly(const SVC_STL_PIC_MSG_s *pMsg)
{
    if (pMsg != NULL) {
        ULONG StrLen;
        char FileName[SVC_STL_MUX_MAX_FILENAME_LEN];

        AmbaSvcWrap_MisraMemset(FileName, 0, sizeof(FileName));
        if (StlMux.FileWp > 0U) {
            AmbaUtility_StringCopy(FileName, sizeof(FileName), StlMux.FileName[StlMux.FileWp - 1U]);
        } else {
            AmbaUtility_StringCopy(FileName, sizeof(FileName), StlMux.FileName[StlMux.FileWp]);
        }
        StrLen = AmbaUtility_StringLength(FileName);

        if (pMsg->CapType == SVC_STL_IDSP_ITUNER) {
            extern void SvcCmdIK_SaveItuner(UINT32 IkCtxID, char *pFileName);

            if (StrLen == 0U) {
                SVC_STG_MONI_DRIVE_INFO_s DriveInfo;
                AmbaSvcWrap_MisraMemset(&DriveInfo, 0, sizeof(DriveInfo));
                if (0U == SvcStgMonitor_GetDriveInfo(StlMux.StorageDrive, &DriveInfo)) {
                    FileName[0] = DriveInfo.Drive;
                } else {
                    FileName[0] = 'C';
                }
                AmbaUtility_StringAppend(&(FileName[1]), (UINT32)SVC_STL_MUX_MAX_FILENAME_LEN - 1U, ":\\still_itn.txt");
            } else {
                char *pChar = SvcWrap_strrchr(FileName, 45);        // ASCII '-'
                if (pChar != NULL) {
                    pChar[3] = '\0';
                }
                AmbaUtility_StringAppend(FileName, (UINT32)SVC_STL_MUX_MAX_FILENAME_LEN, "_itn.txt");
            }

            SvcCmdIK_SaveItuner(pMsg->StreamId, FileName);
        } else if (pMsg->CapType == SVC_STL_IDSP_IK_CR) {
            extern void SvcCmdIK_SaveIsoCfg(UINT32 IkCtxID, UINT32 PreCnt, const char *pFileName);

            if (StrLen == 0U) {
                SVC_STG_MONI_DRIVE_INFO_s DriveInfo;
                AmbaSvcWrap_MisraMemset(&DriveInfo, 0, sizeof(DriveInfo));
                if (0U == SvcStgMonitor_GetDriveInfo(StlMux.StorageDrive, &DriveInfo)) {
                    FileName[0] = DriveInfo.Drive;
                } else {
                    FileName[0] = 'C';
                }
                AmbaUtility_StringAppend(&(FileName[1]), (UINT32)SVC_STL_MUX_MAX_FILENAME_LEN - 1U, ":\\still_ik_cr.bin");
            } else {
                char *pChar = SvcWrap_strrchr(FileName, 45);        // ASCII '-'
                if (pChar != NULL) {
                    pChar[3] = '\0';
                }
                AmbaUtility_StringAppend(FileName, (UINT32)SVC_STL_MUX_MAX_FILENAME_LEN, "_ik_cr.bin");
            }

            SvcCmdIK_SaveIsoCfg(pMsg->StreamId, 0U, FileName);
        } else if (pMsg->CapType == SVC_STL_IDSP_UCODE_CR) {
            extern void SvcCmdIK_SaveIdspCfg(UINT32 ViewZoneID, UINT32 DumpLevel, UINT32 CtrlFlg, char *pDumpPath);

            if (StrLen == 0U) {
                SVC_STG_MONI_DRIVE_INFO_s DriveInfo;
                AmbaSvcWrap_MisraMemset(&DriveInfo, 0, sizeof(DriveInfo));
                if (0U == SvcStgMonitor_GetDriveInfo(StlMux.StorageDrive, &DriveInfo)) {
                    FileName[0] = DriveInfo.Drive;
                } else {
                    FileName[0] = 'C';
                }
                AmbaUtility_StringAppend(&(FileName[1]), (UINT32)SVC_STL_MUX_MAX_FILENAME_LEN - 1U, ":\\still_ucode_cr\\");
            } else {
                char *pChar = SvcWrap_strrchr(FileName, 45);        // ASCII '-'
                if (pChar != NULL) {
                    pChar[3] = '\0';
                }
                AmbaUtility_StringAppend(FileName, (UINT32)SVC_STL_MUX_MAX_FILENAME_LEN, "_ucode_cr\\");
            }

            /* Dump idsp cfg in r2y process. Does not need to stop dsp to dump idsp cfg */
            SvcCmdIK_SaveIdspCfg(pMsg->StreamId, 65535U, 1U, FileName);

        } else {
            // misra-c
        }
    }
}

static void* SvcStillMux_TaskEntry(void* EntryArg)
{
#if !defined(CONFIG_APP_FLOW_CARDV_AONI)
    static const char ModeName[]    = {CONFIG_SOC_NAME};
    static const char Maker[]     = {"Ambarella"};
    static const char Software[]  = {"080001"};
#endif

    SVC_STL_PIC_MSG_s Msg;
    UINT32 Rval;
    AMBA_EXIF_FMT_IMG_s ImageInfo = {0};
    ULONG ArgVal = 0U;

    AmbaMisra_TouchUnused(EntryArg);
    AmbaSvcWrap_MisraMemcpy(&ArgVal, EntryArg, sizeof(ULONG));

    while ( ArgVal != 0xCafeU ) {
        Rval = AmbaKAL_MsgQueueReceive(&StlMux.PicQue, &Msg, AMBA_KAL_WAIT_FOREVER);
        if (Rval == OK) {
            switch (Msg.Action) {
                case SVC_STL_MUX_CREATE_FILE:
                    StillMux_CreateFile(&Msg);
                    break;
                case SVC_STL_MUX_INPUT_PIC:
                    SvcLog_OK(SVC_LOG_STL_MUX, "Got pic, PicType %u picview %u", Msg.PicType, Msg.PicView);
                    SvcStillLog("[STL_MUX]          addr %p w %u h %u size 0x%x", Msg.StartAddr, Msg.Width, Msg.Height, Msg.PicSize, 0);
                    if (0U != SvcPlat_CacheInvalidate(Msg.StartAddr, (ULONG)Msg.PicSize)) {
                        // SvcStillLog("[SvcStillMux_TaskEntry] proc DstAddr %p cache invalid fail. if memory is non-cahce, ignore it.", DstAddr, 0, 0, 0, 0);
                    }
                    if (Msg.PicType == SVC_STL_TYPE_JPEG) {
                        if (Msg.PicView == SVC_STL_MAIN_VIEW) {
                            UINT8 *pBuf;
                            AmbaMisra_TypeCast(&pBuf, &StlMux.StillBufStart);
                            ImageInfo.BufferInfo.pBaseBuf     = pBuf;
                            AmbaMisra_TypeCast(&pBuf, &StlMux.StillBufEnd);
                            ImageInfo.BufferInfo.pLimitBuf    = pBuf;
                            AmbaMisra_TypeCast(&pBuf, &StlMux.StillBufWp);
                            ImageInfo.BufferInfo.pFullview    = pBuf;
                            ImageInfo.BufferInfo.FullviewSize = Msg.PicSize;
                            if (StlMux_DebugOn == 1U) {
                                SvcStillLog("[SvcStillMux_TaskEntry] pBaseBuf %p pLimitBuf %p pFullview %p", StlMux.StillBufStart, StlMux.StillBufEnd, StlMux.StillBufWp, 0, 0);
                            }
                            WriteFromVideoBuf(Msg.StartAddr, Msg.PicSize, Msg.PicType);
                            StlMux.ExifFormaterInfo.Width        = Msg.Width;
                            StlMux.ExifFormaterInfo.Height       = Msg.Height;
                            StlMux.PicSize[SVC_STL_MAIN_VIEW] = Msg.PicSize;
                        }
                        if(Msg.PicView == SVC_STL_SCRN_VIEW) {
                            UINT8 *pBuf;
                            AmbaMisra_TypeCast(&pBuf, &StlMux.StillBufWp);
                            ImageInfo.BufferInfo.pScreen      = pBuf;
                            ImageInfo.BufferInfo.ScreenSize   = Msg.PicSize;
                            if (StlMux_DebugOn == 1U) {
                                SvcStillLog("[SvcStillMux_TaskEntry] pScreen %p", StlMux.StillBufWp, 0, 0, 0, 0);
                            }
                            WriteFromVideoBuf(Msg.StartAddr, Msg.PicSize, Msg.PicType);
                            StlMux.ExifFormaterInfo.ScreenWidth  = Msg.Width;
                            StlMux.ExifFormaterInfo.ScreenHeight = Msg.Height;
                            StlMux.PicSize[SVC_STL_SCRN_VIEW] = Msg.PicSize;
                        }
                        if (Msg.PicView == SVC_STL_THMB_VIEW) {
                            UINT32 t1, t2;
                            UINT8 *pBuf;
                            ULONG StrLen;
                            AmbaMisra_TypeCast(&pBuf, &StlMux.StillBufWp);
                            ImageInfo.BufferInfo.pThumb      = pBuf;
                            ImageInfo.BufferInfo.ThumbSize   = Msg.PicSize;
                            if (StlMux_DebugOn == 1U) {
                                SvcStillLog("[SvcStillMux_TaskEntry] pThumb %p", StlMux.StillBufWp, 0, 0, 0, 0);
                            }
                            WriteFromVideoBuf(Msg.StartAddr, Msg.PicSize, Msg.PicType);
                            StlMux.ExifFormaterInfo.ThumbWidth   = Msg.Width;
                            StlMux.ExifFormaterInfo.ThumbHeight  = Msg.Height;
                            StlMux.ExifFormaterInfo.pMakernote = StlMux.Makernote;

                            SetExif(StlMux.ExifStreamId[StlMux.ExifRp], StlMux.ExifSensorIdx[StlMux.ExifRp], StlMux.ExifAebIdx[StlMux.ExifRp]);
                            if (StlMux.ExifRp < ((UINT8)SVC_STL_MUX_NUM_FILES - 1U)) {
                                StlMux.ExifRp ++;
                            } else {
                                StlMux.ExifRp = 0;
                            }
                            ImageInfo.pFormaterInfo = &(StlMux.ExifFormaterInfo);
                            StrLen = AmbaUtility_StringLength(StlMux.FileName[StlMux.FileRp]);
                            if (StrLen <= AMBA_EXIF_MAX_FILENAME_LENGTH) {
                                AmbaUtility_StringCopy(StlMux.ExifFormaterInfo.Name, (UINT32)StrLen, StlMux.FileName[StlMux.FileRp]);
                            } else {
                                SvcStillLog("SvcStillMux_TaskEntry ERR: file name length %p > AMBA_EXIF_MAX_FILENAME_LENGTH %p", StrLen, AMBA_EXIF_MAX_FILENAME_LENGTH, 0, 0, 0);
                            }
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
                            AmbaUtility_StringCopy(StlMux.ExifFormaterInfo.Model, AMBA_EXIF_TIFF_MODEL_SIZE, PROJECT_NAME);
                            AmbaUtility_StringCopy(StlMux.ExifFormaterInfo.Make, AMBA_EXIF_TIFF_MAKE_SIZE, CUSTOMER_NAME);
                            AmbaUtility_StringCopy(StlMux.ExifFormaterInfo.Software, AMBA_EXIF_TIFF_SOFTWARE_SIZE, app_helper.get_device_info()->sw_version);
#else
                            AmbaUtility_StringCopy(StlMux.ExifFormaterInfo.Model, sizeof(ModeName), ModeName);
                            AmbaUtility_StringCopy(StlMux.ExifFormaterInfo.Make, sizeof(Maker), Maker);
                            AmbaUtility_StringCopy(StlMux.ExifFormaterInfo.Software, sizeof(Software), Software);
#endif
#if defined(CONFIG_AMBA_VFS_GDMA_USED)
                            StillMux_CacheClean(ImageInfo.BufferInfo.pFullview, (ULONG)ImageInfo.BufferInfo.FullviewSize);
                            StillMux_CacheClean(ImageInfo.BufferInfo.pScreen, (ULONG)ImageInfo.BufferInfo.ScreenSize);
                            StillMux_CacheClean(ImageInfo.BufferInfo.pThumb, (ULONG)ImageInfo.BufferInfo.ThumbSize);
#endif


                            if (StlMux_DebugOn == 1U) {
                                SvcLog_DBG(SVC_LOG_STL_MUX, "mux start", 0, 0);
                            }
                            Rval = AmbaKAL_GetSysTickCount(&t1);
                            if (Rval != OK) {
                                SvcLog_NG(SVC_LOG_STL_MUX, "AmbaKAL_GetSysTickCount failed! return 0x%x", Rval, 0U);
                            }
                            Rval = AmbaExifFormatter(&ImageInfo, &(StlMux.File[StlMux.FileRp]));
                            if (Rval == SVC_OK) {
                                Rval = AmbaVFS_Sync(&(StlMux.File[StlMux.FileRp]));
                            }
                            if (Rval == SVC_OK) {
                                Rval = AmbaVFS_Close(&(StlMux.File[StlMux.FileRp]));
                            }
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
                            app_msg_queue_send(APP_MSG_ID_CAM_CAPTURE_PIV_DONE, 0, 0, 0);
#endif
                            if (Rval == SVC_OK) {
                                Rval = AmbaDCF_UpdateFile(StlMux.FileName[StlMux.FileRp]);
                                if (Rval != OK) {
                                    SvcLog_NG(SVC_LOG_STL_MUX, "[line %u] AmbaDCF_UpdateFile return 0x%x", __LINE__, Rval);
                                } else {
                                    Rval = AmbaKAL_GetSysTickCount(&t2);
                                    if (Rval != OK) {
                                        SvcLog_NG(SVC_LOG_STL_MUX, "AmbaKAL_GetSysTickCount failed! return 0x%x", Rval, 0U);
                                    } else {
                                        if (t2 >= t1) {
                                            SvcLog_OK(StlMux.FileName[StlMux.FileRp], "file close, mux time %u ms", t2 - t1, 0);
                                        } else {
                                            SvcLog_OK(StlMux.FileName[StlMux.FileRp], "file close, mux time %u ms", (0xFFFFFFFFU - t1) + t2, 0);
                                        }
                                        if (StlMux.FileRp < (SVC_STL_MUX_NUM_FILES - 1U)) {
                                            StlMux.FileRp ++;
                                        } else {
                                            StlMux.FileRp = 0;
                                        }
                                        StlMux.NumFile --;
                                        StlMux.StillBufAvail += (Msg.PicSize + StlMux.PicSize[SVC_STL_SCRN_VIEW] + StlMux.PicSize[SVC_STL_MAIN_VIEW]);
                                        if (StlMux_DebugOn == 1U) {
                                            SvcLog_DBG(SVC_LOG_STL_MUX, "StillBufAvail %u", StlMux.StillBufAvail, 0);
                                        }
                                    }
                                }
                            }
                        }
                    } else {
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
                        StillMux_SavePictureDirectly(&Msg);
                        if (Msg.PicType == SVC_STL_TYPE_THM) {
                            app_msg_queue_send(APP_MSG_ID_CAM_CAPTURE_THM_DONE, 0, 0, 0);
                        } else {
                            if (Msg.PicType != SVC_STL_TYPE_LUMA) {
                                app_msg_queue_send(APP_MSG_ID_CAM_CAPTURE_PIV_DONE, 0, 0, 0);
                            }
                        }
#else
                        StillMux_SavePictureDirectly(&Msg);
#endif

                    }
                    if (StlMux.NumFile == 0U) {
                        SvcStillCap_ClearStatus(SVC_STL_MUX_BUSY);
                    }
                    break;
                default:
                    /* no action */
                    break;
            }
        }

        AmbaMisra_TouchUnused(&ArgVal);
    }

    return NULL;
}

/**
* turn on/off debugging
* @param [in] On 0-off, 1-on
*/
void SvcStillMux_Debug(UINT8 On)
{
    StlMux_DebugOn = On;
    SvcLog_OK(SVC_LOG_STL_MUX, "SvcStillMux_Debug %u", On, 0);
}

/**
* initialize and create still muxer task
* @param [in] pCfg pointer to configuration of still muxer task
* @return 0-OK, 1-NG
*/
UINT32 SvcStillMux_Create(const SVC_STL_MUX_CONFIG_s *pCfg)
{
    UINT32                  Rval;
    static UINT8            SvcStillMux[SVC_STL_MUX_STACK_SIZE] GNU_SECTION_NOZEROINIT;
    static char             MsgQueName[] = "PicQue";
    static char             FlgName[] = "MuxStatus";

    if (StlMux_Created == 0U) {
        Rval = AmbaWrap_memset(&StlMux, 0x0, sizeof(StlMux));
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_STL_MUX, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
        } else {
            Rval = AmbaKAL_MsgQueueCreate(&StlMux.PicQue, MsgQueName, (UINT32)sizeof(SVC_STL_PIC_MSG_s), StlMux.PicQueBuf, (UINT32)sizeof(StlMux.PicQueBuf));
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_STL_MUX, "PicQue created failed! return 0x%x", Rval, 0U);
            }
        }
        if (Rval == SVC_OK) {
            Rval = AmbaKAL_EventFlagCreate(&StlMux.Status, FlgName);
            if (OK != Rval) {
                SvcLog_NG(SVC_LOG_STL_MUX, "Event flag create failed with 0x%x", Rval, 0U);
            } else {
                Rval = AmbaKAL_EventFlagSet(&StlMux.Status, MUX_AVAILABLE);
                if (OK != Rval) {
                    SvcLog_NG(SVC_LOG_STL_MUX, "Event flag set failed with 0x%x", Rval, 0U);
                }
            }
        }
        if (Rval == SVC_OK) {
            StlMux.StillBufStart = pCfg->StillBufAddr;
            StlMux.StillBufWp = StlMux.StillBufStart;
            StlMux.StillBufSize  = pCfg->StillBufSize;
            StlMux.StillBufAvail = pCfg->StillBufSize;
            SvcLog_DBG(SVC_LOG_STL_MUX, "StillBufAvail %u", StlMux.StillBufAvail, 0);
            StlMux.StillBufEnd   = pCfg->StillBufAddr + (ULONG)pCfg->StillBufSize - 1U;
            StlMux.VideoBufStart = pCfg->VideoBufAddr;
            StlMux.VideoBufSize  = pCfg->VideoBufSize;
            StlMux.VideoBufEnd   = pCfg->VideoBufAddr + (ULONG)pCfg->VideoBufSize - 1U;
            StlMux.YuvBufStart = pCfg->YuvBufAddr;
            StlMux.YuvBufSize  = pCfg->YuvBufSize;
            StlMux.YuvBufEnd   = pCfg->YuvBufAddr + (ULONG)pCfg->YuvBufSize - 1U;
            StlMux.RawBufStart = pCfg->RawBufAddr;
            StlMux.RawBufSize  = pCfg->RawBufSize;
            if ( 1U == SvcStillCap_GetCleanRawDump()) {
                StlMux.RawBufEnd   = pCfg->RawBufAddr + ((ULONG)(pCfg->RawBufSize) << 1U) - 1U;
            } else {
                StlMux.RawBufEnd   = pCfg->RawBufAddr + (ULONG)pCfg->RawBufSize - 1U;
            }
            AmbaMisra_TypeCast(&(StlMux.ExifFormaterInfo.HeaderBuffer), &(pCfg->ExifBufAddr));
            StlMux.ExifFormaterInfo.HeaderBufferSize  = pCfg->ExifBufSize;
            StlMux.StorageDrive = pCfg->StorageDrive;
            /* task create */
            StlMux.TaskCtrl.Priority    = pCfg->Priority;
            StlMux.TaskCtrl.EntryFunc   = SvcStillMux_TaskEntry;
            StlMux.TaskCtrl.EntryArg    = 0U;
            StlMux.TaskCtrl.pStackBase  = SvcStillMux;
            StlMux.TaskCtrl.StackSize   = SVC_STL_MUX_STACK_SIZE;
            StlMux.TaskCtrl.CpuBits     = pCfg->CpuBits;
            Rval = SvcTask_Create("SvcStillMux", &StlMux.TaskCtrl);
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_STL_MUX, "SvcStillMux_Task created failed! return 0x%x", Rval, 0U);
            } else {
                StlMux_Created = 1;
            }
        }
    } else {
        Rval = SVC_OK;
    }
    return Rval;
}

/**
* de-initialize and delete still muxer task
* @return 0-OK, 1-NG
*/
UINT32 SvcStillMux_Delete(void)
{
    UINT32 Rval;

    if (StlMux_Created == 1U) {
        Rval = SvcTask_Destroy(&StlMux.TaskCtrl);
        if (Rval != SVC_OK) {
            SvcLog_NG(SVC_LOG_STL_MUX, "SvcStillEnc_Task delete failed! return 0x%x", Rval, 0U);
        }
        if (Rval == SVC_OK) {
            Rval = AmbaKAL_EventFlagDelete(&StlMux.Status);
        }
        if (Rval == SVC_OK) {
            Rval = AmbaKAL_MsgQueueDelete(&StlMux.PicQue);
            if (Rval != SVC_OK) {
                SvcLog_NG(SVC_LOG_STL_MUX, "PicQue delete failed! return 0x%x", Rval, 0U);
            } else {
                StlMux_Created = 0;
            }
        }
    } else {
        Rval = SVC_OK;
    }
    return Rval;
}

