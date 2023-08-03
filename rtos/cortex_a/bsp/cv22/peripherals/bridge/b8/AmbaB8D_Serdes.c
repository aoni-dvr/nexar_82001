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

#include "AmbaB8D_Serdes.h"
#include "AmbaB8_Serdes.h"
#include "AmbaB8_PHY.h"
#include "AmbaB8_PLL.h"
#include "AmbaB8_Packer.h"
#include "AmbaB8_Depacker.h"
#include "AmbaB8_PwmEnc.h"
#include "AmbaB8_PwmDec.h"
#include "AmbaB8_Scratchpad.h"

#include "AmbaB8CSL_PLL.h"
#include "AmbaB8CSL_PHY.h"
#include "AmbaB8CSL_Packer.h"
#include "AmbaB8CSL_Depacker.h"
#include "AmbaB8CSL_PwmEnc.h"
#include "AmbaB8CSL_PwmDec.h"

UINT32 LsRxBias = 14;
UINT32 PostLsRxBias = 6;
UINT32 LsSpeed = 12;

static void B8D_SerdesForceLsMode(UINT32 HsTxChipID)
{
    UINT32 DataBuf[2];

    /* near-end go to force_ls_mode = 1, Bit[5:4] */
    DataBuf[0] = 0x10;
    (void) AmbaB8_RegWrite(HsTxChipID, & (pAmbaB8_PllReg->PwrForceLsMode), 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);

    /* ref=72MHz; afe_ready=1 */
    DataBuf[0] = 0x9008006;
    DataBuf[1] = 0xa0c4911;
    (void) AmbaB8_RegWrite(HsTxChipID, & (pAmbaB8_PhyReg->RctMphyCtrl29Reg), 1, B8_DATA_WIDTH_32BIT, 2, DataBuf);
    /* toggle tx setup */
    DataBuf[0] = 0x9008007;
    (void) AmbaB8_RegWrite(HsTxChipID, & (pAmbaB8_PhyReg->RctMphyCtrl29Reg), 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    DataBuf[0] = 0x9008006;
    (void) AmbaB8_RegWrite(HsTxChipID, & (pAmbaB8_PhyReg->RctMphyCtrl29Reg), 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    /* wait ls lock for force_ls_mode */
    (void) AmbaKAL_TaskSleep(1);

    /* PACKER0 : enable control packet */
    DataBuf[0] = 0x4;
    (void) AmbaB8_RegWrite(HsTxChipID, & (pAmbaB8_PackerReg->PacketCtrl), 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    /* DEPACKER4 */
    DataBuf[0] = 0x1;
    (void) AmbaB8_RegWrite(HsTxChipID, & (pAmbaB8_Depacker4Reg->Ctrl), 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    /* PWM ENCODER : enable pwm and disable_ack = 1 */
    DataBuf[0] = 0x8fff2001U;
    (void) AmbaB8_RegWrite(HsTxChipID, & (pAmbaB8_PwmEncReg->Ctrl), 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    DataBuf[0] = 0x1000;
    (void) AmbaB8_RegWrite(HsTxChipID, & (pAmbaB8_PwmEncReg->Ctrl1), 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);

    /* Reset pwm decoder/encoder */
    DataBuf[0] = 0x1000;
    (void) AmbaB8_RegWrite(HsTxChipID, & (pAmbaB8_PwmEncReg->ResetCtrl0), 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    (void) AmbaKAL_TaskSleep(1);
    DataBuf[0] = 0x1000;
    (void) AmbaB8_RegWrite(HsTxChipID, & (pAmbaB8_PwmEncReg->ResetCtrl1), 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    (void) AmbaKAL_TaskSleep(1);

    /* Clean replayfail */
    DataBuf[0] = 0x10000000;
    (void) AmbaB8_RegWrite(HsTxChipID, & (pAmbaB8_PwmEncReg->ErrorIntStatus1), 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    DataBuf[0] = 0x10000000;
    (void) AmbaB8_RegWrite(HsTxChipID, & (pAmbaB8_PwmEncReg->ErrorIntStatus0), 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
}

static void B8D_SerdesPreSetting(UINT32 HsTxChipID, UINT32 HsRxChipID, UINT32 SerdesRate)
{
    extern void Serdes_ConfigRxPhySetting(UINT32 ChipID);

    UINT32 DataBuf[4];

    AmbaB8_Misra_TouchUnused(&SerdesRate);
    /* Pre-config */
    if (LsSpeed == 8U) {
        /* LS speed */
        DataBuf[0] = 0x10027;
        (void) AmbaB8_RegWriteU32(HsTxChipID, 0xe0000014U, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
        AmbaB8_PrintStr5("ls speed: 8MHz", NULL, NULL, NULL, NULL, NULL);
    } else if (LsSpeed == 24U) {
        /* LS speed */
        DataBuf[0] = 0x1000D;
        (void) AmbaB8_RegWriteU32(HsTxChipID, 0xe0000014U, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
        AmbaB8_PrintStr5("ls speed: 24MHz", NULL, NULL, NULL, NULL, NULL);
    } else {
        AmbaB8_PrintStr5("ls speed: 12MHz", NULL, NULL, NULL, NULL, NULL);
    }
    /* Pre-config */
    //B8N:txpll_vco_clamp = 1
    DataBuf[0] = 0x00028409;
    (void) AmbaB8_RegWriteU32(HsTxChipID, 0xe00000ccU, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    //B8N:txpll_vco_res_ctrl = 0
    DataBuf[0] = 0x00028009;
    (void) AmbaB8_RegWriteU32(HsTxChipID, 0xe00000ccU, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    //B8N:txpll_vco_range = 2
    DataBuf[0] = 0x0002800a;
    (void) AmbaB8_RegWriteU32(HsTxChipID, 0xe00000ccU, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    //B8N:txpll_vco_type = 0, single VCO
    DataBuf[0] = 0x30571100;
    (void) AmbaB8_RegWriteU32(HsTxChipID, 0xe00000c8U, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    //B8N:txpll_int = 0xf
    DataBuf[0] = 0x0f440000;
    (void) AmbaB8_RegWriteU32(HsTxChipID, 0xe00000c0U, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    //B8N:txpll_div2_8 = 0
    DataBuf[0] = 0x30571000;
    (void) AmbaB8_RegWriteU32(HsTxChipID, 0xe00000c8U, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    //B8N:txpll_div2_9 = 1
    DataBuf[0] = 0x30571200;
    (void) AmbaB8_RegWriteU32(HsTxChipID, 0xe00000c8U, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    //B8N:txpll_div2_11 = 0
    DataBuf[0] = 0x30571200;
    (void) AmbaB8_RegWriteU32(HsTxChipID, 0xe00000c8U, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    //B8N:txpll_sdiv = 0
    DataBuf[0] = 0x0f440000;
    (void) AmbaB8_RegWriteU32(HsTxChipID, 0xe00000c0U, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    //B8N:txpll_sout = 0
    DataBuf[0] = 0x0f400000;
    (void) AmbaB8_RegWriteU32(HsTxChipID, 0xe00000c0U, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    //B8N:txpll_vco_halt = 1
    DataBuf[0] = 0x0f400040;
    (void) AmbaB8_RegWriteU32(HsTxChipID, 0xe00000c0U, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    //B8N:txpll_rst_l = 1
    DataBuf[0] = 0x0f400050;
    (void) AmbaB8_RegWriteU32(HsTxChipID, 0xe00000c0U, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    //B8N:txpll_vco_halt = 0
    DataBuf[0] = 0x0f400010;
    (void) AmbaB8_RegWriteU32(HsTxChipID, 0xe00000c0U, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    //B8N:txpll_rst_l = 0
    DataBuf[0] = 0x0f400000;
    (void) AmbaB8_RegWriteU32(HsTxChipID, 0xe00000c0U, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    //B8N:txpll_vco_clamp = 0
    DataBuf[0] = 0x00028002;
    (void) AmbaB8_RegWriteU32(HsTxChipID, 0xe00000ccU, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);

    /* LS speed */
    if (LsSpeed == 8U) {
        /* LS speed */
        DataBuf[0] = 0x10027;
        (void) AmbaB8_RegWriteU32(HsRxChipID, 0xe0000014U, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    } else if (LsSpeed == 24U) {
        /* LS speed */
        DataBuf[0] = 0x1000D;
        (void) AmbaB8_RegWriteU32(HsRxChipID, 0xe0000014U, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    } else {
        AmbaB8_PrintStr5("ls speed: 12MHz", NULL, NULL, NULL, NULL, NULL);
    }

    // set vref polarity
    DataBuf[0] = 0x5253b400;
    (void) AmbaB8_RegWriteU32(HsRxChipID, 0xe0005248U, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    // set vref range
    DataBuf[0] = 0x00334485;
    (void) AmbaB8_RegWriteU32(HsRxChipID, 0xe0005244U, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    // set vref init value
    DataBuf[0] = 0x20203020;
    (void) AmbaB8_RegWriteU32(HsRxChipID, 0xe0005260U, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    // set sslms_vref_manual_rx = 1'h0, to monitor VrefFilter lock position
    DataBuf[0] = 0x20203020;
    (void) AmbaB8_RegWriteU32(HsRxChipID, 0xe0005260U, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    // set sslms_dfe_manual = 1'h1
    DataBuf[0] = 0x98002000U;
    (void) AmbaB8_RegWriteU32(HsRxChipID, 0xe000525cU, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    // set sslms_ctle_manual = 1'h1
    DataBuf[0] = 0x0307f000;
    (void) AmbaB8_RegWriteU32(HsRxChipID, 0xe0005268U, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);

    // vref lms vref gain
    DataBuf[0] = 0x10103010;
    (void) AmbaB8_RegWriteU32(HsRxChipID, 0xe000526cU, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    // ctle lms res gain
    DataBuf[0] = 0x0317f000;
    (void) AmbaB8_RegWriteU32(HsRxChipID, 0xe0005268U, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    // ctle lms cap gain
    DataBuf[0] = 0x10113010;
    (void) AmbaB8_RegWriteU32(HsRxChipID, 0xe000526cU, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    // dfe lms tap gain
    DataBuf[0] = 0x99002000U;
    (void) AmbaB8_RegWriteU32(HsRxChipID, 0xe000525cU, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    // ctle_func_en_in = 3'h7
    DataBuf[0] = 0x00000070;
    (void) AmbaB8_RegWriteU32(HsRxChipID, 0xe0005270U, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);

#if 0
    // r2 ctrl =3
    DataBuf[0] = 0x0317f000;
    AmbaB8_RegWrite(HsRxChipID, 0xe0005268U, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    // r1 ctrl =2
    DataBuf[0] = 0x0317b000;
    (void) AmbaB8_RegWrite(HsRxChipID, 0xe0005268U, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    // r0 ctrl =0
    DataBuf[0] = 0x03178000;
    (void) AmbaB8_RegWrite(HsRxChipID, 0xe0005268U, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    // c2 ctrl =0
    DataBuf[0] = 0x03178000;
    (void) AmbaB8_RegWrite(HsRxChipID, 0xe0005268U, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    // c1 ctrl =e
    DataBuf[0] = 0x031780e0;
    (void) AmbaB8_RegWrite(HsRxChipID, 0xe0005268U, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    // c1 ctrl =e
    DataBuf[0] = 0x031780ee;
    (void) AmbaB8_RegWrite(HsRxChipID, 0xe0005268U, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    // tap4 = a
    DataBuf[0] = 0x99a02000U;
    (void) AmbaB8_RegWrite(HsRxChipID, 0xe000525cU, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    // tap3 = e
    DataBuf[0] = 0x99ae2000U;
    (void) AmbaB8_RegWrite(HsRxChipID, 0xe000525cU, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    // tap2 = 4
    DataBuf[0] = 0x99ae2400U;
    (void) AmbaB8_RegWrite(HsRxChipID, 0xe000525cU, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    // tap1 = c
    DataBuf[0] = 0x99ae240cU;
    (void) AmbaB8_RegWrite(HsRxChipID, 0xe000525cUU, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
#else
    Serdes_ConfigRxPhySetting(HsRxChipID);
#endif
    //power down lstx0
    DataBuf[0] = 0xc2282620U | LsRxBias;
    (void) AmbaB8_RegWriteU32(HsRxChipID, 0xe000527cU, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    AmbaB8_PrintUInt5("Pre LsSetting: 0x%X", DataBuf[0], 0U, 0U, 0U, 0U);

    //term_res = f
    DataBuf[0] = 0x20202f20;
    (void) AmbaB8_RegWriteU32(HsRxChipID, 0xe0005260U, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);

    // tx_hs_bias = 8'hff
    DataBuf[0] = 0x81ff010cU;
    (void) AmbaB8_RegWriteU32(HsTxChipID, 0xe000528cU, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    //0:data based FA, 1:original FA
    DataBuf[0] = 0x5253b400;
    (void) AmbaB8_RegWriteU32(HsRxChipID, 0xe0005248U, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
}

static void B8D_SerdesSetupLink(UINT32 HsTxChipID, UINT32 HsRxChipID)
{
    UINT32 DataBuf[3];

    /* PWM ENCODER: set default chunk size */
    DataBuf[0] = 0x20000;
    (void) AmbaB8_RegWrite(HsTxChipID, & (pAmbaB8_PwmEncReg->CreditCfg1), 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    /* PWM DECODER: set default credit pwr: ls_credit = 0, hs_credit = 0 */
    DataBuf[0] = 0x0;
    (void) AmbaB8_RegWrite(HsRxChipID, & (pAmbaB8_PwmDecReg->Ctrl), 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    DataBuf[0] = 0x0;
    (void) AmbaB8_RegWrite(HsTxChipID, & (pAmbaB8_PwmEncReg->CreditCfg0), 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    DataBuf[0] = 0x0;
    (void) AmbaB8_RegWrite(HsTxChipID, & (pAmbaB8_PwmEncReg->CreditCfg1), 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
}

static void B8D_SerdesPhySetting(UINT32 HsTxChipID, UINT32 HsRxChipID, UINT32 SerdesRate)
{
    UINT32 DataBuf[4];

    /* B8D PHY */
    /* Ctrl29: set fre 72MHz */
    DataBuf[0] = 0x09008000;
    (void) AmbaB8_RegWrite(HsRxChipID, & (pAmbaB8_PhyReg->RctMphyCtrl29Reg), 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    /* Ctrl19 */
    DataBuf[0] = 0x20202f40;
    (void) AmbaB8_RegWrite(HsRxChipID, & (pAmbaB8_PhyReg->RctMphyCtrl19Reg), 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    /* Ctrl9 */
    DataBuf[0] = 0xb000efffU;
    (void) AmbaB8_RegWrite(HsRxChipID, & (pAmbaB8_PhyReg->RctMphyCtrl9Reg), 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    /* Ctrl5 */
    DataBuf[0] = 0x000e0110;
    (void) AmbaB8_RegWrite(HsRxChipID, & (pAmbaB8_PhyReg->RctMphyCtrl5Reg), 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    /* Ctrl6 */
    DataBuf[0] = 0x000513e0;
    (void) AmbaB8_RegWrite(HsRxChipID, & (pAmbaB8_PhyReg->RctMphyCtrl6Reg), 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);

    /* B8N PHY */
    DataBuf[0] = 0x09008006;
    (void) AmbaB8_RegWriteU32(HsTxChipID, 0xe0005284U, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    DataBuf[0] = 0xa0c4911U;
    (void) AmbaB8_RegWriteU32(HsTxChipID, 0xe0005288U, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);

    //AmbaPrintColor(YELLOW, "Build Link with SerDes rate: %d Mhz", DCOParam[SerdesRate].Frequency);
    (void) AmbaB8_PllSetMphyTx0Clk(HsTxChipID, SerdesRate);
}

static UINT32 B8D_SerdesModeChange(UINT32 HsTxChipID, UINT32 HsRxChipID)
{
    UINT32 DataBuf[4];
    UINT32 RetVal = B8_ERR_NONE;

    /* Reset_sequence timing: Tturn = 600 us, Trstn_pre = 0 us, Trstn = 12 us, Tcdrlock = 1200 us, Ttimeout = 2.79 s, Tturn2 = 600 us */
    /* Tturn = 1000/24M * 14400(0x3840) = 0.6ms */
    /* Tcdrlock = 1000/24M * 28800(0x7080) = 1.2ms */
    /* Ttimeout = 1000/24M * 0x3ffffff = 2.79s */
    DataBuf[0] = 0xf01e0;
    (void) AmbaB8_RegWrite(HsRxChipID, & (pAmbaB8_PhyReg->McMphyRxRstReg), 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    (void) AmbaKAL_TaskSleep(1);
    DataBuf[0] = 0x3840;
    DataBuf[1] = 0x7080;
    DataBuf[2] = 0x3ffffff;
    DataBuf[3] = 0x3840;
    (void) AmbaB8_RegWrite(HsRxChipID, & (pAmbaB8_PhyReg->McMphyRxTurnAroundP1Reg), 1, B8_DATA_WIDTH_32BIT, 4, DataBuf);
    (void) AmbaKAL_TaskSleep(1);
    /* Start mode change */
    DataBuf[0] = 0x10;
    (void) AmbaB8_RegWrite(HsRxChipID, & (pAmbaB8_PhyReg->RctModechgCtrlReg), 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    (void) AmbaKAL_TaskSleep(1);
    /* Disable force ls */
    DataBuf[0] = 0x0;
    (void) AmbaB8_RegWrite(HsTxChipID, & (pAmbaB8_PllReg->PwrForceLsMode), 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    (void) AmbaKAL_TaskSleep(1);
    /* Tx reset */
    DataBuf[0] = 0x9008000;
    (void) AmbaB8_RegWrite(HsTxChipID, & (pAmbaB8_PhyReg->RctMphyCtrl29Reg), 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    (void) AmbaKAL_TaskSleep(1);
    DataBuf[0] = 0x9008006;
    (void) AmbaB8_RegWrite(HsTxChipID, & (pAmbaB8_PhyReg->RctMphyCtrl29Reg), 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    (void) AmbaKAL_TaskSleep(1);

    /* First Tx setup */
    DataBuf[0] = 0x9008007;
    (void) AmbaB8_RegWrite(HsTxChipID, & (pAmbaB8_PhyReg->RctMphyCtrl29Reg), 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    (void) AmbaKAL_TaskSleep(1);
    DataBuf[0] = 0x9008006;
    (void) AmbaB8_RegWrite(HsTxChipID, & (pAmbaB8_PhyReg->RctMphyCtrl29Reg), 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    (void) AmbaKAL_TaskSleep(30);

    /* Seconde Tx setup */
    DataBuf[0] = 0x9008007;
    (void) AmbaB8_RegWrite(HsTxChipID, & (pAmbaB8_PhyReg->RctMphyCtrl29Reg), 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    (void) AmbaKAL_TaskSleep(1);
    DataBuf[0] = 0x9008006;
    (void) AmbaB8_RegWrite(HsTxChipID, & (pAmbaB8_PhyReg->RctMphyCtrl29Reg), 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    DataBuf[0] = 0x0;
    (void) AmbaB8_RegWrite(HsTxChipID, & (pAmbaB8_PwmEncReg->Ctrl1), 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    DataBuf[0] = 0x1000;
    (void) AmbaB8_RegWrite(HsTxChipID, & (pAmbaB8_PwmEncReg->ResetCtrl0), 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    (void) AmbaB8_RegWrite(HsTxChipID, & (pAmbaB8_PwmEncReg->ResetCtrl1), 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);

    DataBuf[0] = 0x8fff2071U;
    (void) AmbaB8_RegWrite(HsTxChipID, & (pAmbaB8_PwmEncReg->Ctrl), 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);

    /* check link */
    (void) AmbaKAL_TaskSleep(20);
    (void) AmbaB8_RegReadU32(HsTxChipID, 0xe00098a8U, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);

    if (DataBuf[0] != 0U) {
        /* NG case, reset PWM encoder */
        DataBuf[0] = 0x1000;
        (void) AmbaB8_RegWriteU32(HsTxChipID, 0xe0009808U, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
        DataBuf[0] = 0x1000;
        (void) AmbaB8_RegWriteU32(HsTxChipID, 0xe00098e4U, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
        //t b8 writew 0x10 ahb 0x98ac+ 0x10000000
        DataBuf[0] = 0x10000000;
        (void) AmbaB8_RegWriteU32(HsTxChipID, 0xe00098acU, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
        //t b8 writew 0x10 ahb 0x98a8+ 0x10000000
        DataBuf[0] = 0x10000000;
        (void) AmbaB8_RegWriteU32(HsTxChipID, 0xe00098a8U, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
        AmbaB8_PrintStr5("Mode change fail, reset pwm encoder", NULL, NULL, NULL, NULL, NULL);
        RetVal = B8_ERR_SERDES_LINK;
    } else {
        /* Disable modechange or trigger hb to prevent fallback */
#if 1
        /* OK case, disable mode change */
        DataBuf[0] = 0x0;
        (void) AmbaB8_RegWrite(HsRxChipID, & (pAmbaB8_PhyReg->RctModechgCtrlReg), 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
#else
        /* In new scrpit */
        DataBuf[0] = 0x1;
        (void) AmbaB8_RegWrite(HsTxChipID, & (pAmbaB8_PwmEncReg->ErrorIntStatus0), 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
        (void) AmbaKAL_TaskSleep(1);

        /* trigger hb to prevent fallback */
        DataBuf[0] = 0x1;
        (void) AmbaB8_RegWrite(HsTxChipID, & (pAmbaB8_MipiReg->MphyRxLinkHeartBeat), 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
#endif
        AmbaB8_PrintStr5("Mode change done", NULL, NULL, NULL, NULL, NULL);
        DataBuf[0] = 0xc2282620U | PostLsRxBias;
        (void) AmbaB8_RegWriteU32(HsRxChipID, 0xe000527cU, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
        AmbaB8_PrintUInt5("Post Ls Setting: 0x%X", DataBuf[0], 0U, 0U, 0U, 0U);

    }

    return RetVal;
}

static UINT32 B8D_SerdesCheck(UINT32 HsRxChipID)
{
    UINT32 DataBuf[2];
    UINT32 RetVal = B8_ERR_NONE;

    DataBuf[0] = 0xabcd1234U;
    (void) AmbaB8_RegWriteU32(HsRxChipID, 0xe0000004U, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);

    DataBuf[0] = 0x0;
    (void) AmbaB8_RegReadU32(HsRxChipID, 0xe0000004U, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    if (DataBuf[0] == 0xabcd1234U) {
        AmbaB8_PrintStr5("B8D read/write test OK", NULL, NULL, NULL, NULL, NULL);
    } else {
        AmbaB8_PrintStr5("B8D read/write test NG", NULL, NULL, NULL, NULL, NULL);
        RetVal = B8_ERR_SERDES_LINK;
    }

    return RetVal;
}

static void B8D_SerdesPostSetting(UINT32 HsRxChipID)
{
    UINT32 DataBuf[2];

    /* Depacket control */
    DataBuf[0] = 0x00000000;
    (void) AmbaB8_RegWriteU32(HsRxChipID, 0xe0002850U, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    DataBuf[0] = 0x00000013;
    (void) AmbaB8_RegWriteU32(HsRxChipID, 0xe0002850U, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    DataBuf[0] = 0x01;
    (void) AmbaB8_RegWriteU32(HsRxChipID, 0xe0002800U, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
}

UINT32 AmbaB8D_SerdesLink(UINT32 ChipID, UINT32 SerdesRate)
{
    UINT32 RetVal = B8_ERR_NONE;
    UINT32 HsTxChipID = ChipID & B8_MAIN_CHIP_ID_B8ND_MASK;
    UINT32 HsRxChipID = ChipID;

    /* Build B8D Serdes Link */
    if ((ChipID & B8_MAIN_CHIP_ID_B8ND_MASK) != 0U) {
        B8D_SerdesForceLsMode(HsTxChipID);

        B8D_SerdesPreSetting(HsTxChipID, HsRxChipID, SerdesRate);
        B8D_SerdesSetupLink(HsTxChipID, HsRxChipID);
        B8D_SerdesPhySetting(HsTxChipID, HsRxChipID, SerdesRate);
        RetVal = B8D_SerdesModeChange(HsTxChipID, HsRxChipID);
        if (RetVal == OK) {
            RetVal = B8D_SerdesCheck(HsRxChipID);
        }

        if ( RetVal == OK) {
            /* Enable depacker */
            B8D_SerdesPostSetting(HsRxChipID);
        }
    } else {
        RetVal = B8_ERR_UNEXPECTED;
    }

    if (RetVal == B8_ERR_NONE) {
        AmbaB8_PrintStr5("B8D Serdes Link Build Succeed.", NULL, NULL, NULL, NULL, NULL);
    } else {
        AmbaB8_PrintStr5("B8D Serdes Link Build Fail.", NULL, NULL, NULL, NULL, NULL);
    }

    return RetVal;
}

#if 0
UINT32 AmbaB8D_SerdesLinkHeartBeat(UINT32 ChipID, UINT8 TimeOut)
{
    UINT32 RetVal = B8_ERR_NONE;
    UINT32 DataBuf[2];
    UINT32 HsRxChipID = ChipID;

    if (((ChipID & B8_SUB_CHIP_ID_MASK) == B8_SUB_CHIP_ID_B8D0) || ((ChipID & B8_SUB_CHIP_ID_MASK) == B8_SUB_CHIP_ID_B8D1)) {
        /* Trigger serdes link rx heart beat */
        DataBuf[0] = TimeOut;
        RetVal = AmbaB8_RegWrite(HsRxChipID, & (pAmbaB8_PhyReg->RxLinkHeartbeatReg), 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
    } else {
        RetVal = B8_ERR_UNEXPECTED;
    }

    return RetVal;
}
#endif
