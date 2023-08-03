#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h> /* sockaddr_in{} and other Internet defns */
#include <arpa/inet.h>  /* inet(3) functions */

#include "ev.h"
#include "socket_helper.h"
#include "text_frame_reader.h"
#include "video_frame_reader.h"
#include "audio_frame_reader.h"
#include "media_subsession.h"
#include "ambastream.h"
#include "amba_frameshare.h"

#define RTP_UDP_SOCK_BUF_SIZE   0x400000    // 4MByte
#define RTP_TCP_SOCK_BUF_SIZE   0x800000
#define REF_CLOCK (1000)

static void read_avail_cb(struct ev_loop* loop, ev_io* w, int revent)
{
    fprintf(stderr, "[%s] %d: This function is suppose to be rewrite\n", __FUNCTION__, __LINE__);
}

static inline void skip_all_pipe_data(int fd_pipe_rd) {
    int len = 1;
    unsigned char what[128];

    if(fd_pipe_rd == 0) {
        return;
    }
    /* Read until buffer empty */
    while(len > 0){
        len = read(fd_pipe_rd, &what, sizeof(what));
    }
}

/* called after preload completed */
static inline int pb_stream_start(void *ctx)
{
    struct media_subsession_s* thiz = NULL;
    media_subs_priv_t* priv = NULL;
    ev_io *io_watch;

    thiz = (struct media_subsession_s *)ctx;
    priv = (media_subs_priv_t*)thiz->priv;
    io_watch = &priv->framed_buf_watcher;
    ev_io_start(priv->srv->loop, io_watch);
    priv->srv->activate_loop(priv->srv);    //need to async activate ev_loop because we're in ipc_svc call
    return 0;
}

static inline int start_stream_internal(struct media_subsession_s* thiz)
{
    media_subs_priv_t* priv = (media_subs_priv_t*)thiz->priv;

    if(priv->played_ref == 0) {
        //XXX
        skip_all_pipe_data(priv->st_fd[0]);
        if(priv->en_rtcp == 1) priv->rtp->send_sr(priv->rtp); // send first SR as early as possible
        ev_io_start(priv->srv->loop, &priv->framed_buf_watcher);
        priv->frame_reader->start(priv->frame_reader);
    }
    priv->played_ref++;
    return 0;
}

static inline int stop_stream_internal(struct media_subsession_s* thiz){
    media_subs_priv_t* priv = (media_subs_priv_t*)thiz->priv;

    if(priv->played_ref==0){
        fprintf(stderr, "%s: not init yet!\n",__FUNCTION__);
        return 0;
    }

    if(--priv->played_ref <= 0) {
        priv->frame_reader->stop(priv->frame_reader);
        skip_all_pipe_data(priv->st_fd[0]);
        ev_io_stop(priv->srv->loop, &priv->framed_buf_watcher);
        priv->played_ref = 0;
    }

    return 0;
}
static int start_stream(struct media_subsession_s* thiz,
        unsigned int session_id,
        unsigned short* rtp_seq_num,
        unsigned int* rtp_timstamp,
        int type) {
    unsigned i = 0;
    rtp_transport_t* transport = NULL;
    media_subs_priv_t* priv = NULL;
    if(thiz == NULL){
        fprintf(stderr, "%s,%d\n",__FUNCTION__,__LINE__);
        return -1;
    }

    priv = (media_subs_priv_t*)thiz->priv;
    if(priv->rtp == NULL) {
        fprintf(stderr, "%s,%d\n",__FUNCTION__,__LINE__);
        return -1;
    }

    for(i = 0; i < MAX_RTP_SESSION; i++) {
        if(priv->map[i].session != 0 && priv->map[i].session == session_id) {
            transport = &priv->map[i].transport;
            break;
        }
    }

    if(transport == NULL) {
        fprintf(stderr, "%s,%d\n",__FUNCTION__,__LINE__);
        return -1;
    }

    priv->rtp->add_transport(priv->rtp, transport);
    if(rtp_seq_num != NULL) {
        *rtp_seq_num = priv->rtp->get_cur_seq_no(priv->rtp);
    }
    if(rtp_timstamp != NULL) {
        *rtp_timstamp = priv->rtp->curr_timestamp(priv->rtp,0);
        priv->rtp->set_start_ts(priv->rtp, *rtp_timstamp);
    }

#if 1
    /* Use default flow for Live and PB streaming on a12 first. */
    if(type==RTSP_PLAY_LIVE){
        priv->total_read_frames = 0;
    } else {
        priv->total_read_pb_frames = 0;
    }
    start_stream_internal(thiz);
#else
    if(type==RTSP_PLAY_LIVE){
        start_stream_internal(thiz);
    }else{
        start_stream_preload(thiz);
    }
#endif

    return 0;
}

