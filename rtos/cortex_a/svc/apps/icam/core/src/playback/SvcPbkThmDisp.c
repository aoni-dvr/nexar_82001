/**
 *  @file SvcPbkThmDisp.c
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
 *  @details thm display related APIs
 *
 */

#include ".svc_autogen"

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaDSP.h"
#include "AmbaDSP_VOUT.h"
#include "AmbaDSP_StillDec.h"
#include "AmbaDCF.h"
#include "AmbaFS.h"
#include "AmbaExif.h"
#include "AmbaVfs.h"
#include "AmbaUtility.h"
#include "AmbaDMA_Def.h"
#include "AmbaAudio_AENC.h"
#include "AmbaAudio_AIN.h"
#include "AmbaAudio_ADEC.h"
#include "AmbaAudio_AOUT.h"
#include "AmbaAudioEfx.h"
#include "AmbaAudioBuf.h"
#include "AmbaAudio.h"
#include "AmbaGDMA.h"

#include "SvcLog.h"
#include "SvcErrCode.h"
#include "SvcPbkStillDec.h"
#include "SvcPbkCtrl.h"
#include "SvcStgMonitor.h"
#include "SvcErrCode.h"
#include "SvcPbkStillDec.h"
#include "SvcWrap.h"
#include "SvcPbkStillDisp.h"
#include "SvcClock.h"
#include "SvcPbkThmDisp.h"

#define SVC_LOG_THM_DISP    "THMDSP"

static THMDISP_MGR_s        G_ThmDispMgr;
static SVC_SDEC_FILE_s      G_ThmFileList[SVC_PICT_DISP_THM_BUF_NUM];

static void PbkDebugUInt5(const char *FmtStr, ULONG Arg1, ULONG Arg2, ULONG Arg3, ULONG Arg4, ULONG Arg5)
{
    const SVC_PICT_DISP_INFO_s *pDispInfo;

    SvcPbkPictDisp_GetInfo(&pDispInfo);

    if (pDispInfo->ShowDbgLog > 0U) {
        SvcWrap_PrintUL(FmtStr, Arg1, Arg2, Arg3, Arg4, Arg5);
    }
}

static UINT32 ThmDispClearThmBuffer(UINT32 FileListEntry)
{
    AMBA_DSP_YUV_IMG_BUF_s DstYuv;
    UINT8 *pU8;
    UINT32 YuvSize;

    DstYuv.BaseAddrY     = G_ThmFileList[FileListEntry].YuvBuf.pYBase;
    DstYuv.BaseAddrUV    = G_ThmFileList[FileListEntry].YuvBuf.pUVBase;
    DstYuv.Window.Height  = (UINT16)SVC_PICT_DISP_THM_BUF_HEIGHT;
    DstYuv.Pitch          = (UINT16)SVC_PICT_DISP_THM_BUF_PITCH;

    YuvSize = ((UINT32)DstYuv.Pitch * (UINT32)DstYuv.Window.Height);
    AmbaMisra_TypeCast(&pU8, &DstYuv.BaseAddrY);
    AmbaUtility_MemorySetU8(pU8, 0x0U, YuvSize);

    AmbaMisra_TypeCast(&pU8, &DstYuv.BaseAddrUV);
#if (SVCAG_SDEC_DISP_FORMAT == AMBA_DSP_YUV422)
    AmbaUtility_MemorySetU8(pU8, 0x0U, YuvSize);
#else
    AmbaUtility_MemorySetU8(pU8, 0x0U, YuvSize/2U);
#endif

    return SVC_OK;
}

static UINT32 ThmDispMarkErrorFile(UINT32 FileListEntry, UINT32 FileIndex)
{
    G_ThmFileList[FileListEntry].FileIndex = FileIndex;
    G_ThmFileList[FileListEntry].ErrorFlag = 1;
    G_ThmFileList[FileListEntry].Valid     = 1;

    return OK;
}

static UINT32 SvcThmDisp_EnlargeThumbIcon(UINT32 LayoutPosition, UINT32 FileIndex, UINT32 RotateFlip)
{
    UINT32                        FileListEntry;
    PBK_STILL_DISP_PARAM_s        DispImg;
    UINT32                        DispWidth = 0U, DispHeight = 0U;
    PBK_STILL_DISP_DRAW_RECT_s    RectInfo;
    PBK_STILL_DISP_VOUT_FRM_BUF_s VoutBuf;
    UINT32                        Err, i, VoutId;
    const SVC_PICT_DISP_INFO_s    *pDispInfo;

    PbkDebugUInt5("SvcThmDisp_EnlargeThumbIcon start", 0U, 0U, 0U, 0U, 0U);

    FileListEntry = FileIdxToFileListEntry(FileIndex);

    SvcPbkPictDisp_GetInfo(&pDispInfo);

    for (i = 0U; i < pDispInfo->DispNum; i++) {
        Err = AmbaWrap_memset(&DispImg, 0, sizeof(PBK_STILL_DISP_PARAM_s));
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_THM_DISP, "AmbaWrap_memset failed %u", Err, 0U);
        }

        VoutId = pDispInfo->Disp[i].VoutId;
        if (VoutId < AMBA_DSP_MAX_VOUT_NUM) {
            DispWidth  = G_ThmDispMgr.ThmDispLayout[VoutId][LayoutPosition].Width;
            DispHeight = G_ThmDispMgr.ThmDispLayout[VoutId][LayoutPosition].Height;


            /* Display single frame */
            DispImg.SrcBufYBase  = G_ThmFileList[FileListEntry].YuvBuf.pYBase;
            DispImg.SrcBufUVBase = G_ThmFileList[FileListEntry].YuvBuf.pUVBase;
            DispImg.SrcOffsetX   = 0;
            DispImg.SrcOffsetY   = 0;
            if (pDispInfo->Mode == SVC_PICT_DISP_MODE_PLAYBACK) {
                DispImg.SrcWidth     = SVC_PICT_DISP_THM_BUF_WIDTH;
                DispImg.SrcHeight    = SVC_PICT_DISP_THM_BUF_HEIGHT;
                DispImg.SrcBufPitch  = SVC_PICT_DISP_THM_BUF_PITCH;
                DispImg.SrcBufHeight = SVC_PICT_DISP_THM_BUF_HEIGHT;
            } else {
                UINT32 ThmWidth, ThmHeight;

                ThmWidth  = SVC_PICT_DISP_THM_BUF_DPX_WIDTH;
                ThmHeight = SVC_PICT_DISP_THM_BUF_DPX_HEIGHT;
                DispImg.SrcWidth       = ThmWidth;
                DispImg.SrcHeight      = ThmHeight;;
                DispImg.SrcBufPitch    = ALIGN64(ThmWidth);
                DispImg.SrcBufHeight   = ThmHeight;
            }
            DispImg.SrcChromaFmt = SVCAG_SDEC_DISP_FORMAT;
            DispImg.RotateFlip   = RotateFlip;
            DispImg.DstOffsetX   = G_ThmDispMgr.ThmDispLayout[VoutId][LayoutPosition].OffsetX;
            if (DispWidth <= G_ThmDispMgr.ThmDispLayout[VoutId][LayoutPosition].Width) {
                DispImg.DstOffsetX += ((G_ThmDispMgr.ThmDispLayout[VoutId][LayoutPosition].Width - DispWidth) >> 1U);
            }
            DispImg.DstOffsetY = G_ThmDispMgr.ThmDispLayout[VoutId][LayoutPosition].OffsetY;
            if (DispHeight <= G_ThmDispMgr.ThmDispLayout[VoutId][LayoutPosition].Height) {
                DispImg.DstOffsetY += ((G_ThmDispMgr.ThmDispLayout[VoutId][LayoutPosition].Height - DispHeight) >> 1U);
            }
            DispImg.DstWidth     = DispWidth;
            DispImg.DstHeight    = DispHeight;

            /* Draw rectangle frame. The rectangle surrounds the thm icon */
            SvcPbkStillDisp_GetCurrentVoutBuf(VoutId, &VoutBuf);
            RectInfo.LineWidth = 6U;
            RectInfo.VoutIdx   = VoutId;
            RectInfo.YBufAddr  = VoutBuf.YBase;
            RectInfo.UVBufAddr = VoutBuf.UVBase;
            RectInfo.Pitch     = VoutBuf.Pitch;
            RectInfo.Height    = VoutBuf.Height;
            RectInfo.StartX    = (DispImg.DstOffsetX - RectInfo.LineWidth) & 0xFFFFFFFEU;
            RectInfo.StartY    = (DispImg.DstOffsetY - RectInfo.LineWidth) & 0xFFFFFFFEU;
            RectInfo.RectWidth = (DispWidth + (RectInfo.LineWidth << 1U)) & 0xFFFFFFFEU;
            RectInfo.RectHeight= (DispHeight + (RectInfo.LineWidth << 1U)) & 0xFFFFFFFEU;
            RectInfo.Y         = 0xb0U;
            RectInfo.U         = 0x30U;
            RectInfo.V         = 0x50U;
            SvcPbkStillDisp_DrawRectangle(&RectInfo);
        }
    }

    PbkDebugUInt5("SvcThmDisp_EnlargeThumbIcon done", 0U, 0U, 0U, 0U, 0U);
    return SVC_OK;
}

