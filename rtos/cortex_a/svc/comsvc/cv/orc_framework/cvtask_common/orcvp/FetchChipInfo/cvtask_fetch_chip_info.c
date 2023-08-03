
#include <cvtask_interface.h>
#include <vis_coproc.h>
#include <cvtask_errno.h>
#include <orc_memory.h>
#include <ucode_debug.h>
#include <vp_common.h>

#include "cvapi_idsp_interface.h"
#include "cvtask_fetch_chip_info.h"
#include "cvapi_flexidag_fetch_chip_info.h"

#define ALIGN_32_BYTE(x)    ((((x) + 31) >> 5) << 5)

#define MAX_PACKED_OUTPUTS 16
#define MAX_OUTPUTS (CVTASK_MAX_OUTPUTS * MAX_PACKED_OUTPUTS)

errcode_enum_t fetch_chip_info_cvtask_register(cvtask_entry_t *pCVTaskEntry)
{
    char task_name[NAME_MAX_LENGTH] = "FTECH_CHIP_INFO";

    CVTASK_PRINTF(LVL_NORMAL, ">>>>> starting fetch_chip_info_cvtask_register\n", 0, 0, 0, 0, 0);

    visorc_strcpy(&pCVTaskEntry->cvtask_name[0], &task_name[0], NAME_MAX_LENGTH);

    pCVTaskEntry->cvtask_type = CVTASK_TYPE_VPANY;
    pCVTaskEntry->cvtask_api_version = CVTASK_API_VERSION;

    pCVTaskEntry->cvtask_query = &fetch_chip_info_cvtask_query;
    pCVTaskEntry->cvtask_init = &fetch_chip_info_cvtask_init;
    pCVTaskEntry->cvtask_get_info = &fetch_chip_info_cvtask_get_info;
    pCVTaskEntry->cvtask_process_messages = &fetch_chip_info_cvtask_process_messages;
    pCVTaskEntry->cvtask_run = &fetch_chip_info_cvtask_run;

    CVTASK_PRINTF(LVL_NORMAL, ">>>>> completed fetch_chip_info_cvtask_register\n", 0, 0, 0, 0, 0);

    return ERRCODE_NONE;
}

cvtask_memory_interface_t fetch_chip_info_cvtask_interface = {
    .num_inputs = 1,
    .input[0].io_name = "IDSP_PICINFO",
    .input[0].history_needed = 0,

    .num_outputs = 1,
    .output[0].io_name = "CHIP_INFO",
    .output[0].buffer_size = sizeof(AMBA_CV_CHIP_INFO_t),
    .output[0].history_needed = 0,
};

errcode_enum_t fetch_chip_info_cvtask_query(uint32_t instance, const uint32_t *pConfigData, cvtask_memory_interface_t *pCVTaskMemory)
{
    errcode_enum_t retcode = ERRCODE_NONE;


    if ((pCVTaskMemory == NULL) || (pConfigData == NULL)) {
        retcode = ERRCODE_BAD_PARAMETER;
    }

    if (is_not_err(retcode)) {

        if (is_not_err(retcode)) {
            CVTASK_PRINTF(LVL_NORMAL, ">>>>> fetch_chip_info_cvtask_query(): fetch_chip_info_dag_query output\n", 0, 0, 0, 0, 0);

            *pCVTaskMemory = fetch_chip_info_cvtask_interface;

            // packed outputs order here matches the order in dag (and dag merge) query functions

            CVTASK_PRINTF(LVL_NORMAL, ">>>>> fetch_chip_info_cvtask_query(): fetch_chip_info_dag_query task 0 output size: %d\n", pCVTaskMemory->output[0].buffer_size, 0, 0, 0, 0);

            pCVTaskMemory->CVTask_shared_storage_needed = 0;
            pCVTaskMemory->DRAM_temporary_scratchpad_needed = 0;
            pCVTaskMemory->Instance_private_storage_needed = 0;
        } else {
            CVTASK_PRINTF(LVL_CRITICAL, ">>>>> fetch_chip_info_cvtask_query(): fetch_chip_info_dag_query failed\n", 0, 0, 0, 0, 0);
        }
    }

    return retcode;
}

errcode_enum_t fetch_chip_info_cvtask_init(const cvtask_parameter_interface_t *pCVTaskParams, const uint32_t *pConfigData)
{
    errcode_enum_t retcode = ERRCODE_NONE;

    if ((pCVTaskParams == NULL) || (pConfigData == NULL)) {
        retcode = ERRCODE_BAD_PARAMETER;
    }
    return retcode;
}

errcode_enum_t fetch_chip_info_cvtask_get_info(const cvtask_parameter_interface_t *pCVTaskParams, uint32_t info_index, void *vpInfoReturn)
{
    errcode_enum_t retcode = ERRCODE_NONE;

    return retcode;
}

errcode_enum_t fetch_chip_info_cvtask_process_messages(const cvtask_parameter_interface_t *pCVTaskParams)
{
    errcode_enum_t retcode = ERRCODE_NONE;

    return retcode;
}

