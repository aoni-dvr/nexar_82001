#include <sys/signalfd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <arpa/inet.h>

#include "ev.h"
#include "rtsp_server.h"
#include "rtsp_client_session.h"
#include "media_session.h"
#include "media_subsession.h"
#include "random.h"
#include "ambastream.h"
#include "socket_helper.h"

//#define DEBUG_SERVER


/**************************************************************************
  *
  *************************************************************************/
typedef struct rtsp_server_priv_s {
    int sock;
    ev_io sock_watcher;
    ev_async exit_watcher;
    ev_async active_watcher;
    unsigned short port;
    unsigned char en_ap;
    unsigned char padding;
    media_session_t* sessions[MAX_MEDIA_SESSION];
    char* rtsp_url;
}rtsp_server_priv_t;

static void exit_loop_cb(struct ev_loop* loop, ev_async* w, int revent){
    ev_break(loop, EVBREAK_ALL);
}

static void active_loop_cb(struct ev_loop* loop, ev_async* w, int revent){
    printf("ev_loop activated by async send\n\r");
}
static int setup_socket(unsigned short port) {
    int sock = setup_stream_socket(INADDR_ANY, port, 1);
    if(sock < 0) {
        return sock;
    }
    if(!increase_snd_buf( sock, 50*1024)) {
        close(sock);
        return -1;
    }
    if(listen(sock, 2) < 0) {
        close(sock);
        return -1;
    }
    return sock;
}
void activate_loop(struct rtsp_server_s* thiz){
    rtsp_server_priv_t* priv = NULL;
    if(thiz == NULL) {
        return;
    }
    priv = (rtsp_server_priv_t*)thiz->priv;
    ev_async_send(thiz->loop, &priv->active_watcher);
}
void exit_loop(struct rtsp_server_s* thiz) {
    rtsp_server_priv_t* priv = NULL;
    if(thiz == NULL) {
        return;
    }
    priv = (rtsp_server_priv_t*)thiz->priv;
    ev_async_send(thiz->loop, &priv->exit_watcher);
}

static int add_media_session(rtsp_server_t* thiz, media_session_t* media_session) {
    int i = 0;
    rtsp_server_priv_t* priv = NULL;

    if(thiz == NULL) {
        fprintf(stderr, "add_media_session: thiz is NULL\n");
        return -EINVAL;
    }

    if(media_session == NULL) {
        fprintf(stderr, "add_media_session: media_session is NULL\n");
        return -EINVAL;
    }

    priv = (rtsp_server_priv_t*)thiz->priv;
    for(i = 0; i < MAX_MEDIA_SESSION; i++ ) {
        if(priv->sessions[i] == NULL) {
            //fprintf(stderr, "get slot %d for media sessions(%p)\n",i,media_session);
            break;
        }
    }

    if(i >= MAX_MEDIA_SESSION) {
        fprintf(stderr, "too many media sessions\n");
        return -ENOSPC;
    }

    priv->sessions[i] = media_session;

    return 0;
}

static int remove_media_session(rtsp_server_t* thiz, media_session_t* media_session) {
    int i = 0;
    rtsp_server_priv_t* priv = NULL;

    fprintf(stderr, ">>> remove_media_session\n");
    if(thiz == NULL) {
        return -EINVAL;
    }

    priv = (rtsp_server_priv_t*)thiz->priv;
    for(i = 0; i < MAX_MEDIA_SESSION; i++ ) {
        if(priv->sessions[i] == media_session) {
            priv->sessions[i] = NULL;
            return 0;
        }
    }
    return 0;
}

// 0: internal(default), 1: external
static int rtsp_server_get_conf_rtsp_live_camera(void)
{
    //rtsp_live
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read = 0;
    int channel = 0;

    fp = fopen("/pref/settings.ini", "r");
    if (fp == NULL) {
        return -1;
    }
    while ((read = getline(&line, &len, fp)) != -1) {
        if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = '\0';
        }
        if (line[strlen(line) - 1] == '\r') {
            line[strlen(line) - 1] = '\0';
        }
        if (strncmp(line, "rtsp_live=", strlen("rtsp_live=")) == 0) {
            channel = atoi(line + strlen("rtsp_live=")) ? 1 : 0;
            break;
        }
    }
    if (line) {
        free(line);
    }
    fclose(fp);

    return channel;
}

/*
 *  init liveview session and create media_session, subsessions
 */