static int seek_stream(struct media_subsession_s* thiz,
        unsigned int session_id,
        unsigned short* rtp_seq_num,
        unsigned int* rtp_timstamp,
        int type)
{
    media_subs_priv_t* priv = NULL;
    if(thiz == NULL){
        fprintf(stderr, "%s,%d\n",__FUNCTION__,__LINE__);
        return -1;
    }
    priv = (media_subs_priv_t*)thiz->priv;
    if(priv->rtp == NULL) {
        return 0;
    }

    if(rtp_seq_num != NULL) {
        *rtp_seq_num = priv->rtp->get_cur_seq_no(priv->rtp);
    }
    if(rtp_timstamp != NULL) {
        *rtp_timstamp = priv->rtp->curr_timestamp(priv->rtp,0);
        priv->rtp->set_start_ts(priv->rtp, *rtp_timstamp);
    }
    if(type==RTSP_PLAY_SEEK){
        priv->frame_reader->clr_buf(priv->frame_reader);
        skip_all_pipe_data(priv->st_fd[0]);
    }
    ev_io_start(priv->srv->loop, &priv->framed_buf_watcher);

    return 0;
}

static int pause_stream(struct media_subsession_s* thiz, unsigned int session_id, unsigned int type)
{
    unsigned i = 0;
    rtp_transport_t* transport = NULL;
    media_subs_priv_t* priv = NULL;
    if(thiz == NULL){
        fprintf(stderr, "%s,%d\n",__FUNCTION__,__LINE__);
        return -1;
    }

    priv = (media_subs_priv_t*)thiz->priv;
    if(priv->rtp == NULL) {
        return 0;
    }
    for(i = 0; i < MAX_RTP_SESSION; i++) {
        if(priv->map[i].session != 0 && priv->map[i].session == session_id) {
            transport = &priv->map[i].transport;
            break;
        }
    }
    if(transport == NULL) {
        fprintf(stderr, "%s,%d\n",__FUNCTION__,__LINE__);
        return -1;
    }

    if(type == RTSP_PLAY_STOP){// liveview, regard pause as stop
        priv->rtp->remove_transport(priv->rtp, transport);
        stop_stream_internal(thiz);
    }else if(type == RTSP_PLAY_PAUSE){// playback pause
        priv->total_read_pb_frames=0;//reset stream
        ev_io_stop(priv->srv->loop, &priv->framed_buf_watcher);
    }

    return 0;
}
static int teardown(struct media_subsession_s* thiz, unsigned session_id)
{
    unsigned i;
    media_subs_priv_t* priv = NULL;
    if(thiz == NULL){
        fprintf(stderr, "%s,%d\n",__FUNCTION__,__LINE__);
        return -1;
    }

    priv = (media_subs_priv_t*)thiz->priv;

    for(i = 0; i < MAX_RTP_SESSION; i++) {
        if(priv->map[i].session != 0 && priv->map[i].session == session_id) {
            priv->rtp->remove_transport(priv->rtp, &priv->map[i].transport);
            stop_stream_internal(thiz);
            if(priv->map[i].transport.is_tcp == 0) {
                close(priv->map[i].transport.rtcp_sock);
                close(priv->map[i].transport.rtp_sock);
            }
            memset(&priv->map[i], 0, sizeof(priv->map[i]));

            /* Due to the video resolution might changed time by time, */
            /* the SPD should be updated ervery time.                  */
            if (priv->aux_sdp_line != NULL){
                free(priv->aux_sdp_line);
                priv->aux_sdp_line = NULL;
            }

            if(priv->used > 0){
                priv->used--;
            }
            break;
        }
    }
    if(priv->used == 0 && priv->rtp != NULL) {
        rtp_sink_release(priv->rtp);
        priv->rtp = NULL;
    }

    return (int)priv->used;
}

