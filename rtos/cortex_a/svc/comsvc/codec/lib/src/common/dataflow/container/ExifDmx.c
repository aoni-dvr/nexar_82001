/**
 * @file ExifDmx.c
 *
 * Copyright (c) 2020 Ambarella International LP
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
#include "format/SvcExifDmx.h"
#include "FormatAPI.h"
#include "Tiff.h"
#include "ByteOp.h"
#include "MemByteOp.h"
#include "demuxer/DemuxerImpl.h"
#include <AmbaDef.h>

//#define DEBUG_MSG
#define SVC_EXIFDMX_HEAD_SIZE    0x400U /**< Default Header Size */

/**
 *  ExifDmx handler
 */
typedef struct {
    SVC_DMX_FORMAT_HDLR_s Hdlr; /**< Format handler, must be the first member */
    AMBA_KAL_MUTEX_t Mutex;     /**< The mutex of exif demux handler */
    UINT8 Used;                 /**< The flag to indicate a handler is used or not */
    UINT8 Open;                 /**< The flag to indicate a handler is opened or not */
} SVC_EXIF_DMX_HDLR_s;

/**
 *  ExifDmx manager
 */
typedef struct {
    AMBA_KAL_MUTEX_t Mutex;     /**< The mutex of exif demux manager */
    SVC_EXIF_DMX_HDLR_s *Hdlr;  /**< ExifDmx format handlers */
    UINT8 MaxHdlr;              /**< The flag to indicate a handler is used or not */
} AMBA_EXIF_DMX_MGR_s;

static AMBA_EXIF_DMX_MGR_s g_ExifdmxMgr GNU_SECTION_NOZEROINIT;   /**< The exif demux manager */

static UINT32 SvcExifDmx_Open(SVC_DMX_FORMAT_HDLR_s *Hdlr);     /**< The interface to open an Exif Format handler */
static UINT32 SvcExifDmx_Close(SVC_DMX_FORMAT_HDLR_s *Hdlr);    /**< The interface to close an Exif Format handler */
static UINT32 SvcExifDmx_Process(SVC_DMX_FORMAT_HDLR_s *Hdlr, UINT64 Param, UINT8 *Event);  /**< The interface to process an Exif Format handler */

static UINT32 SvcExifDmx_GetInitBufSize_ParamCheck(UINT8 MaxHdlr, const UINT32 *BufferSize)
{
    UINT32 Rval = DEMUXER_ERR_INVALID_ARG;
    if (MaxHdlr > 0U) {
        if (BufferSize != NULL) {
            Rval = DEMUXER_OK;
        }
    }
    return Rval;
}

/**
 * Get the required buffer size
 *
 * Get the required buffer size for initializing the ExifDmx module.
 * @param [in] MaxHdlr The maximum number of ExifDmx handlers
 * @param [out] BufferSize The required buffer size.
 * @return 0 - OK, others - DEMUXER_ERR_XXXX
 */
