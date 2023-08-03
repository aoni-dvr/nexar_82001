/**
*  @file AmbaExifDemuxer.c
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
*  @details EXIF Demuxer
*
*/

#include "AmbaFS.h"
//#include "AmbaPrint.h"
#include "AmbaMisraFix.h"

#include "AmbaExifFile.h"
#include "AmbaExifMem.h"
#include "AmbaExif.h"
#include "AmbaExifUtil.h"
#include "AmbaFrwk.h"

#include "AmbaMisraFix.h"
#include "AmbaWrap.h"
#include "AmbaUtility.h"
//#include "SvcWrap.h"

//#define EXIFDLOG_OK       AmbaFrwk_LogOK
#define EXIFDLOG_NG       AmbaFrwk_LogNG
//#define EXIFDLOG_DBG      AmbaFrwk_LogDBG

//#define __DEBUG_EXIFDEMUXER
#if 0//Mic__
#ifdef __DEBUG_EXIFDEMUXER
#define DEBUG_EXIFDEMUXER       (void)AmbaPrint
#else
#define DEBUG_EXIFDEMUXER(...)
#endif
#define DEBUG_EXIFDEMUXER_ERR   (void)AmbaPrint

#define IF_SEEK_FAIL(msg)       if (Rval < 0) {//DEBUG_EXIFDEMUXER_ERR("[%s]#%d  %s", __func__, __LINE__, msg); pImageInfo->Error = 1;}
#endif

/* PAAACopyMode = 1 ==> Copy makernote to pImageInfo->PAAA */
#define PAAACopyMode            0
#if PAAACopyMode
UINT8 MakerNoteBuffer[30 * KB];
#endif

static UINT8 EndianType GNU_SECTION_NOZEROINIT;

static void   GetIfd0(AMBA_EXIF_MEM *pCurrentMemory, AMBA_EXIF_IMAGE_INFO_s *pImageInfo, UINT32 TiffBaseOffset, UINT32 *pExifOffset, UINT32 *pDateOffset, UINT32 *pGpsOffset);
static void   GetIfd1(AMBA_EXIF_MEM *pCurrentMemory, AMBA_EXIF_IMAGE_INFO_s *pImageInfo, UINT32 Offset, UINT32 TiffBaseOffset, UINT8 MPOIndex);
static void   GetExifIfd(AMBA_EXIF_MEM *pCurrentMemory, AMBA_EXIF_IMAGE_INFO_s *pImageInfo, UINT32 IfdOffset, UINT32 TiffBaseOffset);
static void   GetGPSIfd(AMBA_EXIF_MEM *pCurrentMemory, AMBA_EXIF_IMAGE_INFO_s *pImageInfo, UINT32 IfdOffset, UINT32 TiffBaseOffset);
static void   FastGetIfd0(AMBA_EXIF_MEM *pCurrentMemory, AMBA_EXIF_IMAGE_INFO_s *pImageInfo, UINT32 *pExifOffset);
static void   FastGetIfd1(AMBA_EXIF_MEM *pCurrentMemory, AMBA_EXIF_IMAGE_INFO_s *pImageInfo, UINT32 Offset, UINT32 TiffBaseOffset);
static UINT32    CheckDHT(AMBA_EXIF_MEM *pCurrentMemory, UINT8 Type, UINT16 Length);
static INT32    CheckJPEG(AMBA_EXIF_MEM *pCurrentMemory, AMBA_EXIF_IMAGE_INFO_s *pImageInfo, UINT32 Length);
static void    GetIfdEntry(AMBA_EXIF_MEM *pCurrentMemory, UINT32 *pTag, UINT32 *pType, UINT32 *pCount, UINT32 *pParam);
static UINT8  GetHeaderEndianType(AMBA_EXIF_MEM *pCurrentMemory);
static UINT32 (*MemGet16)(AMBA_EXIF_MEM *pCurrentMemory);
static UINT32 (*MemGet32)(AMBA_EXIF_MEM *pCurrentMemory);

UINT32 AmbaExifDemuxer_ParseAPP1TIFF(AMBA_EXIF_MEM *pCurrentMemory, AMBA_EXIF_IMAGE_INFO_s *pImageInfo, const AMBA_FS_FILE_INFO_s *pFileStatus)
{
    UINT32 Rval = OK;
    //INT32 Sec, Min, Hour, Mday, Mon, Year;//Mic__ TBD
    UINT32 TiffBaseOffset, OneStIfdOffset, ExifOffset = 0, DateOffset = 0, GpsOffset = 0;
    //UINT32 ZeroThIfdOffset;

    //DEBUG_EXIFDEMUXER("[%s] START!!!     pCurrentMemory->Position=%d", __func__, pCurrentMemory->Position);

    //    Tag = AmbaExif_MemGetBe32(pCurrentMemory);
    //
    //    keyword, "Exif"
    //    if (Tag != AMBA_EXIF_TIFF_HEADER)
    //        //DEBUG_EXIFDEMUXER_ERR("ExifParse: Expecting EXIF! %X",Tag);
    //    Pad should be 00
    //    Tag = AmbaExif_MemGetByte(pCurrentMemory);
    //    Tag = AmbaExif_MemGetByte(pCurrentMemory);
    /* Pad 0000 */

    AmbaMisra_TouchUnused(&pFileStatus);

    (void)AmbaExif_MemGetBe16(pCurrentMemory);

    TiffBaseOffset = pCurrentMemory->Position;
    //DEBUG_EXIFDEMUXER("[%s] TiffBaseOffset: %d", __func__, TiffBaseOffset);

    /* Get endian type */
    if (GetHeaderEndianType(pCurrentMemory) == 2U) {
        //DEBUG_EXIFDEMUXER_ERR("ExifParse: GetHeaderEndianType fail");
        EXIFDLOG_NG(__func__, "Line %d", __LINE__, 0U);
        Rval = NG;
    } else {
        pImageInfo->Endian = EndianType;
        //ZeroThIfdOffset = MemGet32(pCurrentMemory) + TiffBaseOffset;
        ////DEBUG_EXIFDEMUXER("[%s]===== ZeroThIfdOffset = %d" , __func__, ZeroThIfdOffset);
        ////DEBUG_EXIFDEMUXER("[%s]===== TiffBaseOffset   = %d" , __func__, TiffBaseOffset);
        /* ZeroThIfdOffset = MemGet32(pCurrentMemory) + TiffBaseOffset */
        (void)MemGet32(pCurrentMemory);

        GetIfd0(pCurrentMemory, pImageInfo, TiffBaseOffset, &ExifOffset, &DateOffset, &GpsOffset);
        OneStIfdOffset = MemGet32(pCurrentMemory) + TiffBaseOffset;

        /* Date */
        if (DateOffset != 0U) {
            DateOffset += TiffBaseOffset;
            if (NG == AmbaExif_MemSeek(pCurrentMemory, (INT32)DateOffset, MM_SEEK_SET)) {
                EXIFDLOG_NG(__func__, "Line %d", __LINE__, 0U);
            }
            (void)AmbaExif_MemRead(pCurrentMemory, pImageInfo->Date, 1, AMBA_EXIF_TIFF_DATETIME_SIZE);
        } else {
            #if 0//Mic__ TBD
            // If there hasn't DateTime info, use file DateTime info
            Sec  = (INT32)pFileStatus->TimeCreate.Second * 2;
            Min  = (INT32)pFileStatus->TimeCreate.Minute;
            Hour = (INT32)pFileStatus->TimeCreate.Hour;
            Mday = (INT32)pFileStatus->TimeCreate.Day;
            Mon  = (INT32)pFileStatus->TimeCreate.Month;
            Year = (INT32)pFileStatus->TimeCreate.Year + 1980;
            //(void)snprintf(pImageInfo->Date, sizeof(pImageInfo->Date), "%.4d:%.2d:%.2d %.2d:%.2d:%.2d", Year, Mon, Mday, Hour, Min, Sec);
            (void)AmbaUtility_StringPrintUInt32(pImageInfo->Date, sizeof(pImageInfo->Date), "%.4d:%.2d:%.2d %.2d:%.2d:%.2d", &Year, &Mon, &Mday, &Hour, &Min, &Sec);
            #endif
        }

        /* EXIF IFD */
        if (ExifOffset != 0U) {
            pImageInfo->WithExif = 1;
            ExifOffset += TiffBaseOffset;
            GetExifIfd(pCurrentMemory, pImageInfo, ExifOffset, TiffBaseOffset);
        }

        /* GPSInfo IFD */
        if (GpsOffset != 0U) {
            pImageInfo->WithGPSIfd = 1;
            GpsOffset += TiffBaseOffset;
            GetGPSIfd(pCurrentMemory, pImageInfo, GpsOffset, TiffBaseOffset);
        }

        /* Thumbnail IFD */
        if (OneStIfdOffset > (12U + TiffBaseOffset)) {
            GetIfd1(pCurrentMemory, pImageInfo, OneStIfdOffset, TiffBaseOffset, 0);

            if (NG == AmbaExif_MemSeek(pCurrentMemory, (INT32) pImageInfo->ThumbPos, MM_SEEK_SET)) {
                EXIFDLOG_NG(__func__, "Line %d", __LINE__, 0U);
            }

            Rval = AmbaExifDemuxer_CheckTag(pCurrentMemory, pImageInfo, (UINT8)AMBA_EXIF_THUMBNAIL, pImageInfo->ThumbPos - 2U); /* Add SOI */

            if (Rval != OK) {
                //DEBUG_EXIFDEMUXER_ERR("ExifParse: TMB Unsupported %d", Rval);
                pImageInfo->ThumbPos = 0U;
            } else {
                pImageInfo->Thumbnail++;
            }
        } else {
            //DEBUG_EXIFDEMUXER_ERR("ExifParse: No IFD1");
        }
        //DEBUG_EXIFDEMUXER("[%s] END!!!     pCurrentMemory->Position=%d", __func__, pCurrentMemory->Position);
    }

    return Rval;
}

UINT32 AmbaExifDemuxer_ParseAPP1XMP(const AMBA_EXIF_MEM *pCurrentMemory, const AMBA_EXIF_IMAGE_INFO_s *pImageInfo)
{
    if(pCurrentMemory == NULL){}
    if(pImageInfo == NULL){}

    return OK;
}

UINT32 AmbaExifDemuxer_ParseAPP2(AMBA_EXIF_MEM *pCurrentMemory, AMBA_EXIF_IMAGE_INFO_s *pImageInfo, INT64 OffsetOfApp2, UINT32 Length)
{
    INT32 SoiOffset = 0;
    UINT32 Start, Next;
    UINT32 Rval = OK;

    //DEBUG_EXIFDEMUXER("[%s] START!!!     pCurrentMemory->Position=%d", __func__, pCurrentMemory->Position);

    if (pImageInfo->Screennail == 0U) {
        //DEBUG_EXIFDEMUXER("[%s] MPF CHECK START!!!", __func__);
        Start = AmbaExif_MemGetBe16(pCurrentMemory);
        Next = AmbaExif_MemGetBe16(pCurrentMemory);
        if ((Start == 0x4d50U) && (Next == 0x4600U)) {
            /* MPF */
            Rval = AmbaExifDemuxer_ParseAPP2MPF(pCurrentMemory, pImageInfo);
            /* pImageInfo->SecondPicType == SECPIC_TYPE_MPF; */
            if (Rval != OK) {
                //DEBUG_EXIFDEMUXER("[%s] : SecondPicType = SECPIC_TYPE_MPF", __func__);
            }
        } else {
            if ((Start == 0x4650U) && (Next == 0x5852U)) {
                /* FPXR */
                SoiOffset = CheckJPEG(pCurrentMemory, pImageInfo, (Length - 4U) - 2U); /* subtract the 4 + 2 ('fpxr' and Length) */
                if (SoiOffset == -0x5a5a) {
                    /* refer to CheckJPEG's return value */
                    //DEBUG_EXIFDEMUXER_ERR("[%s]#%d : illegal content in this app2 seg!", __func__, __LINE__);
                    Rval = NG;
                }
                if (Rval == OK) {
                    if (SoiOffset != -1) {
                        //r = AmbaExifDemuxer_CheckTag(pCurrentMemory, pImageInfo, AMBA_EXIF_SCREENNAIL, OffsetOfApp2+pImageInfo->ScreenSoiOffset);
                        Rval = AmbaExifDemuxer_CheckTag(pCurrentMemory, pImageInfo, (UINT8)AMBA_EXIF_SCREENNAIL, 0);
                        if (Rval == OK) {
                            /* Found screennail */
                            pImageInfo->Screennail = 1;
                            pImageInfo->ScreenPos  = (UINT32)OffsetOfApp2 + 2U; /* in front of SOI */
                            pImageInfo->ScreenSize = (Length - pImageInfo->ScreenSoiOffset) - 2U;
                            pImageInfo->SecondPicType = (UINT8)SECPIC_TYPE_FPXR;
                            //DEBUG_EXIFDEMUXER("[%s] : SecondPicType = SECPIC_TYPE_FPXR", __func__);
                        }
                    }
                }
            } else {
                if (Start == (UINT32)AMBA_EXIF_SEGMENT_SOI) {
                    /* SECPIC_TYPE_APP2 */
                    Rval = AmbaExif_MemSeek(pCurrentMemory, (INT32) -4, MM_SEEK_CUR);
                    if (Rval == NG) {
                        EXIFDLOG_NG(__func__, "Line %d", __LINE__, 0U);
                    } else {
                        Rval = AmbaExifDemuxer_CheckTag(pCurrentMemory, pImageInfo, (UINT8)AMBA_EXIF_SCREENNAIL, 0);
                        if (Rval == OK) {
                            /* Found screennail */
                            pImageInfo->Screennail = 1;
                            pImageInfo->ScreenPos += ((UINT32)OffsetOfApp2 - 2U) ; /* Start from FFDB */
                            pImageInfo->ScreenSize = Length + 2U;
                            pImageInfo->SecondPicType = (UINT8)SECPIC_TYPE_APP2;
                            //DEBUG_EXIFDEMUXER("[%s] : SecondPicType = SECPIC_TYPE_APP2", __func__);
                        } else {
                            EXIFDLOG_NG(__func__, "Line %d", __LINE__, 0U);
                        }
                    }
                } else {
                    //DEBUG_EXIFDEMUXER_ERR("[%s]#%d : screennail does not exist in app2", __func__, __LINE__);
                    EXIFDLOG_NG(__func__, "Line %d screennail does not exist in app2", __LINE__, 0U);
                }
            }
        }
    } else {
        if (pImageInfo->SecondPicType == (UINT8)SECPIC_TYPE_APP2) {
            pImageInfo->ScreenSize += (Length - 2U);
        } else {
            if (pImageInfo->SecondPicType == (UINT8)SECPIC_TYPE_FPXR) {
                pImageInfo->ScreenSize += (Length - 15U);
            }
        }
    }
    if (Rval == OK) {
        //DEBUG_EXIFDEMUXER("[%s] pImageInfo-> screennail=%d  scr_pos=%d    scr_size=%d  SoiOffset=%d", __func__, pImageInfo->Screennail, pImageInfo->ScreenPos,
                          //pImageInfo->ScreenSize, pImageInfo->ScreenSoiOffset);
        //DEBUG_EXIFDEMUXER("[%s] END!!!     pCurrentMemory->Position=%d", __func__, pCurrentMemory->Position);
    }
    return Rval;
}

