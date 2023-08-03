/**
*  @file SvcPref.c
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

#include "AmbaKAL.h"
#include "AmbaMisraFix.h"
#include "AmbaNVM_Partition.h"
#include "AmbaNAND_Ctrl.h"
#include "AmbaNAND_FTL.h"
#include "AmbaUtility.h"
#include "AmbaWrap.h"
#include "SvcLog.h"
#include "SvcErrCode.h"
#include "SvcWrap.h"
#include "SvcPref.h"
#include "SvcNvm.h"


#define PREF_START_SECTOR       0
#define SVC_LOG_PREF            "PREF"
#define PREF_BUF_SIZE   (4096U)  /* Need to be multiple of 512, because one sector size is 512 byte*/
static UINT8  SvcPrefBuf[PREF_BUF_SIZE] GNU_SECTION_NOZEROINIT;

/**
* initialization of user preference
* @return 0-OK, 1-NG
*/
UINT32 SvcPref_Init(void)
{
    UINT32 Rval = SVC_OK;

#if defined(CONFIG_ENABLE_NAND_BOOT) || defined(CONFIG_ENABLE_SPINAND_BOOT)
        if (NAND_ERR_NONE != AmbaNAND_FtlInit(AMBA_USER_PARTITION_USER_SETTING)) {
            SvcLog_NG(SVC_LOG_PREF, "AmbaNAND_FtlInit failed", 0U, 0U);
            Rval = SVC_NG;
        }

        if (NAND_ERR_NONE != AmbaNAND_FtlInit(AMBA_USER_PARTITION_FAT_DRIVE_A)) {
            SvcLog_DBG(SVC_LOG_PREF, "Drive A isn't enabled", 0U, 0U);
        }

    #if defined(CONFIG_ICAM_PROJECT_SHMOO)
        if (NAND_ERR_NONE != AmbaNAND_FtlInit(AMBA_USER_PARTITION_RESERVED0)) {
            SvcLog_NG(SVC_LOG_PREF, "Reserve 0 init failed", 0U, 0U);
        }
    #endif
#endif

    return Rval;
}

/**
* store of user preference
* @param [in] PrefBufAddr base of preference buffer
* @param [in] PrefBufSize size of preference buffer
* @return 0-OK, 1-NG
*/
UINT32 SvcPref_Save(ULONG PrefBufAddr, UINT32 PrefBufSize)
{
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
    (void)PrefBufAddr;
    (void)PrefBufSize;
    return SVC_OK;
#else
    UINT32                    Rval, NumSector;
    AMBA_NVM_SECTOR_CONFIG_s  SecConfig;
    AMBA_NVM_FTL_INFO_s       FtlInfo;
    UINT32                    SvcPrefBufSize = (UINT32)sizeof(SvcPrefBuf);

    Rval = SvcNvm_GetFtlInfo(AMBA_USER_PARTITION_USER_SETTING, &FtlInfo);
    if (SVC_OK == Rval) {
        if (FtlInfo.TotalSizeInBytes < (UINT64)PrefBufSize) {
            SvcLog_NG(SVC_LOG_PREF, "The writing size exceed the partition size", 0U, 0U);
            Rval = SVC_NG;
        }
        if (FtlInfo.Initialized != 1U) {
            SvcLog_NG(SVC_LOG_PREF, "Need to init the partition first", 0U, 0U);
            Rval = SVC_NG;
        }

        if (PrefBufSize > (SvcPrefBufSize - 4U)) {
            SvcLog_NG(SVC_LOG_PREF, "The writing size exceed the buffer size", 0U, 0U);
            Rval = SVC_NG;
        } else {
            // If user write pref address does not same with current pref buffer, copy the data to current pref buffer first
            UINT8 *pPrefBuf;

            AmbaMisra_TypeCast(&(pPrefBuf), &(PrefBufAddr));

            /* CERT STR30-C */
            AmbaMisra_TouchUnused(pPrefBuf);

            if (SvcPrefBuf != pPrefBuf) {
                AmbaSvcWrap_MisraMemset(SvcPrefBuf, 0, SvcPrefBufSize);
                AmbaSvcWrap_MisraMemcpy(SvcPrefBuf, pPrefBuf, PrefBufSize);
            }
        }
    } else {
        SvcLog_NG(SVC_LOG_PREF, "SvcNvm_GetFtlInfo failed", 0U, 0U);
        Rval = SVC_NG;
    }

    if (SVC_OK == Rval) {
        const UINT8  *pTmpU8 = &(SvcPrefBuf[SvcPrefBufSize - 4U]);
        UINT32       *pCRC;

        // Calculate CRC
        AmbaMisra_TypeCast(&(pCRC), &(pTmpU8));
        *pCRC = AmbaUtility_Crc32(SvcPrefBuf, (SvcPrefBufSize - 4U));

        AmbaMisra_TouchUnused(&SvcPrefBufSize);

        // Write the pref buffer to NVM
        NumSector = SvcPrefBufSize / 512U;
        if (0U != (SvcPrefBufSize % 512U)) {
            NumSector++;
        }

        SecConfig.NumSector   = NumSector;
        SecConfig.StartSector = PREF_START_SECTOR;
        SecConfig.pDataBuf    = SvcPrefBuf;

        if (NVM_ERR_NONE != SvcNvm_WriteSector(AMBA_USER_PARTITION_USER_SETTING, &SecConfig, 2000U)) {
            SvcLog_NG(SVC_LOG_PREF, "AmbaNVM_WriteSector failed", 0U, 0U);
            Rval = SVC_NG;
        }

    }
    return Rval;
#endif
}

