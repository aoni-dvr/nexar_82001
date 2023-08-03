/**
 *  @file RefDcf.h
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
 *  @details Header File of DCF Main Module
 *
 */
#ifndef REF_DCF_H
#define REF_DCF_H

/**
 * @defgroup DCF
 * @brief DCF (Design rule for Camera File system) implementation
 *
 * The implementation of the DCF module
 * DCF module provides the API and interfaces to browse/update the mapping of media objects to the files in storage.
 *
 */

/**
 * @addtogroup DCF
 * @{
 */

#include <AmbaKAL.h>
#include <AmbaUtility.h>

#define AMBA_DCF_MAX_FILENAME_LENGTH    (64U)   /**< The maximum length of file names */

/**
 * Error code
 */
#define DCF_ERR_BASE    (0x02000000U)           /**< same as COMSVC_ERR_BASE */
#define DCF_ERR_0000    (DCF_ERR_BASE)          /**< Invalid argument */
#define DCF_ERR_0001    (DCF_ERR_BASE | 0x1U)   /**< Fatal error */
#define DCF_ERR_0002    (DCF_ERR_BASE | 0x2U)   /**< IO error */
#define DCF_ERR_0003    (DCF_ERR_BASE | 0x3U)   /**< Object/file not existed */
#define DCF_ERR_0004    (DCF_ERR_BASE | 0x4U)   /**< Object/file already existed */
#define DCF_ERR_0005    (DCF_ERR_BASE | 0x5U)   /**< Not ready */

/**
 * The default configuration for initialize the DCF module
 */
typedef struct {
    UINT8 *Buffer;                      /**< The work buffer of the DCF module */
    UINT32 BufferSize;                  /**< The work buffer size of DCF module */
    UINT32 MaxDnum;                     /**< The maximum Dnum among all handlers (ex: for Dnum = 1~999 => 999) */
    UINT8 MaxDirPerDnum;                /**< The maximum number of directories per directory number */
    UINT8 MaxHdlr;                      /**< The maximum number of DCF handlers in the DCF module */
    UINT8 MaxRootPerHdlr;               /**< The maximum number of roots for each DCF handler */
} REF_DCF_INIT_CFG_s;

/**
 * DCF file entry in a file list
 */
typedef struct {
    char Name[AMBA_DCF_MAX_FILENAME_LENGTH];    /**< File name */
} REF_DCF_FILE_s;


/**
 * DCF directory entry in a directory list
 */
typedef struct {
    char Name[AMBA_DCF_MAX_FILENAME_LENGTH];    /**< Directory name */
} REF_DCF_DIR_s;


struct REF_DCF_TABLE_s;

/**
 * DCF table handler
 */
typedef struct {
    struct REF_DCF_TABLE_s *Func; /**< Functions of a table handler (See REF_DCF_TABLE_s.) */
} REF_DCF_TABLE_HDLR_s;

/**
 * The interface of DCF table functions
 */
