/**
 * @file Amba_ImgProcTest.c
 *
 *  Copyright (c) 2020 Ambarella International LP
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
 *  @Description    :: Image Proc Test Function Implementation
 *
 *
 */




#include "AmbaWrap.h"
#include "AmbaUtility.h"
#include "AmbaImg_External_CtrlFunc.h"
#include "AmbaTypes.h"
#include "AmbaPrint.h"
#include "AmbaKAL.h"

#include "AmbaDSP_ImageFilter.h"
#include "AmbaImg_AaaDef.h"
#include "AmbaImg_Proc.h"
#include "AmbaKAL.h"
#include "AmbaShell.h"

#include "AmbaIQParamHandlerSample.h"
#include "Amba_AeAwbAdj_Control.h"
#include "Amba_AwbCalib.h"
#include "Amba_Image.h"
#include "Amba_AdjTableSelectSample.h"

#if defined (CONFIG_BUILD_IP_MONITOR)
#include "Amba_IPMonitor.h"
#endif

#ifdef CONFIG_BUILD_COMMON_SERVICE_AF
#include "AmbaAf_AfProc.h"
#endif

void AmpUT_ImgProcTest(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);

static SCENE_DATA_s SceneDataTmp __attribute__((section(".bss.noinit")));

extern UINT32 IpctestChno;

#if 0
static void Amba_Detect5060Hz(void)
{
        FLICKER_DETECT_STATUS_s status;

        Img_Get_Flicker_Detection_Status(&status);
        Img_Flicker_Detection_Enable(1);
        status.Running = RUNNING;
        while (status.Running == RUNNING) {
            Img_Get_Flicker_Detection_Status(&status);
            AmbaKAL_TaskSleep(100);
        }
        AmbaPrint("------------flicker : %dHz------------",status.FlickerHz);

}
#endif

static DOUBLE Ssample_Me_Bias(UINT32 target)
{
    static AMBA_AE_TILES_INFO_s AeTileInfoTmp __attribute__((section(".bss.noinit")));
    static const UINT32 MeteringTable[96] =
         { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  //AE_METER_CENTER
           1, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 1,
           1, 1, 1, 1, 2, 3, 3, 2, 1, 1, 1, 1,
           1, 1, 1, 2, 3, 5, 5, 3, 2, 1, 1, 1,
           1, 1, 1, 2, 3, 5, 5, 3, 2, 1, 1, 1,
           1, 1, 2, 3, 4, 5, 5, 4, 3, 2, 1, 1,
           1, 2, 3, 4, 4, 4, 4, 4, 4, 3, 2, 1,
           2, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 2};

    UINT32 LumaStat = 0, Weight = 0, U32RVal = 0U;
    UINT16 i = 0, Total = 0;
    DOUBLE Bias = 0.0;

    U32RVal |= AmbaWrap_memset(&AeTileInfoTmp, 0, sizeof(AeTileInfoTmp));
    U32RVal |= AmbaImgProc_GetAETileInfo(0, 0, &AeTileInfoTmp);
    Total = AeTileInfoTmp.Rows * AeTileInfoTmp.Cols;

    for(i = 0;i < Total;i++){
        LumaStat += (UINT32)AeTileInfoTmp.TilesValue[i] * MeteringTable[i];
        Weight += MeteringTable[i];
    }
    if(Weight != 0U){
        LumaStat = (LumaStat / Weight) >> 4 ;
    }
    if(target != 0U){
        U32RVal |= AmbaWrap_log2(((DOUBLE)LumaStat/(DOUBLE)target), &Bias);
    }
    if(U32RVal != 0U){
        /* MisraC */
    }
    AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "target : %d, LumaStat : %d", target, LumaStat, 0U, 0U, 0U);
    // AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "target : %d, LumaStat : %d, EvBias : %f", target, LumaStat, Bias);
    return (Bias);
}

static void Ssample_Sc(UINT32 chNo, INT32 sceneMode){

    MULTI_SCENE_MODE_s MultiScMode;

    (void)Amba_IQParam_Load_CC_By_Scene(chNo, sceneMode);
    MultiScMode.VinNum = (UINT8)chNo;
    MultiScMode.SceneMode = &sceneMode;

    MultiScMode.Mode = (INT32)IP_MODE_VIDEO;
    (void)AmbaImgProc_SCSetSceneMode(MultiScMode.VinNum, (UINT32)MultiScMode.Mode, MultiScMode.SceneMode);

    MultiScMode.Mode = (INT32)IP_MODE_STILL;
    (void)AmbaImgProc_SCSetSceneMode(MultiScMode.VinNum, (UINT32)MultiScMode.Mode, MultiScMode.SceneMode);

    (void)AmbaImgProc_AEInit(chNo, 0U);
    (void)AmbaImgProc_AWBInit(chNo, 0U);

}

static void Ssample_ScInfo_Sht(UINT32 chNo, INT32 sceneMode, UINT16 ShtTmp){
    INT32 SceneModeTmp = 0;
    SceneModeTmp = sceneMode;

    (void)AmbaImgProc_SCGetSceneModeInfo(chNo, SceneModeTmp, &SceneDataTmp);
    SceneDataTmp.AeControl.Video.DefExp[1] = ShtTmp;
    SceneDataTmp.AeControl.Video.DefExp[2] = ShtTmp;

    SceneDataTmp.AeControl.Photo.DefExp[1] = ShtTmp;
    SceneDataTmp.AeControl.Photo.DefExp[2] = ShtTmp;
    (void)AmbaImgProc_SCSetSceneModeInfo(chNo, SceneModeTmp, &SceneDataTmp);
}

static void Ssample_ScInfo_ISO(UINT32 chNo, INT32 sceneMode, UINT16 IsoTmp){
    INT32 SceneModeTmp = 0;
    SceneModeTmp = sceneMode;

    (void)AmbaImgProc_SCGetSceneModeInfo(chNo, SceneModeTmp, &SceneDataTmp);
    SceneDataTmp.AeControl.Video.DefExp[0] = IsoTmp;
    SceneDataTmp.AeControl.Photo.DefExp[1] = IsoTmp;
    (void)AmbaImgProc_SCSetSceneModeInfo(chNo, SceneModeTmp, &SceneDataTmp);
}

