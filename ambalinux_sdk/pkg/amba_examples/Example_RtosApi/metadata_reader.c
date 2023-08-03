#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

typedef struct _mp4_unit {
    unsigned int size;
    unsigned char body[0];
} mp4_unit;

static int get_mp4_metadata_buffer(const char *mp4file, unsigned char *metadata, int *metadata_len)
{
    FILE *fp = NULL;
    int size = 0, file_len = 0, offset = 0, moov_len = 0, udta_len = 0;
    int flag = 0;
    char szLine[20];
    size_t r_len = 0;
    mp4_unit *pMp4Unit = (mp4_unit *)szLine;

    if (!mp4file || !metadata || !metadata_len) {
        printf("please input the right parameter\n");
        return -1;
    }
    fp = fopen(mp4file, "rb");
    if (!fp) {
        printf("failed to fopen thee file %s\n", mp4file);
        return -1;
    }
    fseek(fp, 0, SEEK_END);
    file_len = ftell(fp);
    fseek(fp, 0, SEEK_SET);
   //!find the tag moov
    while (file_len > size) {
        r_len = fread(szLine, 1, sizeof(szLine), fp);
        if (htonl(pMp4Unit->size) == 0 || r_len == 0){
            break;
        }
        if (!strncmp((const char *)pMp4Unit->body, "moov", strlen("moov")) && htonl(pMp4Unit->size) > 0){
            flag = 1;
            break;
        }
        size += htonl(pMp4Unit->size);
        fseek(fp, size, SEEK_SET);
    }
    if(!flag){
        printf("can not find tag moov\n");
        fclose(fp);
        return -1;
    }
    //!find the tag udta
    flag = 0;
    size += 8;
    offset = 8;
    fseek(fp, size, SEEK_SET);
    moov_len = htonl(pMp4Unit->size) - 8;
    while(file_len > size && moov_len > offset){
        r_len = fread(szLine, 1, sizeof(szLine), fp);
        //printf("Unit size = %u, r_len = %zd, content:%s\n", htonl(pMp4Unit->size), r_len, pMp4Unit->body);
        if(htonl(pMp4Unit->size) == 0 || r_len == 0){
            break;
        }
        else if(!strncmp((const char *)pMp4Unit->body, "udta", strlen("udta")) && htonl(pMp4Unit->size) > 0){
            flag = 1;
            break;
        }
        size += htonl(pMp4Unit->size);
        offset += htonl(pMp4Unit->size);
        fseek(fp, size, SEEK_SET);
    }
    if(!flag){
        printf("can not find tag udta\n");
        fclose(fp);
        return -1;
    }
    //!find CUST
    flag = 0;
    size += 8;
    offset = 8;
    fseek(fp, size, SEEK_SET);
    udta_len = htonl(pMp4Unit->size) - 8;
    while(file_len > size && udta_len > offset){
        r_len = fread(szLine, 1, sizeof(szLine), fp);
        //printf("Unit size = %u, r_len = %zd, content:%s\n", htonl(pMp4Unit->size), r_len, pMp4Unit->body);
        if (htonl(pMp4Unit->size) == 0 || r_len == 0){
            break;
        } else if(!strncmp((const char *)pMp4Unit->body, "CUST", strlen("CUST")) && htonl(pMp4Unit->size) > 0){
            flag = 1;
			printf("CUST found\n");
            break;
        }
        size += htonl(pMp4Unit->size);
        offset += htonl(pMp4Unit->size);
        fseek(fp, size, SEEK_SET);
    }
    if (!flag) {
        printf("can not find tag CUST\n");
        fclose(fp);
        return -1;
    }
    size += 8;
    fseek(fp, size, SEEK_SET);

    r_len = fread((void *)metadata, 1, htonl(pMp4Unit->size) - 8, fp);

	if (r_len != (htonl(pMp4Unit->size) - 8)) {
		printf("failed to read CUST content\n");
        fclose(fp);
		return -1;
	}
	*metadata_len = htonl(pMp4Unit->size) - 8;

    fclose(fp);

    return 0;
}

typedef struct _metadata_s_ {
	unsigned int video_start_time_seconds;
	unsigned int video_start_time_mseconds;
	char sn[32];
	char sw_version[64];
	char build_time[32];
} metadata_s;

int metadata_parse(unsigned char *buffer, int len, metadata_s *metadata)
{
	int index = 0;

	if (buffer == NULL || len <= 0 || metadata == NULL) {
		return -1;
	}
	if (buffer[index] > 0) {
		metadata->video_start_time_seconds = (buffer[index + 1] << 24) + (buffer[index + 2] << 16) + (buffer[index + 3] << 8) + (buffer[index + 4]);
		metadata->video_start_time_mseconds = (buffer[index + 5] << 24) + (buffer[index + 6] << 16) + (buffer[index + 7] << 8) + (buffer[index + 8]);
		index += (buffer[index] + 1);
	}
	//get sn
	if (buffer[index] > 0) {
		memcpy(metadata->sn, &buffer[index + 1], buffer[index]);
		index += (buffer[index] + 1);
	}
	//get sw_version
	if (buffer[index] > 0) {
		memcpy(metadata->sw_version, &buffer[index + 1], buffer[index]);
		index += (buffer[index] + 1);
	}
	//get build_time
	if (buffer[index] > 0) {
		memcpy(metadata->build_time, &buffer[index + 1], buffer[index]);
	}

	return 0;
}

int main(int argc, char **argv)
{
	unsigned char buffer[256] = {0};
	int buffer_len = 0;
	metadata_s metadata;

	if (argc < 2) {
		printf("usage: %s [file_path]\n", argv[0]);
		return -1;
	}
	if (get_mp4_metadata_buffer(argv[1], buffer, &buffer_len) != 0) {
		return -1;
	}
	memset(&metadata, 0, sizeof(metadata));
	metadata_parse(buffer, buffer_len, &metadata);

	struct tm *tm;
	time_t time = (time_t)metadata.video_start_time_seconds;
	tm = gmtime(&time);
	printf("metadata_len=%d\n", buffer_len);
	printf("metadata video start time=%d%d, %d-%d-%d %d:%d:%d\n", metadata.video_start_time_seconds, metadata.video_start_time_mseconds,
														tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
	printf("                       sn=%s\n", metadata.sn);
	printf("               sw_version=%s\n", metadata.sw_version);
	printf("               build_time=%s\n", metadata.build_time);

	return 0;
}

