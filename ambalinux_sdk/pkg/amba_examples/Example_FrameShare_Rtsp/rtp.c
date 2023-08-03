#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>

#include "media_subsession.h"
#include "rtsp_client_session.h"
#include "random.h"
#include "rtp.h"
#include "rtcp.h"
#include "ev.h"

//#define Enable_RTP_Debug 1
#if Enable_RTP_Debug
#define DEBUG printf
#else
#define DEBUG(...) //fprintf
#endif

#define HEADER_SIZE 12
#define RTP_MAX_IOVEC   4
#undef  SPEED_MONITOR

static const char bye_reason[] = "";

typedef struct {
        /* byte 0 */
#ifdef BIG_ENDIAN
    unsigned char csrc_len:4;   /* expect 0 */
    unsigned char extension:1;  /* expect 1, see RTP_OP below */
    unsigned char padding:1;    /* expect 0 */
    unsigned char version:2;    /* expect 2 */
#else
    unsigned char version:2;
    unsigned char padding:1;
    unsigned char extension:1;
    unsigned char csrc_len:4;
#endif
        /* byte 1 */
#ifdef BIG_ENDIAN
    unsigned char payload:7;    /* RTP_PAYLOAD_RTSP */
    unsigned char marker:1;     /* expect 1 */
#else
    unsigned char marker:1;
    unsigned char payload:7;
#endif
    /* bytes 2, 3 */
    unsigned short seq_no;
    /* bytes 4-7 */
    unsigned int timestamp;
    /* bytes 8-11 */
    unsigned ssrc;    /* stream number is used here. */

}rtp_pack_header;


typedef struct rtp_sink_priv_s {
    rtsp_server_t* srv;
    rtp_pack_header header;
    rtp_transport_t* transports[MAX_RTP_SESSION];
    struct iovec iovecs[RTP_MAX_IOVEC];         // only support upto MAX_IOVEC -1
    double create_time;
    uint8_t* rtcp_sr_sdes_data;
    uint32_t rtcp_sr_sdes_data_len;
    uint8_t* rtcp_sr_bye_data;
    uint32_t rtcp_sr_bye_data_len;
    unsigned int timestamp_base;
    unsigned int last_timestamp;
    unsigned int start_ts;  //the timestamp return in the RTSP-PLAY command
    unsigned int ssrc;
    unsigned int octet_count;
    unsigned int pkg_count;
    unsigned int clock_rate;
    void* streamer;
    uint8_t en_rtcp;    //rtcp enable flag
    uint8_t en_dyBR;    //whether we change bitrate based on RR statistic
    uint8_t en_dspCLK;  //get timestamp from dsp clock through ipc
#if 0 //moved to uitron
    //RR statistic variables
    unsigned int current_bitrate;
    unsigned int last_bitrate;
    unsigned int last_fr_lost;
    unsigned int zero_lost_count;
    uint8_t inMid_of_increment;
    uint8_t inMid_of_decrement;
    unsigned int net_bandwidth;
    unsigned int max_config_brate;
    unsigned int max_stable_brate;
    //end of RR statistic variables
#endif
    #ifdef SPEED_MONITOR
    ev_timer speed_monitor;
    unsigned int last_pkg_count;
    unsigned int last_octet_count;
    #endif
    unsigned short seq_no;
    uint8_t interleaved_data[4];
}rtp_sink_priv_t;

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

/*
* Reset statistic of RR
*/
static void reset_dyBR(struct rtp_sink_s *thiz)
{
    struct rtp_sink_priv_s *priv;
    unsigned int reset_bandwidth;
    if(thiz == NULL)
        return;
    priv = (rtp_sink_priv_t*) thiz->priv;
    reset_bandwidth = priv->srv->get_live_avg_brate();
    priv->srv->change_net_bandwidth(priv->streamer, reset_bandwidth);
}

/**
 * "ref_base" is to store base time while calling gettimeinseconds().
 * If we do not do this, it might overflow when we try to convert Wall time to timetick.
 * ex: timetick = (Walltime * ref_clock) <-- this might bigger than 32 bits if Walltime is upto date.
 **/
