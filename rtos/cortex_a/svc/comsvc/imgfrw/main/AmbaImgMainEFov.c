/**
 *  @file AmbaImgMainEFov.c
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
 *  @details Amba Image Main External Fov
 *
 */

#include "AmbaTypes.h"
#include "AmbaVIN_Def.h"

#include "AmbaKAL.h"

#include "AmbaDSP_ImageDefine.h"
#include "AmbaDSP_ImageFilter.h"

#include "AmbaImg_External_CtrlFunc.h"

#include "AmbaImgFramework.h"
#include "AmbaImgMain.h"
#include "AmbaImgMain_Internal.h"
#include "AmbaImgMainAdj.h"
#include "AmbaImgMainAe_Platform.h"
#include "AmbaImgMainAwb.h"
#include "AmbaImgMainEFov.h"

#define OK_UL    ((UINT32) 0U)
#define NG_UL    ((UINT32) 1U)

typedef union /*_EFOV_MEM_INFO_u_*/ {
    AMBA_IMG_MAIN_MEM_ADDR             Data;
#if 0
    AMBA_IMG_MAIN_AE_EFOV_TXRX_s       *pAeTxRx;
    AMBA_IMG_MAIN_ADJ_EFOV_TXRX_s      *pAdjTxRx;
    AMBA_IMG_MAIN_AWB_EFOV_TXRX_s      *pAwbTxRx;
#endif
    UINT64                             *pUint64;
    AMBA_IMG_MAIN_EFOV_TX_CB_f         pFunc;
} EFOV_MEM_INFO_u;

typedef struct /*_EFOV_MEM_INFO_s_*/ {
    EFOV_MEM_INFO_u    Ctx;
} EFOV_MEM_INFO_s;

static AMBA_IMG_MAIN_EFOV_TX_CB_f AmbaImgMainEFov_TxCb = NULL;

static UINT32 AmbaImgMainEFov_Rx(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 RxData, UINT32 Size);

/**
 *  Amba image main external fov tx cb hook
 *  @param[in] pFunc efov tx callback function
 *  @note this function is intended for internal use only
 */
static void AmbaImgMainEFov_TxCbHook(AMBA_IMG_MAIN_EFOV_TX_CB_f pFunc)
{
    AmbaImgMainEFov_TxCb = pFunc;
}

/**
 *  Amba image main external fov rx put
 *  @param[in] RxData rx data
 *  @param[in] Size rx data size
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgMainEFov_RxPut(UINT64 RxData, UINT32 Size)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i, j;

    AMBA_IMG_CHANNEL_ID_s ImageChanId;

    AMBA_IMG_CHANNEL_AIK_PROC_MSG_s Msg2;

    EFOV_MEM_INFO_s MemInfo;

    const AMBA_IMG_CHANNEL_CONTEXT_s *pCtx;

    /* rx get */
    MemInfo.Ctx.Data = (AMBA_IMG_MAIN_MEM_ADDR) RxData;
    /* msg get */
    Msg2.Ctx.Data = *(MemInfo.Ctx.pUint64);

    for (i = 0U; i < AMBA_IMG_NUM_VIN_CHANNEL; i ++) {
        if (pAmbaImgMainChannel[i] != NULL) {
            for (j = 0U; pAmbaImgMainChannel[i][j].Magic == 0xCafeU; j++) {
                /* ctx check */
                if (pAmbaImgMainChannel[i][j].pCtx != NULL) {
                    /* ctx get */
                    pCtx = pAmbaImgMainChannel[i][j].pCtx;
                    /* image channel id get */
                    ImageChanId.Ctx.Data = pCtx->Id.Ctx.Data;
                    /* efov id check */
                    if ((pCtx->EFov.Ctx.Bits.Mode == (UINT8) IMG_CHAN_EFOV_SLAVE) &&
                        (pCtx->EFov.Ctx.Bits.Id == Msg2.Ctx.Bits2.Id)) {
                        FuncRetCode = AmbaImgMainEFov_Rx(ImageChanId, RxData, Size);
                        if (FuncRetCode != OK_UL) {
                            RetCode = NG_UL;
                        }
                    }
                }
            }
        }
    }

    return RetCode;
}

/**
 *  Amba image main external fov tx put
 *  @param[in] TxData tx data
 *  @param[in] Size tx data size
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgMainEFov_TxPut(UINT64 TxData, UINT32 Size)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    if (AmbaImgMainEFov_TxCb != NULL) {
        FuncRetCode = AmbaImgMainEFov_TxCb(TxData, Size);
        if(FuncRetCode != OK_UL) {
            RetCode = NG_UL;
        }
    } else {
#ifdef CONFIG_BUILD_IMGFRW_EFOV_LPBK
        FuncRetCode = AmbaImgMainEFov_RxPut(TxData, Size);
        if (FuncRetCode != OK_UL) {
            RetCode = NG_UL;
        }
#else
        RetCode = NG_UL;
#endif
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image main external fov init
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainEFov_Init(void)
{
    UINT32 RetCode = OK_UL;
    return RetCode;
}

/**
 *  @private
 *  Amba image main external fov dispatch
 *  @param[in] VinId vin id
 *  @param[in] Param command and message
 *  @note this function is intended for internal use only
 */