/**
* load of user preference
* @param [in] PrefBufAddr base of preference buffer
* @param [in] PrefBufSize size of preference buffer
* @return 0-OK, 1-NG
*/
UINT32 SvcPref_Load(ULONG PrefBufAddr, UINT32 PrefBufSize)
{
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
    (void)PrefBufAddr;
    (void)PrefBufSize;
    return SVC_OK;
#else
    UINT32                     Rval, NumSector;
    AMBA_NVM_SECTOR_CONFIG_s   SecConfig;
    AMBA_NVM_FTL_INFO_s        FtlInfo;
    UINT32                     SvcPrefBufSize = (UINT32)sizeof(SvcPrefBuf);

    Rval = SvcNvm_GetFtlInfo(AMBA_USER_PARTITION_USER_SETTING, &FtlInfo);
    if (SVC_OK == Rval) {
        if (FtlInfo.TotalSizeInBytes < (UINT64)PrefBufSize) {
            SvcLog_NG(SVC_LOG_PREF, "The reading size exceed the partition size", 0U, 0U);
            Rval = SVC_NG;
        }
        if (FtlInfo.Initialized != 1U) {
            SvcLog_NG(SVC_LOG_PREF, "Need to init the partition first", 0U, 0U);
            Rval = SVC_NG;
        }
    } else {
        SvcLog_NG(SVC_LOG_PREF, "AmbaNVM_GetFtlInfo failed", 0U, 0U);
        Rval = SVC_NG;
    }

    if (SVC_OK == Rval) {
        // Read the pref data from NVM
        AmbaSvcWrap_MisraMemset(SvcPrefBuf, 0, sizeof(SvcPrefBuf));
        AmbaMisra_TouchUnused(&SvcPrefBufSize);

        NumSector = SvcPrefBufSize / 512U;
        if (0U != (SvcPrefBufSize % 512U)) {
            NumSector++;
        }

        SecConfig.NumSector   = NumSector;
        SecConfig.StartSector = PREF_START_SECTOR;
        SecConfig.pDataBuf    = SvcPrefBuf;

        if (NVM_ERR_NONE != SvcNvm_ReadSector(AMBA_USER_PARTITION_USER_SETTING, &SecConfig, 2000U)) {
            SvcLog_NG(SVC_LOG_PREF, "AmbaNVM_ReadSector failed", 0U, 0U);
            Rval = SVC_NG;

        } else {
            const UINT8  *pTmpU8 = &(SvcPrefBuf[SvcPrefBufSize - 4U]);
            const UINT32 *pCRC;
            UINT32        CalcCRC;

            // Check the CRC
            AmbaMisra_TypeCast(&(pCRC), &(pTmpU8));
            if ((*pCRC == 0U) || (*pCRC == 0xFFFFFFFFU)) {
                AmbaSvcWrap_MisraMemset(SvcPrefBuf, 0, sizeof(SvcPrefBuf));
            } else {
                CalcCRC = AmbaUtility_Crc32(SvcPrefBuf, (SvcPrefBufSize - 4U));
                if (CalcCRC != *pCRC) {
                    SvcLog_NG(SVC_LOG_PREF, "Pref CRC checking fail! 0x%08x != 0x%08x", CalcCRC, *pCRC);
                    Rval = SVC_NG;
                }
            }

            // Assign pref buffer to user's buffer address
            if (Rval == SVC_OK) {
                UINT8 *pPrefBuf;

                AmbaMisra_TypeCast(&(pPrefBuf), &(PrefBufAddr));

                if (pPrefBuf != SvcPrefBuf) {
                    UINT32 CpySize = PrefBufSize;
                    if (CpySize > (SvcPrefBufSize - 4U)) {
                        CpySize = (SvcPrefBufSize - 4U);
                    }

                    AmbaSvcWrap_MisraMemcpy(pPrefBuf, SvcPrefBuf, CpySize);
                }
            }
        }
    }

    return Rval;
#endif
}

/**
* buffer get of user preference
* @param [out] pBufAddr base of preference buffer
* @param [out] pBufSize size of preference buffer
* @return none
*/
void SvcPref_GetPrefBuf(ULONG *pBufAddr, UINT32 *pBufSize)
{
    ULONG         Addr;
    const  UINT8  *pBuf = &SvcPrefBuf[0];

    AmbaMisra_TypeCast(&Addr, &pBuf);

    *pBufAddr = Addr;
    *pBufSize = (PREF_BUF_SIZE - 4U);   // reserve last 4-bytes for CRC
}

