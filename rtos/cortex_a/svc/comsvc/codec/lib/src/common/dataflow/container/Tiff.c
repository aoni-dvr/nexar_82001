/**
 * @file Tiff.c
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
#include "Tiff.h"
#include "FormatAPI.h"

SVC_TIFF_TAG_s Ifd0Tag[SVC_IFD0_TOTAL_TAGS] = {
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL /*| TIFF_EXIF_NONCONFIGURABLE*/,   TIFF_LONG,  SVC_TIFF_ImageWidth,    1U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL /*| TIFF_EXIF_NONCONFIGURABLE*/,   TIFF_LONG,  SVC_TIFF_ImageHeight, 1U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL /*| TIFF_EXIF_NONCONFIGURABLE*/,   TIFF_SHORT,     SVC_TIFF_BitsPerSample, 3U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL /*| TIFF_EXIF_NONCONFIGURABLE*/,   TIFF_SHORT,     SVC_TIFF_Compression, 1U, 6U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL /*| TIFF_EXIF_NONCONFIGURABLE*/,   TIFF_SHORT,     SVC_TIFF_PhotometricInterpretation, 1U, 0U, NULL},
{SVC_TAG_ENABLED,   TIFF_EXIF_OPTIONAL,              TIFF_ASCII,     SVC_TIFF_ImageDescription, 14U, 0U, NULL},
{SVC_TAG_ENABLED,   TIFF_EXIF_OPTIONAL,              TIFF_ASCII,     SVC_TIFF_Make, 24U, 0U, NULL},
{SVC_TAG_ENABLED,   TIFF_EXIF_OPTIONAL,              TIFF_ASCII,     SVC_TIFF_Model, 24U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_LONG,  SVC_TIFF_StripOffsets, 1U, 0U, NULL},
{SVC_TAG_ENABLED,   TIFF_EXIF_OPTIONAL,              TIFF_SHORT,     SVC_TIFF_Orientation, 1U, 1U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL /*| TIFF_EXIF_NONCONFIGURABLE*/,   TIFF_SHORT,     SVC_TIFF_SamplesPerPixel, 1U, 3U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_SHORT,     SVC_TIFF_RowsPerStrip, 1U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_LONG,  SVC_TIFF_StripByteCounts, 1U, 0U, NULL},
{SVC_TAG_ENABLED,   TIFF_EXIF_MANDATORY,             TIFF_RATIONAL,  SVC_TIFF_XResolution, 1U, 0U, NULL},
{SVC_TAG_ENABLED,   TIFF_EXIF_MANDATORY,             TIFF_RATIONAL,  SVC_TIFF_YResolution, 1U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_SHORT,     SVC_TIFF_PlanarConfiguration, 1U, 0U, NULL},
{SVC_TAG_ENABLED,   TIFF_EXIF_MANDATORY,             TIFF_SHORT,     SVC_TIFF_ResolutionUnit, 1U, 2U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_SHORT,     SVC_TIFF_TransferFunction, 768U, 0U, NULL},
{SVC_TAG_ENABLED,   TIFF_EXIF_OPTIONAL,              TIFF_ASCII,     SVC_TIFF_Software, 12U, 0U, NULL},
{SVC_TAG_ENABLED,   TIFF_EXIF_OPTIONAL /*| TIFF_EXIF_NONCONFIGURABLE*/,   TIFF_ASCII,     SVC_TIFF_DateTime, 20U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_ASCII,     SVC_TIFF_Artist, 1U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_RATIONAL,  SVC_TIFF_WhitePoint, 2U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_RATIONAL,  SVC_TIFF_PrimaryChromaticities, 6U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_LONG,  SVC_TIFF_JPEGInterchangeFormat, 1U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_LONG,  SVC_TIFF_JPEGInterchangeFormatLength, 1U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_RATIONAL,  SVC_TIFF_YCbCrCoefficients, 3U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_SHORT,     SVC_TIFF_YCbCrSubSampling, 2U, 0U, NULL},
{SVC_TAG_ENABLED,   TIFF_EXIF_MANDATORY /*| TIFF_EXIF_NONCONFIGURABLE*/,  TIFF_SHORT,     SVC_TIFF_YCbCrPositioning, 1U, 1U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_RATIONAL,  SVC_TIFF_ReferenceBlackWhite, 6U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_ASCII,     SVC_TIFF_Copyright, 1U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL | TIFF_EXIF_NONCONFIGURABLE,   TIFF_UNDEFINED, SVC_TIFF_PIM    , 40U, 0U, NULL},
{SVC_TAG_ENABLED,   TIFF_EXIF_MANDATORY | TIFF_EXIF_NONCONFIGURABLE,  TIFF_LONG,  SVC_TIFF_ExifIFDPointer, 1U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL | TIFF_EXIF_NONCONFIGURABLE,   TIFF_LONG,  SVC_TIFF_GPSInfoIFDPointer, 1U, 0U, NULL}
};

