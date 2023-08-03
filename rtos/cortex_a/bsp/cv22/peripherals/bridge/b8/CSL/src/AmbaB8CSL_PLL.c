/**
 *  @file AmbaCSL_PLL.c
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
 *  @details B6 PLL CSL Device Driver
 *
 */

#include "AmbaB8.h"
#include "AmbaB8_PLL.h"
#include "AmbaB8_Communicate.h"

#include "AmbaB8CSL_PLL.h"

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B8_PLL_GetCtrlRegAddr
 *
 *  @Description:: Get PLL Control register address
 *
 *  @Input      ::
 *      PllIdx:          Index of the PLL
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      UINT32 * : NULL or pointer to a regiser address
\*-----------------------------------------------------------------------------------------------*/
static volatile const B8_PLL_CTRL_REG_s * B8_PLL_GetCtrlRegAddr(UINT32 PllIdx)
{
    volatile const B8_PLL_CTRL_REG_s *pPllCtrlReg = NULL;

    if (PllIdx == B8_PLL_CORE) {
        pPllCtrlReg = &pAmbaB8_PllReg->CorePllCtrl;
    } else if (PllIdx == B8_PLL_SENSOR) {
        pPllCtrlReg = &pAmbaB8_PllReg->SensorPllCtrl;
    } else if (PllIdx == B8_PLL_VIDEO) {
        pPllCtrlReg = &pAmbaB8_PllReg->VideoPllCtrl;
    } else if (PllIdx == B8_PLL_MPHY_TX0) {
        pPllCtrlReg = &pAmbaB8_PllReg->MPhyTx0PllCtrl;
    } else if (PllIdx == B8_PLL_MPHY_RX) {
        pPllCtrlReg = &pAmbaB8_PllReg->MPhyRxPllCtrl;
    } else {
        pPllCtrlReg = NULL;
    }

    return pPllCtrlReg;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B8_PLL_GetCtrl2RegAddr
 *
 *  @Description:: Get PLL Control register address
 *
 *  @Input      ::
 *      PllIdx:          Index of the PLL
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      UINT32 * : NULL or pointer to a regiser address
\*-----------------------------------------------------------------------------------------------*/
static volatile const B8_PLL_CTRL2_REG_s * B8_PLL_GetCtrl2RegAddr(UINT32 PllIdx)
{
    volatile const B8_PLL_CTRL2_REG_s *pPllCtrl2Reg = NULL;

    if (PllIdx == B8_PLL_CORE) {
        pPllCtrl2Reg = &pAmbaB8_PllReg->CorePllCtrl2;
    } else if (PllIdx == B8_PLL_SENSOR) {
        pPllCtrl2Reg = &pAmbaB8_PllReg->SensorPllCtrl2;
    } else if (PllIdx == B8_PLL_VIDEO) {
        pPllCtrl2Reg = &pAmbaB8_PllReg->VideoPllCtrl2;
    } else if (PllIdx == B8_PLL_MPHY_RX) {
        pPllCtrl2Reg = &pAmbaB8_PllReg->MPhyRxPllCtrl2;
    } else {
        pPllCtrl2Reg = NULL;
    }

    return pPllCtrl2Reg;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B8_PLL_GetCtrl3RegAddr
 *
 *  @Description:: Get PLL Control register address
 *
 *  @Input      ::
 *      PllIdx:          Index of the PLL
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      UINT32 * : NULL or pointer to a regiser address
\*-----------------------------------------------------------------------------------------------*/
static volatile const B8_PLL_CTRL3_REG_s * B8_PLL_GetCtrl3RegAddr(UINT32 PllIdx)
{
    volatile const B8_PLL_CTRL3_REG_s *pPllCtrl3Reg = NULL;

    if (PllIdx == B8_PLL_CORE) {
        pPllCtrl3Reg = &pAmbaB8_PllReg->CorePllCtrl3;
    } else if (PllIdx == B8_PLL_SENSOR) {
        pPllCtrl3Reg = &pAmbaB8_PllReg->SensorPllCtrl3;
    } else if (PllIdx == B8_PLL_VIDEO) {
        pPllCtrl3Reg = &pAmbaB8_PllReg->VideoPllCtrl3;
    } else if (PllIdx == B8_PLL_MPHY_RX) {
        pPllCtrl3Reg = &pAmbaB8_PllReg->MPhyRxPllCtrl3;
    } else {
        pPllCtrl3Reg = NULL;
    }

    return pPllCtrl3Reg;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B8_PLL_GetFracRegAddr
 *
 *  @Description:: Get PLL Control register address
 *
 *  @Input      ::
 *      PllIdx:          Index of the PLL
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      UINT32 * : NULL or pointer to a regiser address
\*-----------------------------------------------------------------------------------------------*/
static volatile const UINT32 * B8_PLL_GetFracRegAddr(UINT32 PllIdx)
{
    volatile const UINT32 *pPllFracReg = NULL;

    if (PllIdx == B8_PLL_CORE) {
        pPllFracReg = &pAmbaB8_PllReg->CorePllFraction;
    } else if (PllIdx == B8_PLL_SENSOR) {
        pPllFracReg = &pAmbaB8_PllReg->SensorPllFraction;
    } else if (PllIdx == B8_PLL_VIDEO) {
        pPllFracReg = &pAmbaB8_PllReg->VideoPllFraction;
    } else if (PllIdx == B8_PLL_MPHY_TX0) {
        pPllFracReg = &pAmbaB8_PllReg->MPhyTx0PllFraction;
    } else if (PllIdx == B8_PLL_MPHY_RX) {
        pPllFracReg = &pAmbaB8_PllReg->MPhyRxPllFraction;
    } else {
        pPllFracReg = NULL;
    }
    return pPllFracReg;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B8_PLL_GetPostScaleRegVal
 *
 *  @Description:: Get B8 PLL postscaler register value
 *
 *  @Input      ::
 *      PllIdx:         Index of the PLL
 *      DivVal:         Divider value
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      UINT32 :        DivVal
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaCSL_B8PllGetPostScaleRegVal(UINT32 ChipID, UINT32 PllIdx)
{
    UINT32 DivVal = 0xffffffffU;

    if (PllIdx == B8_PLL_CORE) {
        B8_PLL_CORE_POST_SCALER_REG_s PllRegData;

        (void) AmbaB8_RegRead(ChipID, & pAmbaB8_PllReg->CorePllPostScaler, 1, B8_DATA_WIDTH_32BIT, 1, &PllRegData);
        DivVal = (UINT32)PllRegData.PostDivider + 1U;
    } else if (PllIdx == B8_PLL_SENSOR) {
        B8_PLL_SENSOR_POST_SCALER_REG_s PllRegData;

        (void) AmbaB8_RegRead(ChipID, & pAmbaB8_PllReg->SensorPllPostScaler, 1, B8_DATA_WIDTH_32BIT, 1, &PllRegData);
        DivVal = PllRegData.SoIntPostDivider;
    } else if (PllIdx == B8_PLL_VIDEO) {
        B8_PLL_VO_POST_SCALER_REG_s PllRegData;

        (void) AmbaB8_RegRead(ChipID, & pAmbaB8_PllReg->VideoPllPostScaler, 1, B8_DATA_WIDTH_32BIT, 1, &PllRegData);
        DivVal = PllRegData.PostDivider0;
    } else {
        DivVal = 0xffffffffU;   /* as a hint for no divider */
    }
    return DivVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B8_PLL_SetPostScaleRegVal
 *
 *  @Description:: Set B8 PLL postscaler register value
 *
 *  @Input      ::
 *      PllIdx:         Index of the PLL
 *      DivVal:         Divider value
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaCSL_B8PllSetPostScaleRegVal(UINT32 ChipID, UINT32 PllIdx, UINT32 DivVal)
{
    if ((DivVal == 0U) || (DivVal == 0xffffffffU)) {
        /* Shall not happen!! */
    } else if (PllIdx == B8_PLL_CORE) {
        B8_PLL_CORE_POST_SCALER_REG_s PllRegData;

        PllRegData.PostDivider = (UINT8)(DivVal - 1U);
        PllRegData.PostEnable = 0U;
        (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_PllReg->CorePllPostScaler, 1, B8_DATA_WIDTH_32BIT, 1, &PllRegData);
        PllRegData.PostEnable = 1U;
        (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_PllReg->CorePllPostScaler, 1, B8_DATA_WIDTH_32BIT, 1, &PllRegData);
    } else if (PllIdx == B8_PLL_SENSOR) {
        B8_PLL_SENSOR_POST_SCALER_REG_s PllRegData;

        PllRegData.SoIntPostDivider = (UINT8)DivVal;
        PllRegData.SoVinPostDivider = (UINT8)DivVal;
        (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_PllReg->SensorPllPostScaler, 1, B8_DATA_WIDTH_32BIT, 1, &PllRegData);
    } else if (PllIdx == B8_PLL_VIDEO) {
        B8_PLL_VO_POST_SCALER_REG_s PllRegData;

        (void) AmbaB8_RegRead(ChipID, & pAmbaB8_PllReg->VideoPllPostScaler, 1, B8_DATA_WIDTH_32BIT, 1, &PllRegData);
        PllRegData.PostDivider0 = (UINT8)(DivVal - 1U);
        PllRegData.PostEnable0 = 0U;
        (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_PllReg->VideoPllPostScaler, 1, B8_DATA_WIDTH_32BIT, 1, &PllRegData);
        PllRegData.PostEnable0 = 1U;
        (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_PllReg->VideoPllPostScaler, 1, B8_DATA_WIDTH_32BIT, 1, &PllRegData);
    } else {
        /* No divider. Do nothing. */
    }
    return B8_ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaCSL_B8PllGetCtrlRegsVal
 *
 *  @Description:: Get PLL Control register values
 *
 *  @Input      ::
 *      ChipID:          B6 chip id
 *      PllIdx:          Index of the PLL
 *      pPllCtrlRegsVal: Pointer to the PLL Control register values
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaCSL_B8PllGetCtrlRegsVal(UINT32 ChipID, UINT32 PllIdx, B8_PLL_CTRL_REGS_VAL_s *pPllCtrlRegsVal)
{
    volatile const B8_PLL_CTRL_REG_s   *pPllCtrlReg = B8_PLL_GetCtrlRegAddr(PllIdx);
    volatile const B8_PLL_CTRL2_REG_s *pPllCtrl2Reg = B8_PLL_GetCtrl2RegAddr(PllIdx);
    volatile const B8_PLL_CTRL3_REG_s *pPllCtrl3Reg = B8_PLL_GetCtrl3RegAddr(PllIdx);
    volatile const UINT32              *pPllFracReg = B8_PLL_GetFracRegAddr(PllIdx);

    (void) AmbaB8_RegRead(ChipID, pPllCtrlReg, 1, B8_DATA_WIDTH_32BIT, 1, &pPllCtrlRegsVal->Ctrl);
    (void) AmbaB8_RegRead(ChipID, pPllCtrl2Reg, 1, B8_DATA_WIDTH_32BIT, 1, &pPllCtrlRegsVal->Ctrl2);
    (void) AmbaB8_RegRead(ChipID, pPllCtrl3Reg, 1, B8_DATA_WIDTH_32BIT, 1, &pPllCtrlRegsVal->Ctrl3);

    /* Fraction */
    if (pPllFracReg != NULL) {
        (void) AmbaB8_RegRead(ChipID, pPllFracReg, 1, B8_DATA_WIDTH_32BIT, 1, &pPllCtrlRegsVal->Fraction);
    } else {
        pPllCtrlRegsVal->Fraction = 0U;  /* No fraction */
    }

    return B8_ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaCSL_B8PllSetCtrlRegsVal
 *
 *  @Description:: Set PLL Control register values
 *
 *  @Input      ::
 *      ChipID:      B6 chip id
 *      PllIdx:      Index of the PLL
 *      pCurRegsVal: Pointer to current PLL Control register values
 *      pNewRegsVal: Pointer to the new PLL Control register values
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaCSL_B8PllSetCtrlRegsVal(UINT32 ChipID, UINT32 PllIdx,
                                   const B8_PLL_CTRL_REGS_VAL_s *pCurRegsVal,
                                   B8_PLL_CTRL_REGS_VAL_s *pNewRegsVal)
{
    volatile const B8_PLL_CTRL_REG_s   *pPllCtrlReg = B8_PLL_GetCtrlRegAddr(PllIdx);
    volatile const B8_PLL_CTRL2_REG_s *pPllCtrl2Reg = B8_PLL_GetCtrl2RegAddr(PllIdx);
    volatile const B8_PLL_CTRL3_REG_s *pPllCtrl3Reg = B8_PLL_GetCtrl3RegAddr(PllIdx);
    volatile const UINT32              *pPllFracReg = B8_PLL_GetFracRegAddr(PllIdx);
    UINT32 PllRegData[2];
    UINT32 RetStatus = B8_ERR_NONE;
    INT32 i;

    /* Activate the vco clamp */
    pNewRegsVal->Ctrl3.VcoClamp = 1U;

    /* byp_jdiv should be asserted first after clamp is set */
    pNewRegsVal->Ctrl3.BypassJDiv = 1U;

    /* Set pll's dsmclk = refclk  */
    pNewRegsVal->Ctrl2.CtrlAbility = 0x10U;

    (void) AmbaB8_Wrap_memcpy(&PllRegData[0], &pNewRegsVal->Ctrl2, sizeof(UINT32));
    (void) AmbaB8_Wrap_memcpy(&PllRegData[1], &pNewRegsVal->Ctrl3, sizeof(UINT32));
    (void) AmbaB8_RegWrite(ChipID, pPllCtrl2Reg, 1, B8_DATA_WIDTH_32BIT, 2, PllRegData);

    /* First bring pll out of powerdown */
    if (pCurRegsVal->Ctrl.PowerDown != 0U) {
        pNewRegsVal->Ctrl.ForcePllReset = 0U;
        pNewRegsVal->Ctrl.PowerDown = 0U;
        (void) AmbaB8_Wrap_memcpy(&PllRegData[0], &pNewRegsVal->Ctrl, sizeof(UINT32));
        (void) AmbaB8_RegWrite(ChipID, pPllCtrlReg, 1, B8_DATA_WIDTH_32BIT, 1, PllRegData);
        for (i = 0; i < 0xfff; i++) {}  /* Wait for a while */

        pNewRegsVal->Ctrl.ForcePllReset = 1U;
        (void) AmbaB8_Wrap_memcpy(&PllRegData[0], &pNewRegsVal->Ctrl, sizeof(UINT32));
        (void) AmbaB8_RegWrite(ChipID, pPllCtrlReg, 1, B8_DATA_WIDTH_32BIT, 1, PllRegData);
        for (i = 0; i < 0xfff; i++) {}  /* Wait for a while */

        pNewRegsVal->Ctrl.ForcePllReset = 0U;
        (void) AmbaB8_Wrap_memcpy(&PllRegData[0], &pNewRegsVal->Ctrl, sizeof(UINT32));
        (void) AmbaB8_RegWrite(ChipID, pPllCtrlReg, 1, B8_DATA_WIDTH_32BIT, 1, PllRegData);
    }

    /* reset and dsm reset is asserted, vco_halt = 1 */
    pNewRegsVal->Ctrl.ForcePllReset = 1U;
    pNewRegsVal->Ctrl.DsmReset = 1U;
    pNewRegsVal->Ctrl.HaltVCO = 1U;
    (void) AmbaB8_Wrap_memcpy(&PllRegData[0], &pNewRegsVal->Ctrl, sizeof(UINT32));
    (void) AmbaB8_RegWrite(ChipID, pPllCtrlReg, 1, B8_DATA_WIDTH_32BIT, 1, PllRegData);

    for (i = 0; i < 0xfff; i++) {}  /* After > 15 cycles of reference clock */

    /*-----------------------------------------------------------------------*\
     * Write the new Register Values
    \*-----------------------------------------------------------------------*/

    /* Write new value to fractional register (if required) */
    if (pPllFracReg != NULL) {
        PllRegData[0] = pNewRegsVal->Fraction;
        (void) AmbaB8_RegWrite(ChipID, pPllFracReg, 1, B8_DATA_WIDTH_32BIT, 1, PllRegData);

        if ((pNewRegsVal->Ctrl.FractionalMode != 0U) && (pNewRegsVal->Fraction == 0U)) {
            pNewRegsVal->Ctrl.FractionalMode = 0U;  /* Fraction = 0, use Integer mode, not Fractional mode */
        }
    } else {
        pNewRegsVal->Ctrl.FractionalMode = 0U;      /* Fraction = 0, only Integer mode */
    }

    /* Write new values to pll control register (int, sdiv & sout) with bit[0] set to 0 */
    pNewRegsVal->Ctrl.WriteEnable = 0U;
    (void) AmbaB8_Wrap_memcpy(&PllRegData[0], &pNewRegsVal->Ctrl, sizeof(UINT32));
    (void) AmbaB8_RegWrite(ChipID, pPllCtrlReg, 1, B8_DATA_WIDTH_32BIT, 1, PllRegData);

    if (pCurRegsVal->Ctrl.PowerDown != 0U) {
        for (i = 0; i < 0xfff; i++) {}  /* Wait for a while */
    }

    /* Re-write the pll control register with bit[0] set to 1 */
    pNewRegsVal->Ctrl.WriteEnable = 1U;
    (void) AmbaB8_Wrap_memcpy(&PllRegData[0], &pNewRegsVal->Ctrl, sizeof(UINT32));
    (void) AmbaB8_RegWrite(ChipID, pPllCtrlReg, 1, B8_DATA_WIDTH_32BIT, 1, PllRegData);

    if (pCurRegsVal->Ctrl.PowerDown != 0U) {
        for (i = 0; i < 0xfff; i++) {}  /* Wait for a while */
    }

    pNewRegsVal->Ctrl.WriteEnable = 0U;
    (void) AmbaB8_Wrap_memcpy(&PllRegData[0], &pNewRegsVal->Ctrl, sizeof(UINT32));
    (void) AmbaB8_RegWrite(ChipID, pPllCtrlReg, 1, B8_DATA_WIDTH_32BIT, 1, PllRegData);

    /* Expect slow divider clock to glitch when byp_jdiv is removed */
    pNewRegsVal->Ctrl3.BypassJDiv = 0U;

    /* Dsm state machine will need reset after dsmclk is switched back to fbclk */
    pNewRegsVal->Ctrl2.CtrlAbility = 0U;

    (void) AmbaB8_Wrap_memcpy(&PllRegData[0], &pNewRegsVal->Ctrl2, sizeof(UINT32));
    (void) AmbaB8_Wrap_memcpy(&PllRegData[1], &pNewRegsVal->Ctrl3, sizeof(UINT32));
    (void) AmbaB8_RegWrite(ChipID, pPllCtrl2Reg, 1, B8_DATA_WIDTH_32BIT, 2, PllRegData);

    /* Vco_halt go to 0 */
    pNewRegsVal->Ctrl.HaltVCO = 0U;
    (void) AmbaB8_Wrap_memcpy(&PllRegData[0], &pNewRegsVal->Ctrl, sizeof(UINT32));
    (void) AmbaB8_RegWrite(ChipID, pPllCtrlReg, 1, B8_DATA_WIDTH_32BIT, 1, PllRegData);

    for (i = 0; i < 0xfff; i++) {}  /* After 6 cycles of reference clock */

    /* Reset and dsm reset deasserted */
    pNewRegsVal->Ctrl.ForcePllReset = 0;
    pNewRegsVal->Ctrl.DsmReset = 0;
    (void) AmbaB8_Wrap_memcpy(&PllRegData[0], &pNewRegsVal->Ctrl, sizeof(UINT32));
    (void) AmbaB8_RegWrite(ChipID, pPllCtrlReg, 1, B8_DATA_WIDTH_32BIT, 1, PllRegData);

    for (i = 0; i < 0xfff; i++) {}  /* After 6 cycles of reference clock */

    /* Final step: disable the vco clamp */
    pNewRegsVal->Ctrl3.VcoClamp = 0U;
    (void) AmbaB8_Wrap_memcpy(&PllRegData[0], &pNewRegsVal->Ctrl3, sizeof(UINT32));
    (void) AmbaB8_RegWrite(ChipID, pPllCtrl3Reg, 1, B8_DATA_WIDTH_32BIT, 1, PllRegData);

    /* Wait 2ms for Lock Status */
    (void) AmbaKAL_TaskSleep(2);

    return RetStatus;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaCSL_B8PllPowerDown
 *
 *  @Description:: Power down the PLL (save power)
 *
 *  @Input      ::
 *      ChipID:    B6 chip id
 *      PllIdx:    Index of the PLL
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaCSL_B8PllPowerDown(UINT32 ChipID, UINT32 PllIdx)
{
    volatile const B8_PLL_CTRL_REG_s *pPllCtrlReg = B8_PLL_GetCtrlRegAddr(PllIdx);
    B8_PLL_CTRL_REG_s PllCtrlValue;

    (void) AmbaB8_RegRead(ChipID, pPllCtrlReg, 1, B8_DATA_WIDTH_32BIT, 1, &PllCtrlValue);
    PllCtrlValue.PowerDown = 1U;  /* Power Down the PLL: 0 - disable, 1 - enable */
    (void) AmbaB8_RegWrite(ChipID, pPllCtrlReg, 1, B8_DATA_WIDTH_32BIT, 1, &PllCtrlValue);

    return B8_ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaCSL_B8PllFractionalMode
 *
 *  @Description:: Set PLL fraction mode
 *
 *  @Input      ::
 *      ChipID:        B6 chip id
 *      PllIdx:        Index of the PLL
 *      UseFractional: Use fractional mode
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaCSL_B8PllFractionalMode(UINT32 ChipID, UINT32 PllIdx, UINT8 UseFractional)
{
    volatile const B8_PLL_CTRL_REG_s *pPllCtrlReg = B8_PLL_GetCtrlRegAddr(PllIdx);
    B8_PLL_CTRL_REG_s PllCtrlValue;

    (void) AmbaB8_RegRead(ChipID, pPllCtrlReg, 1, B8_DATA_WIDTH_32BIT, 1, &PllCtrlValue);
    PllCtrlValue.FractionalMode = UseFractional;
    (void) AmbaB8_RegWrite(ChipID, pPllCtrlReg, 1, B8_DATA_WIDTH_32BIT, 1, &PllCtrlValue);

    return B8_ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaCSL_B8PllCheckLockStatus
 *
 *  @Description:: Check PLL Lock Status
 *
 *  @Input      ::
 *      ChipID:    B6 chip id
 *      PllIdx:    Index of the PLL
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : OK(0) = Locked /NG(-1) = Not Lock yet
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaCSL_B8PllCheckLockStatus(UINT32 ChipID, UINT32 PllIdx)
{
    UINT32 RetVal = B8_ERR_NONE;
    UINT32 PllRegData;

    /* TBD */
    (void) AmbaB8_RegRead(ChipID, & (pAmbaB8_PllReg->PllLockStatus), 1, B8_DATA_WIDTH_32BIT, 1, &PllRegData);

    if (((PllIdx ==     B8_PLL_CORE) && ((PllRegData & 0x1U)   ==   0x1U)) ||
        ((PllIdx ==   B8_PLL_SENSOR) && ((PllRegData & 0x4U)   ==   0x4U)) ||
        ((PllIdx ==    B8_PLL_VIDEO) && ((PllRegData & 0x8U)   ==   0x8U)) ||
        ((PllIdx ==  B8_PLL_MPHY_RX) && ((PllRegData & 0x20U)  ==  0x20U)) ||
        ((PllIdx == B8_PLL_MPHY_TX0) && ((PllRegData & 0x100U) == 0x100U))) {
        RetVal = B8_ERR_NONE;
    } else {
        RetVal = B8_ERR_LOCK;
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaCSL_B8PllRescale
 *
 *  @Description:: Rescale Pll clock with new fraction scale
 *
 *  @Input      ::
 *      ChipID:        B8 chip id
 *      PllIdx:        Index of the PLL
 *      FrequencyDiff: Frequency difference
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : OK(0) = Locked /NG(-1) = Not Lock yet
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaCSL_B8PllRescale(UINT32 ChipID, UINT32 PllIdx, UINT32 RefFreq, INT32 FrequencyDiff)
{
    volatile const B8_PLL_CTRL_REG_s   *pPllCtrlReg = B8_PLL_GetCtrlRegAddr(PllIdx);
    volatile const UINT32              *pPllFracReg = B8_PLL_GetFracRegAddr(PllIdx);
    B8_PLL_CTRL_REGS_VAL_s CurPllCtrlRegsVal;
    B8_PLL_CTRL_REG_s NewCtrlRegsVal;
    UINT32 NewFractionRegsVal;
    DOUBLE FractionDiffVal;
    UINT32 CurFracSign, CurFracValue;
    INT64 TempFracValue;
    UINT32 Sout, Sdiv, PostDivider;
    UINT32 RetVal = B8_ERR_NONE;

    /* Get current PLL control registers' values */
    (void) AmbaCSL_B8PllGetCtrlRegsVal(ChipID, PllIdx, &CurPllCtrlRegsVal);

    /* Compute value of (PLL_XXX_FRAC_REG / 0x100000000) of frequency difference */
    Sout = (UINT32) CurPllCtrlRegsVal.Ctrl.Sout + 1U;
    Sdiv = (UINT32) CurPllCtrlRegsVal.Ctrl.Sdiv + 1U;
    PostDivider = AmbaCSL_B8PllGetPostScaleRegVal(ChipID, PllIdx);
    FractionDiffVal = ((((DOUBLE) FrequencyDiff * (DOUBLE) PostDivider) * (DOUBLE) Sout) / (DOUBLE) Sdiv) / (DOUBLE) RefFreq;

    /* FractionDiffVal needs to be -0.5 ~ 0.499999999767169, and avoiding unnecessary RCT programming if FractionDiffVal = 0 */
    if ((FractionDiffVal > ((DOUBLE) 0x7fffffff / (DOUBLE) 0x100000000)) || (FractionDiffVal < -0.5)) {
        RetVal = B8_ERR_ARG;
    }

    if (FractionDiffVal != 0.0) {
        NewCtrlRegsVal = CurPllCtrlRegsVal.Ctrl;
        CurFracValue = CurPllCtrlRegsVal.Fraction & 0x7fffffffU;
        CurFracSign = (CurPllCtrlRegsVal.Fraction >> 31U) & 0x1U;

        FractionDiffVal = FractionDiffVal * (DOUBLE) 0x100000000ULL;
        TempFracValue = (INT64) CurFracValue + (INT64) FractionDiffVal;

        if (CurFracSign == 0U) {
            if (TempFracValue > 0x7fffffff) {
                NewCtrlRegsVal.IntProg++;
                CurFracSign = 1U;
                TempFracValue -= 0x7fffffff;
            } else if (TempFracValue < 0) {
                CurFracSign = 1U;
                TempFracValue += 0x7fffffff;
            } else {
                AmbaB8_Misra_TouchUnused(&TempFracValue);
            }
        } else {
            if (TempFracValue > 0x7fffffff) {
                CurFracSign = 0U;
                TempFracValue -= 0x7fffffff;
            } else if (TempFracValue < 0) {
                CurFracSign = 0U;
                NewCtrlRegsVal.IntProg--;
                TempFracValue += 0x7fffffff;
            } else {
                AmbaB8_Misra_TouchUnused(&TempFracValue);
            }
        }

        NewFractionRegsVal = (CurFracSign << 31U) | (UINT32)((UINT32)TempFracValue & 0x7fffffffU);

        if (NewFractionRegsVal == 0U) {
            /* Fraction = 0, use Integer mode, not Fractional mode */
            NewCtrlRegsVal.FractionalMode = 0U;
        } else {
            (void) AmbaB8_RegWrite(ChipID, pPllFracReg, 1, B8_DATA_WIDTH_32BIT, 1, &NewFractionRegsVal);
        }

        (void) AmbaB8_RegWrite(ChipID, pPllCtrlReg, 1, B8_DATA_WIDTH_32BIT, 1, &NewCtrlRegsVal);

        //     NewCtrlRegsVal.Data |= 0x1;
        NewCtrlRegsVal.WriteEnable = 1U;
        (void) AmbaB8_RegWrite(ChipID, pPllCtrlReg, 1, B8_DATA_WIDTH_32BIT, 1, &NewCtrlRegsVal);

        //     NewCtrlRegsVal.Data &= 0xfffffffe;
        NewCtrlRegsVal.WriteEnable = 0U;
        (void) AmbaB8_RegWrite(ChipID, pPllCtrlReg, 1, B8_DATA_WIDTH_32BIT, 1, &NewCtrlRegsVal);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaCSL_B8PllSetInitCtrlRegsVal
 *
 *  @Description:: Set initialized clock frequency
 *
 *  @Input      ::
 *      ChipID:    B8 chip id (user can only control with one chip)
 *      PllIdx:    Index of the PLL
 *      UINT32:    new PLL Control register values
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
void AmbaCSL_B8PllSetPllCtrl(UINT32 ChipID, UINT32 PllIdx, const UINT32 *pPllCtrlRegsVal)
{
    volatile const B8_PLL_CTRL_REG_s   *pPllCtrlReg = B8_PLL_GetCtrlRegAddr(PllIdx);
    UINT32 RegVal;

    RegVal = *pPllCtrlRegsVal & (~0x1U);
    (void) AmbaB8_RegWrite(ChipID, pPllCtrlReg, 0, B8_DATA_WIDTH_32BIT, 1, &RegVal);
    RegVal = *pPllCtrlRegsVal | 1U;   /*write enable */
    (void) AmbaB8_RegWrite(ChipID, pPllCtrlReg, 0, B8_DATA_WIDTH_32BIT, 1, &RegVal);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaCSL_B8PllSetInitCtrl2RegsVal
 *
 *  @Description:: Set initialized clock frequency
 *
 *  @Input      ::
 *      ChipID:    B8 chip id (user can only control with one chip)
 *      PllIdx:    Index of the PLL
 *      UINT32:    new PLL Control register values
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
void AmbaCSL_B8PllSetPllCtrl2(UINT32 ChipID, UINT32 PllIdx, const UINT32 *pPllCtrlRegsVal)
{
    volatile const B8_PLL_CTRL2_REG_s *pPllCtrl2Reg = B8_PLL_GetCtrl2RegAddr(PllIdx);

    (void) AmbaB8_RegWrite(ChipID, pPllCtrl2Reg, 0, B8_DATA_WIDTH_32BIT, 1, pPllCtrlRegsVal);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaCSL_B8PllSetInitCtrl3RegsVal
 *
 *  @Description:: Set initialized clock frequency
 *
 *  @Input      ::
 *      ChipID:    B8 chip id (user can only control with one chip)
 *      PllIdx:      Index of the PLL
 *      UINT32:    new PLL Control register values
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
void AmbaCSL_B8PllSetPllCtrl3(UINT32 ChipID, UINT32 PllIdx, const UINT32 *pPllCtrlRegsVal)
{
    volatile const B8_PLL_CTRL3_REG_s *pPllCtrl3Reg = B8_PLL_GetCtrl3RegAddr(PllIdx);

    (void) AmbaB8_RegWrite(ChipID, pPllCtrl3Reg, 0, B8_DATA_WIDTH_32BIT, 1, pPllCtrlRegsVal);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaCSL_B8PllSetSwphyDiv
 *
 *  @Description:: Set Swphy clock Divider
 *
 *  @Input      ::
 *      ChipID:    B8 chip id (user can only control with one chip)
 *      Divider:  Clock Divider
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
void AmbaCSL_B8PllSetSwphyDiv(UINT32 ChipID, UINT32 Divider)
{
    (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_PllReg->CorePllSwPhyScaler), 0, B8_DATA_WIDTH_32BIT, 1, &Divider);
}

