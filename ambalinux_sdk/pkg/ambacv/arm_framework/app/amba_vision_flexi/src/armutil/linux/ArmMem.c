/**
*  @file ArmMem.c
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
*   @details The Arm memory management utility
*
*/
/* Standard header */
#include <unistd.h>
#include <fcntl.h>             /* for open() */
#include <sys/ioctl.h>         /* for ioctl() */

/* Arm header */
#include "ArmMem.h"
#include "ArmErrCode.h"
#include "ArmStdC.h"
#include "ArmLog.h"
/* CV common header */
#include "cavalry_ioctl.h"    /* for getting CMA memory usage info */

#define ARM_LOG_MEM          "ArmUtil_Mem"
//#define CV_ERR_0003         (0x01420000U | 3U) // Scheduler internal error
#define MAX_RETRY_NUM       (100)

/*---------------------------------------------------------------------------*\
 * static functions
\*---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: GetMemUsage
 *
 *  @Description:: Get memory usage information of CMA
 *
 *  @Input      ::
 *    pUsedSize:     Pointer to CMA info
 *    pFreeSize:     Pointer to CNA info
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    UINT32:      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
#ifdef ENABLE_AMBA_MAL

typedef struct {
    UINT32 Id;
    UINT64 UsedLen;
    UINT64 FreeLen;
} AMBA_MAL_IOCTL_USAGE_s;

#define AMBA_MAL_GET_USAGE         _IOWR ('A', 0x5U, AMBA_MAL_IOCTL_USAGE_s)

static UINT32 GetMemUsage(UINT32 *pUsedSize, UINT32 *pFreeSize)
{
    UINT32 Rval = ARM_OK;
    INT32  ambamal_fd = 0;
    AMBA_MAL_IOCTL_USAGE_s usage_mem;

    if ((pUsedSize == NULL) || (pFreeSize == NULL)) {
        ArmLog_ERR(ARM_LOG_MEM, "## GetMemUsage: null pointer", 0U, 0U);
        Rval = ARM_NG;
    }

    if(Rval == ARM_OK) {
        if ((ambamal_fd = open("/dev/ambamal", O_SYNC | O_RDWR, 0)) < 0) {
            ArmLog_ERR(ARM_LOG_MEM, "## GetMemUsage: /dev/ambamal open fail", 0U, 0U);
            Rval = ARM_NG;
        }
    }

    if(Rval == ARM_OK) {
        ArmStdC_memset(&usage_mem, 0, sizeof(usage_mem));
        if(ioctl(ambamal_fd, AMBA_MAL_GET_USAGE, &usage_mem) < 0) {
            ArmLog_ERR(ARM_LOG_MEM, "## GetMemUsage: AMBA_MAL_GET_USAGE fail", 0U, 0U);
            Rval = ARM_NG;
        } else {
            *pUsedSize = (UINT32)usage_mem.UsedLen;
            *pFreeSize = (UINT32)usage_mem.FreeLen;
        }
        close(ambamal_fd);
    }

    return Rval;
}
#else
static UINT32 GetMemUsage(UINT32 *pUsedSize, UINT32 *pFreeSize)
{
    UINT32 Rval = ARM_OK;
    INT32  cavalry_fd = 0;
    struct cavalry_usage_mem usage_mem;

    if ((pUsedSize == NULL) || (pFreeSize == NULL)) {
        ArmLog_ERR(ARM_LOG_MEM, "## GetMemUsage: null pointer", 0U, 0U);
        Rval = ARM_NG;
    }

    if(Rval == ARM_OK) {
        if ((cavalry_fd = open("/dev/ambacv", O_SYNC | O_RDWR, 0)) < 0) {
            ArmLog_ERR(ARM_LOG_MEM, "## GetMemUsage: /dev/ambacv open fail", 0U, 0U);
            Rval = ARM_NG;
        }
    }

    if(Rval == ARM_OK) {
        ArmStdC_memset(&usage_mem, 0, sizeof(usage_mem));
        if(ioctl(cavalry_fd, CAVALRY_GET_USAGE_MEM, &usage_mem) < 0) {
            ArmLog_ERR(ARM_LOG_MEM, "## GetMemUsage: CAVALRY_GET_USAGE_MEM fail", 0U, 0U);
            Rval = ARM_NG;
        } else {
            *pUsedSize = (UINT32)usage_mem.used_length;
            *pFreeSize = (UINT32)usage_mem.free_length;
        }
        close(cavalry_fd);
    }

    return Rval;
}
#endif

/*---------------------------------------------------------------------------*\
 * APIs
\*---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ArmMem_Init
 *
 *  @Description:: Give a buffer to manage
 *
 *  @Input      ::
 *    BufAddr:     The given buffer address
 *    BufSize:     The given buffer size
 *    IsCached:    Is it a cached buffer
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    UINT32:      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
UINT32 ArmMem_Init(UINT32 BufAddr, UINT32 BufSize, UINT32 IsCached)
{
    UINT32 Rval = ARM_OK;

    ArmLog_DBG(ARM_LOG_MEM, "Don't need to call ArmMem_Init() in linux environment", 0U, 0U);

    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ArmMem_Allocate
 *
 *  @Description:: Allocate buffer
 *
 *  @Input      ::
 *    ReqSize:     The requested buffer size
 *    IsCached:    Request cached or non-cached buffer
 *
 *  @Output     ::
 *    pReqBuf:     Pointer to the requested buffer
 *
 *  @Return     ::
 *    UINT32:      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
UINT32 ArmMem_Allocate(UINT32 ReqSize, UINT32 IsCached, flexidag_memblk_t *pReqBuf)
{
    UINT32 Rval = ARM_OK;
    UINT32 UsedSize = 0, FreeSize = 0;

    /* 1. Sanity check for input parameters */
    if (pReqBuf == NULL) {
        ArmLog_ERR(ARM_LOG_MEM, "## pReqBuf is null", 0U, 0U);
        Rval = ARM_NG;
    }

    /* 2. Check mem size */
    Rval = GetMemUsage(&UsedSize, &FreeSize);
    if (Rval == ARM_OK) {
        if (ReqSize >= FreeSize) {
            ArmLog_ERR(ARM_LOG_MEM,"Request size(%u) is larger than available size(%u)!!!", ReqSize, FreeSize);
            Rval = ARM_NG;
        }
    }

    /* 3. Allocate memory */
    if (Rval == ARM_OK) {
        for (unsigned int i = 0; i < MAX_RETRY_NUM; i++) {
            Rval = AmbaCV_UtilityCmaMemAlloc(ReqSize, IsCached, pReqBuf);
            if (Rval == CV_ERR_0003) {
                ArmLog_WARN(ARM_LOG_MEM, "PFNs busy: retry!!!");
                usleep(500*1000);
                Rval = ARM_NG;
            } else if (Rval == 0U) {
                break;
            } else {
                Rval = ARM_NG;
                break;
            }
        }
    }

    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ArmMem_Free
 *
 *  @Description:: Free buffer
 *
 *  @Input      ::
 *    pReqBuf:     Pointer to the buffer
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    UINT32:      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
UINT32 ArmMem_Free(flexidag_memblk_t *pBuf)
{
    UINT32 Rval = ARM_OK;

    /* 1. Sanity check for input parameters */
    if (pBuf == NULL) {
        ArmLog_ERR(ARM_LOG_MEM, "## pBuf is null", 0U, 0U);
        Rval = ARM_NG;
    }

    /* 2. Free memory */
    if (Rval == ARM_OK) {
        Rval = AmbaCV_UtilityCmaMemFree(pBuf);
        if (Rval != 0U) {
            ArmLog_ERR(ARM_LOG_MEM, "## AmbaCV_UtilityCmaMemFree() fail (Rval = 0x%x)", Rval, 0U);
            Rval = ARM_NG;
        }
    }

    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ArmMem_CacheClean
 *
 *  @Description:: Do cache clean
 *
 *  @Input      ::
 *    pReqBuf:     Pointer to the buffer
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    UINT32:      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
UINT32 ArmMem_CacheClean(flexidag_memblk_t *pBuf)
{
    UINT32 Rval = ARM_OK;

    /* 1. Sanity check for input parameters */
    if (pBuf == NULL) {
        ArmLog_ERR(ARM_LOG_MEM, "## pBuf is null", 0U, 0U);
        Rval = ARM_NG;
    }

    /* 2. Do cache clean */
    if (Rval == ARM_OK) {
        Rval = AmbaCV_UtilityCmaMemClean(pBuf);
        if (Rval != 0U) {
            ArmLog_ERR(ARM_LOG_MEM, "## AmbaCV_UtilityCmaMemClean() fail (Rval = 0x%x)", Rval, 0U);
            Rval = ARM_NG;
        }
    }

    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ArmMem_Invalid
 *
 *  @Description:: Do cache invalid
 *
 *  @Input      ::
 *    pReqBuf:     Pointer to the buffer
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    UINT32:      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
UINT32 ArmMem_Invalid(flexidag_memblk_t *pBuf)
{
    UINT32 Rval = ARM_OK;

    /* 1. Sanity check for input parameters */
    if (pBuf == NULL) {
        ArmLog_ERR(ARM_LOG_MEM, "## pBuf is null", 0U, 0U);
        Rval = ARM_NG;
    }

    /* 2. Do cache invalid */
    if (Rval == ARM_OK) {
        Rval = AmbaCV_UtilityCmaMemInvalid(pBuf);
        if (Rval != 0U) {
            ArmLog_ERR(ARM_LOG_MEM, "## AmbaCV_UtilityCmaMemInvalid() fail (Rval = 0x%x)", Rval, 0U);
            Rval = ARM_NG;
        }
    }

    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ArmMem_GetMemUsage
 *
 *  @Description:: Get memory usage information of CMA
 *
 *  @Input      ::
 *    pUsedSize:   Pointer to CMA info
 *    pFreeSize:   Pointer to CMA info
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    UINT32:      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
UINT32 ArmMem_GetMemUsage(UINT32 *pUsedSize, UINT32 *pFreeSize)
{
    UINT32 Rval = ARM_OK;
    INT32  cavalry_fd = 0;
    struct cavalry_usage_mem usage_mem;

    if ((pUsedSize == NULL) || (pFreeSize == NULL)) {
        ArmLog_ERR(ARM_LOG_MEM, "## ArmMem_GetMemUsage: null pointer", 0U, 0U);
        Rval = ARM_NG;
    }

    if(Rval == ARM_OK) {
        Rval = GetMemUsage(pUsedSize, pFreeSize);
    }

    if(Rval == ARM_OK) {
        ArmLog_DBG(ARM_LOG_MEM, "[CMA]Used: %luB, Free: %luB", *pUsedSize, *pFreeSize);
        ArmLog_DBG(ARM_LOG_MEM, "[CMA]Used: %luMB Free: %luMB", (*pUsedSize)/(1024*1024), (*pFreeSize)/(1024*1024));
    }

    return Rval;
}

