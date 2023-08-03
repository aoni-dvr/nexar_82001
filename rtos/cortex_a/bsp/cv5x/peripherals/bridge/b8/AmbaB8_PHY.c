/**
 *  @file AmbaB8_SerdesLink.c
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
#include "AmbaB8_IoMap.h"   //to be removed

#include "AmbaB8_PHY.h"

#include "AmbaB8CSL_PHY.h"
#include "AmbaB8CSL_PLL.h"

#ifdef BUILT_IN_SERDES
#include "AmbaCortexA9.h"
#include "AmbaDSP_VIN.h"
#include "AmbaSERDES.h"
#include "AmbaCSL_RCT.h"
#include "AmbaCSL_VIN.h"
#include "AmbaCSL_SERDES.h"
#include "AmbaCSL_DebugPort.h"
#endif

//#undef  Distance
//#define Distance(x, y)      (((x) < (y)) ? ((UINT32)((y) - (x))) : ((UINT32)((x) - (y))))

/* DCO parameters */
typedef struct {
    UINT32 Frequency;   /* Mbps */
    UINT32 DcoFreq;     /* 0:1152Mbps 1:2304Mbps 2:4608Mbps 3:6912Mbps */
    UINT32 FreqAdjust;  /* adj=8192*[(DcoFreq/fbr)-1] in both 24MHz/72MHz reference clk case */
    UINT32 SlvsecCdr;   /* SLVSEC CDR mode. =0 half_rate_mode, fdco = fdata_rate/2;
                                            =1 full_rate_mode, fdco = fdata_rate; DCO can run around from 2~5GHz */
} B8_MPHY_DCO_PARAM_s;

static const B8_MPHY_DCO_PARAM_s DCOParam[B8_NUM_SERDES_RATE] = {
    [B8_SERDES_RATE_2P3G]  = { .Frequency = 2304U, .DcoFreq = 1U, .FreqAdjust = 0x000U, .SlvsecCdr = 1U,},
    [B8_SERDES_RATE_2P59G] = { .Frequency = 2592U, .DcoFreq = 2U, .FreqAdjust = 0x18e3U, .SlvsecCdr = 1U,},
    [B8_SERDES_RATE_3P02G] = { .Frequency = 3024U, .DcoFreq = 2U, .FreqAdjust = 0x10c3U, .SlvsecCdr = 1U,},
    [B8_SERDES_RATE_3P45G] = { .Frequency = 3456U, .DcoFreq = 2U, .FreqAdjust = 0xaaaU, .SlvsecCdr = 1U,},
    [B8_SERDES_RATE_3P60G] = { .Frequency = 3600U, .DcoFreq = 2U, .FreqAdjust = 0x8f6U, .SlvsecCdr = 1U,},
    [B8_SERDES_RATE_3P74G] = { .Frequency = 3744U, .DcoFreq = 2U, .FreqAdjust = 0x762U, .SlvsecCdr = 1U,},
    [B8_SERDES_RATE_3P88G] = { .Frequency = 3888U, .DcoFreq = 2U, .FreqAdjust = 0x5edU, .SlvsecCdr = 1U,},
    [B8_SERDES_RATE_4P03G] = { .Frequency = 4032U, .DcoFreq = 2U, .FreqAdjust = 0x492U, .SlvsecCdr = 0U,},
    [B8_SERDES_RATE_4P17G] = { .Frequency = 4176U, .DcoFreq = 2U, .FreqAdjust = 0x350U, .SlvsecCdr = 0U,},
    [B8_SERDES_RATE_4P32G] = { .Frequency = 4320U, .DcoFreq = 2U, .FreqAdjust = 0x222U, .SlvsecCdr = 0U,},
    [B8_SERDES_RATE_4P46G] = { .Frequency = 4464U, .DcoFreq = 2U, .FreqAdjust = 0x108U, .SlvsecCdr = 0U,},
    [B8_SERDES_RATE_5P04G] = { .Frequency = 5040U, .DcoFreq = 3U, .FreqAdjust = 0xbe2U, .SlvsecCdr = 0U,},
    [B8_SERDES_RATE_5P47G] = { .Frequency = 5472U, .DcoFreq = 3U, .FreqAdjust = 0x86bU, .SlvsecCdr = 0U,},
    [B8_SERDES_RATE_5P76G] = { .Frequency = 5760U, .DcoFreq = 3U, .FreqAdjust = 0x666U, .SlvsecCdr = 0U,},
    [B8_SERDES_RATE_6P04G] = { .Frequency = 6048U, .DcoFreq = 3U, .FreqAdjust = 0x492U, .SlvsecCdr = 0U,},
};


/* Equalizer parameters */
SERDES_REGISTER_TAB_s ParamRegTable = {
    .ctle = {
        //.pReg = (const)(&pAmbaB8_PhyReg->RctMphyCtrl21Reg),
        .Addr = 0x5268U,
        .Data = 0x0316502CU,
    },
    .dfe = {
        //.pReg = (const)(&pAmbaB8_PhyReg->RctMphyCtrl18Reg),
        .Addr = 0x525cU,
        .Data = 0x99332300U,
    },
};

#ifdef BUILT_IN_SERDES
static SERDES_REGISTER_TAB_s BuiltInParamRegTable = {
    .ctle = {
        .Addr = (AMBA_CORTEX_A9_RCT_BASE_ADDR + 0x718),
        .Data = 0x10001034,//0x10001000,
    },
    .dfe = {
        .Addr = (AMBA_CORTEX_A9_RCT_BASE_ADDR + 0x730),
        .Data = 0x00000211,//0x00000200,
    },
};
#endif

/* Link parameters */
UINT32 LocalSearchEnable = 1U;                  /* 0: global search, 1: local search */
UINT32 TxTermAutoEnable = 0U;
UINT32 RxTermAutoEnable = 1U;

/* LSTx */
UINT32 prelstx = 0x26U;
UINT32 poslstx = 0x10U;
UINT32 SwphyClk = 12000000U;                     /* dafault gclk_swphy = 312Mhz/0x1a = 12Mhz */

/* LSRx */
UINT32 Reg5288LsRx = 0x0a1fff01U;

/* HSTx */
UINT32 HsTxBias[2U] = {0xaaU, 0xffU};

/* HSRx */
/* Device auto calibration selection */
SERDES_AUTO_CAL_SEL_s AutoCalSelect[2U] = {     /* 0: auto cal, 1: manual */
    [0] = {0U, 0U, 1U, 1U, 1U, 1U},             /* global search select */
    [1] = {0U, 0U, 1U, 1U, 0U, 0U},             /* local search select */
};
/* Device auto calibration repeat times */
UINT32 AutoCalibNumbers = 3U;
UINT32 AutoCalibThreshold = 4U;
/* Device auto calibration time */
UINT32 AutoCalTimeInMs[4U] = {10U, 10U, 10U, 20U};
/* Device auto calibration coefficient */
UINT32 LpgainVcm = 0x7;
/* Device auto calibration decision making */
UINT32 AutoCalibSelect = 0x0U;
UINT32 AutoCalibCnt = 0x0U;
/* Device auto calibration: Control Code & Value Range */
UINT32 VcmEqBufferCtrl = 0x10;                  /* 0x526c[4:0]  ; Dec {0:31} : 1s-5'h{0x00 : 0x1f} */
UINT32 VcmCtleCtrl = 0x0d;                      /* 0x526c[12:8] ; Dec {0:31} : 1s-5'h{0x00 : 0x1f} */
UINT32 VosCtleCtrl = 0x0U;
UINT32 VosSlicerDat1Ctrl = 0x0U;
UINT32 VosSlicerDat0Ctrl = 0x0U;
UINT32 VosSlicerEdg1Ctrl = 0x0U;
UINT32 VosSlicerEdg0Ctrl = 0x0U;

/* Vref */
UINT32 VrefVal = 0x20;                  /* eye monitor */
UINT32 VrefManual = 0x0;

static UINT32 GetDistance(INT8 x, INT8 y)
{
    UINT32 RetVal;
    INT32 Distance;

    if (x < y) {
        Distance = (INT32)y - (INT32)x;
    } else {
        Distance = (INT32)x - (INT32)y;
    }

    AmbaMisra_TypeCast32(&RetVal, &Distance);

    return RetVal;
}

void AmbaB8_SerdesHsTxReset(UINT32 ChipID)
{
    UINT32 Data;
    //=======================================
    // Reset HSTX/HSRX PHY and stay in reset
    //=======================================
    Data = 0x09008000;
    (void) AmbaB8_RegWriteU32(ChipID, B8_AHB_BASE_ADDR + 0x5284U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);                                  //rst tx afe
    Data = 0x09008000;
    (void) AmbaB8_RegWriteU32(ChipID, B8_AHB_BASE_ADDR + 0x5284U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);                                  //rst tx dig
    (void) AmbaB8_KAL_TaskSleep(3U);
}
void AmbaB8_SerdesHsTxRelease(UINT32 ChipID)
{
    UINT32 Data;
    Data = 0x09008002;
    (void) AmbaB8_RegWriteU32(ChipID, B8_AHB_BASE_ADDR + 0x5284U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);                                  //release rst tx afe
    Data = 0x09008006;
    (void) AmbaB8_RegWriteU32(ChipID, B8_AHB_BASE_ADDR + 0x5284U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);                                  //release rst tx dig
    (void) AmbaB8_KAL_TaskSleep(3U);
}

void AmbaB8_SerdesHsTxSetup(UINT32 ChipID)
{
    UINT32 Data;
    Data = 0x09008007;
    (void) AmbaB8_RegWriteU32(ChipID, B8_AHB_BASE_ADDR + 0x5284U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);                                  //setup tx = 1
    (void) AmbaB8_KAL_TaskSleep(1U);
    Data = 0x09008006;
    (void) AmbaB8_RegWriteU32(ChipID, B8_AHB_BASE_ADDR + 0x5284U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);                                  //setup tx = 0
    (void) AmbaB8_KAL_TaskSleep(1U);
}

void AmbaB8_SerdesHsTxConfig(UINT32 ChipID)
{
    UINT32 Data;
    //=======================================
    // Configure HSTX PHY
    //=======================================
    Data = (HsTxBias[LocalSearchEnable] << (UINT32)16U) | 0x8100010cU;
    (void) AmbaB8_RegWriteU32(ChipID, B8_AHB_BASE_ADDR + 0x528cU, 0, B8_DATA_WIDTH_32BIT, 1, &Data);                                  // tx_hs_bias = 8'hff
    AmbaB8_PrintUInt5("R528c 0x%08x", Data, 0U, 0U, 0U, 0U);

    Data = 0x09008000U;
    (void) AmbaB8_RegWriteU32(ChipID, B8_AHB_BASE_ADDR + 0x5284U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);                                  //SetReferenceClockTo72MHz
    Data = 0x00001000U;
    (void) AmbaB8_RegWriteU32(ChipID, B8_AHB_BASE_ADDR + 0x5200U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);                                  //Reduce Sync Length, Simulation Only
    Data = 0x00004445U;
    (void) AmbaB8_RegWriteU32(ChipID, B8_AHB_BASE_ADDR + 0x5208U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);                                  //Reduce Sync Length, Simulation Only
}

void AmbaB8_SerdesHsTxReady(UINT32 ChipID)
{
    UINT32 Data;
    Data = (Reg5288LsRx | 0x00000010U);     /* [4]: mphy_afe_ready */
    //Data = 0x0a0c4911;
    (void) AmbaB8_RegWriteU32(ChipID, B8_AHB_BASE_ADDR + 0x5288U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);                                  //Tell TX ready to sendout data @ setup
    AmbaB8_PrintUInt5("R5288 0x%x", Data, 0U, 0U, 0U, 0U);
    (void) AmbaB8_KAL_TaskSleep(1);
}

