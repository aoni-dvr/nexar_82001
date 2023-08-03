/**
 * @file AmbaExifFormatter.c
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
 */
#include <AmbaWrap.h>
#include <AmbaMisraFix.h>
#include <AmbaUtility.h>
#include <AmbaKAL.h>

#include "format/SvcExif.h"
#include "format/SvcExifMux.h"
#include "AmbaExifFormatter.h"
#include "WrpFormatLib.h"

#define SVC_IFD_HEADER_BUFFER_SIZE  2048U  /**< Header buffer size */
#define SVC_IFD_INFO_BUFFER_SIZE    (2048U + AMBA_EXIF_DEVICE_SETTING_DECRIPTION_SIZE)  /**< Info buffer size */
#define SVC_EXIF_HEADER_BUFFER_SIZE (SVC_IFD_HEADER_BUFFER_SIZE + SVC_IFD_INFO_BUFFER_SIZE)     /**< Total header buffer size */

static inline void Exif_Perror(const char *Func, UINT32 Line, const char *Message)
{
    char LineStr[8];
    UINT32 Len = AmbaUtility_UInt32ToStr(LineStr, (UINT32)sizeof(LineStr), Line, 10);
    if (Len > 0U) {
        AmbaPrint_PrintStr5("[Error]%s:%s %s", Func, LineStr, Message, NULL, NULL);
    }
}

/**
 *  VFS stream handler
 */
typedef struct {
    SVC_STREAM_HDLR_s Hdlr;     /**< Stream handler */
    AMBA_VFS_FILE_s *VfsFile;   /**< VFS file pointer */
} WRP_VFS_STREAM_HDLR_s;

/**
 *  Image information
 */
typedef struct {
    SVC_FIFO_HDLR_s *BaseFifo;      /**< Base fifo handler */
    SVC_FIFO_HDLR_s *Fifo;          /**< Virtual fifo handler */
    SVC_MUX_FORMAT_HDLR_s *Hdlr;    /**< Muxing format handler */
    SVC_IMAGE_INFO_s Image;         /**< Image information */
    WRP_VFS_STREAM_HDLR_s Stream;   /**< VFS stream handler */
    AMBA_KAL_MUTEX_t Mutex;         /**< Mutex */
} WRP_IMAGE_INFO_s;


static WRP_IMAGE_INFO_s g_ImageMuxer;

static UINT32 VfsFileStream_Write(SVC_STREAM_HDLR_s *Hdlr, UINT32 Size, UINT8 *Buffer, UINT32 *Count)
{
    UINT32 Rval;
    const WRP_VFS_STREAM_HDLR_s *Stream;
    AmbaMisra_TypeCast(&Stream, &Hdlr);
    Rval = V2S(AmbaVFS_Write(Buffer, 1, Size, Stream->VfsFile, Count));
    if (Rval == OK) {
        if (*Count != Size) {
            Exif_Perror(__func__, __LINE__, "AmbaVFS_Write() failed!");
            Rval = STREAM_ERR_0002; //STREAM_ERR_IO_ERROR
        }
    } else {
        Exif_Perror(__func__, __LINE__, "AmbaVFS_Write() failed!");
    }
    AmbaMisra_TouchUnused(Hdlr);
    return Rval;
}

static UINT32 VfsFileStream_Seek(SVC_STREAM_HDLR_s *Hdlr, INT64 Pos, INT32 Orig)
{
    UINT32 Rval;
    const WRP_VFS_STREAM_HDLR_s *Stream;
    AmbaMisra_TypeCast(&Stream, &Hdlr);
    Rval = V2S(AmbaVFS_Seek(Stream->VfsFile, Pos, Orig));
    if (Rval != OK) {
        Exif_Perror(__func__, __LINE__, "AmbaVFS_Seek() failed!");
    }
    AmbaMisra_TouchUnused(Hdlr);
    return Rval;
}

static UINT32 VfsFileStream_GetPos(SVC_STREAM_HDLR_s *Hdlr, UINT64 *Pos)
{
    UINT32 Rval;
    const WRP_VFS_STREAM_HDLR_s *Stream;
    AmbaMisra_TypeCast(&Stream, &Hdlr);
    Rval = V2S(AmbaVFS_GetPos(Stream->VfsFile, Pos));
    if (Rval != OK) {
        Exif_Perror(__func__, __LINE__, "AmbaVFS_GetPos() failed!");
    }
    AmbaMisra_TouchUnused(Hdlr);
    return Rval;
}

static UINT32 VfsFileStream_GetLength(SVC_STREAM_HDLR_s *Hdlr, UINT64 *Size)
{
    AmbaMisra_TouchUnused(Hdlr);
    AmbaMisra_TouchUnused(Size);
    Exif_Perror(__func__, __LINE__, "Not supportted!");
    return STREAM_ERR_0001;
}

static UINT32 VfsFileStream_GetFreeSpace(SVC_STREAM_HDLR_s *Hdlr, UINT64 *Size)
{
    AmbaMisra_TouchUnused(Hdlr);
    AmbaMisra_TouchUnused(Size);
    Exif_Perror(__func__, __LINE__, "Not supportted!");
    return STREAM_ERR_0001;
}

static UINT32 VfsFileStream_Sync(SVC_STREAM_HDLR_s *Hdlr)
{
    AmbaMisra_TouchUnused(Hdlr);
    Exif_Perror(__func__, __LINE__, "Not supportted!");
    return STREAM_ERR_0001;
}