static UINT32 SvcThmDisp_DisplayPage(UINT32 StartFileIndex, UINT32 FileNum, UINT32 RotateFlip)
{
    UINT32                     i, j, Err;
    UINT32                     FileAmount = 0U, FileIndex, FileListEntry = 0U, ThmExist;
    AMBA_FS_FILE               *pFile = NULL;
    UINT32                     ActualFlags;
    PBK_STILL_DISP_CONFIG_s    DispConfig[AMBA_DSP_MAX_VOUT_NUM];
    PBK_STILL_DISP_PARAM_s     DispImg[AMBA_DSP_MAX_VOUT_NUM][THMDISP_MAX_IMG_PER_FRAME];
    UINT32                     DispWidth = 0U, DispHeight = 0U;
    UINT32                     ValidVoutNum = 0U;
    SVC_PBK_CTRL_INFO_s        *pInfo = NULL;
    SVC_STG_MONI_DRIVE_INFO_s  DriveInfo;
    UINT32                     Rval = SVC_OK;
    AMBA_EXIF_THUMB_INFO_s     ThmInfo;
    AMBA_EXIF_IMAGE_INFO_s     *pExifImgInfo;
    AMBA_VFS_FILE_s            MediaFile;
    AMBA_FS_FILE_INFO_s        FileStat;
    UINT32                     AllDecoded = 1U, VoutId;
    const SVC_PICT_DISP_INFO_s *pDispInfo;

    PbkDebugUInt5("SvcThmDisp_DisplayPage StartFileIndex %d FileNum %d", StartFileIndex, FileNum, 0U, 0U, 0U);

    SvcPbkPictDisp_GetInfo(&pDispInfo);

    Err = AmbaWrap_memset(&DispConfig[0], 0, sizeof(DispConfig));
    if (Err != SVC_OK) {
        SvcLog_NG(SVC_LOG_THM_DISP, "AmbaWrap_memset failed %u", Err, 0U);
    }
    Err = AmbaWrap_memset(DispImg, 0, sizeof(DispImg));
    if (Err != SVC_OK) {
        SvcLog_NG(SVC_LOG_THM_DISP, "AmbaWrap_memset failed %u", Err, 0U);
    }
    Err = AmbaWrap_memset(&DriveInfo, 0, sizeof(DriveInfo));
    if (Err != SVC_OK) {
        SvcLog_NG(SVC_LOG_THM_DISP, "AmbaWrap_memset failed %u", Err, 0U);
    }
    Err = AmbaWrap_memset(&ThmInfo, 0, sizeof(ThmInfo));
    if (Err != SVC_OK) {
        SvcLog_NG(SVC_LOG_THM_DISP, "AmbaWrap_memset failed %u", Err, 0U);
    }
    Err = AmbaWrap_memset(&MediaFile, 0, sizeof(MediaFile));
    if (Err != SVC_OK) {
        SvcLog_NG(SVC_LOG_THM_DISP, "AmbaWrap_memset failed %u", Err, 0U);
    }
    Err = AmbaWrap_memset(&FileStat, 0, sizeof(FileStat));
    if (Err != SVC_OK) {
        SvcLog_NG(SVC_LOG_THM_DISP, "AmbaWrap_memset failed %u", Err, 0U);
    }

    FileIndex = StartFileIndex;

    SvcPbkCtrl_InfoGet(0U, &pInfo);
    if (SVC_OK == SvcStgMonitor_GetDriveInfo(pInfo->Drive, &DriveInfo)) {
        if (SVC_OK != AmbaDCF_GetFileAmount(DriveInfo.DriveID, 0U, G_ThmDispMgr.DcfScanType, &FileAmount)) {
            SvcLog_NG(SVC_LOG_THM_DISP, "fail to get file amount", 0U, 0U);
        }
    }

    /* Check if all images was decoded */
    for (i = 0U; i < FileNum; i++) {
        FileIndex = FileIdxWrapAroundForward(StartFileIndex + i, FileAmount);
        FileListEntry = FileIdxToFileListEntry(FileIndex);

        if ((G_ThmFileList[FileListEntry].Valid == 0U) || (FileIndex != G_ThmFileList[FileListEntry].FileIndex)) {
            AllDecoded = 0U;
            break;
        }
    }

    if (0U == AllDecoded) {
        for (i = 0U; i < FileNum; i++) {
            if (SVC_NG == Rval) {
                break;
            }

            ThmExist      = 0U;
            FileIndex     = FileIdxWrapAroundForward(StartFileIndex + i, FileAmount);
            FileListEntry = FileIdxToFileListEntry(FileIndex);

            if ((G_ThmFileList[FileListEntry].Valid == 0U) || (FileIndex != G_ThmFileList[FileListEntry].FileIndex)) {
                Err = ThmDispClearThmBuffer(FileListEntry);
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_THM_DISP, "ThmDispClearThmBuffer failed %u", Err, 0U);
                }

                if (SVC_OK == SvcStgMonitor_GetDriveInfo(pInfo->Drive, &DriveInfo)) {
                    if (SVC_OK != AmbaDCF_GetFileName(DriveInfo.DriveID, 0U, G_ThmDispMgr.DcfScanType, &G_ThmFileList[FileListEntry].FileName[0], FileIndex)) {
                        SvcLog_NG(SVC_LOG_THM_DISP, "SvcThmDisp_DisplayPage: GetFileName error", 0U, 0U);
                        Rval = SVC_NG;
                        continue;
                    }
                }

                if (SVC_OK != AmbaVFS_Open(&G_ThmFileList[FileListEntry].FileName[0], "r", 0U, &MediaFile)) {
                    SvcLog_NG(SVC_LOG_THM_DISP, "SvcThmDisp_DisplayPage: File not exist[%d]", FileIndex, 0U);
                    Err = ThmDispMarkErrorFile(FileListEntry, FileIndex);
                    if (Err != SVC_OK) {
                        SvcLog_NG(SVC_LOG_THM_DISP, "ThmDispClearThmBuffer failed %u", Err, 0U);
                    }
                    Rval = SVC_NG;
                    continue;
                }

                Err = SvcPbkPictDisp_GetFileType(&G_ThmFileList[FileListEntry].FileName[0], &G_ThmDispMgr.FileType);

                if (Err != SVC_OK) {
                    Err = AmbaVFS_Close(&MediaFile);
                    if (Err != SVC_OK) {
                        SvcLog_NG(SVC_LOG_THM_DISP, "ThmDispClearThmBuffer failed %u", Err, 0U);
                    }
                    continue;
                }

                if (G_ThmDispMgr.FileType == AMBA_DCF_FILE_TYPE_IMAGE) {
                    /* parse image file */
                    Err = AmbaWrap_memset(&(G_ThmDispMgr.ImgInfo), 0x0, sizeof(G_ThmDispMgr.ImgInfo));
                    if (Err != SVC_OK) {
                        SvcLog_NG(SVC_LOG_THM_DISP, "AmbaWrap_memset failed %u", Err, 0U);
                    }
                    Err = AmbaWrap_memset(&ThmInfo, 0x0, sizeof(ThmInfo));
                    if (Err != SVC_OK) {
                        SvcLog_NG(SVC_LOG_THM_DISP, "AmbaWrap_memset failed %u", Err, 0U);
                    }

                    Err = AmbaFS_FileOpen(&G_ThmFileList[FileListEntry].FileName[0], "r", &pFile);
                    if (Err != SVC_OK) {
                        SvcLog_NG(SVC_LOG_THM_DISP, "AmbaFS_Open failed %u", Err, 0U);
                        Rval = SVC_NG;
                    }

                    pExifImgInfo = &(G_ThmDispMgr.ImgInfo.Jpeg);
                    Err = AmbaExifFastParse(pFile, &ThmInfo);
                    if (SVC_OK == Err) {
                        pExifImgInfo->ThumbSize = ThmInfo.ThumbSize;
                        pExifImgInfo->ThumbPos = ThmInfo.ThumbPos;
                        pExifImgInfo->ThumbHeight = ThmInfo.ThumbHeight;
                        pExifImgInfo->ThumbWidth = ThmInfo.ThumbWidth;
                        pExifImgInfo->Thumbnail = ThmInfo.Thumbnail;
                        pExifImgInfo->YUVType = ThmInfo.YUVType;
                    } else {
                        SvcLog_NG(SVC_LOG_THM_DISP, "AmbaExifFastParse failed %u", Err, 0U);
                        Rval = SVC_NG;
                    }

                    if ((SVC_OK != Rval) || (G_ThmDispMgr.ImgInfo.Jpeg.Thumbnail == 0U)) {
                        Err = AmbaFS_GetFileInfo(&G_ThmFileList[FileListEntry].FileName[0], &FileStat);
                        if (Err != SVC_OK) {
                            SvcLog_NG(SVC_LOG_THM_DISP, "AmbaFS_GetFileInfo failed %u", Err, 0U);
                        }
                        Err = AmbaWrap_memset(&(G_ThmDispMgr.ImgInfo), 0x0, sizeof(G_ThmDispMgr.ImgInfo));
                        if (Err != SVC_OK) {
                            SvcLog_NG(SVC_LOG_THM_DISP, "AmbaWrap_memset failed %u", Err, 0U);
                        }
                        Err = AmbaExifFullParse(pFile, &(G_ThmDispMgr.ImgInfo.Jpeg), &FileStat);
                        if (Err != SVC_OK) {
                            SvcLog_NG(SVC_LOG_THM_DISP, "AmbaExifFullParse failed %u", Err, 0U);
                            Rval = SVC_NG;
                        }
                    }

                    Err = AmbaFS_FileClose(pFile);
                    if (Err != SVC_OK) {
                        SvcLog_NG(SVC_LOG_THM_DISP, "AmbaFS_Close failed %u", Err, 0U);
                        Rval = SVC_NG;
                    }

                    if (SVC_OK != Rval) {
                        SvcLog_NG(SVC_LOG_THM_DISP, "Parse Exif error[%d]", FileIndex, 0U);
                        Err = ThmDispMarkErrorFile(FileListEntry, FileIndex);
                        if (Err != SVC_OK) {
                            SvcLog_NG(SVC_LOG_THM_DISP, "ThmDispMarkErrorFile failed %u", Err, 0U);
                        }
                        Err = AmbaVFS_Close(&MediaFile);
                        if (Err != SVC_OK) {
                            SvcLog_NG(SVC_LOG_THM_DISP, "AmbaVFS_Close failed %u", Err, 0U);
                        }
                        Rval = SVC_NG;
                        continue;
                    }

                    /* Check sampling factor of SOF: Only support yuv420 and yuv422 */
                    if ((G_ThmDispMgr.ImgInfo.Jpeg.YUVType != 0x21U) && (G_ThmDispMgr.ImgInfo.Jpeg.YUVType != 0x22U)) {
                        PbkDebugUInt5("Unsupported YUV fmt 0x%x", G_ThmDispMgr.ImgInfo.Jpeg.YUVType, 0U, 0U, 0U, 0U);
                        Err = ThmDispMarkErrorFile(FileListEntry, FileIndex);
                        if (Err != SVC_OK) {
                            SvcLog_NG(SVC_LOG_THM_DISP, "ThmDispMarkErrorFile failed %u", Err, 0U);
                        }
                        Err = AmbaVFS_Close(&MediaFile);
                        if (Err != SVC_OK) {
                            SvcLog_NG(SVC_LOG_THM_DISP, "AmbaVFS_Close failed %u", Err, 0U);
                        }
                        Rval = SVC_NG;
                        continue;
                    }
                } else if (G_ThmDispMgr.FileType == AMBA_DCF_FILE_TYPE_VIDEO) {
                    SIZE_t                NameLength;
                    char                  ThmbName[64];
                    AMBA_FS_FILE_INFO_s   FileInfo;

                    SvcWrap_strcpy(&(ThmbName[0]), 64U, &G_ThmFileList[FileListEntry].FileName[0]);
                    NameLength = SvcWrap_strlen(&(ThmbName[0]));
                    ThmbName[NameLength - 3U] = 'T';
                    ThmbName[NameLength - 2U] = 'H';
                    ThmbName[NameLength - 1U] = 'M';

                    Err = AmbaFS_GetFileInfo(&ThmbName[0], &FileInfo);

                    if (Err == AMBA_FS_ERR_NONE) {
                        UINT8           ThumbBuf[100U];
                        UINT32          ThumbPos = 0U, ThumbAddr = 0U, NumSuccess;
                        AMBA_VFS_FILE_s  ThmbFile;

                        AmbaMisra_TouchUnused(&(ThumbBuf[0U]));
                        Err = AmbaWrap_memset(&ThmbFile, 0, sizeof(ThmbFile));
                        if (Err != SVC_OK) {
                            SvcLog_NG(SVC_LOG_THM_DISP, "AmbaWrap_memset failed %u", Err, 0U);
                        }

                        Err = AmbaVFS_Open(&(ThmbName[0]), "r", 0U, &ThmbFile);
                        if (Err != SVC_OK) {
                            SvcLog_NG(SVC_LOG_THM_DISP, "AmbaVFS_Open failed %u", Err, 0U);
                        }

                        if (Err == SVC_OK) {
                            Err = AmbaVFS_Read(ThumbBuf, 1U, 100U, &ThmbFile, &NumSuccess);
                            if (Err != SVC_OK) {
                                SvcLog_NG(SVC_LOG_THM_DISP, "AmbaVFS_Read failed %u", Err, 0U);
                            }
                        }

                        /* find the file position to feed (SKIP FF D8) */
                        if (Err == SVC_OK) {
                            for (ThumbPos = 0U; ThumbPos < 99U; ThumbPos++) {
                                if ((ThumbBuf[ThumbPos] == 0xFFU) && (ThumbBuf[ThumbPos + 1U] == 0xD8U)) {
                                    ThumbAddr = ThumbPos + 2U;
                                    ThmExist = 1U;
                                    break;
                                }
                            }
                        }

                        if (Err == SVC_OK) {
                            if (ThmExist == 1U) {
                                pExifImgInfo              = &(G_ThmDispMgr.ImgInfo.Jpeg);
                                pExifImgInfo->ThumbSize   = (UINT32)FileInfo.Size;
                                pExifImgInfo->ThumbHeight = 88U;
                                pExifImgInfo->ThumbWidth  = 160U;
                                pExifImgInfo->ThumbPos    = ThumbAddr;

                                SvcWrap_strcpy(&G_ThmFileList[FileListEntry].FileName[0], 64U, &(ThmbName[0]));

                                Err = AmbaVFS_Close(&ThmbFile);
                                if (Err != SVC_OK) {
                                    SvcLog_NG(SVC_LOG_THM_DISP, "AmbaVFS_Close failed %u", Err, 0U);
                                }
                            }
                        }
                    }

                    /* if no THM file, use video I frame */
                    if (ThmExist == 0U) {
                        void           *pHdlr;
                        AMBA_PLAYER_s  Player;

                        Err = AmbaStreamWrap_GetHdlr(&pHdlr);
                        if (Err != SVC_OK) {
                            SvcLog_NG(SVC_LOG_THM_DISP, "AmbaStreamWrap_GetHdlr failed %u, LINE %u", Err, __LINE__);
                            Rval = SVC_NG;
                        }

                        Err = AmbaPlayer_Parse(&Player, pHdlr, G_ThmFileList[FileListEntry].FileName, AMBA_PLAYER_MTYPE_MOVIE);
                        if (Err != SVC_OK) {
                            SvcLog_NG(SVC_LOG_THM_DISP, "AmbaPlayer_Parse failed %u, LINE %u", Err, __LINE__);
                            Rval = SVC_NG;
                        }

                        Err = AmbaWrap_memcpy(&(G_ThmDispMgr.ImgInfo.Mov), &(Player.ExpMov), sizeof(Player.ExpMov));
                        if (Err != SVC_OK) {
                            SvcLog_NG(SVC_LOG_THM_DISP, "AmbaWrap_memcpy failed %u, LINE %u", Err, __LINE__);
                            Rval = SVC_NG;
                        }

                        if (Rval != SVC_OK) {
                            continue;
                        }
                    }
                } else {
                    /* UNKNOWN */
                }
                Err = AmbaVFS_Close(&MediaFile);
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_THM_DISP, "AmbaVFS_Close failed %u", Err, 0U);
                }

                SvcSDecConfig.ImgNum = 1;
                SvcWrap_strcpy(&(SvcSDecConfig.Config[0].FName[0]), 64U, &G_ThmFileList[FileListEntry].FileName[0]);

                if ((G_ThmDispMgr.FileType == AMBA_DCF_FILE_TYPE_IMAGE) || (ThmExist == 1U)) {
                    SvcSDecConfig.Config[0].Type                  = SDEC_IMG_TYPE_THM;
                    SvcSDecConfig.Config[0].Info.Jpeg.Width       = (UINT16)G_ThmDispMgr.ImgInfo.Jpeg.ThumbWidth;
                    SvcSDecConfig.Config[0].Info.Jpeg.Height      = (UINT16)G_ThmDispMgr.ImgInfo.Jpeg.ThumbHeight;
                    SvcSDecConfig.Config[0].Info.Jpeg.BitsOffset  = G_ThmDispMgr.ImgInfo.Jpeg.ThumbPos;
                    SvcSDecConfig.Config[0].Info.Jpeg.BitsSize    = G_ThmDispMgr.ImgInfo.Jpeg.ThumbSize;
                    SvcSDecConfig.Config[0].Info.Jpeg.Orientation = 0;
                    if (G_ThmDispMgr.ImgInfo.Jpeg.ThumbDHTStat == -1) {
                        SvcSDecConfig.Config[0].Info.Jpeg.EnableJpegTrans = 1;
                        Err = AmbaWrap_memcpy(&SvcSDecConfig.Config[0].Info.Jpeg.JpegHeaderInfo,
                                        &G_ThmDispMgr.ImgInfo.Jpeg.JPEGThumbHeaderInfo,
                                        sizeof(AMBA_EXIF_JPEG_HEADER_INFO_s));
                        if (Err != SVC_OK) {
                            SvcLog_NG(SVC_LOG_THM_DISP, "AmbaWrap_memcpy failed %u", Err, 0U);
                        }
                        Err = AmbaWrap_memcpy(&SvcSDecConfig.Config[0].Info.Jpeg.JpegInfo,
                                        &G_ThmDispMgr.ImgInfo.Jpeg.JPEGThumbInfo,
                                        sizeof(AMBA_EXIF_JPEG_INFO_s));
                        if (Err != SVC_OK) {
                            SvcLog_NG(SVC_LOG_THM_DISP, "AmbaWrap_memcpy failed %u", Err, 0U);
                        }
                    } else {
                        SvcSDecConfig.Config[0].Info.Jpeg.EnableJpegTrans = 0;
                    }

                    SvcSDecConfig.Config[0].SubEnable        = 1;
                    SvcSDecConfig.Config[0].SubRotateFlip    = 0;
                    SvcSDecConfig.Config[0].SubYuvOffsetX    = 0;
                    SvcSDecConfig.Config[0].SubYuvOffsetY    = 0;
                    if (pDispInfo->Mode == SVC_PICT_DISP_MODE_PLAYBACK) {
                        SvcSDecConfig.Config[0].SubYuvWidth      = (UINT16)SVC_PICT_DISP_THM_BUF_WIDTH;
                        SvcSDecConfig.Config[0].SubYuvHeight     = (UINT16)SVC_PICT_DISP_THM_BUF_HEIGHT;
                        SvcSDecConfig.Config[0].SubBufPitch      = (UINT16)(SVC_PICT_DISP_THM_BUF_PITCH);
                        SvcSDecConfig.Config[0].SubBufHeight     = (UINT16)SVC_PICT_DISP_THM_BUF_HEIGHT;
                    } else {
                        UINT32 ThmWidth, ThmHeight;

                        ThmWidth  = SVC_PICT_DISP_THM_BUF_DPX_WIDTH;
                        ThmHeight = SVC_PICT_DISP_THM_BUF_DPX_HEIGHT;
                        SvcSDecConfig.Config[0].SubYuvWidth      = (UINT16)ThmWidth;
                        SvcSDecConfig.Config[0].SubYuvHeight     = (UINT16)ThmHeight;;
                        SvcSDecConfig.Config[0].SubBufPitch      = (UINT16)ALIGN64(ThmWidth);
                        SvcSDecConfig.Config[0].SubBufHeight     = (UINT16)ThmHeight;
                    }
                    SvcSDecConfig.Config[0].SubBufYBase      = G_ThmFileList[FileListEntry].YuvBuf.pYBase;
                    SvcSDecConfig.Config[0].SubBufUVBase     = G_ThmFileList[FileListEntry].YuvBuf.pUVBase;
                } else {
                    if ((G_ThmDispMgr.ImgInfo.Mov.VideoTrack[0].MediaId == AMBA_FORMAT_MID_H264) || (G_ThmDispMgr.ImgInfo.Mov.VideoTrack[0].MediaId == AMBA_FORMAT_MID_AVC)) {
                        SvcSDecConfig.Config[0].Type = SDEC_IMG_TYPE_H264_IFRAME;
                    } else if ((G_ThmDispMgr.ImgInfo.Mov.VideoTrack[0].MediaId == AMBA_FORMAT_MID_H265) || (G_ThmDispMgr.ImgInfo.Mov.VideoTrack[0].MediaId == AMBA_FORMAT_MID_HVC)) {
                        SvcSDecConfig.Config[0].Type = SDEC_IMG_TYPE_HEVC_IFRAME;
                    } else {
                        SvcSDecConfig.Config[0].Type = SDEC_IMG_TYPE_MJPEG;
                    }
                    Err = AmbaWrap_memcpy(&SvcSDecConfig.Config[0].Info.Mov, &G_ThmDispMgr.ImgInfo.Mov, sizeof(G_ThmDispMgr.ImgInfo.Mov));
                    if (Err != SVC_OK) {
                        SvcLog_NG(SVC_LOG_THM_DISP, "AmbaWrap_memcpy failed %u", Err, 0U);
                    }

                    SvcSDecConfig.Config[0].SubEnable     = 1;
                    SvcSDecConfig.Config[0].SubRotateFlip = 0;
                    SvcSDecConfig.Config[0].SubYuvOffsetX = 0;
                    SvcSDecConfig.Config[0].SubYuvOffsetY = 0;
                    if (pDispInfo->Mode == SVC_PICT_DISP_MODE_PLAYBACK) {
                        SvcSDecConfig.Config[0].SubYuvWidth      = (UINT16)SVC_PICT_DISP_THM_BUF_WIDTH;
                        SvcSDecConfig.Config[0].SubYuvHeight     = (UINT16)SVC_PICT_DISP_THM_BUF_HEIGHT;
                        SvcSDecConfig.Config[0].SubBufPitch      = (UINT16)(SVC_PICT_DISP_THM_BUF_PITCH);
                        SvcSDecConfig.Config[0].SubBufHeight     = (UINT16)SVC_PICT_DISP_THM_BUF_HEIGHT;
                    } else {
                        UINT32 ThmWidth, ThmHeight;

                        ThmWidth  = SVC_PICT_DISP_THM_BUF_DPX_WIDTH;
                        ThmHeight = SVC_PICT_DISP_THM_BUF_DPX_HEIGHT;
                        SvcSDecConfig.Config[0].SubYuvWidth      = (UINT16)ThmWidth;
                        SvcSDecConfig.Config[0].SubYuvHeight     = (UINT16)ThmHeight;;
                        SvcSDecConfig.Config[0].SubBufPitch      = (UINT16)ALIGN64(ThmWidth);
                        SvcSDecConfig.Config[0].SubBufHeight     = (UINT16)ThmHeight;
                    }
                    SvcSDecConfig.Config[0].SubBufYBase   = G_ThmFileList[FileListEntry].YuvBuf.pYBase;
                    SvcSDecConfig.Config[0].SubBufUVBase  = G_ThmFileList[FileListEntry].YuvBuf.pUVBase;
                }
                Err = SvcPbkStillDec_StartDecode();
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_THM_DISP, "SvcPbkStillDec_StartDecode failed %u", Err, 0U);
                }

                if (AmbaKAL_EventFlagGet(&(G_ThmDispMgr.EventId),
                                         SDEC_EVENT_DECODE_DONE,
                                         AMBA_KAL_FLAGS_ANY,
                                         AMBA_KAL_FLAGS_CLEAR_AUTO,
                                         &ActualFlags,
                                         AMBA_KAL_WAIT_FOREVER) != SVC_OK) {
                    SvcLog_NG(SVC_LOG_THM_DISP, "SvcThmDisp_DisplayPage: timeout", 0U, 0U);
                    Rval = SVC_NG;
                    continue;
                }
                G_ThmFileList[FileListEntry].FileIndex = FileIndex;
                G_ThmFileList[FileListEntry].ErrorFlag = 0;
                G_ThmFileList[FileListEntry].Valid = 1;
            }
        }
    }

    // Rescale
    if (SVC_OK == Rval) {
        SvcPbkPictDisp_GetInfo(&pDispInfo);

        for (i = 0; i < FileNum; i++) {
            FileIndex = FileIdxWrapAroundForward(StartFileIndex + i, FileAmount);
            FileListEntry = FileIdxToFileListEntry(FileIndex);

            for (j = 0U; j < pDispInfo->DispNum; j++) {
                VoutId = pDispInfo->Disp[j].VoutId;
                if (VoutId < AMBA_DSP_MAX_VOUT_NUM) {
                    DispWidth  = G_ThmDispMgr.ThmDispLayout[VoutId][i].Width;
                    DispHeight = G_ThmDispMgr.ThmDispLayout[VoutId][i].Height;

                    DispImg[VoutId][i].SrcBufYBase        = G_ThmFileList[FileListEntry].YuvBuf.pYBase;
                    DispImg[VoutId][i].SrcBufUVBase       = G_ThmFileList[FileListEntry].YuvBuf.pUVBase;
                    DispImg[VoutId][i].SrcOffsetX         = 0;
                    DispImg[VoutId][i].SrcOffsetY         = 0;
                    if (pDispInfo->Mode == SVC_PICT_DISP_MODE_PLAYBACK) {
                        DispImg[VoutId][i].SrcWidth           = SVC_PICT_DISP_THM_BUF_WIDTH;
                        DispImg[VoutId][i].SrcHeight          = SVC_PICT_DISP_THM_BUF_HEIGHT;
                        DispImg[VoutId][i].SrcBufPitch        = SVC_PICT_DISP_THM_BUF_PITCH;
                        DispImg[VoutId][i].SrcBufHeight       = SVC_PICT_DISP_THM_BUF_HEIGHT;
                    } else {
                        UINT32 ThmWidth, ThmHeight;

                        ThmWidth  = SVC_PICT_DISP_THM_BUF_DPX_WIDTH;
                        ThmHeight = SVC_PICT_DISP_THM_BUF_DPX_HEIGHT;
                        DispImg[VoutId][i].SrcWidth       = ThmWidth;
                        DispImg[VoutId][i].SrcHeight      = ThmHeight;;
                        DispImg[VoutId][i].SrcBufPitch    = ALIGN64(ThmWidth);
                        DispImg[VoutId][i].SrcBufHeight   = ThmHeight;
                    }
                    DispImg[VoutId][i].SrcChromaFmt = SVCAG_SDEC_DISP_FORMAT;
                    DispImg[VoutId][i].RotateFlip   = RotateFlip;
                    DispImg[VoutId][i].DstOffsetX   = ((G_ThmDispMgr.ThmDispLayout[VoutId][i].Width - DispWidth) >> 1U) + G_ThmDispMgr.ThmDispLayout[VoutId][i].OffsetX;
                    DispImg[VoutId][i].DstOffsetX   = (DispImg[VoutId][i].DstOffsetX+1U) & 0xFFFFFFFEU;
                    DispImg[VoutId][i].DstOffsetY   = ((G_ThmDispMgr.ThmDispLayout[VoutId][i].Height - DispHeight) >> 1U) + G_ThmDispMgr.ThmDispLayout[VoutId][i].OffsetY;
                    DispImg[VoutId][i].DstOffsetY   = (DispImg[VoutId][i].DstOffsetY+1U) & 0xFFFFFFFEU;
                    DispImg[VoutId][i].DstWidth     = DispWidth;
                    DispImg[VoutId][i].DstHeight    = DispHeight;
                    PbkDebugUInt5(" , __LINE__ %d DstOffsetX %d, DstOffsetY %d", __LINE__, DispImg[VoutId][i].DstOffsetX, DispImg[VoutId][i].DstOffsetY, 0U, 0U);
                }
            }
        }

        for (j = 0U; j < pDispInfo->DispNum; j++) {
            VoutId = pDispInfo->Disp[j].VoutId;
            if (VoutId < AMBA_DSP_MAX_VOUT_NUM) {
                DispConfig[ValidVoutNum].VoutIdx      = VoutId;
                DispConfig[ValidVoutNum].FlushVoutBuf = 0U;
                DispConfig[ValidVoutNum].NumImg       = FileNum;
                DispConfig[ValidVoutNum].pParam       = &(DispImg[VoutId][0]);
                ValidVoutNum++;
            }
        }

        SvcPbkStillDisp_RescaleFrmToVoutBuf(ValidVoutNum, &DispConfig[0]);

        Err = SvcThmDisp_EnlargeThumbIcon((G_ThmDispMgr.CurFileIndex - G_ThmDispMgr.PageFirstFile), \
                                          G_ThmDispMgr.CurFileIndex,
                                          G_ThmDispMgr.ImgRotate);
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_THM_DISP, "SvcThmDisp_EnlargeThumbIcon failed %u", Err, 0U);
        }
        /* Display */
        SvcPbkStillDisp_FlushVoutBuf(ValidVoutNum, &DispConfig[0]);

        if (pDispInfo->Mode == SVC_PICT_DISP_MODE_PLAYBACK) {
            if (AmbaKAL_EventFlagGet(&(G_ThmDispMgr.EventId),
                                    STILL_DISP_EVENT_DISP_TO_VOUT,
                                    AMBA_KAL_FLAGS_ANY,
                                    AMBA_KAL_FLAGS_CLEAR_AUTO,
                                    &ActualFlags,
                                    AMBA_KAL_WAIT_FOREVER) != SVC_OK) {
                SvcLog_NG(SVC_LOG_THM_DISP, "SvcThmDisp_DisplayPage: timeout", 0U, 0U);
                Rval = SVC_NG;
            }
        }
    }
    PbkDebugUInt5("SvcThmDisp_DisplayPage done", FileIndex, 0U, 0U, 0U, 0U);

    return Rval;
}

