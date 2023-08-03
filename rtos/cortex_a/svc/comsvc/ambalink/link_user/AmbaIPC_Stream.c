/**
 * @file AmbaIPC_Stream.c
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
 *  @details Test/Reference code for AmbaIPC
\*-------------------------------------------------------------------------------------------------------------------*/
#include "AmbaIntrinsics.h"
#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaINT.h"
#include "AmbaCache.h"
#include "AmbaLink.h"
#include "AmbaLinkPrivate.h"
#include "AmbaIPC_List.h"
#include "AmbaIPC_Rpc.h"
#include "AmbaPrint.h"
#include "AmbaUtility.h"
#include "AmbaMisraFix.h"
#include "AmbaMMU.h"
#include "AmbaShell_Commands.h"
#include "libwrapc.h"
#include "stream/SvcStream.h"
#include "AmbaIPC_RpcProg_LU_Stream.h"
#include "AmbaIPC_Stream.h"

//#pragma GCC optimize ("O0")

#define USER_PROC_INFO          1024U
#define RPC_STREAM_HANDLE_NUM   16U

#define SVC_OK      (0x00000000U)

#define TRACE_IN() //AmbaPrint_PrintStr5("=> InTo: %s",__FUNCTION__,NULL,NULL,NULL,NULL)
#define TRACE_OUT() //AmbaPrint_PrintStr5("<= Leave: %s",__FUNCTION__,NULL,NULL,NULL,NULL)

extern AMBA_LINK_CVSHM_CTRL_s AmbaLinkCvShmCtrl;

typedef struct{
    /** Used */
    UINT32              Used;
    /** Fd */
    UINT32              Fd;
    /** Handler */
    SVC_STREAM_HDLR_s   Hdlr;
} IPC_STREAM_HANDLE_s;

static CLIENT_ID_t RpcStreamClient = NULL;
static IPC_STREAM_HANDLE_s RpcStreamHandle[RPC_STREAM_HANDLE_NUM];
static SVC_STREAM_s RpvStreamFun;

static UINT32 AmbaIPC_StreamGetFdByHandle(SVC_STREAM_HDLR_s *pHdlr, UINT32 *pFd)
{
    UINT32 ret = STREAM_ERR_0001, i;

    for(i = 0;i < RPC_STREAM_HANDLE_NUM;i++){
        if( (RpcStreamHandle[i].Used == 1U) && (pHdlr == &(RpcStreamHandle[i].Hdlr))) {
            *pFd = RpcStreamHandle[i].Fd;
            ret = SVC_OK;
            break;
        }
    }

    if(ret != SVC_OK) {
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaIPC_Stream_GetFdByHandle not found!", 0U, 0U, 0U, 0U, 0U);
    }

    return ret;
}

static UINT32 AmbaIPC_StreamGetPos(SVC_STREAM_HDLR_s *Hdlr, UINT64 *Pos)
{
    UINT32 ret = SVC_OK, Fd = 0U;
    AMBA_IPC_REPLY_STATUS_e status;
    AMBA_RPC_LU_STREAM_GETPOS_s In;

    TRACE_IN();

    if (AmbaIPC_StreamGetFdByHandle(Hdlr, &Fd) != 0U) {
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaIPC_Stream_GetPos IpcStream_GetFdByHandle fail (%d).", 0U, 0U, 0U, 0U, 0U);
        ret = STREAM_ERR_0001;
    } else {
        In.Fd  = Fd;
        status = AmbaIPC_ClientCall(RpcStreamClient, AMBA_RPC_PROG_LU_STREAM_GETPOS,
                                (void *) &In, sizeof(AMBA_RPC_LU_STREAM_GETPOS_s), Pos, sizeof(UINT64), 5000);
        if(status != AMBA_IPC_REPLY_SUCCESS) {
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaIPC_Stream_GetPos AmbaIPC_ClientCall fail (%d).", status, 0U, 0U, 0U, 0U);
            ret = STREAM_ERR_0002;
        }
    }

    TRACE_OUT();

    return ret;
}

