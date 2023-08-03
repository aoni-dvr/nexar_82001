/**
 *  @file AmbaB8_Merger.c
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
 *  @details Ambarella B8 Merger APIs
 *
 */
#include "AmbaB8.h"
#include "AmbaB8_Communicate.h"
#include "AmbaB8_Merger.h"
#include "AmbaB8_PLL.h"

#include "AmbaB8CSL_Merger.h"
#include "AmbaB8CSL_PLL.h"
#include "AmbaB8CSL_PHY.h"

#include "AmbaB8CSL_Decompressor.h"/* !!!TBD */

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_MergerConfig
 *
 *  @Description:: Configure B8 merger
 *
 *  @Input      ::
 *     ChipID:        B8 chip id
 *     pMergerConfig: Pointer to merge configuration
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_MergerConfig(UINT32 ChipID, const B8_MERGER_CONFIG_s *pMergerConfig)
{
    UINT32 DataBuf32, RegVal;
    DOUBLE FloorVal;
    B8_MERGER_SPLIT_STREAM_WIDTH_REG_s    SplitStreamWidth    = {0};
    B8_MERGER_SPLIT_STREAM_HEIGHT_REG_s   SplitStreamHeight   = {0};
    B8_MERGER_STREAM_WIDTH_REG_s          InputStreamWidth    = {0};
    B8_MERGER_OUTPUT_FORMAT0_REG_s        OutputFrameFormat0  = {0};
    B8_MERGER_OUTPUT_FORMAT1_REG_s        OutputFrameFormat1  = {0};
    B8_MERGER_THRESHOLD_REG_s             Threshold           = {0};
    B8_MERGER_CTRL_REG_s                  Ctrl                = {0};
    B8_MERGER_SOURCE_SEL_REG_s            CfgSource           = {0};
    B8_MERGER_CHANNEL_MUX_REG_s           CfgChanMux          = {0};
    B8_MERGER_MODE_CFG_REG_s              CfgMode             = {0};
    B8_MERGER_SENSOR_PATH_REG_s           SensorPath          = {0};
    UINT16 DecprsCtrl;/* !!!TBD */

    /* reset value */
    RegVal = 0xa0;
    (void) AmbaB8_Wrap_memcpy(&Ctrl, &RegVal, sizeof(UINT32));
    RegVal = 0xe4;
    (void) AmbaB8_Wrap_memcpy(&CfgChanMux, &RegVal, sizeof(UINT32));
    RegVal = 0x8000;
    (void) AmbaB8_Wrap_memcpy(&CfgMode, &RegVal, sizeof(UINT32));

    Ctrl.Enable = 1U;
    if (pMergerConfig->PixelWidth == 14U) {
        Ctrl.VoutPixelWidth = 8U;
    } else if (pMergerConfig->PixelWidth == 12U) {
        Ctrl.VoutPixelWidth = 4U;
    } else if (pMergerConfig->PixelWidth == 10U) {
        Ctrl.VoutPixelWidth = 2U;
    } else {
        Ctrl.VoutPixelWidth = 1U;
    }

    /* B8F with only one sensor input */
    if (((ChipID & B8_SUB_CHIP_ID_MASK) != B8_SUB_CHIP_ID_B8N) &&
        ((pMergerConfig->SourceSelect[0U] == B8_MERGER_SOURCE_NONE) || (pMergerConfig->SourceSelect[1U] == B8_MERGER_SOURCE_NONE))) {

        /* 0x1C */
        SplitStreamWidth.SplitWidthTh = pMergerConfig->InputWidth;
        SplitStreamWidth.SplitWidth = pMergerConfig->InputWidth;
        (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_MergerReg->SplitStreamWidth, 0, B8_DATA_WIDTH_32BIT, 1, &SplitStreamWidth);

        /* 0x20 */
        SplitStreamHeight.SplitHeight = pMergerConfig->InputHeight - 1U;  /* Smaller than height (TBD) */
        (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_MergerReg->SplitStreamHeight, 0, B8_DATA_WIDTH_32BIT, 1, &SplitStreamHeight);

        /* Configuration done */
        DataBuf32 = 0x1;
        (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_MergerReg->CfgDone, 0, B8_DATA_WIDTH_32BIT, 1, &DataBuf32);

        /* Ctrl, enable Merger*/
        Ctrl.B8anEnable = 0U;      /* B8F mode */
        if (pMergerConfig->SourceSelect[0U] == B8_MERGER_SOURCE_FROM_SENSOR) {
            Ctrl.FarEndMode = 0U;  /* bypass mode, sensor input from VIN */
        } else {
            Ctrl.FarEndMode = 1U;  /* swap VIN bypass mode, sensor input from PIP*/
        }
        (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_MergerReg->Ctrl, 0, B8_DATA_WIDTH_32BIT, 1, &Ctrl);


        /* B8N or B8F with merger mode */
    } else {

        /* 0x4 */
        (void) AmbaB8_RegRead(ChipID, & (pAmbaB8_DeCompressorReg->Ctrl), 1U, B8_DATA_WIDTH_16BIT, 1, &DecprsCtrl);
        if ((DecprsCtrl & 0x3U) == 0U) { /* if (DecprsCtrl.Enable == 0U) { */
            CfgSource.DecmpSourceEn = 0U;   /* !!!TBD */
        } else {
            CfgSource.DecmpSourceEn = 1U;   /* !!!TBD */
        }

        if (pMergerConfig->SourceSelect[0U] == B8_MERGER_SOURCE_FROM_SENSOR) {
            CfgSource.VoutSensorSelect |= 0x1U;
        }
        if (pMergerConfig->SourceSelect[1U] == B8_MERGER_SOURCE_FROM_SENSOR) {
            CfgSource.VoutSensorSelect |= 0x4U;
        }
        (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_MergerReg->SourceSel, 0, B8_DATA_WIDTH_32BIT, 1, &CfgSource);

        /* 0x8 */
        if (pMergerConfig->SourceSelect[0U] == B8_MERGER_SOURCE_NONE) {
            /* only one input from source 1 */
            CfgChanMux.VoufMuxCh0 = 2U;
            CfgChanMux.VoufLastCh = 0U;
        } else if (pMergerConfig->SourceSelect[1U] == B8_MERGER_SOURCE_NONE) {
            /* only one input from source 0 */
            CfgChanMux.VoufMuxCh0 = 0U;
            CfgChanMux.VoufLastCh = 0U;
        } else {
            /* dual input (merge mode) */
            if (pMergerConfig->ViewSwap == 0U) {
                CfgChanMux.VoufMuxCh0 = 0U;
                CfgChanMux.VoufMuxCh1 = 2U;
            } else {
                CfgChanMux.VoufMuxCh0 = 2U;
                CfgChanMux.VoufMuxCh1 = 0U;
            }
            CfgChanMux.VoufLastCh = 1U;
        }
        (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_MergerReg->ChannelMux, 0, B8_DATA_WIDTH_32BIT, 1, &CfgChanMux);

        /* 0x10 */
        (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_MergerReg->ModeConfig, 0, B8_DATA_WIDTH_32BIT, 1, &CfgMode);

        /* 0x14, 0x18 */
        InputStreamWidth.LineWidth_0_2 = pMergerConfig->InputWidth;
        InputStreamWidth.LineWidth_1_3 = pMergerConfig->InputWidth;
        (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_MergerReg->InputStreamWidth0, 0, B8_DATA_WIDTH_32BIT, 1, &InputStreamWidth);
        (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_MergerReg->InputStreamWidth1, 0, B8_DATA_WIDTH_32BIT, 1, &InputStreamWidth);

        /* 0x2c */
        OutputFrameFormat0.OutputFrameHeight = pMergerConfig->InputHeight - 1U;  /* Smaller than height */
        OutputFrameFormat0.MinHB = 10;
        OutputFrameFormat0.MinVB = 10;
        (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_MergerReg->OutputFrameFormat0, 1, B8_DATA_WIDTH_32BIT, 1, &OutputFrameFormat0);

        /* 0x30*/
        OutputFrameFormat1.MaxHB = pMergerConfig->MaxHblank;
        (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_MergerReg->OutputFrameFormat1, 1, B8_DATA_WIDTH_32BIT, 1, &OutputFrameFormat1);

        /* 0x34, 0x38, 0x3c, 0x40*/
        Threshold.LineBufferRaemptyTh = pMergerConfig->InputWidth;
        (void) AmbaB8_Wrap_floor((DOUBLE)pMergerConfig->InputWidth * 1.5, &FloorVal);
        Threshold.LineBufferWafullTh = (UINT16)FloorVal;
        (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_MergerReg->Threshold0, 0, B8_DATA_WIDTH_32BIT, 1, &Threshold);
        (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_MergerReg->Threshold1, 0, B8_DATA_WIDTH_32BIT, 1, &Threshold);
        (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_MergerReg->Threshold2, 0, B8_DATA_WIDTH_32BIT, 1, &Threshold);
        (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_MergerReg->Threshold3, 0, B8_DATA_WIDTH_32BIT, 1, &Threshold);

        /* 0x64 */
        SensorPath.VoutInternalGen = 0U;       /* VOUTF from sensor */
        SensorPath.VoutGenEnable   = 0xFU;
        (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_MergerReg->SensorPath, 0, B8_DATA_WIDTH_32BIT, 1, &SensorPath);

        /* Configuration done */
        DataBuf32 = 0x1;
        (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_MergerReg->CfgDone, 0, B8_DATA_WIDTH_32BIT, 1, &DataBuf32);

        /* Ctrl, enable Merger*/
        if ((ChipID & B8_SUB_CHIP_ID_MASK) == B8_SUB_CHIP_ID_B8N) {
            Ctrl.B8anEnable = 1U;
        } else {
            Ctrl.B8anEnable = 0U;
            Ctrl.FarEndMode = 3U;
        }
        (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_MergerReg->Ctrl, 0, B8_DATA_WIDTH_32BIT, 1, &Ctrl);
    }

    return B8_ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_MergerConfigWithCodec
 *
 *  @Description:: Configure B8 merger with CFA Codec enabled
 *
 *  @Input      ::
 *     ChipID:                 B8 chip id
 *     pMergerConfigWithCodec: Pointer to merge configuration
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_MergerConfigWithCodec(UINT32 ChipID, const B8_MERGER_CONFIG_WITH_CODEC_s *pMergerConfigWithCodec)
{
    UINT32 DataBuf32[32], RegVal;
    B8_MERGER_CTRL_REG_s Ctrl = {0};
    B8_MERGER_SPLIT_STREAM_WIDTH_REG_s    SplitStreamWidth = {0};
    B8_MERGER_SPLIT_STREAM_HEIGHT_REG_s   SplitStreamHeight = {0};
    B8_MERGER_STREAM_WIDTH_REG_s          InputStreamWidth = {0};
    B8_MERGER_OUTPUT_FORMAT0_REG_s        OutputFrameFormat0 = {0};
    B8_MERGER_HDR_SENSOR_CFG_REG_s        HdrSensorCfg = {0};
    B8_MERGER_THRESHOLD_REG_s             Threshold = {0};

    if ((pMergerConfigWithCodec->MergeType == B8_MERGER_BYPASS) || (pMergerConfigWithCodec->MergeType == B8_MERGER_SPLIT)) {
        if ((ChipID & B8_SUB_CHIP_ID_MASK) != 0U) { /* B8F */
            SplitStreamWidth.SplitWidthTh = pMergerConfigWithCodec->InputWidth;
            SplitStreamWidth.SplitWidth = pMergerConfigWithCodec->InputWidth;
            (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_MergerReg->SplitStreamWidth, 0, B8_DATA_WIDTH_32BIT, 1, &SplitStreamWidth);

            SplitStreamHeight.SplitHeight = pMergerConfigWithCodec->InputHeight - 1U;
            (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_MergerReg->SplitStreamHeight, 0, B8_DATA_WIDTH_32BIT, 1, &SplitStreamHeight);

            /* Merger control */
            if (pMergerConfigWithCodec->MergeType == B8_MERGER_SPLIT) {
                RegVal = 0x1221;
                (void)AmbaB8_Wrap_memcpy(&Ctrl, &RegVal, sizeof(UINT32));
            } else {
                RegVal = 0x221;
                (void)AmbaB8_Wrap_memcpy(&Ctrl, &RegVal, sizeof(UINT32));
            }
            (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_MergerReg->Ctrl, 0, B8_DATA_WIDTH_32BIT, 1, &Ctrl);

            /* Configuration done */
            DataBuf32[0] = 0x1;
            (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_MergerReg->CfgDone, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf32);
        } else { /* B8N */
            /* Source from serdes */
            if (pMergerConfigWithCodec->SourceSelect == B8_MERGER_SOURCE_FROM_SERDES) {
                RegVal = 0x1d20U;
                (void) AmbaB8_Wrap_memcpy(&Ctrl, &RegVal, sizeof(UINT32));

                (void) AmbaB8_Wrap_memcpy(&DataBuf32[0], &Ctrl, sizeof(UINT32));
                DataBuf32[1] = 0x00000U;
                if (pMergerConfigWithCodec->NearEndInputStreams == B8_MERGER_NEAR_END_4_INPUT_STREAMS) {
                    DataBuf32[2] = 0x3e4U;
                } else {
                    DataBuf32[2] = 0x1e4U;
                }
                DataBuf32[3] = 0x0U;
                DataBuf32[4] = 0x8080U;
                (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_MergerReg->Ctrl, 1, B8_DATA_WIDTH_32BIT, 5, DataBuf32);

                InputStreamWidth.LineWidth_0_2 = pMergerConfigWithCodec->InputWidth;
                InputStreamWidth.LineWidth_1_3 = pMergerConfigWithCodec->InputWidth;

                (void) AmbaB8_Wrap_memcpy(&DataBuf32[0], &InputStreamWidth, sizeof(UINT32));
                (void) AmbaB8_Wrap_memcpy(&DataBuf32[1], &InputStreamWidth, sizeof(UINT32));
                /* Stream 0&1: even pixels; Stream 2&3: odd pixels */
                (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_MergerReg->InputStreamWidth0, 1, B8_DATA_WIDTH_32BIT, 2, DataBuf32);

                RegVal = 0xa028437U;
                (void) AmbaB8_Wrap_memcpy(&OutputFrameFormat0, &RegVal, sizeof(UINT32));
                OutputFrameFormat0.OutputFrameHeight = pMergerConfigWithCodec->InputHeight - 1U;  /* Smaller than height */
                OutputFrameFormat0.MinHB = 10U;
                OutputFrameFormat0.MinVB = 10U;

                (void) AmbaB8_Wrap_memcpy(&DataBuf32[0], &OutputFrameFormat0, sizeof(UINT32));
                DataBuf32[1] = pMergerConfigWithCodec->MaxHblank;
                (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_MergerReg->OutputFrameFormat0, 1, B8_DATA_WIDTH_32BIT, 2, DataBuf32);

                Threshold.LineBufferWafullTh = pMergerConfigWithCodec->InputWidth << 1U;  /*   2x of input (stream) line width */
                Threshold.LineBufferRaemptyTh = pMergerConfigWithCodec->InputWidth;      /*   1x of input (stream) line width */
                (void) AmbaB8_Wrap_memcpy(&DataBuf32[0], &Threshold, sizeof(UINT32));
                (void) AmbaB8_Wrap_memcpy(&DataBuf32[1], &Threshold, sizeof(UINT32));

                Threshold.LineBufferWafullTh = pMergerConfigWithCodec->InputWidth;       /*   1x of input (stream) line width */
                Threshold.LineBufferRaemptyTh = pMergerConfigWithCodec->InputWidth >> 1U; /* 0.5x of input (stream) line width */
                (void) AmbaB8_Wrap_memcpy(&DataBuf32[2], &Threshold, sizeof(UINT32));
                (void) AmbaB8_Wrap_memcpy(&DataBuf32[3], &Threshold, sizeof(UINT32));

                (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_MergerReg->Threshold0, 1, B8_DATA_WIDTH_32BIT, 4, DataBuf32);

                if (pMergerConfigWithCodec->NumExposures > 1U) {
                    /* (num_exp_m1_0+1)*4*(width_div_4_m1_0+1) = reg_input_line_pixel_0
                     * (num_exp_m1_1+1)*4*(width_div_4_m1_1+1) = reg_input_line_pixel_1 */
                    HdrSensorCfg.NumExpMinus1_0 = pMergerConfigWithCodec->NumExposures - 1U;
                    HdrSensorCfg.WidthDiv4Minus1_0 = (pMergerConfigWithCodec->PictureWidth / 4U) - 1U;
                    HdrSensorCfg.NumExpMinus1_1 = pMergerConfigWithCodec->NumExposures - 1U;
                    HdrSensorCfg.WidthDiv4Minus1_1 = (pMergerConfigWithCodec->PictureWidth / 4U) - 1U;
                } else {
                    /* Default values */
                    HdrSensorCfg.NumExpMinus1_0 = 0U;
                    HdrSensorCfg.WidthDiv4Minus1_0 = 479U;
                    HdrSensorCfg.NumExpMinus1_1 = 0U;
                    HdrSensorCfg.WidthDiv4Minus1_1 = 479U;
                }
                (void) AmbaB8_Wrap_memcpy(&DataBuf32[0], &HdrSensorCfg, sizeof(UINT32));
                if (pMergerConfigWithCodec->NearEndInputStreams == B8_MERGER_NEAR_END_4_INPUT_STREAMS) {
                    DataBuf32[1] = 0x0f;
                } else {
                    DataBuf32[1] = 0x3;
                }
                DataBuf32[2] = 0x0U;
                if (pMergerConfigWithCodec->NearEndInputStreams == B8_MERGER_NEAR_END_4_INPUT_STREAMS) {
                    DataBuf32[3] = 0x0c;
                } else {
                    DataBuf32[3] = 0x4;
                }
                (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_MergerReg->HdrSensorCfg, 1, B8_DATA_WIDTH_32BIT, 4, DataBuf32);

                /* Merger control */
                Ctrl.Enable = (UINT8)1U;
                if (pMergerConfigWithCodec->PixelWidth == 8U) {
                    Ctrl.VoutPixelWidth = 1U;
                } else if (pMergerConfigWithCodec->PixelWidth == 10U) {
                    Ctrl.VoutPixelWidth = 2U;
                } else if (pMergerConfigWithCodec->PixelWidth == 12U) {
                    Ctrl.VoutPixelWidth = 4U;
                } else if (pMergerConfigWithCodec->PixelWidth == 14U) {
                    Ctrl.VoutPixelWidth = 8U;
                } else {
                    Ctrl.VoutPixelWidth = 2U;
                }
                (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_MergerReg->Ctrl, 0, B8_DATA_WIDTH_32BIT, 1, &Ctrl);

                /* Configuration done */
                DataBuf32[0] = 0x1U;
                (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_MergerReg->CfgDone, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf32);
            }
        }
    }

    return B8_ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_MergerSlvsConfig
 *
 *  @Description:: Configure B8 Merger VOUTF SLVS configuration
 *
 *  @Input      ::
 *     ChipID:            B8 chip id
 *     pMergerSlvsConfig: Pointer to configuration
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_MergerSlvsConfig(UINT32 ChipID, const B8_MERGER_SLVS_CONFIG_s *pMergerSlvsConfig)
{
    UINT32 Rval = B8_ERR_NONE;
    UINT32 DataBuf32[2];
    B8_MERGER_CTRL_REG_s Ctrl = {0x0};
    B8_MERGER_OUTPUT_FORMAT0_REG_s OutputFrameFormat0 = {0};
    B8_MERGER_OUTPUT_FORMAT1_REG_s OutputFrameFormat1 = {0};
//    B8_DSI_CTRL_REG2_REG_s DsiCtrlReg2 = {.Data = 0xe0ef1};
//    B8_DSI_CTRL_AUX0_REG_s DsiCtrlAux0 = {.Data = 0x300};
    B8_SYSTEM_RESET_REG_s SysReset = {0};

    AmbaB8_PrintUInt5("SlvsDataRate:%u, LaneWidth:%u, PixelWidth:%u, MinHB: %u, MinVB: %u",
                      pMergerSlvsConfig->SlvsDataRate, pMergerSlvsConfig->LaneWidth, pMergerSlvsConfig->PixelWidth,
                      pMergerSlvsConfig->MinHblank, pMergerSlvsConfig->MinVblank);

    (void) AmbaB8_PllSetVout0Clk(ChipID, pMergerSlvsConfig->SlvsDataRate);

    (void) AmbaB8_RegRead(ChipID, & pAmbaB8_MergerReg->Ctrl, 0, B8_DATA_WIDTH_32BIT, 1, &Ctrl);

    /* Disable Merger */
    Ctrl.Enable = 0;
    (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_MergerReg->Ctrl, 0, B8_DATA_WIDTH_32BIT, 1, &Ctrl);

    (void) AmbaB8_RegRead(ChipID, & pAmbaB8_MergerReg->OutputFrameFormat0, 1, B8_DATA_WIDTH_32BIT, 2, DataBuf32);

    /* Update MinHblank, MinVblank, MaxHblank */
    (void) AmbaB8_Wrap_memcpy(&OutputFrameFormat0, &DataBuf32[0], sizeof(UINT32));
    OutputFrameFormat0.MinHB = pMergerSlvsConfig->MinHblank;
    OutputFrameFormat0.MinVB = pMergerSlvsConfig->MinVblank;
    (void) AmbaB8_Wrap_memcpy(&DataBuf32[0], &OutputFrameFormat0, sizeof(UINT32));

    (void) AmbaB8_Wrap_memcpy(&OutputFrameFormat1, &DataBuf32[1], sizeof(UINT32));
    OutputFrameFormat1.MaxHB = pMergerSlvsConfig->MaxHblank;
    OutputFrameFormat1.VoutfBorderWidth = 0;
    (void) AmbaB8_Wrap_memcpy(&DataBuf32[1], &OutputFrameFormat1, sizeof(UINT32));

    (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_MergerReg->OutputFrameFormat0, 1, B8_DATA_WIDTH_32BIT, 2, DataBuf32);

    /* SLVS output adjustment (data channel 0 - 3) */
    DataBuf32[0] = 0x1;
    (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_MergerReg->CfgDone, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf32);

    /* Enable clock channel and data channel 0 - 3 */
    DataBuf32[0] = 0x61010313;
    (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_PhyReg->Dsi0CtrlReg0Reg), 0, B8_DATA_WIDTH_32BIT, 1, DataBuf32);

    /* Enable clock channel and data channel 0 - 3 */
    DataBuf32[0] = 0x1000001f;
    (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_PhyReg->Dsi0CtrlReg2Reg), 0, B8_DATA_WIDTH_32BIT, 1, DataBuf32);

    /* Enable serial SLVS mode (channel 0 - 3) */
    DataBuf32[0] = 0x200;
    (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_PhyReg->Dsi0CtrlAux0Reg), 0, B8_DATA_WIDTH_32BIT, 1, DataBuf32);

    if (pMergerSlvsConfig->LaneWidth == 8U) {
        /* SLVS output adjustment (data channel 4 - 7) */
        DataBuf32[0] = 0x61010313;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_PhyReg->Dsi1CtrlReg0Reg), 0, B8_DATA_WIDTH_32BIT, 1, DataBuf32);

        /* Enable clock channel and data channel 4 - 7 */
        DataBuf32[0] = 0x1000001f;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_PhyReg->Dsi1CtrlReg2Reg), 0, B8_DATA_WIDTH_32BIT, 1, DataBuf32);

        /* Enable serial SLVS mode (channel 4 - 7) */
        DataBuf32[0] = 0x200;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_PhyReg->Dsi1CtrlAux0Reg), 0, B8_DATA_WIDTH_32BIT, 1, DataBuf32);

        /* Enable 8 lanes SLVS mode */
        DataBuf32[0] = 0x1;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_PhyReg->Dsi8LaneModeReg), 0, B8_DATA_WIDTH_32BIT, 1, DataBuf32);
    }

    /* Reset DSI */
    SysReset.Dsi0 = 0x1;
    (void) AmbaB8_Wrap_memcpy(&DataBuf32[0], &SysReset, sizeof(UINT32));
    SysReset.Dsi0 = 0x0;
    (void) AmbaB8_Wrap_memcpy(&DataBuf32[1], &SysReset, sizeof(UINT32));
    (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_PllReg->SysReset), 0, B8_DATA_WIDTH_32BIT, 2, DataBuf32);

    /* Slvs lane width */
    if (pMergerSlvsConfig->LaneWidth == 1U) {
        Ctrl.lvdsLaneWidth = 0x01U;
    } else if (pMergerSlvsConfig->LaneWidth == 2U) {
        Ctrl.lvdsLaneWidth = 0x02U;
    } else if (pMergerSlvsConfig->LaneWidth == 4U) {
        Ctrl.lvdsLaneWidth = 0x04U;
    } else if (pMergerSlvsConfig->LaneWidth == 8U) {
        Ctrl.lvdsLaneWidth = 0x08U;
    } else {
        Rval = B8_ERR_ARG;
    }

    /* Pixel width */
    if (pMergerSlvsConfig->PixelWidth == 8U) {
        Ctrl.VoutPixelWidth = 0x1U;
    } else if (pMergerSlvsConfig->PixelWidth == 10U) {
        Ctrl.VoutPixelWidth = 0x2U;
    } else if (pMergerSlvsConfig->PixelWidth == 12U) {
        Ctrl.VoutPixelWidth = 0x4U;
    } else if (pMergerSlvsConfig->PixelWidth == 14U) {
        Ctrl.VoutPixelWidth = 0x8U;
    } else {
        Rval = B8_ERR_ARG;
    }

    /* Enable Merger and SLVS output */
    Ctrl.Enable = 1;
    Ctrl.NearEndLvdsEn = 1;
    (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_MergerReg->Ctrl, 0, B8_DATA_WIDTH_32BIT, 1, &Ctrl);

    return Rval;
}
