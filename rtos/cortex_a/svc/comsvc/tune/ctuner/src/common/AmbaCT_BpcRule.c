/**
 *  @file AmbaCT_BpcRule.c
 *
 * Copyright (c) 2020 Ambarella International LP
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
 */
#include "AmbaCT_Parser.h"
#include "AmbaCT_ProcRule.h"
#include "AmbaCT_BpcRule.h"
#include "AmbaCT_BpcTuner.h"
#include "AmbaTypes.h"

#define SYSTEM_PARAMS_COUNT (3U)
static PARAM_s CT_BpcSystemParams[SYSTEM_PARAMS_COUNT] = {
    {0, "calib_mode", STRING, 1, NULL, 0},
    {1, "chip_rev", STRING, 1, NULL, 0},
    {2, "output_prefix", STRING, 1, NULL, 0},
};

#define RAW_INFO_PARAMS_COUNT (11U)
static PARAM_s CT_BpcRawInfoParams[RAW_INFO_PARAMS_COUNT] = {
    {0, "start_x", U_32, 1, NULL, 0},
    {1, "start_y", U_32, 1, NULL, 0},
    {2, "width", U_32, 1, NULL, 0},
    {3, "height", U_32, 1, NULL, 0},
    {4, "pitch", U_32, 1, NULL, 0},
    {5, "from_file", U_8, 1, NULL, 0},
    {6, "path", STRING, 1, NULL, 0},
    {7, "caliboffsetx", U_32, 1, NULL, 0},
    {8, "bayer", U_8, 1, NULL, 0},
    {9, "sensortype", U_32, 1, NULL, 0},
    {10, "irmode", U_32, 1, NULL, 0},
};

#define VIN_PARAMS_COUNT (8U)
static PARAM_s CT_BpcVinParams[VIN_PARAMS_COUNT] = {
    {0, "start_x", U_32, 1, NULL, 0},
    {1, "start_y", U_32, 1, NULL, 0},
    {2, "width", U_32, 1, NULL, 0},
    {3, "height", U_32, 1, NULL, 0},
    {4, "h_subsample_factor_num", U_8, 1, NULL, 0},
    {5, "h_subsample_factor_den", U_8, 1, NULL, 0},
    {6, "v_subsample_factor_num", U_8, 1, NULL, 0},
    {7, "v_subsample_factor_den", U_8, 1, NULL, 0},
};

#define SBP_PARAMS_COUNT (6U)
static PARAM_s CT_BpcParams[SBP_PARAMS_COUNT] = {
    {0, "algorithm", U_8, 1, NULL, 0},
    {1, "threshold_mode", U_8, 1, NULL, 0},
    {2, "upper_threshold", S_DB, 1, NULL, 0},
    {3, "lower_threshold", S_DB, 1, NULL, 0},
    {4, "block_width", U_32, 1, NULL, 0},
    {5, "block_height", U_32, 1, NULL, 0},
};

#define OB_PARAMS_COUNT (6U)
static PARAM_s CT_BpcObParams[OB_PARAMS_COUNT] = {
    {0, "enable", U_32, 1, NULL, 0},
    {1, "width", U_32, 1, NULL, 0},
    {2, "height", U_32, 1, NULL, 0},
    {3, "offset_x", U_32, 1, NULL, 0},
    {4, "offset_y", U_32, 1, NULL, 0},
    {5, "pitch", U_32, 1, NULL, 0},
};

static void CT_BpcSystemProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[SYSTEM_PARAMS_COUNT];
    AMBA_CT_BPC_TUNER_SYSTEM_s Data;
    AmbaCT_BpcGetSystemInfo(&Data);
    pField[0] = Data.CalibMode;
    pField[1] = Data.ChipRev;
    pField[2] = Data.OutputPrefix;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_BpcSetSystemInfo(&Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
    }
}