void AmbaB8_SerdesHsTxPowerDown(UINT32 ChipID)
{
    UINT32 Data;
    Data = 0x8046010dU;
    (void) AmbaB8_RegWriteU32(ChipID, B8_AHB_BASE_ADDR + 0x528cU, 0, B8_DATA_WIDTH_32BIT, 1, &Data);                                  // pdb_ls_rx_txch0 = 0; tx_drv_pd_txch0 = 1
    Data = 0x01008000U;
    (void) AmbaB8_RegWriteU32(ChipID, B8_AHB_BASE_ADDR + 0x5284U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);                                  // mphy_rst_n_txch0 = 0; rstn_tx_afe_txch0 = 0
}

void AmbaB8_SerdesHsTxAutoTerm(UINT32 ChipID)
{
    /* Enable termination auto calibration */
    UINT32 Data;
    UINT32 TxCode;

    if (TxTermAutoEnable != 0U) {
        // select tx and clear start_auto and start_force
        Data = 0x00000000U;
        (void) AmbaB8_RegWriteU32(ChipID, B8_AHB_BASE_ADDR + 0x030U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);

        // start tx auto
        Data = 0x02000000U;
        (void) AmbaB8_RegWriteU32(ChipID, B8_AHB_BASE_ADDR + 0x030U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);
        (void) AmbaB8_KAL_TaskSleep(1);

        // select rx and clear start_auto and start_force
        Data = 0x00200000U;
        (void) AmbaB8_RegWriteU32(ChipID, B8_AHB_BASE_ADDR + 0x030U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);
        (void) AmbaB8_KAL_TaskSleep(1);

        // start rx auto
        Data = 0x02200000U;
        (void) AmbaB8_RegWriteU32(ChipID, B8_AHB_BASE_ADDR + 0x030U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);
        (void) AmbaB8_KAL_TaskSleep(1);

        // read result, bit[25:21] is rx code; bit[20:16] is tx code
        (void) AmbaB8_RegReadU32(ChipID, B8_AHB_BASE_ADDR + 0x034U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);
        TxCode = ((Data >> (UINT32)16U) & (UINT32)0x1fU);
        AmbaB8_PrintUInt5("Term  TxCode 0x%02x", TxCode, 0U, 0U, 0U, 0U);

        // write tx code to 0x290 hs_ctr_txch0 [27:20]
        (void) AmbaB8_RegReadU32(ChipID, B8_AHB_BASE_ADDR + 0x5290U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);
        Data &= (~0x01f00000U);
        Data |= (TxCode << (UINT32)20U);
        (void) AmbaB8_RegWriteU32(ChipID, B8_AHB_BASE_ADDR + 0x5290U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);
        (void) AmbaB8_KAL_TaskSleep(1);
    }
}

void AmbaB8_SerdesHsRxReset(UINT32 ChipID)
{
    UINT32 Data;
    //=======================================
    // Reset HSTX/HSRX PHY and stay in reset
    //=======================================

    if ((ChipID & B8_MAIN_CHIP_ID_BUILT_IN_B8NF) != 0U) {
#ifdef BUILT_IN_SERDES
        _pAmbaRCT_Reg->MipiPhyCtrl29 = 0x01f00000;
        _pAmbaRCT_Reg->MipiPhyCtrl34 = 0x400001f0;
        _pAmbaRCT_Reg->MipiPhyCtrl1.Data = 0x00001001;
#endif
    } else {
        Data = 0x000503e0U;
        (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5218U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);    //rst rx afe
        Data = 0x00060100U;
        (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5214U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);    //rst rx dig
        Data = 0x00060100U;
        (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5214U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);    //rst cdr dig
    }
    (void) AmbaB8_KAL_TaskSleep(3U);
}

void AmbaB8_SerdesHsRxRelease(UINT32 ChipID)
{
    UINT32 Data;

    if ((ChipID & B8_MAIN_CHIP_ID_BUILT_IN_B8NF) != 0U) {
#ifdef BUILT_IN_SERDES
        _pAmbaRCT_Reg->MipiPhyCtrl29 = 0x01ff0000;
        _pAmbaRCT_Reg->MipiPhyCtrl34 = 0x400001ff;
        _pAmbaRCT_Reg->MipiPhyCtrl1.Data = 0x00000001;
#endif
    } else {
        Data = 0x000513e0U;
        (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5218U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);    //release rst rx afe
        Data = 0x00060110U;
        (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5214U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);    //release rst rx dig
        Data = 0x000e0110U;
        (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5214U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);    //release rst cdr dig
    }
    /* Wait T(CDR-LOCK) */
    (void) AmbaB8_KAL_TaskSleep(3U);
}

void AmbaB8_SerdesHsRxSetup(UINT32 ChipID)
{
    UINT32 Data;

    if ((ChipID & B8_MAIN_CHIP_ID_BUILT_IN_B8NF) != 0U) {
#ifdef BUILT_IN_SERDES
        if ((ChipID & B8_SUB_CHIP_ID_B8F0) != 0U) {
            AmbaCSL_IdspSelectSection(AMBA_DBG_PORT_IDSP_VIN);
            DelayCycles(0xfff);
            _pAmbaIDSP_VinGlobalReg->GlobalConfig.Bits.RxPhySetup = 1;
        }
        if ((ChipID & B8_SUB_CHIP_ID_B8F1) != 0U) {
            AmbaCSL_IdspSelectSection(AMBA_DBG_PORT_IDSP_PIP2_VIN);
            DelayCycles(0xfff);
            _pAmbaIDSP_VinGlobalReg->GlobalConfig.Bits.RxPhySetup = 1;
        }
        if ((ChipID & B8_SUB_CHIP_ID_B8F2) != 0U) {
            AmbaCSL_IdspSelectSection(AMBA_DBG_PORT_IDSP_PIP3_VIN);
            DelayCycles(0xfff);
            _pAmbaIDSP_VinGlobalReg->GlobalConfig.Bits.RxPhySetup = 1;
        }
        if ((ChipID & B8_SUB_CHIP_ID_B8F3) != 0U) {
            AmbaCSL_IdspSelectSection(AMBA_DBG_PORT_IDSP_PIP_VIN);
            DelayCycles(0xfff);
            _pAmbaIDSP_VinGlobalReg->GlobalConfig.Bits.RxPhySetup = 1;
        }

        AmbaB8_KAL_TaskSleep(1);

        if ((ChipID & B8_SUB_CHIP_ID_B8F0) != 0U) {
            AmbaCSL_IdspSelectSection(AMBA_DBG_PORT_IDSP_VIN);
            DelayCycles(0xfff);
            _pAmbaIDSP_VinGlobalReg->GlobalConfig.Bits.RxPhySetup = 0;
        }
        if ((ChipID & B8_SUB_CHIP_ID_B8F1) != 0U) {
            AmbaCSL_IdspSelectSection(AMBA_DBG_PORT_IDSP_PIP2_VIN);
            DelayCycles(0xfff);
            _pAmbaIDSP_VinGlobalReg->GlobalConfig.Bits.RxPhySetup = 0;
        }
        if ((ChipID & B8_SUB_CHIP_ID_B8F2) != 0U) {
            AmbaCSL_IdspSelectSection(AMBA_DBG_PORT_IDSP_PIP3_VIN);
            DelayCycles(0xfff);
            _pAmbaIDSP_VinGlobalReg->GlobalConfig.Bits.RxPhySetup = 0;
        }
        if ((ChipID & B8_SUB_CHIP_ID_B8F3) != 0U) {
            AmbaCSL_IdspSelectSection(AMBA_DBG_PORT_IDSP_PIP_VIN);
            DelayCycles(0xfff);
            _pAmbaIDSP_VinGlobalReg->GlobalConfig.Bits.RxPhySetup = 0;
        }
#endif
    } else {
        Data = 0x000e0111U;
        (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5214U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);    //setup rx = 1
        (void) AmbaB8_KAL_TaskSleep(1);
        Data = 0x000e0110U;
        (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5214U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);    //setup rx = 0
    }
}

