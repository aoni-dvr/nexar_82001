#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdint.h>
#include <assert.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <rtos/AmbaRTOSWrapper.h>
#include "ppm.h"
#include "cvapi_visutil.h"
#include "ambint.h"
#include "cvapi_idsp_interface.h"
#include "cvapi_amba_od_interface.h"
#include "cvapi_ambacv_flexidag.h"
#include "cvapi_memio_interface.h"
#include "AmbaFlexidagIO.h"
#include "idsp_roi_msg.h"


#ifndef CONFIG_CV_CONFIG_TX_SDK7
#define AmbaMisra_TypeCast    AmbaMisra_TypeCast64
#else
#define AmbaMisra_TypeCast    AmbaMisra_TypeCast32
#endif


#define DEFAULT_KEY     0xaabbccddeeff
#define DEFAULT_INET    "127.0.0.1"
#define DEFAULT_PORT    8699
#define OPTIONS         "f:n:p:t:d:c:s:b:m:q:r:i:e"

#define AMBAOD_MAX_BBX_NUM  200

#define MNETSSD_KEY                 0xEE
#define MNETSSD_CVTASK_UUID             2
#define MNETSSD_HL_CVTASK_UUID          4

#define CACHE_ALIGN(x)              ((x + 0x3F)& ~0x3F)

#define MAX_THREAD_NUM              15
#define MAX_GOLDEN_NUM              4
#define MAX_FLEXIDAG_SIZE           0x1000000
#define MAX_FILE_SIZE               0x0400000

#define MODE_FILE_MODE              0
#define MODE_SENSOR_MODE            1


typedef struct {
    uint32_t                        id;
    AMBA_CV_FLEXIDAG_HANDLE_s       fd_gen_handle;
    char                            name[64];
    AMBA_CV_FLEXIDAG_INIT_s         init;
    flexidag_memblk_t               bin_buf;
    AMBA_CV_FLEXIDAG_IO_s           in_buf;
    AMBA_CV_FLEXIDAG_IO_s           out_buf;
} REF_CV_HANDLE_s;

typedef struct {
    amba_od_out_t Info;
    amba_od_candidate_t Bbx[AMBAOD_MAX_BBX_NUM];
} AMBA_OD_s;

typedef struct {
    uint32_t                        id;
    uint32_t                        num_runs;
    uint32_t                        cur_runs;
    uint32_t                        num_err;
    uint32_t                        sleep_usec;
    uint32_t                        RunFlag;
    uint32_t                        data_channel_id;
    char                            file_name[128];
    char                            flexidag_path[128];
    REF_CV_HANDLE_s                 handle;
    pthread_t                       pthread;
    int                             msg_queue;
    flexidag_memblk_t               file_buf;
} REF_CV_PARAMS_s;

static int external_msg_thread = 0;
static uint8_t run_mode = MODE_FILE_MODE;
static uint32_t data_channel_id = 0;
static int blocking_run = 1;
static int num_of_frame = 1;
static int schdr_close = 0;
static int process_id = 0;
static uint32_t thread_id = 1U;
static char flexidag_path[128] = "../flexibin";
static char data_path[128] = "../golden";
static REF_CV_PARAMS_s thread_params[MAX_THREAD_NUM];

static flexidag_memblk_t g_temp_buf;
static UINT32 g_frame_num = 0;

static VIS_UTIL_BBX_LIST_MSG_s golden[MAX_GOLDEN_NUM] = {0};

static uint32_t idsp_pyramid_scale = 0U;
static uint32_t roi_start_col = 810U;
static uint32_t roi_start_row = 390U;
static uint32_t source_vin = 0U;

typedef struct {
    uint32_t raw_w;
    uint32_t raw_h;

    uint32_t roi_w;
    uint32_t roi_h;

    uint32_t roi_start_col;
    uint32_t roi_start_row;

    uint32_t net_in_w;
    uint32_t net_in_h;
} win_ctx_t;

static UINT32 roi_handling(AMBA_CV_FLEXIDAG_HANDLE_s *fd_gen_handle, const cv_pic_info_t *idsp_pic_info)
{
    UINT32 retcode = ERRCODE_NONE;
    uint32_t roi_w_m1, roi_h_m1;
    win_ctx_t win_ctx = {0};
    uint16_t flow_id;
    AMBA_CV_FLEXIDAG_MSG_s msg;
    static uint32_t network_input_width = 300U;
    static uint32_t network_input_height = 300U;

    (void) fd_gen_handle;
    roi_w_m1 = idsp_pic_info->pyramid.half_octave[idsp_pyramid_scale].roi_width_m1;
    roi_h_m1 = idsp_pic_info->pyramid.half_octave[idsp_pyramid_scale].roi_height_m1;

    if (roi_start_col == 9999U) {
        roi_start_col = (roi_w_m1 + 1U - network_input_width) / 2U;
    } else if (roi_start_col > 0x7FFFFFFFU) {
        roi_start_col = 0U;
    } else if (roi_start_col > (roi_w_m1 + 1U - network_input_width)) {
        roi_start_col = (roi_w_m1 + 1U - network_input_width);
    } else {
        /* do nothing */
    }

    if (roi_start_row == 9999U) {
        roi_start_row = (roi_h_m1 + 1U - network_input_height) / 2U;
    } else if (roi_start_row > 0x7FFFFFFFU) {
        roi_start_row = 0U;
    } else if (roi_start_row > (roi_h_m1 + 1U - network_input_height)) {
        roi_start_row = (roi_h_m1 + 1U - network_input_height);
    } else {
        /* do nothing */
    }

    //printf("roi_handling: pyramid_scale = %d, start_col = %d, start_row= %d\n",idsp_pyramid_scale,roi_start_col,roi_start_row);
    //printf("roi_handling: roi_w_m1 = %d, roi_h_m1 = %d\n", roi_w_m1, roi_h_m1);

    win_ctx.raw_w = ((uint32_t)idsp_pic_info->pyramid.half_octave[0].roi_width_m1) + 1U;
    win_ctx.raw_h = ((uint32_t)idsp_pic_info->pyramid.half_octave[0].roi_height_m1) + 1U;
    win_ctx.roi_w = roi_w_m1 + 1U;
    win_ctx.roi_h = roi_h_m1 + 1U;

    win_ctx.roi_start_col    = roi_start_col;
    win_ctx.roi_start_row    = roi_start_row;
    win_ctx.net_in_w         = network_input_width;
    win_ctx.net_in_h         = network_input_height;

    (void) AmbaCV_FlexidagGetFlowIdByUUID(fd_gen_handle, MNETSSD_HL_CVTASK_UUID, &flow_id);
    msg.flow_id = flow_id;
    msg.vpMessage = &win_ctx;
    msg.length = sizeof(win_ctx);
    retcode = AmbaCV_FlexidagSendMsg(fd_gen_handle,&msg);
    return retcode;
}

