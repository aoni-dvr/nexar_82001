/**
 *  @file AmbaImgMainEis.c
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
 *  @details Amba Image Main Eis
 *
 */

#include "AmbaTypes.h"
#include "AmbaVIN_Def.h"

#include "AmbaKAL.h"
#include "AmbaWrap.h"

#include "AmbaSensor.h"

#include "AmbaDSP_ImageDefine.h"
#include "AmbaDSP_ImageFilter.h"
#include "AmbaDSP_ImageUtility.h"
#ifdef CONFIG_BUILD_IMGFRW_EIS
#include "AmbaImg_ImuManager.h"
#include "AmbaEIS_Control.h"
#include "AmbaEIS_Common.h"
#include "AmbaEIS_DeCfg.c"
#endif

#ifdef CONFIG_BUILD_IMGFRW_EIS_WARP_AC
#include "warputility/AmbaWU_WarpUtility.h"
#endif
#include "AmbaImgFramework.h"
#include "AmbaImgMain.h"
#include "AmbaImgMain_Internal.h"
#include "AmbaImgMainEis.h"
#include "AmbaImgMainEis_Platform.h"

#define OK_UL    ((UINT32) 0U)
#define NG_UL    ((UINT32) 1U)

#define IMG_MAX_WARP_HGRID_NUM    256U
#define IMG_MAX_WARP_VGRID_NUM    192U
#define IMG_MAX_WARP_GRID_SIZE    (IMG_MAX_WARP_HGRID_NUM*IMG_MAX_WARP_VGRID_NUM)

typedef union /*_EIS_MEM_INFO_u_*/ {
    AMBA_IMG_MAIN_MEM_ADDR           Data;
    void                             *pVoid;
    UINT8                            *pUint8;
    AMBA_IMG_SYSTEM_PAIK_INFO_s      *pPostAikInfo;
    AMBA_IMG_MAIN_EIS_USER_DATA_s    *pUserData;
#ifdef CONFIG_BUILD_IMGFRW_EIS
    UINT32                           *pUint32;
    AMBA_IMG_IMUMGR_EVENT_DATA_s     *pTimingData;
#endif
#ifdef CONFIG_BUILD_IMGFRW_EIS_WARP_AC
    AMBA_IK_WARP_INFO_s              *pWarpInfo;
#endif
#if defined(CONFIG_BUILD_IMGFRW_EIS_WARP_AC) || defined(CONFIG_BUILD_IMGFRW_EIS_WARP_CT)
    AMBA_IK_GRID_POINT_s             *pWarp;
#endif
} EIS_MEM_INFO_u;

typedef struct /*_EIS_MEM_INFO_s_*/ {
    EIS_MEM_INFO_u Ctx;
} EIS_MEM_INFO_s;

typedef struct /*_AMBA_IMG_MAIN_EIS_TASK_INFO_s_*/ {
    char Name[32];
} AMBA_IMG_MAIN_EIS_TASK_INFO_s;

static AMBA_KAL_TASK_t GNU_SECTION_NOZEROINIT AmbaImgMainEisTaskId[AMBA_IMG_NUM_EIS_CHANNEL];
static void AmbaImgMainEis_Task(UINT32 Param);
static void *AmbaImgMainEis_TaskEx(void *pParam);

static AMBA_KAL_TASK_t GNU_SECTION_NOZEROINIT AmbaImgMainEisAlgoTaskId[AMBA_IMG_NUM_EIS_CHANNEL];
static void AmbaImgMainEis_AlgoTask(UINT32 Param);
static void *AmbaImgMainEis_AlgoTaskEx(void *pParam);

static AMBA_KAL_MSG_QUEUE_t GNU_SECTION_NOZEROINIT AmbaImgMainEis_AlgoQueue[AMBA_IMG_NUM_EIS_CHANNEL];

static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaImgMainEis_Ready[AMBA_IMG_NUM_EIS_CHANNEL];
static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaImgMainEis_Go[AMBA_IMG_NUM_EIS_CHANNEL];
static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaImgMainEis_Ack[AMBA_IMG_NUM_EIS_CHANNEL];

static UINT32 GNU_SECTION_NOZEROINIT AmbaImgMainEis_Cmd[AMBA_IMG_NUM_EIS_CHANNEL];

static AMBA_IMG_CHANNEL_ID_s AmbaImgMainEis_ImageChanId[AMBA_IMG_NUM_EIS_CHANNEL];
UINT32 AmbaImgMainEis_LastIndex = 0U;
#ifdef CONFIG_BUILD_IMGFRW_EIS
static UINT32 AmbaImgMainEis_ImuIdTable[AMBA_IMG_NUM_EIS_CHANNEL] = {0};
static AMBA_IMG_IMUMGR_FIFO_PORT_s AmbaImgMainEis_ImgImuPort[AMBA_IMG_NUM_EIS_CHANNEL];
#else
#define DUMMY_EIS_ZOOM_FACTOR 1200U
#define DUMMY_EIS_DUMMY_FACTOR 1200U
#define DUMMY_EIS_SHIFT_FACTOR 1120U
static AMBA_KAL_MUTEX_t AmbaImgMainEis_ParamUpdateMutex[AMBA_IMG_NUM_EIS_CHANNEL];
static AMBA_IMG_MAIN_EIS_PARAM_s AmbaImgMainEis_Param[AMBA_IMG_NUM_EIS_CHANNEL][2];
static AMBA_KAL_MUTEX_t AmbaImgMainEis_WarpUpdateMutex[AMBA_IMG_NUM_EIS_CHANNEL];
static AMBA_IMG_MAIN_EIS_WARP_SHIFT_s AmbaImgMainEis_WarpShift[AMBA_IMG_NUM_EIS_CHANNEL][2];
#endif
static UINT32 AmbaImgMainEis_R2yUserDelay[AMBA_IMG_NUM_EIS_CHANNEL] = {0};
#ifdef CONFIG_BUILD_IMGFRW_EIS_GYRODLY
static UINT32 AmbaImgMainEis_GyroUserDelay[AMBA_IMG_NUM_EIS_CHANNEL] = {0};
#endif
#ifdef CONFIG_BUILD_IMGFRW_EIS_WARP_AC
static AMBA_IK_WARP_INFO_s AmbaImgMainEis_LivWarpInfo[AMBA_IMG_SYSTEM_FOV_NUM] = {0};
#endif
#if defined(CONFIG_BUILD_IMGFRW_EIS_WARP_AC) || defined(CONFIG_BUILD_IMGFRW_EIS_WARP_CT)
static void AmbaImgMainEis_WarpInfoDebug(const char *pName, UINT32 FovId, const AMBA_IK_WARP_INFO_s *pWarpInfo);
#endif
static FLOAT AmbaImgMainEis_ShutterTime[AMBA_IMG_NUM_EIS_CHANNEL][AMBA_IMG_NUM_EXPOSURE_CHANNEL];
static FLOAT AmbaImgMainEis_LastShutterTime[AMBA_IMG_NUM_EIS_CHANNEL][AMBA_IMG_NUM_EXPOSURE_CHANNEL];

static UINT32 AmbaImgMainEis_ImgCore[AMBA_IMG_NUM_EIS_CHANNEL] = {0};

static UINT32 AmbaImgMainEis_DummyFlag[AMBA_IMG_NUM_EIS_CHANNEL];

static UINT32 AmbaImgMainEis_AlgoDummyFlag[AMBA_IMG_NUM_EIS_CHANNEL];

UINT32 AmbaImgMainEis_DebugCount[AMBA_IMG_NUM_EIS_CHANNEL] = {0};
static UINT32 GNU_SECTION_NOZEROINIT AmbaImgMainEis_IndexStrNum;

static UINT32 AmbaImgMainEis_Create(UINT32 VinId, UINT32 Priority, UINT32 CoreInclusion);
static UINT32 AmbaImgMainEis_Delete(UINT32 VinId);
static UINT32 AmbaImgMainEis_Active(UINT32 VinId);
static UINT32 AmbaImgMainEis_Inactive(UINT32 VinId);
static UINT32 AmbaImgMainEis_IdleWait(UINT32 VinId);

/**
 *  Amba image main eis image framework dispatch
 *  @param[in] VinId vin id
 *  @param[in] Param command and message
 *  @note this function is intended for internal use only
 */
static void AmbaImgMainEis_ImgFrwDispatch(UINT32 VinId, UINT64 Param)
{
    AMBA_IMG_MAIN_CMD_MSG_s CmdMsg;

    CmdMsg.Ctx.Data = Param;

    if (VinId < AMBA_IMG_NUM_VIN_CHANNEL) {
        switch (CmdMsg.Ctx.Bits.Cmd) {
            case (UINT8) IMAGE_CMD_CHAN:
                switch (CmdMsg.Ctx.Bits.Msg) {
                    case (UINT8) CHAN_MSG_EIS:
                        AmbaImgPrint2(PRINT_FLAG_DBG, "chan-msg-eis", VinId, (UINT32) CmdMsg.Ctx.ImageChanEis.Msg);
                        AmbaImgPrint2(PRINT_FLAG_DBG, "chain id", VinId, CmdMsg.Ctx.ImageChanEis.ChainId);
                        AmbaImgPrint2(PRINT_FLAG_DBG, "eis id", VinId, CmdMsg.Ctx.ImageChanEis.Id);
                        AmbaImgPrint2(PRINT_FLAG_DBG, "algo id", VinId, CmdMsg.Ctx.ImageChanEis.AlgoId);
                        AmbaImgPrint2(PRINT_FLAG_DBG, "zone id", VinId, CmdMsg.Ctx.ImageChanEis.ZoneId);
                        AmbaImgMainEis_ImageChanId[CmdMsg.Ctx.ImageChanEis.Id].Ctx.Bits.VinId = (UINT8) VinId;
                        AmbaImgMainEis_ImageChanId[CmdMsg.Ctx.ImageChanEis.Id].Ctx.Bits.ChainId = CmdMsg.Ctx.ImageChanEis.ChainId;
                        AmbaImgMainEis_ImageChanId[CmdMsg.Ctx.ImageChanEis.Id].Ctx.Bits.AlgoId = CmdMsg.Ctx.ImageChanEis.AlgoId;
                        AmbaImgMainEis_ImageChanId[CmdMsg.Ctx.ImageChanEis.Id].Ctx.Bits.ZoneId = CmdMsg.Ctx.ImageChanEis.ZoneId;
                        AmbaImgMainEis_LastIndex = (UINT32) CmdMsg.Ctx.ImageChanEis.Id;
                        break;
                    case (UINT8) CHAN_MSG_EIS_CORE:
                        AmbaImgPrint(PRINT_FLAG_DBG, "chan-msg-eis-core", (UINT32) CmdMsg.Ctx.ImageChanCore.Msg);
                        AmbaImgPrint(PRINT_FLAG_DBG, "chain id", CmdMsg.Ctx.ImageChanCore.ChainId);
                        AmbaImgPrint(PRINT_FLAG_DBG, "eis id", CmdMsg.Ctx.ImageChanCore.AuxId);
                        AmbaImgPrint(PRINT_FLAG_DBG, "core id", CmdMsg.Ctx.ImageChanCore.Id);
                        AmbaImgMainEis_ImgCore[CmdMsg.Ctx.ImageChanCore.AuxId] = CmdMsg.Ctx.ImageChanCore.Id;
                        break;
                    case (UINT8) CHAN_MSG_R2Y_USER_DELAY:
                        AmbaImgPrint(PRINT_FLAG_DBG, "chan-msg-r2y-user-delay", (UINT32) CmdMsg.Ctx.Bits3.Msg);
                        AmbaImgPrint(PRINT_FLAG_DBG, "chain id", CmdMsg.Ctx.Bits3.ChainId);
                        AmbaImgPrint(PRINT_FLAG_DBG, "eis id", CmdMsg.Ctx.Bits3.AuxId);
                        AmbaImgPrint(PRINT_FLAG_DBG, "r2y user delay", CmdMsg.Ctx.Bits3.Var);
                        if (CmdMsg.Ctx.Bits3.Var > 0U) {
                            AmbaImgMainEis_R2yUserDelay[CmdMsg.Ctx.Bits3.AuxId] = CmdMsg.Ctx.Bits3.Var;
                        }
                        break;
#ifdef CONFIG_BUILD_IMGFRW_EIS_GYRODLY
                    case (UINT8) CHAN_MSG_GYRO_USER_DELAY:
                        AmbaImgPrint(PRINT_FLAG_DBG, "chan-msg-gyro-user-delay", (UINT32) CmdMsg.Ctx.Bits3.Msg);
                        AmbaImgPrint(PRINT_FLAG_DBG, "chain id", CmdMsg.Ctx.Bits3.ChainId);
                        AmbaImgPrint(PRINT_FLAG_DBG, "eis id", CmdMsg.Ctx.Bits3.AuxId);
                        AmbaImgPrint(PRINT_FLAG_DBG, "gyro user delay", CmdMsg.Ctx.Bits3.Var);
                        if (CmdMsg.Ctx.Bits3.Var > 0U) {
                            AmbaImgMainEis_GyroUserDelay[CmdMsg.Ctx.Bits3.AuxId] = CmdMsg.Ctx.Bits3.Var;
                        }
                        break;
#endif
                    default:
                        /* */
                        break;
                }
                break;
            default:
                /* */
                break;
        }
    }
}
#ifdef CONFIG_BUILD_IMGFRW_EIS_WARP_AC
/**
 *  Amba image main eis image framework dispatch ex
 *  @param[in] VinId vin id
 *  @param[in] Param command and message
 *  @param[in] Param2 aux data
 *  @note this function is intended for internal use only
 */
static void AmbaImgMainEis_ImgFrwDispatchEx(UINT32 VinId, UINT64 Param, UINT64 Param2)
{
    UINT32 FuncRetCode;
    UINT32 i;

    EIS_MEM_INFO_s MemInfo;

    AMBA_IMG_MAIN_CMD_MSG_s CmdMsg;
    AMBA_IMG_MAIN_AUX_DATA_s AuxData;

    CmdMsg.Ctx.Data = Param;
    AuxData.Ctx.Data = Param2;

    if (VinId < AMBA_IMG_NUM_VIN_CHANNEL) {
        switch (CmdMsg.Ctx.Bits.Cmd) {
            case (UINT8) IMAGE_CMD_CHAN:
                switch (CmdMsg.Ctx.Bits.Msg) {
                    case (UINT8) CHAN_MSG_FOV_WARP:
                        /* liv (LDC) warp info get (2D warp table needed) (TBD) */
                        AmbaImgPrint2(PRINT_FLAG_DBG, "chan-msg-lv-warp", VinId, (UINT32) CmdMsg.Ctx.Bits.Msg);
                        AmbaImgPrint2(PRINT_FLAG_DBG, "zone id", VinId, CmdMsg.Ctx.Bits.Var);
                        /* zone id check */
                        for (i = 0U; (i < AMBA_IMG_SYSTEM_FOV_NUM) && ((CmdMsg.Ctx.Bits.Var >> i) > 0U); i++) {
                            if ((CmdMsg.Ctx.Bits.Var & (((UINT32) 1U) << i)) > 0U) {
                                /* liv warp info copy */
                                MemInfo.Ctx.Data = (AMBA_IMG_MAIN_MEM_ADDR) AuxData.Ctx.Bits.Var;
                                FuncRetCode = AmbaWrap_memcpy(&(AmbaImgMainEis_LivWarpInfo[i]), MemInfo.Ctx.pWarpInfo, sizeof(AMBA_IK_WARP_INFO_s));
                                if (FuncRetCode == OK_UL) {
                                    /* debug message */
                                    AmbaImgMainEis_WarpInfoDebug("liv warp", i, &(AmbaImgMainEis_LivWarpInfo[i]));
                                } else {
                                    /* fail */
                                    AmbaImgPrint(PRINT_FLAG_ERR, "error: eis liv warp info", i);
                                }
                            }
                        }
                        break;
                    default:
                        /* */
                        break;
                }
                break;
            default:
                /* */
                break;
        }
    }
}
#endif
/**
 *  Amba image main eis algorithm dispatch
 *  @param[in] VinId vin id
 *  @param[in] Param command and message
 *  @note this function is intended for internal use only
 */
static void AmbaImgMainEis_AlgoDispatch(UINT32 VinId, UINT64 Param)
{
    AMBA_IMG_MAIN_CMD_MSG_s CmdMsg;

    CmdMsg.Ctx.Data = Param;

    if (VinId < AMBA_IMG_NUM_VIN_CHANNEL) {
        switch (CmdMsg.Ctx.Bits.Cmd) {
            case (UINT8) IMG_EIS_CMD_START:
                /* */
                AmbaImgPrint(PRINT_FLAG_DBG, "eis-cmd-start", VinId);
                break;
            default:
                /* */
                break;
        }
    }
}

/**
 *  Amba image main eis task dispatch
 *  @param[in] VinId vin id
 *  @param[in] Param command and message
 *  @note this function is intended for internal use only
 */
static void AmbaImgMainEis_TaskDispatch(UINT32 VinId, UINT64 Param)
{
    UINT32 FuncRetCode;

    AMBA_IMG_MAIN_CMD_MSG_s CmdMsg;

    CmdMsg.Ctx.Data = Param;

    if (VinId < AMBA_IMG_NUM_VIN_CHANNEL) {
        switch (CmdMsg.Ctx.Bits.Cmd) {
            case (UINT8) IMG_EIS_CMD_START:
                /* eis task create */
                FuncRetCode = AmbaImgMainEis_Create(VinId, 5U, 0x01U);
                if (FuncRetCode == OK_UL) {
                    /* eis task idle wait */
                    FuncRetCode = AmbaImgMainEis_IdleWait(VinId);
                    if (FuncRetCode == OK_UL) {
                        /* eis task active */
                        FuncRetCode = AmbaImgMainEis_Active(VinId);
                        if (FuncRetCode != OK_UL) {
                            AmbaImgPrintStr(PRINT_FLAG_ERR, "error: eis task active");
                        }
                    } else {
                        AmbaImgPrintStr(PRINT_FLAG_ERR, "error: eis task ready");
                    }
                } else {
                    AmbaImgPrintStr(PRINT_FLAG_ERR, "error: eis task create");
                }
                break;
            case (UINT8) IMG_EIS_CMD_STOP:
                /* eis task inactive */
                FuncRetCode = AmbaImgMainEis_Inactive(VinId);
                if (FuncRetCode == OK_UL) {
                    /* eis task idle wait */
                    FuncRetCode = AmbaImgMainEis_IdleWait(VinId);
                    if (FuncRetCode != OK_UL) {
                        AmbaImgPrintStr(PRINT_FLAG_ERR, "error: eis task idle");
                    }
                } else {
                    AmbaImgPrintStr(PRINT_FLAG_ERR, "error: eis task inactive");
                }
                /* eis task delete */
                FuncRetCode = AmbaImgMainEis_Delete(VinId);
                if (FuncRetCode != OK_UL) {
                    AmbaImgPrintStr(PRINT_FLAG_ERR, "error: eis task delete");
                }
                break;
            default:
                /* */
                break;
        }
    }
}

