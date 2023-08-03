/*
 * Copyright (c) 2020 Ambarella International LP
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
 */

#include "hw/gdma.h"
#include "Generic.h"

#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaMisraFix.h"
#include "AmbaKAL.h"
#include "AmbaMMU.h"

#include "AmbaGDMA.h"

/**
 *  AmbaGDMA_DrvEntry - Initializes driver-wide data structures and resources
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaGDMA_DrvEntry(void)
{
    UINT32 RetVal = GDMA_ERR_NONE;

    return RetVal;
}

/**
 *  AmbaGDMA_GetInfo - Retrieve the controller status
 *  @param[out] pNumAvails Number of available transaction slots
 *  @return error code
 */
UINT32 AmbaGDMA_GetInfo(UINT32 * pNumAvails)
{
    UINT32 RetVal = GDMA_ERR_NONE;

    (void)pNumAvails;

    return RetVal;
}

/**
 *  AmbaGDMA_WaitAllCompletion - Wait until the last transaction done
 *  @param[in] TimeOut The maximum wait time
 *  @return error code
 */
UINT32 AmbaGDMA_WaitAllCompletion(UINT32 TimeOut)
{
    UINT32 RetVal = GDMA_ERR_NONE;
    amba_gdma_wait_t GdmaWaitCtrl = {0U};
    int fd, err;

    fd = open("/dev/gdma", O_RDWR);
    if (fd == -1) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: Can't attach to /dev/gdma manager.", __FUNCTION__);
    } else {

        GdmaWaitCtrl.TimeOut = TimeOut;

        err = devctl(fd, DCMD_GDMA_WAIT_COMPLETE, &GdmaWaitCtrl, sizeof(amba_gdma_wait_t), NULL);
        if (err) {
            fprintf(stderr, "%s: failed: %s", __FUNCTION__, strerror(errno));
        }
        close(fd);
    }

    return RetVal;
}

/**
 *  AmbaGDMA_LinearCopy - Schedule a linear copy transaction
 *  @param[in] pLinearBlit The control block of the linear copy
 *  @param[in] NotifierFunc The completion callback
 *  @param[in] NotifierFuncArg The optional argument of the completion callback
 *  @param[in] TimeOut The maximum tolerance time to schedule this transaction
 *  @return error code
 */
UINT32 AmbaGDMA_LinearCopy(const AMBA_GDMA_LINEAR_s * pLinearBlit, AMBA_GDMA_ISR_f NotifierFunc, UINT32 NotifierFuncArg, UINT32 TimeOut)
{
    UINT32 RetVal = GDMA_ERR_NONE;
    amba_gdma_linear_t GdmaCtrl = {0U};
    int fd, err;
    ULONG Src, Dst;

    (void)(NotifierFuncArg);
    (void)(TimeOut);

    if (NotifierFunc != NULL) {
        printf("Warning: NotifierFunc is not supported!\n");
    }

    if (pLinearBlit != NULL) {
        fd = open("/dev/gdma", O_RDWR);
        if (fd == -1) {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: Can't attach to /dev/gdma manager.", __FUNCTION__);
        } else {
            AmbaMMU_VirtToPhys((ULONG)pLinearBlit->pSrcImg, &Src);
            AmbaMMU_VirtToPhys((ULONG)pLinearBlit->pDstImg, &Dst);

            AmbaMisra_TypeCast(&GdmaCtrl.pSrcImg, &Src);
            AmbaMisra_TypeCast(&GdmaCtrl.pDstImg, &Dst);

            GdmaCtrl.NumPixels = pLinearBlit->NumPixels;
            GdmaCtrl.PixelFormat = pLinearBlit->PixelFormat;
            err = devctl(fd, DCMD_GDMA_LINEAR, &GdmaCtrl, sizeof(amba_gdma_linear_t), NULL);
            if (err) {
                fprintf(stderr, "%s: failed: %s", __FUNCTION__, strerror(errno));
            }
            close(fd);
        }
    } else {
        RetVal = GDMA_ERR_ARG;
    }

    return RetVal;
}

