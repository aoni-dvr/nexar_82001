#include "gnss_parser.h"
#include "cardv_include.h"

#define DEBUG_TAG "[gnss_parser]"

#include "gnss_parser_platform.c"

static int gsv_parse(const char *gpgsv);
static int gga_parse(const char *gpgga);
static int rmc_parse(const char *gprmc);

static gnss_data_s gnss_data;
int gnss_parser_init(void)
{
    memset(&gnss_data, 0, sizeof(gnss_data_s));
    if (gnss_parser_init_muxer() < 0) {
        return -1;
    }

    return 0;
}

int gnss_parser_get_data(gnss_data_s *data)
{
    gnss_parser_get_muxer();
    memcpy(data, &gnss_data, sizeof(gnss_data_s));
    gnss_parser_release_muxer();

    return 0;
}

int gnss_parser_reset_data(void)
{
    gnss_parser_get_muxer();
    memset(&gnss_data, 0, sizeof(gnss_data_s));
    gnss_parser_release_muxer();

    return 0;
}

int gnss_parser_get_connected(void)
{
    int connected = 0;

    gnss_parser_get_muxer();
    connected = gnss_data.ready;
    gnss_parser_release_muxer();

    return connected;
}

int gnss_parser_set_connected(int connected)
{
    gnss_parser_get_muxer();
    gnss_data.ready = connected;
    gnss_parser_release_muxer();

    return 0;
}

static int response_flag = 0;
int gnss_parser_set_cold_boot_response(int flag)
{
    response_flag = flag;
    return 0;
}

int gnss_parser_get_cold_boot_response(void)
{
    return response_flag;
}

int gnss_parser_dispatch(const char *input)
{
    if (input == NULL || strlen(input) <= 0) {
        debug_line(DEBUG_TAG"%s invalid input", __func__);
        return -1;
    }

    if (strncmp(input + 3, "RMC", strlen("RMC")) == 0) {
        gnss_parser_get_muxer();
        rmc_parse(input);
        snprintf(gnss_data.RawRmc, sizeof(gnss_data.RawRmc) - 1, "%s", input);
        gnss_parser_release_muxer();
    } else if (strncmp(input + 3, "GGA", strlen("GGA")) == 0) {
        gnss_parser_get_muxer();
        gga_parse(input);
        gnss_parser_release_muxer();
    } else if (strncmp(input + 3, "GSV", strlen("GSV")) == 0) {
        gnss_parser_get_muxer();
        gsv_parse(input);
        gnss_parser_release_muxer();
    } else if (strncmp(input + 3, "TXT,02,01,02", strlen("TXT,02,01,02")) == 0) {
        response_flag = 1;
    }

    return 0;
}

static int gnss_parser_check_checksum(const char *sentence)
{
    unsigned int i = 0, result = 0;
    int xing_index = -1;
    char checksum[32] = {0};
    int checksum_value = 0;

    if (sentence == NULL || strlen(sentence) <= 2) {
        debug_line(DEBUG_TAG"[check fail]%s, %d", __func__, __LINE__);
        return 0;
    }

    for (result = sentence[1], i = 2; i < strlen(sentence); i++) {
        if (sentence[i] == '*') {
            xing_index = i;
            break;
        }
        result ^= sentence[i];
    }

    if (xing_index < 0 || xing_index >= (int)(strlen(sentence) - 1)) {
        debug_line(DEBUG_TAG"[check fail]%s, %d.[%s]", __func__, __LINE__, sentence);
        return 0;
    }
    memset(checksum, 0, sizeof(checksum));
    if ((strlen(sentence) - xing_index - 1) >= 32) {
        debug_line(DEBUG_TAG"[check fail]%s, %d.[%s]", __func__, __LINE__, sentence);
        return 0;
    }
    memcpy(checksum, sentence + xing_index + 1, strlen(sentence) - xing_index - 1);
    for (i = 0; i < strlen(checksum); i++) {
        if (checksum[i] == '\r' || checksum[i] == '\n') {
            checksum[i] = '\0';
            break;
        }
    }
    if (strspn(checksum, "1234567890ABCDEF") != strlen(checksum)) {
        debug_line(DEBUG_TAG"[check fail]%s, %d.[%s]", __func__, __LINE__, sentence);
        return 0;
    }

    checksum_value = strtoul(checksum, NULL, 16);
    if ((unsigned int)checksum_value != result) {
        debug_line(DEBUG_TAG"[check fail]%s, %d.[%s]", __func__, __LINE__, sentence);
        debug_line(DEBUG_TAG"[check fail]%s, %d.checksum=%s,checksum_value=%d,result=%d", __func__, __LINE__,
                                                                                    checksum, checksum_value, result);
        return 0;
    }

    return 1;
}

