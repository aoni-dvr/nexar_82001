#ifndef __GNSS_RECORD_H__
#define __GNSS_RECORD_H__

#define GNSS_RECORD_FILE_OPEN          (1U)
#define GNSS_RECORD_FILE_CLOSE         (2U)
#define GNSS_RECORD_FILE_SPLIT         (3U)
#define GNSS_RECORD_FILE_WRITE         (4U)

#define GNSS_FILE_STAT_OPEN       (0x00000001U)
#define GNSS_FILE_STAT_SPLIT      (0x00000002U)

typedef struct _gnss_record_s_ {
    char                filename[128U];
    void                *pFile;
    unsigned int        status;
    unsigned char       disable_record;
} gnss_record_s;

int gnss_record_control(unsigned int ctrl_type, const char *filename, void *param);
int gnss_record_task_start(void);
int gnss_record_task_stop(void);

#endif//__GNSS_RECORD_H__

