/**
 *  @file AmbaAf_AfProc.c
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
 *  @details Amba Image Auto Focus Process
 *
*/
#include "AmbaAf_AfProc.h"
#include "AmbaPrint.h"
#include "AmbaUtility.h"
#include "AmbaWrap.h"
/* Get 3a info */
#include "AmbaImg_External_CtrlFunc.h"

#ifdef CONFIG_SOC_CV5
/* [Lens control] */
#include "AmbaSensor.h"
#include "AmbaLens.h" // HOOK
#include "AmbaLens_OV48C40_LC898249XH.h"
#endif

static UINT32 AfProcDebugFlag = 0U;

static UINT32 AFtile_roi[4U] = {41, 42, 53, 54};


#ifdef CONFIG_SOC_CV5
/* Hyper-params for CAF Algo */
static UINT16 roiW = 6U;
static UINT16 roiH = 4U;
static INT16 scan_delay = 200;
static INT16 AfStride = 1;            // [1, 1023]
static UINT32 turn_back_rate = 40U;

static INT16 dir_sign = 1;            // +1 or -1
static INT16 AfPos = 0;                // [0, 1023]
static INT16 turn_back_steps = 0;
static UINT32 PeakVal = 0U;

static UINT8 initFlag = 0U;
static UINT8 initDelay = 5U;
static UINT32 (*CAFfunc)(const AMBA_IK_CFA_3A_DATA_s *cfa, const AMBA_IK_PG_3A_DATA_s *rgb);

/*
     [Lens control] Please specify VinID to operate the sensor.
*/
static UINT32 s_VinID = 0U;

static void AmbaAf_PrintHexUInt32(UINT32 Value, AMBA_SHELL_PRINT_f PrintFunc)
{
    char HexString[AF_UTIL_MAX_HEX_STR_LEN];
    HexString[0] = '0';
    HexString[1] = 'x';
    HexString[2] = '\0';
    (void)AmbaUtility_UInt32ToStr(&HexString[2], AF_UTIL_MAX_HEX_STR_LEN-2, Value, 16);
    PrintFunc(HexString);
}


static UINT32 AmbaAf_InitLensModule(void){
    UINT32 retVal = LENS_ERR_NONE;
    /* [Lens control] Please change your lens module here. Default : HOOK DEVICE OV48C40 LC898249XH */
    if (pAmbaLensObj[(UINT8)AMBA_LENS_SENSOR0] == NULL) {
        AmbaLENS_Hook((UINT8)AMBA_LENS_SENSOR0, &AmbaLens_OV48C40Obj);
        AmbaPrint_PrintUInt5("[Lens control] Hook LENS OV48C40 ...", 0U, 0U, 0U, 0U, 0U);

        /* [Lens control] check registers read/write works. */
        retVal = pAmbaLensObj[(UINT8)AMBA_LENS_SENSOR0]->Init(s_VinID);
    }
    return retVal;
}


static void AmbaAf_PrintInfo(const AMBA_IK_CFA_3A_DATA_s *pCfaStat, const AMBA_IK_PG_3A_DATA_s *pRgbStat) {
    UINT32 Afstate[4U];
    UINT16 tileH = 0U;
    UINT16 tileW = 0U;
    UINT32 printedArg[7U];
    UINT32 printRoiInfo[3U];
    UINT16 afIdx = 0U;

    if(AmbaWrap_memset(Afstate, 0, sizeof(UINT32) * 4U) != 0U) {
        AmbaPrint_PrintUInt5("AmbaWrap_memset set error\n", 0U, 0U, 0U, 0U, 0U);
    }
    if(AmbaWrap_memset(printedArg, 0, sizeof(UINT32) * 7U) != 0U) {
        AmbaPrint_PrintUInt5("AmbaWrap_memset set error\n", 0U, 0U, 0U, 0U, 0U);
    }
    if(AmbaWrap_memset(printRoiInfo, 0, sizeof(UINT32) * 3U) != 0U) {
        AmbaPrint_PrintUInt5("AmbaWrap_memset set error\n", 0U, 0U, 0U, 0U, 0U);
    }
    if((AmbaWrap_memcpy(&tileH, &(pCfaStat->Header.CfaAf.AfTileNumRow), sizeof(UINT8)) != 0U) || (AmbaWrap_memcpy(&tileW, &(pCfaStat->Header.CfaAf.AfTileNumCol), sizeof(UINT8)) != 0U)) {
        AmbaPrint_PrintUInt5("AmbaWrap_memcpy set error\n", 0U, 0U, 0U, 0U, 0U);
    }

    AmbaPrint_PrintUInt5("CFA NumCol %d; NumRow %d ", pCfaStat->Header.CfaAf.AfTileNumCol, pCfaStat->Header.CfaAf.AfTileNumRow,0U, 0U, 0U);
    AmbaPrint_PrintUInt5("CFA ColStart %d; RowStart %d ", pCfaStat->Header.CfaAf.AfTileColStart, pCfaStat->Header.CfaAf.AfTileRowStart,0U, 0U, 0U);
    AmbaPrint_PrintUInt5("CFA TileWidth %d; TileHeight %d ", pCfaStat->Header.CfaAf.AfTileWidth, pCfaStat->Header.CfaAf.AfTileHeight,0U, 0U, 0U);
    AmbaPrint_PrintUInt5("CFA ActiveWidth %d; ActiveHeight %d ", pCfaStat->Header.CfaAf.AfTileActiveWidth, pCfaStat->Header.CfaAf.AfTileActiveHeight,0U, 0U, 0U);

    AmbaPrint_PrintUInt5("RGB NumCol %d; NumRow %d ", pRgbStat->Header.PgAf.AfTileNumCol, pRgbStat->Header.PgAf.AfTileNumRow,0U, 0U, 0U);
    AmbaPrint_PrintUInt5("RGB ColStart %d; RowStart %d ", pRgbStat->Header.PgAf.AfTileColStart, pRgbStat->Header.PgAf.AfTileRowStart,0U, 0U, 0U);
    AmbaPrint_PrintUInt5("RGB TileWidth %d; TileHeight %d ", pRgbStat->Header.PgAf.AfTileWidth, pRgbStat->Header.PgAf.AfTileHeight,0U, 0U, 0U);
    AmbaPrint_PrintUInt5("RGB ActiveWidth %d; ActiveHeight %d ", pRgbStat->Header.PgAf.AfTileActiveWidth, pRgbStat->Header.PgAf.AfTileActiveHeight,0U, 0U, 0U);

    AmbaPrint_PrintUInt5(" ====================================================== ", 0U, 0U, 0U, 0U, 0U);

    if((AmbaWrap_memcpy(&printedArg[0], &roiH, sizeof(UINT16)) != 0U) || (AmbaWrap_memcpy(&printedArg[1], &roiW, sizeof(UINT16)) != 0U) ||
        (AmbaWrap_memcpy(&printedArg[2], &AfStride, sizeof(INT16)) != 0U) || (AmbaWrap_memcpy(&printedArg[3], &turn_back_steps, sizeof(INT16)) != 0U) ||
         (AmbaWrap_memcpy(&printedArg[4], &scan_delay, sizeof(INT16)) != 0U) || (AmbaWrap_memcpy(&printedArg[5], &AfPos, sizeof(INT16)) != 0U) ||
         (AmbaWrap_memcpy(&printedArg[6], &initDelay, sizeof(UINT8)) != 0U)) {
        AmbaPrint_PrintUInt5("AmbaWrap_memcpy error\n", 0U, 0U, 0U, 0U, 0U);
    }

    AmbaPrint_PrintUInt5("ROI H %d; W %d\n", printedArg[0], printedArg[1], 0U, 0U, 0U);

    /* Accumulate all FV within roi: roiW x roiH */
    for(UINT16 row = ((tileH - roiH) >> 1); row < (((tileH - roiH) >> 1)+roiH); row++) {
        for(UINT16 col = ((tileW - roiW) >> 1); col < (((tileW - roiW) >> 1)+roiW); col++) {
            afIdx = (tileW * row) + col;
            Afstate[0] += pCfaStat->Af[afIdx].SumFV1;
            Afstate[1] += pCfaStat->Af[afIdx].SumFV2;
            Afstate[2] += pRgbStat->Af[afIdx].SumFV1;
            Afstate[3] += pRgbStat->Af[afIdx].SumFV2;

            if((AmbaWrap_memcpy(&printRoiInfo[0], &row, sizeof(UINT16)) != 0U) || (AmbaWrap_memcpy(&printRoiInfo[1], &col, sizeof(UINT16)) != 0U) ||
                (AmbaWrap_memcpy(&printRoiInfo[2], &afIdx, sizeof(UINT16)) != 0U)) {
                AmbaPrint_PrintUInt5("AmbaWrap_memcpy error\n", 0U, 0U, 0U, 0U, 0U);
            }

            AmbaPrint_PrintUInt5("ROI row %d; col %d; At FOV %d", printRoiInfo[0], printRoiInfo[1], printRoiInfo[2], 0U, 0U);
        }
        AmbaPrint_PrintUInt5("\n", 0U, 0U, 0U, 0U, 0U);
    }

    AmbaPrint_PrintUInt5("[Af] pCfaFV1 %d  pCfaFV2 %d  pRgbFV1 %d  pRgbFV2 %d", Afstate[0], Afstate[1], Afstate[2], Afstate[3], 0U);
    if(dir_sign > 0) {
        AmbaPrint_PrintUInt5("[AF] Sign +1; AfStride %d; Turn_back_steps %d; Delay %d; Init Delay %d; AfPos %d", printedArg[2], printedArg[3], printedArg[4], printedArg[6], printedArg[5]);
    }
    else {
        AmbaPrint_PrintUInt5("[AF] Sign -1; AfStride %d; Turn_back_steps %d; Delay %d; Init Delay %d; AfPos %d\n", printedArg[2], printedArg[3], printedArg[4], printedArg[6], printedArg[5]);
    }

}

