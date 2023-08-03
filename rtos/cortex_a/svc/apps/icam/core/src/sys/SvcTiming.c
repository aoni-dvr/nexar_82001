/**
*  @file SvcBootTiming.c
*
*  @copyright Copyright (c) 2015 Ambarella, Inc.
*
*  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
*  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
*  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
*  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
*  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
*  return this Software to Ambarella, Inc.
*
*  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
*  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
*  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
*  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
*  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
*  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
*  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*  @details svc boot timing measurement
*
*/

#include "AmbaTMR.h"
#include "AmbaSvcWrap.h"
#include ".svc_autogen"
#include "SvcResCfg.h"
#include "SvcVinSrc.h"
#include "SvcPlat.h"
#include "SvcTiming.h"

#if defined(CONFIG_ENABLE_NAND_BOOT)
#include "AmbaRTSL_NAND.h"
#define SVC_TIME_STD_SYSSW_LOAD      SVC_TIME_STD_SYSSW_LOAD_NAND
#define SVC_TIME_STD_UCODE_PART_LOAD SVC_TIME_STD_UCODE_PART_LOAD_NAND
#define AmbaRTSL_NvmGetPartEntry     AmbaRTSL_NandGetPartEntry
#define BOOT_DEV                     "NAND"
#elif defined(CONFIG_ENABLE_SPINAND_BOOT)
#include "AmbaRTSL_NAND.h"
#define SVC_TIME_STD_SYSSW_LOAD      SVC_TIME_STD_SYSSW_LOAD_NAND
#define SVC_TIME_STD_UCODE_PART_LOAD SVC_TIME_STD_UCODE_PART_LOAD_NAND
#define AmbaRTSL_NvmGetPartEntry     AmbaRTSL_NandGetPartEntry
#define BOOT_DEV                     "SPI-NAND"
#elif defined(CONFIG_ENABLE_SPINOR_BOOT)
#include "AmbaRTSL_SpiNOR.h"
#define SVC_TIME_STD_SYSSW_LOAD      SVC_TIME_STD_SYSSW_LOAD_SPINOR
#define SVC_TIME_STD_UCODE_PART_LOAD SVC_TIME_STD_UCODE_PART_LOAD_SPINOR
#define AmbaRTSL_NvmGetPartEntry     AmbaRTSL_NorSpiGetPartEntry
#define BOOT_DEV                     "SPI-NOR"
#elif defined(CONFIG_ENABLE_EMMC_BOOT)
#include "AmbaRTSL_SD.h"
#define SVC_TIME_STD_SYSSW_LOAD      SVC_TIME_STD_SYSSW_LOAD_EMMC
#define SVC_TIME_STD_UCODE_PART_LOAD SVC_TIME_STD_UCODE_PART_LOAD_EMMC
#define AmbaRTSL_NvmGetPartEntry     AmbaRTSL_EmmcGetPartEntry
#define BOOT_DEV                     "EMMC"
#else
#pragma message ("[SvcTiming.c] ERROR !! UNKNOWN BOOT DEVICE !!")
#endif

#if defined(CONFIG_THREADX) && defined(CONFIG_THREADX64)
#define BOOT_APP                     "TX64"
#elif defined(CONFIG_THREADX)
#define BOOT_APP                     "TX32"
#elif defined(CONFIG_LINUX)
#define BOOT_APP                     "LINUX"
#else
#pragma message ("[SvcTiming.c] ERROR !! UNKNOWN APP TYPE !!")
#endif

#define SVC_LOG_TIMING   "SvcTime"
#define SVC_PORL_TIME_MS (36U)
#define PUInt           AmbaPrint_PrintUInt5
#define PStr            AmbaPrint_PrintStr5
static inline void TIM_NG( const char *pFormat, UINT32 Arg1, UINT32 Arg2) { SvcLog_NG( SVC_LOG_TIMING, pFormat, Arg1, Arg2); }
static inline void TIM_OK( const char *pFormat, UINT32 Arg1, UINT32 Arg2) { SvcLog_OK( SVC_LOG_TIMING, pFormat, Arg1, Arg2); }

/*------------------------------------------------------------------------------------------*\
 *   since the latest time stamp can be recorded at the end of
 *      1. DSP boot done
 *      2. DSP ucode load all done
 *      3. Record immediately after booting
 *      4. Auto recording start (if enabled)
 *
 *   to make sure all value printed in time log is valid,
 *   so we added flag here to check these 2 time stamp are all done.
\*------------------------------------------------------------------------------------------*/
#define FLG_DSP_BOOT_DONE         (1U)
#define FLG_UCODE_LOAD_DONE       (2U)
#define FLG_IQ_LOAD_DONE          (4U)
#define FLG_REC_START             (8U)

static UINT32 Flg_PrintCheck     = 0U;
static UINT32 Flg_HasIq          = 0U;
#if defined(CONFIG_AMBALINK_BOOT_OS)
static UINT32 Size_AmbaLinkData  = 0U;
#endif
static UINT32 Size_CvVisOrc      = 0U;
static UINT32 Size_CvTotal       = 0U;
static UINT32 Size_Ucode         = 0U;
static UINT32 Size_Bist          = 0U;
       UINT32 SvcTime_ForcePrint = 0U;

static UINT32 kb(UINT32 n) { return n/1024U; }
static UINT32 ms(UINT32 t) { return t/24000U; }
static UINT32 us(UINT32 t) { return (t/24U)%1000U; }

static void GetAppBssSize(UINT32* pBssSize)
{
    extern UINT32 __bss_start;
    extern UINT32 __bss_end;
    const  UINT32* p__bss_start = &__bss_start;
    const  UINT32* p__bss_end   = &__bss_end;
    ULONG  bss_start_addr       = 0U;
    ULONG  bss_end_addr         = 0U;

    AmbaMisra_TypeCast(&bss_start_addr, &p__bss_start);
    AmbaMisra_TypeCast(&bss_end_addr,   &p__bss_end);
    *pBssSize = (UINT32)(bss_end_addr - bss_start_addr);
}