typedef struct REF_DCF_TABLE_s {
    UINT32 (*Create)(UINT32 MaxDir, UINT32 MaxFile, REF_DCF_TABLE_HDLR_s **Hdlr);   /**< The interface to create a table handler */
    UINT32 (*Delete)(REF_DCF_TABLE_HDLR_s *Hdlr);                                   /**< The interface to delete a table handler */
    UINT32 (*AddDirectory)(REF_DCF_TABLE_HDLR_s *Hdlr, UINT32 Dnum, char *Name);    /**< The interface to add a directory */
    UINT32 (*RemoveDirectory)(REF_DCF_TABLE_HDLR_s *Hdlr, char *Name);              /**< The interface to remove a directory (It could remove a parent directory, so the table must be scanned.) */
    UINT32 (*AddFile)(REF_DCF_TABLE_HDLR_s *Hdlr, UINT32 Dnum, UINT32 Fnum, char *Name);    /**< The interface to add a file */
    UINT32 (*RemoveFile)(REF_DCF_TABLE_HDLR_s *Hdlr, UINT32 Dnum, UINT32 Fnum, char *Name); /**< The interface to remove a file */
    UINT32 (*GetFirstDnum)(REF_DCF_TABLE_HDLR_s *Hdlr, UINT32 *Dnum);                       /**< The interface to get the first directory number (fnum would be set to first) */
    UINT32 (*GetLastDnum)(REF_DCF_TABLE_HDLR_s *Hdlr, UINT32 *Dnum);                    /**< The interface to get the last directory number (fnum would be set to last) */
    UINT32 (*GetNextDnum)(REF_DCF_TABLE_HDLR_s *Hdlr, UINT32 Dnum, UINT32 *Next);       /**< The interface to get the next directory number from the given dnum (dnum is just for reference, could be invalid) (fnum would be set to first) */
    UINT32 (*GetPrevDnum)(REF_DCF_TABLE_HDLR_s *Hdlr, UINT32 Dnum, UINT32 *Prev);       /**< The interface to get the previous directory number from the given dnum (dnum is just for reference, could be invalid) (fnum would be set to last) */
    UINT32 (*GetFirstFnum)(REF_DCF_TABLE_HDLR_s *Hdlr, UINT32 Dnum, UINT32 *Fnum);      /**< The interface to get the first file number (hdlr, dnum) */
    UINT32 (*GetLastFnum)(REF_DCF_TABLE_HDLR_s *Hdlr, UINT32 Dnum, UINT32 *Fnum);       /**< The interface to get the last file number (hdlr, dnum) */
    UINT32 (*GetNextFnum)(REF_DCF_TABLE_HDLR_s *Hdlr, UINT32 Dnum, UINT32 Fnum, UINT32 *Next);  /**< The interface to get the next file number from the given dnum and fnum (hdlr, dnum, fnum) (dnum and fnum are just for reference, could be invalid) */
    UINT32 (*GetPrevFnum)(REF_DCF_TABLE_HDLR_s *Hdlr, UINT32 Dnum, UINT32 Fnum, UINT32 *Prev);  /**< The interface to get the previous file number from the given dnum and fnum (hdlr, dnum, fnum) (dnum and fnum are just for reference, could be invalid) */
    UINT32 (*GetDirectoryList)(REF_DCF_TABLE_HDLR_s *Hdlr, UINT32 Dnum, REF_DCF_DIR_s *DirList, UINT32 MaxDir, UINT32 *Count);              /**< The interface to get a list of directories with a specified number (hdlr, dnum, the address to put the directory list) */
    UINT32 (*GetFileList)(REF_DCF_TABLE_HDLR_s *Hdlr, UINT32 Dnum, UINT32 Fnum, REF_DCF_FILE_s *FileList, UINT32 MaxFile, UINT32 *Count);   /**< The interface to get a list of files with a specified number (hdlr, fnum, the address to put the file list) */
    UINT32 (*CheckIdValid)(REF_DCF_TABLE_HDLR_s *Hdlr, UINT32 Dnum, UINT32 Fnum, UINT8 *Valid);     /**< The interface to check if an input ID is already in table, which is combined from a file number and a directory number (hdlr, dnum, fnum) */
} REF_DCF_TABLE_s;

typedef UINT32 (*REF_DCF_DIR_INFO_FP)(const REF_DCF_TABLE_HDLR_s *Table, const char *Name, UINT32 Dnum);

/**
 * The interface of DCF name filters
 */
typedef struct {
    UINT32 (*NameToDnum)(char *Name, UINT32 *Dnum);                     /**< The interface to convert name to directory number */
    UINT32 (*NameToId)(char *Name, UINT32 *Id);                         /**< The interface to convert name to ID */
    UINT32 (*GetId)(UINT32 Dnum, UINT32 Fnum, UINT32 *Id);              /**< The interface to get an ID in accordance with a directory number and a file number */
    UINT32 (*IdToDnum)(UINT32 Id, UINT32 *Dnum);                        /**< The interface to convert ID to directory number */
    UINT32 (*IdToFnum)(UINT32 Id, UINT32 *Fnum);                        /**< The interface to convert ID to file number */
    UINT32 (*ScanDirectory)(char *Path, REF_DCF_TABLE_HDLR_s *Table, REF_DCF_DIR_INFO_FP DirInfo); /**< The interface to scan directories in a specified root into a DCF table */
    UINT32 (*ScanFile)(char *Path, REF_DCF_TABLE_HDLR_s *Table);       /**< The interface to scan files in a specified root into a DCF table */
} REF_DCF_FILTER_s;

/**
 * DCF handler
 */
typedef struct {
    UINT8 Resv[4];
} REF_DCF_HDLR_s;

/**
 * The configuration for initializing a DCF handler
 */
typedef struct {
    REF_DCF_FILTER_s *Filter;       /**< DCF name filter */
    REF_DCF_TABLE_s *Table;    /**< DCF table handler */
    UINT32 MaxDir;                  /**< The maximum number of directories in a DCF handler */
    UINT32 MaxFile;                 /**< The maximum number of files in a DCF handler */
} REF_DCF_CFG_s;

// move DefFilter and DefTable to APP
/**
 * Get the required buffer size for initializing the DCF module.
 * @param [in] Config The init config
 * @param [out] BufferSize The required buffer size
 * @return 0 - OK, others - DCF_ERR_XXX
 */
