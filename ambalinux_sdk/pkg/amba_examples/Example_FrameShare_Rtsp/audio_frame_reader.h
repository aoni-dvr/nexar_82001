#ifndef _AUDIO_FRAME_READER_
#define _AUDIO_FRAME_READER_

#ifdef __cplusplus
extern "C" {
#endif

#include "frame_reader.h"

struct audio_frame_reader;

typedef struct audio_frame_reader {
    frame_reader_t  super;    // inherit
    int (*get_audio_config)(struct audio_frame_reader* thiz,
        int *format, int* samplerate, int* channel);
    int (*peek_next_frame_meta)(struct audio_frame_reader *, struct frame_info *);
} audio_frame_reader_t;

extern struct audio_frame_reader* audio_frame_reader_create(
    int notify_fd,            // descriptor we used to notify ev_loop
    unsigned int nalu_offset, // offset we preserved in frame_buf
    void* streamer            // corresponding streamer in AmbaStream
    );
extern void audio_frame_reader_release(struct audio_frame_reader* thiz);

#ifdef __cplusplus
}
#endif

#endif

