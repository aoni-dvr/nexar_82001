#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <arpa/inet.h>

#include "rtcp.h"
#include "ev.h"
#include "rtsp_server.h"
#include "rtsp_client_session.h"
#include "media_session.h"
#include "media_subsession.h"
#include "gsensor_subsession.h"
#include "random.h"

#include "rtsp_client_session.h"
#include "ambastream.h"

#define DEBUG_SERVER 1
#define MAX_TRACK               3
#define GSENSOR_MAX_TRACK       3
#define RTSP_BUFFER_SIZE        10000 // for incoming requests, and outgoing responses
#define RTSP_PARAM_STRING_MAX   200
typedef enum session_status_e{
    INIT,
    READY,
    PLAYING,
    PAUSE
}session_status_t;

typedef struct rtsp_client_session {
    rtsp_server_t* srv;
    unsigned session_id;
    int client_sock;
    struct sockaddr_in client_addr;
    // request buf
    unsigned recursion_count;
    unsigned request_bytes;
    unsigned request_buf_left;
    unsigned char* last_CRLF;
    unsigned char request_buf[RTSP_BUFFER_SIZE];
    unsigned char respond_buf[RTSP_BUFFER_SIZE];
    ev_io incoming_watcher;
    ev_tstamp last_activity; // time of last activity
    ev_timer timer;
    unsigned char active;
    unsigned char rtp_over_tcp;
    session_status_t status;
    media_session_t* media_session;
    media_subsession_t* subsessions[MAX_TRACK];
    unsigned char is_gsensor_stream;
    gsensor_subsession_t *gsensor_subsessions[GSENSOR_MAX_TRACK];
    /* to manage pause npt */
    unsigned int pause_npt; // pause npt in msec
    unsigned long last_npt; // last play npt
    unsigned long last_npt_time;    // last play npt in msec
}rtsp_client_session_t;

ev_tstamp timeout = 61.;

/////////////////////////////////////////////

static void rtsp_client_session_reset_buf(rtsp_client_session_t* session);
static void rtsp_client_session_notify_network_error(void* session_ptr);

typedef enum StreamingMode {
  RTP_UDP,
  RTP_TCP,
  RAW_UDP
} StreamingMode;

static void parseTransportHeader(char const* buf,
                 StreamingMode* streamingMode,
                 char** streamingModeString,
                 char** destinationAddressStr,
                 unsigned char* destinationTTL,
                 unsigned short* clientRTPPortNum, // if UDP
                 unsigned short* clientRTCPPortNum, // if UDP
                 unsigned char* rtpChannelId, // if TCP
                 unsigned char* rtcpChannelId // if TCP
                 ) {
  // Initialize the result parameters to default values:
  *streamingMode = RTP_UDP;
  *streamingModeString = NULL;
  *destinationAddressStr = NULL;
  *destinationTTL = 255;
  *clientRTPPortNum = 0;
  *clientRTCPPortNum = 1;
  *rtpChannelId = *rtcpChannelId = 0xFF;

  unsigned short p1, p2;
  unsigned ttl, rtpCid, rtcpCid;

  // First, find "Transport:"
  while (1) {
    if (*buf == '\0') return; // not found
    if (strncasecmp(buf, "Transport: ", 11) == 0) break;
    ++buf;
  }

  // Then, run through each of the fields, looking for ones we handle:
  char const* fields = buf + 11;
  char* field = strdup(fields);
  while (sscanf(fields, "%[^;]", field) == 1) {
    if (strcmp(field, "RTP/AVP/TCP") == 0) {
      *streamingMode = RTP_TCP;
    } else if (strcmp(field, "RAW/RAW/UDP") == 0 ||
           strcmp(field, "MP2T/H2221/UDP") == 0) {
      *streamingMode = RAW_UDP;
      *streamingModeString = strdup(field);
    } else if (strncasecmp(field, "destination=", 12) == 0) {
      free(*destinationAddressStr);
      *destinationAddressStr = strdup(field+12);
    } else if (sscanf(field, "ttl%u", &ttl) == 1) {
      *destinationTTL = (unsigned char)ttl;
    } else if (sscanf(field, "client_port=%hu-%hu", &p1, &p2) == 2) {
    *clientRTPPortNum = p1;
    *clientRTCPPortNum = *streamingMode == RAW_UDP ? 0 : p2; // ignore the second port number if the client asked for raw UDP
    } else if (sscanf(field, "client_port=%hu", &p1) == 1) {
    *clientRTPPortNum = p1;
    *clientRTCPPortNum = *streamingMode == RAW_UDP ? 0 : p1 + 1;
    } else if (sscanf(field, "interleaved=%u-%u", &rtpCid, &rtcpCid) == 2) {
      *rtpChannelId = (unsigned char)rtpCid;
      *rtcpChannelId = (unsigned char)rtcpCid;
    }

    fields += strlen(field);
    while (*fields == ';') ++fields; // skip over separating ';' chars
    if (*fields == '\0' || *fields == '\r' || *fields == '\n') break;
  }
  free(field);
}

