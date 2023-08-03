/**
 *  @file AmbaB8_Depacker.c
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
 *  @details Ambarella B8 Depacker APIs
 *
 */
#include "AmbaB8.h"
#include "AmbaB8_Depacker.h"
#include "AmbaB8_Communicate.h"

#include "AmbaB8CSL_Depacker.h"
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

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_DepackerConfig
 *
 *  @Description:: Config B8 Depacker
 *
 *  @Input      ::
 *     ChipID:           B8 chip id
 *     Channel:          B8 depacker channel
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 *
\*-----------------------------------------------------------------------------------------------*/
void AmbaB8_DepackerConfig(UINT32 ChipID)
{
    B8_DEPACKER_CTRL_REG_s Ctrl = {0};

    /* Enable */
    Ctrl.Enable = 1U;
    (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_DepackerReg->Ctrl, 0U, B8_DATA_WIDTH_32BIT, 1U, &Ctrl);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_DepackerConfigTestMode
 *
 *  @Description:: Configure B8 Depacker in test pattern mode
 *
 *  @Input      ::
 *     ChipID:           B8 chip id
 *     Channel:          B8 depacker channel
 *     pPrescalerConfig: Pointer to configuration
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      UINT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_DepackerTestMode(UINT32 ChipID, const B8_DEPACKER_TEST_CONFIG_s* pDepackerTestConfig)
{
    UINT32 RetVal = B8_ERR_NONE;
    UINT32 DataBuf[1];

    if (pDepackerTestConfig == NULL) {
        RetVal = B8_ERR_ARG;
    } else {
        /* Enable */
        DataBuf[0] = 0x1;
        RetVal |= AmbaB8_RegWrite(ChipID, & pAmbaB8_DepackerReg->Ctrl, 0U, B8_DATA_WIDTH_32BIT, 1U, DataBuf);

        /* Test pattern picture w/h */
        DataBuf[0] = ((UINT32)pDepackerTestConfig->ActiveLine << (UINT32)16U) | ((UINT32)pDepackerTestConfig->ActiveWidth * 2U);
        RetVal |= AmbaB8_RegWrite(ChipID, & pAmbaB8_DepackerReg->TestActivePic, 0U, B8_DATA_WIDTH_32BIT, 1U, DataBuf);

        /* Test pattern timing (TODO) */
        DataBuf[0] = ((UINT32)pDepackerTestConfig->TotalLine << (UINT32)16U) | ((UINT32)pDepackerTestConfig->TotalCycle);
        RetVal |= AmbaB8_RegWrite(ChipID, & pAmbaB8_DepackerReg->TestTimingCtrl, 0U, B8_DATA_WIDTH_32BIT, 1U, DataBuf);

        /* Pattern Value */
        DataBuf[0] = pDepackerTestConfig->PatternVal;
        RetVal |= AmbaB8_RegWrite(ChipID, & pAmbaB8_DepackerReg->TestPatternValue, 0U, B8_DATA_WIDTH_32BIT, 1U, DataBuf);

        /* Test mode control */
        DataBuf[0] = (UINT32)pDepackerTestConfig->TestMode;
        RetVal |= AmbaB8_RegWrite(ChipID, & pAmbaB8_DepackerReg->TestCtrl, 0U, B8_DATA_WIDTH_32BIT, 1U, DataBuf);
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_PackerReset
 *
 *  @Description:: Software Reset B8 Packer
 *
 *  @Input      ::
 *     ChipID:           B8 chip id
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 *
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_DepackerReset(UINT32 ChipID)
{
    UINT32 RetVal = B8_ERR_NONE;
    UINT32 ResetValue = 0xfffff;
    B8_AHB_SW_RST_0_REG_s AhbSwRst0;

    (void) AmbaB8_Wrap_memcpy(&AhbSwRst0, &ResetValue, sizeof(B8_AHB_SW_RST_0_REG_s));

    if ((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK) != 0U) {
        AhbSwRst0.RstLDepacker0_3 = 0U;
        RetVal |= AmbaB8_RegWrite((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), & pAmbaB8_PllReg->AhbSwReset0, 0U, B8_DATA_WIDTH_32BIT, 1U, &AhbSwRst0);
        AhbSwRst0.RstLDepacker0_3 = 1U;
        RetVal |= AmbaB8_RegWrite((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), & pAmbaB8_PllReg->AhbSwReset0, 0U, B8_DATA_WIDTH_32BIT, 1U, &AhbSwRst0);
    }

    return RetVal;
}

UINT32 AmbaB8_DepackerGetStatPkt(UINT32 ChipID, UINT32 *pPassPkt, UINT32 *pErrPkt, UINT32 *pCorPkt)
{
    UINT32 RetVal = B8_ERR_NONE;
    UINT32 ReadData[3] = {0};

#ifdef BUILT_IN_SERDES
    if ((ChipID & B8_MAIN_CHIP_ID_BUILT_IN_MASK) != 0U) {
        if ((ChipID & B8_SUB_CHIP_ID_B8F0) != 0U) {
            AmbaCSL_IdspSelectSection(AMBA_DBG_PORT_IDSP_VIN);
        } else if ((ChipID & B8_SUB_CHIP_ID_B8F1) != 0U) {
            AmbaCSL_IdspSelectSection(AMBA_DBG_PORT_IDSP_PIP2_VIN);
        } else if ((ChipID & B8_SUB_CHIP_ID_B8F2) != 0U) {
            AmbaCSL_IdspSelectSection(AMBA_DBG_PORT_IDSP_PIP3_VIN);
        } else if((ChipID & B8_SUB_CHIP_ID_B8F3) != 0U) {
            AmbaCSL_IdspSelectSection(AMBA_DBG_PORT_IDSP_PIP_VIN);
        }
        DelayCycles(0xfff);
        *pPassPkt = (_pAmbaIDSP_VinMainReg->DepackerStatPassPacketCnt);
        *pErrPkt = (_pAmbaIDSP_VinMainReg->DepackerStatErrorPacketCnt);
        *pCorPkt = (_pAmbaIDSP_VinMainReg->DepackerStatCorrectedPacketCnt);
    } else
#endif
        if ((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK) != 0U) {
            RetVal = AmbaB8_RegRead((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), & (pAmbaB8_DepackerReg->StatPassPkt), 1U, B8_DATA_WIDTH_32BIT, 3U, ReadData);

            if (RetVal == B8_ERR_NONE) {
                *pPassPkt = ReadData[0];
                *pErrPkt = ReadData[1];
                *pCorPkt = ReadData[2];
            }
        } else if ((ChipID & B8_MAIN_CHIP_ID_B8ND_MASK) != 0U) {
            RetVal = AmbaB8_RegRead((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), & (pAmbaB8_DepackerReg->StatPassPkt), 1U, B8_DATA_WIDTH_32BIT, 3U, ReadData);

            if (RetVal == B8_ERR_NONE) {
                *pPassPkt = ReadData[0];
                *pErrPkt = ReadData[1];
                *pCorPkt = ReadData[2];
            }
        } else {
            RetVal = B8_ERR_ARG;
        }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
*  @RoutineName:: AmbaB8_DepackerGetErrorPkt
*
*  @Description:: Get error packet number
*
*  @Input      ::
*      ChipID:    B8 chip id
*
*  @Output     :: none
*
*  @Return     ::
*      UINT32 : Value
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_DepackerGetErrorPkt(UINT32 ChipID)
{
    UINT32 ReadData = 0U;

#ifdef BUILT_IN_SERDES
    if ((ChipID & B8_MAIN_CHIP_ID_BUILT_IN_MASK) != 0U) {
        if ((ChipID & B8_SUB_CHIP_ID_B8F0) != 0U) {
            AmbaCSL_IdspSelectSection(AMBA_DBG_PORT_IDSP_VIN);
        } else if ((ChipID & B8_SUB_CHIP_ID_B8F1) != 0U) {
            AmbaCSL_IdspSelectSection(AMBA_DBG_PORT_IDSP_PIP2_VIN);
        } else if ((ChipID & B8_SUB_CHIP_ID_B8F2) != 0U) {
            AmbaCSL_IdspSelectSection(AMBA_DBG_PORT_IDSP_PIP3_VIN);
        } else if((ChipID & B8_SUB_CHIP_ID_B8F3) != 0U) {
            AmbaCSL_IdspSelectSection(AMBA_DBG_PORT_IDSP_PIP_VIN);
        }
        DelayCycles(0xfff);
        ReadData = (_pAmbaIDSP_VinMainReg->DepackerStatErrorPacketCnt);
    } else
#endif
        if ((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK) != 0U) {
            (void) AmbaB8_RegRead((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), & (pAmbaB8_DepackerReg->StatErrPkt), 0U, B8_DATA_WIDTH_32BIT, 1U, &ReadData);
        } else if ((ChipID & B8_MAIN_CHIP_ID_B8ND_MASK) != 0U) {
            (void) AmbaB8_RegRead(ChipID, & (pAmbaB8_DepackerReg->StatErrPkt), 0U, B8_DATA_WIDTH_32BIT, 1U, &ReadData);
        } else {
            /* not expected */
        }

    return ReadData;
}

/*-----------------------------------------------------------------------------------------------*\
*  @RoutineName:: AmbaB8_DepackerGetCorrectedPkt
*
*  @Description:: Get corrected packet number
*
*  @Input      ::
*      ChipID:    B8 chip id
*
*  @Output     :: none
*
*  @Return     ::
*      UINT32 : Value
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_DepackerGetCorrectedPkt(UINT32 ChipID)
{
    UINT32 ReadData = 0U;

#ifdef BUILT_IN_SERDES
    if ((ChipID & B8_MAIN_CHIP_ID_BUILT_IN_MASK) != 0U) {
        if ((ChipID & B8_SUB_CHIP_ID_B8F0) != 0U) {
            AmbaCSL_IdspSelectSection(AMBA_DBG_PORT_IDSP_VIN);
        } else if ((ChipID & B8_SUB_CHIP_ID_B8F1) != 0U) {
            AmbaCSL_IdspSelectSection(AMBA_DBG_PORT_IDSP_PIP2_VIN);
        } else if ((ChipID & B8_SUB_CHIP_ID_B8F2) != 0U) {
            AmbaCSL_IdspSelectSection(AMBA_DBG_PORT_IDSP_PIP3_VIN);
        } else if((ChipID & B8_SUB_CHIP_ID_B8F3) != 0U) {
            AmbaCSL_IdspSelectSection(AMBA_DBG_PORT_IDSP_PIP_VIN);
        }
        DelayCycles(0xfff);
        ReadData = (_pAmbaIDSP_VinMainReg->DepackerStatCorrectedPacketCnt);
    } else
#endif
        if ((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK) != 0U) {
            (void) AmbaB8_RegRead((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), & (pAmbaB8_DepackerReg->StatCorrectPkt), 0U, B8_DATA_WIDTH_32BIT, 1U, &ReadData);
        } else if ((ChipID & B8_MAIN_CHIP_ID_B8ND_MASK) != 0U) {
            (void) AmbaB8_RegRead(ChipID, & (pAmbaB8_DepackerReg->StatCorrectPkt), 0U, B8_DATA_WIDTH_32BIT, 1U, &ReadData);
        } else {
            /* not expected */
        }
    return ReadData;
}

/*-----------------------------------------------------------------------------------------------*\
*  @RoutineName:: AmbaB8_DepackerGetPassPkt
*
*  @Description:: Get pass packet number
*
*  @Input      ::
*      ChipID:    B8 chip id
*
*  @Output     :: none
*
*  @Return     ::
*      UINT32 : Value
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_DepackerGetPassPkt(UINT32 ChipID)
{
    UINT32 ReadData = 0U;

#ifdef BUILT_IN_SERDES
    if ((ChipID & B8_MAIN_CHIP_ID_BUILT_IN_MASK) != 0U) {
        if ((ChipID & B8_SUB_CHIP_ID_B8F0) != 0U) {
            AmbaCSL_IdspSelectSection(AMBA_DBG_PORT_IDSP_VIN);
        } else if ((ChipID & B8_SUB_CHIP_ID_B8F1) != 0U) {
            AmbaCSL_IdspSelectSection(AMBA_DBG_PORT_IDSP_PIP2_VIN);
        } else if ((ChipID & B8_SUB_CHIP_ID_B8F2) != 0U) {
            AmbaCSL_IdspSelectSection(AMBA_DBG_PORT_IDSP_PIP3_VIN);
        } else if((ChipID & B8_SUB_CHIP_ID_B8F3) != 0U) {
            AmbaCSL_IdspSelectSection(AMBA_DBG_PORT_IDSP_PIP_VIN);
        }
        DelayCycles(0xfff);
        ReadData = (_pAmbaIDSP_VinMainReg->DepackerStatPassPacketCnt);
    } else
#endif
        if ((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK) != 0U) {
            (void) AmbaB8_RegRead((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), & (pAmbaB8_DepackerReg->StatPassPkt), 0U, B8_DATA_WIDTH_32BIT, 1U, &ReadData);
        } else if ((ChipID & B8_MAIN_CHIP_ID_B8ND_MASK) != 0U) {
            (void) AmbaB8_RegRead(ChipID, & (pAmbaB8_DepackerReg->StatPassPkt), 0U, B8_DATA_WIDTH_32BIT, 1U, &ReadData);
        } else {
            /* not expected */
        }

    return ReadData;
}

