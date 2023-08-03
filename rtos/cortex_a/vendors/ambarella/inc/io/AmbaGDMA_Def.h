/**
 *  @file AmbaGDMA_Def.h
 *
 *  Copyright (c) 2020 Ambarella International LP
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
 *  @details Common Definitions & Constants for GDMA APIs
 *
 */

#ifndef AMBA_GDMA_DEF_H
#define AMBA_GDMA_DEF_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif
#ifndef AMBA_ERROR_CODE_H
#include "AmbaErrorCode.h"
#endif

#define GDMA_ERR_0000           (GDMA_ERR_BASE)              /* Invalid argument */
#define GDMA_ERR_0001           (GDMA_ERR_BASE + 0X1U)       /* Unable to do concurrency protection */
#define GDMA_ERR_0002           (GDMA_ERR_BASE + 0X2U)       /* Timeout occurred */
#define GDMA_ERR_00FF           (GDMA_ERR_BASE + 0XFFU)      /* Unexpected error */

#define GDMA_ERR_NONE           (OK)
#define GDMA_ERR_ARG            GDMA_ERR_0000
#define GDMA_ERR_MUTEX          GDMA_ERR_0001
#define GDMA_ERR_TMO            GDMA_ERR_0002
#define GDMA_ERR_UNEXPECTED     GDMA_ERR_00FF

/*
 * Limitations
 */
#define GDMA_NUM_INSTANCE           (8U)
#define GDMA_MAX_WIDTH              (8192U)
#define GDMA_MAX_HEIGHT             (4096U)

/*
 * Pixel Format Definitions
 */
#define GDMA_8_BIT                  (0U) /* Indexed color */
#define GDMA_4_BIT                  (1U) /* Indexed color */
#define GDMA_16_BIT_1555            (2U)
#define GDMA_16_BIT_565             (3U)
#define GDMA_24_BIT                 (4U)
#define GDMA_32_BIT                 (5U)
#define NUM_GDMA_PIXEL_FORMAT       (6U)

/* Legacy definitions */
#define AMBA_GDMA_NUM_INSTANCE      GDMA_NUM_INSTANCE
#define AMBA_GDMA_MAX_WIDTH         GDMA_MAX_WIDTH
#define AMBA_GDMA_MAX_HEIGHT        GDMA_MAX_HEIGHT

#define AMBA_GDMA_8_BIT             GDMA_8_BIT
#define AMBA_GDMA_4_BIT             GDMA_4_BIT
#define AMBA_GDMA_16_BIT_1555       GDMA_16_BIT_1555
#define AMBA_GDMA_16_BIT_565        GDMA_16_BIT_565
#define AMBA_GDMA_24_BIT            GDMA_24_BIT
#define AMBA_GDMA_32_BIT            GDMA_32_BIT
#define AMBA_NUM_GDMA_PIXEL_FORMAT  NUM_GDMA_PIXEL_FORMAT

typedef void (*AMBA_GDMA_ISR_f)(UINT32 UserArg);

typedef struct {
    void    *pSrcImg;                       /* Virtual address */
    void    *pDstImg;                       /* Virtual address */
    UINT32  NumPixels;                      /* Nnumber of pixels */
    UINT32  PixelFormat;
} AMBA_GDMA_LINEAR_s;

typedef struct {
    void    *pSrcImg;                       /* Virtual address */
    void    *pDstImg;                       /* Virtual address */
    UINT32  SrcRowStride;                   /* Rowstride of the source image (bytes) */
    UINT32  DstRowStride;                   /* Rowstride of the destination image (bytes) */
    UINT32  BltWidth;                       /* Width of the block transfer (in number of pixels) */
    UINT32  BltHeight;                      /* Height of the block transfer (in number of rows) */
    UINT32  PixelFormat;
} AMBA_GDMA_BLOCK_s;

#endif /* AMBA_GDMA_DEF_H */
