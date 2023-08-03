/**
*  @file CtShell_CommandCvSdg.c
*
* Copyright (c) [2020] Ambarella International LP
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
*   @details The CvComm related shell commands
*
*/

#include "CtShell_CommandCv.h"

/* ArmUtil header */
#include "ArmLog.h"
#include "ArmStdC.h"

#define ARM_LOG_CV_SDG_CMD        "CvSdgCmd"


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: CvSdgCmdUsage
 *
 *  @Description:: The usage of cv superdag test command
 *
 *  @Input      ::
 *    PrintFunc:   The pointer of print function
 *
 *  @Output     :: None
 *
 *  @Return     :: None
 *
\*-----------------------------------------------------------------------------------------------*/
static void CvSdgCmdUsage(AMBA_SHELL_PRINT_f PrintFunc)
{
  PrintFunc("CvSdg test commands:\n");
  PrintFunc("    run \n");
  PrintFunc("    run_multi \n");
  PrintFunc("    dmsg \n");
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: CtShell_CommandCvSdg
 *
 *  @Description:: The esc CvComm test command
 *
 *  @Input      ::
 *    ArgCount:    The arg number
 *    pArgVector:  The pointer of arg
 *    PrintFunc:   The pointer of print function
 *
 *  @Output     :: None
 *
 *  @Return     :: None
 *
\*-----------------------------------------------------------------------------------------------*/
void CtShell_CommandCvSdg(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
  if (ArgCount < 3U) {    // pArgVector[0] = 'cv', pArgVector[1] = 'sdg'
    CvSdgCmdUsage(PrintFunc);
  } else {

    /* run */
    if ((ArmStdC_strcmp(pArgVector[2], "run") == 0)) {
      ArmLog_DBG(ARM_LOG_CV_SDG_CMD, "Not implement yet.", 0U, 0U);
//      CtShell_CommandCvCommFlexi(ArgCount, pArgVector, PrintFunc);


    } else {
      CvSdgCmdUsage(PrintFunc);
    }
  }
  return;
}

