/**
 *  @file AmbaRTSL_Cache.h
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
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
 *  @details Definitions & Constants for Cache Related APIs
 *
 */

#ifndef AMBA_RTSL_CACHE_H
#define AMBA_RTSL_CACHE_H

#ifndef AMBA_CACHE_DEF_H
#include "AmbaCache_Def.h"
#endif

/*
 * Defined in AmbaRTSL_Cache.c
 */
void AmbaRTSL_CacheInvalDataAll(void);
void AmbaRTSL_CacheCleanDataAll(void);
void AmbaRTSL_CacheFlushDataAll(void);

UINT32 AmbaRTSL_CacheInvalData(ULONG VirtAddr, ULONG Size);
UINT32 AmbaRTSL_CacheCleanData(ULONG VirtAddr, ULONG Size);
UINT32 AmbaRTSL_CacheFlushData(ULONG VirtAddr, ULONG Size);

UINT32 AmbaRTSL_CacheInvalDataPtr(const void *pVirtAddr, ULONG Size);
UINT32 AmbaRTSL_CacheCleanDataPtr(const void *pVirtAddr, ULONG Size);
UINT32 AmbaRTSL_CacheFlushDataPtr(const void *pVirtAddr, ULONG Size);

#endif  /* AMBA_RTSL_CACHE_H */
