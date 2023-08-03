/**
*  @file AmbaExif.h
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
*  @details Definitions & Constants for Exif 2.3 related APIs
*
*/

#ifndef AMBA_EXIF_H
#define AMBA_EXIF_H

#ifndef AMBA_EXIF_FILE_H
#include "AmbaExifFile.h"
#endif

#ifndef AMBA_EXIF_MEM_H
#include "AmbaExifMem.h"
#endif

#include "AmbaExifFormatter.h"
/*
#ifndef TRUE
#define TRUE (UINT32)1U
#endif

#ifndef FALSE
#define FALSE (UINT32)0U
#endif
*/
#define NG                                             1U

#define KB                                             ((UINT32)1 << 10)
#define MB                                             ((UINT32)1 << 20)

/* Tiff Tag Data Size */
#define AMBA_EXIF_MAX_APP_MARKER_NUMBER                 3U
#define AMBA_EXIF_MPO_MAX_NUM                           1U
#define AMBA_EXIF_MPO_LIST                              2U

/* JPEG Marker Segments */
#define AMBA_EXIF_SEGMENT_SOI                           0xFFD8      /* Start of compressed data */
#define AMBA_EXIF_SEGMENT_APP1                          0xFFE1      /* Exif attribute information */
#define AMBA_EXIF_SEGMENT_APP2                          0xFFE2      /* Flashpix extension */
#define AMBA_EXIF_SEGMENT_APP3                          0xFFE3      /* Additional image metadata fields */
#define AMBA_EXIF_SEGMENT_APP13                         0xFFED      /* IPTC photoshop metadata */
#define AMBA_EXIF_SEGMENT_DQT                           0xFFDB      /* Quantization table definition */
#define AMBA_EXIF_SEGMENT_DHT                           0xFFC4      /* Huffman table definition */
#define AMBA_EXIF_SEGMENT_DRI                           0xFFDD      /* Parameter data relating to frame */
#define AMBA_EXIF_SEGMENT_SOF                           0xFFC0      /* Parameter data relating to frame */
#define AMBA_EXIF_SEGMENT_SOS                           0xFFDA      /* Parameters relating to components */
#define AMBA_EXIF_SEGMENT_EOI                           0xFFD9      /* End of compressed data */

/* MP Index IFD tag list */
#define AMBA_EXIF_MPIFD_MPF_VERSION                     45056U
#define AMBA_EXIF_MPIFD_NUMBER_OF_IMAGES                45057U
#define AMBA_EXIF_MPIFD_MP_ENTRY                        45058U
#define AMBA_EXIF_MPIFD_IMAGE_UID_LIST                  45059U
#define AMBA_EXIF_MPIFD_TOTAL_FRAMES                    45060U

/* Exif ACSII keywords */
#define AMBA_EXIF_TIFF_HEADER                           0x45786966  /* Exif */
#define AMBA_EXIF_XMP_HEADER                            0x68747470  /* http */