//Start of Compllexity
static void AmpUT_ImgProcTest_Sub111111(UINT32 ArgCount, char * const * pArgVector)
{
    static char *   ArgvArray[64];     // A large argv array
//    static UINT32   AmbaSampleChNo = 0;
    static char     Zero[2];            // A string of "0"
    UINT32          T = 0;
//    UINT32          U32Val = 0;
    UINT32          U32RVal = 0U;

    Zero[0] = '0';
    Zero[1] = '\0';
    for (T = 0U; T < 64U; ++T) {
        if (T < ArgCount) {
            ArgvArray[T] = pArgVector[T];
        } else {
            ArgvArray[T] = Zero;
        }
    }
//
    if (AmbaUtility_StringCompare(ArgvArray[1],"-stopvin", 8U) == 0) {
#if defined(CONFIG_THREADX)
        extern UINT32 AmbaDSP_Stop(UINT32 CmdType);
        (void)AmbaDSP_Stop(0U);
        (void)AmbaDSP_Stop(1U);
#else
        AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "-stopvin n/a", 0U, 0U, 0U, 0U, 0U);
#endif
    } else if (AmbaUtility_StringCompare(ArgvArray[1],"-offline", 8U) == 0) {
        extern void ImgToolPrtDouble(DOUBLE prtVlu0,DOUBLE prtVlu1,DOUBLE prtVlu2,DOUBLE prtVlu3,DOUBLE prtVlu4, UINT32 prtNum);
        AMBA_IMGPROC_OFFLINE_AAA_INFO_s offlineAAAInfo;


        (void)AmbaImgProc_GetOfflineAAAInfo(0U, &offlineAAAInfo);
        AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "[before HDR AeInfo] evindex: %d nfindex: %d", offlineAAAInfo.BeforeHdrAEInfo[0].EvIndex, offlineAAAInfo.BeforeHdrAEInfo[0].NfIndex, 0U, 0U, 0U);
        AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "[After HDR AeInfo] Dgain: %d nfindex: %d", (UINT32)offlineAAAInfo.AfterHdrAEInfo[0].Dgain, offlineAAAInfo.AfterHdrAEInfo[0].NfIndex, 0U, 0U, 0U);
        AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "[After HDR AeInfo] Dgain: %d nfindex: %d", (UINT32)offlineAAAInfo.AfterHdrAEInfo[1].Dgain, offlineAAAInfo.AfterHdrAEInfo[1].NfIndex, 0U, 0U, 0U);
        AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "[After HDR AeInfo] Dgain: %d nfindex: %d", (UINT32)offlineAAAInfo.AfterHdrAEInfo[2].Dgain, offlineAAAInfo.AfterHdrAEInfo[2].NfIndex, 0U, 0U, 0U);
        AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "[WbGain] %d %d %d", offlineAAAInfo.WbGain.GainR, offlineAAAInfo.WbGain.GainG, offlineAAAInfo.WbGain.GainB, 0U, 0U);
        AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "[ShiftAeGain, BlendRatio]", NULL, NULL, NULL, NULL, NULL);
        ImgToolPrtDouble(offlineAAAInfo.ShiftAeGain, offlineAAAInfo.BlendRatio, 0.0, 0.0, 0.0, 2);
        AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "[MinBlendRatio: %d, MaxBlendRatio: %d]", offlineAAAInfo.MinBlendRatio, offlineAAAInfo.MaxBlendRatio, 0U, 0U, 0U);

    } else if (AmbaUtility_StringCompare(ArgvArray[1],"defog", 5U) == 0) {
        UINT32 ViewID = 0U;
        static UINT32 defogEnable = 0U;
        INT32 defogSceneMode = SCENE_D_LIGHTING;
        INT32 oriSceneMode = SCENE_OFF;

        if (ArgCount > 2U) {
            (void)AmbaUtility_StringToUInt32(ArgvArray[2], &ViewID);

            defogEnable ^= 1U;

            if (defogEnable == 1U) {

                AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Defog ON", NULL, NULL, NULL, NULL, NULL);

                //switch to defog specitic adj table
                (void)Amba_Set_MADJ(ViewID, 1, 2);
                //set scene mode
                (void)AmbaImgProc_SCSetSceneMode(ViewID, IP_MODE_VIDEO, &defogSceneMode);
                (void)AmbaImgProc_SCSetSceneMode(ViewID, IP_MODE_STILL, &defogSceneMode);
            } else {
                AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Defog OFF", NULL, NULL, NULL, NULL, NULL);

                //switch to defog specitic adj table
                (void)Amba_Set_MADJ(ViewID, 0, 0);
                //set scene mode
                (void)AmbaImgProc_SCSetSceneMode(ViewID, IP_MODE_VIDEO, &oriSceneMode);
                (void)AmbaImgProc_SCSetSceneMode(ViewID, IP_MODE_STILL, &oriSceneMode);
            }
        } else {
            AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "defog [chNo]", NULL, NULL, NULL, NULL, NULL);
        }
    } else if (AmbaUtility_StringCompare(ArgvArray[1],"-adjtable", 5U) == 0) {
        UINT32 ViewID = 0U;
        UINT8 AdjTableNo;
        UINT32 ModeDef;
        (void)AmbaUtility_StringToUInt32(ArgvArray[2], &ViewID);
        Amba_ADJGetVideoTable((UINT8)ViewID, &AdjTableNo,&ModeDef);
        AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "video_adj_table : %d, video_mode_def : %d\n",AdjTableNo, ModeDef, 0U, 0U, 0U);
    } else {
        AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "Usage: t imgproc", 0U, 0U, 0U, 0U, 0U);
        AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "         -ver", 0U, 0U, 0U, 0U, 0U);
        AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "         -3a         [on|off]", 0U, 0U, 0U, 0U, 0U);
        AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "         -3a ver", 0U, 0U, 0U, 0U, 0U);
        AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "         -ae         [on|off]", 0U, 0U, 0U, 0U, 0U);
        AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "         -ae info    [chNo]", 0U, 0U, 0U, 0U, 0U);
        AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "         -awb        [on|off]", 0U, 0U, 0U, 0U, 0U);
        AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "         -awb info   [chNo]", 0U, 0U, 0U, 0U, 0U);
        AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "         -adj        [on|off]", 0U, 0U, 0U, 0U, 0U);
        AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "         -madj       [Enable][AdjTableNo][ChNo]", 0U, 0U, 0U, 0U, 0U);
        AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "         -exif_dbg   [0|1]", 0U, 0U, 0U, 0U, 0U);
        AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "         -me         [chNo][Video|Still : 0|1] [ShtTime : us] [Gain : 1x 1000] [Dgain : Unit 4096]", 0U, 0U, 0U, 0U, 0U);
        AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "         -mwb        [chNo][Video|Still : 0|1] [GainR] [GainG] [GainB], Unit 4096", 0U, 0U, 0U, 0U, 0U);
        AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "         -slowshutter[Video|Photo : 0|1] [SlowShutterEnable, 0:1X, 1:2X, 2:4X ...]", 0U, 0U, 0U, 0U, 0U);
        AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "         -sharp      [Level : 0~6]", 0U, 0U, 0U, 0U, 0U);
    }
//
    if(U32RVal != 0U){
        /* MisraC */
    }
}

static void AmpUT_ImgProcTest_Sub11111(UINT32 ArgCount, char * const * pArgVector)
{
    static char *   ArgvArray[64];     // A large argv array
//    static UINT32   AmbaSampleChNo = 0;
    static char     Zero[2];            // A string of "0"
    UINT32          T = 0;
    //UINT32          U32Val = 0;
    UINT32          U32RVal = 0U;

    Zero[0] = '0';
    Zero[1] = '\0';
    for (T = 0U; T < 64U; ++T) {
        if (T < ArgCount) {
            ArgvArray[T] = pArgVector[T];
        } else {
            ArgvArray[T] = Zero;
        }
    }
//
    if(AmbaUtility_StringCompare(ArgvArray[1],"-afstat", 7U) == 0){
        UINT32                      ChNo = 0;
        static AMBA_IK_CFA_3A_DATA_s       pCFAStat;
        static AMBA_IK_PG_3A_DATA_s        pRgbStat;

        static UINT32 LT[4U] =  { 1, 2, 13, 14};
        static UINT32 RT[4U] =  { 9, 10, 21, 22};
        const UINT32 LTM[4U] = {27, 28, 39, 40};
        const UINT32 RTM[4U] = {31, 32, 43, 44};
        const UINT32 CT[4U] =  {41, 42, 53, 54};
        const UINT32 LBM[4U] = {51, 52, 63, 64};
        const UINT32 RBM[4U] = {55, 56, 67, 68};
        static UINT32 LB[4U] =  {73, 74, 85, 86};
        static UINT32 RB[4U] =  {81, 82, 93, 94};
        UINT32 LT1 = 1, RT1 = 9, LB1 = 73, RB1 = 81;

        UINT32 stat[9U] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
        char prtbuf[60U];
        UINT8 cnt = 0U;

        if(AmbaUtility_StringCompare(ArgvArray[2],"set", 3U) == 0){
            (void)AmbaUtility_StringToUInt32(ArgvArray[3], &LT1);
            (void)AmbaUtility_StringToUInt32(ArgvArray[4], &RT1);
            (void)AmbaUtility_StringToUInt32(ArgvArray[5], &LB1);
            (void)AmbaUtility_StringToUInt32(ArgvArray[6], &RB1);
            LT[0] = LT1; LT[1] = LT1+1U; LT[2] = LT1+12U; LT[3] = LT1+13U;
            RT[0] = RT1; RT[1] = RT1+1U; RT[2] = RT1+12U; RT[3] = RT1+13U;
            LB[0] = LB1; LB[1] = LB1+1U; LB[2] = LB1+12U; LB[3] = LB1+13U;
            RB[0] = RB1; RB[1] = RB1+1U; RB[2] = RB1+12U; RB[3] = RB1+13U;
            AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "LT tiles %d %d %d %d", LT[0], LT[1], LT[2], LT[3], 0U);
            AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "RT tiles %d %d %d %d", RT[0], RT[1], RT[2], RT[3], 0U);
            AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "LB tiles %d %d %d %d", LB[0], LB[1], LB[2], LB[3], 0U);
            AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "RB tiles %d %d %d %d", RB[0], RB[1], RB[2], RB[3], 0U);

        }else if(AmbaUtility_StringCompare(ArgvArray[2],"cfa", 3U) == 0){
            (void)AmbaImgProc_GetCFAAAAStat(ChNo, &pCFAStat);

            if(AmbaUtility_StringCompare(ArgvArray[3],"fv1", 3U) == 0){
                for(cnt = 0U; cnt < 4U; cnt++){
                    stat[0] += pCFAStat.Af[ LT[cnt]].SumFV1;
                    stat[1] += pCFAStat.Af[ RT[cnt]].SumFV1;
                    stat[2] += pCFAStat.Af[LTM[cnt]].SumFV1;
                    stat[3] += pCFAStat.Af[RTM[cnt]].SumFV1;
                    stat[4] += pCFAStat.Af[ CT[cnt]].SumFV1;
                    stat[5] += pCFAStat.Af[LBM[cnt]].SumFV1;
                    stat[6] += pCFAStat.Af[RBM[cnt]].SumFV1;
                    stat[7] += pCFAStat.Af[ LB[cnt]].SumFV1;
                    stat[8] += pCFAStat.Af[ RB[cnt]].SumFV1;
                }
                (void)AmbaUtility_StringPrintUInt32(prtbuf, 60U, "@@ %5d %5d %5d %5d %5d %5d %5d %5d %5d @@",9U, stat);
                AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID,prtbuf, NULL, NULL, NULL, NULL, NULL);

            }else if(AmbaUtility_StringCompare(ArgvArray[3],"fv2", 3U) == 0){
                for(cnt = 0U; cnt < 4U; cnt++){
                    stat[0] += pCFAStat.Af[ LT[cnt]].SumFV2;
                    stat[1] += pCFAStat.Af[ RT[cnt]].SumFV2;
                    stat[2] += pCFAStat.Af[LTM[cnt]].SumFV2;
                    stat[3] += pCFAStat.Af[RTM[cnt]].SumFV2;
                    stat[4] += pCFAStat.Af[ CT[cnt]].SumFV2;
                    stat[5] += pCFAStat.Af[LBM[cnt]].SumFV2;
                    stat[6] += pCFAStat.Af[RBM[cnt]].SumFV2;
                    stat[7] += pCFAStat.Af[ LB[cnt]].SumFV2;
                    stat[8] += pCFAStat.Af[ RB[cnt]].SumFV2;
                }
                (void)AmbaUtility_StringPrintUInt32(prtbuf, 60U, "@@ %5d %5d %5d %5d %5d %5d %5d %5d %5d @@",9U, stat);
                AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID,prtbuf, NULL, NULL, NULL, NULL, NULL);
            }else{/*MISRAC*/}

        }else if(AmbaUtility_StringCompare(ArgvArray[2],"rgb", 3U) == 0){
            (void)AmbaImgProc_GetPGAAAStat(ChNo, &pRgbStat);
            if(AmbaUtility_StringCompare(ArgvArray[3],"fv1", 3U) == 0){
                for(cnt = 0U; cnt < 4U; cnt++){
                    stat[0] += pRgbStat.Af[ LT[cnt]].SumFV1;
                    stat[1] += pRgbStat.Af[ RT[cnt]].SumFV1;
                    stat[2] += pRgbStat.Af[LTM[cnt]].SumFV1;
                    stat[3] += pRgbStat.Af[RTM[cnt]].SumFV1;
                    stat[4] += pRgbStat.Af[ CT[cnt]].SumFV1;
                    stat[5] += pRgbStat.Af[LBM[cnt]].SumFV1;
                    stat[6] += pRgbStat.Af[RBM[cnt]].SumFV1;
                    stat[7] += pRgbStat.Af[ LB[cnt]].SumFV1;
                    stat[8] += pRgbStat.Af[ RB[cnt]].SumFV1;
                }
                (void)AmbaUtility_StringPrintUInt32(prtbuf, 60U, "@@ %5d %5d %5d %5d %5d %5d %5d %5d %5d @@",9U, stat);
                AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID,prtbuf, NULL, NULL, NULL, NULL, NULL);
            }else if(AmbaUtility_StringCompare(ArgvArray[3],"fv2", 3U) == 0){
                for(cnt = 0U; cnt < 4U; cnt++){
                    stat[0] += pRgbStat.Af[ LT[cnt]].SumFV2;
                    stat[1] += pRgbStat.Af[ RT[cnt]].SumFV2;
                    stat[2] += pRgbStat.Af[LTM[cnt]].SumFV2;
                    stat[3] += pRgbStat.Af[RTM[cnt]].SumFV2;
                    stat[4] += pRgbStat.Af[ CT[cnt]].SumFV2;
                    stat[5] += pRgbStat.Af[LBM[cnt]].SumFV2;
                    stat[6] += pRgbStat.Af[RBM[cnt]].SumFV2;
                    stat[7] += pRgbStat.Af[ LB[cnt]].SumFV2;
                    stat[8] += pRgbStat.Af[ RB[cnt]].SumFV2;
                }
                (void)AmbaUtility_StringPrintUInt32(prtbuf, 60U, "@@ %5d %5d %5d %5d %5d %5d %5d %5d %5d @@",9U, stat);
                AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID,prtbuf, NULL, NULL, NULL, NULL, NULL);
            }else{/*MISRAC*/}
