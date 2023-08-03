#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include <netinet/in.h> /* sockaddr_in{} and other Internet defns */
#include <arpa/inet.h>  /* inet(3) functions */

#include "ev.h"
#include "rtp.h"
#include "media_subsession.h"
#include "audio_frame_reader.h"
#include "base64.h"
#include "socket_helper.h"
#include "ambastream.h"

#ifdef _DEBUG
#define DEBUG(format, args...) printf("[%s:%d] "format, __FILE__, __LINE__, ##args)
#else
#define DEBUG(args...)
#endif

//#define SKIP_SEI
#define RTP_UDP_SOCK_BUF_SIZE   0x100000    // 1MByte
#define RTP_TCP_SOCK_BUF_SIZE   0x800000
#define MAX_FRAME_SIZE      0x80000     // 512KByte, 12bytes + frame data
#define RTP_HEADER_SIZE     12
#define DEFAULT_MTU         (1448-RTP_HEADER_SIZE)        // 1448 - 12

static unsigned int sampling_frequency = 0;
static unsigned char channel_configuration = 2;
static unsigned int min_sr_period = 0;
static unsigned int nextFrameTick = 0;

#if 0
static void dump_mem(unsigned char* addr, unsigned len)
{
    int i = 0, j = 0;
    int row = 0;
    int remain = 0;
    int mod = 16;

    if(addr == NULL || len == 0) {
        return;
    }
    row = len/mod;
    remain = len%mod;
    for(i = 0; i < row; i++) {
        for(j = 0; j < mod; j++) {
            fprintf(stderr, "%02x ", addr[i*mod + j]);
        }
        fprintf(stderr, "\n");
    }

    if(remain > 0) {
        for(i = 0; i < remain; i++) {
            fprintf(stderr, "%02x ", addr[row*mod + i]);
        }
        fprintf(stderr, "\n");
    }
}
#endif

/*******************************************************************
  * parse the input data, the data is frame based. That means every input is a whole frame.
  * the data should have 13 bytes reserved before the real nalu data
  * that 13 bytes are reserved for rtp packet
  * the nalu data should start with 0x00000001, ambarella stream is always start with this
  *
  * buf: point to the start point including the 13bytes ahead.
  * len: the frame data length
  *******************************************************************/
static int parse_with_13bytes_ahead( media_subsession_t* thiz,
                    unsigned char* buf,
                    unsigned len)
{
    media_subs_priv_t* priv = (media_subs_priv_t*)thiz->priv;

    if(len > DEFAULT_MTU) {
        fprintf(stderr,"Audio Frame size (%d) too big (limit:%d)!!\n",len,DEFAULT_MTU);
        return -1;
    } else {
        /*let the rtp use this space to reduce the copy*/
        priv->rtp_payload_info.buf = buf;
        priv->rtp_payload_info.len = len + RTP_HEADER_SIZE;
        priv->rtp_payload_info.timestamp = priv->cur_timestamp - priv->ts_offset;
        priv->rtp_payload_info.maker = 1;

        /*fprintf(stderr,"Audio Frame(%d/%02x%02x):\n",len,last_start[2],last_start[3]);
        if(len > 400){
            dump_mem(priv->rtp_payload_info.buf, 64);
            fprintf(stderr,"\n");
        }*/
        priv->rtp->send_packet_special(priv->rtp, &priv->rtp_payload_info);
    }

    return 0;
}

/*
 * retrieve frame from framed_buf and send it out in rtp-packet form
 */
