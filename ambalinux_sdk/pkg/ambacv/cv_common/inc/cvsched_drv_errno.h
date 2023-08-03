/*
 * Copyright (c) 2017-2017 Ambarella, Inc.
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign copyrights.
 * This Software is also the confidential and proprietary information of
 * Ambarella, Inc. and its licensors.  You may not use, reproduce, disclose,
 * distribute, modify, or otherwise prepare derivative works of this Software
 * or any portion thereof except pursuant to a signed license agreement or
 * nondisclosure agreement with Ambarella, Inc. or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and
 * return this Software to Ambarella, Inc.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CVSCHED_DRV_ERRNO_H_FILE
#define CVSCHED_DRV_ERRNO_H_FILE

/*-------------------------------------------------------------------------------------*/
/* Interface errors : 0xD000.0000 - 0xD000.FFFF                                        */
/*-------------------------------------------------------------------------------------*/
#define ERR_INTF_BASE                                               0xD0000000U

#define ERR_INTF_SCHDR_VISCORE_UNAVAILABLE                          0xD0000100U    /**< Scheduler VISORC core is unavaiable for this chip */
#define ERR_INTF_FLEXIDAG_BIN_PATH_UNAVAILABLE                      0xD0000180U    /**< Flexidag bin file path open fail */
#define ERR_INTF_FLEXIDAG_MSG_UNAVAILABLE                           0xD0000181U    /**< Flexidag message entry is zero which set in parameter set */
#define ERR_INTF_FLEXIDAG_LOG_UNAVAILABLE                           0xD0000182U    /**< Flexidag log entry is zero which set in parameter set */
#define ERR_INTF_FLEXIDAG_LOG_TYPE_UNAVAILABLE                      0xD0000183U    /**< Flexidag log type not support */
#define ERR_INTF_FLEXIDAG_LOG_PATH_UNAVAILABLE                      0xD0000184U    /**< Flexidag log file path open fail */
#define ERR_INTF_FLEXIDAG_RUN_START_TIME_UNAVAILABLE                0xD0000185U    /**< Flexidag doen't find run start time when wait run finish */
#define ERR_INTF_ERRMGR_IPC_UNAVAILABLE                             0xD0000186U    /**< Error manager IPC is unavaiable which set in init flow */
#define ERR_INTF_ERRMGR_TIMESTAMP_ENTRY_UNAVAILABLE                 0xD0000187U    /**< Error manager timestamp array is full */

#define ERR_INTF_FLEXIDAG_MEMBLK_OUT_OF_RANGE                       0xD0000380U    /**< Flexidag memory block out of range */
#define ERR_INTF_FLEXIDAG_BIN_OUT_OF_RANGE                          0xD0000381U    /**< Flexidag bin memory block out of range */
#define ERR_INTF_FLEXIDAG_STATE_OUT_OF_RANGE                        0xD0000382U    /**< Flexidag state memory block out of range */
#define ERR_INTF_FLEXIDAG_TEMP_OUT_OF_RANGE                         0xD0000383U    /**< Flexidag temp memory block out of range */
#define ERR_INTF_FLEXIDAG_INPUT_OUT_OF_RANGE                        0xD0000384U    /**< Flexidag input memory block out of range */
#define ERR_INTF_FLEXIDAG_OUTPUT_OUT_OF_RANGE                       0xD0000385U    /**< Flexidag output memory block out of range */
#define ERR_INTF_FLEXIDAG_FEEDBACK_OUT_OF_RANGE                     0xD0000386U    /**< Flexidag feedback memory block out of range */
#define ERR_INTF_ERRMGR_TIMESTAMP_SLOT_OUT_OF_RANGE                 0xD0000387U    /**< Error manager timestamp slot ID out of range */

#define ERR_INTF_FLEXIDAG_PARAM_ID_UNKNOW                           0xD0000480U    /**< Flexidag parameter set ID is unknow */
#define ERR_INTF_FLEXIDAG_PERF_EVENT_UNKNOW                         0xD0000481U    /**< Flexidag performance event is unknow */
#define ERR_INTF_ERRMGR_MODULE_ID_UNKNOW                            0xD0000482U    /**< Error manager module ID is unknow */
#define ERR_INTF_ERRMGR_ERROR_ID_UNKNOW                             0xD0000483U    /**< Error manager error ID is unknow */
#define ERR_INTF_HWINFO_ID_UNKNOW                                   0xD0000484U    /**< Scheduler hwinfo ID is unknow */

#define ERR_INTF_SCHDR_VISORC_BIN_CRC_MISMATCHED                    0xD0000500U    /**< Scheduler VISORC binary crc is mismatch with pre-genetated CRC */
#define ERR_INTF_SCHDR_STATE_MISMATCHED                             0xD0000501U    /**< Scheduler state mismatch*/
#define ERR_INTF_SCHDR_VISORC_STATE_MISMATCHED                      0xD0000502U    /**< Scheduler VISORC state mismatch*/
#define ERR_INTF_SCHDR_HWINFO_SIZE_MISMATCHED                       0xD0000503U    /**< Scheduler hwinfo size is mismatch */
#define ERR_INTF_FLEXIDAG_BIN_CRC_MISMATCHED                        0xD0000580U    /**< Flexidag binary CRC is mismatch with pre-genetated CRC */
#define ERR_INTF_FLEXIDAG_PARAM_SIZE_MISMATCHED                     0xD0000581U    /**< Flexidag parameter set size is not match input type */
#define ERR_INTF_FLEXIDAG_STATE_SIZE_MISMATCHED                     0xD0000582U    /**< Flexidag state size is less than the size of memory requirement */
#define ERR_INTF_FLEXIDAG_TEMP_SIZE_MISMATCHED                      0xD0000583U    /**< Flexidag temp size is less than the size of memory requirement */
#define ERR_INTF_FLEXIDAG_OUTPUT_SIZE_MISMATCHED                    0xD0000584U    /**< Flexidag output size is less than the size of memory requirement */
#define ERR_INTF_FLEXIDAG_SLOT_STATE_MISMATCHED                     0xD0000585U    /**< Flexidag slot state is mismatch */
#define ERR_INTF_FLEXIDAG_METADATA_NAME_MISMATCHED                  0xD0000587U    /**< Flexidag metadata name mismatch */