static double ref_base_dsp = 0;
static double ref_base = 0;
static double gettimeinseconds(struct rtp_sink_priv_s *priv, struct timespec *now, uint8_t en_dspCLK)
{
    double rval;

    if(en_dspCLK == 1){
        priv->srv->get_systime(&rval);
        if(ref_base_dsp == 0) {
            ref_base_dsp = rval;
        }
        rval -= ref_base_dsp;
    }else{
        struct timespec tt;

        clock_gettime(CLOCK_MONOTONIC, &tt);
        //rval = (double)lnx_tv.tv_sec + (double)(lnx_tv.tv_usec/1000000) + 0.000000505;
        rval = tt.tv_nsec;
        rval = rval/1000000000 + tt.tv_sec;

        if(ref_base == 0) {
            ref_base = rval;
        }
        rval -= ref_base;
    }
    now->tv_sec = (int) rval;
    now->tv_nsec = (rval - now->tv_sec) * 1000000000;

    //printf("rval:%lf, now.s=%u, now.n=%u\n",rval,(unsigned int)now->tv_sec,(unsigned int)now->tv_nsec);
    return rval;
}
static unsigned int get_start_ts(struct rtp_sink_s* thiz)
{
    struct rtp_sink_priv_s *priv;
    if(thiz == NULL){
        return 0;
    }
    priv = (rtp_sink_priv_t*)thiz->priv;
    return priv->start_ts;
}
static void set_start_ts(struct rtp_sink_s* thiz, unsigned int ref_ts)
{
    struct rtp_sink_priv_s *priv;
    if(thiz == NULL){
        return;
    }
    priv = (rtp_sink_priv_t*)thiz->priv;
    priv->start_ts = ref_ts;
}
static unsigned int get_current_timestamp(struct rtp_sink_s* thiz, unsigned int ref_timestamp)
{
    struct timespec ntp_time;
    double now;
    unsigned long long tmpll;
    unsigned int timestamp;
    struct rtp_sink_priv_s *priv;

    if(thiz == NULL) {
        return 0;
    }

    priv = (rtp_sink_priv_t*)thiz->priv;
    now = gettimeinseconds(priv,&ntp_time, priv->en_dspCLK);
    if(priv->create_time==-1) { //not init yet
        if(ref_timestamp!=0){
            timestamp = ref_timestamp;
            fprintf(stderr, "%s: return time stamp as ref_ts:%u\n",__FUNCTION__,timestamp);
        } else {
            priv->create_time = now;
            priv->timestamp_base = 0;
            tmpll = now * priv->clock_rate;
            timestamp = tmpll & 0xffffffff;
            fprintf(stderr, "%s: init TimeStamp:%u, cur:%u(%d), now:%lf, clock_rate:%u\n",
                    __FUNCTION__, priv->last_timestamp, timestamp,(int)(now*priv->clock_rate), now, priv->clock_rate);
        }
    } else {
        tmpll = (now - priv->create_time)*priv->clock_rate + priv->timestamp_base;
        timestamp = tmpll & 0xffffffff;
    }

    //printf("timestamp=%u, now=%lf, create_time=%lf, timestamp_base=%u, clock_rate=%u\n",
    //  timestamp,now,priv->create_time,priv->timestamp_base, priv->clock_rate);
    return timestamp;
}

static void rtcp_sr(struct rtp_sink_priv_s *priv, rtcp_sr_compound *pkt)
{
    struct timespec ntp_time;
    double now;
    uint32_t sr_size = sizeof(rtcp_header) + sizeof(rtcp_header_sr);

    pkt->sr_hdr.version = 2;

    pkt->sr_hdr.padding = 0;
    pkt->sr_hdr.count = 0;
    pkt->sr_hdr.pt = SR;
    pkt->sr_hdr.length = htons((sr_size>>2) - 1);

    pkt->sr_pkt.ssrc = htonl(priv->ssrc);
    now = gettimeinseconds(priv,&ntp_time, priv->en_dspCLK);
    pkt->sr_pkt.ntp_timestampH =
        htonl((unsigned int) ntp_time.tv_sec + 2208988800u); // 70 year
    pkt->sr_pkt.ntp_timestampL =
        htonl((((uint64_t) ntp_time.tv_nsec) << 32) / 1000000000u);
    if(priv->create_time == -1){
        priv->create_time = now;
        //priv->timestamp_base = priv->last_timestamp;
        priv->last_timestamp = (now*priv->clock_rate);
        fprintf(stderr, "%s: init TimeStamp base: %u, cur: %u\n",__FUNCTION__,
                priv->last_timestamp, (unsigned int)(now*priv->clock_rate));
        pkt->sr_pkt.rtp_timestamp = htonl(priv->last_timestamp);
    } else {
        double curr_RTCPTS = priv->timestamp_base;

        curr_RTCPTS += (now - priv->create_time)*priv->clock_rate;
        if(curr_RTCPTS > 0x00000000ffffffffLU){
            curr_RTCPTS -= 0x00000000ffffffffLU;
            priv->create_time = now;
            priv->timestamp_base = curr_RTCPTS;
        }
        pkt->sr_pkt.rtp_timestamp = htonl(curr_RTCPTS);
        //fprintf(stderr, "curr_RTCPTS: %lf, last_timestamp: %u t=(%f), timestamp_base=%u\n",
        //  curr_RTCPTS,priv->last_timestamp, (double)priv->last_timestamp/(double)priv->clock_rate, priv->timestamp_base);
    }

    pkt->sr_pkt.pkt_count = htonl(priv->pkg_count);
    pkt->sr_pkt.octet_count = htonl(priv->octet_count);
}

