/**
 *  @file SvcVinTree.h
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
 *  @details svc vin tree header file
 *
 */

#ifndef SVC_VIN_TREE_H
#define SVC_VIN_TREE_H

#include "AmbaTypes.h"
#include "SvcMem.h"

#define SVC_VIN_TREE_NODE_TYPE_VIN          (0U)
#define SVC_VIN_TREE_NODE_TYPE_DEC          (1U)
#define SVC_VIN_TREE_NODE_TYPE_BRIDGE       (2U)
#define SVC_VIN_TREE_NODE_TYPE_SENSOR       (3U)
#define SVC_VIN_TREE_NODE_TYPE_MEM          (4U)
#define SVC_VIN_TREE_NODE_TYPE_FOV          (5U)
#define SVC_VIN_TREE_NODE_TYPE_DMYENSOR     (6U)    /* Dummy-sensor for raw-enc */
#define SVC_VIN_TREE_NODE_TYPE_ENTRY        (255U)

#define SVC_VIN_TREE_NODE_VALUE_SENSOR      (0U)
#define SVC_VIN_TREE_NODE_VALUE_YUV         (1U)
#define SVC_VIN_TREE_NODE_VALUE_DMYSENSOR   (2U)

#define SVC_VIN_TREE_NODE_VALUE_FOV_BYPASS  (1U)

#define SVC_VIN_TREE_NODE_VALUE_MEM_RAW     (0U)
#define SVC_VIN_TREE_NODE_VALUE_MEM_422     (1U)
#define SVC_VIN_TREE_NODE_VALUE_MEM_420     (2U)

#define SVC_VIN_TREE_NODE_NAME          (8U)
#define SVC_VIN_TREE_NODE_LEAF_NUM_MAX  (16U)

#define SVC_VIN_TREE_QUEUE_SIZE         (128U)
#define SVC_VIN_TREE_QUEUE_CHAR_SIZE    (16U)
#define SVC_VIN_TREE_QUEUE_RESULT_OK    (0U)
#define SVC_VIN_TREE_QUEUE_RESULT_CONT  (1U)

#define SVC_VIN_TREE_STACK_SIZE         (64U)

#define SVC_VIN_TREE_NODE_MAX_NUM       (128U)

#define SVC_VIN_TREE_BRIDGE_CHECK       (1U)

typedef union /* _SVC_VIN_TREE_VALUE_u_ */ {
    UINT64 Data;
    struct {
        UINT32 VinID;
        UINT32 Reserved;
    } Vin;
    struct {
        UINT32 DecID;
        UINT32 Reserved;
    } Dec;
    struct {
        UINT32 FovIdx;
        UINT32 Attribute;
    } Fov;
    struct {
        UINT32 BridgeIdx;
        UINT32 SensorIdx;
    } Bridge;
    struct {
        UINT16 SensorIdx;
        UINT16 SensorOrYuv;
        UINT32 SensorID;
    } Sensor;   /* Sensor struct is used by both Sensor and YuvInput */
    struct {
        UINT16 MemIdx;
        UINT16 Type;
        UINT32 Reserved;
    } Mem;
} SVC_VIN_TREE_VALUE_u;

typedef struct /* _SVC_VIN_TREE_CONTENT_s_ */ {
    UINT8                Type;
    SVC_VIN_TREE_VALUE_u Value;
} SVC_VIN_TREE_CONTENT_s;

typedef struct SVC_VIN_TREE_NODE {
    SVC_VIN_TREE_CONTENT_s      Content;
    UINT8                       Visited;
    char                        Name[SVC_VIN_TREE_NODE_NAME];
    struct SVC_VIN_TREE_NODE    *pPrevNode;
    struct SVC_VIN_TREE_NODE    *pNextNode[SVC_VIN_TREE_NODE_LEAF_NUM_MAX];
} SVC_VIN_TREE_NODE_s;

typedef struct /* _SVC_VIN_TREE_QUEUE_s_ */ {
    char   Word[SVC_VIN_TREE_QUEUE_SIZE][SVC_VIN_TREE_QUEUE_CHAR_SIZE];
    UINT32 WrIdx;
    UINT32 RdIdx;
} SVC_VIN_TREE_QUEUE_s;

typedef UINT32 (* SVC_VIN_TREE_QUEUE_FUNC)(const char *pVinTree, UINT32 *pRdIdx);
typedef SVC_VIN_TREE_NODE_s* (* SVC_VIN_TREE_PARSING_FUNC)(const char *pWord, UINT32 *pRdIdx, SVC_VIN_TREE_NODE_s *pInputNode, UINT32 InputChan, UINT32 *pOutputChan);
typedef UINT32 (* SVC_VIN_TREE_BRIDGE_FUNC)(SVC_VIN_TREE_NODE_s *pInputNode);

typedef struct /* _SVC_VIN_TREE_QUEUE_PROC_s_ */ {
    char                    Word[1];
    UINT8                   WordNum;
    SVC_VIN_TREE_QUEUE_FUNC pFuncEntry;
} SVC_VIN_TREE_QUEUE_PROC_s;

typedef struct /* _SVC_VIN_TREE_PARSING_PROC_s_ */ {
    char                      Word[8];
    UINT8                     WordNum;
    SVC_VIN_TREE_PARSING_FUNC pFuncEntry;
} SVC_VIN_TREE_PARSING_PROC_s;

typedef struct /* _SVC_VIN_TREE_BRIDGE_s_ */ {
    char                     Word[16];
    UINT8                    WordNum;
    SVC_VIN_TREE_BRIDGE_FUNC pFuncEntry;
} SVC_VIN_TREE_BRIDGE_s;

typedef struct /* _SVC_VIN_TREE_NODE_CONTROL_s_ */ {
    SVC_VIN_TREE_NODE_s Nodes[SVC_VIN_TREE_NODE_MAX_NUM];
    UINT32              UsedNum;
} SVC_VIN_TREE_NODE_CONTROL_s;

typedef struct /* _SVC_VIN_TREE_STACK_s_ */ {
    SVC_VIN_TREE_NODE_s *pNode[SVC_VIN_TREE_STACK_SIZE];
    UINT32              ItemNum;
} SVC_VIN_TREE_STACK_s;

UINT32 SvcVinTree_Init(void);
UINT32 SvcVinTree_Create(const char *pVinTree);
SVC_VIN_TREE_NODE_s* SvcVinTree_Search(const char *pNodeName, UINT32 Length);
SVC_VIN_TREE_NODE_s* SvcVinTree_SearchVin(UINT32 VinIdx);
SVC_VIN_TREE_NODE_s* SvcVinTree_SearchSensor(UINT32 VinIdx, UINT32 SensorIdx);   /* SensorID may be different to SensorIdx */
SVC_VIN_TREE_NODE_s* SvcVinTree_SearchMem(UINT32 MemIdx);
SVC_VIN_TREE_NODE_s* SvcVinTree_SearchFov(UINT32 FovIdx);

UINT32 SvcVinTree_DebugEnable(UINT32 DebugEnable);

#endif /* SVC_VIN_TREE_H */
