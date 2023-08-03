#ifndef __STREAM_SHARE_H__
#define __STREAM_SHARE_H__

int stream_share_get_info(unsigned long long *share_addr, unsigned long long *phy_addr, unsigned int *share_size);
int stream_share_set_enable(int index, int enable);
int stream_share_send_data(int index, unsigned char *data, unsigned int len, int extra_param);

#endif//__STREAM_SHARE_H__