void AmbaImgMainEFov_Dispatch(UINT32 VinId, UINT64 Param)
{
    UINT32 FuncRetCode;

    AMBA_IMG_MAIN_CMD_MSG_s CmdMsg;

    CmdMsg.Ctx.Data = Param;

    if (VinId < AMBA_IMG_NUM_VIN_CHANNEL) {
        switch (CmdMsg.Ctx.Bits.Cmd) {
            case (UINT8) IMAGE_CMD_CHAN:
                switch (CmdMsg.Ctx.Bits.Msg) {
                    case (UINT8) CHAN_MSG_EFOV:
                        AmbaImgPrint2(PRINT_FLAG_DBG, "chan-msg-efov", VinId, (UINT32) CmdMsg.Ctx.ImageChanEFov.Msg);
                        AmbaImgPrint2(PRINT_FLAG_DBG, "chain id", VinId, CmdMsg.Ctx.ImageChanEFov.ChainId);
                        AmbaImgPrint2(PRINT_FLAG_DBG, "mode", VinId, CmdMsg.Ctx.ImageChanEFov.Mode);
                        AmbaImgPrint2(PRINT_FLAG_DBG, "id", VinId, CmdMsg.Ctx.ImageChanEFov.Id);
                        AmbaImgPrintEx2(PRINT_FLAG_DBG, "var", VinId, 10U, CmdMsg.Ctx.ImageChanEFov.Var, 16U);
                        FuncRetCode = AmbaImgChannel_EFovIdPut(VinId, CmdMsg.Ctx.ImageChanEFov.ChainId, CmdMsg.Ctx.Data);
                        if (FuncRetCode != OK_UL) {
                            AmbaImgPrint(PRINT_FLAG_ERR, "error: chan efov id", FuncRetCode);
                        }
                        break;
                    default:
                        /* */
                        break;
                }
                break;
            default:
                /* */
                break;
        }
    }
}

/**
 *  @private
 *  Amba image main external fov dispatch ex
 *  @param[in] VinId vin id
 *  @param[in] Param command and message
 *  @param[in] Param2 aux data
 *  @note this function is intended for internal use only
 */
void AmbaImgMainEFov_DispatchEx(UINT32 VinId, UINT64 Param, UINT64 Param2)
{
    UINT32 FuncRetCode;

    EFOV_MEM_INFO_s MemInfo;

    AMBA_IMG_MAIN_CMD_MSG_s CmdMsg;
    AMBA_IMG_MAIN_AUX_DATA_s AuxData;

    CmdMsg.Ctx.Data = Param;
    AuxData.Ctx.Data = Param2;

    if (VinId < AMBA_IMG_NUM_VIN_CHANNEL) {
        switch (CmdMsg.Ctx.Bits.Cmd) {
            case (UINT8) IMG_EFOV_CMD_TXCB:
                AmbaImgPrint2(PRINT_FLAG_DBG, "efov-cmd-txcb", VinId, (UINT32) CmdMsg.Ctx.Bits.Cmd);
                MemInfo.Ctx.Data = (AMBA_IMG_MAIN_MEM_ADDR) AuxData.Ctx.Bits.Var;
                AmbaImgPrint64Ex(PRINT_FLAG_DBG, "efov tx cb", (UINT64) MemInfo.Ctx.Data, 16U);
                AmbaImgMainEFov_TxCbHook(MemInfo.Ctx.pFunc);
                break;
            case (UINT8) IMG_EFOV_CMD_RX:
                AmbaImgPrint2(PRINT_FLAG_CYCLE, "efov-cmd-rx", VinId, (UINT32) CmdMsg.Ctx.Bits.Cmd);
                MemInfo.Ctx.Data = (AMBA_IMG_MAIN_MEM_ADDR) AuxData.Ctx.Bits.Var;
                AmbaImgPrint64Ex2(PRINT_FLAG_CYCLE, "efov rx", (UINT64) MemInfo.Ctx.Data, 16U, (UINT64) CmdMsg.Ctx.Bits.Var, 10U);
                FuncRetCode = AmbaImgMainEFov_RxPut(AuxData.Ctx.Bits.Var, CmdMsg.Ctx.Bits.Var);
                if (FuncRetCode != OK_UL) {
                    /* */
                }
                break;
            default:
                /* */
                break;
        }
    }
}

