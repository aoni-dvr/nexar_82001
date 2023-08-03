/**
 *  @file SvcVinTree.c
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

#include "AmbaTypes.h"
#include "AmbaWrap.h"
#include "AmbaMisraFix.h"
#include "AmbaUtility.h"
#include "AmbaKAL.h"

#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcMem.h"
#include "SvcBuffer.h"

#include "SvcVinTree.h"

#define SVC_VIN_TREE_PARSING_FORMAT_NUM       (6U)
#define SVC_VIN_TREE_PARSING_SUPPORT_WORD_NUM (8U)
#define SVC_VIN_TREE_PARSING_BRIDGE_NUM       (16U)

/* Internal function for generating queue */
static UINT32 VinTree_ParsingQueue(const char *pVinTree, UINT32 *pRdIdx);
static UINT32 VinTree_ParsingQueuePut(const char *pVinTree, UINT32 *pRdIdx);
static UINT32 VinTree_ParsingQueuePutDone(const char *pVinTree, UINT32 *pRdIdx);
static UINT32 VinTree_ParsingQueueStop(const char *pVinTree, UINT32 *pRdIdx);
static UINT32 VinTree_ParsingQueueSpace(const char *pVinTree, UINT32 *pRdIdx);
static UINT32 VinTree_ParsingQueueTab(const char *pVinTree, UINT32 *pRdIdx);

/* Internal function for generating tree */
static SVC_VIN_TREE_NODE_s* VinTree_ParsingNodeVin(const char *pWord, UINT32 *pRdIdx, SVC_VIN_TREE_NODE_s *pInputNode, UINT32 InputChan, UINT32 *pOutputChan);
static SVC_VIN_TREE_NODE_s* VinTree_ParsingNodeDecoder(const char *pWord, UINT32 *pRdIdx, SVC_VIN_TREE_NODE_s *pInputNode, UINT32 InputChan, UINT32 *pOutputChan);
static SVC_VIN_TREE_NODE_s* VinTree_ParsingNodeBridge(const char *pWord, UINT32 *pRdIdx, SVC_VIN_TREE_NODE_s *pInputNode, UINT32 InputChan, UINT32 *pOutputChan);
static SVC_VIN_TREE_NODE_s* VinTree_ParsingNodeSensor(const char *pWord, UINT32 *pRdIdx, SVC_VIN_TREE_NODE_s *pInputNode, UINT32 InputChan, UINT32 *pOutputChan);
static SVC_VIN_TREE_NODE_s* VinTree_ParsingNodeYuv(const char *pWord, UINT32 *pRdIdx, SVC_VIN_TREE_NODE_s *pInputNode, UINT32 InputChan, UINT32 *pOutputChan);
static SVC_VIN_TREE_NODE_s* VinTree_ParsingNodeFov(const char *pWord, UINT32 *pRdIdx, SVC_VIN_TREE_NODE_s *pInputNode, UINT32 InputChan, UINT32 *pOutputChan);
static SVC_VIN_TREE_NODE_s* VinTree_ParsingNodeMem(const char *pWord, UINT32 *pRdIdx, SVC_VIN_TREE_NODE_s *pInputNode, UINT32 InputChan, UINT32 *pOutputChan);
static SVC_VIN_TREE_NODE_s* VinTree_ParsingNodeDmy(const char *pWord, UINT32 *pRdIdx, SVC_VIN_TREE_NODE_s *pInputNode, UINT32 InputChan, UINT32 *pOutputChan);

/* Internal function for generating sensor ID based on different bridge */
static UINT32               VinTree_GenSensorIDB6N(SVC_VIN_TREE_NODE_s *pInputNode);
static UINT32               VinTree_GenSensorIDB6F(SVC_VIN_TREE_NODE_s *pInputNode);
static UINT32               VinTree_GenSensorIDMaxim96712(SVC_VIN_TREE_NODE_s *pInputNode);
static UINT32               VinTree_GenSensorIDMaxim96722(SVC_VIN_TREE_NODE_s *pInputNode);
static UINT32               VinTree_GenSensorIDMaxim9296(SVC_VIN_TREE_NODE_s *pInputNode);
static UINT32               VinTree_GenSensorIDMaxim9295(SVC_VIN_TREE_NODE_s *pInputNode);
static UINT32               VinTree_GenSensorIDMaxim96707(SVC_VIN_TREE_NODE_s *pInputNode);
static UINT32               VinTree_GenSensorIDMaxim96705(SVC_VIN_TREE_NODE_s *pInputNode);
static UINT32               VinTree_GenSensorIDMaxim9286(SVC_VIN_TREE_NODE_s *pInputNode);
static UINT32               VinTree_GenSensorIDMaxim9288(SVC_VIN_TREE_NODE_s *pInputNode);
static UINT32               VinTree_GenSensorIDMaxim96717(SVC_VIN_TREE_NODE_s *pInputNode);
static UINT32               VinTree_GenSensorIDMaxim96716(SVC_VIN_TREE_NODE_s *pInputNode);
static UINT32               VinTree_GenSensorIDTi960(SVC_VIN_TREE_NODE_s *pInputNode);
static UINT32               VinTree_GenSensorIDTi953(SVC_VIN_TREE_NODE_s *pInputNode);
static UINT32               VinTree_GenSensorIDTi954(SVC_VIN_TREE_NODE_s *pInputNode);
static UINT32               VinTree_GenSensorIDTi913(SVC_VIN_TREE_NODE_s *pInputNode);


/* Internal function for searching tree */
static SVC_VIN_TREE_NODE_s* VinTree_SearchByName(const char *pNodeName, UINT32 Length);
static SVC_VIN_TREE_NODE_s* VinTree_SearchByContent(const SVC_VIN_TREE_CONTENT_s *pContent, SVC_VIN_TREE_NODE_s *pNodeEntry);

/* Internal function for control queue and node */
static SVC_VIN_TREE_NODE_s* VinTree_CreateNode(void);
static UINT32               VinTree_CleanTree(void);
static UINT32               VinTree_CleanQueue(void);
static UINT32               VinTree_CleanVisited(void);

/* Internal function for control searching stack */
static UINT32               VinTree_StackPut(SVC_VIN_TREE_NODE_s *pNode);
static SVC_VIN_TREE_NODE_s* VinTree_StackPop(void);
static UINT32               VinTree_StackFlush(void);
static UINT32               VinTree_StackEmpty(void);

/* Debug print function */
static void                 VinTree_Dbg(const char *pModule, const char *pFormat, UINT32 Arg1, UINT32 Arg2);
static void                 VinTree_Err(const char *pModule, const char *pFormat, UINT32 Arg1, UINT32 Arg2);

static SVC_VIN_TREE_BRIDGE_s SvcVinTreeBridge[SVC_VIN_TREE_PARSING_BRIDGE_NUM] = {
    { "B6N",        3,  VinTree_GenSensorIDB6N         },
    { "B6F",        3,  VinTree_GenSensorIDB6F         },
    { "MAXIM96712", 10, VinTree_GenSensorIDMaxim96712  },
    { "MAXIM96722", 10, VinTree_GenSensorIDMaxim96722  },
    { "MAXIM9295",  9,  VinTree_GenSensorIDMaxim9295   },
    { "MAXIM9296",  9,  VinTree_GenSensorIDMaxim9296   },
    { "MAXIM96707", 10, VinTree_GenSensorIDMaxim96707  },
    { "MAXIM96705", 10, VinTree_GenSensorIDMaxim96705  },
    { "MAXIM9286",  9,  VinTree_GenSensorIDMaxim9286   },
    { "MAXIM9288",  9,  VinTree_GenSensorIDMaxim9288   },
    { "MAXIM96717", 10, VinTree_GenSensorIDMaxim96717  },
    { "MAXIM96716", 10, VinTree_GenSensorIDMaxim96716  },
    { "TI960",      5,  VinTree_GenSensorIDTi960       },
    { "TI953",      5,  VinTree_GenSensorIDTi953       },
    { "TI954",      5,  VinTree_GenSensorIDTi954       },
    { "TI913",      5,  VinTree_GenSensorIDTi913       },
};

static SVC_VIN_TREE_NODE_CONTROL_s VinTreeNodeCtrl GNU_SECTION_NOZEROINIT;
static SVC_VIN_TREE_QUEUE_s ParsingQueue;   /* A queue which depth is 20 and can contain maximum 10 character */
static SVC_VIN_TREE_NODE_s *pVinTreeEntry;
static SVC_VIN_TREE_STACK_s SearchingStack GNU_SECTION_NOZEROINIT;

static UINT32 VinTreeDebugEnable = 0U;
static AMBA_KAL_MUTEX_t VinTreeMtx;

#define SVC_LOG_VIN_TREE "VIN_TREE"

/**
 * Initialize the needed resource to create a tree
 * return 0-OK, 1-NG
 */
UINT32 SvcVinTree_Init(void)
{
    static UINT32 VinTreeMtxCreated = 0U;
    UINT32 RetVal;
    static char     MutexName[] = "SvcVinTreeMutex";

    RetVal = VinTree_CleanQueue();

    if (SVC_OK != RetVal) {
        VinTree_Err(SVC_LOG_VIN_TREE, "VinTree_CleanQueue() failed with %d", RetVal, 0U);
    }

    if (SVC_OK == RetVal) {
        RetVal = AmbaWrap_memset(&VinTreeNodeCtrl, 0, sizeof(SVC_VIN_TREE_NODE_CONTROL_s));
        if (SVC_OK != RetVal) {
            VinTree_Err(SVC_LOG_VIN_TREE, "VinTreeNodeCtrl memset 0 failed with %d", RetVal, 0U);
        }
    }

    if (SVC_OK == RetVal) {
        RetVal = AmbaWrap_memset(&SearchingStack, 0, sizeof(SVC_VIN_TREE_STACK_s));
        if (SVC_OK != RetVal) {
            VinTree_Err(SVC_LOG_VIN_TREE, "SearchingStack memset 0 failed with %d", RetVal, 0U);
        }
    }


    if (SVC_OK == RetVal) {
        RetVal = VinTree_CleanTree();
        if (SVC_OK != RetVal) {
            VinTree_Err(SVC_LOG_VIN_TREE, "VinTree_CleanTree() failed with %d", RetVal, 0U);
        }
    }

    if (SVC_OK == RetVal) {
        const char EntryStr[8U] = "ENTRY";
        /* Create Entry Point */
        pVinTreeEntry = VinTree_CreateNode();
        pVinTreeEntry->Content.Type = SVC_VIN_TREE_NODE_TYPE_ENTRY;
        RetVal = AmbaWrap_memcpy(pVinTreeEntry->Name, EntryStr, sizeof(char) * 5U);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_VIN_TREE, "SvcVinTree_Init() err with %d", RetVal, 0U);
        }
    }

    if (SVC_OK == RetVal) {
        if (VinTreeMtxCreated == 1U) {
            RetVal = AmbaKAL_MutexDelete(&VinTreeMtx);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_VIN_TREE, "Mutex delete failed", 0U, 0U);
            } else {
                VinTreeMtxCreated = 0U;
            }
        }

        RetVal = AmbaKAL_MutexCreate(&VinTreeMtx, MutexName);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_VIN_TREE, "Mutex create failed", 0U, 0U);
        } else {
            VinTreeMtxCreated = 1U;
        }
    }

    return RetVal;
}

