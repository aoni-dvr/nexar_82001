/**
 *  @file AmbaFPD_WDF9648W.h
 *
 *  @copyright Copyright (c) 2018 Ambarella, Inc.
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
 *  @details Definitions & Constants for Wintek FPD panel WD-F9648W APIs.
 *
 */

#ifndef AMBA_FPD_WDF9648W_H
#define AMBA_FPD_WDF9648W_H

#define AMBA_FPD_WDF9648W_8BIT_RGB_THROUGH_MODE     0U  /* 8-bit RGB Through Mode mode */
#define AMBA_FPD_WDF9648W_8BIT_RGB_DA_MODE          1U  /* 8-bit RGB DA Mode mode */
#define AMBA_FPD_WDF9648W_8BIT_RGB_720_MODE         2U  /* 8-bit RGB 720 Mode mode */
#define AMBA_FPD_WDF9648W_8BIT_RGB_640_MODE         3U  /* 8-bit RGB 640 Mode mode */
#define AMBA_FPD_WDF9648W_8BIT_YUV_720_MODE         4U  /* 8-bit YUV 720 Mode mode */
#define AMBA_FPD_WDF9648W_8BIT_YUV_640_MODE         5U  /* 8-bit YUV 640 Mode mode */
#define AMBA_FPD_WDF9648W_16BIT_RGB_THROUGH_MODE    6U  /* 16-bit RGB Through Mode mode */
#define AMBA_FPD_WDF9648W_16BIT_RGB_DA_MODE         7U  /* 16-bit RGB DA Mode mode */
#define AMBA_FPD_WDF9648W_16BIT_RGB_720_MODE        8U  /* 16-bit RGB 720 Mode mode */
#define AMBA_FPD_WDF9648W_16BIT_RGB_640_MODE        9U  /* 16-bit RGB 640 Mode mode */
#define AMBA_FPD_WDF9648W_24BIT_RGB_THROUGH_MODE   10U  /* 24-bit RGB Through Mode mode */
#define AMBA_FPD_WDF9648W_24BIT_RGB_DA_MODE        11U  /* 24-bit RGB DA Mode mode */


/*-----------------------------------------------------------------------------------------------*\
 * Defintions for all the screen modes.
\*-----------------------------------------------------------------------------------------------*/
#define AMBA_FPD_WDF9648W_SCREEN_MODE_WIDE          0U  /* Wide Screen Mode */
#define AMBA_FPD_WDF9648W_SCREEN_MODE_NARROW        1U  /* Narrow Screen Mode */

/*-----------------------------------------------------------------------------------------------*\
 * Defintions for all the control signal types.
\*-----------------------------------------------------------------------------------------------*/
#define AMBA_FPD_WDF9648W_CTRL_DE                   0U  /* Data Enable (DE) signal */
#define AMBA_FPD_WDF9648W_CTRL_HV_SYNC              1U  /* H/V Sync signals */

#define AMBA_FPD_WDF9648W_960_480_60HZ              0U  /* 960x480@59.94Hz */
#define AMBA_FPD_WDF9648W_960_480_50HZ              1U  /* 960x480@50.00Hz */
#define AMBA_FPD_WDF9648W_960_480_A60HZ             2U  /* 960x480@60.00Hz */
#define AMBA_FPD_WDF9648W_NUM_MODE                  3U

/*-----------------------------------------------------------------------------------------------*\
 * FPD DISPLAY CONFIG
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT8                                   ScreenMode;     /* Wide screen mode or Narrow screen mode */
    UINT32                                  OutputMode;     /* Digital Output Mode Register */
    UINT8                                   ColorOrder;     /* Color sequence of output mode */
    AMBA_VOUT_DISPLAY_TIMING_CONFIG_s       DisplayTiming;
    AMBA_VOUT_DATA_LATCH_CONFIG_s           DataLatch;
} AMBA_FPD_WDF9648W_CONFIG_s;


/*-----------------------------------------------------------------------------------------------*\
 * WD-F9648W FPD Panel Management Structure
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32                                  SpiChanNo;      /* SPI channel */
    UINT32                                  SpiSlaveMask;   /* SPI Slave Mask */
    AMBA_SPI_CONFIG_s                       SpiConfig;      /* SPI control interface */
} AMBA_FPD_WDF9648W_CTRL_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaFPD_WDF9648W.c
\*-----------------------------------------------------------------------------------------------*/
extern AMBA_FPD_OBJECT_s AmbaFPD_WdF9648wObj;

#endif /* AMBA_FPD_WDF9648W_H */
