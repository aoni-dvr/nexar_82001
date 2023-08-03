/**
 *  @file AmbaDmux.c
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
 *  @details amba demuxer functions
 *
 */

#include "AmbaDef.h"
#include "AmbaMisraFix.h"
#include "SvcFIFO.h"
#include "SvcStream.h"
#include "SvcFormatDef.h"
#include "SvcIso.h"
#include "SvcFormat.h"
#include "SvcDemuxer.h"
#include "DemuxerImpl.h"
#include "SvcFmp4Dmx.h"
#include "SvcMp4Dmx.h"
#include "AmbaCodecCom.h"
#include "AmbaSvcWrap.h"
#include "AmbaDmux.h"

#define MAX_PIPE_NUM          (1U)
#define MAX_FORMAT_NUM        (1U)

typedef struct {
    SVC_DMX_FORMAT_HDLR_s     *pHdlr;
    SVC_MOVIE_INFO_s          *pMovInfo;
} AMBA_DMUX_FORMAT_INFO_s;

typedef struct {
    UINT8                     FormatCount;
    AMBA_DMUX_FORMAT_INFO_s   FormatInfo[MAX_FORMAT_NUM];
    UINT32                    StartTime;
    UINT32                    Direction;
    UINT32                    Speed;
    SVC_DEMUXER_PIPE_HDLR_s   *pHdlr;
    SVC_STREAM_HDLR_s         *pStreamHdlr;
    UINT8                     ContainerType;
} AMBA_DMUX_PIPE_INFO_s;

typedef struct {
    UINT8                     PipeCount;
    AMBA_DMUX_PIPE_INFO_s     PipeInfo[MAX_PIPE_NUM];
} AMBA_DMUX_INFO_s;

/**
* Evaluate mamory size
* @param [in]  DemuxNum max demux num
* @param [out]  pSize size
* @return ErrorCode
*/
UINT32 AmbaDmux_EvalMemSize(UINT32 DemuxNum, UINT32 *pSize)
{
    UINT32 Size, RSize = 0U, Err,  Rval = PLAYER_OK;

    AmbaMisra_TouchUnused(&Rval);

    /* demuxer */
    if (Rval == PLAYER_OK) {
        Err = SvcDemuxer_GetInitBufferSize((UINT8)(DemuxNum * MAX_PIPE_NUM), &Size);
        if (Err != DEMUXER_OK) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "SvcDemuxer_GetInitBufferSize failed (%u)", Err, 0U, 0U, 0U, 0U);
            Rval = PLAYER_ERROR_GENERAL_ERROR;
        } else {
            Size = GetAlignedValU32(Size, (UINT32)AMBA_CACHE_LINE_SIZE);
            RSize += Size;
        }
    }

    /* mp4 demuxer */
    if (Rval == PLAYER_OK) {
        Err = SvcMp4Dmx_GetInitBufferSize((UINT8)(DemuxNum * MAX_PIPE_NUM * MAX_FORMAT_NUM), &Size);
        if (Err != DEMUXER_OK) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "SvcMp4Dmx_GetInitBufferSize failed (%u)", Err, 0U, 0U, 0U, 0U);
            Rval = PLAYER_ERROR_GENERAL_ERROR;
        } else {
            Size = GetAlignedValU32(Size, (UINT32)AMBA_CACHE_LINE_SIZE);
            RSize += Size;
        }
    }

    /* fmp4 demuxer */
    if (Rval == PLAYER_OK) {
        Err = SvcFmp4Dmx_GetInitBufferSize((UINT8)(DemuxNum * MAX_PIPE_NUM * MAX_FORMAT_NUM), &Size);
        if (Err != DEMUXER_OK) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "SvcFmp4Dmx_GetInitBufferSize failed (%u)", Err, 0U, 0U, 0U, 0U);
            Rval = PLAYER_ERROR_GENERAL_ERROR;
        } else {
            Size = GetAlignedValU32(Size, (UINT32)AMBA_CACHE_LINE_SIZE);
            RSize += Size;
        }
    }

    if (Rval == PLAYER_OK) {
        *pSize = RSize;
    }

    return Rval;
}