static int setup_tcp_transport(struct media_subsession_s* thiz,
        unsigned session_id,
        int client_sock,
        unsigned rtp_channel,
        unsigned rtcp_channel,
        void (*error_notify)(void*),
        void *rtsp_client_session)
{
    rtp_transport_t transport;
    unsigned i;
    int max_transport = 0;
    media_subs_priv_t* priv = NULL;
    if(thiz == NULL) {
        fprintf(stderr, "%s,%d\n",__FUNCTION__,__LINE__);
        return -1;
    }

    priv = (media_subs_priv_t*)thiz->priv;
    if(priv->used >= MAX_RTP_SESSION) {
        fprintf(stderr, "%s,%d\n",__FUNCTION__,__LINE__);
        return -1;
    }

    if(priv->rtp == NULL) {
        priv->rtp = rtp_sink_create(priv->srv, 96 + thiz->track_num - 1,
            thiz->ref_clock, priv->en_dyBR, priv->en_rtcp, priv->en_dspCLK, frame_reader_get_streamer(priv->frame_reader));
        if(priv->rtp == NULL) {
            fprintf(stderr, "create rtp sink failed\n");
            return -1;
        }
        priv->cur_timestamp = priv->rtp->curr_timestamp(priv->rtp,0);
    }

    memset(&transport, 0, sizeof(rtp_transport_t));
    transport.is_tcp = 1;
    transport.rtp_channel = rtp_channel;
    transport.rtcp_channel = rtcp_channel;
    transport.rtp_sock = client_sock;
    transport.rtcp_sock = client_sock;

    transport.rtsp_client_session_error_notify = error_notify;
    transport.rtsp_client_session = rtsp_client_session;

    increase_snd_buf(client_sock, RTP_TCP_SOCK_BUF_SIZE);  //use a bigger buffer
    enable_tcp_nodelay(client_sock);

    // we do not allow multiple playback on the same file(multiple seek is confusing)
    max_transport = AmbaStreamer_IsLive(frame_reader_get_streamer(priv->frame_reader)) ? MAX_RTP_SESSION : 1;

    for(i = 0; i < max_transport; i++) {
        if(priv->map[i].session == 0 ) {
            priv->map[i].session = session_id;
            priv->map[i].transport = transport;
            priv->used++;
            break;
        }
    }

    if (i == max_transport) {
        fprintf(stderr, "Too many transport, not allowed\n");
        return -1;
    }
    return 0;
}

static int setup_udp_transport(struct media_subsession_s* thiz,
        unsigned session_id,
        struct in_addr cln_addr,
        unsigned short cln_rtp_port,
        unsigned short cln_rtcp_port,
        unsigned short* srv_rtp_port,
        unsigned short* srv_rtcp_port,
        void (*error_notify)(void*),
        void *rtsp_client_session)
{
    unsigned short server_rtp_port;
    int rtp_sock = -1;
    int rtcp_sock = -1;
    int max_transport = 0;
    rtp_transport_t transport;
    unsigned i = 0;
    media_subs_priv_t* priv = NULL;
    if(thiz == NULL) {
        fprintf(stderr, "%s,%d\n",__FUNCTION__,__LINE__);
        return -1;
    }

    priv = (media_subs_priv_t*)thiz->priv;
    if(priv->used >= MAX_RTP_SESSION) {
        fprintf(stderr, "%s,%d\n",__FUNCTION__,__LINE__);
        return -1;
    }

    if(priv->rtp == NULL) {
        priv->rtp = rtp_sink_create(priv->srv, 96 + thiz->track_num - 1, thiz->ref_clock, priv->en_dyBR,
            priv->en_rtcp, priv->en_dspCLK, frame_reader_get_streamer(priv->frame_reader));
        if(priv->rtp == NULL) {
            fprintf(stderr, "create rtp sink failed\n");
            return -1;
        }
    }

    // dirty hack to 2000
    for(server_rtp_port = priv->start_port; server_rtp_port <= priv->start_port + 2000; server_rtp_port += 2) {
        rtp_sock = setup_datagram_socket(INADDR_ANY, server_rtp_port, 1);
        if(rtp_sock > 0) {
            rtcp_sock = setup_datagram_socket(INADDR_ANY, server_rtp_port + 1, 1);
            if(rtcp_sock > 0) {
                break;
            } else {
                close(rtp_sock);
                rtp_sock = -1;
            }
        }
    }

    if(rtp_sock<=0) {
        fprintf(stderr, "Cannot find valid port for rtp socket!\n");
        rtp_sink_release(priv->rtp);
        return -1;
    } else if(rtcp_sock<=0) {
        fprintf(stderr, "Cannot find valid port for rtcp socket!\n");
        close(rtp_sock);
        rtp_sink_release(priv->rtp);
        return -1;
    }

    memset(&transport, 0, sizeof(rtp_transport_t));
    transport.rtp_sock = rtp_sock;
    transport.rtcp_sock = rtcp_sock;
    transport.remote_rtp.sin_family = AF_INET;
    transport.remote_rtp.sin_addr   = cln_addr;
    transport.remote_rtp.sin_port   = htons(cln_rtp_port);

    transport.remote_rtcp.sin_family = AF_INET;
    transport.remote_rtcp.sin_addr   = cln_addr;
    transport.remote_rtcp.sin_port   = htons(cln_rtcp_port);

    transport.rtsp_client_session_error_notify = error_notify;
    transport.rtsp_client_session = rtsp_client_session;

    //XXX:
    connect(transport.rtp_sock, (struct sockaddr*)&transport.remote_rtp, sizeof(transport.remote_rtp));
    connect(transport.rtcp_sock, (struct sockaddr*)&transport.remote_rtcp, sizeof(transport.remote_rtcp));
    increase_snd_buf(rtp_sock, RTP_UDP_SOCK_BUF_SIZE);  //use a bigger buffer

    // we do not allow multiple playback on the same file(multiple seek is confusing)
    max_transport = AmbaStreamer_IsLive(frame_reader_get_streamer(priv->frame_reader)) ? MAX_RTP_SESSION : 1;
    for(i = 0; i < max_transport; i++) {
        if(priv->map[i].session == 0 ) {
            //memset(&priv->map[i], 0, sizeof(priv->map[i]));
            priv->map[i].session = session_id;
            priv->map[i].transport = transport;
            priv->used++;
            break;
        }
    }

    if (i == max_transport) {
        fprintf(stderr, "Too many transport, not allowed\n");
        close(rtp_sock);
        close(rtcp_sock);
        return -1;
    }

    if(srv_rtp_port != NULL) {
        *srv_rtp_port = server_rtp_port;
    }
    if(srv_rtcp_port != NULL) {
        *srv_rtcp_port = server_rtp_port + 1;
    }

    return 0;
}