static void AmbaAf_AfInit(UINT32 in_stride, UINT32 in_rate){
    UINT32 retVal = LENS_ERR_NONE;
    UINT32 u_stride = (in_stride < 1U)? 1U : ((in_stride > 1023U)? 1023U : in_stride);
    PeakVal = 0U;
    turn_back_steps = 0;
    turn_back_rate = (in_rate < 1U)? 1U : ((in_rate > 100U)? 100U : in_rate);

    if(AmbaWrap_memcpy(&AfStride, &u_stride, sizeof(INT16)) != 0U) {
        AmbaPrint_PrintUInt5("AmbaWrap_memcpy error\n", 0U, 0U, 0U, 0U, 0U);
        initFlag = 0U;
    }
    else {
        /* [Lens control] check if hook default lens module : OV48C40 */
        if(AmbaAf_InitLensModule() != LENS_ERR_NONE) {
            AmbaPrint_PrintUInt5("[Lens control] AmbaAf_InitLensModule error\n", 0U, 0U, 0U, 0U, 0U);
            initFlag = 0U;
        }
        else {
            if(AfPos != 1023) {
                AfPos = 0;
                dir_sign = 1;
                /* [Lens control] Please push the lens to position 0 (Far) */
                retVal = pAmbaLensObj[(UINT8)AMBA_LENS_SENSOR0]->Oper(s_VinID, 0U);
            }
            else {
                dir_sign = -1;
                /* [Lens control] Please push the lens to position 1023 (Near) */
                retVal = pAmbaLensObj[(UINT8)AMBA_LENS_SENSOR0]->Oper(s_VinID, 1023U);
            }

            /* Activate flag : initFlag */
            if(retVal != LENS_ERR_NONE) {
                AmbaPrint_PrintUInt5("[Lens control] Lens operation error\n", 0U, 0U, 0U, 0U, 0U);
                initFlag = 0U;
            }
            else {
                initFlag = 1U;
            }
        }
    }
}

