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
#include "ppm.h"
#include "ambint.h"
#include "cvapi_memio_interface.h"
#include "cvapi_svccvalgo_memio_interface.h"
#include "cvapi_ambacv_flexidag.h"

#define DEFAULT_KEY 	0xaabbccddeeff
#define DEFAULT_INET	"127.0.0.1"
#define DEFAULT_PORT	8699
#define OPTIONS 		"f:n:p:t:d:c:s:b:m:q:r:e"

#ifndef CONFIG_CV_CONFIG_TX_SDK7
#define AmbaMisra_TouchUnused(x)
#define AmbaMisra_TypeCast32(a, b) memcpy(a, b, sizeof(void *))
#define AmbaWrap_memcpy memcpy
#define AmbaWrap_memset memset
#define AmbaUtility_StringCopy(a, b, c) strncpy(a, c, b)
#define AmbaPrint_PrintUInt5(fmt, p1, p2, p3, p4, p5) printf(fmt"\n", p1, p2, p3, p4, p5)
#define AmbaPrint_PrintStr5(fmt, p1, p2, p3, p4, p5) printf(fmt"\n", p1, p2, p3, p4, p5)

#define AMBAOD_MAX_BBX_NUM	200

typedef unsigned char UINT8;
typedef unsigned short UINT16;
typedef unsigned int UINT32;
typedef int INT32;
#endif

#define BISENET_KEY					0xDD
#define BISENET_CVTASK_UUID			4
#define BISENET_CVTABLE				"DATA_SOURCE_00_IONAME_000"

#define CACHE_ALIGN(x)				((x + 0x3F)& ~0x3F)

#define MAX_THREAD_NUM				15
#define MAX_GOLDEN_NUM				4
#define MAX_FLEXIDAG_SIZE			0x5000000
#define	MAX_FILE_Y_SIZE				0x0200000
#define	MAX_FILE_UV_SIZE			0x0200000
#define	MAX_FILE_SIZE				(MAX_FILE_Y_SIZE + MAX_FILE_UV_SIZE)

typedef struct {
    uint32_t						id;
    AMBA_CV_FLEXIDAG_HANDLE_s		fd_gen_handle;
    char							name[64];
    AMBA_CV_FLEXIDAG_INIT_s			init;
    flexidag_memblk_t				bin_buf;
    AMBA_CV_FLEXIDAG_IO_s			in_buf;
    AMBA_CV_FLEXIDAG_IO_s			out_buf;
} REF_CV_HANDLE_s;

typedef struct {
    uint32_t						id;
    uint32_t						num_runs;
    uint32_t						cur_runs;
    uint32_t						num_err;
    uint32_t						sleep_usec;
    uint32_t						RunFlag;
    char							fileY_name[128];
    char							fileUV_name[128];
    char							flexidag_path[128];
    REF_CV_HANDLE_s					handle;
    pthread_t						pthread;
    int 							msg_queue;
    flexidag_memblk_t				file_bufY;
    flexidag_memblk_t				file_bufUV;
} REF_CV_PARAMS_s;


#define MSGSZ	4
typedef struct msgbuf {
    long mtype;
    uint32_t mtext[1];
} message_buf;

#define getNumberInArray(a) (sizeof(a) / sizeof(a[0]))

static int external_msg_thread = 0;
static uint8_t dump_perf = 0;
static int msg_send = 1;
static int non_blocking_run = 0; //default to use blocking run
static int error_out = 0;
static int num_of_frame = 1;
static int schdr_close = 0;
static int process_id = 0;
static int thread_id = MAX_THREAD_NUM;
static char flexidag_path[128] = "../flexibin";
static char data_path[128] = "../golden";
static REF_CV_PARAMS_s thread_params[MAX_THREAD_NUM];

static flexidag_memblk_t g_temp_buf;

