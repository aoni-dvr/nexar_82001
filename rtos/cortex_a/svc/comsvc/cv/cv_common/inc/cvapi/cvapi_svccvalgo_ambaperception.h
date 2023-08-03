/**
 *  @file cvapi_svccvalgo_ambaperception.h
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
 *  @details Implementation of amba perception svc cv algo
 *
 */

#ifndef CVAPI_SVCCVALGO_AMBAPERCEPTION_H
#define CVAPI_SVCCVALGO_AMBAPERCEPTION_H


#define AMBANET_ODV37_MAX_FD       12U

#define AMBANET_ODV37_KP_BATCH_SZ   64U
#define AMBANET_ODV37_3D_BATCH_SZ   32U
#define AMBANET_ODV37_MK_BATCH_SZ   32U
#define AMBANET_ODV37_TS_BATCH_SZ   64U
#define AMBANET_ODV37_TL_BATCH_SZ   64U
#define AMBANET_ODV37_AR_BATCH_SZ   32U
#define AMBANET_ODV37_VB_BATCH_SZ   32U
#define AMBANET_ODV37_TR_BATCH_SZ   128U



#define AMBANET_OD37_2D             0x0U
#define AMBANET_OD37_KP             0x1U
#define AMBANET_OD37_3D             0x2U
#define AMBANET_OD37_MKL            0x3U
#define AMBANET_OD37_MKS            0x4U
#define AMBANET_OD37_TS             0x5U    /* TrafficSign Classifier */
#define AMBANET_OD37_SL             0x6U    /* SpeedLimit Classifier */
#define AMBANET_OD37_TLC            0x7U    /* TrafficLight Color Classifier */
#define AMBANET_OD37_TLS            0x8U    /* TrafficLight Symbol Classifier */
#define AMBANET_OD37_AR             0x9U    /* Arrow Classifier */
#define AMBANET_OD37_VB             0xAU    /* Vehicle Brake Light Classifier */
#define AMBANET_OD37_TR             0xBU    /* AmbaTracker */



#define CALLBACK_EVENT_KP_OUTPUT    0x1000U
#define CALLBACK_EVENT_3D_OUTPUT    0x2000U
#define CALLBACK_EVENT_MKL_OUTPUT   0x4000U
#define CALLBACK_EVENT_MKS_OUTPUT   0x8000U
#define CALLBACK_EVENT_TS_OUTPUT    0x10000U
#define CALLBACK_EVENT_SL_OUTPUT    0x20000U
#define CALLBACK_EVENT_TLC_OUTPUT   0x40000U
#define CALLBACK_EVENT_TLS_OUTPUT   0x80000U
#define CALLBACK_EVENT_AR_OUTPUT    0x100000U
#define CALLBACK_EVENT_VB_OUTPUT    0x200000U
#define CALLBACK_EVENT_TR_OUTPUT    0x400000U



extern SVC_CV_ALGO_OBJ_s AmbaPcptAlgoObj;


typedef struct {
    uint16_t clsId;
    uint16_t field;
    uint32_t track;
    FLOAT upper_left_x; //box[0]
    FLOAT upper_left_y; //box[1]
    FLOAT bottom_right_x; //box[2]
    FLOAT bottom_right_y; //box[3]
    FLOAT dimension[3];
    FLOAT orientation[12];
    FLOAT confidence[12];
    FLOAT w; //width
    FLOAT h; //height
    FLOAT l; //length
    FLOAT alpha; //orientation
} obj_t;