static UINT32 AmbaIPC_StreamSeek(SVC_STREAM_HDLR_s *Hdlr, INT64 Pos, INT32 Orig)
{
    UINT32 ret = SVC_OK, Fd = 0U;
    AMBA_IPC_REPLY_STATUS_e status;
    AMBA_RPC_LU_STREAM_SEEK_s In;

    TRACE_IN();

    if (AmbaIPC_StreamGetFdByHandle(Hdlr, &Fd) != 0U) {
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaIPC_Stream_Seek IpcStream_GetFdByHandle fail (%d).", 0U, 0U, 0U, 0U, 0U);
        ret = STREAM_ERR_0001;
    } else {
        In.Fd  = Fd;
        In.Pos = Pos;
        if(Orig == SVC_STREAM_SEEK_START) {
            In.Orig = AMBA_RPC_PROG_LU_SEEK_START;
        } else if(Orig == SVC_STREAM_SEEK_CUR) {
            In.Orig = AMBA_RPC_PROG_LU_SEEK_CUR;
        } else if(Orig == SVC_STREAM_SEEK_END) {
            In.Orig = AMBA_RPC_PROG_LU_SEEK_END;
        } else {
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaIPC_StreamSeek fail Orig(%d) not support.", Orig, 0U, 0U, 0U, 0U);
            ret = STREAM_ERR_0001;
        }

        if(ret == 0U) {
            status = AmbaIPC_ClientCall(RpcStreamClient, AMBA_RPC_PROG_LU_STREAM_SEEK,
                                (void *) &In, sizeof(AMBA_RPC_LU_STREAM_SEEK_s), NULL, 0, 5000);
            if(status != AMBA_IPC_REPLY_SUCCESS) {
                AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaIPC_Stream_Seek AmbaIPC_ClientCall fail (%d).", status, 0U, 0U, 0U, 0U);
                ret = STREAM_ERR_0002;
            }
        }
    }

    TRACE_OUT();

    return ret;
}

static UINT32 GetLength(SVC_STREAM_HDLR_s *Hdlr, UINT64 *Size)
{
    UINT32 Err, Rval = SVC_OK;
    UINT64 CurPos, EndPos;

    TRACE_IN();

    Err = AmbaIPC_StreamGetPos(Hdlr, &CurPos);
    if (Err != AMBA_FS_ERR_NONE) {
        Rval = STREAM_ERR_0002;
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID,"GetLength: GetPos(CurPos) failed %u", Err, 0U, 0U, 0U, 0U);
    } else {
        Err = AmbaIPC_StreamSeek(Hdlr, 0UL, SVC_STREAM_SEEK_END);
        if (Err != AMBA_FS_ERR_NONE) {
            Rval = STREAM_ERR_0002;
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID,"GetLength: Seek(End) failed %u", Err, 0U, 0U, 0U, 0U);
        } else {
            Err = AmbaIPC_StreamGetPos(Hdlr, &EndPos);
            if (Err != AMBA_FS_ERR_NONE) {
                Rval = STREAM_ERR_0002;
                AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID,"GetLength: GetPos(End) failed %u", Err, 0U, 0U, 0U, 0U);
            } else {
                Err = AmbaIPC_StreamSeek(Hdlr, CurPos, SVC_STREAM_SEEK_START);
                if (Err != AMBA_FS_ERR_NONE) {
                    Rval = STREAM_ERR_0002;
                    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID,"GetLength: Seek(CurPos) failed %u", Err, 0U, 0U, 0U, 0U);
                } else {
                    *Size = EndPos;
                    //AmbaPrint_PrintUInt5("GetLength: %llu", *Size, 0U, 0U, 0U, 0U);
                }
            }
        }
    }

    TRACE_OUT();

    return Rval;
}

static UINT32 Write(SVC_STREAM_HDLR_s *Hdlr, UINT32 Size, UINT8 *Buffer, UINT32 *Count)
{
    UINT32  Rval = STREAM_ERR_0000;

    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID,"AmbaIPC_Stream: Write is not supported!",0U,0U,0U,0U,0U);

    AmbaMisra_TouchUnused(Hdlr);
    AmbaMisra_TouchUnused(&Size);
    AmbaMisra_TouchUnused(Buffer);
    AmbaMisra_TouchUnused(Count);

    return Rval;
}

