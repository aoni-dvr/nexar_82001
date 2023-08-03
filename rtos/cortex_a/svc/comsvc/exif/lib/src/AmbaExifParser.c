/**
*  @file AmbaExifParser.c
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
*  @details EXIF Parser
*
*/
#include "AmbaFS.h"
#include "AmbaWrap.h"

#include "AmbaFrwk.h"
#include "AmbaExifFile.h"
#include "AmbaExifMem.h"
#include "AmbaExif.h"

#include "AmbaMisraFix.h"
#include "AmbaWrap.h"
#include "AmbaUtility.h"
//#include "SvcWrap.h"

#define EXIFPLOG_OK       AmbaFrwk_LogOK
#define EXIFPLOG_NG       AmbaFrwk_LogNG
//#define EXIFPLOG_DBG      AmbaFrwk_LogDBG

static UINT8 BufferPool[MB] GNU_SECTION_NOZEROINIT;

static void Svc_PrintNg(const char *pFormat, UINT32 Line, UINT32 RVAL)
{
    if(RVAL != OK) {
        AmbaFrwk_LogNG(pFormat, "Line %d RVAL %d", Line, RVAL);
    }
}

/**
 * EXIF parser API for still capture
 * @param [in]  *pCurrentFile pointer to file system
 * @param [out] *pImageInfo pointer to EXIF sturcture
 * @param [out] *pFileStatus pointer to file info
 * @return ErrorCode
 */
