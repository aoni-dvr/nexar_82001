#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include "bbx_handler.h"

typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

#define ADAS_BOX_CTRL_MAX_BBX_NUM       (150)
typedef struct adas_box_ctrl_bbx_s{
    uint16_t cat;
    uint16_t fid;
    uint32_t score;
    uint16_t x;
    uint16_t y;
    uint16_t w;
    uint16_t h;
}adas_box_ctrl_bbx_t;

typedef struct adas_box_ctrl_bbx_list_header_s {
    uint32_t msg_code;
    uint32_t source;
    uint32_t capture_time;
    uint32_t frame_num;
    uint32_t num_bbx;
} adas_box_ctrl_bbx_list_header_t;

//ADAS_BOX_MSG_BBX
typedef struct adas_box_ctrl_bbx_list_msg_s{
    adas_box_ctrl_bbx_list_header_t header;
    adas_box_ctrl_bbx_t bbx[ADAS_BOX_CTRL_MAX_BBX_NUM];
}adas_box_ctrl_bbx_list_msg_t;

static void cleanup(void)
{
    Bbx_Release();
}

static void signalHandlerShutdown(int sig)
{
    printf("%s: Got signal %d, program exits!\n",__FILE__,sig);
    exit(0);
}

static void dumpdata(void *in, int len)
{
    int i;
    unsigned char *data = (unsigned char *)in;

    printf("\n");
    for(i = 0; i<len; i++) {
        printf("%02x ", data[i]);
        if((i&0xf)==0xf) {
            printf("\n");
        }
    }
    printf("\n");
}

int main (int argc, char *argv[])
{
    int rval = 0;
    adas_box_ctrl_bbx_list_msg_t *bbx_list;
    Bbx_Buffer_Setting_s BufSetting = {0};
    Bbx_Buffer_Info_s Buffer = {0};
    Bbx_Info_s BbxInfo = {0};

    rval = Bbx_Init();
    if(rval < 0){
        printf("@@ Fail to do Bbx_Init()\n");
        return -1;
    }

    atexit(cleanup);
    /* Allow ourselves to be shut down gracefully by a signal */
    signal(SIGTERM, signalHandlerShutdown);
    signal(SIGHUP, signalHandlerShutdown);
    signal(SIGUSR1, signalHandlerShutdown);
    signal(SIGQUIT, signalHandlerShutdown);
    signal(SIGINT, signalHandlerShutdown);
    signal(SIGKILL, signalHandlerShutdown);

    BufSetting.MemType = 1;
    BufSetting.Size = sizeof(adas_box_ctrl_bbx_list_msg_t);
    BufSetting.Align = 64;

    rval = Bbx_Create_Buffer(&BufSetting, &Buffer);
    if(rval < 0) {
        printf("Fail to do Bbx_Create_Buffer()\n");
        return -1;
    }

//1st
    bbx_list = (adas_box_ctrl_bbx_list_msg_t *)(Buffer.MmapedAddr);
    bbx_list->header.msg_code = 0x00010001;
    bbx_list->header.source = 0;
    bbx_list->header.capture_time = 0x12345678;
    bbx_list->header.frame_num = 0;
    bbx_list->header.num_bbx = 1;
    bbx_list->bbx[0].cat = 1;
    bbx_list->bbx[0].fid = 2;
    bbx_list->bbx[0].score = 255;
    bbx_list->bbx[0].x = 11;
    bbx_list->bbx[0].y = 11;
    bbx_list->bbx[0].w = 20;
    bbx_list->bbx[0].h = 30;

    BbxInfo.Flags = 0x1; //The last block
    BbxInfo.Size = sizeof(adas_box_ctrl_bbx_list_header_t)+(bbx_list->header.num_bbx * sizeof(adas_box_ctrl_bbx_t));
    BbxInfo.Addr = Buffer.AlignedAddr; //physical addr

    dumpdata(Buffer.MmapedAddr, 16);

    rval = Bbx_Update(&BbxInfo);
    if(rval < 0) {
        printf("Fail to do Bbx_Update()...1\n");
        return -1;
    }

//2nd
    bbx_list->header.msg_code = 0x00020002;
    bbx_list->header.source = 1;
    bbx_list->header.capture_time = 0x87654321;
    bbx_list->header.frame_num = 1;
    bbx_list->header.num_bbx = 2;
    bbx_list->bbx[0].cat = 2;
    bbx_list->bbx[0].fid = 1;
    bbx_list->bbx[0].score = 254;
    bbx_list->bbx[0].x = 22;
    bbx_list->bbx[0].y = 22;
    bbx_list->bbx[0].w = 30;
    bbx_list->bbx[0].h = 40;

    bbx_list->bbx[1].cat = 2;
    bbx_list->bbx[1].fid = 2;
    bbx_list->bbx[1].score = 254;
    bbx_list->bbx[1].x = 23;
    bbx_list->bbx[1].y = 23;
    bbx_list->bbx[1].w = 33;
    bbx_list->bbx[1].h = 43;

    BbxInfo.Flags = 1;
    BbxInfo.Size = sizeof(adas_box_ctrl_bbx_list_header_t)+(bbx_list->header.num_bbx * sizeof(adas_box_ctrl_bbx_t));
    BbxInfo.Addr = Buffer.AlignedAddr; //physical addr

    dumpdata(Buffer.MmapedAddr, 16);

    rval = Bbx_Update(&BbxInfo);
    if(rval < 0) {
        printf("Fail to do Bbx_Update()...2\n");
        return -1;
    }

//3rd
    bbx_list->header.msg_code = 0x00030003;
    bbx_list->header.source = 2;
    bbx_list->header.capture_time = 0xabcdef01;
    bbx_list->header.frame_num = 2;
    bbx_list->header.num_bbx = 3;
    bbx_list->bbx[0].cat = 3;
    bbx_list->bbx[0].fid = 2;
    bbx_list->bbx[0].score = 253;
    bbx_list->bbx[0].x = 33;
    bbx_list->bbx[0].y = 33;
    bbx_list->bbx[0].w = 40;
    bbx_list->bbx[0].h = 50;

    bbx_list->bbx[1].cat = 3;
    bbx_list->bbx[1].fid = 3;
    bbx_list->bbx[1].score = 253;
    bbx_list->bbx[1].x = 34;
    bbx_list->bbx[1].y = 34;
    bbx_list->bbx[1].w = 44;
    bbx_list->bbx[1].h = 54;

    bbx_list->bbx[2].cat = 3;
    bbx_list->bbx[2].fid = 4;
    bbx_list->bbx[2].score = 253;
    bbx_list->bbx[2].x = 35;
    bbx_list->bbx[2].y = 35;
    bbx_list->bbx[2].w = 45;
    bbx_list->bbx[2].h = 55;

    BbxInfo.Flags = 1;
    BbxInfo.Size = sizeof(adas_box_ctrl_bbx_list_header_t)+(bbx_list->header.num_bbx * sizeof(adas_box_ctrl_bbx_t));
    BbxInfo.Addr = Buffer.AlignedAddr; //physical addr

    dumpdata(Buffer.MmapedAddr, 16);

    rval = Bbx_Update(&BbxInfo);
    if(rval < 0) {
        printf("Fail to do Bbx_Update()...2\n");
        return -1;
    }

    rval = Bbx_Free_Buffer(&Buffer);
    if(rval < 0){
        printf("@@ Fail to do Bbx_Free_Buffer()\n");
        return -1;
    }

    rval = Bbx_Release();
    if(rval < 0){
        printf("@@ Fail to do Bbx_Release()\n");
        return -1;
    }

    return 0;
}

