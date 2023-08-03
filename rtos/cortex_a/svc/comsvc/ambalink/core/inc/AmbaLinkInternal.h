/**
 * @file AmbaLinkInternal.h
 *
 * Copyright (c) 2020 Ambarella International LP
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
 * @details Definitions & Constants for Ambarella CPU Link and Network Supports.
 *
 */

#ifndef AMBA_LINK_INTERNAL_H
#define AMBA_LINK_INTERNAL_H

#define LOCK_PORT(port) AmbaKAL_MutexTake(&(port).lock, AMBA_KAL_WAIT_FOREVER)
#define UNLOCK_PORT(port) AmbaKAL_MutexGive(&(port).lock)
#define UNUSED(x) ((x) = (x))

#ifdef RPC_DEBUG
/*record the rpc profiling result, the definition is shared
between dual-OSes.*/
/* Note the statistics of rpmsg is stored after rpc.
If there is any modification with the structure AMBA_RPC_STATISTIC_s,
please update the rpmsg starting address in Ambalink.h*/
typedef struct {
    /***************ThreadX side*****************/
    UINT32 TxLastInjectTime;
    UINT32 TxTotalInjectTime;
    UINT32 TxRpcSendTime;
    UINT32 TxRpcRecvTime;
    /********************************************/
    /***************Linux side*******************/
    UINT32 LuLastInjectTime;
    UINT32 LuTotalInjectTime;
    UINT32 LkToLuTime;
    UINT32 LuToLkTime;
    UINT32 MaxLkToLuTime;
    UINT32 MinLkToLuTime;
    UINT32 MaxLuToLkTime;
    UINT32 MinLuToLkTime;
    /********************************************/
    /***************dual OSes********************/
    UINT32 TxToLuTime;
    UINT32 LuToTxTime;
    UINT32 MaxTxToLuTime;
    UINT32 MinTxToLuTime;
    UINT32 MaxLuToTxTime;
    UINT32 MinLuToTxTime;
    UINT32 TxToLuCount;
    UINT32 LuToTxCount;
    UINT32 SynPktCount;
    UINT32 AsynPktCount;
    UINT32 RoundTripTime;
    UINT32 OneWayTime;
    /********************************************/
} AMBA_RPC_STATISTIC_s;

static inline UINT32 calc_timer_diff(UINT32 start, UINT32 end)
{
    UINT32 diff;
    if(end <= start) {
        diff = start - end;
    } else {
        diff = 0xFFFFFFFFU - end + 1U + start;
    }
    return diff;
}
#endif

#ifdef RPMSG_DEBUG
/******************************The defintion is shared between dual-OSes*******************************/
typedef struct {
    UINT32 ToGetSvqBuffer;
    UINT32 GetSvqBuffer;
    UINT32 SvqToSendInterrupt;
    UINT32 SvqSendInterrupt;
} AMBA_RPMSG_PROFILE_s;

typedef struct {
    /**********************************ThreadX side**************************/
    UINT32 TxLastInjectTime;
    UINT32 TxTotalInjectTime;
    UINT32 TxSendRpmsgTime;
    UINT32 TxResponseTime;
    UINT32 MaxTxResponseTime;
    UINT32 TxRecvRpmsgTime;
    UINT32 TxRecvCallBackTime;
    UINT32 TxReleaseVqTime;
    UINT32 TxToLxRpmsgTime;
    UINT32 MaxTxToLxRpmsgTime;
    UINT32 MinTxToLxRpmsgTime;
    UINT32 MaxTxRecvCBTime;
    UINT32 MinTxRecvCBTime;
    INT32 TxToLxCount;
    INT32 TxToLxWakeUpCount;
    /************************************************************************/
    /**********************************Linux side****************************/
    UINT32 LxLastInjectTime;
    UINT32 LxTotalInjectTime;
    UINT32 LxSendRpmsgTime;
    UINT32 LxResponseTime;
    UINT32 MaxLxResponseTime;
    UINT32 LxRecvRpmsgTime;
    UINT32 LxRecvCallBackTime;
    UINT32 LxReleaseVqTime;
    UINT32 LxToTxRpmsgTime;
    UINT32 MaxLxToTxRpmsgTime;
    UINT32 MinLxToTxRpmsgTime;
    UINT32 MaxLxRecvCBTime;
    UINT32 MinLxRecvCBTime;
    INT32 LxRvqIsrCount;
    INT32 LxToTxCount;
    /************************************************************************/
} AMBA_RPMSG_STATISTIC_s;
#endif