static media_session_t* init_liveview_session(rtsp_server_t* thiz, const char* streamName)
{
    int mediaCount, idx;
    const int enableRTCP = 1, flagLiveView = 0, dontCare = 0;
    rtsp_server_priv_t* priv = NULL;
    media_session_t* media_session;
    media_subsession_t* amba_sub;
    AmbaStreamer_MediaList_t mediaList[MAX_MEDIA_SUBSESSIONS];
    char *name = NULL;
    int retval = 0;

    if (thiz == NULL) {
        return NULL;
    }
    priv = (rtsp_server_priv_t*) thiz->priv;
    fprintf(stderr, "init liveview session(%s)\n", streamName);
    if (strcmp(streamName, DEFAULT_VIEWFINDER_STREAM) == 0) {
        if (rtsp_server_get_conf_rtsp_live_camera() == 0) {
            name = "live-internal";
        } else {
            name = "live-external";
        }
    } else {
        if (strcmp(streamName, DEFAULT_VIEWFINDER_STREAM_INTERNAL) == 0) {
            name = "live-internal";
        } else if (strcmp(streamName, DEFAULT_VIEWFINDER_STREAM_EXTERNAL) == 0) {
            name = "live-external";
        } else {
            name = (char *)streamName;
        }
    }
    mediaCount = AmbaStreamerLive_GetMediaID(name, mediaList);
    if (mediaCount <= 0) {
        return NULL;
    }
    fprintf(stderr, "create liveview session(%s)\n", name);
    media_session = media_session_create(name, NULL, NULL);
    if (media_session == NULL) {
        return NULL;
    }
    for (idx = 0; idx < mediaCount; idx++) {
        switch (mediaList[idx].mediaId) {
            case AMP_FORMAT_MID_H264:
            case AMP_FORMAT_MID_AVC:
                amba_sub = amba_h264_subsession_create(
                        thiz, mediaList[idx].stm, enableRTCP, flagLiveView, dontCare, priv->en_ap);
                media_session->add_media_subsession(media_session, amba_sub);
                break;
            case AMP_FORMAT_MID_H265:
            case AMP_FORMAT_MID_HVC:
                amba_sub = amba_h265_subsession_create(
                        thiz, mediaList[idx].stm, enableRTCP, flagLiveView, dontCare, priv->en_ap);
                media_session->add_media_subsession(media_session, amba_sub);
                break;
            case AMP_FORMAT_MID_AAC:
                amba_sub = amba_aac_subsession_create(thiz, mediaList[idx].stm, enableRTCP, flagLiveView);
                media_session->add_media_subsession(media_session, amba_sub);
                break;
            case AMP_FORMAT_MID_OPUS:
                amba_sub = amba_opus_subsession_create(thiz, mediaList[idx].stm, enableRTCP, flagLiveView);
                media_session->add_media_subsession(media_session, amba_sub);
                break;
            case AMP_FORMAT_MID_TEXT:
                amba_sub = amba_t140_subsession_create(thiz, mediaList[idx].stm, enableRTCP, flagLiveView);
                media_session->add_media_subsession(media_session, amba_sub);
                break;
            default:
                fprintf(stderr, "unsupported media(%x)\n", mediaList[idx].mediaId);
        }
    }

    retval = thiz->add_media_session(thiz, media_session);
    if(retval < 0) {
        fprintf(stderr, "%s: add media session fail(%d) - release pre-allocate session memory \n",__func__,retval);
        media_session_release(media_session);
        return NULL;
    }

    return media_session;
}

static media_session_t* lookup_media_session(rtsp_server_t* thiz, const char* name) {
    int i = 0;
    rtsp_server_priv_t* priv = NULL;
    if (thiz == NULL) {
        return NULL;
    }

    priv = (rtsp_server_priv_t*)thiz->priv;
    for(i = 0; i < MAX_MEDIA_SESSION; i++ ) {
        media_session_t* media_session = priv->sessions[i];
        if(media_session != NULL && strcmp(name, media_session->get_stream_name(media_session)) == 0) {
            return priv->sessions[i];
        }
    }

    // if it is liveview, create
    if (strncmp(name, DEFAULT_VIEWFINDER_STREAM, strlen(DEFAULT_VIEWFINDER_STREAM)) == 0) {
        return init_liveview_session(thiz, name);
    }

    return NULL;
}

static int run(rtsp_server_t* thiz) {
    rtsp_server_priv_t* priv;
    if(thiz == NULL) {
        return -EINVAL;
    }
    priv = (rtsp_server_priv_t*)thiz->priv;
    ev_async_start(thiz->loop, &priv->exit_watcher);
    ev_async_start(thiz->loop, &priv->active_watcher);
    ev_io_start(thiz->loop, &priv->sock_watcher);
    ev_run(thiz->loop, 0);
    return 0;
}

static const char* get_rtsp_url_prefix(rtsp_server_t* thiz){
    rtsp_server_priv_t* priv;
    if(thiz == NULL) {
        return NULL;
    }
    priv = (rtsp_server_priv_t*)thiz->priv;
    if(priv->rtsp_url == NULL) {
        char ip[32] = {0};
        priv->rtsp_url = (char*)malloc(32);
        get_our_ip(ip);
        if(priv->port == 554) {
            sprintf(priv->rtsp_url, "rtsp://%s/", ip);
        } else {
            sprintf(priv->rtsp_url, "rtsp://%s:%hu/", ip, priv->port);
        }
    }
    return priv->rtsp_url;
}