/* EXIF TIFF Attribute Information */
#define AMBA_EXIF_TIFF_IMG_WIDTH                        256U
#define AMBA_EXIF_TIFF_IMG_HEIGHT                       257U
#define AMBA_EXIF_TIFF_BITS_PER_SAMPLE                  258U
#define AMBA_EXIF_TIFF_COMPRESSION                      259U
#define AMBA_EXIF_TIFF_PHOTO_METRIC_INTERPRETATION      262U
#define AMBA_EXIF_TIFF_IMG_DESC                         270U
#define AMBA_EXIF_TIFF_MAKE                             271U
#define AMBA_EXIF_TIFF_MODEL                            272U
#define AMBA_EXIF_TIFF_STRIP_OFFSETS                    273U
#define AMBA_EXIF_TIFF_ORIENTATION                      274U
#define AMBA_EXIF_TIFF_SAMPLES_PER_PIXEL                277U
#define AMBA_EXIF_TIFF_ROWS_PER_STRIP                   278U
#define AMBA_EXIF_TIFF_STRIP_BYTE_COUNTS                279U
#define AMBA_EXIF_TIFF_X_RESOLUTION                     282U
#define AMBA_EXIF_TIFF_Y_RESOLUTION                     283U
#define AMBA_EXIF_TIFF_PLANAR_CONFIGURATION             284U
#define AMBA_EXIF_TIFF_RESOLUTION_UNIT                  296U
#define AMBA_EXIF_TIFF_TRANSFER_FUNCTION                301U
#define AMBA_EXIF_TIFF_SOFTWARE                         305U
#define AMBA_EXIF_TIFF_DATETIME                         306U
#define AMBA_EXIF_TIFF_ARTIST                           315U
#define AMBA_EXIF_TIFF_WHITE_POINT                      318U
#define AMBA_EXIF_TIFF_PRIMARY_CHROMATICITIES           319U
#define AMBA_EXIF_TIFF_JPG_INTRCHG_FMT                  513U         /* Thumbnail SOI offset */
#define AMBA_EXIF_TIFF_JPG_INTRCHG_FMT_LENGTH           514U         /* Thumbnail size */
#define AMBA_EXIF_TIFF_YCBCR_COEFFICIENTS               529U
#define AMBA_EXIF_TIFF_YCBCR_SUB_SAMPLING               530U
#define AMBA_EXIF_TIFF_YCBCR_POSITIONING                531U
#define AMBA_EXIF_TIFF_REFERENCE_BLACK_WHITE            532U
#define AMBA_EXIF_TIFF_COPYRIGHT                        33432U
#define AMBA_EXIF_TIFF_EXIF_IFD_POINTER                 34665U
#define AMBA_EXIF_TIFF_GPS_INFO_IFD_POINTER             34853U