static void frame_to_rtp(media_subsession_t* thiz)
{
    unsigned char stream_restart = 0;
    unsigned int tmp_ts;
    media_subs_priv_t* priv = (media_subs_priv_t*)thiz->priv;
    frame_info_t *cur_frame_info = &priv->frame;

    if(priv->frame_reader->
            get_next_frame_meta_nonblock(priv->frame_reader, cur_frame_info) != 0) {
        fprintf(stderr, "error when read frame %s:%s\n", __FILE__,__FUNCTION__);
        return;
    }

    //fprintf(stderr, "afno=%u, apts=%u\n",cur_frame_info->frame_num,cur_frame_info->pts);
    if(cur_frame_info->pts==0){
        stream_restart = 1;
    }
    if(cur_frame_info->addr_info.len > 0) {
        if(priv->en_rtcp){
            if(priv->total_read_frames==0){
                priv->next_sr_pts = cur_frame_info->pts + min_sr_period;
                stream_restart=1;
            } else if((priv->last_pts>= priv->next_sr_pts)
                    && (priv->last_pts - priv->next_sr_pts < 0xf0000000)) {
                priv->rtp->need_to_send_sr_sdes = 1;
                priv->total_read_frames = 0;
            }
        } else {
            if(priv->total_read_frames==0) {
                stream_restart = 1;
            }
        }

        if(stream_restart) {
            tmp_ts=priv->cur_timestamp;
            priv->cur_timestamp = priv->rtp->curr_timestamp(priv->rtp,priv->cur_timestamp);
            priv->ts_offset = 0;
            fprintf(stderr, "audio stream restarted: ori_ts=%u, new_ts=%u\n",
                    tmp_ts,priv->cur_timestamp);
        } else {
            tmp_ts = cur_frame_info->pts&0xffffffff;
            if(tmp_ts < priv->last_pts){
                tmp_ts = tmp_ts + (0xffffffff - priv->last_pts)+1;
            } else {
                tmp_ts = tmp_ts - priv->last_pts;
            }
            priv->cur_timestamp += tmp_ts;
        }
        priv->last_pts = cur_frame_info->pts;
        DEBUG("audio TS: %u\n",priv->cur_timestamp);

        if(priv->rtp->need_to_send_sr_sdes==1){
            priv->next_sr_pts = priv->last_pts + min_sr_period;
        }

        parse_with_13bytes_ahead(thiz, cur_frame_info->addr_info.base,
                cur_frame_info->addr_info.len);
        // increase the timestamp after send this frame
        priv->total_read_frames++;
    }
}

static void read_avail_cb(struct ev_loop* loop, ev_io* w, int revent) {
    media_subsession_t* subsession = (media_subsession_t*)w->data;
    media_subs_priv_t* priv =  (media_subs_priv_t*)subsession->priv;;

    unsigned char what = '0';
    int bytes_read = read(w->fd, &what, sizeof(what));

    if(bytes_read < sizeof(what)) {
        fprintf(stderr, "error when read the pipe %s:%s\n", __FILE__,__FUNCTION__);
        return;
    }

    if(what == 'd') {
        // have data, encap it to rtp format and send it out
        frame_to_rtp(subsession);
    } else if(what == 'e'){
        // EOF
        fprintf(stderr, "EOS\n");
        priv->rtp->goodbye(priv->rtp);
    }
}

