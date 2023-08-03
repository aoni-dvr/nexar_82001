/**
*  @file host_api.c
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
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>
#include <fcntl.h>
#include "AmbaHost_api.h"
#include "AmbaHost_msg.h"
#include "AmbaHost_connection.h"
#include "AmbaHost_network.h"
#include "AmbaHost_util.h"
#include "private.h"

#define LOCK()         pthread_mutex_lock(&mutex)
#define UNLOCK()       pthread_mutex_unlock(&mutex)

static hccb            connection[MAX_CONNECTION_NUM];
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static int             cid_curr;
static printer_func    printer;

hccb* get_hccb(int cid)
{
    int  i;

    if (cid <= 0 || cid > cid_curr) {
        EMSG("CID %d is out of range!!!\n", cid);
        return  NULL;
    }

    LOCK();
    for (i = 0; i < MAX_CONNECTION_NUM; i++) {
        if (cid == connection[i].cid) {
            break;
        }
    }
    UNLOCK();

    if (i == MAX_CONNECTION_NUM) {
        EMSG("Invalid CID %d!!!\n", cid);
        return NULL;
    }

    return &connection[i];
}

void free_hccb(hccb *handle)
{
    LOCK();
    memset(handle, 0, sizeof(*handle));
    UNLOCK();
}

int AmbaHost_printf(const char *format, ...)
{
    char buff[1024];
    int len;
    va_list args;

    if (printer == NULL)
        return 0;

    va_start(args, format);
    len = vsprintf(buff, format, args);
    va_end(args);

    printer(HOSTAPI_LIBRARY_PRINTER_ID, buff, len);
    return len;
}

int AmbaHost_init(uint32_t flags, printer_func _printer)
{
    printer = _printer;
    AmbaHost_xport_init(flags);
    return AMBA_HOST_ERR_OK;
}

int AmbaHost_fini(void)
{
    AmbaHost_xport_fini();
    return AMBA_HOST_ERR_OK;
}

int AmbaHost_connection_open(const char *addr, uint32_t flags)
{
    int i, ret;
    hccb *handle = NULL;

    LOCK();
    /* acquire a free slot */
    for (i = 0; i < MAX_CONNECTION_NUM; i++) {
        if (connection[i].cid <= 0) {
            handle = &connection[i];
            handle->cid = ++cid_curr;
            if (cid_curr == 0x7FFFFFFF) {
                cid_curr = 0;
            }
            break;
        }
    }
    UNLOCK();
    if (handle == NULL) {
        EMSG("No connection slot available at this moment!!\n");
        return AMBA_HOST_ERR_CONNECTION_MAXOUT;
    }

    strcpy(handle->addr, addr);
    ret = AmbaHost_xport_open(&handle->xport, handle->addr, flags);
    if (ret != AMBA_HOST_ERR_OK) {
        /* release the current slot */
        LOCK();
        handle->cid = ret;
        UNLOCK();
    }

    return handle->cid;
}

int AmbaHost_connection_close(int cid)
{
    GET_AND_CHECK_HCCB(cid);
    AmbaHost_xport_close(handle->xport);
    free_hccb(handle);
    return AMBA_HOST_ERR_OK;
}

int AmbaHost_loopback(int cid, const char *message)
{
    GET_AND_CHECK_HCCB(cid);
    hXport xport = handle->xport;
    AHMSG_HDR header;

    AmbaHost_xport_lock(xport);
    tx_loopback(xport, message);
    AmbaHost_xport_recv(xport, (char*)&header, sizeof(header));
    rx_loopback(xport, &header);
    AmbaHost_xport_unlock(xport);

    return AMBA_HOST_ERR_OK;
}

int AmbaHost_get_addr(int cid, const char **addr)
{
    GET_AND_CHECK_HCCB(cid);
    *addr = handle->addr;
    return 0;
}

int AmbaHost_pull_file(int cid, const char *dst, const char *src)
{
    GET_AND_CHECK_HCCB(cid);
    hXport xport = handle->xport;
    AHMSG_HDR header;
    int ret;

    AmbaHost_xport_lock(xport);
    tx_copyfrom(xport, dst, src);
    AmbaHost_xport_recv(xport, (char*)&header, sizeof(header));
    ret = rx_copyfrom(xport, &header);
    AmbaHost_xport_unlock(xport);

    return ret;
}

int AmbaHost_push_file(int cid, const char *dst, const char *src)
{
    GET_AND_CHECK_HCCB(cid);
    hXport xport = handle->xport;
    AHMSG_HDR header;
    int ret;

    AmbaHost_xport_lock(xport);
    ret = tx_copyto(xport, dst, src);
    if(ret == AMBA_HOST_ERR_OK) {
        AmbaHost_xport_recv(xport, (char*)&header, sizeof(header));
        rx_copyto(xport, &header);
    }
    AmbaHost_xport_unlock(xport);

    return ret;
}

int AmbaHost_read_mem(int cid, void *hAddr, uint32_t tpAddr, uint32_t size)
{
    GET_AND_CHECK_HCCB(cid);
    hXport xport = handle->xport;
    AHMSG_HDR header;

    AmbaHost_xport_lock(xport);
    tx_read(xport, hAddr, tpAddr, size);
    AmbaHost_xport_recv(xport, (char*)&header, sizeof(header));
    rx_read(xport, &header);
    AmbaHost_xport_unlock(xport);

    return header.ret;
}

int AmbaHost_write_mem(int cid, void *hAddr, uint32_t tpAddr, uint32_t size)
{
    GET_AND_CHECK_HCCB(cid);
    hXport xport = handle->xport;
    AHMSG_HDR header;

    AmbaHost_xport_lock(xport);
    tx_write(xport, hAddr, tpAddr, size);
    AmbaHost_xport_recv(xport, (char*)&header, sizeof(header));
    rx_write(xport, &header);
    AmbaHost_xport_unlock(xport);

    return header.ret;
}

