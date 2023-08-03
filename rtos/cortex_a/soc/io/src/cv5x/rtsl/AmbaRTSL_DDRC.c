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
 *  @details DDRC RTSL APIs
 *
 */

#include "AmbaTypes.h"

#include "AmbaCSL_DDRC.h"
#include "AmbaRTSL_DDRC.h"

const char DdrDramType[4][8] = {
    [0] = "LPDDR4",
    [1] = "LPDDR5",
    [2] = "N/A",
    [3] = "N/A",
};
const char DdrDramSize[10][8] = {
    [0] = "2Gb",
    [1] = "3Gb",
    [2] = "4Gb",
    [3] = "6Gb",
    [4] = "8Gb",
    [5] = "12Gb",
    [6] = "16Gb",
    [7] = "24Gb",
    [8] = "32Gb",
    [9] = "N/A",
};
#if 0
const char DdrPageSize[4][8] = {
    [0] = "1KB",
    [1] = "2KB",
    [2] = "4KB",
    [3] = "N/A"
};
const char DdrDqBusWidth[2][8] = {
    [0] = "32-bit",
    [1] = "16-bit"
};
#endif


const char* AmbaRTSL_DdrcGetDramTypeStr(UINT32 Host)
{
    UINT32 temp = AmbaCSL_DdrcGetDramType(Host);
    return DdrDramType[((temp > 3U) ? 3U : temp)];
}

const char* AmbaRTSL_DdrcGetDramSizeStr(UINT32 Host)
{
    UINT32 temp = AmbaCSL_DdrcGetDramSize(Host);
    return DdrDramSize[((temp > 9U) ? 9U : temp)];
}