#define ERR_INTF_INPUT_PARAM_INVALID                                0xD0000600U    /**< function input parameter address is invalid or null */
#define ERR_INTF_AMBACV_FLEXIDAG_HANDLE_INVALID                     0xD0000601U    /**< AMBACV Flexidag handle is NULL */
#define ERR_INTF_CMA_MEMBLK_INVALID                                 0xD0000602U    /**< CMA memory block is NULL */
#define ERR_INTF_FLEXIDAG_HANDLE_INVALID                            0xD0000680U    /**< Flexidag handle memory block is NULL */
#define ERR_INTF_FLEXIDAG_BIN_MEMBLK_INVALID                        0xD0000681U    /**< Flexidag bin memory block is NULL */
#define ERR_INTF_FLEXIDAG_STATE_MEMBLK_INVALID                      0xD0000682U    /**< Flexidag state memory block is NULL */
#define ERR_INTF_FLEXIDAG_TEMP_MEMBLK_INVALID                       0xD0000683U    /**< Flexidag temp memory block is NULL */
#define ERR_INTF_FLEXIDAG_INPUT_MEMBLK_INVALID                      0xD0000684U    /**< Flexidag input memory block is NULL */
#define ERR_INTF_FLEXIDAG_OUTPUT_MEMBLK_INVALID                     0xD0000685U    /**< Flexidag output memory block is NULL */
#define ERR_INTF_FLEXIDAG_FEEDBACK_MEMBLK_INVALID                   0xD0000686U    /**< Flexidag feedback memory block is NULL */
#define ERR_INTF_FLEXIDAG_IO_MEMBLK_INVALID                         0xD0000687U    /**< Flexidag io memory structure is NULL */
#define ERR_INTF_FLEXIDAG_MSG_INVALID                               0xD0000688U    /**< Flexidag message is NULL */
#define ERR_INTF_FLEXIDAG_RUN_CB_INVALID                            0xD0000689U    /**< Flexidag run callback is NULL */
#define ERR_INTF_FLEXIDAG_ERROR_CB_INVALID                          0xD000068AU    /**< Flexidag error callback is NULL */
#define ERR_INTF_FLEXIDAG_BIN_ALIGN_INVALID                         0xD0000691U    /**< Flexidag bin memory block is unalign */
#define ERR_INTF_FLEXIDAG_STATE_ALIGN_INVALID                       0xD0000692U    /**< Flexidag state memory block is unalign */
#define ERR_INTF_FLEXIDAG_TEMP_ALIGN_INVALID                        0xD0000693U    /**< Flexidag temp memory block is unalign */
#define ERR_INTF_FLEXIDAG_INPUT_ALIGN_INVALID                       0xD0000694U    /**< Flexidag input memory block is unalign */
#define ERR_INTF_FLEXIDAG_OUTPUT_ALIGN_INVALID                      0xD0000695U    /**< Flexidag output memory block is unalign */
#define ERR_INTF_FLEXIDAG_FEEDBACK_ALIGN_INVALID                    0xD0000696U    /**< Flexidag feedback memory block is unalign */

#define ERR_INTF_FLEXIDAG_BIN_LOAD_FAIL                             0xD0001000U    /**< Flexidag utility load binary fail */
#define ERR_INTF_FLEXIDAG_CACHE_CLEAN_FAIL                          0xD0001001U    /**< Flexidag cache clean fail */
#define ERR_INTF_FLEXIDAG_CACHE_INVALID_FAIL                        0xD0001002U    /**< Flexidag cache invalidate fail */
#define ERR_INTF_FLEXIDAG_FILE_OPEN_FAIL                            0xD0001010U    /**< Flexidag File open fail */
#define ERR_INTF_FLEXIDAG_FILE_SEEK_FAIL                            0xD0001011U    /**< Flexidag File seek fail */
#define ERR_INTF_FLEXIDAG_FILE_READ_FAIL                            0xD0001012U    /**< Flexidag File read fail */
#define ERR_INTF_FLEXIDAG_FILE_WRITE_FAIL                           0xD0001013U    /**< Flexidag File write fail */
#define ERR_INTF_FLEXIDAG_FILE_CLOSE_FAIL                           0xD0001014U    /**< Flexidag File close fail */

#define ERR_INTF_END                                                0xD000FFFFU
/*-= Drv scheduler specific fatal errors =---------------------------------------------*/
/* These errors are limited to the 0xDF00.0000 - 0xDF00.FFFF                           */
/*-------------------------------------------------------------------------------------*/
#define ERR_DRV_SCHDR_BASE                                          0xDF000000U

#define ERR_DRV_SCHDR_ALREADY_ACTIVATE                              0xDF000001U    /**< Scheduler is already activate */
#define ERR_DRV_SCHDR_NOT_ACTIVATE                                  0xDF000002U    /**< Scheduler is not activate */
#define ERR_DRV_SCHDR_ACTIVATE_FAIL                                 0xDF000003U    /**< Scheduler is activate fail */
#define ERR_DRV_SCHDR_IN_SAFETY_STATE                               0xDF000004U    /**< Scheduler is in safety state */
#define ERR_DRV_SCHDR_VISORC_ACTIVATE                               0xDF000005U    /**< Scheduler VISORC is already activate */
#define ERR_DRV_SCHDR_VISORC_NOT_ACTIVATE                           0xDF000006U    /**< Scheduler VISORC is not activate */
#define ERR_DRV_SCHDR_ISR_NOT_ACTIVATE                              0xDF000007U    /**< Scheduler ISR is not activate */

