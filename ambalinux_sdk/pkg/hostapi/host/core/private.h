#define MAX_CONNECTION_NUM      128

int tx_loopback(hXport xport, const char *msg);
int rx_loopback(hXport xport, AHMSG_HDR *header);

int tx_copyfrom(hXport xport, const char *dst, const char *src);
int rx_copyfrom(hXport xport, AHMSG_HDR *header);

int tx_copyto(hXport xport, const char *dst, const char *src);
int rx_copyto(hXport xport, AHMSG_HDR *header);

int tx_read(hXport xport, void *hAddr, uint32_t tpAddr, uint32_t size);
int rx_read(hXport xport, AHMSG_HDR *header);

int tx_write(hXport xport, void *hAddr, uint32_t tpAddr, uint32_t size);
int rx_write(hXport xport, AHMSG_HDR *header);
