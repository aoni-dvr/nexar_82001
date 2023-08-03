/**
 *  @file RefCV_Test.c
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
 *  @details Implementation of SVC COMSVC Test
 *
 */

#include "AmbaIntrinsics.h"
#include "AmbaKAL.h"
#include "AmbaPrint.h"
#include "AmbaUtility.h"
#include "AmbaMisraFix.h"
#include "AmbaMMU.h"
#include "AmbaFS.h"
#include "AmbaWrap.h"
#include "AmbaCache.h"
#include "cvapi_memio_interface.h"
#include "cvapi_ambacv_flexidag.h"
#include "RefCV_Common.h"
#include "cvapi_flexidag_unittest.h"

#define MAX_THREAD_NUM          1U
#define MAX_GOLDEN_NUM          4U

static uint32_t refcv_net_loop;
static REF_CV_PARAMS_s thread_params[MAX_THREAD_NUM];

static void RefCVTestRunCallback(
    void *vpHandle,
    uint32_t flexidag_output_num,
    flexidag_memblk_t *pblk_Output,
    void *vpParameter)
{
    uint32_t ret = 0U;
    REF_CV_PARAMS_s  *param;

    (void) vpHandle;
    (void) flexidag_output_num;
    (void) pblk_Output;
    (void) vpParameter;
    AmbaMisra_TouchUnused(vpHandle);
    AmbaMisra_TouchUnused(pblk_Output);
    AmbaMisra_TouchUnused(vpParameter);
    AmbaMisra_TypeCast(&param, &vpParameter);
    ret = AmbaKAL_MsgQueueSend(&param->msg_queue, &ret, AMBA_KAL_NO_WAIT);
    (void) ret;
}

static void PicInfoInit(
    const REF_CV_PARAMS_s *param,
    cv_pic_info_t *pic_info,
    uint32_t YuvWidth,
    uint32_t YuvHeight)
{
    pic_info->capture_time = 0U;
    pic_info->channel_id = 0U;
    pic_info->frame_num = 0U;
    pic_info->pyramid.image_width_m1 = (UINT16)(YuvWidth - 1U);
    pic_info->pyramid.image_height_m1 = (UINT16)(YuvHeight - 1U);
    pic_info->pyramid.image_pitch_m1 = (YuvWidth - 1U);
    pic_info->pyramid.half_octave[0].ctrl.roi_pitch = (UINT16)YuvWidth;
    pic_info->pyramid.half_octave[0].roi_start_col = (INT16)0;
    pic_info->pyramid.half_octave[0].roi_start_row = (INT16)0;
    pic_info->pyramid.half_octave[0].roi_width_m1 = (UINT16)(YuvWidth - 1U);
    pic_info->pyramid.half_octave[0].roi_height_m1 = (UINT16)(YuvHeight - 1U);

    pic_info->pyramid.half_octave[1].ctrl.disable = 1U;
    pic_info->pyramid.half_octave[2].ctrl.disable = 1U;
    pic_info->pyramid.half_octave[3].ctrl.disable = 1U;
    pic_info->pyramid.half_octave[4].ctrl.disable = 1U;
    pic_info->pyramid.half_octave[5].ctrl.disable = 1U;

    pic_info->rpLumaLeft[0] = param->file_regionY.buffer_daddr;
    pic_info->rpChromaLeft[0] = (param->file_regionY.buffer_daddr + (YuvWidth * YuvHeight));
    pic_info->rpLumaRight[0] = pic_info->rpLumaLeft[0];
    pic_info->rpChromaRight[0] = pic_info->rpChromaLeft[0];
}

