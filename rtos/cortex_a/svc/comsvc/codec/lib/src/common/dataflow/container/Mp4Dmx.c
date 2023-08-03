/**
 * @file Mp4Dmx.c
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
#include "iso/Mp4DmxImpl.h"
#include "format/SvcMp4Dmx.h"
#include <AmbaDef.h>

/**
 * Mp4Dmx handler
 */
typedef struct {
    SVC_DMX_FORMAT_HDLR_s Hdlr; /**< Demuxing format handler. Must be the first member. */
    AMBA_KAL_MUTEX_t Mutex;     /**< Mutex */
    MP4_IDX_s Mp4Idx;           /**< Mp4 index */
    SVC_FRAME_FEEDER_INFO_s FeedInfo[SVC_FORMAT_MAX_TRACK_PER_MEDIA];   /**< Feeder information */
    UINT32 MaxIdxNum;       /**< The maximum number of the indexes of an Mp4Dmx handler */
    UINT8 Open;             /**< If the handler is opened */
    UINT8 Used;             /**< If the handler is used */
} SVC_MP4_DMX_HDLR_s;

/**
 * Mp4Dmx manager
 */
typedef struct {
    AMBA_KAL_MUTEX_t Mutex;     /**< Mutex */
    SVC_MP4_DMX_HDLR_s *Hdlr;   /**< MP4 demuxer handlers */
    UINT8 MaxHdlr;              /**< Maximum number of handlers */
} SVC_MP4_DMX_MGR_s;

static UINT32 SvcMp4Dmx_Open(SVC_DMX_FORMAT_HDLR_s *Hdlr);
static UINT32 SvcMp4Dmx_Close(SVC_DMX_FORMAT_HDLR_s *Hdlr);
static UINT32 SvcMp4Dmx_Process(SVC_DMX_FORMAT_HDLR_s *Hdlr, UINT64 Param, UINT8 *Event);
static UINT32 SvcMp4Dmx_GetMediaTime(SVC_DMX_FORMAT_HDLR_s *Hdlr, UINT64 *Time);
static UINT32 SvcMp4Dmx_Seek(SVC_DMX_FORMAT_HDLR_s *Hdlr, UINT64 TargetTime, UINT32 Direction, UINT32 Speed);
static UINT32 SvcMp4Dmx_FeedFrame(SVC_DMX_FORMAT_HDLR_s *Hdlr, UINT8 TrackId, UINT64 TargetTime, UINT8 FrameType);

static SVC_MP4_DMX_MGR_s g_Mp4DmxMgr GNU_SECTION_NOZEROINIT;

static inline UINT32 SvcMp4Dmx_GetInitDefaultCfg_ParamCheck(const SVC_MP4_DMX_INIT_CFG_s *Config)
{
    UINT32 Rval = DEMUXER_ERR_INVALID_ARG;
    if (Config != NULL) {
        Rval = DEMUXER_OK;
    }
    return Rval;
}

/**
 * Get the default configuration for initializing the Mp4Dmx module.
 *
 * @param [out] Config The returned configuration
 * @return 0 - OK, others - DEMUXER_ERR_XXX
 */
