/**
 * @file AmbaCFS_Cache.c
 *
 * Copyright (c) 2019 Ambarella International LP
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
 */
#include "AmbaCFS_Util.h"
#include "AmbaCFS_Cache.h"
#include "AmbaCFS_ActiveDta.h"
#include <AmbaPrint.h>

/*
 *   Log macro
 */
#define AMBA_CFS_CACHE_DEBUG    (0U)    /**< Enable debug log */

/*
 *   Constants and Global variables definition
 */
#define AMBA_CFS_CACHE_MAX_NUM_DIR_DEFAULT  (8U)    /**< Max number of cached directories */
#define AMBA_CFS_CACHE_MAX_NUM_FILE_DEFAULT (4000U) /**< Max number of cached files */

/*
 *   Result of matching regular expression with filename. Used in RegExpMatch.
 */
#define AMBA_CFS_REG_EXP_COMPARING          (0U)    /**< Comparison is not finished. */
#define AMBA_CFS_REG_EXP_MATCH              (1U)    /**< Matched. End of comparison. */
#define AMBA_CFS_REG_EXP_NOT_MATCH          (2U)    /**< Not matched. End of comparison. */

/**
 * CFS cache pool
 */
typedef struct {
    AMBA_KAL_MUTEX_t Mutex;                                                     /**< Mutex */
    AMBA_CFS_CACHE_TREE_NODE_s       *NodeCache;                                /**< Node info cache */
    AMBA_CFS_CACHE_TREE_NODE_s       *NodeAvailList;                            /**< Pointing to the head of the available nodes (single linked list) */
    AMBA_CFS_CACHE_SEARCH_s          *SearchCache;                              /**< Search info cache */
    AMBA_CFS_CACHE_SEARCH_s          *SearchAvailList;                          /**< Pointing to the head of the available searches (single linked list) */
    AMBA_CFS_CACHE_SEARCH_s          *SearchUsedList;                           /**< Pointing to the head of the used searches (double linked list) */
    AMBA_CFS_CACHE_TREE_NODE_s        Root[AMBA_CFS_MAX_DRIVE_AMOUNT];          /**< Root nodes for each drive */
    UINT32                            CacheMaxNodeNum;                          /**< Max cached file/directory node number */
    UINT32                            CacheMaxDirNum;                           /**< Max cached directory node number */
    AMBA_CFS_DRIVE_INFO_s             DevInf[AMBA_CFS_MAX_DRIVE_AMOUNT];        /**< Devive info cache */
    UINT8                             DevInfCached[AMBA_CFS_MAX_DRIVE_AMOUNT];  /**< The device info is cached or not */
} AMBA_CFS_CACHE_s;

/**
 * CFS cache core
 */
static AMBA_CFS_CACHE_s g_AmbaCFSCache GNU_SECTION_NOZEROINIT;

/*
 * Internal functions
 */

/**
 *  Lock CFS cache mutex
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_CacheLock(void)
{
    UINT32 Ret;
    Ret = CFS_K2C(AmbaKAL_MutexTake(&g_AmbaCFSCache.Mutex, AMBA_CFS_TIMEOUT_MUTEX));
    if (Ret != CFS_OK) {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaKAL_MutexTake fail", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Unlock CFS cache mutex
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_CacheUnlock(void)
{
    UINT32 Ret;
    Ret = CFS_K2C(AmbaKAL_MutexGive(&g_AmbaCFSCache.Mutex));
    if (Ret != CFS_OK) {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaKAL_MutexGive fail", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Check if the two characters are the same. Different cases are considered as same characters.
 *  @param [in] C1                  The character
 *  @param [in] C2                  The character
 *  @return 0 - Different, 1 - Same
 */
static UINT8 SameAlphabet(char C1, char C2)
{
    UINT8 Ret = 0U;
    char C1Upper = AmbaCFS_UtilToUpper(C1);
    char C2Upper = AmbaCFS_UtilToUpper(C2);
    if (C1Upper == C2Upper) {
        Ret = 1U;
    }
    return Ret;
}

/**
 *  Get cached search from active DTA
 *  @param [in] ActiveDta           The Active DTA
 *  @return The cached search
 */
static inline AMBA_CFS_CACHE_SEARCH_s *AmbaCFS_CacheGetSearch(const AMBA_CFS_ACTIVE_DTA_s *ActiveDta)
{
    AMBA_CFS_CACHE_SEARCH_s *Search = NULL;
    AmbaMisra_TypeCast(&Search, &ActiveDta->CachedSearch);
    return Search;
}

/**
 *  Get cached node from active DTA
 *  @param [in] ActiveDta           The Active DTA
 *  @return The cached node
 */
static inline AMBA_CFS_CACHE_TREE_NODE_s *AmbaCFS_CacheGetNode(const AMBA_CFS_ACTIVE_DTA_s *ActiveDta)
{
    AMBA_CFS_CACHE_TREE_NODE_s *Node = NULL;
    AmbaMisra_TypeCast(&Node, &ActiveDta->CachedNode);
    return Node;
}

/**
 *  Check if the two strings correspond to the same file name
 *  @param [in] Str1                String 1
 *  @param [in] Str2                String 2
 *  @return 1 - TRUE, 0 - FALSE
 */
static inline UINT8 SameFileName(const char *Str1, const char *Str2)
{
    UINT8 Ret = 0U;
    UINT8 IsBreakLoop = 0U;
    /* String length should be bounded by AMBA_CFS_MAX_FILENAME_LENGTH */
    for (UINT32 i = 0U; i < AMBA_CFS_MAX_FILENAME_LENGTH; ++i) {
        if (Str1[i] == '\0') {
            if (Str2[i] == '\0') {
                Ret = 1U;
            }
            IsBreakLoop = 1U;
        } else if (Str2[i] == '\0') {
            IsBreakLoop = 1U;
        } else if (SameAlphabet(Str1[i], Str2[i]) == 0U) {
            IsBreakLoop = 1U;
        } else {
            /* Character matched. Do nothing. */
        }

        if (IsBreakLoop != 0U) {
            break;
        }
    }
    if (IsBreakLoop != 1U) {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] String length unexpected", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Check if the filename matches the regular expression when the current character of RegExp is '*'
 *  AmbaFS is not matched with PrFile2's API doc => a.b. and .a.b. should not be included in "*.", but AmbaFS can get it
 *  @param [in] FileName            Filename
 *  @param [in] RegExp              Regular expression
 *  @param [in,out] CurI            Current index of FileName
 *  @param [in,out] CurJ            Current index of RegExp
 *  @return AMBA_CFS_REG_EXP_COMPARING: Currently match but not finished, AMBA_CFS_REG_EXP_MATCH: Matched, AMBA_CFS_REG_EXP_NOT_MATCH: Not matched
 */
static inline UINT8 RegExpMatch_Star(const char *FileName, const char *RegExp, UINT32 *CurI, UINT32 *CurJ)
{
    UINT8 Ret = AMBA_CFS_REG_EXP_COMPARING;
    UINT32 i = *CurI;
    UINT32 j = *CurJ;
    if ((j == 0U) && (RegExp[1] == '.')) {
        if ((RegExp[2] == '*') && (RegExp[3] == '\0')) {
            // Ex: RegExp = "*.*"
            // Exception:
            //     For example, PrFile2 will find the file "C:\AMBA0001" when searching "*.*" even if the filename doesn't have dot.
            //     So always return "matched" in this special case.
            Ret = AMBA_CFS_REG_EXP_MATCH;
        } else if (RegExp[2] == '\0') {
            // Ex: RegExp = "*."
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Unsupported search pattern \"*.\"! AmbaFS's implementation is different from PrFile2's API doc", __func__, NULL, NULL, NULL, NULL);
            Ret = AMBA_CFS_REG_EXP_NOT_MATCH;
        } else {
            // Ex: RegExp = "*.MP4"
            /* Do nothing */
        }
    }
    if (Ret == AMBA_CFS_REG_EXP_COMPARING) {
        j++;
        if ((RegExp[j] == '*') || (RegExp[j] == '?')) {
            // Ex: RegExp = "**" or "*?"
            AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "RegExpMatch: [ERROR] RegExp[%u] %c", j, (UINT8) RegExp[j], 0, 0, 0);
            Ret = AMBA_CFS_REG_EXP_NOT_MATCH;
        } else if (RegExp[j] == '\0') {
            // Ex: RegExp = "*"
            Ret = AMBA_CFS_REG_EXP_MATCH;
        } else {
            // Ex: FileName = "C:\\AMBA0123", RegExp = "C:\\*0123"
            // TODO: Fix this method. Consider the following cases:
            //     RegExp = FileName = "C:\\AMBA0001", RegExp = "C:\\*0001", should be matched
            //     RegExp = FileName = "C:\\*ACAB*", RegExp = "C:\\*AC*", should be matched
            char Ch = AmbaCFS_UtilToUpper(RegExp[j]);
            for (UINT32 k = i; FileName[k] != '\0'; k++) {
                if (AmbaCFS_UtilToUpper(FileName[k]) == Ch) {
                    i = k;
                }
            }
            if (AmbaCFS_UtilToUpper(FileName[i]) != Ch) {
                Ret = AMBA_CFS_REG_EXP_NOT_MATCH;
            }
            i++;
            j++;
        }
    }

    *CurI = i;
    *CurJ = j;
    return Ret;
}

/**
 *  Check if the filename matches the regular expression
 *  AmbaFS is not matched with PrFile2's API doc => a.b. and .a.b. should not be included in "*.", but AmbaFS can get it
 *  @param [in] FileName            Filename
 *  @param [in] RegExp              Regular expression
 *  @return 1 - TRUE, 0 - FALSE
 */
static inline UINT8 RegExpMatch(const char *FileName, const char *RegExp)
{
    UINT8 Ret = 0U;
    if ((FileName[0] != '\0') && (RegExp[0] != '\0')) {
        UINT8 MatchResult = AMBA_CFS_REG_EXP_COMPARING;
        UINT32 i = 0U; // Current index of FileName
        UINT32 j = 0U; // Current index of RegExp
        while (MatchResult == AMBA_CFS_REG_EXP_COMPARING) {
            if (RegExp[j] == '\0') {
                if (FileName[i] != '\0') {
                    MatchResult = AMBA_CFS_REG_EXP_NOT_MATCH;
                } else {
                    MatchResult = AMBA_CFS_REG_EXP_MATCH;
                }
            } else if (RegExp[j] == '*') {
                MatchResult = RegExpMatch_Star(FileName, RegExp, &i, &j);
            } else if (RegExp[j] == '?') {
                if (FileName[i] == '*') {
                    MatchResult = AMBA_CFS_REG_EXP_NOT_MATCH;
                }
                i++;
                j++;
            } else if (RegExp[j] == '.') {
                if (FileName[i] != '.') {
                    MatchResult = AMBA_CFS_REG_EXP_NOT_MATCH;
                } else {
                    i++;
                    j++;
                    if ((RegExp[j] == '*') && (RegExp[j+1U] == '\0')) {
                        if (FileName[i] == '\0') {
                            MatchResult = AMBA_CFS_REG_EXP_NOT_MATCH;
                        } else {
                            MatchResult = AMBA_CFS_REG_EXP_MATCH;
                        }
                    }
                }
            } else if (SameAlphabet(FileName[i], RegExp[j]) != 0U) {
                i++;
                j++;
            } else {
                MatchResult = AMBA_CFS_REG_EXP_NOT_MATCH;
            }
        }

        if (MatchResult == AMBA_CFS_REG_EXP_MATCH) {
            Ret = 1U;
        }
    } else {
        AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "RegExpMatch: [ERROR] FileName[0] %c RegExp[0] %c", (UINT8) FileName[0], (UINT8) RegExp[0], 0, 0, 0);
    }
    return Ret;
}

