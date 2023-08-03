/**
 *  @file AmbaFPD_T30P61.h
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
 *  @details Definitions & Constants for Shenzhen RZW FPD panel T30P61 APIs.
 *
 */

#ifndef AMBA_FPD_T30P61_H
#define AMBA_FPD_T30P61_H

/*-----------------------------------------------------------------------------------------------*\
 * Defintions for all the screen modes.
\*-----------------------------------------------------------------------------------------------*/
#define AMBA_FPD_T30P61_SCREEN_MODE_WIDE        0U  /* Wide Screen Mode */
#define AMBA_FPD_T30P61_SCREEN_MODE_NARROW      1U  /* Narrow Screen Mode */

/*-----------------------------------------------------------------------------------------------*\
 * Defintions for all the control signal types.
\*-----------------------------------------------------------------------------------------------*/
#define AMBA_FPD_T30P61_CTRL_DE                 0U  /* Data Enable (DE) signal */
#define AMBA_FPD_T30P61_CTRL_HV_SYNC            1U  /* H/V Sync signals */

#define AMBA_FPD_T30P61_960_240_60HZ            0U  /* 960x240@59.94Hz */
#define AMBA_FPD_T30P61_960_240_50HZ            1U  /* 960x240@50.00Hz */
#define AMBA_FPD_T30P61_960_240_A60HZ           2U  /* 960x240@60.00Hz */
#define AMBA_FPD_T30P61_NUM_MODE                3U

/*-----------------------------------------------------------------------------------------------*\
 * FPD DISPLAY CONFIG
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT8                                   ScreenMode;     /* Wide screen mode or Narrow screen mode */
    UINT32                                  OutputMode;     /* Digital Output Mode Register */
    UINT8                                   EvenLineColor;  /* Color sequence of even lines */
    UINT8                                   OddLineColor;   /* Color sequence of odd lines */
    AMBA_VOUT_DISPLAY_TIMING_CONFIG_s       DisplayTiming;
    AMBA_VOUT_DATA_LATCH_CONFIG_s           DataLatch;
} AMBA_FPD_T30P61_CONFIG_s;


/*-----------------------------------------------------------------------------------------------*\
 * T30P61 FPD Panel Management Structure
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32                                  SpiChanNo;      /* SPI channel */
    UINT32                                  SpiSlaveMask;   /* SPI Slave Mask */
    AMBA_SPI_CONFIG_s                       SpiConfig;      /* SPI control interface */
} AMBA_FPD_T30P61_CTRL_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaFPD_T30P61.c
\*-----------------------------------------------------------------------------------------------*/
extern AMBA_FPD_OBJECT_s AmbaFPD_T30P61Obj;

#endif /* AMBA_FPD_T30P61_H */
