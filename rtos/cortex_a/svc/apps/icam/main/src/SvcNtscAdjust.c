/**
 *  @file SvcNtscAdjust.c
 *
 * Copyright (c) [2020] Ambarella International LP
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
 * This file includes sample code and is only for internal testing and evaluation.  If you
 * distribute this sample code (whether in source, object, or binary code form), it will be
 * without any warranty or indemnity protection from Ambarella International LP or its affiliates.
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
 *
 */

#include "AmbaTypes.h"

#include "AmbaDSP_Capability.h"

#include "AmbaReg_VOUT.h"
#include "AmbaADC.h"
#include "AmbaCVBS.h"

#include "SvcLog.h"
#include "SvcErrCode.h"
#include "SvcPref.h"

#include "SvcUserPref.h"
#include "SvcResCfg.h"
#include "SvcGui.h"


#define NTSC_ADC_OUTPUT_TARGET 1088
#define ADC_THRESHOLD 5
#define ADC_VERIFY_NUMBER 256U
#define ADC_VERIFY_PASS_COUNT 128U
#define BASE_OUT_GAIN 0x300U

#define NTSC_ADJUST_LOG "NTSC_ADJUST"

static void NtscAdjust_Draw(UINT32 VoutIdx, UINT32 Level)
{
    extern UINT32 SvcOsd_DrawSolidRect(UINT32 Chan, UINT32 Xstart, UINT32 Ystart, UINT32 Xend, UINT32 Yend, UINT32 Color);
    AmbaMisra_TouchUnused(&VoutIdx);
    AmbaMisra_TouchUnused(&Level);
    (void) SvcOsd_DrawSolidRect(VoutIdx, 0, 0, 720, 480, 0xFFEBEBEB); //235
}

static void NtscAdjust_DrawUpdate(UINT32 VoutIdx, UINT32 Level, UINT32 *pUpdate)
{
    AmbaMisra_TouchUnused(&VoutIdx);
    AmbaMisra_TouchUnused(&Level);

    *pUpdate = 1U;
}

static void NtscAdjust_PrefSave(void)
{

    ULONG   PrefBufAddr;
    UINT32  PrefBufSize;
    SvcPref_GetPrefBuf(&PrefBufAddr, &PrefBufSize);
    if (SVC_OK != SvcPref_Save(PrefBufAddr, PrefBufSize)) {
        SvcLog_NG(NTSC_ADJUST_LOG, "SvcPref_Save failed!!", 0U, 0U);
    }

}

UINT32 SvcNtscAdjust_TuneAdcValue(void)
{
    SVC_USER_PREF_s *pSvcUserPref;
    const SVC_RES_CFG_s *pCfg = SvcResCfg_Get();
    UINT32 VoutID, HaveCvbsOut=0U;
    UINT32 Rval=SVC_OK;
    UINT32 OrgVal;

    for(VoutID=0U; VoutID<pCfg->DispNum; VoutID++){
        if(pCfg->DispStrm[VoutID].FrameRate.Interlace == 1U){
            HaveCvbsOut = 1U;
        }
    }
    if (HaveCvbsOut == 1U){

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
            volatile AMBA_VOUT_DISPLAY_C_CONFIG_REG_s *pRegAddr = pAmbaVoutDisplay2_Reg;
            VoutID = 1U;
#else

#if !defined(CONFIG_SOC_CV28)
            volatile AMBA_VOUT_DISPLAY1_CONFIG_REG_s *pRegAddr = pAmbaVoutDisplay1_Reg;
            VoutID = 1U;
#else
            volatile AMBA_VOUT_DISPLAY0_CONFIG_REG_s *pRegAddr = pAmbaVoutDisplay0_Reg;
            VoutID = 0U;
#endif

#endif

        if (SVC_OK == SvcUserPref_Get(&pSvcUserPref)) {
            if(pSvcUserPref->CvbsOutGain == 0U){
                SvcGui_Register(VoutID, 63U, "NTSC_ADC", NtscAdjust_Draw, NtscAdjust_DrawUpdate);
                AmbaKAL_TaskSleep(100U); // Wait gui works
                Rval = AmbaADC_Config(200000U);
                if(Rval == SVC_OK){
                    UINT32 i, j, count_success, TmpValue;
                    OrgVal = pRegAddr->AnalogCSC6.CoefA4; // Get default value;
                    for(i=BASE_OUT_GAIN; i<0xFFFFFFFFU; i++){
                        // SvcLog_OK(NTSC_ADJUST_LOG, "Gain Value:[%u]", i, 0U);
                        pRegAddr->AnalogCSC6.CoefA4 = i;
                        count_success=0U;
                        for(j=0; j<ADC_VERIFY_NUMBER; j++){
                            AmbaADC_SingleRead(1U, &TmpValue);
                            if((TmpValue >= (NTSC_ADC_OUTPUT_TARGET-ADC_THRESHOLD)) && (TmpValue <= (NTSC_ADC_OUTPUT_TARGET+ADC_THRESHOLD))){
                                count_success++;
                            }
                        }
                        if(count_success >= ADC_VERIFY_PASS_COUNT){
                            pSvcUserPref->CvbsOutGain = i;
                            NtscAdjust_PrefSave();
                            SvcLog_OK(NTSC_ADJUST_LOG, "Sucessfully find CVBS OutGain:[0x%x]", i, 0U);
                            break;
                        }
                    }
                    if(i == 0xFFFFFFFFU){
                        pRegAddr->AnalogCSC6.CoefA4 = OrgVal; // If cannot find suitable value, set to original value.
                        pSvcUserPref->CvbsOutGain = 0xFFFFFFFFU;
                        NtscAdjust_PrefSave();
                        SvcLog_NG(NTSC_ADJUST_LOG, "### No suitable CVBS OutGain value ###", 0U, 0U);
                    }
                } else {
                    SvcLog_NG(NTSC_ADJUST_LOG, "AmbaADC_Config(%d) failed 0x%x", VoutID, Rval);
                }
                SvcGui_Unregister(VoutID, 63U);
            } else {
                if(pSvcUserPref->CvbsOutGain != 0xFFFFFFFFU){
                    pRegAddr->AnalogCSC6.CoefA4 = pSvcUserPref->CvbsOutGain;
                    SvcLog_OK(NTSC_ADJUST_LOG, "Cvbs OutGain:[0x%x] is already trained", pSvcUserPref->CvbsOutGain, 0U);
                } else {
                    SvcLog_OK(NTSC_ADJUST_LOG, "Previous training is not successful", 0U, 0U);
                }
            }
        } else {
            SvcLog_NG(NTSC_ADJUST_LOG, "## SvcNtscAdjust_TuneAdcValue() failed with get preference error", 0U, 0U);
        }
    }
    return Rval;
}