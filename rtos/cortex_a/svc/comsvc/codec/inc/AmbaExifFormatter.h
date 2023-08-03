/**
 * @file AmbaExifFormatter.h
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

#ifndef AMBA_EXIF_FORMATTER_H
#define AMBA_EXIF_FORMATTER_H

#include "AmbaVfs.h"

#define AMBA_EXIF_ERR_INVALID_ARG      (0x00000001U) /**< Invalid argument */
#define AMBA_EXIF_ERR_FATAL            (0x00000002U) /**< Fatal error */
#define AMBA_EXIF_ERR_IO_ERROR         (0x00000003U) /**< I/O error */

/* Set endian type is
   AMBA_EXIF_LITTLE_ENDIAN_II or
   AMBA_EXIF_BIG_ENDIAN_MM */
#define AMBA_EXIF_ENDIAN_TYPE                           AMBA_EXIF_LITTLE_ENDIAN_II    /**< Endian type */

/* Exif ACSII keywords */
#define AMBA_EXIF_BIG_ENDIAN_MM                         0x4D4DU  /**< Big endian */
#define AMBA_EXIF_LITTLE_ENDIAN_II                      0x4949U  /**< Little endian */

/* Exif version keyword */
#define AMBA_EXIF_VERSION                               0x30323231U  /**< Exif version */
#define AMBA_EXIF_TIFF_VERSION                          0x002AU      /**< Tiff version */
#define AMBA_EXIF_R98                                   0x52393800U  /**< R98 */
#define AMBA_EXIF_R98_VERSION                           0x30313030U  /**< Version 1.00 */

/* Tiff Tag Data Size */
#define AMBA_EXIF_MAKERNOTE_SIZE                        256U         /**< Makernote size */
#define AMBA_EXIF_COMPNUM                               0x01020300U  /**< EXIF_IFD_ComponentsConfiguration */
#define AMBA_EXIF_MAX_FILENAME_LENGTH                   48UL         /**< Max filename length */
#define AMBA_EXIF_DEVICE_SETTING_DECRIPTION_SIZE        40000U       /**< Optional: Can be zero */

/* Tiff Tag Data Size */
#define AMBA_EXIF_TIFF_MAKE_SIZE                        30U          /**< TIFF Make Size */
#define AMBA_EXIF_TIFF_MODEL_SIZE                       30U          /**< TIFF Model Size */
#define AMBA_EXIF_TIFF_DATETIME_SIZE                    20U          /**< TIFF DateTime Size */
#define AMBA_EXIF_TIFF_IMG_DESC_SIZE                    24U          /**< TIFF ImgDesc Size */
#define AMBA_EXIF_TIFF_SOFTWARE_SIZE                    24U          /**< TIFF Software Size */

/* To distinguish JPG bitstreams */
/**
 *  Exif Picture type
 */
typedef enum {
    AMBA_EXIF_THUMBNAIL = 0, /**< Thumbnail */
    AMBA_EXIF_FULLVIEW,      /**< Fullview */
    AMBA_EXIF_SCREENNAIL,    /**< Screennail */
    AMBA_EXIF_MAX_STREAM     /**< Max */
} AMBA_EXIF_PIC_TYPE_e;

/**
 *  EXIF Information
 */
typedef struct {
    UINT32                          ExposureTimeNum;                        /**< Numerator of ExposureTime */
    UINT32                          ExposureTimeDen;                        /**< Denominator of ExposureTime */
    UINT32                          FNumberNum;                             /**< Numerator of FNumber */
    UINT32                          FNumberDen;                             /**< Denominator of FNumber */
    UINT32                          ExposureProgram;                        /**< ExposureProgram */
    UINT16                          PhotoGraphicSentivity;                  /**< It was named IsoSpeedRating in old version */
    UINT8                           DateTimeOriginal[AMBA_EXIF_TIFF_DATETIME_SIZE];  /**< DateTimeOriginal */
    UINT8                           DateTimeDigitized[AMBA_EXIF_TIFF_DATETIME_SIZE]; /**< DateTimeDigitized */
    UINT32                          ShutterSpeedNum;                        /**< Numerator of ShutterSpeed */
    UINT32                          ShutterSpeedDen;                        /**< Denominator of ShutterSpeed */
    UINT32                          ApertureValueNum;                       /**< Numerator of ApertureValue */
    UINT32                          ApertureValueDen;                       /**< Denominator of ApertureValue */
    UINT32                          ExposureBiasValueNum;                   /**< Numerator of ExposureBiasValue */
    UINT32                          ExposureBiasValueDen;                   /**< Denominator of ExposureBiasValue */
    UINT32                          MaxApertureValueNum;                    /**< Numerator of MaxApertureValue */
    UINT32                          MaxApertureValueDen;                    /**< Denominator of MaxApertureValue */
    UINT32                          SubjectDistanceRange;                   /**< SubjectDistanceRange */
    UINT32                          SubjectDistanceNum;                     /**< Numerator of SubjectDistance */
    UINT32                          SubjectDistanceDen;                     /**< Denominator of SubjectDistance */
    UINT16                          MeteringMode;                           /**< MeteringMode */
    UINT16                          LightSource;                            /**< LightSource */
    UINT16                          Flash;                                  /**< Flash */
    UINT32                          FocalLengthNum;                         /**< Numerator of FocalLength */
    UINT32                          FocalLengthDen;                         /**< Denominator of FocalLength */
    UINT8                           FlashpixVersion[4];                     /**< FlashpixVersion */
    UINT16                          ColorSpace;                             /**< ColorSpace */
    UINT32                          ExposureIndexNum;                       /**< Numerator of ExposureIndex */
    UINT32                          ExposureIndexDen;                       /**< Denominator of ExposureIndex */
    UINT16                          SensingMethod;                          /**< SensingMethod */
    UINT16                          FileSource;                             /**< FileSource */
    UINT16                          SceneType;                              /**< SceneType */
    UINT16                          CustomRendered;                         /**< CustomRendered */
    UINT16                          ExposureMode;                           /**< ExposureMode */
    UINT16                          WhiteBalance;                           /**< WhiteBalance */
    UINT32                          DigitalZoomRatioNum;                    /**< Numerator of DigitalZoomRatio */
    UINT32                          DigitalZoomRatioDen;                    /**< Denominator of DigitalZoomRatio */
    UINT16                          FocalLength35mmFilm;                    /**< FocalLength35mmFilm */
    UINT16                          SceneCaptureType;                       /**< SceneCaptureType */
    UINT16                          GainControl;                            /**< GainControl */
    UINT16                          Contrast;                               /**< Contrast */
    UINT16                          Saturation;                             /**< Saturation */
    UINT16                          Sharpness;                              /**< Sharpness */
} AMBA_EXIF_AAA_INFO_s;

