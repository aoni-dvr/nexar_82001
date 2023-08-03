/**
 *  @file CamCtrlOsdCmdDef.h
 *
 *  @copyright Copyright (c) 2017 Ambarella, Inc.
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Header of CamCtrl OSD commands definition (For CV TASK)
 *
 */

#ifndef __CAMCTRL_OSD_CMD_DEF_H__
#define __CAMCTRL_OSD_CMD_DEF_H__

#ifndef uint32_t
typedef unsigned int uint32_t;
#endif

typedef struct _CV_OSD_Enable_s_ {
    uint32_t Msg;
    uint32_t ParamSize;
    uint32_t Channel;
    uint32_t Enable;
} CV_OSD_Enable_s;

typedef struct _CV_OSD_DrawPixel_s_ {
    uint32_t Msg;
    uint32_t ParamSize;
    uint32_t Channel;
    uint32_t X;
    uint32_t Y;
    uint32_t Color;
} CV_OSD_DrawPixel_s;

typedef struct _CV_OSD_DrawLine_s_ {
    uint32_t Msg;
    uint32_t ParamSize;
    uint32_t Channel;
    uint32_t X1;
    uint32_t Y1;
    uint32_t X2;
    uint32_t Y2;
    uint32_t Color;
    uint32_t Thickness;
} CV_OSD_DrawLine_s;

typedef struct _CV_OSD_DrawRect_s_ {
    uint32_t Msg;
    uint32_t ParamSize;
    uint32_t Channel;
    uint32_t X1;
    uint32_t Y1;
    uint32_t X2;
    uint32_t Y2;
    uint32_t Color;
    uint32_t Thickness;
} CV_OSD_DrawRect_s;

typedef struct _CV_OSD_Flush_s_ {
    uint32_t Msg;
    uint32_t ParamSize;
    uint32_t Channel;
} CV_OSD_Flush_s;

typedef struct _CV_OSD_Clear_s_ {
    uint32_t Msg;
    uint32_t ParamSize;
    uint32_t Channel;
} CV_OSD_Clear_s;

typedef struct _CV_OSD_FlushCapSeq_s_ {
    uint32_t Msg;
    uint32_t ParamSize;
    uint32_t Channel;
    uint32_t CapSeq;
} CV_OSD_FlushCapSeq_s;

typedef struct _CV_OSD_SetCharConfig_s_ {
    uint32_t Msg;
    uint32_t ParamSize;
    uint32_t Channel;
    uint32_t AttributeData;
    uint32_t AttributeBits;
    uint32_t FontFaceIdx;
    uint32_t FontPixelWidth;
    uint32_t FontPixelHeight;
} CV_OSD_SetCharConfig_s;

typedef struct _CV_OSD_DrawString_s_ {
    uint32_t Msg;
    uint32_t ParamSize;
    uint32_t Channel;
    uint32_t X;
    uint32_t Y;
    uint32_t Color;
    char String[12];
} CV_OSD_DrawString_s;

typedef struct _CV_OSD_SetExtCLUT_s_ {
    uint32_t Msg;
    uint32_t ParamSize;
    uint32_t Channel;
    uint32_t CLUTAddr;
} CV_OSD_SetExtCLUT_s;

typedef struct _CV_OSD_SetExtBufInfo_s_ {
    uint32_t Msg;
    uint32_t ParamSize;
    uint32_t Channel;
    uint32_t PixelFormat;
    uint32_t BufferPitch;
    uint32_t BufferWidth;
    uint32_t BufferHeight;
    uint32_t BufferSize;
    uint32_t WindowOffsetX;
    uint32_t WindowOffsetY;
    uint32_t WindowWidth;
    uint32_t WindowHeight;
    uint32_t Interlace;
} CV_OSD_SetExtBufInfo_s;

typedef struct _CV_OSD_UpdateExtBuf_s_ {
    uint32_t Msg;
    uint32_t ParamSize;
    uint32_t Channel;
    uint32_t OSDBufAddr;
} CV_OSD_UpdateExtBuf_s;

#endif /* __CAMCTRL_OSD_CMD_DEF_H__ */
