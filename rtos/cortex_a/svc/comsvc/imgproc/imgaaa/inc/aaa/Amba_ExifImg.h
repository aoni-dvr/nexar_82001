 /**
 * @file Amba_ExifImg.h
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

#ifndef AMBA_EXIF_IMG_H
#define AMBA_EXIF_IMG_H

/**
 *  Definition of Capture mode
 */
#define IMG_EXIF_STILL                  (0x00)
#define IMG_EXIF_PIV                    (0x01)
#define IMG_EXIF_PB                     (0x02)
#define IMG_EXIF_LASTMODE               (0X02)

#define AMBA_EXIF_OK     (0x00000000U)
#define AMBA_EXIF_NG     (0x00000001U)


/**
 * EXIF private tag, ensure that the data can be referenced until it is processed by the exif muxer
 */
typedef struct
{
    UINT8     *Data;
    UINT32    Size;
} EXIF_PRIINFO_s;

/**
 * EXIF information structure
 */
typedef struct  {
    UINT32    ExposureTimeNum;
    UINT32    ExposureTimeDen;
    UINT32    FNumberNum;
    UINT32    FNumberDen;
    UINT32    ExposureProgram;
    UINT32    FocalLengthNum;
    UINT32    FocalLengthDen;
    UINT32    SubjectDistanceNum;
    UINT32    SubjectDistanceDen;
    UINT16    CustomRendered;
    UINT16    ExposureMode;
    UINT16    WhiteBalance;
    UINT16    FocalLength35mmFilm;
    UINT16    SceneCaptureType;
    UINT16    Contrast;
    UINT16    Saturation;
    UINT16    Sharpness;
    UINT16    SubjectDistanceRange;
    UINT32    ShutterSpeedNum;
    UINT32    ShutterSpeedDen;
    UINT32    ApertureValueNum;
    UINT32    ApertureValueDen;
    UINT32    ExposureBiasValueNum;
    UINT32    ExposureBiasValueDen;
    UINT32    MaxApertureValueNum;
    UINT32    MaxApertureValueDen;
    UINT32    DigitalZoomRatioNum;
    UINT32    DigitalZoomRatioDen;
    UINT16    MeteringMode;
    UINT16    LightSource;
    UINT8     FlashPixVersion[4];
    UINT16    ColorSpace;
    UINT16    SensingMethod;
    UINT32    ExposureIndexNum;
    UINT32    ExposureIndexDen;
    UINT16    FileSource;
    UINT16    SceneType;
    UINT16    Flash;
    UINT16    IsoSpeedRating;
    UINT8     *PrivateImageData;
    UINT32    PriImgDataSize;
    UINT8     GainControl;
} EXIF_INFO_s;

#define IMG_EXIF_NUM_OF_AEB_SHOTS 5
typedef struct{
    //pcompute_func_t    compute_exif_func[3];  // mode 0: STILL, mode 1: PIV, mode 2: PB Capture
    EXIF_INFO_s        ExifInfo[IMG_EXIF_NUM_OF_AEB_SHOTS];
} EXIF_OBJ_INFO_s;

/**
 * Raw Header information
 */
typedef struct {
    INT32     OpticalBlack[4]; //4*4
    UINT32    LensDistortionRatio[46]; //46*4
    UINT32    IndividualColorGain[8]; //8*4
    UINT32    AutoColorGain[3]; //3*4
    UINT32    CustomcolorGain[3]; //3*4;
    UINT32    ChromaticAberration[62]; //62*4
    UINT32    LensRelativeIllumination[46]; //46*4
} RAW_HEADERINFO_s;

/**
* Exif Compute information
*/
typedef struct
{
    UINT32 ChannelNo;
    UINT32 Mode;
    UINT8  AeIdx;
} COMPUTE_EXIF_PARAMS_s;

//Need to get the information from AF,distance and sensor driver
typedef struct
{
    UINT8 FlashType;           /**< type of flash gun supported. '0' if no flash supported **/
    UINT16 AptValueMax;    /**< 100 * valid aperture value */
} LENS_INFORM_s;

/**
* To compute EXIF info.
*
* @param [in] pComputeExifParams poniter to compute exif parameters
* @return void
*/
void Amba_Exif_Compute_AAA_Exif(const COMPUTE_EXIF_PARAMS_s *pComputeExifParams);

/**
* To get EXIF info.
*
* @param [in] aeIdx  Ae Information index
* @param [out] pExifInfo  Pointer to exif information
* @return 0 OK, 1 NG
*/
UINT32 Amba_Exif_Get_Exif_Info(UINT8 aeIdx, EXIF_INFO_s *pExifInfo);


void Amba_Img_Exif_Set_Debug_Flg(UINT8 flg);
UINT8 Amba_Img_Exif_Get_Debug_Flg(void);

#endif  /* __AMBA_IMGEXIF_H__ */
