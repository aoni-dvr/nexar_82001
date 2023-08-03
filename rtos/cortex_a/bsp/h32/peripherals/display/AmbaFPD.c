/**
 *  @file AmbaFPD.c
 *
 *  @copyright Copyright (c) 2017 Ambarella, Inc.
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Definitions for LCD driver APIs
 *
 */

#include "AmbaTypes.h"
#include "AmbaFPD.h"

static AMBA_FPD_OBJECT_s *pAmbaFpdObj[4];

UINT32 AmbaFPD_Hook(UINT32 Chan, AMBA_FPD_OBJECT_s *pFpdObj)
{
    UINT32 RetVal = ERR_NONE;

    if ((Chan >= AMBA_NUM_FPD_CHANNEL) || (pFpdObj == NULL)) {
        RetVal = ERR_ARG;
    } else {
        pAmbaFpdObj[Chan] = pFpdObj;
    }

    return RetVal;
}

UINT32 AmbaFPD_Enable(UINT32 Chan)
{
    UINT32 RetVal = ERR_NONE;

    if (Chan >= AMBA_NUM_FPD_CHANNEL) {
        RetVal = ERR_ARG;
    } else if (pAmbaFpdObj[Chan]->FpdEnable == NULL) {
        RetVal = ERR_NA;
    } else {
        RetVal = pAmbaFpdObj[Chan]->FpdEnable();
    }

    return RetVal;
}

UINT32 AmbaFPD_Disable(UINT32 Chan)
{
    UINT32 RetVal = ERR_NONE;

    if (Chan >= AMBA_NUM_FPD_CHANNEL) {
        RetVal = ERR_ARG;
    } else if (pAmbaFpdObj[Chan]->FpdDisable == NULL) {
        RetVal = ERR_NA;
    } else {
        RetVal = pAmbaFpdObj[Chan]->FpdDisable();
    }

    return RetVal;
}

UINT32 AmbaFPD_GetInfo(UINT32 Chan, AMBA_FPD_INFO_s *pFpdInfo)
{
    UINT32 RetVal = ERR_NONE;

    if ((Chan >= AMBA_NUM_FPD_CHANNEL) || (pFpdInfo == NULL)) {
        RetVal = ERR_ARG;
    } else if (pAmbaFpdObj[Chan]->FpdGetInfo == NULL) {
        RetVal = ERR_NA;
    } else {
        RetVal = pAmbaFpdObj[Chan]->FpdGetInfo(pFpdInfo);
    }

    return RetVal;
}

UINT32 AmbaFPD_Config(UINT32 Chan, UINT8 Mode)
{
    UINT32 RetVal = ERR_NONE;

    if (Chan >= AMBA_NUM_FPD_CHANNEL) {
        RetVal = ERR_ARG;
    } else if (pAmbaFpdObj[Chan]->FpdConfig == NULL) {
        RetVal = ERR_NA;
    } else {
        RetVal = pAmbaFpdObj[Chan]->FpdConfig(Mode);
    }

    return RetVal;
}

UINT32 AmbaFPD_SetBacklight(UINT32 Chan, UINT32 EnableFlag)
{
    UINT32 RetVal = ERR_NONE;

    if (Chan >= AMBA_NUM_FPD_CHANNEL) {
        RetVal = ERR_ARG;
    } else if (pAmbaFpdObj[Chan]->FpdSetBacklight == NULL) {
        RetVal = ERR_NA;
    } else {
        RetVal = pAmbaFpdObj[Chan]->FpdSetBacklight(EnableFlag);
    }

    return RetVal;
}

UINT32 AmbaFPD_GetSerdesLinkStatus(UINT32 Chan, UINT32 *pLinkStatus)
{
    UINT32 RetVal = ERR_NONE;

    if (Chan >= AMBA_NUM_FPD_CHANNEL) {
        RetVal = ERR_ARG;
    } else if (pAmbaFpdObj[Chan]->FpdGetLinkStatus == NULL) {
        RetVal = ERR_NA;
    } else {
        RetVal = pAmbaFpdObj[Chan]->FpdGetLinkStatus(pLinkStatus);
    }

    return RetVal;
}