#ifdef CONFIG_BUILD_COMMON_SERVICE_AF
        }else if(AmbaUtility_StringCompare(ArgvArray[2],"proc", 4U) == 0){
            UINT32 FuncRetCode;
            (void)AmbaImgProc_GetCFAAAAStat(ChNo, &pCFAStat);
            (void)AmbaImgProc_GetPGAAAStat(ChNo, &pRgbStat);
            FuncRetCode = AmbaAf_AfProcess(&pCFAStat, &pRgbStat);
            if (FuncRetCode != AF_ERR_NONE) {
                /* */
            }
#endif
        }else{
                AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "imgproc -afstat set [LT[0]] [RT[0]] [LB[0]] [RB[0]]", 0U, 0U, 0U, 0U, 0U);
                AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "imgproc -afstat cfa [fv1/fv2]", 0U, 0U, 0U, 0U, 0U);
                AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "imgproc -afstat rgb [fv1/fv2]", 0U, 0U, 0U, 0U, 0U);
        }
    } else if (AmbaUtility_StringCompare(ArgvArray[1],"-prt", 4U) == 0) {
        if (AmbaUtility_StringCompare(ArgvArray[2],"enable", 6U) == 0) {
            (void)AmbaPrint_ModuleSetAllowList(IMGPROC_PRINT_MODULE_ID, 1U);
        }
        if (AmbaUtility_StringCompare(ArgvArray[2],"disable", 7U) == 0) {
            (void)AmbaPrint_ModuleSetAllowList(IMGPROC_PRINT_MODULE_ID, 0U);
        }
        if (AmbaUtility_StringCompare(ArgvArray[2],"flush", 5U) == 0) {
            AmbaPrint_StopAndFlush();
        }
    } else {
        AmpUT_ImgProcTest_Sub111111(ArgCount, pArgVector); //Sub111111
    } //6

//
    if(U32RVal != 0U){
        /* MisraC */
    }
}

static void AmpUT_ImgProcTest_Sub1111(UINT32 ArgCount, char * const * pArgVector)
{
    static char *   ArgvArray[64];     // A large argv array
//    static UINT32   AmbaSampleChNo = 0;
    static char     Zero[2];            // A string of "0"
    UINT32          T = 0;
//    UINT32          U32Val = 0;
    UINT32          U32RVal = 0U;

    Zero[0] = '0';
    Zero[1] = '\0';
    for (T = 0U; T < 64U; ++T) {
        if (T < ArgCount) {
            ArgvArray[T] = pArgVector[T];
        } else {
            ArgvArray[T] = Zero;
        }
    }
//
    if (AmbaUtility_StringCompare(ArgvArray[1],"-de", 3U) == 0) {
        IMGPROC_TESTCMD_PARAM_s testCMDParam;
        extern UINT32 AmbaImgProc_TestCMD(UINT32 TestCMDType, const IMGPROC_TESTCMD_PARAM_s *pTestCMDParam);

        testCMDParam.ArgCount = (INT32)ArgCount;
        testCMDParam.ArgvArray = ArgvArray;

        (void)AmbaImgProc_TestCMD(4U , &testCMDParam); /*4U for DE test command*/
#if defined (CONFIG_BUILD_IP_MONITOR)
    } else if (AmbaUtility_StringCompare(ArgvArray[1],"-ipc", 4U) == 0) {
      (void)Ipc_Test_Command((INT32)ArgCount, ArgvArray);
#endif
    } else if (AmbaUtility_StringCompare(ArgvArray[1],"-gaaa", 5U) == 0) {
        UINT8                       GAAAEnb;
        UINT32                      ViewID = 0U;
        AMBA_IP_GLOBAL_AAA_DATA_s   *pGAAAData = NULL;

        if (AmbaUtility_StringCompare(ArgvArray[2],"show", 4U) == 0) {

            (void)AmbaUtility_StringToUInt32(ArgvArray[3], &ViewID);
            (void)AmbaImgProc_GAAAGetEnb(ViewID, &GAAAEnb);
            (void)AmbaImgProc_GAAAGetStat(ViewID, &pGAAAData);

            AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "GAAAEnb [%d]", GAAAEnb, 0U, 0U, 0U, 0U);
            AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "StatCount [%d]", pGAAAData->StatCount, 0U, 0U, 0U, 0U);
        }
    } else if (AmbaUtility_StringCompare(ArgvArray[1],"-aaadump", 8U) == 0) {
        static AAA_MEM_LOG         MemLog = {0};
        UINT32              CurrIdx;
        UINT32              i = 0U, j = 0U;

        (void)AmbaImgProc_GetAAAMEMLog(0U, &MemLog);
        CurrIdx = MemLog.CurrIdx;

        for (i=CurrIdx+1U; i<AAA_MEM_LOG_LEN; i++) {
            AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "%d    GainR %d", j, MemLog.AWBInfoVideo[i].GainR, 0U, 0U, 0U);
            j++;
        }
        for (i=0U; i<=CurrIdx; i++) {
            AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "%d    GainR %d", j, MemLog.AWBInfoVideo[i].GainR, 0U, 0U, 0U);
            j++;
        }
    } else if (AmbaUtility_StringCompare(ArgvArray[1],"-wb", 3U) == 0) {
        if (AmbaUtility_StringCompare(ArgvArray[2],"calib", 5U) == 0) {
            UINT32              ViewID = 0;
            UINT32              Index = 0;
            UINT32              FlickerMode = 60;
            WB_Detect_Info_s    WbDetectInfo = {0U, 0U, 0U, 0U, 0U, 0U};

            (void)AmbaUtility_StringToUInt32(ArgvArray[2], &ViewID);
            (void)AmbaUtility_StringToUInt32(ArgvArray[3], &Index);
            (void)AmbaUtility_StringToUInt32(ArgvArray[4], &FlickerMode);

            (void)ImgProc_WBCal(ViewID, Index, FlickerMode, &WbDetectInfo);

        }
#if 0
        else if (AmbaUtility_StringCompare(ArgvArray[2],"comp", 4U) == 0) {
            UINT32 ViewID = 0;
            AMBA_IK_WB_GAIN_s TargetAvg, SetAvg, unitWB = {WB_UNIT_GAIN, WB_UNIT_GAIN, WB_UNIT_GAIN};
            SetAvg.GainR = 3000;
            SetAvg.GainG = 4096;
            SetAvg.GainB = 3000;

            TargetAvg.GainR = 2000;
            TargetAvg.GainG = 4096;
            TargetAvg.GainB = 3000;

            (void)AmbaImgProc_AWBSetWBCompSetInfo(ViewID, &SetAvg, &SetAvg);
            (void)AmbaImgProc_AWBSetWBCompTargetInfo(ViewID, &TargetAvg, &TargetAvg);

            (void)ImgProc_WbCalibCalculateWbCompInfo(ViewID);

            (void)ImgProc_WBCalibSetToTarget(ViewID,  &unitWB);
            AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "%d %d", unitWB.GainR, unitWB.GainB, 0U, 0U, 0U);
        } else {
            // MISRAC
        }
