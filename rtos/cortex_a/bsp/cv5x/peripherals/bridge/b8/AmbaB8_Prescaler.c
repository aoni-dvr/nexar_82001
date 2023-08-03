/**
 *  @file AmbaB8_Prescaler.c
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
 *  @details Ambarella B8 Prescaler APIs
 *
 */
#include "AmbaB8.h"
#include "AmbaB8_Prescaler.h"
#include "AmbaB8_Communicate.h"

#include "AmbaB8CSL_Prescaler.h"


#define INITIAL_PHASE_ROUNDING                  256
#define IDSP_RGB_SAMPLE_MODE_BIN2                 1
#define IDSP_RGB_SAMPLE_MODE_SKIP2                2
#define IDSP_RGB_SAMPLE_MODE_BIN2_SKIP2           3
#define IDSP_RGB_SAMPLE_MODE_SUM2_SKIP2           4
#define IDSP_RGB_SAMPLE_MODE_BIN4                 5

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_PrescalerConfig
 *
 *  @Description:: Configure B8 Prescaler
 *
 *  @Input      ::
 *     ChipID:           B8 chip id
 *     Channel:          B8 Vin channel
 *     pPrescalerConfig: Pointer to configuration
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      UINT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_PrescalerConfig(UINT32 ChipID, UINT32 Channel, B8_PRESCALER_CONFIG_s* pPrescalerConfig)
{
    UINT32 RetVal = B8_ERR_NONE;
    UINT32 i;
    UINT32 Shift = 0;
    INT32 PhaseFirstColor = 0;
    INT32 PhaseSecondColor = 0;
    UINT16 DataBuf16[4];
    INT32 RoundingVal;
    UINT32 RoundingValU;

    static UINT16 DefaultCoefTable[] = {
        0x0000, 0x017f, 0x0000,    /* Phase Coef  0 */
        0xfb01, 0x067f, 0x00ff,    /* Phase Coef  1 */
        0xf701, 0x0e7c, 0x00fe,    /* Phase Coef  2 */
        0xf402, 0x1678, 0x00fc,    /* Phase Coef  3 */
        0xf202, 0x2072, 0x00fa,    /* Phase Coef  4 */
        0xf202, 0x2a68, 0x01f9,    /* Phase Coef  5 */
        0xf202, 0x355f, 0x01f7,    /* Phase Coef  6 */
        0xf202, 0x4056, 0x01f5,    /* Phase Coef  7 */
        0xf401, 0x4b4b, 0x01f4,    /* Phase Coef  8 */
        0xf501, 0x5640, 0x02f2,    /* Phase Coef  9 */
        0xf701, 0x5f35, 0x02f2,    /* Phase Coef 10 */
        0xf901, 0x682a, 0x02f2,    /* Phase Coef 11 */
        0xfa00, 0x7220, 0x02f2,    /* Phase Coef 12 */
        0xfc00, 0x7816, 0x02f4,    /* Phase Coef 13 */
        0xfe00, 0x7c0e, 0x01f7,    /* Phase Coef 14 */
        0xff00, 0x7f06, 0x01fb,    /* Phase Coef 15 */
    };


    /* Do not support scaling up */
    if (pPrescalerConfig->InputWidth < pPrescalerConfig->OutputWidth) {
        pPrescalerConfig->OutputWidth = pPrescalerConfig->InputWidth;
    }
    /* Ctrl */
    if (pPrescalerConfig->InputWidth == pPrescalerConfig->OutputWidth) {
        DataBuf16[0] = 0x0;
    } else {
        DataBuf16[0] = 0x1;
    }

    /* Output Width */
    DataBuf16[1] = pPrescalerConfig->OutputWidth - 1U;
    /* Phase Increment */
    DataBuf16[2] = (UINT16)((0x2000U * pPrescalerConfig->InputWidth) / pPrescalerConfig->OutputWidth);

    RetVal |= AmbaB8_RegWrite(ChipID, & pAmbaB8_PrescalerReg[Channel]->Ctrl, 1U, B8_DATA_WIDTH_16BIT, 1U, &DataBuf16[0]);
    RetVal |= AmbaB8_RegWrite(ChipID, & pAmbaB8_PrescalerReg[Channel]->OutputWidth, 1U, B8_DATA_WIDTH_16BIT, 2U, &DataBuf16[1]);

    /* Enable Horizontal Downsampling */
    if (DataBuf16[0] != 0U) {
        /* Select formula */
        switch (pPrescalerConfig->ReadoutMode) {
        case 0U:
            Shift = ((((UINT32)0x2000U * pPrescalerConfig->InputWidth) / pPrescalerConfig->OutputWidth) - ((UINT32)1U << 13U)) / 2U;
            PhaseFirstColor -= (INT32)Shift / 2;
            PhaseSecondColor += (INT32)Shift / 2;
            break;
        case IDSP_RGB_SAMPLE_MODE_BIN2:
            Shift = (((((UINT32)2U * 0x2000U) * pPrescalerConfig->InputWidth) / pPrescalerConfig->OutputWidth) - ((UINT32)1U << 13U)) / 4U;
            PhaseFirstColor -= (INT32)Shift / 2;
            PhaseSecondColor += (INT32)Shift / 2;
            break;
        case IDSP_RGB_SAMPLE_MODE_SKIP2:
            Shift = (((((UINT32)2U * 0x2000U) * pPrescalerConfig->InputWidth) / pPrescalerConfig->OutputWidth) - ((UINT32)1U << 13U)) / 4U;
            PhaseFirstColor += (INT32)Shift / 2;
            PhaseSecondColor -= (INT32)Shift / 2;
            break;
        case IDSP_RGB_SAMPLE_MODE_BIN2_SKIP2:
            Shift = (((((UINT32)4U * 0x2000U) * pPrescalerConfig->InputWidth) / pPrescalerConfig->OutputWidth) - ((UINT32)3U << 13U)) / 8U;
            PhaseFirstColor += (INT32)Shift / 2;
            PhaseSecondColor -= (INT32)Shift / 2;
            break;
        case IDSP_RGB_SAMPLE_MODE_SUM2_SKIP2:
            Shift = (((((UINT32)4U * 0x2000U) * pPrescalerConfig->InputWidth) / pPrescalerConfig->OutputWidth) - ((UINT32)3U << 13U)) / 8U;
            PhaseFirstColor -= (INT32)Shift / 2;
            PhaseSecondColor += (INT32)Shift / 2;
            break;
        case IDSP_RGB_SAMPLE_MODE_BIN4:
            Shift = (((((UINT32)4U * 0x2000U) * pPrescalerConfig->InputWidth) / pPrescalerConfig->OutputWidth) - ((UINT32)1U << 13U)) / 8U;
            PhaseFirstColor -= (INT32)Shift / 2;
            PhaseSecondColor += (INT32)Shift / 2;
            break;
        case 6U:
            Shift = (((((UINT32)4U * 0x2000U) * pPrescalerConfig->InputWidth) / pPrescalerConfig->OutputWidth) - ((UINT32)1U << 13U)) / 8U;
            PhaseFirstColor += (INT32)Shift / 2;
            PhaseSecondColor -= (INT32)Shift / 2;
            break;
        default:
            Shift = ((((UINT32)0x2000U * pPrescalerConfig->InputWidth) / pPrescalerConfig->OutputWidth) - ((UINT32)1U << 13U)) / 2U;
            PhaseFirstColor -= (INT32)Shift / 2;
            PhaseSecondColor += (INT32)Shift / 2;
            break;
        }

        /* Rounding */
        RoundingVal = (PhaseFirstColor + INITIAL_PHASE_ROUNDING);
        AmbaMisra_TypeCast32(&RoundingValU, &RoundingVal);
        DataBuf16[0] = (UINT16)(RoundingValU >> 13U);
        DataBuf16[1] = (UINT16)(RoundingValU & 0x1fffU);
        RoundingVal = (PhaseSecondColor + INITIAL_PHASE_ROUNDING);
        AmbaMisra_TypeCast32(&RoundingValU, &RoundingVal);
        DataBuf16[2] = (UINT16)(RoundingValU >> 13U);
        DataBuf16[3] = (UINT16)(RoundingValU & 0x1fffU);

        RetVal |= AmbaB8_RegWrite(ChipID, & pAmbaB8_PrescalerReg[Channel]->FirstColorPhaseInt, 1U, B8_DATA_WIDTH_16BIT, 4U, &DataBuf16[0]);

        for (i = 0U; i < 16U; i++) {
            /* Horizontal coefficients */
            DataBuf16[0] = DefaultCoefTable[((4U - 1U) * i)];
            DataBuf16[1] = DefaultCoefTable[((4U - 1U) * i) + 1U];
            DataBuf16[2] = DefaultCoefTable[((4U - 1U) * i) + 2U];

            RetVal |= AmbaB8_RegWrite(ChipID, & pAmbaB8_PrescalerReg[Channel]->PhaseCoef[i], 1U, B8_DATA_WIDTH_16BIT, 3U, &DataBuf16[0]);
        }
    }

    /* Config done */
    DataBuf16[0] = 0x7U;
    RetVal |= AmbaB8_RegWrite(ChipID, & pAmbaB8_PrescalerReg[Channel]->UpdateDone, 1U, B8_DATA_WIDTH_16BIT, 1, DataBuf16);

    return RetVal;
}

