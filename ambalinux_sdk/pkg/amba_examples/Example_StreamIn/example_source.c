/*
 *  This example source demonstrate the reading from .adts file(.adts from AMBA)
 *
 *  1. First pass the filepath in example_source_open().
 *
 *  2. Then, each call of example_source_read() should return one AAC raw frame
 *     example_source_read() return 1 if met EOS
 *     example_source_read() return -1 if met error in reading ADTS format
 *     example_source_read() return 0 if successful read one frame
 *
 *  3. Calling example_source_close() to release memory/system resource
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

typedef struct example_source_s {
    int fd;
    unsigned char* fmap;
    int map_size;
    unsigned char* buf; //crusor position
} example_source_t;

int example_source_open(struct example_source_s** source, const char* url)
{
    struct stat sb;
    example_source_t* src;
    if(source == NULL || url == NULL) {
        fprintf(stderr, "%s: Invalid input\n", __FUNCTION__);
        return -1;
    }

    src = (example_source_t*) malloc(sizeof(example_source_t));
    src->fd = open(url, O_RDONLY);
    if(src->fd < 0){
        fprintf(stderr, "%s: Fail to open input %s\n", __FUNCTION__, url);
        return -1;
    }
    src->buf = NULL;
    if( fstat(src->fd, &sb) == -1 ) {
        return -1;
    }
    src->fmap = (unsigned char*) mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, src->fd, 0);
    if( src->fmap == MAP_FAILED) {
        return -1;
    }
    src->map_size = sb.st_size;
    src->buf = src->fmap;

    *source = src;
    return 0;
}

/* example to read .adts file and output RAW AAC stream*/
int example_source_read(struct example_source_s* src, unsigned char** data, unsigned int* data_len)
{
    unsigned char* p;
    int frame_size;
    unsigned int protectAbsent = 1; //1 if no CRC
    unsigned int header_size = 0;
    if(src == NULL || src->fmap == NULL || src->buf == NULL
            || data == NULL || *data == NULL || data_len == NULL) {
        fprintf(stderr, "%s: Invalid input\n", __FUNCTION__);
        return -1;
    }
    p = (unsigned char*) src->buf;
    if(p >= src->fmap + src->map_size){
        return 1; //EOS
    } else if(p[0] == 0xff && (p[1]&0xf0) == 0xf0){
        protectAbsent = p[1]&0x01;
        header_size = (protectAbsent == 1) ? 7 : 9;
        frame_size = ((p[3]&0x03) << 11) | (p[4] << 3) | (p[5]>>5); //this frame_size include header_size
        *data = p + header_size;
        *data_len = frame_size - header_size;
        src->buf = p + frame_size;
        return 0;
    } else {
        printf("can't find adts header\n");
        return -1;
    }
}

int example_source_close(struct example_source_s* src)
{
    if(src == NULL) {
        fprintf(stderr, "%s: Invalid input\n", __FUNCTION__);
        return -1;
    }
    if(src->fmap != NULL) munmap(src->fmap, src->map_size);
    if(src->fd != 0) close(src->fd);

    free(src);
    return 0;
}