static UINT32 GetBldBootInfoPtr(AMBA_TIMER_INFO_s** ppTime)
{
    UINT32 Rval = SVC_NG;

    if ((g_pPlatCbEntry != NULL) && (g_pPlatCbEntry->pCbTimeProfleGet != NULL)) {
        g_pPlatCbEntry->pCbTimeProfleGet((void**)ppTime);

        Rval = SVC_OK;
    }

    return Rval;
}

static void TimingShift(UINT32 ShiftMs)
{
    static UINT8 Done = 0U;

    if (0U == Done) {
        AMBA_TIMER_INFO_s *pTime = NULL;
        UINT32 Rval, i;

        Rval = GetBldBootInfoPtr(&pTime);
        if (SVC_OK == Rval) {
            /* turn it into tick of RCT (24MHz) */
            UINT32 ShiftMs_Tick = ShiftMs * 24000U;

            for (i = 0; i < STAMP_NUM; i++) {
                #if defined(CONFIG_SOC_CV2FS)
                pTime->R52_BLD_Time[i] += ShiftMs_Tick;
                pTime->R52_APP_Time[i] += ShiftMs_Tick;
                #endif
                pTime->A53_BLD_Time[i] += ShiftMs_Tick;
                pTime->A53_APP_Time[i] += ShiftMs_Tick;
            }

            Done = 1U;
        }
    }
}

static UINT32 isBootToRec(void)
{
    static UINT32 Done      = 0U;
    static UINT32 BootToRec = 0U;

    if (0U == Done) {
        const SVC_RES_CFG_s *pCfg = SvcResCfg_Get();
        UINT32 i;
        for (i = 0U; i < pCfg->RecNum; i++) {
            if (pCfg->RecStrm[i].RecSetting.BootToRec > 0U) {
                BootToRec = 1U;
                break;
            }
        }
        Done = 1U;
    }

    return BootToRec;
}

static UINT32 CheckFlag(void)
{
    static UINT32 Done = 0U;
    static UINT32 Flg_PrintReady = 0xFFFFFFFFU;
    UINT32 check = 0U;
    UINT32 Rval;

    if (0U == Done) {
        UINT32 i, VinNum, VinSrc = 255U;
        UINT32 VinIndex[AMBA_DSP_MAX_VIRT_VIN_NUM];

        Rval  = AmbaWrap_memset(VinIndex, 0, sizeof(VinIndex));
        Rval |= SvcResCfg_GetVinIDs(VinIndex, &VinNum);
        if (SVC_OK != Rval) {
            TIM_NG("CheckFlag: SvcResCfg_GetVinIDs failed", 0U, 0U);
            Rval = SVC_NG;
        } else {
            for (i = 0; i < VinNum; i++) {
                Rval = SvcResCfg_GetVinSrc(VinIndex[i], &VinSrc);
                if (SVC_OK == Rval) {
                    TIM_OK("CheckFlag: Vin %d VinSrc %d", VinIndex[i], VinSrc);
                    if (VinSrc == SVC_VIN_SRC_SENSOR) {
                        Flg_HasIq = 1U;
                    }
                } else {
                    TIM_NG("CheckFlag: SvcResCfg_GetVinSrc failed !! Vin %d VinSrc %d", VinIndex[i], VinSrc);
                }
            }

            if (0U == Flg_HasIq) {
                Flg_PrintReady = ( FLG_DSP_BOOT_DONE | FLG_UCODE_LOAD_DONE );
            } else {
                Flg_PrintReady = ( FLG_DSP_BOOT_DONE | FLG_UCODE_LOAD_DONE | FLG_IQ_LOAD_DONE );
            }

            if (0U != isBootToRec()) {
                Flg_PrintReady |= FLG_REC_START;
            }

            Done = 1U;
        }
    }

    if (Done != 0U) {
        AmbaMisra_TouchUnused(&Rval);
        check = Flg_PrintCheck & Flg_PrintReady;
        if (check == Flg_PrintReady) {
            Rval = SVC_OK;
        } else {
            Rval = SVC_NG;
        }
    } else {
        Rval = SVC_NG;
    }

    // TIM_OK("CheckFlag: check 0x%X, PrintReady 0x%X", check, Flg_PrintReady);
    return Rval;
}

void SvcTime_CalAmbaLinkDataSize(UINT32 DataSize)
{
    #if defined(CONFIG_AMBALINK_BOOT_OS)
    Size_AmbaLinkData += DataSize;
    #else
    AmbaMisra_TouchUnused(&DataSize);
    #endif
}

void SvcTime_CalCvVisOrcSize(UINT32 DataSize)  { Size_CvVisOrc += DataSize; }
void SvcTime_CalCvTotalSize(UINT32 DataSize)   { Size_CvTotal  += DataSize; }
void SvcTime_CalUcodeDataSize(UINT32 DataSize) { Size_Ucode    += DataSize; }
void SvcTime_CalBistSize(UINT32 DataSize)      { Size_Bist     += DataSize; }

void SvcTime(UINT32 id, const char* pMsg)
{
    if ((g_pPlatCbEntry != NULL) && (g_pPlatCbEntry->pCbTimeProfleSet != NULL)) {
        g_pPlatCbEntry->pCbTimeProfleSet(id);
    }

    if (NULL != pMsg) {
        TIM_OK(pMsg, 0U, 0U);
    }

    switch (id)
    {
        case SVC_TIME_DSP_BOOT_DONE:
            Flg_PrintCheck |= FLG_DSP_BOOT_DONE;
            break;
        case SVC_TIME_IQ_LOAD_DONE:
            Flg_PrintCheck |= FLG_IQ_LOAD_DONE;
            break;
        case SVC_TIME_UCODE_LOAD_DONE:
            Flg_PrintCheck |= FLG_UCODE_LOAD_DONE;
            break;
        case SVC_TIME_REC_START:
            Flg_PrintCheck |= FLG_REC_START;
            break;
        default:
            /* other flag */
            break;
    }
}

