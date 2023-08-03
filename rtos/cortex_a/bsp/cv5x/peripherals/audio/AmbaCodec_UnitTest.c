/**
 *  @file AmbaSensor_UnitTest.c
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
 *  @details Sensor unit test implementation
 *
 */

#include "AmbaTypes.h"
#include "AmbaAudio_CODEC.h"
#include "AmbaI2S.h"
#include "AmbaGPIO.h"

#include <AmbaShell.h>
#include "AmbaUtility.h"
#ifdef CONFIG_AUDIO_AK4951EN
#include "AmbaAudio_AK4951EN.h"
#endif
#ifdef CONFIG_AUDIO_TITLV320AIC3007
#include "AmbaAudio_TLV320AIC3007.h"
#endif
#ifdef CONFIG_AUDIO_NAU8822
#include "AmbaAudio_NAU8822.h"
#endif

void AmbaShell_CommandAuCodec(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);

static void AmbaAudioCodecUT_CmdUsage(char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("Usage: ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" init [index] - init [index]th audio codec register\n");
    PrintFunc(pArgVector[0]);
    PrintFunc(" read [index] [addr] [number] - read [index]th audio codec register\n");
    PrintFunc(pArgVector[0]);
    PrintFunc(" write [index] [addr] [value] - write [index]th audio codec register\n");
    PrintFunc(pArgVector[0]);
    PrintFunc(" setinput [index] [mode] - set [index]th audio codec input path\n");
    PrintFunc(pArgVector[0]);
    PrintFunc(" setoutput [index] [mode] - set [index]th audio codec output path\n");
}

static void AmbaAudioCodecUT_PrintUInt32(UINT32 Value, AMBA_SHELL_PRINT_f PrintFunc)
{
    char IntString[UTIL_MAX_INT_STR_LEN];
    (void)AmbaUtility_UInt32ToStr(IntString, UTIL_MAX_INT_STR_LEN, Value, 10);
    PrintFunc(IntString);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaAudioCodecUT_TestCmd
 *
 *  @Description:: audio codec test commands
 *
 *  @Input      ::
 *      ArgCount:   Argument count
 *      pArgVector: Argument vector
 *      PrintFunc:  function to print messages on shell task
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
void AmbaShell_CommandAuCodec(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 Addr = 0U, Data = 0U, i = 0U;
    UINT32 Index, Num, Mode, RtVal;
#ifdef CONFIG_AUDIO_AK4951EN
    AMBA_AUDIO_CODEC_CTRL_s CodecCtrl;
#endif
#ifdef CONFIG_AUDIO_TITLV320AIC3007
    AMBA_AUDIO_CODEC_CTRL_s ExtCodecCtrl;
#endif
#ifdef CONFIG_AUDIO_NAU8822
    AMBA_AUDIO_CODEC_CTRL_s ExtCodecCtrl;
#endif
    AMBA_I2S_CTRL_s Ctrl;

    if (ArgCount >= 3U) {
        (void)AmbaUtility_StringToUInt32(pArgVector[2], &Index);
    } else {
        Index = 0xFFFFUL;
    }
    if (ArgCount == 1U) {
        AmbaAudioCodecUT_CmdUsage(pArgVector, PrintFunc);
    } else if (AmbaUtility_StringCompare(pArgVector[1], "init", 4) == 0) {
        if (ArgCount == 3U) {
            if (Index == 0U) {
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
                (void)AmbaI2S_Config(0, &Ctrl);
                (void)AmbaI2S_Config(1, &Ctrl);

                (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_15_I2S0_CLK);
                (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_16_I2S0_SI_0);
                (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_17_I2S0_SO_0);
                (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_18_I2S0_WS);
                (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_119_I2S1_CLK);
                (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_121_I2S1_SI_0);
                (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_120_I2S1_SO_0);
                (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_122_I2S1_WS);
                //(void)AmbaGPIO_SetFuncAlt(GPIO_PIN_CLK_AU3);
                (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_107_I2C5_CLK);
                (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_108_I2C5_DATA);
#ifdef CONFIG_AUDIO_AK4951EN
                CodecCtrl.CtrlMode = AUCODEC_I2C_CONTROL;
                CodecCtrl.I2cCtrl.I2cChanNo = AMBA_I2C_CHANNEL5;
                CodecCtrl.I2cCtrl.SlaveAddr = AK4951_I2C_ADDR;
                CodecCtrl.I2cCtrl.I2cSpeed = AMBA_I2C_SPEED_FAST;
                if (AmbaAudio_CodecHook(AMBA_AUDIO_CODEC_0, &AmbaAudio_AK4951Obj, &CodecCtrl) != AUCODEC_ERR_NONE) {
                    PrintFunc("AUCODEC: Hook AK4951 failed\n");
                }
                if (AmbaAudio_CodecInit(AMBA_AUDIO_CODEC_0) != AUCODEC_ERR_NONE) {
                    PrintFunc("AUCODEC: Init AK4951 failed\n");
                }
                if (AmbaAudio_CodecModeConfig(AMBA_AUDIO_CODEC_0, AUCODEC_I2S)  != AUCODEC_ERR_NONE) {
                    PrintFunc("AUCODEC: Mode config AK4951 failed\n");
                }
                if (AmbaAudio_CodecFreqConfig(AMBA_AUDIO_CODEC_0, 48000) != AUCODEC_ERR_NONE) {
                    PrintFunc("AUCODEC: Freq config AK4951 failed\n");
                }
                if (AmbaAudio_CodecSetInput(AMBA_AUDIO_CODEC_0, AUCODEC_AMIC2_IN) != AUCODEC_ERR_NONE) {
                    PrintFunc("AUCODEC: Set AK4951 input path failed\n");
                }
                if (AmbaAudio_CodecSetOutput(AMBA_AUDIO_CODEC_0, AUCODEC_HEADPHONE_OUT) != AUCODEC_ERR_NONE) {
                    PrintFunc("AUCODEC: Set AK4951 output path failed\n");
                }
                if (AmbaAudio_CodecSetMute(AMBA_AUDIO_CODEC_0, 0) != AUCODEC_ERR_NONE) {
                    PrintFunc("AUCODEC: Unmute AK4951 failed\n");
                }
#else
                PrintFunc("AUCODEC: Please enable AK4951\n");
#endif
                PrintFunc("AUCODEC: init index 0: AK4951\n");
            } else if (Index == 1U) {
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
                (void)AmbaI2S_Config(0, &Ctrl);

                (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_15_I2S0_CLK);
                (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_16_I2S0_SI_0);
                (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_17_I2S0_SO_0);
                (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_18_I2S0_WS);
                //(void)AmbaGPIO_SetFuncAlt(GPIO_PIN_CLK_AU3);
                (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_107_I2C5_CLK);
                (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_108_I2C5_DATA);
#ifdef CONFIG_AUDIO_TITLV320AIC3007
                (void)AmbaI2S_Config(1, &Ctrl);
                (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_119_I2S1_CLK);
                (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_121_I2S1_SI_0);
                (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_120_I2S1_SO_0);
                (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_122_I2S1_WS);

                ExtCodecCtrl.CtrlMode = AUCODEC_I2C_CONTROL;
                ExtCodecCtrl.I2cCtrl.I2cChanNo = AMBA_I2C_CHANNEL5;
                ExtCodecCtrl.I2cCtrl.SlaveAddr = (TLV320AIC3007_I2C_ADDR << 1);
                ExtCodecCtrl.I2cCtrl.I2cSpeed = AMBA_I2C_SPEED_FAST;
                if (AmbaAudio_CodecHook(AMBA_AUDIO_CODEC_1, &AmbaAudio_Tlv320Aic3007Obj, &ExtCodecCtrl) != AUCODEC_ERR_NONE) {
                    PrintFunc("AUCODEC: Hook Tlv320Aic3007 failed\n");
                }

                //(void)AmbaGPIO_SetFuncGPO(GPIO_PIN_20, AMBA_GPIO_LEVEL_LOW);
                //(void)AmbaKAL_TaskSleep(1);
                //(void)AmbaGPIO_SetFuncGPO(GPIO_PIN_20, AMBA_GPIO_LEVEL_HIGH);
                //(void)AmbaKAL_TaskSleep(1);

                if (AmbaAudio_CodecInit(AMBA_AUDIO_CODEC_1) != AUCODEC_ERR_NONE) {
                    PrintFunc("AUCODEC: Init Tlv320Aic3007 failed\n");
                }
                if (AmbaAudio_CodecModeConfig(AMBA_AUDIO_CODEC_1, AUCODEC_I2S) != AUCODEC_ERR_NONE) {
                    PrintFunc("AUCODEC: Mode config Tlv320Aic3007 failed\n");
                }
                if (AmbaAudio_CodecFreqConfig(AMBA_AUDIO_CODEC_1, 48000) != AUCODEC_ERR_NONE) {
                    PrintFunc("AUCODEC: Freq config Tlv320Aic3007 failed\n");
                }
                if (AmbaAudio_CodecSetInput(AMBA_AUDIO_CODEC_1, AUCODEC_LINE_IN) != AUCODEC_ERR_NONE) {
                    PrintFunc("AUCODEC: Set Tlv320Aic3007 input path failed\n");
                }
                if (AmbaAudio_CodecSetOutput(AMBA_AUDIO_CODEC_1, AUCODEC_LINE_OUT) != AUCODEC_ERR_NONE) {
                    PrintFunc("AUCODEC: Set Tlv320Aic3007 output path failed\n");
                }
                if (AmbaAudio_CodecSetMute(AMBA_AUDIO_CODEC_1, 0) != AUCODEC_ERR_NONE) {
                    PrintFunc("AUCODEC: Unmute Tlv320Aic3007 failed\n");
                }
#endif
#ifdef CONFIG_AUDIO_NAU8822
                ExtCodecCtrl.CtrlMode = AUCODEC_I2C_CONTROL;
#ifdef CONFIG_SOC_CV52
                ExtCodecCtrl.I2cCtrl.I2cChanNo = AMBA_I2C_CHANNEL3;
#else
                ExtCodecCtrl.I2cCtrl.I2cChanNo = AMBA_I2C_CHANNEL2;
#endif
                ExtCodecCtrl.I2cCtrl.SlaveAddr = NAU8822_I2C_ADDR;
                ExtCodecCtrl.I2cCtrl.I2cSpeed = AMBA_I2C_SPEED_STANDARD;
                if (AmbaAudio_CodecHook(AMBA_AUDIO_CODEC_1, &AmbaAudio_NAU8822Obj, &ExtCodecCtrl) != AUCODEC_ERR_NONE) {
                    PrintFunc("AUCODEC: Hook NAU8822 failed\n");
                }
                if (AmbaAudio_CodecInit(AMBA_AUDIO_CODEC_1) != AUCODEC_ERR_NONE) {
                    PrintFunc("AUCODEC: Init NAU8822 failed\n");
                }
                if (AmbaAudio_CodecModeConfig(AMBA_AUDIO_CODEC_1, AUCODEC_I2S) != AUCODEC_ERR_NONE) {
                    PrintFunc("AUCODEC: Mode config NAU8822 failed\n");
                }
                if (AmbaAudio_CodecFreqConfig(AMBA_AUDIO_CODEC_1, 48000) != AUCODEC_ERR_NONE) {
                    PrintFunc("AUCODEC: Freq config NAU8822 failed\n");
                }
                if (AmbaAudio_CodecSetInput(AMBA_AUDIO_CODEC_1, AUCODEC_AMIC_IN) != AUCODEC_ERR_NONE) {
                    PrintFunc("AUCODEC: Set NAU8822 input path failed\n");
                }
                if (AmbaAudio_CodecSetOutput(AMBA_AUDIO_CODEC_1, AUCODEC_SPEAKER_OUT) != AUCODEC_ERR_NONE) {
                    PrintFunc("AUCODEC: Set NAU8822 output path failed\n");
                }
                if (AmbaAudio_CodecSetMute(AMBA_AUDIO_CODEC_1, 0) != AUCODEC_ERR_NONE) {
                    PrintFunc("AUCODEC: Unmute NAU8822 failed\n");
                }
#endif
                PrintFunc("AUCODEC: init index 1 success\n");
            } else {
                PrintFunc("AUCODEC: invalid init index\n");
            }
        } else {
            AmbaAudioCodecUT_CmdUsage(pArgVector, PrintFunc);
        }
    } else if (AmbaUtility_StringCompare(pArgVector[1], "read", 4) == 0) {
        if ((ArgCount == 5U) && (Index < AMBA_NUM_AUDIO_CODEC)) {
            (void)AmbaUtility_StringToUInt32(pArgVector[3], &Addr);
            (void)AmbaUtility_StringToUInt32(pArgVector[4], &Num);
            if (Num != 0U) {
                for (i = Addr; i < (Addr + Num); i++) {
                    RtVal = AmbaAudio_CodecRead(Index, i, &Data);
                    if (RtVal == AUCODEC_ERR_NONE) {
                        PrintFunc("AUCODEC READ: Idx: ");
                        AmbaAudioCodecUT_PrintUInt32(Index, PrintFunc);
                        PrintFunc(", Addr: ");
                        AmbaAudioCodecUT_PrintUInt32(i, PrintFunc);
                        PrintFunc(", Data: ");
                        AmbaAudioCodecUT_PrintUInt32(Data, PrintFunc);
                        PrintFunc("\n");
                    } else {
                        PrintFunc("AUCODEC Read failed: ");
                        AmbaAudioCodecUT_PrintUInt32(RtVal, PrintFunc);
                        PrintFunc("\n");
                    }
                }
            }
        } else {
            AmbaAudioCodecUT_CmdUsage(pArgVector, PrintFunc);
        }
    } else if (AmbaUtility_StringCompare(pArgVector[1], "write", 5) == 0) {
        if ((ArgCount == 5U) && (Index < AMBA_NUM_AUDIO_CODEC)) {
            (void)AmbaUtility_StringToUInt32(pArgVector[3], &Addr);
            (void)AmbaUtility_StringToUInt32(pArgVector[4], &Data);
            if (AmbaAudio_CodecWrite(Index, Addr, Data) == AUCODEC_ERR_NONE) {
                PrintFunc("AUCODEC WRITE: Idx: ");
                AmbaAudioCodecUT_PrintUInt32(Index, PrintFunc);
                PrintFunc(", Addr: ");
                AmbaAudioCodecUT_PrintUInt32(Addr, PrintFunc);
                PrintFunc(", Data: ");
                AmbaAudioCodecUT_PrintUInt32(Data, PrintFunc);
                PrintFunc("\n");
            } else {
                PrintFunc("AUCODEC Write failed\n");
            }
        } else {
            AmbaAudioCodecUT_CmdUsage(pArgVector, PrintFunc);
        }
    } else if (AmbaUtility_StringCompare(pArgVector[1], "setinput", 8) == 0) {
        if ((ArgCount == 4U) && (Index < AMBA_NUM_AUDIO_CODEC)) {
            (void)AmbaUtility_StringToUInt32(pArgVector[3], &Mode);
            if (AmbaAudio_CodecSetInput(Index, Mode) == AUCODEC_ERR_NONE) {
                PrintFunc("AUCODEC SET INPUT: ");
                AmbaAudioCodecUT_PrintUInt32(Index, PrintFunc);
                PrintFunc(", Mode: ");
                AmbaAudioCodecUT_PrintUInt32(Mode, PrintFunc);
                PrintFunc("\n");
            } else {
                PrintFunc("AUCODEC Set input path failed\n");
            }
        } else {
            AmbaAudioCodecUT_CmdUsage(pArgVector, PrintFunc);
        }
    } else if (AmbaUtility_StringCompare(pArgVector[1], "setoutput", 9) == 0) {
        if ((ArgCount == 4U) && (Index < AMBA_NUM_AUDIO_CODEC)) {
            (void)AmbaUtility_StringToUInt32(pArgVector[3], &Mode);
            if (AmbaAudio_CodecSetOutput(Index, Mode) == AUCODEC_ERR_NONE) {
                PrintFunc("AUCODEC SET OUTPUT: ");
                AmbaAudioCodecUT_PrintUInt32(Index, PrintFunc);
                PrintFunc(", Mode: ");
                AmbaAudioCodecUT_PrintUInt32(Mode, PrintFunc);
                PrintFunc("\n");
            } else {
                PrintFunc("AUCODEC Set output path failed\n");
            }
        } else {
            AmbaAudioCodecUT_CmdUsage(pArgVector, PrintFunc);
        }
    } else {
        AmbaAudioCodecUT_CmdUsage(pArgVector, PrintFunc);
    }
}
