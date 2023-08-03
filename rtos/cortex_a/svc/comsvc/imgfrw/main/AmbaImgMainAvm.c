/**
 *  @file AmbaImgMainAvm.c
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
 *  @details Amba Image Main Avm
 *
 */

#include "AmbaTypes.h"
#include "AmbaVIN_Def.h"

#include "AmbaKAL.h"
#include "AmbaWrap.h"

#include "AmbaDSP_ImageDefine.h"
#include "AmbaDSP_ImageFilter.h"

#include "AmbaImg_External_CtrlFunc.h"
#include "Amba_AeAwbAdj_Control.h"
#include "Amba_Image.h"

#include "AmbaImgFramework.h"
#include "AmbaImgMain.h"
#include "AmbaImgMain_Internal.h"
#include "AmbaImgMainFov.h"
#include "AmbaImgMainAvm.h"
#include "AmbaImgMainAvm_Platform.h"

#define OK_UL    ((UINT32) 0U)
#define NG_UL    ((UINT32) 1U)

typedef union /*_AVM_MEM_INFO_u_*/ {
    AMBA_IMG_MAIN_MEM_ADDR       Data;
    void                         *pVoid;
    const void                   *pCvoid;
    AMBA_IK_CFA_3A_DATA_s        *pCfa;
    AMBA_IK_PG_3A_DATA_s         *pRgb;
    AMBA_IP_GLOBAL_AAA_DATA_s    *pAaaData;
    AMBA_DISP_ROI_RGB_STAT_s     *pRoiRgbStat;
} AVM_MEM_INFO_u;

typedef struct /*_AVM_MEM_INFO_s_*/ {
    AVM_MEM_INFO_u Ctx;
} AVM_MEM_INFO_s;

static AMBA_KAL_TASK_t GNU_SECTION_NOZEROINIT AmbaImgMainAvmTaskId;
static void AmbaImgMainAvm_Task(UINT32 Param);
static void *AmbaImgMainAvm_TaskEx(void *pParam);

static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaImgMainAvm_Ready;
static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaImgMainAvm_Go;
static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaImgMainAvm_Ack;

static UINT32 AmbaImgMainAvm_Cmd = (UINT32) AVM_TASK_CMD_STOP;

AMBA_IMG_CHANNEL_ID_s AmbaImgMainAvm_ImageChanId;

static UINT32 AmbaImgMainAvm_ImgIq = 0U;
static UINT32 AmbaImgMainAvm_ImgHdr = 0U;
static UINT32 AmbaImgMainAvm_ImgCore = 0U;

static UINT32 AmbaImgMainAvm_DummyFlag;

UINT32 AmbaImgMainAvm_DebugCount = 0U;

#define AMBA_IMG_MAIN_AVM_MAX_FOV_NUM  (8U)
static AMBA_DISP_ROI_RGB_STAT_s *AmbaImgMainAvm_RoiRgbStat = NULL;

static UINT32 AmbaImgMainAvm_Reversed = 0U;

static UINT32 AmbaImgMainAvm_Create(UINT32 Priority, UINT32 CoreInclusion);
static UINT32 AmbaImgMainAvm_Delete(void);
static UINT32 AmbaImgMainAvm_Active(void);
static UINT32 AmbaImgMainAvm_Inactive(void);
static UINT32 AmbaImgMainAvm_IdleWait(void);

/**
 *  Amba image main avm roi rgb handler
 *  @param[in] pRoiRgbStat pointer to the roi rgb statistics data
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgMainAvm_RoiRgbHandler(AMBA_DISP_ROI_RGB_STAT_s *pRoiRgbStat)
{
    AmbaImgMainAvm_RoiRgbStat = pRoiRgbStat;
    return OK_UL;
}

/**
 *  Amba image main avm image framework dispatch
 *  @param[in] VinId vin id
 *  @param[in] Param command and message
 *  @note this function is intended for internal use only
 */
