/**
 *  @file AmbaSTU_PGMPacker.h
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
 *  @details stereo utility
 *
 */
#ifndef AMBA_STU_PGM_PACKER_H
#define AMBA_STU_PGM_PACKER_H
#include "AmbaTypes.h"
#include "AmbaCalib_EmirrorDef.h"

#define PGM_BIT16_HEADER "P5\n"
#define PGM_BIT16_HEADER_SIZE (4U)

#define PGM_BASELINE_HEADER "#DISPARITY BASELINE = "
#define PGM_BASELIBE_HEADER_SIZE (23U)

#define PGM_DSI_FMT_HEADER "#DISPARITY FORMAT = "
#define PGM_DSI_FMT_HEADER_SIZE (21U)

#define PGM_SCALED_FOCAL_LENGTH "#DISPARITY FOCAL LENGTH = "
#define PGM_SCALED_FOCAL_LENGTH_SIZE (27U)

#define PGM_OPTICAL_CENTER_HEADER "#DISPARITY OPTICAL CENTER = "
#define PGM_OPTICAL_CENTER_HEADER_SIZE (29U)

#define PGM_PROJECTION_MODEL_HEADER "#DISPARITY PROJECTION MODEL = "
#define PGM_PROJECTION_MODEL_HEADER_SIZE (31U)

#define PGM_SCALE_HEADER "#DISPARITY SCALE = "
#define PGM_SCALE_HEADER_SIZE (20U)

void STU_PGMGen16BitHeader(UINT8 *pPGMDataBuf, UINT32 Sof, UINT32 *pEof);
UINT32 STU_PGMGenBaselineHeader(UINT8 *pPGMDataBuf, UINT32 Sof, DOUBLE Baseline, UINT32 *pEof);
UINT32 STU_PGMGenScaledFocalLength(UINT8 *pPGMDataBuf, UINT32 Sof, DOUBLE ScaledFocalLengthRight, DOUBLE ScaledFocalLengthLeft, UINT32 *pEof);
UINT32 STU_PGMGenDsiFmtHeader(UINT8 *pPGMDataBuf, UINT32 Sof, UINT32 IntegerBitNum, UINT32 DecimalBitNum, UINT32 *pEof);
UINT32 STU_PGMGenResolutionHeader(UINT8 *pPGMDataBuf, UINT32 Sof, UINT32 Width, UINT32 Height, UINT32 Depth, UINT32 *pEof);
UINT32 STU_PGMGenOpticalCenter(UINT8 *pPGMDataBuf, UINT32 Sof, const AMBA_CAL_POINT_DB_2D_s *pOpticalCenter, UINT32 *pEof);
UINT32 STU_PGMGenProjectionModel(UINT8 *pPGMDataBuf, UINT32 Sof, AMBA_CAL_EM_PROJECTION_MODEL_e ProjectionModel, UINT32 *pEof);
UINT32 STU_PGMGenScale(UINT8 *pPGMDataBuf, UINT32 Sof, UINT32 Scale, UINT32 *pEof);
UINT32 STU_PGMReverseDsi(UINT8 *pPGMDataBuf, UINT32 Sof, const UINT16 *pDsiTbl, UINT32 DsiTblSize, UINT32 *pEof);
#endif