/*-----------------------------------------------------------------------------------------------*\
*  @RoutineName:: AmbaB8_DepackerResetPktCounter
*
*  @Description:: Reset packet counter
*
*  @Input      ::
*      ChipID:    B8 chip id
*
*  @Output     :: none
*
*  @Return     :: none
*
\*-----------------------------------------------------------------------------------------------*/
void AmbaB8_DepackerResetPktCounter(UINT32 ChipID)
{
    UINT32 WriteData;

    if ((ChipID & B8_MAIN_CHIP_ID_BUILT_IN_MASK) != 0U) {
#ifdef BUILT_IN_SERDES
        UINT32 VinID;
        /* select section */
        if ((ChipID & B8_SUB_CHIP_ID_B8F1) != 0U) {
            VinID = 2;
        } else if ((ChipID & B8_SUB_CHIP_ID_B8F2) != 0U) {
            VinID = 3;
        } else if((ChipID & B8_SUB_CHIP_ID_B8F3) != 0U) {
            VinID = 1;
        } else {
            VinID = 0;
        }

        /* toggle pkt statistics enable bit at different frame timing to reset pkt counter */
        AmbaDSP_VinPackerStatEnable(VinID, 0);
        AmbaDSP_VinPackerStatEnable(VinID, 1);
#endif
    } else if ((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK) != 0U) {
        /* toggle pkt statistics enable bit */
        WriteData = 0x00;
        (void) AmbaB8_RegWrite((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), & (pAmbaB8_DepackerReg->StatCtrl), 0U, B8_DATA_WIDTH_32BIT, 1U, &WriteData);
        WriteData = 0x11;
        (void) AmbaB8_RegWrite((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), & (pAmbaB8_DepackerReg->StatCtrl), 0U, B8_DATA_WIDTH_32BIT, 1U, &WriteData);
    } else {
        /* not expected */
    }
}

