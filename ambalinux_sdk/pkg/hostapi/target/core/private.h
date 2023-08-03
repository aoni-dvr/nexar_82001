#define MAX_CONNECTION_NUM      128

/* control block for an AmbaHost connection */
typedef struct _hccb {
	int     cid;
	hXport  xport;
} hccb;

hccb* get_hccb(int cid);
