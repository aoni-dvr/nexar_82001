/**
 *  @file SvcUcode.c
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
 *  @details svc ucode functions
 *
 */

#include ".svc_autogen"

#include "AmbaKAL.h"
#include "AmbaDSP.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaFS.h"
#include "AmbaNVM_Partition.h"
#include "AmbaSD.h"
#include "AmbaUtility.h"
#include "AmbaWrap.h"
#include "AmbaVIN_Def.h"
#include "AmbaCache.h"

#include "SvcErrCode.h"
#include "SvcImg.h"
#include "SvcLog.h"
#include "SvcNvm.h"
#include "SvcResCfg.h"
#include "SvcTiming.h"
#include "SvcDSP.h"
#include "SvcUcode.h"
#include "SvcWrap.h"
#include "SvcPlat.h"
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
#include "AmbaRTSL_NAND.h"
#endif
#define UCODE_FILE_NAME_POS             (9U)
#define UCODE_FILE_NAME_LEN_MAX         (128U)

#define UCODE_FILE_SRC_UNKNOWN          (0U)
#define UCODE_FILE_SRC_BOOTDEV          (1U)
#define UCODE_FILE_SRC_SD               (2U)

#define RSA_SIG_SIZE                    (256U)

#define SVC_LOG_ULDR                    "ULDR"

typedef struct {
    char    PartName[UCODE_FILE_NAME_LEN_MAX];
    UINT32  PartSize;
} PARTIAL_INFO_s;

typedef struct {
    char    FileName[UCODE_FILE_NAME_LEN_MAX];
    UINT32  FileSize;
    ULONG   BufBase;
    ULONG   BufSize;

    #define UCODE_PARTIAL_PAR0      (0U)
    #define UCODE_PARTIAL_PAR1      (1U)
    #define UCODE_PARTIAL_PAR2      (2U)
    #define UCODE_PARTIAL_MAX       (3U)
    PARTIAL_INFO_s  Partials[UCODE_PARTIAL_MAX];
} UCODE_INFO_s;

#if defined(CONFIG_BUILD_FOSS_MBEDTLS) && defined(CONFIG_ENABLE_SECURITY)
extern UINT32 AmbaUserRsaSigVerify(UINT8 *pData, UINT32 DataSize);
#endif

static UINT8 UcodeSrc;
static UCODE_INFO_s UInfo[UCODE_FILE_NUM_MAX] = SVCAG_UCODE_LIST;

static inline void ULDR_NG( const char *pFormat, UINT32 Arg1, UINT32 Arg2) { SvcLog_NG( SVC_LOG_ULDR, pFormat, Arg1, Arg2); }
static inline void ULDR_OK( const char *pFormat, UINT32 Arg1, UINT32 Arg2) { SvcLog_OK( SVC_LOG_ULDR, pFormat, Arg1, Arg2); }
static inline void ULDR_DBG(const char *pFormat, UINT32 Arg1, UINT32 Arg2) { SvcLog_DBG(SVC_LOG_ULDR, pFormat, Arg1, Arg2); }

static UINT32 g_BootStatus = SVCDSP_OP_CLEANBOOT;

static inline UINT32 Align256(UINT32 x)
{
    UINT32 x2 = x;
    x2 &= 0xFFFFFF00U;
    x2 += 0x00000100U;
    return x2;
}

static void Cache_CheckNClean(const UINT8 *pBuf, UINT32 LoadSize)
{
    ULONG   Addr;
    UINT32  AlignSize = Align256(LoadSize);

    AmbaMisra_TypeCast(&Addr, &pBuf);

    Addr &= CACHE_LINE_MASK;
    if (SvcPlat_CacheClean(Addr, AlignSize) != OK) {
        SVC_WRAP_PRINT "## fail to cache clean addr/size(%p/0x%X)"
            SVC_PRN_ARG_S SVC_LOG_ULDR
            SVC_PRN_ARG_PROC SvcLog_NG
            SVC_PRN_ARG_CPOINT pBuf      SVC_PRN_ARG_POST
            SVC_PRN_ARG_UINT32 AlignSize SVC_PRN_ARG_POST
            SVC_PRN_ARG_E
    }
}