UINT32 AmbaExifFullParse(AMBA_FS_FILE *pCurrentFile, AMBA_EXIF_IMAGE_INFO_s *pImageInfo, const AMBA_FS_FILE_INFO_s *pFileStatus)
{
    UINT32 Rval = OK;
    UINT32 OffsetOfApp1 = 0, OffsetOfApp2 = 0, TempOffset = 0, APP1ExifHeader, AppXTag;
    UINT32 ExifLength;
    AMBA_EXIF_MEM PF;
    AMBA_EXIF_MEM *pCurrentMemory = &PF;
    static UINT32   SizeOfDSDS = 600;    /* DQT + SOF + DHT + SOS size = 579 */

    /* Get JPEG file size*/
    pImageInfo->FileSize = pFileStatus->Size;
    //DEBUG_EXIFPARSER("pImageInfo->FileSize:%lld    pFileStatus->Size:%lld", pImageInfo->FileSize, pFileStatus->Size);

    /* TYPE of the second picture(screennail, ...) */
    //pImageInfo->SecondPicType;
    pImageInfo->AppMarkerNum = 0;
    //pImageInfo->AppMarkerInfo[0].APPMarker = 0xE1;
    /* initial value = 0xFFFFFFFF. it is impossible to get such value under 32-bits architecture
       so could be used to distingusish we have such app or not. */
    //pImageInfo->AppMarkerInfo[0].Offset = 0xFFFFFFFF;
    pImageInfo->MPOScreennailAPPXStart = 0xFFFFFFFFU;

    /* SOI */
    (void)AmbaFS_FileSeek(pCurrentFile, (INT64)0, AMBA_FS_SEEK_START);
    if (AmbaExif_FileGetBe16(pCurrentFile) != (UINT32)AMBA_EXIF_SEGMENT_SOI) {
        //DEBUG_EXIFPARSER_ERR("Cannot found SOI , CurrentFile position = %d", AmbaExif_FileTell(pCurrentFile));
        EXIFPLOG_NG(__func__, "Line %d", __LINE__, 0U);
        Rval = NG;
    }

    while ((pImageInfo->FullviewPos == 0U) && (Rval == OK)) {
        AppXTag = AmbaExif_FileGetBe16(pCurrentFile);

        /* Get and check length of EXIF APP */
        ExifLength = AmbaExif_FileGetBe16(pCurrentFile);
        if ((((UINT64)ExifLength) >= pImageInfo->FileSize) || (ExifLength == 0U)) {
            //DEBUG_EXIFPARSER_ERR("ExifParse: %s AppXTag=0x%x ExifLength:%d > fsize:%d or == 0", pImageInfo->Name, AppXTag, ExifLength, pImageInfo->FileSize);
            #if 0
            SVC_WRAP_PRINT "ExifParse: %s AppXTag=0x%x ExifLength:%d > fsize:%d or == 0"
            SVC_PRN_ARG_S __func__
            SVC_PRN_ARG_PROC EXIFPLOG_OK
            SVC_PRN_ARG_STR  pImageInfo->Name
            SVC_PRN_ARG_UINT32 AppXTag
            SVC_PRN_ARG_UINT32 ExifLength
            SVC_PRN_ARG_UINT32 pImageInfo->FileSize
            SVC_PRN_ARG_E
            #endif

            Rval = NG;
            break;
        }

        switch (AppXTag) {
        case AMBA_EXIF_SEGMENT_APP1:
            /* APP1, Exif attribute information */
            if (OffsetOfApp1 == 0U) {
                OffsetOfApp1 = (UINT32)AmbaExif_FileTell(pCurrentFile);
                //DEBUG_EXIFPARSER("[%s] ===  OffsetOfApp1=%d   ===", __func__, OffsetOfApp1);

                /* read APP1 to memory pool start */
                Rval = AmbaFS_FileSeek(pCurrentFile, (INT64)0, AMBA_FS_SEEK_START);
                Svc_PrintNg(__func__, __LINE__, Rval);
                if (AmbaExif_FileRead(BufferPool, 1, ExifLength + SizeOfDSDS, pCurrentFile) != (ExifLength + SizeOfDSDS)) {
                    Rval = NG;
                    EXIFPLOG_NG(__func__, "Line %d", __LINE__, 0U);
                    //DEBUG_EXIFPARSER_ERR("[%s]#%d AmbaExif_FileRead fail", __func__, __LINE__);
                    break;
                }
                AmbaExif_MemInit(pCurrentMemory, BufferPool, ExifLength + SizeOfDSDS);
                Rval = AmbaExif_MemSeek(pCurrentMemory, (INT32) OffsetOfApp1, MM_SEEK_SET);
                Svc_PrintNg(__func__, __LINE__, Rval);
                /* read APP1 to memory pool end */

                /* Parse APP1 */
                APP1ExifHeader = AmbaExif_MemGetBe32(pCurrentMemory);

                if (APP1ExifHeader == (UINT32)AMBA_EXIF_TIFF_HEADER) {
                    /* keyword, "Exif" */
                    //DEBUG_EXIFPARSER("ExifParse: Expecting EXIF! %s", APP1ExifHeader);
                    Rval = AmbaExifDemuxer_ParseAPP1TIFF(pCurrentMemory, pImageInfo, pFileStatus);
                    if (Rval != OK) {
                        EXIFPLOG_NG(__func__, "Line %d", __LINE__, 0U);
                        //DEBUG_EXIFPARSER_ERR("AmbaExifDemuxer_ParseAPP1TIFF fail");
                        break;
                    }
                } else {
                    if ((APP1ExifHeader == (UINT32)AMBA_EXIF_XMP_HEADER)) {
                        /* keyword, "http:ns.adobe.com/xap/1.0/" */
                        //DEBUG_EXIFPARSER("ExifParse: Expecting EXIF! %s", APP1ExifHeader);
                        Rval = AmbaExifDemuxer_ParseAPP1XMP(pCurrentMemory, pImageInfo);
                        if (Rval != OK) {
                            EXIFPLOG_NG(__func__, "Line %d", __LINE__, 0U);
                            //DEBUG_EXIFPARSER_ERR("AmbaExifDemuxer_ParseAPP1XMP fail");
                            break;
                        }
                    }
                }

                /* reset memory pool */
                if (AmbaWrap_memset(BufferPool, 0, sizeof(BufferPool)) != OK) {
                    EXIFPLOG_NG(__func__, "Line %d", __LINE__, 0U);
                }

                /* Record App Marker info */
                if (pImageInfo->AppMarkerNum < (UINT8)AMBA_EXIF_MAX_APP_MARKER_NUMBER) {
                    pImageInfo->AppMarkerInfo[pImageInfo->AppMarkerNum].APPMarker = (UINT8)(AppXTag & 0xFFU);
                    pImageInfo->AppMarkerInfo[pImageInfo->AppMarkerNum].Offset = (INT64)OffsetOfApp1 - 4; /* include header */
                    pImageInfo->AppMarkerInfo[pImageInfo->AppMarkerNum].Size = ExifLength + 2U;     /* include header */
                    pImageInfo->AppMarkerNum++;
                }

                Rval = AmbaFS_FileSeek(pCurrentFile, ((INT64)OffsetOfApp1 + (INT64)ExifLength - 2), AMBA_FS_SEEK_START);
                //IF_ERROR_BREAK("[%s]#%d AmbaFS_fseek fail")
            } else {
                /* already has APP1, skip to next */
                Rval = AmbaFS_FileSeek(pCurrentFile, ((INT64)ExifLength - 2), AMBA_FS_SEEK_CUR);
                //IF_ERROR_BREAK("[%s]#%d AmbaFS_FileSeek fail")
            }
            break;
        case AMBA_EXIF_SEGMENT_APP2:
            /* APP2, Screennail */
            OffsetOfApp2 = (UINT32)AmbaExif_FileTell(pCurrentFile);

            /* read APP2 to memory pool start */
            if (AmbaExif_FileRead(BufferPool, 1, ExifLength + SizeOfDSDS, pCurrentFile) != (ExifLength + SizeOfDSDS)) {
                Rval = NG;
                EXIFPLOG_NG(__func__, "Line %d", __LINE__, 0U);
                //DEBUG_EXIFPARSER_ERR("[%s]#%d AmbaExif_FileRead fail", __func__, __LINE__);
                break;
            }
            AmbaExif_MemInit(pCurrentMemory, BufferPool, ExifLength + SizeOfDSDS);
            Rval = AmbaExif_MemSeek(pCurrentMemory, 0, MM_SEEK_SET);
            Svc_PrintNg(__func__, __LINE__, Rval);
            /* read APP2 to memory pool end */

            /* Parse APP2 */
            Rval = AmbaExifDemuxer_ParseAPP2(pCurrentMemory, pImageInfo, (INT64)OffsetOfApp2, ExifLength);
            if (Rval != OK) {
                EXIFPLOG_NG(__func__, "Line %d", __LINE__, 0U);
                //DEBUG_EXIFPARSER_ERR("parse app2 fail");
                break;
            }

            /* reset memory pool */
            if (AmbaWrap_memset(BufferPool, 0, sizeof(BufferPool)) != OK) {
                EXIFPLOG_NG(__func__, "Line %d", __LINE__, 0U);
            }

            /* Record App Marker info */
            if (pImageInfo->AppMarkerNum < (UINT8)AMBA_EXIF_MAX_APP_MARKER_NUMBER) {
                pImageInfo->AppMarkerInfo[pImageInfo->AppMarkerNum].APPMarker = (UINT8)(AppXTag & 0xFFU);
                pImageInfo->AppMarkerInfo[pImageInfo->AppMarkerNum].Offset = (INT64)OffsetOfApp2 - 4; /* include header */
                pImageInfo->AppMarkerInfo[pImageInfo->AppMarkerNum].Size = ExifLength;         /* include header */
                pImageInfo->AppMarkerNum++;
            }

            Rval =  AmbaFS_FileSeek(pCurrentFile, ((INT64)OffsetOfApp2 + (INT64)ExifLength - 2), AMBA_FS_SEEK_START);
            //IF_ERROR_BREAK("[%s]#%d AmbaFS_FileSeek(OffsetOfApp2 + ExifLength - 2)... fail")
            break;
        default:
            //DEBUG_EXIFPARSER("[%s] AppX (except APP1 and APP2)", __func__);
            if ((AppXTag & 0xFFF0U) == 0xFFE0U) {
                /* AppX (except APP1 and APP2) */
                if (pImageInfo->AppMarkerInfo[pImageInfo->AppMarkerNum].APPMarker == (UINT8)(AppXTag & 0xFFU)) {
                    if (pImageInfo->AppMarkerInfo[pImageInfo->AppMarkerNum].Offset == (INT64)0xFFFFFFFFU) {
                        /* re-parsing label */
                        pImageInfo->AppMarkerInfo[pImageInfo->AppMarkerNum].APPMarker = (UINT8)(AppXTag & 0xFFU);
                        /* pay attention to 4!! it means marker + size */
                        pImageInfo->AppMarkerInfo[pImageInfo->AppMarkerNum].Offset = (INT64)AmbaExif_FileTell(pCurrentFile) - 4;
                        pImageInfo->AppMarkerNum++;
                    }
                }
                Rval = AmbaFS_FileSeek(pCurrentFile, ((INT64)ExifLength - 2), AMBA_FS_SEEK_CUR);
                //IF_ERROR_BREAK("[%s]#%d AmbaFS_FileSeek fail")
            } else {
                if (pImageInfo->FullviewPos != 0U) {
                    Rval = AmbaFS_FileSeek(pCurrentFile, ((INT64)ExifLength - 2), AMBA_FS_SEEK_CUR);
                    Svc_PrintNg(__func__, __LINE__, Rval);
                } else {
                    /* If not found Fullview yet */
                    /* Check fullview */
                    TempOffset = (UINT32)AmbaExif_FileTell(pCurrentFile) - 4U; /* DQT + DQT_len */
                    //DEBUG_EXIFPARSER("[%s] CASE DEFAULT   TempOffset=%d" , __func__, TempOffset);

                    /* read fullview info to memory pool start */
                    Rval = AmbaFS_FileSeek(pCurrentFile, (INT64)TempOffset, AMBA_FS_SEEK_START);
                    Svc_PrintNg(__func__, __LINE__, Rval);
                    if (AmbaExif_FileRead(BufferPool, 1, SizeOfDSDS, pCurrentFile) != SizeOfDSDS) {
                        Rval = NG;
                        //DEBUG_EXIFPARSER_ERR("[%s]#%d AmbaExif_FileRead fail", __func__, __LINE__);
                        break;
                    }
                    AmbaExif_MemInit(pCurrentMemory, BufferPool, SizeOfDSDS);
                    Rval = AmbaExif_MemSeek(pCurrentMemory, 0, MM_SEEK_SET);
                    Svc_PrintNg(__func__, __LINE__, Rval);
                    /* read fullview info to memory pool end */

                    /* Parse FullView DQT SOF DHT ...... */
                    Rval = AmbaExifDemuxer_CheckTag(pCurrentMemory, pImageInfo, (UINT8)AMBA_EXIF_FULLVIEW, 0U);

                    if (Rval == OK) {
                        pImageInfo->Fullview++;
                        pImageInfo->FullviewPos = TempOffset;
                        if (pImageInfo->SecondPicType == (UINT8)SECPIC_TYPE_MPF) {
                            if (pImageInfo->FullviewSize != 0U) {
                                pImageInfo->FullviewSize = (pImageInfo->FullviewSize - TempOffset) + 2U; /* SOI */
                            }
                        } else {
                            pImageInfo->FullviewSize = ((UINT32)pImageInfo->FileSize - TempOffset) + 2U;         /* SOI */
                        }
                    } else {
                        //DEBUG_EXIFPARSER_ERR("ExifParse: Full Unsupported %d", Rval);
                        break;
                    }

                    /* reset memory pool */
                    if (AmbaWrap_memset(BufferPool, 0, sizeof(BufferPool)) != OK) {
                        EXIFPLOG_NG(__func__, "Line %d", __LINE__, 0U);
                    }
                }
            }
            break;
        }

        if (Rval == OK) {
            /* leave while loop */
            if (AmbaFS_FileEof(pCurrentFile) != OK) {
                EXIFPLOG_NG(__func__, "Line %d", __LINE__, 0U);
                Rval = NG;
            }
        }
    }

    if (Rval == OK) {
        /* if the second pic(Screen nail) is at tail (MPF) */
        if (pImageInfo->SecondPicType == (UINT8)SECPIC_TYPE_MPF) {
            if ((pImageInfo->ScreenSize != 0U) && (pImageInfo->ScreenPos != 0U)) {
                //DEBUG_EXIFPARSER("[%s] pImageInfo->ScreenSize=%d  pImageInfo->ScreenPos=%d", __func__, pImageInfo->ScreenSize, pImageInfo->ScreenPos);

                /* Read screen nail info to memory pool start */
                Rval = AmbaFS_FileSeek(pCurrentFile, (INT64)(pImageInfo->ScreenPos), AMBA_FS_SEEK_START);
                if (Rval == OK) {
                    if (AmbaExif_FileRead(BufferPool, 1, pImageInfo->ScreenSize, pCurrentFile) != pImageInfo->ScreenSize) {
                        Rval = NG;
                        EXIFPLOG_NG(__func__, "Line %d", __LINE__, 0U);
                        //DEBUG_EXIFPARSER_ERR("[%s]#%d AmbaExif_FileRead fail", __func__, __LINE__);
                    }
                }
                if (Rval == OK) {
                    AmbaExif_MemInit(pCurrentMemory, BufferPool, pImageInfo->ScreenSize);
                    Rval = AmbaExif_MemSeek(pCurrentMemory, 0, MM_SEEK_SET);
                }
                /* Read screen nail info to memory pool end */
                if (Rval == OK) {
                    /* Parse screen nail */
                    Rval = AmbaExifDemuxer_ParseMPFScrn(pCurrentMemory, pImageInfo, 0);
                }
                if (Rval == OK) {
                    /* reset memory pool */
                    if (AmbaWrap_memset(BufferPool, 0, sizeof(BufferPool)) != OK) {
                        EXIFPLOG_NG(__func__, "Line %d", __LINE__, 0U);
                    }
                }
            }
            if (Rval == OK) {
                if (pImageInfo->FullviewSize == 0U) {
                    if (pImageInfo->ScreenPos != 0U) {
                        pImageInfo->FullviewSize = (pImageInfo->ScreenPos - TempOffset) + 2U; /* SOI */
                    } else {
                        pImageInfo->FullviewSize = ((UINT32)pImageInfo->FileSize - TempOffset) + 2U;   /* SOI */
                    }
                }

                for (ExifLength = 1; ExifLength < ((UINT32)AMBA_EXIF_MPO_MAX_NUM + 1U); ExifLength++) {
                    if ((pImageInfo->MPO[(ExifLength - 1U)].FullviewPos != 0U) &&
                        (pImageInfo->MPO[(ExifLength - 1U)].FullviewSize != 0U)) {
                        Rval = AmbaExifDemuxer_ParseMPFFull(pCurrentMemory, pImageInfo, (UINT8)ExifLength);
                        //IF_ERROR_BREAK("[%s]#%d AmbaExifDemuxer_ParseMPFFull fail")
                    }
                    if (Rval == OK) {
                        if ((pImageInfo->MPO[(ExifLength - 1U)].ScreenPos != 0U) && (pImageInfo->MPO[(ExifLength - 1U)].ScreenSize != 0U)) {
                            Rval = AmbaExifDemuxer_ParseMPFScrn(pCurrentMemory, pImageInfo, (UINT8)ExifLength);
                            //IF_ERROR_BREAK("[%s]#%d AmbaExifDemuxer_ParseMPFScrn fail")
                        }
                    }
                }
            }
        }

        if (Rval == OK) {

        //    if (pImageInfo->SecondPicType == SECPIC_TYPE && pImageInfo->Screennail) {
                /* allocate memory pool for Screennail */
        //        AmbaFS_fseek(pCurrentFile, pImageInfo->ScreenPos - 2, AMBA_FS_SEEK_START);
        //        Rval = AmbaFS_fread(BufferPool, 1, SizeOfDSDS, pCurrentFile);
        //        IF_ERROR_GOTO_DONE("[%s]#%d AmbaFS_Read fail")
        //        AmbaExif_MemInit(pCurrentMemory, BufferPool, SizeOfDSDS);
        //
        //
        //        AmbaExif_MemSeek(pCurrentMemory, (INT32)0, MM_SEEK_SET);
                /*allocate memory pool for Screennail  end */
        //
                /* Parse Screennail DQT SOF DHT ...... */
        //        Rval = AmbaExifDemuxer_CheckTag(pCurrentMemory, pImageInfo, AMBA_EXIF_SCREENNAIL, 0);
        //        if (Rval < 0) {
        //            DEBUG_EXIFPARSER("[%s] pImageInfo->ScreenPos = %d",__func__, pImageInfo->ScreenPos);
        //            pImageInfo->ScreenPos = 0;
        //            pImageInfo->Screennail = 0;
        //        } else {
        //            pImageInfo->ScreenSize = pImageInfo->FileSize - pImageInfo->ScreenPos;
        //        }
        //
                /* reset memory pool for Screennail */
        //        (void)AmbaWrap_memset(&BufferPool, 0, sizeof(BufferPool));
        //
        //
        //    }

            /* use fullview info when thumbnail isn't exist */
            if (pImageInfo->ThumbPos == 0U) {
                pImageInfo->ThumbWidth = pImageInfo->Width;
                pImageInfo->ThumbHeight = pImageInfo->Height;
                pImageInfo->ThumbSize = pImageInfo->FullviewSize;
                pImageInfo->ThumbPos = pImageInfo->FullviewPos;
                pImageInfo->ThumbDHTStat = pImageInfo->DHTStat;
                if (OK != AmbaWrap_memcpy(&(pImageInfo->JPEGThumbInfo), &(pImageInfo->JPEGInfo), sizeof(AMBA_EXIF_JPEG_INFO_s))) {
                    EXIFPLOG_NG(__func__, "Line %d", __LINE__, 0U);
                }
                if (OK != AmbaWrap_memcpy(&(pImageInfo->JPEGThumbHeaderInfo), &(pImageInfo->JPEGHeaderInfo), sizeof(AMBA_EXIF_JPEG_HEADER_INFO_s))) {
                    EXIFPLOG_NG(__func__, "Line %d", __LINE__, 0U);
                }
            }

            //DEBUG_EXIFPARSER("[%s]#%d pImageInfo->ScreenSize=%d   pImageInfo->ScreenPos=%d", __func__, __LINE__, pImageInfo->ScreenSize, pImageInfo->ScreenPos);
            if ((pImageInfo->ScreenPos == 0U) && (pImageInfo->ScreenSize == 0U)) {
                pImageInfo->ScreenWidth = pImageInfo->Width;
                pImageInfo->ScreenHeight = pImageInfo->Height;
                pImageInfo->ScreenSize = pImageInfo->FullviewSize;
                pImageInfo->ScreenPos = pImageInfo->FullviewPos;
                pImageInfo->ScreenDHTStat = pImageInfo->DHTStat;
                if (OK != AmbaWrap_memcpy(&(pImageInfo->JPEGScreenInfo), &(pImageInfo->JPEGInfo), sizeof(AMBA_EXIF_JPEG_INFO_s))) {
                    EXIFPLOG_NG(__func__, "Line %d", __LINE__, 0U);

                }
                if (OK != AmbaWrap_memcpy(&(pImageInfo->JPEGScreenHeaderInfo), &(pImageInfo->JPEGHeaderInfo), sizeof(AMBA_EXIF_JPEG_HEADER_INFO_s))) {
                    EXIFPLOG_NG(__func__, "Line %d", __LINE__, 0U);
                }
                //DEBUG_EXIFPARSER("[%s]#%d pImageInfo->JPEGScreenHeaderInfo.DHT.MarkerNumber=%d", __func__, __LINE__,
                                 //pImageInfo->JPEGScreenHeaderInfo.DHT.MarkerNumber);
            }

            pImageInfo->FullviewOriSize = (UINT32)pImageInfo->FullviewSize;
            pImageInfo->ThumbOriSize = pImageInfo->ThumbSize;
            pImageInfo->ScreenOriSize = pImageInfo->ScreenSize;

            for (ExifLength = 1U; ExifLength < ((UINT32)AMBA_EXIF_MPO_MAX_NUM + 1U); ExifLength++) {
                pImageInfo->MPO[(ExifLength - 1U)].FullviewOriSize = pImageInfo->MPO[(ExifLength - 1U)].FullviewSize;
                pImageInfo->MPO[(ExifLength - 1U)].ThumbOriSize = pImageInfo->MPO[(ExifLength - 1U)].ThumbSize;
                pImageInfo->MPO[(ExifLength - 1U)].ScreenOriSize = pImageInfo->MPO[(ExifLength - 1U)].ScreenSize;
            }

            pImageInfo->JPEGInfo.LengthOfFile = (UINT32)pImageInfo->FullviewSize;
            pImageInfo->JPEGThumbInfo.LengthOfFile = pImageInfo->ThumbSize;
            pImageInfo->JPEGScreenInfo.LengthOfFile = pImageInfo->ScreenSize;
        }
    }
    return Rval;
}

