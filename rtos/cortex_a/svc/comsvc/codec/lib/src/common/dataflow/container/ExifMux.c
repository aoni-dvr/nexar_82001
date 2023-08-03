/**
 * @file ExifMux.c
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
#include "format/SvcExifMux.h"
#include "FormatAPI.h"
#include "Tiff.h"
#include "ByteOp.h"
#include "MemByteOp.h"
#include "Muxer.h"
#include <AmbaDef.h>

//#define DEBUG_MSG
#define SVC_EXIFMUX_HEAD_SIZE    0x800U /**< ExifMux header size */

/**
 * The parameter of exif mux handelr
 */
typedef struct {
    SVC_MUX_FORMAT_HDLR_s Hdlr;         /**< Format handler, must be the first member */
    AMBA_KAL_MUTEX_t Mutex;             /**< The mutex of exif mux handler */
    SVC_TIFF_TAG_BUFFER_s TagBuffer;    /**< The buffer information for exif tags */
    SVC_TIFF_TAG_INFO_s TagInfo;        /**< The overall exif tag status */
    UINT8 *DataBuffer;                  /**< The buffer that keeps tags' data after SvcExifMux_Create */
    UINT8 Endian;                       /**< The endia order of exif format */
    UINT8 Used;                         /**< The flag to indicate a handler is used or not */
    UINT8 Open;                         /**< The flag to indicate a handler is opened or not */
} SVC_EXIF_MUX_HDLR_s;

/**
 * Exif mux manager
 */
typedef struct {
    AMBA_KAL_MUTEX_t Mutex;         /**< The mutex of exif mux manager */
    SVC_EXIF_MUX_HDLR_s *Hdlr;      /**< ExifMux format handlers */
    UINT8 MaxHdlr;                  /**< Number of handlers */
    UINT32 ExifHeadSize;            /**< The default size of exif header data */
} AMBA_EXIF_MUX_MGR_s;


static AMBA_EXIF_MUX_MGR_s g_ExifmuxMgr GNU_SECTION_NOZEROINIT;   /**< The exif mux manager */
static SVC_MUX_FORMAT_s g_ExifMux;              /**< The interface of exif mux format */
static void SvcExifMux_InitOperation(void);

static UINT32 SvcExifMux_GetInitBufSize_ParamCheck(UINT8 MaxHdlr, UINT32 HeaderBufferSize, const UINT32 *BufferSize)
{
    UINT32 Rval = MUXER_ERR_INVALID_ARG;
    if (MaxHdlr > 0U) {
        if ((HeaderBufferSize > 0U) && ((HeaderBufferSize & ((UINT32)AMBA_CACHE_LINE_SIZE - 1U)) == 0U)) {
            if (BufferSize != NULL) {
                Rval = MUXER_OK;
            }
        }
    }
    return Rval;
}

/**
 * Get the required buffer size
 *
 * Get the required buffer size for initializing the ExifMux module.
 * @param [in] MaxHdlr The maximum number of ExifMux handlers
 * @param [in] HeaderBufferSize The size of the Exif header buffer for all handlers
 * @param [out] BufferSize The required buffer size.
 * @return 0 - OK, others - MUXER_ERR_XXXX
 */
UINT32 SvcExifMux_GetInitBufferSize(UINT8 MaxHdlr, UINT32 HeaderBufferSize, UINT32 *BufferSize)
{
    UINT32 Rval = SvcExifMux_GetInitBufSize_ParamCheck(MaxHdlr, HeaderBufferSize, BufferSize);
    if (Rval == MUXER_OK) {
        *BufferSize = GetAlignedValU32(((UINT32)MaxHdlr * (UINT32)sizeof(SVC_EXIF_MUX_HDLR_s)) + HeaderBufferSize, (UINT32)AMBA_CACHE_LINE_SIZE);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Invalid agruement!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 SvcExifMux_GetInitDefaultCfg_ParamCheck(const SVC_EXIF_MUX_INIT_CFG_s *Config)
{
    UINT32 Rval = MUXER_ERR_INVALID_ARG;
    if (Config != NULL) {
        Rval = MUXER_OK;
    }
    return Rval;
}

/**
 * Get the default Exif muxer manager config
 *
 * @param [out] Config: The buffer to get default config.
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcExifMux_GetInitDefaultCfg(SVC_EXIF_MUX_INIT_CFG_s *Config)
{
    UINT32 Rval = SvcExifMux_GetInitDefaultCfg_ParamCheck(Config);
    if (Rval == MUXER_OK) {
        Rval = W2M(AmbaWrap_memset(Config, 0, sizeof(SVC_EXIF_MUX_INIT_CFG_s)));
        if (Rval == MUXER_OK) {
            Config->MaxHdlr = 1U;
            Config->HeaderBufferSize = GetAlignedValU32((UINT32)Config->MaxHdlr * SVC_EXIFMUX_HEAD_SIZE, (UINT32)AMBA_CACHE_LINE_SIZE);
            Rval = SvcExifMux_GetInitBufferSize(Config->MaxHdlr, Config->HeaderBufferSize, &Config->BufferSize);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Invalid agruement!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 SvcExifMux_Init_ParamCheck(const SVC_EXIF_MUX_INIT_CFG_s *Config)
{
    UINT32 Rval = MUXER_OK;
    if ((Config != NULL) && (Config->Buffer != NULL) && (Config->HeaderBuffer != NULL)) {
        ULONG BufferAddrUL, HeaderAddrUL, Mask;
        AmbaMisra_TypeCast(&BufferAddrUL, &Config->Buffer);
        AmbaMisra_TypeCast(&HeaderAddrUL, &Config->HeaderBuffer);
        Mask = AMBA_CACHE_LINE_SIZE - 1UL;
        if (((BufferAddrUL & Mask) == 0UL) && ((HeaderAddrUL & Mask) == 0UL)) {
            UINT32 Size = 0U;
            Rval = SvcExifMux_GetInitBufferSize(Config->MaxHdlr, Config->HeaderBufferSize, &Size);
            if (Rval == MUXER_OK) {
                if (Config->BufferSize != Size) {
                    AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "incorrect buffer Size %u", Config->BufferSize, 0U, 0U, 0U, 0U);
                    Rval = MUXER_ERR_INVALID_ARG;
                }
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Buffer is not aligned!", __func__, NULL, NULL, NULL, NULL);
            Rval = MUXER_ERR_INVALID_ARG;
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Invalid argument!", __func__, NULL, NULL, NULL, NULL);
        Rval = MUXER_ERR_INVALID_ARG;
    }
    return Rval;
}

#define MAX_EXIFMUX_NUM  (16U)  /**< Maximum number of ExifMux handlers */
static UINT32 CreateMutex(void)
{
    static char MuxMgrMutexName[16] = "ExifMux_Mgr";
    UINT32 Rval = K2M(AmbaKAL_MutexCreate(&g_ExifmuxMgr.Mutex, MuxMgrMutexName));
    if (Rval == MUXER_OK) {
        static char MutexName[MAX_EXIFMUX_NUM][16];
        UINT32 i;
        for (i = 0; i < g_ExifmuxMgr.MaxHdlr; i++) {
            SVC_EXIF_MUX_HDLR_s *Hdlr = &g_ExifmuxMgr.Hdlr[i];
            UINT32 Len = AmbaUtility_StringPrintUInt32(MutexName[i], (UINT32)sizeof(MutexName[i]), "ExifMux%02u", 1, &i);
            Rval = K2M(AmbaKAL_MutexCreate(&Hdlr->Mutex, MutexName[i]));
            if ((Rval != MUXER_OK) || (i >= MAX_EXIFMUX_NUM)) {
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
 * Initiate Exif muxer manager
 *
 * @param [in] Config: The initial configure.
 * @return 0 - OK, others - MUXER_ERR_XXXX
 */
UINT32 SvcExifMux_Init(const SVC_EXIF_MUX_INIT_CFG_s *Config)
{
    static UINT8 g_ExifMuxMgrInit = 0U;
    UINT32 Rval = SvcExifMux_Init_ParamCheck(Config);
    if (Rval == MUXER_OK) {
        if (g_ExifMuxMgrInit == 0U) {
            UINT32 Offset = 0U, HeaderOffset = 0U;
            UINT8 i;
            SvcExifMux_InitOperation();
            Rval = W2M(AmbaWrap_memset(Config->Buffer, 0, Config->BufferSize));
            if (Rval == MUXER_OK) {
                AmbaMisra_TypeCast(&g_ExifmuxMgr.Hdlr, &Config->Buffer);
                g_ExifmuxMgr.MaxHdlr = Config->MaxHdlr;
                g_ExifmuxMgr.ExifHeadSize = Config->HeaderBufferSize / (UINT32)Config->MaxHdlr;
                Offset += g_ExifmuxMgr.MaxHdlr * (UINT32)sizeof(SVC_EXIF_MUX_HDLR_s);
                for (i = 0U; i < g_ExifmuxMgr.MaxHdlr; i++) {
                    g_ExifmuxMgr.Hdlr[i].TagBuffer.HeadBase = &Config->HeaderBuffer[HeaderOffset];
                    g_ExifmuxMgr.Hdlr[i].DataBuffer = &Config->Buffer[Offset];
                    HeaderOffset += g_ExifmuxMgr.ExifHeadSize;
                    Offset += g_ExifmuxMgr.ExifHeadSize;
                }
                g_ExifMuxMgrInit = 1U;
                Rval = CreateMutex();
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Already initiated!", __func__, NULL, NULL, NULL, NULL);
            Rval = MUXER_ERR_FATAL_ERROR;
        }
    }
    return Rval;
}

/**
 * Get the default exif tag status
 *
 * @param [in] TotalTags The number of tags
 * @param [in] Tag The target tags
 * @param [in] SetTag The set tags
 * @param [in,out] Entrys The total tags
 */
static void SvcExifMux_GetDefaultTag(UINT16 TotalTags, const SVC_TIFF_TAG_s *Tag, SVC_CFG_TAG_s *SetTag, UINT16 *Entrys)
{
    UINT16 i;
    /* Use Default value : SVC_TAG_ENABLED*/
    for (i = 0; i < TotalTags; i++) {
        SetTag[i].Set = Tag[i].Set;
        SetTag[i].Type = Tag[i].Type;
        SetTag[i].Tag = Tag[i].Tag;
        SetTag[i].Count = Tag[i].Count;
        SetTag[i].Value = Tag[i].Value;
    }
    *Entrys = TotalTags;
}

static UINT32 SvcExifMux_GetDefaultCfg_ParamCheck(const SVC_EXIF_MUX_CFG_s *Config)
{
    UINT32 Rval = MUXER_ERR_INVALID_ARG;
    if (Config != NULL) {
        Rval = MUXER_OK;
    }
    return Rval;
}

/**
 * Get the default Exif muxer config
 *
 * @param [out] Config: The buffer to get default configure.
 * @return 0 - OK, others - MUXER_ERR_XXXX
 */
UINT32 SvcExifMux_GetDefaultCfg(SVC_EXIF_MUX_CFG_s *Config)
{
    UINT32 Rval = SvcExifMux_GetDefaultCfg_ParamCheck(Config);
    if (Rval == MUXER_OK) {
        SVC_CFG_TAG_INFO_s *CfgTagInfo = &Config->SetTagInfo;
        Rval = W2M(AmbaWrap_memset(Config, 0, sizeof(SVC_EXIF_MUX_CFG_s)));
        SvcExifMux_GetDefaultTag(SVC_IFD0_TOTAL_TAGS, &Ifd0Tag[0], &CfgTagInfo->Ifd0[0], &CfgTagInfo->Ifd0Tags);
        SvcExifMux_GetDefaultTag(SVC_EXIF_TOTAL_TAGS, &ExifIfdTag[0], &CfgTagInfo->ExifIfd[0], &CfgTagInfo->ExifIfdTags);
        SvcExifMux_GetDefaultTag(SVC_IntIFD_TOTAL_TAGS, &IntIfdTag[0], &CfgTagInfo->IntIfd[0], &CfgTagInfo->IntIfdTags);
        SvcExifMux_GetDefaultTag(SVC_GPS_TOTAL_TAGS, &GpsIfdTag[0], &CfgTagInfo->GpsIfd[0], &CfgTagInfo->GpsIfdTags);
        SvcExifMux_GetDefaultTag(SVC_IFD1_TOTAL_TAGS, &Ifd1Tag[0], &CfgTagInfo->Ifd1[0], &CfgTagInfo->Ifd1Tags);
#if 0
        AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "[ExifMux_Log] Ifd0Tags : %u", CfgTagInfo->Ifd0Tags, 0U, 0U, 0U, 0U);
        AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "[ExifMux_Log] ExifIfdTags : %u", CfgTagInfo->ExifIfdTags, 0U, 0U, 0U, 0U);
        AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "[ExifMux_Log] IntIfdTags : %u", CfgTagInfo->IntIfdTags, 0U, 0U, 0U, 0U);
        AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "[ExifMux_Log] GpsIfdTags : %u", CfgTagInfo->GpsIfdTags, 0U, 0U, 0U, 0U);
        AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "[ExifMux_Log] Ifd1Tags : %u", CfgTagInfo->Ifd1Tags, 0U, 0U, 0U, 0U);
#endif
        Config->Endian = SVC_FORMAT_EXIF_LITTLEENDIAN;
    }
    return Rval;
}

/**
 * Set the exif tag status
 *
 * @param [in] SetTags The number of setTags
 * @param [in] SetTag The set tags
 * @param [in] Tag The The target tags
 * @param [out] Entrys The total tags
 * @return 0 - OK, others - MUXER_ERR_XXXX
 */
static UINT32 SvcExifMux_SetTag(UINT16 SetTags, const SVC_CFG_TAG_s *SetTag, SVC_TIFF_TAG_s *Tag, UINT16 *Entrys)
{
    UINT32 Rval = MUXER_OK;
    UINT16 i, k = 0;
    for (i = 0; i < SetTags; i++) {
        if (SetTag[i].Set != SVC_TAG_DISABLED){
            UINT32 Size = 0U;
            Tag[k].Set = SetTag[i].Set;
            Tag[k].Type = SetTag[i].Type;
            Tag[k].Tag = SetTag[i].Tag;
            Tag[k].Count = SetTag[i].Count;
            Tag[k].Value = SetTag[i].Value;
            switch (Tag[k].Type){
            case TIFF_BYTE: /* 1 byte */
            case TIFF_ASCII:
            case TIFF_UNDEFINED:
                Size = Tag[k].Count * 1U;
                break;
            case TIFF_SHORT: /* 2 bytes */
                Size = Tag[k].Count * 2U;
                break;
            case TIFF_LONG: /* 4 bytes */
            case TIFF_SLONG:
                Size = Tag[k].Count * 4U;
                break;
            case TIFF_RATIONAL: /* 8 bytes */
            case TIFF_SRATIONAL:
                Size = Tag[k].Count * 8U;
                break;
            default:
                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Wrong tiff tag!", __func__, NULL, NULL, NULL, NULL);
                Rval = MUXER_ERR_INVALID_ARG;
                break;
            }
            if (Rval == MUXER_OK) {
                if ((Tag[k].Set == SVC_TAG_CONFIGURED) && (Size > 4U)){
                    //AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "[ExifMux_Log] Configured[%u] => tag: %u, Set: %u, Size: %u", k, Tag[k].Tag, Tag[k].Set, Size, 0U);
                    if (SetTag[i].Data == NULL){
                        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Wrong value!", __func__, NULL, NULL, NULL, NULL);
                        Rval = MUXER_ERR_INVALID_ARG;
                    }
                }
                k++;
            }
            if (Rval != MUXER_OK) {
                break;
            }
        }
    }
    if (Rval == MUXER_OK) {
        *Entrys = k;
    }
    return Rval;
}

/**
 * Config the exif tag status
 *
 * @param [in] CfgTagInfo The set tags
 * @param [in] TagInfo The target tags
 * @return 0 - OK, others - MUXER_ERR_XXXX
 */
static UINT32 SvcExifMux_SetRequiredTag(const SVC_CFG_TAG_INFO_s *CfgTagInfo, SVC_TIFF_TAG_INFO_s *TagInfo)
{
    UINT32 Rval;
    Rval = SvcExifMux_SetTag(CfgTagInfo->Ifd0Tags, &CfgTagInfo->Ifd0[0], &TagInfo->Ifd0[0], &TagInfo->Ifd0Tags);
    if (Rval == MUXER_OK) {
        Rval = SvcExifMux_SetTag(CfgTagInfo->ExifIfdTags, &CfgTagInfo->ExifIfd[0], &TagInfo->ExifIfd[0], &TagInfo->ExifIfdTags);
        if (Rval == MUXER_OK) {
            Rval = SvcExifMux_SetTag(CfgTagInfo->IntIfdTags, &CfgTagInfo->IntIfd[0], &TagInfo->IntIfd[0], &TagInfo->IntIfdTags);
            if (Rval == MUXER_OK) {
                Rval = SvcExifMux_SetTag(CfgTagInfo->GpsIfdTags, &CfgTagInfo->GpsIfd[0], &TagInfo->GpsIfd[0], &TagInfo->GpsIfdTags);
                if (Rval == FORMAT_OK) {
                    Rval = SvcExifMux_SetTag(CfgTagInfo->Ifd1Tags, &CfgTagInfo->Ifd1[0], &TagInfo->Ifd1[0], &TagInfo->Ifd1Tags);
                    if (Rval != MUXER_OK) {
                        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcExifMux_SetTag failed!", __func__, NULL, NULL, NULL, NULL);
                    }
#if 0
                    AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "[ExifMux_Log] Ifd0Tags : %u %u", CfgTagInfo->Ifd0Tags, TagInfo->Ifd0Tags, 0U, 0U, 0U);
                    AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "[ExifMux_Log] ExifIfdTags : %u %u", CfgTagInfo->ExifIfdTags, TagInfo->ExifIfdTags, 0U, 0U, 0U);
                    AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "[ExifMux_Log] IntIfdTags : %u %u", CfgTagInfo->IntIfdTags, TagInfo->IntIfdTags, 0U, 0U, 0U);
                    AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "[ExifMux_Log] GpsIfdTags : %u %u", CfgTagInfo->GpsIfdTags, TagInfo->GpsIfdTags, 0U, 0U, 0U);
                    AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "[ExifMux_Log] Ifd1Tags : %u %u", CfgTagInfo->Ifd1Tags, TagInfo->Ifd1Tags, 0U, 0U, 0U);
#endif
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcExifMux_SetTag failed!", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcExifMux_SetTag failed!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcExifMux_SetTag failed!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcExifMux_SetTag failed!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 * Scan tags and calculate tag size
 *
 * @param [in] TotalTags The number of tags
 * @param [in] Tag The number of tags
 * @param [in,out] TagSize The total size of tags
 */
static void SvcExifMux_ScanTag(UINT16 TotalTags, const SVC_TIFF_TAG_s *Tag, UINT32 *TagSize)
{
    UINT16 i;
    (*TagSize) = 0;
    for (i = 0; i < TotalTags; i++) {
        if (Tag[i].Set != SVC_TAG_DISABLED) {
            UINT32 Size = 0;
            switch (Tag[i].Type){
            case TIFF_BYTE: /* 1 byte */
            case TIFF_ASCII:
            case TIFF_UNDEFINED:
                Size = Tag[i].Count * 1U;
                break;
            case TIFF_SHORT: /* 2 byte */
                Size = Tag[i].Count * 2U;
                break;
            case TIFF_LONG: /* 4 byte */
            case TIFF_SLONG:
                Size = Tag[i].Count * 4U;
                break;
            case TIFF_RATIONAL: /* 8 byte */
            case TIFF_SRATIONAL:
                Size = Tag[i].Count * 8U;
                break;
            default:
                // Wrong tiff Tag
                break;
            }
            if (Size > 4U){
                (*TagSize) += Size;
            }
        }
    }
}

/**
 * Calculate overall tag size
 *
 * @param [in,out] TagInfo The exif tag information
 */
static void SvcExifMux_GetTagInfo(SVC_TIFF_TAG_INFO_s *TagInfo)
{
    UINT32 Ifd0HeaderSz = 0, Ifd0InfoSz, nExififdInfoSz, nIntifdInfoSz, nGpsifdInfoSz;
    SvcExifMux_ScanTag(TagInfo->Ifd0Tags, &TagInfo->Ifd0[0], &Ifd0InfoSz);
    Ifd0HeaderSz += (2U + ((UINT32)TagInfo->Ifd0Tags * 12U) + 4U); /* IFD0 entry + IFD0 tag + NEXTIFD offSet */

    SvcExifMux_ScanTag(TagInfo->ExifIfdTags , &TagInfo->ExifIfd[0], &nExififdInfoSz);
    Ifd0HeaderSz += (2U + ((UINT32)TagInfo->ExifIfdTags * 12U) + 4U); /* EXIFIFD entry + EXIFIFD tag + NEXTIFD offSet */

    SvcExifMux_ScanTag(TagInfo->IntIfdTags, &TagInfo->IntIfd[0], &nIntifdInfoSz);
    Ifd0HeaderSz += (2U + ((UINT32)TagInfo->IntIfdTags * 12U) + 4U); /* INTIFD entry + INTIFD tag + NEXTIFD offSet */

    SvcExifMux_ScanTag(TagInfo->GpsIfdTags, &TagInfo->GpsIfd[0], &nGpsifdInfoSz);
    Ifd0HeaderSz += (2U + ((UINT32)TagInfo->GpsIfdTags * 12U) + 4U); /* GPSIFD entry + GPSIFD tag + NEXTIFD offSet */

    SvcExifMux_ScanTag(TagInfo->Ifd1Tags, &TagInfo->Ifd1[0], &TagInfo->Ifd1InfoSz);
    TagInfo->Ifd1HeaderSz = (2U + ((UINT32)TagInfo->Ifd1Tags * 12U) + 4U); /* IFD1 entry + IFD1 tag + NEXTIFD offSet */

    TagInfo->Ifd0HeaderSz = Ifd0HeaderSz;
    TagInfo->AmbaBoxSz = TIFF_EXIF_AMBA_1_0_SIZE;
    TagInfo->Ifd0InfoSz = Ifd0InfoSz + nExififdInfoSz + nIntifdInfoSz + nGpsifdInfoSz + TagInfo->AmbaBoxSz;

    TagInfo->HeaderSz = 20U + TagInfo->Ifd0HeaderSz + TagInfo->Ifd1HeaderSz;
    TagInfo->InfoSz = TagInfo->Ifd0InfoSz + TagInfo->Ifd1InfoSz;

    TagInfo->HeaderSz = ((TagInfo->HeaderSz / 512U) + 1U) * 512U;
    TagInfo->InfoSz = ((TagInfo->InfoSz / 512U) + 1U) * 512U;
#if 0
    AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "[ExifMux_Log] Ifd0HeaderSz : %u", TagInfo->Ifd0HeaderSz, 0U, 0U, 0U, 0U);
    AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "[ExifMux_Log] Ifd1HeaderSz : %u", TagInfo->Ifd1HeaderSz, 0U, 0U, 0U, 0U);
    AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "[ExifMux_Log] Ifd0InfoSz : %u", TagInfo->Ifd0InfoSz, 0U, 0U, 0U, 0U);
    AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "[ExifMux_Log] Ifd1InfoSz : %u", TagInfo->Ifd1InfoSz, 0U, 0U, 0U, 0U);
    AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "[ExifMux_Log] HeaderSz : %u, InfoSz: %u", TagInfo->HeaderSz, TagInfo->InfoSz, 0U, 0U, 0U);
