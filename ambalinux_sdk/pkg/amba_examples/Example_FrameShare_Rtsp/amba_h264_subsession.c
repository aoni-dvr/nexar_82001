#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h> /* sockaddr_in{} and other Internet defns */
#include <arpa/inet.h>  /* inet(3) functions */
#include <math.h>

#include "ev.h"
#include "media_subsession.h"
#include "rtp.h"
#include "video_frame_reader.h"
#include "base64.h"
#include "socket_helper.h"
#include "ambastream.h"

//#define SKIP_SEI
#define NALU_TYPE_SEI       6
#define RTP_UDP_SOCK_BUF_SIZE   0xE00000    // 14MByte
#define RTP_TCP_SOCK_BUF_SIZE   0x800000
#define MAX_FRAME_SIZE      0x80000     // 512KByte, 12bytes + frame data
#define RTP_HEADER_SIZE     12
#define RTP_FUA_HDR_LEN     13          //(RTP_HEADER_SIZE + 1)  add 1 for FUA
#define DEFAULT_MTU         1436        // 1448 -12
#define REF_CLOCK           90000

static unsigned int TicksPerFrame = 0;
static unsigned int changeTick = 0;
static unsigned int min_sr_period = 0;
static unsigned int enable_stap = 0; //STAP-A

typedef struct nalu_type_s {
    uint8_t type:5;
    uint8_t nri:2;  //NRI
    uint8_t f:1;    //forbidden_zero bit
} nalu_type_t;

// h264 subsession extra info
typedef struct extra_info_s {
    uint8_t sei_length;
    uint8_t* sei_recovery_point;
} extra_info_t;

#if 1
static void dump_mem(unsigned char* addr, unsigned len) {
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

static inline int insertRecoveryPoint(media_subsession_t* thiz) {
    media_subs_priv_t* priv = (media_subs_priv_t*) thiz->priv;
    extra_info_t* extra = (extra_info_t*) thiz->extra_info;
    if (extra->sei_length > 0) {
        priv->rtp_payload_info.buf = extra->sei_recovery_point;
        priv->rtp_payload_info.len = extra->sei_length;
        priv->rtp_payload_info.timestamp = priv->cur_timestamp - priv->ts_offset;
        priv->rtp_payload_info.maker = 0;
        priv->rtp->send_packet_special(priv->rtp, &priv->rtp_payload_info);
    }
    return 0;
}
/********************************************************
  * buf = (12 + 1) head space, follow real data
  * len = (12 + 1) + real data len
  *******************************************************/
static inline int fua(media_subsession_t* thiz,
                    unsigned char* buf, unsigned len,
                    unsigned mtu, unsigned marker) {
    media_subs_priv_t* priv = (media_subs_priv_t*)thiz->priv;
    unsigned int frag_len;
    unsigned remain = len - RTP_FUA_HDR_LEN;
    unsigned char* nalu = buf + RTP_FUA_HDR_LEN;
    unsigned char* payload = buf + RTP_HEADER_SIZE;
    unsigned char fua_indicator = (nalu[0]&0xe0) | 28;
    unsigned char fua_header = nalu[0] & 0x1f;
    unsigned char start = 1;
    //dump_mem(nalu, 32);
    //nalu++;     // have read the nalu header

    remain--;
    while(remain > 0) {
        payload[0] = fua_indicator;
        if(start == 1) {
            payload[1] = fua_header | (1 << 7);
            start = 0;
        } else {
            payload[1] = fua_header;
        }
        if(remain > mtu - 2) {
            frag_len = mtu - 2;
        } else {
            frag_len = remain;
            payload[1] = fua_header | (1 << 6);     //End
        }
        #if 1
        priv->rtp_payload_info.buf = payload - RTP_HEADER_SIZE;     /*let the rtp use this space to reduce the copy*/
        priv->rtp_payload_info.len = frag_len + 2 + RTP_HEADER_SIZE;
        #else
        priv->rtp_payload_info.buf = payload;     /*let the rtp use this space to reduce the copy*/
        priv->rtp_payload_info.len = frag_len + 2;
        #endif
        priv->rtp_payload_info.timestamp = priv->cur_timestamp - priv->ts_offset;
        priv->rtp_payload_info.maker = (remain <= frag_len && marker == 1) ? 1 : 0;
        priv->rtp->send_packet_special(priv->rtp, &priv->rtp_payload_info);

        payload += frag_len;
        remain -= frag_len;
        //nalu += frag_len;
    }

    return 0;
}

/*******************************************************************
  * parse the input data, the data is frame based. That means every input is a whole frame.
  * the data should have 13 bytes reserved before the real nalu data
  * that 13 bytes are reserved for rtp packet
  * the nalu data should start with 0x00000001, ambarella stream is always start with this
  *
  * buf: point to the start point including the 13bytes ahead.
  * len: the frame data length
  *******************************************************************/
#define MAX_NALS_IN_AP 8
#define STAP_UPDATE_INDICATOR(x,y) do{ \
    x.f |= y.f; \
    x.nri = x.nri > y.nri ? x.nri : y.nri; \
}while(0)