/**
 *  Divide the filename into pathname/filename tokens
 *  @param [in] FullName            Full patch filename
 *  @param [out] Token              Pathname/filename tokens
 *  @param [out] NumToken           The returned number of tokens
 *  @return 0 - OK, others - Error
 */
static inline UINT32 ParseFileName(const char *FullName, const char *Token[AMBA_CFS_MAX_TREE_LEVEL], UINT32 *NumToken)
{
    UINT32 Ret = CFS_ERR_API;
    static char Args[UTIL_MAX_STR_TOKEN_NUMBER][UTIL_MAX_STR_TOKEN_LENGTH] GNU_SECTION_NOZEROINIT;
    UINT32 ArgCount = 0U;

    if (AmbaUtility_StringToken(FullName, AMBA_CFS_CACHE_DELIMETER, Args, &ArgCount) == OK) {
        if (ArgCount <= AMBA_CFS_MAX_TREE_LEVEL) {
            for (UINT32 i = 0U; i < ArgCount; ++i) {
                Token[i] = Args[i];
            }
            /* Set return value */
            *NumToken = ArgCount;
            Ret = CFS_OK;
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Too deep", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaUtility_StringToken fail", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Search for a child node with the specified filename
 *  @param [in] Parent              The parent node
 *  @param [in] FileName            The filename to search
 */
static AMBA_CFS_CACHE_TREE_NODE_s *SearchChildren(const AMBA_CFS_CACHE_TREE_NODE_s *Parent, const char *FileName)
{
    AMBA_CFS_CACHE_TREE_NODE_s *ChildNode = Parent->Child;
    while (ChildNode != NULL) {
        if (SameFileName(FileName, ChildNode->FileName) != 0U) {
            break;
        }
        ChildNode = ChildNode->Sibling;
    }
    /* Set return value */
    return ChildNode;
}

/**
 *  Search for a descendant node with the specified filename
 *  @param [in] Root                The root node
 *  @param [in] Token               Filename/pathname tokens
 *  @param [in] Depth               The depth of the fullpath filename
 *  @param [out] Node               The returned matched node. Return NULL when there's no match.
 *  @return 0 - OK, others - Error
 */
static UINT32 SearchDescendant(AMBA_CFS_CACHE_TREE_NODE_s *Root, const char * const Token[AMBA_CFS_MAX_TREE_LEVEL], UINT32 Depth, AMBA_CFS_CACHE_TREE_NODE_s **Node)
{
    UINT32 Ret = CFS_ERR_API;
    if ((Depth > 0U) && (Depth <= AMBA_CFS_MAX_TREE_LEVEL)) {
        if (SameFileName(Root->FileName, Token[0]) != 0U) {   /* Root node must be matched */
            AMBA_CFS_CACHE_TREE_NODE_s *SearchNode = Root;
            /* Search Tree */
            for (UINT32 i = 1U; i < Depth; i++) {
                /* check root in the same level */
                SearchNode = SearchNode->Child;
                for ( ; ; ) { /* Infinite loop */
                    UINT8 IsBreakLoop = 0U;
                    if (SearchNode == NULL) {
                        IsBreakLoop = 1U; /* Not found, end the loop */
                    } else if (SameFileName(Token[i], SearchNode->FileName) != 0U) {
                        IsBreakLoop = 1U; /* found, go to next level */
                    } else {
                        SearchNode = SearchNode->Sibling;
                    }

                    if (IsBreakLoop != 0U) {
                        break;
                    }
                }

                if (SearchNode == NULL) {
                    break; /* Not found, end the loop */
                }
            }
            /* Set return value */
            *Node = SearchNode;

            Ret = CFS_OK;
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] FileName not matched", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "SearchDescendant: [ERROR] Depth %u not supported", Depth, 0, 0, 0, 0);
    }
    return Ret;
}

/**
 *  Move the last used search to head of the search used list.
 *  @param [in] Search              The last used search record
 *  @return 0 - OK, others - Error
 */
static UINT32 TouchSearch(AMBA_CFS_CACHE_SEARCH_s *Search)
{
    UINT32 Ret = CFS_ERR_API;
    if (Search != NULL) {
        if (g_AmbaCFSCache.SearchUsedList != NULL) {    /* g_AmbaCFSCache.SearchUsedList wont be NULL here */
            /* If search is not the head of the used list */
            if (Search != g_AmbaCFSCache.SearchUsedList) {
                /* Move search to the head */
                if (Search->ListPrev != NULL) {
                    Search->ListPrev->ListNext = Search->ListNext;
                }
                if (Search->ListNext != NULL) {
                    Search->ListNext->ListPrev = Search->ListPrev;
                }
                Search->ListPrev = NULL;
                Search->ListNext = g_AmbaCFSCache.SearchUsedList;
                g_AmbaCFSCache.SearchUsedList->ListPrev = Search;
                g_AmbaCFSCache.SearchUsedList = Search;
            }
            Ret = CFS_OK;
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] SearchUsedList is NULL", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Search is NULL", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Release a search record.
 *  @param [in] Search              The search record to be released
 *  @return 0 - OK, others - Error
 */
static UINT32 ReleaseSearch(AMBA_CFS_CACHE_SEARCH_s *Search)
{
    UINT32 Ret = CFS_OK;
    if (Search->Host != NULL) {
        if (Search->Host->Search == Search) {
            Search->Host->Search = NULL;
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Host is corrupted", __func__, NULL, NULL, NULL, NULL);
            Ret = CFS_ERR_API;
        }
    }
    if (Ret == CFS_OK) {
        /* Remove Search from Used List */
        if (Search->ListPrev != NULL) {
            Search->ListPrev->ListNext = Search->ListNext;
        }
        if (Search->ListNext != NULL) {
            Search->ListNext->ListPrev = Search->ListPrev;
        }
        if (Search == g_AmbaCFSCache.SearchUsedList) {
            g_AmbaCFSCache.SearchUsedList = Search->ListNext;
        }
        /* Reset Search (Note the order of this operation because ListPrev and ListNext will be cleared) */
        AmbaUtility_MemorySetU8((UINT8 *)Search, 0, (UINT32)sizeof(AMBA_CFS_CACHE_SEARCH_s));
        /* Add Search to Available List */
        Search->ListNext = g_AmbaCFSCache.SearchAvailList;    /* no need to set ListPrev because avail list is a single linked list */
        g_AmbaCFSCache.SearchAvailList = Search;
    }
    /*
     * Do not change Active DTA when releasing a search because Active DTA should be protected by AmbaCFS mutex.
     * And this function might be called by AmbaCFS_Sch which does not lock AmbaCFS mutex.
     * Let CFS detect the release of search later in VerifySearch.
     */
    return Ret;
}

/**
 *  Release a node.
 *  @param [in] Node                The node to be released
 *  @return 0 - OK, others - Error
 */
static inline UINT32 ReleaseNode(AMBA_CFS_CACHE_TREE_NODE_s *Node)
{
    UINT32 Ret = CFS_OK;
    /* Release search record */
    if (Node->Search != NULL) {
        Ret = ReleaseSearch(Node->Search);
    }
    if (Ret == CFS_OK) {
        /* no matter if error occurs, go ahead to simplify the normal flow (no need to harm the normal flow, because fatal error should never happen) */
        /* Add Node to available list */
        Node->ListNext = g_AmbaCFSCache.NodeAvailList;    /* No need to set ListPrev because avail list is a single linked list */
        g_AmbaCFSCache.NodeAvailList = Node;
    }
    return Ret;
}

/**
 *  Delete a node from cache tree.
 *  @param [in] Node                The tree node to be deleted
 *  @return 0 - OK, others - Error
 */
static UINT32 DelNode(AMBA_CFS_CACHE_TREE_NODE_s *Node)
{
    UINT32 Ret = CFS_OK;
    if (Node != NULL) {
        if (Node->Parent != NULL) {
            AMBA_CFS_CACHE_TREE_NODE_s *Parent = Node->Parent;
            /* remove node from parent's child list */
            if (Parent->Child == Node) {
                /* this node is parent's child, change parent's child to sibling */
                Parent->Child = Node->Sibling;
            } else {
                /* set prev's sibling to this node's sibling */
                AMBA_CFS_CACHE_TREE_NODE_s *Prev = Parent->Child;
                while (Prev->Sibling != Node) {
                    Prev = Prev->Sibling;
                }
                Prev->Sibling = Node->Sibling;
            }
            if (Parent->Search != NULL) {
                /* parent's search record could include this Node, release parent's search record */
                // TODO: not to remove search unless the node would break the search
                Ret = ReleaseSearch(Parent->Search);
            }
            if (Ret == CFS_OK) {
                Ret = ReleaseNode(Node);
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Node->Parent is NULL", __func__, NULL, NULL, NULL, NULL);
            Ret = CFS_ERR_API;
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Node is NULL", __func__, NULL, NULL, NULL, NULL);
        Ret = CFS_ERR_API;
    }
    return Ret;
}

/**
 *  Delete the descendants
 *  @param [in] Node                The root node of the tree to be deleted
 *  @return 0 - OK, others - Error
 */
static UINT32 DelDescendant(const AMBA_CFS_CACHE_TREE_NODE_s *Node)
{
    UINT32 Ret = CFS_OK;
    AMBA_CFS_CACHE_TREE_NODE_s *CurNode = Node->Child;
    if (CurNode != NULL) {
        while (CurNode != Node) {
            if (CurNode->Child != NULL) {
                CurNode = CurNode->Child; /* Move to Child */
            } else {
                AMBA_CFS_CACHE_TREE_NODE_s *Sibling = CurNode->Sibling;
                AMBA_CFS_CACHE_TREE_NODE_s *Parent = CurNode->Parent;
                Ret = DelNode(CurNode);
                if (Ret != CFS_OK) {
                    break;  /* fatal error, no error handling */
                }
                if (Sibling != NULL) {
                    CurNode = Sibling; /* Move to Sibling */
                } else {
                    CurNode = Parent; /* Move to Parent */
                }
            }
        }
    }
    return Ret;
}

/**
 *  Get a free tree node. (can fail if cannot release a tree due to not enough search record => fatal error)
 *  @param [out] FreeNode           The returned free node
 *  @return 0 - OK, others - Error
 */
static UINT32 GetFreeNode(AMBA_CFS_CACHE_TREE_NODE_s **FreeNode)
{
    UINT32 Ret = CFS_OK;
    AMBA_CFS_CACHE_TREE_NODE_s *ListNode = g_AmbaCFSCache.NodeAvailList;
    if (ListNode != NULL) {
        /* Remove ListNode from available list */
        g_AmbaCFSCache.NodeAvailList = ListNode->ListNext; /* No need to set ListPrev because avail list is a single linked list */
        /* Set return value */
        *FreeNode = ListNode;
    } else {
        /* fatal error, no error handling */
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] No available node", __func__, NULL, NULL, NULL, NULL);
        Ret = CFS_ERR_API;
    }
    return Ret;
}

/**
 *  Insert a child node. (can fail if cannot release a tree due to not enough search record => fatal error)
 *  @param [in] Parent              The parent node
 *  @param [in] FileName            Filename of the child node
 *  @param [out] InsertNode         The returned new node
 *  @return 0 - OK, others - Error
 */
static UINT32 InsertChildNode(AMBA_CFS_CACHE_TREE_NODE_s *Parent, const char *FileName, AMBA_CFS_CACHE_TREE_NODE_s **InsertNode)
{
    UINT32 Ret;
    AMBA_CFS_CACHE_TREE_NODE_s *Node = NULL;
    Ret = GetFreeNode(&Node);
    if (Ret == CFS_OK) {
        /* Initialize new node */
        AmbaCFS_UtilCopyFileName(Node->FileName, FileName);
        Node->Cache.ValidBits = 0U;
        Node->Child  = NULL;
        Node->Parent = Parent;
        Node->Search = NULL;
        /* Insert the node to tree */
        if (Parent->Search != NULL) {
            /* Only happens on fstat() inserts child node, and some nodes are not browsed yet */
            /* Insert to the position right after the Search->EndPos, to avoid impacting first/next */
            /* Insert to the tail is also ok but slower */
            if (Parent->Search->EndPos != NULL) {
                Node->Sibling = Parent->Search->EndPos->Sibling;
                Parent->Search->EndPos->Sibling = Node;
            } else {
                /* Only happens when search first not found but inserts parent node. (Search next won't change EndPos to NULL) */
                /* The nodes are not browsed yet, so it's ok to insert new node to the head. */
                /* EndPos represents the last search result, so do not change EndPos here. */

                /* Insert to the front of the child list */
                Node->Sibling = Parent->Child;
                Parent->Child = Node;
            }
        } else {
            /* Insert to the front of the child list */
            Node->Sibling = Parent->Child;
            Parent->Child = Node;
        }
        /* Set return value */
        *InsertNode = Node;
    }
    return Ret;
}

/**
 *  Insert nodes with the specified filename into the cache tree. (can fail if cannot release a subtree due to not enough search record => fatal error)
 *  @param [in] Node                Root of the cache tree
 *  @param [in] Path                The path tokens
 *  @param [in] Depth               The depth of the ptch
 *  @param [out] InsertNode         The returned leaf node
 *  @return 0 - OK, others - Error
 */
static UINT32 InsertPathNodes(AMBA_CFS_CACHE_TREE_NODE_s *Node, const char * const Path[AMBA_CFS_MAX_TREE_LEVEL], UINT32 Depth, AMBA_CFS_CACHE_TREE_NODE_s **InsertNode)
{
    // TODO: support to insert from an intermediate node
    UINT32 Ret = CFS_OK;
    if ((Depth > 0U) && (Depth <= AMBA_CFS_MAX_TREE_LEVEL)) {
        AMBA_CFS_CACHE_TREE_NODE_s *RetNode = Node; /* Return value */
        if (SameFileName(RetNode->FileName, Path[0]) != 0U) {
            AMBA_CFS_CACHE_TREE_NODE_s *Child;
            for (UINT32 i = 1U; i < Depth; i++) {
                /* Check if there is a corresponding child node exist */
                Child = RetNode->Child;
                while (Child != NULL) {
                    if (SameFileName(Path[i], Child->FileName) != 0U) {
                        break;
                    }
                    Child = Child->Sibling;
                }
                if (Child != NULL) {    /* node of the filename found, move to child node */
                    RetNode = Child;
                } else {                /* node of the filename not found */
                    Ret = InsertChildNode(RetNode, Path[i], &RetNode);
                    if (Ret != CFS_OK) {
                        break;  /* fatal error, ignore error handling */
                    }
                }
            }
            /* Set return value */
            if (Ret == CFS_OK) {
                *InsertNode = RetNode;
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] FileName not matched", __func__, NULL, NULL, NULL, NULL);
            /* fatal error, ignore error handling */
            Ret = CFS_ERR_API;
        }
    } else {
        AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "InsertPathNodes: [ERROR] Depth %u not supported", Depth, 0, 0, 0, 0);
        /* fatal error, ignore error handling */
        Ret = CFS_ERR_API;
    }
    return Ret;
}

/*
 * Functions for searching Directory Entry
 */

/**
 *  Get a free search node.
 *  @param [out] FreeSearch         The returned free search node (wont be NULL)
 *  @return 0 - OK, others - Error
 */
static UINT32 GetFreeSearch(AMBA_CFS_CACHE_SEARCH_s **FreeSearch)
{
    UINT32 Ret = CFS_OK;
    AMBA_CFS_CACHE_SEARCH_s *Search = g_AmbaCFSCache.SearchAvailList;
    if (Search != NULL) {
        /* Remove the Search from Available List */
        g_AmbaCFSCache.SearchAvailList = Search->ListNext;
        /* Add the Search to the head of Used List */
        Search->ListPrev = NULL;
        Search->ListNext = g_AmbaCFSCache.SearchUsedList;
        if (g_AmbaCFSCache.SearchUsedList != NULL) {
            g_AmbaCFSCache.SearchUsedList->ListPrev = Search;
        }
        g_AmbaCFSCache.SearchUsedList = Search;
        /* Set return value */
        *FreeSearch = Search;
    } else {
        /* fatal error, no error handling */
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] No available search", __func__, NULL, NULL, NULL, NULL);
        Ret = CFS_ERR_API;
    }
    return Ret;
}

/**
 *  Check if the 1st rule is included by the 2nd rule
 *  @param [in] RegExp1             The 1st regular expression
 *  @param [in] Attr1               The 1st attribute
 *  @param [in] RegExp2             The 2nd regular expression
 *  @param [in] Attr2               The 2nd attribute
 *  @return 1 - TRUE, 0 - FALSE
 */
static inline UINT8 IncludeMatch(const char *RegExp1, UINT32 Attr1, const char *RegExp2, UINT32 Attr2)
{
    UINT8 Ret = 0U;
    if (RegExpMatch(RegExp1, RegExp2) != 0U) {
        if ((Attr1 & Attr2) == Attr1) {
            Ret = 1U;
        }
    }
    return Ret;
}

/**
 *  Get the next node which matches the search condition. (from node and its sibling)
 *  @param [in] Node                The start node
 *  @param [in] RegExp              Regular expression
 *  @param [in] Attribute           The attribute
 *  @param [out] MatchedNode        The returned child node
 *  @return 0 - OK, others - Error
 */
static UINT32 GetMatchedNode(AMBA_CFS_CACHE_TREE_NODE_s *Node, const char *RegExp, UINT8 Attribute, AMBA_CFS_CACHE_TREE_NODE_s **MatchedNode)
{
    UINT32 Ret = CFS_ERR_API;
    /* This function should be called when cache hit, so there must be search result (Node->Parent->Search != NULL). */
    if (Node != NULL) {
        if (Node->Parent != NULL) {
            if (Node->Parent->Search != NULL) {
                /* EndPos is the last search result, so search from Node to EndPos for a matched result. */
                if (Node->Parent->Search->EndPos != NULL) {
                    AMBA_CFS_CACHE_TREE_NODE_s *RetNode = Node;
                    const AMBA_CFS_CACHE_TREE_NODE_s *End = Node->Parent->Search->EndPos->Sibling;
                    Ret = CFS_ERR_OBJ_UNAVAILABLE; /* Return OBJ_UNAVAILABLE when there's no match */
                    while ((RetNode != End) && (RetNode != NULL)) {
                        if (IncludeMatch(RetNode->FileName, RetNode->Cache.Dta.Attribute, RegExp, Attribute) != 0U) {
                            /* Set return value */
                            *MatchedNode = RetNode;
                            Ret = CFS_OK;
                            break;
                        }
                        RetNode = RetNode->Sibling;
                    }
                } else {
                    /*
                     * EndPos is NULL when there's no match in the last search, so return OBJ_UNAVAILABLE.
                     * It's possible in the following code flow:
                     * 1. Assuming there's only a file '0000' in C:\DCIM\100MEDIA
                     * 2. Search "C:\DCIM\100MEDIA\9999" not found.
                     *    Cache miss. The search is cached in Node->Parent->Search with EndPos == NULL in FirstDirEnt_SearchAndCacheDta.
                     * 3. Get file info of "C:\DCIM\100MEDIA\0000"
                     *    Create a node '0000' under '100MEDIA' in AmbaCFS_CacheGetFileInfoImpl.
                     * 4. Search "C:\DCIM\100MEDIA\9999" again.
                     *    Cache hit. This function is called in FirstDirEnt_GetDataFromCache because Parent->Child is created in step3.
                     * Note that if we skip step3, this function won't be called because Parent->Child == NULL.
                     */
                    Ret = CFS_ERR_OBJ_UNAVAILABLE;
                }
            } else {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Node->Parent->Search is NULL", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Node->Parent is NULL", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Node is NULL", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/*
 *   Cache APIs
 */

UINT32 AmbaCFS_CacheGetInitBufferSize(UINT32 CacheMaxDirNum, UINT32 CacheMaxFileNum)
{
    // a directory includes 3 nodes (XXX, ., ..)
    UINT32 Size = ((CacheMaxDirNum * 3U) + CacheMaxFileNum) * (UINT32)sizeof(AMBA_CFS_CACHE_TREE_NODE_s);   /* tree nodes */
    Size += CacheMaxDirNum * (UINT32)sizeof(AMBA_CFS_CACHE_SEARCH_s);  /* Search parameters */
#if 0
    AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "===== CacheGetInitBufferSize ======", 0, 0, 0, 0, 0);
    AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "CacheBufferSize = %u", Size, 0, 0, 0, 0);
    AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "                = (CacheMaxDirNum(%u)+CacheMaxFileNum(%u))*AMBA_CFS_CACHE_TREE_NODE_s(%u)", CacheMaxDirNum, CacheMaxFileNum, sizeof(AMBA_CFS_CACHE_TREE_NODE_s), 0, 0);
    AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "                + CacheMaxDirNum(%d)*AMBA_CFS_CACHE_SEARCH_s(%d)", CacheMaxDirNum, sizeof(AMBA_CFS_CACHE_SEARCH_s), 0, 0, 0);
    AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "== End of CacheGetInitBufferSize ==", 0, 0, 0, 0, 0);
#endif
    return Size;
}

/**
 *  Get default config for cache module.
 *  @param [out] Config             Address of a config
 */
void AmbaCFS_CacheGetInitDefaultCfg(AMBA_CFS_INIT_CFG_s *Config)
{
    Config->CacheEnable = 1U;
    Config->CacheMaxDirNum = AMBA_CFS_CACHE_MAX_NUM_DIR_DEFAULT;
    Config->CacheMaxFileNum = AMBA_CFS_CACHE_MAX_NUM_FILE_DEFAULT;
}

/**
 *  Initialize drive cache.
 *  @param [in]  Drive              Drive name (from A to Z)
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaCFS_CacheInitDrive(char Drive)
{
    UINT32 Ret;
    UINT32 DriveNum;
    Ret = AmbaCFS_UtilGetDriveNum(Drive, &DriveNum);
    if (Ret == CFS_OK) {
        char File[3] = "C:";
        /* Initialize Root Node */
        File[0] = Drive;
        AmbaCFS_UtilCopyFileName(g_AmbaCFSCache.Root[DriveNum].FileName, File);
        g_AmbaCFSCache.Root[DriveNum].Cache.ValidBits = 0U;
        g_AmbaCFSCache.Root[DriveNum].Search  = NULL;
        g_AmbaCFSCache.Root[DriveNum].Parent  = NULL;
        g_AmbaCFSCache.Root[DriveNum].Sibling = NULL;
        g_AmbaCFSCache.Root[DriveNum].Child   = NULL;
        g_AmbaCFSCache.DevInfCached[DriveNum] = 0U;
    }
    return Ret;
}

/**
 *  Check parameters.
 *  @param [in]  Buffer The address of the work buffer
 *  @param [in]  BufferSize The size of the work buffer
 *  @param [in]  CacheMaxDirNum     The maximum number of cache directories
 *  @param [in]  CacheMaxFileNum    The maximum number of cache files
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaCFS_CacheInit_ParamCheck(const UINT8 *Buffer, UINT32 BufferSize, UINT32 CacheMaxDirNum, UINT32 CacheMaxFileNum)
{
    UINT32 Ret = CFS_ERR_ARG;
    if (Buffer != NULL) {
        if ((CacheMaxDirNum > 0U) && (CacheMaxFileNum > 0U)) {
            if (BufferSize == AmbaCFS_CacheGetInitBufferSize(CacheMaxDirNum, CacheMaxFileNum)) {
                Ret = CFS_OK;
            } else {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] BufferSize is incorrect", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] CacheMaxDirNum or CacheMaxFileNum is incorrect", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Buffer is NULL", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

UINT32 AmbaCFS_CacheInit(const UINT8 *Buffer, UINT32 BufferSize, UINT32 CacheMaxDirNum, UINT32 CacheMaxFileNum)
{
    UINT32 Ret;
    Ret = AmbaCFS_CacheInit_ParamCheck(Buffer, BufferSize, CacheMaxDirNum, CacheMaxFileNum);
    if (Ret == CFS_OK) {
        static char CfsCacheMutex[16] = "CfsCacheMutex";
        AmbaUtility_MemorySetU8((UINT8 *) &g_AmbaCFSCache, 0, (UINT32)sizeof(AMBA_CFS_CACHE_s));
        Ret = CFS_K2C(AmbaKAL_MutexCreate(&g_AmbaCFSCache.Mutex, CfsCacheMutex));
        if (Ret == CFS_OK) {
            UINT32 i;
            char Ch = 'A';
            const AMBA_CFS_CACHE_TREE_NODE_s *Node;
            /* Get Cache related parameters and allocate memory */
            g_AmbaCFSCache.CacheMaxNodeNum = (CacheMaxDirNum * 3U) + CacheMaxFileNum;
            g_AmbaCFSCache.CacheMaxDirNum = CacheMaxDirNum;
            AmbaMisra_TypeCast(&g_AmbaCFSCache.NodeCache, &Buffer);
            g_AmbaCFSCache.NodeAvailList = g_AmbaCFSCache.NodeCache;
            Node = &g_AmbaCFSCache.NodeAvailList[g_AmbaCFSCache.CacheMaxNodeNum];
            AmbaMisra_TypeCast(&g_AmbaCFSCache.SearchCache, &Node);
            g_AmbaCFSCache.SearchAvailList = g_AmbaCFSCache.SearchCache;
            g_AmbaCFSCache.SearchUsedList = NULL;
            /* Initialize available node list */
            for (i = 0U; i < (g_AmbaCFSCache.CacheMaxNodeNum - 1U); i++) {
                g_AmbaCFSCache.NodeAvailList[i].ListNext = &g_AmbaCFSCache.NodeAvailList[i + 1U];
            }
            g_AmbaCFSCache.NodeAvailList[i].ListNext = NULL;
            /* Initialize available Search List */
            for (i = 0U; i < (g_AmbaCFSCache.CacheMaxDirNum - 1U); i++) {
                g_AmbaCFSCache.SearchAvailList[i].ListNext = &g_AmbaCFSCache.SearchAvailList[i + 1U];
            }
            g_AmbaCFSCache.SearchAvailList[i].ListNext = NULL;
            /* Initialize Drive Cache */
            for (i = 0U; i < AMBA_CFS_MAX_DRIVE_AMOUNT; i++) {
                Ret = AmbaCFS_CacheInitDrive(Ch);
                if (Ret != CFS_OK) {
                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_CacheInitDrive fail", __func__, NULL, NULL, NULL, NULL);
                    break;
                }
                Ch++;
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaKAL_MutexCreate fail", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Ret;
}

/**
 *  Update cache tree for the remove command.
 *  @param [in]  FileName           Filename
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaCFS_CacheRemoveImpl(const char *FileName)
{
    UINT32 Ret;
    /* Check and remove the node of FileName */
    char DriveName;
    Ret = AmbaCFS_UtilGetDrive(FileName, &DriveName);
    if (Ret == CFS_OK) {
        UINT32 DriveId;
        Ret = AmbaCFS_UtilGetDriveNum(DriveName, &DriveId);
        if (Ret == CFS_OK) {
            char Buffer[AMBA_CFS_MAX_FILENAME_LENGTH];
            const char *Token[AMBA_CFS_MAX_TREE_LEVEL];
            UINT32 Depth;
            /* Invalidate DevInf */
            g_AmbaCFSCache.DevInfCached[DriveId] = 0U;
            /* Invalidate FILE_INFO and DTA */
            AmbaCFS_UtilCopyFileName(Buffer, FileName);
            Ret = ParseFileName(Buffer, Token, &Depth);
            if (Ret == CFS_OK) {
                if (Depth > 1U) {
                    /* find the node of the parent directory */
                    AMBA_CFS_CACHE_TREE_NODE_s *Parent = NULL;
                    Ret = SearchDescendant(&g_AmbaCFSCache.Root[DriveId], Token, Depth - 1U, &Parent);
                    if (Ret == CFS_OK) {
                        if (Parent != NULL) {
                            /* Parent directory is in the cache tree, find node from parent */
                            AMBA_CFS_CACHE_TREE_NODE_s * const Node = SearchChildren(Parent, Token[Depth - 1U]);
                            if (Node != NULL) {
                                /* FileName is in the cache tree */
                                Ret = DelDescendant(Node);
                                if (Ret == CFS_OK) {
                                    Ret = DelNode(Node);
                                }
                            } else {
                                /* Release parent's Search record */
                                // TODO: not to remove search unless the node would break the search
                                if (Parent->Search != NULL) {
                                    Ret = ReleaseSearch(Parent->Search);
                                }
                            }
                        }
                    }
                } else {
                    /* Can not remove root! */
                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Cannot remove root", __func__, NULL, NULL, NULL, NULL);
                    Ret = CFS_ERR_API;
                }
            }
        }
    }
    return Ret;
}

