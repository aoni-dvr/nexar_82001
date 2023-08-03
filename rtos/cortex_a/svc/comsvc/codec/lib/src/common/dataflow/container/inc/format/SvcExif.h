/**
 * @file SvcExif.h
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
#ifndef CONTAINER_EXIF_H
#define CONTAINER_EXIF_H

#include "format/SvcFormatDef.h"
#include <Amba_ExifImg.h>

/**
 * Tags setting
 */
#define SVC_TAG_DISABLED        0x00U    /**< The flag to disable the tag setting */
#define SVC_TAG_ENABLED         0x01U    /**< The flag to enable the tag setting */
#define SVC_TAG_CONFIGURED      0x02U    /**< The flag to enable and to configure the tag attribute information */

/**
 * The tags numbers used in ExifMux
 */
#define SVC_IFD0_TOTAL_TAGS     33U     /**< Number of Ifd0 tags */
#define SVC_IFD1_TOTAL_TAGS     32U     /**< Number of Ifd1 tags */
#define SVC_EXIF_TOTAL_TAGS     70U     /**< Number of Exif tags */
#define SVC_IntIFD_TOTAL_TAGS   2U      /**< Number of IntIfd tags */
#define SVC_GPS_TOTAL_TAGS      32U     /**< Number of GPS tags */
#define SVC_IPTC_TOTAL_TAGS     20U     /**< Number of IPTC tags */
#define SVC_MPIDX_TOTAL_TAGS    5U      /**< Number of MPIdx tags */
#define SVC_MPATTR_TOTAL_TAGS   15U     /**< Number of MPAttr tags */

/**
 * Tags which contains image information data and used by IFD0, IFD1
 */
#define SVC_TIFF_ImageWidth                     256U      /**<  The image width */
#define SVC_TIFF_ImageHeight                    257U      /**<  The image height */
#define SVC_TIFF_BitsPerSample                  258U      /**<  The number of bits per component for each pixel when image format is no compression */
#define SVC_TIFF_Compression                    259U      /**<  The compression method */
#define SVC_TIFF_PhotometricInterpretation      262U      /**<  The clolor space */
#define SVC_TIFF_ImageDescription               270U      /**<  The description of image */
#define SVC_TIFF_Make                           271U      /**<  The manufacturer of digicam */
#define SVC_TIFF_Model                          272U      /**<  The model number of digicam */
#define SVC_TIFF_StripOffsets                   273U      /**<  The offset to image data when image format is no compression */
#define SVC_TIFF_Orientation                    274U      /**<  The orientation of the camera relative to the scene */
#define SVC_TIFF_SamplesPerPixel                277U      /**<  Number of components */
#define SVC_TIFF_RowsPerStrip                   278U      /**<  Number of rows per stripe */
#define SVC_TIFF_StripByteCounts                279U      /**<  Bytes per compressed stripe */
#define SVC_TIFF_XResolution                    282U      /**<  The display resolution in width direction */
#define SVC_TIFF_YResolution                    283U      /**<  The display resolution in height direction */
#define SVC_TIFF_PlanarConfiguration            284U      /**<  Image data arrangement */
#define SVC_TIFF_ResolutionUnit                 296U      /**<  Unit of XResolution/YResolution */
#define SVC_TIFF_TransferFunction               301U      /**<  Transfer function */
#define SVC_TIFF_Software                       305U      /**<  Firmware version */
#define SVC_TIFF_DateTime                       306U      /**<  The modified Date/Time of image*/
#define SVC_TIFF_Artist                         315U      /**<  The orientation of the camera relative to the scene */
#define SVC_TIFF_WhitePoint                     318U      /**<  The chromaticity of white point of image */
#define SVC_TIFF_PrimaryChromaticities          319U      /**<  The chromaticity of the primaries of image */
#define SVC_TIFF_JPEGInterchangeFormat          513U      /**<  Offset to JPEG 'SOI' */
#define SVC_TIFF_JPEGInterchangeFormatLength    514U      /**<  Bytes of JPEG data */
#define SVC_TIFF_YCbCrCoefficients              529U      /**<  YCbCr-RGB transformation coefficients matrix */
#define SVC_TIFF_YCbCrSubSampling               530U      /**<  Chrominance subsampleing information */
#define SVC_TIFF_YCbCrPositioning               531U      /**<  Chroma sample point of subsampling pixel array */
#define SVC_TIFF_ReferenceBlackWhite            532U      /**<  Referenced values */
#define SVC_TIFF_Copyright                      33432U    /**<  Copyright holder */
#define SVC_TIFF_ExifIFDPointer                 34665U    /**<  The offset to the ExifIFD */
#define SVC_TIFF_GPSInfoIFDPointer              34853U    /**<  The offset to the GPSIFD */
#define SVC_TIFF_PIM                            50341U    /**<  The offset to the PrintIM information */

