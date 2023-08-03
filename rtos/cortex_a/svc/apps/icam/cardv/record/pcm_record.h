#ifndef __PCM_RECORD_H__
#define __PCM_RECORD_H__

int pcm_record_task_start(void);
int pcm_record_task_stop(void);
int pcm_record_set_path(const char *path);
int pcm_record_start(void);
int pcm_record_stop(void);
int pcm_record_is_busy(void);
int pcm_record_wait_finish(void);

#endif//__PCM_RECORD_H__