#define ERR_DRV_SCHDR_DEV_UNAVAILABLE                               0xDF000100U    /**< Scheduler open device fail*/
#define ERR_DRV_SCHDR_CMA_DEV_UNAVAILABLE                           0xDF000101U    /**< Scheduler open CMA device fail */
#define ERR_DRV_SCHDR_VISCORE_UNAVAILABLE                           0xDF000102U    /**< Scheduler VISORC core is unavaiable for this chip */
#define ERR_DRV_SCHDR_VISORC_BIN_SIZE_UNAVAILABLE                   0xDF000103U    /**< Scheduler VISORC binary size is 0 */
#define ERR_DRV_SCHDR_VISORC_HW_UNAVAILABLE                         0xDF000104U    /**< Scheduler VISORC HW clock is not set */
#define ERR_DRV_SCHDR_VISORC_SIZE_UNAVAILABLE                       0xDF000105U    /**< Scheduler VISORC load size is 0*/
#define ERR_DRV_SCHDR_VISORC_LOAD_SIZE_UNAVAILABLE                  0xDF000106U    /**< Scheduler VISORC not load when start*/

#define ERR_DRV_SCHDR_MSG_ENTRY_UNABLE_TO_FIND                      0xDF000200U    /**< Scheduler CVTask message is full */
#define ERR_DRV_SCHDR_RECEIVE_VISMSG_UNABLE_TO_FIND                 0xDF000201U    /**< Scheduler receive command message is empty */
#define ERR_DRV_SCHDR_RECEIVE_VISMSG_QUEUE_UNABLE_TO_FIND           0xDF000202U    /**< Scheduler can not find receive command message queue */
#define ERR_DRV_SCHDR_TOKEN_UNABLE_TO_FIND                          0xDF000203U    /**< Scheduler token is full */

#define ERR_DRV_SCHDR_CVTABLE_OUT_OF_RANGE                          0xDF000300U    /**< Scheduler CVTask cvtable number is out of range */
#define ERR_DRV_SCHDR_CVTABLE_SIZE_OUT_OF_RANGE                     0xDF000301U    /**< Scheduler CVTask cvtable total size is out of range */
#define ERR_DRV_SCHDR_SYSFLOW_OUT_OF_RANGE                          0xDF000302U    /**< Scheduler CVTask system flow table number is out of range */
#define ERR_DRV_SCHDR_THPOOL_OUT_OF_RANGE                           0xDF000303U    /**< Scheduler thread pool number is out of range */
#define ERR_DRV_SCHDR_THPOOL_JOB_OUT_OF_RANGE                       0xDF000304U    /**< Scheduler thread pool job number is out of range */
#define ERR_DRV_SCHDR_FLEXIDAG_SLOT_OUT_OF_RANGE                    0xDF000305U    /**< Scheduler initial setting of Flexidag slot number is out of range*/
#define ERR_DRV_SCHDR_VISORC_BIN_SIZE_OUT_OF_RANGE                  0xDF000306U    /**< Scheduler VISORC binary size is out of range */
#define ERR_DRV_SCHDR_ARM_CVTASK_OUT_OF_RANGE                       0xDF000307U    /**< Scheduler CVTask number is out of range */
#define ERR_DRV_SCHDR_CMA_ALLOC_TOTAL_SIZE_OUT_OF_RANGE             0xDF000308U    /**< Scheduler CMA alloc total size is out of range */
#define ERR_DRV_SCHDR_TOKEN_OUT_OF_RANGE                            0xDF000309U    /**< Scheduler token ID is out of range */
#define ERR_DRV_SCHDR_CLOCK_ID_OUT_OF_RANGE                         0xDF00030AU    /**< Scheduler clock ID is out of range */

#define ERR_DRV_SCHDR_BOOTUP_PHASE_UNKNOW                           0xDF000400U    /**< Scheduler bootup phase type is unknow in SCHEDMSG_SET_BOOTUP_PHASE */
#define ERR_DRV_SCHDR_CLUSTER_UNKNOW                                0xDF000401U    /**< Scheduler ARM cluster is unknow */
#define ERR_DRV_SCHDR_IOCTL_UNKNOW                                  0xDF000402U    /**< Scheduler ioctl is unknow */
#define ERR_DRV_SCHDR_VISMSG_UNKNOW                                 0xDF000403U    /**< Scheduler VISORC command message is unknow */

#define ERR_DRV_SCHDR_VERSION_MISMATCHED                            0xDF000503U    /**< Scheduler version is mismatch */
#define ERR_DRV_SCHDR_CAVALRY_VERSION_MISMATCHED                    0xDF000504U    /**< Scheduler cavalry version is mismatch */
#define ERR_DRV_SCHDR_SYSFLOW_SIZE_MISMATCHED                       0xDF000505U    /**< Scheduler CVTask system flow table size is not correct */
#define ERR_DRV_SCHDR_CVTASK_VERSION_MISMATCHED                     0xDF000506U    /**< Scheduler CVTask version is mismatch */
#define ERR_DRV_SCHDR_INTERNAL_THREAD_STATE_MISMATCHED              0xDF000507U    /**< Scheduler internal thread state is mismatch */
#define ERR_DRV_SCHDR_FLEXIDAG_SLOT_MISMATCHED                      0xDF000508U    /**< Scheduler initial setting of Flexidag slot number is mismatch in multi OS */
#define ERR_DRV_SCHDR_TOKEN_STATE_MISMATCHED                        0xDF000509U    /**< Scheduler token state is mismatch */
#define ERR_DRV_SCHDR_VISMSG_SEQNO_MISMATCHED                       0xDF00050AU    /**< Scheduler VISORC command message sequence number mismatch */
#define ERR_DRV_SCHDR_VISMSG_CHECKSUM_MISMATCHED                    0xDF00050BU    /**< Scheduler VISORC command message checksum mismatch */