static const char test_name[] = "Amba-DV";
static uint32_t setup_rtcp_sr_sdes(struct rtp_sink_priv_s *priv, uint8_t** data) {
    const char* name = test_name;
    uint32_t name_len = strlen(name);
    uint32_t sdes_size = sizeof(rtcp_header) + sizeof(rtcp_header_sdes) +
        name_len;
    uint32_t pkt_size = sizeof(rtcp_header) + sizeof(rtcp_header_sr) +
        sdes_size;
    if(pkt_size % 4 != 0) {
        uint32_t padding = 4 - (pkt_size%4);
        sdes_size += padding;
        pkt_size += padding;
    }
    rtcp_sr_compound *pkt;
    *data = (uint8_t*)malloc(pkt_size);
    if(*data == NULL) {
        return 0;
    }
    memset(*data, 0, pkt_size);
    pkt = (rtcp_sr_compound*)*data;

    pkt->payload_hdr.version = 2;
    pkt->payload_hdr.padding = 0;
    pkt->payload_hdr.count = 1;
    pkt->payload_hdr.pt = SDES;
    pkt->payload_hdr.length = htons((sdes_size>>2) - 1);

    pkt->payload.sdes.ssrc = htonl(priv->ssrc);
    pkt->payload.sdes.attr_name = CNAME;
    pkt->payload.sdes.len = name_len;

    memcpy(pkt->payload.sdes.name, name, name_len);
    return pkt_size;
}


static uint32_t setup_rtcp_sr_bye(struct rtp_sink_priv_s *priv, uint8_t** data) {
    uint32_t payload_size = sizeof(rtcp_header) + sizeof(rtcp_header_bye) +
                            sizeof(bye_reason);
    uint32_t pkt_size = sizeof(rtcp_header) + sizeof(rtcp_header_sr) + payload_size;

    if(pkt_size % 4 != 0) {
        uint32_t padding = 4 - (pkt_size%4);
        payload_size += padding;
        pkt_size += padding;
    }
    rtcp_sr_compound *pkt;
    *data = (uint8_t*)malloc(pkt_size);
    if(*data == NULL) {
        return 0;
    }
    memset(*data, 0, pkt_size);
    pkt = (rtcp_sr_compound*)*data;

    pkt->payload_hdr.version = 2;
    pkt->payload_hdr.padding = 0;
    pkt->payload_hdr.count = 1;
    pkt->payload_hdr.pt = BYE;
    pkt->payload_hdr.length = htons((payload_size>>2) - 1);

    pkt->payload.bye.ssrc = htonl(priv->ssrc);
    pkt->payload.bye.length = htonl(sizeof(bye_reason) - 1);

    memcpy(&pkt->payload.bye.reason, &bye_reason, sizeof(bye_reason));

    return pkt_size;
}