/**
 * File parameters: for opening a file in CFS
 */
typedef struct {
    char Filename[64];                                      /**< File name */
    UINT8 AsyncData;                                        /**< Asynchronous mode related data */
    UINT32 Alignment;                                       /**< Alignment in Byte, should be times of a cluster */
    UINT32 BytesToSync;                                     /**< File should be sync after BytesToSync bytes were write */
    UINT8 Mode;                                             /**< The open mode of the File */
    UINT8 AsyncMode;                                        /**< To indicate if the stream is in asynchronous mode. */
    UINT8 LowPriority;                                      /**< To indiFile if the stream is with low priority */
} AMBA_VFS_FILE_PARAM_s;

/**
 *  Define the virtula file system operation.
 */
typedef struct {
    void *(*fopen)(void *);                                     /**< fopen */
    UINT32 (*fclose)(void *);                                   /**< fclose */
    UINT32 (*fread)(void *, UINT32, UINT32, void *, UINT32 *);  /**< fread */
    UINT32 (*fwrite)(void *, UINT32, UINT32, void *, UINT32 *); /**< fwrite */
    UINT32 (*fseek)(void *, INT64, INT32);                      /**< fseek */
    UINT32 (*ftell)(void *, UINT64 *);                          /**< ftell */
    UINT32 (*FSync)(void *);                                    /**< FSync */
    UINT32 (*fappend)(void *, UINT64, UINT64 *);                /**< fappend */
    UINT32 (*feof)(void *);                                     /**< feof */
    UINT32 (*remove)(const char *);                             /**< remove */
    UINT32 (*Move)(const char *, const char *);                 /**< Move */
    UINT32 (*Stat)(const char *, void *);                       /**< Stat */
    UINT32 (*Chmod)(const char *, UINT32);                      /**< Chmod */
    UINT32 (*FirstDirEnt)(const char *, UINT8, void *);         /**< FirstDirEnt */
    UINT32 (*NextDirEnt)(void *);                               /**< NextDirEnt */
    UINT32 (*Combine)(const char *, const char *);              /**< Combine */
    UINT32 (*Divide)(const char *, const char *, UINT64);       /**< Divide */
    UINT32 (*Cinsert)(const char *, UINT32, UINT32);            /**< Cinsert */
    UINT32 (*Cdelete)(const char *, UINT32, UINT32);            /**< Cdelete */
    UINT32 (*Mkdir)(const char *);                              /**< Mkdir */
    UINT32 (*Rmdir)(const char *);                              /**< Rmdir */
    UINT32 (*GetDev)(char, void *);                             /**< GetDev */
    UINT32 (*Format)(char);                                     /**< Format */
    UINT32 (*Sync)(char, INT32);                                /**< Sync */
    UINT32 (*GetFileParam)(AMBA_VFS_FILE_PARAM_s *fileParam);   /**< GetFileParam */
} AMBA_VFS_OP_s;

/**
 * @brief This function is used to register the file system operation.
 * In ambafs, the registered file system operation is invoked. AmbaFS_XXX
 * is the default operation. Currently, the supported file system operations
 * are AmbaFS_XXX and AmpCFS_XXXX.
 *
 * @param [in] Operation The interface of the file system operation.
 * @return 0 if success
 * @see AMBA_VFS_OP_s
 */
INT32 AmbaIPC_RegisterVFSOperation(AMBA_VFS_OP_s *Operation);

/** @} */ // end of AmbaLink

#endif  /* AMBA_LINK_INTERNAL_H */
