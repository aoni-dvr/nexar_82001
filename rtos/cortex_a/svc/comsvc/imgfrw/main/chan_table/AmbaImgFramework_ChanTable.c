/**
 *  @file AmbaImgFramework_ChanTable.c
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
 *  @details Amba Image Framework Chan Table
 *
 */

#include "AmbaTypes.h"
#include "AmbaVIN_Def.h"
#include "AmbaKAL.h"

#include "AmbaDSP_ImageFilter.h"

#include "AmbaImgFramework_Chan.h"

#define OK_UL   ((UINT32) 0U)
#define NG_UL   ((UINT32) 1U)

/**
 *  @private
 *  Amba image channel select
 *  @param[in] VinId vin id
 *  @param[in] ChainIndex chain index
 *  @return (AMBA_IMG_CHANNEL_s **)
 *  @note this function is intended for internal use only
 */
AMBA_IMG_CHANNEL_s **AmbaImgChannel_Select(UINT32 VinId, UINT32 ChainIndex)
{
    if (ChainIndex < AMBA_IMG_NUM_VIN_ALGO) {
        /* default: 0~3 */
        AmbaImgChannel_Entry[VinId] = AmbaImgChannel_EntryTable[VinId][ChainIndex];
    } else {
        if (ChainIndex == 0xFFU) {
            /* null: 255 */
            AmbaImgChannel_Entry[VinId] = NULL;
        } else {
            /* user define (private): 4~254 */
            AmbaImgChannel_Entry[VinId] = AmbaImgChannel_UserEntry(VinId, ChainIndex);

        }
    }

    return AmbaImgChannel_Entry;
}