static UINT32 VfsFileStream_Func(SVC_STREAM_HDLR_s *Hdlr, UINT32 Cmd, UINT32 Param)
{
    AmbaMisra_TouchUnused(Hdlr);
    AmbaMisra_TouchUnused(&Cmd);
    AmbaMisra_TouchUnused(&Param);
    Exif_Perror(__func__, __LINE__, "Not supportted!");
    return STREAM_ERR_0001;
}

static UINT32 CreateMutex(void)
{
    static char MutexName[] = "SvcExifMux";
    UINT32 Rval = K2E(AmbaKAL_MutexCreate(&g_ImageMuxer.Mutex, MutexName));
    if (Rval != OK) {
        Exif_Perror(__func__, __LINE__, NULL);
    }
    return Rval;
}

static UINT32 CheckHeaderBuffer(const UINT8 *HeaderBuffer, UINT32 HeaderSize)
{
    UINT32 Rval = AMBA_EXIF_ERR_INVALID_ARG;
    if ((HeaderBuffer != NULL) && (HeaderSize >= SVC_EXIF_HEADER_BUFFER_SIZE)) {
        Rval = OK;
    }
    return Rval;
}

static void ExifFormatter_memcpy(void *Dst, const void *Src, UINT32 N, UINT32 *Rval)
{
    if (AmbaWrap_memcpy(Dst, Src, N) != OK) {
        *Rval = AMBA_EXIF_ERR_FATAL;
        AmbaPrint_PrintUInt5("[fifo] memcpy fail!", 0U, 0U, 0U, 0U, 0U);
    }
}

#define SVC_EXIF_INIT_BUFFER_SIZE    (4352U + SVC_EXIF_HEADER_BUFFER_SIZE)    /**< Initialization buffer size */
static UINT32 SvcExifFormatter_Init(UINT8 *HeaderBuffer, UINT32 HeaderSize)
{
    static UINT8 ExifBuffer[SVC_EXIF_INIT_BUFFER_SIZE] GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;
    static UINT8 IsInit = 0U;
    UINT32 Rval = OK;
    if (IsInit == 0U) {
        Rval = CheckHeaderBuffer(HeaderBuffer, HeaderSize);
        if (Rval == OK) {
            SVC_EXIF_MUX_INIT_CFG_s ExifInitCfg;
            Rval = WRAP2E(AmbaWrap_memset(&g_ImageMuxer, 0, sizeof(g_ImageMuxer)));  // State = SVC_FMT_STATE_INIT;
            if (Rval == OK) {
                /** Fragment MP4 Mux Init */
                Rval = M2W(SvcExifMux_GetInitDefaultCfg(&ExifInitCfg));
                if (Rval == OK) {
                    ExifInitCfg.MaxHdlr = 1U;
                    ExifInitCfg.HeaderBuffer = HeaderBuffer;
                    ExifInitCfg.HeaderBufferSize = HeaderSize;
                    Rval = M2W(SvcExifMux_GetInitBufferSize(ExifInitCfg.MaxHdlr, ExifInitCfg.HeaderBufferSize, &ExifInitCfg.BufferSize));
                    if (Rval == OK) {
                        AmbaPrint_PrintUInt5("ExifMux: %u / %u", ExifInitCfg.BufferSize, (UINT32)sizeof(ExifBuffer), 0U, 0, 0);
                        if (ExifInitCfg.BufferSize < (UINT32)sizeof(ExifBuffer)) {
                            ExifInitCfg.Buffer = ExifBuffer;
                            Rval = M2W(SvcExifMux_Init(&ExifInitCfg));
                            if (Rval == OK) {
                                Rval = CreateMutex();
                            } else {
                                Exif_Perror(__func__, __LINE__, NULL);
                            }
                        } else {
                            Exif_Perror(__func__, __LINE__, NULL);
                            Rval = AMBA_EXIF_ERR_FATAL;
                        }
                    } else {
                        Exif_Perror(__func__, __LINE__, NULL);
                    }
                } else {
                    Exif_Perror(__func__, __LINE__, NULL);
                }
            }
            IsInit = 1U;
        } else {
            Exif_Perror(__func__, __LINE__, NULL);
        }
    }
    return Rval;
}

static UINT32 InitImageInfo(SVC_IMAGE_INFO_s *Image, const char *Name, const char *Date)
{
    UINT32 Rval = WRAP2E(AmbaWrap_memset(Image, 0, sizeof(SVC_IMAGE_INFO_s)));
    if (Rval == OK) {
        Image->MediaInfo.MediaType = (UINT8)SVC_MEDIA_INFO_IMAGE;
        AmbaUtility_StringCopy(Image->MediaInfo.Name, AMBA_CFS_MAX_FILENAME_LENGTH, Name);
        Image->MediaInfo.Name[AMBA_CFS_MAX_FILENAME_LENGTH - 1U] = '\0';
        ExifFormatter_memcpy(Image->MediaInfo.Date, Date, SVC_FORMAT_MAX_DATE_SIZE, &Rval);
    }
    return Rval;
}

static UINT32 FifoCB(const SVC_FIFO_HDLR_s *Hdlr, UINT8 Event, SVC_FIFO_CALLBACK_INFO_s *Info)
{
    AmbaMisra_TouchUnused(&Hdlr);
    AmbaMisra_TouchUnused(&Event);
    AmbaMisra_TouchUnused(&Info);
    AmbaMisra_TouchUnused(&Info->Desc);
    return OK;
}