static uint32_t WriteFile(
    REF_CV_PARAMS_s *param,
    const char *strbuf,
    char *pbuf,
    uint32_t size)
{
    uint32_t ret;
    uint32_t count;
    AMBA_FS_FILE *ofp;

    ret = AmbaFS_FileOpen(strbuf, "wb", &ofp);
    if (ret != 0U) {
        AmbaPrint_PrintUInt5("AmbaFS_FileOpen fail", 0U, 0U, 0U, 0U, 0U);
        param->num_err = param->num_err + 1U;
    }

    ret = AmbaFS_FileWrite(pbuf, 1, size, ofp, &count);
    if (ret != 0U) {
        AmbaPrint_PrintUInt5("AmbaFS_FileWrite fail", 0U, 0U, 0U, 0U, 0U);
        param->num_err = param->num_err + 1U;
    }

    ret = AmbaFS_FileClose(ofp);
    if (ret != 0U) {
        AmbaPrint_PrintUInt5("AmbaFS_FileClose fail", 0U, 0U, 0U, 0U, 0U);
        param->num_err = param->num_err + 1U;
    }

    return ret;
}

static void* RefCVFileInPicinfo(void* arg)
{
    UINT32 i, j, ret;
    UINT32 YuvWidth = 1920U;
    UINT32 YuvHeight = 1080U;
    memio_source_recv_picinfo_t memio_pic_info = {0};
    cv_pic_info_t *pic_info = &memio_pic_info.pic_info;
    AMBA_CV_FLEXIDAG_RUN_INFO_s run_info = {0U};
    REF_CV_PARAMS_s *param;
    REF_CV_HANDLE_s *phdl;
    char log_path[128];
    UINT32 token_id;
    void *cb_param;
    char strbuf[128];
    UINT32 ret1 = 0U,ret2 = 0U;

    (void) arg;
    AmbaMisra_TypeCast(&param, &arg);
    param->RunFlag  = 1U;

    phdl = &param->handle;

    AmbaPrint_PrintUInt5("RefCVFileInPicinfo (%d) start ",param->id, 0U, 0U, 0U, 0U);

    ret = AmbaCV_FlexidagInit(&phdl->fd_gen_handle, &phdl->init);
    if(ret != 0U) {
        AmbaPrint_PrintUInt5("RefCVFileInPicinfo (%d) AmbaCV_FlexidagInit fail ",param->id, 0U, 0U, 0U, 0U);
    } else {
        PicInfoInit(param, pic_info, YuvWidth, YuvHeight);

        for (i = 0U; i < param->num_runs; i++) {
            AmbaPrint_PrintUInt5("RefCVFileInPicinfo (%d): RefCVRunPicinfo run %d ",param->id, i, 0U, 0U, 0U);

            if(param->blocking_run == 1U) {
                ret = RefCVRunPicinfo(&param->handle, pic_info, &run_info);
            } else {
                AmbaMisra_TypeCast(&cb_param, &param);
                ret = RefCVRunNonBlockingPicinfo(&param->handle, RefCVTestRunCallback, cb_param, pic_info, &token_id);
                if (ret == 0U) {
                    ret1 |= AmbaKAL_MsgQueueReceive(&param->msg_queue, &ret, AMBA_KAL_WAIT_FOREVER);
                    ret1 |= AmbaCV_FlexidagWaitRunFinish(&phdl->fd_gen_handle, token_id, &run_info);
                }
            }

            if (ret != 0U) {
                AmbaPrint_PrintUInt5("RefCVFileInPicinfo (%d) RefCVRunPicinfo %d fail ",param->id, i, 0U, 0U, 0U);
                break;
            }

            for (j = 0; j < phdl->out_buf.num_of_buf; j++) {
                ret1 |= RefCV_MemblkInvalid(&phdl->out_buf.buf[j]);

                ret2 |= RefCVSprintfUint2(&strbuf[0], sizeof(strbuf), ".\\%d_output_%d.out", i % MAX_GOLDEN_NUM, j);
                AmbaPrint_PrintStr5("  output file     = %s", strbuf, NULL, NULL, NULL, NULL);

                ret1 |= WriteFile(param, strbuf, phdl->out_buf.buf[j].pBuffer, phdl->out_buf.buf[j].buffer_size);

                /* clean up output buffer! */
                ret1 |= AmbaWrap_memset(param->handle.out_buf.buf[j].pBuffer, 0x0, param->handle.out_buf.buf[j].buffer_size);
                ret1 |= RefCV_MemblkClean(&param->handle.out_buf.buf[j]);
            }

            param->cur_runs = param->cur_runs + 1U;
            ret1 |= AmbaKAL_TaskSleep(param->sleep_usec);
        }

        ret2 |= RefCVSprintfUint1(&log_path[0], sizeof(log_path), ".\\flexidag_log%d.txt", param->id);
        ret = AmbaCV_FlexidagDumpLog(&param->handle.fd_gen_handle,log_path, FLEXILOG_VIS_CVTASK | FLEXILOG_CORE0);
        if (ret != 0U) {
            AmbaPrint_PrintUInt5("RefCVFileInPicinfo (%d) AmbaCV_FlexidagDumpLog fail ",param->id, 0U, 0U, 0U, 0U);
        }

        ret = AmbaCV_FlexidagClose(&param->handle.fd_gen_handle);
        if (ret != 0U) {
            AmbaPrint_PrintUInt5("RefCVFileInPicinfo (%d) AmbaCV_FlexidagClose fail ",param->id, 0U, 0U, 0U, 0U);
        }
    }

    param->RunFlag = 0U;
    while(refcv_net_loop == 1U) {
        ret1 |= AmbaKAL_TaskSleep(1000U);
    }
    (void) ret1;
    (void) ret2;
    return NULL;
}

