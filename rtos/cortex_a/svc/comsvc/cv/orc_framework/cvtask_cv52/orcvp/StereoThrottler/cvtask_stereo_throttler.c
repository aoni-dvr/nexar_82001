#include "cvtask_stereo_throttler.h"
#include <cvapi_throttler_interface.h>
#include <ucode_debug.h>
#include <vp_common.h>

#ifdef CONFIG_BUILD_CV_THREADX
typedef signed char         INT8;           /* 8 bits, [-128, 127] */
typedef short               INT16;          /* 16 bits */
typedef int                 INT32;          /* 32 bits */

typedef unsigned char       UINT8;          /* 8 bits, [0, 255] */
typedef unsigned short      UINT16;         /* 16 bits */
typedef unsigned int        UINT32;         /* 32 bits */
#endif

#include "cvapi_common.h"
#include "cvapi_flexidag_ambaspufex_def_cv2fs.h"

/*-= Version information =----------------------------------------------------*/

#include "build_version.h"      /* Generated in the build directory */

#ifndef LIBRARY_VERSION
#define LIBRARY_VERSION "UNVERSIONED"
#endif
#ifndef TOOL_VERSION
#define TOOL_VERSION    "UNVERSIONED"
#endif

static const char local_cvtask_version_string[] = LIBRARY_VERSION;
static const char local_cvtask_tool_string[]    = TOOL_VERSION;

extern errcode_enum_t  visorc_sec_get_product_id(uint8_t *pAmbaUUID);


/*----------------------------------------------------------------------------*/

/**
 * cvtask_stereo_throttler_register()
 *
 * @author ckayano (9/8/2017)
 *
 * @param pCVTaskEntry
 *
 * @return errcode_enum_t
 */
errcode_enum_t  cvtask_stereo_throttler_register(cvtask_entry_t *pCVTaskEntry)
{
    char task_name[NAME_MAX_LENGTH] = TASKNAME_STEREO_THROTTLER;

    visorc_strcpy(&pCVTaskEntry->cvtask_name[0], &task_name[0], NAME_MAX_LENGTH);

    pCVTaskEntry->cvtask_type             = CVTASK_TYPE_VPANY;
    pCVTaskEntry->cvtask_api_version      = CVTASK_API_VERSION;
    pCVTaskEntry->cvtask_query            = &cvtask_stereo_throttler_query;
    pCVTaskEntry->cvtask_init             = &cvtask_stereo_throttler_init;
    pCVTaskEntry->cvtask_get_info         = &cvtask_stereo_throttler_get_info;
    pCVTaskEntry->cvtask_run              = &cvtask_stereo_throttler_run;
    pCVTaskEntry->cvtask_process_messages = &cvtask_stereo_throttler_process_messages;

    return ERRCODE_NONE;
} /* cvtask_stereo_throttler_register() */

cvtask_memory_interface_t  cvtask_stereo_throttler_interface = {
    .CVTask_shared_storage_needed     = 0,
    .Instance_private_storage_needed  = 0,
    .num_inputs                       = 1,
    .num_outputs                      = 1,

    .input[0].io_name                 = "DUMMY", /* filled in by query */

    .output[0].io_name                = "DUMMY", /* filled in by query */
    //.output[0].link_size_to_input     = 1,
    //.output[0].buffer_size = sizeof(cv_pic_info_t),


}; /* cvtask_stereo_throttler_interface */

/**
 * cvtask_stereo_throttler_query()
 *
 * @author ckayano (9/8/2017)
 *
 * @param instance
 * @param pConfigData
 * @param pCVTaskMemory
 *
 * @return errcode_enum_t
 */
