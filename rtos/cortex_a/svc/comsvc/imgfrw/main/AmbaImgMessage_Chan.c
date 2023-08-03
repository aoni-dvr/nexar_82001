/**
 *  @file AmbaImgMessage_Chan.c
 *
 *  Copyright (c) [2020] Ambarella International LP
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
 *  This file includes sample code and is only for internal testing and evaluation.  If you
 *  distribute this sample code (whether in source, object, or binary code form), it will be
 *  without any warranty or indemnity protection from Ambarella International LP or its affiliates.
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
 *  @details Amba Image Message Channel
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"

#include "AmbaVIN_Def.h"

#include "AmbaDSP.h"
#include "AmbaDSP_ImageDefine.h"
#include "AmbaDSP_ImageFilter.h"
#ifdef CONFIG_BUILD_IMGFRW_SMC
#include "AmbaI2S.h"
#endif
#include "AmbaImgFramework.h"
#ifdef CONFIG_BUILD_IMGFRW_SMC
#include "AmbaImgMain.h"
#include "AmbaImgMain_Internal.h"
#endif
#include "AmbaImgMessage_Chan.h"

#define OK_UL    ((UINT32) 0U)
#define NG_UL    ((UINT32) 1U)

/**
 *  @private
 *  Amba image message channel function
 *  @param[in] ImageChanId image channel id
 *  @param[in] MsgId message id
 *  @param[in] User user data
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMessage_ChanFunc(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 MsgId, UINT64 User)
{
    UINT32 RetCode = OK_UL;
#ifdef CONFIG_BUILD_IMGFRW_SMC
    UINT32 FuncRetCode;
#endif
    UINT32 VinId;
    UINT32 ChainId;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    switch ((UINT8) (MsgId & 0xFFULL)) {
        case (UINT8) AMBA_IMG_MESSAGE_ID_SVR_CHG:
            /* svr chg */
            AmbaImgSensorHAL_TimingMarkPut(VinId, "Msg_Svr");
            if (AmbaImgSensorHAL_Info[VinId][ChainId].Op.User.ForceSsInVideo > 0U) {
                /* smc */
#ifdef CONFIG_BUILD_IMGFRW_SMC
                UINT32 I2sPortId;
                UINT32 I2sClkDiv;
                UINT32 I2sClkScale;
                UINT32 SmcFlag;

                I2sPortId = (AmbaImgMain_ImgSmcI2sId[VinId][ChainId] >> 16U) & 0xFU;
                if ((AmbaImgSensorHAL_Info[VinId][ChainId].Op.User.ForceSsInVideo & 0xFFU) == AmbaImgSensorHAL_Info[VinId][ChainId].pAux->Ctx.Bits.MaxSlowShutterIndex) {
                    /* smc off */
                    SmcFlag = 0U;
                    I2sClkDiv = AmbaImgMain_ImgSmcI2sId[VinId][ChainId] & 0xFU;
                    I2sClkScale = (AmbaImgMain_ImgSmcI2sId[VinId][ChainId] >> 4U) & 0xFU;
                } else {
                    /* smc on */
                    SmcFlag = 1U;
                    I2sClkDiv = (AmbaImgMain_ImgSmcI2sId[VinId][ChainId] >> 8U) & 0xFU;
                    I2sClkScale = (AmbaImgMain_ImgSmcI2sId[VinId][ChainId] >> 12U) & 0xFU;
                }
                FuncRetCode = AmbaI2S_SetClkDiv(I2sPortId, I2sClkDiv, I2sClkScale);
                if (FuncRetCode != I2S_ERR_NONE) {
                    /* */
                }
                if (SmcFlag == 0U) {
                    AmbaImgPrint2(PRINT_FLAG_DBG, "smc off", ImageChanId.Ctx.Bits.VinId, ImageChanId.Ctx.Bits.ChainId);
                } else {
                    AmbaImgPrint2(PRINT_FLAG_DBG, "smc on", ImageChanId.Ctx.Bits.VinId, ImageChanId.Ctx.Bits.ChainId);
                }
                AmbaImgPrint2(PRINT_FLAG_DBG, "svr", AmbaImgSensorHAL_Info[VinId][ChainId].pAux->Ctx.Bits.MaxSlowShutterIndex, AmbaImgSensorHAL_Info[VinId][ChainId].Op.Status.LastSsIndex);
                AmbaImgPrint2(PRINT_FLAG_DBG, "i2s clk div", I2sPortId, I2sClkDiv);
                AmbaImgPrint2(PRINT_FLAG_DBG, "i2s clk scale", I2sPortId, I2sClkScale);
#endif
            }
            break;
        default:
            /* unknown */
            AmbaImgSensorHAL_TimingMarkPut(VinId, "Msg_Null");
#if 1
            if (AmbaImgSensorHAL_TimingMarkChk(VinId) == OK_UL) {
                char str[11];
                str[0] = 'm';str[1] = ' ';
                var_utoa((UINT32) (MsgId & 0xFFULL), &(str[2]), 16U, 8U, (UINT32) VAR_LEADING_SPACE);
                AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                str[0] = 'u';str[1] = ' ';
                var_utoa((UINT32) (User & 0xFFFFFFFFULL), &(str[2]), 16U, 8U, (UINT32) VAR_LEADING_SPACE);
                AmbaImgSensorHAL_TimingMarkPut(VinId, str);
            }
#endif
            RetCode = NG_UL;
            break;
    }

    return RetCode;
}
