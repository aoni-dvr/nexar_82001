/**
 *  @file AmbaB8_SerdesCalibration.c
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
 *  @details B8 driver APIs
 *
 */

#include "AmbaB8.h"
#include "AmbaB8_Communicate.h"
#include "AmbaB8_GPIO.h"
#include "AmbaB8_I2C.h"
#include "AmbaB8_SPI.h"
#include "AmbaB8_PLL.h"
#include "AmbaB8_Calibration.h"
#include "AmbaB8D_Calibration.h"
#include "AmbaB8_Packer.h"
#include "AmbaB8_Depacker.h"
#include "AmbaB8_PwmEnc.h"
#include "AmbaB8_PwmDec.h"
#include "AmbaB8_Scratchpad.h"

/* Eq parameters (result of global search) */
typedef struct {
    volatile void *pReg;
    UINT32 Addr;    /*to be remove */
    UINT32 Data;
} B8D_SERDES_REGISTER_s;

typedef struct {
    B8D_SERDES_REGISTER_s *pReg;
    UINT32 Value;
    UINT32 StartBit;
    UINT32 BitMask;
} B8D_SERDES_FIELD_s;

typedef struct {
    B8D_SERDES_REGISTER_s ctle;
    B8D_SERDES_REGISTER_s dfe;
} B8D_SERDES_REGISTER_TAB_s;

typedef struct {
    B8D_SERDES_FIELD_s Cap0;        /* CTLE 0x5268[3:0] */
    B8D_SERDES_FIELD_s Cap1;        /* CTLE 0x5268[7:4] */
    B8D_SERDES_FIELD_s Cap2;        /* CTLE 0x5268[11:8] */
    B8D_SERDES_FIELD_s Res0;        /* CTLE 0x5268[13:12] */
    B8D_SERDES_FIELD_s Res1;        /* CTLE 0x5268[15:14] */
    B8D_SERDES_FIELD_s Res2;        /* CTLE 0x5268[17:16] */
    B8D_SERDES_FIELD_s Tap1;        /* DFE 0x525c[4:0] */
    B8D_SERDES_FIELD_s Tap2;        /* DFE 0x525c[11:8] */
    B8D_SERDES_FIELD_s Tap3;        /* DFE 0x525c[18:16] */
    B8D_SERDES_FIELD_s Tap4;        /* DFE 0x525c[22:20] */
} B8D_SERDES_CAL_FIELD_TAB_s;

static B8D_SERDES_REGISTER_TAB_s B8dParamRegTable = {
    .ctle = {
        //term_res[31:27] = 0
        //ibias_ctle[26:24] = 3
        //sslms_ctle_rgain[22:20] = 1
        //sslms_ctle_manual[18] = 0
        //res[17:12] = 0x3f
        .Addr = 0x5268U,
        .Data = 0x0317f000U,
    },
    .dfe = {
        //vocmdfebuf_in[31:27] = 0b'10011
        //sslms_dfe_gain[26:24] = 1
        //sslms_dfe_manual[13] = 0
        .Addr = 0x525cU,
        .Data = 0x99002000U,
    },
};


