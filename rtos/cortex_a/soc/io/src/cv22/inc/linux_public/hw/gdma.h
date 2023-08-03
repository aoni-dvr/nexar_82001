/**
 *  @file gdma.h
 *
 *  @copyright Copyright (c) 2021 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details lib header for GDMA device module
 *
 */

#ifndef _HW_GDMA_DRIVER_H_INCLUDED
#define _HW_GDMA_DRIVER_H_INCLUDED

#include "AmbaGDMA_Def.h"

typedef struct _amba_timer_t {
    unsigned long long SrcImg;
    unsigned long long DstImg;
    unsigned int NumPixels;
    unsigned int PixelFormat;
} amba_gdma_linear_t;

typedef struct {
    unsigned long long SrcImg;                       /* Virtual address */
    unsigned long long DstImg;                       /* Virtual address */
    unsigned int SrcRowStride;              /* Rowstride of the source image (bytes) */
    unsigned int DstRowStride;              /* Rowstride of the destination image (bytes) */
    unsigned int BltWidth;                  /* Width of the block transfer (in number of pixels) */
    unsigned int BltHeight;                 /* Height of the block transfer (in number of rows) */
    unsigned int PixelFormat;
    unsigned int TransparentColor;
    unsigned int AlphaVal;
    unsigned int BlendMode;
} amba_gdma_block_t;

typedef struct {
    unsigned int TimeOut;
} amba_gdma_wait_t;

#define _DCMD_GDMA                'g'

#define DCMD_GDMA_LINEAR            _IOWR(_DCMD_GDMA, 0, amba_gdma_linear_t)
#define DCMD_GDMA_BLOCK             _IOWR(_DCMD_GDMA, 1, amba_gdma_block_t)
#define DCMD_GDMA_COLOTKEY          _IOWR(_DCMD_GDMA, 2, amba_gdma_block_t)
#define DCMD_GDMA_ALPHABLEND        _IOWR(_DCMD_GDMA, 3, amba_gdma_block_t)
#define DCMD_GDMA_WAIT_COMPLETE     _IOWR(_DCMD_GDMA, 4, amba_gdma_wait_t)

#endif /* _HW_GDMA_DRIVER_H_INCLUDED */