/**
 *  @private
 *  Amba image main eis dispatch
 *  @param[in] VinId vin id
 *  @param[in] Param command and message
 *  @note this function is intended for internal use only
 */
void AmbaImgMainEis_Dispatch(UINT32 VinId, UINT64 Param)
{
    /* image framework */
    AmbaImgMainEis_ImgFrwDispatch(VinId, Param);
    /* algorithm */
    AmbaImgMainEis_AlgoDispatch(VinId, Param);
    /* task */
    AmbaImgMainEis_TaskDispatch(VinId, Param);
}
#ifdef CONFIG_BUILD_IMGFRW_EIS_WARP_AC
/**
 *  @private
 *  Amba image main eis dispatch ex
 *  @param[in] VinId vin id
 *  @param[in] Param command and message
 *  @param[in] Param2 aux data
 *  @note this function is intended for internal use only
 */
void AmbaImgMainEis_DispatchEx(UINT32 VinId, UINT64 Param, UINT64 Param2)
{
    /* image framework */
    AmbaImgMainEis_ImgFrwDispatchEx(VinId, Param, Param2);
}
#endif
/**
 *  Amba image main eis create
 *  @param[in] VinId vin id
 *  @param[in] Priority task priority id
 *  @param[in] CoreInclusion cpu core inclusion (bits)
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgMainEis_Create(UINT32 VinId, UINT32 Priority, UINT32 CoreInclusion)
{
#define AMBA_IMG_MAIN_EIS_TASK_STACK_SIZE    0xA000
    static UINT8 GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE AmbaImgMainEis_TaskStack[AMBA_IMG_NUM_EIS_CHANNEL][AMBA_IMG_MAIN_EIS_TASK_STACK_SIZE];
    static AMBA_IMG_MAIN_EIS_TASK_INFO_s GNU_SECTION_NOZEROINIT AmbaImgMainEis_TaskInfo[AMBA_IMG_NUM_EIS_CHANNEL];
#define AMBA_IMG_MAIN_EIS_ALGO_TASK_STACK_SIZE    0x10000
    static UINT8 GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE AmbaImgMainEis_AlgoTaskStack[AMBA_IMG_NUM_EIS_CHANNEL][AMBA_IMG_MAIN_EIS_ALGO_TASK_STACK_SIZE];
    static AMBA_IMG_MAIN_EIS_TASK_INFO_s GNU_SECTION_NOZEROINIT AmbaImgMainEis_AlgoTaskInfo[AMBA_IMG_NUM_EIS_CHANNEL];
#define IMG_EIS_MSG_SIZE    8U /*64-bits*/
#define IMG_EIS_MSG_NUM     8U
#define IMG_EIS_QUEUE_SIZE  (IMG_EIS_MSG_SIZE*IMG_EIS_MSG_NUM)
    static UINT8 GNU_SECTION_NOZEROINIT AmbaImgMainEis_AlgoQueueBuffer[AMBA_IMG_NUM_EIS_CHANNEL][IMG_EIS_QUEUE_SIZE];

    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    for (i = 0U; i <= AmbaImgMainEis_LastIndex; i++) {
        if ((AmbaImgMainEis_ImageChanId[i].Ctx.Bits.VinId == VinId) &&
            (AmbaImgMainEis_ImageChanId[i].Ctx.Bits.ZoneId > 0U)) {
            /* semaphore */
            FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaImgMainEis_Ready[i], NULL, 0U);
            if (FuncRetCode != KAL_ERR_NONE) {
                RetCode = NG_UL;
            }
            FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaImgMainEis_Go[i], NULL, 0);
            if (FuncRetCode != KAL_ERR_NONE) {
                RetCode = NG_UL;
            }
            FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaImgMainEis_Ack[i], NULL, 0);
            if (FuncRetCode != KAL_ERR_NONE) {
                RetCode = NG_UL;
            }
            /* task name */
            {
                char str[11];
                UINT32 IndexStrNum = (AmbaImgMainEis_IndexStrNum > 9U) ? (UINT32) 2U : (UINT32) 1U;
                AmbaImgMainEis_TaskInfo[i].Name[0] = '\0';
                (void) var_strcat(AmbaImgMainEis_TaskInfo[i].Name, "ImageEis");
                var_utoa(i, str, 10U, IndexStrNum, (UINT32) VAR_LEADING_ZERO);
                (void) var_strcat(AmbaImgMainEis_TaskInfo[i].Name, str);
            }
            /* task */
            if (RetCode == OK) {
                AmbaImgPrint(PRINT_FLAG_MSG, "image create eis task", i);
                {
                    EIS_MEM_INFO_s MemInfo;
                    UINT32 Data = ((UINT32) i) | (((UINT32) AmbaImgMainEis_ImageChanId[i].Ctx.Bits.AlgoId) << 16U);
                    MemInfo.Ctx.Data = (AMBA_IMG_MAIN_MEM_ADDR) Data;
                    FuncRetCode = AmbaKAL_TaskCreate(&AmbaImgMainEisTaskId[i],
                                                     AmbaImgMainEis_TaskInfo[i].Name,
                                                     Priority,
                                                     AmbaImgMainEis_TaskEx,
                                                     MemInfo.Ctx.pVoid,
                                                     AmbaImgMainEis_TaskStack[i],
                                                     AMBA_IMG_MAIN_EIS_TASK_STACK_SIZE,
                                                     AMBA_KAL_DONT_START);
                }
                if (FuncRetCode == OK) {
                    UINT32 EisCoreInclusion = CoreInclusion;
                    if (AmbaImgMainEis_ImgCore[i] > 0U) {
                        EisCoreInclusion = AmbaImgMainEis_ImgCore[i];
                    }
                    FuncRetCode = AmbaKAL_TaskSetSmpAffinity(&AmbaImgMainEisTaskId[i], EisCoreInclusion);
                    if (FuncRetCode != KAL_ERR_NONE) {
                        RetCode = NG_UL;
                    }
                    FuncRetCode = AmbaKAL_TaskResume(&AmbaImgMainEisTaskId[i]);
                    if (FuncRetCode != KAL_ERR_NONE) {
                        RetCode = NG_UL;
                    }
                } else {
                    RetCode = NG_UL;
                }
            }

            /* algo task name */
            {
                char str[11];
                AmbaImgMainEis_AlgoTaskInfo[i].Name[0] = '\0';
                (void) var_strcat(AmbaImgMainEis_AlgoTaskInfo[i].Name, "ImageEisAlgo");
                var_utoa(i, str, 10U, 1U, (UINT32) VAR_LEADING_ZERO);
                (void) var_strcat(AmbaImgMainEis_AlgoTaskInfo[i].Name, str);
            }
            /* algo queue */
            FuncRetCode = AmbaKAL_MsgQueueCreate(&(AmbaImgMainEis_AlgoQueue[i]), AmbaImgMainEis_AlgoTaskInfo[i].Name, IMG_EIS_MSG_SIZE, AmbaImgMainEis_AlgoQueueBuffer[i] , IMG_EIS_QUEUE_SIZE);
            if (FuncRetCode != KAL_ERR_NONE) {
                RetCode = NG_UL;
            }
            /* algo task */
            if (RetCode == OK) {
                AmbaImgPrint(PRINT_FLAG_MSG, "image create eis algo task", i);
                {
                    EIS_MEM_INFO_s MemInfo;
                    UINT32 Data = ((UINT32) i) | (((UINT32) AmbaImgMainEis_ImageChanId[i].Ctx.Bits.AlgoId) << 16U);
                    MemInfo.Ctx.Data = (AMBA_IMG_MAIN_MEM_ADDR) Data;
                    FuncRetCode = AmbaKAL_TaskCreate(&AmbaImgMainEisAlgoTaskId[i],
                                                     AmbaImgMainEis_AlgoTaskInfo[i].Name,
                                                     Priority+3U,
                                                     AmbaImgMainEis_AlgoTaskEx,
                                                     MemInfo.Ctx.pVoid,
                                                     AmbaImgMainEis_AlgoTaskStack[i],
                                                     AMBA_IMG_MAIN_EIS_ALGO_TASK_STACK_SIZE,
                                                     AMBA_KAL_DONT_START);
                }
                if (FuncRetCode == OK) {
                    UINT32 AlgoCoreInclusion = CoreInclusion;
                    if (AmbaImgMainEis_ImgCore[i] > 0U) {
                        AlgoCoreInclusion = AmbaImgMainEis_ImgCore[i];
                    }
                    FuncRetCode = AmbaKAL_TaskSetSmpAffinity(&AmbaImgMainEisAlgoTaskId[i], AlgoCoreInclusion);
                    if (FuncRetCode != KAL_ERR_NONE) {
                        RetCode = NG_UL;
                    }
                    FuncRetCode = AmbaKAL_TaskResume(&AmbaImgMainEisAlgoTaskId[i]);
                    if (FuncRetCode != KAL_ERR_NONE) {
                        RetCode = NG_UL;
                    }
                } else {
                    RetCode = NG_UL;
                }
            }

        }
    }

    return RetCode;
}

/**
 *  Amba image main eis delete
 *  @param[in] VinId vin id
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgMainEis_Delete(UINT32 VinId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    for (i = 0U; i <= AmbaImgMainEis_LastIndex; i++) {
        if ((AmbaImgMainEis_ImageChanId[i].Ctx.Bits.VinId == VinId) &&
            (AmbaImgMainEis_ImageChanId[i].Ctx.Bits.ZoneId > 0U)) {
            /* task */
            FuncRetCode = AmbaKAL_TaskTerminate(&AmbaImgMainEisTaskId[i]);
            if (FuncRetCode != KAL_ERR_NONE) {
                RetCode = NG_UL;
            }
            FuncRetCode = AmbaKAL_TaskDelete(&AmbaImgMainEisTaskId[i]);
            if (FuncRetCode != KAL_ERR_NONE) {
                RetCode = NG_UL;
            }
            /* semaphore */
            FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaImgMainEis_Ready[i]);
            if (FuncRetCode != KAL_ERR_NONE) {
                RetCode = NG_UL;
            }
            FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaImgMainEis_Go[i]);
            if (FuncRetCode != KAL_ERR_NONE) {
                RetCode = NG_UL;
            }
            FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaImgMainEis_Ack[i]);
            if (FuncRetCode != KAL_ERR_NONE) {
                RetCode = NG_UL;
            }
            /* algo task */
            FuncRetCode = AmbaKAL_TaskTerminate(&AmbaImgMainEisAlgoTaskId[i]);
            if (FuncRetCode != KAL_ERR_NONE) {
                RetCode = NG_UL;
            }
            FuncRetCode = AmbaKAL_TaskDelete(&AmbaImgMainEisAlgoTaskId[i]);
            if (FuncRetCode != KAL_ERR_NONE) {
                RetCode = NG_UL;
            }
            /* algo queue */
            FuncRetCode = AmbaKAL_MsgQueueDelete(&(AmbaImgMainEis_AlgoQueue[i]));
            if (FuncRetCode != KAL_ERR_NONE) {
                RetCode = NG_UL;
            }
        }
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image main eis init
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainEis_Init(void)
{
    UINT32 RetCode = OK_UL;
    UINT32 i, j;

    for (i = 0U; i < AMBA_IMG_NUM_EIS_CHANNEL; i++) {
        /* image channel id init */
        AmbaImgMainEis_ImageChanId[i].Ctx.Data = 0ULL;
        /* dummy flag init */
        AmbaImgMainEis_DummyFlag[i] = 1U;
        /* algo dummy flag init */
        AmbaImgMainEis_AlgoDummyFlag[i] = 1U;
        /* shutter time reset */
        for (j = 0U; j < AMBA_IMG_NUM_EXPOSURE_CHANNEL; j++) {
            AmbaImgMainEis_ShutterTime[i][j] = 0.f;
            AmbaImgMainEis_LastShutterTime[i][j] = 0.f;
        }
        AmbaImgMainEis_R2yUserDelay[i] = 15U;
#ifdef CONFIG_BUILD_IMGFRW_EIS_GYRODLY
        AmbaImgMainEis_GyroUserDelay[i] = 5U;
#endif
    }
    /* eis debug count reset */
    if (AmbaImgMain_DebugCount == 0U) {
        for (i = 0U; i < AMBA_IMG_NUM_EIS_CHANNEL; i++) {
            AmbaImgMainEis_DebugCount[i] = 0xFFFFFFFFU;
        }
    }
#ifndef CONFIG_BUILD_IMGFRW_EIS
    {
        UINT32 FuncRetCode;
        for (i = 0U; i < AMBA_IMG_NUM_EIS_CHANNEL; i++) {
            /* param mutex */
            FuncRetCode = AmbaKAL_MutexCreate(&(AmbaImgMainEis_ParamUpdateMutex[i]), NULL);
            if (FuncRetCode != KAL_ERR_NONE) {
                AmbaImgPrint(PRINT_FLAG_ERR, "error: image main eis param mutex", i);
            }
            /* param settled */
            for (j = 0U; j < 2U; j++) {
                AmbaImgMainEis_Param[i][j].DZoom.Factor = DUMMY_EIS_ZOOM_FACTOR;
                AmbaImgMainEis_Param[i][j].DZoom.ShiftXFactor = 0;
                AmbaImgMainEis_Param[i][j].DZoom.ShiftYFactor = 0;
                AmbaImgMainEis_Param[i][j].DZoom.ShiftXDir = 0;
                AmbaImgMainEis_Param[i][j].DZoom.ShiftYDir = 0;
                AmbaImgMainEis_Param[i][j].DummyRange.XFactor = DUMMY_EIS_DUMMY_FACTOR;
                AmbaImgMainEis_Param[i][j].DummyRange.YFactor = DUMMY_EIS_DUMMY_FACTOR;
                AmbaImgMainEis_Param[i][j].WarpShift.XFactor = DUMMY_EIS_SHIFT_FACTOR;
                AmbaImgMainEis_Param[i][j].WarpShift.YFactor = DUMMY_EIS_SHIFT_FACTOR;
            }
            /* warp shift mutex */
            FuncRetCode = AmbaKAL_MutexCreate(&(AmbaImgMainEis_WarpUpdateMutex[i]), NULL);
            if (FuncRetCode != KAL_ERR_NONE) {
                AmbaImgPrint(PRINT_FLAG_ERR, "error: image main eis warp mutex", i);
            }
            /* warp shift settled */
            for (j = 0U; j < 2U; j++) {
                AmbaImgMainEis_WarpShift[i][j].Mode = 0U;
                AmbaImgMainEis_WarpShift[i][j].Vector.X = 0;
                AmbaImgMainEis_WarpShift[i][j].Vector.Y = 0;
                AmbaImgMainEis_WarpShift[i][j].StepFactor = 0;
                AmbaImgMainEis_WarpShift[i][j].AccumulatedFactor = 0;
                AmbaImgMainEis_WarpShift[i][j].Point[0].X = 0;
                AmbaImgMainEis_WarpShift[i][j].Point[0].Y = 0;
                AmbaImgMainEis_WarpShift[i][j].Point[1].X = 0;
                AmbaImgMainEis_WarpShift[i][j].Point[1].Y = 0;
            }
        }
    }
#endif
#ifdef CONFIG_BUILD_IMGFRW_EIS
    {
        UINT32 FuncRetCode;
        FuncRetCode = AmbaImg_ImuMgr_Init(20U, 0U);
        if (FuncRetCode != OK) {
            AmbaImgPrintStr(PRINT_FLAG_ERR, "error: image imumgr init");
        }
    }
#endif
    AmbaImgMainEis_IndexStrNum = AMBA_IMG_NUM_EIS_CHANNEL;

    return RetCode;
}

/**
 *  Amba image main eis active
 *  @param[in] VinId vin id
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgMainEis_Active(UINT32 VinId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    for (i = 0U; i <= AmbaImgMainEis_LastIndex; i++) {
        if ((AmbaImgMainEis_ImageChanId[i].Ctx.Bits.VinId == VinId) &&
            (AmbaImgMainEis_ImageChanId[i].Ctx.Bits.ZoneId > 0U)) {
            if (AmbaImgMainEis_Cmd[i] == (UINT32) IMG_EIS_TASK_CMD_STOP) {
                FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaImgMainEis_Ready[i], AMBA_KAL_WAIT_FOREVER);
                if (FuncRetCode == KAL_ERR_NONE) {
                    /* cmd start */
                    AmbaImgMainEis_Cmd[i] = (UINT32) IMG_EIS_TASK_CMD_START;
                    /* task go */
                    FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaImgMainEis_Go[i]);
                    if (FuncRetCode == KAL_ERR_NONE) {
                        /* ack take */
                        FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaImgMainEis_Ack[i], AMBA_KAL_WAIT_FOREVER);
                        if (FuncRetCode != KAL_ERR_NONE) {
                            RetCode = NG_UL;
                        }
                    } else {
                        RetCode = NG_UL;
                    }
                } else {
                    RetCode = NG_UL;
                }
            }
        }
    }

    return RetCode;
}