typedef struct {
    UINT8                   RoiIdx;     //// Indicate which ROI is used
    /* 0x1: enable network, 0x2: run network */
    /* Enable means load the FD. */
    /* Enable can only be set at the init stage and can't be disabled. */
    /* KP supports person(0) and rider (1)class. */
    UINT8                   EnableKP;   ///< Flag to enable / run KeyPoint NN
    /* 3D supports bicycle(2), motorcycle(3),  car(4), and truck(5) class. */
    UINT8                   Enable3D;   ///< Flag to enable / run 3D OD NN
    /* MK supports all classes except RED(6) and GREEN(7) class. */
    UINT8                   EnableMask; ///< Flag to enable / run InstanceSeg NN
    /* TrafficSign classifer supports sign (8) */
    UINT8                   EnableTS;   ///< Flag to enable / run TrafficSign classifier NN
    /* SpeedLimit classifer supports sign (8) */
    UINT8                   EnableSL;   ///< Flag to enable / run SpeedLimit classifier NN
    /* TrafficLight Color classifer supports red (6) and green (7) */
    UINT8                   EnableTLC;   ///< Flag to enable / run TrafficLight Color classifier NN
    /* TrafficLight Symbol classifer supports red (6) and green (7) */
    UINT8                   EnableTLS;   ///< Flag to enable / run TrafficLight Symbol classifier NN
    /* Arrow classifer supports arrow (10) */
    UINT8                   EnableAR;   ///< Flag to enable / run Arrow classifier NN
    /* Vehicle brake light classifier supports  motorcycle(3),  car(4), and truck(5) class */
    UINT8                   EnableVB;   ///< Flag to enable / run Vehicle brake light classifier NN
    /* AmbaTracker supports all classes  */
    UINT8                   EnableTR;   ///< Flag to enable / run AmbaTracker NN

    UINT16                  OSDWidth;   ///< for maks network to resize the output object
    UINT16                  OSDHeight;  ///< for maks network to resize the output object

    AMBA_CV_FLEXIDAG_IO_s   *pKPIn;     ///< Input buffer for KeyPoint
    AMBA_CV_FLEXIDAG_IO_s   *p3DIn;     ///< Input buffer for 3D
    AMBA_CV_FLEXIDAG_IO_s   *pMKIn;     ///< Input buffer for Mask
    AMBA_CV_FLEXIDAG_IO_s   *pTSIn;     ///< Input buffer for TrafficSign classifier
    AMBA_CV_FLEXIDAG_IO_s   *pSLIn;     ///< Input buffer for SpeedLimit classifier
    AMBA_CV_FLEXIDAG_IO_s   *pTLCIn;    ///< Input buffer for TrafficLight Color classifier
    AMBA_CV_FLEXIDAG_IO_s   *pTLSIn;    ///< Input buffer for TrafficLight Symbol classifier
    AMBA_CV_FLEXIDAG_IO_s   *pARIn;     ///< Input buffer for Arrow classifier
    AMBA_CV_FLEXIDAG_IO_s   *pVBIn;     ///< Input buffer for Vehicle brake light classifier
    AMBA_CV_FLEXIDAG_IO_s   *pTRIn;     ///< Input buffer for AmbaTracker

    AMBA_CV_FLEXIDAG_IO_s   *pKPOut;    ///< Output buffer for KeyPoint (for data structure)
    AMBA_CV_FLEXIDAG_IO_s   *p3DOut;    ///< Output buffer for 3D (for data structure)
    AMBA_CV_FLEXIDAG_IO_s   *pMKOut;    ///< Output buffer for Mask (for data structure)
    AMBA_CV_FLEXIDAG_IO_s   *pTSOut;    ///< Output buffer for TrafficSign classifier
    AMBA_CV_FLEXIDAG_IO_s   *pTLCOut;   ///< Output buffer for TrafficLight Color classifier
    AMBA_CV_FLEXIDAG_IO_s   *pTLSOut;   ///< Output buffer for TrafficLight Symbol classifier
    AMBA_CV_FLEXIDAG_IO_s   *pAROut;    ///< Output buffer for Arrow classifier
    AMBA_CV_FLEXIDAG_IO_s   *pVBOut;    ///< Output buffer for Vehicle brake light classifier
    AMBA_CV_FLEXIDAG_IO_s   *pTROut;    ///< Output buffer for AmbaTracker

    AMBA_CV_FLEXIDAG_IO_s   *pKPBuf;    ///< Internal buffer for KeyPoint
    AMBA_CV_FLEXIDAG_IO_s   *p3DBuf;    ///< Internal buffer for 3D
    AMBA_CV_FLEXIDAG_IO_s   *pMKBuf;    ///< Internal buffer for Mask
    AMBA_CV_FLEXIDAG_IO_s   *pTSBuf;    ///< Internal buffer for TrafficSign classifier
    AMBA_CV_FLEXIDAG_IO_s   *pSLBuf;    ///< Internal buffer for SpeedLimit classifier
    AMBA_CV_FLEXIDAG_IO_s   *pTLCBuf;   ///< Internal buffer for TrafficLight Color classifier
    AMBA_CV_FLEXIDAG_IO_s   *pTLSBuf;   ///< Internal buffer for TrafficLight Symbol classifier
    AMBA_CV_FLEXIDAG_IO_s   *pARBuf;    ///< Internal buffer for Arrow classifier
    AMBA_CV_FLEXIDAG_IO_s   *pVBBuf;    ///< Internal buffer for Vehicle brake light classifier
    AMBA_CV_FLEXIDAG_IO_s   *pTRBuf;    ///< Internal buffer for AmbaTracker

    /* User MUST provides the list for sub networks. */
    UINT8                   RunObjsKP[AMBANET_ODV37_KP_BATCH_SZ]; ///< The objs list to run KP
    UINT8                   RunObjs3D[AMBANET_ODV37_3D_BATCH_SZ]; ///< The objs list to run 3D
    UINT8                   RunObjsMK[AMBANET_ODV37_MK_BATCH_SZ]; ///< The objs list to run MK
    UINT8                   RunObjsTS[AMBANET_ODV37_TS_BATCH_SZ]; ///< The objs list to run TS Classifier
    UINT8                   RunObjsTL[AMBANET_ODV37_TL_BATCH_SZ]; ///< The objs list to run TL Classifier
    UINT8                   RunObjsAR[AMBANET_ODV37_AR_BATCH_SZ]; ///< The objs list to run AR Classifier
    UINT8                   RunObjsVB[AMBANET_ODV37_VB_BATCH_SZ]; ///< The objs list to run VB Classifier
    UINT8                   RunObjsTR[AMBANET_ODV37_TR_BATCH_SZ]; ///< The objs list to run AmbaTracker
} AmbaPcptCfgNext;


#endif //CVAPI_SVCCVALGO_AMBAPERCEPTION__H