void SvcTime_PrintBootTime(void)
{
    UINT32                 Rval;
    AMBA_TIMER_INFO_s      *pTime = NULL;

    Rval = CheckFlag();
    if ((SVC_OK == Rval) || (0U != SvcTime_ForcePrint)) {
        Rval = GetBldBootInfoPtr(&pTime);
    }

    if ((SVC_OK == Rval) && (NULL != pTime)) {
        const AMBA_PARTITION_ENTRY_s *pPartEntry;
        const UINT32* pAPP   = pTime->A53_APP_Time;
        const UINT32* pBLD   = pTime->A53_BLD_Time;
        #if defined(CONFIG_SOC_CV2FS)
        const UINT32* pBLD52 = pTime->R52_BLD_Time;

        UINT32 Diff_1  = pBLD52[R52_BLD_POLL_USER_STOP]      - pBLD52[R52_BLD_DEV_INIT_DONE];
        UINT32 Diff_2  = pBLD52[R52_BLD_LOAD_BLD2_DONE]      - pBLD52[R52_BLD_LOAD_BLD2_START];
        UINT32 Diff_3  = pBLD52[R52_BLD_LOAD_SYS_DONE]       - pBLD52[R52_BLD_LOAD_SYS_START];
        #endif
        UINT32 Diff_4  = pBLD[A53_BLD_POLL_USER_STOP]        - pBLD[A53_BLD_SHELL_START];
        #if defined(CONFIG_BLD_SEQ_ATF)
        UINT32 Diff_5  = pBLD[A53_BLD_LOAD_ATF_DONE]         - pBLD[A53_BLD_LOAD_ATF_START];
        #endif
        #if defined(CONFIG_LINUX)
        UINT32 Diff_6  = pBLD[A53_BLD_LOAD_LINUX_DONE]       - pBLD[A53_BLD_LOAD_LINUX_START];
        #else
        UINT32 Diff_6  = pBLD[A53_BLD_LOAD_SYS_DONE]         - pBLD[A53_BLD_LOAD_SYS_START];
        #endif
        UINT32 Diff_7  = pAPP[SVC_TIME_POSTOS_CONFIG]        - pBLD[A53_BLD_ENTER_ATF];
        UINT32 Diff_8  = pAPP[SVC_TIME_UCODE_LOAD_DONE_PART] - pAPP[SVC_TIME_UCODE_LOAD_START];
        UINT32 Diff_9  = pAPP[SVC_TIME_VIN_CONFIG_DONE]      - pAPP[SVC_TIME_VIN_CONFIG_START];
        UINT32 Diff_10 = pAPP[SVC_TIME_VOUT_CONFIG_DONE]     - pAPP[SVC_TIME_VOUT_CONFIG_START];
        UINT32 Diff_11 = pAPP[SVC_TIME_DSP_BOOT_DONE]        - pAPP[SVC_TIME_DSP_BOOT_START];
        #if defined(CONFIG_ICAM_BIST_UCODE)
        UINT32 Diff_12 = pAPP[SVC_TIME_BIST_LOAD_DONE]       - pAPP[SVC_TIME_BIST_LOAD_START];
        #endif
        #if defined(CONFIG_FASTBOOT_LZ4)
        UINT32 Diff_13 = pBLD[A53_BLD_DECOMPRESS_DONE]       - pBLD[A53_BLD_DECOMPRESS_START];
        #endif
        UINT32 Size_AppBss = 0U;
        UINT32 Size_3 = 0U;
        #if defined(CONFIG_BLD_SEQ_ATF)
        UINT32 Size_2 = 0U;
        #endif
        #if defined(CONFIG_SOC_CV2FS)
        UINT32 Size_1 = 0U;

        pPartEntry = AmbaRTSL_NvmGetPartEntry(1U, AMBA_USER_PARTITION_R52SYS);
        if (NULL != pPartEntry) {
            Size_1 = pPartEntry->ActualByteSize;
        }
        #endif
        #if defined(CONFIG_BLD_SEQ_ATF)
        pPartEntry = AmbaRTSL_NvmGetPartEntry(0U, AMBA_SYS_PARTITION_ARM_TRUST_FW);
        if (NULL != pPartEntry) {
            Size_2 = pPartEntry->ActualByteSize;
        }
        #endif
        #if defined(CONFIG_LINUX)
        pPartEntry = AmbaRTSL_NvmGetPartEntry(1U, AMBA_USER_PARTITION_LINUX_KERNEL);
        #else
        pPartEntry = AmbaRTSL_NvmGetPartEntry(1U, AMBA_USER_PARTITION_SYS_SOFTWARE);
        #endif
        if (NULL != pPartEntry) {
            Size_3 = pPartEntry->ActualByteSize;
        }

        GetAppBssSize(&Size_AppBss);
        TimingShift(SVC_PORL_TIME_MS);

        PStr( "*************** BootTimeInfo ***********************",          NULL, NULL, NULL, NULL, NULL);
        PStr( "   | BOARD                 %s",                      CONFIG_BSP_NAME, NULL, NULL, NULL, NULL);
        PStr( "   | BOOT DEV              %s",                             BOOT_DEV, NULL, NULL, NULL, NULL);
        PStr( "   | BOOT APP              %s",                             BOOT_APP, NULL, NULL, NULL, NULL);
        PStr( "   |",                                                          NULL, NULL, NULL, NULL, NULL);
        PStr( "-- STAGE 1 -----------------------------------------",          NULL, NULL, NULL, NULL, NULL);
        PStr( "   | POWER RAIL                      = 0000.000 ms",            NULL, NULL, NULL, NULL, NULL);
        PUInt("   | BST DRAM AND DEV INIT DONE      = %4u.000",    SVC_PORL_TIME_MS,   0U,   0U,   0U,   0U);
        #if defined(CONFIG_SOC_CV2FS)
        PUInt("   | ENTER BLD                       = %4u.%03u",               ms(pBLD52[R52_BLD_START]),                us(pBLD52[R52_BLD_START]),              0U,          0U,          0U);
        PStr( "-- STAGE 2 -----------------------------------------",          NULL, NULL, NULL, NULL, NULL);
        PUInt("   | BSS SIZE              %u",                                 pTime->R52_BssSize, 0U, 0U, 0U, 0U);
        PUInt("   | DEV INIT              START     = %4u.%03u",               ms(pBLD52[R52_BLD_DEV_INIT_START]),       us(pBLD52[R52_BLD_DEV_INIT_START]),     0U,          0U,          0U);
        PUInt("   |                       DONE      = %4u.%03u",               ms(pBLD52[R52_BLD_DEV_INIT_DONE]),        us(pBLD52[R52_BLD_DEV_INIT_DONE]),      0U,          0U,          0U);
        PUInt("   | POLLING USER          STOP      = %4u.%03u, wait %u.%03u", ms(pBLD52[R52_BLD_POLL_USER_STOP]),       us(pBLD52[R52_BLD_POLL_USER_STOP]),     ms(Diff_1),  us(Diff_1),  0U);
        PUInt("   | CHECK DRAM TRAINING   DONE      = %4u.%03u",               ms(pBLD52[R52_BLD_CHK_DRAM_TRAIN_DONE]),  us(pBLD52[R52_BLD_CHK_DRAM_TRAIN_DONE]),0U,          0U,          0U);
     // PUInt("   | LOAD FIRMWARE         START     = %4u.%03u",               ms(pBLD52[R52_BLD_LOAD_START]),           us(pBLD52[R52_BLD_LOAD_START]),         0U,          0U,          0U);
        PUInt("   | LOAD BLD2             DONE      = %4u.%03u, cost %u.%03u", ms(pBLD52[R52_BLD_LOAD_BLD2_DONE]),       us(pBLD52[R52_BLD_LOAD_BLD2_DONE]),     ms(Diff_2),  us(Diff_2),  0U);
        PUInt("   |      R52SYS           DONE      = %4u.%03u, cost %u.%03u, size %u KB", ms(pBLD52[R52_BLD_LOAD_SYS_DONE]), us(pBLD52[R52_BLD_LOAD_SYS_DONE]), ms(Diff_3),  us(Diff_3),  kb(Size_1));
     // PUInt("   | START A53                       = %4u.%03u",               ms(pBLD52[R52_BLD_BOOT_A53_ENTER]),       us(pBLD52[R52_BLD_BOOT_A53_ENTER]),     0U,          0U,          0U);
        PUInt("   | START R52SYS                    = %4u.%03u",               ms(pBLD52[R52_BLD_BOOT_R52SYS_ENTER]),    us(pBLD52[R52_BLD_BOOT_R52SYS_ENTER]),  0U,          0U,          0U);
        PStr( "---+------------------------------------------------",          NULL, NULL, NULL, NULL, NULL);
        #else
        PUInt("   | ENTER BLD                       = %4u.%03u",               ms(pBLD[A53_BLD_START]),                  us(pBLD[A53_BLD_START]),                0U,          0U,          0U);
        PStr( "-- STAGE 2 -----------------------------------------",          NULL, NULL, NULL, NULL, NULL);
        #endif
        PUInt("   | BSS SIZE              %u",                                 pTime->A53_BssSize, 0U, 0U, 0U, 0U);
     // PUInt("   | BLD2                  START     = %4u.%03u",               ms(pBLD[A53_BLD_START]),                  us(pBLD[A53_BLD_START]),                0U,          0U,          0U);
        PUInt("   | DEV INIT              START     = %4u.%03u",               ms(pBLD[A53_BLD_DEV_INIT_START]),         us(pBLD[A53_BLD_DEV_INIT_START]),       0U,          0U,          0U);
        PUInt("   |                       DONE      = %4u.%03u",               ms(pBLD[A53_BLD_DEV_INIT_DONE]),          us(pBLD[A53_BLD_DEV_INIT_DONE]),        0U,          0U,          0U);
     // PUInt("   | POLLING USER          STOP      = %4u.%03u, wait %u.%03u", ms(pBLD[A53_BLD_POLL_USER_STOP]),         us(pBLD[A53_BLD_POLL_USER_STOP]),       ms(Diff_4),  us(Diff_4),  0U);
        PUInt("   | CHECK DRAM TRAINING   DONE      = %4u.%03u",               ms(pBLD[A53_BLD_CHK_DRAM_TRAIN_DONE]),    us(pBLD[A53_BLD_CHK_DRAM_TRAIN_DONE]),  0U,          0U,          0U);
     // PUInt("   | LOAD FIRMWARE         START     = %4u.%03u",               ms(pBLD[A53_BLD_LOAD_START]),             us(pBLD[A53_BLD_LOAD_START]),           0U,          0U,          0U);
        #if defined(CONFIG_BLD_SEQ_ATF)
        PUInt("   | LOAD ATF              DONE      = %4u.%03u, cost %u.%03u, size %u KB", ms(pBLD[A53_BLD_LOAD_ATF_DONE]), us(pBLD[A53_BLD_LOAD_ATF_DONE]),     ms(Diff_5),  us(Diff_5),  kb(Size_2));
        #endif
        #if defined(CONFIG_LINUX)
        PUInt("   |      LNX              DONE      = %4u.%03u, cost %u.%03u, size %u KB", ms(pBLD[A53_BLD_LOAD_LINUX_DONE]), us(pBLD[A53_BLD_LOAD_LINUX_START]),     ms(Diff_6),  us(Diff_6),  kb(Size_3));
        #else
        PUInt("   |      SYS              DONE      = %4u.%03u, cost %u.%03u, size %u KB", ms(pBLD[A53_BLD_LOAD_SYS_DONE]), us(pBLD[A53_BLD_LOAD_SYS_DONE]),     ms(Diff_6),  us(Diff_6),  kb(Size_3));
        #endif
        #if defined(CONFIG_FASTBOOT_LZ4)
        PUInt("   |          (decompress cost %u.%03u)", ms(Diff_13), us(Diff_13), 0U, 0U, 0U);
        #endif
        #ifdef CONFIG_BLD_SEQ_ATF
        PUInt("   | FW ENTRY, INIT ATF    DONE      = %4u.%03u",               ms(pBLD[A53_BLD_ENTER_ATF]),              us(pBLD[A53_BLD_ENTER_ATF]),            0U,          0U,          0U);
     // PUInt("   | PRE  OS CONFIG                  = %4u.%03u",               ms(pBLD[A53_BLD_PREOS_CONFIG]),           us(pBLD[A53_BLD_PREOS_CONFIG]),         0U,          0U,          0U);
        PUInt("   | ATF EXEC& KERNEL INIT DONE      = %4u.%03u, cost %u.%03u, APP bss size %u", ms(pAPP[SVC_TIME_POSTOS_CONFIG]), us(pAPP[SVC_TIME_POSTOS_CONFIG]), ms(Diff_7),  us(Diff_7),  Size_AppBss);
        #else
        PUInt("   | FW ENTRY              DONE      = %4u.%03u",               ms(pBLD[A53_BLD_ENTER_ATF]),              us(pBLD[A53_BLD_ENTER_ATF]),            0U,          0U,          0U);
     // PUInt("   | PRE  OS CONFIG                  = %4u.%03u",               ms(pBLD[A53_BLD_PREOS_CONFIG]),           us(pBLD[A53_BLD_PREOS_CONFIG]),         0U,          0U,          0U);
        PUInt("   | KERNEL INIT           DONE      = %4u.%03u, cost %u.%03u, APP bss size %u", ms(pAPP[SVC_TIME_POSTOS_CONFIG]), us(pAPP[SVC_TIME_POSTOS_CONFIG]), ms(Diff_7), us(Diff_7), Size_AppBss);
        #endif
        PStr( "-- STAGE 3 -----------------------------------------",          NULL, NULL, NULL, NULL, NULL);
        PUInt("   | BSS SIZE              %u",                                 Size_AppBss, 0U, 0U, 0U, 0U);
        PUInt("   | APP ENTRY             START     = %4u.%03u",               ms(pAPP[SVC_TIME_POSTOS_CONFIG]),         us(pAPP[SVC_TIME_POSTOS_CONFIG]),       0U,          0U,          0U);
        PUInt("   | CALIB CONFIG          START     = %4u.%03u",               ms(pAPP[SVC_TIME_CALIB_LOAD_START]),      us(pAPP[SVC_TIME_CALIB_LOAD_START]),    0U,          0U,          0U);
        PUInt("   |                       DONE      = %4u.%03u",               ms(pAPP[SVC_TIME_CALIB_LOAD_DONE]),       us(pAPP[SVC_TIME_CALIB_LOAD_DONE]),     0U,          0U,          0U);
        PUInt("   | VIN CONFIG            START     = %4u.%03u",               ms(pAPP[SVC_TIME_VIN_CONFIG_START]),      us(pAPP[SVC_TIME_VIN_CONFIG_START]),    0U,          0U,          0U);
        PUInt("   |                       DONE      = %4u.%03u, cost %u.%03u", ms(pAPP[SVC_TIME_VIN_CONFIG_DONE]),       us(pAPP[SVC_TIME_VIN_CONFIG_DONE]),     ms(Diff_9),  us(Diff_9),  0U);
        PUInt("   | VOUT CONFIG           START     = %4u.%03u",               ms(pAPP[SVC_TIME_VOUT_CONFIG_START]),     us(pAPP[SVC_TIME_VOUT_CONFIG_START]),   0U,          0U,          0U);
        PUInt("   |                       DONE      = %4u.%03u, cost %u.%03u", ms(pAPP[SVC_TIME_VOUT_CONFIG_DONE]),      us(pAPP[SVC_TIME_VOUT_CONFIG_DONE]),    ms(Diff_10), us(Diff_10), 0U);
        PUInt("   | LOAD UCODE            START     = %4u.%03u",               ms(pAPP[SVC_TIME_UCODE_LOAD_START]),      us(pAPP[SVC_TIME_UCODE_LOAD_START]),    0U,          0U,          0U);
        PUInt("   |                       DONE PART = %4u.%03u, cost %u.%03u, size %u KB", ms(pAPP[SVC_TIME_UCODE_LOAD_DONE_PART]), us(pAPP[SVC_TIME_UCODE_LOAD_DONE_PART]), ms(Diff_8), us(Diff_8), kb(Size_Ucode));
        PUInt("   |                       DONE      = %4u.%03u",               ms(pAPP[SVC_TIME_UCODE_LOAD_DONE]),       us(pAPP[SVC_TIME_UCODE_LOAD_DONE]),     0U,          0U,          0U);
        #if defined(CONFIG_ICAM_BIST_UCODE)
        PUInt("   | LOAD BIST             START     = %4u.%03u",               ms(pAPP[SVC_TIME_BIST_LOAD_START]),       us(pAPP[SVC_TIME_BIST_LOAD_START]),     0U,          0U,          0U);
        PUInt("   |                       DONE      = %4u.%03u, cost %u.%03u, size %u KB", ms(pAPP[SVC_TIME_BIST_LOAD_DONE]), us(pAPP[SVC_TIME_BIST_LOAD_DONE]), ms(Diff_12), us(Diff_12), kb(Size_Bist));
        #else
        AmbaMisra_TouchUnused(&Size_Bist);
        #endif
        PUInt("   | LOAD IQ TABLE         START     = %4u.%03u",               ms(pAPP[SVC_TIME_IQ_LOAD_START]),         us(pAPP[SVC_TIME_IQ_LOAD_START]),       0U,          0U,          0U);
        PUInt("   |                       DONE PART = %4u.%03u",               ms(pAPP[SVC_TIME_IQ_LOAD_DONE_PART]),     us(pAPP[SVC_TIME_IQ_LOAD_DONE_PART]),   0U,          0U,          0U);
        PUInt("   |                       DONE      = %4u.%03u",               ms(pAPP[SVC_TIME_IQ_LOAD_DONE]),          us(pAPP[SVC_TIME_IQ_LOAD_DONE]),        0U,          0U,          0U);
        PUInt("   |                       CHECK     = %4u.%03u",               ms(pAPP[SVC_TIME_IQ_LOAD_CHECK]),         us(pAPP[SVC_TIME_IQ_LOAD_CHECK]),       0U,          0U,          0U);
        PStr( "-- STAGE 4 -----------------------------------------",          NULL, NULL, NULL, NULL, NULL);
        PUInt("   | DSP BOOT              START     = %4u.%03u",               ms(pAPP[SVC_TIME_DSP_BOOT_START]),        us(pAPP[SVC_TIME_DSP_BOOT_START]),      0U,          0U,          0U);
        PUInt("   |                       DONE      = %4u.%03u, cost %u.%03u", ms(pAPP[SVC_TIME_DSP_BOOT_DONE]),         us(pAPP[SVC_TIME_DSP_BOOT_DONE]),       ms(Diff_11), us(Diff_11), 0U);
        /* record immediately after booting */
        if (0U != isBootToRec()) {
        PUInt("   | REC                   START     = %4u.%03u",               ms(pAPP[SVC_TIME_REC_START]),             us(pAPP[SVC_TIME_REC_START]),           0U,          0U,          0U);
        }
        PStr( "----------------------------------------------------",          NULL, NULL, NULL, NULL, NULL);

        /* time diag */
        {
            UINT32 t;
            #if defined(CONFIG_SOC_CV2FS)
            if (ms(Diff_1) > SVC_TIME_STD_BLD_WAIT_KEY) {
                PUInt(" [Note] R52 BLD wait key takes much time  : %u.%03u",      ms(Diff_1), us(Diff_1),           0U, 0U, 0U);
                PUInt("        ideal duration                    : %u.%03u ms\n", SVC_TIME_STD_BLD_WAIT_KEY,    0U, 0U, 0U, 0U);
            }
            if (ms(Diff_2) > SVC_TIME_STD_BLD2_LOAD) {
                PUInt(" [Warn] R52 BLD load BLD2 is too slow     : %u.%03u",      ms(Diff_2), us(Diff_2),           0U, 0U, 0U);
                PUInt("        ideal duration                    : %u.%03u ms\n", SVC_TIME_STD_BLD2_LOAD,       0U, 0U, 0U, 0U);
            }
            if (ms(Diff_3) > SVC_TIME_STD_R52SYS_LOAD) {
                PUInt(" [Warn] R52 BLD load R52SYS is too slow   : %u.%03u (%u Byte/ms)",ms(Diff_3), us(Diff_3), Size_1/ms(Diff_3), 0U, 0U);
                PUInt("        ideal duration                    : %u.%03u ms",   SVC_TIME_STD_R52SYS_LOAD,     0U, 0U, 0U, 0U);
                PUInt("        file size                         : %u Byte\n",    Size_1,                       0U, 0U, 0U, 0U);
            }
            #endif
            if (ms(Diff_4) > SVC_TIME_STD_BLD_WAIT_KEY) {
                PUInt(" [Note] BLD wait key takes much time      : %u.%03u",      ms(Diff_4), us(Diff_4),           0U, 0U, 0U);
                PUInt("        ideal duration                    : %u.%03u ms\n", SVC_TIME_STD_BLD_WAIT_KEY,    0U, 0U, 0U, 0U);
            }
            #if defined(CONFIG_BLD_SEQ_ATF)
            if (ms(Diff_5) > SVC_TIME_STD_ATF_LOAD) {
                PUInt(" [Warn] BLD load ATF is too slow          : %u.%03u (%u Byte/ms)", ms(Diff_5), us(Diff_5), Size_2/ms(Diff_5), 0U, 0U);
                PUInt("        ideal duration                    : %u.%03u ms",   SVC_TIME_STD_ATF_LOAD,        0U, 0U, 0U, 0U);
                PUInt("        file size                         : %u Byte\n",    Size_2,                       0U, 0U, 0U, 0U);
            }
            #endif
            if (ms(Diff_6) > SVC_TIME_STD_SYSSW_LOAD) {
                PUInt(" [Warn] BLD load SYS_SW (APP) is too slow : %u.%03u (%u Byte/ms)", ms(Diff_6), us(Diff_6), Size_3/ms(Diff_6), 0U, 0U);
                PUInt("        ideal duration                    : %u.%03u ms",   SVC_TIME_STD_SYSSW_LOAD,      0U, 0U, 0U, 0U);
                PUInt("        file size                         : %u Byte\n",    Size_3,                       0U, 0U, 0U, 0U);
            }
            if (ms(Diff_7) > SVC_TIME_STD_BSS_INIT) {
                PUInt(" [Note] init APP BSS is too slow          : %u.%03u",      ms(Diff_7), us(Diff_7),           0U, 0U, 0U);
                PUInt("        ideal duration                    : %u.%03u ms",   SVC_TIME_STD_BSS_INIT,        0U, 0U, 0U, 0U);
                PUInt("        bss size                          : %u\n",         Size_AppBss,                  0U, 0U, 0U, 0U);
            }
            if (Size_AppBss > SVC_TIME_STD_BSS_SIZE) {
                PUInt(" [Note] APP BSS size is too fat           : %u",           Size_AppBss,                  0U, 0U, 0U, 0U);
                PUInt("        ideal BSS size should be within   : %u\n",         SVC_TIME_STD_BSS_SIZE,        0U, 0U, 0U, 0U);
            }
            if (ms(Diff_8) > SVC_TIME_STD_UCODE_PART_LOAD) {
                PUInt(" [Warn] APP load part ucode is too slow   : %u.%03u (%u Byte/ms)", ms(Diff_8), us(Diff_8), Size_Ucode/ms(Diff_8), 0U, 0U);
                PUInt("        ideal duration                    : %u.%03u ms",   SVC_TIME_STD_UCODE_PART_LOAD, 0U, 0U, 0U, 0U);
                PUInt("        file size                         : %u Byte\n",    Size_Ucode,                   0U, 0U, 0U, 0U);
            }
            if (ms(Diff_9) > SVC_TIME_STD_VIN_CFG_DEF) {
                PUInt(" [Warn] APP config Vin is too slow        : %u.%03u",      ms(Diff_9), us(Diff_9),           0U, 0U, 0U);
                PUInt("        ideal duration                    : %u.%03u ms\n", SVC_TIME_STD_VIN_CFG_DEF,     0U, 0U, 0U, 0U);
            }
            if (0U != Flg_HasIq) {
                if (pAPP[SVC_TIME_IQ_LOAD_DONE_PART] < pAPP[SVC_TIME_VIN_CONFIG_DONE]) {
                    UINT32 WaitTime = ms(pAPP[SVC_TIME_VIN_CONFIG_DONE] - pAPP[SVC_TIME_IQ_LOAD_DONE_PART]);
                    if (WaitTime > 30U) {
                        PUInt(" [Warn] DSP boot is delayed sine VIN config too late\n",                     0U, 0U, 0U, 0U, 0U);
                    }
                }
            } else {
                if (pAPP[SVC_TIME_UCODE_LOAD_DONE_PART] < pAPP[SVC_TIME_VIN_CONFIG_DONE]) {
                    UINT32 WaitTime = ms(pAPP[SVC_TIME_VIN_CONFIG_DONE] - pAPP[SVC_TIME_UCODE_LOAD_DONE_PART]);
                    if (WaitTime > 30U) {
                        PUInt(" [Warn] DSP boot is delayed sine VIN config too late\n",                     0U, 0U, 0U, 0U, 0U);
                    }
                }
            }
            if (ms(Diff_10) > SVC_TIME_STD_VOUT_CFG_DEF) {
                PUInt(" [Warn] APP config Vout is too slow       : %u.%03u",      ms(Diff_10), us(Diff_10),         0U, 0U, 0U);
                PUInt("        ideal duration                    : %u.%03u ms\n", SVC_TIME_STD_VOUT_CFG_DEF,    0U, 0U, 0U, 0U);
            }
            if (0U != Flg_HasIq) {
                if (pAPP[SVC_TIME_IQ_LOAD_DONE_PART] < pAPP[SVC_TIME_VOUT_CONFIG_DONE]) {
                    UINT32 WaitTime = ms(pAPP[SVC_TIME_VOUT_CONFIG_DONE] - pAPP[SVC_TIME_IQ_LOAD_DONE_PART]);
                    if (WaitTime > 50U) {
                        PUInt(" [Warn] DSP boot is delayed sine VOUT config too late\n",                    0U, 0U, 0U, 0U, 0U);
                    }
                }
            } else {
                if (pAPP[SVC_TIME_UCODE_LOAD_DONE_PART] < pAPP[SVC_TIME_VOUT_CONFIG_DONE]) {
                    UINT32 WaitTime = ms(pAPP[SVC_TIME_VOUT_CONFIG_DONE] - pAPP[SVC_TIME_UCODE_LOAD_DONE_PART]);
                    if (WaitTime > 50U) {
                        PUInt(" [Warn] DSP boot is delayed sine VOUT config too late\n",                    0U, 0U, 0U, 0U, 0U);
                    }
                }
            }
            if (ms(Diff_11) > SVC_TIME_STD_DSP_BOOT) {
                PUInt(" [Warn] DSP boot process is too slow      : %u.%03u",      ms(Diff_11), us(Diff_11),         0U, 0U, 0U);
                PUInt("        ideal duration                    : %u.%03u ms\n", SVC_TIME_STD_DSP_BOOT,        0U, 0U, 0U, 0U);
            }

            #if defined(CONFIG_SOC_CV2FS)
            Diff_4 += Diff_1;
            #endif
            t = pAPP[SVC_TIME_DSP_BOOT_DONE] - Diff_4;
            PUInt("### After removing BLD key detect time, DSP boot done time can be around %u.%03u ms ", ms(t), us(t), 0U, 0U, 0U);
            PStr( "----------------------------------------------------", NULL, NULL, NULL, NULL, NULL);
        }
    }
}