/**
 *  Amba image main eis inactive
 *  @param[in] VinId vin id
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgMainEis_Inactive(UINT32 VinId)
{
    UINT32 i;

    for (i = 0U; i <= AmbaImgMainEis_LastIndex; i++) {
        if ((AmbaImgMainEis_ImageChanId[i].Ctx.Bits.VinId == VinId) &&
            (AmbaImgMainEis_ImageChanId[i].Ctx.Bits.ZoneId > 0U)) {
            if (AmbaImgMainEis_Cmd[i] == (UINT32) IMG_EIS_TASK_CMD_START) {
                AmbaImgMainEis_Cmd[i] = (UINT32) IMG_EIS_TASK_CMD_STOP;
            }
        }
    }

    return OK_UL;
}
#ifndef CONFIG_BUILD_IMGFRW_EIS
/**
 *  @Private
 *  Amba image main eis param update put
 *  @param[in] EisIdx eis index
 *  @param[in] pParam pointer to the dummy eis parameters
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainEis_ParamUpdatePut(UINT32 EisIdx, const AMBA_IMG_MAIN_EIS_PARAM_s *pParam)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    if (EisIdx < AMBA_IMG_NUM_EIS_CHANNEL) {
        /* mutex take */
        FuncRetCode = AmbaKAL_MutexTake(&(AmbaImgMainEis_ParamUpdateMutex[EisIdx]), AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            /* param put */
            AmbaImgMainEis_Param[EisIdx][1].DZoom.Factor = pParam->DZoom.Factor;
            AmbaImgMainEis_Param[EisIdx][1].DZoom.ShiftXFactor = pParam->DZoom.ShiftXFactor;
            AmbaImgMainEis_Param[EisIdx][1].DZoom.ShiftYFactor = pParam->DZoom.ShiftYFactor;
            AmbaImgMainEis_Param[EisIdx][1].DZoom.ShiftXDir = pParam->DZoom.ShiftXDir;
            AmbaImgMainEis_Param[EisIdx][1].DZoom.ShiftYDir = pParam->DZoom.ShiftYDir;
            AmbaImgMainEis_Param[EisIdx][1].DummyRange.XFactor = pParam->DummyRange.XFactor;
            AmbaImgMainEis_Param[EisIdx][1].DummyRange.YFactor = pParam->DummyRange.YFactor;
            AmbaImgMainEis_Param[EisIdx][1].WarpShift.XFactor = pParam->WarpShift.XFactor;
            AmbaImgMainEis_Param[EisIdx][1].WarpShift.YFactor = pParam->WarpShift.YFactor;
            AmbaImgMainEis_Param[EisIdx][1].Update = 1U;
            /* mutex give */
            FuncRetCode = AmbaKAL_MutexGive(&(AmbaImgMainEis_ParamUpdateMutex[EisIdx]));
            if (FuncRetCode != KAL_ERR_NONE) {
                /* */
            }
        } else {
            /* mutex fail */
            RetCode = NG_UL;
        }
    } else {
        /* index fail */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba image main eis param update
 *  @param[in] EisIdx eis index
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgMainEis_ParamUpdate(UINT32 EisIdx)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    if (EisIdx < AMBA_IMG_NUM_EIS_CHANNEL) {
        /* mutex take */
        FuncRetCode = AmbaKAL_MutexTake(&(AmbaImgMainEis_ParamUpdateMutex[EisIdx]), AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            /* update? */
            if (AmbaImgMainEis_Param[EisIdx][1].Update > 0U) {
                /* param put */
                FuncRetCode = AmbaWrap_memcpy(&(AmbaImgMainEis_Param[EisIdx][0]), &(AmbaImgMainEis_Param[EisIdx][1]), sizeof(AMBA_IMG_MAIN_EIS_PARAM_s));
                if (FuncRetCode != OK_UL) {
                    /* copy fail */
                    RetCode = NG_UL;
                }
                /* update clear */
                AmbaImgMainEis_Param[EisIdx][1].Update = 0U;
            }
            /* mutex give */
            FuncRetCode = AmbaKAL_MutexGive(&(AmbaImgMainEis_ParamUpdateMutex[EisIdx]));
            if (FuncRetCode != KAL_ERR_NONE) {
                /* */
            }
        } else {
            /* mutex fail */
            RetCode = NG_UL;
        }
    } else {
        /* index fail */
        RetCode = NG_UL;
    }

    return RetCode;
}
/**
 *  @Private
 *  Amba image main eis warp update put
 *  @param[in] EisIdx eis index
 *  @param[in] pWarpShift pointer to the warp shift data
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainEis_WarpUpdatePut(UINT32 EisIdx, const AMBA_IMG_MAIN_EIS_WARP_SHIFT_s *pWarpShift)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    if (EisIdx < AMBA_IMG_NUM_EIS_CHANNEL) {
        /* mutex take */
        FuncRetCode = AmbaKAL_MutexTake(&(AmbaImgMainEis_WarpUpdateMutex[EisIdx]), AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            /* param put */
            AmbaImgMainEis_WarpShift[EisIdx][1].Mode = pWarpShift->Mode;
            AmbaImgMainEis_WarpShift[EisIdx][1].Vector.X = pWarpShift->Vector.X;
            AmbaImgMainEis_WarpShift[EisIdx][1].Vector.Y = pWarpShift->Vector.Y;
            AmbaImgMainEis_WarpShift[EisIdx][1].StepFactor = pWarpShift->StepFactor;
            AmbaImgMainEis_WarpShift[EisIdx][1].StepDir = pWarpShift->StepDir;
            AmbaImgMainEis_WarpShift[EisIdx][1].AccumulatedFactor = pWarpShift->AccumulatedFactor;
            AmbaImgMainEis_WarpShift[EisIdx][1].Point[0].X = pWarpShift->Point[0].X;
            AmbaImgMainEis_WarpShift[EisIdx][1].Point[0].Y = pWarpShift->Point[0].Y;
            AmbaImgMainEis_WarpShift[EisIdx][1].Point[1].X = pWarpShift->Point[1].X;
            AmbaImgMainEis_WarpShift[EisIdx][1].Point[1].Y = pWarpShift->Point[1].Y;
            AmbaImgMainEis_WarpShift[EisIdx][1].Update = 1U;
            /* mutex give */
            FuncRetCode = AmbaKAL_MutexGive(&(AmbaImgMainEis_WarpUpdateMutex[EisIdx]));
            if (FuncRetCode != KAL_ERR_NONE) {
                /* */
            }
        } else {
            /* mutex fail */
            RetCode = NG_UL;
        }
    } else {
        /* index fail */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba image main eis warp update
 *  @param[in] EisIdx eis index
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgMainEis_WarpUpdate(UINT32 EisIdx)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    if (EisIdx < AMBA_IMG_NUM_EIS_CHANNEL) {
        /* mutex take */
        FuncRetCode = AmbaKAL_MutexTake(&(AmbaImgMainEis_WarpUpdateMutex[EisIdx]), AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            /* update? */
            if (AmbaImgMainEis_WarpShift[EisIdx][1].Update > 0U) {
                /* warp shift put */
                FuncRetCode = AmbaWrap_memcpy(&(AmbaImgMainEis_WarpShift[EisIdx][0]), &(AmbaImgMainEis_WarpShift[EisIdx][1]), sizeof(AMBA_IMG_MAIN_EIS_WARP_SHIFT_s));
                if (FuncRetCode != OK_UL) {
                    /* copy fail */
                    RetCode = NG_UL;
                }
                /* dir preset */
                if (AmbaImgMainEis_WarpShift[EisIdx][0].Mode == IMG_EIS_WARP_SHIFT_LINE) {
                    AmbaImgMainEis_WarpShift[EisIdx][0].StepDir = 1;
                }
                /* update clear */
                AmbaImgMainEis_WarpShift[EisIdx][1].Update = 0U;
            }
            /* mutex give */
            FuncRetCode = AmbaKAL_MutexGive(&(AmbaImgMainEis_WarpUpdateMutex[EisIdx]));
            if (FuncRetCode != KAL_ERR_NONE) {
                /* */
            }
        } else {
            /* mutex fail */
            RetCode = NG_UL;
        }
    } else {
        /* index fail */
        RetCode = NG_UL;
    }

    return RetCode;
}
#endif
/**
 *  Amba image main eis idle wait
 *  @param[in] VinId vin id
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgMainEis_IdleWait(UINT32 VinId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    for (i = 0U; i <= AmbaImgMainEis_LastIndex; i++) {
        if ((AmbaImgMainEis_ImageChanId[i].Ctx.Bits.VinId == VinId) &&
            (AmbaImgMainEis_ImageChanId[i].Ctx.Bits.ZoneId > 0U)) {
            FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaImgMainEis_Ready[i], AMBA_KAL_WAIT_FOREVER);
            if (FuncRetCode == KAL_ERR_NONE) {
                FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaImgMainEis_Ready[i]);
                if (FuncRetCode != KAL_ERR_NONE) {
                    RetCode = NG_UL;
                }
            } else {
                RetCode = NG_UL;
            }
        }
    }

    return RetCode;
}

/**
 *  Amba image main eis sof
 *  @param[in] ImageChanId image channel id
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgMainEis_Sof(AMBA_IMG_CHANNEL_ID_s ImageChanId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i, j;

    UINT32 VinId;
    UINT32 ChainId;

    AMBA_SENSOR_CHANNEL_s SensorChanId;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    for (i = 0U; i <= AmbaImgMainEis_LastIndex; i++) {
        if ((AmbaImgMainEis_ImageChanId[i].Ctx.Bits.VinId == VinId) &&
            (AmbaImgMainEis_ImageChanId[i].Ctx.Bits.ChainId == ChainId)) {
            if (AmbaImgMainEis_ImageChanId[i].Ctx.Bits.ZoneId > 0U) {
                /* sensor id get */
                SensorChanId.VinID = VinId;
                for (j = 0U; (j < AMBA_IMG_NUM_VIN_ALGO) && ((((UINT32) ImageChanId.Ctx.Bits.SensorId) >> j) > 0U); j++) {
                    if ((((UINT32) ImageChanId.Ctx.Bits.SensorId) & (((UINT32) 1U) << j)) > 0U) {
                        SensorChanId.SensorID = AmbaImgMain_SensorIdTable[VinId][j];
                        break;
                    }
                }
                /* last shutter time get */
                for (j = 0U; j < AMBA_IMG_NUM_EXPOSURE_CHANNEL; j++) {
                    AmbaImgMainEis_LastShutterTime[i][j] = AmbaImgMainEis_ShutterTime[i][j];
                }
                /* current shutter time get */
                FuncRetCode = AmbaSensor_GetCurrentShutterSpeed(&SensorChanId, AmbaImgMainEis_ShutterTime[i]);
                if (FuncRetCode != SENSOR_ERR_NONE) {
                    RetCode = NG_UL;
                }
#if 1
                /* debug timing enable? */
                if (AmbaImgSensorHAL_TimingMarkChk(VinId) == OK_UL) {
                    UINT32 ShrInUs;
                    FLOAT VarF;

                    char str[11];
                    str[0] = ' ';str[1] = 's';

                    VarF = AmbaImgMainEis_ShutterTime[i][0]*1000000.0f;
                    ShrInUs = (VarF > 0.0f) ? (UINT32) VarF : 0U;
                    var_utoa(ShrInUs, &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                    AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                }
#endif
                /* sof timing for algo */
#ifdef CONFIG_BUILD_IMGFRW_EIS
                {
                    UINT32 CaptureLine = AmbaImgSystem_Info[VinId][ChainId].Vin.Cap.Bits.Height;
                    UINT32 LineInNs = AmbaImgSensorHAL_Info[VinId][ChainId].Frame.TotalNsInLine;

                    AMBA_IMG_IMUMGR_EVENT_DATA_s TimingData = {0};
                    UINT32 Timetick;
                    EIS_MEM_INFO_s MemInfo;

                    /* vin id get */
                    TimingData.VinId = VinId;
                    /* timetick get */
                    FuncRetCode = AmbaKAL_GetSysTickCount(&Timetick);
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* */
                    }
                    /* sof timetick */
                    TimingData.SofSystemTime = Timetick;
                    /* readout time */
                    TimingData.ReadoutTime = (((FLOAT) LineInNs) * ((FLOAT) CaptureLine)) / 1000000000.f;
                    /* sof timing for imu */
                    MemInfo.Ctx.pTimingData = &TimingData;
                    FuncRetCode = AmbaImg_ImuMgr_SofEventHandler(NULL, MemInfo.Ctx.pUint32);
                    if (FuncRetCode != OK) {
                        /* */
                    }
                }
#endif
            }
        }
    }

    return RetCode;
}

/**
 *  Amba image main eis eof
 *  @param[in] ImageChanId image channel id
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgMainEis_Eof(AMBA_IMG_CHANNEL_ID_s ImageChanId)
{
    UINT32 RetCode = OK_UL;
    UINT32 i;

    UINT32 VinId;
    UINT32 ChainId;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    for (i = 0U; i <= AmbaImgMainEis_LastIndex; i++) {
        if ((AmbaImgMainEis_ImageChanId[i].Ctx.Bits.VinId == VinId) &&
            (AmbaImgMainEis_ImageChanId[i].Ctx.Bits.ChainId == ChainId)) {
            if (AmbaImgMainEis_ImageChanId[i].Ctx.Bits.ZoneId > 0U) {
                /* eof timing for algo */
#ifdef CONFIG_BUILD_IMGFRW_EIS
                {
                    UINT32 FuncRetCode;
                    AMBA_IMG_IMUMGR_EVENT_DATA_s TimingData = {0};
                    UINT32 Timetick;
                    EIS_MEM_INFO_s MemInfo;

                    /* vin id get */
                    TimingData.VinId = VinId;
                    /* timetick get */
                    FuncRetCode = AmbaKAL_GetSysTickCount(&Timetick);
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* */
                    }
                    /* eof timetick */
                    TimingData.EofSystemTime = Timetick;
                    /* eof timing for imu */
                    MemInfo.Ctx.pTimingData = &TimingData;
                    FuncRetCode = AmbaImg_ImuMgr_EofEventHandler(NULL, MemInfo.Ctx.pUint32);
                    if (FuncRetCode != OK) {
                        /* */
                    }
                }
#endif
            }
        }
    }

    return RetCode;
}

/**
 *  Amba image main eis post aik trigger
 *  @param[in] PostAikId post aik id
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgMainEis_PostAikTrigger(UINT64 PostAikId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i, j;

    UINT32 VinId;
    UINT32 ChainId;
    UINT32 PostZoneId;

    EIS_MEM_INFO_s MemInfo;
    EIS_MEM_INFO_s MemInfo2;

    MemInfo.Ctx.Data = (AMBA_IMG_MAIN_MEM_ADDR) PostAikId;

    VinId = MemInfo.Ctx.pPostAikInfo->ImageChanId.Ctx.Bits.VinId;
    ChainId = MemInfo.Ctx.pPostAikInfo->ImageChanId.Ctx.Bits.ChainId;
    PostZoneId = MemInfo.Ctx.pPostAikInfo->ImageChanId.Ctx.Bits.ZoneId;

    for (i = 0U; i <= AmbaImgMainEis_LastIndex; i++) {
        if ((AmbaImgMainEis_ImageChanId[i].Ctx.Bits.VinId == VinId) &&
            (AmbaImgMainEis_ImageChanId[i].Ctx.Bits.ChainId == ChainId)) {
            if ((AmbaImgMainEis_ImageChanId[i].Ctx.Bits.ZoneId & PostZoneId) > 0U) {
                /* eor trigger of eis for algo */
                MemInfo2.Ctx.Data = (AMBA_IMG_MAIN_MEM_ADDR) MemInfo.Ctx.pPostAikInfo->R2yUser;
                /* flag clear */
                MemInfo2.Ctx.pUserData->UpdateFlag = 0U;
                /* shutter time get */
                for (j = 0U; j < AMBA_IMG_NUM_EXPOSURE_CHANNEL; j++) {
                    MemInfo2.Ctx.pUserData->ShutterTime[j] = AmbaImgMainEis_LastShutterTime[i][j];
                }
                /* algo task trigger */
                FuncRetCode = AmbaKAL_MsgQueueSend(&(AmbaImgMainEis_AlgoQueue[i]), &PostAikId, AMBA_KAL_WAIT_FOREVER);
                if (FuncRetCode == KAL_ERR_NONE) {
                    AmbaImgSensorHAL_TimingMarkPut(VinId, "PAik_Q");
                } else {
                    AmbaImgSensorHAL_TimingMarkPut(VinId, "PAik_Q!");
                    RetCode = NG_UL;
                }
#if 1
                /* debug timing enable? */
                if (AmbaImgSensorHAL_TimingMarkChk(VinId) == OK_UL) {
                    char str[11];
                    str[0] = ' ';str[1] = 'p';
                    var_utoa((UINT32) (PostAikId & 0xFFFFFFFFULL), &(str[2]), 16U, 8U, (UINT32) VAR_LEADING_SPACE);
                    AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                    str[1] = 'i';
                    var_utoa(MemInfo.Ctx.pPostAikInfo->Id, &(str[2]), 16U, 8U, (UINT32) VAR_LEADING_SPACE);
                    AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                    str[1] = 'f';
                    var_utoa(MemInfo.Ctx.pPostAikInfo->ImageChanId.Ctx.Bits.ZoneId, &(str[2]), 16U, 8U, (UINT32) VAR_LEADING_SPACE);
                    AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                    str[1] = 'r';
                    var_utoa(MemInfo.Ctx.pPostAikInfo->RawCapSeq, &(str[2]), 16U, 8U, (UINT32) VAR_LEADING_SPACE);
                    AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                    str[1] = 'p';
                    var_utoa(MemInfo.Ctx.pPostAikInfo->PostAikCnt, &(str[2]), 16U, 8U, (UINT32) VAR_LEADING_SPACE);
                    AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                }