/**
 *  GPS Information
 */
typedef struct {
    UINT8                           VersionID[4];                           /**< Version ID */
    char                            LatitudeRef[2];                         /**< 'N' = North, 'S' = South */
    UINT32                          LatitudeNum[3];                         /**< Numerator of Latitude */
    UINT32                          LatitudeDen[3];                         /**< Denominator of Latitude */
    char                            LongitudeRef[2];                        /**< 'E' = East, 'W' = West */
    UINT32                          LongitudeNum[3];                        /**< Numerator of Longitude */
    UINT32                          LongitudeDen[3];                        /**< Denominator of Longitude */
    UINT8                           AltitudeRef;                            /**< 0 = Above Sea Level, 1 = Below Sea Level */
    UINT32                          AltitudeNum;                            /**< Numerator of Longitude */
    UINT32                          AltitudeDen;                            /**< Denominator of Longitude */
    UINT32                          TimeStampNum[3];                        /**< Numerator of TimeStamp as UTC format */
    UINT32                          TimeStampDen[3];                        /**< Denominator of TimeStamp as UTC format */
    UINT8                          *pSatellite;                             /**< The GPS satellites used for measurements */
    UINT32                          SatelliteCount;                         /**< Size of the GPS Satellite. 0 = disable the tag */
    char                            Status[2];                              /**< 'A' = Measurement Active, 'V' = Measurement Void */
    char                            MeasureMode[2];                         /**< 2 = 2-Dimensional Measurement, 3 = 3-Dimensional Measurement */
    UINT32                          DOPNum;                                 /**< Numerator of DOP */
    UINT32                          DOPDen;                                 /**< Denominator of DOP */
    char                            SpeedRef[2];                            /**< 'K' = km/h, 'M' = mph, 'N' = knots */
    UINT32                          SpeedNum;                               /**< Numerator of Speed */
    UINT32                          SpeedDen;                               /**< Denominator of Speed */
    char                            TrackRef[2];                            /**< 'M' = Magnetic North, 'T' = True North */
    UINT32                          TrackNum;                               /**< Numerator of Track */
    UINT32                          TrackDen;                               /**< Denominator of Track */
    char                            ImgDirectionRef[2];                     /**< 'M' = Magnetic North, 'T' = True North */
    UINT32                          ImgDirectionNum;                        /**< Numerator of ImgDirection */
    UINT32                          ImgDirectionDen;                        /**< Denominator of ImgDirection */
    UINT8                          *pMapDatum;                              /**< The geodetic survey data used by the GPS receiver */
    UINT32                          MapDatumCount;                          /**< Size of the MapDatum. 0 = disable the tag */
    char                            DestLatitudeRef[2];                     /**< 'N' = North, 'S' = South */
    UINT32                          DestLatitudeNum[3];                     /**< Numerator of DestLatitude */
    UINT32                          DestLatitudeDen[3];                     /**< Denominator of DestLatitude */
    char                            DestLongitudeRef[2];                    /**< 'E' = East, 'W' = West */
    UINT32                          DestLongitudeNum[3];                    /**< Numerator of DestLongitude */
    UINT32                          DestLongitudeDen[3];                    /**< Denominator of DestLongitude */
    char                            DestBearingRef[2];                      /**< 'M' = Magnetic North, 'T' = True North */
    UINT32                          DestBearingNum;                         /**< Numerator of DestBearing */
    UINT32                          DestBearingDen;                         /**< Denominator of DestBearing */
    char                            DestDistanceRef[2];                     /**< 'K' = Kilometers, 'M' = Miles, 'N' = Nautical Miles */
    UINT32                          DestDistanceNum;                        /**< Numerator of DestDistance */
    UINT32                          DestDistanceDen;                        /**< Denominator of DestDistance */
    UINT8                          *pProcessingMethod;                      /**< Values of "GPS", "CELLID", "WLAN" or "MANUAL" by the EXIF spec. */
    UINT32                          ProcessingMethodCount;                  /**< Size of the ProcessingMethod. 0 = disable the tag */
    UINT8                          *pAreaInformation;                       /**< The name of the GPS area */
    UINT32                          AreaInformationCount;                   /**< Size of the AreaInformation. 0 = disable the tag */
    char                            DateStamp[11];                          /**< Format is YYYY:mm:dd */
    UINT16                          Differential;                           /**< 0 = No Correction, 1 = Differential Corrected */
    UINT32                          HPositioningErrorNum;                   /**< Numerator of HPositioningError */
    UINT32                          HPositioningErrorDen;                   /**< Denominator of HPositioningError */
} AMBA_EXIF_GPS_IFD_INFO_s;

