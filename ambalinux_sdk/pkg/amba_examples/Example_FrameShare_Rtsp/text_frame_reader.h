#ifndef _TEXT_FRAME_READER_
#define _TEXT_FRAME_READER_

#ifdef __cplusplus
extern "C" {
#endif

#include "frame_reader.h"

extern struct frame_reader* text_frame_reader_create(
    int notify_fd,            // descriptor we used to notify ev_loop
    unsigned int nalu_offset, // offset we preserved in frame_buf
    void* streamer            // corresponding streamer in AmbaStream
    );
extern void text_frame_reader_release(struct frame_reader* thiz);

#ifdef __cplusplus
}
#endif

#endif