static UINT32 GetFreeSpace(SVC_STREAM_HDLR_s *Hdlr, UINT64 *Size)
{
    UINT32  Rval = STREAM_ERR_0000;

    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID,"AmbaIPC_Stream: GetFreeSpace is not supported!",0U,0U,0U,0U,0U);

    AmbaMisra_TouchUnused(Hdlr);
    AmbaMisra_TouchUnused(Size);

    return Rval;
}

static UINT32 Sync(SVC_STREAM_HDLR_s *Hdlr)
{
    UINT32  Rval = STREAM_ERR_0000;

    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID,"AmbaIPC_Stream: Sync is not supported!",0U,0U,0U,0U,0U);

    AmbaMisra_TouchUnused(Hdlr);

    return Rval;
}

static UINT32 AmbaIPC_StreamMap(SVC_STREAM_HDLR_s  *pHdlr, UINT32 Fd)
{
    UINT32 ret = STREAM_ERR_0001, i;

    TRACE_IN();

    for(i = 0;i < RPC_STREAM_HANDLE_NUM;i++){
        if( (RpcStreamHandle[i].Used == 1U) && (pHdlr == &(RpcStreamHandle[i].Hdlr))) {
            if(RpcStreamHandle[i].Fd != (RPC_STREAM_HANDLE_NUM+1U)) {
                AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaIPC_Stream_Map twice Fd(%d) Ori Fd(%d)", Fd, RpcStreamHandle[i].Fd, 0U, 0U, 0U);
            }
            RpcStreamHandle[i].Fd = Fd;
            ret = SVC_OK;
            break;
        }
    }

    if(ret != SVC_OK) {
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaIPC_Stream_Map not found!", 0U, 0U, 0U, 0U, 0U);
    }

    TRACE_OUT();

    return ret;
}

static UINT32 AmbaIPC_StreamUnmap(SVC_STREAM_HDLR_s *pHdlr)
{
    UINT32 ret = STREAM_ERR_0001, i;

    TRACE_IN();

    for(i = 0;i < RPC_STREAM_HANDLE_NUM;i++){
        if( (RpcStreamHandle[i].Used == 1U) && (pHdlr == &(RpcStreamHandle[i].Hdlr))) {
            if(RpcStreamHandle[i].Fd == (RPC_STREAM_HANDLE_NUM+1U)) {
                AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaIPC_Stream_Unmap twice!", 0U, 0U, 0U, 0U, 0U);
            }
            RpcStreamHandle[i].Fd = (RPC_STREAM_HANDLE_NUM+1U);
            ret = SVC_OK;
            break;
        }
    }

    if(ret != SVC_OK) {
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaIPC_Stream_Unmap not found!", 0U, 0U, 0U, 0U, 0U);
    }

    TRACE_OUT();

    return ret;
}

static UINT32 AmbaIPC_StreamOpen(SVC_STREAM_HDLR_s *Hdlr, const char *Name, UINT32 Mode)
{
    UINT32 ret = SVC_OK, Fd;
    AMBA_IPC_REPLY_STATUS_e status;
    AMBA_RPC_LU_STREAM_OPEN_s In;

    TRACE_IN();

    if(RpcStreamClient == NULL) {
        RpcStreamClient = AmbaIPC_ClientCreate(AMBA_RPC_PROG_LU_STREAM_HOST,
                        AMBA_RPC_PROG_LU_STREAM_PROG_ID,
                        AMBA_RPC_PROG_LU_STREAM_VER);
        if(RpcStreamClient == NULL) {
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaIPC_Stream_Open AmbaIPC_ClientCreate fail.", 0U, 0U, 0U, 0U, 0U);
            ret = STREAM_ERR_0001;
        }
    }

    if(ret == SVC_OK) {
        AmbaWrap_memcpy(In.Name, Name, sizeof(In.Name));
        if(Mode == SVC_STREAM_MODE_RDONLY) {
            In.Mode = AMBA_RPC_PROG_LU_MODE_RDONLY;
        } else if(Mode == SVC_STREAM_MODE_WRONLY) {
            In.Mode = AMBA_RPC_PROG_LU_MODE_WRONLY;
        } else if( (Mode == SVC_STREAM_MODE_RDWR) || (Mode == SVC_STREAM_MODE_WRRD)) {
            In.Mode = AMBA_RPC_PROG_LU_MODE_RDWR;
        } else {
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaIPC_Stream_Open fail mode(%d) not support.", Mode, 0U, 0U, 0U, 0U);
            ret = STREAM_ERR_0001;
        }

        if(ret == SVC_OK) {
            status = AmbaIPC_ClientCall(RpcStreamClient, AMBA_RPC_PROG_LU_STREAM_OPEN,
                            (void *) &In, sizeof(AMBA_RPC_LU_STREAM_OPEN_s), &Fd, sizeof(Fd), 5000);
            if(status != AMBA_IPC_REPLY_SUCCESS) {
                AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaIPC_Stream_Open AmbaIPC_ClientCall fail (%d).", status, 0U, 0U, 0U, 0U);
                ret = STREAM_ERR_0002;
            } else {
                //AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaIPC_StreamOpen fd (%d).", Fd, 0U, 0U, 0U, 0U);
                AmbaIPC_StreamMap(Hdlr, Fd);
            }
        }
    }

    TRACE_OUT();

    return ret;
}

