/**
 * @file Demuxer.c
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
#include "DemuxerImpl.h"
#include "../FormatAPI.h"
#include "../iso/IsoDmx.h"
#include <AmbaDef.h>

/**
 * The core structure of DMGR
 */
typedef struct {
    AMBA_KAL_MUTEX_t Mutex;                 /**< The mutex of demuxer manager */
    SVC_DEMUXER_PIPE_HDLR_IMPL_s *Pipe;     /**< The pipe handler of demuxer manager */
    UINT8 MaxPipe;                          /**< Max number of pipes in demuxer */
} SVC_DEMUXER_MGR_s;

static SVC_DEMUXER_MGR_s g_Dmgr GNU_SECTION_NOZEROINIT; /**< The demuxer manager */

static UINT32 SvcDemuxer_GetTargetTime(SVC_DMX_FORMAT_HDLR_s *Format, UINT8 Direction, UINT8 Frames, UINT64 *Time)
{
    UINT64 TempTime;
    UINT32 Rval = Format->Func->GetMediaTime(Format, &TempTime);
    if (Rval == DEMUXER_OK) {
        UINT8 Tmp;
        SVC_MOVIE_INFO_s *Movie;
        const SVC_VIDEO_TRACK_INFO_s *Video;
        AmbaMisra_TypeCast(&Movie, &Format->Media);
        Video = SvcFormat_GetDefaultVideoTrack(Movie->VideoTrack, Movie->VideoTrackCount, &Tmp);
        if (Video != NULL) {
            UINT32 TimePerFrame = Video->Info.TimePerFrame;
            UINT32 TimeScale = Video->Info.TimeScale;
            if (Direction == SVC_FORMAT_DIR_FORWARD) {
                if (TempTime != SVC_FORMAT_MAX_TIMESTAMP) {
                    TempTime += SVC_FORMAT_DTS_TO_TIME(SVC_FORMAT_DURATION(Frames, TimePerFrame), TimeScale);
                }
            } else {
                UINT64 Offset = SVC_FORMAT_DTS_TO_TIME(SVC_FORMAT_DURATION(Frames, TimePerFrame), TimeScale);
                if (Offset > TempTime) {
                    TempTime = 0ULL;
                } else {
                    TempTime -= Offset;
                }
            }
            *Time = TempTime;
        } else {
            Rval = DEMUXER_ERR_FATAL_ERROR;
        }
    }
    return Rval;
}

static inline UINT32 SvcDemuxer_Process_ParamCheck(const SVC_DEMUXER_PIPE_HDLR_s *Pipe, const UINT8 *Event)
{
    UINT32 Rval = DEMUXER_ERR_INVALID_ARG;
    if ((Pipe != NULL) && (Event != NULL)) {
        Rval = DEMUXER_OK;
    }
    return Rval;
}

/**
 * Process a Demuxer pipe.
 *
 * @param [in] Pipe The Demuxer pipe being stopped
 * @param [in] FrameNumber The number of frames is processed in the default track.
 * @param [out] Event The event happened in this process
 * @return 0 - OK, others - DEMUXER_ERR_XXXX
 */
