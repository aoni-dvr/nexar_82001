/**
*  @file cv_msg_thread.c
*
* Copyright (c) [2021] Ambarella International LP
*
* This file and its contents ("Software") are protected by intellectual
* property rights including, without limitation, U.S. and/or foreign
* copyrights. This Software is also the confidential and proprietary
* information of Ambarella International LP and its licensors. You may not use, reproduce,
* disclose, distribute, modify, or otherwise prepare derivative works of this
* Software or any portion thereof except pursuant to a signed license agreement
* or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
* In the absence of such an agreement, you agree to promptly notify and return
* this Software to Ambarella International LP.
*
* This file includes sample code and is only for internal testing and evaluation.  If you
* distribute this sample code (whether in source, object, or binary code form), it will be
* without any warranty or indemnity protection from Ambarella International LP or its affiliates.
*
* THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
* MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
*   @details A thread to receive cv message
*
*/

#include <pthread.h>
#include <stdlib.h>

#include "ArmLog.h"
#include "cvapi_ambacv_flexidag.h"
#include "cv_msg_thread.h"

#define ARM_LOG_CVMSG      "CvMsg"

#if defined(CV_EXTERNAL_MSG_THREAD)
static pthread_t cv_msg_thread;
static volatile uint32_t g_cv_msg_loop = 1U;

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: cv_msg_thd
 *
 *  @Description:: The thread to handle schdr message
 *
 *  @Input      ::
 *      arg:       The thread arg
 *
 *  @Output     :: None
 *
 *  @Return     :: None
\*-----------------------------------------------------------------------------------------------*/
static void* cv_msg_thd(void *arg)
{
    uint32_t msg_num, i;
    uint32_t retcode;
    uint32_t msg_type, msg_retcode;
    AMBA_CV_FLEXIDAG_HANDLE_s *phandle;

    (void) (arg);
    while(g_cv_msg_loop == 1U) {
        retcode = AmbaCV_SchdrWaitMsg(&msg_num);
        if(retcode == ERRCODE_NONE) {
            for(i = 0U; i < msg_num; i++) {
                retcode = AmbaCV_SchdrProcessMsg(&phandle, &msg_type, &msg_retcode);
            }
        } else {
            ArmLog_ERR(ARM_LOG_CVMSG, "AmbaCV_SchdrWaitMsg() ret (0x%x)", retcode, 0U);
            break;
        }
    }

    return NULL;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: cv_msg_thread_create
 *
 *  @Description:: Create a thread to receive and handle cv message
 *
 *  @Input      :: None
 *
 *  @Output     :: None
 *
 *  @Return     :: OK(0)/NG(>0)
\*-----------------------------------------------------------------------------------------------*/
uint32_t cv_msg_thread_create(void)
{
    uint32_t ret = 0U;
    ArmLog_DBG(ARM_LOG_CVMSG, "AVF create schdr cmd thread", 0U, 0U);
    pthread_create(&cv_msg_thread, NULL, cv_msg_thd, NULL);
    pthread_setschedprio(cv_msg_thread, 90U);
    return ret;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: cv_msg_thread_delete
 *
 *  @Description:: Terminate cv message thread
 *
 *  @Input      :: None
 *
 *  @Output     :: None
 *
 *  @Return     :: OK(0)/NG(>0)
\*-----------------------------------------------------------------------------------------------*/
uint32_t cv_msg_thread_delete(void)
{
    uint32_t ret = 0U;
    g_cv_msg_loop = 0U;
    ArmLog_DBG(ARM_LOG_CVMSG, "cv_msg_thread terminated!", 0U, 0U);
    pthread_join(cv_msg_thread, NULL);
    return ret;
}

#else
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: cv_msg_thread_create
 *
 *  @Description:: Create a thread to receive and handle cv message
 *
 *  @Input      :: None
 *
 *  @Output     :: None
 *
 *  @Return     :: OK(0)/NG(>0)
\*-----------------------------------------------------------------------------------------------*/
uint32_t cv_msg_thread_create(void)
{
    uint32_t ret = 0U;
    ArmLog_DBG(ARM_LOG_CVMSG, "cvflow_comm config to use internal cmd thread", 0U, 0U);
    return ret;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: cv_msg_thread_delete
 *
 *  @Description:: Terminate cv message thread
 *
 *  @Input      :: None
 *
 *  @Output     :: None
 *
 *  @Return     :: OK(0)/NG(>0)
\*-----------------------------------------------------------------------------------------------*/
uint32_t cv_msg_thread_delete(void)
{
    uint32_t ret = 0U;
    return ret;
}

#endif