static UINT32 AmbaIPC_StreamClose(SVC_STREAM_HDLR_s *Hdlr)
{
    UINT32 ret = SVC_OK, Fd = 0U;
    AMBA_IPC_REPLY_STATUS_e status;
    AMBA_RPC_LU_STREAM_CLOSE_s In;

    TRACE_IN();

    if (AmbaIPC_StreamGetFdByHandle(Hdlr, &Fd) != 0U) {
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaIPC_Stream_Close IpcStream_GetFdByHandle fail.", 0U, 0U, 0U, 0U, 0U);
        ret = STREAM_ERR_0001;
    } else {
        In.Fd  = Fd;
        status = AmbaIPC_ClientCall(RpcStreamClient, AMBA_RPC_PROG_LU_STREAM_CLOSE,
                                (void *) &In, sizeof(AMBA_RPC_LU_STREAM_CLOSE_s), NULL, 0, 5000);
        if(status != AMBA_IPC_REPLY_SUCCESS) {
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaIPC_Stream_Close AmbaIPC_ClientCall fail (%d).", status, 0U, 0U, 0U, 0U);
            ret = STREAM_ERR_0002;
        } else {
            AmbaIPC_StreamUnmap(Hdlr);
        }
    }

    TRACE_OUT();

    return ret;
}

static UINT32 AmbaIPC_StreamRead(SVC_STREAM_HDLR_s *Hdlr, UINT32 Size, UINT8 *Buffer, UINT32 *pCount)
{
    UINT32 ret = SVC_OK, VirtAddr, ReadCount = 0, Fd = 0U, PA_Buffer = 0U;
    AMBA_IPC_REPLY_STATUS_e status;
    AMBA_RPC_LU_STREAM_READ_s In;
    UINT8 *Result;

    TRACE_IN();

    if (AmbaIPC_StreamGetFdByHandle(Hdlr, &Fd) != 0U) {
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaIPC_Stream_Read IpcStream_GetFdByHandle fail", 0U, 0U, 0U, 0U, 0U);
        ret = STREAM_ERR_0001;
    } else if (Size > AmbaLinkCvShmCtrl.AppSharedMemSize) {
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaIPC_Stream_Read Size(%u) is larger than shmem size(%u)", Size, AmbaLinkCvShmCtrl.AppSharedMemSize, 0U, 0U, 0U);
        ret = STREAM_ERR_0001;
    } else {
        AmbaMisra_TypeCast32(&VirtAddr, &AmbaLinkCvShmCtrl.AppSharedMemAddr);
        if( AmbaMMU_VirtToPhys((ULONG)VirtAddr, (ULONG *)&PA_Buffer) != 0U) {
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaIPC_Stream_Read AmbaMMU_VirtToPhys fail Buffer = (%d)", (UINT32)Buffer, 0U, 0U, 0U, 0U);
            ret = STREAM_ERR_0001;
        } else {
            In.Size      = Size;
            In.Fd        = Fd;
            In.PA_Buffer = PA_Buffer;
            status = AmbaIPC_ClientCall(RpcStreamClient, AMBA_RPC_PROG_LU_STREAM_READ,
                                        (void *) &In, sizeof(AMBA_RPC_LU_STREAM_READ_s), &ReadCount, sizeof(UINT32), 5000);
            if(status != AMBA_IPC_REPLY_SUCCESS) {
                AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaIPC_Stream_Read AmbaIPC_ClientCall fail (%d)", status, 0U, 0U, 0U, 0U);
                ret = STREAM_ERR_0002;
            } else {
                if (ReadCount != 0) {
                    Result = NULL;
                    if (AmbaWrap_memcpy(&Result, &AmbaLinkCvShmCtrl.AppSharedMemAddr, sizeof(AmbaLinkCvShmCtrl.AppSharedMemAddr))!= 0U) { }
                    memcpy(Buffer, Result, ReadCount);
                    //AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaIPC_StreamRead pCount (0x%x). %02x %02x %02x %02x", ReadCount, Buffer[0], Buffer[1], Buffer[2], Buffer[3]);
                } else {
                    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaIPC_StreamRead pCount(0x%x).", ReadCount, 0U, 0U, 0U, 0U);
                }
            }
            *pCount = ReadCount;
        }
    }

    TRACE_OUT();

    return ret;
}