/**
 * EXIFIFD is a set of tags for recording exif-specific attribute information
 * and is pointed by the SVC_TIFF_ExifIFDPointer(34665) tag in IFD0 or IFD1.
 */
#define SVC_EXIFIFD_ExposureTime                33434U    /**<  Exposure time, given in seconds */
#define SVC_EXIFIFD_FNumber                     33437U    /**<  The F number */
#define SVC_EXIFIFD_ExposureProgram             34850U    /**<  The class of the program used to set exposure */
#define SVC_EXIFIFD_SpectralSensitivity         34852U    /**<  The spectra sensitivity of each channel of the camera used */
#define SVC_EXIFIFD_ISOSpeedRatings             34855U    /**<  ISOSpeedRatings value up to 2.21; PhohographicSentivity in 2.3 */
#define SVC_EXIFIFD_OECF                        34856U    /**<  Indicates the OECF specified in ISO 14524 */
#define SVC_EXIFIFD_SensitivityType             34864U    /**<  Sensitivity type; defined in exif 2.3 */
#define SVC_EXIFIFD_StandardOutputSensitivity   34865U    /**<  The standard output sensitivity of a camera; define in exif 2.3 */
#define SVC_EXIFIFD_RecommendedExposureIndex    34866U    /**<  The Recommended exposure index value of a camera; defined in exif 2.3 */
#define SVC_EXIFIFD_ISOSpeed                    34867U    /**<  The ISO speed value of a camera; defined in exif 2.3 */
#define SVC_EXIFIFD_ISOSpeedLatitudeyyy         34868U    /**<  The ISO speed latitude yyy value of a camera; defined in exif 2.3 */
#define SVC_EXIFIFD_ISOSpeedLatitudezzz         34869U    /**<  The ISO speed latitude zzz value of a camera; defined in exif 2.3 */
#define SVC_EXIFIFD_ExifVersion                 36864U    /**<  The exif version is supported */
#define SVC_EXIFIFD_DateTimeOriginal            36867U    /**<  The Date/Time when the image was generated */
#define SVC_EXIFIFD_DateTimeDigitized           36868U    /**<  The Date/Time when the image was stored */
#define SVC_EXIFIFD_ComponentsConfiguration     37121U    /**<  Meaning of each component */
#define SVC_EXIFIFD_CompressedBitsPerPixel      37122U    /**<  Image compression mode */
#define SVC_EXIFIFD_ShutterSpeedValue           37377U    /**<  Shutter speed */
#define SVC_EXIFIFD_ApertureValue               37378U    /**<  The lens aperture */
#define SVC_EXIFIFD_BrightnessValue             37379U    /**<  The value of brightness */
#define SVC_EXIFIFD_ExposureBiasValue           37380U    /**<  The exposure bias */
#define SVC_EXIFIFD_MaxApertureValue            37381U    /**<  The smallest F number of the lens */
#define SVC_EXIFIFD_SubjectDistance             37382U    /**<  The distance to the subject */
#define SVC_EXIFIFD_MeteringMode                37383U    /**<  The metering mode */
#define SVC_EXIFIFD_LightSource                 37384U    /**<  The kind of light source */
#define SVC_EXIFIFD_Flash                       37385U    /**<  The status of flash when the image was shot */
#define SVC_EXIFIFD_FocalLength                 37386U    /**<  The actual focal length of the lens */
#define SVC_EXIFIFD_SubjectArea                 37396U    /**<  The location and area of the main subject */
#define SVC_EXIFIFD_MakerNote                   37500U    /**<  Manufacturer notes */
#define SVC_EXIFIFD_UserComment                 37510U    /**<  User comments */
#define SVC_EXIFIFD_SubSecTime                  37520U    /**<  DateTime subseconds */
#define SVC_EXIFIFD_SubSecTimeOriginal          37521U    /**<  DateTimeOriginal subseconds */
#define SVC_EXIFIFD_SubSecTimeDigitized         37522U    /**<  DateTimeDigitized subseconds */
#define SVC_EXIFIFD_FlashpixVersion             40960U    /**<  Supported flash version */
#define SVC_EXIFIFD_ColorSpace                  40961U    /**<  Color space information */
#define SVC_EXIFIFD_PixelXDimension             40962U    /**<  Valid image width */
#define SVC_EXIFIFD_PixelYDimension             40963U    /**<  Valid image height */
#define SVC_EXIFIFD_RelatedSoundFile            40964U    /**<  Related audio file */
#define SVC_EXIFIFD_InteroperabilityIFDPointer  40965U    /**<  The offset to the interoperability tags */
#define SVC_EXIFIFD_FlashEnergy                 41483U    /**<  Flash energy*/
#define SVC_EXIFIFD_SpatialFrequencyResponse    41484U    /**<  Spatial frequency table and SFR values */
#define SVC_EXIFIFD_FocalPlaneXResolution       41486U    /**<  The number of pixels in the image width(X) direction */
#define SVC_EXIFIFD_FocalPlaneYResolution       41487U    /**<  The number of pixels in the image height(Y) direction */
#define SVC_EXIFIFD_FocalPlaneResolutionUnit    41488U    /**<  The unit for measuring FocalPlaneXResolution/FocalPlaneYResolution */
#define SVC_EXIFIFD_SubjectLocation             41492U    /**<  The location of the object in the scene */
#define SVC_EXIFIFD_ExposureIndex               41493U    /**<  The exposure index selected on the camera */
#define SVC_EXIFIFD_SensingMethod               41495U    /**<  The image sensor type on the camera */
#define SVC_EXIFIFD_FileSource                  41728U    /**<  The image source */
#define SVC_EXIFIFD_SceneType                   41729U    /**<  The type of scene */
#define SVC_EXIFIFD_CFAPattern                  41730U    /**<  The color filter array geometric pattern of the image sensor */
#define SVC_EXIFIFD_CustomRendered              41985U    /**<  The use of special processing on image data */
#define SVC_EXIFIFD_ExposureMode                41986U    /**<  The exposure mode set when the image was shot */
#define SVC_EXIFIFD_WhiteBalance                41987U    /**<  The white balance mode set when the image was shot */
#define SVC_EXIFIFD_DigitalZoomRatio            41988U    /**<  The digital zoom ratio when the image was shot */
#define SVC_EXIFIFD_FocalLengthIn35mmFilm       41989U    /**<  Focal length in 35 mm film */
#define SVC_EXIFIFD_SceneCaptureType            41990U    /**<  Scene capture type */
#define SVC_EXIFIFD_GainControl                 41991U    /**<  The degree of overall image gain adjustment */
#define SVC_EXIFIFD_Contrast                    41992U    /**<  The direction of contrast processing */
#define SVC_EXIFIFD_Saturation                  41993U    /**<  The direction of saturation processing */
#define SVC_EXIFIFD_Sharpness                   41994U    /**<  The direction of sharpness processing */
#define SVC_EXIFIFD_DeviceSettingDescription    41995U    /**<  Information on the picture-taking condition */
#define SVC_EXIFIFD_SubjectDistanceRange        41996U    /**<  The distance to the subject */
#define SVC_EXIFIFD_ImageUniqueID               42016U    /**<  An identifier assigned uniquely to each image */
#define SVC_EXIFIFD_CameraOwnerName             42032U    /**<  The owner of the camera */
#define SVC_EXIFIFD_BodySerialNumber            42033U    /**<  The serial number of the body of the camera */
#define SVC_EXIFIFD_LensSpecification           42034U    /**<  The specification of the lens */
#define SVC_EXIFIFD_LensMake                    42035U    /**<  The lens manufacture */
#define SVC_EXIFIFD_LensModel                   42036U    /**<  The lens model name and numeber */
#define SVC_EXIFIFD_LensSerialNumber            42037U    /**<  The serial number of the interchangeble lens */
#define SVC_EXIFIFD_Gamma                       42240U    /**<  The value coefficient gamma */