static void CT_BpcRawInfoProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    AMBA_CT_BPC_TUNER_RAW_INFO_s Data;
    void *pField[RAW_INFO_PARAMS_COUNT];
    AmbaCT_BpcGetRawInfo(&Data);
    pField[0] = &Data.Area.StartX;
    pField[1] = &Data.Area.StartY;
    pField[2] = &Data.Area.Width;
    pField[3] = &Data.Area.Height;
    pField[4] = &Data.Pitch;
    pField[5] = &Data.FromFile;
    pField[6] = Data.Path;
    pField[7] = &Data.CalibOffsetX;
    pField[8] = &Data.Bayer;
    pField[9] = &Data.SensorType;
    pField[10] = &Data.IrMode;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_BpcSetRawInfo(&Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
    }
}
static void CT_BpcVinProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[VIN_PARAMS_COUNT];
    AMBA_CAL_VIN_SENSOR_GEOMETRY_s Data;
    AmbaCT_BpcGetVin(&Data);
    pField[0] = &Data.StartX;
    pField[1] = &Data.StartY;
    pField[2] = &Data.Width;
    pField[3] = &Data.Height;
    pField[4] = &Data.HSubSample.FactorNum;
    pField[5] = &Data.HSubSample.FactorDen;
    pField[6] = &Data.VSubSample.FactorNum;
    pField[7] = &Data.VSubSample.FactorDen;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_BpcSetVin(&Data);
    if (pParamStatus != NULL) {
       *pParamStatus |= (0x1ULL << (UINT32)Index);
    }

}

static void CT_BpcProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[SBP_PARAMS_COUNT];
    AMBA_CT_BPC_TUNER_BPC_s Data;
    AmbaCT_BpcGetBpc(&Data);
    pField[0] = &Data.Algo;
    pField[1] = &Data.ThMode;
    pField[2] = &Data.UpperTh;
    pField[3] = &Data.LowerTh;
    pField[4] = &Data.BlockWidth;
    pField[5] = &Data.BlockHeight;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_BpcSetBpc(&Data);
    if (pParamStatus != NULL) {
       *pParamStatus |= (0x1ULL << (UINT32)Index);
    }
}

static void CT_BpcObProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[OB_PARAMS_COUNT];
    AMBA_CAL_BPC_OB_INFO_s Data;
    AmbaCT_BpcGetOb(&Data);
    pField[0] = &Data.OBEnable;
    pField[1] = &Data.OBWidth;
    pField[2] = &Data.OBHeight;
    pField[3] = &Data.OBOffsetX;
    pField[4] = &Data.OBOffsetY;
    pField[5] = &Data.OBPitch;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_BpcSetOb(&Data);
    if (pParamStatus != NULL) {
       *pParamStatus |= (0x1ULL << (UINT32)Index);
       if ((Index == 0) && (Data.OBEnable == 0U)) {
           *pParamStatus |= (0x1ULL << 1U);
           *pParamStatus |= (0x1ULL << 2U);
           *pParamStatus |= (0x1ULL << 3U);
           *pParamStatus |= (0x1ULL << 4U);
           *pParamStatus |= (0x1ULL << 5U);
       }
    }
}

#define BPC_REGS_COUNT (5U)
static REG_s BpcRegs[BPC_REGS_COUNT] = {
    {(INT32)AMBA_CT_BPC_TUNER_SYSTEM_INFO, "system", 1, 0, (INT32) SYSTEM_PARAMS_COUNT, PARAM_STATUS_DEFAULT, CT_BpcSystemParams, &CT_BpcSystemProc, 0},
    {(INT32)AMBA_CT_BPC_TUNER_RAW_INFO, "raw", 1, 0, (INT32) RAW_INFO_PARAMS_COUNT, PARAM_STATUS_DEFAULT, CT_BpcRawInfoParams, &CT_BpcRawInfoProc, 0},
    {(INT32)AMBA_CT_BPC_TUNER_VIN, "vin", 1, 0, (INT32) VIN_PARAMS_COUNT, PARAM_STATUS_DEFAULT, CT_BpcVinParams, &CT_BpcVinProc, 0},
    {(INT32)AMBA_CT_BPC_TUNER_BPC, "static_bad_pixel", 1, 0, (INT32) SBP_PARAMS_COUNT, PARAM_STATUS_DEFAULT, CT_BpcParams, &CT_BpcProc, 0},
    {(INT32)AMBA_CT_BPC_TUNER_OB, "ob", 1, 0, (INT32) OB_PARAMS_COUNT, PARAM_STATUS_DEFAULT, CT_BpcObParams, &CT_BpcObProc, 0}
};

UINT32 AmbaCT_GetBpcRuleInfo(Rule_Info_t *RuleInfo)
{
    RuleInfo->RegList = &BpcRegs[0];
    RuleInfo->RegCount = BPC_REGS_COUNT;
    return 0U;
}

UINT32 AmbaCT_GetBpcRuleInfoMisra(Rule_Info_t *RuleInfo);

UINT32 AmbaCT_GetBpcRuleInfoMisra(Rule_Info_t *RuleInfo)
{
    RuleInfo->RegList = &BpcRegs[0];
    RuleInfo->RegCount = BPC_REGS_COUNT;
    return 0U;
}