UINT32 AmbaIPC_StreamInit(void)
{
    UINT32 ret = SVC_OK, i;

    RpvStreamFun.Open = AmbaIPC_StreamOpen;
    RpvStreamFun.Close = AmbaIPC_StreamClose;
    RpvStreamFun.Read = AmbaIPC_StreamRead;
    RpvStreamFun.Seek = AmbaIPC_StreamSeek;
    RpvStreamFun.GetPos = AmbaIPC_StreamGetPos;
    RpvStreamFun.GetLength = GetLength;
    RpvStreamFun.Write = Write;
    RpvStreamFun.GetFreeSpace = GetFreeSpace;
    RpvStreamFun.Sync = Sync;
    RpvStreamFun.Func = NULL;
    for(i = 0;i < RPC_STREAM_HANDLE_NUM;i++){
        RpcStreamHandle[i].Fd = (RPC_STREAM_HANDLE_NUM+1U);
        RpcStreamHandle[i].Used = 0U;
        RpcStreamHandle[i].Hdlr.Func = &RpvStreamFun;
    }

    AmbaPrint_PrintUInt5("AppSharedMemAddr=0x%08x, AppSharedMemSize=0x%08x",AmbaLinkCvShmCtrl.AppSharedMemAddr,AmbaLinkCvShmCtrl.AppSharedMemSize,0,0,0);

    return ret;
}


UINT32 AmbaIPC_StreamCreate(void **ppvHdlr)
{
    UINT32 ret = STREAM_ERR_0001, i;
    SVC_STREAM_HDLR_s  **ppHdlr;

    AmbaMisra_TypeCast(&ppHdlr, &ppvHdlr);

    TRACE_IN();

    for(i = 0;i < RPC_STREAM_HANDLE_NUM;i++){
        if(RpcStreamHandle[i].Used == 0U) {
            RpcStreamHandle[i].Used = 1U;
            *ppHdlr = &(RpcStreamHandle[i].Hdlr);
            ret = SVC_OK;
            break;
        }
    }

    if(ret != SVC_OK) {
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaIPC_Stream_Create not found!", 0U, 0U, 0U, 0U, 0U);
    }

    TRACE_OUT();

    return ret;
}

UINT32 AmbaIPC_StreamDelete(void *pvHdlr)
{
    UINT32 ret = STREAM_ERR_0001, i;
    SVC_STREAM_HDLR_s  *pHdlr;

    AmbaMisra_TypeCast(&pHdlr, &pvHdlr);

    TRACE_IN();

    for(i = 0;i < RPC_STREAM_HANDLE_NUM;i++){
        if( (RpcStreamHandle[i].Used == 1U) && (pHdlr == &(RpcStreamHandle[i].Hdlr))) {
            RpcStreamHandle[i].Used = 0U;
            ret = SVC_OK;
            break;
        }
    }

    if(ret != SVC_OK) {
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaIPC_Stream_Delete not found!", 0U, 0U, 0U, 0U, 0U);
    }

    TRACE_OUT();

    return ret;
}
