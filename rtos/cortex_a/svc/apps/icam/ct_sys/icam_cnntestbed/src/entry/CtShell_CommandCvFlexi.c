/**
*  @file CtShell_CommandCvFlexi.c
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
*   @details The cnn_testbed flexidag related shell commands
*
*/

#include "CtShell_CommandCv.h"

/* SvcCvAlgo header */
#include "SvcCvAlgo.h"
#include "CtCvAlgoWrapper.h"

/* Sample cvalgo */
#if defined(CONFIG_CVALGO_CT_SAMPLE)
#include "SingleFD.h"
#include "SingleFDHeader.h"
#include "SSD.h"
#include "DetCls.h"
#include "FrcnnProc.h"
#endif

/* header for app buffer*/
#include "SvcMem.h"
#include "SvcBuffer.h"
#include "SvcBufMap.h"

/* data flow header */
#include "FileFeeder.h"
#include "FileSink.h"
#include "FileSinkHeader.h"
#include "IdspFeeder.h"
#include "SsdDisplay.h"
#include "SegDisplay.h"
#include "FrcnnProcDisplay.h"

/* header for schdr */
#include "schdr_api.h"

#define ARM_LOG_CV_FLEXI_CMD        "CvFlexiCmd"

static UINT32 g_FlexiMemPoolId = 0U;

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: GetBufFromApp
 *
 *  @Description:: Get available buffer from app (icam, cnn_testbed ...)
 *
 *  @Input      :: None
 *
 *  @Output     ::
 *    StartAddr:   The start address of the buffer
 *    Size:        The size of the buffer
 *
 *  @Return     ::
 *     UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 GetBufFromApp(ULONG *pBase, UINT32 *pSize)
{
  UINT32 Rval;
  ULONG  BufBase;

  Rval = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_CV_ALGO, &BufBase, pSize);
  if (Rval != 0U) {
    ArmLog_ERR(ARM_LOG_CV_FLEXI_CMD, "## SvcBuffer_Request() fail (Can't get buffer from app)", 0U, 0U);
    Rval = ARM_NG;
  } else {
    if (pBase != NULL) {
        *pBase = (ULONG)BufBase;
    }
    Rval = ARM_OK;
  }

  return Rval;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: GetBufFromApp
 *
 *  @Description:: Get available buffer from app (icam, cnn_testbed ...)
 *
 *  @Input      :: None
 *
 *  @Output     ::
 *    StartAddr:   The start address of the buffer
 *    Size:        The size of the buffer
 *
 *  @Return     ::
 *     UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