static double ddtran(double d)
{
    unsigned int degree = 0;
    double cent = 0.0;

    degree = (unsigned int)(d / 100);
    cent = (d - (degree * 100)) / 60;
    return (degree + cent);
}

static double atod(char *str)
{
    double rval = 0;
    int t1 = 0, t2 = 0;
    int is_int = 1;
    int num = 0;
    double s = 10;
    int i = 0;

    if (str == NULL || strlen(str) == 0) {
        return 0.0;
    }
    while (*str != '\0') {
        if (*str == 0x2E) {
            str++;
            is_int = 0;
        }
        if ((*str >= 0x30) && (*str <= 0x39)) {
            if (is_int == 1) {
                t1 = t1 * 10 + (*str - 0x30);
            } else if (is_int == 0) {
                t2 = t2 * 10 + (*str - 0x30);
                num++;
            }
        } else {
            break;
        }
        str++;
    }
    for (i = 1; i < num; i++) {
        s *= 10.0;
    }
    rval = t1 + t2 / s;
    return rval;
}

static sat_info_s sat_info[GNSS_NUM][MAX_SAT_NUM];
static int gsv_parse(const char *gsv)
{
    char *p = (char *)gsv;
    char field[21][32];
    int i = 0, j = 0;
    int cur_index = 0, total_num = 0, cur_sat_num = 4;
    int tmp_gnss_inview = 0;
    gnss_data_s *data = &gnss_data;
    int cur_gnss_type = 0;

    if (strncmp(gsv + 1, "GP", 2) == 0) {
        cur_gnss_type = GNSS_GPS;
    } else if (strncmp(gsv + 1, "GL", 2) == 0) {
        cur_gnss_type = GNSS_GALILEO;
    } else if (strncmp(gsv + 1, "GA", 2) == 0) {
        cur_gnss_type = GNSS_QLONASS;
    } else if (strncmp(gsv + 1, "BD", 2) == 0) {
        cur_gnss_type = GNSS_BEIDOU;
    } else {
        return 0;
    }

    if (gnss_parser_check_checksum(gsv) == 0) {
        debug_line(DEBUG_TAG"%s, %d, checksum fail", __func__, __LINE__);
        return 0;
    }

    while (*p != '*') {
        if (*p == ',') {
            if (j >= 32) {
                j = 31;
            }
            field[i][j] = '\0';
            j = 0;
            i++;
            p++;
        } else {
            if (j >= 32) {
                j = 31;
            }
            field[i][j] = *p;
            j++;
            p++;
        }
        if (i > 20) {
            break;
        }
    }

    if (strspn(field[3], "1234567890") != strlen(field[3])) {
        return -1;
    }

    tmp_gnss_inview = atoi(field[3]);
    if (tmp_gnss_inview > MAX_SAT_NUM) {
        tmp_gnss_inview = MAX_SAT_NUM;
    }
    if (tmp_gnss_inview <= 0) {
        for (i = 0; i < MAX_SAT_NUM; i++) {
            memset(&sat_info[cur_gnss_type][i], 0, sizeof(sat_info_s));
            memcpy(&(data->m_Sat[cur_gnss_type][i]), &(sat_info[cur_gnss_type][i]), sizeof(sat_info_s));
        }
        data->m_nSatNumInView[cur_gnss_type] = 0;
        return 0;
    }
    total_num = atoi(field[1]);
    cur_index = atoi(field[2]) - 1;
    if (cur_index == 0) {
        for (i = 0; i < MAX_SAT_NUM; i++) {
            //init sat info
            memset(&sat_info[cur_gnss_type][i], 0, sizeof(sat_info_s));
        }
    }

    if (cur_index == total_num - 1) {
        cur_sat_num = tmp_gnss_inview % 4;
        if (cur_sat_num == 0) {
            cur_sat_num = 4;
        }
    }

    if (cur_sat_num > 0) {
        for (i = 0; i < cur_sat_num; i++) {
            sat_info[cur_gnss_type][(cur_index * 4) + i].PRN = atoi(field[4 + (4 * i)]);
            sat_info[cur_gnss_type][(cur_index * 4) + i].elevation = atoi(field[5 + (4 * i)]);
            sat_info[cur_gnss_type][(cur_index * 4) + i].azimuth = atoi(field[6 + (4 * i)]);
            sat_info[cur_gnss_type][(cur_index * 4) + i].ss = atod(field[7 + (4 * i)]);
            if (sat_info[cur_gnss_type][(cur_index * 4) + i].ss > 50.0) {
                sat_info[cur_gnss_type][(cur_index * 4) + i].ss = 50.0;
            }
        }
    }
    if (cur_index == (total_num - 1)) {
        for (i = 0; i < MAX_SAT_NUM; i++) {
            //init sat info
            memset(&data->m_Sat[cur_gnss_type][i], 0, sizeof(sat_info_s));
        }
        for (i = 0; i < tmp_gnss_inview; i++) {
            memcpy(&data->m_Sat[cur_gnss_type][i], &sat_info[cur_gnss_type][i], sizeof(sat_info_s));
        }
        data->m_nSatNumInView[cur_gnss_type] = tmp_gnss_inview;
    }

    return 0;
}