static UINT32 Crc32Table[] = {
    0x00000000U, 0x77073096U, 0xee0e612cU, 0x990951baU, 0x076dc419U, 0x706af48fU,
    0xe963a535U, 0x9e6495a3U, 0x0edb8832U, 0x79dcb8a4U, 0xe0d5e91eU, 0x97d2d988U,
    0x09b64c2bU, 0x7eb17cbdU, 0xe7b82d07U, 0x90bf1d91U, 0x1db71064U, 0x6ab020f2U,
    0xf3b97148U, 0x84be41deU, 0x1adad47dU, 0x6ddde4ebU, 0xf4d4b551U, 0x83d385c7U,
    0x136c9856U, 0x646ba8c0U, 0xfd62f97aU, 0x8a65c9ecU, 0x14015c4fU, 0x63066cd9U,
    0xfa0f3d63U, 0x8d080df5U, 0x3b6e20c8U, 0x4c69105eU, 0xd56041e4U, 0xa2677172U,
    0x3c03e4d1U, 0x4b04d447U, 0xd20d85fdU, 0xa50ab56bU, 0x35b5a8faU, 0x42b2986cU,
    0xdbbbc9d6U, 0xacbcf940U, 0x32d86ce3U, 0x45df5c75U, 0xdcd60dcfU, 0xabd13d59U,
    0x26d930acU, 0x51de003aU, 0xc8d75180U, 0xbfd06116U, 0x21b4f4b5U, 0x56b3c423U,
    0xcfba9599U, 0xb8bda50fU, 0x2802b89eU, 0x5f058808U, 0xc60cd9b2U, 0xb10be924U,
    0x2f6f7c87U, 0x58684c11U, 0xc1611dabU, 0xb6662d3dU, 0x76dc4190U, 0x01db7106U,
    0x98d220bcU, 0xefd5102aU, 0x71b18589U, 0x06b6b51fU, 0x9fbfe4a5U, 0xe8b8d433U,
    0x7807c9a2U, 0x0f00f934U, 0x9609a88eU, 0xe10e9818U, 0x7f6a0dbbU, 0x086d3d2dU,
    0x91646c97U, 0xe6635c01U, 0x6b6b51f4U, 0x1c6c6162U, 0x856530d8U, 0xf262004eU,
    0x6c0695edU, 0x1b01a57bU, 0x8208f4c1U, 0xf50fc457U, 0x65b0d9c6U, 0x12b7e950U,
    0x8bbeb8eaU, 0xfcb9887cU, 0x62dd1ddfU, 0x15da2d49U, 0x8cd37cf3U, 0xfbd44c65U,
    0x4db26158U, 0x3ab551ceU, 0xa3bc0074U, 0xd4bb30e2U, 0x4adfa541U, 0x3dd895d7U,
    0xa4d1c46dU, 0xd3d6f4fbU, 0x4369e96aU, 0x346ed9fcU, 0xad678846U, 0xda60b8d0U,
    0x44042d73U, 0x33031de5U, 0xaa0a4c5fU, 0xdd0d7cc9U, 0x5005713cU, 0x270241aaU,
    0xbe0b1010U, 0xc90c2086U, 0x5768b525U, 0x206f85b3U, 0xb966d409U, 0xce61e49fU,
    0x5edef90eU, 0x29d9c998U, 0xb0d09822U, 0xc7d7a8b4U, 0x59b33d17U, 0x2eb40d81U,
    0xb7bd5c3bU, 0xc0ba6cadU, 0xedb88320U, 0x9abfb3b6U, 0x03b6e20cU, 0x74b1d29aU,
    0xead54739U, 0x9dd277afU, 0x04db2615U, 0x73dc1683U, 0xe3630b12U, 0x94643b84U,
    0x0d6d6a3eU, 0x7a6a5aa8U, 0xe40ecf0bU, 0x9309ff9dU, 0x0a00ae27U, 0x7d079eb1U,
    0xf00f9344U, 0x8708a3d2U, 0x1e01f268U, 0x6906c2feU, 0xf762575dU, 0x806567cbU,
    0x196c3671U, 0x6e6b06e7U, 0xfed41b76U, 0x89d32be0U, 0x10da7a5aU, 0x67dd4accU,
    0xf9b9df6fU, 0x8ebeeff9U, 0x17b7be43U, 0x60b08ed5U, 0xd6d6a3e8U, 0xa1d1937eU,
    0x38d8c2c4U, 0x4fdff252U, 0xd1bb67f1U, 0xa6bc5767U, 0x3fb506ddU, 0x48b2364bU,
    0xd80d2bdaU, 0xaf0a1b4cU, 0x36034af6U, 0x41047a60U, 0xdf60efc3U, 0xa867df55U,
    0x316e8eefU, 0x4669be79U, 0xcb61b38cU, 0xbc66831aU, 0x256fd2a0U, 0x5268e236U,
    0xcc0c7795U, 0xbb0b4703U, 0x220216b9U, 0x5505262fU, 0xc5ba3bbeU, 0xb2bd0b28U,
    0x2bb45a92U, 0x5cb36a04U, 0xc2d7ffa7U, 0xb5d0cf31U, 0x2cd99e8bU, 0x5bdeae1dU,
    0x9b64c2b0U, 0xec63f226U, 0x756aa39cU, 0x026d930aU, 0x9c0906a9U, 0xeb0e363fU,
    0x72076785U, 0x05005713U, 0x95bf4a82U, 0xe2b87a14U, 0x7bb12baeU, 0x0cb61b38U,
    0x92d28e9bU, 0xe5d5be0dU, 0x7cdcefb7U, 0x0bdbdf21U, 0x86d3d2d4U, 0xf1d4e242U,
    0x68ddb3f8U, 0x1fda836eU, 0x81be16cdU, 0xf6b9265bU, 0x6fb077e1U, 0x18b74777U,
    0x88085ae6U, 0xff0f6a70U, 0x66063bcaU, 0x11010b5cU, 0x8f659effU, 0xf862ae69U,
    0x616bffd3U, 0x166ccf45U, 0xa00ae278U, 0xd70dd2eeU, 0x4e048354U, 0x3903b3c2U,
    0xa7672661U, 0xd06016f7U, 0x4969474dU, 0x3e6e77dbU, 0xaed16a4aU, 0xd9d65adcU,
    0x40df0b66U, 0x37d83bf0U, 0xa9bcae53U, 0xdebb9ec5U, 0x47b2cf7fU, 0x30b5ffe9U,
    0xbdbdf21cU, 0xcabac28aU, 0x53b39330U, 0x24b4a3a6U, 0xbad03605U, 0xcdd70693U,
    0x54de5729U, 0x23d967bfU, 0xb3667a2eU, 0xc4614ab8U, 0x5d681b02U, 0x2a6f2b94U,
    0xb40bbe37U, 0xc30c8ea1U, 0x5a05df1bU, 0x2d02ef8dU
};

