#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include "framed_buf.h"
#include "ambastream.h"

//#define DEBUG_BUF
//#define DBG_WRITE2
#define is_power_of_2(x)    ((x) != 0 && (((x) & ((x) - 1)) == 0))

#define min(x,y) ({ \
    typeof(x) _x = (x);     \
    typeof(y) _y = (y);     \
    (void) (&_x == &_y);    \
    _x < _y ? _x : _y;})

typedef struct frame_meta_s {
    unsigned int start;         // the index, not the actual address
    unsigned int offset;
    unsigned int total_bytes;
    unsigned int bytes_remain;
}frame_meta_t;

typedef struct frame_info_queue_s {
    unsigned int out;
    unsigned int in;
    unsigned int mask;
    unsigned int meta_size;
    unsigned int max_element;
    unsigned int element_size;
    unsigned char *data;
}frame_info_queue_t;

#if 0
typedef struct video_buf_priv_s {
    unsigned int size;
    unsigned int mask;
    unsigned char* data;
    unsigned int max_frame;
    unsigned int out;
    unsigned int in;
    frame_info_queue_t *frame_info_list;
    void* cur_frame_extra_info;
    unsigned int extra_info_size;
    frame_meta_t cur_frame_item;
    unsigned char first_read;
}video_buf_priv_t;
#endif

typedef struct framed_buf {
    unsigned int size;
    unsigned int mask;
    unsigned char* data;
    unsigned int max_frame;
    unsigned int out;
    unsigned int in;
    frame_info_queue_t *frame_info_list;
    void* cur_frame_extra_info;
    unsigned int extra_info_size;
    frame_meta_t cur_frame_item;
    unsigned char first_read;
    unsigned int fullness;
}framed_buf;

static inline unsigned video_buf_unused(struct framed_buf* thiz);

static inline unsigned round_down_power_of_2(int x) {
    x = x| (x >> 1);
    x = x| (x >> 2);
    x = x| (x >> 4);
    x = x| (x >> 8);
    x = x| (x >> 16);
    return x - (x>>1);
}

static inline unsigned round_up_power_of_2(unsigned x) {
    x = x - 1;
    x = x | (x >> 1);
    x = x | (x >> 2);
    x = x | (x >> 4);
    x = x | (x >> 8);
    x = x | (x >>16);
    return x + 1;
}

#ifdef DEBUG_BUF
static void dump_mem(unsigned char* addr, unsigned len) {
    int i = 0;
    int row = 0;
    int remain = 0;
    int mod = 16;
    if(addr == NULL || len == 0) {
        return;
    }
    row = len/mod;
    remain = len%mod;
    for(i = 0; i < row; i++) {
        int j = 0;
        for(j = 0; j < mod; j++) {
            printf("%02x ", addr[i*mod + j]);
        }
        printf("\n");
    }

    if(row > 0 && remain > 0) {
        for(i = 0; i < remain; i++) {
            printf("%02x ", addr[row*mod + i]);
        }
        printf("\n");
    }
}
#endif

static frame_info_queue_t* frame_info_queue_create(unsigned int element_size, unsigned int max_element) {
    frame_info_queue_t* buf;
    unsigned int size ;
    if( element_size == 0 || max_element == 0) {
        return NULL;
    }

    if(!is_power_of_2(max_element)) {
        max_element = round_up_power_of_2(max_element);
    }
    buf = (frame_info_queue_t*)malloc(sizeof(frame_info_queue_t));
    if (buf == NULL ) {
        return NULL;
    }

    memset(buf, 0, sizeof(frame_info_queue_t));
    buf->max_element = max_element;
    buf->mask = max_element - 1;
    buf->meta_size = sizeof(frame_meta_t);
    buf->element_size = element_size;
    size = (buf->meta_size + element_size)* max_element;
    buf->data = (unsigned char*)malloc(size);
    if(buf->data == NULL) {
        free(buf);
        return NULL;
    }

    memset(buf->data, 0, size);
    buf->in = buf->out = 0;
    return buf;
}

static void frame_info_queue_release(frame_info_queue_t* thiz) {
    //if (thiz == NULL) return;
    free(thiz->data);
    free(thiz);
}

static inline void frame_info_queue_reset(frame_info_queue_t* thiz) {
    //if (thiz == NULL) return;
    thiz->in = thiz->out = 0;
}

static inline unsigned frame_info_queue_used(frame_info_queue_t* thiz) {
    //if(thiz == NULL) return 0;
    return thiz->in - thiz->out;
}