UINT32 RefDcf_GetInitBufferSize(const REF_DCF_INIT_CFG_s *Config, UINT32 *BufferSize);

/**
 * Get the default configuration for initializing the DCF module.
 * @param [out] Config The returned configuration of the DCF module
 * @return 0 - OK, others - DCF_ERR_XXX
 */
UINT32 RefDcf_GetInitDefaultCfg(REF_DCF_INIT_CFG_s *Config);

/**
 * Get the default configuration for creating a DCF handler.
 * @param [out] Config The returned configuration of a DCF handler
 * @return 0 - OK, others - DCF_ERR_XXX
 */
UINT32 RefDcf_GetDefaultCfg(REF_DCF_CFG_s *Config);

/**
 * Initialize the DCF module.
 * @param [in] Config The configuration used to initialize the DCF module
 * @return 0 - OK, others - DCF_ERR_XXX
 */
UINT32 RefDcf_Init(const REF_DCF_INIT_CFG_s *Config);

/**
 * Create a DCF handler. The system can have multiple DCF handlers.
 * @param [in] Config The default configuration for creating a DCF handler
 * @param [out] Hdlr The returned DCF handler
 * @return 0 - OK, others - DCF_ERR_XXX
 */
UINT32 RefDcf_Create(const REF_DCF_CFG_s *Config, REF_DCF_HDLR_s **Hdlr);

/**
 * Delete a DCF handler.
 * @param [in] Hdlr The DCF handler being deleted
 * @return 0 - OK, others - DCF_ERR_XXX
 */
UINT32 RefDcf_Delete(REF_DCF_HDLR_s *Hdlr);

/**
 * Add a DCF root into a DCF handler. Only scan sub-directories into the DCF table.
 * Must be executed before RefDcf_Scan()
 * @param [in] Hdlr The DCF handler
 * @param [in] Path The path of a DCF root being added (e.g., c:\DCIM)
 * @return 0 - OK, others - DCF_ERR_XXX
 */
UINT32 RefDcf_AddRoot(REF_DCF_HDLR_s *Hdlr, char *Path);

/**
 * Remove a DCF root from a DCF handler.
 * @param [in] Hdlr The DCF handler
 * @param [in] Path The path of a DCF root being removed (e.g., c:\DCIM)
 * @return 0 - OK, others - DCF_ERR_XXX
 * @note Can be called after scan done
 */
UINT32 RefDcf_RemoveRoot(REF_DCF_HDLR_s *Hdlr, char *Path);

/**
 * Scan all the files in all the directories with the specified DNUM into a DCF table.
 * @param [in] Hdlr The DCF handler
 * @param [in] Dnum The directory number
 * @return 0 - OK, others - DCF_ERR_XXX
 * @note Can call RefDcf_GetXXXId() and RefDcf_GetFileList(Id) after this API, if the current Id's Dnum has been scanned; otherwise, return DCF_ERR_0005
 */
UINT32 RefDcf_Scan(REF_DCF_HDLR_s *Hdlr, UINT32 Dnum);

/**
 * Get the first ID.
 * @param [in] Hdlr The DCF handler
 * @param [out] IdThe ID of the last DCF object
 * @return 0 - OK, others - DCF_ERR_XXX
 * @note Return DCF_ERR_0005 if RefDcf_Scan(Dnum) has not been called
 */
UINT32 RefDcf_GetFirstId(REF_DCF_HDLR_s *Hdlr, UINT32 *Id);

/**
 * Get the last ID.
 * @param [in] Hdlr The DCF handler
 * @param [out] Id The ID of the last DCF object
 * @return 0 - OK, others - DCF_ERR_XXX
 * @note Return DCF_ERR_0005 if RefDcf_Scan(Dnum) has not been called
 */
UINT32 RefDcf_GetLastId(REF_DCF_HDLR_s *Hdlr, UINT32 *Id);

/**
 * Get the next nth object ID from the current ID (No Cycle).
 * @param [in] Hdlr The DCF handler
 * @param [out] Id The ID of the next nth DCF object
 * @return 0 - OK, others - DCF_ERR_XXX
 * @note Return DCF_ERR_0005 if RefDcf_Scan(Dnum) has not been called
 */
UINT32 RefDcf_GetNextId(REF_DCF_HDLR_s *Hdlr, UINT32 *Id);

/**
 * Get the previous nth object ID from the current ID (No Cycle).
 * @param [in] Hdlr The DCF handler
 * @param [out] Id The ID of the previous nth DCF object
 * @return 0 - OK, others - DCF_ERR_XXX
 * @note Return DCF_ERR_0005 if RefDcf_Scan(Dnum) has not been called
 */
UINT32 RefDcf_GetPrevId(REF_DCF_HDLR_s *Hdlr, UINT32 *Id);

