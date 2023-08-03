#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

#include "example_osdutil.h"

#define SAVEFILE (0)
#define COLOR_BASE (128)

int diff(struct timeval *Start, struct timeval *End)
{
    int temp;

    temp = End->tv_sec - Start->tv_sec;
    temp = (temp * 1000000) + (End->tv_usec - Start->tv_usec);

    return temp;
}

int main()
{
    int *Hndlr;
    unsigned int Rval;

    /* init */
    Rval = OSD_Init(&Hndlr, 960, 540);
    if (Rval != 0) {
        printf("Fail to do OSD_init()!\n");
        return 1;
    }

    /* draw rectangle */
    do {
        OSD_DrawRect(Hndlr,10,10,200,200,2,COLOR_BASE+1);
        OSD_DrawRect(Hndlr,210,210,400,400,2,COLOR_BASE+2);
        OSD_DrawRect(Hndlr,410,10,500,400,-1,COLOR_BASE+3);
    } while(0);

    /* draw string */
    do {
        char Text[128] = "Hello World!";

        OSD_DrawString(Hndlr,10,250,Text,2,COLOR_BASE+4);

        sprintf(Text, "This is result for opencv draw funtion");
        OSD_DrawString(Hndlr,210,450,Text,2,COLOR_BASE+4);

        sprintf(Text, "It works fine if you saw this!");
        OSD_DrawString(Hndlr,210,500,Text,2,COLOR_BASE+4);
    } while(0);

    /* draw Line */
    do {
        OSD_DrawLine(Hndlr,10,300,210,500,2,COLOR_BASE+5);
        OSD_DrawLine(Hndlr,10,500,210,300,2,COLOR_BASE+6);
    } while(0);

    /* draw circle */
    do {
        OSD_DrawCircle(Hndlr,640,140,130,2,COLOR_BASE+7);
        OSD_DrawCircle(Hndlr,770,140,130,-1,COLOR_BASE+8);
    } while(0);

    /* save OSD buffer */
    do {
        unsigned char *Buf;
        unsigned int Size;

        OSD_GetBuf(Hndlr, &Buf, &Size);

        if (SAVEFILE) {
            FILE *fptr;
            char OSDFILENAME[] = "/tmp/SD0/OSD.bin";

            fptr = fopen(OSDFILENAME, "wb+");
            if (fptr == NULL) {
                printf ("Cannot open file %s!\n",OSDFILENAME);
            } else {
                fwrite(Buf, Size, 1, fptr);
                fclose(fptr);
            }
        }
    } while(0);

    OSD_Release(Hndlr);

    return 0;
}