static UINT32 AmbaAf_SampleAlgo_CAF(const AMBA_IK_CFA_3A_DATA_s *pCfaStat, const AMBA_IK_PG_3A_DATA_s *pRgbStat){
    /* Collect some info for FV */
    UINT32 retErr = AF_ERR_NONE;
    UINT32 u_AfPos = 0U;
    UINT32 Afstate[4U];
    UINT16 tileH = 0U;
    UINT16 tileW = 0U;

    if(initFlag < initDelay) {
        /* [Init] Wait for lens to push at the specific position : 5 frame time*/
        initFlag++;
    }
    else {
        if(AmbaWrap_memset(Afstate, 0, sizeof(UINT32) * 4U) != 0U ) {
            AmbaPrint_PrintUInt5("AmbaWrap_memset set error\n", 0U, 0U, 0U, 0U, 0U);
            retErr = AF_ERR_MANAGER_TSK_ERR;
            initFlag = 0U;
        }
        else {
            if((AmbaWrap_memcpy(&tileH, &(pCfaStat->Header.CfaAf.AfTileNumRow), sizeof(UINT8)) != 0U) || (AmbaWrap_memcpy(&tileW, &(pCfaStat->Header.CfaAf.AfTileNumCol), sizeof(UINT8)) != 0U)) {
                AmbaPrint_PrintUInt5("AmbaWrap_memcpy set error\n", 0U, 0U, 0U, 0U, 0U);
                retErr = AF_ERR_MANAGER_TSK_ERR;
                initFlag = 0U;
            }
            else {
                /* Accumulate all FV within roi */
                for(UINT16 row = ((tileH - roiH) >> 1); row < (((tileH - roiH) >> 1)+roiH); row++) {
                    for(UINT16 col = ((tileW - roiW) >> 1); col < (((tileW - roiW) >> 1)+roiW); col++) {
                        Afstate[0] += pCfaStat->Af[(tileW * row) + col].SumFV1;
                        Afstate[1] += pCfaStat->Af[(tileW * row) + col].SumFV2;
                        Afstate[2] += pRgbStat->Af[(tileW * row) + col].SumFV1;
                        Afstate[3] += pRgbStat->Af[(tileW * row) + col].SumFV2;
                    }
                }
                if(AfProcDebugFlag > 0U) {
                    AmbaPrint_PrintUInt5("CFA Af PeakVal %d  pCfaFV1 %d  pCfaFV2 %d  pRgbFV1 %d  pRgbFV2 %d\n", PeakVal, Afstate[0], Afstate[1], Afstate[2], Afstate[3]);
                }

                if(PeakVal <= Afstate[1]) {
                    AfPos += (dir_sign * AfStride);
                    PeakVal = Afstate[1];
                    turn_back_steps = 0;
                }
                else if (((PeakVal * turn_back_rate) / 100U) <= Afstate[1]) {
                    if(AfProcDebugFlag > 0U) {
                        AmbaPrint_PrintUInt5("[Meet Maxima] Keep Going !\n", 0U, 0U, 0U, 0U, 0U);
                    }
                    AfPos += (dir_sign * AfStride);
                    turn_back_steps++;
                }
                else {
                    if(AfProcDebugFlag > 0U) {
                        AmbaPrint_PrintUInt5("Turn back !\n", 0U, 0U, 0U, 0U, 0U);
                    }
                    turn_back_steps = (turn_back_steps * 100) + scan_delay;
                    AfPos -= (((turn_back_steps * (dir_sign * AfStride)) + 50) / 100); // round
                    turn_back_steps = 0;
                }

                if((AfPos > 1023) || (AfPos < 0)) {
                    if(turn_back_steps > 0) {
                        turn_back_steps = (turn_back_steps * 100) + scan_delay;
                        AfPos -= (((turn_back_steps * (dir_sign * AfStride)) + 50) / 100);
                    }

                    AfPos = (AfPos > 1023)? 1023 : ((AfPos < 0)? 0 : AfPos);

                    if(AmbaWrap_memcpy(&u_AfPos, &AfPos, sizeof(INT16)) != 0U) {
                        AmbaPrint_PrintUInt5("AmbaWrap_memcpy error !\n", 0U, 0U, 0U, 0U, 0U);
                        retErr = AF_ERR_MANAGER_TSK_ERR;
                        initFlag = 0U;
                    }
                    else {
                        /* [Lens control] Please push the lens to position [u_AfPos] */
                        retErr = pAmbaLensObj[(UINT8)AMBA_LENS_SENSOR0]->Oper(s_VinID, u_AfPos);

                        if(AfProcDebugFlag > 0U) {
                            AmbaPrint_PrintUInt5("Lens already reach the limit (%d)!\n",u_AfPos, 0U, 0U, 0U, 0U);
                        }
                        initFlag = 0U;
                    }
                }
                else {
                    if(AmbaWrap_memcpy(&u_AfPos, &AfPos, sizeof(INT16)) != 0U) {
                        AmbaPrint_PrintUInt5("AmbaWrap_memcpy error !\n", 0U, 0U, 0U, 0U, 0U);
                        retErr = AF_ERR_MANAGER_TSK_ERR;
                        initFlag = 0U;
                    }
                    else {
                        /* [Lens control] Please push the lens to position [u_AfPos] */
                        retErr = pAmbaLensObj[(UINT8)AMBA_LENS_SENSOR0]->Oper(s_VinID, u_AfPos);

                        if (((PeakVal * turn_back_rate) / 100U) > Afstate[1]) {
                            initFlag = 0U;
                        }
                    }
                }

                if(AfProcDebugFlag > 0U) {
                    AmbaPrint_PrintUInt5("Af pos %d\n", u_AfPos, 0U, 0U, 0U, 0U);
                }
            }
        }
    }
return retErr;
}

