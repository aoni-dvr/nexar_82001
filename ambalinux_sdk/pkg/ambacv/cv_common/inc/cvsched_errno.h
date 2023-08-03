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

#ifndef CVSCHED_ERRNO_H_FILE
#define CVSCHED_ERRNO_H_FILE

/*------------------------------------------------------------------------------------------------*/
/* API errors : 0xA000.0000 - 0xAFFF.FFFF                                                         */
/*------------------------------------------------------------------------------------------------*/

#define CVSCH_ERRCODE_NONE                          0x00000000U /* Maps to ERRCODE_NONE */

/*-= CVFlow/API specific errors =-------------------------------------------------------*/
/* These errors are limited to the 0xAF00.0000 - 0xAF0F.FFFF                            */
/*--------------------------------------------------------------------------------------*/
#define ERR_CVFLOW_API_RETCODE_BASE                 0xA0000000U /* vvvv CVFLOW API ERROR BLOCK vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv */
#define ERR_INVALID_SYSFLOW_INDEX                   0xA0000000U /* Invalid sysflow index provided                                                   */
#define CVSCH_APIERR_INVALID_PARAMETER              0xA0000001U /* Invalid parameter provided                                                       */
                                                                /*                                                                                  */
#define ERR_SYSFLOW_NOT_LOADED                      0xA0001000U /* No system flow loaded; API function cannot process                               */
#define ERR_SYSFLOW_CANNOT_FIND_UUID                0xA0001001U /* Cannot find UUID in a particular sysflow table                                   */
#define ERR_PROVIDED_INDEXLIST_TOO_SHORT            0xA0001002U /* Indexlist provided to API function is too short; list is truncated               */
/*                                                                                  */
#define ERR_NO_CVTABLES_LOADED                      0xA0002000U /* No CVTables loaded; Function returns nothing found.                              */
#define ERR_CVTABLE_NAME_NOT_FOUND                  0xA0002001U /* cvtable_find() unable to find requested cvtable; Function returns nothing found. */
/*                                                                                  */
#define ERR_CVMSG_DEFAULT_MSGPOOL_INVALID           0xA0009000U /* Default message pool is invalid for message_create; function call is ignored     */
#define ERR_CVMSG_INVALID_CUSTOM_POOL_INDEX         0xA0009001U /* Invalid message custom pool index used; function call is ignored                 */
#define ERR_CVMSG_CUSTOM_MSGPOOL_INVALID            0xA0009002U /* Invalid custom message pool used; function call is ignored                       */
#define ERR_CVMSG_UNABLE_TO_FIND_AVAILABLE_MESSAGE  0xA0009003U /* Message cannot be found to use; function call is ignored                         */
#define ERR_CVMSG_INVALID_TARGET_SYSFLOW_INDEX      0xA0009004U /* Message targets an invalid sysflow_index; function call is ignored               */
#define ERR_CVMSG_MESSAGE_TOO_LARGE                 0xA0009005U /* Message too large; function call is ignored                                      */
/*                                                                                  */
#define ERR_MSGTOKEN_SYSTEM_NOT_SETUP               0xA0009F00U /* Tokenized message system not set up; function call ignored                       */
#define ERR_MSGTOKEN_ALL_TOKENS_IN_USE              0xA0009F01U /* Tokenized message system unable to find token; function call ignored             */
/*                                                                                  */
#define ERR_IDSP_INVALID_FOV                        0xA000E000U /* Invalid FOV sent to IDSP registration; function call is ignored                  */
#define ERR_IDSP_FOV_ALREADY_REGISTERED             0xA000E001U /* Repeated FOV sent to IDSP registration; function call is ignored                 */
#define ERR_IDSP_CAPTURE_TIME_NOT_POSTED            0xA000E002U /* IDSP capture time not received; using current time instead                       */
/*                                                                                  */
#define ERR_RADAR_INVALID_BOARD_ID                  0xA000E100U /* Invalid board_id sent; function call is ignored                                  */
#define ERR_RADAR_INVALID_MMIC_ID                   0xA000E101U /* Invalid mmic_id sent; function call is ignored                                   */
#define ERR_RADAR_INVALID_VIN_ID                    0xA000E102U /* Invalid vin_id sent; function call is ignored                                    */
#define ERR_RADAR_BOARD_ALREADY_REGISTERED          0xA000E103U /* Repeated board registration made; function call is ignored                       */
#define ERR_RADAR_VIN_ID_ALREADY_REGISTERED         0xA000E104U /* Repeated VIN channel registration made; function call is ignored                 */
/*                                                                                  */
#define ERR_MEMPOOL_INVALID_INSTANCE                0xA000FFF0U /* mempool support (deprecated) error - function returns no valid pool              */
#define ERR_CVFLOW_API_RETCODE_END                  0xA00FFFFFU /* ^^^^ CVFLOW API ERROR BLOCK ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ */

/*-= Specific for AST =-----------------------------------------------------*/
#define CVSCH_APIERR_AST_INVALID_GLOBALADDR         0xAA570000U /* Invalid global_addr                                                              */
#define CVSCH_APIERR_AST_INVALID_LOCALADDR          0xAA570001U /* Invalid local_addr                                                               */

/*-= Specific for visorc_crypto =-------------------------------------------*/
#define ERR_INVALID_UNIQUE_ID                       0xAF0B0000U /* Crypto specific API error - crypto failed                                        */
#define ERR_EXPIRED_UNIQUE_ID                       0xAF0B0001U /* Crypto specific API error - crypto failed                                        */

