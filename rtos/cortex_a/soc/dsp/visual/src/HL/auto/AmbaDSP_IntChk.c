/**
 *  @file AmbaHL_IntChk.c
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
 *  @details Implementation of SSP Bonding Check related APIs
 *
 */

#include "AmbaDSP_Log.h"
#include "AmbaDSP_IntChk.h"
#include "AmbaDSP_ContextUtility.h"

/****************
 * PaserType
 * (00) Unlimit
 * (01) CVXA
 * (02) CVXU
 * (03) CVXAX
 * (04) CVXAQ
 ****************/
static UINT32 ParserType00Part(const void *pMsgInfo);
static UINT32 ParserType01Part(const void *pMsgInfo);
static UINT32 ParserType02Part(const void *pMsgInfo);
static UINT32 ParserType03Part(const void *pMsgInfo);
static UINT32 ParserType04Part(const void *pMsgInfo);

/* PartNumber mapping function */
static UINT8 HL_GetPartNumberType(UINT32 PartNum);

/* Bonding check spec table */
/* FIXME: spec (MHz) */
static UINT32 HL_IntChkPixelClockTable[PART_NUMBER_MAX] = {
    [PART_NUMBER_UNLIMIT]  = 0xFFFFFFFFU,
    [PART_NUMBER_15]       =          0U,
    [PART_NUMBER_25]       =         90U,
    [PART_NUMBER_35]       =        180U,
    [PART_NUMBER_55]       =        240U,
    [PART_NUMBER_70]       =          0U,
    [PART_NUMBER_75]       =        360U,
    [PART_NUMBER_80]       =          0U,
    [PART_NUMBER_85]       =          0U,
};

static inline UINT32 HL_INT_CHK_GET_PART_SERIES(UINT32 x) {return (UINT32)((x/1000U)%100U);}
static inline UINT32 HL_INT_CHK_GET_PART_NUMBER(UINT32 x) {return (UINT32)((x%1000U)/10U);}
static inline UINT32 HL_INT_CHK_PART_SERIES_TYPE(UINT8 x) {return (x < AMBA_SERIES_NUM)? (UINT32)1U: (UINT32)0U;}
static inline UINT32 HL_INT_CHK_PART_NUM_TYPE(UINT8 x) {return (x < PART_NUMBER_MAX)? (UINT32)1U: (UINT32)0U;}

static UINT8 HL_GetPartNumberType(UINT32 PartNum)
{
    UINT8 Type = PART_NUMBER_MAX, i;
    static const UINT8 HL_IntChkPartNumberValueTable[PART_NUMBER_MAX] = {
        [PART_NUMBER_UNLIMIT]  =  9,
        [PART_NUMBER_15]       = 15,
        [PART_NUMBER_25]       = 25,
        [PART_NUMBER_35]       = 35,
        [PART_NUMBER_55]       = 55,
        [PART_NUMBER_70]       = 70,
        [PART_NUMBER_75]       = 75,
        [PART_NUMBER_80]       = 80,
        [PART_NUMBER_85]       = 85,
    };

    for (i = 0U; i < PART_NUMBER_MAX; i++) {
        if (HL_IntChkPartNumberValueTable[i] == PartNum) {
            Type = i;
        }
    }
    return Type;
}

/* AMBA_UNLIMIT_SERIES */
static UINT32 ParserType00Part(const void *pMsgInfo)
{
    UINT32 Rval = OK;
(void)pMsgInfo;

    return Rval;
}