static UINT32 AmbaAf_LensScan(const AMBA_IK_CFA_3A_DATA_s *pCfaStat, const AMBA_IK_PG_3A_DATA_s *pRgbStat){
    /* Collect some info for FV */
    UINT32 retErr = AF_ERR_NONE;
    UINT32 u_AfPos = 0U;
    UINT32 Afstate[4U];
    UINT16 tileH = 0U;
    UINT16 tileW = 0U;

    if(initFlag < initDelay) {
        /* [Init] Wait for lens to push at the specific position : 5 frame time*/
        initFlag++;
    }
    else {
        if(AmbaWrap_memset(Afstate, 0, sizeof(UINT32) * 4U) != 0U) {
            AmbaPrint_PrintUInt5("AmbaWrap_memset set error\n", 0U, 0U, 0U, 0U, 0U);
            retErr = AF_ERR_MANAGER_TSK_ERR;
            initFlag = 0U;
        }
        else {
            if((AmbaWrap_memcpy(&tileH, &(pCfaStat->Header.CfaAf.AfTileNumRow), sizeof(UINT8)) != 0U) || (AmbaWrap_memcpy(&tileW, &(pCfaStat->Header.CfaAf.AfTileNumCol), sizeof(UINT8)) != 0U)) {
                AmbaPrint_PrintUInt5("AmbaWrap_memcpy set error\n", 0U, 0U, 0U, 0U, 0U);
                retErr = AF_ERR_MANAGER_TSK_ERR;
                initFlag = 0U;
            }
            else {
                /* Accumulate all FV within roi */
                for(UINT16 row = ((tileH - roiH) >> 1); row < (((tileH - roiH) >> 1)+roiH); row++) {
                    for(UINT16 col = ((tileW - roiW) >> 1); col < (((tileW - roiW) >> 1)+roiW); col++) {
                        Afstate[0] += pCfaStat->Af[(tileW * row) + col].SumFV1;
                        Afstate[1] += pCfaStat->Af[(tileW * row) + col].SumFV2;
                        Afstate[2] += pRgbStat->Af[(tileW * row) + col].SumFV1;
                        Afstate[3] += pRgbStat->Af[(tileW * row) + col].SumFV2;
                    }
                }
                if(AfProcDebugFlag > 0U) {
                    AmbaPrint_PrintUInt5("CFA Af pCfaFV1 %d  pCfaFV2 %d  pRgbFV1 %d  pRgbFV2 %d\n", Afstate[0], Afstate[1], Afstate[2], Afstate[3], 0U);
                }

                AfPos += (dir_sign * AfStride);

                if((AfPos > 1023) || (AfPos < 0)) {
                    AfPos = (AfPos > 1023)? 1023 : 0;
                    if(AmbaWrap_memcpy(&u_AfPos, &AfPos, sizeof(INT16)) != 0U) {
                        AmbaPrint_PrintUInt5("AmbaWrap_memcpy error\n", 0U, 0U, 0U, 0U, 0U);
                        retErr = AF_ERR_MANAGER_TSK_ERR;
                        initFlag = 0U;
                    }
                    else {
                        /* [Lens control] Please push the lens to position [u_AfPos] */
                        retErr = pAmbaLensObj[(UINT8)AMBA_LENS_SENSOR0]->Oper(s_VinID, u_AfPos);
                    }

                    if(AfProcDebugFlag > 0U) {
                        AmbaPrint_PrintUInt5("Lens already reach the limit (%d)!\n", u_AfPos, 0U, 0U, 0U, 0U);
                    }
                    initFlag = 0U;
                }
                else {
                    if(AmbaWrap_memcpy(&u_AfPos, &AfPos, sizeof(INT16)) != 0U) {
                        AmbaPrint_PrintUInt5("AmbaWrap_memcpy error\n", 0U, 0U, 0U, 0U, 0U);
                        retErr = AF_ERR_MANAGER_TSK_ERR;
                        initFlag = 0U;
                    }
                    else {
                        /* [Lens control] Please push the lens to position [u_AfPos] */
                        retErr = pAmbaLensObj[(UINT8)AMBA_LENS_SENSOR0]->Oper(s_VinID, u_AfPos);
                    }
                }

                if(AfProcDebugFlag > 0U) {
                    AmbaPrint_PrintUInt5("CFA Af pos %d  pCfaFV1 %d  pCfaFV2 %d  pRgbFV1 %d  pRgbFV2 %d", u_AfPos, Afstate[0], Afstate[1], Afstate[2], Afstate[3]);
                }
            }
        }
    }
return retErr;
}

#endif

static void AmbaAf_AfCmdUsage(char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("Usage:\n");
    PrintFunc(pArgVector[0]);
    PrintFunc(" dbg prt [enable]   - print AF Info or statistic\n");
    PrintFunc("            0       - disable\n");
    PrintFunc("            1       - print AF statistic\n");
    PrintFunc("            2       - print AF information\n");
    PrintFunc("af dbg roi [left_top tile No.]  - set AF statistic calc ROI which is based on default AF tile(H,V) = (12,8) \n");
    PrintFunc("    e.g: af dbg roi 41 => means AF statistic is sum by tile(41, 42, 53, 54)\n");

#ifdef CONFIG_SOC_CV5
    PrintFunc("af info                            - Check pCfaFV and pRgbFV\n");
    PrintFunc("af info roi [roiH] [roiW]        - Check pCfaFV and pRgbFV within specific roi\n\n");

    PrintFunc("af ntf [stride] [turn back rate]                        - [CAF] Near-to-Far (1023 -> 0)\n");
    PrintFunc("   ftn [stride] [turn back rate]                        - [CAF] Far-to-Near (0 -> 1023)\n");
    PrintFunc("af ntf [stride] [turn back rate] roi [roiH] [roiW]    - [CAF] Near-to-Far with specific roi\n");
    PrintFunc("   ftn [stride] [turn back rate] roi [roiH] [roiW]    - [CAF] Far-to-Near with specific roi\n\n");

    PrintFunc("af scan ftn [stride]                                - [Scan only] Far-to-Near\n");
    PrintFunc("        ntf [stride]                                - [Scan only] Near-to-Far\n");
    PrintFunc("af scan ftn [stride] roi [roiH] [roiW]            - [Scan only] Far to Near with specific roi\n");
    PrintFunc("         ntf [stride] roi [roiH] [roiW]            - [Scan only] Near to Far with specific roi\n\n");

    PrintFunc("af idelay [number of frame]        - Set delay how many frame times used for initial CAF or Scan (0~255)\n");
    PrintFunc("af delay [number of frame]        - Set delay value used for CAF turn back (1~100)\n");
    PrintFunc("af debug [0/1]                - Moniter the detail msg of the algo. & parameters during Scaning or CAF (0: off, 1: on)\n\n");

    /* [Lens control] */
    PrintFunc("af init               - Hook lens module and Register initialization & set roi region as center crop\n");
    PrintFunc("af oper [position]  - Push lens to the specific position\n");

    PrintFunc("af readpos                    - Check the lens position stored in register (OV48C40: register No. 0x84)\n");
    PrintFunc("af read2 [] [] [register DEC No.] [TRSize]                        - Check value stored in register\n");
    PrintFunc("   e.g., Read the register No. 0x84 : af read2 0 0 132 2\n");

    PrintFunc("af write2 [] [] [register DEC No.] [TRSize] [Value1] [...]        - Write sensor register\n");
    PrintFunc("   e.g., Assign 1023(0x03FF) to the register No. 0x84 : af write2 0 0 132 2 3 255\n");
#endif
}


