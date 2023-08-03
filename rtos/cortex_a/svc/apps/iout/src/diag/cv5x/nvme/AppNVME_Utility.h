/**
 *  @file AppNVME_Utility.h
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
 *  @details NVMe utility functions header file.
 */

#ifndef APPNVME_UTILITY_H
#define APPNVME_UTILITY_H

#ifndef AMBA_FS_H
#include <AmbaFS.h>
#endif
#ifndef AMBA_FS_FORMAT_H
#include <AmbaFS_Format.h>
#endif
#include <AmbaIOUtility.h>

// Enable it for printing debug message
#define  NVME_PRINT_SUPPORT

void   NVME_UtilityMemoryCopy(void *pDst, const void *pSrc, SIZE_t num);
void   NVME_UtilityMemorySet(void *ptr, INT32 v, SIZE_t n);
UINT8* NVME_UtilityConstP8ToP8(const UINT8* Ptr);
AMBA_FS_FAT_MBR* NVME_UtilityP8ToFsMbr(const UINT8* Ptr);
AMBA_FS_FAT_BS32* NVME_UtilityP8ToFsBs32(const UINT8* Ptr);
AMBA_FS_FAT_BS16* NVME_UtilityP8ToFsBs16(const UINT8* Ptr);
AMBA_FS_EXFAT_BS* NVME_UtilityP8ToFsFxfatBs(const UINT8* Ptr);
PF_DRV_TBL* NVME_UtilityVoidP2PrFDrvTbl(const void* VoidPtr);

#ifdef  NVME_PRINT_SUPPORT
void  NVME_UtilityPrint(const char *Str);
void  NVME_UtilityPrintUInt5(const char *FmtStr, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5);
void  NVME_UtilityPrintInt5(const char *FmtStr, INT32 Arg1, INT32 Arg2, INT32 Arg3, INT32 Arg4, INT32 Arg5);
void  NVME_UtilityPrintStr5(const char *FmtStr, const char *Arg1, const char *Arg2, const char *Arg3, const char *Arg4, const char *Arg5);
#endif

#endif