SVC_TIFF_TAG_s Ifd1Tag[SVC_IFD1_TOTAL_TAGS] = {
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL /*| TIFF_EXIF_NONCONFIGURABLE*/,   TIFF_LONG,      SVC_TIFF_ImageWidth, 1U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL /*| TIFF_EXIF_NONCONFIGURABLE*/,   TIFF_LONG,  SVC_TIFF_ImageHeight, 1U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL /*| TIFF_EXIF_NONCONFIGURABLE*/,   TIFF_SHORT,     SVC_TIFF_BitsPerSample, 3U, 0U, NULL},
{SVC_TAG_ENABLED,   TIFF_EXIF_MANDATORY /*| TIFF_EXIF_NONCONFIGURABLE*/,  TIFF_SHORT,     SVC_TIFF_Compression, 1U, 6U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL /*| TIFF_EXIF_NONCONFIGURABLE*/,   TIFF_SHORT,     SVC_TIFF_PhotometricInterpretation, 1U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_ASCII,     SVC_TIFF_ImageDescription, 1U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_ASCII,     SVC_TIFF_Make, 24U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_ASCII,     SVC_TIFF_Model, 24U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_LONG,  SVC_TIFF_StripOffsets, 1U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_SHORT,     SVC_TIFF_Orientation, 1U, 1U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL /*| TIFF_EXIF_NONCONFIGURABLE*/,   TIFF_SHORT,     SVC_TIFF_SamplesPerPixel, 1U, 3U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_SHORT,     SVC_TIFF_RowsPerStrip, 1U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_LONG,  SVC_TIFF_StripByteCounts, 1U, 0U, NULL},
{SVC_TAG_ENABLED,   TIFF_EXIF_MANDATORY,             TIFF_RATIONAL,  SVC_TIFF_XResolution, 1U, 0U, NULL},
{SVC_TAG_ENABLED,   TIFF_EXIF_MANDATORY,             TIFF_RATIONAL,  SVC_TIFF_YResolution, 1U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_SHORT,     SVC_TIFF_PlanarConfiguration, 1U, 0U, NULL},
{SVC_TAG_ENABLED,   TIFF_EXIF_MANDATORY,             TIFF_SHORT,     SVC_TIFF_ResolutionUnit, 1U, 2U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_SHORT,     SVC_TIFF_TransferFunction, 768U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_ASCII,     SVC_TIFF_Software, 12U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL | TIFF_EXIF_NONCONFIGURABLE,   TIFF_ASCII,     SVC_TIFF_DateTime, 20U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_ASCII,     SVC_TIFF_Artist, 1U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_RATIONAL,  SVC_TIFF_WhitePoint, 2U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_RATIONAL,  SVC_TIFF_PrimaryChromaticities, 6U, 0U, NULL},
{SVC_TAG_ENABLED,   TIFF_EXIF_MANDATORY | TIFF_EXIF_NONCONFIGURABLE,  TIFF_LONG,  SVC_TIFF_JPEGInterchangeFormat, 1U, 0U, NULL},
{SVC_TAG_ENABLED,   TIFF_EXIF_MANDATORY | TIFF_EXIF_NONCONFIGURABLE,  TIFF_LONG,  SVC_TIFF_JPEGInterchangeFormatLength, 1U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_RATIONAL,  SVC_TIFF_YCbCrCoefficients, 3U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_SHORT,     SVC_TIFF_YCbCrSubSampling, 2U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL /*| TIFF_EXIF_NONCONFIGURABLE*/,   TIFF_SHORT,     SVC_TIFF_YCbCrPositioning, 1U, 1U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_RATIONAL,  SVC_TIFF_ReferenceBlackWhite, 6U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_ASCII,     SVC_TIFF_Copyright, 1U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL | TIFF_EXIF_NONCONFIGURABLE,   TIFF_LONG,  SVC_TIFF_ExifIFDPointer, 1U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL | TIFF_EXIF_NONCONFIGURABLE,   TIFF_LONG,  SVC_TIFF_GPSInfoIFDPointer, 1U, 0U, NULL}
};

