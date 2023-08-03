/**
 *  @file AmbaImgFrwCmdFlow.c
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
 *  @details AmbaShell Image Framework Command Flow
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaVIN_Def.h"
#include "AmbaShell.h"

#include "AmbaDSP_ImageFilter.h"

#include "AmbaImgFramework.h"
#include "AmbaImgMain.h"
//#include "AmbaImgMain_Internal.h"

#include "AmbaImgFrwCmdApp_Def.h"
#include "AmbaImgFrwCmdFlow.h"
#include "AmbaImgFrwCmdFlow_Table.c"

#define OK_UL  ((UINT32) 0U)
#define NG_UL  ((UINT32) 1U)

/**
 *  Amba image framework flow command attach
 *  @return error code
 */
UINT32 AmbaImgFrwCmdFlow_Attach(void)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    static AMBA_IMG_FRW_CMD_s AmbaImgFrwCmdFlow = { AMBSH_MAGIC, "flow", ambsh_imgfrw_flow, NULL };

    FuncRetCode = AmbaImgFrwCmd_Add(&AmbaImgFrwCmdFlow);
    if (FuncRetCode != OK_UL) {
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba shell image framework flow command entry
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_flow(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 CmdIdx = 0U;
    const AMBA_IMG_FRW_CMD_s *List = AmbaImgFrwCmdFlow_List;

    if (Argc > 1U) {
        while ((List != NULL) && (List->Magic == AMBSH_MAGIC)) {
            /* valid */
            if (fvar_strcmp(Argv[1], List->pName) == OK_UL) {
                /* debug msg */
                AmbaImgFrwCmd_Print(F_PRINT_FLAG_DBG, "imgfrw cmd flow idx", CmdIdx);
                /* found */
                List->Proc(Argc-1U, &(Argv[1]), PrintFunc);
                break;
            } else {
                /* next */
                List = List->pNext;
            }
            /* cmd idx */
            CmdIdx++;
        }
    }
}

/**
 *  Amba shell image framework flow command load
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_flow_load(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 i, CmdNum;
    AMBA_IMG_FRW_CMD_s *List = AmbaImgFrwCmdFlow_List;

    (void) Argc;
    (void) Argv;

    if (PrintFunc == NULL) {
        /* */
    }

    /* cmd num get */
    CmdNum = (UINT32) (sizeof(AmbaImgFrwCmdFlow_List)/sizeof(AMBA_IMG_FRW_CMD_s));
    /* debug msg */
    AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, "imgfrw cmd flow load", CmdNum);

    for (i = 0U; i < (CmdNum-1U); i++) {
        if (List[i].Magic != AMBSH_MAGIC) {
            /* invalid */
            break;
        }
        /* link */
        List[i].pNext = &(List[i+1U]);
    }
}

