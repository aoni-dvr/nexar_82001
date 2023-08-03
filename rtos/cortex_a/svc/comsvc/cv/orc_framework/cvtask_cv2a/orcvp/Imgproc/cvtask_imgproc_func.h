
#ifndef _imgproc_func__H_FILE
#define _imgproc_func__H_FILE

#include <cvtask_interface.h>

errcode_enum_t imgproc_cvtask_register(cvtask_entry_t *pCVTaskEntry);
errcode_enum_t imgproc_cvtask_query(uint32_t instance, const uint32_t *pConfigData, cvtask_memory_interface_t *pCVTaskMemory);
errcode_enum_t imgproc_cvtask_init(const cvtask_parameter_interface_t *pCVTaskParams, const uint32_t *pConfigData);
errcode_enum_t imgproc_cvtask_get_info(const cvtask_parameter_interface_t *pCVTaskParams, uint32_t info_index, void *vpInfoReturn);
errcode_enum_t imgproc_cvtask_process_messages(const cvtask_parameter_interface_t *pCVTaskParams);
errcode_enum_t imgproc_cvtask_run(const cvtask_parameter_interface_t *pCVTaskParams);

errcode_enum_t cvtask_vptask_awb_register(cvtask_entry_t *pCVTaskEntry);
errcode_enum_t cvtask_vptask_awb_query(uint32_t instance, const uint32_t *pConfigData, cvtask_memory_interface_t *pCVTaskMemory);
errcode_enum_t cvtask_vptask_awb_init(const cvtask_parameter_interface_t *pCVTaskParams, const uint32_t *pConfigData);
errcode_enum_t cvtask_vptask_awb_get_info(const cvtask_parameter_interface_t *pCVTaskParams, uint32_t info_index, void *vpInfoReturn);
errcode_enum_t cvtask_vptask_awb_process_messages(const cvtask_parameter_interface_t *pCVTaskParams);
errcode_enum_t cvtask_vptask_awb_run(const cvtask_parameter_interface_t *pCVTaskParams);

#endif
