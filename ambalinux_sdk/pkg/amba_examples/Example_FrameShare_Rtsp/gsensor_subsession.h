#ifndef __GSENSOR_SUBSESSION_H__
#define __GSENSOR_SUBSESSION_H__

#include <string.h>
#include <stdlib.h>
#include "rtp.h"
#include "rtsp_server.h"
#include "media_subsession.h"
#include "gsensor_frame_reader.h"

typedef struct gsensor_subs_priv_s {    
    rtsp_server_t* srv;     // server instance    
    rtp_sink_t* rtp;        // rtp sender
    unsigned short start_port;  // we find usable port begin from this number    
    unsigned char played_ref;    
    unsigned char used;    
    struct gsensor_frame_reader* frame_reader;
    transport_map_t map[MAX_RTP_SESSION];    
    int st_fd[2];   /*0 for read*/
    uint8_t en_rtcp;
    uint8_t en_dyBR;        //dynamic bitrate control
    uint8_t en_dspCLK;      //use dsp clock    
    unsigned int cur_timestamp;    
    unsigned int total_read_frames;
    ev_io framed_buf_watcher;
} gsensor_subs_priv_t;

typedef struct gsensor_subsession_s {
    int (*start_stream)(struct gsensor_subsession_s* thiz,
            unsigned int session_id,
            unsigned short* rtp_seq_num,
            unsigned int* rtp_timstamp,
            int type);
    int (*pause_stream)(struct gsensor_subsession_s* thiz,
            unsigned int session_id,
            unsigned int type);
    int (*setup_udp_transport)(struct gsensor_subsession_s* thiz,
            unsigned int session_id,
            struct in_addr cln_addr,
            unsigned short cln_rtp_port,
            unsigned short cln_rtcp_port,
            unsigned short* srv_rtp_port,
            unsigned short* srv_rtcp_port,
            void (*error_notify)(void*),
            void *rtsp_client_session);
    int (*setup_tcp_transport)(struct gsensor_subsession_s* thiz,
            unsigned session_id,
            int client_sock,
            unsigned rtp_channel,
            unsigned rtcp_channel,
            void (*error_notify)(void*),
            void *rtsp_client_session);
    int (*teardown)(struct gsensor_subsession_s* thiz, unsigned session_id);
    void (*release)(struct gsensor_subsession_s* thiz);
    gsensor_subs_priv_t priv;
    unsigned int track_num;
    unsigned int ref_clock;
} gsensor_subsession_t;

gsensor_subsession_t *gsensor_subsession_create(rtsp_server_t *srv);
void gsensor_subsession_release(gsensor_subsession_t* thiz);

#endif//__GSENSOR_SUBSESSION_H__