// The first element in ap_vec has STAP-header(ahead), we'll fill it when we send
// nal_start-3 :1st NAL in AP, we need to place AP-header(1byte) ahead of it, utilize start code space
// nal_start-2 :place NALU size (2bytes), utilize original 0x000001 start code space
#define append_ap(ap_vec, ap_num, ap_size, nalu_hdr, nal_start, nal_end, ap_indicator) do{ \
    unsigned short ma_nal_size = nal_end - nal_start;              \
    STAP_UPDATE_INDICATOR(ap_indicator, nalu_hdr); \
    if(ap_num == 0){                                               \
        ap_vec[ap_num].iov_base = nal_start -3;                    \
        ap_vec[ap_num].iov_len = 1 + ma_nal_size + 2;              \
        memcpy((char*) ap_vec[ap_num].iov_base + 1, (char*)&ma_nal_size + 1, 1);    \
        memcpy((char*) ap_vec[ap_num].iov_base + 2, (char*)&ma_nal_size, 1);\
    } else {                                                              \
        ap_vec[ap_num].iov_base = nal_start - 2;                       \
        ap_vec[ap_num].iov_len = ma_nal_size + 2;               \
        memcpy(ap_vec[ap_num].iov_base, (char*)&ma_nal_size + 1, 1);    \
        memcpy((char*) ap_vec[ap_num].iov_base + 1, (char*)&ma_nal_size, 1);\
    }   \
    ap_size += ap_vec[ap_num].iov_len;                           \
    ap_num ++;                                                      \
}while(0)
#define send_ap(ap_vec, ap_num, ap_size, rtp_info, rtp, ap_indicator, marker) do{ \
    if(ap_num == 1) {                                           \
        rtp_info.buf = (unsigned char*)ap_vec[0].iov_base + 3 - RTP_HEADER_SIZE;     \
        rtp_info.len = ap_vec[0].iov_len - 3 + RTP_HEADER_SIZE;     \
        rtp_info.timestamp = priv->cur_timestamp - priv->ts_offset;  \
        rtp_info.maker = marker;                                  \
        rtp->send_packet_special(rtp, &rtp_info);                  \
    } else {                                                    \
        memcpy(ap_vec[0].iov_base, &ap_indicator, 1);                   \
        rtp_info.len = ap_size + RTP_HEADER_SIZE ; \
        rtp_info.timestamp = priv->cur_timestamp - priv->ts_offset; \
        rtp_info.maker = marker;                                    \
        rtp->send_packet_vector(rtp, &rtp_info, ap_vec, ap_num);    \
    } \
    ap_size = 0;                                                \
    ap_num = 0;                                                 \
}while(0)
// send AP if we have alrerady append pkt into it
#define check_and_send_ap(ap_vec, ap_num, ap_size, rtp_info, rtp, ap_indicator, marker) do{ \
    if(enable_stap == 1 && ap_size != 0) {                            \
        send_ap(ap_vec, ap_num, ap_size, rtp_info, rtp, ap_indicator, marker); \
    }                                                               \
}while(0)