errcode_enum_t fetch_chip_info_cvtask_run(const cvtask_parameter_interface_t *pCVTaskParams)
{
    errcode_enum_t retcode = ERRCODE_NONE;
    AMBA_CV_CHIP_INFO_t *output;

    CVTASK_PRINTF(LVL_DEBUG, ">>>>> cvtask_run() : input num    = %d : %08x %08x %08x %08x\n", pCVTaskParams->cvtask_num_inputs, (uint32_t)pCVTaskParams->vpInputBuffer[0], (uint32_t)pCVTaskParams->vpInputBuffer[1], (uint32_t)pCVTaskParams->vpInputBuffer[2], (uint32_t)pCVTaskParams->vpInputBuffer[3]);
    CVTASK_PRINTF(LVL_DEBUG, "                                           %08x %08x %08x %08x\n", (uint32_t)pCVTaskParams->vpInputBuffer[4], (uint32_t)pCVTaskParams->vpInputBuffer[5], (uint32_t)pCVTaskParams->vpInputBuffer[6], (uint32_t)pCVTaskParams->vpInputBuffer[7], 0);
    CVTASK_PRINTF(LVL_DEBUG, ">>>>> cvtask_run() : output num    = %d : %08x %08x %08x %08x\n", pCVTaskParams->cvtask_num_outputs, (uint32_t)pCVTaskParams->vpOutputBuffer[0], (uint32_t)pCVTaskParams->vpOutputBuffer[1], (uint32_t)pCVTaskParams->vpOutputBuffer[2], (uint32_t)pCVTaskParams->vpOutputBuffer[3]);
    CVTASK_PRINTF(LVL_DEBUG, "                                           %08x %08x %08x %08x\n", (uint32_t)pCVTaskParams->vpOutputBuffer[4], (uint32_t)pCVTaskParams->vpOutputBuffer[5], (uint32_t)pCVTaskParams->vpOutputBuffer[6], (uint32_t)pCVTaskParams->vpOutputBuffer[7], 0);
    CVTASK_PRINTF(LVL_DEBUG, ">>>>> cvtask_run() : feedback num = %d : %08x %08x %08x %08x\n", pCVTaskParams->cvtask_num_feedback, (uint32_t)pCVTaskParams->vpFeedbackBuffer[0], (uint32_t)pCVTaskParams->vpFeedbackBuffer[1], (uint32_t)pCVTaskParams->vpFeedbackBuffer[2], (uint32_t)pCVTaskParams->vpFeedbackBuffer[3]);
    CVTASK_PRINTF(LVL_DEBUG, ">>>>> cvtask_run() : message num    = %d : %08x %08x %08x %08x\n", pCVTaskParams->cvtask_num_messages, (uint32_t)pCVTaskParams->vpMessagePayload[0], (uint32_t)pCVTaskParams->vpMessagePayload[1], (uint32_t)pCVTaskParams->vpMessagePayload[2], (uint32_t)pCVTaskParams->vpMessagePayload[3]);
    if (pCVTaskParams->vpCMEM_temporary_scratchpad         != NULL) CVTASK_PRINTF(LVL_DEBUG, "        cvtask_run() : CMEM Scratchpad = %08x (%9d)\n", (uint32_t)pCVTaskParams->vpCMEM_temporary_scratchpad, pCVTaskParams->CMEM_temporary_scratchpad_size, 0, 0, 0);
    if (pCVTaskParams->vpDRAM_temporary_scratchpad         != NULL) CVTASK_PRINTF(LVL_DEBUG, "        cvtask_run() : DRAM Scratchpad = %08x (%9d)\n", (uint32_t)pCVTaskParams->vpDRAM_temporary_scratchpad, pCVTaskParams->DRAM_temporary_scratchpad_size, 0, 0, 0);
    if (pCVTaskParams->vpInstance_private_storage          != NULL) CVTASK_PRINTF(LVL_DEBUG, "        cvtask_run() : Private Storage = %08x (%9d)\n", (uint32_t)pCVTaskParams->vpInstance_private_storage, pCVTaskParams->Instance_private_storage_size, 0, 0, 0);
    if (pCVTaskParams->vpInstance_private_uncached_storage != NULL) CVTASK_PRINTF(LVL_DEBUG, "        cvtask_run() : UC-Priv Storage = %08x (%9d)\n", (uint32_t)pCVTaskParams->vpInstance_private_uncached_storage, pCVTaskParams->Instance_private_uncached_storage_size, 0, 0, 0);
    if (pCVTaskParams->vpCVTask_shared_storage             != NULL) CVTASK_PRINTF(LVL_DEBUG, "        cvtask_run() : Shared  Storage = %08x (%9d) [%2d]\n", (uint32_t)pCVTaskParams->vpCVTask_shared_storage, pCVTaskParams->CVTask_shared_storage_size, pCVTaskParams->cvtask_instance, 0, 0);

    output = pCVTaskParams->vpOutputBuffer[0];
    retcode = fetch_chip_info_cvtask_process_messages(pCVTaskParams);
    output->AmbaUUID[0] = 0;
    output->AmbaUUID[1] = 1;
    output->AmbaUUID[2] = 2;
    output->AmbaUUID[3] = 3;
    output->AmbaUUID[4] = 4;
    if (is_not_err(retcode)) {
        retcode = visorc_sec_get_unique_id(output->AmbaUUID);
        if (is_err(retcode)) {
            CVTASK_PRINTF(LVL_CRITICAL, "[ERROR] >>>>> fetch_chip_info_cvtask_run():visorc_sec_get_unique_id failed: ret = 0x%x\n", retcode, 0, 0, 0, 0);
        } else if (is_warn(retcode)) {
            CVTASK_PRINTF(LVL_CRITICAL, "[WARN] >>>>> fetch_chip_info_cvtask_run():visorc_sec_get_unique_id failed: ret = 0x%x\n", retcode, 0, 0, 0, 0);
        }
    } else {
        CVTASK_PRINTF(LVL_CRITICAL, ">>>>> fetch_chip_info_cvtask_run():fetch_chip_info_cvtask_process_messages failed: ret = 0x%x\n", retcode, 0, 0, 0, 0);
    }

    return retcode;
}