errcode_enum_t  cvtask_stereo_throttler_query(uint32_t instance, const uint32_t *pConfigData, cvtask_memory_interface_t *pCVTaskMemory)
{
    errcode_enum_t  retcode;

    if (pConfigData == NULL) {
        CVTASK_PRINTF(LVL_CRITICAL, "[ERROR] : cvtask_stereo_throttler_query() : Invalid pConfigData (0x%08x)\n", (uint32_t)pConfigData, 0, 0, 0, 0);
        retcode = ERRCODE_BAD_PARAMETER;
    } /* if (pConfigData == NULL) */
    else if (pCVTaskMemory == NULL) {
        CVTASK_PRINTF(LVL_CRITICAL, "[ERROR] : cvtask_stereo_throttler_query() : Invalid pCVTaskMemory (0x%08x)\n", (uint32_t)pCVTaskMemory, 0, 0, 0, 0);
        retcode = ERRCODE_BAD_PARAMETER;
    } /* if (pCVTaskMemory == NULL) */
    else { /* if ((pConfigData != NULL) && (pCVTaskMemory != NULL)) */
        if (pConfigData[0] > 999) {
            CVTASK_PRINTF(LVL_CRITICAL, "[ERROR] : cvtask_stereo_throttler_query() : Invalid config_data[0] (%d, max 999)\n", pConfigData[0], 0, 0, 0, 0);
            retcode = ERRCODE_BAD_PARAMETER;
        } /* if (pConfigData[0] > 999) */
        else { /* if (pConfigData[0] <= 999) */
            char      throttler_ioname[32] = "THROTTLER_IONAME_###";
            char     *dPtr_throttler_ioname;
            uint32_t  throttler_ioname_size;

            throttler_ioname[17]  = 0x30 + ((pConfigData[0] / 100) % 10);
            throttler_ioname[18]  = 0x30 + ((pConfigData[0] /  10) % 10);
            throttler_ioname[19]  = 0x30 + ((pConfigData[0]      ) % 10);

            retcode = cvtable_find(&throttler_ioname[0], (const void **)&dPtr_throttler_ioname, &throttler_ioname_size);

            if (is_err(retcode)) {
                CVTASK_PRINTF(LVL_CRITICAL, "[ERROR] : cvtask_stereo_throttler_query() : Cannot find THROTTLER_IONAME_%03d name\n",pConfigData[0], 0, 0, 0, 0);
                retcode = ERRCODE_CVTASK_BASE + 1;
            } /* if (is_err(retcode)) */
            else { /* if (is_not_err(retcode)) */
                uint8_t AmbaUUID[32];

                CVTASK_PRINTF(LVL_DEBUG, ">>> cvtask_stereo_throttler_query(%d, 0x%08x, 0x%08x) - linking to THROTTLER_IONAME_%03d\n",
                              instance, (uint32_t)pConfigData, (uint32_t)pCVTaskMemory, pConfigData[0], 0);

                retcode         = ERRCODE_NONE;
                *pCVTaskMemory  = cvtask_stereo_throttler_interface;  /* Copy data */
                visorc_strcpy(&pCVTaskMemory->input[0].io_name[0], (char*)&dPtr_throttler_ioname[0], NAME_MAX_LENGTH);
                visorc_strcpy(&pCVTaskMemory->output[0].io_name[0], (char*)&dPtr_throttler_ioname[0], NAME_MAX_LENGTH);

                //fetch product id
                {
                    //uint8_t AmbaUUID[32];
                    AmbaUUID[0] = 0;
                    AmbaUUID[1] = 1;
                    AmbaUUID[2] = 2;
                    AmbaUUID[3] = 3;
                    AmbaUUID[4] = 4;
                    retcode = visorc_sec_get_product_id(AmbaUUID);
                    if (is_err(retcode)) {
                        CVTASK_PRINTF(LVL_CRITICAL, "[ERROR] >>>>> cvtask_stereo_throttler_run():visorc_sec_get_product_id failed: ret = 0x%x\n", retcode, 0, 0, 0, 0);
                    } else if (is_warn(retcode)) {
                        CVTASK_PRINTF(LVL_CRITICAL, "[WARN] >>>>> cvtask_stereo_throttler_run():visorc_sec_get_product_id failed: ret = 0x%x\n", retcode, 0, 0, 0, 0);
                    } else {
                        CVTASK_PRINTF(LVL_DEBUG, "      cvtask_stereo_throttler_run() :AmbaUUID = %2x %2x %2x %2x\n", AmbaUUID[0], AmbaUUID[1], AmbaUUID[2], AmbaUUID[3], 0);
                    }
                }

#define STEREO_IDSP_PICINFO 'I'
#define STEREO_SP_DISPARITY_MAP 'S'
#define STEREO_FEX_FEATURES_LIST 'F'

//product id
#define STEREO_CV2FS_CHIP    0x0BU

//cv2fs amba id
#define STEREO_CV2FS25        0x33U
#define STEREO_CV2FS75        0x13U
#define STEREO_CV2FS85        0x0BU
#define STEREO_CV2FS99        0x00U

//cv22fs amba id
#define STEREO_CV22FS15       0x7BU
#define STEREO_CV22FS55       0x53U
#define STEREO_CV22FS85       0x4BU
#define STEREO_CV22FS99       0x00U

                pCVTaskMemory->output[0].buffer_size = 0;
                if(pConfigData[4] == 0U) { //sp mode
                    if((AmbaUUID[0] == STEREO_CV2FS99) || (AmbaUUID[0] == STEREO_CV2FS85) || (AmbaUUID[0] == STEREO_CV2FS25)) {  //spfex+fusion
                        if(pCVTaskMemory->input[0].io_name[0] == STEREO_IDSP_PICINFO) { //"IDSP_PICINFO" for spfex
                            pCVTaskMemory->output[0].buffer_size = sizeof(cv_pic_info_t);
                        } else if(pCVTaskMemory->input[0].io_name[0] == STEREO_SP_DISPARITY_MAP) {   //"SP_DISPARITY_MAP" for fusioin
                            pCVTaskMemory->output[0].buffer_size = sizeof(AMBA_CV_SPU_DATA_s);
                        }
                    }
                } else if (pConfigData[4] == 1U) { //of mode
                    if ((AmbaUUID[0] == STEREO_CV2FS99) || (AmbaUUID[0] == STEREO_CV2FS85) || (AmbaUUID[0] == STEREO_CV2FS75) || (AmbaUUID[0] == STEREO_CV2FS25)
                        ||(AmbaUUID[0] == STEREO_CV22FS85) || (AmbaUUID[0] == STEREO_CV22FS55)) { //offex+fex
                        if(pCVTaskMemory->input[0].io_name[0] == STEREO_IDSP_PICINFO) { //"IDSP_PICINFO" for offex+fex
                            pCVTaskMemory->output[0].buffer_size = sizeof(cv_pic_info_t);
                        }
                    }
                }

            } /* if (is_not_err(retcode)) */
        } /* if (pConfigData[0] <= 999) */
    } /* if ((pConfigData != NULL) && (pCVTaskMemory != NULL)) */

    return retcode;
} /* cvtask_stereo_throttler_query() */