static inline int skip_SEI(media_subsession_t* thiz)
{
#ifdef SKIP_SEI
    return 1;
#else
    extra_info_t* extra = (extra_info_t*) thiz->extra_info;
    if(extra->sei_length > 0){
        return 1;   //we need to remove uCode's SEI and insert our correct one
    } else {
        return 0;
    }
#endif
}
static int parse_with_13bytes_ahead( media_subsession_t* thiz,
                                            unsigned char* buf,
                                            unsigned len,
                                            unsigned mark) {
    media_subs_priv_t* priv = (media_subs_priv_t*)thiz->priv;
    unsigned char* data = buf + RTP_FUA_HDR_LEN;
    unsigned char* last_start;
    unsigned char* p;
    //unsigned char nalu_type = 0;    //always point to the last nalu we got
    nalu_type_t    nalu;        //always last nalu we got
    unsigned w;
    unsigned int *i_ptr;
    unsigned ap_num = 0;  // number of NALs in aggregation packet
    unsigned ap_size = 0; // total size of NALs in aggregation packet
    nalu_type_t    ap_indicator = { 24, 0 , 0};
    struct iovec iov[MAX_NALS_IN_AP];

    // in amba solution, the first 4 bytes must be 0x00000001
    i_ptr = (unsigned int*)data;
    if (*i_ptr != 0x01000000) {
        fprintf(stderr, "not amba format\n");
        dump_mem((unsigned char *)data,64);
        return -1;
    }
    // skip the start code
    last_start = data + 4;
    p = last_start;

    // it's a hack for amba solution, in bit stream from amba,
    // every frame only have one slice nalu,
    // the slice nalu alway be the last nalu.
    memcpy(&nalu, &last_start[0], 1);
    if( (nalu.type > 0 && nalu.type < 6) ){
        p = data + len - 4;
    }

    while (p < data + len - 4 ) {
        w = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
        if( w == 0x00000001) { //|| (w & 0xffffff00) == 0x00000100) {
            //get one nalu, p point to the next start
            do{
                if(nalu.type == NALU_TYPE_SEI && skip_SEI(thiz)>0 ) break;
                if(nalu.type == 0x1a) break; //omit AMBA proprietary GOP Header
                if(nalu.type == 0x09) break; //omit AU-delimiter
                if(p - last_start > DEFAULT_MTU) {
                    check_and_send_ap(iov, ap_num, ap_size, priv->rtp_payload_info, priv->rtp,
                        ap_indicator, 0);
                    fua(thiz, last_start - RTP_FUA_HDR_LEN,
                            p - last_start + RTP_FUA_HDR_LEN, DEFAULT_MTU, 0);
                } else if(enable_stap == 1
                        && ap_size+ p-last_start +2/*nal_size, 2 bytes*/ < DEFAULT_MTU
                        && ap_num + 1 <= MAX_NALS_IN_AP) { //append to STAP
                    append_ap(iov, ap_num, ap_size, nalu, last_start, p, ap_indicator);
                } else {
                    check_and_send_ap(iov, ap_num, ap_size, priv->rtp_payload_info, priv->rtp,
                        ap_indicator, 0);
                    if(enable_stap == 1 &&(1+2+p-last_start) < DEFAULT_MTU) {//begin next ap
                        append_ap(iov, ap_num, ap_size, nalu, last_start, p, ap_indicator);
                    } else {
                        priv->rtp_payload_info.buf = last_start - RTP_HEADER_SIZE;
                        priv->rtp_payload_info.len = p - last_start + RTP_HEADER_SIZE;
                        priv->rtp_payload_info.timestamp = priv->cur_timestamp - priv->ts_offset;
                        priv->rtp_payload_info.maker = 0;       //XXX
                        priv->rtp->send_packet_special(priv->rtp, &priv->rtp_payload_info);
                    }
                }
                // force to insert SEI recovery point after sps/pps
                // (workaround for uCode doesn't generate correct recovery_frame_cnt)
                // For now, do not support STAP mode
                if(enable_stap !=1 && nalu.type == 0x08) {
                    insertRecoveryPoint(thiz);
                }
            }while(0);
            if(p[3] == 0x01) {
                // skip 4, for next search
                p += 4;
            } else {
                p += 3;
            }
            last_start = p;
            memcpy(&nalu, &last_start[0], 1);
            if((nalu.type > 0 && nalu.type < 6)){
                p = data + len - 4;
            }

        } else if(p[3] > 1) {
            p += 4;
        } else if (p[2] > 1){
            p += 3;
        } else if (p[1] > 1){
            p += 2;
        } else {
            p++;
        }
    }
    //last nalu
    if(data + len - last_start > DEFAULT_MTU) {
        check_and_send_ap(iov, ap_num, ap_size, priv->rtp_payload_info, priv->rtp, ap_indicator, 0);
        fua(thiz, last_start - RTP_FUA_HDR_LEN,
                data + len - last_start + RTP_FUA_HDR_LEN, DEFAULT_MTU, mark);
    } else if(enable_stap == 1
            && (ap_size + data + len - last_start + 2/*nal_size, 2bytes*/) < DEFAULT_MTU
            &&  ap_num + 1 <= MAX_NALS_IN_AP) { //add into AP and send
        append_ap(iov, ap_num, ap_size, nalu, last_start, data + len, ap_indicator);
        send_ap(iov, ap_num, ap_size, priv->rtp_payload_info, priv->rtp, ap_indicator, mark);
    } else {
        check_and_send_ap(iov, ap_num, ap_size, priv->rtp_payload_info, priv->rtp, ap_indicator, 0);
        #if 1
        priv->rtp_payload_info.buf = last_start - RTP_HEADER_SIZE;
        priv->rtp_payload_info.len = data + len - last_start + RTP_HEADER_SIZE;
        #else
        priv->rtp_payload_info.buf = last_start;
        priv->rtp_payload_info.len = data + len - last_start;
        #endif
        priv->rtp_payload_info.timestamp = priv->cur_timestamp - priv->ts_offset;
        priv->rtp_payload_info.maker = mark;
        priv->rtp->send_packet_special(priv->rtp, &priv->rtp_payload_info);
    }

    return 0;
}

