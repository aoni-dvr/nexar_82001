/**
 *  @file AmbaImgFramework_ChanTable_Platform.c
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
 *  @details Amba Image Framework Chan Table for common soc
 *
 */

#include "AmbaTypes.h"
#include "AmbaVIN_Def.h"
#include "AmbaKAL.h"

#include "AmbaWrap.h"

#include "AmbaDSP_ImageFilter.h"

#include "AmbaImgFramework_Chan.h"
#include "AmbaImgMain.h"
//#include "AmbaImgMain_Internal.h"
#include "AmbaImgSensorDrv_Chan.h"
#include "AmbaImgMessage_Chan.h"

#define OK_UL   ((UINT32) 0U)
//#define NG_UL   ((UINT32) 1U)

/**
 *  @private
 *  Amba image channel user entry
 *  @param[in] VinId vin id
 *  @param[in] ChainIndex chain index
 *  @return (AMBA_IMG_CHANNEL_s *)
 *  @note this function is intended for internal use only
 */
AMBA_IMG_CHANNEL_s *AmbaImgChannel_UserEntry(UINT32 VinId, UINT32 ChainIndex)
{
    (void) VinId;
    (void) ChainIndex;

    return NULL;
}

/*---------------------------------------------------------------------------*\
 *  AmbaImgChannel Entry
\*---------------------------------------------------------------------------*/
AMBA_IMG_CHANNEL_TIMING_PORT_s GNU_SECTION_NOZEROINIT AmbaImgChannel_MofTimingPort[AMBA_IMG_NUM_VIN_CHANNEL];
AMBA_IMG_CHANNEL_s *AmbaImgChannel_Entry[AMBA_IMG_NUM_VIN_CHANNEL];
AMBA_IMG_CHANNEL_s *AmbaImgChannel_EntryTable[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];

/*---------------------------------------------------------------------------*\
 *  AmbaImgSystem Vin & CHan
\*---------------------------------------------------------------------------*/
AMBA_IMG_SYSTEM_AUX_INFO_s AmbaImgSystem_AuxInfo;
AMBA_IMG_SYSTEM_INFO_s GNU_SECTION_NOZEROINIT AmbaImgSystem_Info[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];
AMBA_IMG_SYSTEM_VIN_s AmbaImgSystem_Vin[AMBA_IMG_NUM_VIN_CHANNEL];
AMBA_IMG_SYSTEM_CHAN_s *AmbaImgSystem_Chan[AMBA_IMG_NUM_VIN_CHANNEL];

/*---------------------------------------------------------------------------*\
 *  AmbaImgEvent Vin & Chan
\*---------------------------------------------------------------------------*/
AMBA_IMG_EVENT_VIN_s GNU_SECTION_NOZEROINIT AmbaImgEvent_Vin[AMBA_IMG_NUM_VIN_CHANNEL];
AMBA_IMG_EVENT_CHAN_s *AmbaImgEvent_Chan[AMBA_IMG_NUM_VIN_CHANNEL];

/*---------------------------------------------------------------------------*\
 *  AmbaImgMessage Vin & Chan
\*---------------------------------------------------------------------------*/
AMBA_IMG_MESSAGE_CHAN_s *AmbaImgMessage_Chan[AMBA_IMG_NUM_VIN_CHANNEL];

/*---------------------------------------------------------------------------*\
 *  AmbaImgStatistics Vin & Chan
\*---------------------------------------------------------------------------*/
AMBA_IMG_STATISTICS_VIN_s GNU_SECTION_NOZEROINIT AmbaImgStatistics_Vin[AMBA_IMG_NUM_VIN_CHANNEL];
AMBA_IMG_STATISTICS_CHAN_s *AmbaImgStatistics_Chan[AMBA_IMG_NUM_VIN_CHANNEL];