static int gga_parse(const char *gga)
{
    char *p = (char *)gga;
    char field[15][32];
    int i = 0, j = 0;
    gnss_data_s *data = &gnss_data;

    if (gnss_parser_check_checksum(gga) == 0) {
        debug_line(DEBUG_TAG"%s, %d, checksum fail", __func__, __LINE__);
        return 0;
    }

    while (*p != '*') {
        if (*p == ',') {
            if (j >= 32) {
                j = 31;
            }
            field[i][j] = '\0';
            j = 0;
            i++;
            p++;
        } else {
            if (j >= 32) {
                j = 31;
            }
            field[i][j] = *p;
            j++;
            p++;
        }
        if (i > 14) {
            break;
        }
    }

    if (i < 14) {
        return -1;
    }

    if (strspn(field[7], "1234567890.") != strlen(field[7])) {
        return -1;
    }
    if (strspn(field[9], "1234567890.") != strlen(field[9])) {
        return -1;
    }
    data->m_nSatNumInUse = atoi(field[7]);
    data->m_nHdop = atod(field[8]);
    data->m_nAltitude = atod(field[9]);
    data->m_nGeoidHeight = atod(field[11]);

    return 0;
}

static int rmc_parse(const char *rmc)
{
    char *p = (char *)rmc;
    char field[12][32];
    int i = 0, j = 0;
    int temp = 0;
    double temp1 = 0.0;
    double temp2 = 0.0;
    gnss_data_s *data = &gnss_data;

    if (gnss_parser_check_checksum(rmc) == 0) {
        debug_line(DEBUG_TAG"%s, %d, checksum fail", __func__, __LINE__);
        return 0;
    }

    while (*p != '*') {
        if (*p == ',') {
            if (j >= 32) {
                j = 31;
            }
            field[i][j] = '\0';
            j = 0;
            i++;
            p++;
        } else {
            if (j >= 32) {
                j = 31;
            }
            field[i][j] = *p;
            j++;
            p++;
        }
        if (i > 11) {
            break;
        }
    }

    if (i < 11) {
        return -1;
    }

    for (i = 0; i < 12; i++) {
        memset(&data->RmcFields[i], 0, sizeof(data->RmcFields[i]));
        memcpy(&data->RmcFields[i], &field[i], sizeof(data->RmcFields[i]));
    }

    if (field[ED_STATUS][0] == 'A') {
        data->m_bLocal = 1;
    } else {
        data->m_bLocal = 0;
    }

    temp2 = atod(field[ED_HHMMSS]);
    temp = (int)temp2;
    if (temp < 250000) {
        data->m_nHour = temp / 10000;
        data->m_nMinute = (temp % 10000) / 100;
        data->m_nSecond = temp % 100;
        data->m_nMSecond = (temp2 - temp) * 1000;
    }
    temp = atoi(field[ED_DDMMYY]);
    if (temp < 320000) {
        data->m_nDay = temp / 10000;
        data->m_nMonth = (temp % 10000) / 100;
        data->m_nYear = temp % 100 + 2000;
    }
    temp1 = atod(field[ED_LATITUDE]);
    data->m_rawLat = temp1;
    temp1 = ddtran(temp1);
    if (field[ED_LATITUDE_TYPE][0] == 'S') {
        temp1 = 0 - temp1;
    }
    data->m_dLat = temp1;

    temp1 = atod(field[ED_LONGTITUDE]);
    data->m_rawLon = temp1;
    temp1 = ddtran(temp1);
    if (field[ED_LONGTITUDE_TYPE][0] == 'W') {
        temp1 = 0 - temp1;
    }
    data->m_dLon = temp1;

    temp1 = atod(field[ED_SPEED]);
    temp1 *= 1.932;// 1.852; //KNOTS_TO_KPH   //1.932
    data->m_nSpeed = temp1;

    temp1 = atod(field[ED_COURSE]);
    data->m_nAngle = (int)(temp1 + 0.5);

    return 0;
}