/**
 *  Check parameters.
 *  @param [in]  FileName           Filename
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaCFS_CacheRemove_ParamCheck(const char *FileName)
{
    UINT32 Ret = CFS_ERR_ARG;
    if (AmbaCFS_UtilIsValidPath(FileName) != 0U) { /* Check file path */
        Ret = CFS_OK;
    }
    return Ret;
}

/**
 *  Update cache tree for the remove command.
 *  @param [in]  FileName           Filename
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_CacheRemove(const char *FileName)
{
    UINT32 Ret;
    Ret = AmbaCFS_CacheRemove_ParamCheck(FileName);
    if (Ret == CFS_OK) {
        Ret = AmbaCFS_CacheLock();
        if (Ret == CFS_OK) {
            Ret = AmbaCFS_CacheRemoveImpl(FileName);
            if (AmbaCFS_CacheUnlock() != CFS_OK) {
                Ret = CFS_ERR_FATAL;
            }
        }
    }
    return Ret;
}

/**
 *  Cache a file stat
 *  @param [in] Info                The file STAT to be cached
 *  @param [in] Node                The node correspond to the file STAT
 */
static inline void Cache_FileInfo(const AMBA_CFS_FILE_INFO_s *Info, AMBA_CFS_CACHE_TREE_NODE_s *Node)
{
#if AMBA_CFS_CACHE_DEBUG
    char InfoStr[64];
    UINT32 ArgU32[3];
    ArgU32[0] = (UINT32)Info->Size;
    ArgU32[1] = Info->Attr;
    ArgU32[2] = Node->Cache.ValidBits;
    (void)AmbaUtility_StringPrintUInt32(InfoStr, sizeof(InfoStr), "S:%u A:0x%X V=0x%X", 3, ArgU32);
    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "Cache_FileInfo(%s) : %s", Node->FileName, InfoStr, NULL, NULL, NULL);