#define NUM_FIFO_ENTRIES (3U)   /**< Number of fifo entries */
#define FIFO_BUFFER_SIZE    (0x100U)    /**< Fifo buffer size */
static UINT32 CreateFifo(SVC_IMAGE_INFO_s *Image)
{
    static UINT8 FifoBuffer[2][FIFO_BUFFER_SIZE];
    SVC_FIFO_CFG_s FifoCfg;
    UINT32 Rval = F2E(SvcFIFO_GetDefaultCfg(&FifoCfg));
    if (Rval == OK) {
        Rval = F2E(SvcFIFO_GetHdlrBufferSize(NUM_FIFO_ENTRIES, &FifoCfg.BufferSize));
        if (Rval == OK) {
            if (FifoCfg.BufferSize <= FIFO_BUFFER_SIZE) {
                FifoCfg.NumEntries = NUM_FIFO_ENTRIES;
                FifoCfg.CbEvent = FifoCB;
                FifoCfg.BufferAddr = FifoBuffer[0];
                FifoCfg.BaseHdlr = NULL;
                Rval = F2E(SvcFIFO_Create(&FifoCfg, &g_ImageMuxer.BaseFifo));
                if (Rval == OK) {
                    FifoCfg.BaseHdlr = g_ImageMuxer.BaseFifo;
                    FifoCfg.BufferAddr = FifoBuffer[1];
                    Rval = F2E(SvcFIFO_Create(&FifoCfg, &g_ImageMuxer.Fifo));
                    if (Rval == OK) {
                        Image->Fifo = g_ImageMuxer.Fifo;
                        AmbaPrint_PrintUInt5("CreateFifo B/V %u/%u NumEntries %u Size%u",
                           g_ImageMuxer.BaseFifo->FifoId, g_ImageMuxer.Fifo->FifoId, NUM_FIFO_ENTRIES, FifoCfg.BufferSize, 0U);
                    } else {
                        Exif_Perror(__func__, __LINE__, NULL);
                    }
                } else {
                    Exif_Perror(__func__, __LINE__, NULL);
                }
            } else {
                Rval = AMBA_EXIF_ERR_FATAL;
                Exif_Perror(__func__, __LINE__, "DescBuffer not enough");
                AmbaPrint_PrintUInt5("NumEntries %u Size %u / %u", NUM_FIFO_ENTRIES, FifoCfg.BufferSize, FIFO_BUFFER_SIZE, 0U, 0U);
            }
        } else {
            Exif_Perror(__func__, __LINE__, NULL);
        }
    } else {
        Exif_Perror(__func__, __LINE__, NULL);
    }
    return Rval;
}

static UINT32 SetImageExifInfo(EXIF_INFO_s *ExifInfo, const AMBA_EXIF_AAA_INFO_s *PAAA, UINT8 *DeviceSetDescrip, UINT32 DeviceSetDescripSize)
{
    UINT32 Rval = OK;
    ExifInfo->ExposureTimeNum = PAAA->ExposureTimeNum;
    ExifInfo->ExposureTimeDen = PAAA->ExposureTimeDen;
    ExifInfo->FNumberNum = PAAA->FNumberNum;
    ExifInfo->FNumberDen = PAAA->FNumberDen;
    ExifInfo->ExposureProgram = PAAA->ExposureProgram;
    ExifInfo->FocalLengthNum = PAAA->FocalLengthNum;
    ExifInfo->FocalLengthDen = PAAA->FocalLengthDen;
    ExifInfo->SubjectDistanceNum = PAAA->SubjectDistanceNum;
    ExifInfo->SubjectDistanceDen = PAAA->SubjectDistanceDen;
    ExifInfo->CustomRendered = PAAA->CustomRendered;
    ExifInfo->ExposureMode = PAAA->ExposureMode;
    ExifInfo->WhiteBalance = PAAA->WhiteBalance;
    ExifInfo->FocalLength35mmFilm = PAAA->FocalLength35mmFilm;
    ExifInfo->SceneCaptureType = PAAA->SceneCaptureType;
    ExifInfo->Contrast = PAAA->Contrast;
    ExifInfo->Saturation = PAAA->Saturation;
    ExifInfo->Sharpness = PAAA->Sharpness;
    ExifInfo->SubjectDistanceRange = (UINT16)PAAA->SubjectDistanceRange;
    ExifInfo->ShutterSpeedNum = PAAA->ShutterSpeedNum;
    ExifInfo->ShutterSpeedDen = PAAA->ShutterSpeedDen;
    ExifInfo->ApertureValueNum = PAAA->ApertureValueNum;
    ExifInfo->ApertureValueDen = PAAA->ApertureValueDen;
    ExifInfo->ExposureBiasValueNum = PAAA->ExposureBiasValueNum;
    ExifInfo->ExposureBiasValueDen = PAAA->ExposureBiasValueDen;
    ExifInfo->MaxApertureValueNum = PAAA->MaxApertureValueNum;
    ExifInfo->MaxApertureValueDen = PAAA->MaxApertureValueDen;
    ExifInfo->DigitalZoomRatioNum = PAAA->DigitalZoomRatioNum;
    ExifInfo->DigitalZoomRatioDen = PAAA->DigitalZoomRatioDen;
    ExifInfo->MeteringMode = PAAA->MeteringMode;
    ExifInfo->LightSource = PAAA->LightSource;
    ExifFormatter_memcpy(ExifInfo->FlashPixVersion, PAAA->FlashpixVersion, (UINT32)sizeof(ExifInfo->FlashPixVersion), &Rval);
    ExifInfo->ColorSpace = PAAA->ColorSpace;
    ExifInfo->SensingMethod = PAAA->SensingMethod;
    ExifInfo->ExposureIndexNum = PAAA->ExposureIndexNum;
    ExifInfo->ExposureIndexDen = PAAA->ExposureIndexDen;
    ExifInfo->FileSource = PAAA->FileSource;
    ExifInfo->SceneType = PAAA->SceneType;
    ExifInfo->Flash = PAAA->Flash;
    ExifInfo->IsoSpeedRating = PAAA->PhotoGraphicSentivity;
    ExifInfo->PrivateImageData = DeviceSetDescrip;
    ExifInfo->PriImgDataSize = DeviceSetDescripSize;
    ExifInfo->GainControl = (UINT8)PAAA->GainControl;
    return Rval;
}