#define ERR_DRV_SCHDR_INPUT_PARAM_INVALID                           0xDF000600U    /**< Scheduler function input parameter address is invalid or null */
#define ERR_DRV_SCHDR_VISMSG_INVALID                                0xDF000601U    /**< Scheduler VISORC command message address is invalid or null */
#define ERR_DRV_SCHDR_VISMSG_IDSP_INVALID                           0xDF000602U    /**< Scheduler VISORC IDSP command message address is invalid or null */
#define ERR_DRV_SCHDR_VISMSG_QUEUE_ORC2ARM_INVALID                  0xDF000603U    /**< Scheduler VISORC to ARM command message queue address is invalid or null */
#define ERR_DRV_SCHDR_VISMSG_QUEUE_ARM2ORC_INVALID                  0xDF000604U    /**< Scheduler ARM to VISORC command message queue address is invalid or null */
#define ERR_DRV_SCHDR_SYSTEM_START_INVALID                          0xDF000605U    /**< Scheduler system start address is invalid or null */
#define ERR_DRV_SCHDR_VISORC_INIT_INVALID                           0xDF000606U    /**< Scheduler VISORC init parameter address is invalid or null */
#define ERR_DRV_SCHDR_VISORC_BIN_LOAD_START_INVALID                 0xDF000607U    /**< Scheduler VISORC binary load start address is invalid or null */
#define ERR_DRV_SCHDR_MSG_POOL_INVALID                              0xDF000608U    /**< Scheduler CVTask message pool address is invalid or null */
#define ERR_DRV_SCHDR_CVTABLE_INVALID                               0xDF000609U    /**< Scheduler CVTask cvtable address is invalid or null */
#define ERR_DRV_SCHDR_CVTABLE_TOC_INVALID                           0xDF00060AU    /**< Scheduler CVTask cvtable toc (Table of Contents) address is invalid or null */
#define ERR_DRV_SCHDR_SYSFLOW_INVALID                               0xDF00060BU    /**< Scheduler CVTask system flow address is invalid or null */
#define ERR_DRV_SCHDR_LINFO_INVALID                                 0xDF00060CU    /**< Scheduler log info address is invalid or null */
#define ERR_DRV_SCHDR_FORMAT_INVALID                                0xDF00060DU    /**< Scheduler log format address is invalid or null */
#define ERR_DRV_SCHDR_BUILDINFO_INVALID                             0xDF00060EU    /**< Scheduler build info address is invalid or null */
#define ERR_DRV_SCHDR_FILE_DESC_INVALID                             0xDF00060FU    /**< Scheduler function input file descriptor is invalid or null */
#define ERR_DRV_SCHDR_SCRATCHPAD_INVALID                            0xDF000610U    /**< Scheduler CVTask scratchpad address is invalid or null */
#define ERR_DRV_SCHDR_TRACE_INVALID                                 0xDF000611U    /**< Scheduler trace address is invalid or null */
#define ERR_DRV_SCHDR_AUTORUN_TRACE_INVALID                         0xDF000612U    /**< Scheduler autorun trace address is invalid or null */
#define ERR_DRV_SCHDR_FLEXIDAG_TRACE_INVALID                        0xDF000613U    /**< Scheduler flexidag trace address is invalid or null */
#define ERR_DRV_SCHDR_INPUT_BUF_INVALID                             0xDF000614U    /**< Scheduler CVTask input buffer address is invalid or null */
#define ERR_DRV_SCHDR_OUTPUT_BUF_INVALID                            0xDF000615U    /**< Scheduler CVTask output buffer address is invalid or null */
#define ERR_DRV_SCHDR_FEEDBACK_BUF_INVALID                          0xDF000616U    /**< Scheduler CVTask feedback buffer address is invalid or null */
#define ERR_DRV_SCHDR_MSG_BUF_INVALID                               0xDF000617U    /**< Scheduler CVTask message buffer address is invalid or null */
#define ERR_DRV_SCHDR_CMA_BUF_INVALID                               0xDF000618U    /**< Scheduler CMA buffer address is invalid or null */

#define ERR_DRV_SCHDR_SEM_WAIT_TIMEOUT                              0xDF000700U    /**< Scheduler semaphore wait timeout */
#define ERR_DRV_SCHDR_EVENTFLAG_GET_TIMEOUT                         0xDF000701U    /**< Scheduler eventflag get timeout */
#define ERR_DRV_SCHDR_ERRMGR_HEARTBEAT_TIMEOUT                      0xDF000702U    /**< Scheduler error manager heartbeat timeout */
#define ERR_DRV_SCHDR_ERRMGR_FLEXIDAG_OPEN_TIMEOUT                  0xDF000703U    /**< Scheduler error manager flexidag open timeout */
#define ERR_DRV_SCHDR_ERRMGR_FLEXIDAG_INIT_TIMEOUT                  0xDF000704U    /**< Scheduler error manager flexidag init timeout */
#define ERR_DRV_SCHDR_ERRMGR_FLEXIDAG_RUN_TIMEOUT                   0xDF000705U    /**< Scheduler error manager flexidag run timeout */
#define ERR_DRV_SCHDR_ERRMGR_FLEXIDAG_WAIT_RUN_FINISH_TIMEOUT       0xDF000706U    /**< Scheduler error manager flexidag wait run finish timeout */
#define ERR_DRV_SCHDR_ERRMGR_FLEXIDAG_CLOSE_TIMEOUT                 0xDF000707U    /**< Scheduler error manager flexidag close timeout */