/**
 * Create the vin tree
 * @param [in] pVinTree Input VinTree string to construct the VinTree
 * return 0-OK, 1-NG
 */
UINT32 SvcVinTree_Create(const char *pVinTree)
{
    static const SVC_VIN_TREE_QUEUE_PROC_s SvcVinTreeQueueProc[SVC_VIN_TREE_PARSING_FORMAT_NUM] = {
        { "$",    1,   VinTree_ParsingQueue        },  /* The key word to start generate queue */
        { "[",    1,   VinTree_ParsingQueuePut     },  /* The key word to put the next character into queue */
        { "]",    1,   VinTree_ParsingQueuePutDone },  /* The key word to stop putting */
        { "#",    1,   VinTree_ParsingQueueStop    },  /* The key word to end generate queue */
        { " ",    1,   VinTree_ParsingQueueSpace   },  /* Will do nothing and just by-pass */
        { "\t",   1,   VinTree_ParsingQueueTab     },  /* Will do nothing and just by-pass*/
    };

    static const SVC_VIN_TREE_PARSING_PROC_s SvcVinTreeParsingProc[SVC_VIN_TREE_PARSING_SUPPORT_WORD_NUM] = {
        { "VIN_", 4,   VinTree_ParsingNodeVin     },  /* Node VIN */
        { "DEC_", 4,   VinTree_ParsingNodeDecoder },  /* Node Decoder */
        { "B_",   2,   VinTree_ParsingNodeBridge  },  /* Node Bridge */
        { "S_",   2,   VinTree_ParsingNodeSensor  },  /* Node Sensor */
        { "Y_",   2,   VinTree_ParsingNodeYuv     },  /* Node Yuv Input */
        { "M_",   2,   VinTree_ParsingNodeMem     },  /* Node Memory Input, usally for raw encode function */
        { "FOV_", 4,   VinTree_ParsingNodeFov     },  /* Node Fov */
        { "DMY_", 4,   VinTree_ParsingNodeDmy     },  /* Node Dummy sensor, for raw encode */
    };

    const char *pSubVinTree, *pSubQueueStr;
    UINT32 RdIdx = 0;
    UINT32 WrapRval;
    UINT32 RetVal;

    UINT32 i;
    UINT32 QueueResult, TreeCreateDone = 0U;
    SVC_VIN_TREE_NODE_s *pOutputNode, *pInputNode;

    while (TreeCreateDone == 0U) {
        UINT32 InputChan = 0U, OutputChan = 0U;
        const char ExclamationMarkStr[2U] = "!";

        pOutputNode = NULL;
        pInputNode = pVinTreeEntry;
        QueueResult = SVC_VIN_TREE_QUEUE_RESULT_CONT;

        pSubVinTree = &pVinTree[RdIdx];
        /* Check whether parsing end */
        RetVal = AmbaWrap_memcmp(pSubVinTree, ExclamationMarkStr, sizeof(char) * 1U, &WrapRval);
        if (SVC_OK == WrapRval) {
            VinTree_Dbg(SVC_LOG_VIN_TREE, "Find \"!\" End of VinTreeStr", 0U, 0U);
            TreeCreateDone = 1;
        }

        if (TreeCreateDone == 0U) {
            /* Generate queue */
            while (QueueResult != SVC_VIN_TREE_QUEUE_RESULT_OK) {
                /* Each parsing step started address */
                pSubVinTree = &pVinTree[RdIdx];
                for (i = 0; i < SVC_VIN_TREE_PARSING_FORMAT_NUM; i++) {
                    RetVal = AmbaWrap_memcmp(pSubVinTree, SvcVinTreeQueueProc[i].Word, sizeof(char) * SvcVinTreeQueueProc[i].WordNum, &WrapRval);
                    if (SVC_OK == WrapRval) {
                        QueueResult = SvcVinTreeQueueProc[i].pFuncEntry(pVinTree, &RdIdx);
                        break;
                    }
                }
            }

            /* Compare the key word in queue */
            while (ParsingQueue.WrIdx != ParsingQueue.RdIdx) {
                UINT32 QueueRdIdx = ParsingQueue.RdIdx;
                pSubQueueStr = ParsingQueue.Word[QueueRdIdx];
                for (i = 0; i < SVC_VIN_TREE_PARSING_SUPPORT_WORD_NUM; i++) {
                    /* If key word matching */
                    RetVal = AmbaWrap_memcmp(pSubQueueStr, SvcVinTreeParsingProc[i].Word, sizeof(char) * SvcVinTreeParsingProc[i].WordNum, &WrapRval);
                    if (SVC_OK == WrapRval) {
                        pOutputNode = SvcVinTreeParsingProc[i].pFuncEntry(ParsingQueue.Word[QueueRdIdx], &ParsingQueue.RdIdx, pInputNode, InputChan, &OutputChan);
                        pInputNode = pOutputNode;
                        InputChan = OutputChan;
                        break;
                    }
                }
            }
        }

        RetVal = VinTree_CleanQueue();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_VIN_TREE, "SvcVinTree_Create() Clean Queue err (%d)", RetVal, 0U);
        }
    }

    return SVC_OK;
}

/**
 * Search based on node name
 * @param [in] pNodeName The node name to be searched in VinTree
 * @param [in] Length The pNodeName length
 * return The searched node
 */
SVC_VIN_TREE_NODE_s* SvcVinTree_Search(const char *pNodeName, UINT32 Length)
{
    SVC_VIN_TREE_NODE_s* pNode;

    VinTree_Dbg(SVC_LOG_VIN_TREE, "SvcVinTree_Search() start", 0U, 0U);
    pNode = VinTree_SearchByName(pNodeName, Length);

    if (pNode == NULL) {
        VinTree_Err(SVC_LOG_VIN_TREE, "VinTree_SearchByName() failed with NULL", 0U, 0U);
    }

    VinTree_Dbg(SVC_LOG_VIN_TREE, "SvcVinTree_Search() done", 0U, 0U);

    return pNode;
}

/**
 * Search based on vin idx
 * @param [in] VinIdx The desired vin index
 * return The searched node
 */
SVC_VIN_TREE_NODE_s* SvcVinTree_SearchVin(UINT32 VinIdx)
{
    SVC_VIN_TREE_NODE_s* pNode;
    SVC_VIN_TREE_CONTENT_s InputContent = { .Type = SVC_VIN_TREE_NODE_TYPE_VIN , .Value = { .Vin = { .VinID = VinIdx, .Reserved = 0U } } };

    VinTree_Dbg(SVC_LOG_VIN_TREE, "SvcVinTree_SearchVin() start", 0U, 0U);

    pNode = VinTree_SearchByContent(&InputContent, pVinTreeEntry);
    if (pNode == NULL) {
        VinTree_Err(SVC_LOG_VIN_TREE, "SvcVinTree_Search() failed with NULL", 0U, 0U);
    }

    VinTree_Dbg(SVC_LOG_VIN_TREE, "SvcVinTree_SearchVin() done", 0U, 0U);

    return pNode;
}

/**
 * Search based on sensor idx
 * @param [in] VinIdx The desired vin index
 * @param [in] SensorIdx The desired sensor index
 * return The searched node
 */
SVC_VIN_TREE_NODE_s* SvcVinTree_SearchSensor(UINT32 VinIdx, UINT32 SensorIdx)
{
    SVC_VIN_TREE_NODE_s *pVinNode, *pSensorNode;
    SVC_VIN_TREE_CONTENT_s InputContentVin = {
        .Type = SVC_VIN_TREE_NODE_TYPE_VIN,
        .Value = { .Vin = { .VinID = VinIdx, .Reserved = 0U } }
    };

    SVC_VIN_TREE_CONTENT_s InputContentSensor = {
        .Type = SVC_VIN_TREE_NODE_TYPE_SENSOR,
        .Value = { .Sensor = { .SensorIdx = (UINT16) SensorIdx, .SensorOrYuv = 0U, .SensorID = 0U } } 
    };

    VinTree_Dbg(SVC_LOG_VIN_TREE, "SvcVinTree_SearchSensor() start", 0U, 0U);

    pVinNode = VinTree_SearchByContent(&InputContentVin, pVinTreeEntry);

    if (pVinNode != NULL) {
        pSensorNode = VinTree_SearchByContent(&InputContentSensor, pVinNode);
        if (pSensorNode == NULL) {
            VinTree_Err(SVC_LOG_VIN_TREE, "SvcVinTree_SearchSensor() failed with NULL", 0U, 0U);
        }
    } else {
        VinTree_Err(SVC_LOG_VIN_TREE, "SvcVinTree_SearchSensor() failed. Cannot find the corresponding VinNode.", 0U, 0U);
        pSensorNode = NULL;
    }

    VinTree_Dbg(SVC_LOG_VIN_TREE, "SvcVinTree_SearchSensor() done", 0U, 0U);

    /* Not support yet */
    return pSensorNode;
}

/**
 * Search based on memory idx
 * @param [in] MemIdx The desired memory index
 * return The searched node
 */

SVC_VIN_TREE_NODE_s* SvcVinTree_SearchMem(UINT32 MemIdx)
{
    SVC_VIN_TREE_NODE_s *pNode;

    SVC_VIN_TREE_CONTENT_s InputContent = {
        .Type = SVC_VIN_TREE_NODE_TYPE_MEM,
        .Value = { .Mem = { .MemIdx = (UINT16) MemIdx, .Type = 0U, .Reserved = 0U } } 
    };

    VinTree_Dbg(SVC_LOG_VIN_TREE, "SvcVinTree_SearchMem() start", 0U, 0U);

    pNode = VinTree_SearchByContent(&InputContent, pVinTreeEntry);
    if (pNode == NULL) {
        VinTree_Err(SVC_LOG_VIN_TREE, "SvcVinTree_SearchMem() failed with NULL", 0U, 0U);
    }

    VinTree_Dbg(SVC_LOG_VIN_TREE, "SvcVinTree_SearchMem() done", 0U, 0U);

    /* Not support yet */
    return pNode;
}

/**
 * Search based on fov idx
 * @param [in] FovIdx The desired Fov index
 * return The searched node
 */
SVC_VIN_TREE_NODE_s* SvcVinTree_SearchFov(UINT32 FovIdx)
{
    SVC_VIN_TREE_NODE_s* pNode;
    SVC_VIN_TREE_CONTENT_s InputContent = {
        .Type = SVC_VIN_TREE_NODE_TYPE_FOV,
        .Value = { .Fov = { .FovIdx = FovIdx, .Attribute = 0 } }
    };

    VinTree_Dbg(SVC_LOG_VIN_TREE, "SvcVinTree_SearchFov() start", 0U, 0U);

    pNode = VinTree_SearchByContent(&InputContent, pVinTreeEntry);
    if (pNode == NULL) {
        VinTree_Err(SVC_LOG_VIN_TREE, "SvcVinTree_SearchFov() failed with NULL", 0U, 0U);
    }

    VinTree_Dbg(SVC_LOG_VIN_TREE, "SvcVinTree_SearchFov() done", 0U, 0U);

    return pNode;
}