static uint32_t RefCV_Open(const char *path, REF_CV_HANDLE_s *handle)
{
    uint32_t ret = 0U;
    AMBA_CV_FLEXIDAG_HANDLE_s *fd_gen_handle = &handle->fd_gen_handle;
    uint32_t size_align;

    ret = AmbaCV_UtilityFileSize(path, &size_align);
    if (ret != 0) {
        printf("RefCV_Open : name = %s AmbaCV_UtilityFileSize fail path = %s\n", handle->name, path);
        ret = 1U;
    } else {
        if(handle->bin_buf.pBuffer == NULL) {
            ret = AmbaCV_UtilityCmaMemAlloc(size_align, 1,&handle->bin_buf);
        } else {
            //printf("RefCV_Open : name = %s handle->bin_buf.pBuffer != NULL\n", handle->name);
        }

        if (ret != 0) {
            printf("RefCV_Open : name = %s AmbaCV_UtilityCmaMemAlloc fail path = %s\n", handle->name, path);
            ret = 1U;
        } else {
            ret = AmbaCV_UtilityFileLoad(path, &handle->bin_buf);
            if (ret != 0) {
                printf("RefCV_Open : name = %s AmbaCV_UtilityFileLoad fail path = %s\n", handle->name, path);
                ret = 1U;
            } else {
                ret = AmbaCV_FlexidagOpen(&handle->bin_buf, fd_gen_handle);
                if (ret != 0) {
                    printf("RefCV_Open : name = %s AmbaCV_FlexidagOpen fail path = %s size_align = %d\n", handle->name,path,size_align);
                    ret = 1U;
                }
            }
        }
    }

    return ret;
}

static uint32_t RefCV_Init(REF_CV_HANDLE_s *handle)
{
    uint32_t ret = 0U;
    AMBA_CV_FLEXIDAG_HANDLE_s *fd_gen_handle = &handle->fd_gen_handle;

    if(handle->init.state_buf.pBuffer == NULL) {
        ret = AmbaCV_UtilityCmaMemAlloc(CACHE_ALIGN(fd_gen_handle->mem_req.flexidag_state_buffer_size),1, &handle->init.state_buf);
    } else {
        //printf("RefCV_Init : name = %s handle->init.state_buf.pBuffer != NULL\n", handle->name);
    }

    if (ret != 0) {
        printf("RefCV_Init : name = %s AmbaCV_UtilityCmaMemAlloc state_buf fail \n", handle->name);
        ret = 1U;
    } else {
        if(handle->init.temp_buf.pBuffer == NULL) {
            handle->init.temp_buf.buffer_size = g_temp_buf.buffer_size;
            handle->init.temp_buf.buffer_daddr  = g_temp_buf.buffer_daddr;
            handle->init.temp_buf.buffer_cacheable = g_temp_buf.buffer_cacheable;
            handle->init.temp_buf.pBuffer        = g_temp_buf.pBuffer;
        } else {
            //printf("RefCV_Init : name = %s handle->init.temp_buf.pBuffer != NULL\n", handle->name);
        }

        if (ret != 0) {
            printf("RefCV_Init : name = %s AmbaCV_UtilityCmaMemAlloc temp_buf fail \n", handle->name);
            ret = 1U;
        } else {
            ret = AmbaCV_FlexidagInit(fd_gen_handle, &handle->init);
            if (ret != 0) {
                printf("RefCV_Init : name = %s AmbaCV_FlexidagInit fail \n", handle->name);
                ret = 1U;
            }
        }
    }

    return ret;
}