static uint32_t bisenet_golden[MAX_GOLDEN_NUM] = {
#if defined(CHIP_CV2A) || defined(CHIP_CV2FS) || defined(CHIP_CV22FS) || defined(CHIP_CV5) || defined(CHIP_CV52)
    0xf3647648U,
    0xc806f96dU,
    0x3f943256U,
    0x1e66fd8dU,
#elif defined(CHIP_CV28)
    0x8707bb19U,
    0x2f8c2e71U,
    0x9f1fb42bU,
    0x5452d506U,
#elif defined(CHIP_CV6)
    0x113fa428U,
    0xbbe5f46dU,
    0x1ac2faa8U,
    0x6d26d411U,
#else
    0xbd29bf68U,
    0x553ad91fU,
    0xdca92e6cU,
    0x329d7a64U
#endif
};

static int log_golden[MAX_THREAD_NUM] = {0};

static uint32_t RefCV_Open(const char *path, REF_CV_HANDLE_s *handle)
{
    uint32_t ret = 0U;
    AMBA_CV_FLEXIDAG_HANDLE_s *fd_gen_handle = &handle->fd_gen_handle;
    uint32_t size_align;

    ret = AmbaCV_UtilityFileSize(path, &size_align);
    if (ret != 0U) {
        printf("RefCV_Open : name = %s AmbaCV_UtilityFileSize fail path = %s\n", handle->name, path);
        ret = 1U;
    } else {
        if(handle->bin_buf.pBuffer == NULL) {
            ret = AmbaCV_UtilityCmaMemAlloc(size_align, 1, &handle->bin_buf);
        } else {
            //printf("RefCV_Open : name = %s handle->bin_buf.pBuffer != NULL\n", handle->name);
        }

        if (ret != 0U) {
            printf("RefCV_Open : name = %s AmbaCV_UtilityCmaMemAlloc fail path = %s\n", handle->name, path);
            ret = 1U;
        } else {
            ret = AmbaCV_UtilityFileLoad(path, &handle->bin_buf);
            if (ret != 0U) {
                printf("RefCV_Open : name = %s AmbaCV_UtilityFileLoad fail path = %s\n", handle->name, path);
                ret = 1U;
            } else {
                ret = AmbaCV_FlexidagOpen(&handle->bin_buf, fd_gen_handle);
                if (ret != 0U) {
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
        ret = AmbaCV_UtilityCmaMemAlloc(CACHE_ALIGN(fd_gen_handle->mem_req.flexidag_state_buffer_size), 1, &handle->init.state_buf);
    } else {
        //printf("RefCV_Init : name = %s handle->init.state_buf.pBuffer != NULL\n", handle->name);
    }

    if (ret != 0U) {
        printf("RefCV_Init : name = %s AmbaCV_UtilityCmaMemAlloc state_buf fail \n", handle->name);
        ret = 1U;
    } else {
        if(handle->init.temp_buf.pBuffer == NULL) {
            handle->init.temp_buf.buffer_size = g_temp_buf.buffer_size;
            handle->init.temp_buf.buffer_daddr	= g_temp_buf.buffer_daddr;
            handle->init.temp_buf.buffer_cacheable = g_temp_buf.buffer_cacheable;
            handle->init.temp_buf.pBuffer		 = g_temp_buf.pBuffer;
        } else {
            //printf("RefCV_Init : name = %s handle->init.temp_buf.pBuffer != NULL\n", handle->name);
        }

        if (ret != 0U) {
            printf("RefCV_Init : name = %s AmbaCV_UtilityCmaMemAlloc temp_buf fail \n", handle->name);
            ret = 1U;
        } else {
            ret = AmbaCV_FlexidagInit(fd_gen_handle, &handle->init);
            if (ret != 0U) {
                printf("RefCV_Init : name = %s AmbaCV_FlexidagInit fail \n", handle->name);
                ret = 1U;
            }
        }
    }

    return ret;
}

UINT32 RefCV_Crc32(const UINT8 *pBuffer, UINT32 Size)
{
    const UINT8 *p;
    UINT32 Crc;
    UINT32 idx;

    p = pBuffer;
    Crc = ~0U;

    while (Size > 0U) {
        idx = (Crc ^ (UINT32)*p) & 0x0FFU;
        Crc = Crc32Table[idx] ^ (Crc >> 8U);
        p++;
        Size--;
    }

    return Crc ^ ~0U;
}

static UINT32 RefCV_PrintCrc(UINT32 id, const flexidag_memblk_t *pBlk)
{
    UINT32 Rval = 0U, Crc;
    const UINT8 *ptr;

    AmbaMisra_TypeCast32(&ptr, &pBlk->pBuffer);
    Crc = RefCV_Crc32(ptr, pBlk->buffer_size);
    AmbaPrint_PrintUInt5("RefCV_PrintCrc (%d) crc = %08x ",id, Crc, 0U, 0U, 0U);
    if(Crc != bisenet_golden[id]) {
        Rval = 1U;
    }
    return Rval;
}

static uint32_t RefCV_RunRaw(REF_CV_HANDLE_s *handle, memio_source_recv_multi_raw_t *in, AMBA_CV_FLEXIDAG_RUN_INFO_s *run_info)
{
    uint32_t ret = 0U, i;
    AMBA_CV_FLEXIDAG_HANDLE_s *fd_gen_handle = &handle->fd_gen_handle;
    memio_source_recv_raw_t *pDataIn;
    char log_path[128];

    handle->in_buf.num_of_buf = in->num_io;
    for(i = 0 ; i < in->num_io ; i++) {
        if(handle->in_buf.buf[i].pBuffer == NULL) {
            ret = AmbaCV_UtilityCmaMemAlloc(CACHE_ALIGN(sizeof(memio_source_recv_raw_t)), 1, &handle->in_buf.buf[i]);
        } else {
            //printf("RefCV_RunRaw : name = %s handle->in_buf.buf[0].pBuffer != NULL\n", handle->name);
        }

        if (ret != 0U) {
            printf("RefCV_RunRaw : name = %s AmbaCV_UtilityCmaMemAlloc in_buf %d fail \n", handle->name,i);
            ret = 1U;
            break;
        } else {
            AmbaMisra_TypeCast32(&pDataIn, &handle->in_buf.buf[i].pBuffer);
            memcpy(pDataIn, &in->io[i], sizeof(memio_source_recv_raw_t));
            AmbaCV_UtilityCmaMemClean(&handle->in_buf.buf[i]);
        }
    }

    if(ret == 0U) {
        handle->out_buf.num_of_buf = fd_gen_handle->mem_req.flexidag_num_outputs;
        for(i = 0; i < fd_gen_handle->mem_req.flexidag_num_outputs; i++) {
            if(handle->out_buf.buf[i].pBuffer == NULL) {
                ret = AmbaCV_UtilityCmaMemAlloc(CACHE_ALIGN(fd_gen_handle->mem_req.flexidag_output_buffer_size[i]), 1,&handle->out_buf.buf[i]);
            } else {
                //printf("RefCV_RunRaw : name = %s handle->out_buf.buf[i].pBuffer != NULL\n", handle->name);
            }

            if (ret != 0U) {
                printf("RefCV_RunRaw : name = %s AmbaCV_UtilityCmaMemAlloc out_buf %d fail \n", handle->name, i);
                ret = 1U;
                break;
            } else {
                AmbaCV_UtilityCmaMemInvalid(&handle->out_buf.buf[i]);
            }
        }

        if (ret == 0U) {
            ret = AmbaCV_FlexidagRun(fd_gen_handle, &handle->in_buf, &handle->out_buf, run_info);
            if (ret != 0U) {
                printf("RefCV_RunRaw : name = %s AmbaCV_FlexidagRun fail \n", handle->name);
                snprintf(&log_path[0], sizeof(log_path), "./flexidag_log%s.txt", handle->name);
                AmbaCV_FlexidagDumpLog(&handle->fd_gen_handle,log_path, FLEXILOG_CORE0 | FLEXILOG_VIS_CVTASK);
                ret = 1U;
            }
        }
    }

    return ret;
}

static void RefCV_RunCallback(void *vpHandle, uint32_t flexidag_output_num, flexidag_memblk_t *pblk_Output, void *vpParameter)
{
    static uint32_t ret = 0U;
    message_buf sbuf;
    REF_CV_PARAMS_s  *param = (REF_CV_PARAMS_s *)vpParameter;

    sbuf.mtype = 1;
    memcpy(&sbuf.mtext[0], &ret, MSGSZ);
    //printf("RefCV_RunCallback() called: %d\n", ret);
    ret++;
    msgsnd(param->msg_queue, &sbuf, MSGSZ, 0);
}

static uint32_t RefCV_RunNonBlockingRaw(REF_CV_HANDLE_s *handle, flexidag_cb cb, void *cb_param, memio_source_recv_multi_raw_t *in, uint32_t *token_id)
{
    uint32_t ret = 0U, i;
    AMBA_CV_FLEXIDAG_HANDLE_s *fd_gen_handle = &handle->fd_gen_handle;
    memio_source_recv_raw_t *pDataIn;
    char log_path[128];

    handle->in_buf.num_of_buf = in->num_io;
    for(i = 0 ; i < in->num_io ; i++) {
        if(handle->in_buf.buf[i].pBuffer == NULL) {
            ret = AmbaCV_UtilityCmaMemAlloc(CACHE_ALIGN(sizeof(memio_source_recv_raw_t)), 1,&handle->in_buf.buf[i]);
        } else {
            //printf("RefCV_RunRaw : name = %s handle->in_buf.buf[0].pBuffer != NULL\n", handle->name);
        }

        if (ret != 0U) {
            printf("RefCV_RunRaw : name = %s AmbaCV_UtilityCmaMemAlloc in_buf %d fail \n", handle->name,i);
            ret = 1U;
            break;
        } else {
            AmbaMisra_TypeCast32(&pDataIn, &handle->in_buf.buf[i].pBuffer);
            memcpy(pDataIn, &in->io[i], sizeof(memio_source_recv_raw_t));
            AmbaCV_UtilityCmaMemClean(&handle->in_buf.buf[i]);
        }
    }

    if(ret == 0U) {
        handle->out_buf.num_of_buf = fd_gen_handle->mem_req.flexidag_num_outputs;
        for(i = 0; i < fd_gen_handle->mem_req.flexidag_num_outputs; i++) {
            if(handle->out_buf.buf[i].pBuffer == NULL) {
                ret = AmbaCV_UtilityCmaMemAlloc(CACHE_ALIGN(fd_gen_handle->mem_req.flexidag_output_buffer_size[i]), 1,&handle->out_buf.buf[i]);
            } else {
                //printf("RefCV_RunRaw : name = %s handle->out_buf.buf[i].pBuffer != NULL\n", handle->name);
            }

            if (ret != 0U) {
                printf("RefCV_RunRaw : name = %s AmbaCV_UtilityCmaMemAlloc out_buf %d fail \n", handle->name, i);
                ret = 1U;
                break;
            } else {
                AmbaCV_UtilityCmaMemInvalid(&handle->out_buf.buf[i]);
            }
        }

        if (ret == 0U) {
            ret = AmbaCV_FlexidagRunNonBlock(fd_gen_handle, cb, cb_param, &handle->in_buf, &handle->out_buf, token_id);
            if (ret != 0U) {
                printf("RefCV_RunRaw : name = %s AmbaCV_FlexidagRun fail \n", handle->name);
                snprintf(&log_path[0], sizeof(log_path), "./flexidag_log%s.txt", handle->name);
                AmbaCV_FlexidagDumpLog(&handle->fd_gen_handle,log_path, FLEXILOG_CORE0 | FLEXILOG_VIS_CVTASK);
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
    if (ret != 0U) {
        printf("RefCV_Close : name = %s AmbaCV_FlexidagClose fail \n", handle->name);
        ret = 1U;
    }

    if(AmbaCV_UtilityCmaMemFree(&handle->bin_buf) != 0U ) {
        printf("RefCV_Close : name = %s AmbaCV_UtilityCmaMemFree bin_buf fail \n", handle->name);
        ret = 1U;
    }

    if(AmbaCV_UtilityCmaMemFree(&handle->init.state_buf) != 0U ) {
        printf("RefCV_Close : name = %s AmbaCV_UtilityCmaMemFree state_buf fail \n", handle->name);
        ret = 1U;
    }
    return ret;
}


UINT32 loop;

static void* RefCV_FileInput(void *arg)
{
    FILE *FpY;
    FILE *FpUV;
    UINT32 i,j;
    UINT32 Rval = 0,err_count = 0;
    UINT32 NumSuccess = 0U;
    void* pVoidDataY;
    void* pVoidDataUV;
    uint64_t y_len,uv_len;
    memio_source_recv_multi_raw_t raw;
    AMBA_CV_FLEXIDAG_RUN_INFO_s run_info;
    uint32_t token_id;
    REF_CV_PARAMS_s  *param = (REF_CV_PARAMS_s *)arg;
    char log_path[128];
    uint16_t flow_id;
    AMBA_CV_FLEXIDAG_MSG_s msg;
    const char *cvtable_name = BISENET_CVTABLE;
    const void *cvtable_data;
    uint32_t cvtable_size;
    AMBA_CV_FLEXIDAG_PERF_s flow_id_perf[4];

    printf("process (%d) RefCV_FileInput (%d) start \n",process_id,param->id);
    AmbaMisra_TypeCast32(&pVoidDataY, &param->file_bufY.pBuffer);
    AmbaMisra_TypeCast32(&pVoidDataUV, &param->file_bufUV.pBuffer);

    RefCV_Init(&param->handle);

    // prepare input data
    FpY = fopen(param->fileY_name, "rb");
    fseek(FpY, 0, SEEK_END);
    y_len = ftell(FpY);
    fseek(FpY, 0, SEEK_SET);
    printf("fileY len = %d	\n",(uint32_t)y_len);
    NumSuccess = fread(pVoidDataY, (uint32_t)y_len, 1U, FpY);
    if (NumSuccess <= 0U) {
        AmbaPrint_PrintUInt5("process (%d) thread (%d) fread Y failed", process_id, param->id, 0U, 0U, 0U);
    }
    fclose(FpY);
    if(param->file_bufY.buffer_cacheable != 0U) {
        AmbaCV_UtilityCmaMemClean(&param->file_bufY);
    }

    FpUV = fopen(param->fileUV_name, "rb");
    fseek(FpUV, 0, SEEK_END);
    uv_len = ftell(FpUV);
    fseek(FpUV, 0, SEEK_SET);
    printf("fileUV len = %d  \n",(uint32_t)uv_len);
    NumSuccess = fread(pVoidDataUV, uv_len, 1U, FpUV);
    if (NumSuccess <= 0U) {
        AmbaPrint_PrintUInt5("process (%d) thread (%d) fread UV failed", process_id, param->id, 0U, 0U, 0U);
    }
    fclose(FpUV);
    if(param->file_bufUV.buffer_cacheable != 0U) {
        AmbaCV_UtilityCmaMemClean(&param->file_bufUV);
    }

    raw.num_io = 2;
    raw.io[0].addr = param->file_bufY.buffer_caddr;
    raw.io[0].size = y_len;
    raw.io[0].pitch = 0U;

    raw.io[1].addr = param->file_bufUV.buffer_caddr;
    raw.io[1].size = uv_len;
    raw.io[1].pitch = 0U;

    Rval = AmbaCV_FlexidagFindCvtable(&param->handle.fd_gen_handle, cvtable_name, &cvtable_data, &cvtable_size);
    if(Rval == 0U) {
        printf("process (%d) thread (%d) cvtable_name(%s) = %s size = %d \n", process_id, param->id, cvtable_name, cvtable_data, cvtable_size, 0U, 0U);
    }
    (void) AmbaCV_FlexidagGetFlowIdByUUID(&param->handle.fd_gen_handle, BISENET_CVTASK_UUID, &flow_id);

    for (i = 0; i < param->num_runs; i++) {
        if(msg_send == 1) {
            printf("process (%d) thread (%d) flow_id = %d \n", process_id, param->id, flow_id);
            loop = i;
            msg.flow_id = flow_id;
            msg.vpMessage = &loop;
            msg.length = sizeof(loop);
            (void) AmbaCV_FlexidagSendMsg(&param->handle.fd_gen_handle,&msg);
        }
        if(non_blocking_run == 0) {
            Rval = RefCV_RunRaw(&param->handle, &raw, &run_info);
            if(Rval != 0) {
                break;
            }
        } else {
            int j;
            int loop;
            uint32_t token_id_array[128];
            message_buf rbuf;

            if (getNumberInArray(token_id_array) < non_blocking_run) {
                printf("Set non_blocking_run to %d\n", getNumberInArray(token_id_array));
                non_blocking_run = getNumberInArray(token_id_array);
            }

            loop = non_blocking_run;

            for (j=0; j<loop; j++) {
                Rval = RefCV_RunNonBlockingRaw(&param->handle, RefCV_RunCallback,(void *) param, &raw, &token_id_array[j]);
                if(Rval != 0U) {
                    printf("%d: RefCV_RunNonBlockingRaw() failed Rval= 0x%x\n", j, Rval);
                } else {
                    printf("RefCV_RunNonBlockingRaw() ok: token_id_array[%d]=%d\n", j,	token_id_array[j]);
                }
            }
            if(Rval != 0U) {
                break;
            } else {
                for (j=0; j<loop; j++) {
                    msgrcv( param->msg_queue, &rbuf, MSGSZ, 1, 0);
                    (void)AmbaCV_FlexidagWaitRunFinish(&param->handle.fd_gen_handle, token_id_array[j], &run_info);
                    printf("AmbaCV_FlexidagWaitRunFinish() called, token_id_array[%d]=%d, mtext=%d\n", j, token_id_array[j], rbuf.mtext[0]);
                }
            }
        }
        printf("process (%d) RefCV_FileInput (%d):	start time (%d), end time (%d), valid (%d) \n",process_id,param->id, run_info.start_time, run_info.end_time, run_info.output_not_generated);
        if(run_info.output_not_generated == 1U) {
            continue;
        }
        printf("process (%d) RefCV_FileInput (%d): RefCV_RunRaw run %d \n",process_id,param->id, i);

        for(j = 0; j < param->handle.fd_gen_handle.mem_req.flexidag_num_outputs; j++) {
            AmbaCV_UtilityCmaMemInvalid(&param->handle.out_buf.buf[j]);
            if(RefCV_PrintCrc(param->handle.id, &param->handle.out_buf.buf[j]) != 0U) {
                param->num_err += 1;
                break;
            }
        }
        memset(param->handle.out_buf.buf[0].pBuffer, 0x0, param->handle.out_buf.buf[0].buffer_size);
        AmbaCV_UtilityCmaMemClean(&param->handle.out_buf.buf[0]);
        param->cur_runs += 1;
        usleep(param->sleep_usec);
    }

    for(i = 0; i < param->handle.in_buf.num_of_buf; i++) {
        if(AmbaCV_UtilityCmaMemFree(&param->handle.in_buf.buf[i]) != 0U ) {
            printf("process (%d) RefCV_FileInput (%d) AmbaCV_UtilityCmaMemFree in_buf[%d] fail \n",process_id,param->id, i);
        }
    }

    for(i = 0; i < param->handle.out_buf.num_of_buf; i++) {
        if(AmbaCV_UtilityCmaMemFree(&param->handle.out_buf.buf[i]) != 0U ) {
            printf("process (%d) RefCV_FileInput (%d) AmbaCV_UtilityCmaMemFree out_buf[%d] fail \n",process_id,param->id, i);
        }
    }

    if(dump_perf == 0U) {
        snprintf(&log_path[0], sizeof(log_path), "./flexidag_log%d_%d.txt",process_id, param->id);
        AmbaCV_FlexidagDumpLog(&param->handle.fd_gen_handle,log_path, FLEXILOG_CORE0 | FLEXILOG_VIS_CVTASK);
    } else {
        snprintf(&log_path[0], sizeof(log_path), "./flexidag_perf%d_%d.prf",process_id, param->id);
        AmbaCV_FlexidagDumpLog(&param->handle.fd_gen_handle,log_path, FLEXILOG_VIS_PERF);
    }
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
    printf("\t-t [thread num] not more than 4\n");
    printf("\t-s [0 : schdr omit shutdown, 1 : schdr will shutdown]\n");
    printf("\t-b [0: blocking run: > 0 num of non-blocking run in one frame]\n");
    printf("\t-q [run frequency]\n");
    printf("\t-r [dump perf]\n");
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
            non_blocking_run = strtoul(optarg, NULL, 0);
            break;
        case 'm':
            msg_send = strtoul(optarg, NULL, 0);
            break;
        case 'r':
            dump_perf = strtoul(optarg, NULL, 0);
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

    while(cv_msg_loop == 1U) {
        retcode = AmbaCV_SchdrWaitMsg(&msg_num);
        if(retcode == ERRCODE_NONE) {
            for(i = 0U; i < msg_num; i++) {
                retcode = AmbaCV_SchdrProcessMsg(&phandle, &message_type, &message_retcode);
            }
        } else {
            printf("[ERROR] cv_msg_tsk() : AmbaCV_SchdrWaitMsg fail ret (0x%x)", retcode, 0U, 0U, 0U, 0U);
            break;
        }
    }

    return NULL;
}

static void RefCV_CleanUp(void)
{
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
    int retcode;
    uint32_t i,all_stop = 0;
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
        printf("[error] thread_num(%d) > MAX_THREAD_NUM \n",thread_id,MAX_THREAD_NUM);
        return 0;
    }
    printf("process_id = %d thread_num = %d flexidag path = %s data path = %s num_of_frame = %d non_blocking_run = %d msg_send = %d dump_perf = %d\n",process_id,thread_id,flexidag_path,data_path,num_of_frame,non_blocking_run,msg_send,dump_perf);
    if (external_msg_thread != 0) {
        printf("create schdr cmd thread \n");
        pthread_create(&cv_msg_thread, NULL, cv_msg_tsk, NULL);
        //pthread_setschedprio(cv_msg_thread, 90U);
    }

    /* init scheduler */
    cfg.cpu_map = 0xD;
    cfg.log_level = LVL_DEBUG;
    AmbaCV_FlexidagSchdrStart(&cfg);

    for(i = 0; i < thread_id; i ++) {
        memset(&thread_params[i].handle, 0x0, sizeof(REF_CV_HANDLE_s));

        thread_params[i].id	= i;
        thread_params[i].num_runs	= num_of_frame;
        thread_params[i].sleep_usec = 20*1000*i;
        thread_params[i].RunFlag	= 0;
        snprintf(&thread_params[i].fileY_name[0], sizeof(thread_params[i].fileY_name), "%s/%d_y.bin",data_path, i%MAX_GOLDEN_NUM);
        snprintf(&thread_params[i].fileUV_name[0], sizeof(thread_params[i].fileUV_name), "%s/%d_uv.bin",data_path, i%MAX_GOLDEN_NUM);
        snprintf(&thread_params[i].flexidag_path[0], sizeof(thread_params[i].flexidag_path), "%s/flexibin0.bin",flexidag_path);

        AmbaCV_UtilityCmaMemAlloc(MAX_FILE_Y_SIZE, 1, &thread_params[i].file_bufY);
        AmbaCV_UtilityCmaMemAlloc(MAX_FILE_UV_SIZE, 1, &thread_params[i].file_bufUV);
        snprintf(&thread_params[i].handle.name[0], sizeof(thread_params[i].handle.name), "00%02d-00%02d",process_id, i);
        thread_params[i].handle.id = i;
        if(non_blocking_run > 0) {
            thread_params[i].msg_queue = msgget((key_t)(i | BISENET_KEY), 0666 | IPC_CREAT);
            printf("msgget create: %d\n", thread_params[i].msg_queue);
        }
        printf("process (%d) thread_params[%d].num_runs 		   = %d \n",process_id,i,thread_params[i].num_runs);
        printf("process (%d) thread_params[%d].sleep_usec		   = %d \n",process_id,i,thread_params[i].sleep_usec);
        printf("process (%d) thread_params[%d].fileY_name		   = %s \n",process_id,i,thread_params[i].fileY_name);
        printf("process (%d) thread_params[%d].fileUV_name		   = %s \n",process_id,i,thread_params[i].fileUV_name);
        printf("process (%d) thread_params[%d].flexidag_path	   = %s \n",process_id,i,thread_params[i].flexidag_path);
        printf("process (%d) thread_params[%d].fileY va 		   = %p \n",process_id,i,thread_params[i].file_bufY.pBuffer);
        printf("process (%d) thread_params[%d].fileY pa 		   = %08x \n",process_id,i,thread_params[i].file_bufY.buffer_daddr);
        printf("process (%d) thread_params[%d].fileUV va		   = %p \n",process_id,i,thread_params[i].file_bufUV.pBuffer);
        printf("process (%d) thread_params[%d].fileUV pa		   = %08x \n",process_id,i,thread_params[i].file_bufUV.buffer_daddr);
        set.flexidag_msg_entry = 1U;		//no requirement to send msg from flexidag to cvtask
        set.cvtask_msg_entry = 1U;		//no requirement to send internal cvtask msg
        set.arm_cpu_map = 0xFU;
        set.arm_log_entry = 0U;
        set.orc_log_entry = 512U;
        AmbaCV_FlexidagSetParamSet(&thread_params[i].handle.fd_gen_handle, FLEXIDAG_PARAMSET_LOG_MSG, (void *)&set, sizeof(AMBA_CV_FLEXIDAG_LOG_MSG_s));
        RefCV_Open(thread_params[i].flexidag_path, &thread_params[i].handle);
    }

    // alloc global temp buffer
    if(thread_params[0].handle.fd_gen_handle.mem_req.flexidag_temp_buffer_size != 0U) {
        AmbaCV_UtilityCmaMemAlloc(CACHE_ALIGN(thread_params[0].handle.fd_gen_handle.mem_req.flexidag_temp_buffer_size), 1, &g_temp_buf);
    }

    for(i = 0; i < thread_id; i ++) {
        thread_params[i].RunFlag = 1U;
        retcode = pthread_create(&thread_params[i].pthread, NULL, RefCV_FileInput, &thread_params[i]);
        if(retcode != 0) {
            printf("main process (%d) thread (%d) pthread_create fail retcode (%d)\n", process_id, thread_params[i].id, retcode);
            thread_params[i].RunFlag = 0U;
        }
    }
    usleep(200);
    do {
        uint32_t still_run = 0U;
        for(i = 0; i < thread_id; i ++) {
            if( thread_params[i].RunFlag != 0U ) {
                still_run = 1;
                break;
            }
        }
        if(still_run == 0U) {
            all_stop = 1;
        }
        usleep(250*1000); /* Sleep 250ms */
    } while (all_stop == 0U);

    if(g_temp_buf.pBuffer != NULL) {
        if(AmbaCV_UtilityCmaMemFree(&g_temp_buf) != 0U ) {
            printf("main : AmbaCV_UtilityCmaMemFree g_temp_buf fail \n");
        }
    }

    for(i = 0; i < thread_id; i ++) {
        AmbaCV_UtilityCmaMemFree(&thread_params[i].file_bufY);
        AmbaCV_UtilityCmaMemFree(&thread_params[i].file_bufUV);
    }


    for(i = 0; i < thread_id; i ++) {
        printf("main process (%d) thread (%d) total (%d) error (%d) \n",process_id,thread_params[i].id, thread_params[i].cur_runs, thread_params[i].num_err);
    }

    if (schdr_close == 1) {
        printf("main process (%d) schdr_shutdown \n",process_id);
        schdr_shutdown(0);
    }
    printf("main process (%d) finish \n",process_id);

    if (external_msg_thread != 0) {
        cv_msg_loop = 0;
    }

    return 0;
}