static const char* get_track_id(struct media_subsession_s* thiz)
{
    media_subs_priv_t* priv = NULL;
    if(thiz == NULL) {
        return NULL;
    }

    priv = (media_subs_priv_t*)thiz->priv;
    if(thiz->track_num == 0) {
        return NULL;
    }
    if(priv->track_id == NULL) {
        priv->track_id = (char*)malloc(8);  //XXX: enough for track0 ... track10
        sprintf(priv->track_id, "track%d", thiz->track_num);
    }

    return priv->track_id;
}
static const char* get_sdp_lines(struct media_subsession_s* thiz)
{
    fprintf(stderr, "[%s] %d: this func is supposed to be rewrite\n", __FUNCTION__, __LINE__);
    return NULL;
}
/********************************************************************************
 * public function
 ********************************************************************************/
int media_subsession_init(media_subsession_t* thiz,
        rtsp_server_t* srv,
        unsigned int en_rtcp,
        unsigned int en_dyBR,
        unsigned int en_dspCLK,
        unsigned int rtp_start_port,
        unsigned int hdr_size,
        void* streamer,
        void (*frame_cb)(struct ev_loop*, struct ev_io* , int))
{
    ev_io *io_watcher;

    if(thiz == NULL) {
        printf("Error: %s,%d\n",__FUNCTION__,__LINE__);
        return -1;
    }
    memset(thiz, 0, sizeof(media_subsession_t));

    media_subs_priv_t* priv = (media_subs_priv_t*)malloc(sizeof(media_subs_priv_t));
    if(priv == NULL) {
        printf("Error: %s,%d\n",__FUNCTION__,__LINE__);
        free(thiz);
        return -1;
    }
    memset(priv, 0 , sizeof(media_subs_priv_t));

    thiz->start_stream = start_stream;
    thiz->seek_stream = seek_stream;
    thiz->pause_stream = pause_stream;
    thiz->teardown = teardown;
    thiz->get_track_id = get_track_id;
    thiz->setup_udp_transport = setup_udp_transport;
    thiz->setup_tcp_transport = setup_tcp_transport;
    thiz->get_sdp_lines = get_sdp_lines;                       //expect to be rewrite
    thiz->ref_clock = REF_CLOCK;

    priv->srv = srv;
    priv->start_port = rtp_start_port;
    priv->en_rtcp = en_rtcp;
    priv->en_dyBR = en_dyBR;
    priv->en_dspCLK = en_dspCLK;
    priv->trackType = AmbaStreamer_GetTrackType(streamer);

    if(pipe(priv->st_fd) < 0) {
        fprintf(stderr,"%s: priv->st_fd is NULL\n", __FUNCTION__);
        free(priv);
        free(thiz);
        return -1;
    }
    if(make_socket_nonblocking(priv->st_fd[0])<0){
        fprintf(stderr,"%s[%d]: make_socket_nonblocking returns fail.\n",__FUNCTION__,__LINE__);
    }
    if(make_socket_nonblocking(priv->st_fd[1])<0){
        fprintf(stderr,"%s[%d]: make_socket_nonblocking returns fail.\n",__FUNCTION__,__LINE__);
    }

    if ((priv->trackType >= AMBA_EXAMFRAMESHARERTSP_MEDIA_TRACK_TYPE_VIDEO_MIN_IDX) &&
        (priv->trackType <= AMBA_EXAMFRAMESHARERTSP_MEDIA_TRACK_TYPE_VIDEO_MAX_IDX)) {

                priv->frame_reader = (frame_reader_t *)
                video_frame_reader_create(priv->st_fd[1], hdr_size, streamer);

    } else if ((priv->trackType >= AMBA_EXAMFRAMESHARERTSP_MEDIA_TRACK_TYPE_AUDIO_MIN_IDX) &&
               (priv->trackType <= AMBA_EXAMFRAMESHARERTSP_MEDIA_TRACK_TYPE_AUDIO_MAX_IDX)) {

                priv->frame_reader = (frame_reader_t *)
                audio_frame_reader_create(priv->st_fd[1], hdr_size, streamer);

    } else if ((priv->trackType >= AMBA_EXAMFRAMESHARERTSP_MEDIA_TRACK_TYPE_TEXT_MIN_IDX) &&
               (priv->trackType <= AMBA_EXAMFRAMESHARERTSP_MEDIA_TRACK_TYPE_TEXT_MAX_IDX)) {

                priv->frame_reader = (frame_reader_t *)
                text_frame_reader_create(priv->st_fd[1], hdr_size, streamer);

    } else {
                fprintf(stderr, "%s: unrecognized stream type\n", __FUNCTION__);
                priv->frame_reader = NULL;
    }

    if(priv->frame_reader == NULL) {
        fprintf(stderr,"%s: priv->frame_reader is NULL\n", __FUNCTION__);
        close(priv->st_fd[0]);
        close(priv->st_fd[1]);
        free(priv);
        free(thiz);
        return -1;
    }
    priv->framed_buf_watcher.data= thiz;
    io_watcher = &priv->framed_buf_watcher;
    if(frame_cb == NULL) {
        ev_io_init(io_watcher, read_avail_cb, priv->st_fd[0]/*read*/, EV_READ);
    }else {
        ev_io_init(io_watcher, frame_cb, priv->st_fd[0], EV_READ);
    }
    thiz->priv = priv;

    return 0;
}