/**
 *  AmbaGDMA_BlockCopy - Schedule a block copy transaction
 *  @param[in] pBlockBlit The control block of the block copy
 *  @param[in] NotifierFunc The completion callback
 *  @param[in] NotifierFuncArg The optional argument of the completion callback
 *  @param[in] TimeOut The maximum tolerance time to schedule this transaction
 *  @return error code
 */
UINT32 AmbaGDMA_BlockCopy(const AMBA_GDMA_BLOCK_s * pBlockBlit, AMBA_GDMA_ISR_f NotifierFunc, UINT32 NotifierFuncArg, UINT32 TimeOut)
{
    UINT32 RetVal = GDMA_ERR_NONE;
    amba_gdma_block_t GdmaCtrl = {0U};
    int fd, err;
    ULONG Src, Dst;

    (void)(NotifierFuncArg);
    (void)(TimeOut);

    if (NotifierFunc != NULL) {
        printf("Warning: NotifierFunc is not supported!\n");
    }

    if (pBlockBlit != NULL) {
        fd = open("/dev/gdma", O_RDWR);
        if (fd == -1) {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: Can't attach to /dev/gdma manager.", __FUNCTION__);
        } else {
            AmbaMMU_VirtToPhys((ULONG)pBlockBlit->pSrcImg, &Src);
            AmbaMMU_VirtToPhys((ULONG)pBlockBlit->pDstImg, &Dst);

            AmbaMisra_TypeCast(&GdmaCtrl.pSrcImg, &Src);
            AmbaMisra_TypeCast(&GdmaCtrl.pDstImg, &Dst);

            GdmaCtrl.SrcRowStride = pBlockBlit->SrcRowStride;
            GdmaCtrl.DstRowStride = pBlockBlit->DstRowStride;
            GdmaCtrl.BltWidth = pBlockBlit->BltWidth;
            GdmaCtrl.BltHeight = pBlockBlit->BltHeight;
            GdmaCtrl.PixelFormat = pBlockBlit->PixelFormat;
            err = devctl(fd, DCMD_GDMA_BLOCK, &GdmaCtrl, sizeof(amba_gdma_block_t), NULL);
            if (err) {
                fprintf(stderr, "%s: failed: %s", __FUNCTION__, strerror(errno));
            }
            close(fd);
        }
    } else {
        RetVal = GDMA_ERR_ARG;
    }

    return RetVal;
}

/**
 *  AmbaGDMA_ColorKeying - Schedule a chroma key compositing transaction
 *  @param[in] pBlockBlit The control block of the chroma key compositing
 *  @param[in] TransparentColor The color hue in foreground image treated as transparent
 *  @param[in] NotifierFunc The completion callback
 *  @param[in] NotifierFuncArg The optional argument of the completion callback
 *  @param[in] TimeOut The maximum tolerance time to schedule this transaction
 *  @return error code
 */