static char parseRTSPRequestString(char const* reqStr,
                   unsigned reqStrSize,
                   char* resultCmdName,
                   unsigned resultCmdNameMaxSize,
                   char* resultURLPreSuffix,
                   unsigned resultURLPreSuffixMaxSize,
                   char* resultURLSuffix,
                   unsigned resultURLSuffixMaxSize,
                   char* resultCSeq,
                   unsigned resultCSeqMaxSize,
                   unsigned* contentLength) {
    // This parser is currently rather dumb; it should be made smarter #####
    *contentLength = 0; // default value

    // Read everything up to the first space as the command name:
    char parseSucceeded = 0; //False
    unsigned i;
    unsigned k;
    for (i = 0; i < resultCmdNameMaxSize-1 && i < reqStrSize; ++i) {
        char c = reqStr[i];
        if (c == ' ' || c == '\t') {
            parseSucceeded = 1;   //True;
            break;
        }

        resultCmdName[i] = c;
    }
    resultCmdName[i] = '\0';
    if (parseSucceeded == 0) return 0;    //False;


    // Skip over the prefix of any "rtsp://" or "rtsp:/" URL that follows:
    unsigned j = i+1;
    while (j < reqStrSize && (reqStr[j] == ' ' || reqStr[j] == '\t')) ++j; // skip over any additional white space
    for (; (int)j < (int)(reqStrSize-8); ++j) {
        if ((reqStr[j] == 'r' || reqStr[j] == 'R')
        && (reqStr[j+1] == 't' || reqStr[j+1] == 'T')
        && (reqStr[j+2] == 's' || reqStr[j+2] == 'S')
        && (reqStr[j+3] == 'p' || reqStr[j+3] == 'P')
        && reqStr[j+4] == ':' && reqStr[j+5] == '/') {
        j += 6;
        if (reqStr[j] == '/') {
            // This is a "rtsp://" URL; skip over the host:port part that follows:
            ++j;
            while (j < reqStrSize && reqStr[j] != '/' && reqStr[j] != ' ')
                ++j;
        } else {
            // This is a "rtsp:/" URL; back up to the "/":
            --j;
        }
        i = j;
        break;
        }
    }

    // Look for the URL suffix (before the following "RTSP/"):
    parseSucceeded = 0;   //False;
    for (k = i+1; (int)k < (int)(reqStrSize-5); ++k) {
        if (reqStr[k] == 'R' && reqStr[k+1] == 'T' && reqStr[k+2] == 'S' && reqStr[k+3] == 'P' && reqStr[k+4] == '/') {
          while (--k >= i && reqStr[k] == ' ') {} // go back over all spaces before "RTSP/"
          unsigned k1 = k;
          while (k1 > i && reqStr[k1] != '/') --k1;

          // The URL suffix comes from [k1+1,k]
          // Copy "resultURLSuffix":
          if (k - k1 + 1 > resultURLSuffixMaxSize) return 0;    //False; // there's no room

          unsigned n = 0, k2 = k1+1;
          while (k2 <= k) resultURLSuffix[n++] = reqStr[k2++];
          resultURLSuffix[n] = '\0';

          // The URL 'pre-suffix' comes from [i+1,k1-1]
          // Copy "resultURLPreSuffix":
          if ( k1  > resultURLPreSuffixMaxSize +i) return 0; //False; // there's no room
          n = 0; k2 = i + 1;
          while (k2 <= k1 - 1) resultURLPreSuffix[n++] = reqStr[k2++];
          resultURLPreSuffix[n] = '\0';

          i = k + 7; // to go past " RTSP/"
          parseSucceeded = 1;   //True;
          break;
        }
    }

    if (parseSucceeded == 0) return 0;    //False;

    // Look for "CSeq:", skip whitespace,
    // then read everything up to the next \r or \n as 'CSeq':
    parseSucceeded = 0;   //False;
    for (j = i; (int)j < (int)(reqStrSize-5); ++j) {
        if (reqStr[j] == 'C' &&
            reqStr[j+1] == 'S' &&
            reqStr[j+2] == 'e' &&
            reqStr[j+3] == 'q' &&
            reqStr[j+4] == ':') {
            j += 5;
            while (j < reqStrSize && (reqStr[j] ==  ' ' || reqStr[j] == '\t'))
                ++j;
            unsigned n;
            for (n = 0; n < resultCSeqMaxSize-1 && j < reqStrSize; ++n,++j) {
                char c = reqStr[j];
                if (c == '\r' || c == '\n') {
                  parseSucceeded = 1;       //True;
                  break;
                }

                resultCSeq[n] = c;
            }
          resultCSeq[n] = '\0';
          break;
        }
    }
    if (parseSucceeded == 0) return 0;    //False;

    // Also: Look for "Content-Length:" (optional)
    for (j = i; (int)j < (int)(reqStrSize-15); ++j) {
        if (reqStr[j] == 'C' &&
            reqStr[j+1] == 'o' &&
            reqStr[j+2] == 'n' &&
            reqStr[j+3] == 't' &&
            reqStr[j+4] == 'e' &&
            reqStr[j+5] == 'n' &&
            reqStr[j+6] == 't' &&
            reqStr[j+7] == '-' &&
            (reqStr[j+8] == 'L' || reqStr[j+8] == 'l') &&
            reqStr[j+9] == 'e' &&
            reqStr[j+10] == 'n' &&
            reqStr[j+11] == 'g' &&
            reqStr[j+12] == 't' &&
            reqStr[j+13] == 'h' &&
            reqStr[j+14] == ':') {
            j += 15;
            while (j < reqStrSize && (reqStr[j] ==  ' ' || reqStr[j] == '\t'))
                ++j;
            unsigned num;
            if (sscanf(&reqStr[j], "%u", &num) == 1) {
                *contentLength = num;
          }
        }
    }
    return 1; //True;
}

char const* dateHeader() {
    static char date_buf[200];
    time_t tt = time(NULL);
    strftime(date_buf, sizeof date_buf, "Date: %a, %b %d %Y %H:%M:%S GMT\r\n", gmtime(&tt));
    return date_buf;
}

static inline char* getLine(char* startOfLine) {
    char* ptr;
    for (ptr = startOfLine; *ptr != '\0'; ++ptr) {
        // Check for the end of line: \r\n (but also accept \r or \n by itself):
        if (*ptr == '\r' || *ptr == '\n') {
            // We found the end of the line
            if (*ptr == '\r') {
                *ptr++ = '\0';
                if (*ptr == '\n') ++ptr;
            } else {
                *ptr++ = '\0';
            }
            return ptr;
        }
    }
    return NULL;
}
/* This function remove RTSP parameter and only return un-recognized string
 **/
static void parseExtensionString(char const* reqStr, char* resultStr)
{
    char* curline;
    char* nextline;
    char* dup_header = strdup(reqStr);


    curline = dup_header;
    nextline = getLine(curline); // always omit first line(command line)
    while(1){
        curline = nextline;
        if(curline == NULL) break;
        nextline = getLine(curline);
        if(curline[0] == '\0') continue; //blank line
        if(curline[0] == 'C' && curline[1] == 'S' &&
            curline[2] == 'e' && curline[3] == 'q') continue;  //Cseq line
        if(strncasecmp("User-Agent:", curline, strlen("User-Agent:")) == 0) continue;
        if(strncasecmp("Session:", curline, strlen("Session:")) == 0) continue;
        if(strncasecmp("Accept:", curline, strlen("Accept:")) == 0) continue;
        if(strncasecmp("Transport:", curline, strlen("Transport:")) == 0) continue;
        if(strncasecmp("Range:", curline, strlen("Range:")) == 0) continue;
        if(strlen(resultStr) + strlen(curline) + 1 >= 128){
            fprintf(stderr,"Error: extend SET_PARAMETER string truncated!\n");
            break;
        }
        strcat(resultStr, curline);
        strcat(resultStr, "\n");
    }

    free(dup_header);
}

static char const* allowedCommandNames =
    "OPTIONS, DESCRIBE, SETUP, TEARDOWN, PLAY, PAUSE, GET_PARAMETER, SET_PARAMETER";

static char const* allowedCommandNamesGSensor =
    "OPTIONS, SETUP, TEARDOWN, PLAY, PAUSE";

static void handle_cmd_bad(rtsp_client_session_t* s, char const* cseq) {    
    if (s->is_gsensor_stream) {
        // Don't do anything with "cseq", because it might be nonsense
         snprintf((char*)s->respond_buf, sizeof s->respond_buf,
             "RTSP/1.0 400 Bad Request\r\n%sAllow: %s\r\n\r\n",
             dateHeader(), allowedCommandNamesGSensor);
    } else {
       // Don't do anything with "cseq", because it might be nonsense
        snprintf((char*)s->respond_buf, sizeof s->respond_buf,
            "RTSP/1.0 400 Bad Request\r\n%sAllow: %s\r\n\r\n",
            dateHeader(), allowedCommandNames);
   }
   fprintf(stderr,"%s\n",__FUNCTION__);
}

static void handle_cmd_notSupported(rtsp_client_session_t* s,char const* cseq) {    
    if (s->is_gsensor_stream) {
        snprintf((char*)s->respond_buf, sizeof s->respond_buf,
           "RTSP/1.0 405 Method Not Allowed\r\nCSeq: %s\r\n%sAllow: %s\r\n\r\n",
           cseq, dateHeader(), allowedCommandNamesGSensor);
    } else {
        snprintf((char*)s->respond_buf, sizeof s->respond_buf,
           "RTSP/1.0 405 Method Not Allowed\r\nCSeq: %s\r\n%sAllow: %s\r\n\r\n",
           cseq, dateHeader(), allowedCommandNames);
    }
    fprintf(stderr,"%s\n",__FUNCTION__);
}

