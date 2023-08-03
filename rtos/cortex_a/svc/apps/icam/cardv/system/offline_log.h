#ifndef __OFFLINE_LOG_H__
#define __OFFLINE_LOG_H__

int offline_log_append(const unsigned char *line, unsigned int len);
int offline_log_dump(const char *path);
int offline_log_get_mutex(void);
int offline_log_release_mutex(void);
unsigned char *offline_log_get_power_on_log_buffer(unsigned int *len);
int file_log_append(const unsigned char *line, unsigned int len);
int file_log_close(void);

#endif//__OFFLINE_LOG_H__