static void read_avail_cb(struct ev_loop* loop, ev_io* w, int revent) {
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

        if(priv->frame_reader->get_next_frame_meta_nonblock(priv->frame_reader, cur_frame_info)!=0){
            fprintf(stderr, "error when read frame %s:%s\n", __FILE__,__FUNCTION__);
            return;
        }

        if(cur_frame_info->pts==0){
            stream_restart = 1;
        }

        if(cur_frame_info->addr_info.len > 0) {
            if(priv->en_rtcp){
                if(priv->total_read_frames==0) {
                    priv->next_sr_pts = cur_frame_info->pts + min_sr_period;
                    stream_restart = 1;
                } else {
                    if(((priv->last_pts >= priv->next_sr_pts)
                        && (priv->last_pts-priv->next_sr_pts<0xf0000000))
                        ||(stream_restart)) {
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
                priv->rtp->reset_dyBR(priv->rtp);
                priv->cur_timestamp = priv->rtp->curr_timestamp(priv->rtp,0);
                priv->ts_offset = 0;
                //priv->ts_offset = priv->cur_timestamp - priv->rtp->get_start_ts(priv->rtp);
                fprintf(stderr, "stream restarted: ori_ts=%u, new_ts=%u\n",
                    tmp_ts,priv->cur_timestamp);
            } else {
                tmp_ts = cur_frame_info->pts&0xffffffff;
                if(tmp_ts < priv->last_pts){
                    tmp_ts = tmp_ts + (0xffffffff - priv->last_pts)+1;
                } else {
                    tmp_ts = tmp_ts - priv->last_pts;
                }
                priv->cur_timestamp += tmp_ts;

                //tmp_ts = priv->rtp->curr_timestamp(priv->rtp,0);
                //fprintf(stderr,"curr_ts=%u, tmp_ts=%u, diff=%d, finfo.pts=%lu\n",priv->cur_timestamp, tmp_ts, (int)(priv->cur_timestamp-tmp_ts),cur_frame_info->pts);

                //printf("cur_timestamp:%u, tmp_ts:%u, cur_frame->pts=%lu, priv->last_pts=%u\n",
                //  priv->cur_timestamp,tmp_ts,cur_frame_info->pts,priv->last_pts);
            }
            priv->last_pts = cur_frame_info->pts;

            if(priv->rtp->need_to_send_sr_sdes==1){
                priv->next_sr_pts = priv->last_pts + min_sr_period;
            }
            //fprintf(stderr, "video TS: %u\n",priv->cur_timestamp);
            parse_with_13bytes_ahead(subsession, cur_frame_info->addr_info.base,
                cur_frame_info->addr_info.len, cur_frame_info->mark);

            //priv->cur_timestamp += TicksPerFrame;
            priv->total_read_frames++;
        }
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

    what=0;
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
        //fprintf(stderr, "fno=%u, pts=%lu\n",cur_frame_info->frame_num,cur_frame_info->pts);

        if(cur_frame_info->addr_info.len > 0) {
            if(priv->en_rtcp){
                if(priv->total_read_pb_frames==0) {
                    priv->next_sr_pts = cur_frame_info->pts + min_sr_period;
                    stream_restart = 1;
                } else {
                    if(((priv->last_pts >= priv->next_sr_pts) && (priv->last_pts-priv->next_sr_pts<0xf0000000)) ||
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
                tmp_ts = priv->cur_timestamp;
                priv->cur_timestamp = priv->rtp->curr_timestamp(priv->rtp,priv->cur_timestamp);
                priv->ts_offset = priv->cur_timestamp - priv->rtp->get_start_ts(priv->rtp);
                changeTick = 0;
                fprintf(stderr, "stream restarted: ori_ts=%u, new_ts=%u\n",
                        tmp_ts,priv->cur_timestamp - priv->ts_offset);
            } else {
                if(changeTick == 1){
                    printf("pts=%lu, last_pts=%u, change ticksPerFrame to %lu\n",cur_frame_info->pts,
                        priv->last_pts, cur_frame_info->pts - priv->last_pts);
                    TicksPerFrame = cur_frame_info->pts - priv->last_pts;
                    changeTick = 0;
                }
                priv->cur_timestamp += TicksPerFrame;
            }
            if(cur_frame_info->pts - priv->last_pts != TicksPerFrame &&
                (cur_frame_info->pts != 0 || priv->last_pts != 0)){
                changeTick = 1;
            }
            //fprintf(stderr, "current frame pts %lu\n", cur_frame_info->pts);
            priv->last_pts = cur_frame_info->pts;

            if(priv->rtp->need_to_send_sr_sdes==1){
                priv->next_sr_pts = priv->last_pts + min_sr_period;
            }
            parse_with_13bytes_ahead(subsession, cur_frame_info->addr_info.base,
                    cur_frame_info->addr_info.len, cur_frame_info->mark);
            priv->total_read_pb_frames++;
        }
    } else if(what == 'e'){
        // EOF
        fprintf(stderr, "EOS\n");
        priv->rtp->goodbye(priv->rtp);
    }
}

static const char* get_aux_sdp(struct media_subsession_s* thiz) {
    media_subs_priv_t* priv = NULL;

    if(thiz == NULL) {
        fprintf(stderr,"get_aux_sdp: thiz == NULL\n");
        return NULL;
    }
    priv = (media_subs_priv_t*)thiz->priv;

    if (priv->aux_sdp_line != NULL) return priv->aux_sdp_line;

    unsigned char* sps = (unsigned char*)malloc(64);
    unsigned sps_len = 0;
    unsigned char* pps = (unsigned char*)malloc(32);
    unsigned pps_len = 0;
    unsigned profile_id = 0;
    memset(sps, 0, 64);
    memset(pps, 0, 32);

    int ret = ((video_frame_reader_t*)priv->frame_reader)->get_sps_pps(
                            (video_frame_reader_t*) priv->frame_reader,
                            sps, &sps_len,
                            pps, &pps_len,
                            &profile_id);
    //printf("ret %d, sps_len %d, pps_len %d\n", ret, sps_len, pps_len);

    if (ret != 1) return NULL; // our source isn't ready

    // Set up the "a=fmtp:" SDP line for this stream:
    char* sps_base64 = base64Encode((char*)sps, sps_len);
    char* pps_base64 = base64Encode((char*)pps, pps_len);
    char const* fmtpFmt =
        "a=fmtp:%d packetization-mode=1"
        ";profile-level-id=%06X"
        ";sprop-parameter-sets=%s,%s\r\n";
    unsigned fmtpFmtSize = strlen(fmtpFmt)
                + 3 /* max char len */
                + 6 /* 3 bytes in hex */
                + strlen(sps_base64) + strlen(pps_base64);

    priv->aux_sdp_line = (char*)malloc(fmtpFmtSize);       //new char[fmtpFmtSize];
    if(priv->aux_sdp_line == NULL) {
        fprintf(stderr, "can't get memory for aux sdp line\n");
        return NULL;
    }
    sprintf(priv->aux_sdp_line, fmtpFmt,
        96 + thiz->track_num - 1,
        profile_id,
        sps_base64, pps_base64);
    free(sps_base64);
    free(pps_base64);
    free(sps);
    free(pps);

    return priv->aux_sdp_line;
}

static const char* get_sdp_lines(struct media_subsession_s* thiz) {
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
        char const* mediaType = "video";
        char const* sdp_server = "0.0.0.0";
        unsigned char rtpPayloadType = 96 + thiz->track_num - 1;
        char const* auxSDPLine = get_aux_sdp(thiz);

        if(auxSDPLine==NULL){
            fprintf(stderr,"get_sdp_lines: auxSDPLine == NULL\n");
        }

        char const* const sdpFmt =
            "m=%s %u RTP/AVP %d\r\n"
            "c=IN IP4 %s\r\n"
            "b=AS:%u\r\n"
            "a=rtpmap:%u H264/90000\r\n"
            "%s"
            "a=control:%s\r\n";

        unsigned sdpFmtSize = strlen(sdpFmt)
            + strlen(mediaType) + 5 /* max short len */ + 3 /* max char len */
            + strlen(sdp_server)
            + 20 /* max int len */
            + 3
            + ((auxSDPLine==NULL)? 0:strlen(auxSDPLine))
            + strlen(thiz->get_track_id(thiz));

        priv->sdp_lines = (char*)malloc(sdpFmtSize);

        if(priv->sdp_lines == NULL) return NULL;

        sprintf(priv->sdp_lines, sdpFmt,
            mediaType, // m= <media>
            0, // m= <port>
            rtpPayloadType, // m= <fmt list>
            sdp_server, // c= address
            10000, // b=AS:<bandwidth> (as kbps)
            rtpPayloadType, // a=rtpmap:... (if present)
            (auxSDPLine==NULL)? "":auxSDPLine, // optional extra SDP line
            thiz->get_track_id(thiz)); // a=control:<track-id>
    }

    return priv->sdp_lines;
}

// bits manipulation for generating SEI NALU
#define BITS_BYTE 8
#define MOD8(x) (x&0x07)
static inline int addBit(uint8_t* bitsBuffer, uint32_t* lastBitsInBuffer, uint32_t val)
{
    uint32_t bitPosInByte, bytePos;
    uint32_t valTmp;

    // index the current byte
    bytePos = *lastBitsInBuffer >> 3;
    // first bit to add is on the left
    bitPosInByte = 7 - MOD8(*lastBitsInBuffer);

    // get the byte value from buffer
    valTmp = bitsBuffer[bytePos];

    // change the bit
    if(val > 0){
        valTmp = (valTmp | (0x01 << bitPosInByte));
    } else {
        valTmp = (valTmp & ~(0x01 << bitPosInByte));
    }

    // save byte to buffer
    bitsBuffer[bytePos] = valTmp;

    (*lastBitsInBuffer)++;
    return 0;
}
static inline int getBitNum(uint32_t val, uint32_t index)
{
    int lrc = 0;

    uint32_t lmask = (uint32_t) (0x01 << index);
    if ((val & lmask) > 0)
        lrc = 1;

    return lrc;
}

static inline int addBits(uint8_t* bitsBuffer, uint32_t* lastBitsInBuffer, uint32_t val, uint32_t numBits)
{
    int bit = 0;
    int index = numBits - 1;
    if(numBits <= 0 || numBits > 32){
        printf("Error: number of bits must be [1:32]\n");
        return -1;
    }

    while(index >= 0){
        bit = getBitNum(val, index);
        index--;
        addBit(bitsBuffer, lastBitsInBuffer, bit);
    }
    return 0;
}

static int generate_recovery_point(uint8_t** buffer, uint32_t recovery_frame_cnt)
{
    const uint32_t sei_header_size = 3;
    const uint32_t rbsp_trailing_bits = 1; /*1 byte*/
    uint8_t payload_size;
    uint32_t num_of_bits, leading_zero_bits;
    uint32_t pkt_size;
    uint8_t* seiRecovery;
    uint32_t lastBitsInBuffer = 0;

    // generate SEI pkt
    leading_zero_bits = log2(recovery_frame_cnt + 1);

    num_of_bits = 2*leading_zero_bits+1 /*recovery_frame_cnt*/
        + 1 /*exact match flag */ + 1/*broken_link_flag*/
        + 2 /*changing_slice_group_idc*/;

    payload_size = num_of_bits >> 3 ; // divide by 8
    payload_size = (MOD8(num_of_bits) > 0) ? payload_size + 1 : payload_size; // if mod(8) > 0, plus one

    /* preserve the RTP_HEADER_SIZE space in buffer */
    pkt_size = sei_header_size + payload_size + RTP_HEADER_SIZE + rbsp_trailing_bits;
    *buffer = (uint8_t*) malloc(sizeof(uint8_t)*pkt_size);

    seiRecovery = *buffer + RTP_HEADER_SIZE;
    memset(*buffer, 0 , sizeof(pkt_size));
    addBits(seiRecovery, &lastBitsInBuffer, 0x06, BITS_BYTE); // nal = SEI
    addBits(seiRecovery, &lastBitsInBuffer, 0x06, BITS_BYTE); // sei type = recovery point
    addBits(seiRecovery, &lastBitsInBuffer, payload_size, BITS_BYTE);
    addBits(seiRecovery, &lastBitsInBuffer, 0x00, leading_zero_bits);
    addBits(seiRecovery, &lastBitsInBuffer, recovery_frame_cnt + 1, leading_zero_bits + 1);
    addBits(seiRecovery, &lastBitsInBuffer, 0x00, 1); // exact match flag
    addBits(seiRecovery, &lastBitsInBuffer, 0x00, 1); // broken_link_flag
    addBits(seiRecovery, &lastBitsInBuffer, 0x0, 2); // changing_slice_group_idc
    // rbsp stop bit
    addBit(seiRecovery, &lastBitsInBuffer, 1);
    num_of_bits++;
    while(MOD8(num_of_bits) != 0){
        addBit(seiRecovery, &lastBitsInBuffer, 0);
        num_of_bits++;
    }
    // rbsp trailing bits
    addBits(seiRecovery, &lastBitsInBuffer, 0x80, 8);
    return pkt_size;
}

static int update_extra_info(media_subsession_t* thiz)
{
    int ret;
    media_subs_priv_t* priv;
    extra_info_t* extra;
    video_frame_reader_t* frame_reader;
    uint16_t M;          // distance between P frame
    uint16_t N;          // distance between I frame
    uint16_t GOPSize;    // distance between IDR
    uint16_t IRCycle;    // intra refresh cycle
    uint16_t recoveryFrameCnt; // recovery_frame_cnt value of SEI recovery point

    priv = (media_subs_priv_t*)thiz->priv;
    extra = (extra_info_t*)thiz->extra_info;
    frame_reader = (video_frame_reader_t*) priv->frame_reader;
    if (frame_reader != NULL && frame_reader->get_gop_info != NULL) {
        ret = frame_reader->get_gop_info(frame_reader, &M, &N, &GOPSize, &IRCycle, &recoveryFrameCnt);
        if(ret == 1 && N == 0xff && IRCycle != 0){
            uint32_t recovery_frame_cnt = recoveryFrameCnt == 0 ? IRCycle - 1 : recoveryFrameCnt;
            extra->sei_length = generate_recovery_point(&extra->sei_recovery_point,
                    recovery_frame_cnt);
        }
        return 0;
    } else {
        return -1;
    }
}


/********************************************************************************
 * public function
 ********************************************************************************/
void amba_h264_subsession_release(media_subsession_t* thiz); //FW declaration
media_subsession_t* amba_h264_subsession_create(rtsp_server_t* srv, void* streamer,
        unsigned int en_rtcp, unsigned int playback, unsigned int ticks, unsigned en_ap)
{
    media_subsession_t* media_sub;
    int rval;

    media_sub = (media_subsession_t*)malloc(sizeof(media_subsession_t));
    if(media_sub == NULL) {
        printf("Error: %s,%d\n",__FUNCTION__,__LINE__);
        return NULL;
    }

    if(playback == 0){
        rval = media_subsession_init(media_sub, srv, en_rtcp,
            1,  // enable RR
            1,  // use dsp time
            6970,
            RTP_FUA_HDR_LEN,
            streamer,
            read_avail_cb);
    } else {
        rval = media_subsession_init(media_sub, srv, en_rtcp,
            0,  // DON'T enable RR
            0,  // DON'T use dsp clock
            6972,
            RTP_FUA_HDR_LEN,
            streamer,
            read_avail_pb_cb);
    }
    if (rval < 0) {
        fprintf(stderr, "%s:%d, subsession init fail\n", __FUNCTION__, __LINE__);
        return NULL;
    }

    media_sub->get_sdp_lines = get_sdp_lines;
    media_sub->release = amba_h264_subsession_release;
    /* we should update this from get video_config */
    media_sub->ref_clock = REF_CLOCK;
    TicksPerFrame = ticks;

    min_sr_period = REF_CLOCK * 5;
    enable_stap = en_ap; //STAP-A

    media_sub->extra_info = (extra_info_t*)malloc(sizeof(extra_info_t));
    if(media_sub->extra_info == NULL){
        printf("Error: %s,%d: allocate space for extra info fail\n",__FUNCTION__,__LINE__);
        return NULL;
    }
    memset(media_sub->extra_info, 0, sizeof(extra_info_t));
    update_extra_info(media_sub);
    return media_sub;
}

void amba_h264_subsession_release(media_subsession_t* thiz)
{
    if(thiz == NULL) {
        return;
    }
    media_subsession_deinit(thiz);
    if(thiz->extra_info != NULL){
        extra_info_t* extra = (extra_info_t*) thiz->extra_info;
        if(extra->sei_recovery_point != NULL) free(extra->sei_recovery_point);
        free(thiz->extra_info);
    }
    free(thiz);
}