/**
 * GPSIFD is a set of tags for recording information such as GPS information,
 * and is pointed by the SVC_TIFF_GPSInfoIFDPointer(34853) tag in IFD0 or IFD1.
 */
#define SVC_GPSIFD_GPSVersionID             0U    /**<  GPS tag version */
#define SVC_GPSIFD_GPSLatitudeRef           1U    /**<  North or south latitude */
#define SVC_GPSIFD_GPSLatitude              2U    /**<  Latitude */
#define SVC_GPSIFD_GPSLongitudeRef          3U    /**<  East or west latitude */
#define SVC_GPSIFD_GPSLongitude             4U    /**<  Longitude */
#define SVC_GPSIFD_GPSAltitudeRef           5U    /**<  Altitude reference */
#define SVC_GPSIFD_GPSAltitude              6U    /**<  Altitude */
#define SVC_GPSIFD_GPSTimeStamp             7U    /**<  GPS time(atomic clock) */
#define SVC_GPSIFD_GPSSatellites            8U    /**<  GPS satellites used for measurement */
#define SVC_GPSIFD_GPSStatus                9U    /**<  GPS receiver status */
#define SVC_GPSIFD_GPSMeasureMode           10U   /**<  GPS measurement mode */
#define SVC_GPSIFD_GPSDOP                   11U   /**<  Measurement precision */
#define SVC_GPSIFD_GPSSpeedRef              12U   /**<  Speed unit */
#define SVC_GPSIFD_GPSSpeed                 13U   /**<  Speed of GPS receiver */
#define SVC_GPSIFD_GPSTrackRef              14U   /**<  Reference for direction of movement */
#define SVC_GPSIFD_GPSTrack                 15U   /**<  Direction of movement */
#define SVC_GPSIFD_GPSImgDirectionRef       16U   /**<  Reference for direction of image */
#define SVC_GPSIFD_GPSImgDirection          17U   /**<  Direction of image */
#define SVC_GPSIFD_GPSMapDatum              18U   /**<  Geodetic survey data used */
#define SVC_GPSIFD_GPSDestLatitudeRef       19U   /**<  Reference for latitude of destination */
#define SVC_GPSIFD_GPSDestLatitude          20U   /**<  Latitude of destination */
#define SVC_GPSIFD_GPSDestLongitudeRef      21U   /**<  Reference for longitude of destination */
#define SVC_GPSIFD_GPSDestLongitude         22U   /**<  Longitude of destination */
#define SVC_GPSIFD_GPSDestBearingRef        23U   /**<  Reference for bearing of destination */
#define SVC_GPSIFD_GPSDestBearing           24U   /**<  Bearing of destination */
#define SVC_GPSIFD_GPSDestDistanceRef       25U   /**<  Reference for distance of destination */
#define SVC_GPSIFD_GPSDestDistance          26U   /**<  Distance to destination */
#define SVC_GPSIFD_GPSProcessingMethod      27U   /**<  Name of GPS processing method */
#define SVC_GPSIFD_GPSAreaInformation       28U   /**<  Name of GPS area */
#define SVC_GPSIFD_GPSDateStamp             29U   /**<  GPS date */
#define SVC_GPSIFD_GPSDifferential          30U   /**<  GPS differential correction */
#define SVC_GPSIFD_GPSHPositioningError     31U   /**<  Horizontal position error */

