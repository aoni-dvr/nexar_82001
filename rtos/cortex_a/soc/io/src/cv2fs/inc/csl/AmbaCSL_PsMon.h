/**
 *  @file AmbaCSL_PsMon.h
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
 *  @details Definitions & Constants for Power Monitor CSL APIs
 *
 */

#ifndef AMBA_CSL_POWER_MON_H
#define AMBA_CSL_POWER_MON_H

#ifndef AMBA_REG_RCT_H
#include "AmbaReg_RCT.h"
#endif

/*
 * Inline Function Definitions
 */

typedef struct {
    UINT8  LowGearDetUVLevel;     /* Low gear Detector UV level select control */
    UINT8  LowGearDetOVLevel;     /* Low gear Detector OV level select control */
    UINT8  HighGearDet1UVLevel;   /* High gear Detector 1 UV level select control */
    UINT8  HighGearDet1OVLevel;   /* High gear Detector 1 OV level select control */
    UINT8  HighGearDet2UVLevel;   /* High gear Detector 2 UV level select control */
    UINT8  HighGearDet2OVLevel;   /* High gear Detector 2 OV level select control */
    UINT8  FlagClearError;        /* Clear Error */
} AMBA_POWER_MON_GEAR_DET_SEL_s;

typedef struct {
    UINT8  vsenselosel;           /* Low gear vsense select control */
    UINT8  vsensehi1sel;          /* High gear 1 vsense select control */
    UINT8  vsensehi2sel;          /* High gear 2 vsense select control */
} AMBA_POWER_MON_VSENCE_SEL_s;

typedef struct {
    UINT8  PdDecLo;               /* DetectorLo power down control */
    UINT8  PdDechi1;              /* DetectorHi1 power down control */
    UINT8  PdDechi2;              /* DetectorHi2 power down control */
    UINT8  PdRef;                 /* Vrefgen power down control */
} AMBA_POWER_MON_DETECTOR_POWER_DOWN_s;

typedef struct {
    UINT8  AbistEn;               /* PSMON ABIST enable control */
    UINT8  AbistSel;              /* PSMON ABIST select control */
    UINT8  AbistMan;              /* PSMON manual mode control */
    UINT8  AbistRstn;             /* PSMON logic resetn */
    UINT8  AbistWindow;           /* ABIST comparator settle time window control */
    UINT8  AbistToggle;           /* PSMON ABIST check pattern toggle time number control */
    UINT8  PmDisable;             /* PSMON disable control */
    UINT8  SfWait;                /* PSMON SF waiting time for analog part */
    UINT8  PdPsmon;               /* PSMON whole analog part power down */
    UINT8  SfEnable;              /* PSMON SF disable control */
    UINT8  AbistBrek;             /* ignore ABIST state, directly SF outputs */
    UINT8  UvovSwap;              /* PSMON UV/OV sensing voltage swap */
} AMBA_POWER_MON_ABIST_s;

typedef struct {
    AMBA_POWER_MON_GEAR_DET_SEL_s        PmGearDetSel;        /* Gear Detector Control */
    AMBA_POWER_MON_VSENCE_SEL_s          PmVsenceSel;         /* Gear Vsence Control */
    AMBA_POWER_MON_DETECTOR_POWER_DOWN_s PmDetPowerDown;      /* Detector Power Down Control */
    AMBA_POWER_MON_ABIST_s               PmAbist;             /* ABIST control */
} AMBA_PM_CTRL_REGS_VAL_s;

typedef struct {
    AMBA_POWER_MON_GEAR_DET_SEL_s        PmGearDetSel;        /* Gear Detector Control */
    AMBA_POWER_MON_VSENCE_SEL_s          PmVsenceSel;         /* Gear Vsence Control */
    AMBA_POWER_MON_DETECTOR_POWER_DOWN_s PmDetPowerDown;      /* Detector Power Down Control */
    AMBA_POWER_MON_ABIST_s               PmAbist;             /* ABIST control */
} AMBA_PM_STATUS_VAL_s;

/*
 * Defined in AmbaCSL_CLK_Monitor.c
 */
void AmbaCSL_PmSetCtrlRegsVal(UINT32 PmIdx, const AMBA_PM_CTRL_REGS_VAL_s *pNewRegsVal);
UINT32 AmbaCSL_PmGetErrorStatus(UINT32 PmIdx);

#endif /* AMBA_CSL_POWER_MON_H */
