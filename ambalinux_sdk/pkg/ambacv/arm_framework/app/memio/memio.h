/**
 *  @file memio.h
 *
 *  @copyright Copyright (c) 2018 Ambarella, Inc.
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
 *  @details memio
 *
 */

#ifndef MEMIO_H
#define MEMIO_H

#define MEMIO_MODE_CVTASK_L_APP_RR       0U /* CVTASK: Linux, DataSrc:RTOS,  DataSink: RTOS  */
#define MEMIO_MODE_CVTASK_L_APP_LL       1U /* CVTASK: Linux, DataSrc:Linux, DataSink: Linux */
#define MEMIO_MODE_CVTASK_L_APP_RL       2U /* CVTASK: Linux, DataSrc:RTOS,  DataSink: Linux */
#define MEMIO_MODE_CVTASK_R_APP_RL       3U /* CVTASK: RTOS,  DataSrc:RTOS,  DataSink: Linux */

#define MEMIO_TYPE_CLASSIFICATION     0U
#define MEMIO_TYPE_OBJDETRCTION       1U
#define MEMIO_TYPE_SEGMENTATION       2U



typedef struct{
    uint32_t mode;
    uint32_t type;
    uint32_t debug;
    char input[64];
    char output[64];
    char outputfilename_prefix[128];
} MEMIO_INFO_s;


uint32_t MemIO_Init(MEMIO_INFO_s *info);
uint32_t MemIO_Run(MEMIO_INFO_s *info);
uint32_t MemIO_Stop(MEMIO_INFO_s *info);
uint32_t MemIO_TriggerTask(MEMIO_INFO_s *info);
uint32_t MemIO_Finish(MEMIO_INFO_s *info);

#endif // MEMIO_H

