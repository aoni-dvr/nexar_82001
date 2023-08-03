#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <string.h>

static int speed_arr[] = {B115200, B57600,  B38400, B19200, B9600, B4800, B2400, B1200, B300,
          B38400, B19200, B9600, B4800, B2400, B1200, B300, };
static int name_arr[] = {115200, 57600, 38400,  19200,  9600,  4800,  2400,  1200,  300, 38400,  
          19200,  9600, 4800, 2400, 1200,  300, };
static int set_speed(int fd, int speed)
{
    int   i;
    int   status;
    struct termios   Opt;

    tcgetattr(fd, &Opt);
    for (i= 0;  i < sizeof(speed_arr) / sizeof(int);  i++) {
        if  (speed == name_arr[i]) {
            tcflush(fd, TCIOFLUSH);
            cfsetispeed(&Opt, speed_arr[i]);
            cfsetospeed(&Opt, speed_arr[i]);
            status = tcsetattr(fd, TCSANOW, &Opt);
            if  (status != 0) {
                printf("tcsetattr failed\n");
                return 1;
            }
            tcflush(fd,TCIOFLUSH);
        }
    }

    return 0;
}

static int set_parity(int fd, int databits, int parity, int stopbits, int rtc_cts)
{
    struct termios options;
    if (tcgetattr( fd,&options) != 0) {
        perror("SetupSerial 1");
        return -1;
    }
    options.c_cflag &= ~CSIZE;
    switch (databits) {
    case 7:
        options.c_cflag |= CS7;
        break;
    case 8:
        options.c_cflag |= CS8;
        break;
    default:
        fprintf(stderr, "Unsupported data size\n");
        return -1;
    }
    options.c_iflag |= INPCK;
    cfmakeraw(&options);
    //options.c_lflag |= (ICANON | ECHO | ECHOE);
    //options.c_lflag &= ~(ICANON | ECHO | ECHOE);
    //options.c_iflag &= ~(IXON | IXOFF);
    switch (parity) {
    case 'n':
    case 'N':
        options.c_cflag &= ~PARENB;   /* Clear parity enable */
        options.c_iflag &= ~INPCK;     /* Enable parity checking */
        break;
    case 'o':
    case 'O':
        options.c_cflag |= (PARODD | PARENB);
        break;
    case 'e':
    case 'E':
        options.c_cflag |= PARENB;
        options.c_cflag &= ~PARODD;
        break;
    case 'S':
    case 's':  /*as no parity*/
        options.c_cflag &= ~PARENB;
        options.c_cflag &= ~CSTOPB;
        break;
    default:
        fprintf(stderr, "Unsupported parity\n");
        return -1;
    }  

    switch (stopbits) {
    case 1:
        options.c_cflag &= ~CSTOPB;
        break;
    case 2:
        options.c_cflag |= CSTOPB;
       break;
    default:
         fprintf(stderr, "Unsupported stop bits\n");
         return -1;
    }

    /* Set rts/cts */ 
    if (rtc_cts) {
        printf("Set rts/cts");
        options.c_cflag |= CRTSCTS;
    }

    tcflush(fd,TCIFLUSH);
    options.c_cc[VTIME] = 150;
    options.c_cc[VMIN] = 0;
    if (tcsetattr(fd,TCSANOW,&options) != 0) {
        printf("SetupSerial failed\n");
        return -1;
    }
    return 0;
}

static int serial_set_line_input(int fd)
{
    struct termios options;
    if (tcgetattr(fd, &options) != 0) {
        perror("SetupSerial 1");
        return -1;
    }
    options.c_lflag |= ICANON;
    tcflush(fd, TCIFLUSH);
    options.c_cc[VTIME] = 150;
    options.c_cc[VMIN] = 0;
    if (tcsetattr(fd, TCSANOW, &options) != 0) {
        perror("SetupSerial 3");
        return -1;
    }
    return 0;
}

int serial_init(const char *serial_dev_name, int *p_fd, int rtc_cts, int speed, int need_line_input)
{
    int fd;

    fd = open(serial_dev_name, O_RDWR);
    if (fd < 0) {
        printf("init %s failed\n", serial_dev_name);
        return -1;
    }
    set_speed(fd, speed);
    set_parity(fd, 8, 'n', 1, rtc_cts);
    if (need_line_input) serial_set_line_input(fd);
    *p_fd = fd;

    return 0;
}

int serial_write(int fd, void *src, int len)
{
    int ret = write(fd, src, len);
    if (len != ret) perror("oh, write serial failed!");
    return 0;
}

int serial_read(int fd, char *buf, int len, int timeout_ms)
{
    int received = 0, ret = 0;
    fd_set pending_data;
    struct timeval block_time;

    FD_ZERO(&pending_data);
    FD_SET(fd, &pending_data);
    block_time.tv_sec = (timeout_ms / 1000);
    block_time.tv_usec = (timeout_ms % 1000) * 1000;
    switch (select(fd + 1, &pending_data, NULL, NULL, &block_time)) {
    case 0:
        memset(buf, 0, len);
        received = 0;
        break;
    case -1:
        printf("%s, error\n", __func__);
        received = -1;
        break;
    default:
        ret = read(fd, buf, len);
        if (ret <= 0) {
            received = -1;
            break;
        }
        received = ret;
        break;
    }

    return received;

}

int serial_close(int fd)
{
    if (fd > 0) {
        close(fd);
    }

    return 0;
}