/*-= Cavalry/API specific errors =------------------------------------------------------*/
/* These errors are limited to the 0xAF0C.0000 - 0xAF0C.FFFF range                      */
/*--------------------------------------------------------------------------------------*/
#define ERR_CAVALRY_API_RETCODE_BASE                0xAF0C0000U /* vvvv CAVALRY API ERROR BLOCK vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv */
#define ERR_CAVALRY_UNAVAILABLE                     0xAF0C0000U /* Cavalry system is not available; command ignored                                 */
#define ERR_CAVALRY_VP_UNAVAILABLE                  0xAF0C0100U /* Cavalry/VP not available; command ignored                                        */
#define ERR_CAVALRY_HOTLINK_INVALID_SLOT            0xAF0C0201U /* Cavalry Hotlink Slot-ID is invalid; command ignored                              */
#define ERR_CAVALRY_HOTLINK_INVALID_FUNCTION        0xAF0C0202U /* Cavalry Hotlink function invalid; command ignored                                */
#define ERR_CAVALRY_FEX_UNAVAILABLE                 0xAF0C0300U /* Cavalry/FEX not available; command ignored                                       */
#define ERR_CAVALRY_FMA_UNAVAILABLE                 0xAF0C0400U /* Cavalry/FMA not available; command ignored                                       */
#define ERR_CAVALRY_API_RETCODE_END                 0xAF0CFFFFU /* ^^^^ CAVALRY API ERROR BLOCK ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ */

/*-= FlexiDAG/API specific errors =-----------------------------------------------------*/
/* These errors are limited to the 0xAF0F.0000 - 0xAF0F.FFFF range                      */
/*--------------------------------------------------------------------------------------*/
#define ERR_FLEXIDAG_API_RETCODE_BASE               0xAF0F0000U /* vvvv FLEXIDAG API ERROR BLOCK vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv */
#define ERR_FLEXIDAG_UNAVAILABLE                    0xAF0F0000U /* Flexidag is currently unavailable in this system.  Command ignored.              */
#define ERR_FLEXIDAG_INVALID_VPHANDLE               0xAF0F0001U /* Invalid vpHandle provided to the flexidag_* api call.  Command ignored.          */
#define ERR_FLEXIDAG_INVALID_SLOT_ID                0xAF0F0002U /* Invalid flexidag_slot_id.  Command ignored.                                      */
#define ERR_FLEXIDAG_INVALID_TOKEN_ID               0xAF0F0003U /* Invalid token id.  Command ignored.                                              */
#define ERR_FLEXIDAG_SYSTEM_ALREADY_STARTED         0xAF0F0010U /* Flexidag already started.  Command ignored.                                      */
#define ERR_FLEXIDAG_SYSTEM_NOT_STARTED             0xAF0F0011U /* Flexidag not started.  Command ignored.                                          */
#define ERR_FLEXIDAG_NO_AVAILABLE_SLOTS             0xAF0F0100U /* Flexidag slot could not be found by flexidag_create().  Command ignored.         */
#define ERR_FLEXIDAG_NO_AVAILABLE_TOKENS            0xAF0F0101U /* token could not be found.  Command ignored.                                      */
#define ERR_FLEXIDAG_SLOT_IN_ERROR_STATE            0xAF0F0200U /* Flexidag slot is in error state.  Command ignored.                               */
#define ERR_FLEXIDAG_TOKEN_IN_ERROR_STATE           0xAF0F0201U /* Token is in error state.  Command ignored.                                       */
#define ERR_FLEXIDAG_VPHANDLE_UNEXPECT              0xAF0F0202U /* VpHandle is not expect.  Command ignored.                                        */
#define ERR_FLEXIDAG_UNSUPPORTED_CVFUNCTION         0xAF0F0300U /* Function call is not supported by FlexiDAG.  Command ignored.                    */
#define ERR_FLEXIDAG_UNSUPPORTED_VISORC_CRCCHECK    0xAF0F0301U /* visorc binary doesn't append crc header.                                         */
#define ERR_FLEXIDAG_INVALID_OUTPUT_NUM             0xAF0F0400U /* Flexidag API provided an invalid output number.  Command ignored.                */
#define ERR_FLEXIDAG_CANNOT_OPEN_INVALID_STATE      0xAF0F1000U /* flexidag_open requested on a slot that is in the wrong state; command ignored    */
#define ERR_FLEXIDAG_CANNOT_INIT_INVALID_STATE      0xAF0F1001U /* flexidag_init requested on a slot that is in the wrong state; command ignored.   */
#define ERR_FLEXIDAG_CANNOT_RUN_INVALID_STATE       0xAF0F1002U /* flexidag_run requested on a slot that is in the wrong state; command ignored.    */
#define ERR_FLEXIDAG_IGNORE_RUN_WHILE_CLOSING       0xAF0F1003U /* flexidag_run requested on a slot that is being closed; command ignored.          */
#define ERR_FLEXIDAG_COMMAND_INVALID_STATE          0xAF0F10FFU /* Command sent during state that cannot accept it.  Command ignored.               */
#define ERR_FLEXIDAG_CANNOT_ADD_SFB                 0xAF0F2000U /* Cannot add SFB to FlexiDAG.  Command ignored.                                    */
#define ERR_FLEXIDAG_CANNOT_ADD_TBAR                0xAF0F2001U /* Cannot add TBAR to FlexiDAG.  Command ignored.                                   */
#define ERR_FLEXIDAG_API_RETCODE_END                0xAF0FFFFFU /* ^^^^ FLEXIDAG API ERROR BLOCK ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ */

/*------------------------------------------------------------------------------------------------*/
/* CVTask errors                                                                                  */
/* 0xC000.0000 - 0xC7FF.FFFF : User defined, Class-C1 (sys-recoverable) CVTask errors.            */
/* 0xC800.0000 - 0xCEFE.FFFF : User defined, Class-C2 (sys-recoverable) CVTask errors.            */
/* 0xCEFF.0000 - 0xCEFF.FFFF : Scheduler defined, Class-C2 (sys-recoverable) CVTask errors.       */
/* 0xCF00.0000 - 0xCF7F.FFFF : User defined, Class-C3 (sys-irrecoverable) CVTask errors.          */
/* 0xCF80.0000 - 0xCFFE.FFFF : User defined, Class-C4 (sys-irrecoverable) CVTask errors.          */
/* 0xCFFF.0000 - 0xCFFF.FFFF : Scheduler defined, Class-C4 (sys-irrecoverable) CVTask errors.     */
/*                                                                                                */
/* All errors caused by a FlexiDAG CVTask shall shut down the associated FlexiDAG slot.           */
/* All errors caused by a SuperDAG CVTask shall shut down the SuperDAG and scheduler.             */
/* All errors caused by a AutoRUN CVTask shall shut down the system if class C3/C4.               */
/* All errors are reported to the Vision Driver upon detection                                    */
/*------------------------------------------------------------------------------------------------*/
#define ERRCODE_CVTASK_BASE                         0xC0000000U
#define ERRCODE_CVTASK_END                          0xCFFFFFFFU