/**
* init demuxer
* @param [in]  pInitInfo init info
* @return ErrorCode
*/
UINT32 AmbaDmux_Init(const AMBA_DMUX_INIT_s *pInitInfo)
{
    UINT32        Err, Rval = PLAYER_OK;
    UINT32        RSize = pInitInfo->DmxBuf.BufSize;
    ULONG         BufAddr = pInitInfo->DmxBuf.BufAddr;
    static UINT32 Init = 0U;

    AmbaMisra_TouchUnused(&Rval);

    if (Init == 0U) {
        /* Init demuxer */
        if (Rval == PLAYER_OK) {
            SVC_DEMUXER_INIT_CFG_s InitCfg;

            Err = SvcDemuxer_GetInitDefaultCfg(&InitCfg);
            if (Err != DEMUXER_OK) {
                AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "SvcDemuxer_GetInitDefaultCfg failed (%u)", Err, 0U, 0U, 0U, 0U);
            }

            InitCfg.MaxPipe = (UINT8)(pInitInfo->DemuxNum * MAX_PIPE_NUM);
            AmbaMisra_TypeCast(&(InitCfg.Buffer), &BufAddr);
            Err = SvcDemuxer_GetInitBufferSize(InitCfg.MaxPipe, &(InitCfg.BufferSize));
            if (Err != DEMUXER_OK) {
                AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "SvcDemuxer_GetInitBufferSize failed (%u)", Err, 0U, 0U, 0U, 0U);
                Rval = PLAYER_ERROR_GENERAL_ERROR;
            }

            if (Rval == PLAYER_OK) {
                if (RSize < InitCfg.BufferSize) {
                    AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "SvcDemuxer buffer is not enough (%u/%u)", RSize, InitCfg.BufferSize, 0U, 0U, 0U);
                    Rval = PLAYER_ERROR_ARG;
                } else {
                    Err = SvcDemuxer_Init(&InitCfg);
                    if (Err != DEMUXER_OK) {
                        AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "SvcDemuxer_Init failed (%u)", Err, 0U, 0U, 0U, 0U);
                        Rval = PLAYER_ERROR_GENERAL_ERROR;
                    } else {
                        RSize   -= GetAlignedValU32(InitCfg.BufferSize, (UINT32)AMBA_CACHE_LINE_SIZE);
                        BufAddr += (ULONG)GetAlignedValU32(InitCfg.BufferSize, (UINT32)AMBA_CACHE_LINE_SIZE);
                    }
                }
            }
        }

        /* Init mp4 demuxer */
        if (Rval == PLAYER_OK) {
            SVC_MP4_DMX_INIT_CFG_s InitCfg;

            Err = SvcMp4Dmx_GetInitDefaultCfg(&InitCfg);
            if (Err != DEMUXER_OK) {
                AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "SvcMp4Dmx_GetInitDefaultCfg failed (%u)", Err, 0U, 0U, 0U, 0U);
            }

            InitCfg.MaxHdlr = (UINT8)(pInitInfo->DemuxNum * MAX_PIPE_NUM * MAX_FORMAT_NUM);
            AmbaMisra_TypeCast(&(InitCfg.Buffer), &BufAddr);
            Err = SvcMp4Dmx_GetInitBufferSize(InitCfg.MaxHdlr, &(InitCfg.BufferSize));
            if (Err != DEMUXER_OK) {
                AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "SvcMp4Dmx_GetInitBufferSize failed (%u)", Err, 0U, 0U, 0U, 0U);
                Rval = PLAYER_ERROR_GENERAL_ERROR;
            }

            if (Rval == PLAYER_OK) {
                if (RSize < InitCfg.BufferSize) {
                    AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "SvcMp4Dmx buffer is not enough (%u/%u)", RSize, InitCfg.BufferSize, 0U, 0U, 0U);
                    Rval = PLAYER_ERROR_ARG;
                } else {
                    Err = SvcMp4Dmx_Init(&InitCfg);
                    if (Err != DEMUXER_OK) {
                        AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "SvcMp4Dmx_Init failed (%u)", Err, 0U, 0U, 0U, 0U);
                        Rval = PLAYER_ERROR_GENERAL_ERROR;
                    } else {
                        RSize   -= GetAlignedValU32(InitCfg.BufferSize, (UINT32)AMBA_CACHE_LINE_SIZE);
                        BufAddr += GetAlignedValU32(InitCfg.BufferSize, (UINT32)AMBA_CACHE_LINE_SIZE);
                    }
                }
            }
        }

        /* Init fmp4 demuxer */
        if (Rval == PLAYER_OK) {
            SVC_FMP4_DMX_INIT_CFG_s InitCfg;

            Err = SvcFmp4Dmx_GetInitDefaultCfg(&InitCfg);
            if (Err != DEMUXER_OK) {
                AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "SvcFmp4Dmx_GetInitDefaultCfg failed (%u)", Err, 0U, 0U, 0U, 0U);
            }

            InitCfg.MaxHdlr = (UINT8)(pInitInfo->DemuxNum * MAX_PIPE_NUM * MAX_FORMAT_NUM);
            AmbaMisra_TypeCast(&(InitCfg.Buffer), &BufAddr);
            Err = SvcFmp4Dmx_GetInitBufferSize(InitCfg.MaxHdlr, &(InitCfg.BufferSize));
            if (Err != DEMUXER_OK) {
                AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "SvcFmp4Dmx_GetInitBufferSize failed (%u)", Err, 0U, 0U, 0U, 0U);
                Rval = PLAYER_ERROR_GENERAL_ERROR;
            }

            if (Rval == PLAYER_OK) {
                if (RSize < InitCfg.BufferSize) {
                    AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "SvcFmp4Dmx buffer is not enough (%u/%u)", RSize, InitCfg.BufferSize, 0U, 0U, 0U);
                    Rval = PLAYER_ERROR_ARG;
                } else {
                    Err = SvcFmp4Dmx_Init(&InitCfg);
                    if (Err != DEMUXER_OK) {
                        AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "SvcFmp4Dmx_Init failed (%u)", Err, 0U, 0U, 0U, 0U);
                        Rval = PLAYER_ERROR_GENERAL_ERROR;
                    } else {
                        RSize   -= GetAlignedValU32(InitCfg.BufferSize, (UINT32)AMBA_CACHE_LINE_SIZE);
                        BufAddr += GetAlignedValU32(InitCfg.BufferSize, (UINT32)AMBA_CACHE_LINE_SIZE);
                    }
                }
            }
        }

        if (Rval == PLAYER_OK) {
            Init = 1U;
        }
    }

    AmbaMisra_TouchUnused(&RSize);
    AmbaMisra_TouchUnused(&BufAddr);

    return Rval;
}

