/**
 *  @file AmbaB8_Packer.c
 *
 *  @copyright Copyright (c) 2018 Ambarella, Inc.
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
 *  @details Ambarella B8 Packer APIs
 *
 */
#include "AmbaB8.h"
#include "AmbaB8_Packer.h"
#include "AmbaB8_Communicate.h"

#include "AmbaB8CSL_Packer.h"
#include "AmbaB8CSL_PLL.h"

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_PackerReset
 *
 *  @Description:: Software Reset B8 Packer
 *
 *  @Input      ::
 *     ChipID:           B8 chip id
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 *
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_PackerReset(UINT32 ChipID)
{
    UINT32 RetVal = B8_ERR_NONE;
    UINT32 ResetValue = 0x1fffff;
    B8_AHB_SW_RST_1_REG_s AhbSwRst1;

    (void) AmbaB8_Wrap_memcpy(&AhbSwRst1, &ResetValue, sizeof(B8_AHB_SW_RST_1_REG_s));

    AhbSwRst1.RstLPacker = 0U;
    RetVal |= AmbaB8_RegWrite(ChipID, & pAmbaB8_PllReg->AhbSwReset1, 0U, B8_DATA_WIDTH_32BIT, 1U, &AhbSwRst1);
    AhbSwRst1.RstLPacker = 3U;
    RetVal |= AmbaB8_RegWrite(ChipID, & pAmbaB8_PllReg->AhbSwReset1, 0U, B8_DATA_WIDTH_32BIT, 1U, &AhbSwRst1);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_PackerConfig
 *
 *  @Description:: Configure B8 Packer Config
 *
 *  @Input      ::
 *     ChipID:           B8 chip id
 *     pPackerConfig: Pointer to configuration
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 *
\*-----------------------------------------------------------------------------------------------*/
void AmbaB8_PackerConfig(UINT32 ChipID, const B8_PACKER_CONFIG_s *pPackerConfig)
{
    B8_PACKER_PACKET_CTRL_REG_s PacketCtrl = {0};
    B8_PACKER_PIXEL_CTRL_REG_s PixelCtrl = {0};
    B8_PACKER_PACKET_LENGTH_REG_s PacketLength = {0};
    B8_PACKER_TEST_CTRL_REG_s TestCtrl = {0};

    /* disable pixel packet */
    (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_PackerReg->PacketCtrl), 0U, B8_DATA_WIDTH_32BIT, 1U, &PacketCtrl);

    /* normal mode */
    (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_PackerReg->TestCtrl), 0U, B8_DATA_WIDTH_32BIT, 1U, &TestCtrl);

    /* pixel depth */
    PixelCtrl.PelDepth = (UINT8)pPackerConfig->PelDepth;
    /* ecc enable */
    PixelCtrl.PelEccEnable = 1U;
    (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_PackerReg->PixelCtrl), 0U, B8_DATA_WIDTH_32BIT, 1U, &PixelCtrl);

    /* pixel length */
    PacketLength.MinBytePkt = pPackerConfig->PktByteNumMin;
    PacketLength.MaxBytePkt = pPackerConfig->PktByteNumMax;
    (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_PackerReg->PacketLength), 0U, B8_DATA_WIDTH_32BIT, 1U, &PacketLength);

    /* enable pixel packet & counter */
    PacketCtrl.PacketCntEn = 1U;
    if((ChipID & B8_MAIN_CHIP_ID_B8ND_MASK) != 0U) {
        PacketCtrl.CtlPktEn = 1U;
    }

    PacketCtrl.PelPktEn = 1U;
    (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_PackerReg->PacketCtrl), 0U, B8_DATA_WIDTH_32BIT, 1U, &PacketCtrl);
}

UINT32 AmbaB8_PackerTestMode(UINT32 ChipID)
{
    UINT32 RetVal = B8_ERR_NONE;
    B8_PACKER_TEST_CTRL_REG_s TestCtrl = {0};
    B8_PACKER_TEST_ACTIVE_REG_s TestActivePic = {0};
    B8_PACKER_TEST_TIMING_CTRL_REG_s TestTimingCtrl = {0};

    TestActivePic.TestActiveWidth = (UINT16)(1920U - 2U);
    TestActivePic.TestActiveHeight = (UINT16)(1080U - 1U);
    RetVal |= AmbaB8_RegWrite(ChipID, & (pAmbaB8_PackerReg->TestActivePic), 0U, B8_DATA_WIDTH_32BIT, 1U, &TestActivePic);

    /*Data = 0x46506f4;*/  /* Correct frame rate to 60/1.001 fps */
    /* Correct frame rate to 30/1.001 fps */
    TestTimingCtrl.TestLineCycleCnt = 0x2444;
    TestTimingCtrl.TestPicLineCnt = 0x0465;
    RetVal |= AmbaB8_RegWrite(ChipID, & (pAmbaB8_PackerReg->TestTimingCtrl), 0U, B8_DATA_WIDTH_32BIT, 1U, &TestTimingCtrl);

    /*Data = 0xf15;*/  /* Unlimited packets */
    TestCtrl.TestMode = 1U;
    TestCtrl.StatusSrc = 1U;
    TestCtrl.TestSampleRate = 5U;
    RetVal |= AmbaB8_RegWrite(ChipID, & (pAmbaB8_PackerReg->TestCtrl), 0U, B8_DATA_WIDTH_32BIT, 1U, &TestCtrl);

    return RetVal;
}

