/**
 *  [Engineering Version]
 *  @file AmbaSensor_AR0238_PARALLEL.h
 *
 *  @copyright Copyright (c) 2020 Ambarella, Inc.
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
 *  @details Control APIs of Aptina AR0238_PARALLEL CMOS sensor with HiSPi interface
 *
 */

#ifndef AMBA_SENSOR_AR0238_PARALLEL_H
#define AMBA_SENSOR_AR0238_PARALLEL_H

#define AR0238_PARALLEL_SENSOR_I2C_SLAVE_ADDR       0x30
#define AR0238_PARALLEL_SENSOR_I2C_MAX_SIZE         64U

//#define NA (0xff)

/*-----------------------------------------------------------------------------------------------*\
 * Control registers
\*-----------------------------------------------------------------------------------------------*/
#define AR0238_PARALLEL_NUM_READOUT_MODE_REG        17U
#define AR0238_PARALLEL_NUM_SEQUENCER_LINEAR_REG    183U
#define AR0238_PARALLEL_NUM_DEFAULT_SETTING_REG     14U
#define AR0238_PARALLEL_NUM_SETUP_REG               4U
#define AR0238_PARALLEL_NUM_AGC_STEP                51U  //52

typedef struct {
    UINT32  InputClk;                       /* Sensor side input clock frequency */
    UINT32  Linelengthpck;                  /* XHS period (in input clock cycles) */
    UINT32  FrameLengthLines;               /* horizontal operating period (in number of XHS pulses) */
    AMBA_VIN_FRAME_RATE_s   FrameRate;      /* framerate value of this sensor mode */
} AR0238_PARALLEL_FRAME_TIMING_s;

typedef struct {
    AMBA_SENSOR_STATUS_INFO_s   Status;
    UINT32                      CurrentShtCtrl; /* Shutter setting */
    UINT32                      CurrentAgc;     /* AGC table index */
    UINT32                      CurrentDgc;     /* DGC setting */
} AR0238_PARALLEL_CTRL_s;

//#define AR0238_PARALLEL_1920_1080_30P       0U
//#define AR0238_PARALLEL_NUM_MODE            1U
/*-----------------------------------------------------------------------------------------------*\
 *  Application Mode Definition
\*-----------------------------------------------------------------------------------------------*/
#define AMBA_SENSOR_AR0238_PARALLEL_1920_1080_30P   0U
#define AMBA_SENSOR_AR0238_PARALLEL_NUM_MODE        1U

#define AR0238_PARALLEL_LOW_CONVERSION_GAIN     0U
#define AR0238_PARALLEL_HIGH_CONVERSION_GAIN    1U

typedef struct {
    //AR0238_PARALLEL_READOUT_MODE_e  ReadoutMode;
    AR0238_PARALLEL_FRAME_TIMING_s  FrameTiming;
    FLOAT RowTime;   /* calculated in AR0230_Init */
    /* float PixelRate; */ /* calculated in AR0230_Init */
} AR0238_PARALLEL_MODE_INFO_s;

typedef struct {
    UINT16  Addr;
    UINT16  Data[AMBA_SENSOR_AR0238_PARALLEL_NUM_MODE];
} AR0238_PARALLEL_REG_s;

typedef struct {
    UINT16  Addr;
    UINT16  Data;
} AR0238_PARALLEL_SEQ_REG_s;

typedef struct {
    FLOAT   Factor;
    UINT16  Data;
    UINT32  ConvGain;
} AR0238_PARALLEL_AGC_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 *  Defined in AmbaSensor_AR0238_PARALLEL.c
\*-----------------------------------------------------------------------------------------------*/
extern const AMBA_SENSOR_DEVICE_INFO_s AR0238_PARALLELDeviceInfo;
extern const AMBA_SENSOR_INPUT_INFO_s AR0238_PARALLELInputInfo[AMBA_SENSOR_AR0238_PARALLEL_NUM_MODE];
extern const AMBA_SENSOR_OUTPUT_INFO_s AR0238_PARALLELOutputInfo[AMBA_SENSOR_AR0238_PARALLEL_NUM_MODE];
extern AR0238_PARALLEL_MODE_INFO_s AR0238_PARALLELModeInfoList[AMBA_SENSOR_AR0238_PARALLEL_NUM_MODE];

extern AR0238_PARALLEL_SEQ_REG_s AR0238_PARALLELLinearSeqRegTbl[AR0238_PARALLEL_NUM_SEQUENCER_LINEAR_REG];
extern AR0238_PARALLEL_SEQ_REG_s AR0238_PARALLELDefSetRegTable[AR0238_PARALLEL_NUM_DEFAULT_SETTING_REG];
extern AR0238_PARALLEL_SEQ_REG_s AR0238_PARALLELSetupRegTable[AR0238_PARALLEL_NUM_SETUP_REG];
extern AR0238_PARALLEL_REG_s AR0238_PARALLELModeRegTable[AR0238_PARALLEL_NUM_READOUT_MODE_REG];

extern AR0238_PARALLEL_AGC_REG_s AR0238_PARALLELAgcRegTable[AR0238_PARALLEL_NUM_AGC_STEP];

extern AMBA_SENSOR_OBJ_s AmbaSensor_AR0238_PARALLELObj;
#endif /* AMBA_SENSOR_AR0238_PARALLEL_H */
