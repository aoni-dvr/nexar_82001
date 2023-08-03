/**
 *  @file flexidag_FetchChipInfo.c
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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdint.h>
#include <assert.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include "cvapi_memio_interface.h"
#include "cvapi_ambacv_flexidag.h"
#include "cvapi_flexidag_fetch_chip_info.h"
#include "rtos/AmbaRTOSWrapper.h"


typedef struct {
	UINT32     						id;
    UINT32                          init_cfg;
	AMBA_CV_FLEXIDAG_HANDLE_s       fd_gen_handle;
    flexidag_memblk_t               bin_buf;
	AMBA_CV_FLEXIDAG_INIT_s			init;
	AMBA_CV_FLEXIDAG_IO_s           in_buf;
	AMBA_CV_FLEXIDAG_IO_s           out_buf;
} REF_CV_HANDLE_s;

uint32_t is_cache = 1;
static REF_CV_HANDLE_s handle;

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//
// Please note that in real vp use case, you will need to allocate
// your input/output/intermedia buffer in cv_memory region
// user need to handle cache betwnn blocks
//
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#define DBG_LOG     AmbaPrint_PrintUInt5
#define CRI_LOG     AmbaPrint_PrintUInt5

static UINT32 RunPicinfo(REF_CV_HANDLE_s *hdl, AMBA_CV_FLEXIDAG_RUN_INFO_s *run_info)
{
	UINT32 ret = 0, addr;
	const AMBA_CV_FLEXIDAG_HANDLE_s *fd_gen_handle = &hdl->fd_gen_handle;


    if (is_cache) {
        ret = AmbaCV_UtilityCmaMemClean(&hdl->in_buf.buf[0]);
    	if(ret != 0U) {
    		CRI_LOG("RunPicinfo : AmbaCache_DataClean fail", 0U, 0U, 0U, 0U, 0U);
            CRI_LOG("RunPicinfo : 0x%x %d", (UINT32)hdl->in_buf.buf[0].buffer_daddr, hdl->in_buf.buf[0].buffer_size, 0U, 0U, 0U);
    		ret = 1U;
    	}
    }


    ret = AmbaCV_FlexidagRun(fd_gen_handle, &hdl->in_buf, &hdl->out_buf, run_info);
	if (ret != 0U) {
		AmbaPrint_PrintStr5("RunPicinfo : AmbaCV_FlexidagRun fail ", NULL, NULL, NULL, NULL, NULL);
		return ret;
	}

    CRI_LOG("RunPicinfo : AmbaCV_FlexidagRun ok", 0U, 0U, 0U, 0U, 0U);

    if (is_cache) {
        ret = AmbaCV_UtilityCmaMemInvalid(&hdl->out_buf.buf[0]);
    	if(ret != 0U) {
    		CRI_LOG("RunPicinfo : AmbaCV_UtilityCmaMemInvalid fail", 0U, 0U, 0U, 0U, 0U);
            CRI_LOG("RunPicinfo : 0x%x %d", (UINT32)hdl->out_buf.buf[0].buffer_daddr, hdl->out_buf.buf[0].buffer_size, 0U, 0U, 0U);
    		ret = 1U;
    	}
    }

	return ret;
}

static UINT32 _FetchChipInfoProcess(const void* Handler, AMBA_CV_CHIP_INFO_t* Out)
{
    REF_CV_HANDLE_s* Hdlr = (REF_CV_HANDLE_s*)Handler;
    AMBA_CV_FLEXIDAG_RUN_INFO_s RunInfo;
    const void *vpIn, *vpOut;
    void *vpOut2;
    UINT32 ret;

    ret = AmbaCV_UtilityCmaMemAlloc(sizeof(memio_source_recv_picinfo_t), is_cache, &handle.in_buf.buf[0]);
    ret = AmbaCV_UtilityCmaMemAlloc(sizeof(AMBA_CV_CHIP_INFO_t), is_cache, &handle.out_buf.buf[0]);
    handle.in_buf.num_of_buf = 1;
    handle.out_buf.num_of_buf = 1;

    memset((void *) handle.in_buf.buf[0].pBuffer, 0xff, sizeof(AMBA_CV_CHIP_INFO_t));
    if (is_cache) {
        ret = AmbaCV_UtilityCmaMemInvalid(&handle.out_buf.buf[0]);
        if(ret != 0U) {
            AmbaPrint_PrintUInt5("_FetchChipInfoProcess AmbaCV_UtilityCmaMemInvalid fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
//    DBG_LOG("AmbaFetchChipInfo Process start", 0U, 0U, 0U, 0U, 0U);

    (void)RunPicinfo(Hdlr, &RunInfo);

//    DBG_LOG("AmbaFetchChipInfo Process end", 0U, 0U, 0U, 0U, 0U);
    memcpy(Out, handle.out_buf.buf[0].pBuffer, sizeof(AMBA_CV_CHIP_INFO_t));

    ret = AmbaCV_FlexidagDumpLog(&Hdlr->fd_gen_handle, "./flexidag_FetchChipInfo.log", FLEXILOG_CORE0 | FLEXILOG_VIS_CVTASK);
    if (ret != 0U) {
	    AmbaPrint_PrintUInt5("_FetchChipInfoProcess (%d) AmbaCV_FlexidagDumpLog fail ",Hdlr->id, 0U, 0U, 0U, 0U);
    } else {
        AmbaPrint_PrintUInt5("_FetchChipInfoProcess (%d) AmbaCV_FlexidagDumpLog done ",Hdlr->id, 0U, 0U, 0U, 0U);
    }

    ret = AmbaCV_FlexidagClose(&Hdlr->fd_gen_handle);
    if (ret != 0U) {
    	AmbaPrint_PrintUInt5("_FetchChipInfoProcess (%d) AmbaCV_FlexidagClose fail ",Hdlr->id, 0U, 0U, 0U, 0U);
    } else {
        AmbaPrint_PrintUInt5("_FetchChipInfoProcess (%d) AmbaCV_FlexidagClose done ",Hdlr->id, 0U, 0U, 0U, 0U);
    }

    ret = AmbaCV_UtilityCmaMemFree(&handle.bin_buf);
    ret = AmbaCV_UtilityCmaMemFree(&handle.init.temp_buf);
    ret = AmbaCV_UtilityCmaMemFree(&handle.init.state_buf);
    ret = AmbaCV_UtilityCmaMemFree(&handle.in_buf.buf[0]);
    ret = AmbaCV_UtilityCmaMemFree(&handle.out_buf.buf[0]);
    return ret;
}

static UINT32 _FetchChipInfoInit(const char* path)
{
#define AMBA_FETCH_CHIP_INFO_BIN_SZ          ((16U * 1024U) & 0xFFFFFF80U)
#define AMBA_FETCH_CHIP_INFO_TEMP_SZ         (1024U)
#define AMBA_FETCH_CHIP_INFO_STATE_SZ        ((32U * 1024U) & 0xFFFFFF80U)

    static UINT32 fetch_chip_info_init = 0U;
    const char* paddr;
	AMBA_CV_FLEXIDAG_LOG_MSG_s set;
	const AMBA_CV_FLEXIDAG_LOG_MSG_s *pset;
	const void *param_set;
    UINT32 ret = 0U;
    UINT32 addr, Paddr;

    DBG_LOG("!!!! AmbaFetchChipInfo MUST boot dsp to get correct result !!!!", 0U, 0U, 0U, 0U, 0U);

    (void)AmbaWrap_memset(&handle, 0x0, sizeof(REF_CV_HANDLE_s));

    ret = AmbaCV_UtilityCmaMemAlloc(AMBA_FETCH_CHIP_INFO_BIN_SZ, is_cache, &handle.bin_buf);
    ret = AmbaCV_UtilityCmaMemAlloc(AMBA_FETCH_CHIP_INFO_TEMP_SZ, is_cache, &handle.init.temp_buf);
    ret = AmbaCV_UtilityCmaMemAlloc(AMBA_FETCH_CHIP_INFO_STATE_SZ, is_cache, &handle.init.state_buf);

    DBG_LOG("AmbaFetchChipInfo bin_buf phys = 0x%x, virt = 0x%x, size = 0x%x", handle.bin_buf.buffer_daddr, (UINT64) handle.bin_buf.pBuffer, handle.bin_buf.buffer_size, 0U, 0U);
    DBG_LOG("AmbaFetchChipInfo state_buf phys = 0x%x, virt = 0x%x, size = 0x%x", handle.init.state_buf.buffer_daddr, (UINT64) handle.init.state_buf.pBuffer, handle.init.state_buf.buffer_size, 0U, 0U);
    DBG_LOG("AmbaFetchChipInfo temp_buf phys = 0x%x virt = 0x%x, size = 0x%x", handle.init.temp_buf.buffer_daddr, (UINT64) handle.init.temp_buf.pBuffer, handle.init.temp_buf.buffer_size, 0U, 0U);

    if (fetch_chip_info_init == 1U){
        ret = 0U;
    } else {
        uint32_t bin_size;
        ret = AmbaCV_UtilityFileLoad(path, &handle.bin_buf);
        if (ret != 0U) {
            DBG_LOG("AmbaFetchChipInfo AmbaCV_UtilityFileLoad failed: 0x%x", ret, 0U, 0U, 0U, 0U);
        }

        //fetch_chip_info_init = 1;

        // set param
        set.flexidag_msg_entry = 1U;		//no requirement to send msg from flexidag to cvtask
        set.cvtask_msg_entry = 1U;			//no requirement to send internal cvtask msg
        set.arm_cpu_map = 0xFU;
        set.arm_log_entry = 0U;
        set.orc_log_entry = 128U;
        pset = &set;
        AmbaMisra_TypeCast64(&param_set, &pset);
       (void)AmbaCV_FlexidagSetParamSet(&(handle.fd_gen_handle), FLEXIDAG_PARAMSET_LOG_MSG, param_set, sizeof(AMBA_CV_FLEXIDAG_LOG_MSG_s));
        // Open
        if (ret == 0U){
            ret = AmbaCV_FlexidagOpen(&handle.bin_buf, &(handle.fd_gen_handle));
            if (ret!=0U) {
                DBG_LOG("AmbaFetchChipInfo AmbaCV_FlexidagOpen failed:0x%x", ret, 0U, 0U, 0U, 0U);
            }
        }

        // Init
        if (ret == 0U){
            ret = AmbaCV_FlexidagInit(&(handle.fd_gen_handle), &(handle.init));
            if (ret!=0U) {
                DBG_LOG("AmbaFetchChipInfo AmbaCV_FlexidagInit failed: 0x%x", ret, 0U, 0U, 0U, 0U);
            }
        }
        handle.id = 0;
        handle.init_cfg = 0U;
    }

    return ret;
}

uint32_t AmbaCV_FetchChipInfo(const char* pFlexibinPath, AMBA_CV_CHIP_INFO_t *pChipInfo)
{
    // init
    (void )_FetchChipInfoInit(pFlexibinPath);
    pChipInfo->AmbaUUID[0] = 0U;

    // run
    (void )_FetchChipInfoProcess(&handle, pChipInfo);



    return 0U;
}


static void _DumpResult(const AMBA_CV_CHIP_INFO_t *out)
{
    INT32 i;
    printf("AmbaUUID bytes[00-31]: ", 0U, 0U, 0U, 0U, 0U);
    for (i = 0; i < 32; i++) {
        printf("%.2x", out->AmbaUUID[i]);
    }
    printf("\n");
}

static void RefCV_AmbaFCI_UT_Run(const char* InPath)
{
    AMBA_CV_CHIP_INFO_t Info;
    (void)AmbaCV_FetchChipInfo(InPath, &Info);
    _DumpResult(&Info);
}

int main(int argc, char **argv)
{
    static UINT32 init = 0;
    char path0[128]="../flexibin/flexibin0.bin";
	AMBA_CV_FLEXIDAG_SCHDR_CFG_s cfg;

	cfg.cpu_map = 0xD;
	cfg.log_level = LVL_DEBUG;
	AmbaCV_FlexidagSchdrStart(&cfg);


  	RefCV_AmbaFCI_UT_Run(path0);

    return 0;
}