/**
 * Set debug enable flag
 * @param [in] DebugEnable (1)enable (0)disable
 * return 0-OK, 1-NG
 */
UINT32 SvcVinTree_DebugEnable(UINT32 DebugEnable)
{
    VinTreeDebugEnable = DebugEnable;
    SvcLog_OK(SVC_LOG_VIN_TREE, "DebugEnable = %d", DebugEnable, 0U);
    
    return SVC_OK;
}

static SVC_VIN_TREE_NODE_s* VinTree_SearchByName(const char *pNodeName, UINT32 Length)
{
    UINT32 FindNode = 0U, CannotFind = 0U;
    UINT32 WrapRval = SVC_NG;
    UINT32 RetVal;
    SVC_VIN_TREE_NODE_s *pNode = pVinTreeEntry;

    VinTree_Dbg(SVC_LOG_VIN_TREE, "VinTree_SearchByName() start", 0U, 0U);

    RetVal = AmbaKAL_MutexTake(&VinTreeMtx, AMBA_KAL_WAIT_FOREVER);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_VIN_TREE, "VinTree_SearchByName() take mutex failed", 0U, 0U);
    }

    /* Clean the stack before searching */
    if (SVC_OK == RetVal) {
        RetVal = VinTree_StackFlush();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_VIN_TREE, "VinTree_SearchByContent() Stack flush failed (%d) ",RetVal, 0U);
        }
    }

    /* Clean the node visited status before searching */
    if (SVC_OK == RetVal) {
        RetVal = VinTree_CleanVisited();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_VIN_TREE, "VinTree_SearchByContent() Clean node visit record failed (%d) ",RetVal, 0U);
        }
    }

    while ((FindNode == 0U) && (CannotFind == 0U) && (SVC_OK == RetVal)) {
        if (pNode != NULL) {
            VinTree_Dbg(pNode->Name, " searching ... Visited(%d)", pNode->Visited, 0U);
            if (pNode->Visited < 1U) {
                RetVal = VinTree_StackPut(pNode);
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_VIN_TREE, "VinTree_SearchByName() Stack put err with (%d)", RetVal, 0U);
                }
                RetVal = AmbaWrap_memcmp(pNodeName, pNode->Name, sizeof(char) * Length, &WrapRval);
                if ((SVC_OK == RetVal) && (SVC_OK == WrapRval)) {
                    FindNode = 1;
                    VinTree_Dbg(SVC_LOG_VIN_TREE, "FIND !!", 0U, 0U);
                }
            }
            if (FindNode != 1U) {
                UINT32 NextIdx = pNode->Visited;

                pNode->Visited = pNode->Visited + 1U;
                pNode = pNode->pNextNode[NextIdx];
            }
        } else {
            pNode = VinTree_StackPop();
        }

        if (SVC_OK == VinTree_StackEmpty()) {
            CannotFind = 1U;
        }
    }

    if (FindNode == 0U) {
        pNode = NULL;
    }

    VinTree_Dbg(SVC_LOG_VIN_TREE, "VinTree_SearchByName() done", 0U, 0U);

    if (SVC_OK != AmbaKAL_MutexGive(&VinTreeMtx)) {
        SvcLog_NG(SVC_LOG_VIN_TREE, "VinTree_SearchByName() give mutex failed", 0U, 0U);
    }

    return pNode;
}

static SVC_VIN_TREE_NODE_s* VinTree_SearchByContent(const SVC_VIN_TREE_CONTENT_s *pContent, SVC_VIN_TREE_NODE_s *pNodeEntry)
{
    UINT32 FindNode = 0U, CannotFind = 0U;
    UINT32 WrapRval = SVC_NG;
    UINT32 RetVal;
    SVC_VIN_TREE_NODE_s *pNode = pNodeEntry;

    VinTree_Dbg(SVC_LOG_VIN_TREE, "VinTree_SearchByContent() start", 0U, 0U);

    RetVal = AmbaKAL_MutexTake(&VinTreeMtx, AMBA_KAL_WAIT_FOREVER);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_VIN_TREE, "VinTree_SearchByName() take mutex failed", 0U, 0U);
    }

    /* Clean the stack before searching */
    if (SVC_OK == RetVal) {
        RetVal = VinTree_StackFlush();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_VIN_TREE, "VinTree_SearchByContent() Stack flush failed (%d) ",RetVal, 0U);
        }
    }

    /* Clean the node visited status before searching */
    if (SVC_OK == RetVal) {
        RetVal = VinTree_CleanVisited();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_VIN_TREE, "VinTree_SearchByContent() Clean node visit record failed (%d) ",RetVal, 0U);
        }
    }

    while ((FindNode == 0U) && (CannotFind == 0U) && (SVC_OK == RetVal)) {
        if (pNode != NULL) {
            VinTree_Dbg(pNode->Name, " searching ... Visited(%d)", pNode->Visited, 0U);
            if (pNode->Visited < 1U) {
                RetVal = VinTree_StackPut(pNode);
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_VIN_TREE, "VinTree_SearchByContent() Stack put failed (%d) ",RetVal, 0U);
                }

                if (pContent->Type == SVC_VIN_TREE_NODE_TYPE_SENSOR) {
                    /* For Sensor case, only compare sensor idx */
                    if ((pContent->Type == pNode->Content.Type) && (pContent->Value.Sensor.SensorIdx == pNode->Content.Value.Sensor.SensorIdx)) {
                        WrapRval = SVC_OK;
                    }
                } else if (pContent->Type == SVC_VIN_TREE_NODE_TYPE_MEM) {
                    /* For Sensor case, only compare mem idx */
                    if ((pContent->Type == pNode->Content.Type) && (pContent->Value.Mem.MemIdx == pNode->Content.Value.Mem.MemIdx)) {
                        WrapRval = SVC_OK;
                    }
                } else if (pContent->Type == SVC_VIN_TREE_NODE_TYPE_FOV) {
                    if ((pContent->Type == pNode->Content.Type) && (pContent->Value.Fov.FovIdx == pNode->Content.Value.Fov.FovIdx)) {
                        WrapRval = SVC_OK;
                    }
                } else {
                    if ((pNode->Content.Type == pContent->Type) && (pNode->Content.Value.Data == pContent->Value.Data)) {
                        WrapRval = SVC_OK;
                    }
                }

                if (SVC_OK == WrapRval) {
                    FindNode = 1U;
                    VinTree_Dbg(SVC_LOG_VIN_TREE, "FIND !!", 0U, 0U);
                }
            }
            if (FindNode != 1U) {
                UINT32 NextIdx = pNode->Visited;

                pNode->Visited = pNode->Visited + 1U;
                pNode = pNode->pNextNode[NextIdx];
            }
        } else {
            pNode = VinTree_StackPop();
        }

        if (SVC_OK == VinTree_StackEmpty()) {
            CannotFind = 1U;
        }
    }

    if (FindNode == 0U) {
        pNode = NULL;
    }

    VinTree_Dbg(SVC_LOG_VIN_TREE, "VinTree_SearchByContent() done", 0U, 0U);

    if (SVC_OK != AmbaKAL_MutexGive(&VinTreeMtx)) {
        SvcLog_NG(SVC_LOG_VIN_TREE, "VinTree_SearchByName() give mutex failed", 0U, 0U);
    }

    return pNode;
}

static UINT32 VinTree_ParsingQueue(const char *pVinTree, UINT32 *pRdIdx)
{
    UINT32 RdIdx = *pRdIdx;
    /* Do nothing */
    VinTree_Dbg(SVC_LOG_VIN_TREE, pVinTree, 0U, 0U);
    VinTree_Dbg(SVC_LOG_VIN_TREE, "Find \"$\" Queue generating start", 0U, 0U);
    RdIdx = RdIdx + 1U;

    /* Update read index */
    *pRdIdx = RdIdx;

    return SVC_VIN_TREE_QUEUE_RESULT_CONT;
}

static UINT32 VinTree_ParsingQueuePut(const char *pVinTree, UINT32 *pRdIdx)
{
    UINT32 RdIdx = *pRdIdx;
    UINT32 QueueWrIdx = ParsingQueue.WrIdx;
    const char *pSubVinTree;
    const char RightBracketStr[2U] = "]";
    UINT8 NodeStrSize = 0U;
    UINT8 i;

    UINT32 RetVal;
    UINT32 WrapRval;

    VinTree_Dbg(SVC_LOG_VIN_TREE, pVinTree, 0U, 0U);
    VinTree_Dbg(SVC_LOG_VIN_TREE, "Find \"[\" Queue put", 0U, 0U);
    RdIdx = RdIdx + 1U;

    pSubVinTree = &pVinTree[RdIdx];
    for (i = 0; i < SVC_VIN_TREE_QUEUE_CHAR_SIZE; i++) {
        RetVal = AmbaWrap_memcmp(&pSubVinTree[i], RightBracketStr, sizeof(char) * 1U, &WrapRval);
        if ((SVC_OK == RetVal) && (SVC_OK == WrapRval)) {
            /* put into queue */
            RetVal = AmbaWrap_memcpy(&ParsingQueue.Word[QueueWrIdx], pSubVinTree, sizeof(char) * NodeStrSize);
            if (SVC_OK == RetVal) {
                ParsingQueue.Word[QueueWrIdx][NodeStrSize] = '\0';
                QueueWrIdx = QueueWrIdx + 1U;

                ParsingQueue.WrIdx = QueueWrIdx;
            }
            break;
        } else {
            NodeStrSize++;
        }
    }
    RdIdx = RdIdx + NodeStrSize;

    /* Update read index */
    *pRdIdx = RdIdx;

    return SVC_VIN_TREE_QUEUE_RESULT_CONT;

}

static UINT32 VinTree_ParsingQueuePutDone(const char *pVinTree, UINT32 *pRdIdx)
{
    UINT32 RdIdx = *pRdIdx;

    VinTree_Dbg(SVC_LOG_VIN_TREE, pVinTree, 0U, 0U);
    VinTree_Dbg(SVC_LOG_VIN_TREE, "Find \"]\" Queue put done", 0U, 0U);

    RdIdx = RdIdx + 1U;

    /* Update read index */
    *pRdIdx = RdIdx;

    return SVC_VIN_TREE_QUEUE_RESULT_CONT;
}

static UINT32 VinTree_ParsingQueueStop(const char *pVinTree, UINT32 *pRdIdx)
{
    UINT32 RdIdx = *pRdIdx;

    VinTree_Dbg(SVC_LOG_VIN_TREE, pVinTree, 0U, 0U);
    VinTree_Dbg(SVC_LOG_VIN_TREE, "Find \"#\" Queue generating stop", 0U, 0U);

    RdIdx = RdIdx + 1U;

    /* Update read index */
    *pRdIdx = RdIdx;

    return SVC_VIN_TREE_QUEUE_RESULT_OK;
}

