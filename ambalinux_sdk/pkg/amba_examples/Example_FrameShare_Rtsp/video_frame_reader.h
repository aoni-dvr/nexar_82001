#ifndef _VIDEO_FRAME_READER_
#define _VIDEO_FRAME_READER_

#ifdef __cplusplus
extern "C" {
#endif

#include "frame_reader.h"

typedef struct video_frame_reader {
    frame_reader_t super;
    int (*get_sps_pps)(struct video_frame_reader* thiz, unsigned char* sps, unsigned int* sps_len,
        unsigned char* pps, unsigned int* pps_len, unsigned int* profile_id);
    int (*get_vps_sps_pps)(struct video_frame_reader* thiz, unsigned char* vps, unsigned int* vps_len,
        unsigned char* sps, unsigned int* sps_len, unsigned char* pps, unsigned int* pps_len);
    int (*get_gop_info)(struct video_frame_reader* thiz, unsigned short* M, unsigned short* N,
        unsigned short* gopSize, unsigned short* IRCycle, unsigned short* recovery_frame_cnt);
} video_frame_reader_t;

extern video_frame_reader_t* video_frame_reader_create(
    int notify_fd,            // descriptor we used to notify ev_loop
    unsigned int nalu_offset, // offset we preserved in frame_buf
    void* streamer            // corresponding streamer in AmbaStream
    );
extern void video_frame_reader_release(video_frame_reader_t* thiz);

#ifdef __cplusplus
}
#endif

#endif