static void RefCV_GoldenInit(void)
{
#if defined(CHIP_CV2A) || defined(CHIP_CV2FS) || defined(CHIP_CV22FS) || defined(CHIP_CV5) || defined(CHIP_CV52)
    golden[0].NumBbx = 4U;
    golden[0].Bbx[0].Cat = (UINT16)255U;
    golden[0].Bbx[0].X = (UINT16)810U;
    golden[0].Bbx[0].Y = (UINT16)390U;
    golden[0].Bbx[0].W = (UINT16)(1110U - 810U);
    golden[0].Bbx[0].H = (UINT16)(690U-390U);
    golden[0].Bbx[1].Cat = (UINT16)3U;
    golden[0].Bbx[1].X = (UINT16)884U;
    golden[0].Bbx[1].Y = (UINT16)475U;
    golden[0].Bbx[1].W = (UINT16)(994U - 884U);
    golden[0].Bbx[1].H = (UINT16)(579U - 475U);
    golden[0].Bbx[2].Cat = (UINT16)3U;
    golden[0].Bbx[2].X = (UINT16)1025U;
    golden[0].Bbx[2].Y = (UINT16)482U;
    golden[0].Bbx[2].W = (UINT16)(1103U - 1025U);
    golden[0].Bbx[2].H = (UINT16)(535U - 482U);
    golden[0].Bbx[3].Cat = (UINT16)3U;
    golden[0].Bbx[3].X = (UINT16)813U;
    golden[0].Bbx[3].Y = (UINT16)483U;
    golden[0].Bbx[3].W = (UINT16)(853U - 813U);
    golden[0].Bbx[3].H = (UINT16)(563U - 483U);

    golden[1].NumBbx = 3U;
    golden[1].Bbx[0].Cat = (UINT16)255U;
    golden[1].Bbx[0].X = (UINT16)810U;
    golden[1].Bbx[0].Y = (UINT16)390U;
    golden[1].Bbx[0].W = (UINT16)(1110U - 810U);
    golden[1].Bbx[0].H = (UINT16)(690U - 390U);
    golden[1].Bbx[1].Cat = (UINT16)3U;
    golden[1].Bbx[1].X = (UINT16)908U;
    golden[1].Bbx[1].Y = (UINT16)513U;
    golden[1].Bbx[1].W = (UINT16)(979U - 908U);
    golden[1].Bbx[1].H = (UINT16)(574U - 513U);
    golden[1].Bbx[2].Cat = (UINT16)3U;
    golden[1].Bbx[2].X = (UINT16)860U;
    golden[1].Bbx[2].Y = (UINT16)511U;
    golden[1].Bbx[2].W = (UINT16)(907U - 860U);
    golden[1].Bbx[2].H = (UINT16)(548U - 511U);

    golden[2].NumBbx = 2U;
    golden[2].Bbx[0].Cat = (UINT16)255U;
    golden[2].Bbx[0].X = (UINT16)810U;
    golden[2].Bbx[0].Y = (UINT16)390U;
    golden[2].Bbx[0].W = (UINT16)(1110U - 810U);
    golden[2].Bbx[0].H = (UINT16)(690U - 390U);
    golden[2].Bbx[1].Cat = (UINT16)3U;
    golden[2].Bbx[1].X = (UINT16)852U;
    golden[2].Bbx[1].Y = (UINT16)504U;
    golden[2].Bbx[1].W = (UINT16)(1009U - 852U);
    golden[2].Bbx[1].H = (UINT16)(639U - 504U);

    golden[3].NumBbx = 4U;
    golden[3].Bbx[0].Cat = (UINT16)255U;
    golden[3].Bbx[0].X = (UINT16)810U;
    golden[3].Bbx[0].Y = (UINT16)390U;
    golden[3].Bbx[0].W = (UINT16)(1110U - 810U);
    golden[3].Bbx[0].H = (UINT16)(690U - 390U);
    golden[3].Bbx[1].Cat = (UINT16)3U;
    golden[3].Bbx[1].X = (UINT16)969U;
    golden[3].Bbx[1].Y = (UINT16)512U;
    golden[3].Bbx[1].W = (UINT16)(1103U - 969U);
    golden[3].Bbx[1].H = (UINT16)(579U - 512U);
    golden[3].Bbx[2].Cat = (UINT16)3U;
    golden[3].Bbx[2].X = (UINT16)810U;
    golden[3].Bbx[2].Y = (UINT16)516U;
    golden[3].Bbx[2].W = (UINT16)(914U - 810U);
    golden[3].Bbx[2].H = (UINT16)(604U - 516U);
    golden[3].Bbx[3].Cat = (UINT16)3U;
    golden[3].Bbx[3].X = (UINT16)905U;
    golden[3].Bbx[3].Y = (UINT16)518U;
    golden[3].Bbx[3].W = (UINT16)(980U - 905U);
    golden[3].Bbx[3].H = (UINT16)(576U - 518U);
#elif defined(CHIP_CV28)
    golden[0].NumBbx = 4U;
    golden[0].Bbx[0].Cat = (UINT16)255U;
    golden[0].Bbx[0].X = (UINT16)810U;
    golden[0].Bbx[0].Y = (UINT16)390U;
    golden[0].Bbx[0].W = (UINT16)(1110U - 810U);
    golden[0].Bbx[0].H = (UINT16)(690U-390U);
    golden[0].Bbx[1].Cat = (UINT16)3U;
    golden[0].Bbx[1].X = (UINT16)884U;
    golden[0].Bbx[1].Y = (UINT16)473U;
    golden[0].Bbx[1].W = (UINT16)(994U - 884U);
    golden[0].Bbx[1].H = (UINT16)(580U - 473U);
    golden[0].Bbx[2].Cat = (UINT16)3U;
    golden[0].Bbx[2].X = (UINT16)1026U;
    golden[0].Bbx[2].Y = (UINT16)482U;
    golden[0].Bbx[2].W = (UINT16)(1104U - 1026U);
    golden[0].Bbx[2].H = (UINT16)(535U - 482U);
    golden[0].Bbx[3].Cat = (UINT16)3U;
    golden[0].Bbx[3].X = (UINT16)812U;
    golden[0].Bbx[3].Y = (UINT16)482U;
    golden[0].Bbx[3].W = (UINT16)(854U - 812U);
    golden[0].Bbx[3].H = (UINT16)(564U - 482U);

    golden[1].NumBbx = 3U;
    golden[1].Bbx[0].Cat = (UINT16)255U;
    golden[1].Bbx[0].X = (UINT16)810U;
    golden[1].Bbx[0].Y = (UINT16)390U;
    golden[1].Bbx[0].W = (UINT16)(1110U - 810U);
    golden[1].Bbx[0].H = (UINT16)(690U - 390U);
    golden[1].Bbx[1].Cat = (UINT16)3U;
    golden[1].Bbx[1].X = (UINT16)908U;
    golden[1].Bbx[1].Y = (UINT16)515U;
    golden[1].Bbx[1].W = (UINT16)(981U - 908U);
    golden[1].Bbx[1].H = (UINT16)(575U - 515U);
    golden[1].Bbx[2].Cat = (UINT16)3U;
    golden[1].Bbx[2].X = (UINT16)859U;
    golden[1].Bbx[2].Y = (UINT16)511U;
    golden[1].Bbx[2].W = (UINT16)(909U - 859U);
    golden[1].Bbx[2].H = (UINT16)(548U - 511U);

    golden[2].NumBbx = 2U;
    golden[2].Bbx[0].Cat = (UINT16)255U;
    golden[2].Bbx[0].X = (UINT16)810U;
    golden[2].Bbx[0].Y = (UINT16)390U;
    golden[2].Bbx[0].W = (UINT16)(1110U - 810U);
    golden[2].Bbx[0].H = (UINT16)(690U - 390U);
    golden[2].Bbx[1].Cat = (UINT16)3U;
    golden[2].Bbx[1].X = (UINT16)846U;
    golden[2].Bbx[1].Y = (UINT16)504U;
    golden[2].Bbx[1].W = (UINT16)(1011U - 846U);
    golden[2].Bbx[1].H = (UINT16)(635U - 504U);

    golden[3].NumBbx = 3U;
    golden[3].Bbx[0].Cat = (UINT16)255U;
    golden[3].Bbx[0].X = (UINT16)810U;
    golden[3].Bbx[0].Y = (UINT16)390U;
    golden[3].Bbx[0].W = (UINT16)(1110U - 810U);
    golden[3].Bbx[0].H = (UINT16)(690U - 390U);
    golden[3].Bbx[1].Cat = (UINT16)3U;
    golden[3].Bbx[1].X = (UINT16)973U;
    golden[3].Bbx[1].Y = (UINT16)512U;
    golden[3].Bbx[1].W = (UINT16)(1104U - 973U);
    golden[3].Bbx[1].H = (UINT16)(579U - 512U);
    golden[3].Bbx[2].Cat = (UINT16)3U;
    golden[3].Bbx[2].X = (UINT16)904U;
    golden[3].Bbx[2].Y = (UINT16)518U;
    golden[3].Bbx[2].W = (UINT16)(979U - 904U);
    golden[3].Bbx[2].H = (UINT16)(576U - 518U);
#else
    golden[0].NumBbx = 4U;
    golden[0].Bbx[0].Cat = (UINT16)255U;
    golden[0].Bbx[0].X = (UINT16)810U;
    golden[0].Bbx[0].Y = (UINT16)390U;
    golden[0].Bbx[0].W = (UINT16)(1110U - 810U);
    golden[0].Bbx[0].H = (UINT16)(690U-390U);
    golden[0].Bbx[1].Cat = (UINT16)3U;
    golden[0].Bbx[1].X = (UINT16)884U;
    golden[0].Bbx[1].Y = (UINT16)472U;
    golden[0].Bbx[1].W = (UINT16)(994U - 884U);
    golden[0].Bbx[1].H = (UINT16)(579U - 472U);
    golden[0].Bbx[2].Cat = (UINT16)3U;
    golden[0].Bbx[2].X = (UINT16)1025U;
    golden[0].Bbx[2].Y = (UINT16)482U;
    golden[0].Bbx[2].W = (UINT16)(1103U - 1025U);
    golden[0].Bbx[2].H = (UINT16)(535U - 482U);
    golden[0].Bbx[3].Cat = (UINT16)3U;
    golden[0].Bbx[3].X = (UINT16)812U;
    golden[0].Bbx[3].Y = (UINT16)482U;
    golden[0].Bbx[3].W = (UINT16)(854U - 812U);
    golden[0].Bbx[3].H = (UINT16)(564U - 482U);

    golden[1].NumBbx = 3U;
    golden[1].Bbx[0].Cat = (UINT16)255U;
    golden[1].Bbx[0].X = (UINT16)810U;
    golden[1].Bbx[0].Y = (UINT16)390U;
    golden[1].Bbx[0].W = (UINT16)(1110U - 810U);
    golden[1].Bbx[0].H = (UINT16)(690U - 390U);
    golden[1].Bbx[1].Cat = (UINT16)3U;
    golden[1].Bbx[1].X = (UINT16)908U;
    golden[1].Bbx[1].Y = (UINT16)515U;
    golden[1].Bbx[1].W = (UINT16)(981U - 908U);
    golden[1].Bbx[1].H = (UINT16)(575U - 515U);
    golden[1].Bbx[2].Cat = (UINT16)3U;
    golden[1].Bbx[2].X = (UINT16)859U;
    golden[1].Bbx[2].Y = (UINT16)511U;
    golden[1].Bbx[2].W = (UINT16)(909U - 859U);
    golden[1].Bbx[2].H = (UINT16)(548U - 511U);

    golden[2].NumBbx = 2U;
    golden[2].Bbx[0].Cat = (UINT16)255U;
    golden[2].Bbx[0].X = (UINT16)810U;
    golden[2].Bbx[0].Y = (UINT16)390U;
    golden[2].Bbx[0].W = (UINT16)(1110U - 810U);
    golden[2].Bbx[0].H = (UINT16)(690U - 390U);
    golden[2].Bbx[1].Cat = (UINT16)3U;
    golden[2].Bbx[1].X = (UINT16)846U;
    golden[2].Bbx[1].Y = (UINT16)504U;
    golden[2].Bbx[1].W = (UINT16)(1011U - 846U);
    golden[2].Bbx[1].H = (UINT16)(635U - 504U);

    golden[3].NumBbx = 3U;
    golden[3].Bbx[0].Cat = (UINT16)255U;
    golden[3].Bbx[0].X = (UINT16)810U;
    golden[3].Bbx[0].Y = (UINT16)390U;
    golden[3].Bbx[0].W = (UINT16)(1110U - 810U);
    golden[3].Bbx[0].H = (UINT16)(690U - 390U);
    golden[3].Bbx[1].Cat = (UINT16)3U;
    golden[3].Bbx[1].X = (UINT16)973U;
    golden[3].Bbx[1].Y = (UINT16)512U;
    golden[3].Bbx[1].W = (UINT16)(1104U - 973U);
    golden[3].Bbx[1].H = (UINT16)(579U - 512U);
    golden[3].Bbx[2].Cat = (UINT16)3U;
    golden[3].Bbx[2].X = (UINT16)904U;
    golden[3].Bbx[2].Y = (UINT16)518U;
    golden[3].Bbx[2].W = (UINT16)(979U - 904U);
    golden[3].Bbx[2].H = (UINT16)(576U - 518U);
#endif
}

