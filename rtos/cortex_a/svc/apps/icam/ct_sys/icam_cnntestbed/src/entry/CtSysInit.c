/**
*  @file CtSysInit.c
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
*   @details The init function of cnn_testbed
*
*/

#include "CtShell_CommandCv.h"
#include "CtSysInit.h"

/* ArmUtil header */
#include "ArmStdC.h"
#include "ArmErrCode.h"

#include "SvcFlowControl.h"
#include "SvcCmd.h"
#include "AmbaSYS.h"
#include "SvcLog.h"

#ifdef CONFIG_ENABLE_AMBALINK
#include "AmbaLink.h"
#endif

#if defined(CONFIG_QNX)
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#endif

#if defined(CONFIG_THREADX)
#include "AmbaFDT.h"
#endif

#define SVC_LOG_CT_INIT        "CtInit"

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: DumpCvMemLayout
 *
 *  @Description:: Dump cv memory layout
 *
 *  @Input      :: None
 *
 *  @Output     :: None
 *
 *  @Return     :: None
 *
\*-----------------------------------------------------------------------------------------------*/
static void DumpCvMemLayout(void)
{
#if defined(CONFIG_THREADX) && !defined(CONFIG_THREADX64)
  const void *Fdt;
  ULONG DtbAddr;
  INT32 Rval, Offset, Len;
  const char *pChr;
  const UINT32 *pData;
  const struct fdt_property *pProp = NULL;

  ULONG CvAddr_s = 0U, CvAddr_e = 0U, CvSchAddr_s = 0U, CvSchAddr_e = 0U;
  ULONG CvLnxAddr_s = 0U, CvLnxAddr_e = 0U, CvTXAddr_s = 0U, CvTXAddr_e = 0U;
  UINT32 CvSize = 0U, CvSchSize = 0U, CvLnxSize = 0U, CvTXSize = 0U;

  DtbAddr = (ULONG)CONFIG_DTB_LOADADDR;
  AmbaMisra_TypeCast(&Fdt, &DtbAddr);
  Rval = AmbaFDT_CheckHeader(Fdt);
  if (Rval == 0) {
    Offset = AmbaFDT_PathOffset(Fdt, "scheduler");
    if (Offset >= 0) {
      pProp = AmbaFDT_GetProperty(Fdt, Offset, "cv_att_pa", &Len);
      if ((pProp != NULL) && (Len > 2)) {
        pChr = (const char *)&pProp->data[0];
        AmbaMisra_TypeCast(&pData, &pChr);
        CvAddr_s = (ULONG)AmbaFDT_Fdt32ToCpu(pData[0]);
        CvSchAddr_s = CvAddr_s;
      }

      pProp = AmbaFDT_GetProperty(Fdt, Offset, "cv_att_size", &Len);
      if ((pProp != NULL) && (Len > 2)) {
        pChr = (const char *)&pProp->data[0];
        AmbaMisra_TypeCast(&pData, &pChr);
        CvSize = AmbaFDT_Fdt32ToCpu(pData[0]);
        CvAddr_e = CvAddr_s + CvSize;
      }

      pProp = AmbaFDT_GetProperty(Fdt, Offset, "cv_schdr_size", &Len);
      if ((pProp != NULL) && (Len > 2)) {
        pChr = (const char *)&pProp->data[0];
        AmbaMisra_TypeCast(&pData, &pChr);
        CvSchSize = AmbaFDT_Fdt32ToCpu(pData[0]);
        CvSchAddr_e = CvSchAddr_s + CvSchSize;
      }
    }

    Offset = AmbaFDT_PathOffset(Fdt, "flexidag_sys");
    if (Offset >= 0) {
      pProp = AmbaFDT_GetProperty(Fdt, Offset, "reg", &Len);
      if ((pProp != NULL) && (Len > 3)) {
        pChr = (const char *)&pProp->data[0];
        AmbaMisra_TypeCast(&pData, &pChr);
        CvLnxAddr_s = CvSchAddr_e;
        CvLnxSize = AmbaFDT_Fdt32ToCpu(pData[1]) - CvSchSize;
        CvLnxAddr_e = CvLnxAddr_s + CvLnxSize;
      }
    }

    Offset = AmbaFDT_PathOffset(Fdt, "flexidag_rtos");
    if (Offset >= 0) {
      pProp = AmbaFDT_GetProperty(Fdt, Offset, "reg", &Len);
      if ((pProp != NULL) && (Len > 3)) {
        pChr = (const char *)&pProp->data[0];
        AmbaMisra_TypeCast(&pData, &pChr);
        CvTXAddr_s = (ULONG)AmbaFDT_Fdt32ToCpu(pData[0]);
        CvTXSize = AmbaFDT_Fdt32ToCpu(pData[1]);
        CvTXAddr_e = CvTXAddr_s + CvTXSize;
      }
    }

    if (CvTXSize == 0U) {
      // no cv linux area
      CvTXAddr_s = CvLnxAddr_s;
      CvTXAddr_e = CvLnxAddr_e;
      CvTXSize = CvLnxSize;
      CvLnxSize = 0U;
    }

    SvcLog_DBG(SVC_LOG_CT_INIT, "[CV]", 0U, 0U);
    SvcLog_DBG(SVC_LOG_CT_INIT, "  - Total    : 0x%08X ~ 0x%08X", CvAddr_s, CvAddr_e);
    SvcLog_DBG(SVC_LOG_CT_INIT, "  - schdr    : 0x%08X ~ 0x%08X", CvSchAddr_s, CvSchAddr_e);
    if (CvLnxSize != 0U) {
      SvcLog_DBG(SVC_LOG_CT_INIT, "  - linux    : 0x%08X ~ 0x%08X", CvLnxAddr_s, CvLnxAddr_e);
    }
    SvcLog_DBG(SVC_LOG_CT_INIT, "  - threadX  : 0x%08X ~ 0x%08X", CvTXAddr_s, CvTXAddr_e);

    SvcLog_DBG(SVC_LOG_CT_INIT, "==================================================", 0U, 0U);
    SvcLog_DBG(SVC_LOG_CT_INIT, "<---------------- cv memory --------------------->", 0U, 0U);
    SvcLog_DBG(SVC_LOG_CT_INIT, "                  %d MB", CvSize>>20U, 0U);
    if (CvLnxSize != 0U) {
      SvcLog_DBG(SVC_LOG_CT_INIT, "<- schdr -><------ linux ------> ", 0U, 0U);
      SvcLog_DBG(SVC_LOG_CT_INIT, "   %d MB           %d MB", CvSchSize>>20U, CvLnxSize>> 20U);
      SvcLog_DBG(SVC_LOG_CT_INIT, "                                 <--- threadX --->", 0U, 0U);
      SvcLog_DBG(SVC_LOG_CT_INIT, "                                      %d MB", CvTXSize>>20U, 0U);
    } else {
      SvcLog_DBG(SVC_LOG_CT_INIT, "<----- schdr -----><---------- threadX ----------> ", 0U, 0U);
      SvcLog_DBG(SVC_LOG_CT_INIT, "       %d MB                   %d MB", CvSchSize>>20U, CvTXSize>>20U);
    }
    SvcLog_DBG(SVC_LOG_CT_INIT, "==================================================", 0U, 0U);
  }
#elif defined(CONFIG_THREADX) && defined(CONFIG_THREADX64)
  SvcLog_DBG(SVC_LOG_CT_INIT, "DumpCvMemLayout(): Dummy function!", 0U, 0U);
#elif defined(CONFIG_QNX)
  UINT32 CvAddr_s = 0, CvAddr_e = 0, CvRtosAddr_s = 0, CvRtosAddr_e = 0;
  UINT32 CvSysAddr_s = 0, CvSysAddr_e = 0, CvSchdr_s = 0, CvSchdr_e = 0;
  UINT32 CvSchSize = CONFIG_CV_MEM_SCHDR_SIZE;
  UINT32 CvRtosSize = (CONFIG_CV_MEM_SIZE - CONFIG_CV_MEM_SCHDR_SIZE) - CONFIG_ICAM_CV_USER_RESERVED;

  INT32 MapFd = -1, Rval = -1;
  void *pVirAddr = NULL;
  off_t PhyAddr;
  struct posix_typed_mem_info CvMemInfo;

  /* OPEN CV memory area */
  MapFd = posix_typed_mem_open("/ram/flexidag_sys", O_RDWR, POSIX_TYPED_MEM_ALLOCATE_CONTIG);
  if (MapFd >= 0) {
    (void) posix_typed_mem_get_info(MapFd, &CvMemInfo);
    pVirAddr = mmap(NULL, 1<<10, PROT_READ | PROT_WRITE, MAP_SHARED, MapFd, 0); // Map 1KB to process space
    (void) mem_offset(pVirAddr, NOFD, 1, &PhyAddr, 0); // Get physical address
    (void) munmap(pVirAddr, 1<<10);// Unmap it
    Rval = 0;
  } else {
    ArmLog_STR(SVC_LOG_CT_INIT, "## fail to call posix_typed_mem_open()! %s", strerror(errno), NULL);
  }

  /* Show CV memory information */
  if (Rval == 0) {
    CvAddr_s     = (UINT32)PhyAddr - (UINT32)(CvMemInfo.__posix_tmi_total - CvMemInfo.posix_tmi_length);
    CvAddr_e     = CvAddr_s + (UINT32)CvMemInfo.__posix_tmi_total;
    CvSchdr_s    = CvAddr_s;
    CvSchdr_e    = CvSchdr_s + CvSchSize;
    CvRtosAddr_s = CvSchdr_e;
    CvRtosAddr_e = CvRtosAddr_s + CvRtosSize;
    CvSysAddr_s  = CvRtosAddr_e;
    CvSysAddr_e  = CvAddr_e;

    SvcLog_DBG(SVC_LOG_CT_INIT, "[CV]", 0U, 0U);
    SvcLog_DBG(SVC_LOG_CT_INIT, "  - Total         : 0x%08X ~ 0x%08X", CvAddr_s, CvAddr_e);
    SvcLog_DBG(SVC_LOG_CT_INIT, "  - schdr         : 0x%08X ~ 0x%08X", CvSchdr_s, CvSchdr_e);
    SvcLog_DBG(SVC_LOG_CT_INIT, "  - icam_proc     : 0x%08X ~ 0x%08X", CvRtosAddr_s, CvRtosAddr_e);
    SvcLog_DBG(SVC_LOG_CT_INIT, "  - non-icam_proc : 0x%08X ~ 0x%08X", CvSysAddr_s, CvSysAddr_e);

    SvcLog_DBG(SVC_LOG_CT_INIT, "==================================================", 0U, 0U);
    SvcLog_DBG(SVC_LOG_CT_INIT, "<---------------- cv memory --------------------->", 0U, 0U);
    SvcLog_DBG(SVC_LOG_CT_INIT, "                  %d MB", (CvAddr_e - CvAddr_s) >> 20U, 0U);
    SvcLog_DBG(SVC_LOG_CT_INIT, "<- schdr -><-- icam_proc --> ", 0U, 0U);
    SvcLog_DBG(SVC_LOG_CT_INIT, "   %d MB       %d MB", (CvSchdr_e - CvSchdr_s) >> 20U, (CvRtosAddr_e - CvRtosAddr_s) >> 20U);
    SvcLog_DBG(SVC_LOG_CT_INIT, "                             <-- non-icam_proc -->", 0U, 0U);
    SvcLog_DBG(SVC_LOG_CT_INIT, "                                     %d MB", (CvSysAddr_e - CvSysAddr_s) >> 20U, 0U);
    SvcLog_DBG(SVC_LOG_CT_INIT, "==================================================", 0U, 0U);

    (void) close(MapFd);
  }
#endif
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: HookCtTestCommand
 *
 *  @Description:: Hook cnn_testbed test command
 *
 *  @Input      :: None
 *
 *  @Output     :: None
 *
 *  @Return     :: None
 *
\*-----------------------------------------------------------------------------------------------*/
static void HookCtTestCommand(void)
{
  static AMBA_SHELL_COMMAND_s CtCmds[] = {
      {.pName = "cv",     .MainFunc = CtShell_CommandCv,        .pNext = NULL},
      {.pName = "lv",     .MainFunc = CtShell_CommandLv,        .pNext = NULL},
  };
  UINT32 i, Rval, CmdNum;

  CmdNum = sizeof(CtCmds) / sizeof(AMBA_SHELL_COMMAND_s);
  for (i = 0U; i < CmdNum; i++) {
    Rval = SvcCmd_CommandRegister(&CtCmds[i]);
    if (SHELL_ERR_SUCCESS != Rval) {
      SvcLog_NG(SVC_LOG_CT_INIT, "## fail to register cnn_testbed test cmd", 0U, 0U);
    }
  }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: CtSysInit
 *
 *  @Description:: The init function of cnn_testbed
 *
 *  @Input      :: None
 *
 *  @Output     :: None
 *
 *  @Return     :: None
 *
\*-----------------------------------------------------------------------------------------------*/
void CtSysInit(void)
{
  /* 1. Dump cv memory info */
  DumpCvMemLayout();

  /* 2. Hook cnn_testbed test command */
  HookCtTestCommand();

  /* 3. Enable ambalink print module */
#ifdef CONFIG_ENABLE_AMBALINK
  (void) AmbaPrint_ModuleSetAllowList(AMBALINK_MODULE_ID, 1U);
#endif
}

