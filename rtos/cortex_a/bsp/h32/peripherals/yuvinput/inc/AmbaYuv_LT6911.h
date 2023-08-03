/**
 *  @file AmbaYuv_LT6911.h
 *
 *  @copyright Copyright (c) 2015 Ambarella, Inc.
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
 *  @details Control APIs of OmniVision LT6911 CMOS sensor with MIPI interface
 *
 */

#ifndef AMBA_YUV_LT6911_H
#define AMBA_YUV_LT6911_H

/* i2c slave addr */
#define LT6911_I2C_SLAVE_ADDR   0x56U

/* Mode ID */
#define LT6911_3840_2160_30P    0U
#define LT6911_1920_1080_60P    1U
#define LT6911_1920_1080_30P    2U
#define LT6911_1280_720_60P     3U
#define LT6911_1280_720_30P     4U
#define LT6911_NUM_YUV_MODE     5U

/* Register */
#define LT6911_NUM_AUDIO_I2S_REG        26U
#define LT6911_NUM_RX_PHY_REG           8U
#define LT6911_NUM_TX_PHY_REG           17U
#define LT6911_NUM_RX_PLL_ENABLE_REG    5U
#define LT6911_NUM_TX_PORT_REG          13U
#define LT6911_NUM_EDID_REG             256U

typedef struct {
    UINT16  Addr;
    UINT8   Data;
} LT6911_REG_s;

typedef struct {
    UINT32 MipiClock;
    UINT8  NumDataLanes;                  /* active data channels */
    AMBA_YUV_OUTPUT_INFO_s OutputInfo;
} LT6911_MODE_INFO_s;

typedef struct {
    UINT16 ActiveH;
    UINT16 ActiveV;
    UINT16 TotalH;
    UINT16 TotalV;
    UINT32 PixelClk;
} LT6911_FORMAT;

typedef struct {
    UINT16  Hfp;
    UINT16  Hs;
    UINT16  Hbp;
    UINT16  Hact;
    UINT16  Htotal;
    UINT16  Vfp;
    UINT16  Vs;
    UINT16  Vbp;
    UINT16  Vact;
    UINT16  Vtotal;
} LT6911_MODE_TIMING_s;

/*-----------------------------------------------------------------------------------------------*\
 *  Defined in AmbaSensor_LT6911.c
\*-----------------------------------------------------------------------------------------------*/
extern const LT6911_REG_s AudioI2sRegTable[LT6911_NUM_AUDIO_I2S_REG];
extern UINT8 EdidRegTable[LT6911_NUM_YUV_MODE][LT6911_NUM_EDID_REG];
extern const LT6911_REG_s RxPhyRegTable[LT6911_NUM_RX_PHY_REG];
extern const LT6911_REG_s RxPllEnableRegTable[LT6911_NUM_RX_PLL_ENABLE_REG];
extern const LT6911_REG_s TxPhyRegTable[LT6911_NUM_TX_PHY_REG];
extern const LT6911_REG_s TxPortRegTable[LT6911_NUM_TX_PORT_REG];
extern const LT6911_MODE_TIMING_s LT6911ModeTiming[LT6911_NUM_YUV_MODE];
extern const LT6911_MODE_INFO_s LT6911ModeInfo[LT6911_NUM_YUV_MODE];
extern AMBA_YUV_OBJ_s AmbaYuv_LT6911Obj;

#endif /* AMBA_YUV_LT6911_H */