static void handle_cmd_notFound(rtsp_client_session_t* s, char const* cseq) {
    snprintf((char*)s->respond_buf, sizeof s->respond_buf,
       "RTSP/1.0 404 Stream Not Found\r\nCSeq: %s\r\n%s\r\n",
       cseq, dateHeader());
    fprintf(stderr,"%s\n",__FUNCTION__);

    s->active = 0; // triggers deletion of ourself after responding
}

static void handle_cmd_unsupportedTransport(rtsp_client_session_t* s, char const* cseq) {
    snprintf((char*)s->respond_buf, sizeof s->respond_buf,
        "RTSP/1.0 461 Unsupported Transport\r\nCSeq: %s\r\n%s\r\n",
        cseq, dateHeader());
    fprintf(stderr,"%s\n",__FUNCTION__);

    s->active = 0;  // triggers deletion of ourself after responding
}

static void handle_cmd_OPTIONS(rtsp_client_session_t* s, const char* cseq, const unsigned char is_gsensor_stream) {
    if (is_gsensor_stream) {
        snprintf((char*)s->respond_buf, sizeof s->respond_buf,
         "RTSP/1.0 200 OK\r\nCSeq: %s\r\n%sPublic: %s\r\n\r\n",
         cseq, dateHeader(), allowedCommandNamesGSensor);
    } else {
        snprintf((char*)s->respond_buf, sizeof s->respond_buf,
         "RTSP/1.0 200 OK\r\nCSeq: %s\r\n%sPublic: %s\r\n\r\n",
         cseq, dateHeader(), allowedCommandNames);
    }
    fprintf(stderr,"%s\n",__FUNCTION__);

    s->last_activity = ev_now(s->srv->loop);
}

static void handle_cmd_notEnoughBandwidth(rtsp_client_session_t* s, const char* cseq) {
    snprintf((char*)s->respond_buf, sizeof s->respond_buf,
        "RTSP/1.0 453 Not Enough Bandwidth\r\nCSeq: %s\r\n%s\r\n",
        cseq, dateHeader());
    fprintf(stderr,"%s\n",__FUNCTION__);
}

static void handle_cmd_DESCRIBE(rtsp_client_session_t* s, char const* cseq,
             char const* urlPreSuffix, char const* urlSuffix,
             char const* fullRequestStr) {
    char* sdpDescription = NULL;
    char rtspURL[254] = {0};
    float duration = 0.00;
    //fprintf(stderr,"%s\n",__FUNCTION__);
    do {
        char urlTotalSuffix[RTSP_PARAM_STRING_MAX];
        if (strlen(urlPreSuffix) + strlen(urlSuffix) + 2 > sizeof urlTotalSuffix) {
            handle_cmd_bad(s, cseq);
            break;
        }

        urlTotalSuffix[0] = '\0';
        if (urlPreSuffix[0] != '\0') {
            strcat(urlTotalSuffix, urlPreSuffix);
            strcat(urlTotalSuffix, "/");
        }
        strcat(urlTotalSuffix, urlSuffix);

        // We should really check that the request contains an "Accept:" #####
        // for "application/sdp", because that's what we're sending back #####


        // Begin by looking up the "ServerMediaSession" object for the specified "urlTotalSuffix":
        media_session_t* media_session = s->srv->lookup_media_session(s->srv, urlTotalSuffix);
        if (media_session == NULL) {
            handle_cmd_notFound(s, cseq);
            break;
        }
        s->media_session = media_session;

        // Then, assemble a SDP description for this session:
        sdpDescription = media_session->gen_sdp_description(media_session, duration);
        if (sdpDescription == NULL) {
            // This usually means that a file name that was specified for a
            // "ServerMediaSubsession" does not exist.
            snprintf((char*)s->respond_buf, sizeof s->respond_buf,
                "RTSP/1.0 404 File Not Found, Or In Incorrect Format\r\n"
                "CSeq: %s\r\n"
                "%s\r\n",
                cseq,
                dateHeader());
            break;
        }
        unsigned sdpDescriptionSize = strlen(sdpDescription);


        // Also, generate our RTSP URL, for the "Content-Base:" header
        // (which is necessary to ensure that the correct URL gets used in
        // subsequent "SETUP" requests).
        s->srv->get_rtsp_url(s->srv, media_session, rtspURL, sizeof(rtspURL));

        snprintf((char*)s->respond_buf, sizeof s->respond_buf,
            "RTSP/1.0 200 OK\r\nCSeq: %s\r\n"
            "%s"
            "Content-Base: %s/\r\n"
            "Content-Type: application/sdp\r\n"
            "Content-Length: %d\r\n\r\n"
            "%s",
            cseq,
            dateHeader(),
            rtspURL,
            sdpDescriptionSize,
            sdpDescription);

        s->last_activity = ev_now(s->srv->loop);
    } while (0);
}