/* EXIF IFD Attribute Information */
#define AMBA_EXIF_IFD_EXPOSURE_TIME                     33434U
#define AMBA_EXIF_IFD_F_NUMBER                          33437U
#define AMBA_EXIF_IFD_EXPOSURE_PROGRAM                  34850U
#define AMBA_EXIF_IFD_SPECTRAL_SENSITIVITY              34852U
#define AMBA_EXIF_IFD_PHOTO_GRAPHIC_SENTIVITY           34855U       /* ISOSpeedRatings in 2.2 */
#define AMBA_EXIF_IFD_OECF                              34856U
#define AMBA_EXIF_IFD_SENSTIVITY_TYPE                   34864U
#define AMBA_EXIF_IFD_STANDARD_OUTPUT_SENSITIVITY       34865U
#define AMBA_EXIF_IFD_RECOMMENDED_EXPOSURE_INDEX        34866U
#define AMBA_EXIF_IFD_ISO_SPEED                         34867U
#define AMBA_EXIF_IFD_ISO_SPEED_LAT_YYY                 34868U
#define AMBA_EXIF_IFD_ISO_SPEED_LAT_ZZZ                 34869U
#define AMBA_EXIF_IFD_EXIF_VERSION                      36864U
#define AMBA_EXIF_IFD_DATE_TIME_ORIGINAL                36867U
#define AMBA_EXIF_IFD_DATE_TIME_DIGITIZED               36868U
#define AMBA_EXIF_IFD_COMPONENTS_CONFIGURATION          37121U
#define AMBA_EXIF_IFD_COMPRESSED_BITS_PER_PIXEL         37122U
#define AMBA_EXIF_IFD_SHUTTER_SPEED_VALUE               37377U
#define AMBA_EXIF_IFD_APERTURE_VALUE                    37378U
#define AMBA_EXIF_IFD_BRIGHTNESS_VALUE                  37379U
#define AMBA_EXIF_IFD_EXPOSURE_BIAS_VALUE               37380U
#define AMBA_EXIF_IFD_MAX_APERTURE_VALUE                37381U
#define AMBA_EXIF_IFD_SUBJECT_DISTANCE                  37382U
#define AMBA_EXIF_IFD_METERING_MODE                     37383U
#define AMBA_EXIF_IFD_LIGHT_SOURCE                      37384U
#define AMBA_EXIF_IFD_FLASH                             37385U
#define AMBA_EXIF_IFD_FOCAL_LENGTH                      37386U
#define AMBA_EXIF_IFD_SUBJECT_AREA                      37396U
#define AMBA_EXIF_IFD_MAKER_NOTE                        37500U
#define AMBA_EXIF_IFD_USER_COMMENT                      37510U
#define AMBA_EXIF_IFD_SUB_SEC_TIME                      37520U
#define AMBA_EXIF_IFD_SUB_SEC_TIME_ORIGINAL             37521U
#define AMBA_EXIF_IFD_SUB_SEC_TIME_DIGITIZED            37522U
#define AMBA_EXIF_IFD_FLASHPIX_VERSION                  40960U
#define AMBA_EXIF_IFD_COLOR_SPACE                       40961U
#define AMBA_EXIF_IFD_PIXEL_X_DIMENSION                 40962U
#define AMBA_EXIF_IFD_PIXEL_Y_DIMENSION                 40963U
#define AMBA_EXIF_IFD_RELATED_SOUND_FILE                40964U
#define AMBA_EXIF_IFD_INTEROPERABILITY_IFD_POINTER      40965U
#define AMBA_EXIF_IFD_FLASH_ENERGY                      41483U
#define AMBA_EXIF_IFD_SPATIAL_FREQUENCY_RESPONSE        41484U
#define AMBA_EXIF_IFD_FOCAL_PLANE_X_RESOLUTION          41486U
#define AMBA_EXIF_IFD_FOCAL_PLANE_Y_RESOLUTION          41487U
#define AMBA_EXIF_IFD_FOCAL_PLANE_RESOLUTION_UNIT       41488U
#define AMBA_EXIF_IFD_SUBJECT_LOCATION                  41492U
#define AMBA_EXIF_IFD_EXPOSURE_INDEX                    41493U
#define AMBA_EXIF_IFD_SENSING_METHOD                    41495U
#define AMBA_EXIF_IFD_FILE_SOURCE                       41728U
#define AMBA_EXIF_IFD_SCENE_TYPE                        41729U
#define AMBA_EXIF_IFD_CFA_PATTERN                       41730U
#define AMBA_EXIF_IFD_CUSTOM_RENDERED                   41985U
#define AMBA_EXIF_IFD_EXPOSURE_MODE                     41986U
#define AMBA_EXIF_IFD_WHITE_BALANCE                     41987U
#define AMBA_EXIF_IFD_DIGITAL_ZOOM_RATIO                41988U
#define AMBA_EXIF_IFD_FOCAL_LENGTH_IN_35MM_FILM         41989U
#define AMBA_EXIF_IFD_SCENE_CAPTURE_TYPE                41990U
#define AMBA_EXIF_IFD_GAIN_CONTROL                      41991U
#define AMBA_EXIF_IFD_CONTRAST                          41992U
#define AMBA_EXIF_IFD_SATURATION                        41993U
#define AMBA_EXIF_IFD_SHARPNESS                         41994U
#define AMBA_EXIF_IFD_DEVICE_SETTING_DECRIPTION         41995U
#define AMBA_EXIF_IFD_SUBJECT_DISTANCE_RANGE            41996U
#define AMBA_EXIF_IFD_IMAGE_UNIQUE_ID                   42016U
#define AMBA_EXIF_IFD_CAMERA_OWNER_NAME                 42032U
#define AMBA_EXIF_IFD_BODY_SERIAL_NUMBER                42033U
#define AMBA_EXIF_IFD_LENS_SPECIFICATION                42034U
#define AMBA_EXIF_IFD_LENS_MAKE                         42035U
#define AMBA_EXIF_IFD_LENS_MODEL                        42036U
#define AMBA_EXIF_IFD_LENS_SERIAL_NUMBER                42037U
#define AMBA_EXIF_IFD_GAMMA                             42240U