UINT32 AmbaAf_AfProcess(const AMBA_IK_CFA_3A_DATA_s *pCfaStat, const AMBA_IK_PG_3A_DATA_s *pRgbStat)
{
    UINT32 retErr = AF_ERR_NONE;
#ifdef CONFIG_SOC_CV5
    if(initFlag == 0U) {
#endif
        static UINT16 AfStat_DebugCount = 0U;
        UINT32 AfStat_DebugSum[4U];
        UINT8 cnt = 0U;

        if(AmbaWrap_memset(AfStat_DebugSum, 0, sizeof(UINT32) * 4U) != 0U) {
            AmbaPrint_PrintUInt5("AmbaWrap_memset set error\n", 0U, 0U, 0U, 0U, 0U);
            retErr = AF_ERR_MANAGER_TSK_ERR;
        }

        /* Debug test*/
        if(AMBA_AF_DEBUG_STAT_PRT_EN == AfProcDebugFlag){
            for(cnt = 0U; cnt < 4U; cnt++){
                AfStat_DebugSum[0] += pCfaStat->Af[ AFtile_roi[cnt]].SumFV1;
                AfStat_DebugSum[1] += pCfaStat->Af[ AFtile_roi[cnt]].SumFV2;
                AfStat_DebugSum[2] += pRgbStat->Af[ AFtile_roi[cnt]].SumFV1;
                AfStat_DebugSum[3] += pRgbStat->Af[ AFtile_roi[cnt]].SumFV2;
            }
            if(5U == AfStat_DebugCount){
                AmbaPrint_PrintUInt5("CFA_FV1: %5d, CFA_FV2: %5d, RGB_FV1: %5d, RGB_FV2: %5d", AfStat_DebugSum[0],AfStat_DebugSum[1],AfStat_DebugSum[2],AfStat_DebugSum[3],0U);
                AfStat_DebugCount = 0U;
            }
            AfStat_DebugCount++;

        }else if(AMBA_AF_DEBUG_INFO_PRT_EN == AfProcDebugFlag){
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV5)
                AmbaPrint_PrintUInt5("CFA NumCol %d; NumRow %d ", pCfaStat->Header.CfaAf.AfTileNumCol, pCfaStat->Header.CfaAf.AfTileNumRow,0U, 0U, 0U);
                AmbaPrint_PrintUInt5("CFA ColStart %d; RowStart %d ", pCfaStat->Header.CfaAf.AfTileColStart, pCfaStat->Header.CfaAf.AfTileRowStart,0U, 0U, 0U);
                AmbaPrint_PrintUInt5("CFA TileWidth %d; TileHeight %d ", pCfaStat->Header.CfaAf.AfTileWidth, pCfaStat->Header.CfaAf.AfTileHeight,0U, 0U, 0U);
                AmbaPrint_PrintUInt5("CFA ActiveWidth %d; ActiveHeight %d ", pCfaStat->Header.CfaAf.AfTileActiveWidth, pCfaStat->Header.CfaAf.AfTileActiveHeight,0U, 0U, 0U);
                AmbaPrint_PrintUInt5("RGB NumCol %d; NumRow %d ", pRgbStat->Header.PgAf.AfTileNumCol, pRgbStat->Header.PgAf.AfTileNumRow,0U, 0U, 0U);
                AmbaPrint_PrintUInt5("RGB ColStart %d; RowStart %d ", pRgbStat->Header.PgAf.AfTileColStart, pRgbStat->Header.PgAf.AfTileRowStart,0U, 0U, 0U);
                AmbaPrint_PrintUInt5("RGB TileWidth %d; TileHeight %d ", pRgbStat->Header.PgAf.AfTileWidth, pRgbStat->Header.PgAf.AfTileHeight,0U, 0U, 0U);
                AmbaPrint_PrintUInt5("RGB ActiveWidth %d; ActiveHeight %d ", pRgbStat->Header.PgAf.AfTileActiveWidth, pRgbStat->Header.PgAf.AfTileActiveHeight,0U, 0U, 0U);
                AfProcDebugFlag = AMBA_AF_DEBUG_DISABLE;

#else
                AmbaPrint_PrintUInt5("CFA NumCol %d; NumRow %d ", pCfaStat->Header.AfTileNumCol, pCfaStat->Header.AfTileNumRow,0U, 0U, 0U);
                AmbaPrint_PrintUInt5("CFA ColStart %d; RowStart %d ", pCfaStat->Header.AfTileColStart, pCfaStat->Header.AfTileRowStart,0U, 0U, 0U);
                AmbaPrint_PrintUInt5("CFA TileWidth %d; TileHeight %d ", pCfaStat->Header.AfTileWidth, pCfaStat->Header.AfTileHeight,0U, 0U, 0U);
                AmbaPrint_PrintUInt5("CFA ActiveWidth %d; ActiveHeight %d ", pCfaStat->Header.AfTileActiveWidth, pCfaStat->Header.AfTileActiveHeight,0U, 0U, 0U);
                AmbaPrint_PrintUInt5("RGB NumCol %d; NumRow %d ", pRgbStat->Header.AfTileNumCol, pRgbStat->Header.AfTileNumRow,0U, 0U, 0U);
                AmbaPrint_PrintUInt5("RGB ColStart %d; RowStart %d ", pRgbStat->Header.AfTileColStart, pRgbStat->Header.AfTileRowStart,0U, 0U, 0U);
                AmbaPrint_PrintUInt5("RGB TileWidth %d; TileHeight %d ", pRgbStat->Header.AfTileWidth, pRgbStat->Header.AfTileHeight,0U, 0U, 0U);
                AmbaPrint_PrintUInt5("RGB ActiveWidth %d; ActiveHeight %d ", pRgbStat->Header.AfTileActiveWidth, pRgbStat->Header.AfTileActiveHeight,0U, 0U, 0U);
                AfProcDebugFlag = AMBA_AF_DEBUG_DISABLE;
#endif

        }else if(AMBA_AF_DEBUG_DISABLE == AfProcDebugFlag){
            AfStat_DebugCount = 0U;
        }else{
            /*misra c*/
        }
#ifdef CONFIG_SOC_CV5
    }

    else {
        retErr = CAFfunc(pCfaStat, pRgbStat);
    }
#endif
    return retErr;
}