UINT32 AmbaExifDemuxer_ParseAPP2MPF(AMBA_EXIF_MEM *pCurrentMemory, AMBA_EXIF_IMAGE_INFO_s *pImageInfo)
{
    INT32  i;
    UINT16 Count;
    UINT32 Base, Pos = 0, Tmp, Tag = 0U, Type = 0U, Count1 = 0U, Param = 0U, MpTypeCode = 0, RepresentativeImgFlg = 0;
    UINT8  /*RepresentativeImg = 0,*/ FullImg = 0, ScrImg = 0;
    UINT32 Rval = OK;
    UINT32 BreakLoop = 0U;
    //UINT32 Mpoffset = 0;

    //DEBUG_EXIFDEMUXER("[%s] START!!!     pCurrentMemory->Position=%d", __func__, pCurrentMemory->Position);
    Base = pCurrentMemory->Position;
    EndianType = GetHeaderEndianType(pCurrentMemory);
    if (EndianType == 2U) {
        //DEBUG_EXIFDEMUXER_ERR("ExifParse: tiff_get_header fail");
        Rval = NG;
    } else {
        pImageInfo->Endian = EndianType;
    }

    if (Rval == OK) {
        /* offset to first IFD */
        //Mpoffset = MemGet32(pCurrentMemory) + Base;
        (void)MemGet32(pCurrentMemory);

        /* parse mp index ifd */
        Count = (UINT16)MemGet16(pCurrentMemory);
        for (i = 0; i < (INT32)Count; i ++) {
            GetIfdEntry(pCurrentMemory, &Tag, &Type, &Count1, &Param);
            if (Tag == (UINT32)AMBA_EXIF_MPIFD_MP_ENTRY) {
                /* get info of pics 0xB002 */
                Pos = pCurrentMemory->Position;
                Rval = AmbaExif_MemSeek(pCurrentMemory, ((INT32)Param + (INT32)Base), MM_SEEK_SET);
                if (Rval != OK) {
                    //break;
                    BreakLoop = 1U;
                }

                if(BreakLoop == 0U) {
                    while (Count1 != 0U) {
                        //DEBUG_EXIFDEMUXER("[MPF]:pCurrentMemory->Position:%x", pCurrentMemory->Position);
                        Tmp = MemGet32(pCurrentMemory);
                        MpTypeCode = Tmp & 0xffffffU;
                        //DEBUG_EXIFDEMUXER("[MPF]:Tmp:%x", Tmp);
                        //DEBUG_EXIFDEMUXER("[MPF]:MpTypeCode:%x", MpTypeCode);
                        switch (MpTypeCode) {
                        case 0x010001:
                            pImageInfo->MPOImgsTypeList[i] = (UINT8)MPO_TYPE_LARGE_THUMBNAIL_VGA;
                            break;
                        case 0x010002:
                            pImageInfo->MPOImgsTypeList[i] = (UINT8)MPO_TYPE_LARGE_THUMBNAIL_FULL_HD;
                            break;
                        case 0x020001:
                            pImageInfo->MPOImgsTypeList[i] = (UINT8)MPO_TYPE_PANORAMA;
                            break;
                        case 0x020002:
                            pImageInfo->MPOImgsTypeList[i] = (UINT8)MPO_TYPE_DISPARITY;
                            break;
                        case 0x020003:
                            pImageInfo->MPOImgsTypeList[i] = (UINT8)MPO_TYPE_MULTI_ANGLE;
                            break;
                        case 0x030000:
                            pImageInfo->MPOImgsTypeList[i] = (UINT8)MPO_TYPE_BASELINE_MP_PRIMARY_IMG;
                            break;
                        default:
                            pImageInfo->MPOImgsTypeList[i] = (UINT8)MPO_TYPE_UNDEFINED;
                            break;
                        }

                        /* Screennail, Baseline MP primary image */
                        if (((MpTypeCode == 0x010001U) || (MpTypeCode == 0x010002U)) || (MpTypeCode == 0x030000U)) {
                            if (ScrImg == 0U) {
                                /* include APPx */
                                //pImageInfo->ScreenSize = MemGet32(pCurrentMemory);

                                /* For Nikon */
                                /* For Nikon picture, there are 18 bytes 0x00 infront of SOI */
                                //pImageInfo->ScreenPos = MemGet32(pCurrentMemory) + 18;
                                pImageInfo->ScreenPos = MemGet32(pCurrentMemory);
                                pImageInfo->ScreenSize = (UINT32)pImageInfo->FileSize - pImageInfo->ScreenPos; /* infront of SOI */
                                pImageInfo->SecondPicType = (UINT8)SECPIC_TYPE_MPF;
                                Rval = AmbaExif_MemSeek(pCurrentMemory, 4, MM_SEEK_CUR);
                                if (Rval != OK) {
                                    //break;
                                    BreakLoop = 1U;
                                }
                            } else {
                                pImageInfo->MPO[(ScrImg - 1U)].ScreenPos = MemGet32(pCurrentMemory) + 2U; /* infront of SOI */
                                pImageInfo->MPO[(ScrImg - 1U)].ScreenSize = MemGet32(pCurrentMemory);    /* include APPx */

                                Rval = AmbaExif_MemSeek(pCurrentMemory, (INT32) 4, MM_SEEK_CUR);
                                if (Rval != OK) {
                                    //break;
                                    BreakLoop = 1U;
                                }
                            }

                            ScrImg ++;
                        /*else if (Tmp==0x80020002 || Tmp==0x80030000) {  */ //fullview
                        } else if (((MpTypeCode == 0x020001U) || (MpTypeCode == 0x020002U)) || (MpTypeCode == 0x020003U)) {
                            /* Panorama, Disparity, Multi-Angle */
                            RepresentativeImgFlg = Tmp & 0x20000000U; /* MP_REPRESENTATIVE_MASK */
                            //DEBUG_EXIFDEMUXER("Individual img attribute field %x", Tmp);
                            //DEBUG_EXIFDEMUXER("RepresentativeImgFlg  %x", RepresentativeImgFlg);

                            if (RepresentativeImgFlg == 0U) {
                                //DEBUG_EXIFDEMUXER("pImageInfo1->app_marker_info[0].size :%x", pImageInfo->AppMarkerInfo[0].Size);

                                pImageInfo->MPO[FullImg].FullviewSize = MemGet32(pCurrentMemory); /* include APPx */
                                pImageInfo->MPO[(FullImg)].FullviewPos = MemGet32(pCurrentMemory) + Base; /* infront of SOI */

                                //DEBUG_EXIFDEMUXER("pImageInfo->MPO[%u].full_size %x", FullImg, pImageInfo->MPO[FullImg].FullviewSize);
                                //DEBUG_EXIFDEMUXER("pImageInfo->MPO[%u].full_pos  %x", FullImg, pImageInfo->MPO[FullImg].FullviewPos);

                                Rval = AmbaExif_MemSeek(pCurrentMemory, (INT32) 4, MM_SEEK_CUR);
                                FullImg++;
                                //DEBUG_EXIFDEMUXER("FullImg:%d", FullImg);
                                if (Rval != OK) {
                                    //break;
                                    BreakLoop = 1U;
                                }
                            } else {
                                /* RepresentativeImgFlg != 0 */
                                pImageInfo->FullviewSize = MemGet32(pCurrentMemory); /* include APPx */
                                pImageInfo->FullviewPos  = MemGet32(pCurrentMemory);  /* include APPx */

                                //DEBUG_EXIFDEMUXER("APP2MP  pImageInfo->FullviewPos  %x", pImageInfo->FullviewPos);
                                //DEBUG_EXIFDEMUXER("APP2MP  pImageInfo->FullviewSize %x", pImageInfo->FullviewSize);
                                //RepresentativeImg++;

                                Rval = AmbaExif_MemSeek(pCurrentMemory, (INT32) 4, MM_SEEK_CUR);
                                if (Rval != OK) {
                                    //break;
                                    BreakLoop = 1U;
                                }
                            }
                        } else {
                            Rval = AmbaExif_MemSeek(pCurrentMemory, (INT32) 12, MM_SEEK_CUR);
                            if (Rval != OK) {
                                //break;
                                BreakLoop = 1U;
                            }
                        }
                        Count1 -= 16U;
                    }
                }

                if (Rval != OK) {
                    //break;
                    BreakLoop = 1U;
                }
                //DEBUG_EXIFDEMUXER("[MPF]:FullImg:%d", FullImg);
                ////DEBUG_EXIFDEMUXER("[MPF]:RepresentativeImg:%d", RepresentativeImg);
                /* must have one and only one representative img. */
                //if (RepresentativeImg!=1)
                //    goto error;
                if(BreakLoop == 0U) {
                    Rval = AmbaExif_MemSeek(pCurrentMemory, (INT32) Pos, MM_SEEK_SET);
                    if (Rval != OK) {
                        //break;
                        BreakLoop = 1U;
                    } else {
                        Rval = OK;
                    }
                }

                if(BreakLoop == 1U) {
                    break;
                }
            } else {
                if (Tag == (UINT32)AMBA_EXIF_MPIFD_MPF_VERSION) {
                    /* 0xB000 */
                    //DEBUG_EXIFDEMUXER("EXIF_MPIFD_MPFVersion");
                } else {
                    if (Tag == (UINT32)AMBA_EXIF_MPIFD_NUMBER_OF_IMAGES) {
                        /* 0xB001 */
                        pImageInfo->MPONumberOfImgs = (UINT8)Param;
                        //DEBUG_EXIFDEMUXER("pImageInfo->MPONumberOfImgs = %u", pImageInfo->MPONumberOfImgs);
                    }
                }
            }
        }
    }
    if (Rval != OK) {
        //DEBUG_EXIFDEMUXER_ERR("AmbaExifDemuxer_ParseAPP2MPF():  fail.");
        pImageInfo->Error = 1;
        Rval = NG;
    }
    return Rval;
}

UINT32 AmbaExifDemuxer_ParseMPFFull(AMBA_EXIF_MEM *pCurrentMemory, AMBA_EXIF_IMAGE_INFO_s *pImageInfo, UINT8 Index)
{
    INT32 Count, i;
    UINT32 Tag, Length, TempOffset, Base, Type, Param, OffsetOfApp1 = 0;
    UINT32 *p_full_pos = &pImageInfo->MPO[(Index - 1U)].FullviewPos;
    UINT32 *p_full_size = &pImageInfo->MPO[(Index - 1U)].FullviewSize;
    UINT32 Rval;

    //DEBUG_EXIFDEMUXER("=======================");
    //DEBUG_EXIFDEMUXER("[%s] &pImageInfo->MPO[(Index-1)].FullviewPos=%d", __func__, &pImageInfo->MPO[(Index - 1)].FullviewPos);
    //DEBUG_EXIFDEMUXER("[%s] &pImageInfo->MPO[(Index-1)].FullviewSize=%d", __func__, &pImageInfo->MPO[(Index - 1)].FullviewSize);
    //DEBUG_EXIFDEMUXER("=======================");

    Rval = AmbaExif_MemSeek(pCurrentMemory, (INT32) 0, MM_SEEK_SET);
    if (Rval != OK) {
        //DEBUG_EXIFDEMUXER_ERR("ERR: %s %d", __func__, __LINE__);
        EXIFDLOG_NG(__func__, "Line %d", __LINE__, 0U);
    }
    if (Rval == OK) {
        if (AmbaExif_MemGetBe16(pCurrentMemory) != (UINT32)AMBA_EXIF_SEGMENT_SOI) {
            /* SOI 0xFFD8 */
            //DEBUG_EXIFDEMUXER_ERR("@Cannot found SOI");
            Rval = NG;
        } else {
            /* update later */
            (*p_full_pos) = 0;
        }
    }
    while ((Rval == OK) && (*p_full_pos != 0U)) {
        Tag = AmbaExif_MemGetBe16(pCurrentMemory);
        Length = AmbaExif_MemGetBe16(pCurrentMemory);
        if ((Length >= pImageInfo->FileSize) || (Length == 0U)) {
            //DEBUG_EXIFDEMUXER_ERR("@ExifParse: %s Tag 0x%x Length %d > fsize %d or == 0", pImageInfo->Name, Tag, Length, pImageInfo->FileSize);
            Rval = NG;
        }
        if ((Rval == OK) && ((Tag == (UINT32)AMBA_EXIF_SEGMENT_APP1) && (OffsetOfApp1 == 0U))) {
            /* APP1 0xFFE1 */
            OffsetOfApp1 = pCurrentMemory->Position;
            Base = AmbaExif_MemGetBe32(pCurrentMemory);
            if (Base != (UINT32)AMBA_EXIF_TIFF_HEADER) {
                /* keyword, Exif */
                //DEBUG_EXIFDEMUXER_ERR("@ExifParse: Expecting EXIF! %x", Base);
            }
            (void)MemGet16(pCurrentMemory); /* dont care */
            Base = pCurrentMemory->Position ;
            (void)MemGet32(pCurrentMemory); /* dont care */

            /* offset to IFD0 */
            TempOffset = MemGet32(pCurrentMemory) + Base;
            Rval = AmbaExif_MemSeek(pCurrentMemory, (INT32) TempOffset, MM_SEEK_SET);
            if (Rval != OK) {
                EXIFDLOG_NG(__func__, "Line %d", __LINE__, 0U);
            }
            if (Rval == OK) {
                Count = (INT32)MemGet16(pCurrentMemory);
                for (i = 0; i < Count; i++) {
                    /* skip IFD0 */
                    GetIfdEntry(pCurrentMemory, &Tag, &Type, &TempOffset, &Param);
                }

                /* offset to IFD1 */
                TempOffset = MemGet32(pCurrentMemory) + Base;

                /* Thumbnail IFD */
                if (TempOffset > (12U + Base)) {
                    GetIfd1(pCurrentMemory, pImageInfo, TempOffset, Base, Index);
                    Rval = AmbaExif_MemSeek(pCurrentMemory, (INT32) (pImageInfo->MPO[(Index - 1U)].ThumbPos), MM_SEEK_SET);
                    if (Rval != OK) {
                        EXIFDLOG_NG(__func__, "Line %d", __LINE__, 0U);
                    }
                    if (Rval == OK) {
                        Rval = AmbaExifDemuxer_CheckTag(pCurrentMemory, pImageInfo, (UINT8)(Index << 4U), pImageInfo->MPO[(Index - 1U)].ThumbPos - 2U); /* Add SOI */
                        if (Rval != OK) {
                            //DEBUG_EXIFDEMUXER_ERR("@ExifParse: TMB Unsupported");
                            pImageInfo->MPO[(Index - 1U)].ThumbPos = 0;
                        } else {
                            pImageInfo->Thumbnail++;
                        }
                    }
                } else {
                    //DEBUG_EXIFDEMUXER_ERR("@ExifParse: No IFD1");
                }
                if (Rval == OK) {
                    Rval = AmbaExif_MemSeek(pCurrentMemory,  ((INT32)OffsetOfApp1 + (INT32)Length) - 2, MM_SEEK_SET);
                    if (Rval != OK) {
                        EXIFDLOG_NG(__func__, "Line %d", __LINE__, 0U);
                    }
                }
            }
        } else {
            if ((Tag & 0xFFF0U) == 0xFFE0U) {
                /* AppX */
                (*p_full_size) -= Length;
                Rval = AmbaExif_MemSeek(pCurrentMemory, (INT32)Length - 2, MM_SEEK_CUR);
                if (Rval != OK) {
                    EXIFDLOG_NG(__func__, "Line %d", __LINE__, 0U);
                }
            } else {
                if (*p_full_pos != 0U) {
                    /* Check fullview */
                    TempOffset = pCurrentMemory->Position - 4U; /* DQT + DQT_len */
                    Rval = AmbaExif_MemSeek(pCurrentMemory, (INT32) TempOffset, MM_SEEK_SET);
                    if (Rval != OK) {
                        EXIFDLOG_NG(__func__, "Line %d", __LINE__, 0U);
                    }
                    if (Rval == OK) {
                        Rval = AmbaExifDemuxer_CheckTag(pCurrentMemory, pImageInfo, (UINT8)(Index << 4U) | (UINT8)AMBA_EXIF_FULLVIEW, TempOffset - 2U); /* Add SOI */
                        if (Rval != OK) {
                            EXIFDLOG_NG(__func__, "Line %d", __LINE__, 0U);
                        }
                    }
                    if (Rval == OK) {
                        pImageInfo->Fullview++;
                        (*p_full_pos) = TempOffset;
                    } else {
                        //DEBUG_EXIFDEMUXER_ERR("@ExifParse: %uth fullview Unsupported", Index);
                    }
                } else {
                    Rval = AmbaExif_MemSeek(pCurrentMemory, (INT32)Length - 2, MM_SEEK_CUR);
                    if (Rval != OK) {
                        EXIFDLOG_NG(__func__, "Line %d", __LINE__, 0U);
                    }
                }
            }
        }

        if (Rval == OK) {
            if (AmbaExif_MemEof(pCurrentMemory) == NG) {
                break;
            }
        }
    }
    return Rval;
}