/* GPS IFD Attribute Information */
#define AMBA_EXIF_GPS_IFD_VERSION_ID                    0U
#define AMBA_EXIF_GPS_IFD_LATITUDE_REF                  1U           /* North or South Latitude */
#define AMBA_EXIF_GPS_IFD_LATITUDE                      2U
#define AMBA_EXIF_GPS_IFD_LONGITUDE_REF                 3U           /* East or West Longitude */
#define AMBA_EXIF_GPS_IFD_LONGITUDE                     4U
#define AMBA_EXIF_GPS_IFD_ALTITUDE_REF                  5U
#define AMBA_EXIF_GPS_IFD_ALTITUDE                      6U
#define AMBA_EXIF_GPS_IFD_TIME_STAMP                    7U
#define AMBA_EXIF_GPS_IFD_SATELLITES                    8U           /* Satellites used */
#define AMBA_EXIF_GPS_IFD_STATUS                        9U           /* Receiver status */
#define AMBA_EXIF_GPS_IFD_MEASURE_MODE                  10U          /* Measurement mode*/
#define AMBA_EXIF_GPS_IFD_DOP                           11U          /* Measurement precision */
#define AMBA_EXIF_GPS_IFD_SPEED_REF                     12U          /* Speed unit */
#define AMBA_EXIF_GPS_IFD_SPEED                         13U          /* Speed of GPS receiver */
#define AMBA_EXIF_GPS_IFD_TRACK_REF                     14U          /* Reference for direction of movement */
#define AMBA_EXIF_GPS_IFD_TRACK                         15U          /* Direction of movement*/
#define AMBA_EXIF_GPS_IFD_IMG_DIR_REF                   16U          /* Reference for direction of image */
#define AMBA_EXIF_GPS_IFD_IMG_DIR                       17U          /* Direction of image */
#define AMBA_EXIF_GPS_IFD_MAP_DATUM                     18U          /* Geodetic survey data used */
#define AMBA_EXIF_GPS_IFD_DEST_LAT_REF                  19U
#define AMBA_EXIF_GPS_IFD_DEST_LAT                      20U
#define AMBA_EXIF_GPS_IFD_DEST_LONG_REF                 21U
#define AMBA_EXIF_GPS_IFD_DEST_LONG                     22U
#define AMBA_EXIF_GPS_IFD_DEST_BEARING_REF              23U
#define AMBA_EXIF_GPS_IFD_DEST_BEARING                  24U
#define AMBA_EXIF_GPS_IFD_DEST_DIST_REF                 25U
#define AMBA_EXIF_GPS_IFD_DEST_DIST                     26U
#define AMBA_EXIF_GPS_IFD_PROCESSING_METHOD             27U          /* Name of GPS processing method */
#define AMBA_EXIF_GPS_IFD_AREA_INFORMATION              28U          /* Name of GPS area */
#define AMBA_EXIF_GPS_IFD_DATE_STAMP                    29U
#define AMBA_EXIF_GPS_IFD_DIFFERENTIAL                  30U          /* GPS differential correction */
#define AMBA_EXIF_GPS_IFD_H_POSITIONING_ERROR           31U          /* Horizontal positioning error. Added in 2.3 */

/* Tag data Type */
typedef enum {
    TYPE_BYTE = 1,
    TYPE_ASCII,
    TYPE_SHORT,
    TYPE_LONG,
    TYPE_RATIONAL,

    TYPE_UNDEFINE = 7,
    TYPE_ERROR,             /* Type number = 8 is not in the spec */
    TYPE_SLONG,
    TYPE_SRATIONAL
} AMBA_EXIF_IFD_STRUCTURE_TYPE_e;

/* Type of the second picture(screennail, ...) */
typedef enum {
    SECPIC_TYPE_APP2 = 0,
    SECPIC_TYPE_MPF,
    SECPIC_TYPE_FPXR
} AMBA_EXIF_SECOND_PIC_TYPE_e;

/* Type of the MPO picture */
typedef enum {
    MPO_TYPE_UNDEFINED,
    MPO_TYPE_LARGE_THUMBNAIL_VGA,
    MPO_TYPE_LARGE_THUMBNAIL_FULL_HD,
    MPO_TYPE_PANORAMA,
    MPO_TYPE_DISPARITY,
    MPO_TYPE_MULTI_ANGLE,
    MPO_TYPE_BASELINE_MP_PRIMARY_IMG
} AMBA_EXIF_MPO_TYPE_e;

#if 0
typedef struct {
    UINT32                          IFDZeroHeaderSize;                      /* EXIF IFD0 header buffer size */
    UINT32                          IFDZeroInfoSize;                        /* EXIF IFD0 info buffer size */
    UINT32                          IFDOneHeaderSize;                       /* EXIF IFD1 header buffer size */
    UINT32                          IFDOneInfoSize;                         /* EXIF IFD1 info buffer size */
    UINT32                          IFDHeaderSize;                          /* EXIF total header buffer size */
    UINT32                          IFDInfoSize;                            /* EXIF total info buffer size */
    UINT16                          IFDZeroTagsNumber;                      /* IFD0 usage Tag number */
    UINT16                          IFDOneTagsNumber;                       /* IFD1 usage Tag number */
    UINT16                          ExifFDTagsNumber;                       /* EXIF usage Tag number */
    UINT16                          IntFDTagsNumber;                        /* IntIFD usage Tag number */
    UINT16                          GpsFDTagsNumber;                        /* GPSIFD usage Tag number */
} AMBA_EXIF_IFD_PARAM_s;
#endif

