/**
 * @file Amba_ExifImg.c
 *
 *  Copyright (c) 2020 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  This file includes sample code and is only for internal testing and evaluation.  If you
 *  distribute this sample code (whether in source, object, or binary code form), it will be
 *  without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *  @details EXIF Formatter
 *
 */

#ifdef CONFIG_LINUX
#include "AmbaPrint.h"
#include "AmbaWrap.h"
#endif
#include "AmbaUtility.h"
#include "AmbaImg_External_CtrlFunc.h"

// #include "SvcFrwk.h"
#include "Amba_ExifImg.h"


//Take of AE/AWB algo. Infomation first.

static UINT32 ExifChannel = 0;
static EXIF_OBJ_INFO_s GExifObj __attribute__((section(".bss.noinit")));

static UINT8 Sconvert_ae_index(UINT8 ae_index)
{
    AEB_INFO_s    AebInfoTmp = {0};
    UINT8         rval = 0;
    UINT16        format = 0;
    UINT8         AeIndexTmp = ae_index;
    UINT32        U32RVal = 0U;

    U32RVal |= AmbaImgProc_GetCapFormat(ExifChannel, &format);
    if (format == (UINT32)IMG_CAP_AEB) {
        U32RVal |= AmbaImgProc_AEGetAEBInfo(0, &AebInfoTmp);
        if(AebInfoTmp.Num == 0U){
            AebInfoTmp.Num = 1U;
        }
        AeIndexTmp = ae_index % AebInfoTmp.Num;
        if (AeIndexTmp > (UINT8)AEB_MAX_NUM) {
            (void)AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "<Sconvert_ae_index> ,Incorrect ae_idx:%d for AEB mode!", AeIndexTmp, 0U, 0U, 0U, 0U);
            rval = 0;
        } else {
            rval = AeIndexTmp;
        }
        //Nature capture 2nd/3rd photo share same Exif infomration
    }
    if(U32RVal != 0U){
        /* MisraC */
    }
    return rval;
}