#define ERR_CVTASK_USER_C1_BASE                     0xC0000000U
#define ERR_CVTASK_USER_C1_END                      0xC7FFFFFFU
#define ERR_CVTASK_USER_C2_BASE                     0xC8000000U
#define ERR_CVTASK_USER_C2_END                      0xCEFEFFFFU

#define ERR_CVTASK_SCHED_C2_BASE                    0xCEFF0000U
#define ERR_CVTASK_DEADLINE_BEFORE_RUN              0xCEFF0001U /* cvtask passed deadline_fail before run.  CVTask is skipped   */
#define ERR_CVTASK_DEADLINE_DURING_RUN              0xCEFF0002U /* cvtask passed deadline_fail during run.  CVTask completed    */
#define ERR_CVTASK_PROC_TIMEOUT_DURING_RUN          0xCEFF0003U /* cvtask_proc_time has expired during run.  CVTask completed   */
#define ERR_CVTASK_FD_SECRET_MISMATCH               0xCEFFC000U /* Mismatched identification for flexidag */
#define ERR_CVTASK_SCHED_C2_END                     0xCEFFFFFFU

#define ERR_CVTASK_USER_C3_BASE                     0xCF000000U
#define ERR_CVTASK_USER_C3_END                      0xCF7FFFFFU
#define ERR_CVTASK_USER_C4_BASE                     0xCF800000U
#define ERR_CVTASK_USER_C4_END                      0xCFFEFFFFU

#define ERR_CVTASK_SCHED_C4_BASE                    0xCFFF0000U
#define ERR_CVTASK_SIST_FAILED                      0xCFFFA000U /* CVTask SIST failed - reserve 0xCFFA.0000 - 0xCFFA.FFFF for this class */
#define ERR_CVTASK_SCHED_C4_END                     0xCFFFFFFFU

/*------------------------------------------------------------------------------------------------*/
/* SysFlow Fatal errors: 0xF000.0000 - 0xFEFF.FFFF                                                */
/*                                                                                                */
/* Any errors in this category will have the following effect on the SysFlow that it affects:     */
/*   SuperDAG mode : This will cause a graceful shutdown of the whole VISORC scheduler            */
/*   FlexiDAG mode : This will cause the affected flexidag slot to transition into an ERROR state */
/*------------------------------------------------------------------------------------------------*/
#define ERR_SYSFLOW_FATAL_BASE                      0xF0000000U

/*-= Tasklist specific errors =-----------------------------------------------*/
#define ERR_TASKLIST_HAS_NONUNIQUE_NAME             0xF0001000U /* VISORC Tasklist has multiple copies of the same task                             */
#define ERR_ARM_TASKLIST_HAS_NONUNIQUE_NAME         0xF0001001U /* ARM Tasklist has multiple copies of the same task                                */
#define ERR_TASKLIST_HAS_INVALID_ORCANY             0xF0001002U /* Invalid ORCANY task compiled into SysFlow                                        */
#define ERR_CVTASK_TOO_MANY_LOADED                  0xF0001003U /* Too many CVTasks loaded into the SysFlow                                         */
#define ERR_INVALID_CVTASK_ON_VISORC                0xF0001004U /* Invalid CVTask attached to VISORC                                                */

/*-= CVTask registration phase errors =---------------------------------------*/
#define ERR_CVTASK_HAS_NO_QUERY_FUNCTION            0xF0002000U /* CVTask has no query function associated with it                                  */
#define ERR_CVTASK_HAS_NO_INIT_FUNCTION             0xF0002001U /* CVTask has no init function associated with it                                   */
#define ERR_CVTASK_HAS_NO_GET_INFO_FUNCTION         0xF0002002U /* CVTask has no get_info function associated with it                               */
#define ERR_CVTASK_HAS_NO_MESSAGE_FUNCTION          0xF0002003U /* CVTask has no process_messages function associated with it                       */
#define ERR_CVTASK_HAS_NO_RUN_FUNCTION              0xF0002004U /* CVTask has no run function associated with it                                    */
#define ERR_CVTASK_MISMATCHED_CVAPI                 0xF0002005U /* CVTask has a mismatched API compiled with it                                     */
#define ERR_CVTASK_MISMATCHED_CVCHIP                0xF0002006U /* CVTask has a mismatched CVCHIP compiled with it                                  */
#define ERR_CVTASK_MISMATCHED_AMALGAM_CVTASK        0xF0002007U /* CVTask compiled for AMALGAM mode compiled with HARDWARE                          */

/*-= Query phase errors =-----------------------------------------------------*/
#define ERR_CVTASK_INSUFFICIENT_INPUTS              0xF0003000U /* Sysflow doesn't link enough inputs for this particular cvtask                    */
#define ERR_CVTASK_INSUFFICIENT_FEEDBACKS           0xF0003001U /* Sysflow doesn't link enough feedbacks for this particular cvtask                 */
#define ERR_CVTASK_NONUNIQUE_OUTPUT_IONAME          0xF0003002U /* ioname on outputs are non-unique                                                 */
#define ERR_CVTASK_INPUT_REQUESTS_TOO_MUCH_HISTORY  0xF0003003U /* CVTask requests too much input history                                           */
#define ERR_CVTASK_FDBCK_REQUESTS_TOO_MUCH_HISTORY  0xF0003004U /* CVTask requests too much feedback history                                        */
#define ERR_ARM_CVTASK_CANNOT_REQUEST_CMEM          0xF0003010U /* ARM specific : ARM cannot request CMEM; cvtask is marked as invalid              */
#define ERR_ARM_CVTASK_CANNOT_REQUEST_UNCACHED      0xF0003011U /* ARM specific : ARM cannot request uncached memory; cvtask is marked as invalid   */
#define ERR_CVTASK_REQUIRES_TOO_MUCH_CMEM           0xF0003020U /* VISORC CVTask requests too much CMEM                                             */
#define ERR_CVTASK_REQUIRES_UNAVAILABLE_UNIT        0xF0003021U /* CVTask being run needs an unavailable hardware unit                              */