#endif
                break;
            }
        }
    }

    return RetCode;
}
#ifdef CONFIG_BUILD_IMGFRW_EIS
/**
 *  Amba image main eis algorithm init
 *  @param[in] Param task parameter
 *  @param[in] ImageChanId image channel id
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgMainEis_ImgEisInit(UINT32 Param, AMBA_IMG_CHANNEL_ID_s ImageChanId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 VinId;
    UINT32 ChainId;
    UINT32 PostZoneId;

    AMBA_SENSOR_CHANNEL_s SensorChanId;
    AMBA_SENSOR_DEVICE_INFO_s SensorDevInfo;
    AMBA_SENSOR_STATUS_INFO_s SensorStatus;

    AMBA_IK_MODE_CFG_s *pImgMode;
    AMBA_IK_MODE_CFG_s ImgMode;
    AMBA_IK_WINDOW_SIZE_INFO_s VinWinInfo;
    AMBA_IK_VIN_ACTIVE_WINDOW_s VinActWin;

    AMBA_IMU_INFO_s ImuInfo;
    AMBA_EISCTRL_SENSOR_s EisCtrlSensor;
    AMBA_EISCTRL_WINPARM_s EisCtrlWin = {0};

    UINT32 EisIdx = Param & 0xFFFFU;
    UINT32 EisCfgIdx = 1U;
    //UINT32 EisAlgoId = Param >> 16U;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;
    PostZoneId = ImageChanId.Ctx.Bits.ZoneId;

    /* eis ctrl sensor reset */
    FuncRetCode = AmbaWrap_memset(&EisCtrlSensor, 0, sizeof(AMBA_EISCTRL_SENSOR_s));
    if (FuncRetCode != OK_UL) {
        /* */
    }

    /* imu info */
    FuncRetCode = AmbaIMU_GetDeviceInfo((UINT8) AmbaImgMainEis_ImuIdTable[EisIdx], &ImuInfo);
    if (FuncRetCode != OK_UL) {
        AmbaImgPrint2(PRINT_FLAG_ERR, "error: eis imu dev", VinId, ChainId);
        RetCode = NG_UL;
    }

    /* sensor info */
    if (RetCode == OK_UL) {
        /* vin id get */
        SensorChanId.VinID = VinId;
        /* sensor id reset */
        SensorChanId.SensorID = 0U;
        /* sensor id get */
        for (i = 0U; (i < AMBA_IMG_NUM_VIN_ALGO) && ((((UINT32) ImageChanId.Ctx.Bits.SensorId) >> i) > 0U); i++) {
            if ((((UINT32) ImageChanId.Ctx.Bits.SensorId) & (((UINT32) 1U) << i)) > 0U) {
                SensorChanId.SensorID = AmbaImgMain_SensorIdTable[VinId][i];
                break;
            }
        }
        /* sensor dev info get */
        FuncRetCode = AmbaSensor_GetDeviceInfo(&SensorChanId, &SensorDevInfo);
        if (FuncRetCode == SENSOR_ERR_NONE) {
            /* sensor dev info put */
            FuncRetCode = AmbaWrap_memcpy(&(EisCtrlSensor.SensorDeviceInfo), &SensorDevInfo, sizeof(AMBA_SENSOR_DEVICE_INFO_s));
            if (FuncRetCode != OK_UL) {
                /* copy fail */
                RetCode = NG_UL;
            }
        } else {
            /* info fail */
            AmbaImgPrint2(PRINT_FLAG_ERR, "error: eis sensor dev", VinId, ChainId);
            RetCode = NG_UL;
        }
        /* sensor status get */
        FuncRetCode = AmbaSensor_GetStatus(&SensorChanId, &SensorStatus);
        if (FuncRetCode == SENSOR_ERR_NONE) {
            /* sensor status put */
            FuncRetCode = AmbaWrap_memcpy(&(EisCtrlSensor.SensorStatus), &SensorStatus, sizeof(AMBA_SENSOR_STATUS_INFO_s));
            if (FuncRetCode != OK_UL) {
                /* copy fail */
                RetCode = NG_UL;
            }
        } else {
            /* status fail */
            AmbaImgPrint2(PRINT_FLAG_ERR, "error: eis sensor status", VinId, ChainId);
            RetCode = NG_UL;
        }
    }

    /* win info */
    if (RetCode == OK_UL) {
        /* post zone index get */
        for (i = 0U; (i < AMBA_IMG_SYSTEM_FOV_NUM) && ((PostZoneId >> i) > 0U); i++) {
            if ((PostZoneId & (((UINT32) 1U) << i)) > 0U) {
                break;
            }
        }
        /* image mode get */
        FuncRetCode = AmbaImgSystem_ImageModeGet(ImageChanId, AMBA_IK_PIPE_VIDEO, &pImgMode);
        if (FuncRetCode == OK_UL) {
            FuncRetCode = AmbaWrap_memcpy(&ImgMode, pImgMode, sizeof(AMBA_IK_MODE_CFG_s));
            if (FuncRetCode == OK_UL) {
                /* ctx id get */
                ImgMode.ContextId = AmbaImgSystem_ContextIdGet(i);
                /* vin win size get */
                FuncRetCode = AmbaIK_GetWindowSizeInfo(&ImgMode, &VinWinInfo);
                if (FuncRetCode == IK_OK) {
                    /* vin win size put */
                    FuncRetCode = AmbaWrap_memcpy(&(EisCtrlWin.WindowSizeInfo), &VinWinInfo, sizeof(AMBA_IK_WINDOW_SIZE_INFO_s));
                    if (FuncRetCode != OK_UL) {
                        /* copy fail */
                        RetCode = NG_UL;
                    }
                } else {
                    /* vin win size fail */
                    AmbaImgPrint2(PRINT_FLAG_ERR, "error: eis vin win", VinId, ChainId);
                    RetCode = NG_UL;
                }

                /* act win get */
                FuncRetCode = AmbaIK_GetVinActiveWin(&ImgMode, &VinActWin);
                if (FuncRetCode == IK_OK) {
                    /* act win put */
                    FuncRetCode = AmbaWrap_memcpy(&(EisCtrlWin.VinActiveWin), &VinActWin, sizeof(AMBA_IK_VIN_ACTIVE_WINDOW_s));
                    if (FuncRetCode != OK_UL) {
                        /* copy fail */
                        RetCode = NG_UL;
                    }
                } else {
                    /* act win fail */
                    AmbaImgPrint2(PRINT_FLAG_ERR, "error: eis act win", VinId, ChainId);
                    RetCode = NG_UL;
                }
            } else {
                /* img mode copy fail */
                RetCode = NG_UL;
            }
        } else {
            /* img mode fail */
            RetCode = NG_UL;
        }
    }

    /* eis algo init */
    if (RetCode == OK_UL) {
        FuncRetCode = AmbaEisCtrl_Init(&EisCtrlWin, &EisCtrlSensor, &ImuInfo);
        if (FuncRetCode != OK_UL) {
            /* eis algo init */
            AmbaImgPrint2(PRINT_FLAG_ERR, "error: eis init", VinId, ChainId);
            RetCode = NG_UL;
        }

        /* eis cfg (AmbaEIS_DeCfg.c) */
        FuncRetCode = AmbaEisCtrl_SetDefCfg(&(gEisCfgList[EisCfgIdx]));
        if (FuncRetCode != OK_UL) {
            /* eis cfg fail */
            AmbaImgPrint2(PRINT_FLAG_ERR, "error: eis ctrl cfg", VinId, ChainId);
            RetCode = NG_UL;
        }

        if (FuncRetCode == OK_UL) {
            /* eis algo enable */
            FuncRetCode = AmbaEisCtrl_Enable(1U, 1U);
            if (FuncRetCode != OK_UL) {
                /* eis enable fail */
                AmbaImgPrint2(PRINT_FLAG_ERR, "error: eis ctrl en", VinId, ChainId);
                RetCode = NG_UL;
            }
        } else {
            /* eis init fail */
            AmbaImgPrint2(PRINT_FLAG_ERR, "error: eis ctrl init", VinId, ChainId);
            RetCode = NG_UL;
        }
    }

    return RetCode;
}
#else
/**
 *  Amba image main eis dummy eis init
 *  @param[in] Param task parameter
 *  @param[in] ImageChanId image channel id
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgMainEis_DummyEisInit(UINT32 Param, AMBA_IMG_CHANNEL_ID_s ImageChanId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    AMBA_IK_WINDOW_SIZE_INFO_s VinWinInfo;

    UINT32 VinId;
    UINT32 ChainId;
    UINT32 PostZoneId;

    UINT32 EisIdx = Param & 0xFFFFU;
    //UINT32 EisAlgoId = Param >> 16U;

    AMBA_IK_MODE_CFG_s *pImgMode;
    AMBA_IK_MODE_CFG_s ImgMode;

    UINT32 Width;
    UINT32 Height;

    UINT32 MainWidth;
    UINT32 MainHeight;

    //UINT32 Factor = DUMMY_EIS_ZOOM_FACTOR;

    UINT32 DZoomWidth;
    UINT32 DZoomHeight;

    UINT32 DZoomShiftWidth;
    UINT32 DZoomShiftHeight;
     INT32 DZoomShiftX;
     INT32 DZoomShiftY;

    //UINT32 DummyFactor = DUMMY_EIS_DUMMY_FACTOR;

    UINT32 DummyWidth;
    UINT32 DummyHeight;

     INT32 DummyOffsetX;
     INT32 DummyOffsetY;

     INT32 VarL;

    AMBA_IK_VIN_ACTIVE_WINDOW_s ActWin;
    AMBA_IK_DZOOM_INFO_s DZoomInfo;
    AMBA_IK_DUMMY_MARGIN_RANGE_s DummyRange;

    AMBA_IMG_CHANNEL_ID_s ImageChanId2;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;
    PostZoneId = ImageChanId.Ctx.Bits.ZoneId;

    /* param update */
    FuncRetCode = AmbaImgMainEis_ParamUpdate(EisIdx);
    if (FuncRetCode != OK_UL) {
        /* */
    }

    /* image channel id get */
    ImageChanId2.Ctx.Data = ImageChanId.Ctx.Data;
    /* fov check  */
    for (i = 0U; (i < AMBA_IMG_SYSTEM_FOV_NUM) && ((PostZoneId >> i) > 0U); i++) {
        if ((PostZoneId & (((UINT32) 1U) << i)) > 0U) {
            /* fov select */
            ImageChanId2.Ctx.Bits.ZoneId = (((UINT32) 1U) << i);
            /* image mode get */
            FuncRetCode = AmbaImgSystem_ImageModeGet(ImageChanId2, AMBA_IK_PIPE_VIDEO, &pImgMode);
            if (FuncRetCode == OK_UL) {
                FuncRetCode = AmbaWrap_memcpy(&ImgMode, pImgMode, sizeof(AMBA_IK_MODE_CFG_s));
                if (FuncRetCode == OK_UL) {
                    /* context id set */
                    ImgMode.ContextId = AmbaImgSystem_ContextIdGet(i);
                    /* vin win size get */
                    FuncRetCode = AmbaIK_GetWindowSizeInfo(&ImgMode, &VinWinInfo);
                    if (FuncRetCode == IK_OK) {
                        /* vin win size */
                        Width = VinWinInfo.VinSensor.Width;
                        Height = VinWinInfo.VinSensor.Height;

                        /* act win */
                        ActWin.Enable = 1U;
                        ActWin.ActiveGeo.StartX = 0U;
                        ActWin.ActiveGeo.StartY = 0U;
                        ActWin.ActiveGeo.Width = Width;
                        ActWin.ActiveGeo.Height = Height;

                        AmbaImgPrint2(PRINT_FLAG_DBG, "act win", EisIdx, ActWin.Enable);
                        AmbaImgPrint(PRINT_FLAG_DBG, " start x", ActWin.ActiveGeo.StartX);
                        AmbaImgPrint(PRINT_FLAG_DBG, " start y", ActWin.ActiveGeo.StartY);
                        AmbaImgPrint(PRINT_FLAG_DBG, " width", ActWin.ActiveGeo.Width);
                        AmbaImgPrint(PRINT_FLAG_DBG, " height", ActWin.ActiveGeo.Height);

                        /* main win */
                        MainWidth = VinWinInfo.Main.Width;
                        MainHeight = VinWinInfo.Main.Height;

                        /* dzoom size */
                        DZoomWidth = (Width * 1000U) / AmbaImgMainEis_Param[EisIdx][0].DZoom.Factor;
                        DZoomHeight = (DZoomWidth * MainHeight) / MainWidth;

                        /* dzoom Y factor calculated */
                        AmbaImgMainEis_Param[EisIdx][0].DZoom.YFactor = ((Height * 1000U) + (DZoomHeight/2U)) / DZoomHeight;

                        /* dummy range Y factor protect */
                        if (AmbaImgMainEis_Param[EisIdx][0].DummyRange.YFactor > AmbaImgMainEis_Param[EisIdx][0].DZoom.YFactor) {
                            AmbaImgMainEis_Param[EisIdx][0].DummyRange.YFactor = AmbaImgMainEis_Param[EisIdx][0].DZoom.YFactor;
                        }
                        /* warp shift Y factor protect */
                        if (AmbaImgMainEis_Param[EisIdx][0].WarpShift.YFactor > AmbaImgMainEis_Param[EisIdx][0].DZoom.YFactor) {
                            AmbaImgMainEis_Param[EisIdx][0].WarpShift.YFactor = AmbaImgMainEis_Param[EisIdx][0].DZoom.YFactor;
                        }

                        AmbaImgPrint(PRINT_FLAG_DBG, "dzoom win", EisIdx);
                        AmbaImgPrint(PRINT_FLAG_DBG, " x factor", AmbaImgMainEis_Param[EisIdx][0].DZoom.Factor);
                        AmbaImgPrint(PRINT_FLAG_DBG, " y factor", AmbaImgMainEis_Param[EisIdx][0].DZoom.YFactor);
                        AmbaImgPrint(PRINT_FLAG_DBG, " width", DZoomWidth);
                        AmbaImgPrint(PRINT_FLAG_DBG, " height", DZoomHeight);

                        /* shift size */
                        DZoomShiftWidth = (DZoomWidth * AmbaImgMainEis_Param[EisIdx][0].DZoom.ShiftXFactor) / 1000U;
                        DZoomShiftHeight = (DZoomHeight * AmbaImgMainEis_Param[EisIdx][0].DZoom.ShiftYFactor) / 1000U;

                        /* dzoom shift */
                        DZoomShiftX = ((((INT32) DZoomShiftWidth) - ((INT32) DZoomWidth)) / 2) * AmbaImgMainEis_Param[EisIdx][0].DZoom.ShiftXDir;
                        DZoomShiftY = ((((INT32) DZoomShiftHeight) - ((INT32) DZoomHeight)) / 2) * AmbaImgMainEis_Param[EisIdx][0].DZoom.ShiftYDir;

                        /* dzoom win */
                        DZoomInfo.Enable = 1U;
                        DZoomInfo.ShiftX =  DZoomShiftX * 65536; /* 16.16 format */
                        DZoomInfo.ShiftY =  DZoomShiftY * 65536;
                        DZoomInfo.ZoomX = (Width * 65536U) / DZoomWidth; /* 16.16 format */
                        DZoomInfo.ZoomY = (Height * 65536U) / DZoomHeight;

                        AmbaImgPrint2(PRINT_FLAG_DBG, "dzoom info", EisIdx, DZoomInfo.Enable);
                        VarL = DZoomInfo.ShiftX / 65536;
                        AmbaImgPrintEx2(PRINT_FLAG_DBG, " shift x", (UINT32) VarL, 16U, (UINT32) DZoomInfo.ShiftX, 16U);
                        VarL = DZoomInfo.ShiftY / 65536;
                        AmbaImgPrintEx2(PRINT_FLAG_DBG, " shift y", (UINT32) VarL, 16U, (UINT32) DZoomInfo.ShiftY, 16U);
                        AmbaImgPrint2(PRINT_FLAG_DBG, " zoom x", ((DZoomInfo.ZoomX * 1000U) + 32768U) / 65536U, DZoomInfo.ZoomX);
                        AmbaImgPrint2(PRINT_FLAG_DBG, " zoom y", ((DZoomInfo.ZoomY * 1000U) + 32768U) / 65536U, DZoomInfo.ZoomY);

                        /* dummy win size */
                        DummyWidth = (DZoomWidth * AmbaImgMainEis_Param[EisIdx][0].DummyRange.XFactor) / 1000U;
                        DummyHeight = (DZoomHeight * AmbaImgMainEis_Param[EisIdx][0].DummyRange.YFactor) / 1000U;

                        AmbaImgPrint(PRINT_FLAG_DBG, "dummy win", EisIdx);
                        AmbaImgPrint(PRINT_FLAG_DBG, " width", DummyWidth);
                        AmbaImgPrint(PRINT_FLAG_DBG, " height", DummyHeight);

                        /* dzoom win offset from dummy */
                        DummyOffsetX = (((INT32) DummyWidth) - ((INT32) DZoomWidth)) / 2;
                        DummyOffsetY = (((INT32) DummyHeight) - ((INT32) DZoomHeight)) / 2;

                        /* dummy range (percent, .16 format) */
                        DummyRange.Enable = 1U;
                        VarL = (DummyOffsetX * 65536) / ((INT32) Width);
                        DummyRange.Left = (VarL > 0) ? (UINT32) VarL : 0U; /* .16 format */
                        DummyRange.Right = (VarL > 0) ? (UINT32) VarL : 0U;
                        VarL = (DummyOffsetY * 65536) / ((INT32) Height);
                        DummyRange.Top = (VarL > 0) ? (UINT32) VarL : 0U;
                        DummyRange.Bottom = (VarL > 0) ? (UINT32) VarL : 0U;

                        AmbaImgPrint2(PRINT_FLAG_DBG, "dummy range", EisIdx, DummyRange.Enable);
                        AmbaImgPrint2(PRINT_FLAG_DBG, " left", (DummyOffsetX > 0) ? (UINT32) DummyOffsetX : 0U, DummyRange.Left);
                        AmbaImgPrint2(PRINT_FLAG_DBG, " right", (DummyOffsetX > 0) ? (UINT32) DummyOffsetX : 0U, DummyRange.Right);
                        AmbaImgPrint2(PRINT_FLAG_DBG, " top", (DummyOffsetY > 0) ? (UINT32) DummyOffsetY : 0U, DummyRange.Top);
                        AmbaImgPrint2(PRINT_FLAG_DBG, " bottom", (DummyOffsetY > 0) ? (UINT32) DummyOffsetY : 0U, DummyRange.Bottom);

                        /* act win */
                        FuncRetCode = AmbaIK_SetVinActiveWin(&ImgMode, &ActWin);
                        if (FuncRetCode != IK_OK) {
                            AmbaImgPrint2(PRINT_FLAG_ERR, "error: dummy eis act win", VinId, ChainId);
                        }
                        /* dzoom */
                        FuncRetCode = AmbaIK_SetDzoomInfo(&ImgMode, &DZoomInfo);
                        if (FuncRetCode != IK_OK) {
                            AmbaImgPrint2(PRINT_FLAG_ERR, "error: dummy eis dzoom", VinId, ChainId);
                        }
                        /* dummy range */
                        FuncRetCode = AmbaIK_SetDummyMarginRange(&ImgMode, &DummyRange);
                        if (FuncRetCode != IK_OK) {
                            AmbaImgPrint2(PRINT_FLAG_ERR, "error: dummy eis dummy range", VinId, ChainId);
                        }
                    } else {
                        /* win size fail */
                        RetCode = NG_UL;
                    }
                } else {
                    /* copy fail */
                    RetCode = NG_UL;
                }
            } else {
                /* img mode fail */
                RetCode = NG_UL;
            }
        }
    }

    return RetCode;
}
#endif
/**
 *  Amba image main eis task ex
 *  @param[in] pParam pointer to parameter
 *  @note this function is intended for internal use only
 */