// Set private tag
static INT32 img_exif_set_private_tag(UINT8 aeIdx, const EXIF_PRIINFO_s *ppri_exif)
{
    UINT8 aeIdxTmp = aeIdx;
    INT32 rval = -1;

    aeIdxTmp = Sconvert_ae_index(aeIdxTmp);
    if(ppri_exif != NULL){
         if (aeIdxTmp >= (UINT8)IMG_EXIF_NUM_OF_AEB_SHOTS){
             rval = -1;
         }else{
             GExifObj.ExifInfo[aeIdxTmp].PrivateImageData = ppri_exif->Data;
             GExifObj.ExifInfo[aeIdxTmp].PriImgDataSize  = ppri_exif->Size;
             rval = 0;
         }
    }
    return rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Amba_Exif_Get_Exif_Info
 *
 *  @Description:: To get EXIF info.
 *
 *  @Input      ::
 *    UINT8 aeIdx : Ae Information index
 *
 *  @Output     ::
 *    EXIF_INFO_s *pExifInfo : Pointer to exif information
 *
 *  @Return     ::
 *     UINT32 : OK(0)/NG(1)
 *
\*-----------------------------------------------------------------------------------------------*/
// Get exif information by exif muxer, 0 for singal still, 0-4 for AEB, don't care for PIV/PB capture
UINT32 Amba_Exif_Get_Exif_Info(UINT8 aeIdx, EXIF_INFO_s *pExifInfo)
{
    UINT32 rval = AMBA_EXIF_OK;
    UINT8 aeIdxTmp = aeIdx;

    aeIdxTmp = Sconvert_ae_index(aeIdxTmp);
    if(pExifInfo != NULL){
        if (aeIdxTmp >= (UINT8)IMG_EXIF_NUM_OF_AEB_SHOTS){
            rval = AMBA_EXIF_NG;
        }else{
            *pExifInfo = GExifObj.ExifInfo[aeIdxTmp];
            rval = AMBA_EXIF_OK;
        }
    }

    return rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Amba_Exif_Compute_AAA_Exif
 *
 *  @Description:: To compute EXIF info.
 *
 *  @Input      ::
 *    COMPUTE_EXIF_PARAMS_s *pComputeExifParams : poniter to compute exif parameters.
 *
 *  @Output     ::
 *     None
 *
 *  @Return     ::
 *     None
 *
\*-----------------------------------------------------------------------------------------------*/
void Amba_Exif_Compute_AAA_Exif (const COMPUTE_EXIF_PARAMS_s *pComputeExifParams)
{

    static const UINT8 LightSource[WB_LASTMODE+1U][2] = {
                                                    /* WB MODE,        EXIF 2.2 number*/
                                                    {WB_AUTOMATIC,            0},  /* unknown */
                                                    {WB_INCANDESCENT,         3},  /* Tungsten (incandescent light) */
                                                    {WB_D4000,              255},  /* other light source */
                                                    {WB_D5000,               23},  /* D50 */
                                                    {WB_SUNNY,                9},  /* Fine weather */
                                                    {WB_CLOUDY,              10},  /* Cloudy weather */
                                                    {WB_D9000,              255},  /* other light source */
                                                    {WB_D10000,             255},  /* other light source */
                                                    {WB_FLASH,                4},  /* Flash */
                                                    {WB_FLUORESCENT,         15},  /* White fluorescent (WW 3200 �V 3700K) */
                                                    {WB_FLUORESCENT_2,       14},  /* Cool white fluorescent (W 3900 �V 4500K) */
                                                    {WB_FLUORESCENT_3,       13},  /* Day white fluorescent (N 4600 �V 5400K) */
                                                    {WB_FLUORESCENT_4,       12},  /* Daylight fluorescent (D 5700 �V 7100K) */
                                                    {WB_OUTDOOR,              1},  /* Daylight */
                                                    {0,                       0},
    };

    static AWB_CONTROL_s           ImgExifAwbControlMode __attribute__((section(".bss.noinit")));
    static AE_CONTROL_s            ImgExifAeControlMode __attribute__((section(".bss.noinit")));
    static AMBA_AE_INFO_s          AeInfo[AEB_MAX_NUM] __attribute__((section(".bss.noinit")));
    static AEB_INFO_s              AebInfo __attribute__((section(".bss.noinit")));
    static EXIF_PRIINFO_s         PpriExif = {NULL, 0};
    static UINT8 ExifDebugFlg = 0;
    UINT16 FNumber, ApValue;
    LENS_INFORM_s Lensinfo;
    UINT32 FocusDistance;
    UINT8 AeMetering;
    UINT8 FlashpixVersion[4];// = {(UINT8)'0', (UINT8)'1', (UINT8)'0', (UINT8)'0' }; // Flashpix Format Version 1.0
    UINT32 ZoomStsZpFocalTemp = 0;
    INT16 EvBias;
    UINT16 FlashMode;
    UINT16 ii;
    DOUBLE misracTmp = 0.0;
    DOUBLE misracTmp2 = 0.0;
    UINT32 U32RVal = 0U;
    INT32  I32RVal = 0;

    FlashpixVersion[0] = (UINT8)0;
    FlashpixVersion[1] = (UINT8)1;
    FlashpixVersion[2] = (UINT8)0;
    FlashpixVersion[3] = (UINT8)0;

    ExifChannel = pComputeExifParams->ChannelNo;

    U32RVal |= AmbaWrap_memset(&GExifObj, 0, sizeof(GExifObj));

    if (ExifDebugFlg == 0U) {
        PpriExif.Data = NULL;
        PpriExif.Size = 0;
        I32RVal = img_exif_set_private_tag(pComputeExifParams->AeIdx,&PpriExif);
        if(I32RVal != 0){
            /* MisraC */
        }
    }

    U32RVal |= AmbaImgProc_AEGetAEControlCap(pComputeExifParams->ChannelNo, &ImgExifAeControlMode);


    if (pComputeExifParams->Mode == (UINT32)IMG_EXIF_PIV) {
        FlashMode = ImgExifAeControlMode.VideoFlash;
        U32RVal |= AmbaImgProc_AEGetExpInfo(pComputeExifParams->ChannelNo, 0U, IP_MODE_VIDEO, AeInfo);
    } else { // IMG_EXIF_PIV
        FlashMode = ImgExifAeControlMode.StillFlash;
        U32RVal |= AmbaImgProc_AEGetExpInfo(pComputeExifParams->ChannelNo, 0U, IP_MODE_STILL, AeInfo);
    }

    // Calculate shutter speed value  //calculate with log funciton
    misracTmp = ((DOUBLE)(AeInfo[pComputeExifParams->AeIdx].ShutterTime) * (DOUBLE)1000000.0);
    GExifObj.ExifInfo[pComputeExifParams->AeIdx].ExposureTimeNum = (UINT32)misracTmp;
    GExifObj.ExifInfo[pComputeExifParams->AeIdx].ExposureTimeDen = 1000000;///exposure_time.den;

    U32RVal |= (UINT32)AmbaWrap_log2(AeInfo[pComputeExifParams->AeIdx].ShutterTime, &misracTmp2);
    misracTmp = (misracTmp2*1000.0f);
    GExifObj.ExifInfo[pComputeExifParams->AeIdx].ShutterSpeedNum = (UINT32)misracTmp;
    GExifObj.ExifInfo[pComputeExifParams->AeIdx].ShutterSpeedDen = (UINT32)1000;
    GExifObj.ExifInfo[pComputeExifParams->AeIdx].IsoSpeedRating = AeInfo[pComputeExifParams->AeIdx].IsoValue;

    FNumber = 280; //FixMe, the value is for test
    U32RVal |= (UINT32)AmbaWrap_log2(((DOUBLE)FNumber/(DOUBLE)100), &misracTmp2);
    misracTmp = ((DOUBLE)100 * (DOUBLE)2 *misracTmp2);
    ApValue = (UINT16) misracTmp;
    GExifObj.ExifInfo[pComputeExifParams->AeIdx].FNumberNum = (UINT32)FNumber;
    GExifObj.ExifInfo[pComputeExifParams->AeIdx].FNumberDen = (UINT32)100;
    GExifObj.ExifInfo[pComputeExifParams->AeIdx].ApertureValueNum = (UINT32)ApValue;
    GExifObj.ExifInfo[pComputeExifParams->AeIdx].ApertureValueDen = (UINT32)100;

    Lensinfo.FlashType = 0;
    //Lensinfo.AptValueMax = 100;
    //
    GExifObj.ExifInfo[pComputeExifParams->AeIdx].MaxApertureValueNum = (UINT32)ApValue;//FixMe, (UINT32)Lensinfo.AptValueMax;
    GExifObj.ExifInfo[pComputeExifParams->AeIdx].MaxApertureValueDen = (UINT32)100;

    GExifObj.ExifInfo[pComputeExifParams->AeIdx].FocalLengthNum = ZoomStsZpFocalTemp;//FixMe
    GExifObj.ExifInfo[pComputeExifParams->AeIdx].FocalLengthDen = (UINT32)100;

    FocusDistance = 0;//FixMe
    GExifObj.ExifInfo[pComputeExifParams->AeIdx].SubjectDistanceNum = (UINT32)FocusDistance;
    GExifObj.ExifInfo[pComputeExifParams->AeIdx].SubjectDistanceDen = (UINT32)100;


    GExifObj.ExifInfo[pComputeExifParams->AeIdx].DigitalZoomRatioDen = 1024;  //FIXME

    EvBias = ImgExifAeControlMode.EvBias;

    GExifObj.ExifInfo[pComputeExifParams->AeIdx].ExposureProgram = 2;        /* Normal program */
    GExifObj.ExifInfo[pComputeExifParams->AeIdx].CustomRendered = 0;         /* 0: Normal process; 1: Custom process */
    GExifObj.ExifInfo[pComputeExifParams->AeIdx].ExposureMode = 0;           /* 0: Auto exposure; 1: Manual exposure; 2:Auto bracket */

    U32RVal |= AmbaImgProc_AEGetAEBInfo(0, &AebInfo);

    #if 0
    if (AebInfo.Num > 0U){//AEB MODE
        GExifObj.ExifInfo[pComputeExifParams->AeIdx].ExposureBiasValueNum = (UINT32)(EvBias - AebInfo.EvBias[pComputeExifParams->AeIdx]);
        GExifObj.ExifInfo[pComputeExifParams->AeIdx].ExposureBiasValueDen = 32U;
    } else {
        GExifObj.ExifInfo[pComputeExifParams->AeIdx].ExposureBiasValueNum = (UINT32)EvBias;
        GExifObj.ExifInfo[pComputeExifParams->AeIdx].ExposureBiasValueDen = 32U;
    }
    #else//AEB MODE <== NEED FIX
    GExifObj.ExifInfo[pComputeExifParams->AeIdx].ExposureBiasValueNum = (UINT32)EvBias;
    GExifObj.ExifInfo[pComputeExifParams->AeIdx].ExposureBiasValueDen = 32U;
    #endif

    if (pComputeExifParams->Mode == (UINT32)IMG_EXIF_PIV) {
        GExifObj.ExifInfo[pComputeExifParams->AeIdx].Flash = 0x20;           /* No flash function */
    } else {
        if (Lensinfo.FlashType == 0U) {
            GExifObj.ExifInfo[pComputeExifParams->AeIdx].Flash = 0x20;      /* No flash function */
        } else {
            /* Values for bit 0 indicating whether the flash fired */
            if (AeInfo[pComputeExifParams->AeIdx].Flash == 0U){
                GExifObj.ExifInfo[pComputeExifParams->AeIdx].Flash = 0x00; /* 0: Flash did not fire */
            }
            else{
                GExifObj.ExifInfo[pComputeExifParams->AeIdx].Flash = 0x01; /* 1: Flash fired */
            }

            /* Values for bits 3 and 4 indicating the camera's flash mode. */
            switch (FlashMode) {
                case FLASH_AUTO:
                    GExifObj.ExifInfo[pComputeExifParams->AeIdx].Flash = GExifObj.ExifInfo[pComputeExifParams->AeIdx].Flash | 0x18U;
                    break;
                case FLASH_ALWAYS_ON:
                    GExifObj.ExifInfo[pComputeExifParams->AeIdx].Flash = GExifObj.ExifInfo[pComputeExifParams->AeIdx].Flash | 0x08U;
                    break;
                case FLASH_ALWAYS_OFF:
                    GExifObj.ExifInfo[pComputeExifParams->AeIdx].Flash = GExifObj.ExifInfo[pComputeExifParams->AeIdx].Flash | 0x10U;
                    break;
                default: /* unknown */
                    GExifObj.ExifInfo[pComputeExifParams->AeIdx].Flash = GExifObj.ExifInfo[pComputeExifParams->AeIdx].Flash | 0x00U;
                    break;
            }
        }
    }

    U32RVal |= AmbaImgProc_AWBGetAWBCtrlCap(pComputeExifParams->ChannelNo, &ImgExifAwbControlMode);

    GExifObj.ExifInfo[pComputeExifParams->AeIdx].LightSource = LightSource[ImgExifAwbControlMode.MenuMode][1];
    GExifObj.ExifInfo[pComputeExifParams->AeIdx].ColorSpace = 1;             /* sRGB */
    GExifObj.ExifInfo[pComputeExifParams->AeIdx].SensingMethod = 2;          /* One-chip color area sensor*/
    GExifObj.ExifInfo[pComputeExifParams->AeIdx].FileSource = 3;             /* always to be 3 for DSC */
    GExifObj.ExifInfo[pComputeExifParams->AeIdx].SceneType = 1;              /* always to be 1 for A directly photographed image */

    if (ImgExifAwbControlMode.MenuMode == (UINT8)WB_AUTOMATIC){
        GExifObj.ExifInfo[pComputeExifParams->AeIdx].WhiteBalance = 0;   /* Auto white balance */
    }
    else{
        GExifObj.ExifInfo[pComputeExifParams->AeIdx].WhiteBalance = 1;   /* Manual white balance */
    }

    //GExifObj.ExifInfo[aeIdx].FocalLength35mmFilm = _img_exif_compute_focal_length_35mm_film(&geometry, &dev_spec, &DzoomStatus, &zoom_sts);
    GExifObj.ExifInfo[pComputeExifParams->AeIdx].FocalLength35mmFilm = 28;//FixMe

    GExifObj.ExifInfo[pComputeExifParams->AeIdx].SceneCaptureType = 0;      /* 0: Standard; 1: Landscape; 2: Portrait; 3: Night scene */
    GExifObj.ExifInfo[pComputeExifParams->AeIdx].Contrast = 0;                /* 0: Normal */
    GExifObj.ExifInfo[pComputeExifParams->AeIdx].Saturation = 0;              /* 0: Normal */
    GExifObj.ExifInfo[pComputeExifParams->AeIdx].Sharpness = 0;               /* 0: Normal */

    AeMetering = ImgExifAeControlMode.MeteringMode;
    if (AeMetering == (UINT8)AE_METER_CENTER){
        GExifObj.ExifInfo[pComputeExifParams->AeIdx].MeteringMode = 2;     /* CenterWeightedAverage */
    }
    else if (AeMetering == (UINT8)AE_METER_AVERAGE){
        GExifObj.ExifInfo[pComputeExifParams->AeIdx].MeteringMode = 1;     /* Average */
    }
    else if (AeMetering == (UINT8)AE_METER_SPOT){
        GExifObj.ExifInfo[pComputeExifParams->AeIdx].MeteringMode = 3;     /* Spot */
    }
    else{
        GExifObj.ExifInfo[pComputeExifParams->AeIdx].MeteringMode = 0;     /* unknown */
    }

    for (ii=0; ii<4U; ii++){
        GExifObj.ExifInfo[pComputeExifParams->AeIdx].FlashPixVersion[ii] = FlashpixVersion[ii];
    }

    GExifObj.ExifInfo[pComputeExifParams->AeIdx].GainControl = 0;              /* 0: None */
    if(U32RVal != 0U){
        /* MisraC */
    }
}
