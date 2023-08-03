#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/in.h>	/* sockaddr_in{} and other Internet defns */
#include <arpa/inet.h>	/* inet(3) functions */

#include <netinet/tcp.h>

int make_socket_nonblocking(int sock) {
  int curFlags = fcntl(sock, F_GETFL, 0);
  return fcntl(sock, F_SETFL, curFlags|O_NONBLOCK);
}

int make_socket_blocking(int sock) {
  int curFlags = fcntl(sock, F_GETFL, 0);
  return fcntl(sock, F_SETFL, curFlags&(~O_NONBLOCK));
}

static unsigned get_buffer_size(int bufOptName,
			      int socket) {
  unsigned curSize;
  int sizeSize = sizeof curSize;
  if (getsockopt(socket, SOL_SOCKET, bufOptName,
		 (char*)&curSize, (socklen_t *)&sizeSize) < 0) {
    fprintf(stderr, "getBufferSize() error\n");
    return 0;
  }

  return curSize;
}


static unsigned increase_buf_to(int bufOptName, int sock, unsigned request_size) {
    // First, get the current buffer size.  If it's already at least
    // as big as what we're requesting, do nothing.
    unsigned curSize = get_buffer_size(bufOptName, sock);

    // Next, try to increase the buffer to the requested size,
    // or to some smaller size, if that's not possible:
    while (request_size > curSize) {
      int sizeSize = sizeof request_size;
      if (setsockopt(sock, SOL_SOCKET, bufOptName,
             (char*)&request_size, sizeSize) >= 0) {
        // success
        return request_size;
      }
      request_size = (request_size+curSize)/2;
    }

    return get_buffer_size(bufOptName, sock);
}

int increase_snd_buf(int sock, unsigned int request_size) {
    return increase_buf_to(SO_SNDBUF, sock, request_size);
}

int increase_rcv_buf(int sock, unsigned int request_size) {
    return increase_buf_to(SO_RCVBUF, sock, request_size);

}

int enable_tcp_nodelay(int sock) {
    int nodelay = 1;
    return setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, &nodelay, sizeof(nodelay));
}

int disable_tcp_nodelay(int sock) {
    int nodelay = 0;
    return setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, &nodelay, sizeof(nodelay));
}


int setup_stream_socket(unsigned int addr, unsigned short port, int nonblocking) {
    struct sockaddr_in servaddr;
    unsigned int reuse_flag = 1;
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0) {
        fprintf(stderr, "unable to create stream socket\n");
        return sock;
    }
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse_flag, sizeof reuse_flag) < 0) {
        fprintf(stderr, "setsockopt(SO_REUSEADDR) error\n");
        close(sock);
        return -1;
    }
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(addr);
    servaddr.sin_port        = htons(port);
    if (bind(sock, (struct sockaddr*)&servaddr, sizeof servaddr) != 0) {
        fprintf(stderr, "bind() error (port number: %d)\n", port);
        close(sock);
        return -1;
        }
    if (nonblocking) {
        if(make_socket_nonblocking(sock) < 0) {
            fprintf(stderr, "failed to make non-blocking");
            close(sock);
            return -1;
        }
    }
    return sock;
}

int setup_datagram_socket(unsigned int addr, unsigned short port, int nonblocking) {
    struct sockaddr_in servaddr;
    unsigned int reuse_flag = 1;
    int sock = socket(AF_INET, SOCK_DGRAM, 0);

    if (sock < 0) {
        fprintf(stderr, "unable to create stream socket\n");
        return sock;
    }
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse_flag, sizeof reuse_flag) < 0) {
        fprintf(stderr, "setsockopt(SO_REUSEADDR) error\n");
        close(sock);
        return -1;
    }
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(addr);
    servaddr.sin_port        = htons(port);
    if (bind(sock, (struct sockaddr*)&servaddr, sizeof servaddr) != 0) {
        fprintf(stderr, "bind() error (port number: %d)\n", port);
        close(sock);
        return -1;
        }
    if (nonblocking) {
        if(make_socket_nonblocking(sock) < 0) {
            fprintf(stderr, "failed to make non-blocking");
            close(sock);
            return -1;
        }
    }
    return sock;
}

int send_datagram(int sock, unsigned remote_addr, unsigned short remote_port, void* buf, unsigned len) {
    struct sockaddr_in dst;
    memset(&dst, 0, sizeof(dst));

    dst.sin_family      = AF_INET;
    dst.sin_addr.s_addr = htonl(remote_addr);
    dst.sin_port        = htons(remote_port);
    return sendto(sock, buf, len, 0, (struct sockaddr*)&dst, sizeof(dst));
}

int recv_datagram(int sock, unsigned* remote_addr, unsigned short* remote_port, void* buf, unsigned len) {
    struct sockaddr_in src;
    unsigned addr_len = sizeof(src);
    int bytes_read = recvfrom(sock, buf, len, 0, (struct sockaddr*)&src, &addr_len);
    if(bytes_read > 0) {
        *remote_addr = src.sin_addr.s_addr;
        *remote_port = src.sin_port;
    }
    return bytes_read;
}

int get_our_ip(char* outip)
{
    int i=0;
    int sockfd;
    struct ifconf ifconf;
    char buf[512];
    struct ifreq *ifreq;
    char* ip;
    ifconf.ifc_len = 512;
    ifconf.ifc_buf = buf;
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0))<0)
    {
        return -1;
    }
    if(ioctl(sockfd, SIOCGIFCONF, &ifconf)<0) {
    	fprintf(stderr, "failed to get IFCONF!\n");
    }
    close(sockfd);
    ifreq = (struct ifreq*)buf;
    for(i=(ifconf.ifc_len/sizeof(struct ifreq)); i>0;i--)
    {
        ip = inet_ntoa(((struct sockaddr_in*)&(ifreq->ifr_addr))->sin_addr);
        if(strcmp(ip,"127.0.0.1")==0)
        {
            ifreq++;
            continue;
        }
        strcpy(outip,ip);
        return 0;
    }
    return -1;
}


