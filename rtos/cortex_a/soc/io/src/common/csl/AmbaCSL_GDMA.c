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

#include "AmbaMisraFix.h"
#include "AmbaCSL_GDMA.h"

/**
 *  AmbaCSL_GdmaSetHeight - Set image height
 *  @param[in] Val The height value
 */
void AmbaCSL_GdmaSetHeight(UINT32 Val)
{
    AMBA_GDMA_HEIGHT_REG_s HeightRegVal;
    UINT32 RegVal = pAmbaGDMA_Reg->Height;

    AmbaMisra_TypeCast32(&HeightRegVal, &RegVal);
    HeightRegVal.Height = (UINT16)(Val - 1U);

    AmbaMisra_TypeCast32(&RegVal, &HeightRegVal);
    pAmbaGDMA_Reg->Height = RegVal;
}

/**
 *  AmbaCSL_GdmaSetWidth - Set image width
 *  @param[in] Val The width value
 */
void AmbaCSL_GdmaSetWidth(UINT32 Val)
{
    AMBA_GDMA_WIDTH_REG_s WidthRegVal;
    UINT32 RegVal = pAmbaGDMA_Reg->Width;

    AmbaMisra_TypeCast32(&WidthRegVal, &RegVal);
    WidthRegVal.Width = (UINT16)(Val - 1U);
#ifdef CONFIG_QNX
    WidthRegVal.CompleteInt = (UINT8)0;
#endif

    AmbaMisra_TypeCast32(&RegVal, &WidthRegVal);
    pAmbaGDMA_Reg->Width = RegVal;
}

/**
 *  AmbaCSL_GdmaSetPixelFormat - Set pixel format of the image
 *  @param[in] PixelFormat The pixel format
 */
void AmbaCSL_GdmaSetPixelFormat(UINT32 PixelFormat)
{
    AMBA_GDMA_PIXEL_FORMAT_REG_s PelFmtRegVal;
    UINT32 RegVal = pAmbaGDMA_Reg->PixelFormat;

    AmbaMisra_TypeCast32(&PelFmtRegVal, &RegVal);
    PelFmtRegVal.DestFormat = (UINT8)PixelFormat;
    PelFmtRegVal.Src2Format = (UINT8)PixelFormat;
    PelFmtRegVal.Src1Format = (UINT8)PixelFormat;

    AmbaMisra_TypeCast32(&RegVal, &PelFmtRegVal);
    pAmbaGDMA_Reg->PixelFormat = RegVal;
}

/**
 *  AmbaCSL_GdmaSetDramThrottle - Set DRAM arbitration parameters
 *  @param[in] MaxOutstandingRequest DRAM arbitration parameters
 */
void AmbaCSL_GdmaSetDramThrottle(UINT32 MaxOutstandingRequest)
{
    AMBA_GDMA_PIXEL_FORMAT_REG_s PelFmtRegVal;
    UINT32 RegVal = pAmbaGDMA_Reg->PixelFormat;

    AmbaMisra_TypeCast32(&PelFmtRegVal, &RegVal);
    PelFmtRegVal.DramThrottle = (UINT8)MaxOutstandingRequest;

    AmbaMisra_TypeCast32(&RegVal, &PelFmtRegVal);
    pAmbaGDMA_Reg->PixelFormat = RegVal;
}