UINT32 SvcUcode_LoadPartial0(UINT32* pBufOff, UINT32* pIsPartial)
{
    const char              *pFileName;
    UINT8                   *pBuf;
    ULONG                   LoadAddr;
    UINT32                  i, Rval = SVC_OK, TimeS, TimeE;
    UINT32                  LoadSize;
    AMBA_PARTITION_ENTRY_s  PartInfo;
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
    UINT32 PartID = 0;
    const AMBA_PARTITION_ENTRY_s *pPartEntry;

    pPartEntry = AmbaRTSL_NandGetPartEntry(1, AMBA_USER_PARTITION_SYS_SOFTWARE);
    if ((pPartEntry != NULL) && (pPartEntry->ActualByteSize != 0U) && (pPartEntry->Attribute & FW_PARTITION_ACTIVE_FLAG)) {
        PartID = AMBA_USER_PARTITION_DSP_uCODE;
    } else {
        PartID = AMBA_USER_PARTITION_DSP_uCODE_B;
    }
#endif

    if (g_BootStatus == SVCDSP_OP_CLEANBOOT) {
        #if defined(CONFIG_ICAM_TIMING_LOG)
        SvcTime(SVC_TIME_UCODE_LOAD_START, "Ucode load START");
        #endif

        /* Check ucode data in NVM or not */
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
        (void) SvcNvm_ReadPartitionInfo(1U, PartID, &PartInfo);
#else
        (void) SvcNvm_ReadPartitionInfo(1U, AMBA_USER_PARTITION_DSP_uCODE, &PartInfo);
#endif
        ULDR_OK("Ucode size inside NVM = 0x%X", PartInfo.ActualByteSize, 0U);
        if (0U == PartInfo.ActualByteSize) {
            ULDR_OK("No ucode inside NVM", 0U, 0U);
            Rval = SVC_NG;
        }

        if (SVC_OK == Rval) {
            Rval = AmbaKAL_GetSysTickCount(&TimeS);
            AmbaMisra_TouchUnused(&Rval);

            if (0U < UInfo[UCODE_FILE_DEFBIN].Partials[UCODE_PARTIAL_PAR1].PartSize) {
                *pIsPartial = 1U;
            }

            /* step 1 loading */
            for (i = 0U; i < UCODE_FILE_NUM_MAX; i++) {
            #if defined(CONFIG_ICAM_UCODE_ORCIDSP_SHARING) && defined(UCODE_FILE_ORCIDSP1)
                if (i == UCODE_FILE_ORCIDSP1) {
                    continue;
                }
            #endif

                LoadAddr = UInfo[i].BufBase;
                AmbaMisra_TypeCast(&pBuf, &LoadAddr);
                if (0U < UInfo[i].Partials[UCODE_PARTIAL_PAR0].PartSize) {
                    pFileName = UInfo[i].Partials[UCODE_PARTIAL_PAR0].PartName;

                    (void) SvcNvm_GetRomFileSize(AMBA_USER_PARTITION_DSP_uCODE,
                                                pFileName,
                                                &(UInfo[i].FileSize));

                    LoadSize = UInfo[i].Partials[UCODE_PARTIAL_PAR0].PartSize + RSA_SIG_SIZE;

                    if (LoadSize <= UInfo[i].BufSize) {
                        #if defined(CONFIG_BUILD_FOSS_MBEDTLS) && defined(CONFIG_ENABLE_SECURITY)
                        /* RSA signature check */
                        Rval = SvcNvm_ReadRomFile( AMBA_USER_PARTITION_DSP_uCODE,
                                                pFileName,
                                                0U,
                                                LoadSize,
                                                pBuf,
                                                1000U);

                        Rval |= AmbaUserRsaSigVerify(pBuf, LoadSize);
                        // ULDR_DBG("## RSA verify finished", 0U, 0U);
                        #else
                        /* CRC check */
                        Rval = SvcNvm_ReadRomFileCrc( AMBA_USER_PARTITION_DSP_uCODE,
                                                    pFileName,
                                                    0U,
                                                    LoadSize,
                                                    pBuf,
                                                    1000U);
                        #endif

                        /* if ucode mem is cache, need to do cache clean */
                        Cache_CheckNClean(pBuf, LoadSize);

                        if (NVM_ERR_NONE == Rval) {
                            #if defined(CONFIG_ICAM_TIMING_LOG)
                            SvcTime_CalUcodeDataSize(LoadSize);
                            #endif
                            LoadSize -= RSA_SIG_SIZE;

                            SVC_WRAP_PRINT "## [Step 0] load ucode('%s') to %p, size = 0x%08X"
                            SVC_PRN_ARG_S               SVC_LOG_ULDR
                            SVC_PRN_ARG_CSTR pFileName  SVC_PRN_ARG_POST
                            SVC_PRN_ARG_CPOINT pBuf     SVC_PRN_ARG_POST
                            SVC_PRN_ARG_UINT32 LoadSize SVC_PRN_ARG_POST
                            SVC_PRN_ARG_E

                            pBufOff[i] += LoadSize;
                        } else {
                            ULDR_NG("## [Step 0] AmbaNVM_ReadRomFile fail to load U%u", i, 0U);
                        }
                    } else {
                        ULDR_NG("## [Step 0] buffer is too small, (%u/%u)", (UINT32)(UInfo[i].BufSize), UInfo[i].FileSize);
                    }
                }
            }

            if (SVC_OK == Rval) {
                #if defined(CONFIG_ICAM_TIMING_LOG)
                SvcTime(SVC_TIME_UCODE_LOAD_DONE_PART, "Ucode load partial DONE");
                #endif
                ULDR_DBG("[Step 0] load done", 0U, 0U);
                UcodeSrc = UCODE_FILE_SRC_BOOTDEV;

                if (1U == *pIsPartial) {
                    (void)AmbaKAL_GetSysTickCount(&TimeE);
                    ULDR_OK("partial ucodes are loaded, time = %u ms", (TimeE - TimeS), 0U);
                #if defined(CONFIG_ICAM_UCODE_ORCIDSP_SHARING)
                    (void)AmbaDSP_ParLoadConfig(1U, 1U);
                #else
                    (void)AmbaDSP_ParLoadConfig(1U, 0U);
                #endif
                    (void)AmbaDSP_ParLoadRegionUnlock(DSP_REGION_VOUT_IDX, 0U);
                    (void)AmbaDSP_ParLoadRegionUnlock(DSP_REGION_LV_IDX, 0U);
                    (void)AmbaKAL_GetSysTickCount(&TimeE);
                }
            }
        }
    }

    return Rval;
}