static int send_rtcp_packet(struct rtp_sink_priv_s* priv, enum rtcp_type type, rtp_transport_t* transport)
{
    int ret = 0;

    if(priv == NULL)
        return -EINVAL;

    rtcp_sr_compound *pkt = NULL;
    uint32_t len = 0;
    switch(type) {
        case SDES:
            pkt = (rtcp_sr_compound*)priv->rtcp_sr_sdes_data;
            rtcp_sr(priv, pkt);
            len = priv->rtcp_sr_sdes_data_len;
            break;

        case BYE:
            pkt = (rtcp_sr_compound*)priv->rtcp_sr_bye_data;
            rtcp_sr(priv, pkt);
            len = priv->rtcp_sr_bye_data_len;
            break;

        default:
            fprintf(stderr, "rtcp type %u is not implement\n", type);
            return 0;
    }
    if(transport != NULL) {
        if(transport->is_tcp == 0) {
            ret = write(transport->rtcp_sock, pkt, len);
        } else {
            // tcp
            struct iovec iov[2];
            uint16_t n = htons((uint16_t)len);
            priv->interleaved_data[1] = transport->rtcp_channel;
            memcpy(&priv->interleaved_data[2], &n, sizeof(uint16_t));

            iov[0].iov_base = &priv->interleaved_data[0];
            iov[0].iov_len = 4;
            iov[1].iov_base = pkt;
            iov[1].iov_len = len;
            ret = writev(transport->rtcp_sock, &iov[0], 2);

        }
        if(ret < 0) {
            fprintf(stderr, "error send rtcp packet: %s\n", strerror(errno));
        }
    }
    return 0;
}
#define STANDARD_TS_BASE (0x83aa7e80)   //1970/01/01, 00:00:00
#define LWORD_TIME_DIFF(a,b) ((a<b)?(65536 + a - b):(a-b))
static void parse_receiver_report(rtp_sink_t* thiz, uint8_t *packet, int count)
{
    rtp_sink_priv_t* priv = NULL;
    double now = 0;
    double propagation_delay = 0;
    struct timespec test;
    double last_sr_ts = 0;
    unsigned int t_sec;

    if(thiz == NULL){
        fprintf(stderr, "%s: thiz is NULL!\n",__FUNCTION__);
        return;
    }
    priv = thiz->priv;
    now =  gettimeinseconds(priv, &test, priv->en_dspCLK);
    t_sec = now + STANDARD_TS_BASE;

    packet+=4;
    while(count--) {
        rtcp_report_block *report = (rtcp_report_block *)packet;
        if(priv->en_rtcp){
            /* LSR represent the middle 32bits of 64bit NTP timestamp format(a fixed-point number with integer part in the first 32bit
             *  and the fractional part in the last 32 bits).
             *  Hence, LSR&0xffff0000 >> 16 = integer part. LSR& 0x0000ffff/65536 = fraction part.
             */
            last_sr_ts = (double)((ntohl(report->last_sr) & 0xffff0000)>>16) + (double)(ntohl(report->last_sr) & 0x0000ffff)/(double)65536;
            now = (double)(t_sec & (unsigned int)0x0000ffff) + (now - (unsigned int) now);
            if(report->last_sr !=0 && report->delay_last_sr !=0){
                propagation_delay = LWORD_TIME_DIFF(now, last_sr_ts) - (double)ntohl(report->delay_last_sr)/(double)65536;
            }
        }
        /*
           do {
        //int ssrc = ntohl(((rtcp_header_rr*)packet)->ssrc);
        printf("[RTCP] ssrc %u, fraction %d, lost %d, "
        "sequence %u, jitter %u, "
        "last Sender Report %lf, delay %u\n",
        report->ssrc, report->fract_lost,
        (report->packet_lost[0]<<3)|(report->packet_lost[1]<<2)|(report->packet_lost[2]),
        ntohl(report->h_seq_no), ntohl(report->jitter),
        last_sr, ntohl(report->delay_last_sr));
        } while(0);
        */
        packet+=sizeof(rtcp_report_block);
        if(priv->en_dyBR){
            priv->srv->send_rr_stat(priv->streamer, report->fract_lost, ntohl(report->jitter), propagation_delay);
        }
    }
}

static inline void reset_rtsp_session_timeout(rtp_sink_t* thiz, uint32_t sd)
{
    int i;
    rtp_sink_priv_t* priv = (rtp_sink_priv_t*)thiz->priv;
    for(i = 0; i < MAX_RTP_SESSION; i++) {
        if(priv->transports[i] != NULL){
            if(priv->transports[i]->rtcp_sock == sd) {
                rtsp_client_session_reset_timeout((struct rtsp_client_session*) priv->transports[i]->rtsp_client_session);
                break;
            }
        }
    }
}

static void rtcp_handler(rtp_sink_t* thiz, uint8_t *data, uint32_t len, uint32_t sd)
{
    uint32_t rtcp_size = 0;

    while(len > sizeof(rtcp_header)) {
        rtcp_header *rtcp = (rtcp_header*)data;
        rtcp_size = (ntohs(rtcp->length) + 1 )<<2;
        //printf("[RTCP] %s\n", rtcp_pt_to_string(rtcp->pt));
        if(rtcp_size > len) {
            fprintf(stderr, "[RTCP] malformed packet\n");
            return;
        }
        switch(rtcp->pt) {
            case BYE:
                break;
            case SR:
                break;
            case RR:
                parse_receiver_report(thiz, data + 4, rtcp->count);
                reset_rtsp_session_timeout(thiz, sd); //RR can be keep-alive signal
                break;
            case SDES:
            default:
                break;
        }
        len -= rtcp_size;
        data += rtcp_size;
    }
}

static void rtcp_read_cb(struct ev_loop *ev_loop, struct ev_io *w, int revent)
{
    static char data[RTP_DEFAULT_MTU*2] = {0};
    rtp_sink_t* thiz;
    int len;

    thiz = (rtp_sink_t*) w->data;
    len = recv(w->fd, data, RTP_DEFAULT_MTU*2, MSG_DONTWAIT);
    if(len > 0) {
        rtcp_handler(thiz, (uint8_t *)data, len, w->fd);
    }
}

/********************End of RTCP*****************************/
static int send_sr_sdes(struct rtp_sink_s* thiz) {
    rtp_sink_priv_t* priv = NULL;
    int i = 0;
    if(thiz == NULL) {
        return -1;
    }

    //fprintf(stderr, "send_sr_sdes...\n");
    priv = (rtp_sink_priv_t*)thiz->priv;
    for(i = 0; i < MAX_RTP_SESSION; i++) {
        if(priv->transports[i] != NULL) {
            send_rtcp_packet(priv, SDES, priv->transports[i]);
        }
    }
    return 0;
}