/**
 *  @private
 *  Amba image channel sensor id put
 *  @param[in] VinId vin id
 *  @param[in] ChainId chain id
 *  @param[in] SensorId sensor id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgChannel_SensorIdPut(UINT32 VinId, UINT32 ChainId, UINT32 SensorId)
{
    UINT32 RetCode = OK_UL;

    if (AmbaImgChannel_Entry[VinId] != NULL) {
        if (AmbaImgChannel_Entry[VinId][ChainId].Magic == 0xCafeU) {
            if (AmbaImgChannel_Entry[VinId][ChainId].pCtx != NULL) {
                AmbaImgChannel_Entry[VinId][ChainId].pCtx->Id.Ctx.Bits.SensorId = (UINT8) (SensorId & 0xFFU);
            } else {
                RetCode = NG_UL;
            }
        } else {
            RetCode = NG_UL;
        }
    } else {
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image channel algo id put
 *  @param[in] VinId vin id
 *  @param[in] ChainId chain id
 *  @param[in] AlgoId algo id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgChannel_AlgoIdPut(UINT32 VinId, UINT32 ChainId, UINT32 AlgoId)
{
    UINT32 RetCode = OK_UL;

    if (AmbaImgChannel_Entry[VinId] != NULL) {
        if (AmbaImgChannel_Entry[VinId][ChainId].Magic == 0xCafeU) {
            if (AmbaImgChannel_Entry[VinId][ChainId].pCtx != NULL) {
                AmbaImgChannel_Entry[VinId][ChainId].pCtx->Id.Ctx.Bits.AlgoId = (UINT8) (AlgoId & 0xFFU);
            } else {
                RetCode = NG_UL;
            }
        } else {
            RetCode = NG_UL;
        }
    } else {
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image channel zone id put
 *  @param[in] VinId vin id
 *  @param[in] ChainId chain id
 *  @param[in] ZoneId zone id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgChannel_ZoneIdPut(UINT32 VinId, UINT32 ChainId, UINT32 ZoneId)
{
    UINT32 RetCode = OK_UL;

    if (AmbaImgChannel_Entry[VinId] != NULL) {
        if (AmbaImgChannel_Entry[VinId][ChainId].Magic == 0xCafeU) {
            if (AmbaImgChannel_Entry[VinId][ChainId].pCtx != NULL) {
                AmbaImgChannel_Entry[VinId][ChainId].pCtx->Id.Ctx.Bits.ZoneId = ZoneId;
            } else {
                RetCode = NG_UL;
            }
        } else {
            RetCode = NG_UL;
        }
    } else {
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image channel zone msb id put
 *  @param[in] VinId vin id
 *  @param[in] ChainId chain id
 *  @param[in] ZoneMsbId zone msb id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgChannel_ZoneMsbIdPut(UINT32 VinId, UINT32 ChainId, UINT32 ZoneMsbId)
{
    UINT32 RetCode = OK_UL;

    if (AmbaImgChannel_Entry[VinId] != NULL) {
        if (AmbaImgChannel_Entry[VinId][ChainId].Magic == 0xCafeU) {
            if (AmbaImgChannel_Entry[VinId][ChainId].pCtx != NULL) {
                AmbaImgChannel_Entry[VinId][ChainId].pCtx->Id.Ctx.Bits.ZoneMsb = (UINT8) (ZoneMsbId & 0xFFU);
            } else {
                RetCode = NG_UL;
            }
        } else {
            RetCode = NG_UL;
        }
    } else {
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image channel vr id put
 *  @param[in] VinId vin id
 *  @param[in] ChainId chain id
 *  @param[in] VrId vr id
 *  @param[in] VrAltId vr alt id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgChannel_VrIdPut(UINT32 VinId, UINT32 ChainId, UINT32 VrId, UINT32 VrAltId)
{
    UINT32 RetCode = OK_UL;

    if (AmbaImgChannel_Entry[VinId] != NULL) {
        if (AmbaImgChannel_Entry[VinId][ChainId].Magic == 0xCafeU) {
            if (AmbaImgChannel_Entry[VinId][ChainId].pCtx != NULL) {
                AmbaImgChannel_Entry[VinId][ChainId].pCtx->VrMap.Id.Ctx.Bits.VrId = VrId;
                AmbaImgChannel_Entry[VinId][ChainId].pCtx->VrMap.Id.Ctx.Bits.VrAltId = (UINT8) (VrAltId & 0xFFU);
            } else {
                RetCode = NG_UL;
            }
        } else {
            RetCode = NG_UL;
        }
    } else {
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image channel inter id put
 *  @param[in] VinId vin id
 *  @param[in] ChainId chain id
 *  @param[in] InterId inter id
 *  @param[in] InterNum inter number
 *  @param[in] SkipFrame skip frame
 *  @param[in] Mode mode
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgChannel_InterIdPut(UINT32 VinId, UINT32 ChainId, UINT32 InterId, UINT32 InterNum, UINT32 SkipFrame, UINT32 Mode)
{
    UINT32 RetCode = OK_UL;
    UINT32 i;

    if (AmbaImgChannel_Entry[VinId] != NULL) {
        if (AmbaImgChannel_Entry[VinId][ChainId].Magic == 0xCafeU) {
            if (AmbaImgChannel_Entry[VinId][ChainId].pCtx != NULL) {
                AmbaImgChannel_Entry[VinId][ChainId].pCtx->Inter.Ctx.Data = 0ULL;
                AmbaImgChannel_Entry[VinId][ChainId].pCtx->Inter.Ctx.Bits.Id = (UINT32) InterId;
                for (i = 0U; i < InterNum; i++) {
                    if ((InterId & (((UINT32) 1U) << i)) > 0U) {
                        AmbaImgChannel_Entry[VinId][ChainId].pCtx->Inter.Ctx.Bits.FirstId = (UINT8) i;
                        break;
                    }
                }
                AmbaImgChannel_Entry[VinId][ChainId].pCtx->Inter.Ctx.Bits.Num = (UINT8) (InterNum & 0xFFU);
                AmbaImgChannel_Entry[VinId][ChainId].pCtx->Inter.Ctx.Bits.Skip = (UINT8) (SkipFrame & 0xFFU);
                AmbaImgChannel_Entry[VinId][ChainId].pCtx->Inter.Ctx.Bits.Mode = (UINT8) (Mode & 0xFFU);
            } else {
                RetCode = NG_UL;
            }
        } else {
            RetCode = NG_UL;
        }
    } else {
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image channel post zone id put
 *  @param[in] VinId vin id
 *  @param[in] ChainId chain id
 *  @param[in] PostZoneId post zone id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgChannel_PostZoneIdPut(UINT32 VinId, UINT32 ChainId, UINT32 PostZoneId)
{
    UINT32 RetCode = OK_UL;

    if (AmbaImgChannel_Entry[VinId] != NULL) {
        if (AmbaImgChannel_Entry[VinId][ChainId].Magic == 0xCafeU) {
            if (AmbaImgChannel_Entry[VinId][ChainId].pCtx != NULL) {
                AmbaImgChannel_Entry[VinId][ChainId].pCtx->PAik.Ctx.Data = 0ULL;
                AmbaImgChannel_Entry[VinId][ChainId].pCtx->PAik.Ctx.Bits.ZoneId = PostZoneId;
            } else {
                RetCode = NG_UL;
            }
        } else {
            RetCode = NG_UL;
        }
    } else {
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image channel post zone id add
 *  @param[in] VinId vin id
 *  @param[in] ChainId chain id
 *  @param[in] PostZoneId post zone id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgChannel_PostZoneIdAdd(UINT32 VinId, UINT32 ChainId, UINT32 PostZoneId)
{
    UINT32 RetCode = OK_UL;

    if (AmbaImgChannel_Entry[VinId] != NULL) {
        if (AmbaImgChannel_Entry[VinId][ChainId].Magic == 0xCafeU) {
            if (AmbaImgChannel_Entry[VinId][ChainId].pCtx != NULL) {
                AmbaImgChannel_Entry[VinId][ChainId].pCtx->PAik.Ctx.Bits.ZoneId |= PostZoneId;
            } else {
                RetCode = NG_UL;
            }
        } else {
            RetCode = NG_UL;
        }
    } else {
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image channel zone iso disable id put
 *  @param[in] VinId vin id
 *  @param[in] ChainId chain id
 *  @param[in] ZoneIsoDisId zone iso disable id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgChannel_ZoneIsoDisIdPut(UINT32 VinId, UINT32 ChainId, UINT32 ZoneIsoDisId)
{
    UINT32 RetCode = OK_UL;

    if (AmbaImgChannel_Entry[VinId] != NULL) {
        if (AmbaImgChannel_Entry[VinId][ChainId].Magic == 0xCafeU) {
            if (AmbaImgChannel_Entry[VinId][ChainId].pCtx != NULL) {
                AmbaImgChannel_Entry[VinId][ChainId].pCtx->Aik.Ctx.Bits.IsoDisId = ZoneIsoDisId;
            } else {
                RetCode = NG_UL;
            }
        } else {
            RetCode = NG_UL;
        }
    } else {
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image channel zone iso disable id add
 *  @param[in] VinId vin id
 *  @param[in] ChainId chain id
 *  @param[in] ZoneIsoDisId zone iso disable id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgChannel_ZoneIsoDisIdAdd(UINT32 VinId, UINT32 ChainId, UINT32 ZoneIsoDisId)
{
    UINT32 RetCode = OK_UL;

    if (AmbaImgChannel_Entry[VinId] != NULL) {
        if (AmbaImgChannel_Entry[VinId][ChainId].Magic == 0xCafeU) {
            if (AmbaImgChannel_Entry[VinId][ChainId].pCtx != NULL) {
                AmbaImgChannel_Entry[VinId][ChainId].pCtx->Aik.Ctx.Bits.IsoDisId |= ZoneIsoDisId;
            } else {
                RetCode = NG_UL;
            }
        } else {
            RetCode = NG_UL;
        }
    } else {
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image channel zone iso callback
 *  @param[in] VinId vin id
 *  @param[in] ChainId chain id
 *  @param[in] pFunc callback function
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgChannel_ZoneIsoCb(UINT32 VinId, UINT32 ChainId, AMBA_IMG_CHANNEL_ISO_CB_f pFunc)
{
    UINT32 RetCode = OK_UL;

    if (AmbaImgChannel_Entry[VinId] != NULL) {
        if (AmbaImgChannel_Entry[VinId][ChainId].Magic == 0xCafeU) {
            if (AmbaImgChannel_Entry[VinId][ChainId].pCtx != NULL) {
                AmbaImgChannel_Entry[VinId][ChainId].IsoCb = pFunc;
            } else {
                RetCode = NG_UL;
            }
        } else {
            RetCode = NG_UL;
        }
    } else {
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image channel snap aeb id put
 *  @param[in] VinId vin id
 *  @param[in] ChainId chain id
 *  @param[in] SnapAebId snap aeb id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgChannel_SnapAebIdPut(UINT32 VinId, UINT32 ChainId, UINT32 SnapAebId)
{
    UINT32 RetCode = OK_UL;
    UINT32 i;

    UINT32 VrId;
    UINT32 VrAltId;

    UINT32 mVinId = VinId;
    UINT32 mChainId = ChainId;

    const AMBA_IMG_CHANNEL_CONTEXT_s *pCtx;

    if (AmbaImgChannel_Entry[VinId] != NULL) {
        if (AmbaImgChannel_Entry[VinId][ChainId].Magic == 0xCafeU) {
            if (AmbaImgChannel_Entry[VinId][ChainId].pCtx != NULL) {
                AmbaImgChannel_Entry[VinId][ChainId].pCtx->Snap.Ctx.Bits.AebId = (UINT16) (SnapAebId & 0xFFFFU);
                /* ctx get */
                pCtx = AmbaImgChannel_Entry[VinId][ChainId].pCtx;
                /* vr? */
                if (pCtx->VrMap.Id.Ctx.Bits.VrId > 0U) {
                    /* vr id get */
                    VrId = pCtx->VrMap.Id.Ctx.Bits.VrId;
                    /* vr alt id get */
                    VrAltId = pCtx->VrMap.Id.Ctx.Bits.VrAltId;
                    /* slave? */
                    if (((VrId & (((UINT32) 1U) << VinId)) == 0U) ||
                        ((VrAltId & (((UINT32) 1U) << ChainId)) == 0U)) {
                        /* master vin id get */
                        for (i = 0U; i < AMBA_IMG_NUM_VIN_CHANNEL; i++) {
                            if ((VrId & (((UINT32) 1U) << i)) > 0U) {
                                mVinId = i;
                                break;
                            }
                        }
                        /* master chain id get */
                        for (i = 0U; i < AMBA_IMG_NUM_VIN_ALGO; i++) {
                            if ((VrAltId & (((UINT32) 1U) << i)) > 0U) {
                                mChainId = i;
                                break;
                            }
                        }
                        /* master put */
                        if (AmbaImgChannel_Entry[mVinId] != NULL) {
                            if (AmbaImgChannel_Entry[mVinId][mChainId].Magic == 0xCafeU) {
                                if (AmbaImgChannel_Entry[mVinId][mChainId].pCtx != NULL) {
                                    AmbaImgChannel_Entry[mVinId][mChainId].pCtx->Snap.Ctx.Bits.AebId = (UINT16) (SnapAebId & 0xFFFFU);
                                } else {
                                    RetCode = NG_UL;
                                }
                            } else {
                                RetCode = NG_UL;
                            }
                        } else {
                            RetCode = NG_UL;
                        }
                    }
                }
            } else {
                RetCode = NG_UL;
            }
        } else {
            RetCode = NG_UL;
        }
    } else {
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image channel snap aeb id get
 *  @param[in] VinId vin id
 *  @param[in] ChainId chain id
 *  @param[in] *pSnapAebId pointer to the sanp aeb id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgChannel_SnapAebIdGet(UINT32 VinId, UINT32 ChainId, UINT32 *pSnapAebId)
{
    UINT32 RetCode = OK_UL;

    if (AmbaImgChannel_Entry[VinId] != NULL) {
        if (AmbaImgChannel_Entry[VinId][ChainId].Magic == 0xCafeU) {
            if (AmbaImgChannel_Entry[VinId][ChainId].pCtx != NULL) {
                *pSnapAebId = (UINT32) AmbaImgChannel_Entry[VinId][ChainId].pCtx->Snap.Ctx.Bits.AebId;
            } else {
                RetCode = NG_UL;
            }
        } else {
            RetCode = NG_UL;
        }
    } else {
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image channel pipe out put
 *  @param[in] VinId vin id
 *  @param[in] ChainId chain id
 *  @param[in] ZoneIdx zone index
 *  @param[in] Width ouput width
 *  @param[in] Height ouput heigh
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgChannel_PipeOutPut(UINT32 VinId, UINT32 ChainId, UINT32 ZoneIdx, UINT32 Width, UINT32 Height)
{
    UINT32 RetCode = OK_UL;

    const AMBA_IMG_SYSTEM_CONTEXT_s *pCtx;
    AMBA_IMG_SYSTEM_ZONE_s *pZone;

    if (AmbaImgSystem_Chan[VinId] != NULL) {
        if (AmbaImgSystem_Chan[VinId][ChainId].Magic == 0xCafeU) {
            if (AmbaImgSystem_Chan[VinId][ChainId].pCtx != NULL) {
                /* ctx get */
                pCtx = AmbaImgSystem_Chan[VinId][ChainId].pCtx;
                if (pCtx[ZoneIdx].Magic == 0xCafeU) {
                    /* zone get */
                    pZone = pCtx[ZoneIdx].pZone;
                    if (pZone != NULL) {
                        /* pipe out put */
                        pZone->Cfg.Out.Data = 0ULL;
                        pZone->Cfg.Out.Bits.Width = (UINT16) (Width & 0xFFFFU);
                        pZone->Cfg.Out.Bits.Height = (UINT16) (Height & 0xFFFFU);
                    } else {
                        RetCode = NG_UL;
                    }
                } else {
                    RetCode = NG_UL;
                }
            } else {
                RetCode = NG_UL;
            }
        } else {
            RetCode = NG_UL;
        }
    } else {
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image channel pipe out get
 *  @param[in] VinId vin id
 *  @param[in] ChainId chain id
 *  @param[in] ZoneIdx zone index
 *  @param[in] pWidth pointer to the ouput width
 *  @param[in] pHeight pointer to the ouput heigh
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgChannel_PipeOutGet(UINT32 VinId, UINT32 ChainId, UINT32 ZoneIdx, UINT32 *pWidth, UINT32 *pHeight)
{
    UINT32 RetCode = OK_UL;

    const AMBA_IMG_SYSTEM_CONTEXT_s *pCtx;
    const AMBA_IMG_SYSTEM_ZONE_s *pZone;

    if (AmbaImgSystem_Chan[VinId] != NULL) {
        if (AmbaImgSystem_Chan[VinId][ChainId].Magic == 0xCafeU) {
            if (AmbaImgSystem_Chan[VinId][ChainId].pCtx != NULL) {
                /* ctx get */
                pCtx = AmbaImgSystem_Chan[VinId][ChainId].pCtx;
                if (pCtx[ZoneIdx].Magic == 0xCafeU) {
                    /* zone get */
                    pZone = pCtx[ZoneIdx].pZone;
                    if (pZone != NULL) {
                        /* pipe out get */
                        *pWidth = pZone->Cfg.Out.Bits.Width;
                        *pHeight = pZone->Cfg.Out.Bits.Height;
                    } else {
                        RetCode = NG_UL;
                    }
                } else {
                    RetCode = NG_UL;
                }
            } else {
                RetCode = NG_UL;
            }
        } else {
            RetCode = NG_UL;
        }
    } else {
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image channel pipe mode put
 *  @param[in] VinId vin id
 *  @param[in] ChainId chain id
 *  @param[in] ZoneIdx zone index
 *  @param[in] Mode pipe mode
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgChannel_PipeModePut(UINT32 VinId, UINT32 ChainId, UINT32 ZoneIdx, UINT32 Mode)
{
    UINT32 RetCode = OK_UL;

    const AMBA_IMG_SYSTEM_CONTEXT_s *pCtx;
    AMBA_IMG_SYSTEM_ZONE_s *pZone;

    if (AmbaImgSystem_Chan[VinId] != NULL) {
        if (AmbaImgSystem_Chan[VinId][ChainId].Magic == 0xCafeU) {
            if (AmbaImgSystem_Chan[VinId][ChainId].pCtx != NULL) {
                /* ctx get */
                pCtx = AmbaImgSystem_Chan[VinId][ChainId].pCtx;
                if (pCtx[ZoneIdx].Magic == 0xCafeU) {
                    /* zone get */
                    pZone = pCtx[ZoneIdx].pZone;
                    if (pZone != NULL) {
                        /* pipe mode put */
                        pZone->Op.Mode = Mode;
                    } else {
                        RetCode = NG_UL;
                    }
                } else {
                    RetCode = NG_UL;
                }
            } else {
                RetCode = NG_UL;
            }
        } else {
            RetCode = NG_UL;
        }
    } else {
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image channel pipe mode get
 *  @param[in] VinId vin id
 *  @param[in] ChainId chain id
 *  @param[in] ZoneIdx zone index
 *  @param[in] pMode pointer to the pipe mode
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgChannel_PipeModeGet(UINT32 VinId, UINT32 ChainId, UINT32 ZoneIdx, UINT32 *pMode)
{
    UINT32 RetCode = OK_UL;

    const AMBA_IMG_SYSTEM_CONTEXT_s *pCtx;
    const AMBA_IMG_SYSTEM_ZONE_s *pZone;

    if (AmbaImgSystem_Chan[VinId] != NULL) {
        if (AmbaImgSystem_Chan[VinId][ChainId].Magic == 0xCafeU) {
            if (AmbaImgSystem_Chan[VinId][ChainId].pCtx != NULL) {
                /* ctx get */
                pCtx = AmbaImgSystem_Chan[VinId][ChainId].pCtx;
                if (pCtx[ZoneIdx].Magic == 0xCafeU) {
                    /* zone get */
                    pZone = pCtx[ZoneIdx].pZone;
                    if (pZone != NULL) {
                        /* pipe mode get */
                        *pMode = pZone->Op.Mode;
                    } else {
                        RetCode = NG_UL;
                    }
                } else {
                    RetCode = NG_UL;
                }
            } else {
                RetCode = NG_UL;
            }
        } else {
            RetCode = NG_UL;
        }
    } else {
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image channel external fov id put
 *  @param[in] VinId vin id
 *  @param[in] ChainId chain id
 *  @param[in] EFovId EFov id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgChannel_EFovIdPut(UINT32 VinId, UINT32 ChainId, UINT64 EFovId)
{
    UINT32 RetCode = OK_UL;

    if (AmbaImgChannel_Entry[VinId] != NULL) {
        if (AmbaImgChannel_Entry[VinId][ChainId].Magic == 0xCafeU) {
            if (AmbaImgChannel_Entry[VinId][ChainId].pCtx != NULL) {
                AmbaImgChannel_Entry[VinId][ChainId].pCtx->EFov.Ctx.Data = EFovId;
            } else {
                RetCode = NG_UL;
            }
        } else {
            RetCode = NG_UL;
        }
    } else {
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image channel external fov id get
 *  @param[in] VinId vin id
 *  @param[in] ChainId chain id
 *  @param[in] pEFovId pointer to the EFov id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgChannel_EFovIdGet(UINT32 VinId, UINT32 ChainId, UINT64 *pEFovId)
{
    UINT32 RetCode = OK_UL;

    if (AmbaImgChannel_Entry[VinId] != NULL) {
        if (AmbaImgChannel_Entry[VinId][ChainId].Magic == 0xCafeU) {
            if (AmbaImgChannel_Entry[VinId][ChainId].pCtx != NULL) {
                *pEFovId = AmbaImgChannel_Entry[VinId][ChainId].pCtx->EFov.Ctx.Data;
            } else {
                RetCode = NG_UL;
            }
        } else {
            RetCode = NG_UL;
        }
    } else {
        RetCode = NG_UL;
    }

    return RetCode;
}