static uint32_t RefCV_BbxResult(REF_CV_HANDLE_s *handle)
{
    uint32_t ret = 0U,i;
    const AMBA_OD_s *pAmbaOD;
    VIS_UTIL_BBX_LIST_MSG_s BBX = {0};
    char print_buf[256];
    int value = 0;

    AmbaMisra_TypeCast(&pAmbaOD, &handle->out_buf.buf[0].pBuffer);
    BBX.NumBbx = pAmbaOD->Info.num_objects;
    for (i = 0U ; i < pAmbaOD->Info.num_objects; i++) {
        BBX.Bbx[i].X = (UINT16)pAmbaOD->Bbx[i].bb_start_col;
        BBX.Bbx[i].Y = (UINT16)pAmbaOD->Bbx[i].bb_start_row;
        BBX.Bbx[i].W = (UINT16)(pAmbaOD->Bbx[i].bb_width_m1 + 1U);
        BBX.Bbx[i].H = (UINT16)(pAmbaOD->Bbx[i].bb_height_m1 + 1U);
        BBX.Bbx[i].Cat = (UINT16)pAmbaOD->Bbx[i].clsId;
        BBX.Bbx[i].Score = pAmbaOD->Bbx[i].score;
    }

    //print out result
    if(golden[(handle->id%MAX_GOLDEN_NUM)].NumBbx != BBX.NumBbx) {
        ret = 1;
    }
    value += sprintf(&print_buf[value],"%s Result (%d) \n",handle->name,(UINT32)BBX.NumBbx);
    for (i=0U ; i<BBX.NumBbx ; i++) {
        value += sprintf(&print_buf[value],"%s (%d, %d)-(%d,%d) c:%d \n", \
                         handle->name, ((UINT32)BBX.Bbx[i].X)\
                         , ((UINT32)BBX.Bbx[i].Y)\
                         , ((UINT32)BBX.Bbx[i].X + (UINT32)BBX.Bbx[i].W)\
                         , ((UINT32)BBX.Bbx[i].Y + (UINT32)BBX.Bbx[i].H)\
                         , ((UINT32)BBX.Bbx[i].Cat));
        if( (golden[(handle->id%MAX_GOLDEN_NUM)].Bbx[i].X != BBX.Bbx[i].X) || (golden[(handle->id%MAX_GOLDEN_NUM)].Bbx[i].Y != BBX.Bbx[i].Y)
            ||(golden[(handle->id%MAX_GOLDEN_NUM)].Bbx[i].W != BBX.Bbx[i].W) || (golden[(handle->id%MAX_GOLDEN_NUM)].Bbx[i].H != BBX.Bbx[i].H)) {
            ret = 1;
        }
    }
    printf("%s ",print_buf);
    return ret;
}

static uint32_t RefCV_RunPicinfo(REF_CV_HANDLE_s *handle, cv_pic_info_t *in, AMBA_CV_FLEXIDAG_RUN_INFO_s *run_info)
{
    uint32_t ret = 0U, i;
    AMBA_CV_FLEXIDAG_HANDLE_s *fd_gen_handle = &handle->fd_gen_handle;
    cv_pic_info_t *pDataIn;
    uint32_t U32DataIn;

    handle->in_buf.num_of_buf = 1U;
    if(handle->in_buf.buf[0].pBuffer == NULL) {
        ret = AmbaCV_UtilityCmaMemAlloc(CACHE_ALIGN(sizeof(cv_pic_info_t)), 1,&handle->in_buf.buf[0]);
    } else {
        //printf("RefCV_RunPicinfo : name = %s handle->in_buf.buf[0].pBuffer != NULL\n", handle->name);
    }

    if (ret != 0) {
        printf("RefCV_RunPicinfo : name = %s AmbaCV_UtilityCmaMemAlloc in_buf 0 fail \n", handle->name);
        ret = 1U;
    } else {
        AmbaMisra_TypeCast(&pDataIn, &handle->in_buf.buf[0].pBuffer);
        memcpy(pDataIn, in, sizeof(cv_pic_info_t));

        U32DataIn = handle->in_buf.buf[0].buffer_daddr;
        for (i = 0U ; i < MAX_HALF_OCTAVES ; i++) {
            pDataIn->rpLumaLeft[i] = in->rpLumaLeft[i] - U32DataIn;
            pDataIn->rpLumaRight[i] = in->rpLumaRight[i] - U32DataIn;
            pDataIn->rpChromaLeft[i] = in->rpChromaLeft[i] - U32DataIn;
            pDataIn->rpChromaRight[i] = in->rpChromaRight[i] - U32DataIn;
        }
        AmbaCV_UtilityCmaMemClean(&handle->in_buf.buf[0]);

        handle->out_buf.num_of_buf = fd_gen_handle->mem_req.flexidag_num_outputs;
        for(i = 0; i < fd_gen_handle->mem_req.flexidag_num_outputs; i++) {
            if((handle->out_buf.buf[i].pBuffer == NULL) && (fd_gen_handle->mem_req.flexidag_output_buffer_size[i] != 0U)) {
                ret = AmbaCV_UtilityCmaMemAlloc(CACHE_ALIGN(fd_gen_handle->mem_req.flexidag_output_buffer_size[i]), 1,&handle->out_buf.buf[i]);
            } else {
                //printf("RefCV_RunPicinfo : name = %s handle->out_buf.buf[i].pBuffer != NULL\n", handle->name);
            }

            if (ret != 0) {
                printf("RefCV_RunPicinfo : name = %s AmbaCV_UtilityCmaMemAlloc out_buf %d fail \n", handle->name, i);
                ret = 1U;
                break;
            } else {
                if (fd_gen_handle->mem_req.flexidag_output_buffer_size[i] != 0U) {
                    AmbaCV_UtilityCmaMemInvalid(&handle->out_buf.buf[i]);
                }
            }
        }

        if (ret == 0) {
            (void) roi_handling(fd_gen_handle, pDataIn);
            ret = AmbaCV_FlexidagRun(fd_gen_handle, &handle->in_buf, &handle->out_buf, run_info);
            if (ret != 0) {
                printf("RefCV_RunPicinfo : name = %s AmbaCV_FlexidagRun fail \n", handle->name);
                ret = 1U;
            }
        }
    }
    return ret;
}