static UINT32 SetImageGpsInfo(SVC_EXIF_GPS_INFO_s *GpsInfo, const AMBA_EXIF_GPS_IFD_INFO_s *Cfg)
{
    UINT32 Rval = OK;
    ExifFormatter_memcpy(GpsInfo->VersionId, Cfg->VersionID, (UINT32)sizeof(GpsInfo->VersionId), &Rval);
    ExifFormatter_memcpy(GpsInfo->LatitudeRef, Cfg->LatitudeRef, (UINT32)sizeof(GpsInfo->LatitudeRef), &Rval);
    ExifFormatter_memcpy(GpsInfo->LatitudeNum, Cfg->LatitudeNum, (UINT32)sizeof(GpsInfo->LatitudeNum), &Rval);
    ExifFormatter_memcpy(GpsInfo->LatitudeDen, Cfg->LatitudeDen, (UINT32)sizeof(GpsInfo->LatitudeDen), &Rval);
    ExifFormatter_memcpy(GpsInfo->LongitudeRef, Cfg->LongitudeRef, (UINT32)sizeof(GpsInfo->LongitudeRef), &Rval);
    ExifFormatter_memcpy(GpsInfo->LongitudeNum, Cfg->LongitudeNum, (UINT32)sizeof(GpsInfo->LongitudeNum), &Rval);
    ExifFormatter_memcpy(GpsInfo->LongitudeDen, Cfg->LongitudeDen, (UINT32)sizeof(GpsInfo->LongitudeDen), &Rval);
    GpsInfo->AltitudeRef = Cfg->AltitudeRef;
    GpsInfo->AltitudeNum = Cfg->AltitudeNum;
    GpsInfo->AltitudeDen = Cfg->AltitudeDen;
    ExifFormatter_memcpy(GpsInfo->TimestampNum, Cfg->TimeStampNum, (UINT32)sizeof(GpsInfo->TimestampNum), &Rval);
    ExifFormatter_memcpy(GpsInfo->TimestampDen, Cfg->TimeStampDen, (UINT32)sizeof(GpsInfo->TimestampDen), &Rval);
    AmbaMisra_TypeCast(&GpsInfo->SatelliteOffset, &Cfg->pSatellite);
    GpsInfo->SatelliteCount = Cfg->SatelliteCount;
    ExifFormatter_memcpy(GpsInfo->Status, Cfg->Status, (UINT32)sizeof(GpsInfo->Status), &Rval);
    ExifFormatter_memcpy(GpsInfo->MeasureMode, Cfg->MeasureMode, (UINT32)sizeof(GpsInfo->MeasureMode), &Rval);
    GpsInfo->DopNum = Cfg->DOPNum;
    GpsInfo->DopDen = Cfg->DOPDen;
    ExifFormatter_memcpy(GpsInfo->SpeedRef, Cfg->SpeedRef, (UINT32)sizeof(GpsInfo->SpeedRef), &Rval);
    GpsInfo->SpeedNum = Cfg->SpeedNum;
    GpsInfo->SpeedDen = Cfg->SpeedDen;
    ExifFormatter_memcpy(GpsInfo->TrackRef, Cfg->TrackRef, (UINT32)sizeof(GpsInfo->TrackRef), &Rval);
    GpsInfo->TrackNum = Cfg->TrackNum;
    GpsInfo->TrackDen = Cfg->TrackDen;
    ExifFormatter_memcpy(GpsInfo->ImgdirectionRef, Cfg->ImgDirectionRef, (UINT32)sizeof(GpsInfo->ImgdirectionRef), &Rval);
    GpsInfo->ImgdirectionNum = Cfg->ImgDirectionNum;
    GpsInfo->ImgdirectionDen = Cfg->ImgDirectionDen;
    AmbaMisra_TypeCast(&GpsInfo->MapdatumOffset, &Cfg->pMapDatum);
    GpsInfo->MapdatumCount = Cfg->MapDatumCount;
    ExifFormatter_memcpy(GpsInfo->DestlatitudeRef, Cfg->DestLatitudeRef, (UINT32)sizeof(GpsInfo->DestlatitudeRef), &Rval);
    ExifFormatter_memcpy(GpsInfo->DestlatitudeNum, Cfg->DestLatitudeNum, (UINT32)sizeof(GpsInfo->DestlatitudeNum), &Rval);
    ExifFormatter_memcpy(GpsInfo->DestlatitudeDen, Cfg->DestLatitudeDen, (UINT32)sizeof(GpsInfo->DestlatitudeDen), &Rval);
    ExifFormatter_memcpy(GpsInfo->DestlongitudeRef, Cfg->DestLongitudeRef, (UINT32)sizeof(GpsInfo->DestlongitudeRef), &Rval);
    ExifFormatter_memcpy(GpsInfo->DestlongitudeNum, Cfg->DestLongitudeNum, (UINT32)sizeof(GpsInfo->DestlongitudeNum), &Rval);
    ExifFormatter_memcpy(GpsInfo->DestlongitudeDen, Cfg->DestLongitudeDen, (UINT32)sizeof(GpsInfo->DestlongitudeDen), &Rval);
    ExifFormatter_memcpy(GpsInfo->DestbearingRef, Cfg->DestBearingRef, (UINT32)sizeof(GpsInfo->DestbearingRef), &Rval);
    GpsInfo->DestbearingNum = Cfg->DestBearingNum;
    GpsInfo->DestbearingDen = Cfg->DestBearingDen;
    ExifFormatter_memcpy(GpsInfo->DestdistanceRef, Cfg->DestDistanceRef, (UINT32)sizeof(GpsInfo->DestdistanceRef), &Rval);
    GpsInfo->DestdistanceNum = Cfg->DestDistanceNum;
    GpsInfo->DestdistanceDen = Cfg->DestDistanceDen;
    AmbaMisra_TypeCast(&GpsInfo->ProcessingmethodOffset, &Cfg->pProcessingMethod);
    GpsInfo->ProcessingmethodCount = Cfg->ProcessingMethodCount;
    AmbaMisra_TypeCast(&GpsInfo->AreainformationOffset, &Cfg->pAreaInformation);
    GpsInfo->AreainformationCount = Cfg->AreaInformationCount;
    ExifFormatter_memcpy(GpsInfo->Datestamp, Cfg->DateStamp, (UINT32)sizeof(GpsInfo->Datestamp), &Rval);
    GpsInfo->Differential = Cfg->Differential;
    GpsInfo->HPositioningErrorNum = Cfg->HPositioningErrorNum;
    GpsInfo->HPositioningErrorDen = Cfg->HPositioningErrorDen;
    return Rval;
}