/*---------------------------------------------------------------------------*\
 *  AmbaImgSensorHAL Vin & Chan
\*---------------------------------------------------------------------------*/
AMBA_IMG_SENSOR_HAL_AUX_s GNU_SECTION_NOZEROINIT AmbaImgSensorHAL_AuxInfo[AMBA_IMG_NUM_VIN_CHANNEL];
AMBA_IMG_SENSOR_HAL_INFO_s GNU_SECTION_NOZEROINIT AmbaImgSensorHAL_Info[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];
AMBA_IMG_SENSOR_HAL_HDR_INFO_s GNU_SECTION_NOZEROINIT AmbaImgSensorHAL_HdrInfo[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];
AMBA_IMG_SENSOR_HAL_VIN_s AmbaImgSensorHAL_Vin[AMBA_IMG_NUM_VIN_CHANNEL];
AMBA_IMG_SENSOR_HAL_CHAN_s *AmbaImgSensorHAL_Chan[AMBA_IMG_NUM_VIN_CHANNEL];

/*---------------------------------------------------------------------------*\
 *  AmbaImgSensorSync Vin & Chan
\*---------------------------------------------------------------------------*/
AMBA_IMG_SENSOR_SYNC_VIN_s AmbaImgSensorSync_Vin[AMBA_IMG_NUM_VIN_CHANNEL];
AMBA_IMG_SENSOR_SYNC_CHAN_s *AmbaImgSensorSync_Chan[AMBA_IMG_NUM_VIN_CHANNEL];

/*---------------------------------------------------------------------------*\
 *  AmbaImgSensorDrv Vin & Chan
\*---------------------------------------------------------------------------*/
AMBA_IMG_SENSOR_DRV_CHAN_s *AmbaImgSensorDrv_Chan[AMBA_IMG_NUM_VIN_CHANNEL];

/**
 *  Amba image channel memory init
 *  @note the function is intended for internal use only
 */
