#include "AmbaKAL.h"
#include "AmbaMisraFix.h"
#include "AmbaNVM_Partition.h"
#include "AmbaNAND_Ctrl.h"
#include "AmbaNAND_FTL.h"
#include "AmbaUtility.h"
#include "SvcLog.h"
#include "SvcErrCode.h"
#include "SvcWrap.h"
#include "SvcPrefCustom.h"
#include "SvcNvm.h"

#define PREF_START_SECTOR       0
#define SVC_LOG_PREF            "PREF"

#define PREF_CUSTOM_BUF_SIZE   (512U)  /* Need to be multiple of 512, because one sector size is 512 byte*/
static UINT8 SvcPrefCustomBuf[PREF_CUSTOM_BUF_SIZE] GNU_SECTION_NOZEROINIT;

/**
* store of user preference
* @param [in] PrefBufAddr base of preference buffer
* @param [in] PrefBufSize size of preference buffer
* @return 0-OK, 1-NG
*/
UINT32 SvcPrefCustom_Save(ULONG PrefBufAddr, UINT32 PrefBufSize)
{
    UINT32                    Rval = SVC_OK, NumSector, NvmID;
    AMBA_NVM_SECTOR_CONFIG_s  SecConfig;
    AMBA_NVM_FTL_INFO_s       FtlInfo;
    UINT32                    SvcPrefBufSize = sizeof(SvcPrefCustomBuf);

    Rval = AmbaSvcWrap_GetNVMID(&NvmID, 1U, AMBA_USER_PARTITION_USER_SETTING);
    if(Rval != 0U) {
        SvcLog_NG(SVC_LOG_PREF, "Fail to SvcNVM_GetNVMID(), NvmID", 0U, 0U);
    }
    if (NVM_ERR_NONE != AmbaNVM_GetFtlInfo(NvmID, AMBA_USER_PARTITION_USER_SETTING, &FtlInfo)) {
        SvcLog_NG(SVC_LOG_PREF, "AmbaNVM_GetFtlInfo failed NvmID %lu", NvmID, 0U);
        Rval = SVC_NG;
    }    
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
        const UINT8 *pPrefBuf;
        AmbaMisra_TypeCast(&(pPrefBuf), &(PrefBufAddr));
        if (SvcPrefCustomBuf != pPrefBuf) {
            (void) AmbaWrap_memset(SvcPrefCustomBuf, 0, SvcPrefBufSize);
            (void) AmbaWrap_memcpy(SvcPrefCustomBuf, pPrefBuf, PrefBufSize);
        }
    }

    if (SVC_OK == Rval) {
        const UINT8  *pTmpU8 = &(SvcPrefCustomBuf[SvcPrefBufSize - 4U]);
        UINT32       *pCRC;

        // Calculate CRC
        AmbaMisra_TypeCast(&(pCRC), &(pTmpU8));
        *pCRC = AmbaUtility_Crc32(SvcPrefCustomBuf, (SvcPrefBufSize - 4U));

        AmbaMisra_TouchUnused(&SvcPrefBufSize);

        // Write the pref buffer to NVM
        NumSector = SvcPrefBufSize / 512U;
        if (0U != (SvcPrefBufSize % 512U)) {
            NumSector++;
        }

        SecConfig.NumSector   = NumSector;
        SecConfig.StartSector = PREF_START_SECTOR;
        SecConfig.pDataBuf    = SvcPrefCustomBuf;

        if (NVM_ERR_NONE != AmbaNVM_WriteSector(NvmID, AMBA_USER_PARTITION_USER_SETTING, &SecConfig, 2000U)) {
            SvcLog_NG(SVC_LOG_PREF, "AmbaNVM_WriteSector failed", 0U, 0U);
            Rval = SVC_NG;
        }
    }
    return Rval;
}