SVC_TIFF_TAG_s ExifIfdTag[SVC_EXIF_TOTAL_TAGS] = {
{SVC_TAG_ENABLED,   TIFF_EXIF_OPTIONAL /*| TIFF_EXIF_NONCONFIGURABLE*/,   TIFF_RATIONAL,  SVC_EXIFIFD_ExposureTime, 1U, 0U, NULL},
{SVC_TAG_ENABLED,   TIFF_EXIF_OPTIONAL /*| TIFF_EXIF_NONCONFIGURABLE*/,   TIFF_RATIONAL,  SVC_EXIFIFD_FNumber, 1U, 0U, NULL},
{SVC_TAG_ENABLED,   TIFF_EXIF_OPTIONAL /*| TIFF_EXIF_NONCONFIGURABLE*/,   TIFF_SHORT,     SVC_EXIFIFD_ExposureProgram, 1U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_ASCII,     SVC_EXIFIFD_SpectralSensitivity, 1U, 0U, NULL},
{SVC_TAG_ENABLED,   TIFF_EXIF_OPTIONAL,              TIFF_SHORT,     SVC_EXIFIFD_ISOSpeedRatings, 1U, 0U, NULL}, /* ~2.21; PhohographicSentivity in 2.3 */
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_UNDEFINED,     SVC_EXIFIFD_OECF, 1U, 0U, NULL},
{SVC_TAG_ENABLED,   TIFF_EXIF_OPTIONAL,              TIFF_SHORT,     SVC_EXIFIFD_SensitivityType, 1U, 3U/*ISO Speed*/, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_LONG,  SVC_EXIFIFD_StandardOutputSensitivity, 1U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_LONG,  SVC_EXIFIFD_RecommendedExposureIndex, 1U, 0U, NULL},
{SVC_TAG_ENABLED,   TIFF_EXIF_OPTIONAL,              TIFF_LONG,  SVC_EXIFIFD_ISOSpeed, 1U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_LONG,  SVC_EXIFIFD_ISOSpeedLatitudeyyy, 1U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_LONG,  SVC_EXIFIFD_ISOSpeedLatitudezzz, 1U, 0U, NULL},
{SVC_TAG_ENABLED,   TIFF_EXIF_MANDATORY,             TIFF_UNDEFINED,     SVC_EXIFIFD_ExifVersion, 4U, 0x30323331U, NULL},
{SVC_TAG_ENABLED,   TIFF_EXIF_OPTIONAL,              TIFF_ASCII,     SVC_EXIFIFD_DateTimeOriginal, 20U, 0U, NULL},
{SVC_TAG_ENABLED,   TIFF_EXIF_OPTIONAL,              TIFF_ASCII,     SVC_EXIFIFD_DateTimeDigitized, 20U, 0U, NULL},
{SVC_TAG_ENABLED,   TIFF_EXIF_MANDATORY /*| TIFF_EXIF_NONCONFIGURABLE*/,  TIFF_UNDEFINED,     SVC_EXIFIFD_ComponentsConfiguration, 4U, 0U, NULL},
{SVC_TAG_ENABLED,   TIFF_EXIF_OPTIONAL /*| TIFF_EXIF_NONCONFIGURABLE*/,   TIFF_RATIONAL,  SVC_EXIFIFD_CompressedBitsPerPixel, 1U, 0U, NULL},
{SVC_TAG_ENABLED,   TIFF_EXIF_OPTIONAL /*| TIFF_EXIF_NONCONFIGURABLE*/,   TIFF_SRATIONAL,     SVC_EXIFIFD_ShutterSpeedValue, 1U, 0U, NULL},
{SVC_TAG_ENABLED,   TIFF_EXIF_OPTIONAL /*| TIFF_EXIF_NONCONFIGURABLE*/,   TIFF_RATIONAL,  SVC_EXIFIFD_ApertureValue, 1U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_SRATIONAL,     SVC_EXIFIFD_BrightnessValue, 1U, 0U, NULL},
{SVC_TAG_ENABLED,   TIFF_EXIF_OPTIONAL /*| TIFF_EXIF_NONCONFIGURABLE*/,   TIFF_SRATIONAL,     SVC_EXIFIFD_ExposureBiasValue, 1U, 0U, NULL},
{SVC_TAG_ENABLED,   TIFF_EXIF_OPTIONAL /*| TIFF_EXIF_NONCONFIGURABLE*/,   TIFF_RATIONAL,  SVC_EXIFIFD_MaxApertureValue, 1U, 0U, NULL},
{SVC_TAG_ENABLED,   TIFF_EXIF_OPTIONAL /*| TIFF_EXIF_NONCONFIGURABLE*/,   TIFF_RATIONAL,  SVC_EXIFIFD_SubjectDistance, 1U, 0U, NULL},
{SVC_TAG_ENABLED,   TIFF_EXIF_OPTIONAL /*| TIFF_EXIF_NONCONFIGURABLE*/,   TIFF_SHORT,     SVC_EXIFIFD_MeteringMode, 1U, 0U, NULL},
{SVC_TAG_ENABLED,   TIFF_EXIF_OPTIONAL /*| TIFF_EXIF_NONCONFIGURABLE*/,   TIFF_SHORT,     SVC_EXIFIFD_LightSource, 1U, 0U, NULL},
{SVC_TAG_ENABLED,   TIFF_EXIF_OPTIONAL /*| TIFF_EXIF_NONCONFIGURABLE*/,   TIFF_SHORT,     SVC_EXIFIFD_Flash, 1U, 0U, NULL},
{SVC_TAG_ENABLED,   TIFF_EXIF_OPTIONAL /*| TIFF_EXIF_NONCONFIGURABLE*/,   TIFF_RATIONAL,  SVC_EXIFIFD_FocalLength, 1U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_SHORT,     SVC_EXIFIFD_SubjectArea, 2U, 0U, NULL},
{SVC_TAG_ENABLED,   TIFF_EXIF_OPTIONAL,              TIFF_UNDEFINED,     SVC_EXIFIFD_MakerNote, 0U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_UNDEFINED,     SVC_EXIFIFD_UserComment, 1U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_ASCII,     SVC_EXIFIFD_SubSecTime, 1U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_ASCII,     SVC_EXIFIFD_SubSecTimeOriginal, 1U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_ASCII,     SVC_EXIFIFD_SubSecTimeDigitized, 1U, 0U, NULL},
{SVC_TAG_ENABLED,   TIFF_EXIF_MANDATORY /*| TIFF_EXIF_NONCONFIGURABLE*/,  TIFF_UNDEFINED,     SVC_EXIFIFD_FlashpixVersion, 4U, 0U, NULL},
{SVC_TAG_ENABLED,   TIFF_EXIF_MANDATORY /*| TIFF_EXIF_NONCONFIGURABLE*/,  TIFF_SHORT,     SVC_EXIFIFD_ColorSpace, 1U, 0U, NULL},
{SVC_TAG_ENABLED,   TIFF_EXIF_MANDATORY /*| TIFF_EXIF_NONCONFIGURABLE*/,  TIFF_LONG,  SVC_EXIFIFD_PixelXDimension, 1U, 0U, NULL},
{SVC_TAG_ENABLED,   TIFF_EXIF_MANDATORY /*| TIFF_EXIF_NONCONFIGURABLE*/,  TIFF_LONG,  SVC_EXIFIFD_PixelYDimension, 1U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_ASCII,     SVC_EXIFIFD_RelatedSoundFile, 13U, 0U, NULL},
{SVC_TAG_ENABLED,   TIFF_EXIF_MANDATORY | TIFF_EXIF_NONCONFIGURABLE,  TIFF_LONG,  SVC_EXIFIFD_InteroperabilityIFDPointer, 1U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_RATIONAL,  SVC_EXIFIFD_FlashEnergy, 1U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_UNDEFINED,     SVC_EXIFIFD_SpatialFrequencyResponse, 1U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_RATIONAL,  SVC_EXIFIFD_FocalPlaneXResolution, 1U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_RATIONAL,  SVC_EXIFIFD_FocalPlaneYResolution, 1U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_SHORT,     SVC_EXIFIFD_FocalPlaneResolutionUnit, 1U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_SHORT,     SVC_EXIFIFD_SubjectLocation, 2U, 0U, NULL},
{SVC_TAG_ENABLED,   TIFF_EXIF_OPTIONAL /*| TIFF_EXIF_NONCONFIGURABLE*/,   TIFF_RATIONAL,  SVC_EXIFIFD_ExposureIndex, 1U, 0U, NULL},
{SVC_TAG_ENABLED,   TIFF_EXIF_OPTIONAL /*| TIFF_EXIF_NONCONFIGURABLE*/,   TIFF_SHORT,     SVC_EXIFIFD_SensingMethod, 1U, 0U, NULL},
{SVC_TAG_ENABLED,   TIFF_EXIF_OPTIONAL /*| TIFF_EXIF_NONCONFIGURABLE*/,   TIFF_UNDEFINED,     SVC_EXIFIFD_FileSource, 1U, 0U, NULL},
{SVC_TAG_ENABLED,   TIFF_EXIF_OPTIONAL /*| TIFF_EXIF_NONCONFIGURABLE*/,   TIFF_UNDEFINED,     SVC_EXIFIFD_SceneType, 1U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_UNDEFINED,     SVC_EXIFIFD_CFAPattern, 1U, 0U, NULL},
{SVC_TAG_ENABLED,   TIFF_EXIF_OPTIONAL /*| TIFF_EXIF_NONCONFIGURABLE*/,   TIFF_SHORT,     SVC_EXIFIFD_CustomRendered, 1U, 0U, NULL},
{SVC_TAG_ENABLED,   TIFF_EXIF_OPTIONAL /*| TIFF_EXIF_NONCONFIGURABLE*/,   TIFF_SHORT,     SVC_EXIFIFD_ExposureMode, 1U, 0U, NULL},
{SVC_TAG_ENABLED,   TIFF_EXIF_OPTIONAL /*| TIFF_EXIF_NONCONFIGURABLE*/,   TIFF_SHORT,     SVC_EXIFIFD_WhiteBalance, 1U, 0U, NULL},
{SVC_TAG_ENABLED,   TIFF_EXIF_OPTIONAL /*| TIFF_EXIF_NONCONFIGURABLE*/,   TIFF_RATIONAL,  SVC_EXIFIFD_DigitalZoomRatio, 1U, 0U, NULL},
{SVC_TAG_ENABLED,   TIFF_EXIF_OPTIONAL /*| TIFF_EXIF_NONCONFIGURABLE*/,   TIFF_SHORT,     SVC_EXIFIFD_FocalLengthIn35mmFilm, 1U, 0U, NULL},
{SVC_TAG_ENABLED,   TIFF_EXIF_OPTIONAL /*| TIFF_EXIF_NONCONFIGURABLE*/,   TIFF_SHORT,     SVC_EXIFIFD_SceneCaptureType, 1U, 0U, NULL},
{SVC_TAG_ENABLED,   TIFF_EXIF_OPTIONAL /*| TIFF_EXIF_NONCONFIGURABLE*/,   TIFF_SHORT,     SVC_EXIFIFD_GainControl, 1U, 0U, NULL},
{SVC_TAG_ENABLED,   TIFF_EXIF_OPTIONAL /*| TIFF_EXIF_NONCONFIGURABLE*/,   TIFF_SHORT,     SVC_EXIFIFD_Contrast, 1U, 0U, NULL},
{SVC_TAG_ENABLED,   TIFF_EXIF_OPTIONAL /*| TIFF_EXIF_NONCONFIGURABLE*/,   TIFF_SHORT,     SVC_EXIFIFD_Saturation, 1U, 0U, NULL},
{SVC_TAG_ENABLED,   TIFF_EXIF_OPTIONAL /*| TIFF_EXIF_NONCONFIGURABLE*/,   TIFF_SHORT,     SVC_EXIFIFD_Sharpness, 1U, 0U, NULL},
{SVC_TAG_ENABLED,   TIFF_EXIF_OPTIONAL,              TIFF_UNDEFINED, SVC_EXIFIFD_DeviceSettingDescription, 1U, 0U, NULL},
{SVC_TAG_ENABLED,   TIFF_EXIF_OPTIONAL /*| TIFF_EXIF_NONCONFIGURABLE*/,   TIFF_SHORT,     SVC_EXIFIFD_SubjectDistanceRange, 1U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_ASCII,     SVC_EXIFIFD_ImageUniqueID, 33U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_ASCII,     SVC_EXIFIFD_CameraOwnerName, 1U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_ASCII,     SVC_EXIFIFD_BodySerialNumber, 1U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_RATIONAL,  SVC_EXIFIFD_LensSpecification, 4U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_ASCII,     SVC_EXIFIFD_LensMake, 1U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_ASCII,     SVC_EXIFIFD_LensModel, 1U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_ASCII,     SVC_EXIFIFD_LensSerialNumber, 1U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_RATIONAL,  SVC_EXIFIFD_Gamma, 1U, 0U, NULL}
};

