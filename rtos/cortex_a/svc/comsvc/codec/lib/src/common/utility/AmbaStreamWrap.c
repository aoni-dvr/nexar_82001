/**
 *  @file AmbaStreamWrap.c
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
 *  @details amba stream wrap
 *
 */

#include "AmbaDef.h"
#include "AmbaMisraFix.h"
#include "AmbaUtility.h"
#include "AmbaWrap.h"
#include "AmbaVfs.h"
#include "AmbaSvcWrap.h"
#include "AmbaCodecCom.h"
#include "SvcStream.h"
#include "AmbaStreamWrap.h"

#define SVC_STREAM_MAX_FILE_NUM   (8U)

#define SVC_STREAM_FLG_USED       (0x1U)

typedef struct {
    UINT32              Flag;
    char                FileName[128U];
    AMBA_VFS_FILE_s     File;
    SVC_STREAM_HDLR_s   Hdlr;
    PFN_GET_FREE_CB     pfnGetFreeCB;
} SVC_STREAM_CTRL_s;

static SVC_STREAM_CTRL_s  Stream[SVC_STREAM_MAX_FILE_NUM];

static UINT32 Open(SVC_STREAM_HDLR_s *Hdlr, const char *Name, UINT32 Mode)
{
    UINT32  Err, Rval = STREAM_ERR_0001, i;
    char    FileMode[8U];

    AmbaMisra_TouchUnused(Hdlr);

    for (i = 0U; i < SVC_STREAM_MAX_FILE_NUM; i++) {
        if ((&(Stream[i].Hdlr) == Hdlr) && (0U == CheckBits(Stream[i].Flag, SVC_STREAM_FLG_USED))) {
            Rval = OK;
            break;
        }
    }

    /* pass misra rule */
    AmbaMisra_TouchUnused(&i);

    if ((Rval == OK) && (i < SVC_STREAM_MAX_FILE_NUM)) {
        switch (Mode) {
            case SVC_STREAM_MODE_RDONLY:
                AmbaUtility_StringCopy(FileMode, sizeof(FileMode), "r");
                break;
            case SVC_STREAM_MODE_WRONLY:
                AmbaUtility_StringCopy(FileMode, sizeof(FileMode), "w");
                break;
            case SVC_STREAM_MODE_RDWR:
                AmbaUtility_StringCopy(FileMode, sizeof(FileMode), "r+");
                break;
            case SVC_STREAM_MODE_WRRD:
                AmbaUtility_StringCopy(FileMode, sizeof(FileMode), "w+");
                break;
            default:
                Rval = STREAM_ERR_0000;
                AmbaPrint_ModulePrintUInt5(AMBA_CODEC_PRINT_MODULE_ID, "Invalid mode %u", Mode, 0U, 0U, 0U, 0U);
                break;
        }
    }

    if ((Rval == OK) && (i < SVC_STREAM_MAX_FILE_NUM)) {
        Err = AmbaVFS_Open(Name, FileMode, 0U, &(Stream[i].File));
        if (Err != 0U) {
            Rval = STREAM_ERR_0002;
            AmbaPrint_ModulePrintUInt5(AMBA_CODEC_PRINT_MODULE_ID, "AmbaVFS_Open failed %u", Err, 0U, 0U, 0U, 0U);
        }
    }

    if ((Rval == OK) && (i < SVC_STREAM_MAX_FILE_NUM)) {
        Stream[i].Flag = SetBits(Stream[i].Flag, SVC_STREAM_FLG_USED);
        AmbaUtility_StringCopy(Stream[i].FileName, sizeof(Stream[i].FileName), Name);
    }

    return Rval;
}

static UINT32 Close(SVC_STREAM_HDLR_s *Hdlr)
{
    UINT32  Err, Rval = STREAM_ERR_0000, i;

    AmbaMisra_TouchUnused(Hdlr);

    for (i = 0U; i < SVC_STREAM_MAX_FILE_NUM; i++) {
        if (&(Stream[i].Hdlr) == Hdlr) {
            Rval = OK;
            break;
        }
    }

    /* pass misra rule */
    AmbaMisra_TouchUnused(&i);

    if ((Rval == OK) && (i < SVC_STREAM_MAX_FILE_NUM)) {
        Err = AmbaVFS_Close(&(Stream[i].File));
        if (Err != 0U) {
            Rval = STREAM_ERR_0002;
            AmbaPrint_ModulePrintUInt5(AMBA_CODEC_PRINT_MODULE_ID, "AmbaVFS_Close failed %u", Err, 0U, 0U, 0U, 0U);
        }
    }

    if ((Rval == OK) && (i < SVC_STREAM_MAX_FILE_NUM)) {
        Stream[i].Flag = ClearBits(Stream[i].Flag, SVC_STREAM_FLG_USED);
    }

    return Rval;
}