static void Serdes_UpdatePhySetting(UINT32 ChipID, const GLOBAL_SEARCH_PARAM_s *pSerdesPhyConfig)
{
    static B8D_SERDES_CAL_FIELD_TAB_s B8dCalFieldTable = {
        .Cap0 = {
            .pReg = &B8dParamRegTable.ctle,
            .Value = 0,
            .StartBit = 0,
            .BitMask = 0xf,
        },
        .Cap1 = {
            .pReg = &B8dParamRegTable.ctle,
            .Value = 0,
            .StartBit = 4,
            .BitMask = 0xf,
        },
        .Cap2 = {
            .pReg = &B8dParamRegTable.ctle,
            .Value = 0,
            .StartBit = 8,
            .BitMask = 0xf,
        },
        .Res0 = {
            .pReg = &B8dParamRegTable.ctle,
            .Value = 0x3,
            .StartBit = 12,
            .BitMask = 0x3,
        },
        .Res1 = {
            .pReg = &B8dParamRegTable.ctle,
            .Value = 0x3,
            .StartBit = 14,
            .BitMask = 0x3,
        },
        .Res2 = {
            .pReg = &B8dParamRegTable.ctle,
            .Value = 0x3,
            .StartBit = 16,
            .BitMask = 0x3,
        },
        .Tap1 = {
            .pReg = &B8dParamRegTable.dfe,
            .Value = 0,
            .StartBit = 0,
            .BitMask = 0x1f,
        },
        .Tap2 = {
            .pReg = &B8dParamRegTable.dfe,
            .Value = 0,
            .StartBit = 8,
            .BitMask = 0xf,
        },
        .Tap3 = {
            .pReg = &B8dParamRegTable.dfe,
            .Value = 0,
            .StartBit = 16,
            .BitMask = 0x7,
        },
        .Tap4 = {
            .pReg = &B8dParamRegTable.dfe,
            .Value = 0,
            .StartBit = 20,
            .BitMask = 0x7,
        },

    };

    AmbaB8_Misra_TouchUnused(&ChipID);
    B8dCalFieldTable.Cap0.Value = ((pSerdesPhyConfig->Cap) & B8dCalFieldTable.Cap0.BitMask);
    B8dCalFieldTable.Cap0.pReg->Data &= (~(B8dCalFieldTable.Cap0.BitMask << B8dCalFieldTable.Cap0.StartBit));
    B8dCalFieldTable.Cap0.pReg->Data |= ((B8dCalFieldTable.Cap0.Value & B8dCalFieldTable.Cap0.BitMask) << B8dCalFieldTable.Cap0.StartBit);

    B8dCalFieldTable.Cap1.Value = ((pSerdesPhyConfig->Cap >> 4U) & B8dCalFieldTable.Cap1.BitMask);
    B8dCalFieldTable.Cap1.pReg->Data &= (~(B8dCalFieldTable.Cap1.BitMask << B8dCalFieldTable.Cap1.StartBit));
    B8dCalFieldTable.Cap1.pReg->Data |= ((B8dCalFieldTable.Cap1.Value & B8dCalFieldTable.Cap1.BitMask) << B8dCalFieldTable.Cap1.StartBit);

    B8dCalFieldTable.Cap2.Value = ((pSerdesPhyConfig->Cap >> 8U) & B8dCalFieldTable.Cap2.BitMask);
    B8dCalFieldTable.Cap2.pReg->Data &= (~(B8dCalFieldTable.Cap2.BitMask << B8dCalFieldTable.Cap2.StartBit));
    B8dCalFieldTable.Cap2.pReg->Data |= ((B8dCalFieldTable.Cap2.Value & B8dCalFieldTable.Cap2.BitMask) << B8dCalFieldTable.Cap2.StartBit);


    B8dCalFieldTable.Res0.Value = ((pSerdesPhyConfig->Res) & B8dCalFieldTable.Res0.BitMask);
    B8dCalFieldTable.Res0.pReg->Data &= (~(B8dCalFieldTable.Res0.BitMask << B8dCalFieldTable.Res0.StartBit));
    B8dCalFieldTable.Res0.pReg->Data |= ((B8dCalFieldTable.Res0.Value & B8dCalFieldTable.Res0.BitMask) << B8dCalFieldTable.Res0.StartBit);

    B8dCalFieldTable.Res1.Value = ((pSerdesPhyConfig->Res >> 2U) & B8dCalFieldTable.Res1.BitMask);
    B8dCalFieldTable.Res1.pReg->Data &= (~(B8dCalFieldTable.Res1.BitMask << B8dCalFieldTable.Res1.StartBit));
    B8dCalFieldTable.Res1.pReg->Data |= ((B8dCalFieldTable.Res1.Value & B8dCalFieldTable.Res1.BitMask) << B8dCalFieldTable.Res1.StartBit);

    B8dCalFieldTable.Res2.Value = ((pSerdesPhyConfig->Res >> 4U) & B8dCalFieldTable.Res2.BitMask);
    B8dCalFieldTable.Res2.pReg->Data &= (~(B8dCalFieldTable.Res2.BitMask << B8dCalFieldTable.Res2.StartBit));
    B8dCalFieldTable.Res2.pReg->Data |= ((B8dCalFieldTable.Res2.Value & B8dCalFieldTable.Res2.BitMask) << B8dCalFieldTable.Res2.StartBit);

    B8dCalFieldTable.Tap1.Value = (pSerdesPhyConfig->Tap1 & B8dCalFieldTable.Tap1.BitMask);
    B8dCalFieldTable.Tap1.pReg->Data &= (~(B8dCalFieldTable.Tap1.BitMask << B8dCalFieldTable.Tap1.StartBit));
    B8dCalFieldTable.Tap1.pReg->Data |= ((B8dCalFieldTable.Tap1.Value & B8dCalFieldTable.Tap1.BitMask) << B8dCalFieldTable.Tap1.StartBit);

    B8dCalFieldTable.Tap2.Value = (pSerdesPhyConfig->Tap2 & B8dCalFieldTable.Tap2.BitMask);
    B8dCalFieldTable.Tap2.pReg->Data &= (~(B8dCalFieldTable.Tap2.BitMask << B8dCalFieldTable.Tap2.StartBit));
    B8dCalFieldTable.Tap2.pReg->Data |= ((B8dCalFieldTable.Tap2.Value & B8dCalFieldTable.Tap2.BitMask) << B8dCalFieldTable.Tap2.StartBit);

    B8dCalFieldTable.Tap3.Value = (pSerdesPhyConfig->Tap3 & B8dCalFieldTable.Tap3.BitMask);
    B8dCalFieldTable.Tap3.pReg->Data &= (~(B8dCalFieldTable.Tap3.BitMask << B8dCalFieldTable.Tap3.StartBit));
    B8dCalFieldTable.Tap3.pReg->Data |= ((B8dCalFieldTable.Tap3.Value & B8dCalFieldTable.Tap3.BitMask) << B8dCalFieldTable.Tap3.StartBit);

    B8dCalFieldTable.Tap4.Value = (pSerdesPhyConfig->Tap4 & B8dCalFieldTable.Tap4.BitMask);
    B8dCalFieldTable.Tap4.pReg->Data &= (~(B8dCalFieldTable.Tap4.BitMask << B8dCalFieldTable.Tap4.StartBit));
    B8dCalFieldTable.Tap4.pReg->Data |= ((B8dCalFieldTable.Tap4.Value & B8dCalFieldTable.Tap4.BitMask) << B8dCalFieldTable.Tap4.StartBit);
#if 0
    B8dParamRegTable.ctle.Data &= (~(0x0003ffff));
    B8dParamRegTable.ctle.Data |= (pSerdesPhyConfig->Cap & 0xfff) << 0;    /* CTLE 0x5268[11:0] */
    B8dParamRegTable.ctle.Data |= (pSerdesPhyConfig->Res & 0x03f) << 12;   /* CTLE 0x5268[17:12] */

    B8dParamRegTable.dfe.Data  &= (~(0x00770f1f));
    B8dParamRegTable.dfe.Data  |= (pSerdesPhyConfig->Tap1 & 0x01f) << 0;   /* DFE 0x525c[4:0] */
    B8dParamRegTable.dfe.Data  |= (pSerdesPhyConfig->Tap2 & 0x00f) << 8;   /* DFE 0x525c[11:8] */
    B8dParamRegTable.dfe.Data  |= (pSerdesPhyConfig->Tap3 & 0x007) << 16;  /* DFE 0x525c[18:16] */
    B8dParamRegTable.dfe.Data  |= (pSerdesPhyConfig->Tap4 & 0x007) << 20;  /* DFE 0x525c[22:20] */
#endif
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Serdes_ConfigRxPhySetting
 *
 *  @Description:: Configure SERDES phy setting
 *
 *  @Input      ::
 *      ChipID:           B8 chip id
 *      pSerdesPhyConfig: Pointer to SERDES phy configuration
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
void Serdes_ConfigRxPhySetting(UINT32 ChipID)
{
#if 1
    (void) AmbaB8_RegWriteU32(ChipID, B8dParamRegTable.ctle.Addr, 0, B8_DATA_WIDTH_32BIT, 1, &B8dParamRegTable.ctle.Data);
    (void) AmbaB8_KAL_TaskSleep(10);
    (void) AmbaB8_RegWriteU32(ChipID, B8dParamRegTable.dfe.Addr, 0, B8_DATA_WIDTH_32BIT, 1, &B8dParamRegTable.dfe.Data);
    (void) AmbaB8_KAL_TaskSleep(10);

    AmbaB8_PrintUInt5("B8D 0x5268 = 0x%08x (set ctle)", B8dParamRegTable.ctle.Data, 0U, 0U, 0U, 0U);
    AmbaB8_PrintUInt5("B8D 0x525c = 0x%08x (set dfe)", B8dParamRegTable.dfe.Data, 0U, 0U, 0U, 0U);
#else
    //0x5268
    //[11:0]:   cap
    //[17:12]:  res
    //[18]:     ctle_manual = 1
    UINT32 Reg_ctle = 0x03140000;
    //0x525c
    //[13]:     sslms_dfe_manual=1
    //[14]:     dfe_pauset=0
    //[26:24]:  sslms_dfe_gain_rx
    UINT32 Reg_dfe  = 0x99002000U;

    Reg_ctle |= (pSerdesPhyConfig->Cap & 0xfff);
    Reg_ctle |= ((pSerdesPhyConfig->Res & 0x3f) << 12);

    Reg_dfe |= (pSerdesPhyConfig->Tap1 & 0x1f);
    Reg_dfe |= ((pSerdesPhyConfig->Tap2 & 0xf) << 8);
    Reg_dfe |= ((pSerdesPhyConfig->Tap3 & 0x7) << 16);
    Reg_dfe |= ((pSerdesPhyConfig->Tap4 & 0x7) << 20);

    (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5268, 0, B8_DATA_WIDTH_32BIT, 1, &Reg_ctle);
    (void) AmbaB8_KAL_TaskSleep(10);
    (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x525c, 0, B8_DATA_WIDTH_32BIT, 1, &Reg_dfe);
    (void) AmbaB8_KAL_TaskSleep(10);

    AmbaPrintColor(RED, "0x5268 = 0x%08x (set ctle)", Reg_ctle);
    AmbaPrintColor(RED, "0x525c = 0x%08x (set dfe)", Reg_dfe);
#endif
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_SerdesSetGlobalCalibParam
 *
 *  @Description:: Set SERDES calibration parameters
 *
 *  @Input      ::
 *      ChipID:       B8 chip id
 *      pCalibParam:  Pointer to calibration data
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8D_SerdesSetGlobalCalParam(UINT32 ChipID, const GLOBAL_SEARCH_PARAM_s *pCalibParam)
{
    UINT32 RetVal = B8_ERR_NONE;

    if (pCalibParam == NULL) {
        RetVal = B8_ERR_ARG;
    } else {

        if ((ChipID & B8_SUB_CHIP_ID_MASK) != B8_SUB_CHIP_ID_B8N) {
            AmbaB8_PrintUInt5("B8D_SerdesSetGlobalCalibParam", 0U, 0U, 0U, 0U, 0U);
            AmbaB8_PrintUInt5("Cap:  0x%x", pCalibParam->Cap, 0U, 0U, 0U, 0U);
            AmbaB8_PrintUInt5("Res:  0x%x", pCalibParam->Res, 0U, 0U, 0U, 0U);
            AmbaB8_PrintUInt5("Tap1: 0x%x", pCalibParam->Tap1, 0U, 0U, 0U, 0U);
            AmbaB8_PrintUInt5("Tap2: 0x%x", pCalibParam->Tap2, 0U, 0U, 0U, 0U);
            AmbaB8_PrintUInt5("Tap3: 0x%x", pCalibParam->Tap3, 0U, 0U, 0U, 0U);
            AmbaB8_PrintUInt5("Tap4: 0x%x", pCalibParam->Tap4, 0U, 0U, 0U, 0U);
            Serdes_UpdatePhySetting(ChipID, pCalibParam);
        }
    }

    return RetVal;
}