#if 0
#define MAX_TABLE_PATH_LENGTH           (256)
#define MAX_FD_PATH_TABLE_AMOUNT        (5)

static UINT32 fd_idx = 0U;
static char fd_path[MAX_FD_PATH_TABLE_AMOUNT][MAX_TABLE_PATH_LENGTH];

static void GetFileName(const char *path)
{
    UINT32 RetVal;
    AMBA_FS_DTA_t FileSearch;
    char *load_path;
    const char *pArgs[2];
    const char * pChar;
    const INT8 *pI8;

    for (RetVal = AmbaFS_FileSearchFirst(path, ATTR_ALL, &FileSearch);
         RetVal == 0U;
         RetVal = AmbaFS_FileSearchNext(&FileSearch)) {

        load_path = fd_path[fd_idx];

        pArgs[0] = ".";
        if (FileSearch.LongName[0] != (INT8)'\0') {
            pI8 = &FileSearch.LongName[0];
        } else {
            pI8 = &FileSearch.FileName[0];
        }
        AmbaMisra_TypeCast(&pChar, &pI8);
        pArgs[1] = pChar;

        (void)AmbaUtility_StringPrintStr(load_path,
                                         MAX_TABLE_PATH_LENGTH,
                                         "%s\\%s",
                                         2,
                                         pArgs);

        AmbaPrint_PrintStr5("FlexiDagBin Found (%s)",
                            load_path, NULL, NULL, NULL, NULL);

        fd_idx++;
        if(fd_idx >= (UINT32) MAX_FD_PATH_TABLE_AMOUNT) {
            AmbaPrint_PrintStr5("[Warning] Reach amount limit!!",
                                NULL, NULL, NULL, NULL, NULL);
            fd_idx = 0;
        }
    }
}
#endif