#endif
    Node->Cache.FileInfo = *Info;
    Node->Cache.ValidBits |= AMBA_CFS_CACHE_VALIDBIT_FILE_INFO;
}

/**
 *  Retrieve a file stat from cache
 *  @param [out] Info               The file STAT
 *  @param [in] Node                The node correspond to the file STAT
 *  @return 0 - OK, others - Error
 */
static UINT32 Retrieve_Cached_FileInfo(AMBA_CFS_FILE_INFO_s *Info, const AMBA_CFS_CACHE_TREE_NODE_s *Node)
{
    UINT32 Ret = CFS_OK;
    if ((Node->Cache.ValidBits & AMBA_CFS_CACHE_VALIDBIT_FILE_INFO) != 0U) {
        *Info = Node->Cache.FileInfo;
#if AMBA_CFS_CACHE_DEBUG
        {
            char InfoStr[64];
            UINT32 ArgU32[3];
            ArgU32[0] = (UINT32)Info->Size;
            ArgU32[1] = Info->Attr;
            ArgU32[2] = Node->Cache.ValidBits;
            (void)AmbaUtility_StringPrintUInt32(InfoStr, sizeof(InfoStr), "S:%u A:0x%X V=0x%X", 3, ArgU32);
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "Retrieve_Cached_FileInfo(%s) : %s", Node->FileName, InfoStr, NULL, NULL, NULL);
        }
#endif
    } else {
        AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "Retrieve_Cached_FileInfo: [ERROR] ValidBits 0x%X AMBA_CFS_CACHE_VALIDBIT_FILE_INFO 0x%X", Node->Cache.ValidBits, AMBA_CFS_CACHE_VALIDBIT_FILE_INFO, 0, 0, 0);
        Ret = CFS_ERR_API;
    }
    return Ret;
}

/**
 *  The implementation of getting file stat information from cache.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in]  FileName           Filename
 *  @param [in]  Info               Pointer of a info record
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaCFS_CacheGetFileInfoImpl(const char *FileName, AMBA_CFS_FILE_INFO_s *Info)
{
    UINT32 Ret;
    char DriveName;
    Ret = AmbaCFS_UtilGetDrive(FileName, &DriveName);
    if (Ret == CFS_OK) {
        UINT32 DriveId;
        Ret = AmbaCFS_UtilGetDriveNum(DriveName, &DriveId);
        if (Ret == CFS_OK) {
            const char *Token[AMBA_CFS_MAX_TREE_LEVEL];
            char Buffer[AMBA_CFS_MAX_FILENAME_LENGTH];
            UINT32 Depth;
            AmbaCFS_UtilCopyFileName(Buffer, FileName);
            Ret = ParseFileName(Buffer, Token, &Depth);
            if (Ret == CFS_OK) {
                AMBA_CFS_CACHE_TREE_NODE_s * const Root = &g_AmbaCFSCache.Root[DriveId];
                AMBA_CFS_CACHE_TREE_NODE_s *Node = NULL;
                /* Search FileName from the cache tree */
                Ret = SearchDescendant(Root, Token, Depth, &Node);
                if (Ret == CFS_OK) {
                    if ((Node != NULL) && ((Node->Cache.ValidBits & AMBA_CFS_CACHE_VALIDBIT_FILE_INFO) != 0U)) {
                        // AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: Data is from cache!", __func__, NULL, NULL, NULL, NULL);
                        Ret = Retrieve_Cached_FileInfo(Info, Node);
                    } else {
                        Ret = CFS_F2C(AmbaFS_GetFileInfo(FileName, Info));
                        if (Ret == CFS_OK) {
                            if (Node == NULL) {
                                // AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: Insert Nodes to cache!", __func__, NULL, NULL, NULL, NULL);
                                Ret = InsertPathNodes(Root, Token, Depth, &Node);
                            }
                            if (Ret == CFS_OK) {
                                // AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: Put data to cache!", __func__, NULL, NULL, NULL, NULL);
                                Cache_FileInfo(Info, Node);
                            }
                        } else if (Ret == CFS_ERR_OBJ_UNAVAILABLE) { /* No such file or directory */
                            /* The file does not exist */
                        } else {
                            AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_CacheGetFileInfoImpl: [ERROR] Failed to get the file info. ErrNum: %p", AmbaCFS_UtilGetAmbaFSError(), 0, 0, 0, 0);
                        }
                    }
                }
            }
        }
    }
    return Ret;
}

/**
 *  Get file stat information from cache.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in]  FileName           Filename
 *  @param [in]  Info               Pointer of a info record
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_CacheGetFileInfo(const char *FileName, AMBA_CFS_FILE_INFO_s *Info)
{
    UINT32 Ret;
    Ret = AmbaCFS_CacheLock();
    if (Ret == CFS_OK) {
        Ret = AmbaCFS_CacheGetFileInfoImpl(FileName, Info);
        if (AmbaCFS_CacheUnlock() != CFS_OK) {
            Ret = CFS_ERR_FATAL;
        }
    }
    return Ret;
}

/**
 *  Copy DTA data to cache
 *  @param [in] ActiveDta           The Active DTA
 *  @param [in] Node                The node correspond to the file STAT
 *  @param [in] Search              The search record
 *  @return 0 - OK, others - Error
 */
