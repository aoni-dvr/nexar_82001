/*
 * Copyright (c) 2021 Ambarella International LP
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
 */

#define VOID     void

#include "AmbaKAL.h"
#include "AmbaTypes.h"
#include "AmbaRNG.h"

#define RNG_DEVNAME "/dev/hwrng"

void AmbaRNG_GetValue(UINT32 *random)
{
    AmbaRNG_DataGet(random, NULL, NULL, NULL, NULL);
}

UINT32 AmbaRNG_Init(void)
{
#ifndef CONFIG_ATF_SPD_OPTEE
    if (system("modprobe ambarella_rng 2>/dev/null") < 0) { }
#endif
    return RNG_ERR_NONE;
}

UINT32 AmbaRNG_DataGet(UINT32 *data0, UINT32 *data1, UINT32 *data2, UINT32 *data3, UINT32 *data4)
{
    UINT32 Ret = RNG_ERR_NONE;
#ifndef CONFIG_ATF_SPD_OPTEE
    INT32 fd;
    char buf[20];
    UINT32 ro;
    if (data0 == NULL) {
        Ret = RNG_ERR_ARG;
#if defined (CONFIG_SOC_CV2)
    } else if (data4 != NULL) { // cv2 only support at most 4 outputs, so data4 should be null from the caller.
        Ret = RNG_ERR_ARG;
#endif
    } else {
        if ((fd = open(RNG_DEVNAME, O_RDONLY)) == -1) {
            fprintf(stderr, "open:%s (Please init RNG module by rng start)\n", strerror(errno));
            Ret = RNG_ERR_FLOW;
        } else {
            ro = read(fd, buf, sizeof(buf));
            if (ro > 0U) {
                if (data0 != NULL) {
                    *data0 = *(UINT32*)&buf[0];
                }
                if (data1 != NULL) {
                    *data1 = *(UINT32*)&buf[4];
                }
                if (data2 != NULL) {
                    *data2 = *(UINT32*)&buf[8];
                }
                if (data3 != NULL) {
                    *data3 = *(UINT32*)&buf[12];
                }
#if !defined (CONFIG_SOC_CV2)
                if (data4 != NULL) {
                    *data4 = *(UINT32*)&buf[16];
                }
#endif
            } else {
                Ret = RNG_ERR_FLOW;
            }
            close(fd);
        }

    }
#else
    (void) data0;
    (void) data1;
    (void) data2;
    (void) data3;
    (void) data4;
#endif

    return Ret;
}

UINT32 AmbaRNG_Deinit(void)
{
    if (system("modprobe -r ambarella_rng 2>/dev/null") < 0) {}
    return RNG_ERR_NONE;
}
