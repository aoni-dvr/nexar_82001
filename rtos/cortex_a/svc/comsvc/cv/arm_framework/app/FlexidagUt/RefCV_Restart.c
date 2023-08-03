/**
 *  @file RefCV_Restart.c
 *
 *  Copyright (c) 2020 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  This file includes sample code and is only for internal testing and evaluation.  If you
 *  distribute this sample code (whether in source, object, or binary code form), it will be
 *  without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Implementation of Bisenet Restart Test
 *
 */

#include "AmbaPrint.h"
#include "AmbaMisraFix.h"
#include "AmbaWrap.h"
#include "cvapi_ambacv_flexidag.h"
#include "cvapi_svccvalgo_memio_interface.h"
#include "RefCV_Common.h"
#include "cvapi_flexidag_unittest.h"

static flexidag_memblk_t file_regionY;
static flexidag_memblk_t file_regionUV;
static flexidag_memblk_t bin_buf;
static AMBA_CV_FLEXIDAG_INIT_s init;
static AMBA_CV_FLEXIDAG_IO_s in_buf;
static AMBA_CV_FLEXIDAG_IO_s out_buf;
static uint32_t SchdrRestartFlag = 0U;
static uint32_t FlexidagRestartFlag = 0U;

static uint32_t check_crc32(const uint8_t *pBuffer, uint32_t Size)
{
    const uint8_t *p;
    uint32_t Crc;
    uint32_t idx;
    static const uint32_t Crc32Table[] = {
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

    p = pBuffer;
    Crc = ~0U;

    while (Size > 0U) {
        idx = (Crc ^ (uint32_t)*p) & 0x0FFU;
        Crc = Crc32Table[idx] ^ (Crc >> 8U);
        p++;
        Size--;
    }

    return Crc ^ ~0U;
}

static void Do_Show_Output(uint32_t loop, AMBA_CV_FLEXIDAG_IO_s *pOut_buf)
{
    UINT32 Crc;
    UINT32 ret = 0U;
    UINT32 Golden;

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    Golden = 0xf3647648U;
#elif defined(CONFIG_SOC_CV28)
    Golden = 0x8707bb19U;
#elif defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    Golden = 0xf3647648U;
#else
    Golden = 0xbd29bf68U;
#endif

    ret = AmbaCV_UtilityCmaMemInvalid(&pOut_buf->buf[0]);
    (void) ret;
    Crc = check_crc32((const uint8_t *)pOut_buf->buf[0].pBuffer, pOut_buf->buf[0].buffer_size);
    if (Crc != Golden) {
        AmbaPrint_PrintUInt5("%d fail Crc32 = 0x%x\n", loop, Crc, 0U, 0U, 0U);
    } else {
        AmbaPrint_PrintUInt5("%d ok Crc32 = 0x%x\n", loop, Crc, 0U, 0U, 0U);
    }
    ret = AmbaWrap_memset(pOut_buf->buf[0].pBuffer, 0x0, pOut_buf->buf[0].buffer_size);
    (void) ret;
    ret = AmbaCV_UtilityCmaMemClean(&pOut_buf->buf[0]);
    (void) ret;
}

static UINT32 S_LoadFile(const char* pFileName, flexidag_memblk_t* pMemBlk)
{
    UINT32 ret = 0U;
    UINT32 size_align = 0U;

    ret = AmbaCV_UtilityFileSize(pFileName, &size_align);

    if(ret == 0U) {
        if(pMemBlk->pBuffer == NULL) {
            ret = RefCV_MemblkAlloc(size_align, pMemBlk);
        }
    } else {
        AmbaPrint_PrintStr5("S_LoadFile (%s) AmbaCV_UtilityFileSize fail ",pFileName, NULL, NULL, NULL, NULL);
    }

    if(ret == 0U) {
        ret = AmbaCV_UtilityFileLoad(pFileName, pMemBlk);
    } else {
        AmbaPrint_PrintStr5("S_LoadFile (%s) RefCV_MemblkAlloc fail ",pFileName, NULL, NULL, NULL, NULL);
    }

    if(ret != 0U) {
        AmbaPrint_PrintStr5("S_LoadFile (%d) AmbaCV_UtilityFileLoad fail ",pFileName, NULL, NULL, NULL, NULL);
    }

    return ret;
}

extern UINT32 AmbaShell_CommandSchdrStart(void);
static UINT32 restart_loop = 1U;
static void* Do_SchdrRestart(void *args)
{
    uint32_t schdr_drv_state;
    UINT32 Rval = 0U;

    (void) args;
    AmbaMisra_TouchUnused(args);
    while(restart_loop == 1U) {
        if (SchdrRestartFlag == 1U) {
            Rval = AmbaCV_SchdrDrvState(&schdr_drv_state);
            (void) Rval;
            while(schdr_drv_state != FLEXIDAG_SCHDR_DRV_OFF) {
                Rval = AmbaKAL_TaskSleep(100U);
                (void) Rval;
                AmbaPrint_PrintUInt5("Do_SchdrRestart: sleep drv_state %d", schdr_drv_state, 0U, 0U, 0U, 0U);
                Rval = AmbaCV_SchdrDrvState(&schdr_drv_state);
                (void) Rval;
            }
            Rval = AmbaKAL_TaskSleep(100U); // Add sleep here then could work well
            (void) Rval;
            AmbaPrint_PrintUInt5("Do_SchdrRestart: Restarting", 0U, 0U, 0U, 0U, 0U);
            Rval = AmbaShell_CommandSchdrStart();
            if(Rval == 0U) {
                SchdrRestartFlag = 0U;
                AmbaPrint_PrintUInt5("Do_SchdrRestart: Restart done", 0U, 0U, 0U, 0U, 0U);
            } else {
                AmbaPrint_PrintUInt5("Do_SchdrRestart: Restart fail", 0U, 0U, 0U, 0U, 0U);
                break;
            }
        }
        Rval = AmbaKAL_TaskSleep(250U);
        (void) Rval;
    }
    return NULL;
}

static void Do_LoadFile(void)
{
    UINT32 ret = 0U;
    const char flexidag_path[128]= "c:\\flexidag_bisenet_mnv2_raw/flexibin/flexibin0.bin";
    const char y_path[128] = "c:\\flexidag_bisenet_mnv2_raw/golden/0_y.bin";
    const char uv_path[128] = "c:\\flexidag_bisenet_mnv2_raw/golden/0_uv.bin";

    // load input Y file
    ret = S_LoadFile(y_path, &file_regionY);
    if (ret != 0U) {
        AmbaPrint_PrintUInt5("S_LoadFile input Y fail ", 0U, 0U, 0U, 0U, 0U);
    }

    // load input UV file
    if (ret == 0U) {
        ret = S_LoadFile(uv_path, &file_regionUV);
        if(ret != 0U) {
            AmbaPrint_PrintUInt5("S_LoadFile input UV fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }

    // load bin file
    if (ret == 0U) {
        ret = S_LoadFile(flexidag_path, &bin_buf);
        if(ret != 0U) {
            AmbaPrint_PrintUInt5("S_LoadFile bin fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
}

static void Do_AllocBuf(const AMBA_CV_FLEXIDAG_HANDLE_s *handle)
{
    memio_source_recv_multi_raw_t raw;
    memio_source_recv_raw_t *pDataIn;
    uint32_t i;
    UINT32 Rval = 0U;

    //state buffer
    Rval = AmbaCV_UtilityCmaMemAlloc(handle->mem_req.flexidag_state_buffer_size, 1U, &init.state_buf);
    (void) Rval;

    raw.num_io = 2;
    raw.io[0].addr = file_regionY.buffer_caddr;
    raw.io[0].size = file_regionY.buffer_size;
    raw.io[0].pitch = 0U;

    raw.io[1].addr = file_regionUV.buffer_caddr;
    raw.io[1].size = file_regionUV.buffer_size;
    raw.io[1].pitch = 0U;

    //input_buf
    in_buf.num_of_buf = raw.num_io;
    for(i = 0; i < in_buf.num_of_buf; i++) {
        Rval = AmbaCV_UtilityCmaMemAlloc(sizeof(memio_source_recv_raw_t), 1, &in_buf.buf[i]);
        (void) Rval;
        AmbaMisra_TypeCast(&pDataIn, &in_buf.buf[i].pBuffer);
        Rval = AmbaWrap_memcpy(pDataIn, &raw.io[i], sizeof(memio_source_recv_raw_t));
        (void) Rval;
        Rval = AmbaCV_UtilityCmaMemClean(&in_buf.buf[i]);
        (void) Rval;
    }

    //output buffer
    out_buf.num_of_buf = handle->mem_req.flexidag_num_outputs;
    for(i = 0; i < out_buf.num_of_buf; i++) {
        Rval = AmbaCV_UtilityCmaMemAlloc(handle->mem_req.flexidag_output_buffer_size[i], 1, &out_buf.buf[i]);
        (void) Rval;
    }
}

static void RefCVErrorCb(void *vpHandle, flexidag_error_struct_t *pErrorStruct, void *vpParameter)
{
    (void) vpHandle;
    (void) pErrorStruct;
    (void) vpParameter;
    AmbaMisra_TouchUnused(vpHandle);
    AmbaMisra_TouchUnused(pErrorStruct);
    AmbaMisra_TouchUnused(vpParameter);
    AmbaPrint_PrintUInt5("RefCVErrorCb : retcode (0x%x) > 0x%x", pErrorStruct->retcode, ERR_SYSFLOW_FATAL_BASE, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("RefCVErrorCb : frame (%d) sys_flow_id (%d)", pErrorStruct->cvtask_frameset_id, pErrorStruct->sysflow_index, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5(pErrorStruct->pFormatString, pErrorStruct->arg0, pErrorStruct->arg1, pErrorStruct->arg2, pErrorStruct->arg3, pErrorStruct->arg4);
    if (pErrorStruct->retcode > ERR_SYSFLOW_FATAL_BASE) {
        SchdrRestartFlag = 1U;
        FlexidagRestartFlag = 1U;
    }
}

static void Do_FlexidagRun(AMBA_CV_FLEXIDAG_HANDLE_s *handle)
{
    static uint32_t AllocBuf_Init = 0U;
    AMBA_CV_FLEXIDAG_RUN_INFO_s run_info = {0};
    void *cb_param = NULL;
    uint32_t i = 0U;
    UINT32 Rval = 0U;

    Rval = AmbaCV_FlexidagOpen(&bin_buf, handle);
    (void) Rval;
    if (AllocBuf_Init == 0U) {
        Do_AllocBuf(handle);
        AllocBuf_Init = 1U;
    }
    Rval = AmbaCV_FlexidagInit(handle, &init);
    (void) Rval;
    Rval = AmbaCV_FlexidagSetErrorCb(handle, RefCVErrorCb, cb_param);
    (void) Rval;
    while(i < 100U) { /* loop until orc fatal error happen */
        Rval = AmbaCV_FlexidagRun(handle, &in_buf, &out_buf, &run_info);
        (void) Rval;
        if (i >= 10U) { /* simulate fatal error happen */
            Rval = AmbaCV_SchdrShutdown(0);
            (void) Rval;
            SchdrRestartFlag = 1U;
            FlexidagRestartFlag = 1U;
        }
        Do_Show_Output(i, &out_buf);
        i++;
        if (FlexidagRestartFlag != 0U) {
            break;
        }
    }
}

static AMBA_KAL_TASK_t restart_task;
static UINT32 RefCV_Restart_Init(void)
{
    UINT32 Rval = 0U;
    static UINT8 refcv_restart_stack[0x4000] GNU_SECTION_NOZEROINIT;
    char task_name[32] =  "refcv_restart";

    restart_loop = 1U;
    /* Create restart thread */
    Rval  |= AmbaKAL_TaskCreate(&restart_task,                                       /* pTask */
                                task_name,                                                   /* pTaskName */
                                100,     /* Priority */
                                Do_SchdrRestart,                                             /* void (*EntryFunction)(UINT32) */
                                NULL,                                                   /* EntryArg */
                                &refcv_restart_stack[0],                                /* pStackBase */
                                sizeof(refcv_restart_stack),                                /* StackByteSize */
                                0U);
    if(Rval == 0U) {
#if !defined(CONFIG_QNX) && !defined(CONFIG_LINUX)
        Rval  |= AmbaKAL_TaskSetSmpAffinity(&restart_task, 0x1U);
#endif
        Rval  |= AmbaKAL_TaskResume(&restart_task);
    } else {
        AmbaPrint_PrintUInt5("RefCV_Restart AmbaKAL_TaskCreate fail (%d)", Rval, 0U, 0U, 0U, 0U);
    }

    return Rval;
}

static void RefCV_Restart_DeInit(void)
{
    UINT32 Rval = 0U,ret1 = 0U;
#if !defined(CONFIG_QNX) && !defined(CONFIG_LINUX)
    AMBA_KAL_TASK_INFO_s taskInfo;
#endif

    restart_loop = 0U;
    /* Delete restart thread */
#if !defined(CONFIG_QNX) && !defined(CONFIG_LINUX)
    ret1  = AmbaKAL_TaskQuery(&restart_task, &taskInfo);
    if ((taskInfo.TaskState != TX_COMPLETED) && (taskInfo.TaskState != TX_TERMINATED)) {
        Rval  |= AmbaKAL_TaskTerminate(&restart_task);
        if ( Rval  != KAL_ERR_NONE ) {
            AmbaPrint_PrintUInt5("RefCV_Restart AmbaKAL_TaskTerminate fail (%d)", Rval, 0U, 0U, 0U, 0U);
        }
    }
#endif
    Rval  |= AmbaKAL_TaskDelete(&restart_task);
    if ( Rval != KAL_ERR_NONE ) {
        AmbaPrint_PrintUInt5("RefCV_Restart AmbaKAL_TaskDelete fail (%d)", Rval, 0U, 0U, 0U, 0U);
    }
    (void) ret1;
}

void RefCV_Restart(UINT32 loop)
{
    UINT32 i;
    AMBA_CV_FLEXIDAG_HANDLE_s handle;
    UINT32 Rval = 0U;

    Rval = AmbaWrap_memset(&handle, 0x0, sizeof(AMBA_CV_FLEXIDAG_HANDLE_s));
    (void) Rval;
    if(RefCV_Restart_Init() == 0U) {
        /* Load orcvp and input binary */
        Do_LoadFile();

        /* Restart test loop */
        for(i = 0U; i < loop; i++) {
            Do_FlexidagRun(&handle);

            if (FlexidagRestartFlag == 1U) {
                uint32_t schdr_drv_state;

                Rval = AmbaKAL_TaskSleep(100U);
                /* wait restart success */
                Rval = AmbaCV_SchdrDrvState(&schdr_drv_state);
                (void) Rval;
                while(schdr_drv_state == FLEXIDAG_SCHDR_DRV_OFF) {
                    Rval = AmbaKAL_TaskSleep(100U);
                    (void) Rval;
                    AmbaPrint_PrintUInt5("RefCV_Restart: restart(%d) sleep drv_state(%d)", i, schdr_drv_state, 0U, 0U, 0U);
                    Rval = AmbaCV_SchdrDrvState(&schdr_drv_state);
                    (void) Rval;
                }
                AmbaPrint_PrintUInt5("RefCV_Restart: restart(%d) success", i, 0U, 0U, 0U, 0U);
                handle.fd_handle = NULL;
                FlexidagRestartFlag = 0U;
            }
        }
        RefCV_Restart_DeInit();
    }
}
