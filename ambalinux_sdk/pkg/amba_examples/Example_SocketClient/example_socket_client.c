#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>
#include <getopt.h>
#include <execinfo.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "mem_util.h"
#include <arpa/inet.h>

//#include <ambacv/cvapi/cvapi_amba_od_interface.h>

#define AMBA_ODV37_MAX_NUM_BBOX         (1024)   /* max number of output bbox */
typedef struct {
        uint32_t type;                  /* AMBA_OD_OUTPUT_TYPE                */
        uint32_t length;                /* sizeof(amba_od_out_t)              */
        uint32_t num_objects;           /* size of object list                */
        uint32_t objects_offset;        /* relative pointer to object list    */
        uint32_t num_proposals;         /* size of proposal list              */
        uint32_t proposals_offset;      /* relative pointer to proposal list  */
        uint32_t frame_num;             /* frame_num of the current frame     */
        uint32_t capture_time;          /* capture time of the frame          */
        uint32_t errorcode;             /* Error code issued by the task.     */
        uint32_t  class_name;            /* phy addr of class-name list        */
        uint16_t camera_id;             /* camera ID                          */
#define AMBAOD_TYPE_GENERAL     0
#define AMBAOD_TYPE_BARRIER     1
#define AMBAOD_TYPE_POLE        2
        uint16_t network_type;          /* network type                       */
        uint8_t  major_version[32];     /* version string of ambaod           */
        uint8_t  minor_version[32];     /* version string of classifier       */
        uint64_t  class_name_p64;        /* class-name addr, for RTOS only     */
        uint8_t  padding[128-116];      /* padding to 128 bytes               */
} amba_od_out_t;
#define AC_CVAPI_CNN_OBJECT_T_VERSION           (1U)    // This shall be updated also when data pointed to are changed
#define AC_CVAPI_CNN_OBJECT_T_VER_BACK_0        (0U)    // Back compatibility version

typedef struct {
        uint32_t score;                 /* confidence score                   */
        uint32_t field;                 /* field ID, assigned by step1        */
        uint32_t clsId;                 /* class ID, assigned by step4        */
        uint32_t track;                 /* track ID, assigned by tracker      */
        int32_t  bb_start_row;
        int32_t  bb_start_col;
        uint32_t bb_height_m1;
        uint32_t bb_width_m1;
} amba_od_candidate_t;

uint32_t g_debug_en;

static int g_sockfd = 0;

static void AmbaOD_AmbaODTrans(amba_od_out_t *pOdResult)
{
    #define AMBANET_OD_FC_NUM_OF_CATEGORIES   (17U)
    uint32_t ObjNum;
    amba_od_candidate_t *pObjBase;

    pObjBase = (amba_od_candidate_t *)((uint64_t)pOdResult + (uint64_t)pOdResult->objects_offset);
    ObjNum = pOdResult->num_objects;

    if (ObjNum <= AMBA_ODV37_MAX_NUM_BBOX) {
        printf("[Linux] AmbaOD result: numobj = %u, cap_time = %u, frame_num = %u\n",
            ObjNum, pOdResult->capture_time, pOdResult->frame_num);

        if (0) {
            for (uint32_t i = 0U ; i < ObjNum; i++) {
                printf("[Linux] C %u X %u Y %u W %u H %u\n",
                                      pObjBase[i].clsId,
                                      pObjBase[i].bb_start_col,
                                      pObjBase[i].bb_start_row,
                                      (pObjBase[i].bb_width_m1 + 1U),
                                      (pObjBase[i].bb_height_m1 + 1U));
            }
        }
    } else {
        printf("[Linux] AmbaOD result: invalid numobj = %u\n", ObjNum);
    }
}

/**
 * signal() callback
 * This function will be invoked when got related signal.
 **/
static void SignalHandler(int sig)
{
    close(g_sockfd);
    printf("%s: Got signal %d, program exits and close socket\n",__FILE__,sig);
    exit(1);
}

static void SEGVhandler(int signo)
{
    void *array[10];
    size_t size;

    close(g_sockfd);
    // get void*'s for all entries on the stack
    size = backtrace(array, 10);

    // print out all the frames to stderr
    fprintf(stderr, "%s: Got segmentation fault (%d)\n", __FILE__, signo);
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    exit(1);
}

/**
 * aexit() callback
 * This function will be invoked when program exits.
 **/
static void CleanUp(void)
{
    printf("program exit!\n");
    printf("close Socket\n");
    close(g_sockfd);
    printf("Program built at %s - %s\n", __DATE__, __TIME__);
}


int main(int argc, char **argv)
{
    atexit(CleanUp);

    /* Allow ourselves to be shut down gracefully by a signal */
    signal(SIGTERM, SignalHandler);
    signal(SIGHUP, SignalHandler);
    signal(SIGUSR1, SignalHandler);
    signal(SIGQUIT, SignalHandler);
    signal(SIGINT, SignalHandler);
    signal(SIGKILL, SignalHandler);
    signal(SIGSEGV, SEGVhandler);

    {
        //Send a message to server
        struct sockaddr_in info;
        g_sockfd = socket(AF_INET , SOCK_STREAM , 0);

        if (g_sockfd == -1){
            printf("Fail to create a socket.");
        }

        bzero(&info,sizeof(info));
        info.sin_family = PF_INET;

        //localhost test
        info.sin_addr.s_addr = inet_addr("192.168.1.100");
        info.sin_port = htons(8700);


        int err = connect(g_sockfd,(struct sockaddr *)&info,sizeof(info));
        if(err==-1){
            printf("Connection error");
        }

        while (1) {
            uint64_t PhysAddr;
            if ( 0!= recv(g_sockfd, &PhysAddr, sizeof(PhysAddr),0)) {
                amba_od_out_t *pBBx;
                uint32_t MemSize = 0;

                //Physical address from RTOS need to map to virtual address
                MemSize = sizeof(amba_od_out_t) + AMBA_ODV37_MAX_NUM_BBOX * sizeof(amba_od_candidate_t);
                pBBx = (amba_od_out_t *) MemUtil_Remap(PhysAddr, MemSize);
                AmbaOD_AmbaODTrans(pBBx);
                MemUtil_Unmap(pBBx);
            } else {
                printf("Socket disconnect\n");
            }
        }
    }

    return 0;
}