UINT32 AmbaExifDemuxer_ParseMPFScrn(AMBA_EXIF_MEM *pCurrentMemory, AMBA_EXIF_IMAGE_INFO_s *pImageInfo, UINT8 Index)
{
    UINT32 Rval;
    UINT32 Tag, Len;
    UINT32 TempOffset, OriScreenPos = 0;
    UINT32 *pScreennailPos = (Index == 0U) ? &pImageInfo->ScreenPos : &pImageInfo->MPO[(Index - 1U)].ScreenPos;
    UINT32 *pScreennailSize = (Index == 0U) ? &pImageInfo->ScreenSize : &pImageInfo->MPO[(Index - 1U)].ScreenSize;

    //DEBUG_EXIFDEMUXER("=======================");
    //DEBUG_EXIFDEMUXER("[%s] pImageInfo->ScreenSize=%d  \r\n pImageInfo->ScreenPos=%d", __func__, pImageInfo->ScreenSize, pImageInfo->ScreenPos);
    //DEBUG_EXIFDEMUXER("=======================");

    Rval = AmbaExif_MemSeek(pCurrentMemory, 0, MM_SEEK_SET);
    if (Rval != OK) {
        EXIFDLOG_NG(__func__, "Line %d", __LINE__, 0U);
    } else {
        if (AmbaExif_MemGetBe16(pCurrentMemory) != (UINT32)AMBA_EXIF_SEGMENT_SOI) {
            /* SOI 0xFFD8 */
            //DEBUG_EXIFDEMUXER_ERR("[%s]: %uth screennail Cannot found SOI", __func__, Index);
            (*pScreennailPos) = 0;
            (*pScreennailSize) = 0;
            Rval = NG;
            EXIFDLOG_NG(__func__, "Line %d", __LINE__, 0U);
        } else {
            OriScreenPos = pImageInfo->ScreenPos;
            /* update later */
            (*pScreennailPos) = 0;
        }
    }

    while ((*pScreennailPos == 0U) && (Rval == OK)) {
        Tag = AmbaExif_MemGetBe16(pCurrentMemory);
        Len = AmbaExif_MemGetBe16(pCurrentMemory);
        if ((Len >= (UINT32)pImageInfo->FileSize) || (Len == 0U)) {
            //DEBUG_EXIFDEMUXER_ERR("ExifParse: %s Tag 0x%x Length %d > fsize %d or == 0", pImageInfo->Name, Tag, Len, pImageInfo->FileSize);
            EXIFDLOG_NG(__func__, "Line %d", __LINE__, 0U);
            Rval = NG;
        }

        if (Rval == OK) {
            if ((Tag & 0xFFF0U) == 0xFFE0U) {
                /* AppX */
                if (pImageInfo->MPOScreennailAPPXStart == 0xFFFFFFFFU) {
                    /* first time */
                    pImageInfo->MPOScreennailAPPXStart = pCurrentMemory->Position - 4U;
                }

                pImageInfo->MPOScreennailAPPXSize += (Len + 4U);
                (*pScreennailSize) -= Len;
                Rval = AmbaExif_MemSeek(pCurrentMemory, (INT32)Len - 2, MM_SEEK_CUR);
                if (Rval != OK) {
                    EXIFDLOG_NG(__func__, "Line %d", __LINE__, 0U);
                }
            } else {
                if (*pScreennailPos == 0U) {
                    /* Check screennail */
                    TempOffset = pCurrentMemory->Position - 4U; /* DQT + DQT_len */
                    Rval = AmbaExif_MemSeek(pCurrentMemory, (INT32) TempOffset, MM_SEEK_SET);
                    if (Rval != OK) {
                        EXIFDLOG_NG(__func__, "Line %d", __LINE__, 0U);
                    }

                    if (Rval == OK) {
                        Rval = AmbaExifDemuxer_CheckTag(pCurrentMemory, pImageInfo, (UINT8)(Index << 4U) | (UINT8)AMBA_EXIF_SCREENNAIL, TempOffset - 2U); /* Add SOI */
                    }

                    if (Rval == OK) {
                        pImageInfo->Screennail++;
                        if (pImageInfo->ScreenPos != 0U) {
                            pImageInfo->ScreenSoiOffset = TempOffset;
                        } else {
                            (*pScreennailPos) = TempOffset;
                        }
                    } else {
                        //DEBUG_EXIFDEMUXER_ERR("[%s]: %uth screennail Unsupported %d", __func__, Index, Rval);
                        EXIFDLOG_NG(__func__, "Line %d", __LINE__, 0U);
                    }
                } else {
                    Rval = AmbaExif_MemSeek(pCurrentMemory, (INT32)Len - 2, MM_SEEK_CUR);
                    if (Rval != OK) {
                        EXIFDLOG_NG(__func__, "Line %d", __LINE__, 0U);
                    }
                 }
            }
        }

        if (Rval == OK) {
            if (AmbaExif_MemEof(pCurrentMemory) == NG) {
                break;
            }
        }
    }

    if (OriScreenPos > 0U) {
        pImageInfo->ScreenPos += OriScreenPos - 2U;
    }

    //DEBUG_EXIFDEMUXER("=======================");
    //DEBUG_EXIFDEMUXER("[%s]#%d pImageInfo->ScreenPos=%d", __func__, __LINE__, pImageInfo->ScreenPos);
    //DEBUG_EXIFDEMUXER("[%s] pImageInfo->ScreenSize=%d  \r\n pImageInfo->ScreenPos=%d", __func__, pImageInfo->ScreenSize, pImageInfo->ScreenPos);
    //DEBUG_EXIFDEMUXER("=======================");

    return Rval;
}

UINT32 AmbaExifDemuxer_FastParseAPP1(AMBA_EXIF_MEM *pCurrentMemory, AMBA_EXIF_IMAGE_INFO_s *pImageInfo)
{
    UINT32 Rval;
    UINT32 TiffBaseOffset, OneStIfdOffset, ExifOffset = 0, Pos;
    //UINT32 ZeroThIfdOffset;

    TiffBaseOffset = pCurrentMemory->Position;

    /* Get endian type */
    (void)GetHeaderEndianType(pCurrentMemory);

    //ZeroThIfdOffset = MemGet32(pCurrentMemory) + TiffBaseOffset;
    /* ZeroThIfdOffset = MemGet32(pCurrentMemory) + TiffBaseOffset */
    (void)MemGet32(pCurrentMemory);

    /* Get IFD0 Start */
    FastGetIfd0(pCurrentMemory, pImageInfo, &ExifOffset);

    Pos = pCurrentMemory->Position;
    /* EXIF IFD */
    if (ExifOffset != 0U) {
        pImageInfo->WithExif = 1;
        ExifOffset += TiffBaseOffset;
        GetExifIfd(pCurrentMemory, pImageInfo, ExifOffset, TiffBaseOffset);
    }
    Rval = AmbaExif_MemSeek(pCurrentMemory, (INT32) Pos, MM_SEEK_SET);
    if (Rval != OK) {
        EXIFDLOG_NG(__func__, "Line %d", __LINE__, 0U);
    } else {
        /* Get IFD1 (thumbnail) Start */
        OneStIfdOffset = MemGet32(pCurrentMemory) + TiffBaseOffset;
        FastGetIfd1(pCurrentMemory, pImageInfo, OneStIfdOffset, TiffBaseOffset);
    }
    return Rval;
}

UINT32 AmbaExifDemuxer_CheckTag(AMBA_EXIF_MEM *pCurrentMemory, AMBA_EXIF_IMAGE_INFO_s *pImageInfo, UINT8 FullThumbScr, UINT32 TagStartPos)
{
    INT32 *pDhtStat, Done = 0;
    UINT32 TagA1, TagA2, Length, Type, *pWidth, *pHeight;
    UINT8 Temp, DQT = 0, DHT = 0, SOF = 0, SOS = 0;
    AMBA_EXIF_JPEG_HEADER_INFO_s *pJpegHeaderInfo;
    AMBA_EXIF_JPEG_INFO_s *pJpegInfo;
    UINT8 Temp2, Temp3;
    UINT32 Rval, DhtVal;

    //DEBUG_EXIFDEMUXER("[%s] START!!!    FullThumbScr=%X  TagStartPos=%d", __func__, FullThumbScr, TagStartPos);
    Rval = AmbaExif_MemSeek(pCurrentMemory, (INT32) TagStartPos, MM_SEEK_SET);
    if (Rval != OK) {
        EXIFDLOG_NG(__func__, "Line %d", __LINE__, 0U);
    }
    /* index */
    Temp = (FullThumbScr & 0xf0U) >> 4U;

    if (((FullThumbScr & 0x0fU) == (UINT8)AMBA_EXIF_FULLVIEW) && (Rval == OK)) {
        //DEBUG_EXIFDEMUXER("[%s] AMBA_EXIF_FULLVIEW !!!!", __func__);
        if (Temp != 0U) {
            pWidth = &pImageInfo->MPO[(Temp - 1U)].Width;
            pHeight = &pImageInfo->MPO[(Temp - 1U)].Height;
            pDhtStat = &pImageInfo->MPO[(Temp - 1U)].DHTStat;
            pJpegHeaderInfo = &pImageInfo->MPO[(Temp - 1U)].JPEGHeaderInfo;
            pJpegInfo = &pImageInfo->MPO[(Temp - 1U)].JPEGInfo;
        } else {
            pWidth = &pImageInfo->Width;
            pHeight = &pImageInfo->Height;
            pDhtStat = &pImageInfo->DHTStat;
            pJpegHeaderInfo = &pImageInfo->JPEGHeaderInfo;
            pJpegInfo = &pImageInfo->JPEGInfo;
        }
    } else {
        if ((FullThumbScr & 0x0fU) == (UINT8)AMBA_EXIF_THUMBNAIL) {
            //DEBUG_EXIFDEMUXER("[%s] THUMB NAIL!!!!", __func__);
            if (Temp != 0U) {
                pWidth = &pImageInfo->MPO[(Temp - 1U)].ThumbWidth;
                pHeight = &pImageInfo->MPO[(Temp - 1U)].ThumbHeight;
                pDhtStat = &pImageInfo->MPO[(Temp - 1U)].ThumbDHTStat;
                pJpegHeaderInfo = &pImageInfo->MPO[(Temp - 1U)].JPEGThumbHeaderInfo;
                pJpegInfo = &pImageInfo->MPO[(Temp - 1U)].JPEGThumbInfo;
            } else {
                pWidth = &pImageInfo->ThumbWidth;
                pHeight = &pImageInfo->ThumbHeight;
                pDhtStat = &pImageInfo->ThumbDHTStat;
                pJpegHeaderInfo = &pImageInfo->JPEGThumbHeaderInfo;
                pJpegInfo = &pImageInfo->JPEGThumbInfo;
            }
        } else {
            /* screennail */
            //DEBUG_EXIFDEMUXER("[%s] SCREEN NAIL!!!!", __func__);
            if (Temp != 0U) {
                pWidth = &pImageInfo->MPO[(Temp - 1U)].ScreenWidth;
                pHeight = &pImageInfo->MPO[(Temp - 1U)].ScreenHeight;
                pDhtStat = &pImageInfo->MPO[(Temp - 1U)].ScreenDHTStat;
                pJpegHeaderInfo = &pImageInfo->MPO[(Temp - 1U)].JPEGScreenHeaderInfo;
                pJpegInfo = &pImageInfo->MPO[(Temp - 1U)].JPEGScreenInfo;
            } else {
                pWidth = &pImageInfo->ScreenWidth;
                pHeight = &pImageInfo->ScreenHeight;
                pDhtStat = &pImageInfo->ScreenDHTStat;
                pJpegHeaderInfo = &pImageInfo->JPEGScreenHeaderInfo;
                pJpegInfo = &pImageInfo->JPEGScreenInfo;
            }
        }
    }
    //DEBUG_EXIFDEMUXER("[%s] pCurrentMemory->Position=%d", __func__, pCurrentMemory->Position);
    while ((AmbaExif_MemEof(pCurrentMemory) == OK) && (Rval == OK)) {
        TagA1 = AmbaExif_MemGetByte(pCurrentMemory);
        //DEBUG_EXIFDEMUXER("[%s] TagA1=%X     pCurrentMemory->Position=%d", __func__, TagA1, pCurrentMemory->Position);
        if (TagA1 != 0xFFU) {
            continue;
        }

        TagA2 = AmbaExif_MemGetByte(pCurrentMemory);
        //DEBUG_EXIFDEMUXER("[%s] TagA2=%X     pCurrentMemory->Position=%d", __func__, TagA2, pCurrentMemory->Position);
        if (((TagA2 & 0xF0U) == 0xc0U) && (TagA2 > 0xc4U)) {
            /* Only can support SOF0 */
            //DEBUG_EXIFDEMUXER("ExifParse: %x%x Found", TagA2, TagA2 & 0x00FFU);
            EXIFDLOG_NG(__func__, "Line %d", __LINE__, 0U);
            Rval = NG;
            Done = 1;
        }

        if (Rval == OK) {
            TagA1 = (TagA1 << 8U) | TagA2;

            switch (TagA1) {
            case AMBA_EXIF_SEGMENT_SOI:
                /* 0xFFD8: SOI */
                //DEBUG_EXIFDEMUXER("[%s]   0xFFD8: SOI Found Pos=%d", __func__, pCurrentMemory->Position);
                break;
            case AMBA_EXIF_SEGMENT_DQT:
                /* 0xFFDB: DQT (Define quatization tables) */
                if ((FullThumbScr == 2U) && (pImageInfo->ScreenPos == 0U)) {
                    pImageInfo->ScreenPos = pCurrentMemory->Position;
                }
                Length = AmbaExif_MemGetBe16(pCurrentMemory);
                pJpegHeaderInfo->DQT.Offset[pJpegHeaderInfo->DQT.MarkerNumber] = pCurrentMemory->Position - TagStartPos;
                //DEBUG_EXIFDEMUXER("[%s]    0xFFDB: DQT (Define quatization tables)   LINE:#%d     TagStartPos=%d", __func__, __LINE__, TagStartPos);
                pJpegHeaderInfo->DQT.Length[pJpegHeaderInfo->DQT.MarkerNumber] = Length;
                //DEBUG_EXIFDEMUXER("[%s]    0xFFDB: DQT (Define quatization tables)   Length=%d", __func__, Length);

                pJpegHeaderInfo->DQT.MarkerNumber++;
                if (pJpegHeaderInfo->DQT.MarkerNumber > 3) {
                    //DEBUG_EXIFDEMUXER_ERR("ExifParse: Too many DQTs, max number is 3");
                    EXIFDLOG_NG(__func__, "Line %d", __LINE__, 0U);
                    Rval = NG;
                    Done = 1;
                } else {
                    Rval = AmbaExif_MemSeek(pCurrentMemory, (INT32)Length - 2, MM_SEEK_CUR);
                    if (Rval != OK) {
                        EXIFDLOG_NG(__func__, "Line %d", __LINE__, 0U);
                    }
                    DQT = 1;
                }
                break;
            case AMBA_EXIF_SEGMENT_SOF:
                /* 0xFFC0: SOF0 */
                /* Lf: Frame header length */
                Length = AmbaExif_MemGetBe16(pCurrentMemory) - 2U;
                /* P: Sample precision. We can only support 8 */
                Temp = (UINT8)AmbaExif_MemGetByte(pCurrentMemory);
                //DEBUG_EXIFDEMUXER("[%s]    0xFFC0: SOF0   Length=%d", __func__, Length);
                if (Temp != 8U) {
                    //DEBUG_EXIFDEMUXER_ERR("ExifParse: Precision != 8. Not support  %x, Pos %x", Temp, pCurrentMemory->Position);
                    /* Precision != 8 */
                    Rval = NG;
                    Done = 1;
                    EXIFDLOG_NG(__func__, "Line %d", __LINE__, 0U);
                }
                if (Rval == OK) {
                    pJpegHeaderInfo->SOF.Length = Length + 2U;
                    pJpegHeaderInfo->SOF.Offset = (pCurrentMemory->Position - 1U) - TagStartPos; /* Start from P: Sample precision */
                    *pHeight = AmbaExif_MemGetBe16(pCurrentMemory);         /* Y: Number of lines */
                    pJpegInfo->YImage = *pHeight;
                    *pWidth = AmbaExif_MemGetBe16(pCurrentMemory);          /* X: Number of samples per line */
                    pJpegInfo->XImage = *pWidth;
                    Temp = (UINT8)AmbaExif_MemGetByte(pCurrentMemory);          /* Nf: Number of image components in frame. We can only support 3 */
                    pJpegInfo->Component = Temp;
                    if (Temp != 3U) {
                        //DEBUG_EXIFDEMUXER_ERR("ExifParse: Need 3 components. Not support %x , Pos %x", Temp, pCurrentMemory->Position );
                        /* Only support Y + U + V */
                        Rval = NG;
                        Done = 1;
                        EXIFDLOG_NG(__func__, "Line %d", __LINE__, 0U);
                        break;
                    } else {
                        (void)AmbaExif_MemGetByte(pCurrentMemory);          /* Ci: Component identifier. (Y component) */
                        Temp = (UINT8)AmbaExif_MemGetByte(pCurrentMemory);   /* Hi: Horizontal sampling factor (4 bits), Vi: Vertical sampling factor (4 bits). (Y component) */
                        pJpegInfo->YH = (Temp >> 4U);
                        pJpegInfo->YV = (Temp & 0xFU);
                        (void)AmbaExif_MemGetByte(pCurrentMemory);          /* Tq: Quantization table destination selector. (Y component) */
                        (void)AmbaExif_MemGetByte(pCurrentMemory);          /* Ci: Component identifier. (Cb component) */
                        Temp2 = (UINT8)AmbaExif_MemGetByte(pCurrentMemory);/* Hi: Horizontal sampling factor (4 bits), Vi: Vertical sampling factor (4 bits). (Cb component) */
                        AmbaMisra_TouchUnused(&Temp2);

                        (void)AmbaExif_MemGetByte(pCurrentMemory);          /* Temp2 */

                        (void)AmbaExif_MemGetByte(pCurrentMemory);          /* Tq: Quantization table destination selector. (Cb component) */
                        (void)AmbaExif_MemGetByte(pCurrentMemory);          /* Ci: Component identifier. (Cr component) */

                        Temp3 = (UINT8)AmbaExif_MemGetByte(pCurrentMemory);/* Hi: Horizontal sampling factor (4 bits), Vi: Vertical sampling factor (4 bits). (Cr component) */
                        AmbaMisra_TouchUnused(&Temp3);

                        (void)AmbaExif_MemGetByte(pCurrentMemory);          /* Temp 3 */

                        if (FullThumbScr == (UINT8)AMBA_EXIF_FULLVIEW) {
                            pImageInfo->YUVType = Temp;
                        }
                        if ((Temp != 0x22U) && (Temp != 0x21U)) {
                            //DEBUG_EXIFDEMUXER_ERR("ExifParse: Subsample error, Not support  Y (%d, %d); Cb (%d, %d); Cr (%d, %d).", (Temp >> 4U),
                                                  //(Temp & 0xFU), (Temp2 >> 4U), (Temp2 & 0xFU), (Temp3 >> 4U), (Temp3 & 0xFU));
                            /* 420, 422 only */
                            /* error tolerant: let decoder decode even if subsample type is not supported */
                            //Rval = -3;
                            //goto done;
                        }
                        (void)AmbaExif_MemGetByte(pCurrentMemory);          /* Tq: Quantization table destination selector. (Cr component) */
                        Rval = AmbaExif_MemSeek(pCurrentMemory,  ((INT32)Length - 8) - 7, MM_SEEK_CUR);
                        if (Rval != OK) {
                            EXIFDLOG_NG(__func__, "Line %d", __LINE__, 0U);
                        }
                        SOF = 1;
                    }
                }
                break;
            case 0xC1:
            case 0xC2:
            case 0xC3:
                /* 0xFFC1(SOF1), 0xFFC2(SOF2), 0xFFC3(SOF3): not support now */
                //DEBUG_EXIFDEMUXER_ERR("ExifParse: We do not support SOF%d now, Pos %x", (TagA2 & 0xFU), pCurrentMemory->Position);
                Rval = NG;
                Done = 1;
                EXIFDLOG_NG(__func__, "Line %d", __LINE__, 0U);
                break;
            case AMBA_EXIF_SEGMENT_DHT:
                /* 0xFFC4: DHT (Huffman table) */
                Length = AmbaExif_MemGetBe16(pCurrentMemory);

                if (pJpegHeaderInfo->DHT.MarkerNumber <= 3) {
                    pJpegHeaderInfo->DHT.Offset[pJpegHeaderInfo->DHT.MarkerNumber] = pCurrentMemory->Position - TagStartPos;
                    pJpegHeaderInfo->DHT.Length[pJpegHeaderInfo->DHT.MarkerNumber] = Length;
                }
                pJpegHeaderInfo->DHT.MarkerNumber++;

                if (pJpegHeaderInfo->DHT.MarkerNumber > 4) {
                    //DEBUG_EXIFDEMUXER_ERR("ExifParse: Too many DHTs, max number is 4");
                    Rval = NG;
                    Done = 1;
                    EXIFDLOG_NG(__func__, "Line %d", __LINE__, 0U);
                } else {
                    Type = AmbaExif_MemGetByte(pCurrentMemory);
                    DhtVal = CheckDHT(pCurrentMemory, (UINT8)Type, (UINT16)Length);
                    *pDhtStat = (INT32)DhtVal;
                    DHT = 1;
                    //DEBUG_EXIFDEMUXER("[%s]    0xFFC4: DHT (Huffman table)   END", __func__);
                }
                break;
            case AMBA_EXIF_SEGMENT_EOI:
                /* 0xFFD9 */
                break;
            case AMBA_EXIF_SEGMENT_DRI:
                /* 0xFFDD: DRI (define restart interval) */
                Length = AmbaExif_MemGetBe16(pCurrentMemory);
                pJpegHeaderInfo->DRI.Offset = pCurrentMemory->Position - TagStartPos;
                pJpegHeaderInfo->DRI.Length = Length;
                pJpegInfo->MCURestart = MemGet16(pCurrentMemory);
                Rval = AmbaExif_MemSeek(pCurrentMemory,  (INT32)Length - 4, MM_SEEK_CUR);
                if (Rval != OK) {
                    EXIFDLOG_NG(__func__, "Line %d", __LINE__, 0U);
                }
                break;
            case AMBA_EXIF_SEGMENT_SOS:
                /* 0xFFDA: SOS (Start of scan) */
                Length = AmbaExif_MemGetBe16(pCurrentMemory);
                pJpegHeaderInfo->SOS.Offset = pCurrentMemory->Position - TagStartPos;
                pJpegHeaderInfo->SOS.Length = Length;
                //DEBUG_EXIFDEMUXER("[%s]    0xFFDA: SOS (Start of scan)   Length=%d    offset=%d", __func__, Length, pJpegHeaderInfo->SOS.Offset);
                Rval = OK;
                SOS = 1;
                Done = 1;
                break;
            default:
                /* get Next header */
                Length = AmbaExif_MemGetBe16(pCurrentMemory);
                if (Length != 0xFFFFFFFFU) {
                    Rval = AmbaExif_MemSeek(pCurrentMemory, (INT32)Length - 2, MM_SEEK_CUR);
                    if (Rval != OK) {
                        //pass vcast
                    }
                }
                break;
            }
        }
    }

    if (Done == 0) {
        //DEBUG_EXIFDEMUXER_ERR("ExifParse: EOS, no FFDA:SOS ?! %s", pImageInfo->Name);
        #if 0
        EXIFDLOG_NG(__func__, "Line %d", __LINE__, 0U);
        SVC_WRAP_PRINT "ExifParse: EOS, no FFDA:SOS %s"
        SVC_PRN_ARG_S __func__
        SVC_PRN_ARG_PROC EXIFDLOG_NG
        SVC_PRN_ARG_STR  pImageInfo->Name
        SVC_PRN_ARG_E
        #endif
        Rval = NG;
    }

    if (((DQT != 0U) || (DHT != 0U)) || ((SOF != 0U) || (SOS != 0U))) {
        //DEBUG_EXIFDEMUXER_ERR("insufficient maker. DQT %d DHT %d SOF %d SOS %d", DQT, DHT, SOF, SOS);
        #if 0
        EXIFDLOG_NG(__func__, "Line %d", __LINE__, 0U);
        SVC_WRAP_PRINT "insufficient maker. DQT %d DHT %d SOF %d SOS %d"
        SVC_PRN_ARG_S __func__
        SVC_PRN_ARG_PROC EXIFDLOG_NG
        SVC_PRN_ARG_UINT8 DQT
        SVC_PRN_ARG_UINT8 DHT
        SVC_PRN_ARG_UINT8 SOF
        SVC_PRN_ARG_UINT8 SOS
        SVC_PRN_ARG_E
        #endif
        Rval = OK;
    }

    //DEBUG_EXIFDEMUXER("[%s]     END", __func__);
    return Rval;
}

