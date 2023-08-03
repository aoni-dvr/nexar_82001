/**
 *  @file AmbaImgEvent.c
 *
 *  Copyright (c) [2020] Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  This file includes sample code and is only for internal testing and evaluation.  If you
 *  distribute this sample code (whether in source, object, or binary code form), it will be
 *  without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Amba Image Event
 *
 */
#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaVIN_Def.h"

#include "AmbaImgChannel.h"
//#include "AmbaImgChannel_Internal.h"
#include "AmbaImgEvent.h"
#include "AmbaImgEvent_Internal.h"

#define OK_UL  ((UINT32) 0U)
#define NG_UL  ((UINT32) 1U)

/**
 *  Amba image channel event init
 *  @return error code
 */
UINT32 AmbaImgEvent_Init(void)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i, j, k;

    AMBA_IMG_EVENT_VIN_s *pVin;
    const AMBA_IMG_EVENT_CHAN_s *pChan;

    for (i = 0U; i < AMBA_IMGFRW_NUM_VIN_CHANNEL; i++) {
        /* get vin */
        pVin = &(AmbaImgEvent_Vin[i]);
        /* vin init */
        for(j = 0U; j < 2U; j++) {
            /* vin group event */
            FuncRetCode = AmbaKAL_EventFlagCreate(&(pVin->Group[j]), NULL);
            if (FuncRetCode != KAL_ERR_NONE) {
                RetCode = NG_UL;
            }
        }
        /* chan check */
        if (AmbaImgEvent_Chan[i] != NULL) {
            /* chan get */
            pChan = AmbaImgEvent_Chan[i];
            /* chain find */
            for (j = 0U; pChan[j].Magic == 0xCafeU; j++) {
                /* chain init */
                if (pChan[j].pCtx != NULL) {
                    for (k = 0U; k < 2U; k++) {
                        /* chan group event */
                        FuncRetCode = AmbaKAL_EventFlagCreate(&(pChan[j].pCtx->Group[k]), NULL);
                        if (FuncRetCode != KAL_ERR_NONE) {
                            RetCode = NG_UL;
                        }
                    }
                }
            }
        } else {
            /* chan null */
            RetCode = NG_UL;
        }
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image event vin put
 *  @param[in] VinId vin id
 *  @param[in] Flag event flag
 *  @return error code
 *  @note the function is intended for internal use only
 */
UINT32 AmbaImgEvent_VinPut(UINT32 VinId, UINT64 Flag)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 GroupFlag;
    AMBA_IMG_EVENT_VIN_s *pVin;

    if (VinId < AMBA_IMGFRW_NUM_VIN_CHANNEL) {
        /* vin get */
        pVin = &(AmbaImgEvent_Vin[VinId]);
        /* flag to group */
        for (i = 0U; i < 2U; i++) {
            /* find flag in group */
            GroupFlag = (UINT32) ((Flag >> (32ULL * i)) & 0xFFFFFFFFULL);
            /* flag check */
            if (GroupFlag > 0U) {
                /* flag set */
                FuncRetCode = AmbaKAL_EventFlagSet(&(pVin->Group[i]), GroupFlag);
                if (FuncRetCode != KAL_ERR_NONE) {
                    RetCode = NG_UL;
                }
            }
        }
    } else {
        /* vin null */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image event vin get
 *  @param[in] VinId vin id
 *  @param[in] Flag event flag
 *  @param[in] Option event option
 *  @param[in] Timeout time out
 *  @return actual event flag
 *  @note the function is intended for internal use only
 */
UINT64 AmbaImgEvent_VinGet(UINT32 VinId, UINT64 Flag, UINT32 Option, UINT32 Timeout)
{
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 GroupFlag;
    UINT32 GroupActualFlag;

    UINT64 ActualFlag = 0ULL;

    AMBA_IMG_EVENT_VIN_s *pVin = &(AmbaImgEvent_Vin[VinId]);

    if (VinId < AMBA_IMGFRW_NUM_VIN_CHANNEL) {
        /* vin get */
        pVin = &(AmbaImgEvent_Vin[VinId]);
        /* flag to group */
        for (i = 0U; i < 2U; i++) {
            /* find flag in group */
            GroupFlag = (UINT32) ((Flag >> (32ULL * i)) & 0xFFFFFFFFULL);
            /* group flag check */
            if (GroupFlag > 0U) {
                /* group flag get */
                FuncRetCode = AmbaKAL_EventFlagGet(&(pVin->Group[i]), GroupFlag, (Option >> IMG_EVENT_OP_SHIFT), (Option & IMG_EVENT_CLR_MASK), &GroupActualFlag, Timeout);
                if (FuncRetCode == KAL_ERR_NONE) {
                    /* accumulated flag */
                    ActualFlag |= (((UINT64) GroupActualFlag) << (32ULL * i));
                }
            }
        }
    }

    return ActualFlag;
}

/**
 *  @private
 *  Amba image event vin clear
 *  @param[in] VinId vin id
 *  @param[in] Flag event flag
 *  @return error code
 *  @note the function is intended for internal use only
 */
UINT32 AmbaImgEvent_VinClr(UINT32 VinId, UINT64 Flag)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 GroupFlag;
    AMBA_IMG_EVENT_VIN_s *pVin = &(AmbaImgEvent_Vin[VinId]);

    if (VinId < AMBA_IMGFRW_NUM_VIN_CHANNEL) {
        /* vin get */
        pVin = &(AmbaImgEvent_Vin[VinId]);
        /* flag to group */
        for (i = 0U; i < 2U; i++) {
            /* find flag in group */
            GroupFlag = (UINT32) ((Flag >> (32ULL * i)) & 0xFFFFFFFFULL);
            /* flag check */
            if (GroupFlag > 0U) {
                /* flag set */
                FuncRetCode = AmbaKAL_EventFlagClear(&(pVin->Group[i]), GroupFlag);
                if (FuncRetCode != KAL_ERR_NONE) {
                    RetCode = NG_UL;
                }
            }
        }
    } else {
        /* vin null */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image event channel put
 *  @param[in] ImageChanId image channel id
 *  @param[in] Flag event flag
 *  @return error code
 *  @note the function is intended for internal use only
 */
UINT32 AmbaImgEvent_ChanPut(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 Flag)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 VinId;
    UINT32 ChainId;

    UINT32 GroupFlag;

    AMBA_IMG_EVENT_CONTEXT_s *pCtx;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    /* chan check */
    if ((AmbaImgEvent_Chan[VinId] != NULL) &&
        (AmbaImgEvent_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        /* ctx check */
        if (AmbaImgEvent_Chan[VinId][ChainId].pCtx != NULL) {
            /* get ctx */
            pCtx = AmbaImgEvent_Chan[VinId][ChainId].pCtx;
            /* flag to group */
            for (i = 0U; i < 2U; i++) {
                /* find flag in group */
                GroupFlag = (UINT32) ((Flag >> (32ULL * i)) & 0xFFFFFFFFULL);
                /* flag check */
                if (GroupFlag > 0U) {
                    /* flag set */
                    FuncRetCode = AmbaKAL_EventFlagSet(&(pCtx->Group[i]), GroupFlag);
                    if (FuncRetCode != KAL_ERR_NONE) {
                        RetCode = NG_UL;
                    }
                }
            }
        }
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image event channel get
 *  @param[in] ImageChanId image channel id
 *  @param[in] Flag event flag
 *  @param[in] Option event option
 *  @param[in] Timeout time out
 *  @return actual event flag
 *  @note the function is intended for internal use only
 */
UINT64 AmbaImgEvent_ChanGet(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 Flag, UINT32 Option, UINT32 Timeout)
{
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 VinId;
    UINT32 ChainId;

    UINT32 GroupFlag;
    UINT32 GroupActualFlag;

    UINT64 ActualFlag = 0ULL;

    AMBA_IMG_EVENT_CONTEXT_s *pCtx;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    /* chan check */
    if ((AmbaImgEvent_Chan[VinId] != NULL) &&
        (AmbaImgEvent_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        /* ctx check */
        if (AmbaImgEvent_Chan[VinId][ChainId].pCtx != NULL) {
            /* get ctx */
            pCtx = AmbaImgEvent_Chan[VinId][ChainId].pCtx;
            /* flag to group */
            for (i = 0U; i < 2U; i++) {
                /* find flag in group */
                GroupFlag = (UINT32) ((Flag >> (32ULL * i)) & 0xFFFFFFFFULL);
                /* group flag check */
                if (GroupFlag > 0U) {
                    /* group flag get */
                    FuncRetCode = AmbaKAL_EventFlagGet(&(pCtx->Group[i]), GroupFlag, (Option >> IMG_EVENT_OP_SHIFT), (Option & IMG_EVENT_CLR_MASK), &GroupActualFlag, Timeout);
                    if (FuncRetCode == KAL_ERR_NONE) {
                        /* accumulated flag */
                        ActualFlag |= (((UINT64) GroupActualFlag) << (32ULL * i));
                    }
                }
            }
        }
    }

    return ActualFlag;
}

/**
 *  @private
 *  Amba image event channel clear
 *  @param[in] ImageChanId image channle id
 *  @param[in] Flag event flag
 *  @return error code
 *  @note the function is intended for internal use only
 */
UINT32 AmbaImgEvent_ChanClr(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 Flag)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 VinId;
    UINT32 ChainId;

    UINT32 GroupFlag;

    AMBA_IMG_EVENT_CONTEXT_s *pCtx;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    /* chan check */
    if ((AmbaImgEvent_Chan[VinId] != NULL) &&
        (AmbaImgEvent_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        /* ctx check */
        if (AmbaImgEvent_Chan[VinId][ChainId].pCtx != NULL) {
            /* get ctx */
            pCtx = AmbaImgEvent_Chan[VinId][ChainId].pCtx;
            /* flag to group */
            for (i = 0U; i < 2U; i++) {
                /* find flag in group */
                GroupFlag = (UINT32) ((Flag >> (32ULL * i)) & 0xFFFFFFFFULL);
                /* flag check */
                if (GroupFlag > 0U) {
                    /* flag set */
                    FuncRetCode = AmbaKAL_EventFlagClear(&(pCtx->Group[i]), GroupFlag);
                    if (FuncRetCode != KAL_ERR_NONE) {
                        RetCode = NG_UL;
                    }
                }
            }
        }
    }

    return RetCode;
}

