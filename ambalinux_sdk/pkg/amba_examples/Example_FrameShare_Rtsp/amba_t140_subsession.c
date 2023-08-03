#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h> /* sockaddr_in{} and other Internet defns */
#include <arpa/inet.h>  /* inet(3) functions */

#include "ev.h"
#include "socket_helper.h"
#include "text_frame_reader.h"
#include "media_subsession.h"
#include "ambastream.h"
#include "rtp.h"

#define RTP_UDP_SOCK_BUF_SIZE   0x100000    // 1MByte
#define RTP_TCP_SOCK_BUF_SIZE   0x800000
#define RTP_HEADER_SIZE 12
#define REF_CLOCK (1000)
#define DEFAULT_MTU         (1448-RTP_HEADER_SIZE)        // 1448 - 12

static unsigned int idlePeriod = 1;
static unsigned int TicksPerFrame = 0;
static unsigned int min_sr_period = REF_CLOCK * 5;

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
    if (len > DEFAULT_MTU) {
        fprintf(stderr,"Text Frame size (%d) too big (limit:%d)!!\n",len,DEFAULT_MTU);
        return -1;
    } else {
        priv->rtp_payload_info.buf = buf;     /*let the rtp use this space to reduce the copy*/
        priv->rtp_payload_info.len = len + RTP_HEADER_SIZE;
        priv->rtp_payload_info.timestamp = priv->cur_timestamp - priv->ts_offset;
        if (len == 0 && idlePeriod == 0) { //entering idle period
            idlePeriod = 1;
            priv->rtp_payload_info.maker = 0;
        } else if (len != 0 && idlePeriod == 1) {   //first frame after idle period
            priv->rtp_payload_info.maker = 1;
            idlePeriod = 0;
        } else {
            priv->rtp_payload_info.maker = 0;
        }

        priv->rtp->send_packet_special(priv->rtp, &priv->rtp_payload_info);
    }
    return 0;
}

