/**
 *  @file AmbaVIN.h
 *
 *  Copyright (c) 2020 Ambarella International LP
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
 *  @details Definitions & Constants for Video Input Middleware APIs
 *
 */

#ifndef AMBA_VIN_H
#define AMBA_VIN_H

#include "AmbaVIN_Def.h"

#ifndef AMBA_SYS_DEF_H
#include "AmbaSYS_Def.h"
#endif

/*
 * Defined in AmbaVIN.c
 */
typedef struct {
    UINT8  GotActSof;               /* Detected start of active region */
    UINT8  GotWinSof;               /* Reached start of crop region */
    UINT8  GotWinEof;               /* Reached end of crop region */
    UINT8  GotVsync;                /* Reached end of active region (start of VBLANK) */
    UINT8  AfifoOverflow;           /* AFIFO overflow */
    UINT8  SfifoOverflow;           /* SFIFO has overflowed */
    UINT8  ShortLine;               /* EOL received before reaching end of programmed crop width */
    UINT8  ShortFrame;              /* EOF received before reaching end of programmed crop region */
    UINT8  WatchdogTimeout;         /* only avaliable for cv2fs/cv5 */

    UINT8  EccError2Bit;            /* 2-bit uncorrectable ECC error */
    UINT8  EccError1Bit;            /* 1-bit ECC error corrected */
    UINT8  CrcError;                /* Long Packet CRC Error */
    UINT8  CrcPhError;              /* C-PHY Packet Header CRC Error, only avaliable for cv5 */

    UINT8  FrameSyncError;          /* Frame sync error (Received unpaired SOF or EOF) */
    UINT8  SofPacketReceived;       /* SOF Short Packet Received */
    UINT8  EofPacketReceived;       /* EOF Short Packet Received */
    UINT8  SerialDeskewError;       /* only avaliable for cv2fs/cv5 MIPI/SLVS */

    UINT8 Uncorrectable656Error;    /* only available for SLVS and LVCMOS(ITU-656 sync type) */
    UINT8 PartialSyncDetected;      /* only available for SLVS and LVCMOS */
    UINT8 UnknownSyncCode;          /* only available for SLVS and LVCMOS */

    UINT8 SlvsecCrcError;           /* SLVSEC CRC Error, only avaliable for cv5 */
    UINT8 SlvsecLineBoundaryErr;    /* SLVSEC Line Boundary Error, only avaliable for cv5 */
    UINT8 SlvsecDelayedLineEndErr;  /* SLVSEC Delayed Line End Error, only avaliable for cv5 */
    UINT8 SlvsecTruncatedPacketErr; /* SLVSEC Truncated Packet Error, only avaliable for cv5 */
    UINT8 SlvsecIllegalStandbyErr;  /* SLVSEC Illegal Standby Error, only avaliable for cv5 */

    //Print follow SLVSEC reg TBD
    UINT8 SlvsecL0EccCorrect;       /* SLVSEC LINK0 ECC Correct, only avaliable for cv5 */
    UINT8 SlvsecL0EccCorreactable;  /* SLVSEC LINK0 ECC Correactable, only avaliable for cv5 */
    UINT8 SlvsecL0EccUncorrectable; /* SLVSEC LINK0 ECC Uncorrectable, only avaliable for cv5 */
    UINT8 SlvsecL1EccCorrect;       /* SLVSEC LINK1 ECC Correct, only avaliable for cv5 */
    UINT8 SlvsecL1EccCorreactable;  /* SLVSEC LINK1 ECC Correactable, only avaliable for cv5 */
    UINT8 SlvsecL1EccUncorrectable; /* SLVSEC LINK1 ECC Uncorrectable, only avaliable for cv5 */

} IODIAG_VIN_STATUS_s;