static UINT32 RefCVTestParaInit(UINT32 thread_id, UINT32 num_of_frame, UINT32 blocking_run)
{
    static UINT32 cvtest_run = 0U;
    UINT32 i, j, ret = 0U;
    UINT32 pos = 0U,tmp_pos;
    char name[32] = "Test";
    const char path[]=".\\flexibin\\flexibin0.bin";
    const char filein[] = ".\\golden\\";
    uint32_t size;
    REF_CV_PARAMS_s *ptp;
    REF_CV_HANDLE_s *phdl;
    const flexidag_memory_requirements_t *p_mem_req;
    UINT32 ret1 = 0U;

    if(cvtest_run == 1U) {
        AmbaPrint_PrintUInt5("[ERROR] RefCVTestParaInit : Can only run one time, please reset power and test again ",
                             thread_id,MAX_THREAD_NUM, 0U, 0U, 0U);
        ret = 1U;
    } else if(thread_id > MAX_THREAD_NUM) {
        AmbaPrint_PrintUInt5("[ERROR] thread_num(%d) > MAX_THREAD_NUM",
                             thread_id,MAX_THREAD_NUM, 0U, 0U, 0U);
        ret = 1U;
    } else {
        cvtest_run = 1U;
        for(i = 0U; i < thread_id; i ++) {
            ptp     = &thread_params[i];
            phdl    = &ptp->handle;

            ret1 |= AmbaWrap_memset(ptp, 0x0, sizeof(REF_CV_PARAMS_s));

            ptp->id         = i;
            ptp->num_runs   = num_of_frame;
            ptp->sleep_usec = (UINT32)(20U * i);
            ptp->RunFlag    = 0U;
            AmbaUtility_StringCopy(&ptp->flexidag_path[0], sizeof(ptp->flexidag_path), path);

            pos = 0U;
            tmp_pos = RefCVSprintfStr(&ptp->file_nameY[pos], filein);
            pos = pos + tmp_pos;
            tmp_pos = RefCVSprintfUint1(&ptp->file_nameY[pos], sizeof(ptp->file_nameY), "%d.yuv", i % MAX_GOLDEN_NUM);
            pos = pos + tmp_pos;
            (void) pos;

            ret1 |= RefCVSprintfUint1(&phdl->name[0], sizeof(phdl->name), "00%02d", i);
            phdl->id = i;

            //input raw buffer
            ret1 |= AmbaCV_UtilityFileSize(ptp->file_nameY, &size);
            ret1 |= RefCV_MemblkAlloc(size, &ptp->file_regionY);

            //bin buffer
            ret1 |= AmbaCV_UtilityFileSize(path, &size);
            ret1 |= RefCV_MemblkAlloc(size, &phdl->bin_buf);

            // Load FD bin to buffer
            ret = AmbaCV_UtilityFileLoad(
                      ptp->flexidag_path,
                      &phdl->bin_buf);
            if (ret != 0U) {
                AmbaPrint_PrintStr5("name = %s AmbaCV_UtilityFileLoad fail path = %s", phdl->name, ptp->flexidag_path, NULL, NULL, NULL);
            }

            // Open Fd to get the size of required memory
            ret = AmbaCV_FlexidagOpen(
                      &phdl->bin_buf,
                      &phdl->fd_gen_handle);
            if(ret != 0U) {
                AmbaPrint_PrintUInt5("AmbaCV_FlexidagOpen fail", 0U, 0U, 0U, 0U, 0U);
            }

            p_mem_req = &phdl->fd_gen_handle.mem_req;

            //state buffer
            ret1 |= RefCV_MemblkAlloc(p_mem_req->flexidag_state_buffer_size, &phdl->init.state_buf);

            //temp buffer
            ret1 |= RefCV_MemblkAlloc(p_mem_req->flexidag_temp_buffer_size, &phdl->init.temp_buf);

            //in_buf buffer
            /* 3840 * 2160 * 1.5 */
            size = 12441600;
            phdl->in_buf.num_of_buf = 1U;
            ret1 |= RefCV_MemblkAlloc(size, &phdl->in_buf.buf[0]);

            //out_buf buffer
            phdl->out_buf.num_of_buf = p_mem_req->flexidag_num_outputs;
            for (j = 0; j < phdl->out_buf.num_of_buf; j++) {
                ret1 |= RefCV_MemblkAlloc(p_mem_req->flexidag_output_buffer_size[j], &phdl->out_buf.buf[j]);
            }

            ptp->blocking_run = blocking_run;
            ret1 |= AmbaKAL_MsgQueueCreate(&ptp->msg_queue, name, sizeof(UINT32), ptp->queue_buffer, sizeof(ptp->queue_buffer));

            if(ret == 0U) {
                AmbaPrint_PrintUInt5("thread[%d]: ", i, 0U, 0U, 0U, 0U);
                AmbaPrint_PrintUInt5("  num_runs        = %d ", ptp->num_runs, 0U, 0U, 0U, 0U);
                AmbaPrint_PrintUInt5("  sleep_usec      = %d ", ptp->sleep_usec, 0U, 0U, 0U, 0U);
                AmbaPrint_PrintStr5 ("  flexidag_path   = %s ", ptp->flexidag_path, NULL, NULL, NULL, NULL);
                AmbaPrint_PrintStr5 ("  file_nameY      = %s ", ptp->file_nameY, NULL, NULL, NULL, NULL);
                AmbaPrint_PrintUInt5("  fileY pa        = 0x%x size = 0x%x", ptp->file_regionY.buffer_daddr, ptp->file_regionY.buffer_size, 0U, 0U, 0U);
                AmbaPrint_PrintUInt5("  bin_buf pa      = 0x%x size = 0x%x", phdl->bin_buf.buffer_daddr, phdl->bin_buf.buffer_size, 0U, 0U, 0U);
                AmbaPrint_PrintUInt5("  state_buf pa    = 0x%x size = 0x%x", phdl->init.state_buf.buffer_daddr, phdl->init.state_buf.buffer_size, 0U, 0U, 0U);
                AmbaPrint_PrintUInt5("  temp_buf pa     = 0x%x size = 0x%x", phdl->init.temp_buf.buffer_daddr,  phdl->init.temp_buf.buffer_size, 0U, 0U, 0U);
                AmbaPrint_PrintUInt5("  in_buf pa       = 0x%x size = 0x%x", phdl->in_buf.buf[0].buffer_daddr,  phdl->in_buf.buf[0].buffer_size, 0U, 0U, 0U);
                AmbaPrint_PrintUInt5("  num of outputs  = %d", phdl->out_buf.num_of_buf, 0U, 0U, 0U, 0U);
                for (j = 0; j < phdl->out_buf.num_of_buf; j++) {
                    AmbaPrint_PrintUInt5("  out_buf%d pa     = 0x%x size = 0x%x", j, phdl->out_buf.buf[j].buffer_daddr, phdl->out_buf.buf[j].buffer_size, 0U, 0U);
                }
            }
        }
    }
    (void) ret1;
    return ret;
}

