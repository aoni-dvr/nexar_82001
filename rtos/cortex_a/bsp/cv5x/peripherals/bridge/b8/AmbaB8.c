/**
 *  @file AmbaB8.c
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
 *  @details B8 driver APIs
 *
 */

#include "AmbaB8.h"
#include "AmbaB8_Communicate.h"
#include "AmbaB8_GPIO.h"
#include "AmbaB8_I2C.h"
#include "AmbaB8_SPI.h"
#include "AmbaB8_PLL.h"
#include "AmbaB8_Codec.h"
#include "AmbaB8_Merger.h"
#include "AmbaB8_Packer.h"
#include "AmbaB8_Prescaler.h"
#include "AmbaB8_Compressor.h"
#include "AmbaB8_Decompressor.h"
#include "AmbaB8_Serdes.h"
#include "AmbaB8_PHY.h"

#include "AmbaB8CSL_PLL.h"

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_Init
 *
 *  @Description:: B8 device driver initialization
 *
 *  @Input      ::
 *      ChipID:       B8 chip id
 *      pPinConfig:   Pointer to B8 Pin Configure
 *      PhyResetFlag: Serdes Phy reset flag (0: none, others: reset serdes phy)
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_Init(UINT32 ChipID, const B8_PIN_CONFIG_s *pPinConfig)
{
    UINT32 RetVal = B8_ERR_NONE;
    UINT32 Relink = 20U;
    UINT32 i;

    /* Semaphore and Mutex initialization */
    RetVal |= AmbaB8_CommInit();
    RetVal |= AmbaB8_I2cInit();
    RetVal |= AmbaB8_SpiInit();

    /* PLL initialization */
    if ((ChipID & B8_SUB_CHIP_ID_MASK) != 0U) {
        (void) AmbaB8_PllInit(ChipID, 72000000U);
    }
    if ((ChipID & B8_MAIN_CHIP_ID_B8N_MASK) != 0U) {
        (void) AmbaB8_PllInit((ChipID & B8_MAIN_CHIP_ID_B8N_MASK), 72000000U);
    }

    if (RetVal == B8_ERR_NONE) {
        /* Serdes link initialization */
        if ((ChipID & B8_SUB_CHIP_ID_MASK) != 0U) {
            RetVal = AmbaB8_SerdesLink(ChipID, pPinConfig->SerDesRate);

            /* Serdes relink */
            while ((RetVal != B8_ERR_NONE) && (Relink > 0U)) {
                RetVal = AmbaB8_SerdesLink(ChipID, pPinConfig->SerDesRate);
                Relink = Relink - 1U;
            }
            AmbaB8_PrintUInt5("---- Serdes Link(%u, %u) ----\n\n", RetVal, (20U - Relink), 0U, 0U, 0U);
        }

        if (RetVal == B8_ERR_NONE) {
            /* PinMux initialization */
            if ((ChipID & B8_SUB_CHIP_ID_MASK) != 0U) {
                (void) AmbaB8_GpioPinMuxInit(ChipID, pPinConfig);
            }
            if ((ChipID & B8_MAIN_CHIP_ID_B8N_MASK) != 0U) {
                (void) AmbaB8_GpioPinMuxInit((ChipID & B8_MAIN_CHIP_ID_B8N_MASK), pPinConfig);
            }

            /* VIN reset & initialization */
            if ((ChipID & B8_SUB_CHIP_ID_MASK) != 0U) {
                /* sweep B8F */
                for (i = 0U; i < B8_MAX_NUM_SUBCHIP; i++) {
                    if ((ChipID & ((UINT32)0x1U << i)) != 0U) {
                        /* Vin */
                        if ((pPinConfig->SensorID & ((UINT32)0x10 << (4U * i))) != 0U) {
                            (void) AmbaB8_VinReset(((ChipID & B8_MAIN_CHIP_ID_B8N_MASK) | (ChipID & ((UINT32)0x1U << i))), B8_VIN_CHANNEL0, pPinConfig->VinInterface, pPinConfig->pVinMipiDphyCtrl);
                        }
                        /* Pip */
                        if ((pPinConfig->SensorID & ((UINT32)0x20 << (4U * i))) != 0U) {
                            (void) AmbaB8_VinReset(((ChipID & B8_MAIN_CHIP_ID_B8N_MASK) | (ChipID & ((UINT32)0x1U << i))), B8_VIN_CHANNEL1, pPinConfig->VinInterface, pPinConfig->pVinMipiDphyCtrl);
                        }
                    }
                }
            }
            if ((ChipID & B8_MAIN_CHIP_ID_B8N_MASK) != 0U) {
                /* Vin */
                if ((pPinConfig->SensorID & B8_SENSOR_ID_B8N_VIN) != 0U) {
                    (void) AmbaB8_VinReset((ChipID & B8_MAIN_CHIP_ID_B8N_MASK), B8_VIN_CHANNEL0, pPinConfig->VinInterface, pPinConfig->pVinMipiDphyCtrl);
                }
                /* Pip */
                if ((pPinConfig->SensorID & B8_SENSOR_ID_B8N_PIP) != 0U) {
                    (void) AmbaB8_VinReset((ChipID & B8_MAIN_CHIP_ID_B8N_MASK), B8_VIN_CHANNEL1, pPinConfig->VinInterface, pPinConfig->pVinMipiDphyCtrl);
                }
            }

        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_Config
 *
 *  @Description:: Configure B8 for ready to receive video data
 *
 *  @Input      ::
 *      ChipID:        B8 chip id
 *      pSensorInfo: Pointer to B8 Sensor Info
 *      pB8Config:     Pointer to B8 Configuraiton
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_Config(UINT32 ChipID, const B8_CONFIG_s *pB8Config)
{
    UINT32 RetVal = B8_ERR_NONE;
    UINT32 i;
    const B8_SERDES_INFO_s *pSerdesInfo;
    const B8_SENSOR_INFO_s *pSensorInfo;
    const B8_INPUT_INFO_s  *pInputInfo;

    B8_VOUT_CONFIG_s VoutConfig = {0U};
    B8_CODEC_CONFIG_s CodecConfig = {0U};
    B8_MERGER_CONFIG_s B8fMergerConfig = {0};
    B8_MERGER_CONFIG_s B8nMergerConfig = {0};
    B8_MERGER_CONFIG_WITH_CODEC_s B8fMergerConfigWithCodec = {0};
    B8_MERGER_CONFIG_WITH_CODEC_s B8nMergerConfigWithCodec = {0};
    B8_PACKER_CONFIG_s PackerConfig = {0};
    B8_MERGER_SLVS_CONFIG_s B8nSlvsConfig = {0};
    B8_PRESCALER_CONFIG_s PrescalerConfig = {0};

    UINT32 SubChipID = 0;
    UINT16 SubSensorId[B8_MAX_NUM_B8F_ON_CHAN + 1U] = {0};
    UINT16 NumSensorInputOnB8N = 0, NumSensorInputOnB8F[B8_MAX_NUM_B8F_ON_CHAN] = {0}, TotalSensorNum = 0;
    UINT16 VinCropWidth, VinCropHeight;
    UINT32 B8nCoreClk, MinHblank, MaxHblank, FrameWidth;
    DOUBLE FloorVal, SlvsPixelNum, VoutfHblank;
    UINT32 FarSplitLine = 0U;

    const B8_VIN_MIPI_CONFIG_s* pVinConfigMipi = NULL;
    const B8_VIN_SLVS_CONFIG_s* pVinConfigSlvs = NULL;
    const B8_VIN_DVP_CONFIG_s* pVinConfigDvp = NULL;
    static const UINT32 B8_SerdesDataRate[B8_NUM_SERDES_RATE] = {
        [B8_SERDES_RATE_2P3G ] = 2304U,
        [B8_SERDES_RATE_2P59G] = 2592U,
        [B8_SERDES_RATE_3P02G] = 3024U,
        [B8_SERDES_RATE_3P45G] = 3456U,
        [B8_SERDES_RATE_3P60G] = 3600U,
        [B8_SERDES_RATE_3P74G] = 3744U,
        [B8_SERDES_RATE_3P88G] = 3888U,
        [B8_SERDES_RATE_4P03G] = 4032U,
        [B8_SERDES_RATE_4P17G] = 4176U,
        [B8_SERDES_RATE_4P32G] = 4320U,
        [B8_SERDES_RATE_4P46G] = 4464U,
        [B8_SERDES_RATE_5P04G] = 5040U,
        [B8_SERDES_RATE_5P47G] = 5472U,
        [B8_SERDES_RATE_5P76G] = 5760U,
        [B8_SERDES_RATE_6P04G] = 6048U,
    };

#if (B8_DBG_TEMP)
    extern UINT32 gB8DbgConfig;
    if (gB8DbgConfig != 0U) {
#endif

        if ((ChipID == 0U) || (pB8Config == NULL)) {
            RetVal = B8_ERR_ARG;
        } else {
            pSerdesInfo = &pB8Config->SerdesInfo;
            pSensorInfo = &pB8Config->InputInfo.SensorInfo;
            pInputInfo  = &pB8Config->InputInfo;

            /* Group B8n, f0, f1 */
            for (i = 0U; i < (B8_MAX_NUM_B8F_ON_CHAN + 1U); i++) {
                SubSensorId[i]  = (UINT16)((pInputInfo->SensorID >> (i << 2))& 0x3U);
            }

            /* #Sensors on B8n, f1, f2... (!note: Max. #Sensors = 2) */
            NumSensorInputOnB8N = ((SubSensorId[0] + 1U) >> 1U);
            TotalSensorNum += NumSensorInputOnB8N;
            for (i = 0U; i < B8_MAX_NUM_B8F_ON_CHAN; i++) {
                NumSensorInputOnB8F[i]  = ((SubSensorId[i+1U] + (UINT16)1U) >> 1U);
                TotalSensorNum += NumSensorInputOnB8F[i];
            }

            VinCropWidth  = (UINT16)((pInputInfo->VinCropWindow.EndX - pInputInfo->VinCropWindow.StartX) + 1U);
            VinCropHeight = (UINT16)((pInputInfo->VinCropWindow.EndY - pInputInfo->VinCropWindow.StartY) + 1U);

            /* CFA Codec Configuration */
            if (((ChipID & B8_SUB_CHIP_ID_MASK) != 0U) && (pSerdesInfo->CompressCtrl.Ratio == B8_USE_CFA_CODEC)) {
                if ((NumSensorInputOnB8F[0] == 1U) || (NumSensorInputOnB8F[1] == 1U)) {
                    if(((FLOAT)VinCropWidth / (((FLOAT)AmbaB8_PllGetCoreClk(ChipID) * 2.0) / 3.0)) > pSensorInfo->RowTime) {
                        FarSplitLine = 1U;
                    }
                }
                CodecConfig.PictureHeight = VinCropHeight / pSensorInfo->NumPicturesPerFrame;
                CodecConfig.PictureWidth = (UINT16)pInputInfo->PrescalerWidth;
                CodecConfig.TargetBitRate = AmbaB8_CodecGetVariableBitRate((UINT16)pInputInfo->PrescalerWidth, (UINT8)NumSensorInputOnB8F[0],
                                            B8_SerdesDataRate[pSerdesInfo->SerDesRate], pSensorInfo->RowTime);
                CodecConfig.CfaType = (UINT8)pSensorInfo->BayerPattern;
                CodecConfig.TwoCables = 0U;
                if (FarSplitLine != 0U) {
                    CodecConfig.SplitHorizontal = 1U;
                } else {
                    if (NumSensorInputOnB8F[0] > 1U) {
                        CodecConfig.SplitHorizontal = 1U;
                    } else {
                        CodecConfig.SplitHorizontal = 0U;
                    }
                }
                CodecConfig.NumFarSensors = (UINT8)NumSensorInputOnB8F[0];
                CodecConfig.NumPicturesPerFrame = pSensorInfo->NumPicturesPerFrame;
                CodecConfig.NumInputBits = (UINT8)pSensorInfo->NumDataBits;
            }

            /* Configure B8n */
            if ((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK) != 0U) {
                /* Vout CSI */
                for (i = 0U; i < AMBA_NUM_B8_VOUT_CHANNEL; i++) {
                    if (pB8Config->OutputInfo[i].OutputMode == B8_VOUT_MODE_CSI) {
                        VoutConfig.Input.DataRate = pSensorInfo->DataRate;
                        VoutConfig.Input.NumDataLanes = pSensorInfo->NumDataLanes;
                        VoutConfig.Input.NumDataBits = pSensorInfo->NumDataBits;
                        VoutConfig.Output.DataRate = pB8Config->OutputInfo[i].DataRate;
                        VoutConfig.Output.NumDataLanes = pB8Config->OutputInfo[i].NumDataLanes;
                        VoutConfig.Output.NumDataBits = pB8Config->OutputInfo[i].NumDataBits;

                        if ((pSerdesInfo->CompressCtrl.Ratio == B8_USE_CFA_CODEC) && (pSensorInfo->NumPicturesPerFrame > 1U)) {
                            VoutConfig.Source = B8_VOUT_SOURCE_MERGER_CODEC;
                            VoutConfig.LineLengthPck = (UINT16)(pSensorInfo->LineLengthPck * pSensorInfo->NumPicturesPerFrame);
                            VoutConfig.FrameLengthLines = pSensorInfo->FrameLengthLines / pSensorInfo->NumPicturesPerFrame;
                            VoutConfig.OutputWidth = (UINT16)(pInputInfo->PrescalerWidth * TotalSensorNum * pSensorInfo->NumPicturesPerFrame);
                            VoutConfig.OutputHeight = VinCropHeight / pSensorInfo->NumPicturesPerFrame;
                        } else {
                            VoutConfig.Source = B8_VOUT_SOURCE_MERGER;
                            VoutConfig.LineLengthPck = pSensorInfo->LineLengthPck;
                            VoutConfig.FrameLengthLines = pSensorInfo->FrameLengthLines;
                            VoutConfig.OutputWidth = (UINT16)(pInputInfo->PrescalerWidth * TotalSensorNum);
                            VoutConfig.OutputHeight = VinCropHeight;
                        }
                        (void) AmbaB8_VoutMipiCsiConfig((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), i, &VoutConfig);
                    }
                }

                /* Decompressor/ Codec */
                if ((ChipID & B8_SUB_CHIP_ID_MASK) != 0U) {
                    if (pSerdesInfo->CompressCtrl.Ratio == B8_USE_CFA_CODEC) {
                        (void) AmbaB8_CodecConfig((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_CODEC_DECODER, &CodecConfig);
                    } else {
                        for (i = 0U; i < B8_MAX_NUM_B8F_ON_CHAN; i++) {
                            if ((ChipID & ((UINT32)1U << i)) != 0U) {
                                (void) AmbaB8_DecompressorConfig(((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK) | ((UINT32)1U << i)),
                                                                 (UINT16)(pInputInfo->PrescalerWidth * NumSensorInputOnB8F[i]),
                                                                 VinCropHeight, &(pSerdesInfo->CompressCtrl));
                            }
                        }
                    }
                }

                /* Merger */
                if (((ChipID & B8_SUB_CHIP_ID_MASK) != 0U) && (pSerdesInfo->CompressCtrl.Ratio == B8_USE_CFA_CODEC)) {
                    /* codec case */
                    B8nMergerConfigWithCodec.MergeType = B8_MERGER_BYPASS;
                    B8nMergerConfigWithCodec.SourceSelect = B8_MERGER_SOURCE_FROM_SERDES;
                    if (FarSplitLine != 0U) {
                        B8nMergerConfigWithCodec.InputWidth = (UINT16)(((pInputInfo->PrescalerWidth * pSensorInfo->NumPicturesPerFrame) >> 1U) >> 1U);
                    } else {
                        B8nMergerConfigWithCodec.InputWidth = (UINT16)((pInputInfo->PrescalerWidth * pSensorInfo->NumPicturesPerFrame) >> 1U);
                    }
                    B8nMergerConfigWithCodec.InputHeight = VinCropHeight / pSensorInfo->NumPicturesPerFrame;

                    if (FarSplitLine != 0U) {
                        B8nMergerConfigWithCodec.NearEndInputStreams = B8_MERGER_NEAR_END_4_INPUT_STREAMS;
                    } else {
                        if ((NumSensorInputOnB8F[0] > 1U) || (NumSensorInputOnB8F[1] > 1U)) {
                            B8nMergerConfigWithCodec.NearEndInputStreams = B8_MERGER_NEAR_END_4_INPUT_STREAMS;
                        } else if ((NumSensorInputOnB8F[0] != 0U) && (NumSensorInputOnB8F[1] != 0U)) {
                            B8nMergerConfigWithCodec.NearEndInputStreams = B8_MERGER_NEAR_END_4_INPUT_STREAMS;
                        } else {
                            B8nMergerConfigWithCodec.NearEndInputStreams = B8_MERGER_NEAR_END_2_INPUT_STREAMS;
                        }
                    }
                    B8nMergerConfigWithCodec.PictureWidth = (UINT16)pInputInfo->PrescalerWidth;
                    B8nMergerConfigWithCodec.NumExposures = pSensorInfo->NumPicturesPerFrame;
                    B8nMergerConfigWithCodec.PixelWidth = pSensorInfo->NumDataBits;

                    B8nCoreClk = AmbaB8_PllGetCoreClk(ChipID & B8_MAIN_CHIP_ID_B8NF_MASK);
                    (void) AmbaB8_Wrap_floor((((DOUBLE)pSensorInfo->RowTime * (DOUBLE)B8nCoreClk * (DOUBLE)pSensorInfo->NumPicturesPerFrame) + 0.5), &FloorVal);
                    B8nMergerConfigWithCodec.MaxHblank = (UINT16)FloorVal;

                    (void) AmbaB8_MergerConfigWithCodec((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), &B8nMergerConfigWithCodec);
                } else {
                    /* merger input ch0 */
                    if ((SubSensorId[0U] & 0x1U) != 0U) {
                        B8nMergerConfig.SourceSelect[0U] = B8_MERGER_SOURCE_FROM_SENSOR;
                        B8nMergerConfig.InputWidth = (UINT16)pInputInfo->PrescalerWidth;
                    } else if (SubSensorId[1U] != 0U) {
                        B8nMergerConfig.SourceSelect[0U] = B8_MERGER_SOURCE_FROM_SERDES;
                        B8nMergerConfig.InputWidth = (UINT16)(pInputInfo->PrescalerWidth * NumSensorInputOnB8F[0U]);
                    } else {
                        B8nMergerConfig.SourceSelect[0U] = B8_MERGER_SOURCE_NONE;
                    }

                    /* merger input ch1 */
                    if ((SubSensorId[0U] & 0x2U) != 0U) {
                        B8nMergerConfig.SourceSelect[1U] = B8_MERGER_SOURCE_FROM_SENSOR;
                        B8nMergerConfig.InputWidth = (UINT16)pInputInfo->PrescalerWidth;
                    } else if (SubSensorId[2U] != 0U) {
                        B8nMergerConfig.SourceSelect[1U] = B8_MERGER_SOURCE_FROM_SERDES;
                        B8nMergerConfig.InputWidth = (UINT16)(pInputInfo->PrescalerWidth * NumSensorInputOnB8F[1U]);
                    } else {
                        B8nMergerConfig.SourceSelect[1U] = B8_MERGER_SOURCE_NONE;
                    }

                    B8nMergerConfig.InputHeight = VinCropHeight;
                    /* TBD, wait for B8_PLL driver to report actual core clock */
                    /* (void) AmbaB8_Wrap_floor(1.1 * (DOUBLE)pSensorInfo->RowTime * (DOUBLE)AmbaB8_PllGetCoreClk(ChipID) - ((DOUBLE)VinCropWidth / 2.0), &FloorVal); */
                    B8nMergerConfig.MaxHblank = 0x2fffU; //(UINT16) FloorVal;
                    B8nMergerConfig.PixelWidth = pSensorInfo->NumDataBits;
                    B8nMergerConfig.ViewSwap = pB8Config->OutputInfo[0].ViewSwap;

                    (void) AmbaB8_MergerConfig((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), &B8nMergerConfig);
                }

                /* Merger SLVS */
                for (i = 0U; i < AMBA_NUM_B8_VOUT_CHANNEL; i++) {
                    if (pB8Config->OutputInfo[i].OutputMode == B8_VOUT_MODE_SLVS) {
                        B8nCoreClk = AmbaB8_PllGetCoreClk(ChipID & B8_MAIN_CHIP_ID_B8NF_MASK);
                        if (((ChipID & B8_SUB_CHIP_ID_MASK) != 0U) && (pSerdesInfo->CompressCtrl.Ratio == B8_USE_CFA_CODEC) && (pSensorInfo->NumPicturesPerFrame > 1U)) {
                            SlvsPixelNum = ((DOUBLE)pB8Config->OutputInfo[i].DataRate / (DOUBLE)pB8Config->OutputInfo[i].NumDataBits) * (pSensorInfo->RowTime * (DOUBLE)pSensorInfo->NumPicturesPerFrame);
                            FrameWidth = ((UINT32)pSensorInfo->OutputWidth * (UINT32)TotalSensorNum) * (UINT32)pSensorInfo->NumPicturesPerFrame;
                            (void) AmbaB8_Wrap_floor((((DOUBLE)pSensorInfo->RowTime * (DOUBLE)B8nCoreClk * (DOUBLE)pSensorInfo->NumPicturesPerFrame) + 0.5), &FloorVal);
                            MaxHblank = (UINT32)FloorVal;
                        } else {
                            SlvsPixelNum = ((DOUBLE)pB8Config->OutputInfo[i].DataRate / (DOUBLE)pB8Config->OutputInfo[i].NumDataBits) * pSensorInfo->RowTime;
                            FrameWidth = (UINT32)pSensorInfo->OutputWidth * (UINT32)TotalSensorNum;
                            (void) AmbaB8_Wrap_floor(((pSensorInfo->RowTime * (DOUBLE)B8nCoreClk) + 0.5), &FloorVal);
                            MaxHblank = (UINT32)FloorVal;
                        }
                        (void) AmbaB8_Wrap_ceil(((DOUBLE) FrameWidth / (DOUBLE)pB8Config->OutputInfo[i].NumDataLanes), &VoutfHblank);
                        VoutfHblank = (SlvsPixelNum - 8.0) - VoutfHblank;

                        (void) AmbaB8_Wrap_floor((((VoutfHblank / 2.0) - 1.0) + 0.5), &FloorVal);
                        MinHblank = 1U + (UINT32)FloorVal;

                        B8nSlvsConfig.SlvsDataRate = pB8Config->OutputInfo[i].DataRate;
                        B8nSlvsConfig.LaneWidth = (UINT8)pB8Config->OutputInfo[i].NumDataLanes;
                        B8nSlvsConfig.PixelWidth = (UINT8)pB8Config->OutputInfo[i].NumDataBits;
                        B8nSlvsConfig.MinVblank = 10U;
                        if (MinHblank < 1U) {
                            B8nSlvsConfig.MinHblank = 1U;
                        } else if (MinHblank > 1023U) {
                            B8nSlvsConfig.MinHblank = 1023U;
                        } else {
                            B8nSlvsConfig.MinHblank = (UINT16)MinHblank;
                        }
                        if (MaxHblank < 1U) {
                            B8nSlvsConfig.MaxHblank = 1U;
                        } else if (MaxHblank > 65535U) {
                            B8nSlvsConfig.MaxHblank = 65535U;
                        } else {
                            B8nSlvsConfig.MaxHblank = (UINT16)MaxHblank;
                        }

                        (void) AmbaB8_MergerSlvsConfig((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), &B8nSlvsConfig);
                    }
                }

            }

            /* Configure B8f */
            if ((ChipID & B8_SUB_CHIP_ID_MASK) != 0U) {
                /* Packer */
                if (pSerdesInfo->CompressCtrl.Ratio != B8_COMPRESS_NONE) {
                    if (pSerdesInfo->CompressCtrl.Ratio == B8_USE_CFA_CODEC) {
                        /* Fixed pixel width 12-bit when using CFA Codec */
                        PackerConfig.PelDepth = B8_PACKER_12PIXEL_DEPTH;

                        /* max_byte_pkt:251, min_byte_pkt:128. (Note: min_byte_pkt should be greater than (x + 1) * 3 + 7, where x=seg_length_minus1) */
                        PackerConfig.PktByteNumMax = 251U;
                        PackerConfig.PktByteNumMin = 128U;
                    } else {
                        /* configure PACKER to tx the data with 14-bit for compress mode */
                        PackerConfig.PelDepth = B8_PACKER_14PIXEL_DEPTH;
                        PackerConfig.PktByteNumMax = 251U;
                        PackerConfig.PktByteNumMin = 64U;
                    }
                } else {
                    PackerConfig.PelDepth = (UINT16)((pSensorInfo->NumDataBits - (UINT32)8U) >> (UINT32)1U);
                    PackerConfig.PktByteNumMax = 251U;
                    PackerConfig.PktByteNumMin = 64U;
                }
                AmbaB8_PackerConfig(ChipID, &PackerConfig);

                /* Compressior / Codec */
                if (pSerdesInfo->CompressCtrl.Ratio == B8_USE_CFA_CODEC) {
                    (void) AmbaB8_CodecConfig(ChipID, B8_CODEC_ENCODER, &CodecConfig);
                } else {
                    (void) AmbaB8_CompressorConfig(ChipID, &(pSerdesInfo->CompressCtrl));
                }

                /* Merger */
                if (pSerdesInfo->CompressCtrl.Ratio == B8_USE_CFA_CODEC) {
                    B8fMergerConfigWithCodec.SourceSelect = B8_MERGER_SOURCE_FROM_SENSOR;
                    if (FarSplitLine != 0U) {
                        B8fMergerConfigWithCodec.MergeType = B8_MERGER_SPLIT;
                        B8fMergerConfigWithCodec.InputWidth = (UINT16)((pInputInfo->PrescalerWidth * pSensorInfo->NumPicturesPerFrame) >> 1);
                    } else {
                        B8fMergerConfigWithCodec.MergeType = B8_MERGER_BYPASS;
                        B8fMergerConfigWithCodec.InputWidth = (UINT16)(pInputInfo->PrescalerWidth * pSensorInfo->NumPicturesPerFrame);
                    }
                    B8fMergerConfigWithCodec.InputHeight = VinCropHeight / pSensorInfo->NumPicturesPerFrame;
                    B8fMergerConfigWithCodec.PictureWidth = (UINT16)pInputInfo->PrescalerWidth;
                    B8fMergerConfigWithCodec.NumExposures = pSensorInfo->NumPicturesPerFrame;
                    B8fMergerConfigWithCodec.PixelWidth = pSensorInfo->NumDataBits;
                    B8fMergerConfigWithCodec.MaxHblank = 0x2fff; /* FIXME: to be checked */
                    (void) AmbaB8_MergerConfigWithCodec(ChipID, &B8fMergerConfigWithCodec);
                } else {
                    if ((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK) != 0U) {
                        for (i = 0U; i < B8_MAX_NUM_B8F_ON_CHAN; i++) {
                            if ((ChipID & ((UINT32)1U << i)) != 0U) {
                                B8fMergerConfig.SourceSelect[0U] = B8_MERGER_SOURCE_NONE;
                                B8fMergerConfig.SourceSelect[1U] = B8_MERGER_SOURCE_NONE;

                                if ((SubSensorId[i + 1U] & (UINT16)0x1U) != 0U) {    /* VIN */
                                    B8fMergerConfig.SourceSelect[0U] = B8_MERGER_SOURCE_FROM_SENSOR;
                                }

                                if ((SubSensorId[i + 1U] & (UINT16)0x2U) != 0U) {    /* PIP */
                                    B8fMergerConfig.SourceSelect[1U] = B8_MERGER_SOURCE_FROM_SENSOR;
                                }

                                B8fMergerConfig.InputWidth = (UINT16)pInputInfo->PrescalerWidth;
                                B8fMergerConfig.InputHeight = VinCropHeight;
                                (void) AmbaB8_Wrap_floor((((DOUBLE)pInputInfo->PrescalerWidth / 2.0) + ((DOUBLE)pSensorInfo->RowTime * (DOUBLE)AmbaB8_PllGetCoreClk(ChipID)) + 0.5), &FloorVal);
                                B8fMergerConfig.MaxHblank = (UINT16) FloorVal;
                                B8fMergerConfig.PixelWidth = pSensorInfo->NumDataBits;

                                (void) AmbaB8_MergerConfig(((ChipID & B8_MAIN_CHIP_ID_MASK) | ((UINT32)1U << i)), &B8fMergerConfig);
                            }
                        }
                    } else {
                        if ((SubSensorId[1U] & (UINT16)0x1U) != 0U) {    /* VIN */
                            B8fMergerConfig.SourceSelect[0U] = B8_MERGER_SOURCE_FROM_SENSOR;
                        } else {
                            B8fMergerConfig.SourceSelect[0U] = B8_MERGER_SOURCE_NONE;
                        }

                        if ((SubSensorId[1U] & (UINT16)0x2U) != 0U) {    /* PIP */
                            B8fMergerConfig.SourceSelect[1U] = B8_MERGER_SOURCE_FROM_SENSOR;
                        } else {
                            B8fMergerConfig.SourceSelect[1U] = B8_MERGER_SOURCE_NONE;
                        }
                        B8fMergerConfig.InputWidth = (UINT16)pInputInfo->PrescalerWidth;
                        B8fMergerConfig.InputHeight = VinCropHeight;
                        (void) AmbaB8_Wrap_floor((((DOUBLE)pInputInfo->PrescalerWidth / 2.0) + ((DOUBLE)pSensorInfo->RowTime * (DOUBLE)AmbaB8_PllGetCoreClk(ChipID)) + 0.5), &FloorVal);
                        B8nMergerConfig.MaxHblank = (UINT16) FloorVal;
                        B8fMergerConfig.PixelWidth = pSensorInfo->NumDataBits;

                        (void) AmbaB8_MergerConfig(ChipID, &B8fMergerConfig);
                    }
                }
            }

            /* Vin */
            if (pInputInfo->VinInterface == B8_VIN_MIPI) {
                AmbaB8_Misra_TypeCast(&pVinConfigMipi, &(pInputInfo->pVinConfig));
            } else if (pInputInfo->VinInterface == B8_VIN_LVDS) {
                AmbaB8_Misra_TypeCast(&pVinConfigSlvs, &(pInputInfo->pVinConfig));
            } else if (pInputInfo->VinInterface == B8_VIN_LVCMOS) {
                AmbaB8_Misra_TypeCast(&pVinConfigDvp, &(pInputInfo->pVinConfig));
            } else {
                RetVal = B8_ERR_ARG;
            }

            /* Horizontal Prescale 2D */
            PrescalerConfig.InputWidth = VinCropWidth;
            PrescalerConfig.OutputWidth = (UINT16)pInputInfo->PrescalerWidth;
            PrescalerConfig.ReadoutMode = 1U;

            for (i = 0; i < (B8_MAX_NUM_B8F_ON_CHAN + 1U); i++) {
                if (SubSensorId[i] != 0U) {
                    /* get chip id */
                    if (i == 0U) {
                        /* Sensor on B8N */
                        SubChipID = (ChipID & B8_MAIN_CHIP_ID_MASK);
                    } else {
                        /* Sensor on B8F0 or B8F1 */
                        if ((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK) != 0U) {
                            SubChipID = (ChipID & B8_MAIN_CHIP_ID_MASK) | ((UINT32)1U << (i - 1U));
                        } else {
                            SubChipID = ChipID;
                        }
                    }

                    if ((SubSensorId[i] & (UINT16)0x1U) != 0U) {
                        (void) AmbaB8_PrescalerConfig(SubChipID, B8_VIN_CHANNEL0, &PrescalerConfig);

                        if (pInputInfo->VinInterface == B8_VIN_MIPI) {
                            (void) AmbaB8_VinConfigMIPI(SubChipID, B8_VIN_CHANNEL0, pVinConfigMipi);

                        } else if (pInputInfo->VinInterface == B8_VIN_LVDS) {
                            (void) AmbaB8_VinConfigSLVS(SubChipID, B8_VIN_CHANNEL0, pVinConfigSlvs);

                        } else if (pInputInfo->VinInterface == B8_VIN_LVCMOS) {
                            (void) AmbaB8_VinConfigDVP(SubChipID, B8_VIN_CHANNEL0, pVinConfigDvp);
                        } else {
                            RetVal = B8_ERR_ARG;
                        }

                        /* Capture Control */
                        (void) AmbaB8_VinCaptureConfig(SubChipID, B8_VIN_CHANNEL0, &pInputInfo->VinCropWindow);
                    }
                    if ((SubSensorId[i] & (UINT16)0x2U) != 0U) {
                        (void) AmbaB8_PrescalerConfig(SubChipID, B8_VIN_CHANNEL1, &PrescalerConfig);

                        if (pInputInfo->VinInterface == B8_VIN_MIPI) {
                            (void) AmbaB8_VinConfigMIPI(SubChipID, B8_VIN_CHANNEL1, pVinConfigMipi);

                        } else if (pInputInfo->VinInterface == B8_VIN_LVDS) {
                            (void) AmbaB8_VinConfigSLVS(SubChipID, B8_VIN_CHANNEL1, pVinConfigSlvs);

                        } else if (pInputInfo->VinInterface == B8_VIN_LVCMOS) {
                            (void) AmbaB8_VinConfigDVP(SubChipID, B8_VIN_CHANNEL1, pVinConfigDvp);
                        } else {
                            RetVal = B8_ERR_ARG;
                        }

                        /* Capture Control */
                        (void) AmbaB8_VinCaptureConfig(SubChipID, B8_VIN_CHANNEL1, &pInputInfo->VinCropWindow);
                    }
                }
            }

            /* power down SERDES for near-end only case */
            if ((pB8Config->InputInfo.SensorID & \
                 (B8_SENSOR_ID_B8F0_VIN | B8_SENSOR_ID_B8F0_PIP | B8_SENSOR_ID_B8F1_VIN | B8_SENSOR_ID_B8F1_PIP)) == 0U) {
                AmbaB8_SerdesPowerDown(ChipID & B8_MAIN_CHIP_ID_B8NF_MASK);
            }

        }

#if (B8_DBG_TEMP)
    }
#endif

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_GetLinkStatus
 *
 *  @Description:: Get B8 SERDES Link Status
 *
 *  @Input      ::
 *      ChipID:    B8 chip id
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_GetLinkStatus(UINT32 ChipID)
{
    UINT32 RetVal;
    UINT32 DataBackup;
    UINT32 RxData;
    UINT32 TestData = 0xB815600DU;

    RetVal = AmbaB8_RegRead(ChipID, &pAmbaB8_PllReg->CorePllFraction, 0, B8_DATA_WIDTH_32BIT, 1, &DataBackup);

    if (RetVal == B8_ERR_NONE) {

        (void) AmbaB8_RegWrite(ChipID, &pAmbaB8_PllReg->CorePllFraction, 0, B8_DATA_WIDTH_32BIT, 1, &TestData);
        (void) AmbaB8_RegRead(ChipID, &pAmbaB8_PllReg->CorePllFraction, 0, B8_DATA_WIDTH_32BIT, 1, &RxData);

        if (RxData != TestData) {
            RetVal = B8_ERR_SERDES_LINK;
        }

        (void) AmbaB8_RegWrite(ChipID, &pAmbaB8_PllReg->CorePllFraction, 0, B8_DATA_WIDTH_32BIT, 1, &DataBackup);
        (void) AmbaB8_RegRead(ChipID, &pAmbaB8_PllReg->CorePllFraction, 0, B8_DATA_WIDTH_32BIT, 1, &RxData);

        if (RxData != DataBackup) {
            RetVal = B8_ERR_SERDES_LINK;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_GetCtrlIndex
 *
 *  @Description:: Get Unique Index by B8 Channel ID and SubChip ID.
 *
 *  @Input      ::
 *      ChipID:    B8 chip id
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      UINT32:    B8 Control Index
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_GetCtrlIndex(UINT32 ChipID)
{
    UINT32 ChanID = B8_NUM_CHANNEL;
    UINT32 Index = 0xffffffffU;
    UINT32 i;
    extern UINT32 B8SocVinChanSelect[B8_NUM_SOC_VIN_CHANNEL];
    extern UINT32 B8SocVoutChanSelect[B8_NUM_SOC_VOUT_CHANNEL];

    if ((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK) != 0U) {
        for (i = 0U; i < B8_NUM_SOC_VIN_CHANNEL; i ++) {
            if ((ChipID & ((UINT32)1U << (i + 4U))) != 0U) {
                break;
            }
        }

        if (i < B8_NUM_SOC_VIN_CHANNEL) {
            ChanID = B8SocVinChanSelect[i];
        }

    } else if ((ChipID & B8_MAIN_CHIP_ID_B8ND_MASK) != 0U) {
        for (i = 0U; i < B8_NUM_SOC_VOUT_CHANNEL; i ++) {
            if ((ChipID & ((UINT32)1U << (i + (UINT32)12U))) != 0U) {
                break;
            }
        }

        if (i < B8_NUM_SOC_VOUT_CHANNEL) {
            ChanID = B8SocVoutChanSelect[i];
        }

    } else if ((ChipID & B8_MAIN_CHIP_ID_BUILT_IN_B8NF) != 0U) {
        UINT32 VinID[9U] = {0, 0, 2, 0, 3, 0, 0, 0, 1};

        if ((ChipID & B8_SUB_CHIP_ID_MASK) < 9U) {
            ChanID = B8SocVinChanSelect[(VinID[(ChipID & B8_SUB_CHIP_ID_MASK)])];
        }
    } else if ((ChipID & B8_MAIN_CHIP_ID_BUILT_IN_B8ND) != 0U) {
        if ((ChipID & B8_SUB_CHIP_ID_MASK) == 0x1U) {
            ChanID = B8SocVoutChanSelect[0];

        } else {    /* if ((ChipID & B8_SUB_CHIP_ID_MASK) == 0x2U) { */
            ChanID = B8SocVoutChanSelect[1];
        }
    } else {
        Index = 0xffffffffU;
    }

    if (ChanID < B8_NUM_CHANNEL) {
        if ((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK) != 0U) {
            if ((ChipID & B8_SUB_CHIP_ID_MASK) == 0U) {             /* N */
                Index = ChanID * B8_MAX_NUM_B8_ON_CHAN;

            } else if ((ChipID & B8_SUB_CHIP_ID_MASK) == 0x1U) {     /* F0 */
                Index = (ChanID * B8_MAX_NUM_B8_ON_CHAN) + 1U;

            } else if ((ChipID & B8_SUB_CHIP_ID_MASK) == 0x2U) {     /* F1 */
                Index = (ChanID * B8_MAX_NUM_B8_ON_CHAN) + 2U;
            } else {
                Index = 0xffffffffU;
            }

        } else if ((ChipID & B8_MAIN_CHIP_ID_B8ND_MASK) != 0U) {
            if ((ChipID & B8_SUB_CHIP_ID_MASK) == 0U) {             /* N */
                Index = ChanID * B8_MAX_NUM_B8_ON_CHAN;

            } else if ((ChipID & B8_SUB_CHIP_ID_MASK) == 0x1U) {     /* D0 */
                Index = (ChanID * B8_MAX_NUM_B8_ON_CHAN) + 3U;
            } else {
                Index = 0xffffffffU;
            }

        } else if ((ChipID & B8_MAIN_CHIP_ID_BUILT_IN_B8NF) != 0U) {/* F0 */
            Index = (ChanID * B8_MAX_NUM_B8_ON_CHAN) + 1U;

        } else if ((ChipID & B8_MAIN_CHIP_ID_BUILT_IN_B8ND) != 0U) {/* D0 */
            Index = (ChanID * B8_MAX_NUM_B8_ON_CHAN) + 3U;
        } else {
            Index = 0xffffffffU;
        }
    }

    if (Index == 0xffffffffU) {
        AmbaB8_PrintUInt5("AmbaB8_GetCtrlIndex err @ ChipID:0x%x", ChipID, 0U, 0U, 0U, 0U);
    }

    return Index;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_GetSubChipCount
 *
 *  @Description:: Get number of SubChip of input ChipID
 *
 *  @Input      ::
 *      ChipID:    B8 chip id
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      UINT32:    Number of B8 SubChip
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_GetSubChipCount(UINT32 ChipID)
{
    UINT32 Count = 0U;
    UINT32 i;

    for (i = 0U; i < B8_MAX_NUM_SUBCHIP; i ++) {
        if ((ChipID & ((UINT32)1U << i)) != 0U) {
            Count ++;
        }
    }

    return Count;
}