/**
 *  @private
 *  Amba image main efov tx
 *  @param[in] ImageChanId image channel id
 *  @param[in] TxData tx data
 *  @param[in] Size tx data size
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainEFov_Tx(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 TxData, UINT32 Size)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;
    UINT32 ChainId;

    UINT32 EFovId;

    const AMBA_IMG_CHANNEL_EFOV_s *pEFov;
    AMBA_IMG_CHANNEL_AIK_PROC_MSG_s Msg2;

    EFOV_MEM_INFO_s MemInfo;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    /* tx get */
    MemInfo.Ctx.Data = (AMBA_IMG_MAIN_MEM_ADDR) TxData;
    /* msg get */
    Msg2.Ctx.Data = *(MemInfo.Ctx.pUint64);

    if ((pAmbaImgMainChannel != NULL) &&
        (pAmbaImgMainChannel[VinId] != NULL) &&
        (pAmbaImgMainChannel[VinId][ChainId].Magic == 0xCafeU)) {
        if (pAmbaImgMainChannel[VinId][ChainId].pCtx != NULL) {
            /* efov id get */
            pEFov = &(pAmbaImgMainChannel[VinId][ChainId].pCtx->EFov);
            /* master? */
            if (pEFov->Ctx.Bits.Mode == (UINT32) IMG_CHAN_EFOV_MASTER) {
                /* efov id get */
                EFovId = pEFov->Ctx.Bits.Id;
                /* efov id put */
                Msg2.Ctx.Bits2.Id = (UINT8) EFovId;
                /* msg put */
                *(MemInfo.Ctx.pUint64) = Msg2.Ctx.Data;
                /* tx ae transfer */
                FuncRetCode = AmbaImgMainEFov_TxPut(TxData, Size);
                if (FuncRetCode != OK_UL) {
                    RetCode = NG_UL;
                }
                /* debug msg */
                if (FuncRetCode == OK_UL) {
                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Tx");
                } else {
                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Tx!");
                }
                {
                    char str[11];
                    /* id */
                    str[0] = 'i';str[1] = ' ';
                    var_utoa(EFovId, &(str[2]), 16U, 8U, (UINT32) VAR_LEADING_SPACE);
                    AmbaImgSensorHAL_TimingMarkPut(ImageChanId.Ctx.Bits.VinId, str);
                }
            } else {
                /* mode error */
                RetCode = NG_UL;
            }
        }
    }

    return RetCode;
}