#define ERR_DRV_SCHDR_VISMSG_RET_FAIL                               0xDF001000U    /**< Scheduler VISORC command message return fail */
#define ERR_DRV_SCHDR_CVTASK_MAP_FAIL                               0xDF001001U    /**< Scheduler CVTask region mapping fail */
#define ERR_DRV_SCHDR_CVTASK_REMAP_FAIL                             0xDF001002U    /**< Scheduler CVTask region re-mapping fail */
#define ERR_DRV_SCHDR_MAP_FAIL                                      0xDF001003U    /**< Scheduler general region mapping fail */
#define ERR_DRV_SCHDR_UNMAP_FAIL                                    0xDF001004U    /**< Scheduler general region unmapping fail */
#define ERR_DRV_SCHDR_MAP_REG_FAIL                                  0xDF001005U    /**< Scheduler register region mapping fail */
#define ERR_DRV_SCHDR_CORE_MAP_FAIL                                 0xDF001006U    /**< Scheduler core region mapping fail */
#define ERR_DRV_SCHDR_BIN_MAP_FAIL                                  0xDF001007U    /**< Scheduler binary region mapping fail */
#define ERR_DRV_SCHDR_LOG_MAP_FAIL                                  0xDF001008U    /**< Scheduler log region mapping fail */
#define ERR_DRV_SCHDR_RTOS_MAP_FAIL                                 0xDF001009U    /**< Scheduler RTOS region mapping fail */
#define ERR_DRV_SCHDR_CMA_MAP_FAIL                                  0xDF00100AU    /**< Scheduler CMA region mapping fail */
#define ERR_DRV_SCHDR_CMA_UNMAP_FAIL                                0xDF00100BU    /**< Scheduler CMA region un-mapping fail */
#define ERR_DRV_SCHDR_CACHE_INIT_FAIL                               0xDF00100CU    /**< Scheduler cache init fail */
#define ERR_DRV_SCHDR_CACHE_CLEAN_FAIL                              0xDF00100DU    /**< Scheduler cache clean fail */
#define ERR_DRV_SCHDR_CACHE_INVALID_FAIL                            0xDF00100EU    /**< Scheduler cache invalidate fail */
#define ERR_DRV_SCHDR_SEM_INIT_FAIL                                 0xDF00100FU    /**< Scheduler semaphore init fail */
#define ERR_DRV_SCHDR_SEM_DEINIT_FAIL                               0xDF001010U    /**< Scheduler semaphore de-init fail */
#define ERR_DRV_SCHDR_SEM_WAIT_FAIL                                 0xDF001011U    /**< Scheduler semaphore wait fail */
#define ERR_DRV_SCHDR_SEM_POST_FAIL                                 0xDF001012U    /**< Scheduler semaphore post fail */
#define ERR_DRV_SCHDR_MUTEX_INIT_FAIL                               0xDF001013U    /**< Scheduler mutex init fail */
#define ERR_DRV_SCHDR_MUTEX_DEINIT_FAIL                             0xDF001014U    /**< Scheduler mutex de-init fail */
#define ERR_DRV_SCHDR_MUTEX_LOCK_FAIL                               0xDF001015U    /**< Scheduler mutex lock fail */
#define ERR_DRV_SCHDR_MUTEX_TRY_LOCK_FAIL                           0xDF001016U    /**< Scheduler mutex try lock fail */
#define ERR_DRV_SCHDR_MUTEX_UNLOCK_FAIL                             0xDF001017U    /**< Scheduler mutex unlock fail */
#define ERR_DRV_SCHDR_EVENTFLAG_INIT_FAIL                           0xDF001018U    /**< Scheduler eventflag init fail */
#define ERR_DRV_SCHDR_EVENTFLAG_SET_FAIL                            0xDF001019U    /**< Scheduler eventflag set fail */
#define ERR_DRV_SCHDR_EVENTFLAG_GET_FAIL                            0xDF00101AU    /**< Scheduler eventflag get fail */
#define ERR_DRV_SCHDR_THREAD_CREATE_FAIL                            0xDF00101BU    /**< Scheduler thread create fail */
#define ERR_DRV_SCHDR_THREAD_DELETE_FAIL                            0xDF00101CU    /**< Scheduler thread delete fail */
#define ERR_DRV_SCHDR_IRQ_ENABLE_FAIL                               0xDF00101DU    /**< Scheduler irq enable fail */
#define ERR_DRV_SCHDR_IRQ_DISABLE_FAIL                              0xDF00101EU    /**< Scheduler irq disable fail */
#define ERR_DRV_SCHDR_COPY_FROM_USER_FAIL                           0xDF00101FU    /**< Scheduler copy from user fail */
#define ERR_DRV_SCHDR_COPY_TO_USER_FAIL                             0xDF001020U    /**< Scheduler copy to user fail */
#define ERR_DRV_SCHDR_CMA_ALLOC_FAIL                                0xDF001021U    /**< Scheduler CMA allocate fail */
#define ERR_DRV_SCHDR_CMA_FREE_FAIL                                 0xDF001022U    /**< Scheduler CMA free fail */
#define ERR_DRV_SCHDR_CMA_SYNC_FAIL                                 0xDF001023U    /**< Scheduler CMA SYNC cache fail */
#define ERR_DRV_SCHDR_CMA_GET_USAGE_FAIL                            0xDF001024U    /**< Scheduler CMA get usage fail */
#define ERR_DRV_SCHDR_FILE_SEEK_FAIL                                0xDF001025U    /**< Scheduler File seek fail */
#define ERR_DRV_SCHDR_FILE_READ_FAIL                                0xDF001026U    /**< Scheduler File read fail */
#define ERR_DRV_SCHDR_FILE_CLOSE_FAIL                               0xDF001027U    /**< Scheduler File close fail */
#define ERR_DRV_SCHDR_VISORC_LOAD_FAIL                              0xDF001028U    /**< Scheduler VISORC binary load fail */
#define ERR_DRV_SCHDR_CVTASK_INIT_MOD_FAIL                          0xDF001029U    /**< Scheduler CVTask module init fail */
#define ERR_DRV_SCHDR_THPOOL_ADD_TSK_FAIL                           0xDF00102AU    /**< Scheduler thread pool add task fail */
#define ERR_DRV_SCHDR_GET_ATT_RANGE_FAIL                            0xDF00102BU    /**< Scheduler get ATT range fail */
#define ERR_DRV_SCHDR_FLEXIDAG_CREATE_FAIL                          0xDF00102CU    /**< Scheduler Flexidag slot create fail */
#define ERR_DRV_SCHDR_MEMCPY_FAIL                                   0xDF00102DU    /**< Scheduler memcpy fail */
#define ERR_DRV_SCHDR_MEMSET_FAIL                                   0xDF00102EU    /**< Scheduler memset fail */
#define ERR_DRV_SCHDR_FILE_WRITE_FAIL                               0xDF00102FU    /**< Scheduler File write fail */
#define ERR_DRV_SCHDR_SYSFLOW_FILL_FAIL                             0xDF001030U    /**< Scheduler sysflow fill fail */
#define ERR_DRV_SCHDR_CVTABLE_FILL_FAIL                             0xDF001031U    /**< Scheduler cvtable fill fail */
#define ERR_DRV_SCHDR_P2C_FAIL                                      0xDF001032U    /**< Scheduler ATT translate p2c fail */
#define ERR_DRV_SCHDR_C2P_FAIL                                      0xDF001033U    /**< Scheduler ATT translate c2p fail*/
#define ERR_DRV_SCHDR_ERRMGR_IPC_ALLOC_FAIL                         0xDF001034U    /**< Scheduler error manager IPC alloc fail */
#define ERR_DRV_SCHDR_ERRMGR_IPC_REGISRER_FAIL                      0xDF001035U    /**< Scheduler error manager IPC register fail */
#define ERR_DRV_SCHDR_ERRMGR_IPC_SEND_FAIL                          0xDF001036U    /**< Scheduler error manager IPC send fail */
#define ERR_DRV_SCHDR_GET_CLOCK_FAIL                                0xDF001037U    /**< Scheduler get clock fail */
#define ERR_DRV_SCHDR_FDT_CHECK_HEADER_FAIL                         0xDF001038U    /**< Scheduler FDT check header fail */
#define ERR_DRV_SCHDR_FDT_PATH_OFFSET_FAIL                          0xDF001039U    /**< Scheduler FDT path offset fail */
#define ERR_DRV_SCHDR_FDT_GET_PROPERTY_FAIL                         0xDF001040U    /**< Scheduler FDT get property fail */
#define ERR_DRV_SCHDR_THREAD_SLEEP_FAIL                             0xDF001041U    /**< Scheduler thread sleep fail */
#define ERR_DRV_SCHDR_GET_TIME_FAIL                                 0xDF001042U    /**< Scheduler get time fail */

