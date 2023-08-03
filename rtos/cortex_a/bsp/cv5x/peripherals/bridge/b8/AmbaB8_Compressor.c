/**
 *  @file AmbaB8_Compressor.c
 *
 *  @copyright Copyright (c) 2015 Ambarella, Inc.
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
 *  @details Ambarella B8 Compressor APIs
 *
 */

#include "AmbaB8.h"
#include "AmbaB8_Compressor.h"
#include "AmbaB8_Communicate.h"

#include "AmbaB8CSL_Compressor.h"

const UINT16 AmbaB8_RawCompressCtrl[B8_NUM_LEGACY_COMPRESS_RATIO] = {
    [B8_COMPRESS_NONE]       = 0x00,
    [B8_COMPRESS_4P75]       = 0x31,          /* 4.75 bits/pixel (4 mantissa bits per pixel, 4 pixels per block) */
    [B8_COMPRESS_5P5]        = 0x11,          /* 5.5 bits/pixel (4 mantissa bits per pixel, 2 pixels per block) */
    [B8_COMPRESS_5P75]       = 0x35,          /* 5.75 bits/pixel (5 mantissa bits per pixel, 4 pixels per block) */
    [B8_COMPRESS_6P5]        = 0x15,          /* 6.5 bits/pixel (5 mantissa bits per pixel, 2 pixels per block) */
    [B8_COMPRESS_6P75]       = 0x21,          /* 6.75 bits/pixel (6 mantissa bits per pixel, 4 pixels per block) */
    [B8_COMPRESS_7P5]        = 0x1,           /* 7.5 bits/pixel (6 mantissa bits per pixel, 2 pixels per block) */
    [B8_COMPRESS_7P75]       = 0x25,          /* 7.75 bits/pixel (7 mantissa bits per pixel, 4 pixels per block) */
    [B8_COMPRESS_8P5]        = 0x5,           /* 8.5 bits/pixel (7 mantissa bits per pixel, 2 pixels per block) */
    [B8_COMPRESS_8P75]       = 0x29,          /* 8.75 bits/pixel (8 mantissa bits per pixel, 4 pixels per block) */
    [B8_COMPRESS_9P5]        = 0x9,           /* 9.5 bits/pixel (8 mantissa bits per pixel, 2 pixels per block) */
    [B8_COMPRESS_9P75]       = 0x2D,          /* 9.75 bits/pixel (9 mantissa bits per pixel, 4 pixels per block) */
    [B8_COMPRESS_10P5]       = 0xD,           /* 10.5 bits/pixel (9 mantissa bits per pixel, 2 pixels per block) */
};

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_CompressorConfig
 *
 *  @Description:: Configure B8 Compressor
 *
 *  @Input      ::
 *     ChipID:        B8 chip id
 *     pCompressCtrl: Pointer to configuration
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      UINT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_CompressorConfig(UINT32 ChipID, const B8_SERDES_COMPRESS_s* pCompressCtrl)
{
    UINT16 DataBuf16[10];

    /* Compressor */
    DataBuf16[0] = AmbaB8_RawCompressCtrl[pCompressCtrl->Ratio];
    if (pCompressCtrl->DitherCtrl.Enable == 0U) {
        DataBuf16[1] = 0x0U;
        DataBuf16[2] = 0x0U;
        DataBuf16[3] = 0x0U;
        DataBuf16[4] = 0x0U;
        DataBuf16[5] = 0x0U;
    } else {
        DataBuf16[1] = 0x1U;
        DataBuf16[2] = (UINT16)(pCompressCtrl->DitherCtrl.DitherRandomSeedX & 0xffU);
        DataBuf16[3] = (UINT16)(pCompressCtrl->DitherCtrl.DitherRandomSeedX >> 8U);
        DataBuf16[4] = (UINT16)(pCompressCtrl->DitherCtrl.DitherRandomSeedY & 0xffU);
        DataBuf16[5] = (UINT16)(pCompressCtrl->DitherCtrl.DitherRandomSeedY >> 8U);
    }
    DataBuf16[6] = 0x0U;
    DataBuf16[7] = pCompressCtrl->Offset;
    (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_CompressorReg->Ctrl), 1, B8_DATA_WIDTH_16BIT, 8, DataBuf16);
    DataBuf16[0] = 0x1U;
    (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_CompressorReg->ConfigDone), 0, B8_DATA_WIDTH_16BIT, 1, DataBuf16);

    return B8_ERR_NONE;
}
