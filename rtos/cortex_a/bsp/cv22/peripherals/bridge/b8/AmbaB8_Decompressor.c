/**
 *  @file AmbaB8_Decompressor.c
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
 *  @details Ambarella B8 Decompressor APIs
 *
 */
#include "AmbaB8.h"

#include "AmbaB8_Compressor.h"
#include "AmbaB8_Decompressor.h"
#include "AmbaB8_Communicate.h"

#include "AmbaB8CSL_Decompressor.h"
#include "AmbaB8CSL_Scratchpad.h"

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_DecompressorConfig
 *
 *  @Description:: Configure B8 Decompressor
 *
 *  @Input      ::
 *     ChipID:        B8 chip id
 *     pCodecConfig:  Pointer to configuration
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      UINT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_DecompressorConfig(UINT32 ChipID, UINT16 ImageWidth, UINT16 ImageHeight, const B8_SERDES_COMPRESS_s* pCompressCtrl)
{
    UINT32 DataBuf32[2];
    UINT16 DataBuf16[10];
    UINT16 Ctrl;
    UINT16 VinfWidth;
    DOUBLE CeilVal;
    UINT8 Mantissa, BlockSize;
    UINT32 i = 0U;

    if ((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK) != 0U) {
        /* Decompressor */
        Ctrl = AmbaB8_RawCompressCtrl[pCompressCtrl->Ratio];
        DataBuf16[0] = AmbaB8_RawCompressCtrl[pCompressCtrl->Ratio];
        DataBuf16[1] = (UINT16)(ImageWidth - 1U);
        DataBuf16[2] = (UINT16)(ImageHeight - 1U);
        if (pCompressCtrl->DitherCtrl.Enable == 0U) {
            DataBuf16[3] = 0x0U;
            DataBuf16[4] = 0xc040U;
            DataBuf16[5] = 0x5707U;
            DataBuf16[6] = 0xf864U;
            DataBuf16[7] = 0x1636U;
        } else {
            DataBuf16[3] = 0x1U;
            DataBuf16[4] = (UINT16)(pCompressCtrl->DitherCtrl.DitherRandomSeedX & 0xffU);
            DataBuf16[5] = (UINT16)(pCompressCtrl->DitherCtrl.DitherRandomSeedX >> 8U);
            DataBuf16[6] = (UINT16)(pCompressCtrl->DitherCtrl.DitherRandomSeedY & 0xffU);
            DataBuf16[7] = (UINT16)(pCompressCtrl->DitherCtrl.DitherRandomSeedY >> 8U);
        }
        DataBuf16[8] = pCompressCtrl->Offset;
        DataBuf16[9] = 0x1U;


        (void) AmbaB8_RegWrite((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), & (pAmbaB8_DeCompressorReg->Ctrl), 1U, B8_DATA_WIDTH_16BIT, 10, DataBuf16);

        if (pCompressCtrl->Ratio != B8_COMPRESS_NONE) {
            /* B8N Vinf */
            Mantissa = (UINT8)((Ctrl >> 2U) & (UINT16)0x7U);  /* Ctrl.MantissaBits; */
            BlockSize = (UINT8)((((Ctrl >> 5U) & (UINT16)0x1U) * 2U) + 2U); /* (Ctrl.BlockSize * 2U) + 2U; */

            if ((Mantissa != 4U) && (Mantissa != 5U)) {
                Mantissa = (UINT8)(((Ctrl >> 2U) & (UINT16)0x7U) + 6U); /* Ctrl.MantissaBits + 6U */;
            }

            if (pCompressCtrl->Ratio == B8_COMPRESS_NONE) {
                VinfWidth = (UINT16)ImageWidth;
            } else {
                if ((BlockSize == (UINT8)4U) || (((ImageWidth / (UINT32)BlockSize) % 2U) == 1U)) {
                    (void) AmbaB8_Wrap_ceil(((((DOUBLE)ImageWidth / (DOUBLE)BlockSize) + 1.0) * (((DOUBLE)BlockSize * (DOUBLE)Mantissa) + 3.0) / 28.0), &CeilVal);
                    VinfWidth = (UINT16)((UINT16)CeilVal * 2U); //(UINT32)(CeilVal * 2.0);
                } else {
                    (void) AmbaB8_Wrap_ceil((((DOUBLE)ImageWidth / (DOUBLE)BlockSize)  * (((DOUBLE)BlockSize * (DOUBLE)Mantissa) + 3.0) / 28.0), &CeilVal);
                    VinfWidth = (UINT16)((UINT16)CeilVal * 2U); //(UINT16)(CeilVal * 2.0);
                }
            }

            DataBuf32[0] = (((UINT32)VinfWidth - 1U) << (UINT32)16U) | ((UINT32)ImageHeight - 1U) | 0x80000000U;
            DataBuf32[1] = 0xffffU;

            for (i = 0U; i < B8_MAX_NUM_B8F_ON_CHAN; i++) {
                if ((ChipID & ((UINT32)1U << i)) != 0U) {
                    (void) AmbaB8_RegWrite((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), &(pAmbaB8_ScratchpadReg->Vinf0Config), 1U, B8_DATA_WIDTH_32BIT, 2, DataBuf32);
                }
            }
        }
    }

    return B8_ERR_NONE;
}
