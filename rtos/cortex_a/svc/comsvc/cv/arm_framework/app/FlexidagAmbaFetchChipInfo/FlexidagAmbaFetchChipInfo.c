/**
 *  @file CvRef_FlexidagAlgoPipeExample.c
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Implementation of Cv simple pipeline
 *
 */
#include "AmbaTypes.h"
#include "cvapi_memio_interface.h"
#include "cvapi_ambacv_flexidag.h"
#include "cvapi_flexidag_fetch_chip_info.h"
#include <AmbaPrint.h>
#include <AmbaMisraFix.h>
#include <AmbaWrap.h>
#include "AmbaUtility.h"
#include "AmbaMMU.h"
#include "cvapi_flexidag_unittest.h"

typedef struct {
    UINT32                          id;
    AMBA_CV_FLEXIDAG_HANDLE_s       fd_gen_handle;
    AMBA_CV_FLEXIDAG_INIT_s         init;
    AMBA_CV_FLEXIDAG_IO_s           in_buf;
    AMBA_CV_FLEXIDAG_IO_s           out_buf;
    UINT32                          init_cfg;
} REF_CV_FETCH_INFO_HANDLE_s;

static REF_CV_FETCH_INFO_HANDLE_s handle;

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//
// Please note that in real vp use case, you will need to allocate
// your input/output/intermedia buffer in cv_memory region
// user need to handle cache betwnn blocks
//
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#define DBG_LOG     AmbaPrint_PrintUInt5

