/**
 *  @file AmbaFlashFwProg_HwStack_A64.c
 *
 *  Copyright (c) 2020 Ambarella International LP
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
 *  @details Definitions for all of the hardware Stacks
 */

#include "AmbaTypes.h"

static UINT8 _HwCore0Stack[0x2000] GNU_SECTION_NOZEROINIT;
static UINT8 _HwCore1Stack[0x2000] GNU_SECTION_NOZEROINIT;
static UINT8 _HwCore2Stack[0x2000] GNU_SECTION_NOZEROINIT;
static UINT8 _HwCore3Stack[0x2000] GNU_SECTION_NOZEROINIT;

UINT8 *const _pAmbaHwStack[] = {
    [0] = (UINT8 *) &_HwCore0Stack[sizeof(_HwCore0Stack)],
    [1] = (UINT8 *) &_HwCore1Stack[sizeof(_HwCore1Stack)],
    [2] = (UINT8 *) &_HwCore2Stack[sizeof(_HwCore2Stack)],
    [3] = (UINT8 *) &_HwCore3Stack[sizeof(_HwCore3Stack)],
};