UINT32 SvcUcode_LoadStage1(UINT32* pBufOff, const UINT32* pIsPartial)
{
    UINT32  Rval = SVC_OK, TimeS, TimeE;

    if (g_BootStatus == SVCDSP_OP_CLEANBOOT) {
        (void) AmbaKAL_GetSysTickCount(&TimeS);
        if (1U == *pIsPartial) {
            UINT32     LoadSize, Step, i;
            const char *pFileName;
            UINT8      *pBuf;
            ULONG      LoadAddr;

            /* step 1/2 loading */
            for (Step = UCODE_PARTIAL_PAR1; Step < UCODE_PARTIAL_MAX; Step++) {

                if (Step == UCODE_PARTIAL_PAR2) {
                    continue;
                }

                for (i = 0U; i < UCODE_FILE_NUM_MAX; i++) {
                #if defined(CONFIG_ICAM_UCODE_ORCIDSP_SHARING) && defined(UCODE_FILE_ORCIDSP1)
                    if (i == UCODE_FILE_ORCIDSP1) {
                        continue;
                    }
                #endif

                    if (0U < UInfo[i].Partials[Step].PartSize) {
                        LoadAddr  =  UInfo[i].BufBase + pBufOff[i];
                        pFileName =  UInfo[i].Partials[Step].PartName;
                        LoadSize  = (UInfo[i].Partials[Step].PartSize + RSA_SIG_SIZE);

                        AmbaMisra_TypeCast(&pBuf, &LoadAddr);
                        #if defined(CONFIG_BUILD_FOSS_MBEDTLS) && defined(CONFIG_ENABLE_SECURITY)
                        /* RSA signature check */
                        Rval = SvcNvm_ReadRomFile( AMBA_USER_PARTITION_DSP_uCODE,
                                                pFileName,
                                                0U,
                                                LoadSize,
                                                pBuf,
                                                1000U);
                        Rval |= AmbaUserRsaSigVerify(pBuf, LoadSize);
                        // ULDR_DBG("## RSA verify finished", 0U, 0U);
                        #else
                        /* CRC check */
                        Rval = SvcNvm_ReadRomFileCrc( AMBA_USER_PARTITION_DSP_uCODE,
                                                    pFileName,
                                                    0U,
                                                    LoadSize,
                                                    pBuf,
                                                    1000U);
                        #endif

                        /* if ucode mem is cache, need to do cache clean */
                        Cache_CheckNClean(pBuf, LoadSize);

                        if (NVM_ERR_NONE == Rval) {
                            LoadSize -= RSA_SIG_SIZE;

                            SVC_WRAP_PRINT "## [Step 1/2] load ucode('%s') to %p, size = 0x%08X"
                            SVC_PRN_ARG_S               SVC_LOG_ULDR
                            SVC_PRN_ARG_CSTR pFileName  SVC_PRN_ARG_POST
                            SVC_PRN_ARG_CPOINT pBuf     SVC_PRN_ARG_POST
                            SVC_PRN_ARG_UINT32 LoadSize SVC_PRN_ARG_POST
                            SVC_PRN_ARG_E

                            pBufOff[i] += LoadSize;
                        } else {
                            ULDR_NG("## [Step 1/2] AmbaNVM_ReadRomFile fail to load U%u", i, 0U);
                        }
                    }
                }
            }
        }

        (void)AmbaKAL_GetSysTickCount(&TimeE);
        ULDR_OK("all ucodes are loaded, stap 2 due time = %u ms", (TimeE - TimeS), 0U);
        #if defined(CONFIG_ICAM_TIMING_LOG)
        SvcTime(SVC_TIME_UCODE_LOAD_DONE, "Ucode load all DONE");
        SvcTime_PrintBootTime();
        #endif
    }

    return Rval;
}