UINT32 AmbaVIN_SlvsReset(UINT32 VinID, const AMBA_VIN_SLVS_PAD_CONFIG_s *pPadConfig);
UINT32 AmbaVIN_MipiReset(UINT32 VinID, const AMBA_VIN_MIPI_PAD_CONFIG_s *pPadConfig);
UINT32 AmbaVIN_DvpReset(UINT32 VinID, const AMBA_VIN_DVP_PAD_CONFIG_s *pPadConfig);
UINT32 AmbaVIN_SlvsConfig(UINT32 VinID, const AMBA_VIN_SLVS_CONFIG_s *pVinSlvsConfig);
UINT32 AmbaVIN_MipiConfig(UINT32 VinID, const AMBA_VIN_MIPI_CONFIG_s *pVinMipiConfig);
UINT32 AmbaVIN_DvpConfig(UINT32 VinID, const AMBA_VIN_DVP_CONFIG_s *pVinDvpConfig);

UINT32 AmbaVIN_MasterSyncEnable(UINT32 MSyncID, const AMBA_VIN_MASTER_SYNC_CONFIG_s *pMSyncConfig);
UINT32 AmbaVIN_MasterSyncDisable(UINT32 MSyncID);

UINT32 AmbaVIN_SetLvdsTermination(UINT32 VinID, UINT32 Value);

UINT32 AmbaVIN_SensorClkEnable(UINT32 ClkID, UINT32 Frequency);
UINT32 AmbaVIN_SensorClkDisable(UINT32 ClkID);
UINT32 AmbaVIN_SensorClkSetDrvStrength(UINT32 Value);

UINT32 AmbaVIN_DataLaneRemap(UINT32 VinID, const AMBA_VIN_LANE_REMAP_CONFIG_s *pLaneRemapConfig);
UINT32 AmbaVIN_MipiVirtChanConfig(UINT32 VinID, const AMBA_VIN_MIPI_VC_CONFIG_s *pMipiVirtChanConfig);

UINT32 AmbaVIN_GetMainCfgBufInfo(UINT32 VinID, ULONG *pAddr, UINT32 *pSize);
UINT32 AmbaVIN_CaptureConfig(UINT32 VinID, const AMBA_VIN_WINDOW_s *pCaptureWindow);
UINT32 AmbaVIN_GetInfo(UINT32 VinID, AMBA_VIN_INFO_s *pInfo);
UINT32 AmbaVIN_IsEnabled(UINT32 VinID);

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
UINT32 AmbaVIN_MipiCphyReset(UINT32 VinID, const AMBA_VIN_MIPI_CPHY_PAD_CONFIG_s *pPadConfig);
UINT32 AmbaVIN_MipiCphyConfig(UINT32 VinID, const AMBA_VIN_MIPI_CONFIG_s *pVinMipiCphyConfig);
UINT32 AmbaVIN_SlvsEcReset(UINT32 VinID, const AMBA_VIN_SLVSEC_PAD_CONFIG_s *pPadConfig);
UINT32 AmbaVIN_SlvsEcCalib(UINT32 VinID, const AMBA_VIN_SLVSEC_PAD_CONFIG_s *pPadConfig);
UINT32 AmbaVIN_SlvsEcConfig(UINT32 VinID, const AMBA_VIN_SLVSEC_CONFIG_s *pVinSlvsEcConfig);
#endif

/* For debugging purpose */
UINT32 AmbaDiag_VinShowInfo(UINT32 VinID, AMBA_SYS_LOG_f LogFunc);
UINT32 AmbaDiag_VinResetStatus(UINT32 VinID, AMBA_SYS_LOG_f LogFunc);
UINT32 AmbaDiag_VinConfigThrDbgPort(UINT32 VinID, UINT32 CaptureWidth, UINT32 CaptureHeight, AMBA_SYS_LOG_f LogFunc);
UINT32 AmbaDiag_VinGetStatus(UINT32 VinID, IODIAG_VIN_STATUS_s *pVinStatus);

#endif /* _AMBA_VIN_H_ */