static void read_avail_pb_cb(struct ev_loop* loop, ev_io* w, int revent) {
    media_subsession_t* subsession = (media_subsession_t*)w->data;
    media_subs_priv_t* priv =  (media_subs_priv_t*)subsession->priv;;

    unsigned char what = '0';
    unsigned int tmp_ts;
    int bytes_read;
    int ret;
    audio_frame_reader_t* frame_reader = (audio_frame_reader_t*) priv->frame_reader;
    frame_info_t *cur_frame_info = &priv->frame;

    if (priv->total_read_frames != 0) { //not 1st frame
        int ts_diff = 0;
        tmp_ts = priv->rtp->curr_timestamp(priv->rtp,priv->cur_timestamp);
        if(tmp_ts < priv->cur_timestamp){ //TS overflow
            ts_diff = tmp_ts + (0xffffffff - priv->cur_timestamp) + 1;
        } else {
            ts_diff = tmp_ts - priv->cur_timestamp;
        }

        if(ts_diff < 0) { //not yet
            DEBUG("%s: Frame too early! ts_diff=%d, nextFrameTick=%d\n",
                    __FUNCTION__,ts_diff, nextFrameTick);
            usleep( (nextFrameTick - ts_diff)/(subsession->ref_clock/1000) *200 );
            return;
        } else if (ts_diff < nextFrameTick*4/5) { //not yet.
            DEBUG("%s: Frame too early! ts_diff=%d, nextFrameTick=%d\n",
                    __FUNCTION__,ts_diff, nextFrameTick);
            usleep( (nextFrameTick)/(subsession->ref_clock/1000) *200 );
            return;
        }
    }
    /*
     * It's possible that we didn't get nextFrameTick last time(nextFrameTick=0)
     * if what = 'e', okay, send EOS
     * if what = 'd', anyway, we send it out (next ts_diff<0)
     */
    bytes_read = read(w->fd, &what, sizeof(what));
    if(bytes_read < sizeof(what)) {
        fprintf(stderr, "error when read the pipe %s:%s\n", __FILE__,__FUNCTION__);
        return;
    }

    if(what == 'd') {
        // have data, encap it to rtp format and send it out
        frame_to_rtp(subsession);

        // peek next frame's pts, opus frame lenght might be variable
        ret = frame_reader->peek_next_frame_meta(frame_reader, cur_frame_info);
        if(ret >= 0){
            nextFrameTick = cur_frame_info->pts - priv->last_pts;
            DEBUG("peek next pts = %lu, cur pts(%d) -> ntTick = %d\n",
                    cur_frame_info->pts, priv->last_pts, nextFrameTick);
        } else if(ret == -EAGAIN) { // no next frame
            nextFrameTick = 0;
            DEBUG("peek next frame, No next frame\n");
            return;
        } else {
            fprintf(stderr, "error when peek frame %s:%s\n", __FILE__,__FUNCTION__);
            return;
        }
    } else if(what == 'e'){
        // EOF
        fprintf(stderr, "EOS\n");
        priv->rtp->goodbye(priv->rtp);
    }
}

static unsigned const samplingFrequencyTable[16] = {
  96000, 88200, 64000, 48000,
  44100, 32000, 24000, 22050,
  16000, 12000, 11025, 8000,
  7350, 0, 0, 0
};

static const char* get_aux_sdp(struct media_subsession_s* thiz)
{
    media_subs_priv_t* priv = NULL;
    int format=0,samplerate=0,channel=0;
    unsigned char sampling_frequency_index;
    char const* fmtpFmt =
        "a=fmtp:%d "
        "sprop-maxcapturerate=%d;sprop-stereo=%d\r\n";

    if(thiz == NULL) {
        fprintf(stderr,"audio get_aux_sdp: thiz == NULL\n");
        return NULL;
    }
    priv = (media_subs_priv_t*)thiz->priv;

    if (priv->aux_sdp_line != NULL) return priv->aux_sdp_line;

    if(((audio_frame_reader_t*)priv->frame_reader)->get_audio_config(
                (audio_frame_reader_t*)priv->frame_reader, &format, &samplerate, &channel)<0){
        printf("Fail to AmbaStream_get_AudioConf()! Terminate Audio!!\n");
        return NULL;
    }

    for(sampling_frequency_index=0; sampling_frequency_index<16; sampling_frequency_index++){
        if(samplingFrequencyTable[sampling_frequency_index]==(unsigned)samplerate){
            sampling_frequency = (unsigned)samplerate;
            min_sr_period = samplerate * 5;
            break;
        }
    }

    if(sampling_frequency_index >= 16){ //cannot found related index
        printf("Bad 'sampling_frequency_index' in first frame of ADTS file\n");
        return NULL;
    }

    if(channel == 0){
        channel_configuration = 2;
    } else {
        channel_configuration = channel;
    }

    unsigned fmtpFmtSize = strlen(fmtpFmt)
        + 6 /* max digit len */
        + 6 /* max digit len */;
    unsigned spropstereo = channel_configuration == 2 ? 1 : 0;

    priv->aux_sdp_line = (char*)malloc(fmtpFmtSize+1);       //new char[fmtpFmtSize];
    if(priv->aux_sdp_line == NULL) {
        fprintf(stderr, "Audio: can't get memory for aux sdp line\n");
        return NULL;
    }
    sprintf(priv->aux_sdp_line, fmtpFmt,
            96 + thiz->track_num - 1,
            sampling_frequency, spropstereo);

    return priv->aux_sdp_line;
}