static UINT32 VinTree_ParsingQueueSpace(const char *pVinTree, UINT32 *pRdIdx)
{
    UINT32 RdIdx = *pRdIdx;
    UINT32 SpaceSkip = 0U;
    const char SpaceStr[2U] = " ";

    UINT32 WrapRval;
    UINT32 RetVal;

    VinTree_Dbg(SVC_LOG_VIN_TREE, "Find \" \" By-pass", 0U, 0U);

    while (SpaceSkip == 0U) {
        RetVal = AmbaWrap_memcmp(&pVinTree[RdIdx], SpaceStr, sizeof(char) * 1U, &WrapRval);
        if ((SVC_OK == RetVal) && (SVC_OK != WrapRval)) {
            SpaceSkip = 1U;
        } else {
            RdIdx = RdIdx + 1U;
        }
    }

    /* Update read index */
    *pRdIdx = RdIdx;

    return SVC_VIN_TREE_QUEUE_RESULT_CONT;

}

static UINT32 VinTree_ParsingQueueTab(const char *pVinTree, UINT32 *pRdIdx)
{
    UINT32 RdIdx = *pRdIdx;
    UINT32 TabSkip = 0U;
    const char TabStr[2U] = "\t";

    UINT32 WrapRval;
    UINT32 RetVal;

    VinTree_Dbg(SVC_LOG_VIN_TREE, "Find \"\t\" By-pass", 0U, 0U);

    while (TabSkip == 0U) {
        RetVal = AmbaWrap_memcmp(&pVinTree[RdIdx], TabStr, sizeof(char) * 1U, &WrapRval);
        if ((SVC_OK == RetVal) && (SVC_OK != WrapRval)) {
            TabSkip = 1U;
        } else {
            RdIdx = RdIdx + 1U;
        }
    }

    /* Update read index */
    *pRdIdx = RdIdx;

    return SVC_VIN_TREE_QUEUE_RESULT_CONT;

}

static SVC_VIN_TREE_NODE_s* VinTree_ParsingNodeVin(const char *pWord, UINT32 *pRdIdx, SVC_VIN_TREE_NODE_s *pInputNode, UINT32 InputChan, UINT32 *pOutputChan)
{
    UINT32 RdIdx = *pRdIdx;
    UINT32 VinID = 0U;
    const char *pVinIDStr = &pWord[4];
    SVC_VIN_TREE_NODE_s *pNode = NULL;
    UINT32 Idx = 0U;

    UINT32 RetVal;

    AmbaMisra_TouchUnused(&InputChan);

    VinTree_Dbg(SVC_LOG_VIN_TREE, "VinTree_ParsingNodeVin() Start", 0U, 0U);

    if (SVC_OK != SvcWrap_strtoul(pVinIDStr, &VinID)) {
        VinTree_Err(SVC_LOG_VIN_TREE, "VinTree_ParsingNodeVin() SvcWrap_strtoul() failed", 0U, 0U);
    }

    /* AnyWay create a node */
    for (Idx = 0U; Idx < SVC_VIN_TREE_NODE_LEAF_NUM_MAX; Idx++) {
        if (pInputNode->pNextNode[Idx] != NULL) {
            if ((pInputNode->pNextNode[Idx]->Content.Type == SVC_VIN_TREE_NODE_TYPE_VIN) && (pInputNode->pNextNode[Idx]->Content.Value.Vin.VinID == VinID)) {
                pNode = pInputNode->pNextNode[Idx];
            }
        } else {
            pNode = VinTree_CreateNode();
            if (pNode != NULL) {
                pNode->Content.Type = SVC_VIN_TREE_NODE_TYPE_VIN;
                pNode->Content.Value.Vin.VinID = VinID;
                RetVal = AmbaWrap_memcpy(pNode->Name, pWord, sizeof(char) * 5U);
                if (SVC_OK != RetVal) {
                    VinTree_Err(SVC_LOG_VIN_TREE, "VinTree_ParsingNodeVin() SvcWrap_strtoul() failed", 0U, 0U);
                }

                /* Link the vin tree. pInputNode should only be pVinTreeEntry */
                pInputNode->pNextNode[Idx] = pNode;
                pNode->pPrevNode = pInputNode;
            }
        }
        if (pNode != NULL) {
            break;
        }
    }

    if (pNode == NULL) {
        VinTree_Err(SVC_LOG_VIN_TREE, "VinTree cannot hook vin node", 0U, 0U);
    }

    RdIdx = RdIdx + 1U;

    /* Update queue item num */
    *pRdIdx = RdIdx;

    /* Output the pOutputChan */
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
    *pOutputChan = 0U;
#else
    *pOutputChan = VinID;
#endif

    VinTree_Dbg(SVC_LOG_VIN_TREE, "VinTree_ParsingNodeVin() Done. Pass Linker %d", VinID, 0U);

    return pNode;
}

static SVC_VIN_TREE_NODE_s* VinTree_ParsingNodeDecoder(const char *pWord, UINT32 *pRdIdx, SVC_VIN_TREE_NODE_s *pInputNode, UINT32 InputChan, UINT32 *pOutputChan)
{
    UINT32 RdIdx = *pRdIdx;
    UINT32 DecID = 0U;
    const char *pDecIDStr = &pWord[4];
    SVC_VIN_TREE_NODE_s *pNode = NULL;
    UINT32 Idx = 0U;

    UINT32 RetVal;

    AmbaMisra_TouchUnused(&InputChan);

    VinTree_Dbg(SVC_LOG_VIN_TREE, "VinTree_ParsingNodeDecoder() Start", 0U, 0U);

    if (SVC_OK != SvcWrap_strtoul(pDecIDStr, &DecID)) {
        VinTree_Err(SVC_LOG_VIN_TREE, "VinTree_ParsingNodeDecoder() SvcWrap_strtoul() failed", 0U, 0U);
    }

    /* AnyWay create a node */
    for (Idx = 0U; Idx < SVC_VIN_TREE_NODE_LEAF_NUM_MAX; Idx++) {
        if (pInputNode->pNextNode[Idx] != NULL) {
            if ((pInputNode->pNextNode[Idx]->Content.Type == SVC_VIN_TREE_NODE_TYPE_DEC) && (pInputNode->pNextNode[Idx]->Content.Value.Dec.DecID == DecID)) {
                pNode = pInputNode->pNextNode[Idx];
            }
        } else {
            pNode = VinTree_CreateNode();
            if (pNode != NULL) {
                pNode->Content.Type = SVC_VIN_TREE_NODE_TYPE_DEC;
                pNode->Content.Value.Dec.DecID = DecID;
                RetVal = AmbaWrap_memcpy(pNode->Name, pWord, sizeof(char) * 5U);
                if (SVC_OK != RetVal) {
                    VinTree_Err(SVC_LOG_VIN_TREE, "VinTree_ParsingNodeDecoder() SvcWrap_strtoul() failed", 0U, 0U);
                }

                /* Link the vin tree. pInputNode should only be pVinTreeEntry */
                pInputNode->pNextNode[Idx] = pNode;
                pNode->pPrevNode = pInputNode;
            }
        }
        if (pNode != NULL) {
            break;
        }
    }

    if (pNode == NULL) {
        VinTree_Err(SVC_LOG_VIN_TREE, "VinTree cannot hook decoder node", 0U, 0U);
    }

    RdIdx = RdIdx + 1U;

    /* Update queue item num */
    *pRdIdx = RdIdx;

    /* Output the pOutputChan */
    *pOutputChan = DecID;

    VinTree_Dbg(SVC_LOG_VIN_TREE, "VinTree_ParsingNodeDecoder() Done. Pass Linker %d", DecID, 0U);

    return pNode;
}

static SVC_VIN_TREE_NODE_s* VinTree_ParsingNodeBridge(const char *pWord, UINT32 *pRdIdx, SVC_VIN_TREE_NODE_s *pInputNode, UINT32 InputChan, UINT32 *pOutputChan)
{
    UINT32 RdIdx = *pRdIdx;
    const char *pBridgeName = &pWord[2];  /* Becase B_xxx_y. Where xxx is bridge name, y is chan */
    const char *pChanStr;
    UINT32 Chan = 0U;
    UINT32 i;
    SVC_VIN_TREE_NODE_s *pNode = NULL;

    UINT32 WrapRval;
    UINT32 RetVal;

    VinTree_Dbg(SVC_LOG_VIN_TREE, "VinTree_ParsingNodeBridge() Start", 0U, 0U);

    for (i = 0U; i < SVC_VIN_TREE_PARSING_BRIDGE_NUM; i++) {
        RetVal = AmbaWrap_memcmp(SvcVinTreeBridge[i].Word, pBridgeName, sizeof(char) * SvcVinTreeBridge[i].WordNum, &WrapRval);
        if ((SVC_OK == RetVal) && (SVC_OK == WrapRval)) {

            pChanStr = &pWord[2U + SvcVinTreeBridge[i].WordNum + 1U];
            RetVal = SvcWrap_strtoul(pChanStr, &Chan);
            if (SVC_OK != RetVal) {
                VinTree_Err(SVC_LOG_VIN_TREE, "VinTree_ParsingNodeBridge() SvcWrap_strtoul() failed", 0U, 0U);
            }

            if (pInputNode->pNextNode[InputChan] == NULL) {
                pNode = VinTree_CreateNode();
                if (pNode != NULL) {
                    pNode->Content.Type = SVC_VIN_TREE_NODE_TYPE_BRIDGE;
                    if (pInputNode->Content.Type == SVC_VIN_TREE_NODE_TYPE_BRIDGE) {
                        pNode->Content.Value.Bridge.BridgeIdx = InputChan;
                    }
                    RetVal = AmbaWrap_memcpy(pNode->Name, SvcVinTreeBridge[i].Word, sizeof(char) * SvcVinTreeBridge[i].WordNum);
                    if (SVC_OK != RetVal) {
                        VinTree_Err(SVC_LOG_VIN_TREE, "VinTree_ParsingNodeBridge() SvcWrap_strtoul() failed", 0U, 0U);
                    }

                    /* Link the vin tree */
                    pInputNode->pNextNode[InputChan] = pNode;
                    pNode->pPrevNode = pInputNode;
                }
            } else {
                pNode = pInputNode->pNextNode[InputChan];
            }

            RdIdx = RdIdx + 1U;

            /* Update queue item num */
            *pRdIdx = RdIdx;

            /* Output the pOutputChan */
            *pOutputChan = Chan;

            break;
        }
    }

    VinTree_Dbg(SVC_LOG_VIN_TREE, "VinTree_ParsingNodeBridge() Done. Pass Linker %d", Chan, 0U);

    return pNode;
}