static void RefCV_RunCallback(void *vpHandle, uint32_t flexidag_output_num, flexidag_memblk_t *pblk_Output, void *vpParameter)
{
    uint32_t ret = 0U;

    REF_CV_PARAMS_s  *param = (REF_CV_PARAMS_s *)vpParameter;
    msgsnd(param->msg_queue, (void *)&ret, sizeof(ret), 0);
}

static uint32_t RefCV_RunNonBlockingPicinfo(REF_CV_HANDLE_s *handle, flexidag_cb cb, void *cb_param, cv_pic_info_t *in, uint32_t *token_id)
{
    uint32_t ret = 0U, i;
    AMBA_CV_FLEXIDAG_HANDLE_s *fd_gen_handle = &handle->fd_gen_handle;
    cv_pic_info_t *pDataIn;
    uint32_t U32DataIn;

    handle->in_buf.num_of_buf = 1U;
    if(handle->in_buf.buf[0].pBuffer == NULL) {
        ret = AmbaCV_UtilityCmaMemAlloc(CACHE_ALIGN(sizeof(cv_pic_info_t)), 1,&handle->in_buf.buf[0]);
    } else {
        //printf("RefCV_RunPicinfo : name = %s handle->in_buf.buf[0].pBuffer != NULL\n", handle->name);
    }

    if (ret != 0) {
        printf("RefCV_RunPicinfo : name = %s AmbaCV_UtilityCmaMemAlloc in_buf 0 fail \n", handle->name);
        ret = 1U;
    } else {
        AmbaMisra_TypeCast(&pDataIn, &handle->in_buf.buf[0].pBuffer);
        memcpy(pDataIn, in, sizeof(cv_pic_info_t));

        U32DataIn = handle->in_buf.buf[0].buffer_daddr;
        for (i = 0U ; i < MAX_HALF_OCTAVES ; i++) {
            pDataIn->rpLumaLeft[i] = in->rpLumaLeft[i] - U32DataIn;
            pDataIn->rpLumaRight[i] = in->rpLumaRight[i] - U32DataIn;
            pDataIn->rpChromaLeft[i] = in->rpChromaLeft[i] - U32DataIn;
            pDataIn->rpChromaRight[i] = in->rpChromaRight[i] - U32DataIn;
        }
        AmbaCV_UtilityCmaMemClean(&handle->in_buf.buf[0]);

        handle->out_buf.num_of_buf = fd_gen_handle->mem_req.flexidag_num_outputs;
        for(i = 0; i < fd_gen_handle->mem_req.flexidag_num_outputs; i++) {
            if((handle->out_buf.buf[i].pBuffer == NULL) && (fd_gen_handle->mem_req.flexidag_output_buffer_size[i] != 0U)) {
                ret = AmbaCV_UtilityCmaMemAlloc(CACHE_ALIGN(fd_gen_handle->mem_req.flexidag_output_buffer_size[i]), 1,&handle->out_buf.buf[i]);
            } else {
                //printf("RefCV_RunPicinfo : name = %s handle->out_buf.buf[i].pBuffer != NULL\n", handle->name);
            }

            if (ret != 0) {
                printf("RefCV_RunPicinfo : name = %s AmbaCV_UtilityCmaMemAlloc out_buf %d fail \n", handle->name, i);
                ret = 1U;
                break;
            } else {
                if (fd_gen_handle->mem_req.flexidag_output_buffer_size[i] != 0U) {
                    AmbaCV_UtilityCmaMemInvalid(&handle->out_buf.buf[i]);
                }
            }
        }

        if (ret == 0) {
            (void) roi_handling(fd_gen_handle, pDataIn);
            ret = AmbaCV_FlexidagRunNonBlock(fd_gen_handle, cb, cb_param, &handle->in_buf, &handle->out_buf, token_id);
            if (ret != 0) {
                printf("RefCV_RunPicinfo : name = %s AmbaCV_FlexidagRun fail \n", handle->name);
                ret = 1U;
            }
        }
    }
    return ret;

}

static uint32_t RefCV_Close(REF_CV_HANDLE_s *handle)
{
    uint32_t ret = 0U;
    AMBA_CV_FLEXIDAG_HANDLE_s *fd_gen_handle = &handle->fd_gen_handle;

    ret = AmbaCV_FlexidagClose(fd_gen_handle);
    if (ret != 0) {
        printf("RefCV_Close : name = %s AmbaCV_FlexidagClose fail \n", handle->name);
        ret = 1U;
    }

    if(AmbaCV_UtilityCmaMemFree(&handle->bin_buf) != 0 ) {
        printf("RefCV_Close : name = %s AmbaCV_UtilityCmaMemFree bin_buf fail \n", handle->name);
        ret = 1U;
    }

    if(AmbaCV_UtilityCmaMemFree(&handle->init.state_buf) != 0 ) {
        printf("RefCV_Close : name = %s AmbaCV_UtilityCmaMemFree state_buf fail \n", handle->name);
        ret = 1U;
    }
    return ret;
}

static void RefCV_TransferInit(uint32_t Channel, uint32_t OutType)
{
    if (AmbaIPC_FlexidagIO_Init(Channel) != FLEXIDAGIO_OK) {
        printf("RefCV_TransferInit AmbaIPC_FlexidagIO_Init fail");
    }

    if (AmbaIPC_FlexidagIO_Config(Channel, OutType) != FLEXIDAGIO_OK) {
        printf("RefCV_TransferInit AmbaIPC_FlexidagIO_Config fail");
    }
}

static void RefCV_TransferDeinit(uint32_t Channel)
{
    if (AmbaIPC_FlexidagIO_Deinit(Channel) != FLEXIDAGIO_OK) {
        printf("RefCV_TransferDeinit AmbaIPC_FlexidagIO_Deinit fail");
    }
}

static uint32_t RefCV_TransferSourceData(uint32_t Channel, memio_source_recv_picinfo_t *pInfo)
{
    UINT32 Rval = FLEXIDAGIO_OK;
    unsigned int GetLen;

    Rval = AmbaIPC_FlexidagIO_GetInput(Channel, pInfo, &GetLen);
    if (Rval != FLEXIDAGIO_OK) {
        printf("RefCV_TransferSourceData AmbaIPC_FlexidagIO_GetProcess fail");
    } else {
        g_frame_num = pInfo->pic_info.frame_num;
        printf("RefCV_TransferSourceData Channel=%d frame_num=%d rpLumaLeft=0x%x", Channel, pInfo->pic_info.frame_num, pInfo->pic_info.rpLumaLeft[0]);
    }

    return Rval;
}