/**
 * cvtask_stereo_throttler_init()
 *
 * @author ckayano (9/8/2017)
 *
 * @param pCVTaskParams
 * @param pConfigData
 *
 * @return errcode_enum_t
 */
errcode_enum_t  cvtask_stereo_throttler_init(const cvtask_parameter_interface_t *pCVTaskParams, const uint32_t *pConfigData)
{
    errcode_enum_t  retcode;

    if (pCVTaskParams == NULL) {
        CVTASK_PRINTF(LVL_CRITICAL, "[ERROR] : cvtask_stereo_throttler_init() : Invalid pCVTaskParams (0x%08x)\n", (uint32_t)pCVTaskParams, 0, 0, 0, 0);
        retcode = ERRCODE_BAD_PARAMETER;
    } /* if (pCVTaskParams == NULL) */
    else if (pConfigData == NULL) {
        CVTASK_PRINTF(LVL_CRITICAL, "[ERROR] : cvtask_stereo_throttler_init() : Invalid pConfigData (0x%08x)\n", (uint32_t)pConfigData, 0, 0, 0, 0);
        retcode = ERRCODE_BAD_PARAMETER;
    } /* if (pConfigData == NULL) */
    else { /* if ((pCVTaskParams != NULL) && (pConfigData != NULL)) */
        retcode = ERRCODE_NONE;
        CVTASK_PRINTF(LVL_DEBUG, ">>> cvtask_stereo_throttler_init(0x%08x, 0x%08x)\n", (uint32_t)pCVTaskParams, (uint32_t)pConfigData, 0, 0, 0);
    } /* if ((pCVTaskParams != NULL) && (pConfigData != NULL)) */

    return retcode;
} /* cvtask_stereo_throttler_init() */

