#ifndef _RTSP_CLIENT_SESSION_H_
#define _RTSP_CLIENT_SESSION_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <arpa/inet.h>
#include "rtsp_server.h"

struct rtsp_client_session;
extern struct rtsp_client_session* rtsp_client_session_create(rtsp_server_t* srv,
                                                        unsigned session_id,
                                                        int client_sock,
                                                        struct sockaddr_in client_addr);
extern void rtsp_client_session_release(struct rtsp_client_session* session);
extern void rtsp_client_session_reset_timeout(struct rtsp_client_session* session);

#ifdef __cplusplus
}
#endif

#endif