#define ERR_DRV_SCHDR_END                                           0xDF00FFFFU
/*-= Drv flexidag specific fatal errors =----------------------------------------------*/
/* These errors are limited to the 0xDF01.0000 - 0xDF01.FFFF range                     */
/*-----------------------------------------------------------------------------------  */
#define ERR_DRV_FLEXIDAG_BASE                                       0xDF010000U

#define ERR_DRV_FLEXIDAG_SCHDR_UNAVAILABLE                          0xDF010101U    /**< Flexidag check scheduler not start */
#define ERR_DRV_FLEXIDAG_DEV_UNAVAILABLE                            0xDF010102U    /**< Flexidag check scheduler device not opened */
#define ERR_DRV_FLEXIDAG_HANDLE_UNAVAILABLE                         0xDF010103U    /**< Flexidag check handle is not created */
#define ERR_DRV_FLEXIDAG_VISCORE_UNAVAILABLE                        0xDF010104U    /**< Flexidag VISORC core is unavaiable for this chip */

#define ERR_DRV_FLEXIDAG_MSG_ENTRY_UNABLE_TO_FIND                   0xDF010200U    /**< Flexidag message is full */
#define ERR_DRV_FLEXIDAG_TOKEN_UNABLE_TO_FIND                       0xDF010201U    /**< Flexidag token is full */
#define ERR_DRV_FLEXIDAG_METADATA_UNABLE_TO_FIND                    0xDF010202U    /**< Flexidag metadata unable to find */

