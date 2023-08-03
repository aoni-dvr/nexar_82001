/**
 *  @file SvcCvFlowProfile.h
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
 *  @details Header of SVC CVFlow profile
 *
 */

#ifndef SVC_CV_FLOW_PROFILE_H
#define SVC_CV_FLOW_PROFILE_H

#define SVC_CV_FLOW_PROF_MAX_INPUT_IF           (1U)
#define SVC_CV_FLOW_PROF_MAX_OUTPUT_IF          (1U)

/* Input Interface */
#define SVC_CV_FLOW_INPUT_IF_FLEXIDAG           (0U)
#define SVC_CV_FLOW_INPUT_IF_FLEXIDAG_LINUX     (1U)

/* Output Interface */
#define SVC_CV_FLOW_OUTPUT_IF_FLEXIDAG          (0U)
#define SVC_CV_FLOW_OUTPUT_IF_FLEXIDAG_LINUX    (1U)

/* Configuration of Input interface */
typedef struct {
    UINT32                      Interface;
    UINT32                      NumFov;
} SVC_CV_FLOW_INPUT_IF_CFG_s;

/* Configuration of Output interface */
typedef struct {
    UINT32                      Interface;
} SVC_CV_FLOW_OUTPUT_IF_CFG_s;

/* CvFlow Profile */
typedef struct {
    UINT32                      NumInIF;
    SVC_CV_FLOW_INPUT_IF_CFG_s  InIF[SVC_CV_FLOW_PROF_MAX_INPUT_IF];
    UINT32                      NumOutIF;
    SVC_CV_FLOW_OUTPUT_IF_CFG_s OutIF[SVC_CV_FLOW_PROF_MAX_OUTPUT_IF];
} SVC_CV_FLOW_PROF_s;

/* CVFlow(SuperDAG/FlexiDAG) profile ID */
#define SVC_CV_FLOW_PROF_FLEXIDAG               (0U)   /* RTOS FlexiDAG */
#define SVC_CV_FLOW_PROF_FLEXIDAG_LINUX         (1U)   /* Linux FlexiDAG */
#define SVC_CV_FLOW_PROF_ID_NUM                 (2U)

UINT32 SvcCvFlow_GetProfile(UINT32 ProfID, const SVC_CV_FLOW_PROF_s **pProfInfo);

#endif /* SVC_CV_FLOW_PROFILE_H */