typedef struct {
    UINT32                          Offset;
    UINT32                          Length;
} AMBA_EXIF_MARKER_INFO_s;

typedef struct {
    INT32                           MarkerNumber;                           /* DQT marker number should be less than 3 */
    UINT32                          Offset[3];                              /* Marker offset */
    UINT32                          Length[3];                              /* Field length */
} AMBA_EXIF_DQT_HEADER_INFO_s;

typedef struct {
    INT32                           MarkerNumber;                           /* DHT marker number should be less than 4 */
    UINT32                          Offset[4];                              /* Marker offset */
    UINT32                          Length[4];                              /* Field length */
} AMBA_EXIF_DHT_HEADER_INFO_s;

typedef struct {
    UINT8                           APPMarker;                              /* 0xFFE0~0xFFEF, so only tailing byte is stored. */
    INT64                           Offset;
    UINT32                          Size;
} AMBA_EXIF_APP_MARKER_INFO_s;

/* Record the offset and length of thumbnail, screennail and fullview */
typedef struct {
    AMBA_EXIF_MARKER_INFO_s          APP0;
    AMBA_EXIF_DQT_HEADER_INFO_s      DQT;
    AMBA_EXIF_MARKER_INFO_s          DRI;
    AMBA_EXIF_MARKER_INFO_s          SOF;
    AMBA_EXIF_MARKER_INFO_s          SOS;
    AMBA_EXIF_DHT_HEADER_INFO_s      DHT;
} AMBA_EXIF_JPEG_HEADER_INFO_s;

/* For JPEG transcoder */
typedef struct {
    UINT32                          LengthOfFile;
    UINT32                          XImage;                                 /* Dimension of the image */
    UINT32                          YImage;
    UINT8                           YH;                                     /* Sampling factors (horizontal and vertical) for Y; Cb,Cr should be one */
    UINT8                           YV;
    UINT32                          MCURestart;                             /* Restart markers appears every MCU_restart MCU blocks */
    UINT8                           Component;
} AMBA_EXIF_JPEG_INFO_s;

typedef struct {
    UINT32                          Width;                                  /* Image width */
    UINT32                          Height;                                 /* Image height*/
    UINT32                          ThumbWidth;                             /* Thumbnail image width */
    UINT32                          ThumbHeight;                            /* Thumbnail image height */
    UINT32                          ThumbPos;                               /* Thumbnail image position */
    UINT32                          ThumbSize;                              /* Thumbnail image size */
    UINT32                          ThumbOriSize;
    UINT32                          ScreenWidth;                            /* Screennail image width */
    UINT32                          ScreenHeight;                           /* Screennail image height */
    UINT32                          ScreenPos;                              /* Screennail image position */
    UINT32                          ScreenSize;                             /* Screennail image size */
    UINT32                          ScreenOriSize;
    UINT32                          FullviewPos;                            /* FullView image position */
    UINT32                          FullviewSize;                           /* FullView image size */
    UINT32                          FullviewOriSize;
    INT32                           DHTStat;
    INT32                           ThumbDHTStat;
    INT32                           ScreenDHTStat;
    AMBA_EXIF_JPEG_HEADER_INFO_s     JPEGThumbHeaderInfo;
    AMBA_EXIF_JPEG_INFO_s            JPEGThumbInfo;
    AMBA_EXIF_JPEG_HEADER_INFO_s     JPEGHeaderInfo;
    AMBA_EXIF_JPEG_INFO_s            JPEGInfo;
    AMBA_EXIF_JPEG_HEADER_INFO_s     JPEGScreenHeaderInfo;
    AMBA_EXIF_JPEG_INFO_s            JPEGScreenInfo;
} AMBA_EXIF_MPO_INFO_s;

