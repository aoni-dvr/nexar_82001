#ifndef  __SERIAL_PORT_H__
#define  __SERIAL_PORT_H__

int serial_init(const char *serial_dev_name, int *p_fd, int RTSCTS, int speed, int need_line_input);
int serial_write(int fd_serial, void *src, int len);
int serial_read(int fd, unsigned char *buf, int len, int timeout_ms);
int serial_close(int fd_serial);

#endif