static void handle_cmd_SETUP(rtsp_client_session_t* s, char const* cseq,
          char const* urlPreSuffix, char const* urlSuffix,
          char const* fullRequestStr) {

    char const* stream_name = urlPreSuffix;
    media_subsession_t* subsession = NULL;
    gsensor_subsession_t* gsensor_subsession = NULL;

    if (s->is_gsensor_stream) {
        unsigned i = 0;
        for (i = 0; i < GSENSOR_MAX_TRACK; i++) {
            if (s->gsensor_subsessions[i] == NULL) {
                break;
            }
        }
        if (i >= GSENSOR_MAX_TRACK) {
            handle_cmd_bad(s, cseq);
            return;
        }
        gsensor_subsession = gsensor_subsession_create(s->srv);
        if (gsensor_subsession == NULL) {
            handle_cmd_bad(s, cseq);
            return;
        }
        s->gsensor_subsessions[i] = gsensor_subsession;
    } else {
        char const* track_id = urlSuffix;
        media_session_t* media_session = s->srv->lookup_media_session(s->srv, stream_name);

        //fprintf(stderr,"%s\n",__FUNCTION__);
        if(media_session == NULL) {
            handle_cmd_notFound(s, cseq);
            return;
        }
        if(track_id != NULL && track_id[0] != '\0') {
            subsession = media_session->lookup_media_subsession_by_track_id(media_session, track_id);
            if(subsession == NULL) {
                handle_cmd_notFound(s, cseq);
                return;
            } else {
                unsigned i = 0;
                for(i = 0; i < MAX_TRACK; i++) {
                    if(s->subsessions[i] == NULL) {
                        s->subsessions[i] = subsession;
                        break;
                    }
                }
                if(i >= MAX_TRACK) {
                    handle_cmd_bad(s, cseq);
                    return;
                }
            }
        } else {
            handle_cmd_bad(s, cseq);
            return;
        }
    }

    StreamingMode streamingMode;
    char* streamingModeString = NULL; // set when RAW_UDP streaming is specified
    char* clientsDestinationAddressStr;
    unsigned char clientsDestinationTTL;
    unsigned short clientRTPPortNum, clientRTCPPortNum;
    unsigned char rtpChannelId, rtcpChannelId;
    parseTransportHeader(fullRequestStr, &streamingMode, &streamingModeString,
             &clientsDestinationAddressStr, &clientsDestinationTTL,
             &clientRTPPortNum, &clientRTCPPortNum,
             &rtpChannelId, &rtcpChannelId);

    if (streamingMode == RTP_UDP) {
        char dst_str[16] = {'\0'};
        char src_str[16] = {'\0'};
        unsigned short srv_rtp_port = 0;
        unsigned short srv_rtcp_port = 0;
        struct sockaddr_in source_addr;
        unsigned sock_len = sizeof(source_addr);

        if (getsockname(s->client_sock, (struct sockaddr*)&source_addr, &sock_len) < 0) {
            fprintf(stderr, "getsockname() failed %s\n", strerror(errno));
        }
        if (s->is_gsensor_stream) {
            if (gsensor_subsession->setup_udp_transport(gsensor_subsession, s->session_id, s->client_addr.sin_addr,
                            clientRTPPortNum, clientRTCPPortNum,
                            &srv_rtp_port, &srv_rtcp_port,
                            rtsp_client_session_notify_network_error, (void *)s) < 0) {
                handle_cmd_notEnoughBandwidth(s, cseq);
                return;
            }
        } else {
            if (subsession->setup_udp_transport(subsession, s->session_id, s->client_addr.sin_addr,
                            clientRTPPortNum, clientRTCPPortNum,
                            &srv_rtp_port, &srv_rtcp_port,
                            rtsp_client_session_notify_network_error, (void *)s) < 0) {
                handle_cmd_notEnoughBandwidth(s, cseq);
                return;
            }
        }
        //printf("%d-%d, %d-%d\n", clientRTPPortNum, clientRTCPPortNum, srv_rtp_port, srv_rtcp_port);
        snprintf((char*)s->respond_buf, sizeof s->respond_buf,
             "RTSP/1.0 200 OK\r\n"
             "CSeq: %s\r\n"
             "%s"
             "Transport: RTP/AVP;unicast;destination=%s;source=%s;client_port=%d-%d;server_port=%d-%d\r\n"
             "Session: %08X\r\n\r\n",
             cseq,
             dateHeader(),
             inet_ntop(AF_INET, &s->client_addr.sin_addr.s_addr, dst_str, sizeof(dst_str)),
             inet_ntop(AF_INET, &source_addr.sin_addr.s_addr,src_str, sizeof(src_str)),
             (clientRTPPortNum), (clientRTCPPortNum), (srv_rtp_port), (srv_rtcp_port),
             //ntohs(clientRTPPortNum), ntohs(clientRTCPPortNum), ntohs(srv_rtp_port), ntohs(srv_rtcp_port),
             s->session_id);
    } else if (streamingMode == RTP_TCP) {
        char dst_str[16] = {'\0'};
        char src_str[16] = {'\0'};
        struct sockaddr_in source_addr;
        unsigned sock_len = sizeof(source_addr);

        getsockname(s->client_sock, (struct sockaddr*)&source_addr, &sock_len);
        if (s->is_gsensor_stream) {
           if (gsensor_subsession->setup_tcp_transport(gsensor_subsession, s->session_id, s->client_sock,
                rtpChannelId, rtcpChannelId,
                rtsp_client_session_notify_network_error, (void*)s) < 0) {
                handle_cmd_notEnoughBandwidth(s, cseq);
                return;
            }
        } else {
            if (subsession->setup_tcp_transport(subsession, s->session_id, s->client_sock,
                rtpChannelId, rtcpChannelId,
                rtsp_client_session_notify_network_error, (void*)s) < 0) {
                handle_cmd_notEnoughBandwidth(s, cseq);
                return;
            }
        }
        s->rtp_over_tcp = 1;
        snprintf((char*)s->respond_buf, sizeof s->respond_buf,
             "RTSP/1.0 200 OK\r\n"
             "CSeq: %s\r\n"
             "%s"
             "Transport: RTP/AVP/TCP;unicast;destination=%s;source=%s;interleaved=%d-%d\r\n"
             "Session: %08X\r\n\r\n",
             cseq,
             dateHeader(),
             inet_ntop(AF_INET, &s->client_addr.sin_addr.s_addr, dst_str, sizeof(dst_str)),
             inet_ntop(AF_INET, &source_addr.sin_addr.s_addr,src_str, sizeof(src_str)),
             rtpChannelId, rtcpChannelId,
             s->session_id);
    }else {
        handle_cmd_unsupportedTransport(s,  cseq);
    }

    if(s->status == INIT)
        s->status = READY;

    s->last_activity = ev_now(s->srv->loop);
}
/*
*   Parse seek time from PLAY command
*
*/
static void getSeekTime(char* requestString, unsigned int *seek_time)
{
    char seek_str[32] = {'\0'};
    char* range_head, *range_tail;
    int range_size;
    range_head = strstr(requestString,"Range");
    if(range_head != NULL){
        range_head = strstr(range_head, "=");
        if(range_head == NULL) return ;
        range_head ++;  // omit '='
        while(*range_head == ' ') range_head++; // omit spaces
        range_tail = strstr(range_head,"-");
        if(range_tail == NULL) return ;
        range_size = (range_tail - range_head)/sizeof(char);
        strncpy(seek_str, range_head, range_size);
        *seek_time = (unsigned int)(1000*atof(seek_str));
    }
}

static void handle_cmd_gsensor_PLAY(rtsp_client_session_t* s, gsensor_subsession_t* subsession, char const* cseq,
            char const* fullRequestStr) {
    char const* rtp_info_fmt =
        "%s" // "RTP-Info:", plus any preceding rtpInfo items
        "%s" // comma separator, if needed
        "url=%s/%s"
        ";seq=%d"
        ";rtptime=%u"
        ;
    char rtsp_url[64] = {'\0'};
    unsigned rtp_info_fmt_len = strlen(rtp_info_fmt);
    char* rtp_info = strdup("RTP-Info: ");
    unsigned i = 0;
    unsigned num_rtp_info_items = 0;

    s->srv->get_rtsp_url(s->srv, NULL, rtsp_url, sizeof(rtsp_url));
    printf("%s get_rtsp_url: %s\n", __func__, rtsp_url);

    for (i = 0; i < GSENSOR_MAX_TRACK; i++) {
        if (s->gsensor_subsessions[i] != NULL && (subsession == NULL || s->gsensor_subsessions[i] == subsession)) {
            unsigned short rtp_seq = 0;
            unsigned rtp_timestamp = 0;
            unsigned rtp_info_len = 0;
            const char* url_suffix = "track0";
            char* prev_rtp_info = rtp_info;

            s->gsensor_subsessions[i]->start_stream(s->gsensor_subsessions[i], s->session_id, &rtp_seq, &rtp_timestamp, RTSP_PLAY_LIVE);
            rtp_info_len = rtp_info_fmt_len
                        + strlen(prev_rtp_info)
                        + 1
                        + strlen(rtsp_url) + strlen(url_suffix)
                        + 5
                        + 10
                        + 2;
            rtp_info = (char*)malloc(rtp_info_len);
            sprintf(rtp_info, rtp_info_fmt,
                prev_rtp_info,
                (num_rtp_info_items++ == 0) ? "" : ",",
                rtsp_url, url_suffix,
                rtp_seq,
                rtp_timestamp);
            free(prev_rtp_info);
        }
    }

    if (s->status == READY) {
        fprintf(stderr, "[handle_cmd_gsensor_PLAY]\n");
        AmbaStream_send_PbCmd(AMBASTREAM_GSENSOR_PLAY, 0, NULL, 0, 0);
    }

    s->status = PLAYING;
    if (num_rtp_info_items == 0) {
        rtp_info[0] = '\0';
    } else {
        unsigned rtp_info_len = strlen(rtp_info);
        rtp_info[rtp_info_len] = '\r';
        rtp_info[rtp_info_len] = '\n';
        rtp_info[rtp_info_len] = '\0';
    }
//done:
    snprintf((char*)s->respond_buf, sizeof(s->respond_buf),
        "RTSP/1.0 200 OK\r\n"
        "CSeq: %s\r\n"
        "%s"
        "Session: %08X\r\n"
        "%s\r\n\r\n",
        cseq,
        dateHeader(),
        s->session_id,
        rtp_info);

    free(rtp_info);
    s->last_activity = ev_now(s->srv->loop);
}

