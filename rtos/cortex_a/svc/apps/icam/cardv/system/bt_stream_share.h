#ifndef __BT_STREAM_SHARE_H__
#define __BT_STREAM_SHARE_H__

int bt_imu_stream_get_share_info(unsigned long long *share_addr, unsigned long long *phy_addr, unsigned int *share_size);
int bt_gnss_stream_get_share_info(unsigned long long *share_addr, unsigned long long *phy_addr, unsigned int *share_size);
int bt_stream_share_set_imu_stream_enable(int enable);
int bt_stream_share_set_gnss_stream_enable(int enable);
int bt_imu_stream_send_message(unsigned short acc_x, unsigned short acc_y, unsigned short acc_z,
                            unsigned short gyro_x, unsigned short gyro_y, unsigned short gyro_z);
int bt_imu_stream_check_enable(void);
int bt_gnss_stream_check_enable(void);

#endif//__BT_STREAM_SHARE_H__