static void AmbaImgChannel_MemInit(void)
{
    static AMBA_IMG_CHANNEL_TIMING_s GNU_SECTION_NOZEROINIT AmbaImgChannel_Timing[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];
    static AMBA_IMG_CHANNEL_AIK_SYNC_s GNU_SECTION_NOZEROINIT AmbaImgChannel_AikSync[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];
    static AMBA_IMG_CHANNEL_CONTEXT_s GNU_SECTION_NOZEROINIT AmbaImgChannel_VinAlgoContext[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];
    static AMBA_IMG_CHANNEL_s GNU_SECTION_NOZEROINIT AmbaImgChannel_Vin_AlgoChain[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO][AMBA_IMG_NUM_VIN_ALGO+1UL];

    UINT32 FuncRetCode;
    UINT32 i, j, k;

    /* ctx reset */
    for (i = 0U; i < AMBA_IMG_NUM_VIN_CHANNEL; i++) {
        for (j = 0U; j < AMBA_IMG_NUM_VIN_ALGO; j++) {
            /* id */
            AmbaImgChannel_VinAlgoContext[i][j].Id.Ctx.Data = 0ULL;
            /* vin id */
            AmbaImgChannel_VinAlgoContext[i][j].Id.Ctx.Bits.VinId = (UINT8) i;
            /* chain id */
            AmbaImgChannel_VinAlgoContext[i][j].Id.Ctx.Bits.ChainId = (UINT8) j;
            /* vr map */
            AmbaImgChannel_VinAlgoContext[i][j].VrMap.Cnt = 0U;
            AmbaImgChannel_VinAlgoContext[i][j].VrMap.Id.Ctx.Data = 0ULL;
            /* inter */
            AmbaImgChannel_VinAlgoContext[i][j].Inter.Ctx.Data = 0ULL;
            /* aik */
            AmbaImgChannel_VinAlgoContext[i][j].Aik.Ctx.Data = 0ULL;
            /* post aik */
            AmbaImgChannel_VinAlgoContext[i][j].PAik.Ctx.Data = 0ULL;
            /* snap */
            AmbaImgChannel_VinAlgoContext[i][j].Snap.Ctx.Data = 0ULL;
            /* efov */
            AmbaImgChannel_VinAlgoContext[i][j].EFov.Ctx.Data = 0ULL;
        }
    }

    /* aik sync clear */
    FuncRetCode = AmbaWrap_memset(AmbaImgChannel_AikSync, 0, sizeof(AmbaImgChannel_AikSync));
    if (FuncRetCode != OK_UL) {
        /* */
    }

    /* aik hook */
    for (i = 0U; i < AMBA_IMG_NUM_VIN_CHANNEL; i++) {
        for (j = 0U; j < AMBA_IMG_NUM_VIN_ALGO; j++) {
            /* sync */
            AmbaImgChannel_VinAlgoContext[i][j].pAikSync = &(AmbaImgChannel_AikSync[i][j]);
        }
    }

    /* chain clear */
    FuncRetCode = AmbaWrap_memset(AmbaImgChannel_Vin_AlgoChain, 0, sizeof(AmbaImgChannel_Vin_AlgoChain));
    if (FuncRetCode != OK_UL) {
        /* */
    }

    /* ctx and timing hook */
    for (i = 0U; i < AMBA_IMG_NUM_VIN_CHANNEL; i++) {
        for(j = 0U; j < AMBA_IMG_NUM_VIN_ALGO; j++) {
            for (k = 0U; k <= j; k++) {
                AmbaImgChannel_Vin_AlgoChain[i][j][k].Magic = 0xCafeU;
                AmbaImgChannel_Vin_AlgoChain[i][j][k].pCtx = &(AmbaImgChannel_VinAlgoContext[i][k]);
                AmbaImgChannel_Vin_AlgoChain[i][j][k].pTiming = &(AmbaImgChannel_Timing[i][k]);
            }
        }
    }

    /* chain hook */
    for (i = 0U; i < AMBA_IMG_NUM_VIN_CHANNEL; i++) {
        for(j = 0U; j < AMBA_IMG_NUM_VIN_ALGO; j++) {
            AmbaImgChannel_EntryTable[i][j] = AmbaImgChannel_Vin_AlgoChain[i][j];
        }
    }

    /* chan reset */
    for (i = 0U; i < AMBA_IMG_NUM_VIN_CHANNEL; i++) {
        AmbaImgChannel_Entry[i] = NULL;
    }
}

/**
 *  Amba image system memory init
 *  @note the function is intended for internal use only
 */
