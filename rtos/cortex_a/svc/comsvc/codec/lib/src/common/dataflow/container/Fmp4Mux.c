/**
 * @file Fmp4Mux.c
 *
 * Copyright (c) 2019 Ambarella International LP
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
 */
#include "format/SvcFmp4Mux.h"
#include "iso/Fmp4MuxImpl.h"
#include "FormatAPI.h"
#include "Muxer.h"
#include <AmbaDef.h>

/**
 *  Fmp4Mux handler
 */
typedef struct {
    SVC_MUX_FORMAT_HDLR_s Hdlr; /**< Muxing format handler. Must be the first member. */
    AMBA_KAL_MUTEX_t Mutex;     /**< Mutex */
    ISO_MOOV_BOX_s MoovAtom;    /**< Moov atom */
    ISO_MOOF_BOX_s MoofAtom;    /**< Moof atom */
    FRAGMENT_INFO_s Fragment;   /**< Fragment information */
    UINT8 Used;                 /**< Used flag */
    UINT8 Open;                 /**< Open flag */
} SVC_FMP4_MUX_HDLR_s;

/**
 *  Fmp4Mux manager
 */
typedef struct {
    AMBA_KAL_MUTEX_t Mutex;     /**< Mutex */
    SVC_FMP4_MUX_HDLR_s *Hdlr;  /**< Fmp4Mux handlers */
    UINT8 MaxHdlr;              /**< Maximum number of hanlders */
} SVC_FMP4_MUX_MGR_s;

static SVC_FMP4_MUX_MGR_s g_Fmp4MuxMgr GNU_SECTION_NOZEROINIT;
static SVC_MUX_FORMAT_s g_Fmp4Mux;
static void SvcFmp4Mux_InitOperation(void);

static UINT32 SvcFmp4Mux_GetInitBufSize_ParamCheck(UINT8 MaxHdlr, const UINT32 *BufferSize)
{
    UINT32 Rval = MUXER_ERR_INVALID_ARG;
    if (MaxHdlr > 0U) {
        if (BufferSize != NULL) {
            Rval = MUXER_OK;
        }
    }
    return Rval;
}

/**
 *  Get the required buffer size for initializing the Fmp4Mux module.
 *
 *  @param [in] MaxHdlr The maximum number of Fmp4Mux handlers
 *  @param [out] BufferSize The required buffer size.
 *  @return 0 - OK, others - MUXER_ERR_XXXX
 */
UINT32 SvcFmp4Mux_GetInitBufferSize(UINT8 MaxHdlr, UINT32 *BufferSize)
{
    UINT32 Rval = SvcFmp4Mux_GetInitBufSize_ParamCheck(MaxHdlr, BufferSize);
    if (Rval == MUXER_OK) {
        *BufferSize = GetAlignedValU32((UINT32)MaxHdlr * (UINT32)sizeof(SVC_FMP4_MUX_HDLR_s), (UINT32)AMBA_CACHE_LINE_SIZE);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Invalid agruement!", __func__, NULL, NULL, NULL, NULL);
    }
    return MUXER_OK;
}

static UINT32 SvcFmp4Mux_GetInitDefaultCfg_ParamCheck(const SVC_FMP4_MUX_INIT_CFG_s *Config)
{
    UINT32 Rval = MUXER_ERR_INVALID_ARG;
    if (Config != NULL) {
        Rval = MUXER_OK;
    }
    return Rval;
}

/**
 *  Get the default configuration for initializing the Fmp4Mux module.
 *
 *  @param [out] Config The returned configuration
 *  @return 0 - OK, others - MUXER_ERR_XXXX
 */