#endif
    }else {
        AmpUT_ImgProcTest_Sub11111(ArgCount, pArgVector); //Sub11111
    } //5

//
    if(U32RVal != 0U){
        /* MisraC */
    }
}
static void AmpUT_ImgProcTest_Sub111(UINT32 ArgCount, char * const * pArgVector)
{
    static char *   ArgvArray[64];     // A large argv array
//    static UINT32   AmbaSampleChNo = 0;
    static char     Zero[2];            // A string of "0"
    UINT32          T = 0;
    UINT32          U32Val = 0;
    UINT32          U32RVal = 0U;

    Zero[0] = '0';
    Zero[1] = '\0';
    for (T = 0U; T < 64U; ++T) {
        if (T < ArgCount) {
            ArgvArray[T] = pArgVector[T];
        } else {
            ArgvArray[T] = Zero;
        }
    }

//
    if (AmbaUtility_StringCompare(ArgvArray[1],"-scenedata", 10U) == 0) {
        UINT32 SceneModeTmp = 0;
        UINT16 IsoTmp = 0;
        UINT16 ShtTmp = 0;


        (void)AmbaUtility_StringToUInt32(ArgvArray[2], &U32Val);
        SceneModeTmp = (UINT32)U32Val;
        (void)AmbaUtility_StringToUInt32(ArgvArray[3], &U32Val);
        IsoTmp = (UINT16)U32Val;
        (void)AmbaUtility_StringToUInt32(ArgvArray[4], &U32Val);
        ShtTmp = (UINT16)U32Val;

        (void)AmbaImgProc_SCGetSceneModeInfo(0, (INT32)SceneModeTmp, &SceneDataTmp);
        SceneDataTmp.AeControl.Video.DefExp[0] = IsoTmp;
        SceneDataTmp.AeControl.Video.DefExp[1] = ShtTmp;
        SceneDataTmp.AeControl.Video.DefExp[2] = ShtTmp;

        SceneDataTmp.AeControl.Photo.DefExp[0] = IsoTmp;
        SceneDataTmp.AeControl.Photo.DefExp[1] = ShtTmp;
        SceneDataTmp.AeControl.Photo.DefExp[2] = ShtTmp;
        AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "IsoTmp : %d, ShtTmp : %d", IsoTmp, ShtTmp, 0U, 0U, 0U);
        (void)AmbaImgProc_SCSetSceneModeInfo(0U, (INT32)SceneModeTmp, &SceneDataTmp);

    } else if (AmbaUtility_StringCompare(ArgvArray[1],"-chgiqparam", 11U) == 0) {
        //extern UINT32 App_Image_Init_Iq_Params(UINT32 chNo, INT32 sensorID);
        UINT32 chNo = 0;
        INT32 sensorID = 0;

        (void)AmbaUtility_StringToUInt32(ArgvArray[2], &U32Val);
        sensorID = (INT32)U32Val;
        (void)App_Image_Init_Iq_Params(chNo, sensorID);
    } else if (AmbaUtility_StringCompare(ArgvArray[1],"-wbpos", 6U) == 0) {
        //UINT32 AmbaImgProc_AWBSetWBPosition(UINT32 ViewID, UINT32 WBPosition);

        UINT32 ViewID = 0;
        UINT32 WBPosition = 0;

        (void)AmbaUtility_StringToUInt32(ArgvArray[2], &U32Val);
        ViewID = U32Val;
        (void)AmbaUtility_StringToUInt32(ArgvArray[3], &U32Val);
        WBPosition = U32Val;
        AmbaPrint_PrintUInt5("WB position 16:snesor wb, 32:front end wb , 48: before CE wb", 0U, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("ViewID: %d, WB position : %d", ViewID, WBPosition, 0U, 0U, 0U);
        (void)AmbaImgProc_AWBSetWBPosition(ViewID, WBPosition);


    } else if (AmbaUtility_StringCompare(ArgvArray[1], "-motionspeed", 12U) == 0){

        UINT32 ChannelNo;
        INT32 motionSpeed;
        ADJ_MOTION_SPEED_THRESHOLD_s TestThreshold;

        (void)AmbaUtility_StringToUInt32(ArgvArray[2], &U32Val);
        ChannelNo = (UINT32)U32Val;
        (void)AmbaUtility_StringToUInt32(ArgvArray[3], &U32Val);
        motionSpeed = (INT32)U32Val;
        (void)AmbaUtility_StringToUInt32(ArgvArray[4], &U32Val);
        TestThreshold.MinThreshold = (INT32)U32Val;
        (void)AmbaUtility_StringToUInt32(ArgvArray[5], &U32Val);
        TestThreshold.MaxThreshold = (INT32)U32Val;

        (void)AmbaImgProc_ADJSetMSpeed(ChannelNo, motionSpeed);
        (void)AmbaImgProc_ADJSetMSpeedThresh(ChannelNo, &TestThreshold);


    } else if (AmbaUtility_StringCompare(ArgvArray[1],"-adjblbs", 8U) == 0) {
        AMBA_BLACK_CORRECTION_s     blackLevelBase[3];
        UINT8                       Mode = IP_MODE_VIDEO;
        UINT32                      ChNo = 0;

        //Video
        Mode = IP_MODE_VIDEO;
        (void)AmbaImgProc_ADJGetBlackLevelBase(ChNo, Mode, blackLevelBase);
        AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "Video", 0U, 0U, 0U, 0U, 0U);
        AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "BlackR: %d, BlackGr: %d, BlackGb: %d, BlackB: %d",
                (UINT32)blackLevelBase[0].BlackR, (UINT32)blackLevelBase[0].BlackGr, (UINT32)blackLevelBase[0].BlackGb, (UINT32)blackLevelBase[0].BlackB, 0U);
        //Still
        Mode = IP_MODE_STILL;
        AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "Still", 0U, 0U, 0U, 0U, 0U);
        (void)AmbaImgProc_ADJGetBlackLevelBase(ChNo, Mode, blackLevelBase);
        AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "BlackR: %d, BlackGr: %d, BlackGb: %d, BlackB: %d",
            (UINT32)blackLevelBase[0].BlackR, (UINT32)blackLevelBase[0].BlackGr, (UINT32)blackLevelBase[0].BlackGb, (UINT32)blackLevelBase[0].BlackB, 0U);

    } else if (AmbaUtility_StringCompare(ArgvArray[1],"-ae", 3U) == 0) {
        IMGPROC_TESTCMD_PARAM_s testCMDParam;
        extern UINT32 AmbaImgProc_TestCMD(UINT32 TestCMDType, const IMGPROC_TESTCMD_PARAM_s *pTestCMDParam);

        testCMDParam.ArgCount = (INT32)ArgCount;
        testCMDParam.ArgvArray = ArgvArray;


        (void)AmbaImgProc_TestCMD(0U , &testCMDParam); /*0U for AE test command*/

    } else if (AmbaUtility_StringCompare(ArgvArray[1],"-awb", 4U) == 0) {
        IMGPROC_TESTCMD_PARAM_s testCMDParam;
        extern UINT32 AmbaImgProc_TestCMD(UINT32 TestCMDType, const IMGPROC_TESTCMD_PARAM_s *pTestCMDParam);

        testCMDParam.ArgCount = (INT32)ArgCount;
        testCMDParam.ArgvArray = ArgvArray;

        (void)AmbaImgProc_TestCMD(1U , &testCMDParam); /*1U for AWB test command*/

    } else if (AmbaUtility_StringCompare(ArgvArray[1],"-adj", 4U) == 0) {
        IMGPROC_TESTCMD_PARAM_s testCMDParam;
        extern UINT32 AmbaImgProc_TestCMD(UINT32 TestCMDType, const IMGPROC_TESTCMD_PARAM_s *pTestCMDParam);

        testCMDParam.ArgCount = (INT32)ArgCount;
        testCMDParam.ArgvArray = ArgvArray;

        (void)AmbaImgProc_TestCMD(2U , &testCMDParam); /*2U for ADJ test command*/

    } else if (AmbaUtility_StringCompare(ArgvArray[1],"-sc", 3U) == 0) {
        IMGPROC_TESTCMD_PARAM_s testCMDParam;
        extern UINT32 AmbaImgProc_TestCMD(UINT32 TestCMDType, const IMGPROC_TESTCMD_PARAM_s *pTestCMDParam);

        testCMDParam.ArgCount = (INT32)ArgCount;
        testCMDParam.ArgvArray = ArgvArray;

        (void)AmbaImgProc_TestCMD(3U , &testCMDParam); /*3U for SC test command*/

    } else if (AmbaUtility_StringCompare(ArgvArray[1],"-deset", 6U) == 0) {

        extern UINT32 AmbaImgProc_TestCMD(UINT32 TestCMDType, const IMGPROC_TESTCMD_PARAM_s *pTestCMDParam);
        UINT32 ViewID;
        INT32  DeMode;
        AMBA_IK_MODE_CFG_s ModeCfg;

        U32RVal |= AmbaWrap_memset(&ModeCfg, 0, sizeof(AMBA_IK_MODE_CFG_s));

        (void)AmbaUtility_StringToUInt32(ArgvArray[2], &U32Val);
        ViewID = U32Val;
        (void)AmbaUtility_StringToUInt32(ArgvArray[3], &U32Val);
        DeMode = (INT32) U32Val;
        (void)Amba_IQParam_Load_CC_By_DE(ViewID, DeMode);
        (void)AmbaImgProc_DESetDEffect(ViewID, &ModeCfg, (UINT8)DeMode);

    } else {


        AmpUT_ImgProcTest_Sub1111(ArgCount, pArgVector); //Sub1111
    } //4