static void AmbaImgSystem_MemInit(void)
{
#define AMBA_IMG_SYSTEM_ZONE_NUM   AMBA_IMG_NUM_FOV_CHANNEL
    static AMBA_IMG_SYSTEM_AIK_STATUS_s GNU_SECTION_NOZEROINIT AmbaImgSystem_AikStatus[AMBA_IMG_SYSTEM_ZONE_NUM];
    static AMBA_IMG_SYSTEM_ZONE_s GNU_SECTION_NOZEROINIT AmbaImgSystem_SixteenZone[AMBA_IMG_SYSTEM_ZONE_NUM];
    static AMBA_IMG_SYSTEM_PAIK_s GNU_SECTION_NOZEROINIT AmbaImgSystem_AikPost[AMBA_IMG_SYSTEM_ZONE_NUM];
#define AMBA_IMG_SYSTEM_PAIK_INFO_NUM 8U
    static AMBA_IMG_SYSTEM_PAIK_INFO_s GNU_SECTION_NOZEROINIT AmbaImgSystem_PostAikInfo[AMBA_IMG_SYSTEM_ZONE_NUM][AMBA_IMG_SYSTEM_PAIK_INFO_NUM];
#ifdef CONFIG_BUILD_IMGFRW_RAW_ENC
    static AMBA_IMG_SYSTEM_RING_s GNU_SECTION_NOZEROINIT AmbaImgSystem_Ring[AMBA_IMG_SYSTEM_ZONE_NUM];
#endif
    static AMBA_IMG_SYSTEM_CONTEXT_s GNU_SECTION_NOZEROINIT AmbaImgSystem_VinContext[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_SYSTEM_ZONE_NUM+1UL];
    static AMBA_IMG_SYSTEM_CHAN_s GNU_SECTION_NOZEROINIT AmbaImgSystem_VinChain[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO+1UL];

    static AMBA_IMG_SYSTEM_AIK_s GNU_SECTION_NOZEROINIT AmbaImgSystem_Aik[AMBA_IMG_SYSTEM_ZONE_NUM];
    static AMBA_IMG_SYSTEM_USER_s GNU_SECTION_NOZEROINIT AmbaImgSystem_AikR2yUser[AMBA_IMG_SYSTEM_ZONE_NUM];

#define AMBA_IMG_SYSTEM_AIK_STATUS_NUM   8U
    static AMBA_IMG_SYSTEM_AIK_CTX_s GNU_SECTION_NOZEROINIT AmbaImgSystem_AikStatusCtx[AMBA_IMG_SYSTEM_ZONE_NUM][AMBA_IMG_SYSTEM_AIK_STATUS_NUM];
#ifdef CONFIG_BUILD_IMGFRW_RAW_ENC
#define AMBA_IMG_SYSTEM_RING_CTX_NUM 16U
    static AMBA_IMG_SYSTEM_RING_CTX_s GNU_SECTION_NOZEROINIT AmbaImgSystem_RingCtx[AMBA_IMG_SYSTEM_ZONE_NUM][AMBA_IMG_SYSTEM_RING_CTX_NUM];
#endif
    UINT32 FuncRetCode;
    UINT32 i, j;

    /* vin reset */
    for (i = 0U; i < AMBA_IMG_NUM_VIN_CHANNEL; i++) {
        AmbaImgSystem_Vin[i].Debug.Data = 0U;
        AmbaImgSystem_Vin[i].Op.Data = 0U;
        AmbaImgSystem_Vin[i].Counter.Raw = 0U;
    }

    /* aik and ring hook */
    for (i = 0U; i < AMBA_IMG_SYSTEM_ZONE_NUM; i++) {
        /* flag */
        AmbaImgSystem_SixteenZone[i].InitFlag = 0U;
        /* aik */
        AmbaImgSystem_SixteenZone[i].pAik = &(AmbaImgSystem_Aik[i]);

        /* aik status */
        AmbaImgSystem_SixteenZone[i].pAik->pStatus = &(AmbaImgSystem_AikStatus[i]);
        /* aik status ctx */
        AmbaImgSystem_SixteenZone[i].pAik->pStatus->Num = AMBA_IMG_SYSTEM_AIK_STATUS_NUM;
        AmbaImgSystem_SixteenZone[i].pAik->pStatus->Index.Data = 0U;
        AmbaImgSystem_SixteenZone[i].pAik->pStatus->pCtx = AmbaImgSystem_AikStatusCtx[i];

        /* post aik */
        AmbaImgSystem_SixteenZone[i].pAik->pPost = &(AmbaImgSystem_AikPost[i]);
        /* post aik info */
        AmbaImgSystem_SixteenZone[i].pAik->pPost->Ring.Num = AMBA_IMG_SYSTEM_PAIK_INFO_NUM;
        AmbaImgSystem_SixteenZone[i].pAik->pPost->Ring.Index.Data = 0U;
        AmbaImgSystem_SixteenZone[i].pAik->pPost->Ring.pInfo = AmbaImgSystem_PostAikInfo[i];

        /* user */
        AmbaImgSystem_SixteenZone[i].pAik->pR2yUser = &(AmbaImgSystem_AikR2yUser[i]);
        /* user data */
        AmbaImgSystem_SixteenZone[i].pAik->pR2yUser->Ring.Count = 0U;
        AmbaImgSystem_SixteenZone[i].pAik->pR2yUser->Ring.Index = 0U;
        AmbaImgSystem_SixteenZone[i].pAik->pR2yUser->Ring.pMem = NULL;
#ifdef CONFIG_BUILD_IMGFRW_RAW_ENC
        /* ring */
        AmbaImgSystem_SixteenZone[i].pRing = &(AmbaImgSystem_Ring[i]);
        /* ring ctx */
        AmbaImgSystem_SixteenZone[i].pRing->Num = AMBA_IMG_SYSTEM_RING_CTX_NUM;
        AmbaImgSystem_SixteenZone[i].pRing->Index.Data = 0U;
        AmbaImgSystem_SixteenZone[i].pRing->pCtx = AmbaImgSystem_RingCtx[i];
#else
        /* ring */
        AmbaImgSystem_SixteenZone[i].pRing = NULL;
#endif
        /* active aik ctx */
        AmbaImgSystem_SixteenZone[i].ActAikCtx.Id = 0U;
        AmbaImgSystem_SixteenZone[i].ActAikCtx.pIsoCfg = NULL;
    }

    /* ctx clear */
    FuncRetCode = AmbaWrap_memset(AmbaImgSystem_VinContext, 0, sizeof(AmbaImgSystem_VinContext));
    if (FuncRetCode != OK_UL) {
        /* */
    }

    /* zone hook */
    for (i = 0U; i < AMBA_IMG_NUM_VIN_CHANNEL; i++) {
        for (j = 0U; j < AMBA_IMG_SYSTEM_ZONE_NUM; j++) {
            AmbaImgSystem_VinContext[i][j].Magic = 0xCafeU;
            AmbaImgSystem_VinContext[i][j].pZone = &(AmbaImgSystem_SixteenZone[j]);
        }
    }

    /* chain clear */
    FuncRetCode = AmbaWrap_memset(AmbaImgSystem_VinChain, 0, sizeof(AmbaImgSystem_VinChain));
    if (FuncRetCode != OK_UL) {
        /* */
    }

    /* ctx hook */
    for (i = 0U; i < AMBA_IMG_NUM_VIN_CHANNEL; i++) {
        for (j = 0U; j < AMBA_IMG_NUM_VIN_ALGO; j++) {
            AmbaImgSystem_VinChain[i][j].Magic = 0xCafeU;
            AmbaImgSystem_VinChain[i][j].pCtx = AmbaImgSystem_VinContext[i];
            AmbaImgSystem_VinChain[i][j].pInfo = &(AmbaImgSystem_Info[i][j]);
        }
    }

    /* chain hook */
    for (i = 0U; i < AMBA_IMG_NUM_VIN_CHANNEL; i++) {
        AmbaImgSystem_Chan[i] = AmbaImgSystem_VinChain[i];
    }
}

