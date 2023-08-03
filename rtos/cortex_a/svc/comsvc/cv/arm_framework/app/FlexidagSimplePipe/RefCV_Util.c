/**
 *  @file cvapi_flexidag_ref_util.h
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
 *  @details Implementation of Cv flexidag util
 *
 */


#include "AmbaTypes.h"
#include "cvapi_memio_interface.h"
#include "cvapi_ambacv_flexidag.h"
#include "cvapi_flexidag_ref_util.h"
#include <AmbaPrint.h>
#include <AmbaMisraFix.h>
#include <AmbaWrap.h>
#include "AmbaMMU.h"
#include "AmbaCache.h"

#define DBG_LOG     AmbaPrint_PrintUInt5
#define CRI_LOG     AmbaPrint_PrintUInt5

static UINT32 S_StoreTokenIO(REF_FD_HANDLE_s* Handler, UINT32 Token, const AMBA_CV_FLEXIDAG_IO_s *In, const AMBA_CV_FLEXIDAG_IO_s *Out)
{
    UINT32 Ret = 0U;
    UINT32 i;

    if(AmbaKAL_MutexTake(&Handler->IOQueueMutex, AMBA_KAL_WAIT_FOREVER) != 0U) {
        AmbaPrint_PrintUInt5("S_StoreTokenIO : AmbaKAL_MutexTake fail", 0U, 0U, 0U, 0U, 0U);
    }
    for (i=0 ; i<MAX_DEPTH ; i++) {
        if (Handler->IOQueue[i].Used == 0U) {
            break;
        }
    }

    if (i == MAX_DEPTH) {
        Ret = 1U;
    } else {
        Handler->IOQueue[i].Used = 1U;
        Handler->IOQueue[i].Token = Token;
        if(AmbaWrap_memcpy(&Handler->IOQueue[i].Input, In, sizeof(AMBA_CV_FLEXIDAG_IO_s)) != 0U) {
            AmbaPrint_PrintUInt5("S_StoreTokenIO : AmbaWrap_memcpy fail", 0U, 0U, 0U, 0U, 0U);
        }

        if(AmbaWrap_memcpy(&Handler->IOQueue[i].Output, Out, sizeof(AMBA_CV_FLEXIDAG_IO_s)) != 0U) {
            AmbaPrint_PrintUInt5("S_StoreTokenIO : AmbaWrap_memcpy fail", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if(AmbaKAL_MutexGive(&Handler->IOQueueMutex) != 0U) {
        AmbaPrint_PrintUInt5("S_StoreTokenIO : AmbaKAL_MutexGive fail", 0U, 0U, 0U, 0U, 0U);
    }
    return Ret;
}

static UINT32 S_PopTokenIO(REF_FD_HANDLE_s* Handler, UINT32 Token, AMBA_CV_FLEXIDAG_IO_s *In, AMBA_CV_FLEXIDAG_IO_s *Out)
{
    UINT32 Ret = 0U;
    UINT32 i;

    if(AmbaKAL_MutexTake(&Handler->IOQueueMutex, AMBA_KAL_WAIT_FOREVER) != 0U) {
        AmbaPrint_PrintUInt5("S_PopTokenIO : AmbaKAL_MutexTake fail", 0U, 0U, 0U, 0U, 0U);
    }
    for (i=0 ; i<MAX_DEPTH ; i++) {
        if (Handler->IOQueue[i].Token == Token) {
            break;
        }
    }

    if (i == MAX_DEPTH) {
        Ret = 1U;
    } else {
        if(AmbaWrap_memcpy(In, &Handler->IOQueue[i].Input, sizeof(AMBA_CV_FLEXIDAG_IO_s)) != 0U) {
            AmbaPrint_PrintUInt5("S_PopTokenIO : AmbaWrap_memcpy fail", 0U, 0U, 0U, 0U, 0U);
        }
        if(AmbaWrap_memcpy(Out, &Handler->IOQueue[i].Output, sizeof(AMBA_CV_FLEXIDAG_IO_s)) != 0U) {
            AmbaPrint_PrintUInt5("S_PopTokenIO : AmbaWrap_memcpy fail", 0U, 0U, 0U, 0U, 0U);
        }
        Handler->IOQueue[i].Token = 0U;
        Handler->IOQueue[i].Used = 0U;
    }
    if(AmbaKAL_MutexGive(&Handler->IOQueueMutex) != 0U) {
        AmbaPrint_PrintUInt5("S_PopTokenIO : AmbaKAL_MutexGive fail", 0U, 0U, 0U, 0U, 0U);
    }
    return Ret;
}


UINT32 RefCVUtil_FlexiDagProc(REF_FD_HANDLE_s* Handler, void** In, const UINT32* InSz, UINT32 InNum, void **Out, UINT32 OutNum)
{
    ULONG addr, Paddr = 0UL;
    UINT32 ret, i,Ret1 = 0U;
    const void* vp;
    AMBA_CV_FLEXIDAG_HANDLE_s *FdGenHdlr;
    UINT32 Token;
    AMBA_CV_FLEXIDAG_IO_s InBuf,OutBuf;

    (void) In;
    (void) Out;
    AmbaMisra_TouchUnused(In);
    AmbaMisra_TouchUnused(Out);
    FdGenHdlr = &Handler->FDGenHandle;

    InBuf.num_of_buf = InNum;
    for (i=0; i<InNum ; i++) {
        InBuf.buf[i].buffer_cacheable = 1;
        //Hdlr->InBuf.buf[0].pBuffer = In;
        vp = In[i];
        AmbaMisra_TypeCast(&InBuf.buf[i].pBuffer, &vp);
        InBuf.buf[i].buffer_size = InSz[i];
        AmbaMisra_TypeCast(&addr, &vp);
        Ret1 |= AmbaMMU_VirtToPhys(addr, &Paddr);
        InBuf.buf[i].buffer_daddr = Paddr;
    }
    OutBuf.num_of_buf = OutNum;
    for (i=0; i<OutNum ; i++) {
        OutBuf.buf[i].buffer_cacheable = 1;
        //Hdlr->OutBuf.buf[0].pBuffer = Out;
        vp = Out[i];
        AmbaMisra_TypeCast(&OutBuf.buf[i].pBuffer, &vp);
        OutBuf.buf[i].buffer_size = Handler->FDGenHandle.mem_req.flexidag_output_buffer_size[i];
        AmbaMisra_TypeCast(&addr, &vp);
        Ret1 |=  AmbaMMU_VirtToPhys(addr, &Paddr);
        OutBuf.buf[i].buffer_daddr = Paddr;
    }
    AmbaMisra_TypeCast(&addr, &InBuf.buf[0].pBuffer);
    ret = AmbaCache_DataClean(addr, InBuf.buf[0].buffer_size);
    if(ret != 0U) {
        CRI_LOG("RefCVUtil_FlexiDagProc : AmbaCache_DataClean fail", 0U, 0U, 0U, 0U, 0U);
        CRI_LOG("RefCVUtil_FlexiDagProc : %x %d", (UINT32)addr, InBuf.buf[0].buffer_size, 0U, 0U, 0U);
        ret = 1U;
    }
    if (ret == 0U) {
        //ret = AmbaCV_FlexidagRun(FdGenHdlr, &Hdlr->InBuf, &Hdlr->OutBuf, &RunInfo);
        ret = AmbaCV_FlexidagRunNonBlock(FdGenHdlr, NULL, NULL, &InBuf, &OutBuf, &Token);
        if (ret != 0U) {
            AmbaPrint_PrintStr5("RefCVUtil_FlexiDagProc : AmbaCV_FlexidagRun fail ", NULL, NULL, NULL, NULL, NULL);
            ret = 1U;
        }
        if (ret == 0U) {
            // store io and token
            UINT32 Msg[1];

            Ret1 |= S_StoreTokenIO(Handler, Token, &InBuf, &OutBuf);

            Msg[0] = Token;

            ret = AmbaKAL_MsgQueueSend(&Handler->MqueueToRslt, Msg, 100);
            if (ret != 0U) {
                // send failed
                CRI_LOG("AmbaKAL_MsgQueueSend failed",
                        0U, 0U, 0U, 0U, 0U);
            }
        }
    }
    (void) Ret1;
    return ret;
}

UINT32 RefCVUtil_FlexiDagOpen(const UINT8* FlexiDagBinBuf, UINT32 FlexiDagBinSz, UINT32* StateBufSz, UINT32* TempBufSz, UINT32* OutputBufNum, UINT32 *OutputBufSz, REF_FD_HANDLE_s* Handler)
{
    UINT32 ret,ret2 = 0U;
    ULONG addr, Paddr = 0UL;
    UINT32 i;

    if(AmbaWrap_memset(Handler, 0x0, sizeof(REF_FD_HANDLE_s)) != 0U) {
        AmbaPrint_PrintUInt5("[ERROR] RefCVUtil_FlexiDagOpen AmbaWrap_memset fail  ",0U, 0U, 0U, 0U, 0U);
    }
    Handler->FlexidagBinBuf.buffer_cacheable = 1;
    //Handler->FlexidagBinBuf.pBuffer = FlexiDagBinBuf;
    AmbaMisra_TypeCast(&(Handler->FlexidagBinBuf.pBuffer), &FlexiDagBinBuf);
    Handler->FlexidagBinBuf.buffer_size = FlexiDagBinSz;
    AmbaMisra_TypeCast(&addr, &Handler->FlexidagBinBuf);
    ret2 = AmbaMMU_VirtToPhys(addr, &Paddr);
    Handler->FlexidagBinBuf.buffer_daddr = Paddr;


    // Open
    ret = AmbaCV_FlexidagOpen(&Handler->FlexidagBinBuf, &Handler->FDGenHandle);
    if (ret!=0U) {
        CRI_LOG(" AmbaCV_FlexidagOpen failed:%x", ret, 0U, 0U, 0U, 0U);
    } else {
        DBG_LOG("state_buf size:%d, temp_buf size:%d", Handler->FDGenHandle.mem_req.flexidag_state_buffer_size, Handler->FDGenHandle.mem_req.flexidag_temp_buffer_size, 0U, 0U, 0U);
        *StateBufSz = Handler->FDGenHandle.mem_req.flexidag_state_buffer_size;
        *TempBufSz = Handler->FDGenHandle.mem_req.flexidag_temp_buffer_size;
        *OutputBufNum = Handler->FDGenHandle.mem_req.flexidag_num_outputs;
        Handler->InitData.state_buf.buffer_size = Handler->FDGenHandle.mem_req.flexidag_state_buffer_size;
        (void) Handler->InitData.state_buf.buffer_size;
        Handler->InitData.temp_buf.buffer_size = Handler->FDGenHandle.mem_req.flexidag_temp_buffer_size;
        (void) Handler->InitData.temp_buf.buffer_size;
        for (i = 0U; i< *OutputBufNum ; i++) {
            OutputBufSz[i] = Handler->FDGenHandle.mem_req.flexidag_output_buffer_size[i];
        }
    }
    if((ret != 0U) || (ret2 != 0U)) {
        AmbaPrint_PrintUInt5("[ERROR] RefCVUtil_FlexiDagOpen fail 0x%x 0x%x ",ret, ret2, 0U, 0U, 0U);
    }
    return ret;
}


UINT32 RefCVUtil_FlexiDagInit(REF_FD_HANDLE_s* Handler, UINT8* StateBuf, UINT8* TempBuf)
{
    UINT32 ret = 0U,ret1 = 0U;
    ULONG addr, Paddr = 0UL;
    static char MQName[] = "FDMQ";
    static char MutexName[] = "FDMUTEX";

    // it's to fix misra c parser tool iuues that we convert the pointer to other type by typecast...
    (void) StateBuf;
    (void) TempBuf;
    Handler->InitData.state_buf.buffer_cacheable = 1;
    //Handler->InitData.state_buf.pBuffer = StateBuf;
    AmbaMisra_TypeCast(&Handler->InitData.state_buf.pBuffer, &StateBuf);
    AmbaMisra_TypeCast(&addr, &StateBuf);
    ret1 |=  AmbaMMU_VirtToPhys(addr, &Paddr);
    Handler->InitData.state_buf.buffer_daddr = Paddr;

    Handler->InitData.temp_buf.buffer_cacheable = 1;
    //Handler->InitData.temp_buf.pBuffer = TempBuf;
    AmbaMisra_TypeCast(&Handler->InitData.temp_buf.pBuffer, &TempBuf);
    AmbaMisra_TypeCast(&addr, &TempBuf);
    ret1 |=  AmbaMMU_VirtToPhys(addr, &Paddr);
    Handler->InitData.temp_buf.buffer_daddr = Paddr;

    // Init
    ret = AmbaCV_FlexidagInit(&Handler->FDGenHandle, &Handler->InitData);
    if (ret!=0U) {
        DBG_LOG(" AmbaCV_FlexidagInit failed:%x", ret, 0U, 0U, 0U, 0U);
    }
    if (ret == 0U) {
        ret = AmbaKAL_MsgQueueCreate(&(Handler->MqueueToRslt),
                                     MQName,
                                     (uint32_t)sizeof(UINT32),
                                     Handler->IOTokenMsgSpace,
                                     (uint32_t)sizeof(Handler->IOTokenMsgSpace));
        if (ret!= 0U) {
            CRI_LOG(" AmbaKAL_MsgQueueCreate failed", 0U, 0U, 0U, 0U, 0U);
        } else {
            ret = AmbaKAL_MutexCreate(&(Handler->IOQueueMutex), MutexName);
            if (ret!= 0U) {
                CRI_LOG(" AmbaKAL_MutexCreate failed", 0U, 0U, 0U, 0U, 0U);
            }
        }
    }
    Handler->State = 1U;
    (void) ret1;
    return ret;
}

UINT32 RefCVUtil_FlexiDagGetResult(REF_FD_HANDLE_s* Handler, AMBA_CV_FLEXIDAG_IO_s* In, AMBA_CV_FLEXIDAG_IO_s* Out)
{
    UINT32 Rval,ret1 = 0U;
    UINT32 Msg[1];
    AMBA_CV_FLEXIDAG_RUN_INFO_s RunInfo;

    while(Handler->State != 1U) {
        ret1 |=  AmbaKAL_TaskSleep(2);
        AmbaMisra_TouchUnused(&Handler->State);
    }

    Rval = AmbaKAL_MsgQueueReceive(&(Handler->MqueueToRslt), Msg, AMBA_KAL_WAIT_FOREVER);
    if (Rval != 0U) {
        CRI_LOG("GetResult AmbaKAL_MsgQueueReceive ERR_CODE:%d", Rval, 0U, 0U, 0U, 0U);
    }

    ret1 |=  AmbaCV_FlexidagWaitRunFinish(&Handler->FDGenHandle, Msg[0], &RunInfo);
    Rval = S_PopTokenIO(Handler, Msg[0], In, Out);
    if (Rval != 0U) {
        CRI_LOG("GetResult PopIO node failed:%d", Rval, 0U, 0U, 0U, 0U);
    }
    (void) ret1;
    return Rval;
}

/*
static void _ProcCVPicInfo(const memio_source_recv_picinfo_t *pDataIn, memio_source_recv_picinfo_t *pDataOut)
{
    ULONG U32DataOut;
    ULONG U32PDataOut;
    UINT32 i;
    (void)AmbaWrap_memcpy(pDataOut, pDataIn, sizeof(cv_pic_info_t));

    AmbaMisra_TypeCast(&U32DataOut, &pDataOut);
    (void)AmbaMMU_VirtToPhys(U32DataOut, &U32PDataOut);
    for (i = 0U ; i < MAX_HALF_OCTAVES ; i++) {
        pDataOut->pic_info.rpLumaLeft[i] = pDataIn->pic_info.rpLumaLeft[i] - U32PDataOut;
        pDataOut->pic_info.rpLumaRight[i] = pDataIn->pic_info.rpLumaRight[i] - U32PDataOut;
        pDataOut->pic_info.rpChromaLeft[i] = pDataIn->pic_info.rpChromaLeft[i] - U32PDataOut;
        pDataOut->pic_info.rpChromaRight[i] = pDataIn->pic_info.rpChromaRight[i] - U32PDataOut;
    }
}
*/