/**
* parse file
* @param [in]  MediaType media type
* @param [in]  FileName FileName
* @param [in]  pStreamHdlr pointer to stream handler
* @param [out]  pMovInfo media info
* @param [out]  pContainerType container type
* @return ErrorCode
*/
UINT32 AmbaDmux_Parse(UINT32 MediaType, const char *FileName, SVC_STREAM_HDLR_s *pStreamHdlr, SVC_MOVIE_INFO_s *pMovInfo, UINT8 *pContainerType)
{
    UINT32             Rval = PLAYER_OK, Err;

    AmbaMisra_TouchUnused(&Rval);

    /* input parameters check */
    if (Rval == PLAYER_OK) {
        if (FileName == NULL) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "FileName is a pointer to null", 0U, 0U, 0U, 0U, 0U);
            Rval = PLAYER_ERROR_ARG;
        }
    }

    /* parse the file */
    if (Rval == PLAYER_OK) {
        if (pStreamHdlr == NULL) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "pStreamHdlr is a pointer to null", 0U, 0U, 0U, 0U, 0U);
            Rval = PLAYER_ERROR_ARG;
        } else {
            Err = pStreamHdlr->Func->Open(pStreamHdlr, FileName, SVC_STREAM_MODE_RDONLY);
            if (Err != PLAYER_OK) {
                AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "Stream open failed (%u)", Err, 0U, 0U, 0U, 0U);
                Rval = PLAYER_ERROR_GENERAL_ERROR;
            }

            if (Rval == PLAYER_OK) {
                UINT8 Type;
                Err = SvcDemuxer_GetFormatType(pStreamHdlr, &Type);
                if (Err != DEMUXER_OK) {
                    AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "SvcDemuxer_GetFormatType failed (%u)", Err, 0U, 0U, 0U, 0U);
                    Rval = PLAYER_ERROR_GENERAL_ERROR;
                } else {
                    if (Type == SVC_DEMUXER_FORMAT_TYPE_MP4) {
                        *pContainerType = AMBA_DMUX_CTYPE_MP4;
                    } else if (Type == SVC_DEMUXER_FORMAT_TYPE_FMP4) {
                        *pContainerType = AMBA_DMUX_CTYPE_FMP4;
                    } else {
                        AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "Unknown container type (%u)", Type, 0U, 0U, 0U, 0U);
                        Rval = PLAYER_ERROR_GENERAL_ERROR;
                    }
                }
            }

            if (Rval == PLAYER_OK) {
                Err = AmbaWrap_memset(pMovInfo, 0, sizeof(SVC_MOVIE_INFO_s));
                if (Err != PLAYER_OK) {
                    AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaWrap_memset failed (%u)", Err, 0U, 0U, 0U, 0U);
                }
                if (MediaType == AMBA_DMUX_MTYPE_IMAGE) {
                    pMovInfo->MediaInfo.MediaType    = (UINT8)SVC_MEDIA_INFO_IMAGE;
                } else {
                    pMovInfo->MediaInfo.MediaType    = (UINT8)SVC_MEDIA_INFO_MOVIE;
                }
                AmbaUtility_StringCopy(pMovInfo->MediaInfo.Name, sizeof(pMovInfo->MediaInfo.Name), FileName);

                if (*pContainerType == AMBA_DMUX_CTYPE_MP4) {
                    Err = SvcMp4Dmx_Parse(&(pMovInfo->MediaInfo), pStreamHdlr);
                    if (Err != PLAYER_OK) {
                        AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "SvcMp4Dmx_Parse failed (%u)", Err, 0U, 0U, 0U, 0U);
                        Rval = PLAYER_ERROR_GENERAL_ERROR;
                    }
                } else if (*pContainerType == AMBA_DMUX_CTYPE_FMP4) {
                    Err = SvcFmp4Dmx_Parse(&(pMovInfo->MediaInfo), pStreamHdlr);
                    if (Err != PLAYER_OK) {
                        AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "SvcFmp4Dmx_Parse failed (%u)", Err, 0U, 0U, 0U, 0U);
                        Rval = PLAYER_ERROR_GENERAL_ERROR;
                    }
                } else {
                    AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "Unknown container type (%u)", *pContainerType, 0U, 0U, 0U, 0U);
                    Rval = PLAYER_ERROR_GENERAL_ERROR;
                }
            }

            Err = pStreamHdlr->Func->Close(pStreamHdlr);
            if (Err != PLAYER_OK) {
                AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "Stream close failed (%u)", Err, 0U, 0U, 0U, 0U);
                Rval = PLAYER_ERROR_GENERAL_ERROR;
            }
        }
    }

    return Rval;
}

