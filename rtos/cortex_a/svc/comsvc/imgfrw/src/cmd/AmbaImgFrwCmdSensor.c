/**
 *  @file AmbaImgFrwCmdSensor.c
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
 *  @details AmbaShell Image Framework Command Sensor
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaVIN_Def.h"
#include "AmbaShell.h"

#include "AmbaImgChannel.h"
#include "AmbaImgSensorHAL.h"
#include "AmbaImgSensorSync.h"
#include "AmbaImgSensorDrv.h"

#include "AmbaImgFrwCmd_Def.h"
#include "AmbaImgFrwCmdVar.h"
#include "AmbaImgFrwCmdSensor.h"
#include "AmbaImgFrwCmdSensor_Table.c"

#define OK_UL  ((UINT32) 0U)
//#define NG_UL  ((UINT32) 1U)

/**
 *  @private
 *  Amba shell image framework sensor command entry
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
void ambsh_imgfrw_sensor(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 CmdIdx = 0U;
    const AMBA_IMG_FRW_CMD_s *List = AmbaImgFrwCmdSensor_List;

    if (Argc > 1U) {
        while ((List != NULL) && (List->Magic == AMBSH_MAGIC)) {
            /* valid */
            if (fvar_strcmp(Argv[1], List->pName) == OK_UL) {
                /* debug msg */
                AmbaImgFrwCmd_Print(F_PRINT_FLAG_DBG, "imgfrw cmd sensor idx", CmdIdx);
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
 *  Amba shell image framework sensor command load
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_sensor_load(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 i, CmdNum;
    AMBA_IMG_FRW_CMD_s *List = AmbaImgFrwCmdSensor_List;

    (void) Argc;
    (void) Argv;

    if (PrintFunc == NULL) {
        /* */
    }

    /* cmd num get */
    CmdNum = (UINT32) (sizeof(AmbaImgFrwCmdSensor_List)/sizeof(AMBA_IMG_FRW_CMD_s));
    /* debug msg */
    AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, "imgfrw cmd sensor load", CmdNum);

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
 *  Amba shell image framework sensor command timing enable
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_sensor_tm_en(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 2U) {
        UINT32 FuncRetCode;
        UINT32 VinId;
        UINT32 Enable;

        VinId = fvar_atou(Argv[1]);
        Enable = fvar_atou(Argv[2]);

        if (VinId < AMBA_IMGFRW_NUM_VIN_CHANNEL) {
            AmbaImgFrwCmd_PrintEx2(F_PRINT_FLAG_MSG, "timing mark enable", VinId, 10U, Enable, 16U);
            FuncRetCode = AmbaImgSensorHAL_TimingMarkEn(VinId, Enable);
            if (FuncRetCode != OK_UL) {
                AmbaImgFrwCmd_PrintEx2(F_PRINT_FLAG_ERR, "error: timing mark enable", VinId, 10U, Enable, 16U);
            }
        }
    } else {
        AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "usage: [vin_id] [enable]");
    }
}

/**
 *  Amba shell image framework sensor command timing clear
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_sensor_tm_clr(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 1U) {
        UINT32 FuncRetCode;
        UINT32 VinId;

        VinId = fvar_atou(Argv[1]);

        if (VinId < AMBA_IMGFRW_NUM_VIN_CHANNEL) {
            AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, "timing mark clear", VinId);
            FuncRetCode =  AmbaImgSensorHAL_TimingMarkClr(VinId);
            if (FuncRetCode != OK_UL) {
                AmbaImgFrwCmd_Print(F_PRINT_FLAG_ERR, "error: timing mark clear", VinId);
            }
        }
    } else {
        AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "usage: [vin id]");
    }
}

/**
 *  Amba shell image framework sensor command timing print
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_sensor_tm_prt(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 1U) {
        UINT32 VinId;

        VinId = fvar_atou(Argv[1]);

        if (VinId < AMBA_IMGFRW_NUM_VIN_CHANNEL) {
            AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, "timing mark print", VinId);
            AmbaImgSensorHAL_TimingMarkPrt(VinId);
        }
    } else {
        AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "usage: [vin_id]");
    }
}

/**
 *  Amba shell image framework sensor command hal debug
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_sensor_hal_dbg(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 2U) {
        UINT32 FuncRetCode;

        UINT32 VinId;
        UINT32 Flag;

        VinId = fvar_atou(Argv[1]);
        Flag = fvar_atou(Argv[2]);

        if (VinId < AMBA_IMGFRW_NUM_VIN_CHANNEL) {
            AmbaImgFrwCmd_PrintEx2(F_PRINT_FLAG_MSG, "hal debug", VinId, 10U, Flag, 16U);
            FuncRetCode = AmbaImgSensorHAL_Debug(VinId, Flag);
            if (FuncRetCode != OK_UL) {
                /* */
            }
        }
    } else {
        AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "usage: [vin_id] [flag]");
    }
}

/**
 *  Amba shell image framework sensor command sync debug
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_sensor_sync_dbg(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 2U) {
        UINT32 FuncRetCode;

        UINT32 VinId;
        UINT32 Flag;

        VinId = fvar_atou(Argv[1]);
        Flag = fvar_atou(Argv[2]);

        if (VinId < AMBA_IMGFRW_NUM_VIN_CHANNEL) {
            AmbaImgFrwCmd_PrintEx2(F_PRINT_FLAG_MSG, "sync debug", VinId, 10U, Flag, 16U);
            FuncRetCode = AmbaImgSensorSync_Debug(VinId, Flag);
            if (FuncRetCode != OK_UL) {
                /* */
            }
        }
    } else {
        AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "usage: [vin_id] [flag]");
    }
}

/**
 *  Amba shell image framework sensor command register write
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_sensor_reg_wr(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 4U) {
        UINT32 FuncRetCode;

        UINT32 VinId;
        UINT32 SensorId;

        UINT16 Addr;
        UINT16 Data;

        UINT8 Buf[4];

        AMBA_IMG_CHANNEL_ID_s ImageChanId = { .Ctx = { .Data = 0ULL } };

        VinId = fvar_atou(Argv[1]);
        SensorId = fvar_atou(Argv[2]);

        Addr = (UINT16) (fvar_atou(Argv[3]) & 0xFFFFU);
        Data = (UINT16) (fvar_atou(Argv[4]) & 0xFFFFU);

        ImageChanId.Ctx.Data = 0ULL;
        ImageChanId.Ctx.Bits.VinId = (UINT8) (VinId & 0xFFU);
        ImageChanId.Ctx.Bits.SensorId = (UINT8) (SensorId & 0xFFU);

        Buf[0] = (UINT8) ((Addr >> 8U) & 0xFFU);
        Buf[1] = (UINT8) (Addr & 0xFFU);
        Buf[2] = (UINT8) ((Data >> 8U) & 0xFFU);
        Buf[3] = (UINT8) (Data & 0xFFU);

        AmbaImgFrwCmd_PrintEx2(F_PRINT_FLAG_MSG, "sensor", VinId, 10U, SensorId, 16U);
        AmbaImgFrwCmd_PrintEx2(F_PRINT_FLAG_MSG, "reg write", Addr, 16U, Data, 16U);
        FuncRetCode = AmbaImgSensorDrv_RegWrite(ImageChanId, Buf, 4U);
        if (FuncRetCode != OK_UL) {
            AmbaImgFrwCmd_PrintEx2(F_PRINT_FLAG_ERR, "error: reg write", Addr, 16U, Data, 16U);
        }
    } else {
        AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "usage: [vin id] [sensor id] [addr] [data]");
    }
}