static UINT32 SetImageInfo(SVC_IMAGE_INFO_s *Image, const AMBA_EXIF_FMT_IMG_s *pImageInfo)
{
    UINT32 Rval;
#if (AMBA_EXIF_ENDIAN_TYPE == AMBA_EXIF_BIG_ENDIAN_MM)
    Image->Endian = 0U;
#else
    Image->Endian = 1U;
#endif
    Image->BufferBase = pImageInfo->BufferInfo.pBaseBuf;
    {
        /* Note: pLimitBuf = base + size -1, while BufferLimit = base + size */
        ULONG BaseUL, LimitUL, SizeUL;
        AmbaMisra_TypeCast(&BaseUL, &pImageInfo->BufferInfo.pBaseBuf);
        AmbaMisra_TypeCast(&LimitUL, &pImageInfo->BufferInfo.pLimitBuf);
        SizeUL = LimitUL - BaseUL + 1UL;
        Image->BufferSize = (UINT32)SizeUL;
    }
    Rval = SetImageExifInfo(&Image->ExifInfo.ExifIfdInfo, &pImageInfo->pFormaterInfo->PAAA, pImageInfo->pFormaterInfo->pDeviceSetDescrip, pImageInfo->pFormaterInfo->DeviceSetDescripSize);
    if (Rval == OK) {
        Rval = SetImageGpsInfo(&Image->ExifInfo.GpsIfdInfo, &pImageInfo->pFormaterInfo->GPSInfo);
    }
    if (Rval == OK) {
        if (pImageInfo->BufferInfo.FullviewSize != 0U) {
            SVC_IMAGE_FRAME_INFO_s *Frame = &Image->Frame[Image->FrameCount];
            Frame->Type = SVC_FIFO_TYPE_JPEG_FRAME;
            Frame->Width = (UINT16)pImageInfo->pFormaterInfo->Width;
            Frame->Height= (UINT16)pImageInfo->pFormaterInfo->Height;
            Image->FrameCount++;
        }
        if (pImageInfo->BufferInfo.ThumbSize != 0U) {
            SVC_IMAGE_FRAME_INFO_s *Frame = &Image->Frame[Image->FrameCount];
            Frame->Type = SVC_FIFO_TYPE_THUMBNAIL_FRAME;
            Frame->Width = (UINT16)pImageInfo->pFormaterInfo->ThumbWidth;
            Frame->Height= (UINT16)pImageInfo->pFormaterInfo->ThumbHeight;
            Image->FrameCount++;
        }
        if (pImageInfo->BufferInfo.ScreenSize != 0U) {
            SVC_IMAGE_FRAME_INFO_s *Frame = &Image->Frame[Image->FrameCount];
            Frame->Type = SVC_FIFO_TYPE_SCREENNAIL_FRAME;
            Frame->Width = (UINT16)pImageInfo->pFormaterInfo->ScreenWidth;
            Frame->Height= (UINT16)pImageInfo->pFormaterInfo->ScreenHeight;
            Image->FrameCount++;
        }
        Rval = CreateFifo(Image);
        if (Rval != OK) {
            Exif_Perror(__func__, __LINE__, "Invalid argument!");
        }
    } else {
        Exif_Perror(__func__, __LINE__, NULL);
    }
    return Rval;
}

