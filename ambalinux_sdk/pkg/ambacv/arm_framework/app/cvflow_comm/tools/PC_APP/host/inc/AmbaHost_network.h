/**
*  @file AmbaHost_network.h
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
#ifndef __AMBA_HOST_NETWORK__
#define __AMBA_HOST_NETWORK__

#define HOSTAPI_SOCKET_PORT     8888

/* hXport is used to idenfiy a connection */
typedef void* hXport;

/*
 * Initialize the xport module.
 */
int AmbaHost_xport_init(uint32_t flags);

/*
 * Shut down the xport module
 */
int AmbaHost_xport_fini(void);

/*
 * Open a connection to a target
 * @pHandler:   pointer to receive the connection handle
 * @addr:       the IP address of the target
 */
int AmbaHost_xport_open(hXport *pHandle, const char *addr, uint32_t flags);

/*
 * Close a connection
 */
int AmbaHost_xport_close(hXport handle);

/*
 * Lock the connection so that no other thread can access the connection
 * This funnction is called before we want to send/recv multiple segments of
 * data that should be atomic.
 */
int AmbaHost_xport_lock(hXport port);

/*
 * unlock the connection
 */
int AmbaHost_xport_unlock(hXport port);

/*
 * Send a chunk of data to remote. Buffer is owned by caller.
 */
int AmbaHost_xport_send(hXport xport, const char *buff, uint32_t size);

/*
 * Receieve a chunk of data from remote. Buffer is owned by caller.
 */
int AmbaHost_xport_recv(hXport xport, char *buff, uint32_t size);

/*
 * skip a chunk of data from remote.
 */
int AmbaHost_xport_skip(hXport xport, uint32_t size);

/*
 * Get next sequence number fo the connection
 */
int AmbaHost_xport_next_seqnum(hXport xport);

#endif  //__AMBA_HOST_NETWORK__