/**
 *  Amba shell image framework flow command vin capture
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_flow_vin_cap(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 4U) {
        UINT32 VinId;
        UINT32 ChainId;
        UINT32 Width;
        UINT32 Height;

        AMBA_IMG_MAIN_CMD_MSG_s CmdMsg;

        VinId = fvar_atou(Argv[1]);
        ChainId = fvar_atou(Argv[2]);
        Width = fvar_atou(Argv[3]);
        Height = fvar_atou(Argv[4]);

        CmdMsg.Ctx.Data = 0ULL;
        CmdMsg.Ctx.VinCap.Width = (UINT16) (Width & 0xFFFFU);
        CmdMsg.Ctx.VinCap.Height = (UINT16) (Height & 0xFFFFU);
        CmdMsg.Ctx.VinCap.ChainId = (UINT8) (ChainId & 0xFFU);
        CmdMsg.Ctx.VinCap.Cmd = (UINT8) VIN_CMD_CAP;
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, "image vin cap", VinId);
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, " chain id", ChainId);
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, " width", Width);
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, " height", Height);
        AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
    } else {
        AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "usage: [vin id][chain id][width][height]");
    }
}

/**
 *  Amba shell image framework flow command vin sensor
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_flow_vin_sensor(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 3U) {
        UINT32 VinId;
        UINT32 SensorIndex;
        UINT32 SensorId;

        AMBA_IMG_MAIN_CMD_MSG_s CmdMsg;

        VinId = fvar_atou(Argv[1]);
        SensorIndex = fvar_atou(Argv[2]);
        SensorId = fvar_atou(Argv[3]);

        CmdMsg.Ctx.Data = 0ULL;
        CmdMsg.Ctx.VinSensor.Id = SensorId;
        CmdMsg.Ctx.VinSensor.Idx = (UINT8) (SensorIndex & 0xFFU);
        CmdMsg.Ctx.VinSensor.Cmd = (UINT8) VIN_CMD_SENSOR;
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, "image chan sensor", VinId);
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, " sensor index", SensorIndex);
        AmbaImgFrwCmd_PrintEx(F_PRINT_FLAG_MSG, " sensor id", SensorId, 16U);
        AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
    } else {
        AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "usage: [vin id][sensor index][sensor id]");
    }
}

/**
 *  Amba shell image framework flow command pipe mode
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_flow_pipe_mode(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 4U) {
        UINT32 VinId;
        UINT32 ChainId;
        UINT32 ZoneIdx;
        UINT32 PipeId;

         UINT8 PipeModeId;

        AMBA_IMG_MAIN_CMD_MSG_s CmdMsg;

        VinId = fvar_atou(Argv[1]);
        ChainId = fvar_atou(Argv[2]);
        ZoneIdx = fvar_atou(Argv[3]);
        PipeId = fvar_atou(Argv[4]);

        CmdMsg.Ctx.Data = 0ULL;
        PipeModeId = (UINT8) (PipeId & 0xFFU);
        CmdMsg.Ctx.PipeMode.Id = PipeModeId;
        CmdMsg.Ctx.PipeMode.ZoneIdx = (UINT8) (ZoneIdx & 0xFFU);
        CmdMsg.Ctx.PipeMode.ChainId = (UINT8) (ChainId & 0xFFU);
        CmdMsg.Ctx.PipeMode.Cmd = (UINT8) PIPE_CMD_MODE;
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, "image pipe mode", VinId);
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, " chain id", ChainId);
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, " zone idx", ZoneIdx);
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, " pipe id", PipeId);
        AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
    } else {
        AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "usage: [vin id][chain id][zone idx][pipe id]");
    }
}

/**
 *  Amba shell image framework flow command pipe output
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_flow_pipe_out(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 4U) {
        UINT32 VinId;
        UINT32 ChainId;
        UINT32 Width;
        UINT32 Height;

        AMBA_IMG_MAIN_CMD_MSG_s CmdMsg;

        VinId = fvar_atou(Argv[1]);
        ChainId = fvar_atou(Argv[2]);
        Width = fvar_atou(Argv[3]);
        Height = fvar_atou(Argv[4]);

        CmdMsg.Ctx.Data = 0ULL;
        CmdMsg.Ctx.PipeOut.Width = (UINT16) (Width & 0xFFFFU);
        CmdMsg.Ctx.PipeOut.Height = (UINT16) (Height & 0xFFFFU);
        CmdMsg.Ctx.PipeOut.ChainId = (UINT8) (ChainId & 0xFFU);
        CmdMsg.Ctx.PipeOut.Cmd = (UINT8) PIPE_CMD_OUT;
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, "image pipe out", VinId);
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, " chain id", ChainId);
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, " width", Width);
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, " height", Height);
        AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
    } else {
        AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "usage: [vin id][chain id][width][height]");
    }
}

/**
 *  Amba shell image framework flow command pipe hdr
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_flow_pipe_hdr(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 5U) {
        UINT32 VinId;
        UINT32 ChainId;
        UINT32 HdrEnable;
        UINT32 ExposureNum;
        UINT32 LCeEnable;

        AMBA_IMG_MAIN_CMD_MSG_s CmdMsg;

        VinId = fvar_atou(Argv[1]);
        ChainId = fvar_atou(Argv[2]);
        HdrEnable = fvar_atou(Argv[3]);
        ExposureNum = fvar_atou(Argv[4]);
        LCeEnable = fvar_atou(Argv[5]);

        CmdMsg.Ctx.Data = 0ULL;
        CmdMsg.Ctx.PipeHdr.Enable = (UINT8) (HdrEnable & 0xFFU);
        CmdMsg.Ctx.PipeHdr.ExposureNum = (UINT8) (ExposureNum & 0xFFU);
        CmdMsg.Ctx.PipeHdr.LCeEnable = (UINT8) (LCeEnable & 0xFFU);
        CmdMsg.Ctx.PipeHdr.ChainId = (UINT8) (ChainId & 0xFFU);
        CmdMsg.Ctx.PipeHdr.Cmd = (UINT8) PIPE_CMD_HDR;
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, "image pipe hdr", VinId);
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, " chain id", ChainId);
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, " hdr enable", HdrEnable);
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, " exposure num", ExposureNum);
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, " linear ce", LCeEnable);
        AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
    } else {
        AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "usage: [vin id][chain id][hdr enable][exposure num][linear ce]");
    }
}

/**
 *  Amba shell image framework flow command channel select
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_flow_chan_select(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 2U) {
        UINT32 VinId;
        UINT32 ChainIndex;

        AMBA_IMG_MAIN_CMD_MSG_s CmdMsg;

        VinId = fvar_atou(Argv[1]);
        ChainIndex = fvar_atou(Argv[2]);

        CmdMsg.Ctx.Data = 0ULL;
        ChainIndex &= 0xFFU;
        CmdMsg.Ctx.ImageChanSelect.ChainIndex = (UINT8) ChainIndex;
        CmdMsg.Ctx.ImageChanSelect.Msg = (UINT8) CHAN_MSG_SELECT;
        CmdMsg.Ctx.ImageChanSelect.Cmd = (UINT8) IMAGE_CMD_CHAN;
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, "image chan select", VinId);
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, " chain index", ChainIndex);
        AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
    } else {
        AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "usage: [vin id][chain index]");
    }
}

/**
 *  Amba shell image framework flow command channel sensor
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_flow_chan_sensor(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 3U) {
        UINT32 VinId;
        UINT32 ChainId;
        UINT32 SensorId;

        AMBA_IMG_MAIN_CMD_MSG_s CmdMsg;

        VinId = fvar_atou(Argv[1]);
        ChainId = fvar_atou(Argv[2]);
        SensorId = fvar_atou(Argv[3]);

        CmdMsg.Ctx.Data = 0ULL;
        CmdMsg.Ctx.ImageChanSensor.Id = (UINT8) (SensorId & 0xFFU);
        CmdMsg.Ctx.ImageChanSensor.ChainId = (UINT8) (ChainId & 0xFFU);
        CmdMsg.Ctx.ImageChanSensor.Msg = (UINT8) CHAN_MSG_SENSOR;
        CmdMsg.Ctx.ImageChanSensor.Cmd = (UINT8) IMAGE_CMD_CHAN;
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG,   "image chan sensor", VinId);
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG,   " chain id", ChainId);
        AmbaImgFrwCmd_PrintEx(F_PRINT_FLAG_MSG, " sensor id", SensorId, 16U);
        AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
    } else {
        AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "usage: [vin id][chain index]");
    }
}

/**
 *  Amba shell image framework flow command channel algo
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_flow_chan_algo(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 3U) {
        UINT32 VinId;
        UINT32 ChainId;
        UINT32 AlgoId;

        AMBA_IMG_MAIN_CMD_MSG_s CmdMsg;

        VinId = fvar_atou(Argv[1]);
        ChainId = fvar_atou(Argv[2]);
        AlgoId = fvar_atou(Argv[3]);

        CmdMsg.Ctx.Data = 0ULL;
        CmdMsg.Ctx.ImageChanAlgo.Id = (UINT8) (AlgoId & 0xFFU);
        CmdMsg.Ctx.ImageChanAlgo.ChainId = (UINT8) (ChainId & 0xFFU);
        CmdMsg.Ctx.ImageChanAlgo.Msg = (UINT8) CHAN_MSG_ALGO;
        CmdMsg.Ctx.ImageChanAlgo.Cmd = (UINT8) IMAGE_CMD_CHAN;
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, "image chan algo", VinId);
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, " chain id", ChainId);
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, " algo id", AlgoId);
        AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
    } else {
        AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "usage: [vin id][chain id][algo id]");
    }
}

/**
 *  Amba shell image framework flow command channel zone
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_flow_chan_zone(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 3U) {
        UINT32 VinId;
        UINT32 ChainId;
        UINT32 ZoneId;

        AMBA_IMG_MAIN_CMD_MSG_s CmdMsg;

        VinId = fvar_atou(Argv[1]);
        ChainId = fvar_atou(Argv[2]);
        ZoneId = fvar_atou(Argv[3]);

        CmdMsg.Ctx.Data = 0ULL;
        CmdMsg.Ctx.ImageChanZone.Id = ZoneId;
        CmdMsg.Ctx.ImageChanZone.ChainId = (UINT8) (ChainId & 0xFFU);
        CmdMsg.Ctx.ImageChanZone.Msg = (UINT8) CHAN_MSG_ZONE;
        CmdMsg.Ctx.ImageChanZone.Cmd = (UINT8) IMAGE_CMD_CHAN;
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG,   "image chan zone", VinId);
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG,   " chain id", ChainId);
        AmbaImgFrwCmd_PrintEx(F_PRINT_FLAG_MSG, " zone id", ZoneId, 16U);
        AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
    } else {
        AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "usage: [vin id][chain id][zone id]");
    }
}

/**
 *  Amba shell image framework flow command channel zone msb
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_flow_chan_zone_msb(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 3U) {
        UINT32 VinId;
        UINT32 ChainId;
        UINT32 MsbId;

        AMBA_IMG_MAIN_CMD_MSG_s CmdMsg;

        VinId = fvar_atou(Argv[1]);
        ChainId = fvar_atou(Argv[2]);
        MsbId = fvar_atou(Argv[3]);

        CmdMsg.Ctx.Data = 0ULL;
        CmdMsg.Ctx.ImageChanZoneMsb.Id = (UINT8) (MsbId & 0xFFU);
        CmdMsg.Ctx.ImageChanZoneMsb.ChainId = (UINT8) (ChainId & 0xFFU);
        CmdMsg.Ctx.ImageChanZoneMsb.Msg = (UINT8) CHAN_MSG_ZONE_MSB;
        CmdMsg.Ctx.ImageChanZoneMsb.Cmd = (UINT8) IMAGE_CMD_CHAN;
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, "image chan zone msb", VinId);
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, " chain id", ChainId);
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, " zone msb", MsbId);
        AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
    } else {
        AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "usage: [vin id][chain id][zone msb]");
    }
}

/**
 *  Amba shell image framework flow command channel inter
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_flow_chan_inter(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 5U) {
        UINT32 VinId;
        UINT32 ChainId;
        UINT32 InterId;
        UINT32 InterNum;
        UINT32 SkipFrame;

        AMBA_IMG_MAIN_CMD_MSG_s CmdMsg;

        VinId = fvar_atou(Argv[1]);
        ChainId = fvar_atou(Argv[2]);
        InterId = fvar_atou(Argv[3]);
        InterNum = fvar_atou(Argv[4]);
        SkipFrame = fvar_atou(Argv[5]);

        CmdMsg.Ctx.Data = 0ULL;
        CmdMsg.Ctx.ImageChanInter.Id = InterId;
        CmdMsg.Ctx.ImageChanInter.Num = (UINT8) (InterNum & 0xFFU);
        CmdMsg.Ctx.ImageChanInter.Skip = (UINT8) (SkipFrame & 0xFFU);
        CmdMsg.Ctx.ImageChanInter.ChainId = (UINT8) (ChainId & 0xFFU);
        CmdMsg.Ctx.ImageChanInter.Msg = (UINT8) CHAN_MSG_INTER;
        CmdMsg.Ctx.ImageChanInter.Cmd = (UINT8) IMAGE_CMD_CHAN;
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG,   "image chan inter", VinId);
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG,   " chain id", ChainId);
        AmbaImgFrwCmd_PrintEx(F_PRINT_FLAG_MSG, " inter id", InterId, 16U);
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG,   " inter num", InterNum);
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG,   " skip frame", SkipFrame);
        AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
    } else {
        AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "usage: [vin id][chain id][inter id][inter num][skip frame]");
    }
}

/**
 *  Amba shell image framework flow command channel vr
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_flow_chan_vr(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 4U) {
        UINT32 VinId;
        UINT32 ChainId;
        UINT32 VrId;
        UINT32 VrAltId;

        AMBA_IMG_MAIN_CMD_MSG_s CmdMsg;

        VinId = fvar_atou(Argv[1]);
        ChainId = fvar_atou(Argv[2]);
        VrId = fvar_atou(Argv[3]);
        VrAltId = fvar_atou(Argv[4]);

        CmdMsg.Ctx.Data = 0ULL;
        CmdMsg.Ctx.ImageChanVr.Id = VrId;
        CmdMsg.Ctx.ImageChanVr.AltId = (UINT8) (VrAltId & 0xFFU);
        CmdMsg.Ctx.ImageChanVr.ChainId = (UINT8) (ChainId & 0xFFU);
        CmdMsg.Ctx.ImageChanVr.Msg = (UINT8) CHAN_MSG_VR;
        CmdMsg.Ctx.ImageChanVr.Cmd = (UINT8) IMAGE_CMD_CHAN;
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG,   "image chan vr", VinId);
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG,   " chain id", ChainId);
        AmbaImgFrwCmd_PrintEx(F_PRINT_FLAG_MSG, " vr id", VrId, 16U);
        AmbaImgFrwCmd_PrintEx(F_PRINT_FLAG_MSG, " vr alt id", VrAltId, 16U);
        AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
    } else {
        AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "usage: [vin id][chain id][vr id][vr alt id]");
    }
}

/**
 *  Amba shell image framework flow command channel iq
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_flow_chan_iq(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 4U) {
        UINT32 VinId;
        UINT32 ChainId;
        UINT32 IqId;
        UINT32 HdrId;

        AMBA_IMG_MAIN_CMD_MSG_s CmdMsg;

        VinId = fvar_atou(Argv[1]);
        ChainId = fvar_atou(Argv[2]);
        IqId = fvar_atou(Argv[3]);
        HdrId = fvar_atou(Argv[4]);

        CmdMsg.Ctx.Data = 0ULL;
        IqId &= 0xFFU;
        CmdMsg.Ctx.ImageChanIq.Id = (UINT8) IqId;
        HdrId &= 0xFFU;
        CmdMsg.Ctx.ImageChanIq.HdrId = (UINT8) HdrId;
        CmdMsg.Ctx.ImageChanIq.ChainId = (UINT8) (ChainId & 0xFFU);
        CmdMsg.Ctx.ImageChanIq.Msg = (UINT8) CHAN_MSG_IQ;
        CmdMsg.Ctx.ImageChanIq.Cmd = (UINT8) IMAGE_CMD_CHAN;
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, "image chan iq", VinId);
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, " chain id", ChainId);
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, " iq id", IqId);
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, " hdr id", HdrId);
        AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
    } else {
        AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "usage: [vin id][chain id][iq id][hdr id]");
    }
}

/**
 *  Amba shell image framework flow command channel fov
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_flow_chan_fov(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 5U) {
        UINT32 VinId;
        UINT32 ChainId;
        UINT32 ZoneId;
        UINT32 AlgoId;
        UINT32 AdjId;

        AMBA_IMG_MAIN_CMD_MSG_s CmdMsg;

        VinId = fvar_atou(Argv[1]);
        ChainId = fvar_atou(Argv[2]);
        ZoneId = fvar_atou(Argv[3]);
        AlgoId = fvar_atou(Argv[4]);
        AdjId = fvar_atou(Argv[5]);

        CmdMsg.Ctx.Data = 0ULL;
        CmdMsg.Ctx.ImageChanFov.ZoneId = ZoneId;
        CmdMsg.Ctx.ImageChanFov.AlgoId = (UINT8) (AlgoId & 0xFFU);
        CmdMsg.Ctx.ImageChanFov.AdjId = (UINT8) (AdjId & 0xFFU);
        CmdMsg.Ctx.ImageChanFov.ChainId = (UINT8) (ChainId & 0xFFU);
        CmdMsg.Ctx.ImageChanFov.Msg = (UINT8) CHAN_MSG_FOV;
        CmdMsg.Ctx.ImageChanFov.Cmd = (UINT8) IMAGE_CMD_CHAN;
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, "image chan fov", VinId);
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, " chain id", ChainId);
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, " zone id", ZoneId);
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, " algo id", AlgoId);
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, " adj id", AdjId);
        AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
    } else {
        AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "usage: [vin id][chain id][zone id][algo id][adj id]");
    }
}

/**
 *  Amba shell image framework flow command channel fov iq
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_flow_chan_fov_iq(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 3U) {
        UINT32 IqId;
        UINT32 HdrId;
        UINT32 AdjId;

        AMBA_IMG_MAIN_CMD_MSG_s CmdMsg;

        IqId = fvar_atou(Argv[1]);
        HdrId = fvar_atou(Argv[2]);
        AdjId = fvar_atou(Argv[3]);

        CmdMsg.Ctx.Data = 0ULL;
        IqId &= 0xFFU;
        CmdMsg.Ctx.ImageChanFovIq.Id = (UINT8) IqId;
        HdrId &= 0xFFU;
        CmdMsg.Ctx.ImageChanFovIq.HdrId = (UINT8) HdrId;
        CmdMsg.Ctx.ImageChanFovIq.AdjId = (UINT8) (AdjId & 0xFFU);
        CmdMsg.Ctx.ImageChanFovIq.Msg = (UINT8) CHAN_MSG_FOV_IQ;
        CmdMsg.Ctx.ImageChanFovIq.Cmd = (UINT8) IMAGE_CMD_CHAN;
        AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "image chan fov iq");
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG,    " adj id", AdjId);
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG,    " iq id", IqId);
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG,    " hdr id", HdrId);
        AmbaImgMain_Config(0U/*don't care*/, CmdMsg.Ctx.Data);
    } else {
        AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "usage: [iq id][hdr id][adj id]");
    }
}