static inline unsigned frame_info_queue_unused(frame_info_queue_t* thiz) {
    //if(thiz == NULL) return 0;
    return thiz->max_element - (thiz->in - thiz->out);
}

static inline int frame_info_queue_append(frame_info_queue_t* thiz, frame_meta_t* meta, void* element) {
    unsigned off;
    if (thiz == NULL) {
        return -EINVAL;
    }
    if (frame_info_queue_unused(thiz) <= 0) {
        fprintf(stderr, "no space to store frame info\n");
        return -ENOMEM;
    }
    off = (thiz->in & thiz->mask) * ( thiz->meta_size + thiz->element_size);
    memcpy(thiz->data + off, meta, thiz->meta_size);
    memcpy(thiz->data + off + thiz->meta_size, element, thiz->element_size);
    thiz->in += 1;

    return 0;
}

static inline int frame_info_queue_fetch(frame_info_queue_t* thiz, frame_meta_t *meta, void* element_copy_out) {
    unsigned off;
    if (thiz == NULL) {
        return -EINVAL;
    }
    if ( thiz->in == thiz->out ) {
       // not perfect, may introdue one frame delay, but not introduce more lock
        return -EAGAIN;
    }

    off = (thiz->out & thiz->mask) * ( thiz->meta_size + thiz->element_size);
    memcpy(meta, thiz->data + off, thiz->meta_size);
    memcpy(element_copy_out, thiz->data + off + thiz->meta_size, thiz->element_size);
    thiz->out += 1;
    return 0;
}

/*
 * Do not increase read pointer, only retrieve cur frame info
 */
static inline int frame_info_queue_peek(frame_info_queue_t* thiz, frame_meta_t *meta, void* element_copy_out) {
    unsigned off;
    if (thiz == NULL) {
        return -EINVAL;
    }
    if ( thiz->in == thiz->out ) {
       // not perfect, may introdue one frame delay, but not introduce more lock
        return -EAGAIN;
    }

    off = (thiz->out & thiz->mask) * ( thiz->meta_size + thiz->element_size);
    memcpy(meta, thiz->data + off, thiz->meta_size);
    memcpy(element_copy_out, thiz->data + off + thiz->meta_size, thiz->element_size);
    return 0;
}