static SVC_VIN_TREE_NODE_s* VinTree_ParsingNodeSensor(const char *pWord, UINT32 *pRdIdx, SVC_VIN_TREE_NODE_s *pInputNode, UINT32 InputChan, UINT32 *pOutputChan)
{
    UINT32 RdIdx = *pRdIdx;
    const char *pSensorName = &pWord[2], *pSensorIdxStr;  /* Because S_xxx_y. Where xxx is sensor name, y is channel */
    const char UnderLineStr[2U] = "_";
    UINT32 SensorIdx = 0U;
    UINT8 i = 0U, SensorNameLength;
    UINT32 SensorIDGen = 0U;
    SVC_VIN_TREE_NODE_s *pNode;
    const SVC_VIN_TREE_NODE_s *pParentNode;

    UINT32 WrapRval = SVC_NG;
    UINT32 RetVal;

    VinTree_Dbg(SVC_LOG_VIN_TREE, "VinTree_ParsingNodeSensor() Start", 0U, 0U);

    while (SVC_OK != WrapRval) {
        RetVal = AmbaWrap_memcmp(&pSensorName[i], UnderLineStr, sizeof(char) * 1U, &WrapRval);
        if ((SVC_OK == RetVal) && (SVC_OK != WrapRval)) {
            i++;
        }
    }
    SensorNameLength = i;
    pSensorIdxStr = &pWord[2U + SensorNameLength + 1U];

    if (pInputNode->pNextNode[InputChan] == NULL) {
        pNode = VinTree_CreateNode();
        if (pNode != NULL) {
            pNode->Content.Type = SVC_VIN_TREE_NODE_TYPE_SENSOR;
            pNode->Content.Value.Sensor.SensorOrYuv = SVC_VIN_TREE_NODE_VALUE_SENSOR;
            RetVal = AmbaWrap_memcpy(pNode->Name, &pWord[2U], sizeof(char) * SensorNameLength);
            if (SVC_OK != RetVal) {
                VinTree_Err(SVC_LOG_VIN_TREE, "VinTree_ParsingNodeSensor() AmbaWrap_memcpy() failed", 0U, 0U);
            }
            RetVal = SvcWrap_strtoul(pSensorIdxStr, &SensorIdx);
            if (SVC_OK != RetVal) {
                VinTree_Err(SVC_LOG_VIN_TREE, "VinTree_ParsingNodeSensor() SvcWrap_strtoul() failed", 0U, 0U);
            }

            /* Link the vin tree */
            pInputNode->pNextNode[InputChan] = pNode;
            pNode->pPrevNode = pInputNode;

            pNode->Content.Value.Sensor.SensorIdx = (UINT16) SensorIdx;

            pParentNode = pInputNode->pPrevNode;

            while ((pParentNode != NULL) && (pParentNode->Content.Type == SVC_VIN_TREE_NODE_TYPE_BRIDGE) && (SensorIDGen == 0U)) {
                for (i = 0U; i < SVC_VIN_TREE_PARSING_BRIDGE_NUM; i++) {
                    /* If key word matching */
                    RetVal = AmbaWrap_memcmp(pParentNode->Name, SvcVinTreeBridge[i].Word, sizeof(char) * SvcVinTreeBridge[i].WordNum, &WrapRval);
                    if ((SVC_OK == RetVal) && (SVC_OK == WrapRval)) {

                        /* Pass to the bridge function to construct the Sensor ID */
                        RetVal = SvcVinTreeBridge[i].pFuncEntry(pNode);
                        if (SVC_OK != RetVal) {
                            /* If this parent node of sensor cannot determine the sensor ID, pass to upper level (grandparent node) */
                            if (pParentNode->pPrevNode != NULL) {
                                pParentNode = pParentNode->pPrevNode;
                                /* Reset counter to re-matching the bridge name of new parent node */
                                // i = 0U;
                            }
                        } else {
                            SensorIDGen = 1U;
                            break;
                        }
                    }
                }
            }
            /* if sensor directly connect with VIN */
            if (pNode->Content.Value.Sensor.SensorID == 0U) {
                pNode->Content.Value.Sensor.SensorID = ((UINT32) 1U) << pNode->Content.Value.Sensor.SensorIdx;
            }
        }
    } else {
        pNode = NULL;
    }

    RdIdx = RdIdx + 1U;

    /* Update the queue item num */
    *pRdIdx = RdIdx;

    /* Output the pOutputChan */
    *pOutputChan = 0U;

    VinTree_Dbg(SVC_LOG_VIN_TREE, "VinTree_ParsingNodeSensor() Done. Pass Linker %d", 0U, 0U);

    return pNode;  /* Sensor only with one chan. And Fov will connect on this */
}

static SVC_VIN_TREE_NODE_s* VinTree_ParsingNodeYuv(const char *pWord, UINT32 *pRdIdx, SVC_VIN_TREE_NODE_s *pInputNode, UINT32 InputChan, UINT32 *pOutputChan)
{
    UINT32 RdIdx = *pRdIdx;
    const char *pSensorName = &pWord[2], *pSensorModeStr;  /* Because Y_xxx_y. Where xxx is sensor name, y is channel */
    const char UnderLineStr[2U] = "_";
    UINT32 SensorIdx = 0U;
    UINT8 i = 0U, SensorNameLength = 0U;
    UINT32 SensorIDGen = 0U;
    SVC_VIN_TREE_NODE_s *pNode;
    const SVC_VIN_TREE_NODE_s *pParentNode;

    UINT32 RetVal, WrapRval = SVC_NG;

    VinTree_Dbg(SVC_LOG_VIN_TREE, "VinTree_ParsingNodeYuv() Start", 0U, 0U);

    while (SVC_OK != WrapRval) {
        RetVal = AmbaWrap_memcmp(&pSensorName[i], UnderLineStr, sizeof(char), &WrapRval);
        if ((SVC_OK == RetVal) && (SVC_OK != WrapRval)) {
            i++;
        }
    }
    SensorNameLength = i;
    pSensorModeStr = &pWord[2U + SensorNameLength + 1U];

    if (pInputNode->pNextNode[InputChan] == NULL) {
        pNode = VinTree_CreateNode();
        if (pNode != NULL) {
            pNode->Content.Type = SVC_VIN_TREE_NODE_TYPE_SENSOR;
            pNode->Content.Value.Sensor.SensorOrYuv = SVC_VIN_TREE_NODE_VALUE_YUV;
            RetVal = AmbaWrap_memcpy(pNode->Name, &pWord[2U], sizeof(char) * SensorNameLength);
            if (SVC_OK != RetVal) {
                VinTree_Err(SVC_LOG_VIN_TREE, "VinTree_ParsingNodeYuv() AmbaWrap_memcpy() failed", 0U, 0U);
            }
            RetVal = SvcWrap_strtoul(pSensorModeStr, &SensorIdx);
            if (SVC_OK != RetVal) {
                VinTree_Err(SVC_LOG_VIN_TREE, "VinTree_ParsingNodeYuv() SvcWrap_strtoul() failed", 0U, 0U);
            }

            /* Link the vin tree */
            pInputNode->pNextNode[InputChan] = pNode;
            pNode->pPrevNode = pInputNode;

            pNode->Content.Value.Sensor.SensorIdx = (UINT16) SensorIdx;

            /* Yuv ID need to be constructed with parent if it is bridge */
            pParentNode = pInputNode->pPrevNode;

            while ((pParentNode != NULL) && (pParentNode->Content.Type == SVC_VIN_TREE_NODE_TYPE_BRIDGE) && (SensorIDGen == 0U)) {
                for (i = 0U; i < SVC_VIN_TREE_PARSING_BRIDGE_NUM; i++) {
                    /* If key word matching */
                    RetVal = AmbaWrap_memcmp(pParentNode->Name, SvcVinTreeBridge[i].Word, sizeof(char) * SvcVinTreeBridge[i].WordNum, &WrapRval);
                    if ((SVC_OK == RetVal) && (SVC_OK == WrapRval)) {

                        /* Pass to the bridge function to construct the Sensor ID */
                        if (SVC_OK != SvcVinTreeBridge[i].pFuncEntry(pNode)) {
                            /* If this parent node of sensor cannot determine the sensor ID, pass to upper level (grandparent node) */
                            if ((pParentNode->pPrevNode != NULL) && (pParentNode->Content.Type == SVC_VIN_TREE_NODE_TYPE_BRIDGE)) {
                                pParentNode = pParentNode->pPrevNode;
                                /* Reset counter to re-matching the bridge name of new parent node */
                                // i = 0U;
                            }
                        } else {
                            SensorIDGen = 1U;
                            break;
                        }
                    }
                }
            }

            /* if Yuv input directly connect with VIN */
            if (pNode->Content.Value.Sensor.SensorID == 0U) {
                pNode->Content.Value.Sensor.SensorID = pNode->Content.Value.Sensor.SensorIdx;
            }
        }
    } else {
        pNode = NULL;
    }

    RdIdx = RdIdx + 1U;

    /* Update the queue item num */
    *pRdIdx = RdIdx;

    /* Output the pOutputChan */
    *pOutputChan = 0U;

    VinTree_Dbg(SVC_LOG_VIN_TREE, "VinTree_ParsingNodeYuv() Done. Pass Linker %d", 0U, 0U);

    return pNode;  /* Sensor only with one chan. And Fov will connect on this */
}
static SVC_VIN_TREE_NODE_s* VinTree_ParsingNodeMem(const char *pWord, UINT32 *pRdIdx, SVC_VIN_TREE_NODE_s *pInputNode, UINT32 InputChan, UINT32 *pOutputChan)
{
    UINT32 RdIdx = *pRdIdx;
    const char *pMemName = &pWord[2], *pMemIdxStr;  /* Because M_xxx_y. Where xxx is input name, y is channel */
    const char UnderLineStr[2U] = "_", RawStr[4U] = "RAW", Yuv422Str[8U] = "YUV422", Yuv420Str[8U] = "YUV420";
    UINT32 Chan, MemIdx;
    UINT8 i = 0U, MemTypeLength = 0U;
    SVC_VIN_TREE_NODE_s *pNode;
    UINT32 RetVal, WrapRval = SVC_NG;

    VinTree_Dbg(SVC_LOG_VIN_TREE, "VinTree_ParsingNodeMem() Start", 0U, 0U);

    while (SVC_OK != WrapRval) {
        RetVal = AmbaWrap_memcmp(&pMemName[i], UnderLineStr, sizeof(char), &WrapRval);
        if ((SVC_OK == RetVal) && (SVC_OK != WrapRval)) {
            i++;
        }
    }
    MemTypeLength = i;
    pMemIdxStr = &pWord[2U + MemTypeLength + 1U];
    Chan = InputChan;

    while((Chan < SVC_VIN_TREE_NODE_LEAF_NUM_MAX) && (pInputNode->pNextNode[Chan] != NULL)) {
        Chan = Chan + 1U;
    }

    if ((Chan < SVC_VIN_TREE_NODE_LEAF_NUM_MAX) && (pInputNode->pNextNode[Chan] == NULL)) {
        pNode = VinTree_CreateNode();
        if (pNode != NULL) {
            pNode->Content.Type = SVC_VIN_TREE_NODE_TYPE_MEM;
            RetVal = AmbaWrap_memcpy(pNode->Name, &pWord[2U], sizeof(char) * MemTypeLength);
            if (SVC_OK == RetVal) {
                pNode->Name[MemTypeLength] = '\0';
            }

            if ((SVC_OK == AmbaWrap_memcmp(RawStr, pNode->Name, sizeof(char) *3U, &WrapRval)) && (WrapRval == SVC_OK)) {
                pNode->Content.Value.Mem.Type = SVC_VIN_TREE_NODE_VALUE_MEM_RAW;
            } else if ((SVC_OK == AmbaWrap_memcmp(Yuv422Str, pNode->Name, sizeof(char) * 6U, &WrapRval)) && (WrapRval == SVC_OK)) {
                pNode->Content.Value.Mem.Type = SVC_VIN_TREE_NODE_VALUE_MEM_422;
            } else if ((SVC_OK == AmbaWrap_memcmp(Yuv420Str, pNode->Name, sizeof(char) * 6U, &WrapRval)) && (WrapRval == SVC_OK)) {
                pNode->Content.Value.Mem.Type = SVC_VIN_TREE_NODE_VALUE_MEM_420;
            } else {
                SvcLog_NG(SVC_LOG_VIN_TREE, "VinTree_ParsingNodeMem() err. Unknown MEM type. Should not be here", 0U, 0U);
            }

            RetVal = SvcWrap_strtoul(pMemIdxStr, &MemIdx);
            if (SVC_OK == RetVal) {
                pNode->Content.Value.Mem.MemIdx = (UINT16) MemIdx;
            }

            /* Link the vin tree */
            pInputNode->pNextNode[Chan] = pNode;
            pNode->pPrevNode = pInputNode;
        }
    } else {
        pNode = NULL;
    }

    RdIdx = RdIdx + 1U;

    /* Update the queue item num */
    *pRdIdx = RdIdx;

    /* Output the pOutputChan */
    *pOutputChan = 0U;

    VinTree_Dbg(SVC_LOG_VIN_TREE, "VinTree_ParsingNodeMemIn() Done. Pass Linker %d", 0U, 0U);

    return pNode;  /* Sensor only with one chan. And Fov will connect on this */
}

