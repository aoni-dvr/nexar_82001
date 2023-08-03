/**
*  @file AmbaHost_connection.h
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
#ifndef __AMBA_HOST_CONNECTION__
#define __AMBA_HOST_CONNECTION__

#include <stdint.h>
#include <pthread.h>              /* for pthread */
#include "AmbaHost_network.h"

#define MAX_CONNECTION_NUM      128
#define MAX_SHELL_NUM           8
#define GET_AND_CHECK_HCCB(cid) \
    hccb *handle = get_hccb(cid); \
    if (handle == NULL) \
        return  AMBA_HOST_ERR_INVALID_CID;

/* control block for an AmbaHost shell */
typedef struct _hscb {
    int             socket;
    int             sid;
    pthread_t       thread;
    printer_func    printer;
} hscb;

/* control block for an AmbaHost connection */
typedef struct _hccb {
    char            addr[64];       // connection address
    int             cid;            // connection ID
    hXport          xport;          // transport handle
    int             shell_socket;   // private shell for Amba_exec
    hscb            shell[MAX_SHELL_NUM];
} hccb;

hccb* get_hccb(int cid);

#endif  //__AMBA_HOST_CONNECTION__