/**
 * Interoperability IFD is composed tags which store the information to ensure the
 * interoperability and pointed by the SVC_EXIFIFD_InteroperabilityIFDPointer(40965) tag in EXIFIFD.
 */
#define SVC_IntIFD_InteroperabilityIndex        1U   /**<  The identification of the interoperability rule */
#define SVC_IntIFD_InteroperabilityVersion      2U   /**<  The interoperability version is supported */

/**
 * TODO: The below tags are other metadata information for image
 * IPTC tag list
 * MP Index IFD tag list
 * MP Attribute IFD tag list
 */
#define SVC_IPTC_ObjectName                     0x0205U /**< Object Name */
#define SVC_IPTC_Urgent                         0x020AU /**< Urgent */
#define SVC_IPTC_Categories                     0x020FU /**< Categories */
#define SVC_IPTC_SupplementalCategories         0x0214U /**< Supplemental Categories */
#define SVC_IPTC_Keywords                       0x0219U /**< Keywords */
#define SVC_IPTC_SpecialInstructions            0x0228U /**< Special Instructions */
#define SVC_IPTC_DateCreated                    0x0237U /**< Date Created */
#define SVC_IPTC_Byline                         0x0250U /**< By line  */
#define SVC_IPTC_BylineTitle                    0x0255U /**< By line Title  */
#define SVC_IPTC_City                           0x025AU /**< City */
#define SVC_IPTC_Sublocation                    0x025CU /**< Sublocation */
#define SVC_IPTC_ProvinceState                  0x025FU /**< Province State */
#define SVC_IPTC_Country                        0x0265U /**< Country */
#define SVC_IPTC_OriginalTransmissionReference  0x0267U /**< Original Transmission Reference */
#define SVC_IPTC_Headline                       0x0269U /**< Headline */
#define SVC_IPTC_Credits                        0x026EU /**< Credits */
#define SVC_IPTC_Source                         0x0273U /**< Source */
#define SVC_IPTC_Copyright                      0x0274U /**< Copyright */
#define SVC_IPTC_Caption                        0x0278U /**< Caption */
#define SVC_IPTC_CaptionWriter                  0x027AU /**< CaptionWriter */

