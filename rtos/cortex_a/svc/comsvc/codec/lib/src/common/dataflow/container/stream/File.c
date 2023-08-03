/**
 * @file File.c
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
#include "stream/SvcFile.h"
#include "FormatAPI.h"
#include <AmbaCFS.h>
#include <AmbaDef.h>

#define STREAM_OK                   OK                      /**< Execution OK */
#define STREAM_ERR_INVALID_ARG      STREAM_ERR_0000         /**< Inavalid argument */
#define STREAM_ERR_FATAL_ERROR      STREAM_ERR_0001         /**< Fatal error */
#define STREAM_ERR_IO_ERROR         STREAM_ERR_0002         /**< IO error */

#define SVC_STREAM_PRINT_MODULE_ID        ((UINT16)(STREAM_ERR_BASE >> 16U))     /**< Module ID for AmbaPrint */

#define SVC_FILE_WAIT_TIMEOUT 30000U    /**< Mutex time out */

//#define AMBA_LOG_CUSTOM_MODULE_TAG "STREAM"
//#include "Amba_Log.h"

/*#define DEFAULT_FIO_RATE    (10 * 1024)*/

/**
 *  File stream handler
 */
typedef struct {
    SVC_STREAM_HDLR_s Hdlr; /**< Stream handler */
    AMBA_KAL_MUTEX_t Mutex; /**< To protect handler's internal variables */ //  TODO: if performance not good, remove it
    AMBA_CFS_FILE_s *File;  /**< Cfs file pointer */
    char URL[AMBA_CFS_MAX_FILENAME_LENGTH]; /**< URL */
    UINT32 Alignment;       /**< Pre-allocation */
    UINT32 BytesToSync;     /**< The number of bytes that the CFS would invoke FileSync() one time */
    UINT8 Async;            /**< The flag to enable async mode */
    UINT8 Mode;             /**< File open mode */
    UINT8 Used;             /**< Used flag */
    UINT8 *BankBuffer;      /**< The bank buffer. This is dummy when Async = 0. */
    UINT32 BankBufferSize;  /**< The size of the bank buffer. This is dummy when Async = 0. The bank amount will be (BankBufferSize / BankSize). BankBufferSize should be aligned to BankSize. */
} SVC_FILE_STREAM_HDLR_s;

/**
 *  File stream manager
 */
typedef struct {
    AMBA_KAL_MUTEX_t Mutex; /**< To protect handler */
    UINT8 MaxHdlr;          /**< Maximum number of handlers */
    SVC_FILE_STREAM_HDLR_s *Hdlr; /**< File stream handlers */
} SVC_FILE_STREAM_MGR_s;

static SVC_FILE_STREAM_MGR_s g_FileMgr GNU_SECTION_NOZEROINIT;
static SVC_STREAM_s g_File;
static void SvcFileStream_InitOperation(void);

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

static inline UINT32 C2S(UINT32 Ret)
{
    UINT32 Rval = STREAM_OK;
    if (Ret != OK) {
        if (Ret == CFS_ERR_0000) {
            Rval = STREAM_ERR_INVALID_ARG;
        } else if (Ret == CFS_ERR_0002) {
            Rval = STREAM_ERR_IO_ERROR;
        } else {
            Rval = STREAM_ERR_FATAL_ERROR;
        }
    }
    return Rval;
}

static UINT32 SvcFileStream_GetInitDefCfg_ParamCheck(const SVC_FILE_STREAM_INIT_CFG_s *Config)
{
    UINT32 Rval = STREAM_ERR_INVALID_ARG;
    if (Config != NULL) {
        Rval = STREAM_OK;
    }
    return Rval;
}

/**
 * Get the default configuration for initializing the File Stream module.
 *
 * @param [out] Config The returned configuration
 * @return OK or STREAM_ERR_XXXX
 */
