#ifndef SERIAL_H
#define SERIAL_H

int serial_init(const char *serial_dev_name, int *p_fd, int rtc_cts, int speed, int need_line_input);
int serial_write(int fd, void *src, int len);
int serial_read(int fd, char *buf, int len, int timeout_ms);
int serial_close(int fd);

#endif /* SERIAL_H */

