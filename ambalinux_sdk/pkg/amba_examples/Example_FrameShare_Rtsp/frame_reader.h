#ifndef _FRAME_READER_
#define _FRAME_READER_

#ifdef __cplusplus
extern "C" {
#endif

#include <pthread.h>
#include "framed_buf.h"

typedef struct frame_info {
    struct frame_addr_info addr_info;
    unsigned int frame_num;
    unsigned long pts;
    unsigned int mark; // indicate the end of AU
}frame_info_t;

struct frame_reader;

typedef struct frame_reader {
    int (*start)(struct frame_reader* thiz);
    int (*stop)(struct frame_reader* thiz);
    int (*clr_buf)(struct frame_reader* thiz);
    int (*get_next_frame_meta)(struct frame_reader* thiz, struct frame_info* frame);
    int (*get_next_frame_meta_nonblock)(struct frame_reader* thiz, struct frame_info* frame);
    void* priv;
} frame_reader_t;

typedef struct frame_reader_priv {
    struct framed_buf* buf;     // buf to store the frame data and meta data
    unsigned int lastFrameNum;  // check frame sequence number
    unsigned int nalu_offset;   // the offset we preserve in memory for rtp and nalu header
    int notify_fd;              // whom we notify when there is frame come
    void* streamer;             // corresponding streamer in AmbaStream
    unsigned char waiting;
    unsigned char enable;
    unsigned char got_first_idr;
    unsigned char drop_frame;
    char notify_data_available;
    char notify_eos;
    char intra_refresh_mode;    //indicate whether the bitstream is Intra-refresh
    char padding_1;
    void (*frame_ready_cb)();
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} frame_reader_priv_t;

extern int frame_reader_init(
    struct frame_reader* thiz,
    int notify_fd,            // descriptor we used to notify ev_loop
    unsigned int nalu_offset, // offset we preserved in frame_buf
    unsigned int buf_size,    // frame buffer size
    unsigned int max_frame_buffed,  // max frame stored in frame buffer
    void* streamer,           // corresponding streamer in AmbaStream
    void* fr_cb               // frame ready callback
    );
extern void frame_reader_deinit(struct frame_reader* thiz);
extern void* frame_reader_get_streamer(struct frame_reader* thiz);
#ifdef __cplusplus
}
#endif

#endif