#if defined(CONFIG_AMBALINK_BOOT_OS)
void SvcTime_PrintBootTimeLinux(void)
{
    UINT32 Rval;
    AMBA_TIMER_INFO_s *pTime;

    Rval = GetBldBootInfoPtr(&pTime);
    if ((SVC_OK == Rval) && (NULL != pTime)) {
        const UINT32* pAPP = pTime->A53_APP_Time;
        UINT32 Diff_1 = pAPP[SVC_TIME_LINUX_BOOT_DONE] - pAPP[SVC_TIME_LINUX_BOOT_START];

        PStr( "----------------------------------------------------",          NULL, NULL, NULL, NULL, NULL);
        PUInt("   | AMBALINK            START     = %4u.%03u, data size %d KB",ms(pAPP[SVC_TIME_LINUX_BOOT_START]),          us(pAPP[SVC_TIME_LINUX_BOOT_START]),          kb(Size_AmbaLinkData), 0U, 0U);
        PUInt("   | IPC INIT            DONE      = %4u.%03u",                 ms(pAPP[SVC_TIME_LINUX_IPC_INIT_DONE]),       us(pAPP[SVC_TIME_LINUX_IPC_INIT_DONE]),       0U,                    0U, 0U);
        PUInt("   | OS INIT             DONE      = %4u.%03u",                 ms(pAPP[SVC_TIME_LINUX_OS_DONE]),             us(pAPP[SVC_TIME_LINUX_OS_DONE]),             0U,                    0U, 0U);
        PUInt("   | RPMSG INIT          DONE      = %4u.%03u",                 ms(pAPP[SVC_TIME_LINUX_RPMSG_INIT_DONE]),     us(pAPP[SVC_TIME_LINUX_RPMSG_INIT_DONE]),     0U,                    0U, 0U);
        PUInt("   | RPMSG INIT APP      DONE      = %4u.%03u",                 ms(pAPP[SVC_TIME_LINUX_RPMSG_INIT_APP_DONE]), us(pAPP[SVC_TIME_LINUX_RPMSG_INIT_APP_DONE]), 0U,                    0U, 0U);
        PUInt("   | AMBALINK BOOT       DONE      = %4u.%03u, cost %u.%03u",   ms(pAPP[SVC_TIME_LINUX_BOOT_DONE]),           us(pAPP[SVC_TIME_LINUX_BOOT_DONE]),           ms(Diff_1),    us(Diff_1), 0U);
        PStr( "******************************************************",        NULL, NULL, NULL, NULL, NULL);
    }
}
#endif