/**
* Update ucode encode and decode regions are loaded done
*/
void SvcUcode_LoadStage1Done(void)
{
    /* we should only unlock ucode encode and decode region after dsp bootup */
    (void)AmbaDSP_ParLoadRegionUnlock(DSP_REGION_ENC_IDX, 0U);
    (void)AmbaDSP_ParLoadRegionUnlock(DSP_REGION_DEC_IDX, 0U);
#if defined(CONFIG_ICAM_UCODE_ORCIDSP_SHARING)
    (void)AmbaDSP_ParLoadConfig(0U, 1U);
#else
    (void)AmbaDSP_ParLoadConfig(0U, 0U);
#endif
}

UINT32 SvcUcode_LoadFromStorage(void)
{
    UINT8                *pBuf;
    ULONG                LoadAddr;
    UINT32               i, Rval = SVC_OK;
    UINT32               LoadSize;
    UINT32               IsLoaded = 0U;
    AMBA_FS_FILE         *pFile;
    AMBA_FS_FILE_INFO_s  FileInfo;
    AMBA_SD_CARD_STATUS_s CardStatus;

    if (g_BootStatus == SVCDSP_OP_CLEANBOOT) {
        #if defined(CONFIG_ICAM_TIMING_LOG)
        SvcTime(SVC_TIME_UCODE_LOAD_START, "Ucode load from storage START");
        #endif

        /* Wait SD card init done */
        do {
            Rval = AmbaSD_GetCardStatus(0, &CardStatus);
            if (SVC_OK != Rval) {
                break;
            }
            (void)AmbaKAL_TaskSleep(1);
        } while (CardStatus.CardIsInit == (UINT8)0);

        for (i = 0; i < UCODE_FILE_NUM_MAX; i++) {
            Rval = AmbaFS_GetFileInfo(UInfo[i].FileName, &FileInfo);

            if (AMBA_FS_ERR_NONE == Rval) {
                IsLoaded = 0U;
                UInfo[i].FileSize = (UINT32)FileInfo.Size;

                if (UInfo[i].BufSize < FileInfo.Size) {
                    ULDR_NG("## buffer is too small, (%u/%u)"
                                            , (UINT32)(UInfo[i].BufSize)
                                            , UInfo[i].FileSize);
                } else {
                    Rval = AmbaFS_FileOpen(UInfo[i].FileName, "r+b", &pFile);
                    if (AMBA_FS_ERR_NONE == Rval) {
                        LoadAddr = UInfo[i].BufBase;
                        AmbaMisra_TypeCast(&pBuf, &LoadAddr);
                        Rval = AmbaFS_FileRead(pBuf, 1U, (UINT32)FileInfo.Size, pFile, &LoadSize);
                        if (AMBA_FS_ERR_NONE == Rval) {
                            if (LoadSize != FileInfo.Size) {
                                ULDR_NG("## fail to load U%u file", i, 0U);
                            }

                            Rval = AmbaFS_FileClose(pFile);

                            /* if ucode mem is cache, need to do cache clean */
                            Cache_CheckNClean(pBuf, LoadSize);

                            if (AMBA_FS_ERR_NONE == Rval) {
                                IsLoaded = 1U;
                                #if defined(CONFIG_ICAM_TIMING_LOG)
                                SvcTime_CalUcodeDataSize(LoadSize);
                                #endif
                                SVC_WRAP_PRINT "load to %p, size = 0x%08X"
                                    SVC_PRN_ARG_S SVC_LOG_ULDR
                                    SVC_PRN_ARG_CPOINT pBuf     SVC_PRN_ARG_POST
                                    SVC_PRN_ARG_UINT32 LoadSize SVC_PRN_ARG_POST
                                    SVC_PRN_ARG_E
                            } else {
                                ULDR_NG("## fail to close U%u file", i, 0U);
                            }
                        }
                    }
                }

                if (0U == IsLoaded) {
                    break;
                }
            }
        }

        if (SVC_OK == Rval) {
            UcodeSrc = UCODE_FILE_SRC_SD;
            #if defined(CONFIG_ICAM_TIMING_LOG)
            SvcTime(SVC_TIME_UCODE_LOAD_DONE_PART, NULL);
            SvcTime(SVC_TIME_UCODE_LOAD_DONE, "Ucode load from storage DONE");
            SvcTime_PrintBootTime();
            #endif
        }
    }

    return Rval;
}