/**
 * fast EXIF parser API for still capture
 * @param [in]  *pCurrentFile pointer to file system
 * @param [out] *pThumbInfo pointer to thumbnail info
 * @return ErrorCode
 */
UINT32 AmbaExifFastParse(AMBA_FS_FILE *pCurrentFile, AMBA_EXIF_THUMB_INFO_s *pThumbInfo)
{
    UINT32 Rval;
    UINT32 Size;
    UINT32 ExifLength, TagA1, TagA2, Length;
    AMBA_EXIF_MEM PF;
    AMBA_EXIF_MEM* pCurrentMemory = &PF;
    AMBA_EXIF_IMAGE_INFO_s ImageInfo;

    if (AmbaWrap_memset(&ImageInfo, 0, sizeof(ImageInfo)) != OK) {
        EXIFPLOG_NG(__func__, "Line %d", __LINE__, 0U);
    }

    Rval = AmbaFS_FileSeek(pCurrentFile, (INT64)4, AMBA_FS_SEEK_START);
    if (Rval == OK) {
        /* Length of EXIF APP */
        ExifLength = AmbaExif_FileGetBe16(pCurrentFile);
        /* read APP1 info to memory pool start */
        Rval = AmbaFS_FileSeek(pCurrentFile, (INT64)0, AMBA_FS_SEEK_START);
        if (Rval == OK) {
            Size = AmbaExif_FileRead(BufferPool, 1, ExifLength, pCurrentFile);
            if (Size != ExifLength) {
                Rval = NG;
            }
        }
        if (Rval == OK) {
            AmbaExif_MemInit(pCurrentMemory, BufferPool, ExifLength);
            Rval = AmbaExif_MemSeek(pCurrentMemory, 12, MM_SEEK_SET);
        }
    }
    /* read APP1 info to memory pool end */

    /* Parse APP1 Start */
    if (Rval == OK) {
        Rval = AmbaExifDemuxer_FastParseAPP1(pCurrentMemory, &ImageInfo);
    }

    if (Rval == OK) {
        Rval = AmbaExif_MemSeek(pCurrentMemory, (INT32) ImageInfo.ThumbPos, MM_SEEK_SET);
    }

    if (Rval == OK) {
        while ((AmbaExif_MemEof(pCurrentMemory) == OK) && (ImageInfo.ThumbHeight == 0U)) {
            TagA1 = AmbaExif_MemGetBe16(pCurrentMemory);
            if ((TagA1 & 0xFF00U) != 0xFF00U) {
                continue;
            }
            TagA2 = TagA1 & 0x00FFU;
            //DEBUG_EXIFPARSER("TagA2 = FF%X", TagA2);
            switch (TagA2) {
            case 0xC0:
                /* 0xFFC0: SOF0 */
                /* Lf: Frame header length */
                Length = AmbaExif_MemGetBe16(pCurrentMemory) - 2U;
                (void)AmbaExif_MemGetByte(pCurrentMemory);
                ImageInfo.ThumbHeight = AmbaExif_MemGetBe16(pCurrentMemory);
                ImageInfo.ThumbWidth = AmbaExif_MemGetBe16(pCurrentMemory);
                ImageInfo.Thumbnail = 1;

                //DEBUG_EXIFPARSER("Length=%d  pImageInfo->ThumbHeight=0x%X   pImageInfo->ThumbWidth=0x%X", Length,
                        //pImageInfo->ThumbHeight, pImageInfo->ThumbWidth);
                (void)AmbaExif_MemGetByte(pCurrentMemory);
                (void)AmbaExif_MemGetByte(pCurrentMemory);
                ImageInfo.YUVType = AmbaExif_MemGetByte(pCurrentMemory);
                Rval = AmbaExif_MemSeek(pCurrentMemory,  (((INT32)Length - 8) - 7), MM_SEEK_CUR);
                break;
            default:
                /* get next header */
                Length = AmbaExif_MemGetBe16(pCurrentMemory);
                Rval = AmbaExif_MemSeek(pCurrentMemory, ((INT32)Length - 2), MM_SEEK_CUR);
                break;
            }
        }
    }

    pThumbInfo->ThumbSize   = ImageInfo.ThumbSize;
    pThumbInfo->ThumbPos    = ImageInfo.ThumbPos;
    pThumbInfo->ThumbHeight = ImageInfo.ThumbHeight;
    pThumbInfo->ThumbWidth  = ImageInfo.ThumbWidth;
    pThumbInfo->Thumbnail   = ImageInfo.Thumbnail;
    pThumbInfo->YUVType     = ImageInfo.YUVType;

    #if 1
    EXIFPLOG_OK(__func__, "||==||==== ThumbWidth = %d", pThumbInfo->ThumbWidth, 0U);
    EXIFPLOG_OK(__func__, "||==||==== ThumbHeight = %d", pThumbInfo->ThumbHeight, 0U);
    EXIFPLOG_OK(__func__, "||==||==== JPEGInterchangeFormat(ThumbPos) = %d", pThumbInfo->ThumbPos, 0U);
    EXIFPLOG_OK(__func__, "||==||==== JPEGInterchangeFormatLength(ThumbSize) = %d", pThumbInfo->ThumbSize, 0U);
    #endif

    return Rval;
}

