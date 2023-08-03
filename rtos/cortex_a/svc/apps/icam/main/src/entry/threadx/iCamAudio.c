/**
*  @file iCamAudio.c
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
*  @details icam audio drv functions
*
*/

#include ".svc_autogen"

// #include "AmbaKAL.h"
#include "AmbaGPIO_Def.h"
#include "AmbaGPIO.h"
#include "AmbaAudio_CODEC.h"
#include "AmbaI2S.h"
#if defined(CONFIG_BSP_H32_NEXAR_D081)
#include "AmbaAudio_MAX98090.h"
#elif defined(CONFIG_BSP_CV25_NEXAR_D080)
#include "AmbaAudio_ES7210_ES8516.h"
#endif
#if defined(GPIO_PIN_DMIC_CLK)
#include "AmbaRTSL_PLL_Def.h"
#include "AmbaRTSL_PLL.h"
#include "AmbaDMIC.h"
#endif

#include "AmbaSvcWrap.h"
#include "SvcLog.h"
#include "SvcPlat.h"
#include "iCamAudio.h"

#define SVC_LOG_AUDM        "AUDM"

static UINT32 ACodecBits = 0U;

/**
 *  Initialization of audio driver
 *  @param[in] I2sCh I2S channel
 *  @return none
 */
#if 0
static UINT32 NAU8822_Check(const AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 Addr, UINT32 *pRxData)
{
    AMBA_I2C_TRANSACTION_s I2cTxConfig = {0};
    AMBA_I2C_TRANSACTION_s I2cRxConfig = {0};

    UINT8 TxData;
    UINT8 Temp;
    UINT32 TxSize, AddrTmp;
    UINT32 RetVal = AUCODEC_ERR_NONE;

    I2cTxConfig.SlaveAddr = pCodecCtrl->I2cCtrl.SlaveAddr;
    I2cTxConfig.DataSize = 1U;
    I2cTxConfig.pDataBuf = &TxData;

    AddrTmp = (Addr << 1U);
    TxData = (UINT8)AddrTmp;

    I2cRxConfig.SlaveAddr = pCodecCtrl->I2cCtrl.SlaveAddr | 1U;
    I2cRxConfig.DataSize  = 2U;
    AmbaMisra_TypeCast(&(I2cRxConfig.pDataBuf), &pRxData);

    RetVal = AmbaI2C_MasterReadAfterWrite(pCodecCtrl->I2cCtrl.I2cChanNo, pCodecCtrl->I2cCtrl.I2cSpeed, 1U, &I2cTxConfig, &I2cRxConfig, &TxSize, AMBA_KAL_WAIT_FOREVER);
    Temp = I2cRxConfig.pDataBuf[0];
    I2cRxConfig.pDataBuf[0] = I2cRxConfig.pDataBuf[1];
    I2cRxConfig.pDataBuf[1] = Temp;

    return RetVal;
}