UINT32 AmbaGDMA_ColorKeying(const AMBA_GDMA_BLOCK_s * pBlockBlit, UINT32 TransparentColor, AMBA_GDMA_ISR_f NotifierFunc, UINT32 NotifierFuncArg, UINT32 TimeOut)
{
    UINT32 RetVal = GDMA_ERR_NONE;
    amba_gdma_block_t GdmaCtrl = {0U};
    int fd, err;
    ULONG Src, Dst;

    (void)(NotifierFuncArg);
    (void)(TimeOut);

    if (NotifierFunc != NULL) {
        printf("Warning: NotifierFunc is not supported!\n");
    }

    if (pBlockBlit != NULL) {
        fd = open("/dev/gdma", O_RDWR);
        if (fd == -1) {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: Can't attach to /dev/gdma manager.", __FUNCTION__);
        } else {
            AmbaMMU_VirtToPhys((ULONG)pBlockBlit->pSrcImg, &Src);
            AmbaMMU_VirtToPhys((ULONG)pBlockBlit->pDstImg, &Dst);

            AmbaMisra_TypeCast(&GdmaCtrl.pSrcImg, &Src);
            AmbaMisra_TypeCast(&GdmaCtrl.pDstImg, &Dst);

            GdmaCtrl.SrcRowStride = pBlockBlit->SrcRowStride;
            GdmaCtrl.DstRowStride = pBlockBlit->DstRowStride;
            GdmaCtrl.BltWidth = pBlockBlit->BltWidth;
            GdmaCtrl.BltHeight = pBlockBlit->BltHeight;
            GdmaCtrl.PixelFormat = pBlockBlit->PixelFormat;
            GdmaCtrl.TransparentColor = TransparentColor;
            err = devctl(fd, DCMD_GDMA_COLOTKEY, &GdmaCtrl, sizeof(amba_gdma_block_t), NULL);
            if (err) {
                fprintf(stderr, "%s: failed: %s", __FUNCTION__, strerror(errno));
            }
            close(fd);
        }
    } else {
        RetVal = GDMA_ERR_ARG;
    }

    return RetVal;
}

/**
 *  AmbaGDMA_AlphaBlending - Schedule a alpha blending transaction
 *  @param[in] pBlockBlit The control block of the alpha blending
 *  @param[in] AlphaVal The global alpha value
 *  @param[in] BlendMode The alpha value is premultiplied or not
 *  @param[in] NotifierFunc The completion callback
 *  @param[in] NotifierFuncArg The optional argument of the completion callback
 *  @param[in] TimeOut The maximum tolerance time to schedule this transaction
 *  @return error code
 */
UINT32 AmbaGDMA_AlphaBlending(const AMBA_GDMA_BLOCK_s * pBlockBlit, UINT32 AlphaVal, UINT32 BlendMode, AMBA_GDMA_ISR_f NotifierFunc, UINT32 NotifierFuncArg, UINT32 TimeOut)
{
    UINT32 RetVal = GDMA_ERR_NONE;
    amba_gdma_block_t GdmaCtrl = {0U};
    int fd, err;
    ULONG Src, Dst;

    (void)(NotifierFuncArg);
    (void)(TimeOut);

    if (NotifierFunc != NULL) {
        printf("Warning: NotifierFunc is not supported!\n");
    }

    if (pBlockBlit != NULL) {
        fd = open("/dev/gdma", O_RDWR);
        if (fd == -1) {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: Can't attach to /dev/gdma manager.", __FUNCTION__);
        } else {
            AmbaMMU_VirtToPhys((ULONG)pBlockBlit->pSrcImg, &Src);
            AmbaMMU_VirtToPhys((ULONG)pBlockBlit->pDstImg, &Dst);

            AmbaMisra_TypeCast(&GdmaCtrl.pSrcImg, &Src);
            AmbaMisra_TypeCast(&GdmaCtrl.pDstImg, &Dst);

            GdmaCtrl.SrcRowStride = pBlockBlit->SrcRowStride;
            GdmaCtrl.DstRowStride = pBlockBlit->DstRowStride;
            GdmaCtrl.BltWidth = pBlockBlit->BltWidth;
            GdmaCtrl.BltHeight = pBlockBlit->BltHeight;
            GdmaCtrl.PixelFormat = pBlockBlit->PixelFormat;
            GdmaCtrl.TransparentColor = AlphaVal;
            GdmaCtrl.TransparentColor = BlendMode;
            err = devctl(fd, DCMD_GDMA_ALPHABLEND, &GdmaCtrl, sizeof(amba_gdma_block_t), NULL);
            if (err) {
                fprintf(stderr, "%s: failed: %s", __FUNCTION__, strerror(errno));
            }
            close(fd);
        }
    } else {
        RetVal = GDMA_ERR_ARG;
    }

    return RetVal;
}

