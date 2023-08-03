#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <sys/time.h>
#include <sys/stat.h>
#include "AmbaTest_msg.h"
#include "ambatestc.h"

#define LINE_START_WITH(x)      !strncmp(line, x, strlen(x))

#define CMSG(fmt, ...)    log_msg(LL_CRITICAL, fmt, ##__VA_ARGS__)
#define IMSG(fmt, ...)    log_msg(LL_NORMAL,   fmt, ##__VA_ARGS__)
#define LINE_SIZE               1024

enum {
        ATC_STATUS_REGISTERED,
        ATC_STATUS_READY,
        ATC_STATUS_CLOSE,
};

typedef struct {
        int             socket;
        int             mode;
        int             status;
        int             seqnum;
        int             exit_flag;
        atc_header_t    reply;
        pthread_t       rx_thread;
        pthread_mutex_t lock;
        pthread_cond_t  cond;
        char            buf[MAX_LOGSIZE];
} client_t;

static int recv_from_server(client_t *client, void *_buf, int size)
{
        char *buf = (char *)_buf;
        int pos = 0, fd = client->socket;

        while (pos < size) {
                int nbytes = recv(fd, buf+pos, size-pos, 0);
                if (nbytes == 0) {
                        client->exit_flag = 1;
                        pos = 0;
                        break;
                } else if (nbytes == -1) {
                        if (errno != EAGAIN) {
                                client->exit_flag = 1;
                                pos = 0;
                                break;
                        }
                        continue;
                }
                pos += nbytes;
        }
        return pos;
}

static int send_to_server(client_t *client, const void *_buf, int size)
{
        const char *buf = (const char *)_buf;
        int pos = 0, fd = client->socket;

        while (pos < size) {
                int nbytes = send(fd, buf+pos, size-pos, 0);
                pos += nbytes;
        }
        return AT_ERR_OK;
}

static int fill_num(char *dst, const char *src,  const char *pat, uint32_t n)
{
        const char *mark;
        char num[16];
        int found = 0;

        sprintf(num, "%d", n);
        dst[0] = 0;
        while ((mark = strstr(src, pat)) != NULL) {
                found = 1;
                int len = mark - src;
                strncpy(dst, src, len);
                dst += len;
                src += len + strlen(pat);
                strcpy(dst, num);
                dst += strlen(num);
        }
        strcpy(dst, src);
        return found;
}

static void waitfor_reply(client_t *client, int seqnum)
{
        struct timeval now;
        struct timespec ts = {};
        int ret;
        
        pthread_mutex_lock(&client->lock);
        while (seqnum > client->reply.seqnum) {
                gettimeofday(&now, NULL);
                ts.tv_sec = now.tv_sec + 2;
                ts.tv_nsec = 0;
                ret = pthread_cond_timedwait(&client->cond, &client->lock, &ts);
                if (ret == ETIMEDOUT && client->exit_flag) 
                        break;
        }
        pthread_mutex_unlock(&client->lock);
}

static int parse_run(const char *line, char *pat) {
        char *start, *end;
        int loops = 1;

        start = strchr(line, '"');
        if (start != NULL) {
                start++;
                end = strrchr(start, '"');
                if (end == NULL) {
                        CMSG("syntax error --> %s\n", line);
                        return 0;
                }
                strncpy(pat, start, end - start);
                pat[end-start] = 0;
                sscanf(end+1, "%d", &loops);

        } else {
                sscanf(line, "[run] %s %d\n", pat, &loops);
        }

        return loops;
}

static int register_with_test_server(
        client_t *client,
        const char *username,
        const char *ipaddr)
{
        atc_header_t header;
        atc_register_t info;

        memset(&info, 0, sizeof(info));
        header.type   = AMBA_TEST_MSG_REGISTER;
        header.len    = sizeof(info);
        header.seqnum = ++client->seqnum;
        if (username != NULL)
                strcpy(info.client_username, username);
        else
                getlogin_r(info.client_username, sizeof(info.client_username));
        if (ipaddr != NULL)
                strcpy(info.client_ipaddr, ipaddr);
        send_to_server(client, &header, sizeof(header));
        send_to_server(client, &info, sizeof(info));

        CMSG("send [register]\n"); 
        waitfor_reply(client, header.seqnum);
        return client->reply.ret;
}