static uint32_t RefCV_TransferSinkData(uint32_t Channel, UINT32 type, uint32_t BufOutAddr, uint32_t BufOutSize)
{
    UINT32 Rval = FLEXIDAGIO_OK;
    memio_sink_send_out_t SinkOut;

    SinkOut.type = type;
    SinkOut.cvtask_frameset_id = g_frame_num;
    SinkOut.num_of_io = 1U;
    SinkOut.io[0].addr = BufOutAddr;
    SinkOut.io[0].size= BufOutSize;
    Rval = AmbaIPC_FlexidagIO_SetResult(Channel, &SinkOut, sizeof(SinkOut));
    if (Rval != FLEXIDAGIO_OK) {
        printf("RefCV_TransferSinkData AmbaIPC_FlexidagIO_Result fail");
    } else {
        printf("RefCV_TransferSinkData Channel=%d frameset_id=%d CA=0x%x size=0x%x\n",
               Channel, SinkOut.cvtask_frameset_id, SinkOut.io[0].addr, SinkOut.io[0].size);
    }

    return Rval;
}

static void* RefCV_SensorInput(void *arg)
{
    UINT32 i;
    UINT32 Rval = 0;
    memio_source_recv_picinfo_t picinfo = {0};
    AMBA_CV_FLEXIDAG_RUN_INFO_s run_info = {0};
    uint32_t token_id;
    REF_CV_PARAMS_s  *param = (REF_CV_PARAMS_s *)arg;
    char log_path[128];
    uint16_t flow_id;
    AMBA_CV_FLEXIDAG_MSG_s msg;
    amba_roi_config_t roi_cfg;

    param->RunFlag  = 1;
    printf("process (%d) RefCV_SensorInput (%d) start \n",process_id,param->id);

    RefCV_Init(&param->handle);
    RefCV_TransferInit(param->data_channel_id, FLEXIDAGIO_RESULT_OUTTYPE_OD);

    (void) AmbaCV_FlexidagGetFlowIdByUUID(&param->handle.fd_gen_handle, MNETSSD_CVTASK_UUID, &flow_id);
    roi_cfg.msg_type = AMBA_ROI_CONFIG_MSG;
    roi_cfg.image_pyramid_index = idsp_pyramid_scale;
    roi_cfg.source_vin = source_vin;
    roi_cfg.roi_start_col = roi_start_col;
    roi_cfg.roi_start_row = roi_start_row;

    msg.flow_id = flow_id;
    msg.vpMessage = &roi_cfg;
    msg.length = sizeof(roi_cfg);
    (void) AmbaCV_FlexidagSendMsg(&param->handle.fd_gen_handle,&msg);

    while (1) {
        RefCV_TransferSourceData(param->data_channel_id, &picinfo);

        if(blocking_run == 1) {
            Rval = RefCV_RunPicinfo(&param->handle, &picinfo.pic_info, &run_info);
            if(Rval != 0) {
                break;
            }
        } else {
            Rval = RefCV_RunNonBlockingPicinfo(&param->handle, RefCV_RunCallback,(void *) param, &picinfo.pic_info, &token_id);
            if(Rval != 0) {
                break;
            } else {
                msgrcv( param->msg_queue, &Rval, sizeof(Rval), 0, 0) ;
                (void)AmbaCV_FlexidagWaitRunFinish(&param->handle.fd_gen_handle, token_id, &run_info);
            }
        }
        printf("process (%d) RefCV_SensorInput (%d):  start time (%d), end time (%d), valid (%d) \n",process_id,param->id, run_info.start_time, run_info.end_time, run_info.output_not_generated);
        if(run_info.output_not_generated == 1U) {
            continue;
        }
        printf("process (%d) RefCV_SensorInput (%d): RefCV_RunPicinfo run\n",process_id,param->id);
        if (Rval != 0) {
            break;
        } else {
            AmbaCV_UtilityCmaMemInvalid(&param->handle.out_buf.buf[0]);
            RefCV_TransferSinkData(param->data_channel_id, 0, param->handle.out_buf.buf[0].buffer_caddr, param->handle.out_buf.buf[0].buffer_size);
        }
    }

    for(i = 0; i < param->handle.in_buf.num_of_buf; i++) {
        if(AmbaCV_UtilityCmaMemFree(&param->handle.in_buf.buf[i]) != 0 ) {
            printf("process (%d) RefCV_SensorInput (%d) AmbaCV_UtilityCmaMemFree in_buf[%d] fail \n",process_id,param->id, i);
        }
    }

    for(i = 0; i < param->handle.out_buf.num_of_buf; i++) {
        if(AmbaCV_UtilityCmaMemFree(&param->handle.out_buf.buf[i]) != 0 ) {
            printf("process (%d) RefCV_SensorInput (%d) AmbaCV_UtilityCmaMemFree out_buf[%d] fail \n",process_id,param->id, i);
        }
    }

    snprintf(&log_path[0], sizeof(log_path), "./flexidag_log%d_%d.txt",process_id, param->id);
    AmbaCV_FlexidagDumpLog(&param->handle.fd_gen_handle,log_path, FLEXILOG_CORE0 | FLEXILOG_VIS_CVTASK);
    printf("process (%d) RefCV_SensorInput (%d) end RefCV_Close \n",process_id,param->id);
    RefCV_Close(&param->handle);
    printf("process (%d) RefCV_SensorInput (%d) end \n",process_id,param->id);
    param->RunFlag = 0;
    return NULL;
}

