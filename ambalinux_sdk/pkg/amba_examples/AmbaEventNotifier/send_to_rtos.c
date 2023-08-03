#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "AmbaEventNotifier.h"
#include "AmbaEventDef.h"


static void cleanup(void)
{
    AmbaEventNotify_Release();
}

void ShowUsage(char *p_name)
{
    printf("usage: %s <event type> [further info]\n"
            "<event type>:\n"
            "   \"boot_done\": Linux boot done.\n"
            "   \"net_ready\" <type> <: network ready as specified type.\n"
            "       <type>:\n"
            "           0: AP mode.\n"
            "           1: STA mode.\n"
            "           2: P2P mode.\n"
            "   \"net_off\": network turned-off.\n"
            "   \"photo\": Capture photo with bluetooth shutter.\n"
            "   \"record\": Record with bluetooth shutter.\n"
            "\n",
        p_name);
}

int main (int argc, char *argv[])
{
    int rval = 0;
    char param[512];
    unsigned int param_len;

    if(argc < 2) {
        ShowUsage(argv[0]);
        return -1;
    }

    rval = AmbaEventNotify_Init();
    if(rval < 0){
        printf("@@ Fail to do AmbaEventNotify_Init()\n");
        return -1;
    }
    atexit(cleanup);

    if (strcmp(argv[1], "boot_done") == 0) {
        rval = AmbaEventNotify_Send(EVENTNOTIFIER_LINUXEVENT_BOOTED, NULL, 0);
    } else if (strcmp(argv[1], "net_ready") == 0) {
        unsigned int *iptr;

        iptr=(unsigned int *)param;
        iptr[0] = strtoul(argv[2], NULL, 0);
        param_len = sizeof(unsigned int);

        rval = AmbaEventNotify_Send(EVENTNOTIFIER_LINUXEVENT_NETWORK_READY, param, param_len);
    } else if (strcmp(argv[1], "net_off") == 0) {
        rval = AmbaEventNotify_Send(EVENTNOTIFIER_LINUXEVENT_NETWORK_OFF, NULL, 0);
    } else if (strcmp(argv[1], "photo") == 0) {
        rval = AmbaEventNotify_Send(EVENTNOTIFIER_LINUXEVENT_TAKEPHOTO, NULL, 0);
    } else if (strcmp(argv[1], "record") == 0) {
        rval = AmbaEventNotify_Send(EVENTNOTIFIER_LINUXEVENT_RECORD, NULL, 0);
    } else {
        ShowUsage(argv[0]);
        return -1;
    }

    return rval;
}