static void GetIfd0(AMBA_EXIF_MEM *pCurrentMemory, AMBA_EXIF_IMAGE_INFO_s *pImageInfo, UINT32 TiffBaseOffset, UINT32 *pExifOffset, UINT32 *pDateOffset, UINT32 *pGpsOffset)
{
    UINT32 i, Count;
    UINT32 Tag = 0U, Type = 0U, Count1 = 0U, Param = 0U, Pos = 0;

    Count = MemGet16(pCurrentMemory);
    //DEBUG_EXIFDEMUXER("[%s]===== Count = %X", __func__, Count);

    for (i = 0; i < Count; i++) {
        GetIfdEntry(pCurrentMemory, &Tag, &Type, &Count1, &Param);
        switch (Tag) {
        case (UINT32)AMBA_EXIF_TIFF_IMG_DESC:
            Pos = pCurrentMemory->Position;
            pImageInfo->ImageDescriptionSize = Count1;
            if (Count1 <= 4U) {
                pImageInfo->ImageDescriptionOffset = pCurrentMemory->Position - 4U;
            } else {
                pImageInfo->ImageDescriptionOffset = Param + TiffBaseOffset;
            }
            if (OK != AmbaExif_MemSeek(pCurrentMemory, (INT32) pImageInfo->ImageDescriptionOffset, MM_SEEK_SET)) {
                EXIFDLOG_NG(__func__, "Line %d", __LINE__, 0U);
            }
            (void)AmbaExif_MemRead(pCurrentMemory, pImageInfo->ImageDescription, 1, pImageInfo->ImageDescriptionSize);
            if (OK != AmbaExif_MemSeek(pCurrentMemory, (INT32) Pos, MM_SEEK_SET)) {
                EXIFDLOG_NG(__func__, "Line %d", __LINE__, 0U);
            }
            //DEBUG_EXIFDEMUXER("[%s] TIFF_ImageDescription  Count1=%d  pImageInfo->ImageDescriptionOffset=0x%X", __func__, Count1,
                              //pImageInfo->ImageDescriptionOffset);
            //DEBUG_EXIFDEMUXER("[%s] TIFF_ImageDescription  pImageInfo->ImageDescription=%s", __func__, pImageInfo->ImageDescription);
            break;
        case (UINT32)AMBA_EXIF_TIFF_MAKE:
            Pos = pCurrentMemory->Position;
            pImageInfo->MakeSize = Count1;
            if (Count1 <= 4U) {
                pImageInfo->MakeOffset = pCurrentMemory->Position - 4U;
            } else {
                pImageInfo->MakeOffset = Param + TiffBaseOffset;
            }
            if (OK != AmbaExif_MemSeek(pCurrentMemory, (INT32) pImageInfo->MakeOffset, MM_SEEK_SET)) {
                EXIFDLOG_NG(__func__, "Line %d", __LINE__, 0U);
            }
            (void)AmbaExif_MemRead(pCurrentMemory, pImageInfo->Make, 1, pImageInfo->MakeSize);
            if (OK != AmbaExif_MemSeek(pCurrentMemory, (INT32) Pos, MM_SEEK_SET)) {
                EXIFDLOG_NG(__func__, "Line %d", __LINE__, 0U);
            }
            //DEBUG_EXIFDEMUXER("[%s] TIFF_Make  Count1=%d  pImageInfo->MakeOffset=0x%X", __func__, Count1, pImageInfo->MakeOffset);
            //DEBUG_EXIFDEMUXER("[%s] TIFF_Make  pImageInfo->Make=%s", __func__, pImageInfo->Make);
            break;
        case (UINT32)AMBA_EXIF_TIFF_MODEL:
            Pos = pCurrentMemory->Position;
            pImageInfo->ModelSize = Count1;
            if (Count1 <= 4U) {
                pImageInfo->ModelOffset = pCurrentMemory->Position - 4U;
            } else {
                pImageInfo->ModelOffset = Param + TiffBaseOffset;
            }
            if (OK != AmbaExif_MemSeek(pCurrentMemory, (INT32) pImageInfo->ModelOffset, MM_SEEK_SET)) {
                EXIFDLOG_NG(__func__, "Line %d", __LINE__, 0U);
            }
            (void)AmbaExif_MemRead(pCurrentMemory, pImageInfo->Model, 1, pImageInfo->ModelSize);
            if (OK != AmbaExif_MemSeek(pCurrentMemory, (INT32) Pos, MM_SEEK_SET)) {
                EXIFDLOG_NG(__func__, "Line %d", __LINE__, 0U);
            }
            //DEBUG_EXIFDEMUXER("[%s] TIFF_Model  Count1=%d  pImageInfo->Model_offset=0x%X", __func__, Count1, pImageInfo->ModelOffset);
            //DEBUG_EXIFDEMUXER("[%s] TIFF_Model  pImageInfo->Model=%s", __func__, pImageInfo->Model);
            break;
        case (UINT32)AMBA_EXIF_TIFF_ORIENTATION:
            pImageInfo->OrientationOffset = pCurrentMemory->Position - 4U;
            pImageInfo->Orientation = (UINT8)Param;
            //DEBUG_EXIFDEMUXER("[%s] TIFF_orientation  Param=%d", __func__, Param);
            break;
        case (UINT32)AMBA_EXIF_TIFF_SOFTWARE:
            Pos = pCurrentMemory->Position;
            pImageInfo->SoftwareSize = Count1;
            if (Count1 <= 4U) {
                pImageInfo->SoftwareOffset = pCurrentMemory->Position - 4U;
            } else {
                pImageInfo->SoftwareOffset = Param + TiffBaseOffset;
            }
            if (OK != AmbaExif_MemSeek(pCurrentMemory, (INT32) pImageInfo->SoftwareOffset, MM_SEEK_SET)) {
                EXIFDLOG_NG(__func__, "Line %d", __LINE__, 0U);
            }
            (void)AmbaExif_MemRead(pCurrentMemory, pImageInfo->Software, 1, pImageInfo->SoftwareSize);
            if (OK != AmbaExif_MemSeek(pCurrentMemory, (INT32) Pos, MM_SEEK_SET)) {
                EXIFDLOG_NG(__func__, "Line %d", __LINE__, 0U);
            }
            //DEBUG_EXIFDEMUXER("[%s] TIFF_Software  Count1=%d  pImageInfo->SoftwareOffset=0x%X", __func__, Count1, pImageInfo->SoftwareOffset);
            //DEBUG_EXIFDEMUXER("[%s] TIFF_Software  pImageInfo->Software= %s", __func__, pImageInfo->Software);
            break;
        case (UINT32)AMBA_EXIF_TIFF_DATETIME:
            *pDateOffset = Param;
            Pos = pCurrentMemory->Position;
            if (OK != AmbaExif_MemSeek(pCurrentMemory, (INT32)Param + (INT32)TiffBaseOffset, MM_SEEK_SET)) {
                EXIFDLOG_NG(__func__, "Line %d", __LINE__, 0U);
            }
            (void)AmbaExif_MemRead(pCurrentMemory, pImageInfo->Date, 1, 20);
            if (OK != AmbaExif_MemSeek(pCurrentMemory, (INT32) Pos, MM_SEEK_SET)) {
                EXIFDLOG_NG(__func__, "Line %d", __LINE__, 0U);
            }
            //DEBUG_EXIFDEMUXER("[%s] TIFF_DateTime  pImageInfo->Date= %s", __func__, pImageInfo->Date);
            break;
        case (UINT32)AMBA_EXIF_TIFF_EXIF_IFD_POINTER:
            *pExifOffset = Param;
            //DEBUG_EXIFDEMUXER("[%s] TIFF_ExifIFDPointer  Param=%d", __func__, Param);
            break;
        case (UINT32)AMBA_EXIF_TIFF_GPS_INFO_IFD_POINTER:
            *pGpsOffset = Param;
            //DEBUG_EXIFDEMUXER("[%s] TIFF_GPSInfoIFDPointer  Param=%d", __func__, Param);
            break;
        default:
            //DEBUG_EXIFDEMUXER_ERR("Non-parsed Tag %d Type %d Count1 %d Param %d", Tag, Type, Count1, Param);
            break;
        }
    }
    //DEBUG_EXIFDEMUXER("[%s] END!!!", __func__);
}