SVC_TIFF_TAG_s IntIfdTag[SVC_IntIFD_TOTAL_TAGS] = {
{SVC_TAG_ENABLED,   TIFF_EXIF_OPTIONAL,              TIFF_ASCII,     SVC_IntIFD_InteroperabilityIndex, 4U, 0x52393800U, NULL},
{SVC_TAG_ENABLED,   TIFF_EXIF_OPTIONAL | TIFF_EXIF_COUNT_NONCONFIG,   TIFF_UNDEFINED, SVC_IntIFD_InteroperabilityVersion, 4U, 0x30313030U, NULL}
};
SVC_TIFF_TAG_s GpsIfdTag[SVC_GPS_TOTAL_TAGS] = {
{SVC_TAG_ENABLED,  TIFF_EXIF_MANDATORY | TIFF_EXIF_COUNT_NONCONFIG,  TIFF_BYTE,  SVC_GPSIFD_GPSVersionID, 4U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL | TIFF_EXIF_COUNT_NONCONFIG,   TIFF_ASCII,     SVC_GPSIFD_GPSLatitudeRef, 2U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL | TIFF_EXIF_COUNT_NONCONFIG,   TIFF_RATIONAL,  SVC_GPSIFD_GPSLatitude, 3U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL | TIFF_EXIF_COUNT_NONCONFIG,   TIFF_ASCII,     SVC_GPSIFD_GPSLongitudeRef, 2U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL | TIFF_EXIF_COUNT_NONCONFIG,   TIFF_RATIONAL,  SVC_GPSIFD_GPSLongitude, 3U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL | TIFF_EXIF_COUNT_NONCONFIG,   TIFF_BYTE,     SVC_GPSIFD_GPSAltitudeRef, 1U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL | TIFF_EXIF_COUNT_NONCONFIG,   TIFF_RATIONAL,  SVC_GPSIFD_GPSAltitude, 1U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL | TIFF_EXIF_COUNT_NONCONFIG,   TIFF_RATIONAL,  SVC_GPSIFD_GPSTimeStamp, 3U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_ASCII,     SVC_GPSIFD_GPSSatellites, 0U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL | TIFF_EXIF_COUNT_NONCONFIG,   TIFF_ASCII,     SVC_GPSIFD_GPSStatus, 2U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL | TIFF_EXIF_COUNT_NONCONFIG,   TIFF_ASCII,     SVC_GPSIFD_GPSMeasureMode, 2U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL | TIFF_EXIF_COUNT_NONCONFIG,   TIFF_RATIONAL,  SVC_GPSIFD_GPSDOP, 1U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL | TIFF_EXIF_COUNT_NONCONFIG,   TIFF_ASCII,     SVC_GPSIFD_GPSSpeedRef, 2U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL | TIFF_EXIF_COUNT_NONCONFIG,   TIFF_RATIONAL,  SVC_GPSIFD_GPSSpeed, 1U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL | TIFF_EXIF_COUNT_NONCONFIG,   TIFF_ASCII,     SVC_GPSIFD_GPSTrackRef, 2U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL | TIFF_EXIF_COUNT_NONCONFIG,   TIFF_RATIONAL,  SVC_GPSIFD_GPSTrack, 1U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL | TIFF_EXIF_COUNT_NONCONFIG,   TIFF_ASCII,     SVC_GPSIFD_GPSImgDirectionRef, 2U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL | TIFF_EXIF_COUNT_NONCONFIG,   TIFF_RATIONAL,  SVC_GPSIFD_GPSImgDirection, 1U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_ASCII,     SVC_GPSIFD_GPSMapDatum, 0U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL | TIFF_EXIF_COUNT_NONCONFIG,   TIFF_ASCII,     SVC_GPSIFD_GPSDestLatitudeRef, 2U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL | TIFF_EXIF_COUNT_NONCONFIG,   TIFF_RATIONAL,  SVC_GPSIFD_GPSDestLatitude, 3U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL | TIFF_EXIF_COUNT_NONCONFIG,   TIFF_ASCII,     SVC_GPSIFD_GPSDestLongitudeRef, 2U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL | TIFF_EXIF_COUNT_NONCONFIG,   TIFF_RATIONAL,  SVC_GPSIFD_GPSDestLongitude, 3U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL | TIFF_EXIF_COUNT_NONCONFIG,   TIFF_ASCII,     SVC_GPSIFD_GPSDestBearingRef, 2U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL | TIFF_EXIF_COUNT_NONCONFIG,   TIFF_RATIONAL,  SVC_GPSIFD_GPSDestBearing, 1U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL | TIFF_EXIF_COUNT_NONCONFIG,   TIFF_ASCII,     SVC_GPSIFD_GPSDestDistanceRef, 2U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL | TIFF_EXIF_COUNT_NONCONFIG,   TIFF_RATIONAL,  SVC_GPSIFD_GPSDestDistance, 1U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_UNDEFINED, SVC_GPSIFD_GPSProcessingMethod, 0U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_UNDEFINED, SVC_GPSIFD_GPSAreaInformation, 0U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL | TIFF_EXIF_COUNT_NONCONFIG,   TIFF_ASCII, SVC_GPSIFD_GPSDateStamp, 11U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL | TIFF_EXIF_COUNT_NONCONFIG,   TIFF_SHORT, SVC_GPSIFD_GPSDifferential, 1U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL | TIFF_EXIF_COUNT_NONCONFIG,   TIFF_RATIONAL,  SVC_GPSIFD_GPSHPositioningError, 1U, 0U, NULL},
};
#if 0
SVC_TIFF_TAG_s IptcTag[SVC_IPTC_TOTAL_TAGS] = {
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_ASCII,     SVC_IPTC_ObjectName, 0U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL | TIFF_EXIF_COUNT_NONCONFIG,   TIFF_ASCII,     SVC_IPTC_Urgent,     1U, 0x38U, NULL}, /* 0x31: high, 0x36: normal, 0x38: low*/
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_ASCII,     IPTC_Categories, 0U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_ASCII,     IPTC_SupplementalCategories, 0U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_ASCII,     IPTC_Keywords, 0U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_ASCII,     IPTC_SpecialInstructions, 0U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL | TIFF_EXIF_COUNT_NONCONFIG,   TIFF_ASCII,     IPTC_DateCreated, 8U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_ASCII,     IPTC_Byline, 0U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_ASCII,     IPTC_BylineTitle, 0U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_ASCII,     IPTC_City, 0U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_ASCII,     IPTC_Sublocation, 0U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_ASCII,     IPTC_ProvinceState, 0U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_ASCII,     IPTC_Country, 0U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_ASCII,     IPTC_OriginalTransmissionReference, 0U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_ASCII,     IPTC_Headline, 0U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_ASCII,     IPTC_Credits, 0U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_ASCII,     IPTC_Source, 0U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_ASCII,     IPTC_Copyright, 0U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_ASCII,     IPTC_Caption, 0U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_ASCII,     IPTC_CaptionWriter, 0U, 0U, NULL},
};

