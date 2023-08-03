/**
 * @file BufferStream.c
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
#include "stream/BufferStream.h"

#define STREAM_OK                   OK                      /**< Execution OK */
#define STREAM_ERR_INVALID_ARG      STREAM_ERR_0000         /**< Invalid argument */
#define STREAM_ERR_FATAL_ERROR      STREAM_ERR_0001         /**< Fatal error */
#define STREAM_ERR_IO_ERROR         STREAM_ERR_0002         /**< IO error */

#define SVC_STREAM_PRINT_MODULE_ID        ((UINT16)(STREAM_ERR_BASE >> 16U))     /**< Module ID for AmbaPrint */

#define SVC_BUF_WAIT_TIMEOUT 30000U  /**< Time out */

/**
 *  Buffer stream handler
 */
typedef struct {
    SVC_STREAM_HDLR_s Hdlr; /**< Stream Handler */
    AMBA_KAL_MUTEX_t Mutex; /**< Mutex to protect handler's internal variables */
    UINT32 (*CbBufCopy)(ULONG Dst, ULONG Src, UINT32 Size);   /**< Function pointer that copy data to a buffer */
    UINT8 Used;             /**< Used flag */
    UINT8 Mode;             /**< Mode */
    ULONG BufBase;          /**< Buffer base */
    UINT32 BufSize;         /**< Buffer size */
    UINT32 CurPos;          /**< Current position of the buffer */
    UINT32 CurLength;       /**< Current Length of the buffer */
    UINT32 TotalLength;     /**< Accumulated length after opening the stream handler */
} SVC_BUFFER_STREAM_HDLR_s;

/**
 *  Buffer stream manager
 */
typedef struct {
    AMBA_KAL_MUTEX_t Mutex; /**< Mutex to protect handler */
    SVC_BUFFER_STREAM_HDLR_s Hdlr[SVC_BUFFER_STREAM_MAX_NUM];   /**< Buffer stream handlers */
} SVC_BUFFER_STREAM_MGR_s;

static SVC_BUFFER_STREAM_MGR_s g_BufMgr GNU_SECTION_NOZEROINIT;
static SVC_STREAM_s g_Buffer;
static void SvcBufferStream_InitOperation(void);

static inline UINT32 W2S(UINT32 Ret)
{
    UINT32 Rval = STREAM_OK;
    if (Ret != OK) {
        Rval = STREAM_ERR_FATAL_ERROR;
    }
    return Rval;
}

static inline UINT32 K2S(UINT32 Ret)
{
    UINT32 Rval = STREAM_OK;
    if (Ret != OK) {
        Rval = STREAM_ERR_FATAL_ERROR;
    }
    return Rval;
}

