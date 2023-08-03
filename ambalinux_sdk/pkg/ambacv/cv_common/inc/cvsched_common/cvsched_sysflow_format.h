/*
 * Copyright (c) 2017-2020 Ambarella, Inc.
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

#ifndef CVSCHED_SYSFLOW_FORMAT_H_FILE
#define CVSCHED_SYSFLOW_FORMAT_H_FILE

/*-= CVScheduler API file =-----------------------------------------------------------------------*/
#ifdef ORCVIS
#include <dsp_types.h>
#endif

#include <cvtask_api.h>
#include <cvtask_coredef.h>

/*================================================================================================================*/
/* Sysflow File Format                                                                                            */
/*                                                                                                                */
/* Sysflow file binaries (SFB) is a collection of "sysflow_entry_t" structures.                                   */
/*                                                                                                                */
/* Current format (SYSFLOW_FILE_VERSION) requires several of the defines in cvtask_api.h and cvtask_coredef.h to  */
/* be as follows, to maintain the 384-byte structure size                                                         */
/*                                                                                                                */
/* SYSFLOW_FILE_VERSION         1U                                                                                */
/* cvtask_api.h     : SYSFLOW_INVALID_INDEX       0xFFFFU                                                         */
/* cvtask_api.h     : CVTASK_FREQUENCY_ON_DEMAND  0xFFU   (entry_initial_frequency is 8-bit)                      */
/* cvtask_coredef.h : NAME_MAX_LENGTH             32U                                                             */
/* cvtask_coredef.h : ENGINE_NAME_MAX_LENGTH      8U                                                              */
/* cvtask_coredef.h : SYSFLOW_MAX_CONFIG_DATA     8U                                                              */
/* cvtask_coredef.h : CVTASK_MAX_INPUTS           16U                                                             */
/* cvtask_coredef.h : CVTASK_MAX_FEEDBACK         4U                                                              */
/*================================================================================================================*/
#define SYSFLOW_FILE_VERSION                1U

typedef struct {
    char        instance_name[NAME_MAX_LENGTH];
    char        algorithm_name[NAME_MAX_LENGTH];
    char        step_name[NAME_MAX_LENGTH];
    char        cvtask_name[NAME_MAX_LENGTH];

    uint8_t     cvtask_chan_id;
    uint8_t     cvtask_num_inputs;
    uint8_t     cvtask_num_feedback;
    uint8_t     cvtask_initial_frequency;

    uint32_t    uuid;
    uint32_t    cvtask_debug_lvl;
    uint8_t     extrabuf_input;
    uint8_t     extrabuf_feedback;
    uint8_t     fixed_outbuf_num;
    uint8_t     cvtask_flags_2;
    uint32_t    cvtask_config_data[SYSFLOW_MAX_CONFIG_DATA];

    uint32_t    delta_task_deadline;
    uint32_t    delta_task_deadline_fail;
    uint32_t    input_uuid[CVTASK_MAX_INPUTS];
    uint32_t    feedback_uuid[CVTASK_MAX_FEEDBACK];

    char        engine_name[ENGINE_NAME_MAX_LENGTH];

    uint32_t    timeout_warning;
    uint32_t    timeout_error;
    uint32_t    timeout_panic1;
    uint32_t    timeout_panic2;

    uint32_t    timeout_cvtask_proc;

    uint32_t    reserved[23];       /* Align to 384 byte size, for future expansion */

} sysflow_entry_t;

#endif /* ?CVSCHED_SYSFLOW_FORMAT_H_FILE */