UINT32 SvcFileStream_GetInitDefaultCfg(SVC_FILE_STREAM_INIT_CFG_s *Config)
{
    UINT32 Rval;
    Rval = SvcFileStream_GetInitDefCfg_ParamCheck(Config);
    if (Rval == STREAM_OK) {
        Rval = W2S(AmbaWrap_memset(Config, 0, sizeof(SVC_FILE_STREAM_INIT_CFG_s)));
        if (Rval == STREAM_OK) {
            Config->MaxHdlr = 1;
            Rval = SvcFileStream_GetInitBufferSize(Config->MaxHdlr, &Config->BufferSize);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Invalid argument!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 SvcFileStream_GetInitBufSize_ParamCheck(UINT8 MaxHdlr, const UINT32 *BufferSize)
{
    UINT32 Rval = STREAM_ERR_INVALID_ARG;
    if ((MaxHdlr > 0U)) {
        if (BufferSize != NULL) {
            Rval = STREAM_OK;
        }
    }
    return Rval;
}

/**
 * Get the required buffer size for initializing the File Stream module.
 *
 * @param [in] MaxHdlr The maximum number of handlers held in the File Stream module
 * @param [in] BufferSize The required buffer size
 * @return OK or STREAM_ERR_XXXX
 */
UINT32 SvcFileStream_GetInitBufferSize(UINT8 MaxHdlr, UINT32 *BufferSize)
{
    UINT32 Rval;
    Rval = SvcFileStream_GetInitBufSize_ParamCheck(MaxHdlr, BufferSize);
    if (Rval == STREAM_OK) {
        *BufferSize = GetAlignedValU32((UINT32)MaxHdlr * (UINT32)sizeof(SVC_FILE_STREAM_HDLR_s), (UINT32)AMBA_CACHE_LINE_SIZE);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Invalid argument!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

#define MAX_STREAM_NUM  (16U)   /**< Maximum number of stream handlers */
static UINT32 CreateMutex(void)
{
    static char MuxMgrMutexName[16] = "FileMgr";
    UINT32 Rval = K2S(AmbaKAL_MutexCreate(&g_FileMgr.Mutex, MuxMgrMutexName));
    if (Rval == STREAM_OK) {
        static char MutexName[MAX_STREAM_NUM][16];
        UINT32 i;
        for (i = 0; i < g_FileMgr.MaxHdlr; i++) {
            SVC_FILE_STREAM_HDLR_s *Hdlr = &g_FileMgr.Hdlr[i];
            UINT32 Len = AmbaUtility_StringPrintUInt32(MutexName[i], (UINT32)sizeof(MutexName[i]), "FileStr%02u", 1, &i);
            Rval = K2S(AmbaKAL_MutexCreate(&Hdlr->Mutex, MutexName[i]));
            if ((Rval != OK) || (i >= MAX_STREAM_NUM)) {
                AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Create handler's mutex failed!", __func__, NULL, NULL, NULL, NULL);
                if (Rval == STREAM_OK) {
                    Rval = STREAM_ERR_FATAL_ERROR;
                }
                break;
            }
            AmbaMisra_TouchUnused(&Len);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Create mutex failed!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 SvcFileStream_Init_ParamCheck(const SVC_FILE_STREAM_INIT_CFG_s *Config)
{
    UINT32 Rval = STREAM_ERR_INVALID_ARG;
    if ((Config != NULL) && (Config->MaxHdlr > 0U) && (Config->Buffer != NULL)) {
        Rval = STREAM_OK;
    }
    return Rval;
}

/**
 * Initialize the File Stream module.
 *
 * @param [in] Config The configuration used to initialize the File Stream module
 * @return OK or STREAM_ERR_XXXX
 */
UINT32 SvcFileStream_Init(const SVC_FILE_STREAM_INIT_CFG_s *Config)
{
    static UINT8 g_FileMgrInit = 0U;
    UINT32 Rval;
    Rval = SvcFileStream_Init_ParamCheck(Config);
    if (Rval == STREAM_OK) {
        UINT32 size;
        Rval = SvcFileStream_GetInitBufferSize(Config->MaxHdlr, &size);
        if (Rval == STREAM_OK) {
            if (Config->BufferSize == size) {
                if (g_FileMgrInit == 0U) {
                    SvcFileStream_InitOperation();
                    Rval = W2S(AmbaWrap_memset(&g_FileMgr, 0, sizeof(SVC_FILE_STREAM_MGR_s)));
                    if (Rval == STREAM_OK) {
                        Rval = W2S(AmbaWrap_memset(Config->Buffer, 0, Config->BufferSize));
                        if (Rval == STREAM_OK) {
                            AmbaMisra_TypeCast(&g_FileMgr.Hdlr, &Config->Buffer);
                            g_FileMgr.MaxHdlr = Config->MaxHdlr;
                            g_FileMgrInit = 1U;
                            Rval = CreateMutex();
                        }
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Already initiated!", __func__, NULL, NULL, NULL, NULL);
                    Rval = STREAM_ERR_FATAL_ERROR;
                }
            } else {
                AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s incorrect argument!", __func__, NULL, NULL, NULL, NULL);
                Rval = STREAM_ERR_INVALID_ARG;
            }
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s incorrect argument!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 SvcFileStream_GetDefaultCfg_ParamCheck(const SVC_FILE_STREAM_CFG_s *Config)
{
    UINT32 Rval = STREAM_ERR_INVALID_ARG;
    if (Config != NULL) {
        Rval = STREAM_OK;
    }
    return Rval;
}

/**
 * Get the default configuration of a file stream handler.
 *
 * @param [out] Config The returned configuration
 * @return OK or STREAM_ERR_XXXX
 */
UINT32 SvcFileStream_GetDefaultCfg(SVC_FILE_STREAM_CFG_s *Config)
{
    UINT32 Rval;
    Rval = SvcFileStream_GetDefaultCfg_ParamCheck(Config);
    if (Rval == STREAM_OK) {
        AMBA_CFS_FILE_PARAM_s Param;
        Rval = W2S(AmbaWrap_memset(Config, 0, sizeof(SVC_FILE_STREAM_CFG_s)));
        if (Rval == STREAM_OK) {
            Rval = C2S(AmbaCFS_GetFileParam(&Param));
            if (Rval == STREAM_OK) {
                Config->BytesToSync = Param.BytesToSync;
            } else {
                AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s AmbaCFS_GetFileParam() fail!", __func__, NULL, NULL, NULL, NULL);
            }
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Invalid argument!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static SVC_FILE_STREAM_HDLR_s *GetHdlr(void)
{
    UINT32 i;
    SVC_FILE_STREAM_HDLR_s *Hdlr = NULL;
    for (i = 0; i < g_FileMgr.MaxHdlr; i++) {
        Hdlr = &g_FileMgr.Hdlr[i];
        if (Hdlr->Used == 0U) {
            Hdlr->Used = 1U;
            break;
        }
    }
    if (i == g_FileMgr.MaxHdlr) {
        AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Get hdlr fail!", __func__, NULL, NULL, NULL, NULL);
        Hdlr = NULL;
    }
    return Hdlr;
}

static UINT32 RelHdlr(SVC_FILE_STREAM_HDLR_s *Hdlr)
{
    UINT32 Rval = STREAM_OK;
    UINT32 i;
    AmbaMisra_TouchUnused(Hdlr);
    for (i = 0; i < g_FileMgr.MaxHdlr; i++) {
        SVC_FILE_STREAM_HDLR_s *FileStr = &g_FileMgr.Hdlr[i];
        if (FileStr == Hdlr) {
            if (FileStr->Used == 1U) {
                FileStr->Used = 0U;
            } else {
                // error...
                AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s FileStr idle!", __func__, NULL, NULL, NULL, NULL);
                Rval = STREAM_ERR_FATAL_ERROR;
            }
            break;
        }
    }
    if (i == g_FileMgr.MaxHdlr) {
        AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Get Hdlr fail!", __func__, NULL, NULL, NULL, NULL);
        Rval = STREAM_ERR_FATAL_ERROR;
    }
    return Rval;
}

static UINT32 SvcFileStream_CreateImpl(const SVC_FILE_STREAM_CFG_s *Config, SVC_FILE_STREAM_HDLR_s **Hdlr)
{
    UINT32 Rval = STREAM_ERR_FATAL_ERROR;
    SVC_FILE_STREAM_HDLR_s *FileStr = GetHdlr();
    if (FileStr != NULL) {
        Rval = K2S(AmbaKAL_MutexTake(&FileStr->Mutex, SVC_FILE_WAIT_TIMEOUT));
        if (Rval == STREAM_OK) {
            FileStr->Hdlr.Func = &g_File;
            FileStr->File = NULL;
            FileStr->Alignment = Config->Alignment;
            FileStr->BytesToSync = Config->BytesToSync;
            FileStr->Async = Config->Async;
            FileStr->BankBuffer = Config->BankBuffer;
            FileStr->BankBufferSize = Config->BankBufferSize;
            if (AmbaKAL_MutexGive(&FileStr->Mutex) != OK) {
                AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Give mutex fail!", __func__, NULL, NULL, NULL, NULL);
                Rval = STREAM_ERR_FATAL_ERROR;
            }
        }
    }
    *Hdlr = FileStr;
    return Rval;
}

static UINT32 SvcFileStream_Create_ParamCheck(const SVC_FILE_STREAM_CFG_s *Config, SVC_STREAM_HDLR_s * const *Hdlr)
{
    UINT32 Rval = STREAM_ERR_INVALID_ARG;
    if ((Config != NULL) && (Hdlr != NULL)) {
        Rval = STREAM_OK;
    }
    return Rval;
}

/**
 * Create a file stream handler.
 *
 * @param [in] Config The configuration used to create a file stream handler
 * @param [out] Hdlr The returned handler
 * @return OK or STREAM_ERR_XXXX
 */
UINT32 SvcFileStream_Create(const SVC_FILE_STREAM_CFG_s *Config, SVC_STREAM_HDLR_s **Hdlr)
{
    UINT32 Rval;
    Rval = SvcFileStream_Create_ParamCheck(Config, Hdlr);
    if (Rval == STREAM_OK) {
        Rval = K2S(AmbaKAL_MutexTake(&g_FileMgr.Mutex, SVC_FILE_WAIT_TIMEOUT));
        if (Rval == STREAM_OK) {
            SVC_FILE_STREAM_HDLR_s *File;
            Rval = SvcFileStream_CreateImpl(Config, &File);
            if (Rval == STREAM_OK) {
                *Hdlr = &File->Hdlr;
            }
            if (AmbaKAL_MutexGive(&g_FileMgr.Mutex) != OK) {
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

static UINT32 SvcFileStream_DeleteImpl(SVC_FILE_STREAM_HDLR_s *Hdlr)
{
    UINT32 Rval = K2S(AmbaKAL_MutexTake(&Hdlr->Mutex, SVC_FILE_WAIT_TIMEOUT));
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

static UINT32 SvcFileStream_Delete_ParamCheck(const SVC_STREAM_HDLR_s *Hdlr)
{
    UINT32 Rval = STREAM_ERR_INVALID_ARG;
    if (Hdlr != NULL) {
        Rval = STREAM_OK;
    }
    return Rval;
}

/**
 * Delete a file stream handler.
 *
 * @param [in] Hdlr The handler of a file stream being removed
 * @return OK or STREAM_ERR_XXXX
 */
UINT32 SvcFileStream_Delete(SVC_STREAM_HDLR_s *Hdlr)
{
    UINT32 Rval;
    Rval = SvcFileStream_Delete_ParamCheck(Hdlr);
    if (Rval == STREAM_OK) {
        AmbaMisra_TouchUnused(Hdlr);
        Rval = K2S(AmbaKAL_MutexTake(&g_FileMgr.Mutex, SVC_FILE_WAIT_TIMEOUT));
        if (Rval == STREAM_OK) {
            SVC_FILE_STREAM_HDLR_s *FileStr;
            AmbaMisra_TypeCast(&FileStr, &Hdlr);
            Rval = SvcFileStream_DeleteImpl(FileStr);
            if (AmbaKAL_MutexGive(&g_FileMgr.Mutex) != OK) {
                AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Give mutex fail!", __func__, NULL, NULL, NULL, NULL);
                Rval = STREAM_ERR_FATAL_ERROR;
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Take mutex fail!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Invalid handler!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 SvcFileStream_Open_ParamCheck(const SVC_STREAM_HDLR_s *Hdlr, const char *Name, UINT32 Mode)
{
    UINT32 Rval = STREAM_ERR_INVALID_ARG;
    if (Hdlr != NULL) {
        if (Name != NULL) {
            if ((Mode > 0U) && (Mode <= SVC_STREAM_MODE_WRRD)) {
                Rval = STREAM_OK;
            }
        }
    }
    return Rval;
}

static UINT32 SvcFileStream_Open(SVC_STREAM_HDLR_s *Hdlr, const char *Name, UINT32 Mode)
{
    UINT32 Rval;
    Rval = SvcFileStream_Open_ParamCheck(Hdlr, Name, Mode);
    if (Rval == STREAM_OK) {
        SVC_FILE_STREAM_HDLR_s *FileStr;
        AmbaMisra_TouchUnused(Hdlr);
        AmbaMisra_TypeCast(&FileStr, &Hdlr);
        Rval = K2S(AmbaKAL_MutexTake(&FileStr->Mutex, SVC_FILE_WAIT_TIMEOUT));
        if (Rval == STREAM_OK) {
            AMBA_CFS_FILE_PARAM_s Param;
            Rval = C2S(AmbaCFS_GetFileParam(&Param));
            if (Rval == STREAM_OK) {
                AmbaUtility_StringCopy(Param.Filename, AMBA_CFS_MAX_FILENAME_LENGTH, Name);
                Param.Filename[AMBA_CFS_MAX_FILENAME_LENGTH - 1U] = '\0';
                if (FileStr->Alignment == 0U) {
                    Param.AlignMode = AMBA_CFS_ALIGN_MODE_NONE;
                } else {
                    Param.AlignMode = AMBA_CFS_ALIGN_MODE_SIZE;
                }
                Param.AlignSize = FileStr->Alignment;
                Param.BytesToSync = FileStr->BytesToSync;
                Param.AsyncEnable = FileStr->Async;
                Param.BankBuffer = FileStr->BankBuffer;
                Param.BankBufferSize = FileStr->BankBufferSize;
                if (Param.AsyncEnable != 0U) {
                    Param.DmaEnable = 1U;    // always enable DMA for conta
                }
                switch (Mode) {
                case SVC_STREAM_MODE_RDONLY:
                    Param.Mode = AMBA_CFS_FILE_MODE_READ_ONLY;
                    Rval = C2S(AmbaCFS_FileOpen(&Param, &FileStr->File));
                    break;
                case SVC_STREAM_MODE_WRONLY:
                    Param.Mode = AMBA_CFS_FILE_MODE_WRITE_ONLY;
                    Rval = C2S(AmbaCFS_FileOpen(&Param, &FileStr->File));
                    break;
                case SVC_STREAM_MODE_RDWR:
                    Param.Mode = AMBA_CFS_FILE_MODE_READ_WRITE;
                    Rval = C2S(AmbaCFS_FileOpen(&Param, &FileStr->File));
                    break;
                case SVC_STREAM_MODE_WRRD:
                    Param.Mode = AMBA_CFS_FILE_MODE_WRITE_READ;
                    Rval = C2S(AmbaCFS_FileOpen(&Param, &FileStr->File));
                    break;
                default:
                    FileStr->File = NULL;
                    Rval = STREAM_ERR_INVALID_ARG;
                    break;
                }
                if (Rval == STREAM_OK) {
                    FileStr->Mode = (UINT8)Mode;
                    AmbaUtility_StringCopy(FileStr->URL, AMBA_CFS_MAX_FILENAME_LENGTH, Name);
                    FileStr->URL[AMBA_CFS_MAX_FILENAME_LENGTH - 1U] = '\0';
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s File is null!", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s AmbaCFS_GetFileParam() failed!", __func__, NULL, NULL, NULL, NULL);
            }
            if (AmbaKAL_MutexGive(&FileStr->Mutex) != OK) {
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

static UINT32 SvcFileStream_Close_ParamCheck(const SVC_STREAM_HDLR_s *Hdlr)
{
    UINT32 Rval = STREAM_ERR_INVALID_ARG;
    if (Hdlr != NULL) {
        Rval = STREAM_OK;
    }
    return Rval;
}

static UINT32 SvcFileStream_Close(SVC_STREAM_HDLR_s *Hdlr)
{
    UINT32 Rval;
    Rval = SvcFileStream_Close_ParamCheck(Hdlr);
    if (Rval == STREAM_OK) {
        SVC_FILE_STREAM_HDLR_s *FileStr;
        AmbaMisra_TouchUnused(Hdlr);
        AmbaMisra_TypeCast(&FileStr, &Hdlr);
        Rval = K2S(AmbaKAL_MutexTake(&FileStr->Mutex, SVC_FILE_WAIT_TIMEOUT));
        if (Rval == STREAM_OK) {
            if (FileStr->File != NULL)  {  // could be NULL due to cinsert failure
                Rval = C2S(AmbaCFS_FileClose(FileStr->File));
                if (Rval == STREAM_OK) {
                    FileStr->File = NULL;
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s AmbaCFS_FileClose() failed!", __func__, NULL, NULL, NULL, NULL);
                }
            }
            if (AmbaKAL_MutexGive(&FileStr->Mutex) != OK) {
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

static UINT32 SvcFileStream_Read_ParamCheck(const SVC_STREAM_HDLR_s *Hdlr, UINT32 Size, const UINT8 *Buffer, const UINT32 *Count)
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

static UINT32 SvcFileStream_Read(SVC_STREAM_HDLR_s *Hdlr, UINT32 Size, UINT8 *Buffer, UINT32 *Count)
{
    UINT32 Rval;
    Rval = SvcFileStream_Read_ParamCheck(Hdlr, Size, Buffer, Count);
    if (Rval == STREAM_OK) {
        SVC_FILE_STREAM_HDLR_s *FileStr;
        AmbaMisra_TouchUnused(Hdlr);
        AmbaMisra_TypeCast(&FileStr, &Hdlr);
        Rval = K2S(AmbaKAL_MutexTake(&FileStr->Mutex, SVC_FILE_WAIT_TIMEOUT));
        if (Rval == STREAM_OK) {
            Rval = C2S(AmbaCFS_FileRead(Buffer, 1, Size, FileStr->File, Count));
            if (Rval != STREAM_OK) {
                AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s AmbaCFS_FileRead() failed!", __func__, NULL, NULL, NULL, NULL);
            }
            if (AmbaKAL_MutexGive(&FileStr->Mutex) != OK) {
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

static UINT32 SvcFileStream_Write_ParamCheck(const SVC_STREAM_HDLR_s *Hdlr, UINT32 Size, const UINT8 *Buffer, const UINT32 *Count)
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

static UINT32 SvcFileStream_Write(SVC_STREAM_HDLR_s *Hdlr, UINT32 Size, UINT8 *Buffer, UINT32 *Count)
{
    UINT32 Rval;
    Rval = SvcFileStream_Write_ParamCheck(Hdlr, Size, Buffer, Count);
    if (Rval == STREAM_OK) {
        SVC_FILE_STREAM_HDLR_s *FileStr;
        AmbaMisra_TouchUnused(Hdlr);
        AmbaMisra_TypeCast(&FileStr, &Hdlr);
        Rval = K2S(AmbaKAL_MutexTake(&FileStr->Mutex, SVC_FILE_WAIT_TIMEOUT));
        if (Rval == STREAM_OK) {
            Rval = C2S(AmbaCFS_FileWrite(Buffer, 1, Size, FileStr->File, Count));
            if (Rval == STREAM_OK) {
                if (*Count != Size) {
                    AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s AmbaCFS_FileWrite() failed!", __func__, NULL, NULL, NULL, NULL);
                    Rval = STREAM_ERR_IO_ERROR;
                }
            } else {
                AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s AmbaCFS_FileWrite() failed!", __func__, NULL, NULL, NULL, NULL);
            }
            if (AmbaKAL_MutexGive(&FileStr->Mutex) != OK) {
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

static UINT32 SvcFileStream_GetPos_ParamCheck(const SVC_STREAM_HDLR_s *Hdlr, const UINT64 *Pos)
{
    UINT32 Rval = STREAM_ERR_INVALID_ARG;
    if (Hdlr != NULL) {
        if (Pos != NULL) {
            Rval = STREAM_OK;
        }
    }
    return Rval;
}

static UINT32 SvcFileStream_GetPos(SVC_STREAM_HDLR_s *Hdlr, UINT64 *Pos)
{
    UINT32 Rval;
    Rval = SvcFileStream_GetPos_ParamCheck(Hdlr, Pos);
    if (Rval == STREAM_OK) {
        SVC_FILE_STREAM_HDLR_s *FileStr;
        AmbaMisra_TouchUnused(Hdlr);
        AmbaMisra_TypeCast(&FileStr, &Hdlr);
        Rval = K2S(AmbaKAL_MutexTake(&FileStr->Mutex, SVC_FILE_WAIT_TIMEOUT));
        if (Rval == STREAM_OK) {
            Rval = C2S(AmbaCFS_FileTell(FileStr->File, Pos));
            if (Rval != STREAM_OK) {
                AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s AmbaCFS_ftell() failed!", __func__, NULL, NULL, NULL, NULL);
            }
            if (AmbaKAL_MutexGive(&FileStr->Mutex) != OK) {
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

static UINT32 SvcFileStream_GetLength_ParamCheck(const SVC_STREAM_HDLR_s *Hdlr, const UINT64 *Size)
{
    UINT32 Rval = STREAM_ERR_INVALID_ARG;
    if (Hdlr != NULL) {
        if (Size != NULL) {
            Rval = STREAM_OK;
        }
    }
    return Rval;
}

static UINT32 SvcFileStream_GetLength(SVC_STREAM_HDLR_s *Hdlr, UINT64 *Size)
{
    UINT32 Rval;
    Rval = SvcFileStream_GetLength_ParamCheck(Hdlr, Size);
    if (Rval == STREAM_OK) {
        SVC_FILE_STREAM_HDLR_s *FileStr;
        AmbaMisra_TouchUnused(Hdlr);
        AmbaMisra_TypeCast(&FileStr, &Hdlr);
        Rval = K2S(AmbaKAL_MutexTake(&FileStr->Mutex, SVC_FILE_WAIT_TIMEOUT));
        if (Rval == STREAM_OK) {
            Rval = C2S(AmbaCFS_FileGetLen(FileStr->File, Size));
            if (Rval != STREAM_OK) {
                AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s AmbaCFS_FGetLen() failed!", __func__, NULL, NULL, NULL, NULL);
            }
            if (AmbaKAL_MutexGive(&FileStr->Mutex) != OK) {
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

static UINT32 SvcFileStream_GetFreeSpace_ParamCheck(const SVC_STREAM_HDLR_s *Hdlr, const UINT64 *Size)
{
    UINT32 Rval = STREAM_ERR_INVALID_ARG;
    if (Hdlr != NULL) {
        if (Size != NULL) {
            Rval = STREAM_OK;
        }
    }
    return Rval;
}

static UINT32 SvcFileStream_GetFreeSpace(SVC_STREAM_HDLR_s *Hdlr, UINT64 *Size)
{
    UINT32 Rval;
    AMBA_CFS_DRIVE_INFO_s DevInf;
    Rval = SvcFileStream_GetFreeSpace_ParamCheck(Hdlr, Size);
    if (Rval == STREAM_OK) {
        const SVC_FILE_STREAM_HDLR_s *FileStr;
        AmbaMisra_TouchUnused(Hdlr);
        AmbaMisra_TypeCast(&FileStr, &Hdlr);
        Rval = C2S(AmbaCFS_GetDriveInfo(FileStr->URL[0], &DevInf));
        if (Rval == STREAM_OK) {
            const UINT64 Space = (UINT64)DevInf.EmptyClusters * DevInf.SectorsPerCluster * DevInf.BytesPerSector;
            UINT64 Cached;
            Rval = C2S(AmbaCFS_GetCachedDataSize(FileStr->URL[0], &Cached));
            if (Rval == STREAM_OK) {
                *Size = Space - Cached;
            } else {
                AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s AmbaCFS_GetCachedDataSize() failed!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s AmbaCFS_GetDriveInfo() fail!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s Invalid argument!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 SvcFileStream_Seek_ParamCheck(const SVC_STREAM_HDLR_s *Hdlr, INT64 Pos, INT32 Orig)
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

static UINT32 SvcFileStream_Seek(SVC_STREAM_HDLR_s *Hdlr, INT64 Pos, INT32 Orig)
{
    UINT32 Rval;
    Rval = SvcFileStream_Seek_ParamCheck(Hdlr, Pos, Orig);
    if (Rval == STREAM_OK) {
        SVC_FILE_STREAM_HDLR_s *FileStr;
        AmbaMisra_TouchUnused(Hdlr);
        AmbaMisra_TypeCast(&FileStr, &Hdlr);
        Rval = K2S(AmbaKAL_MutexTake(&FileStr->Mutex, SVC_FILE_WAIT_TIMEOUT));
        if (Rval == STREAM_OK) {
            Rval = C2S(AmbaCFS_FileSeek(FileStr->File, Pos, Orig));
            if (AmbaKAL_MutexGive(&FileStr->Mutex) != OK) {
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

static UINT32 SvcFileStream_Sync_ParamCheck(const SVC_STREAM_HDLR_s *Hdlr)
{
    UINT32 Rval = STREAM_ERR_INVALID_ARG;
    if (Hdlr != NULL) {
        Rval = STREAM_OK;
    }
    return Rval;
}

static UINT32 SvcFileStream_Sync(SVC_STREAM_HDLR_s *Hdlr)
{
    UINT32 Rval;
    Rval = SvcFileStream_Sync_ParamCheck(Hdlr);
    if (Rval == STREAM_OK) {
        SVC_FILE_STREAM_HDLR_s *FileStr;
        AmbaMisra_TouchUnused(Hdlr);
        AmbaMisra_TypeCast(&FileStr, &Hdlr);
        Rval = K2S(AmbaKAL_MutexTake(&FileStr->Mutex, SVC_FILE_WAIT_TIMEOUT));
        if (Rval == STREAM_OK) {
            Rval = C2S(AmbaCFS_FileSync(FileStr->File));
            if (Rval != STREAM_OK) {
                AmbaPrint_ModulePrintStr5(SVC_STREAM_PRINT_MODULE_ID, "%s AmbaCFS_FileSync() failed!", __func__, NULL, NULL, NULL, NULL);
            }
            if (AmbaKAL_MutexGive(&FileStr->Mutex) != OK) {
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

static UINT32 SvcFileStream_FuncImpl(SVC_FILE_STREAM_HDLR_s *Hdlr, UINT32 Cmd, UINT32 Param)
{
    UINT32 Rval = STREAM_ERR_FATAL_ERROR;
    AmbaPrint_ModulePrintUInt5(SVC_STREAM_PRINT_MODULE_ID, "(%x, %x) : unsupported!!", Cmd, Param, 0U, 0U, 0U);
    AmbaMisra_TouchUnused(Hdlr);
    return Rval;
}

static UINT32 SvcFileStream_Func_ParamCheck(const SVC_STREAM_HDLR_s *Hdlr, UINT32 Cmd, UINT32 Param)
{
    UINT32 Rval = STREAM_ERR_INVALID_ARG;
    if (Hdlr != NULL) {
        Rval = STREAM_OK;
    }
    AmbaMisra_TouchUnused(&Cmd);
    AmbaMisra_TouchUnused(&Param);
    return Rval;
}

static UINT32 SvcFileStream_Func(SVC_STREAM_HDLR_s *Hdlr, UINT32 Cmd, UINT32 Param)
{
    UINT32 Rval;
    Rval = SvcFileStream_Func_ParamCheck(Hdlr, Cmd, Param);
    if (Rval == STREAM_OK) {
        SVC_FILE_STREAM_HDLR_s *FileStr;
        AmbaMisra_TouchUnused(Hdlr);
        AmbaMisra_TypeCast(&FileStr, &Hdlr);
        Rval = K2S(AmbaKAL_MutexTake(&FileStr->Mutex, SVC_FILE_WAIT_TIMEOUT));
        if (Rval == STREAM_OK) {
            Rval = SvcFileStream_FuncImpl(FileStr, Cmd, Param);
            if (AmbaKAL_MutexGive(&FileStr->Mutex) != OK) {
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

static void SvcFileStream_InitOperation(void)
{
    g_File.Open = SvcFileStream_Open;
    g_File.Close = SvcFileStream_Close;
    g_File.Read = SvcFileStream_Read;
    g_File.Write = SvcFileStream_Write;
    g_File.Seek = SvcFileStream_Seek;
    g_File.GetPos = SvcFileStream_GetPos;
    g_File.GetLength = SvcFileStream_GetLength;
    g_File.GetFreeSpace = SvcFileStream_GetFreeSpace;
    g_File.Sync = SvcFileStream_Sync;
    g_File.Func = SvcFileStream_Func;
}

