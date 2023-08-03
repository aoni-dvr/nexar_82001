/**
*  @file CtShell_CommandCv.c
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
*   @details The cnn_testbed cv related shell commands
*
*/

#include "CtShell_CommandCv.h"

/* ArmUtil header */
#include "ArmLog.h"
#include "ArmStdC.h"
#include "ArmFIO.h"
#include "ArmErrCode.h"

/* Amba header */
#include "AmbaCache.h"
#include "cvapi_ambacv_flexidag.h"

//#define ARM_LOG_CV_CMD        "CvCmd"

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: CvCommCmdUsage
 *
 *  @Description:: The usage of CvComm test command
 *
 *  @Input      ::
 *    PrintFunc:   The pointer of print function
 *
 *  @Output     :: None
 *
 *  @Return     :: None
 *
\*-----------------------------------------------------------------------------------------------*/
static void CvCmdUsage(AMBA_SHELL_PRINT_f PrintFunc)
{
  PrintFunc("Cv test commands:\n");
  PrintFunc("    load [visorc binary dir]\n");
  PrintFunc("        Example: \"t cv load .\" to load orcvp.bin\n");
  PrintFunc("    vpstatus [OPTIONS]\n");
  PrintFunc("        -v arg[0|1]: vp instance [vp0|vp1]\n");
  PrintFunc("        -s arg[0|1]: summary of  [vorc|vp]\n");
  PrintFunc("        -n arg: name of the register to read\n");
  PrintFunc("        -d arg: file name of the debug registers\n");
  PrintFunc("    log_console [enb]\n");
  PrintFunc("        Set log output to console module\n");
  PrintFunc("        [enb]: 0 Disable log out CVTask and SCHDR module to console\n");
  PrintFunc("               1 Enable log out CVTask and SCHDR module to console\n");
  PrintFunc("    log_flush\n");
  PrintFunc("        Flush log from buffer\n");
  PrintFunc("    dmsg\n");
  PrintFunc("        Enable orcvp log\n");
  PrintFunc("        [mode]: 0x4 Enable orcvp scheduler log\n");
  PrintFunc("\n");
  PrintFunc("    sdg [...] \n");
  PrintFunc("        Superdag related test command\n");
  PrintFunc("    flexi [...] \n");
  PrintFunc("        Flexidag related test command\n");
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: CtShell_CommandCv
 *
 *  @Description:: The cnn_testbed cv test command
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
extern INT32 VPutilsTestEntry(INT32 argc, char **argv, AMBA_SHELL_PRINT_f PrintFunc);

void CtShell_CommandCv(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
  if (ArgCount < 2U) {
    CvCmdUsage(PrintFunc);
  } else {

    /* load orcvp.bin */
    if (((ArmStdC_strcmp(pArgVector[1], "load") == 0)) && (ArgCount == 3U)) {
      (void) AmbaCV_SchdrLoad(pArgVector[2]);
    /* dump vpstatus */
    } else if ((ArmStdC_strcmp(pArgVector[1], "vpstatus") == 0) && (ArgCount >= 2U)) {
      char *argv[32];
      for(UINT32 i = 0U; i < (ArgCount - 1U); i++) {  // cv vpstatus xxx -> vpstatus xxx
        argv[i] = pArgVector[i + 1U];
      }
      (void) VPutilsTestEntry((INT32)ArgCount - 1, argv, PrintFunc);
    /* print log on console */
    } else if ((ArmStdC_strcmp(pArgVector[1], "log_console") == 0) && (ArgCount == 3U)) {
      UINT32 Mode = 0U;
      (void) ArmStdC_strtoul(pArgVector[2],&Mode);

      if(Mode == 0x0U) {
        (void) AmbaPrint_ModuleSetAllowList(SCHDR_PRINT_MODULE_ID,  (UINT8)0U);
        (void) AmbaPrint_ModuleSetAllowList(CVTASK_PRINT_MODULE_ID, (UINT8)0U);
        (void) AmbaPrint_ModuleSetAllowList(CVAPP_PRINT_MODULE_ID,  (UINT8)0U);
      } else {
        (void) AmbaPrint_ModuleSetAllowList(SCHDR_PRINT_MODULE_ID,  (UINT8)1U);
        (void) AmbaPrint_ModuleSetAllowList(CVTASK_PRINT_MODULE_ID, (UINT8)1U);
        (void) AmbaPrint_ModuleSetAllowList(CVAPP_PRINT_MODULE_ID,  (UINT8)1U);
      }
    } else if ((ArmStdC_strcmp(pArgVector[1], "log_flush") == 0) && (ArgCount == 3U)) {
      AmbaPrint_StopAndFlush();

    } else if ((ArmStdC_strcmp(pArgVector[1], "dmsg") == 0) && (ArgCount == 3U)) {
      UINT32 Mode = 0U;
      (void) ArmStdC_strtoul(pArgVector[2],&Mode);
      if((Mode & 0x1U) != 0U) {
        (void)AmbaCV_SchdrDumpLog(FLEXIDAG_SCHDR_SOD_BIN, NULL, 0x7FFFFFFF);
      }
      if((Mode & 0x4U) != 0U) {
        (void)AmbaCV_SchdrDumpLog(FLEXIDAG_SCHDR_VP_BIN, NULL, 0x7FFFFFFF);
      }
    /* superdag */
    } else if ((ArmStdC_strcmp(pArgVector[1], "sdg") == 0) && (ArgCount >= 2U)) {
      CtShell_CommandCvSdg(ArgCount, pArgVector, PrintFunc);

    /* flexidag */
    } else if ((ArmStdC_strcmp(pArgVector[1], "flexi") == 0) && (ArgCount >= 2U)) {
      CtShell_CommandCvFlexi(ArgCount, pArgVector, PrintFunc);

    } else {
      CvCmdUsage(PrintFunc);
    }
  }
  return;
}

