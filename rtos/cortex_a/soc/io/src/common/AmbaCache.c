/**
 *  @file AmbaCache.c
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
 *  @details CPU Cache Function
 *
 */

#include "AmbaTypes.h"
#include "AmbaCache.h"
#include "AmbaRTSL_Cache.h"

/**
 *  AmbaCache_DataClean - Write back the contents of data cache to main memory by specifying address range
 *  @param[in] VirtAddr The first virtual address to be performed data cache operations
 *  @param[in] Size Number of virtual addresses to be performed data cache operations
 *  @return error code
 */
UINT32 AmbaCache_DataClean(ULONG VirtAddr, ULONG Size)
{
    return AmbaRTSL_CacheCleanData(VirtAddr, Size);
}

/**
 *  AmbaCache_DataInvalidate - Invalidate data cache by specifying address range
 *  @param[in] VirtAddr The first virtual address to be performed data cache operations
 *  @param[in] Size Number of virtual addresses to be performed data cache operations
 *  @return error code
 */
UINT32 AmbaCache_DataInvalidate(ULONG VirtAddr, ULONG Size)
{
    return AmbaRTSL_CacheInvalData(VirtAddr, Size);
}

/**
 *  AmbaCache_DataFlush - Clean and invalidate data cache by specifying address range
 *  @param[in] VirtAddr The first virtual address to be performed data cache operations
 *  @param[in] Size Number of virtual addresses to be performed data cache operations
 *  @return error code
 */
UINT32 AmbaCache_DataFlush(ULONG VirtAddr, ULONG Size)
{
    return AmbaRTSL_CacheFlushData(VirtAddr, Size);
}
