#ifndef __REC_DVR_H__
#define __REC_DVR_H__

typedef enum _rec_status_e_ {
    REC_STATUS_IDLE = 0,
    REC_STATUS_RECORD,
    REC_STATUS_VF,
    REC_STATUS_BUSY,
    REC_STATUS_NUM
} rec_status_e;

typedef enum _CAMERA_CHANNEL_e_ {
    CAMERA_CHANNEL_INTERNAL = 0,
    CAMERA_CHANNEL_EXTERNAL,
    CAMERA_CHANNEL_BOTH,
    CAMERA_CHANNEL_NUM
} CAMERA_CHANNEL_e;

typedef struct _rec_channel_info_s_ {
    rec_status_e rec_status;
    unsigned int rec_time;
    rec_status_e preview_status;
} rec_channel_info_s;

typedef struct _rec_dvr_status_s_ {
    rec_channel_info_s channels[CAMERA_CHANNEL_BOTH];
} rec_dvr_status_s;

typedef int (*record_stop_cb_func)(void);

#define EVENT_RECORD_MAX_PRE_SECONDS (15)
#define EVENT_RECORD_DEFAULT_PRE_SECONDS (10)

extern rec_dvr_status_s rec_dvr_status;
extern void SvcResCfgTask_LoadSetting(void);
extern UINT32 SvcFlowControl_Exec(const char *pCmd);

int rec_dvr_init(void);
int rec_dvr_record_auto_start(void);
int rec_dvr_record_start(CAMERA_CHANNEL_e channel);
int rec_dvr_record_stop(CAMERA_CHANNEL_e channel, record_stop_cb_func func);
int rec_dvr_record_started_handler(unsigned char bits);
int rec_dvr_record_stopped_handler(unsigned char bits);
int rec_dvr_is_recording(CAMERA_CHANNEL_e channel);
int rec_dvr_is_event_recording(CAMERA_CHANNEL_e channel);
int rec_dvr_event_record_start(CAMERA_CHANNEL_e channel, unsigned int pre_seconds, unsigned int length);
int rec_dvr_event_record_stop(CAMERA_CHANNEL_e channel);
int rec_dvr_capture_piv(CAMERA_CHANNEL_e channel, int is_mjpg);
int rec_dvr_capture_piv_done(void);
int rec_dvr_check_piv_finish(void);
int rec_dvr_capture_thm(CAMERA_CHANNEL_e channel);
int rec_dvr_capture_thm_done(void);
int rec_dvr_update_thm(CAMERA_CHANNEL_e channel, const char *path);
CAMERA_CHANNEL_e rec_dvr_bits_to_channel(UINT32 Bits);
unsigned int rec_dvr_channel_to_bits(CAMERA_CHANNEL_e channel);
unsigned int rec_dvr_channel_to_piv_bits(CAMERA_CHANNEL_e channel);
unsigned int rec_dvr_channel_to_event_record_bits(CAMERA_CHANNEL_e channel);
unsigned int rec_dvr_get_channel_second_stream_zone_id(CAMERA_CHANNEL_e channel);
CAMERA_CHANNEL_e rec_dvr_stream_id_to_channel(UINT32 Id);
rec_status_e rec_dvr_get_rec_status(CAMERA_CHANNEL_e channel);
rec_status_e rec_dvr_get_preview_status(CAMERA_CHANNEL_e channel);
const char *rec_dvr_rec_status_2_str(rec_status_e status);
unsigned int rec_dvr_get_rec_time(CAMERA_CHANNEL_e channel);
int rec_dvr_get_event_pre_record_buffer_length(void);
int rec_dvr_check_card_status(void);
int rec_dvr_hot_config(void);
int rec_dvr_hot_config_busy_check(void);
int rec_dvr_camera_record_control(CAMERA_CHANNEL_e channel, int record);
int rec_dvr_switch_black_white_mode(CAMERA_CHANNEL_e channel, int on);

#endif//__REC_H__

