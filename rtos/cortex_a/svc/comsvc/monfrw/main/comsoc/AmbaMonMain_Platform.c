/**
 *  @file AmbaMonMain_Platform.c
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
 *  @details Amba Monitor Main for Common SOC
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaVIN_Def.h"

#include "AmbaWrap.h"

#include "AmbaDSP_Capability.h"
//#include "AmbaDSP_Event.h"
#include "AmbaDSP_EventInfo.h"

#include "AmbaMonFramework.h"
#include "AmbaMonDef.h"

#include "AmbaMonVin.h"
#include "AmbaMonDsp.h"
#include "AmbaMonVout.h"

#include "AmbaMonMain.h"
#include "AmbaMonMain_Internal.h"
#include "AmbaMonMain_Platform.h"

#define OK_UL  ((UINT32) 0U)
//#define NG_UL  ((UINT32) 1U)

typedef struct /*_AMBA_MON_MAIN_TIMEOUT_CB_INFO_s_*/ {
    char Name[32];
} AMBA_MON_MAIN_TIMEOUT_CB_INFO_s;

AMBA_MON_TIMEOUT_CB_s VinSofTimeoutCb[AMBA_MON_NUM_VIN_CHANNEL];
AMBA_MON_TIMEOUT_CB_s VinEofTimeoutCb[AMBA_MON_NUM_VIN_CHANNEL];
AMBA_MON_TIMEOUT_CB_s DspRawTimeoutCb[AMBA_MON_NUM_VIN_CHANNEL];
AMBA_MON_TIMEOUT_CB_s DspYuvAltTimeoutCb[AMBA_MON_NUM_FOV_CHANNEL];
AMBA_MON_TIMEOUT_CB_s DspVoutTimeoutCb[AMBA_MON_NUM_VOUT_CHANNEL];
AMBA_MON_TIMEOUT_CB_s VoutIsrTimeoutCb[AMBA_MON_NUM_VOUT_CHANNEL];