/**
 *  Amba image event memory init
 *  @note the function is intended for internal use only
 */
static void AmbaImgEvent_MemInit(void)
{
    static AMBA_IMG_EVENT_CONTEXT_s GNU_SECTION_NOZEROINIT AmbaImgEvent_Context[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];
    static AMBA_IMG_EVENT_CHAN_s GNU_SECTION_NOZEROINIT AmbaImgEvent_VinChain[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO+1UL];

    UINT32 FuncRetCode;
    UINT32 i, j;

    /* chain clear*/
    FuncRetCode = AmbaWrap_memset(AmbaImgEvent_VinChain, 0, sizeof(AmbaImgEvent_VinChain));
    if (FuncRetCode != OK_UL) {
        /* */
    }

    /* ctx hook */
    for (i = 0U; i < AMBA_IMG_NUM_VIN_CHANNEL; i++) {
        for (j = 0U; j < AMBA_IMG_NUM_VIN_ALGO; j++) {
            AmbaImgEvent_VinChain[i][j].Magic = 0xCafeU;
            AmbaImgEvent_VinChain[i][j].pCtx = &(AmbaImgEvent_Context[i][j]);
        }
    }

    /* chain hook */
    for (i = 0U; i < AMBA_IMG_NUM_VIN_CHANNEL; i++) {
        AmbaImgEvent_Chan[i] = AmbaImgEvent_VinChain[i];
    }
}