static void AmbaImgMainAvm_ImgFrwDispatch(UINT32 VinId, UINT64 Param)
{
    AMBA_IMG_MAIN_CMD_MSG_s CmdMsg;

    CmdMsg.Ctx.Data = Param;

    if (VinId < AMBA_IMG_NUM_VIN_CHANNEL) {
        switch (CmdMsg.Ctx.Bits.Cmd) {
            case (UINT8) IMAGE_CMD_CHAN:
                switch (CmdMsg.Ctx.Bits.Msg) {
                    case (UINT8) CHAN_MSG_AVM:
                        AmbaImgPrint2(PRINT_FLAG_DBG, "chan-msg-avm", VinId, (UINT32) CmdMsg.Ctx.ImageChanAvm.Msg);
                        AmbaImgPrint2(PRINT_FLAG_DBG, "chain id", VinId, CmdMsg.Ctx.ImageChanAvm.ChainId);
                        AmbaImgPrint2(PRINT_FLAG_DBG, "algo id", VinId, CmdMsg.Ctx.ImageChanAvm.AlgoId);
                        AmbaImgPrint2(PRINT_FLAG_DBG, "zone id", VinId, CmdMsg.Ctx.ImageChanAvm.ZoneId);
                        AmbaImgMainAvm_ImageChanId.Ctx.Bits.VinId = (UINT8) VinId;
                        AmbaImgMainAvm_ImageChanId.Ctx.Bits.ChainId = CmdMsg.Ctx.ImageChanAvm.ChainId;
                        AmbaImgMainAvm_ImageChanId.Ctx.Bits.AlgoId = CmdMsg.Ctx.ImageChanAvm.AlgoId;
                        AmbaImgMainAvm_ImageChanId.Ctx.Bits.ZoneId = CmdMsg.Ctx.ImageChanAvm.ZoneId;
                        break;
                    case (UINT8) CHAN_MSG_AVM_IQ:
                        AmbaImgPrint2(PRINT_FLAG_DBG, "chan-msg-avm-iq", VinId, (UINT32) CmdMsg.Ctx.ImageChanIq.Msg);
                        AmbaImgPrint2(PRINT_FLAG_DBG, "chain id", VinId, CmdMsg.Ctx.ImageChanIq.ChainId);
                        AmbaImgPrint2(PRINT_FLAG_DBG, "hdr mode", VinId, CmdMsg.Ctx.ImageChanIq.HdrId);
                        AmbaImgPrint2(PRINT_FLAG_DBG, "iq table", VinId, CmdMsg.Ctx.ImageChanIq.Id);
                        AmbaImgMainAvm_ImgHdr = (UINT32) CmdMsg.Ctx.ImageChanIq.HdrId;
                        AmbaImgMainAvm_ImgIq = (UINT32) CmdMsg.Ctx.ImageChanIq.Id;
                        break;
                    case (UINT8) CHAN_MSG_AVM_CORE:
                        AmbaImgPrint(PRINT_FLAG_DBG, "chan-msg-avm-core", (UINT32) CmdMsg.Ctx.ImageChanCore.Msg);
                        AmbaImgPrint(PRINT_FLAG_DBG, "chain id", CmdMsg.Ctx.ImageChanCore.ChainId);
                        AmbaImgPrint(PRINT_FLAG_DBG, "core id", CmdMsg.Ctx.ImageChanCore.Id);
                        AmbaImgMainAvm_ImgCore = CmdMsg.Ctx.ImageChanCore.Id;
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

/**
 *  Amba image main avm algorithm dispatch
 *  @param[in] VinId vin id
 *  @param[in] Param command and message
 *  @note this function is intended for internal use only
 */
static void AmbaImgMainAvm_AlgoDispatch(UINT32 VinId, UINT64 Param)
{
    UINT32 FuncRetCode;
    UINT32 AlgoId;
    UINT32 PipeMode = (UINT32) IMAGE_PIPE_NORMAL;
    UINT32 i;

    UINT16 ImgIq;

    static UINT32 AmbaImgMainAvm_AlgoMode = 1U;

    AMBA_IMG_MAIN_CMD_MSG_s CmdMsg;

    CmdMsg.Ctx.Data = Param;

    if (VinId < AMBA_IMG_NUM_VIN_CHANNEL) {
        switch (CmdMsg.Ctx.Bits.Cmd) {
            case (UINT8) AVM_CMD_START:
                /* avm iq table */
                if (AmbaImgMainAvm_ImageChanId.Ctx.Bits.ZoneId > 0U) {
                    /* algo id get */
                    AlgoId = AmbaImgMainAvm_ImageChanId.Ctx.Bits.AlgoId;
                    /* algo to image channel */
                    AmbaImgMain_Algo2ChanTable[AlgoId].Ctx.Data = AmbaImgMainAvm_ImageChanId.Ctx.Data;
                    /* pipe mode */
                    for (i = 0U; i < AMBA_IMG_NUM_FOV_CHANNEL; i++) {
                        if ((AmbaImgMainAvm_ImageChanId.Ctx.Bits.ZoneId & (((UINT32) 1U) << i)) > 0U) {
                            /* pipe mode get */
                            FuncRetCode = AmbaImgChannel_PipeModeGet(VinId, AmbaImgMainAvm_ImageChanId.Ctx.Bits.ChainId, i, &PipeMode);
                            if (FuncRetCode != OK_UL) {
                                /* pipe mode fail */
                                PipeMode = (UINT32) IMAGE_PIPE_NORMAL;
                            }
                            break;
                        }
                    }
                    AmbaImgPrint2(PRINT_FLAG_MSG, "avm algo pipe mode", AlgoId, PipeMode);
                    /* pipe mode set */
                    if (PipeMode == (UINT32) IMAGE_PIPE_DRAM_EFCY) {
                        FuncRetCode = AmbaImgProc_SetPipelineMode(AlgoId, 4U/*dram efficiency*/);
                        if (FuncRetCode != OK_UL) {
                            /* */
                        }
                    } else {
                        FuncRetCode = AmbaImgProc_SetPipelineMode(AlgoId, 1U/*normal*/);
                        if (FuncRetCode != OK_UL) {
                            /* */
                        }
                    }
                    /* hdr mode */
                    AmbaImgPrint2(PRINT_FLAG_MSG, "avm algo hdr mode", AlgoId, AmbaImgMainAvm_ImgHdr);
                    FuncRetCode = AmbaImgProc_HDRSetVideoMode(AlgoId, AmbaImgMainAvm_ImgHdr);
                    if (FuncRetCode != OK) {
                        AmbaImgPrintStr(PRINT_FLAG_ERR, "error: image main avm hdr modr init");
                        AmbaImgPrint_Flush();
                    }
                    /* linear ce? */
                    if (AmbaImgSystem_Info[AmbaImgMainAvm_ImageChanId.Ctx.Bits.VinId][AmbaImgMainAvm_ImageChanId.Ctx.Bits.ChainId].Pipe.Hdr.Bits.Enable == 0U) {
                        /* non-hdr */
                        AmbaImgPrint2(PRINT_FLAG_MSG, "algo lce", AlgoId, AmbaImgSystem_Info[AmbaImgMainAvm_ImageChanId.Ctx.Bits.VinId][AmbaImgMainAvm_ImageChanId.Ctx.Bits.ChainId].Pipe.Hdr.Bits.LCeEnable);
                        if (AmbaImgSystem_Info[AmbaImgMainAvm_ImageChanId.Ctx.Bits.VinId][AmbaImgMainAvm_ImageChanId.Ctx.Bits.ChainId].Pipe.Hdr.Bits.LCeEnable > 0U) {
                            FuncRetCode = AmbaImgProc_HDRSetLinearCEEnable(AlgoId, 1U);
                            if (FuncRetCode != OK_UL) {
                                /* */
                            }
                        } else {
                            FuncRetCode = AmbaImgProc_HDRSetLinearCEEnable(AlgoId, 0U);
                            if (FuncRetCode != OK_UL) {
                                /* */
                            }
                        }
                    }
                    /* iq table */
                    AmbaImgPrint2(PRINT_FLAG_MSG, "load avm iq table", AlgoId, AmbaImgMainAvm_ImgIq);
                    ImgIq = (UINT16) (AmbaImgMainAvm_ImgIq & 0xFFFFU);
                    FuncRetCode = App_Image_Init_Iq_Params(AlgoId, (INT32) ImgIq);
                    if (FuncRetCode != OK) {
                        AmbaImgPrintStr(PRINT_FLAG_ERR, "error: image main avm iq param init");
                        AmbaImgPrint_Flush();
                    }
                    AmbaImgPrint2(PRINT_FLAG_MSG, "load avm iq table end", AlgoId, AmbaImgMainAvm_ImgIq);
                }
                /* avm algo enable */
                FuncRetCode = AmbaImgProc_MVINSetState((UINT8) (AmbaImgMainAvm_AlgoMode & 0xFFU)/*1:Avm,2:eMirror*/);
                if (FuncRetCode == OK) {
                    UINT8 AmbaImgMainAvm_ViewChk[16] = {
                        1U, 1U, 1U, 1U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U };
                    UINT8 AmbaImgMainAvm_RunChk[16] = {
                        1U, 1U, 1U, 1U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U };
                    INT32 AmbaImgMainAvm_Roi2StatIdx[16] = {
                         0,  1,  2,  3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 };

                    if (AmbaImgMainAvm_AlgoMode == 2U) {
                        /* e-mirror: 3-fov */
                        AmbaImgMainAvm_ViewChk[3] = 0U;
                        AmbaImgMainAvm_RunChk[3] = 0U;
                        AmbaImgMainAvm_Roi2StatIdx[3] = 0;
                    }

                    FuncRetCode = AmbaImgProc_MVINSetVinViewChk(AmbaImgMainAvm_ViewChk);
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                    FuncRetCode = AmbaImgProc_MVINSetRunChk(AmbaImgMainAvm_RunChk);
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                    FuncRetCode = AmbaImgProc_SetStitchingIdxToViewID(AmbaImgMainAvm_Roi2StatIdx);
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                }
                break;
            case (UINT8) IMAGE_CMD_ALGO:
                switch (CmdMsg.Ctx.Bits.Msg) {
                    case (UINT8) ALGO_MSG_AVM_MODE:
                        AmbaImgPrint(PRINT_FLAG_DBG, "avm mode", (UINT32) CmdMsg.Ctx.Bits.Var);
                        AmbaImgMainAvm_AlgoMode = CmdMsg.Ctx.Bits.Var & 0xFFFFU;
                        AmbaImgMainAvm_Reversed = CmdMsg.Ctx.Bits.Var >> 16U;
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

/**
 *  Amba image main avm algorithm dispatch ex
 *  @param[in] VinId vin id
 *  @param[in] Param command and message
 *  @param[in] Param2 aux data
 *  @note this function is intended for internal use only
 */
static void AmbaImgMainAvm_AlgoDispatchEx(UINT32 VinId, UINT64 Param, UINT64 Param2)
{
    UINT32 FuncRetCode;

    AVM_MEM_INFO_s MemInfo;

    AMBA_IMG_MAIN_CMD_MSG_s CmdMsg;
    AMBA_IMG_MAIN_AUX_DATA_s AuxData;

    CmdMsg.Ctx.Data = Param;
    AuxData.Ctx.Data = Param2;

    if (VinId < AMBA_IMG_NUM_VIN_CHANNEL) {
        switch (CmdMsg.Ctx.Bits.Cmd) {
            case (UINT8) IMAGE_CMD_ALGO:
                switch (CmdMsg.Ctx.Bits.Msg) {
                    case (UINT8) ALGO_MSG_AVM_ROI:
                        MemInfo.Ctx.Data = (AMBA_IMG_MAIN_MEM_ADDR) AuxData.Ctx.Bits.Var;
                        AmbaImgPrintEx(PRINT_FLAG_CYCLE, "avm roi", (UINT32) MemInfo.Ctx.Data, 16U);
                        FuncRetCode = AmbaImgMainAvm_RoiRgbHandler(MemInfo.Ctx.pRoiRgbStat);
                        if (FuncRetCode != OK_UL) {
                            /* */
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

/**
 *  Amba image main avm task dispatch
 *  @param[in] VinId vin id
 *  @param[in] Param command and message
 *  @note this function is intended for internal use only
 */
static void AmbaImgMainAvm_TaskDispatch(UINT32 VinId, UINT64 Param)
{
    UINT32 FuncRetCode;

    AMBA_IMG_MAIN_CMD_MSG_s CmdMsg;

    CmdMsg.Ctx.Data = Param;

    if (VinId < AMBA_IMG_NUM_VIN_CHANNEL) {
        switch (CmdMsg.Ctx.Bits.Cmd) {
            case (UINT8) AVM_CMD_START:
                if (AmbaImgMainAvm_ImageChanId.Ctx.Bits.ZoneId > 0U) {
                    /* avm task create */
                    FuncRetCode = AmbaImgMainAvm_Create(22U, 0x01U);
                    if (FuncRetCode == OK_UL) {
                        /* avm task idle wait */
                        FuncRetCode = AmbaImgMainAvm_IdleWait();
                        if (FuncRetCode == OK_UL) {
                            /* avm task active */
                            FuncRetCode = AmbaImgMainAvm_Active();
                            if (FuncRetCode != OK_UL) {
                                AmbaImgPrintStr(PRINT_FLAG_ERR, "error: avm task active");
                            }
                        } else {
                            AmbaImgPrintStr(PRINT_FLAG_ERR, "error: avm task ready");
                        }
                    } else {
                        AmbaImgPrintStr(PRINT_FLAG_ERR, "error: avm task create");
                    }
                }
                break;
            case (UINT8) AVM_CMD_STOP:
                if (AmbaImgMainAvm_ImageChanId.Ctx.Bits.ZoneId > 0U) {
                    /* avm task inactive */
                    FuncRetCode = AmbaImgMainAvm_Inactive();
                    if (FuncRetCode == OK_UL) {
                        /* statistics push */
                        FuncRetCode = AmbaImgStatistics_Push(AmbaImgMainAvm_ImageChanId.Ctx.Bits.VinId);
                        if (FuncRetCode != OK_UL) {
                            AmbaImgPrintStr(PRINT_FLAG_ERR, "error: statistics push");
                        }
                        /* avm task idle wait */
                        FuncRetCode = AmbaImgMainAvm_IdleWait();
                        if (FuncRetCode != OK_UL) {
                            AmbaImgPrintStr(PRINT_FLAG_ERR, "error: avm task idle");
                        }
                    } else {
                        AmbaImgPrintStr(PRINT_FLAG_ERR, "error: avm task inactive");
                    }
                    /* avm task delete */
                    FuncRetCode = AmbaImgMainAvm_Delete();
                    if (FuncRetCode != OK_UL) {
                        AmbaImgPrintStr(PRINT_FLAG_ERR, "error: avm task delete");
                    }
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
 *  Amba image main avm dispatch
 *  @param[in] VinId vin id
 *  @param[in] Param command and message
 *  @note this function is intended for internal use only
 */
void AmbaImgMainAvm_Dispatch(UINT32 VinId, UINT64 Param)
{
    /* image framework */
    AmbaImgMainAvm_ImgFrwDispatch(VinId, Param);
    /* algorithm */
    AmbaImgMainAvm_AlgoDispatch(VinId, Param);
    /* task */
    AmbaImgMainAvm_TaskDispatch(VinId, Param);
}

/**
 *  @private
 *  Amba image main avm dispatch ex
 *  @param[in] VinId vin id
 *  @param[in] Param command and message
 *  @param[in] Param2 aux data
 *  @note this function is intended for internal use only
 */
void AmbaImgMainAvm_DispatchEx(UINT32 VinId, UINT64 Param, UINT64 Param2)
{
    /* algorithm */
    AmbaImgMainAvm_AlgoDispatchEx(VinId, Param, Param2);
}

/**
 *  Amba image main avm create
 *  @param[in] Priority task priority id
 *  @param[in] CoreInclusion cpu core inclusion (bits)
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgMainAvm_Create(UINT32 Priority, UINT32 CoreInclusion)
{
#define AMBA_IMG_MAIN_AVM_TASK_STACK_SIZE    0xA000
    static UINT8 GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE AmbaImgMainAvm_TaskStack[AMBA_IMG_MAIN_AVM_TASK_STACK_SIZE];
    static char AmbaImgMainAvm_TaskName[] = "ImageAvm";

    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    /* semaphore */
    FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaImgMainAvm_Ready, NULL, 0U);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaImgMainAvm_Go, NULL, 0);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaImgMainAvm_Ack, NULL, 0);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    /* task */
    if (RetCode == OK) {
        AmbaImgPrintStr(PRINT_FLAG_MSG, "image create avm task");
        {
            AVM_MEM_INFO_s MemInfo;
            MemInfo.Ctx.Data = 0U;
            FuncRetCode = AmbaKAL_TaskCreate(&AmbaImgMainAvmTaskId,
                                             AmbaImgMainAvm_TaskName,
                                             Priority,
                                             AmbaImgMainAvm_TaskEx,
                                             MemInfo.Ctx.pVoid,
                                             AmbaImgMainAvm_TaskStack,
                                             AMBA_IMG_MAIN_AVM_TASK_STACK_SIZE,
                                             AMBA_KAL_DONT_START);
        }
        if (FuncRetCode == OK) {
            UINT32 AvmCoreInclusion = CoreInclusion;
            if (AmbaImgMainAvm_ImgCore > 0U) {
                AvmCoreInclusion = AmbaImgMainAvm_ImgCore;
            }
            FuncRetCode = AmbaKAL_TaskSetSmpAffinity(&AmbaImgMainAvmTaskId, AvmCoreInclusion);
            if (FuncRetCode != KAL_ERR_NONE) {
                RetCode = NG_UL;
            }
            FuncRetCode = AmbaKAL_TaskResume(&AmbaImgMainAvmTaskId);
            if (FuncRetCode != KAL_ERR_NONE) {
                RetCode = NG_UL;
            }
        } else {
            RetCode = NG_UL;
        }
    }

    return RetCode;
}

/**
 *  Amba image main avm delete
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgMainAvm_Delete(void)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    /* task */
    FuncRetCode = AmbaKAL_TaskTerminate(&AmbaImgMainAvmTaskId);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    FuncRetCode = AmbaKAL_TaskDelete(&AmbaImgMainAvmTaskId);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    /* semaphore */
    FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaImgMainAvm_Ready);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaImgMainAvm_Go);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaImgMainAvm_Ack);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image main avm algo id get
 *  @param[out] pAlgoId pointer to the avm algorithm id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainAvm_AlgoIdGet(UINT32 *pAlgoId)
{
    UINT32 RetCode = OK_UL;

    *pAlgoId = AmbaImgMainAvm_ImageChanId.Ctx.Bits.AlgoId;

    return RetCode;
}

/**
 *  @private
 *  Amba image main avm init
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainAvm_Init(void)
{
    UINT32 RetCode = OK_UL;

    AmbaImgMainAvm_DummyFlag = 1U;
    AmbaImgMainAvm_ImageChanId.Ctx.Data = 0ULL;
    /* avm debug count reset */
    if (AmbaImgMain_DebugCount == 0U) {
        AmbaImgMainAvm_DebugCount = 0xFFFFFFFFU;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image main avm active
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgMainAvm_Active(void)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    if (AmbaImgMainAvm_Cmd == (UINT32) AVM_TASK_CMD_STOP) {
        FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaImgMainAvm_Ready, AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            /* cmd start */
            AmbaImgMainAvm_Cmd = (UINT32) AVM_TASK_CMD_START;
            /* task go */
            FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaImgMainAvm_Go);
            if (FuncRetCode == KAL_ERR_NONE) {
                /* ack take */
                FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaImgMainAvm_Ack, AMBA_KAL_WAIT_FOREVER);
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

    return RetCode;
}

/**
 *  Amba image main avm inactive
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgMainAvm_Inactive(void)
{
    if (AmbaImgMainAvm_Cmd == (UINT32) AVM_TASK_CMD_START) {
        AmbaImgMainAvm_Cmd = (UINT32) AVM_TASK_CMD_STOP;
    }

    return OK_UL;
}

/**
 *  Amba image main avm idle wait
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgMainAvm_IdleWait(void)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaImgMainAvm_Ready, AMBA_KAL_WAIT_FOREVER);
    if (FuncRetCode == KAL_ERR_NONE) {
        FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaImgMainAvm_Ready);
        if (FuncRetCode != KAL_ERR_NONE) {
            RetCode = NG_UL;
        }
    } else {
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba image main avm aaa algorithm id get
 *  @param[in] ImageChanId image channel id
 *  @return algorithm id
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgMainAvm_AaaAlgoIdGet(AMBA_IMG_CHANNEL_ID_s ImageChanId)
{
    UINT32 i;

    UINT32 VrId;
    UINT32 VrAltId;

    UINT32 mVinId = 0U;
    UINT32 mChainId = 0U;

    UINT32 mAlgoId;

    const AMBA_IMG_CHANNEL_VR_MAP_s *pVrMap;
    AMBA_IMG_CHANNEL_ID_s ImageChanId2;

    /* image channel id */
    ImageChanId2.Ctx.Data = pAmbaImgMainChannel[ImageChanId.Ctx.Bits.VinId][ImageChanId.Ctx.Bits.ChainId].pCtx->Id.Ctx.Data;
    /* vr map get */
    pVrMap = &(pAmbaImgMainChannel[ImageChanId.Ctx.Bits.VinId][ImageChanId.Ctx.Bits.ChainId].pCtx->VrMap);
    /* vr? */
    if (pVrMap->Id.Ctx.Bits.VrId > 0U) {
        /* vr id */
        VrId = pVrMap->Id.Ctx.Bits.VrId;
        /* vr alt id */
        VrAltId = pVrMap->Id.Ctx.Bits.VrAltId;
        /* vr master? */
        if (((VrId & (((UINT32) 1U) << ImageChanId2.Ctx.Bits.VinId)) > 0U) &&
            ((VrAltId & (((UINT32) 1U) << ImageChanId2.Ctx.Bits.ChainId)) > 0U)) {
            /* algo id (vr) */
            mAlgoId = ImageChanId2.Ctx.Bits.AlgoId;
        } else {
            /* slave */
            for (i = 0U; i < AMBA_IMG_NUM_VIN_CHANNEL; i++) {
                if ((VrId & (((UINT32) 1U) << i)) > 0U) {
                    /* master vin id */
                    mVinId = i;
                    break;
                }
            }
            for (i = 0U; i < AMBA_IMG_NUM_VIN_ALGO; i++) {
                if ((VrAltId & (((UINT32) 1U) << i)) > 0U) {
                    /* master chain id */
                    mChainId = i;
                    break;
                }
            }
            /* algo id (vr) */
            mAlgoId = pAmbaImgMainChannel[mVinId][mChainId].pCtx->Id.Ctx.Bits.AlgoId;
        }
    } else {
        /* algo id (non-vr) */
        mAlgoId = ImageChanId2.Ctx.Bits.AlgoId;
    }

    return mAlgoId;
}

/**
 *  Amba image main avm statistics processing
 *  @param[in] pPort pointer to the statistics port data
 *  @note this function is intended for internal use only
 */
static void AmbaImgMainAvm_StatProc(const AMBA_IMG_STATISTICS_PORT_s *pPort)
{
    UINT32 FuncRetCode;
    UINT32 i, j;

    UINT32 AlgoId = pPort->ImageChanId.Ctx.Bits.AlgoId;
    UINT32 StatZoneId = pPort->ZoneId.Req;

    UINT32 StatCnt = 0U;

    UINT32 TileRow;
    UINT32 TileCol;

    AVM_MEM_INFO_s MemInfo;
    AMBA_IP_GLOBAL_AAA_DATA_s *pAaaData = NULL;

    INT32 Weighting[MAX_VIEW_NUM];
    UINT32 WeightingU;

    /* algo aaa buffer get */
    FuncRetCode = AmbaImgProc_GAAAGetStat(AlgoId, &pAaaData);
    if (FuncRetCode == OK) {
        /* algo aaa weighting get */
        FuncRetCode = AmbaImgProc_GAAAGetWeighting(AlgoId, Weighting);
        if (FuncRetCode != OK_UL) {
            /* */
        }

        /* cfa histo reset */
        for (i = 0U; i < AMBA_IK_CFA_HISTO_COUNT; i++) {
            pAaaData->CfaHist.HisBinY[i] = 0U;
            pAaaData->CfaHist.HisBinR[i] = 0U;
            pAaaData->CfaHist.HisBinG[i] = 0U;
            pAaaData->CfaHist.HisBinB[i] = 0U;
        }

        /* rgb histo reset */
        for (i = 0U; i < AMBA_IK_PG_HISTO_COUNT; i++) {
            pAaaData->PgHist.HisBinY[i] = 0U;
            pAaaData->PgHist.HisBinR[i] = 0U;
            pAaaData->PgHist.HisBinG[i] = 0U;
            pAaaData->PgHist.HisBinB[i] = 0U;
        }

        for (i = 0U; (i < AMBA_IMG_NUM_FOV_CHANNEL) && ((StatZoneId >> i) > 0U); i++) {
            if ((StatZoneId & (((UINT32) 1U) << i)) > 0U) {
                /* cfa get */
                MemInfo.Ctx.pCvoid = pPort->Data.pCfa[i];

                /* cfa header */
                if (StatCnt == 0U) {
                    FuncRetCode = AmbaWrap_memcpy(&(pAaaData->Header), &(MemInfo.Ctx.pCfa->Header), sizeof(AMBA_IK_3A_HEADER_s));
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                }

                /* cfa ae */
                TileRow = MemInfo.Ctx.pCfa->AAA_HEADER_CFA_AE.AeTileNumRow;
                TileCol = MemInfo.Ctx.pCfa->AAA_HEADER_CFA_AE.AeTileNumCol;

                FuncRetCode = AmbaWrap_memcpy(&(pAaaData->Ae[TileRow*TileCol*StatCnt]), MemInfo.Ctx.pCfa->Ae, sizeof(AMBA_IK_CFA_AE_s)*TileRow*TileCol);
                if (FuncRetCode != OK_UL) {
                    /* */
                }

                /* cfa awb */
                TileRow = MemInfo.Ctx.pCfa->AAA_HEADER_CFA_AWB.AwbTileNumRow;
                TileCol = MemInfo.Ctx.pCfa->AAA_HEADER_CFA_AWB.AwbTileNumCol;

                FuncRetCode = AmbaWrap_memcpy(&(pAaaData->Awb[TileRow*TileCol*StatCnt]), MemInfo.Ctx.pCfa->Awb, sizeof(AMBA_IK_CFA_AWB_s)*TileRow*TileCol);
                if (FuncRetCode != OK_UL) {
                    /* */
                }

                /* weighting*/
                WeightingU = (Weighting[StatCnt] > 0) ? (UINT32) Weighting[StatCnt] : 0U;

                /* cfa histo */
                for (j = 0U; j < AMBA_IK_CFA_HISTO_COUNT; j++) {
                    pAaaData->CfaHist.HisBinY[j] += (MemInfo.Ctx.pCfa->Histogram.HisBinY[j]*WeightingU);
                    pAaaData->CfaHist.HisBinR[j] += (MemInfo.Ctx.pCfa->Histogram.HisBinR[j]*WeightingU);
                    pAaaData->CfaHist.HisBinG[j] += (MemInfo.Ctx.pCfa->Histogram.HisBinG[j]*WeightingU);
                    pAaaData->CfaHist.HisBinB[j] += (MemInfo.Ctx.pCfa->Histogram.HisBinB[j]*WeightingU);
                }

                /* rgb get */
                MemInfo.Ctx.pCvoid = pPort->Data.pRgb[i];

                /* rgb histo */
                for (j = 0U; j < AMBA_IK_PG_HISTO_COUNT; j++) {
                    pAaaData->PgHist.HisBinY[j] += (MemInfo.Ctx.pRgb->Histogram.HisBinY[j]*WeightingU);
                    pAaaData->PgHist.HisBinR[j] += (MemInfo.Ctx.pRgb->Histogram.HisBinR[j]*WeightingU);
                    pAaaData->PgHist.HisBinG[j] += (MemInfo.Ctx.pRgb->Histogram.HisBinG[j]*WeightingU);
                    pAaaData->PgHist.HisBinB[j] += (MemInfo.Ctx.pRgb->Histogram.HisBinB[j]*WeightingU);
                }

                /* stat algo id put */
                pAaaData->StatisticMap[StatCnt] = AlgoId;

                /* count update */
                StatCnt++;
            }
        }

        /* stat count */
        pAaaData->StatCount = StatCnt;

        /* debug only */
        if (AmbaImgMainAvm_DebugCount < AmbaImgMain_DebugCount) {
            AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
            AmbaImgPrint2(PRINT_FLAG_DBG, "avm algo id", pPort->ImageChanId.Ctx.Bits.VinId, AlgoId);
            MemInfo.Ctx.pAaaData = pAaaData;
            AmbaImgPrintEx2(PRINT_FLAG_DBG, "algo aaa buffer", pPort->ImageChanId.Ctx.Bits.VinId, 10U, (UINT32) MemInfo.Ctx.Data, 16U);
            j = 0U;
            for (i = 0U; (i < AMBA_IMG_NUM_FOV_CHANNEL) && ((StatZoneId >> i) > 0U); i++) {
                if ((StatZoneId & (((UINT32) 1U) << i)) > 0U) {
                    AmbaImgPrint2(PRINT_FLAG_DBG, "zone 2 algo", i, AlgoId);
                    AmbaImgPrint2(PRINT_FLAG_DBG, "weighting", j, (Weighting[j] > 0) ? (UINT32) Weighting[j] : 0U);
                    j++;
                }
            }
            AmbaImgPrint2(PRINT_FLAG_DBG, "stat count", pPort->ImageChanId.Ctx.Bits.VinId, pAaaData->StatCount);
            AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
        }
    }
}

/**
 *  Amba image main avm task ex
 *  @param[in] pParam pointer to parameter
 *  @note this function is intended for internal use only
 */
static void *AmbaImgMainAvm_TaskEx(void *pParam)
{
    AVM_MEM_INFO_s MemInfo;

    MemInfo.Ctx.pVoid = pParam;
    AmbaImgMainAvm_Task((UINT32) MemInfo.Ctx.Data);

    return NULL;
}

/**
 *  Amba image main avm task
 *  @param[in] Param vin id with the chain id
 *  @note this function is intended for internal use only
 */
static void AmbaImgMainAvm_Task(UINT32 Param)
{
    UINT8 FirstActive = 1U;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 ExclusiveZoneId;
    UINT32 InclusiveZoneId = 0U;

    //AVM_MEM_INFO_s MemInfo;

    AMBA_IMG_CHANNEL_ID_s ImageChanId;
    AMBA_IMG_CHANNEL_ID_s ImageChanId2;

    UINT32 AlgoId = 0U;
    UINT32 ExposureNum = 1U;

    UINT32 AaaAlgoId = 0U;
    AMBA_AE_INFO_s AeInfo[AMBA_IMG_SENSOR_HAL_HDR_SIZE];
    AMBA_IK_WB_GAIN_s WbGain[AMBA_IMG_SENSOR_HAL_HDR_SIZE] = {0};

    AMBA_IK_MODE_CFG_s *pImgMode;
    AMBA_IK_MODE_CFG_s ImgMode;

    static AMBA_IMG_STATISTICS_PORT_s AmbaImgMainAvm_StatPort = {0};
    AMBA_IMG_STATISTICS_PORT_s *pPort;

    AMBA_AAA_OP_INFO_s AaaOpInfo;

    UINT32 Mode = AMBA_IK_PIPE_VIDEO;
    UINT32 j;
    UINT32 FineGgc[3];
    UINT32 FineDgc[3];
    UINT32 FineWgc[3];
    UINT32 LastFineGgc[AMBA_IMG_MAIN_AVM_MAX_FOV_NUM];
    UINT32 LastFineDgc[AMBA_IMG_MAIN_AVM_MAX_FOV_NUM];
    UINT32 LastFineWgc[AMBA_IMG_MAIN_AVM_MAX_FOV_NUM][3];

    UINT32 WbId = (UINT32) AMBA_IMG_MAIN_AVM_WBID;

    /* param init */
    ImageChanId.Ctx.Data = 0ULL;

    /* info reset */
    FuncRetCode = AmbaWrap_memset(LastFineGgc, 0, sizeof(LastFineGgc));
    if (FuncRetCode != OK_UL) {
        /* */
    }
    FuncRetCode = AmbaWrap_memset(LastFineDgc, 0, sizeof(LastFineDgc));
    if (FuncRetCode != OK_UL) {
        /* */
    }
    FuncRetCode = AmbaWrap_memset(LastFineWgc, 0, sizeof(LastFineWgc));
    if (FuncRetCode != OK_UL) {
        /* */
    }

    pPort = &AmbaImgMainAvm_StatPort;

    AmbaImgMainAvm_Cmd = (UINT32) AVM_TASK_CMD_STOP;

    (void) Param;

    while (AmbaImgMainAvm_DummyFlag == 1U) {
        if (AmbaImgMainAvm_Cmd == (UINT32) AVM_TASK_CMD_STOP) {
            if (FirstActive == 1U) {
                /* task first time create */
                FirstActive = 0U;
            } else {
                /* avm adj table set */
                Amba_Set_AdjTableNo(AlgoId, -1);

                /* fine wb gain enable */
                FuncRetCode = AmbaImgSystem_WbGainFine(ImageChanId, Mode | (WbId << 16U), 0U);
                if (FuncRetCode != OK_UL) {
                    AmbaImgPrintStr(PRINT_FLAG_ERR, "error: avm task wbgain fine disable");
                }

                /* statistics close */
                FuncRetCode = AmbaImgStatistics_Close(pPort);
                if (FuncRetCode != OK_UL) {
                    AmbaImgPrint(PRINT_FLAG_ERR, "error: avm statistics close", FuncRetCode);
                }
            }
            /* ready give */
            FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaImgMainAvm_Ready);
            if (FuncRetCode != KAL_ERR_NONE) {
                AmbaImgPrintStr(PRINT_FLAG_ERR, "error: avm task idle");
            }
            /* go take */
            FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaImgMainAvm_Go, AMBA_KAL_WAIT_FOREVER);
            if (FuncRetCode != KAL_ERR_NONE) {
                AmbaImgPrintStr(PRINT_FLAG_ERR, "error: avm task go");
            }

            /* image channel get */
            ImageChanId.Ctx.Data = AmbaImgMainAvm_ImageChanId.Ctx.Data;
            /* algo id get */
            AlgoId = ImageChanId.Ctx.Bits.AlgoId;
            /* aaa algo id */
            AaaAlgoId = AmbaImgMainAvm_AaaAlgoIdGet(ImageChanId);

            /* exposure number get */
            ExposureNum = (UINT32) AmbaImgSystem_Info[ImageChanId.Ctx.Bits.VinId][ImageChanId.Ctx.Bits.ChainId].Pipe.Hdr.Bits.ExposureNum;

            AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
            AmbaImgPrint2(PRINT_FLAG_DBG, "exposure num", ImageChanId.Ctx.Bits.VinId, ExposureNum);

            /* avm ae init */
            AmbaImgPrint2(PRINT_FLAG_MSG, "avm ae algo init...", ImageChanId.Ctx.Bits.VinId, AlgoId);
            FuncRetCode = AmbaImgProc_AEInit(AlgoId, 0U);
            if (FuncRetCode != OK) {
                AmbaImgPrint2(PRINT_FLAG_ERR, "error: avm ae algo init", ImageChanId.Ctx.Bits.VinId, AlgoId);
            }
            /* avm awb init */
            AmbaImgPrint2(PRINT_FLAG_MSG, "avm awb algo init...", ImageChanId.Ctx.Bits.VinId, AlgoId);
            FuncRetCode = AmbaImgProc_AWBInit(AlgoId, 0U);
            if (FuncRetCode != OK) {
                AmbaImgPrint2(PRINT_FLAG_ERR, "error: avm awb algo init", ImageChanId.Ctx.Bits.VinId, AlgoId);
            }

            /* avm adj table set */
            Amba_Set_AdjTableNo(AlgoId, 1);

            /* avm adj init */
            AmbaImgPrint2(PRINT_FLAG_MSG, "avm adj algo init...", ImageChanId.Ctx.Bits.VinId, AlgoId);
            FuncRetCode = Amba_AdjInit(AlgoId);
            if (FuncRetCode != OK) {
                AmbaImgPrint2(PRINT_FLAG_ERR, "error: avm adj algo init", ImageChanId.Ctx.Bits.VinId, AlgoId);
            }

            /* global enable for statistics */
            FuncRetCode = AmbaImgProc_GAAASetEnb(AlgoId, 1U);
            if (FuncRetCode != OK) {
                /* */
            }

            AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
            AmbaImgPrint(PRINT_FLAG_DBG, "vin id", ImageChanId.Ctx.Bits.VinId);
            AmbaImgPrint(PRINT_FLAG_DBG, "chain id", ImageChanId.Ctx.Bits.ChainId);
            AmbaImgPrint(PRINT_FLAG_DBG, "algo id", AlgoId);
            AmbaImgPrintEx(PRINT_FLAG_DBG, "zone id", ImageChanId.Ctx.Bits.ZoneId, 16U);
            AmbaImgPrint(PRINT_FLAG_DBG, "aaa algo id", AaaAlgoId);

            /* ae/awb preset */
            for (i = 0U; i < ExposureNum; i++) {
                /* ae */
                FuncRetCode = AmbaImgProc_AEGetExpInfo(AaaAlgoId, i, IP_MODE_VIDEO, &(AeInfo[i]));
                if (FuncRetCode != OK_UL) {
                    /* */
                }
                FuncRetCode = AmbaImgProc_AESetExpInfo(AlgoId, i, IP_MODE_VIDEO, &(AeInfo[i]));
                if (FuncRetCode != OK_UL) {
                    /* */
                }
                /* awb */
                FuncRetCode = AmbaImgProc_AWBGetWBGain(AaaAlgoId, i, IP_MODE_VIDEO, &(WbGain[i]));
                if (FuncRetCode != OK_UL) {
                    /* */
                }
                FuncRetCode = AmbaImgProc_AWBSetWBGain(AlgoId, i, IP_MODE_VIDEO, &(WbGain[i]));
                if (FuncRetCode != OK_UL) {
                    /* */
                }
            }

            /* exclusive zone id*/
            ExclusiveZoneId = 0U;

            /* fov adj */
            for (i = 0U; i <= AmbaImgMainFov_LastIndex; i++) {
                ExclusiveZoneId |= AmbaImgMainFov_ImageChanId[i].Ctx.Bits.ZoneId;
            }

            /* inclusive zone id */
            InclusiveZoneId = ImageChanId.Ctx.Bits.ZoneId & (~ExclusiveZoneId);

            /* adj preset */
            ImageChanId2.Ctx.Data = ImageChanId.Ctx.Data;
            for (i = 0U; i < AMBA_IMG_NUM_FOV_CHANNEL; i++) {
                if ((InclusiveZoneId >> i) == 0U) {
                    break;
                }
                if ((InclusiveZoneId & (((UINT32) 1U) << i)) > 0U) {
                    ImageChanId2.Ctx.Bits.ZoneId = (UINT32) (((UINT32) 1U) << i);
                    FuncRetCode = AmbaImgSystem_ImageModeGet(ImageChanId2, AMBA_IK_PIPE_VIDEO, &pImgMode);
                    if (FuncRetCode == OK_UL) {
                        FuncRetCode = AmbaWrap_memcpy(&ImgMode, pImgMode, sizeof(AMBA_IK_MODE_CFG_s));
                        if (FuncRetCode == OK_UL) {
                            INT32 FuncRetCode_L;
                            ImgMode.ContextId = AmbaImgSystem_ContextIdGet(i);
                            AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
                            AmbaImgPrint2(PRINT_FLAG_DBG, "avm adj context", i, ImgMode.ContextId);
                            AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
                            /* tile settings */
                            FuncRetCode_L = App_Image_Init_Ae_Tile_Config(AlgoId, &ImgMode);
                            if (FuncRetCode_L != 0L) {
                                /* */
                            }
                            FuncRetCode_L = App_Image_Init_Awb_Tile_Config(AlgoId, &ImgMode);
                            if (FuncRetCode_L != 0L) {
                                /* */
                            }
                            FuncRetCode_L = App_Image_Init_Af_Tile_Config(AlgoId, &ImgMode);
                            if (FuncRetCode_L != 0L) {
                                /* */
                            }
                            /* video param set */
#ifndef CONFIG_BUILD_IMGFRW_AAA_ADJ_EXT
                            FuncRetCode = AmbaImgProc_ADJSetVideoParam(AlgoId, &ImgMode);
#else
                            FuncRetCode = Amba_Adj_SetVideoPipeCtrlParams(AlgoId, &ImgMode);
#endif
                            if (FuncRetCode != OK_UL) {
                                /* */
                            }
                        }
                    }
                }
            }
            /* adj reset flags */
#ifndef CONFIG_BUILD_IMGFRW_AAA_ADJ_EXT
            FuncRetCode = AmbaImgProc_ADJResetVideoFlags(AlgoId, 0U);
#else
            FuncRetCode = Amba_Adj_ResetVideoFlags(AlgoId, 0U);
#endif
            if (FuncRetCode != OK_UL) {
                /* */
            }

            /* statistic open */
            FuncRetCode = AmbaImgStatistics_Open(ImageChanId, pPort);
            if (FuncRetCode != OK_UL) {
                AmbaImgPrint(PRINT_FLAG_ERR, "error: avm statistics open", FuncRetCode);
            }

            /* fine wb gain enable */
            FuncRetCode = AmbaImgSystem_WbGainFine(ImageChanId, Mode | (WbId << 16U), 1U);
            if (FuncRetCode != OK_UL) {
                AmbaImgPrintStr(PRINT_FLAG_ERR, "error: avm task wbgain fine enable");
            } else {
                /* fine gain preset */
                FineGgc[0] = 4096U;
                FineDgc[0] = 4096U;
                FineWgc[0] = 4096U;
                FineWgc[1] = 4096U;
                FineWgc[2] = 4096U;

                /* zone find  */
                j = 0U;
                for (i = 0U; i < AMBA_IMG_NUM_FOV_CHANNEL; i++) {
                    if (((UINT32) ImageChanId.Ctx.Bits.ZoneId >> i) == 0U) {
                        break;
                    }
                    if (((UINT32) ImageChanId.Ctx.Bits.ZoneId & (((UINT32) 1U) << i)) > 0U) {
                        /* image channel for single zone */
                        ImageChanId2.Ctx.Bits.ZoneId = (UINT32) (((UINT32) 1U) << i);
                        /* fine gain reset */
                        FuncRetCode = AmbaImgSystem_FineGgcWrite(ImageChanId2, Mode | (WbId << 16U), FineGgc);
                        if (FuncRetCode != OK_UL) {
                            /* */
                        }
                        FuncRetCode = AmbaImgSystem_FineDgcWrite(ImageChanId2, Mode | (WbId << 16U), FineDgc);
                        if (FuncRetCode != OK_UL) {
                            /* */
                        }
                        FuncRetCode = AmbaImgSystem_FineWgcWrite(ImageChanId2, Mode | (WbId << 16U), FineWgc);
                        if (FuncRetCode != OK_UL) {
                            /* */
                        }
                        /* last fine gain update */
                        LastFineGgc[j] = FineGgc[0];
                        LastFineDgc[j] = FineDgc[0];
                        LastFineWgc[j][0] = FineWgc[0];
                        LastFineWgc[j][1] = FineWgc[1];
                        LastFineWgc[j][2] = FineWgc[2];
                        /* index update */
                        j++;
                    }
                }
            }

            /* avm roi reset */
            AmbaImgMainAvm_RoiRgbStat = NULL;

            /* ack */
            FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaImgMainAvm_Ack);
            if (FuncRetCode != KAL_ERR_NONE) {
                AmbaImgPrintStr(PRINT_FLAG_ERR, "error: avm task ack");
            }
        }

        FuncRetCode = AmbaImgStatistics_Request(pPort, REQUEST_WAIT_FOREVER);
        if (FuncRetCode != OK_UL) {
            AmbaImgPrint(PRINT_FLAG_ERR, "error: avm statistics request", FuncRetCode);
        } else {
            /* task stop & statistics null? */
            if ((AmbaImgMainAvm_Cmd == (UINT32) AVM_TASK_CMD_STOP) || (pPort->Data.pCfa == NULL) || (pPort->Data.pRgb == NULL)) {
                continue;
            }

            /* aaa op get */
            FuncRetCode = AmbaImgProc_GetAAAOPInfo(AlgoId, &AaaOpInfo);
            if (FuncRetCode != OK_UL) {
                /* */
            }

            /* op enable? */
            if (AaaOpInfo.AdjOp > 0U) {
                /* stat proc */
                AmbaImgMainAvm_StatProc(pPort);

                /* avm ae/awb set */
                for (i = 0U; i < ExposureNum; i++) {
                    /* awb */
                    FuncRetCode = AmbaImgProc_AEGetExpInfo(AaaAlgoId, i, IP_MODE_VIDEO, &(AeInfo[i]));
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                    FuncRetCode = AmbaImgProc_AESetExpInfo(AlgoId, i, IP_MODE_VIDEO, &(AeInfo[i]));
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                    /* awb */
                    FuncRetCode = AmbaImgProc_AWBGetWBGain(AaaAlgoId, i, IP_MODE_VIDEO, &(WbGain[i]));
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                    FuncRetCode = AmbaImgProc_AWBSetWBGain(AlgoId, i, IP_MODE_VIDEO, &(WbGain[i]));
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                }

                /* hdr? */
                if (ExposureNum > 1U) {
                    /* hdr control */
                    Amba_Adj_VideoHdr_Control(AlgoId, AeInfo, WbGain);
                }

                /* avm adj control */
                Amba_AdjControl(AlgoId);

                /* avm adj set */
                ImageChanId2.Ctx.Data = pPort->ImageChanId.Ctx.Data;
                for (i = 0U; i < AMBA_IMG_NUM_FOV_CHANNEL; i++) {
                    if ((InclusiveZoneId >> i) == 0U) {
                        break;
                    }
                    if ((InclusiveZoneId & (((UINT32) 1U) << i)) > 0U) {
                        ImageChanId2.Ctx.Bits.ZoneId = (UINT32) (((UINT32) 1U) << i);
                        FuncRetCode = AmbaImgSystem_ImageModeGet(ImageChanId2, AMBA_IK_PIPE_VIDEO, &pImgMode);
                        if (FuncRetCode == OK_UL) {
                            FuncRetCode = AmbaWrap_memcpy(&ImgMode, pImgMode, sizeof(AMBA_IK_MODE_CFG_s));
                            if (FuncRetCode == OK_UL) {
                                ImgMode.ContextId = AmbaImgSystem_ContextIdGet(i);
                                /* debug only */
                                if (AmbaImgMainAvm_DebugCount < AmbaImgMain_DebugCount) {
                                    AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
                                    AmbaImgPrint2(PRINT_FLAG_DBG, "avm adj context", i, ImgMode.ContextId);
                                }
#ifndef CONFIG_BUILD_IMGFRW_AAA_ADJ_EXT
                                FuncRetCode = AmbaImgProc_ADJSetVideoParam(AlgoId, &ImgMode);
#else
                                FuncRetCode = Amba_Adj_SetVideoPipeCtrlParams(AlgoId, &ImgMode);
#endif
                                if (FuncRetCode != OK_UL) {
                                    /* */
                                }
                            }
                        }
                    }
                }
                /* adj reset flags */
#ifndef CONFIG_BUILD_IMGFRW_AAA_ADJ_EXT
                FuncRetCode = AmbaImgProc_ADJResetVideoFlags(AlgoId, 0U);
#else
                FuncRetCode = Amba_Adj_ResetVideoFlags(AlgoId, 0U);
#endif
                if (FuncRetCode != OK_UL) {
                    /* */
                }

                /* avm algo */
                if (AmbaImgMainAvm_RoiRgbStat != NULL) {
                    /* zone find  */
                    j = 0U;
                    for (i = 0U; i < AMBA_IMG_NUM_FOV_CHANNEL; i++) {
                        if (((UINT32) ImageChanId.Ctx.Bits.ZoneId >> i) == 0U) {
                            break;
                        }
                        if (((UINT32) ImageChanId.Ctx.Bits.ZoneId & (((UINT32) 1U) << i)) > 0U) {
                            /* roi rgb stat put */
                            FuncRetCode = AmbaImgProc_MVINSetRgbstat(j, &(AmbaImgMainAvm_RoiRgbStat[j]));
                            if (FuncRetCode != OK_UL) {
                                /* */
                            }
                            j++;
                        }
                    }
                    /* avm dgain control */
                    FuncRetCode = AmbaImgProc_MVINAEControlDGain(AlgoId, 0U);
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                }
                {
                    FLOAT VarF;
                    DOUBLE BLDgainY[4];
                    DOUBLE AEDgain[4];
                    AMBA_MVIN_BL_RGB_GAIN_s BLDgainRGB[4];
                    UINT8 UpdateFlag[4];

                    /* image channel get */
                    ImageChanId2.Ctx.Data = pPort->ImageChanId.Ctx.Data;
                    /* zone find  */
                    j = 0U;
                    for (i = 0U; i < AMBA_IMG_NUM_FOV_CHANNEL; i++) {
                        if (((UINT32) ImageChanId.Ctx.Bits.ZoneId >> i) == 0U) {
                            break;
                        }
                        if (((UINT32) ImageChanId.Ctx.Bits.ZoneId & (((UINT32) 1U) << i)) > 0U) {
                            /* image channel for single zone */
                            if (AmbaImgMainAvm_Reversed == 0U) {
                                ImageChanId2.Ctx.Bits.ZoneId = (UINT32) (((UINT32) 1U) << i);
                            } else {
                                ImageChanId2.Ctx.Bits.ZoneId = (UINT32) (((UINT32) 1U) << ((i + 2U) % 3U));
                            }
                            /* dgain get */
                            FuncRetCode = AmbaImgProc_MVINGetBLDgainY(j, &(BLDgainY[j]), &(UpdateFlag[j]));
                            if (FuncRetCode != OK_UL) {
                                /* */
                            }
                            FuncRetCode = AmbaImgProc_MVINGetAEDgain(j, &(AEDgain[j]), &(UpdateFlag[j]));
                            if (FuncRetCode != OK_UL) {
                                /* */
                            }
                            FuncRetCode = AmbaImgProc_MVINGetBLDgainRGB(j, &(BLDgainRGB[j]));
                            if (FuncRetCode != OK_UL) {
                                /* */
                            }
                            /* dgain transfer */
                            VarF = (FLOAT) (BLDgainY[j] * 4096.0f);
                            FineGgc[0] = (UINT32) VarF;
                            VarF = (FLOAT) (AEDgain[j] * 4096.0f);
                            FineDgc[0] = (UINT32) VarF;
                            VarF = (FLOAT) (BLDgainRGB[j].GainR * 4096.0f);
                            FineWgc[0] = (UINT32) VarF;
                            VarF = (FLOAT) (BLDgainRGB[j].GainG * 4096.0f);
                            FineWgc[1] = (UINT32) VarF;
                            VarF = (FLOAT) (BLDgainRGB[j].GainB * 4096.0f);
                            FineWgc[2] = (UINT32) VarF;
                            /* dgain check */
                            if ((LastFineGgc[j] != FineGgc[0]) ||
                                (LastFineDgc[j] != FineDgc[0]) ||
                                (LastFineWgc[j][0] != FineWgc[0]) ||
                                (LastFineWgc[j][1] != FineWgc[1]) ||
                                (LastFineWgc[j][2] != FineWgc[2])) {
                                /* dgain update */
                                FuncRetCode = AmbaImgSystem_FineGgcWrite(ImageChanId2, Mode | (WbId << 16U), FineGgc);
                                if (FuncRetCode != OK_UL) {
                                    /* */
                                }
                                FuncRetCode = AmbaImgSystem_FineDgcWrite(ImageChanId2, Mode | (WbId << 16U), FineDgc);
                                if (FuncRetCode != OK_UL) {
                                    /* */
                                }
                                FuncRetCode = AmbaImgSystem_FineWgcWrite(ImageChanId2, Mode | (WbId << 16U), FineWgc);
                                if (FuncRetCode != OK_UL) {
                                    /* */
                                }
                                /* last fine gain update */
                                LastFineGgc[j] = FineGgc[0];
                                LastFineDgc[j] = FineDgc[0];
                                LastFineWgc[j][0] = FineWgc[0];
                                LastFineWgc[j][1] = FineWgc[1];
                                LastFineWgc[j][2] = FineWgc[2];
                            }
                            /* index update */
                            j++;
                        }
                    }
                }
            }

            /* debug only */
            if (AmbaImgMainAvm_DebugCount < AmbaImgMain_DebugCount) {
                /* zone find  */
                j = 0U;
                for (i = 0U; i < AMBA_IMG_NUM_FOV_CHANNEL; i++) {
                    if (((UINT32) ImageChanId.Ctx.Bits.ZoneId >> i) == 0U) {
                        break;
                    }
                    if (((UINT32) ImageChanId.Ctx.Bits.ZoneId & (((UINT32) 1U) << i)) > 0U) {
                        AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
                        AmbaImgPrintStr(PRINT_FLAG_DBG, "avm");
                        AmbaImgPrint(PRINT_FLAG_DBG, "fine ggc", LastFineGgc[j]);
                        AmbaImgPrint(PRINT_FLAG_DBG, "fine dgc", LastFineDgc[j]);
                        AmbaImgPrint2(PRINT_FLAG_DBG, "fine wgc", 0U, LastFineWgc[j][0]);
                        AmbaImgPrint2(PRINT_FLAG_DBG, "fine wgc", 1U, LastFineWgc[j][1]);
                        AmbaImgPrint2(PRINT_FLAG_DBG, "fine wgc", 2U, LastFineWgc[j][2]);
                        j++;
                    }
                }
                /* count accumulate */
                AmbaImgMainAvm_DebugCount++;
                if (AmbaImgMainAvm_DebugCount == AmbaImgMain_DebugCount) {
                    /* count reset */
                    AmbaImgMainAvm_DebugCount = 0xFFFFFFFFU;
                }
            }
        }
    }
}