static UINT32 Cache_Dta(AMBA_CFS_ACTIVE_DTA_s *ActiveDta, AMBA_CFS_CACHE_TREE_NODE_s *Node, AMBA_CFS_CACHE_SEARCH_s *Search)
{
    UINT32 Ret = CFS_OK;
    const AMBA_CFS_DTA_s *DirEntry = ActiveDta->DirEntry;
    AMBA_CFS_CACHE_DTA_s * const CachedDta = &(Node->Cache.Dta);
#if AMBA_CFS_CACHE_DEBUG
    char DtaStr[64];
    UINT32 ArgU32[3];
    ArgU32[0] = (UINT32)DirEntry->FileSize;
    ArgU32[1] = DirEntry->Attribute;
    ArgU32[2] = Node->Cache.ValidBits;
    (void)AmbaUtility_StringPrintUInt32(DtaStr, sizeof(DtaStr), "S:%u A:0x%X V=0x%X", 3, ArgU32);
    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "Cache_Dta(%s) : %s", DirEntry->FileName, DtaStr, NULL, NULL, NULL);
#endif
    CachedDta->Time = DirEntry->Time;
    CachedDta->Date = DirEntry->Date;
    CachedDta->FileSize = DirEntry->FileSize;
    CachedDta->Attribute = DirEntry->Attribute;
    Node->Cache.ValidBits |= AMBA_CFS_CACHE_VALIDBIT_DTA;
    /* Keep the current node and current search in ActiveDta (for binding ActiveDta with search) */
    AmbaCFS_ActiveDtaSetCacheInfo(ActiveDta, Search, Node);
    /* Bind search together with ActiveDta */
    Search->ActiveDta = ActiveDta;
    return Ret;
}

/**
 *  Retrieve a DTA from cache
 *  @param [out] ActiveDta          The Active DTA
 *  @param [in] Node                The node correspond to the file STAT
 *  @param [in] Search              The search record
 *  @return 0 - OK, others - Error
 */
static UINT32 Retrieve_Cached_Dta(AMBA_CFS_ACTIVE_DTA_s *ActiveDta, AMBA_CFS_CACHE_TREE_NODE_s *Node, AMBA_CFS_CACHE_SEARCH_s *Search)
{
    UINT32 Ret = CFS_OK;
    if ((Node->Cache.ValidBits & AMBA_CFS_CACHE_VALIDBIT_DTA) != 0U) {
        AMBA_CFS_DTA_s *DirEntry = ActiveDta->DirEntry;
        const AMBA_CFS_CACHE_DTA_s * const CachedDta = &(Node->Cache.Dta);
        DirEntry->Time = CachedDta->Time;
        DirEntry->Date = CachedDta->Date;
        DirEntry->FileSize = CachedDta->FileSize;
        AmbaCFS_UtilCopyFileName(DirEntry->FileName, Node->FileName);
        DirEntry->Attribute = CachedDta->Attribute;
        /* Keep the current node and current search in ActiveDta (for binding ActiveDta with search) */
        AmbaCFS_ActiveDtaSetCacheInfo(ActiveDta, Search, Node);
        /* Bind search together with ActiveDta */
        Search->ActiveDta = ActiveDta;
#if AMBA_CFS_CACHE_DEBUG
        {
            char DtaStr[64];
            UINT32 ArgU32[3];
            ArgU32[0] = (UINT32)DirEntry->FileSize;
            ArgU32[1] = DirEntry->Attribute;
            ArgU32[2] = Node->Cache.ValidBits;
            (void)AmbaUtility_StringPrintUInt32(DtaStr, sizeof(DtaStr), "S:%u A:0x%X V=0x%X", 3, ArgU32);
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "Retrieve_Cached_Dta(%s) : %s", DirEntry->FileName, DtaStr, NULL, NULL, NULL);
        }
#endif
    } else {
        AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "Retrieve_Cached_Dta: [ERROR] ValidBits 0x%X AMBA_CFS_CACHE_VALIDBIT_DTA 0x%X", Node->Cache.ValidBits, AMBA_CFS_CACHE_VALIDBIT_DTA, 0, 0, 0);
        Ret = CFS_ERR_API;
    }
    return Ret;
}

/**
 *  Reuse search record.
 *  @param [in] Parent              The parent node
 *  @param [in] RegExp              Regular expression
 *  @param [in] Attribute           The attribute
 *  @param [out] ActiveDta          The Active DTA
 *  @return 0 - OK, others - Error
 */
static UINT32 FirstDirEnt_GetDataFromCache(const AMBA_CFS_CACHE_TREE_NODE_s *Parent, const char *RegExp, UINT8 Attribute, AMBA_CFS_ACTIVE_DTA_s *ActiveDta)
{
    UINT32 Ret = CFS_OK;
    /* Get the first matched child node */
    if (Parent->Child != NULL) {
        AMBA_CFS_CACHE_TREE_NODE_s *Node = NULL;
        Ret = GetMatchedNode(Parent->Child, RegExp, Attribute, &Node);
        if (Ret == CFS_OK) {
            AMBA_CFS_CACHE_SEARCH_s * const Search = Parent->Search;
            // AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: Get DTA from cache", __func__, NULL, NULL, NULL, NULL);
            /* Get data from cache */
            Ret = Retrieve_Cached_Dta(ActiveDta, Node, Search);
            if (Ret == CFS_OK) {
                /* Move current search to head of the search used list */
                Ret = TouchSearch(Search);
            }
        }
    } else {
        // AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: No more data in the cache!", __func__, NULL, NULL, NULL, NULL);
        Ret = CFS_ERR_OBJ_UNAVAILABLE;
    }
    return Ret;
}

/**
 *  Initiate the search record of a node
 *  @param [in] Parent              The parent node
 *  @param [in] Node                The node of the search record
 *  @param [in] RegExp              Regular expression
 *  @param [in] Attribute           The attribute of the search
 */
static void InitSearch(AMBA_CFS_CACHE_TREE_NODE_s *Parent, AMBA_CFS_CACHE_TREE_NODE_s *Node, const char *RegExp, UINT8 Attribute)
{
    AMBA_CFS_CACHE_SEARCH_s * const Search = Parent->Search;
    Search->Host = Parent;
    Search->EndPos = Node;
    Search->Attr = Attribute;
    Search->CacheStatus = AMBA_CFS_CACHE_SEARCH_STATUS_CREATING;
    AmbaCFS_UtilCopyFileName(Search->RegExp, RegExp);
}

/**
 *  Prepare a child node (can fail if cannot release a tree due to not enough search record => fatal error)
 *  @param [in] Parent              The parent node
 *  @param [in] FileName            The filename
 *  @param [out] ChildNode          The returned child node
 *  @return 0 - OK, others - Error
 */
static UINT32 PrepareChildNode(AMBA_CFS_CACHE_TREE_NODE_s *Parent, const char *FileName, AMBA_CFS_CACHE_TREE_NODE_s **ChildNode)
{
    UINT32 Ret;
    AMBA_CFS_CACHE_TREE_NODE_s *Node = Parent->Child;
    AMBA_CFS_CACHE_TREE_NODE_s *Prev = NULL;
    while (Node != NULL) {
        if (SameFileName(FileName, Node->FileName) != 0U) {
            break;
        }
        Prev = Node;
        Node = Node->Sibling;
    }
    if (Node != NULL) {
        // AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: Reuse the child node!", __func__, NULL, NULL, NULL, NULL);
        if (Node != Parent->Child) {
            /* let it be the new child */
            if (Prev != NULL) {
                Prev->Sibling = Node->Sibling;  /* remove node from child list */
            }
            Node->Sibling = Parent->Child;      /* Insert to the node to the head of child list */
            Parent->Child = Node;
        }
        Ret = CFS_OK;
    } else {
        // AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: Insert a child Node to the path!", __func__, NULL, NULL, NULL, NULL);
        Ret = InsertChildNode(Parent, FileName, &Node);
    }
    /* Set return value */
    if (Ret == CFS_OK) {
        *ChildNode = Node;
    }
    return Ret;
}

/**
 *  Search the first file in cache
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] Name                The file name
 *  @param [in] Attr                The attribute
 *  @param [out] DirEntry           The DTA record
 *  @param [in,out] ActiveDta       The Active DTA to keep the FS search. Return the FS search.
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_CacheDoSearchFileFirst(const char *Name, UINT8 Attr, AMBA_CFS_DTA_s *DirEntry, AMBA_CFS_ACTIVE_DTA_s *ActiveDta)
{
    UINT32 Ret = AmbaCFS_SearchFileFirst(Name, Attr, &ActiveDta->FsSearch);
    if ((Ret == CFS_OK) || (Ret == CFS_ERR_OBJ_UNAVAILABLE)) {
        /* In the case of CFS_ERR_OBJ_UNAVAILABLE, the DTA is still kept to avoid the I/O of the next fsfirst() */
        (void)CFS_SET_RETURN(&Ret, AmbaCFS_SearchCopyDtaToSearch(DirEntry, &ActiveDta->FsSearch));
    }
    return Ret;
}

/**
 *  Search the next file in cache
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in,out] DirEntry        The DTA record
 *  @param [in,out] ActiveDta       The Active DTA to keep the FS search
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_CacheDoSearchFileNext(AMBA_CFS_DTA_s *DirEntry, AMBA_CFS_ACTIVE_DTA_s *ActiveDta)
{
    UINT32 Ret = AmbaCFS_SearchFileNext(&ActiveDta->FsSearch);
    if (Ret == CFS_OK) {
        Ret = AmbaCFS_SearchCopyDtaToSearch(DirEntry, &ActiveDta->FsSearch);
    }
    return Ret;
}

/**
 *  Finish the file search in cache
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in,out] ActiveDta       The Active DTA to keep the FS search
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_CacheDoSearchFileFinish(AMBA_CFS_ACTIVE_DTA_s *ActiveDta)
{
    UINT32 Ret = AmbaCFS_SearchFileFinish(&ActiveDta->FsSearch);
    return Ret;
}

/**
 *  Get first directory entry from cache.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] Root                The root node
 *  @param [in] Parent              The parent node
 *  @param [in] Path                The path array (only is used on Parent is NULL)
 *  @param [in] Depth               The path depth (only is used on Parent is NULL)
 *  @param [in] DirName             Directory and pattern string
 *  @param [in] RegExp              Regular expression
 *  @param [in] Attribute           Attribure
 *  @param [in,out] ActiveDta       The Active DTA. Get Attr and RegExp from ActiveDta.
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 FirstDirEnt_SearchAndCacheDta(AMBA_CFS_CACHE_TREE_NODE_s *Root, AMBA_CFS_CACHE_TREE_NODE_s *Parent, const char * const Path[AMBA_CFS_MAX_TREE_LEVEL], UINT32 Depth, const char *DirName, const char *RegExp, UINT8 Attribute, AMBA_CFS_ACTIVE_DTA_s *ActiveDta)
{
    UINT32 Ret;
    /* Get Search record */
    AMBA_CFS_CACHE_SEARCH_s *Search = NULL;
    Ret = GetFreeSearch(&Search);
    if (Ret == CFS_OK) {
        AMBA_CFS_DTA_s *DirEntry = ActiveDta->DirEntry;
        AMBA_CFS_CACHE_TREE_NODE_s *ParentNode = Parent;
        /*
         * Get First Directory Entry
         * Consider these cases:
         * 1. A file is found
         * ==> AmbaCFS_CacheDoSearchFileFirst returns CFS_OK
         * ==> Cache the result.
         * ==> Insert tree nodes if needed.
         * 2. File not found and the folder of DirName exists
         * ==> AmbaCFS_CacheDoSearchFileFirst returns CFS_ERR_OBJ_UNAVAILABLE
         * ==> Cache the result (to avoid I/O if user query the same DirName again).
         * ==> Insert tree nodes if needed.
         * 3. File not found and the folder of DirName not exists
         * ==> AmbaCFS_CacheDoSearchFileFirst returns CFS_ERR_FATAL
         * ==> Release search.
         */
        Ret = AmbaCFS_CacheDoSearchFileFirst(DirName, Attribute, DirEntry, ActiveDta);
        if (Ret == CFS_OK) {
            const char *FileName = DirEntry->FileName;
            AMBA_CFS_CACHE_TREE_NODE_s *Node = NULL;
            if (ParentNode == NULL) {
                Ret = InsertPathNodes(Root, Path, Depth, &ParentNode);
                if (Ret == CFS_OK) {
                    Ret = InsertChildNode(ParentNode, FileName, &Node);
                }
            } else {
                Ret = PrepareChildNode(ParentNode, FileName, &Node);
            }
            if (Ret == CFS_OK) {
                ParentNode->Search = Search;
                /* Set Search record */
                InitSearch(ParentNode, Node, RegExp, Attribute);
                /* Cache Dta Record */
                Ret = Cache_Dta(ActiveDta, Node, Search);
                if (Ret == CFS_OK) {
                    /* Move current search to head of the search used list */
                    Ret = TouchSearch(Search);
                }
            }
        } else if (Ret == CFS_ERR_OBJ_UNAVAILABLE) {
            if (ParentNode == NULL) {
                (void)CFS_SET_RETURN(&Ret, InsertPathNodes(Root, Path, Depth, &ParentNode));
            }
            if (Ret == CFS_ERR_OBJ_UNAVAILABLE) {
                ParentNode->Search = Search;
                InitSearch(ParentNode, NULL, RegExp, Attribute);
                Search->CacheStatus = AMBA_CFS_CACHE_SEARCH_STATUS_READY;
                (void)CFS_SET_RETURN(&Ret, TouchSearch(Search));
            }
        } else {
            if (ReleaseSearch(Search) != CFS_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] ReleaseSearch fail", __func__, NULL, NULL, NULL, NULL);
                Ret = CFS_ERR_API;
            }
        }
    }
    return Ret;
}