int framed_buf_peek_frame(struct framed_buf* thiz) {
    if (thiz == NULL) {
        return -EINVAL;
    }

    if(thiz->first_read) {
        thiz->first_read = 0;
    }
    if( frame_info_queue_peek(thiz->frame_info_list, &thiz->cur_frame_item, thiz->cur_frame_extra_info)!= 0 ) {
        return -EAGAIN;
    }
    thiz->out = thiz->cur_frame_item.start;
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////

struct framed_buf* framed_buf_create(unsigned int buffer_size, unsigned int extra_info_size, unsigned int max_frame) {
    struct framed_buf* thiz;
    if(buffer_size <= 0 ) {
        fprintf(stderr, "framed_buf_create: buffer_size<=0\n");
        return NULL;
    }
    thiz = (struct framed_buf*)malloc(sizeof(struct framed_buf));
    if( thiz == NULL ) {
        fprintf(stderr, "framed_buf_create: thiz is NULL\n");
        return NULL;
    }
    memset(thiz, 0, sizeof(struct framed_buf));

    if(!is_power_of_2(buffer_size)) {
        buffer_size = round_up_power_of_2(buffer_size);
    }
    thiz->size = buffer_size;
    thiz->mask = thiz->size - 1;
    thiz->data = (unsigned char*)malloc(thiz->size);
    if( thiz->data == NULL) {
        fprintf(stderr, "framed_buf_create: thiz->data is NULL\n");
        free(thiz);
        return NULL;
    }
    memset(thiz->data, 0, thiz->size);
    thiz->extra_info_size = extra_info_size;
    thiz->cur_frame_extra_info = (void*)malloc(thiz->extra_info_size);
    if(thiz->cur_frame_extra_info == NULL) {
        fprintf(stderr, "framed_buf_create: thiz->cur_frame_extra_info is NULL\n");
        free(thiz->data);
        free(thiz);
        return NULL;
    }
    memset(thiz->cur_frame_extra_info, 0, sizeof(thiz->extra_info_size));
    thiz->frame_info_list = frame_info_queue_create(extra_info_size, max_frame);
    if(thiz->frame_info_list == NULL) {
        fprintf(stderr, "framed_buf_create: thiz->frame_info_list is NULL\n");
        free(thiz->cur_frame_extra_info);
        free(thiz->data);
        free(thiz);
        return NULL;
    }
    thiz->out = thiz->in = 0;
    thiz->first_read = 1;
    thiz->fullness = 0;
    return thiz;
}

void framed_buf_release(struct framed_buf* thiz) {
    if ( thiz == NULL) {
        return;
    }

    frame_info_queue_release(thiz->frame_info_list);
    free(thiz->cur_frame_extra_info);
    free(thiz->data);
    free(thiz);
}

void framed_buf_reset(struct framed_buf* thiz) {
    if (thiz == NULL) {
        return;
    }

    thiz->in = thiz->out = 0;
    thiz->first_read = 0;
    thiz->fullness = 0;
    memset(thiz->cur_frame_extra_info, 0, thiz->extra_info_size);
    memset(&(thiz->cur_frame_item), 0, sizeof(thiz->cur_frame_item));
    frame_info_queue_reset(thiz->frame_info_list);
}

void framed_buf_check_fullness(struct framed_buf* thiz) {
    if ((thiz->fullness == 0)
        && ((video_buf_unused(thiz) < thiz->size / 10 * 4)
            || (frame_info_queue_unused(thiz->frame_info_list) < thiz->frame_info_list->max_element / 10 * 4))) {
        AmbaStream_send_PbCmd(AMBASTREAM_PB_PAUSE, 0, NULL, 0, 0);
        thiz->fullness = 1;
    } else if ((thiz->fullness == 1)
                && ((video_buf_unused(thiz) > thiz->size / 10 * 7)
                &&(frame_info_queue_unused(thiz->frame_info_list) > thiz->frame_info_list->max_element / 10 * 7))) {
        AmbaStream_send_PbCmd(AMBASTREAM_PB_RESUME, 0, NULL, 0, 0);
        thiz->fullness = 0;
    }
}

unsigned int framed_buf_get_buffer_size(struct framed_buf* thiz) {
    if(thiz == NULL){
        return 0;
    }
    return thiz->size;
}

int framed_buf_next_frame(struct framed_buf* thiz) {
    if (thiz == NULL) {
        return -EINVAL;
    }

    if(thiz->first_read) {
        thiz->first_read = 0;
    }
    if( frame_info_queue_fetch(thiz->frame_info_list, &thiz->cur_frame_item, thiz->cur_frame_extra_info)!= 0 ) {
        return -EAGAIN;
    }
    thiz->out = thiz->cur_frame_item.start;
    return 0;
}

inline unsigned int framed_buf_get_cur_frame_remain_bytes(struct framed_buf* thiz) {
    if (thiz == NULL) {
        return -EINVAL;
    }

    return thiz->cur_frame_item.bytes_remain;
}

inline unsigned int framed_buf_get_cur_frame_total_bytes(struct framed_buf* thiz) {
    if (thiz == NULL) {
        return -EINVAL;
    }
    return thiz->cur_frame_item.total_bytes;
}

inline int framed_buf_get_cur_frame_addr_info(struct framed_buf* thiz, struct frame_addr_info* info) {
    if(thiz == NULL || info == NULL) return -EINVAL;

    info->base = &thiz->data[thiz->cur_frame_item.start & thiz->mask];
    info->len = thiz->cur_frame_item.total_bytes;
    info->offset = thiz->cur_frame_item.offset;
    return 0;
}

const void* framed_buf_get_cur_frame_extra_info(struct framed_buf* thiz) {
    if(thiz == NULL) {
        return NULL;
    }
    return thiz->cur_frame_extra_info;
}

static inline unsigned video_buf_unused(struct framed_buf* thiz) {
    return thiz->size - (thiz->in - thiz->out);
}

int framed_buf_extract_data_in_frame2(struct framed_buf* thiz,
                                        unsigned char *to,
                                        unsigned len,
                                        unsigned int* remain,
                                        unsigned char forward_pointer)
{
    unsigned int off = 0;
    if(thiz == NULL || to == NULL) {
        return -EINVAL;
    }

    if(len > thiz->cur_frame_item.bytes_remain) {
        len = thiz->cur_frame_item.bytes_remain;
    }
    off = thiz->out & thiz->mask;

    memcpy(to, thiz->data + off + thiz->cur_frame_item.offset, len);
#ifdef DEBUG_BUF
    dump_mem(to, len > 32 ? 32 : len);
#endif
    thiz->cur_frame_item.bytes_remain -= len;
    if(remain != NULL) {
        *remain = thiz->cur_frame_item.bytes_remain;
    }
    if(forward_pointer != 0) {
        thiz->out += len;
    }
    return len;
}

int framed_buf_write_one_frame2(struct framed_buf* thiz,
                                    unsigned offset,
                                    write_vec_t *write_vecs,
                                    unsigned int num_of_write_vec,
                                    void* extra_info) {
    if(thiz == NULL || write_vecs == NULL || num_of_write_vec == 0 || extra_info == NULL){
        return -EINVAL;
    }
    unsigned int i, total_length = 0;
    unsigned int in_off, out_off;

    for(i = 0; i < num_of_write_vec; i++) {
        total_length += write_vecs[i].size;
    }
    total_length += offset;
    in_off = thiz->in & thiz->mask;
    out_off = thiz->out & thiz->mask;

    if( video_buf_unused(thiz) < total_length ) {
        fprintf(stderr, "unused(%d) < total_length (%d)\n", video_buf_unused(thiz), total_length);
        return -ENOMEM;
    }
    // we have enough space, check if it's split
    if(thiz->size - in_off >= total_length ) {
#ifdef DBG_WRITE2
        fprintf(stderr, "in_off 0x%x, input_len 0x%x\n", in_off, total_length);
#endif
        // normal case
        //thiz->in += offset;
    } else if (out_off >= total_length) {
        // enough space at the start point to 'read pointer'
#ifdef DBG_WRITE2
        fprintf(stderr, "out_off 0x%x, input_len 0x%x\n", out_off, total_length);
#endif
        thiz->in += (thiz->size - in_off);
    } else {
        // total size is enough, but they are split to small pieces
#ifdef DBG_WRITE2
        fprintf(stderr, "total size is enough, but they are split to small pieces");
#endif
        return -ENOMEM;
    }

    do {
        unsigned int   len;
        unsigned char* addr;
        unsigned int   off;
        frame_meta_t   frame_meta;

        frame_meta.start        = thiz->in;
        frame_meta.offset       = offset;
        frame_meta.bytes_remain = frame_meta.total_bytes = total_length - offset;

        thiz->in += offset;     // the offset is for rtp, fua

        for( i = 0; i < num_of_write_vec; i++ ) {
            len  = write_vecs[i].size;
            addr = write_vecs[i].addr;
            off  = (thiz->in) & thiz->mask;

            memcpy(thiz->data + off, addr, len);

            thiz->in += len;
        }

        if (frame_info_queue_append(thiz->frame_info_list, &frame_meta, extra_info) < 0) {
            fprintf(stderr, "frame_info_queue_append fail\n");
            return -ENOMEM;
        }
    } while(0);

    return total_length;
}

int framed_buf_write_one_frame(struct framed_buf* thiz,
                                    write_vec_t *write_vecs,
                                    unsigned int num_of_write_vec,
                                    void* extra_info) {
    unsigned int i, total_length = 0;

    if(thiz == NULL || write_vecs == NULL || num_of_write_vec == 0 || extra_info == NULL){
        return -EINVAL;
    }

    for(i = 0; i < num_of_write_vec; i++) {
        total_length += write_vecs[i].size;
    }
    if( video_buf_unused(thiz) < total_length ) {
        return -ENOMEM;
    } else {
        frame_meta_t frame_meta;
        frame_meta.start = thiz->in;
        frame_meta.total_bytes = total_length;
        frame_meta.bytes_remain = total_length;
        for( i = 0; i < num_of_write_vec; i++ ) {
            unsigned int len = write_vecs[i].size;
            unsigned char* addr = write_vecs[i].addr;
            unsigned int l = 0;
            unsigned off;
            off = thiz->in & thiz->mask;
            // check if fragment
            l = min(len, thiz->size - off);
            memcpy(thiz->data + off, addr, l);
            #ifdef DEBUG_BUF
            printf("vec[%d] addr 0x%p, len 0x%x\n", i, addr, len);
            dump_mem(thiz->data + off, l > 32 ? 32 : l);
            #endif
            memcpy(thiz->data, addr + l, len - l);
            thiz->in += len;
        }
        if (frame_info_queue_append(thiz->frame_info_list, &frame_meta, extra_info) < 0) {
            return -ENOMEM;
        }
    }
    return total_length;
}

int framed_buf_extract_data_in_frame(struct framed_buf* thiz,
                                        unsigned char *to,
                                        unsigned len,
                                        unsigned int* remain,
                                        unsigned char forward_pointer) {
    unsigned int l = 0;
    unsigned int off = 0;
    if(thiz == NULL || to == NULL) {
        return -EINVAL;
    }

    if(len > thiz->cur_frame_item.bytes_remain) {
        len = thiz->cur_frame_item.bytes_remain;
    }
    off = thiz->out & thiz->mask;
    l = min(len, thiz->size - off);
    memcpy(to, thiz->data + off, l);
    memcpy(to + l, thiz->data, len - l);
    #ifdef DEBUG_BUF
    dump_mem(to, len > 32 ? 32 : len);
    #endif
    thiz->cur_frame_item.bytes_remain -= len;
    if(remain != NULL) {
        *remain = thiz->cur_frame_item.bytes_remain;
    }
    if(forward_pointer != 0) {
        thiz->out += len;
    }
    return len;
}

int framed_buf_skip_bytes_in_frame(struct framed_buf* thiz, unsigned len, unsigned int* remain) {
    if(thiz == NULL) {
        return -EINVAL;
    }

    if(len > thiz->cur_frame_item.bytes_remain) {
        len = thiz->cur_frame_item.bytes_remain;
    }
    thiz->cur_frame_item.bytes_remain -= len;
    if(remain != NULL) {
        *remain = thiz->cur_frame_item.bytes_remain;
    }
    thiz->out += len;
    return len;
}

inline unsigned framed_buf_valid_frames(struct framed_buf *thiz) {
    if(thiz == NULL) {
        return 0;
    }
    return frame_info_queue_used(thiz->frame_info_list);
}

///////////////////////////////////////////////
unsigned framed_buf_valid_bytes(struct framed_buf *thiz) {
    if(thiz == NULL) {
        return 0;
    }

    return thiz->in - thiz->out;
}

// this is for stream base read, not used now.
#if 0
int framed_buf_get_bytes(struct framed_buf *thiz, unsigned char* to, unsigned int len, unsigned char forward_pointer) {
    unsigned int l = 0;
    unsigned int off = 0;
    if(thiz == NULL) {
        return -EINVAL;
    }

    if(len > framed_buf_valid_bytes(thiz)) {
        return -EAGAIN;
    }
    off = thiz->out & thiz->mask;
    l = min(len, thiz->size - off);
    memcpy(to, thiz->data + off, l);
    memcpy(to + l, thiz->data, len - l);
    #ifdef DEBUG_BUF
    dump_mem(to, len > 32 ? 32 : len);
    #endif
    if(forward_pointer != 0) {
        thiz->out += len;
        unsigned tmp = len;
        while(tmp > thiz->cur_frame_item.bytes_remain ) {
            tmp -= thiz->cur_frame_item.bytes_remain;
            if(frame_info_queue_fetch(thiz->frame_info_list, &thiz->cur_frame_item, thiz->cur_frame_extra_info) != 0)
                break;
        }
        if(thiz->cur_frame_item.bytes_remain >= tmp) {
           thiz->cur_frame_item.bytes_remain -= tmp;
        }
    }
    return len;
}

int framed_buf_skip_bytes(struct framed_buf *thiz, unsigned int num) {
    unsigned int tmp_num;
    if(thiz == NULL) {
        return -EINVAL;
    }

    tmp_num = framed_buf_valid_bytes(thiz);
    if (tmp_num < 0) {
        return -EINVAL;
    }
    num = (num > tmp_num) ? tmp_num : num;
    thiz->out += num;
    return num;
}
#endif


#if 0
typedef struct wow_s {
    int i;
    int j;
}wow_t;
void test() {    int i;
    frame_meta_t meta;
    wow_t item;
    item.i = 0xffeeaa00;
    item.j = 0xccddbb00;
    meta.start = 0x22334400;
    meta.bytes_remain = 0x55667700;
    frame_info_queue_t* buf = frame_info_queue_create(sizeof(wow_t), 16);
    for( i = 0;
 i < 19;
 i ++) {        item.i += 1;
 item.j += 1;
        meta.start += 1;
 meta.bytes_remain += 1;
        frame_info_queue_append(buf, &meta, &item);
    }    for( i = 0;
 i < 19;
 i ++) {        int ret =frame_info_queue_fetch(buf, &meta, &item);
        if(ret < 0) {            printf("empty\n");
        } else {            printf("wow %x %x\n", meta.start, item.i);
        }    }    frame_info_queue_release(buf);
}
#endif