static void handle_cmd_gsensor_PAUSE(rtsp_client_session_t* s, gsensor_subsession_t* subsession, char const* cseq) {
    int i = 0;

    for (i = 0; i < GSENSOR_MAX_TRACK; i++) {
        if (s->gsensor_subsessions[i] != NULL && (subsession == NULL || s->gsensor_subsessions[i] == subsession)) {
            s->gsensor_subsessions[i]->pause_stream(s->gsensor_subsessions[i], s->session_id, RTSP_PLAY_STOP);
        }
    }
    usleep(60000);
    snprintf((char*)s->respond_buf, sizeof s->respond_buf,
       "RTSP/1.0 200 OK\r\nCSeq: %s\r\n%sSession: %08X\r\n\r\n",
       cseq, dateHeader(), s->session_id);
    s->last_activity = ev_now(s->srv->loop);
}

static void handle_cmd_gsensor_TEARDOWN(rtsp_client_session_t* s, gsensor_subsession_t* subsession, char const* cseq) {
    int i = 0;

    for(i = 0; i < GSENSOR_MAX_TRACK; i++) {
        if(s->gsensor_subsessions[i] != NULL && (subsession == NULL || s->gsensor_subsessions[i] == subsession)) {
            if (s->gsensor_subsessions[i]->teardown(s->gsensor_subsessions[i], s->session_id) <= 0) {
                s->gsensor_subsessions[i] = NULL;
            }
            if (s->gsensor_subsessions[i] != NULL) {
                fprintf(stderr,"There is more than one client connecting! " \
                    "System might be abnormal![%d]\n",__LINE__);
            }
        }
    }
    snprintf((char*)s->respond_buf, sizeof s->respond_buf,
        "RTSP/1.0 200 OK\r\nCSeq: %s\r\n%s\r\n",
        cseq, dateHeader());
    s->active = 0; // triggers deletion of ourself after responding
    s->last_activity = ev_now(s->srv->loop);
}

static void handle_cmd_within_gsensor_session(rtsp_client_session_t* rtsp_s, char const* cmdName,
              char const* urlPreSuffix, char const* urlSuffix,
              char const* cseq, char const* fullRequestStr)
{
    if (rtsp_s->status== INIT) { // There wasn't a previous SETUP!
        handle_cmd_notSupported(rtsp_s, cseq);
        return;
    }
    if (strcmp(cmdName, "TEARDOWN") == 0) {
        handle_cmd_gsensor_TEARDOWN(rtsp_s, NULL, cseq);
    } else if (strcmp(cmdName, "PLAY") == 0) {
        handle_cmd_gsensor_PLAY(rtsp_s, NULL, cseq, fullRequestStr);
    } else if (strcmp(cmdName, "PAUSE") == 0) {
        handle_cmd_gsensor_PAUSE(rtsp_s, NULL, cseq);
    } else {
        handle_cmd_notSupported(rtsp_s, cseq);
    }
}

static void handle_cmd_PLAY(rtsp_client_session_t* s, media_subsession_t* subsession, char const* cseq,
            char const* fullRequestStr) {
    // Create a "RTP-Info:" line.  It will get filled in from each subsession's state:
    char const* rtp_info_fmt =
        "%s" // "RTP-Info:", plus any preceding rtpInfo items
        "%s" // comma separator, if needed
        "url=%s/%s"
        ";seq=%d"
        ";rtptime=%u"
        ;
    char rtsp_url[64] = {'\0'};
    unsigned rtp_info_fmt_len = strlen(rtp_info_fmt);
    char* rtp_info = strdup("RTP-Info: ");
    unsigned i = 0;
    unsigned num_rtp_info_items = 0;
    unsigned int seek_time = 0xFFFFFFFF;

    //fprintf(stderr,"%s\n",__FUNCTION__);    
    s->srv->get_rtsp_url(s->srv, s->media_session, rtsp_url, sizeof(rtsp_url));
    getSeekTime((char *)fullRequestStr, &seek_time);// TODO: reply error if seek time illeagal
    for(i = 0; i < MAX_TRACK; i++) {
        if(s->subsessions[i] != NULL && (subsession == NULL || s->subsessions[i] == subsession)) {
            unsigned short rtp_seq = 0;
            unsigned rtp_timestamp = 0;
            unsigned rtp_info_len = 0;
            const char* url_suffix = s->subsessions[i]->get_track_id(s->subsessions[i]);
            char* prev_rtp_info = rtp_info;

            // live streaming
            s->subsessions[i]->start_stream(s->subsessions[i], s->session_id, &rtp_seq, &rtp_timestamp, RTSP_PLAY_LIVE);
            rtp_info_len = rtp_info_fmt_len
                        + strlen(prev_rtp_info)
                        + 1
                        + strlen(rtsp_url) + strlen(url_suffix)
                        + 5
                        + 10
                        + 2;
            rtp_info = (char*)malloc(rtp_info_len);
            sprintf(rtp_info, rtp_info_fmt,
                prev_rtp_info,
                (num_rtp_info_items++ == 0) ? "" : ",",
                rtsp_url, url_suffix,
                rtp_seq,
                rtp_timestamp);
            free(prev_rtp_info);
        }
    }

    if (s->status == READY) {
        fprintf(stderr, "[handle_cmd_live_PLAY]\n");
        AmbaStream_send_PbCmd(AMBASTREAM_LIVE_PLAY, 0, NULL, 0, 0);
    }
    s->status = PLAYING;

    if (num_rtp_info_items == 0) {
        rtp_info[0] = '\0';
    } else {
        unsigned rtp_info_len = strlen(rtp_info);
        rtp_info[rtp_info_len] = '\r';
        rtp_info[rtp_info_len] = '\n';
        rtp_info[rtp_info_len] = '\0';
    }
//done:
    snprintf((char*)s->respond_buf, sizeof s->respond_buf,
        "RTSP/1.0 200 OK\r\n"
        "CSeq: %s\r\n"
        "%s"
        "Range: npt=%.3f-\r\n"
        "Session: %08X\r\n"
        "%s\r\n\r\n",
        cseq,
        dateHeader(),
        (float)seek_time/(float)1000,
        s->session_id,
        rtp_info);
    free(rtp_info);
    s->last_activity = ev_now(s->srv->loop);
}