static void* RefCV_FileInput(void *arg)
{
    FILE *Fp;
    UINT32 i;
    UINT32 Rval = 0;
    UINT32 YuvWidth = 1920U;
    UINT32 YuvHeight = 1080U;
    UINT32 DataAddr,PaDataAddr;
    UINT32 PicInfoAddr;
    void* pVoidData;
    UINT32 NumSuccess = 0U;
    cv_pic_info_t pic_info = {0};
    cv_pic_info_t *pPicInfo;
    AMBA_CV_FLEXIDAG_RUN_INFO_s run_info = {0};
    uint32_t token_id;
    REF_CV_PARAMS_s  *param = (REF_CV_PARAMS_s *)arg;
    char log_path[128];
    uint16_t flow_id;
    AMBA_CV_FLEXIDAG_MSG_s msg;
    amba_roi_config_t roi_cfg;

    param->RunFlag  = 1;
    pPicInfo = &pic_info;
    printf("process (%d) RefCV_FileInput (%d) start \n",process_id,param->id);
    AmbaMisra_TypeCast32(&DataAddr, &param->file_buf.pBuffer);
    AmbaMisra_TypeCast(&pVoidData, &param->file_buf.pBuffer);
    AmbaMisra_TypeCast32(&PicInfoAddr, &pPicInfo);

    RefCV_Init(&param->handle);

    // prepare input data
    Fp = fopen(param->file_name, "rb");
    NumSuccess = fread(pVoidData, YuvWidth * YuvHeight * 3 / 2, 1U, Fp);
    if (NumSuccess <= 0U) {
        printf("process (%d) thread (%d) fread failed", process_id, param->id);
    }
    fclose(Fp);
    if(param->file_buf.buffer_cacheable != 0) {
        AmbaCV_UtilityCmaMemClean(&param->file_buf);
    }

    pPicInfo->capture_time = 0U;
    pPicInfo->channel_id = 0U;
    pPicInfo->frame_num = 0U;
    pPicInfo->pyramid.image_width_m1 = YuvWidth - 1U;
    pPicInfo->pyramid.image_height_m1 = YuvHeight - 1U;
    pPicInfo->pyramid.image_pitch_m1 = YuvWidth - 1U;
    pPicInfo->pyramid.half_octave[0].ctrl.roi_pitch = YuvWidth;
    pPicInfo->pyramid.half_octave[0].roi_start_col = 0;
    pPicInfo->pyramid.half_octave[0].roi_start_row = 0;
    pPicInfo->pyramid.half_octave[0].roi_width_m1 = YuvWidth - 1U;
    pPicInfo->pyramid.half_octave[0].roi_height_m1 = YuvHeight - 1U;

    pPicInfo->pyramid.half_octave[1].ctrl.disable = 1U;
    pPicInfo->pyramid.half_octave[2].ctrl.disable = 1U;
    pPicInfo->pyramid.half_octave[3].ctrl.disable = 1U;
    pPicInfo->pyramid.half_octave[4].ctrl.disable = 1U;
    pPicInfo->pyramid.half_octave[5].ctrl.disable = 1U;

    PaDataAddr = param->file_buf.buffer_daddr;
    pPicInfo->rpLumaLeft[0] = PaDataAddr;
    pPicInfo->rpChromaLeft[0] = (PaDataAddr + (YuvWidth * YuvHeight));
    pPicInfo->rpLumaRight[0] = pPicInfo->rpLumaLeft[0];
    pPicInfo->rpChromaRight[0] = pPicInfo->rpChromaLeft[0];

    (void) AmbaCV_FlexidagGetFlowIdByUUID(&param->handle.fd_gen_handle, MNETSSD_CVTASK_UUID, &flow_id);
    roi_cfg.msg_type = AMBA_ROI_CONFIG_MSG;
    roi_cfg.image_pyramid_index = idsp_pyramid_scale;
    roi_cfg.source_vin = source_vin;
    roi_cfg.roi_start_col = roi_start_col;
    roi_cfg.roi_start_row = roi_start_row;

    msg.flow_id = flow_id;
    msg.vpMessage = &roi_cfg;
    msg.length = sizeof(roi_cfg);
    (void) AmbaCV_FlexidagSendMsg(&param->handle.fd_gen_handle,&msg);
    for (i = 0; i < param->num_runs; i++) {
        if(blocking_run == 1) {
            Rval = RefCV_RunPicinfo(&param->handle, pPicInfo, &run_info);
            if(Rval != 0) {
                break;
            }
        } else {
            Rval = RefCV_RunNonBlockingPicinfo(&param->handle, RefCV_RunCallback,(void *) param, pPicInfo, &token_id);
            if(Rval != 0) {
                break;
            } else {
                msgrcv( param->msg_queue, &Rval, sizeof(Rval), 0, 0) ;
                (void)AmbaCV_FlexidagWaitRunFinish(&param->handle.fd_gen_handle, token_id, &run_info);
            }
        }
        printf("process (%d) RefCV_FileInput (%d):  start time (%d), end time (%d), valid (%d) \n",process_id,param->id, run_info.start_time, run_info.end_time, run_info.output_not_generated);
        if(run_info.output_not_generated == 1U) {
            continue;
        }
        printf("process (%d) RefCV_FileInput (%d): RefCV_RunPicinfo run %d \n",process_id,param->id, i);
        if (Rval != 0) {
            break;
        } else {
            AmbaCV_UtilityCmaMemInvalid(&param->handle.out_buf.buf[0]);
            Rval = RefCV_BbxResult(&param->handle);
            if(Rval != 0) {
                param->num_err += 1;
            }
        }
        memset(param->handle.out_buf.buf[0].pBuffer, 0x0, param->handle.out_buf.buf[0].buffer_size);
        AmbaCV_UtilityCmaMemClean(&param->handle.out_buf.buf[0]);
        param->cur_runs += 1;
        usleep(param->sleep_usec);
    }

    for(i = 0; i < param->handle.in_buf.num_of_buf; i++) {
        if(AmbaCV_UtilityCmaMemFree(&param->handle.in_buf.buf[i]) != 0 ) {
            printf("process (%d) RefCV_FileInput (%d) AmbaCV_UtilityCmaMemFree in_buf[%d] fail \n",process_id,param->id, i);
        }
    }

    for(i = 0; i < param->handle.out_buf.num_of_buf; i++) {
        if(param->handle.out_buf.buf[i].pBuffer != NULL) {
            if(AmbaCV_UtilityCmaMemFree(&param->handle.out_buf.buf[i]) != 0 ) {
                printf("process (%d) RefCV_FileInput (%d) AmbaCV_UtilityCmaMemFree out_buf[%d] fail \n",process_id,param->id, i);
            }
        }
    }

    snprintf(&log_path[0], sizeof(log_path), "./flexidag_log%d_%d.txt",process_id, param->id);
    AmbaCV_FlexidagDumpLog(&param->handle.fd_gen_handle,log_path, FLEXILOG_CORE0 | FLEXILOG_VIS_CVTASK);
    printf("process (%d) RefCV_FileInput (%d) end RefCV_Close \n",process_id,param->id);
    RefCV_Close(&param->handle);
    printf("process (%d) RefCV_FileInput (%d) end \n",process_id,param->id);
    param->RunFlag = 0;
    return NULL;
}

static void RefCV_PrintHelp(const char *exe)
{
    printf("\nusage: %s [args]\n", exe);
    printf("\t-f [flexibin folder path]\n");
    printf("\t-d [Input data folder path]\n");
    printf("\t-n [number of frame]\n");
    printf("\t-p [process id]\n");
    printf("\t-p [thread num] not more than 4\n");
    printf("\t-s [0 : schdr omit shutdown, 1 : schdr will shutdown]\n");
    printf("\t-b [blocking run]\n");
    printf("\t-q [run frequency]\n");
    printf("\t-r [run mode] 0:file mode, 1:sensor mode\n");
    printf("\t-i [data channel id]\n");
    printf("\t-e [external msg thread]\n");
    printf("\nfor example: %s \n", exe);
}

static void RefCV_ParseOpt(int argc, char **argv)
{
    int c;

    optind = 1;
    while ((c = getopt(argc, argv, OPTIONS)) != -1) {
        switch (c) {
        case 'n':
            num_of_frame = strtoul(optarg, NULL, 0);
            printf("num_of_frame = %d\n", num_of_frame);
            break;
        case 'f':
            strcpy(flexidag_path, optarg);
            break;
        case 'd':
            strcpy(data_path, optarg);
            break;
        case 'p':
            process_id = strtoul(optarg, NULL, 0);
            break;
        case 't':
            thread_id = strtoul(optarg, NULL, 0);
            break;
        case 's':
            schdr_close = strtoul(optarg, NULL, 0);
            break;
        case 'b':
            blocking_run = strtoul(optarg, NULL, 0);
            break;
        case 'r':
            run_mode = strtoul(optarg, NULL, 0);
            break;
        case 'i':
            data_channel_id = strtoul(optarg, NULL, 0);
            break;
        case 'e':
            external_msg_thread = 1;
            break;
        default:
            RefCV_PrintHelp(argv[0]);
            exit(-1);
        }
    }
}

static pthread_t cv_msg_thread;
static int cv_msg_loop = 1;