void AmbaB8_SerdesHsRxConfig(UINT32 ChipID, UINT32 SerDesRate)
{
    UINT32 Data;

    if ((ChipID & B8_MAIN_CHIP_ID_BUILT_IN_B8NF) != 0U) {
#ifdef BUILT_IN_SERDES
        _pAmbaRCT_Reg->MipiPhyCtrl37 = 0x0080000aU;
        _pAmbaRCT_Reg->MipiPhyCtrl3 = 0x0000c220U;
        _pAmbaRCT_Reg->MipiPhyCtrl9 = 0x00020000U;
        _pAmbaRCT_Reg->MipiPhyCtrl2 = 0x8800efffU;
        _pAmbaRCT_Reg->MipiPhyCtrl29 = 0x01f00000U;
        _pAmbaRCT_Reg->MipiPhyCtrl50 = 0x00003133U;
        _pAmbaRCT_Reg->RxMphyPllCtrl3.Data = 0x00028306U;
        _pAmbaRCT_Reg->RxMphyPllCtrl2.Data = 0x3f710000U;

        if (SerDesRate == B8_SERDES_RATE_3P60G) {
            _pAmbaRCT_Reg->RxMphyPllCtrl.Data = 0x18091000U;
            _pAmbaRCT_Reg->RxMphyPllCtrl.Data = 0x18091001U;
        } else if (SerDesRate == B8_SERDES_RATE_3P45G) {
            _pAmbaRCT_Reg->RxMphyPllCtrl.Data = 0x17091000U;
            _pAmbaRCT_Reg->RxMphyPllCtrl.Data = 0x17091001U;
        } else if (SerDesRate == B8_SERDES_RATE_3P02G) {
            _pAmbaRCT_Reg->RxMphyPllCtrl.Data = 0x14091000U;
            _pAmbaRCT_Reg->RxMphyPllCtrl.Data = 0x14091001U;
        } else if (SerDesRate == B8_SERDES_RATE_2P59G) {
            _pAmbaRCT_Reg->RxMphyPllCtrl.Data = 0x11091000U;
            _pAmbaRCT_Reg->RxMphyPllCtrl.Data = 0x11091001U;
        } else if (SerDesRate == B8_SERDES_RATE_2P3G) {
            _pAmbaRCT_Reg->RxMphyPllCtrl.Data = 0x0f091000U;
            _pAmbaRCT_Reg->RxMphyPllCtrl.Data = 0x0f091001U;
        } else {        // default 1152Mbps
            _pAmbaRCT_Reg->RxMphyPllCtrl.Data = 0x07091000U;
            _pAmbaRCT_Reg->RxMphyPllCtrl.Data = 0x07091001U;
        }
        AmbaB8_PrintUInt5("R%08x 0x%08x ", (UINT32)(&(_pAmbaRCT_Reg->RxMphyPllCtrl)), (_pAmbaRCT_Reg->RxMphyPllCtrl.Data), 0U, 0U, 0U);
#endif
    } else {
        //=======================================
        // Configure Equalization Adapation Filter
        //=======================================
        Data = 0x5253b400U;
        (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5248U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);    // set vref polarity
        Data = 0x00334485U;
        (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5244U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);    // set vref range

        Data = (0x20203000U | (VrefVal & 0x7fU));
        (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5260U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);    // set vref init value[6:0]
        Data = (0x20203000U | (VrefVal & 0x7fU) | ((VrefManual & 0x1U) << 15U));
        (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5260U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);    // set sslms_vref_manual_rx = 1'h0, to monitor VrefFilter lock position
        AmbaB8_PrintUInt5("R5260 0x%08x", Data, 0U, 0U, 0U, 0U);

        Data = 0x98002000U;
        (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x525cU, 0, B8_DATA_WIDTH_32BIT, 1, &Data);    // set sslms_dfe_manual = 1'h1
        Data = 0x0307f000U;
        (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5268U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);    // set sslms_ctle_manual = 1'h1

        Data = 0x10103010U;
        (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x526cU, 0, B8_DATA_WIDTH_32BIT, 1, &Data);    // vref lms vref gain = 1'h1
        Data = 0x0317f000U;
        (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5268U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);    // ctle lms res gain = 1'h1
        Data = 0x10113010U;
        (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x526cU, 0, B8_DATA_WIDTH_32BIT, 1, &Data);    // ctle lms cap gain = 1'h1
        Data = 0x99002000U;
        (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x525cU, 0, B8_DATA_WIDTH_32BIT, 1, &Data);    // dfe lms tap gain = 1'h1
        Data = 0x00000070U;
        (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5270U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);    // ctle_func_en_in = 3'h7

        Data = (0x20202F00U | (VrefVal & 0x7fU) | ((VrefManual & 0x1U) << 15U));
        (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5260U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);    //term_res = f [12:8]: term_res_therm_rxch0

        //=======================================
        // Configure HSRX Digitial Controlled Oscillator
        //=======================================
        Data = 0x5253b400U;
        (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5248U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);    //0:data based FA, 1:original FA
        Data = 0x00084280U;
        (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x523cU, 0, B8_DATA_WIDTH_32BIT, 1, &Data);    //filter_mode_rx[19] set to 1'h1 DCO mode
        Data = ((DCOParam[SerDesRate].SlvsecCdr & (UINT32)0x1U) << 30U) | 0xb000efffU;
        (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5238U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);    //slvs_mode_rx[30] set to 1'h1
        Data = 0xe0015051U;
        (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5240U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);    //halt_dco_rx[30] set to 1'h1 halt DCO
        Data = 0xc0015051U;
        (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5240U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);    //power on dco
        Data = 0xc0015051U;
        (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5240U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);    //dco_bypass_rx[31] set to 1'b0 not to sync dctrl
        Data = 0xc0015051U;
        (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5240U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);    //dco_gain_rx[18:16]
        Data = ((DCOParam[SerDesRate].DcoFreq & 0x3U) << 16U) | 0x00200356U;
        (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5258U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);    //dco_lf_des_sel_rx[18] set to 1'h0, des ratio, =0 r=4(1:4), =1 r=10(1:10)
        Data = ((DCOParam[SerDesRate].FreqAdjust & 0xffffU) << 8U) | 0x00000020U;
        (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5254U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);    //ref_clk_sel_dco_rx[5] set to 1'h1, FA refclk select: =0 24MHz;=1 72MHz
        Data = ((DCOParam[SerDesRate].DcoFreq & 0x3U) << 16U) | 0x00200256U;
        (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5258U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);    //freqoffset_thrsh_rx[11:8] set to 4'h3   -- xxx need to be expand to 8 bit as T10
        Data = 0x00048fffU;
        (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5250U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);    //freq_div_sel_rx[15:14] set to 2'h2, =0 2000ppm, =1 1000ppm, =2 500ppm, =3 250ppm
        Data = ((DCOParam[SerDesRate].DcoFreq & 0x3U) << 16U) | 0x00200256U;
        (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5258U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);    //datarate_sel_rx[17:16] set to 2'h1 0:1152Mbps 1:2304Mbps 2:4608Mbps 3:6912Mbps
        Data = ((DCOParam[SerDesRate].FreqAdjust & 0xffffU) << 8U) | 0x00000020U;
        (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5254U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);    //freq_adjust_rx[17:8] set to 10'h0 adj=8192*[(4608/fbr)-1] in both 24MHz/72MHz reference clk case
        Data = 0x00000000U;
        (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5280U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);    //cdr_out_select_rx[17:16] set to 2'h0
        Data = ((DCOParam[SerDesRate].FreqAdjust & 0xffffU) << 8U) | 0x00000020U;
        (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5254U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);    //freq_lock_mode_rx[4] set to 1'h0
        Data = ((DCOParam[SerDesRate].DcoFreq & 0x3U) << 16U) | 0x00200256U;
        (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5258U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);    //force_freq_lock_rx[12] set to 1'h0
        Data = 0x00008fffU;
        (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5250U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);    //dctrl_in_rx[31:18] set to 14'h0
        Data = 0x00028fffU;
        (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5250U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);    //pause_lf_dco_rx[17] set to 1'h1
        Data = ((DCOParam[SerDesRate].DcoFreq & 0x3U) << 16U) | 0x00200252U; //0x00200251;
        (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5258U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);    //div_int_dco_rx[3:0] set to 3'h1 -> 2
        Data = ((DCOParam[SerDesRate].DcoFreq & 0x3U) << 16U) | 0x002002f2U; //0x002002f1
        (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5258U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);    //div_p_dco_rx[6:4] set to 3'hf
        Data = 0xc0015051U;
        (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5240U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);    //en_dsm_rx = 0
        Data = 0xc0015051U;
        (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5240U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);    //en_dither_rx = 0
        Data = 0xb8234533U;//0xb5234533;
        (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x524cU, 0, B8_DATA_WIDTH_32BIT, 1, &Data);    //phased_fr_gain_rx[27:24] set to 4'h5 -> 8
        Data = 0xc0015059U;
        (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5240U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);    //disable auto band shift
        Data = 0xc0015e59U;
        (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5240U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);    //the value here is for function test
        Data = 0xc0015e59U;
        (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5240U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);    //manually set binthm shift
        Data = 0x00084280U;
        (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x523cU, 0, B8_DATA_WIDTH_32BIT, 1, &Data);    //save dctrl into reg array

        Data = 0x80015e59U;
        (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5240U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);    //halt_dco_rx[30] set to 1'h0 release DCO

        //Data = 0x09008000;
        //AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5284, 0, B8_DATA_WIDTH_32BIT, 1, &Data);                                  //SetReferenceClockTo72MHz
        Data = 0x00060100U;
        (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5214U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);    //SetReferenceClockTo72MHz
        Data = 0x000503e0U;
        (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5218U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);    //Set RX ChipID
        Data = 0x000503e0U;
        (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5218U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);    //Set No Deskew
        Data = 0x00001000U;
        (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5200U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);    //Reduce Sync Length, Simulation Only
        Data = 0x5253b002U;
        (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5248U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);    //Reduce Sync Length, Simulation Only
        Data = 0x00004445U;
        (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5208U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);    //Reduce Sync Length, Simulation Only
    }

    /* Wait T(PLL-LOCK) */
    (void) AmbaB8_KAL_TaskSleep(3);
}

void AmbaB8_SerdesHsRxCtleConfig(UINT32 ChipID)
{
#ifdef BUILT_IN_SERDES
    if ((ChipID & B8_MAIN_CHIP_ID_BUILT_IN_B8NF) != 0U) {
        _pAmbaRCT_Reg->MipiPhyCtrl36 = BuiltInParamRegTable.ctle.Data;
        AmbaB8_PrintUInt5("R0718 0x%08x", _pAmbaRCT_Reg->MipiPhyCtrl36, 0U, 0U, 0U, 0U);
    } else
#endif
    {
        AmbaB8CSL_PHYSetCtle((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), ParamRegTable.ctle.Data);
        AmbaB8_PrintUInt5("R5268 0x%08x", ParamRegTable.ctle.Data, 0U, 0U, 0U, 0U);
    }
    (void) AmbaB8_KAL_TaskSleep(1);
}


void AmbaB8_SerdesHsRxDfeConfig(UINT32 ChipID)
{
#ifdef BUILT_IN_SERDES
    if ((ChipID & B8_MAIN_CHIP_ID_BUILT_IN_B8NF) != 0U) {
        _pAmbaRCT_Reg->MipiPhyCtrl42 = BuiltInParamRegTable.dfe.Data;
        AmbaB8_PrintUInt5("R0730 0x%08x", _pAmbaRCT_Reg->MipiPhyCtrl42, 0U, 0U, 0U, 0U);
    } else
#endif
    {
        AmbaB8CSL_PHYSetDfe((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), ParamRegTable.dfe.Data);
        AmbaB8_PrintUInt5("R525c 0x%08x", ParamRegTable.dfe.Data, 0U, 0U, 0U, 0U);
    }
    (void) AmbaB8_KAL_TaskSleep(1);
}

void AmbaB8_SerdesHsRxDCOPause(UINT32 ChipID)
{
    UINT32 Data;
    if ((ChipID & B8_MAIN_CHIP_ID_BUILT_IN_B8NF) != 0U) {

    } else {
        Data = 0x00028fffU;
        (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5250U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);      // pause_lf_dco_rx[17] = 1, pause dco filter before tx data come out
    }
}

void AmbaB8_SerdesHsRxDCORelease(UINT32 ChipID)
{
    UINT32 Data;

    if ((ChipID & B8_MAIN_CHIP_ID_BUILT_IN_B8NF) != 0U) {

    } else {
        //===============================
        // Release HSRX PHY (DCO pause)
        //===============================
        Data = 0x00008fffU;
        (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5250U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);    //release dco filter after tx data come out
    }
    (void) AmbaB8_KAL_TaskSleep(3);
}

void AmbaB8_SerdesHsRxSslmsInit(UINT32 ChipID)
{
    UINT32 Data;

    if ((ChipID & B8_MAIN_CHIP_ID_BUILT_IN_B8NF) != 0U) {

    } else {
        //=============================
        // Monitor VrefFilter lock position.
        // Do sslms vref adaptation if setting sslms_vref_manual_rx = 1'h0 (auto mode)
        //=============================
        Data = 0x10113010U;
        (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x526cU, 0, B8_DATA_WIDTH_32BIT, 1, &Data);    //sslms init
        (void) AmbaB8_KAL_TaskSleep(100);
        Data = 0x10113030U;
        (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x526cU, 0, B8_DATA_WIDTH_32BIT, 1, &Data);    //eq mode = sslms vref adapt
    }
}

