#ifndef CVTASK_STEREO_THROTTLER_H_FILE
#define CVTASK_STEREO_THROTTLER_H_FILE

#include <cvtask_interface.h>

errcode_enum_t  cvtask_stereo_throttler_register(cvtask_entry_t *pCVTaskEntry);
errcode_enum_t  cvtask_stereo_throttler_query(uint32_t instance, const uint32_t *pConfigData, cvtask_memory_interface_t *pCVTaskMemory);
errcode_enum_t  cvtask_stereo_throttler_init(const cvtask_parameter_interface_t *pCVTaskParams, const uint32_t *pConfigData);
errcode_enum_t  cvtask_stereo_throttler_get_info(const cvtask_parameter_interface_t *pCVTaskParams, uint32_t info_index, void *vpInfoReturn);
errcode_enum_t  cvtask_stereo_throttler_run(const cvtask_parameter_interface_t *pCVTaskParams);
errcode_enum_t  cvtask_stereo_throttler_process_messages(const cvtask_parameter_interface_t *pCVTaskParams);

/**
 * pConfig[0] : Links to THROTTLER_IONAME_###
 */

#endif /* CVTASK_STEREO_THROTTLER_H_FILE */