static unsigned short get_current_seq_no(struct rtp_sink_s* thiz) {
    rtp_sink_priv_t* priv = NULL;
    if(thiz == NULL) {
        return -1;
    }
    priv = (rtp_sink_priv_t*)thiz->priv;
    return priv->seq_no;

}

static unsigned timestamp(struct rtp_sink_s* thiz) {
    rtp_sink_priv_t* priv = NULL;
    if(thiz == NULL) {
        return -1;
    }
    priv = (rtp_sink_priv_t*)thiz->priv;
    return priv->last_timestamp;
}

#if 0
static int send_packet_special2(struct rtp_sink_s* thiz,
                    const struct iovec *iov, unsigned iovcnt,
                    unsigned int timestamp,
                    unsigned char marker)
{
    unsigned i = 0;
    int ret;
    rtp_transport_t* transport;
    rtp_sink_priv_t* priv = NULL;

    if(thiz == NULL || iov == NULL || iovcnt > (RTP_MAX_IOVEC - 1)) {
        return -1;
    }
    priv = (rtp_sink_priv_t*)thiz->priv;
    priv->header.version = 2;
    priv->header.padding = 0;
    priv->header.extension = 0;
    priv->header.csrc_len = 0;
    priv->header.marker = marker & 0x01;
    priv->header.payload = thiz->payload_type &0x7f;
    priv->header.seq_no = htons(priv->seq_no++);
    priv->header.timestamp = htonl(timestamp);       //XXX
    priv->header.ssrc = htonl(priv->ssrc);
    //dump_mem(buf, 32);
    priv->iovecs[0].iov_base = &priv->header;
    priv->iovecs[0].iov_len = HEADER_SIZE;
    memcpy(&priv->iovecs[1], iov, iovcnt);

    for(i = 0; i < MAX_RTP_SESSION; i++) {
        if(priv->transports[i] != NULL) {
            transport = priv->transports[i];
            ret = writev(transport->rtp_sock, &priv->iovecs[0], iovcnt + 1);
            if(ret < 0) {
                fprintf(stderr, "error sendto %s\n", strerror(errno));
            }
        }
    }
    priv->pkg_count++;
    return 0;
}
#endif

static int send_packet_special_iov(struct rtp_sink_s* thiz, rtp_payload_info_t* info) {
    unsigned i = 0;
    int ret;
    rtp_transport_t* transport;
    rtp_sink_priv_t* priv = NULL;
    struct iovec iov[3];
    if(thiz == NULL || info == NULL) {
        return -1;
    }
    priv = (rtp_sink_priv_t*)thiz->priv;

    /* set last_timestamp first for the sr_sdes */
    priv->last_timestamp = info->timestamp;

    if(thiz->need_to_send_sr_sdes){
        thiz->need_to_send_sr_sdes = 0;
        ret = send_sr_sdes(thiz);
    if(ret < 0){
        fprintf(stderr, "Fail to do send_sr_sdes, ret=%d\n", ret);
    }
    }

    priv->header.version = 2;
    priv->header.padding = 0;
    priv->header.extension = 0;
    priv->header.csrc_len = 0;
    priv->header.marker = info->maker& 0x01;
    priv->header.payload = thiz->payload_type &0x7f;
    priv->header.seq_no = htons(priv->seq_no++);
    priv->header.timestamp = htonl(info->timestamp);       //XXX
    priv->header.ssrc = htonl(priv->ssrc);
    //dump_mem(buf, 32);

    // interleaved data
    iov[0].iov_base = &priv->interleaved_data[0];
    iov[0].iov_len = 4;
    // interleaved data end
    iov[1].iov_base = &priv->header;
    iov[1].iov_len = sizeof(priv->header);
    iov[2].iov_base = info->buf + 12;
    iov[2].iov_len = info->len - 12;
    for(i = 0; i < MAX_RTP_SESSION; i++) {
        if(priv->transports[i] != NULL) {
            transport = priv->transports[i];
            //ret = sendto(transport->rtp_sock, priv->buf, info->len, 0, (struct sockaddr*)&transport->remote_rtp, sizeof(transport->remote_rtp));
            //ret = write(transport->rtp_sock, info->buf, info->len);
            if(transport->is_tcp == 0) {
                // udp
                ret = writev(transport->rtp_sock, &iov[1], 2);
            } else {
                // tcp
                uint16_t n = htons((uint16_t)info->len);
                priv->interleaved_data[1] = transport->rtp_channel;
                memcpy(&priv->interleaved_data[2], &n, sizeof(uint16_t));
                ret = writev(transport->rtp_sock, &iov[0], 3);
            }
            if(ret < 0) {
                fprintf(stderr, "error send packet: %s\n", strerror(errno));
                if (errno != EAGAIN && errno != EWOULDBLOCK){
                    fprintf(stderr, "Notify error occurs!\n");
                    if(transport->rtsp_client_session_error_notify != NULL) {
                        transport->rtsp_client_session_error_notify(transport->rtsp_client_session);
                    }
                }
            } else if ( ret < info->len) {
                //fprintf(stderr, "truncted sendto\n");
                fprintf(stderr, "truncted, only send %u bytes\n", ret);
            }
        }
    }
    priv->pkg_count++;
    priv->octet_count += (info->len - 12);  //FIXME:

    return 0;
}