static void GetIfd1(AMBA_EXIF_MEM *pCurrentMemory, AMBA_EXIF_IMAGE_INFO_s *pImageInfo, UINT32 Offset, UINT32 TiffBaseOffset, UINT8 MPOIndex)
{
    UINT32 i, Count;
    UINT32 Tag, Type, Count1, Param = 0U;
    UINT32 *pThumbnailPos = (MPOIndex == 0U) ? &pImageInfo->ThumbPos : &pImageInfo->MPO[(MPOIndex - 1U)].ThumbPos;
    UINT32 *pThumbnailSize = (MPOIndex == 0U) ? &pImageInfo->ThumbSize : &pImageInfo->MPO[(MPOIndex - 1U)].ThumbSize;

    //DEBUG_EXIFDEMUXER("[%s] START!!!     pCurrentMemory->Position=%d", __func__, pCurrentMemory->Position);
    if (OK != AmbaExif_MemSeek(pCurrentMemory, (INT32) Offset, MM_SEEK_SET)) {
        EXIFDLOG_NG(__func__, "Line %d", __LINE__, 0U);
    }
    Count = MemGet16(pCurrentMemory);

    for (i = 0; i < Count; i++) {
        GetIfdEntry(pCurrentMemory, &Tag, &Type, &Count1, &Param);
        switch (Tag) {
        case (UINT32)AMBA_EXIF_TIFF_ORIENTATION:
            pImageInfo->ThmOrientationOffset = pCurrentMemory->Position - 4U;
            pImageInfo->ThumbOrientation = (UINT8)Param;
            break;
        case (UINT32)AMBA_EXIF_TIFF_JPG_INTRCHG_FMT:
            (*pThumbnailPos) = (Param + TiffBaseOffset) + 2U;
            break;
        case (UINT32)AMBA_EXIF_TIFF_JPG_INTRCHG_FMT_LENGTH:
            (*pThumbnailSize) = Param;
            break;
        default:
            //DEBUG_EXIFDEMUXER_ERR("Non-parsed Tag %d Type %d Count1 %d Param %d", Tag, Type, Count1, Param);
            break;
        }
    }
    //DEBUG_EXIFDEMUXER("[%s] END!!!     pCurrentMemory->Position=%d", __func__, pCurrentMemory->Position);
}

static void GetExifIfd(AMBA_EXIF_MEM *pCurrentMemory, AMBA_EXIF_IMAGE_INFO_s *pImageInfo, UINT32 IfdOffset, UINT32 TiffBaseOffset)
{
    //INT32 Rval;
    UINT32 i, Count;
    UINT32 Tag = 0U, Type = 0U, Count1 = 0U, Param = 0U, Pos = 0;

    //DEBUG_EXIFDEMUXER("[%s]===== IFDoffset=%d  TiffBaseOffset=%d", __func__, IfdOffset, TiffBaseOffset);

    (void)AmbaExif_MemSeek(pCurrentMemory, (INT32) IfdOffset, MM_SEEK_SET);
    //IF_SEEK_FAIL("AmbaExif_MemSeek Fail")

    Count = MemGet16(pCurrentMemory);
    //DEBUG_EXIFDEMUXER("[%s]===== count = %d", __func__, Count);

    for (i = 0; i < Count; i++) {
        GetIfdEntry(pCurrentMemory, &Tag, &Type, &Count1, &Param);
        switch (Tag) {
        case AMBA_EXIF_IFD_EXPOSURE_TIME:
            Pos = pCurrentMemory->Position;
            (void)AmbaExif_MemSeek(pCurrentMemory, (INT32)Param + (INT32)TiffBaseOffset, MM_SEEK_SET);
            //IF_SEEK_FAIL("AmbaExif_MemSeek Fail")
            pImageInfo->PAAA.ExposureTimeNum = MemGet32(pCurrentMemory);
            pImageInfo->PAAA.ExposureTimeDen = MemGet32(pCurrentMemory);
            (void)AmbaExif_MemSeek(pCurrentMemory, (INT32) Pos, MM_SEEK_SET);
            //IF_SEEK_FAIL("AmbaExif_MemSeek Fail")
            //DEBUG_EXIFDEMUXER("[%s] AMBA_EXIF_IFD_ExposureTime  \r\n exposure_time_num=%d exposure_time_den=%d", __func__,
                              //pImageInfo->PAAA.ExposureTimeNum, pImageInfo->PAAA.ExposureTimeDen);
            break;
        case AMBA_EXIF_IFD_F_NUMBER:
            Pos = pCurrentMemory->Position;
            (void)AmbaExif_MemSeek(pCurrentMemory, (INT32)Param + (INT32)TiffBaseOffset, MM_SEEK_SET);
            //IF_SEEK_FAIL("AmbaExif_MemSeek Fail")
            pImageInfo->PAAA.FNumberNum = MemGet32(pCurrentMemory);
            pImageInfo->PAAA.FNumberDen = MemGet32(pCurrentMemory);
            (void)AmbaExif_MemSeek(pCurrentMemory, (INT32) Pos, MM_SEEK_SET);
            //IF_SEEK_FAIL("AmbaExif_MemSeek Fail")
            //DEBUG_EXIFDEMUXER("[%s] AMBA_EXIF_IFD_FNumber  FNumber_num=  %d   FNumber_den=%d", __func__, pImageInfo->PAAA.FNumberNum,
                              //ImageInfo->PAAA.FNumberDen);
            break;
        case AMBA_EXIF_IFD_EXPOSURE_PROGRAM:
            pImageInfo->PAAA.ExposureProgram = Param;
            break;
        case AMBA_EXIF_IFD_SPECTRAL_SENSITIVITY:
            pImageInfo->PAAA.PhotoGraphicSentivity = (UINT16)Param;
            break;
        case AMBA_EXIF_IFD_DATE_TIME_ORIGINAL:
            Pos = pCurrentMemory->Position;
            (void)AmbaExif_MemSeek(pCurrentMemory, (INT32)Param + (INT32)TiffBaseOffset, MM_SEEK_SET);
            //IF_SEEK_FAIL("AmbaExif_MemSeek Fail")
            (void)AmbaExif_MemRead(pCurrentMemory, pImageInfo->PAAA.DateTimeOriginal, 1, (UINT32)AMBA_EXIF_TIFF_DATETIME_SIZE);
            (void)AmbaExif_MemSeek(pCurrentMemory, (INT32) Pos, MM_SEEK_SET);
            //IF_SEEK_FAIL("AmbaExif_MemSeek Fail")
            //DEBUG_EXIFDEMUXER("[%s] AMBA_EXIF_IFD_DateTimeOriginal  pImageInfo->PAAA.datetime_original= %s", __func__,
                              //pImageInfo->PAAA.DateTimeOriginal);
            break;
        case AMBA_EXIF_IFD_DATE_TIME_DIGITIZED:
            Pos = pCurrentMemory->Position;
            (void)AmbaExif_MemSeek(pCurrentMemory, (INT32)Param + (INT32)TiffBaseOffset, MM_SEEK_SET);
            //IF_SEEK_FAIL("AmbaExif_MemSeek Fail")
            (void)AmbaExif_MemRead(pCurrentMemory, pImageInfo->PAAA.DateTimeDigitized, 1, (UINT32)AMBA_EXIF_TIFF_DATETIME_SIZE);
            (void)AmbaExif_MemSeek(pCurrentMemory, (INT32) Pos, MM_SEEK_SET);
            //IF_SEEK_FAIL("AmbaExif_MemSeek Fail")
            //DEBUG_EXIFDEMUXER("[%s] AMBA_EXIF_IFD_DateTimeDigitized  pImageInfo->PAAA.dateTime_digitized= %s", __func__,
                              //pImageInfo->PAAA.DateTimeDigitized);
            break;
        case AMBA_EXIF_IFD_SHUTTER_SPEED_VALUE:
            Pos = pCurrentMemory->Position;
            (void)AmbaExif_MemSeek(pCurrentMemory, (INT32)Param + (INT32)TiffBaseOffset, MM_SEEK_SET);
            //IF_SEEK_FAIL("AmbaExif_MemSeek Fail")
            pImageInfo->PAAA.ShutterSpeedNum = MemGet32(pCurrentMemory);
            pImageInfo->PAAA.ShutterSpeedDen = MemGet32(pCurrentMemory);
            (void)AmbaExif_MemSeek(pCurrentMemory, (INT32) Pos, MM_SEEK_SET);
            //IF_SEEK_FAIL("AmbaExif_MemSeek Fail")
            break;
        case AMBA_EXIF_IFD_APERTURE_VALUE:
            Pos = pCurrentMemory->Position;
            (void)AmbaExif_MemSeek(pCurrentMemory, (INT32)Param + (INT32)TiffBaseOffset, MM_SEEK_SET);
            //IF_SEEK_FAIL("AmbaExif_MemSeek Fail")
            pImageInfo->PAAA.ApertureValueNum = MemGet32(pCurrentMemory);
            pImageInfo->PAAA.ApertureValueDen = MemGet32(pCurrentMemory);
            (void)AmbaExif_MemSeek(pCurrentMemory, (INT32) Pos, MM_SEEK_SET);
            //IF_SEEK_FAIL("AmbaExif_MemSeek Fail")
            break;
        case AMBA_EXIF_IFD_EXPOSURE_BIAS_VALUE:
            Pos = pCurrentMemory->Position;
            (void)AmbaExif_MemSeek(pCurrentMemory, (INT32)Param + (INT32)TiffBaseOffset, MM_SEEK_SET);
            //IF_SEEK_FAIL("AmbaExif_MemSeek Fail")
            pImageInfo->PAAA.ExposureBiasValueNum = MemGet32(pCurrentMemory);
            pImageInfo->PAAA.ExposureBiasValueDen = MemGet32(pCurrentMemory);
            (void)AmbaExif_MemSeek(pCurrentMemory, (INT32) Pos, MM_SEEK_SET);
            //IF_SEEK_FAIL("AmbaExif_MemSeek Fail")
            //DEBUG_EXIFDEMUXER("[%s] AMBA_EXIF_IFD_ExposureBiasValue  exposure_bias_value_num=%d exposure_bias_value_den=%d", __func__,
                              //pImageInfo->PAAA.ExposureBiasValueNum, pImageInfo->PAAA.ExposureBiasValueDen);
            break;
        case AMBA_EXIF_IFD_MAX_APERTURE_VALUE:
            Pos = pCurrentMemory->Position;
            (void)AmbaExif_MemSeek(pCurrentMemory, (INT32)Param + (INT32)TiffBaseOffset, MM_SEEK_SET);
            //IF_SEEK_FAIL("AmbaExif_MemSeek Fail")
            pImageInfo->PAAA.MaxApertureValueNum = MemGet32(pCurrentMemory);
            pImageInfo->PAAA.MaxApertureValueDen = MemGet32(pCurrentMemory);
            (void)AmbaExif_MemSeek(pCurrentMemory, (INT32) Pos, MM_SEEK_SET);
            //IF_SEEK_FAIL("AmbaExif_MemSeek Fail")
            //DEBUG_EXIFDEMUXER("[%s] AMBA_EXIF_IFD_MaxApertureValue  max_aperture_value_num=%d max_aperture_value_den=%d", __func__,
                              //pImageInfo->PAAA.MaxApertureValueNum, pImageInfo->PAAA.MaxApertureValueDen);
            break;
        case AMBA_EXIF_IFD_SUBJECT_DISTANCE:
            Pos = pCurrentMemory->Position;
            (void)AmbaExif_MemSeek(pCurrentMemory, (INT32)Param + (INT32)TiffBaseOffset, MM_SEEK_SET);
            //IF_SEEK_FAIL("AmbaExif_MemSeek Fail")
            pImageInfo->PAAA.SubjectDistanceRange = MemGet32(pCurrentMemory);
            (void)AmbaExif_MemSeek(pCurrentMemory, (INT32) Pos, MM_SEEK_SET);
            //IF_SEEK_FAIL("AmbaExif_MemSeek Fail")
            break;
        case AMBA_EXIF_IFD_METERING_MODE:
            pImageInfo->PAAA.MeteringMode = (UINT16)Param;
            break;
        case AMBA_EXIF_IFD_LIGHT_SOURCE:
            pImageInfo->PAAA.LightSource = (UINT16)Param;
            break;
        case AMBA_EXIF_IFD_FLASH:
            pImageInfo->PAAA.Flash = (UINT16)Param;
            break;
        case AMBA_EXIF_IFD_FOCAL_LENGTH:
            Pos = pCurrentMemory->Position;
            (void)AmbaExif_MemSeek(pCurrentMemory, (INT32)Param + (INT32)TiffBaseOffset, MM_SEEK_SET);
            //IF_SEEK_FAIL("AmbaExif_MemSeek Fail")
            pImageInfo->PAAA.FocalLengthNum = MemGet32(pCurrentMemory);
            pImageInfo->PAAA.FocalLengthDen = MemGet32(pCurrentMemory);
            (void)AmbaExif_MemSeek(pCurrentMemory, (INT32) Pos, MM_SEEK_SET);
            //IF_SEEK_FAIL("AmbaExif_MemSeek Fail")
            break;
        case AMBA_EXIF_IFD_MAKER_NOTE:
            Pos = pCurrentMemory->Position;
            pImageInfo->MakernoteSize = Count1;
            if (Count1 <= 4U) {
                pImageInfo->MakernoteOffset = Pos - 4U;
            } else {
                UINT32 TempPos = (((Param + TiffBaseOffset) + Count1) - 2U);
                pImageInfo->MakernoteOffset = Param + TiffBaseOffset;
                /* Copy makernote to pImageInfo->aaa */
#if PAAACopyMode
                {
                    (void)AmbaExif_MemSeek(pCurrentMemory, (INT32) pImageInfo->MakernoteOffset, MM_SEEK_SET);
                    //IF_SEEK_FAIL("AmbaExif_MemSeek Fail")
                    (void)AmbaExif_MemRead(pCurrentMemory, MakerNoteBuffer, 1, Count1);

                    pImageInfo->PAAA.pMakerNoteData = &MakerNoteBuffer;
                }
#endif
                (void)AmbaExif_MemSeek(pCurrentMemory, (INT32) TempPos, MM_SEEK_SET);
                //IF_SEEK_FAIL("AmbaExif_MemSeek Fail")
                //magicat
                // todo PARSE MAKERNOTE
                //                AmbaPrintColor(RED,"pCurrentMemory->Position=%d", pCurrentMemory->Position);
                //                AmbaPrintColor(RED,"pImageInfo->MakernoteOffset=%d", pImageInfo->MakernoteOffset);
                //                AmbaPrintColor(RED,"pImageInfo->MakernoteSize=%d", pImageInfo->MakernoteSize);
                //                AmbaPrintColor(RED,"TempPos=%d", TempPos);
                (void)AmbaExif_MemSeek(pCurrentMemory, (INT32) Pos, MM_SEEK_SET);
                //IF_SEEK_FAIL("AmbaExif_MemSeek Fail")
            }

            //DEBUG_EXIFDEMUXER("=======================================================");
            //DEBUG_EXIFDEMUXER("[%s] pImageInfo->MakernoteSize = %d", __func__, pImageInfo->MakernoteSize);
            //DEBUG_EXIFDEMUXER("[%s] pImageInfo->Screennail = %d", __func__, pImageInfo->Screennail);
            //DEBUG_EXIFDEMUXER("[%s] pImageInfo->SecondPicType = %d", __func__, pImageInfo->SecondPicType);
            //DEBUG_EXIFDEMUXER("[%s] pImageInfo->ScreenPos = %d", __func__, pImageInfo->ScreenPos);
            //DEBUG_EXIFDEMUXER("=======================================================");
            break;
        case AMBA_EXIF_IFD_FLASHPIX_VERSION:
            /* 0: big endian, 1: little endian */
            if (EndianType != 0U) {
                /* Swap to big endian */
                Param = AmbaExif_FileByteSwap(Param);
            }
            Pos = pCurrentMemory->Position;
            (void)AmbaExif_MemRead(pCurrentMemory, pImageInfo->PAAA.FlashpixVersion, 1, 4);
            (void)AmbaExif_MemSeek(pCurrentMemory, (INT32) Pos, MM_SEEK_SET);
            break;
        case AMBA_EXIF_IFD_COLOR_SPACE:
            pImageInfo->PAAA.ColorSpace = (UINT16)Param;
            break;
        case AMBA_EXIF_IFD_PIXEL_X_DIMENSION:
            pImageInfo->Width = Param;
            break;
        case AMBA_EXIF_IFD_PIXEL_Y_DIMENSION:
            pImageInfo->Height = Param;
            break;
        case AMBA_EXIF_IFD_EXPOSURE_INDEX:
            Pos = pCurrentMemory->Position;
            (void)AmbaExif_MemSeek(pCurrentMemory, (INT32)Param + (INT32)TiffBaseOffset, MM_SEEK_SET);
            //IF_SEEK_FAIL("AmbaExif_MemSeek Fail")
            pImageInfo->PAAA.ExposureIndexNum = MemGet32(pCurrentMemory);
            pImageInfo->PAAA.ExposureIndexDen = MemGet32(pCurrentMemory);
            (void)AmbaExif_MemSeek(pCurrentMemory, (INT32) Pos, MM_SEEK_SET);
            //IF_SEEK_FAIL("AmbaExif_MemSeek Fail")
            break;
        case AMBA_EXIF_IFD_SENSING_METHOD:
            pImageInfo->PAAA.SensingMethod = (UINT16)Param;
            break;
        case AMBA_EXIF_IFD_FILE_SOURCE:
            pImageInfo->PAAA.FileSource = (UINT16)Param;
            break;
        case AMBA_EXIF_IFD_SCENE_TYPE:
            pImageInfo->PAAA.SceneType = (UINT16)Param;
            break;
        case AMBA_EXIF_IFD_CUSTOM_RENDERED:
            pImageInfo->PAAA.CustomRendered = (UINT16)Param;
            break;
        case AMBA_EXIF_IFD_EXPOSURE_MODE:
            pImageInfo->PAAA.ExposureMode = (UINT16)Param;
            break;
        case AMBA_EXIF_IFD_WHITE_BALANCE:
            pImageInfo->PAAA.WhiteBalance = (UINT16)Param;
            break;
        case AMBA_EXIF_IFD_DIGITAL_ZOOM_RATIO:
            Pos = pCurrentMemory->Position;
            (void)AmbaExif_MemSeek(pCurrentMemory, (INT32)Param + (INT32)TiffBaseOffset, MM_SEEK_SET);
            //IF_SEEK_FAIL("AmbaExif_MemSeek Fail")
            pImageInfo->PAAA.DigitalZoomRatioNum = MemGet32(pCurrentMemory);
            pImageInfo->PAAA.DigitalZoomRatioDen = MemGet32(pCurrentMemory);
            (void)AmbaExif_MemSeek(pCurrentMemory, (INT32) Pos, MM_SEEK_SET);
            //IF_SEEK_FAIL("AmbaExif_MemSeek Fail")
            break;
        case AMBA_EXIF_IFD_FOCAL_LENGTH_IN_35MM_FILM:
            pImageInfo->PAAA.FocalLength35mmFilm = (UINT16)Param;
            break;
        case AMBA_EXIF_IFD_SCENE_CAPTURE_TYPE:
            pImageInfo->PAAA.SceneCaptureType = (UINT16)Param;
            break;
        case AMBA_EXIF_IFD_GAIN_CONTROL:
            pImageInfo->PAAA.GainControl = (UINT16)Param;
            break;
        case AMBA_EXIF_IFD_DEVICE_SETTING_DECRIPTION:
            Pos = pCurrentMemory->Position;
            pImageInfo->DeviceSetDescripSize = Count1;
            if (Count1 <= 4U) {
                pImageInfo->DeviceSetDescripOffset = Pos - 4U;
            } else {
                pImageInfo->DeviceSetDescripOffset = Param + TiffBaseOffset;
            }
            break;
        default:
            //DEBUG_EXIFDEMUXER("unsupported Tag");
            break;
        }
    }
}