static UINT32 SetIfd0TagCfg(SVC_CFG_TAG_INFO_s *TagCfg, const AMBA_EXIF_FORMATTER_INFO_s *FormaterInfo)
{
    UINT32 Rval = OK;
    UINT16 i;
    for(i = 0; i < TagCfg->Ifd0Tags; i++) {
        const char* CharPtr;
        switch (TagCfg->Ifd0[i].Tag) {
        case SVC_TIFF_ImageDescription:
            /* ASCII (1 byte) */
            /* The file name here is like X:\DCIM\100MEDIA\XXXX0001.JPG,
               and ImageDescription only needs this portion:"DCIM/100MEDIA" */
            TagCfg->Ifd0[i].Set = SVC_TAG_CONFIGURED;
            TagCfg->Ifd0[i].Count = (UINT32)AmbaUtility_StringLength(g_ImageMuxer.Image.MediaInfo.Name) - 3U + 1U;
            if (TagCfg->Ifd0[i].Count < 5U) {
                ExifFormatter_memcpy(&TagCfg->Ifd0[i].Value, &g_ImageMuxer.Image.MediaInfo.Name[3U], TagCfg->Ifd0[i].Count, &Rval);
            } else {
                //TagCfg.Ifd0[i].Data = &g_ImageMuxer.Image.MediaInfo.Name[3];
                CharPtr = &g_ImageMuxer.Image.MediaInfo.Name[3];
                AmbaMisra_TypeCast(&TagCfg->Ifd0[i].Data, &CharPtr);
            }
            break;
        case SVC_TIFF_Make:
            TagCfg->Ifd0[i].Set = SVC_TAG_CONFIGURED;
            TagCfg->Ifd0[i].Count = AMBA_EXIF_TIFF_MAKE_SIZE;
            //TagCfg.Ifd0[i].Data = FormaterInfo->Make;
            CharPtr = FormaterInfo->Make;
            AmbaMisra_TypeCast(&TagCfg->Ifd0[i].Data, &CharPtr);
            break;
        case SVC_TIFF_Model:
            TagCfg->Ifd0[i].Set = SVC_TAG_CONFIGURED;
            TagCfg->Ifd0[i].Count = AMBA_EXIF_TIFF_MODEL_SIZE;
            //TagCfg.Ifd0[i].Data = FormaterInfo->Model;
            CharPtr = FormaterInfo->Model;
            AmbaMisra_TypeCast(&TagCfg->Ifd0[i].Data, &CharPtr);
            break;
        case SVC_TIFF_Software:
            TagCfg->Ifd0[i].Set = SVC_TAG_CONFIGURED;
            TagCfg->Ifd0[i].Count = AMBA_EXIF_TIFF_SOFTWARE_SIZE;
            //TagCfg.Ifd0[i].Data = FormaterInfo->Software;
            CharPtr = FormaterInfo->Software;
            AmbaMisra_TypeCast(&TagCfg->Ifd0[i].Data, &CharPtr);
            break;
        case SVC_TIFF_GPSInfoIFDPointer:
            if (FormaterInfo->WithGPSInfo == 1U) {
                TagCfg->Ifd0[i].Set = SVC_TAG_ENABLED;
            }
            break;
        default:
            // default
            break;
        }
        if (Rval != OK) {
            break;
        }
    }
    return Rval;
}

static UINT32 SetExifIfdTagCfg(SVC_CFG_TAG_INFO_s *TagCfg, const AMBA_EXIF_FORMATTER_INFO_s *FormaterInfo)
{
    UINT32 Rval = OK;
    UINT16 i;
    for(i = 0; i < TagCfg->ExifIfdTags; i++) {
        switch (TagCfg->ExifIfd[i].Tag) {
        case SVC_EXIFIFD_DateTimeOriginal:
            {
                static UINT8 DateTimeOriginal[AMBA_EXIF_TIFF_DATETIME_SIZE];
                TagCfg->ExifIfd[i].Set = SVC_TAG_CONFIGURED;
                TagCfg->ExifIfd[i].Count = AMBA_EXIF_TIFF_DATETIME_SIZE;
                ExifFormatter_memcpy(DateTimeOriginal, FormaterInfo->PAAA.DateTimeOriginal, AMBA_EXIF_TIFF_DATETIME_SIZE, &Rval);
                TagCfg->ExifIfd[i].Data = DateTimeOriginal;
            }
            break;
        case SVC_EXIFIFD_DateTimeDigitized:
            {
                static UINT8 DateTimeDigitized[AMBA_EXIF_TIFF_DATETIME_SIZE];
                TagCfg->ExifIfd[i].Set = SVC_TAG_CONFIGURED;
                TagCfg->ExifIfd[i].Count = AMBA_EXIF_TIFF_DATETIME_SIZE;
                ExifFormatter_memcpy(DateTimeDigitized, FormaterInfo->PAAA.DateTimeDigitized, AMBA_EXIF_TIFF_DATETIME_SIZE, &Rval);
                TagCfg->ExifIfd[i].Data = DateTimeDigitized;
            }
            break;
        case SVC_EXIFIFD_MakerNote:
            TagCfg->ExifIfd[i].Set = SVC_TAG_CONFIGURED;
            TagCfg->ExifIfd[i].Count = AMBA_EXIF_MAKERNOTE_SIZE;
            TagCfg->ExifIfd[i].Data = FormaterInfo->pMakernote;
            break;
        case SVC_EXIFIFD_ISOSpeed:
            TagCfg->ExifIfd[i].Set = SVC_TAG_DISABLED;
            break;
        case SVC_EXIFIFD_SensitivityType:
            TagCfg->ExifIfd[i].Set = SVC_TAG_DISABLED;
            break;
        default:
            // default
            break;
        }
        if (Rval != OK) {
            break;
        }
    }
    return Rval;
}