/**
 *  Amba image message memory init
 *  @note the function is intended for internal use only
 */
static void AmbaImgMessage_MemInit(void)
{
    static AMBA_IMG_MESSAGE_CHAN_s GNU_SECTION_NOZEROINIT AmbaImgMessage_VinChain[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO+1UL];

    UINT32 FuncRetCode;
    UINT32 i, j;

    /* chain clear */
    FuncRetCode = AmbaWrap_memset(AmbaImgMessage_VinChain, 0, sizeof(AmbaImgMessage_VinChain));
    if (FuncRetCode != OK_UL) {
        /* */
    }

    /* ctx hook */
    for (i = 0U; i < AMBA_IMG_NUM_VIN_CHANNEL; i++) {
        for (j = 0U; j < AMBA_IMG_NUM_VIN_ALGO; j++) {
            AmbaImgMessage_VinChain[i][j].Magic = 0xCafeU;
            AmbaImgMessage_VinChain[i][j].pFunc = AmbaImgMessage_ChanFunc;
        }
    }

    /* chain hook */
    for (i = 0U; i < AMBA_IMG_NUM_VIN_CHANNEL; i++) {
        AmbaImgMessage_Chan[i] = AmbaImgMessage_VinChain[i];
    }
}

/**
 *  Amba image statistics memory init
 *  @note the function is intended for internal use only
 */
static void AmbaImgStatistics_MemInit(void)
{
    static AMBA_IMG_STATISTICS_CONTEXT_s GNU_SECTION_NOZEROINIT AmbaImgStatistics_Context[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];
    static AMBA_IMG_STATISTICS_CHAN_s GNU_SECTION_NOZEROINIT AmbaImgStatistics_VinChain[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO+1UL];

    UINT32 FuncRetCode;
    UINT32 i, j;

    /* chain clear*/
    FuncRetCode = AmbaWrap_memset(AmbaImgStatistics_VinChain, 0, sizeof(AmbaImgStatistics_VinChain));
    if (FuncRetCode != OK_UL) {
        /* */
    }

    /* ctx hook */
    for (i = 0U; i < AMBA_IMG_NUM_VIN_CHANNEL; i++) {
        for (j = 0U; j < AMBA_IMG_NUM_VIN_ALGO; j++) {
            AmbaImgStatistics_VinChain[i][j].Magic = 0xCafeU;
            AmbaImgStatistics_VinChain[i][j].pCtx = &(AmbaImgStatistics_Context[i][j]);
        }
    }

    /* chain hook */
    for (i = 0U; i < AMBA_IMG_NUM_VIN_CHANNEL; i++) {
        AmbaImgStatistics_Chan[i] = AmbaImgStatistics_VinChain[i];
    }
}

/**
 *  Amba image sensor HAL memory init
 *  @note the function is intended for internal use only
 */