static void deregister_with_test_server(client_t *client)
{
        atc_header_t header;

        header.type   = AMBA_TEST_MSG_DEREGISTER;
        header.len    = 0;
        header.seqnum = ++client->seqnum;
        send_to_server(client, &header, sizeof(header));

        CMSG("send [deregister]\n"); 
        waitfor_reply(client, header.seqnum);
}

static void req_open(char *line, client_t *client, FILE *tfp)
{
        atc_header_t hdr;
        atc_open_t msg;
        char mode[32], priority[32], pat[256];
        int nread, pos;
        size_t len = LINE_SIZE;

        if (client->status != ATC_STATUS_REGISTERED) {
                CMSG("send [open]  skipped due to wrong status\n");
                return;
        }

        nread = sscanf(line, "[open] %s %s %s %s",
                       msg.device_spec.device_hw,
                       msg.device_spec.device_os,
                       mode, priority);
        if (nread >= 3) {
                if (!strcmp(mode, "SINGLE")) {
                        msg.mode = ATC_CONN_MODE_SINGLE;
                } else if (!strcmp(mode, "MULTI")) {
                        msg.mode = ATC_CONN_MODE_MULTI;
                } else {
                        CMSG("unknow device mode %s\n", mode);
                        return;
                }
        } else {
                msg.mode = ATC_CONN_MODE_SINGLE;
        }
        client->mode = msg.mode;

        if (nread >= 4) {
                msg.priority = atoi(priority);
        } else {
                msg.priority = 128;
        }

        /* find number of jobs for this client */
        pos = ftell(tfp);
        msg.num_jobs = 0;
        while ((nread = getline(&line, &len, tfp)) != -1) {
                /* skip comments or empty line */
                if (line[0] == '#' || nread == strspn(line, " \r\n\t")) {
                        continue;
                } else if (LINE_START_WITH("[open]")) {
                        CMSG("unpaird OPEN found, exit\n");
                        exit(-1);
                } else if (LINE_START_WITH("[run]")) {
                        msg.num_jobs += parse_run(line, pat); 
                } else if (LINE_START_WITH("[close]")) {
                        break;
                }
        }
        fseek(tfp, pos, SEEK_SET);

        hdr.type   = AMBA_TEST_MSG_OPEN;
        hdr.len    = sizeof(msg);
        hdr.seqnum = ++client->seqnum;
        send_to_server(client, &hdr, sizeof(hdr));
        send_to_server(client, &msg, sizeof(msg));

        CMSG("send [open] \n"); 
        waitfor_reply(client, hdr.seqnum);
}

static void req_run(const char *line, client_t *client)
{
        atc_header_t hdr;
        atc_drun_t msg;
        char pat[256];
        int i, loops;

        if (client->status != ATC_STATUS_READY) {
                CMSG("send [drun]  skipped due to wrong status\n");
                return;
        }

        loops = parse_run(line, pat);
        if (loops == 0) return;

        hdr.type    = AMBA_TEST_MSG_DRUN;
        hdr.len     = sizeof(msg);
        msg.timeout = 0;
        for (i = 0; i < loops; i++) {
                int found = fill_num(msg.command, pat, "$#", i);
                hdr.seqnum = ++client->seqnum;
                send_to_server(client, &hdr, sizeof(hdr));
                send_to_server(client, &msg, sizeof(msg));
                if (loops == 1 || found) {
                        IMSG("send [drun]  %s \n", msg.command);
                } else {
                        IMSG("send [drun]  %s (%d)\n", msg.command, i);
                }
        }
}

