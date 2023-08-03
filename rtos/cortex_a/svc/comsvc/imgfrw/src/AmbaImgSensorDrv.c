/**
 *  @file AmbaImgSensorDrv.c
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
 *  @details Amba Image Sensor Drv
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"

#include "AmbaVIN_Def.h"

#include "AmbaImgChannel.h"
#include "AmbaImgSensorDrv.h"

#define OK_UL  ((UINT32) 0U)
#define NG_UL  ((UINT32) 1U)

/**
 *  Amba image sensor drv init
 *  @return error code
 */
UINT32 AmbaImgSensorDrv_Init(void)
{
    UINT32 RetCode = OK_UL;
    return RetCode;
}

/**
 *  Amba image sensor drv register write
 *  @param[in] ImageChanId image channel id
 *  @param[in] pBuf pointer to the write buffer
 *  @param[in] Size buffer size
 *  @return error code
 */
UINT32 AmbaImgSensorDrv_RegWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT8 *pBuf, UINT8 Size)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;
    UINT32 ChainId;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    if ((AmbaImgSensorDrv_Chan[VinId] != NULL) &&
        (AmbaImgSensorDrv_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        FuncRetCode = AmbaImgSensorDrv_Chan[VinId][ChainId].pFunc->RegWrite(ImageChanId, pBuf, Size);
        if (FuncRetCode != OK_UL) {
            RetCode = NG_UL;
        }
    } else {
        /* chan null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba image sensor drv analog gain write
 *  @param[in] ImageChanId image channel id
 *  @param[in] pAgc pointer to the analog gain
 *  @return error code
 */
UINT32 AmbaImgSensorDrv_AgcWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT32 *pAgc)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;
    UINT32 ChainId;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    if ((AmbaImgSensorDrv_Chan[VinId] != NULL) &&
        (AmbaImgSensorDrv_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        FuncRetCode = AmbaImgSensorDrv_Chan[VinId][ChainId].pFunc->AgcWrite(ImageChanId, pAgc);
        if (FuncRetCode != OK_UL) {
            RetCode = NG_UL;
        }
    } else {
        /* chan null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba image sensor drv digital gain write
 *  @param[in] ImageChanId image channel id
 *  @param[in] pDgc pointer to the digital gain
 *  @return error code
 */
UINT32 AmbaImgSensorDrv_DgcWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT32 *pDgc)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;
    UINT32 ChainId;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    if ((AmbaImgSensorDrv_Chan[VinId] != NULL) &&
        (AmbaImgSensorDrv_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        FuncRetCode = AmbaImgSensorDrv_Chan[VinId][ChainId].pFunc->DgcWrite(ImageChanId, pDgc);
        if (FuncRetCode != OK_UL) {
            RetCode = NG_UL;
        }
    } else {
        /* chan null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba image sensor drv shutter write
 *  @param[in] ImageChanId image channel id
 *  @param[in] pShr pointer to the shutter
 *  @return error code
 */
UINT32 AmbaImgSensorDrv_ShrWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT32 *pShr)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;
    UINT32 ChainId;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    if ((AmbaImgSensorDrv_Chan[VinId] != NULL) &&
        (AmbaImgSensorDrv_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        FuncRetCode = AmbaImgSensorDrv_Chan[VinId][ChainId].pFunc->ShrWrite(ImageChanId, pShr);
        if (FuncRetCode != OK_UL) {
            RetCode = NG_UL;
        }
    } else {
        /* chan null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba image sensor drv slow shutter write
 *  @param[in] ImageChanId image channel id
 *  @param[in] pSvr pointer to the slow shutter
 *  @return error code
 */
UINT32 AmbaImgSensorDrv_SvrWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT32 *pSvr)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;
    UINT32 ChainId;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    if ((AmbaImgSensorDrv_Chan[VinId] != NULL) &&
        (AmbaImgSensorDrv_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        FuncRetCode = AmbaImgSensorDrv_Chan[VinId][ChainId].pFunc->SvrWrite(ImageChanId, pSvr);
        if (FuncRetCode != OK_UL) {
            RetCode = NG_UL;
        }
    } else {
        /* chan null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba image sensor drv master sync write
 *  @param[in] ImageChanId image channel id
 *  @param[in] pMsc pointer to the master sync
 *  @return error code
 */
UINT32 AmbaImgSensorDrv_MscWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT32 *pMsc)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;
    UINT32 ChainId;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    if ((AmbaImgSensorDrv_Chan[VinId] != NULL) &&
        (AmbaImgSensorDrv_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        FuncRetCode = AmbaImgSensorDrv_Chan[VinId][ChainId].pFunc->MscWrite(ImageChanId, pMsc);
        if (FuncRetCode != OK_UL) {
            RetCode = NG_UL;
        }
    } else {
        /* chan null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba image sensor drv sync light source write
 *  @param[in] ImageChanId image channel id
 *  @param[in] pSls pointer to the sync light source
 *  @return error code
 */
UINT32 AmbaImgSensorDrv_SlsWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT32 *pSls)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;
    UINT32 ChainId;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    if ((AmbaImgSensorDrv_Chan[VinId] != NULL) &&
        (AmbaImgSensorDrv_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        FuncRetCode = AmbaImgSensorDrv_Chan[VinId][ChainId].pFunc->SlsWrite(ImageChanId, pSls);
        if (FuncRetCode != OK_UL) {
            RetCode = NG_UL;
        }
    } else {
        /* chan null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba image sensor drv white balance gain write
 *  @param[in] ImageChanId image channel id
 *  @param[in] pWgc pointer to the white balance gain
 *  @return error code
 */
UINT32 AmbaImgSensorDrv_WgcWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT64 *pWgc)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;
    UINT32 ChainId;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    if ((AmbaImgSensorDrv_Chan[VinId] != NULL) &&
        (AmbaImgSensorDrv_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        FuncRetCode = AmbaImgSensorDrv_Chan[VinId][ChainId].pFunc->WgcWrite(ImageChanId, pWgc);
        if (FuncRetCode != OK_UL) {
            RetCode = NG_UL;
        }
    } else {
        /* chan null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba image sensor drv gain convert
 *  @param[in] ImageChanId image channel id
 *  @param[in] pFactor pointer to the gain factor
 *  @param[out] pAgc pointer to the analog gain
 *  @param[out] pDgc pointer to the digital gain
 *  @param[in,out] pWgc pointer to the white balance gain
 *  @return error code
 */
UINT32 AmbaImgSensorDrv_GainConvert(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT32 *pFactor, UINT32 *pAgc, UINT32 *pDgc, AMBA_IMG_SENSOR_WB_s *pWgc)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;
    UINT32 ChainId;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    if ((AmbaImgSensorDrv_Chan[VinId] != NULL) &&
        (AmbaImgSensorDrv_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        FuncRetCode = AmbaImgSensorDrv_Chan[VinId][ChainId].pFunc->GainConvert(ImageChanId, pFactor, pAgc, pDgc, pWgc);
        if (FuncRetCode != OK_UL) {
            RetCode = NG_UL;
        }
    } else {
        /* chan null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba image sensor drv shutter convert
 *  @param[in] ImageChanId image channel id
 *  @param[in] pFactor pointer to the shuffer factor
 *  @param[out] pShr pointer to the shutter
 *  @return error code
 */
UINT32 AmbaImgSensorDrv_ShutterConvert(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT32 *pFactor, UINT32 *pShr)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;
    UINT32 ChainId;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    if ((AmbaImgSensorDrv_Chan[VinId] != NULL) &&
        (AmbaImgSensorDrv_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        FuncRetCode = AmbaImgSensorDrv_Chan[VinId][ChainId].pFunc->ShutterConvert(ImageChanId, pFactor, pShr);
        if (FuncRetCode != OK_UL) {
            RetCode = NG_UL;
        }
    } else {
        /* chan null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba image sensor drv slow shutter convert
 *  @param[in] ImageChanId image channel id
 *  @param[in] pShr pointer to the shutter
 *  @param[out] pSvr pointer to the slow shutter
 *  @return error code
 */
UINT32 AmbaImgSensorDrv_SvrConvert(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT32 *pShr, UINT32 *pSvr)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;
    UINT32 ChainId;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    if ((AmbaImgSensorDrv_Chan[VinId] != NULL) &&
        (AmbaImgSensorDrv_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        FuncRetCode = AmbaImgSensorDrv_Chan[VinId][ChainId].pFunc->SvrConvert(ImageChanId, pShr, pSvr);
        if (FuncRetCode != OK_UL) {
            RetCode = NG_UL;
        }
    } else {
        /* chan null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba image sensor drv sync light source convert
 *  @param[in] ImageChanId image channel id
 *  @param[in] pFactor light source factor
 *  @param[out] pSls pointer to the sync light source
 *  @return error code
 */
UINT32 AmbaImgSensorDrv_SlsConvert(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT32 *pFactor, UINT32 *pSls)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;
    UINT32 ChainId;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    if ((AmbaImgSensorDrv_Chan[VinId] != NULL) &&
        (AmbaImgSensorDrv_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        FuncRetCode = AmbaImgSensorDrv_Chan[VinId][ChainId].pFunc->SlsConvert(ImageChanId, pFactor, pSls);
        if (FuncRetCode != OK_UL) {
            RetCode = NG_UL;
        }
    } else {
        /* chan null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