static void iCamAudio_MISCBoard_CHECK(AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, const AMBA_AUDIO_CODEC_OBJ_s *pCodecDrv)
{
   // Check MISC board is V110 or V100 version
    extern AMBA_AUDIO_CODEC_OBJ_s AmbaAudio_NAU8822Obj GNU_WEAK_SYMBOL;
    const AMBA_AUDIO_CODEC_OBJ_s * pAmbaAudio_NAU8822O = &AmbaAudio_NAU8822Obj; // If non-define NAU8822, pointer will be NULL
    UINT32 Addr = 0X3FU, RxData = 0xFFU;
    UINT32 TmpSlaveAddr = pCodecCtrl->I2cCtrl.SlaveAddr;
    UINT32 Is_changed = 0U;

    AmbaMisra_TouchUnused(&pAmbaAudio_NAU8822O);
    if (pAmbaAudio_NAU8822O == NULL) {
        SvcLog_OK(SVC_LOG_AUDM, " No support NAU8822", 0U, 0U);
    } else {
        if (pCodecDrv == pAmbaAudio_NAU8822O) {
            (void)NAU8822_Check(pCodecCtrl, Addr, &RxData);
            if ((RxData == 0X1DU) || (RxData == 0X1AU)) {
                SvcLog_OK(SVC_LOG_AUDM, " Audio slave addr (%d)", pCodecCtrl->I2cCtrl.SlaveAddr, 0U);
            } else {
                RxData = 0xFFU;
                if (pCodecCtrl->I2cCtrl.SlaveAddr == NAU8822AYG_I2C_ADDR) {
                    pCodecCtrl->I2cCtrl.SlaveAddr = NAU8822_I2C_ADDR;
                    (void)NAU8822_Check(pCodecCtrl, Addr, &RxData);
                    if (RxData == 0X1AU) { // V110 misc board
                        Is_changed = 1U;
                        SvcLog_OK(SVC_LOG_AUDM, "Change Audio slave addr (%d)", pCodecCtrl->I2cCtrl.SlaveAddr, 0U);
                    }
                } else if (pCodecCtrl->I2cCtrl.SlaveAddr == NAU8822_I2C_ADDR) {
                    pCodecCtrl->I2cCtrl.SlaveAddr = NAU8822AYG_I2C_ADDR;
                    (void)NAU8822_Check(pCodecCtrl, Addr, &RxData);
                    if (RxData == 0X1DU) {  // V100 misc board
                        Is_changed = 1U;
                        SvcLog_OK(SVC_LOG_AUDM, "Change Audio slave addr (%d)", pCodecCtrl->I2cCtrl.SlaveAddr, 0U);
                    }
                } else {
                    // do nothing
                }
                if (Is_changed == 0U) {
                    pCodecCtrl->I2cCtrl.SlaveAddr = TmpSlaveAddr;
                    SvcLog_OK(SVC_LOG_AUDM, "Non-detect Audio SlaveAddr(%d) and SlaveAddr (%d)", NAU8822_I2C_ADDR, NAU8822AYG_I2C_ADDR);
                }
            }
        }
    }

}
#endif
void iCamAudio_DrvInit(UINT32 I2sCh, UINT32 EnableDMIC)
{
    UINT32                        I2cCh = 0U, I2cAddr = 0U, I2cSpeed = AMBA_I2C_SPEED_FAST;
    UINT32                        CodecCh = 0U, CodecIn = 0U, CodecOut = 0U;
    AMBA_I2S_CTRL_s               Ctrl;
    AMBA_AUDIO_CODEC_CTRL_s       CodecCtrl;
    const AMBA_AUDIO_CODEC_OBJ_s  *pCodecDrv = NULL;

    Ctrl.ChannelNum = AMBA_I2S_AUDIO_CHANNELS_2;
    Ctrl.ClkDirection = AMBA_I2S_MASTER;
    Ctrl.ClkDivider = 1;
    Ctrl.DspModeSlots = 0;
    Ctrl.Echo = 0;
    Ctrl.Mode = AMBA_I2S_MODE_I2S;
    Ctrl.RxCtrl.Loopback = 0;
    Ctrl.RxCtrl.Order = AMBA_I2S_MSB_FIRST;
    Ctrl.RxCtrl.Rsp = AMBA_I2S_CLK_EDGE_RISING;
    Ctrl.RxCtrl.Shift = 0;
    Ctrl.TxCtrl.Loopback = 0;
    Ctrl.TxCtrl.Mono = 0;
    Ctrl.TxCtrl.Mute = 0;
    Ctrl.TxCtrl.Order = AMBA_I2S_MSB_FIRST;
    Ctrl.TxCtrl.Shift = 0;
    Ctrl.TxCtrl.Tsp = AMBA_I2S_CLK_EDGE_FALLING;
    Ctrl.TxCtrl.Unison = 0;
    Ctrl.WordPos = 0;
    Ctrl.WordPrecision = 32;

    if (AmbaI2S_Config(I2sCh, &Ctrl) != OK) {
        SvcLog_NG(SVC_LOG_AUDM, "## fail to configure I2S%d", I2sCh, 0U);
    }

    CodecCh = I2sCh;
    if (I2sCh == 0U) {
#if defined(SOC_I2S0_EXIST)
        if (AmbaGPIO_SetFuncAlt(GPIO_PIN_I2S0_CLK) != OK) {
            SvcLog_NG(SVC_LOG_AUDM, "## fail to configure GPIO I2S0_CLK", 0U, 0U);
        }

        if (AmbaGPIO_SetFuncAlt(GPIO_PIN_I2S0_SI) != OK) {
            SvcLog_NG(SVC_LOG_AUDM, "## fail to configure GPIO I2S0_SI", 0U, 0U);
        }

        if (AmbaGPIO_SetFuncAlt(GPIO_PIN_I2S0_SO) != OK) {
            SvcLog_NG(SVC_LOG_AUDM, "## fail to configure GPIO I2S0_SO", 0U, 0U);
        }

        if (AmbaGPIO_SetFuncAlt(GPIO_PIN_I2S0_WS) != OK) {
            SvcLog_NG(SVC_LOG_AUDM, "## fail to configure GPIO I2S0_WS", 0U, 0U);
        }
        SvcLog_DBG(SVC_LOG_AUDM, "I2S0 is configured", 0U, 0U);
#if defined(AUD_CODEC0_EXIST)
        I2cCh     = AUD_CODEC0_I2C_CH;
        I2cAddr   = AUD_CODEC0_I2C_ADDR;
        I2cSpeed  = AUD_CODEC0_I2C_SPEED;
        CodecIn   = AUD_CODEC0_IN;
        CodecOut  = AUD_CODEC0_OUT;
        pCodecDrv = AUD_CODEC0_DRV;

        ACodecBits |= 1U;
#endif
#else
        SvcLog_NG(SVC_LOG_AUDM, "I2S0 isn't existing", 0U, 0U);
#endif
    }

    if (I2sCh == 1U) {
#if defined(SOC_I2S1_EXIST)
        if (AmbaGPIO_SetFuncAlt(GPIO_PIN_I2S1_CLK) != OK) {
            SvcLog_NG(SVC_LOG_AUDM, "## fail to configure GPIO I2S1_CLK", 0U, 0U);
        }

        if (AmbaGPIO_SetFuncAlt(GPIO_PIN_I2S1_SI) != OK) {
            SvcLog_NG(SVC_LOG_AUDM, "## fail to configure GPIO I2S1_SI", 0U, 0U);
        }

        if (AmbaGPIO_SetFuncAlt(GPIO_PIN_I2S1_SO) != OK) {
            SvcLog_NG(SVC_LOG_AUDM, "## fail to configure GPIO I2S1_SO", 0U, 0U);
        }

        if (AmbaGPIO_SetFuncAlt(GPIO_PIN_I2S1_WS) != OK) {
            SvcLog_NG(SVC_LOG_AUDM, "## fail to configure GPIO I2S1_WS", 0U, 0U);
        }

        SvcLog_DBG(SVC_LOG_AUDM, "I2S1 is configured", 0U, 0U);
#if defined(AUD_CODEC1_EXIST)
        I2cCh     = AUD_CODEC1_I2C_CH;
        I2cAddr   = AUD_CODEC1_I2C_ADDR;
        I2cSpeed  = AUD_CODEC1_I2C_SPEED;
        CodecIn   = AUD_CODEC1_IN;
        CodecOut  = AUD_CODEC1_OUT;
        pCodecDrv = AUD_CODEC1_DRV;

        ACodecBits |= 2U;
#endif
#else
        SvcLog_NG(SVC_LOG_AUDM, "I2S1 isn't existing", 0U, 0U);
#endif
    }

    if (pCodecDrv != NULL) {
        AmbaSvcWrap_MisraMemset(&CodecCtrl, 0, sizeof(CodecCtrl));
        CodecCtrl.CtrlMode          = AUCODEC_I2C_CONTROL;
        CodecCtrl.I2cCtrl.I2cChanNo = I2cCh;
        CodecCtrl.I2cCtrl.SlaveAddr = I2cAddr;
        CodecCtrl.I2cCtrl.I2cSpeed  = I2cSpeed;

        //iCamAudio_MISCBoard_CHECK(&CodecCtrl, pCodecDrv);

        if (AmbaAudio_CodecHook(CodecCh, pCodecDrv, &CodecCtrl) != OK) {
            SvcLog_NG(SVC_LOG_AUDM, "## fail to hook CodecCh(%d)", CodecCh, 0U);
        }

        if (AmbaAudio_CodecInit(CodecCh) != OK) {
            SvcLog_NG(SVC_LOG_AUDM, "## fail to initialize CodecCh(%d)", CodecCh, 0U);
        }

        if (AmbaAudio_CodecModeConfig(CodecCh, AUCODEC_I2S) != OK) {
            SvcLog_NG(SVC_LOG_AUDM, "## fail to mode config CodecCh(%d)", CodecCh, 0U);
        }

        if (AmbaAudio_CodecFreqConfig(CodecCh, 48000) != OK) {
            SvcLog_NG(SVC_LOG_AUDM, "## fail to freq config CodecCh(%d)", CodecCh, 0U);
        }

        if (AmbaAudio_CodecSetInput(CodecCh, CodecIn) != OK) {
            SvcLog_NG(SVC_LOG_AUDM, "## fail to set input CodecCh(%d)", CodecCh, 0U);
        }

        if (AmbaAudio_CodecSetOutput(CodecCh, CodecOut) != OK) {
            SvcLog_NG(SVC_LOG_AUDM, "## fail to set output CodecCh(%d)", CodecCh, 0U);
        }

        if (AmbaAudio_CodecSetMute(CodecCh, 0) != OK) {
            SvcLog_NG(SVC_LOG_AUDM, "## fail to set mute CodecCh(%d)", CodecCh, 0U);
        }

        SvcLog_DBG(SVC_LOG_AUDM, "AudCodec_%u is configured", CodecCh, 0U);
    } else {
        SvcLog_DBG(SVC_LOG_AUDM, "AudioCodec_%u isn't existing", CodecCh, 0U);
    }

    if (1U == EnableDMIC) {
#if defined(GPIO_PIN_DMIC_CLK)
        if (AmbaGPIO_SetFuncAlt(GPIO_PIN_DMIC_CLK) != OK) {
            SvcLog_NG(SVC_LOG_AUDM, "## fail to configure GPIO DMIC_CLK", 0U, 0U);
        }

        if (AmbaGPIO_SetFuncAlt(GPIO_PIN_DMIC_DATA_IN) != OK) {
            SvcLog_NG(SVC_LOG_AUDM, "## fail to configure GPIO DMIC_DATA_IN", 0U, 0U);
        }
#if defined(GPIO_PIN_DMIC_CLK_AU3)
        if (AmbaGPIO_SetFuncAlt(GPIO_PIN_DMIC_CLK_AU3) != OK) {
            SvcLog_NG(SVC_LOG_AUDM, "## fail to configure GPIO DMIC_CLK_AU3", 0U, 0U);
        }
#endif
        if (AmbaRTSL_PllSetAudioClkConfig(GPIO_PIN_DMIC_PLL_AUDIO_CLK_REF) != OK) {
            SvcLog_NG(SVC_LOG_AUDM, "## fail to set DMIC_PLL_AUDIO_CLK_REF", 0U, 0U);
        }

        if (AmbaRTSL_PllSetAudioClk(12288000 * 4) != OK) {
            /* 49152000 -> 48000 * 1024 */
            SvcLog_NG(SVC_LOG_AUDM, "## fail to set AudioClk", 0U, 0U);
        }

        if (AmbaRTSL_PllSetAudio2Clk(12288000) != OK) {
            SvcLog_NG(SVC_LOG_AUDM, "## fail to set Audio2Clk", 0U, 0U);
        }

        if (AmbaDMIC_DcBlocking(0U) != OK) {
            SvcLog_NG(SVC_LOG_AUDM, "## fail to configure DMIC DcBlocking", 0U, 0U);
        }

        SvcLog_DBG(SVC_LOG_AUDM, "AudCodec DMIC is configured", 0U, 0U);
#endif
    }
}