static void req_push(const char *line, client_t *client)
{
        int fd, size;
        atc_header_t hdr;
        atc_push_t msg;
        struct stat st;
        char buf[4096];

        if (client->status != ATC_STATUS_READY) {
                CMSG("send [push]  skipped due to wrong status\n");
                return;
        }

        if (client->mode != ATC_CONN_MODE_SINGLE) {
                CMSG("send [push]  Error! skip invalid operation\n");
                return;
        }

        /* open source file for pushing */
        sscanf(line, "[push] %s %s\n", msg.src, msg.dst);
        stat(msg.src, &st);
        fd = open(msg.src, O_RDONLY);
        if (fd <= 0) {
                CMSG("Can't open push source %s\n", msg.src);
                exit(-1);
        }

        /* send the header */
        hdr.type = AMBA_TEST_MSG_PUSH;
        hdr.len = sizeof(msg);
        hdr.seqnum = ++client->seqnum;
        send_to_server(client, &hdr, sizeof(hdr));

        /* send the atc_push_t message */
        msg.len = st.st_size;
        send_to_server(client, &msg, sizeof(msg));

        /* send the file content */
        size = st.st_size;
        while (size > 0) {
                int nbytes = (size < sizeof(buf)) ? size : sizeof(buf);
                nbytes = read(fd, buf, nbytes);
                send_to_server(client, buf, nbytes);
                size -= nbytes;
        }
        close(fd);

        CMSG("send [push]  %s --> %s\n", msg.src, msg.dst);
        waitfor_reply(client, hdr.seqnum);
}

static void req_pull(const char *line, client_t *client)
{
        int fd;
        atc_header_t hdr;
        atc_pull_t msg;

        if (client->status != ATC_STATUS_READY) {
                CMSG("send [pull]  skipped due to wrong status\n");
                return;
        }

        if (client->mode != ATC_CONN_MODE_SINGLE) {
                CMSG("send [push]  Error! skip invalid operation\n");
                return;
        }

        /* check if we can open destination file */
        sscanf(line, "[pull] %s %s\n", msg.src, msg.dst);
        fd = open(msg.dst, O_CREAT | O_WRONLY | O_TRUNC, 0600);
        if (fd <= 0) {
                CMSG("Can't open pull destionation %s\n", msg.dst);
                exit(-1);
        }
        close(fd);

        /* send the header */
        hdr.type = AMBA_TEST_MSG_PULL;
        hdr.len = sizeof(msg);
        hdr.seqnum = ++client->seqnum;
        send_to_server(client, &hdr, sizeof(hdr));

        /* send the atc_push_t message */
        msg.len = 0;
        send_to_server(client, &msg, sizeof(msg));

        CMSG("send [pull]  %s --> %s\n", msg.src, msg.dst);
        waitfor_reply(client, hdr.seqnum);
}

static void req_close(const char *line, client_t *client)
{
        atc_header_t hdr;
        atc_close_t msg;

        if (client->status != ATC_STATUS_READY) {
                CMSG("send [close] skipped due to wrong status\n");
                return;
        }

        hdr.type = AMBA_TEST_MSG_CLOSE;
        hdr.len = sizeof(msg);
        hdr.seqnum = ++client->seqnum;
        send_to_server(client, &hdr, sizeof(hdr));
        send_to_server(client, &msg, sizeof(msg));
        CMSG("send [close] \n");
        waitfor_reply(client, hdr.seqnum);
}

static void req_stat(const char *line, client_t *client)
{
        atc_header_t hdr;

        hdr.type = AMBA_TEST_MSG_STAT;
        hdr.len = 0;
        hdr.seqnum = ++client->seqnum;
        send_to_server(client, &hdr, sizeof(hdr));
        CMSG("send [stat] \n");
        waitfor_reply(client, hdr.seqnum);
}

static void ack_open(client_t *client, atc_header_t *header)
{
        atc_open_t msg;
        atd_spec_t *spec = &msg.device_spec;

        recv_from_server(client, &msg, sizeof(msg));
        CMSG("recv [open]  %s %s\n", spec->device_hw, spec->device_os);
        if (header->ret != AT_ERR_OK) {
                CMSG("\t %s\n", ambatest_strerror(header->ret));
                return;
        }
        client->status = ATC_STATUS_READY;
}

static void ack_close(client_t *client, atc_header_t *header)
{
        atc_close_t msg;

        recv_from_server(client, &msg, sizeof(msg));
        CMSG("recv [close]\n");
        client->status = ATC_STATUS_REGISTERED;
}