/*-= sysflow_validation errors =----------------------------------------------*/
#define ERR_SYSFLOW_TOO_MANY_ENTRIES                0xF0004000U /* Too many sysflow entries are loaded                                              */
#define ERR_SYSFLOW_NONUNIQUE_ID                    0xF0004001U /* Sysflow table has a non-unique UUID                                              */
#define ERR_SYSFLOW_NONUNIQUE_NAME                  0xF0004002U /* Sysflow table has a non-unique name                                              */
#define ERR_SYSFLOW_CANNOT_FIND_CVTASK              0xF0004003U /* Sysflow table requests a task that isn't available                               */
#define ERR_SYSFLOW_INVALID_ENGINE_NAME             0xF0004004U /* Engine name provided for cvtask is invalid                                       */
#define ERR_SYSFLOW_CANNOT_ASSIGN_ARM_CVTASK        0xF0004005U /* Cannot assign cvtask to ARM                                                      */

/*-= sysflow_link errors =----------------------------------------------------*/
#define ERR_SYSFLOWLINK_INPUT_UUID                  0xF0005000U /* Cannot link output->input via UUID                                               */
#define ERR_SYSFLOWLINK_INPUT_IONAME                0xF0005001U /* Cannot link output->input via ioname                                             */
#define ERR_SYSFLOWLINK_INPUT_DIRECT                0xF0005002U /* Cannot link output->input via direct attach                                      */

#define ERR_SYSFLOWLINK_FEEDBACK_UUID               0xF0005010U /* Cannot link output->feedback via UUID                                            */
#define ERR_SYSFLOWLINK_FEEDBACK_IONAME             0xF0005011U /* Cannot link output->feedback via ioname                                          */
#define ERR_SYSFLOWLINK_FEEDBACK_DIRECT             0xF0005012U /* Cannot link output->feedback via direct attach                                   */

/*-= sysflow_build_dependencies errors =--------------------------------------*/
#define ERR_DEPENDENCY_AT_MAX_FANOUT                0xF0005100U /* Too many dependencies for cvtask                                                 */
#define ERR_DEPENDENCY_BAD_LINK_INDEX               0xF0005101U /* Bad link found in dependency table                                               */
#define ERR_DEPENDENCY_BAD_LINK_OUTPUT_INDEX        0xF0005102U /* Bad link found in dependency table                                               */

/*-= cvtable errors =---------------------------------------------------------*/
#define ERR_CVTABLE_UNSUPPORTED_VERSION             0xF0006000U /* Unsupported CVTable version                                                      */
#define ERR_CVTABLE_TOO_MANY_SETS_LOADED            0xF0006001U /* Too many CVTable Sets loaded                                                     */
#define ERR_CVTABLE_INVALID_SET_LOADED              0xF0006002U /* Invalid CVTable loaded                                                           */

/*-= sysflow_lists errors =---------------------------------------------------*/
#define ERR_SYSFLOW_TOO_MANY_FEEDER_CVTASKS         0xF000B000U /* Too many feeder tasks in this sysflow                                            */
#define ERR_SYSFLOW_TOO_MANY_INDEPENDENT_CVTASKS    0xF000B001U /* Too many independent tasks in this sysflow                                       */
#define ERR_SYSFLOW_TOO_MANY_CRITICAL_CVTASKS       0xF000B002U /* Too many critical tasks in this sysflow                                          */
#define ERR_SYSFLOW_TOO_MANY_ERROR_HANDLER_CVTASKS  0xF000B003U /* Too many error handler cvtasks in this sysflow                                   */

/*-= Membuf runtime specific - SysFlow may be running slow =------------------*/
#define ERR_INPUT_MEMBUF_BEING_WRITTEN              0xF000C000U /* Input buffer still being written                                                 */
#define ERR_FEEDBACK_MEMBUF_BEING_WRITTEN           0xF000C010U /* Feedback buffer still being written                                              */
#define ERR_OUTPUT_MEMBUF_BEING_WRITTEN             0xF000C020U /* Output buffer still being written                                                */
#define ERR_OUTPUT_MEMBUF_STILL_IN_USE              0xF000C021U /* Output buffer still in use                                                       */
#define ERR_INPUT_OVERWRITTEN_BEFORE_PROCESSING     0xF000C100U /* Input buffer overwritten before processing; SysFlow is too slow                  */
#define ERR_INPUT_OVERWRITTEN_WHILE_PROCESSING      0xF000C101U /* Input buffer overwritten while processing; SysFlow is too slow                   */
#define ERR_FEEDBACK_OVERWRITTEN_BEFORE_PROCESSING  0xF000C110U /* Feedback buffer overwritten before processing; SysFlow is too slow               */
#define ERR_FEEDBACK_OVERWRITTEN_WHILE_PROCESSING   0xF000C111U /* Feedback buffer overwritten while processing; SysFlow is too slow                */

/*-= CVTaskNode non-fatal errors =--------------------------------------------*/
#define ERR_CVTASK_TOO_FAR_BEHIND                   0xF000C200U /* CVTask is too far behind; SysFlow is too slow                                    */
#define ERR_INVALID_CVTASK_OUTPUT_INDEX             0xF000C201U /* Invalid output index used; SysFlow may be corrupted                              */

/*-= Message system has an error =--------------------------------------------*/
#define ERR_CVMSG_CANNOT_ATTACH_TOO_MANY_MESSAGES   0xF000D000U /* Unable to attach a message                                                       */
#define ERR_CVMSG_CANNOT_ATTACH_TO_PAST_FRAME       0xF000D001U /* Unable to attach to a past frame                                                 */
#define ERR_CVMSG_CANNOT_ATTACH_TOO_FAR_AHEAD       0xF000D002U /* Unable to attach to a frame too far in the future                                */
#define ERR_CVMSG_TOO_MANY_PENDING_TIMED_MESSAGES   0xF000D003U /* Unable to attach a timed message                                                 */

