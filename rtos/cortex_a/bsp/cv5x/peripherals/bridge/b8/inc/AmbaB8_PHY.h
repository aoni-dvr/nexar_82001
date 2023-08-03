/**
 *  @file AmbaB8_PHY.h
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
 *  @details B8 PHY driver APIs
 *
 */

#ifndef AMBA_B8_PHY_H
#define AMBA_B8_PHY_H

typedef struct {
    volatile void *pReg;
    UINT32 Addr;        /*to be remove */
    UINT32 Data;
} SERDES_REGISTER_s;

typedef struct {
    SERDES_REGISTER_s ctle;
    SERDES_REGISTER_s dfe;
} SERDES_REGISTER_TAB_s;

typedef struct {
    UINT32 EqVcmCal:        1;
    UINT32 CtleVcmCal:      1;
    UINT32 DfeVcmCal:       1;
    UINT32 SlicerVcmCal:    1;
    UINT32 SlicerOffsetCal: 1;
    UINT32 CtleOffsetCal:   1;
} SERDES_AUTO_CAL_SEL_s;                /* 0: auto cal, 1: manual */

/*---------------------------------------------------------------------------*\
 * Defined in AmbaB8_PHY.c
\*---------------------------------------------------------------------------*/
void AmbaB8_SerdesHsTxReset(UINT32 ChipID);
void AmbaB8_SerdesHsTxRelease(UINT32 ChipID);
void AmbaB8_SerdesHsTxSetup(UINT32 ChipID);
void AmbaB8_SerdesHsTxConfig(UINT32 ChipID);
void AmbaB8_SerdesHsTxReady(UINT32 ChipID);
void AmbaB8_SerdesHsTxPowerDown(UINT32 ChipID);
void AmbaB8_SerdesHsTxAutoTerm(UINT32 ChipID);
void AmbaB8_SerdesHsRxReset(UINT32 ChipID);
void AmbaB8_SerdesHsRxRelease(UINT32 ChipID);
void AmbaB8_SerdesHsRxSetup(UINT32 ChipID);
void AmbaB8_SerdesHsRxConfig(UINT32 ChipID, UINT32 SerDesRate);
void AmbaB8_SerdesHsRxCtleConfig(UINT32 ChipID);
void AmbaB8_SerdesHsRxDfeConfig(UINT32 ChipID);
void AmbaB8_SerdesHsRxDCOPause(UINT32 ChipID);
void AmbaB8_SerdesHsRxDCORelease(UINT32 ChipID);
void AmbaB8_SerdesHsRxSslmsInit(UINT32 ChipID);
void AmbaB8_SerdesHsRxCalibration(UINT32 ChipID, UINT32 SerDesRate);
void AmbaB8_SerdesHsRxPowerDown(UINT32 ChipID);
void AmbaB8_SerdesHsRxAutoTerm(UINT32 ChipID);
void AmbaB8_SerdesLsTxPowerUp(UINT32 ChipID);
void AmbaB8_SerdesLsTxPowerDown(UINT32 ChipID);
void AmbaB8_SerdesLsTxConfig(UINT32 ChipID, UINT32 Value);
void AmbaB8_SerdesLsRxConfig(UINT32 ChipID);
void AmbaB8_SerdesPowerDown(UINT32 ChipID);

UINT32 AmbaB8_SerdesStatus(UINT32 ChipID, UINT32 Round);
UINT32 AmbaB8_SerdesHsRxStatus(UINT32 ChipID);
#ifdef B8_DEV_VERSION
void AmbaB8_SerdesHsRxDfeStatus(UINT32 ChipID);
#endif

extern SERDES_REGISTER_TAB_s ParamRegTable;
extern UINT32 LocalSearchEnable;
extern UINT32 TxTermAutoEnable;
extern UINT32 RxTermAutoEnable;
extern UINT32 prelstx;
extern UINT32 poslstx;
extern UINT32 SwphyClk;
extern UINT32 Reg5288LsRx;
extern UINT32 HsTxBias[2U];
extern SERDES_AUTO_CAL_SEL_s AutoCalSelect[2U];
extern UINT32 AutoCalibNumbers;
extern UINT32 AutoCalibThreshold;
extern UINT32 AutoCalTimeInMs[4U];
extern UINT32 LpgainVcm;
extern UINT32 AutoCalibSelect;
extern UINT32 AutoCalibCnt;
extern UINT32 VcmEqBufferCtrl;
extern UINT32 VcmCtleCtrl;
extern UINT32 VosCtleCtrl;
extern UINT32 VosSlicerDat1Ctrl;
extern UINT32 VosSlicerDat0Ctrl;
extern UINT32 VosSlicerEdg1Ctrl;
extern UINT32 VosSlicerEdg0Ctrl;
extern UINT32 VrefVal;
extern UINT32 VrefManual;


#endif /* AMBA_B8_PHY_H */

