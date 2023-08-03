#ifndef __AMBA_HOST_NETWORK__
#define __AMBA_HOST_NETWORK__

#define HOSTAPI_SOCKET_PORT     6969

/* hXport is used to idenfiy a connection */
typedef void* hXport;

/*
 * Initialize the xport module.
 */
int AmbaHost_xport_init(uint32_t flags);

/*
 * Shut down the xport module
 */
int AmbaHost_xport_fini(void);

/*
 * Open a connection to a target
 * @pHandler:   pointer to receive the connection handle
 * @addr:       the IP address of the target
 */
int AmbaHost_xport_open(hXport *pHandle, const char *addr, uint32_t flags);

/*
 * Close a connection
 */
int AmbaHost_xport_close(hXport handle);

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

/*
 * Get next sequence number fo the connection
 */
int AmbaHost_xport_next_seqnum(hXport xport);

#endif  //__AMBA_HOST_NETWORK__