void AmbaB8_SerdesHsRxCalibration(UINT32 ChipID, UINT32 SerDesRate)
{
    /* Device auto calibration parameters */
    typedef struct {
        INT8   ValueRange;
        UINT32 ControlCode;
    } SERDES_AUTO_CAL_DATA_s;

    UINT32 Reg0x523c, Reg0x5240, Reg0x5244, Reg0x5248, Reg0x524c, Reg0x5254, Reg0x5258, Reg0x5264, Reg0x526c, Reg0x5270, Reg0x5280;

    /* Calibration select */
    UINT8 EqVcmCal         = AutoCalSelect[LocalSearchEnable].EqVcmCal;         //0
    UINT8 CtleVcmCal       = AutoCalSelect[LocalSearchEnable].CtleVcmCal;       //0->1 manual :  0x526c 0x10112d10 [12:8] 5'hd ctle vcm manual mode input
    UINT8 DfeVcmCal        = AutoCalSelect[LocalSearchEnable].DfeVcmCal;        //1
    UINT8 SlicerVcmCal     = AutoCalSelect[LocalSearchEnable].SlicerVcmCal;     //1
    UINT8 SlicerOffsetCal  = AutoCalSelect[LocalSearchEnable].SlicerOffsetCal;  //0
    UINT8 CtleOffsetCal    = AutoCalSelect[LocalSearchEnable].CtleOffsetCal;    //0

    /* Calibration Read Code */
    UINT8 ReadVcmEqBuf;
    UINT8 ReadVcmCtle;
    UINT8 ReadVosCtleStg1p, ReadVosCtleStg2p, ReadVosCtleStg3p, ReadVosCtleStg1n, ReadVosCtleStg2n, ReadVosCtleStg3n;
    UINT8 ReadVosSliceDat0p, ReadVosSliceDat0n, ReadVosSliceDat1p, ReadVosSliceDat1n;
    UINT8 ReadVosSliceEdg0p, ReadVosSliceEdg0n, ReadVosSliceEdg1p, ReadVosSliceEdg1n;

    UINT32 Data, i, j, k, LoopBreak = 0;
    UINT32 Distance[3], Vote = 0xffU;
    SERDES_AUTO_CAL_DATA_s VosCtle[3];           /* 0x5264[13:8] ; Dec {-31:+31} : 2s 6'h{0x00 : 0x1f} */
    SERDES_AUTO_CAL_DATA_s VosSlicerDat1[3];     /* 0x5264[13:8] ; Dec {-31:+31} : 2s 6'h{0x00 : 0x1f} */
    SERDES_AUTO_CAL_DATA_s VosSlicerDat0[3];     /* 0x5280[21:16]; Dec {-31:+31} : 2s 6'h{0x00 : 0x1f} */
    SERDES_AUTO_CAL_DATA_s VosSlicerEdg1[3];     /* 0x5244[27:22]; Dec {-31:+31} : 2s 6'h{0x00 : 0x1f} */
    SERDES_AUTO_CAL_DATA_s VosSlicerEdg0[3];     /* 0x5280[27:22]; Dec {-31:+31} : 2s 6'h{0x00 : 0x1f} */
    INT32 DataInt32;

    (void) AmbaB8_Wrap_memset(VosCtle, 0, sizeof(VosCtle));
    (void) AmbaB8_Wrap_memset(VosSlicerDat1, 0, sizeof(VosSlicerDat1));
    (void) AmbaB8_Wrap_memset(VosSlicerDat0, 0, sizeof(VosSlicerDat0));
    (void) AmbaB8_Wrap_memset(VosSlicerEdg1, 0, sizeof(VosSlicerEdg1));
    (void) AmbaB8_Wrap_memset(VosSlicerEdg0, 0, sizeof(VosSlicerEdg0));
    (void) AmbaB8_Wrap_memset(Distance, 0, sizeof(Distance));

    if ((ChipID & B8_MAIN_CHIP_ID_BUILT_IN_B8NF) == 0U) {

        AmbaB8_PrintStr5("==Start Calibration", NULL, NULL, NULL, NULL, NULL);
        // --- Start Calibration
        AmbaB8CSL_PHYPowerDownLSTx(ChipID & B8_MAIN_CHIP_ID_B8NF_MASK);
        (void) AmbaB8_KAL_TaskSleep(3);

        for (i = 0; i < AutoCalibNumbers; i++) {
            /* Register inital value */
            Reg0x523c = 0x00084280U;
            Reg0x5240 = 0x80015e59U;
            Reg0x5244 = 0x00334485U;
            Reg0x5248 = 0x5253b002U;
            Reg0x524c = 0xb8234533U;
            Reg0x5254 = ((DCOParam[SerDesRate].FreqAdjust& 0xffffU) << 8U) | 0x00000020U;
            Reg0x5258 = ((DCOParam[SerDesRate].DcoFreq & 0x3U) << 16U) | 0x000002f2U;/* div_int_dco_rx[3:0] set to 3'h1 -> 2 */
            Reg0x5264 = 0x00000000U;
            Reg0x526c = 0x10112000U;
            Reg0x5270 = 0x00000070U;
            Reg0x5280 = 0x00000000U;

            // Cal select: eq data buf vcm, ctle vcm, dfe vcm(off)
            Reg0x523c &= (~((UINT32)0x7U << 5U));
            Reg0x523c |= (((UINT32)EqVcmCal << 7U) | ((UINT32)CtleVcmCal << 6U) | ((UINT32)DfeVcmCal << 5U));
            Reg0x523c |= ((UINT32)0x1 << 17U); /* mipi_cdr_out is Calibration related blocks */
            (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x523cU, 0, B8_DATA_WIDTH_32BIT, 1, &Reg0x523c);

            // Cal select: slicer buffer vcm(off)
            Reg0x5240 &= (~((UINT32)0x1 << 26U));
            Reg0x5240 |= ((UINT32)SlicerVcmCal << 26U);
            (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5240U, 0, B8_DATA_WIDTH_32BIT, 1, &Reg0x5240);

            // Cal select: slicer offset [21] 1'h1 slicer offset calibration manual mode; 0: auto calib; 1:manual force
            Reg0x5258 &= (~((UINT32)0x1 << 21U));
            Reg0x5258 |= ((UINT32)SlicerOffsetCal << 21U);
            (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5258U, 0, B8_DATA_WIDTH_32BIT, 1, &Reg0x5258);

            // Cal select: ctle offset [23] 1'h1 ctle offset calibration manual mode; 0: auto calib; 1:manual force
            Reg0x524c &= (~((UINT32)0x1 << 23U));
            Reg0x524c |= ((UINT32)CtleOffsetCal << 23U);
            (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x524cU, 0, B8_DATA_WIDTH_32BIT, 1, &Reg0x524c);

            // enable AFE calibration block
            /* slicer offset calibration enable. short buffer P/N output; [31] voscalib_en_rxch0 */
            Reg0x5248 &= (~((UINT32)0x1 << 31U));
            (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5248U, 0, B8_DATA_WIDTH_32BIT, 1, &Reg0x5248);
            /* vcm calib power down control=0 */
            Reg0x5270 &= (~((UINT32)0x1 << 3U));
            (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5270U, 0, B8_DATA_WIDTH_32BIT, 1, &Reg0x5270);

            // --- start eq data buf vcm calibration
            if (EqVcmCal == 0U) {
                /* set vcm reference level as 450mv for eq_buf; 1:700m;2:660mv;4:600mv;8:550mv;16:500mv;32:450mv;64:400mv:128:300mv */
                /* Reg0x5264 &= (~((UINT32)0xff)); */ /* remove since Reg0x5264 = 0U */
                Reg0x5264 |= (UINT32)0x00000010;
                (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5264U, 0, B8_DATA_WIDTH_32BIT, 1, &Reg0x5264);

                /* vcm comparator sel = eq_buf (00:eq_buf;01:vcm_ctle;10:vcm_dfe;11:vcm_slicer) */
                Reg0x5270 &= (~((UINT32)0x3));
                /* Reg0x5270 |= 0x0U; */
                (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5270U, 0, B8_DATA_WIDTH_32BIT, 1, &Reg0x5270);

                /* sel_calib[3:1] = 0 (0: eq data buf vcm 1: ctle vcm 2: dfe vcm 3: slicer buf vcm 4: ctle offset 5: slicer offset 6/7: N/A) */
                Reg0x5254 &= (~((UINT32)0x7 << 1U));
                /* Reg0x5254 |= (0x0 << 1); */
                (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5254U, 0, B8_DATA_WIDTH_32BIT, 1, &Reg0x5254);

                AmbaB8_PrintUInt5("start eq vcm  %d msec calibration", AutoCalTimeInMs[0], 0U, 0U, 0U, 0U);
                /* calibration start/enable */
                Reg0x5258 |= ((UINT32)0x1 << 31U);
                (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5258U, 0, B8_DATA_WIDTH_32BIT, 1, &Reg0x5258);

                /* calibration time */
                (void) AmbaB8_KAL_TaskSleep(AutoCalTimeInMs[0]);

                /* calibration disable */
                Reg0x5258 &= (~((UINT32)0x1 << 31U));
                (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5258U, 0, B8_DATA_WIDTH_32BIT, 1, &Reg0x5258);

                /* boot-up-link auto calibration only */
                EqVcmCal = 1U;
                AutoCalSelect[LocalSearchEnable].EqVcmCal = 1U;

                /* Observe vcm eq buf */
                AmbaB8CSL_PHYSetCdrOutSelect((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), 0x00);
                Data = AmbaB8CSL_PHYGetCdrStatus(ChipID & B8_MAIN_CHIP_ID_B8NF_MASK);
                if ((Data & 0xfff0ffffU) ==  0x00000010U) {
                    AmbaB8_PrintUInt5("obsv0:(vcm_eq): 0x%08x (default)", Data, 0U, 0U, 0U, 0U);
                } else {
                    AmbaB8_PrintUInt5("obsv0:(vcm_eq): 0x%08x", Data, 0U, 0U, 0U, 0U);
                }

                ReadVcmEqBuf = ((UINT8)Data & 0x1fU);
                VcmEqBufferCtrl = ReadVcmEqBuf;
                AmbaB8_PrintInt5("Value Range = d'%d", (INT32)ReadVcmEqBuf, 0, 0, 0, 0);
                AmbaB8_PrintUInt5("Control Code = h'%x", VcmEqBufferCtrl, 0U, 0U, 0U, 0U);

            } else {
                AmbaB8_PrintUInt5("manual vcm_eq h'%x", VcmEqBufferCtrl, 0U, 0U, 0U, 0U);
                /* vcm databuf setting for manual mode; [4:0]vcm_databuf_ctrlpin_in_rxch0 */
                Reg0x526c &= (~((UINT32)0x1f));
                Reg0x526c |= ((VcmEqBufferCtrl & (UINT32)0x1f));
                (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x526cU, 0, B8_DATA_WIDTH_32BIT, 1, &Reg0x526c);
            }

            // --- start ctle vcm calibration
            if (CtleVcmCal == 0U) {
                /* set vcm reference level as 500mv for ctle; 1:700m;2:660mv;4:600mv;8:550mv;16:500mv;32:450mv;64:400mv:128:300mv */
                Reg0x5264 &= (~((UINT32)0xff));
                Reg0x5264 |= 0x00000008U;
                (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5264U, 0, B8_DATA_WIDTH_32BIT, 1, &Reg0x5264);

                /* vcm comparator sel = vcm_ctle (00:eq_buf;01:vcm_ctle;10:vcm_dfe;11:vcm_slicer) */
                Reg0x5270 &= (~((UINT32)0x3));
                Reg0x5270 |= 0x1U;
                (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5270U, 0, B8_DATA_WIDTH_32BIT, 1, &Reg0x5270);

                /* sel_calib[3:1] = 1 (0: eq data buf vcm 1: ctle vcm 2: dfe vcm 3: slicer buf vcm 4: ctle offset 5: slicer offset 6/7: N/A) */
                Reg0x5254 &= (~((UINT32)0x7 << 1U));
                Reg0x5254 |= ((UINT32)0x1 << 1U);
                (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5254U, 0, B8_DATA_WIDTH_32BIT, 1, &Reg0x5254);

                AmbaB8_PrintUInt5("start ctle vcm  %d msec calibration", AutoCalTimeInMs[1], 0U, 0U, 0U, 0U);

                /* calibration start/enable */
                Reg0x5258 |= ((UINT32)0x1 << 31U);
                (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5258U, 0, B8_DATA_WIDTH_32BIT, 1, &Reg0x5258);

                /* calibration time */
                (void) AmbaB8_KAL_TaskSleep(AutoCalTimeInMs[1]);

                /* calibration disable */
                Reg0x5258 &= (~((UINT32)0x1 << 31U));
                (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5258U, 0, B8_DATA_WIDTH_32BIT, 1, &Reg0x5258);

                /* boot-up-link auto calibration only */
                CtleVcmCal = 1U;
                AutoCalSelect[LocalSearchEnable].CtleVcmCal = 1U;

                /* Observe vcm ctle */
                AmbaB8CSL_PHYSetCdrOutSelect((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), 0x01);
                Data = AmbaB8CSL_PHYGetCdrStatus(ChipID & B8_MAIN_CHIP_ID_B8NF_MASK);
                if (Data ==  0x00101010U) {
                    AmbaB8_PrintUInt5("obsv1:(vcm_ctle 0x%08x (default)", Data, 0U, 0U, 0U, 0U);
                } else {
                    AmbaB8_PrintUInt5("obsv1:(vcm_ctle 0x%08x", Data, 0U, 0U, 0U, 0U);
                }

                ReadVcmCtle= ((UINT8)Data & 0x1fU);
                VcmCtleCtrl = ReadVcmCtle;
                AmbaB8_PrintInt5("Value Range = d'%d", (INT32)ReadVcmCtle, 0, 0, 0, 0);
                AmbaB8_PrintUInt5("Control Code = h'%x", VcmCtleCtrl, 0U, 0U, 0U, 0U);

            } else {
                AmbaB8_PrintUInt5("manual vcm_eq h'%x", VcmCtleCtrl, 0U, 0U, 0U, 0U);
                /* vcm ctle setting for manual mode; [12:8]vcm_ctle_ctrlpin */
                Reg0x526c &= (~((UINT32)0x1f << 8U));
                Reg0x526c |= (((UINT32)VcmCtleCtrl & 0x1fU) << 8U);
                (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x526cU, 0, B8_DATA_WIDTH_32BIT, 1, &Reg0x526c);
            }

            // --- start ctle offset calibration
            if (CtleOffsetCal == 0U) {
                /* sel_calib[3:1] = 4 (0: eq data buf vcm 1: ctle vcm 2: dfe vcm 3: slicer buf vcm 4: ctle offset 5: slicer offset 6/7: N/A) */
                Reg0x5254 &= (~((UINT32)0x7 << 1U));
                Reg0x5254 |= ((UINT32)0x4 << 1U);
                (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5254U, 0, B8_DATA_WIDTH_32BIT, 1, &Reg0x5254);

                AmbaB8_PrintUInt5("start ctle offset  %d msec calibration", AutoCalTimeInMs[2], 0U, 0U, 0U, 0U);
                /* calibration start/enable */
                Reg0x5258 |= ((UINT32)0x1 << 31U);
                (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5258U, 0, B8_DATA_WIDTH_32BIT, 1, &Reg0x5258);

                /* calibration time */
                (void) AmbaB8_KAL_TaskSleep(AutoCalTimeInMs[2]);

                /* calibration disable */
                Reg0x5258 &= (~((UINT32)0x1 << 31U));
                (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5258U, 0, B8_DATA_WIDTH_32BIT, 1, &Reg0x5258);

                /* Observe vos_ctle */
                AmbaB8CSL_PHYSetCdrOutSelect((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), 0x04);
                Data = AmbaB8CSL_PHYGetCdrStatus(ChipID & B8_MAIN_CHIP_ID_B8NF_MASK);
                if ((Data & 0x0ffffff0U) ==  0x00000000U) {
                    AmbaB8_PrintUInt5("obsv4:(vos_ctle) 0x%08x (default)", Data, 0U, 0U, 0U, 0U);
                } else {
                    AmbaB8_PrintUInt5("obsv4:(vos_ctle) 0x%08x", Data, 0U, 0U, 0U, 0U);
                }
                ReadVosCtleStg1p = (UINT8)((Data >> 16U) & 0xfU);
                ReadVosCtleStg2p = (UINT8)((Data >> 20U) & 0xfU);
                ReadVosCtleStg3p = (UINT8)((Data >> 24U) & 0xfU);
                ReadVosCtleStg1n = (UINT8)((Data >> 4U) & 0xfU);
                ReadVosCtleStg2n = (UINT8)((Data >> 8U) & 0xfU);
                ReadVosCtleStg3n = (UINT8)((Data >> 12U) & 0xfU);

                DataInt32 = ((((((INT32)ReadVosCtleStg1p + (INT32)ReadVosCtleStg2p) + (INT32)ReadVosCtleStg3p) * 4) - ((((INT32)ReadVosCtleStg1n + (INT32)ReadVosCtleStg2n) + (INT32)ReadVosCtleStg3n) * 4)) / 4);
                VosCtle[i].ValueRange = (INT8)DataInt32;
                AmbaMisra_TypeCast32(&Data, &DataInt32);
                VosCtle[i].ControlCode = Data & 0x3fU;
                AmbaB8_PrintInt5("Value Range = d'%d", (INT32)VosCtle[i].ValueRange, 0, 0, 0, 0);
                AmbaB8_PrintUInt5("Control Code = h'%x", VosCtle[i].ControlCode, 0U, 0U, 0U, 0U);
            } else {
                AmbaB8_PrintUInt5("manual vos_ctle h'%x", VosCtleCtrl, 0U, 0U, 0U, 0U);
                /* ctle offset calib manual input in 2s. encode inside module to obtain offset_ctle_ctrlp; [13:8]offset_ctle_code_in_rxch0 */
                Reg0x5264 &= (~((UINT32)0x3fU << 8U));
                Reg0x5264 |= ((UINT32)VosCtleCtrl << 8U);
                (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5264U, 0, B8_DATA_WIDTH_32BIT, 1, &Reg0x5264);
            }

            // --- start slicer offset calibration
            if (SlicerOffsetCal == 0U) {
                /* This increase the calibration gain so that calibration can done much faster; [23:20] slicer offset calibration gain control 7'h1=2^-26 4'hf=2^-12: N/A */
                /* The field name "lpgain_vcm_rx"  set value = 4'h7 */
                Reg0x5270 &= (~((UINT32)0xf << 20U));
                Reg0x5270 |= ((UINT32)LpgainVcm << 20U);
                (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5270U, 0, B8_DATA_WIDTH_32BIT, 1, &Reg0x5270);

                /* sel_calib[3:1] = 5 (0: eq data buf vcm 1: ctle vcm 2: dfe vcm 3: slicer buf vcm 4: ctle offset 5: slicer offset 6/7: N/A) */
                Reg0x5254 &= (~((UINT32)0x7 << 1U));
                Reg0x5254 |= ((UINT32)0x5 << 1U);
                (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5254U, 0, B8_DATA_WIDTH_32BIT, 1, &Reg0x5254);

                AmbaB8_PrintUInt5("start slicer offset  %d msec calibration", AutoCalTimeInMs[3], 0U, 0U, 0U, 0U);
                /* calibration start/enable */
                Reg0x5258 |= ((UINT32)0x1 << 31U);
                (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5258U, 0, B8_DATA_WIDTH_32BIT, 1, &Reg0x5258);

                /* calibration time */
                (void) AmbaB8_KAL_TaskSleep(AutoCalTimeInMs[3]);

                /* calibration disable */
                Reg0x5258 &= (~((UINT32)0x1 << 31U));
                (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5258U, 0, B8_DATA_WIDTH_32BIT, 1, &Reg0x5258);

                /* vos_dat_slicer */
                AmbaB8CSL_PHYSetCdrOutSelect((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), 0x05);
                Data = AmbaB8CSL_PHYGetCdrStatus(ChipID & B8_MAIN_CHIP_ID_B8NF_MASK);
                if (Data ==  0x00084210U) {
                    AmbaB8_PrintUInt5("obsv5:(vos_dat_slicer) 0x%08x (default)", Data, 0U, 0U, 0U, 0U);
                } else {
                    AmbaB8_PrintUInt5("obsv5:(vos_dat_slicer) 0x%08x", Data, 0U, 0U, 0U, 0U);
                }
                ReadVosSliceDat1p = (UINT8)((Data >> 5U) & 0x1fU);
                ReadVosSliceDat1n = (UINT8)((Data) & 0x1fU);
                ReadVosSliceDat0p = (UINT8)((Data >> 15U) & 0x1fU);
                ReadVosSliceDat0n = (UINT8)((Data >> 10U) & 0x1fU);

                DataInt32 = (((INT32)ReadVosSliceDat1p * 4) - ((INT32)ReadVosSliceDat1n * 4)) / 4;
                AmbaMisra_TypeCast32(&Data, &DataInt32);
                VosSlicerDat1[i].ValueRange = (INT8)DataInt32;
                VosSlicerDat1[i].ControlCode = Data & 0x3fU;

                DataInt32 = (((INT32)ReadVosSliceDat0p * 4) - ((INT32)ReadVosSliceDat0n * 4)) / 4;
                AmbaMisra_TypeCast32(&Data, &DataInt32);
                VosSlicerDat0[i].ValueRange = (INT8)DataInt32;
                VosSlicerDat0[i].ControlCode = Data & 0x3fU;

                /* vos_edg_slicer */
                AmbaB8CSL_PHYSetCdrOutSelect((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), 0x06);
                Data = AmbaB8CSL_PHYGetCdrStatus(ChipID & B8_MAIN_CHIP_ID_B8NF_MASK);
                if (Data ==  0x00084210U) {
                    AmbaB8_PrintUInt5("obsv6:(vos_edg_slicer) 0x%08x (default)", Data, 0U, 0U, 0U, 0U);
                } else {
                    AmbaB8_PrintUInt5("obsv6:(vos_edg_slicer) 0x%08x", Data, 0U, 0U, 0U, 0U);
                }
                ReadVosSliceEdg0p = (UINT8)((Data >> 5U) & 0x1fU);
                ReadVosSliceEdg0n = (UINT8)((Data) & 0x1fU);
                ReadVosSliceEdg1p = (UINT8)((Data >> 15U) & 0x1fU);
                ReadVosSliceEdg1n = (UINT8)((Data >> 10U) & 0x1fU);
                DataInt32 = (((INT32)ReadVosSliceEdg1p * 4) - ((INT32)ReadVosSliceEdg1n * 4)) / 4;
                AmbaMisra_TypeCast32(&Data, &DataInt32);
                VosSlicerEdg1[i].ValueRange = (INT8)DataInt32;
                VosSlicerEdg1[i].ControlCode = Data & 0x3fU;

                DataInt32 = (((INT32)ReadVosSliceEdg0p * 4) - ((INT32)ReadVosSliceEdg0n * 4)) / 4;;
                AmbaMisra_TypeCast32(&Data, &DataInt32);
                VosSlicerEdg0[i].ValueRange = (INT8)DataInt32;
                VosSlicerEdg0[i].ControlCode = Data & 0x3fU;

                AmbaB8_PrintInt5("Value Range = d'(%d, %d, %d, %d)", \
                                 VosSlicerDat1[i].ValueRange, VosSlicerDat0[i].ValueRange, VosSlicerEdg1[i].ValueRange, VosSlicerEdg0[i].ValueRange, 0);
                AmbaB8_PrintUInt5("Control Code = h'(%x, %x, %x, %x)", \
                                  VosSlicerDat1[i].ControlCode, VosSlicerDat0[i].ControlCode, VosSlicerEdg1[i].ControlCode, VosSlicerEdg0[i].ControlCode, 0U);
            } else {
                AmbaB8_PrintUInt5("manual vos_dat_slicer h'%x",VosSlicerDat1Ctrl, VosSlicerDat0Ctrl, 0U, 0U, 0U);
                AmbaB8_PrintUInt5("manual vos_edg_slicer h'%x", VosSlicerEdg1Ctrl, VosSlicerEdg0Ctrl, 0U, 0U, 0U);

                /* Reg0x5280 &= (~((UINT32)0x3ffff << 10U)); */ /* remove, since Reg0x5280 is 0 */
                Reg0x5280 |= ((VosSlicerDat1Ctrl & 0x3fU) << 10U);
                Reg0x5280 |= ((VosSlicerDat0Ctrl & 0x3fU) << 16U);
                Reg0x5280 |= ((VosSlicerEdg0Ctrl & 0x3fU) << 22U);
                (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5280U, 0, B8_DATA_WIDTH_32BIT, 1, &Reg0x5280);
                Reg0x5244 &= (~((UINT32)0x3f << 22U));
                Reg0x5244 |= ((VosSlicerEdg1Ctrl & 0x3fU) << 22U);
                (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5244U, 0, B8_DATA_WIDTH_32BIT, 1, &Reg0x5244);
            }

            /* slicer offset calibration enable. short buffer P/N output; [31] voscalib_en_rxch0 */
            Reg0x5248 &= (~((UINT32)0x1 << 31U));
            (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5248U, 0, B8_DATA_WIDTH_32BIT, 1, &Reg0x5248);        //B6N:short_resistor = 0, not short

            /* Vote for auto calibration result */
            if ((CtleOffsetCal == 0U) || (SlicerOffsetCal == 0U)) {
                if ((i + 1U) >= AutoCalibNumbers) {
                    /* fill distance data */
                    for (j = 0U; j < AutoCalibNumbers; j++) {
                        Distance[j] = 0U;

                        /* fill distance of ith calibration data */
                        for (k = 0U; k < AutoCalibNumbers; k++) {
                            if (j == k) {
                                continue;
                            }
                            if (CtleOffsetCal == 0U) {
                                Distance[j] += (UINT32)GetDistance(VosCtle[j].ValueRange, VosCtle[k].ValueRange);
                            }
                            if (SlicerOffsetCal == 0U) {
                                Distance[j] += (UINT32)GetDistance(VosSlicerDat1[j].ValueRange, VosSlicerDat1[k].ValueRange);
                                Distance[j] += (UINT32)GetDistance(VosSlicerDat0[j].ValueRange, VosSlicerDat0[k].ValueRange);
                                Distance[j] += (UINT32)GetDistance(VosSlicerEdg1[j].ValueRange, VosSlicerEdg1[k].ValueRange);
                                Distance[j] += (UINT32)GetDistance(VosSlicerEdg0[j].ValueRange, VosSlicerEdg0[k].ValueRange);
                            }
                        }
                        AmbaB8_PrintUInt5("Distance[%u]=%u", j, Distance[j], 0U, 0U, 0U);
                    }

                    /* have to make a decision */
                    Vote = 0U;
                    for (j = 1U; j < AutoCalibNumbers; j++) {
                        if (Distance[Vote] > Distance[j]) {
                            Vote = j;
                        }
                    }
                } else if (i == 1U) {
                    Vote = 1U;
                    if (CtleOffsetCal == 0U) {
                        if (GetDistance(VosCtle[1].ValueRange, VosCtle[0].ValueRange) > AutoCalibThreshold) {
                            Vote = 0xffU;
                        }
                    }
                    if (SlicerOffsetCal == 0U) {
                        if (GetDistance(VosSlicerDat1[1].ValueRange, VosSlicerDat1[0].ValueRange) > AutoCalibThreshold) {
                            Vote = 0xff;
                        }
                        if (GetDistance(VosSlicerDat0[1].ValueRange, VosSlicerDat0[0].ValueRange) > AutoCalibThreshold) {
                            Vote = 0xff;
                        }
                        if (GetDistance(VosSlicerEdg1[1].ValueRange, VosSlicerEdg1[0].ValueRange) > AutoCalibThreshold) {
                            Vote = 0xff;
                        }
                        if (GetDistance(VosSlicerEdg0[1].ValueRange, VosSlicerEdg0[0].ValueRange) > AutoCalibThreshold) {
                            Vote = 0xff;
                        }
                    }
                } else {
                    /* no action here */
                }

                if (Vote != 0xffU) {
                    if (CtleOffsetCal == 0U) {
                        VosCtleCtrl = VosCtle[Vote].ControlCode;
                        if (Vote != i) {
                            /* Ctle vos calibration manual mode */
                            Reg0x524c &= (~((UINT32)0x1 << 23U));
                            Reg0x524c |= ((UINT32)1 << 23U);
                            (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x524cU, 0, B8_DATA_WIDTH_32BIT, 1, &Reg0x524c);
                            AmbaB8_PrintUInt5("manual vos_ctle h'%x", VosCtleCtrl, 0U, 0U, 0U, 0U);
                            Reg0x5264 &= (~((UINT32)0x3f << 8U));
                            Reg0x5264 |= (VosCtleCtrl << 8U);
                            (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5264U, 0, B8_DATA_WIDTH_32BIT, 1, &Reg0x5264);
                        }
                    }
                    if (SlicerOffsetCal == 0U) {
                        VosSlicerDat1Ctrl = VosSlicerDat1[Vote].ControlCode;
                        VosSlicerDat0Ctrl = VosSlicerDat0[Vote].ControlCode;
                        VosSlicerEdg1Ctrl = VosSlicerEdg1[Vote].ControlCode;
                        VosSlicerEdg0Ctrl = VosSlicerEdg0[Vote].ControlCode;

                        if (Vote != i) {
                            /* Slicer vos calibration manual mode */
                            Reg0x5258 &= (~((UINT32)0x1 << 21U));
                            Reg0x5258 |= ((UINT32)1 << 21U);
                            (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5258U, 0, B8_DATA_WIDTH_32BIT, 1, &Reg0x5258);
                            AmbaB8_PrintUInt5("manual vos_dat_slicer h'%x %x",VosSlicerDat1Ctrl, VosSlicerDat0Ctrl, 0U, 0U, 0U);
                            AmbaB8_PrintUInt5("manual vos_edg_slicer h'%x %x", VosSlicerEdg1Ctrl, VosSlicerEdg0Ctrl, 0U, 0U, 0U);
                            Reg0x5280 &= (~((UINT32)0x3ffff << 10U));
                            Reg0x5280 |= ((VosSlicerDat1Ctrl & 0x3fU) << 10U);
                            Reg0x5280 |= ((VosSlicerDat0Ctrl & 0x3fU) << 16U);
                            Reg0x5280 |= ((VosSlicerEdg0Ctrl & 0x3fU) << 22U);
                            (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5280U, 0, B8_DATA_WIDTH_32BIT, 1, &Reg0x5280);
                            Reg0x5244 &= (~((UINT32)0x3f << 22U));
                            Reg0x5244 |= ((VosSlicerEdg1Ctrl & 0x3fU) << 22U);
                            (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5244U, 0, B8_DATA_WIDTH_32BIT, 1, &Reg0x5244);
                        }
                    }
                    CtleOffsetCal = 1U;
                    SlicerOffsetCal = 1U;

                    AmbaB8_PrintUInt5("\nApply following command for calibration manual mode:", 0U, 0U, 0U, 0U, 0U);
                    AmbaB8_PrintUInt5("t b8 calib sel ls 1 1 1 1", 0U, 0U, 0U, 0U, 0U);
                    AmbaB8_PrintUInt5("t b8 calib man %d %d %d %d %d ", VcmEqBufferCtrl, VcmCtleCtrl, VosCtleCtrl, VosSlicerDat1Ctrl, VosSlicerDat0Ctrl);
                    AmbaB8_PrintUInt5("                              %d %d", VosSlicerEdg1Ctrl, VosSlicerEdg0Ctrl, 0U, 0U, 0U);
                    LoopBreak = 1U; /* break; */
                }
            } else {
                LoopBreak = 1U; /* break; */
            }

            if (LoopBreak == 1U) {
                break;
            }

            /* Reset HsRx PHY */
            Data = 0x000503e0U;
            (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5218U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);    //rst rx afe
            Data = 0x00060100U;
            (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5214U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);    //rst rx dig
            Data = 0x00060100U;
            (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5214U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);    //rst cdr dig
            (void) AmbaB8_KAL_TaskSleep(3U);

            /* Release HsRx PHY */
            Data = 0x000513e0U;
            (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5218U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);    //release rst rx afe
            Data = 0x00060110U;
            (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5214U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);    //release rst rx dig
            Data = 0x000e0110U;
            (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5214U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);    //release rst cdr dig
            /* Wait T(CDR-LOCK) */
            (void) AmbaB8_KAL_TaskSleep(3U);
        }

        AmbaB8CSL_PHYPowerUpLSTx(ChipID & B8_MAIN_CHIP_ID_B8NF_MASK);
        (void) AmbaB8_KAL_TaskSleep(3);
        AmbaB8_PrintStr5("==End Calibration", NULL, NULL, NULL, NULL, NULL);
        //  End of Calibration
    }
}