static void read_avail_cb(struct ev_loop* loop, ev_io* w, int revent)
{
    media_subsession_t* subsession = (media_subsession_t*)w->data;
    media_subs_priv_t* priv =  (media_subs_priv_t*)subsession->priv;;
    unsigned char what = '0';
    unsigned char stream_restart = 0;
    unsigned int tmp_ts;
    int bytes_read = read(w->fd, &what, sizeof(what));

    if(bytes_read < sizeof(what)) {
        fprintf(stderr, "error when read the pipe %s:%s\n", __FILE__,__FUNCTION__);
        return;
    }

    if(what == 'd') {
        // have data
        frame_info_t *cur_frame_info = &priv->frame;

        if(priv->frame_reader->get_next_frame_meta_nonblock(priv->frame_reader, cur_frame_info) != 0) {
            fprintf(stderr, "error when read frame %s:%s\n", __FILE__,__FUNCTION__);
            return;
        }

        if(cur_frame_info->pts==0){
            stream_restart = 1;
        }

        if(priv->en_rtcp){
            if(priv->total_read_frames==0) {
                priv->next_sr_pts = cur_frame_info->pts + min_sr_period;
                stream_restart = 1;
            } else {
                if(((priv->last_pts >= priv->next_sr_pts)
                    && (priv->last_pts-priv->next_sr_pts<0xf0000000))
                    || (stream_restart)) {
                    priv->rtp->need_to_send_sr_sdes = 1;
                }
            }
        } else {
            if(priv->total_read_frames==0) {
                stream_restart = 1;
            }
        }

        if(stream_restart) {
            tmp_ts = priv->cur_timestamp;
            priv->cur_timestamp = priv->rtp->curr_timestamp(priv->rtp,0);
            priv->ts_offset = 0;
            idlePeriod = 1;
            fprintf(stderr, "text stream restarted: ori_ts=%u, new_ts=%u\n",
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

        if(priv->rtp->need_to_send_sr_sdes==1){
            priv->next_sr_pts = priv->last_pts + min_sr_period;
        }
        parse_with_13bytes_ahead(subsession, cur_frame_info->addr_info.base,
                cur_frame_info->addr_info.len);

        priv->total_read_frames++;
    } else if(what == 'e'){
    // EOF
        fprintf(stderr, "EOS\n");
        priv->rtp->goodbye(priv->rtp);
    }
}

static void read_avail_pb_cb(struct ev_loop* loop, ev_io* w, int revent)
{
    media_subsession_t* subsession = (media_subsession_t*)w->data;
    media_subs_priv_t* priv =  (media_subs_priv_t*)subsession->priv;;
    unsigned char what = '0';
    unsigned char stream_restart = 0;
    unsigned int tmp_ts;
    int bytes_read = 0;

    if (priv->total_read_pb_frames != 0) { //not 1st frame
        int ts_diff = 0;
        tmp_ts = priv->rtp->curr_timestamp(priv->rtp,priv->cur_timestamp);
        if(tmp_ts < priv->cur_timestamp){ //TS overflow
            ts_diff = tmp_ts + (0xffffffff - priv->cur_timestamp) + 1;
        } else {
            ts_diff = tmp_ts - priv->cur_timestamp;
        }

        if((ts_diff < 0) || (ts_diff < TicksPerFrame*4/5)){ //not yet.
            //printf("%s: Frame too early! ts_diff=%d, TicksPerFrame=%d\n",__FUNCTION__,ts_diff,TicksPerFrame);
            usleep( (TicksPerFrame-ts_diff)/(subsession->ref_clock/1000) *100 );
            return;
        }
    }

    bytes_read = read(w->fd, &what, sizeof(what));
    if(bytes_read < sizeof(what)) {
        fprintf(stderr, "error when read the pipe %s:%s\n", __FILE__,__FUNCTION__);
        return;
    }

    if(what == 'd') {
        // have data
        frame_info_t *cur_frame_info = &priv->frame;

        if(priv->frame_reader->get_next_frame_meta_nonblock(priv->frame_reader, cur_frame_info) != 0) {
            fprintf(stderr, "error when read frame %s:%s\n", __FILE__,__FUNCTION__);
            return;
        }

        if(cur_frame_info->pts==0){
            stream_restart = 1;
        }

        if(priv->en_rtcp){
            if(priv->total_read_pb_frames==0) {
                priv->next_sr_pts = cur_frame_info->pts + min_sr_period;
                stream_restart = 1;
            } else {
                if(((priv->last_pts >= priv->next_sr_pts)
                    && (priv->last_pts-priv->next_sr_pts<0xf0000000)) ||
                    (stream_restart)) {
                    priv->rtp->need_to_send_sr_sdes = 1;
                }
            }
        } else {
            if(priv->total_read_pb_frames==0) {
                stream_restart = 1;
            }
        }

        if(stream_restart) {
            unsigned int tmp_ts;
            tmp_ts = priv->cur_timestamp;
            priv->cur_timestamp = priv->rtp->curr_timestamp(priv->rtp,0);
            priv->ts_offset = priv->cur_timestamp - priv->rtp->get_start_ts(priv->rtp);
            fprintf(stderr, "text stream restarted: ori_ts=%u, new_ts=%u\n",
                    tmp_ts,priv->cur_timestamp);
        } else {
            priv->cur_timestamp += TicksPerFrame;
        }
        priv->last_pts = cur_frame_info->pts;

        if(priv->rtp->need_to_send_sr_sdes==1){
            priv->next_sr_pts = priv->last_pts + min_sr_period;
        }
        //fprintf(stderr, "video TS: %u\n",priv->cur_timestamp);
        parse_with_13bytes_ahead(subsession, cur_frame_info->addr_info.base,
                cur_frame_info->addr_info.len);

        priv->total_read_pb_frames++;
    } else if(what == 'e'){
    // EOF
        fprintf(stderr, "EOS\n");
        priv->rtp->goodbye(priv->rtp);
    }
}

static const char* get_aux_sdp(struct media_subsession_s* thiz)
{
   return NULL;
}

static const char* get_sdp_lines(struct media_subsession_s* thiz)
{
    media_subs_priv_t* priv = NULL;
    unsigned int timeScale = 0, timePerFrame = 0, ret;

    if(thiz == NULL) {
        fprintf(stderr, "%s: thiz == NULL\n", __FUNCTION__);
        return NULL;
    }
    priv = (media_subs_priv_t*)thiz->priv;

    ret = AmbaStreamer_GetTextConf(frame_reader_get_streamer(priv->frame_reader),
            &timeScale, &timePerFrame);
    if (ret == 0) {
        TicksPerFrame = timePerFrame * ((double)thiz->ref_clock/(double)timeScale);
    } else {
        fprintf(stderr, "get TextConfig fail\n");
    }
    do{
        char const* mediaType = "text";
        char const* sdp_server = "c=IN IP4 0.0.0.0\r\n";
        unsigned char rtpPayloadType = 96 + thiz->track_num - 1;
        char const* auxSDPLine = get_aux_sdp(thiz);

        if(auxSDPLine==NULL){
            fprintf(stderr,"get_sdp_lines: auxSDPLine == NULL\n");
        }

        char const* const sdpFmt =
            "m=%s %u RTP/AVP %d\r\n"
            "%s"
            "b=AS:%u\r\n"
            "a=rtpmap:%u t140/%u\r\n"
            "a=lang:en\r\n"
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
                1000, // b=AS:<bandwidth> (as kbps)
                rtpPayloadType, thiz->ref_clock,// a=rtpmap:... (if present)
                (auxSDPLine==NULL)? "":auxSDPLine, // optional extra SDP line
                thiz->get_track_id(thiz)); // a=control:<track-id>
    }while(0);

    return priv->sdp_lines;
}
/********************************************************************************
 * public function
 ********************************************************************************/
void amba_t140_subsession_release(media_subsession_t* thiz); // FW declaration
media_subsession_t* amba_t140_subsession_create(rtsp_server_t* srv, void* streamer,
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
                6980,
                RTP_HEADER_SIZE,
                streamer,
                read_avail_cb);
    } else {
        rval = media_subsession_init(media_sub, srv, en_rtcp,
                0, // DONT enable RR
                0, // DONT use dsp clock
                6982,
                RTP_HEADER_SIZE,
                streamer,
                read_avail_pb_cb);
    }
    if (rval < 0) {
        fprintf(stderr, "%s:%d, subsession init fail\n", __FUNCTION__, __LINE__);
        return NULL;
    }
    media_sub->get_sdp_lines = get_sdp_lines;
    media_sub->release = amba_t140_subsession_release;
    media_sub->ref_clock = REF_CLOCK;

    min_sr_period = REF_CLOCK * 5;

    return media_sub;
}

void amba_t140_subsession_release(media_subsession_t* thiz)
{
    if (thiz == NULL) {
        return;
    }
    media_subsession_deinit(thiz);
    free(thiz);
}
