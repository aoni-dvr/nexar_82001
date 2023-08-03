
#ifndef _fetch_chip_info__H_FILE
#define _fetch_chip_info__H_FILE

#include <cvtask_interface.h>

errcode_enum_t fetch_chip_info_cvtask_register(cvtask_entry_t *pCVTaskEntry);
errcode_enum_t fetch_chip_info_cvtask_query(uint32_t instance, const uint32_t *pConfigData, cvtask_memory_interface_t *pCVTaskMemory);
errcode_enum_t fetch_chip_info_cvtask_init(const cvtask_parameter_interface_t *pCVTaskParams, const uint32_t *pConfigData);
errcode_enum_t fetch_chip_info_cvtask_get_info(const cvtask_parameter_interface_t *pCVTaskParams, uint32_t info_index, void *vpInfoReturn);
errcode_enum_t fetch_chip_info_cvtask_process_messages(const cvtask_parameter_interface_t *pCVTaskParams);
errcode_enum_t fetch_chip_info_cvtask_run(const cvtask_parameter_interface_t *pCVTaskParams);

#endif