static UINT32 CreateMutex(void)
{
    static char MgrMutexName[16] = "BufStrMgr";
    UINT32 Rval = K2S(AmbaKAL_MutexCreate(&g_BufMgr.Mutex, MgrMutexName));
    if (Rval == STREAM_OK) {
        static char MutexName[SVC_BUFFER_STREAM_MAX_NUM][16];
        UINT32 i;
        for (i = 0; i < SVC_BUFFER_STREAM_MAX_NUM; i++) {
            UINT32 Len = AmbaUtility_StringPrintUInt32(MutexName[i], (UINT32)sizeof(MutexName[i]), "BufStr%02u", 1, &i);
            Rval = K2S(AmbaKAL_MutexCreate(&g_BufMgr.Hdlr[i].Mutex, MutexName[i]));
            if (Rval != OK) {
                AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Create handler's mutex failed!", __func__, NULL, NULL, NULL, NULL);
                break;
            }
            AmbaMisra_TouchUnused(&Len);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Create mutex failed!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}


/**
 *  Initialize the buffer stream module.
 *
 *  @return OK or STREAM_ERR_XXXX
 */
UINT32 SvcBufferStream_Init(void)
{
    static UINT8 g_BufMgrInit = 0U;
    UINT32 Rval = STREAM_OK;
    if (g_BufMgrInit == 0U) {
        g_BufMgrInit = 1U;
        Rval = W2S(AmbaWrap_memset(&g_BufMgr, 0, sizeof(g_BufMgr)));
        if (Rval == STREAM_OK) {
            SvcBufferStream_InitOperation();
            Rval = CreateMutex();
        }
    }
    return Rval;
}

static SVC_BUFFER_STREAM_HDLR_s *GetHdlr(void)
{
    UINT32 i;
    SVC_BUFFER_STREAM_HDLR_s *Hdlr = NULL;
    for (i = 0; i < SVC_BUFFER_STREAM_MAX_NUM; i++) {
        Hdlr = &g_BufMgr.Hdlr[i];
        if (Hdlr->Used == 0U) {
            Hdlr->Used = 1U;
            break;
        }
    }
    if (i == SVC_BUFFER_STREAM_MAX_NUM) {
        AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Get hdlr fail!", __func__, NULL, NULL, NULL, NULL);
        Hdlr = NULL;
    }
    return Hdlr;
}

static UINT32 RelHdlr(SVC_BUFFER_STREAM_HDLR_s *Hdlr)
{
    UINT32 Rval = STREAM_OK;
    UINT32 i;
    for (i = 0; i < SVC_BUFFER_STREAM_MAX_NUM; i++) {
        SVC_BUFFER_STREAM_HDLR_s *BufStr = &g_BufMgr.Hdlr[i];
        if (BufStr == Hdlr) {
            if (BufStr->Used == 1U) {
                BufStr->Used = 0U;
            } else {
                AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s BufStr idle!", __func__, NULL, NULL, NULL, NULL);
                Rval = STREAM_ERR_FATAL_ERROR;
            }
            break;
        }
    }
    if (i == SVC_BUFFER_STREAM_MAX_NUM) {
        AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Get Hdlr fail!", __func__, NULL, NULL, NULL, NULL);
        Rval = STREAM_ERR_FATAL_ERROR;
    }
    AmbaMisra_TouchUnused(Hdlr);
    return Rval;
}

static UINT32 SvcBufferStream_CreateImpl(const SVC_BUFFER_STREAM_CFG_s *Config, SVC_BUFFER_STREAM_HDLR_s **Hdlr)
{
    UINT32 Rval = STREAM_ERR_FATAL_ERROR;
    SVC_BUFFER_STREAM_HDLR_s *BufStr = GetHdlr();
    if (BufStr != NULL) {
        Rval = K2S(AmbaKAL_MutexTake(&BufStr->Mutex, SVC_BUF_WAIT_TIMEOUT));
        if (Rval == STREAM_OK) {
            BufStr->Hdlr.Func = &g_Buffer;
            BufStr->CbBufCopy = Config->pfnBufCopy;
            BufStr->Mode = 0U;
            if (AmbaKAL_MutexGive(&BufStr->Mutex) != OK) {
                AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Give mutex fail!", __func__, NULL, NULL, NULL, NULL);
                Rval = STREAM_ERR_FATAL_ERROR;
            }
        }
    }
    *Hdlr = BufStr;
    return Rval;
}

static UINT32 SvcBufferStream_Create_ParamCheck(const SVC_BUFFER_STREAM_CFG_s *Config, SVC_STREAM_HDLR_s * const *Hdlr)
{
    UINT32 Rval = STREAM_ERR_INVALID_ARG;
    if ((Config != NULL) && (Hdlr != NULL)) {
        Rval = STREAM_OK;
    }
    return Rval;
}

/**
 *  Create a buffer stream handler.
 *
 *  @param [in] Config The configuration used to create a buffer stream handler
 *  @param [out] Hdlr The returned handler
 *  @return OK or STREAM_ERR_XXXX
 */
UINT32 SvcBufferStream_Create(const SVC_BUFFER_STREAM_CFG_s *Config, SVC_STREAM_HDLR_s **Hdlr)
{
    UINT32 Rval;
    Rval = SvcBufferStream_Create_ParamCheck(Config, Hdlr);
    if (Rval == STREAM_OK) {
        Rval = K2S(AmbaKAL_MutexTake(&g_BufMgr.Mutex, SVC_BUF_WAIT_TIMEOUT));
        if (Rval == STREAM_OK) {
            SVC_BUFFER_STREAM_HDLR_s *Buf;
            Rval = SvcBufferStream_CreateImpl(Config, &Buf);
            if (Rval == STREAM_OK) {
                *Hdlr = &Buf->Hdlr;
            }
            if (AmbaKAL_MutexGive(&g_BufMgr.Mutex) != OK) {
                AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Give mutex fail!", __func__, NULL, NULL, NULL, NULL);
                Rval = STREAM_ERR_FATAL_ERROR;
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Take mutex fail!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Invalid argument!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 SvcBufferStream_DeleteImpl(SVC_BUFFER_STREAM_HDLR_s *Hdlr)
{
    UINT32 Rval = K2S(AmbaKAL_MutexTake(&Hdlr->Mutex, SVC_BUF_WAIT_TIMEOUT));
    if (Rval == STREAM_OK) {
        Rval = RelHdlr(Hdlr);
        if (AmbaKAL_MutexGive(&Hdlr->Mutex) != OK) {
            AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Give mutex fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = STREAM_ERR_FATAL_ERROR;
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Take mutex fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 SvcBufferStream_Delete_ParamCheck(const SVC_STREAM_HDLR_s *Hdlr)
{
    UINT32 Rval = STREAM_ERR_INVALID_ARG;
    if (Hdlr != NULL) {
        Rval = STREAM_OK;
    }
    return Rval;
}

/**
 *  Delete a buffer stream handler.
 *
 *  @param [in] Hdlr The handler of a buffer stream being removed
 *  @return OK or STREAM_ERR_XXXX
 */
UINT32 SvcBufferStream_Delete(SVC_STREAM_HDLR_s *Hdlr)
{
    UINT32 Rval;
    Rval = SvcBufferStream_Delete_ParamCheck(Hdlr);
    if (Rval == STREAM_OK) {
        Rval = K2S(AmbaKAL_MutexTake(&g_BufMgr.Mutex, SVC_BUF_WAIT_TIMEOUT));
        if (Rval == STREAM_OK) {
            SVC_BUFFER_STREAM_HDLR_s *BufStr;
            AmbaMisra_TypeCast(&BufStr, &Hdlr);
            Rval = SvcBufferStream_DeleteImpl(BufStr);
            if (AmbaKAL_MutexGive(&g_BufMgr.Mutex) != OK) {
                AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Give mutex fail!", __func__, NULL, NULL, NULL, NULL);
                Rval = STREAM_ERR_FATAL_ERROR;
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Take mutex fail!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Invalid handler!", __func__, NULL, NULL, NULL, NULL);
    }
    AmbaMisra_TouchUnused(Hdlr);
    return Rval;
}

static UINT32 SvcBufferStream_ConfigBuffer_ParamCheck(const SVC_STREAM_HDLR_s *Hdlr, ULONG Addr, UINT32 Size)
{
    UINT32 Rval = STREAM_ERR_INVALID_ARG;
    if ((Hdlr != NULL) && (Addr != 0UL) && (Size != 0U)) {
        Rval = STREAM_OK;
    }
    return Rval;
}

/**
 *  Config the destination buffer of a buffer stream handler
 *
 *  @param [in] Hdlr The buffer stream handler
 *  @param [in] Addr The new destination buffer
 *  @param [in] Size The size of the new destination buffer
 *  @return OK or STREAM_ERR_XXXX
 */
UINT32 SvcBufferStream_ConfigBuffer(SVC_STREAM_HDLR_s *Hdlr, ULONG Addr, UINT32 Size)
{
    UINT32 Rval;
    Rval = SvcBufferStream_ConfigBuffer_ParamCheck(Hdlr, Addr, Size);
    if (Rval == STREAM_OK) {
        SVC_BUFFER_STREAM_HDLR_s *BufStr;
        AmbaMisra_TypeCast(&BufStr, &Hdlr);
        Rval = K2S(AmbaKAL_MutexTake(&BufStr->Mutex, SVC_BUF_WAIT_TIMEOUT));
        if (Rval == STREAM_OK) {
            BufStr->BufBase = Addr;
            BufStr->BufSize = Size;
            BufStr->TotalLength += BufStr->CurLength;
            BufStr->CurPos = 0U;
            BufStr->CurLength = 0U;
            if (AmbaKAL_MutexGive(&BufStr->Mutex) != OK) {
                AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Give mutex fail!", __func__, NULL, NULL, NULL, NULL);
                Rval = STREAM_ERR_FATAL_ERROR;
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Take mutex fail!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Invalid handler!", __func__, NULL, NULL, NULL, NULL);
    }
    AmbaMisra_TouchUnused(Hdlr);
    return Rval;
}


static UINT32 SvcBufferStream_Open_ParamCheck(const SVC_STREAM_HDLR_s *Hdlr, const char *Name, UINT32 Mode)
{
    UINT32 Rval = STREAM_ERR_INVALID_ARG;
    if (Hdlr != NULL) {
        if (Mode == SVC_STREAM_MODE_WRONLY) {
            Rval = STREAM_OK;
        } else {
            AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Only support SVC_STREAM_MODE_WRONLY!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    AmbaMisra_TouchUnused(&Name);
    return Rval;
}

static UINT32 SvcBufferStream_Open(SVC_STREAM_HDLR_s *Hdlr, const char *Name, UINT32 Mode)
{
    UINT32 Rval;
    Rval = SvcBufferStream_Open_ParamCheck(Hdlr, Name, Mode);
    if (Rval == STREAM_OK) {
        SVC_BUFFER_STREAM_HDLR_s *BufStr;
        AmbaMisra_TypeCast(&BufStr, &Hdlr);
        Rval = K2S(AmbaKAL_MutexTake(&BufStr->Mutex, SVC_BUF_WAIT_TIMEOUT));
        if (Rval == STREAM_OK) {
            if (BufStr->Mode == 0U) {
                BufStr->Mode = (UINT8)Mode;
                BufStr->BufBase = 0UL;
                BufStr->BufSize = 0U;
                BufStr->CurPos = 0U;
                BufStr->CurLength = 0U;
                BufStr->TotalLength = 0U;
            } else {
                AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Already opened!", __func__, NULL, NULL, NULL, NULL);
                Rval = STREAM_ERR_FATAL_ERROR;
            }
            if (AmbaKAL_MutexGive(&BufStr->Mutex) != OK) {
                AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Give mutex fail!", __func__, NULL, NULL, NULL, NULL);
                Rval = STREAM_ERR_FATAL_ERROR;
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Take mutex fail!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Invalid argument!", __func__, NULL, NULL, NULL, NULL);
    }
    AmbaMisra_TouchUnused(Hdlr);
    return Rval;
}

static UINT32 SvcBufferStream_Close_ParamCheck(const SVC_STREAM_HDLR_s *Hdlr)
{
    UINT32 Rval = STREAM_ERR_INVALID_ARG;
    if (Hdlr != NULL) {
        Rval = STREAM_OK;
    }
    return Rval;
}

static UINT32 SvcBufferStream_Close(SVC_STREAM_HDLR_s *Hdlr)
{
    UINT32 Rval;
    Rval = SvcBufferStream_Close_ParamCheck(Hdlr);
    if (Rval == STREAM_OK) {
        SVC_BUFFER_STREAM_HDLR_s *BufStr;
        AmbaMisra_TypeCast(&BufStr, &Hdlr);
        Rval = K2S(AmbaKAL_MutexTake(&BufStr->Mutex, SVC_BUF_WAIT_TIMEOUT));
        if (Rval == STREAM_OK) {
            if (BufStr->Mode != 0U)  {
                BufStr->Mode = 0U;
            } else {
                AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Already closed!", __func__, NULL, NULL, NULL, NULL);
                Rval = STREAM_ERR_FATAL_ERROR;
            }
            if (AmbaKAL_MutexGive(&BufStr->Mutex) != OK) {
                AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Give mutex fail!", __func__, NULL, NULL, NULL, NULL);
                Rval = STREAM_ERR_FATAL_ERROR;
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Take mutex fail!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Invalid argument!", __func__, NULL, NULL, NULL, NULL);
    }
    AmbaMisra_TouchUnused(Hdlr);
    return Rval;
}

static UINT32 SvcBufferStream_Read_ParamCheck(const SVC_STREAM_HDLR_s *Hdlr, UINT32 Size, const UINT8 *Buffer, const UINT32 *Count)
{
    UINT32 Rval = STREAM_ERR_INVALID_ARG;
    if (Hdlr != NULL) {
        if (Size > 0U) {
            if (Buffer != NULL) {
                if (Count != NULL) {
                    Rval = STREAM_OK;
                }
            }
        }
    }
    return Rval;
}

static UINT32 SvcBufferStream_Read(SVC_STREAM_HDLR_s *Hdlr, UINT32 Size, UINT8 *Buffer, UINT32 *Count)
{
    UINT32 Rval;
    Rval = SvcBufferStream_Read_ParamCheck(Hdlr, Size, Buffer, Count);
    if (Rval == STREAM_OK) {
        SVC_BUFFER_STREAM_HDLR_s *BufStr;
        AmbaMisra_TypeCast(&BufStr, &Hdlr);
        Rval = K2S(AmbaKAL_MutexTake(&BufStr->Mutex, SVC_BUF_WAIT_TIMEOUT));
        if (Rval == STREAM_OK) {
            Rval = STREAM_ERR_FATAL_ERROR;
            AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s unsupported!", __func__, NULL, NULL, NULL, NULL);
            if (AmbaKAL_MutexGive(&BufStr->Mutex) != OK) {
                AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Give mutex fail!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Take mutex fail!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Invalid argument!", __func__, NULL, NULL, NULL, NULL);
    }
    AmbaMisra_TouchUnused(Hdlr);
    AmbaMisra_TouchUnused(Buffer);
    AmbaMisra_TouchUnused(Count);
    return Rval;
}

static UINT32 SvcBufferStream_Write_ParamCheck(const SVC_STREAM_HDLR_s *Hdlr, UINT32 Size, const UINT8 *Buffer, const UINT32 *Count)
{
    UINT32 Rval = STREAM_ERR_INVALID_ARG;
    if (Hdlr != NULL) {
        if (Size > 0U) {
            if (Buffer != NULL) {
                if (Count != NULL) {
                    Rval = STREAM_OK;
                }
            }
        }
    }
    return Rval;
}

static UINT32 SvcBufferStream_Write(SVC_STREAM_HDLR_s *Hdlr, UINT32 Size, UINT8 *Buffer, UINT32 *Count)
{
    UINT32 Rval;
    Rval = SvcBufferStream_Write_ParamCheck(Hdlr, Size, Buffer, Count);
    if (Rval == STREAM_OK) {
        SVC_BUFFER_STREAM_HDLR_s *BufStr;
        AmbaMisra_TypeCast(&BufStr, &Hdlr);
        Rval = K2S(AmbaKAL_MutexTake(&BufStr->Mutex, SVC_BUF_WAIT_TIMEOUT));
        if (Rval == STREAM_OK) {
            if (BufStr->Mode != 0U) {
                if ((BufStr->CurPos + Size) <= BufStr->BufSize) {
                    ULONG SrcBuf = 0U;
                    AmbaMisra_TypeCast(&SrcBuf, &Buffer);
                    Rval = BufStr->CbBufCopy(BufStr->BufBase + (ULONG)BufStr->CurPos, SrcBuf, Size);
                    if (Rval == OK) {
                        BufStr->CurPos += Size;
                        if (BufStr->CurPos > BufStr->CurLength) {
                            BufStr->CurLength = BufStr->CurPos;
                        }
                        *Count = Size;
                    } else {
                        *Count = 0U;
                        if (Rval != STREAM_ERR_IO_ERROR) {
                            Rval = STREAM_ERR_FATAL_ERROR;
                        }
                        AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s CbBufCopy failed!", __func__, NULL, NULL, NULL, NULL);
                    }
                } else {
                    Rval = STREAM_ERR_FATAL_ERROR;
                    AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Not enough space!", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Hdlr not opened!", __func__, NULL, NULL, NULL, NULL);
                Rval = STREAM_ERR_FATAL_ERROR;
            }
            if (AmbaKAL_MutexGive(&BufStr->Mutex) != OK) {
                AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Give mutex fail!", __func__, NULL, NULL, NULL, NULL);
                Rval = STREAM_ERR_FATAL_ERROR;
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Take mutex fail!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Invalid argument!", __func__, NULL, NULL, NULL, NULL);
    }
    AmbaMisra_TouchUnused(Hdlr);
    AmbaMisra_TouchUnused(Buffer);
    return Rval;
}

static UINT32 SvcBufferStream_GetPos_ParamCheck(const SVC_STREAM_HDLR_s *Hdlr, const UINT64 *Pos)
{
    UINT32 Rval = STREAM_ERR_INVALID_ARG;
    if (Hdlr != NULL) {
        if (Pos != NULL) {
            Rval = STREAM_OK;
        }
    }
    return Rval;
}

static UINT32 SvcBufferStream_GetPos(SVC_STREAM_HDLR_s *Hdlr, UINT64 *Pos)
{
    UINT32 Rval;
    Rval = SvcBufferStream_GetPos_ParamCheck(Hdlr, Pos);
    if (Rval == STREAM_OK) {
        SVC_BUFFER_STREAM_HDLR_s *BufStr;
        AmbaMisra_TypeCast(&BufStr, &Hdlr);
        Rval = K2S(AmbaKAL_MutexTake(&BufStr->Mutex, SVC_BUF_WAIT_TIMEOUT));
        if (Rval == STREAM_OK) {
            if (BufStr->Mode != 0U) {
                *Pos = (UINT64)BufStr->TotalLength + (UINT64)BufStr->CurPos;
            } else {
                AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Hdlr not opened!", __func__, NULL, NULL, NULL, NULL);
                Rval = STREAM_ERR_FATAL_ERROR;
            }
            if (AmbaKAL_MutexGive(&BufStr->Mutex) != OK) {
                AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Give mutex fail!", __func__, NULL, NULL, NULL, NULL);
                Rval = STREAM_ERR_FATAL_ERROR;
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Take mutex fail!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Invalid argument!", __func__, NULL, NULL, NULL, NULL);
    }
    AmbaMisra_TouchUnused(Hdlr);
    return Rval;
}

static UINT32 SvcBufferStream_GetLength_ParamCheck(const SVC_STREAM_HDLR_s *Hdlr, const UINT64 *Size)
{
    UINT32 Rval = STREAM_ERR_INVALID_ARG;
    if (Hdlr != NULL) {
        if (Size != NULL) {
            Rval = STREAM_OK;
        }
    }
    return Rval;
}

static UINT32 SvcBufferStream_GetLength(SVC_STREAM_HDLR_s *Hdlr, UINT64 *Size)
{
    UINT32 Rval;
    Rval = SvcBufferStream_GetLength_ParamCheck(Hdlr, Size);
    if (Rval == STREAM_OK) {
        SVC_BUFFER_STREAM_HDLR_s *BufStr;
        AmbaMisra_TypeCast(&BufStr, &Hdlr);
        Rval = K2S(AmbaKAL_MutexTake(&BufStr->Mutex, SVC_BUF_WAIT_TIMEOUT));
        if (Rval == STREAM_OK) {
            if (BufStr->Mode != 0U) {
                *Size = (UINT64)BufStr->TotalLength + (UINT64)BufStr->CurLength;
            } else {
                AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Hdlr not opened!", __func__, NULL, NULL, NULL, NULL);
                Rval = STREAM_ERR_FATAL_ERROR;
            }
            if (AmbaKAL_MutexGive(&BufStr->Mutex) != OK) {
                AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Give mutex fail!", __func__, NULL, NULL, NULL, NULL);
                Rval = STREAM_ERR_FATAL_ERROR;
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Take mutex fail!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Invalid argument!", __func__, NULL, NULL, NULL, NULL);
    }
    AmbaMisra_TouchUnused(Hdlr);
    return Rval;
}

static UINT32 SvcBufferStream_GetFreeSpace_ParamCheck(const SVC_STREAM_HDLR_s *Hdlr, const UINT64 *Size)
{
    UINT32 Rval = STREAM_ERR_INVALID_ARG;
    if (Hdlr != NULL) {
        if (Size != NULL) {
            Rval = STREAM_OK;
        }
    }
    return Rval;
}

static UINT32 SvcBufferStream_GetFreeSpace(SVC_STREAM_HDLR_s *Hdlr, UINT64 *Size)
{
    UINT32 Rval;
    Rval = SvcBufferStream_GetFreeSpace_ParamCheck(Hdlr, Size);
    if (Rval == STREAM_OK) {
        SVC_BUFFER_STREAM_HDLR_s *BufStr;
        AmbaMisra_TypeCast(&BufStr, &Hdlr);
        Rval = K2S(AmbaKAL_MutexTake(&BufStr->Mutex, SVC_BUF_WAIT_TIMEOUT));
        if (Rval == STREAM_OK) {
            if (BufStr->Mode != 0U) {
                *Size = (UINT64)BufStr->BufSize - (UINT64)BufStr->CurLength;
            } else {
                AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Hdlr not opened!", __func__, NULL, NULL, NULL, NULL);
                Rval = STREAM_ERR_FATAL_ERROR;
            }
            if (AmbaKAL_MutexGive(&BufStr->Mutex) != OK) {
                AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Give mutex fail!", __func__, NULL, NULL, NULL, NULL);
                Rval = STREAM_ERR_FATAL_ERROR;
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Take mutex fail!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Invalid argument!", __func__, NULL, NULL, NULL, NULL);
    }
    AmbaMisra_TouchUnused(Hdlr);
    return Rval;
}

static UINT32 SvcBufferStream_Seek_ParamCheck(const SVC_STREAM_HDLR_s *Hdlr, INT64 Pos, INT32 Orig)
{
    UINT32 Rval = STREAM_ERR_INVALID_ARG;
    if (Hdlr != NULL) {
        if ((Orig == SVC_STREAM_SEEK_CUR) || (Orig == SVC_STREAM_SEEK_START) || (Orig == SVC_STREAM_SEEK_END)) {
            Rval = STREAM_OK;
        }
    }
    AmbaMisra_TouchUnused(&Pos);
    return Rval;
}

static UINT32 SvcBufferStream_Seek(SVC_STREAM_HDLR_s *Hdlr, INT64 Pos, INT32 Orig)
{
    UINT32 Rval;
    Rval = SvcBufferStream_Seek_ParamCheck(Hdlr, Pos, Orig);
    if (Rval == STREAM_OK) {
        SVC_BUFFER_STREAM_HDLR_s *BufStr;
        AmbaMisra_TypeCast(&BufStr, &Hdlr);
        Rval = K2S(AmbaKAL_MutexTake(&BufStr->Mutex, SVC_BUF_WAIT_TIMEOUT));
        if (Rval == STREAM_OK) {
            INT64 TargetPos = Pos;
            if (Orig == SVC_STREAM_SEEK_CUR) {
                TargetPos +=(INT64)BufStr->CurPos;
            } else if (Orig == SVC_STREAM_SEEK_START) {
                TargetPos -=(INT64)BufStr->TotalLength;
            } else {
                TargetPos += (INT64)BufStr->CurLength;
            }
            if ((TargetPos >= 0) && (TargetPos <= (INT64)BufStr->CurLength)) {
                BufStr->CurPos = (UINT32)TargetPos;
            } else {
                AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Out of range!", __func__, NULL, NULL, NULL, NULL);
                Rval = STREAM_ERR_FATAL_ERROR;
            }
            if (AmbaKAL_MutexGive(&BufStr->Mutex) != OK) {
                AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Give mutex fail!", __func__, NULL, NULL, NULL, NULL);
                Rval = STREAM_ERR_FATAL_ERROR;
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Take mutex fail!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Invalid argument!", __func__, NULL, NULL, NULL, NULL);
    }
    AmbaMisra_TouchUnused(Hdlr);
    return Rval;
}

static UINT32 SvcBufferStream_Sync_ParamCheck(const SVC_STREAM_HDLR_s *Hdlr)
{
    UINT32 Rval = STREAM_ERR_INVALID_ARG;
    if (Hdlr != NULL) {
        Rval = STREAM_OK;
    }
    return Rval;
}

static UINT32 SvcBufferStream_Sync(SVC_STREAM_HDLR_s *Hdlr)
{
    UINT32 Rval;
    Rval = SvcBufferStream_Sync_ParamCheck(Hdlr);
    if (Rval == STREAM_OK) {
        SVC_BUFFER_STREAM_HDLR_s *BufStr;
        AmbaMisra_TypeCast(&BufStr, &Hdlr);
        Rval = K2S(AmbaKAL_MutexTake(&BufStr->Mutex, SVC_BUF_WAIT_TIMEOUT));
        if (Rval == STREAM_OK) {
            Rval = STREAM_ERR_FATAL_ERROR;
            AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s unsupported!", __func__, NULL, NULL, NULL, NULL);
            if (AmbaKAL_MutexGive(&BufStr->Mutex) != OK) {
                AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Give mutex fail!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Take mutex fail!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Invalid argument!", __func__, NULL, NULL, NULL, NULL);
    }
    AmbaMisra_TouchUnused(Hdlr);
    return Rval;
}

static UINT32 SvcBufferStream_Func_ParamCheck(const SVC_STREAM_HDLR_s *Hdlr, UINT32 Cmd, UINT32 Param)
{
    UINT32 Rval = STREAM_ERR_INVALID_ARG;
    if (Hdlr != NULL) {
        Rval = STREAM_OK;
    }
    AmbaMisra_TouchUnused(&Cmd);
    AmbaMisra_TouchUnused(&Param);
    return Rval;
}

static UINT32 SvcBufferStream_Func(SVC_STREAM_HDLR_s *Hdlr, UINT32 Cmd, UINT32 Param)
{
    UINT32 Rval;
    Rval = SvcBufferStream_Func_ParamCheck(Hdlr, Cmd, Param);
    if (Rval == STREAM_OK) {
        SVC_BUFFER_STREAM_HDLR_s *BufStr;
        AmbaMisra_TypeCast(&BufStr, &Hdlr);
        Rval = K2S(AmbaKAL_MutexTake(&BufStr->Mutex, SVC_BUF_WAIT_TIMEOUT));
        if (Rval == STREAM_OK) {
            Rval = STREAM_ERR_FATAL_ERROR;
            AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s unsupported!", __func__, NULL, NULL, NULL, NULL);
            if (AmbaKAL_MutexGive(&BufStr->Mutex) != OK) {
                AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Give mutex fail!", __func__, NULL, NULL, NULL, NULL);
                Rval = STREAM_ERR_FATAL_ERROR;
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Take mutex fail!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Invalid argument!", __func__, NULL, NULL, NULL, NULL);
    }
    AmbaMisra_TouchUnused(Hdlr);
    return Rval;
}

static void SvcBufferStream_InitOperation(void)
{
    g_Buffer.Open = SvcBufferStream_Open;
    g_Buffer.Close = SvcBufferStream_Close;
    g_Buffer.Read = SvcBufferStream_Read;
    g_Buffer.Write = SvcBufferStream_Write;
    g_Buffer.Seek = SvcBufferStream_Seek;
    g_Buffer.GetPos = SvcBufferStream_GetPos;
    g_Buffer.GetLength = SvcBufferStream_GetLength;
    g_Buffer.GetFreeSpace = SvcBufferStream_GetFreeSpace;
    g_Buffer.Sync = SvcBufferStream_Sync;
    g_Buffer.Func = SvcBufferStream_Func;
}