#endif
}

/**
 * Copy the setting data of specific tag
 *
 * @param [in] Tag The target tag
 * @param [in] SetTag The set tag
 * @param [in] TagBuffer The buffer to set data
 * @param [in] BufferSize The buffer size
 * @return buffer address
 */
static UINT8 *SvcExifMux_CopyData(SVC_TIFF_TAG_s *Tag, const SVC_CFG_TAG_s *SetTag, UINT8 *TagBuffer, UINT32 *BufferSize)
{
    UINT8 *BufAddr = TagBuffer;
    UINT32 Size = 0U;
    switch (Tag->Type){
    case TIFF_BYTE: /* 1 byte */
    case TIFF_ASCII:
    case TIFF_UNDEFINED:
        Size = SetTag->Count * 1U;
        break;
    case TIFF_SHORT: /* 2 byte */
        Size = SetTag->Count * 2U;
        break;
    case TIFF_LONG: /* 4 byte */
    case TIFF_SLONG:
        Size = SetTag->Count * 4U;
        break;
    case TIFF_RATIONAL: /* 8 byte */
    case TIFF_SRATIONAL:
        Size = SetTag->Count * 8U;
        break;
    default:
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "Wrong tiff Tag!", __func__, NULL, NULL, NULL, NULL);
        BufAddr = NULL;
        break;
    }
    if (BufAddr != NULL) {
        if (Size > 4U){
            if (SetTag->Data != NULL){
                if (Size >= *BufferSize) {
                    AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "Wrong addr!", __func__, NULL, NULL, NULL, NULL);
                    BufAddr = NULL;
                }
                if (BufAddr != NULL) {
                    Tag->Data = BufAddr;
                    if (W2F(AmbaWrap_memcpy(Tag->Data, SetTag->Data, Size)) != FORMAT_OK) {
                        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "memcpy failed!", __func__, NULL, NULL, NULL, NULL);
                        BufAddr = NULL;
                    } else {
                        BufAddr = &BufAddr[Size];
                        (*BufferSize) -= Size;
                    }
                }
            } else {
                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "Wrong value!", __func__, NULL, NULL, NULL, NULL);
                BufAddr = NULL;
            }
        }
    }
    return BufAddr;
}

/**
 * Copy the setting data of specific IFD
 *
 * @param [in] TotalTags The number of set tags
 * @param [in] SetTag The set tag
 * @param [in] Entrys The number of target tags
 * @param [in] Tag The target tag
 * @param [in] TagBuffer The buffer to set data
 * @param [in] BufferSize The buffer size
 * @return buffer address
 */
static UINT8 *SvcExifMux_CopyIfdData(UINT16 TotalTags, const SVC_CFG_TAG_s *SetTag, UINT16 Entrys, SVC_TIFF_TAG_s *Tag, UINT8 *TagBuffer, UINT32 *BufferSize)
{
    UINT8 *CurPtr = TagBuffer;
    UINT16 i, j;
    for (i = 0; i < Entrys; i++) {
        for (j = 0; j < TotalTags; j++){
            if (Tag[i].Tag == SetTag[j].Tag) {
                if (Tag[i].Set == SVC_TAG_CONFIGURED){
                    CurPtr = SvcExifMux_CopyData(&Tag[i], &SetTag[j], CurPtr, BufferSize);
                    if (CurPtr == NULL){
                        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcExifMux_CopyData failed!", __func__, NULL, NULL, NULL, NULL);
                        break;
                    }
                }
            }
        }
        if (CurPtr == NULL){
            break;
        }
    }
    return CurPtr;
}

/**
 * Copy the setting data of overall tags
 *
 * @param [in] SetTagInfo The number of set tags
 * @param [in] TagInfo The set tag
 * @param [in] TagBuffer The buffer to set data
 * @param [in] BufferSize The buffer size
 * @return 0 - OK, others - MUXER_ERR_XXXX
 */
