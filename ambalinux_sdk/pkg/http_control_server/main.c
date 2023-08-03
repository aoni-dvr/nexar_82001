#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <pthread.h>
#include "debug.h"
#include "cmd_handler.h"
#include "cmd_response.h"
#include "cmd_upload.c"
#include "cmd_ota.c"
#include "rtos_api_lib.h"

static pthread_mutex_t mutex;

#define ISspace(x) isspace((int)(x))

static void *accept_request(void *arg);
static void bad_request(int);
static void cat(int, FILE *);
static void cannot_execute(int);
static void error_die(const char *);
static void handler(int, const char *, const char *, const char *);
static int get_line(int, char *, int);
static void headers(int, const char *);
static int startup(u_short *);
static void unimplemented(int);

static void *accept_request(void *arg)
{
    char buf[1024];
    int numchars;
    char method[255];
    char url[255];
    char *query_string = NULL;
    size_t i, j;
    struct stat st;
    int client = *((int *)arg);

    pthread_mutex_unlock(&mutex);

    numchars = get_line(client, buf, sizeof(buf));
    i = 0; j = 0;
    while (!ISspace(buf[j]) && (i < sizeof(method) - 1)) {
        method[i] = buf[j];
        i++; j++;
    }
    method[i] = '\0';

    if (strcasecmp(method, "GET") && strcasecmp(method, "POST")) {
        unimplemented(client);
        return NULL;
    }

    i = 0;
    while (ISspace(buf[j]) && (j < sizeof(buf))) {
        j++;
    }
    while (!ISspace(buf[j]) && (i < sizeof(url) - 1) && (j < sizeof(buf))) {
        url[i] = buf[j];
        i++; j++;
    }
    url[i] = '\0';

    query_string = url;
    while ((*query_string != '?') && (*query_string != '\0')) {
        query_string++;
    }
    if (*query_string == '?')  {
        *query_string = '\0';
        query_string++;
    }
    printf("\n");
    debug_line("path: %s, query_str: %s, method: %s", url, query_string, method);
    handler(client, url, method, query_string);
    sleep(2);
    close(client);

    return NULL;
}

static void bad_request(int client)
{
    char buf[1024];

    sprintf(buf, "HTTP/1.0 400 BAD REQUEST\r\n");
    send(client, buf, sizeof(buf), 0);
    sprintf(buf, "Content-type: text/html\r\n");
    send(client, buf, sizeof(buf), 0);
    sprintf(buf, "\r\n");
    send(client, buf, sizeof(buf), 0);
    sprintf(buf, "<P>Your browser sent a bad request, ");
    send(client, buf, sizeof(buf), 0);
    sprintf(buf, "such as a POST without a Content-Length.\r\n");
    send(client, buf, sizeof(buf), 0);
}

static void cat(int client, FILE *resource)
{
    char buf[1024];

    fgets(buf, sizeof(buf), resource);
    while (!feof(resource)) {
        send(client, buf, strlen(buf), 0);
        fgets(buf, sizeof(buf), resource);
    }
}

static void cannot_execute(int client)
{
    char buf[1024];

    sprintf(buf, "HTTP/1.0 500 Internal Server Error\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-type: text/html\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<P>Error prohibited CGI execution.\r\n");
    send(client, buf, strlen(buf), 0);
}

static void error_die(const char *sc)
{
    perror(sc);
    exit(1);
}

static void handler(int client, const char *path, const char *method, const char *query_string)
{
    int rval = -1;

    if (strcasecmp(method, "GET") == 0) {
        rval = cmd_do_get_handler(client, path, query_string);
    } else {   /* POST */
        char buf[1024] = {0};
        int content_length = -1;
        int numchars = 0;

        numchars = get_line(client, buf, sizeof(buf));
        //debug_line("%.*s", numchars, buf);
        while ((numchars > 0) && strcmp("\n", buf)) {
            buf[15] = '\0';
            if (strcasecmp(buf, "Content-Length:") == 0) {
                content_length = atoi(&(buf[16]));
            }
            numchars = get_line(client, buf, sizeof(buf));
            //debug_line("%.*s", numchars, buf);
        }
        if (content_length == -1) {
            bad_request(client);
            return;
        }

        //debug_line("%s", buf);
        if (strcmp(path, "/api/ctrl") == 0
            || strcmp(path, "/api/xctrl") == 0) {
            memset(buf, 0, sizeof(buf));
            recv(client, buf, content_length, 0);
            rval = cmd_do_ctrl_handler(client, buf);
        } else if (strcmp(path, "/api/upload") == 0) {
            ///api/upload?file=/data/mgaoffilne.ubx&md5=32432342341232312321323232323
            char file[64] = {0};
            char md5[64] = {0};
            memset(file, 0, sizeof(file));
            memset(md5, 0, sizeof(md5));
            parse_key(query_string, "file=", file, sizeof(file) - 1);
            parse_key(query_string, "md5=", md5, sizeof(md5) - 1);
            rval = cmd_do_upload(client, query_string + strlen("file="), content_length);
        } else if (strcmp(path, "/api/otaupload") == 0) {
            ///api/otaupload
             rval = cmd_do_upload(client, SD_MOUNT_PATH"ota.bin", content_length);
        } else if (strcmp(path, "/api/doupdate") == 0) {
            memset(buf, 0, sizeof(buf));
            recv(client, buf, content_length, 0);
            rval = cmd_do_ota_update(client, buf);
        } else {
            rval = RESPONSE_CODE_INVALID_PARAM;
        }
    }

    if (rval != 0) {
        send_code(client, rval);
    }
}