/**
 *  Amba shell image framework flow command channel avm
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_flow_chan_avm(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 4U) {
        UINT32 VinId;
        UINT32 ChainId;
        UINT32 ZoneId;
        UINT32 AlgoId;

        AMBA_IMG_MAIN_CMD_MSG_s CmdMsg;

        VinId = fvar_atou(Argv[1]);
        ChainId = fvar_atou(Argv[2]);
        ZoneId = fvar_atou(Argv[3]);
        AlgoId = fvar_atou(Argv[4]);

        CmdMsg.Ctx.Data = 0ULL;
        CmdMsg.Ctx.ImageChanAvm.ZoneId = ZoneId;
        CmdMsg.Ctx.ImageChanAvm.AlgoId = (UINT8) (AlgoId & 0xFFU);
        CmdMsg.Ctx.ImageChanAvm.ChainId = (UINT8) (ChainId & 0xFFU);
        CmdMsg.Ctx.ImageChanAvm.Msg = (UINT8) CHAN_MSG_AVM;
        CmdMsg.Ctx.ImageChanAvm.Cmd = (UINT8) IMAGE_CMD_CHAN;
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, "image chan avm", VinId);
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, " chain id", ChainId);
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, " zone id", ZoneId);
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, " algo id", AlgoId);
        AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
    } else {
        AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "usage: [vin id][chain id][zone id][algo id]");
    }
}

/**
 *  Amba shell image framework flow command channel avm iq
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_flow_chan_avm_iq(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 4U) {
        UINT32 VinId;
        UINT32 ChainId;
        UINT32 IqId;
        UINT32 HdrId;

        AMBA_IMG_MAIN_CMD_MSG_s CmdMsg;

        VinId = fvar_atou(Argv[1]);
        ChainId = fvar_atou(Argv[2]);
        IqId = fvar_atou(Argv[3]);
        HdrId = fvar_atou(Argv[4]);

        CmdMsg.Ctx.Data = 0ULL;
        IqId &= 0xFFU;
        CmdMsg.Ctx.ImageChanIq.Id = (UINT8) IqId;
        HdrId &= 0xFFU;
        CmdMsg.Ctx.ImageChanIq.HdrId = (UINT8) HdrId;
        CmdMsg.Ctx.ImageChanIq.ChainId = (UINT8) (ChainId & 0xFFU);
        CmdMsg.Ctx.ImageChanIq.Msg = (UINT8) CHAN_MSG_AVM_IQ;
        CmdMsg.Ctx.ImageChanIq.Cmd = (UINT8) IMAGE_CMD_CHAN;
        AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "image chan avm iq");
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, " chain id", ChainId);
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, " iq id", IqId);
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, " hdr id", HdrId);
        AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
    } else {
        AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "usage: [vin id][chain id][iq id][hdr id]");
    }
}

/**
 *  Amba shell image framework flow command channel eis
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_flow_chan_eis(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 5U) {
        UINT32 VinId;
        UINT32 ChainId;
        UINT32 EisIdx;
        UINT32 AlgoId;
        UINT32 ZoneId;

        AMBA_IMG_MAIN_CMD_MSG_s CmdMsg;

        VinId = fvar_atou(Argv[1]);
        ChainId = fvar_atou(Argv[2]);
        EisIdx = fvar_atou(Argv[3]);
        AlgoId = fvar_atou(Argv[4]);
        ZoneId = fvar_atou(Argv[5]);

        CmdMsg.Ctx.Data = 0ULL;
        CmdMsg.Ctx.ImageChanEis.ZoneId = ZoneId;
        CmdMsg.Ctx.ImageChanEis.AlgoId = (UINT8) (AlgoId & 0xFFU);
        CmdMsg.Ctx.ImageChanEis.Id = (UINT8) (EisIdx & 0xFFU);
        CmdMsg.Ctx.ImageChanEis.ChainId = (UINT8) (ChainId & 0xFFU);
        CmdMsg.Ctx.ImageChanEis.Msg = (UINT8) CHAN_MSG_EIS;
        CmdMsg.Ctx.ImageChanEis.Cmd = (UINT8) IMAGE_CMD_CHAN;
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, "image pipe mode", VinId);
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, " chain id", ChainId);
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, " eis idx", EisIdx);
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, " algo id", AlgoId);
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, " zone id", ZoneId);
        AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
    } else {
        AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "usage: [vin id][chain id][eis idx][algo id][zone id]");
    }
}

/**
 *  Amba shell image framework flow command channel post zone
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_flow_chan_post(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 3U) {
        UINT32 VinId;
        UINT32 ChainId;
        UINT32 PostZoneId;

        AMBA_IMG_MAIN_CMD_MSG_s CmdMsg;

        VinId = fvar_atou(Argv[1]);
        ChainId = fvar_atou(Argv[2]);
        PostZoneId = fvar_atou(Argv[3]);

        CmdMsg.Ctx.Data = 0ULL;
        CmdMsg.Ctx.ImageChanPostZone.Id = PostZoneId;
        CmdMsg.Ctx.ImageChanPostZone.ChainId = (UINT8) (ChainId & 0xFFU);
        CmdMsg.Ctx.ImageChanPostZone.Msg = (UINT8) CHAN_MSG_POST_ZONE;
        CmdMsg.Ctx.ImageChanPostZone.Cmd = (UINT8) IMAGE_CMD_CHAN;
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, "image pipe mode", VinId);
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, " chain id", ChainId);
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, " post zone id", PostZoneId);
        AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
    } else {
        AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "usage: [vin id][chain id][post zone id]");
    }
}

/**
 *  Amba shell image framework flow command channel post zone add
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_flow_chan_post_add(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 3U) {
        UINT32 VinId;
        UINT32 ChainId;
        UINT32 PostZoneId;

        AMBA_IMG_MAIN_CMD_MSG_s CmdMsg;

        VinId = fvar_atou(Argv[1]);
        ChainId = fvar_atou(Argv[2]);
        PostZoneId = fvar_atou(Argv[3]);

        CmdMsg.Ctx.Data = 0ULL;
        CmdMsg.Ctx.ImageChanPostZone.Id = PostZoneId;
        CmdMsg.Ctx.ImageChanPostZone.ChainId = (UINT8) (ChainId & 0xFFU);
        CmdMsg.Ctx.ImageChanPostZone.Msg = (UINT8) CHAN_MSG_POST_ZONE_ADD;
        CmdMsg.Ctx.ImageChanPostZone.Cmd = (UINT8) IMAGE_CMD_CHAN;
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, "image pipe mode", VinId);
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, " chain id", ChainId);
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, " post zone id add", PostZoneId);
        AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
    } else {
        AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "usage: [vin id][chain id][post zone id add]");
    }
}

/**
 *  Amba shell image framework flow command channel aeb
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_flow_chan_aeb(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 5U) {
        UINT32 VinId;
        UINT32 ChainId;
        UINT32 AebCnt;
        UINT32 AebNum;
        UINT32 AebDen;

        AMBA_IMG_MAIN_CMD_MSG_s CmdMsg;

        VinId = fvar_atou(Argv[1]);
        ChainId = fvar_atou(Argv[2]);
        AebCnt = fvar_atou(Argv[3]);
        AebNum = fvar_atou(Argv[4]);
        AebDen = fvar_atou(Argv[5]);

        CmdMsg.Ctx.Data = 0ULL;
        CmdMsg.Ctx.ImageChanSnapAeb.Cnt = (UINT8) (AebCnt & 0xFFU);
        CmdMsg.Ctx.ImageChanSnapAeb.Num = (UINT8) (AebNum & 0xFFU);
        CmdMsg.Ctx.ImageChanSnapAeb.Den = (UINT8) (AebDen & 0xFFU);
        CmdMsg.Ctx.ImageChanSnapAeb.ChainId = (UINT8) (ChainId & 0xFFU);
        CmdMsg.Ctx.ImageChanSnapAeb.Msg = (UINT8) CHAN_MSG_SNAP_AEB;
        CmdMsg.Ctx.ImageChanSnapAeb.Cmd = (UINT8) IMAGE_CMD_CHAN;
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, "image chan aeb", VinId);
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, " chain id", ChainId);
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, " aeb cnt", AebCnt);
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, " aeb num", AebNum);
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, " aeb den", AebDen);
        AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
    } else {
        AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "usage: [vin id][chain id][aeb cnt][aeb num][aeb den]");
    }
}

/**
 *  Amba shell image framework flow command live view start
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_flow_liv_start(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 8U) {
        UINT32 VinId;
        UINT32 OpMode;
        UINT32 ForceSlowShutter;
        UINT32 MaxSlowShutterIndex;
        UINT32 LowDelay;
        UINT32 DeferredBlackLevel;
        UINT32 Stitch;
        UINT32 R2yBackground;

        AMBA_IMG_MAIN_CMD_MSG_s CmdMsg;

        VinId = fvar_atou(Argv[1]);
        OpMode = fvar_atou(Argv[2]);
        ForceSlowShutter = fvar_atou(Argv[3]);
        MaxSlowShutterIndex = fvar_atou(Argv[4]);
        LowDelay = fvar_atou(Argv[5]);
        DeferredBlackLevel = fvar_atou(Argv[6]);
        Stitch = fvar_atou(Argv[7]);
        R2yBackground = fvar_atou(Argv[8]);

        CmdMsg.Ctx.Data = 0ULL;
        CmdMsg.Ctx.Video.R2yBackground = (UINT8) (R2yBackground & 0xFFU);
        CmdMsg.Ctx.Video.Stitch = (UINT8) (Stitch & 0xFFU);
        CmdMsg.Ctx.Video.DeferredBlackLevel = (UINT8) (DeferredBlackLevel & 0xFFU);
        CmdMsg.Ctx.Video.LowDelay = (UINT8) (LowDelay & 0xFFU);
        CmdMsg.Ctx.Video.MaxSlowShutterIndex = (UINT8) (MaxSlowShutterIndex & 0xFFU);
        CmdMsg.Ctx.Video.ForceSlowShutter = (UINT8) (ForceSlowShutter & 0xFFU);
        OpMode &= 0xFFU;
        CmdMsg.Ctx.Video.OpMode = (UINT8) OpMode;
        CmdMsg.Ctx.Video.Cmd = (UINT8) LIVEVIEW_CMD_START;
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, "image liveview start", VinId);
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, " op mode", OpMode);
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, " force ss", ForceSlowShutter);
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, " max ss", MaxSlowShutterIndex);
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, " low delay", LowDelay);
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, " deferred black level", DeferredBlackLevel);
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, " stitch", Stitch);
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, " r2y background", R2yBackground);
        AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
    } else {
        AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "usage: [vin id][op mode][force ss][max ss][low delay][defer bl][stitch][r2y background]");
    }
}

/**
 *  Amba shell image framework flow command live view stop
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_flow_liv_stop(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 1U) {
        UINT32 VinId;

        AMBA_IMG_MAIN_CMD_MSG_s CmdMsg;

        VinId = fvar_atou(Argv[1]);

        CmdMsg.Ctx.Data = 0ULL;
        CmdMsg.Ctx.Video.Cmd = (UINT8) LIVEVIEW_CMD_STOP;
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, "image liveview stop", VinId);
        AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
    } else {
        AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "usage: [vin id]");
    }
}

/**
 *  Amba shell image framework flow command live view lock
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_flow_liv_lock(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 1U) {
        UINT32 VinId;

        AMBA_IMG_MAIN_CMD_MSG_s CmdMsg;

        VinId = fvar_atou(Argv[1]);

        CmdMsg.Ctx.Data = 0ULL;
        CmdMsg.Ctx.Bits.Cmd = (UINT8) LIVEVIEW_CMD_LOCK;
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, "image liveview lock", VinId);
        AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
    } else {
        AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "usage: [vin id]");
    }
}

/**
 *  Amba shell image framework flow command live view unlock
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_flow_liv_unlock(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 1U) {
        UINT32 VinId;

        AMBA_IMG_MAIN_CMD_MSG_s CmdMsg;

        VinId = fvar_atou(Argv[1]);

        CmdMsg.Ctx.Data = 0ULL;
        CmdMsg.Ctx.Bits.Cmd = (UINT8) LIVEVIEW_CMD_UNLOCK;
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, "image liveview unlock", VinId);
        AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
    } else {
        AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "usage: [vin id]");
    }
}

/**
 *  Amba shell image framework flow command fov adj start
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_flow_fov_start(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 1U) {
        UINT32 VinId;

        AMBA_IMG_MAIN_CMD_MSG_s CmdMsg;

        VinId = fvar_atou(Argv[1]);

        CmdMsg.Ctx.Data = 0ULL;
        CmdMsg.Ctx.Bits.Cmd = (UINT8) FOV_CMD_START;
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, "image fov start", VinId);
        AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
    } else {
        AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "usage: [vin id]");
    }
}

/**
 *  Amba shell image framework flow command fov adj stop
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_flow_fov_stop(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 1U) {
        UINT32 VinId;

        AMBA_IMG_MAIN_CMD_MSG_s CmdMsg;

        VinId = fvar_atou(Argv[1]);

        CmdMsg.Ctx.Data = 0ULL;
        CmdMsg.Ctx.Bits.Cmd = (UINT8) FOV_CMD_STOP;
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, "image fov stop", VinId);
        AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
    } else {
        AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "usage: [vin id]");
    }
}

/**
 *  Amba shell image framework flow command avm start
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_flow_avm_start(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    AMBA_IMG_MAIN_CMD_MSG_s CmdMsg;

    (void) Argc;
    (void) Argv;

    if (PrintFunc == NULL) {
        /* */
    }

    CmdMsg.Ctx.Data = 0ULL;
    CmdMsg.Ctx.Bits.Cmd = (UINT8) AVM_CMD_START;
    AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "image avm start");
    AmbaImgMain_Config(0U/*don't care*/, CmdMsg.Ctx.Data);
}

