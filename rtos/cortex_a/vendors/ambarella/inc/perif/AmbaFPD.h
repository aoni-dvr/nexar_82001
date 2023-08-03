/**
 *  @file AmbaFPD.h
 *
 *  Copyright (c) 2020 Ambarella International LP
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
 *  @details Definitions & Constants for FPD Driver
 *
 */

#ifndef AMBA_FPD_H
#define AMBA_FPD_H

#define AMBA_NUM_FPD_CHANNEL    4U

/*
 * This structure is used to provide necessary configuration for FPD input video timing
 */
typedef struct {
    UINT32  PixelClock;             /* Pixel clock frequency */
    UINT8   PixelRepetition;        /* Pixel repetition factor */

    UINT16  Htotal;                 /* Number of columns per row */
    UINT16  Vtotal;                 /* Number of rows per field */

    UINT16  HsyncColStart;          /* Start column of Hsync pulse */
    UINT16  HsyncColEnd;            /* End column of Hsync pluse */

    UINT16  VsyncColStart;          /* Start column of Vsync pulse */
    UINT16  VsyncColEnd;            /* End column of Vsync pulse */
    UINT16  VsyncRowStart;          /* Start row of Vsync pulse */
    UINT16  VsyncRowEnd;            /* End row of Vsync pulse */

    UINT16  ActiveColStart;         /* Start column of active region */
    UINT16  ActiveColWidth;         /* End column of active region */
    UINT16  ActiveRowStart;         /* Start row of active region */
    UINT16  ActiveRowHeight;        /* End row of active region */
} AMBA_FPD_TIMING_s;

typedef struct {
    UINT8   Y;  /* Low byte is height */
    UINT8   X;  /* High byte is width */
} AMBA_FPD_ASPECT_RATIO_s;


typedef struct {
    UINT8   Interlace;                  /* 1 - Interlace; 0 - Progressive */
    UINT32  TimeScale;                  /* time scale */
    UINT32  NumUnitsInTick;             /* Frames per Second = TimeScale / (NumUnitsInTick * (1 + Interlace)) */
} AMBA_FPD_FRAME_RATE_s;

/*
 * This structure is used to provide information of the current FPD configuration
 */
typedef struct {
    UINT16  Width;          /* Horizontal display resolution of FPD panel */
    UINT16  Height;         /* Vertical display resolution of FPD panel */
    AMBA_FPD_FRAME_RATE_s   FrameRate;      /* Frame rate of the current FPD configuration */
    AMBA_FPD_ASPECT_RATIO_s AspectRatio;    /* Aspect ratio of the FPD panel display */
} AMBA_FPD_INFO_s;

/*
 * FPD Driver Object
 */
typedef struct {
    const char    *pName;          /* Name of the FPD panel */

    UINT32     (*FpdEnable)(void);
    UINT32     (*FpdDisable)(void);
    UINT32     (*FpdGetInfo)(AMBA_FPD_INFO_s *pFpdInfo);
    UINT32     (*FpdGetModeInfo)(UINT8 Mode, AMBA_FPD_INFO_s *pLcdInfo);
    UINT32     (*FpdConfig)(UINT8 Mode);
    UINT32     (*FpdSetBacklight)(UINT32 EnableFlag);
    UINT32     (*FpdGetLinkStatus)(UINT32 *pLinkStatus);
} AMBA_FPD_OBJECT_s;

/*
 * Defined in AmbaFPD.c
 */
UINT32 AmbaFPD_Hook(UINT32 Chan, AMBA_FPD_OBJECT_s *pFpdObj);
UINT32 AmbaFPD_Enable(UINT32 Chan);
UINT32 AmbaFPD_Disable(UINT32 Chan);
UINT32 AmbaFPD_GetInfo(UINT32 Chan, AMBA_FPD_INFO_s *pFpdInfo);
UINT32 AmbaFPD_Config(UINT32 Chan, UINT8 Mode);
UINT32 AmbaFPD_SetBacklight(UINT32 Chan, UINT32 EnableFlag);
UINT32 AmbaFPD_GetSerdesLinkStatus(UINT32 Chan, UINT32 *pLinkStatus);

#endif  /* AMBA_FPD_H */