void AmbaB8_SerdesHsRxPowerDown(UINT32 ChipID)
{
    UINT32 Data;
    Data = 0x725bb400U;
    (void) AmbaB8_RegWriteU32(ChipID, B8_AHB_BASE_ADDR + 0x5248U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);                                  // pdeye_rxch0 = 1, pderr_rxch0 = 1, pd_des_rxch0 = 1
    Data = 0x42282620U;
    (void) AmbaB8_RegWriteU32(ChipID, B8_AHB_BASE_ADDR + 0x527cU, 0, B8_DATA_WIDTH_32BIT, 1, &Data);                                  // pdb_lstx_rxch0 = 0
    Data = 0x0f440020U;
    (void) AmbaB8_RegWriteU32(ChipID, B8_AHB_BASE_ADDR + 0x0230U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);                                  // power down mphy_rx pll
    Data = 0xcU;
    (void) AmbaB8_RegWriteU32(ChipID, B8_AHB_BASE_ADDR + 0x5270U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);                                  // pd_calib_rxch0 = 1, pd_ctle_rxch0 = 1

    Data = 0x21U;
    (void) AmbaB8_RegWriteU32(ChipID, B8_AHB_BASE_ADDR + 0x5254U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);                                  // pd_slicer_rxch0 = 1
    Data = 0x80000888U;
    (void) AmbaB8_RegWriteU32(ChipID, B8_AHB_BASE_ADDR + 0x5278U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);                                  // pd_eq_rxch0 = 1
}

