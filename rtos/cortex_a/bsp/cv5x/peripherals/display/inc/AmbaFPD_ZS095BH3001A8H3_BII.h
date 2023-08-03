/**
 *  @file AmbaFPD_ZS095BH3001A8H3_BII.h
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
 *  @details Definitions & Constants for Shenzhen Zhongshen Optoelectronics Co. Ltd FPD ZS095BH3001A8H3-BII APIs.
 *
 */

#ifndef AMBA_FPD_ZS095BH3001A8H3_H
#define AMBA_FPD_ZS095BH3001A8H3_H

/*-----------------------------------------------------------------------------------------------*\
 * Defintions for all the control signal types.
\*-----------------------------------------------------------------------------------------------*/
#define AMBA_FPD_ZS095BH_1920_480_60HZ            0U      /* 1920x480@59.94Hz */
#define AMBA_FPD_ZS095BH_1920_480_A60HZ           1U      /* 1920x480@60.00Hz */
#define AMBA_FPD_ZS095BH_NUM_MODE                 2U

/*-----------------------------------------------------------------------------------------------*\
 * FPD DISPLAY CONFIG
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32                                  OutputMode;
    UINT8                                   BitOrder;
    UINT8                                   DataValidPolarity;
    AMBA_VOUT_DISPLAY_TIMING_CONFIG_s       DisplayTiming;
} AMBA_FPD_ZS095BH3001A8H3_CONFIG_s;

/*-----------------------------------------------------------------------------------------------*\
 * ZS095BH3001A8H3 FPD Panel Management Structure
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32                                  SpiChanNo;      /* SPI channel */
    UINT32                                  SpiSlaveMask;   /* SPI Slave Mask */
    AMBA_SPI_CONFIG_s                       SpiConfig;      /* SPI control interface */
} AMBA_FPD_ZS095BH3001A8H3_CTRL_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaFPD_ZS095BH3001A8H3.c
\*-----------------------------------------------------------------------------------------------*/
extern AMBA_FPD_OBJECT_s AmbaFPD_ZS095BH3001A8H3Obj;

#endif /* AMBA_FPD_ZS095BH3001A8H3_H */