/**
 *  Input control of audio driver
 *  @param[in] I2sCh I2S channel
 *  @param[in] Op Rx operation
 *  @return none
 */
void iCamAudio_AinCtrl(UINT32 I2sCh, UINT32 Op)
{
    UINT32  Rval;

    switch (Op) {
    case SVC_PLAT_AIN_RESET:
        Rval = AmbaI2S_RxResetFifo(I2sCh);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_AUDM, "AmbaI2S_RxResetFifo error, Rval = %d", Rval, 0U);
        }
        break;
    case SVC_PLAT_AIN_ON:
        Rval = AmbaI2S_RxTrigger(I2sCh, 1);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_AUDM, "AmbaI2S_RxTrigger on error, Rval = %d", Rval, 0U);
        }
        break;
    case SVC_PLAT_AIN_OFF:
        Rval = AmbaI2S_RxTrigger(I2sCh, 0);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_AUDM, "AmbaI2S_RxTrigger off error, Rval = %d", Rval, 0U);
        }
        break;
    default:
        SvcLog_DBG(SVC_LOG_AUDM, "unknown operation, %u", Op, 0U);
        break;
    }
}

/**
 *  Output control of audio driver
 *  @param[in] I2sCh I2S channel
 *  @param[in] Op Tx operation
 *  @return none
 */