#if 0 //XXX it has bug, not use
static int send_packet_special(struct rtp_sink_s* thiz, rtp_payload_info_t* info) {
    unsigned i = 0;
    int ret;
    rtp_transport_t* transport;
    rtp_sink_priv_t* priv = NULL;
    if(thiz == NULL || info == NULL) {
        return -1;
    }
    priv = (rtp_sink_priv_t*)thiz->priv;
    #if 1
    rtp_pack_header* header = (rtp_pack_header*)info->buf;
    header->version = 2;
    header->padding = 0;
    header->extension = 0;
    header->csrc_len = 0;
    header->marker = info->maker& 0x01;
    header->payload = thiz->payload_type &0x7f;
    header->seq_no = htons(priv->seq_no++);
    header->timestamp = htonl(info->timestamp);       //XXX
    header->ssrc = htonl(priv->ssrc);
    #else
    priv->header.version = 2;
    priv->header.padding = 0;
    priv->header.extension = 0;
    priv->header.csrc_len = 0;
    priv->header.marker = info->maker& 0x01;
    priv->header.payload = thiz->payload_type &0x7f;
    priv->header.seq_no = htons(priv->seq_no++);
    priv->header.timestamp = htonl(info->timestamp);       //XXX
    priv->header.ssrc = htonl(priv->ssrc);
    memcpy(&info->buf, &priv->header, 12);
    #endif
    //dump_mem(buf, 32);

    for(i = 0; i < MAX_RTP_SESSION; i++) {
        if(priv->transports[i] != NULL) {
            transport = priv->transports[i];
            //ret = sendto(transport->rtp_sock, info->buf, info->len, 0, (struct sockaddr*)&transport->remote_rtp, sizeof(transport->remote_rtp));
            ret = write(transport->rtp_sock, info->buf, info->len);
            if(ret < 0) {
                fprintf(stderr, "erro sendto %s\n", strerror(errno));
            } else if ( ret < info->len) {
                fprintf(stderr, "truncted sendto\n");
            }
        }
    }
    priv->pkg_count++;
    priv->octet_count += (info->len - 12);
    return 0;
}
#endif
/*
 * send packet in iov(filled by subsession)
 *
 * info  : payload info such as ts, len, marker
 * vec: iovec contains the NALs vector
 * vec_num : indicate how many nals are in iovec
 */
static int send_packet_vector(struct rtp_sink_s* thiz, rtp_payload_info_t* info,
        struct iovec* vec, unsigned vec_num)
{
    int ret, i;
    rtp_transport_t* transport;
    rtp_sink_priv_t* priv = NULL;
    struct iovec* iov =
        (struct iovec*) malloc(sizeof(struct iovec) * (vec_num+2/*interleaved+rtpHdr*/));
    if(thiz == NULL || info == NULL || vec_num <= 1 || vec == NULL || iov == NULL) {
        fprintf(stderr, "%s: Invalid parameter. thiz=%p, info=%p, vec_num=%d, vec=%p, iov=%p\n",
                __FUNCTION__, thiz, info, vec_num, vec, iov);
        return -1;
    }
    priv = (rtp_sink_priv_t*)thiz->priv;

    priv->header.version = 2;
    priv->header.padding = 0;
    priv->header.extension = 0;
    priv->header.csrc_len = 0;
    priv->header.marker = info->maker& 0x01;
    priv->header.payload = thiz->payload_type &0x7f;
    priv->header.seq_no = htons(priv->seq_no++);
    priv->header.timestamp = htonl(info->timestamp);       //XXX
    priv->header.ssrc = htonl(priv->ssrc);

    // interleaved data
    iov[0].iov_base = &priv->interleaved_data[0];
    iov[0].iov_len = 4;
    // rtp header
    iov[1].iov_base = &priv->header;
    iov[1].iov_len = sizeof(priv->header);
    memcpy(&iov[2], vec, sizeof(struct iovec) * vec_num);

