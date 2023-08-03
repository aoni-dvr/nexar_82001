/**
 *  @file AmbaB8_Calibration.h
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
 *  @details Definitions & Constants for B6 APIs
 *
 */

#ifndef AMBA_B8_SERDES_CALIBRATION_H
#define AMBA_B8_SERDES_CALIBRATION_H

/*---------------------------------------------------------------------------*\
 * Parameters Configuration
\*---------------------------------------------------------------------------*/
typedef struct {
    UINT32 Cap;        /* CTLE 0x5268[11:0] */      /* 0x718[3:0] */
    UINT32 Res;        /* CTLE 0x5268[17:12] */     /* 0x718[7:4] */
    UINT32 Tap1;       /* DFE  0x525c[4:0] */       /* 0x730[3:0] */
    UINT32 Tap2;       /* DFE  0x525c[11:8] */      /* 0x730[7:4] */
    UINT32 Tap3;       /* DFE  0x525c[18:16] */     /* NA */
    UINT32 Tap4;       /* DFE  0x525c[22:20] */     /* NA */
} GLOBAL_SEARCH_PARAM_s;

/*---------------------------------------------------------------------------*\
 * Defined in AmbaB8_SerdesCalibration.c
\*---------------------------------------------------------------------------*/
UINT32 AmbaB8_SerdesGlobalCalibration(UINT32 ChipID, UINT32 SerDesRate, UINT32 TimeInterval, void *pMemBase);
UINT32 AmbaB8_SerdesCalibration(UINT32 ChipID, UINT32 SamplePeriod, GLOBAL_SEARCH_PARAM_s *pCalibParam);
UINT32 AmbaB8_SerdesSetCalibParam(UINT32 ChipID, const GLOBAL_SEARCH_PARAM_s *pCalibParam);
UINT32 AmbaB8_SerdesSweepCalibration(UINT32 ChipID, UINT32 SerDesRate, UINT32 TimeInterval);

#endif /* AMBA_B8_SERDES_CALIBRATION_H */