UINT32 AmbaB8_SerdesHsRxStatus(UINT32 ChipID)
{
    UINT32 Data = 0U;
    if ((ChipID & B8_MAIN_CHIP_ID_BUILT_IN_B8NF) != 0U) {
#ifdef BUILT_IN_SERDES
        if ((ChipID & B8_SUB_CHIP_ID_B8F0) != 0U) {
            _pAmbaRCT_Reg->MipiPhyCtrl15 = (_pAmbaRCT_Reg->MipiPhyCtrl15 & 0xff1fffffU);
            Data = _pAmbaRCT_Reg->MipiPhyObsv0_21[17];
            Data = ((Data >> 4U ) & 0xfU);
            AmbaB8_PrintUInt5("DES-P0 RxStatus: 0x%01x", Data, 0U, 0U, 0U, 0U);
        }
        if ((ChipID & B8_SUB_CHIP_ID_B8F1) != 0U) {
            _pAmbaRCT_Reg->MipiPhyCtrl15 = (_pAmbaRCT_Reg->MipiPhyCtrl15 & 0xff1fffffU) | (1U << 21U);
            Data = _pAmbaRCT_Reg->MipiPhyObsv0_21[17];
            Data = ((Data >> 4U ) & 0xfU);
            AmbaB8_PrintUInt5("DES-P1 RxStatus: 0x%01x", Data, 0U, 0U, 0U, 0U);
        }
        if ((ChipID & B8_SUB_CHIP_ID_B8F2) != 0U) {
            _pAmbaRCT_Reg->MipiPhyCtrl15 = (_pAmbaRCT_Reg->MipiPhyCtrl15 & 0xff1fffffU) | (2U << 21U);
            Data = _pAmbaRCT_Reg->MipiPhyObsv0_21[17];
            Data = ((Data >> 4U ) & 0xfU);
            AmbaB8_PrintUInt5("DES-P2 RxStatus: 0x%01x", Data, 0U, 0U, 0U, 0U);
        }
#endif
    } else {
        Data = AmbaB8CSL_PHYGetRxStatus((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK));
        AmbaB8_PrintUInt5("RxStatus: 0x%01x", Data, 0U, 0U, 0U, 0U);
    }
    return Data;
}