/**
 *  The implementation of getting first directory entry from cache.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in]     DirName         Directory and pattern string
 *  @param [in]     Attribute       Attribure
 *  @param [in,out] ActiveDta       The Active DTA
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_CacheFirstDirEntImpl(const char *DirName, UINT8 Attribute, AMBA_CFS_ACTIVE_DTA_s *ActiveDta)
{
    UINT32 Ret;
    char DriveName;
    /* Check length of filename, and pattern */
    Ret = AmbaCFS_UtilGetDrive(DirName, &DriveName);
    if (Ret == CFS_OK) {
        UINT32 DriveId;
        Ret = AmbaCFS_UtilGetDriveNum(DriveName, &DriveId);
        /* Check drive */
        if (Ret == CFS_OK) {
            char Buffer[AMBA_CFS_MAX_FILENAME_LENGTH];
            const char *Token[AMBA_CFS_MAX_TREE_LEVEL];
            UINT32 Depth;
            /* Get path name */
            AmbaCFS_UtilCopyFileName(Buffer, DirName);
            Ret = ParseFileName(Buffer, Token, &Depth);
            if (Ret == CFS_OK) {
                if (Depth > 1U) {
                    /* check if the parent directory is in the cache tree */
                    AMBA_CFS_CACHE_TREE_NODE_s * const Root = &g_AmbaCFSCache.Root[DriveId];
                    AMBA_CFS_CACHE_TREE_NODE_s *Parent = NULL;
                    Ret = SearchDescendant(Root, Token, Depth - 1U, &Parent);
                    if (Ret == CFS_OK) {
                        UINT8 CacheIsHit = 0U;
                        const char *RegExp = Token[Depth - 1U];
                        /* Keep RegExp and Attribute in ActiveDta */
                        AmbaCFS_ActiveDtaSetSearchPattern(ActiveDta, RegExp, Attribute);
                        if ((Parent != NULL) && (Parent->Search != NULL)) {
                            if (Parent->Search->CacheStatus == AMBA_CFS_CACHE_SEARCH_STATUS_READY) {
                                if (IncludeMatch(RegExp, Attribute, Parent->Search->RegExp, Parent->Search->Attr) != 0U) {
                                    CacheIsHit = 1U;
                                }
                            }
                            if (CacheIsHit == 0U) {
                                Ret = ReleaseSearch(Parent->Search);
                            }
                        }
                        if (Ret == CFS_OK) {
                            if (CacheIsHit != 0U) {
                                /* Cache Hit. Invoke FirstDirEnt_GetDataFromCache */
                                Ret = FirstDirEnt_GetDataFromCache(Parent, RegExp, Attribute, ActiveDta);
                            } else {
                                /* Cache Miss. Invoke FirstDirEnt_SearchAndCacheDta */
                                Ret = FirstDirEnt_SearchAndCacheDta(Root, Parent, Token, Depth - 1U, DirName, RegExp, Attribute, ActiveDta);
                            }
                        }
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Incorrect path name", __func__, NULL, NULL, NULL, NULL);
                    Ret = CFS_ERR_API;
                }
            }
        }
    }
    return Ret;
}

/**
 *  Get first directory entry from cache.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in]     DirName         Directory and pattern string
 *  @param [in]     Attribute       Attribure
 *  @param [in,out] ActiveDta       The Active DTA
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_CacheFirstDirEnt(const char *DirName, UINT8 Attribute, AMBA_CFS_ACTIVE_DTA_s *ActiveDta)
{
    UINT32 Ret;
    Ret = AmbaCFS_CacheLock();
    if (Ret == CFS_OK) {
        Ret = AmbaCFS_CacheFirstDirEntImpl(DirName, Attribute, ActiveDta);
        if (AmbaCFS_CacheUnlock() != CFS_OK) {
            Ret = CFS_ERR_FATAL;
        }
    }
    return Ret;
}

/**
 *  Check if the file status of a DTA is the same as that of the corresponding node
 *  @param [in] Node                The node
 *  @param [in] DirEntry            The DTA record
 *  @return 1 - TRUE, 0 - FALSE
 */
static inline UINT8 SameFileStatus(const AMBA_CFS_CACHE_TREE_NODE_s *Node, const AMBA_CFS_DTA_s *DirEntry)
{
    /*
     * Not comparing the DTA because the correctness is already confirmed by the checking the binding of DirEntry and ActiveDta.
     * For more safety, all the members of DirEntry should be checked.
     * And beware of the cost of less efficiency in each "Next" operation.
     */
#if 0
    UINT8 Ret = 0U;
    const AMBA_CFS_CACHE_DTA_s * const Dta = &Node->Cache.Dta;

    if (Dta->FileSize == DirEntry->FileSize) {
        if (Dta->Time == DirEntry->Time) {
            if (Dta->Date == DirEntry->Date) {
                if (Dta->Attribute == DirEntry->Attribute) {
                    if (SameFileName(Node->FileName, DirEntry->FileName) != 0U) {
                        Ret = 0U;
                    }
                }
            }
        }
    }
    return Ret;
#else
    AmbaMisra_TouchUnused(&Node);
    AmbaMisra_TouchUnused(&DirEntry);
    return 1U;
#endif
}

/**
 *  Check if the search record correspond to a DTA is cached
 *  @param [in] Search              The search
 *  @param [in] ActiveDta           The Active DTA
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static inline UINT32 VerifySearch(const AMBA_CFS_CACHE_SEARCH_s *Search, const AMBA_CFS_ACTIVE_DTA_s *ActiveDta)
{
    UINT32 Ret = CFS_ERR_API;
    ULONG SearchUL;
    ULONG SearchBaseUL;
    ULONG Diff;
    AmbaMisra_TypeCast(&SearchUL, &Search);
    AmbaMisra_TypeCast(&SearchBaseUL, &g_AmbaCFSCache.SearchCache);
    Diff = SearchUL - SearchBaseUL;
    if ((Diff % sizeof(AMBA_CFS_CACHE_SEARCH_s)) == 0U) {
        if ((intptr_t)Diff >= 0) {
            if ((Diff / sizeof(AMBA_CFS_CACHE_SEARCH_s)) < g_AmbaCFSCache.CacheMaxDirNum) {
                /* check search & dta binding */
                /* In case that the cache is removed (by ReleaseSearch) or even reused during search. */
                if (Search->Host == NULL) {
                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Invalid search. Search->Host is NULL. Need to search again.", __func__, NULL, NULL, NULL, NULL);
                    Ret = CFS_ERR_INVALID_SEARCH;
                } else if (Search->Host->Search != Search) {
                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Invalid search. Search->Host->Search != Search. Need to search again.", __func__, NULL, NULL, NULL, NULL);
                    Ret = CFS_ERR_INVALID_SEARCH;
                } else if (Search->ActiveDta != ActiveDta) {
                    /*
                     * If cache is removed during search (due to new file created, ...), the Search->ActiveDta will be NULL in ReleaseSearch.
                     * When AmbaCFS_CacheNextDirEnt is called again, there are two possibilities for Search->ActiveDta
                     * 1. Search->ActiveDta == NULL
                     * 2. Search is used by another search so Search->ActiveDta links to another ActiveDta
                     * In either cases VerifySearch will return CFS_ERR_INVALID_SEARCH.
                     * And then user should call Finish and then search again to get the correct result.
                     */
                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Invalid search. Search->ActiveDta != ActiveDta. Need to search again.", __func__, NULL, NULL, NULL, NULL);
                    Ret = CFS_ERR_INVALID_SEARCH;
                } else {
                    /* check if the search is still in used list */
                    const AMBA_CFS_CACHE_SEARCH_s *SearchList = g_AmbaCFSCache.SearchUsedList;
                    while (SearchList != NULL) {
                        /* in general, the loop would be done immediately because the list usually is at head */
                        if (Search == SearchList) {
                            Ret = CFS_OK;
                            break;
                        }
                        SearchList = SearchList->ListNext;
                    }
                    if (Ret != CFS_OK) {
                        AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "VerifySearch: [ERROR] SearchList not found. Search %p", (UINT32)SearchUL, 0, 0, 0, 0);
                    }
                }
            } else {
                AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "VerifySearch(%p): [ERROR] Out of range. SearchBase %p Diff %u Size %u Num %u", (UINT32)SearchUL, (UINT32)SearchBaseUL, (UINT32)Diff, (UINT32)sizeof(AMBA_CFS_CACHE_SEARCH_s), g_AmbaCFSCache.CacheMaxDirNum);
            }
        } else {
            AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "VerifySearch(%p): [ERROR] Out of range. SearchBase %p Diff %u Size %u Num %u", (UINT32)SearchUL, (UINT32)SearchBaseUL, (UINT32)Diff, (UINT32)sizeof(AMBA_CFS_CACHE_SEARCH_s), g_AmbaCFSCache.CacheMaxDirNum);
        }
    } else {
        AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "VerifySearch(%p): [ERROR] Not aligned. SearchBase %p Diff %u Size %u", (UINT32)SearchUL, (UINT32)SearchBaseUL, (UINT32)Diff, (UINT32)sizeof(AMBA_CFS_CACHE_SEARCH_s), 0U);
    }
    return Ret;
}

