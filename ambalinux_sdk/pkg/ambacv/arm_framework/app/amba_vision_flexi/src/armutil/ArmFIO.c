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

#include <stdio.h>
#include <errno.h>
#include <string.h>

/* Arm header */
#include "ArmLog.h"
#include "ArmFIO.h"

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
 *
 *  @Return     ::
 *    INT32:       The total length to load
\*-----------------------------------------------------------------------------------------------*/
INT32 ArmFIO_Load(void *pOutBuf, UINT32 BufSize, const char *pFileName)
{
    FILE *Fp = NULL;
    INT32 Fsize = 0, Ret = 0;
    UINT64 Fpos = 0U;
    UINT32 Tmp = 0U;

    Fp = fopen(pFileName, "rb");

    if (Fp == NULL) {
        ArmLog_ERR(ARM_LOG_FIO, "## AmbaFS_FileOpen fail", 0U, 0U);
        ArmLog_STR(ARM_LOG_FIO, "## Error: %s", strerror(errno), NULL);
        Fsize = -1;
    } else {
        fseek(Fp, 0LL, SEEK_END);
        Fpos = ftell(Fp);
        Tmp = (UINT32)(Fpos & 0xFFFFFFFFU);
        Fsize = (INT32)Tmp;

        if (pOutBuf != NULL) {
            if ((UINT32)Fsize > BufSize) {
                ArmLog_ERR(ARM_LOG_FIO, "## pOutBuf(%u) is too small for file(%u)", BufSize, (UINT32)Fsize);
                Ret = -2;
            } else {
                fseek(Fp, 0LL, SEEK_SET);
                Tmp = fread(pOutBuf, 1, (UINT32)Fsize, Fp);
            }
        }

        fclose(Fp);
        if (Ret == 0) {
            //ArmLog_DBG(ARM_LOG_FIO, "Load successfully... Size = %u, Addr = %p", (UINT32)Fsize, pOutBuf);
        } else {
            Fsize = Ret;
        }
    }

    return Fsize;
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
 *  @Output     :: None
 *
 *  @Return     ::
 *    UINT32:      The total length to save
\*-----------------------------------------------------------------------------------------------*/
INT32 ArmFIO_Save(void *pBuf, UINT32 Size, const char *pFileName)
{
    FILE *Fp = NULL;
    UINT32 WriteSize = 0U;

    Fp = fopen(pFileName, "wb");
    if (Fp != NULL) {
        WriteSize = fwrite(pBuf, 1, Size, Fp);
        if (WriteSize == 0U) {
            ArmLog_ERR(ARM_LOG_FIO, "## AmbaFS_FileWrite fail", 0u, 0U);
        }
        fclose(Fp);
    } else {
        ArmLog_ERR(ARM_LOG_FIO, "## AmbaFS_FileOpen fail", 0u, 0U);
        ArmLog_STR(ARM_LOG_FIO, "## Error: %s", strerror(errno), NULL);
    }

    return WriteSize;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ArmFIO_GetSize
 *
 *  @Description:: Get the file Size by given filename
 *
 *  @Input      ::
 *    pFileName:   The file path
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    INT32:       The total size
\*-----------------------------------------------------------------------------------------------*/
INT32 ArmFIO_GetSize(const char *pFileName)
{
    FILE *Fp = NULL;
    INT32 Fsize = 0;
    UINT64 Fpos = 0U;
    UINT32 Tmp = 0U;

    Fp = fopen(pFileName, "rb");

    if (Fp == NULL) {
        ArmLog_ERR(ARM_LOG_FIO, "## AmbaFS_FileOpen fail", 0U, 0U);
        ArmLog_STR(ARM_LOG_FIO, "## Error: %s", strerror(errno), NULL);
        Fsize = -1;
    } else {
        fseek(Fp, 0LL, SEEK_END);
        Fpos = ftell(Fp);
        Tmp = (UINT32)(Fpos & 0xFFFFFFFFU);
        Fsize = (INT32)Tmp;

        fclose(Fp);
    }

    return Fsize;
}