SVC_TIFF_TAG_s MpidxTag[SVC_MPIDX_TOTAL_TAGS] = {
{SVC_TAG_DISABLED,  TIFF_EXIF_MANDATORY | TIFF_EXIF_NONCONFIGURABLE,  TIFF_UNDEFINED, MPIFD_MPFVersion, 4U, 0x30313030U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_MANDATORY | TIFF_EXIF_NONCONFIGURABLE,  TIFF_LONG,  MPIFD_NumberOfImages, 1U, 2U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_MANDATORY | TIFF_EXIF_NONCONFIGURABLE,  TIFF_UNDEFINED, MPIFD_MPEntry, 32U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_MANDATORY,             TIFF_UNDEFINED, MPIFD_ImageUIDList, 66U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_MANDATORY | TIFF_EXIF_NONCONFIGURABLE,  TIFF_LONG,  MPIFD_TotalFrames, 1U, 1U, NULL},
};

SVC_TIFF_TAG_s MpattrTag[SVC_MPATTR_TOTAL_TAGS] = {
{SVC_TAG_DISABLED,  TIFF_EXIF_MANDATORY | TIFF_EXIF_NONCONFIGURABLE,  TIFF_UNDEFINED, MPIFD_MPFVersion, 4U, 0x30313030U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_LONG,  MPIFD_MPIndividualNum, 1U, 1U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_LONG,  MPIFD_PanOrientation, 1U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_RATIONAL,  MPIFD_PanOverlap_H, 1U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_RATIONAL,  MPIFD_PanOverlap_V, 1U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_LONG,  MPIFD_BaseViewpointNum, 1U, 1U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_SRATIONAL, MPIFD_CovergenceAngle, 1U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_RATIONAL,  MPIFD_BaselineLength, 1U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_SRATIONAL, MPIFD_VerticalDivergence, 1U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_SRATIONAL, MPIFD_AxisDistance_X, 1U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_SRATIONAL, MPIFD_AxisDistance_Y, 1U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_SRATIONAL, MPIFD_AxisDistance_Z, 1U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_SRATIONAL, MPIFD_YawAngle, 1U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_SRATIONAL, MPIFD_PitchAngle, 1U, 0U, NULL},
{SVC_TAG_DISABLED,  TIFF_EXIF_OPTIONAL,              TIFF_SRATIONAL, MPIFD_RollAngle, 1U, 0U, NULL}
};
#endif
/**
 * Function pointer of data operation for different endian order
 */
UINT32 (*SvcFormat_PutMem16)(UINT32 *Offset, UINT8 *Base, UINT32 Size, UINT16 Code); /**< The function pointer to put 2 bytes data */
UINT32 (*SvcFormat_PutMem32)(UINT32 *Offset, UINT8 *Base, UINT32 Size, UINT32 Code); /**< The function pointer to put 4 bytes data */
UINT32 (*SvcFormat_GetMem16)(SVC_MEM_OP_s *Ctx, UINT16 *Buffer); /**< The function pointer to get 2 bytes data */
UINT32 (*SvcFormat_GetMem32)(SVC_MEM_OP_s *Ctx, UINT32 *Buffer);  /**< The function pointer to get 4 bytes data */
void (*SvcFormat_Convert64)(UINT8 *Buffer, UINT32 Num, UINT32 Deno); /**< The function pointer to convert 8 bytes data */

