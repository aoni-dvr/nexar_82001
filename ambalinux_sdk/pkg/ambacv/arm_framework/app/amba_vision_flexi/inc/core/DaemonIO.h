/**
*  @file DaemonIO.h
*
* Copyright (c) [2020] Ambarella International LP
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
* This file includes sample code and is only for internal testing and evaluation.  If you
* distribute this sample code (whether in source, object, or binary code form), it will be
* without any warranty or indemnity protection from Ambarella International LP or its affiliates.
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
*
*  @detail xxxx
*
*/
#ifndef DAEMONIO_H
#define DAEMONIO_H

#include <stdint.h>         /* for fixed width integer types */
#include "cvapi_ambacv_flexidag.h"
#include "avf.h"

#define DAEMON_OK              (0)        /* OK */
#define DAEMON_NG             (-1)        /* NG */

#define DAEMON_STATE_OFF       (0)        /* TBD: Allocate CMA by ArmMem_Allocate() */
#define DAEMON_STATE_ON        (1)        /* Allocate CMA by AVF daemon self */

#define DAEMON_MAX_IO          (8)
#define DAEMON_MAX_PATH_LEN  (256)

/* Client Side APIs */
int32_t DaemonIO_Init();
int32_t DaemonIO_CheckSvc();
int32_t DaemonIO_RunFlexidag();
int32_t DaemonIO_RunFlexidagWithMultiVP(uint32_t vp_num);
int32_t DaemonIO_RunNonBlock();
int32_t DaemonIO_TestYield();
int32_t DaemonIO_ShutdownSvc();
int32_t DaemonIO_GetUnsolicitedMsg();
int32_t DaemonIO_GetUnsolicitedMsgWithMultiVP(uint32_t vp_num);
int32_t DaemonIO_DeInit();

/* Daemon Side APIs */
uint32_t DaemonIO_GetCmaInfo(uint32_t *p_usedSize, uint32_t *p_freeSize);
uint32_t DaemonIO_AllocateCma(uint32_t req_size, flexidag_memblk_t *p_buf);
uint32_t DaemonIO_FreeCma(flexidag_memblk_t *p_buf);
uint32_t DaemonIO_InitMem();
uint32_t DaemonIO_DeInitMem();

#endif