static UINT32 ThmDispThmViewStart(AMBA_DCF_FILE_TYPE_e DcfScanType, UINT32 FileIndex)
{
    UINT32                          PageFileNum = 0U, FileAmount = 0U, Err;
    SVC_SDEC_CTRL_s                 SDecCtrl;
    PBK_STILL_DISP_VOUT_INFO_s      DispInfo[AMBA_DSP_MAX_VOUT_NUM];
    UINT32                          i, j, DispNum = 0U;
    UINT16                          DefWidth, DefHeight;
    SVC_PBK_CTRL_INFO_s             *pInfo = NULL;
    const SVC_PBK_CTRL_VOUT_INFO_s  *pVout;
    SVC_STG_MONI_DRIVE_INFO_s       DriveInfo;
    SVC_PICT_DISP_MEM_s             *PictDispMem = NULL;
    const SVC_PICT_DISP_INFO_s      *pDispInfo;
    const THMDISP_CONFIG_s          *pDefLayout;
    THMDISP_CONFIG_s                *pLayout;
    const AMBA_DSP_WINDOW_s         *pDispWin;

    static const THMDISP_CONFIG_s ThmLayout_16X9[THMDISP_PAGE_FILE_NUM] =    /* Based on 1000 x 1000 */
    {
#define LAYOUT_16_9_BASE_WIDTH  (1000U)
#define LAYOUT_16_9_BASE_HEIGHT (1000U)
        /* 1st row */
        {   40,   80,  208,  278},
        {  270,   80,  208,  278},
        {  500,   80,  208,  278},
        {  730,   80,  208,  278},
        /* 2nd row */
        {   40,  370,  208,  278},
        {  270,  370,  208,  278},
        {  500,  370,  208,  278},
        {  730,  370,  208,  278},
        /* 3rd row */
        {   40,  660,  208,  278},
        {  270,  660,  208,  278},
        {  500,  660,  208,  278},
        {  730,  660,  208,  278},
    };

    static const THMDISP_CONFIG_s ThmLayout_4X3[THMDISP_PAGE_FILE_NUM] =    /* Based on 1000 x 1000*/
    {
#define LAYOUT_4_3_BASE_WIDTH  (1000U)
#define LAYOUT_4_3_BASE_HEIGHT (1000U)
        /* 1st row */
        {   60,  106,  208,  208},
        {  284,  106,  208,  208},
        {  504,  106,  208,  208},
        {  724,  106,  208,  208},
        /* 2nd row */
        {   60,  390,  208,  208},
        {  284,  390,  208,  208},
        {  504,  390,  208,  208},
        {  724,  390,  208,  208},
        /* 3rd row */
        {   60,  674,  208,  208},
        {  284,  674,  208,  208},
        {  504,  674,  208,  208},
        {  724,  674,  208,  208},
    };

    Err = AmbaWrap_memset(&SDecCtrl, 0, sizeof(SVC_SDEC_CTRL_s));
    if (Err != SVC_OK) {
        SvcLog_NG(SVC_LOG_THM_DISP, "AmbaWrap_memset failed %u", Err, 0U);
    }
    Err = AmbaWrap_memset(DispInfo, 0, sizeof(DispInfo));
    if (Err != SVC_OK) {
        SvcLog_NG(SVC_LOG_THM_DISP, "AmbaWrap_memset failed %u", Err, 0U);
    }
    Err = AmbaWrap_memset(&DriveInfo, 0, sizeof(SVC_STG_MONI_DRIVE_INFO_s));
    if (Err != SVC_OK) {
        SvcLog_NG(SVC_LOG_THM_DISP, "AmbaWrap_memset failed %u", Err, 0U);
    }

    PbkDebugUInt5("SvcThmDisp_ThmViewStart start", 0U, 0U, 0U, 0U, 0U);
    SvcPbkStillDisp_WaitDspDispIdle();

    SvcPbkPictDisp_GetPictDispMem(&PictDispMem);
    SDecCtrl.MainBufBase = PictDispMem->JpegDecOutBuf.BufBase;
    SDecCtrl.MainBufSize = PictDispMem->JpegDecOutBuf.BufSize;
    SDecCtrl.BitsBufBase = PictDispMem->JpegDecBitsBuf.BufBase;
    SDecCtrl.BitsBufSize = PictDispMem->JpegDecBitsBuf.BufSize;
    SDecCtrl.pFbEventId  = &G_ThmDispMgr.EventId;   /* Register feedback event id*/
    SvcPbkStillDec_Setup(&SDecCtrl);

    SvcPbkCtrl_InfoGet(0U, &pInfo);
    SvcPbkPictDisp_GetInfo(&pDispInfo);
    /* Setup SDISP SVC */

    if (pDispInfo->Mode == SVC_PICT_DISP_MODE_PLAYBACK) {
        for (i = 0U; i < pDispInfo->DispNum; i++) {
            for (j = 0U; j < pInfo->VoutNum; j++) {
                pVout = &(pInfo->VoutInfo[j]);
                if (pDispInfo->Disp[i].VoutId == pVout->VoutIdx) {
                    DispInfo[DispNum].VoutId      = pVout->VoutIdx;
                    DispInfo[DispNum].Width       = pVout->VoutWindow.Width;
                    DispInfo[DispNum].Height      = pVout->VoutWindow.Height;
                    DispInfo[DispNum].VoutAR      = pVout->VoutWindowAR;
                    DispInfo[DispNum].Pitch       = ALIGN64(DispInfo[DispNum].Width);
                    DispInfo[DispNum].RotateFlip  = pVout->VoutRotateFlip;
                    DispNum++;
                    break;
                }
            }
        }
    } else {
        DispInfo[DispNum].VoutId      = pDispInfo->Disp[0].VoutId;
        DispInfo[DispNum].Width       = SVC_PICT_DISP_SCRNAIL_BUF_WIDTH;
        DispInfo[DispNum].Height      = SVC_PICT_DISP_SCRNAIL_BUF_HEIGHT;
        DispInfo[DispNum].VoutAR      = (DispInfo[DispNum].Width << 16U) / DispInfo[DispNum].Height;
        DispInfo[DispNum].Pitch       = ALIGN64(pDispInfo->Disp[0].DispWin.Width);
        DispInfo[DispNum].RotateFlip  = 0U;
        DispNum++;
    }

    Err = SvcPbkStillDisp_Setup(&G_ThmDispMgr.EventId, DispNum, DispInfo);  /* Register feedback event id*/
    if (Err != SVC_OK) {
        SvcLog_NG(SVC_LOG_THM_DISP, "SvcPbkStillDisp_Setup failed %u", Err, 0U);
    }

    /* THM Layout */
    for (i = 0U; i < DispNum; i++) {
        if ((DispInfo[i].VoutAR == AR_16X9) || (DispInfo[i].VoutAR == 0x115651U)) {
            pDefLayout  = ThmLayout_16X9;
            DefWidth    = LAYOUT_16_9_BASE_WIDTH;
            DefHeight   = LAYOUT_16_9_BASE_HEIGHT;
        } else if (DispInfo[i].VoutAR == AR_4X3) {
            pDefLayout  = ThmLayout_4X3;
            DefWidth    = LAYOUT_4_3_BASE_WIDTH;
            DefHeight   = LAYOUT_4_3_BASE_HEIGHT;
        } else {
            SvcLog_DBG(SVC_LOG_THM_DISP, "Unsupport this aspect ratio, use 16X9 instead, %u", DispInfo[i].VoutAR, 0U);
            pDefLayout  = ThmLayout_16X9;
            DefWidth    = LAYOUT_16_9_BASE_WIDTH;
            DefHeight   = LAYOUT_16_9_BASE_HEIGHT;
        }

        pLayout  = G_ThmDispMgr.ThmDispLayout[DispInfo[i].VoutId];
        pDispWin = &(pDispInfo->Disp[i].DispWin);

        for (j = 0U; j < THMDISP_PAGE_FILE_NUM; j++) {
            if (pDispInfo->Mode == SVC_PICT_DISP_MODE_PLAYBACK) {
                pLayout[j].OffsetX = (pDispWin->Width * pDefLayout[j].OffsetX) / DefWidth;
                pLayout[j].OffsetY = (pDispWin->Height * pDefLayout[j].OffsetY) / DefHeight;
                pLayout[j].Width   = (pDispWin->Width * pDefLayout[j].Width) / DefWidth;
                pLayout[j].Height  = (pDispWin->Height * pDefLayout[j].Height) / DefHeight;
            } else {
                pLayout[j].OffsetX = (SVC_PICT_DISP_SCRNAIL_BUF_WIDTH  * pDefLayout[j].OffsetX) / DefWidth;
                pLayout[j].OffsetY = (SVC_PICT_DISP_SCRNAIL_BUF_HEIGHT * pDefLayout[j].OffsetY) / DefHeight;
                pLayout[j].Width   = (SVC_PICT_DISP_SCRNAIL_BUF_WIDTH  * pDefLayout[j].Width) / DefWidth;
                pLayout[j].Height  = (SVC_PICT_DISP_SCRNAIL_BUF_HEIGHT * pDefLayout[j].Height) / DefHeight;
            }
        }
    }

    G_ThmDispMgr.DcfScanType = DcfScanType;

    if (SVC_OK == SvcStgMonitor_GetDriveInfo(pInfo->Drive, &DriveInfo)) {
        if (SVC_OK != AmbaDCF_GetFileAmount(DriveInfo.DriveID, 0U, AMBA_DCF_FILE_TYPE_VIDEO, &FileAmount)) {//G_ThmDispMgr.DcfScanType
            SvcLog_NG(SVC_LOG_THM_DISP, "fail to get file amount", 0U, 0U);
        }
    }

    if (FileAmount > 0U) {
        UINT8  Str34C = (UINT8)pInfo->Drive;   /* CERT STR34-C */

        G_ThmDispMgr.CurFileIndex = ((FileIndex == 0U) || (FileIndex > FileAmount)) ? FileAmount : FileIndex;
        G_ThmDispMgr.PageFirstFile = PageFstFileIdx(G_ThmDispMgr.CurFileIndex);

        PbkDebugUInt5("SvcThmDisp_ThmViewStart , __LINE__ %d, CurFileIndex %d, PageFirstFile %d, FileAmount %d, Drive %d", \
                __LINE__, G_ThmDispMgr.CurFileIndex, G_ThmDispMgr.PageFirstFile, FileAmount, Str34C);

        if (((FileAmount - G_ThmDispMgr.PageFirstFile) + 1U) >= THMDISP_PAGE_FILE_NUM) {
            PageFileNum = THMDISP_PAGE_FILE_NUM;
        } else {
            PageFileNum = (FileAmount - G_ThmDispMgr.PageFirstFile) + 1U;
        }

        Err = SvcThmDisp_DisplayPage(G_ThmDispMgr.PageFirstFile, PageFileNum, G_ThmDispMgr.ImgRotate);
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_THM_DISP, "SvcThmDisp_DisplayPage failed %u", Err, 0U);
        }
        SvcPbkPictDisp_SetState(SVC_PICT_DISP_THM_VIEW_STATE);
    }
    return SVC_OK;
}