void media_subsession_deinit(media_subsession_t* media_subsession)
{
    media_subs_priv_t* priv = NULL;

    if(media_subsession == NULL) {
        fprintf(stderr,"%s: media_subsession is NULL\n", __FUNCTION__);
        return;
    }
    priv = (media_subs_priv_t*)media_subsession->priv;
    if(priv == NULL) {
        fprintf(stderr,"%s: media_subsession->priv is NULL\n", __FUNCTION__);
        return;
    }
    close(priv->st_fd[0]);
    close(priv->st_fd[1]);
    if(priv->track_id != NULL)  free(priv->track_id);
    if(priv->aux_sdp_line != NULL)  free(priv->aux_sdp_line);
    if(priv->sdp_lines != NULL) free(priv->sdp_lines);

    if ((priv->trackType >= AMBA_EXAMFRAMESHARERTSP_MEDIA_TRACK_TYPE_VIDEO_MIN_IDX) &&
        (priv->trackType <= AMBA_EXAMFRAMESHARERTSP_MEDIA_TRACK_TYPE_VIDEO_MAX_IDX)) {

                video_frame_reader_release((video_frame_reader_t*)priv->frame_reader);

    } else if ((priv->trackType >= AMBA_EXAMFRAMESHARERTSP_MEDIA_TRACK_TYPE_AUDIO_MIN_IDX) &&
               (priv->trackType <= AMBA_EXAMFRAMESHARERTSP_MEDIA_TRACK_TYPE_AUDIO_MAX_IDX)) {

                audio_frame_reader_release((audio_frame_reader_t*)priv->frame_reader);

    } else if ((priv->trackType >= AMBA_EXAMFRAMESHARERTSP_MEDIA_TRACK_TYPE_TEXT_MIN_IDX) &&
               (priv->trackType <= AMBA_EXAMFRAMESHARERTSP_MEDIA_TRACK_TYPE_TEXT_MAX_IDX)) {

                text_frame_reader_release(priv->frame_reader);

    } else {
                fprintf(stderr, "%s: unrecognized stream type\n", __FUNCTION__);
    }
    free(priv);
}