static int get_rtsp_url(rtsp_server_t* thiz, media_session_t* media_session, char* buf, unsigned len){
    if (thiz == NULL || buf == NULL) {
        return -EINVAL;
    }
    if (media_session == NULL) {
        snprintf(buf, len, "%s%s", get_rtsp_url_prefix(thiz), DEFAULT_GSENSOR_STREAM);
    } else {
        snprintf(buf, len, "%s%s", get_rtsp_url_prefix(thiz), media_session->get_stream_name(media_session));
    }
    return 0;
}

static void rtsp_incoming_handler(struct ev_loop* loop, ev_io* w, int revent)
{
    rtsp_server_t* srv = (rtsp_server_t*)w->data;
    //rtsp_server_priv_t* priv = (rtsp_server_priv_t*)srv->priv;
    unsigned int session_id = 0;
    struct sockaddr_in client_addr;
    int client_addr_len = sizeof(client_addr);
    int client_sock = accept(w->fd, (struct sockaddr*)&client_addr, (socklen_t *)&client_addr_len);

    if(client_sock < 0) {
        fprintf(stderr, "accept() failed %s\n", strerror(errno));
        return;
    }

    if(srv->connection_count >= MAX_RTP_SESSION) {
        close(client_sock);
        return;
    }

    if(make_socket_nonblocking(client_sock)<0){
        fprintf(stderr, "make_socket_nonblocking() failed %s\n", strerror(errno));
    }
    increase_snd_buf(client_sock, 50*1024);
    do {
        session_id = our_random32();
    } while(session_id == 0);

    rtsp_client_session_create(srv, session_id, client_sock, client_addr);
    srv->connection_count++;
    return;
}

static unsigned int max_rtp_session(struct rtsp_server_s* thiz) {
    return MAX_RTP_SESSION;
}

rtsp_server_t* rtsp_server_create(unsigned short port_num, unsigned char en_ap) {
    rtsp_server_t* server = NULL;
    rtsp_server_priv_t* priv = NULL;
    int sock = setup_socket(port_num);
    ev_io *io_watcher = NULL;
    ev_async * async_watcher = NULL;

    if(sock < 0) {
        return NULL;
    }
    server = (rtsp_server_t*)malloc(sizeof(rtsp_server_t));
    if(server == NULL) {
        close(sock);
        return NULL;
    }
    memset(server, 0, sizeof(rtsp_server_t));
    priv = (rtsp_server_priv_t*)malloc(sizeof(rtsp_server_priv_t));
    if(priv == NULL) {
        close(sock);
        free(server);
        return NULL;
    }
    memset(priv, 0, sizeof(rtsp_server_priv_t));
    server->run = run;
    server->exit_loop = exit_loop;
    server->activate_loop = activate_loop;
    server->add_media_session = add_media_session;
    server->remove_media_session = remove_media_session;
    server->get_rtsp_url = get_rtsp_url;
    server->get_rtsp_url_prefix = get_rtsp_url_prefix;
    server->lookup_media_session = lookup_media_session;
    server->loop = ev_default_loop(0);   //XXX: change to epool
    server->max_rtp_session = max_rtp_session;
    // setup priv
    priv->sock = sock;
    priv->port = port_num;
    priv->en_ap = en_ap;
    priv->sock_watcher.data = server;
    io_watcher = &priv->sock_watcher;
    ev_io_init(io_watcher, rtsp_incoming_handler, sock, EV_READ);
    async_watcher = &priv->exit_watcher;
    ev_async_init(async_watcher, exit_loop_cb);
    async_watcher = &priv->active_watcher;
    ev_async_init(async_watcher, active_loop_cb);
    server->priv = priv;

    return server;
}

void rtsp_server_release(rtsp_server_t* server) {
    int i;
    rtsp_server_priv_t* priv = NULL;
    if(server == NULL) {
        return;
    }
    priv = (rtsp_server_priv_t*)server->priv;
    for(i = 0; i < MAX_MEDIA_SESSION && priv->sessions[i] !=NULL; i++ ) {
        media_session_release(priv->sessions[i]);
    }
    ev_async_stop(server->loop, &priv->exit_watcher);
    ev_async_stop(server->loop, &priv->active_watcher);
    ev_io_stop(server->loop, &priv->sock_watcher);
    ev_break(server->loop, EVBREAK_ALL);
    close(priv->sock);
    if(priv->rtsp_url) free(priv->rtsp_url);
    free(server->priv);
    free(server);
}