static UINT32 RefCVTestStop(UINT32 thread_id)
{
    UINT32 i, ret = 0U, all_stop = 0U;
    AMBA_KAL_TASK_INFO_s taskInfo;
    REF_CV_PARAMS_s *ptp;
    UINT32 ret1 = 0U;

    do {
        UINT32 still_run = 0U;
        for(i = 0U; i < thread_id; i ++) {
            ptp = &thread_params[i];
            if( ptp->RunFlag != 0U ) {
                still_run = 1U;
                break;
            }
        }
        if(still_run == 0U) {
            all_stop = 1U;
        }
        ret1 |= AmbaKAL_TaskSleep(250U); /* Sleep 250ms */
    } while (all_stop == 0U);

    for(i = 0U; i < thread_id; i ++) {
        ptp = &thread_params[i];
        AmbaPrint_PrintUInt5("RefCVTestRun thread (%d) total (%d) error (%d) ",ptp->id, ptp->cur_runs, ptp->num_err, 0U, 0U);
    }

    for(i = 0U; i < thread_id; i ++) {
        ptp = &thread_params[i];
        ret1 |= AmbaKAL_TaskQuery(&ptp->task, &taskInfo);
        if ((taskInfo.TaskState != TX_COMPLETED) && (taskInfo.TaskState != TX_TERMINATED)) {
            ret  = AmbaKAL_TaskTerminate(&ptp->task);
            if ( ret  != TX_SUCCESS ) {
                AmbaPrint_PrintUInt5("RefCVTestRun AmbaKAL_TaskTerminate fail (%d)", ret, 0U, 0U, 0U, 0U);
            }
        }
        ret = AmbaKAL_TaskDelete(&ptp->task);
        if ( ret != TX_SUCCESS ) {
            AmbaPrint_PrintUInt5("RefCVTestRun AmbaKAL_TaskDelete fail (%d)", ret, 0U, 0U, 0U, 0U);
        }
    }
    refcv_net_loop = 0U;
    (void) ret1;
    return ret;
}