static void handle_cmd_PAUSE(rtsp_client_session_t* s, media_subsession_t* subsession, char const* cseq) {
    int i = 0;

    //fprintf(stderr,"%s\n",__FUNCTION__);
    for(i = 0; i < MAX_TRACK; i++) {
        if(s->subsessions[i] != NULL && (subsession == NULL || s->subsessions[i] == subsession)) {
            if(strncmp(s->media_session->get_stream_name(s->media_session), DEFAULT_VIEWFINDER_STREAM, strlen(DEFAULT_VIEWFINDER_STREAM)) == 0) { //live streaming
                s->subsessions[i]->pause_stream(s->subsessions[i], s->session_id, RTSP_PLAY_STOP);
            }
        }
    }
    usleep(60000);
    snprintf((char*)s->respond_buf, sizeof s->respond_buf,
       "RTSP/1.0 200 OK\r\nCSeq: %s\r\n%sSession: %08X\r\n\r\n",
       cseq, dateHeader(), s->session_id);
    s->last_activity = ev_now(s->srv->loop);
}

static void handle_cmd_TEARDOWN(rtsp_client_session_t* s, media_subsession_t* subsession, char const* cseq) {
    int i = 0;

    //fprintf(stderr,"%s\n",__FUNCTION__);
    for(i = 0; i < MAX_TRACK; i++) {
        if(s->subsessions[i] != NULL && (subsession == NULL || s->subsessions[i] == subsession)) {
            if(s->subsessions[i]->teardown(s->subsessions[i], s->session_id)<=0) {
                s->subsessions[i] = NULL;
            }
            if(s->subsessions[i] != NULL) {
                fprintf(stderr,"There is more than one client connecting! " \
                    "System might be abnormal![%d]\n",__LINE__);
            }
        }
    }
    snprintf((char*)s->respond_buf, sizeof s->respond_buf,
        "RTSP/1.0 200 OK\r\nCSeq: %s\r\n%s\r\n",
        cseq, dateHeader());

    s->active = 0; // triggers deletion of ourself after responding
    s->last_activity = ev_now(s->srv->loop);
}

static void handle_cmd_GET_PARAMETER(rtsp_client_session_t* s, media_subsession_t* subsession, char const* cseq,
              char const* fullRequestStr) {
    // By default, we implement "GET_PARAMETER" just as a 'keep alive', and send back an empty response.
    snprintf((char*)s->respond_buf, sizeof s->respond_buf,
       "RTSP/1.0 200 OK\r\nCSeq: %s\r\n%sSession: %08X\r\n\r\n",
       cseq, dateHeader(), s->session_id);
    //fprintf(stderr,"%s\n",__FUNCTION__);

    s->last_activity = ev_now(s->srv->loop);
}

static void handle_cmd_SET_PARAMETER(rtsp_client_session_t* s, media_subsession_t* subsession, char const* cseq,
              char const* fullRequestStr) {
    /*
     * For Ambarella RTSP extension, we remove all RTSP-standard parameter and
     * bypass the un-recognized parameter to rtos
     **/
    char extensionStr[128] = {0};
    char responseStr[128] = {0};
    parseExtensionString((const char*) s->request_buf, extensionStr);
    AmbaStream_send_PbCmd(AMBASTREAM_SET_PARAMETER, (void*)extensionStr, NULL, 128, responseStr);

    snprintf((char*)s->respond_buf, sizeof s->respond_buf,
       "RTSP/1.0 200 OK\r\nCSeq: %s\r\n%sSession: %08X\r\n%s\r\n\r\n",
       cseq, dateHeader(), s->session_id, responseStr);
    //fprintf(stderr,"%s\n",__FUNCTION__);

    s->last_activity = ev_now(s->srv->loop);
}

static void handle_cmd_within_session(rtsp_client_session_t* rtsp_s, char const* cmdName,
              char const* urlPreSuffix, char const* urlSuffix,
              char const* cseq, char const* fullRequestStr) {
  // This will either be:
  // - an operation on the entire server, if "urlPreSuffix" is "", and "urlSuffix" is "*" (i.e., the special "*" URL), or
  // - a non-aggregated operation, if "urlPreSuffix" is the session (stream)
  //   name and "urlSuffix" is the subsession (track) name, or
  // - an aggregated operation, if "urlSuffix" is the session (stream) name,
  //   or "urlPreSuffix" is the session (stream) name, and "urlSuffix" is empty,
  //   or "urlPreSuffix" and "urlSuffix" are both nonempty, but when concatenated, (with "/") form the session (stream) name.
  // Begin by figuring out which of these it is:
    media_subsession_t* subsession;
    if (urlPreSuffix[0] == '\0' && urlSuffix[0] == '*' && urlSuffix[1] == '\0') {
        // An operation on the entire server.  This works only for GET_PARAMETER and SET_PARAMETER:
        if (strcmp(cmdName, "GET_PARAMETER") == 0) {
            handle_cmd_GET_PARAMETER(rtsp_s, NULL, cseq, fullRequestStr);
        } else if (strcmp(cmdName, "SET_PARAMETER") == 0) {
            handle_cmd_SET_PARAMETER(rtsp_s, NULL, cseq, fullRequestStr);
        } else {
            handle_cmd_notSupported(rtsp_s, cseq);
        }
        return;
    } else if (rtsp_s->status== INIT) { // There wasn't a previous SETUP!
        handle_cmd_notSupported(rtsp_s, cseq);
        return;
    } else if (urlSuffix[0] != '\0' && strcmp(rtsp_s->media_session->get_stream_name(rtsp_s->media_session), urlPreSuffix) == 0) {
        // Non-aggregated operation.
        // Look up the media subsession whose track id is "urlSuffix":
        int i = 0;

        for(i = 0; i < MAX_TRACK; i++) {
            subsession = rtsp_s->subsessions[i];
            if(subsession != NULL && strcmp(subsession->get_track_id(subsession), urlSuffix) == 0)
                break;
        }
        if (subsession == NULL) { // no such track!
            handle_cmd_notFound(rtsp_s, cseq);
            return;
        }
    } else if (strcmp(rtsp_s->media_session->get_stream_name(rtsp_s->media_session), urlSuffix) == 0 ||
        (strncmp(rtsp_s->media_session->get_stream_name(rtsp_s->media_session), urlPreSuffix, strlen(urlPreSuffix)) == 0 &&
        rtsp_s->media_session->get_stream_name(rtsp_s->media_session)[strlen(urlPreSuffix)] == '/' &&
        strncmp(rtsp_s->media_session->get_stream_name(rtsp_s->media_session)+ strlen(urlPreSuffix)+1, urlSuffix, strlen(urlSuffix)) == 0) ||
        (urlSuffix[0] == '\0' && strcmp(rtsp_s->media_session->get_stream_name(rtsp_s->media_session), urlPreSuffix) == 0)) {
        // Aggregated operation
        subsession = NULL;
    } else if (urlPreSuffix[0] != '\0' && urlSuffix[0] != '\0') {
        handle_cmd_notFound(rtsp_s, cseq);
        return;
    } else {
        handle_cmd_notFound(rtsp_s, cseq);
        return;
    }

    if (strcmp(cmdName, "TEARDOWN") == 0) {
        handle_cmd_TEARDOWN(rtsp_s, subsession, cseq);
    } else if (strcmp(cmdName, "PLAY") == 0) {
        handle_cmd_PLAY(rtsp_s, subsession, cseq, fullRequestStr);
    } else if (strcmp(cmdName, "PAUSE") == 0) {
        handle_cmd_PAUSE(rtsp_s, subsession, cseq);
    } else if (strcmp(cmdName, "GET_PARAMETER") == 0) {
        handle_cmd_GET_PARAMETER(rtsp_s, subsession, cseq, fullRequestStr);
    } else if (strcmp(cmdName, "SET_PARAMETER") == 0) {
        handle_cmd_SET_PARAMETER(rtsp_s, subsession, cseq, fullRequestStr);
    }
}
void rtsp_client_session_reset_timeout(struct rtsp_client_session* s)
{
    if(s != NULL) {
        s->last_activity = ev_now(s->srv->loop);
    }
}
static void timer_cb(struct ev_loop* loop, ev_timer *w, int revents)
{
    rtsp_client_session_t* session = (rtsp_client_session_t*)w->data;

    // calculate when the timeout would happen
    ev_tstamp after = session->last_activity - ev_now(session->srv->loop) + timeout;

//fprintf(stderr,"rtsp_client_session Timer Callback!!, after = %d\n", (int)after);
    // if negative, it means we the timeout already occurred
    if (after < 0.) {
        fprintf(stderr,"rtsp_client_session Time out!!\n");
        // timeout occurred, take action
        session->active = 0;
        rtsp_client_session_release(session);
        w->data = NULL;
    } else {
//fprintf(stderr,"rtsp_client_session: Normal!!\n");
        // callback was invoked, but there was some recent
        // activity. simply restart the timer to time out
        // after "after" seconds, which is the earliest time
        // the timeout can occur.
        ev_timer_set (w, after, 0.);
        ev_timer_start (session->srv->loop, w);
    }
}