/**
 * Convert the data to big endian order for rational type tags.
 *
 * @param [in,out] Buffer The target buffer
 * @param [in] Upper32 The upper 4 bytes
 * @param [in] Lower32 The lower 4 bytes
 */
static void SvcFormat_ConvertTo_Be64(UINT8 *Buffer, UINT32 Upper32, UINT32 Lower32)
{
    Buffer[0] = (UINT8)((Upper32 & 0xFF000000U) >> 24);
    Buffer[1] = (UINT8)((Upper32 & 0xFF0000U) >> 16);
    Buffer[2] = (UINT8)((Upper32 & 0xFF00U) >> 8);
    Buffer[3] = (UINT8)(Upper32 & 0xFFU);

    Buffer[4] = (UINT8)((Lower32 & 0xFF000000U) >> 24);
    Buffer[5] = (UINT8)((Lower32 & 0xFF0000U) >> 16);
    Buffer[6] = (UINT8)((Lower32 & 0xFF00U) >> 8);
    Buffer[7] = (UINT8)(Lower32 & 0xFFU);
}

/**
 * Convert the data to little endian order for rational type tags.
 *
 * @param [in,out] Buffer The target buffer
 * @param [in] Upper32 The upper 4 bytes
 * @param [in] Lower32 The lower 4 bytes
 */
static void SvcFormat_ConvertTo_Le64(UINT8 *Buffer, UINT32 Upper32, UINT32 Lower32)
{
    Buffer[0] = (UINT8)(Upper32 & 0xFFU);
    Buffer[1] = (UINT8)((Upper32 & 0xFF00U) >> 8);
    Buffer[2] = (UINT8)((Upper32 & 0xFF0000U) >> 16);
    Buffer[3] = (UINT8)((Upper32 & 0xFF000000U) >> 24);

    Buffer[4] = (UINT8)(Lower32 & 0xFFU);
    Buffer[5] = (UINT8)((Lower32 & 0xFF00U) >> 8);
    Buffer[6] = (UINT8)((Lower32 & 0xFF0000U) >> 16);
    Buffer[7] = (UINT8)((Lower32 & 0xFF000000U) >> 24);
}

/**
 * Get the exif TIFF header
 *
 * @param [in] Ctx The context of memory operation
 * @param [out] Endian 0 - Big endian, 1 - Little Endian
 * @return FORMAT_OK => ok, others => NG
 */