UINT32 SvcExifDmx_GetInitBufferSize(UINT8 MaxHdlr, UINT32 *BufferSize)
{
    UINT32 Rval = SvcExifDmx_GetInitBufSize_ParamCheck(MaxHdlr, BufferSize);
    if (Rval == DEMUXER_OK) {
        *BufferSize = GetAlignedValU32((UINT32)MaxHdlr * (UINT32)sizeof(SVC_EXIF_DMX_HDLR_s), (UINT32)AMBA_CACHE_LINE_SIZE);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Invalid agruement!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 SvcExifDmx_GetInitDefaultCfg_ParamCheck(const SVC_EXIF_DMX_INIT_CFG_s *Config)
{
    UINT32 Rval = DEMUXER_ERR_INVALID_ARG;
    if (Config != NULL) {
        Rval = DEMUXER_OK;
    }
    return Rval;
}

/**
 * Get the default Exif demuxer manager config
 *
 * @param [out] Config: The buffer to get default config.
 * @return 0 - OK, others - DEMUXER_ERR_XXXX
 */
UINT32 SvcExifDmx_GetInitDefaultCfg(SVC_EXIF_DMX_INIT_CFG_s *Config)
{
    UINT32 Rval = SvcExifDmx_GetInitDefaultCfg_ParamCheck(Config);
    if (Rval == DEMUXER_OK) {
        Rval = W2D(AmbaWrap_memset(Config, 0, sizeof(SVC_EXIF_DMX_INIT_CFG_s)));
        if (Rval == DEMUXER_OK) {
            Config->MaxHdlr = 1;
            Rval = SvcExifDmx_GetInitBufferSize(Config->MaxHdlr, &Config->BufferSize);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Invalid agruement!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 SvcExifDmx_Init_ParamCheck(const SVC_EXIF_DMX_INIT_CFG_s *Config)
{
    UINT32 Rval = DEMUXER_OK;
    if (Config != NULL) {
        UINT32 Size = 0U;
        Rval = SvcExifDmx_GetInitBufferSize(Config->MaxHdlr, &Size);
        if (Rval == DEMUXER_OK) {
            if (Config->BufferSize != Size) {
                AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "incorrect buffer Size %u", Config->BufferSize, 0U, 0U, 0U, 0U);
                Rval = DEMUXER_ERR_INVALID_ARG;
            }
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Invalid argument!", __func__, NULL, NULL, NULL, NULL);
        Rval = DEMUXER_ERR_INVALID_ARG;
    }
    return Rval;
}

#define MAX_EXIFDMX_NUM  (16U)  /**< Maximum number of ExifDmx handlers */
static UINT32 CreateMutex(void)
{
    static char DmxMgrMutexName[16] = "ExifDmx_Mgr";
    UINT32 Rval = K2D(AmbaKAL_MutexCreate(&g_ExifdmxMgr.Mutex, DmxMgrMutexName));
    if (Rval == DEMUXER_OK) {
        static char MutexName[MAX_EXIFDMX_NUM][16];
        UINT32 i;
        for (i = 0; i < g_ExifdmxMgr.MaxHdlr; i++) {
            SVC_EXIF_DMX_HDLR_s *Hdlr = &g_ExifdmxMgr.Hdlr[i];
            UINT32 Len = AmbaUtility_StringPrintUInt32(MutexName[i], (UINT32)sizeof(MutexName[i]), "ExifDmx%02u", 1, &i);
            Rval = K2D(AmbaKAL_MutexCreate(&Hdlr->Mutex, MutexName[i]));
            if ((Rval != DEMUXER_OK) || (i >= MAX_EXIFDMX_NUM)) {
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

/**
 * Initiate Exif demuxer manager
 *
 * @param [in] Config: The initial configure.
 * @return 0 - OK, others - DEMUXER_ERR_XXXX
 */
UINT32 SvcExifDmx_Init(const SVC_EXIF_DMX_INIT_CFG_s *Config)
{
    static UINT8 g_ExifDmxMgrInit = 0U;
    UINT32 Rval = SvcExifDmx_Init_ParamCheck(Config);
    if (Rval == DEMUXER_OK) {
        if (g_ExifDmxMgrInit == 0U) {
            Rval = W2D(AmbaWrap_memset(Config->Buffer, 0, Config->BufferSize));
            if (Rval == DEMUXER_OK) {
                AmbaMisra_TypeCast(&g_ExifdmxMgr.Hdlr, &Config->Buffer);
                g_ExifdmxMgr.MaxHdlr = Config->MaxHdlr;
                g_ExifDmxMgrInit = 1U;
                Rval = CreateMutex();
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Already initiated!", __func__, NULL, NULL, NULL, NULL);
            Rval = DEMUXER_ERR_FATAL_ERROR;
        }
    }
    return Rval;
}


static UINT32 SvcExifDmx_GetDefaultCfg_ParamCheck(const SVC_EXIF_DMX_CFG_s *Config)
{
    UINT32 Rval = DEMUXER_ERR_INVALID_ARG;
    if (Config != NULL) {
        Rval = DEMUXER_OK;
    }
    return Rval;
}

/**
 * Get the default Exif demuxer config
 *
 * @param [out] Config: The buffer to get default configure.
 * @return 0 - OK, others - DEMUXER_ERR_XXXX
 */
UINT32 SvcExifDmx_GetDefaultCfg(SVC_EXIF_DMX_CFG_s *Config)
{
    UINT32 Rval = SvcExifDmx_GetDefaultCfg_ParamCheck(Config);
    if (Rval == DEMUXER_OK) {
        Rval = W2D(AmbaWrap_memset(Config, 0, sizeof(SVC_EXIF_DMX_CFG_s)));
    }
    return Rval;
}

static UINT32 GetHdlr(SVC_EXIF_DMX_HDLR_s **Hdlr)
{
    UINT32 Rval = DEMUXER_ERR_FATAL_ERROR;
    UINT32 i;
    for (i = 0; i < g_ExifdmxMgr.MaxHdlr; i++) {
        SVC_EXIF_DMX_HDLR_s *ExifDmx = &g_ExifdmxMgr.Hdlr[i];
        if (ExifDmx->Used == 0U) {
            ExifDmx->Used = 1U;
            *Hdlr = ExifDmx;
            Rval = DEMUXER_OK;
            break;
        }
    }
    if (i == g_ExifdmxMgr.MaxHdlr) {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Get Hdlr fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 RelHdlr(SVC_EXIF_DMX_HDLR_s *Hdlr)
{
    UINT32 Rval = DEMUXER_ERR_INVALID_ARG;
    UINT32 i;
    for (i = 0; i < g_ExifdmxMgr.MaxHdlr; i++) {
        const SVC_EXIF_DMX_HDLR_s *ExifDmx = &g_ExifdmxMgr.Hdlr[i];
        if (ExifDmx == Hdlr) {
            if (Hdlr->Used != 0U) {
                Hdlr->Used = 0U;
                Rval = DEMUXER_OK;
            } else {
                // error...
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s ExifDmx idle!", __func__, NULL, NULL, NULL, NULL);
                Rval = DEMUXER_ERR_FATAL_ERROR;
            }
            break;
        }
    }
    if (i == g_ExifdmxMgr.MaxHdlr) {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Get Hdlr fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 SvcExifDmx_CreateImpl(const SVC_EXIF_DMX_CFG_s *Config, SVC_EXIF_DMX_HDLR_s **Hdlr)
{
    UINT32 Rval;
    SVC_EXIF_DMX_HDLR_s *ExifDmx;
    static SVC_DMX_FORMAT_s g_ExifDmx = {
        SvcExifDmx_Open,
        SvcExifDmx_Close,
        SvcExifDmx_Process,
        NULL,
        NULL,
        NULL
    };
    Rval = GetHdlr(&ExifDmx);
    if (Rval == DEMUXER_OK) {
        Rval = K2D(AmbaKAL_MutexTake(&ExifDmx->Mutex, SVC_FORMAT_WAIT_TIMEOUT));
        if (Rval == DEMUXER_OK) {
            SvcDemuxer_InitImageDmxHdlr(&ExifDmx->Hdlr, &g_ExifDmx, Config->Stream);
            *Hdlr = ExifDmx;
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Take mutex fail!", __func__, NULL, NULL, NULL, NULL);
        }
        if (AmbaKAL_MutexGive(&ExifDmx->Mutex) != OK) {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Give mutex fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = DEMUXER_ERR_FATAL_ERROR;
        }
    } else {
        Rval = DEMUXER_ERR_FATAL_ERROR;
    }
    return Rval;
}

static inline UINT32 SvcExifDmx_Create_ParamCheck(const SVC_EXIF_DMX_CFG_s *Config, SVC_DMX_FORMAT_HDLR_s * const *Hdlr)
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
 * Create a Exif demux handler
 *
 * @param [in] Config The configure of the exif demuxer.
 * @param [out] Hdlr The double pointer to get the handler.
 * @return 0 - OK, others - DEMUXER_ERR_XXXX
 */
UINT32 SvcExifDmx_Create(const SVC_EXIF_DMX_CFG_s *Config, SVC_DMX_FORMAT_HDLR_s **Hdlr)
{
    UINT32 Rval = SvcExifDmx_Create_ParamCheck(Config, Hdlr);
    if (Rval == DEMUXER_OK) {
        Rval = K2D(AmbaKAL_MutexTake(&g_ExifdmxMgr.Mutex, SVC_FORMAT_WAIT_TIMEOUT));
        if (Rval == DEMUXER_OK) {
            SVC_EXIF_DMX_HDLR_s *ExifDmx;
            Rval = SvcExifDmx_CreateImpl(Config, &ExifDmx);
            if (Rval == DEMUXER_OK) {
                *Hdlr = &ExifDmx->Hdlr;
            }
            if (AmbaKAL_MutexGive(&g_ExifdmxMgr.Mutex) != OK) {
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


static UINT32 SvcExifDmx_DeleteImpl(SVC_EXIF_DMX_HDLR_s *Hdlr)
{
    UINT32 Rval = K2D(AmbaKAL_MutexTake(&Hdlr->Mutex, SVC_FORMAT_WAIT_TIMEOUT));
    if (Rval == DEMUXER_OK) {
        Rval = RelHdlr(Hdlr);
        if (AmbaKAL_MutexGive(&Hdlr->Mutex) != OK) {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Give mutex failed!", __func__, NULL, NULL, NULL, NULL);
            Rval = DEMUXER_ERR_FATAL_ERROR;
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Take mutex fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static inline UINT32 SvcExifDmx_Delete_ParamCheck(const SVC_DMX_FORMAT_HDLR_s *Hdlr)
{
    UINT32 Rval = DEMUXER_ERR_INVALID_ARG;
    if (Hdlr != NULL) {
        Rval = DEMUXER_OK;
    }
    return Rval;
}

/**
 * Delete a Exif demuxer handler.
 * @param [in] Hdlr The demuxer handler to close.
 * @return 0 - OK, others - DEMUXER_ERR_XXXX
 */
UINT32 SvcExifDmx_Delete(SVC_DMX_FORMAT_HDLR_s *Hdlr)
{
    UINT32 Rval = SvcExifDmx_Delete_ParamCheck(Hdlr);
    if (Rval == DEMUXER_OK) {
        Rval= K2D(AmbaKAL_MutexTake(&g_ExifdmxMgr.Mutex, SVC_FORMAT_WAIT_TIMEOUT));
        if (Rval == DEMUXER_OK) {
            SVC_EXIF_DMX_HDLR_s *ExifDmx;
            AmbaMisra_TypeCast(&ExifDmx, &Hdlr);
            Rval = SvcExifDmx_DeleteImpl(ExifDmx);
            if (AmbaKAL_MutexGive(&g_ExifdmxMgr.Mutex) != OK) {
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

static UINT32 ExifDmx_SkipTag(SVC_MEM_OP_s *MemOp)
{
    UINT16 Len;
    UINT64 ParsePos = SvcFormat_GetMemPos(MemOp);
    UINT32 Rval = SvcFormat_GetMemBe16(MemOp, &Len);
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_SeekMem(MemOp, (INT64)ParsePos + (INT64)Len, SVC_STREAM_SEEK_START);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetMemBe16() error!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 ExifDmx_CheckTag(SVC_MEM_OP_s *MemOp, UINT16 Marker, UINT16 *Width, UINT16 *Height)
{
    UINT32 Rval = FORMAT_OK;
    switch (Marker) {
    case 0xFFD8U: // SOI
        break;
    case 0xFFC0U: // SOF0
        {
            UINT16 Len;
            UINT64 ParsePos = SvcFormat_GetMemPos(MemOp);
            Rval = SvcFormat_GetMemBe16(MemOp, &Len);
            if (Rval == FORMAT_OK) {
                UINT8 Temp8;
                Rval = SvcFormat_GetMemByte(MemOp, &Temp8);
                if (Rval == FORMAT_OK) {
                    if (Temp8 != 8U) {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Wrong value!", __func__, NULL, NULL, NULL, NULL);
                        Rval = FORMAT_ERR_INVALID_FORMAT;
                    }
                    if (Rval == FORMAT_OK) {
                        Rval = SvcFormat_GetMemBe16(MemOp, Height);
                        if (Rval == FORMAT_OK) {
                            Rval = SvcFormat_GetMemBe16(MemOp, Width);
                            if (Rval == FORMAT_OK) {
                                Rval = SvcFormat_SeekMem(MemOp, (INT64)ParsePos + (INT64)Len, SVC_STREAM_SEEK_START);
                            } else {
                                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetMemBe16() error!", __func__, NULL, NULL, NULL, NULL);
                            }
                        } else {
                            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetMemBe16() error!", __func__, NULL, NULL, NULL, NULL);
                        }
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetMemByte() error!", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetMemBe16() error!", __func__, NULL, NULL, NULL, NULL);
            }
        }
        break;
    case 0xFFC1U: // SOF1, not support now.
    case 0xFFC2U: // SOF2, not support now.
    case 0xFFC3U: // SOF3, not support now.
        AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "ExifDmx_CheckFormat Unsupported marker(%x)!", Marker, 0U, 0U, 0U, 0U);
        Rval = FORMAT_ERR_INVALID_FORMAT;
        break;
    default:
        // 0xFFC4U: DHT (Huffman table)
        // 0xFFDDU: DRI (define restart interval)
        // 0xFFDBU: DQT (Define quatization tables)
        // 0xFFDAU: SOS (Start of scan)
        Rval = ExifDmx_SkipTag(MemOp);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "ExifDmx_CheckFormat Parse 0x%8x error!", (UINT32)Marker, 0U, 0U, 0U, 0U);
        }
        break;
    }
    return Rval;
}
/**
 * Check the frame format
 *
 * @param [in] Image The image
 * @param [in] Stream The stream handler of the media
 * @param [in] MemOp The handler of memory operation
 * @param [in] FrameNo The frame number of the image
 * @param [in] Position The file offset of the frame
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
static UINT32 ExifDmx_CheckFormat(SVC_IMAGE_INFO_s *Image, SVC_STREAM_HDLR_s *Stream, SVC_MEM_OP_s *MemOp, UINT32 FrameNo, UINT64 Position)
{
    UINT32 Rval;
    UINT8 DQT=0, DHT=0, SOF=0, SOS=0;
    Rval = SvcFormat_SeekMem(MemOp, (INT64)Position, SVC_STREAM_SEEK_START);
    if (Rval == FORMAT_OK) {
        const SVC_MEDIA_INFO_s *Media = &Image->MediaInfo;
        do {
            UINT64 ParsePos;
            UINT16 Marker = 0U;
            Rval = S2F(Stream->Func->GetPos(Stream, &ParsePos));
            if (Rval == FORMAT_OK) {
                if ((ParsePos >= Media->Size) || (SOS != 0U)) {
                    break;
                } else {
                    Rval = SvcFormat_GetMemBe16(MemOp, &Marker);
                    if (Rval != FORMAT_OK) {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetMemBe16() error!", __func__, NULL, NULL, NULL, NULL);
                    }
                }
            } else {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Stream->Func->GetPos() error!", __func__, NULL, NULL, NULL, NULL);
            }
            if (Rval == FORMAT_OK) {
                UINT16 Width = 0U, Height = 0U;
                Rval = ExifDmx_CheckTag(MemOp, Marker, &Width, &Height);
                if (Rval == FORMAT_OK) {
                    if (Marker == 0xFFC0U) {
                        Image->Frame[FrameNo].Width = Width;
                        Image->Frame[FrameNo].Height = Height;
                        SOF = 1U;
                    } else if (Marker == 0xFFC4U) {
                        DHT = 1U;
                    } else if (Marker == 0xFFDBU) {
                        DQT = 1U;
                    } else if (Marker == 0xFFDAU) {
                        SOS = 1U;
                    } else {
                        // ok
                    }
                }
            }
        } while (Rval == FORMAT_OK);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_SeekMem() error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        if ((DQT == 0U) || (DHT == 0U) || (SOF == 0U) || (SOS == 0U)) {
            AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "ExifDmx_CheckFormat %u %u %u %u", DQT, DHT, SOF, SOS, 0U);
            Rval = FORMAT_ERR_INVALID_FORMAT;
        }
#if 0
        AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "ExifDmx_CheckFormat Width %u Height %u", (UINT32)*Width, (UINT32)*Height, 0U, 0U, 0U);
#endif
    }
    return Rval;
}

/**
 * Parse the IFD0 entry of exif
 *
 * @param [in] ExifDmx The private information of exif
 * @param [in] MemOp The handler of memory operation
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
static UINT32 ExifDmx_ParseIfd0(SVC_EXIF_INFO_s *ExifDmx, SVC_MEM_OP_s *MemOp)
{
    UINT32 Rval = SvcFormat_SeekMem(MemOp, (INT64)ExifDmx->Ifd0Off, SVC_STREAM_SEEK_START);
    if (Rval == FORMAT_OK) {
        UINT16 Ifd0Entry;
        Rval = SvcFormat_GetMem16(MemOp, &Ifd0Entry);
        if (Rval == FORMAT_OK) {
            UINT16 i, Tag, Type;
            UINT32 Count, Param;
            for (i = 0; i < Ifd0Entry; i++) {
                // TODO: Parse other information
                Rval = SvcFormat_GetIfdEntry(MemOp, &Tag, &Type, &Count, &Param);
                if (Rval == FORMAT_OK) {
                    switch (Tag) {
                    case SVC_TIFF_ExifIFDPointer:
                        ExifDmx->ExifIfdOff = Param;
                        break;
                    case SVC_TIFF_GPSInfoIFDPointer:
                        ExifDmx->GpsIfdOff = Param;
                        break;
                    case SVC_TIFF_Make:
                        break;
                    case SVC_TIFF_Model:
                        break;
                    case SVC_TIFF_Orientation:
                    default:
                        // do nothing
                        break;
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetIfdEntry() error!", __func__, NULL, NULL, NULL, NULL);
                    break;
                }
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetMem16() error!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_SeekMem() error!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 ExifDmx_ParseAmbaBox0x(SVC_IMAGE_INFO_s *Image, SVC_MEM_OP_s *MemOp, UINT32 Count)
{
    UINT32 Rval = SvcFormat_SeekMem(MemOp, -1LL * (INT64)TIFF_EXIF_AMBA_0_1_SIZE, SVC_STREAM_SEEK_CUR);
    if (Rval != FORMAT_OK) {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_SeekMem() error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        UINT32 TagType = 0U, TagSize = 0U;
        Rval = SvcFormat_GetMem32(MemOp, &TagSize);
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetMem32(MemOp, &TagType);
            if (Rval != FORMAT_OK) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetMem32() error!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetMem32() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            if((TagSize == TIFF_EXIF_AMBA_0_1_SIZE) && (TagType == 0x414d4241U/* AMBA */)) {
                Image->ExifInfo.MakerNoteSize = Count - TIFF_EXIF_AMBA_0_1_SIZE;
            }
        }
    }
    return Rval;
}

static UINT32 ExifDmx_ParseAmbaBox1x(SVC_IMAGE_INFO_s *Image, SVC_MEM_OP_s *MemOp, UINT32 Count)
{
    UINT32 TagType = 0U, TagSize = 0U;
    UINT32 Rval = SvcFormat_SeekMem(MemOp, -6LL, SVC_STREAM_SEEK_CUR);
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetMem32(MemOp, &TagSize);
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_SeekMem(MemOp, 2LL - (INT64)TagSize, SVC_STREAM_SEEK_CUR);
            if (Rval == FORMAT_OK) {
                Rval = SvcFormat_GetMem32(MemOp, &TagType);
                if (Rval != FORMAT_OK) {
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetMem32() error!", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_SeekMem() error!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetMem32() error!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_SeekMem() error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        if (TagType == 0x414d4241U /* AMBA */) {
            Image->ExifInfo.MakerNoteSize = Count - TagSize;
        }
    }
    return Rval;
}

/**
 * Parse the AmbaBox of exif
 *
 * @param [in] Image The image information
 * @param [in] MemOp The handler of memory operation
 * @param [in] Count The size of maker note
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
static UINT32 ExifDmx_ParseAmbaBox(SVC_IMAGE_INFO_s *Image, SVC_MEM_OP_s *MemOp, UINT32 Count)
{
    UINT32 Rval;
    UINT8 Ver1 = 0U, Ver2 = 0U;
    /* ver_0_1  size|tag|    data    |version| default: 64 byte
       ver_1_0  tag|    data    |size|version|                */
    Rval = SvcFormat_GetMemByte(MemOp, &Ver1);
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetMemByte(MemOp, &Ver2);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetMemByte() error!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetMemByte() error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        if ((Ver1 == 0U) && (Ver2 == 1U)) { /* AMBA ver_0_1 */
            Rval = ExifDmx_ParseAmbaBox0x(Image, MemOp, Count);
        } else if ((Ver1 == 1U) && (Ver2 == 0U)) { /* AMBA ver_1_0 */
            Rval = ExifDmx_ParseAmbaBox1x(Image, MemOp, Count);
        } else {
            // ok
        }
    }
    return Rval;
}

/**
 * Get the Exif IFD entry of exif (RATIONAL/SRATIONAL)
 *
 * @param [in] MemOp The handler of memory operation
 * @param [in] Tag The id of the tag
 * @param [in] Offset The offset of the tag
 * @param [in] ExifInfo The exif information
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
static UINT32 ExifDmx_GetExifIfd_8byte(SVC_MEM_OP_s *MemOp, UINT16 Tag, UINT32 Offset, EXIF_INFO_s *ExifInfo)
{
    UINT64 Temp64 = SvcFormat_GetMemPos(MemOp);
    UINT32 Rval = SvcFormat_SeekMem(MemOp, (INT64)Offset, SVC_STREAM_SEEK_START);
    if (Rval == FORMAT_OK) {
        switch (Tag) {
        case SVC_EXIFIFD_ExposureTime:
            Rval = SvcFormat_GetMem32(MemOp, &ExifInfo->ExposureTimeNum);
            if (Rval == FORMAT_OK) {
                Rval = SvcFormat_GetMem32(MemOp, &ExifInfo->ExposureTimeDen);
            }
            break;
        case SVC_EXIFIFD_FNumber:
            Rval = SvcFormat_GetMem32(MemOp, &ExifInfo->FNumberNum);
            if (Rval == FORMAT_OK) {
                Rval = SvcFormat_GetMem32(MemOp, &ExifInfo->FNumberDen);
            }
            break;
        case SVC_EXIFIFD_ShutterSpeedValue:
            Rval = SvcFormat_GetMem32(MemOp, &ExifInfo->ShutterSpeedNum);
            if (Rval == FORMAT_OK) {
                Rval = SvcFormat_GetMem32(MemOp, &ExifInfo->ShutterSpeedDen);
            }
            break;
        case SVC_EXIFIFD_ApertureValue:
            Rval = SvcFormat_GetMem32(MemOp, &ExifInfo->ApertureValueNum);
            if (Rval == FORMAT_OK) {
                Rval = SvcFormat_GetMem32(MemOp, &ExifInfo->ApertureValueDen);
            }
            break;
        /*case SVC_EXIFIFD_ExposureBiasValue: INT32,INIT32
            Rval = SvcFormat_GetMem32(MemOp, ExifInfo->ExposureBiasValueNum);
            if (Rval == FORMAT_OK) {
                Rval = SvcFormat_GetMem32(MemOp, ExifInfo->ExposureBiasValueDen);
            }
            break;*/
        case SVC_EXIFIFD_MaxApertureValue:
            Rval = SvcFormat_GetMem32(MemOp, &ExifInfo->MaxApertureValueNum);
            if (Rval == FORMAT_OK) {
                Rval = SvcFormat_GetMem32(MemOp, &ExifInfo->MaxApertureValueDen);
            }
            break;
        case SVC_EXIFIFD_SubjectDistance:
            Rval = SvcFormat_GetMem32(MemOp, &ExifInfo->SubjectDistanceNum);
            if (Rval == FORMAT_OK) {
                Rval = SvcFormat_GetMem32(MemOp, &ExifInfo->SubjectDistanceDen);
            }
            break;
        case SVC_EXIFIFD_FocalLength:
            Rval = SvcFormat_GetMem32(MemOp, &ExifInfo->FocalLengthNum);
            if (Rval == FORMAT_OK) {
                Rval = SvcFormat_GetMem32(MemOp, &ExifInfo->FocalLengthDen);
            }
            break;
        case SVC_EXIFIFD_ExposureIndex: // ExposureIndex > 4bytes
            Rval = SvcFormat_GetMem32(MemOp, &ExifInfo->ExposureIndexNum);
            if (Rval == FORMAT_OK) {
                Rval = SvcFormat_GetMem32(MemOp, &ExifInfo->ExposureIndexDen);
            }
            break;
        default:
            // do nothing
            break;
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_SeekMem(MemOp, (INT64)Temp64, SVC_STREAM_SEEK_START);
            if (Rval != FORMAT_OK) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_SeekMem() error!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "ExifDmx_GetExifIfd_8byte Get(%x) error!", (UINT32)Tag, 0U, 0U, 0U, 0U);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_SeekMem() error!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 ExifDmx_ParseExifIfd_Tag(SVC_MEM_OP_s *MemOp, UINT16 Tag, UINT32 Count, UINT32 Value, const SVC_EXIF_INFO_s *ExifDmx, SVC_IMAGE_INFO_s *Image)
{
    UINT32 Rval = FORMAT_OK;
    switch (Tag) {
    case SVC_EXIFIFD_PixelXDimension:
        Image->Frame[0].Width = (UINT16)Value;
        break;
    case SVC_EXIFIFD_PixelYDimension:
        Image->Frame[0].Height = (UINT16)Value;
        break;
    case SVC_EXIFIFD_ExposureProgram:
        Image->ExifInfo.ExifIfdInfo.ExposureProgram = Value;
        break;
    case SVC_EXIFIFD_MeteringMode:
        Image->ExifInfo.ExifIfdInfo.MeteringMode = (UINT16)Value;
        break;
    case SVC_EXIFIFD_LightSource:
        Image->ExifInfo.ExifIfdInfo.LightSource = (UINT16)Value;
        break;
    case SVC_EXIFIFD_Flash:
        Image->ExifInfo.ExifIfdInfo.Flash = (UINT16)Value;
        break;
    case SVC_EXIFIFD_MakerNote:
        {
            UINT64 Temp64 = SvcFormat_GetMemPos(MemOp);
            Image->ExifInfo.MakerNoteSize = Count;
            if (Count < 4U) {
                Image->ExifInfo.MakerNoteOff = (UINT32)Temp64 - 4U;
            } else {
                Image->ExifInfo.MakerNoteOff = Value + ExifDmx->TiffBase;
                Rval = SvcFormat_SeekMem(MemOp, (INT64)Image->ExifInfo.MakerNoteOff + (INT64)Count - 2, SVC_STREAM_SEEK_START);
                if (Rval == FORMAT_OK) {
                    Rval = ExifDmx_ParseAmbaBox(Image, MemOp, Count);
                    if (Rval == FORMAT_OK) {
                        Rval = SvcFormat_SeekMem(MemOp, (INT64)Temp64, SVC_STREAM_SEEK_START);
                    }
                }
            }
        }
        break;
    case SVC_EXIFIFD_FlashpixVersion:
        break;
    case SVC_EXIFIFD_ColorSpace:
        Image->ExifInfo.ExifIfdInfo.ColorSpace = (UINT16)Value;
        break;
    case SVC_EXIFIFD_SensingMethod: // SensingMethod
        Image->ExifInfo.ExifIfdInfo.SensingMethod = (UINT16)Value;
        break;
    case SVC_EXIFIFD_FileSource:
        Image->ExifInfo.ExifIfdInfo.FileSource = (UINT16)Value;
        break;
    case SVC_EXIFIFD_SceneType:
        Image->ExifInfo.ExifIfdInfo.SceneType = (UINT16)Value;
        break;
    case SVC_EXIFIFD_ExposureMode:
        Image->ExifInfo.ExifIfdInfo.ExposureMode = (UINT16)Value;
        break;
    case SVC_EXIFIFD_DeviceSettingDescription:
        break;
    default:
        // do nothing
        break;
    }
    return Rval;
}

/**
 * Parse the EXIF IFD entry of exif
 *
 * @param [in] Image The image
 * @param [in] ExifDmx The private information of exif
 * @param [in] MemOp The handler of memory operation
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
static UINT32 ExifDmx_ParseExifIfd(SVC_IMAGE_INFO_s *Image, const SVC_EXIF_INFO_s *ExifDmx, SVC_MEM_OP_s *MemOp)
{
    UINT32 Rval = SvcFormat_SeekMem(MemOp, (INT64)ExifDmx->ExifIfdOff, SVC_STREAM_SEEK_START);
    if (Rval == FORMAT_OK) {
        UINT16 ExifIfdEntry;
        Rval = SvcFormat_GetMem16(MemOp, &ExifIfdEntry);
        if (Rval == FORMAT_OK) {
            UINT16 i, Tag, Type;
            UINT32 Count, Param;
            for (i = 0; i < ExifIfdEntry; i++) {
                // TODO: Parse other information
                Rval = SvcFormat_GetIfdEntry(MemOp, &Tag, &Type, &Count, &Param);
                if (Rval == FORMAT_OK) {
                    if ((Type == TIFF_RATIONAL) || (Type == TIFF_SRATIONAL)) {
                        Rval = ExifDmx_GetExifIfd_8byte(MemOp, Tag, Param + ExifDmx->TiffBase, &Image->ExifInfo.ExifIfdInfo);
                        if (Rval != FORMAT_OK) {
                            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s ExifDmx_GetExifIfd_8byte() error!", __func__, NULL, NULL, NULL, NULL);
                        }
                    } else {
                        Rval = ExifDmx_ParseExifIfd_Tag(MemOp, Tag, Count, Param, ExifDmx, Image);
                        if (Rval != FORMAT_OK) {
                            AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "ExifDmx_ParseExifIfd Get(%x) error!", (UINT32)Tag, 0U, 0U, 0U, 0U);
                        }
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetIfdEntry() error!", __func__, NULL, NULL, NULL, NULL);
                }
                if (Rval != FORMAT_OK) {
                    break;
                }
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetMem16() error!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_SeekMem() error!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 * Get the GPS IFD entry of exif (ASCII/BYTE/UNDEFINED)
 *
 * @param [in] MemOp The handler of memory operation
 * @param [in] Offset The offset of the tag
 * @param [in] Entry The buufer of the gps entry
 * @param [in] Size The size of the gps entry
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
static UINT32 ExifDmx_GetIfdEntry_1byte(SVC_MEM_OP_s *MemOp, UINT32 Offset, UINT8 *Entry, UINT32 Size)
{
    UINT32 Rval = SvcFormat_SeekMem(MemOp, (INT64)Offset, SVC_STREAM_SEEK_START);
    if (Rval == FORMAT_OK) {
        UINT32 Count;
        Rval = SvcFormat_ReadMem(MemOp, Size, Entry, &Count);
        if (Rval == FORMAT_OK) {
            if (Count != Size) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Not enough bytes to read!!", __func__, NULL, NULL, NULL, NULL);
                Rval = FORMAT_ERR_NOT_ENOUGH_BYTES;
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_ReadMem() fail!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_SeekMem() fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 * Get the GPS IFD entry of exif (RATIONAL/SRATIONAL)
 *
 * @param [in] MemOp The handler of memory operation
 * @param [in] Offset The offset of the tag
 * @param [in] Numerator Array of numerators
 * @param [in] Denominator Array of denominators
 * @param [in] Count Number of rationals
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
static UINT32 ExifDmx_GetIfdEntry_8byte(SVC_MEM_OP_s *MemOp, UINT32 Offset, UINT32 *Numerator, UINT32 *Denominator, UINT32 Count)
{
    UINT32 Rval = SvcFormat_SeekMem(MemOp, (INT64)Offset, SVC_STREAM_SEEK_START);
    if (Rval == FORMAT_OK) {
        UINT32 i;
        for (i = 0U; i < Count; i++) {
            Rval = SvcFormat_GetMem32(MemOp, &Numerator[i]);
            if (Rval == FORMAT_OK) {
                Rval = SvcFormat_GetMem32(MemOp, &Denominator[i]);
            }
            if (Rval != FORMAT_OK) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetMem32() fail!", __func__, NULL, NULL, NULL, NULL);
                break;
            }
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_SeekMem() fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 * Get the GPS IFD of exif (ASCII/BYTE/UNDEFINED)
 *
 * @param [in] MemOp The handler of memory operation
 * @param [in] Tag The id of the tag
 * @param [in] Offset The offset of the tag
 * @param [in] GpsInfo The gps information
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
static UINT32 ExifDmx_GetGpsIfd_1byte(SVC_MEM_OP_s *MemOp, UINT16 Tag, UINT32 Offset, SVC_EXIF_GPS_INFO_s *GpsInfo)
{
    UINT32 Rval = FORMAT_OK;
    UINT64 Temp64 = SvcFormat_GetMemPos(MemOp);
    switch (Tag) {
    case SVC_GPSIFD_GPSDateStamp:
        Rval = ExifDmx_GetIfdEntry_1byte(MemOp, Offset, &GpsInfo->Datestamp[0], (UINT32)sizeof(GpsInfo->Datestamp));
        break;
    default:
        // do nothing
        break;
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_SeekMem(MemOp, (INT64)Temp64, SVC_STREAM_SEEK_START);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_SeekMem() error!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "ExifDmx_GetGpsIfd_1byte Get(%x) error!", (UINT32)Tag, 0U, 0U, 0U, 0U);
    }
    return Rval;
}

/**
 * Get the GPS IFD of exif (RATIONAL/SRATIONAL)
 *
 * @param [in] MemOp The handler of memory operation
 * @param [in] Tag The id of the tag
 * @param [in] Count The size of the tag
 * @param [in] Offset The offset of the tag
 * @param [in] GpsInfo The gps information
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
static UINT32 ExifDmx_GetGpsIfd_8byte(SVC_MEM_OP_s *MemOp, UINT16 Tag, UINT32 Count, UINT32 Offset, SVC_EXIF_GPS_INFO_s *GpsInfo)
{
    UINT32 Rval = FORMAT_OK;
    UINT64 Temp64 = SvcFormat_GetMemPos(MemOp);
    switch (Tag) {
    case SVC_GPSIFD_GPSLatitude:
        Rval = ExifDmx_GetIfdEntry_8byte(MemOp, Offset, &GpsInfo->LatitudeNum[0], &GpsInfo->LatitudeDen[0], Count);
        break;
    case SVC_GPSIFD_GPSLongitude:
        Rval = ExifDmx_GetIfdEntry_8byte(MemOp, Offset, &GpsInfo->LongitudeNum[0], &GpsInfo->LongitudeDen[0], Count);
        break;
    case SVC_GPSIFD_GPSAltitude:
        Rval = ExifDmx_GetIfdEntry_8byte(MemOp, Offset, &GpsInfo->AltitudeNum, &GpsInfo->AltitudeDen, Count);
        break;
    case SVC_GPSIFD_GPSTimeStamp:
        Rval = ExifDmx_GetIfdEntry_8byte(MemOp, Offset, &GpsInfo->TimestampNum[0], &GpsInfo->TimestampDen[0], Count);
        break;
    case SVC_GPSIFD_GPSDOP:
        Rval = ExifDmx_GetIfdEntry_8byte(MemOp, Offset, &GpsInfo->DopNum, &GpsInfo->DopDen, Count);
        break;
    case SVC_GPSIFD_GPSSpeed:
        Rval = ExifDmx_GetIfdEntry_8byte(MemOp, Offset, &GpsInfo->SpeedNum, &GpsInfo->SpeedDen, Count);
        break;
    case SVC_GPSIFD_GPSTrack:
        Rval = ExifDmx_GetIfdEntry_8byte(MemOp, Offset, &GpsInfo->TrackNum, &GpsInfo->TrackDen, Count);
        break;
    case SVC_GPSIFD_GPSImgDirection:
        Rval = ExifDmx_GetIfdEntry_8byte(MemOp, Offset, &GpsInfo->ImgdirectionNum, &GpsInfo->ImgdirectionDen, Count);
        break;
    case SVC_GPSIFD_GPSDestLatitude:
        Rval = ExifDmx_GetIfdEntry_8byte(MemOp, Offset, &GpsInfo->DestlatitudeNum[0], &GpsInfo->DestlatitudeNum[0], Count);
        break;
    case SVC_GPSIFD_GPSDestLongitude:
        Rval = ExifDmx_GetIfdEntry_8byte(MemOp, Offset, &GpsInfo->DestlongitudeNum[0], &GpsInfo->DestlongitudeDen[0], Count);
        break;
    case SVC_GPSIFD_GPSDestBearing:
        Rval = ExifDmx_GetIfdEntry_8byte(MemOp, Offset, &GpsInfo->DestbearingNum, &GpsInfo->DestbearingDen, Count);
        break;
    case SVC_GPSIFD_GPSDestDistance:
        Rval = ExifDmx_GetIfdEntry_8byte(MemOp, Offset, &GpsInfo->DestdistanceNum, &GpsInfo->DestdistanceDen, Count);
        break;
    case SVC_GPSIFD_GPSDateStamp:
        Rval = ExifDmx_GetIfdEntry_1byte(MemOp, Offset, &GpsInfo->Datestamp[0], (UINT32)sizeof(GpsInfo->Datestamp));
        break;
    default:
        // do nothing
        break;
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_SeekMem(MemOp, (INT64)Temp64, SVC_STREAM_SEEK_START);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_SeekMem() error!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "ExifDmx_GetGpsIfd_8byte Get(%x) error!", (UINT32)Tag, 0U, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 ExifDmx_ParseGpsIfd_Offset(const SVC_MEM_OP_s *MemOp, UINT16 Tag, UINT32 Count, UINT32 Value, const SVC_EXIF_INFO_s *ExifDmx, SVC_EXIF_GPS_INFO_s *GpsInfo)
{
    UINT32 Rval = FORMAT_OK;
    UINT64 Temp64;
    switch (Tag) {
    case SVC_GPSIFD_GPSSatellites:
        Temp64 = SvcFormat_GetMemPos(MemOp);
        GpsInfo->SatelliteCount = Count;
        if (Count <= 4U) {
            GpsInfo->SatelliteOffset = (ULONG)Temp64 - 4UL;
        } else {
            GpsInfo->SatelliteOffset = (ULONG)Value + (ULONG)ExifDmx->TiffBase;
        }
        break;
    case SVC_GPSIFD_GPSMapDatum:
        Temp64 = SvcFormat_GetMemPos(MemOp);
        GpsInfo->MapdatumCount = Count;
        if (Count <= 4U) {
            GpsInfo->MapdatumOffset = (ULONG)Temp64 - 4UL;
        } else {
            GpsInfo->MapdatumOffset = (ULONG)Value + (ULONG)ExifDmx->TiffBase;
        }
        break;
    case SVC_GPSIFD_GPSProcessingMethod:
        Temp64 = SvcFormat_GetMemPos(MemOp);
        GpsInfo->ProcessingmethodCount = Count;
        if (Count <= 4U) {
            GpsInfo->ProcessingmethodOffset = (ULONG)Temp64 - 4UL;
        } else {
            GpsInfo->ProcessingmethodOffset = (ULONG)Value + (ULONG)ExifDmx->TiffBase;
        }
        break;
    default:
    //case SVC_GPSIFD_GPSAreaInformation:
        Temp64 = SvcFormat_GetMemPos(MemOp);
        GpsInfo->AreainformationCount = Count;
        if (Count <= 4U) {
            GpsInfo->AreainformationOffset = (ULONG)Temp64 - 4UL;
        } else {
            GpsInfo->AreainformationOffset = (ULONG)Value + (ULONG)ExifDmx->TiffBase;
        }
        break;
    }
    return Rval;
}

static UINT32 ExifDmx_ParseGpsIfd_Tag(SVC_MEM_OP_s *MemOp, UINT16 Tag, UINT32 Count, UINT32 Value, const SVC_EXIF_INFO_s *ExifDmx, SVC_EXIF_GPS_INFO_s *GpsInfo)
{
    UINT32 Rval = FORMAT_OK;
    UINT32 Tmp32 = SvcFormat_Le2Be32(Value);
    switch (Tag) {
    case SVC_GPSIFD_GPSVersionID:
        Rval = W2F(AmbaWrap_memcpy(&GpsInfo->VersionId[0], &Tmp32, 4U));
        break;
    case SVC_GPSIFD_GPSLatitudeRef:
        Rval = W2F(AmbaWrap_memcpy(&GpsInfo->LatitudeRef[0], &Tmp32, 2U));
        break;
    case SVC_GPSIFD_GPSLongitudeRef:
        Rval = W2F(AmbaWrap_memcpy(&GpsInfo->LongitudeRef[0], &Tmp32, 2U));
        break;
    case SVC_GPSIFD_GPSStatus:
        Rval = W2F(AmbaWrap_memcpy(&GpsInfo->Status[0], &Tmp32, 2U));
        break;
    case SVC_GPSIFD_GPSMeasureMode:
        Rval = W2F(AmbaWrap_memcpy(&GpsInfo->MeasureMode[0], &Tmp32, 2U));
        break;
    case SVC_GPSIFD_GPSSpeedRef:
        Rval = W2F(AmbaWrap_memcpy(&GpsInfo->SpeedRef[0], &Tmp32, 2U));
        break;
    case SVC_GPSIFD_GPSTrackRef:
        Rval = W2F(AmbaWrap_memcpy(&GpsInfo->TrackRef[0], &Tmp32, 2U));
        break;
    case SVC_GPSIFD_GPSImgDirectionRef:
        Rval = W2F(AmbaWrap_memcpy(&GpsInfo->ImgdirectionRef[0], &Tmp32, 2U));
        break;
    case SVC_GPSIFD_GPSDestLatitudeRef:
        Rval = W2F(AmbaWrap_memcpy(&GpsInfo->DestlatitudeRef[0], &Tmp32, 2U));
        break;
    case SVC_GPSIFD_GPSDestLongitudeRef:
        Rval = W2F(AmbaWrap_memcpy(&GpsInfo->DestlongitudeRef[0], &Tmp32, 2U));
        break;
    case SVC_GPSIFD_GPSDestBearingRef:
        Rval = W2F(AmbaWrap_memcpy(&GpsInfo->DestbearingRef[0], &Tmp32, 2U));
        break;
    case SVC_GPSIFD_GPSDestDistanceRef:
        Rval = W2F(AmbaWrap_memcpy(&GpsInfo->DestdistanceRef[0], &Tmp32, 2U));
        break;
    case SVC_GPSIFD_GPSAltitudeRef:
        GpsInfo->AltitudeRef = (UINT8)(Value & 0xFFU);
        break;
    case SVC_GPSIFD_GPSSatellites:
    case SVC_GPSIFD_GPSMapDatum:
    case SVC_GPSIFD_GPSProcessingMethod:
    case SVC_GPSIFD_GPSAreaInformation:
        Rval = ExifDmx_ParseGpsIfd_Offset(MemOp, Tag, Count, Value, ExifDmx, GpsInfo);
        break;
    case SVC_GPSIFD_GPSDateStamp:
        Rval = ExifDmx_GetGpsIfd_1byte(MemOp, Tag, Value + ExifDmx->TiffBase, GpsInfo);
        break;
    case SVC_GPSIFD_GPSDifferential:
        GpsInfo->Differential = (UINT16)(Value & 0xFFFFU);
        break;
    default:
        // do nothing
        break;
    }
    return Rval;
}

/**
 * Parse the GPS IFD entry of exif
 *
 * @param [in] Image The image
 * @param [in] ExifDmx The private information of exif
 * @param [in] MemOp The handler of memory operation
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
static UINT32 ExifDmx_ParseGpsIfd(SVC_IMAGE_INFO_s *Image, const SVC_EXIF_INFO_s *ExifDmx, SVC_MEM_OP_s *MemOp)
{
    UINT32 Rval = SvcFormat_SeekMem(MemOp, (INT64)ExifDmx->GpsIfdOff, SVC_STREAM_SEEK_START);
    if (Rval == FORMAT_OK) {
        UINT16 GpsIfdEntry;
        Rval = SvcFormat_GetMem16(MemOp, &GpsIfdEntry);
        if (Rval == FORMAT_OK) {
            SVC_EXIF_GPS_INFO_s * const GpsInfo = &Image->ExifInfo.GpsIfdInfo;
            UINT16 i, Tag, Type;
            UINT32 Count, Param;
            for (i = 0; i < GpsIfdEntry; i++) {
                // TODO: Parse other information
                Rval = SvcFormat_GetIfdEntry(MemOp, &Tag, &Type, &Count, &Param);
                if (Rval == FORMAT_OK) {
                    if ((Type == TIFF_RATIONAL) || (Type == TIFF_SRATIONAL)) {
                        Rval = ExifDmx_GetGpsIfd_8byte(MemOp, Tag, Count, Param + ExifDmx->TiffBase, GpsInfo);
                        if (Rval != FORMAT_OK) {
                            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s ExifDmx_GetGpsIfd_8byte() error!", __func__, NULL, NULL, NULL, NULL);
                        }
                    }else {
                        Rval = ExifDmx_ParseGpsIfd_Tag(MemOp, Tag, Count, Param, ExifDmx, GpsInfo);
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetIfdEntry() error!", __func__, NULL, NULL, NULL, NULL);
                }
                if (Rval != FORMAT_OK) {
                    break;
                }
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetMem16() error!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_SeekMem() error!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 * Parse the IFD1 entry of exif
 *
 * @param [in] Image The image
 * @param [in] ExifDmx The private information of exif
 * @param [in] MemOp The handler of memory operation
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
static UINT32 ExifDmx_ParseIfd1(SVC_IMAGE_INFO_s *Image, const SVC_EXIF_INFO_s *ExifDmx, SVC_MEM_OP_s *MemOp)
{
    UINT32 Rval = SvcFormat_SeekMem(MemOp, (INT64)ExifDmx->Ifd1Off, SVC_STREAM_SEEK_START);
    if (Rval == FORMAT_OK) {
        UINT16 Ifd1Entry;
        Rval = SvcFormat_GetMem16(MemOp, &Ifd1Entry);
        if (Rval == FORMAT_OK) {
            UINT16 i, Tag, Type;
            UINT32 Count, Param;
            for (i = 0; i < Ifd1Entry; i++) {
                // TODO: Parse other information
                Rval = SvcFormat_GetIfdEntry(MemOp, &Tag, &Type, &Count, &Param);
                if (Rval == FORMAT_OK) {
                    switch (Tag) {
                    case SVC_TIFF_JPEGInterchangeFormat:
                        Image->Frame[1].Pos = Param + ExifDmx->TiffBase + 2U; /* After SOI */
                        break;
                    case SVC_TIFF_JPEGInterchangeFormatLength:
                        Image->Frame[1].Size = Param;
                        break;
                    case SVC_TIFF_Orientation:
                        break;
                    default:
                        // do nothing
                        break;
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetIfdEntry() error!", __func__, NULL, NULL, NULL, NULL);
                    break;
                }
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetMem16() error!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_SeekMem() error!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 * Parse APP1 of exif
 *
 * @param [in] Image The image
 * @param [in] Stream The stream handler of the media
 * @param [in] ExifDmx The private information of exif
 * @param [in] MemOp The handler of memory operation
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
static UINT32 ExifDmx_ParseApp1(SVC_IMAGE_INFO_s *Image, SVC_STREAM_HDLR_s *Stream, SVC_EXIF_INFO_s *ExifDmx, SVC_MEM_OP_s *MemOp)
{
    UINT32 Rval, Temp32;
    Rval = SvcFormat_GetMemBe32(MemOp, &Temp32);
    if (Rval == FORMAT_OK) {
        if (Temp32 == Marker_Exif) {
            UINT8 Temp8;
            Rval = SvcFormat_GetMemByte(MemOp, &Temp8);
            if (Rval == FORMAT_OK) {
                Rval = SvcFormat_GetMemByte(MemOp, &Temp8);
                if (Rval == FORMAT_OK) {
                    /* get TIFF header */
                    ExifDmx->TiffBase = (UINT32)SvcFormat_GetMemPos(MemOp);
                    Rval = SvcFormat_GetTiffHeader(MemOp, &Image->Endian);
                    if (Rval == FORMAT_OK) {
                        /* get IFD0 Offset*/
                        Rval = SvcFormat_GetMem32(MemOp, &ExifDmx->Ifd0Off);
                        if (Rval == FORMAT_OK) {
                            if (ExifDmx->Ifd0Off != 0U){
                                ExifDmx->Ifd0Off += ExifDmx->TiffBase;
                                /* get IFD0 info and EXIFIFD/GPSIFD offset */
                                Rval = ExifDmx_ParseIfd0(ExifDmx, MemOp);
                                if (Rval == FORMAT_OK) {
                                    /* get IFD1 Offset */
                                    Rval = SvcFormat_GetMem32(MemOp, &ExifDmx->Ifd1Off);
                                    if (Rval != FORMAT_OK) {
                                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetMem32() error!", __func__, NULL, NULL, NULL, NULL);
                                    }
                                } else {
                                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s ExifDmx_ParseIfd0() error!", __func__, NULL, NULL, NULL, NULL);
                                }
                            }
                            if (Rval == FORMAT_OK) {
                                if (ExifDmx->ExifIfdOff != 0U){
                                    ExifDmx->ExifIfdOff += ExifDmx->TiffBase;
                                    Rval = ExifDmx_ParseExifIfd(Image, ExifDmx, MemOp);
                                    if (Rval != FORMAT_OK) {
                                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s ExifDmx_ParseExifIfd() error!", __func__, NULL, NULL, NULL, NULL);
                                    }
                                }
                            }
                            if (Rval == FORMAT_OK) {
                                if (ExifDmx->GpsIfdOff != 0U){
                                    ExifDmx->GpsIfdOff += ExifDmx->TiffBase;
                                    Rval = ExifDmx_ParseGpsIfd(Image, ExifDmx, MemOp);
                                    if (Rval != FORMAT_OK) {
                                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s ExifDmx_ParseGpsIfd() error!", __func__, NULL, NULL, NULL, NULL);
                                    }
                                }
                            }
                            if (Rval == FORMAT_OK) {
                                if (ExifDmx->Ifd1Off != 0U){
                                    ExifDmx->Ifd1Off += ExifDmx->TiffBase;
                                    Rval = ExifDmx_ParseIfd1(Image, ExifDmx, MemOp);
                                    if (Rval == FORMAT_OK) {
                                        Rval = ExifDmx_CheckFormat(Image, Stream, MemOp, 1U, (UINT64)Image->Frame[1].Pos - 2ULL /* Add SOI */);
                                        if (Rval == FORMAT_OK) {
                                            ExifDmx->Thumbnail = 1U;
                                            Image->FrameCount++;
                                        } else {
                                            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s ExifDmx_CheckFormat() error!", __func__, NULL, NULL, NULL, NULL);
                                        }
                                    } else {
                                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s ExifDmx_ParseIfd1() error!", __func__, NULL, NULL, NULL, NULL);
                                    }
                                }
                            }
                        } else {
                            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetMem32() error!", __func__, NULL, NULL, NULL, NULL);
                        }
                    } else {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetTiffHeader() error!", __func__, NULL, NULL, NULL, NULL);
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetMemByte() error!", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetMemByte() error!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Marker error! (expected Marker_Exif)", __func__, NULL, NULL, NULL, NULL);
            Rval = FORMAT_ERR_INVALID_FORMAT;
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetMemBe32() error!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 * Parse APP2 of exif
 *
 * @param [in] Image The image
 * @param [in] Stream The stream handler of the media
 * @param [in] ExifDmx The private information of exif
 * @param [in] MemOp The handler of memory operation
 * @param [in] Length The length of App2
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
static UINT32 ExifDmx_ParseApp2(SVC_IMAGE_INFO_s *Image, SVC_STREAM_HDLR_s *Stream, SVC_EXIF_INFO_s *ExifDmx, SVC_MEM_OP_s *MemOp, UINT32 Length)
{
    UINT32 Rval;
    UINT16 Start, Next;
    // TODO: Add other screennail type
    if (ExifDmx->Screennail == 0U) {
        Rval = SvcFormat_GetMemBe16(MemOp, &Start);
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetMemBe16(MemOp, &Next);
            if (Rval == FORMAT_OK) {
                if (Start == Marker_SOI) {
                    // Found screennail
                    Rval = ExifDmx_CheckFormat(Image, Stream, MemOp, 2U, (UINT64)ExifDmx->App2Off);
                    if (Rval == FORMAT_OK) {
                        ExifDmx->Screennail = 1U;
                        ExifDmx->SecPicType = SVC_FORMAT_SCRAPP2;
                        Image->Frame[2].Pos = ExifDmx->App2Off; // in front of SOI
                        Image->Frame[2].Size = Length - 2U;
                        Image->FrameCount++;
                    } else {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s ExifDmx_CheckFormat() error!", __func__, NULL, NULL, NULL, NULL);
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Wrong Marker!", __func__, NULL, NULL, NULL, NULL);
                    Rval = FORMAT_ERR_GENERAL_ERROR;
                }
            } else {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetMemBe16() error!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetMemBe16() error!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        Rval = SvcFormat_GetMemBe16(MemOp, &Start);
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetMemBe16(MemOp, &Next);
            if (Rval == FORMAT_OK) {
                Image->Frame[2].Size += (Length - 2U);
            } else {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetMemBe16() error!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetMemBe16() error!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Rval;
}

static UINT32 ExifDmx_ParseTag(SVC_IMAGE_INFO_s *Image, SVC_STREAM_HDLR_s *Stream, SVC_EXIF_INFO_s *ExifDmx, SVC_MEM_OP_s *MemOp, UINT16 Tag, UINT16 Len)
{
    UINT32 Rval = FORMAT_OK;
    switch (Tag) {
    case 0xFFE1U:// App1, EXIF
        ExifDmx->App1Off = (UINT32)SvcFormat_GetMemPos(MemOp);
        Rval = ExifDmx_ParseApp1(Image, Stream, ExifDmx, MemOp);
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_SeekMem(MemOp, (INT64)ExifDmx->App1Off + (INT64)Len - 2LL, SVC_STREAM_SEEK_START);
            if (Rval != FORMAT_OK) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_SeekMem() error!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s ExifDmx_ParseApp1() error!", __func__, NULL, NULL, NULL, NULL);
        }
        break;
    case 0xFFE2U:// App2, Screennail
        ExifDmx->App2Off = (UINT32)SvcFormat_GetMemPos(MemOp);
        Rval = ExifDmx_ParseApp2(Image, Stream, ExifDmx, MemOp, (UINT32)Len);
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_SeekMem(MemOp, (INT64)ExifDmx->App2Off + (INT64)Len - 2LL, SVC_STREAM_SEEK_START);
            if (Rval != FORMAT_OK) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_SeekMem() error!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s ExifDmx_ParseApp2() error!", __func__, NULL, NULL, NULL, NULL);
        }
        break;
    default:
        if ((Tag & 0xFFF0U) == 0xFFE0U) {// AppX (except APP1 and APP2)
            Rval = SvcFormat_SeekMem(MemOp, (INT64)Len - 2LL, SVC_STREAM_SEEK_CUR);
        } else {
            if (Image->Frame[0].Pos == 0U){
                UINT64 TempOff = SvcFormat_GetMemPos(MemOp) - 4ULL;
                Rval = ExifDmx_CheckFormat(Image, Stream, MemOp, 0U, TempOff);
                if (Rval == FORMAT_OK) {
                    const SVC_MEDIA_INFO_s * Media = &Image->MediaInfo;
                    ExifDmx->Fullview = 1U;
                    Image->Frame[0].Pos = (UINT32)TempOff; /* NO SOI*/
                    Image->Frame[0].Size = (UINT32)Media->Size - (UINT32)TempOff + 2U/*SOI*/;
                    Image->FrameCount++;
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s ExifDmx_CheckFormat() error!", __func__, NULL, NULL, NULL, NULL);
                }
            }
        }
        break;
    }
    return Rval;
}

/**
 * Implementation of parse a media (AMP_DMX_FORMAT_PARSE_FP)
 *
 * @param [in,out] Image The image
 * @param [in] Stream The stream of the media
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
static UINT32 ExifDmx_ParseImpl(SVC_IMAGE_INFO_s *Image, SVC_STREAM_HDLR_s *Stream)
{
    UINT32 Rval;
    SVC_MEDIA_INFO_s * const Media = &Image->MediaInfo;
    UINT64 FileLength;
    Media->Valid = 0U;
    Rval = S2F(Stream->Func->GetLength(Stream, &FileLength));
    if (Rval == FORMAT_OK) {
        SVC_MEM_OP_s MemOp;
        UINT8 ParseBuffer[SVC_EXIFDMX_HEAD_SIZE];
        UINT16 Temp16;
        Media->Size = FileLength;
        MemOp.Base = NULL;
        MemOp.Size = (UINT32)FileLength;
        MemOp.Offset = 0U;
        MemOp.Buffer = ParseBuffer;
        MemOp.BufferBase = 0xFFFFFFFFULL; // make sure to read first block
        MemOp.BufferSize = SVC_EXIFDMX_HEAD_SIZE;
        MemOp.Stream = Stream;
        Rval = SvcFormat_GetMemBe16(&MemOp, &Temp16);
        if (Rval == FORMAT_OK) {
            if (Temp16 == 0xFFD8U) { // SOI
                SVC_EXIF_INFO_s * const ExifDmx = &Image->ExifInfo;
                UINT8 Eoh = 0;
                while ((Eoh == 0U) && (Image->Frame[0].Pos == 0U)) {
                    UINT16 Tag, Len = 0U;
                    Rval = SvcFormat_GetMemBe16(&MemOp, &Tag);
                    if (Rval == FORMAT_OK) {
                        Rval = SvcFormat_GetMemBe16(&MemOp, &Len);
                        if (Rval == FORMAT_OK) {
                            if ((Len >= Media->Size) || (Len == 0U)) {
                                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Wrong tag length!", __func__, NULL, NULL, NULL, NULL);
                                Rval = FORMAT_ERR_GENERAL_ERROR;
                            }
                        } else {
                            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetMemBe16() error!", __func__, NULL, NULL, NULL, NULL);
                        }
                    } else {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetMemBe16() error!", __func__, NULL, NULL, NULL, NULL);
                    }
                    if (Rval == FORMAT_OK) {
                        Rval = ExifDmx_ParseTag(Image, Stream, ExifDmx, &MemOp, Tag, Len);
                    }
                    if (Rval == FORMAT_OK) {
                        UINT64 Pos;
                        Rval = S2F(Stream->Func->GetPos(Stream, &Pos));
                        if (Rval == FORMAT_OK) {
                            if (Pos == Media->Size) {
                                Eoh = 1U; // leave while loop
                            }
                        } else {
                            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Stream->Func->GetPos() error!", __func__, NULL, NULL, NULL, NULL);
                        }
                    }
                    if (Rval != FORMAT_OK) {
                        break;
                    }
                }
#if 0
                AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "Ifd0Off = %u", ExifDmx->Ifd0Off, 0U, 0U, 0U, 0U);
                AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "ExifIfdOff = %u", ExifDmx->ExifIfdOff, 0U, 0U, 0U, 0U);
                AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "Ifd1Off = %u", ExifDmx->Ifd1Off, 0U, 0U, 0U, 0U);
#endif
            } else {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Marker error! (expected 0xFFD8U)", __func__, NULL, NULL, NULL, NULL);
                Rval = FORMAT_ERR_INVALID_FORMAT;
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetMemBe16() error!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    if (Rval == FORMAT_OK) {
        Media->Valid = 1U;
    }
    return Rval;
}

static inline UINT32 SvcExifDmx_Parse_ParamCheck(const SVC_MEDIA_INFO_s *Media, const SVC_STREAM_HDLR_s *Stream)
{
    UINT32 Rval = DEMUXER_ERR_INVALID_ARG;
    if ((Media != NULL) && (Stream != NULL)) {
        Rval = DEMUXER_OK;
    }
    return Rval;
}


/**
 * Parse a media
 *
 * @param [in,out] Media The media
 * @param [in] Stream The stream of the media
 * @return 0 - OK, others - DEMUXER_ERR_XXXX
 */
UINT32 SvcExifDmx_Parse(SVC_MEDIA_INFO_s *Media, SVC_STREAM_HDLR_s *Stream)
{
    UINT32 Rval = SvcExifDmx_Parse_ParamCheck(Media, Stream);
    if (Rval == DEMUXER_OK) {
        SVC_IMAGE_INFO_s *Image;
        AmbaMisra_TypeCast(&Image, &Media);
        AmbaMisra_TouchUnused(Media);
        /* When return DEMUXER_ERR_IO_ERROR and Media->Valid = 0U, it means invalid format. */
        Rval = F2D(ExifDmx_ParseImpl(Image, Stream));
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Invalid argument!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static inline UINT32 SvcExifDmx_Open_ParamCheck(const SVC_DMX_FORMAT_HDLR_s *Hdlr)
{
    UINT32 Rval = DEMUXER_ERR_INVALID_ARG;
    if (Hdlr != NULL) {
        Rval = DEMUXER_OK;
    }
    return Rval;
}

/**
 * Open Exif demux handler
 *
 * @param [in] Hdlr The handler of exif demux to open.
 * @return 0 - OK, others - DEMUXER_ERR_XXXX
 */
static UINT32 SvcExifDmx_Open(SVC_DMX_FORMAT_HDLR_s *Hdlr)
{
    UINT32 Rval = SvcExifDmx_Open_ParamCheck(Hdlr);
    if (Rval == DEMUXER_OK) {
        SVC_EXIF_DMX_HDLR_s *ExifDmx;
        AmbaMisra_TypeCast(&ExifDmx, &Hdlr);
        if (Hdlr->Media->MediaType == SVC_MEDIA_INFO_IMAGE) {
            Rval = K2D(AmbaKAL_MutexTake(&ExifDmx->Mutex, SVC_FORMAT_WAIT_TIMEOUT));
            if (Rval == DEMUXER_OK) {
                if (ExifDmx->Open == 0U) {
                    ExifDmx->Open = 1U;
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Already opened!", __func__, NULL, NULL, NULL, NULL);
                }
                if (AmbaKAL_MutexGive(&ExifDmx->Mutex) != OK) {
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
    AmbaMisra_TouchUnused(Hdlr);
    return Rval;
}

static inline UINT32 SvcExifDmx_Close_ParamCheck(const SVC_DMX_FORMAT_HDLR_s *Hdlr)
{
    UINT32 Rval = DEMUXER_ERR_INVALID_ARG;
    if (Hdlr != NULL) {
        Rval = DEMUXER_OK;
    }
    return Rval;
}

/**
 * Close Exif demux handler
 *
 * @param [in] Hdlr The handler of exif demux to close.
 * @return 0 - OK, others - DEMUXER_ERR_XXXX
 */
static UINT32 SvcExifDmx_Close(SVC_DMX_FORMAT_HDLR_s *Hdlr)
{
    UINT32 Rval = SvcExifDmx_Close_ParamCheck(Hdlr);
    if (Rval == DEMUXER_OK) {
        SVC_EXIF_DMX_HDLR_s *ExifDmx;
        AmbaMisra_TypeCast(&ExifDmx, &Hdlr);
        Rval = K2D(AmbaKAL_MutexTake(&ExifDmx->Mutex, SVC_FORMAT_WAIT_TIMEOUT));
        if (Rval == DEMUXER_OK) {
            if (ExifDmx->Open != 0U) {
                ExifDmx->Open = 0U;
            } else {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s No open yet!", __func__, NULL, NULL, NULL, NULL);
            }
            if (AmbaKAL_MutexGive(&ExifDmx->Mutex) != OK) {
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

static UINT32 GetOffset(const UINT8 *Base, const UINT8 *Addr)
{
    ULONG AddrUL, BaseUL, OffsetUL;
    AmbaMisra_TypeCast(&AddrUL, &Addr);
    AmbaMisra_TypeCast(&BaseUL, &Base);
    OffsetUL = AddrUL - BaseUL;
    return (UINT32)OffsetUL;
}

/**
 * Feed screen-nail frame
 *
 * @param [in] Image The media information of the image
 * @param [in] Stream The stream handler
 * @param [in] ExifDmx The private information of exif
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
static UINT32 ExifDmx_FeedScreenNail(const SVC_IMAGE_INFO_s *Image, SVC_STREAM_HDLR_s *Stream, const SVC_EXIF_INFO_s *ExifDmx)
{
    SVC_FIFO_BITS_DESC_s Frame = {0};
    UINT32 Rval = SvcFormat_PrepareFrame(Image->Fifo, &Frame, Image->Frame[2].Size, SVC_FIFO_TYPE_JPEG_FRAME, 0ULL, 0U);
    if (Rval == FORMAT_OK){
        if (ExifDmx->SecPicType == SVC_FORMAT_SCRAPP2){
            Rval = S2F(Stream->Func->Seek(Stream, (INT64)Image->Frame[2].Pos - 2LL, SVC_STREAM_SEEK_START));
            if (Rval == FORMAT_OK) {
                UINT16 App2Size;
                Rval = SvcFormat_GetBe16(Stream, &App2Size);
                if (Rval == FORMAT_OK) {
                    UINT32 CurrPos = GetOffset(Image->BufferBase, Frame.StartAddr);
                    Rval = SvcFormat_ReadStream(Stream, (UINT32)App2Size - 2U, CurrPos, Image->BufferBase, Image->BufferSize, &CurrPos);
                    if (Rval == FORMAT_OK) {
                        UINT16 Marker;
                        do {
                            Rval = SvcFormat_GetBe16(Stream, &Marker);
                            if (Rval == FORMAT_OK) {
                                if (Marker == 0xFFE2U) {
                                    Rval = SvcFormat_GetBe16(Stream, &App2Size);
                                    if (Rval == FORMAT_OK) {
                                        Rval = SvcFormat_ReadStream(Stream, (UINT32)App2Size - 2U, CurrPos, Image->BufferBase, Image->BufferSize, &CurrPos);
                                        if (Rval != FORMAT_OK) {
                                            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_ReadStream() error!", __func__, NULL, NULL, NULL, NULL);
                                        }
                                    } else {
                                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe16() error!", __func__, NULL, NULL, NULL, NULL);
                                    }
                                }
                            } else {
                                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe16() error!", __func__, NULL, NULL, NULL, NULL);
                            }
                            if (Rval != FORMAT_OK) {
                                break;
                            }
                        } while(Marker == 0xFFE2U);
                        if (Rval == FORMAT_OK) {
                            Rval = SvcFormat_WriteFrame(Image->Fifo, &Frame, Image->Frame[2].Size, SVC_FIFO_TYPE_JPEG_FRAME, 0ULL, 0U, 1U);
                            if (Rval != FORMAT_OK) {
                                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_WriteFrame() error!", __func__, NULL, NULL, NULL, NULL);
                            }
                        }
                    } else {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_ReadStream() error!", __func__, NULL, NULL, NULL, NULL);
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe16() error!", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Func->Seek() error!", __func__, NULL, NULL, NULL, NULL);
            }
        } else if (ExifDmx->SecPicType == SVC_FORMAT_SCRMPF) {
            // TODO:
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Unsupported SecPicType! (SVC_FORMAT_SCRMPF)", __func__, NULL, NULL, NULL, NULL);
            Rval = FORMAT_ERR_GENERAL_ERROR;
        } else if (ExifDmx->SecPicType == SVC_FORMAT_SCRFPXR) {
            // TODO:
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Unsupported SecPicType! (SVC_FORMAT_SCRFPXR)", __func__, NULL, NULL, NULL, NULL);
            Rval = FORMAT_ERR_GENERAL_ERROR;
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Unsupported SecPicType!", __func__, NULL, NULL, NULL, NULL);
            Rval = FORMAT_ERR_GENERAL_ERROR;
        }
    } else {
        if (Rval != FORMAT_ERR_FIFO_FULL) {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_PrepareFrame() error!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Rval;
}

/**
 * Feed image frame
 *
 * @param [in] Image The media information of the image
 * @param [in] Stream The stream handler
 * @param [in] Pos The file offset of the frame
 * @param [in] Size The size of the frame
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
static UINT32 ExifDmx_FeedImage(const SVC_IMAGE_INFO_s *Image, SVC_STREAM_HDLR_s *Stream, UINT32 Pos, UINT32 Size)
{
    SVC_FIFO_BITS_DESC_s Frame = {0};
    UINT32 Rval = SvcFormat_PrepareFrame(Image->Fifo, &Frame, Size, SVC_FIFO_TYPE_JPEG_FRAME, 0ULL, 0U);
    if (Rval == FORMAT_OK) {
        UINT8 *FifoAddr = Frame.StartAddr;
        FifoAddr[0] = 0xFFU;
        FifoAddr[1] = 0xD8U;
        FifoAddr = &FifoAddr[2]; // FifoAddr += 2;
        Rval = S2F(Stream->Func->Seek(Stream, (INT64)Pos, SVC_STREAM_SEEK_START));
        if (Rval == FORMAT_OK) {
            UINT32 CurrPos = GetOffset(Image->BufferBase, FifoAddr);
            Rval = SvcFormat_ReadStream(Stream, Size - 2U, CurrPos, Image->BufferBase, Image->BufferSize, &CurrPos);
            if (Rval == FORMAT_OK) {
                Rval = SvcFormat_WriteFrame(Image->Fifo, &Frame, Size, SVC_FIFO_TYPE_JPEG_FRAME, 0ULL, 0U, 1U);
                if (Rval != FORMAT_OK){
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_WriteFrame() error!", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_ReadStream() error!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Func->Seek() error!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        if (Rval != FORMAT_ERR_FIFO_FULL) {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_PrepareFrame() error!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Rval;
}

static UINT32 ExifDmx_ProcessImpl(SVC_EXIF_DMX_HDLR_s *ExifDmx, UINT64 TargetFrame)
{
    UINT32 Rval = FORMAT_ERR_GENERAL_ERROR;
    SVC_MEDIA_INFO_s * const Media = ExifDmx->Hdlr.Media;
    AmbaMisra_TouchUnused(ExifDmx);
    if ((Media->Valid == 1U) && (Media->MediaType == SVC_MEDIA_INFO_IMAGE)) {
        SVC_STREAM_HDLR_s * const Stream  = ExifDmx->Hdlr.Stream;
        const SVC_EXIF_INFO_s *ExifInfo;
        const SVC_IMAGE_INFO_s *Image;
        AmbaMisra_TypeCast(&Image, &Media);
        ExifInfo = &Image->ExifInfo;
#if 0
        AmbaPrint_ModulePrintUInt5("ExifDmx_ProcessImpl: Feed the %u-th frame", TargetFrame, 0U, 0U, 0U, 0U);
        AmbaPrint_ModulePrintUInt5("Image->Frame[0]: %u %u %u", Image->Frame[0].Width, Image->Frame[0].Height, 0U, 0U, 0U);
        AmbaPrint_ModulePrintUInt5("Image->Frame[1]: %u %u %u", Image->Frame[1].Width, Image->Frame[1].Height, 0U, 0U, 0U);
        AmbaPrint_ModulePrintUInt5("Image->Frame[2]: %u %u %u", Image->Frame[2].Width, Image->Frame[2].Height, 0U, 0U, 0U);
#endif
        switch (TargetFrame) {
        case 2U:
            Rval = ExifDmx_FeedScreenNail(Image, Stream, ExifInfo);
            break;
        case 1U:
            Rval = ExifDmx_FeedImage(Image, Stream, Image->Frame[1].Pos, Image->Frame[1].Size);
            break;
        default:
            // case 0
            Rval = ExifDmx_FeedImage(Image, Stream, Image->Frame[0].Pos, Image->Frame[0].Size);
            break;
        }
    } else {
        AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "Process: Invalid media %u or media type %u", Media->Valid, Media->MediaType, 0U, 0U, 0U);
        Rval = FORMAT_ERR_GENERAL_ERROR;
    }
    if (Rval == FORMAT_ERR_INVALID_FORMAT) {
        Media->Valid = 0U;
    }
    return Rval;
}

static UINT32 SvcExifDmx_Process_ParamCheck(const SVC_DMX_FORMAT_HDLR_s *Hdlr, UINT64 Param, const UINT8 *Event)
{
    UINT32 Rval = DEMUXER_ERR_INVALID_ARG;
    if ((Hdlr != NULL) && (Param <= 2U) && (Event != NULL)) {
        Rval = DEMUXER_OK;
    }
    return Rval;
}

/**
 * Process exif demux
 *
 * @param [in] Hdlr The handler of mp4 demux to process.
 * @param [in] Param The target frame.
 * @param [out] Event Demuxer event.
 * @return 0 - OK, others - SWMUXER_ERR_XXXX
 */
static UINT32 SvcExifDmx_Process(SVC_DMX_FORMAT_HDLR_s *Hdlr, UINT64 Param, UINT8 *Event)
{
    UINT32 Rval = SvcExifDmx_Process_ParamCheck(Hdlr, Param, Event);
    if (Rval == DEMUXER_OK) {
        SVC_EXIF_DMX_HDLR_s *ExifDmx;
        AmbaMisra_TypeCast(&ExifDmx, &Hdlr);
        *Event = SVC_DEMUXER_EVENT_NONE;
        Rval = K2D(AmbaKAL_MutexTake(&ExifDmx->Mutex, SVC_FORMAT_WAIT_TIMEOUT));
        if (Rval == DEMUXER_OK) {
            Rval = F2D(ExifDmx_ProcessImpl(ExifDmx, Param));
            if (AmbaKAL_MutexGive(&ExifDmx->Mutex) != DEMUXER_OK) {
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