/**
 *  @private
 *  amba monitor main memory init
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonMain_MemInit(void)
{
    /* example of vin sof timeout cb */
    static AMBA_MON_MAIN_TIMEOUT_CB_INFO_s VinSofTimeoutCb_Info[AMBA_MON_NUM_VIN_CHANNEL];
    static AMBA_MON_VIN_ISR_RDY_s VinSofTimeoutInfo[AMBA_MON_NUM_VIN_CHANNEL];
    /* example of vin eof timeout cb */
    static AMBA_MON_MAIN_TIMEOUT_CB_INFO_s VinEofTimeoutCb_Info[AMBA_MON_NUM_VIN_CHANNEL];
    static AMBA_MON_VIN_ISR_RDY_s VinEofTimeoutInfo[AMBA_MON_NUM_VIN_CHANNEL];
    /* example of raw msg timeout cb */
    static AMBA_MON_MAIN_TIMEOUT_CB_INFO_s DspRawTimeoutCb_Info[AMBA_MON_NUM_VIN_CHANNEL];
    static AMBA_DSP_RAW_DATA_RDY_s DspRawTimeoutInfo[AMBA_MON_NUM_VIN_CHANNEL];
    /* example of fov msg timeout cb */
    static AMBA_MON_MAIN_TIMEOUT_CB_INFO_s DspYuvAltTimeoutCb_Info[AMBA_MON_NUM_FOV_CHANNEL];
    static AMBA_DSP_YUV_DATA_RDY_EXTEND_s DspYuvAltTimeoutInfo[AMBA_MON_NUM_FOV_CHANNEL];
    /* example of vout msg timeout cb */
    static AMBA_MON_MAIN_TIMEOUT_CB_INFO_s DspVoutTimeoutCb_Info[AMBA_MON_NUM_VOUT_CHANNEL];
    static AMBA_DSP_VOUT_DATA_INFO_s DspVoutTimeoutInfo[AMBA_MON_NUM_VOUT_CHANNEL];
    /* example of vout isr timeout cb */
    static AMBA_MON_MAIN_TIMEOUT_CB_INFO_s VoutIsrTimeoutCb_Info[AMBA_MON_NUM_VOUT_CHANNEL];
    static AMBA_MON_VOUT_ISR_RDY_s VoutIsrTimeoutInfo[AMBA_MON_NUM_VOUT_CHANNEL];

    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    /* vin sof timeout */
    for (i = 0U; i < AMBA_MON_NUM_VIN_CHANNEL; i++) {
        /* timeout info */
        VinSofTimeoutInfo[i].VinId = i;
        VinSofTimeoutInfo[i].Pts = 0ULL;
        VinSofTimeoutInfo[i].Dts = 0ULL;
        /* name */
        {
            char str[11];
            VinSofTimeoutCb_Info[i].Name[0] = '\0';
            (void) svar_strcat(VinSofTimeoutCb_Info[i].Name, "sof_");
            svar_utoa(i, str, 10U, 1U, (UINT32) SVAR_LEADING_ZERO);
            (void) svar_strcat(VinSofTimeoutCb_Info[i].Name, str);
        }
        /* cb name */
        VinSofTimeoutCb[i].pName = VinSofTimeoutCb_Info[i].Name;
        /* cb info */
        VinSofTimeoutCb[i].Param.pVinIsrRdy = &(VinSofTimeoutInfo[i]);
        /* cb func */
        VinSofTimeoutCb[i].pFunc = AmbaMonVin_SofTimeout;
    }
    /* vin eof timeout */
    for (i = 0U; i < AMBA_MON_NUM_VIN_CHANNEL; i++) {
        /* timeout info */
        VinEofTimeoutInfo[i].VinId = i;
        VinEofTimeoutInfo[i].Pts = 0ULL;
        VinEofTimeoutInfo[i].Dts = 0ULL;
        /* name */
        {
            char str[11];
            VinEofTimeoutCb_Info[i].Name[0] = '\0';
            (void) svar_strcat(VinEofTimeoutCb_Info[i].Name, "eof_");
            svar_utoa(i, str, 10U, 1U, (UINT32) SVAR_LEADING_ZERO);
            (void) svar_strcat(VinEofTimeoutCb_Info[i].Name, str);
        }
        /* cb name */
        VinEofTimeoutCb[i].pName = VinEofTimeoutCb_Info[i].Name;
        /* cb info */
        VinEofTimeoutCb[i].Param.pVinIsrRdy = &(VinEofTimeoutInfo[i]);
        /* cb func */
        VinEofTimeoutCb[i].pFunc = AmbaMonVin_EofTimeout;
    }
    /* vin raw timeout */
    for (i = 0U; i < AMBA_MON_NUM_VIN_CHANNEL; i++) {
        /* timeout info */
        DspRawTimeoutInfo[i].VinId = (UINT16) i;
        DspRawTimeoutInfo[i].CapPts = 0ULL;
        DspRawTimeoutInfo[i].CapSequence = 0ULL;
        /* name */
        {
            char str[11];
            DspRawTimeoutCb_Info[i].Name[0] = '\0';
            (void) svar_strcat(DspRawTimeoutCb_Info[i].Name, "raw_");
            svar_utoa(i, str, 10U, 1U, (UINT32) SVAR_LEADING_ZERO);
            (void) svar_strcat(DspRawTimeoutCb_Info[i].Name, str);
        }
        /* cb name */
        DspRawTimeoutCb[i].pName = DspRawTimeoutCb_Info[i].Name;
        /* cb info */
        DspRawTimeoutCb[i].Param.pRawRdy = &(DspRawTimeoutInfo[i]);
        /* cb func */
        DspRawTimeoutCb[i].pFunc = AmbaMonDsp_RawTimeout;
    }
    /* dsp fov timeout */
    for (i = 0U; i < AMBA_MON_NUM_FOV_CHANNEL; i++) {
        /* timeout info */
        DspYuvAltTimeoutInfo[i].ViewZoneId = i;
        DspYuvAltTimeoutInfo[i].VinId = 0U;
        DspYuvAltTimeoutInfo[i].CapPts = 0ULL;
        DspYuvAltTimeoutInfo[i].CapSequence = 0ULL;
        DspYuvAltTimeoutInfo[i].YuvPts = 0ULL;
        DspYuvAltTimeoutInfo[i].YuvSequence = 0ULL;
        FuncRetCode = AmbaWrap_memset(&(DspYuvAltTimeoutInfo[i].Buffer), 0, sizeof(AMBA_DSP_YUV_IMG_BUF_s));
        if (FuncRetCode != OK_UL) {
            /* */
        }
        /* name */
        {
            char str[11];
            DspYuvAltTimeoutCb_Info[i].Name[0] = '\0';
            (void) svar_strcat(DspYuvAltTimeoutCb_Info[i].Name, "fov_");
            svar_utoa(i, str, 10U, 2U, (UINT32) SVAR_LEADING_ZERO);
            (void) svar_strcat(DspYuvAltTimeoutCb_Info[i].Name, str);
        }
        /* cb name */
        DspYuvAltTimeoutCb[i].pName = DspYuvAltTimeoutCb_Info[i].Name;
        /* cb info */
        DspYuvAltTimeoutCb[i].Param.pYuvAltRdy = &(DspYuvAltTimeoutInfo[i]);
        /* cb func */
        DspYuvAltTimeoutCb[i].pFunc = AmbaMonDsp_YuvAltTimeout;
    }
    /* dsp vout timeout */
    for (i = 0U; i < AMBA_MON_NUM_VOUT_CHANNEL; i++) {
        /* timeout info */
        DspVoutTimeoutInfo[i].VoutIdx = (UINT8) i;
        DspVoutTimeoutInfo[i].DispStartTime = 0ULL;
        DspVoutTimeoutInfo[i].DispDoneTime = 0ULL;
        FuncRetCode = AmbaWrap_memset(&(DspVoutTimeoutInfo[i].YuvBuf), 0, sizeof(AMBA_DSP_YUV_IMG_BUF_s));
        if (FuncRetCode != OK_UL) {
            /* */
        }
        /* name */
        {
            char str[11];
            DspVoutTimeoutCb_Info[i].Name[0] = '\0';
            (void) svar_strcat(DspVoutTimeoutCb_Info[i].Name, "vout_");
            svar_utoa(i, str, 10U, 1U, (UINT32) SVAR_LEADING_ZERO);
            (void) svar_strcat(DspVoutTimeoutCb_Info[i].Name, str);
        }
        /* cb name */
        DspVoutTimeoutCb[i].pName = DspVoutTimeoutCb_Info[i].Name;
        /* cb info */
        DspVoutTimeoutCb[i].Param.pVoutStatus = &(DspVoutTimeoutInfo[i]);
        /* cb func */
        DspVoutTimeoutCb[i].pFunc = AmbaMonDsp_VoutYuvTimeout;
    }
    /* vout isr timeout */
    for (i = 0U; i < AMBA_MON_NUM_VOUT_CHANNEL; i++) {
        /* timeout info */
        VoutIsrTimeoutInfo[i].VoutId = i;
        VoutIsrTimeoutInfo[i].Pts = 0ULL;
        VoutIsrTimeoutInfo[i].Dts = 0ULL;
        /* name */
        {
            char str[11];
            VoutIsrTimeoutCb_Info[i].Name[0] = '\0';
            (void) svar_strcat(VoutIsrTimeoutCb_Info[i].Name, "vout_");
            svar_utoa(i, str, 10U, 1U, (UINT32) SVAR_LEADING_ZERO);
            (void) svar_strcat(VoutIsrTimeoutCb_Info[i].Name, str);
        }
        /* cb name */
        VoutIsrTimeoutCb[i].pName = VoutIsrTimeoutCb_Info[i].Name;
        /* cb info */
        VoutIsrTimeoutCb[i].Param.pVoutIsrRdy = &(VoutIsrTimeoutInfo[i]);
        /* cb func */
        VoutIsrTimeoutCb[i].pFunc = AmbaMonVout_IsrTimeout;
    }

    return RetCode;
}
