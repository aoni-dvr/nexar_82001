/**
 *  @file AppNVME_Utility.c
 *
 *  Copyright (c) 2021 Ambarella International LP
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
 *  @details NVMe utility functions.
 */

#include <AmbaTypes.h>
#define AMBA_KAL_SOURCE_CODE
#include <AmbaKAL.h>
#include <AmbaWrap.h>

#include "AppNVME_Utility.h"

#ifdef NVME_PRINT_SUPPORT
#include "../../../../svc/comsvc/print/AmbaPrint.h"
#endif



/** Wrapper function for C memory copy. */
void NVME_UtilityMemoryCopy(void *pDst, const void *pSrc, SIZE_t num)
{
    if (AmbaWrap_memcpy(pDst, pSrc, num) != 0U) {
        // action TDB
    }
}

/** Wrapper function for C memory set. */
void NVME_UtilityMemorySet(void *ptr, INT32 v, SIZE_t n)
{
    if (AmbaWrap_memset(ptr, v, n) != 0U) {
        // action TDB
    }
}

UINT8* NVME_UtilityConstP8ToP8(const UINT8* Ptr)
{
    const UINT8 *src_ptr = Ptr;
    UINT8 *ptr_ret;

    NVME_UtilityMemoryCopy(&ptr_ret, &src_ptr, sizeof(UINT8*));

    return ptr_ret;
}

AMBA_FS_FAT_MBR* NVME_UtilityP8ToFsMbr(const UINT8* Ptr)
{
    const UINT8 *src_ptr = Ptr;
    AMBA_FS_FAT_MBR *ptr_ret;

    NVME_UtilityMemoryCopy(&ptr_ret, &src_ptr, sizeof(UINT8*));

    return ptr_ret;
}

AMBA_FS_FAT_BS32* NVME_UtilityP8ToFsBs32(const UINT8* Ptr)
{
    const UINT8 *src_ptr = Ptr;
    AMBA_FS_FAT_BS32 *ptr_ret;

    NVME_UtilityMemoryCopy(&ptr_ret, &src_ptr, sizeof(UINT8*));

    return ptr_ret;
}

AMBA_FS_FAT_BS16* NVME_UtilityP8ToFsBs16(const UINT8* Ptr)
{
    const UINT8 *src_ptr = Ptr;
    AMBA_FS_FAT_BS16 *ptr_ret;

    NVME_UtilityMemoryCopy(&ptr_ret, &src_ptr, sizeof(UINT8*));

    return ptr_ret;
}

AMBA_FS_EXFAT_BS* NVME_UtilityP8ToFsFxfatBs(const UINT8* Ptr)
{
    const UINT8 *src_ptr = Ptr;
    AMBA_FS_EXFAT_BS *ptr_ret;

    NVME_UtilityMemoryCopy(&ptr_ret, &src_ptr, sizeof(UINT8*));

    return ptr_ret;
}

PF_DRV_TBL* NVME_UtilityVoidP2PrFDrvTbl(const void* VoidPtr)
{
    const void *src_ptr = VoidPtr;
    PF_DRV_TBL *ptr_ret;

    NVME_UtilityMemoryCopy(&ptr_ret, &src_ptr, sizeof(void*));

    return ptr_ret;
}

#ifdef NVME_PRINT_SUPPORT
void NVME_UtilityPrint(const char *Str)
{
    //AmbaPrint_ModulePrintStr5(USB_PRINT_MODULE_ID, Str, NULL, NULL, NULL, NULL, NULL);
    AmbaPrint_PrintStr5(Str, NULL, NULL, NULL, NULL, NULL);
}

void NVME_UtilityPrintUInt5(const char *FmtStr, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5)
{
    //AmbaPrint_ModulePrintUInt5(USB_PRINT_MODULE_ID, FmtStr, Arg1, Arg2, Arg3, Arg4, Arg5);
    AmbaPrint_PrintUInt5(FmtStr, Arg1, Arg2, Arg3, Arg4, Arg5);
}

void NVME_UtilityPrintInt5(const char *FmtStr, INT32 Arg1, INT32 Arg2, INT32 Arg3, INT32 Arg4, INT32 Arg5)
{
    //AmbaPrint_ModulePrintInt5(USB_PRINT_MODULE_ID, FmtStr, Arg1, Arg2, Arg3, Arg4, Arg5);
    AmbaPrint_PrintInt5(FmtStr, Arg1, Arg2, Arg3, Arg4, Arg5);
}

void  NVME_UtilityPrintStr5(const char *FmtStr, const char *Arg1, const char *Arg2, const char *Arg3, const char *Arg4, const char *Arg5)
{
    //AmbaPrint_ModulePrintStr5(USB_PRINT_MODULE_ID, FmtStr, Arg1, Arg2, Arg3, Arg4, Arg5);
    AmbaPrint_PrintStr5(FmtStr, Arg1, Arg2, Arg3, Arg4, Arg5);
}
#endif