static SVC_VIN_TREE_NODE_s* VinTree_ParsingNodeFov(const char *pWord, UINT32 *pRdIdx, SVC_VIN_TREE_NODE_s *pInputNode, UINT32 InputChan, UINT32 *pOutputChan)
{
    UINT32 RdIdx = *pRdIdx;
    char FovIdxStr[4U] = "";
    SVC_VIN_TREE_NODE_s *pNode = NULL;
    UINT8  i = 0U, FovIdxStrLength = 0U, FovStrLength = 0U;
    const char ByPassStr[8U] = "_BYPASS";
    const char UnderLineStr[2U] = "_";
    UINT32 WrapRval = SVC_NG;
    UINT32 RetVal;

    VinTree_Dbg(SVC_LOG_VIN_TREE, "VinTree_ParsingNodeFov() Start", 0U, 0U);

    AmbaMisra_TouchUnused(pInputNode);
    while (SVC_OK != WrapRval) {

        if (pWord[4U + i] == '\0') {
            break;
        }
        RetVal = AmbaWrap_memcmp(&pWord[4U + i], UnderLineStr, sizeof(char), &WrapRval);
        if ((SVC_OK == RetVal) && (SVC_OK != WrapRval)) {
            i++;
        }
    }

    FovIdxStrLength = i;
    FovStrLength = 4U + FovIdxStrLength;
    /* Copy the FovIdx to temp buffer */
    if (FovIdxStrLength != 0U) {
        RetVal = AmbaWrap_memcpy(&FovIdxStr[0U], &pWord[4U], sizeof(char) * FovIdxStrLength);
        if (SVC_OK == RetVal) {
            FovIdxStr[FovIdxStrLength] = '\0';
        }
    }

    pNode = VinTree_CreateNode();
    if (pNode != NULL) {
        pNode->Content.Type = SVC_VIN_TREE_NODE_TYPE_FOV;

        RetVal = SvcWrap_strtoul(FovIdxStr, &pNode->Content.Value.Fov.FovIdx);

        if (SVC_OK != RetVal) {
            VinTree_Err(SVC_LOG_VIN_TREE, "VinTree_ParsingNodeFov() SvcWrap_strtoul() failed", 0U, 0U);
        }
        RetVal = AmbaWrap_memcpy(pNode->Name, pWord, sizeof(char) * FovStrLength);

        if (SVC_OK != RetVal) {
            VinTree_Err(SVC_LOG_VIN_TREE, "VinTree_ParsingNodeFov() AmbaWrap_memcpy() failed", 0U, 0U);
        }

        if (pWord[FovStrLength] != '\0') {
            RetVal = AmbaWrap_memcmp(&pWord[FovStrLength], ByPassStr, sizeof(char) * 7U, &WrapRval);
            if ((SVC_OK == RetVal) && (WrapRval == SVC_OK)) {
                pNode->Content.Value.Fov.Attribute = SVC_VIN_TREE_NODE_VALUE_FOV_BYPASS;
            }
        }

        /* Link the vin tree */
        pInputNode->pNextNode[InputChan] = pNode;
        pNode->pPrevNode = pInputNode;
    }

    RdIdx = RdIdx + 1U;

    /* Update the queue item num */
    *pRdIdx = RdIdx;

    /* Output the pOutputChan */
    *pOutputChan = 0U;

    VinTree_Dbg(SVC_LOG_VIN_TREE, "VinTree_ParsingNodeFov() Done. Pass Linker %d", 0U, 0U);

    return pNode;  /* If there's more than one fov on one sensor. Link it next to this. */
}

static SVC_VIN_TREE_NODE_s* VinTree_ParsingNodeDmy(const char *pWord, UINT32 *pRdIdx, SVC_VIN_TREE_NODE_s *pInputNode, UINT32 InputChan, UINT32 *pOutputChan)
{
    UINT32 RdIdx = *pRdIdx;
    const char *pSensorName = &pWord[4], *pSensorIdxStr;  /* Because S_xxx_y. Where xxx is sensor name, y is channel */
    const char UnderLineStr[2U] = "_";
    UINT32 SensorIdx = 0U;
    UINT8 i = 0U, SensorNameLength;
    SVC_VIN_TREE_NODE_s *pNode;

    UINT32 WrapRval = SVC_NG;
    UINT32 RetVal;

    VinTree_Dbg(SVC_LOG_VIN_TREE, "VinTree_ParsingNodeSensor() Start", 0U, 0U);

    while (SVC_OK != WrapRval) {
        RetVal = AmbaWrap_memcmp(&pSensorName[i], UnderLineStr, sizeof(char) * 1U, &WrapRval);
        if ((SVC_OK == RetVal) && (SVC_OK != WrapRval)) {
            i++;
        }
    }
    SensorNameLength = i;
    pSensorIdxStr = &pWord[2U + SensorNameLength + 1U];

    if (pInputNode->pNextNode[InputChan] == NULL) {
        pNode = VinTree_CreateNode();
        if (pNode != NULL) {
            pNode->Content.Type = SVC_VIN_TREE_NODE_TYPE_SENSOR;
            pNode->Content.Value.Sensor.SensorOrYuv = SVC_VIN_TREE_NODE_VALUE_DMYSENSOR;
            RetVal = AmbaWrap_memcpy(pNode->Name, &pWord[4U], sizeof(char) * SensorNameLength);
            if (SVC_OK != RetVal) {
                VinTree_Err(SVC_LOG_VIN_TREE, "VinTree_ParsingNodeSensor() AmbaWrap_memcpy() failed", 0U, 0U);
            }
            RetVal = SvcWrap_strtoul(pSensorIdxStr, &SensorIdx);
            if (SVC_OK != RetVal) {
                VinTree_Err(SVC_LOG_VIN_TREE, "VinTree_ParsingNodeSensor() SvcWrap_strtoul() failed", 0U, 0U);
            }

            /* Link the vin tree */
            pInputNode->pNextNode[InputChan] = pNode;
            pNode->pPrevNode = pInputNode;

            pNode->Content.Value.Sensor.SensorIdx = (UINT16) SensorIdx;

            /* if sensor directly connect with VIN */
            if (pNode->Content.Value.Sensor.SensorID == 0U) {
                pNode->Content.Value.Sensor.SensorID = ((UINT32) 1U) << pNode->Content.Value.Sensor.SensorIdx;
            }
        }
    } else {
        pNode = NULL;
    }

    RdIdx = RdIdx + 1U;

    /* Update the queue item num */
    *pRdIdx = RdIdx;

    /* Output the pOutputChan */
    *pOutputChan = 0U;

    VinTree_Dbg(SVC_LOG_VIN_TREE, "VinTree_ParsingNodeSensor() Done. Pass Linker %d", 0U, 0U);

    return pNode;  /* Sensor only with one chan. And Fov will connect on this */

}

static UINT32 VinTree_GenSensorIDB6N(SVC_VIN_TREE_NODE_s *pInputNode)
{
    UINT32 RetVal = SVC_OK;
    const SVC_VIN_TREE_NODE_s *pParentNode;
    UINT32 ConnectedBridgeIdx, ConnectedSensorIdx;

    if ((pInputNode != NULL) && (pInputNode->Content.Type == SVC_VIN_TREE_NODE_TYPE_SENSOR)) {
        if (pInputNode->pPrevNode != NULL) {
            pParentNode = pInputNode->pPrevNode;
            if (pParentNode->Content.Type == SVC_VIN_TREE_NODE_TYPE_BRIDGE) {
                ConnectedBridgeIdx = pParentNode->Content.Value.Bridge.BridgeIdx;
                ConnectedSensorIdx = pParentNode->Content.Value.Bridge.SensorIdx;

                if (ConnectedBridgeIdx == 0U) {
                    pInputNode->Content.Value.Sensor.SensorID = 0x00000010U;
                } else if (ConnectedBridgeIdx == 1U) {
                    pInputNode->Content.Value.Sensor.SensorID = 0x00000020U;
                } else {
                    VinTree_Err(SVC_LOG_VIN_TREE, "VinTree_GenSensorIDB6N() failed with error BridgeChan(%d) SensorChan(%d)", ConnectedBridgeIdx, ConnectedSensorIdx);
                    pInputNode->Content.Value.Sensor.SensorID = 0xDeadbeafU;
                }
            }
        }
    }

    return RetVal;
}

static UINT32 VinTree_GenSensorIDB6F(SVC_VIN_TREE_NODE_s *pInputNode)
{
    AmbaMisra_TouchUnused(pInputNode);

    /* Cannot determine */
    return SVC_NG;
}