UINT32 SvcDemuxer_Process(SVC_DEMUXER_PIPE_HDLR_s *Pipe, UINT8 FrameNumber, UINT8 *Event)
{
    UINT32 Rval = SvcDemuxer_Process_ParamCheck(Pipe, Event);
    if (Rval == DEMUXER_OK) {
        SVC_DEMUXER_PIPE_HDLR_IMPL_s *PipeHdlr;
        AmbaMisra_TypeCast(&PipeHdlr, &Pipe);
        if (PipeHdlr->FormatCount > 0U) {
            Rval = K2D(AmbaKAL_MutexTake(&PipeHdlr->Mutex, SVC_FORMAT_WAIT_TIMEOUT));
            if (Rval == DEMUXER_OK) {
                SVC_DMX_FORMAT_HDLR_s *Format = PipeHdlr->Format[0];
                const SVC_MEDIA_INFO_s *Media = Format->Media;
                if (Media->MediaType == SVC_MEDIA_INFO_MOVIE) {
                    if (FrameNumber != 0U) {
                        UINT64 TargetTime = 0;
                        Rval = SvcDemuxer_GetTargetTime(Format, Format->Direction, FrameNumber, &TargetTime);
                        if (Rval == DEMUXER_OK) {
                            Rval = SvcDemuxer_ProcessMovie(PipeHdlr, TargetTime, Event);
                        }
                    } else {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Invalid arguement!", __func__, NULL, NULL, NULL, NULL);
                        Rval = DEMUXER_ERR_INVALID_ARG;
                    }
                } else if (Media->MediaType == SVC_MEDIA_INFO_IMAGE) {
                     Rval = SvcDemuxer_ProcessImage(PipeHdlr, FrameNumber, Event);
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Invalid MediaType!", __func__, NULL, NULL, NULL, NULL);
                    Rval = DEMUXER_ERR_FATAL_ERROR;
                }
                if (AmbaKAL_MutexGive(&PipeHdlr->Mutex) != OK) {
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Give mutex failed!", __func__, NULL, NULL, NULL, NULL);
                    Rval = DEMUXER_ERR_FATAL_ERROR;
                }
            } else {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Take mutex failed!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s No format handler!", __func__, NULL, NULL, NULL, NULL);
            Rval = DEMUXER_ERR_FATAL_ERROR;
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Invalid arguement!", __func__, NULL, NULL, NULL, NULL);
    }
    AmbaMisra_TouchUnused(Pipe);
    return Rval;
}

static inline UINT32 SvcDemuxer_GetInitDefaultCfg_ParamCheck(const SVC_DEMUXER_INIT_CFG_s *Config)
{
    UINT32 Rval = DEMUXER_ERR_INVALID_ARG;
    if (Config != NULL) {
        Rval = DEMUXER_OK;
    }
    return Rval;
}

/**
 * Get the default configuration for initializing the Demuxer module.
 *
 * @param [out] Config The returned configuration
 * @return 0 - OK, others - DEMUXER_ERR_XXXX
 */
UINT32 SvcDemuxer_GetInitDefaultCfg(SVC_DEMUXER_INIT_CFG_s *Config)
{
    UINT32 Rval = SvcDemuxer_GetInitDefaultCfg_ParamCheck(Config);
    if (Rval == DEMUXER_OK) {
        Rval = W2D(AmbaWrap_memset(Config, 0, sizeof(SVC_DEMUXER_INIT_CFG_s)));
        if (Rval == DEMUXER_OK) {
            /* default 1 pipe */
            Config->MaxPipe = 1;
            /* default pipe = task */
            Rval = SvcDemuxer_GetInitBufferSize(Config->MaxPipe, &Config->BufferSize);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Invalid arguement!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static inline UINT32 SvcDemuxer_GetInitBufferSize_ParamCheck(UINT8 MaxPipe, const UINT32 *BufferSize)
{
    UINT32 Rval = DEMUXER_ERR_INVALID_ARG;
    if ((MaxPipe > 0U) && (BufferSize != NULL)) {
        Rval = DEMUXER_OK;
    }
    return Rval;
}

/**
 * Get the required buffer size for initializing the Demuxer module.
 *
 * @param [in] MaxPipe The maximum number of Demuxer pipes
 * @param [out] BufferSize The required working buffer size
 * @return 0 - OK, others - DEMUXER_ERR_XXXX
 */
UINT32 SvcDemuxer_GetInitBufferSize(UINT8 MaxPipe, UINT32 *BufferSize)
{
    UINT32 Rval = SvcDemuxer_GetInitBufferSize_ParamCheck(MaxPipe, BufferSize);
    if (Rval == DEMUXER_OK) {
        *BufferSize = GetAlignedValU32(MaxPipe * (UINT32)sizeof(SVC_DEMUXER_PIPE_HDLR_IMPL_s), (UINT32)AMBA_CACHE_LINE_SIZE);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Invalid arguement!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval; /* additional pointer space for block pool */
}

#define MAX_DEMUXER_NUM  (32U)  /**< Maximum number of demuxer pipe */
static UINT32 CreateMutex(void)
{
    static char DmxMgrMutexName[16] = "Demuxer_Mmgr";
    UINT32 Rval = K2D(AmbaKAL_MutexCreate(&g_Dmgr.Mutex, DmxMgrMutexName));
    if (Rval == DEMUXER_OK) {
        static char MutexName[MAX_DEMUXER_NUM][16];
        UINT32 i;
        for (i = 0; i < g_Dmgr.MaxPipe; i++) {
            SVC_DEMUXER_PIPE_HDLR_IMPL_s *Hdlr = &g_Dmgr.Pipe[i];
            UINT32 Len = AmbaUtility_StringPrintUInt32(MutexName[i], (UINT32)sizeof(MutexName[i]), "DemuxerPipe%02u", 1, &i);
            Rval = K2D(AmbaKAL_MutexCreate(&Hdlr->Mutex, MutexName[i]));
            if ((Rval != DEMUXER_OK) || (i >= MAX_DEMUXER_NUM)) {
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

static UINT32 SvcDemuxer_InitImpl(void *Buffer, UINT8 MaxPipe)
{
    UINT32 Rval;
    AmbaMisra_TouchUnused(Buffer);
    g_Dmgr.MaxPipe = MaxPipe;
    AmbaMisra_TypeCast(&g_Dmgr.Pipe, &Buffer);
    Rval = W2D(AmbaWrap_memset(g_Dmgr.Pipe, 0, g_Dmgr.MaxPipe * sizeof(SVC_DEMUXER_PIPE_HDLR_IMPL_s)));
    if (Rval == DEMUXER_OK) {
        Rval = CreateMutex();
    }
    return Rval;
}

static inline UINT32 SvcDemuxer_Init_ParamCheck(const SVC_DEMUXER_INIT_CFG_s *Config)
{
    UINT32 Rval = DEMUXER_ERR_INVALID_ARG;
    if (Config != NULL) {
        Rval = DEMUXER_OK;
    }
    return Rval;
}

/**
 * Initialize the Demuxer module.
 *
 * @param [in] Config The configuration used to initialize the Demuxer module
 * @return 0 - OK, others - DEMUXER_ERR_XXXX
 */
UINT32 SvcDemuxer_Init(SVC_DEMUXER_INIT_CFG_s *Config)
{
    static UINT8 g_DmgrInit = 0U;
    UINT32 Rval = SvcDemuxer_Init_ParamCheck(Config);
    if (Rval == DEMUXER_OK) {
        UINT32 Size = 0U;
        Rval = SvcDemuxer_GetInitBufferSize(Config->MaxPipe, &Size);
        if (Rval == DEMUXER_OK) {
            if (Config->BufferSize == Size) {
                if (g_DmgrInit == 0U) {
                    g_DmgrInit = 1U;
                    Rval = W2D(AmbaWrap_memset(&g_Dmgr, 0, sizeof(SVC_DEMUXER_MGR_s)));
                    if (Rval == DEMUXER_OK) {
                        Rval = SvcDemuxer_InitImpl(Config->Buffer, Config->MaxPipe);
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Wrong status of DMGR!", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Invalid arguement!", __func__, NULL, NULL, NULL, NULL);
            }
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Invalid arguement!", __func__, NULL, NULL, NULL, NULL);
    }
    AmbaMisra_TouchUnused(Config);
    return Rval;
}

static inline UINT32 SvcDemuxer_GetDefaultCfg_ParamCheck(const SVC_DEMUXER_PIPE_CFG_s *Config)
{
    UINT32 Rval = DEMUXER_ERR_INVALID_ARG;
    if (Config != NULL) {
        Rval = DEMUXER_OK;
    }
    return Rval;
}

/**
 * Get the default configuration for creating Demuxer pipes.
 *
 * @param [out] Config The returned configuration
 * @return 0 - OK, others - DEMUXER_ERR_XXXX
 */
UINT32 SvcDemuxer_GetDefaultCfg(SVC_DEMUXER_PIPE_CFG_s *Config)
{
    UINT32 Rval = SvcDemuxer_GetDefaultCfg_ParamCheck(Config);
    if (Rval == DEMUXER_OK) {
        Rval = W2D(AmbaWrap_memset(Config, 0, sizeof(SVC_DEMUXER_PIPE_CFG_s)));
        Config->Speed = 1;
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Invalid arguement!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 SvcDemuxer_GetPipe(SVC_DEMUXER_PIPE_HDLR_IMPL_s **Pipe)
{
    UINT32 Rval = DEMUXER_ERR_FATAL_ERROR;
    UINT32 i;
    for (i = 0; i < g_Dmgr.MaxPipe; i++) {
        SVC_DEMUXER_PIPE_HDLR_IMPL_s *DmxPipe = &g_Dmgr.Pipe[i];
        if (DmxPipe->Used == 0U) {
            DmxPipe->Hdlr.PipeId = (UINT8) i;
            DmxPipe->Used = 1U;
            *Pipe = DmxPipe;
            Rval = DEMUXER_OK;
            break;
        }
    }
    if (i == g_Dmgr.MaxPipe) {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Get pipe fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 SvcDemuxer_RelPipe(SVC_DEMUXER_PIPE_HDLR_IMPL_s *Pipe)
{
    UINT32 Rval = DEMUXER_ERR_FATAL_ERROR;
    const UINT32 Id = Pipe->Hdlr.PipeId;
    if (g_Dmgr.Pipe[Id].Used == 1U) {
        g_Dmgr.Pipe[Id].Used = 0U;
        Rval = DEMUXER_OK;
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Release pipe fail!", __func__, NULL, NULL, NULL, NULL);
    }
    AmbaMisra_TouchUnused(Pipe);
    return Rval;
}

static inline UINT32 SvcDemuxer_CreateImpl_CheckCfg(const SVC_DEMUXER_PIPE_CFG_s *Config)
{
    UINT32 Rval = DEMUXER_OK;
    if ((Config->Speed == 0U) || (Config->FormatCount == 0U)) {
        Rval = DEMUXER_ERR_INVALID_ARG;
    } else {
        UINT8 i;
        for (i = 0U; i < Config->FormatCount; i++) {
            if ((Config->Format[i] == NULL) || (Config->Media[i] == NULL)) {
                Rval = DEMUXER_ERR_INVALID_ARG;
                break;
            }
        }
    }
    return Rval;
}

static UINT32 SvcDemuxer_CreateImpl(SVC_DEMUXER_PIPE_CFG_s *Config, SVC_DEMUXER_PIPE_HDLR_IMPL_s **Pipe)
{
    UINT32 Rval = SvcDemuxer_CreateImpl_CheckCfg(Config);
    if (Rval == DEMUXER_OK) {
        SVC_DEMUXER_PIPE_HDLR_IMPL_s *DmxPipe = NULL;
        Rval = SvcDemuxer_GetPipe(&DmxPipe);
        if (Rval == DEMUXER_OK) {
            Rval = K2D(AmbaKAL_MutexTake(&DmxPipe->Mutex, SVC_FORMAT_WAIT_TIMEOUT));
            if (Rval == DEMUXER_OK) {
                UINT32 i;
                UINT8 MasterType = Config->Media[0]->MediaType;
                for (i = 0; i < Config->FormatCount; i++) {
                    SVC_DMX_FORMAT_HDLR_s * const Format = Config->Format[i];
                    Format->Media = Config->Media[i];
                    if (Format->Media->MediaType == SVC_MEDIA_INFO_MOVIE) {
                        if (MasterType != SVC_MEDIA_INFO_MOVIE) {
                            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Wrong media type!", __func__, NULL, NULL, NULL, NULL);
                            Rval = DEMUXER_ERR_INVALID_ARG;
                        } else {
                            Format->Direction = SVC_FORMAT_DIR_FORWARD;
                            Format->Speed = Config->Speed;
                        }
                    } else if (MasterType == SVC_MEDIA_INFO_IMAGE) {
                        if ((i > 0U) || (Format->Media->MediaType != SVC_MEDIA_INFO_IMAGE)) {
                            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Wrong media type!", __func__, NULL, NULL, NULL, NULL);
                            Rval = DEMUXER_ERR_INVALID_ARG;
                        }
                    } else {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Wrong media type!", __func__, NULL, NULL, NULL, NULL);
                        Rval = DEMUXER_ERR_INVALID_ARG;
                    }
                    if (Rval == DEMUXER_OK) {
                        DmxPipe->Format[i] = Format;
                    } else {
                        break;
                    }
                }
                if (Rval == DEMUXER_OK) {
                    DmxPipe->State = SVC_DEMUXER_STATE_IDLE;
                    DmxPipe->FormatCount = Config->FormatCount;
                    SvcDemuxer_PrintPipeInfo(DmxPipe, "DMXER : ");
                } else {
                    if (SvcDemuxer_RelPipe(DmxPipe) != DEMUXER_OK) {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Release pipe failed!", __func__, NULL, NULL, NULL, NULL);
                    }
                }
                if (AmbaKAL_MutexGive(&DmxPipe->Mutex) != OK) {
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Give mutex failed!", __func__, NULL, NULL, NULL, NULL);
                    Rval = DEMUXER_ERR_FATAL_ERROR;
                }
            } else {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Take mutex failed!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Create pipe fail!", __func__, NULL, NULL, NULL, NULL);
        }
        *Pipe = (Rval == DEMUXER_OK)? DmxPipe : NULL;
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Invalid arguement!", __func__, NULL, NULL, NULL, NULL);
    }
    AmbaMisra_TouchUnused(Config);
    return Rval;
}

static inline UINT32 SvcDemuxer_Create_ParamCheck(const SVC_DEMUXER_PIPE_CFG_s *Config, SVC_DEMUXER_PIPE_HDLR_s *const *Pipe)
{
    UINT32 Rval = DEMUXER_ERR_INVALID_ARG;
    if ((Config != NULL) && (Pipe != NULL)) {
        Rval = DEMUXER_OK;
    }
    return Rval;
}

/**
 * Create a Demuxer pipe.
 *
 * @param [in] Config The configuration used to create a Demuxer pipe
 * @param [out] Pipe The created pipe
 * @return 0 - OK, others - DEMUXER_ERR_XXXX
 * @note Media types of Demuxer pipes indicate the kinds of media that will be processed. An image type
 * cannot appear concurrently with any other types; however, movie and sound types can appear with each other.
 */
UINT32 SvcDemuxer_Create(SVC_DEMUXER_PIPE_CFG_s *Config, SVC_DEMUXER_PIPE_HDLR_s **Pipe)
{
    UINT32 Rval = SvcDemuxer_Create_ParamCheck(Config, Pipe);
    if (Rval == DEMUXER_OK) {
        Rval = K2D(AmbaKAL_MutexTake(&g_Dmgr.Mutex, SVC_FORMAT_WAIT_TIMEOUT));
        if (Rval == DEMUXER_OK) {
            SVC_DEMUXER_PIPE_HDLR_IMPL_s *DmxPipe;
            Rval = SvcDemuxer_CreateImpl(Config, &DmxPipe);
            if (Rval == DEMUXER_OK) {
                SVC_DEMUXER_PIPE_HDLR_s *RetPipe;
                AmbaMisra_TypeCast(&RetPipe, &DmxPipe);
                *Pipe = RetPipe;
            }
            if (AmbaKAL_MutexGive(&g_Dmgr.Mutex) != OK) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Give mutex failed!", __func__, NULL, NULL, NULL, NULL);
                Rval = DEMUXER_ERR_FATAL_ERROR;
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Take Mutex Fail!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Invalid arguement!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 SvcDemuxer_DeleteImpl(SVC_DEMUXER_PIPE_HDLR_IMPL_s *Pipe)
{
    UINT32 Rval = K2D(AmbaKAL_MutexTake(&Pipe->Mutex, SVC_FORMAT_WAIT_TIMEOUT));
    if (Rval == DEMUXER_OK) {
        if ((Pipe->State != SVC_DEMUXER_STATE_IDLE)
            && (Pipe->State != SVC_DEMUXER_STATE_END)
            && (Pipe->State != SVC_DEMUXER_STATE_ERROR)) {
            AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "SvcDemuxer_DeleteImpl: Wrong state of pipe 0x%x!", Pipe->State, 0U, 0U, 0U, 0U);
            Rval = DEMUXER_ERR_FATAL_ERROR;
        } else {
            Rval = SvcDemuxer_RelPipe(Pipe);
        }
        if (AmbaKAL_MutexGive(&Pipe->Mutex) != OK) {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Give mutex failed", __func__, NULL, NULL, NULL, NULL);
            Rval = DEMUXER_ERR_FATAL_ERROR;
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Take mutex failed!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static inline UINT32 SvcDemuxer_Delete_ParamCheck(const SVC_DEMUXER_PIPE_HDLR_s *Pipe)
{
    UINT32 Rval = DEMUXER_ERR_INVALID_ARG;
    if (Pipe != NULL) {
        Rval = DEMUXER_OK;
    }
    return Rval;
}

/**
 * Delete a Demuxer pipe.
 *
 * @param [in] Pipe The Demuxer pipe being deleted
 * @return 0 - OK, others - DEMUXER_ERR_XXXX
 */
UINT32 SvcDemuxer_Delete(SVC_DEMUXER_PIPE_HDLR_s *Pipe)
{
    UINT32 Rval = SvcDemuxer_Delete_ParamCheck(Pipe);
    if (Rval == DEMUXER_OK) {
        Rval = K2D(AmbaKAL_MutexTake(&g_Dmgr.Mutex, SVC_FORMAT_WAIT_TIMEOUT));
        if (Rval == OK) {
            SVC_DEMUXER_PIPE_HDLR_IMPL_s *DmxPipe;
            AmbaMisra_TypeCast(&DmxPipe, &Pipe);
            Rval = SvcDemuxer_DeleteImpl(DmxPipe);
            if (AmbaKAL_MutexGive(&g_Dmgr.Mutex) != OK) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Give mutex failed", __func__, NULL, NULL, NULL, NULL);
                Rval = DEMUXER_ERR_FATAL_ERROR;
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Take mutex failed!", __func__, NULL, NULL, NULL, NULL);
            Rval = DEMUXER_ERR_FATAL_ERROR;
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Invalid arguement!", __func__, NULL, NULL, NULL, NULL);
    }
    AmbaMisra_TouchUnused(Pipe);
    return Rval;
}

static UINT32 SvcDemuxer_StartImpl(SVC_DEMUXER_PIPE_HDLR_IMPL_s *Pipe)
{
    UINT32 Rval = DEMUXER_ERR_FATAL_ERROR;
    if ((Pipe->State == SVC_DEMUXER_STATE_IDLE) || (Pipe->State == SVC_DEMUXER_STATE_END)) {
        /* change state to start */
        Pipe->State = SVC_DEMUXER_STATE_START;
        Rval = DEMUXER_OK;
    } else {
        AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "SvcDemuxer_StartImpl: Wrong state of pipe 0x%x!", Pipe->State, 0U, 0U, 0U, 0U);
    }
    return Rval;
}

static inline UINT32 SvcDemuxer_Start_ParamCheck(const SVC_DEMUXER_PIPE_HDLR_s *Pipe)
{
    UINT32 Rval = DEMUXER_ERR_INVALID_ARG;
    if (Pipe != NULL) {
        Rval = DEMUXER_OK;
    }
    return Rval;
}

/**
 * Start a Demuxer pipe.
 *
 * @param [in] Pipe The Demuxer pipe being started
 * @return 0 - OK, others - DEMUXER_ERR_XXXX
 */
UINT32 SvcDemuxer_Start(SVC_DEMUXER_PIPE_HDLR_s *Pipe)
{
    UINT32 Rval = SvcDemuxer_Start_ParamCheck(Pipe);
    if (Rval == DEMUXER_OK) {
        SVC_DEMUXER_PIPE_HDLR_IMPL_s *DmxPipe;
        AmbaMisra_TypeCast(&DmxPipe, &Pipe);
        Rval = K2D(AmbaKAL_MutexTake(&DmxPipe->Mutex, SVC_FORMAT_WAIT_TIMEOUT));
        if (Rval == OK) {
            Rval = SvcDemuxer_StartImpl(DmxPipe);
            if (AmbaKAL_MutexGive(&DmxPipe->Mutex) != OK) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Give mutex failed!", __func__, NULL, NULL, NULL, NULL);
                Rval = DEMUXER_ERR_FATAL_ERROR;
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Take Mutex Fail!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Invalid arguement!", __func__, NULL, NULL, NULL, NULL);
    }
    AmbaMisra_TouchUnused(Pipe);
    return Rval;
}

static UINT32 SvcDemuxer_StopImpl(SVC_DEMUXER_PIPE_HDLR_IMPL_s *Pipe)
{
    UINT32 Rval = DEMUXER_OK;
    if (Pipe->State == SVC_DEMUXER_STATE_START) {
        Pipe->State = SVC_DEMUXER_STATE_END;
    } else if (Pipe->State == SVC_DEMUXER_STATE_RUNNING) {
        Pipe->State = SVC_DEMUXER_STATE_STOPPING;
    } else if ((Pipe->State == SVC_DEMUXER_STATE_END) || (Pipe->State == SVC_DEMUXER_STATE_ERROR)) {
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Incorrect state!", __func__, NULL, NULL, NULL, NULL);
        Rval = DEMUXER_ERR_FATAL_ERROR;
    }
    return Rval;
}

static inline UINT32 SvcDemuxer_Stop_ParamCheck(const SVC_DEMUXER_PIPE_HDLR_s *Pipe)
{
    UINT32 Rval = DEMUXER_ERR_INVALID_ARG;
    if (Pipe != NULL) {
        Rval = DEMUXER_OK;
    }
    return Rval;
}

/**
 * Stop a Demuxer pipe.
 *
 * @param [in] Pipe The Demuxer pipe being stopped
 * @return 0 - OK, others - DEMUXER_ERR_XXXX
 */
UINT32 SvcDemuxer_Stop(SVC_DEMUXER_PIPE_HDLR_s *Pipe)
{
    UINT32 Rval = SvcDemuxer_Stop_ParamCheck(Pipe);
    if (Rval == DEMUXER_OK) {
        SVC_DEMUXER_PIPE_HDLR_IMPL_s *DmxPipe;
        AmbaMisra_TypeCast(&DmxPipe, &Pipe);
        Rval = K2D(AmbaKAL_MutexTake(&DmxPipe->Mutex, SVC_FORMAT_WAIT_TIMEOUT));
        if (Rval == OK) {
            Rval = SvcDemuxer_StopImpl(DmxPipe);
            if (AmbaKAL_MutexGive(&DmxPipe->Mutex) != OK) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Give mutex failed!", __func__, NULL, NULL, NULL, NULL);
                Rval = DEMUXER_ERR_FATAL_ERROR;
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Take Mutex Fail!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Invalid arguement!", __func__, NULL, NULL, NULL, NULL);
    }
    AmbaMisra_TouchUnused(Pipe);
    return Rval;
}

static UINT32 SvcDemuxer_DoSeek(SVC_DEMUXER_PIPE_HDLR_IMPL_s *Pipe, UINT32 TargetTime, UINT8 Direction, UINT32 Speed)
{
    UINT32 Rval = DEMUXER_OK;
    if (Pipe->State == SVC_DEMUXER_STATE_ERROR) {
        Rval = DEMUXER_ERR_FATAL_ERROR;
        AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "SvcDemuxer_DoSeek: Wrong state of pipe 0x%x!", Pipe->State, 0U, 0U, 0U, 0U);
    } else {
        if ((Pipe->State != SVC_DEMUXER_STATE_RUNNING) && (Pipe->State != SVC_DEMUXER_STATE_STOPPING)) {
            Pipe->State = SVC_DEMUXER_STATE_START;
            Rval = SvcDemuxer_StartPipe(Pipe);
        }
        if (Rval == DEMUXER_OK) {
            UINT32 i;
            Pipe->State = SVC_DEMUXER_STATE_RUNNING;
            for (i = 0; i < Pipe->FormatCount; i++) {
                SVC_DMX_FORMAT_HDLR_s * const Format = Pipe->Format[i];
                switch (Format->Media->MediaType) {
                case SVC_MEDIA_INFO_MOVIE:
                    Format->Direction = Direction;
                    Format->Speed = (UINT8) Speed;
                    Rval = Format->Func->Seek(Format, TargetTime, Direction, Speed);
                    if (Rval != DEMUXER_OK) {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Seek fail!", __func__, NULL, NULL, NULL, NULL);
                    }
                    break;
                case SVC_MEDIA_INFO_IMAGE:
                    break;
                default:
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Wrong media type!", __func__, NULL, NULL, NULL, NULL);
                    Rval = DEMUXER_ERR_INVALID_ARG;
                    break;
                }
                if (Rval != DEMUXER_OK) {
                    break;
                }
            }
        }
    }
    return Rval;
}

static UINT32 SvcDemuxer_SeekImpl(SVC_DEMUXER_PIPE_HDLR_IMPL_s *Pipe, UINT32 TargetTime, UINT8 Direction, UINT32 Speed)
{
    UINT32 Rval = K2D(AmbaKAL_MutexTake(&Pipe->Mutex, SVC_FORMAT_WAIT_TIMEOUT));
    if (Rval == DEMUXER_OK) {
        Rval = SvcDemuxer_DoSeek(Pipe, TargetTime, Direction, Speed);
        if (AmbaKAL_MutexGive(&Pipe->Mutex) != OK) {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Give mutex failed!", __func__, NULL, NULL, NULL, NULL);
            Rval = DEMUXER_ERR_FATAL_ERROR;
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Take mutex failed!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static inline UINT32 SvcDemuxer_Seek_ParamCheck(const SVC_DEMUXER_PIPE_HDLR_s *Pipe, UINT32 Direction, UINT32 Speed)
{
    UINT32 Rval = DEMUXER_ERR_INVALID_ARG;
    if ((Pipe != NULL) && ((Direction == SVC_FORMAT_DIR_FORWARD) || (Direction == SVC_FORMAT_DIR_BACKWARD)) && (Speed > 0U)) {
        Rval = DEMUXER_OK;
    }
    return Rval;
}

/**
 * Seek and set the start time of demuxing.
 *
 * @param [in] Pipe The Demuxer pipe
 * @param [in] TargetTime The reference time being sought
 * @param [in] Direction Seek direction
 * @param [in] Speed The demuxing speed after seeking.
 * @return 0 - OK, others - DEMUXER_ERR_XXXX
 */
UINT32 SvcDemuxer_Seek(SVC_DEMUXER_PIPE_HDLR_s *Pipe, UINT32 TargetTime, UINT8 Direction, UINT32 Speed)
{
    UINT32 Rval = SvcDemuxer_Seek_ParamCheck(Pipe, Direction, Speed);
    if (Rval == DEMUXER_OK) {
        Rval = K2D(AmbaKAL_MutexTake(&g_Dmgr.Mutex, SVC_FORMAT_WAIT_TIMEOUT));
        if (Rval == DEMUXER_OK) {
            SVC_DEMUXER_PIPE_HDLR_IMPL_s *DmxPipe;
            AmbaMisra_TypeCast(&DmxPipe, &Pipe);
            Rval = SvcDemuxer_SeekImpl(DmxPipe, TargetTime, Direction, Speed);
            if (AmbaKAL_MutexGive(&g_Dmgr.Mutex) != OK) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Give mutex failed!", __func__, NULL, NULL, NULL, NULL);
                Rval = DEMUXER_ERR_FATAL_ERROR;
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Take Mutex Fail!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Invalid arguement!", __func__, NULL, NULL, NULL, NULL);
    }
    AmbaMisra_TouchUnused(Pipe);
    return Rval;
}

static UINT32 SvcDemuxer_FeedFrameImpl(SVC_DMX_FORMAT_HDLR_s *Format, UINT8 TrackId, UINT32 TargetTime, UINT8 FrameType)
{
    UINT32 Rval = Format->Func->Open(Format);
    if (Rval == DEMUXER_OK) {
        UINT32 Ret;
        Rval = Format->Func->FeedFrame(Format, TrackId, TargetTime, FrameType);
        if (Rval != DEMUXER_OK) {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Format->Func->FeedFrame failed!", __func__, NULL, NULL, NULL, NULL);
        }
        Ret = Format->Func->Close(Format);
        if (Ret != DEMUXER_OK) {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Format->Func->Close failed!", __func__, NULL, NULL, NULL, NULL);
            if ((Rval == DEMUXER_OK) || (Rval == DEMUXER_ERR_IO_ERROR)) {
                // overwrite error code if more serious
                Rval = Ret;
            }
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Open format handler fail!", __func__, NULL, NULL, NULL, NULL);
    }

    return Rval;
}

static inline UINT32 SvcDemuxer_FeedFrame_ParamCheck(const SVC_DMX_FORMAT_HDLR_s *Format, UINT8 TrackId, UINT8 FrameType)
{
    UINT32 Rval = DEMUXER_ERR_INVALID_ARG;
    if ((Format != NULL) && (TrackId < SVC_FORMAT_MAX_VIDEO_TRACK_PER_MEDIA) && (FrameType <= SVC_FIFO_TYPE_UNDEFINED)) {
        Rval = DEMUXER_OK;
    }
    return Rval;
}

/**
 * Feed a frame into a FIFO.
 *
 * @param [in] Format The Format handler
 * @param [in] TrackId The ID of a track that the new frame is fed into its FIFO
 * @param [in] TargetTime The reference time of the frame
 * @param [in] FrameType The type of the frame
 * @return SVC_FORMAT_EVENT_e
 */
UINT32 SvcDemuxer_FeedFrame(SVC_DMX_FORMAT_HDLR_s *Format, UINT8 TrackId, UINT32 TargetTime, UINT8 FrameType)
{
    UINT32 Rval = SvcDemuxer_FeedFrame_ParamCheck(Format, TrackId, FrameType);
    if (Rval == DEMUXER_OK) {
        Rval = K2D(AmbaKAL_MutexTake(&g_Dmgr.Mutex, SVC_FORMAT_WAIT_TIMEOUT));
        if (Rval == DEMUXER_OK) {
            Rval = SvcDemuxer_FeedFrameImpl(Format, TrackId, TargetTime, FrameType);
            if (AmbaKAL_MutexGive(&g_Dmgr.Mutex) != OK) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Give mutex failed!", __func__, NULL, NULL, NULL, NULL);
                Rval = DEMUXER_ERR_FATAL_ERROR;
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Take Mutex Fail!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Invalid arguement!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static inline UINT32 SvcDemuxer_GetFormatType_ParamCheck(const SVC_STREAM_HDLR_s *Stream, const UINT8 *FormatType)
{
    UINT32 Rval = DEMUXER_ERR_INVALID_ARG;
    if ((Stream != NULL) && (FormatType != NULL)) {
        Rval = DEMUXER_OK;
    }
    return Rval;
}

/**
 * Get the type of a container format.
 *
 * @param [in] Stream The I/O stream
 * @param [out] FormatType The returned format type
 * @return 0 - OK, others - DEMUXER_ERR_XXXX
 */
UINT32 SvcDemuxer_GetFormatType(SVC_STREAM_HDLR_s *Stream, UINT8 *FormatType)
{
    UINT32 Rval = SvcDemuxer_GetFormatType_ParamCheck(Stream, FormatType);
    if (Rval == DEMUXER_OK) {
        UINT64 ParseEndPos;
        *FormatType = SVC_DEMUXER_FORMAT_TYPE_MP4;
        Rval = S2D(Stream->Func->GetLength(Stream, &ParseEndPos));
        if (Rval == DEMUXER_OK) {
            UINT64 ParsePos = 0U;
            UINT32 BoxSize, BoxTag = 0U;
            while (ParsePos < ParseEndPos) {
                Rval = S2D(Stream->Func->Seek(Stream, (INT64)ParsePos, SVC_STREAM_SEEK_START));
                if (Rval == DEMUXER_OK) {
                    Rval = F2D(IsoDmx_GetBoxHeader(Stream, &BoxSize, &BoxTag));
                    if (Rval == DEMUXER_OK) {
                        if (BoxTag == TAG_MOOF) {
                            *FormatType = SVC_DEMUXER_FORMAT_TYPE_FMP4;
                        } else {
                            ParsePos += BoxSize;
                        }
                    } else {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s BetBoxHeadr failed!", __func__, NULL, NULL, NULL, NULL);
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Seek failed!", __func__, NULL, NULL, NULL, NULL);
                }
                if ((Rval != OK) || (BoxTag == TAG_MOOF)) {
                    break;
                }
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Get file length error!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Invalid argument!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