/**
* create demuxer
* @param [in]  pDmxCtrl pointer to DmxCtrl handler
* @param [in]  pCreate create info
* @return ErrorCode
*/
UINT32 AmbaDmux_Create(AMBA_DMUX_CTRL *pDmxCtrl, const AMBA_DMUX_CREATE_s *pCreate)
{
    static UINT32            DmxPrivSize = (UINT32)sizeof(AMBA_DMUX_INFO_s);
    UINT32                   Rval = PLAYER_OK, Err, Reserved = (UINT32)sizeof(AMBA_DMUX_CTRL);
    AMBA_DMUX_FORMAT_INFO_s  *pFormatInfo;
    AMBA_DMUX_PIPE_INFO_s    *pPipeInfo;
    AMBA_DMUX_INFO_s         *pDmx;

    AmbaMisra_TouchUnused(&Rval);
    AmbaMisra_TouchUnused(&pFormatInfo);
    AmbaMisra_TouchUnused(&pPipeInfo);
    AmbaMisra_TouchUnused(pDmxCtrl);

    AmbaMisra_TypeCast(&pDmx, &pDmxCtrl);

    if (Rval == PLAYER_OK) {
        if (DmxPrivSize > Reserved) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AMBA_DMUX_CTRL size not enough (%u/%u)", DmxPrivSize, Reserved, 0U, 0U, 0U);
            Rval = PLAYER_ERROR_ARG;
        }
    }

    /* input parameters check */
    if (Rval == PLAYER_OK) {
        if (pCreate->Direction > SVC_FORMAT_DIR_BACKWARD) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "invalid Direction (%u/%u)", pCreate->Direction, SVC_FORMAT_DIR_BACKWARD, 0U, 0U, 0U);
            Rval = PLAYER_ERROR_ARG;
        }
        if ((pCreate->Speed != 1U) && (pCreate->Speed != 2U) && (pCreate->Speed != 4U) && (pCreate->Speed != 8U) && (pCreate->Speed != 16U)) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "invalid Speed (%u)", pCreate->Speed, 0U, 0U, 0U, 0U);
            Rval = PLAYER_ERROR_ARG;
        }
    }

    if (Rval == PLAYER_OK) {
        Err = AmbaWrap_memset(pDmx, 0, sizeof(AMBA_DMUX_INFO_s));
        if (Err != PLAYER_OK) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaWrap_memset failed (%u)", Err, 0U, 0U, 0U, 0U);
        }

        pFormatInfo = &(pDmx->PipeInfo[0].FormatInfo[0]);
        pPipeInfo   = &(pDmx->PipeInfo[0]);

        pDmx->PipeCount           = 1U;
        pPipeInfo->FormatCount    = 1U;
        pPipeInfo->StartTime      = pCreate->StartTime;
        pPipeInfo->Direction      = pCreate->Direction;
        pPipeInfo->Speed          = (UINT8)pCreate->Speed;
        pPipeInfo->pStreamHdlr    = pCreate->pStreamHdlr;
        pPipeInfo->ContainerType  = pCreate->ContainerType;
        pFormatInfo->pMovInfo     = pCreate->pMovInfo;
    }

    /* open file stream */
    if (Rval == PLAYER_OK) {
        const char *FileName = pDmx->PipeInfo[0].FormatInfo[0].pMovInfo->MediaInfo.Name;

        Err = pPipeInfo->pStreamHdlr->Func->Open(pPipeInfo->pStreamHdlr, FileName, SVC_STREAM_MODE_RDONLY);
        if (Err != PLAYER_OK) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "Stream open failed (%u)", Err, 0U, 0U, 0U, 0U);
            Rval = PLAYER_ERROR_GENERAL_ERROR;
        }
    }

    if (Rval == PLAYER_OK) {
        /* create mp4 demuxer */
        if (pPipeInfo->ContainerType == AMBA_DMUX_CTYPE_MP4) {
            SVC_MP4_DMX_CFG_s Mp4Cfg;

            Err = SvcMp4Dmx_GetDefaultCfg(&Mp4Cfg);
            if (Err != DEMUXER_OK) {
                AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "SvcMp4Dmx_GetDefaultCfg failed (%u)", Err, 0U, 0U, 0U, 0U);
            }

            Mp4Cfg.Stream = pPipeInfo->pStreamHdlr;
            Err = SvcMp4Dmx_Create(&Mp4Cfg, &(pFormatInfo->pHdlr));

            if (Err != DEMUXER_OK) {
                AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "SvcMp4Dmx_Create failed (%u)", Err, 0U, 0U, 0U, 0U);
                Rval = PLAYER_ERROR_GENERAL_ERROR;
            }

        /* create fmp4 demuxer */
        } else if (pPipeInfo->ContainerType == AMBA_DMUX_CTYPE_FMP4) {
            SVC_FMP4_DMX_CFG_s Fmp4Cfg;

            Err = SvcFmp4Dmx_GetDefaultCfg(&Fmp4Cfg);
            if (Err != DEMUXER_OK) {
                AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "SvcFmp4Dmx_GetDefaultCfg failed (%u)", Err, 0U, 0U, 0U, 0U);
            }

            Fmp4Cfg.Stream = pPipeInfo->pStreamHdlr;
            Err = SvcFmp4Dmx_Create(&Fmp4Cfg, &(pFormatInfo->pHdlr));

            if (Err != DEMUXER_OK) {
                AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "SvcFmp4Dmx_Create failed (%u)", Err, 0U, 0U, 0U, 0U);
                Rval = PLAYER_ERROR_GENERAL_ERROR;
            }

        } else {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "Unknown ContainerType (%u)", pPipeInfo->ContainerType, 0U, 0U, 0U, 0U);
            Rval = PLAYER_ERROR_ARG;
        }
    }

    /* create demuxer */
    if (Rval == PLAYER_OK) {
        SVC_DEMUXER_PIPE_CFG_s DmxCfg;

        Err = SvcDemuxer_GetDefaultCfg(&DmxCfg);
        if (Err != DEMUXER_OK) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "SvcDemuxer_GetDefaultCfg failed (%u)", Err, 0U, 0U, 0U, 0U);
        }

        DmxCfg.FormatCount = 1U;
        DmxCfg.Format[0]   = pFormatInfo->pHdlr;
        DmxCfg.Speed       = (UINT8)pPipeInfo->Speed;
        DmxCfg.Media[0]    = &(pFormatInfo->pMovInfo->MediaInfo);

        Err = SvcDemuxer_Create(&DmxCfg, &(pDmx->PipeInfo[0].pHdlr));
        if (Err != DEMUXER_OK) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "SvcDemuxer_Create failed (%u)", Err, 0U, 0U, 0U, 0U);
            Rval = PLAYER_ERROR_GENERAL_ERROR;
        }
    }

    return Rval;
}