/* AMBA_A_SERIES */
static UINT32 ParserType01Part(const void *pMsgInfo)
{
    const HL_DSP_BONDING_MSG_s *pMsg;
    UINT32 Rval = OK;
    UINT8 PartNumType;

    dsp_osal_typecast(&pMsg, &pMsgInfo);
    PartNumType = HL_GetPartNumberType(pMsg->PartNumber);

    if (HL_INT_CHK_PART_NUM_TYPE(PartNumType) == 0U) {
        AmbaLL_LogUInt5("Unknown Part Number (%u)", pMsg->PartNumber, 0U, 0U, 0U, 0U);
        Rval = DSP_ERR_0001;
    } else if (PartNumType > PART_NUMBER_UNLIMIT) {
        switch (pMsg->Type){
            case BONDING_PIXEL_CLOCK:
                {
                    if (pMsg->U32CheckValue > HL_IntChkPixelClockTable[PartNumType]) {
                        Rval = DSP_ERR_0001;
                        AmbaLL_LogUInt5("[ERROR] This Chip version CANNOT support: PixelClock > %uMHz (%uMHz)",
                            HL_IntChkPixelClockTable[PartNumType], pMsg->U32CheckValue, 0U, 0U, 0U);
                        dsp_osal_print_stop_and_flush();
                    }
                }
                break;
            case BONDING_SEC_STREAM:
                break;
            case BONDING_LISO_PRI_STREAM:
                break;
            case BONDING_HISO_PRI_STREAM:
                break;
            case BONDING_2ND_VIN_PRI_STREAM:
                break;
            case BONDING_2ND_VIN_SEC_STREAM:
                break;
            case BONDING_EIS_DRONE:
                break;
            case BONDING_EIS_SPORT:
                break;
            case BONDING_ENCODE_FMT:
                break;
            case BONDING_MUTLI_VIN:
                break;
            case BONDING_CHECK_TYPE:
            default:
                Rval = DSP_ERR_0001;
                AmbaLL_LogUInt5("Unknown Bonding Ckeck Type (%u)", pMsg->Type, 0U, 0U, 0U, 0U);
                break;
        }
    } else {
        // PART_NUMBER_UNLIMIT do nothing
    }
    return Rval;
}

/* AMBA_U_SERIES */
static UINT32 ParserType02Part(const void *pMsgInfo)
{
    const HL_DSP_BONDING_MSG_s *pMsg;
    UINT32 Rval = OK;
    UINT8 PartNumType;

    dsp_osal_typecast(&pMsg, &pMsgInfo);
    PartNumType = HL_GetPartNumberType(pMsg->PartNumber);

    if (HL_INT_CHK_PART_NUM_TYPE(PartNumType) == 0U) {
        AmbaLL_LogUInt5("Unknown Part Number (%u)", pMsg->PartNumber, 0U, 0U, 0U, 0U);
        Rval = DSP_ERR_0001;
    } else if (PartNumType > PART_NUMBER_UNLIMIT) {
        switch (pMsg->Type){
            case BONDING_PIXEL_CLOCK:
                {
                    if (pMsg->U32CheckValue > HL_IntChkPixelClockTable[PartNumType]) {
                        Rval = DSP_ERR_0001;
                        AmbaLL_LogUInt5("[ERROR] This Chip version CANNOT support: PixelClock > %uMHz (%uMHz)",
                            HL_IntChkPixelClockTable[PartNumType], pMsg->U32CheckValue, 0U, 0U, 0U);
                        dsp_osal_print_stop_and_flush();
                    }
                }
                break;
            case BONDING_SEC_STREAM:
                break;
            case BONDING_LISO_PRI_STREAM:
                break;
            case BONDING_HISO_PRI_STREAM:
                break;
            case BONDING_2ND_VIN_PRI_STREAM:
                break;
            case BONDING_2ND_VIN_SEC_STREAM:
                break;
            case BONDING_EIS_DRONE:
                break;
            case BONDING_EIS_SPORT:
                break;
            case BONDING_ENCODE_FMT:
                break;
            case BONDING_MUTLI_VIN:
                break;
            case BONDING_CHECK_TYPE:
            default:
                Rval = DSP_ERR_0001;
                AmbaLL_LogUInt5("Unknown Bonding Ckeck Type (%u)", pMsg->Type, 0U, 0U, 0U, 0U);
                break;
        }
    } else {
        // PART_NUMBER_UNLIMIT do nothing
    }
    return Rval;
}