#define SVC_MPIFD_MPFVersion                45056U /**< MPF Version */
#define SVC_MPIFD_NumberOfImages            45057U /**< Number Of Images */
#define SVC_MPIFD_MPEntry                   45058U /**< MP Entry */
#define SVC_MPIFD_ImageUIDList              45059U /**< Image UID List  */
#define SVC_MPIFD_TotalFrames               45060U /**< Total Frames */

#define SVC_MPIFD_MPIndividualNum           45313U /**< MP Individual Num */
#define SVC_MPIFD_PanOrientation            45569U /**< PanOrientation */
#define SVC_MPIFD_PanOverlap_H              45570U /**< PanOverlap_H */
#define SVC_MPIFD_PanOverlap_V              45571U /**< PanOverlap_V */
#define SVC_MPIFD_BaseViewpointNum          45572U /**< Base Viewpoint Num */
#define SVC_MPIFD_CovergenceAngle           45573U /**< Covergence Angle */
#define SVC_MPIFD_BaselineLength            45574U /**< Baseline Length */
#define SVC_MPIFD_VerticalDivergence        45575U /**< Vertical Divergence */
#define SVC_MPIFD_AxisDistance_X            45576U /**< AxisDistance_X */
#define SVC_MPIFD_AxisDistance_Y            45577U /**< AxisDistance_Y */
#define SVC_MPIFD_AxisDistance_Z            45578U /**< AxisDistance_Z */
#define SVC_MPIFD_YawAngle                  45579U /**< Yaw Angle */
#define SVC_MPIFD_PitchAngle                45580U /**< Pitch Angle */
#define SVC_MPIFD_RollAngle                 45581U /**< Roll Angle */

/**
 * Exif GPS information
 */