void AmbaAf_AfTestCmd(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 CaseID;
    UINT32 Arg1 = 0;
    UINT32 Arg2 = 0;
    UINT32 Arg3 = 0;

#ifdef CONFIG_SOC_CV5
    UINT32    in_roiW = 0U;
    UINT32  in_roiH = 0U;
    UINT32  in_init_delay = 0U;
    UINT32    in_scan_delay = 0U;
    UINT32    debugMSG = 0U;
    UINT32    in_stride = 0U;
    UINT32    in_rate = 0U;
    UINT32     ChNo = 0;

    /* [Lens control] */
    UINT32 Addr = 0U;
    UINT32 Data = 0U;
    UINT32 TRSize = 0U;
    UINT32 i = 0U;
    UINT16 Addr16 = 0U;
    UINT8 R_Data8 = 0U;
    UINT8 T_Data8[16] = {0U};

    static AMBA_IK_CFA_3A_DATA_s        pCFAStat __attribute__((section(".bss.noinit")));
    static AMBA_IK_PG_3A_DATA_s            pRgbStat __attribute__((section(".bss.noinit")));
#endif
    if (ArgCount < 3U) {
#ifdef CONFIG_SOC_CV5
        if ((AmbaUtility_StringCompare(pArgVector[1],"init", 4U) == 0)) {
            /* [Lens control] Please change your lens module here. Default : HOOK DEVICE OV48C40 LC898249XH */
            AmbaLENS_Hook((UINT8)AMBA_LENS_SENSOR0, &AmbaLens_OV48C40Obj);
            AmbaPrint_PrintUInt5("[Lens control] Hook LENS OV48C40 ...", 0U, 0U, 0U, 0U, 0U);

            /* [Lens control] check registers read/write works. */
            if(pAmbaLensObj[(UINT8)AMBA_LENS_SENSOR0]->Init(s_VinID) != LENS_ERR_NONE) {
                AmbaPrint_PrintUInt5("[Lens control] Lens init error ...\n", 0U, 0U, 0U, 0U, 0U);
            }
            else {
                /* Default roi for FV : (tileH/2, tileW/2) */
                (void)AmbaImgProc_GetCFAAAAStat(ChNo, &pCFAStat);
                if((AmbaWrap_memcpy(&roiH, &(pCFAStat.Header.CfaAf.AfTileNumRow), sizeof(UINT8)) != 0U) || (AmbaWrap_memcpy(&roiW, &(pCFAStat.Header.CfaAf.AfTileNumCol), sizeof(UINT8)) != 0U)) {
                    AmbaPrint_PrintUInt5("AmbaWrap_memcpy error\n", 0U, 0U, 0U, 0U, 0U);
                }
                roiH >>= 1;
                roiW >>= 1;
            }
        }
        else if(AmbaUtility_StringCompare(pArgVector[1],"info", 4U) == 0){
            (void)AmbaImgProc_GetCFAAAAStat(ChNo, &pCFAStat);
            (void)AmbaImgProc_GetPGAAAStat(ChNo, &pRgbStat);
            AmbaAf_PrintInfo(&pCFAStat, &pRgbStat);
        }
        else if(AmbaUtility_StringCompare(pArgVector[1],"readpos", 7U) == 0) {
            /* [Lens control] Default : HOOK DEVICE : OV48C40 LC898249XH*/
            if(AmbaAf_InitLensModule() != LENS_ERR_NONE) {
                AmbaPrint_PrintUInt5("[Lens control] AmbaAf_InitLensModule error\n", 0U, 0U, 0U, 0U, 0U);
            }
            else {
                if(pAmbaLensObj[(UINT8)AMBA_LENS_SENSOR0]->GetStatus(s_VinID) != LENS_ERR_NONE) {
                    PrintFunc("[Lens control] GetStatus (Read position) Error!\n");
                }
            }
        }
        else {
            AmbaAf_AfCmdUsage(pArgVector, PrintFunc);
        }
#else
        AmbaAf_AfCmdUsage(pArgVector, PrintFunc);
#endif
    }
    else {
        AmbaPrint_PrintUInt5("ArgCount = %d",ArgCount, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintStr5("af %s %s %s %s %s",pArgVector[1],pArgVector[2],pArgVector[3],pArgVector[4], pArgVector[5]);

        (void)AmbaUtility_StringToUInt32(pArgVector[2], &CaseID);
        (void)AmbaUtility_StringToUInt32(pArgVector[3], &Arg1);
        (void)AmbaUtility_StringToUInt32(pArgVector[4], &Arg2);
        (void)AmbaUtility_StringToUInt32(pArgVector[5], &Arg3);


        if (AmbaUtility_StringCompare(pArgVector[1], "dbg",3U) == 0){
            if((AmbaUtility_StringCompare(pArgVector[2], "prt", 3U) == 0) && (ArgCount == 4U)) {
                AfProcDebugFlag = Arg1;
                if(AfProcDebugFlag > 2U){
                    AmbaAf_AfCmdUsage(pArgVector, PrintFunc);
                }
            }else if((AmbaUtility_StringCompare(pArgVector[2], "roi", 3U) == 0) && (ArgCount == 4U)) {

                if(Arg1 < 83U){
                    AFtile_roi[0] = Arg1;  AFtile_roi[1] = Arg1+1U;  AFtile_roi[2] = Arg1+12U;  AFtile_roi[3] = Arg1+13U;
                    AmbaPrint_PrintUInt5("AF Stat Sum by tile (%d,%d,%d,%d)", AFtile_roi[0], AFtile_roi[1], AFtile_roi[2], AFtile_roi[3], 0U);
                }else{
                    AmbaAf_AfCmdUsage(pArgVector, PrintFunc);
                }
            }else{
                AmbaPrint_PrintUInt5("unknown case ", 0U, 0U, 0U, 0U, 0U);
            }
        }

#ifdef CONFIG_SOC_CV5
        else if(AmbaUtility_StringCompare(pArgVector[1],"info", 4U) == 0) {
            if(AmbaUtility_StringCompare(pArgVector[2],"roi", 3U) == 0) {
                if((AmbaUtility_StringToUInt32(pArgVector[3], &in_roiH) != 0U) || (AmbaUtility_StringToUInt32(pArgVector[4], &in_roiW)!= 0U)){
                    PrintFunc("AmbaUtility_StringToUInt32 Error!\n");
                }
                else {
                    if((AmbaWrap_memcpy(&roiH, &in_roiH, sizeof(UINT8)) != 0U) || (AmbaWrap_memcpy(&roiW, &in_roiW, sizeof(UINT8)) != 0U)) {
                        PrintFunc("AmbaWrap_memcpy Error!\n");
                    }
                }
            }
            (void)AmbaImgProc_GetCFAAAAStat(ChNo, &pCFAStat);
            (void)AmbaImgProc_GetPGAAAStat(ChNo, &pRgbStat);
            AmbaAf_PrintInfo(&pCFAStat, &pRgbStat);
        }
        else if ((AmbaUtility_StringCompare(pArgVector[1],"oper", 4U) == 0)) {
            if(AmbaUtility_StringToUInt32(pArgVector[2], &Arg2)!= 0U){
                PrintFunc("AmbaUtility_StringToUInt32 Error!\n");
            }
            else {
                /* [Lens control] Default : HOOK DEVICE : OV48C40 LC898249XH*/
                if(AmbaAf_InitLensModule() != LENS_ERR_NONE) {
                    PrintFunc("[Lens control] AmbaAf_InitLensModule Error!\n");
                }
                /* [Lens control] Please push the lens to position [Arg2] */
                else if(pAmbaLensObj[(UINT8)AMBA_LENS_SENSOR0]->Oper(s_VinID, Arg2) != LENS_ERR_NONE){
                    PrintFunc("[Lens control] Lens operation error!\n");
                }
                else {
                    /* Misra */
                }
            }
        }
        else if(AmbaUtility_StringCompare(pArgVector[1],"delay", 5U) == 0) {
            if(AmbaUtility_StringToUInt32(pArgVector[2], &in_scan_delay)!= 0U) {
                PrintFunc("AmbaUtility_StringToUInt32 Error!\n");
            }
            else {
                if(AmbaWrap_memcpy(&scan_delay, &in_scan_delay, sizeof(INT16)) != 0U) {
                    PrintFunc("AmbaWrap_memcpy Error!\n");
                }
            }
        }
        else if(AmbaUtility_StringCompare(pArgVector[1],"idelay", 6U) == 0) {
            if(AmbaUtility_StringToUInt32(pArgVector[2], &in_init_delay)!= 0U) {
                PrintFunc("AmbaUtility_StringToUInt32 Error!\n");
            }
            else {
                if(AmbaWrap_memcpy(&initDelay, &in_init_delay, sizeof(INT8)) != 0U) {
                    PrintFunc("AmbaWrap_memcpy Error!\n");
                }
            }
        }
        else if ((AmbaUtility_StringCompare(pArgVector[1],"scan", 3U) == 0)) {
            if(AmbaUtility_StringToUInt32(pArgVector[3], &in_stride)!= 0U){
                PrintFunc("AmbaUtility_StringToUInt32 Error!\n");
            }
            else if (AmbaUtility_StringCompare(pArgVector[2],"ntf", 3U) == 0) {
                if(AmbaUtility_StringCompare(pArgVector[4],"roi", 3U) == 0) {
                    if((AmbaUtility_StringToUInt32(pArgVector[5], &in_roiH)!= 0U) || (AmbaUtility_StringToUInt32(pArgVector[6], &in_roiW)!= 0U)){
                        PrintFunc("AmbaUtility_StringToUInt32 Error!\n");
                    }
                    else {
                        if((AmbaWrap_memcpy(&roiH, &in_roiH, sizeof(UINT16)) != 0U)  || (AmbaWrap_memcpy(&roiW, &in_roiW, sizeof(UINT16)) != 0U)) {
                            PrintFunc("AmbaWrap_memcpy Error!\n");
                        }
                    }
                }
                else {
                    (void)AmbaImgProc_GetCFAAAAStat(ChNo, &pCFAStat);
                    if((AmbaWrap_memcpy(&roiH, &(pCFAStat.Header.CfaAf.AfTileNumRow), sizeof(UINT8)) != 0U) || (AmbaWrap_memcpy(&roiW, &(pCFAStat.Header.CfaAf.AfTileNumCol), sizeof(UINT8)) != 0U)) {
                        AmbaPrint_PrintUInt5("AmbaWrap_memcpy error\n", 0U, 0U, 0U, 0U, 0U);
                    }
                    roiH >>= 1;
                    roiW >>= 1;
                }
                AfPos = 1023;
                CAFfunc = AmbaAf_LensScan;
                AmbaAf_AfInit(in_stride, 1U);
            }
            else if (AmbaUtility_StringCompare(pArgVector[2],"ftn", 3U) == 0) {
                if(AmbaUtility_StringCompare(pArgVector[4],"roi", 3U) == 0) {
                    if((AmbaUtility_StringToUInt32(pArgVector[5], &in_roiH)!= 0U) || (AmbaUtility_StringToUInt32(pArgVector[6], &in_roiW)!= 0U)){
                        PrintFunc("AmbaUtility_StringToUInt32 Error!\n");
                    }
                    else {
                        if((AmbaWrap_memcpy(&roiH, &in_roiH, sizeof(UINT16)) != 0U)  || (AmbaWrap_memcpy(&roiW, &in_roiW, sizeof(UINT16)) != 0U)) {
                            PrintFunc("AmbaWrap_memcpy Error!\n");
                        }
                    }
                }
                else {
                    (void)AmbaImgProc_GetCFAAAAStat(ChNo, &pCFAStat);

                    if((AmbaWrap_memcpy(&roiH, &(pCFAStat.Header.CfaAf.AfTileNumRow), sizeof(UINT8)) != 0U) || (AmbaWrap_memcpy(&roiW, &(pCFAStat.Header.CfaAf.AfTileNumCol), sizeof(UINT8)) != 0U)) {
                        AmbaPrint_PrintUInt5("AmbaWrap_memcpy error\n", 0U, 0U, 0U, 0U, 0U);
                    }
                    roiH >>= 1;
                    roiW >>= 1;
                }
                AfPos = 0;
                CAFfunc = AmbaAf_LensScan;
                AmbaAf_AfInit(in_stride, 1U);
            }
            else {
                AmbaAf_AfCmdUsage(pArgVector, PrintFunc);
            }
        }
        else if (AmbaUtility_StringCompare(pArgVector[1],"ntf", 3U) == 0) {
            if((AmbaUtility_StringToUInt32(pArgVector[2], &in_stride)!= 0U) || (AmbaUtility_StringToUInt32(pArgVector[3], &in_rate)!= 0U)){
                    PrintFunc("AmbaUtility_StringToUInt32 Error!\n");
            }
            else {
                if(AmbaUtility_StringCompare(pArgVector[4],"roi", 3U) == 0) {
                    if((AmbaUtility_StringToUInt32(pArgVector[5], &in_roiH)!= 0U) || (AmbaUtility_StringToUInt32(pArgVector[6], &in_roiW)!= 0U)){
                        PrintFunc("AmbaUtility_StringToUInt32 Error!\n");
                    }
                    else {
                        if((AmbaWrap_memcpy(&roiH, &in_roiH, sizeof(UINT16)) != 0U)    || (AmbaWrap_memcpy(&roiW, &in_roiW, sizeof(UINT16)) != 0U)) {
                            PrintFunc("AmbaWrap_memcpy Error!\n");
                        }
                    }
                }
                else {
                    (void)AmbaImgProc_GetCFAAAAStat(ChNo, &pCFAStat);
                    if((AmbaWrap_memcpy(&roiH, &(pCFAStat.Header.CfaAf.AfTileNumRow), sizeof(UINT8)) != 0U) || (AmbaWrap_memcpy(&roiW, &(pCFAStat.Header.CfaAf.AfTileNumCol), sizeof(UINT8)) != 0U)) {
                        AmbaPrint_PrintUInt5("AmbaWrap_memcpy error\n", 0U, 0U, 0U, 0U, 0U);
                    }
                    roiH >>= 1;
                    roiW >>= 1;
                }
                AfPos = 1023;
                CAFfunc = AmbaAf_SampleAlgo_CAF;
                AmbaAf_AfInit(in_stride, in_rate);
            }
        }
        else if (AmbaUtility_StringCompare(pArgVector[1],"ftn", 3U) == 0) {
            if((AmbaUtility_StringToUInt32(pArgVector[2], &in_stride)!= 0U) || (AmbaUtility_StringToUInt32(pArgVector[3], &in_rate)!= 0U)){
                    PrintFunc("AmbaUtility_StringToUInt32 Error!\n");
            }
            else {
                if(AmbaUtility_StringCompare(pArgVector[4],"roi", 3U) == 0) {
                    if((AmbaUtility_StringToUInt32(pArgVector[5], &in_roiH)!= 0U) || (AmbaUtility_StringToUInt32(pArgVector[6], &in_roiW)!= 0U)){
                        PrintFunc("AmbaUtility_StringToUInt32 Error!\n");
                    }
                    else {
                        if((AmbaWrap_memcpy(&roiH, &in_roiH, sizeof(UINT16)) != 0U)    || (AmbaWrap_memcpy(&roiW, &in_roiW, sizeof(UINT16)) != 0U)) {
                            PrintFunc("AmbaWrap_memcpy Error!\n");
                        }
                    }
                }
                else {
                    (void)AmbaImgProc_GetCFAAAAStat(ChNo, &pCFAStat);
                    if((AmbaWrap_memcpy(&roiH, &(pCFAStat.Header.CfaAf.AfTileNumRow), sizeof(UINT8)) != 0U) || (AmbaWrap_memcpy(&roiW, &(pCFAStat.Header.CfaAf.AfTileNumCol), sizeof(UINT8)) != 0U)) {
                        AmbaPrint_PrintUInt5("AmbaWrap_memcpy error\n", 0U, 0U, 0U, 0U, 0U);
                    }
                    roiH >>= 1;
                    roiW >>= 1;
                }

                AfPos = 0;
                CAFfunc = AmbaAf_SampleAlgo_CAF;
                AmbaAf_AfInit(in_stride, in_rate);
            }
        }
        else if(AmbaUtility_StringCompare(pArgVector[1],"debug", 5U) == 0) {
            if(AmbaUtility_StringToUInt32(pArgVector[2], &debugMSG)!= 0U){
                PrintFunc("AmbaUtility_StringToUInt32 Error!\n");
            }
            else {
                AfProcDebugFlag = (debugMSG == 0U)? 0U : 1U;
            }
        }

        else if ((AmbaUtility_StringCompare(pArgVector[1], "read2", 5U) == 0) && (ArgCount == 6U)) {
            /* read sensor register */
            if (AmbaUtility_StringToUInt32(pArgVector[4], &Addr) != 0U) {
                PrintFunc("AmbaUtility_StringToUInt32 Error!\n");
                AmbaAf_AfCmdUsage(pArgVector, PrintFunc);
            }
            else if (AmbaUtility_StringToUInt32(pArgVector[5], &TRSize) != 0U) {
                PrintFunc("AmbaUtility_StringToUInt32 Error!\n");
                AmbaAf_AfCmdUsage(pArgVector, PrintFunc);
            }
            else if(AmbaWrap_memcpy(&Addr16, &Addr, sizeof(UINT16)) != 0U) {
                PrintFunc("AmbaWrap_memcpy Error!\n");
                AmbaAf_AfCmdUsage(pArgVector, PrintFunc);
            }
            else {
                /* [Lens control] Default : HOOK DEVICE : OV48C40 LC898249XH*/
                if(AmbaAf_InitLensModule() != LENS_ERR_NONE) {
                    PrintFunc("[Lens control] AmbaAf_InitLensModule Error!\n");
                }
                else if (pAmbaLensObj[(UINT8)AMBA_LENS_SENSOR0]->RegRead(s_VinID, Addr16, &R_Data8, TRSize) != LENS_ERR_NONE) {
                    PrintFunc("[Lens control] Register read error\n");
                }
                else {
                    Data = R_Data8;
                    PrintFunc("UT Reg");
                    AmbaAf_PrintHexUInt32(Addr, PrintFunc);
                    PrintFunc(" = ");
                    AmbaAf_PrintHexUInt32(Data, PrintFunc);
                    PrintFunc(" \n ");
                }
            }
        }

        else if ((AmbaUtility_StringCompare(pArgVector[1], "write2", 6U) == 0) && (ArgCount > 6U)) {
            /* write sensor register */
            if (AmbaUtility_StringToUInt32(pArgVector[4], &Addr) != 0U) {
                PrintFunc("AmbaUtility_StringToUInt32 Error!\n");
                AmbaAf_AfCmdUsage(pArgVector, PrintFunc);
            }
            else if (AmbaUtility_StringToUInt32(pArgVector[5], &TRSize) != 0U) {
                PrintFunc("AmbaUtility_StringToUInt32 Error!\n");
                AmbaAf_AfCmdUsage(pArgVector, PrintFunc);
            }
            else {
                if (TRSize > 0U) {
                    for (i = 0U; i < TRSize; i++) {
                        if (AmbaUtility_StringToUInt32(pArgVector[6U + i], &Data) != 0U) {
                            PrintFunc("AmbaUtility_StringToUInt32 Error!\n");
                            break;
                        }
                        T_Data8[i] = (UINT8)Data;
                    }
                    if (i >= TRSize) {
                        if(AmbaWrap_memcpy(&Addr16, &Addr, sizeof(UINT16)) != 0U) {
                            PrintFunc("AmbaWrap_memcpy Error!\n");
                        }
                        else {
                            /* [Lens control] Default : HOOK DEVICE : OV48C40 LC898249XH*/
                            if(AmbaAf_InitLensModule() != LENS_ERR_NONE) {
                                PrintFunc("[Lens control] AmbaAf_InitLensModule Error!\n");
                            }
                            else {
                                if (pAmbaLensObj[(UINT8)AMBA_LENS_SENSOR0]->RegWrite(s_VinID, Addr16, T_Data8, TRSize) != LENS_ERR_NONE) {
                                    PrintFunc("[Lens control] Register read error\n");
                                }
                            }
                        }
                    }
                    else {
                        AmbaAf_AfCmdUsage(pArgVector, PrintFunc);
                    }
                }
            }
        }
#endif
        else {
            AmbaAf_AfCmdUsage(pArgVector, PrintFunc);
        }
    }

}