//
    if(U32RVal != 0U){
        /* MisraC */
    }
}
static void AmpUT_ImgProcTest_Sub11(UINT32 ArgCount, char * const * pArgVector)
{
    static char *   ArgvArray[64];     // A large argv array
//    static UINT32   AmbaSampleChNo = 0;
    static char     Zero[2];            // A string of "0"
    UINT32          T = 0;
    UINT32          U32Val = 0;
    UINT32          U32RVal = 0U;

    Zero[0] = '0';
    Zero[1] = '\0';
    for (T = 0U; T < 64U; ++T) {
        if (T < ArgCount) {
            ArgvArray[T] = pArgVector[T];
        } else {
            ArgvArray[T] = Zero;
        }
    }
//
    if (AmbaUtility_StringCompare(ArgvArray[1],"-madj", 5U) == 0) {
        UINT8 enable;
        UINT32 madj_no;
        UINT32 chNo = 0;

        (void)AmbaUtility_StringToUInt32(ArgvArray[2], &U32Val);
        enable = (UINT8)U32Val;
        (void)AmbaUtility_StringToUInt32(ArgvArray[3], &U32Val);
        madj_no = U32Val;
        (void)AmbaUtility_StringToUInt32(ArgvArray[4], &U32Val);
        chNo = U32Val;

        (void)Amba_Set_MADJ(chNo, enable, madj_no);

    } else if (AmbaUtility_StringCompare(ArgvArray[1],"-stilladj", 9U) == 0) {

        static ADJ_STILL_IQ_INFO_s  AdjStillIqInfoTest;
        IQ_PARAM_s                  *pIQParams = NULL;
        UINT16                      evindex = 0;
        UINT16                      nfindex = 0;
        UINT16                      hiLoIso = 0;
        AMBA_IK_WB_GAIN_s           StillWbGain = {WB_UNIT_GAIN, WB_UNIT_GAIN, WB_UNIT_GAIN};

        (void)AmbaUtility_StringToUInt32(ArgvArray[2], &U32Val);
        evindex = (UINT16)U32Val;
        (void)AmbaUtility_StringToUInt32(ArgvArray[3], &U32Val);
        nfindex = (UINT16)U32Val;
        (void)AmbaUtility_StringToUInt32(ArgvArray[4], &U32Val);
        hiLoIso = (UINT16)U32Val;

        if(hiLoIso == 0U){
            AdjStillIqInfoTest.Mode = 2;    //IP_MODE_LISO_STILL:2  IP_MODE_HISO_STILL:3
        }else{
            AdjStillIqInfoTest.Mode = 3;    //IP_MODE_LISO_STILL:2  IP_MODE_HISO_STILL:3
        }
        (void)Amba_IQParam_Get_IqParams(0U, &pIQParams);
        AdjStillIqInfoTest.pStillLISOParam = &pIQParams->stillLISOParams[0];
        AdjStillIqInfoTest.pStillHISOParam = &pIQParams->stillHISOParams[0];
        AdjStillIqInfoTest.AE.ShutterIndex= 1012;
        AdjStillIqInfoTest.AE.EvIndex = evindex;
        AdjStillIqInfoTest.AE.NfIndex = nfindex;
        AdjStillIqInfoTest.WB= StillWbGain;

        AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "ParamVersionNum:0x%x", AdjStillIqInfoTest.pStillLISOParam->ParamVersionNum, 0U, 0U, 0U, 0U);
        AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "ParamVersionNum:0x%x", AdjStillIqInfoTest.pStillHISOParam->ParamVersionNum, 0U, 0U, 0U, 0U);
        AmbaPrint_Flush();
        (void)AmbaImgProc_ADJStillCtrl(0, &AdjStillIqInfoTest);

    } else if (AmbaUtility_StringCompare(ArgvArray[1],"-me", 3U) == 0) {
        UINT8           Mode = 0;
        FLOAT           ShtTime, AgcGain;
        UINT32          DGain = 0;
        UINT32          Flash = 0;
        UINT32          chNo = 0;
        AMBA_AE_INFO_s  AeInfo = {0};
        AMBA_AE_INFO_s  StillAeInfo[MAX_AEB_NUM] = {0};
        char            buf1[50], buf2[50];
        (void)AmbaUtility_StringToUInt32(ArgvArray[2], &U32Val);
        chNo = U32Val;
        (void)AmbaUtility_StringToUInt32(ArgvArray[3], &U32Val);
        Mode = (UINT8)U32Val;
        (void)AmbaUtility_StringToUInt32(ArgvArray[4], &U32Val);
        ShtTime = (FLOAT)U32Val / 1000000.0f;
        (void)AmbaUtility_StringToUInt32(ArgvArray[5], &U32Val);
        AgcGain = (FLOAT)U32Val / 1000.0f;
        (void)AmbaUtility_StringToUInt32(ArgvArray[6], &U32Val);
        DGain = (UINT32)U32Val;
        (void)AmbaUtility_StringToUInt32(ArgvArray[7], &U32Val);
        Flash = (UINT32)U32Val;

        AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "chNo : %d,  Mode : %d, DGain : %d ", chNo, Mode, DGain, 0U, 0U);

        (void)AmbaUtility_FloatToStr(&buf1[0],  50U, ShtTime, 6U);
        (void)AmbaUtility_FloatToStr(&buf2[0],  50U, AgcGain, 6U);
        AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "ShtTime: %s, AgcGain: %s", &buf1[0], &buf2[0], NULL, NULL, NULL);

        if (Mode == 0U) {
            (void) AmbaImgProc_AEGetExpInfo(chNo, 0, IP_MODE_VIDEO, &AeInfo);
            AeInfo.AgcGain = AgcGain;
            AeInfo.ShutterTime = ShtTime;
            AeInfo.Dgain = (INT32)DGain;
            AeInfo.Flash = (UINT16) Flash;

            (void)AmbaImgProc_AESetExpInfo(chNo, 0U, IP_MODE_VIDEO, &AeInfo);
        }else{
            (void) AmbaImgProc_AEGetExpInfo(chNo, 0, IP_MODE_STILL, StillAeInfo);
            StillAeInfo[0].AgcGain = AgcGain;
            StillAeInfo[0].ShutterTime = ShtTime;
            StillAeInfo[0].Dgain = (INT32)DGain;
            StillAeInfo[0].Flash = (UINT16) Flash;

            (void)AmbaImgProc_AESetExpInfo(chNo, 0U, IP_MODE_STILL, StillAeInfo);
        }

        (void)AmbaImgProc_SetMEMWBCommand(0U, ENABLE);

    } else if (AmbaUtility_StringCompare(ArgvArray[1],"-mwb", 4U) == 0) {
        UINT8                Mode = 0;
        AMBA_IK_WB_GAIN_s    WbGain = {WB_UNIT_GAIN,WB_UNIT_GAIN,WB_UNIT_GAIN};
        AMBA_IK_WB_GAIN_s    StillWbGain[MAX_AEB_NUM];
        UINT32               chNo = 0;
        UINT32               i = 0U;

        (void)AmbaUtility_StringToUInt32(ArgvArray[2], &U32Val);
        chNo = (UINT32)U32Val;
        (void)AmbaUtility_StringToUInt32(ArgvArray[3], &U32Val);
        Mode = (UINT8)U32Val;

        if (Mode == 0U) {
            (void)AmbaImgProc_AWBGetWBGain(chNo, 0U, IP_MODE_VIDEO, &WbGain);
        }else{
            (void)AmbaImgProc_AWBGetWBGain(chNo, 0U, IP_MODE_STILL, StillWbGain);
        }

        (void)AmbaUtility_StringToUInt32(ArgvArray[4], &U32Val);
        WbGain.GainR = (UINT16)U32Val;
        (void)AmbaUtility_StringToUInt32(ArgvArray[5], &U32Val);
        WbGain.GainG = (UINT16)U32Val;
        (void)AmbaUtility_StringToUInt32(ArgvArray[6], &U32Val);
        WbGain.GainB = (UINT16)U32Val;

        AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, " chNo : %d, Mode : %d, GainR : %5d, GainG : %5d, GainB : %5d ",chNo, Mode, WbGain.GainR, WbGain.GainG, WbGain.GainB);

        if (Mode == 0U) {
            (void)AmbaImgProc_AWBSetWBGain(chNo, 0U, IP_MODE_VIDEO, &WbGain);
        }else{
            for (i=0U; i<MAX_AEB_NUM; i++) {
                StillWbGain[i] = WbGain;
            }
            (void)AmbaImgProc_AWBSetWBGain(chNo, 0U, IP_MODE_STILL, StillWbGain);
        }
        (void)AmbaImgProc_SetMEMWBCommand(0U, ENABLE);

    } else if (AmbaUtility_StringCompare(ArgvArray[1],"-evbias", 7U) == 0) {
        INT16 EvBias = 0;
        AE_CONTROL_s AeCtrlMode;

        (void)AmbaUtility_StringToUInt32(ArgvArray[2], &U32Val);
        EvBias = (INT16)U32Val;

        (void)AmbaImgProc_AEGetAEControlCap(0U, &AeCtrlMode);
        AeCtrlMode.EvBias = EvBias;
        (void)AmbaImgProc_AESetAEControlCap(0U, &AeCtrlMode);


    } else if (AmbaUtility_StringCompare(ArgvArray[1],"-slowshutter", 12U) == 0) {
        //UINT8 SlowShutterFps = 0;
        UINT8 Mode = 0;
        INT32 Enable = 0;
        AE_CONTROL_s AeCtrlMode;

        (void)AmbaUtility_StringToUInt32(ArgvArray[2], &U32Val);
        Mode = (UINT8)U32Val;
        (void)AmbaUtility_StringToUInt32(ArgvArray[3], &U32Val);
        Enable = (INT32)U32Val;

        if(ArgvArray[3][0]=='-'){
            INT32 INT32Val;
            (void)AmbaUtility_StringToUInt32(&ArgvArray[3][1], &U32Val);
            INT32Val = (INT32)U32Val*(-1);
            AmbaPrint_ModulePrintInt5(IMGPROC_PRINT_MODULE_ID, "\n\n SlowShutter:%d,",
            INT32Val,0 , 0, 0, 0);
            Enable = INT32Val;
        }


        (void)AmbaImgProc_AEGetAEControlCap(0U, &AeCtrlMode);
        if (Mode == 0U) { //Video Preview
            AeCtrlMode.SlowShutter = Enable;
            //AeCtrlMode.SlowShutterFps = SlowShutterFps;
        }else{ //Photo Preview
            AeCtrlMode.PhotoSlowShutter = Enable;
            //AeCtrlMode.PhotoSlowShutterFps = SlowShutterFps;
        }
        (void)AmbaImgProc_AESetAEControlCap(0U, &AeCtrlMode);

    } else{
        AmpUT_ImgProcTest_Sub111(ArgCount, pArgVector); //Sub111
    }//3