static void rtsp_client_session_notify_network_error(void* session_ptr)
{
    rtsp_client_session_t *session = (rtsp_client_session_t *) session_ptr;
    ev_feed_event(session->srv->loop, &session->incoming_watcher, EV_ERROR);
}

static inline void rtsp_client_session_reset_buf(rtsp_client_session_t* session){
    if(session == NULL) {
        return;
    }
    session->request_bytes = 0;
    session->request_buf_left = RTSP_BUFFER_SIZE;
    session->last_CRLF = (unsigned char*)(&(session->request_buf[0]) - 2);
}

/**
 *  [rtp over tcp]
 *  We only recognize rtcp pkt type here for now
 *  What to do base on the rtcp pkt is left as todo
 */
static void rtcp_handler(uint8_t *data, uint32_t len) {
    uint32_t rtcp_size = 0;
    while(len > sizeof(rtcp_header)) {
        rtcp_header *rtcp = (rtcp_header*)data;
        rtcp_size = (ntohs(rtcp->length) + 1 )<<2;
        if(rtcp_size > len) {
            fprintf(stderr, "[RTCP] malformed packet\n");
            return;
        }
        switch(rtcp->pt){
            case BYE:
                break;
            case SR:
                break;
            case RR:
                //parse_receiver_report(data  4, rtcp->count);
                break;
            case SDES:
                break;
            default:
                printf("unregonized \n\r");
                break;
        }
        len -= rtcp_size;
        data += rtcp_size;
    }
}

static void rtsp_client_session_incomming_handler(struct ev_loop* loop, ev_io* w, int revent)
{
    int bytes_read = 0;

    fprintf(stderr,"\n\nrtsp_client_session_incomming_handler______________________________\n");

    rtsp_client_session_t* session = (rtsp_client_session_t*)w->data;
    if (session == NULL) {
        return;
    }

    if (EV_ERROR & revent) {
        fprintf(stderr,"got error revent:0x%x\n",revent);
        session->active = 0;
        rtsp_client_session_release(session);
        w->data = NULL;
        return;
    }

    bytes_read = recv(w->fd, &session->request_buf[session->request_bytes], session->request_buf_left, 0);
    session->recursion_count++;

    do {
        unsigned char end_of_msg = 0;
        unsigned char* ptr = &session->request_buf[session->request_bytes];

        if(bytes_read < 0) {
            fprintf(stderr, "recv error: %s!!!\n", strerror(errno));
            session->active = 0;
            break;
        }
        if(bytes_read == 0) {
            fprintf(stderr, "client close!!!\n");
            session->active = 0;
            break;
        }
        if(bytes_read >= session->request_buf_left) {
            fprintf(stderr, "request too large!!! read %d bytes, left %u bytes\n",
            bytes_read, session->request_buf_left);
            session->active = 0;
            break;
        }
#if 1
        ptr[bytes_read] = '\0';
        //fprintf(stderr, "read %d new bytes:%s\n", bytes_read, ptr);
#endif
        session->request_buf_left -= bytes_read;
        session->request_bytes += bytes_read;

        //In RTP over TCP interleaved mode, RTCP packet may come from RTSP port(i.e. VLC)
        if(session->rtp_over_tcp != 0 && session->request_bytes >= 4){
            //int channel_id;
            int length;
            if(session->request_buf[0] == 0x24){//check the magic = 0x24 for RTP interleaved frame
                //channel_id = (int) session->request_buf[1];
                length = ((int) session->request_buf[2] << 8) + (int) session->request_buf[3];
                //printf("Got inteleaved-RTCP, channel=%d, len=%d\n\r", channel_id, length);
                if(session->request_bytes < length + 4){
                    //printf("request_bytes(%d) < length(%d) , need further msg\n\r", session->request_bytes, length);
                }else if(session->request_bytes == length + 4){
                    // if it is tcp mode  interleave mode, check RTCP packet
                    rtcp_handler(session->request_buf+4, length);
                    //printf("Clean the buffer\n\r");
                    rtsp_client_session_reset_buf(session);
                    break;
                }else{ //we got redundant
                    int data_len = session->request_bytes - length - 4;
                    rtcp_handler(session->request_buf+4, length);
                    //printf("There is redundant data(%d), first byte=%x\n\r", data_len, session->request_buf[data_len]);
                    rtsp_client_session_reset_buf(session);
                    memcpy(session->request_buf, session->request_buf + length + 4, data_len);
                    bytes_read = data_len;
                    session->request_buf_left -= bytes_read;
                    session->request_bytes += bytes_read;
                }
            }
        }


        // Look for the end of the message: <CR><LF><CR><LF>
        unsigned char *tmpPtr = session->last_CRLF + 2;
        if (tmpPtr < &session->request_buf[0]) {
            tmpPtr = &session->request_buf[0];
        }

        while (tmpPtr < &ptr[bytes_read - 1]) {
            if (*tmpPtr == '\r' && *(tmpPtr+1) == '\n') {
                if (tmpPtr - session->last_CRLF == 2) { // This is it:
                    end_of_msg = 1;
                    break;
                }
                session->last_CRLF= tmpPtr;
            }
            ++tmpPtr;
        }

        if (end_of_msg == 0){
            // subsequent reads will be needed to complete the request
            fprintf(stderr, "need futher msg!!\n");
            break;
        }
#if 0
        fprintf(stderr, "got end of msg\n");
#endif
        if(1){
            session->request_buf[session->request_bytes] = '\0';
            char cmdName[RTSP_PARAM_STRING_MAX];
            char urlPreSuffix[RTSP_PARAM_STRING_MAX];
            char urlSuffix[RTSP_PARAM_STRING_MAX];
            char cseq[RTSP_PARAM_STRING_MAX];
            unsigned contentLength = 0;
            if (parseRTSPRequestString((char*)session->request_buf, session->request_bytes,
                cmdName, sizeof cmdName,urlPreSuffix, sizeof urlPreSuffix, urlSuffix,
                sizeof urlSuffix, cseq, sizeof cseq, &contentLength) == 1) {
#ifdef DEBUG_SERVER
                fprintf(stderr, "parseRTSPRequestString() succeeded, returning cmdName \"%s\", " \
                "urlPreSuffix \"%s\", urlSuffix \"%s\", CSeq \"%s\", Content-Length %u, with %ld bytes " \
                "following the message.\n",
                cmdName, urlPreSuffix, urlSuffix, cseq, contentLength, ptr + bytes_read - (tmpPtr + 2));
#endif
                // If there was a "Content-Length:" header,
                //then make sure we've received all of the data that it specified:
                if (ptr + bytes_read < tmpPtr + 2 + contentLength) {
                    break; // we still need more data; subsequent reads will give it to us
                }

                if (strcmp(cmdName, "OPTIONS") == 0) {
                    if (strcmp(urlSuffix, DEFAULT_GSENSOR_STREAM) == 0) {
                        handle_cmd_OPTIONS(session, cseq, 1);
                    } else {
                        handle_cmd_OPTIONS(session, cseq, 0);
                    }
                } else if (strcmp(cmdName, "DESCRIBE") == 0) {
                    if (strcmp(urlSuffix, DEFAULT_GSENSOR_STREAM) == 0) {
                        handle_cmd_notSupported(session, cseq);
                    } else {
                        handle_cmd_DESCRIBE(session, cseq, urlPreSuffix,
                            urlSuffix, (char const*)session->request_buf);
                    }
                } else if (strcmp(cmdName, "SETUP") == 0) {
                    if (strcmp(urlSuffix, DEFAULT_GSENSOR_STREAM) == 0) {
                        session->is_gsensor_stream = 1;
                    } else {
                        session->is_gsensor_stream = 0;
                    }
                    printf("%s for %s stream\n", cmdName, session->is_gsensor_stream ? "gsensor" : "live");
                    handle_cmd_SETUP(session, cseq, urlPreSuffix, urlSuffix,
                        (char const*)session->request_buf);
                } else if (strcmp(cmdName, "TEARDOWN") == 0
                    || strcmp(cmdName, "PLAY") == 0
                    || strcmp(cmdName, "PAUSE") == 0
                    || (strcmp(cmdName, "GET_PARAMETER") == 0 && session->is_gsensor_stream == 0)
                    || (strcmp(cmdName, "SET_PARAMETER") == 0 && session->is_gsensor_stream == 0)) {
                    printf("%s for %s stream\n", cmdName, session->is_gsensor_stream ? "gsensor" : "live");
                    if (session->is_gsensor_stream) {
                        handle_cmd_within_gsensor_session(session, cmdName, urlPreSuffix, urlSuffix, cseq,
                            (char const*)session->request_buf);
                    } else {
                        handle_cmd_within_session(session, cmdName, urlPreSuffix, urlSuffix, cseq,
                            (char const*)session->request_buf);
                    }
                } else {
                    handle_cmd_notSupported(session, cseq);
                }
            } else {
#ifdef DEBUG_SERVER
                fprintf(stderr, "bad request\n");
#endif
            }

        }
        write(session->client_sock, (const char*)session->respond_buf, strlen((char*)session->respond_buf));
        rtsp_client_session_reset_buf(session);
    } while(0);
    --session->recursion_count;
    if(session->active == 0) {
#if 0
        if(session->recursion_count > 0) {
        close(session->client_sock);
        } else {
        rtsp_client_session_release(session);
        }
#else
        rtsp_client_session_release(session);
        w->data = session = NULL;
#endif
    }
}

