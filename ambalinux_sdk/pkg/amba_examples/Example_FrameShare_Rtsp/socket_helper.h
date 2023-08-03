#ifndef _SOCKET_HELPER_H_
#define _SOCKET_HELPER_H_

#ifdef __cplusplus
extern "C" {
#endif

extern int make_socket_nonblocking(int sock);

extern int make_socket_blocking(int sock);

extern int increase_snd_buf(int sock, unsigned int request_size);
extern int increase_rcv_buf(int sock, unsigned int size);

extern int setup_stream_socket(unsigned int addr, unsigned short port, int nonblocking);
extern int setup_datagram_socket(unsigned int addr, unsigned short port, int nonblocking);
extern int send_datagram(int sock, unsigned remote_addr, unsigned short remote_port, void* buf, unsigned len);
extern int recv_datagram(int sock, unsigned* remote_addr, unsigned short* remote_port, void* buf, unsigned len);

extern int get_our_ip(char* outip);

extern int enable_tcp_nodelay(int sock);
extern int disable_tcp_nodelay(int sock);

#ifdef __cplusplus
}
#endif

#endif
