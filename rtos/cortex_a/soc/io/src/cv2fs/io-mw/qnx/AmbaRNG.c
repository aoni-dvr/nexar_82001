/*
 * Copyright (c) 2020 Ambarella International LP
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

#include <fcntl.h>
#include <stdio.h>

#include "AmbaTypes.h"

typedef void   VOID;

#include "hw/ambarella_rng.h"


void AmbaRNG_GetValue(UINT32 *random)
{
    int fd, err = 0;
    rng_msg_t msg;

    fd = open("/dev/rng", O_RDWR);
    if (fd == -1) {
        *random = 0xFFFFFFFF;
    } else {

        err = devctl(fd, DCMD_RNG_ENABLE, &msg, sizeof(rng_msg_t), NULL);
        if (err) {
            printf("DCMD_RNG_ENABLE error \n");
        }

        err = devctl(fd, DCMD_RNG_OUTPUT, &msg, sizeof(rng_msg_t), NULL);
        if (err) {
            printf("DCMD_RNG_OUTPUT error \n");
        }

        *random = msg.data0;

        close(fd);
    }
}


UINT32 AmbaRNG_Init(VOID)
{
    UINT32 Ret = RNG_ERR_NONE;
    int fd, err = 0;
    rng_msg_t msg;

    fd = open("/dev/rng", O_RDWR);
    if (fd == -1) {
        printf("Failed to open rng \n");
    } else {
        err = devctl(fd, DCMD_RNG_ENABLE, &msg, sizeof(rng_msg_t), NULL);
        if (err) {
            printf("DCMD_RNG_ENABLE error \n");
        } else {
            Ret = RNG_ERR_NONE;
        }

    }
    return Ret;
}

UINT32 AmbaRNG_DataGet(UINT32 *data0, UINT32 *data1, UINT32 *data2, UINT32 *data3, UINT32 *data4)
{
    UINT32 Ret = RNG_ERR_NONE;
    int fd, err = 0;
    rng_msg_t msg;

    if (data0 == NULL) {
        Ret = RNG_ERR_ARG;
    } else {
        fd = open("/dev/rng", O_RDWR);
        if (fd != -1) {
            err = devctl(fd, DCMD_RNG_OUTPUT, &msg, sizeof(rng_msg_t), NULL);
            if (err) {
                printf("DCMD_RNG_OUTPUT error \n");
            } else {
                *data0 = msg.data0;
                if(data1 != NULL) {
                    *data1 = msg.data1;
                }
                if(data2 != NULL) {
                    *data2 = msg.data2;
                }
                if(data3 != NULL) {
                    *data3 = msg.data3;
                }
                if(data4 != NULL) {
                    *data4 = msg.data4;
                }
            }
        } else {
            printf("Failed to open rng \n");
        }
    }

    return Ret;
}

UINT32 AmbaRNG_Deinit(VOID)
{
    UINT32 Ret = RNG_ERR_NONE;
    int fd, err = 0;
    rng_msg_t msg;

    fd = open("/dev/rng", O_RDWR);
    if (fd != -1) {
        err = devctl(fd, DCMD_RNG_DISABLE, &msg, sizeof(rng_msg_t), NULL);
        if (err) {
            printf("DCMD_RNG_DISABLE error \n");
        } else {
            fd = -1;
            close(fd);
        }
    } else {
        printf("Failed to open rng \n");
    }

    return Ret;
}
