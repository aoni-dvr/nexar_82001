/**
 *  @file AmbaTempSensor.c
 *
 *  @copyright Copyright (c) 2022 Ambarella, Inc.
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
 *  @details Temperature Sensor Control APIs
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"

#include "AmbaDrvEntry.h"

#include "AmbaTempSensor.h"

#define TEMPERATURE_DEVNAME "/sys/bus/iio/devices/iio:device0/in_temp3_input"

/**
 *  Initializes driver-wide data structures and resources
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaTempSensor_DrvEntry(void)
{
    UINT32 RetVal = TEMPSENSOR_ERR_NONE;
    if (system("modprobe ambarella_adc") < 0) {
        printf("## [NG] AmbaWDT_DrvEntry: modprobe ambarella_adc FAILED !!");
        RetVal = TEMPSENSOR_ERR_UNEXPECTED;
    } else {
        printf("## [OK] AmbaWDT_DrvEntry: modprobe ambarella_adc SUCCESS !!");
    }

    return RetVal;
}

/**
 *  AmbaTempSensor_GetTemp - Find an avaliable timer instance
 *  @param[in] EnableDOC Mesure method
 *  @param[in] Channel The specified channel id
 *  @param[out] pTemp Temperature value
 *  @return error code
 */
UINT32 AmbaTempSensor_GetTemp(UINT32 EnableDOC, UINT32 Channel, DOUBLE *pTemp)
{
    UINT32 Ret = TEMPSENSOR_ERR_NONE;
    INT32 fd;
    char buf[20];
    UINT32 ro;
    char *ptr;

    (void)EnableDOC;

    if ((pTemp == NULL) || (Channel >= AMBA_TEMPSEN_NUM)) {
        Ret = TEMPSENSOR_ERR_ARG;
    } else {
        if ((fd = open(TEMPERATURE_DEVNAME, O_RDONLY)) == -1) {
            Ret = AmbaTempSensor_DrvEntry();
            if (Ret == TEMPSENSOR_ERR_NONE) {
                fd = open(TEMPERATURE_DEVNAME, O_RDONLY);
                ro = read(fd, buf, sizeof(buf));
                if (ro > 0U) {
                    *pTemp = strtod(buf, &ptr);
                    fprintf(stderr, "get temp %lf, %s\n", *pTemp,buf);
                } else {
                    Ret = TEMPSENSOR_ERR_UNEXPECTED;
                }
                close(fd);
            } else {
                fprintf(stderr, "open:%s fail, error(%s)\n", TEMPERATURE_DEVNAME, strerror(errno));
                Ret = TEMPSENSOR_ERR_UNEXPECTED;
            }
        } else {
            ro = read(fd, buf, sizeof(buf));
            if (ro > 0U) {
                *pTemp = strtod(buf, &ptr);
                fprintf(stderr, "get temp %lf, %s\n", *pTemp,buf);
            } else {
                Ret = TEMPSENSOR_ERR_UNEXPECTED;
            }
            close(fd);
        }

    }

    return Ret;
}

