/**
 * @file AmbaCFS_Cache.h
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
#ifndef AMBACFS_CACHE_H
#define AMBACFS_CACHE_H

#include "AmbaCFS_ActiveDta.h"
#include <AmbaCFS.h>

#define AMBA_CFS_CACHE_DELIMETER         '\\'   /**< Path delimeter */
#define AMBA_CFS_MAX_TREE_LEVEL          (5U)   /**< At most 5 level (C:/DCIM/100MEDIA/VIDEO/AMBA0001.MP4) */

/**
 * Valid Bits defination, for Tree nodes
 */
#define AMBA_CFS_CACHE_VALIDBIT_DTA         (0x00000001U)   /**< Valid status of AMBA_CFS_DTA_s */
#define AMBA_CFS_CACHE_VALIDBIT_FILE_INFO   (0x00000002U)   /**< Valid status of AMBA_CFS_FILE_INFO_s */

/**
 *  Status of a SEARCH, for FirstDirEnt and NextDirEnt functions
 */
#define AMBA_CFS_CACHE_SEARCH_STATUS_CREATING   (1U)    /**< Creating cache     */
#define AMBA_CFS_CACHE_SEARCH_STATUS_READY      (2U)    /**< Cache is ready     */

/**
 *  Data Structure of a SEARCH
 */
typedef struct AMBA_CFS_CACHE_SEARCH_s_ {
    char RegExp[AMBA_CFS_MAX_FILENAME_LENGTH];          /**< Pattern for the search */
    struct AMBA_CFS_CACHE_TREE_NODE_s_ *EndPos;         /**< Point to the last directory entry of the search (The last node returned by search first/next) */
    struct AMBA_CFS_CACHE_TREE_NODE_s_ *Host;           /**< Pointing to the tree node contains this search node */
    UINT8 CacheStatus;                                  /**< Status of the search */
    UINT8 Attr;                                         /**< Attr of the search */
    AMBA_CFS_ACTIVE_DTA_s *ActiveDta;                   /**< The ActiveDta that the search links to (bind search with ActiveDta, to simplify code, to ensure one folder can have only one ActiveDta) */
    /* Member for search available list and search used list */
    struct AMBA_CFS_CACHE_SEARCH_s_ *ListPrev;          /**< Pointing to previous list item */
    struct AMBA_CFS_CACHE_SEARCH_s_ *ListNext;          /**< Pointing to next list item */
} AMBA_CFS_CACHE_SEARCH_s;

/**
 *  Data Structure of a DTA
 */
typedef struct {
    UINT64 FileSize;                        /**< File size in bytes */
    UINT16 Date;                            /**< Date */
    UINT16 Time;                            /**< Time */
    UINT32 Attribute;                       /**< File attributes */
} AMBA_CFS_CACHE_DTA_s;

/**
 *  Node structure of the cache tree
 */
typedef struct {
    UINT32 ValidBits;               /**< Valid bits - bit0: DTA (no use, just for log), bit1: FILE_INFO */
    AMBA_CFS_CACHE_DTA_s Dta;       /**< Cached DTA data */
    AMBA_CFS_FILE_INFO_s FileInfo;  /**< For storing FileInfo data */
} AMBA_CFS_NODE_CACHE_s;

/**
 *  Tree node structure of the cache tree
 */
typedef struct AMBA_CFS_CACHE_TREE_NODE_s_ {
    char FileName[AMBA_CFS_MAX_FILENAME_LENGTH];        /**< Filename or directory name */
    AMBA_CFS_NODE_CACHE_s Cache;                        /**< For storing DTA data and Info data */
    struct AMBA_CFS_CACHE_SEARCH_s_    *Search;         /**< Pointing to a SEARCH record */
    struct AMBA_CFS_CACHE_TREE_NODE_s_ *Parent;         /**< Pointing to the parent node */
    struct AMBA_CFS_CACHE_TREE_NODE_s_ *Sibling;        /**< Pointing to the next node of the same parent */
    struct AMBA_CFS_CACHE_TREE_NODE_s_ *Child;          /**< Pointing to the first child node */
    /* Member for node available list */
    struct AMBA_CFS_CACHE_TREE_NODE_s_ *ListNext;       /**< Pointing to next list item */
} AMBA_CFS_CACHE_TREE_NODE_s;

/*
 *  Cache APIs
 */

/**
 *  Get required buffer size for cache.
 *  @param [in] CacheMaxDirNum      The maximum number of cache directories
 *  @param [in] CacheMaxFileNum     Maximum available number of cached file
 *  @return The required buffer size
 */
UINT32 AmbaCFS_CacheGetInitBufferSize(UINT32 CacheMaxDirNum, UINT32 CacheMaxFileNum);

/**
 *  Get default config for cache module.
 *  @param [out] Config             Address of a config
 */
void AmbaCFS_CacheGetInitDefaultCfg(AMBA_CFS_INIT_CFG_s *Config);

/**
 *  Initialize the Cache module.
 *  @param [in]  Buffer             The address of the work buffer
 *  @param [in]  BufferSize         The size of the work buffer
 *  @param [in]  CacheMaxDirNum     The maximum number of cache directories
 *  @param [in]  CacheMaxFileNum    The maximum number of cache files
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_CacheInit(const UINT8 *Buffer, UINT32 BufferSize, UINT32 CacheMaxDirNum, UINT32 CacheMaxFileNum);

/**
 *  Update cache tree for the remove command.
 *  @param [in]  FileName           Filename
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_CacheRemove(const char *FileName);

/**
 *  Get drive information from cache.
 *  @param [in]  Drive              Drive name (from A to Z)
 *  @param [out] DriveInfo          The returned drive info
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_CacheGetDriveInfo(char Drive, AMBA_CFS_DRIVE_INFO_s *DriveInfo);

/**
 *  Get file stat information from cache.
 *  @param [in]  FileName           Filename
 *  @param [in]  Info               Pointer of a info record
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_CacheGetFileInfo(const char *FileName, AMBA_CFS_FILE_INFO_s *Info);

/**
 *  Get first directory entry from cache.
 *  @param [in]     DirName         Directory and pattern string
 *  @param [in]     Attribute       Attribure
 *  @param [in,out] ActiveDta       The Active DTA
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_CacheFirstDirEnt(const char *DirName, UINT8 Attribute, AMBA_CFS_ACTIVE_DTA_s *ActiveDta);

/**
 *  Get next directory entry from cache.
 *  @param [in,out] ActiveDta       The Active DTA
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_CacheNextDirEnt(AMBA_CFS_ACTIVE_DTA_s *ActiveDta);

/**
 *  Finish the file search
 *  @param [in] ActiveDta           The Active DTA
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_CacheFinishDirEnt(AMBA_CFS_ACTIVE_DTA_s *ActiveDta);

/**
 *  Clear cache of a drive.
 *  @param [in]  Drive              Drive name (from A to Z)
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_CacheClearDrive(char Drive);

#endif

