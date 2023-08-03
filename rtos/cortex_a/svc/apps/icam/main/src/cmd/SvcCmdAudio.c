/**
*  @file SvcCmdAudio.c
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
*  @details svc audio command
*
*/

#include "AmbaKAL.h"
#include "AmbaShell.h"
#include "AmbaUtility.h"
#include "AmbaMisraFix.h"
#include "AmbaPrint.h"
#include "AmbaSYS.h"
#include "AmbaNVM_Partition.h"
#include "AmbaDMA_Def.h"
#include "AmbaVfs.h"
#include "AmbaWrap.h"
#include "AmbaAudioEfx.h"
#include "AmbaAudio_AENC.h"
#include "AmbaAudio_AIN.h"
#include "AmbaAudioBuf.h"
#include "AmbaAudio.h"
#include "AmbaAudioBSBuf.h"
#include "AmbaAEncFlow.h"
#include "AmbaADecFlow.h"
#include "AmbaPrint.h"

#include "AmbaSYS.h"
#include "AmbaNVM_Partition.h"
#include "SvcErrCode.h"
#include "SvcWrap.h"
#include "SvcLog.h"
#include "SvcTask.h"
#include "SvcRecMain.h"
#include "SvcCmd.h"
#include "SvcNvm.h"
#include "SvcPlat.h"
#include "SvcCmdAudio.h"

#define SVC_LOG_CMDA        "CMDA"

static AMBA_FS_FILE  *pFile GNU_SECTION_NOZEROINIT;
static ULONG         DecId;

static void CmdAppUsage(AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("svc_audio commands:\n");
    PrintFunc(" coding [coding_format]: configure coding format, 0=AAC, 1=PCM\n");
    PrintFunc(" start                 : start audio record\n");
    PrintFunc(" stop                  :  stop audio record\n");
    PrintFunc(" set_vol [level]       : Level range: 1~128, 64 by default (Original DB) \n");
}

static void AoutCtrl(UINT32 Chan, UINT32 Op)
{
    if ((g_pPlatCbEntry != NULL) && (g_pPlatCbEntry->pCbAoutCtrl != NULL)) {
        g_pPlatCbEntry->pCbAoutCtrl(Chan, Op);
    }
}

static void BEEP_ONETIME(UINT32 Delay)
{
    const char FileName[20] = "c:\\sample.pcm";
    ULONG      FileInfo;

    (void)AmbaFS_FileOpen(FileName, "rb", &pFile);
    AmbaMisra_TypeCast(&FileInfo, &(pFile));
    AmbaADecFlow_Setup(AMBA_AUDIO_TYPE_PCM, AMBA_AUDIO_SOURCE_AUDIO, FileInfo, NULL, &DecId);
    AmbaADecFlow_FeedTaskCreate(DecId);
    AmbaADecFlow_DecStart(DecId);
    AmbaADecFlow_OupStart(DecId, AoutCtrl);
    (void)AmbaKAL_TaskSleep(Delay);
    AmbaADecFlow_Stop(DecId, 0U, AoutCtrl);
    (void)AmbaFS_FileClose(pFile);
    AmbaADecFlow_Delete(DecId);
    AmbaADecFlow_FeedTaskDelete(DecId);
}

static UINT32 CheckAudRomExist(AMBA_AUD_FROM_ROM_s *pAudFromRom)
{
    UINT32 RtVal = SvcNvm_GetRomFileSize(AMBA_USER_PARTITION_SYS_DATA,
                                         pAudFromRom->AudRomFileName, &pAudFromRom->AudROMSize);
    return RtVal;
}

static void AinCtrl(UINT32 Chan, UINT32 Op)
{
    if ((g_pPlatCbEntry != NULL) && (g_pPlatCbEntry->pCbAinCtrl != NULL)) {
        g_pPlatCbEntry->pCbAinCtrl(Chan, Op);
    }
}