static UINT32 RefCVTestRun(UINT32 thread_id)
{
    UINT32 i, ret = 0U;
    AMBA_FS_FILE *fp;
    UINT32 Fsize, op_res;
    UINT64 fpos = 0U;
    static UINT8 refcv_test_stack[MAX_THREAD_NUM][0x4000] GNU_SECTION_NOZEROINIT;
    char task_name[32] = "refcv_test";
    REF_CV_PARAMS_s *ptp;
    UINT32 ret1 = 0U;

    for(i = 0U; i < thread_id; i ++) {
        ptp     = &thread_params[i];

        // prepare input data
        ret = AmbaFS_FileOpen(ptp->file_nameY, "rb", &fp);
        if(ret !=  0U) {
            AmbaPrint_PrintUInt5("RefCVTestRun AmbaFS_FileOpen fail ", 0U, 0U, 0U, 0U, 0U);
        } else {
            ret1 |= AmbaFS_FileSeek(fp, 0LL, AMBA_FS_SEEK_END);
            ret1 |= AmbaFS_FileTell(fp, &fpos);
            Fsize = (UINT32)(fpos & 0xFFFFFFFFU);
            ret1 |= AmbaFS_FileSeek(fp, 0LL, AMBA_FS_SEEK_START);
            ret = AmbaFS_FileRead(ptp->file_regionY.pBuffer, 1U, (UINT32)Fsize, fp, &op_res);
            if(ret !=  0U) {
                AmbaPrint_PrintUInt5("RefCVTestRun AmbaFS_FileRead fail ", 0U, 0U, 0U, 0U, 0U);
            } else {
                ret = RefCV_MemblkClean(&ptp->file_regionY);
                if(ret !=  0U) {
                    AmbaPrint_PrintUInt5("RefCVTestRun RefCV_MemblkClean fail ", 0U, 0U, 0U, 0U, 0U);
                } else {
                    ptp->file_regionY.buffer_size = Fsize;
                }
            }
            ret1 |= AmbaFS_FileClose(fp);
        }
    }

    for(i = 0U; i < thread_id; i ++) {
        ptp = &thread_params[i];
        refcv_net_loop = 1U;
        ret = AmbaKAL_TaskCreate(&ptp->task,            /* pTask */
                                 task_name,                      /* pTaskName */
                                 100,                            /* Priority */
                                 RefCVFileInPicinfo,             /* void (*EntryFunction)(UINT32) */
                                 ptp,                            /* EntryArg */
                                 &refcv_test_stack[i][0],        /* pStackBase */
                                 sizeof(refcv_test_stack[i]),    /* StackByteSize */
                                 0U);

        if(ret == 0U) {
            ret1 |= AmbaKAL_TaskSetSmpAffinity(&ptp->task, 0x1U);
            ret1 |= AmbaKAL_TaskResume(&ptp->task);
        } else {
            AmbaPrint_PrintUInt5("RefCVTestRun AmbaKAL_TaskCreate fail (%d)", ret, 0U, 0U, 0U, 0U);
        }
    }

    ret1 |= AmbaKAL_TaskSleep(200U);
    ret = RefCVTestStop(thread_id);

    (void) ret1;
    return ret;
}

void RefCV_Test(UINT32 thread_id, UINT32 num_of_frame, UINT32 blocking_run)
{
    UINT32 Rval;

    Rval = RefCVTestParaInit(thread_id, num_of_frame, blocking_run);
    if(Rval == 0U) {
        Rval = RefCVTestRun(thread_id);
        if(Rval == 0U) {
        }
    }
}