static void* cv_msg_tsk(void* arg)
{
    unsigned int msg_num, i;
    unsigned int retcode;
    unsigned int message_type, message_retcode;
    AMBA_CV_FLEXIDAG_HANDLE_s *phandle;

    (void)arg;
    while(cv_msg_loop == 1U) {
        retcode = AmbaCV_SchdrWaitMsg(&msg_num);
        if(retcode == ERRCODE_NONE) {
            for(i = 0U; i < msg_num; i++) {
                retcode = AmbaCV_SchdrProcessMsg(&phandle, &message_type, &message_retcode);
            }
        } else {
            printf("[ERROR] cv_msg_tsk() : AmbaCV_SchdrWaitMsg fail ret (0x%x)", retcode);
            break;
        }
    }

    return NULL;
}

static void RefCV_CleanUp(void)
{
    uint32_t i;

    if (run_mode == MODE_SENSOR_MODE) {
        for(i = 0U; i < thread_id; i ++) {
            RefCV_TransferDeinit(thread_params[i].data_channel_id);
        }
    }

    /* de-init scheduler*/
    if (schdr_close == 1) {
        AmbaCV_SchdrShutdown(0);
    }
    if (external_msg_thread != 0) {
        cv_msg_loop = 0;
    }
}

static void RefCV_SignalHandlerShutdown(int sig)
{
    printf("%s: Got signal %d, program exits!\n",__FILE__,sig);
    exit(0);
}

int main(int argc, char **argv)
{
    uint32_t i;
    AMBA_CV_FLEXIDAG_SCHDR_CFG_s cfg;
    AMBA_CV_FLEXIDAG_LOG_MSG_s set;

    atexit(RefCV_CleanUp);
    /* Allow ourselves to be shut down gracefully by a signal */
    signal(SIGTERM, RefCV_SignalHandlerShutdown);
    signal(SIGHUP, RefCV_SignalHandlerShutdown);
    signal(SIGUSR1, RefCV_SignalHandlerShutdown);
    signal(SIGQUIT, RefCV_SignalHandlerShutdown);
    signal(SIGINT, RefCV_SignalHandlerShutdown);
    signal(SIGKILL, RefCV_SignalHandlerShutdown);

    RefCV_ParseOpt(argc, argv);
    if(thread_id > MAX_THREAD_NUM) {
        printf("[error] thread_num(%d) > MAX_THREAD_NUM(%d) \n",thread_id,MAX_THREAD_NUM);
        return 0;
    }
    printf("process_id = %d thread_num = %d flexidag path = %s data path = %s num_of_frame = %d blocking_run = %d \n",process_id,thread_id,flexidag_path,data_path,num_of_frame,blocking_run);

    RefCV_GoldenInit();

    if (external_msg_thread != 0) {
        printf("create schdr cmd thread \n");
        pthread_create(&cv_msg_thread, NULL, cv_msg_tsk, NULL);
        //pthread_setschedprio(cv_msg_thread, 90U);
    }

    /* init scheduler */
    cfg.cpu_map = 0xD;
    cfg.log_level = LVL_DEBUG;
    AmbaCV_FlexidagSchdrStart(&cfg);

    for(i = 0U; i < thread_id; i ++) {
        memset(&thread_params[i].handle, 0x0, sizeof(REF_CV_HANDLE_s));

        thread_params[i].id = i;
        thread_params[i].num_runs   = num_of_frame;
        thread_params[i].sleep_usec = 20*1000*i;
        thread_params[i].RunFlag    = 0;
        thread_params[i].data_channel_id     = data_channel_id + i;
        snprintf(&thread_params[i].flexidag_path[0], sizeof(thread_params[i].flexidag_path), "%s/flexibin0.bin",flexidag_path);
        if(run_mode == MODE_FILE_MODE) {
            snprintf(&thread_params[i].file_name[0], sizeof(thread_params[i].file_name), "%s/%d.yuv",data_path, i%MAX_GOLDEN_NUM);
            AmbaCV_UtilityCmaMemAlloc(MAX_FILE_SIZE, 1,&thread_params[i].file_buf);
        }

        snprintf(&thread_params[i].handle.name[0], sizeof(thread_params[i].handle.name), "00%02d-00%02d",process_id, i);
        thread_params[i].handle.id = i;
        if(blocking_run == 0) {
            thread_params[i].msg_queue = msgget((key_t)(i | MNETSSD_KEY), 0666 | IPC_CREAT);
        }
        printf("process (%d) thread_params[%d].num_runs            = %d \n",process_id,i,thread_params[i].num_runs);
        printf("process (%d) thread_params[%d].sleep_usec          = %d \n",process_id,i,thread_params[i].sleep_usec);
        printf("process (%d) thread_params[%d].flexidag_path       = %s \n",process_id,i,thread_params[i].flexidag_path);
        if(run_mode == MODE_FILE_MODE) {
            printf("process (%d) thread_params[%d].file_name           = %s \n",process_id,i,thread_params[i].file_name);
            printf("process (%d) thread_params[%d].file va             = %p \n",process_id,i,thread_params[i].file_buf.pBuffer);
            printf("process (%d) thread_params[%d].file pa             = %08x \n",process_id,i,thread_params[i].file_buf.buffer_daddr);
        }

        set.flexidag_msg_entry = 32U;       //no requirement to send msg from flexidag to cvtask
        set.cvtask_msg_entry = 32U;         //no requirement to send internal cvtask msg
        set.arm_cpu_map = 0xFU;
        set.arm_log_entry = 2048U;
        set.orc_log_entry = 2048U;
        AmbaCV_FlexidagSetParamSet(&thread_params[i].handle.fd_gen_handle, FLEXIDAG_PARAMSET_LOG_MSG, (void *)&set, sizeof(AMBA_CV_FLEXIDAG_LOG_MSG_s));
        RefCV_Open(thread_params[i].flexidag_path, &thread_params[i].handle);
    }

    // alloc global temp buffer
    if (thread_params[0].handle.fd_gen_handle.mem_req.flexidag_temp_buffer_size != 0) {
        AmbaCV_UtilityCmaMemAlloc(CACHE_ALIGN(thread_params[0].handle.fd_gen_handle.mem_req.flexidag_temp_buffer_size), 1,&g_temp_buf);
    }

    for(i = 0U; i < thread_id; i ++) {
        if(run_mode == MODE_FILE_MODE) {
            pthread_create(&thread_params[i].pthread, NULL, RefCV_FileInput, &thread_params[i]);
        } else {
            pthread_create(&thread_params[i].pthread, NULL, RefCV_SensorInput, &thread_params[i]);
        }
    }

    for(i = 0U; i < thread_id; i ++) {
        pthread_join(thread_params[i].pthread, NULL);
    }

    for(i = 0U; i < thread_id; i ++) {
        if(run_mode == MODE_FILE_MODE) {
            AmbaCV_UtilityCmaMemFree(&thread_params[i].file_buf);
        }
    }

    if(g_temp_buf.pBuffer != NULL) {
        if(AmbaCV_UtilityCmaMemFree(&g_temp_buf) != 0 ) {
            printf("main : AmbaCV_UtilityCmaMemFree g_temp_buf fail \n");
        }
    }

    for(i = 0U; i < thread_id; i ++) {
        printf("main process (%d) thread (%d) total (%d) error (%d) \n",process_id,thread_params[i].id, thread_params[i].cur_runs, thread_params[i].num_err);
    }

    if (schdr_close == 1) {
        printf("main process (%d) schdr_shutdown \n",process_id);
        AmbaCV_SchdrShutdown(0);
    }
    printf("main process (%d) finish \n",process_id);

    if (external_msg_thread != 0) {
        cv_msg_loop = 0;
    }
    return 0;
}