typedef struct {
    /* INFO */
    char                            Name[AMBA_EXIF_MAX_FILENAME_LENGTH];     /* File name */
    char                            Date[AMBA_EXIF_TIFF_DATETIME_SIZE];      /* Create date */
    UINT32                          Width;                                  /* Image width */
    UINT32                          Height;                                 /* Image height */
    UINT32                          ThumbWidth;                             /* Thumbnail image width */
    UINT32                          ThumbHeight;                            /* Thumbnail image height */
    UINT32                          ThumbPos;                               /* Thumbnail image position */
    UINT32                          ThumbSize;                              /* Thumbnail image size */
    UINT32                          ThumbOriSize;
    UINT32                          ScreenWidth;                            /* Screennail image width */
    UINT32                          ScreenHeight;                           /* Screennail image height */
    UINT32                          ScreenPos;                              /* Screen nail image position */
    UINT32                          ScreenSoiOffset;                        /* Offset from app2 to image's SOI maker, for ftyp segment */
    UINT32                          ScreenSize;                             /* Screennail image size */
    UINT32                          ScreenOriSize;
    UINT32                          FullviewPos;                            /* FullView image position */
    UINT32                          FullviewSize;                           /* FullView image size */
    UINT32                          FullviewOriSize;
    AMBA_EXIF_APP_MARKER_INFO_s      AppMarkerInfo[AMBA_EXIF_MAX_APP_MARKER_NUMBER];
    UINT8                           AppMarkerNum;
    UINT64                          FileSize;                               /* File length */
    /* DEMUX */
    UINT32                          ImageDescriptionSize;                   /* Image description size */
    UINT32                          ImageDescriptionOffset;                 /* Image description offset */
    /* Image description content */
    UINT8                           ImageDescription[AMBA_EXIF_TIFF_IMG_DESC_SIZE];
    UINT32                          MakeSize;                               /* Manufacture company name size */
    UINT32                          MakeOffset;                             /* Manufacture company name offset */
    UINT8                           Make[AMBA_EXIF_TIFF_MAKE_SIZE];          /* Manufacture company name description */
    UINT32                          ModelSize;                              /* DSC model size */
    UINT32                          ModelOffset;                            /* DSC model offset */
    UINT8                           Model[AMBA_EXIF_TIFF_MODEL_SIZE];        /* DSC model description */
    UINT32                          SoftwareSize;                           /* DSC software version size */
    UINT32                          SoftwareOffset;                         /* DSC software version offset */
    /* DSC software version description */
    UINT8                           Software[AMBA_EXIF_TIFF_SOFTWARE_SIZE];
    UINT32                          MakernoteSize;                          /* Maker note size */
    UINT32                          MakernoteOffset;                        /* Maker note offset */
    UINT32                          DeviceSetDescripSize;                   /* Device setting description size */
    UINT32                          DeviceSetDescripOffset;                 /* Device setting description offset */
    UINT32                          OrientationOffset;                      /* Orientation offset */
    UINT32                          ThmOrientationOffset;                   /* Orientation offset of thumbnail */
    INT32                           ThmOffset;                              /* Thumbnail offset for .tdt file */
    UINT32                          YUVType;                                /* YUV type 4:2:2=21,  4:2:0=22 */
    UINT8                           Endian;                                 /* 0: big endian, 1: little endian */
    UINT8                           Orientation;                            /* Orienation */
    UINT8                           ThumbOrientation;                       /* Orienation of thumbnail */
    UINT8                           WithExif;                               /* 0: Without exif Ifd info, 1: with exif Ifd info */
    UINT8                           Fullview;                               /* Fullview number */
    UINT8                           Thumbnail;                              /* Thumbnail number */
    UINT8                           Screennail;                             /* Screennail number */
    UINT8                           Error;                                  /* Run time error detected */
    /* AAA */
    AMBA_EXIF_AAA_INFO_s             PAAA;                                   /* AAA related information */
    /* Transcoder */
    INT32                           DHTStat;
    INT32                           ThumbDHTStat;
    INT32                           ScreenDHTStat;
    AMBA_EXIF_JPEG_HEADER_INFO_s     JPEGThumbHeaderInfo;
    AMBA_EXIF_JPEG_INFO_s            JPEGThumbInfo;
    AMBA_EXIF_JPEG_HEADER_INFO_s     JPEGHeaderInfo;
    AMBA_EXIF_JPEG_INFO_s            JPEGInfo;
    AMBA_EXIF_JPEG_HEADER_INFO_s     JPEGScreenHeaderInfo;
    AMBA_EXIF_JPEG_INFO_s            JPEGScreenInfo;
    UINT8                           WithGPSIfd;                             /* GPS ifd is in header or not */
    AMBA_EXIF_GPS_IFD_INFO_s         GPSInfo;                                /* GPS IFD info */
    UINT8                           SecondPicType;                          /* Type of the second picture */
    AMBA_EXIF_MPO_INFO_s             MPO[AMBA_EXIF_MPO_MAX_NUM];
    /* MPO */
    UINT8                           MPONumberOfImgs;
    UINT8                           MPOImgsTypeList[AMBA_EXIF_MPO_LIST];
    UINT32                          MPOScreennailAPPXSize;                  /* MPO screennail size */
    UINT32                          MPOScreennailAPPXStart;                 /* MPO screennail start offset */
} AMBA_EXIF_IMAGE_INFO_s;

