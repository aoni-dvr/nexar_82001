/**
*  @file AmbaDSP_IntCheck.h
*
 * Copyright (c) 2020 Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella International LP.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
*  @details Definitions & Constants for chip bonding check
*
*/

#ifndef AMBADSP_INTCHECK_H
#define AMBADSP_INTCHECK_H

#include "AmbaDSP.h"

#define AMBA_UNLIMIT_SERIES           (0U)
#define AMBA_A_SERIES                 (1U)
#define AMBA_U_SERIES                 (2U)
#define AMBA_AX_SERIES                (3U)
#define AMBA_AQ_SERIES                (4U)
#define AMBA_SERIES_NUM               (5U)

#define AMBA_SERIES_MAX_NUM           (6U) /* Defined by different Chip: Max(Series Value) + 1U */

#define PART_NUMBER_UNLIMIT           (0U)
#define PART_NUMBER_15                (1U)
#define PART_NUMBER_25                (2U)
#define PART_NUMBER_35                (3U)
#define PART_NUMBER_55                (4U)
#define PART_NUMBER_70                (5U)
#define PART_NUMBER_75                (6U)
#define PART_NUMBER_80                (7U)
#define PART_NUMBER_85                (8U)
#define PART_NUMBER_MAX               (9U)

#define BONDING_PIXEL_CLOCK           (0U)
#define BONDING_SEC_STREAM            (1U) /* TBD */
#define BONDING_LISO_PRI_STREAM       (2U) /* TBD */
#define BONDING_HISO_PRI_STREAM       (3U) /* TBD */
#define BONDING_2ND_VIN_PRI_STREAM    (4U) /* TBD */
#define BONDING_2ND_VIN_SEC_STREAM    (5U) /* TBD */
#define BONDING_EIS_DRONE             (6U) /* TBD */
#define BONDING_EIS_SPORT             (7U) /* TBD */
#define BONDING_ENCODE_FMT            (8U) /* TBD */
#define BONDING_MUTLI_VIN             (9U) /* TBD */
#define BONDING_CHECK_TYPE            (10U)

typedef struct
{
    UINT8 Type;
    UINT32                  PartNumber;
    AMBA_DSP_WINDOW_s       *Window;
    AMBA_DSP_FRAME_RATE_s   *FrameRate;
    UINT32                  U32CheckValue;
} HL_DSP_BONDING_MSG_s;

typedef UINT32 (*HL_BONDING_PARSER_f)(const void *pMsgInfo);

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaHL_IntChk.c
\*-----------------------------------------------------------------------------------------------*/

/**
 * BondingCheck for pixel clock
 * @param [in] TotalPixel Total pixel for all YuvStream
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 HL_Int00Check(UINT32 TotalPixel);

/**
 * BondingCheck for Secondary stream
 * @param [in] Window The DSP window
 * @return 0 - OK, others - ErrorCode
 */
//extern UINT32 HL_Int01Check(AMBA_DSP_WINDOW_s *Window);

/**
 * BondingCheck for LISO Primary stream
 * @param [in] Window The DSP window
 * @param [in] FrameRateInfo The frame rate info
 * @return 0 - OK, others - ErrorCode
 */
//extern UINT32 HL_Int02Check(AMBA_DSP_WINDOW_s *Window,AMBA_DSP_FRAME_RATE_s *FrameRateInfo);

/**
 * BondingCheck for HISO stream
 * @param [in] Window The DSP window
 * @param [in] FrameRateInfo The frame rate info
 * @return 0 - OK, others - ErrorCode
 */
//extern UINT32 HL_Int03Check(AMBA_DSP_WINDOW_s *Window,AMBA_DSP_FRAME_RATE_s *FrameRateInfo);

/**
 * BondingCheck for 2ndVin main stream
 * @param [in] Window The DSP window
 * @param [in] FrameRateInfo The frame rate info
 * @return 0 - OK, others - ErrorCode
 */
//extern UINT32 HL_Int04Check(AMBA_DSP_WINDOW_s *Window,AMBA_DSP_FRAME_RATE_s *FrameRateInfo);

/**
 * BondingCheck for 2Ch main stream
 * @param [in] Window The DSP window
 * @param [in] FrameRateInfo The frame rate info
 * @return 0 - OK, others - ErrorCode
 */
//extern UINT32 HL_Int05Check(AMBA_DSP_WINDOW_s *Window,AMBA_DSP_FRAME_RATE_s *FrameRateInfo);

/**
 * BondingCheck for EIS
 * @param [in] Window The DSP window
 * @param [in] FrameRateInfo The frame rate info
 * @return 0 - OK, others - ErrorCode
 */
//extern UINT32 HL_Int06Check(AMBA_DSP_WINDOW_s *Window, AMBA_DSP_FRAME_RATE_s *FrameRateInfo);

/**
 * BondingCheck for EIS sport
 * @param [in] Window The DSP window
 * @param [in] FrameRateInfo The frame rate info
 * @return 0 - OK, others - ErrorCode
 */
//extern UINT32 HL_Int07Check(AMBA_DSP_WINDOW_s *Window, AMBA_DSP_FRAME_RATE_s *FrameRateInfo);

/**
 * BondingCheck for encode format
 * @param [in] Window The DSP window
 * @param [in] FrameRateInfo The frame rate info
 * @return 0 - OK, others - ErrorCode
 */
//extern UINT32 HL_Int08Check(AMBA_DSP_WINDOW_s *Window, AMBA_DSP_FRAME_RATE_s *FrameRateInfo);

/**
 * BondingCheck for MutliVin support
 * @param [in] ActiveVinBit Active Vin bit
 * @return 0 - OK, others - ErrorCode
 */
//extern UINT32 HL_Int09Check(UINT32 ActiveVinBit);

#endif /* AMBADSP_INTCHECK_H */