/* AMBA_AX_SERIES */
static UINT32 ParserType03Part(const void *pMsgInfo)
{
    const HL_DSP_BONDING_MSG_s *pMsg;
    UINT32 Rval = OK;
    UINT8 PartNumType;

    dsp_osal_typecast(&pMsg, &pMsgInfo);
    PartNumType = HL_GetPartNumberType(pMsg->PartNumber);

    if (HL_INT_CHK_PART_NUM_TYPE(PartNumType) == 0U) {
        AmbaLL_LogUInt5("Unknown Part Number (%u)", pMsg->PartNumber, 0U, 0U, 0U, 0U);
        Rval = DSP_ERR_0001;
    } else if (PartNumType > PART_NUMBER_UNLIMIT) {
        switch (pMsg->Type){
            case BONDING_PIXEL_CLOCK:
                {
                    if (pMsg->U32CheckValue > HL_IntChkPixelClockTable[PartNumType]) {
                        Rval = DSP_ERR_0001;
                        AmbaLL_LogUInt5("[ERROR] This Chip version CANNOT support: PixelClock > %uMHz (%uMHz)",
                            HL_IntChkPixelClockTable[PartNumType], pMsg->U32CheckValue, 0U, 0U, 0U);
                        dsp_osal_print_stop_and_flush();
                    }
                }
                break;
            case BONDING_SEC_STREAM:
                break;
            case BONDING_LISO_PRI_STREAM:
                break;
            case BONDING_HISO_PRI_STREAM:
                break;
            case BONDING_2ND_VIN_PRI_STREAM:
                break;
            case BONDING_2ND_VIN_SEC_STREAM:
                break;
            case BONDING_EIS_DRONE:
                break;
            case BONDING_EIS_SPORT:
                break;
            case BONDING_ENCODE_FMT:
                break;
            case BONDING_MUTLI_VIN:
                break;
            case BONDING_CHECK_TYPE:
            default:
                AmbaLL_LogUInt5("Unknown Bonding Ckeck Type (%u)", pMsg->Type, 0U, 0U, 0U, 0U);
                Rval = DSP_ERR_0001;
                break;
        }
    } else {
        // PART_NUMBER_UNLIMIT do nothing
    }
    return Rval;
}

/* AMBA_AQ_SERIES */
static UINT32 ParserType04Part(const void *pMsgInfo)
{
    const HL_DSP_BONDING_MSG_s *pMsg;
    UINT32 Rval = OK;
    UINT8 PartNumType;

    dsp_osal_typecast(&pMsg, &pMsgInfo);
    PartNumType = HL_GetPartNumberType(pMsg->PartNumber);

    if (HL_INT_CHK_PART_NUM_TYPE(PartNumType) == 0U) {
        AmbaLL_LogUInt5("Unknown Part Number (%u)", pMsg->PartNumber, 0U, 0U, 0U, 0U);
        Rval = DSP_ERR_0001;
    } else if (PartNumType > PART_NUMBER_UNLIMIT) {
        switch (pMsg->Type){
            case BONDING_PIXEL_CLOCK:
                {
                    if (pMsg->U32CheckValue > HL_IntChkPixelClockTable[PartNumType]) {
                        Rval = DSP_ERR_0001;
                        AmbaLL_LogUInt5("[ERROR] This Chip version CANNOT support: PixelClock > %uMHz (%uMHz)",
                            HL_IntChkPixelClockTable[PartNumType], pMsg->U32CheckValue, 0U, 0U, 0U);
                        dsp_osal_print_stop_and_flush();
                    }
                }
                break;
            case BONDING_SEC_STREAM:
                break;
            case BONDING_LISO_PRI_STREAM:
                break;
            case BONDING_HISO_PRI_STREAM:
                break;
            case BONDING_2ND_VIN_PRI_STREAM:
                break;
            case BONDING_2ND_VIN_SEC_STREAM:
                break;
            case BONDING_EIS_DRONE:
                break;
            case BONDING_EIS_SPORT:
                break;
            case BONDING_ENCODE_FMT:
                break;
            case BONDING_MUTLI_VIN:
                break;
            case BONDING_CHECK_TYPE:
            default:
                AmbaLL_LogUInt5("Unknown Bonding Ckeck Type (%u)", pMsg->Type, 0U, 0U, 0U, 0U);
                Rval = DSP_ERR_0001;
                break;
        }
    } else {
        // PART_NUMBER_UNLIMIT do nothing
    }
    return Rval;
}