void SvcUcode_Config(void)
{
    const void *pBuf;
    extern UINT32 AmbaDSP_MainGetDspBinAddr(AMBA_DSP_BIN_ADDR_s *Info);

    AMBA_DSP_BIN_ADDR_s  DspBin = {0};

    (void)AmbaDSP_MainGetDspBinAddr(&DspBin);

    /* fill load address and size */
    UInfo[UCODE_FILE_DEFBIN].BufBase  = DspBin.DefaultDataStartAddr;
    UInfo[UCODE_FILE_DEFBIN].BufSize  = DspBin.DefaultDataEndAddr;
    AmbaMisra_TypeCast(&pBuf, &(UInfo[UCODE_FILE_DEFBIN].BufBase));
    SVC_WRAP_PRINT "defbin base/size(%p/0x%X)"
        SVC_PRN_ARG_S SVC_LOG_ULDR
        SVC_PRN_ARG_CPOINT pBuf                              SVC_PRN_ARG_POST
        SVC_PRN_ARG_UINT32 UInfo[UCODE_FILE_DEFBIN].BufSize SVC_PRN_ARG_POST
        SVC_PRN_ARG_E

    UInfo[UCODE_FILE_ORCCODE].BufBase = DspBin.CoreStartAddr;
    UInfo[UCODE_FILE_ORCCODE].BufSize = DspBin.CoreEndAddr;
    AmbaMisra_TypeCast(&pBuf, &(UInfo[UCODE_FILE_ORCCODE].BufBase));
    SVC_WRAP_PRINT "orccode base/size(%p/0x%X)"
        SVC_PRN_ARG_S SVC_LOG_ULDR
        SVC_PRN_ARG_CPOINT pBuf                              SVC_PRN_ARG_POST
        SVC_PRN_ARG_UINT32 UInfo[UCODE_FILE_ORCCODE].BufSize SVC_PRN_ARG_POST
        SVC_PRN_ARG_E

    UInfo[UCODE_FILE_ORCME0].BufBase   = DspBin.MeStartAddr;
    UInfo[UCODE_FILE_ORCME0].BufSize   = DspBin.MeEndAddr;
    AmbaMisra_TypeCast(&pBuf, &(UInfo[UCODE_FILE_ORCME0].BufBase));
    SVC_WRAP_PRINT "orcme0 base/size(%p/0x%X)"
        SVC_PRN_ARG_S SVC_LOG_ULDR
        SVC_PRN_ARG_CPOINT pBuf                              SVC_PRN_ARG_POST
        SVC_PRN_ARG_UINT32 UInfo[UCODE_FILE_ORCME0].BufSize SVC_PRN_ARG_POST
        SVC_PRN_ARG_E

    UInfo[UCODE_FILE_ORCMD0].BufBase   = DspBin.MdxfStartAddr;
    UInfo[UCODE_FILE_ORCMD0].BufSize   = DspBin.MdxfEndAddr;
    AmbaMisra_TypeCast(&pBuf, &(UInfo[UCODE_FILE_ORCMD0].BufBase));
    SVC_WRAP_PRINT "orcmd0 base/size(%p/0x%X)"
        SVC_PRN_ARG_S SVC_LOG_ULDR
        SVC_PRN_ARG_CPOINT pBuf                              SVC_PRN_ARG_POST
        SVC_PRN_ARG_UINT32 UInfo[UCODE_FILE_ORCMD0].BufSize SVC_PRN_ARG_POST
        SVC_PRN_ARG_E

    #if defined(UCODE_FILE_ORCME1)
    UInfo[UCODE_FILE_ORCME1].BufBase   = DspBin.Me1StartAddr;
    UInfo[UCODE_FILE_ORCME1].BufSize   = DspBin.Me1EndAddr;
    AmbaMisra_TypeCast(&pBuf, &(UInfo[UCODE_FILE_ORCME1].BufBase));
    SVC_WRAP_PRINT "orcme1 base/size(%p/0x%X)"
        SVC_PRN_ARG_S SVC_LOG_ULDR
        SVC_PRN_ARG_CPOINT pBuf                              SVC_PRN_ARG_POST
        SVC_PRN_ARG_UINT32 UInfo[UCODE_FILE_ORCME1].BufSize SVC_PRN_ARG_POST
        SVC_PRN_ARG_E
    #endif
    #if defined(UCODE_FILE_ORCMD1)
    UInfo[UCODE_FILE_ORCMD1].BufBase   = DspBin.Mdxf1StartAddr;
    UInfo[UCODE_FILE_ORCMD1].BufSize   = DspBin.Mdxf1EndAddr;
    AmbaMisra_TypeCast(&pBuf, &(UInfo[UCODE_FILE_ORCMD1].BufBase));
    SVC_WRAP_PRINT "orcmd1 base/size(%p/0x%X)"
        SVC_PRN_ARG_S SVC_LOG_ULDR
        SVC_PRN_ARG_CPOINT pBuf                              SVC_PRN_ARG_POST
        SVC_PRN_ARG_UINT32 UInfo[UCODE_FILE_ORCMD1].BufSize SVC_PRN_ARG_POST
        SVC_PRN_ARG_E
    #endif
    #if defined(UCODE_FILE_ORCVIN)
    UInfo[UCODE_FILE_ORCVIN].BufBase = DspBin.Core1StartAddr;
    UInfo[UCODE_FILE_ORCVIN].BufSize = DspBin.Core1EndAddr;
    AmbaMisra_TypeCast(&pBuf, &(UInfo[UCODE_FILE_ORCVIN].BufBase));
    SVC_WRAP_PRINT "orcvin base/size(%p/0x%X)"
        SVC_PRN_ARG_S SVC_LOG_ULDR
        SVC_PRN_ARG_CPOINT pBuf                              SVC_PRN_ARG_POST
        SVC_PRN_ARG_UINT32 UInfo[UCODE_FILE_ORCVIN].BufSize SVC_PRN_ARG_POST
        SVC_PRN_ARG_E
    #endif
    #if defined(UCODE_FILE_ORCIDSP0)
    UInfo[UCODE_FILE_ORCIDSP0].BufBase = DspBin.Core2StartAddr;
    UInfo[UCODE_FILE_ORCIDSP0].BufSize = DspBin.Core2EndAddr;
    AmbaMisra_TypeCast(&pBuf, &(UInfo[UCODE_FILE_ORCIDSP0].BufBase));
    SVC_WRAP_PRINT "orcidsp0 base/size(%p/0x%X)"
        SVC_PRN_ARG_S SVC_LOG_ULDR
        SVC_PRN_ARG_CPOINT pBuf                              SVC_PRN_ARG_POST
        SVC_PRN_ARG_UINT32 UInfo[UCODE_FILE_ORCIDSP0].BufSize SVC_PRN_ARG_POST
        SVC_PRN_ARG_E
    #endif
    #if defined(UCODE_FILE_ORCIDSP1)
    UInfo[UCODE_FILE_ORCIDSP1].BufBase = DspBin.Core3StartAddr;
    UInfo[UCODE_FILE_ORCIDSP1].BufSize = DspBin.Core3EndAddr;
    AmbaMisra_TypeCast(&pBuf, &(UInfo[UCODE_FILE_ORCIDSP1].BufBase));
    SVC_WRAP_PRINT "orcidsp1 base/size(%p/0x%X)"
        SVC_PRN_ARG_S SVC_LOG_ULDR
        SVC_PRN_ARG_CPOINT pBuf                              SVC_PRN_ARG_POST
        SVC_PRN_ARG_UINT32 UInfo[UCODE_FILE_ORCIDSP1].BufSize SVC_PRN_ARG_POST
        SVC_PRN_ARG_E
    #endif

    /* currently the ucode source is unknown,
       this flag will be changed once it loaded success from boot dev or from SD card */
    UcodeSrc = UCODE_FILE_SRC_UNKNOWN;

    /* get dsp boot status */
    g_BootStatus = SvcDSP_IsCleanBoot();
}

