#ifndef _RTCP_H_
#define _RTCP_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum rtcp_type {
    SR      = 200,
    RR      = 201,
    SDES    = 202,
    BYE     = 203,
    APP     = 204
}rtcp_type_t;

#define rtcp_pt_to_string(pt)\
    ((pt == SR) ?  "Sender Report" : \
     (pt == RR) ?  "Receiver Report" : \
     (pt == SDES)? "Source Description" : \
     (pt == BYE) ? "Bye" : \
     (pt == APP) ? "Application" : \
                   "Unknown")

typedef enum {
    CNAME = 1,
    NAME = 2,
    EMAIL = 3,
    PHONE = 4,
    LOC = 5,
    TOOL = 6,
    NOTE = 7,
    PRIV = 8
} rtcp_info;

typedef struct rtcp_header {
#ifdef BIG_ENDIAN
        uint8_t count:5;    //< SC or RC
        uint8_t padding:1;
        uint8_t version:2;
#else
        uint8_t version:2;
        uint8_t padding:1;
        uint8_t count:5;    //< SC or RC
#endif
        uint8_t pt;
        uint16_t length;

}rtcp_header;

typedef struct rtcp_header_sr {
    uint32_t ssrc;
    uint32_t ntp_timestampH;
    uint32_t ntp_timestampL;
    uint32_t rtp_timestamp;
    uint32_t pkt_count;
    uint32_t octet_count;
} rtcp_header_sr;

typedef struct rtcp_header_rr {
    uint32_t ssrc;
} rtcp_header_rr;

typedef struct rtcp_report_block {
    uint32_t ssrc;
    uint8_t fract_lost;
    uint8_t packet_lost[3];
    uint32_t h_seq_no;
    uint32_t jitter;
    uint32_t last_sr;
    uint32_t delay_last_sr;
} rtcp_report_block;

typedef struct rtcp_header_sdes {
    uint32_t ssrc;
    uint8_t attr_name;
    uint8_t len;
    char name[];
} __attribute__((__packed__)) rtcp_header_sdes;

typedef struct rtcp_header_bye {
    uint32_t ssrc;
    uint8_t length;
    char reason[];
} __attribute__((__packed__)) rtcp_header_bye;

typedef struct rtcp_sr_compound {
    /** The header for the SR preamble */
    rtcp_header sr_hdr;
    /** The actual SR preamble */
    rtcp_header_sr sr_pkt;
    /** The header for the payload packet */
    rtcp_header payload_hdr;
    /** Polymorphic payload */
    union {
        /** Payload for the source description packet */
        rtcp_header_sdes sdes;
        /** Payload for the goodbye packet */
        rtcp_header_bye bye;
    } payload;
} rtcp_sr_compound;

#ifdef __cplusplus
}
#endif

#endif
