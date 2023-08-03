/**
*  @file SvcCvAppDef.h
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
*  @details svc cv app definition
*
*/

#ifndef SVC_CV_APP_DEF_H
#define SVC_CV_APP_DEF_H

/* CV Flow Type */
#define SVC_CV_FLOW_APP_NONE                            (0U)        /* NONE */
#define SVC_CV_FLOW_REF_OD                              (1U)        /* Support open network object detection with SCA interface */
#define SVC_CV_FLOW_REF_SEG                             (2U)        /* Support open network segmentation with SCA interface */
#define SVC_CV_FLOW_AMBA_OD                             (3U)        /* Support Ambarella object detection with SCA interface */
#define SVC_CV_FLOW_AMBA_SEG                            (4U)        /* Support Ambarella segmentation with SCA interface */
#define SVC_CV_FLOW_STEREO                              (5U)        /* Support Ambarella stereo with SCA interface */
#define SVC_CV_FLOW_STIXEL                              (6U)        /* Support Ambarella stixel with SCA interface */
#define SVC_CV_FLOW_STEREO_AC                           (7U)        /* Support Ambarella stereo auto calibration with SCA interface */
#define SVC_CV_FLOW_CNN_TESTBED                         (8U)        /* Support Cnn testbed cvalgo examples */
#define SVC_CV_FLOW_FEX                                 (9U)        /* Support Ambarella fex with SCA interface */
#define SVC_CV_FLOW_LINUX_AC_FUSION_STIXEL              (10U)        /* Support Ambarella AC Fusion Stixel with SCA interface */


/* Following are obsolete */
#define SVC_CV_FLOW_REF_OD_FDAG         (50U)        /* Support open network object detection with wrapper interface */
#define SVC_CV_FLOW_AMBA_OD_FDAG        (51U)        /* Support Ambarella object detection with wrapper interface */
#define SVC_CV_FLOW_REF_SEG_FDAG        (52U)        /* Support segmentation with wrapper interface */
#define SVC_CV_FLOW_STEREO_FDAG         (53U)        /* Support Ambarella stereo with wrapper interface */

/* CV User Flag */
#define SVC_CV_APP_FILE_INPUT_TEST      (0x00000001U)   /* Enable File input test */
#define SVC_CV_APP_LOAD_TEST_DATA       (0x00000002U)   /* Enable reading test files */

#endif  /* SVC_CV_APP_DEF_H */