/**
* get ucode info
* @param [in] ID uCode id
* @param [out] pBufBase buffer base
* @param [out] pBufSize buffer size
* @param [out] pBinSize binary file size
* @return none
*/
void SvcUcode_GetInfo(UINT32 ID, ULONG *pBufBase, UINT32 *pBufSize, UINT32 *pBinSize)
{
    if (ID < UCODE_FILE_NUM_MAX) {
        if (pBufBase != NULL) {
            *pBufBase = UInfo[ID].BufBase;
        }
        if (pBufSize != NULL) {
            *pBufSize = (UINT32)(UInfo[ID].BufSize);
        }
        if (pBinSize != NULL) {
            if (UcodeSrc == UCODE_FILE_SRC_SD) {
                *pBinSize = UInfo[ID].FileSize;
            } else if (UcodeSrc == UCODE_FILE_SRC_BOOTDEV) {
                *pBinSize = (UInfo[ID].FileSize - RSA_SIG_SIZE);
            } else {
                ULDR_NG("## SvcUcode_GetInfo: unknown ucode src type %d", UcodeSrc, 0U);
            }
        }
    }
}

/**
* load uCode string table region
* @return none
*/
void SvcUcode_LoadStrTbl(void)
{
    const char   *pFileName;
    UINT8        *pBuf;
    ULONG         LoadAddr;
    UINT32        LoadSize, Step = UCODE_PARTIAL_PAR2, StepIdx;
    UINT32        BufOff[UCODE_FILE_NUM_MAX];
    UINT32        i, Rval = SVC_OK;
    AMBA_PARTITION_ENTRY_s  PartInfo;

    if (g_BootStatus == SVCDSP_OP_CLEANBOOT) {
        /* Check ucode data in NAND or not */
        (void) SvcNvm_ReadPartitionInfo(1U, AMBA_USER_PARTITION_DSP_uCODE, &PartInfo);
        ULDR_OK("Ucode size inside NVM = 0x%X", PartInfo.ActualByteSize, 0U);
        if (0U == PartInfo.ActualByteSize) {
            ULDR_OK("No ucode inside NVM", 0U, 0U);
            Rval = SVC_NG;
        }

        if (SVC_OK == Rval) {
            AmbaSvcWrap_MisraMemset(BufOff, 0, sizeof(BufOff));

            for (i = 0U; i < UCODE_FILE_NUM_MAX; i++) {
                for (StepIdx = UCODE_PARTIAL_PAR0; StepIdx < Step; StepIdx ++) {
                    BufOff[i] += UInfo[i].Partials[StepIdx].PartSize;
                }
            }

            for (i = 0U; i < UCODE_FILE_NUM_MAX; i++) {
            #if defined(CONFIG_ICAM_UCODE_ORCIDSP_SHARING) && defined(UCODE_FILE_ORCIDSP1)
                if (i == UCODE_FILE_ORCIDSP1) {
                    continue;
                }
            #endif

                if (0U < UInfo[i].Partials[Step].PartSize) {
                    LoadAddr  = UInfo[i].BufBase + BufOff[i];
                    pFileName = UInfo[i].Partials[Step].PartName;
                    LoadSize  = UInfo[i].Partials[Step].PartSize + RSA_SIG_SIZE;

                    AmbaMisra_TypeCast(&pBuf, &LoadAddr);

                    Rval = SvcNvm_ReadRomFileCrc( AMBA_USER_PARTITION_DSP_uCODE,
                                                pFileName,
                                                0U,
                                                LoadSize,
                                                pBuf,
                                                1000U);

                    /* if ucode mem is cache, need to do cache clean */
                    Cache_CheckNClean(pBuf, LoadSize);

                    if (NVM_ERR_NONE == Rval) {
                        #if defined(CONFIG_ICAM_TIMING_LOG)
                        SvcTime_CalUcodeDataSize(LoadSize);
                        #endif
                        LoadSize -= RSA_SIG_SIZE;
                        SVC_WRAP_PRINT "[Step 1/2] load to %p, size = 0x%08X"
                            SVC_PRN_ARG_S SVC_LOG_ULDR
                            SVC_PRN_ARG_CPOINT pBuf     SVC_PRN_ARG_POST
                            SVC_PRN_ARG_UINT32 LoadSize SVC_PRN_ARG_POST
                            SVC_PRN_ARG_E
                        BufOff[i] += LoadSize;
                    } else {
                        ULDR_NG("## fail to load ucode_%u_%u", Step, i);
                    }
                }
            }
        }
    }
}
