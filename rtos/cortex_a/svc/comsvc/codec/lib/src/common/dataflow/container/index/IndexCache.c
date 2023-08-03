/**
 * @file IndexCache.c
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
#include <AmbaMisraFix.h>
#include "format/SvcFormat.h"
#include "IndexCache.h"
#include "../ByteOp.h"
#include "iso/IsoBoxDef.h"

static UINT32 IndexCache_InitTrack(INDEX_CACHE_HDLR_s *Hdlr, const INDEX_CACHE_CFG_s *Config)
{
    UINT32 Rval = FORMAT_OK;
    UINT8 i, j;
    Hdlr->TrackCount = Config->TrackCount;
    for (i = 0; i < Hdlr->TrackCount; i++) {
        INDEX_CACHE_TRACK_INFO_s *Track = &Hdlr->Track[i];
        const INDEX_CACHE_TRACK_CFG_s *TrackCfg = &Config->Track[i];
        Track->ItemCount = TrackCfg->ItemCount;
        Track->MaxCachedIndex = TrackCfg->MaxCachedIndex;
        for (j = 0; j < Track->ItemCount; j++) {
            INDEX_CACHE_ITEM_INFO_s *Item = &Hdlr->Track[i].Item[j];
            const INDEX_CACHE_ITEM_CFG_s *ItemCfg = &Config->Track[i].Item[j];
            Item->BytePerIndex = ItemCfg->BytePerIndex;
            Item->IndexCount = ItemCfg->IndexCount;
            Item->FileOffset = ItemCfg->FileOffset;
        }
    }
    return Rval;
}

/**
 * Create an IndexCache handler.
 *
 * @param [in] Hdlr IndexCache handler
 * @param [in] Config Configuration
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 IndexCache_Create(INDEX_CACHE_HDLR_s *Hdlr, const INDEX_CACHE_CFG_s *Config)
{
    UINT32 Rval = FORMAT_OK;
    if (Config->TrackCount <= SVC_FORMAT_MAX_TRACK_PER_MEDIA) {
        UINT8 i;
        for (i = 0U; i < Config->TrackCount; i++) {
            if (Config->Track[i].ItemCount <= INDEX_CACHE_MAX_ITEM_PER_TRACK) {
                if (Config->Track[i].MaxCachedIndex > INDEX_CACHE_MAX_CACHED_COUNT) {
                    AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "Config->Track[i].MaxCachedIndex > INDEX_CACHE_MAX_CACHED_COUNT", i, 0U, 0U, 0U, 0U);
                    Rval = FORMAT_ERR_INVALID_ARG;
                }
            } else {
                AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "Config->Track[%u].ItemCount > INDEX_CACHE_MAX_ITEM_PER_TRACK", i, 0U, 0U, 0U, 0U);
                Rval = FORMAT_ERR_INVALID_ARG;
            }
            if (Rval != FORMAT_OK) {
                break;
            }
        }
        if (Rval == FORMAT_OK) {
            Rval = W2F(AmbaWrap_memset(Hdlr, 0, sizeof(INDEX_CACHE_HDLR_s)));
            if (Rval == FORMAT_OK) {
                Hdlr->Mode = Config->Mode;
                Hdlr->Stream = Config->Stream;
                Rval = IndexCache_InitTrack(Hdlr, Config);
            }
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s, Config->TrackCount > SVC_FORMAT_MAX_TRACK_PER_MEDIA", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_INVALID_ARG;
    }
    return Rval;
}

/**
 * Delete an IndexCache handler.
 *
 * @param [in] Hdlr IndexCache handler
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 IndexCache_Delete(INDEX_CACHE_HDLR_s *Hdlr)
{
    AmbaMisra_TouchUnused(&Hdlr);
    AmbaMisra_TouchUnused(Hdlr);
    return FORMAT_OK;
}

static UINT32 IndexCache_ReadImpl(INDEX_CACHE_ITEM_INFO_s *Item, SVC_STREAM_HDLR_s *Stream, UINT32 ReadStart, UINT32 ReadNum)
{
    const UINT64 ReadOffset = Item->FileOffset + ((UINT64)ReadStart * (UINT64)Item->BytePerIndex);
    UINT32 Rval = S2F(Stream->Func->Seek(Stream, (INT64)ReadOffset, SVC_STREAM_SEEK_START));
    if (Rval == FORMAT_OK) {
        const UINT32 ReadSize = ReadNum * (UINT32)Item->BytePerIndex;
        if (ReadSize != 0U) {
            UINT32 BytesReaded;
            Rval = S2F(Stream->Func->Read(Stream, ReadSize, (UINT8 *)Item->Buffer, &BytesReaded));
            if (Rval == FORMAT_OK) {
                if (BytesReaded != ReadSize) {
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s, insufficient bytes!", __func__, NULL, NULL, NULL, NULL);
                    Rval = FORMAT_ERR_INVALID_FORMAT;
                }
            } else {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s, Stream->Func->Read failed!", __func__, NULL, NULL, NULL, NULL);
            }
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s, Stream->Func->Seek failed!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT8 IndexCache_IsCached(const INDEX_CACHE_ITEM_INFO_s *Item, UINT32 Index)
{
    UINT8 Rval = 1U;
    if ((Index < Item->IndexStart) || (Index >= Item->IndexEnd)) {
        Rval = 0U;
    }
    return Rval;
}

static UINT32 CheckReadParam(UINT8 TrackId, UINT8 TrackCount, UINT8 ItemId, UINT8 ItemCount, UINT32 Index, UINT32 IndexCount)
{
    UINT32 Rval = FORMAT_OK;
    if (TrackId >= TrackCount) {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s, incorrect TrackId!", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_INVALID_ARG;
    }
    if (ItemId >= ItemCount) {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s, incorrect ItemId!", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_INVALID_ARG;
    }
    if (Index >= IndexCount) {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s, incorrect Index!", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_INVALID_ARG;
    }
    return Rval;
}

/**
 * Read from IndexCache handler.
 *
 * @param [in] Hdlr IndexCache handler
 * @param [in] TrackId Track id
 * @param [in] ItemId Item id
 * @param [in] Index Index
 * @param [in] Direction Direction (SVC_FORMAT_DIR_FORWARD / SVC_FORMAT_DIR_BACKWARD)
 * @param [in] Data Destination buffer
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 IndexCache_Read(INDEX_CACHE_HDLR_s *Hdlr, UINT8 TrackId, UINT8 ItemId, UINT32 Index, UINT8 Direction, void *Data)
{
    INDEX_CACHE_TRACK_INFO_s * const Track = &Hdlr->Track[TrackId];
    INDEX_CACHE_ITEM_INFO_s * const Item = &Track->Item[ItemId];
    UINT32 Rval = CheckReadParam(TrackId, Hdlr->TrackCount, ItemId, Track->ItemCount, Index, Item->IndexCount);
    if (Rval == FORMAT_OK) {
        if (IndexCache_IsCached(Item, Index) == 0U) {
            UINT32 ReadNum;
            UINT32 ReadStart = 0U;
            if (Item->IndexCount < Track->MaxCachedIndex) {
                ReadNum = Item->IndexCount;
            } else {
                if ((Item->IndexCount - Index) <= Track->MaxCachedIndex) {
                    ReadStart = (Item->IndexCount - Track->MaxCachedIndex);
                } else {
                    if (Direction == SVC_FORMAT_DIR_FORWARD) {
                        ReadStart = Index;
                    } else if (Direction == SVC_FORMAT_DIR_BACKWARD) {
                        if (Index >= Track->MaxCachedIndex) {
                            ReadStart = Index - Track->MaxCachedIndex + 1U;
                        } else {
                            ReadStart = 0U;
                        }
                    } else {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s, incorrect Direction!", __func__, NULL, NULL, NULL, NULL);
                        Rval = FORMAT_ERR_INVALID_ARG;
                    }
                }
                ReadNum = Track->MaxCachedIndex;
            }
            if (Rval == FORMAT_OK) {
                //AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "Load index (%u, %u, %u) : Start=%u Num=%u", TrackId, ItemId, Index, ReadStart, ReadNum);
                Rval = IndexCache_ReadImpl(Item, Hdlr->Stream, ReadStart, ReadNum);
                if (Rval == FORMAT_OK) {
                    Item->IndexStart = ReadStart;
                    Item->IndexEnd = ReadStart + ReadNum;
                }
            }
        }
    }
    if (Rval == FORMAT_OK) {
        const UINT8 *Buffer = (UINT8 *)&Item->Buffer[0];
        Buffer = &Buffer[(Index - Item->IndexStart) * (UINT32)Item->BytePerIndex];
        Rval = W2F(AmbaWrap_memcpy(Data, Buffer, Item->BytePerIndex));
    }
    return Rval;
}