UINT32 SvcMp4Dmx_GetInitDefaultCfg(SVC_MP4_DMX_INIT_CFG_s *Config)
{
    UINT32 Rval = SvcMp4Dmx_GetInitDefaultCfg_ParamCheck(Config);
    if (Rval == DEMUXER_OK) {
        Rval = W2D(AmbaWrap_memset(Config, 0, sizeof(SVC_MP4_DMX_INIT_CFG_s)));
        if (Rval == DEMUXER_OK) {
            Config->MaxHdlr = 1;
            Rval = SvcMp4Dmx_GetInitBufferSize(Config->MaxHdlr, &Config->BufferSize);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Invalid argument!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static inline UINT32 SvcMp4Dmx_GetInitBufferSize_ParamCheck(UINT8 MaxHdlr, const UINT32 *BufferSize)
{
    UINT32 Rval = DEMUXER_ERR_INVALID_ARG;
    if ((MaxHdlr > 0U) && (BufferSize != NULL)) {
        Rval = DEMUXER_OK;
    }
    return Rval;
}

/**
 * Get the required buffer size for initializing the Mp4Dmx module.
 *
 * @param [in] MaxHdlr The maximum number of Mp4Dmx handlers
 * @param [in] BufferSize The work buffer size of the Mp4Dmx module.
 * @return 0 - OK, others - DEMUXER_ERR_XXX
 */
UINT32 SvcMp4Dmx_GetInitBufferSize(UINT8 MaxHdlr, UINT32 *BufferSize)
{
    UINT32 Rval = SvcMp4Dmx_GetInitBufferSize_ParamCheck(MaxHdlr, BufferSize);
    if (Rval == DEMUXER_OK) {
        *BufferSize = GetAlignedValU32((UINT32)MaxHdlr * (UINT32)sizeof(SVC_MP4_DMX_HDLR_s), (UINT32)AMBA_CACHE_LINE_SIZE);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Invalid argument!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 CheckInitParam(const SVC_MP4_DMX_INIT_CFG_s *Config)
{
    UINT32 Rval = DEMUXER_OK;
    UINT32 BufferSize;
    if (Config->MaxHdlr == 0U) {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s incorrect handler number!", __func__, NULL, NULL, NULL, NULL);
        Rval = DEMUXER_ERR_INVALID_ARG;
    }
    if (Config->Buffer == NULL) {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s buffer is NULL!", __func__, NULL, NULL, NULL, NULL);
        Rval = DEMUXER_ERR_INVALID_ARG;
    }
    if (Rval == DEMUXER_OK) {
        Rval = SvcMp4Dmx_GetInitBufferSize(Config->MaxHdlr, &BufferSize);
        if (Rval == DEMUXER_OK) {
            if (Config->BufferSize != BufferSize) {
                AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "CheckInitParam incorrect buffer size (%u, %u)!", Config->BufferSize, BufferSize, 0, 0, 0);
                Rval = DEMUXER_ERR_INVALID_ARG;
            }
        }
    }
    return Rval;
}

#define MAX_MP4DMX_NUM  (32U)   /**< Maximum number of Mp4Dmx handlers */
static UINT32 CreateMutex(void)
{
    static char DmxMgrMutexName[16] = "Mp4Dmx_Mgr";
    UINT32 Rval = K2D(AmbaKAL_MutexCreate(&g_Mp4DmxMgr.Mutex, DmxMgrMutexName));
    if (Rval == DEMUXER_OK) {
        static char MutexName[MAX_MP4DMX_NUM][32];
        UINT32 i;
        SVC_MP4_DMX_HDLR_s *Hdlr;
        for (i = 0; i < g_Mp4DmxMgr.MaxHdlr; i++) {
            UINT32 Len;
            Hdlr = &g_Mp4DmxMgr.Hdlr[i];
            Len = AmbaUtility_StringPrintUInt32(MutexName[i], (UINT32)sizeof(MutexName[i]), "Mp4Dmx%02u", 1, &i);
            Rval = K2D(AmbaKAL_MutexCreate(&Hdlr->Mutex, MutexName[i]));
            if ((Rval != DEMUXER_OK) || (i >= MAX_MP4DMX_NUM)) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Create handler's mutex failed!", __func__, NULL, NULL, NULL, NULL);
                if (Rval == DEMUXER_OK) {
                    Rval = DEMUXER_ERR_FATAL_ERROR;
                }
                break;
            }
            AmbaMisra_TouchUnused(&Len);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Create mutex failed!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static inline UINT32 SvcMp4Dmx_Init_ParamCheck(const SVC_MP4_DMX_INIT_CFG_s *Config)
{
    UINT32 Rval = DEMUXER_ERR_INVALID_ARG;
    if (Config != NULL) {
        Rval = CheckInitParam(Config);
    }
    return Rval;
}

/**
 * Initialize the Mp4Dmx module.
 *
 * @param [in] Config The configuration used to initialize the module
 * @return 0 - OK, others - DEMUXER_ERR_XXX
 */
UINT32 SvcMp4Dmx_Init(SVC_MP4_DMX_INIT_CFG_s *Config)
{
    UINT32 Rval = SvcMp4Dmx_Init_ParamCheck(Config);
    if (Rval == DEMUXER_OK) {
        static UINT8 g_Mp4DmxMgrInit = 0U;
        if (g_Mp4DmxMgrInit == 0U) {
            Rval = W2D(AmbaWrap_memset(Config->Buffer, 0, Config->BufferSize));
            if (Rval == DEMUXER_OK) {
                AmbaMisra_TypeCast(&g_Mp4DmxMgr.Hdlr, &Config->Buffer);
                g_Mp4DmxMgr.MaxHdlr = Config->MaxHdlr;
                g_Mp4DmxMgrInit = 1U;
                Rval = CreateMutex();
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Already initiated!", __func__, NULL, NULL, NULL, NULL);
            Rval = DEMUXER_ERR_FATAL_ERROR;
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Invalid argument!", __func__, NULL, NULL, NULL, NULL);
    }
    AmbaMisra_TouchUnused(Config);
    return Rval;
}

static inline UINT32 SvcMp4Dmx_GetDefaultCfg_ParamCheck(const SVC_MP4_DMX_CFG_s *Config)
{
    UINT32 Rval = DEMUXER_ERR_INVALID_ARG;
    if (Config != NULL) {
        Rval = DEMUXER_OK;
    }
    return Rval;
}

/**
 * Get the default configuration of an Mp4Dmx handler.
 *
 * @param [out] Config The returned configuration
 * @return 0 - OK, others - DEMUXER_ERR_XXX
 */
UINT32 SvcMp4Dmx_GetDefaultCfg(SVC_MP4_DMX_CFG_s *Config)
{
    UINT32 Rval = SvcMp4Dmx_GetDefaultCfg_ParamCheck(Config);
    if (Rval == DEMUXER_OK) {
        Rval = W2D(AmbaWrap_memset(Config, 0, sizeof(SVC_MP4_DMX_CFG_s)));
        Config->MaxIdxNum = 512;
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Invalid argument!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 GetHdlr(SVC_MP4_DMX_HDLR_s **Hdlr)
{
    UINT32 Rval = DEMUXER_ERR_FATAL_ERROR;
    UINT32 i;
    SVC_MP4_DMX_HDLR_s *Mp4Dmx;
    for (i = 0; i < g_Mp4DmxMgr.MaxHdlr; i++) {
        Mp4Dmx = &g_Mp4DmxMgr.Hdlr[i];
        if (Mp4Dmx->Used == 0U) {
            Mp4Dmx->Used = 1U;
            *Hdlr = Mp4Dmx;
            Rval = DEMUXER_OK;
            break;
        }
    }
    if (i == g_Mp4DmxMgr.MaxHdlr) {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Get Hdlr fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 RelHdlr(SVC_MP4_DMX_HDLR_s *Hdlr)
{
    UINT32 Rval = DEMUXER_ERR_INVALID_ARG;
    UINT32 i;
    const SVC_MP4_DMX_HDLR_s *Mp4Dmx;
    for (i = 0; i < g_Mp4DmxMgr.MaxHdlr; i++) {
        Mp4Dmx = &g_Mp4DmxMgr.Hdlr[i];
        if (Mp4Dmx == Hdlr) {
            if (Hdlr->Used != 0U) {
                Hdlr->Used = 0U;
                Rval = DEMUXER_OK;
            } else {
                // error...
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Mp4Dmx idle!", __func__, NULL, NULL, NULL, NULL);
                Rval = DEMUXER_ERR_FATAL_ERROR;
            }
            break;
        }
    }
    if (i == g_Mp4DmxMgr.MaxHdlr) {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Get Hdlr fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 SvcMp4Dmx_CreateImpl(SVC_MP4_DMX_CFG_s *Config, SVC_MP4_DMX_HDLR_s **Hdlr)
{
    UINT32 Rval;
    SVC_MP4_DMX_HDLR_s *Mp4Dmx;
    static SVC_DMX_FORMAT_s g_Mp4Dmx = {
        SvcMp4Dmx_Open,
        SvcMp4Dmx_Close,
        SvcMp4Dmx_Process,
        SvcMp4Dmx_GetMediaTime,
        SvcMp4Dmx_Seek,
        SvcMp4Dmx_FeedFrame
    };
    Rval = GetHdlr(&Mp4Dmx);
    if (Rval == DEMUXER_OK) {
        Rval = K2D(AmbaKAL_MutexTake(&Mp4Dmx->Mutex, SVC_FORMAT_WAIT_TIMEOUT));
        if (Rval == DEMUXER_OK) {
            SvcDemuxer_InitMovieDmxHdlr(&Mp4Dmx->Hdlr, &g_Mp4Dmx, Config->Stream);
            Mp4Dmx->MaxIdxNum = Config->MaxIdxNum;
            *Hdlr = Mp4Dmx;
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Take mutex fail!", __func__, NULL, NULL, NULL, NULL);
        }
        if (AmbaKAL_MutexGive(&Mp4Dmx->Mutex) != OK) {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Give mutex fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = DEMUXER_ERR_FATAL_ERROR;
        }
    } else {
        Rval = DEMUXER_ERR_FATAL_ERROR;
    }
    AmbaMisra_TouchUnused(Config);
    return Rval;
}

static inline UINT32 SvcMp4Dmx_Create_ParamCheck(const SVC_MP4_DMX_CFG_s *Config, SVC_DMX_FORMAT_HDLR_s *const *Hdlr)
{
    UINT32 Rval = DEMUXER_ERR_INVALID_ARG;
    if ((Config != NULL) && (Hdlr != NULL)) {
        if (Config->Stream != NULL) {
            Rval = DEMUXER_OK;
        }
    }
    return Rval;
}

/**
 * Create an Mp4Dmx handler.
 *
 * @param [in] Config The configuration used to create an Mp4Dmx handler
 * @param [out] Hdlr The returned Mp4Dmx handler
 * @return 0 - OK, others - DEMUXER_ERR_XXX
 */
UINT32 SvcMp4Dmx_Create(SVC_MP4_DMX_CFG_s *Config, SVC_DMX_FORMAT_HDLR_s **Hdlr)
{
    UINT32 Rval = SvcMp4Dmx_Create_ParamCheck(Config, Hdlr);
    if (Rval == DEMUXER_OK) {
        Rval = K2D(AmbaKAL_MutexTake(&g_Mp4DmxMgr.Mutex, SVC_FORMAT_WAIT_TIMEOUT));
        if (Rval == DEMUXER_OK) {
            SVC_MP4_DMX_HDLR_s *Mp4Dmx;
            Rval = SvcMp4Dmx_CreateImpl(Config, &Mp4Dmx);
            if (Rval == DEMUXER_OK) {
                *Hdlr = &Mp4Dmx->Hdlr;
            }
            if (AmbaKAL_MutexGive(&g_Mp4DmxMgr.Mutex) != OK) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s AmbaKAL_MutexGive() failed", __func__, NULL, NULL, NULL, NULL);
                Rval = DEMUXER_ERR_FATAL_ERROR;
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Take mutex fail!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Invalid argument!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 SvcMp4Dmx_DeleteImpl(SVC_MP4_DMX_HDLR_s *Hdlr)
{
    UINT32 Rval = K2D(AmbaKAL_MutexTake(&Hdlr->Mutex, SVC_FORMAT_WAIT_TIMEOUT));
    if (Rval == DEMUXER_OK) {
        Rval = RelHdlr(Hdlr);
        if (AmbaKAL_MutexGive(&Hdlr->Mutex) != OK) {  /* delete mutex */
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Give mutex failed!", __func__, NULL, NULL, NULL, NULL);
            Rval = DEMUXER_ERR_FATAL_ERROR;
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Take mutex fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static inline UINT32 SvcMp4Dmx_Delete_ParamCheck(const SVC_DMX_FORMAT_HDLR_s *Hdlr)
{
    UINT32 Rval = DEMUXER_ERR_INVALID_ARG;
    if (Hdlr != NULL) {
        Rval = DEMUXER_OK;
    }
    return Rval;
}

/**
 * Delete an Mp4Dmx handler.
 *
 * @param [in] Hdlr The Mp4Dmx handler being deleted
 * @return 0 - OK, others - DEMUXER_ERR_XXX
 */
UINT32 SvcMp4Dmx_Delete(SVC_DMX_FORMAT_HDLR_s *Hdlr)
{
    UINT32 Rval = SvcMp4Dmx_Delete_ParamCheck(Hdlr);
    if (Rval == DEMUXER_OK) {
        Rval = K2D(AmbaKAL_MutexTake(&g_Mp4DmxMgr.Mutex, SVC_FORMAT_WAIT_TIMEOUT));
        if (Rval == DEMUXER_OK) {
            SVC_MP4_DMX_HDLR_s *Mp4Dmx;
            AmbaMisra_TypeCast(&Mp4Dmx, &Hdlr);
            Rval = SvcMp4Dmx_DeleteImpl(Mp4Dmx);
            if (AmbaKAL_MutexGive(&g_Mp4DmxMgr.Mutex) != OK) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Give mutex failed", __func__, NULL, NULL, NULL, NULL);
                Rval = DEMUXER_ERR_FATAL_ERROR;
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Take mutex fail!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Invalid argument!", __func__, NULL, NULL, NULL, NULL);
    }
    AmbaMisra_TouchUnused(Hdlr);
    return Rval;
}

static inline UINT32 SvcMp4Dmx_Parse_ParamCheck(const SVC_MEDIA_INFO_s *Media, const SVC_STREAM_HDLR_s *Stream)
{
    UINT32 Rval = DEMUXER_ERR_INVALID_ARG;
    if ((Media != NULL) && (Stream != NULL)) {
        Rval = DEMUXER_OK;
    }
    return Rval;
}

/**
 * Parse media data from a stream, and pack the data into a Media Info object.
 *
 * @param [in,out] Media The returned Media Info object
 * @param [in] Stream The I/O stream
 * @return 0 - OK, others - DEMUXER_ERR_XXX
 */
UINT32 SvcMp4Dmx_Parse(SVC_MEDIA_INFO_s *Media, SVC_STREAM_HDLR_s *Stream)
{
    UINT32 Rval = SvcMp4Dmx_Parse_ParamCheck(Media, Stream);
    if (Rval == DEMUXER_OK) {
        SVC_MOVIE_INFO_s *Movie;
        AmbaMisra_TypeCast(&Movie, &Media);
        AmbaMisra_TouchUnused(Media);
        /* When return DEMUXER_ERR_IO_ERROR and Media->Valid = 0U, it means invalid format. */
        Rval = F2D(Mp4Dmx_Parse(Movie, Stream));
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Invalid argument!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static inline UINT32 SvcMp4Dmx_Open_ParamCheck(const SVC_DMX_FORMAT_HDLR_s *Hdlr)
{
    UINT32 Rval = DEMUXER_ERR_INVALID_ARG;
    if (Hdlr != NULL) {
        Rval = DEMUXER_OK;
    }
    return Rval;
}

static UINT32 SvcMp4Dmx_Open(SVC_DMX_FORMAT_HDLR_s *Hdlr)
{
    UINT32 Rval = SvcMp4Dmx_Open_ParamCheck(Hdlr);
    if (Rval == DEMUXER_OK) {
        SVC_MP4_DMX_HDLR_s *Mp4Dmx;
        AmbaMisra_TypeCast(&Mp4Dmx, &Hdlr);
        if (Hdlr->Media->MediaType == SVC_MEDIA_INFO_MOVIE) {
            Rval = K2D(AmbaKAL_MutexTake(&Mp4Dmx->Mutex, SVC_FORMAT_WAIT_TIMEOUT));
            if (Rval == DEMUXER_OK) {
                if (Mp4Dmx->Open == 0U) {
                    Rval = W2D(AmbaWrap_memset(Mp4Dmx->FeedInfo, 0, sizeof(Mp4Dmx->FeedInfo)));
                    if (Rval == DEMUXER_OK) {
                        Rval = F2D(Mp4Dmx_Open(Hdlr, &Mp4Dmx->Mp4Idx, Mp4Dmx->MaxIdxNum, Mp4Dmx->FeedInfo));
                        if (Rval == DEMUXER_OK) {
                            Mp4Dmx->Open = 1U;
                        }
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Already opened!", __func__, NULL, NULL, NULL, NULL);
                }
                if (AmbaKAL_MutexGive(&Mp4Dmx->Mutex) != OK) {
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Give mutex failed", __func__, NULL, NULL, NULL, NULL);
                    Rval = DEMUXER_ERR_FATAL_ERROR;
                }
            } else {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Take mutex failed!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Not supported!", __func__, NULL, NULL, NULL, NULL);
            Rval = DEMUXER_ERR_FATAL_ERROR;
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Invalid argument!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static inline UINT32 SvcMp4Dmx_Close_ParamCheck(const SVC_DMX_FORMAT_HDLR_s *Hdlr)
{
    UINT32 Rval = DEMUXER_ERR_INVALID_ARG;
    if (Hdlr != NULL) {
        Rval = DEMUXER_OK;
    }
    return Rval;
}

static UINT32 SvcMp4Dmx_Close(SVC_DMX_FORMAT_HDLR_s *Hdlr)
{
    UINT32 Rval = SvcMp4Dmx_Close_ParamCheck(Hdlr);
    if (Rval == DEMUXER_OK) {
        SVC_MP4_DMX_HDLR_s *Mp4Dmx;
        AmbaMisra_TypeCast(&Mp4Dmx, &Hdlr);
        Rval = K2D(AmbaKAL_MutexTake(&Mp4Dmx->Mutex, SVC_FORMAT_WAIT_TIMEOUT));
        if (Rval == DEMUXER_OK) {
            if (Mp4Dmx->Open != 0U) {
                Rval = F2D(Mp4Dmx_Close(&Mp4Dmx->Mp4Idx));
                Mp4Dmx->Open = 0U;
            } else {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s No open yet!", __func__, NULL, NULL, NULL, NULL);
            }
            if (AmbaKAL_MutexGive(&Mp4Dmx->Mutex) != OK) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Give mutex failed!", __func__, NULL, NULL, NULL, NULL);
                Rval = DEMUXER_ERR_FATAL_ERROR;
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Take mutex failed!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Invalid argument!", __func__, NULL, NULL, NULL, NULL);
    }
    AmbaMisra_TouchUnused(Hdlr);
    return Rval;
}

static inline UINT32 SvcMp4Dmx_Process_ParamCheck(const SVC_DMX_FORMAT_HDLR_s *Hdlr, UINT64 Param, const UINT8 *Event)
{
    UINT32 Rval = DEMUXER_ERR_INVALID_ARG;
    if ((Hdlr != NULL) && (Event != NULL)) {
        if (((Hdlr->Direction == SVC_FORMAT_DIR_FORWARD) && (Param > 0U)) || ((Hdlr->Direction == SVC_FORMAT_DIR_BACKWARD) && (Param < SVC_FORMAT_MAX_TIMESTAMP))) {
            Rval = DEMUXER_OK;
        }
    }
    return Rval;
}

static UINT32 SvcMp4Dmx_Process(SVC_DMX_FORMAT_HDLR_s *Hdlr, UINT64 Param, UINT8 *Event)
{
    UINT32 Rval = SvcMp4Dmx_Process_ParamCheck(Hdlr, Param, Event);
    if (Rval == DEMUXER_OK) {
        SVC_MP4_DMX_HDLR_s *Mp4Dmx;
        AmbaMisra_TypeCast(&Mp4Dmx, &Hdlr);
        *Event = SVC_DEMUXER_EVENT_NONE;
        Rval = K2D(AmbaKAL_MutexTake(&Mp4Dmx->Mutex, SVC_FORMAT_WAIT_TIMEOUT));
        if (Rval == DEMUXER_OK) {
            UINT8 TempEvent = FORMAT_EVENT_NONE;
            SVC_MOVIE_INFO_s *Movie;
            AmbaMisra_TypeCast(&Movie, &Hdlr->Media);
            Rval = F2D(Mp4Dmx_Process(Movie, Hdlr->Stream, &Mp4Dmx->Mp4Idx, Mp4Dmx->FeedInfo, Param, Hdlr->Direction, Hdlr->End, &TempEvent));
            if (Rval == DEMUXER_OK) {
                switch (TempEvent) {
                case FORMAT_EVENT_NONE:
                    *Event = SVC_DEMUXER_EVENT_NONE;
                    break;
                case FORMAT_EVENT_REACH_END:
                    *Event = SVC_DEMUXER_EVENT_REACH_END;
                    break;
                default:
                    Rval = DEMUXER_ERR_FATAL_ERROR;
                    break;
                }
            }
            if (AmbaKAL_MutexGive(&Mp4Dmx->Mutex) != DEMUXER_OK) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Give mutex failed", __func__, NULL, NULL, NULL, NULL);
                Rval = DEMUXER_ERR_FATAL_ERROR;
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Take mutex failed!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Invalid argument!", __func__, NULL, NULL, NULL, NULL);
    }
    AmbaMisra_TouchUnused(Hdlr);
    return Rval;
}

static inline UINT32 SvcMp4Dmx_GetMediaTime_ParamCheck(const SVC_DMX_FORMAT_HDLR_s *Hdlr, const UINT64 *Time)
{
    UINT32 Rval = DEMUXER_ERR_INVALID_ARG;
    if ((Hdlr != NULL) && (Time != NULL)) {
        Rval = DEMUXER_OK;
    }
    return Rval;
}

static UINT32 SvcMp4Dmx_GetMediaTime(SVC_DMX_FORMAT_HDLR_s *Hdlr, UINT64 *Time)
{
    UINT32 Rval = SvcMp4Dmx_GetMediaTime_ParamCheck(Hdlr, Time);
    if (Rval == DEMUXER_OK) {
        SVC_MP4_DMX_HDLR_s *Mp4Dmx;
        AmbaMisra_TypeCast(&Mp4Dmx, &Hdlr);
        *Time = SVC_FORMAT_MAX_TIMESTAMP;
        Rval = K2D(AmbaKAL_MutexTake(&Mp4Dmx->Mutex, SVC_FORMAT_WAIT_TIMEOUT));
        if (Rval == DEMUXER_OK) {
            const SVC_MOVIE_INFO_s *Movie;
            AmbaMisra_TypeCast(&Movie, &Hdlr->Media);
            *Time = SvcFormat_GetMediaTime(Movie->VideoTrack, Movie->VideoTrackCount, Movie->AudioTrack, Movie->AudioTrackCount, Movie->TextTrack, Movie->TextTrackCount, Hdlr->Direction);
            if (AmbaKAL_MutexGive(&Mp4Dmx->Mutex) != DEMUXER_OK) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Give mutex failed!", __func__, NULL, NULL, NULL, NULL);
                Rval = DEMUXER_ERR_FATAL_ERROR;
                *Time = SVC_FORMAT_MAX_TIMESTAMP;
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Take mutex failed!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Invalid argument!", __func__, NULL, NULL, NULL, NULL);
    }
    AmbaMisra_TouchUnused(Hdlr);
    return Rval;
}

static inline UINT32 SvcMp4Dmx_Seek_ParamCheck(const SVC_DMX_FORMAT_HDLR_s *Hdlr, UINT32 Direction, UINT32 Speed)
{
    UINT32 Rval = DEMUXER_ERR_INVALID_ARG;
    if ((Hdlr != NULL) && ((Direction == SVC_FORMAT_DIR_FORWARD) || (Direction == SVC_FORMAT_DIR_BACKWARD)) && (Speed > 0U)) {
        Rval = DEMUXER_OK;
    }
    return Rval;
}

static UINT32 SvcMp4Dmx_Seek(SVC_DMX_FORMAT_HDLR_s *Hdlr, UINT64 TargetTime, UINT32 Direction, UINT32 Speed)
{
    UINT32 Rval = SvcMp4Dmx_Seek_ParamCheck(Hdlr, Direction, Speed);
    if (Rval == DEMUXER_OK) {
        SVC_MP4_DMX_HDLR_s *Mp4Dmx;
        AmbaMisra_TypeCast(&Mp4Dmx, &Hdlr);
        Rval = K2D(AmbaKAL_MutexTake(&Mp4Dmx->Mutex, SVC_FORMAT_WAIT_TIMEOUT));
        if (Rval == DEMUXER_OK) {
            Rval = F2D(Mp4Dmx_Seek(&Mp4Dmx->Hdlr, &Mp4Dmx->Mp4Idx, Mp4Dmx->FeedInfo, TargetTime, Direction, Speed));
            if (AmbaKAL_MutexGive(&Mp4Dmx->Mutex) != OK) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Give mutex failed", __func__, NULL, NULL, NULL, NULL);
                Rval = DEMUXER_ERR_FATAL_ERROR;
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Take mutex failed!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Invalid argument!", __func__, NULL, NULL, NULL, NULL);
    }
    AmbaMisra_TouchUnused(Hdlr);
    return Rval;
}

static inline UINT32 SvcMp4Dmx_FeedFrame_ParamCheck(const SVC_DMX_FORMAT_HDLR_s *Hdlr, UINT8 TrackId, UINT8 FrameType)
{
    UINT32 Rval = DEMUXER_ERR_INVALID_ARG;
    if ((Hdlr != NULL) && (TrackId < SVC_FORMAT_MAX_VIDEO_TRACK_PER_MEDIA) && (FrameType <= SVC_FIFO_TYPE_UNDEFINED)) {
        Rval = DEMUXER_OK;
    }
    return Rval;
}

static UINT32 SvcMp4Dmx_FeedFrame(SVC_DMX_FORMAT_HDLR_s *Hdlr, UINT8 TrackId, UINT64 TargetTime, UINT8 FrameType)
{
    UINT32 Rval = SvcMp4Dmx_FeedFrame_ParamCheck(Hdlr, TrackId, FrameType);
    if (Rval == DEMUXER_OK) {
    SVC_MP4_DMX_HDLR_s *Mp4Dmx;
        AmbaMisra_TypeCast(&Mp4Dmx, &Hdlr);
        Rval = K2D(AmbaKAL_MutexTake(&Mp4Dmx->Mutex, SVC_FORMAT_WAIT_TIMEOUT));
        if (Rval == DEMUXER_OK) {
            Rval = F2D(Mp4Dmx_FeedFrame(&Mp4Dmx->Hdlr, &Mp4Dmx->Mp4Idx, TrackId, TargetTime, FrameType));
            if (AmbaKAL_MutexGive(&Mp4Dmx->Mutex) != OK) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Give mutex failed", __func__, NULL, NULL, NULL, NULL);
                Rval = DEMUXER_ERR_FATAL_ERROR;
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Take mutex fail!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Invalid argument!", __func__, NULL, NULL, NULL, NULL);
    }
    AmbaMisra_TouchUnused(Hdlr);
    return Rval;
}

