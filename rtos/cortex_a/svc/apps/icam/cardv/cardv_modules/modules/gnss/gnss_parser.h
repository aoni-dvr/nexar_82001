#ifndef __GNSS_PARSER_H__
#define __GNSS_PARSER_H__

#define MAX_GNSS_RAW (80)
#define MAX_SAT_NUM (20)

typedef struct _sat_info_s_ {
    int PRN;	/* PRNs of satellite */
    int elevation;	/* elevation of satellite */
    int azimuth;	/* azimuth */
    double ss;	/* signal-to-noise ratio (dB) */
} sat_info_s;

typedef enum _gnss_e_ {
    GNSS_GPS = 0,
    GNSS_BEIDOU,
    GNSS_GALILEO,
    GNSS_QLONASS,
    GNSS_SBAS,
    GNSS_IMES,
    GNSS_QZSS,
    GNSS_NUM
} gnss_e;

typedef enum {
    ED_HHMMSS = 1,
    ED_STATUS,
    ED_LATITUDE,
    ED_LATITUDE_TYPE,
    ED_LONGTITUDE,
    ED_LONGTITUDE_TYPE,
    ED_SPEED,
    ED_COURSE,
    ED_DDMMYY,
    ED_NUM
} ed_filed;

typedef struct _gnss_data_s_ {
    int         valid;
    int         m_bLocal;
    double      m_rawLat;
    double      m_dLat;
    double      m_rawLon;
    double      m_dLon;
    double      m_nSpeed;
    int         m_nAngle;
    int         m_nYear;
    int         m_nMonth;
    int         m_nDay;
    int         m_nHour;
    int         m_nMinute;
    int         m_nSecond;
    int         m_nMSecond;
    int         m_nSatNumInUse;
    double      m_nHdop;
    int         m_nSatNumInView[GNSS_NUM];
    double      m_nAltitude;
    double      m_nGeoidHeight;
    sat_info_s  m_Sat[GNSS_NUM][MAX_SAT_NUM];
    int         ready;
    char        RawRmc[MAX_GNSS_RAW];
    char        RmcFields[12][32];
} gnss_data_s;

int gnss_parser_init(void);
int gnss_parser_get_data(gnss_data_s *data);
int gnss_parser_reset_data(void);
int gnss_parser_get_connected(void);
int gnss_parser_set_connected(int connected);
int gnss_parser_dispatch(const char *input);
void gnss_parser_debug_result(void *pFile);
int gnss_parser_set_cold_boot_response(int flag);
int gnss_parser_get_cold_boot_response(void);

#endif//__GNSS_PARSER_H__