UINT32 SvcFormat_GetTiffHeader(SVC_MEM_OP_s *Ctx, UINT8 *Endian)
{
    UINT16 Temp16 = 0;
    UINT32 Rval = SvcFormat_GetMemBe16(Ctx, &Temp16);
    if (Rval == FORMAT_OK) {
        if (Temp16 == Marker_MM) {    /* Big Endian */
            SvcFormat_GetMem16 = SvcFormat_GetMemBe16;
            SvcFormat_GetMem32 = SvcFormat_GetMemBe32;
            *Endian = SVC_FORMAT_EXIF_BIGENDIAN;
        } else {                    /* Little Endian */
            SvcFormat_GetMem16 = SvcFormat_GetMemLe16;
            SvcFormat_GetMem32 = SvcFormat_GetMemLe32;
            *Endian = SVC_FORMAT_EXIF_LITTLEENDIAN;
        }
        Rval = SvcFormat_GetMem16(Ctx, &Temp16);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetMem16() failed!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetMem16() failed!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 * Set the endia order of exif tags
 *
 * @param [in] Endian The endian order to be set
 */
void SvcFormat_SetTiffHeader(UINT8 Endian)
{
    if (Endian == SVC_FORMAT_EXIF_BIGENDIAN) {
        SvcFormat_PutMem16 = SvcFormat_PutMemBe16;
        SvcFormat_PutMem32 = SvcFormat_PutMemBe32;
        SvcFormat_Convert64 = SvcFormat_ConvertTo_Be64;
    } else {
        SvcFormat_PutMem16 = SvcFormat_PutMemLe16;
        SvcFormat_PutMem32 = SvcFormat_PutMemLe32;
        SvcFormat_Convert64 = SvcFormat_ConvertTo_Le64;
    }
}

/**
 * Put the exif tag data to memory (Type : ASCII / BYTE / UNDEFINED)
 *
 * @param [in] Offset The current offset of memory buffer pointer
 * @param [in] Base The start address of memory buffer pointer
 * @param [in] Size The size of memory buffer * @param [in] Count The count of the tag
 * @param [in] Param The parameter to be put
 * @return FORMAT_OK => ok, others => NG
 */
static UINT32 SvcFormat_PutIfdEntry_1byte(UINT32 *Offset, UINT8 *Base, UINT32 Size, UINT32 Count, UINT32 Param)
{
    UINT32 Rval;
    if (Count <= 4U) {
        Rval = SvcFormat_PutMemByte(Offset, Base, Size, (UINT8)((Param >> 24U) & 0xffU));
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_PutMemByte(Offset, Base, Size, (UINT8)((Param >> 16U) & 0xffU));
            if (Rval == FORMAT_OK) {
                Rval = SvcFormat_PutMemByte(Offset, Base, Size, (UINT8)((Param >> 8U) & 0xffU));
                if (Rval == FORMAT_OK) {
                    Rval = SvcFormat_PutMemByte(Offset, Base, Size, (UINT8)((Param & 0xffU)));
                    if (Rval != FORMAT_OK) {
                        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s AmpFormat_PutMemByte() failed!", __func__, NULL, NULL, NULL, NULL);
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s AmpFormat_PutMemByte() failed!", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s AmpFormat_PutMemByte() failed!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s AmpFormat_PutMemByte() failed!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        Rval = SvcFormat_PutMem32(Offset, Base, Size, Param);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PutMem32() failed!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Rval;
}

/**
 * Put the exif tag data to memory (Type : SHORT)
 *
 * @param [in] Offset The current offset of memory buffer pointer
 * @param [in] Base The start address of memory buffer pointer
 * @param [in] Size The size of memory buffer
 * @param [in] Param The parameter to be put
 * @return FORMAT_OK => ok, others => NG
 */
static UINT32 SvcFormat_PutIfdEntry_2byte(UINT32 *Offset, UINT8 *Base, UINT32 Size, UINT32 Param)
{
    UINT32 Rval;
    Rval = SvcFormat_PutMem16(Offset, Base, Size, (UINT16)Param);
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutMem16(Offset, Base, Size, 0);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PutMem16() failed!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PutMem16() failed!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 * Put the exif tag entris to memory
 *
 * @param [in] Offset The current offset of memory buffer pointer
 * @param [in] Base The start address of memory buffer pointer
 * @param [in] Size The size of memory buffer
 * @param [in] Tag The id of the tag
 * @param [in] Type The type of the tag
 * @param [in] Count The count of the tag
 * @param [in] Param The parameter to be put
 * @return FORMAT_OK => ok, others => NG
 */
UINT32 SvcFormat_PutIfdEntry(UINT32 *Offset, UINT8 *Base, UINT32 Size, UINT16 Tag,
        UINT16 Type, UINT32 Count, UINT32 Param)
{
    UINT32 Rval;
    Rval = SvcFormat_PutMem16(Offset, Base, Size, Tag);
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutMem16(Offset, Base, Size, Type);
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_PutMem32(Offset, Base, Size, Count);
            if (Rval == FORMAT_OK) {
                switch (Type) {
                case TIFF_BYTE:
                case TIFF_ASCII:
                case TIFF_UNDEFINED:
                    Rval = SvcFormat_PutIfdEntry_1byte(Offset, Base, Size, Count, Param);
                    break;
                case TIFF_SHORT:
                    Rval = SvcFormat_PutIfdEntry_2byte(Offset, Base, Size, Param);
                    break;
                default:
                    Rval = SvcFormat_PutMem32(Offset, Base, Size, Param);
                    break;
                }
                if (Rval != FORMAT_OK) {
                    AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PutIfdEntry() failed!", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PutMem32() failed!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PutMem16() failed!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PutMem16() failed!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 * Put the exif tag entries to memory (Type : SHORT, using when data size < 5 bytes)
 *
 * @param [in] Offset The current offset of memory buffer pointer
 * @param [in] Base The start address of memory buffer pointer
 * @param [in] Size The size of memory buffer * @param [in] Tag The id of the tag
 * @param [in] Type The type of the tag
 * @param [in] Count The count of the tag
 * @param [in] Upper16 The first 2 bytes data
 * @param [in] Down16 The last 2 bytes data
 * @return FORMAT_OK => ok, others => NG
 */
static UINT32 SvcFormat_PutIfdEntry16(UINT32 *Offset, UINT8 *Base, UINT32 Size, UINT16 Tag,
        UINT16 Type, UINT32 Count, UINT16 Upper16, UINT16 Down16)
{
    UINT32 Rval;
    Rval = SvcFormat_PutMem16(Offset, Base, Size, Tag);
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutMem16(Offset, Base, Size, Type);
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_PutMem32(Offset, Base, Size, Count);
            if (Rval == FORMAT_OK) {
                Rval = SvcFormat_PutMem16(Offset, Base, Size, Upper16);
                if (Rval == FORMAT_OK) {
                    Rval = SvcFormat_PutMem16(Offset, Base, Size, Down16);
                    if (Rval != FORMAT_OK) {
                        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PutMem16() failed!", __func__, NULL, NULL, NULL, NULL);
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PutMem16() failed!", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PutMem32() failed!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PutMem16() failed!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PutMem16() failed!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 * Put the exif tag information to memory, including entries and data. (Type : ASCII / BYTE / UNDEFINED)
 *
 * @param [in] Tag The target tag
 * @param [in] Buffer The tag buffer status
 * @param [in] HeadSize The header size
 * @param [in] Data The data to be put
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcFormat_Put1Byte(const SVC_TIFF_TAG_s *Tag, SVC_TIFF_TAG_BUFFER_s *Buffer, UINT32 HeadSize, const UINT8 *Data)
{
    UINT32 Rval;
    /* ASCII, BYTE, UNDEFINED  (1 byte) */
    if (Tag->Count < 5U) {
        Rval = SvcFormat_PutIfdEntry(&Buffer->HeadOffset, Buffer->HeadBase, Buffer->HeadSize, Tag->Tag, Tag->Type, Tag->Count, Tag->Value);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PutIfdEntry() failed!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        Rval = SvcFormat_PutIfdEntry(&Buffer->HeadOffset, Buffer->HeadBase, Buffer->HeadSize, Tag->Tag, Tag->Type, Tag->Count, Buffer->InfoOffset + HeadSize - 12U);
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_WriteMem(&Buffer->InfoOffset, Buffer->InfoBase, Buffer->InfoSize, Tag->Count, Data);
            if (Rval != FORMAT_OK) {
                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_WriteMem() failed!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PutIfdEntry() failed!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Rval;
}

/**
 * Put the exif tag information to memory, including entries and data. (Type : SHORT)
 *
 * @param [in] Tag The target tag
 * @param [in] Buffer The tag buffer status
 * @param [in] HeadSize The header size
 * @param [in] Data The data to be put
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcFormat_Put2Byte(const SVC_TIFF_TAG_s *Tag, SVC_TIFF_TAG_BUFFER_s *Buffer, UINT32 HeadSize, const UINT8 *Data)
{
    UINT32 Rval;
    /* SHORT (2 bytes) */
    if (Tag->Count == 1U) {
        Rval = SvcFormat_PutIfdEntry16(&Buffer->HeadOffset, Buffer->HeadBase, Buffer->HeadSize, Tag->Tag, Tag->Type, Tag->Count, (UINT16)Tag->Value, 0U);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PutIfdEntry16() failed!", __func__, NULL, NULL, NULL, NULL);
        }
    } else if (Tag->Count == 2U) {
        Rval = SvcFormat_PutIfdEntry16(&Buffer->HeadOffset, Buffer->HeadBase, Buffer->HeadSize, Tag->Tag, Tag->Type, Tag->Count, (UINT16)((Tag->Value & 0xFFFF0000U) >> 16), (UINT16)(Tag->Value & 0xFFFFU));
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PutIfdEntry16() failed!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        Rval = SvcFormat_PutIfdEntry(&Buffer->HeadOffset, Buffer->HeadBase, Buffer->HeadSize, Tag->Tag, Tag->Type, Tag->Count, Buffer->InfoOffset + HeadSize - 12U);
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_WriteMem(&Buffer->InfoOffset, Buffer->InfoBase, Buffer->InfoSize, Tag->Count * 2U, Data);
            if (Rval != FORMAT_OK) {
                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_WriteMem() failed!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PutIfdEntry() failed!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Rval;
}

/**
 * Put the exif tag information to memory, including entries and data. (Type : LONG)
 *
 * @param [in] Tag The target tag
 * @param [in] Buffer The tag buffer status
 * @param [in] HeadSize The header size
 * @param [in] Data The data to be put
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcFormat_Put4Byte(const SVC_TIFF_TAG_s *Tag, SVC_TIFF_TAG_BUFFER_s *Buffer, UINT32 HeadSize, const UINT8 *Data)
{
    UINT32 Rval;
    /* LONG (4 bytes) */
    if (Tag->Count == 1U) {
        Rval = SvcFormat_PutIfdEntry(&Buffer->HeadOffset, Buffer->HeadBase, Buffer->HeadSize, Tag->Tag, Tag->Type, Tag->Count, Tag->Value);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PutIfdEntry() failed!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        Rval = SvcFormat_PutIfdEntry(&Buffer->HeadOffset, Buffer->HeadBase, Buffer->HeadSize, Tag->Tag, Tag->Type, Tag->Count, Buffer->InfoOffset + HeadSize - 12U);
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_WriteMem(&Buffer->InfoOffset, Buffer->InfoBase, Buffer->InfoSize, Tag->Count * 4U, Data);
            if (Rval != FORMAT_OK) {
                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_WriteMem() failed!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PutIfdEntry() failed!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Rval;
}

/**
 * Put the exif tag information to memory, including entries and data. (Type : RATIONAL / SRATIONAL)
 *
 * @param [in] Tag The target tag
 * @param [in] Buffer The tag buffer status
 * @param [in] HeadSize The header size
 * @param [in] Data The data to be put
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcFormat_Put8Byte(const SVC_TIFF_TAG_s *Tag, SVC_TIFF_TAG_BUFFER_s *Buffer, UINT32 HeadSize, const UINT8 *Data)
{
    UINT32 Rval;
    /* RATIONAL, SRATIONAL(8 bytes) */
    Rval = SvcFormat_PutIfdEntry(&Buffer->HeadOffset, Buffer->HeadBase, Buffer->HeadSize, Tag->Tag, Tag->Type, Tag->Count, Buffer->InfoOffset + HeadSize - 12U);
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_WriteMem(&Buffer->InfoOffset, Buffer->InfoBase, Buffer->InfoSize, Tag->Count * 8U, Data);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_WriteMem() failed!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PutIfdEntry() failed!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 * Get the exif tag entries from memory (Type : ASCII / BYTE / UNDEFINED)
 *
 * @param [in] Ctx The context of memory operation
 * @param [in] Count The count of the tag
 * @param [in,out] Param The parameter of the tag
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
static UINT32 SvcFormat_GetIfdEntry_1byte(SVC_MEM_OP_s *Ctx, UINT32 Count, UINT32 *Param)
{
    UINT32 Rval;
    UINT32 Temp32;
    UINT8 Temp8 = 0U;
    if (Count <= 4U) {
        Rval = SvcFormat_GetMemByte(Ctx, &Temp8);
        if (Rval == FORMAT_OK) {
            Temp32 = (UINT32)Temp8;
            Rval = SvcFormat_GetMemByte(Ctx, &Temp8);
            if (Rval == FORMAT_OK) {
                Temp32 = (Temp32<<8) + Temp8;
                Rval = SvcFormat_GetMemByte(Ctx, &Temp8);
                if (Rval == FORMAT_OK) {
                    Temp32 = (Temp32<<8) + Temp8;
                    Rval = SvcFormat_GetMemByte(Ctx, &Temp8);
                    if (Rval == FORMAT_OK) {
                         *Param = (Temp32<<8) + Temp8;
                    } else {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetMemByte() failed!", __func__, NULL, NULL, NULL, NULL);
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetMemByte() failed!", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetMemByte() failed!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetMemByte() failed!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        Rval = SvcFormat_GetMem32(Ctx, Param);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetMemByte() failed!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Rval;
}

/**
 * Get the exif tag entries from memory (Type : SHORT)
 *
 * @param [in] Ctx The context of memory operation
 * @param [in] Count The count of the tag
 * @param [in,out] Param The parameter of the tag
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
static UINT32 SvcFormat_GetIfdEntry_2byte(SVC_MEM_OP_s *Ctx, UINT32 Count, UINT32 *Param)
{
    UINT32 Rval;
    UINT32 Temp32;
    UINT16 Temp16 = 0;
    Rval = SvcFormat_GetMem16(Ctx, &Temp16);
    if (Rval == FORMAT_OK) {
        Temp32 = (UINT32)Temp16;
        if (Count == 1U) {
            *Param = Temp32;
            Rval = SvcFormat_GetMem16(Ctx, &Temp16);
            if (Rval != FORMAT_OK) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetMem16() failed!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            Rval = SvcFormat_GetMem16(Ctx, &Temp16);
            if (Rval == FORMAT_OK) {
                *Param = (Temp32<<16) + Temp16;
            } else {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetMem16() failed!", __func__, NULL, NULL, NULL, NULL);
            }
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetMem16() failed!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 * Get the exif tag entries from memory
 *
 * @param [in] Ctx The context of memory operation
 * @param [in,out] Tag The id of the tag
 * @param [in,out] Type The type of the tag
 * @param [in,out] Count The count of the tag
 * @param [in,out] Param The parameter of the tag
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcFormat_GetIfdEntry(SVC_MEM_OP_s *Ctx, UINT16 *Tag, UINT16 *Type, UINT32 *Count, UINT32 *Param)
{
    UINT32 Rval;
    Rval = SvcFormat_GetMem16(Ctx, Tag);
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetMem16(Ctx, Type);
        if (Rval == FORMAT_OK) {
            Rval= SvcFormat_GetMem32(Ctx, Count);
            if (Rval == FORMAT_OK) {
                switch (*Type) {
                case TIFF_BYTE:
                case TIFF_ASCII:
                case TIFF_UNDEFINED:
                    Rval = SvcFormat_GetIfdEntry_1byte(Ctx, *Count, Param);
                    break;
                case TIFF_SHORT:
                    Rval = SvcFormat_GetIfdEntry_2byte(Ctx, *Count, Param);
                    break;
                default:
                    Rval = SvcFormat_GetMem32(Ctx, Param);
                    break;
                }
                if (Rval != FORMAT_OK) {
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetIfdEntry() failed!", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetMem32() failed!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetMem16() failed!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetMem16() failed!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

