#include <sys/time.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "media_session.h"
#include "media_subsession.h"
#include "socket_helper.h"
#include "ambastream.h"

typedef struct media_session_priv_s {
    media_subsession_t* subsessions[MAX_MEDIA_SUBSESSIONS];
    char* stream_name;
    char* info_sdp_string;
    char* desc_sdp_string;
    char* sdp_line;
    unsigned subsession_count;
    float duration;
    struct timeval creation_time;
}media_session_priv_t;

static char const* server_str = "Ambarella streaming";
static char const* server_ver = "2012.03.12";

static const char* get_stream_name(struct media_session_s* thiz){
    media_session_priv_t* priv = NULL;
    if(thiz == NULL) {
        fprintf(stderr, "get_stream_name: thiz == NULL\n");
        return NULL;
    }
    priv = (media_session_priv_t*)thiz->priv;

    return priv->stream_name;
}

static char* gen_sdp_description(struct media_session_s* thiz, float duration) {
    media_session_priv_t* priv = NULL;

    if(thiz == NULL) {
        return NULL;
    }
    priv = (media_session_priv_t*)thiz->priv;

    /* Due to the video resolution might changed time by time, */
    /* the SPD should be updated ervery time.                  */
    if(priv->sdp_line != NULL){
        free(priv->sdp_line);
        priv->sdp_line = NULL;
    }

    if(1){
        int i = 0;
        char ip[16];
        unsigned sdp_len = 0;
        media_subsession_t* subsession = NULL;
        char range_line[32];
        const char* source_filter_line = "";
        char const* const sdpPrefixFmt =
            "v=0\r\n"
            "o=- %ld%06ld %d IN IP4 %s\r\n"
            "s=%s\r\n"
            "i=%s\r\n"
            "t=0 0\r\n"
            "a=tool:%s %s\r\n"
            "a=type:broadcast\r\n"
            "a=control:*\r\n"
            "%s"
            "%s"
            "a=x-qt-text-nam:%s\r\n"
            "a=x-qt-text-inf:%s\r\n";

        get_our_ip(ip);

        if(duration == 0){
            sprintf(range_line, "a=range:npt=0-\r\n");
        }else{
            sprintf(range_line, "a=range:npt=0-%.3f\r\n", duration);
        }

        for(i = 0; i < MAX_MEDIA_SUBSESSIONS; i++) {
            subsession = priv->subsessions[i];
            if(subsession != NULL) {
                sdp_len += strlen(subsession->get_sdp_lines(subsession));
            }
        }


        sdp_len += strlen(sdpPrefixFmt)
            + 20 + 6 + 20 + strlen(ip)
            + strlen(priv->desc_sdp_string)
            + strlen(priv->info_sdp_string)
            + strlen(server_str) + strlen(server_ver)
            + strlen(source_filter_line)
            + strlen(range_line)
            + strlen(priv->desc_sdp_string)
            + strlen(priv->info_sdp_string);

        priv->sdp_line = (char*)malloc(sdp_len);
        if(priv->sdp_line == NULL)
            return NULL;

        // Generate the SDP prefix (session-level lines):
        sprintf(priv->sdp_line, sdpPrefixFmt,
            priv->creation_time.tv_sec, priv->creation_time.tv_usec, // o= <session id>
            1, // o= <version> // (needs to change if params are modified)
            ip, // o= <address>
            priv->desc_sdp_string, // s= <description>
            priv->info_sdp_string, // i= <info>
            server_str, server_ver, // a=tool:
            source_filter_line, // a=source-filter: incl (if a SSM session)
            range_line, // a=range: line
            priv->desc_sdp_string, // a=x-qt-text-nam: line
            priv->info_sdp_string // a=x-qt-text-inf: line
        );

        // Then, add the (media-level) lines for each subsession:
        char* media_sdp = priv->sdp_line;
        for(i = 0; i < MAX_MEDIA_SUBSESSIONS; i++) {
            subsession = priv->subsessions[i];
            if(subsession != NULL) {
                media_sdp += strlen(media_sdp);
                sprintf(media_sdp, "%s", subsession->get_sdp_lines(subsession));
            }
        }
    }
    return priv->sdp_line;
}

static int add_media_subsession(struct media_session_s* thiz, media_subsession_t* subsession) {
    int i = 0;
    media_session_priv_t* priv = NULL;
    if(thiz == NULL) {
        return -EINVAL;
    }

    priv = (media_session_priv_t*)thiz->priv;
    for(i = 0; i < MAX_MEDIA_SUBSESSIONS; i++ ) {
        if(priv->subsessions[i] == NULL) {
            //fprintf(stderr, "found slot %d for media subsessions(%p)\n",i,subsession);
            break;
        }
    }
    if(i >= MAX_MEDIA_SUBSESSIONS) {
        fprintf(stderr, "too many media subsessions\n");
        return -ENOSPC;
    }
    priv->subsessions[i] = subsession;
    subsession->parent = thiz;
    subsession->track_num = ++(priv->subsession_count);
    return 0;
}

static media_subsession_t* lookup_media_subsession_by_track_id(struct media_session_s* thiz, const char* track_id) {
    int i = 0;
    media_session_priv_t* priv = NULL;
    if(thiz == NULL) {
        return NULL;
    }
    priv = (media_session_priv_t*)thiz->priv;

    for(i = 0; i < MAX_MEDIA_SUBSESSIONS; i++) {
        media_subsession_t* subsession = priv->subsessions[i];
        if(subsession != NULL && strcmp(track_id, subsession->get_track_id(subsession)) == 0){
            return subsession;
        }
    }
    return 0;
}

media_session_t* media_session_create(char const* stream_name, char const* info, char const* desc) {
    if(stream_name == NULL) {
        fprintf(stderr,"media_session_create: stream_name is NULL\n");
        return NULL;
    }
    media_session_t* media_session = (media_session_t*)malloc(sizeof(media_session_t));
    if(media_session == NULL) {
        fprintf(stderr,"media_session_create: media_session is NULL\n");
        return NULL;
    }
    memset(media_session, 0, sizeof(media_session_t));

    media_session_priv_t* priv = (media_session_priv_t*)malloc(sizeof(media_session_priv_t));
    if(priv == NULL) {
        fprintf(stderr,"media_session_create: priv is NULL\n");
        free(media_session);
        return NULL;
    }
    memset(priv, 0, sizeof(media_session_priv_t));
    priv->stream_name = strdup(stream_name);
    priv->info_sdp_string = strdup(info == NULL ? server_str : info);
    priv->desc_sdp_string = strdup(desc == NULL ? server_str : desc);
    gettimeofday(&priv->creation_time, NULL);

    media_session->add_media_subsession = add_media_subsession;
    media_session->gen_sdp_description = gen_sdp_description;
    media_session->get_stream_name = get_stream_name;
    media_session->lookup_media_subsession_by_track_id = lookup_media_subsession_by_track_id;
    media_session->priv = priv;
    return media_session;
}

void media_session_release(media_session_t* media_session) {
    media_session_priv_t* priv;
    int i;
    if(media_session == NULL) {
        return;
    }
    priv = (media_session_priv_t*)media_session->priv;
    for(i = 0; i < MAX_MEDIA_SUBSESSIONS; i++ ) {
        if(priv->subsessions[i] != NULL && priv->subsessions[i]->release != NULL) {
            priv->subsessions[i]->release(priv->subsessions[i]);
        }
    }
    free(priv->sdp_line);
    free(priv->stream_name);
    free(priv->info_sdp_string);
    free(priv->desc_sdp_string);
    free(priv);
    free(media_session);
}