typedef struct {
    /* INFO */
    UINT32                          ThumbWidth;                             /* Thumbnail image width */
    UINT32                          ThumbHeight;                            /* Thumbnail image height */
    UINT32                          ThumbPos;                               /* Thumbnail image position */
    UINT32                          ThumbSize;                              /* Thumbnail image size */
    UINT8                           Thumbnail;                              /* Thumbnail number */
    UINT32                          YUVType;                                /* YUV type 4:2:2=21,  4:2:0=22 */
} AMBA_EXIF_THUMB_INFO_s;

/*----------------------------------------------------------------------------*\
 * Defined in AmbaExifDemuxer.c
\*----------------------------------------------------------------------------*/
UINT32 AmbaExifDemuxer_ParseAPP1TIFF(AMBA_EXIF_MEM *pCurrentMemory, AMBA_EXIF_IMAGE_INFO_s *pImageInfo, const AMBA_FS_FILE_INFO_s *pFileStatus);
UINT32 AmbaExifDemuxer_ParseAPP1XMP(const AMBA_EXIF_MEM *pCurrentMemory, const AMBA_EXIF_IMAGE_INFO_s *pImageInfo);
UINT32 AmbaExifDemuxer_ParseAPP2(AMBA_EXIF_MEM *pCurrentMemory, AMBA_EXIF_IMAGE_INFO_s *pImageInfo, INT64 OffsetOfApp2, UINT32 Length);
UINT32 AmbaExifDemuxer_ParseAPP2MPF(AMBA_EXIF_MEM *pCurrentMemory, AMBA_EXIF_IMAGE_INFO_s *pImageInfo);
UINT32 AmbaExifDemuxer_ParseMPFFull(AMBA_EXIF_MEM *pCurrentMemory, AMBA_EXIF_IMAGE_INFO_s *pImageInfo, UINT8 Index);
UINT32 AmbaExifDemuxer_ParseMPFScrn(AMBA_EXIF_MEM *pCurrentMemory, AMBA_EXIF_IMAGE_INFO_s *pImageInfo, UINT8 Index);
UINT32 AmbaExifDemuxer_FastParseAPP1(AMBA_EXIF_MEM *pCurrentMemory, AMBA_EXIF_IMAGE_INFO_s *pImageInfo);
UINT32 AmbaExifDemuxer_CheckTag(AMBA_EXIF_MEM *pCurrentMemory, AMBA_EXIF_IMAGE_INFO_s *pImageInfo, UINT8 FullThumbScr, UINT32 TagStartPos);

/*----------------------------------------------------------------------------*\
 * Defined in AmbaExifParser.c
\*----------------------------------------------------------------------------*/
UINT32 AmbaExifFullParse(AMBA_FS_FILE *pCurrentFile, AMBA_EXIF_IMAGE_INFO_s *pImageInfo, const AMBA_FS_FILE_INFO_s *pFileStatus);
UINT32 AmbaExifFastParse(AMBA_FS_FILE *pCurrentFile, AMBA_EXIF_THUMB_INFO_s *pThumbInfo);

void AmbaExifParseDebugPrint(const AMBA_EXIF_IMAGE_INFO_s *pImageInfo);

#endif /* AMBA_EXIF_H */