    for(i = 0; i < MAX_RTP_SESSION; i++) {
        if(priv->transports[i] != NULL) {
            transport = priv->transports[i];
            if(transport->is_tcp == 0) {
                // udp
                ret = writev(transport->rtp_sock, &iov[1], vec_num + 1);
            } else {
                // tcp
                uint16_t n = htons((uint16_t)info->len);
                priv->interleaved_data[1] = transport->rtp_channel;
                memcpy(&priv->interleaved_data[2], &n, sizeof(uint16_t));
                ret = writev(transport->rtp_sock, &iov[0], vec_num + 2/*interleaved data*/);
            }
            if(ret < 0) {
                fprintf(stderr, "error send packet: %s\n", strerror(errno));
                if (errno != EAGAIN && errno != EWOULDBLOCK){
                    fprintf(stderr, "Notify error occurs!\n");
                    if(transport->rtsp_client_session_error_notify != NULL) {
                        transport->rtsp_client_session_error_notify(transport->rtsp_client_session);
                    }
                }
            } else if ( ret < info->len) {
                fprintf(stderr, "truncted, only send %u bytes\n", ret);
            }
        }
    }
    priv->pkg_count++;
    priv->octet_count += (info->len - 12);
    free(iov);
    return 0;
}

static int send_packet(struct rtp_sink_s* thiz, rtp_payload_info_t* info) {
    unsigned i = 0;
    int ret;
    rtp_transport_t* transport;
    rtp_sink_priv_t* priv = NULL;
    struct iovec iov[2];
    if(thiz == NULL || info == NULL) {
        return -1;
    }
    priv = (rtp_sink_priv_t*)thiz->priv;

    priv->header.version = 2;
    priv->header.padding = 0;
    priv->header.extension = 0;
    priv->header.csrc_len = 0;
    priv->header.marker = info->maker& 0x01;
    priv->header.payload = thiz->payload_type &0x7f;
    priv->header.seq_no = htons(priv->seq_no++);
    priv->header.timestamp = htonl(info->timestamp);       //XXX
    priv->header.ssrc = htonl(priv->ssrc);
    //dump_mem(buf, 32);

    iov[0].iov_base = &priv->header;
    iov[0].iov_len = sizeof(priv->header);
    iov[1].iov_base = info->buf;
    iov[1].iov_len = info->len;
    for(i = 0; i < MAX_RTP_SESSION; i++) {
        if(priv->transports[i] != NULL) {
            transport = priv->transports[i];
            //ret = sendto(transport->rtp_sock, priv->buf, info->len, 0, (struct sockaddr*)&transport->remote_rtp, sizeof(transport->remote_rtp));
            //ret = write(transport->rtp_sock, info->buf, info->len);
            ret = writev(transport->rtp_sock, &iov[0], 2);
            if(ret < 0) {
                fprintf(stderr, "erro send packet: %s\n", strerror(errno));
            } else if ( ret < info->len) {
                fprintf(stderr, "truncted sendto\n");
            }
        }
    }
    priv->pkg_count++;
    priv->octet_count += info->len;
    return 0;
}

static int add_transport(struct rtp_sink_s* thiz, rtp_transport_t* transport)
{
    unsigned i = 0;
    rtp_sink_priv_t* priv = NULL;
    ev_io *io_watcher;

    if(thiz == NULL) {
        return -1;
    }
    priv = (rtp_sink_priv_t*)thiz->priv;

    for(i = 0; i < MAX_RTP_SESSION; i++) {
        if(priv->transports[i] == NULL) {
            priv->transports[i] = transport;
            transport->rtcp_reader.data = thiz;
            io_watcher = &transport->rtcp_reader;
            if(transport->is_tcp == 0){
                ev_io_init(io_watcher, rtcp_read_cb, transport->rtcp_sock, EV_READ);
                ev_io_start(priv->srv->loop, io_watcher);
            }
            return 0;
        }
    }
    return -1;
}

static int remove_transport(struct rtp_sink_s* thiz, rtp_transport_t* transport){
    unsigned i = 0;
    rtp_sink_priv_t* priv = NULL;
    if(thiz == NULL) {
        return -1;
    }

    priv = (rtp_sink_priv_t*)thiz->priv;

    for(i = 0; i < MAX_RTP_SESSION; i++) {
        if(priv->transports[i] == transport) {
            send_rtcp_packet(priv, BYE, transport);
            if(transport->is_tcp == 0){
                ev_io_stop(priv->srv->loop, &transport->rtcp_reader);
            }
            priv->transports[i] = NULL;
            return 0;
        }
    }
    return -1;
}

static int goodbye(struct rtp_sink_s* thiz) {
    rtp_sink_priv_t* priv = NULL;
    int i = 0;
    if(thiz == NULL) {
        return -1;
    }

    priv = (rtp_sink_priv_t*)thiz->priv;
    for(i = 0; i < MAX_RTP_SESSION; i++) {
        if(priv->transports[i] != NULL) {
            send_rtcp_packet(priv, BYE, priv->transports[i]);
        }
    }
    return 0;
}