#ifdef VLSI_DEBUG
void AmbaB8_SerdesHsRxCdrStatus(UINT32 ChipID)
{
    UINT32 Data;
    INT8   ValueRange;
    UINT32 ControlCode;

    /* Calibration Read Code */
    UINT8 ReadVcmEqBuf;
    UINT8 ReadVcmCtle;
    UINT8 ReadVosCtleStg1p, ReadVosCtleStg2p, ReadVosCtleStg3p, ReadVosCtleStg1n, ReadVosCtleStg2n, ReadVosCtleStg3n;
    UINT8 ReadVosSliceDat0p, ReadVosSliceDat0n, ReadVosSliceDat1p, ReadVosSliceDat1n;
    UINT8 ReadVosSliceEdg0p, ReadVosSliceEdg0n, ReadVosSliceEdg1p, ReadVosSliceEdg1n;
    UINT8 EqVcmCal         = AutoCalSelect[LocalSearchEnable].EqVcmCal;
    UINT8 CtleVcmCal       = AutoCalSelect[LocalSearchEnable].CtleVcmCal;
    UINT8 DfeVcmCal        = AutoCalSelect[LocalSearchEnable].DfeVcmCal;

    if ((ChipID & B8_MAIN_CHIP_ID_BUILT_IN_B8NF) != 0U) {
#ifdef BUILT_IN_SERDES
        if ((ChipID & B8_SUB_CHIP_ID_B8F0) != 0U) {
            Data = _pAmbaRCT_Reg->MipiPhyObsv0_21[19];
            AmbaB8_PrintUInt5("DES-P0 CDR Status: 0x%08x", Data, 0U, 0U, 0U, 0U);

        }
        if ((ChipID & B8_SUB_CHIP_ID_B8F1) != 0U) {
            Data = _pAmbaRCT_Reg->MipiPhyObsv0_21[0];
            AmbaB8_PrintUInt5("DES-P1 CDR Status: 0x%08x", Data, 0U, 0U, 0U, 0U);

        }
        if ((ChipID & B8_SUB_CHIP_ID_B8F2) != 0U) {
            Data = _pAmbaRCT_Reg->MipiPhyObsv0_21[1];
            AmbaB8_PrintUInt5("DES-P2 CDR Status: 0x%08x", Data, 0U, 0U, 0U, 0U);

        }
        if ((ChipID & B8_SUB_CHIP_ID_B8F3) != 0U) {
            Data = _pAmbaRCT_Reg->MipiPhyObsv0_21[2];
            AmbaB8_PrintUInt5("DES-P3 CDR Status: 0x%08x", Data, 0U, 0U, 0U, 0U);
        }
#endif
    } else {
        /* Observe results */
        Data = 0x00084200 | (0x1 << 17);
        Data |= ((EqVcmCal << 7) | (CtleVcmCal << 6) | (DfeVcmCal << 5));
        (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x523c, 0, B8_DATA_WIDTH_32BIT, 1, &Data);        //B6N:mipi_cdr_out selection: =0 mipi_cdr_out is PI/DCO; =1 mipi_cdr_out is Calibration related blocks; default=0

        /* vcm eq buf */
        AmbaB8CSL_PHYSetCdrOutSelect((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), 0x00);
        Data = AmbaB8CSL_PHYGetCdrStatus(ChipID & B8_MAIN_CHIP_ID_B8NF_MASK);
        AmbaPrintColor(YELLOW, "obsv0:(vcm_eq)");
        if ((Data & 0xfff0ffffU) ==  0x00000010U) {
            AmbaB8_PrintUInt5("CDR Status: 0x%08x (default)", Data, 0U, 0U, 0U, 0U);
        } else {
            AmbaB8_PrintUInt5("CDR Status: 0x%08x", Data, 0U, 0U, 0U, 0U);
        }

        ReadVcmEqBuf = (UINT8)(Data & 0x1fU);
        VcmEqBufferCtrl = ReadVcmEqBuf;
        AmbaPrintColor(BLUE, "Value Range = d'%d", (INT32)ReadVcmEqBuf);
        AmbaPrintColor(BLUE, "Control Code = 5h'%x", VcmEqBufferCtrl);

        /* vcm ctle */
        AmbaB8CSL_PHYSetCdrOutSelect((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), 0x01);
        Data = AmbaB8CSL_PHYGetCdrStatus(ChipID & B8_MAIN_CHIP_ID_B8NF_MASK);
        AmbaPrintColor(YELLOW, "obsv1:(vcm_ctle)");
        if (Data ==  0x00101010) {
            AmbaB8_PrintUInt5("CDR Status: 0x%08x (default)", Data, 0U, 0U, 0U, 0U);
        } else {
            AmbaB8_PrintUInt5("CDR Status: 0x%08x", Data, 0U, 0U, 0U, 0U);
        }

        ReadVcmCtle= (Data & 0x1f);
        VcmCtleCtrl = ReadVcmCtle;
        AmbaPrintColor(BLUE, "Value Range = d'%d", (INT32)ReadVcmCtle);
        AmbaPrintColor(BLUE, "Control Code = 5h'%x", VcmCtleCtrl);

        /* vcm dfe */
        AmbaB8CSL_PHYSetCdrOutSelect((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), 0x02);
        Data = AmbaB8CSL_PHYGetCdrStatus(ChipID & B8_MAIN_CHIP_ID_B8NF_MASK);
        AmbaB8_PrintStr5("obsv2:(vcm_dfe)", NULL, NULL, NULL, NULL, NULL);
        if (Data ==  0x00000013) {
            AmbaB8_PrintUInt5("CDR Status: 0x%08x (default)", Data, 0U, 0U, 0U, 0U);
        } else {
            AmbaB8_PrintUInt5("CDR Status: 0x%08x", Data, 0U, 0U, 0U, 0U);
        }

        /* vcm slicer */
        AmbaB8CSL_PHYSetCdrOutSelect((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), 0x03);
        Data = AmbaB8CSL_PHYGetCdrStatus(ChipID & B8_MAIN_CHIP_ID_B8NF_MASK);
        AmbaB8_PrintStr5("obsv3:(vcm_slicer)", NULL, NULL, NULL, NULL, NULL);
        if (Data ==  0x00000013) {
            AmbaB8_PrintUInt5("CDR Status: 0x%08x (default)", Data, 0U, 0U, 0U, 0U);
        } else {
            AmbaB8_PrintUInt5("CDR Status: 0x%08x", Data, 0U, 0U, 0U, 0U);
        }


        /* vos_ctle */
        AmbaB8CSL_PHYSetCdrOutSelect((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), 0x04);
        Data = AmbaB8CSL_PHYGetCdrStatus(ChipID & B8_MAIN_CHIP_ID_B8NF_MASK);
        AmbaPrintColor(YELLOW, "obsv4:(vos_ctle)");
        if ((Data & 0x0ffffff0) ==  0x00000000) {
            AmbaB8_PrintUInt5("CDR Status: 0x%08x (default)", Data, 0U, 0U, 0U, 0U);
        } else {
            AmbaB8_PrintUInt5("CDR Status: 0x%08x", Data, 0U, 0U, 0U, 0U);
        }
        ReadVosCtleStg1p = ((Data >> 16) & 0xf);
        ReadVosCtleStg2p = ((Data >> 20) & 0xf);
        ReadVosCtleStg3p = ((Data >> 24) & 0xf);
        ReadVosCtleStg1n = ((Data >> 4) & 0xf);
        ReadVosCtleStg2n = ((Data >> 8) & 0xf);
        ReadVosCtleStg3n = ((Data >> 12) & 0xf);
        ValueRange = ((INT8)((INT8)((ReadVosCtleStg1p + ReadVosCtleStg2p + ReadVosCtleStg3p) << 2) - (INT8)((ReadVosCtleStg1n + ReadVosCtleStg2n + ReadVosCtleStg3n) << 2)) / 4);
        ControlCode = (((UINT32)((INT8)((ReadVosCtleStg1p + ReadVosCtleStg2p + ReadVosCtleStg3p) << 2) - (INT8)((ReadVosCtleStg1n + ReadVosCtleStg2n + ReadVosCtleStg3n) << 2)) >> 2) & 0x3f);
        VosCtleCtrl = ControlCode;
        AmbaPrintColor(BLUE, "Value Range = d'%d", ValueRange);
        AmbaPrintColor(BLUE, "Control Code = 5h'%x", ControlCode);


        /* vos_dat_slicer */
        AmbaB8CSL_PHYSetCdrOutSelect((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), 0x05);
        Data = AmbaB8CSL_PHYGetCdrStatus(ChipID & B8_MAIN_CHIP_ID_B8NF_MASK);
        AmbaPrintColor(YELLOW, "obsv5:(vos_dat_slicer)");
        if (Data ==  0x00084210) {
            AmbaB8_PrintUInt5("CDR Status: 0x%08x (default)", Data, 0U, 0U, 0U, 0U);
        } else {
            AmbaB8_PrintUInt5("CDR Status: 0x%08x", Data, 0U, 0U, 0U, 0U);
        }
        ReadVosSliceDat1p = ((Data >> 5) & 0x1f);
        ReadVosSliceDat1n = ((Data >> 0) & 0x1f);
        ReadVosSliceDat0p = ((Data >> 15) & 0x1f);
        ReadVosSliceDat0n = ((Data >> 10) & 0x1f);

        ValueRange = (INT8)((INT8)(ReadVosSliceDat1p << 2) - (INT8)(ReadVosSliceDat1n << 2)) / 4;
        ControlCode = (((UINT32)((INT8)(ReadVosSliceDat1p << 2) - (INT8)(ReadVosSliceDat1n << 2)) >> 2) & 0x3f);
        VosSlicerDat1Ctrl = ControlCode;
        AmbaPrintColor(YELLOW, "VosSlicerDat1");
        AmbaPrintColor(BLUE, "Value Range = d'%d", ValueRange);
        AmbaPrintColor(BLUE, "Control Code = 5h'%x", ControlCode);

        ValueRange = (INT8)((INT8)(ReadVosSliceDat0p << 2) - (INT8)(ReadVosSliceDat0n << 2)) / 4;
        ControlCode = (((UINT32)((INT8)(ReadVosSliceDat0p << 2) - (INT8)(ReadVosSliceDat0n << 2)) >> 2) & 0x3f);
        VosSlicerDat0Ctrl = ControlCode;
        AmbaPrintColor(YELLOW, "VosSlicerDat0");
        AmbaPrintColor(BLUE, "Value Range = d'%d", ValueRange);
        AmbaPrintColor(BLUE, "Control Code = 5h'%x", ControlCode);

        /* vos_edg_slicer */
        AmbaB8CSL_PHYSetCdrOutSelect((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), 0x06);
        Data = AmbaB8CSL_PHYGetCdrStatus(ChipID & B8_MAIN_CHIP_ID_B8NF_MASK);
        AmbaPrintColor(YELLOW, "obsv6:(vos_edg_slicer)");
        if (Data ==  0x00084210) {
            AmbaB8_PrintUInt5("CDR Status: 0x%08x (default)", Data, 0U, 0U, 0U, 0U);
        } else {
            AmbaB8_PrintUInt5("CDR Status: 0x%08x", Data, 0U, 0U, 0U, 0U);
        }
        ReadVosSliceEdg0p = ((Data >> 5) & 0x1f);
        ReadVosSliceEdg0n = ((Data >> 0) & 0x1f);
        ReadVosSliceEdg1p = ((Data >> 15) & 0x1f);
        ReadVosSliceEdg1n = ((Data >> 10) & 0x1f);
        ValueRange = ((INT8)((INT8)(ReadVosSliceEdg1p << 2) - (INT8)(ReadVosSliceEdg1n << 2)) / 4);
        ControlCode = (((UINT32)((INT8)(ReadVosSliceEdg1p << 2) - (INT8)(ReadVosSliceEdg1n << 2)) >> 2) & 0x3f);
        VosSlicerEdg1Ctrl = ControlCode;
        AmbaPrintColor(YELLOW, "VosSlicerEdg1");
        AmbaPrintColor(BLUE, "Value Range = d'%d", ValueRange);
        AmbaPrintColor(BLUE, "Control Code = 5h'%x", ControlCode);


        ValueRange = ((INT8)((INT8)(ReadVosSliceEdg0p << 2) - (INT8)(ReadVosSliceEdg0n << 2)) / 4);
        ControlCode = (((UINT32)((INT8)(ReadVosSliceEdg0p << 2) - (INT8)(ReadVosSliceEdg0n << 2)) >> 2) & 0x3f);
        VosSlicerEdg0Ctrl = ControlCode;
        AmbaPrintColor(YELLOW, "VosSlicerEdg0");
        AmbaPrintColor(BLUE, "Value Range = d'%d", ValueRange);
        AmbaPrintColor(BLUE, "Control Code = 5h'%x", ControlCode);

        /* eye */
        AmbaB8CSL_PHYSetCdrOutSelect((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), 0x0a);
        Data = AmbaB8CSL_PHYGetCdrStatus(ChipID & B8_MAIN_CHIP_ID_B8NF_MASK);
        ValueRange = (((Data >> 8) & 0x3f) - (Data & 0x3f));
        AmbaPrintColor(BLUE, "Auto locked eye: d'%d", ValueRange, 0U, 0U, 0U, 0U);
    }
    return;
}
#endif