static void ack_drun(client_t *client, atc_header_t *header)
{
        char *buf = client->buf;
        atc_drun_t msg;
        int size = header->len;

        recv_from_server(client, &msg, sizeof(msg));
        size -= sizeof(msg);
        assert(size <= MAX_LOGSIZE);
        recv_from_server(client, buf, size);
        IMSG("recv [drun]  %s\n", msg.command);
        printf("%s\n", buf);
}

static void ack_push(client_t *client, atc_header_t *header)
{
        atc_push_t msg;

        recv_from_server(client, &msg, sizeof(msg));
        CMSG("recv [push]  %s --> %s\n", msg.src, msg.dst);

        if (header->ret != AT_ERR_OK) {
                CMSG("\t %s\n", ambatest_strerror(header->ret));
        }
}

static void ack_pull(client_t *client, atc_header_t *header)
{
        atc_pull_t msg;
        char *buf = client->buf;
        int fd, size, nbytes;

        recv_from_server(client, &msg, sizeof(msg));
        CMSG("recv [pull]  %s --> %s\n", msg.src, msg.dst);

        if (header->ret != AT_ERR_OK) {
                CMSG("\t %s\n", ambatest_strerror(header->ret));
                return;
        }

        fd = open(msg.dst, O_CREAT | O_WRONLY | O_TRUNC, 0600);
        for (size = msg.len; size > 0; size -= nbytes) {
                nbytes = (size < MAX_LOGSIZE) ? size : MAX_LOGSIZE;
                nbytes = recv_from_server(client, buf, nbytes);
                nbytes = write(fd, buf, nbytes);
        }
        close(fd);
}

static void ack_stat(client_t *client, atc_header_t *header) 
{
        char *base;
        at_stat_t *stat;
        atd_stat_t *dev;
        atc_stat_t *clnt;
        atu_stat_t *user;
        struct timeval tv;
        struct tm *time;
        char timestr[32];
        int i;

        gettimeofday(&tv, NULL);
        base = malloc(header->len);
        stat = (at_stat_t*)base;

        recv_from_server(client, stat, header->len);
        CMSG("recv [stat]  Test Server Statistics:\n\n");

        printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
        printf("                        devices: %d\n", stat->device_num);
        printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
        dev = (atd_stat_t*) (base + stat->device_stat_offset);
        for (i = 0; i < stat->device_num; i++, dev++) {
                atd_spec_t *p = &dev->spec;
                time_t sec = tv.tv_sec - dev->uptime;
                time = localtime(&sec);
                strftime(timestr, sizeof(timestr), "%m-%d %H:%M:%S", time);
                printf("DEVICE: ID       = %d\n", dev->id);
                printf("\tspec.    = %s, %s\n", p->device_hw, p->device_os); 
                printf("\tnum_jobs = %d\n", dev->num_jobs);
                printf("\tup since = %s\n", timestr);
                printf("\tavg_load = %d%%\n", dev->busytime/10/dev->uptime);
                printf("\n");
        }

        printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
        printf("                        clients: %d\n", stat->client_num);
        printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
        clnt = (atc_stat_t*) (base + stat->client_stat_offset);
        for (i = 0; i < stat->client_num; i++, clnt++) {
                atd_spec_t *p = &clnt->spec;
                time_t sec = tv.tv_sec - (time_t)clnt->uptime / 1000;
                time = localtime(&sec);
                strftime(timestr, sizeof(timestr), "%m-%d %H:%M:%S", time);
                printf("CLIENT: ID       = %d\n", clnt->id);
                printf("\tusername = %s\n", clnt->username);
                printf("\tup since = %s\n", timestr);
                printf("\tspec.    = %s %s", p->device_hw, p->device_os); 
                if (clnt->device_id >= 0)
                        printf(", locked to device %d\n", clnt->device_id);
                else 
                        printf("\n");
                printf("\tprogress = %d/%d\n", clnt->job_done, clnt->job_total);
                printf("\n");
        }

        printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
        printf("                           usrs: %d\n", stat->user_num);
        printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
        user = (atu_stat_t*) (base + stat->user_stat_offset);
        for (i = 0; i < stat->user_num; i++, user++) {
                printf("USER:   username = %s\n", user->username);
                printf("\tclients  = %d\n", user->num_clients);
                printf("\tjobs     = %d\n", user->num_jobs);
                printf("\tcputime  = %d ms\n", user->cputime);
                printf("\n");
        }

        printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
        printf("                total jobs done: %" PRIu64 "\n", stat->num_jobs);
        printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");

        free(base);
}
        