typedef struct {
    UINT8   VersionId[4];           /**< GPSVersionID */
    UINT8   LatitudeRef[2];         /**< GPSLatitudeRef */
    UINT32  LatitudeNum[3];         /**< Numerator of GPSLatitude */
    UINT32  LatitudeDen[3];         /**< Denominator of GPSLatitude */
    UINT8   LongitudeRef[2];        /**< GPSLongitudeRef */
    UINT32  LongitudeNum[3];        /**< Numerator of GPSLongitude */
    UINT32  LongitudeDen[3];        /**< Denominator GPSLongitude */
    UINT8   AltitudeRef;            /**< GPSAltitudeRef */
    UINT32  AltitudeNum;            /**< Numerator of GPSAltitude */
    UINT32  AltitudeDen;            /**< Denominator of GPSAltitude */
    UINT32  TimestampNum[3];        /**< Numerator of GPSTimeStamp */
    UINT32  TimestampDen[3];        /**< Denominator of GPSTimeStamp */
    ULONG   SatelliteOffset;        /**< GPSSatellites tag offset*/
    UINT32  SatelliteCount;         /**< GPSSatellites tag size */
    UINT8   Status[2];              /**< GPSStatus */
    UINT8   MeasureMode[2];         /**< GPSMeasureMode */
    UINT32  DopNum;                 /**< Numerator of GPSDOP */
    UINT32  DopDen;                 /**< Denominator of GPSDOP */
    UINT8   SpeedRef[2];            /**< GPSSpeedRef */
    UINT32  SpeedNum;               /**< Numerator of GPSSpeed */
    UINT32  SpeedDen;               /**< Denominator of GPSSpeed */
    UINT8   TrackRef[2];            /**< GPSTrackRef */
    UINT32  TrackNum;               /**< Numerator of GPSTrack */
    UINT32  TrackDen;               /**< Denominator of GPSTrack */
    UINT8   ImgdirectionRef[2];     /**< GPSImgDirectionRef */
    UINT32  ImgdirectionNum;        /**< Numerator of GPSImgDirection */
    UINT32  ImgdirectionDen;        /**< Denominator of GPSImgDirection */
    ULONG   MapdatumOffset;         /**< GPSMapDatum tag offset*/
    UINT32  MapdatumCount;          /**< GPSMapDatum tag size */
    UINT8   DestlatitudeRef[2];     /**< GPSDestLatitudeRef */
    UINT32  DestlatitudeNum[3];     /**< Numerator of GPSDestLatitude */
    UINT32  DestlatitudeDen[3];     /**< Denominator of GPSDestLatitude */
    UINT8   DestlongitudeRef[2];    /**< GPSDestLongitudeRef */
    UINT32  DestlongitudeNum[3];    /**< Numerator of GPSDestLongitude */
    UINT32  DestlongitudeDen[3];    /**< Denominator of GPSDestLongitude */
    UINT8   DestbearingRef[2];      /**< GPSDestBearingRef */
    UINT32  DestbearingNum;         /**< Numerator of GPSDestBearing */
    UINT32  DestbearingDen;         /**< Denominator of GPSDestBearing */
    UINT8   DestdistanceRef[2];     /**< GPSDestDistanceRef */
    UINT32  DestdistanceNum;        /**< Numerator of GPSDestDistance */
    UINT32  DestdistanceDen;        /**< Denominator of GPSDestDistance */
    ULONG   ProcessingmethodOffset; /**< GPSProcessingMethod tag offset */
    UINT32  ProcessingmethodCount;  /**< GPSProcessingMethod tag size */
    ULONG   AreainformationOffset;  /**< GPSAreaInformation tag offset */
    UINT32  AreainformationCount;   /**< GPSAreaInformation tag size */
    UINT8   Datestamp[11];          /**< GPSDateStamp */
    UINT16  Differential;           /**< GPSDifferential */
    UINT32  HPositioningErrorNum;   /**< Numerator of GPSHPositioningError */
    UINT32  HPositioningErrorDen;   /**< Denominator of GPSHPositioningError */
} SVC_EXIF_GPS_INFO_s;


/** TYPE of the second picture(screennail, ...)*/
#define SVC_FORMAT_SCRAPP2  1U  /**< SCRAAP2 */
#define SVC_FORMAT_SCRMPF   2U  /**< SCRMPF */
#define SVC_FORMAT_SCRFPXR  3U  /**< SCRFPXR */

/**
 * Exif private information
 */
typedef struct {
    EXIF_INFO_s ExifIfdInfo;   /**< Exif information (See EXIF_INFO_s.) */
    SVC_EXIF_GPS_INFO_s GpsIfdInfo;     /**< GPS information (See SVC_EXIF_GPS_INFO_s.) */
    UINT32 MakerNoteOff;  /**< The start offset of makernote in the file */
    UINT32 MakerNoteSize; /**< The size of makernote */
    UINT32 App1Off;       /**< The start offset of maker APP1 in the file */
    UINT32 App2Off;       /**< The start offset of maker APP2 in the file */
    UINT32 TiffBase;      /**< The start offset of Tiff header in the file */
    UINT32 Ifd0Off;       /**< The start offset of IFD0 structure in the file */
    UINT32 Ifd1Off;       /**< The start offset of IFD1 structure in the file */
    UINT32 ExifIfdOff;    /**< The start offset of ExifIfd structure in the file */
    UINT32 GpsIfdOff;     /**< The start offset of GpsIfd structure in the file */
    UINT32 SecPicType;    /**< The format type of the second picture (screennail) */
    UINT8 Thumbnail;      /**< The flag of the thumbnail in the image */
    UINT8 Screennail;     /**< The flag of the screennail in the image */
    UINT8 Fullview;       /**< The flag of the fullview in the image */
} SVC_EXIF_INFO_s;

#endif