static void GetGPSIfd(AMBA_EXIF_MEM *pCurrentMemory, AMBA_EXIF_IMAGE_INFO_s *pImageInfo, UINT32 IfdOffset, UINT32 TiffBaseOffset)
{
    //INT32 Rval;
    UINT32 i, Count;
    UINT32 Tag = 0U, Type = 0U, Count1 = 0U, Param = 0U, Pos;
    static UINT32 G_Satellite, G_MapDatum, G_ProcessingMethod, G_AreaInformation GNU_SECTION_NOZEROINIT;

    (void)AmbaExif_MemSeek(pCurrentMemory, (INT32) IfdOffset, MM_SEEK_SET);
    //IF_SEEK_FAIL("AmbaExif_MemSeek Fail")

    Count = MemGet16(pCurrentMemory);
    //DEBUG_EXIFDEMUXER("[%s]===== Count = %d", __func__, Count);

    for (i = 0; i < Count; i++) {
        GetIfdEntry(pCurrentMemory, &Tag, &Type, &Count1, &Param);
        switch (Tag) {
        case AMBA_EXIF_GPS_IFD_VERSION_ID:
            (void)AmbaExif_MemRead(pCurrentMemory, pImageInfo->GPSInfo.VersionID, 1, Count1);
            //DEBUG_EXIFDEMUXER("[%s] AMBA_EXIF_GPS_IFD_VersionID  GPSInfo.VersionID=%X.%X.%X.%X ", __func__, pImageInfo->GPSInfo.VersionID[0],
                              //pImageInfo->GPSInfo.VersionID[1], pImageInfo->GPSInfo.VersionID[2], pImageInfo->GPSInfo.VersionID[3]);
            break;
        case AMBA_EXIF_GPS_IFD_LATITUDE_REF:
            (void)AmbaExif_MemRead(pCurrentMemory, pImageInfo->GPSInfo.LatitudeRef, 1, Count1);
            pCurrentMemory->Position += (4U - Count1); /* Pad to 4 bytes */

            //pCurrentMemory->pBuf += (4U - Count1);
            pCurrentMemory->pBuf = AmbaExifUtil_PtrShift(pCurrentMemory->pBuf, 4 - (INT32)Count1);

            //DEBUG_EXIFDEMUXER("[%s] AMBA_EXIF_GPS_IFD_LatitudeRef  GPSInfo.LatitudeRef=%s ", __func__, pImageInfo->GPSInfo.LatitudeRef);
            break;
        case AMBA_EXIF_GPS_IFD_LATITUDE:
            Pos = pCurrentMemory->Position;
            (void)AmbaExif_MemSeek(pCurrentMemory, (INT32)Param + (INT32)TiffBaseOffset, MM_SEEK_SET);
            //IF_SEEK_FAIL("AmbaExif_MemSeek Fail")
            pImageInfo->GPSInfo.LatitudeNum[0] = MemGet32(pCurrentMemory);
            pImageInfo->GPSInfo.LatitudeDen[0] = MemGet32(pCurrentMemory);
            pImageInfo->GPSInfo.LatitudeNum[1] = MemGet32(pCurrentMemory);
            pImageInfo->GPSInfo.LatitudeDen[1] = MemGet32(pCurrentMemory);
            pImageInfo->GPSInfo.LatitudeNum[2] = MemGet32(pCurrentMemory);
            pImageInfo->GPSInfo.LatitudeDen[2] = MemGet32(pCurrentMemory);
            (void)AmbaExif_MemSeek(pCurrentMemory, (INT32) Pos, MM_SEEK_SET);
            //IF_SEEK_FAIL("AmbaExif_MemSeek Fail")
            //DEBUG_EXIFDEMUXER("[%s] AMBA_EXIF_GPS_IFD_Latitude  GPSInfo.Latitude=%d/%d %d/%d %d/%d", __func__,
                              //pImageInfo->GPSInfo.LatitudeNum[0], pImageInfo->GPSInfo.LatitudeDen[0],
                              //pImageInfo->GPSInfo.LatitudeNum[1], pImageInfo->GPSInfo.LatitudeDen[1],
                              //pImageInfo->GPSInfo.LatitudeNum[2], pImageInfo->GPSInfo.LatitudeDen[2]);
            break;
        case AMBA_EXIF_GPS_IFD_LONGITUDE_REF:
            (void)AmbaExif_MemRead(pCurrentMemory, pImageInfo->GPSInfo.LongitudeRef, 1, Count1);
            pCurrentMemory->Position += (4U - Count1); /* Pad to 4 bytes */

            //pCurrentMemory->pBuf += (4U - Count1);
            pCurrentMemory->pBuf = AmbaExifUtil_PtrShift(pCurrentMemory->pBuf, 4 - (INT32)Count1);

            //DEBUG_EXIFDEMUXER("[%s] AMBA_EXIF_GPS_IFD_LongitudeRef  GPSInfo.LongitudeRef=%s ", __func__, pImageInfo->GPSInfo.LongitudeRef);
            break;
        case AMBA_EXIF_GPS_IFD_LONGITUDE:
            Pos = pCurrentMemory->Position;
            (void)AmbaExif_MemSeek(pCurrentMemory, (INT32)Param + (INT32)TiffBaseOffset, MM_SEEK_SET);
            //IF_SEEK_FAIL("AmbaExif_MemSeek Fail")
            pImageInfo->GPSInfo.LongitudeNum[0] = MemGet32(pCurrentMemory);
            pImageInfo->GPSInfo.LongitudeDen[0] = MemGet32(pCurrentMemory);
            pImageInfo->GPSInfo.LongitudeNum[1] = MemGet32(pCurrentMemory);
            pImageInfo->GPSInfo.LongitudeDen[1] = MemGet32(pCurrentMemory);
            pImageInfo->GPSInfo.LongitudeNum[2] = MemGet32(pCurrentMemory);
            pImageInfo->GPSInfo.LongitudeDen[2] = MemGet32(pCurrentMemory);
            (void)AmbaExif_MemSeek(pCurrentMemory, (INT32) Pos, MM_SEEK_SET);
            //IF_SEEK_FAIL("AmbaExif_MemSeek Fail")
            //DEBUG_EXIFDEMUXER("[%s] AMBA_EXIF_GPS_IFD_Longitude  GPSInfo.Longitude=%d/%d %d/%d %d/%d", __func__,
                              //pImageInfo->GPSInfo.LongitudeNum[0], pImageInfo->GPSInfo.LongitudeDen[0],
                              //pImageInfo->GPSInfo.LongitudeNum[1], pImageInfo->GPSInfo.LongitudeDen[1],
                              //pImageInfo->GPSInfo.LongitudeNum[2], pImageInfo->GPSInfo.LongitudeDen[2]);
            break;
        case AMBA_EXIF_GPS_IFD_ALTITUDE_REF:
            pImageInfo->GPSInfo.AltitudeRef = (UINT8)AmbaExif_MemGetByte(pCurrentMemory);
            pCurrentMemory->Position += (4U - Count1); /* Pad to 4 bytes */

            //pCurrentMemory->pBuf += (4U - Count1);
            pCurrentMemory->pBuf = AmbaExifUtil_PtrShift(pCurrentMemory->pBuf, 4 - (INT32)Count1);

            //DEBUG_EXIFDEMUXER("[%s] AMBA_EXIF_GPS_IFD_AltitudeRef  GPSInfo.AltitudeRef=%d ", __func__, pImageInfo->GPSInfo.AltitudeRef);
            break;
        case AMBA_EXIF_GPS_IFD_ALTITUDE:
            Pos = pCurrentMemory->Position;
            (void)AmbaExif_MemSeek(pCurrentMemory, (INT32)Param + (INT32)TiffBaseOffset, MM_SEEK_SET);
            //IF_SEEK_FAIL("AmbaExif_MemSeek Fail")
            pImageInfo->GPSInfo.AltitudeNum = MemGet32(pCurrentMemory);
            pImageInfo->GPSInfo.AltitudeDen = MemGet32(pCurrentMemory);
            (void)AmbaExif_MemSeek(pCurrentMemory, (INT32) Pos, MM_SEEK_SET);
            //IF_SEEK_FAIL("AmbaExif_MemSeek Fail")
            //DEBUG_EXIFDEMUXER("[%s] AMBA_EXIF_GPS_IFD_Altitude  GPSInfo.Altitude=%d/%d ", __func__, pImageInfo->GPSInfo.AltitudeNum,
                              //pImageInfo->GPSInfo.AltitudeDen);
            break;
        case AMBA_EXIF_GPS_IFD_TIME_STAMP:
            Pos = pCurrentMemory->Position;
            (void)AmbaExif_MemSeek(pCurrentMemory, (INT32)Param + (INT32)TiffBaseOffset, MM_SEEK_SET);
            //IF_SEEK_FAIL("AmbaExif_MemSeek Fail")
            pImageInfo->GPSInfo.TimeStampNum[0] = MemGet32(pCurrentMemory);
            pImageInfo->GPSInfo.TimeStampDen[0] = MemGet32(pCurrentMemory);
            pImageInfo->GPSInfo.TimeStampNum[1] = MemGet32(pCurrentMemory);
            pImageInfo->GPSInfo.TimeStampDen[1] = MemGet32(pCurrentMemory);
            pImageInfo->GPSInfo.TimeStampNum[2] = MemGet32(pCurrentMemory);
            pImageInfo->GPSInfo.TimeStampDen[2] = MemGet32(pCurrentMemory);
            (void)AmbaExif_MemSeek(pCurrentMemory, (INT32) Pos, MM_SEEK_SET);
            //IF_SEEK_FAIL("AmbaExif_MemSeek Fail")
            //DEBUG_EXIFDEMUXER("[%s] AMBA_EXIF_GPS_IFD_TimeStamp  GPSInfo.TimeStamp=%d/%d %d/%d %d/%d", __func__,
                              //pImageInfo->GPSInfo.TimeStampNum[0], pImageInfo->GPSInfo.TimeStampDen[0],
                              //pImageInfo->GPSInfo.TimeStampNum[1], pImageInfo->GPSInfo.TimeStampDen[1],
                              //pImageInfo->GPSInfo.TimeStampNum[2], pImageInfo->GPSInfo.TimeStampDen[2]);
            break;
        case AMBA_EXIF_GPS_IFD_SATELLITES:
            Pos = pCurrentMemory->Position;
            pImageInfo->GPSInfo.SatelliteCount = Count1;
            if (Count1 <= 4U) {
                G_Satellite = Pos;
                pImageInfo->GPSInfo.pSatellite = (UINT8*)&G_Satellite;
                Pos += 4U;
            } else {
                G_Satellite = Param + TiffBaseOffset;
                pImageInfo->GPSInfo.pSatellite = (UINT8*)&G_Satellite;
            }
            (void)AmbaExif_MemSeek(pCurrentMemory, (INT32) Pos, MM_SEEK_SET);
            //IF_SEEK_FAIL("AmbaExif_MemSeek Fail")
            //DEBUG_EXIFDEMUXER("[%s] AMBA_EXIF_GPS_IFD_Satellites  *pSatellite=0x%X SatelliteCount=0x%X", __func__,
                              //*pImageInfo->GPSInfo.pSatellite, pImageInfo->GPSInfo.SatelliteCount);
            break;
        case AMBA_EXIF_GPS_IFD_STATUS:
            (void)AmbaExif_MemRead(pCurrentMemory, pImageInfo->GPSInfo.Status, 1, Count1);
            pCurrentMemory->Position += (4U - Count1); /* Pad to 4 bytes */

            //pCurrentMemory->pBuf += (4U - Count1);
            pCurrentMemory->pBuf = AmbaExifUtil_PtrShift(pCurrentMemory->pBuf, 4 - (INT32)Count1);

            //DEBUG_EXIFDEMUXER("[%s] AMBA_EXIF_GPS_IFD_Status  GPSInfo.Status=%s ", __func__, pImageInfo->GPSInfo.Status);
            break;
        case AMBA_EXIF_GPS_IFD_MEASURE_MODE:
            (void)AmbaExif_MemRead(pCurrentMemory, pImageInfo->GPSInfo.MeasureMode, 1, Count1);
            pCurrentMemory->Position += (4U - Count1); /* Pad to 4 bytes */

            //pCurrentMemory->pBuf += (4U - Count1);
            pCurrentMemory->pBuf = AmbaExifUtil_PtrShift(pCurrentMemory->pBuf, 4 - (INT32)Count1);

            //DEBUG_EXIFDEMUXER("[%s] AMBA_EXIF_GPS_IFD_MeasureMode  GPSInfo.MeasureMode=%s ", __func__, pImageInfo->GPSInfo.MeasureMode);
            break;
        case AMBA_EXIF_GPS_IFD_DOP:
            Pos = pCurrentMemory->Position;
            (void)AmbaExif_MemSeek(pCurrentMemory, (INT32)Param + (INT32)TiffBaseOffset, MM_SEEK_SET);
            //IF_SEEK_FAIL("AmbaExif_MemSeek Fail")
            pImageInfo->GPSInfo.DOPNum = MemGet32(pCurrentMemory);
            pImageInfo->GPSInfo.DOPDen = MemGet32(pCurrentMemory);
            (void)AmbaExif_MemSeek(pCurrentMemory, (INT32) Pos, MM_SEEK_SET);
            //IF_SEEK_FAIL("AmbaExif_MemSeek Fail")
            //DEBUG_EXIFDEMUXER("[%s] AMBA_EXIF_GPS_IFD_DOP  GPSInfo.DOP = %d/%d ", __func__, pImageInfo->GPSInfo.DOPNum, pImageInfo->GPSInfo.DOPDen);
            break;
        case AMBA_EXIF_GPS_IFD_SPEED_REF:
            (void)AmbaExif_MemRead(pCurrentMemory, pImageInfo->GPSInfo.SpeedRef, 1, Count1);
            pCurrentMemory->Position += (4U - Count1); /* Pad to 4 bytes */

            //pCurrentMemory->pBuf += (4U - Count1);
            pCurrentMemory->pBuf = AmbaExifUtil_PtrShift(pCurrentMemory->pBuf, 4 - (INT32)Count1);

            //DEBUG_EXIFDEMUXER("[%s] AMBA_EXIF_GPS_IFD_SpeedRef  GPSInfo.SpeedRef=%s ", __func__, pImageInfo->GPSInfo.SpeedRef);
            break;
        case AMBA_EXIF_GPS_IFD_SPEED:
            Pos = pCurrentMemory->Position;
            (void)AmbaExif_MemSeek(pCurrentMemory, (INT32)Param + (INT32)TiffBaseOffset, MM_SEEK_SET);
            //IF_SEEK_FAIL("AmbaExif_MemSeek Fail")
            pImageInfo->GPSInfo.SpeedNum = MemGet32(pCurrentMemory);
            pImageInfo->GPSInfo.SpeedDen = MemGet32(pCurrentMemory);
            (void)AmbaExif_MemSeek(pCurrentMemory, (INT32) Pos, MM_SEEK_SET);
            //IF_SEEK_FAIL("AmbaExif_MemSeek Fail")
            //DEBUG_EXIFDEMUXER("[%s] AMBA_EXIF_GPS_IFD_Speed  GPSInfo.Speed = %d/%d ", __func__, pImageInfo->GPSInfo.SpeedNum,
                              //pImageInfo->GPSInfo.SpeedDen);
            break;
        case AMBA_EXIF_GPS_IFD_TRACK_REF:
            (void)AmbaExif_MemRead(pCurrentMemory, pImageInfo->GPSInfo.TrackRef, 1, Count1);
            pCurrentMemory->Position += (4U - Count1); /* Pad to 4 bytes */

            //pCurrentMemory->pBuf += (4U - Count1);
            pCurrentMemory->pBuf = AmbaExifUtil_PtrShift(pCurrentMemory->pBuf, 4 - (INT32)Count1);

            //DEBUG_EXIFDEMUXER("[%s] AMBA_EXIF_GPS_IFD_TrackRef  GPSInfo.TrackRef=%s ", __func__, pImageInfo->GPSInfo.TrackRef);
            break;
        case AMBA_EXIF_GPS_IFD_TRACK:
            Pos = pCurrentMemory->Position;
            (void)AmbaExif_MemSeek(pCurrentMemory, (INT32)Param + (INT32)TiffBaseOffset, MM_SEEK_SET);
            //IF_SEEK_FAIL("AmbaExif_MemSeek Fail")
            pImageInfo->GPSInfo.TrackNum = MemGet32(pCurrentMemory);
            pImageInfo->GPSInfo.TrackDen = MemGet32(pCurrentMemory);
            (void)AmbaExif_MemSeek(pCurrentMemory, (INT32) Pos, MM_SEEK_SET);
            //IF_SEEK_FAIL("AmbaExif_MemSeek Fail")
            //DEBUG_EXIFDEMUXER("[%s] AMBA_EXIF_GPS_IFD_Track  GPSInfo.Track = %d/%d ", __func__, pImageInfo->GPSInfo.TrackNum,
                              //pImageInfo->GPSInfo.TrackDen);
            break;
        case AMBA_EXIF_GPS_IFD_IMG_DIR_REF:
            (void)AmbaExif_MemRead(pCurrentMemory, pImageInfo->GPSInfo.ImgDirectionRef, 1, Count1);
            pCurrentMemory->Position += (4U - Count1); /* Pad to 4 bytes */

            //pCurrentMemory->pBuf += (4U - Count1);
            pCurrentMemory->pBuf = AmbaExifUtil_PtrShift(pCurrentMemory->pBuf, 4 - (INT32)Count1);

            //DEBUG_EXIFDEMUXER("[%s] AMBA_EXIF_GPS_IFD_ImgDirectionRef  GPSInfo.ImgDirectionRef=%s ", __func__, pImageInfo->GPSInfo.ImgDirectionRef);
            break;
        case AMBA_EXIF_GPS_IFD_IMG_DIR:
            Pos = pCurrentMemory->Position;
            (void)AmbaExif_MemSeek(pCurrentMemory, (INT32)Param + (INT32)TiffBaseOffset, MM_SEEK_SET);
            //IF_SEEK_FAIL("AmbaExif_MemSeek Fail")
            pImageInfo->GPSInfo.ImgDirectionNum = MemGet32(pCurrentMemory);
            pImageInfo->GPSInfo.ImgDirectionDen = MemGet32(pCurrentMemory);
            (void)AmbaExif_MemSeek(pCurrentMemory, (INT32) Pos, MM_SEEK_SET);
            //IF_SEEK_FAIL("AmbaExif_MemSeek Fail")
            //DEBUG_EXIFDEMUXER("[%s] AMBA_EXIF_GPS_IFD_ImgDirection  GPSInfo.ImgDirection = %d/%d ", __func__,
                              //pImageInfo->GPSInfo.ImgDirectionNum, pImageInfo->GPSInfo.ImgDirectionDen);
            break;
        case AMBA_EXIF_GPS_IFD_MAP_DATUM:
            Pos = pCurrentMemory->Position;
            pImageInfo->GPSInfo.MapDatumCount = Count1;
            if (Count1 <= 4U) {
                G_MapDatum = Pos;
                pImageInfo->GPSInfo.pMapDatum = (UINT8*)&G_MapDatum;
                Pos += 4U;
            } else {
                G_MapDatum = Param + TiffBaseOffset;
                pImageInfo->GPSInfo.pMapDatum = (UINT8*)&G_MapDatum;
            }
            (void)AmbaExif_MemSeek(pCurrentMemory, (INT32) Pos, MM_SEEK_SET);
            //IF_SEEK_FAIL("AmbaExif_MemSeek Fail")
            //DEBUG_EXIFDEMUXER("[%s] AMBA_EXIF_GPS_IFD_MapDatum   *pMapDatum=0x%X MapDatumCount=0x%X", __func__,
                              //*pImageInfo->GPSInfo.pMapDatum, pImageInfo->GPSInfo.MapDatumCount);
            break;
        case AMBA_EXIF_GPS_IFD_DEST_LAT_REF:
            (void)AmbaExif_MemRead(pCurrentMemory, pImageInfo->GPSInfo.DestLatitudeRef, 1, Count1);
            pCurrentMemory->Position += (4U - Count1); /* Pad to 4 bytes */

            //pCurrentMemory->pBuf += (4U - Count1);
            pCurrentMemory->pBuf = AmbaExifUtil_PtrShift(pCurrentMemory->pBuf, 4 - (INT32)Count1);

            //DEBUG_EXIFDEMUXER("[%s] AMBA_EXIF_GPS_IFD_DestLatitudeRef  GPSInfo.DestLatitudeRef=%s ", __func__, pImageInfo->GPSInfo.DestLatitudeRef);
            break;
        case AMBA_EXIF_GPS_IFD_DEST_LAT:
            Pos = pCurrentMemory->Position;
            (void)AmbaExif_MemSeek(pCurrentMemory, (INT32)Param + (INT32)TiffBaseOffset, MM_SEEK_SET);
            //IF_SEEK_FAIL("AmbaExif_MemSeek Fail")
            pImageInfo->GPSInfo.DestLatitudeNum[0] = MemGet32(pCurrentMemory);
            pImageInfo->GPSInfo.DestLatitudeDen[0] = MemGet32(pCurrentMemory);
            pImageInfo->GPSInfo.DestLatitudeNum[1] = MemGet32(pCurrentMemory);
            pImageInfo->GPSInfo.DestLatitudeDen[1] = MemGet32(pCurrentMemory);
            pImageInfo->GPSInfo.DestLatitudeNum[2] = MemGet32(pCurrentMemory);
            pImageInfo->GPSInfo.DestLatitudeDen[2] = MemGet32(pCurrentMemory);
            (void)AmbaExif_MemSeek(pCurrentMemory, (INT32) Pos, MM_SEEK_SET);
            //IF_SEEK_FAIL("AmbaExif_MemSeek Fail")
            //DEBUG_EXIFDEMUXER("[%s] AMBA_EXIF_GPS_IFD_DestLatitude  GPSInfo.DestLatitude=%d/%d %d/%d %d/%d", __func__,
                              //pImageInfo->GPSInfo.DestLatitudeNum[0], pImageInfo->GPSInfo.DestLatitudeDen[0],
                              //pImageInfo->GPSInfo.DestLatitudeNum[1], pImageInfo->GPSInfo.DestLatitudeDen[1],
                              //pImageInfo->GPSInfo.DestLatitudeNum[2], pImageInfo->GPSInfo.DestLatitudeDen[2]);
            break;
        case AMBA_EXIF_GPS_IFD_DEST_LONG_REF:
            (void)AmbaExif_MemRead(pCurrentMemory, pImageInfo->GPSInfo.DestLongitudeRef, 1, Count1);
            pCurrentMemory->Position += (4U - Count1); /* Pad to 4 bytes */

            //pCurrentMemory->pBuf += (4U - Count1);
            pCurrentMemory->pBuf = AmbaExifUtil_PtrShift(pCurrentMemory->pBuf, 4 - (INT32)Count1);

            //DEBUG_EXIFDEMUXER("[%s] AMBA_EXIF_GPS_IFD_DestLongitudeRef  GPSInfo.DestLongitudeRef=%s ", __func__, pImageInfo->GPSInfo.DestLongitudeRef);
            break;
        case AMBA_EXIF_GPS_IFD_DEST_LONG:
            Pos = pCurrentMemory->Position;
            (void)AmbaExif_MemSeek(pCurrentMemory, (INT32)Param + (INT32)TiffBaseOffset, MM_SEEK_SET);
            //IF_SEEK_FAIL("AmbaExif_MemSeek Fail")
            pImageInfo->GPSInfo.DestLongitudeNum[0] = MemGet32(pCurrentMemory);
            pImageInfo->GPSInfo.DestLongitudeDen[0] = MemGet32(pCurrentMemory);
            pImageInfo->GPSInfo.DestLongitudeNum[1] = MemGet32(pCurrentMemory);
            pImageInfo->GPSInfo.DestLongitudeDen[1] = MemGet32(pCurrentMemory);
            pImageInfo->GPSInfo.DestLongitudeNum[2] = MemGet32(pCurrentMemory);
            pImageInfo->GPSInfo.DestLongitudeDen[2] = MemGet32(pCurrentMemory);
            (void)AmbaExif_MemSeek(pCurrentMemory, (INT32) Pos, MM_SEEK_SET);
            //IF_SEEK_FAIL("AmbaExif_MemSeek Fail")
            //DEBUG_EXIFDEMUXER("[%s] AMBA_EXIF_GPS_IFD_DestLongitude  GPSInfo.DestLongitude=%d/%d %d/%d %d/%d", __func__,
                              //pImageInfo->GPSInfo.DestLongitudeNum[0], pImageInfo->GPSInfo.DestLongitudeDen[0],
                              //pImageInfo->GPSInfo.DestLongitudeNum[1], pImageInfo->GPSInfo.DestLongitudeDen[1],
                              //pImageInfo->GPSInfo.DestLongitudeNum[2], pImageInfo->GPSInfo.DestLongitudeDen[2]);
            break;
        case AMBA_EXIF_GPS_IFD_DEST_BEARING_REF:
            (void)AmbaExif_MemRead(pCurrentMemory, pImageInfo->GPSInfo.DestBearingRef, 1, Count1);
            pCurrentMemory->Position += (4U - Count1); /* Pad to 4 bytes */

            //pCurrentMemory->pBuf += (4U - Count1);
            pCurrentMemory->pBuf = AmbaExifUtil_PtrShift(pCurrentMemory->pBuf, 4 - (INT32)Count1);

            //DEBUG_EXIFDEMUXER("[%s] AMBA_EXIF_GPS_IFD_DestBearingRef  GPSInfo.DestBearingRef=%s ", __func__, pImageInfo->GPSInfo.DestBearingRef);
            break;
        case AMBA_EXIF_GPS_IFD_DEST_BEARING:
            Pos = pCurrentMemory->Position;
            (void)AmbaExif_MemSeek(pCurrentMemory, (INT32)Param + (INT32)TiffBaseOffset, MM_SEEK_SET);
            //IF_SEEK_FAIL("AmbaExif_MemSeek Fail")
            pImageInfo->GPSInfo.DestBearingNum = MemGet32(pCurrentMemory);
            pImageInfo->GPSInfo.DestBearingDen = MemGet32(pCurrentMemory);
            (void)AmbaExif_MemSeek(pCurrentMemory, (INT32) Pos, MM_SEEK_SET);
            //IF_SEEK_FAIL("AmbaExif_MemSeek Fail")
            //DEBUG_EXIFDEMUXER("[%s] AMBA_EXIF_GPS_IFD_DestBearing  GPSInfo.DestBearing = %d/%d ", __func__,
                              //pImageInfo->GPSInfo.DestBearingNum, pImageInfo->GPSInfo.DestBearingDen);
            break;
        case AMBA_EXIF_GPS_IFD_DEST_DIST_REF:
            (void)AmbaExif_MemRead(pCurrentMemory, pImageInfo->GPSInfo.DestDistanceRef, 1, Count1);
            pCurrentMemory->Position += (4U - Count1); /* Pad to 4 bytes */

            //pCurrentMemory->pBuf += (4U - Count1);
            pCurrentMemory->pBuf = AmbaExifUtil_PtrShift(pCurrentMemory->pBuf, 4 - (INT32)Count1);

            //DEBUG_EXIFDEMUXER("[%s] AMBA_EXIF_GPS_IFD_DestDistanceRef  GPSInfo.DestDistanceRef=%s ", __func__, pImageInfo->GPSInfo.DestDistanceRef);
            break;
        case AMBA_EXIF_GPS_IFD_DEST_DIST:
            Pos = pCurrentMemory->Position;
            (void)AmbaExif_MemSeek(pCurrentMemory, (INT32)Param + (INT32)TiffBaseOffset, MM_SEEK_SET);
            //IF_SEEK_FAIL("AmbaExif_MemSeek Fail")
            pImageInfo->GPSInfo.DestDistanceNum = MemGet32(pCurrentMemory);
            pImageInfo->GPSInfo.DestDistanceDen = MemGet32(pCurrentMemory);
            (void)AmbaExif_MemSeek(pCurrentMemory, (INT32) Pos, MM_SEEK_SET);
            //IF_SEEK_FAIL("AmbaExif_MemSeek Fail")
            //DEBUG_EXIFDEMUXER("[%s] AMBA_EXIF_GPS_IFD_DestDistance  GPSInfo.DestDistance = %d/%d ", __func__,
                              //pImageInfo->GPSInfo.DestDistanceNum, pImageInfo->GPSInfo.DestDistanceDen);
            break;
        case AMBA_EXIF_GPS_IFD_PROCESSING_METHOD:
            Pos = pCurrentMemory->Position;
            pImageInfo->GPSInfo.ProcessingMethodCount = Count1;
            if (Count1 <= 4U) {
                G_ProcessingMethod = Pos - 4U;
                pImageInfo->GPSInfo.pProcessingMethod = (UINT8*)&G_ProcessingMethod;
            } else {
                G_ProcessingMethod = Param + TiffBaseOffset;
                pImageInfo->GPSInfo.pProcessingMethod = (UINT8*)&G_ProcessingMethod;
            }
            (void)AmbaExif_MemSeek(pCurrentMemory, (INT32) Pos, MM_SEEK_SET);
            //IF_SEEK_FAIL("AmbaExif_MemSeek Fail")
            //DEBUG_EXIFDEMUXER("[%s] AMBA_EXIF_GPS_IFD_ProcessingMethod   *pProcessingMethod=0x%X MapDatumCount=0x%X", __func__,
                              //*pImageInfo->GPSInfo.pProcessingMethod, pImageInfo->GPSInfo.ProcessingMethodCount);
            break;
        case AMBA_EXIF_GPS_IFD_AREA_INFORMATION:
            Pos = pCurrentMemory->Position;
            pImageInfo->GPSInfo.AreaInformationCount = Count1;
            if (Count1 <= 4U) {
                G_AreaInformation = Pos - 4U;
                pImageInfo->GPSInfo.pAreaInformation = (UINT8*)&G_AreaInformation;
            } else {
                G_AreaInformation = Param + TiffBaseOffset;
                pImageInfo->GPSInfo.pAreaInformation = (UINT8*)&G_AreaInformation;
            }
            (void)AmbaExif_MemSeek(pCurrentMemory, (INT32) Pos, MM_SEEK_SET);
            //IF_SEEK_FAIL("AmbaExif_MemSeek Fail")
            //DEBUG_EXIFDEMUXER("[%s] AMBA_EXIF_GPS_IFD_AreaInformation   *pAreaInformation=0x%X MapDatumCount=0x%X", __func__,
                              //*pImageInfo->GPSInfo.pAreaInformation, pImageInfo->GPSInfo.AreaInformationCount);
            break;
        case AMBA_EXIF_GPS_IFD_DATE_STAMP:
            Pos = pCurrentMemory->Position;
            (void)AmbaExif_MemSeek(pCurrentMemory, (INT32)Param + (INT32)TiffBaseOffset, MM_SEEK_SET);
            //IF_SEEK_FAIL("AmbaExif_MemSeek Fail")
            (void)AmbaExif_MemRead(pCurrentMemory, pImageInfo->GPSInfo.DateStamp, 1, Count1);
            (void)AmbaExif_MemSeek(pCurrentMemory, (INT32) Pos, MM_SEEK_SET);
            //IF_SEEK_FAIL("AmbaExif_MemSeek Fail")
            //DEBUG_EXIFDEMUXER("[%s] AMBA_EXIF_GPS_IFD_DateStamp  GPSInfo.DateStamp = %s ", __func__, pImageInfo->GPSInfo.DateStamp);
            break;
        case AMBA_EXIF_GPS_IFD_DIFFERENTIAL:
            pImageInfo->GPSInfo.Differential = (UINT16) (Param & 0xffffU);
            break;
        case AMBA_EXIF_GPS_IFD_H_POSITIONING_ERROR:
            Pos = pCurrentMemory->Position;
            (void)AmbaExif_MemSeek(pCurrentMemory, (INT32)Param + (INT32)TiffBaseOffset, MM_SEEK_SET);
            //IF_SEEK_FAIL("AmbaExif_MemSeek Fail")
            pImageInfo->GPSInfo.HPositioningErrorNum = MemGet32(pCurrentMemory);
            pImageInfo->GPSInfo.HPositioningErrorDen = MemGet32(pCurrentMemory);
            (void)AmbaExif_MemSeek(pCurrentMemory, (INT32) Pos, MM_SEEK_SET);
            //IF_SEEK_FAIL("AmbaExif_MemSeek Fail")
            //DEBUG_EXIFDEMUXER("[%s] AMBA_EXIF_GPS_IFD_HPositioningError  GPSInfo.HPositioningError = %d/%d ", __func__,
                              //pImageInfo->GPSInfo.HPositioningErrorNum, pImageInfo->GPSInfo.HPositioningErrorDen);
            break;
        default:
            //DEBUG_EXIFDEMUXER("unsupported Tag");
            break;
        }
    }
}