/**
 *  Check if the node correspond to a DTA is cached
 *  @param [in] Node                The node
 *  @param [in] ActiveDta           The Active DTA
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static inline UINT32 VerifyNode(const AMBA_CFS_CACHE_TREE_NODE_s *Node, const AMBA_CFS_ACTIVE_DTA_s *ActiveDta)
{
    UINT32 Ret = CFS_ERR_API;
    ULONG NodeUL;
    ULONG NodeBaseUL;
    ULONG Diff;
    AmbaMisra_TypeCast(&NodeUL, &Node);
    AmbaMisra_TypeCast(&NodeBaseUL, &g_AmbaCFSCache.NodeCache);
    Diff = NodeUL - NodeBaseUL;
    if ((Diff % sizeof(AMBA_CFS_CACHE_TREE_NODE_s)) == 0U) {
        if ((intptr_t)Diff >= 0) {
            if ((Diff / sizeof(AMBA_CFS_CACHE_TREE_NODE_s)) < g_AmbaCFSCache.CacheMaxNodeNum) {
                /* check search & node binding */
                const AMBA_CFS_CACHE_TREE_NODE_s * const Parent = Node->Parent;
                const AMBA_CFS_CACHE_SEARCH_s * Search = AmbaCFS_CacheGetSearch(ActiveDta);
                if (Parent == NULL) {
                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Parent is NULL", __func__, NULL, NULL, NULL, NULL);
                } else if (Parent->Search != Search) {
                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Parent->Search != Search", __func__, NULL, NULL, NULL, NULL);
                } else if (Search == NULL) {
                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Search is NULL", __func__, NULL, NULL, NULL, NULL);
                } else if (Search->Host != Parent) {
                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Search->Host != Parent", __func__, NULL, NULL, NULL, NULL);
                } else {
                    /* check if it is parent's child */
                    /* search has been verified, so if parent is correct, then just need to check parent's children, no need to scan used list */
                    const AMBA_CFS_CACHE_TREE_NODE_s *TempNode = Parent->Child;
                    while (TempNode != NULL) {
                        if (TempNode == Node) {
                            Ret = CFS_OK;
                            break;
                        }
                        TempNode = TempNode->Sibling;
                    }
                    if (Ret != CFS_OK) {
                        AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "VerifyNode: [ERROR] Node not found. Node %p", (UINT32)NodeUL, 0, 0, 0, 0);
                    }
                }
            } else {
                AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "VerifyNode(%p): [ERROR] Out of range. NodeBase %p Diff %u Size %u Num %u", (UINT32)NodeUL, (UINT32)NodeBaseUL, (UINT32)Diff, (UINT32)sizeof(AMBA_CFS_CACHE_TREE_NODE_s), g_AmbaCFSCache.CacheMaxNodeNum);
            }
        } else {
            AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "VerifyNode(%p): [ERROR] Out of range. NodeBase %p Diff %u Size %u Num %u", (UINT32)NodeUL, (UINT32)NodeBaseUL, (UINT32)Diff, (UINT32)sizeof(AMBA_CFS_CACHE_TREE_NODE_s), g_AmbaCFSCache.CacheMaxNodeNum);
        }
    } else {
        AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "VerifyNode(%p): [ERROR] Not aligned. NodeBase %p Diff %u Size %u", (UINT32)NodeUL, (UINT32)NodeBaseUL, (UINT32)Diff, (UINT32)sizeof(AMBA_CFS_CACHE_TREE_NODE_s), 0U);
    }
    return Ret;
}

/**
 *  Get node from DTA
 *  @param [in] ActiveDta           The Active DTA
 *  @param [out] SearchNode         The returned node
 *  @return 0 - OK, others - Error
 */
static UINT32 GetNodeFromDta(const AMBA_CFS_ACTIVE_DTA_s *ActiveDta, AMBA_CFS_CACHE_TREE_NODE_s **SearchNode)
{
    UINT32 Ret;
    AMBA_CFS_CACHE_TREE_NODE_s *Node = AmbaCFS_CacheGetNode(ActiveDta);
    /* can only be called after search is verified */
    Ret = VerifyNode(Node, ActiveDta);
    if (Ret == CFS_OK) {
        /* compare with search */
        const AMBA_CFS_CACHE_SEARCH_s *Search = AmbaCFS_CacheGetSearch(ActiveDta);
        if (Node->Parent == Search->Host) {
            if (SameFileStatus(Node, ActiveDta->DirEntry) != 0U) {
                /* Set return value */
                *SearchNode = Node;
            } else {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] SameFileStatus fail", __func__, NULL, NULL, NULL, NULL);
                Ret = CFS_ERR_API;
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Incorrect search", __func__, NULL, NULL, NULL, NULL);
            Ret = CFS_ERR_API;
        }
    }
    return Ret;
}

/**
 *  Find a matched search record.
 *  @param [in] ActiveDta           The Active DTA
 *  @param [out] MatchedSearch      The returned matched search record
 *  @return 0 - OK, others - Error
 */
static UINT32 GetSearchFromDta(const AMBA_CFS_ACTIVE_DTA_s *ActiveDta, AMBA_CFS_CACHE_SEARCH_s **MatchedSearch)
{
    UINT32 Ret;
    AMBA_CFS_CACHE_SEARCH_s *Search = AmbaCFS_CacheGetSearch(ActiveDta);
    Ret = VerifySearch(Search, ActiveDta);
    if (Ret == CFS_OK) {
        /* Set return value */
        *MatchedSearch = Search;
    }
    return Ret;
}

/**
 *  Get the next DTA from cache
 *  @param [in] Node                The node
 *  @param [in] Search              The search
 *  @param [in,out] ActiveDta       The Active DTA
 *  @return 0 - OK, others - Error
 */
static UINT32 NextDirEnt_GetDataFromCache(AMBA_CFS_CACHE_TREE_NODE_s *Node, AMBA_CFS_CACHE_SEARCH_s *Search, AMBA_CFS_ACTIVE_DTA_s *ActiveDta)
{
    UINT32 Ret = CFS_OK;
    AMBA_CFS_CACHE_TREE_NODE_s *TempNode = Node;
    /* Node is the previous search result (in dta), so start from Node's sibling */
    if (TempNode->Sibling != NULL) {
        Ret = GetMatchedNode(TempNode->Sibling, ActiveDta->RegExp, ActiveDta->Attr, &TempNode);
        if (Ret == CFS_OK) {
            if (TempNode->Parent != NULL) {
                // AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: Get DTA from cache! Path:%s", __func__, TempNode->FileName, NULL, NULL, NULL);
                /* Get data from cache */
                Ret = Retrieve_Cached_Dta(ActiveDta, TempNode, Search);
                if (Ret == CFS_OK) {
                    /* Move current search to head of the search used list */
                    Ret = TouchSearch(Search);
                }
            } else {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Parent is NULL", __func__, NULL, NULL, NULL, NULL);
                Ret = CFS_ERR_API;
            }
        } else {
            // AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: No more data in the cache!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        // AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: No more data in the cache!", __func__, NULL, NULL, NULL, NULL);
        Ret = CFS_ERR_OBJ_UNAVAILABLE;
    }
    return Ret;
}

/**
 *  Crate the next DTA and cache it
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] Node                The node
 *  @param [in] Search              The search
 *  @param [in,out] ActiveDta       The Active DTA
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 NextDirEnt_SearchAndCacheDta(const AMBA_CFS_CACHE_TREE_NODE_s *Node, AMBA_CFS_CACHE_SEARCH_s *Search, AMBA_CFS_ACTIVE_DTA_s *ActiveDta)
{
    UINT32 Ret = CFS_ERR_API;
    if (Search->EndPos == Node) {
        AMBA_CFS_CACHE_TREE_NODE_s * const Parent = Node->Parent;
        if (Parent != NULL) {
            if (Parent->Search == Search) {
                AMBA_CFS_DTA_s *DirEntry = ActiveDta->DirEntry;
                /* Get next Directory Entry */
                Ret = AmbaCFS_CacheDoSearchFileNext(DirEntry, ActiveDta);
                if (Ret == CFS_OK) {
                    AMBA_CFS_CACHE_TREE_NODE_s *TempNode = Node->Sibling;
                    AMBA_CFS_CACHE_TREE_NODE_s *Prev = NULL;
                    const char *FileName = DirEntry->FileName;
                    /* Check if the node of the filename is presented (due to fstat) */
                    while (TempNode != NULL) {
                        if (SameFileName(TempNode->FileName, FileName) != 0U) {
                            break;
                        }
                        Prev = TempNode;
                        TempNode = TempNode->Sibling;
                    }
                    if (TempNode == NULL) {
                        /* Insert a new node */
                        // AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: Insert a new node!", __func__, NULL, NULL, NULL, NULL);
                        Ret = InsertChildNode(Parent, FileName, &TempNode);
                    } else {
                        /* Reuse the child node */
                        // AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: Reuse the child node!", __func__, NULL, NULL, NULL, NULL);
                        if (Prev != NULL) {
                            /* remove node from child list */
                            Prev->Sibling = TempNode->Sibling;
                            /* Insert to the position right after the Search->EndPos */
                            TempNode->Sibling = Search->EndPos->Sibling;
                            Search->EndPos->Sibling = TempNode;
                        }
                        /* no prev means node is right after EndPos, nothing to do */
                    }
                    if (Ret == CFS_OK) {
                        // AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: Cache Dta data!", __func__, NULL, NULL, NULL, NULL);
                        /* Cache Dta Record */
                        Ret = Cache_Dta(ActiveDta, TempNode, Search);
                        if (Ret == CFS_OK) {
                            /* Update search record */
                            Search->EndPos = TempNode;
                            /* Move current search to head of the search used list */
                            Ret = TouchSearch(Search);
                        }
                    }
                } else if (Ret == CFS_ERR_OBJ_UNAVAILABLE) {
                    /* no more entry, end */
                    // AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: Reach to the last directory entry!", __func__, NULL, NULL, NULL, NULL);
                    Search->CacheStatus = AMBA_CFS_CACHE_SEARCH_STATUS_READY;
                } else {
                    /* Do nothing */
                }
            } else {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Parent->Search != Search", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Parent is NULL", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Search->EndPos != Node", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  The implementation of getting next directory entry from cache.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in,out] ActiveDta       The Active DTA
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_CacheNextDirEntImpl(AMBA_CFS_ACTIVE_DTA_s *ActiveDta)
{
    UINT32 Ret;
    AMBA_CFS_CACHE_SEARCH_s *Search = NULL;
    Ret = GetSearchFromDta(ActiveDta, &Search);
    if (Ret == CFS_OK) {
        AMBA_CFS_CACHE_TREE_NODE_s *Node = NULL;
        Ret = GetNodeFromDta(ActiveDta, &Node);
        if (Ret == CFS_OK) {
            /* the search and the previous node are still in cache */
            if (Search->CacheStatus == AMBA_CFS_CACHE_SEARCH_STATUS_READY) {
                /* Cache Hit. Invoke NextDirEnt_GetDataFromCache */
                Ret = NextDirEnt_GetDataFromCache(Node, Search, ActiveDta);
            } else {
                if (Search->CacheStatus == AMBA_CFS_CACHE_SEARCH_STATUS_CREATING) {
                    /* Cache Miss. Invoke NextDirEnt_SearchAndCacheDta */
                    Ret = NextDirEnt_SearchAndCacheDta(Node, Search, ActiveDta);
                } else {
                    AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_CacheNextDirEntImpl: [ERROR] CacheStatus %u", Search->CacheStatus, 0, 0, 0, 0);
                    Ret = CFS_ERR_API;
                }
            }
        }
    }
    return Ret;
}

/**
 *  Get next directory entry from cache.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in,out] ActiveDta       The Active DTA
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_CacheNextDirEnt(AMBA_CFS_ACTIVE_DTA_s *ActiveDta)
{
    UINT32 Ret;
    Ret = AmbaCFS_CacheLock();
    if (Ret == CFS_OK) {
        Ret = AmbaCFS_CacheNextDirEntImpl(ActiveDta);
        if (AmbaCFS_CacheUnlock() != CFS_OK) {
            Ret = CFS_ERR_FATAL;
        }
    }
    return Ret;
}

/**
 *  The implemenation of finishing the file search.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] ActiveDta           The Active DTA
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_CacheFinishDirEntImpl(AMBA_CFS_ACTIVE_DTA_s *ActiveDta)
{
    UINT32 Ret;
    /*
     * No need to "VerifySearch".
     * Even if the search is invalid (maybe due to cache remove), still need to release resources.
     * To support this code flow:
     *     First(OK) -> Next(NG due to cache remove) -> Finish(OK)
     */
    Ret = AmbaCFS_CacheDoSearchFileFinish(ActiveDta);
    return Ret;
}