/*-= System frequency system has an error =-----------------------------------*/
#define ERR_CVFREQ_TOO_MANY_PENDING_TIMED_MESSAGES  0xF000D400U /* Unable to attach a timed frequency message                                       */

/*-= SysFlow allocation failures =--------------------------------------------*/
#define ERR_ALLOCFAIL_DRAM_SCRATCHPAD               0xF000F000U /* Failed to allocate scratchpad (arg1 = size, arg2 = CVCORE)                       */
#define ERR_ALLOCFAIL_ARM_DRAM_SCRATCHPAD           0xF000F001U /* Failed to allocate ARM scratchpad                                                */

/*-= CVTask SysFlow Fatal errors =--------------------------------------------*/
#define ERR_CVTASK_SYSFLOW_FATAL_BASE               0xFC000000U
#define ERR_CVTASK_SYSFLOW_FATAL_END                0xFCFFFFFFU

/*--------------------------------------------------------------------------------------------*/
/* FlexiDAG interface specific errors : 0xFE00.0000 - 0xFEFF.FFFF                             */
/*                                                                                            */
/* All of these errors are considered fatal for the specific FlexiDAG, but should not shut    */
/* down the entire system.  These errors are also specific for FlexiDAG only.  These errors   */
/* will auto-transition the slot into an error state                                          */
/*--------------------------------------------------------------------------------------------*/

#define ERR_FLEXIDAG_FATAL_ERROR_BASE               0xFE000000U /* vvvv FLEXIDAG FATAL ERROR BLOCK vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv */
/*                                                                                  */
#define ERR_FLEXIDAG_NO_FLEXIBIN                    0xFE001000U /* Flexidag has no FlexiBin associated with it                                      */
#define ERR_FLEXIBIN_INVALID_HEADER                 0xFE001001U /* Flexibin has an invalid header                                                   */
#define ERR_FLEXIBIN_VERSION_MISMATCH               0xFE001002U /* Flexibin has a mismatched version                                                */
#define ERR_FLEXIBIN_CVCHIP_MISMATCH                0xFE001003U /* Flexibin has a mismatched cvchip type                                            */
#define ERR_FLEXIBIN_UNKNOWN_COMPONENT              0xFE001004U /* Flexibin has an unknown component                                                */
#define ERR_FLEXIBIN_TOO_MANY_CVTASKS               0xFE001005U /* Flexibin has too many VISORC cvtasks (max defined FLEXIDAG_MAX_CVTASK_ENTRIES)   */
#define ERR_FLEXIBIN_INVALID_VISORC_ENTRY           0xFE001006U /* Flexibin has an invalid visorc code or patch block                               */
#define ERR_FLEXIBIN_REPEATED_VISORC_ENTRY          0xFE001007U /* Flexibin has an multiple repeated visorc code or patch block                     */
/*                                                                                  */
#define ERR_FLEXIBIN_INVALID_SFB_ENTRY              0xFE002000U /* Flexibin has an invalid SFB attached                                             */
#define ERR_FLEXIBIN_TOO_MANY_SFB_ATTACHED          0xFE002001U /* Flexibin has too many SFBs attached (max defined by FLEXIDAG_MAX_SFB)            */
#define ERR_FLEXIDAG_INVALID_FLEXICFG_ENTRY         0xFE002002U /* Sysflow table in FlexiBin contains an invalid FlexiCFG entry                     */
#define ERR_FLEXIDAG_INVALID_SYSFLOW_ENTRY          0xFE002003U /* Invalid FlexiCFG name                                                            */
#define ERR_FLEXIDAG_INVALID_VISORC_INVALID_CRC     0xFE002004U /* Invalid visorc binary CRC check compared with pre-genetated CRC                  */
/*                                                                                  */
#define ERR_FLEXIBIN_INVALID_TBAR_ENTRY             0xFE003000U /* Flexibin has an invalid TBAR attached                                            */
#define ERR_FLEXIBIN_TOO_MANY_TBAR_ATTACHED         0xFE003001U /* Flexibin has too many TBARs attached (max defined by FLEXIDAG_MAX_TBAR)          */
#define ERR_FLEXIBIN_TOO_MANY_CVTABLE_ENTRIES       0xFE003002U /* Flexibin has too many TBAR entries (max defined by FLEXIDAG_MAX_TBAR_ENTRIES)    */
/*                                                                                  */
#define ERR_FLEXIDAG_TOO_MANY_MEMBUFSETS            0xFE004000U /* Flexidag requires too many membufsets (too many buffers)                         */
/*                                                                                  */
#define ERR_FLEXIDAG_INVALID_STATE_BUFFER           0xFE008000U /* Invalid state buffer provided to _init or _run                                   */
#define ERR_FLEXIDAG_STATEBUF_MUST_NOT_MOVE         0xFE008001U /* Flexidag state buffer must remain constant                                       */
#define ERR_FLEXIDAG_INVALID_TEMP_BUFFER            0xFE008010U /* Invalid temp buffer provided to _init or _run                                    */
#define ERR_FLEXIDAG_INVALID_INPUT_BUFFER           0xFE008020U /* Invalid input buffer provided to _run                                            */
#define ERR_FLEXIDAG_INVALID_FEEDBACK_BUFFER        0xFE008030U /* Invalid feedback buffer provided to _run                                         */
#define ERR_FLEXIDAG_INVALID_OUTPUT_BUFFER          0xFE008040U /* Invalid output buffer provided to _run                                           */
#define ERR_FLEXIDAG_PRERUN_NOT_RXED                0xFE0080A0U /* (ASIL_COMPLIANCE) : Prerun message not recieved (_run only)                      */
#define ERR_FLEXIDAG_PRERUN_TOKEN_MISMATCH          0xFE0080A1U /* (ASIL_COMPLIANCE) : Prerun message token mismatched (_run only)                  */
#define ERR_FLEXIDAG_MEMBUF_HEADER_ERROR            0xFE0080A2U /* (ASIL_COMPLIANCE) : Membuf creation header verification failed (_run or _init)   */
#define ERR_FLEXIDAG_MEMBUF_CSUM_ERROR              0xFE0080A3U /* (ASIL_COMPLIANCE) : Membuf checksum verification failed (_run only)              */
/*                                                                                  */
#define ERR_FLEXIDAG_INVALID_SYSFLOW_INDEX          0xFE008100U /* Invalid sysflow index used internally for _run                                   */
#define ERR_FLEXIDAG_INVALID_RUN_INDEX              0xFE008101U /* Invalid run index used internally for _run                                       */
/*                                                                                  */
#define ERR_FLEXIDAG_CREATE_TIMED_OUT               0xFE009000U /* Flexidag close requested, but wasn't able to complete in expected time           */
#define ERR_FLEXIDAG_OPEN_TIMED_OUT                 0xFE009010U /* Flexidag open requested, but wasn't able to complete in expected time            */
#define ERR_FLEXIDAG_INIT_TIMED_OUT                 0xFE009020U /* Flexidag init requested, but wasn't able to complete in expected time            */
#define ERR_FLEXIDAG_RUN_TIMED_OUT                  0xFE009080U /* Flexidag run requested, but wasn't able to complete in expected time             */
#define ERR_FLEXIDAG_CLOSE_TIMED_OUT                0xFE0090F0U /* Flexidag close requested, but wasn't able to complete in expected time           */
/*                                                                                  */
#define ERR_FLEXIDAG_FATAL_ERROR_END                0xFE00FFFFU /* ^^^^ FLEXIDAG FATAL ERROR BLOCK ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ */

/*------------------------------------------------------------------------------------------------*/
/* System Fatal errors: 0xFF00.0000 - 0xFFFE.FFFF                                                 */
/*                                                                                                */
/* These errors will cause the VISORC scheduler to completely exit.  These errors usually imply   */
/* a system-catastrophic error (i.e. something has corrupted the scheduler's internal state), and */
/* will start the shutdown process for the system.                                                */
/*------------------------------------------------------------------------------------------------*/
#define ERR_SYSTEM_FATAL_BASE                       0xFF000000U

/*-= Generic fatal errors - implies system is corrupted =-------------------------------*/
#define ERR_INVALID_VISCORE                         0xFF000000U

/*-= Configuration specific - happens during bootup =-----------------------------------*/

/*-= Internal function specific- happens during bootup =--------------------------------*/
#define ERR_UNKNOWN_INTERNAL_FUNCTION               0xFF001000U /* System has an unknown/unregistered internal function                             */
#define ERR_REPEATED_INTERNAL_FUNCTION              0xFF001001U /* System has an unknown/unregistered internal function                             */

#define ERR_ORC_STARTUP_SEQUENCE_CORRUPTED          0xFF002000U /* Startup sequence for ORC corrupted                                               */
#define ERR_ARM_STARTUP_SEQUENCE_CORRUPTED          0xFF002001U /* Startup sequence for ARM corrupted                                               */
#define ERR_INVALID_CVTASK_INDEX                    0xFF003000U /* Invalid cvtask index                                                             */

/*-= CVTasknode runtime specific - system may be corrupted beyond recovery =------------*/
#define ERR_CVTASKNODES_EXHAUSTED                   0xFF100000U /* CVTaskNode pool is exhausted                                                     */
#define ERR_CVTASKLIST_CORRUPTED                    0xFF100001U /* CVTaskNode list is corrupted                                                     */
#define ERR_CVTASKNODE_INVALID                      0xFF100002U /* CVTaskNode is invalid                                                            */
#define ERR_CVTASKNODE_CORRUPTED                    0xFF100003U /* CVTaskNode header is corrupted                                                   */
#define ERR_CVTASKNODE_INVALID_CVTASK_MSG           0xFF100004U /* CVTaskNode has an invalid message attached                                       */
#define ERR_CVTASKNODE_INVALID_CVTASK_TYPE          0xFF100005U /* CVTaskNode has an invalid cvtask_type                                            */
#define ERR_CVTASKNODE_INVALID_RUNMODE              0xFF100006U /* CVTaskNode has an invalid run mode                                               */
#define ERR_CVTASKNODE_LINK_BAD_INDEX               0xFF100007U /* CVTaskNode has an invalid link index                                             */
#define ERR_CVTASKNODE_LINK_BAD_OUTPUT_INDEX        0xFF100008U /* CVTaskNode has an invalid output index                                           */
#define ERR_CVTASKNODE_INVALID_PROC_MSG             0xFF100009U /* CVTaskNode has an invalid run function                                           */
#define ERR_CVTASKNODE_INVALID_RUN                  0xFF100010U /* CVTaskNode has an invalid run function                                           */
#define ERR_CVTASKNODE_TOO_MANY_INPUTS              0xFF100011U /* CVTaskNode has an invalid number of inputs                                       */
#define ERR_CVTASKNODE_TOO_MANY_FEEDBACKS           0xFF100012U /* CVTaskNode has an invalid number of feedbacks                                    */
#define ERR_CVTASKNODE_TOO_MANY_OUTPUTS             0xFF100013U /* CVTaskNode has an invalid number of outputs                                      */
#define ERR_CVTASKNODE_INVALID_MEMBUF               0xFF100014U /* CVTaskNode has an invalid membuf                                                 */
#define ERR_CVTASKNODE_LARGE_FRAMEJUMP              0xFF100015U /* CVTaskNode has a large framejump detected                                        */
#define ERR_CVTASKNODE_WAS_NOT_SCHEDULED            0xFF100016U /* CVTaskNode was run but wasn't properly scheduled                                 */
#define ERR_CVTASKNODE_FINISHED_SAME_FRAME          0xFF100017U /* CVTaskNode finished the same frame multiple times                                */
#define ERR_CVTASKNODE_FINISHED_SKIPPED_FRAME       0xFF100018U /* CVTaskNode finished a skipped frame                                              */