static UINT32 VinTree_GenSensorIDMaxim96712(SVC_VIN_TREE_NODE_s *pInputNode)
{
    UINT32 RetVal = SVC_OK;
    const SVC_VIN_TREE_NODE_s *pParentNode;
    UINT32 ConnectedBridgeIdx, ConnectedSensorIdx;

    if ((pInputNode != NULL) && (pInputNode->Content.Type == SVC_VIN_TREE_NODE_TYPE_SENSOR)) {
        if (pInputNode->pPrevNode != NULL) {
            pParentNode = pInputNode->pPrevNode;
            if (pParentNode->Content.Type == SVC_VIN_TREE_NODE_TYPE_BRIDGE) {
                ConnectedBridgeIdx = pParentNode->Content.Value.Bridge.BridgeIdx;
                ConnectedSensorIdx = pParentNode->Content.Value.Bridge.SensorIdx;

                if (ConnectedBridgeIdx == 0U) {
                    pInputNode->Content.Value.Sensor.SensorID = 0x00000010U;
                } else if (ConnectedBridgeIdx == 1U) {
                    pInputNode->Content.Value.Sensor.SensorID = 0x00000100U;
                } else if (ConnectedBridgeIdx == 2U) {
                    pInputNode->Content.Value.Sensor.SensorID = 0x00001000U;
                } else if (ConnectedBridgeIdx == 3U) {
                    pInputNode->Content.Value.Sensor.SensorID = 0x00010000U;
                } else {
                    VinTree_Err(SVC_LOG_VIN_TREE, "VinTree_GenSensorIDMaxim96712() failed with error BridgeChan(%d) SensorChan(%d)", ConnectedBridgeIdx, ConnectedSensorIdx);
                    pInputNode->Content.Value.Sensor.SensorID = 0xDeadbeafU;
                }
            }
        }
        VinTree_Dbg(SVC_LOG_VIN_TREE, "pInputNode->Content.Value.Sensor.SensorID = 0x%8x", pInputNode->Content.Value.Sensor.SensorID, 0U);
    } else {
        VinTree_Err(SVC_LOG_VIN_TREE, "VinTree_GenSensorIDMaxim96712() failed with error Input err", 0U, 0U);
        if (pInputNode != NULL) {
            pInputNode->Content.Value.Sensor.SensorID = 0xDeadbeafU;
        }
    }

    return RetVal;
}

static UINT32 VinTree_GenSensorIDMaxim96722(SVC_VIN_TREE_NODE_s *pInputNode)
{
    UINT32 RetVal = SVC_OK;
    const SVC_VIN_TREE_NODE_s *pParentNode;
    UINT32 ConnectedBridgeIdx, ConnectedSensorIdx;

    if ((pInputNode != NULL) && (pInputNode->Content.Type == SVC_VIN_TREE_NODE_TYPE_SENSOR)) {
        if (pInputNode->pPrevNode != NULL) {
            pParentNode = pInputNode->pPrevNode;
            if (pParentNode->Content.Type == SVC_VIN_TREE_NODE_TYPE_BRIDGE) {
                ConnectedBridgeIdx = pParentNode->Content.Value.Bridge.BridgeIdx;
                ConnectedSensorIdx = pParentNode->Content.Value.Bridge.SensorIdx;

                if (ConnectedBridgeIdx == 0U) {
                    pInputNode->Content.Value.Sensor.SensorID = 0x00000010U;
                } else if (ConnectedBridgeIdx == 1U) {
                    pInputNode->Content.Value.Sensor.SensorID = 0x00000100U;
                } else if (ConnectedBridgeIdx == 2U) {
                    pInputNode->Content.Value.Sensor.SensorID = 0x00001000U;
                } else if (ConnectedBridgeIdx == 3U) {
                    pInputNode->Content.Value.Sensor.SensorID = 0x00010000U;
                } else {
                    VinTree_Err(SVC_LOG_VIN_TREE, "VinTree_GenSensorIDMaxim96722() failed with error BridgeChan(%d) SensorChan(%d)", ConnectedBridgeIdx, ConnectedSensorIdx);
                    pInputNode->Content.Value.Sensor.SensorID = 0xDeadbeafU;
                }
            }
        }
        VinTree_Dbg(SVC_LOG_VIN_TREE, "pInputNode->Content.Value.Sensor.SensorID = 0x%8x", pInputNode->Content.Value.Sensor.SensorID, 0U);
    } else {
        VinTree_Err(SVC_LOG_VIN_TREE, "VinTree_GenSensorIDMaxim96722() failed with error Input err", 0U, 0U);
        if (pInputNode != NULL) {
            pInputNode->Content.Value.Sensor.SensorID = 0xDeadbeafU;
        }
    }

    return RetVal;
}

static UINT32 VinTree_GenSensorIDMaxim9296(SVC_VIN_TREE_NODE_s *pInputNode)
{
    UINT32 RetVal = SVC_OK;
    const SVC_VIN_TREE_NODE_s *pParentNode;
    UINT32 ConnectedBridgeIdx, ConnectedSensorIdx;

    if ((pInputNode != NULL) && (pInputNode->Content.Type == SVC_VIN_TREE_NODE_TYPE_SENSOR)) {
        if (pInputNode->pPrevNode != NULL) {
            pParentNode = pInputNode->pPrevNode;
            if (pParentNode->Content.Type == SVC_VIN_TREE_NODE_TYPE_BRIDGE) {
                ConnectedBridgeIdx = pParentNode->Content.Value.Bridge.BridgeIdx;
                ConnectedSensorIdx = pParentNode->Content.Value.Bridge.SensorIdx;

                if (ConnectedBridgeIdx == 0U) {
                    pInputNode->Content.Value.Sensor.SensorID = 0x00000010U;
                } else if (ConnectedBridgeIdx == 1U) {
                    pInputNode->Content.Value.Sensor.SensorID = 0x00000100U;
                } else {
                    VinTree_Err(SVC_LOG_VIN_TREE, "VinTree_GenSensorIDMaxim9296() failed with error BridgeChan(%d) SensorChan(%d)", ConnectedBridgeIdx, ConnectedSensorIdx);
                    pInputNode->Content.Value.Sensor.SensorID = 0xDeadbeafU;
                }
            }
        }
        VinTree_Dbg(SVC_LOG_VIN_TREE, "pInputNode->Content.Value.Sensor.SensorID = 0x%8x", pInputNode->Content.Value.Sensor.SensorID, 0U);
    } else {
        VinTree_Err(SVC_LOG_VIN_TREE, "VinTree_GenSensorIDMaxim9296() failed with error Input err", 0U, 0U);
        if (pInputNode != NULL) {
            pInputNode->Content.Value.Sensor.SensorID = 0xDeadbeafU;
        }
    }

    return RetVal;
}

static UINT32 VinTree_GenSensorIDMaxim9295(SVC_VIN_TREE_NODE_s *pInputNode)
{
    AmbaMisra_TouchUnused(pInputNode);

    /* Cannot determine */
    return SVC_NG;
}

static UINT32 VinTree_GenSensorIDTi960(SVC_VIN_TREE_NODE_s *pInputNode)
{
    UINT32 RetVal = SVC_OK;
    const SVC_VIN_TREE_NODE_s *pParentNode;
    UINT32 ConnectedBridgeIdx, ConnectedSensorIdx;

    if ((pInputNode != NULL) && (pInputNode->Content.Type == SVC_VIN_TREE_NODE_TYPE_SENSOR)) {
        if (pInputNode->pPrevNode != NULL) {
            pParentNode = pInputNode->pPrevNode;
            if (pParentNode->Content.Type == SVC_VIN_TREE_NODE_TYPE_BRIDGE) {
                ConnectedBridgeIdx = pParentNode->Content.Value.Bridge.BridgeIdx;
                ConnectedSensorIdx = pParentNode->Content.Value.Bridge.SensorIdx;

                if (ConnectedBridgeIdx == 0U) {
                    pInputNode->Content.Value.Sensor.SensorID = 0x00000010U;
                } else if (ConnectedBridgeIdx == 1U) {
                    pInputNode->Content.Value.Sensor.SensorID = 0x00000100U;
                } else if (ConnectedBridgeIdx == 2U) {
                    pInputNode->Content.Value.Sensor.SensorID = 0x00001000U;
                } else if (ConnectedBridgeIdx == 3U) {
                    pInputNode->Content.Value.Sensor.SensorID = 0x00010000U;
                } else {
                    VinTree_Err(SVC_LOG_VIN_TREE, "VinTree_GenSensorIDTi960() failed with error BridgeChan(%d) SensorChan(%d)", ConnectedBridgeIdx, ConnectedSensorIdx);
                    pInputNode->Content.Value.Sensor.SensorID = 0xDeadbeafU;
                }
            }
        }
        VinTree_Dbg(SVC_LOG_VIN_TREE, "pInputNode->Content.Value.Sensor.SensorID = 0x%8x", pInputNode->Content.Value.Sensor.SensorID, 0U);
    } else {
        VinTree_Err(SVC_LOG_VIN_TREE, "VinTree_GenSensorIDTi960() failed with error Input err", 0U, 0U);
        if (pInputNode != NULL) {
            pInputNode->Content.Value.Sensor.SensorID = 0xDeadbeafU;
        }
    }

    return RetVal;
}

static UINT32 VinTree_GenSensorIDTi953(SVC_VIN_TREE_NODE_s *pInputNode)
{
    AmbaMisra_TouchUnused(pInputNode);

    /* Cannot determine */
    return SVC_NG;
}

static UINT32 VinTree_GenSensorIDTi913(SVC_VIN_TREE_NODE_s *pInputNode)
{
    AmbaMisra_TouchUnused(pInputNode);

    /* Cannot determine */
    return SVC_NG;
}

static UINT32 VinTree_GenSensorIDTi954(SVC_VIN_TREE_NODE_s *pInputNode)
{
    UINT32 RetVal = SVC_OK;
    const SVC_VIN_TREE_NODE_s *pParentNode;
    UINT32 ConnectedBridgeIdx, ConnectedSensorIdx;

    if ((pInputNode != NULL) && (pInputNode->Content.Type == SVC_VIN_TREE_NODE_TYPE_SENSOR)) {
        if (pInputNode->pPrevNode != NULL) {
            pParentNode = pInputNode->pPrevNode;
            if (pParentNode->Content.Type == SVC_VIN_TREE_NODE_TYPE_BRIDGE) {
                ConnectedBridgeIdx = pParentNode->Content.Value.Bridge.BridgeIdx;
                ConnectedSensorIdx = pParentNode->Content.Value.Bridge.SensorIdx;

                if (ConnectedBridgeIdx == 0U) {
                    pInputNode->Content.Value.Sensor.SensorID = 0x00000010U;
                } else if (ConnectedBridgeIdx == 1U) {
                    pInputNode->Content.Value.Sensor.SensorID = 0x00000100U;
                } else {
                    VinTree_Err(SVC_LOG_VIN_TREE, "VinTree_GenSensorIDTi960() failed with error BridgeChan(%d) SensorChan(%d)", ConnectedBridgeIdx, ConnectedSensorIdx);
                    pInputNode->Content.Value.Sensor.SensorID = 0xDeadbeafU;
                }
            }
        }
        VinTree_Dbg(SVC_LOG_VIN_TREE, "pInputNode->Content.Value.Sensor.SensorID = 0x%8x", pInputNode->Content.Value.Sensor.SensorID, 0U);
    } else {
        VinTree_Err(SVC_LOG_VIN_TREE, "VinTree_GenSensorIDTi960() failed with error Input err", 0U, 0U);
        if (pInputNode != NULL) {
            pInputNode->Content.Value.Sensor.SensorID = 0xDeadbeafU;
        }
    }

    return RetVal;
}

