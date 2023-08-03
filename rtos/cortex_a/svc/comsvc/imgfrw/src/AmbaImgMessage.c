/**
 *  @file AmbaImgMessage.c
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
 *  @details Amba Image Message
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"

#include "AmbaVIN_Def.h"

#include "AmbaImgVar.h"
#include "AmbaImgChannel.h"
#include "AmbaImgSensorHAL.h"

#include "AmbaImgMessage.h"
#include "AmbaImgMessage_Internal.h"

#define OK_UL  ((UINT32) 0U)
#define NG_UL  ((UINT32) 1U)

/**
 *  Amba image message init
 *  @return error code
 */
UINT32 AmbaImgMessage_Init(void)
{
    UINT32 RetCode = OK_UL;
    return RetCode;
}

/**
 *  Amba image message put
 *  @param[in] ImageChanId image channel id
 *  @param[in] MsgId message id
 *  @param[in] User user data
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMessage_Put(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 MsgId, UINT64 User)
{
    static const char * const AmbaImgMessage_IdString[AMBA_IMG_MESSAGE_ID_TOTAL][2] = {
        { "Svr_Chg",  "Svr_Chg!" }
    };

    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;
    UINT32 ChainId;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    if ((AmbaImgMessage_Chan[VinId] != NULL) &&
        (AmbaImgMessage_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        if (AmbaImgMessage_Chan[VinId][ChainId].pFunc != NULL) {
            /* callbcak */
            FuncRetCode = AmbaImgMessage_Chan[VinId][ChainId].pFunc(ImageChanId, MsgId, User);
            if (FuncRetCode == OK_UL) {
                /* sucess */
                AmbaImgSensorHAL_TimingMarkPut(VinId, AmbaImgMessage_IdString[(UINT8) (MsgId & 0xFFULL)][0]);
            } else {
                /* fail */
                AmbaImgSensorHAL_TimingMarkPut(VinId, AmbaImgMessage_IdString[(UINT8) (MsgId & 0xFFULL)][1]);
                RetCode = NG_UL;
            }
        } else {
            /* cb null */
            RetCode = NG_UL;
        }
    } else {
        /* chan null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}