/**
 *  Amba shell image framework flow command avm stop
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_flow_avm_stop(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    AMBA_IMG_MAIN_CMD_MSG_s CmdMsg;

    (void) Argc;
    (void) Argv;

    if (PrintFunc == NULL) {
        /* */
    }

    CmdMsg.Ctx.Data = 0ULL;
    CmdMsg.Ctx.Bits.Cmd = (UINT8) AVM_CMD_STOP;
    AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "image avm stop");
    AmbaImgMain_Config(0U/*don't care*/, CmdMsg.Ctx.Data);
}

/**
 *  Amba shell image framework flow command eis start
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_flow_eis_start(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 1U) {
        UINT32 VinId;

        AMBA_IMG_MAIN_CMD_MSG_s CmdMsg;

        VinId = fvar_atou(Argv[1]);

        CmdMsg.Ctx.Data = 0ULL;
        CmdMsg.Ctx.Bits.Cmd = (UINT8) IMG_EIS_CMD_START;
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, "image eis start", VinId);
        AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
    } else {
        AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "usage: [vin id]");
    }
}

/**
 *  Amba shell image framework flow command eis stop
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_flow_eis_stop(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 1U) {
        UINT32 VinId;

        AMBA_IMG_MAIN_CMD_MSG_s CmdMsg;

        VinId = fvar_atou(Argv[1]);

        CmdMsg.Ctx.Data = 0ULL;
        CmdMsg.Ctx.Bits.Cmd = (UINT8) IMG_EIS_CMD_STOP;
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, "image eis stop", VinId);
        AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
    } else {
        AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "usage: [vin id]");
    }
}

/**
 *  Amba shell image framework flow command sync start
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_flow_sync_start(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 1U) {
        UINT32 VinId;

        AMBA_IMG_MAIN_CMD_MSG_s CmdMsg;

        VinId = fvar_atou(Argv[1]);

        CmdMsg.Ctx.Data = 0ULL;
        CmdMsg.Ctx.Bits.Cmd = (UINT8) SYNC_CMD_START;
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, "image sync start", VinId);
        AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
    } else {
        AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "usage: [vin id]");
    }
}

/**
 *  Amba shell image framework flow command sync stop
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_flow_sync_stop(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 1U) {
        UINT32 VinId;

        AMBA_IMG_MAIN_CMD_MSG_s CmdMsg;

        VinId = fvar_atou(Argv[1]);

        CmdMsg.Ctx.Data = 0ULL;
        CmdMsg.Ctx.Bits.Cmd = (UINT8) SYNC_CMD_STOP;
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, "image sync stop", VinId);
        AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
    } else {
        AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "usage: [vin id]");
    }
}

/**
 *  Amba shell image framework flow command statistics start
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_flow_stat_start(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    AMBA_IMG_MAIN_CMD_MSG_s CmdMsg;

    (void) Argc;
    (void) Argv;

    if (PrintFunc == NULL) {
        /* */
    }

    CmdMsg.Ctx.Data = 0ULL;
    CmdMsg.Ctx.Bits.Cmd = (UINT8) STATISTICS_CMD_START;
    AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "image statistics start");
    AmbaImgMain_Config(0U/*don't care*/, CmdMsg.Ctx.Data);
}

/**
 *  Amba shell image framework flow command statistics stop
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_flow_stat_stop(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    AMBA_IMG_MAIN_CMD_MSG_s CmdMsg;

    (void) Argc;
    (void) Argv;

    if (PrintFunc == NULL) {
        /* */
    }

    CmdMsg.Ctx.Data = 0ULL;
    CmdMsg.Ctx.Bits.Cmd = (UINT8) STATISTICS_CMD_STOP;
    AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "image statistics stop");
    AmbaImgMain_Config(0U/*don't care*/, CmdMsg.Ctx.Data);
}

/**
 *  Amba shell image framework flow command statistics inter
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_flow_stat_inter(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 2U) {
        UINT32 VinId;
        UINT32 Enable;

        AMBA_IMG_MAIN_CMD_MSG_s CmdMsg;

        VinId = fvar_atou(Argv[1]);
        Enable = fvar_atou(Argv[2]);

        CmdMsg.Ctx.Data = 0ULL;
        CmdMsg.Ctx.Bits.Var = Enable;
        CmdMsg.Ctx.Bits.Cmd = (UINT8) STATISTICS_CMD_INTER;
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, "image statistics inter", VinId);
        AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
    } else {
        AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "usage: [vin id][enable]");
    }
}

