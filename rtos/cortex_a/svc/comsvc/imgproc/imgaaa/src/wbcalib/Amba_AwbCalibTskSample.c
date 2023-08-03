/**
 * @file Amba_AwbCalibTskSample.c
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
 *  @Description    :: Awb Task Control Function
 *
 */



#include "AmbaUtility.h"
#include "AmbaWrap.h"
#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaPrint.h"
#include "AmbaImg_Proc.h"
//#include "Amba_AwbTskSample.h"
#include "Amba_AeAwbAdj_Control.h"
#include "Amba_AwbCalib.h"
#include "Amba_AwbCalibTskSample.h"
#include "AmbaImg_External_CtrlFunc.h"

typedef union /*_AMBA_IMG_AWB_CALIB_MEM_u_*/ {
    ULONG  Data;
    void   *pVoid;
} AMBA_IMG_AWB_CALIB_MEM_u;

typedef struct /*_AMBA_IMG_AWB_CALIB_MEM_s_*/ {
    AMBA_IMG_AWB_CALIB_MEM_u    Ctx;
} AMBA_IMG_AWB_CALIB_MEM_s;

typedef struct {
    AMBA_KAL_EVENT_FLAG_t   Flag;
    char                    FlagName[30];
    INT32                   Running;                    /**< Runing state of thread */
    AMBA_KAL_TASK_t         Task;
    char                    TaskName[30];
} AMBA_IMG_AWB_CALIB_TSK_CTRL_s;

static AMBA_IMG_AWB_CALIB_TSK_CTRL_s AmbaImgAwbCalibCtrl[MAX_VIEW_NUM] __attribute__((section(".bss.noinit")));



void AmbaImg_AwbCalibTaskDispatch(UINT32 viewId)
{
    (void)AmbaKAL_EventFlagSet(&AmbaImgAwbCalibCtrl[viewId].Flag, 1);
}

static void AmbaImg_AwbCalibTask(UINT32 Param)
{
    UINT32 rVal = 0;
    UINT32 Data = 0;
    UINT32 viewId = Param; //In this example, param is viewId. But you can custimize what you want to do in this task.

    while (AmbaImgAwbCalibCtrl[Param].Running != 0) {
        /* Wait for event from data queue */
        rVal |= AmbaKAL_EventFlagGet(&AmbaImgAwbCalibCtrl[Param].Flag, 1, 0U, 0U, &Data, AMBA_KAL_WAIT_FOREVER);
        (void)ImgProc_WbCalibCtrl(viewId);
        rVal |= AmbaKAL_EventFlagClear(&AmbaImgAwbCalibCtrl[Param].Flag, 1);

        if(rVal != 0U){
            AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "check AmbaImg_AwbCalibTask", 0U, 0U, 0U, 0U, 0U);
            AmbaPrint_Flush();
        }
    }
}

static void *AmbaImg_AwbCalibTaskEx(void *pParam)
{
    AMBA_IMG_AWB_CALIB_MEM_s MemInfo;
    MemInfo.Ctx.pVoid = pParam;
    AmbaImg_AwbCalibTask((UINT32) MemInfo.Ctx.Data);
    return NULL;
}

void AmbaImg_AwbCalibTask_Init(UINT32 viewCount)
{
    UINT32                          i;
    UINT32                          ErrMSG;
    static UINT8                    AwbCalibStack[MAX_VIEW_NUM][AMBA_IMG_AWB_TSK_STACK_SIZE] __attribute__((section(".bss.noinit")));
    static AMBA_IMG_TSK_Info_s      AmbaImgAwbCalibTskInfo[MAX_VIEW_NUM] __attribute__((section(".bss.noinit")));
    static AMBA_IMG_TSK_Info_s      AwbCalibTskInfo = {AMBA_IMG_AWB_TSK_PRIORITY, AMBA_IMG_AWB_TSK_STACK_SIZE, AMBA_IMG_AWB_TSK_CORE_BIT_MAP, 0};
    UINT32                          U32RVal = 0U;

    for (i = 0;i < viewCount;i++) {
        U32RVal |= AmbaWrap_memcpy(&AmbaImgAwbCalibTskInfo[i], &AwbCalibTskInfo, sizeof(AMBA_IMG_TSK_Info_s));
    }
    if(U32RVal != 0U){
        /* MisraC */
    }
    for (i = 0;i < viewCount; i++){
        AmbaUtility_StringCopy(AmbaImgAwbCalibCtrl[i].FlagName, 30U, "AmbaImgAwbCalibEventFlag#");
        AmbaUtility_StringAppendUInt32(AmbaImgAwbCalibCtrl[i].FlagName, 30U, i, 10U);
        ErrMSG = AmbaKAL_EventFlagCreate(&AmbaImgAwbCalibCtrl[i].Flag, AmbaImgAwbCalibCtrl[i].FlagName);
        if (ErrMSG != 0U) {
            AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "AmbaImg_AwbCalibTask_Init ERR:   %d", ErrMSG, 0U, 0U, 0U, 0U);
            AmbaPrint_Flush();
            break;
        }
        AmbaUtility_StringCopy(AmbaImgAwbCalibCtrl[i].TaskName, 30U, "AmbaImgAwbCalibTask#");
        AmbaUtility_StringAppendUInt32(AmbaImgAwbCalibCtrl[i].TaskName, 30U, i, 10U);

        AmbaImgAwbCalibCtrl[i].Running = 1;
        /* stetup task */
        {
            AMBA_IMG_AWB_CALIB_MEM_s MemInfo;
            MemInfo.Ctx.Data = i;
            (void)AmbaKAL_TaskCreate(&AmbaImgAwbCalibCtrl[i].Task,          /* pTask */
                                     AmbaImgAwbCalibCtrl[i].TaskName,       /* pTaskName */
                                     AmbaImgAwbCalibTskInfo[i].Priority,    /* Priority */
                                     AmbaImg_AwbCalibTaskEx,                /* void *(*EntryFunction)(void *) */
                                     MemInfo.Ctx.pVoid,                     /* EntryArg */
                                     AwbCalibStack[i],                      /* pStackBase */
                                     AmbaImgAwbCalibTskInfo[i].StackSize,   /* StackByteSize */
                                     0U                                     /* AutoStart */
                                    );
        }
        (void)AmbaKAL_TaskSetSmpAffinity(&AmbaImgAwbCalibCtrl[i].Task, AmbaImgAwbCalibTskInfo[i].CoreExclusiveBitMap);
        (void)AmbaKAL_TaskResume(&AmbaImgAwbCalibCtrl[i].Task);

#if defined(CONFIG_QNX) || defined(CONFIG_LINUX)
#else
        AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "<%s>  %s ",  __func__, AmbaImgAwbCalibCtrl[i].Task.tx_thread_name, NULL, NULL, NULL);
#endif
        AmbaPrint_Flush();
    }
}