/**
 *  Finish the file search
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] ActiveDta           The Active DTA
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_CacheFinishDirEnt(AMBA_CFS_ACTIVE_DTA_s *ActiveDta)
{
    UINT32 Ret;
    Ret = AmbaCFS_CacheLock();
    if (Ret == CFS_OK) {
        Ret = AmbaCFS_CacheFinishDirEntImpl(ActiveDta);
        if (AmbaCFS_CacheUnlock() != CFS_OK) {
            Ret = CFS_ERR_FATAL;
        }
    }
    return Ret;
}

/**
 *  Cache device info
 *  @param [in] DevInfo             The device info to be cached
 *  @param [in] DriveId             The drive ID
 */
static inline void Cache_DevInf(const AMBA_CFS_DRIVE_INFO_s *DevInfo, UINT32 DriveId)
{
#if AMBA_CFS_CACHE_DEBUG
    AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "Cache_DevInf(%u) : T=%u E=%u F=%u A=0x%X", DriveId, DevInfo->TotalClusters, DevInfo->EmptyClusters, DevInfo->FormatType, DevInfo->Attr);
#endif
    g_AmbaCFSCache.DevInf[DriveId] = *DevInfo;
    g_AmbaCFSCache.DevInfCached[DriveId] = 1U;
}

/**
 *  Retrieve cached device info
 *  @param [in] DevInfo             The address to put device info
 *  @param [in] DriveId             The drive ID
 */
static inline void Retrieve_Cached_DevInf(AMBA_CFS_DRIVE_INFO_s *DevInfo, UINT32 DriveId)
{
    *DevInfo = g_AmbaCFSCache.DevInf[DriveId];
#if AMBA_CFS_CACHE_DEBUG
    AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "Retrieve_Cached_DevInf(%u) : T=%u E=%u F=%u A=0x%X", DriveId, DevInfo->TotalClusters, DevInfo->EmptyClusters, DevInfo->FormatType, DevInfo->Attr);
#endif
}

/**
 *  The implementation of getting drive information from cache.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in]  Drive              Drive name (from A to Z)
 *  @param [out] DriveInfo          The returned drive info
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_CacheGetDriveInfoImpl(char Drive, AMBA_CFS_DRIVE_INFO_s *DriveInfo)
{
    UINT32 Ret;
    UINT32 DriveId;
    Ret = AmbaCFS_UtilGetDriveNum(Drive, &DriveId);
    if (Ret == CFS_OK) {
        /* Get data from cache */
        if (g_AmbaCFSCache.DevInfCached[DriveId] != 0U) {
            // AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: Data is from cache!", __func__, NULL, NULL, NULL, NULL);
            Retrieve_Cached_DevInf(DriveInfo, DriveId);
        } else {
            // AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: Data is not from cache!", __func__, NULL, NULL, NULL, NULL);
            Ret = CFS_F2C(AmbaFS_GetDriveInfo(Drive, DriveInfo));
            if (Ret == CFS_OK) {
                Cache_DevInf(DriveInfo, DriveId);
            } else {
                AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_CacheGetDriveInfoImpl: [ERROR] AmbaFS_GetDriveInfo fail. ErrNum: %p", AmbaCFS_UtilGetAmbaFSError(), 0, 0, 0, 0);
            }
        }
    }
    return Ret;
}

/**
 *  Get drive information from cache.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in]  Drive              Drive name (from A to Z)
 *  @param [out] DriveInfo          The returned drive info
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_CacheGetDriveInfo(char Drive, AMBA_CFS_DRIVE_INFO_s *DriveInfo)
{
    UINT32 Ret;
    Ret = AmbaCFS_CacheLock();
    if (Ret == CFS_OK) {
        Ret = AmbaCFS_CacheGetDriveInfoImpl(Drive, DriveInfo);
        if (AmbaCFS_CacheUnlock() != CFS_OK) {
            Ret = CFS_ERR_FATAL;
        }
    }
    return Ret;
}

/**
 *  The implementation of clearing cache of a drive.
 *  @param [in]  Drive              Drive name (from A to Z)
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaCFS_CacheClearDriveImpl(char Drive)
{
    UINT32 Ret;
    UINT32 DriveId;
    Ret = AmbaCFS_UtilGetDriveNum(Drive, &DriveId);
    if (Ret == CFS_OK) {
        const AMBA_CFS_CACHE_TREE_NODE_s * const Root = &g_AmbaCFSCache.Root[DriveId];
        Ret = DelDescendant(Root);  /* just delete sub-tree, keep the root node */
        if (Ret == CFS_OK) {
            if (Root->Search != NULL) {
                Ret = ReleaseSearch(Root->Search);
            }
            if (Ret == CFS_OK) {
                g_AmbaCFSCache.DevInfCached[DriveId] = 0U;
            }
        }
    }
    return Ret;
}

/**
 *  Clear cache of a drive.
 *  @param [in]  Drive              Drive name (from A to Z)
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_CacheClearDrive(char Drive)
{
    UINT32 Ret;
    Ret = AmbaCFS_CacheLock();
    if (Ret == CFS_OK) {
        Ret = AmbaCFS_CacheClearDriveImpl(Drive);
        if (AmbaCFS_CacheUnlock() != CFS_OK) {
            Ret = CFS_ERR_FATAL;
        }
    }
    return Ret;
}

#if AMBA_CFS_CACHE_DEBUG
/*
 * For debuging
 */
static void GetNodeStr(const AMBA_CFS_CACHE_TREE_NODE_s *Node, char *Buffer, UINT32 Size)
{
    const void *Temp = Node;
    ULONG TempUL;
    UINT32 ArgU32[3];
    AmbaMisra_TypeCast(&TempUL, &Temp);
    ArgU32[0] = (UINT32)TempUL;
    Temp = Node->Parent;
    AmbaMisra_TypeCast(&TempUL, &Temp);
    ArgU32[1] = (UINT32)TempUL;
    ArgU32[2] = Node->Cache.ValidBits;
    (void)AmbaUtility_StringPrintUInt32(Buffer, Size, "{%p P:%p V:0x%X}", 3, ArgU32);
}

static void GetSearchStr(const AMBA_CFS_CACHE_SEARCH_s *Search, char *Buffer, UINT32 Size)
{
    const void *Temp = Search;
    ULONG TempUL;
    UINT32 ArgU32[5];
    AmbaMisra_TypeCast(&TempUL, &Temp);
    ArgU32[0] = (UINT32)TempUL;
    Temp = Search->Host;
    AmbaMisra_TypeCast(&TempUL, &Temp);
    ArgU32[1] = (UINT32)TempUL;
    ArgU32[2] = Search->Attr;
    ArgU32[3] = Search->CacheStatus;
    Temp = Search->EndPos;
    AmbaMisra_TypeCast(&TempUL, &Temp);
    ArgU32[4] = (UINT32)TempUL;
    (void)AmbaUtility_StringPrintUInt32(Buffer, Size, "{%p H:%p A:0x%X S:0x%X E:%p}", 5, ArgU32);
}

/**
 *  Print all nodes in a tree.
 *  @param [in] Root                The root node of the tree
 *  @param [in] Level               The level of Node
 */
static void PrintTree(const AMBA_CFS_CACHE_TREE_NODE_s *Root, UINT32 Level)
{
    char Str[AMBA_CFS_MAX_FILENAME_LENGTH];
    const AMBA_CFS_CACHE_TREE_NODE_s *Node = Root;
    UINT32 NodeLevel = Level;
    char NodeStr[32];
    char SearchStr[64];
    /* Depth first search */
    for ( ; ; ) {
        /* Print filename */
        for (UINT32 i = 0; i < NodeLevel; ++i) {
            Str[i] = '-';
        }
        Str[NodeLevel] = '\0';
        GetNodeStr(Node, NodeStr, sizeof(NodeStr));
        if (Node->Search != NULL) {
            GetSearchStr(Node->Search, SearchStr, sizeof(SearchStr));
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s> %s FN=%s S=%s", Str, NodeStr, Node->FileName, SearchStr, NULL);
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s> %s FN=%s", Str, NodeStr, Node->FileName, NULL, NULL);
        }
        if (Node->Child != NULL) {
            Node = Node->Child; /* Move to Child */
            NodeLevel++;
        } else {
            while (Node->Sibling == NULL) {
                if (Node == Root) {
                    /* Return */
                    break;
                }
                Node = Node->Parent; /* Move to Parent */
                NodeLevel--;
            }

            if (Node == Root) {
                /* Return */
                break;
            }
            Node = Node->Sibling; /* Move to Sibling */
            /* Level is the same */
        }
    }
}

/**
 *  Print the nodes
 */
static void PrintNodeList(void)
{
    const AMBA_CFS_CACHE_TREE_NODE_s *List = g_AmbaCFSCache.NodeAvailList;
    UINT8 Count = 0U;
    while (List != NULL) {
        List = List->ListNext;
        Count++;
    }
    AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "==== Node Avail=%u Total=%u ====", Count, g_AmbaCFSCache.CacheMaxNodeNum, 0, 0, 0);
}

/**
 *  Print search records.
 *  @param [in] Option          The print option
 */
static void PrintSearchList(UINT8 Option)
{
    const AMBA_CFS_CACHE_SEARCH_s *List = g_AmbaCFSCache.SearchAvailList;
    UINT8 Count = 0U;
    while (List != NULL) {
        List = List->ListNext;
        Count++;
    }
    AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "==== Search Use List (Avail=%u)  ====", Count, 0, 0, 0, 0);
    List = g_AmbaCFSCache.SearchUsedList;
    Count = 0U;
    while (List != NULL) {
        const AMBA_CFS_CACHE_SEARCH_s *Search;
        char CountStr[16];
        char SearchStr[64];
        UINT32 ArgU32[1];
        ArgU32[0] = Count;
        (void)AmbaUtility_StringPrintUInt32(CountStr, sizeof(CountStr), "%u", 1, ArgU32);
        Search = &List->Search;
        GetSearchStr(Search, SearchStr, sizeof(SearchStr));
        if (Option != 0U) {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "Search[%s]:%s FN=%s RegExp=%s", CountStr, SearchStr, Search->Host->FileName, Search->RegExp, NULL);
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "Search[%s]:%s", CountStr, SearchStr, NULL, NULL, NULL);
        }
        List = List->ListNext;
        Count++;
    }
}

/**
 *  The implementation of printing cached tree structure.
 *  @param [in]  Drive              Drive name (from A to Z)
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaCFS_CachePrintTreeImpl(char Drive)
{
    UINT32 Ret;
    /* Check drive */
    UINT32 DriveId;
    Ret = AmbaCFS_UtilGetDriveNum(Drive, &DriveId);
    if (Ret == CFS_OK) {
        AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "==== Tree Structure ====", 0, 0, 0, 0, 0);
        PrintTree(&g_AmbaCFSCache.Root[DriveId], 0);
    }
    PrintNodeList();
    PrintSearchList(1);
    return Ret;
}

/**
 *  Print cache tree.
 *  @param [in]  Drive              Drive name (from A to Z)
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_CachePrintTree(char Drive)
{
    UINT32 Ret;
    Ret = AmbaCFS_CacheLock();
    if (Ret == CFS_OK) {
        Ret = AmbaCFS_CachePrintTreeImpl(Drive);
        if (AmbaCFS_CacheUnlock() != CFS_OK) {
            Ret = CFS_ERR_FATAL;
        }
    }
    return Ret;
}
#endif