/**
* SvcThmDisp_Create
* @return ErrorCode
*/
UINT32 SvcThmDisp_Create(void)
{
    UINT32                       i = 0U, RetVal = SVC_OK, Err;
    UINT32                       ThmBufSize;
    ULONG                        pBuf;
    static char                  ThmDispEvtName[] = "ThmDispMgrFlg";
    SVC_PICT_DISP_MEM_s          *PictDispMem = NULL;
    const SVC_PICT_DISP_INFO_s   *pDispInfo;

    SvcPbkPictDisp_GetInfo(&pDispInfo);

    Err = AmbaWrap_memset((&G_ThmDispMgr), 0, sizeof(THMDISP_MGR_s));
    if (Err != SVC_OK) {
        SvcLog_NG(SVC_LOG_THM_DISP, "AmbaWrap_memset failed %u", Err, 0U);
    }

    if (KAL_ERR_NONE != AmbaKAL_EventFlagCreate(&(G_ThmDispMgr.EventId), ThmDispEvtName)) {
        RetVal = SVC_NG;
    } else {
        Err = AmbaKAL_EventFlagClear(&(G_ThmDispMgr.EventId), 0xFFFFFFFFU);
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_THM_DISP, "AmbaKAL_EventFlagClear failed %u", Err, 0U);
        }

        if (pDispInfo->Mode == SVC_PICT_DISP_MODE_PLAYBACK) {
            ThmBufSize = THMDISP_THM_YUV_SIZE;
        } else {
            UINT32 ThmWidth, ThmHeight;

            ThmWidth   = SVC_PICT_DISP_THM_BUF_DPX_WIDTH;
            ThmHeight  = SVC_PICT_DISP_THM_BUF_DPX_HEIGHT;
            ThmBufSize = ThmHeight * ALIGN64(ThmWidth);
        }

        /* Initialize parameters */
        SvcPbkPictDisp_GetPictDispMem(&PictDispMem);
        pBuf = PictDispMem->ThmYuvBuf.BufBase;
        for (i = 0U; i < SVC_PICT_DISP_THM_BUF_NUM; i++) {
            G_ThmFileList[i].Valid = 0;
            G_ThmFileList[i].YuvBuf.pYBase = pBuf;
            pBuf += ThmBufSize;
            G_ThmFileList[i].YuvBuf.pUVBase = pBuf;
            #if (SVCAG_SDEC_DISP_FORMAT == AMBA_DSP_YUV422)
            pBuf += (ULONG)ThmBufSize;
            #else
            pBuf += ((ULONG)ThmBufSize / 2U);
            #endif
        }

        G_ThmDispMgr.DcfScanType   = AMBA_DCF_FILE_TYPE_ALL;
        G_ThmDispMgr.CurFileIndex  = 1U;
        G_ThmDispMgr.PageFirstFile = 1U;
        G_ThmDispMgr.ImgRotate     = AMBA_DSP_ROTATE_0;
    }

    return RetVal;
}

