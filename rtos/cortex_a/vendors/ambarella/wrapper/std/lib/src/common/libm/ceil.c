/**
 *  @file ceil
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
 *  @details Wrapper of ceil().
 *
 */

#include <math.h>

#include <AmbaWrap.h>

/**
 *  @brief  Wrapper of ceil()
 *
 *  Get the smallest integral value that is not less than x.
 *
 *  @param[in]  x   Value to round up.
 *  @param[in]  pV  The smallest integral value that is not less than x in DOUBLE.
 *
 *  @return     OK or LIBWRAP_ERR_0000
 */
UINT32 AmbaWrap_ceil(DOUBLE x, void *pV)
{
    UINT32 err = 0;
    DOUBLE v;

    if (pV == NULL) {
        err = LIBWRAP_ERR_INVAL;
    } else {
        v = ceil(x);
        err = AmbaWrap_memcpy(pV, &v, sizeof(v));
        if (err != OK) {
            err = LIBWRAP_ERR_0000;
        }
    }

    return err;
}

