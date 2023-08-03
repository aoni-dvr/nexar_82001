#ifndef _RTSP_MEDIA_SESSION_H_
#define _RTSP_MEDIA_SESSION_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <arpa/inet.h>
struct media_subsession_s;

typedef struct media_session_s {
    const char* (*get_stream_name)(struct media_session_s* thiz);
    char* (*gen_sdp_description)(struct media_session_s* thiz, float duration);
    int (*add_media_subsession)(struct media_session_s* thiz, struct media_subsession_s* subsession);
    struct media_subsession_s* (*lookup_media_subsession_by_track_id)(struct media_session_s* thiz, const char* track_id);
    void* priv;
}media_session_t;

extern media_session_t* media_session_create(char const* stream_name, char const* info, char const* desc);
extern void media_session_release(media_session_t* media_session);

#ifdef __cplusplus
}
#endif

#endif