/**
 *  Amba image main efov rx
 *  @param[in] ImageChanId image channel id
 *  @param[in] RxData rx data
 *  @param[in] Size rx data size
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgMainEFov_Rx(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 RxData, UINT32 Size)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;
    UINT32 ChainId;

    AMBA_IMG_CHANNEL_EFOV_RX_CB_f pFunc;

    const AMBA_IMG_CHANNEL_EFOV_s *pEFov;
    AMBA_IMG_CHANNEL_AIK_PROC_MSG_s Msg2;

    EFOV_MEM_INFO_s MemInfo;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    /* rx get */
    MemInfo.Ctx.Data = (AMBA_IMG_MAIN_MEM_ADDR) RxData;
    /* msg get */
    Msg2.Ctx.Data = *(MemInfo.Ctx.pUint64);

    if ((pAmbaImgMainChannel != NULL) &&
        (pAmbaImgMainChannel[VinId] != NULL) &&
        (pAmbaImgMainChannel[VinId][ChainId].Magic == 0xCafeU)) {
        if (pAmbaImgMainChannel[VinId][ChainId].pCtx != NULL) {
            /* efov id get */
            pEFov = &(pAmbaImgMainChannel[VinId][ChainId].pCtx->EFov);
            /* master? */
            if (pEFov->Ctx.Bits.Mode == (UINT32) IMG_CHAN_EFOV_SLAVE) {
                /* rx put */
                if (Msg2.Ctx.Bits2.Ae > 0U) {
                    if (Size != sizeof(AMBA_IMG_MAIN_AE_EFOV_TXRX_s)) {
                        /* data mismatch */
                        AmbaImgPrintStr(PRINT_FLAG_ERR, "error: rx ae");
                    } else {
                        /* ae rx */

                        /* ae rx user */

                        /* ae rx cb get */
                        pFunc = pAmbaImgMainChannel[VinId][ChainId].EFovRxCb[IMG_CHAN_EFOV_CB_ID_AE];
                        /* ae user put */
                        if (pFunc != NULL) {
                            /* ae rx cb */
                            FuncRetCode = pFunc(ImageChanId, Msg2.Ctx.Data, RxData);
                            if (FuncRetCode != OK_UL) {
                                RetCode = NG_UL;
                            }
                            /* debug msg */
                            if (FuncRetCode == OK_UL) {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Rx_Ae");
                            } else {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Rx_Ae!");
                            }
                        } else {
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Rx_Ae?");
                        }
                    }
                } else if (Msg2.Ctx.Bits2.Awb > 0U) {
                    if (Size != sizeof(AMBA_IMG_MAIN_AWB_EFOV_TXRX_s)) {
                        /* data mismatch */
                        AmbaImgPrintStr(PRINT_FLAG_ERR, "error: rx awb");
                    } else {
                        /* awb rx */

                        /* awb rx user */

                        /* awb rx cb get */
                        pFunc = pAmbaImgMainChannel[VinId][ChainId].EFovRxCb[IMG_CHAN_EFOV_CB_ID_AWB];
                        /* awb user put */
                        if (pFunc != NULL) {
                            FuncRetCode = pFunc(ImageChanId, Msg2.Ctx.Data, RxData);
                            if (FuncRetCode != OK_UL) {
                                RetCode = NG_UL;
                            }
                            /* debug msg */
                            if (FuncRetCode == OK_UL) {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Rx_Awb");
                            } else {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Rx_Awb!");
                            }
                        } else {
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Rx_Awb?");
                        }
                    }
                } else if (Msg2.Ctx.Bits2.Adj > 0U) {
                    if (Size != sizeof(AMBA_IMG_MAIN_ADJ_EFOV_TXRX_s)) {
                        /* data mismatch */
                        AmbaImgPrintStr(PRINT_FLAG_ERR, "error: rx adj");
                    } else {
                       /* adj rx */

                       /* adj rx user */

                       /* adj rx cb get */
                       pFunc = pAmbaImgMainChannel[VinId][ChainId].EFovRxCb[IMG_CHAN_EFOV_CB_ID_ADJ];
                       /* adj user put */
                       if (pFunc != NULL) {
                           /* adj rx cb */
                           FuncRetCode = pFunc(ImageChanId, Msg2.Ctx.Data, RxData);
                           if (FuncRetCode != OK_UL) {
                               RetCode = NG_UL;
                           }
                           /* debug msg */
                           if (FuncRetCode == OK_UL) {
                               AmbaImgSensorHAL_TimingMarkPut(VinId, "Rx_Adj");
                           } else {
                               AmbaImgSensorHAL_TimingMarkPut(VinId, "Rx_Adj!");
                           }
                       } else {
                           AmbaImgSensorHAL_TimingMarkPut(VinId, "Rx_Adj?");
                       }
                    }
                } else if (Msg2.Ctx.Bits2.AikExec > 0U) {
                    if (Size != sizeof(UINT64)) {
                        /* data mismatch */
                        AmbaImgPrintStr(PRINT_FLAG_ERR, "error: rx aik exec");
                    } else {
                        /* ae rx cb get */
                        pFunc = pAmbaImgMainChannel[VinId][ChainId].EFovRxCb[IMG_CHAN_EFOV_CB_ID_AE];
                        /* ae user put */
                        if (pFunc != NULL) {
                            /* ae rx cb */
                            FuncRetCode = pFunc(ImageChanId, Msg2.Ctx.Data, RxData);
                            if (FuncRetCode != OK_UL) {
                                RetCode = NG_UL;
                            }
                            /* debug msg */
                            if (FuncRetCode == OK_UL) {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Rx_AikExe");
                            } else {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Rx_AikExe!");
                            }
                        } else {
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Rx_AikExe?");
                        }
                    }
                } else if (Msg2.Ctx.Bits2.Raw > 0U) {
                    if (Size != sizeof(UINT64)) {
                        /* data mismatch */
                        AmbaImgPrintStr(PRINT_FLAG_ERR, "error: rx raw");
                    } else {
                        /* ae rx cb get */
                        pFunc = pAmbaImgMainChannel[VinId][ChainId].EFovRxCb[IMG_CHAN_EFOV_CB_ID_ADJ];
                        /* ae user put */
                        if (pFunc != NULL) {
                            /* ae rx cb */
                            FuncRetCode = pFunc(ImageChanId, Msg2.Ctx.Data, RxData);
                            if (FuncRetCode != OK_UL) {
                                RetCode = NG_UL;
                            }
                            /* debug msg */
                            if (FuncRetCode == OK_UL) {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Rx_Raw");
                            } else {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Rx_Raw!");
                            }
                        } else {
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Rx_Raw?");
                        }
                    }
                } else if (Msg2.Ctx.Bits2.SofEnd > 0U) {
                    /* sof end */
                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Rx_End");
                } else {
                    /* */
                    RetCode = NG_UL;
                }
            } else {
                /* mode error */
                RetCode = NG_UL;
            }
        }
    }

    return RetCode;
}

