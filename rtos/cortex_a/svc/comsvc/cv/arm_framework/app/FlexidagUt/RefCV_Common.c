/**
 *  @file RefCV_Common.c
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
 *  @details SVC COMSVC CV Common APIs
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaINT.h"
#include "AmbaPrint.h"
#include "AmbaUtility.h"
#include "AmbaMisraFix.h"
#include "AmbaMMU.h"
#include "AmbaFS.h"
#include "AmbaWrap.h"
#include "AmbaCache.h"
#include "AmbaShell.h"
#include "cvapi_visutil.h"
#include "cvapi_memio_interface.h"
#include "cvapi_svccvalgo_memio_interface.h"
#include "cvapi_ambacv_flexidag.h"
#include "RefCV_Common.h"
#include "cvapi_flexidag_unittest.h"
#if defined(CONFIG_AMBALINK_BOOT_OS) || defined(CONFIG_LINUX)
#include "AmbaIPC_FlexidagIO.h"
#endif

#if defined(CONFIG_QNX)
#define RefCV_YieldTest
//#define RefCV_AccUT
//#define RefCV_AccCustomUT
#define SCA_AmbaSeg_UT
#define SCA_Plain_UT
#elif defined(CONFIG_LINUX)
#ifdef CONFIG_SOC_CV2
#define RefCV_AmbaFexV1_UT(x,...) do { } while (0)
#define RefCV_AmbaFmaV1_UT(x,...) do { } while (0)
#define RefCV_AmbaSpuFexV1_UT(x,...) do { } while (0)
#define RefCV_AmbaSpuFusionV1_UT(x,...) do { } while (0)
#define RefCV_AmbaSpuFusionV1_UT(x,...) do { } while (0)
#endif
#define RefCV_AmbaSofFex_UT(x,...) do { } while (0)
#define RefCV_YieldTest(x,...) do { } while (0)
#define RefCV_Net(x,...) do { } while (0)
#define SCA_AmbaSeg_UT(x,...) do { } while (0)
#define RefCV_AccUT(x,...) do { } while (0)
#define RefCV_AmbaSWFma_UT(x,...) do { } while (0)
#define RefCV_AccCustomUT(x,...) do { } while (0)
#define RefCV_AmbaFex_UT(x,...) do { } while (0)
#define RefCV_AmbaSpuFex_UT(x,...) do { } while (0)
#define RefCV_AmbaOfFex_UT(x,...) do { } while (0)
#define RefCV_AmbaSpuFusion_UT(x,...) do { } while (0)
#define SCA_Plain_UT(x,...) do { } while (0)
#endif

UINT32 RefCVSprintfStr(char *str, const char *str_src)
{
    SIZE_t len;

    len = AmbaUtility_StringLength(str_src);
    AmbaUtility_StringCopy(str, len + 1U, str_src);

    return len;
}

UINT32 RefCVSprintfUint1(
    char *str,
    UINT32 strbuf_size,
    const char *format,
    UINT32 arg0)
{
    UINT32 rval;

    rval = AmbaUtility_StringPrintUInt32(
               str, strbuf_size, format, 1, &arg0);

    return rval;
}

UINT32 RefCVSprintfUint2(
    char *str,
    UINT32 strbuf_size,
    const char *format,
    UINT32 arg0,
    UINT32 arg1)
{
    UINT32 rval;
    UINT32 args[2];

    args[0] = arg0;
    args[1] = arg1;

    rval = AmbaUtility_StringPrintUInt32(
               str, strbuf_size, format, 2, args);

    return rval;
}

UINT32 RefCVSprintfUint3(
    char *str,
    UINT32 strbuf_size,
    const char *format,
    UINT32 arg0,
    UINT32 arg1,
    UINT32 arg2)
{
    UINT32 rval;
    UINT32 args[3];

    args[0] = arg0;
    args[1] = arg1;
    args[2] = arg2;

    rval = AmbaUtility_StringPrintUInt32(
               str, strbuf_size, format, 3, args);

    return rval;
}

UINT32 RefCVSprintfUint4(
    char *str,
    UINT32 strbuf_size,
    const char *format,
    UINT32 arg0,
    UINT32 arg1,
    UINT32 arg2,
    UINT32 arg3)
{
    UINT32 rval;
    UINT32 args[4];

    args[0] = arg0;
    args[1] = arg1;
    args[2] = arg2;
    args[3] = arg3;

    rval = AmbaUtility_StringPrintUInt32(
               str, strbuf_size, format, 4, args);

    return rval;
}

UINT32 RefCVSprintfUint5(
    char *str,
    UINT32 strbuf_size,
    const char *format,
    UINT32 arg0,
    UINT32 arg1,
    UINT32 arg2,
    UINT32 arg3,
    UINT32 arg4)
{
    UINT32 rval;
    UINT32 args[5];

    args[0] = arg0;
    args[1] = arg1;
    args[2] = arg2;
    args[3] = arg3;
    args[4] = arg4;

    rval = AmbaUtility_StringPrintUInt32(
               str, strbuf_size, format, 5, args);

    return rval;
}

UINT32 RefCVRunPicinfo(REF_CV_HANDLE_s *handle, const cv_pic_info_t *in, AMBA_CV_FLEXIDAG_RUN_INFO_s *run_info)
{
    UINT32 ret = 0U, i;
    const AMBA_CV_FLEXIDAG_HANDLE_s *fd_gen_handle = &handle->fd_gen_handle;
    cv_pic_info_t *pDataIn;
    UINT32 U32DataIn;

    AmbaMisra_TypeCast(&pDataIn, &handle->in_buf.buf[0].pBuffer);
    ret = AmbaWrap_memcpy(pDataIn, in, sizeof(cv_pic_info_t));
    if(ret != 0U) {
        AmbaPrint_PrintStr5("RefCVRunPicinfo : name = %s AmbaWrap_memcpy fail ", handle->name, NULL, NULL, NULL, NULL);
        ret = 1U;
    } else {
        U32DataIn = handle->in_buf.buf[0].buffer_daddr;
        for (i = 0U ; i < MAX_HALF_OCTAVES ; i++) {
            pDataIn->rpLumaLeft[i] = in->rpLumaLeft[i] - U32DataIn;
            pDataIn->rpLumaRight[i] = in->rpLumaRight[i] - U32DataIn;
            pDataIn->rpChromaLeft[i] = in->rpChromaLeft[i] - U32DataIn;
            pDataIn->rpChromaRight[i] = in->rpChromaRight[i] - U32DataIn;
        }
        ret = RefCV_MemblkClean(&handle->in_buf.buf[0]);
        if(ret != 0U) {
            AmbaPrint_PrintStr5("RefCVRunPicinfo : name = %s RefCV_MemblkClean fail ", handle->name, NULL, NULL, NULL, NULL);
            ret = 1U;
        } else {
            if( handle->out_buf.num_of_buf != fd_gen_handle->mem_req.flexidag_num_outputs ) {
                AmbaPrint_PrintUInt5("RefCVRunPicinfo : mem_req.num_outputs (0x%x) != out_buf.num_of_buf (0x%x)", fd_gen_handle->mem_req.flexidag_num_outputs, handle->out_buf.num_of_buf, 0U, 0U, 0U);
                ret = 1U;
            } else {
                for(i = 0U; i < handle->out_buf.num_of_buf; i++) {
                    if(fd_gen_handle->mem_req.flexidag_output_buffer_size[i] > handle->out_buf.buf[i].buffer_size) {
                        AmbaPrint_PrintUInt5("RefCVRunPicinfo : mem_req.output_size[%d] (0x%x) > out_buf (0x%x)", i, fd_gen_handle->mem_req.flexidag_output_buffer_size[i], handle->out_buf.buf[i].buffer_size, 0U, 0U);
                        ret = 1U;
                    }

                    if (ret != 0U) {
                        break;
                    }
                }

                if (ret == 0U) {
                    ret = AmbaCV_FlexidagRun(fd_gen_handle, &handle->in_buf, &handle->out_buf, run_info);
                    if (ret != 0U) {
                        AmbaPrint_PrintStr5("RefCVRunPicinfo : name = %s AmbaCV_FlexidagRun fail ", handle->name, NULL, NULL, NULL, NULL);
                        ret = 1U;
                    }
                }
            }
        }
    }
    return ret;
}

UINT32 RefCVRunNonBlockingPicinfo(REF_CV_HANDLE_s *handle, flexidag_cb cb, void *cb_param, const cv_pic_info_t *in, UINT32 *token_id)
{
    UINT32 ret = 0U, i;
    AMBA_CV_FLEXIDAG_HANDLE_s *fd_gen_handle = &handle->fd_gen_handle;
    cv_pic_info_t *pDataIn;
    UINT32 U32DataIn;

    AmbaMisra_TypeCast(&pDataIn, &handle->in_buf.buf[0].pBuffer);
    ret = AmbaWrap_memcpy(pDataIn, in, sizeof(cv_pic_info_t));
    if(ret != 0U) {
        AmbaPrint_PrintStr5("RefCVRunPicinfo : name = %s AmbaWrap_memcpy fail ", handle->name, NULL, NULL, NULL, NULL);
        ret = 1U;
    } else {
        U32DataIn = handle->in_buf.buf[0].buffer_daddr;
        for (i = 0U ; i < MAX_HALF_OCTAVES ; i++) {
            pDataIn->rpLumaLeft[i] = in->rpLumaLeft[i] - U32DataIn;
            pDataIn->rpLumaRight[i] = in->rpLumaRight[i] - U32DataIn;
            pDataIn->rpChromaLeft[i] = in->rpChromaLeft[i] - U32DataIn;
            pDataIn->rpChromaRight[i] = in->rpChromaRight[i] - U32DataIn;
        }
        ret = RefCV_MemblkClean(&handle->in_buf.buf[0]);
        if(ret != 0U) {
            AmbaPrint_PrintStr5("RefCVRunPicinfo : name = %s RefCV_MemblkClean fail ", handle->name, NULL, NULL, NULL, NULL);
            ret = 1U;
        } else {
            if( handle->out_buf.num_of_buf != fd_gen_handle->mem_req.flexidag_num_outputs ) {
                AmbaPrint_PrintUInt5("RefCVRunPicinfo : mem_req.num_outputs (0x%x) != out_buf.num_of_buf (0x%x)", fd_gen_handle->mem_req.flexidag_num_outputs, handle->out_buf.num_of_buf, 0U, 0U, 0U);
                ret = 1U;
            } else {
                for(i = 0U; i < handle->out_buf.num_of_buf; i++) {
                    if(fd_gen_handle->mem_req.flexidag_output_buffer_size[i] > handle->out_buf.buf[i].buffer_size) {
                        AmbaPrint_PrintUInt5("RefCVRunPicinfo : mem_req.output_size[%d] (0x%x) > out_buf (0x%x)", i, fd_gen_handle->mem_req.flexidag_output_buffer_size[i], handle->out_buf.buf[i].buffer_size, 0U, 0U);
                        ret = 1U;
                    }

                    if (ret != 0U) {
                        break;
                    }
                }

                if (ret == 0U) {
                    ret = AmbaCV_FlexidagRunNonBlock(fd_gen_handle, cb, cb_param, &handle->in_buf, &handle->out_buf, token_id);
                    if (ret != 0U) {
                        AmbaPrint_PrintStr5("RefCVRunPicinfo : name = %s AmbaCV_FlexidagRun fail ", handle->name, NULL, NULL, NULL, NULL);
                        ret = 1U;
                    }
                }
            }
        }
    }
    return ret;
}

UINT32 RefCVRunRaw(REF_CV_HANDLE_s *handle, const memio_source_recv_multi_raw_t *in, AMBA_CV_FLEXIDAG_RUN_INFO_s *run_info)
{
    UINT32 ret = 0U, i;
    const AMBA_CV_FLEXIDAG_HANDLE_s *fd_gen_handle = &handle->fd_gen_handle;
    memio_source_recv_raw_t *pDataIn;

    if( handle->in_buf.num_of_buf != in->num_io ) {
        AmbaPrint_PrintUInt5("RefCVRunRaw : in->num_io (0x%x) != in_buf.num_of_buf (0x%x)", in->num_io, handle->in_buf.num_of_buf, 0U, 0U, 0U);
        ret = 1U;
    } else {
        for(i = 0U ; i < in->num_io ; i++) {
            AmbaMisra_TypeCast(&pDataIn, &handle->in_buf.buf[i].pBuffer);
            ret = AmbaWrap_memcpy(pDataIn, &in->io[i], sizeof(memio_source_recv_raw_t));
            if (ret != 0U) {
                AmbaPrint_PrintStr5("RefCVRunRaw : name = %s AmbaWrap_memcpy fail ", handle->name, NULL, NULL, NULL, NULL);
                ret = 1U;
            } else {
                ret = RefCV_MemblkClean(&handle->in_buf.buf[i]);
                if (ret != 0U) {
                    AmbaPrint_PrintStr5("RefCVRunRaw : name = %s RefCV_MemblkClean fail ", handle->name, NULL, NULL, NULL, NULL);
                    ret = 1U;

                }
            }

            if(ret != 0U) {
                break;
            }
        }

        if(ret == 0U) {
            if( handle->out_buf.num_of_buf != fd_gen_handle->mem_req.flexidag_num_outputs ) {
                AmbaPrint_PrintUInt5("RefCVRunRaw : mem_req.num_outputs (0x%x) != out_buf.num_of_buf (0x%x)", fd_gen_handle->mem_req.flexidag_num_outputs, handle->out_buf.num_of_buf, 0U, 0U, 0U);
                ret = 1U;
            } else {
                for(i = 0U; i < handle->out_buf.num_of_buf; i++) {
                    if(fd_gen_handle->mem_req.flexidag_output_buffer_size[i] > handle->out_buf.buf[i].buffer_size) {
                        AmbaPrint_PrintUInt5("RefCVRunRaw : mem_req.output_size[%d] (0x%x) > out_buf (0x%x)", i, fd_gen_handle->mem_req.flexidag_output_buffer_size[i], handle->out_buf.buf[i].buffer_size, 0U, 0U);
                        ret = 1U;
                    }

                    if(ret != 0U) {
                        break;
                    }
                }

                if (ret == 0U) {
                    ret = AmbaCV_FlexidagRun(fd_gen_handle, &handle->in_buf, &handle->out_buf, run_info);
                    if (ret != 0U) {
                        AmbaPrint_PrintStr5("RefCVRunRaw : name = %s AmbaCV_FlexidagRun fail ", handle->name, NULL, NULL, NULL, NULL);
                        ret = 1U;
                    }
                }
            }
        }
    }

    return ret;
}

UINT32 RefCVRunNonBlockingRaw(REF_CV_HANDLE_s *handle, flexidag_cb cb, void *cb_param, const memio_source_recv_multi_raw_t *in, UINT32 *token_id)
{
    UINT32 ret = 0U, i;
    AMBA_CV_FLEXIDAG_HANDLE_s *fd_gen_handle = &handle->fd_gen_handle;
    memio_source_recv_raw_t *pDataIn;

    if( handle->in_buf.num_of_buf != in->num_io ) {
        AmbaPrint_PrintUInt5("RefCV_RunNonBlockingRaw : in->num_io (0x%x) != in_buf.num_of_buf (0x%x)", in->num_io, handle->in_buf.num_of_buf, 0U, 0U, 0U);
        ret = 1U;
    } else {
        for(i = 0U ; i < in->num_io ; i++) {
            AmbaMisra_TypeCast(&pDataIn, &handle->in_buf.buf[i].pBuffer);
            ret = AmbaWrap_memcpy(pDataIn, &in->io[i], sizeof(memio_source_recv_raw_t));
            if (ret != 0U) {
                AmbaPrint_PrintStr5("RefCV_RunNonBlockingRaw : name = %s AmbaWrap_memcpy fail ", handle->name, NULL, NULL, NULL, NULL);
                ret = 1U;
            } else {
                ret = RefCV_MemblkClean(&handle->in_buf.buf[i]);
                if (ret != 0U) {
                    AmbaPrint_PrintStr5("RefCV_RunNonBlockingRaw : name = %s RefCV_MemblkClean fail ", handle->name, NULL, NULL, NULL, NULL);
                    ret = 1U;

                }
            }

            if(ret != 0U) {
                break;
            }
        }

        if(ret == 0U) {
            if( handle->out_buf.num_of_buf != fd_gen_handle->mem_req.flexidag_num_outputs ) {
                AmbaPrint_PrintUInt5("RefCV_RunNonBlockingRaw : mem_req.num_outputs (0x%x) != out_buf.num_of_buf (0x%x)", fd_gen_handle->mem_req.flexidag_num_outputs, handle->out_buf.num_of_buf, 0U, 0U, 0U);
                ret = 1U;
            } else {
                for(i = 0U; i < handle->out_buf.num_of_buf; i++) {
                    if(fd_gen_handle->mem_req.flexidag_output_buffer_size[i] > handle->out_buf.buf[i].buffer_size) {
                        AmbaPrint_PrintUInt5("RefCV_RunNonBlockingRaw : mem_req.output_size[%d] (0x%x) > out_buf (0x%x)", i, fd_gen_handle->mem_req.flexidag_output_buffer_size[i], handle->out_buf.buf[i].buffer_size, 0U, 0U);
                        ret = 1U;
                    }

                    if(ret != 0U) {
                        break;
                    }
                }

                if (ret == 0U) {
                    ret = AmbaCV_FlexidagRunNonBlock(fd_gen_handle, cb, cb_param, &handle->in_buf, &handle->out_buf, token_id);
                    if (ret != 0U) {
                        AmbaPrint_PrintStr5("RefCV_RunNonBlockingRaw : name = %s AmbaCV_FlexidagRun fail ", handle->name, NULL, NULL, NULL, NULL);
                        ret = 1U;
                    }
                }
            }
        }
    }

    return ret;
}

UINT32 RefCV_MemblkInit(void)
{
    uint32_t ret = 0U;

    return ret;
}

UINT32 RefCV_MemblkClean(const flexidag_memblk_t* buf)
{
    uint32_t ret = 0U;
    flexidag_memblk_t tmp_buf;

    if(AmbaWrap_memcpy(&tmp_buf, buf, sizeof(flexidag_memblk_t)) != 0U) {
        AmbaPrint_PrintUInt5("RefCV_MemblkClean : AmbaWrap_memcpy fail", 0U, 0U, 0U, 0U, 0U);
    }

    ret = AmbaCV_UtilityCmaMemClean(&tmp_buf);

    return ret;
}

UINT32 RefCV_MemblkInvalid(const flexidag_memblk_t* buf)
{
    uint32_t ret = 0U;
    flexidag_memblk_t tmp_buf;

    if(AmbaWrap_memcpy(&tmp_buf, buf, sizeof(flexidag_memblk_t)) != 0U) {
        AmbaPrint_PrintUInt5("RefCV_MemblkInvalid : AmbaWrap_memcpy fail", 0U, 0U, 0U, 0U, 0U);
    }
    ret = AmbaCV_UtilityCmaMemInvalid(&tmp_buf);

    return ret;
}


UINT32 RefCV_MemblkAlloc(UINT32 buf_size, flexidag_memblk_t* buf)
{
    uint32_t ret = 0U,ret1 = 0U;

    ret = AmbaCV_UtilityCmaMemAlloc(buf_size, 1U, buf);
    ret1 |= AmbaWrap_memset(buf->pBuffer, 0, buf->buffer_size);
    ret1 |= RefCV_MemblkClean(buf);
    (void) ret1;
    return ret;
}

UINT32 RefCV_MemblkFree(flexidag_memblk_t* buf)
{
    uint32_t ret = 0U;

    ret = AmbaCV_UtilityCmaMemFree(buf);

    return ret;
}

UINT32 RefCV_UT_GetCVBuf(UINT8** ppU8, UINT32 Size, UINT32* AlignedSize)
{
    UINT32 Ret = 0U;
    flexidag_memblk_t buf;

    buf.pBuffer = NULL;
    buf.buffer_daddr = 0U;
    buf.buffer_cacheable = 0U;
    buf.buffer_size = 0U;
    buf.buffer_caddr = 0U;
    buf.reserved_expansion = 0U;
    Ret = RefCV_MemblkAlloc(Size, &buf);
    if (Ret == 0U) {
        AmbaMisra_TypeCast(ppU8, &buf.pBuffer);
        *AlignedSize = buf.buffer_size;
    } else {
        AmbaPrint_PrintUInt5("RefCV_UT_GetCVBuf: OOM", 0U, 0U, 0U, 0U, 0U);
    }
    return Ret;
}

void RefCV_TestHelp(AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("    flexidag run freq [num]\n");
    PrintFunc("    flexidag run [mode] [thread num] [loop num][blocking_run]\n");
    PrintFunc("        [mode]: mnetssd 1 1 1\n");
    PrintFunc("                bisenet 1 1 1\n");
    PrintFunc("                net_test [path]\n");
    PrintFunc("    flexidag run [mode] [loop num]\n");
#ifdef CONFIG_CV_FLEXIDAG_AMBAODFC
    PrintFunc("                ambaod [flexibin] [log file] [in file] [outfile] [license] [mode=0]\n");
#endif
#if defined(CONFIG_CV_FLEXIDAG_AMBASEGFC) || defined(CONFIG_CV_FLEXIDAG_AMBASEG39FC)
    PrintFunc("                ambaseg [flexibin] [log file] [in file] [outfile] [license] [mode=0]\n");
#endif
#ifdef CONFIG_CV_FLEXIDAG_AMBASEGAVM
    PrintFunc("                ambaseg_avm [flexibin] [log file] [in file] [outfile] [license] [mode=0]\n");
#endif
#ifdef CONFIG_CV_FLEXIDAG_AMBAPERCEPTION
    PrintFunc("                ambapcpt [enable] [flexibin...] [log path] [in file] [license] [loop]\n");
    PrintFunc("                    [enable]: 0x0: 2d, 0x1: kp, 0x2: 3d, 0x4: mk, 0x8: tsc, 0x20: tlsc, 0x40: arc\n");
#endif
#ifdef CONFIG_BUILD_AMBA_ADAS_UNITTEST
    PrintFunc("                ambaadas_fc [Seg file] [OD file] [OD_3D file] [OD_MK file] [CANBus file] [Calibration file] [focal length(um)] [Process step] [output file]\n");
#endif

}

static UINT32 RefCV_UtCommand1(UINT32 ArgCount, char * const * pArgVector)
{
    static UINT32 cv_freq = 1U;
    UINT32 thread,loop,blocking_run,fund = 1U;
    UINT32 ret = 0U;

    if ((AmbaUtility_StringCompare(pArgVector[0], "freq", 4U) == 0) && (ArgCount == 2U)) {
        ret |= AmbaUtility_StringToUInt32(pArgVector[1],&cv_freq);
#if defined(CONFIG_AMBALINK_BOOT_OS) || defined(CONFIG_LINUX)
    } else if (AmbaUtility_StringCompare(pArgVector[0], "flexidagio", 10U) == 0) {
        if ((AmbaUtility_StringCompare(pArgVector[1], "reinit", 6U) == 0) && (ArgCount == 3U)) {
            UINT32 channel;
            ret |= AmbaUtility_StringToUInt32(pArgVector[2],&channel);
            ret |= AmbaIPC_FlexidagIO_Deinit(channel);
            ret |= AmbaIPC_FlexidagIO_Init(channel, NULL);
        }
    } else if ((AmbaUtility_StringCompare(pArgVector[0], "mnetssdambalink", 15U) == 0) && (ArgCount == 4U)) {
        ret |= AmbaUtility_StringToUInt32(pArgVector[1],&thread);
        ret |= AmbaUtility_StringToUInt32(pArgVector[2],&loop);
        ret |= AmbaUtility_StringToUInt32(pArgVector[3],&blocking_run);
        RefCV_MnetSSD(thread,loop,blocking_run,(UINT8)(cv_freq& 0xFFU), REF_CV_RUN_MODE_AMBALINK);
#endif
    } else if ((AmbaUtility_StringCompare(pArgVector[0], "restart", 7U) == 0) && (ArgCount == 2U)) {
        ret |= AmbaUtility_StringToUInt32(pArgVector[1],&loop);
        RefCV_Restart(loop);
    } else if ((AmbaUtility_StringCompare(pArgVector[0], "mnetssd", 7U) == 0) && (ArgCount == 4U)) {
        ret |= AmbaUtility_StringToUInt32(pArgVector[1],&thread);
        ret |= AmbaUtility_StringToUInt32(pArgVector[2],&loop);
        ret |= AmbaUtility_StringToUInt32(pArgVector[3],&blocking_run);
        RefCV_MnetSSD(thread,loop,blocking_run,(UINT8)(cv_freq& 0xFFU), REF_CV_RUN_MODE_RTOS);
    } else if ((AmbaUtility_StringCompare(pArgVector[0], "bisenet", 7U) == 0) && (ArgCount == 4U)) {
        ret |= AmbaUtility_StringToUInt32(pArgVector[1],&thread);
        ret |= AmbaUtility_StringToUInt32(pArgVector[2],&loop);
        ret |= AmbaUtility_StringToUInt32(pArgVector[3],&blocking_run);
        RefCV_Bisenet(thread,loop,blocking_run,(UINT8)(cv_freq& 0xFFU));
    } else if ((AmbaUtility_StringCompare(pArgVector[0], "net_test", 8U) == 0)  && (ArgCount == 2U)) {
        RefCV_Net(pArgVector[1]);
    } else if ((AmbaUtility_StringCompare(pArgVector[0], "openod", 6U) == 0)  && (ArgCount == 6U)) {
        ret |= AmbaUtility_StringToUInt32(pArgVector[5],&loop);
        SCA_OpenOD_UT(pArgVector[1], pArgVector[2], pArgVector[3], pArgVector[4], loop);
    } else if ((AmbaUtility_StringCompare(pArgVector[0], "test", 4U) == 0) && (ArgCount == 4U)) {
        ret |= AmbaUtility_StringToUInt32(pArgVector[1],&thread);
        ret |= AmbaUtility_StringToUInt32(pArgVector[2],&loop);
        ret |= AmbaUtility_StringToUInt32(pArgVector[3],&blocking_run);
    } else if ((AmbaUtility_StringCompare(pArgVector[0], "yield_test", 10U) == 0) && (ArgCount == 4U)) {
        ret |= AmbaUtility_StringToUInt32(pArgVector[1],&thread);
        ret |= AmbaUtility_StringToUInt32(pArgVector[2],&loop);
        ret |= AmbaUtility_StringToUInt32(pArgVector[3],&blocking_run);

        RefCV_YieldTest(thread,loop,blocking_run);
#if defined(CONFIG_BUILD_CTFW)
#if defined(CONFIG_CV_FLEXIDAG_STIXEL_V2)
    } else if ((AmbaUtility_StringCompare(pArgVector[0], "ctfw_stixelv2", 13U) == 0) && (ArgCount == 6U)) {
        UINT32 StartFrameIdx;
        UINT32 EndFrameIdx;
        UINT32 StixelRoadDetMode;

        ret |= AmbaUtility_StringToUInt32(pArgVector[3U], &StartFrameIdx);
        ret |= AmbaUtility_StringToUInt32(pArgVector[4U], &EndFrameIdx);
        ret |= AmbaUtility_StringToUInt32(pArgVector[5U], &StixelRoadDetMode);
        RefCV_CtfwStixelV2_UT(pArgVector[1U], pArgVector[2U],
                              StartFrameIdx, EndFrameIdx, StixelRoadDetMode);
#endif
#if defined(CONFIG_CV_FLEXIDAG_STIXEL)
#ifndef CONFIG_THREADX
    } else if ((AmbaUtility_StringCompare(pArgVector[0], "ctfw_ac_stixel", 14U) == 0) && (ArgCount == 6U)) {
        UINT32 StartFrameIdx;
        UINT32 EndFrameIdx;
        UINT32 FrameIdxStep;
        UINT32 StixelRoadDetMode;

        ret |= AmbaUtility_StringToUInt32(pArgVector[2U], &StartFrameIdx);
        ret |= AmbaUtility_StringToUInt32(pArgVector[3U], &EndFrameIdx);
        ret |= AmbaUtility_StringToUInt32(pArgVector[4U], &FrameIdxStep);
        ret |= AmbaUtility_StringToUInt32(pArgVector[5U], &StixelRoadDetMode);

        RefCV_CtfwAcStixel_UT(pArgVector[1U], StartFrameIdx,
                              EndFrameIdx, FrameIdxStep, StixelRoadDetMode);
#endif
    } else if ((AmbaUtility_StringCompare(pArgVector[0], "ctfw_stixel", 11U) == 0) && (ArgCount == 5U)) {
        UINT32 StartFrameIdx;
        UINT32 EndFrameIdx;
        UINT32 StixelRoadDetMode;

        ret |= AmbaUtility_StringToUInt32(pArgVector[2U], &StartFrameIdx);
        ret |= AmbaUtility_StringToUInt32(pArgVector[3U], &EndFrameIdx);
        ret |= AmbaUtility_StringToUInt32(pArgVector[4U], &StixelRoadDetMode);
        RefCV_CtfwStixel_UT(pArgVector[1U],
                            StartFrameIdx, EndFrameIdx, StixelRoadDetMode);
#endif
#ifndef CONFIG_THREADX
    } else if ((AmbaUtility_StringCompare(pArgVector[0], "ctfw_ac", 7U) == 0) && (ArgCount == 5U)) {
        UINT32 StartFrameIdx;
        UINT32 EndFrameIdx;
        UINT32 FrameIdxStep;

        ret |= AmbaUtility_StringToUInt32(pArgVector[2U], &StartFrameIdx);
        ret |= AmbaUtility_StringToUInt32(pArgVector[3U], &EndFrameIdx);
        ret |= AmbaUtility_StringToUInt32(pArgVector[4U], &FrameIdxStep);

        RefCV_CtfwAc_UT(pArgVector[1U], StartFrameIdx,
                        EndFrameIdx, FrameIdxStep);
#endif
#endif
    } else {
        fund = 0U;
    }

    (void)ret;
    return fund;
}



void RefCV_UtCommand(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 ID, loop, input_count, output_count, output_offset;
    UINT32 ret = 0U;

    if (RefCV_UtCommand1(ArgCount, pArgVector) == 1U) {
#ifdef CONFIG_CV_FLEXIDAG_AMBAPERCEPTION
    } else if ((AmbaUtility_StringCompare(pArgVector[0], "ambapcpt", 8U) == 0)  && (ArgCount >= 6U)) {
        SCA_AmbaPcpt_UT(pArgVector);
#endif
#ifdef CONFIG_CV_FLEXIDAG_AMBAODFC
    } else if ((AmbaUtility_StringCompare(pArgVector[0], "ambaod", 6U) == 0)  && (ArgCount == 7U)) {
        ret |= AmbaUtility_StringToUInt32(pArgVector[6],&loop);
        SCA_AmbaOD_UT(pArgVector[1], pArgVector[2], pArgVector[3], pArgVector[4], pArgVector[5], loop);
#endif
#if defined(CONFIG_CV_FLEXIDAG_AMBASEGFC) || defined(CONFIG_CV_FLEXIDAG_AMBASEG39FC)
    } else if ((AmbaUtility_StringCompare(pArgVector[0], "ambaseg", 7U) == 0)  && (ArgCount == 7U)) {
        ret |= AmbaUtility_StringToUInt32(pArgVector[6],&loop);
        SCA_AmbaSeg_UT(pArgVector[1], pArgVector[2], pArgVector[3], pArgVector[4], pArgVector[5], loop);
#endif
#ifdef CONFIG_CV_FLEXIDAG_AMBASEGAVM
    } else if ((AmbaUtility_StringCompare(pArgVector[0], "ambaseg_avm", 11U) == 0)  && (ArgCount == 6U)) {
        ret |= AmbaUtility_StringToUInt32(pArgVector[6],&loop);
        SCA_AmbaSeg_UT(pArgVector[1], pArgVector[2], pArgVector[3], pArgVector[4], pArgVector[5], loop);
#endif
    } else if ((AmbaUtility_StringCompare(pArgVector[0], "fci", 3U) == 0) && (ArgCount == 2U)) {
        ret |= AmbaUtility_StringToUInt32(pArgVector[1],&loop);
        RefCV_AmbaFCI_UT(loop);
    } else if ((AmbaUtility_StringCompare(pArgVector[0], "openseg_fc", 10U) == 0) && (ArgCount == 6U)) {
        ret |= AmbaUtility_StringToUInt32(pArgVector[5],&loop);
        SCA_OpenSeg_UT(pArgVector[1], pArgVector[2], pArgVector[3], pArgVector[4], loop);
#ifndef CONFIG_SOC_CV2
#ifndef CONFIG_QNX
    } else if ((AmbaUtility_StringCompare(pArgVector[0], "ambaswfma", 9U) == 0) && (ArgCount == 6U)) {
        ret |= AmbaUtility_StringToUInt32(pArgVector[5],&ID);
        RefCV_AmbaSWFma_UT(pArgVector[1], pArgVector[2], pArgVector[3], pArgVector[4], ID);
#endif
#endif
#ifdef CONFIG_SOC_CV2
    } else if ((AmbaUtility_StringCompare(pArgVector[0], "ambaspufexv1", 12U) == 0)  && (ArgCount == 12U)) {
        UINT32 Width, Height;
        UINT32 RoiStartX, RoiStartY, RoiWidth, RoiHeight, FusionOutput;
        ret |= AmbaUtility_StringToUInt32(pArgVector[5],&FusionOutput);
        ret |= AmbaUtility_StringToUInt32(pArgVector[6],&Width);
        ret |= AmbaUtility_StringToUInt32(pArgVector[7],&Height);
        ret |= AmbaUtility_StringToUInt32(pArgVector[8],&RoiStartX);
        ret |= AmbaUtility_StringToUInt32(pArgVector[9],&RoiStartY);
        ret |= AmbaUtility_StringToUInt32(pArgVector[10],&RoiWidth);
        ret |= AmbaUtility_StringToUInt32(pArgVector[11],&RoiHeight);

        RefCV_AmbaSpuFexV1_UT(pArgVector[1], pArgVector[2], pArgVector[3], pArgVector[4], FusionOutput, Width, Height,
                              RoiStartX, RoiStartY, RoiWidth, RoiHeight);
    } else if ((AmbaUtility_StringCompare(pArgVector[0], "ambafexv1", 9U) == 0) && (ArgCount >= 11U)) {
        UINT32 Width, Height;
        UINT32 RoiStartX, RoiStartY, RoiWidth, RoiHeight;
        UINT32 ScaleId = 0U;
        UINT32 EchoMode = 0U;
        ret |= AmbaUtility_StringToUInt32(pArgVector[5],&Width);
        ret |= AmbaUtility_StringToUInt32(pArgVector[6],&Height);
        ret |= AmbaUtility_StringToUInt32(pArgVector[7],&RoiStartX);
        ret |= AmbaUtility_StringToUInt32(pArgVector[8],&RoiStartY);
        ret |= AmbaUtility_StringToUInt32(pArgVector[9],&RoiWidth);
        ret |= AmbaUtility_StringToUInt32(pArgVector[10],&RoiHeight);
        if(ArgCount >= 12U) {
            ret |= AmbaUtility_StringToUInt32(pArgVector[11],&ScaleId);
        }
        if (ArgCount >= 13U) {
            ret |= AmbaUtility_StringToUInt32(pArgVector[12],&EchoMode);
        }

        RefCV_AmbaFexV1_UT(pArgVector[1], pArgVector[2], pArgVector[3], pArgVector[4], Width, Height,
                           RoiStartX, RoiStartY, RoiWidth, RoiHeight, ScaleId, EchoMode);
    } else if ((AmbaUtility_StringCompare(pArgVector[0], "ambaspufusionv1", 15U) == 0)  && (ArgCount == 8U)) {
        UINT32 LayerNum;
        ret |= AmbaUtility_StringToUInt32(pArgVector[3], &LayerNum);
        if ( LayerNum == 2U ) {
            RefCV_AmbaSpuFusionV1_UT(pArgVector[1], pArgVector[2], LayerNum, pArgVector[4], pArgVector[5], NULL, pArgVector[7]);
        } else if ( LayerNum == 3U ) {
            RefCV_AmbaSpuFusionV1_UT(pArgVector[1], pArgVector[2], LayerNum, pArgVector[4], pArgVector[5], pArgVector[6], pArgVector[7]);
        } else {
            // Not supported
        }
    } else if ((AmbaUtility_StringCompare(pArgVector[0], "ambafmav1", 9U) == 0) && (ArgCount >= 5U)) {
        UINT32 ScaleId, MvacMode, EchoMode;
        ScaleId = 0U;
        MvacMode = 0U;
        EchoMode = 0U;
        if(ArgCount >= 7U) {
            ret |= AmbaUtility_StringToUInt32(pArgVector[5],&ScaleId);
            ret |= AmbaUtility_StringToUInt32(pArgVector[6],&MvacMode);
        }
        if(ArgCount >= 8U) {
            ret |= AmbaUtility_StringToUInt32(pArgVector[7],&EchoMode);
        }
        RefCV_AmbaFmaV1_UT(pArgVector[1], pArgVector[2], pArgVector[3], pArgVector[4], ScaleId, MvacMode, EchoMode);
#ifdef CONFIG_THREADX
    } else if ((AmbaUtility_StringCompare(pArgVector[0], "ambaspufex", 10U) == 0)  && (ArgCount == 12U)) {
        UINT32 Width, Height;
        UINT32 RoiStartX, RoiStartY, RoiWidth, RoiHeight, FusionOutput;
        ret |= AmbaUtility_StringToUInt32(pArgVector[5],&FusionOutput);
        ret |= AmbaUtility_StringToUInt32(pArgVector[6],&Width);
        ret |= AmbaUtility_StringToUInt32(pArgVector[7],&Height);
        ret |= AmbaUtility_StringToUInt32(pArgVector[8],&RoiStartX);
        ret |= AmbaUtility_StringToUInt32(pArgVector[9],&RoiStartY);
        ret |= AmbaUtility_StringToUInt32(pArgVector[10],&RoiWidth);
        ret |= AmbaUtility_StringToUInt32(pArgVector[11],&RoiHeight);

        RefCV_AmbaSpuFex_UT(pArgVector[1], pArgVector[2], pArgVector[3], pArgVector[4], FusionOutput, Width, Height,
                            RoiStartX, RoiStartY, RoiWidth, RoiHeight);
    } else if ((AmbaUtility_StringCompare(pArgVector[0], "ambafex", 7U) == 0)  && (ArgCount == 11U)) {
        UINT32 Width, Height;
        UINT32 RoiStartX, RoiStartY, RoiWidth, RoiHeight;
        ret |= AmbaUtility_StringToUInt32(pArgVector[5],&Width);
        ret |= AmbaUtility_StringToUInt32(pArgVector[6],&Height);
        ret |= AmbaUtility_StringToUInt32(pArgVector[7],&RoiStartX);
        ret |= AmbaUtility_StringToUInt32(pArgVector[8],&RoiStartY);
        ret |= AmbaUtility_StringToUInt32(pArgVector[9],&RoiWidth);
        ret |= AmbaUtility_StringToUInt32(pArgVector[10],&RoiHeight);

        RefCV_AmbaFex_UT(pArgVector[1], pArgVector[2], pArgVector[3], pArgVector[4], Width, Height,
                         RoiStartX, RoiStartY, RoiWidth, RoiHeight);
    } else if ((AmbaUtility_StringCompare(pArgVector[0], "ambaspufusion", 13U) == 0)  && (ArgCount == 8U)) {
        UINT32 LayerNum;
        ret |= AmbaUtility_StringToUInt32(pArgVector[3], &LayerNum);
        if ( LayerNum == 2U ) {
            RefCV_AmbaSpuFusion_UT(pArgVector[1], pArgVector[2], LayerNum, pArgVector[4], pArgVector[5], NULL, pArgVector[7]);
        } else if ( LayerNum == 3U ) {
            RefCV_AmbaSpuFusion_UT(pArgVector[1], pArgVector[2], LayerNum, pArgVector[4], pArgVector[5], pArgVector[6], pArgVector[7]);
        } else {
            // Not supported
        }
    } else if ((AmbaUtility_StringCompare(pArgVector[0], "ambafma", 7U) == 0)  && (ArgCount == 5U)) {
        RefCV_AmbaFma_UT(pArgVector[1], pArgVector[2], pArgVector[3], pArgVector[4]);
#endif
#ifdef CONFIG_QNX
    } else if ((AmbaUtility_StringCompare(pArgVector[0], "ambavo", 6U) == 0)  && (ArgCount == 1U)) {
        RefCV_AmbaVO_UT();
    } else if ((AmbaUtility_StringCompare(pArgVector[0], "ambamvac", 8U) == 0)  && (ArgCount == 1U)) {
        RefCV_AmbaMVAC_UT();
    } else if ((AmbaUtility_StringCompare(pArgVector[0], "ambaac", 6U) == 0)  && (ArgCount == 1U)) {
        RefCV_AmbaAc_UT();
#endif
#endif
#ifdef CONFIG_SOC_CV2FS
    } else if ((AmbaUtility_StringCompare(pArgVector[0], "ambafex", 7U) == 0)  && (ArgCount == 5U)) {
        RefCV_AmbaFex_UT(pArgVector[1], pArgVector[2], pArgVector[3], pArgVector[4]);
    } else if ((AmbaUtility_StringCompare(pArgVector[0], "ambaspufex", 10U) == 0)  && (ArgCount == 6U)) {
        RefCV_AmbaSpuFex_UT(pArgVector[1], pArgVector[2], pArgVector[3], pArgVector[4], pArgVector[5]);
    } else if ((AmbaUtility_StringCompare(pArgVector[0], "ambaoffex", 9U) == 0)  && (ArgCount == 6U)) {
        RefCV_AmbaOfFex_UT(pArgVector[1], pArgVector[2], pArgVector[3], pArgVector[4], pArgVector[5]);
    } else if ((AmbaUtility_StringCompare(pArgVector[0], "ambaspufusion", 13U) == 0)  && (ArgCount == 8U)) {
        UINT32 LayerNum;
        ret |= AmbaUtility_StringToUInt32(pArgVector[3], &LayerNum);
        if ( LayerNum == 2U ) {
            RefCV_AmbaSpuFusion_UT(pArgVector[1], pArgVector[2], LayerNum, pArgVector[4], pArgVector[5], NULL, pArgVector[7]);
        } else if ( LayerNum == 3U ) {
            RefCV_AmbaSpuFusion_UT(pArgVector[1], pArgVector[2], LayerNum, pArgVector[4], pArgVector[5], pArgVector[6], pArgVector[7]);
        } else {
            // Not supported
        }
#endif
    } else if ((AmbaUtility_StringCompare(pArgVector[0], "acc", 3U) == 0) && (ArgCount == 1U)) {
        RefCV_AccUT(0U, 1U);
    } else if ((AmbaUtility_StringCompare(pArgVector[0], "acc", 3U) == 0) && (ArgCount == 2U)) {
        ret |= AmbaUtility_StringToUInt32(pArgVector[1],&ID);
        RefCV_AccUT(ID, 1U);
    } else if ((AmbaUtility_StringCompare(pArgVector[0], "acc", 3U) == 0) && (ArgCount == 3U)) {
        ret |= AmbaUtility_StringToUInt32(pArgVector[1],&ID);
        ret |= AmbaUtility_StringToUInt32(pArgVector[2],&loop);
        RefCV_AccUT(ID, loop);
    } else if ((AmbaUtility_StringCompare(pArgVector[0], "custom", 6U) == 0) && (ArgCount == 8U)) {
        ret |= AmbaUtility_StringToUInt32(pArgVector[4],&input_count);
        ret |= AmbaUtility_StringToUInt32(pArgVector[5],&output_count);
        ret |= AmbaUtility_StringToUInt32(pArgVector[6],&output_offset);
        ret |= AmbaUtility_StringToUInt32(pArgVector[7],&loop);
        RefCV_AccCustomUT(pArgVector[1], pArgVector[2], pArgVector[3], input_count, output_count, output_offset, loop);
#ifdef CONFIG_THREADX
    } else if ((AmbaUtility_StringCompare(pArgVector[0], "ambasoffex", 10U) == 0)) {
        RefCV_AmbaSofFex_UT();
#if defined(CONFIG_CV_FLEXIDAG_STIXEL)
    } else if ((AmbaUtility_StringCompare(pArgVector[0], "stixel", 6U) == 0)) {
        UINT32 RoadDetMode;
        ret |= AmbaUtility_StringToUInt32(pArgVector[3], &RoadDetMode);
        SCA_Stixel_UT(pArgVector[1], pArgVector[2], RoadDetMode);
#endif
#endif
    } else if ((AmbaUtility_StringCompare(pArgVector[0], "plain", 5U) == 0) && (ArgCount == 5U)) {
        ret |= AmbaUtility_StringToUInt32(pArgVector[4],&loop);
        SCA_Plain_UT(pArgVector[1], pArgVector[2], pArgVector[3], loop);
#if defined(CONFIG_BUILD_AMBA_ADAS_UNITTEST)
    } else if ((AmbaUtility_StringCompare(pArgVector[0], "ambaadas_fc", 11U) == 0) && (ArgCount == 10U)) {
        DOUBLE FL_mm;  // in mm
        UINT32 FL_um;
        UINT32 ProcStep;

        ret |= AmbaUtility_StringToUInt32(pArgVector[7],&FL_um);
        ret |= AmbaUtility_StringToUInt32(pArgVector[8],&ProcStep);
        FL_mm = (DOUBLE)FL_um/1000.0;
        SCA_ADAS_FC_UT(pArgVector[1], pArgVector[2], pArgVector[3], pArgVector[4], pArgVector[5], pArgVector[6], FL_mm, ProcStep, pArgVector[9]);
#endif
    } else {
        RefCV_TestHelp(PrintFunc);
    }

    (void) ret;
}