static void* rx_entry(void *arg)
{
        client_t *client = (client_t*)arg;

        while (1) {
                atc_header_t header;
                int nbytes = recv_from_server(client, &header, sizeof(header));

                /* handle exit cases */
                if (nbytes <= 0) {
                        CMSG("Disconnected from server!\n");
                        break;
                }

                /* ack some message types */
                switch(header.type) {
                case AMBA_TEST_MSG_OPEN:
                        ack_open(client, &header);
                        break;
                case AMBA_TEST_MSG_CLOSE:
                        ack_close(client, &header);
                        break;
                case AMBA_TEST_MSG_DRUN:
                        ack_drun(client, &header);
                        break;
                case AMBA_TEST_MSG_PUSH:
                        ack_push(client, &header);
                        break;
                case AMBA_TEST_MSG_PULL:
                        ack_pull(client, &header);
                        break;
                case AMBA_TEST_MSG_STAT:
                        ack_stat(client, &header);
                        break;
                }

                /* wake up waiting client */
                client->reply = header;
                pthread_mutex_lock(&client->lock);
                pthread_cond_signal(&client->cond);
                pthread_mutex_unlock(&client->lock);

                if (header.type == AMBA_TEST_MSG_DEREGISTER)
                        break;
        }

        return client;
}

static client_t *alloc_client(
        const char *server_addr, 
        int server_port,
        const char *username,
        const char *ipaddr)
{
        client_t *client;
        client = calloc(sizeof(client_t), 1);
        client->socket = connect_to_server(server_addr, server_port);
        pthread_create(&client->rx_thread, NULL, rx_entry, client);
        if (register_with_test_server(client, username, ipaddr) == AT_ERR_OK) {
                pthread_mutex_init(&client->lock, NULL);
                pthread_cond_init(&client->cond, NULL);
        } else {
                CMSG("Connection rejected by server!\n");
                free(client);
                client = NULL;
        }
        return client;
}

static void free_client(client_t *client)
{
        deregister_with_test_server(client);
        close(client->socket);
        client->exit_flag = 1;
        pthread_join(client->rx_thread, NULL);
        pthread_mutex_destroy(&client->lock);
        pthread_cond_destroy(&client->cond);
        free(client);
}

int execute_test_script(
        FILE *tfp, 
        const char *server_addr, 
        int server_port,
        const char *username,
        const char *ipaddr)
{
        char *line = NULL;
        size_t len = LINE_SIZE;
        int nread;
        client_t *client;

        client = alloc_client(server_addr, server_port, username, ipaddr);
        if (client == NULL) 
                return -1;

        line = malloc(LINE_SIZE);

        while ((nread = getline(&line, &len, tfp)) != -1) {
                /* skip comments or empty line */
                if (line[0] == '#' || nread == strspn(line, " \r\n\t")) {
                        continue;
                } else if (LINE_START_WITH("[open]")) {
                        req_open(line, client, tfp);
                } else if (LINE_START_WITH("[run]")) {
                        req_run(line, client);
                } else if (LINE_START_WITH("[push]")) {
                        req_push(line, client);
                } else if (LINE_START_WITH("[pull]")) {
                        req_pull(line, client);
                } else if (LINE_START_WITH("[stat]")) {
                        req_stat(line, client);
                } else if (LINE_START_WITH("[close]")) {
                        req_close(line, client);
                } else {
                        CMSG("unsupport line: %s\n", line);
                }
        }

        free(line);
        free_client(client);
        return 0;
}