static UINT32 Read(SVC_STREAM_HDLR_s *Hdlr, UINT32 Size, UINT8 *Buffer, UINT32 *Count)
{
    UINT32  Err, Rval = STREAM_ERR_0000, i;

    AmbaMisra_TouchUnused(Hdlr);

    for (i = 0U; i < SVC_STREAM_MAX_FILE_NUM; i++) {
        if (&(Stream[i].Hdlr) == Hdlr) {
            Rval = OK;
            break;
        }
    }

    /* pass misra rule */
    AmbaMisra_TouchUnused(&i);

    if ((Rval == OK) && (i < SVC_STREAM_MAX_FILE_NUM)) {
        Err = AmbaVFS_Read(Buffer, Size, 1U, &(Stream[i].File), Count);
        if (Err != 0U) {
            Rval = STREAM_ERR_0002;
            AmbaPrint_ModulePrintUInt5(AMBA_CODEC_PRINT_MODULE_ID, "AmbaVFS_Read failed %u", Err, 0U, 0U, 0U, 0U);
        }
    }

    *Count = (*Count) * Size;

    return Rval;
}

static UINT32 Write(SVC_STREAM_HDLR_s *Hdlr, UINT32 Size, UINT8 *Buffer, UINT32 *Count)
{
    UINT32  Err, Rval = STREAM_ERR_0000, i;

    AmbaMisra_TouchUnused(Hdlr);

    for (i = 0U; i < SVC_STREAM_MAX_FILE_NUM; i++) {
        if (&(Stream[i].Hdlr) == Hdlr) {
            Rval = OK;
            break;
        }
    }

    /* pass misra rule */
    AmbaMisra_TouchUnused(&i);

    if ((Rval == OK) && (i < SVC_STREAM_MAX_FILE_NUM)) {
        Err = AmbaVFS_Write(Buffer, Size, 1U, &(Stream[i].File), Count);
        if (Err != 0U) {
            Rval = STREAM_ERR_0002;
            AmbaPrint_ModulePrintUInt5(AMBA_CODEC_PRINT_MODULE_ID, "AmbaVFS_Write failed %u", Err, 0U, 0U, 0U, 0U);
        }
    }

    return Rval;
}

static UINT32 Seek(SVC_STREAM_HDLR_s *Hdlr, INT64 Pos, INT32 Orig)
{
    UINT32  Err, Rval = STREAM_ERR_0000, i;

    AmbaMisra_TouchUnused(Hdlr);

    for (i = 0U; i < SVC_STREAM_MAX_FILE_NUM; i++) {
        if (&(Stream[i].Hdlr) == Hdlr) {
            Rval = OK;
            break;
        }
    }

    /* pass misra rule */
    AmbaMisra_TouchUnused(&i);

    if ((Rval == OK) && (i < SVC_STREAM_MAX_FILE_NUM)) {
        Err = AmbaVFS_Seek(&(Stream[i].File), Pos, Orig);
        if (Err != 0U) {
            Rval = STREAM_ERR_0002;
            AmbaPrint_ModulePrintUInt5(AMBA_CODEC_PRINT_MODULE_ID, "AmbaVFS_Seek failed %u", Err, 0U, 0U, 0U, 0U);
        }
    }

    return Rval;
}

static UINT32 GetPos(SVC_STREAM_HDLR_s *Hdlr, UINT64 *Pos)
{
    UINT32  Err, Rval = STREAM_ERR_0000, i;

    AmbaMisra_TouchUnused(Hdlr);

    for (i = 0U; i < SVC_STREAM_MAX_FILE_NUM; i++) {
        if (&(Stream[i].Hdlr) == Hdlr) {
            Rval = OK;
            break;
        }
    }

    /* pass misra rule */
    AmbaMisra_TouchUnused(&i);

    if ((Rval == OK) && (i < SVC_STREAM_MAX_FILE_NUM)) {
        Err = AmbaVFS_GetPos(&(Stream[i].File), Pos);
        if (Err != 0U) {
            Rval = STREAM_ERR_0002;
            AmbaPrint_ModulePrintUInt5(AMBA_CODEC_PRINT_MODULE_ID, "AmbaVFS_GetPos failed %u", Err, 0U, 0U, 0U, 0U);
        }
    }

    return Rval;
}

