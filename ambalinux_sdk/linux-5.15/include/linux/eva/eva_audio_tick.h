/*
 *
 *  audio tick
 *
 *  Copyright (C) ARM Limited
 *  Copyright (C) 2000 Deep Blue Solutions Ltd.
 */
#ifndef AUDIO_TICK_H
#define AUDIO_TICK_H

#include <linux/types.h>


#define eva_audio_tick_recoder_len_dft (30 * 5) 	// vsync_rate_30 * 5s
#define eva_audio_tick_recoder_len_max (30 * 10)	// vsync_rate_30 * 10s
#define eva_audio_tick_recoder_len_min (1)

typedef enum {
	EVA_AUDIOTICK_TRIG_START,
	EVA_AUDIOTICK_TRIG_STOP,
} eva_audio_tick_trig_type_e;

typedef enum {
	EVA_AUDIOTICK_QUERY_NOT_RDY,
	EVA_AUDIOTICK_QUERY_RDY,
} eva_audio_tick_query_ready_e;

typedef struct {
	/* output */
	uint64_t tick;
	uint64_t tick_us;
} eva_audio_tick_cur_s;

typedef struct {
	/* input */
	uint32_t cap_seq_no_user;

	/* output */
	uint32_t cap_seq_no_real; 	// <= cap_seq_no_user
	uint64_t tick_real;	
	uint64_t tick_us_real;
} eva_audio_tick_query_with_capseqno_s;

typedef struct {
	/* input */
	uint32_t tick_user;

	/* output */
	uint32_t cap_seq_no_real;
	uint64_t tick_real;	// <= tick_user
	uint64_t tick_us_real;
} eva_audio_tick_query_with_tick_s;

typedef struct {
	/* input */
	uint32_t len;		 									// number of cap_seq_no & tick & tick_us

	/* output */
	uint32_t cap_seq_no[eva_audio_tick_recoder_len_max]; 	// valid count : len
	uint64_t tick[eva_audio_tick_recoder_len_max]; 		 	// valid count : len
	uint64_t tick_us[eva_audio_tick_recoder_len_max]; 		// valid count : len
} eva_audio_tick_query_lastest_s;

typedef struct {
	/* input */
	uint32_t len;		 									// number of cap_seq_no & tick & tick_us

	/* output */
	uint32_t cap_seq_no[eva_audio_tick_recoder_len_max]; 	// valid count : len
	uint64_t tick[eva_audio_tick_recoder_len_max]; 		 	// valid count : len
	uint64_t tick_us[eva_audio_tick_recoder_len_max]; 		// valid count : len
} eva_audio_tick_query_oldest_s;


/*
	set
*/
#define EVA_AUDIOTICK_IOCTL_SET_TRIG		_IOWR('a', 0, eva_audio_tick_trig_type_e)	// trigger start/stop record
#define EVA_AUDIOTICK_IOCTL_SET_REC_LEN 	_IOWR('a', 1, uint32_t) 					// set record len, valid when EVA_AUDIOTICK_TRIG_STOP


/*
	get
*/
#define EVA_AUDIOTICK_IOCTL_GET_TRIG			_IOWR('a', 100, eva_audio_tick_trig_type_e)		// get start/stop record
#define EVA_AUDIOTICK_IOCTL_GET_REC_LEN 		_IOWR('a', 101, uint32_t)						// get record len
#define EVA_AUDIOTICK_IOCTL_GET_CUR_TICK 		_IOWR('a', 102, eva_audio_tick_cur_s)			// get current tick, unnecessary EVA_AUDIOTICK_TRIG_START
#define EVA_AUDIOTICK_IOCTL_GET_QUERY_RDY		_IOWR('a', 103, eva_audio_tick_query_ready_e)	// check query ready
#define EVA_AUDIOTICK_IOCTL_GET_CUR_CAPSEQNO	_IOWR('a', 104, uint32_t)						// get current cap_seq_no, unnecessary EVA_AUDIOTICK_TRIG_START


/*
	query info
	- valid when EVA_AUDIOTICK_TRIG_START
	- blocking when EVA_AUDIOTICK_QUERY_NOT_RDY
*/
#define EVA_AUDIOTICK_IOCTL_QUERY_WITH_CAPSEQNO _IOWR('a', 201, eva_audio_tick_query_with_capseqno_s)
#define EVA_AUDIOTICK_IOCTL_QUERY_WITH_TICK 	_IOWR('a', 202, eva_audio_tick_query_with_tick_s)
#define EVA_AUDIOTICK_IOCTL_QUERY_LASTEST 		_IOWR('a', 203, eva_audio_tick_query_lastest_s)
#define EVA_AUDIOTICK_IOCTL_QUERY_OLDEST 		_IOWR('a', 204, eva_audio_tick_query_oldest_s)

int audio_tick_query_lastest(eva_audio_tick_query_lastest_s *in);
int audio_tick_query_oldest(eva_audio_tick_query_oldest_s *in);
int audio_tick_get_cur_tick(eva_audio_tick_cur_s *out);
int audio_tick_get_cur_capseqno(uint32_t *out);

#endif