static void bubble_sort(sat_info_s *satInfo, int n)
{
    int i = 0, j = 0;
    sat_info_s tmp;

    for (i = 0; i < n; i++) {
        for (j = 0; i + j < n - 1; j++) {
            if (satInfo[j].ss < satInfo[j + 1].ss) {
                memcpy(&tmp, &satInfo[j], sizeof(sat_info_s));
                memcpy(&satInfo[j], &satInfo[j + 1], sizeof(sat_info_s));
                memcpy(&satInfo[j + 1], &tmp, sizeof(sat_info_s));
            }
        }
    }
}

static char gnss_name[GNSS_NUM][16] = {"GPS", "BEIDOU", "GALILEO", "GLONASS", "SBAS", "IMES", "QZSS",};
void gnss_parser_debug_result(void *pFile)
{
    gnss_data_s data;
    char tmp[512]  = {0};
    int i = 0, index = 0;
    char line1[512] = {0};
    char line2[512] = {0};
    static int pre_gps_status = 0;
    static unsigned int location_time = 0;
    int use_time = 0;

    gnss_parser_get_data(&data);
    if (data.ready == 0) {
        return;
    }

    if (pre_gps_status != data.m_bLocal) {
        pre_gps_status = data.m_bLocal;
        if (data.m_bLocal) {
            beep_play(BEEP_ID_1K);
            if (location_time == 0) {
                location_time = gnss_parser_get_tickcount() / 1000;
            }
        }
    }

    if (location_time == 0) {
        use_time = gnss_parser_get_tickcount() / 1000;
    } else {
        use_time = location_time;
    }
    memset(tmp, 0, sizeof(tmp));
    snprintf(tmp, sizeof(tmp) - 1, "S(%d) P(%.6f,%.6f) U(%d) Heading(%d) T(%d)", data.m_bLocal, data.m_dLat, data.m_dLon,
                                                                data.m_nSatNumInUse, data.m_nAngle, use_time);
    debug_line(DEBUG_TAG"%s", tmp);
    if (pFile) {
        strcat(tmp, "\r\n");
        fwrite(tmp, strlen(tmp), 1, pFile);
    }

    memset(tmp, 0, sizeof(tmp));
    snprintf(tmp, sizeof(tmp) - 1, "%.2fKM/H UTC(%.4d-%.2d-%.2d %.2d:%.2d:%.2d)", data.m_nSpeed,
                                                    data.m_nYear, data.m_nMonth, data.m_nDay,
                                                    data.m_nHour, data.m_nMinute, data.m_nSecond);
    debug_line(DEBUG_TAG"%s", tmp);
    if (pFile) {
        strcat(tmp, "\r\n");
        fwrite(tmp, strlen(tmp), 1, pFile);
    }

    for (index = 0; index < GNSS_NUM; index++) {
        memset(line1, 0, sizeof(line1));
        snprintf(line1, sizeof(line1) - 1, "(%s)Sat NumInView: %d", gnss_name[index], data.m_nSatNumInView[index]);
        debug_line(DEBUG_TAG"%s", line1);
        if (pFile) {
            memset(line1, 0, sizeof(line1));
            strcat(line1, "\r\n");
            fwrite(line1, strlen(line1), 1, pFile);
        }
        memset(line1, 0, sizeof(line1));
        memset(line2, 0, sizeof(line2));
        strcat(line1, "PRN: ");
        strcat(line2, "SS:  ");
        bubble_sort((sat_info_s *)&data.m_Sat[index][0], data.m_nSatNumInView[index]);
        for (i = 0; i < 12; i++) {
            char t[32] = {0};
            memset(t, 0, sizeof(t));
            snprintf(t, sizeof(t) - 1, "%8d ", data.m_Sat[index][i].PRN);
            strcat(line1, t);
            memset(t, 0, sizeof(t));
            snprintf(t, sizeof(t) - 1, "%8d ", (int)data.m_Sat[index][i].ss);
            strcat(line2, t);
        }
        debug_line(DEBUG_TAG"%s", line1);
        if (pFile) {
            strcat(line1, "\r\n");
            fwrite(line1, strlen(line1), 1, pFile);
        }
        debug_line(DEBUG_TAG"%s", line2);
        if (pFile) {
            strcat(line2, "\r\n");
            fwrite(line2, strlen(line2), 1, pFile);
        }
    }
    debug_line(DEBUG_TAG"");
    if (pFile) {
        memset(line1, 0, sizeof(line1));
        strcat(line1, "\r\n");
        fwrite(line1, strlen(line1), 1, pFile);
        fflush(pFile);
    }
}