#define ERR_DRV_FLEXIDAG_SLOT_OUT_OF_RANGE                          0xDF010300U    /**< Flexidag slot ID is out of range */
#define ERR_DRV_FLEXIDAG_INTPUT_NUM_OUT_OF_RANGE                    0xDF010301U    /**< Flexidag input number is out of range */
#define ERR_DRV_FLEXIDAG_OUTPUT_NUM_OUT_OF_RANGE                    0xDF010302U    /**< Flexidag output number is out of range */
#define ERR_DRV_FLEXIDAG_FEEDBACK_NUM_OUT_OF_RANGE                  0xDF010303U    /**< Flexidag feedback number is out of range */
#define ERR_DRV_FLEXIDAG_MSG_SIZE_OUT_OF_RANGE                      0xDF010304U    /**< Flexidag message size is out of range */
#define ERR_DRV_FLEXIDAG_TOKEN_OUT_OF_RANGE                         0xDF010305U    /**< Flexidag token ID is out of range */
#define ERR_DRV_FLEXIDAG_CVTABLE_OUT_OF_RANGE                       0xDF010306U    /**< Flexidag cvtable number is out of range */
#define ERR_DRV_FLEXIDAG_SYSFLOW_OUT_OF_RANGE                       0xDF010307U    /**< Flexidag system flow number is out of range */

#define ERR_DRV_FLEXIDAG_PARAM_ID_UNKNOW                            0xDF010400U    /**< Flexidag parameter id is unknow */
#define ERR_DRV_FLEXIDAG_VISMSG_UNKNOW                              0xDF010401U    /**< Flexidag VISORC command message is unknow */
#define ERR_DRV_FLEXIDAG_IOCTL_UNKNOW                               0xDF010402U    /**< Flexidag ioctl is unknow */

#define ERR_DRV_FLEXIDAG_TOKEN_STATE_MISMATCHED                     0xDF010500U    /**< Flexidag token state is mismatch */
#define ERR_DRV_FLEXIDAG_HANDLE_MISMATCHED                          0xDF010501U    /**< Flexidag handle is not expected */
#define ERR_DRV_FLEXIDAG_SYSFLOW_MISMATCHED                         0xDF010502U    /**< Flexidag system flow is not found */
#define ERR_DRV_FLEXIDAG_CVTABLE_MISMATCHED                         0xDF010503U    /**< Flexidag cvtable is not found */
#define ERR_DRV_FLEXIDAG_SLOT_STATE_MISMATCHED                      0xDF010504U    /**< Flexidag slot state is mismatch */
#define ERR_DRV_FLEXIDAG_VISMSG_SEQNO_MISMATCHED                    0xDF010505U    /**< Flexidag VISORC command message sequence number mismatch */
#define ERR_DRV_FLEXIDAG_VISMSG_CHECKSUM_MISMATCHED                 0xDF010506U    /**< Flexidag VISORC command message checksum mismatch */

#define ERR_DRV_FLEXIDAG_INPUT_PARAM_INVALID                        0xDF010600U    /**< Flexidag function input parameter address is null */
#define ERR_DRV_FLEXIDAG_HANDLE_INVALID                             0xDF010601U    /**< Flexidag handle address is null */
#define ERR_DRV_FLEXIDAG_VISMSG_INVALID                             0xDF010602U    /**< Flexidag VISORC command message address is null */
#define ERR_DRV_FLEXIDAG_MSG_POOL_INVALID                           0xDF010603U    /**< Flexidag message pool address is null */
#define ERR_DRV_FLEXIDAG_MSG_INVALID                                0xDF010604U    /**< Flexidag message content address is null */
#define ERR_DRV_FLEXIDAG_MSG_ENTRY_INVALID                          0xDF010605U    /**< Flexidag message entry address is null */
#define ERR_DRV_FLEXIDAG_CVTABLE_INVALID                            0xDF010606U    /**< Flexidag cvtable address is null */
#define ERR_DRV_FLEXIDAG_SYSFLOW_INVALID                            0xDF010607U    /**< Flexidag system flow address is null */
#define ERR_DRV_FLEXIDAG_VISORC_PERF_INVALID                        0xDF010608U    /**< Flexidag VISORC perf log address is null */
#define ERR_DRV_FLEXIDAG_VISORC_SCHDR_LOG_INVALID                   0xDF010609U    /**< Flexidag VISORC scheduler log address is null */
#define ERR_DRV_FLEXIDAG_VISORC_CVTASK_LOG_INVALID                  0xDF01060AU    /**< Flexidag VISORC CVTask log address is null */
#define ERR_DRV_FLEXIDAG_ARM_PERF_INVALID                           0xDF01060BU    /**< Flexidag ARM perf log address is null */
#define ERR_DRV_FLEXIDAG_ARM_SCHDR_LOG_INVALID                      0xDF01060CU    /**< Flexidag ARM scheduler log address is null */
#define ERR_DRV_FLEXIDAG_ARM_CVTASK_LOG_INVALID                     0xDF01060DU    /**< Flexidag ARM CVTask log address is null */
#define ERR_DRV_FLEXIDAG_CODE_BASE_INVALID                          0xDF01060EU    /**< Flexidag code base address is null */
#define ERR_DRV_FLEXIDAG_BLOCK_BASE_INVALID                         0xDF01060FU    /**< Flexidag block base address is null */
#define ERR_DRV_FLEXIDAG_LINFO_INVALID                              0xDF010610U    /**< Flexidag log info address is null */
#define ERR_DRV_FLEXIDAG_FLEXIBIN_BUF_INVALID                       0xDF010611U    /**< Flexidag binary buffer address is null */
#define ERR_DRV_FLEXIDAG_STATE_BUF_INVALID                          0xDF010612U    /**< Flexidag state buffer address is null */
#define ERR_DRV_FLEXIDAG_INPUT_BUF_INVALID                          0xDF010613U    /**< Flexidag input buffer address is null */
#define ERR_DRV_FLEXIDAG_OUTPUT_BUF_INVALID                         0xDF010614U    /**< Flexidag output buffer address is null */
#define ERR_DRV_FLEXIDAG_FEEDBACK_BUF_INVALID                       0xDF010615U    /**< Flexidag feedback buffer address is null */
#define ERR_DRV_FLEXIDAG_MSG_BUF_INVALID                            0xDF010616U    /**< Flexidag message buffer address is null */