#ifdef SPEED_MONITOR
static void monitor_cb(struct ev_loop* loop, ev_timer* w, int revent) {
    rtp_sink_t* thiz = (rtp_sink_t*)w->data;
    rtp_sink_priv_t* priv = thiz->priv;
    fprintf(stderr, "delta octet %u, cur octet cnt : %u\n", (priv->octet_count - priv->last_octet_count), priv->octet_count);
    priv->last_octet_count = priv->octet_count;
    priv->last_pkg_count = priv->pkg_count;
}
#endif

rtp_sink_t* rtp_sink_create(rtsp_server_t* srv, unsigned char payload_type, unsigned int clock_rate,
                    uint8_t enableBR, uint8_t en_rtcp, uint8_t en_dspCLK, void* streamer)
{
    rtp_sink_t* thiz = NULL;
    rtp_sink_priv_t* priv = NULL;

    if(srv == NULL) {
        return NULL;
    }
    thiz = (rtp_sink_t*)malloc(sizeof(rtp_sink_t));
    if(thiz == NULL) {
        return NULL;
    }
    memset(thiz, 0, sizeof(rtp_sink_t));

    priv = (rtp_sink_priv_t*)malloc(sizeof(rtp_sink_priv_t));
    if(priv == NULL) {
        free(thiz);
        return NULL;
    }
    memset(priv, 0, sizeof(rtp_sink_priv_t));

    ref_base_dsp = 0;
    ref_base = 0;
    priv->srv = srv;
    priv->seq_no = (unsigned short)our_random();
    priv->ssrc = our_random32();
    //priv->last_timestamp = our_random32();
    //priv->last_timestamp = priv->timestamp_base = our_random32();
    /*do {
        struct timespec time;
        struct timeval currTime;
        gettimeofday(&currTime, NULL);

        priv->timestamp_base = 0;
        priv->last_timestamp = (currTime.tv_sec*1000000)+currTime.tv_usec;
        //fprintf(stderr,"create ssrc=%u, last_timestamp=%u\n\n",priv->ssrc,priv->last_timestamp);
        priv->create_time = gettimeinseconds(&time);
    } while(0); */
    priv->create_time = -1;
    priv->clock_rate = clock_rate;
#if 0//moved to uitron
    priv->max_config_brate = srv->get_live_avg_brate();//init with sensor_config
    priv->net_bandwidth = priv->max_config_brate;
#endif
    priv->en_dyBR = enableBR;
    priv->en_rtcp = en_rtcp;
    priv->en_dspCLK = en_dspCLK;
    priv->streamer = streamer;
    priv->rtcp_sr_bye_data_len = setup_rtcp_sr_bye(priv, &priv->rtcp_sr_bye_data);
    if( priv->rtcp_sr_bye_data_len == 0) {
        free(priv);
        free(thiz);
        return NULL;
    }

    priv->rtcp_sr_sdes_data_len = setup_rtcp_sr_sdes(priv, &priv->rtcp_sr_sdes_data);
    if(priv->rtcp_sr_sdes_data_len == 0) {
        free(priv->rtcp_sr_bye_data);
        free(priv);
        free(thiz);
        return NULL;
    }
    priv->interleaved_data[0] = '$';    //0:'$', 1:channel, [2,3]:length

    thiz->priv = priv;
    thiz->payload_type = payload_type;
    thiz->get_cur_seq_no = get_current_seq_no;
    thiz->get_cur_timestamp = timestamp;
    thiz->send_packet = send_packet;
    thiz->add_transport = add_transport;
    thiz->remove_transport = remove_transport;
    thiz->send_packet_special = send_packet_special_iov;//send_packet_special;
    thiz->send_packet_vector = send_packet_vector;
    thiz->send_sr = send_sr_sdes;
    thiz->goodbye = goodbye;
    thiz->curr_timestamp = get_current_timestamp;
    thiz->reset_dyBR = reset_dyBR;
    thiz->get_start_ts  = get_start_ts;
    thiz->set_start_ts  = set_start_ts;


#ifdef SPEED_MONITOR
    priv->speed_monitor.data = thiz;
    ev_init(&priv->speed_monitor, monitor_cb);
    ev_timer_set(&priv->speed_monitor, 1, 1.);
    ev_timer_start(priv->srv->loop, &priv->speed_monitor);
#endif
    return thiz;
}

void rtp_sink_release(rtp_sink_t* rtp) {
    rtp_sink_priv_t* priv = NULL;
    if(rtp == NULL) {
        return;
    }
    priv = (rtp_sink_priv_t*)rtp->priv;
    if (priv->en_dyBR == 1) {
        rtp->reset_dyBR(rtp);//clear the network streaming bandwidth limitation
    }

#ifdef SPEED_MONITOR
    ev_timer_stop(priv->srv->loop, &priv->speed_monitor);
#endif
    free(priv->rtcp_sr_bye_data);
    free(priv->rtcp_sr_sdes_data);
    free(priv);
    free(rtp);
}