static void *AmbaImgMainEis_TaskEx(void *pParam)
{
    EIS_MEM_INFO_s MemInfo;

    MemInfo.Ctx.pVoid = pParam;
    AmbaImgMainEis_Task((UINT32) MemInfo.Ctx.Data);

    return NULL;
}

/**
 *  Amba image main eis task
 *  @param[in] Param eis index with the eis algo id
 *  @note this function is intended for internal use only
 */
static void AmbaImgMainEis_Task(UINT32 Param)
{
#define AMBA_IMG_MAIN_EIS_USER_DATA_NUM  8U
    static AMBA_IMG_MAIN_EIS_USER_DATA_s GNU_SECTION_NOZEROINIT AmbaImgMainEis_R2yUserData[AMBA_IMG_SYSTEM_FOV_NUM][AMBA_IMG_MAIN_EIS_USER_DATA_NUM];

    UINT8 FirstActive = 1U;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 VinId = 0U;
    UINT32 ChainId = 0U;
    UINT32 PostZoneId = 0U;

    AMBA_IMG_CHANNEL_ID_s ImageChanId;
    AMBA_IMG_CHANNEL_ID_s ImageChanId2;

    UINT32 EisIdx = Param & 0xFFFFU;
    //UINT32 EisAlgoId = Param >> 16U;

    UINT32 R2yUserDelay = AmbaImgMainEis_R2yUserDelay[EisIdx];

    AMBA_IMG_SYSTEM_PAIK_INFO_s *pPostAikInfo = NULL;

    EIS_MEM_INFO_s MemInfo;

    /* param init */
    ImageChanId.Ctx.Data = 0ULL;

    AmbaImgMainEis_Cmd[EisIdx] = (UINT32) IMG_EIS_TASK_CMD_STOP;

    while (AmbaImgMainEis_DummyFlag[EisIdx] == 1U) {
        if (AmbaImgMainEis_Cmd[EisIdx] == (UINT32) IMG_EIS_TASK_CMD_STOP) {
            if (FirstActive == 1U) {
                /* task first time create */
                FirstActive = 0U;
            } else {
#ifdef CONFIG_BUILD_IMGFRW_EIS
                /* imu port close */
                FuncRetCode = AmbaImg_ImuMgr_CloseFiFoPort(&(AmbaImgMainEis_ImgImuPort[EisIdx]));
                if (FuncRetCode != OK_UL) {
                    /* imu close fail */
                }
                FuncRetCode = AmbaEisCtrl_Enable(0U, 0U);
                if (FuncRetCode != OK_UL) {
                    /* eis disable fail */
                    AmbaImgPrint2(PRINT_FLAG_ERR, "error: eis ctrl dis", ImageChanId.Ctx.Bits.VinId, ImageChanId.Ctx.Bits.ChainId);
                }
#endif
                /* post aik cb (unhook) */
                pAmbaImgMainChannel[VinId][ChainId].PostAikCb = NULL;
                /* sof/eof cb (unhook) */
                pAmbaImgMainChannel[VinId][ChainId].SofCb = NULL;
                pAmbaImgMainChannel[VinId][ChainId].EofCb = NULL;
            }

            /* ready give */
            FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaImgMainEis_Ready[EisIdx]);
            if (FuncRetCode != KAL_ERR_NONE) {
                AmbaImgPrint2(PRINT_FLAG_ERR, "error: eis task idle", EisIdx, FuncRetCode);
            }

            /* go take */
            FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaImgMainEis_Go[EisIdx], AMBA_KAL_WAIT_FOREVER);
            if (FuncRetCode != KAL_ERR_NONE) {
                AmbaImgPrint2(PRINT_FLAG_ERR, "error: eis task go", EisIdx, FuncRetCode);
            }
            /* image channel get */
            ImageChanId.Ctx.Data = AmbaImgMainEis_ImageChanId[EisIdx].Ctx.Data;

            /* vin & chain id */
            VinId = ImageChanId.Ctx.Bits.VinId;
            ChainId = ImageChanId.Ctx.Bits.ChainId;
            PostZoneId = ImageChanId.Ctx.Bits.ZoneId;

            /* debug msg */
            AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
            AmbaImgPrint(PRINT_FLAG_DBG, "eis idx", EisIdx);
            AmbaImgPrint(PRINT_FLAG_DBG, "vin id", VinId);
            AmbaImgPrint(PRINT_FLAG_DBG, "chain id", ChainId);
            AmbaImgPrintEx(PRINT_FLAG_DBG, "post zone id", PostZoneId, 16U);

            /* sof/eof cb (hook) */
            pAmbaImgMainChannel[VinId][ChainId].SofCb = AmbaImgMainEis_Sof;
            pAmbaImgMainChannel[VinId][ChainId].EofCb = AmbaImgMainEis_Eof;

            /* post aik cb (hook) */
            pAmbaImgMainChannel[VinId][ChainId].PostAikCb = AmbaImgMainEis_PostAikTrigger;

            /* eis init */
            AmbaImgPrint(PRINT_FLAG_MSG, "eis init...", EisIdx);
#ifdef CONFIG_BUILD_IMGFRW_EIS
            /* eis algo init */
            FuncRetCode = AmbaImgMainEis_ImgEisInit(Param, ImageChanId);
            if (FuncRetCode == OK_UL) {
                /* imu port open */
                FuncRetCode = AmbaImg_ImuMgr_OpenFiFoPort(AmbaImgMainEis_ImuIdTable[EisIdx], &(AmbaImgMainEis_ImgImuPort[EisIdx]));
                if (FuncRetCode != OK) {
                    AmbaImgPrint(PRINT_FLAG_ERR, "error: img imu port", EisIdx);
                }
            } else {
                AmbaImgPrint(PRINT_FLAG_ERR, "error: img eis init", EisIdx);
            }
#else
            /* dummy eis init */
            FuncRetCode = AmbaImgMainEis_DummyEisInit(Param, ImageChanId);
            if (FuncRetCode != OK_UL) {
                AmbaImgPrint(PRINT_FLAG_ERR, "error: img dummy eis init", EisIdx);
            }
#endif
            /* r2y user buffer */
            ImageChanId2.Ctx.Data = ImageChanId.Ctx.Data;
            for (i = 0U; (i < AMBA_IMG_SYSTEM_FOV_NUM) && ((PostZoneId >> i) > 0U); i++) {
                if ((PostZoneId & (((UINT32) 1U) << i)) > 0U) {
                    ImageChanId2.Ctx.Bits.ZoneId = (((UINT32) 1U) << i);
                    MemInfo.Ctx.pUserData = AmbaImgMainEis_R2yUserData[i];
                    FuncRetCode = AmbaImgSystem_R2yUserBuffer(ImageChanId2, (UINT32) sizeof(AMBA_IMG_MAIN_EIS_USER_DATA_s), AMBA_IMG_MAIN_EIS_USER_DATA_NUM, MemInfo.Ctx.pUint8);
                    if (FuncRetCode != OK_UL) {
                        AmbaImgPrint2(PRINT_FLAG_ERR, "error: eis r2y user", EisIdx, i);
                    }
                }
            }

            /* r2y user delay */
            FuncRetCode = AmbaImgSystem_R2yUserDelay(ImageChanId, R2yUserDelay, (R2yUserDelay+5U));
            if (FuncRetCode != OK_UL) {
                AmbaImgPrint2(PRINT_FLAG_ERR, "error: eis r2y delay", EisIdx, FuncRetCode);
            }
            AmbaImgPrint2(PRINT_FLAG_MSG, "r2y user delay...", EisIdx, R2yUserDelay);
            {
                UINT32 Timetick = 0xCafeU;
                FuncRetCode = AmbaKAL_GetSysTickCount(&Timetick);
                if (FuncRetCode != KAL_ERR_NONE) {
                    /* */
                }
                FuncRetCode = AmbaWrap_srand(Timetick);
                if (FuncRetCode != OK_UL) {
                    /* */
                }
            }
            /* ack */
            FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaImgMainEis_Ack[EisIdx]);
            if (FuncRetCode != KAL_ERR_NONE) {
                AmbaImgPrint2(PRINT_FLAG_ERR, "error: eis task ack", EisIdx, FuncRetCode);
            }
        }

        /* fov check  */
        ImageChanId2.Ctx.Data = ImageChanId.Ctx.Data;
        for (i = 0U; (i < AMBA_IMG_SYSTEM_FOV_NUM) && ((PostZoneId >> i) > 0U); i++) {
            if ((PostZoneId & (((UINT32) 1U) << i)) > 0U) {
                /* fov select */
                ImageChanId2.Ctx.Bits.ZoneId = (((UINT32) 1U) << i);
                /* post aik wait */
                FuncRetCode = AmbaImgSystem_PostAikWait(ImageChanId2, &pPostAikInfo);
                if ((FuncRetCode == OK_UL) &&
                    (pPostAikInfo != NULL)) {
                    /* debug message */
                    AmbaImgSensorHAL_TimingMarkPut(VinId, "PAik_Wait");
#if 1
                    /* debug timing enable? */
                    if (AmbaImgSensorHAL_TimingMarkChk(VinId) == OK_UL) {
                        UINT32 Timetick;

                        char str[11];
                        str[0] = ' ';str[1] = 'p';

                        MemInfo.Ctx.pPostAikInfo = pPostAikInfo;
                        var_utoa((UINT32) MemInfo.Ctx.Data, &(str[2]), 16U, 8U, (UINT32) VAR_LEADING_SPACE);
                        AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                        str[1] = 'i';
                        var_utoa(pPostAikInfo->Id, &(str[2]), 16U, 8U, (UINT32) VAR_LEADING_SPACE);
                        AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                        str[1] = 'f';
                        var_utoa(pPostAikInfo->ImageChanId.Ctx.Bits.ZoneId, &(str[2]), 16U, 8U, (UINT32) VAR_LEADING_SPACE);
                        AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                        str[1] = ' ';
                        FuncRetCode = AmbaKAL_GetSysTickCount(&Timetick);
                        if (FuncRetCode != KAL_ERR_NONE) {
                            /* */
                        }
                        var_utoa(Timetick, &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                        AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                        var_utoa(pPostAikInfo->EorTimetick, &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                        AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                        var_utoa(pPostAikInfo->R2yUserDelay, &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                        AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                    }
#endif
                    /* user delay wait */
                    FuncRetCode = AmbaImgSystem_R2yUserWait(pPostAikInfo);
                    if (FuncRetCode == OK_UL) {
                        /*
                         * eis result: warp info in r2y user, (pPostAikInfo->R2yUser)
                         */
                        MemInfo.Ctx.Data = (AMBA_IMG_MAIN_MEM_ADDR) pPostAikInfo->R2yUser;
                        if (MemInfo.Ctx.pUserData->UpdateFlag > 0U) {
                            /* warp update */
                            AMBA_IK_MODE_CFG_s *pImgMode;
                            AMBA_IK_MODE_CFG_s ImgMode;
                            /* image mode get */
                            FuncRetCode = AmbaImgSystem_ImageModeGet(ImageChanId2, AMBA_IK_PIPE_VIDEO, &pImgMode);
                            if (FuncRetCode == OK_UL) {
                                FuncRetCode = AmbaWrap_memcpy(&ImgMode, pImgMode, sizeof(AMBA_IK_MODE_CFG_s));
                                if (FuncRetCode == OK_UL) {
                                    ImgMode.ContextId = AmbaImgSystem_ContextIdGet(i);
                                    /* act win */
                                    FuncRetCode = AmbaIK_SetVinActiveWin(&ImgMode, &(MemInfo.Ctx.pUserData->ActWin));
                                    if (FuncRetCode == IK_OK) {
                                        AmbaImgSensorHAL_TimingMarkPut(VinId, "PAik_ActW");
                                    } else {
                                        AmbaImgSensorHAL_TimingMarkPut(VinId, "PAik_ActW!");
                                    }
                                    /* dzoom */
                                    FuncRetCode = AmbaIK_SetDzoomInfo(&ImgMode, &(MemInfo.Ctx.pUserData->DZoomInfo));
                                    if (FuncRetCode == IK_OK) {
                                        AmbaImgSensorHAL_TimingMarkPut(VinId, "PAik_Zoom");
                                    } else {
                                        AmbaImgSensorHAL_TimingMarkPut(VinId, "PAik_Zoom!");
                                    }
                                    /* dummy range */
                                    FuncRetCode = AmbaIK_SetDummyMarginRange(&ImgMode, &(MemInfo.Ctx.pUserData->DummyRange));
                                    if (FuncRetCode == IK_OK) {
                                        AmbaImgSensorHAL_TimingMarkPut(VinId, "PAik_DmyR");
                                    } else {
                                        AmbaImgSensorHAL_TimingMarkPut(VinId, "PAik_DmyR!");
                                    }
                                    /* warp settings */
                                    FuncRetCode = AmbaIK_SetWarpEnb(&ImgMode, 0x1U);
                                    if (FuncRetCode == IK_OK) {
                                        FuncRetCode = AmbaIK_SetWarpInfo(&ImgMode, &(MemInfo.Ctx.pUserData->WarpInfo));
                                        if (FuncRetCode == IK_OK) {
                                            AmbaImgSensorHAL_TimingMarkPut(VinId, "PAik_Warp");
                                        } else {
                                            AmbaImgSensorHAL_TimingMarkPut(VinId, "PAik_Warp!");
                                        }
                                    }
#if 1
                                    /* debug message */
                                    if (AmbaImgSensorHAL_TimingMarkChk(VinId) == OK_UL) {
                                        char str[11];
                                        str[0] = ' ';

                                        /* user data */
                                        str[1] = 'c';
                                        var_utoa(ImgMode.ContextId, &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                                        AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                                        str[1] = 'a';
                                        var_utoa(MemInfo.Ctx.pUserData->ActWin.ActiveGeo.StartX, &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                                        AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                                        str[1] = ' ';
                                        var_utoa(MemInfo.Ctx.pUserData->ActWin.ActiveGeo.StartY, &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                                        AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                                        str[1] = ' ';
                                        var_utoa(MemInfo.Ctx.pUserData->ActWin.ActiveGeo.Width, &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                                        AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                                        str[1] = ' ';
                                        var_utoa(MemInfo.Ctx.pUserData->ActWin.ActiveGeo.Height, &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                                        AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                                        str[1] = 'z';
                                        var_itoa((MemInfo.Ctx.pUserData->DZoomInfo.ShiftX / 65536), &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                                        AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                                        str[1] = ' ';
                                        var_itoa((MemInfo.Ctx.pUserData->DZoomInfo.ShiftY / 65536), &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                                        AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                                        str[1] = ' ';
                                        var_utoa((((MemInfo.Ctx.pUserData->DZoomInfo.ZoomX * 100U) + 32768U) / 65536U), &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                                        AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                                        str[1] = ' ';
                                        var_utoa((((MemInfo.Ctx.pUserData->DZoomInfo.ZoomY * 100U) + 32768U) / 65536U), &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                                        AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                                        str[1] = 'd';
                                        var_utoa(MemInfo.Ctx.pUserData->DummyRange.Left, &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                                        AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                                        str[1] = ' ';
                                        var_utoa(MemInfo.Ctx.pUserData->DummyRange.Right, &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                                        AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                                        str[1] = ' ';
                                        var_utoa(MemInfo.Ctx.pUserData->DummyRange.Top, &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                                        AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                                        str[1] = ' ';
                                        var_utoa(MemInfo.Ctx.pUserData->DummyRange.Bottom, &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                                        AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                                        str[1] = 'w';
                                        var_utoa(MemInfo.Ctx.pUserData->WarpInfo.Version, &(str[2]), 16U, 8U, (UINT32) VAR_LEADING_SPACE);
                                        AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                                        str[1] = 'g';
                                        var_utoa(MemInfo.Ctx.pUserData->WarpInfo.HorGridNum, &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                                        AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                                        str[1] = ' ';
                                        var_utoa(MemInfo.Ctx.pUserData->WarpInfo.VerGridNum, &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                                        AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                                        str[1] = 't';
                                        var_utoa(MemInfo.Ctx.pUserData->WarpInfo.TileWidthExp, &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                                        AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                                        str[1] = ' ';
                                        var_utoa(MemInfo.Ctx.pUserData->WarpInfo.TileHeightExp, &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                                        AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                                        str[1] = 'c';
                                        var_utoa(MemInfo.Ctx.pUserData->WarpInfo.Enb_2StageCompensation, &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                                        AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                                        str[1] = 'o';
                                        var_utoa(MemInfo.Ctx.pUserData->WarpInfo.VinSensorGeo.StartX, &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                                        AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                                        str[1] = ' ';
                                        var_utoa(MemInfo.Ctx.pUserData->WarpInfo.VinSensorGeo.StartY, &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                                        AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                                        str[1] = 's';
                                        var_utoa(MemInfo.Ctx.pUserData->WarpInfo.VinSensorGeo.Width, &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                                        AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                                        str[1] = ' ';
                                        var_utoa(MemInfo.Ctx.pUserData->WarpInfo.VinSensorGeo.Height, &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                                        AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                                    }
#endif
                                }
                            }
                        }
                    } else {
                        /* timeout */
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "PAik_TOut!");
                    }
#if 1
                    /* debug message */
                    if (AmbaImgSensorHAL_TimingMarkChk(VinId) == OK_UL) {
                        char str[11];
                        str[0] = ' ';str[1] = 'p';

                        /* post aik info */
                        MemInfo.Ctx.pPostAikInfo = pPostAikInfo;
                        var_utoa((UINT32) MemInfo.Ctx.Data, &(str[2]), 16U, 8U, (UINT32) VAR_LEADING_SPACE);
                        AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                        str[1] = 'i';
                        var_utoa(MemInfo.Ctx.pPostAikInfo->Id, &(str[2]), 16U, 8U, (UINT32) VAR_LEADING_SPACE);
                        AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                        str[1] = 'f';
                        var_utoa(MemInfo.Ctx.pPostAikInfo->ImageChanId.Ctx.Bits.ZoneId, &(str[2]), 16U, 8U, (UINT32) VAR_LEADING_SPACE);
                        AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                    }
#endif
                    /* post aik execute */
                    FuncRetCode = AmbaImgSystem_PostAik(pPostAikInfo);
                    if (FuncRetCode == OK_UL) {
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "PAik_Exec");
                    } else {
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "PAik_Exec!");
                    }
                }

                /* debug only */
                if (AmbaImgMainEis_DebugCount[EisIdx] < AmbaImgMain_DebugCount) {
                    /* count accumulate */
                    AmbaImgMainEis_DebugCount[EisIdx]++;
                    if (AmbaImgMainEis_DebugCount[EisIdx] == AmbaImgMain_DebugCount) {
                        /* count reset */
                        AmbaImgMainEis_DebugCount[EisIdx] = 0xFFFFFFFFU;
                    }
                }
            }
        }
    }
}
#ifdef CONFIG_BUILD_IMGFRW_EIS
/**
 *  Amba image main eis algorithm
 *  @param[in] Param eis index with the eis algorithm id
 *  @param[in] ImageChanId image channel id
 *  @param[out] pActWin pointer to the active window data
 *  @param[out] pDZoomInfo pointer to the digital zoom information
 *  @param[out] pDummyRange pointer to the dummy margin range data
 *  @param[out] pWarpInfo pointer to the warp informatiopn
 *  @param[in] Msg psot aik information address
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgMainEis_ImgEisAlgo(UINT32 Param,
                                        AMBA_IMG_CHANNEL_ID_s ImageChanId,
                                        AMBA_IK_VIN_ACTIVE_WINDOW_s *pActWin,
                                        AMBA_IK_DZOOM_INFO_s *pDZoomInfo,
                                        AMBA_IK_DUMMY_MARGIN_RANGE_s *pDummyRange,
                                        AMBA_IK_WARP_INFO_s *pWarpInfo,
                                        UINT64 Msg)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 VinId;
    UINT32 ChainId;
    UINT32 PostZoneId;

    UINT32 EisIdx = Param & 0xFFFFU;
    //UINT32 EisAlgoId = Param >> 16U;

    AMBA_SENSOR_CHANNEL_s SensorChanId;
    AMBA_SENSOR_DEVICE_INFO_s SensorDevInfo;
    AMBA_SENSOR_STATUS_INFO_s SensorStatus;

    AMBA_IK_MODE_CFG_s *pImgMode;
    AMBA_IK_MODE_CFG_s ImgMode;
    AMBA_IK_WINDOW_SIZE_INFO_s VinWinInfo;
    AMBA_IK_VIN_ACTIVE_WINDOW_s VinActWin;

    AMBA_EISCTRL_IMU_INFO_s EisCtrlImu;
    AMBA_EISCTRL_SENSOR_s EisCtrlSensor;
    AMBA_EISCTRL_WINPARM_s EisCtrlWin = {0};
    AMBA_EISCTRL_SYS_INFO_s EisCtrlSys;
    AMBA_EISCTRL_USER_DATA_s EisCtrlResult;

    UINT32 ImuFiFoDataNum;
    static AMBA_IMG_IMUMGR_FIFO_INFO_s AmbaImgMainEis_ImuFiFoDataInfo[AMBA_IMG_IMUMGR_INFO_NUM];

    UINT32 TimeScale;
    UINT32 NumUnitsInTick;
    UINT32 Interlace;

    FLOAT VarF;

    /* param */
    (void) Param;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;
    PostZoneId = ImageChanId.Ctx.Bits.ZoneId;

    /* eis ctrl sensor/sys reset */
    FuncRetCode = AmbaWrap_memset(&EisCtrlSensor, 0, sizeof(AMBA_EISCTRL_SENSOR_s));
    if (FuncRetCode != OK_UL) {
        /* */
    }
    FuncRetCode = AmbaWrap_memset(&EisCtrlSys, 0, sizeof(AMBA_EISCTRL_SYS_INFO_s));
    if (FuncRetCode != OK_UL) {
        /* */
    }

    /* sensor info */
    {
        /* vin id get */
        SensorChanId.VinID = VinId;
        /* sensor id reset */
        SensorChanId.SensorID = 0U;
        /* sensor id get */
        for (i = 0U; (i < AMBA_IMG_NUM_VIN_ALGO) && ((((UINT32) ImageChanId.Ctx.Bits.SensorId) >> i) > 0U); i++) {
            if ((((UINT32) ImageChanId.Ctx.Bits.SensorId) & (((UINT32) 1U) << i)) > 0U) {
                SensorChanId.SensorID = AmbaImgMain_SensorIdTable[VinId][i];
                break;
            }
        }
        /* sensor dev info get */
        FuncRetCode = AmbaSensor_GetDeviceInfo(&SensorChanId, &SensorDevInfo);
        if (FuncRetCode == SENSOR_ERR_NONE) {
            /* sensor dev info put */
            FuncRetCode = AmbaWrap_memcpy(&(EisCtrlSensor.SensorDeviceInfo), &SensorDevInfo, sizeof(AMBA_SENSOR_DEVICE_INFO_s));
            if (FuncRetCode != OK_UL) {
                /* copy fail */
                RetCode = NG_UL;
            }
        } else {
            /* info fail */
            AmbaImgPrint2(PRINT_FLAG_DBG, "error: eis sensor dev", VinId, ChainId);
            RetCode = NG_UL;
        }
        /* sensor status get */
        FuncRetCode = AmbaSensor_GetStatus(&SensorChanId, &SensorStatus);
        if (FuncRetCode == SENSOR_ERR_NONE) {
            /* sensor status put */
            FuncRetCode = AmbaWrap_memcpy(&(EisCtrlSensor.SensorStatus), &SensorStatus, sizeof(AMBA_SENSOR_STATUS_INFO_s));
            if (FuncRetCode == OK_UL) {
                /* framerate get */
                TimeScale = EisCtrlSensor.SensorStatus.ModeInfo.FrameRate.TimeScale;
                NumUnitsInTick = EisCtrlSensor.SensorStatus.ModeInfo.FrameRate.NumUnitsInTick;
                Interlace = EisCtrlSensor.SensorStatus.ModeInfo.FrameRate.Interlace;
                VarF = (((FLOAT) TimeScale)/(((FLOAT) NumUnitsInTick) * (((FLOAT) Interlace) + 1.f))) + 0.999999f;
                /* framerate put */
                EisCtrlSensor.FrameRatePerSec = (UINT32) VarF;
            } else {
                /* copy fail */
                RetCode = NG_UL;
            }
        } else {
            /* status fail */
            AmbaImgPrint2(PRINT_FLAG_DBG, "error: eis sensor status", VinId, ChainId);
            RetCode = NG_UL;
        }
    }

    /* win info */
    if (RetCode == OK_UL) {
        /* post zone index get */
        for (i = 0U; (i < AMBA_IMG_SYSTEM_FOV_NUM) && ((PostZoneId >> i) > 0U); i++) {
            if ((PostZoneId & (((UINT32) 1U) << i)) > 0U) {
                break;
            }
        }
        /* image mode get */
        FuncRetCode = AmbaImgSystem_ImageModeGet(ImageChanId, AMBA_IK_PIPE_VIDEO, &pImgMode);
        if (FuncRetCode == OK_UL) {
            FuncRetCode = AmbaWrap_memcpy(&ImgMode, pImgMode, sizeof(AMBA_IK_MODE_CFG_s));
            if (FuncRetCode == OK_UL) {
                ImgMode.ContextId = AmbaImgSystem_ContextIdGet(i);

                /* vin win size get */
                FuncRetCode = AmbaIK_GetWindowSizeInfo(&ImgMode, &VinWinInfo);
                if (FuncRetCode == IK_OK) {
                    /* vin win size put */
                    FuncRetCode = AmbaWrap_memcpy(&(EisCtrlWin.WindowSizeInfo), &VinWinInfo, sizeof(AMBA_IK_WINDOW_SIZE_INFO_s));
                    if (FuncRetCode != OK_UL) {
                        /* copy fail */
                        RetCode = NG_UL;
                    }
                } else {
                    /* vin win size fail */
                    AmbaImgPrint2(PRINT_FLAG_DBG, "error: eis vin win", VinId, ChainId);
                    RetCode = NG_UL;
                }

                /* act win get */
                FuncRetCode = AmbaIK_GetVinActiveWin(&ImgMode, &VinActWin);
                if (FuncRetCode == IK_OK) {
                    /* act win put */
                    FuncRetCode = AmbaWrap_memcpy(&(EisCtrlWin.VinActiveWin), &VinActWin, sizeof(AMBA_IK_VIN_ACTIVE_WINDOW_s));
                    if (FuncRetCode != OK_UL) {
                        /* copy fail */
                        RetCode = NG_UL;
                    }
                } else {
                    /* act win fail */
                    AmbaImgPrint2(PRINT_FLAG_DBG, "error: eis act win", VinId, ChainId);
                    RetCode = NG_UL;
                }
            } else {
                /* copy fail */
                RetCode = NG_UL;
            }
        } else {
            /* img mode fail */
            RetCode = NG_UL;
        }
    }

    /* shutter/sys info */
    if (RetCode == OK_UL) {
        EIS_MEM_INFO_s MemInfo;
        EIS_MEM_INFO_s MemInfo2;
        /* msg get */
        MemInfo.Ctx.Data = (AMBA_IMG_MAIN_MEM_ADDR) Msg;
        /* r2y user get */
        MemInfo2.Ctx.Data = (AMBA_IMG_MAIN_MEM_ADDR) MemInfo.Ctx.pPostAikInfo->R2yUser;
        /* shutter time put (TBD) */
        VarF = (MemInfo2.Ctx.pUserData->ShutterTime[0]*1000.f) + 0.5f;
        EisCtrlSensor.ExposureTime = (UINT32) VarF;
        /* sys info put (TBD) */
        EisCtrlSys.ChanID = 0U;
        EisCtrlSys.Frame = 0U; //MemInfo.Ctx.pPostAikInfo->RawCapSeq;
    }

    /* imu data get */
    if (RetCode == OK_UL) {
#ifdef CONFIG_BUILD_IMGFRW_EIS_POST
        AMBA_EISCTRL_USER_DATA_s EisCtrlResultTemp;
        static AMBA_IK_GRID_POINT_s GNU_SECTION_NOZEROINIT AmbaImgMainEis_WarpTableTemp3[AMBA_IMG_NUM_EIS_CHANNEL][IMG_MAX_WARP_GRID_SIZE];
#endif
        ImuFiFoDataNum = AmbaImg_ImuMgr_FiFoDataGet(&(AmbaImgMainEis_ImgImuPort[EisIdx]), 0U, AmbaImgMainEis_ImuFiFoDataInfo);
        if (ImuFiFoDataNum > 0U) {
            /* imu data put */
            EisCtrlImu.ExtImuDataCnt = ImuFiFoDataNum;
            EisCtrlImu.pExtImuInfoBuf = AmbaImgMainEis_ImuFiFoDataInfo;
            /* warp table buf (max) attach */
            EisCtrlResult.WarpInfo.pWarp = pWarpInfo->pWarp;
            /* eis proc */
#ifndef CONFIG_BUILD_IMGFRW_EIS_POST
            FuncRetCode = AmbaEisCtrl_Proc(&EisCtrlWin, &EisCtrlSensor, EisCtrlImu, EisCtrlSys, &EisCtrlResult);
#else
            /* warp table buf (temp) attach */
            EisCtrlResultTemp.WarpInfo.pWarp = AmbaImgMainEis_WarpTableTemp3[EisIdx];
            /* eis proc */
            FuncRetCode = AmbaEisCtrl_Proc(&EisCtrlWin, &EisCtrlSensor, EisCtrlImu, EisCtrlSys, &EisCtrlResultTemp);
            //FuncRetCode = AmbaEISCommon_UnitTest(&EisCtrlWin, &EisCtrlSensor, EisCtrlImu, EisCtrlSys, &EisCtrlResultTemp);
#endif
            if (FuncRetCode == OK_UL) {
#ifdef CONFIG_BUILD_IMGFRW_EIS_POST
                /* eis post proc */
                FuncRetCode = AmbaEISCtrl_PostProc(&EisCtrlResultTemp, &EisCtrlResult);
                if (FuncRetCode != OK_UL) {
                    /* crop fail */
                    RetCode = NG_UL;
                }
#endif
                FuncRetCode = AmbaWrap_memcpy(pActWin, &(EisCtrlResult.ActWin), sizeof(AMBA_IK_VIN_ACTIVE_WINDOW_s));
                if (FuncRetCode != OK_UL) {
                    /* copy fail */
                    RetCode = NG_UL;
                }
                FuncRetCode = AmbaWrap_memcpy(pDZoomInfo, &(EisCtrlResult.DZoomInfo), sizeof(AMBA_IK_DZOOM_INFO_s));
                if (FuncRetCode != OK_UL) {
                    /* copy fail */
                    RetCode = NG_UL;
                }
                FuncRetCode = AmbaWrap_memcpy(pDummyRange, &(EisCtrlResult.DummyRange), sizeof(AMBA_IK_DUMMY_MARGIN_RANGE_s));
                if (FuncRetCode != OK_UL) {
                    /* copy fail */
                    RetCode = NG_UL;
                }
                FuncRetCode = AmbaWrap_memcpy(pWarpInfo, &(EisCtrlResult.WarpInfo), sizeof(AMBA_IK_WARP_INFO_s));
                if (FuncRetCode != OK_UL) {
                    /* copy fail */
                    RetCode = NG_UL;
                }
            } else {
                /* eis proc fail */
                AmbaImgPrint2(PRINT_FLAG_DBG, "error: eis proc", VinId, ChainId);
                RetCode = NG_UL;
            }
        } else {
            /* imu data fail */
            AmbaImgPrint2(PRINT_FLAG_DBG, "error: eis imu data", VinId, ChainId);
            RetCode = NG_UL;
        }
    }

    return RetCode;
}
#else
/**
 *  Amba image main eis dummy eis algo
 *  @param[in] Param eis index with the eis algorithm id
 *  @param[in] ImageChanId image channel id
 *  @param[out] pActWin pointer to the active window data
 *  @param[out] pDZoomInfo pointer to the digital zoom information
 *  @param[out] pDummyRange pointer to the dummy margin range data
 *  @param[out] pWarpInfo pointer to the warp informatiopn
 *  @param[in] Msg psot aik information address
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgMainEis_DummyEisAlgo(UINT32 Param,
                                          AMBA_IMG_CHANNEL_ID_s ImageChanId,
                                          AMBA_IK_VIN_ACTIVE_WINDOW_s *pActWin,
                                          AMBA_IK_DZOOM_INFO_s *pDZoomInfo,
                                          AMBA_IK_DUMMY_MARGIN_RANGE_s *pDummyRange,
                                          AMBA_IK_WARP_INFO_s *pWarpInfo,
                                          UINT64 Msg)
{
    /*
     * dummy eis algo for the verification of timing
     */
#define IMG_EIS_MAX_WARP_HGRID_NUM    128U
#define IMG_EIS_MAX_WARP_VGRID_NUM    96U

    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i, j;

    AMBA_IK_WINDOW_SIZE_INFO_s VinWinInfo;

    UINT32 PostZoneId;

    UINT32 EisIdx = Param & 0xFFFFU;
    //UINT32 EisAlgoId = Param >> 16U;
    UINT32 FovId = 0U;

    AMBA_IK_MODE_CFG_s *pImgMode;
    AMBA_IK_MODE_CFG_s ImgMode;

    UINT32 Width;
    UINT32 Height;

    UINT32 MainWidth;
    UINT32 MainHeight;

    //UINT32 Factor = DUMMY_EIS_ZOOM_FACTOR;

    UINT32 DZoomWidth;
    UINT32 DZoomHeight;

    UINT32 DZoomShiftWidth;
    UINT32 DZoomShiftHeight;
     INT32 DZoomShiftX;
     INT32 DZoomShiftY;

    //UINT32 DummyFactor = DUMMY_EIS_DUMMY_FACTOR;

    UINT32 DummyWidth;
    UINT32 DummyHeight;

     INT32 DummyOffsetX;
     INT32 DummyOffsetY;

    //UINT32 ShiftFactor = DUMMY_EIS_SHIFT_FACTOR;

    UINT32 ShiftWidth;
    UINT32 ShiftHeight;

    UINT32 MaxShiftX;
    UINT32 MaxShiftY;

     INT32 VarL;

     INT32 ShiftX;
     INT32 ShiftY;

     INT32 X;
     INT32 Y;

     FLOAT VarF;

    /* param */
    //(void) Param;
    /* msg */
    (void) Msg;

    /* param update */
    FuncRetCode = AmbaImgMainEis_ParamUpdate(EisIdx);
    if (FuncRetCode != OK_UL) {
        /* */
    }

    /* warp shift update */
    FuncRetCode = AmbaImgMainEis_WarpUpdate(EisIdx);
    if (FuncRetCode != OK_UL) {
        /* */
    }

    /* post zone get */
    PostZoneId = ImageChanId.Ctx.Bits.ZoneId;

    /* post zone index get */
    for (i = 0U; (i < AMBA_IMG_SYSTEM_FOV_NUM) && ((PostZoneId >> i) > 0U); i++) {
        if ((PostZoneId & (((UINT32) 1U) << i)) > 0U) {
            FovId = i;
            break;
        }
    }

    if ((PostZoneId > 0U) && (i < AMBA_IMG_SYSTEM_FOV_NUM)) {
        /* image mode get */
        FuncRetCode = AmbaImgSystem_ImageModeGet(ImageChanId, AMBA_IK_PIPE_VIDEO, &pImgMode);
        if (FuncRetCode == OK_UL) {
            FuncRetCode = AmbaWrap_memcpy(&ImgMode, pImgMode, sizeof(AMBA_IK_MODE_CFG_s));
            if (FuncRetCode == OK_UL) {
                ImgMode.ContextId = AmbaImgSystem_ContextIdGet(FovId);

                /* vin win size get */
                FuncRetCode = AmbaIK_GetWindowSizeInfo(&ImgMode, &VinWinInfo);
                if (FuncRetCode == IK_OK) {
                    /* vin win size */
                    Width = VinWinInfo.VinSensor.Width;
                    Height = VinWinInfo.VinSensor.Height;

                    /* act win */
                    pActWin->Enable = 1U;
                    pActWin->ActiveGeo.StartX = 0U;
                    pActWin->ActiveGeo.StartY = 0U;
                    pActWin->ActiveGeo.Width = Width;
                    pActWin->ActiveGeo.Height = Height;

                    /* main win */
                    MainWidth = VinWinInfo.Main.Width;
                    MainHeight = VinWinInfo.Main.Height;

                    /* dzoom size */
                    DZoomWidth = (Width * 1000U) / AmbaImgMainEis_Param[EisIdx][0].DZoom.Factor;
                    DZoomHeight = (DZoomWidth * MainHeight) / MainWidth;

                    /* dzoom Y factor re-calculated */
                    AmbaImgMainEis_Param[EisIdx][0].DZoom.YFactor = ((Height * 1000U) + (DZoomHeight/2U)) / DZoomHeight;

                    /* shift size */
                    DZoomShiftWidth = (DZoomWidth * AmbaImgMainEis_Param[EisIdx][0].DZoom.ShiftXFactor) / 1000U;
                    DZoomShiftHeight = (DZoomHeight * AmbaImgMainEis_Param[EisIdx][0].DZoom.ShiftYFactor) / 1000U;

                    /* dzoom shift */
                    DZoomShiftX = ((((INT32) DZoomShiftWidth) - ((INT32) DZoomWidth)) / 2) * AmbaImgMainEis_Param[EisIdx][0].DZoom.ShiftXDir;
                    DZoomShiftY = ((((INT32) DZoomShiftHeight) - ((INT32) DZoomHeight)) / 2) * AmbaImgMainEis_Param[EisIdx][0].DZoom.ShiftYDir;

                    /* dzoom win */
                    pDZoomInfo->Enable = 1U;
                    pDZoomInfo->ShiftX = DZoomShiftX * 65536; /* 16.16 format */
                    pDZoomInfo->ShiftY = DZoomShiftY * 65536;
                    pDZoomInfo->ZoomX = (Width * 65536U) / DZoomWidth; /* 16.16 format */
                    pDZoomInfo->ZoomY = (Height * 65536U) / DZoomHeight;

                    /* dummy win size */
                    DummyWidth = (DZoomWidth * AmbaImgMainEis_Param[EisIdx][0].DummyRange.XFactor) / 1000U;
                    DummyHeight = (DZoomHeight * AmbaImgMainEis_Param[EisIdx][0].DummyRange.YFactor) / 1000U;

                    /* dzoom win offset from dummy */
                    DummyOffsetX = (((INT32) DummyWidth) - ((INT32) DZoomWidth)) / 2;
                    DummyOffsetY = (((INT32) DummyHeight) - ((INT32) DZoomHeight)) / 2;

                    /* dummy range (percent, .16 format) */
                    pDummyRange->Enable = 1U;
                    VarL = (DummyOffsetX * 65536) / ((INT32) Width);
                    pDummyRange->Left = (VarL > 0) ? (UINT32) VarL : 0U; /* .16 format */
                    pDummyRange->Right = (VarL > 0) ? (UINT32) VarL : 0U;
                    VarL = (DummyOffsetY * 65536) / ((INT32) Height);
                    pDummyRange->Top = (VarL > 0) ? (UINT32) VarL : 0U;
                    pDummyRange->Bottom = (VarL > 0) ? (UINT32) VarL : 0U;

                    /* shift win size */
                    ShiftWidth = (DZoomWidth * AmbaImgMainEis_Param[EisIdx][0].WarpShift.XFactor) / 1000U;
                    ShiftHeight = (DZoomHeight * AmbaImgMainEis_Param[EisIdx][0].WarpShift.YFactor) / 1000U;

                    /* max shift offset */
                    MaxShiftX = (ShiftWidth - DZoomWidth) >> 1U;
                    MaxShiftY = (ShiftHeight - DZoomHeight) >> 1U;

                    /* warp (random shift), testing only */
                    FuncRetCode = AmbaWrap_rand(&X); /*0~0x7FFFFFFF*/
                    if (FuncRetCode == OK) {
                        VarF = (FLOAT) (((((((DOUBLE) X) / 32768.0f) - 32768.0f) / 32768.0f) * ((DOUBLE) MaxShiftX) * 65536.0f)/4096.0f);
                        ShiftX = (INT32) VarF; /* 11.4 foramt */
                    } else {
                        ShiftX = 0;
                    }
                    FuncRetCode = AmbaWrap_rand(&Y);
                    if (FuncRetCode == OK) {
                        VarF = (FLOAT) (((((((DOUBLE) Y) / 32768.0f) - 32768.0f) / 32768.0f) * ((DOUBLE) MaxShiftY) * 65536.0f)/4096.0f);
                        ShiftY = (INT32) VarF; /* 11.4 foramt */
                    } else {
                        ShiftY = 0;
                    }
                    /* warp */
                    FuncRetCode = AmbaWrap_memcpy(&(pWarpInfo->VinSensorGeo), &(VinWinInfo.VinSensor), sizeof(AMBA_IK_VIN_SENSOR_GEOMETRY_s));
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                    /* version */
                    pWarpInfo->Version = 0x20180401U; //IK_WARP_VER;
                    /* hor tile */
                    for (i = 3U; i < 9U; i++) { /* 2^3 = 8, 2^4 = 16 ~ 2^9 = 512 */
                        if (((((UINT32) 1U) << i) * (IMG_EIS_MAX_WARP_HGRID_NUM - 1U)) >= Width) {
                            break;
                        }
                    }
                    /* hor tile exp */
                    pWarpInfo->TileWidthExp = i;
                    /* hor grid num */
                    pWarpInfo->HorGridNum = ((Width + ((((UINT32) 1U) << i) -1U)) >> i) + 1U;
                    /* ver tile */
                    for (j = 3U; j < 9U; j++) { /* 2^3 = 8, 2^4 = 16 ~ 2^9 = 512 */
                        if (((((UINT32) 1U) << j) * (IMG_EIS_MAX_WARP_VGRID_NUM - 1U)) >= Height) {
                            break;
                        }
                    }
                    /* ver tile exp */
                    pWarpInfo->TileHeightExp = j;
                    /* ver grid num */
                    pWarpInfo->VerGridNum = ((Height + ((((UINT32) 1U) << j) -1U)) >> j) + 1U;
                    /* warp table (2D) */
                    pWarpInfo->Enb_2StageCompensation = 1U;
                    /* warp table put */
                    switch (AmbaImgMainEis_WarpShift[EisIdx][0].Mode) {
                        case IMG_EIS_WARP_SHIFT_RANDOM:
                            /* random shift */
                            for (i = 0U; i < pWarpInfo->VerGridNum; i ++) {
                                for (j = 0U; j < pWarpInfo->HorGridNum; j++) {
                                    pWarpInfo->pWarp[(i*pWarpInfo->HorGridNum)+j].X = (INT16) ShiftX;
                                    pWarpInfo->pWarp[(i*pWarpInfo->HorGridNum)+j].Y = (INT16) ShiftY;
                                }
                            }
                            break;
                        case IMG_EIS_WARP_SHIFT_MANUAL:
                            /* manual shift */
                            for (i = 0U; i < pWarpInfo->VerGridNum; i ++) {
                                for (j = 0U; j < pWarpInfo->HorGridNum; j++) {
                                    pWarpInfo->pWarp[(i*pWarpInfo->HorGridNum)+j].X = (INT16) AmbaImgMainEis_WarpShift[EisIdx][0].Vector.X * 16;
                                    pWarpInfo->pWarp[(i*pWarpInfo->HorGridNum)+j].Y = (INT16) AmbaImgMainEis_WarpShift[EisIdx][0].Vector.Y * 16;
                                }
                            }
                            break;
                        case IMG_EIS_WARP_SHIFT_LINE:
                            /* line moving */
                            {
                                INT32 *pStepDir = &(AmbaImgMainEis_WarpShift[EisIdx][0].StepDir);
                                const INT32 *pStepFactor = &(AmbaImgMainEis_WarpShift[EisIdx][0].StepFactor);
                                INT32 *pAccumulatedFactor = &(AmbaImgMainEis_WarpShift[EisIdx][0].AccumulatedFactor);

                                AMBA_IMG_MAIN_EIS_WARP_POINT_s *pVector = &(AmbaImgMainEis_WarpShift[EisIdx][0].Vector);

                                const AMBA_IMG_MAIN_EIS_WARP_POINT_s *pPoint1 = &(AmbaImgMainEis_WarpShift[EisIdx][0].Point[0]);
                                const AMBA_IMG_MAIN_EIS_WARP_POINT_s *pPoint2 = &(AmbaImgMainEis_WarpShift[EisIdx][0].Point[1]);

                                (*pAccumulatedFactor) += ((*pStepFactor) * (*pStepDir));

                                if (((*pAccumulatedFactor) >= 1000) || ((*pAccumulatedFactor) <= 0)) {
                                    (*pStepDir) *= -1;
                                }

                                pVector->X = pPoint1->X + (((pPoint2->X - pPoint1->X) * (*pAccumulatedFactor)) / 1000);
                                pVector->Y = pPoint1->Y + (((pPoint2->Y - pPoint1->Y) * (*pAccumulatedFactor)) / 1000);

                                for (i = 0U; i < pWarpInfo->VerGridNum; i ++) {
                                    for (j = 0U; j < pWarpInfo->HorGridNum; j++) {
                                        pWarpInfo->pWarp[(i*pWarpInfo->HorGridNum)+j].X = (INT16) (pVector->X * 16);
                                        pWarpInfo->pWarp[(i*pWarpInfo->HorGridNum)+j].Y = (INT16) (pVector->Y * 16);
                                    }
                                }
                            }
                            break;
                        default:
                            /* zero shift */
                            for (i = 0U; i < pWarpInfo->VerGridNum; i ++) {
                                for (j = 0U; j < pWarpInfo->HorGridNum; j++) {
                                    pWarpInfo->pWarp[(i*pWarpInfo->HorGridNum)+j].X = (INT16) 0;
                                    pWarpInfo->pWarp[(i*pWarpInfo->HorGridNum)+j].Y = (INT16) 0;
                                }
                            }
                            break;
                    }
#if 0 /* disabled for the multi-fov example */
                    /*
                     * add latency for timing testing
                     */
                    FuncRetCode = AmbaKAL_TaskSleep(2U);
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* */
                    }
#endif
#if 1
                    /* debug message */
                    {
                        UINT32 VinId = ImageChanId.Ctx.Bits.VinId;

                        /* debug timing enable? */
                        if (AmbaImgSensorHAL_TimingMarkChk(VinId) == OK_UL)  {
                            char str[11];
                            str[0] = ' ';

                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Dummy_Eis");
                            str[1] = 'f';
                            var_utoa(FovId, &(str[2]), 16U, 8U, (UINT32) VAR_LEADING_SPACE);
                            AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                            /* dummy win */
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Dummy_Win");
                            str[1] = 'w';
                            var_utoa(DummyWidth, &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                            AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                            str[1] = 'h';
                            var_utoa(DummyHeight, &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                            AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                            /* shift win */
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Shift_Win");
                            str[1] = 'w';
                            var_utoa(ShiftWidth, &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                            AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                            str[1] = 'h';
                            var_utoa(ShiftHeight, &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                            AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                            /* dzoom win */
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Dzoom_Win");
                            str[1] = 'x';
                            var_itoa(DZoomShiftX, &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                            AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                            str[1] = 'y';
                            var_itoa(DZoomShiftY, &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                            AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                            str[1] = 'w';
                            var_utoa(DZoomWidth, &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                            AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                            str[1] = 'h';
                            var_utoa(DZoomHeight, &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                            AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                            /* shift x & y */
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Shift_XY");
                            str[1] = 'x';
                            var_itoa(ShiftX/16, &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                            AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                            str[1] = 'y';
                            var_itoa(ShiftY/16, &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                            AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                        }
                    }
#endif
                } else {
                    RetCode = NG_UL;
                }
            } else {
                /* copy fail */
                RetCode = NG_UL;
            }
        } else {
            /* img mode fail */
            RetCode = NG_UL;
        }
    } else {
        /* zone id invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}
#endif
/**
 *  Amba image main eis algorithm task ex
 *  @param[in] pParam pointer to parameter
 *  @note this function is intended for internal use only
 */
static void *AmbaImgMainEis_AlgoTaskEx(void *pParam)
{
    EIS_MEM_INFO_s MemInfo;

    MemInfo.Ctx.pVoid = pParam;
    AmbaImgMainEis_AlgoTask((UINT32) MemInfo.Ctx.Data);

    return NULL;
}

/**
 *  Amba image main eis algorithm task
 *  @param[in] Param eis index with the eis algo id
 *  @note this function is intended for internal use only
 */
static void AmbaImgMainEis_AlgoTask(UINT32 Param)
{
    static UINT32 GNU_SECTION_NOZEROINIT AmbaImgMainEis_WarpTableIndex[AMBA_IMG_SYSTEM_FOV_NUM];
#define IMG_MAX_WARP_RING_SIZE    6U
    static AMBA_IK_GRID_POINT_s GNU_SECTION_NOZEROINIT AmbaImgMainEis_WarpTable[AMBA_IMG_SYSTEM_FOV_NUM][IMG_MAX_WARP_RING_SIZE][IMG_MAX_WARP_GRID_SIZE];
#if defined(CONFIG_BUILD_IMGFRW_EIS_WARP_CT) || defined(CONFIG_BUILD_IMGFRW_EIS_WARP_AC)
    static AMBA_IK_GRID_POINT_s GNU_SECTION_NOZEROINIT AmbaImgMainEis_WarpTableTemp[AMBA_IMG_SYSTEM_FOV_NUM][IMG_MAX_WARP_GRID_SIZE];
#endif
#if defined (CONFIG_BUILD_IMGFRW_EIS_WARP_CT) && defined(CONFIG_BUILD_IMGFRW_EIS_WARP_AC)
    static AMBA_IK_GRID_POINT_s GNU_SECTION_NOZEROINIT AmbaImgMainEis_WarpTableTemp2[AMBA_IMG_SYSTEM_FOV_NUM][IMG_MAX_WARP_GRID_SIZE];
#endif
#ifdef CONFIG_BUILD_IMGFRW_EIS_WARP_AC
    UINT32 LivWarpInfoFlag = 0U;
#endif
#ifdef CONFIG_BUILD_IMGFRW_EIS_WARP_CT
    UINT32 WarpConvFlag = 0U;
#endif
    UINT32 FuncRetCode;

    UINT32 PostZoneId;
    UINT32 FovId = 0U;
    UINT32 i;

    UINT32 *pRingIdx;

    UINT64 Msg;

    EIS_MEM_INFO_s MemInfo;
    EIS_MEM_INFO_s MemInfo2;

    UINT32 VinId;
    UINT32 EisIdx = Param & 0xFFFFU;
    //UINT32 EisAlgoId = Param >> 16U;

    /* warp table index reset */
    for (i = 0U; (i < AMBA_IMG_SYSTEM_FOV_NUM) && ((AmbaImgMainEis_ImageChanId[EisIdx].Ctx.Bits.ZoneId >> i) > 0U); i++) {
        if ((AmbaImgMainEis_ImageChanId[EisIdx].Ctx.Bits.ZoneId & (((UINT32) 1U) << i)) > 0U) {
            AmbaImgMainEis_WarpTableIndex[i] = 0U;
        }
    }

    while (AmbaImgMainEis_AlgoDummyFlag[EisIdx] == 1U) {
        /* msg receive */
        FuncRetCode = AmbaKAL_MsgQueueReceive(&(AmbaImgMainEis_AlgoQueue[EisIdx]), &Msg, AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            /* msg get */
            MemInfo.Ctx.Data = (AMBA_IMG_MAIN_MEM_ADDR) Msg;
            /* vin id get */
            VinId = MemInfo.Ctx.pPostAikInfo->ImageChanId.Ctx.Bits.VinId;
            /*
             * algo go, then put the result of warp info into r2y user
             */
            MemInfo2.Ctx.Data = (AMBA_IMG_MAIN_MEM_ADDR) MemInfo.Ctx.pPostAikInfo->R2yUser;

            /* post zone id get */
            PostZoneId = MemInfo.Ctx.pPostAikInfo->ImageChanId.Ctx.Bits.ZoneId;

            /* post zone index get */
            for (i = 0U; (i < AMBA_IMG_SYSTEM_FOV_NUM) && ((PostZoneId >> i) > 0U); i++) {
                if ((PostZoneId & (((UINT32) 1U) << i)) > 0U) {
                    FovId = i;
                    break;
                }
            }

            /* post zone check */
            if ((PostZoneId > 0U) &&
                (i < AMBA_IMG_SYSTEM_FOV_NUM)) {
                /* warp table ring index get */
                pRingIdx = &(AmbaImgMainEis_WarpTableIndex[FovId]);
#ifdef CONFIG_BUILD_IMGFRW_EIS_WARP_AC
                /* liv (LDC) warp info get for evaluation (ik 2D warp table needed, CV2x only) */
#ifdef CONFIG_BUILD_IMGFRW_EIS_WARP_LV
                /* liv warp info flag check */
                if ((LivWarpInfoFlag == 0U) &&
                    (MemInfo.Ctx.pPostAikInfo->RawCapSeq <= 5U)) {
                    AMBA_IK_MODE_CFG_s *pImgMode;
                    AMBA_IK_MODE_CFG_s ImgMode;
                    /* image mode get */
                    FuncRetCode = AmbaImgSystem_ImageModeGet(MemInfo.Ctx.pPostAikInfo->ImageChanId, AMBA_IK_PIPE_VIDEO, &pImgMode);
                    if (FuncRetCode == OK_UL) {
                        FuncRetCode = AmbaWrap_memcpy(&ImgMode, pImgMode, sizeof(AMBA_IK_MODE_CFG_s));
                        if (FuncRetCode == OK_UL) {
                            /* context id get */
                            ImgMode.ContextId = AmbaImgSystem_ContextIdGet(i);
                            /* liv warp info get */
                            FuncRetCode = AmbaIK_GetWarpInfo(&ImgMode, &(AmbaImgMainEis_LivWarpInfo[FovId]));
                            if (FuncRetCode == IK_OK) {
                                /* sucess */
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Eis_Liv_W");
                                /* debug message */
                                AmbaImgMainEis_WarpInfoDebug("liv warp", FovId, &(AmbaImgMainEis_LivWarpInfo[FovId]));
                            } else {
                                /* fail */
                                AmbaImgPrint2(PRINT_FLAG_ERR, "error: eis liv warp info", EisIdx, ImgMode.ContextId);
                                /* debug message */
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Eis_Liv_W!");
                            }
                        }
                    }

                    /* liv flag update */
                    LivWarpInfoFlag++;
                }
#else
                if (LivWarpInfoFlag == 0U) {
                    /* debug message */
                    AmbaImgMainEis_WarpInfoDebug("app warp", FovId, &(AmbaImgMainEis_LivWarpInfo[FovId]));
                    /* liv flag update */
                    LivWarpInfoFlag++;
                }
#endif
#endif
#ifdef CONFIG_BUILD_IMGFRW_EIS_GYRODLY
                /* algo delay for gyro acquired (example only) */
                {
                    UINT32 GyroUserDelay = AmbaImgMainEis_GyroUserDelay[EisIdx]; /* (GyroUserDelay + Algo Calculation time) < R2yUserDelay */
                    UINT32 SysTimetick;
                    UINT32 EorTimetick;
                    UINT32 GyroTimetick;
                    UINT32 Time2Wait;
                    /* sys timetick get */
                    FuncRetCode = AmbaKAL_GetSysTickCount(&SysTimetick);
                    if (FuncRetCode == KAL_ERR_NONE) {
                        /* eor timetick get */
                        EorTimetick = MemInfo.Ctx.pPostAikInfo->EorTimetick;
                        /* gyro user delay timetick get */
                        GyroTimetick = EorTimetick + GyroUserDelay;
                        /* time to wait */
                        if (GyroTimetick > SysTimetick) {
                            Time2Wait = GyroTimetick - SysTimetick;
                            /* debug message */
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Eis_Gyro_W");
                            /* debug timing enable? */
                            if (AmbaImgSensorHAL_TimingMarkChk(VinId) == OK_UL) {
                                char str[11];
                                str[0] = ' ';

                                str[1] = 'e';
                                var_utoa(EorTimetick, &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                                AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                                str[1] = 'g';
                                var_utoa(GyroUserDelay, &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                                AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                                str[1] = 'w';
                                var_utoa(Time2Wait, &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                                AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                            }
                            FuncRetCode = AmbaKAL_TaskSleep(Time2Wait);
                            if (FuncRetCode != KAL_ERR_NONE) {
                                /* */
                            }
                        }
                    }
                }
#endif
#if defined(CONFIG_BUILD_IMGFRW_EIS_WARP_CT) || defined(CONFIG_BUILD_IMGFRW_EIS_WARP_AC)
#ifndef CONFIG_BUILD_IMGFRW_EIS_WARP_CT /* ac only */
                if (AmbaImgMainEis_LivWarpInfo[FovId].pWarp != NULL) {
                    /* warp table (temp) hook */
                    MemInfo2.Ctx.pUserData->WarpInfo.pWarp = AmbaImgMainEis_WarpTableTemp[FovId];
                } else {
                    /* warp table (ring) hook directly */
                    MemInfo2.Ctx.pUserData->WarpInfo.pWarp = AmbaImgMainEis_WarpTable[FovId][(*pRingIdx)];
                }
#else /* ct only or ac+ct */
                /* warp table (temp) hook */
                MemInfo2.Ctx.pUserData->WarpInfo.pWarp = AmbaImgMainEis_WarpTableTemp[FovId];
#endif
#else
                /* warp table (ring) hook directly */
                MemInfo2.Ctx.pUserData->WarpInfo.pWarp = AmbaImgMainEis_WarpTable[FovId][(*pRingIdx)];
#endif
                /* amba eis algo */
                AmbaImgSensorHAL_TimingMarkPut(VinId, "Eis_Algo");
                /* debug timing enable? */
                if (AmbaImgSensorHAL_TimingMarkChk(VinId) == OK_UL) {
                    char str[11];
                    str[0] = ' '; str[1] = 'r';
                    var_utoa((*pRingIdx), &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                    AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                }
#ifdef CONFIG_BUILD_IMGFRW_EIS
                FuncRetCode = AmbaImgMainEis_ImgEisAlgo(Param,
                                                        MemInfo.Ctx.pPostAikInfo->ImageChanId,
                                                        &(MemInfo2.Ctx.pUserData->ActWin),
                                                        &(MemInfo2.Ctx.pUserData->DZoomInfo),
                                                        &(MemInfo2.Ctx.pUserData->DummyRange),
                                                        &(MemInfo2.Ctx.pUserData->WarpInfo),
                                                        Msg);
#else
                FuncRetCode = AmbaImgMainEis_DummyEisAlgo(Param,
                                                          MemInfo.Ctx.pPostAikInfo->ImageChanId,
                                                          &(MemInfo2.Ctx.pUserData->ActWin),
                                                          &(MemInfo2.Ctx.pUserData->DZoomInfo),
                                                          &(MemInfo2.Ctx.pUserData->DummyRange),
                                                          &(MemInfo2.Ctx.pUserData->WarpInfo),
                                                          Msg);
#endif
                if (FuncRetCode == OK_UL) {
#ifdef CONFIG_BUILD_IMGFRW_EIS_WARP_AC
                    /* additional liv (LDC) warp table add for evaluation (2D warp table needed) */
                    if (AmbaImgMainEis_LivWarpInfo[FovId].pWarp != NULL) {
                        AMBA_WU_WARP_ADD_IN_s WarpAddIn;
                        AMBA_WU_WARP_ADD_RESULT_s WarpAddOut;

                        /* debug message */
                        if (LivWarpInfoFlag == 1U) {
                            AmbaImgMainEis_WarpInfoDebug("warp eis", FovId, &(MemInfo2.Ctx.pUserData->WarpInfo));
                        }

                        /* warp in (liv) */
                        FuncRetCode = AmbaWrap_memcpy(&(WarpAddIn.InputAInfo), &(AmbaImgMainEis_LivWarpInfo[FovId]), sizeof(AMBA_IK_WARP_INFO_s));
                        if (FuncRetCode != OK_UL) {
                            /* */
                        }
                        /* warp in (eis) */
                        FuncRetCode = AmbaWrap_memcpy(&(WarpAddIn.InputBInfo), &(MemInfo2.Ctx.pUserData->WarpInfo), sizeof(AMBA_IK_WARP_INFO_s));
                        if (FuncRetCode != OK_UL) {
                            /* */
                        }
                        /* warp out table (ring) hook */
#ifndef CONFIG_BUILD_IMGFRW_EIS_WARP_CT
                        MemInfo2.Ctx.pUserData->WarpInfo.pWarp = AmbaImgMainEis_WarpTable[FovId][(*pRingIdx)];
#else
                        MemInfo2.Ctx.pUserData->WarpInfo.pWarp = AmbaImgMainEis_WarpTableTemp2[FovId];
#endif
                        /* warp out */
                        FuncRetCode = AmbaWrap_memcpy(&(WarpAddOut.ResultInfo), &(MemInfo2.Ctx.pUserData->WarpInfo), sizeof(AMBA_IK_WARP_INFO_s));
                        if (FuncRetCode != OK_UL) {
                            /* */
                        }

                        /* debug message */
                        if (LivWarpInfoFlag == 1U) {
                            /* warp in (liv) */
                            AmbaImgMainEis_WarpInfoDebug("warp in (a)", FovId, &(WarpAddIn.InputAInfo));
                            /* warp in (eis) */
                            AmbaImgMainEis_WarpInfoDebug("warp in (b)", FovId, &(WarpAddIn.InputBInfo));
                            {
                                EIS_MEM_INFO_s MemInfo3;
                                MemInfo3.Ctx.pWarp = AmbaImgMainEis_WarpTableTemp[FovId];
                                AmbaImgPrint64Ex(PRINT_FLAG_WARP, " warp temp table", (UINT64) MemInfo3.Ctx.Data, 16U);
                                AmbaImgPrintStr(PRINT_FLAG_WARP, "\n\n");
                            }
                            /* warp out (liv+eis) */
                            AmbaImgMainEis_WarpInfoDebug("warp out", FovId, &(WarpAddOut.ResultInfo));
#ifdef CONFIG_BUILD_IMGFRW_EIS_WARP_CT
                            {
                                EIS_MEM_INFO_s MemInfo3;
                                MemInfo3.Ctx.pWarp = AmbaImgMainEis_WarpTableTemp2[FovId];
                                AmbaImgPrint64Ex(PRINT_FLAG_WARP, " warp temp2 table", (UINT64) MemInfo3.Ctx.Data, 16U);
                                AmbaImgPrintStr(PRINT_FLAG_WARP, "\n\n");
                            }
#endif
                            /* liv flag update */
                            LivWarpInfoFlag++;
                        }

                        /* warp add */
                        FuncRetCode = AmbaWU_WarpAdd(&WarpAddIn, &WarpAddOut);
                        if (FuncRetCode == WU_OK) {
                            /* warp out copy */
                            FuncRetCode = AmbaWrap_memcpy(&(MemInfo2.Ctx.pUserData->WarpInfo), &(WarpAddOut.ResultInfo), sizeof(AMBA_IK_WARP_INFO_s));
                            if (FuncRetCode != OK_UL) {
                                /* */
                            }
                            /* debug message */
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Warp_Add");
                            if (LivWarpInfoFlag == 2U) {
                                AmbaImgPrintStr(PRINT_FLAG_WARP, "warp add\n\n");
                            }
                        } else {
                            /* debug message */
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Warp_Add!");
                            if (LivWarpInfoFlag == 2U) {
                                AmbaImgPrintStr(PRINT_FLAG_WARP, "error: warp add\n\n");
                            }
                        }

                        /* debug message */
                        if (LivWarpInfoFlag == 2U) {
                            /* warp add */
                            AmbaImgMainEis_WarpInfoDebug("warp add", FovId, &(MemInfo2.Ctx.pUserData->WarpInfo));
                            /* liv flag update */
                            LivWarpInfoFlag++;
                        }
                    } else {
                        /* debug message */
                        if (LivWarpInfoFlag == 1U) {
                            AmbaImgMainEis_WarpInfoDebug("warp eis", FovId, &(MemInfo2.Ctx.pUserData->WarpInfo));
                            {
                                EIS_MEM_INFO_s MemInfo3;
                                MemInfo3.Ctx.pWarp = AmbaImgMainEis_WarpTableTemp[FovId];
                                AmbaImgPrint64Ex(PRINT_FLAG_WARP, " warp temp table", (UINT64) MemInfo3.Ctx.Data, 16U);
                                AmbaImgPrintStr(PRINT_FLAG_WARP, "\n\n");
                            }
                            LivWarpInfoFlag++;
                        }
                    }
#endif
#ifdef CONFIG_BUILD_IMGFRW_EIS_WARP_CT
                    /* warp table 2D-to-H_V_1D for evaluation (CV2xFS/CV5 ik H_V_1D needed) */
                    {
#define IMG_WARP_CONV_WORKING_BUF_SIZE   393984U
                        static UINT8 GNU_SECTION_NOZEROINIT AmbaImgMainEis_WorkingBuf[AMBA_IMG_SYSTEM_FOV_NUM][IMG_WARP_CONV_WORKING_BUF_SIZE];
                        AMBA_IK_IN_WARP_PRE_PROC_s WarpInfoIn;
                        AMBA_IK_OUT_WARP_PRE_PROC_s WarpInfoOut;
                        AMBA_IK_WARP_INFO_s WarpInfoTemp;
                        EIS_MEM_INFO_s MemInfo3;

                        /* warp info temp copy */
                        FuncRetCode = AmbaWrap_memcpy(&WarpInfoTemp, &(MemInfo2.Ctx.pUserData->WarpInfo), sizeof(AMBA_IK_WARP_INFO_s));
                        if (FuncRetCode == OK_UL) {
                            /* debug message */
                            if (WarpConvFlag == 0U) {
                                AmbaImgMainEis_WarpInfoDebug("warp eis", FovId, &(MemInfo2.Ctx.pUserData->WarpInfo));
                                {
                                    MemInfo3.Ctx.pWarp = AmbaImgMainEis_WarpTableTemp[FovId];
                                    AmbaImgPrint64Ex(PRINT_FLAG_WARP, " warp temp table", (UINT64) MemInfo3.Ctx.Data, 16U);
                                    AmbaImgPrintStr(PRINT_FLAG_WARP, "\n\n");
                                }
                                WarpConvFlag++;
                            }

                            /* warp table (ring) hook */
                            MemInfo2.Ctx.pUserData->WarpInfo.pWarp = AmbaImgMainEis_WarpTable[FovId][(*pRingIdx)];

                            /* warp in */
                            WarpInfoIn.pInputInfo = &WarpInfoTemp;
                            /* warp working buf */
                            MemInfo3.Ctx.pUint8 = AmbaImgMainEis_WorkingBuf[FovId];
                            WarpInfoIn.pWorkingBuffer = MemInfo3.Ctx.pVoid;
                            WarpInfoIn.WorkingBufferSize = IMG_WARP_CONV_WORKING_BUF_SIZE;
                            /* warp out */
                            WarpInfoOut.pResultInfo = &(MemInfo2.Ctx.pUserData->WarpInfo);
                            /* warp conv */
                            FuncRetCode = AmbaIK_PreProcWarpTbl(&WarpInfoIn, &WarpInfoOut);
                            if (FuncRetCode == IK_OK) {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Warp_Conv");
                                /* debug message */
                                if (WarpConvFlag == 1U) {
                                    AmbaImgMainEis_WarpInfoDebug("warp conv", FovId, &(MemInfo2.Ctx.pUserData->WarpInfo));
                                    WarpConvFlag++;
                                }
                            } else {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Warp_Conv!");
                            }
                        }
                    }
#endif
                    /* warp table ring index update */
                    (*pRingIdx) = ((*pRingIdx) + 1U) % IMG_MAX_WARP_RING_SIZE;
                    /* update flag set */
                    MemInfo2.Ctx.pUserData->UpdateFlag = 1U;
                } else {
                    /* update flag clear */
                    MemInfo2.Ctx.pUserData->UpdateFlag = 0U;
                }
                /* r2y user done */
                FuncRetCode = AmbaImgSystem_R2yUserDone((UINT64) MemInfo.Ctx.Data);
                if (FuncRetCode == OK_UL) {
                    AmbaImgSensorHAL_TimingMarkPut(VinId, "R2y_UserD");
                } else {
                    AmbaImgSensorHAL_TimingMarkPut(VinId, "R2y_UserD!");
                }
            }
        }
    }
}

#if defined(CONFIG_BUILD_IMGFRW_EIS_WARP_AC) || defined(CONFIG_BUILD_IMGFRW_EIS_WARP_CT)
/**
 *  Amba image main eis warp info debug
 *  @param[in] pName pointer to the string
 *  @param[in] FovId fov id
 *  @param[in] pWarpInfo pointer to the warp information
 *  @note this function is intended for internal use only
 */
static void AmbaImgMainEis_WarpInfoDebug(const char *pName, UINT32 FovId, const AMBA_IK_WARP_INFO_s *pWarpInfo)
{
#if 1
    EIS_MEM_INFO_s MemInfo3;

    AmbaImgPrintStr(PRINT_FLAG_WARP, pName);
    AmbaImgPrintEx(PRINT_FLAG_WARP, " fov id", FovId, 10U);
    AmbaImgPrintEx(PRINT_FLAG_WARP, " version", pWarpInfo->Version, 16U);
    AmbaImgPrintEx(PRINT_FLAG_WARP, " tile width exp", pWarpInfo->TileWidthExp, 10U);
    AmbaImgPrintEx(PRINT_FLAG_WARP, " tile height exp", pWarpInfo->TileHeightExp, 10U);
    AmbaImgPrintEx(PRINT_FLAG_WARP, " hor grip num", pWarpInfo->HorGridNum, 10U);
    AmbaImgPrintEx(PRINT_FLAG_WARP, " ver grip num", pWarpInfo->VerGridNum, 10U);
    AmbaImgPrintEx(PRINT_FLAG_WARP, " enable 2-stage compensation", pWarpInfo->Enb_2StageCompensation, 10U);
    AmbaImgPrintEx(PRINT_FLAG_WARP, " vin sensor geo start x", pWarpInfo->VinSensorGeo.StartX, 10U);
    AmbaImgPrintEx(PRINT_FLAG_WARP, " vin sensor geo start y", pWarpInfo->VinSensorGeo.StartY, 10U);
    AmbaImgPrintEx(PRINT_FLAG_WARP, " vin sensor geo width", pWarpInfo->VinSensorGeo.Width, 10U);
    AmbaImgPrintEx(PRINT_FLAG_WARP, " vin sensor geo height", pWarpInfo->VinSensorGeo.Height, 10U);
    AmbaImgPrintEx(PRINT_FLAG_WARP, " vin sensor geo h sub sample num", pWarpInfo->VinSensorGeo.HSubSample.FactorNum, 10U);
    AmbaImgPrintEx(PRINT_FLAG_WARP, " vin sensor geo h sub sample den", pWarpInfo->VinSensorGeo.HSubSample.FactorDen, 10U);
    AmbaImgPrintEx(PRINT_FLAG_WARP, " vin sensor geo v sub sample num", pWarpInfo->VinSensorGeo.VSubSample.FactorNum, 10U);
    AmbaImgPrintEx(PRINT_FLAG_WARP, " vin sensor geo v sub sample den", pWarpInfo->VinSensorGeo.VSubSample.FactorDen, 10U);
    MemInfo3.Ctx.pWarp = pWarpInfo->pWarp;
    AmbaImgPrint64Ex(PRINT_FLAG_WARP, " warp table", (UINT64) MemInfo3.Ctx.Data, 16U);
    AmbaImgPrintStr(PRINT_FLAG_WARP, "\n\n");
#else
    (void) pName;
    (void) FovId;
    (void) pWarpInfo;
#endif
}
#endif