static void CmdAudioEntry(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32  Rval = OK;
    static  UINT32 g_EncNum = 0U;

    if (0 == SvcWrap_strcmp("coding", pArgVector[1U])) {
#if defined(CONFIG_ICAM_RECORD_USED)
        UINT32  Val;

        if (SVC_OK == SvcWrap_strtoul(pArgVector[2U], &Val)) {
            if (Val == 0U) {
                Val = AMBA_RSC_AUD_SUBTYPE_AAC;
                SvcRecMain_Control(SVC_RCM_PMT_AUCODING, 0U, NULL, &Val);

                Val = AMBA_AUDIO_TYPE_AAC;
                AmbaAEncFlow_ParamCtrl(AMBA_AENC_PMT_CODING, &Val);
                SvcLog_OK(SVC_LOG_CMDA, "audio coding selection succeeded(AAC)", 0U, 0U);
            } else {
                Val = AMBA_RSC_AUD_SUBTYPE_PCM;
                SvcRecMain_Control(SVC_RCM_PMT_AUCODING, 0U, NULL, &Val);

                Val = AMBA_AUDIO_TYPE_PCM;
                AmbaAEncFlow_ParamCtrl(AMBA_AENC_PMT_CODING, &Val);
                SvcLog_OK(SVC_LOG_CMDA, "audio coding selection succeeded(PCM)", 0U, 0U);
            }
        } else {
            Rval = ERR_ARG;
        }
#else
        /* do nothing */
#endif
    } else if (0 == SvcWrap_strcmp("svc_audio", pArgVector[0U])) {
        if (0 == SvcWrap_strcmp("start", pArgVector[1U])) {
            if (ArgCount > 2U) {
                (void)SvcWrap_strtoul(pArgVector[2], &g_EncNum);
                AmbaAEncFlow_EncAudioOnly(1U);

                if (g_EncNum > (UINT32)CONFIG_AENC_NUM) {
                    SvcLog_OK(SVC_LOG_CMDA, "EncNum(%d) should less-equal to (%d).", g_EncNum, (UINT32)CONFIG_AENC_NUM);
                    SvcLog_OK(SVC_LOG_CMDA, "Set EncNum to %d ", CONFIG_AENC_NUM, 0U);
                    g_EncNum = (UINT32)CONFIG_AENC_NUM;
                }

                AmbaAEncFlow_Start(AinCtrl, 1U, g_EncNum);
                SvcLog_OK(SVC_LOG_CMDA, "Start to Encode %d Audio Encoder", g_EncNum, 0U);
            } else {
                AmbaAEncFlow_EncAudioOnly(1U);
                AmbaAEncFlow_Start(AinCtrl, 1U, 1U);
                SvcLog_OK(SVC_LOG_CMDA, "Start to Encode 1 Audio Encoder Only", 0U, 0U);
                g_EncNum = 1U;
            }
        } else if (0 == SvcWrap_strcmp("set_vol", pArgVector[1U])) {
            UINT32 Volume = 0U, Rvals;
            UINT32 Resolution = AAC_SAMPLE_RESOLUTION;
            AMBA_AENC_FLOW_INFO_s AudioEncInfo;

            (void)SvcWrap_strtoul(pArgVector[2], &Volume);
            AmbaAEncFlow_InfoGet(&AudioEncInfo);
            if (AMBA_AUDIO_TYPE_PCM == (*(AudioEncInfo.pAEncType))) {
                Resolution = PCM_SAMPLE_RESOLUTION;
            } else if (AMBA_AUDIO_TYPE_AAC == (*(AudioEncInfo.pAEncType))) {
                Resolution = AAC_SAMPLE_RESOLUTION;
            } else {
                SvcLog_NG(SVC_LOG_CMDA, "Unknown pAEncType = %d", (*(AudioEncInfo.pAEncType)), 0U);
            }

            Rvals = AmbaAudio_BufSetup_Volume(0U, 1U, AENC_CH_NUM, Resolution, 1024U, Volume);
            if ((UINT32) OK == Rvals) {
                SvcLog_OK(SVC_LOG_CMDA, "AmbaAudio_BufSetup_Volume() OK, Volume = %d", Volume, 0U);
            } else {
                SvcLog_NG(SVC_LOG_CMDA, "AmbaAudio_BufSetup_Volume() NG", 0U, 0U);
            }
        } else if (0 == SvcWrap_strcmp("set_dws", pArgVector[1U])) {
            UINT32 SrcFreq = 48000U, DstFreq = 48000U, Rvals;

            (void)SvcWrap_strtoul(pArgVector[2], &SrcFreq);
            (void)SvcWrap_strtoul(pArgVector[3], &DstFreq);
            Rvals = AmbaAudio_BufSetup_Dws(0U, 0U, SrcFreq, DstFreq);
            if ((UINT32) OK == Rvals) {
                SvcLog_OK(SVC_LOG_CMDA, "AmbaAudio_BufSetup_Dws() OK, SrcFreq = %d, DstFreq = %d", SrcFreq, DstFreq);
            } else {
                SvcLog_NG(SVC_LOG_CMDA, "AmbaAudio_BufSetup_Dws() NG, SrcFreq = %d, DstFreq = %d", SrcFreq, DstFreq);
            }
        } else if (0 == SvcWrap_strcmp("set_dcb", pArgVector[1U])) {
            UINT32 Alpha = 0x7F5C28F5; /* 0.995 */

            if (ArgCount > 2U) {
                (void)SvcWrap_strtoul(pArgVector[2], &Alpha);
            }
            Rval = AmbaAudio_BufSetup_DcBlocker(0U, 0U, Alpha);
            if ((UINT32) OK == Rval) {
                SvcLog_OK(SVC_LOG_CMDA, "AmbaAudio_BufSetup_DcBlocker() OK, Alpha = %d", Alpha, 0U);
            } else {
                SvcLog_NG(SVC_LOG_CMDA, "AmbaAudio_BufSetup_DcBlocker() NG, Alpha = %d", Alpha, 0U);
            }
        } else if (0 == SvcWrap_strcmp("ssp_dbg_print", pArgVector[1U])) {
            Rval = AmbaPrint_ModuleSetAllowList(AENC_MODULE_ID, 1U);
            Rval = AmbaPrint_ModuleSetAllowList(AIN_MODULE_ID, 1U);
            Rval = AmbaPrint_ModuleSetAllowList(AOUT_MODULE_ID, 1U);
            Rval = AmbaPrint_ModuleSetAllowList(ADEC_MODULE_ID, 1U);
        } else if (0 == SvcWrap_strcmp("stop", pArgVector[1U])) {
            AmbaAEncFlow_Stop(AinCtrl, 1U, g_EncNum);
            AmbaAEncFlow_EncAudioOnly(0U);
            SvcLog_OK(SVC_LOG_CMDA, "Stop to Encode %d Audio Encoder", g_EncNum, 0U);
        } else if (0 == SvcWrap_strcmp("decode_start_aac", pArgVector[1U])) {
            const char   FileName[20] = "c:\\sample.aac";
            ULONG        FileInfo;

            (void)AmbaFS_FileOpen(FileName, "rb", &pFile);
            AmbaMisra_TypeCast(&FileInfo, &(pFile));
            AmbaADecFlow_Setup(AMBA_AUDIO_TYPE_AAC, AMBA_AUDIO_SOURCE_AUDIO, FileInfo, NULL, &DecId);
            AmbaADecFlow_FeedTaskCreate(DecId);
            AmbaADecFlow_DecStart(DecId);
            AmbaADecFlow_OupStart(DecId, AoutCtrl);
        } else if (0 == SvcWrap_strcmp("decode_start_pcm", pArgVector[1U])) {
            const char   FileName[20] = "c:\\sample.pcm";
            ULONG        FileInfo;

            (void)AmbaFS_FileOpen(FileName, "rb", &pFile);
            AmbaMisra_TypeCast(&FileInfo, &(pFile));
            AmbaADecFlow_Setup(AMBA_AUDIO_TYPE_PCM, AMBA_AUDIO_SOURCE_AUDIO, FileInfo, NULL, &DecId);
            AmbaADecFlow_FeedTaskCreate(DecId);
            AmbaADecFlow_DecStart(DecId);
            AmbaADecFlow_OupStart(DecId, AoutCtrl);
        } else if (0 == SvcWrap_strcmp("decode_stop", pArgVector[1U])) {
            AmbaADecFlow_Stop(DecId, 0U, AoutCtrl);
            (void)AmbaFS_FileClose(pFile);
            AmbaADecFlow_Delete(DecId);
            AmbaADecFlow_FeedTaskDelete(DecId);
        } else if (0 == SvcWrap_strcmp("beep_test", pArgVector[1U])) {
            AMBA_FS_FILE_INFO_s Info;
            if(AmbaFS_GetFileInfo("c:\\sample.pcm", &Info) == 0U){
                for (UINT32 i = 0U; i< 5U;i++) {
                    BEEP_ONETIME(5U);
                }
            } else {
                SvcLog_OK(SVC_LOG_CMDA, "No c:\\sample.pcm in SD card!", 0U, 0U);
            }
        } else if (0 == SvcWrap_strcmp("load_rom_beep_test", pArgVector[1U])) {
            ULONG FileInfo = 0U;

            AmbaADecFlow_Setup(AMBA_AUDIO_TYPE_PCM, AMBA_AUDIO_SOURCE_ROM_AUD, FileInfo, NULL, &DecId);
            AmbaADecFlow_FeedTaskCreate(DecId);
            AmbaADecFlow_DecStart(DecId);
            AmbaADecFlow_OupStart(DecId, AoutCtrl);
            (void)AmbaKAL_TaskSleep(5U);
            AmbaADecFlow_Stop(DecId, 0U, AoutCtrl);
            AmbaADecFlow_Delete(DecId);
            AmbaADecFlow_FeedTaskDelete(DecId);

        } else if (0 == SvcWrap_strcmp("load_rom_beep", pArgVector[1U])) {
            static AMBA_AUD_FROM_ROM_s  *pAudFromRom GNU_SECTION_NOZEROINIT;
            static AMBA_AUD_FROM_ROM_s  AudFromRom GNU_SECTION_NOZEROINIT;
            if (ArgCount >= 5U) {
                UINT32 RtVal;
                ULONG  FileInfo;
                if (0 == SvcWrap_strcmp("start", pArgVector[3U])) {
                    SvcWrap_strcpy(AudFromRom.AudRomFileName, sizeof(AudFromRom.AudRomFileName), pArgVector[4]);
                    RtVal = CheckAudRomExist(&AudFromRom);
                    if ((UINT32) OK == RtVal) {
                        pAudFromRom = &(AudFromRom);
                        AmbaMisra_TypeCast(&FileInfo, &pAudFromRom);
                        SvcLog_OK(SVC_LOG_CMDA, "Audio size = %d", AudFromRom.AudROMSize, 0U);
                        AmbaPrint_PrintStr5("Start to playback %s from ROM partition.", AudFromRom.AudRomFileName, NULL, NULL, NULL, NULL);

                        if (0 == SvcWrap_strcmp("pcm", pArgVector[2U])) {
                            AmbaADecFlow_Setup(AMBA_AUDIO_TYPE_PCM, AMBA_AUDIO_SOURCE_ROM_AUD, FileInfo, NULL, &DecId);
                            SvcLog_OK(SVC_LOG_CMDA, "Setup audio decode PCM from ROM", 0U, 0U);
                        } else if (0 == SvcWrap_strcmp("aac", pArgVector[2U])) {
                            AmbaADecFlow_Setup(AMBA_AUDIO_TYPE_AAC, AMBA_AUDIO_SOURCE_ROM_AUD, FileInfo, NULL, &DecId);
                            SvcLog_OK(SVC_LOG_CMDA, "Setup audio decode AAC from ROM", 0U, 0U);
                        } else {
                            //Do nothing
                        }
                        AmbaADecFlow_FeedTaskCreate(DecId);
                        AmbaADecFlow_DecStart(DecId);
                        AmbaADecFlow_OupStart(DecId, AoutCtrl);
                    } else {
                        AmbaPrint_PrintStr5("No this audio name: %s in your ROM partition.", AudFromRom.AudRomFileName, NULL, NULL, NULL, NULL);
                    }
                } else if (0 == SvcWrap_strcmp("stop", pArgVector[3U])) {
                    SvcWrap_strcpy(AudFromRom.AudRomFileName, sizeof(AudFromRom.AudRomFileName), pArgVector[4]);
                    AmbaPrint_PrintStr5("Start to playback %s from ROM partition.", AudFromRom.AudRomFileName, NULL, NULL, NULL, NULL);
                    AmbaADecFlow_Stop(DecId, 0U, AoutCtrl);
                    AmbaADecFlow_Delete(DecId);
                }
                else {
                    //Do nothing
                }
            } else {
                SvcLog_NG(SVC_LOG_CMDA, "Please enter your ROM audio file name.", 0U, 0U);
            }
        } else {
            //Do nothing
            SvcLog_NG(SVC_LOG_CMDA, "Audio test cmd error!", 0U, 0U);
        }
    } else {
        Rval = ERR_ARG;
    }

    AmbaMisra_TouchUnused(&ArgCount);
    if (Rval != OK) {
        CmdAppUsage(PrintFunc);
    }
}

/**
 *  Svc audio shell command install
 */
void SvcCmdAudio_Install(void)
{
    UINT32  Rval;
    AMBA_SHELL_COMMAND_s  SvcCmdAudio;

    SvcCmdAudio.pName    = "svc_audio";
    SvcCmdAudio.MainFunc = CmdAudioEntry;
    SvcCmdAudio.pNext    = NULL;

    Rval = SvcCmd_CommandRegister(&SvcCmdAudio);
    if (SHELL_ERR_SUCCESS != Rval) {
        SvcLog_NG(SVC_LOG_CMDA, "## fail to install svc audio command", 0U, 0U);
    }
}