/**
 * cvtask_stereo_throttler_get_info()
 *
 * @author ckayano (9/8/2017)
 * @update ckayano (1/19/2018) : added build_info support
 *
 * @param pCVTaskParams
 * @param info_index
 * @param vpInfoReturn
 *
 * @return errcode_enum_t
 */
errcode_enum_t  cvtask_stereo_throttler_get_info(const cvtask_parameter_interface_t *pCVTaskParams, uint32_t info_index, void *vpInfoReturn)
{
    errcode_enum_t  retcode;

    if (vpInfoReturn == NULL) {
        retcode = ERRCODE_CVTASK_BASE;
        CVTASK_PRINTF(LVL_CRITICAL, "[ERROR] : (ERRCODE_CVTASK_BASE / 0x%08x) : cvtask_stereo_throttler_get_info() : invalid vpInfoReturn (0x%08x)\n",
                      retcode, (uint32_t)vpInfoReturn, 0, 0, 0);
    } /* if (vpInfoReturn == NULL) */
    else { /* if (vpInfoReturn != NULL) */
        retcode = ERRCODE_NONE;
        if (info_index == 0) {
            uint32_t   *recast32;
            recast32    = (uint32_t *)vpInfoReturn;
            recast32[0] = (uint32_t)&local_cvtask_version_string[0];
        } /* if (info_index == 0) */
        if (info_index == 1) {
            uint32_t   *recast32;
            recast32    = (uint32_t *)vpInfoReturn;
            recast32[0] = (uint32_t)&local_cvtask_tool_string[0];
        } /* if (info_index == 0) */
    } /* if (vpInfoReturn != NULL) */
    return retcode;
} /* cvtask_stereo_throttler_get_info() */

/**
 * cvtask_stereo_throttler_process_messages()
 *
 * @author ckayano (9/8/2017)
 *
 * @param pCVTaskParams
 *
 * @return errcode_enum_t
 */
errcode_enum_t  cvtask_stereo_throttler_process_messages(const cvtask_parameter_interface_t *pCVTaskParams)
{
    return ERRCODE_NONE;
} /* cvtask_stereo_throttler_process_messages() */

inline static uint32_t output_relative_pointer(uint32_t input_base_addr, uint32_t input_relative_pointer,uint32_t output_base_addr)
{
    return input_base_addr + input_relative_pointer - output_base_addr;
}

/**
 * cvtask_stereo_throttler_run()
 *
 * @author ckayano (9/8/2017)
 *
 * @param pCVTaskParams
 *
 * @return errcode_enum_t
 */