static UINT32 FetchChipInfoProcess(AMBA_CV_CHIP_INFO_t* Out)
{
    UINT32 Rval;
    AMBA_CV_FLEXIDAG_RUN_INFO_s RunInfo;
    UINT32 ret = 0U;

    handle.in_buf.num_of_buf = 1;
    if(handle.in_buf.buf[0].pBuffer == NULL) {
        ret |= RefCV_MemblkAlloc((uint32_t)sizeof(memio_source_recv_picinfo_t), &handle.in_buf.buf[0]);
    }

    handle.out_buf.num_of_buf = 1;
    if(handle.out_buf.buf[0].pBuffer == NULL) {
        ret |= RefCV_MemblkAlloc(128U, &handle.out_buf.buf[0]);
        ret |= RefCV_MemblkInvalid(&handle.out_buf.buf[0]);
    }

    Rval = AmbaCV_FlexidagRun(&handle.fd_gen_handle, &handle.in_buf, &handle.out_buf, &RunInfo);
    if (Rval != 0U) {
        AmbaPrint_PrintStr5("RunPicinfo : AmbaCV_FlexidagRun fail ", NULL, NULL, NULL, NULL, NULL);
    }
    ret |= RefCV_MemblkInvalid(&handle.out_buf.buf[0]);
    ret |= AmbaWrap_memcpy(Out, handle.out_buf.buf[0].pBuffer, sizeof(AMBA_CV_CHIP_INFO_t));
#if defined (CONFIG_BSP_CV2FSDK_OPTION_A_V110) || defined (CONFIG_BSP_CV28DK_8LAYER_A_V100)
    Rval = AmbaCV_FlexidagDumpLog(&handle.fd_gen_handle, "d:\\flexidag_fetch_chip_info/log.txt", FLEXILOG_CORE0 | FLEXILOG_VIS_CVTASK);
#else
    Rval = AmbaCV_FlexidagDumpLog(&handle.fd_gen_handle, "c:\\flexidag_fetch_chip_info/log.txt", FLEXILOG_CORE0 | FLEXILOG_VIS_CVTASK);
#endif
    if (Rval != 0U) {
        AmbaPrint_PrintUInt5("FetchChipInfoProcess (%d) AmbaCV_FlexidagDumpLog fail ",handle.id, 0U, 0U, 0U, 0U);
    } else {
        AmbaPrint_PrintUInt5("FetchChipInfoProcess (%d) AmbaCV_FlexidagDumpLog done ",handle.id, 0U, 0U, 0U, 0U);
    }

    /*
    Rval = AmbaCV_FlexidagClose(&Hdlr->fd_gen_handle);
    if (Rval != 0U) {
        AmbaPrint_PrintUInt5("FetchChipInfoProcess (%d) AmbaCV_FlexidagClose fail ",Hdlr->id, 0U, 0U, 0U, 0U);
    } else {
        AmbaPrint_PrintUInt5("FetchChipInfoProcess (%d) AmbaCV_FlexidagClose done ",Hdlr->id, 0U, 0U, 0U, 0U);
    }
    */

    if((ret != 0U) || (Rval != 0U)) {
        AmbaPrint_PrintUInt5("[ERROR] FetchChipInfoProcess fail ",0U, 0U, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 FetchChipInfoInit(const char* path)
{
    static UINT32 fetch_chip_info_init = 0U;
    static flexidag_memblk_t FlexidagBinBuf;
    AMBA_CV_FLEXIDAG_LOG_MSG_s set;
    const AMBA_CV_FLEXIDAG_LOG_MSG_s *pset;
    const void *param_set;
    UINT32 ret = 0U,ret1 = 0U,size_align = 0U;

    DBG_LOG("!!!! AmbaFetchChipInfo MUST boot dsp to get correct result !!!!", 0U, 0U, 0U, 0U, 0U);

    if (fetch_chip_info_init == 1U) {
        ret = 0U;
    } else {
        ret1 |= AmbaCV_UtilityFileSize(path, &size_align);
        if(FlexidagBinBuf.pBuffer == NULL) {
            ret1 |= RefCV_MemblkAlloc(size_align, &FlexidagBinBuf);
        }
        ret = AmbaCV_UtilityFileLoad(path, &FlexidagBinBuf);
        if (ret != 0U) {
            DBG_LOG("AmbaFetchChipInfo AmbaCV_UtilityFileLoad failed:%x", ret, 0U, 0U, 0U, 0U);
        }

        fetch_chip_info_init = 1;
        if(AmbaWrap_memset(&handle, 0x0, sizeof(REF_CV_FETCH_INFO_HANDLE_s)) != 0U) {
            AmbaPrint_PrintUInt5("[ERROR] FetchChipInfoInit AmbaWrap_memset fail ",0U, 0U, 0U, 0U, 0U);
        }
        // set param
        set.flexidag_msg_entry = 1U;        //no requirement to send msg from flexidag to cvtask
        set.cvtask_msg_entry = 1U;          //no requirement to send internal cvtask msg
        set.arm_cpu_map = 0xFU;
        set.arm_log_entry = 0U;
        set.orc_log_entry = 128U;
        pset = &set;
        AmbaMisra_TypeCast(&param_set, &pset);
        ret1 |= AmbaCV_FlexidagSetParamSet(&(handle.fd_gen_handle), FLEXIDAG_PARAMSET_LOG_MSG, param_set, (uint32_t)sizeof(AMBA_CV_FLEXIDAG_LOG_MSG_s));

        // Open
        if (ret == 0U) {
            ret = AmbaCV_FlexidagOpen(&FlexidagBinBuf, &(handle.fd_gen_handle));
            if (ret!=0U) {
                DBG_LOG("AmbaFetchChipInfo AmbaCV_FlexidagOpen failed:%x", ret, 0U, 0U, 0U, 0U);
            }
        }

        if(handle.init.state_buf.pBuffer == NULL) {
            ret1 |=  RefCV_MemblkAlloc(handle.fd_gen_handle.mem_req.flexidag_state_buffer_size, &handle.init.state_buf);
            DBG_LOG("AmbaFetchChipInfo state_buf %x", (uint32_t)handle.init.state_buf.buffer_daddr, 0U, 0U, 0U, 0U);
        }

        if((handle.init.temp_buf.pBuffer == NULL) && (handle.fd_gen_handle.mem_req.flexidag_temp_buffer_size != 0U)) {
            ret1 |=  RefCV_MemblkAlloc(handle.fd_gen_handle.mem_req.flexidag_state_buffer_size, &handle.init.temp_buf);
            DBG_LOG("AmbaFetchChipInfo temp_buf %x", (uint32_t)handle.init.temp_buf.buffer_daddr, 0U, 0U, 0U, 0U);
        }

        // Init
        if (ret == 0U) {
            ret = AmbaCV_FlexidagInit(&(handle.fd_gen_handle), &(handle.init));
            if (ret!=0U) {
                DBG_LOG("AmbaFetchChipInfo AmbaCV_FlexidagInit failed:%x", ret, 0U, 0U, 0U, 0U);
            }
        }

        // Dump init buf requirement
        DBG_LOG("state_buf size:%d, temp_buf size:%d", handle.fd_gen_handle.mem_req.flexidag_state_buffer_size, handle.fd_gen_handle.mem_req.flexidag_temp_buffer_size, 0U, 0U, 0U);
        handle.id = 0;
        handle.init_cfg = 0U;

    }

    if((ret != 0U) || (ret1 != 0U)) {
        AmbaPrint_PrintUInt5("[ERROR] FetchChipInfoInit fail 0x%x 0x%x ",ret, ret1, 0U, 0U, 0U);
    }
    return ret;
}

uint32_t AmbaCV_FetchChipInfo(const char* pFlexibinPath, AMBA_CV_CHIP_INFO_t *pChipInfo)
{
    UINT32 ret = 0U;

    // init
    ret |= FetchChipInfoInit(pFlexibinPath);
    pChipInfo->AmbaUUID[0] = 0U;

    // run
    ret |= FetchChipInfoProcess(pChipInfo);

    if(ret != 0U) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaCV_FetchChipInfo fail ",0U, 0U, 0U, 0U, 0U);
    }
    return 0U;
}