static void FastGetIfd0(AMBA_EXIF_MEM *pCurrentMemory, AMBA_EXIF_IMAGE_INFO_s *pImageInfo, UINT32 *pExifOffset)
{
    UINT32 i, Count;
    UINT32 Tag = 0U, Type = 0U, Count1 = 0U, Param = 0U;

    Count = MemGet16(pCurrentMemory);

    for (i = 0; i < Count; i++) {
        GetIfdEntry(pCurrentMemory, &Tag, &Type, &Count1, &Param);
        switch (Tag) {
        case AMBA_EXIF_TIFF_ORIENTATION:
            pImageInfo->Orientation = (UINT8)Param;
            break;
        case AMBA_EXIF_TIFF_EXIF_IFD_POINTER:
            *pExifOffset = Param;
            //DEBUG_EXIFDEMUXER("[%s] TIFF_ExifIFDPointer  Param=%d", __func__, Param);
            break;
        default:
            //DEBUG_EXIFDEMUXER_ERR("Non-parsed Tag %d Type %d Count1 %d Param %d", Tag, Type, Count1, Param);
            break;
        }
    }
}

static void FastGetIfd1(AMBA_EXIF_MEM *pCurrentMemory, AMBA_EXIF_IMAGE_INFO_s *pImageInfo, UINT32 Offset, UINT32 TiffBaseOffset)
{
    UINT32 i, Count;
    UINT32 Tag;

    if (OK != AmbaExif_MemSeek(pCurrentMemory, (INT32) Offset, MM_SEEK_SET)) {
        EXIFDLOG_NG(__func__, "Line %d", __LINE__, 0U);
    }
    Count = MemGet16(pCurrentMemory);

    for (i = 0; i < Count; i++) {
        Tag = MemGet16(pCurrentMemory);
        if (OK != AmbaExif_MemSeek(pCurrentMemory, 6, MM_SEEK_CUR)) {
            EXIFDLOG_NG(__func__, "Line %d", __LINE__, 0U);
        }
        switch (Tag) {
        case AMBA_EXIF_TIFF_JPG_INTRCHG_FMT:
            pImageInfo->ThumbPos = MemGet32(pCurrentMemory) + TiffBaseOffset + 2U;
            break;
        case AMBA_EXIF_TIFF_JPG_INTRCHG_FMT_LENGTH:
            pImageInfo->ThumbSize = MemGet32(pCurrentMemory);
            break;
        default:
            if (OK != AmbaExif_MemSeek(pCurrentMemory, 4, MM_SEEK_CUR)) {
                EXIFDLOG_NG(__func__, "Line %d", __LINE__, 0U);
            }
            break;
        }
    }
}