/**
* start demuxer
* @param [in]  pDmxCtrl pointer to DmxCtrl handler
* @return ErrorCode
*/
UINT32 AmbaDmux_Start(AMBA_DMUX_CTRL *pDmxCtrl)
{
    UINT32                  Rval = PLAYER_OK, Err;
    const AMBA_DMUX_INFO_s  *pDmx;

    AmbaMisra_TouchUnused(&Rval);
    AmbaMisra_TouchUnused(pDmxCtrl);

    AmbaMisra_TypeCast(&pDmx, &pDmxCtrl);

    /* start demuxer */
    if (Rval == PLAYER_OK) {
        const AMBA_DMUX_PIPE_INFO_s *pPipeInfo = &(pDmx->PipeInfo[0]);

        if ((pPipeInfo->Direction == 0U) && (pPipeInfo->StartTime == 0U) && (pPipeInfo->Speed == 1U)) {
            Err = SvcDemuxer_Start(pPipeInfo->pHdlr);
            if (Err != DEMUXER_OK) {
                AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "SvcDemuxer_Start failed (%u)", Err, 0U, 0U, 0U, 0U);
                Rval = PLAYER_ERROR_GENERAL_ERROR;
            }
        } else {
            Err = SvcDemuxer_Seek(pPipeInfo->pHdlr, pPipeInfo->StartTime, (UINT8)(pPipeInfo->Direction), pPipeInfo->Speed);
            if (Err != DEMUXER_OK) {
                AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "SvcDemuxer_Seek failed (%u)", Err, 0U, 0U, 0U, 0U);
                Rval = PLAYER_ERROR_GENERAL_ERROR;
            }
        }
    }

    return Rval;
}