//
    if(U32RVal != 0U){
        /* MisraC */
    }
}

static void AmpUT_ImgProcTest_Sub1(UINT32 ArgCount, char * const * pArgVector)
{
    static char *   ArgvArray[64];     // A large argv array
    static UINT32   AmbaSampleChNo = 0;
    static char     Zero[2];            // A string of "0"
    UINT32          T = 0;
    UINT32          U32Val = 0;
    UINT32          U32RVal = 0U;

    Zero[0] = '0';
    Zero[1] = '\0';
    for (T = 0U; T < 64U; ++T) {
        if (T < ArgCount) {
            ArgvArray[T] = pArgVector[T];
        } else {
            ArgvArray[T] = Zero;
        }
    }
//
    if (AmbaUtility_StringCompare(ArgvArray[1],"-get3astatus", 12U) == 0) {
        AMBA_3A_STATUS_s    VideoStatus;
        AMBA_3A_STATUS_s    StillStatus;

        (void)AmbaImgProc_GetAAAStatus(AmbaSampleChNo, &VideoStatus, &StillStatus);
        AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "---- VideoStatus : %d,%d,%d ----", VideoStatus.Ae, VideoStatus.Awb, VideoStatus.Af, 0U, 0U);
        AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "---- StillStatus : %d,%d,%d ----", StillStatus.Ae, StillStatus.Awb, StillStatus.Af, 0U, 0U);

    } else if (AmbaUtility_StringCompare(ArgvArray[1],"-set3astatus", 12U) == 0) {
        AMBA_3A_STATUS_s    VideoStatus;
        AMBA_3A_STATUS_s    StillStatus;
        (void)AmbaImgProc_GetAAAStatus(AmbaSampleChNo, &VideoStatus, &StillStatus);

        if (AmbaUtility_StringCompare(ArgvArray[2],"video", 5U) == 0) {
            (void)AmbaUtility_StringToUInt32(ArgvArray[3], &U32Val);
            VideoStatus.Ae = (UINT8)U32Val;
            (void)AmbaUtility_StringToUInt32(ArgvArray[4], &U32Val);
            VideoStatus.Awb = (UINT8)U32Val;
            (void)AmbaUtility_StringToUInt32(ArgvArray[5], &U32Val);
            VideoStatus.Af = (UINT8)U32Val;
        } else {
            (void)AmbaUtility_StringToUInt32(ArgvArray[3], &U32Val);
            StillStatus.Ae = (UINT8)U32Val;
            (void)AmbaUtility_StringToUInt32(ArgvArray[4], &U32Val);
            StillStatus.Awb = (UINT8)U32Val;
            (void)AmbaUtility_StringToUInt32(ArgvArray[5], &U32Val);
            StillStatus.Af = (UINT8)U32Val;
        }

        (void)AmbaImgProc_SetAAAStatus(AmbaSampleChNo, &VideoStatus, &StillStatus);

    } else if (AmbaUtility_StringCompare(ArgvArray[1],"-3a", 3U) == 0) {
        AMBA_AAA_OP_INFO_s  AaaOpInfoTmp = {0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U};
        UINT32              ViewCount;
        UINT32              i;

        if (AmbaUtility_StringCompare(ArgvArray[2],"on", 2U) == 0) {
            AaaOpInfoTmp.AeOp = 1;
            AaaOpInfoTmp.AwbOp = 1;
            AaaOpInfoTmp.AfOp = 1;
            AaaOpInfoTmp.AdjOp = 1;
            (void)AmbaImgProc_GetTotalViewCount(&ViewCount);
            for(i = 0; i < ViewCount; i++){
                (void)AmbaImgProc_SetAAAOPInfo(i, &AaaOpInfoTmp);
            }

        } else if (AmbaUtility_StringCompare(ArgvArray[2],"off", 3U) == 0) {
            AaaOpInfoTmp.AeOp = 0;
            AaaOpInfoTmp.AwbOp = 0;
            AaaOpInfoTmp.AfOp = 0;
            AaaOpInfoTmp.AdjOp = 0;
            (void)AmbaImgProc_GetTotalViewCount(&ViewCount);
            for(i = 0; i < ViewCount; i++){
                (void)AmbaImgProc_SetAAAOPInfo(i, &AaaOpInfoTmp);
            }

        } else if (AmbaUtility_StringCompare(ArgvArray[2],"ver", 3U) == 0) {
            AMBA_IP_VER IPVer;

            (void)AmbaImgProc_GetAAALibVersion(&IPVer);
            AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "--- Amba Ae  Lib.,Verison : %5d ----", IPVer.AeRev, 0U, 0U, 0U, 0U);
            AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "--- Amba Awb Lib.,Verison : %5d ----", IPVer.AwbRev, 0U, 0U, 0U, 0U);
            AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "--- Amba Adj Lib.,Verison : %5d ----", IPVer.AdjRev, 0U, 0U, 0U, 0U);
        } else {
            (void)AmbaImgProc_GetTotalViewCount(&ViewCount);

            for (i = 0; i < ViewCount; i++) {
                (void)AmbaImgProc_GetAAAOPInfo(i, &AaaOpInfoTmp);
                AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "ChNo[%u]: --- AeOp : %d, AwbOp : %d, AdjOp : %d, AfOp : %d ---", \
                    i, \
                    AaaOpInfoTmp.AeOp, \
                    AaaOpInfoTmp.AwbOp, \
                    AaaOpInfoTmp.AdjOp, \
                    AaaOpInfoTmp.AfOp);
            }
        }
    } else if(AmbaUtility_StringCompare(ArgvArray[1],"-loadsc", 3U) == 0) {
        INT32 DeTmp = 0;

        (void)AmbaUtility_StringToUInt32(ArgvArray[2], &U32Val);
        DeTmp = (INT32)U32Val;
        (void)Amba_IQParam_Load_CC_By_Scene(0, DeTmp);
        (void)AmbaImgProc_SCSetSceneMode(0U, IP_MODE_VIDEO, &DeTmp);
        (void)AmbaImgProc_SCSetSceneMode(0U, IP_MODE_STILL, &DeTmp);

    } else{
        AmpUT_ImgProcTest_Sub11(ArgCount, pArgVector); //Sub11
    }//2