static UINT32 VinTree_GenSensorIDMaxim9286(SVC_VIN_TREE_NODE_s *pInputNode)
{
    UINT32 RetVal = SVC_OK;
    const SVC_VIN_TREE_NODE_s *pParentNode;
    UINT32 ConnectedBridgeIdx, ConnectedSensorIdx;

    if ((pInputNode != NULL) && (pInputNode->Content.Type == SVC_VIN_TREE_NODE_TYPE_SENSOR)) {
        if (pInputNode->pPrevNode != NULL) {
            pParentNode = pInputNode->pPrevNode;
            if (pParentNode->Content.Type == SVC_VIN_TREE_NODE_TYPE_BRIDGE) {
                ConnectedBridgeIdx = pParentNode->Content.Value.Bridge.BridgeIdx;
                ConnectedSensorIdx = pParentNode->Content.Value.Bridge.SensorIdx;

                if (ConnectedBridgeIdx == 0U) {
                    pInputNode->Content.Value.Sensor.SensorID = 0x00000010U;
                } else if (ConnectedBridgeIdx == 1U) {
                    pInputNode->Content.Value.Sensor.SensorID = 0x00000100U;
                } else if (ConnectedBridgeIdx == 2U) {
                    pInputNode->Content.Value.Sensor.SensorID = 0x00001000U;
                } else if (ConnectedBridgeIdx == 3U) {
                    pInputNode->Content.Value.Sensor.SensorID = 0x00010000U;
                } else {
                    VinTree_Err(SVC_LOG_VIN_TREE, "VinTree_GenSensorIDMaxim9286() failed with error BridgeChan(%d) SensorChan(%d)", ConnectedBridgeIdx, ConnectedSensorIdx);
                    pInputNode->Content.Value.Sensor.SensorID = 0xDeadbeafU;
                }
            }
        }
        VinTree_Dbg(SVC_LOG_VIN_TREE, "pInputNode->Content.Value.Sensor.SensorID = 0x%8x", pInputNode->Content.Value.Sensor.SensorID, 0U);
    } else {
        VinTree_Err(SVC_LOG_VIN_TREE, "VinTree_GenSensorIDMaxim9286() failed with error Input err", 0U, 0U);
        if (pInputNode != NULL) {
            pInputNode->Content.Value.Sensor.SensorID = 0xDeadbeafU;
        }
    }

    return RetVal;
}

static UINT32 VinTree_GenSensorIDMaxim9288(SVC_VIN_TREE_NODE_s *pInputNode)
{
    UINT32 RetVal = SVC_OK;
    const SVC_VIN_TREE_NODE_s *pParentNode;
    UINT32 ConnectedBridgeIdx, ConnectedSensorIdx;

    if ((pInputNode != NULL) && (pInputNode->Content.Type == SVC_VIN_TREE_NODE_TYPE_SENSOR)) {
        if (pInputNode->pPrevNode != NULL) {
            pParentNode = pInputNode->pPrevNode;
            if (pParentNode->Content.Type == SVC_VIN_TREE_NODE_TYPE_BRIDGE) {
                ConnectedBridgeIdx = pParentNode->Content.Value.Bridge.BridgeIdx;
                ConnectedSensorIdx = pParentNode->Content.Value.Bridge.SensorIdx;

                if (ConnectedBridgeIdx == 0U) {
                    pInputNode->Content.Value.Sensor.SensorID = 0x00000010U;
                } else {
                    VinTree_Err(SVC_LOG_VIN_TREE, "VinTree_GenSensorIDMaxim9288() failed with error BridgeChan(%d) SensorChan(%d)", ConnectedBridgeIdx, ConnectedSensorIdx);
                    pInputNode->Content.Value.Sensor.SensorID = 0xDeadbeafU;
                }
            }
        }
        VinTree_Dbg(SVC_LOG_VIN_TREE, "pInputNode->Content.Value.Sensor.SensorID = 0x%8x", pInputNode->Content.Value.Sensor.SensorID, 0U);
    } else {
        VinTree_Err(SVC_LOG_VIN_TREE, "VinTree_GenSensorIDMaxim9288() failed with error Input err", 0U, 0U);
        if (pInputNode != NULL) {
            pInputNode->Content.Value.Sensor.SensorID = 0xDeadbeafU;
        }
    }

    return RetVal;
}

static UINT32 VinTree_GenSensorIDMaxim96707(SVC_VIN_TREE_NODE_s *pInputNode)
{
    AmbaMisra_TouchUnused(pInputNode);

    /* Cannot determine */
    return SVC_NG;
}

static UINT32 VinTree_GenSensorIDMaxim96705(SVC_VIN_TREE_NODE_s *pInputNode)
{
    AmbaMisra_TouchUnused(pInputNode);

    /* Cannot determine */
    return SVC_NG;
}

static UINT32 VinTree_GenSensorIDMaxim96716(SVC_VIN_TREE_NODE_s *pInputNode)
{
    UINT32 RetVal = SVC_OK;
    const SVC_VIN_TREE_NODE_s *pParentNode;
    UINT32 ConnectedBridgeIdx, ConnectedSensorIdx;

    if ((pInputNode != NULL) && (pInputNode->Content.Type == SVC_VIN_TREE_NODE_TYPE_SENSOR)) {
        if (pInputNode->pPrevNode != NULL) {
            pParentNode = pInputNode->pPrevNode;
            if (pParentNode->Content.Type == SVC_VIN_TREE_NODE_TYPE_BRIDGE) {
                ConnectedBridgeIdx = pParentNode->Content.Value.Bridge.BridgeIdx;
                ConnectedSensorIdx = pParentNode->Content.Value.Bridge.SensorIdx;

                if (ConnectedBridgeIdx == 0U) {
                    pInputNode->Content.Value.Sensor.SensorID = 0x00000010U;
                } else if (ConnectedBridgeIdx == 1U) {
                    pInputNode->Content.Value.Sensor.SensorID = 0x00000100U;
                } else {
                    VinTree_Err(SVC_LOG_VIN_TREE, "VinTree_GenSensorIDMaxim96716() failed with error BridgeChan(%d) SensorChan(%d)", ConnectedBridgeIdx, ConnectedSensorIdx);
                    pInputNode->Content.Value.Sensor.SensorID = 0xDeadbeafU;
                }
            }
        }
        VinTree_Dbg(SVC_LOG_VIN_TREE, "pInputNode->Content.Value.Sensor.SensorID = 0x%8x", pInputNode->Content.Value.Sensor.SensorID, 0U);
    } else {
        VinTree_Err(SVC_LOG_VIN_TREE, "VinTree_GenSensorIDMaxim96716() failed with error Input err", 0U, 0U);
        if (pInputNode != NULL) {
            pInputNode->Content.Value.Sensor.SensorID = 0xDeadbeafU;
        }
    }

    return RetVal;
}

static UINT32 VinTree_GenSensorIDMaxim96717(SVC_VIN_TREE_NODE_s *pInputNode)
{
    AmbaMisra_TouchUnused(pInputNode);

    /* Cannot determine */
    return SVC_NG;
}

static SVC_VIN_TREE_NODE_s* VinTree_CreateNode(void)
{
    UINT32 ValidIdx;
    SVC_VIN_TREE_NODE_s *pNode;

    ValidIdx = VinTreeNodeCtrl.UsedNum;

    if (ValidIdx < SVC_VIN_TREE_NODE_MAX_NUM) {
        pNode = &VinTreeNodeCtrl.Nodes[ValidIdx];
        VinTreeNodeCtrl.UsedNum = VinTreeNodeCtrl.UsedNum + 1U;
    } else {
        pNode = NULL;
    }
    return pNode;
}

static UINT32 VinTree_CleanQueue(void)
{
    UINT32 RetVal;

    RetVal = AmbaWrap_memset(&ParsingQueue, 0, sizeof(ParsingQueue));
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_VIN_TREE, "VinTree_CleanQueue() err. AmbaWrap_memset() failed with %d", RetVal, 0U);
    }

    return RetVal;
}

static UINT32 VinTree_CleanTree(void)
{
    UINT32 RetVal;

    pVinTreeEntry = NULL;
    RetVal = AmbaWrap_memset(&VinTreeNodeCtrl, 0, sizeof(VinTreeNodeCtrl));
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_VIN_TREE, "VinTree_CleanQueue() err. AmbaWrap_memset() failed with %d", RetVal, 0U);
    }

    return RetVal;
}

static UINT32 VinTree_CleanVisited(void)
{
    UINT32 i;
    UINT32 ItemNum = VinTreeNodeCtrl.UsedNum;
    SVC_VIN_TREE_NODE_s *pNode;

    for (i = 0U; i < ItemNum; i++) {
        pNode = &VinTreeNodeCtrl.Nodes[i];
        pNode->Visited = 0U;
    }

    return SVC_OK;
}

static UINT32 VinTree_StackPut(SVC_VIN_TREE_NODE_s *pNode)
{
    UINT32 WrIdx = SearchingStack.ItemNum;

    SearchingStack.pNode[WrIdx] = pNode;
    WrIdx = WrIdx + 1U;
    SearchingStack.ItemNum = WrIdx;

    return SVC_OK;
}

static SVC_VIN_TREE_NODE_s* VinTree_StackPop(void)
{
    UINT32 PopIdx = SearchingStack.ItemNum - 1U;

    if (SearchingStack.pNode[PopIdx] != NULL) {
        if (SearchingStack.pNode[PopIdx]->Visited == SVC_VIN_TREE_NODE_LEAF_NUM_MAX) {
            SearchingStack.pNode[PopIdx] = NULL;
            SearchingStack.ItemNum = PopIdx;
        }
    }

    return SearchingStack.pNode[PopIdx];
}

static UINT32 VinTree_StackFlush(void)
{
    UINT32 ItemNum = SearchingStack.ItemNum;
    UINT32 i;

    VinTree_Dbg(SVC_LOG_VIN_TREE, "VinTree_StackFlush() start", 0U, 0U);

    for (i = 0U; i < ItemNum; i++) {
        VinTree_Dbg(SearchingStack.pNode[i]->Name, "", 0U, 0U);
        SearchingStack.pNode[i] = NULL;
        SearchingStack.ItemNum = SearchingStack.ItemNum - 1U;
    }

    VinTree_Dbg(SVC_LOG_VIN_TREE, "VinTree_StackFlush() done", 0U, 0U);

    return SVC_OK;
}

static UINT32 VinTree_StackEmpty(void)
{
    UINT32 RetVal;

    if (SearchingStack.ItemNum == 0U) {
        RetVal = SVC_OK;
    } else {
        RetVal = SVC_NG;
    }

    return RetVal;
}

static void VinTree_Dbg(const char *pModule, const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
    if (VinTreeDebugEnable > 0U) {
        SvcLog_OK(pModule, pFormat, Arg1, Arg2);
    }
}

static void VinTree_Err(const char *pModule, const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
    if (VinTreeDebugEnable > 0U) {
        SvcLog_NG(pModule, pFormat, Arg1, Arg2);
    }
}