UINT32 SvcFmp4Mux_GetInitDefaultCfg(SVC_FMP4_MUX_INIT_CFG_s *Config)
{
    UINT32 Rval;
    Rval = SvcFmp4Mux_GetInitDefaultCfg_ParamCheck(Config);
    if (Rval == MUXER_OK) {
        Rval = W2M(AmbaWrap_memset(Config, 0, sizeof(SVC_FMP4_MUX_INIT_CFG_s)));
        if (Rval == MUXER_OK) {
            Config->MaxHdlr = 1;
            Config->Buffer = NULL;
            Rval = SvcFmp4Mux_GetInitBufferSize(Config->MaxHdlr, &Config->BufferSize);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Invalid agruement!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 SvcFmp4Mux_Init_ParamCheck(const SVC_FMP4_MUX_INIT_CFG_s *Config)
{
    UINT32 Rval = MUXER_OK;
    if (Config->MaxHdlr == 0U) {
        AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "incorrect handler number %u", Config->MaxHdlr, 0U, 0U, 0U, 0U);
        Rval = MUXER_ERR_INVALID_ARG;
    }
    if (Config->Buffer == NULL) {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s buffer is NULL!", __func__, NULL, NULL, NULL, NULL);
        Rval = MUXER_ERR_INVALID_ARG;
    }
    if (Rval == MUXER_OK) {
        UINT32 Size;
        Rval = SvcFmp4Mux_GetInitBufferSize(Config->MaxHdlr, &Size);
        if (Rval == MUXER_OK) {
            if (Config->BufferSize != Size) {
                AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "incorrect buffer Size %u", Config->BufferSize, 0U, 0U, 0U, 0U);
                Rval = MUXER_ERR_INVALID_ARG;
            }
        }
    }
    return Rval;
}

#define MAX_FMP4MUX_NUM  (16U)  /**< Maximum number of Fmp4Mux handlers */
static UINT32 CreateMutex(void)
{
    static char MuxMgrMutexName[16] = "Fmp4Mux_Mgr";
    UINT32 Rval = K2M(AmbaKAL_MutexCreate(&g_Fmp4MuxMgr.Mutex, MuxMgrMutexName));
    if (Rval == MUXER_OK) {
        static char MutexName[MAX_FMP4MUX_NUM][16];
        UINT32 i;
        for (i = 0; i < g_Fmp4MuxMgr.MaxHdlr; i++) {
            SVC_FMP4_MUX_HDLR_s *Hdlr = &g_Fmp4MuxMgr.Hdlr[i];
            UINT32 Len = AmbaUtility_StringPrintUInt32(MutexName[i], (UINT32)sizeof(MutexName[i]), "Fmp4Mux%02u", 1, &i);
            Rval = K2M(AmbaKAL_MutexCreate(&Hdlr->Mutex, MutexName[i]));
            if ((Rval != MUXER_OK) || (i >= MAX_FMP4MUX_NUM)) {
                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Create handler's mutex failed!", __func__, NULL, NULL, NULL, NULL);
                if (Rval == MUXER_OK) {
                    Rval = MUXER_ERR_FATAL_ERROR;
                }
                break;
            }
            AmbaMisra_TouchUnused(&Len);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Create mutex failed!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 *  Initialize the Fmp4Mux module.
 *
 *  @param [in] Config The configuration used to initialize the module
 *  @return 0 - OK, others - MUXER_ERR_XXXX
 */
UINT32 SvcFmp4Mux_Init(const SVC_FMP4_MUX_INIT_CFG_s *Config)
{
    UINT32 Rval;
    static UINT8 g_Fmp4MuxMgrInit = 0U;
    Rval = SvcFmp4Mux_Init_ParamCheck(Config);
    if (Rval == MUXER_OK) {
        if (g_Fmp4MuxMgrInit == 0U) {
            SvcFmp4Mux_InitOperation();
            Rval = W2M(AmbaWrap_memset(Config->Buffer, 0, Config->BufferSize));
            if (Rval == MUXER_OK) {
                AmbaMisra_TypeCast(&g_Fmp4MuxMgr.Hdlr, &Config->Buffer);
                g_Fmp4MuxMgr.MaxHdlr = Config->MaxHdlr;
                g_Fmp4MuxMgrInit = 1U;
                Rval = CreateMutex();
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Already initiated!", __func__, NULL, NULL, NULL, NULL);
            Rval = MUXER_ERR_FATAL_ERROR;
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Invalid agruement!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 SvcFmp4Mux_GetDefaultCfg_ParamCheck(const SVC_FMP4_MUX_CFG_s *Config)
{
    UINT32 Rval = MUXER_ERR_INVALID_ARG;
    if (Config != NULL) {
        Rval = MUXER_OK;
    }
    return Rval;
}

/**
 *  Get the default configuration of a Fmp4Mux handler.
 *
 *  @param [out] Config The returned configuration
 *  @return 0 - OK, others - MUXER_ERR_XXXX
 */
UINT32 SvcFmp4Mux_GetDefaultCfg(SVC_FMP4_MUX_CFG_s *Config)
{
    UINT32 Rval;
    Rval = SvcFmp4Mux_GetDefaultCfg_ParamCheck(Config);
    if (Rval == MUXER_OK) {
        Rval = W2M(AmbaWrap_memset(Config, 0, sizeof(SVC_FMP4_MUX_CFG_s)));
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Invalid agruement!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 GetHdlr(SVC_FMP4_MUX_HDLR_s **Hdlr)
{
    UINT32 Rval = MUXER_ERR_FATAL_ERROR;
    UINT32 i;
    for (i = 0; i < g_Fmp4MuxMgr.MaxHdlr; i++) {
        SVC_FMP4_MUX_HDLR_s *Fmp4Mux = &g_Fmp4MuxMgr.Hdlr[i];
        if (Fmp4Mux->Used == 0U) {
            Fmp4Mux->Used = 1U;
            *Hdlr = Fmp4Mux;
            Rval = MUXER_OK;
            break;
        }
    }
    if (i == g_Fmp4MuxMgr.MaxHdlr) {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Get Hdlr fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 RelHdlr(SVC_FMP4_MUX_HDLR_s *Hdlr)
{
    UINT32 Rval = MUXER_ERR_INVALID_ARG;
    UINT32 i;
    for (i = 0; i < g_Fmp4MuxMgr.MaxHdlr; i++) {
        const SVC_FMP4_MUX_HDLR_s *Fmp4Mux = &g_Fmp4MuxMgr.Hdlr[i];
        if (Fmp4Mux == Hdlr) {
            if (Hdlr->Used == 1U) {
                Hdlr->Used = 0U;
                Rval = MUXER_OK;
            } else {
                // error...
                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Fmp4Mux idle!", __func__, NULL, NULL, NULL, NULL);
                Rval = MUXER_ERR_FATAL_ERROR;
            }
            break;
        }
    }
    if (i == g_Fmp4MuxMgr.MaxHdlr) {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Get Hdlr fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 SvcFmp4Mux_CreateImpl(const SVC_FMP4_MUX_CFG_s *Config, SVC_FMP4_MUX_HDLR_s **Hdlr)
{
    UINT32 Rval;
    SVC_FMP4_MUX_HDLR_s *Fmp4Mux;
    Rval = GetHdlr(&Fmp4Mux);
    if (Rval == MUXER_OK) {
        Rval = K2M(AmbaKAL_MutexTake(&Fmp4Mux->Mutex, SVC_FORMAT_WAIT_TIMEOUT));
        if (Rval == MUXER_OK) {
            SvcMuxer_InitMovieMuxHdlr(&Fmp4Mux->Hdlr, &g_Fmp4Mux, Config->Stream);
            Rval = W2M(AmbaWrap_memset(&Fmp4Mux->MoovAtom, 0, sizeof(ISO_MOOV_BOX_s)));
            if (Rval == MUXER_OK) {
                Fmp4Mux->MoovAtom.UdtaAtom.UserData                  = Config->UserData;
                Fmp4Mux->MoovAtom.UdtaAtom.UserDataBufferSize        = Config->UserDataSize;
                Fmp4Mux->MoovAtom.MvexAtom.EnableMehd                = Config->EnableMehd;
                Fmp4Mux->MoovAtom.MvexAtom.MehdAtom.FragmentDuration = Config->MehdDefDuration;
                Fmp4Mux->MoovAtom.MvhdAtom.TimeScale                 = Config->MvhdTimeScale;
                *Hdlr = Fmp4Mux;
            }
            if (AmbaKAL_MutexGive(&Fmp4Mux->Mutex) != OK) {
                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Give mutex fail!", __func__, NULL, NULL, NULL, NULL);
                Rval = MUXER_ERR_FATAL_ERROR;
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Take mutex fail!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        Rval = MUXER_ERR_FATAL_ERROR;
    }
    return Rval;
}

static UINT32 SvcFmp4Mux_Create_ParamCheck(const SVC_FMP4_MUX_CFG_s *Config, SVC_MUX_FORMAT_HDLR_s * const *Hdlr)
{
    UINT32 Rval = MUXER_ERR_INVALID_ARG;
    if (Config != NULL) {
        if (Hdlr != NULL) {
            Rval = MUXER_OK;
        }
    }
    return Rval;
}

/**
 *  Create an Fmp4Mux handler.
 *
 *  @param [in] Config The configuration used to create an Fmp4Mux handler
 *  @param [out] Hdlr The returned Fmp4Mux handler
 *  @return 0 - OK, others - MUXER_ERR_XXXX
 */
UINT32 SvcFmp4Mux_Create(const SVC_FMP4_MUX_CFG_s *Config, SVC_MUX_FORMAT_HDLR_s **Hdlr)
{
    UINT32 Rval;
    Rval = SvcFmp4Mux_Create_ParamCheck(Config, Hdlr);
    if (Rval == MUXER_OK) {
        Rval = K2M(AmbaKAL_MutexTake(&g_Fmp4MuxMgr.Mutex, SVC_FORMAT_WAIT_TIMEOUT));
        if (Rval == MUXER_OK) {
            SVC_FMP4_MUX_HDLR_s *Fmp4Mux;
            Rval = SvcFmp4Mux_CreateImpl(Config, &Fmp4Mux);
            if (Rval == MUXER_OK) {
                *Hdlr = &Fmp4Mux->Hdlr;
            }
            if (AmbaKAL_MutexGive(&g_Fmp4MuxMgr.Mutex) != OK) {
                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Give mutex fail!", __func__, NULL, NULL, NULL, NULL);
                Rval = MUXER_ERR_FATAL_ERROR;
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Take mutex fail!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Invalid agruement!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 SvcFmp4Mux_DeleteImpl(SVC_FMP4_MUX_HDLR_s *Hdlr)
{
    UINT32 Rval;
    Rval = K2M(AmbaKAL_MutexTake(&Hdlr->Mutex, SVC_FORMAT_WAIT_TIMEOUT));
    if (Rval == MUXER_OK) {
        Rval = RelHdlr(Hdlr);
        if (AmbaKAL_MutexGive(&Hdlr->Mutex) != OK) {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Give mutex fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = MUXER_ERR_FATAL_ERROR;
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Take mutex fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 SvcFmp4Mux_Delete_ParamCheck(const SVC_MUX_FORMAT_HDLR_s *Hdlr)
{
    UINT32 Rval = MUXER_ERR_INVALID_ARG;
    if (Hdlr != NULL) {
        Rval = MUXER_OK;
    }
    return Rval;
}

/**
 *  Delete a Fmp4Mux handler.
 *
 *  @param [in] Hdlr The Fmp4Mux handler being deleted
 *  @return 0 - OK, others - MUXER_ERR_XXXX
 */
UINT32 SvcFmp4Mux_Delete(SVC_MUX_FORMAT_HDLR_s *Hdlr)
{
    UINT32 Rval;
    Rval = SvcFmp4Mux_Delete_ParamCheck(Hdlr);
    if (Rval == MUXER_OK) {
        Rval = K2M(AmbaKAL_MutexTake(&g_Fmp4MuxMgr.Mutex, SVC_FORMAT_WAIT_TIMEOUT));
        if (Rval == MUXER_OK) {
            SVC_FMP4_MUX_HDLR_s *Fmp4Mux;
            AmbaMisra_TypeCast(&Fmp4Mux, &Hdlr);
            Rval = SvcFmp4Mux_DeleteImpl(Fmp4Mux);
            if (AmbaKAL_MutexGive(&g_Fmp4MuxMgr.Mutex) != OK) {
                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Give mutex fail!", __func__, NULL, NULL, NULL, NULL);
                Rval = MUXER_ERR_FATAL_ERROR;
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Take mutex fail!", __func__, NULL, NULL, NULL, NULL);
        }
        AmbaMisra_TouchUnused(Hdlr);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Invalid agruement!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 SvcFmp4Mux_Open_ParamCheck(const SVC_MUX_FORMAT_HDLR_s *Hdlr)
{
    UINT32 Rval = MUXER_ERR_INVALID_ARG;
    if (Hdlr != NULL) {
        Rval = MUXER_OK;
    }
    return Rval;
}

static UINT32 SvcFmp4Mux_Open(SVC_MUX_FORMAT_HDLR_s *Hdlr)
{
    UINT32 Rval;
    Rval = SvcFmp4Mux_Open_ParamCheck(Hdlr);
    if (Rval == MUXER_OK) {
        SVC_FMP4_MUX_HDLR_s *Fmp4Mux;
        AmbaMisra_TypeCast(&Fmp4Mux, &Hdlr);
        if (Hdlr->Media->MediaType == SVC_MEDIA_INFO_MOVIE) {
            Rval = K2M(AmbaKAL_MutexTake(&Fmp4Mux->Mutex, SVC_FORMAT_WAIT_TIMEOUT));
            if (Rval == MUXER_OK) {
                if (Fmp4Mux->Open == 0U) {
                    Rval = F2M(Fmp4Mux_OpenImpl(&Fmp4Mux->Fragment, &Fmp4Mux->Hdlr));
                    if (Rval == MUXER_OK) {
                        Fmp4Mux->Open = 1U;
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Already opened!", __func__, NULL, NULL, NULL, NULL);
                    Rval = MUXER_ERR_FATAL_ERROR;
                }
                if (AmbaKAL_MutexGive(&Fmp4Mux->Mutex) != OK) {
                    AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Give mutex fail!", __func__, NULL, NULL, NULL, NULL);
                    Rval = MUXER_ERR_FATAL_ERROR;
                }
            } else {
                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Take mutex fail!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Not a movie!", __func__, NULL, NULL, NULL, NULL);
        }
        AmbaMisra_TouchUnused(Hdlr);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Invalid agruement!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 SvcFmp4Mux_Close_ParamCheck(const SVC_MUX_FORMAT_HDLR_s *Hdlr, UINT8 Mode)
{
    UINT32 Rval = MUXER_ERR_INVALID_ARG;
    if (Hdlr != NULL) {
        if (Mode <= 3U) {
            Rval = MUXER_OK;
        }
    }
    return Rval;
}

static UINT32 SvcFmp4Mux_Close(SVC_MUX_FORMAT_HDLR_s *Hdlr, UINT8 Mode)
{
    UINT32 Rval;
    Rval = SvcFmp4Mux_Close_ParamCheck(Hdlr, Mode);
    if (Rval == MUXER_OK) {
        SVC_FMP4_MUX_HDLR_s *Fmp4Mux;
        AmbaMisra_TypeCast(&Fmp4Mux, &Hdlr);
        Rval = K2M(AmbaKAL_MutexTake(&Fmp4Mux->Mutex, SVC_FORMAT_WAIT_TIMEOUT));
        if (Rval == MUXER_OK) {
            if (Fmp4Mux->Open == 1U) {
                Rval = F2M(Fmp4Mux_CloseImpl(&Fmp4Mux->Fragment, &Fmp4Mux->Hdlr, Mode));
                Fmp4Mux->Open = 0U;
            } else {
                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Not open yet!", __func__, NULL, NULL, NULL, NULL);
                Rval = MUXER_ERR_FATAL_ERROR;
            }
            if (AmbaKAL_MutexGive(&Fmp4Mux->Mutex) != OK) {
                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Give mutex fail!", __func__, NULL, NULL, NULL, NULL);
                Rval = MUXER_ERR_FATAL_ERROR;
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Take mutex fail!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Invalid agruement!", __func__, NULL, NULL, NULL, NULL);
    }
    AmbaMisra_TouchUnused(Hdlr);
    return Rval;
}

static UINT32 SvcFmp4Mux_Process_ParamCheck(const SVC_MUX_FORMAT_HDLR_s *Hdlr, UINT64 Param, const UINT8 *Event)
{
    UINT32 Rval = MUXER_ERR_INVALID_ARG;
    if ((Hdlr != NULL) && (Param > 0U) && (Event != NULL)) {
        Rval = MUXER_OK;
    }
    return Rval;
}

static UINT32 SvcFmp4Mux_Process(SVC_MUX_FORMAT_HDLR_s *Hdlr, UINT64 Param, UINT8 *Event)
{
    UINT32 Rval = MUXER_ERR_INVALID_ARG;
    Rval = SvcFmp4Mux_Process_ParamCheck(Hdlr, Param, Event);
    if (Rval == MUXER_OK) {
        SVC_FMP4_MUX_HDLR_s *Fmp4Mux;
        AmbaMisra_TypeCast(&Fmp4Mux, &Hdlr);
        *Event = SVC_MUXER_EVENT_NONE;
        Rval = K2M(AmbaKAL_MutexTake(&Fmp4Mux->Mutex, SVC_FORMAT_WAIT_TIMEOUT));
        if (Rval == MUXER_OK) {
            UINT8 TempEvent = FORMAT_EVENT_NONE;
            Rval = F2M(Fmp4Mux_ProcessImpl(&Fmp4Mux->Fragment, Hdlr->Media, Hdlr->Stream, &Fmp4Mux->MoovAtom, Param, Hdlr->MaxDuration, Hdlr->MaxSize, &TempEvent));
            if (Rval == MUXER_OK) {
                switch (TempEvent) {
                case FORMAT_EVENT_NONE:
                    *Event = SVC_MUXER_EVENT_NONE;
                    break;
                case FORMAT_EVENT_REACH_END:
                    *Event = SVC_MUXER_EVENT_REACH_END;
                    break;
                case FORMAT_EVENT_REACH_LIMIT:
                    *Event = SVC_MUXER_EVENT_REACH_LIMIT;
                    break;
                default:
                    Rval = MUXER_ERR_FATAL_ERROR;
                    break;
                }
            }
            if (AmbaKAL_MutexGive(&Fmp4Mux->Mutex) != OK) {
                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Give mutex fail!", __func__, NULL, NULL, NULL, NULL);
                Rval = MUXER_ERR_FATAL_ERROR;
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Take mutex fail!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "Process: incorrect Param %u!", (UINT32)Param, 0U, 0U, 0U, 0U);
    }
    AmbaMisra_TouchUnused(Hdlr);
    return Rval;
}

static UINT32 SvcFmp4Mux_GetMediaTime_ParamCheck(const SVC_MUX_FORMAT_HDLR_s *Hdlr, const UINT64 *Time)
{
    UINT32 Rval = MUXER_ERR_INVALID_ARG;
    if ((Hdlr != NULL) && (Time != NULL)) {
        Rval = MUXER_OK;
    }
    return Rval;
}

static UINT32 SvcFmp4Mux_GetMediaTime(SVC_MUX_FORMAT_HDLR_s *Hdlr, UINT64 *Time)
{
    UINT32 Rval;
    Rval = SvcFmp4Mux_GetMediaTime_ParamCheck(Hdlr, Time);
    if (Rval == MUXER_OK) {
        SVC_FMP4_MUX_HDLR_s *Fmp4Mux;
        *Time = SVC_FORMAT_MAX_TIMESTAMP;
        AmbaMisra_TypeCast(&Fmp4Mux, &Hdlr);
        Rval = K2M(AmbaKAL_MutexTake(&Fmp4Mux->Mutex, SVC_FORMAT_WAIT_TIMEOUT));
        if (Rval == MUXER_OK) {
            const SVC_MOVIE_INFO_s *Movie;
            AmbaMisra_TypeCast(&Movie, &Hdlr->Media);
            if (Fmp4Mux->Open == 0U) {    /* not opened */
                *Time = SvcFormat_GetMediaTime(Movie->VideoTrack, Movie->VideoTrackCount, Movie->AudioTrack, Movie->AudioTrackCount, Movie->TextTrack, Movie->TextTrackCount, SVC_FORMAT_DIR_FORWARD);
            } else {
                UINT8 Tmp;
                const SVC_VIDEO_TRACK_INFO_s *Video = SvcFormat_GetDefaultVideoTrack(Fmp4Mux->Fragment.VideoTrack, Movie->VideoTrackCount, &Tmp);
                if ((Video != NULL) && (Video->Info.DTS < SVC_FORMAT_MAX_TIMESTAMP)) {
                    *Time = SVC_FORMAT_DTS_TO_TIME(Video->Info.DTS, Video->Info.TimeScale);
                } else {
                    *Time = SVC_FORMAT_MAX_TIMESTAMP;
                }
            }
            if (AmbaKAL_MutexGive(&Fmp4Mux->Mutex) != OK) {
                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Give mutex fail!", __func__, NULL, NULL, NULL, NULL);
                Rval = MUXER_ERR_FATAL_ERROR;
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Take mutex fail!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Invalid agruement!", __func__, NULL, NULL, NULL, NULL);
    }
    AmbaMisra_TouchUnused(Hdlr);
    return Rval;
}

static UINT32 SvcFmp4Mux_SetUdtaImpl(SVC_MUX_FORMAT_HDLR_s *Hdlr, UINT32 Size, const UINT8 *Data)
{
    UINT32 Rval = MUXER_OK;
    SVC_FMP4_MUX_HDLR_s * Fmp4Mux;
    AmbaMisra_TypeCast(&Fmp4Mux, &Hdlr);
    if ((Data == NULL) || (Size == 0U)) {
        Fmp4Mux->MoovAtom.UdtaAtom.UserDataSize = 0;
    } else {
        if (Size <= Fmp4Mux->MoovAtom.UdtaAtom.UserDataBufferSize) {
            Rval = W2M(AmbaWrap_memcpy(Fmp4Mux->MoovAtom.UdtaAtom.UserData, Data, Size));
            Fmp4Mux->MoovAtom.UdtaAtom.UserDataSize = Size;
        } else {
            AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "set user Data Size failed: too large %u %u", Size, Fmp4Mux->MoovAtom.UdtaAtom.UserDataSize, 0U, 0U, 0U);
            Rval = MUXER_ERR_INVALID_ARG;
        }
    }
    AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "[Fmp4Mux]Set user Data Size = %u", Size, 0U, 0U, 0U, 0U);
    AmbaMisra_TouchUnused(Hdlr);
    return Rval;
}

static UINT32 SvcFmp4Mux_SetUdta_ParamCheck(const SVC_MUX_FORMAT_HDLR_s *Hdlr, UINT32 Size, const UINT8 *Data)
{
    UINT32 Rval = MUXER_ERR_INVALID_ARG;
    if (Hdlr != NULL) {
        const SVC_FMP4_MUX_HDLR_s * Fmp4Mux;
        AmbaMisra_TypeCast(&Fmp4Mux, &Hdlr);
        if (Data != NULL) {
            if ((Size > 0U) && (Size <= Fmp4Mux->MoovAtom.UdtaAtom.UserDataBufferSize)) {
                Rval = MUXER_OK;
            }
        }
    }
    return Rval;
}

static UINT32 SvcFmp4Mux_SetUdta(SVC_MUX_FORMAT_HDLR_s *Hdlr, UINT32 Size, const UINT8 *Data)
{
    UINT32 Rval;
    Rval = SvcFmp4Mux_SetUdta_ParamCheck(Hdlr, Size, Data);
    if (Rval == MUXER_OK) {
        SVC_FMP4_MUX_HDLR_s * Fmp4Mux;
        AmbaMisra_TypeCast(&Fmp4Mux, &Hdlr);
        Rval = K2M(AmbaKAL_MutexTake(&Fmp4Mux->Mutex, SVC_FORMAT_WAIT_TIMEOUT));
        if (Rval == MUXER_OK) {
            Rval = SvcFmp4Mux_SetUdtaImpl(Hdlr, Size, Data);
            if (AmbaKAL_MutexGive(&Fmp4Mux->Mutex) != OK) {
                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Give mutex fail!", __func__, NULL, NULL, NULL, NULL);
                Rval = MUXER_ERR_FATAL_ERROR;
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Take mutex fail!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Invalid agruement!", __func__, NULL, NULL, NULL, NULL);
    }
    AmbaMisra_TouchUnused(Hdlr);
    return Rval;
}

static void SvcFmp4Mux_InitOperation(void)
{
    g_Fmp4Mux.Open = SvcFmp4Mux_Open;
    g_Fmp4Mux.Close = SvcFmp4Mux_Close;
    g_Fmp4Mux.Process = SvcFmp4Mux_Process;
    g_Fmp4Mux.GetMediaTime = SvcFmp4Mux_GetMediaTime;
    g_Fmp4Mux.SetUserData = SvcFmp4Mux_SetUdta;
}