static const char* get_sdp_lines(struct media_subsession_s* thiz)
{
    media_subs_priv_t* priv = NULL;

    if(thiz == NULL) {
        fprintf(stderr,"get_sdp_lines: thiz == NULL\n");
        return NULL;
    }
    priv = (media_subs_priv_t*)thiz->priv;

    /* Due to the video resolution might changed time by time, */
    /* the SPD should be updated ervery time.                  */
    if(priv->sdp_lines != NULL) {
        free(priv->sdp_lines);
        priv->sdp_lines = NULL;
    }

    if(1){
        char const* mediaType = "audio";
        char const* sdp_server = "c=IN IP4 0.0.0.0\r\n";
        unsigned char rtpPayloadType = 96 + thiz->track_num - 1;
        char const* auxSDPLine = get_aux_sdp(thiz);

        if(auxSDPLine==NULL){
            fprintf(stderr,"opus get_sdp_lines: auxSDPLine == NULL\n");
        }

        char const* const sdpFmt =
            "m=%s %u RTP/AVP %d\r\n"
            "%s"
            "b=AS:%u\r\n"
            "a=rtpmap:%u OPUS/%u/%u\r\n"
            "%s"
            "a=control:%s\r\n";

        unsigned sdpFmtSize = strlen(sdpFmt)
            + strlen(mediaType) + 5 /* max short len */ + 3 /* max char len */
            + strlen(sdp_server)
            + 20 /* max int len */ + 20 /* max int len */ + 3 /* max char len */
            + 3
            + ((auxSDPLine==NULL)? 0:strlen(auxSDPLine))
            + strlen(thiz->get_track_id(thiz));

        priv->sdp_lines = (char*)malloc(sdpFmtSize);

        if(priv->sdp_lines == NULL) return NULL;

        sprintf(priv->sdp_lines, sdpFmt,
            mediaType, // m= <media>
            0, // m= <port>
            rtpPayloadType, // m= <fmt list>
            (thiz->track_num>1)? "":sdp_server, // c= address
            500, // b=AS:<bandwidth> (as kbps)
            rtpPayloadType, 48000, 2,// a=rtpmap:... (if present)
            (auxSDPLine==NULL)? "":auxSDPLine, // optional extra SDP line
            thiz->get_track_id(thiz)); // a=control:<track-id>
    }

    thiz->ref_clock = sampling_frequency;

    return priv->sdp_lines;
}




/********************************************************************************
 * public function
 ********************************************************************************/
void amba_opus_subsession_release(media_subsession_t* thiz); //FW declaration

media_subsession_t* amba_opus_subsession_create(rtsp_server_t* srv, void* streamer,
        unsigned int en_rtcp, unsigned int playback)
{
    media_subsession_t* media_sub;
    int rval;

    media_sub = (media_subsession_t*)malloc(sizeof(media_subsession_t));
    if(media_sub == NULL) {
        printf("Error: %s,%d\n",__FUNCTION__,__LINE__);
        return NULL;
    }

    if (playback ==0) {
        rval = media_subsession_init(media_sub, srv, en_rtcp,
                0, // DONT enable RR
                1, // use dsp clock
                6974,
                RTP_HEADER_SIZE,
                streamer,
                read_avail_cb);
    } else {
        rval = media_subsession_init(media_sub, srv, en_rtcp,
                0, // DONT enable RR
                0, // DONT use dsp clock
                6976,
                RTP_HEADER_SIZE,
                streamer,
                read_avail_pb_cb);
    }
    if (rval < 0) {
        fprintf(stderr, "%s:%d, subsession init fail\n", __FUNCTION__, __LINE__);
        return NULL;
    }
    media_sub->get_sdp_lines = get_sdp_lines;
    media_sub->release = amba_opus_subsession_release;

    return media_sub;
}

void amba_opus_subsession_release(media_subsession_t* thiz)
{
    if (thiz == NULL) {
        return;
    }
    media_subsession_deinit(thiz);
    free(thiz);
}