#ifdef B8_DEV_VERSION
void AmbaB8_SerdesHsRxDfeStatus(UINT32 ChipID)
{
    UINT32 Reg0x523c, Reg0x5280, Reg0x5220;
    UINT32 Tap1;
    UINT32 Tap2;
    UINT32 Tap3;
    UINT32 Tap4;
    UINT32 TapSign;

    //observe calibration status mode; [17]: obsv_calib_rxch0=1
    AmbaB8_RegReadU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x523c, 0, B8_DATA_WIDTH_32BIT, 1, &Reg0x523c);
    Reg0x523c |= 0x20000;
    (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x523c, 0, B8_DATA_WIDTH_32BIT, 1, &Reg0x523c);

    //set cdr_out_select_rxch0 = 6'ha
    AmbaB8_RegReadU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5280, 0, B8_DATA_WIDTH_32BIT, 1, &Reg0x5280);
    Reg0x5280 &= (~0x3f0);
    Reg0x5280 |= 0x0a0;
    (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5280, 0, B8_DATA_WIDTH_32BIT, 1, &Reg0x5280);

    // read mipi_cdr_out
    AmbaB8_RegReadU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5220, 0, B8_DATA_WIDTH_32BIT, 1, &Reg0x5220);
    //mipi_cdr_out[24:20]= tap1[4:0];
    Tap1 = ((Reg0x5220 >> 20) & 0x1f);
    //mipi_cdr_out[19:16]= tap2[3:0];
    Tap2 = ((Reg0x5220 >> 16) & 0xf);
    //mipi_cdr_out[31:28]= tap_sgn[3:0]  (ordered as sign of tap4,3,2,1)
    TapSign = ((Reg0x5220 >> 28) & 0xf);
    AmbaB8_PrintUInt5("cdr_out_select_rxch0 = 6'ha, mipi_cdr_out = 0x%08x",Reg0x5220, 0U, 0U, 0U, 0U);

    //set cdr_out_select_rxch0 = 6'hb
    Reg0x5280 &= (~0x3f0);
    Reg0x5280 |= 0x0b0;
    (void) AmbaB8_RegWriteU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5280, 0, B8_DATA_WIDTH_32BIT, 1, &Reg0x5280);

    // read mipi_cdr_out
    AmbaB8_RegReadU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5220, 0, B8_DATA_WIDTH_32BIT, 1, &Reg0x5220);
    //mipi_cdr_out[30:28]= tap3[2:0];
    Tap3 = ((Reg0x5220 >> 28) & 0x7);
    //mipi_cdr_out[26:24]= tap4[2:0];
    Tap4 = ((Reg0x5220 >> 24) & 0x7);
    AmbaB8_PrintUInt5("cdr_out_select_rxch0 = 6'hb, mipi_cdr_out = 0x%08x",Reg0x5220, 0U, 0U, 0U, 0U);

    AmbaPrint("Tap1: 0b'%d%d%d%d%d%d   %s%u", (TapSign & 0x1) ? 1 : 0, ((Tap1 >> 4)&0x1), ((Tap1 >> 3)&0x1), ((Tap1 >> 2)&0x1), ((Tap1 >> 1)&0x1), ((Tap1 >> 0)&0x1), \
              (TapSign & 0x1) ? "+" : "-", (Tap1 & 0x1f));
    AmbaPrint("Tap2: 0b'%d%d%d%d%d     %s%u", (TapSign & 0x2) ? 1 : 0, ((Tap2 >> 3)&0x1), ((Tap2 >> 2)&0x1), ((Tap2 >> 1)&0x1), ((Tap2 >> 0)&0x1), \
              (TapSign & 0x2) ? "+" : "-", (Tap2 & 0xf));
    AmbaPrint("Tap3: 0b'%d%d%d%d       %s%u", (TapSign & 0x4) ? 1 : 0, ((Tap3 >> 2)&0x1), ((Tap3 >> 1)&0x1), ((Tap3 >> 0)&0x1), \
              (TapSign & 0x4) ? "+" : "-", (Tap3 & 0x7));
    AmbaPrint("Tap4: 0b'%d%d%d%d       %s%u", (TapSign & 0x8) ? 1 : 0, ((Tap4 >> 2)&0x1), ((Tap4 >> 1)&0x1), ((Tap4 >> 0)&0x1), \
              (TapSign & 0x8) ? "+" : "-", (Tap4 & 0x7));
}
#endif
void AmbaB8_SerdesHsRxAutoTerm(UINT32 ChipID)
{
    /* Enable termination auto calibration */
    UINT32 Data;
    UINT32 RxCode;

    if (RxTermAutoEnable != 0U) {
        // select tx and clear start_auto and start_force
        Data = 0x00000000U;
        (void) AmbaB8_RegWriteU32(ChipID, B8_AHB_BASE_ADDR + 0x030U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);

        // start tx auto
        Data = 0x02000000U;
        (void) AmbaB8_RegWriteU32(ChipID, B8_AHB_BASE_ADDR + 0x030U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);
        (void) AmbaB8_KAL_TaskSleep(1);

        // select rx and clear start_auto and start_force
        Data = 0x00200000U;
        (void) AmbaB8_RegWriteU32(ChipID, B8_AHB_BASE_ADDR + 0x030U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);
        (void) AmbaB8_KAL_TaskSleep(1);

        // start rx auto
        Data = 0x02200000U;
        (void) AmbaB8_RegWriteU32(ChipID, B8_AHB_BASE_ADDR + 0x030U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);
        (void) AmbaB8_KAL_TaskSleep(1);

        // read result, bit[25:21] is rx code; bit[20:16] is tx code
        (void) AmbaB8_RegReadU32(ChipID, B8_AHB_BASE_ADDR + 0x034U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);
        RxCode = ((Data >> (UINT32)21U) & (UINT32)0x1fU);
        AmbaB8_PrintUInt5("Term  RxCode 0x%02x", RxCode, 0U, 0U, 0U, 0U);
        RxCode = (~(RxCode) & (UINT32)0x1fU);
        AmbaB8_PrintUInt5("Term ~RxCode 0x%02x", RxCode, 0U, 0U, 0U, 0U);

        // write rx code to 0x260 term_res_therm_rxch0 [12:8] to change HSRX termination
        (void) AmbaB8_RegReadU32(ChipID, B8_AHB_BASE_ADDR + 0x5260U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);
        Data &= (~(UINT32)0x00001f00U);
        Data |= (RxCode << (UINT32)8U);
        (void) AmbaB8_RegWriteU32(ChipID, B8_AHB_BASE_ADDR + 0x5260U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);
        (void) AmbaB8_KAL_TaskSleep(1);
    }
}


void AmbaB8_SerdesLsTxPowerUp(UINT32 ChipID)
{
    if ((ChipID & B8_MAIN_CHIP_ID_BUILT_IN_B8NF) != 0U) {
#ifdef BUILT_IN_SERDES
#endif
    } else {
        AmbaB8CSL_PHYPowerUpLSTx(ChipID & B8_MAIN_CHIP_ID_B8NF_MASK);
        (void) AmbaB8_KAL_TaskSleep(3);
    }
}

void AmbaB8_SerdesLsTxPowerDown(UINT32 ChipID)
{
    if ((ChipID & B8_MAIN_CHIP_ID_BUILT_IN_B8NF) != 0U) {
#ifdef BUILT_IN_SERDES
#endif
    } else {
        AmbaB8CSL_PHYPowerDownLSTx(ChipID & B8_MAIN_CHIP_ID_B8NF_MASK);
        (void) AmbaB8_KAL_TaskSleep(3);
    }
}

void AmbaB8_SerdesPowerDown(UINT32 ChipID)
{
    if ((ChipID & B8_MAIN_CHIP_ID_BUILT_IN_B8NF) != 0U) {
#ifdef BUILT_IN_SERDES
#endif
    } else {
        AmbaB8CSL_PHYPowerDown(ChipID);
        (void) AmbaB8_KAL_TaskSleep(3);
    }
}

void AmbaB8_SerdesLsTxConfig(UINT32 ChipID, UINT32 Value)
{
    if ((ChipID & B8_MAIN_CHIP_ID_BUILT_IN_B8NF) != 0U) {
#ifdef BUILT_IN_SERDES
        AmbaCSL_VinSetSerdesLsTxCtrl(0x42282600 | (Value & 0xff));
#endif
    } else {
        AmbaB8CSL_PHYSetLSTx((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), Value);
    }
    AmbaB8_PrintUInt5("R527c 0x%x", Value, 0U, 0U, 0U, 0U);
}

void AmbaB8_SerdesLsRxConfig(UINT32 ChipID)
{
    UINT32 Data;
    Data = (Reg5288LsRx & 0xffffffefU);
    (void) AmbaB8_RegWriteU32(ChipID, B8_AHB_BASE_ADDR + 0x5288U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);
    AmbaB8_PrintUInt5("R5288 0x%x", Data, 0U, 0U, 0U, 0U);
    (void) AmbaB8_KAL_TaskSleep(1);
}

UINT32 AmbaB8_SerdesStatus(UINT32 ChipID, UINT32 Round)
{
    UINT32 DataBuf32;
    UINT32 RandomValue;
    UINT32 i;
    UINT32 RetVal = B8_ERR_NONE;

    for(i = 0U; i < Round; i++) {
        (void) AmbaB8_Wrap_rand(&RandomValue);
        RandomValue = (RandomValue % 65535U) + 1U;

        DataBuf32 = RandomValue;
        RetVal = AmbaB8_RegWrite(ChipID, & (pAmbaB8_PllReg->CorePllFraction), 0U, B8_DATA_WIDTH_32BIT, 1U, &DataBuf32);
        DataBuf32 = 0U;
        if (RetVal == B8_ERR_NONE) {
            RetVal = AmbaB8_RegRead(ChipID, & (pAmbaB8_PllReg->CorePllFraction), 0U, B8_DATA_WIDTH_32BIT, 1U, &DataBuf32);
        }

        if ((RetVal != B8_ERR_NONE) || (DataBuf32 != RandomValue)) {
            AmbaB8_PrintUInt5("%d/%d", i, Round, 0U, 0U, 0U);
            break;
        }
    }
//    return Round;
    return i;
}