/**
* SvcThmDisp_Delete
* @return ErrorCode
*/
UINT32 SvcThmDisp_Delete(void)
{
    UINT32 RetVal = SVC_OK;

    if (SVC_OK != AmbaKAL_EventFlagDelete(&(G_ThmDispMgr.EventId))) {
        SvcLog_NG(SVC_LOG_THM_DISP, "SvcThmDisp_Delete, failed to EventFlagDelete", 0U, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}

/**
* SvcThmDisp_Handler
* @param [in]  KeyCode
* @return ErrorCode
*/
UINT32 SvcThmDisp_Handler(UINT32 KeyCode)
{
    UINT32                     PageFileNum = 0U, FileAmount = 0U, Err;
    SVC_PBK_CTRL_INFO_s        *pInfo = NULL;
    SVC_STG_MONI_DRIVE_INFO_s  DriveInfo;

    PbkDebugUInt5("SvcThmDisp_Handler start", 0U, 0U, 0U, 0U, 0U);
    SvcPbkCtrl_InfoGet(0U, &pInfo);
    if (SVC_OK == SvcStgMonitor_GetDriveInfo(pInfo->Drive, &DriveInfo)) {
        if (SVC_OK != AmbaDCF_GetFileAmount(DriveInfo.DriveID, 0U, G_ThmDispMgr.DcfScanType, &FileAmount)) {
            SvcLog_NG(SVC_LOG_THM_DISP, "fail to get file amount", 0U, 0U);
        }
    }
    PbkDebugUInt5("SvcThmDisp_Handler , __LINE__ %d, FileAmount %d", \
            __LINE__, FileAmount, 0U, 0U, 0U);

    switch (KeyCode) {
    case SVC_PICT_DISP_KEY_OK:
        Err = SvcPbkPictDisp_SingleViewStart(G_ThmDispMgr.DcfScanType, G_ThmDispMgr.CurFileIndex);
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_THM_DISP, "SvcPbkPictDisp_SingleViewStart failed %u", Err, 0U);
        }
        break;
    case SVC_PICT_DISP_KEY_RIGHT:
        if (G_ThmDispMgr.CurFileIndex == FileAmount) {
            break;
        } else {
            G_ThmDispMgr.CurFileIndex++;
        }

        G_ThmDispMgr.PageFirstFile = PageFstFileIdx(G_ThmDispMgr.CurFileIndex);
        if (((FileAmount - G_ThmDispMgr.PageFirstFile) + 1U) >= THMDISP_PAGE_FILE_NUM) {
            PageFileNum = THMDISP_PAGE_FILE_NUM;
        } else {
            PageFileNum = (FileAmount - G_ThmDispMgr.PageFirstFile) + 1U;
        }

        Err = SvcThmDisp_DisplayPage(G_ThmDispMgr.PageFirstFile, PageFileNum, G_ThmDispMgr.ImgRotate);
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_THM_DISP, "SvcThmDisp_DisplayPage failed %u", Err, 0U);
        }
        break;
    case SVC_PICT_DISP_KEY_LEFT:
        if (G_ThmDispMgr.CurFileIndex == 1U) {
            break;
        } else {
            G_ThmDispMgr.CurFileIndex--;
        }

        G_ThmDispMgr.PageFirstFile = PageFstFileIdx(G_ThmDispMgr.CurFileIndex);
        if (((FileAmount - G_ThmDispMgr.PageFirstFile) + 1U) >= THMDISP_PAGE_FILE_NUM) {
            PageFileNum = THMDISP_PAGE_FILE_NUM;
        } else {
            PageFileNum = (FileAmount - G_ThmDispMgr.PageFirstFile) + 1U;
        }

        PbkDebugUInt5("SvcThmDisp_Handler , __LINE__ %d, PageFileNum %d", __LINE__, PageFileNum, 0U, 0U, 0U);
        Err = SvcThmDisp_DisplayPage(G_ThmDispMgr.PageFirstFile, PageFileNum, G_ThmDispMgr.ImgRotate);
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_THM_DISP, "SvcThmDisp_DisplayPage failed %u", Err, 0U);
        }
        break;
    case SVC_PICT_DISP_KEY_DEL:
        break;
    default:
        // Do nothing
        break;
    }

    PbkDebugUInt5("SvcThmDisp_Handler done", 0U, 0U, 0U, 0U, 0U);

    return SVC_OK;
}