/**
* load of user preference
* @param [in] PrefBufAddr base of preference buffer
* @param [in] PrefBufSize size of preference buffer
* @return 0-OK, 1-NG
*/
UINT32 SvcPrefCustom_Load(ULONG PrefBufAddr, UINT32 PrefBufSize)
{
    UINT32                     Rval = SVC_OK, NumSector, NvmID;
    AMBA_NVM_SECTOR_CONFIG_s   SecConfig;
    AMBA_NVM_FTL_INFO_s        FtlInfo;
    UINT32                     SvcPrefBufSize = sizeof(SvcPrefCustomBuf);

    Rval = AmbaSvcWrap_GetNVMID(&NvmID, 1U, AMBA_USER_PARTITION_USER_SETTING);
    if(Rval != 0U) {
        SvcLog_NG(SVC_LOG_PREF, "Fail to SvcNVM_GetNVMID()", 0U, 0U);
    }

    if (NVM_ERR_NONE != AmbaNVM_GetFtlInfo(NvmID, AMBA_USER_PARTITION_USER_SETTING, &FtlInfo)) {
        SvcLog_NG(SVC_LOG_PREF, "AmbaNVM_GetFtlInfo failed NvmID %lu", NvmID, 0U);
        Rval = SVC_NG;
    }

    if (FtlInfo.TotalSizeInBytes < (UINT64)PrefBufSize) {
        SvcLog_NG(SVC_LOG_PREF, "The reading size exceed the partition size", 0U, 0U);
        Rval = SVC_NG;
    }
    if (FtlInfo.Initialized != 1U) {
        SvcLog_NG(SVC_LOG_PREF, "Need to init the partition first", 0U, 0U);
        Rval = SVC_NG;
    }

    if (SVC_OK == Rval) {
        // Read the pref data from NVM
        (void) AmbaWrap_memset(SvcPrefCustomBuf, 0, sizeof(SvcPrefCustomBuf));
        AmbaMisra_TouchUnused(&SvcPrefBufSize);

        NumSector = SvcPrefBufSize / 512U;
        if (0U != (SvcPrefBufSize % 512U)) {
            NumSector++;
        }

        SecConfig.NumSector   = NumSector;
        SecConfig.StartSector = PREF_START_SECTOR;
        SecConfig.pDataBuf    = SvcPrefCustomBuf;

        if (NVM_ERR_NONE != AmbaNVM_ReadSector(NvmID, AMBA_USER_PARTITION_USER_SETTING, &SecConfig, 2000U)) {
            SvcLog_NG(SVC_LOG_PREF, "AmbaNVM_ReadSector failed", 0U, 0U);
            Rval = SVC_NG;
        } else {
            const UINT8  *pTmpU8 = &(SvcPrefCustomBuf[SvcPrefBufSize - 4U]);
            const UINT32 *pCRC;
            UINT32        CalcCRC;

            // Check the CRC
            AmbaMisra_TypeCast(&(pCRC), &(pTmpU8));
            if ((*pCRC == 0U) || (*pCRC == 0xFFFFFFFFU)) {
                (void) AmbaWrap_memset(SvcPrefCustomBuf, 0, sizeof(SvcPrefCustomBuf));
            } else {
                CalcCRC = AmbaUtility_Crc32(SvcPrefCustomBuf, (SvcPrefBufSize - 4U));
                if (CalcCRC != *pCRC) {
                    SvcLog_NG(SVC_LOG_PREF, "Pref CRC checking fail! 0x%08x != 0x%08x", CalcCRC, *pCRC);
                    Rval = SVC_NG;
                }
            }

            // Assign pref buffer to user's buffer address
            if (Rval == SVC_OK) {
                UINT8 *pPrefBuf;

                AmbaMisra_TypeCast(&(pPrefBuf), &(PrefBufAddr));
                if (pPrefBuf != SvcPrefCustomBuf) {
                    UINT32 CpySize = PrefBufSize;
                    if (CpySize > (SvcPrefBufSize - 4U)) {
                        CpySize = (SvcPrefBufSize - 4U);
                    }
                    (void) AmbaWrap_memcpy(pPrefBuf, SvcPrefCustomBuf, CpySize);
                }
            }
        }
    }

    return Rval;
}

/**
* buffer get of user preference
* @param [out] pBufAddr base of preference buffer
* @param [out] pBufSize size of preference buffer
* @return none
*/
void SvcPrefCustom_GetPrefBuf(ULONG *pBufAddr, UINT32 *pBufSize)
{
    ULONG         Addr;
    const  UINT8  *pBuf = &SvcPrefCustomBuf[0];

    AmbaMisra_TypeCast(&Addr, &pBuf);
    *pBufAddr = Addr;
    *pBufSize = (PREF_CUSTOM_BUF_SIZE - 4U);   // reserve last 4-bytes for CRC
}