static void SetGpsTagCfg(SVC_CFG_TAG_INFO_s *TagCfg, const AMBA_EXIF_GPS_IFD_INFO_s *GPSInfo)
{
    UINT16 i;
    for(i = 0; i < TagCfg->GpsIfdTags; i++) {
        switch (TagCfg->GpsIfd[i].Tag) {
        case SVC_GPSIFD_GPSSatellites:
            TagCfg->GpsIfd[i].Set = (GPSInfo->SatelliteCount == 0U)? SVC_TAG_DISABLED : SVC_TAG_ENABLED;
            TagCfg->GpsIfd[i].Count = GPSInfo->SatelliteCount;
            break;
        case SVC_GPSIFD_GPSMapDatum:
            TagCfg->GpsIfd[i].Set = (GPSInfo->MapDatumCount == 0U)? SVC_TAG_DISABLED : SVC_TAG_ENABLED;
            TagCfg->GpsIfd[i].Count = GPSInfo->MapDatumCount;
            break;
        case SVC_GPSIFD_GPSProcessingMethod:
            TagCfg->GpsIfd[i].Set = (GPSInfo->ProcessingMethodCount == 0U)? SVC_TAG_DISABLED : SVC_TAG_ENABLED;
            TagCfg->GpsIfd[i].Count = GPSInfo->ProcessingMethodCount;
            break;
        case SVC_GPSIFD_GPSAreaInformation:
            TagCfg->GpsIfd[i].Set = (GPSInfo->AreaInformationCount == 0U)? SVC_TAG_DISABLED : SVC_TAG_ENABLED;
            TagCfg->GpsIfd[i].Count = GPSInfo->AreaInformationCount;
            break;
        default:
            TagCfg->GpsIfd[i].Set = SVC_TAG_ENABLED;
            break;
        }
    }
}

static UINT32 WriteFifo(const AMBA_EXIF_FORMATTER_BUFFER_INFO_s *BufferInfo)
{
    UINT32 Rval;
    SVC_FIFO_BITS_DESC_s FifoDesc = {0};
    FifoDesc.FrameType = SVC_FIFO_TYPE_JPEG_FRAME;
    FifoDesc.StartAddr = BufferInfo->pFullview;
    FifoDesc.Size= BufferInfo->FullviewSize;
    FifoDesc.Completed = 1U;
    Rval = F2E(SvcFIFO_WriteEntry(g_ImageMuxer.BaseFifo, &FifoDesc, 0U));
    if (Rval == OK) {
        FifoDesc.FrameType = SVC_FIFO_TYPE_THUMBNAIL_FRAME;
        FifoDesc.StartAddr = BufferInfo->pThumb;
        FifoDesc.Size= BufferInfo->ThumbSize;
        Rval = F2E(SvcFIFO_WriteEntry(g_ImageMuxer.BaseFifo, &FifoDesc, 0U));
        if (Rval == OK) {
            FifoDesc.FrameType = SVC_FIFO_TYPE_SCREENNAIL_FRAME;
            FifoDesc.StartAddr = BufferInfo->pScreen;
            FifoDesc.Size= BufferInfo->ScreenSize;
            Rval = F2E(SvcFIFO_WriteEntry(g_ImageMuxer.BaseFifo, &FifoDesc, 0U));
            if (Rval != OK) {
                Exif_Perror(__func__, __LINE__, NULL);
            }
        } else {
            Exif_Perror(__func__, __LINE__, NULL);
        }
    } else {
        Exif_Perror(__func__, __LINE__, NULL);
    }
    return Rval;
}

static UINT32 SvcExifFormatter_Create(const AMBA_EXIF_FMT_IMG_s *pImageInfo, AMBA_VFS_FILE_s  *pFile)
{
    SVC_EXIF_MUX_CFG_s ExifCfg;
    UINT32 Rval = M2E(SvcExifMux_GetDefaultCfg(&ExifCfg));
    if (Rval == OK) {
        static SVC_STREAM_s g_VfsFileStream = {
            .Open = NULL,
            .Close = NULL,
            .Read = NULL,
            .Write = VfsFileStream_Write,
            .Seek = VfsFileStream_Seek,
            .GetPos = VfsFileStream_GetPos,
            .GetLength = VfsFileStream_GetLength,
            .GetFreeSpace = VfsFileStream_GetFreeSpace,
            .Sync = VfsFileStream_Sync,
            .Func = VfsFileStream_Func
        };
        // Set Stream Handler
        g_ImageMuxer.Stream.VfsFile = pFile;
        g_ImageMuxer.Stream.Hdlr.Func = &g_VfsFileStream;
        ExifCfg.Stream = &g_ImageMuxer.Stream.Hdlr;
        // Set Tag Configs
        Rval = SetIfd0TagCfg(&ExifCfg.SetTagInfo, pImageInfo->pFormaterInfo);
        if (Rval == OK) {
            Rval = SetExifIfdTagCfg(&ExifCfg.SetTagInfo, pImageInfo->pFormaterInfo);
            if (Rval == OK) {
                if (pImageInfo->pFormaterInfo->WithGPSInfo == 1U) {
                    SetGpsTagCfg(&ExifCfg.SetTagInfo, &pImageInfo->pFormaterInfo->GPSInfo);
                }
                Rval = M2E(SvcExifMux_Create(&ExifCfg, &g_ImageMuxer.Hdlr));
                if (Rval == OK) {
                    g_ImageMuxer.Hdlr->Media = &g_ImageMuxer.Image.MediaInfo;
                } else {
                    Exif_Perror(__func__, __LINE__, NULL);
                }
            }
        } else {
            Exif_Perror(__func__, __LINE__, NULL);
        }
    } else {
        Exif_Perror(__func__, __LINE__, NULL);
    }
    return Rval;
}