#define ERR_CVTASKLIST_BAD_ARM_TASKNODE             0xFF100040U /* CVTaskNode returned from arm is an invalid tasknode                              */
#define ERR_CVTASKPOOL_INVALID_NODEARRAY            0xFF100041U /* CVTaskPool has an invalid node array                                             */
#define ERR_CVTASKPOOL_INVALID_POOL_ID              0xFF100042U /* CVTaskPool has an invalid pool_id                                                */
#define ERR_CVTASKPOOL_INVALID_NODE_ID              0xFF100043U /* CVTaskPool has an invalid node_id                                                */
#define ERR_CVTASKPOOL_INVALID_POOL_SIZE            0xFF100044U /* CVTaskPool has an invalid pool_size                                              */

#define ERR_AUTORUN_SYSTEM_CORRUPTED                0xFF100100U /* Autorun system has been corrupted                                                */
#define ERR_AUTORUN_TOO_MANY_CVTASKS                0xFF100101U /* Autorun system has too many autorun tasks allocated                              */

/*-= Allocation errors - note these will be expanded and fixed upon in next comment =---*/
#define ERR_ALLOCFAIL_GLOBALS                       0xFFA00000U /* Unable to allocate globals */
#define ERR_ALLOCFAIL_MESSAGE_QUEUE                 0xFFA00001U /* Unable to allocate message queue */
#define ERR_ALLOCFAIL_CVTASKLIST                    0xFFA00002U /* Unable to allocate cvtasklist */
#define ERR_ALLOCFAIL_SCHEDULER_STATE_TABLE         0xFFA00003U /* Unable to allocate scheduler state table */
#define ERR_ALLOCFAIL_MEMBUFSET_TABLE               0xFFA00004U /* Unable to allocate membufset table */
#define ERR_ALLOCFAIL_SYSFLOW_NAME_TABLE            0xFFA00005U /* Unable to allocate sysflow name table */
#define ERR_ALLOCFAIL_TASK_CONTEXT_TABLE            0xFFA00006U /* Unable to allocate task context table */
#define ERR_ALLOCFAIL_AUTORUN_STATE_TABLE           0xFFA00007U /* Unable to allocate autorun state table */

#define ERR_ALLOCFAIL_PREPARTITION                  0xFFA00100U /* Failed to allocate in prepartition state */
#define ERR_ALLOCFAIL_CACHED                        0xFFA00101U /* Failed to allocate in cached area */
#define ERR_ALLOCFAIL_UNCACHED                      0xFFA00102U /* Failed to allocate in uncached area */
#define ERR_ALLOCFAIL_ARMBLOCK                      0xFFA00103U /* Failed to allocate in armblock area */
#define ERR_ALLOCFAIL_BAD_LOCATION                  0xFFA00104U /* Failed to allocate in bad location */

#define ERR_MEMORY_REPARTITION_FAILED               0xFFA00200U /* Failed to repartition memory */

#define ERR_ALLOCFAIL_GUARDBAND_CAP_CACHED          0xFFA00300U
#define ERR_ALLOCFAIL_GUARDBAND_CAP_UNCACHED        0xFFA00301U
#define ERR_ALLOCFAIL_GUARDBAND_CAP_ARMBLOCK        0xFFA00302U

#define ERR_ALLOCFAIL_MEMBUFSET_CACHED              0xFFA00400U
#define ERR_ALLOCFAIL_MEMBUFSET_UNCACHED            0xFFA00401U
#define ERR_ALLOCFAIL_MEMBUFSET_ARMBLOCK            0xFFA00402U
#define ERR_ALLOCFAIL_MEMORY_BLOCK_CACHED           0xFFA00403U
#define ERR_ALLOCFAIL_MEMORY_BLOCK_UNCACHED         0xFFA00404U
#define ERR_ALLOCFAIL_MEMORY_BLOCK_ARMBLOCK         0xFFA00405U
#define ERR_ALLOCFAIL_MEMPOOL_HEADER_CACHED         0xFFA00406U
#define ERR_ALLOCFAIL_MEMPOOL_HEADER_UNCACHED       0xFFA00407U
#define ERR_ALLOCFAIL_MEMPOOL_HEADER_ARMBLOCK       0xFFA00408U
#define ERR_ALLOCFAIL_MEMPOOL_BLOCK_CACHED          0xFFA00409U
#define ERR_ALLOCFAIL_MEMPOOL_BLOCK_UNCACHED        0xFFA0040AU
#define ERR_ALLOCFAIL_MEMPOOL_BLOCK_ARMBLOCK        0xFFA0040BU
#define ERR_ALLOCFAIL_MSGPOOL_ARMBLOCK              0xFFA0040CU

#define ERR_ALLOCFAIL_CVTASK_SHARED_STORAGE         0xFFA0040DU
#define ERR_ALLOCFAIL_CVTASK_PRIVATE_STORAGE        0xFFA0040EU
#define ERR_ALLOCFAIL_CVTASK_UNCACHED_STORAGE       0xFFA0040FU
#define ERR_ALLOCFAIL_CVTASK_OUTPUT_BUFFER          0xFFA00410U
#define ERR_ALLOCFAIL_CVTASK_MESSAGE_BUFFER         0xFFA00411U
#define ERR_MEMBUFSET_REQUIRES_TOO_MANY_BUFFERS     0xFFA00412U

/*-= Scheduler runtime corrupted beyond recovery =--------------------------------------*/
#define ERR_CVTASK_LIST_CORRUPTED                   0xFFB00000U /* CVTask list system corrupted; internal structures may be corrupted               */

#define ERR_CVTASK_SYSTEM_FATAL_BASE                0xFFC00000U
#define ERR_CVTASK_SYSTEM_FATAL_END                 0xFFCFFFFFU

/*-= Hardware thread specific errors; thread possibly hung =----------------------------*/
/* (these should only be sent by the scheduler_check_hw_health())                       */
#define ERR_VISORC_HARDWARE_STALLED                 0xFFDEAD00U /* arg1 : cvcore_id that hung; forces system exit                                   */
#define ERR_VISORC_COREDUMP_REQUEST                 0xFFDEADCCU /* arg1 : cvcore_id that hung; forces system exit after coredump completes          */

