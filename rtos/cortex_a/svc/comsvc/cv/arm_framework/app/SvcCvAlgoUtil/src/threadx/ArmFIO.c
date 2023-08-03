/**
*  @file ArmFIO.c
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
*   @details The Arm file access utility
*
*/

/* Amba header */
#include "AmbaFS.h"
#include "AmbaMisraFix.h"

/* Arm header */
#include "ArmLog.h"
#include "ArmFIO.h"
#include "ArmErrCode.h"

#define ARM_LOG_FIO          "ArmUtil_FIO"

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ArmFIO_Load
 *
 *  @Description:: Load binary from file
 *
 *  @Input      ::
 *    BufSize:     The max buffer size
 *    pFileName:   The file path
 *
 *  @Output     ::
 *    pOutBuf:     The output buffer
 *    pLoadSize:   The total length to load
 *
 *  @Return     ::
 *    UINT32:      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
UINT32 ArmFIO_Load(void *pOutBuf, UINT32 BufSize, const char *pFileName, UINT32 *pLoadSize)
{
    AMBA_FS_FILE *Fp = NULL;
    UINT64 Fpos = 0U;
    UINT32 Tmp = 0U, Fsize = 0U;
    UINT32 Rval = ARM_OK;

    (void)AmbaFS_FileOpen(pFileName, "rb", &Fp);

    if (Fp == NULL) {
        UINT32 ErrCode = AmbaFS_GetError();
        ArmLog_ERR(ARM_LOG_FIO, "## AmbaFS_FileOpen fail (0x%x)", ErrCode, 0U);
        Rval = ARM_NG;
    } else {
        (void)AmbaFS_FileSeek(Fp, 0LL, AMBA_FS_SEEK_END);
        (void)AmbaFS_FileTell(Fp, &Fpos);
        Tmp = (UINT32)(Fpos & 0xFFFFFFFFU);
        Fsize = Tmp;

        if (pOutBuf != NULL) {
            if (Fsize > BufSize) {
                ArmLog_ERR(ARM_LOG_FIO, "## pOutBuf(%u) is too small for file(%u)", BufSize, (UINT32)Fsize);
                Rval = ARM_NG;
            } else {
                (void)AmbaFS_FileSeek(Fp, 0LL, AMBA_FS_SEEK_START);
                (void)AmbaFS_FileRead(pOutBuf, 1, Fsize, Fp, &Tmp);
                if (Tmp != Fsize) {
                    ArmLog_ERR(ARM_LOG_FIO, "Read size (%u) is not eqaul to file size (%u)", Tmp, Fsize);
                    Rval = ARM_NG;
                }
            }
        }

        (void)AmbaFS_FileClose(Fp);
        if (Rval != ARM_NG) {
            AmbaMisra_TypeCast32(&Tmp, &pOutBuf);
            ArmLog_DBG(ARM_LOG_FIO, "Load successfully... Size = %u, Addr = 0x%x", Fsize, Tmp);
            *pLoadSize = Fsize;
        }
    }

    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ArmFIO_Save
 *
 *  @Description:: Save data to file
 *
 *  @Input      ::
 *    BufSize:     The saved size
 *    pFileName:   The saved file path
 *    pBuf:        The pointer to saved buffer
 *
 *  @Output     ::
 *    pWriteSize:  The total length to save
 *
 *  @Return     ::
 *    UINT32:      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
UINT32 ArmFIO_Save(void *pBuf, UINT32 Size, const char *pFileName, UINT32 *pWriteSize)
{
    AMBA_FS_FILE *Fp = NULL;
    UINT32 Rval = ARM_OK;

    (void)AmbaFS_FileOpen(pFileName, "wb", &Fp);

    if (Fp == NULL) {
        UINT32 ErrCode = AmbaFS_GetError();
        ArmLog_ERR(ARM_LOG_FIO, "## AmbaFS_FileOpen fail (0x%x)", ErrCode, 0U);
        Rval = ARM_NG;
    } else {
        (void)AmbaFS_FileWrite(pBuf, 1, Size, Fp, pWriteSize);
        if ((*pWriteSize) == 0U) {
            ArmLog_ERR(ARM_LOG_FIO, "## AmbaFS_FileWrite fail", 0U, 0U);
            Rval = ARM_NG;
        }
        (void)AmbaFS_FileClose(Fp);
    }

    return Rval;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ArmFIO_GetSize
 *
 *  @Description:: Get the file Size by given filename
 *
 *  @Input      ::
 *    pFileName:   The file path
 *
 *  @Output     ::
 *    pFileName:   The returned file length
 *
 *  @Return     ::
 *    UINT32:      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
UINT32 ArmFIO_GetSize(const char *pFileName, UINT32 *pLength)
{
    AMBA_FS_FILE *Fp = NULL;
    UINT32 Fsize = 0;
    UINT64 Fpos = 0U;
    UINT32 Tmp = 0U;
    UINT32 Rval = ARM_OK;

    (void)AmbaFS_FileOpen(pFileName, "rb", &Fp);

    if (Fp == NULL) {
        UINT32 ErrCode = AmbaFS_GetError();
        ArmLog_ERR(ARM_LOG_FIO, "## AmbaFS_FileOpen fail (0x%x)", ErrCode, 0U);
        Rval = ARM_NG;
    } else {
        (void)AmbaFS_FileSeek(Fp, 0LL, AMBA_FS_SEEK_END);
        (void)AmbaFS_FileTell(Fp, &Fpos);
        Tmp = (UINT32)(Fpos & 0xFFFFFFFFU);
        Fsize = Tmp;

        (void)AmbaFS_FileClose(Fp);
        *pLength = Fsize;
    }

    return Rval;
}