static UINT32 GetLength(SVC_STREAM_HDLR_s *Hdlr, UINT64 *Size)
{
    UINT32                Err, Rval = STREAM_ERR_0000, i;
    AMBA_FS_FILE_INFO_s   Info;

    AmbaMisra_TouchUnused(Hdlr);

    for (i = 0U; i < SVC_STREAM_MAX_FILE_NUM; i++) {
        if (&(Stream[i].Hdlr) == Hdlr) {
            Rval = OK;
            break;
        }
    }

    /* pass misra rule */
    AmbaMisra_TouchUnused(&i);

    if ((Rval == OK) && (i < SVC_STREAM_MAX_FILE_NUM)) {
        Err = AmbaFS_GetFileInfo(Stream[i].FileName, &Info);
        if (Err != AMBA_FS_ERR_NONE) {
            Rval = STREAM_ERR_0002;
            AmbaPrint_ModulePrintUInt5(AMBA_CODEC_PRINT_MODULE_ID, "AmbaFS_GetFileInfo failed %u", Err, 0U, 0U, 0U, 0U);
        } else {
            *Size = Info.Size;
        }
    }

    return Rval;
}

static UINT32 GetFreeSpace(SVC_STREAM_HDLR_s *Hdlr, UINT64 *Size)
{
    UINT32  Rval = STREAM_ERR_0000, i;

    AmbaMisra_TouchUnused(Hdlr);

    for (i = 0U; i < SVC_STREAM_MAX_FILE_NUM; i++) {
        if (&(Stream[i].Hdlr) == Hdlr) {
            Rval = OK;
            break;
        }
    }

    /* pass misra rule */
    AmbaMisra_TouchUnused(&i);

    if ((Rval == OK) && (i < SVC_STREAM_MAX_FILE_NUM)) {
        if (Stream[i].pfnGetFreeCB != NULL) {
            Stream[i].pfnGetFreeCB(Stream[i].File.Drive, Size);
        }
    }

    return Rval;
}

static UINT32 Sync(SVC_STREAM_HDLR_s *Hdlr)
{
    UINT32  Err, Rval = STREAM_ERR_0000, i;

    AmbaMisra_TouchUnused(Hdlr);

    for (i = 0U; i < SVC_STREAM_MAX_FILE_NUM; i++) {
        if (&(Stream[i].Hdlr) == Hdlr) {
            Rval = OK;
            break;
        }
    }

    /* pass misra rule */
    AmbaMisra_TouchUnused(&i);

    if ((Rval == OK) && (i < SVC_STREAM_MAX_FILE_NUM)) {
        Err = AmbaVFS_Sync(&(Stream[i].File));
        if (Err != 0U) {
            Rval = STREAM_ERR_0002;
            AmbaPrint_ModulePrintUInt5(AMBA_CODEC_PRINT_MODULE_ID, "AmbaVFS_Sync failed %u", Err, 0U, 0U, 0U, 0U);
        }
    }

    return Rval;
}

/**
* initialization of fifo stream wrap
* @param [in] pfnGetFree function to get free space size
* @return none
*/
void AmbaStreamWrap_Init(PFN_GET_FREE_CB pfnGetFree)
{
    UINT32                Err, i;
    static SVC_STREAM_s   StreamFunc;
    static UINT32         Init = 0U;

    if (Init == 0U) {
        Err = AmbaWrap_memset(Stream, 0, sizeof(Stream));
        if (Err != 0U) {
            AmbaPrint_ModulePrintUInt5(AMBA_CODEC_PRINT_MODULE_ID, "AmbaWrap_memset failed %u", Err, 0U, 0U, 0U, 0U);
        }

        {
            StreamFunc.Open          = Open;
            StreamFunc.Close         = Close;
            StreamFunc.Read          = Read;
            StreamFunc.Write         = Write;
            StreamFunc.Seek          = Seek;
            StreamFunc.GetPos        = GetPos;
            StreamFunc.GetLength     = GetLength;
            StreamFunc.GetFreeSpace  = GetFreeSpace;
            StreamFunc.Sync          = Sync;
            StreamFunc.Func          = NULL;
        }

        for (i = 0U; i < SVC_STREAM_MAX_FILE_NUM; i++) {
            Stream[i].Hdlr.Func = &StreamFunc;
            Stream[i].pfnGetFreeCB = pfnGetFree;
        }

        Init = 1U;
    }
}

/**
* handler get of fifo stream wrap
* @param [out] ppvHdlr handler of fifo stream
* @return 0-OK, 1-CODEC_ERROR_GENERAL_ERROR
*/
UINT32 AmbaStreamWrap_GetHdlr(void **ppvHdlr)
{
    UINT32  Rval = CODEC_ERROR_GENERAL_ERROR, i;

    for (i = 0U; i < SVC_STREAM_MAX_FILE_NUM; i++) {
        if (0U == CheckBits(Stream[i].Flag, SVC_STREAM_FLG_USED)) {
            *ppvHdlr = &(Stream[i].Hdlr);
            Rval    = OK;
            break;
        }
    }

    if (Rval == CODEC_ERROR_GENERAL_ERROR) {
        AmbaPrint_ModulePrintUInt5(AMBA_CODEC_PRINT_MODULE_ID, "No available handler", 0U, 0U, 0U, 0U, 0U);
    }

    return Rval;
}
