#ifndef __AMBASTREAM_H__
#define __AMBASTREAM_H__


#ifdef  __cplusplus
extern "C" {
#endif

#define true 1
#define false 0
#define STREAMER_MGR_MAX_ACTIVE_LIVE_STREAM_GROUP 4
#define STREAMER_MGR_MAX_STREAM_NAME_LENGTH 64
#define STREAMER_MGR_MAX_STREAM_PER_GROUP 100
#define STREAMER_MGR_MAX_STREAMERS 100
#define MAX_MEDIA_SUBSESSIONS STREAMER_MGR_MAX_STREAM_PER_GROUP

typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

struct AmbaStream_Streamer_s;

typedef struct AmbaStreamer_MediaList_s {
    unsigned int mediaId;
    struct AmbaStream_Streamer_s* stm;
} AmbaStreamer_MediaList_t;

typedef struct AmbaStream_frameinfo_s
{
    unsigned int frame_num;
    unsigned int pts;
    unsigned int pic_type;
    unsigned long long start_addr;
    unsigned int size;
    unsigned long long base_addr;
    unsigned long long limit_addr;
    unsigned int mark;
} AmbaStream_frameinfo_t;

typedef void (*frame_ready_cb)(AmbaStream_frameinfo_t* framedata, void* ctx);
typedef int (*buf_ready_cb)(void* ctx);

#define AMBASTREAM_IGNORE (-255)

enum ambastream_pb_op_e {
    AMBASTREAM_PB_OPEN = 1,
    AMBASTREAM_PB_PLAY,
    AMBASTREAM_PB_STOP,
    AMBASTREAM_LIVE_PLAY,
    AMBASTREAM_LIVE_STOP,
    AMBASTREAM_PB_RESET,
    AMBASTREAM_PB_PAUSE,
    AMBASTREAM_PB_RESUME,
    AMBASTREAM_PB_CONFIG,
    AMBASTREAM_GET_VID_FR,
    AMBASTREAM_GET_VID_TICK,
    AMBASTREAM_GET_AUD_FR,
    AMBASTREAM_GET_DURATION,
    AMBASTREAM_SET_LIVE_BITRATE,
    AMBASTREAM_GET_LIVE_BITRATE,
    AMBASTREAM_GET_LIVE_AVG_BITRATE,
    AMBASTREAM_SET_NET_BANDWIDTH,
    AMBASTREAM_SEND_RR_STAT,
    AMBASTREAM_SET_PARAMETER,
    AMBASTREAM_GET_SPS_PPS,
    AMBASTREAM_GSENSOR_PLAY,
    AMBASTREAM_GSENSOR_STOP,
};


#define AMBA_MEDIA_ID_A_MASK   0x20
#define AMBA_MEDIA_ID_T_MASK   0x40

typedef enum {
    AMP_FORMAT_MID_H264 = 0x01,                             /**< H264 media id */
    AMP_FORMAT_MID_AVC = 0x02,                              /**< AVC media id */
    AMP_FORMAT_MID_MJPEG = 0x03,                            /**< MJPEG media id */
    AMP_FORMAT_MID_H265 = 0x04,                             /**< H265 media id */
    AMP_FORMAT_MID_HVC = 0x05,                              /**< HVC media id */
    AMP_FORMAT_MID_AAC = (AMBA_MEDIA_ID_A_MASK | 0x01),    /**< AAC media id */
    AMP_FORMAT_MID_PCM = (AMBA_MEDIA_ID_A_MASK | 0x02),    /**< PCM media id */
    AMP_FORMAT_MID_ADPCM = (AMBA_MEDIA_ID_A_MASK | 0x03),  /**< ADPCM media id */
    AMP_FORMAT_MID_MP3 = (AMBA_MEDIA_ID_A_MASK | 0x04),    /**< MP3 media id */
    AMP_FORMAT_MID_AC3 = (AMBA_MEDIA_ID_A_MASK | 0x05),    /**< AC3 media id */
    AMP_FORMAT_MID_WMA = (AMBA_MEDIA_ID_A_MASK | 0x06),    /**< WMA media id */
    AMP_FORMAT_MID_OPUS = (AMBA_MEDIA_ID_A_MASK | 0x07),   /**< OPUS media id */
    AMP_FORMAT_MID_TEXT = (AMBA_MEDIA_ID_T_MASK | 0x01),   /**< Text media id */
    AMP_FORMAT_MID_MP4S = (AMBA_MEDIA_ID_T_MASK | 0x02)    /**< MP4S media id */
} AMBA_MEDIA_ID_e;


typedef struct RR_report_s
{
    unsigned int fr_lost;
    unsigned int jitter;
    double pg_delay;
} RR_report_t;

//========[ Module control ]========

/**
 * init AmbaStream.
 *
 * @return  0 - success
 *	    1 - Video encode does not start yet
 *         <0 - fail
 */
int AmbaStream_init(void);

/**
 * release AmbaStream.
 *
 * @return  0 - success
 *         <0 - fail
 */
int AmbaStream_release(void);

/**
 * enable specified media stream.
 *
 * [in] stm, streamer to be enabled. (refer to AmbaStream_Streamer_s)
 *
 * @return  0 - success
 *         <0 - fail
 */
int AmbaStreamer_enable(struct AmbaStream_Streamer_s* stm);

/**
 * disable specified media stream.
 *
 * [in] stm, streamer to be disabled. (refer to AmbaStream_Streamer_s)
 *
 * @return  0 - success
 *         <0 - fail
 */
int AmbaStreamer_disable(struct AmbaStream_Streamer_s* stm);

/**
 * register callback for media frame ready.
 *
 * [in] stm, the streamer which callback will hook to. (refer to AmbaStream_Streamer_s)
 * [in] cb_func, callback function
 * [in] ctx, user specific data
 *
 * @return  0 - success
 *         <0 - fail
 */
int AmbaStreamer_RegisterFrameReadyCallback(struct AmbaStream_Streamer_s* stm, frame_ready_cb cb_func, void* ctx);

/**
 * register callback for playback buffer ready.
 *
 * [in] stm, the streamer which callback will hook to. (refer to AmbaStream_Streamer_s)
 * [in] cb_func, callback function
 * [in] ctx, user specific data
 *
 * @return  0 - success
 *         <0 - fail
 */
int AmbaStreamer_RegisterBufReadyCallback(struct AmbaStream_Streamer_s* stm, buf_ready_cb cb_func, void* ctx);

void AmbaStream_stop_waiting_encode(void* streamer);

int AmbaStream_send_PbCmd(int type, void *value, void *hdlr, int bSize, void *result);

//========[ Utility ]========

/**
 * get SPS, PPS and ProfileLevelID.
 *
 * [in] stm, the streamer type to get these info.
 * [out] SPS, SPS info
 * [out] SPS_len, valid length for SPS info
 * [out] PPS, PPS info
 * [out] PPS_len, valid length for PPS info
 * [out] ProfileLevelID, ProfileLevelID
 *
 * @return  1 - success
 *          0 - SPS or PPS is not valid
 *	   <0 - fail
 */
int AmbaStreamer_GetSPSPPS(struct AmbaStream_Streamer_s* stm, u8 *SPS, int *SPS_len, u8 *PPS, int *PPS_len, unsigned int *ProfileLevelID);
int AmbaStreamer_GetVPS(struct AmbaStream_Streamer_s* stm, u8 *VPS, int *VPS_len);

/**
 * get gop(Group of Picture) information.
 *
 * [in] stm, the streamer to get these info.
 * [out] gopM, distance between P frame
 * [out] gopN, distance between I frame
 * [out] gopSize,  distance between Idr frame
 * [out] IRCycle, intra refresh cycle, only valid when gopN=255
 * [out] recovery_frame_cnt, recovery_frame_cnt value of SEI recovery point
 *
 * @return  1 - success
 *          0 - GOP info is not valid
 *	   <0 - fail
 */
int AmbaStreamer_GetGOPInfo(struct AmbaStream_Streamer_s* stm, unsigned short* gopM, unsigned short* gopN,
        unsigned short* gopSize, unsigned short* IRCycle, unsigned short* recovery_frame_cnt);

/**
 * get Audio config.
 *
 * [in]  stm, what streamer's audio config to retrieve
 * [out] format, audio format
 * [out] SampleRate, audio sample rate
 * [out] channels, audo channels
 *
 * @return  0 - success
 *         <0 - fail
 */
int AmbaStreamer_GetAudioConf(struct AmbaStream_Streamer_s* stm, int *format, int *SampleRate, int *channels);

/**
 * publish RTSP server Link.
 *
 * [in] s_link, the string to indicate RTSP server link
 *
 * @return  0 - success
 *         <0 - fail
 */
int AmbaStream_set_ServerLink(char *s_link);

/**
 * retrieve reference time.
 *
 * [out] ref_time, reference time for RTSP flow
 *
 * @return  0 - success
 *         <0 - fail
 */
int AmbaStream_get_itrontime(double *ref_time);

/**
 * send receiver report statistic to RTOS.
 *
 * [in] stm, which streamer this report correspond to
 * [in] fr_lost, frame loss info
 * [in] jitter, jitter info
 * [in] pg_delay, package delay info
 *
 * @return  0 - success
 *         <0 - fail
 */
int AmbaStream_send_RRstat(struct AmbaStream_Streamer_s* stm, unsigned int fr_lost, unsigned int jitter, double pg_delay);

/**
 * get Text config.
 *
 * [in]  stm, what streamer's text config to retrieve
 * [out] TimeScale, frequency of the time below
 * [out] TimePerFrame, how many time units this frame represent
 *
 * @return  0 - success
 *         <0 - fail
 */
int AmbaStreamer_GetTextConf(struct AmbaStream_Streamer_s* stm, unsigned int *TimeScale, unsigned int *TimePerFrame);

/**
 * Check whether this streamer is liveview.
 *
 * [in]  stm, streamer pointer
 *
 * @return  1 - ture
 *          0 - false
 */
int AmbaStreamer_IsLive(struct AmbaStream_Streamer_s*);

/**
 * Get the streamer's track type
 *
 * [in]  stm, streamer pointer
 *
 * @return  1 - ture
 *          0 - false
 */
int AmbaStreamer_GetTrackType(struct AmbaStream_Streamer_s* stm);

/**
 * Get the streamer's correspond hCodec
 *
 * [in]  stm, streamer pointer
 *
 * @return  1 - ture
 *          0 - false
 */
void* AmbaStreamer_GetCodec(struct AmbaStream_Streamer_s* stm);

/**
 * retrieve Amba liveview stream's media ID
 *
 * [in] streamName, the stream name of the liveview
 * [out] list, a list of (mediaID:streamer) belongs to the same liveGroup
 *
 * @return  0 - success
 *         <0 - fail
 */
int AmbaStreamerLive_GetMediaID(const char* streamName, AmbaStreamer_MediaList_t* list);

void* AmbaStreamer_GetGSensorStreamer(void);

int AmbaStreamerLive_StartStream(void);

int AmbaStreamerLive_StopStream(void);

#ifdef  __cplusplus
}
#endif
#endif /* __AMBASTREAM_H__ */

