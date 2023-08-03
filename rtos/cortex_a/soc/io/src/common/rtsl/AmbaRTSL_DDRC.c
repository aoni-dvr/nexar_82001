/**
 *  @file AmbaRTSL_DDRC.c
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
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
 *  @details DDR Controller RTSL APIs
 *
 */

#include "AmbaTypes.h"

#include "AmbaRTSL_DDRC.h"
#include "AmbaCSL_DDRC.h"

/**
 *  AmbaRTSL_DdrcSetDllSet - Set DLL register
 *  @param[in] Host Ddrc host ID
 *  @param[in] ByteId Byte ID
 *  @param[in] DllRegVal Register value
 */
void AmbaRTSL_DdrcSetDllSet(UINT32 Host, UINT8 ByteId, UINT32 DllRegVal)
{
    switch (ByteId) {
    case 0U:
        AmbaCSL_DdrcSetDll0Setting(Host, DllRegVal);
        break;
    case 1U:
        AmbaCSL_DdrcSetDll1Setting(Host, DllRegVal);
        break;
    case 2U:
        AmbaCSL_DdrcSetDll2Setting(Host, DllRegVal);
        break;
    case 3U:
        AmbaCSL_DdrcSetDll3Setting(Host, DllRegVal);
        break;
    default:
        /* for misraC, do nothing */
        break;
    }
}

void AmbaRTSL_DdrcSetPadTerm(UINT32 Host, UINT32 DllRegVal)
{
    AmbaCSL_DdrcSetPadTerm(Host, DllRegVal);
}

void AmbaRTSL_DdrcSetPadTerm2(UINT32 Host, UINT32 DllRegVal)
{
    AmbaCSL_DdrcSetPadTerm2(Host, DllRegVal);
}

void AmbaRTSL_DdrcSetPadTerm3(UINT32 Host, UINT32 DllRegVal)
{
    AmbaCSL_DdrcSetPadTerm3(Host, DllRegVal);
}

void AmbaRTSL_DdrcSetDqVref(UINT32 Host, UINT32 DllRegVal)
{
    AmbaCSL_DdrcSetDqVref(Host, DllRegVal);
}

void AmbaRTSL_DdrcSetDqReadDly(UINT32 Host, UINT32 DllRegVal)
{
    AmbaCSL_DdrcSetDqReadDly(Host, DllRegVal);
}

void AmbaRTSL_DdrcSetDqWriteDly(UINT32 Host, UINT32 DllRegVal)
{
    AmbaCSL_DdrcSetDqWriteDly(Host, DllRegVal);
}

void AmbaRTSL_DdrcSetModeReg(UINT32 Host, UINT32 DllRegVal)
{
    AmbaCSL_DdrcSetModeReg(Host, DllRegVal);
}

UINT32 AmbaRTSL_DdrcGetDllSet(UINT32 Host)
{
    return AmbaCSL_DdrcGetDll0Setting(Host);
}

UINT32 AmbaRTSL_DdrcGetPadTerm(UINT32 Host)
{
    return AmbaCSL_DdrcGetPadTerm(Host);
}

UINT32 AmbaRTSL_DdrcGetPadTerm2(UINT32 Host)
{
    return AmbaCSL_DdrcGetPadTerm2(Host);
}

UINT32 AmbaRTSL_DdrcGetPadTerm3(UINT32 Host)
{
    return AmbaCSL_DdrcGetPadTerm3(Host);
}

UINT32 AmbaRTSL_DdrcGetDqVref(UINT32 Host)
{
    return AmbaCSL_DdrcGetDqVref(Host);
}

UINT32 AmbaRTSL_DdrcGetDqReadDly(UINT32 Host)
{
    return AmbaCSL_DdrcGetDqReadDly(Host);
}

UINT32 AmbaRTSL_DdrcGetDqWriteDly(UINT32 Host)
{
    return AmbaCSL_DdrcGetDqWriteDly(Host);
}

UINT32 AmbaRTSL_DdrcGetModeReg(UINT32 Host)
{
    return AmbaCSL_DdrcGetModeReg(Host);
}