/**
* SvcThmDisp_ThmViewStart
* @param [in]  DcfScanType
* @param [in]  FileIndex
* @return ErrorCode
*/
UINT32 SvcThmDisp_ThmViewStart(AMBA_DCF_FILE_TYPE_e DcfScanType, UINT32 FileIndex)
{
    UINT32 ActualFlags = 0U, RetVal = SVC_OK, Err;

    SvcClock_FeatureCtrl(1U, (SVC_CLK_FEA_BIT_CODEC | SVC_CLK_FEA_BIT_HEVC));

    if (AmbaKAL_EventFlagGet(&SvcPictDispEventId,
                             SVC_PICT_DISP_EVENT_VOUT_IDLE,
                             AMBA_KAL_FLAGS_ANY,
                             AMBA_KAL_FLAGS_CLEAR_AUTO,
                             &ActualFlags,
                             5000U) != SVC_OK) {
        SvcLog_NG(SVC_LOG_THM_DISP, "SvcThmDisp_ThmViewStart: timeout", 0U, 0U);
        RetVal = SVC_NG;
    } else {
        Err = ThmDispThmViewStart(DcfScanType, FileIndex);
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_THM_DISP, "ThmDispThmViewStart failed %u", Err, 0U);
        }
        Err = AmbaKAL_EventFlagSet(&SvcPictDispEventId, SVC_PICT_DISP_EVENT_VOUT_IDLE);
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_THM_DISP, "AmbaKAL_EventFlagSet failed %u", Err, 0U);
        }
    }

    return RetVal;
}

/**
* SvcThmDisp_SetCurFileIndex
* @param [in]  Index
*/
void SvcThmDisp_SetCurFileIndex(UINT32 Index)
{
    G_ThmDispMgr.CurFileIndex = Index;
    G_ThmDispMgr.PageFirstFile = ((Index / THMDISP_PAGE_FILE_NUM) * THMDISP_PAGE_FILE_NUM) + 1U;
}

/**
* SvcThmDisp_GetCurFileIndex
* @param [in]  pIndex Index
*/
void SvcThmDisp_GetCurFileIndex(UINT32 *pIndex)
{
    *pIndex = G_ThmDispMgr.CurFileIndex;
}