/* BONDING_PIXEL_CLOCK */
UINT32 HL_Int00Check(UINT32 TotalPixel)
{
    /* Silicon version define: ref to cmd_msg_dsp.c SIL_VER_XX */
    static const UINT8 HL_IntChkPartSeriesMapTypeTable[AMBA_SERIES_MAX_NUM] = {
        [0]  = AMBA_UNLIMIT_SERIES,
    #ifdef PROJECT_CV2S
        [1]  = AMBA_A_SERIES,
    #elif defined(PROJECT_CV2E)
        [1]  = AMBA_A_SERIES,
    #else
        [1]  = AMBA_U_SERIES,
    #endif
        [2]  = AMBA_SERIES_NUM,
        [3]  = AMBA_SERIES_NUM,
        [4]  = AMBA_AX_SERIES,
        [5]  = AMBA_AQ_SERIES,
    };

    static const HL_BONDING_PARSER_f HL_IntChkParserTable[AMBA_SERIES_NUM] = {
        [AMBA_UNLIMIT_SERIES]  = ParserType00Part,
        [AMBA_A_SERIES]        = ParserType01Part,
        [AMBA_U_SERIES]        = ParserType02Part,
        [AMBA_AX_SERIES]       = ParserType03Part,
        [AMBA_AQ_SERIES]       = ParserType04Part,
    };

    UINT32 Rval = OK, ChipSeries;
    UINT8 ChipSeriesType;
    AMBA_DSP_VERSION_INFO_s UcodeVersion = {0};

    HL_GetUCodeVerInfo(&UcodeVersion);
    ChipSeries = HL_INT_CHK_GET_PART_SERIES(UcodeVersion.SiliconVer);

    if (ChipSeries < AMBA_SERIES_MAX_NUM) {
        ChipSeriesType = HL_IntChkPartSeriesMapTypeTable[ChipSeries];

        if ((HL_INT_CHK_PART_SERIES_TYPE(ChipSeriesType) == 1U) &&
            (HL_IntChkParserTable[ChipSeriesType] != NULL)) {
            HL_DSP_BONDING_MSG_s Msg = {0};
            Msg.Type = BONDING_PIXEL_CLOCK;
            Msg.PartNumber = HL_INT_CHK_GET_PART_NUMBER(UcodeVersion.SiliconVer);
            Msg.U32CheckValue = TotalPixel;
            Rval = HL_IntChkParserTable[ChipSeriesType](&Msg);
        }
    }
    return Rval;
}

#if 0
/* BONDING_SEC_STREAM */
UINT32 HL_Int01Check(AMBA_DSP_WINDOW_s *Window)
{
    UINT32 Rval = OK, ChipSeries;
    UINT8 ChipSeriesType;
    AMBA_DSP_VERSION_INFO_s UcodeVersion = {0};

    HL_GetUCodeVerInfo(&UcodeVersion);
    ChipSeries = HL_INT_CHK_GET_PART_SERIES(UcodeVersion.SiliconVer);

    if (ChipSeries < AMBA_SERIES_MAX_NUM) {
        ChipSeriesType = HL_IntChkPartSeriesMapTypeTable[ChipSeries];

        if ((HL_INT_CHK_PART_SERIES_TYPE(ChipSeriesType) == 1U) &&
            (HL_IntChkParserTable[ChipSeriesType] != NULL)) {
            HL_DSP_BONDING_MSG_s Msg = {0};
            Msg.Type = BONDING_SEC_STREAM;
            Msg.PartNumber = HL_INT_CHK_GET_PART_NUMBER(UcodeVersion.SiliconVer);
            Msg.Window = Window;
            Rval = HL_IntChkParserTable[ChipSeriesType](&Msg);
        }
    }
    return Rval;
}
#endif