#if 1//Mic__ TBD
void AmbaExifParseDebugPrint(const AMBA_EXIF_IMAGE_INFO_s *pImageInfo)
{

    #if 0
    (void)AmbaPrint("[%s] start!", __func__);

    AmbaPrintColor(GREEN, "||== APP %X   Offset(%d,0x%X) Size(%d,0x%X)", pImageInfo->AppMarkerInfo[0].APPMarker, pImageInfo->AppMarkerInfo[0].Offset, pImageInfo->AppMarkerInfo[0].Offset, pImageInfo->AppMarkerInfo[0].Size, pImageInfo->AppMarkerInfo[0].Size);
    AmbaPrintColor(GREEN, "||== TIFF Header");
    AmbaPrintColor(GREEN, "||==||= 0th IFD");
    (void)AmbaPrint("||==||== Make:%s\tOffset(%d,0x%X) Size(%d,0x%X)", pImageInfo->Make, pImageInfo->MakeOffset, pImageInfo->MakeOffset, pImageInfo->MakeSize, pImageInfo->MakeSize);
    (void)AmbaPrint("||==||== Model:%s\tOffset(%d,0x%X) Size(%d,0x%X)", pImageInfo->Model, pImageInfo->ModelOffset, pImageInfo->ModelOffset, pImageInfo->ModelSize, pImageInfo->ModelSize);
    (void)AmbaPrint("||==||== Orientation:%d", pImageInfo->Orientation);
    (void)AmbaPrint("||==||== Software:%s\tOffset(%d,0x%X) Size(%d,0x%X)", pImageInfo->Software, pImageInfo->SoftwareOffset, pImageInfo->SoftwareOffset, pImageInfo->SoftwareSize, pImageInfo->SoftwareSize);
    (void)AmbaPrint("||==||== Date:%s", pImageInfo->Date);
    AmbaPrintColor(GREEN, "||======== Exif IFD");
    (void)AmbaPrint("||=====||= ExposureTime = %d / %d", pImageInfo->PAAA.ExposureTimeNum, pImageInfo->PAAA.ExposureTimeDen);
    (void)AmbaPrint("||=====||= FNumber = %d / %d", pImageInfo->PAAA.FNumberNum, pImageInfo->PAAA.FNumberDen);
    (void)AmbaPrint("||=====||= ExposureProgram = %d", pImageInfo->PAAA.ExposureProgram);
    (void)AmbaPrint("||=====||= PhotoGraphicSentivity = %d", pImageInfo->PAAA.PhotoGraphicSentivity);
    (void)AmbaPrint("||=====||= DateTimeOriginal = %s", pImageInfo->PAAA.DateTimeOriginal);
    (void)AmbaPrint("||=====||= DateTimeDigitized = %s", pImageInfo->PAAA.DateTimeDigitized);
    (void)AmbaPrint("||=====||= ShutterSpeed = %d / %d", pImageInfo->PAAA.ShutterSpeedNum, pImageInfo->PAAA.ShutterSpeedDen);
    (void)AmbaPrint("||=====||= ApertureValue = %d / %d", pImageInfo->PAAA.ApertureValueNum, pImageInfo->PAAA.ApertureValueDen);
    (void)AmbaPrint("||=====||= ExposureBiasValue = %d / %d", pImageInfo->PAAA.ExposureBiasValueNum, pImageInfo->PAAA.ExposureBiasValueDen);
    (void)AmbaPrint("||=====||= MaxApertureValue = %d / %d", pImageInfo->PAAA.MaxApertureValueNum, pImageInfo->PAAA.MaxApertureValueDen);
    (void)AmbaPrint("||=====||= SubjectDistanceRange = %d", pImageInfo->PAAA.SubjectDistanceRange);
    (void)AmbaPrint("||=====||= SubjectDistance = %d / %d", pImageInfo->PAAA.SubjectDistanceNum, pImageInfo->PAAA.SubjectDistanceDen);
    (void)AmbaPrint("||=====||= MeteringMode = %d", pImageInfo->PAAA.MeteringMode);
    (void)AmbaPrint("||=====||= LightSource = %d", pImageInfo->PAAA.LightSource);
    (void)AmbaPrint("||=====||= Flash = %d", pImageInfo->PAAA.Flash);
    (void)AmbaPrint("||=====||= FocalLength = %d / %d", pImageInfo->PAAA.FocalLengthNum, pImageInfo->PAAA.FocalLengthDen);
    (void)AmbaPrint("||=====||= MakerNote\tOffset(%d,0x%X) Size(%d,0x%X)", pImageInfo->MakernoteOffset, pImageInfo->MakernoteOffset, pImageInfo->MakernoteSize, pImageInfo->MakernoteSize);
    (void)AmbaPrint("||=====||= FlashpixVersion = %s", pImageInfo->PAAA.FlashpixVersion);
    (void)AmbaPrint("||=====||= ColorSpace = %d", pImageInfo->PAAA.ColorSpace);
    (void)AmbaPrint("||=====||= ExposureIndex = %d / %d", pImageInfo->PAAA.ExposureIndexNum, pImageInfo->PAAA.ExposureIndexDen);
    (void)AmbaPrint("||=====||= SensingMethod = %d", pImageInfo->PAAA.SensingMethod);
    (void)AmbaPrint("||=====||= FileSource = %d", pImageInfo->PAAA.FileSource);
    (void)AmbaPrint("||=====||= SceneType = %d", pImageInfo->PAAA.SceneType);
    (void)AmbaPrint("||=====||= CustomRendered = %d", pImageInfo->PAAA.CustomRendered);
    (void)AmbaPrint("||=====||= ExposureMode = %d", pImageInfo->PAAA.ExposureMode);
    (void)AmbaPrint("||=====||= WhiteBalance = %d", pImageInfo->PAAA.WhiteBalance);
    (void)AmbaPrint("||=====||= DigitalZoomRatio = %d / %d", pImageInfo->PAAA.DigitalZoomRatioNum, pImageInfo->PAAA.DigitalZoomRatioDen);
    (void)AmbaPrint("||=====||= FocalLength35mmFilm = %d", pImageInfo->PAAA.FocalLength35mmFilm);
    (void)AmbaPrint("||=====||= SceneCaptureType = %d", pImageInfo->PAAA.SceneCaptureType);
    (void)AmbaPrint("||=====||= GainControl = %d", pImageInfo->PAAA.GainControl);
    (void)AmbaPrint("||=====||= Contrast = %d", pImageInfo->PAAA.Contrast);
    (void)AmbaPrint("||=====||= Saturation = %d", pImageInfo->PAAA.Saturation);
    (void)AmbaPrint("||=====||= Sharpness = %d", pImageInfo->PAAA.Sharpness);

    if (pImageInfo->WithGPSIfd != 0U) {
        AmbaPrintColor(GREEN, "||======== GPS info IFD");
        (void)AmbaPrint("||=====||= VersionID = %X.%X.%X.%X ", pImageInfo->GPSInfo.VersionID[0], pImageInfo->GPSInfo.VersionID[1], pImageInfo->GPSInfo.VersionID[2], pImageInfo->GPSInfo.VersionID[3]);
        (void)AmbaPrint("||=====||= LatitudeRef = %s", pImageInfo->GPSInfo.LatitudeRef);
        (void)AmbaPrint("||=====||= Latitude = %d/%d %d/%d %d/%d",
                  pImageInfo->GPSInfo.LatitudeNum[0], pImageInfo->GPSInfo.LatitudeDen[0],
                  pImageInfo->GPSInfo.LatitudeNum[1], pImageInfo->GPSInfo.LatitudeDen[1],
                  pImageInfo->GPSInfo.LatitudeNum[2], pImageInfo->GPSInfo.LatitudeDen[2]);
        (void)AmbaPrint("||=====||= LongitudeRef = %s", pImageInfo->GPSInfo.LongitudeRef);
        (void)AmbaPrint("||=====||= Longitude = %d/%d %d/%d %d/%d",
                  pImageInfo->GPSInfo.LongitudeNum[0], pImageInfo->GPSInfo.LongitudeDen[0],
                  pImageInfo->GPSInfo.LongitudeNum[1], pImageInfo->GPSInfo.LongitudeDen[1],
                  pImageInfo->GPSInfo.LongitudeNum[2], pImageInfo->GPSInfo.LongitudeDen[2]);
        (void)AmbaPrint("||=====||= AltitudeRef = %d", pImageInfo->GPSInfo.AltitudeRef);
        (void)AmbaPrint("||=====||= Altitude = %d/%d ", pImageInfo->GPSInfo.AltitudeNum, pImageInfo->GPSInfo.AltitudeDen);
        (void)AmbaPrint("||=====||= TimeStamp = %d/%d %d/%d %d/%d",
                  pImageInfo->GPSInfo.TimeStampNum[0], pImageInfo->GPSInfo.TimeStampDen[0],
                  pImageInfo->GPSInfo.TimeStampNum[1], pImageInfo->GPSInfo.TimeStampDen[1],
                  pImageInfo->GPSInfo.TimeStampNum[2], pImageInfo->GPSInfo.TimeStampDen[2]);
        (void)AmbaPrint("||=====||= SatelliteOffse t= 0x%X , SatelliteCount = 0x%X",  pImageInfo->GPSInfo.SatelliteOffset, pImageInfo->GPSInfo.SatelliteCount);
        (void)AmbaPrint("||=====||= Status = %s", pImageInfo->GPSInfo.Status);
        (void)AmbaPrint("||=====||= MeasureMode = %s", pImageInfo->GPSInfo.MeasureMode);
        (void)AmbaPrint("||=====||= DOP = %d/%d ", pImageInfo->GPSInfo.DOPNum, pImageInfo->GPSInfo.DOPDen);
        (void)AmbaPrint("||=====||= SpeedRef = %s", pImageInfo->GPSInfo.SpeedRef);
        (void)AmbaPrint("||=====||= Speed = %d/%d ", pImageInfo->GPSInfo.SpeedNum, pImageInfo->GPSInfo.SpeedDen);
        (void)AmbaPrint("||=====||= TrackRef = %s", pImageInfo->GPSInfo.TrackRef);
        (void)AmbaPrint("||=====||= Track = %d/%d ", pImageInfo->GPSInfo.TrackNum, pImageInfo->GPSInfo.TrackDen);
        (void)AmbaPrint("||=====||= ImgDirectionRef = %s", pImageInfo->GPSInfo.ImgDirectionRef);
        (void)AmbaPrint("||=====||= ImgDirection = %d/%d ", pImageInfo->GPSInfo.ImgDirectionNum, pImageInfo->GPSInfo.ImgDirectionDen);
        (void)AmbaPrint("||=====||= MapDatumOffset = 0x%X , MapDatumCount = 0x%X", pImageInfo->GPSInfo.MapDatumOffset, pImageInfo->GPSInfo.MapDatumCount);
        (void)AmbaPrint("||=====||= DestLatitudeRef = %s", pImageInfo->GPSInfo.DestLatitudeRef);
        (void)AmbaPrint("||=====||= DestLatitude = %d/%d %d/%d %d/%d",
                  pImageInfo->GPSInfo.DestLatitudeNum[0], pImageInfo->GPSInfo.DestLatitudeDen[0],
                  pImageInfo->GPSInfo.DestLatitudeNum[1], pImageInfo->GPSInfo.DestLatitudeDen[1],
                  pImageInfo->GPSInfo.DestLatitudeNum[2], pImageInfo->GPSInfo.DestLatitudeDen[2]);
        (void)AmbaPrint("||=====||= DestLongitudeRef = %s", pImageInfo->GPSInfo.DestLongitudeRef);
        (void)AmbaPrint("||=====||= DestLongitude = %d/%d %d/%d %d/%d",
                  pImageInfo->GPSInfo.DestLongitudeNum[0], pImageInfo->GPSInfo.DestLongitudeDen[0],
                  pImageInfo->GPSInfo.DestLongitudeNum[1], pImageInfo->GPSInfo.DestLongitudeDen[1],
                  pImageInfo->GPSInfo.DestLongitudeNum[2], pImageInfo->GPSInfo.DestLongitudeDen[2]);
        (void)AmbaPrint("||=====||= DestBearingRef = %s", pImageInfo->GPSInfo.DestBearingRef);
        (void)AmbaPrint("||=====||= DestBearing = %d/%d ", pImageInfo->GPSInfo.DestBearingNum, pImageInfo->GPSInfo.DestBearingDen);
        (void)AmbaPrint("||=====||= DestDistanceRef = %s", pImageInfo->GPSInfo.DestDistanceRef);
        (void)AmbaPrint("||=====||= DestDistance = %d/%d ", pImageInfo->GPSInfo.DestDistanceNum, pImageInfo->GPSInfo.DestDistanceDen);
        (void)AmbaPrint("||=====||= ProcessingMethodOffset = 0x%X , MapDatumCount = 0x%X", pImageInfo->GPSInfo.ProcessingMethodOffset, pImageInfo->GPSInfo.ProcessingMethodCount);
        (void)AmbaPrint("||=====||= AreaInformationOffset = 0x%X , MapDatumCount = 0x%X", pImageInfo->GPSInfo.AreaInformationOffset, pImageInfo->GPSInfo.AreaInformationCount);
        (void)AmbaPrint("||=====||= DateStamp = %s", pImageInfo->GPSInfo.DateStamp);
        (void)AmbaPrint("||=====||= Differential = %d", pImageInfo->GPSInfo.Differential);
        (void)AmbaPrint("||=====||= HPositioningError = %d/%d ", pImageInfo->GPSInfo.HPositioningErrorNum, pImageInfo->GPSInfo.HPositioningErrorDen);
    }
    #endif

    EXIFPLOG_OK(__func__, "||==||= 1st IFD", 0U, 0U);
    EXIFPLOG_OK(__func__, "||==||==== ThumbWidth = %d", pImageInfo->ThumbWidth, 0U);
    EXIFPLOG_OK(__func__, "||==||==== ThumbHeight = %d", pImageInfo->ThumbHeight, 0U);
    EXIFPLOG_OK(__func__, "||==||==== JPEGInterchangeFormat(ThumbPos) = %d", pImageInfo->ThumbPos, 0U);
    EXIFPLOG_OK(__func__, "||==||==== JPEGInterchangeFormatLength(ThumbSize) = %d", pImageInfo->ThumbSize, 0U);
    EXIFPLOG_OK(__func__, "||==||==== ThumbOriSize = %d", pImageInfo->ThumbOriSize, 0U);
    EXIFPLOG_OK(__func__, "||==||==== JPEGThumbInfo.XImage = %d", pImageInfo->JPEGThumbInfo.XImage, 0U);
    EXIFPLOG_OK(__func__, "||==||==== JPEGThumbInfo.YImage = %d", pImageInfo->JPEGThumbInfo.YImage, 0U);
    EXIFPLOG_OK(__func__, "||==||==== JPEGThumbInfo.LengthOfFile = %d", pImageInfo->JPEGThumbInfo.LengthOfFile, 0U);

    EXIFPLOG_OK(__func__, "||== FullView info", 0U, 0U);
    EXIFPLOG_OK(__func__, "||== FullviewPos = %d", pImageInfo->FullviewPos, 0U);
    EXIFPLOG_OK(__func__, "||== FullviewSize = %d", pImageInfo->FullviewSize, 0U);
    EXIFPLOG_OK(__func__, "||== FullviewOriSize = %d", pImageInfo->FullviewOriSize, 0U);
    EXIFPLOG_OK(__func__, "||== Width = %d", pImageInfo->Width, 0U);
    EXIFPLOG_OK(__func__, "||== Height = %d", pImageInfo->Height, 0U);
    EXIFPLOG_OK(__func__, "||== JPEGInfo.XImage = %d", pImageInfo->JPEGInfo.XImage, 0U);
    EXIFPLOG_OK(__func__, "||== JPEGInfo.YImage = %d", pImageInfo->JPEGInfo.YImage, 0U);
    EXIFPLOG_OK(__func__, "||== JPEGInfo.LengthOfFile = %d", pImageInfo->JPEGInfo.LengthOfFile, 0U);

    EXIFPLOG_OK(__func__, "||== Screennail info", 0U, 0U);
    EXIFPLOG_OK(__func__, "||== ScreenWidth = %d", pImageInfo->ScreenWidth, 0U);
    EXIFPLOG_OK(__func__, "||== ScreenHeight = %d", pImageInfo->ScreenHeight, 0U);
    EXIFPLOG_OK(__func__, "||== ScreenPos = %d", pImageInfo->ScreenPos, 0U);
    EXIFPLOG_OK(__func__, "||== ScreenSoiOffset = %d", pImageInfo->ScreenSoiOffset, 0U);
    EXIFPLOG_OK(__func__, "||== ScreenSize = %d", pImageInfo->ScreenSize, 0U);
    EXIFPLOG_OK(__func__, "||== ScreenOriSize = %d", pImageInfo->ScreenOriSize, 0U);
    EXIFPLOG_OK(__func__, "||== JPEGScreenInfo.XImage = %d", pImageInfo->JPEGScreenInfo.XImage, 0U);
    EXIFPLOG_OK(__func__, "||== JPEGScreenInfo.YImage = %d", pImageInfo->JPEGScreenInfo.YImage, 0U);
    EXIFPLOG_OK(__func__, "||== JPEGScreenInfo.LengthOfFile = %d", pImageInfo->JPEGScreenInfo.LengthOfFile, 0U);
    EXIFPLOG_OK(__func__, "==================================================================", 0U, 0U);

    EXIFPLOG_OK(__func__, "||== Additional info", 0U, 0U);

    #if 0
    SVC_WRAP_PRINT "||== Name = %s"
    SVC_PRN_ARG_S __func__
    SVC_PRN_ARG_PROC EXIFPLOG_OK
    SVC_PRN_ARG_STR  pImageInfo->Name
    SVC_PRN_ARG_E
    #endif

    EXIFPLOG_OK(__func__, "||== Fullview exist= %d", pImageInfo->Fullview, 0U);
    EXIFPLOG_OK(__func__, "||== Thumbnail exist= %d", pImageInfo->Thumbnail, 0U);
    EXIFPLOG_OK(__func__, "||== Screennail exist= %d", pImageInfo->Screennail, 0U);
    EXIFPLOG_OK(__func__, "||==== AppMarkerNum = %d", pImageInfo->AppMarkerNum, 0U);

    #if 0
    for (UINT8 i = 0; ((i < pImageInfo->AppMarkerNum) && (i < (UINT8)AMBA_EXIF_MAX_APP_MARKER_NUMBER)); i++) {
        (void)AmbaPrint("||==== AppMarkerInfo[%d].APPMarker = FF%X", i, pImageInfo->AppMarkerInfo[i].APPMarker);
        (void)AmbaPrint("||==== AppMarkerInfo[%d].offset = %X", i, pImageInfo->AppMarkerInfo[i].Offset);
        (void)AmbaPrint("||==== AppMarkerInfo[%d].size = %X", i, pImageInfo->AppMarkerInfo[i].Size);
    }
    #endif
}
#endif
