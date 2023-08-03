#ifndef _RTP_H_
#define _RTP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/uio.h>
#include <netinet/in.h>	/* sockaddr_in{} and other Internet defns */

#include "rtsp_server.h"

#define RTP_DEFAULT_PORT    5004
#define RTP_DEFAULT_MTU     1500

typedef struct rtp_transport_s {
    int rtp_sock;
    int rtcp_sock;
    ev_io rtcp_reader;
    struct sockaddr_in remote_rtp;
    struct sockaddr_in remote_rtcp;
    unsigned rtp_channel;   // for rtp over tcp
    unsigned rtcp_channel;  // for rtp over tcp
    char is_tcp;
    void (*rtsp_client_session_error_notify)(void*);
    void *rtsp_client_session;
}rtp_transport_t;

typedef struct rtp_payload_info_s {
    /* the buf must have 12 free bytes ahead:  12 bytes + real rtp payload*/
    unsigned char* buf;
    unsigned int len;
    unsigned int timestamp;
    unsigned char maker;
}rtp_payload_info_t;


typedef struct rtp_sink_s{
    int (*send_packet)(struct rtp_sink_s* rtp, rtp_payload_info_t* info);

    /* the input buffer should have 12 bytes ahead the real data*/
    int (*send_packet_special)(struct rtp_sink_s* rtp, rtp_payload_info_t* info);
    int (*send_packet_vector)(struct rtp_sink_s* rtp, rtp_payload_info_t* info,
            struct iovec* vec, unsigned vec_num);
    int (*send_sr)(struct rtp_sink_s* rtp);
    unsigned short (*get_cur_seq_no)(struct rtp_sink_s* rtp);
    unsigned (*get_cur_timestamp)(struct rtp_sink_s* rtp);
    int (*add_transport)(struct rtp_sink_s* rtp, rtp_transport_t* transport);
    int (*remove_transport)(struct rtp_sink_s* rtp, rtp_transport_t* transport);
    int (*goodbye)(struct rtp_sink_s* rtp); //say goodbye to all transport
    unsigned int (*curr_timestamp)(struct rtp_sink_s* rtp, unsigned int ref_timestamp);
    void (*reset_dyBR)(struct rtp_sink_s* rtp);
    unsigned int (*get_start_ts)(struct rtp_sink_s* rtp);
    void (*set_start_ts)(struct rtp_sink_s* rtp, unsigned int ref_ts);
    unsigned char payload_type;
    unsigned char need_to_send_sr_sdes;
    void* priv;
}rtp_sink_t;

extern rtp_sink_t* rtp_sink_create(struct rtsp_server_s* srv, unsigned char payload_type,
    unsigned int clock_rate, uint8_t enableBR, uint8_t en_rtcp, uint8_t en_dspCLK, void* streamer);
extern void rtp_sink_release(rtp_sink_t* rtp);

#ifdef __cplusplus
}
#endif

#endif