static void CreateMemPool(void)
{
  static UINT32 IsCreateMemPool = 0U;
  UINT32 Rval;

  /* Create Mem Pool */
  if (IsCreateMemPool == 0U) {
    ULONG StartAddr, StartPhyAddr;
    UINT32 Size;
    flexidag_memblk_t CvFlexiBuf;

    Rval = GetBufFromApp(&StartAddr, &Size);

    if (Rval == ARM_OK) {
      AmbaMisra_TypeCast(&CvFlexiBuf.pBuffer, &StartAddr);
      (void)AmbaMMU_VirtToPhys(StartAddr, &StartPhyAddr);
      CvFlexiBuf.buffer_daddr      = (UINT32)StartPhyAddr;
      CvFlexiBuf.buffer_size       = Size;
      CvFlexiBuf.buffer_cacheable  = 1;

      Rval = ArmMemPool_Create(&CvFlexiBuf, &g_FlexiMemPoolId);
      if (Rval == ARM_OK) {
        IsCreateMemPool = 1U;
      } else {
        ArmLog_ERR(ARM_LOG_CV_FLEXI_CMD, "## ArmMemPool_Create fail (Rval 0x%x)", Rval, 0U);
      }
    } else {
      ArmLog_ERR(ARM_LOG_CV_FLEXI_CMD, "## No buffer to create mempool", 0U, 0U);
    }
  }
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: CvFlexiFeedCmdUsage
 *
 *  @Description:: The usage of "cv flexi feed" test command
 *
 *  @Input      ::
 *    PrintFunc:   The pointer of print function
 *
 *  @Output     :: None
 *
 *  @Return     :: None
 *
\*-----------------------------------------------------------------------------------------------*/
static void CvFlexiFeedCmdUsage(AMBA_SHELL_PRINT_f PrintFunc)
{
  PrintFunc("CvFlexiFeed test commands:\n");
  PrintFunc("    feed file [Slot] [FeedFile_0] [FeedFile_1] ... \n");
  PrintFunc("        [Slot]:       The slot id \n");
  PrintFunc("        [FeedFile_N]: The file to be fed \n");
  PrintFunc("\n");
  PrintFunc("    feed idspfile [Slot] [FileY] [FileUV] [PydScale] [RoiPitch] [RoiW] [RoiH] [OffX] [OffY] [DataW] [DataH] \n");
  PrintFunc("        [Slot]:       The slot id \n");
  PrintFunc("        [FileY]:      The input Y file \n");
  PrintFunc("        [FileUV]:     The input UV file \n");
  PrintFunc("        [PydScale]:   The pyramid scale index \n");
  PrintFunc("        [RoiPitch]:   The pitch of roi yuv buffer \n");
  PrintFunc("        [RoiW]:       The width of roi yuv buffer \n");
  PrintFunc("        [RoiH]:       The height of roi yuv buffer \n");
  PrintFunc("        [OffX]:       The offset x to put input data \n");
  PrintFunc("        [OffY]:       The offset y to put input data \n");
  PrintFunc("        [DataW]:      The width of input data \n");
  PrintFunc("        [DataH]:      The height of input data \n");
  PrintFunc("\n");
  PrintFunc("    feed idsp start [Slot] [FrameRateDivisor] \n");
  PrintFunc("        [Slot]:             The slot id \n");
  PrintFunc("        [FrameRateDivisor]: The feeding framerate = current framerate / FrameRateDivisor \n");
  PrintFunc("    feed idsp stop [Slot] \n");
  PrintFunc("        [Slot]:             The slot id \n");
  PrintFunc("    feed secret [Slot] [SecretFile] \n");
  PrintFunc("        [Slot]:             The slot id \n");
  PrintFunc("        [SecretFile]:       The secret file \n");
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: CvFlexiRegCbCmdUsage
 *
 *  @Description:: The usage of "cv flexi reg_cb" test command
 *
 *  @Input      ::
 *    PrintFunc:   The pointer of print function
 *
 *  @Output     :: None
 *
 *  @Return     :: None
 *
\*-----------------------------------------------------------------------------------------------*/
static void CvFlexiRegCbCmdUsage(AMBA_SHELL_PRINT_f PrintFunc)
{
  PrintFunc("CvFlexiRegCb test commands:\n");
  PrintFunc("    reg_cb file [Slot] \n");
  PrintFunc("        [Slot]:      The slot id \n");
  PrintFunc("        The output filename is DUMP_XX_YYY.out \n");
  PrintFunc("        XX is the order of output node, YYY is the index of test iteration \n");
  PrintFunc("\n");
  PrintFunc("    reg_cb ssd [Slot] [OsdClear(opt)] [OsdFlush(opt)]\n");
  PrintFunc("        [Slot]:         The slot id \n");
  PrintFunc("        [OsdClear]:     Clear OSD before drawing (optional and default is 1) \n");
  PrintFunc("        [OsdFlush]:     Flush OSD after drawing (optional and default is 1) \n");
  PrintFunc("    reg_cb frcnnproc [Slot] [OsdClear(opt)] [OsdFlush(opt)]\n");
  PrintFunc("        [Slot]:         The slot id \n");
  PrintFunc("        [OsdClear]:     Clear OSD before drawing (optional and default is 1) \n");
  PrintFunc("        [OsdFlush]:     Flush OSD after drawing (optional and default is 1) \n");
  PrintFunc("\n");
  PrintFunc("    reg_cb seg [Slot] [ROI_W] [ROI_H] [ROI_X] [ROI_Y] [NET_W] [NET_H] [OsdClear(opt)] [OsdFlush(opt)]\n");
  PrintFunc("        [Slot]:         The slot id \n");
  PrintFunc("        [ROI_W]:        Width of ROI \n");
  PrintFunc("        [ROI_H]:        Height of ROI \n");
  PrintFunc("        [ROI_X]:        Start X of ROI \n");
  PrintFunc("        [ROI_Y]:        Start Y of ROI \n");
  PrintFunc("        [NET_W]:        Width of Net(CNN) \n");
  PrintFunc("        [NET_H]:        Height of Net(CNN) \n");
  PrintFunc("        [OsdClear]:     Clear OSD before drawing (optional and default is 1) \n");
  PrintFunc("        [OsdFlush]:     Flush OSD after drawing (optional and default is 1) \n");
  PrintFunc("\n");
  PrintFunc("    reg_cb file_header [Slot] \n");
  PrintFunc("        [Slot]:      The slot id \n");
  PrintFunc("        The output filename is OUTPUT_RAW_XX_YY.out \n");
  PrintFunc("        XX is XX-th Flexidag output buffer, YY is the YY-th output packed in this Flexidag output buffer \n");
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: CvFlexiCtrlCmdUsage
 *
 *  @Description:: The usage of "cv flexi ctrl" test command
 *
 *  @Input      ::
 *    PrintFunc:   The pointer of print function
 *
 *  @Output     :: None
 *
 *  @Return     :: None
 *
\*-----------------------------------------------------------------------------------------------*/
static void CvFlexiCtrlCmdUsage(AMBA_SHELL_PRINT_f PrintFunc)
{
  PrintFunc("CvFlexiCtrl test commands:\n");
  PrintFunc("    ctrl roi [Slot] [PyramidIdx] [ROIStartCol] [ROIStartRow] \n");
  PrintFunc("        [Slot]: The slot id \n");
  PrintFunc("        [PyramidIdx]: Level of idsp image pyramid [0 - 6] \n");
  PrintFunc("        [ROIStartCol]: Start column of roi \n");
  PrintFunc("        [ROIStartRow]: Start row of roi \n");
  PrintFunc("\n");
  PrintFunc("    ctrl ssd_nms [Slot] [ClassNum] [PriorBoxPath] [ROI_W] [ROI_H]\n");
  PrintFunc("    [ROI_X] [ROI_Y] [NET_W] [NET_H] [VIS_W] [VIS_H] [ModelType] \n");
  PrintFunc("        [Slot]:         The slot id \n");
  PrintFunc("        [ClassNum]:     Number of detect class \n");
  PrintFunc("        [PriorBoxPath]: Path of priorbox \n");
  PrintFunc("        [ROI_W]:        Width of ROI \n");
  PrintFunc("        [ROI_H]:        Height of ROI \n");
  PrintFunc("        [ROI_X]:        Start X of ROI \n");
  PrintFunc("        [ROI_Y]:        Start Y of ROI \n");
  PrintFunc("        [NET_W]:        Width of Net(CNN) \n");
  PrintFunc("        [NET_W]:        Height of Net(CNN) \n");
  PrintFunc("        [VIS_W]:        Width of Visual(OSD) \n");
  PrintFunc("        [VIS_H]:        Height of Visual(OSD) \n");
  PrintFunc("        [ModelType]:    0: caffe model, 1: Tensorflow model \n");
  PrintFunc("\n");
  PrintFunc("    ctrl frcnnproc [Slot] [ImgHeight] [ImgWidth] [ROI_W] [ROI_H]\n");
  PrintFunc("    [ROI_X] [ROI_Y] [NET_W] [NET_H] [VIS_W] [VIS_H] \n");
  PrintFunc("        [Slot]:         The slot id \n");
  PrintFunc("        [ImgWidth]:     Width of image \n");
  PrintFunc("        [ImgHeight]:    Height of image \n");
  PrintFunc("        [ROI_W]:        Width of ROI \n");
  PrintFunc("        [ROI_H]:        Height of ROI \n");
  PrintFunc("        [ROI_X]:        Start X of ROI \n");
  PrintFunc("        [ROI_Y]:        Start Y of ROI \n");
  PrintFunc("        [NET_W]:        Width of Net(CNN) \n");
  PrintFunc("        [NET_W]:        Height of Net(CNN) \n");
  PrintFunc("        [VIS_W]:        Width of Visual(OSD) \n");
  PrintFunc("        [VIS_H]:        Height of Visual(OSD) \n");
  PrintFunc("\n");
  PrintFunc("    ctrl dmsg [Slot]\n");
  PrintFunc("        [Slot]:         The slot id \n");
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: CvFlexiCmdUsage
 *
 *  @Description:: The usage of "cv flexi" test command
 *
 *  @Input      ::
 *    PrintFunc:   The pointer of print function
 *
 *  @Output     :: None
 *
 *  @Return     :: None
 *
\*-----------------------------------------------------------------------------------------------*/
static void CvFlexiCmdUsage(AMBA_SHELL_PRINT_f PrintFunc)
{
  PrintFunc("CvFlexi test commands:\n");
  PrintFunc("    schdr_start [MaxNum]\n");
  PrintFunc("        [MaxNum]: Maximum flexidag number \n");
  PrintFunc("    create [Slot] [CvAlgo] [Flexibin0] [Flexibin1] ...\n");
  PrintFunc("        [Slot]: The slot id \n");
  PrintFunc("        [CvAlgo]: The hooked CvAlgo object \n");
  PrintFunc("                  single - SingleFD cvalgo \n");
  PrintFunc("                  ssd    - SSD cvalgo \n");
  PrintFunc("                  detcls - Det + Cls cvalgo \n");
  PrintFunc("                  header - SingleFDHeader cvalgo \n");
  PrintFunc("        [FlexibinN]: The path of flexibin \n");
  PrintFunc("    delete [Slot]\n");
  PrintFunc("        [Slot]: The slot id \n");
  PrintFunc("\n");
  PrintFunc("    feed [...] \n");
  PrintFunc("        Feed related test command\n");
  PrintFunc("    reg_cb [...] \n");
  PrintFunc("        Register callback related test command\n");
  PrintFunc("    ctrl [...] \n");
  PrintFunc("        Control related test command\n");
  PrintFunc("\n");

}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: CtShell_CommandCvFlexiFeed
 *
 *  @Description:: The cnn_testbed "cv flexi feed" test command
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
static void CtShell_CommandCvFlexiFeed(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
  UINT32 Rval;

  if (ArgCount < 4U) {  // pArgVector[0] = 'cv', pArgVector[1] = 'flexi', pArgVector[2] = 'feed'
    CvFlexiFeedCmdUsage(PrintFunc);
  } else {

    /* feed file [Slot] [FeedFile_0] [FeedFile_1] ... */
    if ((ArmStdC_strcmp(pArgVector[3], "file") == 0) && (ArgCount >= 6U) && (ArgCount <= 13U)) {
      FILE_FEEDER_CFG_s FeederCfg = {0};

      (void) ArmStdC_strtoul(pArgVector[4], &FeederCfg.Slot);
      FeederCfg.MemPoolId = g_FlexiMemPoolId;

      FeederCfg.Num   = ArgCount - 5U;
      for (UINT32 i = 0; i < FeederCfg.Num; i++) {
        (void) ArmStdC_memcpy(FeederCfg.Filename[i], pArgVector[i + 5U], MAX_FEEDER_FN_LEN);
      }
      Rval = FileFeeder_Start(&FeederCfg);
      if (Rval != ARM_OK) {
        ArmLog_ERR(ARM_LOG_CV_FLEXI_CMD, "## FileFeeder_Start() fail", 0U, 0U);
      }

    /* feed idspfile [Slot] [FileY] [FileUV] [PydScale] [RoiPitch] [RoiW] [RoiH] [OffX] [OffY] [DataW] [DataH] */
    } else if ((ArmStdC_strcmp(pArgVector[3], "idspfile") == 0) && (ArgCount == 15U)) {
      IDSPFILE_FEEDER_CFG_s FeederCfg = {0};

      (void) ArmStdC_strtoul(pArgVector[4], &FeederCfg.Slot);
      FeederCfg.MemPoolId = g_FlexiMemPoolId;
      (void) ArmStdC_memcpy(FeederCfg.FilenameY, pArgVector[5], MAX_FEEDER_FN_LEN);
      (void) ArmStdC_memcpy(FeederCfg.FilenameUV, pArgVector[6], MAX_FEEDER_FN_LEN);
      (void) ArmStdC_strtoul(pArgVector[7], &FeederCfg.PyramidScale);
      (void) ArmStdC_strtoul(pArgVector[8], &FeederCfg.RoiPitch);
      (void) ArmStdC_strtoul(pArgVector[9], &FeederCfg.RoiWidth);
      (void) ArmStdC_strtoul(pArgVector[10], &FeederCfg.RoiHeight);
      (void) ArmStdC_strtoul(pArgVector[11], &FeederCfg.OffsetX);
      (void) ArmStdC_strtoul(pArgVector[12], &FeederCfg.OffsetY);
      (void) ArmStdC_strtoul(pArgVector[13], &FeederCfg.DataWidth);
      (void) ArmStdC_strtoul(pArgVector[14], &FeederCfg.DataHeight);

      Rval = IdspFileFeeder_Start(&FeederCfg);
      if (Rval != ARM_OK) {
        ArmLog_ERR(ARM_LOG_CV_FLEXI_CMD, "## IdspFileFeeder_Start() fail", 0U, 0U);
      }

    /* feed idsp ... */
    } else if ((ArmStdC_strcmp(pArgVector[3], "idsp") == 0) && (ArgCount >= 5U)) {

      /* start [Slot] [FrameRateDivisor] */
      if ((ArmStdC_strcmp(pArgVector[4], "start") == 0) && (ArgCount == 7U)) {
        IDSP_FEEDER_CFG_s FeederCfg = {0};

        (void) ArmStdC_strtoul(pArgVector[5], &FeederCfg.Slot);
        (void) ArmStdC_strtoul(pArgVector[6], &FeederCfg.FrameRateDivisor);

        Rval = IdspFeeder_Start(&FeederCfg);
        if (Rval != ARM_OK) {
          ArmLog_ERR(ARM_LOG_CV_FLEXI_CMD, "## IdspFeeder_Start() fail", 0U, 0U);
        }

      /* stop [Slot] */
      } else if ((ArmStdC_strcmp(pArgVector[4], "stop") == 0) && (ArgCount == 6U)) {
        UINT32 Slot = 0;

        (void) ArmStdC_strtoul(pArgVector[5], &Slot);

        Rval = IdspFeeder_Stop(Slot);
        if (Rval != ARM_OK) {
          ArmLog_ERR(ARM_LOG_CV_FLEXI_CMD, "## IdspFeeder_Stop() fail", 0U, 0U);
        }

      } else {
        CvFlexiFeedCmdUsage(PrintFunc);
      }

    /* feed secret [Slot] [SecretFile] */
    } else if ((ArmStdC_strcmp(pArgVector[3], "feed") == 0) && (ArgCount == 7U)) {
      SECRET_FEEDER_CFG_s FeederCfg = {0};

      (void) ArmStdC_strtoul(pArgVector[4], &FeederCfg.Slot);
      FeederCfg.MemPoolId = g_FlexiMemPoolId;
      (void) ArmStdC_memcpy(FeederCfg.Filename, pArgVector[5], MAX_FEEDER_FN_LEN);

      Rval = SecretFeeder_Start(&FeederCfg);
      if (Rval != ARM_OK) {
        ArmLog_ERR(ARM_LOG_CV_FLEXI_CMD, "## SecretFeeder_Start() fail", 0U, 0U);
      }

    } else {
      CvFlexiFeedCmdUsage(PrintFunc);
    }
  }
  return;
}



/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: CtShell_CommandCvFlexiRegCb
 *
 *  @Description:: The cnn_testbed "cv flexi reg_cb" test command
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
static void CtShell_CommandCvFlexiRegCb(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
  UINT32 Rval;

  if (ArgCount < 4U) {  // pArgVector[0] = 'cv', pArgVector[1] = 'flexi', pArgVector[2] = 'reg_cb'
    CvFlexiRegCbCmdUsage(PrintFunc);
  } else {

    /* reg_cb file [Slot] */
    if ((ArmStdC_strcmp(pArgVector[3], "file") == 0) && (ArgCount == 5U)) {
      FILE_SINK_CFG_s SinkCfg = {0};
      (void) ArmStdC_strtoul(pArgVector[4], &SinkCfg.Slot);

      Rval = FileSink_Register(&SinkCfg);
      if (Rval != ARM_OK) {
        ArmLog_ERR(ARM_LOG_CV_FLEXI_CMD, "## FileSink_Register() fail", 0U, 0U);
      }

    /* reg_cb file_header [Slot] */
    } else if ((ArmStdC_strcmp(pArgVector[3], "file_header") == 0) && (ArgCount == 5U)) {
      FILE_SINK_HEADER_CFG_s SinkHeaderCfg = {0};
      (void) ArmStdC_strtoul(pArgVector[4], &SinkHeaderCfg.Slot);

      Rval = FileSinkHeader_Register(&SinkHeaderCfg);
      if (Rval != ARM_OK) {
        ArmLog_ERR(ARM_LOG_CV_FLEXI_CMD, "## FileSinkHeader_Register() fail", 0U, 0U);
      }

    /* reg_cb ssd [Slot] [OsdClear(opt)] [OsdFlush(opt)] */
    } else if ((ArmStdC_strcmp(pArgVector[3], "ssd") == 0) && (ArgCount >= 5U)) {
      SSD_DISPLAY_CFG_s SsdDispCfg = {0};

      (void) ArmStdC_strtoul(pArgVector[4], &SsdDispCfg.Slot);

      if (ArgCount >= 6U) {
        (void) ArmStdC_strtoul(pArgVector[5], &SsdDispCfg.OsdClearFlg);
      } else {
        SsdDispCfg.OsdClearFlg = 1;
      }

      if (ArgCount >= 7U) {
        (void) ArmStdC_strtoul(pArgVector[6], &SsdDispCfg.OsdFlushFlg);
      } else {
        SsdDispCfg.OsdFlushFlg = 1;
      }

      Rval = SsdDisplay_Register(&SsdDispCfg);
      if (Rval != ARM_OK) {
        ArmLog_ERR(ARM_LOG_CV_FLEXI_CMD, "## SsdDisplay_Register() fail", 0U, 0U);
      }

    /* reg_cb frcnnproc [Slot] [OsdClear(opt)] [OsdFlush(opt)] */
    } else if ((ArmStdC_strcmp(pArgVector[3], "frcnnproc") == 0) && (ArgCount >= 5U)) {
      FRCNN_PROC_DISPLAY_CFG_s FrcnnProcDispCfg = {0};

      (void) ArmStdC_strtoul(pArgVector[4], &FrcnnProcDispCfg.Slot);

      if (ArgCount >= 6U) {
        (void) ArmStdC_strtoul(pArgVector[5], &FrcnnProcDispCfg.OsdClearFlg);
      } else {
        FrcnnProcDispCfg.OsdClearFlg = 1;
      }

      if (ArgCount >= 7U) {
        (void) ArmStdC_strtoul(pArgVector[6], &FrcnnProcDispCfg.OsdFlushFlg);
      } else {
        FrcnnProcDispCfg.OsdFlushFlg = 1;
      }

      Rval = FrcnnProcDisplay_Register(&FrcnnProcDispCfg);
      if (Rval != ARM_OK) {
        ArmLog_ERR(ARM_LOG_CV_FLEXI_CMD, "## FrcnnProcDisplay_Register() fail", 0U, 0U);
      }

    /* reg_cb seg [Slot] [ROI_W] [ROI_H] [ROI_X] [ROI_Y] [NET_W] [NET_H] [OsdClear(opt)] [OsdFlush(opt)] */
    } else if ((ArmStdC_strcmp(pArgVector[3], "seg") == 0) && (ArgCount >= 11U)) {
      SEG_DISPLAY_CFG_s SegDispCfg = {0};

      (void) ArmStdC_strtoul(pArgVector[4], &SegDispCfg.Slot);
      (void) ArmStdC_strtoul(pArgVector[5], &SegDispCfg.Win.RoiWidth);
      (void) ArmStdC_strtoul(pArgVector[6], &SegDispCfg.Win.RoiHeight);
      (void) ArmStdC_strtoul(pArgVector[7], &SegDispCfg.Win.RoiStartX);
      (void) ArmStdC_strtoul(pArgVector[8], &SegDispCfg.Win.RoiStartY);
      (void) ArmStdC_strtoul(pArgVector[9], &SegDispCfg.Win.NetworkWidth);
      (void) ArmStdC_strtoul(pArgVector[10], &SegDispCfg.Win.NetworkHeight);

      if (ArgCount >= 12U) {
        (void) ArmStdC_strtoul(pArgVector[11], &SegDispCfg.OsdClearFlg);
      } else {
        SegDispCfg.OsdClearFlg = 1;
      }

      if (ArgCount >= 13U) {
        (void) ArmStdC_strtoul(pArgVector[12], &SegDispCfg.OsdFlushFlg);
      } else {
        SegDispCfg.OsdFlushFlg = 1;
      }

      Rval = SegDisplay_Register(&SegDispCfg);
      if (Rval != ARM_OK) {
        ArmLog_ERR(ARM_LOG_CV_FLEXI_CMD, "## SegDisplay_Register() fail", 0U, 0U);
      }

    } else {
      CvFlexiRegCbCmdUsage(PrintFunc);
    }
  }
  return;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: CtShell_CommandCvFlexiCtrl
 *
 *  @Description:: The cnn_testbed "cv flexi reg_cb" test command
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
static void CtShell_CommandCvFlexiCtrl(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
  UINT32 Rval;

  if (ArgCount < 4U) {  // pArgVector[0] = 'cv', pArgVector[1] = 'flexi', pArgVector[2] = 'ctrl'
    CvFlexiCtrlCmdUsage(PrintFunc);
  } else {

    /* ctrl roi [Slot] [PyramidIdx] [ROIStartCol] [ROIStartRow] */
    if ((ArmStdC_strcmp(pArgVector[3], "roi") == 0) && (ArgCount == 8U)) {
      amba_roi_config_t RoiCtrl = {0};
      UINT32 Slot;

      (void) ArmStdC_strtoul(pArgVector[4], &Slot);
      (void) ArmStdC_strtoul(pArgVector[5], &RoiCtrl.image_pyramid_index);
      (void) ArmStdC_strtoul(pArgVector[6], &RoiCtrl.roi_start_col);
      (void) ArmStdC_strtoul(pArgVector[7], &RoiCtrl.roi_start_row);

      Rval = CtCvAlgoWrapper_Control(Slot, CTRL_TYPE_ROI, &RoiCtrl);
      if (Rval != ARM_OK) {
        ArmLog_ERR(ARM_LOG_CV_FLEXI_CMD, "## CtCvAlgoWrapper_Control fail", 0U, 0U);
      }

    /* ctrl ssd_nms [Slot] [ClassNum] [PriorBoxPath] [ROI_W] [ROI_H] [ROI_X] [ROI_Y] [NET_W] [NET_H] [VIS_W] [VIS_H] [ModelType] */
    } else if ((ArmStdC_strcmp(pArgVector[3], "ssd_nms") == 0) && (ArgCount >= 16U)) {
      SSD_NMS_CTRL_s SsdNmsCtrl;
      UINT32 Slot;

      (void) ArmStdC_strtoul(pArgVector[4], &Slot);
      (void) ArmStdC_strtoul(pArgVector[5], &SsdNmsCtrl.ClassNum);
      ArmStdC_strcpy(SsdNmsCtrl.PriorBox, MAX_PBOX_FN_LEN, pArgVector[6]);
      (void) ArmStdC_strtoul(pArgVector[7], &SsdNmsCtrl.Win.RoiWidth);
      (void) ArmStdC_strtoul(pArgVector[8], &SsdNmsCtrl.Win.RoiHeight);
      (void) ArmStdC_strtoul(pArgVector[9], &SsdNmsCtrl.Win.RoiStartX);
      (void) ArmStdC_strtoul(pArgVector[10], &SsdNmsCtrl.Win.RoiStartY);
      (void) ArmStdC_strtoul(pArgVector[11], &SsdNmsCtrl.Win.NetworkWidth);
      (void) ArmStdC_strtoul(pArgVector[12], &SsdNmsCtrl.Win.NetworkHeight);
      (void) ArmStdC_strtoul(pArgVector[13], &SsdNmsCtrl.Win.VisWidth);
      (void) ArmStdC_strtoul(pArgVector[14], &SsdNmsCtrl.Win.VisHeight);
      (void) ArmStdC_strtoul(pArgVector[15], &SsdNmsCtrl.ModelType);

      Rval = CtCvAlgoWrapper_Control(Slot, CTRL_TYPE_SSD_NMS, &SsdNmsCtrl);
      if (Rval != ARM_OK) {
        ArmLog_ERR(ARM_LOG_CV_FLEXI_CMD, "## CtCvAlgoWrapper_Control fail", 0U, 0U);
      }

    /* ctrl frcnnproc [Slot] [ImgWidth] [ImgHeight] [ROI_W] [ROI_H] [ROI_X] [ROI_Y] [NET_W] [NET_H] [VIS_W] [VIS_H] */
    } else if ((ArmStdC_strcmp(pArgVector[3], "frcnnproc") == 0) && (ArgCount >= 7U)) {
      FRCNN_PROC_PARAM_s FrcnnProcParam;
      UINT32 Slot;

      (void) ArmStdC_strtoul(pArgVector[4], &Slot);
      (void) ArmStdC_strtoul(pArgVector[5], &FrcnnProcParam.ImgWidth);
      (void) ArmStdC_strtoul(pArgVector[6], &FrcnnProcParam.ImgHeight);

      if (ArgCount >= 15U) {
        FrcnnProcParam.OsdEnable = 1U;
        (void) ArmStdC_strtoul(pArgVector[7], &FrcnnProcParam.RoiWidth);
        (void) ArmStdC_strtoul(pArgVector[8], &FrcnnProcParam.RoiHeight);
        (void) ArmStdC_strtoul(pArgVector[9], &FrcnnProcParam.RoiStartX);
        (void) ArmStdC_strtoul(pArgVector[10], &FrcnnProcParam.RoiStartY);
        (void) ArmStdC_strtoul(pArgVector[11], &FrcnnProcParam.NetworkWidth);
        (void) ArmStdC_strtoul(pArgVector[12], &FrcnnProcParam.NetworkHeight);
        (void) ArmStdC_strtoul(pArgVector[13], &FrcnnProcParam.VisWidth);
        (void) ArmStdC_strtoul(pArgVector[14], &FrcnnProcParam.VisHeight);
      } else {
        FrcnnProcParam.OsdEnable = 0U;
      }

      Rval = CtCvAlgoWrapper_Control(Slot, CTRL_TYPE_FRCNN_PROC_PARAM, &FrcnnProcParam);
      if (Rval != ARM_OK) {
        ArmLog_ERR(ARM_LOG_CV_FLEXI_CMD, "## CtCvAlgoWrapper_Control fail", 0U, 0U);
      }

    /* ctrl dmsg [Slot] */
    } else if ((ArmStdC_strcmp(pArgVector[3], "dmsg") == 0) && (ArgCount >= 5U)) {
      UINT32 Slot;

      (void) ArmStdC_strtoul(pArgVector[4], &Slot);

      Rval = CtCvAlgoWrapper_Control(Slot, CTRL_TYPE_DMSG, NULL);
      if (Rval != ARM_OK) {
        ArmLog_ERR(ARM_LOG_CV_FLEXI_CMD, "## CtCvAlgoWrapper_Control fail", 0U, 0U);
      }

    } else {
      CvFlexiCtrlCmdUsage(PrintFunc);
    }
  }
  return;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: CtShell_CommandCvFlexi
 *
 *  @Description:: The cnn_testbed "cv flexi" test command
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
void CtShell_CommandCvFlexi(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
  UINT32 Rval;

  if (ArgCount < 3U) {    // pArgVector[0] = 'cv', pArgVector[1] = 'flexi'
    CvFlexiCmdUsage(PrintFunc);
  } else {

    /* schdr_start [MaxNum] */
    if ((ArmStdC_strcmp(pArgVector[2], "schdr_start") == 0) && (ArgCount == 4U)) {
      AMBA_CV_STANDALONE_SCHDR_CFG_s SchdrCfg = {0};
      UINT32 MaxNum = 0U;

      (void) ArmStdC_strtoul(pArgVector[3], &MaxNum);
      SchdrCfg.flexidag_slot_num = (INT32) MaxNum;
      SchdrCfg.cavalry_slot_num  = 0;
      SchdrCfg.cpu_map           = 0xE;
      SchdrCfg.log_level         = LVL_DEBUG;
#ifdef CONFIG_CV_EXTERNAL_MSG_THREAD
      SchdrCfg.flag              = (INT32) (SCHDR_FASTBOOT | SCHDR_NO_INTERNAL_THREAD);
#else
      SchdrCfg.flag              = (INT32) SCHDR_FASTBOOT;
#endif
      SchdrCfg.boot_cfg          = 0;
      Rval = AmbaCV_StandAloneSchdrStart(&SchdrCfg);

      if (Rval != 0U) {
        ArmLog_ERR(ARM_LOG_CV_FLEXI_CMD, "## AmbaCV_StandAloneSchdrStart fail (Rval 0x%x)", Rval, 0U);
      }

    /* create [Slot] [CvAlgo] [Flexibin0] [Flexibin1] ... */
    } else if ((ArmStdC_strcmp(pArgVector[2], "create") == 0) && (ArgCount >= 6U)) {
      UINT32 Slot = 0U;
      SVC_CV_ALGO_OBJ_s              *pCvAlgoObj = NULL;
      CT_CVALGO_WRAPPER_CREATE_CFG_s CreateCfg;

      (void) ArmStdC_strtoul(pArgVector[3], &Slot);

      /* 1. Create MemPool at first time */
      CreateMemPool();

      /* 2. Hook CvAlgoObj */
#if defined(CONFIG_CVALGO_CT_SAMPLE)
      if (ArmStdC_strcmp(pArgVector[4], "single") == 0) {
        pCvAlgoObj = &CvAlgoObj_SingleFD;
      }
      if (ArmStdC_strcmp(pArgVector[4], "ssd") == 0) {
        pCvAlgoObj = &CvAlgoObj_SSD;
      }
      if (ArmStdC_strcmp(pArgVector[4], "detcls") == 0) {
        pCvAlgoObj = &CvAlgoObj_DetCls;
      }
      if (ArmStdC_strcmp(pArgVector[4], "frcnnproc") == 0) {
        pCvAlgoObj = &CvAlgoObj_FrcnnProc;
      }
      if (ArmStdC_strcmp(pArgVector[4], "header") == 0) {
        pCvAlgoObj = &CvAlgoObj_SingleFDHeader;
      }
#endif

      if (pCvAlgoObj != NULL) {
        /* 3. Copy flexibin path */
        CreateCfg.NumFD = ArgCount - 5U;  // " cv flexi create [Slot] [CvAlgo]" = 5 extra args
        for (UINT32 i = 0; i < CreateCfg.NumFD; i ++) {
          ArmStdC_strcpy(CreateCfg.Flexibin[i], MAX_PATH_LEN, pArgVector[5U + i]);
        }

        /* 4. Call CtCvAlgoWrapper_Create */
        CreateCfg.pCvAlgoObj = pCvAlgoObj;
        CreateCfg.MemPoolId  = g_FlexiMemPoolId;

        Rval = CtCvAlgoWrapper_Create(Slot, &CreateCfg);
        if (Rval != ARM_OK) {
          ArmLog_ERR(ARM_LOG_CV_FLEXI_CMD, "## CtCvAlgoWrapper_Create fail", 0U, 0U);
        }
      } else {
        ArmLog_ERR(ARM_LOG_CV_FLEXI_CMD, "## unknown cvalgo", 0U, 0U);
      }

    /* delete [Slot] */
    } else if ((ArmStdC_strcmp(pArgVector[2], "delete") == 0) && (ArgCount == 4U)) {
      UINT32 Slot = 0U;

      (void) ArmStdC_strtoul(pArgVector[3], &Slot);

      Rval = CtCvAlgoWrapper_Delete(Slot);
      if (Rval != ARM_OK) {
        ArmLog_ERR(ARM_LOG_CV_FLEXI_CMD, "## CtCvAlgoWrapper_Delete fail", 0U, 0U);
      }

    /* feed */
    } else if (ArmStdC_strcmp(pArgVector[2], "feed") == 0) {
      CtShell_CommandCvFlexiFeed(ArgCount, pArgVector, PrintFunc);

    /* register_cb */
    } else if (ArmStdC_strcmp(pArgVector[2], "reg_cb") == 0) {
      CtShell_CommandCvFlexiRegCb(ArgCount, pArgVector, PrintFunc);

    /* ctrl */
    } else if (ArmStdC_strcmp(pArgVector[2], "ctrl") == 0) {
      CtShell_CommandCvFlexiCtrl(ArgCount, pArgVector, PrintFunc);

    } else {
      CvFlexiCmdUsage(PrintFunc);
    }
  }
  return;
}