static int get_line(int sock, char *buf, int size)
{
    int i = 0;
    char c = '\0';
    int n;

    while ((i < size - 1) && (c != '\n')) {
        n = recv(sock, &c, 1, 0);
        /* DEBUG printf("%02X\n", c); */
        if (n > 0) {
            if (c == '\r') {
                n = recv(sock, &c, 1, MSG_PEEK);
                /* DEBUG printf("%02X\n", c); */
                if ((n > 0) && (c == '\n')) {
                     recv(sock, &c, 1, 0);
                } else {
                     c = '\n';
                }
            }
            buf[i] = c;
            i++;
        } else {
            c = '\n';
        }
    }
    buf[i] = '\0';

    return(i);
}

static void headers(int client, const char *filename)
{
    char buf[1024];
    (void)filename;  /* could use filename to determine file type */

    strcpy(buf, "HTTP/1.0 200 OK\r\n");
    send(client, buf, strlen(buf), 0);
    strcpy(buf, SERVER_STRING);
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-Type: text/html\r\n");
    send(client, buf, strlen(buf), 0);
    strcpy(buf, "\r\n");
    send(client, buf, strlen(buf), 0);
}

static int startup(u_short *port)
{
    int sock = 0;
    struct sockaddr_in name;
    unsigned int value = 1;
    int nNetTimeout = 10000;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        error_die("socket");
    }
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (void *)&value, sizeof(value));
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&nNetTimeout, sizeof(int));
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&nNetTimeout, sizeof(int));
    memset(&name, 0, sizeof(name));
    name.sin_family = AF_INET;
    name.sin_port = htons(*port);
    name.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(sock, (struct sockaddr *)&name, sizeof(name)) < 0) {
        error_die("bind");
    }
    if (*port == 0) { /* if dynamically allocating a port */
        int namelen = sizeof(name);
        if (getsockname(sock, (struct sockaddr *)&name, &namelen) == -1) {
            error_die("getsockname");
        }
        *port = ntohs(name.sin_port);
    }
    if (listen(sock, 10) < 0) {
        error_die("listen");
    }

    return(sock);
}

static void unimplemented(int client)
{
    char buf[1024];

    sprintf(buf, "HTTP/1.0 501 Method Not Implemented\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, SERVER_STRING);
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-Type: text/html\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<HTML><HEAD><TITLE>Method Not Implemented\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "</TITLE></HEAD>\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<BODY><P>HTTP request method not supported.\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "</BODY></HTML>\r\n");
    send(client, buf, strlen(buf), 0);
}

global_s global_data;


static void signal_handler(int signal)
{
    static int bExit = 0;
    if (bExit == 0) {
        bExit = 1;
        debug_line("application exit by signal:%d", signal);
        exit(1);
    }
}

static void capture_all_signal(void)
{
    for (int i = 0; i < 32; i++) {
        if ((i == SIGPIPE) || (i == SIGCHLD) || (i == SIGALRM) || (i == SIGPROF)) {            
            signal(i, SIG_IGN);
        } else {
            signal(i, signal_handler);
        }
    }
}

static void on_app_exit(void)
{
    signal_handler(0);
}

int main(int argc, char **argv)
{
    int server_sock = -1;
    u_short port = 80;
    int client_sock = -1;
    struct sockaddr_in client_name;
    int client_name_len = sizeof(client_name);
    pthread_t newthread;

    atexit(on_app_exit);
    capture_all_signal();

    if (daemon(0, 1)) {
        debug_line("Couldn't start daemon!\n");
        exit(1);
    }

    if (pthread_mutex_init(&mutex, NULL) != 0) {
        debug_line("Fail to create mutex!!\n");
        exit(1);
    }

    //init global data
    memset(&global_data, 0, sizeof(global_data));
    if (rtos_api_lib_get_dev_info(&global_data.dev_info) < 0) {
        debug_line("%s get dev info failed", argv[0]);
    }
    debug_line("token=%s,first_access=%d", global_data.dev_info.token, global_data.dev_info.first_access);
#if 0
    if (argc > 1) {
        global_data.authed = 1;//just for debug
    }
#endif
    //start http server
    server_sock = startup(&port);
    debug_line("httpd running on port %d", port);

    while (1) {
        pthread_mutex_lock(&mutex);
        client_sock = accept(server_sock, (struct sockaddr *)&client_name, &client_name_len);
        if (client_sock == -1) {
            error_die("accept");
        }
        if (pthread_create(&newthread , NULL, accept_request, &client_sock) != 0) {
            perror("pthread_create");
            pthread_mutex_unlock(&mutex);
        }
        pthread_detach(newthread);
    }
    close(server_sock);

    return (0);
}