rtsp_client_session_t* rtsp_client_session_create(rtsp_server_t* srv, unsigned session_id, int client_sock,
                            struct sockaddr_in client_addr)
{
    rtsp_client_session_t* session;
    ev_io *io_watcher = NULL;
    ev_timer *timer_watcher = NULL;

    if(srv == NULL) {
        return NULL;
    }
    session = (rtsp_client_session_t*)malloc(sizeof(rtsp_client_session_t));
    if(session == NULL) {
        return NULL;
    }
    memset(session, 0, sizeof(rtsp_client_session_t));
    session->srv =  srv;
    session->session_id = session_id;
    session->client_sock = client_sock;
    session->client_addr = client_addr;
    session->recursion_count = 0;
    session->active = 1;
    rtsp_client_session_reset_buf(session);
    session->incoming_watcher.data = session;
    io_watcher = &session->incoming_watcher;
    ev_io_init(io_watcher, rtsp_client_session_incomming_handler, client_sock, EV_READ);
    ev_io_start(srv->loop, &session->incoming_watcher);

    session->timer.data = session;
    session->last_activity = ev_now(session->srv->loop);
    timer_watcher = &session->timer;
    ev_init(timer_watcher, timer_cb);
    timer_cb(session->srv->loop, &session->timer, 0);

    fprintf(stderr, ">>> new rtsp session\n");
    return session;
}

void rtsp_client_session_release(rtsp_client_session_t* session)
{
    int i = 0;
    int subsession_all_released = 1;
    
    if (session == NULL) {
        return;
    }

    if (session->is_gsensor_stream) {
        fprintf(stderr, ">>> release gsensor rtsp session\n");
        for (i = 0; i < GSENSOR_MAX_TRACK; i++) {
            if (session->gsensor_subsessions[i] != NULL) {
                if (session->gsensor_subsessions[i]->teardown(session->gsensor_subsessions[i], session->session_id) <= 0) {
                    session->gsensor_subsessions[i]->release(session->gsensor_subsessions[i]);
                    session->gsensor_subsessions[i] = NULL;
                }
                if (session->gsensor_subsessions[i] != NULL) {
                    subsession_all_released = 0;
                    fprintf(stderr,"There is more than one gsensor client connecting! " \
                        "System might be abnormal![%d]\n",__LINE__);
                }
            }
        }
        if (subsession_all_released == 1) {
            AmbaStream_send_PbCmd(AMBASTREAM_GSENSOR_STOP, 0, NULL, 0, 0);
        }
    } else {
        fprintf(stderr, ">>> release rtsp session\n");
        for (i = 0; i < MAX_TRACK; i++) {
            if (session->subsessions[i] != NULL) {
                if (session->subsessions[i]->teardown(session->subsessions[i], session->session_id) <= 0) {
                    session->subsessions[i] = NULL;
                }
                if (session->subsessions[i] != NULL) {
                    subsession_all_released = 0;
                    fprintf(stderr,"There is more than one client connecting! " \
                        "System might be abnormal![%d]\n",__LINE__);
                }
            }
        }
        //if (subsession_all_released == 1) {
            AmbaStream_send_PbCmd(AMBASTREAM_LIVE_STOP, 0, NULL, 0, 0);
        //}
    }

    ev_io_stop(session->srv->loop, &session->incoming_watcher);
    ev_timer_stop(session->srv->loop, &session->timer);
    close(session->client_sock);
    session->srv->connection_count--;
    free(session);
    if (session->is_gsensor_stream) {
        fprintf(stderr, "<<< release gsensor rtsp session finish\n");
    } else {
        fprintf(stderr, "<<< release rtsp session finish\n");
    }
}

