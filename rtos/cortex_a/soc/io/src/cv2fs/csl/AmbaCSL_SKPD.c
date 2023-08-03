/**
 *  @file AmbaCSL_SKPD.c
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
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
 *  @details Scratchpad (Power Control Circuits) CSL Device Driver
 *
 */

#include "AmbaTypes.h"
#include "AmbaCSL_Scratchpad.h"
#include "AmbaCSL_VIN.h"

/**
 *  AmbaCSL_SkpdGetScratchpadData - Get Scratchpad User Defined Data
 *  @param[in] BitMask Data bits to be cleared
 */
UINT32 AmbaCSL_SkpdGetScratchpadData(UINT32 Index)
{
    UINT32 RetVal = 0U;
    if (Index < NUM_SKPD_REG) {
        RetVal = pAmbaScratchpadNS_Reg->AhbScratchpad[Index];
    }
    return RetVal;
}

/**
 *  AmbaCSL_SkpdSetScratchpadData - Set Scratchpad User Defined Data
 *  @param[in] BitMask Data bits to be cleared
 */
void AmbaCSL_SkpdSetScratchpadData(UINT32 Index, UINT32 Data)
{
    if (Index < NUM_SKPD_REG) {
        pAmbaScratchpadNS_Reg->AhbScratchpad[Index] = Data;
    }
}

/*
 *  @RoutineName:: AmbaCSL_SetVsdelaySrc
 *
 *  @Description:: Select input H/Vsync source
 *
 *  @Input      ::
 *      VsdelaySrc: Input source selection
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *       UINT32 :  VIN_ERR_NONE(0) / NG
 */

UINT32 AmbaCSL_SetVsdelaySrc(UINT32 VsdelaySrc)
{
    UINT32 RetVal = VIN_ERR_NONE;

    if (VsdelaySrc == AMBA_VIN_VSDLY_SRC_HV_MSYNC0) {
        AmbaCSL_SetVsdelayVsyncVin();
    } else if (VsdelaySrc == AMBA_VIN_VSDLY_SRC_HV_MSYNC1) {
        AmbaCSL_SetVsdelayVsyncPip();
    } else if (VsdelaySrc == AMBA_VIN_VSDLY_SRC_H_MSYNC0_V_EXT) {
        AmbaCSL_SetVsdelayVsyncVin();    /* Hsync source from MSYNC0 */
        AmbaCSL_SetVsdelayVsyncExt();    /* External Vsync source */
    } else if (VsdelaySrc == AMBA_VIN_VSDLY_SRC_H_MSYNC1_V_EXT) {
        AmbaCSL_SetVsdelayVsyncPip();    /* Hsync source from MSYNC1 */
        AmbaCSL_SetVsdelayVsyncExt();    /* External Vsync source */
    } else {
        RetVal = VIN_ERR_ARG;
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaCSL_SetVsdelayWidth
 *
 *  @Description:: Set H/Vsync pulse width
 *
 *  @Input      ::
 *      HsPulseWidth: Hsync pulse width
 *      *VsPulseWidth: Vsync0/Vsync1 pulse width
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */

void AmbaCSL_SetVsdelayWidth(const UINT32 HsPulseWidth, const UINT32 *VsPulseWidth)
{
    if (HsPulseWidth == 0U) {
        AmbaCSL_SetVsdelayHsyncWidthSrc(AMBA_VIN_VSDLY_WIDTH_AS_INPUT);
    } else {
        AmbaCSL_SetVsdelayHsyncWidthSrc(AMBA_VIN_VSDLY_WIDTH_MANUAL);
        AmbaCSL_SetVsdelayHsyncWidth(HsPulseWidth);
    }
    if (VsPulseWidth[0] == 0U) {
        AmbaCSL_SetVsdelayVsync0WidthSrc(AMBA_VIN_VSDLY_WIDTH_AS_INPUT);
    } else {
        AmbaCSL_SetVsdelayVsync0WidthSrc(AMBA_VIN_VSDLY_WIDTH_MANUAL);
        AmbaCSL_SetVsdelayVsync0Width(VsPulseWidth[0]);
    }
    if (VsPulseWidth[1] == 0U) {
        AmbaCSL_SetVsdelayVsync1WidthSrc(AMBA_VIN_VSDLY_WIDTH_AS_INPUT);
    } else {
        AmbaCSL_SetVsdelayVsync1WidthSrc(AMBA_VIN_VSDLY_WIDTH_MANUAL);
        AmbaCSL_SetVsdelayVsync1Width(VsPulseWidth[1]);
    }
}