void iCamAudio_AoutCtrl(UINT32 I2sCh, UINT32 Op)
{
    UINT32  Rval;

    switch (Op) {
    case SVC_PLAT_AOU_RESET:
        Rval = AmbaI2S_TxResetFifo(I2sCh);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_AUDM, "AmbaI2S_TxResetFifo error, Rval = %d", Rval, 0U);
        }
        break;
    case SVC_PLAT_AOU_ON:
        Rval = AmbaI2S_TxTrigger(I2sCh, 1);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_AUDM, "AmbaI2S_TxTrigger on error, Rval = %d", Rval, 0U);
        }
        break;
    case SVC_PLAT_AOU_OFF:
        Rval = AmbaI2S_TxTrigger(I2sCh, 0);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_AUDM, "AmbaI2S_TxTrigger off error, Rval = %d", Rval, 0U);
        }
        break;
    default:
        SvcLog_DBG(SVC_LOG_AUDM, "unknown operation, %u", Op, 0U);
        break;
    }
}

/**
 *  Misc control of audio driver
 *  @param[in] I2sCh I2S channel
 *  @param[in] Op Misc operation
 *  @return none
 */
void iCamAudio_DrvCtrl(UINT32 I2sCh, UINT32 Op)
{
    UINT32  Rval, Bit = 1U;

    switch (Op) {
    case SVC_PLAT_ADRV_MUTE_ON:
        if ((ACodecBits & (Bit << I2sCh)) > 0U) {
            Rval = AmbaAudio_CodecSetMute(I2sCh, 1U);
            if (Rval != OK) {
                SvcLog_DBG(SVC_LOG_AUDM, "AmbaAudio_CodecSetMute on failed, Rval = %u", Rval, 0U);
            }
        }
        break;
    case SVC_PLAT_ADRV_MUTE_OFF:
        if ((ACodecBits & (Bit << I2sCh)) > 0U) {
            Rval = AmbaAudio_CodecSetMute(I2sCh, 0U);
            if (Rval != OK) {
                SvcLog_DBG(SVC_LOG_AUDM, "AmbaAudio_CodecSetMute off failed, Rval = %u", Rval, 0U);
            }
        }
        break;
    default:
        SvcLog_DBG(SVC_LOG_AUDM, "unknown operation, %u", Op, 0U);
        break;
    }
}