static void AmbaImgSensorHAL_MemInit(void)
{
#define AMBA_IMG_SENSOR_HAL_MARK_DATA_NUM    (512U/*128U*/)
    static AMBA_IMG_SENSOR_HAL_MARK_DATA_s GNU_SECTION_NOZEROINIT AmbaImgSensorHAL_MarkData[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_SENSOR_HAL_MARK_DATA_NUM];
    static AMBA_IMG_SENSOR_HAL_CONTEXT_s GNU_SECTION_NOZEROINIT AmbaImgSensorHAL_Context[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];

    static AMBA_IMG_SENSOR_HAL_HDR_CONTEXT_s GNU_SECTION_NOZEROINIT AmbaImgSensorHAL_HdrContext[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];
    static AMBA_IMG_SENSOR_HAL_USER_CONTEXT_s GNU_SECTION_NOZEROINIT AmbaImgSensorHAL_UserContext[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];

    static AMBA_IMG_SENSOR_HAL_CHAN_s GNU_SECTION_NOZEROINIT AmbaImgSensorHAL_VinChain[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO+1UL];

    UINT32 FuncRetCode;
    UINT32 i, j;

    /* aux hook */
    for (i = 0U; i < AMBA_IMG_NUM_VIN_CHANNEL; i++) {
        for (j = 0U; j < AMBA_IMG_NUM_VIN_ALGO; j++) {
            AmbaImgSensorHAL_Info[i][j].pAux = &(AmbaImgSensorHAL_AuxInfo[i]);
        }
    }

    /* ctx clear */
    FuncRetCode = AmbaWrap_memset(AmbaImgSensorHAL_Context, 0, sizeof(AmbaImgSensorHAL_Context));
    if (FuncRetCode != OK_UL) {
        /* */
    }

    /* info hook */
    for (i = 0U; i < AMBA_IMG_NUM_VIN_CHANNEL; i++) {
        for (j = 0U; j < AMBA_IMG_NUM_VIN_ALGO; j++) {
            AmbaImgSensorHAL_Context[i][j].pInfo = &(AmbaImgSensorHAL_Info[i][j]);
            AmbaImgSensorHAL_HdrContext[i][j].pInfo = &(AmbaImgSensorHAL_HdrInfo[i][j]);
        }
    }

    /* chain clear */
    FuncRetCode = AmbaWrap_memset(AmbaImgSensorHAL_VinChain, 0, sizeof(AmbaImgSensorHAL_VinChain));
    if (FuncRetCode != OK_UL) {
        /* */
    }

    /* ctx, hdr and user hook */
    for (i = 0U; i < AMBA_IMG_NUM_VIN_CHANNEL; i++) {
        for (j = 0U; j < AMBA_IMG_NUM_VIN_ALGO; j++) {
            AmbaImgSensorHAL_VinChain[i][j].Magic = 0xCafeU;
            AmbaImgSensorHAL_VinChain[i][j].pCtx = &(AmbaImgSensorHAL_Context[i][j]);
            AmbaImgSensorHAL_VinChain[i][j].pHdrCtx = &(AmbaImgSensorHAL_HdrContext[i][j]);
            AmbaImgSensorHAL_VinChain[i][j].pUserCtx = &(AmbaImgSensorHAL_UserContext[i][j]);
        }
    }

    /* chain hook */
    for (i = 0U; i < AMBA_IMG_NUM_VIN_CHANNEL; i++) {
        AmbaImgSensorHAL_Chan[i] = AmbaImgSensorHAL_VinChain[i];
    }

    /* vin reset, timing mark hook */
    for (i = 0U; i < AMBA_IMG_NUM_VIN_CHANNEL; i++) {
        AmbaImgSensorHAL_Vin[i].Debug.Data = 0U;
        AmbaImgSensorHAL_Vin[i].Op.Data = 0U;
        AmbaImgSensorHAL_Vin[i].Timing.Op.Data = 0U;
        AmbaImgSensorHAL_Vin[i].Timing.Msg.Num = AMBA_IMG_SENSOR_HAL_MARK_DATA_NUM;
        AmbaImgSensorHAL_Vin[i].Timing.Msg.Data = AmbaImgSensorHAL_MarkData[i];
        AmbaImgSensorHAL_Vin[i].Counter.Raw = 0U;
        AmbaImgSensorHAL_Vin[i].pAux = &(AmbaImgSensorHAL_AuxInfo[i]);
    }
}

/**
 *  Amba image sensor sync memory init
 *  @note the function is intended for internal use only
 */