/**
 * Get the list of files with a specified ID.
 * @param [in] Hdlr The DCF handler
 * @param [in] Id The object ID
 * @param [out] FileList The file list
 * @param [in] MaxFile Max file count
 * @param [out] Count The file count
 * @return 0 - OK, others - DCF_ERR_XXX
 * @note Return DCF_ERR_0005 if RefDcf_Scan(Dnum) has not been called
 */
UINT32 RefDcf_GetFileList(REF_DCF_HDLR_s *Hdlr, UINT32 Id, REF_DCF_FILE_s *FileList, UINT32 MaxFile, UINT32 *Count);

/**
 * Get the first directory number.
 * @param [in] Hdlr The DCF handler
 * @param [out] Dnum The first directory number
 * @return 0 - OK, others - DCF_ERR_XXX
 */
UINT32 RefDcf_GetFirstDnum(REF_DCF_HDLR_s *Hdlr, UINT32 *Dnum);

/**
 * Get the last directory number.
 * @param [in] Hdlr The DCF handler
 * @param [out] Dnum The last directory number
 * @return 0 - OK, others - DCF_ERR_XXX
 */
UINT32 RefDcf_GetLastDnum(REF_DCF_HDLR_s *Hdlr, UINT32 *Dnum);

/**
 * Get the next nth directory number from the current directory (No Cycle).
 * @param [in] Hdlr The DCF handler
 * @param [out] Dnum The number of the next nth directory
 * @return 0 - OK, others - DCF_ERR_XXX
 * @note Can be called after RefDcf_GetFirstDnum()/RefDcf_GetLastDnum()
 */
UINT32 RefDcf_GetNextDnum(REF_DCF_HDLR_s *Hdlr, UINT32 *Dnum);

/**
 * Get the previous nth directory number from the current directory (No Cycle).
 * @param [in] Hdlr The DCF handler
 * @param [out] Dnum The number of the previous nth directory
 * @return 0 - OK, others - DCF_ERR_XXX
 * @note Can be called after RefDcf_GetFirstDnum()/RefDcf_GetLastDnum()
 */
UINT32 RefDcf_GetPrevDnum(REF_DCF_HDLR_s *Hdlr, UINT32 *Dnum);

/**
 * Get the list of directories with a specified directory number.
 * @param [in] Hdlr The DCF handler
 * @param [in] Dnum The directory number
 * @param [out] DirList The directory list
 * @param [in] MaxDir Max directory count
 * @param [out] Count The directory count
 * @return 0 - OK, others - DCF_ERR_XXX
 */
UINT32 RefDcf_GetDirectoryList(REF_DCF_HDLR_s *Hdlr, UINT32 Dnum, REF_DCF_DIR_s *DirList, UINT32 MaxDir, UINT32 *Count);

/**
 * Add a directory to a DCF handler.
 * @param [in] Hdlr The DCF handler
 * @param [in] Name The directory name
 * @return 0 - OK, others - DCF_ERR_XXX
 * @note Return DCF_ERR_0005 before scan done
 */
UINT32 RefDcf_AddDirectory(REF_DCF_HDLR_s *Hdlr, char *Name);

/**
 * Remove a directory from a DCF handler.
 * @param [in] Hdlr The DCF handler
 * @param [in] Name The directory name
 * @return 0 - OK, others - DCF_ERR_XXX
 * @note Return DCF_ERR_0005 before scan done
 */
UINT32 RefDcf_RemoveDirectory(REF_DCF_HDLR_s *Hdlr, char *Name);

/**
 * Add a file to a DCF handler.
 * @param [in] Hdlr The DCF handler
 * @param [in] Name The file name
 * @return 0 - OK, others - DCF_ERR_XXX
 * @note Return DCF_ERR_0005 before scan done
 */
UINT32 RefDcf_AddFile(REF_DCF_HDLR_s *Hdlr, char *Name);

/**
 * Remove a file from a DCF handler.
 * @param [in] Hdlr The DCF handler
 * @param [in] Name The file name
 * @return 0 - OK, others - DCF_ERR_XXX
 * @note Return DCF_ERR_0005 before scan done
 */
UINT32 RefDcf_RemoveFile(REF_DCF_HDLR_s *Hdlr, char *Name);

/**
 * Check if an object is in a DCF handler.
 * @param [in] Hdlr The DCF handler
 * @param [in] Id The object ID
 * @param [out] Valid The ID is valid or not
 * @return 0 - OK, others - DCF_ERR_XXX
 */
UINT32 RefDcf_CheckIdValid(REF_DCF_HDLR_s *Hdlr, UINT32 Id, UINT8 *Valid);

/**
 * @}
 */
#endif

