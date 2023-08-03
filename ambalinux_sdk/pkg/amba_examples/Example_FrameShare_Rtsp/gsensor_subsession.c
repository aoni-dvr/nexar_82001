#include <stdio.h>
#include <unistd.h>
#include "socket_helper.h"
#include "gsensor_subsession.h"
#include "gsensor_frame_reader.h"
#include "ambastream.h"

#define RTP_UDP_SOCK_BUF_SIZE   0x100000    // 1MByte
#define RTP_TCP_SOCK_BUF_SIZE   0x800000

static inline void skip_all_pipe_data(int fd_pipe_rd) {
    int len = 1;
    unsigned char what[128];

    if (fd_pipe_rd == 0) {
        return;
    }
    /* Read until buffer empty */
    while (len > 0) {
        len = read(fd_pipe_rd, &what, sizeof(what));
    }
}

static inline int start_stream_internal(struct gsensor_subsession_s* thiz) {
    gsensor_subs_priv_t* priv = (gsensor_subs_priv_t*)&thiz->priv;

    if (priv == NULL) {
        return -1;
    }
    if (priv->played_ref == 0) {
        skip_all_pipe_data(priv->st_fd[0]);
        if (priv->en_rtcp == 1) priv->rtp->send_sr(priv->rtp); // send first SR as early as possible
        ev_io_start(priv->srv->loop, &priv->framed_buf_watcher);
        priv->frame_reader->start(priv->frame_reader);
    }
    priv->played_ref++;

    return 0;
}

static inline int stop_stream_internal(struct gsensor_subsession_s* thiz) {
    gsensor_subs_priv_t* priv = (gsensor_subs_priv_t*)&thiz->priv;

    if (--priv->played_ref <= 0) {        
        priv->frame_reader->stop(priv->frame_reader);
        skip_all_pipe_data(priv->st_fd[0]);
        ev_io_stop(priv->srv->loop, &priv->framed_buf_watcher);
        priv->played_ref = 0;
    }

    return 0;
}

static int start_stream(struct gsensor_subsession_s* thiz,
        unsigned int session_id,
        unsigned short* rtp_seq_num,
        unsigned int* rtp_timstamp,
        int type) {
    unsigned i = 0;
    rtp_transport_t* transport = NULL;
    gsensor_subs_priv_t* priv = NULL;

    if (thiz == NULL){
        fprintf(stderr, "%s,%d\n", __FUNCTION__,__LINE__);
        return -1;
    }

    priv = (gsensor_subs_priv_t *)&thiz->priv;
    if (priv->rtp == NULL) {
        fprintf(stderr, "%s,%d\n", __FUNCTION__,__LINE__);
        return -1;
    }
    for (i = 0; i < MAX_RTP_SESSION; i++) {
        if (priv->map[i].session != 0 && priv->map[i].session == session_id) {
            transport = &priv->map[i].transport;
            break;
        }
    }
    if (transport == NULL) {
        fprintf(stderr, "%s,%d\n", __FUNCTION__,__LINE__);
        return -1;
    }
    priv->rtp->add_transport(priv->rtp, transport);
    if (rtp_seq_num != NULL) {
        *rtp_seq_num = priv->rtp->get_cur_seq_no(priv->rtp);
    }
    if (rtp_timstamp != NULL) {
        *rtp_timstamp = priv->rtp->curr_timestamp(priv->rtp,0);
        priv->rtp->set_start_ts(priv->rtp, *rtp_timstamp);
    }
    /* Use default flow for Live and PB streaming on a12 first. */
    if (type == RTSP_PLAY_LIVE) {
        priv->total_read_frames = 0;
    }
    start_stream_internal(thiz);

    return 0;
}

static int pause_stream(struct gsensor_subsession_s* thiz, unsigned int session_id, unsigned int type)
{
    unsigned i = 0;
    rtp_transport_t* transport = NULL;
    gsensor_subs_priv_t* priv = NULL;

    if (thiz == NULL) {
        fprintf(stderr, "%s,%d\n",__FUNCTION__,__LINE__);
        return -1;
    }
    priv = (gsensor_subs_priv_t *)&thiz->priv;
    if (priv->rtp == NULL) {
        return 0;
    }
    for (i = 0; i < MAX_RTP_SESSION; i++) {
        if(priv->map[i].session != 0 && priv->map[i].session == session_id) {
            transport = &priv->map[i].transport;
            break;
        }
    }
    if (transport == NULL) {
        fprintf(stderr, "%s,%d\n",__FUNCTION__,__LINE__);
        return -1;
    }

    if (type == RTSP_PLAY_STOP){// gsensor, regard pause as stop
        priv->rtp->remove_transport(priv->rtp, transport);
        stop_stream_internal(thiz);
    }

    return 0;
}