static void AmbaImgSensorSync_MemInit(void)
{
    static AMBA_IMG_SENSOR_SYNC_CONTEXT_s GNU_SECTION_NOZEROINIT AmbaImgSensorSync_Context[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];
    static AMBA_IMG_SENSOR_SYNC_CHAN_s GNU_SECTION_NOZEROINIT AmbaImgSensorSync_VinChain[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO+1UL];

    UINT32 FuncRetCode;
    UINT32 i, j;

    /* chain clear */
    FuncRetCode = AmbaWrap_memset(AmbaImgSensorSync_VinChain, 0, sizeof(AmbaImgSensorSync_VinChain));
    if (FuncRetCode != OK_UL) {
        /* */
    }

    /* ctx hook */
    for (i = 0U; i < AMBA_IMG_NUM_VIN_CHANNEL; i++) {
        for (j = 0U; j < AMBA_IMG_NUM_VIN_ALGO; j++) {
            AmbaImgSensorSync_VinChain[i][j].Magic = 0xCafeU;
            AmbaImgSensorSync_VinChain[i][j].pCtx = &(AmbaImgSensorSync_Context[i][j]);
        }
    }

    /* chain hook */
    for (i = 0U; i < AMBA_IMG_NUM_VIN_CHANNEL; i++) {
        AmbaImgSensorSync_Chan[i] = AmbaImgSensorSync_VinChain[i];
    }

    /* vin reset */
    for (i = 0U; i < AMBA_IMG_NUM_VIN_CHANNEL; i++) {
        /* debug */
        AmbaImgSensorSync_Vin[i].Debug.Data = 0U;
    }
}

/**
 *  Amba image sensor drv memory init
 *  @note the function is intended for internal use only
 */
static void AmbaImgSensorDrv_MemInit(void)
{
    static AMBA_IMG_SENSOR_DRV_CHAN_s GNU_SECTION_NOZEROINIT AmbaImgSensorDrv_VinChain[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO+1UL];

    UINT32 FuncRetCode;
    UINT32 i, j;

    /* chain clear */
    FuncRetCode = AmbaWrap_memset(AmbaImgSensorDrv_VinChain, 0, sizeof(AmbaImgSensorDrv_VinChain));
    if (FuncRetCode != OK_UL) {
        /* */
    }

    /* ctx hook */
    for (i = 0U; i < AMBA_IMG_NUM_VIN_CHANNEL; i++) {
        for (j = 0U; j < AMBA_IMG_NUM_VIN_ALGO; j++) {
            AmbaImgSensorDrv_VinChain[i][j].Magic = 0xCafeU;
            AmbaImgSensorDrv_VinChain[i][j].pFunc = &AmbaImgSensorDrv_ChanFunc;
        }
    }

    /* chain hook */
    for (i = 0U; i < AMBA_IMG_NUM_VIN_CHANNEL; i++) {
        AmbaImgSensorDrv_Chan[i] = AmbaImgSensorDrv_VinChain[i];
    }
}

/**
 *  @private
 *  Amba image channel user init
 *  @return error code
 *  @note the function is intended for internal use only
 */
UINT32 AmbaImgMain_ChanMemInit(void)
{
    UINT32 RetCode = OK_UL;

    /* image channel memory init */
    AmbaImgChannel_MemInit();

    /* image system memory init */
    AmbaImgSystem_MemInit();

    /* image event memory init */
    AmbaImgEvent_MemInit();

    /* image message memory init */
    AmbaImgMessage_MemInit();

    /* image statistics memory init */
    AmbaImgStatistics_MemInit();

    /* image sensor HAL memory init */
    AmbaImgSensorHAL_MemInit();

    /* image sensor sync memory init */
    AmbaImgSensorSync_MemInit();

    /* image sensor drv memory init */
    AmbaImgSensorDrv_MemInit();

    return RetCode;
}