static UINT32 CheckDHT(AMBA_EXIF_MEM *pCurrentMemory, UINT8 Type, UINT16 Length)
{
    UINT32 i, CheckLoop = 1U;
    UINT8 DhtType = 0U, Temp = 0U;
    UINT32 Rval = 0;
    UINT16 Length2 = Length;
    const UINT8 *pDHTStd = NULL;

    static const UINT8 DHTStd1[29] = {
        0x00, 0x00, 0x01, 0x05, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B};

    static const UINT8 DHTStd2[179] = {
        0x10, 0x00, 0x02, 0x01, 0x03, 0x03, 0x02, 0x04, 0x03, 0x05, 0x05, 0x04, 0x04, 0x00, 0x00, 0x01,
        0x7D, 0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12, 0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61,
        0x07, 0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xA1, 0x08, 0x23, 0x42, 0xB1, 0xC1, 0x15, 0x52, 0xD1,
        0xF0, 0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0A, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x25, 0x26, 0x27,
        0x28, 0x29, 0x2A, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
        0x49, 0x4A, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
        0x69, 0x6A, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88,
        0x89, 0x8A, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6,
        0xA7, 0xA8, 0xA9, 0xAA, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xC2, 0xC3, 0xC4,
        0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xE1,
        0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7,
        0xF8, 0xF9, 0xFA};

    static const UINT8 DHTStd3[29] = {
        0x01, 0x00, 0x03, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B};

    static const UINT8 DHTStd4[179] = {
        0x11, 0x00, 0x02, 0x01, 0x02, 0x04, 0x04, 0x03, 0x04, 0x07, 0x05, 0x04, 0x04, 0x00, 0x01, 0x02,
        0x77, 0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21, 0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61,
        0x71, 0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91, 0xA1, 0xB1, 0xC1, 0x09, 0x23, 0x33, 0x52,
        0xF0, 0x15, 0x62, 0x72, 0xD1, 0x0A, 0x16, 0x24, 0x34, 0xE1, 0x25, 0xF1, 0x17, 0x18, 0x19, 0x1A,
        0x26, 0x27, 0x28, 0x29, 0x2A, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x43, 0x44, 0x45, 0x46, 0x47,
        0x48, 0x49, 0x4A, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x63, 0x64, 0x65, 0x66, 0x67,
        0x68, 0x69, 0x6A, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x82, 0x83, 0x84, 0x85, 0x86,
        0x87, 0x88, 0x89, 0x8A, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0xA2, 0xA3, 0xA4,
        0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xC2,
        0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9,
        0xDA, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7,
        0xF8, 0xF9, 0xFA};

    static const UINT8 DHTSize[4] = {29, 179, 29, 179};

    AmbaMisra_TouchUnused(&Rval);

    if (Length != 0x1a2U) {          /* DHTSize[0] + DHTSize[1] + DHTSize[2] + DHTSize[3] + 2 */
        switch (Type) {
        case 0x01:
            if (Length != 0x1FU) {     /* DHTSize[2] + 2 */
                Rval = NG;
            }
            pDHTStd = DHTStd3;
            break;
        case 0x10:
            if (Length != 0xB5U) {     /* DHTSize[1] + 2 */
                Rval = NG;
            }
            pDHTStd = DHTStd2;
            break;
        case 0x11:
            if (Length != 0xB5U) {    /* DHTSize[3] + 2 */
                Rval = NG;
            }
            pDHTStd = DHTStd4;
            break;
        default:
            if (Length != 0x1FU) {   /* DHTSize[0] + 2 */
                Rval = NG;
            }
            pDHTStd = DHTStd1;
            break;
        }
    } else {
        CheckLoop = 4;
        DhtType = Type;
    }

    if ((CheckLoop != 1U) && (Rval == OK)) {
        for (; CheckLoop > 0U; CheckLoop--) {
            switch (DhtType) {
            case 0x00:
                pDHTStd = DHTStd1;
                Length2 = DHTSize[0];
                break;
            case 0x01:
                pDHTStd = DHTStd3;
                Length2 = DHTSize[2];
                break;
            case 0x10:
                pDHTStd = DHTStd2;
                Length2 = DHTSize[1];
                break;
            case 0x11:
                pDHTStd = DHTStd4;
                Length2 = DHTSize[3];
                break;
            default:
                //DEBUG_EXIFDEMUXER_ERR("ExifParse: DHT Error!!!!!!!!");
                break;
            }

            for (i = 1; i < (UINT32)Length2; i++) {
                Temp = (UINT8)AmbaExif_MemGetByte(pCurrentMemory);
                if ((pDHTStd != NULL) && (Temp != pDHTStd[i])) {
                    //DEBUG_EXIFDEMUXER_ERR("Temp=%X", Temp);
                    Rval = NG;
                }
            }
            if ((CheckLoop != 1U) && (Rval == OK)) {
                DhtType = (UINT8)AmbaExif_MemGetByte(pCurrentMemory);
                //DEBUG_EXIFDEMUXER("ExifParse: [Done] DhtType = %d Temp=%X", DhtType, Temp);
            }
        }
    } else {
        for (i = 1; i < ((UINT32)Length - 2U); i++) {
            Temp = (UINT8)AmbaExif_MemGetByte(pCurrentMemory);
            if ((pDHTStd != NULL) && (Temp != pDHTStd[i])) {
                Rval = NG;
                break;
            }
        }
    }

    if (Rval == NG) {
        //DEBUG_EXIFDEMUXER_ERR("ExifParse: !DHT found@ 0x%x %x %x", pCurrentMemory->Position, Type, Length2);
    }

    return Rval;
}

static INT32 CheckJPEG(AMBA_EXIF_MEM *pCurrentMemory, AMBA_EXIF_IMAGE_INFO_s *pImageInfo, UINT32 Length)
{
    UINT32 CurrentFilePointer;
    /* record current file pointer, at the end of this function it will be used to reset the file pointer */
    /* record whether representative maker segment exists or not; if all of them exist, we say there is a JPEG image in this appX segment */
    UINT8 Soi = 0; /* D8 */
    UINT8 Dqt = 0; /* DB */
    UINT8 Dht = 0; /* C4 */
    UINT8 Sof = 0; /* C0 */
    UINT8 Sos = 0; /* DA */
    UINT8 Eoi = 0; /* D9 */
    UINT32 CurrentByte = 0; /* content of current byte */
    UINT32 ReadIn = 0;      /* bytes having been read in; should not exceed size */
    UINT32 SoiOffset = 0;   /* Soi's (Start of image) offset */
    INT32 Rval;

    AmbaMisra_TouchUnused(&Eoi);

    //DEBUG_EXIFDEMUXER("***CheckJPEG() Start***");

    CurrentFilePointer = pCurrentMemory->Position;
    //DEBUG_EXIFDEMUXER("    At Beginning file Pos is 0x%x", CurrentFilePointer);
    //DEBUG_EXIFDEMUXER("    length  = 0x%x", Length);

    while (ReadIn < Length) {
        CurrentByte = AmbaExif_MemGetByte(pCurrentMemory);
        ReadIn++;
        if (CurrentByte != 0xFFU) {
            continue;
        }
        CurrentByte = AmbaExif_MemGetByte(pCurrentMemory);
        ReadIn++;
        switch (CurrentByte) {
        case 0xD8:
            Soi++;
            SoiOffset = (ReadIn - 2U) + 4U;
            //DEBUG_EXIFDEMUXER("    SOI at %llu", pCurrentMemory->Position - 2);
            break; /* 2:0xffd8; 4:"ftyp" */
        case 0xDB:
            Dqt++;
            break;
        case 0xC4:
            Dht++;
            break;
        case 0xC0:
            Sof++;
            break;
        case 0xDA:
            Sos++;
            break;
        case 0xD9:
            Eoi++;
            break;
        default :
            //DEBUG_EXIFDEMUXER("unknown CurrentByte");
            break;
        }
        if (((((Soi != 0U) && (Dqt != 0U)) && (Dht != 0U)) && (Sof != 0U)) && (Sos != 0U)) {
            break;
        }
    }

    if (OK != AmbaExif_MemSeek(pCurrentMemory, (INT32) (CurrentFilePointer), MM_SEEK_SET)) {
        EXIFDLOG_NG(__func__, "Line %d", __LINE__, 0U);
    }
    pImageInfo->ScreenSoiOffset = SoiOffset;
    //CurrentFilePointer = AmbaExif_FileTell(pCurrentFile);
    //DEBUG_EXIFDEMUXER("    At End file Pos is 0x%x", CurrentFilePointer);
    //DEBUG_EXIFDEMUXER("    SOI  %u", Soi);
    //DEBUG_EXIFDEMUXER("    DQT  %u", Dqt);
    //DEBUG_EXIFDEMUXER("    DHT  %u", Dht);
    //DEBUG_EXIFDEMUXER("    SOF %u", Sof);
    //DEBUG_EXIFDEMUXER("    EOI  %u", Eoi);

    //DEBUG_EXIFDEMUXER("***CheckJPEG() End***");

    if (((((Soi != 0U) && (Dqt != 0U)) && (Dht != 0U)) && (Sof != 0U)) && (Sos != 0U)) {
        Rval = (INT32)SoiOffset;
    } else if (Soi > 1U) {
        //DEBUG_EXIFDEMUXER_ERR("There are more than one SOI!");
        Rval = -0x5a5a;
    } else {
        Rval = -1;
    }

    return Rval;
}

static void GetIfdEntry(AMBA_EXIF_MEM *pCurrentMemory, UINT32 *pTag, UINT32 *pType, UINT32 *pCount, UINT32 *pParam)
{
    UINT32 val;
    *pTag = MemGet16(pCurrentMemory);
    *pType = MemGet16(pCurrentMemory);
    *pCount = MemGet32(pCurrentMemory);

    switch (*pType) {
    case (UINT32)TYPE_BYTE:
    case (UINT32)TYPE_ASCII:
        if (*pCount > 4U) {
            *pParam = MemGet32(pCurrentMemory); /* pParam is offset to Value */
        }
        break;
    case (UINT32)TYPE_UNDEFINE:
        if (*pCount > 4U) {
            *pParam = MemGet32(pCurrentMemory); /* pParam is offset to Value */
        } else {
            val = AmbaExif_MemGetByte(pCurrentMemory);
            val = (val << 8U) + AmbaExif_MemGetByte(pCurrentMemory);
            val = (val << 8U) + AmbaExif_MemGetByte(pCurrentMemory);
            *pParam = (val << 8U) + AmbaExif_MemGetByte(pCurrentMemory);
        }
        break;
    case (UINT32)TYPE_SHORT:
        val = MemGet16(pCurrentMemory);
        if (*pCount == 1U) {
            *pParam = val;
            (void)MemGet16(pCurrentMemory);
        } else {
            *pParam = (val << 16) + MemGet16(pCurrentMemory);
        }
        break;
    default:
        *pParam = MemGet32(pCurrentMemory); /* pParam is offset to Value */
        break;
    }
}

static UINT8 GetHeaderEndianType(AMBA_EXIF_MEM *pCurrentMemory)
{
    if (AmbaExif_MemGetBe16(pCurrentMemory) == (UINT32)AMBA_EXIF_BIG_ENDIAN_MM) {
        MemGet16 = AmbaExif_MemGetBe16;
        MemGet32 = AmbaExif_MemGetBe32;
        /* 0: big endian, 1: little endian */
        EndianType = 0;
    } else {
        MemGet16 = AmbaExif_MemGetLe16;
        MemGet32 = AmbaExif_MemGetLe32;
        EndianType = 1;
    }

    if (MemGet16(pCurrentMemory) != 0x2AU) {
        EndianType = 2;
    }

    return EndianType;
}