/**
* stop demuxer
* @param [in]  pDmxCtrl pointer to DmxCtrl handler
* @return ErrorCode
*/
UINT32 AmbaDmux_Stop(AMBA_DMUX_CTRL *pDmxCtrl)
{
    UINT32                  Rval = PLAYER_OK, Err;
    const AMBA_DMUX_INFO_s  *pDmx;

    AmbaMisra_TouchUnused(&Rval);
    AmbaMisra_TouchUnused(pDmxCtrl);

    AmbaMisra_TypeCast(&pDmx, &pDmxCtrl);

    /* start demuxer */
    if (Rval == PLAYER_OK) {
        const AMBA_DMUX_PIPE_INFO_s *pPipeInfo = &(pDmx->PipeInfo[0]);

        Err = SvcDemuxer_Stop(pPipeInfo->pHdlr);
        if (Err != DEMUXER_OK) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "SvcDemuxer_Stop failed (%u)", Err, 0U, 0U, 0U, 0U);
        }
    }

    return Rval;
}

/**
* delete demuxer
* @param [in]  pDmxCtrl pointer to DmxCtrl handler
* @return ErrorCode
*/
UINT32 AmbaDmux_Delete(AMBA_DMUX_CTRL *pDmxCtrl)
{
    UINT32                       Rval = PLAYER_OK, Err;
    const AMBA_DMUX_PIPE_INFO_s  *pPipeInfo;
    const AMBA_DMUX_INFO_s       *pDmx;

    AmbaMisra_TouchUnused(&Rval);
    AmbaMisra_TouchUnused(&pPipeInfo);
    AmbaMisra_TouchUnused(pDmxCtrl);

    AmbaMisra_TypeCast(&pDmx, &pDmxCtrl);

    /* delete demuxer */
    if (Rval == PLAYER_OK) {
        pPipeInfo = &(pDmx->PipeInfo[0]);

        Err = SvcDemuxer_Delete(pPipeInfo->pHdlr);
        if (Err != DEMUXER_OK) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "SvcDemuxer_Delete failed (%u)", Err, 0U, 0U, 0U, 0U);
            Rval = PLAYER_ERROR_GENERAL_ERROR;
        }
    }

    if (Rval == PLAYER_OK) {
        const AMBA_DMUX_FORMAT_INFO_s *pFormatInfo = &(pDmx->PipeInfo[0].FormatInfo[0]);

        /* delete mp4 demuxer */
        if (pPipeInfo->ContainerType == AMBA_DMUX_CTYPE_MP4) {
            Err = SvcMp4Dmx_Delete(pFormatInfo->pHdlr);
            if (Err != DEMUXER_OK) {
                AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "SvcMp4Dmx_Delete failed (%u)", Err, 0U, 0U, 0U, 0U);
                Rval = PLAYER_ERROR_GENERAL_ERROR;
            }

        /* delete fmp4 demuxer */
        } else if (pPipeInfo->ContainerType == AMBA_DMUX_CTYPE_FMP4) {
            Err = SvcFmp4Dmx_Delete(pFormatInfo->pHdlr);
            if (Err != DEMUXER_OK) {
                AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "SvcFmp4Dmx_Delete failed (%u)", Err, 0U, 0U, 0U, 0U);
                Rval = PLAYER_ERROR_GENERAL_ERROR;
            }

        } else {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "Unknown ContainerType (%u)", pPipeInfo->ContainerType, 0U, 0U, 0U, 0U);
            Rval = PLAYER_ERROR_GENERAL_ERROR;
        }
    }

    /* close file stream */
    if (Rval == PLAYER_OK) {
        Err = pPipeInfo->pStreamHdlr->Func->Close(pPipeInfo->pStreamHdlr);
        if (Err != PLAYER_OK) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "Stream close failed (%u)", Err, 0U, 0U, 0U, 0U);
            Rval = PLAYER_ERROR_GENERAL_ERROR;
        }
    }

    return Rval;
}

