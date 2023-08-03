#ifndef _RTSP_SERVER_H_
#define _RTSP_SERVER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ev.h"
#include "media_session.h"

#define MAX_MEDIA_SESSION       4
#define MAX_RTP_SESSION         2   // THE NUMBER OF THE CLIENT
#define DEFAULT_VIEWFINDER_STREAM   "live"
#define DEFAULT_VIEWFINDER_STREAM_INTERNAL "live/video0"
#define DEFAULT_VIEWFINDER_STREAM_EXTERNAL "live/video1"
#define DEFAULT_GSENSOR_STREAM      "gsensor"

typedef struct rtsp_server_s {
	int (*add_media_session)(struct rtsp_server_s* thiz, media_session_t* media_session);
	int (*remove_media_session) (struct rtsp_server_s* thiz, media_session_t* media_session);
	int (*get_rtsp_url)(struct rtsp_server_s* thiz, media_session_t* media_session, char* buf, unsigned len);
	const char* (*get_rtsp_url_prefix)(struct rtsp_server_s* thiz);
	int (*run)(struct rtsp_server_s* thiz);
	void (*exit_loop)(struct rtsp_server_s* thiz);
	media_session_t* (*lookup_media_session)(struct rtsp_server_s* thiz, const char* name);
	unsigned int (*max_rtp_session)(struct rtsp_server_s* thiz);
	struct ev_loop* loop;
	unsigned int connection_count;
	void (*activate_loop)(struct rtsp_server_s* thiz);	//used for async activate ev_loop
	void* priv;
	int (*get_systime)(double *ref_time);
	int (*change_brate)(int bitrate);
	int (*change_net_bandwidth)(void* streamer, int bandwidth);
	int (*get_live_brate)(void);
	int (*get_live_avg_brate)(void);
	int (*send_rr_stat)(void* streamer, unsigned int fr_lost, unsigned jitter, double pg_delay);
}rtsp_server_t;

rtsp_server_t* rtsp_server_create(unsigned short, unsigned char);
void rtsp_server_release(rtsp_server_t* server);

#ifdef __cplusplus
}
#endif

#endif
