/**
 *  @file AmbaCVBS_Def.h
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
 *  @details Common Definitions & Constants for CVBS APIs
 *
 */

#ifndef AMBA_CVBS_DEF_H
#define AMBA_CVBS_DEF_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif
#ifndef AMBA_ERROR_CODE_H
#include "AmbaErrorCode.h"
#endif

/* Definitions of CVBS error code */
#define CVBS_ERR_0000           (CVBS_ERR_BASE)              /* Invalid argument */
#define CVBS_ERR_0001           (CVBS_ERR_BASE + 0X1U)
#define CVBS_ERR_0002           (CVBS_ERR_BASE + 0X2U)
#define CVBS_ERR_0003           (CVBS_ERR_BASE + 0X3U)
#define CVBS_ERR_00FF           (CVBS_ERR_BASE + 0XFFU)      /* Unexpected error */

#define CVBS_ERR_NONE           (OK)
#define CVBS_ERR_ARG            CVBS_ERR_0000
#define CVBS_ERR_MUTEX          CVBS_ERR_0001
#define CVBS_ERR_DEV_CTL        CVBS_ERR_0002
#define CVBS_ERR_OPEN_FILE      CVBS_ERR_0003
#define CVBS_ERR_UNEXPECTED     CVBS_ERR_00FF

/* Definitions for VOUT CVBS systems */
#define NUM_CVBS_SYSTEM         (2U)
#define CVBS_VIDEO_NTSC         (0U)   /* real video in NTSC-like signal */
#define CVBS_VIDEO_PAL          (1U)   /* real video in PAL-like signal */
#define CVBS_COLORBAR_NTSC      (16U)  /* color bars in NTSC-like signal */
#define CVBS_COLORBAR_PAL       (17U)  /* color bars in PAL-like signal */
#define CVBS_POWER_DOWN         (18U)  /* turn-off signal output */

/* Legacy definitions */
#define AMBA_NUM_CVBS_SYSTEM    NUM_CVBS_SYSTEM
#define AMBA_CVBS_SYSTEM_NTSC   CVBS_VIDEO_NTSC
#define AMBA_CVBS_SYSTEM_PAL    CVBS_VIDEO_PAL
#define AMBA_CVBS_NTSC_COLORBAR CVBS_COLORBAR_NTSC
#define AMBA_CVBS_PAL_COLORBAR  CVBS_COLORBAR_PAL
#define AMBA_CVBS_POWER_DOWN    CVBS_POWER_DOWN

/*
 * This structure is used to provide information of the current CVBS configuration
 */
typedef struct {
    UINT32  PixelClkFreq;       /* Pixel clock frequency */
    UINT32  VideoWidth;         /* Horizontal display resolution of TV */
    UINT32  VideoHeight;        /* Vertical display resolution of TV */
    UINT32  TimeScale;          /* Time scale */
    UINT32  NumUnitsInTick;     /* Field refresh rate = (TimeScale / NumUnitsInTick) */
    UINT32  ScanType;           /* Image scanning technique */
} AMBA_CVBS_INFO_s;

typedef struct {
    UINT8   ClampLevel;         /* Used to clamp incoming black level to 0 */
    UINT8   BlackLevel;         /* Black level indicator */
    UINT8   BlankLevel;         /* Blank level indicator */
    UINT8   SyncLevel;          /* Sync level indicator */
    UINT16  OutGain;            /* Gain factor of scale unit before DAC output */
    UINT16  OutOffset;          /* Offset factor of scale unit before DAC output */
    UINT16  OutMin;             /* Clamping value before DAC output */
    UINT16  OutMax;             /* Clipping value before DAC output */
} AMBA_CVBS_IRE_CTRL_s;

#endif /* AMBA_CVBS_DEF_H */