//
    if(U32RVal != 0U){
        /* MisraC */
    }
}
//End of Compllexity
void AmpUT_ImgProcTest(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    static char *   ArgvArray[64];     // A large argv array
//    static UINT32   AmbaSampleChNo = 0;
    static char     Zero[2];            // A string of "0"
    UINT32          T = 0;
    UINT32          U32Val = 0;
    UINT32          U32RVal = 0U;

    if (PrintFunc == NULL) {
        /* */
    }
    Zero[0] = '0';
    Zero[1] = '\0';
    for (T = 0U; T < 64U; ++T) {
        if (T < ArgCount) {
            ArgvArray[T] = pArgVector[T];
        } else {
            ArgvArray[T] = Zero;
        }
    }

    (void)AmbaPrint_ModuleSetAllowList(IMGPROC_PRINT_MODULE_ID, 1U);
    AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "AmpUT_ImgProcTest cmd: %s", ArgvArray[1], NULL, NULL, NULL, NULL);

    if (AmbaUtility_StringCompare(ArgvArray[1],"-chno", 5U) == 0) {
        /*Do something*/
#if defined(CONFIG_SOC_CV2FS)
    } else if (AmbaUtility_StringCompare(ArgvArray[1],"-asilen", 7U) == 0) {
        UINT32 ViewID = 0;
        AMBA_AAA_ASIL_OP_INFO_s AAAAsilOpInfo;
        AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "imgproc -asilen [ViewID] [AE] [AWB] [ADJ]", NULL, NULL, NULL, NULL, NULL);
        (void)AmbaUtility_StringToUInt32(ArgvArray[2], &ViewID);
        (void)AmbaUtility_StringToUInt32(ArgvArray[3], &AAAAsilOpInfo.Ae);
        (void)AmbaUtility_StringToUInt32(ArgvArray[4], &AAAAsilOpInfo.Awb);
        (void)AmbaUtility_StringToUInt32(ArgvArray[5], &AAAAsilOpInfo.Adj);
        (void)AmbaImgProc_SetAAAAsilOPInfo(ViewID, &AAAAsilOpInfo);

    } else if (AmbaUtility_StringCompare(ArgvArray[1],"-fldwb", 6U) == 0) {
        IMG_PROC_ASIL_FUNC_s AAAASILIpFunc;
        // extern UINT32 AppTest_AAAFlexidagInputCb(void *pInput, UINT32 DataSize);
        extern UINT32 AmbaImgProc_AWBGetAWBAlgoInfo(UINT32 ViewID, AWB_ALGO_INFO_s *pAWBAlgoInfo);

        // extern uint32_t AmbaCV_FetchChipInfo(const char* pFlexibinPath, AMBA_CV_CHIP_INFO_t *pChipInfo);
        // AMBA_CV_CHIP_INFO_t ambacvchipinfo = {0};
        // char path0[128]="c:\\flexidag_fetch_chip_info/flexibin/flexibin0.bin";
        // UINT32 i;
        // static AMBA_IK_CFA_3A_DATA_s   CFAStat = {0};
        // static AWB_ALGO_INFO_s AwbAlgoInfo = {0};
        // AMBA_IK_WB_GAIN_s WBGain;
        UINT32 ViewID = 0;
        UINT32 DbgPrt = 0;
        UINT32 InOutDoor = 0;
        AMBA_AAA_FLEXIDAG_INFO_s *pAAAFlexidagInfo;

        // INT32 *parray;
        (void)AmbaUtility_StringToUInt32(ArgvArray[2], &ViewID);
        (void)AmbaUtility_StringToUInt32(ArgvArray[3], &DbgPrt);
        (void)AmbaUtility_StringToUInt32(ArgvArray[4], &InOutDoor);
        (void)AmbaImgProc_AAAGetFlexidagInfoAdd(ViewID, &pAAAFlexidagInfo);

        pAAAFlexidagInfo->DbgPrt = (UINT8)DbgPrt;

        if(InOutDoor==1U){
            pAAAFlexidagInfo->HighLightEvIndex = pAAAFlexidagInfo->OutdoorEvIndex + (pAAAFlexidagInfo->DoubleInc*2U);
            pAAAFlexidagInfo->EvIndex = pAAAFlexidagInfo->OutdoorEvIndex - (pAAAFlexidagInfo->DoubleInc>>1);
            AmbaPrint_PrintUInt5("set manual evindex for awb flexidag HighLightEvIndex  %d,OutdoorEvIndex  %d,EvIndex  %d,",
                                                                 pAAAFlexidagInfo->HighLightEvIndex,
                                                                 pAAAFlexidagInfo->OutdoorEvIndex,
                                                                 pAAAFlexidagInfo->EvIndex,
                                                                 0U,0U);
        }

        AmbaPrint_PrintUInt5("size of AMBA_AAA_FLEXIDAG_INFO_s  %d",sizeof(AMBA_AAA_FLEXIDAG_INFO_s) ,
                                                                 0U, 0U, 0U,0U);//76028

{
    extern UINT32 AmbaImgProc_AEGetPreExpInfo(UINT32 ViewID, UINT32 ExpNo, UINT32 Mode, AMBA_AE_INFO_s *pAEInfo);
    UINT32 viewID = 0;
    static AMBA_AE_INFO_s CurAeInfo;
    static AMBA_AE_INFO_s PreAeInfo;
    char charBuf[5][50];
    (void)AmbaImgProc_AEGetExpInfo(viewID, 0U/*ExpNo*/, IP_MODE_VIDEO, &CurAeInfo);
    (void)AmbaImgProc_AEGetPreExpInfo(viewID, 0U/*ExpNo*/, IP_MODE_VIDEO, &PreAeInfo);

    //AE
    pAAAFlexidagInfo->CurShutterTime = CurAeInfo.ShutterTime;
    pAAAFlexidagInfo->CurAgcGain = CurAeInfo.AgcGain;
    pAAAFlexidagInfo->CurDgain = CurAeInfo.Dgain;
    pAAAFlexidagInfo->CurLumaStat = CurAeInfo.LumaStat;
    pAAAFlexidagInfo->CurLimitStatus = CurAeInfo.LimitStatus;
    pAAAFlexidagInfo->CurTarget = CurAeInfo.Target;

    pAAAFlexidagInfo->PreShutterTime = PreAeInfo.ShutterTime;
    pAAAFlexidagInfo->PreAgcGain = PreAeInfo.AgcGain;
    pAAAFlexidagInfo->PreDgain = PreAeInfo.Dgain;

    (void)AmbaUtility_FloatToStr(&charBuf[0][0],  50U, CurAeInfo.ShutterTime, 6U);
    (void)AmbaUtility_FloatToStr(&charBuf[1][0],  50U, CurAeInfo.AgcGain, 6U);
    (void)AmbaUtility_FloatToStr(&charBuf[2][0],  50U, PreAeInfo.ShutterTime, 6U);
    (void)AmbaUtility_FloatToStr(&charBuf[3][0],  50U, PreAeInfo.AgcGain, 6U);
    AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Cur ShtTime: %s, AgcGain: %s,  Pre ShtTime: %s, AgcGain: %s", &charBuf[0][0], &charBuf[1][0], &charBuf[2][0], &charBuf[3][0], NULL);
    AmbaPrint_PrintInt5("CurDgain:  %d,CurLumaStat:  %d,CurLimitStatus:  %d,CurTarget:  %d, PreDgain:%d ",
                         (INT32)CurAeInfo.Dgain,  (INT32)CurAeInfo.LumaStat,  (INT32)CurAeInfo.LimitStatus,   (INT32)CurAeInfo.Target,   (INT32)PreAeInfo.Dgain);

}

{
    extern INT32 Amba_Img_Get_Video_Pipe_Ctrl_Addr(UINT32 ViewID, PIPELINE_CONTROL_s **pTableAddr);
    PIPELINE_CONTROL_s          *pAdjVideoPipe;
    AMBA_IP_MONITOR_CONFIG_s    MonCfg;
    (void)Amba_Img_Get_Video_Pipe_Ctrl_Addr(ViewID, &pAdjVideoPipe);
    AmbaPrint_PrintUInt5("PIPELINE_CONTROL_s:  %d, ",
                         sizeof(PIPELINE_CONTROL_s), 0U, 0U, 0U, 0U);//38380

    // pAAAFlexidagInfo->pUINT32Array  = (UINT32*)&pAdjVideoPipe->BlackCorr.BlackR;
    // pAAAFlexidagInfo->pINT32Array  = (INT32*)&pAdjVideoPipe->BlackCorr.BlackR;

    // AmbaPrint_PrintUInt5("pAAAFlexidagInfo->pUINT32Array:  %d, ",
    //                      (UINT32)pAAAFlexidagInfo->pUINT32Array, 0U, 0U, 0U, 0U);

    // AmbaPrint_PrintUInt5("pAAAFlexidagInfo->pINT32Array:  %d,%d,%d,%d,%d, ",
    //                      (INT32)*pAAAFlexidagInfo->pINT32Array, *(pAAAFlexidagInfo->pINT32Array+1), *(pAAAFlexidagInfo->pINT32Array+2), *(pAAAFlexidagInfo->pINT32Array+3), *(pAAAFlexidagInfo->pINT32Array+4));

    // parray = (INT32*)&pAdjVideoPipe->BlackCorr.BlackR;
    // AmbaPrint_PrintInt5( "[parray ] : %d, %d, %d, %d, %d, \n",
    // *parray, pAdjVideoPipe->BlackCorr.BlackB, pAdjVideoPipe->BlackCorr.BlackR, pAdjVideoPipe->BlackCorr.BlackGr, (UINT32)parray);

    pAAAFlexidagInfo->pPipe0 = pAdjVideoPipe;
    pAAAFlexidagInfo->pPipe1 = pAdjVideoPipe;
    pAAAFlexidagInfo->pPipe2 = pAdjVideoPipe;


    //(void)AppTest_AAAFlexidagInputCb(pAAAFlexidagInfo, sizeof(AMBA_AAA_FLEXIDAG_INFO_s));
    (void)AmbaImgProc_GetAsilRegFunc(ViewID, &AAAASILIpFunc);
    (void)AAAASILIpFunc.AAAFlexidagInputCb(pAAAFlexidagInfo, sizeof(AMBA_AAA_FLEXIDAG_INFO_s));

    (void)AAAASILIpFunc.AmbaIP_GetMonitorConfig((IPMONADJ_ERR_BASE >> 16U),&MonCfg);
    AmbaPrint_PrintUInt5("AmbaIP_GetMonitorConfig,MonCfg.BitMask adj:  %d, ",
                          MonCfg.BitMask, 0U, 0U, 0U, 0U);

    (void)AAAASILIpFunc.AmbaIP_GetMonitorConfig((IPMONAE_ERR_BASE >> 16U),&MonCfg);
    AmbaPrint_PrintUInt5("AmbaIP_GetMonitorConfig,MonCfg.BitMask ae:  %d, ",
                          MonCfg.BitMask, 0U, 0U, 0U, 0U);

    (void)AAAASILIpFunc.AmbaIP_GetMonitorConfig((IPMONAWB_ERR_BASE >> 16U),&MonCfg);
    AmbaPrint_PrintUInt5("AmbaIP_GetMonitorConfig,MonCfg.BitMask awb :  %d, ",
                          MonCfg.BitMask, 0U, 0U, 0U, 0U);

}
#endif
    } else if (AmbaUtility_StringCompare(ArgvArray[1],"-sc_chg", 7U) == 0) {
        UINT32 chNo = 0;
        INT32 sceneMode = 0;
        (void)AmbaUtility_StringToUInt32(ArgvArray[2], &chNo);
        (void)AmbaUtility_StringToUInt32(ArgvArray[3], &U32Val);
        sceneMode = (INT32)U32Val;
        AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "chNo : %d, sceneMode : %d", chNo, (UINT32)sceneMode, 0U, 0U, 0U);
        Ssample_Sc(chNo, sceneMode);

    } else if (AmbaUtility_StringCompare(ArgvArray[1],"-sc_chginfo_sht", 15U) == 0) {
        UINT32 chNo = 0;
        UINT16 ShtTmp = 0;
        INT32  sceneMode = 0;

        (void)AmbaUtility_StringToUInt32(ArgvArray[2], &chNo);
        (void)AmbaUtility_StringToUInt32(ArgvArray[3], &U32Val);
        ShtTmp = (UINT16)U32Val;
        (void)AmbaUtility_StringToUInt32(ArgvArray[4], &U32Val);
        sceneMode = (INT32)U32Val;

        AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "ShtTmp : %d", ShtTmp, 0U, 0U, 0U, 0U);
        Ssample_ScInfo_Sht(chNo, sceneMode, ShtTmp);
    } else if (AmbaUtility_StringCompare(ArgvArray[1],"-sc_chginfo_iso", 15U) == 0) {
        UINT32 chNo = 0;
        UINT16 IsoTmp = 0;
        INT32  sceneMode = 0;

        (void)AmbaUtility_StringToUInt32(ArgvArray[2], &chNo);
        (void)AmbaUtility_StringToUInt32(ArgvArray[3], &U32Val);
        IsoTmp = (UINT16)U32Val;
        (void)AmbaUtility_StringToUInt32(ArgvArray[4], &U32Val);
        sceneMode = (INT32)U32Val;

        AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "IsoTmp : %d", IsoTmp, 0U, 0U, 0U, 0U);
        Ssample_ScInfo_ISO(chNo, sceneMode, IsoTmp);

    } else if (AmbaUtility_StringCompare(ArgvArray[1],"-mebias", 7U) == 0) {
        UINT32  Target = 0;
        DOUBLE  EvBias = 0.0;
        char    buffer[50];

        (void)AmbaUtility_StringToUInt32(ArgvArray[2], &Target);
        EvBias = Ssample_Me_Bias(Target);
        (void)AmbaUtility_DoubleToStr(&buffer[0], 50U, EvBias, 6U);

        AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "EvBias: %s", &buffer[0], NULL, NULL, NULL, NULL);
        AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "Target : %d", Target, 0U, 0U, 0U, 0U);

    } else if (AmbaUtility_StringCompare(ArgvArray[1],"-setmode", 8U) == 0) {
        UINT32 CurrMode = 0U;
        UINT32 NextMode = 0U;

        (void)AmbaUtility_StringToUInt32(ArgvArray[2], &CurrMode);
        (void)AmbaUtility_StringToUInt32(ArgvArray[3], &NextMode);

        (void)AmbaImgProc_SetMode(0U, &CurrMode, &NextMode);

    } else if (AmbaUtility_StringCompare(ArgvArray[1],"-getmode", 8U) == 0) {
        UINT32 CurrMode;
        UINT32 NextMode;

        (void)AmbaImgProc_GetMode(0U, &CurrMode, &NextMode);
        AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "---- CurrMode : %d, NextMode : %d ----", CurrMode, NextMode, 0U, 0U, 0U);

    } else {
        AmpUT_ImgProcTest_Sub1(ArgCount, pArgVector); //Sub1
    }

    if(U32RVal != 0U){
        /* MisraC */
    }
}