static int teardown(struct gsensor_subsession_s *thiz, unsigned session_id)
{
    unsigned i;
    gsensor_subs_priv_t *priv;

    if (thiz == NULL) {
        fprintf(stderr, "%s,%d\n", __FUNCTION__, __LINE__);
        return -1;
    }
    priv = (gsensor_subs_priv_t *)&thiz->priv;
    for (i = 0; i < MAX_RTP_SESSION; i++) {
        if (priv->map[i].session != 0 && priv->map[i].session == session_id) {
            priv->rtp->remove_transport(priv->rtp, &priv->map[i].transport);
            stop_stream_internal(thiz);
            if (priv->map[i].transport.is_tcp == 0) {
                close(priv->map[i].transport.rtcp_sock);
                close(priv->map[i].transport.rtp_sock);
            }
            memset(&priv->map[i], 0, sizeof(priv->map[i]));
            if (priv->used > 0){
                priv->used--;
            }
            break;
        }
    }
    if (priv->used == 0 && priv->rtp != NULL) {
        rtp_sink_release(priv->rtp);
        priv->rtp = NULL;
    }

    return (int)priv->used;
}

static int setup_udp_transport(struct gsensor_subsession_s* thiz,
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
    rtp_transport_t transport;
    unsigned i;
    gsensor_subs_priv_t *priv;

    if (thiz == NULL) {
        fprintf(stderr, "%s,%d\n", __FUNCTION__, __LINE__);
        return -1;
    }    
    priv = (gsensor_subs_priv_t *)&thiz->priv;
    if (priv->used >= MAX_RTP_SESSION) {
        fprintf(stderr, "%s,%d\n", __FUNCTION__,__LINE__);
        return -1;
    }
    if (priv->rtp == NULL) {
        priv->rtp = rtp_sink_create(priv->srv, 96 + thiz->track_num - 1, thiz->ref_clock, priv->en_dyBR,
            priv->en_rtcp, priv->en_dspCLK, NULL);
        if (priv->rtp == NULL) {
            fprintf(stderr, "create rtp sink failed\n");
            return -1;
        }
    }
    // dirty hack to 2000
    for (server_rtp_port = priv->start_port; server_rtp_port <= priv->start_port + 2000; server_rtp_port += 2) {
        rtp_sock = setup_datagram_socket(INADDR_ANY, server_rtp_port, 1);
        if (rtp_sock > 0) {
            rtcp_sock = setup_datagram_socket(INADDR_ANY, server_rtp_port + 1, 1);
            if (rtcp_sock > 0) {
                break;
            } else {
                close(rtp_sock);
                rtp_sock = -1;
            }
        }
    }

    if (rtp_sock <= 0) {
        fprintf(stderr, "Cannot find valid port for rtp socket!\n");
        rtp_sink_release(priv->rtp);
        return -1;
    } else if(rtcp_sock <= 0) {
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

    for (i = 0; i < MAX_RTP_SESSION; i++) {
        if (priv->map[i].session == 0) {
            priv->map[i].session = session_id;
            priv->map[i].transport = transport;
            priv->used++;
            break;
        }
    }

    if (i == MAX_RTP_SESSION) {
        fprintf(stderr, "Too many transport, not allowed\n");
        close(rtp_sock);
        close(rtcp_sock);
        return -1;
    }

    if (srv_rtp_port != NULL) {
        *srv_rtp_port = server_rtp_port;
    }
    if (srv_rtcp_port != NULL) {
        *srv_rtcp_port = server_rtp_port + 1;
    }

    return 0;
}

static int setup_tcp_transport(struct gsensor_subsession_s* thiz,
        unsigned session_id,
        int client_sock,
        unsigned rtp_channel,
        unsigned rtcp_channel,
        void (*error_notify)(void*),
        void *rtsp_client_session)
{
    rtp_transport_t transport;
    unsigned i;
    gsensor_subs_priv_t *priv;

    if (thiz == NULL) {
        fprintf(stderr, "%s,%d\n", __func__, __LINE__);
        return -1;
    }    
    priv = (gsensor_subs_priv_t *)&thiz->priv;
    if (priv->used >= MAX_RTP_SESSION) {
        fprintf(stderr, "%s,%d\n", __func__, __LINE__);
        return -1;
    }
    if (priv->rtp == NULL) {
        priv->rtp = rtp_sink_create(priv->srv, 96 + thiz->track_num - 1,
            thiz->ref_clock, priv->en_dyBR, priv->en_rtcp, priv->en_dspCLK, NULL);
        if (priv->rtp == NULL) {
            fprintf(stderr, "create rtp sink failed\n");
            return -1;
        }
        priv->cur_timestamp = priv->rtp->curr_timestamp(priv->rtp, 0);
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
    for (i = 0; i < MAX_RTP_SESSION; i++) {
        if (priv->map[i].session == 0) {
            priv->map[i].session = session_id;
            priv->map[i].transport = transport;
            priv->used++;
            break;
        }
    }

    if (i == MAX_RTP_SESSION) {
        fprintf(stderr, "Too many transport, not allowed\n");
        return -1;
    }

    return 0;
}

static void read_avail_cb(struct ev_loop* loop, ev_io* w, int revent)
{
    gsensor_subsession_t *subsession = (gsensor_subsession_t *)w->data;
    if (subsession == NULL) {
        return;
    }
    gsensor_subs_priv_t *priv =  (gsensor_subs_priv_t *)&subsession->priv;
    if (priv == NULL) {
        return;
    }
    unsigned char what = '0';
    int bytes_read = read(w->fd, &what, sizeof(what));

    if (bytes_read < sizeof(what)) {
        fprintf(stderr, "error when read the pipe %s:%s\n", __FILE__,__FUNCTION__);
        return;
    }
    if (what == 'd') {
        gsensor_frame_info_s frame;
        if (gsensor_queue_pop(priv->frame_reader->priv.queue, &frame) < 0) {
            return;
        }
        printf("got data to rtp: %.*s\n", frame.len, frame.buffer);
        priv->total_read_frames++;
    } else if (what == 'e') {
        // EOF
        fprintf(stderr, "EOS\n");
        priv->rtp->goodbye(priv->rtp);
    }
}

gsensor_subsession_t *gsensor_subsession_create(rtsp_server_t *srv)
{
    gsensor_subsession_t* media_sub;
    gsensor_subs_priv_t *priv = NULL;
    ev_io *io_watcher = NULL;

    media_sub = (gsensor_subsession_t *)malloc(sizeof(gsensor_subsession_t));
    if (media_sub == NULL) {
        fprintf(stderr, "%s,%d\n", __func__, __LINE__);
        return NULL;
    }
    media_sub->track_num = 1;
    media_sub->ref_clock = 1000;
    media_sub->priv.srv = srv;
    media_sub->priv.en_rtcp = 0;
    media_sub->priv.en_dyBR = 0;
    media_sub->priv.en_dspCLK = 0;
    media_sub->priv.start_port = 6974;
    media_sub->setup_udp_transport = setup_udp_transport;
    media_sub->setup_tcp_transport = setup_tcp_transport;
    media_sub->start_stream = start_stream;
    media_sub->pause_stream = pause_stream;
    media_sub->teardown = teardown;
    media_sub->release = gsensor_subsession_release;

    priv = (gsensor_subs_priv_t *)&media_sub->priv;
    if (pipe(priv->st_fd) < 0) {
        fprintf(stderr,"%s: priv->st_fd is NULL\n", __FUNCTION__);
        free(media_sub);
        return NULL;
    }
    if (make_socket_nonblocking(priv->st_fd[0]) < 0) {
        fprintf(stderr,"%s[%d]: make_socket_nonblocking returns fail.\n",__FUNCTION__,__LINE__);
    }
    if (make_socket_nonblocking(priv->st_fd[1]) < 0) {
        fprintf(stderr,"%s[%d]: make_socket_nonblocking returns fail.\n",__FUNCTION__,__LINE__);
    }
    priv->frame_reader = (gsensor_frame_reader_t *)gsensor_frame_reader_create(AmbaStreamer_GetGSensorStreamer(), priv->st_fd[1]);
    if (priv->frame_reader == NULL) {
        fprintf(stderr,"%s: priv->frame_reader is NULL\n", __FUNCTION__);
        close(priv->st_fd[0]);
        close(priv->st_fd[1]);
        free(media_sub);
        return NULL;
    }
    priv->framed_buf_watcher.data = media_sub;
    io_watcher = &priv->framed_buf_watcher;
    ev_io_init(io_watcher, read_avail_cb, priv->st_fd[0], EV_READ);

    return media_sub;
}

void gsensor_subsession_release(gsensor_subsession_t* thiz)
{
    gsensor_subs_priv_t* priv = NULL;

    if (thiz == NULL) {
        return;
    }
    priv = (gsensor_subs_priv_t*)&thiz->priv;
    if (priv == NULL) {
        fprintf(stderr,"%s: gsensor_subsession_t->priv is NULL\n", __FUNCTION__);
        return;
    }
    close(priv->st_fd[0]);
    close(priv->st_fd[1]);
    gsensor_frame_reader_release(priv->frame_reader);

    free(thiz);
}

