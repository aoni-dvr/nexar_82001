#ifndef __AAC_RECORD_H__
#define __AAC_RECORD_H__

int aac_record_task_start(void);
int aac_record_task_stop(void);
int aac_record_set_path(const char *path);
int aac_record_start(void);
int aac_record_stop(void);
int aac_record_is_busy(void);

#endif//__AAC_RECORD_H__

