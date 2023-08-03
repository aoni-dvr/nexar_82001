#ifndef _RTSP_MEDIA_SUBSESSION_H_
#define _RTSP_MEDIA_SUBSESSION_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "media_session.h"
#include "frame_reader.h"
#include "rtp.h"
#include "rtsp_server.h"

#define RTSP_PLAY_LIVE      0
#define RTSP_PLAY_PB        1
#define RTSP_PLAY_SEEK      2
#define RTSP_PLAY_RESUME    3
#define RTSP_PLAY_PAUSE     4
#define RTSP_PLAY_STOP      5

typedef struct media_subsession_s {
    int (*start_stream)(struct media_subsession_s* thiz,
            unsigned int session_id,
            unsigned short* rtp_seq_num,
            unsigned int* rtp_timstamp,
            int type);
    int (*seek_stream)(struct media_subsession_s* thiz,
            unsigned int session_id,
            unsigned short* rtp_seq_num,
            unsigned int* rtp_timstamp,
            int type);
    int (*pause_stream)(struct media_subsession_s* thiz,
            unsigned int session_id,
            unsigned int type);
    int (*setup_udp_transport)(struct media_subsession_s* thiz,
            unsigned int session_id,
            struct in_addr cln_addr,
            unsigned short cln_rtp_port,
            unsigned short cln_rtcp_port,
            unsigned short* srv_rtp_port,
            unsigned short* srv_rtcp_port,
            void (*error_notify)(void*),
            void *rtsp_client_session);
    int (*setup_tcp_transport)(struct media_subsession_s* thiz,
            unsigned session_id,
            int client_sock,
            unsigned rtp_channel,
            unsigned rtcp_channel,
            void (*error_notify)(void*),
            void *rtsp_client_session);
    int (*teardown)(struct media_subsession_s* thiz, unsigned session_id);
    void (*release)(struct media_subsession_s* thiz);
    const char* (*get_sdp_lines)(struct media_subsession_s* thiz);
    const char* (*get_track_id)(struct media_subsession_s* thiz);
    unsigned int track_num;
    unsigned int ref_clock;
    struct media_session_s* parent;
    void* priv;
    void* extra_info;
} media_subsession_t;

typedef struct transport_map_s {
    unsigned session;
    rtp_transport_t transport;
}transport_map_t;

typedef struct media_subs_priv_s {
    rtsp_server_t* srv;     // server instance
    rtp_sink_t* rtp;        // rtp sender
    char* sdp_lines;        // the sdp description of this subsession
    char* aux_sdp_line;
    char* track_id;
    transport_map_t map[MAX_RTP_SESSION];
    unsigned short start_port;  // we find usable port begin from this number
    unsigned char played_ref;
    unsigned char used;
    struct frame_reader* frame_reader;
    struct frame_info frame;
    int st_fd[2];   /*0 for read*/
    ev_io framed_buf_watcher;
    unsigned int cur_timestamp;
    unsigned int last_pts;
    unsigned int next_sr_pts;
    rtp_payload_info_t rtp_payload_info;
    uint8_t en_rtcp;
    uint8_t en_dyBR;        //dynamic bitrate control
    uint8_t en_dspCLK;      //use dsp clock
    uint8_t trackType;
    unsigned int ts_offset;
    unsigned int total_read_frames;
    unsigned int total_read_pb_frames;
} media_subs_priv_t;

extern int media_subsession_init(media_subsession_t* thiz, rtsp_server_t* srv,
        unsigned int en_rtcp,
        unsigned int en_dyBR,
        unsigned int en_dspCLK,
        unsigned int rtp_start_port,
        unsigned int hdr_size,  //rtp header size
        void* streamer,
        void (*frame_cb)(struct ev_loop*, struct ev_io* , int));

extern void media_subsession_deinit(media_subsession_t* thiz);

extern media_subsession_t* amba_h264_subsession_create(rtsp_server_t* srv,
        void* streamer,
        unsigned int en_rtcp,
        unsigned int playback,
        unsigned int ticks,
        unsigned int en_ap);
extern media_subsession_t* amba_h265_subsession_create(rtsp_server_t* srv,
        void* streamer,
        unsigned int en_rtcp,
        unsigned int playback,
        unsigned int ticks,
        unsigned int en_ap);
extern media_subsession_t* amba_aac_subsession_create(rtsp_server_t* srv,
        void* streamer,
        unsigned int en_rtcp,
        unsigned int playback);
extern media_subsession_t* amba_t140_subsession_create(rtsp_server_t* srv,
        void* streamer,
        unsigned int en_rtcp,
        unsigned int playback);
extern media_subsession_t* amba_opus_subsession_create(rtsp_server_t* srv,
        void* streamer,
        unsigned int en_rtcp,
        unsigned int playback);

#ifdef __cplusplus
}
#endif

#endif