static UINT32 SvcExifMux_ConfigTagData(const SVC_CFG_TAG_INFO_s *SetTagInfo, SVC_TIFF_TAG_INFO_s *TagInfo, UINT8 *TagBuffer, UINT32 BufferSize)
{
    UINT32 Rval = MUXER_ERR_FATAL_ERROR;
    UINT8 *CurPtr = TagBuffer;
    UINT32 CurrBufferSize = BufferSize;
    CurPtr = SvcExifMux_CopyIfdData(SetTagInfo->Ifd0Tags, &SetTagInfo->Ifd0[0], TagInfo->Ifd0Tags, &TagInfo->Ifd0[0], CurPtr, &CurrBufferSize);
    if (CurPtr != NULL) {
        CurPtr = SvcExifMux_CopyIfdData(SetTagInfo->ExifIfdTags, &SetTagInfo->ExifIfd[0], TagInfo->ExifIfdTags, &TagInfo->ExifIfd[0], CurPtr, &CurrBufferSize);
        if (CurPtr != NULL) {
            CurPtr = SvcExifMux_CopyIfdData(SetTagInfo->IntIfdTags, &SetTagInfo->IntIfd[0], TagInfo->IntIfdTags, &TagInfo->IntIfd[0], CurPtr, &CurrBufferSize);
            if (CurPtr != NULL) {
                CurPtr = SvcExifMux_CopyIfdData(SetTagInfo->GpsIfdTags, &SetTagInfo->GpsIfd[0], TagInfo->GpsIfdTags, &TagInfo->GpsIfd[0], CurPtr, &CurrBufferSize);
                if (CurPtr != NULL) {
                    CurPtr = SvcExifMux_CopyIfdData(SetTagInfo->Ifd1Tags, &SetTagInfo->Ifd1[0], TagInfo->Ifd1Tags, &TagInfo->Ifd1[0], CurPtr, &CurrBufferSize);
                    if (CurPtr != NULL) {
                        Rval = MUXER_OK;
                    } else {
                        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcExifMux_CopyIfdData failed!", __func__, NULL, NULL, NULL, NULL);
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcExifMux_CopyIfdData failed!", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcExifMux_CopyIfdData failed!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcExifMux_CopyIfdData failed!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcExifMux_CopyIfdData failed!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 GetHdlr(SVC_EXIF_MUX_HDLR_s **Hdlr)
{
    UINT32 Rval = MUXER_ERR_FATAL_ERROR;
    UINT8 i;
    for (i = 0; i < g_ExifmuxMgr.MaxHdlr; i++) {
        SVC_EXIF_MUX_HDLR_s *ExifMux = &g_ExifmuxMgr.Hdlr[i];
        if (ExifMux->Used == 0U) {
            ExifMux->Used = 1U;
            *Hdlr = ExifMux;
            Rval = MUXER_OK;
            break;
        }
    }
    if (i == g_ExifmuxMgr.MaxHdlr) {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Get Hdlr fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 RelHdlr(SVC_EXIF_MUX_HDLR_s *Hdlr)
{
    UINT32 Rval = MUXER_ERR_INVALID_ARG;
    UINT8 i;
    for (i = 0; i < g_ExifmuxMgr.MaxHdlr; i++) {
        const SVC_EXIF_MUX_HDLR_s *ExifMux = &g_ExifmuxMgr.Hdlr[i];
        if (ExifMux == Hdlr) {
            if (Hdlr->Used == 1U) {
                Hdlr->Used = 0U;
                Rval = MUXER_OK;
            } else {
                // error...
                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s ExifMux idle!", __func__, NULL, NULL, NULL, NULL);
                Rval = MUXER_ERR_FATAL_ERROR;
            }
            break;
        }
    }
    if (i == g_ExifmuxMgr.MaxHdlr) {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Get Hdlr fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 SvcExifMux_CreateImpl(const SVC_EXIF_MUX_CFG_s *Config, SVC_EXIF_MUX_HDLR_s **Hdlr)
{
    UINT32 Rval;
    SVC_EXIF_MUX_HDLR_s *ExifMux;
    Rval = GetHdlr(&ExifMux);
    if (Rval == MUXER_OK) {
        Rval = K2M(AmbaKAL_MutexTake(&ExifMux->Mutex, SVC_FORMAT_WAIT_TIMEOUT));
        if (Rval == MUXER_OK) {
            const SVC_CFG_TAG_INFO_s * const SetTagInfo = &Config->SetTagInfo;
            SVC_TIFF_TAG_INFO_s * const TagInfo = &ExifMux->TagInfo;
            SvcMuxer_InitImageMuxHdlr(&ExifMux->Hdlr, &g_ExifMux, Config->Stream);
            ExifMux->Endian = Config->Endian;
            Rval = SvcExifMux_SetRequiredTag(SetTagInfo, TagInfo);
            if (Rval == MUXER_OK) {
                SvcExifMux_GetTagInfo(TagInfo);
                Rval = W2M(AmbaWrap_memset(ExifMux->TagBuffer.HeadBase, 0, g_ExifmuxMgr.ExifHeadSize));
                if (Rval == MUXER_OK) {
                    Rval = W2M(AmbaWrap_memset(ExifMux->DataBuffer, 0, g_ExifmuxMgr.ExifHeadSize));
                    if (Rval == MUXER_OK) {
                        ExifMux->TagBuffer.HeadOffset = 0U;
                        ExifMux->TagBuffer.HeadSize = TagInfo->HeaderSz;
                        ExifMux->TagBuffer.InfoBase = &ExifMux->TagBuffer.HeadBase[TagInfo->HeaderSz];
                        ExifMux->TagBuffer.InfoOffset = 0U;
                        ExifMux->TagBuffer.InfoSize = TagInfo->InfoSz;
                        if ((TagInfo->HeaderSz + TagInfo->InfoSz) <= g_ExifmuxMgr.ExifHeadSize) {
                            UINT8 *DataBuffer = ExifMux->DataBuffer;
#if 0
                            AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "[ExifMux_Log] HeadBuffer : %x", (UINT32)ExifMux->TagBuffer.HeadBase, 0U, 0U, 0U, 0U);
                            AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "[ExifMux_Log] InfoBuffer : %x", (UINT32)ExifMux->TagBuffer.InfoBase, 0U, 0U, 0U, 0U);
                            AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "[ExifMux_Log] DataBuffer : %x", (UINT32)ExifMux->TagBuffer.InfoSize, 0U, 0U, 0U, 0U);
                            AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "[ExifMux_Log] BufferLimit : %x", (UINT32)BufferLimit, 0U, 0U, 0U, 0U);
#endif
                            Rval = SvcExifMux_ConfigTagData(SetTagInfo, TagInfo, DataBuffer, g_ExifmuxMgr.ExifHeadSize);
                            if (Rval == MUXER_OK) {
                                *Hdlr = ExifMux;
                            }
                        } else {
                            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s The header size is not enough!", __func__, NULL, NULL, NULL, NULL);
                            Rval = MUXER_ERR_FATAL_ERROR;
                        }
                    }
                }
            }
            if (AmbaKAL_MutexGive(&ExifMux->Mutex) != OK) {
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

static UINT32 SvcExifMux_Create_ParamCheck(const SVC_EXIF_MUX_CFG_s *Config, SVC_MUX_FORMAT_HDLR_s * const *Hdlr)
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
 * Create a Exif mux handler
 *
 * @param [in] Config The configure of the muxer.
 * @param [out] Hdlr The double pointer to get the handler.
 * @return 0 - OK, others - MUXER_ERR_XXXX
 */
UINT32 SvcExifMux_Create(const SVC_EXIF_MUX_CFG_s *Config, SVC_MUX_FORMAT_HDLR_s **Hdlr)
{
    UINT32 Rval;
    Rval = SvcExifMux_Create_ParamCheck(Config, Hdlr);
    if (Rval == MUXER_OK) {
        Rval = K2M(AmbaKAL_MutexTake(&g_ExifmuxMgr.Mutex, SVC_FORMAT_WAIT_TIMEOUT));
        if (Rval == MUXER_OK) {
            SVC_EXIF_MUX_HDLR_s *ExifMux;
            Rval = SvcExifMux_CreateImpl(Config, &ExifMux);
            if (Rval == MUXER_OK) {
                *Hdlr = &ExifMux->Hdlr;
            }
            if (AmbaKAL_MutexGive(&g_ExifmuxMgr.Mutex) != OK) {
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

/**
 * Implement function to delete Exif mux handler
 *
 * @param [in] Hdlr The handler of exif mux to delete.
 * @return 0 - OK, others - MUXER_ERR_XXXX
 */
static UINT32 SvcExifMux_DeleteImpl(SVC_EXIF_MUX_HDLR_s *Hdlr)
{
    UINT32 Rval = K2M(AmbaKAL_MutexTake(&Hdlr->Mutex, SVC_FORMAT_WAIT_TIMEOUT));
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

static UINT32 SvcExifMux_Delete_ParamCheck(const SVC_MUX_FORMAT_HDLR_s *Hdlr)
{
    UINT32 Rval = MUXER_ERR_INVALID_ARG;
    if (Hdlr != NULL) {
        Rval = MUXER_OK;
    }
    return Rval;
}

/**
 * Delete a Exif muxer handler
 *
 * @param [in] Hdlr the handler of exif mux to delete.
 * @return 0 - OK, others - MUXER_ERR_XXXX
 */
UINT32 SvcExifMux_Delete(SVC_MUX_FORMAT_HDLR_s *Hdlr)
{
    UINT32 Rval = SvcExifMux_Delete_ParamCheck(Hdlr);
    if (Rval == MUXER_OK) {
        Rval = K2M(AmbaKAL_MutexTake(&g_ExifmuxMgr.Mutex, SVC_FORMAT_WAIT_TIMEOUT));
        if (Rval == MUXER_OK) {
            SVC_EXIF_MUX_HDLR_s *ExifMux;
            AmbaMisra_TypeCast(&ExifMux, &Hdlr);
            Rval = SvcExifMux_DeleteImpl(ExifMux);
            if (AmbaKAL_MutexGive(&g_ExifmuxMgr.Mutex) != OK) {
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

/**
 * Put pim entries
 *
 * @param [in] Buffer The tag buffer status
 * @param [in] Tag The tag id to be written
 * @param [in] Value The value to be written
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
static UINT32 ExifMux_PutPim_Entry(SVC_TIFF_TAG_BUFFER_s *Buffer, UINT16 Tag, UINT32 Value)
{
    UINT32 Rval;
    Rval = SvcFormat_PutMem16(&Buffer->InfoOffset, Buffer->InfoBase, Buffer->InfoSize, Tag);
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutMem32(&Buffer->InfoOffset, Buffer->InfoBase, Buffer->InfoSize, Value);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PutMem32() failed!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PutMem16() failed!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 * PIM 3.0
 *
 * @param [in] Buffer The tag buffer status
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
static UINT32 ExifMux_PutPim(SVC_TIFF_TAG_BUFFER_s *Buffer)
{
    UINT32 Rval;
    Rval = SvcFormat_PutMemBe32(&Buffer->InfoOffset, Buffer->InfoBase, Buffer->InfoSize, 0x5072696EU); /* Prin */
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutMemBe32(&Buffer->InfoOffset, Buffer->InfoBase, Buffer->InfoSize, 0x74494D00U); /* tIM\0 */
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_PutMemBe32(&Buffer->InfoOffset, Buffer->InfoBase, Buffer->InfoSize, 0x30333030U); /* 0300 */
            if (Rval == FORMAT_OK) {
                Rval = SvcFormat_PutMem16(&Buffer->InfoOffset, Buffer->InfoBase, Buffer->InfoSize, 0U); /* Reserved */
                if (Rval == FORMAT_OK) {
                    Rval = SvcFormat_PutMem16(&Buffer->InfoOffset, Buffer->InfoBase, Buffer->InfoSize, TIFF_PIMTAGS); /* Number of Tags(3) */
                    if (Rval == FORMAT_OK) {
                        Rval = ExifMux_PutPim_Entry(Buffer, 0x01U, 0x00220022U); /* GAMMA , {pos[15:0],neg[15:0]} */
                        if (Rval == FORMAT_OK) {
                            Rval = ExifMux_PutPim_Entry(Buffer, 0x0AU, 0U); /* Brightness Setting */
                            if (Rval == FORMAT_OK) {
                                Rval = ExifMux_PutPim_Entry(Buffer, 0x0BU, Buffer->InfoOffset + 4U); /* RGB color balance */
                                if (Rval == FORMAT_OK) {
                                    Rval = SvcFormat_PutMemPadding(&Buffer->InfoOffset, Buffer->InfoBase, Buffer->InfoSize, 6U);
                                    if (Rval != FORMAT_OK) {
                                        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PutMemPadding() failed!", __func__, NULL, NULL, NULL, NULL);
                                    }
                                } else {
                                    AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s ExifMux_PutPim_Entry() failed!", __func__, NULL, NULL, NULL, NULL);
                                }
                            } else {
                                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s ExifMux_PutPim_Entry() failed!", __func__, NULL, NULL, NULL, NULL);
                            }
                        } else {
                            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s ExifMux_PutPim_Entry() failed!", __func__, NULL, NULL, NULL, NULL);
                        }
                    } else {
                        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PutMem16() failed!", __func__, NULL, NULL, NULL, NULL);
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PutMem16() failed!", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PutMemBe32() failed!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PutMemBe32() failed!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Rval;
}

/**
 * Put information of exif tags
 *
 * @param [in] Tag The tag to be set
 * @param [in] Buffer The tag buffer status
 * @param [in] HeadSize The header size
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
static UINT32 ExifMux_PutEntry(const SVC_TIFF_TAG_s *Tag, SVC_TIFF_TAG_BUFFER_s *Buffer, UINT32 HeadSize)
{
    UINT32 Rval;
    if ((Tag->Type == TIFF_ASCII) || (Tag->Type == TIFF_BYTE) || (Tag->Type == TIFF_UNDEFINED)) {
        Rval = SvcFormat_Put1Byte(Tag, Buffer, HeadSize, Tag->Data);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_Put1Byte() failed!", __func__, NULL, NULL, NULL, NULL);
        }
    } else if (Tag->Type == TIFF_SHORT) {
        Rval = SvcFormat_Put2Byte(Tag, Buffer, HeadSize, Tag->Data);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_Put2Byte() failed!", __func__, NULL, NULL, NULL, NULL);
        }
    } else if (Tag->Type == TIFF_LONG) {
        Rval = SvcFormat_Put4Byte(Tag, Buffer, HeadSize, Tag->Data);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_Put4Byte() failed!", __func__, NULL, NULL, NULL, NULL);
        }
    } else if ((Tag->Type == TIFF_RATIONAL) || (Tag->Type == TIFF_SRATIONAL)) {
        Rval = SvcFormat_Put8Byte(Tag, Buffer, HeadSize, Tag->Data);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_Put8Byte() failed!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        Rval = FORMAT_ERR_GENERAL_ERROR;
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Wrong tiff type!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 * Write Exif Ifd0
 *
 * @param [in] Image The image information
 * @param [in] Buffer The tag buffer status
 * @param [in] TagInfo The tag information
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
static UINT32 ExifMux_PutIfd0(const SVC_IMAGE_INFO_s *Image, SVC_TIFF_TAG_BUFFER_s *Buffer, SVC_TIFF_TAG_INFO_s *TagInfo)
{
    /* TODO: add Maker, Model, SoftVer, ImageDescr, BitsPerSample, Resolution, Date ... to Image Info */
    UINT32 Rval = SvcFormat_PutMem16(&Buffer->HeadOffset, Buffer->HeadBase,  Buffer->HeadSize, TagInfo->Ifd0Tags);/* entries */
    if (Rval == FORMAT_OK) {
        UINT16 i;
        for (i = 0; i < TagInfo->Ifd0Tags; i++) {
            if (TagInfo->Ifd0[i].Set == SVC_TAG_ENABLED) {
                switch (TagInfo->Ifd0[i].Tag) {
                case SVC_TIFF_ImageDescription:
                    /* ASCII (1 byte) , Use default value "DCIM/100MEDIA" */
                    {
                        const char ImageDescr[14] = {'T','E','S','T','.','J','P','G','0','0','0','0','0','\0'};
                        Rval = SvcFormat_Put1Byte(&TagInfo->Ifd0[i], Buffer, TagInfo->HeaderSz, (const UINT8 *)ImageDescr);
                    }
                    break;
                case SVC_TIFF_Make :
                    /* ASCII (1 byte), Use default value "AMBARELLA"*/
                    {
                        const char Maker[24] = {'A','m','b','a','r','e','l','l','a',' ',' ',' ',
                            ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','\0'};
                        Rval = SvcFormat_Put1Byte(&TagInfo->Ifd0[i], Buffer, TagInfo->HeaderSz, (const UINT8 *)Maker);
                    }
                    break;
                case SVC_TIFF_Model:
                    /* ASCII (1 byte), Use default value "AMBARELLA MK-1"*/
                    {
                        const char Model[24] = {'A','m','b','a','r','e','l','l','a',' ','M','K',
                            '-','1',' ',' ',' ',' ',' ',' ',' ',' ',' ','\0'};
                        Rval = SvcFormat_Put1Byte(&TagInfo->Ifd0[i], Buffer, TagInfo->HeaderSz, (const UINT8 *)Model);
                    }
                    break;
                case SVC_TIFF_Software:
                    /* ASCII (1 byte), Use default value "Ver. 1.0.000"*/
                    {
                        const char SoftVer[12] = {'V','e','r','.','1','.','0','.','0','0','0','\0'};
                        Rval = SvcFormat_Put1Byte(&TagInfo->Ifd0[i], Buffer, TagInfo->HeaderSz, (const UINT8 *)SoftVer);
                    }
                    break;
                case SVC_TIFF_DateTime:
                    /* ASCII (1 byte) Not adjustable now*/
                    Rval = SvcFormat_Put1Byte(&TagInfo->Ifd0[i], Buffer, TagInfo->HeaderSz, (const UINT8 *)Image->MediaInfo.Date/*U8Ptr*/);
                    break;
                case SVC_TIFF_BitsPerSample:
                    /* SHORT (2 bytes), use default value R: 8, G: 8, B: 8 */
                    {
                        UINT16 BitsPerSample[3] = {8U, 8U, 8U};
                        const UINT16 *U16Ptr = BitsPerSample;
                        const UINT8 *U8Ptr;
                        AmbaMisra_TypeCast(&U8Ptr, &U16Ptr);
                        Rval = SvcFormat_Put2Byte(&TagInfo->Ifd0[i], Buffer, TagInfo->HeaderSz, U8Ptr);
                    }
                    break;
                case SVC_TIFF_ExifIFDPointer:
                    /* LONG (4 bytes), EXIF IFD offSet, cannot be adjusted */
                    TagInfo->Ifd0[i].Value = 8U + 2U + 4U + ((UINT32)TagInfo->Ifd0Tags * 12U);
                    Rval = SvcFormat_Put4Byte(&TagInfo->Ifd0[i], Buffer, TagInfo->HeaderSz, NULL);
                    break;
                case SVC_TIFF_GPSInfoIFDPointer:
                    /* LONG (4 bytes), GPS IFD offSet, cannot be adjusted */
                    TagInfo->Ifd0[i].Value = 8U + 2U + 4U + ((UINT32)TagInfo->Ifd0Tags * 12U) + 2U + 4U + ((UINT32)TagInfo->ExifIfdTags * 12U) + 2U + 4U + ((UINT32)TagInfo->IntIfdTags * 12U);
                    Rval = SvcFormat_Put4Byte(&TagInfo->Ifd0[i], Buffer, TagInfo->HeaderSz, NULL);
                    break;
                case SVC_TIFF_ImageWidth:
                    /* LONG (4 bytes). Not adjustable now*/
                    TagInfo->Ifd0[i].Value = (UINT32)Image->Frame[0].Width;
                    Rval = SvcFormat_Put4Byte(&TagInfo->Ifd0[i], Buffer, TagInfo->HeaderSz, NULL);
                    break;
                case SVC_TIFF_ImageHeight:
                    /* LONG (4 bytes). Not adjustable now*/
                    TagInfo->Ifd0[i].Value = (UINT32)Image->Frame[0].Height;
                    Rval = SvcFormat_Put4Byte(&TagInfo->Ifd0[i], Buffer, TagInfo->HeaderSz, NULL);
                    break;
                case SVC_TIFF_XResolution:
                case SVC_TIFF_YResolution:
                    /* RATIONAL (8 bytes), always more than 4 bytes, Use default value "72, 1" */
                    {
                        UINT8 Resolution[8];
                        SvcFormat_Convert64(Resolution, 72U, 1U);
                        Rval = SvcFormat_Put8Byte(&TagInfo->Ifd0[i], Buffer, TagInfo->HeaderSz, Resolution);
                    }
                    break;
                case SVC_TIFF_PIM:
                    /* Undefined (1 byte), Not configurable, use default value */
                    Rval = SvcFormat_PutIfdEntry(&Buffer->HeadOffset, Buffer->HeadBase, Buffer->HeadSize, TagInfo->Ifd0[i].Tag, TagInfo->Ifd0[i].Type, TagInfo->Ifd0[i].Count, Buffer->InfoOffset + TagInfo->HeaderSz - 12U);
                    if (Rval == FORMAT_OK) {
                        Rval = ExifMux_PutPim(Buffer);
                        if (Rval != FORMAT_OK) {
                            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s ExifMux_PutPim() failed!", __func__, NULL, NULL, NULL, NULL);
                        }
                    } else {
                        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PutIfdEntry() failed!", __func__, NULL, NULL, NULL, NULL);
                    }
                    break;
                default:
                    Rval = ExifMux_PutEntry(&TagInfo->Ifd0[i], Buffer, TagInfo->HeaderSz);
                    break;
                }
            } else if (TagInfo->Ifd0[i].Set == SVC_TAG_CONFIGURED) {
                Rval = ExifMux_PutEntry(&TagInfo->Ifd0[i], Buffer, TagInfo->HeaderSz);
            } else {
                // SVC_TAG_DISABLED
            }
            if (Rval != FORMAT_OK) {
                AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "ExifMux_PutIfd0 Put(%x) error! Type %u Count %u", TagInfo->Ifd0[i].Tag, TagInfo->Ifd0[i].Type, TagInfo->Ifd0[i].Count, 0U, 0U);
                break;
            }
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PutMem16() failed!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 * Write Exif IntIfd
 *
 * @param [in] Buffer The tag buffer status
 * @param [in] TagInfo The tag information
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
static UINT32 ExifMux_PutIntIfd(SVC_TIFF_TAG_BUFFER_s *Buffer, const SVC_TIFF_TAG_INFO_s *TagInfo)
{
    UINT32 Rval = SvcFormat_PutMem16(&Buffer->HeadOffset, Buffer->HeadBase, Buffer->HeadSize, TagInfo->IntIfdTags);
    if (Rval == FORMAT_OK) {
        UINT16 i;
        for (i = 0; i < TagInfo->IntIfdTags; i++) {
            if (TagInfo->IntIfd[i].Set != SVC_TAG_DISABLED) {
                Rval = ExifMux_PutEntry(&TagInfo->IntIfd[i], Buffer, TagInfo->HeaderSz);
                if (Rval != FORMAT_OK) {
                    AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s ExifMux_PutEntry() failed!", __func__, NULL, NULL, NULL, NULL);
                    break;
                }
            }
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_PutMem32(&Buffer->HeadOffset, Buffer->HeadBase, Buffer->HeadSize, 0U); /* NextIfdOffset */
            if (Rval != FORMAT_OK) {
                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PutMem32() failed!", __func__, NULL, NULL, NULL, NULL);
            }
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PutMem16() failed!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 * Write ambabox (user data)
 *
 * @param [in] Buffer The tag buffer status
 * @param [in] AmbaBoxSz The size of amba box
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
static UINT32 ExifMux_PutAmbaBox(SVC_TIFF_TAG_BUFFER_s *Buffer, UINT32 AmbaBoxSz)
{
    /* ver_0_1  size|tag|    data    |version| default: 64 byte
       ver_1_0  tag|    data    |size|version|                */
    UINT32 Rval = SvcFormat_PutMem32(&Buffer->InfoOffset, Buffer->InfoBase, Buffer->InfoSize, 0x414d4241U/*AMBA*/);
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutMemPadding(&Buffer->InfoOffset, Buffer->InfoBase, Buffer->InfoSize, AmbaBoxSz - 10U);
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_PutMem32(&Buffer->InfoOffset, Buffer->InfoBase, Buffer->InfoSize, AmbaBoxSz);
            if (Rval == FORMAT_OK) {
                 /* AMBA box version */
                Rval = SvcFormat_PutMemByte(&Buffer->InfoOffset, Buffer->InfoBase, Buffer->InfoSize, 1U);
                if (Rval == FORMAT_OK) {
                    Rval = SvcFormat_PutMemByte(&Buffer->InfoOffset, Buffer->InfoBase, Buffer->InfoSize, 0U);
                    if (Rval != FORMAT_OK) {
                        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PutMemByte() failed!", __func__, NULL, NULL, NULL, NULL);
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PutMemByte() failed!", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PutMem32() failed!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PutMemPadding() failed!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PutMem32() failed!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 * Write ExifIFD (Type : ASCII / BYTE / UNDEFINED)
 *
 * @param [in] Image The image information
 * @param [in] Tag The target tag to be written
 * @param [in] Buffer The tag buffer status
 * @param [in] HeadSize The header size of tags
 * @param [in] AmbaBoxSz The size of amba box
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
static UINT32 ExifMux_PutExifIfd_1byte(const SVC_IMAGE_INFO_s *Image, SVC_TIFF_TAG_s *Tag, SVC_TIFF_TAG_BUFFER_s *Buffer, UINT32 HeadSize, UINT32 AmbaBoxSz)
{
    UINT32 Rval;
    /* ASCII (1 byte), BYTE(1 byte), UNDEFINED(1byte)*/
    switch (Tag->Tag) {
    case SVC_EXIFIFD_DateTimeOriginal:
        /*Use default value "Image->date" */
        Rval = SvcFormat_Put1Byte(Tag, Buffer, HeadSize, (const UINT8 *)Image->MediaInfo.Date);
        break;
    case SVC_EXIFIFD_DateTimeDigitized:
        /* Use default value "Image->date" */
        Rval = SvcFormat_Put1Byte(Tag, Buffer, HeadSize, (const UINT8 *)Image->MediaInfo.Date);
        break;
    case SVC_EXIFIFD_ComponentsConfiguration:
        Tag->Value = 0x01020300U;
        Rval = SvcFormat_Put1Byte(Tag, Buffer, HeadSize, NULL);
        break;
    case SVC_EXIFIFD_MakerNote:
        Rval = SvcFormat_PutIfdEntry(&Buffer->HeadOffset, Buffer->HeadBase, Buffer->HeadSize,
                Tag->Tag, Tag->Type, Tag->Count + AmbaBoxSz, Buffer->InfoOffset + HeadSize - 12U);
        if (Rval == FORMAT_OK) {
            UINT32 Count = Tag->Count;
            if (Count > 4U) {
                Rval = SvcFormat_WriteMem(&Buffer->InfoOffset, Buffer->InfoBase, Buffer->InfoSize, Count, Tag->Data);
            } else if (Count > 0U) {
                Rval = SvcFormat_PutMem32(&Buffer->InfoOffset, Buffer->InfoBase, Buffer->InfoSize, Tag->Value);
            } else {
                // without user defined MakerNote
            }
            if (Rval == FORMAT_OK) {
                Rval = ExifMux_PutAmbaBox(Buffer, AmbaBoxSz);
                if (Rval != FORMAT_OK) {
                    AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s ExifMux_PutAmbaBox() failed!", __func__, NULL, NULL, NULL, NULL);
                }
            }
        }
        break;
    case SVC_EXIFIFD_FlashpixVersion:
        Tag->Value = ((UINT32)Image->ExifInfo.ExifIfdInfo.FlashPixVersion[0] << 24) + ((UINT32)Image->ExifInfo.ExifIfdInfo.FlashPixVersion[1] << 16)
        + ((UINT32)Image->ExifInfo.ExifIfdInfo.FlashPixVersion[2] << 8) + ((UINT32)Image->ExifInfo.ExifIfdInfo.FlashPixVersion[3]);
        Rval = SvcFormat_Put1Byte(Tag, Buffer, HeadSize, NULL);
        break;
    case SVC_EXIFIFD_DeviceSettingDescription:
        if (Image->ExifInfo.ExifIfdInfo.PriImgDataSize > 0U) {
            Tag->Count = Image->ExifInfo.ExifIfdInfo.PriImgDataSize;
            Rval = SvcFormat_Put1Byte(Tag, Buffer, HeadSize, Image->ExifInfo.ExifIfdInfo.PrivateImageData);
        } else {
            Tag->Count = 4;
            Tag->Value = 0;
            Rval = SvcFormat_Put1Byte(Tag, Buffer, HeadSize, NULL);
        }
        break;
    case SVC_EXIFIFD_FileSource:
        Tag->Value = SvcFormat_Le2Be32(Image->ExifInfo.ExifIfdInfo.FileSource);
        Rval = SvcFormat_Put1Byte(Tag, Buffer, HeadSize, NULL);
        break;
    case SVC_EXIFIFD_SceneType:
        Tag->Value = SvcFormat_Le2Be32(Image->ExifInfo.ExifIfdInfo.SceneType);
        Rval = SvcFormat_Put1Byte(Tag, Buffer, HeadSize, NULL);
        break;
    default:
        Rval = SvcFormat_Put1Byte(Tag, Buffer, HeadSize, Tag->Data);
        break;
    }
    return Rval;
}

/**
 * Write ExifIFD (Type : SHORT)
 *
 * @param [in] Image The image information
 * @param [in] Tag The target tag to be written
 * @param [in] Buffer The tag buffer status
 * @param [in] HeadSize The header size of tags
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
static UINT32 ExifMux_PutExifIfd_2byte(const SVC_IMAGE_INFO_s *Image, SVC_TIFF_TAG_s *Tag, SVC_TIFF_TAG_BUFFER_s *Buffer, UINT32 HeadSize)
{
    UINT32 Rval;
    /* SHORT (2 bytes) */
    switch (Tag->Tag) {
    case SVC_EXIFIFD_ExposureProgram:
        Tag->Value = Image->ExifInfo.ExifIfdInfo.ExposureProgram;
        break;
    case SVC_EXIFIFD_ISOSpeedRatings: /* ~2.21; PhohographicSentivity in 2.3 */
        Tag->Value = Image->ExifInfo.ExifIfdInfo.IsoSpeedRating;
        break;
    case SVC_EXIFIFD_WhiteBalance:
        Tag->Value = Image->ExifInfo.ExifIfdInfo.WhiteBalance;
        break;
    case SVC_EXIFIFD_MeteringMode:
        Tag->Value = Image->ExifInfo.ExifIfdInfo.MeteringMode;
        break;
    case SVC_EXIFIFD_LightSource:
        Tag->Value = Image->ExifInfo.ExifIfdInfo.LightSource;
        break;
    case SVC_EXIFIFD_Flash:
        Tag->Value = Image->ExifInfo.ExifIfdInfo.Flash;
        break;
    case SVC_EXIFIFD_ColorSpace:
        Tag->Value = Image->ExifInfo.ExifIfdInfo.ColorSpace;
        break;
    case SVC_EXIFIFD_SensingMethod:
        Tag->Value = Image->ExifInfo.ExifIfdInfo.SensingMethod;
        break;
    case SVC_EXIFIFD_CustomRendered:
        Tag->Value = Image->ExifInfo.ExifIfdInfo.CustomRendered;
        break;
    case SVC_EXIFIFD_FocalLengthIn35mmFilm:
        Tag->Value = Image->ExifInfo.ExifIfdInfo.FocalLength35mmFilm;
        break;
    case SVC_EXIFIFD_SceneCaptureType:
        Tag->Value = Image->ExifInfo.ExifIfdInfo.SceneCaptureType;
        /* tagInfo->ExifIfd[i].Value =Image->ExifInfo.ExifIfdInfo.SceneCaptureType;  for fpxr_scr */
        break;
    case SVC_EXIFIFD_GainControl:
        Tag->Value = Image->ExifInfo.ExifIfdInfo.GainControl;
        break;
    case SVC_EXIFIFD_Contrast:
        Tag->Value = Image->ExifInfo.ExifIfdInfo.Contrast;
        break;
    case SVC_EXIFIFD_Saturation:
        Tag->Value = Image->ExifInfo.ExifIfdInfo.Saturation;
        break;
    case SVC_EXIFIFD_Sharpness:
        Tag->Value = Image->ExifInfo.ExifIfdInfo.Sharpness;
        break;
    case SVC_EXIFIFD_SubjectDistanceRange:
        Tag->Value = Image->ExifInfo.ExifIfdInfo.SubjectDistanceRange;
        break;
    case SVC_EXIFIFD_ExposureMode:
        Tag->Value = Image->ExifInfo.ExifIfdInfo.ExposureMode;
        break;
    default:
        // do nothing
        break;
    }
    Rval = SvcFormat_Put2Byte(Tag, Buffer, HeadSize, Tag->Data);
    return Rval;
}

/**
 * Write ExifIFD (Type : LONG / SLONG)
 *
 * @param [in] Image The image information
 * @param [in] Tag The target tag to be written
 * @param [in] Buffer The tag buffer status
 * @param [in] HeadSize The header size of tags
 * @param [in] Ifd0Tags The numebr of exif IFD0 tags
 * @param [in] ExifIfdTags TThe numebr of exif EXIFIFD tags
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
static UINT32 ExifMux_PutExifIfd_4byte(const SVC_IMAGE_INFO_s *Image, SVC_TIFF_TAG_s *Tag, SVC_TIFF_TAG_BUFFER_s *Buffer, UINT32 HeadSize, UINT32 Ifd0Tags, UINT32 ExifIfdTags)
{
    UINT32 Rval;
    /* LONG (4 bytes) */
    switch (Tag->Tag) {
    case SVC_EXIFIFD_ISOSpeed: /* 2.3 */
        Tag->Value = Image->ExifInfo.ExifIfdInfo.IsoSpeedRating;
        break;
    case SVC_EXIFIFD_PixelXDimension:
        Tag->Value = Image->Frame[0].Width;
        break;
    case SVC_EXIFIFD_PixelYDimension:
        Tag->Value = Image->Frame[0].Height;
        break;
    case SVC_EXIFIFD_InteroperabilityIFDPointer:
        Tag->Value = 8U + 2U + 4U + (Ifd0Tags * 12U) + 2U + 4U + (ExifIfdTags * 12U);
        break;
    default:
        // do nothing
        break;
    }
    Rval = SvcFormat_Put4Byte(Tag, Buffer, HeadSize, Tag->Data);
    return Rval;
}

/**
 * Write ExifIFD (Type : RATIONAL / SRATIONAL)
 *
 * @param [in] Image The image information
 * @param [in] Tag The target tag to be written
 * @param [in] Buffer The tag buffer status
 * @param [in] HeadSize The header size of tags
 * @param [in] FullSize The size of main image
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
static UINT32 ExifMux_PutExifIfd_8byte(const SVC_IMAGE_INFO_s *Image, const SVC_TIFF_TAG_s *Tag, SVC_TIFF_TAG_BUFFER_s *Buffer, UINT32 HeadSize, UINT32 FullSize)
{
    UINT32 Rval;
    UINT8 Data[8];
    /* RATIONAL (8 bytes), SRATIONAL (8 bytes) */
    switch (Tag->Tag) {
    case SVC_EXIFIFD_ExposureTime:
        SvcFormat_Convert64(Data, Image->ExifInfo.ExifIfdInfo.ExposureTimeNum, Image->ExifInfo.ExifIfdInfo.ExposureTimeDen);
        Rval = SvcFormat_Put8Byte(Tag, Buffer, HeadSize, (UINT8 *)Data);
        break;
    case SVC_EXIFIFD_FNumber:
        SvcFormat_Convert64(Data, Image->ExifInfo.ExifIfdInfo.FNumberNum, Image->ExifInfo.ExifIfdInfo.FNumberDen);
        Rval = SvcFormat_Put8Byte(Tag, Buffer, HeadSize, (UINT8 *)Data);
        break;
    case SVC_EXIFIFD_CompressedBitsPerPixel:
        SvcFormat_Convert64(Data, FullSize, ((UINT32)Image->Frame[0].Width >> 3) * (UINT32)Image->Frame[0].Height);
        Rval = SvcFormat_Put8Byte(Tag, Buffer, HeadSize, (UINT8 *)Data);
        break;
    case SVC_EXIFIFD_ShutterSpeedValue:
        SvcFormat_Convert64(Data, Image->ExifInfo.ExifIfdInfo.ShutterSpeedNum, Image->ExifInfo.ExifIfdInfo.ShutterSpeedDen);
        Rval = SvcFormat_Put8Byte(Tag, Buffer, HeadSize, (UINT8 *)Data);
        break;
    case SVC_EXIFIFD_ApertureValue:
        SvcFormat_Convert64(Data, Image->ExifInfo.ExifIfdInfo.ApertureValueNum, Image->ExifInfo.ExifIfdInfo.ApertureValueDen);
        Rval = SvcFormat_Put8Byte(Tag, Buffer, HeadSize, (UINT8 *)Data);
        break;
    case SVC_EXIFIFD_ExposureBiasValue:
        SvcFormat_Convert64(Data, Image->ExifInfo.ExifIfdInfo.ExposureBiasValueNum, Image->ExifInfo.ExifIfdInfo.ExposureBiasValueDen);
        Rval = SvcFormat_Put8Byte(Tag, Buffer, HeadSize, (UINT8 *)Data);
        break;
    case SVC_EXIFIFD_MaxApertureValue:
        SvcFormat_Convert64(Data, Image->ExifInfo.ExifIfdInfo.MaxApertureValueNum, Image->ExifInfo.ExifIfdInfo.MaxApertureValueDen);
        Rval = SvcFormat_Put8Byte(Tag, Buffer, HeadSize, (UINT8 *)Data);
        break;
    case SVC_EXIFIFD_SubjectDistance:
        SvcFormat_Convert64(Data, Image->ExifInfo.ExifIfdInfo.SubjectDistanceNum, Image->ExifInfo.ExifIfdInfo.SubjectDistanceDen);
        Rval = SvcFormat_Put8Byte(Tag, Buffer, HeadSize, (UINT8 *)Data);
        break;
    case SVC_EXIFIFD_FocalLength:
        SvcFormat_Convert64(Data, Image->ExifInfo.ExifIfdInfo.FocalLengthNum, Image->ExifInfo.ExifIfdInfo.FocalLengthDen);
        Rval = SvcFormat_Put8Byte(Tag, Buffer, HeadSize, (UINT8 *)Data);
        break;
    case SVC_EXIFIFD_ExposureIndex:
        SvcFormat_Convert64(Data, Image->ExifInfo.ExifIfdInfo.ExposureIndexNum, Image->ExifInfo.ExifIfdInfo.ExposureIndexDen);
        Rval = SvcFormat_Put8Byte(Tag, Buffer, HeadSize, (UINT8 *)Data);
        break;
    case SVC_EXIFIFD_DigitalZoomRatio:
        SvcFormat_Convert64(Data, Image->ExifInfo.ExifIfdInfo.DigitalZoomRatioNum, Image->ExifInfo.ExifIfdInfo.DigitalZoomRatioDen);
        Rval = SvcFormat_Put8Byte(Tag, Buffer, HeadSize, (UINT8 *)Data);
        break;
    default:
        Rval = SvcFormat_Put8Byte(Tag, Buffer, HeadSize, Tag->Data);
        break;
    }
    return Rval;
}

/**
 * Write Exif ExifIFD
 *
 * @param [in] Image The image information
 * @param [in] Buffer The tag buffer status
 * @param [in] TagInfo The tag information
 * @param [in] FullSize The size of main image
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
static UINT32 ExifMux_PutExifIfd(const SVC_IMAGE_INFO_s *Image, SVC_TIFF_TAG_BUFFER_s *Buffer, SVC_TIFF_TAG_INFO_s *TagInfo, UINT32 FullSize)
{
    UINT32 Rval;
    Rval = SvcFormat_PutMem16(&Buffer->HeadOffset, Buffer->HeadBase, Buffer->HeadSize, TagInfo->ExifIfdTags); /* entries */
    if (Rval == FORMAT_OK) {
        UINT16 i;
        for (i = 0; i < TagInfo->ExifIfdTags; i++) {
            if (TagInfo->ExifIfd[i].Set == SVC_TAG_ENABLED) {
                if ((TagInfo->ExifIfd[i].Type == TIFF_ASCII) || (TagInfo->ExifIfd[i].Type == TIFF_BYTE) || (TagInfo->ExifIfd[i].Type == TIFF_UNDEFINED)) {
                    Rval = ExifMux_PutExifIfd_1byte(Image, &TagInfo->ExifIfd[i], Buffer, TagInfo->HeaderSz, TagInfo->AmbaBoxSz);
                } else if (TagInfo->ExifIfd[i].Type == TIFF_SHORT) {
                    Rval = ExifMux_PutExifIfd_2byte(Image, &TagInfo->ExifIfd[i], Buffer, TagInfo->HeaderSz);
                } else if (TagInfo->ExifIfd[i].Type == TIFF_LONG) {
                    Rval = ExifMux_PutExifIfd_4byte(Image, &TagInfo->ExifIfd[i], Buffer, TagInfo->HeaderSz, TagInfo->Ifd0Tags, TagInfo->ExifIfdTags);
                } else if ((TagInfo->ExifIfd[i].Type == TIFF_RATIONAL) || (TagInfo->ExifIfd[i].Type == TIFF_SRATIONAL)) {
                    Rval = ExifMux_PutExifIfd_8byte(Image, &TagInfo->ExifIfd[i], Buffer, TagInfo->HeaderSz, FullSize);
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Wrong tiff tag!", __func__, NULL, NULL, NULL, NULL);
                    Rval = FORMAT_ERR_GENERAL_ERROR;
                }
            } else if (TagInfo->ExifIfd[i].Set == SVC_TAG_CONFIGURED) {
                if (TagInfo->ExifIfd[i].Tag == SVC_EXIFIFD_MakerNote) {
                    Rval = SvcFormat_PutIfdEntry(&Buffer->HeadOffset, Buffer->HeadBase, Buffer->HeadSize,
                            TagInfo->ExifIfd[i].Tag, TagInfo->ExifIfd[i].Type, TagInfo->ExifIfd[i].Count + TagInfo->AmbaBoxSz, Buffer->InfoOffset + TagInfo->HeaderSz - 12U);
                    if (Rval == FORMAT_OK) {
                        UINT32 Count = TagInfo->ExifIfd[i].Count;
                        if (Count > 4U) {
                            Rval =SvcFormat_WriteMem(&Buffer->InfoOffset, Buffer->InfoBase, Buffer->InfoSize, Count, TagInfo->ExifIfd[i].Data);
                        } else if (Count > 0U) {
                            Rval = SvcFormat_PutMem32(&Buffer->InfoOffset, Buffer->InfoBase, Buffer->InfoSize, TagInfo->ExifIfd[i].Value);
                        } else {
                            // without user defined MakerNote
                        }
                        if (Rval == FORMAT_OK) {
                            Rval = ExifMux_PutAmbaBox(Buffer, TagInfo->AmbaBoxSz);
                        }
                    }
                } else {
                    Rval = ExifMux_PutEntry(&TagInfo->ExifIfd[i], Buffer, TagInfo->HeaderSz);
                }
            } else {
                // SVC_TAG_DISABLED
            }
            if (Rval != FORMAT_OK) {
                AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "ExifMux_PutExifIfd Put(%x) error! Type %u Count %u Set %u", TagInfo->ExifIfd[i].Tag, TagInfo->ExifIfd[i].Type, TagInfo->ExifIfd[i].Count, TagInfo->ExifIfd[i].Set, 0U);
                break;
            }
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_PutMem32(&Buffer->HeadOffset, Buffer->HeadBase, Buffer->HeadSize, 0); /* NextIfdOffset */
            if (Rval == FORMAT_OK) {
                /* IntIFD takes 2 + IntIfdTags*12 + 4 bytes */
                Rval = ExifMux_PutIntIfd(Buffer, TagInfo);
            }
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PutMem16() error!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}



static inline UINT32 ExifMux_PutValue_1byte(const UINT8 *Data, UINT32 Count)
{
    UINT32 i, Value = 0U;
    for(i = 0; i < Count; i++) {
        Value |= ((UINT32)Data[i] << (8U * (3U - i)));
    }
    return Value;
}

static inline void ExifMux_PutData_8byte(UINT8 *Data, const UINT32 *Numerator, const UINT32 *Denominator, UINT32 Count)
{
    UINT32 i;
    for(i = 0; i < Count; i++) {
        SvcFormat_Convert64(&Data[8U * i], Numerator[i], Denominator[i]);
    }
}

static void ExifMux_ConvertIfdOffset(SVC_TIFF_TAG_s *Tag, ULONG Addr)
{
    AmbaMisra_TypeCast(&Tag->Data, &Addr);
    if (Tag->Count < 5U) {
        Tag->Value = ExifMux_PutValue_1byte(Tag->Data, Tag->Count);
    }
}

/**
 * Write GpsIFD (Type : ASCII / BYTE / UNDEFINED)
 *
 * @param [in] GpsInfo The GPS information
 * @param [in] Tag The target tag to be written
 * @param [in] Buffer The tag buffer status
 * @param [in] HeadSize The header size of tags
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
static UINT32 ExifMux_PutGpsIfd_1byte(const SVC_EXIF_GPS_INFO_s *GpsInfo, SVC_TIFF_TAG_s *Tag, SVC_TIFF_TAG_BUFFER_s *Buffer, UINT32 HeadSize)
{
    UINT32 Rval;
    /* ASCII (1 byte), BYTE(1 byte), UNDEFINED(1byte)*/
    switch (Tag->Tag) {
    case SVC_GPSIFD_GPSVersionID:
        Tag->Value = ExifMux_PutValue_1byte(GpsInfo->VersionId, Tag->Count);
        Rval = SvcFormat_Put1Byte(Tag, Buffer, HeadSize, NULL);
        break;
    case SVC_GPSIFD_GPSLatitudeRef:
        Tag->Value = ExifMux_PutValue_1byte(GpsInfo->LatitudeRef, Tag->Count);
        Rval = SvcFormat_Put1Byte(Tag, Buffer, HeadSize, NULL);
        break;
    case SVC_GPSIFD_GPSLongitudeRef:
        Tag->Value = ExifMux_PutValue_1byte(GpsInfo->LongitudeRef, Tag->Count);
        Rval = SvcFormat_Put1Byte(Tag, Buffer, HeadSize, NULL);
        break;
    case SVC_GPSIFD_GPSAltitudeRef:
        Tag->Value = (UINT32)GpsInfo->AltitudeRef << 24U; //ExifMux_PutValue_1byte
        Rval = SvcFormat_Put1Byte(Tag, Buffer, HeadSize, NULL);
        break;
    case SVC_GPSIFD_GPSSatellites:
        ExifMux_ConvertIfdOffset(Tag, GpsInfo->SatelliteOffset);
        Rval = SvcFormat_Put1Byte(Tag, Buffer, HeadSize, Tag->Data);
        break;
    case SVC_GPSIFD_GPSStatus:
        Tag->Value = ExifMux_PutValue_1byte(GpsInfo->Status, Tag->Count);
        Rval = SvcFormat_Put1Byte(Tag, Buffer, HeadSize, NULL);
        break;
    case SVC_GPSIFD_GPSMeasureMode:
        Tag->Value = ExifMux_PutValue_1byte(GpsInfo->MeasureMode, Tag->Count);
        Rval = SvcFormat_Put1Byte(Tag, Buffer, HeadSize, NULL);
        break;
    case SVC_GPSIFD_GPSSpeedRef:
        Tag->Value = ExifMux_PutValue_1byte(GpsInfo->SpeedRef, Tag->Count);
        Rval = SvcFormat_Put1Byte(Tag, Buffer, HeadSize, NULL);
        break;
    case SVC_GPSIFD_GPSTrackRef:
        Tag->Value = ExifMux_PutValue_1byte(GpsInfo->TrackRef, Tag->Count);
        Rval = SvcFormat_Put1Byte(Tag, Buffer, HeadSize, NULL);
        break;
    case SVC_GPSIFD_GPSImgDirectionRef:
        Tag->Value = ExifMux_PutValue_1byte(GpsInfo->ImgdirectionRef, Tag->Count);
        Rval = SvcFormat_Put1Byte(Tag, Buffer, HeadSize, NULL);
        break;
    case SVC_GPSIFD_GPSMapDatum:
        ExifMux_ConvertIfdOffset(Tag, GpsInfo->MapdatumOffset);
        Rval = SvcFormat_Put1Byte(Tag, Buffer, HeadSize, Tag->Data);
        break;
    case SVC_GPSIFD_GPSDestLatitudeRef:
        Tag->Value = ExifMux_PutValue_1byte(GpsInfo->DestlatitudeRef, Tag->Count);
        Rval = SvcFormat_Put1Byte(Tag, Buffer, HeadSize, NULL);
        break;
    case SVC_GPSIFD_GPSDestLongitudeRef:
        Tag->Value = ExifMux_PutValue_1byte(GpsInfo->DestlongitudeRef, Tag->Count);
        Rval = SvcFormat_Put1Byte(Tag, Buffer, HeadSize, NULL);
        break;
    case SVC_GPSIFD_GPSDestBearingRef:
        Tag->Value = ExifMux_PutValue_1byte(GpsInfo->DestbearingRef, Tag->Count);
        Rval = SvcFormat_Put1Byte(Tag, Buffer, HeadSize, NULL);
        break;
    case SVC_GPSIFD_GPSDestDistanceRef:
        Tag->Value = ExifMux_PutValue_1byte(GpsInfo->DestdistanceRef, Tag->Count);
        Rval = SvcFormat_Put1Byte(Tag, Buffer, HeadSize, NULL);
        break;
    case SVC_GPSIFD_GPSProcessingMethod:
        ExifMux_ConvertIfdOffset(Tag, GpsInfo->ProcessingmethodOffset);
        Rval = SvcFormat_Put1Byte(Tag, Buffer, HeadSize, Tag->Data);
        break;
    case SVC_GPSIFD_GPSAreaInformation:
        ExifMux_ConvertIfdOffset(Tag, GpsInfo->AreainformationOffset);
        Rval = SvcFormat_Put1Byte(Tag, Buffer, HeadSize, Tag->Data);
        break;
    case SVC_GPSIFD_GPSDateStamp:
        Rval = SvcFormat_Put1Byte(Tag, Buffer, HeadSize, GpsInfo->Datestamp);
        break;
    default:
        Rval = SvcFormat_Put1Byte(Tag, Buffer, HeadSize, Tag->Data);
        break;
    }
    return Rval;
}

/**
 * Write GpsIFD (Type : SHORT)
 *
 * @param [in] GpsInfo The GPS information
 * @param [in] Tag The target tag to be written
 * @param [in] Buffer The tag buffer status
 * @param [in] HeadSize The header size of tags
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
static UINT32 ExifMux_PutGpsIfd_2byte(const SVC_EXIF_GPS_INFO_s *GpsInfo, SVC_TIFF_TAG_s *Tag, SVC_TIFF_TAG_BUFFER_s *Buffer, UINT32 HeadSize)
{
    UINT32 Rval;
    /* SHORT (2 bytes) */
    switch (Tag->Tag) {
    case SVC_GPSIFD_GPSDifferential:
        Tag->Value = (UINT32)GpsInfo->Differential;
        break;
    default:
        // do nothing
        break;
    }
    Rval = SvcFormat_Put2Byte(Tag, Buffer, HeadSize, Tag->Data);
    return Rval;
}

/**
 * Write GpsIFD (Type : LONG / SLONG)
 *
 * @param [in] Tag The target tag to be written
 * @param [in] Buffer The tag buffer status
 * @param [in] HeadSize The header size of tags
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
static UINT32 ExifMux_PutGpsIfd_4byte(const SVC_TIFF_TAG_s *Tag, SVC_TIFF_TAG_BUFFER_s *Buffer, UINT32 HeadSize)
{
    UINT32 Rval;
    Rval = SvcFormat_Put4Byte(Tag, Buffer, HeadSize, Tag->Data);
    return Rval;
}

/**
 * Write GpsIFD (Type : RATIONAL / SRATIONAL)
 *
 * @param [in] GpsInfo The GPS information
 * @param [in] Tag The target tag to be written
 * @param [in] Buffer The tag buffer status
 * @param [in] HeadSize The header size of tags
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
static UINT32 ExifMux_PutGpsIfd_8byte(const SVC_EXIF_GPS_INFO_s *GpsInfo, const SVC_TIFF_TAG_s *Tag, SVC_TIFF_TAG_BUFFER_s *Buffer, UINT32 HeadSize)
{
    UINT32 Rval;
    UINT8 Data[24] = {0U};
    /* RATIONAL (8 bytes), SRATIONAL (8 bytes) */
    switch (Tag->Tag) {
    case SVC_GPSIFD_GPSLatitude:
        ExifMux_PutData_8byte(Data, GpsInfo->LatitudeNum, GpsInfo->LatitudeDen, Tag->Count);
        Rval = SvcFormat_Put8Byte(Tag, Buffer, HeadSize, Data);
        break;
    case SVC_GPSIFD_GPSLongitude:
        ExifMux_PutData_8byte(Data, GpsInfo->LongitudeNum, GpsInfo->LongitudeDen, Tag->Count);
        Rval = SvcFormat_Put8Byte(Tag, Buffer, HeadSize, Data);
        break;
    case SVC_GPSIFD_GPSAltitude:
        SvcFormat_Convert64(Data, GpsInfo->AltitudeNum, GpsInfo->AltitudeDen);
        Rval = SvcFormat_Put8Byte(Tag, Buffer, HeadSize, Data);
        break;
    case SVC_GPSIFD_GPSTimeStamp:
        ExifMux_PutData_8byte(Data, GpsInfo->TimestampNum, GpsInfo->TimestampDen, Tag->Count);
        Rval = SvcFormat_Put8Byte(Tag, Buffer, HeadSize, Data);
        break;
    case SVC_GPSIFD_GPSDOP:
        SvcFormat_Convert64(Data, GpsInfo->DopNum, GpsInfo->DopDen);
        Rval = SvcFormat_Put8Byte(Tag, Buffer, HeadSize, Data);
        break;
    case SVC_GPSIFD_GPSSpeed:
        SvcFormat_Convert64(Data, GpsInfo->SpeedNum, GpsInfo->SpeedDen);
        Rval = SvcFormat_Put8Byte(Tag, Buffer, HeadSize, Data);
        break;
    case SVC_GPSIFD_GPSTrack:
        SvcFormat_Convert64(Data, GpsInfo->TrackNum, GpsInfo->TrackDen);
        Rval = SvcFormat_Put8Byte(Tag, Buffer, HeadSize, Data);
        break;
    case SVC_GPSIFD_GPSImgDirection:
        SvcFormat_Convert64(Data, GpsInfo->ImgdirectionNum, GpsInfo->ImgdirectionDen);
        Rval = SvcFormat_Put8Byte(Tag, Buffer, HeadSize, Data);
        break;
    case SVC_GPSIFD_GPSDestLatitude:
        ExifMux_PutData_8byte(Data, GpsInfo->DestlatitudeNum, GpsInfo->DestlatitudeDen, Tag->Count);
        Rval = SvcFormat_Put8Byte(Tag, Buffer, HeadSize, Data);
        break;
    case SVC_GPSIFD_GPSDestLongitude:
        ExifMux_PutData_8byte(Data, GpsInfo->DestlongitudeNum, GpsInfo->DestlongitudeDen, Tag->Count);
        Rval = SvcFormat_Put8Byte(Tag, Buffer, HeadSize, Data);
        break;
    case SVC_GPSIFD_GPSDestBearing:
        SvcFormat_Convert64(Data, GpsInfo->DestbearingNum, GpsInfo->DestbearingDen);
        Rval = SvcFormat_Put8Byte(Tag, Buffer, HeadSize, Data);
        break;
    case SVC_GPSIFD_GPSDestDistance:
        SvcFormat_Convert64(Data, GpsInfo->DestdistanceNum, GpsInfo->DestdistanceDen);
        Rval = SvcFormat_Put8Byte(Tag, Buffer, HeadSize, Data);
        break;
    case SVC_GPSIFD_GPSHPositioningError:
        SvcFormat_Convert64(Data, GpsInfo->HPositioningErrorNum, GpsInfo->HPositioningErrorDen);
        Rval = SvcFormat_Put8Byte(Tag, Buffer, HeadSize, Data);
        break;
    default:
        Rval = SvcFormat_Put8Byte(Tag, Buffer, HeadSize, Tag->Data);
        break;
    }
    return Rval;
}

/**
 * Write Exif GpsIfd
 *
 * @param [in] GpsInfo The GPS information
 * @param [in] Buffer The tag buffer status
 * @param [in] TagInfo The tag information
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
static UINT32 ExifMux_PutGpsIfd(const SVC_EXIF_GPS_INFO_s *GpsInfo, SVC_TIFF_TAG_BUFFER_s *Buffer, SVC_TIFF_TAG_INFO_s *TagInfo)
{
    UINT32 Rval = SvcFormat_PutMem16(&Buffer->HeadOffset, Buffer->HeadBase, Buffer->HeadSize, TagInfo->GpsIfdTags);/* entries */
    if (Rval == FORMAT_OK) {
        UINT16 i;
        for (i = 0; i < TagInfo->GpsIfdTags; i++) {
            if (TagInfo->GpsIfd[i].Set == SVC_TAG_ENABLED) {
                if ((TagInfo->GpsIfd[i].Type == TIFF_ASCII) || (TagInfo->GpsIfd[i].Type == TIFF_BYTE) || (TagInfo->GpsIfd[i].Type == TIFF_UNDEFINED)) {
                    Rval = ExifMux_PutGpsIfd_1byte(GpsInfo, &TagInfo->GpsIfd[i], Buffer, TagInfo->HeaderSz);
                } else if (TagInfo->GpsIfd[i].Type == TIFF_SHORT) {
                    Rval = ExifMux_PutGpsIfd_2byte(GpsInfo, &TagInfo->GpsIfd[i], Buffer, TagInfo->HeaderSz);
                } else if (TagInfo->GpsIfd[i].Type == TIFF_LONG) {
                    Rval = ExifMux_PutGpsIfd_4byte(&TagInfo->GpsIfd[i], Buffer, TagInfo->HeaderSz);
                } else if ((TagInfo->GpsIfd[i].Type == TIFF_RATIONAL) || (TagInfo->GpsIfd[i].Type == TIFF_SRATIONAL)) {
                    Rval = ExifMux_PutGpsIfd_8byte(GpsInfo, &TagInfo->GpsIfd[i], Buffer, TagInfo->HeaderSz);
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Wrong tiff tag!", __func__, NULL, NULL, NULL, NULL);
                    Rval = FORMAT_ERR_GENERAL_ERROR;
                }
            } else if (TagInfo->GpsIfd[i].Set == SVC_TAG_CONFIGURED) {
                Rval = ExifMux_PutEntry(&TagInfo->GpsIfd[i], Buffer, TagInfo->HeaderSz);
            } else {
                //SVC_TAG_ENABLED
            }
            if (Rval != FORMAT_OK) {
                AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "ExifMux_PutGpsIfd Put(%x) error! Type %u Count %u Set %u", TagInfo->GpsIfd[i].Tag, TagInfo->GpsIfd[i].Type, TagInfo->GpsIfd[i].Count, TagInfo->GpsIfd[i].Set, 0U);
                break;
            }
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_PutMem32(&Buffer->HeadOffset, Buffer->HeadBase, Buffer->HeadSize, 0); /* NextIfdOffset */
            if (Rval != FORMAT_OK) {
                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PutMem32() error!", __func__, NULL, NULL, NULL, NULL);
            }
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PutMem16() error!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 * Write Exif Ifd1, the tags are available when there are thumbnail image
 *
 * @param [in] Image The image information
 * @param [in] Buffer The tag buffer status
 * @param [in] TagInfo The tag information
 * @param [in] TbSize The size of thumbnail
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
static UINT32 ExifMux_PutIfd1(const SVC_IMAGE_INFO_s *Image, SVC_TIFF_TAG_BUFFER_s *Buffer, SVC_TIFF_TAG_INFO_s *TagInfo, UINT32 TbSize)
{
    UINT32 Rval = SvcFormat_PutMem16(&Buffer->HeadOffset, Buffer->HeadBase, Buffer->HeadSize, TagInfo->Ifd1Tags);/* entries */
    if (Rval == FORMAT_OK) {
        UINT16 i;
        for (i=0; i<TagInfo->Ifd1Tags; i++) {
            if (TagInfo->Ifd1[i].Set == SVC_TAG_ENABLED) {
                switch (TagInfo->Ifd1[i].Tag) {
                case SVC_TIFF_XResolution:
                case SVC_TIFF_YResolution:
                    /* RATIONAL (8 bytes), always more than 4 bytes, Use default value "72, 1" */
                    {
                        UINT8 Resolution[8];
                        SvcFormat_Convert64((UINT8 *)Resolution, 72U, 1U);
                        Rval = SvcFormat_Put8Byte(&TagInfo->Ifd1[i], Buffer, TagInfo->HeaderSz, (UINT8 *)Resolution);
                    }
                    break;
                case SVC_TIFF_ImageWidth:
                    /* LONG (4 bytes). Not adjustable now*/
                    TagInfo->Ifd1[i].Value = Image->Frame[1].Width;
                    Rval = SvcFormat_Put4Byte(&TagInfo->Ifd1[i], Buffer, TagInfo->HeaderSz, NULL);
                    break;
                case SVC_TIFF_ImageHeight:
                    /* LONG (4 bytes). Not adjustable now*/
                    TagInfo->Ifd1[i].Value = Image->Frame[1].Height;
                    Rval = SvcFormat_Put4Byte(&TagInfo->Ifd1[i], Buffer, TagInfo->HeaderSz, NULL);
                    break;
                case SVC_TIFF_DateTime:
                    /* ASCII (1 byte)*/
                    Rval = SvcFormat_Put1Byte(&TagInfo->Ifd1[i], Buffer, TagInfo->HeaderSz, (const UINT8 *)Image->MediaInfo.Date);
                    break;
                case SVC_TIFF_JPEGInterchangeFormat:
                    /* LONG (4 bytes), thumbnail offset.  Not adjustable. */
                    TagInfo->Ifd1[i].Value = TagInfo->HeaderSz + TagInfo->InfoSz-12U;
                    Rval = SvcFormat_Put4Byte(&TagInfo->Ifd1[i], Buffer, TagInfo->HeaderSz, NULL);
                    break;
                case SVC_TIFF_JPEGInterchangeFormatLength:
                    /* LONG (4 bytes), thumbnail offset. Not adjustable. */
                    TagInfo->Ifd1[i].Value = TbSize;
                    Rval = SvcFormat_Put4Byte(&TagInfo->Ifd1[i], Buffer, TagInfo->HeaderSz, NULL);
                    break;
                default:
                    Rval = ExifMux_PutEntry(&TagInfo->Ifd1[i], Buffer, TagInfo->HeaderSz);
                    break;
                }
            } else if (TagInfo->Ifd1[i].Set == SVC_TAG_CONFIGURED) {
                Rval = ExifMux_PutEntry(&TagInfo->Ifd1[i], Buffer, TagInfo->HeaderSz);
            } else {
                //SVC_TAG_ENABLED
            }
            if (Rval != FORMAT_OK) {
                AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "ExifMux_PutIfd1 Put(%x) error! Type %u Count %u", TagInfo->Ifd1[i].Tag, TagInfo->Ifd1[i].Type, TagInfo->Ifd1[i].Count, 0U, 0U);
                break;
            }
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_PutMem32(&Buffer->HeadOffset, Buffer->HeadBase, Buffer->HeadSize, 0); /* NextIfdOffset */
            if (Rval != FORMAT_OK) {
                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PutMem32() error!", __func__, NULL, NULL, NULL, NULL);
            }
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PutMem16() error!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 * Write Exif tiff header
 *
 * @param [in] Buffer The tag buffer status
 * @param [in] TagInfo The tag information
 * @param [in] TbSize The size of thumbnail
 * @param [in] Endian The endian order of tags
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
static UINT32 ExifMux_PutTiffHead(SVC_TIFF_TAG_BUFFER_s *Buffer, const SVC_TIFF_TAG_INFO_s *TagInfo, UINT32 TbSize, UINT8 Endian)
{
    /* App0 - JFIF takes 18 bytes use DSP's JFIF*/
    UINT32 Rval = SvcFormat_PutMemBe16(&Buffer->HeadOffset, Buffer->HeadBase, Buffer->HeadSize, Marker_SOI);
    if(Rval == FORMAT_OK) {
        /* App1 - EXIF start */
        Rval = SvcFormat_PutMemBe16(&Buffer->HeadOffset, Buffer->HeadBase, Buffer->HeadSize, Marker_APP1);
        if (Rval == FORMAT_OK) {
            /* App1 Size, need modify in new structure. */
            Rval = SvcFormat_PutMemBe16(&Buffer->HeadOffset, Buffer->HeadBase, Buffer->HeadSize, (UINT16)TagInfo->HeaderSz + (UINT16)TagInfo->InfoSz + (UINT16)TbSize - (UINT16)4U);
            if (Rval == FORMAT_OK) {
                Rval = SvcFormat_PutMemBe32(&Buffer->HeadOffset, Buffer->HeadBase, Buffer->HeadSize, Marker_Exif);
                if (Rval == FORMAT_OK) {
                    Rval = SvcFormat_PutMemBe16(&Buffer->HeadOffset, Buffer->HeadBase, Buffer->HeadSize, 0U);
                    if (Rval == FORMAT_OK) {  /* TIFF exif_head takes 8 bytes */
                        UINT16 Marker = (Endian == SVC_FORMAT_EXIF_BIGENDIAN)? Marker_MM : Marker_II;
                        Rval = SvcFormat_PutMemBe16(&Buffer->HeadOffset, Buffer->HeadBase, Buffer->HeadSize, Marker);
                        if (Rval == FORMAT_OK) {
                            Rval = SvcFormat_PutMem16(&Buffer->HeadOffset, Buffer->HeadBase, Buffer->HeadSize, 42U);
                            if (Rval == FORMAT_OK) {
                                Rval = SvcFormat_PutMem32(&Buffer->HeadOffset, Buffer->HeadBase, Buffer->HeadSize, 8U);
                                if (Rval != FORMAT_OK) {
                                    AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PutMem32 error!", __func__, NULL, NULL, NULL, NULL);
                                }
                            } else {
                                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PutMem16 error!", __func__, NULL, NULL, NULL, NULL);
                            }
                        } else {
                            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PutMemBe16 error!", __func__, NULL, NULL, NULL, NULL);
                        }
                    } else {
                        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PutMemBe16 error!", __func__, NULL, NULL, NULL, NULL);
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PutMemBe32 error!", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PutMemBe16 error!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PutMemBe16 error!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PutMemBe16 error!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 * Write exif header
 *
 * @param [in] Hdlr The exif mux handler
 * @param [in] Image The image information
 * @param [in] TbSize The size of the thumbnail
 * @param [in] FullSize The size of the main image
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
static UINT32 ExifMux_PutHead(SVC_EXIF_MUX_HDLR_s *Hdlr, const SVC_IMAGE_INFO_s *Image, UINT32 TbSize, UINT32 FullSize)
{
    SVC_TIFF_TAG_INFO_s * const TagInfo = &Hdlr->TagInfo;
    SVC_TIFF_TAG_BUFFER_s * const Buffer = &Hdlr->TagBuffer;
    UINT32 Rval = ExifMux_PutTiffHead(Buffer, TagInfo, TbSize, Hdlr->Endian);
    if (Rval == FORMAT_OK) {
        /* IDF0 starts at 8, and takes 2 + 9*12 + 4 bytes */
        Rval = ExifMux_PutIfd0(Image, Buffer, TagInfo);
        if (Rval == FORMAT_OK) {
            UINT32 nextIfdoffet = 0U; /* No next IFD Offset (IDF1) */
            if (TbSize != 0U) { /* Next IFD Offset (IDF1) */
                nextIfdoffet = 8U + (2U + ((UINT32)TagInfo->Ifd0Tags * 12U) + 4U) + (2U + ((UINT32)TagInfo->ExifIfdTags * 12U) + 4U) + (2U + ((UINT32)TagInfo->GpsIfdTags * 12U) + 4U) + (2U + ((UINT32)TagInfo->IntIfdTags * 12U) +4U);
            }
            Rval = SvcFormat_PutMem32(&Buffer->HeadOffset, Buffer->HeadBase, Buffer->HeadSize, nextIfdoffet);
            if (Rval == FORMAT_OK) {
                /* EXIF takes 2 + 15*12 + 4 bytes */
                Rval = ExifMux_PutExifIfd(Image, Buffer, TagInfo, FullSize);
                if (Rval == FORMAT_OK) {
                    Rval = ExifMux_PutGpsIfd(&Image->ExifInfo.GpsIfdInfo, Buffer, TagInfo);
                    if (Rval == FORMAT_OK) {
                        if (TbSize != 0U) {
                            /* IDF1 takes 2 + 8*12 + 4 bytes */
                            Rval = ExifMux_PutIfd1(Image, Buffer, TagInfo, TbSize);
                        }
                        if (Rval == FORMAT_OK) {
                            SVC_STREAM_HDLR_s * const Stream  = Hdlr->Hdlr.Stream;
                            Rval = SvcFormat_WriteStream(Stream, TagInfo->HeaderSz, 0U, Buffer->HeadBase, Buffer->HeadSize);
                            if (Rval == FORMAT_OK) {
                                Rval = SvcFormat_WriteStream(Stream, TagInfo->InfoSz, 0U, Buffer->InfoBase, Buffer->InfoSize);
                                if (Rval != FORMAT_OK) {
                                    AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Write data to stream failed!", __func__, NULL, NULL, NULL, NULL);
                                }
                            } else {
                                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Write data to stream failed!", __func__, NULL, NULL, NULL, NULL);
                            }
                        } else {
                            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s ExifMux_PutIfd1() error!", __func__, NULL, NULL, NULL, NULL);
                        }
                    } else {
                        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s ExifMux_PutGpsIfd() error!", __func__, NULL, NULL, NULL, NULL);
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s ExifMux_PutExifIfd() error!", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PutMemBe16() error!", __func__, NULL, NULL, NULL, NULL);
            }
        }
    }
    return Rval;
}

/**
 * Write screen-nail
 *
 * @param [in] Hdlr The exif mux handler
 * @param [in] Image The image information
 * @param [in] Size The size of the screen-nail
 * @param [in] FramePos The position of the screen-nail
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
static UINT32 ExifMux_PutApp2Scr(const SVC_EXIF_MUX_HDLR_s *Hdlr, const SVC_IMAGE_INFO_s *Image, UINT32 Size, UINT32 FramePos)
{
    UINT32 Rval = FORMAT_OK;
    SVC_STREAM_HDLR_s * const Stream  = Hdlr->Hdlr.Stream;
    UINT32 i, App2Num, TempSize = 0U, App2MaxSize = 65535U - 2U;/* (max: 65535 - 2 header_Size) */
    App2Num = (Size / App2MaxSize);
    if ((Size % App2MaxSize) != 0U) {
        App2Num += 1U;
    }
    for (i = 0U; i < App2Num; i++) {
        UINT32 App2Size = App2MaxSize;
        if (((Size % App2MaxSize) != 0U) && (i == (App2Num - 1U))) {
            App2Size = Size % App2MaxSize;
        }
        Rval = SvcFormat_PutBe16(Stream, 0xFFE2U); /* App2, for screennail : tag */
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_PutBe16(Stream, (UINT16)App2Size + 2U); /* App2, for screennail : size */
            if (Rval == FORMAT_OK) {
                UINT32 TempPos = FramePos;
                Rval = SvcFormat_PlusAddr(TempSize, FramePos, Image->BufferSize, &TempPos);
                if (Rval != FORMAT_OK) {
                    AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PlusAddr() error!", __func__, NULL, NULL, NULL, NULL);
                } else {
                    Rval = SvcFormat_WriteStream(Stream, App2Size, TempPos, Image->BufferBase, Image->BufferSize);
                    if (Rval == FORMAT_OK) {
                        TempSize += App2Size;
                    } else {
                        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Write data to stream failed!", __func__, NULL, NULL, NULL, NULL);
                    }
                }
            } else {
                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PutBe16() error!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PutBe16() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval != FORMAT_OK) {
            break;
        }
    }
    return Rval;
}

/**
 * Get target frame from fifo
 *
 * @param [in] Image The image information
 * @param [in] Type The frame type of the image
 * @param [in,out] Frame The target frame
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
static UINT32 ExifMux_GetFrame(const SVC_IMAGE_INFO_s *Image, UINT8 Type, SVC_FIFO_BITS_DESC_s **Frame)
{
    UINT32 Rval = FORMAT_OK;
    UINT8 i;
    *Frame = NULL;
    for (i = 0U; i < Image->FrameCount; i++) {
        SVC_FIFO_BITS_DESC_s *Temp;
        Rval = F2F(SvcFIFO_PeekEntry(Image->Fifo, &Temp, i));
        if (Rval == FORMAT_OK) {
            if (Temp->Size > 0U) {
                if (Temp->FrameType == Type) {
                    /* frame found */
                    *Frame = Temp;
                }
            } else {
                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Get frame error!", __func__, NULL, NULL, NULL, NULL);
                Rval = FORMAT_ERR_GENERAL_ERROR;
            }
        } else {
            AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "ExifMux_GetFrame Peek failed! %u", Rval, 0U, 0U, 0U, 0U);
        }
        if ((Rval != FORMAT_OK) || (*Frame != NULL)) {
            break;
        }
    }
    if ((Rval == FORMAT_OK) && (i == Image->FrameCount)) {
        /* frame not found */
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Get frame error!", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_GENERAL_ERROR;
    }
    return Rval;
}

/**
 * Write DQT table
 *
 * @param [in] Stream The handler of stream
 * @param [in] FramePos The position of frame
 * @param [in] BufferBase The base address of buffer
 * @param [in] BufferSize The size of buffer
 * @param [in] Width The with of the frame
 * @param [in] Height The height of the frame
 * @param [out] NewFramePos The resulted position
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
static UINT32 ExifMux_WriteDQT(SVC_STREAM_HDLR_s *Stream, UINT32 FramePos, UINT8 *BufferBase, UINT32 BufferSize, UINT16 Width, UINT16 Height, UINT32 *NewFramePos)
{
    UINT32 Rval;
    UINT32 CurrPos = FramePos;
    /* DQT1 head , DQT size = 0x0084 */
    Rval = SvcFormat_PutBe16(Stream, Marker_DQT);
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe16(Stream, 0x0084U);
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_PlusAddr(0x18, CurrPos, BufferSize, &CurrPos);
            if (Rval != FORMAT_OK) {
                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PlusAddr() error!", __func__, NULL, NULL, NULL, NULL);
            } else {
                /* DQT1 data */
                Rval = SvcFormat_WriteStream(Stream, 0x41, CurrPos, BufferBase, BufferSize);
                if (Rval == FORMAT_OK) {
                    Rval = SvcFormat_PlusAddr(0x41 + 0x04, CurrPos, BufferSize, &CurrPos);
                    if (Rval != FORMAT_OK) {
                        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PlusAddr() error!", __func__, NULL, NULL, NULL, NULL);
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_WriteStream() error!", __func__, NULL, NULL, NULL, NULL);
                }
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PutBe16() error!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PutBe16() error!", __func__, NULL, NULL, NULL, NULL);
    }

    if (Rval == FORMAT_OK) {
        /* DQT2 data + SOF + DHT head */
        if ((Width != 0U) && (Height != 0U)) {
            Rval = SvcFormat_WriteStream(Stream, 0x41 + 0x5, CurrPos, BufferBase, BufferSize);
            if (Rval == FORMAT_OK) {
                Rval = SvcFormat_PlusAddr(0x41 + 0x09, CurrPos, BufferSize, &CurrPos);
                if (Rval != FORMAT_OK) {
                    AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PlusAddr() error!", __func__, NULL, NULL, NULL, NULL);
                } else {
                    /* Replace SOF Width and Height */
                    Rval = SvcFormat_PutBe16(Stream, Height);
                    if (Rval == FORMAT_OK) {
                        Rval = SvcFormat_PutBe16(Stream, Width);
                        if (Rval == FORMAT_OK) {
                            Rval = SvcFormat_WriteStream(Stream, 0xC, CurrPos, BufferBase, BufferSize);
                            if (Rval == FORMAT_OK) {
                                Rval = SvcFormat_PlusAddr(0xC + 0x2, CurrPos, BufferSize, &CurrPos);
                                if (Rval != FORMAT_OK) {
                                    AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PlusAddr() error!", __func__, NULL, NULL, NULL, NULL);
                                }
                            } else {
                                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_WriteStream() error!", __func__, NULL, NULL, NULL, NULL);
                            }
                        } else {
                            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PutBe16() error!", __func__, NULL, NULL, NULL, NULL);
                        }
                    } else {
                        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PutBe16() error!", __func__, NULL, NULL, NULL, NULL);
                    }
                }
            } else {
                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_WriteStream() error!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            Rval = SvcFormat_WriteStream(Stream, 0x41U + 0x15U, CurrPos, BufferBase, BufferSize);
            if (Rval == FORMAT_OK) {
                Rval = SvcFormat_PlusAddr(0x41U + 0x17U, CurrPos, BufferSize, &CurrPos);
                if (Rval != FORMAT_OK) {
                    AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PlusAddr() error!", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_WriteStream() error!", __func__, NULL, NULL, NULL, NULL);
            }
        }
    }
    if (Rval == FORMAT_OK) {
        *NewFramePos = CurrPos;
    }
    return Rval;
}

/**
 * Write DHT table
 *
 * @param [in] Stream The handler of stream
 * @param [in] FramePos The position of frame
 * @param [in] BufferBase The base address of buffer
 * @param [in] BufferSize The size of buffer
 * @param [in] SkipDRI The flag to skip DRI
 * @param [out] NewFramePos The resulted position
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
static UINT32 ExifMux_WriteDHT(SVC_STREAM_HDLR_s *Stream, UINT32 FramePos, UINT8 *BufferBase, UINT32 BufferSize, UINT8 SkipDRI, UINT32 *NewFramePos)
{
    UINT32 Rval;
    UINT32 CurrPos = FramePos;
    /* DHT size = 0x01A2 */
    Rval = SvcFormat_PutBe16(Stream, 0x01A2U);
    if(Rval == FORMAT_OK) {
        /* DHT1 data (0x1D) */
        Rval = SvcFormat_WriteStream(Stream, 0x1DU, CurrPos, BufferBase, BufferSize);
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_PlusAddr(0x1DU + 0x4U, CurrPos, BufferSize, &CurrPos);
            if (Rval != FORMAT_OK) {
                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PlusAddr() error!", __func__, NULL, NULL, NULL, NULL);
            } else {
                /* DHT2 data (0xB3) */
                Rval = SvcFormat_WriteStream(Stream, 0xB3U,  CurrPos, BufferBase, BufferSize);
                if (Rval == FORMAT_OK) {
                    Rval = SvcFormat_PlusAddr(0xB3U + 0x4U, CurrPos, BufferSize, &CurrPos);
                    if (Rval != FORMAT_OK) {
                        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PlusAddr() error!", __func__, NULL, NULL, NULL, NULL);
                    } else {
                        /* DHT3 data (0x1D) */
                        Rval = SvcFormat_WriteStream(Stream, 0x1DU, CurrPos, BufferBase, BufferSize);
                        if (Rval == FORMAT_OK) {
                            Rval = SvcFormat_PlusAddr(0x1DU + 0x4U, CurrPos, BufferSize, &CurrPos);
                            if (Rval != FORMAT_OK) {
                                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PlusAddr() error!", __func__, NULL, NULL, NULL, NULL);
                            } else {
                                /* DHT4 data (0xB3) */
                                Rval = SvcFormat_WriteStream(Stream, 0xB3U, CurrPos, BufferBase, BufferSize);
                                if (Rval == FORMAT_OK) {
                                    Rval = SvcFormat_PlusAddr(0xB3U + (UINT32)SkipDRI, CurrPos, BufferSize, &CurrPos);
                                    if (Rval != FORMAT_OK) {
                                        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PlusAddr() error!", __func__, NULL, NULL, NULL, NULL);
                                    }
                                } else {
                                    AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_WriteStream() error!", __func__, NULL, NULL, NULL, NULL);
                                }
                            }
                        } else {
                            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_WriteStream() error!", __func__, NULL, NULL, NULL, NULL);
                        }
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_WriteStream() error!", __func__, NULL, NULL, NULL, NULL);
                }
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_WriteStream() error!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PutBe16() error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        *NewFramePos = CurrPos;
    }
    return Rval;
}

/**
 * Write image frame
 *
 * @param [in] Hdlr The exif handler
 * @param [in] Image The image information
 * @param [in] Size The size of the frame
 * @param [in] FramePos The position of the frame
 * @param [in] SkipDRI The flag of skip DRI
 * @param [in] Width The with of the frame
 * @param [in] Height The height of the frame
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
static UINT32 ExifMux_WriteImage(const SVC_EXIF_MUX_HDLR_s *Hdlr, const SVC_IMAGE_INFO_s *Image, UINT32 Size, UINT32 FramePos, UINT8 SkipDRI, UINT16 Width, UINT16 Height)
{
    SVC_STREAM_HDLR_s * const Stream  = Hdlr->Hdlr.Stream;
    UINT32 CurrPos = FramePos;
    UINT32 Rval = ExifMux_WriteDQT(Stream, CurrPos, Image->BufferBase, Image->BufferSize, Width, Height, &CurrPos);
    if (Rval == FORMAT_OK) {
        Rval = ExifMux_WriteDHT(Stream, CurrPos, Image->BufferBase, Image->BufferSize, SkipDRI, &CurrPos);
        if (Rval == FORMAT_OK) {
            /* SOS + EOI */
            Rval = SvcFormat_WriteStream(Stream, Size - 0x261U - SkipDRI, CurrPos, Image->BufferBase, Image->BufferSize);
            if (Rval != FORMAT_OK) {
                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_WriteStream() error!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s ExifMux_WriteDHT() error!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s ExifMux_WriteDQT() error!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 SvcExifMux_Open_ParamCheck(const SVC_MUX_FORMAT_HDLR_s *Hdlr)
{
    UINT32 Rval = MUXER_ERR_INVALID_ARG;
    if (Hdlr != NULL) {
        Rval = MUXER_OK;
    }
    return Rval;
}

/**
 * Open Exif mux handler.
 * @param [in] Hdlr The handler of exif mux to open.
 * @return 0 - OK, others - MUXER_ERR_XXXX
 */
static UINT32 SvcExifMux_Open(SVC_MUX_FORMAT_HDLR_s *Hdlr)
{
    UINT32 Rval;
    Rval = SvcExifMux_Open_ParamCheck(Hdlr);
    if (Rval == MUXER_OK) {
        if (Hdlr->Media->MediaType == SVC_MEDIA_INFO_IMAGE) {
            SVC_EXIF_MUX_HDLR_s *ExifMux;
            AmbaMisra_TypeCast(&ExifMux, &Hdlr);
            Rval = K2M(AmbaKAL_MutexTake(&ExifMux->Mutex, SVC_FORMAT_WAIT_TIMEOUT));
            if (Rval == MUXER_OK) {
                if (ExifMux->Open == 0U) {
                    SvcFormat_SetTiffHeader(ExifMux->Endian);
                    ExifMux->Open = 1U;
                    /* tiff_get_time(Image->Date); */
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Already opened!", __func__, NULL, NULL, NULL, NULL);
                    Rval = MUXER_ERR_FATAL_ERROR;
                }
                if (AmbaKAL_MutexGive(&ExifMux->Mutex) != OK) {
                    AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Give mutex fail!", __func__, NULL, NULL, NULL, NULL);
                    Rval = MUXER_ERR_FATAL_ERROR;
                }
            } else {
                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Take mutex fail!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Not a image!", __func__, NULL, NULL, NULL, NULL);
            Rval = MUXER_ERR_FATAL_ERROR;
        }
        AmbaMisra_TouchUnused(Hdlr);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Invalid agruement!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 SvcExifMux_Close_ParamCheck(const SVC_MUX_FORMAT_HDLR_s *Hdlr)
{
    UINT32 Rval = MUXER_ERR_INVALID_ARG;
    if (Hdlr != NULL) {
        Rval = MUXER_OK;
    }
    return Rval;
}

/**
 * Close Exif mux handler.
 * @param [in] Hdlr The handler of exif mux to close.
 * @param [in] Mode Not used, for Movie instance.
 * @return 0 - OK, others - MUXER_ERR_XXXX
 */
static UINT32 SvcExifMux_Close(SVC_MUX_FORMAT_HDLR_s *Hdlr, UINT8 Mode)
{
    UINT32 Rval;
    Rval = SvcExifMux_Close_ParamCheck(Hdlr);
    if (Rval == MUXER_OK) {
        if (Hdlr->Media->MediaType == SVC_MEDIA_INFO_IMAGE) {
            SVC_EXIF_MUX_HDLR_s *ExifMux;
            AmbaMisra_TypeCast(&ExifMux, &Hdlr);
            Rval = K2M(AmbaKAL_MutexTake(&ExifMux->Mutex, SVC_FORMAT_WAIT_TIMEOUT));
            if (Rval == MUXER_OK) {
                if (ExifMux->Open == 1U) {
                    ExifMux->Open = 0U;
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Not open yet!", __func__, NULL, NULL, NULL, NULL);
                    Rval = MUXER_ERR_FATAL_ERROR;
                }
                if (AmbaKAL_MutexGive(&ExifMux->Mutex) != OK) {
                    AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Give mutex fail!", __func__, NULL, NULL, NULL, NULL);
                    Rval = MUXER_ERR_FATAL_ERROR;
                }
            } else {
                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Take mutex fail!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Not a image!", __func__, NULL, NULL, NULL, NULL);
            Rval = MUXER_ERR_FATAL_ERROR;
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Invalid agruement!", __func__, NULL, NULL, NULL, NULL);
    }
    AmbaMisra_TouchUnused(Hdlr);
    AmbaMisra_TouchUnused(&Mode);
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
 * Muxing one image to jpeg (Fullview)
 * @param [in] Hdlr The handler of exif mux
 * @param [in] Image The image information
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
static UINT32 ExifMux_Process_OneFrame(SVC_EXIF_MUX_HDLR_s *Hdlr, const SVC_IMAGE_INFO_s *Image)
{
    SVC_FIFO_BITS_DESC_s *Frame1;
    UINT32 Rval = ExifMux_GetFrame(Image, Image->Frame[0].Type, &Frame1);
    if (Rval == FORMAT_OK) {
        UINT32 FullSize = Frame1->Size - 0x22U; /* 34 = APP0 (18), DQT head (4), DHT head * 3 (4*3) */
        Rval = ExifMux_PutHead(Hdlr, Image, 0, FullSize);
        if (Rval == FORMAT_OK) {
            Rval = ExifMux_WriteImage(Hdlr, Image, Frame1->Size, GetOffset(Image->BufferBase, Frame1->StartAddr), 0, Image->Frame[0].Width, Image->Frame[0].Height);
            if (Rval != FORMAT_OK) {
                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s ExifMux_WriteImage failed!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s ExifMux_PutHead failed!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s ExifMux_GetFrame failed!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 * Muxing two image to jpeg (Fullview, thumbnail)
 * @param [in] Hdlr The handler of exif mux
 * @param [in] Image The image information
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
static UINT32 ExifMux_Process_TwoFrame(SVC_EXIF_MUX_HDLR_s *Hdlr, const SVC_IMAGE_INFO_s *Image)
{
    UINT32 Rval;
    SVC_FIFO_BITS_DESC_s *Frame1;
    Rval = ExifMux_GetFrame(Image, Image->Frame[0].Type, &Frame1);
    if (Rval == FORMAT_OK) {
        SVC_FIFO_BITS_DESC_s *Frame2;
        Rval = ExifMux_GetFrame(Image, Image->Frame[1].Type, &Frame2);
        if (Rval == FORMAT_OK) {
            UINT32 FullSize, TbSize;
            FullSize = Frame1->Size - 0x22U; /* 34 = APP0 (18), DQT head (4), DHT head * 3 (4*3) */
            TbSize = Frame2->Size - 0x22U;
            Rval = ExifMux_PutHead(Hdlr, Image, TbSize, FullSize);
            if (Rval == FORMAT_OK) {
                Rval = SvcFormat_PutBe16(Hdlr->Hdlr.Stream, Marker_SOI);
                if (Rval == FORMAT_OK) {
                    Rval = ExifMux_WriteImage(Hdlr, Image, Frame2->Size, GetOffset(Image->BufferBase, Frame2->StartAddr), 0, Image->Frame[1].Width, Image->Frame[1].Height);
                    if (Rval == FORMAT_OK) {
                        Rval = ExifMux_WriteImage(Hdlr, Image, Frame1->Size, GetOffset(Image->BufferBase, Frame1->StartAddr), 0, Image->Frame[0].Width, Image->Frame[0].Height);
                        if (Rval != FORMAT_OK) {
                            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s ExifMux_WriteImage() failed!(0)", __func__, NULL, NULL, NULL, NULL);
                        }
                    } else {
                        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s ExifMux_WriteImage() failed!(1)", __func__, NULL, NULL, NULL, NULL);
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PutBe16() failed!", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s ExifMux_PutHead() failed!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s ExifMux_GetFrame() failed!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s ExifMux_GetFrame() failed!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 * Muxing three image to jpeg (Fullview, screen-nail, thumbnail)
 * @param [in] Hdlr The handler of exif mux
 * @param [in] Image The image information
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
static UINT32 ExifMux_Process_ThreeFrame(SVC_EXIF_MUX_HDLR_s *Hdlr, const SVC_IMAGE_INFO_s *Image)
{
    UINT32 Rval;
    SVC_FIFO_BITS_DESC_s *Frame1;
    Rval = ExifMux_GetFrame(Image, Image->Frame[0].Type, &Frame1);
    if (Rval == FORMAT_OK) {
        SVC_FIFO_BITS_DESC_s *Frame2;
        Rval = ExifMux_GetFrame(Image, Image->Frame[1].Type, &Frame2);
        if (Rval == FORMAT_OK) {
            SVC_FIFO_BITS_DESC_s *Frame3;
            Rval = ExifMux_GetFrame(Image, Image->Frame[2].Type, &Frame3);
            if (Rval == FORMAT_OK) {
                UINT32 FullSize, TbSize;
                FullSize = Frame1->Size - 0x22U; /* 34 = APP0 (18), DQT head (4), DHT head * 3 (4*3) */
                TbSize = Frame2->Size - 0x22U;
                Rval = ExifMux_PutHead(Hdlr, Image, TbSize, FullSize);
                if (Rval == FORMAT_OK) {
                    Rval = SvcFormat_PutBe16(Hdlr->Hdlr.Stream, Marker_SOI);
                    if (Rval == FORMAT_OK) {
                        Rval = ExifMux_WriteImage(Hdlr, Image, Frame2->Size, GetOffset(Image->BufferBase, Frame2->StartAddr), 0, Image->Frame[1].Width, Image->Frame[1].Height);
                        if (Rval == FORMAT_OK) {
                            Rval = ExifMux_PutApp2Scr(Hdlr, Image, Frame3->Size, GetOffset(Image->BufferBase, Frame3->StartAddr));
                            if (Rval == FORMAT_OK) {
                                Rval = ExifMux_WriteImage(Hdlr, Image, Frame1->Size, GetOffset(Image->BufferBase, Frame1->StartAddr), 0, Image->Frame[0].Width, Image->Frame[0].Height);
                                if (Rval != FORMAT_OK) {
                                    AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s ExifMux_WriteImage() failed!", __func__, NULL, NULL, NULL, NULL);
                                }
                            } else {
                                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s ExifMux_PutApp2Scr() failed!", __func__, NULL, NULL, NULL, NULL);
                            }
                        } else {
                            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s ExifMux_WriteImage() failed!", __func__, NULL, NULL, NULL, NULL);
                        }
                    } else {
                        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_PutBe16() failed!", __func__, NULL, NULL, NULL, NULL);
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s ExifMux_PutHead() failed!", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s ExifMux_GetFrame() failed!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s ExifMux_GetFrame() failed!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s ExifMux_GetFrame() failed!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 * The implementation function of processing  exif mux.
 * @param [in] ExifMux The handler of exif mux to process.
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
static UINT32 ExifMux_ProcessImpl(SVC_EXIF_MUX_HDLR_s *ExifMux)
{
    UINT32 Rval = FORMAT_ERR_GENERAL_ERROR;
    const SVC_IMAGE_INFO_s *Image;
    AmbaMisra_TypeCast(&Image, &ExifMux->Hdlr.Media);
#if 0
    AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "FrameCount %u", Image->FrameCount, 0U, 0U, 0U, 0U);
    AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "Image->Frame[0]: %u %u %u", Image->Frame[0].Type, Image->Frame[0].Width, Image->Frame[0].Height, 0U, 0U);
    AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "Image->Frame[1]: %u %u %u", Image->Frame[1].Type, Image->Frame[1].Width, Image->Frame[1].Height, 0U, 0U);
    AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "Image->Frame[2]: %u %u %u", Image->Frame[2].Type, Image->Frame[2].Width, Image->Frame[2].Height, 0U, 0U);
#endif
    switch (Image->FrameCount) {
    case 1U:
        Rval = ExifMux_Process_OneFrame(ExifMux, Image);
        break;
    case 2U:
        Rval = ExifMux_Process_TwoFrame(ExifMux, Image);
        break;
    case 3U:
        Rval = ExifMux_Process_ThreeFrame(ExifMux, Image);
        break;
    default:
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Wrong FrameCount!", __func__, NULL, NULL, NULL, NULL);
        break;
    }
    if (Rval == FORMAT_OK) {
        Rval = F2F(SvcFIFO_RemoveEntry(Image->Fifo, Image->FrameCount));
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFIFO_RemoveEntry() failed!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s ExifMux_ProcessImpl failed!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}


static UINT32 SvcExifMux_Process_ParamCheck(const SVC_MUX_FORMAT_HDLR_s *Hdlr, const UINT8 *Event)
{
    UINT32 Rval = MUXER_ERR_INVALID_ARG;
    if ((Hdlr != NULL) && (Event != NULL)) {
        Rval = MUXER_OK;
    }
    return Rval;
}

/**
 * Process Exif mux.
 * @param [in] Hdlr The handler of exif mux to process.
 * @param [in] Param Not used, for Movie instance.
 * @param [out] Event Muxer event.
 * @return 0 - OK, others - MUXER_ERR_XXXX
 */
static UINT32 SvcExifMux_Process(SVC_MUX_FORMAT_HDLR_s *Hdlr, UINT64 Param, UINT8 *Event)
{
    UINT32 Rval = SvcExifMux_Process_ParamCheck(Hdlr, Event);
    if (Rval == MUXER_OK) {
        if (Hdlr->Media->MediaType == SVC_MEDIA_INFO_IMAGE) {
            SVC_EXIF_MUX_HDLR_s *ExifMux;
            AmbaMisra_TypeCast(&ExifMux, &Hdlr);
            *Event = SVC_MUXER_EVENT_NONE;
            Rval = K2M(AmbaKAL_MutexTake(&ExifMux->Mutex, SVC_FORMAT_WAIT_TIMEOUT));
            if (Rval == MUXER_OK) {
                Rval = F2M(ExifMux_ProcessImpl(ExifMux));
                if (AmbaKAL_MutexGive(&ExifMux->Mutex) != OK) {
                    AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Give mutex fail!", __func__, NULL, NULL, NULL, NULL);
                    Rval = MUXER_ERR_FATAL_ERROR;
                }
            } else {
                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Take mutex fail!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Not a image!", __func__, NULL, NULL, NULL, NULL);
            Rval = MUXER_ERR_FATAL_ERROR;
        }
    } else {
        AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "Process: incorrect Param %u!", (UINT32)Param, 0U, 0U, 0U, 0U);
    }
    AmbaMisra_TouchUnused(Hdlr);
    AmbaMisra_TouchUnused(&Param);
    return Rval;
}

static UINT32 SvcExifMux_SetUserData(SVC_MUX_FORMAT_HDLR_s *Hdlr, UINT32 Size, const UINT8 *Data)
{
    AmbaMisra_TouchUnused(Hdlr);
    AmbaMisra_TouchUnused(&Size);
    AmbaMisra_TouchUnused(&Data);
    return MUXER_OK;
}

static void SvcExifMux_InitOperation(void)
{
    g_ExifMux.Open = SvcExifMux_Open;
    g_ExifMux.Close = SvcExifMux_Close;
    g_ExifMux.Process = SvcExifMux_Process;
    g_ExifMux.GetMediaTime = NULL;
    g_ExifMux.SetUserData = SvcExifMux_SetUserData;
}

