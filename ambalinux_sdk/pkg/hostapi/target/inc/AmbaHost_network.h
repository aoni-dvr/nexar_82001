#ifndef __AMBA_HOST_NETWORK__
#define __AMBA_HOST_NETWORK__

#define HOSTAPI_SOCKET_PORT     6969

/* hXport is used to idenfiy a connection */
typedef void* hXport;

typedef void* (*thread_entry_t)(void*);

/* structure to be used by external programs to initialize hostapi listener port.
 * passed as an argument to @AmbaHost_xport_init() method below. */
struct xport_config {
    uint32_t port;
    uint32_t flags;
};

/*
 * Initialize the xport module.
 * Each time a connect request is received from a host machine, we open a
 * connection for the request and a new thread is created to handle the
 * data transfer.
 * The newly-created thread uses @conn_entry as entry point and an hXport
 * identifying the connection is passed to that function.
 */
int AmbaHost_xport_init(thread_entry_t conn_entry, struct xport_config *xport_args);

/*
 * Shut down the xport module
 */
int AmbaHost_xport_fini();

/*
 * Lock the connection so that no other thread can access the connection
 * This funnction is called before we want to send/recv multiple segments of
 * data that should be atomic.
 */
int AmbaHost_xport_lock(hXport port);

/*
 * unlock the connection
 */
int AmbaHost_xport_unlock(hXport port);

/*
 * Send a chunk of data to remote. Buffer is owned by caller.
 */
int AmbaHost_xport_send(hXport xport, const char *buff, uint32_t size);

/*
 * Receieve a chunk of data from remote. Buffer is owned by caller.
 */
int AmbaHost_xport_recv(hXport xport, char *buff, uint32_t size);

/*
 * skip a chunk of data from remote.
 */
int AmbaHost_xport_skip(hXport xport, uint32_t size);

#endif  //__AMBA_HOST_NETWORK__
