#include "AmbaHost_network.h"

#define MAX_CONNECTION_NUM      128
#define MAX_SHELL_NUM           8
#define GET_AND_CHECK_HCCB(cid) \
	hccb *handle = get_hccb(cid); \
	if (handle == NULL) \
		return  AMBA_HOST_ERR_INVALID_CID;

/* control block for an AmbaHost shell */
typedef struct _hscb {
	int             socket;
	int             sid;
	pthread_t       thread;
	printer_func    printer;
	int             pfd[2];
} hscb;

/* control block for an AmbaHost connection */
typedef struct _hccb {
	char            addr[64];       // connection address
	int             cid;            // connection ID
	hXport          xport;          // transport handle
	int             shell_socket;   // private shell for Amba_exec
        int             is_closing;     // flag to closing the connection
	hscb            shell[MAX_SHELL_NUM];
} hccb;

hccb* get_hccb(int cid);
