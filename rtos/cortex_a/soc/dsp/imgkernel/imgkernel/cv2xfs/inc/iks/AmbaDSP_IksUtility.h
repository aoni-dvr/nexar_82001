#ifndef IKS_UTILITY_H
#define IKS_UTILITY_H

#include "AmbaDSP_ImgArchSystemAPI.h"
#include "AmbaDSP_IksFilter.h"
#include "AmbaDSP_ImgCommon.h"


typedef struct {
    uint32 ik_major_num;
    uint32 iks_major_num;
    uint32 ik_minor_num;
    uint32 ikc_minor_num;
    void *p_ikc_addr;
} iks_check_info_t;


uint32 iks_query_arch_memory_size(const ik_context_setting_t *p_setting_of_each_context, size_t *p_total_ctx_size, size_t *p_total_cr_size);
uint32 iks_init_arch(const ik_context_setting_t *p_setting_of_each_context, void* p_mem_addr, size_t mem_size, uint32 ik_init_mode,
                     const iks_check_info_t *p_info);
uint32 iks_init_default_binary(void *p_bin_data_dram_addr);
uint32 iks_init_context(uint32 context_id, const ik_ability_t *p_ability);
uint32 iks_get_context_ability(uint32 context_id, ik_ability_t *p_ability);
uint32 iks_execute(uint32 context_id, ik_execute_container_t *p_execute_container);
uint32 iks_calc_geo_settings(const ik_ability_t *p_ability, const ik_calc_geo_settings_t *p_info, const ik_calc_geo_result_t *p_result);

uint32 iks_get_minor_version(uint32 *p_minor_ver);
uint32 iks_arch_get_ik_working_buffer(ik_buffer_info_t **p_working_buffer);
#endif