/*-= Flexidag system corrupted beyond recovery =----------------------------------------*/
#define ERR_FLEXIDAG_INVALID_NUM_SLOTS              0xFFE10000U /* Flexidag system configured with an invalid number of slots                       */
#define ERR_FLEXIDAG_SYSTEM_CORRUPTED               0xFFE10010U /* Flexidag system is corrupted                                                     */
#define ERR_FLEXIDAG_INVALID_MESSAGE                0xFFE10020U /* Flexidag system received invalid message                                         */

/*-= Memory subsystem corrupted beyond recovery =---------------------------------------*/
#define ERR_DRAM_BUFFER_INVALID                     0xFFEE0000U /* DRAM buffer address is invalid                                                   */
#define ERR_DRAM_BUFFER_CORRUPTED_BEFORE_PAYLOAD    0xFFEE0001U /* DRAM buffer/block corrupted before payload                                       */
#define ERR_DRAM_BUFFER_CORRUPTED_AFTER_PAYLOAD     0xFFEE0002U /* DRAM buffer/block corrupted after payload                                        */
/*                                                                                  */
#define ERR_CMEM_BUFFER_INVALID                     0xFFEE0010U /* CMEM buffer address is invalid                                                   */
#define ERR_CMEM_BUFFER_CORRUPTED_BEFORE_PAYLOAD    0xFFEE0011U /* CMEM buffer/block corrupted before payload                                       */
#define ERR_CMEM_BUFFER_CORRUPTED_AFTER_PAYLOAD     0xFFEE0012U /* CMEM buffer/block corrupted after payload                                        */
/*                                                                                  */
#define ERR_MEMORY_BLOCK_GUARDBAND_CORRUPTED        0xFFEE0020U /* Memory block guardband corrupted                                                 */
/*                                                                                  */
#define ERR_INPUT_MEMBUF_NEVER_USED                 0xFFEE0030U /* Input buffer never used (unexpected state)                                       */
#define ERR_INPUT_MEMBUF_ALREADY_RELEASED           0xFFEE0031U /* Input buffer already released (unexpected state)                                 */
/*                                                                                  */
#define ERR_FEEDBACK_MEMBUF_NEVER_USED              0xFFEE0040U /* Feedback buffer never used (unexpected state)                                    */
#define ERR_FEEDBACK_MEMBUF_ALREADY_RELEASED        0xFFEE0041U /* Feedback buffer already released (unexpected state)                              */
/*                                                                                  */
#define ERR_OUTPUT_MEMBUF_STATE_CORRUPTED           0xFFEE0050U /* Output buffer state corrupted (unexpected state)                                 */

#define ERR_MEMBUFSET_INVALID                       0xFFEE0060U /* Invalid membufset                                                                */

/*-= IDSP errors =----------------------------------------------------------------------*/
#define ERR_IDSP_FRAME_TOO_OLD                      0xFFEE8000U /* IDSP Frame captured is too old; unexpected state - system possibly corrupted     */
#define ERR_IDSP_FRAME_TOO_NEW                      0xFFEE8001U /* IDSP Frame captured is too new; unexpected state - system possibly corrupted     */
#define ERR_IDSP_FRAME_ALREADY_RXED                 0xFFEE8002U /* IDSP Frame received twice; unexpected state - system possibly corrupted          */
#define ERR_IDSP_PARTIAL_STEREO_RXED                0xFFEE8003U /* IDSP Partial stereo recieved; unexpected state - system possibly corrupted       */
#define ERR_IDSP_PARTIAL_FRAMESET_RXED              0xFFEE8004U /* IDSP partial frameset received; unexpected state - system possibly corrupted     */
#define ERR_IDSP_SYNCHRONOUS_CAP_SEQ_NO_JUMP        0xFFEE8005U /* IDSP synchronus capture sequence jump detected; system possibly corrupted        */

/*-= RADAR errors =---------------------------------------------------------------------*/
#define ERR_RADAR_FRAME_TOO_OLD                     0xFFEE8100U /* Radar Frame captured is too old; unexpected state - system possibly corrupted    */
#define ERR_RADAR_FRAME_TOO_NEW                     0xFFEE8101U /* Radar Frame captured is too new; unexpected state - system possibly corrupted    */
#define ERR_RADAR_FRAME_ALREADY_RXED                0xFFEE8102U /* Radar Frame received twice; unexpected state - system possibly corrupted         */
#define ERR_RADAR_PARTIAL_FRAMESET_RXED             0xFFEE8103U /* Radar partial frameset received; unexpected state - system possibly corrupted    */
#define ERR_RADAR_SYNCHRONOUS_CAP_SEQ_NO_JUMP       0xFFEE8104U /* Radar synchronus capture sequence jump detected; system possibly corrupted       */

/*-= Workspace errors - most likely system corrupted, or incorrectly configured =-------*/
#define ERR_FRAMEWORK_CMEM_WORK_INVALID             0xFFEE1000U /* Framework CMEM workspace is invalid                                              */
#define ERR_FRAMEWORK_CMEM_WORK_INSUFFICIENT        0xFFEE1001U /* Framework CMEM workspace size is insufficient                                    */
#define ERR_SCHEDULER_WORKSPACE_INVALID             0xFFEE1010U /* Scheduler CMEM workspace is invalid                                              */
#define ERR_SCHEDULER_WORKSPACE_INSUFFICIENT        0xFFEE1011U /* Scheduler CMEM workspace size is insufficient                                    */

/*-= System resources corrupted beyond recovery =---------------------------------------*/
#define ERR_VISORC_MSGNODES_EXHAUSTED               0xFFFE0000U /* Internal system error; no ERROR_PRINTF is generated for this error, as error_printf uses the msgnode system */
#define ERR_ARMSTATE_CORRUPTED                      0xFFFE0001U /* ARM state is corrupted; no ERROR_PRINTF is generated for this, as error_printf uses the arm interface */

/*------------------------------------------------------------------------------------------------*/
/* System reserved errors: 0xFFFF.0000 - 0xFFFF.FFFF                                              */
/*                                                                                                */
/* These errors are considered reserved, as they will also map to negated linux error codes       */
/*------------------------------------------------------------------------------------------------*/


#endif /* !CVSCHED_ERRNO_H_FILE */