/**
* demuxer process
* @param [in]  pDmxCtrl pointer to DmxCtrl handler
* @param [in]  FrameCount frame count
* @param [out]  pEvent event
* @return ErrorCode
*/
UINT32 AmbaDmux_Process(AMBA_DMUX_CTRL *pDmxCtrl, UINT8 FrameCount, UINT8 *pEvent)
{
    UINT32                    Rval = PLAYER_OK, Err;
    const AMBA_DMUX_INFO_s    *pDmx;

    AmbaMisra_TouchUnused(&Rval);
    AmbaMisra_TouchUnused(pDmxCtrl);

    AmbaMisra_TypeCast(&pDmx, &pDmxCtrl);

    if (Rval == PLAYER_OK) {
        const AMBA_DMUX_PIPE_INFO_s *pPipeInfo = &(pDmx->PipeInfo[0]);

        Err = SvcDemuxer_Process(pPipeInfo->pHdlr, FrameCount, pEvent);

        if (Err != DEMUXER_OK) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "SvcDemuxer_Process failed (%u)", Err, 0U, 0U, 0U, 0U);
            Rval = PLAYER_ERROR_GENERAL_ERROR;
        }
    }

    return Rval;
}

/**
* feed frame
* @param [in]  pDmxCtrl pointer to DmxCtrl handler
* @param [in]  TrackId track id
* @param [in]  TargetTime target time
* @param [in]  FrameType frame type
* @return ErrorCode
*/
UINT32 AmbaDmux_FeedFrame(AMBA_DMUX_CTRL *pDmxCtrl, UINT8 TrackId, UINT32 TargetTime, UINT8 FrameType)
{
    UINT32                    Rval = PLAYER_OK, Err;
    const AMBA_DMUX_INFO_s    *pDmx;

    AmbaMisra_TouchUnused(&Rval);
    AmbaMisra_TouchUnused(pDmxCtrl);

    AmbaMisra_TypeCast(&pDmx, &pDmxCtrl);

    if (Rval == PLAYER_OK) {
        SVC_DMX_FORMAT_HDLR_s        *pHdlr = pDmx->PipeInfo[0].FormatInfo[0].pHdlr;

        Err = pHdlr->Func->Open(pHdlr);
        if (Err != DEMUXER_OK) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaDmux_FeedFrame Open failed (%u)", Err, 0U, 0U, 0U, 0U);
            Rval = PLAYER_ERROR_GENERAL_ERROR;
        }

        if (Rval == PLAYER_OK) {
            Err = pHdlr->Func->FeedFrame(pHdlr, TrackId, TargetTime, FrameType);
            if (Err != DEMUXER_OK) {
                AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaDmux_FeedFrame FeedFrame failed (%u)", Err, 0U, 0U, 0U, 0U);
                Rval = PLAYER_ERROR_GENERAL_ERROR;
            }
        }

        if (Rval == PLAYER_OK) {
            Err = pHdlr->Func->Close(pHdlr);
            if (Err != DEMUXER_OK) {
                AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaDmux_FeedFrame Close failed (%u)", Err, 0U, 0U, 0U, 0U);
                Rval = PLAYER_ERROR_GENERAL_ERROR;
            }
        }
    }

    return Rval;
}