static UINT32 GetHeaderSize_ParamCheck(const UINT32 *HeaderSize)
{
    UINT32 Rval = OK;
    if (HeaderSize == NULL) {
        Rval = AMBA_EXIF_ERR_INVALID_ARG;
    }
    return Rval;
}

/**
 * EXIF API for geting the size of the header buffer
 * @param [out] HeaderSize The returned header buffer size
 * @return 0 - OK, others - AMBA_EXIF_ERR_XXXX
 */
UINT32 AmbaExifFormatter_GetHeaderSize(UINT32 *HeaderSize)
{
    UINT32 Rval = GetHeaderSize_ParamCheck(HeaderSize);
    *HeaderSize = SVC_EXIF_HEADER_BUFFER_SIZE;
    return Rval;
}

static UINT32 SvcExifFormatter_ParamCheck(const AMBA_EXIF_FMT_IMG_s *pImageInfo, const AMBA_VFS_FILE_s  *pFile)
{
    UINT32 Rval;
    if ((pImageInfo == NULL) || (pFile == NULL)) {
        Rval = AMBA_EXIF_ERR_INVALID_ARG;
        Exif_Perror(__func__, __LINE__, "Invalid argument!");
    } else {
        Rval = OK;
    }
    return Rval;
}

/**
 * EXIF API for still capture
 * @param [in] pImageInfo Pointer to EXIF sturcture
 * @param [in] pFile Pointer to VFS file
 * @return 0 - OK, others - AMBA_EXIF_ERR_XXXX
 */
UINT32 AmbaExifFormatter(const AMBA_EXIF_FMT_IMG_s *pImageInfo, AMBA_VFS_FILE_s  *pFile)
{
    UINT32 Rval = SvcExifFormatter_ParamCheck(pImageInfo, pFile);
    if (Rval == OK) {
        Rval = SvcExifFormatter_Init(pImageInfo->pFormaterInfo->HeaderBuffer, pImageInfo->pFormaterInfo->HeaderBufferSize);
        if (Rval == OK) {
            Rval = K2E(AmbaKAL_MutexTake(&g_ImageMuxer.Mutex, WRP_FORMAT_WAIT_TIMEOUT));
            if (Rval != OK) {
                Exif_Perror(__func__, __LINE__, NULL);
            }
        }
    }
    if (Rval == OK) {
        const AMBA_EXIF_FORMATTER_INFO_s *FormaterInfo = pImageInfo->pFormaterInfo;
        Rval = InitImageInfo(&g_ImageMuxer.Image, FormaterInfo->Name, FormaterInfo->Date);
        if (Rval == OK) {
            Rval = SetImageInfo(&g_ImageMuxer.Image, pImageInfo);
            if (Rval == OK) {
                Rval = SvcExifFormatter_Create(pImageInfo, pFile);
                if (Rval == OK) {
                    Rval = WriteFifo(&pImageInfo->BufferInfo);
                }
            }
        }
    }
    if (Rval == OK) {
        SVC_MUX_FORMAT_HDLR_s *Format = g_ImageMuxer.Hdlr;
        Rval = M2E(Format->Func->Open(Format));
        if (Rval == OK) {
            UINT8 Event = SVC_MUXER_EVENT_NONE;
            Rval = M2E(Format->Func->Process(Format, 0ULL/*dummy*/, &Event));
            if (Rval == OK) {
                Rval = M2E(Format->Func->Close(Format, 0U/*dummy*/));
                if (Rval != OK) {
                    Exif_Perror(__func__, __LINE__, NULL);
                }
            } else {
                Exif_Perror(__func__, __LINE__, NULL);
            }
        }
    }
    if (Rval == OK) {
        Rval = M2E(SvcExifMux_Delete(g_ImageMuxer.Hdlr));
        if (Rval == OK) {
            Rval = F2E(SvcFIFO_Reset(g_ImageMuxer.Fifo));
            if (Rval != OK) {
                Exif_Perror(__func__, __LINE__, NULL);
            }
            Rval = F2E(SvcFIFO_Delete(g_ImageMuxer.Fifo));
            if (Rval == OK) {
                Rval = F2E(SvcFIFO_Reset(g_ImageMuxer.BaseFifo));
                if (Rval != OK) {
                    Exif_Perror(__func__, __LINE__, NULL);
                }
                Rval = F2E(SvcFIFO_Delete(g_ImageMuxer.BaseFifo));
                if (Rval != OK) {
                    Exif_Perror(__func__, __LINE__, NULL);
                }
            } else {
                Exif_Perror(__func__, __LINE__, NULL);
            }
        } else {
            Exif_Perror(__func__, __LINE__, NULL);
        }
    }
    if ((Rval == OK) || (Rval == AMBA_EXIF_ERR_IO_ERROR)) {
        Rval = K2E(AmbaKAL_MutexGive(&g_ImageMuxer.Mutex));
        if (Rval != OK) {
            Exif_Perror(__func__, __LINE__, NULL);
        }
    }
    return Rval;
}

