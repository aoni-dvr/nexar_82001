/**
 * Copyright (C) 2015 Ambarella Corporation. All rights reserved.
 *
 * No part of this file may be reproduced, stored in a retrieval
 * system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella Corporation.
 */
#ifndef __AMBASOCKETSVC_H__
#define __AMBASOCKETSVC_H__

#ifdef  __cplusplus
extern "C" {
#endif

/**
 * Init SocketSVC
 *
 * [in] port - server port number
 * [in] max_clnts - maximal accepted client connection
 * [in] max_qlen - maximal lisent queue length
 *
 * return:  0 - sucess
 *         <0 - error
 **/
int SocketSvc_init(unsigned short port, int max_clnts, int max_qlen);

/**
 * Release SocketSVC
 *
 * return:  0 - sucess
 *         <0 - error
 **/
int SocketSvc_release(void);

 /**
 * Get request
 *
 * [out] sender_sd - sender_sd for the coming request
 * [out] ReqBuf - received request
 * [in] ReqBufLen - maximal ReqBuf size
 *
 * return:  0 - new client connected
 *         <0 - error
 *         >0 - received request lenght
 **/
int SocketSvc_waitRequest(int *sender_sd, char *ReqBuf, int ReqBufLen);

 /**
 * Receve data.
 * Normally, Caller will do SocketSvc_waitRequest() first and then
 * call SocketSvc_Recv() if the request is not complete.
 *
 * [in] sender_sd - sender_sd for the coming data
 * [out] data - received data
 * [in] datalen - maximal data buffer size
 *
 * return:  0 - client disconnected
 *         <0 - error
 *         >0 - received data lenght
 **/
int SocketSvc_Recv(int sd, char *data, int datalen);

 /**
 * Send out data
 *
 * [in] sd - client socket_sd for this send operation
 * [in] data - data to send
 * [in] datalen - valid data size for this send operation
 *
 * return: <0 - error
 *         >0 - send out lenght
 **/
int SocketSvc_Send(int sd, char *data, int datalen);

 /**
 * Query client sd by port
 *
 * [in] client_port - the port number which client used for connection
 *
 * return: <=0 - error
 *         >0 - socket_sd of this client
 **/
int SocketSvc_QuerySD(unsigned long long client_port);

 /**
 * Query client sd by index
 *
 * [in] index - the index for client entry.
 *
 * return: <=0 - error
 *         >0 - socket_sd of this client
 **/
int SocketSvc_getClntSD(int index);

 /**
 * get maximal client amount setting
 *
 * return: <=0 - error
 *         >0 - Maximal client amount
 **/
int SocketSvc_getMaxAmount(void);

#ifdef  __cplusplus
}
#endif
#endif /* __AMBASOCKETSVC_H__ */