/**
 *  Exif Formatter Information
 */
typedef struct {
    /* User should give below items for formater */
    char                            Name[AMBA_EXIF_MAX_FILENAME_LENGTH];    /**< File name */
    char                            Date[AMBA_EXIF_TIFF_DATETIME_SIZE];     /**< Create date */
    char                            Make[AMBA_EXIF_TIFF_MAKE_SIZE];         /**< Manufacture company name description */
    char                            Model[AMBA_EXIF_TIFF_MODEL_SIZE];       /**< DSC model description */
    char                            Software[AMBA_EXIF_TIFF_SOFTWARE_SIZE]; /**< DSC software version description */
    UINT32                          Width;                                  /**< Image width */
    UINT32                          Height;                                 /**< Image height */
    UINT32                          ThumbWidth;                             /**< Thumbnail image width */
    UINT32                          ThumbHeight;                            /**< Thumbnail image height */
    UINT32                          ScreenWidth;                            /**< Screennail image width */
    UINT32                          ScreenHeight;                           /**< Screennail image height */
    UINT8                          *pMakernote;                             /**< Maker note of size AMBA_EXIF_MAKERNOTE_SIZE (256) */
    UINT32                          DeviceSetDescripSize;                   /**< Device setting description size */
    UINT8                          *pDeviceSetDescrip;                      /**< Pointer to device setting description */
    /* AAA */
    AMBA_EXIF_AAA_INFO_s            PAAA;                                   /**< AAA related infomation */
    /* Exif GPS info enable flag and value */
    UINT8                           WithGPSInfo;                            /**< Exif GPS info enable flag */
    AMBA_EXIF_GPS_IFD_INFO_s        GPSInfo;                                /**< Exif GPS info */
    /* Exif header buffer. */
    /* The buffer should be non-cached if VFS enable DMA and the size exceeds AMBA_CFS_DMA_SIZE_THRESHOLD. */
    UINT8                          *HeaderBuffer;                           /**< Exif header buffer */
    UINT32                          HeaderBufferSize;                       /**< Exif header buffer size */
} AMBA_EXIF_FORMATTER_INFO_s;

/**
 *  Buffer Information
 */
typedef struct {
    UINT8                           *pBaseBuf;                              /**< Pointer to the buffer base */
    UINT8                           *pLimitBuf;                             /**< Pointer to the buffer limit (base + size -1) */
    UINT8                           *pFullview;                             /**< Pointer to the fullview image */
    UINT8                           *pThumb;                                /**< Pointer to the thumbnail image */
    UINT8                           *pScreen;                               /**< Pointer to the Screennail image  */
    UINT32                          FullviewSize;                           /**< FullView image size */
    UINT32                          ThumbSize;                              /**< Thumbnail image size. Set to 0, if thumbnail doesn't exist. */
    UINT32                          ScreenSize;                             /**< Screennail image size. Set to 0, if screennail doesn't exist. */
} AMBA_EXIF_FORMATTER_BUFFER_INFO_s;

/**
 *  Exif Formatter Image
 */
typedef  struct {
    AMBA_EXIF_FORMATTER_BUFFER_INFO_s BufferInfo;                            /**< Buffer information */
    AMBA_EXIF_FORMATTER_INFO_s       *pFormaterInfo;                         /**< Formatter information*/
} AMBA_EXIF_FMT_IMG_s;

UINT32 AmbaExifFormatter(const AMBA_EXIF_FMT_IMG_s *pImageInfo, AMBA_VFS_FILE_s  *pFile);

UINT32 AmbaExifFormatter_GetHeaderSize(UINT32 *HeaderSize);

#endif /* AMBA_EXIF_FORMATTER */
