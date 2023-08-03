/**
 *  @file AmbaRTSL_PoMon.h
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
 *  @details Definitions & Constants for POWER SUPPLY MONITOR RTSL APIs
 *
 */

#ifndef AMBA_RTSL_PSMON_H
#define AMBA_RTSL_PSMON_H

typedef struct {
    UINT8  vsenselosel;     /* Low gear vsense select control */
    UINT8  vsensehi1sel;    /* High gear 1 vsense select control */
    UINT8  vsensehi2sel;    /* High gear 2 vsense select control */
    UINT8  PdDecLo;         /* DetectorLo power down control */
    UINT8  PdDechi1;        /* DetectorHi1 power down control */
    UINT8  PdDechi2;        /* DetectorHi2 power down control */
    UINT8  FlagClearError;  /* Clear error state of the monitor */
} AMBA_POWER_MON_CONFIG_s;

/*
 * Defined in AmbaRTSL_PsMon.c
 */
UINT32 AmbaRTSL_PmSetCtrl(UINT32 PmIdx, const AMBA_POWER_MON_CONFIG_s *pPsMonConfig);
UINT32 AmbaRTSL_PmGetErrorStatus(UINT32 PmIdx, UINT32 *pPmErrorStatus);

#endif /* AMBA_RTSL_PSMON_H */