errcode_enum_t  cvtask_stereo_throttler_run(const cvtask_parameter_interface_t *pCVTaskParams)
{
    errcode_enum_t  retcode;

    if (pCVTaskParams == NULL) {
        CVTASK_PRINTF(LVL_CRITICAL, "[ERROR] : cvtask_stereo_throttler_run() : Invalid pCVTaskParams (0x%08x)\n", (uint32_t)pCVTaskParams, 0, 0, 0, 0);
        retcode = ERRCODE_BAD_PARAMETER;
    } /* if (pCVTaskParams == NULL) */
    else { /* if (pCVTaskParams != NULL) */
        if (pCVTaskParams->vpCMEM_temporary_scratchpad == NULL) {
            CVTASK_PRINTF(LVL_CRITICAL, "[ERROR] : cvtask_stereo_throttler_run() : Invoked with no CMEM Scratchpad provided\n", 0, 0, 0, 0, 0);
            retcode = ERRCODE_BAD_PARAMETER;
        } /* if (pCVTaskParams->vpCMEM_temporary_scratchpad == NULL) */
        else { /* if (pCVTaskParams->vpCMEM_temporary_scratchpad != NULL) */
            CVTASK_PRINTF(LVL_DEBUG, ">>>>> cvtask_stereo_throttler_run() : instance = %d, %08x, fno=%5d\n", pCVTaskParams->cvtask_instance, (uint32_t)pCVTaskParams->vpInstance_private_storage, pCVTaskParams->cvtask_frameset_id, 0, 0);
            CVTASK_PRINTF(LVL_DEBUG, ">>>>> cvtask_stereo_throttler_run() : input num    = %d : %08x %08x %08x %08x\n", pCVTaskParams->cvtask_num_inputs, (uint32_t)pCVTaskParams->vpInputBuffer[0], (uint32_t)pCVTaskParams->vpInputBuffer[1], (uint32_t)pCVTaskParams->vpInputBuffer[2], (uint32_t)pCVTaskParams->vpInputBuffer[3]);
            CVTASK_PRINTF(LVL_DEBUG, "      cvtask_stereo_throttler_run() : output num   = %d : %08x %08x %08x %08x\n", pCVTaskParams->cvtask_num_outputs, (uint32_t)pCVTaskParams->vpOutputBuffer[0], (uint32_t)pCVTaskParams->vpOutputBuffer[1], (uint32_t)pCVTaskParams->vpOutputBuffer[2], (uint32_t)pCVTaskParams->vpOutputBuffer[3]);
            CVTASK_PRINTF(LVL_DEBUG, "      cvtask_stereo_throttler_run() : feedback num = %d : %08x %08x %08x %08x\n", pCVTaskParams->cvtask_num_feedback, (uint32_t)pCVTaskParams->vpFeedbackBuffer[0], (uint32_t)pCVTaskParams->vpFeedbackBuffer[1], (uint32_t)pCVTaskParams->vpFeedbackBuffer[2], (uint32_t)pCVTaskParams->vpFeedbackBuffer[3]);
            CVTASK_PRINTF(LVL_DEBUG, "      cvtask_stereo_throttler_run() : message num  = %d : %08x %08x %08x %08x\n", pCVTaskParams->cvtask_num_messages, (uint32_t)pCVTaskParams->vpMessagePayload[0], (uint32_t)pCVTaskParams->vpMessagePayload[1], (uint32_t)pCVTaskParams->vpMessagePayload[2], (uint32_t)pCVTaskParams->vpMessagePayload[3]);
            if (pCVTaskParams->vpCMEM_temporary_scratchpad         != NULL) CVTASK_PRINTF(LVL_DEBUG, "      cvtask_stereo_throttler_run() : CMEM Scratchpad = %08x (%9d)\n", (uint32_t)pCVTaskParams->vpCMEM_temporary_scratchpad, pCVTaskParams->CMEM_temporary_scratchpad_size, 0, 0, 0);
            if (pCVTaskParams->vpDRAM_temporary_scratchpad         != NULL) CVTASK_PRINTF(LVL_DEBUG, "      cvtask_stereo_throttler_run() : DRAM Scratchpad = %08x (%9d)\n", (uint32_t)pCVTaskParams->vpDRAM_temporary_scratchpad, pCVTaskParams->DRAM_temporary_scratchpad_size, 0, 0, 0);
            if (pCVTaskParams->vpInstance_private_storage          != NULL) CVTASK_PRINTF(LVL_DEBUG, "      cvtask_stereo_throttler_run() : Private Storage = %08x (%9d)\n", (uint32_t)pCVTaskParams->vpInstance_private_storage, pCVTaskParams->Instance_private_storage_size, 0, 0, 0);
            if (pCVTaskParams->vpInstance_private_uncached_storage != NULL) CVTASK_PRINTF(LVL_DEBUG, "      cvtask_stereo_throttler_run() : UC-Priv Storage = %08x (%9d)\n", (uint32_t)pCVTaskParams->vpInstance_private_uncached_storage, pCVTaskParams->Instance_private_uncached_storage_size, 0, 0, 0);
            if (pCVTaskParams->vpCVTask_shared_storage             != NULL) CVTASK_PRINTF(LVL_DEBUG, "      cvtask_stereo_throttler_run() : Shared  Storage = %08x (%9d) [%2d]\n", (uint32_t)pCVTaskParams->vpCVTask_shared_storage, pCVTaskParams->CVTask_shared_storage_size, pCVTaskParams->cvtask_instance, 0, 0);

            retcode = cvtask_stereo_throttler_process_messages(pCVTaskParams);

            if (is_not_err(retcode)) {
                uint32_t  insize, outsize, left, xfersize;
                uint32_t  src_daddr, dst_daddr;
                uint32_t i;




                insize    = cvtask_get_input_size(0);
                outsize   = cvtask_get_output_size(0);
                //left      = (outsize > insize) ? outsize : insize;
                left = outsize;
                src_daddr = (uint32_t)pCVTaskParams->vpInputBuffer[0];
                dst_daddr = (uint32_t)pCVTaskParams->vpOutputBuffer[0];

                vp_init();

                //if(AmbaUUID[1] == 0x09U){
                while (left > 0) {
                    xfersize = (left > 65536) ? 65536 : left;
                    vp_load_buf(src_daddr, 0x1000, xfersize);
                    vp_store_buf(dst_daddr, 0x1000, xfersize);
                    src_daddr  += xfersize;
                    dst_daddr  += xfersize;
                    left       -= xfersize;
                } /* while (left > 0) */

#if 1
                if(outsize == sizeof(cv_pic_info_t)) {
                    cv_pic_info_t *in_pic_info = pCVTaskParams->vpInputBuffer[0];
                    cv_pic_info_t *out_pic_info = pCVTaskParams->vpOutputBuffer[0];
                    for(i = 0; i<MAX_HALF_OCTAVES; ++i) {
                        out_pic_info->rpLumaLeft[i] = output_relative_pointer((uint32_t)in_pic_info, in_pic_info->rpLumaLeft[i], (uint32_t)out_pic_info);
                        out_pic_info->rpLumaRight[i] = output_relative_pointer((uint32_t)in_pic_info, in_pic_info->rpLumaRight[i], (uint32_t)out_pic_info);
                    }
                } else if(outsize == sizeof(AMBA_CV_SPU_DATA_s)) {
                    AMBA_CV_SPU_DATA_s *in_spu_data = pCVTaskParams->vpInputBuffer[0];
                    AMBA_CV_SPU_DATA_s *out_spu_data = pCVTaskParams->vpOutputBuffer[0];
                    for(i = 0; i<MAX_HALF_OCTAVES; ++i) {
                        out_spu_data->Scales[i].DisparityMapOffset = output_relative_pointer((uint32_t)in_spu_data, in_spu_data->Scales[i].DisparityMapOffset, (uint32_t)out_spu_data);
                    }
                } else if(outsize == sizeof(AMBA_CV_FEX_DATA_s)) {
                    AMBA_CV_FEX_DATA_s *in_fex_data = pCVTaskParams->vpInputBuffer[0];
                    AMBA_CV_FEX_DATA_s *out_fex_data = pCVTaskParams->vpOutputBuffer[0];
                    for(i = 0; i<MAX_HALF_OCTAVES; ++i) {
                        out_fex_data->PrimaryList[i].KeypointsCountOffset = output_relative_pointer((uint32_t)in_fex_data, in_fex_data->PrimaryList[i].KeypointsCountOffset, (uint32_t)out_fex_data);
                        out_fex_data->PrimaryList[i].KeypointsOffset      = output_relative_pointer((uint32_t)in_fex_data, in_fex_data->PrimaryList[i].KeypointsOffset, (uint32_t)out_fex_data);
                        out_fex_data->PrimaryList[i].DescriptorsOffset    = output_relative_pointer((uint32_t)in_fex_data, in_fex_data->PrimaryList[i].DescriptorsOffset, (uint32_t)out_fex_data);
                        out_fex_data->SecondaryList[i].KeypointsCountOffset = output_relative_pointer((uint32_t)in_fex_data, in_fex_data->SecondaryList[i].KeypointsCountOffset, (uint32_t)out_fex_data);
                        out_fex_data->SecondaryList[i].KeypointsOffset      = output_relative_pointer((uint32_t)in_fex_data, in_fex_data->SecondaryList[i].KeypointsOffset, (uint32_t)out_fex_data);
                        out_fex_data->SecondaryList[i].DescriptorsOffset    = output_relative_pointer((uint32_t)in_fex_data, in_fex_data->SecondaryList[i].DescriptorsOffset, (uint32_t)out_fex_data);
                    }
                }
#endif

            } /* if (is_not_err(retcode)) : cvtask_stereo_throttler_process_messages() */
        } /* if (pCVTaskParams->vpCMEM_temporary_scratchpad != NULL) */
    } /* if (pCVTaskParams != NULL) */

    return retcode;

} /* cvtask_stereo_throttler_run() */

