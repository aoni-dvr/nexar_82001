#ifndef FRAMED_BUF_H
#define FRAMED_BUF_H
#ifdef __cplusplus
extern "C" {
#endif

struct framed_buf;

typedef struct frame_addr_info {
    unsigned char* base;
    unsigned int len;
    unsigned int offset;
}frame_addr_info_t;

typedef struct write_vec_s {
    unsigned char *addr;
    unsigned int size;
}write_vec_t;

extern struct framed_buf* framed_buf_create(unsigned int buffer_size,
                                        unsigned int extra_info_size,
                                        unsigned int max_frame);
extern void framed_buf_release(struct framed_buf* thiz);
extern unsigned int framed_buf_get_buffer_size(struct framed_buf* thiz);
/**
 * Write one frame to video buffer.
 *
 */
extern int framed_buf_write_one_frame(struct framed_buf* thiz,
                                            write_vec_t *write_vecs,
                                            unsigned int num_of_write_vec,
                                            void* extrac_info);

extern int framed_buf_write_one_frame2(struct framed_buf* thiz,
                                    unsigned offset,
                                    write_vec_t *write_vecs,
                                    unsigned int num_of_write_vec,
                                    void* extra_info);
/**
 * get total valid frames in buf
 */
/* extern inline */unsigned framed_buf_valid_frames(struct framed_buf *thiz);
/**
 * Move the read pointer to next frame.
 *
 */
extern int framed_buf_next_frame(struct framed_buf* thiz);
/**
 * Peek current frame, do not move read pointer
 *
 */
extern int framed_buf_peek_frame(struct framed_buf* thiz);
/**
 * Read info of cur frame.
 *
 */

/*extern inline */int framed_buf_get_cur_frame_addr_info(struct framed_buf* thiz, struct frame_addr_info* info);
extern const void* framed_buf_get_cur_frame_extra_info(struct framed_buf* thiz);
/*extern inline */unsigned int framed_buf_get_cur_frame_remain_bytes(struct framed_buf* thiz);
/*extern inline */unsigned int framed_buf_get_cur_frame_total_bytes(struct framed_buf* thiz);
/**
 * Copy out the frame data of current frame.
 */
extern int framed_buf_extract_data_in_frame(struct framed_buf* thiz,
                                                    unsigned char *to,
                                                    unsigned num,
                                                    unsigned int* remain,
                                                    unsigned char forward_poiter);

extern int framed_buf_extract_data_in_frame2(struct framed_buf* thiz,
                                                    unsigned char *to,
                                                    unsigned len,
                                                    unsigned int* remain,
                                                    unsigned char forward_pointer);
extern void framed_buf_reset(struct framed_buf* thiz);
extern void framed_buf_check_fullness(struct framed_buf* thiz);
extern int framed_buf_skip_bytes_in_frame(struct framed_buf* thiz, unsigned num, unsigned int* remain);
// interface for bytes acess, work on the whole buffer, but not on frame unit
extern unsigned framed_buf_valid_bytes(struct framed_buf *thiz);
#ifdef __cplusplus
}
#endif
#endif // FRAMED_BUF_H