void SvcTime_PrintBootTimeCv(void)
{
    static UINT8 Done = 0U;

    if (0U == Done) {
        UINT32 Rval;
        AMBA_TIMER_INFO_s *pTime;

        Rval = GetBldBootInfoPtr(&pTime);
        if ((SVC_OK == Rval) && (NULL != pTime)) {
            const UINT32* pAPP = pTime->A53_APP_Time;
            UINT32 Diff_1      = pAPP[SVC_TIME_CV_INIT_DONE]          - pAPP[SVC_TIME_CV_INIT_START];
            UINT32 Diff_2      = pAPP[SVC_TIME_CV_VISORC_LOAD_DONE]   - pAPP[SVC_TIME_CV_VISORC_LOAD_START];
            UINT32 Diff_3      = pAPP[SVC_TIME_CV_FLEXIDAG_LOAD_DONE] - pAPP[SVC_TIME_CV_FLEXIDAG_LOAD_START];
            UINT32 Diff_4      = pAPP[SVC_TIME_CV_VISORC_BOOT_DONE]   - pAPP[SVC_TIME_CV_VISORC_BOOT_START];
            UINT32 Size_CvFlexiDag = Size_CvTotal - Size_CvVisOrc;

            PStr( "-- STAGE 5 -----------------------------------------",                    NULL, NULL, NULL, NULL, NULL);
            PUInt("   | CV INIT        DONE           = %4u.%03u, cost %u.%03u",             ms(pAPP[SVC_TIME_CV_INIT_DONE]),           us(pAPP[SVC_TIME_CV_INIT_DONE]),           ms(Diff_1), us(Diff_1), 0U);
            PUInt("   | VISORC LOAD    START          = %4u.%03u",                           ms(pAPP[SVC_TIME_CV_VISORC_LOAD_START]),   us(pAPP[SVC_TIME_CV_VISORC_LOAD_START]),   0U,         0U,         0U);
            PUInt("   |                DONE           = %4u.%03u, cost %u.%03u, size %u KB", ms(pAPP[SVC_TIME_CV_VISORC_LOAD_DONE]),    us(pAPP[SVC_TIME_CV_VISORC_LOAD_DONE]),    ms(Diff_2), us(Diff_2), kb(Size_CvVisOrc));
            PUInt("   | FLEXIDAG LOAD  START          = %4u.%03u",                           ms(pAPP[SVC_TIME_CV_FLEXIDAG_LOAD_START]), us(pAPP[SVC_TIME_CV_FLEXIDAG_LOAD_START]), 0U,         0U,         0U);
            PUInt("   |                DONE           = %4u.%03u, cost %u.%03u, size %u KB", ms(pAPP[SVC_TIME_CV_FLEXIDAG_LOAD_DONE]),  us(pAPP[SVC_TIME_CV_FLEXIDAG_LOAD_DONE]),  ms(Diff_3), us(Diff_3), kb(Size_CvFlexiDag));
            PUInt("   | VISORC BOOT    START          = %4u.%03u",                           ms(pAPP[SVC_TIME_CV_VISORC_BOOT_START]),   us(pAPP[SVC_TIME_CV_VISORC_BOOT_START]),   0U,         0U,         0U);
            PUInt("   |                DONE           = %4u.%03u, cost %u.%03u",             ms(pAPP[SVC_TIME_CV_VISORC_BOOT_DONE]),    us(pAPP[SVC_TIME_CV_VISORC_BOOT_DONE]),    ms(Diff_4), us(Diff_4), 0U);
            PUInt("   | FLEXIDAG       START          = %4u.%03u",                           ms(pAPP[SVC_TIME_CV_FLEXIDAG_BOOT_START]), us(pAPP[SVC_TIME_CV_FLEXIDAG_BOOT_START]), 0U,         0U,         0U);
            PUInt("   |                1ST RESULT     = %4u.%03u",                           ms(pAPP[SVC_TIME_CV_FLEXIDAG_1ST_RESULT]), us(pAPP[SVC_TIME_CV_FLEXIDAG_1ST_RESULT]), 0U,         0U,         0U);
            PStr( "******************************************************",                  NULL, NULL, NULL, NULL, NULL);

            Done = 1U; /* only print once */
        }
    }
}
