/**
 *  @file memcmp
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
 *  @details Wrapper of memcmp().
 *
 */

#include <string.h>

#include <AmbaWrap.h>

/**
 *  @brief  Wrapper of memcmp()
 *
 *  Returns an integral value to indicate the relationship between
 *  the first n-bytes content of memory arrays.
 *
 *  @param[in]	p1	Pointer to a block of memory.
 *  @param[in]	p2	Pointer to a block of memory.
 *  @param[in]  n   (First) n bytes to be compared.
 *  @param[in,out]	pV	Integer value to indicate relationship between p1 and p2
 *                      - < 0: the first n bytes of p1 is less than p2.
 *                      - = 0: the first n bytes of p1 is equal to p2.
 *                      - > 0: the first n bytes of p1 is greater than p2.
 *
 *  @return     OK or LIBWRAP_ERR_0000
 */
UINT32 AmbaWrap_memcmp(const void *p1, const void *p2, SIZE_t n, void *pV)
{
    UINT32 err = 0;
    INT32 v;
    const UINT8 *pp1;
    const UINT8 *pp2;

    if ((p1 == NULL) ||
        (p2 == NULL) ||
        (pV == NULL) ||
        (n == 0U)) {
        err = LIBWRAP_ERR_INVAL;
    } else {
        /* For MISRA-C 21.16 */
        err = AmbaWrap_memcpy(&pp1, &p1, sizeof(void *));
        if (err != OK) {
            err = LIBWRAP_ERR_0000;
        } else {
            err = AmbaWrap_memcpy(&pp2, &p2, sizeof(void *));
            if (err != OK) {
                err = LIBWRAP_ERR_0000;
            } else {
                v = memcmp(pp1, pp2, n);
                err = AmbaWrap_memcpy(pV, &v, sizeof(v));
                if (err != OK) {
                    err = LIBWRAP_ERR_0000;
                }
            }
        }
    }

    return err;
}

