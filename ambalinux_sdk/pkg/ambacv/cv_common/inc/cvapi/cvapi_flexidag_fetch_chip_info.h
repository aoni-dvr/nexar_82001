/**
 *  @file cvapi_flexidag_template.h
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
 *  @details Implementation of template flexidag
 *
 */

#ifndef CVAPI_FLEXIDAG_FETCH_CHIP_INFO_H
#define CVAPI_FLEXIDAG_FETCH_CHIP_INFO_H

#include "ambint.h"

typedef struct {
    uint8_t AmbaUUID[32];
} AMBA_CV_CHIP_INFO_t;

#ifdef ORCVIS

/********************/
/* For ORCVP cvtask */
/********************/

/**
 * The function is used to fetch chip info in ORCVP cvtask.
 *
 * @param Info - (Out) pointer to 16 bytes AmbaUUID for return
 *
 * @return 0-OK
 */
errcode_enum_t  visorc_sec_get_unique_id(uint8_t *pAmbaUUID);

#else
/********************/
/* For ARM APP      */
/********************/

/**
 * The function is used to fetch chip info in ARM App.
 *
 * @param pFlexibinPath - (IN) path to flexidag binary
 * @param pChipInfo - (Out) pointer to AMBA_CV_CHIP_INFO_t for return info
 *
 * @return 0-OK
 */
uint32_t AmbaCV_FetchChipInfo(const char* pFlexibinPath, AMBA_CV_CHIP_INFO_t *pChipInfo);

#endif // ORCVIS

#endif //CVAPI_FLEXIDAG_FETCH_CHIP_INFO_H