#define ERR_DRV_FLEXIDAG_SEM_WAIT_TIMEOUT                           0xDF010700U    /**< Flexidag semaphore wait timeout */

#define ERR_DRV_FLEXIDAG_VISMSG_SEND_FAIL                           0xDF011000U    /**< Flexidag VISORC command message send fail */
#define ERR_DRV_FLEXIDAG_VISMSG_RET_FAIL                            0xDF011001U    /**< Flexidag VISORC command message return fail */
#define ERR_DRV_FLEXIDAG_BUILDINFO_FAIL                             0xDF011002U    /**< Flexidag get build info fail */
#define ERR_DRV_FLEXIDAG_CACHE_CLEAN_FAIL                           0xDF011003U    /**< Flexidag cache clean fail */
#define ERR_DRV_FLEXIDAG_CACHE_INVALID_FAIL                         0xDF011004U    /**< Flexidag cache invalidate fail */
#define ERR_DRV_FLEXIDAG_SEM_INIT_FAIL                              0xDF011005U    /**< Flexidag semaphore init fail */
#define ERR_DRV_FLEXIDAG_SEM_DEINIT_FAIL                            0xDF011006U    /**< Flexidag semaphore de-init fail */
#define ERR_DRV_FLEXIDAG_SEM_WAIT_FAIL                              0xDF011007U    /**< Flexidag semaphore wait fail */
#define ERR_DRV_FLEXIDAG_SEM_POST_FAIL                              0xDF011008U    /**< Flexidag semaphore post fail */
#define ERR_DRV_FLEXIDAG_MUTEX_INIT_FAIL                            0xDF011009U    /**< Flexidag mutex init fail */
#define ERR_DRV_FLEXIDAG_MUTEX_LOCK_FAIL                            0xDF01100AU    /**< Flexidag mutex lock fail */
#define ERR_DRV_FLEXIDAG_MUTEX_UNLOCK_FAIL                          0xDF01100BU    /**< Flexidag mutex unlock fail */
#define ERR_DRV_FLEXIDAG_INVALID_LOGGER_FAIL                        0xDF01100CU    /**< Flexidag invalid logger fail */
#define ERR_DRV_FLEXIDAG_MEMCPY_FAIL                                0xDF01100DU    /**< Flexidag memcpy fail */
#define ERR_DRV_FLEXIDAG_MEMSET_FAIL                                0xDF01100EU    /**< Flexidag memset fail */
#define ERR_DRV_FLEXIDAG_FILE_OPEN_FAIL                             0xDF011010U    /**< Flexidag File open fail */
#define ERR_DRV_FLEXIDAG_FILE_SEEK_FAIL                             0xDF011011U    /**< Flexidag File seek fail */
#define ERR_DRV_FLEXIDAG_FILE_READ_FAIL                             0xDF011012U    /**< Flexidag File read fail */
#define ERR_DRV_FLEXIDAG_FILE_WRITE_FAIL                            0xDF011013U    /**< Flexidag File write fail */
#define ERR_DRV_FLEXIDAG_FILE_CLOSE_FAIL                            0xDF011014U    /**< Flexidag File close fail */

#define ERR_DRV_FLEXIDAG_END                                        0xDF01FFFFU

static inline uint32_t is_drv_interface_err(uint32_t x)
{
    uint32_t ret_val = 0U;

    if( (x > ERR_INTF_BASE) && (x < ERR_INTF_END) ) {
        ret_val = 1U;
    }
    return ret_val;
} /* is_drv_interface_err() */

static inline uint32_t is_drv_scheduler_err(uint32_t x)
{
    uint32_t ret_val = 0U;

    if( (x > ERR_DRV_SCHDR_BASE) && (x < ERR_DRV_SCHDR_END) ) {
        ret_val = 1U;
    }
    return ret_val;
} /* is_drv_scheduler_err() */

static inline uint32_t is_drv_flexidag_err(uint32_t x)
{
    uint32_t ret_val = 0U;

    if( (x > ERR_DRV_FLEXIDAG_BASE) && (x < ERR_DRV_FLEXIDAG_END) ) {
        ret_val = 1U;
    }
    return ret_val;
} /* is_drv_flexidag_err() */


#define CV_MODULE_ID_NUM            2U
#define CV_MODULE_ID_BASE           0x01U
#define CV_MODULE_ID_DRIVER         (CV_MODULE_ID_BASE       )
#define CV_MODULE_ID_UCODE          (CV_MODULE_ID_BASE + 0x1U)

#define CV_BITMASK_ALL              0xFFU
#define CV_BITMASK_INTF             0x01U
#define CV_BITMASK_SCHDR            0x02U
#define CV_BITMASK_FLEXIDAG         0x04U

#define CV_TIMESTAMP_SLOT_NUM       1024U
#define CV_TIMESTAMP_FLAG_STOP      0U
#define CV_TIMESTAMP_FLAG_START     1U
#define CV_TIMESTAMP_FLAG_COUNT     2U


uint32_t errmgr_ipc_send_timestamp_start(uint32_t ModuleID, uint32_t SlotID, uint32_t ErrorID, uint32_t Timeout);
uint32_t errmgr_ipc_send_timestamp_stop(uint32_t ModuleID, uint32_t SlotID, uint32_t ErrorID);
uint32_t errmgr_get_timestamp_slot(uint32_t *SlotID);
uint32_t errmgr_report_error(uint32_t ModuleID, uint32_t ErrorID);
uint32_t errmgr_ipc_send_heartbeat_config(uint64_t Addr);
uint32_t errmgr_init(void);

#endif /* !CVSCHED_DRV_ERRNO_H_FILE */

