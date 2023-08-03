/*
 * Copyright (c) 2017-2020 Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use,
 * reproduce, disclose, distribute, modify, or otherwise prepare derivative
 * works of this Software or any portion thereof except pursuant to a signed
 * license agreement or nondisclosure agreement with Ambarella International LP
 * or its authorized affiliates. In the absence of such an agreement, you agree
 * to promptly notify and return this Software to Ambarella International LP.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "mnet_ssd_adas_flex_pic_ag.h"
#include "mnet_ssd_adas_flex_pic_ag_private.h"

static const uint32_t dag_img_sizes[] = {
    4968U, 
    2968U, 
    12620U, 
    20984U, 
    2224U, 
    1380U, 
    39912U, 
    1276U, 
    63780U, 
    8380U, 
    126088U, 
    261084U, 
    15048U, 
    251888U, 
    259744U, 
    14372U, 
    1476U, 
    252940U, 
    15152U, 
    253136U, 
    31684U, 
    1452U, 
    1308U, 
    216152U, 
    223352U, 
    25296U, 
    1396U, 
    1232U, 
    328652U, 
    329072U, 
    327916U, 
    313468U, 
    296068U, 
    295868U, 
    295868U, 
    296384U, 
    397404U, 
    347644U, 
    81124U, 
    9840U, 
    168U, 
};

static const uint32_t vmem_dag_bases[] = {
    509692U, 
    510560U, 
    499296U, 
    495200U, 
    509792U, 
    499552U, 
    475872U, 
    512352U, 
    436320U, 
    507872U, 
    385248U, 
    225376U, 
    499456U, 
    262432U, 
    249952U, 
    497120U, 
    508768U, 
    256288U, 
    501376U, 
    260512U, 
    266144U, 
    234024U, 
    233176U, 
    262240U, 
    176224U, 
    488672U, 
    414272U, 
    311632U, 
    173160U, 
    173160U, 
    177876U, 
    170108U, 
    64232U, 
    27232U, 
    27232U, 
    122952U, 
    50408U, 
    16904U, 
    62232U, 
    94280U, 
    53088U, 
};

static const uint32_t vmem_dagbin_bases[] = {
    513172U, 
    511900U, 
    510072U, 
    513868U, 
    510260U, 
    499552U, 
    513620U, 
    512352U, 
    499332U, 
    513868U, 
    510204U, 
    484976U, 
    511852U, 
    513036U, 
    508104U, 
    509360U, 
    508768U, 
    508476U, 
    513876U, 
    512364U, 
    295660U, 
    234024U, 
    233176U, 
    478084U, 
    399332U, 
    511592U, 
    414272U, 
    311632U, 
    501568U, 
    501988U, 
    505420U, 
    483332U, 
    359848U, 
    322848U, 
    322848U, 
    418568U, 
    447088U, 
    363836U, 
    142192U, 
    103092U, 
    53088U, 
};

/******************************* CORE FUNCTIONS *******************************/
/* Populate structs with constants
 * Calls the wrapper function for cvtable_find()
 */
errcode_enum_t mnet_ssd_adas_flex_pic_ag_init (
    mnet_ssd_adas_flex_pic_ag_ips_t *ctxt
) {
    errcode_enum_t status = ERRCODE_NONE;

    if (!ctxt) {
        status = ERRCODE_BAD_PARAMETER;
        cvtask_printf(LVL_CRITICAL, " >>> ** FATAL! Context pointer is NULL",
            0, 0, 0, 0, 0);
        return status;
    }

    ctxt->split_id = 0U;

    // Find dram_dag_base for each split
    status = mnet_ssd_adas_flex_pic_ag_read_cvtable(ctxt);
    if (is_err(status)) {
        cvtask_printf(LVL_DEBUG, "mnet_ssd_adas_flex_pic_ag_read_cvtable() has failed.", 0, 0
            , 0, 0, 0);
        return status;
    }

    return status;
} // end of mnet_ssd_adas_flex_pic_ag_init()

/* Execute VP DAGs
 * run() is NOT re-entrant with respect to run() calls for other DAGs
 */
errcode_enum_t mnet_ssd_adas_flex_pic_ag_run (
    mnet_ssd_adas_flex_pic_ag_ips_t             *ctxt,
    mnet_ssd_adas_flex_pic_ag_required_fields_t *r_args,
    mnet_ssd_adas_flex_pic_ag_optional_fields_t *o_args
) {
    int split_id = 0;
    errcode_enum_t status = ERRCODE_NONE;
    struct mnet_ssd_adas_flex_pic_ag_split_context *split_ctxt = NULL;
#ifdef CVTASK_PRINT
    uint32_t time_start, time_end;
#endif

    if (!ctxt || !r_args || !o_args) {
        status = ERRCODE_BAD_PARAMETER;
        cvtask_printf(LVL_DEBUG, "  > Error: NULL pointer(s).", 0, 0, 0, 0, 0);
        return status;
    }
    if(!r_args->instance_private_uncached_storage) {
        status = ERRCODE_BAD_PARAMETER;
        cvtask_printf(LVL_DEBUG, "  > Error: NULL pointer(s).", 0, 0, 0, 0, 0);
        return status;
    }

    split_id = ctxt->split_id;
#ifdef CVTASK_PRINT
    time_start = get_cur_time();
#endif

    // Initialize VP
    vp_init();

    for (; split_id < MNET_SSD_ADAS_FLEX_PIC_AG_SPLIT_COUNT; split_id++) {
        split_ctxt = &ctxt->split_ctxt[split_id];
        mnet_ssd_adas_flex_pic_ag_load_split(split_ctxt, split_id);
        wait(VIS_W_COPROC); // wait on dummy run

        mnet_ssd_adas_flex_pic_ag_poke_split(r_args, o_args, split_ctxt, split_id);
#ifdef CVTASK_PRINT
        time_end = get_cur_time();
        cvtask_printf(LVL_NORMAL,
            "  >> done loading split-#%03u.     cycles=(%u)",
            split_id, time_end - time_start, 0, 0, 0);
        time_start = time_end;
#endif
        mnet_ssd_adas_flex_pic_ag_run_split(split_id);
        mnet_ssd_adas_flex_pic_ag_peek_split(r_args, split_ctxt, split_id);

        wait(VIS_W_COPROC); // wait on coproc_run()
        status |= mnet_ssd_adas_flex_pic_ag_vp_status();

#ifdef CVTASK_PRINT
        time_end = get_cur_time();
        cvtask_printf(LVL_NORMAL,
            "  >> done executing split-#%03u.   cycles=(%u)",
            split_id, time_end - time_start, 0, 0, 0);
        time_start = time_end;
#endif
        
        if (cvtask_check_yield()) {
            split_id += 1U;
            ctxt->split_id = split_id;
            return RETCODE_CVTASK_YIELDED;
        }

    }

    ctxt->split_id = 0U;

    return status;
} // end of mnet_ssd_adas_flex_pic_ag_run()

/* Populate struct with default values */
errcode_enum_t mnet_ssd_adas_flex_pic_ag_reset(
    mnet_ssd_adas_flex_pic_ag_optional_fields_t *o_args
) {
    errcode_enum_t status = ERRCODE_NONE;

    if (!o_args) {
        status = ERRCODE_BAD_PARAMETER;
        cvtask_printf(LVL_DEBUG, "  > Error: NULL pointer(s).", 0, 0, 0, 0, 0);
        return status;
    }

#ifdef __PVCN_1007__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_1007__dram_pitch_enable = 1U;
    o_args->__pvcn_1007__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_1018__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_1018__dram_pitch_enable = 1U;
    o_args->__pvcn_1018__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_1029__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_1029__dram_pitch_enable = 1U;
    o_args->__pvcn_1029__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_1040__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_1040__dram_pitch_enable = 1U;
    o_args->__pvcn_1040__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_1051__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_1051__dram_pitch_enable = 1U;
    o_args->__pvcn_1051__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_1062__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_1062__dram_pitch_enable = 1U;
    o_args->__pvcn_1062__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_1073__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_1073__dram_pitch_enable = 1U;
    o_args->__pvcn_1073__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_1084__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_1084__dram_pitch_enable = 1U;
    o_args->__pvcn_1084__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_1293__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_1293__dram_pitch_enable = 1U;
    o_args->__pvcn_1293__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_1304__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_1304__dram_pitch_enable = 1U;
    o_args->__pvcn_1304__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_1315__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_1315__dram_pitch_enable = 1U;
    o_args->__pvcn_1315__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_1326__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_1326__dram_pitch_enable = 1U;
    o_args->__pvcn_1326__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_1337__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_1337__dram_pitch_enable = 1U;
    o_args->__pvcn_1337__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_1348__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_1348__dram_pitch_enable = 1U;
    o_args->__pvcn_1348__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_1359__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_1359__dram_pitch_enable = 1U;
    o_args->__pvcn_1359__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_1370__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_1370__dram_pitch_enable = 1U;
    o_args->__pvcn_1370__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_1381__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_1381__dram_pitch_enable = 1U;
    o_args->__pvcn_1381__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_1392__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_1392__dram_pitch_enable = 1U;
    o_args->__pvcn_1392__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_1403__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_1403__dram_pitch_enable = 1U;
    o_args->__pvcn_1403__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_1414__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_1414__dram_pitch_enable = 1U;
    o_args->__pvcn_1414__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_1425__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_1425__dram_pitch_enable = 1U;
    o_args->__pvcn_1425__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_1436__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_1436__dram_pitch_enable = 1U;
    o_args->__pvcn_1436__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_1626__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_1626__dram_pitch_enable = 1U;
    o_args->__pvcn_1626__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_1670__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_1670__dram_pitch_enable = 1U;
    o_args->__pvcn_1670__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_1681__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_1681__dram_pitch_enable = 1U;
    o_args->__pvcn_1681__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_1692__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_1692__dram_pitch_enable = 1U;
    o_args->__pvcn_1692__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_1703__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_1703__dram_pitch_enable = 1U;
    o_args->__pvcn_1703__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_1714__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_1714__dram_pitch_enable = 1U;
    o_args->__pvcn_1714__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_1725__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_1725__dram_pitch_enable = 1U;
    o_args->__pvcn_1725__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_1736__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_1736__dram_pitch_enable = 1U;
    o_args->__pvcn_1736__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_1747__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_1747__dram_pitch_enable = 1U;
    o_args->__pvcn_1747__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_1758__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_1758__dram_pitch_enable = 1U;
    o_args->__pvcn_1758__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_1769__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_1769__dram_pitch_enable = 1U;
    o_args->__pvcn_1769__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_1780__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_1780__dram_pitch_enable = 1U;
    o_args->__pvcn_1780__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_1791__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_1791__dram_pitch_enable = 1U;
    o_args->__pvcn_1791__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_1802__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_1802__dram_pitch_enable = 1U;
    o_args->__pvcn_1802__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_1813__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_1813__dram_pitch_enable = 1U;
    o_args->__pvcn_1813__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_1830__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_1830__dram_pitch_enable = 1U;
    o_args->__pvcn_1830__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_1831__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_1831__dram_pitch_enable = 1U;
    o_args->__pvcn_1831__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_1832__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_1832__dram_pitch_enable = 1U;
    o_args->__pvcn_1832__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_1833__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_1833__dram_pitch_enable = 1U;
    o_args->__pvcn_1833__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_1834__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_1834__dram_pitch_enable = 1U;
    o_args->__pvcn_1834__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_1835__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_1835__dram_pitch_enable = 1U;
    o_args->__pvcn_1835__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_1836__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_1836__dram_pitch_enable = 1U;
    o_args->__pvcn_1836__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_1879__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_1879__dram_pitch_enable = 1U;
    o_args->__pvcn_1879__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_1890__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_1890__dram_pitch_enable = 1U;
    o_args->__pvcn_1890__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_1901__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_1901__dram_pitch_enable = 1U;
    o_args->__pvcn_1901__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_1912__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_1912__dram_pitch_enable = 1U;
    o_args->__pvcn_1912__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_1923__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_1923__dram_pitch_enable = 1U;
    o_args->__pvcn_1923__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_1934__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_1934__dram_pitch_enable = 1U;
    o_args->__pvcn_1934__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_1945__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_1945__dram_pitch_enable = 1U;
    o_args->__pvcn_1945__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_1956__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_1956__dram_pitch_enable = 1U;
    o_args->__pvcn_1956__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_1967__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_1967__dram_pitch_enable = 1U;
    o_args->__pvcn_1967__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_1978__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_1978__dram_pitch_enable = 1U;
    o_args->__pvcn_1978__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_1989__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_1989__dram_pitch_enable = 1U;
    o_args->__pvcn_1989__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2000__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2000__dram_pitch_enable = 1U;
    o_args->__pvcn_2000__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2011__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2011__dram_pitch_enable = 1U;
    o_args->__pvcn_2011__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2022__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2022__dram_pitch_enable = 1U;
    o_args->__pvcn_2022__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2094__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2094__dram_pitch_enable = 1U;
    o_args->__pvcn_2094__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2100__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2100__dram_pitch_enable = 1U;
    o_args->__pvcn_2100__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2105__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2105__dram_pitch_enable = 1U;
    o_args->__pvcn_2105__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2111__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2111__dram_pitch_enable = 1U;
    o_args->__pvcn_2111__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2116__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2116__dram_pitch_enable = 1U;
    o_args->__pvcn_2116__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2122__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2122__dram_pitch_enable = 1U;
    o_args->__pvcn_2122__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2127__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2127__dram_pitch_enable = 1U;
    o_args->__pvcn_2127__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2133__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2133__dram_pitch_enable = 1U;
    o_args->__pvcn_2133__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2138__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2138__dram_pitch_enable = 1U;
    o_args->__pvcn_2138__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2144__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2144__dram_pitch_enable = 1U;
    o_args->__pvcn_2144__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2149__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2149__dram_pitch_enable = 1U;
    o_args->__pvcn_2149__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2155__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2155__dram_pitch_enable = 1U;
    o_args->__pvcn_2155__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2160__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2160__dram_pitch_enable = 1U;
    o_args->__pvcn_2160__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2166__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2166__dram_pitch_enable = 1U;
    o_args->__pvcn_2166__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2171__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2171__dram_pitch_enable = 1U;
    o_args->__pvcn_2171__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2177__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2177__dram_pitch_enable = 1U;
    o_args->__pvcn_2177__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2182__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2182__dram_pitch_enable = 1U;
    o_args->__pvcn_2182__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2188__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2188__dram_pitch_enable = 1U;
    o_args->__pvcn_2188__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2193__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2193__dram_pitch_enable = 1U;
    o_args->__pvcn_2193__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2199__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2199__dram_pitch_enable = 1U;
    o_args->__pvcn_2199__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2204__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2204__dram_pitch_enable = 1U;
    o_args->__pvcn_2204__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2210__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2210__dram_pitch_enable = 1U;
    o_args->__pvcn_2210__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2215__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2215__dram_pitch_enable = 1U;
    o_args->__pvcn_2215__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2221__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2221__dram_pitch_enable = 1U;
    o_args->__pvcn_2221__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2226__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2226__dram_pitch_enable = 1U;
    o_args->__pvcn_2226__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2232__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2232__dram_pitch_enable = 1U;
    o_args->__pvcn_2232__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2237__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2237__dram_pitch_enable = 1U;
    o_args->__pvcn_2237__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2243__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2243__dram_pitch_enable = 1U;
    o_args->__pvcn_2243__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2281__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2281__dram_pitch_enable = 1U;
    o_args->__pvcn_2281__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2292__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2292__dram_pitch_enable = 1U;
    o_args->__pvcn_2292__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2359__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2359__dram_pitch_enable = 1U;
    o_args->__pvcn_2359__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2365__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2365__dram_pitch_enable = 1U;
    o_args->__pvcn_2365__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2370__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2370__dram_pitch_enable = 1U;
    o_args->__pvcn_2370__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2376__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2376__dram_pitch_enable = 1U;
    o_args->__pvcn_2376__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2381__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2381__dram_pitch_enable = 1U;
    o_args->__pvcn_2381__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2387__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2387__dram_pitch_enable = 1U;
    o_args->__pvcn_2387__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2392__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2392__dram_pitch_enable = 1U;
    o_args->__pvcn_2392__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2398__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2398__dram_pitch_enable = 1U;
    o_args->__pvcn_2398__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2403__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2403__dram_pitch_enable = 1U;
    o_args->__pvcn_2403__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2409__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2409__dram_pitch_enable = 1U;
    o_args->__pvcn_2409__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2414__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2414__dram_pitch_enable = 1U;
    o_args->__pvcn_2414__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2420__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2420__dram_pitch_enable = 1U;
    o_args->__pvcn_2420__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2425__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2425__dram_pitch_enable = 1U;
    o_args->__pvcn_2425__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2431__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2431__dram_pitch_enable = 1U;
    o_args->__pvcn_2431__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2436__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2436__dram_pitch_enable = 1U;
    o_args->__pvcn_2436__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2442__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2442__dram_pitch_enable = 1U;
    o_args->__pvcn_2442__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2447__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2447__dram_pitch_enable = 1U;
    o_args->__pvcn_2447__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2453__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2453__dram_pitch_enable = 1U;
    o_args->__pvcn_2453__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2458__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2458__dram_pitch_enable = 1U;
    o_args->__pvcn_2458__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2464__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2464__dram_pitch_enable = 1U;
    o_args->__pvcn_2464__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2469__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2469__dram_pitch_enable = 1U;
    o_args->__pvcn_2469__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2475__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2475__dram_pitch_enable = 1U;
    o_args->__pvcn_2475__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2480__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2480__dram_pitch_enable = 1U;
    o_args->__pvcn_2480__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2486__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2486__dram_pitch_enable = 1U;
    o_args->__pvcn_2486__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2491__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2491__dram_pitch_enable = 1U;
    o_args->__pvcn_2491__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2497__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2497__dram_pitch_enable = 1U;
    o_args->__pvcn_2497__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2502__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2502__dram_pitch_enable = 1U;
    o_args->__pvcn_2502__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2508__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2508__dram_pitch_enable = 1U;
    o_args->__pvcn_2508__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2513__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2513__dram_pitch_enable = 1U;
    o_args->__pvcn_2513__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2519__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2519__dram_pitch_enable = 1U;
    o_args->__pvcn_2519__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2556__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2556__dram_pitch_enable = 1U;
    o_args->__pvcn_2556__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2564__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2564__dram_pitch_enable = 1U;
    o_args->__pvcn_2564__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2592__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2592__dram_pitch_enable = 1U;
    o_args->__pvcn_2592__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2600__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2600__dram_pitch_enable = 1U;
    o_args->__pvcn_2600__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2608__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2608__dram_pitch_enable = 1U;
    o_args->__pvcn_2608__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_263__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_263__dram_pitch_enable = 1U;
    o_args->__pvcn_263__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2663__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2663__dram_pitch_enable = 1U;
    o_args->__pvcn_2663__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2666__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2666__dram_pitch_enable = 1U;
    o_args->__pvcn_2666__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2683__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2683__dram_pitch_enable = 1U;
    o_args->__pvcn_2683__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_2686__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_2686__dram_pitch_enable = 1U;
    o_args->__pvcn_2686__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_270__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_270__dram_pitch_enable = 1U;
    o_args->__pvcn_270__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_332__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_332__dram_pitch_enable = 1U;
    o_args->__pvcn_332__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_343__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_343__dram_pitch_enable = 1U;
    o_args->__pvcn_343__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_354__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_354__dram_pitch_enable = 1U;
    o_args->__pvcn_354__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_365__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_365__dram_pitch_enable = 1U;
    o_args->__pvcn_365__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_376__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_376__dram_pitch_enable = 1U;
    o_args->__pvcn_376__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_387__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_387__dram_pitch_enable = 1U;
    o_args->__pvcn_387__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_398__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_398__dram_pitch_enable = 1U;
    o_args->__pvcn_398__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_409__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_409__dram_pitch_enable = 1U;
    o_args->__pvcn_409__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_543__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_543__dram_pitch_enable = 1U;
    o_args->__pvcn_543__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_549__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_549__dram_pitch_enable = 1U;
    o_args->__pvcn_549__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_554__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_554__dram_pitch_enable = 1U;
    o_args->__pvcn_554__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_560__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_560__dram_pitch_enable = 1U;
    o_args->__pvcn_560__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_565__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_565__dram_pitch_enable = 1U;
    o_args->__pvcn_565__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_571__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_571__dram_pitch_enable = 1U;
    o_args->__pvcn_571__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_576__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_576__dram_pitch_enable = 1U;
    o_args->__pvcn_576__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_582__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_582__dram_pitch_enable = 1U;
    o_args->__pvcn_582__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_587__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_587__dram_pitch_enable = 1U;
    o_args->__pvcn_587__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_593__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_593__dram_pitch_enable = 1U;
    o_args->__pvcn_593__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_598__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_598__dram_pitch_enable = 1U;
    o_args->__pvcn_598__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_604__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_604__dram_pitch_enable = 1U;
    o_args->__pvcn_604__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_609__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_609__dram_pitch_enable = 1U;
    o_args->__pvcn_609__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_615__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_615__dram_pitch_enable = 1U;
    o_args->__pvcn_615__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_620__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_620__dram_pitch_enable = 1U;
    o_args->__pvcn_620__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_626__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_626__dram_pitch_enable = 1U;
    o_args->__pvcn_626__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_631__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_631__dram_pitch_enable = 1U;
    o_args->__pvcn_631__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_637__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_637__dram_pitch_enable = 1U;
    o_args->__pvcn_637__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_642__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_642__dram_pitch_enable = 1U;
    o_args->__pvcn_642__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_648__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_648__dram_pitch_enable = 1U;
    o_args->__pvcn_648__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_653__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_653__dram_pitch_enable = 1U;
    o_args->__pvcn_653__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_659__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_659__dram_pitch_enable = 1U;
    o_args->__pvcn_659__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_664__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_664__dram_pitch_enable = 1U;
    o_args->__pvcn_664__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_670__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_670__dram_pitch_enable = 1U;
    o_args->__pvcn_670__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_675__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_675__dram_pitch_enable = 1U;
    o_args->__pvcn_675__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_681__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_681__dram_pitch_enable = 1U;
    o_args->__pvcn_681__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_686__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_686__dram_pitch_enable = 1U;
    o_args->__pvcn_686__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_692__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_692__dram_pitch_enable = 1U;
    o_args->__pvcn_692__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_703__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_703__dram_pitch_enable = 1U;
    o_args->__pvcn_703__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_714__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_714__dram_pitch_enable = 1U;
    o_args->__pvcn_714__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_719__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_719__dram_pitch_enable = 1U;
    o_args->__pvcn_719__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_725__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_725__dram_pitch_enable = 1U;
    o_args->__pvcn_725__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_755__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_755__dram_pitch_enable = 1U;
    o_args->__pvcn_755__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_756__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_756__dram_pitch_enable = 1U;
    o_args->__pvcn_756__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_757__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_757__dram_pitch_enable = 1U;
    o_args->__pvcn_757__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_789__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_789__dram_pitch_enable = 1U;
    o_args->__pvcn_789__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_800__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_800__dram_pitch_enable = 1U;
    o_args->__pvcn_800__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_811__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_811__dram_pitch_enable = 1U;
    o_args->__pvcn_811__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_822__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_822__dram_pitch_enable = 1U;
    o_args->__pvcn_822__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_833__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_833__dram_pitch_enable = 1U;
    o_args->__pvcn_833__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_844__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_844__dram_pitch_enable = 1U;
    o_args->__pvcn_844__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_855__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_855__dram_pitch_enable = 1U;
    o_args->__pvcn_855__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_866__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_866__dram_pitch_enable = 1U;
    o_args->__pvcn_866__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_877__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_877__dram_pitch_enable = 1U;
    o_args->__pvcn_877__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_888__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_888__dram_pitch_enable = 1U;
    o_args->__pvcn_888__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_899__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_899__dram_pitch_enable = 1U;
    o_args->__pvcn_899__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_910__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_910__dram_pitch_enable = 1U;
    o_args->__pvcn_910__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_925__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_925__dram_pitch_enable = 1U;
    o_args->__pvcn_925__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_926__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_926__dram_pitch_enable = 1U;
    o_args->__pvcn_926__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_927__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_927__dram_pitch_enable = 1U;
    o_args->__pvcn_927__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_928__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_928__dram_pitch_enable = 1U;
    o_args->__pvcn_928__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_929__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_929__dram_pitch_enable = 1U;
    o_args->__pvcn_929__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_930__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_930__dram_pitch_enable = 1U;
    o_args->__pvcn_930__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_963__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_963__dram_pitch_enable = 1U;
    o_args->__pvcn_963__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_974__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_974__dram_pitch_enable = 1U;
    o_args->__pvcn_974__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_985__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_985__dram_pitch_enable = 1U;
    o_args->__pvcn_985__dram_pitch_value = 64U;
#endif
#ifdef __PVCN_996__DRAM_PITCH_MODIFIABLE
    o_args->__pvcn_996__dram_pitch_enable = 1U;
    o_args->__pvcn_996__dram_pitch_value = 64U;
#endif
#ifdef __VCN_2726__DRAM_PITCH_MODIFIABLE
    o_args->__vcn_2726__dram_pitch_enable = 1U;
    o_args->__vcn_2726__dram_pitch_value = 64U;
#endif
#ifdef CONV2_1__SEP_____BN_CONV2_1__SEP__SCALE_MUL___MULI___14_____BN_CONV2_1__SEP__SCALE_MUL_____SCALE___MULI___336_DRAM_PITCH_MODIFIABLE
    o_args->conv2_1__sep_____bn_conv2_1__sep__scale_mul___muli___14_____bn_conv2_1__sep__scale_mul_____scale___muli___336_dram_pitch_enable = 1U;
    o_args->conv2_1__sep_____bn_conv2_1__sep__scale_mul___muli___14_____bn_conv2_1__sep__scale_mul_____scale___muli___336_dram_pitch_value = 64U;
#endif
#ifdef CONV3_1__DW_____BN_CONV3_1__DW__SCALE_MUL___MULI___23_____BN_CONV3_1__DW__SCALE_MUL_____SCALE___RELU3_1__DW_DRAM_PITCH_MODIFIABLE
    o_args->conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_dram_pitch_enable = 1U;
    o_args->conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_dram_pitch_value = 64U;
#endif
#ifdef CONV4_1__SEP_____BN_CONV4_1__SEP__SCALE_MUL___MULI___38_____BN_CONV4_1__SEP__SCALE_MUL_____SCALE___MULI___344_DRAM_PITCH_MODIFIABLE
    o_args->conv4_1__sep_____bn_conv4_1__sep__scale_mul___muli___38_____bn_conv4_1__sep__scale_mul_____scale___muli___344_dram_pitch_enable = 1U;
    o_args->conv4_1__sep_____bn_conv4_1__sep__scale_mul___muli___38_____bn_conv4_1__sep__scale_mul_____scale___muli___344_dram_pitch_value = 64U;
#endif
#ifdef CONV4_2__SEP_____BN_CONV4_2__SEP__SCALE_MUL___MULI___44_____BN_CONV4_2__SEP__SCALE_MUL_____SCALE___RELU4_2__SEP_DRAM_PITCH_MODIFIABLE
    o_args->conv4_2__sep_____bn_conv4_2__sep__scale_mul___muli___44_____bn_conv4_2__sep__scale_mul_____scale___relu4_2__sep_dram_pitch_enable = 1U;
    o_args->conv4_2__sep_____bn_conv4_2__sep__scale_mul___muli___44_____bn_conv4_2__sep__scale_mul_____scale___relu4_2__sep_dram_pitch_value = 64U;
#endif
#ifdef CONV5_1__SEP_____BN_CONV5_1__SEP__SCALE_MUL___MULI___50_____BN_CONV5_1__SEP__SCALE_MUL_____SCALE___RELU5_1__SEP_DRAM_PITCH_MODIFIABLE
    o_args->conv5_1__sep_____bn_conv5_1__sep__scale_mul___muli___50_____bn_conv5_1__sep__scale_mul_____scale___relu5_1__sep_dram_pitch_enable = 1U;
    o_args->conv5_1__sep_____bn_conv5_1__sep__scale_mul___muli___50_____bn_conv5_1__sep__scale_mul_____scale___relu5_1__sep_dram_pitch_value = 64U;
#endif
#ifdef CONV5_2__SEP_____BN_CONV5_2__SEP__SCALE_MUL___MULI___56_____BN_CONV5_2__SEP__SCALE_MUL_____SCALE___RELU5_2__SEP_DRAM_PITCH_MODIFIABLE
    o_args->conv5_2__sep_____bn_conv5_2__sep__scale_mul___muli___56_____bn_conv5_2__sep__scale_mul_____scale___relu5_2__sep_dram_pitch_enable = 1U;
    o_args->conv5_2__sep_____bn_conv5_2__sep__scale_mul___muli___56_____bn_conv5_2__sep__scale_mul_____scale___relu5_2__sep_dram_pitch_value = 64U;
#endif
#ifdef CONV5_4__SEP_____BN_CONV5_4__SEP__SCALE_MUL___MULI___68_____BN_CONV5_4__SEP__SCALE_MUL_____SCALE___RELU5_4__SEP_DRAM_PITCH_MODIFIABLE
    o_args->conv5_4__sep_____bn_conv5_4__sep__scale_mul___muli___68_____bn_conv5_4__sep__scale_mul_____scale___relu5_4__sep_dram_pitch_enable = 1U;
    o_args->conv5_4__sep_____bn_conv5_4__sep__scale_mul___muli___68_____bn_conv5_4__sep__scale_mul_____scale___relu5_4__sep_dram_pitch_value = 64U;
#endif
#ifdef CONV5_5__SEP_____BN_CONV5_5__SEP__SCALE_MUL___MULI___74_____BN_CONV5_5__SEP__SCALE_MUL_____SCALE___RELU5_5__SEP_DRAM_PITCH_MODIFIABLE
    o_args->conv5_5__sep_____bn_conv5_5__sep__scale_mul___muli___74_____bn_conv5_5__sep__scale_mul_____scale___relu5_5__sep_dram_pitch_enable = 1U;
    o_args->conv5_5__sep_____bn_conv5_5__sep__scale_mul___muli___74_____bn_conv5_5__sep__scale_mul_____scale___relu5_5__sep_dram_pitch_value = 64U;
#endif
#ifdef CONV5_5_MBOX_CONF___CONV2I___76______LOC___CONV2I___136______MULI___370___1_DRAM_PITCH_MODIFIABLE
    o_args->conv5_5_mbox_conf___conv2i___76______loc___conv2i___136______muli___370___1_dram_pitch_enable = 1U;
    o_args->conv5_5_mbox_conf___conv2i___76______loc___conv2i___136______muli___370___1_dram_pitch_value = 64U;
#endif
#ifdef CONV5_5_MBOX_CONF___CONV2I___76______LOC___CONV2I___136___CROP___320_DRAM_PITCH_MODIFIABLE
    o_args->conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___320_dram_pitch_enable = 1U;
    o_args->conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___320_dram_pitch_value = 64U;
#endif
#ifdef CONV5_5_MBOX_CONF___CONV2I___76______LOC___CONV2I___136___CROP___321_DRAM_PITCH_MODIFIABLE
    o_args->conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___321_dram_pitch_enable = 1U;
    o_args->conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___321_dram_pitch_value = 64U;
#endif
#ifdef CONV5_5_MBOX_CONF_FLAT_DRAM_PITCH_MODIFIABLE
    o_args->conv5_5_mbox_conf_flat_dram_pitch_enable = 1U;
    o_args->conv5_5_mbox_conf_flat_dram_pitch_value = 64U;
#endif
#ifdef CONV5_5_MBOX_LOC_FLAT_DRAM_PITCH_MODIFIABLE
    o_args->conv5_5_mbox_loc_flat_dram_pitch_enable = 1U;
    o_args->conv5_5_mbox_loc_flat_dram_pitch_value = 64U;
#endif
#ifdef CONV5_6__DW_____BN_CONV5_6__DW__SCALE_MUL___MULI___79_____BN_CONV5_6__DW__SCALE_MUL_____SCALE___MULI___358_DRAM_PITCH_MODIFIABLE
    o_args->conv5_6__dw_____bn_conv5_6__dw__scale_mul___muli___79_____bn_conv5_6__dw__scale_mul_____scale___muli___358_dram_pitch_enable = 1U;
    o_args->conv5_6__dw_____bn_conv5_6__dw__scale_mul___muli___79_____bn_conv5_6__dw__scale_mul_____scale___muli___358_dram_pitch_value = 64U;
#endif
#ifdef CONV5_6__DW_____BN_CONV5_6__DW__SCALE_MUL___MULI___79_____BN_CONV5_6__DW__SCALE_MUL_____SCALE___RELU5_6__DW_DRAM_PITCH_MODIFIABLE
    o_args->conv5_6__dw_____bn_conv5_6__dw__scale_mul___muli___79_____bn_conv5_6__dw__scale_mul_____scale___relu5_6__dw_dram_pitch_enable = 1U;
    o_args->conv5_6__dw_____bn_conv5_6__dw__scale_mul___muli___79_____bn_conv5_6__dw__scale_mul_____scale___relu5_6__dw_dram_pitch_value = 64U;
#endif
#ifdef CONV6__DW_____BN_CONV6__DW__SCALE_MUL___MULI___85_____BN_CONV6__DW__SCALE_MUL_____SCALE___MULI___360_DRAM_PITCH_MODIFIABLE
    o_args->conv6__dw_____bn_conv6__dw__scale_mul___muli___85_____bn_conv6__dw__scale_mul_____scale___muli___360_dram_pitch_enable = 1U;
    o_args->conv6__dw_____bn_conv6__dw__scale_mul___muli___85_____bn_conv6__dw__scale_mul_____scale___muli___360_dram_pitch_value = 64U;
#endif
#ifdef CONV6__SEP_____BN_CONV6__SEP__SCALE_MUL___MULI___88_____BN_CONV6__SEP__SCALE_MUL_____SCALE___RELU6__SEP_DRAM_PITCH_MODIFIABLE
    o_args->conv6__sep_____bn_conv6__sep__scale_mul___muli___88_____bn_conv6__sep__scale_mul_____scale___relu6__sep_dram_pitch_enable = 1U;
    o_args->conv6__sep_____bn_conv6__sep__scale_mul___muli___88_____bn_conv6__sep__scale_mul_____scale___relu6__sep_dram_pitch_value = 64U;
#endif
#ifdef CONV6_MBOX_CONF___CONV2I___90______7_1_____BN_CONV7_1__SCALE_MUL___MULI___93_____BN_CONV7_1__SCALE_MUL_____SCALE___LOC___CONV2I___138______MULI___367___8___9_DRAM_PITCH_MODIFIABLE
    o_args->conv6_mbox_conf___conv2i___90______7_1_____bn_conv7_1__scale_mul___muli___93_____bn_conv7_1__scale_mul_____scale___loc___conv2i___138______muli___367___8___9_dram_pitch_enable = 1U;
    o_args->conv6_mbox_conf___conv2i___90______7_1_____bn_conv7_1__scale_mul___muli___93_____bn_conv7_1__scale_mul_____scale___loc___conv2i___138______muli___367___8___9_dram_pitch_value = 64U;
#endif
#ifdef CONV6_MBOX_LOC_FLAT_DRAM_PITCH_MODIFIABLE
    o_args->conv6_mbox_loc_flat_dram_pitch_enable = 1U;
    o_args->conv6_mbox_loc_flat_dram_pitch_value = 64U;
#endif
#ifdef CONV7_2_____BN_CONV7_2__SCALE_MUL___MULI___96_____BN_CONV7_2__SCALE_MUL_____SCALE___RELU7_2_DRAM_PITCH_MODIFIABLE
    o_args->conv7_2_____bn_conv7_2__scale_mul___muli___96_____bn_conv7_2__scale_mul_____scale___relu7_2_dram_pitch_enable = 1U;
    o_args->conv7_2_____bn_conv7_2__scale_mul___muli___96_____bn_conv7_2__scale_mul_____scale___relu7_2_dram_pitch_value = 64U;
#endif
#ifdef CONV7_2_MBOX_CONF_FLAT_DRAM_PITCH_MODIFIABLE
    o_args->conv7_2_mbox_conf_flat_dram_pitch_enable = 1U;
    o_args->conv7_2_mbox_conf_flat_dram_pitch_value = 64U;
#endif
#ifdef CONV7_2_MBOX_LOC_FLAT_DRAM_PITCH_MODIFIABLE
    o_args->conv7_2_mbox_loc_flat_dram_pitch_enable = 1U;
    o_args->conv7_2_mbox_loc_flat_dram_pitch_value = 64U;
#endif
#ifdef CONV8_2_____BN_CONV8_2__SCALE_MUL___MULI___104_____BN_CONV8_2__SCALE_MUL_____SCALE___RELU8_2_DRAM_PITCH_MODIFIABLE
    o_args->conv8_2_____bn_conv8_2__scale_mul___muli___104_____bn_conv8_2__scale_mul_____scale___relu8_2_dram_pitch_enable = 1U;
    o_args->conv8_2_____bn_conv8_2__scale_mul___muli___104_____bn_conv8_2__scale_mul_____scale___relu8_2_dram_pitch_value = 64U;
#endif
#ifdef CONV8_2_MBOX_CONF_FLAT_DRAM_PITCH_MODIFIABLE
    o_args->conv8_2_mbox_conf_flat_dram_pitch_enable = 1U;
    o_args->conv8_2_mbox_conf_flat_dram_pitch_value = 64U;
#endif
#ifdef CONV8_2_MBOX_LOC_FLAT_DRAM_PITCH_MODIFIABLE
    o_args->conv8_2_mbox_loc_flat_dram_pitch_enable = 1U;
    o_args->conv8_2_mbox_loc_flat_dram_pitch_value = 64U;
#endif
#ifdef CONV9_2_____BN_CONV9_2__SCALE_MUL___MULI___112_____BN_CONV9_2__SCALE_MUL_____SCALE___RELU9_2_DRAM_PITCH_MODIFIABLE
    o_args->conv9_2_____bn_conv9_2__scale_mul___muli___112_____bn_conv9_2__scale_mul_____scale___relu9_2_dram_pitch_enable = 1U;
    o_args->conv9_2_____bn_conv9_2__scale_mul___muli___112_____bn_conv9_2__scale_mul_____scale___relu9_2_dram_pitch_value = 64U;
#endif
#ifdef CONV9_2_MBOX_CONF_FLAT_DRAM_PITCH_MODIFIABLE
    o_args->conv9_2_mbox_conf_flat_dram_pitch_enable = 1U;
    o_args->conv9_2_mbox_conf_flat_dram_pitch_value = 64U;
#endif
#ifdef CONV9_2_MBOX_LOC_FLAT_DRAM_PITCH_MODIFIABLE
    o_args->conv9_2_mbox_loc_flat_dram_pitch_enable = 1U;
    o_args->conv9_2_mbox_loc_flat_dram_pitch_value = 64U;
#endif
#ifdef DATA_DRAM_PITCH_MODIFIABLE
    o_args->data_dram_pitch_enable = 1U;
    o_args->data_dram_pitch_value = 320U;
#endif
#ifdef DATA_UV_DRAM_PITCH_MODIFIABLE
    o_args->data_uv_dram_pitch_enable = 1U;
    o_args->data_uv_dram_pitch_value = 320U;
#endif
#ifdef MBOX_CONF_FLATTEN__DRAM_PITCH_MODIFIABLE
    o_args->mbox_conf_flatten__dram_pitch_enable = 1U;
    o_args->mbox_conf_flatten__dram_pitch_value = 64U;
#endif
#ifdef MBOX_CONF_FLATTEN_DRAM_PITCH_MODIFIABLE
    o_args->mbox_conf_flatten_dram_pitch_enable = 1U;
    o_args->mbox_conf_flatten_dram_pitch_value = 53696U;
#endif
#ifdef MBOX_LOC_DRAM_PITCH_MODIFIABLE
    o_args->mbox_loc_dram_pitch_enable = 1U;
    o_args->mbox_loc_dram_pitch_value = 30720U;
#endif
#ifdef RELU7_1_DRAM_PITCH_MODIFIABLE
    o_args->relu7_1_dram_pitch_enable = 1U;
    o_args->relu7_1_dram_pitch_value = 64U;
#endif

    return status;
} // end of mnet_ssd_adas_flex_pic_ag_reset()


/********************* USED BY MNET_SSD_ADAS_FLEX_PIC_AG_INIT() **********************/
/* Wrapper function for cvtable_find() */
inline errcode_enum_t mnet_ssd_adas_flex_pic_ag_read_cvtable (
    mnet_ssd_adas_flex_pic_ag_ips_t *ctxt
) {
    unsigned int split_id = 0; // size_t
    int32_t tbar_sz;
    errcode_enum_t status = ERRCODE_NONE;
    struct mnet_ssd_adas_flex_pic_ag_split_context *split_ctxt = NULL;
    char dag_key[] = "MNET_SSD_ADAS_FLEX_PIC_AG_---_DAG";

    for (; split_id < MNET_SSD_ADAS_FLEX_PIC_AG_SPLIT_COUNT; split_id++) {
        tbar_sz = 0;
        split_ctxt = &ctxt->split_ctxt[split_id];

#ifdef CVTASK_PRINT
        cvtask_printf(LVL_DEBUG, "  >> #Split-%u# Extracting DAG base address "
            "from CV Table Archive", split_id, 0, 0, 0, 0);
#endif

        dag_key[26] = '0' + ((split_id / 100U) % 10U);
        dag_key[27] = '0' + ((split_id / 10U ) % 10U);
        dag_key[28] = '0' + ((split_id / 1U  ) % 10U);

        // extracting DAG base from CV Table Archive
        status = cvtable_find(
            dag_key,
            (const void **)&split_ctxt->dram_dag_base,
            &tbar_sz
        );
        if (is_err(status))
        {
            cvtask_printf(LVL_DEBUG, "  >> cvtable_find() key MNET_SSD_ADAS_FLEX_PIC_AG_"
                "%c%c%c_DAG look up failed for split(%u)", 
                '0' + ((split_id / 100U) % 10U),
                '0' + ((split_id / 10U ) % 10U),
                '0' + ((split_id / 1U  ) % 10U),
                split_id, 0);
            return ERRCODE_GENERIC;
        }
        else if(tbar_sz != dag_img_sizes[split_id])
        {
            cvtask_printf(LVL_DEBUG, "  >> size of dag from table archive(%u) "
                "!= size (%u) in header file for split(%u)",
                tbar_sz, dag_img_sizes[split_id], split_id, 0, 0);
            return ERRCODE_GENERIC;
        }
#ifdef CVTASK_PRINT
        else
            cvtask_printf(LVL_DEBUG, "  >> * extracted DAG binary "
                "@address(0x%08X)", split_ctxt->dram_dag_base, 0, 0, 0, 0);
#endif

    }

    return status;
} // end of mnet_ssd_adas_flex_pic_ag_read_cvtable()


/********************** USED BY MNET_SSD_ADAS_FLEX_PIC_AG_RUN() **********************/
/* Load machine code into VP */
inline void mnet_ssd_adas_flex_pic_ag_load_split(
    struct mnet_ssd_adas_flex_pic_ag_split_context *split_ctxt,
    uint32_t split_id
) {

    //vp_load_dag(
    //    split_ctxt->dram_dag_base,
    //    vmem_dag_bases[split_id],
    //    dag_img_sizes[split_id],
    //    0
    //);
    uint32_t dag_dbase = split_ctxt->dram_dag_base;
    uint32_t dag_vbase = vmem_dag_bases[split_id];
    uint32_t dag_size  = dag_img_sizes[split_id];

    // vp_load_dag()
    coproc_poke_ldq_word(dag_vbase,            0xFD4U);
    coproc_poke_ldq_word(dag_size + 256U - 1U, 0xFE0U);
    coproc_poke_ldq_word(dag_dbase,            0xFE8U); 
    coproc_dmald(0xFD0U);

    vp_dummy_run();

    return;
} // end of mnet_ssd_adas_flex_pic_ag_load_split()

/* Modify input and output buffer addresses of a particular split DAG */
inline void mnet_ssd_adas_flex_pic_ag_poke_split(
    mnet_ssd_adas_flex_pic_ag_required_fields_t *r_args,
    mnet_ssd_adas_flex_pic_ag_optional_fields_t *o_args,
    struct mnet_ssd_adas_flex_pic_ag_split_context *split_ctxt,
    uint32_t split_id) {
    uint32_t dag_base = (uint32_t) r_args->instance_private_uncached_storage;
    uint32_t dagbin_base = vmem_dagbin_bases[split_id];

    // Avoid unused variable warning
    (void) dag_base;

    switch (split_id) {
        case 0:
            /*** poke() calls ***/
            /* Primary inputs */
            // Poke HMB input buffer: data
            coproc_poke_word(r_args->data_addr + 0, dagbin_base + 636U);

            // Poke HMB input buffer: data_uv
            coproc_poke_word(r_args->data_uv_addr + 0, dagbin_base + 676U);

            /* Intermediate buffers */
            // Poke HMB intermediate output buffer: conv2_1__sep_____bn_conv2_1__sep__scale_mul___muli___14_____bn_conv2_1__sep__scale_mul_____scale___muli___336
            coproc_poke_word(dag_base + 0,
                dagbin_base + 1400U);

            /* Pokable fields */
#ifdef DATA_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: data_dram_pitch
            if (o_args->data_dram_pitch_enable)
                coproc_poke_half(o_args->data_dram_pitch_value, dagbin_base + 640U);

#endif

#ifdef DATA_UV_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: data_uv_dram_pitch
            if (o_args->data_uv_dram_pitch_enable)
                coproc_poke_half(o_args->data_uv_dram_pitch_value, dagbin_base + 680U);

#endif

#ifdef CONV2_1__SEP_____BN_CONV2_1__SEP__SCALE_MUL___MULI___14_____BN_CONV2_1__SEP__SCALE_MUL_____SCALE___MULI___336_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv2_1__sep_____bn_conv2_1__sep__scale_mul___muli___14_____bn_conv2_1__sep__scale_mul_____scale___muli___336_dram_pitch
            if (o_args->conv2_1__sep_____bn_conv2_1__sep__scale_mul___muli___14_____bn_conv2_1__sep__scale_mul_____scale___muli___336_dram_pitch_enable)
                coproc_poke_half(o_args->conv2_1__sep_____bn_conv2_1__sep__scale_mul___muli___14_____bn_conv2_1__sep__scale_mul_____scale___muli___336_dram_pitch_value, dagbin_base + 1404U);

#endif

            /*** dmald() calls ***/
            break;
        case 1:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: conv2_1__sep_____bn_conv2_1__sep__scale_mul___muli___14_____bn_conv2_1__sep__scale_mul_____scale___muli___336
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 28U);

            // Poke HMB intermediate output buffer: __pvcn_332_
            coproc_poke_word(dag_base + 1459200,
                dagbin_base + 784U);

            // Poke HMB intermediate output buffer: __pvcn_343_
            coproc_poke_word(dag_base + 1555200,
                dagbin_base + 892U);

            // Poke HMB intermediate output buffer: __pvcn_354_
            coproc_poke_word(dag_base + 1639232,
                dagbin_base + 1000U);

            // Poke HMB intermediate output buffer: __pvcn_365_
            coproc_poke_word(dag_base + 1771264,
                dagbin_base + 1108U);

            // Poke HMB intermediate output buffer: __pvcn_376_
            coproc_poke_word(dag_base + 1879296,
                dagbin_base + 1216U);

            // Poke HMB intermediate output buffer: __pvcn_387_
            coproc_poke_word(dag_base + 1963328,
                dagbin_base + 1324U);

            // Poke HMB intermediate output buffer: __pvcn_398_
            coproc_poke_word(dag_base + 2059328,
                dagbin_base + 1432U);

            // Poke HMB intermediate output buffer: __pvcn_409_
            coproc_poke_word(dag_base + 2155328,
                dagbin_base + 1540U);

            /* Pokable fields */
#ifdef CONV2_1__SEP_____BN_CONV2_1__SEP__SCALE_MUL___MULI___14_____BN_CONV2_1__SEP__SCALE_MUL_____SCALE___MULI___336_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv2_1__sep_____bn_conv2_1__sep__scale_mul___muli___14_____bn_conv2_1__sep__scale_mul_____scale___muli___336_dram_pitch
            if (o_args->conv2_1__sep_____bn_conv2_1__sep__scale_mul___muli___14_____bn_conv2_1__sep__scale_mul_____scale___muli___336_dram_pitch_enable)
                coproc_poke_half_offset(o_args->conv2_1__sep_____bn_conv2_1__sep__scale_mul___muli___14_____bn_conv2_1__sep__scale_mul_____scale___muli___336_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef __PVCN_332__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_332__dram_pitch
            if (o_args->__pvcn_332__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_332__dram_pitch_value, dagbin_base + 788U);

#endif

#ifdef __PVCN_343__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_343__dram_pitch
            if (o_args->__pvcn_343__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_343__dram_pitch_value, dagbin_base + 896U);

#endif

#ifdef __PVCN_354__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_354__dram_pitch
            if (o_args->__pvcn_354__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_354__dram_pitch_value, dagbin_base + 1004U);

#endif

#ifdef __PVCN_365__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_365__dram_pitch
            if (o_args->__pvcn_365__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_365__dram_pitch_value, dagbin_base + 1112U);

#endif

#ifdef __PVCN_376__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_376__dram_pitch
            if (o_args->__pvcn_376__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_376__dram_pitch_value, dagbin_base + 1220U);

#endif

#ifdef __PVCN_387__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_387__dram_pitch
            if (o_args->__pvcn_387__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_387__dram_pitch_value, dagbin_base + 1328U);

#endif

#ifdef __PVCN_398__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_398__dram_pitch
            if (o_args->__pvcn_398__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_398__dram_pitch_value, dagbin_base + 1436U);

#endif

#ifdef __PVCN_409__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_409__dram_pitch
            if (o_args->__pvcn_409__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_409__dram_pitch_value, dagbin_base + 1544U);

#endif

            /*** dmald() calls ***/
            break;
        case 2:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: __pvcn_332_
            coproc_poke_word_offset(dag_base + 1459200,
                dagbin_base, 268U);

            // Poke HMB intermediate input buffer: __pvcn_343_
            coproc_poke_word_offset(dag_base + 1555200,
                dagbin_base, 304U);

            // Poke HMB intermediate input buffer: __pvcn_354_
            coproc_poke_word_offset(dag_base + 1639232,
                dagbin_base, 188U);

            // Poke HMB intermediate input buffer: __pvcn_365_
            coproc_poke_word_offset(dag_base + 1771264,
                dagbin_base, 228U);

            // Poke HMB intermediate input buffer: __pvcn_376_
            coproc_poke_word_offset(dag_base + 1879296,
                dagbin_base, 148U);

            // Poke HMB intermediate input buffer: __pvcn_387_
            coproc_poke_word_offset(dag_base + 1963328,
                dagbin_base, 28U);

            // Poke HMB intermediate input buffer: __pvcn_398_
            coproc_poke_word_offset(dag_base + 2059328,
                dagbin_base, 68U);

            // Poke HMB intermediate input buffer: __pvcn_409_
            coproc_poke_word_offset(dag_base + 2155328,
                dagbin_base, 108U);

            // Poke HMB intermediate output buffer: conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw
            coproc_poke_word(dag_base + 0,
                dagbin_base + 1504U);

            /* Pokable fields */
#ifdef __PVCN_387__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_387__dram_pitch
            if (o_args->__pvcn_387__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_387__dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef __PVCN_398__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_398__dram_pitch
            if (o_args->__pvcn_398__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_398__dram_pitch_value, dagbin_base, 
                    72U);

#endif

#ifdef __PVCN_409__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_409__dram_pitch
            if (o_args->__pvcn_409__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_409__dram_pitch_value, dagbin_base, 
                    112U);

#endif

#ifdef __PVCN_376__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_376__dram_pitch
            if (o_args->__pvcn_376__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_376__dram_pitch_value, dagbin_base, 
                    152U);

#endif

#ifdef __PVCN_354__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_354__dram_pitch
            if (o_args->__pvcn_354__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_354__dram_pitch_value, dagbin_base, 
                    192U);

#endif

#ifdef __PVCN_365__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_365__dram_pitch
            if (o_args->__pvcn_365__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_365__dram_pitch_value, dagbin_base, 
                    232U);

#endif

#ifdef __PVCN_332__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_332__dram_pitch
            if (o_args->__pvcn_332__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_332__dram_pitch_value, dagbin_base, 
                    272U);

#endif

#ifdef __PVCN_343__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_343__dram_pitch
            if (o_args->__pvcn_343__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_343__dram_pitch_value, dagbin_base, 
                    308U);

#endif

#ifdef CONV3_1__DW_____BN_CONV3_1__DW__SCALE_MUL___MULI___23_____BN_CONV3_1__DW__SCALE_MUL_____SCALE___RELU3_1__DW_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_dram_pitch
            if (o_args->conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_dram_pitch_enable)
                coproc_poke_half(o_args->conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_dram_pitch_value, dagbin_base + 1508U);

#endif

            /*** dmald() calls ***/
            break;
        case 3:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw
            coproc_poke_word(dag_base + 0,
                dagbin_base + 1248U);

            // Poke HMB intermediate output buffer: __pvcn_549_
            coproc_poke_word(dag_base + 768000,
                dagbin_base + 1452U);

            // Poke HMB intermediate output buffer: __pvcn_560_
            coproc_poke_word(dag_base + 789312,
                dagbin_base + 1508U);

            // Poke HMB intermediate output buffer: __pvcn_571_
            coproc_poke_word(dag_base + 810624,
                dagbin_base + 1564U);

            // Poke HMB intermediate output buffer: __pvcn_582_
            coproc_poke_word(dag_base + 831936,
                dagbin_base + 1620U);

            // Poke HMB intermediate output buffer: __pvcn_593_
            coproc_poke_word(dag_base + 856256,
                dagbin_base + 1676U);

            // Poke HMB intermediate output buffer: __pvcn_604_
            coproc_poke_word(dag_base + 877568,
                dagbin_base + 1732U);

            // Poke HMB intermediate output buffer: __pvcn_615_
            coproc_poke_word(dag_base + 901888,
                dagbin_base + 1788U);

            // Poke HMB intermediate output buffer: __pvcn_626_
            coproc_poke_word(dag_base + 929280,
                dagbin_base + 1844U);

            // Poke HMB intermediate output buffer: __pvcn_637_
            coproc_poke_word(dag_base + 953600,
                dagbin_base + 1900U);

            // Poke HMB intermediate output buffer: __pvcn_648_
            coproc_poke_word(dag_base + 977920,
                dagbin_base + 1956U);

            // Poke HMB intermediate output buffer: __pvcn_659_
            coproc_poke_word(dag_base + 999232,
                dagbin_base + 2012U);

            // Poke HMB intermediate output buffer: __pvcn_670_
            coproc_poke_word(dag_base + 1020544,
                dagbin_base + 2068U);

            // Poke HMB intermediate output buffer: __pvcn_681_
            coproc_poke_word(dag_base + 1044864,
                dagbin_base + 2124U);

            // Poke HMB intermediate output buffer: __pvcn_692_
            coproc_poke_word(dag_base + 1072256,
                dagbin_base + 2180U);

            // Poke HMB intermediate output buffer: __vcn_2726_
            coproc_poke_word(dag_base + 1093568,
                dagbin_base + 2228U);

            /* Pokable fields */
#ifdef CONV3_1__DW_____BN_CONV3_1__DW__SCALE_MUL___MULI___23_____BN_CONV3_1__DW__SCALE_MUL_____SCALE___RELU3_1__DW_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_dram_pitch
            if (o_args->conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_dram_pitch_enable)
                coproc_poke_half(o_args->conv3_1__dw_____bn_conv3_1__dw__scale_mul___muli___23_____bn_conv3_1__dw__scale_mul_____scale___relu3_1__dw_dram_pitch_value, dagbin_base + 1252U);

#endif

#ifdef __PVCN_549__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_549__dram_pitch
            if (o_args->__pvcn_549__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_549__dram_pitch_value, dagbin_base + 1456U);

#endif

#ifdef __PVCN_560__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_560__dram_pitch
            if (o_args->__pvcn_560__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_560__dram_pitch_value, dagbin_base + 1512U);

#endif

#ifdef __PVCN_571__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_571__dram_pitch
            if (o_args->__pvcn_571__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_571__dram_pitch_value, dagbin_base + 1568U);

#endif

#ifdef __PVCN_582__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_582__dram_pitch
            if (o_args->__pvcn_582__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_582__dram_pitch_value, dagbin_base + 1624U);

#endif

#ifdef __PVCN_593__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_593__dram_pitch
            if (o_args->__pvcn_593__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_593__dram_pitch_value, dagbin_base + 1680U);

#endif

#ifdef __PVCN_604__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_604__dram_pitch
            if (o_args->__pvcn_604__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_604__dram_pitch_value, dagbin_base + 1736U);

#endif

#ifdef __PVCN_615__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_615__dram_pitch
            if (o_args->__pvcn_615__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_615__dram_pitch_value, dagbin_base + 1792U);

#endif

#ifdef __PVCN_626__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_626__dram_pitch
            if (o_args->__pvcn_626__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_626__dram_pitch_value, dagbin_base + 1848U);

#endif

#ifdef __PVCN_637__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_637__dram_pitch
            if (o_args->__pvcn_637__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_637__dram_pitch_value, dagbin_base + 1904U);

#endif

#ifdef __PVCN_648__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_648__dram_pitch
            if (o_args->__pvcn_648__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_648__dram_pitch_value, dagbin_base + 1960U);

#endif

#ifdef __PVCN_659__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_659__dram_pitch
            if (o_args->__pvcn_659__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_659__dram_pitch_value, dagbin_base + 2016U);

#endif

#ifdef __PVCN_670__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_670__dram_pitch
            if (o_args->__pvcn_670__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_670__dram_pitch_value, dagbin_base + 2072U);

#endif

#ifdef __PVCN_681__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_681__dram_pitch
            if (o_args->__pvcn_681__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_681__dram_pitch_value, dagbin_base + 2128U);

#endif

#ifdef __PVCN_692__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_692__dram_pitch
            if (o_args->__pvcn_692__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_692__dram_pitch_value, dagbin_base + 2184U);

#endif

#ifdef __VCN_2726__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __vcn_2726__dram_pitch
            if (o_args->__vcn_2726__dram_pitch_enable)
                coproc_poke_half(o_args->__vcn_2726__dram_pitch_value, dagbin_base + 2232U);

#endif

            /*** dmald() calls ***/
            break;
        case 4:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: __pvcn_549_
            coproc_poke_word(dag_base + 768000,
                dagbin_base + 588U);

            // Poke HMB intermediate input buffer: __pvcn_560_
            coproc_poke_word_offset(dag_base + 789312,
                dagbin_base, 468U);

            // Poke HMB intermediate input buffer: __pvcn_571_
            coproc_poke_word_offset(dag_base + 810624,
                dagbin_base, 388U);

            // Poke HMB intermediate input buffer: __pvcn_582_
            coproc_poke_word(dag_base + 831936,
                dagbin_base + 548U);

            // Poke HMB intermediate input buffer: __pvcn_593_
            coproc_poke_word_offset(dag_base + 856256,
                dagbin_base, 508U);

            // Poke HMB intermediate input buffer: __pvcn_604_
            coproc_poke_word_offset(dag_base + 877568,
                dagbin_base, 148U);

            // Poke HMB intermediate input buffer: __pvcn_615_
            coproc_poke_word_offset(dag_base + 901888,
                dagbin_base, 428U);

            // Poke HMB intermediate input buffer: __pvcn_626_
            coproc_poke_word_offset(dag_base + 929280,
                dagbin_base, 108U);

            // Poke HMB intermediate input buffer: __pvcn_637_
            coproc_poke_word_offset(dag_base + 953600,
                dagbin_base, 348U);

            // Poke HMB intermediate input buffer: __pvcn_648_
            coproc_poke_word_offset(dag_base + 977920,
                dagbin_base, 308U);

            // Poke HMB intermediate input buffer: __pvcn_659_
            coproc_poke_word_offset(dag_base + 999232,
                dagbin_base, 228U);

            // Poke HMB intermediate input buffer: __pvcn_670_
            coproc_poke_word_offset(dag_base + 1020544,
                dagbin_base, 268U);

            // Poke HMB intermediate input buffer: __pvcn_681_
            coproc_poke_word_offset(dag_base + 1044864,
                dagbin_base, 68U);

            // Poke HMB intermediate input buffer: __pvcn_692_
            coproc_poke_word_offset(dag_base + 1072256,
                dagbin_base, 28U);

            // Poke HMB intermediate input buffer: __vcn_2726_
            coproc_poke_word_offset(dag_base + 1093568,
                dagbin_base, 188U);

            // Poke HMB intermediate output buffer: __pvcn_543_
            coproc_poke_word(dag_base + 0,
                dagbin_base + 876U);

            // Poke HMB intermediate output buffer: __pvcn_554_
            coproc_poke_word(dag_base + 21312,
                dagbin_base + 924U);

            // Poke HMB intermediate output buffer: __pvcn_565_
            coproc_poke_word(dag_base + 42624,
                dagbin_base + 972U);

            // Poke HMB intermediate output buffer: __pvcn_576_
            coproc_poke_word(dag_base + 63936,
                dagbin_base + 1020U);

            // Poke HMB intermediate output buffer: __pvcn_587_
            coproc_poke_word(dag_base + 88256,
                dagbin_base + 1068U);

            // Poke HMB intermediate output buffer: __pvcn_598_
            coproc_poke_word(dag_base + 109568,
                dagbin_base + 1116U);

            // Poke HMB intermediate output buffer: __pvcn_609_
            coproc_poke_word(dag_base + 133888,
                dagbin_base + 1164U);

            // Poke HMB intermediate output buffer: __pvcn_620_
            coproc_poke_word(dag_base + 161280,
                dagbin_base + 1212U);

            // Poke HMB intermediate output buffer: __pvcn_631_
            coproc_poke_word(dag_base + 185600,
                dagbin_base + 1260U);

            // Poke HMB intermediate output buffer: __pvcn_642_
            coproc_poke_word(dag_base + 209920,
                dagbin_base + 1308U);

            // Poke HMB intermediate output buffer: __pvcn_653_
            coproc_poke_word(dag_base + 231232,
                dagbin_base + 1356U);

            // Poke HMB intermediate output buffer: __pvcn_664_
            coproc_poke_word(dag_base + 252544,
                dagbin_base + 1404U);

            // Poke HMB intermediate output buffer: __pvcn_675_
            coproc_poke_word(dag_base + 276864,
                dagbin_base + 1452U);

            // Poke HMB intermediate output buffer: __pvcn_686_
            coproc_poke_word(dag_base + 304256,
                dagbin_base + 1500U);

            // Poke HMB intermediate output buffer: __pvcn_703_
            coproc_poke_word(dag_base + 325568,
                dagbin_base + 1556U);

            // Poke HMB intermediate output buffer: __pvcn_714_
            coproc_poke_word(dag_base + 346880,
                dagbin_base + 1612U);

            // Poke HMB intermediate output buffer: __pvcn_725_
            coproc_poke_word(dag_base + 371200,
                dagbin_base + 1668U);

            /* Pokable fields */
#ifdef __PVCN_692__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_692__dram_pitch
            if (o_args->__pvcn_692__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_692__dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef __PVCN_681__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_681__dram_pitch
            if (o_args->__pvcn_681__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_681__dram_pitch_value, dagbin_base, 
                    72U);

#endif

#ifdef __PVCN_626__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_626__dram_pitch
            if (o_args->__pvcn_626__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_626__dram_pitch_value, dagbin_base, 
                    112U);

#endif

#ifdef __PVCN_604__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_604__dram_pitch
            if (o_args->__pvcn_604__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_604__dram_pitch_value, dagbin_base, 
                    152U);

#endif

#ifdef __VCN_2726__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __vcn_2726__dram_pitch
            if (o_args->__vcn_2726__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__vcn_2726__dram_pitch_value, dagbin_base, 
                    192U);

#endif

#ifdef __PVCN_659__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_659__dram_pitch
            if (o_args->__pvcn_659__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_659__dram_pitch_value, dagbin_base, 
                    232U);

#endif

#ifdef __PVCN_670__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_670__dram_pitch
            if (o_args->__pvcn_670__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_670__dram_pitch_value, dagbin_base, 
                    272U);

#endif

#ifdef __PVCN_648__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_648__dram_pitch
            if (o_args->__pvcn_648__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_648__dram_pitch_value, dagbin_base, 
                    312U);

#endif

#ifdef __PVCN_637__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_637__dram_pitch
            if (o_args->__pvcn_637__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_637__dram_pitch_value, dagbin_base, 
                    352U);

#endif

#ifdef __PVCN_571__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_571__dram_pitch
            if (o_args->__pvcn_571__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_571__dram_pitch_value, dagbin_base, 
                    392U);

#endif

#ifdef __PVCN_615__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_615__dram_pitch
            if (o_args->__pvcn_615__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_615__dram_pitch_value, dagbin_base, 
                    432U);

#endif

#ifdef __PVCN_560__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_560__dram_pitch
            if (o_args->__pvcn_560__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_560__dram_pitch_value, dagbin_base, 
                    472U);

#endif

#ifdef __PVCN_593__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_593__dram_pitch
            if (o_args->__pvcn_593__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_593__dram_pitch_value, dagbin_base + 512U);

#endif

#ifdef __PVCN_582__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_582__dram_pitch
            if (o_args->__pvcn_582__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_582__dram_pitch_value, dagbin_base + 552U);

#endif

#ifdef __PVCN_549__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_549__dram_pitch
            if (o_args->__pvcn_549__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_549__dram_pitch_value, dagbin_base + 592U);

#endif

#ifdef __PVCN_543__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_543__dram_pitch
            if (o_args->__pvcn_543__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_543__dram_pitch_value, dagbin_base + 880U);

#endif

#ifdef __PVCN_554__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_554__dram_pitch
            if (o_args->__pvcn_554__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_554__dram_pitch_value, dagbin_base + 928U);

#endif

#ifdef __PVCN_565__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_565__dram_pitch
            if (o_args->__pvcn_565__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_565__dram_pitch_value, dagbin_base + 976U);

#endif

#ifdef __PVCN_576__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_576__dram_pitch
            if (o_args->__pvcn_576__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_576__dram_pitch_value, dagbin_base + 1024U);

#endif

#ifdef __PVCN_587__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_587__dram_pitch
            if (o_args->__pvcn_587__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_587__dram_pitch_value, dagbin_base + 1072U);

#endif

#ifdef __PVCN_598__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_598__dram_pitch
            if (o_args->__pvcn_598__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_598__dram_pitch_value, dagbin_base + 1120U);

#endif

#ifdef __PVCN_609__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_609__dram_pitch
            if (o_args->__pvcn_609__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_609__dram_pitch_value, dagbin_base + 1168U);

#endif

#ifdef __PVCN_620__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_620__dram_pitch
            if (o_args->__pvcn_620__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_620__dram_pitch_value, dagbin_base + 1216U);

#endif

#ifdef __PVCN_631__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_631__dram_pitch
            if (o_args->__pvcn_631__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_631__dram_pitch_value, dagbin_base + 1264U);

#endif

#ifdef __PVCN_642__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_642__dram_pitch
            if (o_args->__pvcn_642__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_642__dram_pitch_value, dagbin_base + 1312U);

#endif

#ifdef __PVCN_653__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_653__dram_pitch
            if (o_args->__pvcn_653__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_653__dram_pitch_value, dagbin_base + 1360U);

#endif

#ifdef __PVCN_664__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_664__dram_pitch
            if (o_args->__pvcn_664__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_664__dram_pitch_value, dagbin_base + 1408U);

#endif

#ifdef __PVCN_675__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_675__dram_pitch
            if (o_args->__pvcn_675__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_675__dram_pitch_value, dagbin_base + 1456U);

#endif

#ifdef __PVCN_686__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_686__dram_pitch
            if (o_args->__pvcn_686__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_686__dram_pitch_value, dagbin_base + 1504U);

#endif

#ifdef __PVCN_703__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_703__dram_pitch
            if (o_args->__pvcn_703__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_703__dram_pitch_value, dagbin_base + 1560U);

#endif

#ifdef __PVCN_714__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_714__dram_pitch
            if (o_args->__pvcn_714__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_714__dram_pitch_value, dagbin_base + 1616U);

#endif

#ifdef __PVCN_725__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_725__dram_pitch
            if (o_args->__pvcn_725__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_725__dram_pitch_value, dagbin_base + 1672U);

#endif

            /*** dmald() calls ***/
            break;
        case 5:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: __pvcn_543_
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 28U);

            // Poke HMB intermediate input buffer: __pvcn_554_
            coproc_poke_word(dag_base + 21312,
                dagbin_base + 628U);

            // Poke HMB intermediate input buffer: __pvcn_565_
            coproc_poke_word(dag_base + 42624,
                dagbin_base + 548U);

            // Poke HMB intermediate input buffer: __pvcn_576_
            coproc_poke_word(dag_base + 63936,
                dagbin_base + 588U);

            // Poke HMB intermediate input buffer: __pvcn_587_
            coproc_poke_word_offset(dag_base + 88256,
                dagbin_base, 428U);

            // Poke HMB intermediate input buffer: __pvcn_598_
            coproc_poke_word_offset(dag_base + 109568,
                dagbin_base, 508U);

            // Poke HMB intermediate input buffer: __pvcn_609_
            coproc_poke_word_offset(dag_base + 133888,
                dagbin_base, 348U);

            // Poke HMB intermediate input buffer: __pvcn_620_
            coproc_poke_word_offset(dag_base + 161280,
                dagbin_base, 68U);

            // Poke HMB intermediate input buffer: __pvcn_631_
            coproc_poke_word_offset(dag_base + 185600,
                dagbin_base, 388U);

            // Poke HMB intermediate input buffer: __pvcn_642_
            coproc_poke_word_offset(dag_base + 209920,
                dagbin_base, 468U);

            // Poke HMB intermediate input buffer: __pvcn_653_
            coproc_poke_word_offset(dag_base + 231232,
                dagbin_base, 268U);

            // Poke HMB intermediate input buffer: __pvcn_664_
            coproc_poke_word_offset(dag_base + 252544,
                dagbin_base, 308U);

            // Poke HMB intermediate input buffer: __pvcn_675_
            coproc_poke_word_offset(dag_base + 276864,
                dagbin_base, 188U);

            // Poke HMB intermediate input buffer: __pvcn_686_
            coproc_poke_word_offset(dag_base + 304256,
                dagbin_base, 108U);

            // Poke HMB intermediate input buffer: __pvcn_703_
            coproc_poke_word(dag_base + 325568,
                dagbin_base + 668U);

            // Poke HMB intermediate input buffer: __pvcn_714_
            coproc_poke_word_offset(dag_base + 346880,
                dagbin_base, 228U);

            // Poke HMB intermediate input buffer: __pvcn_725_
            coproc_poke_word_offset(dag_base + 371200,
                dagbin_base, 148U);

            // Poke HMB intermediate output buffer: __pvcn_719_
            coproc_poke_word(dag_base + 389440,
                dagbin_base + 788U);

            // Poke HMB intermediate output buffer: __pvcn_755_
            coproc_poke_word(dag_base + 407680,
                dagbin_base + 1196U);

            // Poke HMB intermediate output buffer: __pvcn_756_
            coproc_poke_word(dag_base + 498880,
                dagbin_base + 1244U);

            // Poke HMB intermediate output buffer: __pvcn_757_
            coproc_poke_word(dag_base + 593152,
                dagbin_base + 1292U);

            /* Pokable fields */
#ifdef __PVCN_543__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_543__dram_pitch
            if (o_args->__pvcn_543__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_543__dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef __PVCN_620__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_620__dram_pitch
            if (o_args->__pvcn_620__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_620__dram_pitch_value, dagbin_base, 
                    72U);

#endif

#ifdef __PVCN_686__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_686__dram_pitch
            if (o_args->__pvcn_686__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_686__dram_pitch_value, dagbin_base, 
                    112U);

#endif

#ifdef __PVCN_725__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_725__dram_pitch
            if (o_args->__pvcn_725__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_725__dram_pitch_value, dagbin_base, 
                    152U);

#endif

#ifdef __PVCN_675__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_675__dram_pitch
            if (o_args->__pvcn_675__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_675__dram_pitch_value, dagbin_base, 
                    192U);

#endif

#ifdef __PVCN_714__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_714__dram_pitch
            if (o_args->__pvcn_714__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_714__dram_pitch_value, dagbin_base, 
                    232U);

#endif

#ifdef __PVCN_653__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_653__dram_pitch
            if (o_args->__pvcn_653__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_653__dram_pitch_value, dagbin_base, 
                    272U);

#endif

#ifdef __PVCN_664__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_664__dram_pitch
            if (o_args->__pvcn_664__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_664__dram_pitch_value, dagbin_base, 
                    312U);

#endif

#ifdef __PVCN_609__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_609__dram_pitch
            if (o_args->__pvcn_609__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_609__dram_pitch_value, dagbin_base, 
                    352U);

#endif

#ifdef __PVCN_631__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_631__dram_pitch
            if (o_args->__pvcn_631__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_631__dram_pitch_value, dagbin_base, 
                    392U);

#endif

#ifdef __PVCN_587__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_587__dram_pitch
            if (o_args->__pvcn_587__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_587__dram_pitch_value, dagbin_base, 
                    432U);

#endif

#ifdef __PVCN_642__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_642__dram_pitch
            if (o_args->__pvcn_642__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_642__dram_pitch_value, dagbin_base, 
                    472U);

#endif

#ifdef __PVCN_598__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_598__dram_pitch
            if (o_args->__pvcn_598__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_598__dram_pitch_value, dagbin_base + 512U);

#endif

#ifdef __PVCN_565__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_565__dram_pitch
            if (o_args->__pvcn_565__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_565__dram_pitch_value, dagbin_base + 552U);

#endif

#ifdef __PVCN_576__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_576__dram_pitch
            if (o_args->__pvcn_576__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_576__dram_pitch_value, dagbin_base + 592U);

#endif

#ifdef __PVCN_554__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_554__dram_pitch
            if (o_args->__pvcn_554__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_554__dram_pitch_value, dagbin_base + 632U);

#endif

#ifdef __PVCN_703__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_703__dram_pitch
            if (o_args->__pvcn_703__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_703__dram_pitch_value, dagbin_base + 672U);

#endif

#ifdef __PVCN_719__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_719__dram_pitch
            if (o_args->__pvcn_719__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_719__dram_pitch_value, dagbin_base + 792U);

#endif

#ifdef __PVCN_755__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_755__dram_pitch
            if (o_args->__pvcn_755__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_755__dram_pitch_value, dagbin_base + 1200U);

#endif

#ifdef __PVCN_756__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_756__dram_pitch
            if (o_args->__pvcn_756__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_756__dram_pitch_value, dagbin_base + 1248U);

#endif

#ifdef __PVCN_757__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_757__dram_pitch
            if (o_args->__pvcn_757__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_757__dram_pitch_value, dagbin_base + 1296U);

#endif

            /*** dmald() calls ***/
            break;
        case 6:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: __pvcn_719_
            coproc_poke_word_offset(dag_base + 389440,
                dagbin_base, 100U);

            // Poke HMB intermediate input buffer: __pvcn_755_
            coproc_poke_word_offset(dag_base + 407680,
                dagbin_base, 28U);

            // Poke HMB intermediate input buffer: __pvcn_756_
            coproc_poke_word_offset(dag_base + 498880,
                dagbin_base, 64U);

            // Poke HMB intermediate input buffer: __pvcn_757_
            coproc_poke_word_offset(dag_base + 593152,
                dagbin_base, 140U);

            // Poke HMB intermediate output buffer: __pvcn_789_
            coproc_poke_word(dag_base + 0,
                dagbin_base + 1460U);

            // Poke HMB intermediate output buffer: __pvcn_800_
            coproc_poke_word(dag_base + 66880,
                dagbin_base + 1516U);

            // Poke HMB intermediate output buffer: __pvcn_811_
            coproc_poke_word(dag_base + 139840,
                dagbin_base + 1572U);

            // Poke HMB intermediate output buffer: __pvcn_822_
            coproc_poke_word(dag_base + 212800,
                dagbin_base + 1628U);

            // Poke HMB intermediate output buffer: __pvcn_833_
            coproc_poke_word(dag_base + 282752,
                dagbin_base + 1684U);

            // Poke HMB intermediate output buffer: __pvcn_910_
            coproc_poke_word(dag_base + 346624,
                dagbin_base + 2076U);

            // Poke HMB intermediate output buffer: __pvcn_844_
            coproc_poke_word(dag_base + 778624,
                dagbin_base + 1740U);

            // Poke HMB intermediate output buffer: __pvcn_855_
            coproc_poke_word(dag_base + 842496,
                dagbin_base + 1796U);

            // Poke HMB intermediate output buffer: __pvcn_866_
            coproc_poke_word(dag_base + 909376,
                dagbin_base + 1852U);

            // Poke HMB intermediate output buffer: __pvcn_877_
            coproc_poke_word(dag_base + 982336,
                dagbin_base + 1908U);

            // Poke HMB intermediate output buffer: __pvcn_888_
            coproc_poke_word(dag_base + 1049216,
                dagbin_base + 1964U);

            // Poke HMB intermediate output buffer: __pvcn_899_
            coproc_poke_word(dag_base + 1110016,
                dagbin_base + 2020U);

            /* Pokable fields */
#ifdef __PVCN_755__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_755__dram_pitch
            if (o_args->__pvcn_755__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_755__dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef __PVCN_756__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_756__dram_pitch
            if (o_args->__pvcn_756__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_756__dram_pitch_value, dagbin_base, 
                    68U);

#endif

#ifdef __PVCN_719__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_719__dram_pitch
            if (o_args->__pvcn_719__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_719__dram_pitch_value, dagbin_base, 
                    104U);

#endif

#ifdef __PVCN_757__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_757__dram_pitch
            if (o_args->__pvcn_757__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_757__dram_pitch_value, dagbin_base, 
                    144U);

#endif

#ifdef __PVCN_789__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_789__dram_pitch
            if (o_args->__pvcn_789__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_789__dram_pitch_value, dagbin_base + 1464U);

#endif

#ifdef __PVCN_800__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_800__dram_pitch
            if (o_args->__pvcn_800__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_800__dram_pitch_value, dagbin_base + 1520U);

#endif

#ifdef __PVCN_811__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_811__dram_pitch
            if (o_args->__pvcn_811__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_811__dram_pitch_value, dagbin_base + 1576U);

#endif

#ifdef __PVCN_822__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_822__dram_pitch
            if (o_args->__pvcn_822__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_822__dram_pitch_value, dagbin_base + 1632U);

#endif

#ifdef __PVCN_833__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_833__dram_pitch
            if (o_args->__pvcn_833__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_833__dram_pitch_value, dagbin_base + 1688U);

#endif

#ifdef __PVCN_844__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_844__dram_pitch
            if (o_args->__pvcn_844__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_844__dram_pitch_value, dagbin_base + 1744U);

#endif

#ifdef __PVCN_855__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_855__dram_pitch
            if (o_args->__pvcn_855__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_855__dram_pitch_value, dagbin_base + 1800U);

#endif

#ifdef __PVCN_866__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_866__dram_pitch
            if (o_args->__pvcn_866__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_866__dram_pitch_value, dagbin_base + 1856U);

#endif

#ifdef __PVCN_877__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_877__dram_pitch
            if (o_args->__pvcn_877__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_877__dram_pitch_value, dagbin_base + 1912U);

#endif

#ifdef __PVCN_888__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_888__dram_pitch
            if (o_args->__pvcn_888__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_888__dram_pitch_value, dagbin_base + 1968U);

#endif

#ifdef __PVCN_899__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_899__dram_pitch
            if (o_args->__pvcn_899__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_899__dram_pitch_value, dagbin_base + 2024U);

#endif

#ifdef __PVCN_910__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_910__dram_pitch
            if (o_args->__pvcn_910__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_910__dram_pitch_value, dagbin_base + 2080U);

#endif

            /*** dmald() calls ***/
            break;
        case 7:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: __pvcn_789_
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 468U);

            // Poke HMB intermediate input buffer: __pvcn_800_
            coproc_poke_word_offset(dag_base + 66880,
                dagbin_base, 428U);

            // Poke HMB intermediate input buffer: __pvcn_811_
            coproc_poke_word_offset(dag_base + 139840,
                dagbin_base, 388U);

            // Poke HMB intermediate input buffer: __pvcn_822_
            coproc_poke_word_offset(dag_base + 212800,
                dagbin_base, 348U);

            // Poke HMB intermediate input buffer: __pvcn_833_
            coproc_poke_word_offset(dag_base + 282752,
                dagbin_base, 108U);

            // Poke HMB intermediate input buffer: __pvcn_910_
            coproc_poke_word_offset(dag_base + 346624,
                dagbin_base, 148U);

            // Poke HMB intermediate input buffer: __pvcn_844_
            coproc_poke_word_offset(dag_base + 778624,
                dagbin_base, 308U);

            // Poke HMB intermediate input buffer: __pvcn_855_
            coproc_poke_word_offset(dag_base + 842496,
                dagbin_base, 188U);

            // Poke HMB intermediate input buffer: __pvcn_866_
            coproc_poke_word_offset(dag_base + 909376,
                dagbin_base, 268U);

            // Poke HMB intermediate input buffer: __pvcn_877_
            coproc_poke_word_offset(dag_base + 982336,
                dagbin_base, 28U);

            // Poke HMB intermediate input buffer: __pvcn_888_
            coproc_poke_word_offset(dag_base + 1049216,
                dagbin_base, 68U);

            // Poke HMB intermediate input buffer: __pvcn_899_
            coproc_poke_word_offset(dag_base + 1110016,
                dagbin_base, 228U);

            // Poke HMB intermediate output buffer: __pvcn_925_
            coproc_poke_word(dag_base + 386176,
                dagbin_base + 948U);

            // Poke HMB intermediate output buffer: __pvcn_926_
            coproc_poke_word(dag_base + 526016,
                dagbin_base + 996U);

            // Poke HMB intermediate output buffer: __pvcn_930_
            coproc_poke_word(dag_base + 668928,
                dagbin_base + 1188U);

            // Poke HMB intermediate output buffer: __pvcn_927_
            coproc_poke_word(dag_base + 1170816,
                dagbin_base + 1044U);

            // Poke HMB intermediate output buffer: __pvcn_928_
            coproc_poke_word(dag_base + 1298496,
                dagbin_base + 1092U);

            // Poke HMB intermediate output buffer: __pvcn_929_
            coproc_poke_word(dag_base + 1438336,
                dagbin_base + 1140U);

            /* Pokable fields */
#ifdef __PVCN_877__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_877__dram_pitch
            if (o_args->__pvcn_877__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_877__dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef __PVCN_888__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_888__dram_pitch
            if (o_args->__pvcn_888__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_888__dram_pitch_value, dagbin_base, 
                    72U);

#endif

#ifdef __PVCN_833__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_833__dram_pitch
            if (o_args->__pvcn_833__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_833__dram_pitch_value, dagbin_base, 
                    112U);

#endif

#ifdef __PVCN_910__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_910__dram_pitch
            if (o_args->__pvcn_910__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_910__dram_pitch_value, dagbin_base, 
                    152U);

#endif

#ifdef __PVCN_855__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_855__dram_pitch
            if (o_args->__pvcn_855__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_855__dram_pitch_value, dagbin_base, 
                    192U);

#endif

#ifdef __PVCN_899__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_899__dram_pitch
            if (o_args->__pvcn_899__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_899__dram_pitch_value, dagbin_base, 
                    232U);

#endif

#ifdef __PVCN_866__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_866__dram_pitch
            if (o_args->__pvcn_866__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_866__dram_pitch_value, dagbin_base, 
                    272U);

#endif

#ifdef __PVCN_844__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_844__dram_pitch
            if (o_args->__pvcn_844__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_844__dram_pitch_value, dagbin_base, 
                    312U);

#endif

#ifdef __PVCN_822__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_822__dram_pitch
            if (o_args->__pvcn_822__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_822__dram_pitch_value, dagbin_base, 
                    352U);

#endif

#ifdef __PVCN_811__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_811__dram_pitch
            if (o_args->__pvcn_811__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_811__dram_pitch_value, dagbin_base, 
                    392U);

#endif

#ifdef __PVCN_800__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_800__dram_pitch
            if (o_args->__pvcn_800__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_800__dram_pitch_value, dagbin_base, 
                    432U);

#endif

#ifdef __PVCN_789__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_789__dram_pitch
            if (o_args->__pvcn_789__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_789__dram_pitch_value, dagbin_base, 
                    472U);

#endif

#ifdef __PVCN_925__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_925__dram_pitch
            if (o_args->__pvcn_925__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_925__dram_pitch_value, dagbin_base + 952U);

#endif

#ifdef __PVCN_926__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_926__dram_pitch
            if (o_args->__pvcn_926__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_926__dram_pitch_value, dagbin_base + 1000U);

#endif

#ifdef __PVCN_927__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_927__dram_pitch
            if (o_args->__pvcn_927__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_927__dram_pitch_value, dagbin_base + 1048U);

#endif

#ifdef __PVCN_928__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_928__dram_pitch
            if (o_args->__pvcn_928__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_928__dram_pitch_value, dagbin_base + 1096U);

#endif

#ifdef __PVCN_929__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_929__dram_pitch
            if (o_args->__pvcn_929__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_929__dram_pitch_value, dagbin_base + 1144U);

#endif

#ifdef __PVCN_930__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_930__dram_pitch
            if (o_args->__pvcn_930__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_930__dram_pitch_value, dagbin_base + 1192U);

#endif

            /*** dmald() calls ***/
            break;
        case 8:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: __pvcn_925_
            coproc_poke_word_offset(dag_base + 386176,
                dagbin_base, 28U);

            // Poke HMB intermediate input buffer: __pvcn_926_
            coproc_poke_word_offset(dag_base + 526016,
                dagbin_base, 224U);

            // Poke HMB intermediate input buffer: __pvcn_930_
            coproc_poke_word_offset(dag_base + 668928,
                dagbin_base, 144U);

            // Poke HMB intermediate input buffer: __pvcn_927_
            coproc_poke_word_offset(dag_base + 1170816,
                dagbin_base, 104U);

            // Poke HMB intermediate input buffer: __pvcn_928_
            coproc_poke_word_offset(dag_base + 1298496,
                dagbin_base, 64U);

            // Poke HMB intermediate input buffer: __pvcn_929_
            coproc_poke_word_offset(dag_base + 1438336,
                dagbin_base, 184U);

            // Poke HMB intermediate output buffer: conv4_1__sep_____bn_conv4_1__sep__scale_mul___muli___38_____bn_conv4_1__sep__scale_mul_____scale___muli___344
            coproc_poke_word(dag_base + 769280,
                dagbin_base + 680U);

            /* Pokable fields */
#ifdef __PVCN_925__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_925__dram_pitch
            if (o_args->__pvcn_925__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_925__dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef __PVCN_928__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_928__dram_pitch
            if (o_args->__pvcn_928__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_928__dram_pitch_value, dagbin_base, 
                    68U);

#endif

#ifdef __PVCN_927__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_927__dram_pitch
            if (o_args->__pvcn_927__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_927__dram_pitch_value, dagbin_base, 
                    108U);

#endif

#ifdef __PVCN_930__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_930__dram_pitch
            if (o_args->__pvcn_930__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_930__dram_pitch_value, dagbin_base, 
                    148U);

#endif

#ifdef __PVCN_929__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_929__dram_pitch
            if (o_args->__pvcn_929__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_929__dram_pitch_value, dagbin_base, 
                    188U);

#endif

#ifdef __PVCN_926__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_926__dram_pitch
            if (o_args->__pvcn_926__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_926__dram_pitch_value, dagbin_base, 
                    228U);

#endif

#ifdef CONV4_1__SEP_____BN_CONV4_1__SEP__SCALE_MUL___MULI___38_____BN_CONV4_1__SEP__SCALE_MUL_____SCALE___MULI___344_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv4_1__sep_____bn_conv4_1__sep__scale_mul___muli___38_____bn_conv4_1__sep__scale_mul_____scale___muli___344_dram_pitch
            if (o_args->conv4_1__sep_____bn_conv4_1__sep__scale_mul___muli___38_____bn_conv4_1__sep__scale_mul_____scale___muli___344_dram_pitch_enable)
                coproc_poke_half(o_args->conv4_1__sep_____bn_conv4_1__sep__scale_mul___muli___38_____bn_conv4_1__sep__scale_mul_____scale___muli___344_dram_pitch_value, dagbin_base + 684U);

#endif

            /*** dmald() calls ***/
            break;
        case 9:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: conv4_1__sep_____bn_conv4_1__sep__scale_mul___muli___38_____bn_conv4_1__sep__scale_mul_____scale___muli___344
            coproc_poke_word_offset(dag_base + 769280,
                dagbin_base, 28U);

            // Poke HMB intermediate output buffer: __pvcn_963_
            coproc_poke_word(dag_base + 0,
                dagbin_base + 1108U);

            // Poke HMB intermediate output buffer: __pvcn_974_
            coproc_poke_word(dag_base + 21888,
                dagbin_base + 1216U);

            // Poke HMB intermediate output buffer: __pvcn_985_
            coproc_poke_word(dag_base + 41088,
                dagbin_base + 1324U);

            // Poke HMB intermediate output buffer: __pvcn_996_
            coproc_poke_word(dag_base + 61184,
                dagbin_base + 1432U);

            // Poke HMB intermediate output buffer: __pvcn_1007_
            coproc_poke_word(dag_base + 82176,
                dagbin_base + 1540U);

            // Poke HMB intermediate output buffer: __pvcn_1018_
            coproc_poke_word(dag_base + 98624,
                dagbin_base + 1648U);

            // Poke HMB intermediate output buffer: __pvcn_1029_
            coproc_poke_word(dag_base + 116864,
                dagbin_base + 1756U);

            // Poke HMB intermediate output buffer: __pvcn_1040_
            coproc_poke_word(dag_base + 137856,
                dagbin_base + 1864U);

            // Poke HMB intermediate output buffer: __pvcn_1051_
            coproc_poke_word(dag_base + 159744,
                dagbin_base + 1972U);

            // Poke HMB intermediate output buffer: __pvcn_1062_
            coproc_poke_word(dag_base + 181632,
                dagbin_base + 2080U);

            // Poke HMB intermediate output buffer: __pvcn_1073_
            coproc_poke_word(dag_base + 201728,
                dagbin_base + 2188U);

            // Poke HMB intermediate output buffer: __pvcn_1084_
            coproc_poke_word(dag_base + 220928,
                dagbin_base + 2296U);

            /* Pokable fields */
#ifdef CONV4_1__SEP_____BN_CONV4_1__SEP__SCALE_MUL___MULI___38_____BN_CONV4_1__SEP__SCALE_MUL_____SCALE___MULI___344_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv4_1__sep_____bn_conv4_1__sep__scale_mul___muli___38_____bn_conv4_1__sep__scale_mul_____scale___muli___344_dram_pitch
            if (o_args->conv4_1__sep_____bn_conv4_1__sep__scale_mul___muli___38_____bn_conv4_1__sep__scale_mul_____scale___muli___344_dram_pitch_enable)
                coproc_poke_half_offset(o_args->conv4_1__sep_____bn_conv4_1__sep__scale_mul___muli___38_____bn_conv4_1__sep__scale_mul_____scale___muli___344_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef __PVCN_963__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_963__dram_pitch
            if (o_args->__pvcn_963__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_963__dram_pitch_value, dagbin_base + 1112U);

#endif

#ifdef __PVCN_974__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_974__dram_pitch
            if (o_args->__pvcn_974__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_974__dram_pitch_value, dagbin_base + 1220U);

#endif

#ifdef __PVCN_985__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_985__dram_pitch
            if (o_args->__pvcn_985__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_985__dram_pitch_value, dagbin_base + 1328U);

#endif

#ifdef __PVCN_996__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_996__dram_pitch
            if (o_args->__pvcn_996__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_996__dram_pitch_value, dagbin_base + 1436U);

#endif

#ifdef __PVCN_1007__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1007__dram_pitch
            if (o_args->__pvcn_1007__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_1007__dram_pitch_value, dagbin_base + 1544U);

#endif

#ifdef __PVCN_1018__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1018__dram_pitch
            if (o_args->__pvcn_1018__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_1018__dram_pitch_value, dagbin_base + 1652U);

#endif

#ifdef __PVCN_1029__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1029__dram_pitch
            if (o_args->__pvcn_1029__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_1029__dram_pitch_value, dagbin_base + 1760U);

#endif

#ifdef __PVCN_1040__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1040__dram_pitch
            if (o_args->__pvcn_1040__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_1040__dram_pitch_value, dagbin_base + 1868U);

#endif

#ifdef __PVCN_1051__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1051__dram_pitch
            if (o_args->__pvcn_1051__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_1051__dram_pitch_value, dagbin_base + 1976U);

#endif

#ifdef __PVCN_1062__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1062__dram_pitch
            if (o_args->__pvcn_1062__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_1062__dram_pitch_value, dagbin_base + 2084U);

#endif

#ifdef __PVCN_1073__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1073__dram_pitch
            if (o_args->__pvcn_1073__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_1073__dram_pitch_value, dagbin_base + 2192U);

#endif

#ifdef __PVCN_1084__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1084__dram_pitch
            if (o_args->__pvcn_1084__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_1084__dram_pitch_value, dagbin_base + 2300U);

#endif

            /*** dmald() calls ***/
            break;
        case 10:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: __pvcn_963_
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 68U);

            // Poke HMB intermediate input buffer: __pvcn_974_
            coproc_poke_word_offset(dag_base + 21888,
                dagbin_base, 464U);

            // Poke HMB intermediate input buffer: __pvcn_985_
            coproc_poke_word_offset(dag_base + 41088,
                dagbin_base, 224U);

            // Poke HMB intermediate input buffer: __pvcn_996_
            coproc_poke_word_offset(dag_base + 61184,
                dagbin_base, 104U);

            // Poke HMB intermediate input buffer: __pvcn_1007_
            coproc_poke_word_offset(dag_base + 82176,
                dagbin_base, 384U);

            // Poke HMB intermediate input buffer: __pvcn_1018_
            coproc_poke_word_offset(dag_base + 98624,
                dagbin_base, 424U);

            // Poke HMB intermediate input buffer: __pvcn_1029_
            coproc_poke_word_offset(dag_base + 116864,
                dagbin_base, 184U);

            // Poke HMB intermediate input buffer: __pvcn_1040_
            coproc_poke_word_offset(dag_base + 137856,
                dagbin_base, 344U);

            // Poke HMB intermediate input buffer: __pvcn_1051_
            coproc_poke_word_offset(dag_base + 159744,
                dagbin_base, 264U);

            // Poke HMB intermediate input buffer: __pvcn_1062_
            coproc_poke_word_offset(dag_base + 181632,
                dagbin_base, 304U);

            // Poke HMB intermediate input buffer: __pvcn_1073_
            coproc_poke_word_offset(dag_base + 201728,
                dagbin_base, 28U);

            // Poke HMB intermediate input buffer: __pvcn_1084_
            coproc_poke_word_offset(dag_base + 220928,
                dagbin_base, 144U);

            // Poke HMB intermediate output buffer: conv4_2__sep_____bn_conv4_2__sep__scale_mul___muli___44_____bn_conv4_2__sep__scale_mul_____scale___relu4_2__sep
            coproc_poke_word(dag_base + 233728,
                dagbin_base + 1044U);

            /* Pokable fields */
#ifdef __PVCN_1073__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1073__dram_pitch
            if (o_args->__pvcn_1073__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_1073__dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef __PVCN_963__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_963__dram_pitch
            if (o_args->__pvcn_963__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_963__dram_pitch_value, dagbin_base, 
                    72U);

#endif

#ifdef __PVCN_996__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_996__dram_pitch
            if (o_args->__pvcn_996__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_996__dram_pitch_value, dagbin_base, 
                    108U);

#endif

#ifdef __PVCN_1084__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1084__dram_pitch
            if (o_args->__pvcn_1084__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_1084__dram_pitch_value, dagbin_base, 
                    148U);

#endif

#ifdef __PVCN_1029__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1029__dram_pitch
            if (o_args->__pvcn_1029__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_1029__dram_pitch_value, dagbin_base, 
                    188U);

#endif

#ifdef __PVCN_985__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_985__dram_pitch
            if (o_args->__pvcn_985__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_985__dram_pitch_value, dagbin_base, 
                    228U);

#endif

#ifdef __PVCN_1051__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1051__dram_pitch
            if (o_args->__pvcn_1051__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_1051__dram_pitch_value, dagbin_base, 
                    268U);

#endif

#ifdef __PVCN_1062__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1062__dram_pitch
            if (o_args->__pvcn_1062__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_1062__dram_pitch_value, dagbin_base, 
                    308U);

#endif

#ifdef __PVCN_1040__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1040__dram_pitch
            if (o_args->__pvcn_1040__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_1040__dram_pitch_value, dagbin_base, 
                    348U);

#endif

#ifdef __PVCN_1007__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1007__dram_pitch
            if (o_args->__pvcn_1007__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_1007__dram_pitch_value, dagbin_base, 
                    388U);

#endif

#ifdef __PVCN_1018__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1018__dram_pitch
            if (o_args->__pvcn_1018__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_1018__dram_pitch_value, dagbin_base, 
                    428U);

#endif

#ifdef __PVCN_974__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_974__dram_pitch
            if (o_args->__pvcn_974__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_974__dram_pitch_value, dagbin_base, 
                    468U);

#endif

#ifdef CONV4_2__SEP_____BN_CONV4_2__SEP__SCALE_MUL___MULI___44_____BN_CONV4_2__SEP__SCALE_MUL_____SCALE___RELU4_2__SEP_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv4_2__sep_____bn_conv4_2__sep__scale_mul___muli___44_____bn_conv4_2__sep__scale_mul_____scale___relu4_2__sep_dram_pitch
            if (o_args->conv4_2__sep_____bn_conv4_2__sep__scale_mul___muli___44_____bn_conv4_2__sep__scale_mul_____scale___relu4_2__sep_dram_pitch_enable)
                coproc_poke_half(o_args->conv4_2__sep_____bn_conv4_2__sep__scale_mul___muli___44_____bn_conv4_2__sep__scale_mul_____scale___relu4_2__sep_dram_pitch_value, dagbin_base + 1048U);

#endif

            /*** dmald() calls ***/
            break;
        case 11:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: conv4_2__sep_____bn_conv4_2__sep__scale_mul___muli___44_____bn_conv4_2__sep__scale_mul_____scale___relu4_2__sep
            coproc_poke_word_offset(dag_base + 233728,
                dagbin_base, 28U);

            // Poke HMB intermediate output buffer: conv5_1__sep_____bn_conv5_1__sep__scale_mul___muli___50_____bn_conv5_1__sep__scale_mul_____scale___relu5_1__sep
            coproc_poke_word(dag_base + 0,
                dagbin_base + 1396U);

            /* Pokable fields */
#ifdef CONV4_2__SEP_____BN_CONV4_2__SEP__SCALE_MUL___MULI___44_____BN_CONV4_2__SEP__SCALE_MUL_____SCALE___RELU4_2__SEP_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv4_2__sep_____bn_conv4_2__sep__scale_mul___muli___44_____bn_conv4_2__sep__scale_mul_____scale___relu4_2__sep_dram_pitch
            if (o_args->conv4_2__sep_____bn_conv4_2__sep__scale_mul___muli___44_____bn_conv4_2__sep__scale_mul_____scale___relu4_2__sep_dram_pitch_enable)
                coproc_poke_half_offset(o_args->conv4_2__sep_____bn_conv4_2__sep__scale_mul___muli___44_____bn_conv4_2__sep__scale_mul_____scale___relu4_2__sep_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef CONV5_1__SEP_____BN_CONV5_1__SEP__SCALE_MUL___MULI___50_____BN_CONV5_1__SEP__SCALE_MUL_____SCALE___RELU5_1__SEP_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv5_1__sep_____bn_conv5_1__sep__scale_mul___muli___50_____bn_conv5_1__sep__scale_mul_____scale___relu5_1__sep_dram_pitch
            if (o_args->conv5_1__sep_____bn_conv5_1__sep__scale_mul___muli___50_____bn_conv5_1__sep__scale_mul_____scale___relu5_1__sep_dram_pitch_enable)
                coproc_poke_half(o_args->conv5_1__sep_____bn_conv5_1__sep__scale_mul___muli___50_____bn_conv5_1__sep__scale_mul_____scale___relu5_1__sep_dram_pitch_value, dagbin_base + 1400U);

#endif

            /*** dmald() calls ***/
            break;
        case 12:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: conv5_1__sep_____bn_conv5_1__sep__scale_mul___muli___50_____bn_conv5_1__sep__scale_mul_____scale___relu5_1__sep
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 28U);

            // Poke HMB intermediate output buffer: __pvcn_1293_
            coproc_poke_word(dag_base + 233472,
                dagbin_base + 1264U);

            // Poke HMB intermediate output buffer: __pvcn_1304_
            coproc_poke_word(dag_base + 268160,
                dagbin_base + 1364U);

            // Poke HMB intermediate output buffer: __pvcn_1315_
            coproc_poke_word(dag_base + 300992,
                dagbin_base + 1464U);

            // Poke HMB intermediate output buffer: __pvcn_1326_
            coproc_poke_word(dag_base + 338432,
                dagbin_base + 1564U);

            // Poke HMB intermediate output buffer: __pvcn_1337_
            coproc_poke_word(dag_base + 371264,
                dagbin_base + 1664U);

            // Poke HMB intermediate output buffer: __pvcn_1348_
            coproc_poke_word(dag_base + 405952,
                dagbin_base + 1764U);

            // Poke HMB intermediate output buffer: __pvcn_1359_
            coproc_poke_word(dag_base + 439744,
                dagbin_base + 1864U);

            // Poke HMB intermediate output buffer: __pvcn_1370_
            coproc_poke_word(dag_base + 473536,
                dagbin_base + 1964U);

            // Poke HMB intermediate output buffer: __pvcn_1381_
            coproc_poke_word(dag_base + 512768,
                dagbin_base + 2064U);

            // Poke HMB intermediate output buffer: __pvcn_1392_
            coproc_poke_word(dag_base + 547456,
                dagbin_base + 2164U);

            // Poke HMB intermediate output buffer: __pvcn_1403_
            coproc_poke_word(dag_base + 583936,
                dagbin_base + 2264U);

            // Poke HMB intermediate output buffer: __pvcn_1414_
            coproc_poke_word(dag_base + 617728,
                dagbin_base + 2364U);

            // Poke HMB intermediate output buffer: __pvcn_1425_
            coproc_poke_word(dag_base + 652416,
                dagbin_base + 2464U);

            // Poke HMB intermediate output buffer: __pvcn_1436_
            coproc_poke_word(dag_base + 688000,
                dagbin_base + 2564U);

            /* Pokable fields */
#ifdef CONV5_1__SEP_____BN_CONV5_1__SEP__SCALE_MUL___MULI___50_____BN_CONV5_1__SEP__SCALE_MUL_____SCALE___RELU5_1__SEP_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv5_1__sep_____bn_conv5_1__sep__scale_mul___muli___50_____bn_conv5_1__sep__scale_mul_____scale___relu5_1__sep_dram_pitch
            if (o_args->conv5_1__sep_____bn_conv5_1__sep__scale_mul___muli___50_____bn_conv5_1__sep__scale_mul_____scale___relu5_1__sep_dram_pitch_enable)
                coproc_poke_half_offset(o_args->conv5_1__sep_____bn_conv5_1__sep__scale_mul___muli___50_____bn_conv5_1__sep__scale_mul_____scale___relu5_1__sep_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef __PVCN_1293__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1293__dram_pitch
            if (o_args->__pvcn_1293__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_1293__dram_pitch_value, dagbin_base + 1268U);

#endif

#ifdef __PVCN_1304__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1304__dram_pitch
            if (o_args->__pvcn_1304__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_1304__dram_pitch_value, dagbin_base + 1368U);

#endif

#ifdef __PVCN_1315__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1315__dram_pitch
            if (o_args->__pvcn_1315__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_1315__dram_pitch_value, dagbin_base + 1468U);

#endif

#ifdef __PVCN_1326__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1326__dram_pitch
            if (o_args->__pvcn_1326__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_1326__dram_pitch_value, dagbin_base + 1568U);

#endif

#ifdef __PVCN_1337__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1337__dram_pitch
            if (o_args->__pvcn_1337__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_1337__dram_pitch_value, dagbin_base + 1668U);

#endif

#ifdef __PVCN_1348__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1348__dram_pitch
            if (o_args->__pvcn_1348__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_1348__dram_pitch_value, dagbin_base + 1768U);

#endif

#ifdef __PVCN_1359__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1359__dram_pitch
            if (o_args->__pvcn_1359__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_1359__dram_pitch_value, dagbin_base + 1868U);

#endif

#ifdef __PVCN_1370__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1370__dram_pitch
            if (o_args->__pvcn_1370__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_1370__dram_pitch_value, dagbin_base + 1968U);

#endif

#ifdef __PVCN_1381__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1381__dram_pitch
            if (o_args->__pvcn_1381__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_1381__dram_pitch_value, dagbin_base + 2068U);

#endif

#ifdef __PVCN_1392__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1392__dram_pitch
            if (o_args->__pvcn_1392__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_1392__dram_pitch_value, dagbin_base + 2168U);

#endif

#ifdef __PVCN_1403__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1403__dram_pitch
            if (o_args->__pvcn_1403__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_1403__dram_pitch_value, dagbin_base + 2268U);

#endif

#ifdef __PVCN_1414__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1414__dram_pitch
            if (o_args->__pvcn_1414__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_1414__dram_pitch_value, dagbin_base + 2368U);

#endif

#ifdef __PVCN_1425__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1425__dram_pitch
            if (o_args->__pvcn_1425__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_1425__dram_pitch_value, dagbin_base + 2468U);

#endif

#ifdef __PVCN_1436__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1436__dram_pitch
            if (o_args->__pvcn_1436__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_1436__dram_pitch_value, dagbin_base + 2568U);

#endif

            /*** dmald() calls ***/
            break;
        case 13:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: __pvcn_1293_
            coproc_poke_word_offset(dag_base + 233472,
                dagbin_base, 268U);

            // Poke HMB intermediate input buffer: __pvcn_1304_
            coproc_poke_word(dag_base + 268160,
                dagbin_base + 544U);

            // Poke HMB intermediate input buffer: __pvcn_1315_
            coproc_poke_word_offset(dag_base + 300992,
                dagbin_base, 464U);

            // Poke HMB intermediate input buffer: __pvcn_1326_
            coproc_poke_word_offset(dag_base + 338432,
                dagbin_base, 504U);

            // Poke HMB intermediate input buffer: __pvcn_1337_
            coproc_poke_word_offset(dag_base + 371264,
                dagbin_base, 424U);

            // Poke HMB intermediate input buffer: __pvcn_1348_
            coproc_poke_word_offset(dag_base + 405952,
                dagbin_base, 68U);

            // Poke HMB intermediate input buffer: __pvcn_1359_
            coproc_poke_word_offset(dag_base + 439744,
                dagbin_base, 344U);

            // Poke HMB intermediate input buffer: __pvcn_1370_
            coproc_poke_word_offset(dag_base + 473536,
                dagbin_base, 384U);

            // Poke HMB intermediate input buffer: __pvcn_1381_
            coproc_poke_word_offset(dag_base + 512768,
                dagbin_base, 304U);

            // Poke HMB intermediate input buffer: __pvcn_1392_
            coproc_poke_word_offset(dag_base + 547456,
                dagbin_base, 148U);

            // Poke HMB intermediate input buffer: __pvcn_1403_
            coproc_poke_word_offset(dag_base + 583936,
                dagbin_base, 28U);

            // Poke HMB intermediate input buffer: __pvcn_1414_
            coproc_poke_word_offset(dag_base + 617728,
                dagbin_base, 228U);

            // Poke HMB intermediate input buffer: __pvcn_1425_
            coproc_poke_word_offset(dag_base + 652416,
                dagbin_base, 108U);

            // Poke HMB intermediate input buffer: __pvcn_1436_
            coproc_poke_word_offset(dag_base + 688000,
                dagbin_base, 188U);

            // Poke HMB intermediate output buffer: conv5_2__sep_____bn_conv5_2__sep__scale_mul___muli___56_____bn_conv5_2__sep__scale_mul_____scale___relu5_2__sep
            coproc_poke_word(dag_base + 0,
                dagbin_base + 1196U);

            /* Pokable fields */
#ifdef __PVCN_1403__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1403__dram_pitch
            if (o_args->__pvcn_1403__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_1403__dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef __PVCN_1348__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1348__dram_pitch
            if (o_args->__pvcn_1348__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_1348__dram_pitch_value, dagbin_base, 
                    72U);

#endif

#ifdef __PVCN_1425__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1425__dram_pitch
            if (o_args->__pvcn_1425__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_1425__dram_pitch_value, dagbin_base, 
                    112U);

#endif

#ifdef __PVCN_1392__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1392__dram_pitch
            if (o_args->__pvcn_1392__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_1392__dram_pitch_value, dagbin_base, 
                    152U);

#endif

#ifdef __PVCN_1436__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1436__dram_pitch
            if (o_args->__pvcn_1436__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_1436__dram_pitch_value, dagbin_base, 
                    192U);

#endif

#ifdef __PVCN_1414__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1414__dram_pitch
            if (o_args->__pvcn_1414__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_1414__dram_pitch_value, dagbin_base, 
                    232U);

#endif

#ifdef __PVCN_1293__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1293__dram_pitch
            if (o_args->__pvcn_1293__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_1293__dram_pitch_value, dagbin_base, 
                    272U);

#endif

#ifdef __PVCN_1381__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1381__dram_pitch
            if (o_args->__pvcn_1381__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_1381__dram_pitch_value, dagbin_base, 
                    308U);

#endif

#ifdef __PVCN_1359__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1359__dram_pitch
            if (o_args->__pvcn_1359__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_1359__dram_pitch_value, dagbin_base, 
                    348U);

#endif

#ifdef __PVCN_1370__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1370__dram_pitch
            if (o_args->__pvcn_1370__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_1370__dram_pitch_value, dagbin_base, 
                    388U);

#endif

#ifdef __PVCN_1337__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1337__dram_pitch
            if (o_args->__pvcn_1337__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_1337__dram_pitch_value, dagbin_base, 
                    428U);

#endif

#ifdef __PVCN_1315__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1315__dram_pitch
            if (o_args->__pvcn_1315__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_1315__dram_pitch_value, dagbin_base, 
                    468U);

#endif

#ifdef __PVCN_1326__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1326__dram_pitch
            if (o_args->__pvcn_1326__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_1326__dram_pitch_value, dagbin_base, 
                    508U);

#endif

#ifdef __PVCN_1304__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1304__dram_pitch
            if (o_args->__pvcn_1304__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_1304__dram_pitch_value, dagbin_base + 548U);

#endif

#ifdef CONV5_2__SEP_____BN_CONV5_2__SEP__SCALE_MUL___MULI___56_____BN_CONV5_2__SEP__SCALE_MUL_____SCALE___RELU5_2__SEP_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv5_2__sep_____bn_conv5_2__sep__scale_mul___muli___56_____bn_conv5_2__sep__scale_mul_____scale___relu5_2__sep_dram_pitch
            if (o_args->conv5_2__sep_____bn_conv5_2__sep__scale_mul___muli___56_____bn_conv5_2__sep__scale_mul_____scale___relu5_2__sep_dram_pitch_enable)
                coproc_poke_half(o_args->conv5_2__sep_____bn_conv5_2__sep__scale_mul___muli___56_____bn_conv5_2__sep__scale_mul_____scale___relu5_2__sep_dram_pitch_value, dagbin_base + 1200U);

#endif

            /*** dmald() calls ***/
            break;
        case 14:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: conv5_2__sep_____bn_conv5_2__sep__scale_mul___muli___56_____bn_conv5_2__sep__scale_mul_____scale___relu5_2__sep
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 28U);

            // Poke HMB intermediate output buffer: __pvcn_1626_
            coproc_poke_word(dag_base + 233472,
                dagbin_base + 1504U);

            /* Pokable fields */
#ifdef CONV5_2__SEP_____BN_CONV5_2__SEP__SCALE_MUL___MULI___56_____BN_CONV5_2__SEP__SCALE_MUL_____SCALE___RELU5_2__SEP_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv5_2__sep_____bn_conv5_2__sep__scale_mul___muli___56_____bn_conv5_2__sep__scale_mul_____scale___relu5_2__sep_dram_pitch
            if (o_args->conv5_2__sep_____bn_conv5_2__sep__scale_mul___muli___56_____bn_conv5_2__sep__scale_mul_____scale___relu5_2__sep_dram_pitch_enable)
                coproc_poke_half_offset(o_args->conv5_2__sep_____bn_conv5_2__sep__scale_mul___muli___56_____bn_conv5_2__sep__scale_mul_____scale___relu5_2__sep_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef __PVCN_1626__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1626__dram_pitch
            if (o_args->__pvcn_1626__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_1626__dram_pitch_value, dagbin_base + 1508U);

#endif

            /*** dmald() calls ***/
            break;
        case 15:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: __pvcn_1626_
            coproc_poke_word_offset(dag_base + 233472,
                dagbin_base, 28U);

            // Poke HMB intermediate output buffer: __pvcn_1670_
            coproc_poke_word(dag_base + 0,
                dagbin_base + 1316U);

            // Poke HMB intermediate output buffer: __pvcn_1681_
            coproc_poke_word(dag_base + 31936,
                dagbin_base + 1372U);

            // Poke HMB intermediate output buffer: __pvcn_1692_
            coproc_poke_word(dag_base + 64768,
                dagbin_base + 1428U);

            // Poke HMB intermediate output buffer: __pvcn_1703_
            coproc_poke_word(dag_base + 97600,
                dagbin_base + 1484U);

            // Poke HMB intermediate output buffer: __pvcn_1714_
            coproc_poke_word(dag_base + 129536,
                dagbin_base + 1540U);

            // Poke HMB intermediate output buffer: __pvcn_1725_
            coproc_poke_word(dag_base + 166016,
                dagbin_base + 1596U);

            // Poke HMB intermediate output buffer: __pvcn_1813_
            coproc_poke_word(dag_base + 200704,
                dagbin_base + 2044U);

            // Poke HMB intermediate output buffer: __pvcn_1736_
            coproc_poke_word(dag_base + 700416,
                dagbin_base + 1652U);

            // Poke HMB intermediate output buffer: __pvcn_1747_
            coproc_poke_word(dag_base + 733248,
                dagbin_base + 1708U);

            // Poke HMB intermediate output buffer: __pvcn_1758_
            coproc_poke_word(dag_base + 767040,
                dagbin_base + 1764U);

            // Poke HMB intermediate output buffer: __pvcn_1769_
            coproc_poke_word(dag_base + 803520,
                dagbin_base + 1820U);

            // Poke HMB intermediate output buffer: __pvcn_1780_
            coproc_poke_word(dag_base + 836352,
                dagbin_base + 1876U);

            // Poke HMB intermediate output buffer: __pvcn_1791_
            coproc_poke_word(dag_base + 872832,
                dagbin_base + 1932U);

            // Poke HMB intermediate output buffer: __pvcn_1802_
            coproc_poke_word(dag_base + 905664,
                dagbin_base + 1988U);

            /* Pokable fields */
#ifdef __PVCN_1626__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1626__dram_pitch
            if (o_args->__pvcn_1626__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_1626__dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef __PVCN_1670__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1670__dram_pitch
            if (o_args->__pvcn_1670__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_1670__dram_pitch_value, dagbin_base + 1320U);

#endif

#ifdef __PVCN_1681__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1681__dram_pitch
            if (o_args->__pvcn_1681__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_1681__dram_pitch_value, dagbin_base + 1376U);

#endif

#ifdef __PVCN_1692__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1692__dram_pitch
            if (o_args->__pvcn_1692__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_1692__dram_pitch_value, dagbin_base + 1432U);

#endif

#ifdef __PVCN_1703__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1703__dram_pitch
            if (o_args->__pvcn_1703__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_1703__dram_pitch_value, dagbin_base + 1488U);

#endif

#ifdef __PVCN_1714__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1714__dram_pitch
            if (o_args->__pvcn_1714__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_1714__dram_pitch_value, dagbin_base + 1544U);

#endif

#ifdef __PVCN_1725__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1725__dram_pitch
            if (o_args->__pvcn_1725__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_1725__dram_pitch_value, dagbin_base + 1600U);

#endif

#ifdef __PVCN_1736__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1736__dram_pitch
            if (o_args->__pvcn_1736__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_1736__dram_pitch_value, dagbin_base + 1656U);

#endif

#ifdef __PVCN_1747__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1747__dram_pitch
            if (o_args->__pvcn_1747__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_1747__dram_pitch_value, dagbin_base + 1712U);

#endif

#ifdef __PVCN_1758__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1758__dram_pitch
            if (o_args->__pvcn_1758__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_1758__dram_pitch_value, dagbin_base + 1768U);

#endif

#ifdef __PVCN_1769__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1769__dram_pitch
            if (o_args->__pvcn_1769__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_1769__dram_pitch_value, dagbin_base + 1824U);

#endif

#ifdef __PVCN_1780__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1780__dram_pitch
            if (o_args->__pvcn_1780__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_1780__dram_pitch_value, dagbin_base + 1880U);

#endif

#ifdef __PVCN_1791__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1791__dram_pitch
            if (o_args->__pvcn_1791__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_1791__dram_pitch_value, dagbin_base + 1936U);

#endif

#ifdef __PVCN_1802__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1802__dram_pitch
            if (o_args->__pvcn_1802__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_1802__dram_pitch_value, dagbin_base + 1992U);

#endif

#ifdef __PVCN_1813__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1813__dram_pitch
            if (o_args->__pvcn_1813__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_1813__dram_pitch_value, dagbin_base + 2048U);

#endif

            /*** dmald() calls ***/
            break;
        case 16:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: __pvcn_1670_
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 188U);

            // Poke HMB intermediate input buffer: __pvcn_1681_
            coproc_poke_word(dag_base + 31936,
                dagbin_base + 544U);

            // Poke HMB intermediate input buffer: __pvcn_1692_
            coproc_poke_word_offset(dag_base + 64768,
                dagbin_base, 264U);

            // Poke HMB intermediate input buffer: __pvcn_1703_
            coproc_poke_word_offset(dag_base + 97600,
                dagbin_base, 504U);

            // Poke HMB intermediate input buffer: __pvcn_1714_
            coproc_poke_word_offset(dag_base + 129536,
                dagbin_base, 464U);

            // Poke HMB intermediate input buffer: __pvcn_1725_
            coproc_poke_word_offset(dag_base + 166016,
                dagbin_base, 424U);

            // Poke HMB intermediate input buffer: __pvcn_1813_
            coproc_poke_word_offset(dag_base + 200704,
                dagbin_base, 108U);

            // Poke HMB intermediate input buffer: __pvcn_1736_
            coproc_poke_word_offset(dag_base + 700416,
                dagbin_base, 384U);

            // Poke HMB intermediate input buffer: __pvcn_1747_
            coproc_poke_word_offset(dag_base + 733248,
                dagbin_base, 344U);

            // Poke HMB intermediate input buffer: __pvcn_1758_
            coproc_poke_word_offset(dag_base + 767040,
                dagbin_base, 68U);

            // Poke HMB intermediate input buffer: __pvcn_1769_
            coproc_poke_word_offset(dag_base + 803520,
                dagbin_base, 304U);

            // Poke HMB intermediate input buffer: __pvcn_1780_
            coproc_poke_word_offset(dag_base + 836352,
                dagbin_base, 224U);

            // Poke HMB intermediate input buffer: __pvcn_1791_
            coproc_poke_word_offset(dag_base + 872832,
                dagbin_base, 148U);

            // Poke HMB intermediate input buffer: __pvcn_1802_
            coproc_poke_word_offset(dag_base + 905664,
                dagbin_base, 28U);

            // Poke HMB intermediate output buffer: __pvcn_1830_
            coproc_poke_word(dag_base + 227200,
                dagbin_base + 1100U);

            // Poke HMB intermediate output buffer: __pvcn_1831_
            coproc_poke_word(dag_base + 291968,
                dagbin_base + 1148U);

            // Poke HMB intermediate output buffer: __pvcn_1832_
            coproc_poke_word(dag_base + 356736,
                dagbin_base + 1196U);

            // Poke HMB intermediate output buffer: __pvcn_1833_
            coproc_poke_word(dag_base + 427904,
                dagbin_base + 1244U);

            // Poke HMB intermediate output buffer: __pvcn_1834_
            coproc_poke_word(dag_base + 494528,
                dagbin_base + 1292U);

            // Poke HMB intermediate output buffer: __pvcn_1835_
            coproc_poke_word(dag_base + 563840,
                dagbin_base + 1340U);

            // Poke HMB intermediate output buffer: __pvcn_1836_
            coproc_poke_word(dag_base + 633152,
                dagbin_base + 1388U);

            /* Pokable fields */
#ifdef __PVCN_1802__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1802__dram_pitch
            if (o_args->__pvcn_1802__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_1802__dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef __PVCN_1758__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1758__dram_pitch
            if (o_args->__pvcn_1758__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_1758__dram_pitch_value, dagbin_base, 
                    72U);

#endif

#ifdef __PVCN_1813__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1813__dram_pitch
            if (o_args->__pvcn_1813__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_1813__dram_pitch_value, dagbin_base, 
                    112U);

#endif

#ifdef __PVCN_1791__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1791__dram_pitch
            if (o_args->__pvcn_1791__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_1791__dram_pitch_value, dagbin_base, 
                    152U);

#endif

#ifdef __PVCN_1670__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1670__dram_pitch
            if (o_args->__pvcn_1670__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_1670__dram_pitch_value, dagbin_base, 
                    192U);

#endif

#ifdef __PVCN_1780__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1780__dram_pitch
            if (o_args->__pvcn_1780__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_1780__dram_pitch_value, dagbin_base, 
                    228U);

#endif

#ifdef __PVCN_1692__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1692__dram_pitch
            if (o_args->__pvcn_1692__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_1692__dram_pitch_value, dagbin_base, 
                    268U);

#endif

#ifdef __PVCN_1769__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1769__dram_pitch
            if (o_args->__pvcn_1769__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_1769__dram_pitch_value, dagbin_base, 
                    308U);

#endif

#ifdef __PVCN_1747__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1747__dram_pitch
            if (o_args->__pvcn_1747__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_1747__dram_pitch_value, dagbin_base, 
                    348U);

#endif

#ifdef __PVCN_1736__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1736__dram_pitch
            if (o_args->__pvcn_1736__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_1736__dram_pitch_value, dagbin_base, 
                    388U);

#endif

#ifdef __PVCN_1725__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1725__dram_pitch
            if (o_args->__pvcn_1725__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_1725__dram_pitch_value, dagbin_base, 
                    428U);

#endif

#ifdef __PVCN_1714__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1714__dram_pitch
            if (o_args->__pvcn_1714__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_1714__dram_pitch_value, dagbin_base, 
                    468U);

#endif

#ifdef __PVCN_1703__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1703__dram_pitch
            if (o_args->__pvcn_1703__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_1703__dram_pitch_value, dagbin_base, 
                    508U);

#endif

#ifdef __PVCN_1681__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1681__dram_pitch
            if (o_args->__pvcn_1681__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_1681__dram_pitch_value, dagbin_base + 548U);

#endif

#ifdef __PVCN_1830__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1830__dram_pitch
            if (o_args->__pvcn_1830__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_1830__dram_pitch_value, dagbin_base + 1104U);

#endif

#ifdef __PVCN_1831__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1831__dram_pitch
            if (o_args->__pvcn_1831__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_1831__dram_pitch_value, dagbin_base + 1152U);

#endif

#ifdef __PVCN_1832__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1832__dram_pitch
            if (o_args->__pvcn_1832__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_1832__dram_pitch_value, dagbin_base + 1200U);

#endif

#ifdef __PVCN_1833__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1833__dram_pitch
            if (o_args->__pvcn_1833__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_1833__dram_pitch_value, dagbin_base + 1248U);

#endif

#ifdef __PVCN_1834__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1834__dram_pitch
            if (o_args->__pvcn_1834__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_1834__dram_pitch_value, dagbin_base + 1296U);

#endif

#ifdef __PVCN_1835__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1835__dram_pitch
            if (o_args->__pvcn_1835__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_1835__dram_pitch_value, dagbin_base + 1344U);

#endif

#ifdef __PVCN_1836__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1836__dram_pitch
            if (o_args->__pvcn_1836__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_1836__dram_pitch_value, dagbin_base + 1392U);

#endif

            /*** dmald() calls ***/
            break;
        case 17:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: __pvcn_1830_
            coproc_poke_word_offset(dag_base + 227200,
                dagbin_base, 108U);

            // Poke HMB intermediate input buffer: __pvcn_1831_
            coproc_poke_word_offset(dag_base + 291968,
                dagbin_base, 264U);

            // Poke HMB intermediate input buffer: __pvcn_1832_
            coproc_poke_word_offset(dag_base + 356736,
                dagbin_base, 28U);

            // Poke HMB intermediate input buffer: __pvcn_1833_
            coproc_poke_word_offset(dag_base + 427904,
                dagbin_base, 144U);

            // Poke HMB intermediate input buffer: __pvcn_1834_
            coproc_poke_word_offset(dag_base + 494528,
                dagbin_base, 184U);

            // Poke HMB intermediate input buffer: __pvcn_1835_
            coproc_poke_word_offset(dag_base + 563840,
                dagbin_base, 68U);

            // Poke HMB intermediate input buffer: __pvcn_1836_
            coproc_poke_word_offset(dag_base + 633152,
                dagbin_base, 224U);

            // Poke HMB intermediate output buffer: conv5_4__sep_____bn_conv5_4__sep__scale_mul___muli___68_____bn_conv5_4__sep__scale_mul_____scale___relu5_4__sep
            coproc_poke_word(dag_base + 694272,
                dagbin_base + 664U);

            /* Pokable fields */
#ifdef __PVCN_1832__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1832__dram_pitch
            if (o_args->__pvcn_1832__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_1832__dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef __PVCN_1835__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1835__dram_pitch
            if (o_args->__pvcn_1835__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_1835__dram_pitch_value, dagbin_base, 
                    72U);

#endif

#ifdef __PVCN_1830__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1830__dram_pitch
            if (o_args->__pvcn_1830__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_1830__dram_pitch_value, dagbin_base, 
                    112U);

#endif

#ifdef __PVCN_1833__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1833__dram_pitch
            if (o_args->__pvcn_1833__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_1833__dram_pitch_value, dagbin_base, 
                    148U);

#endif

#ifdef __PVCN_1834__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1834__dram_pitch
            if (o_args->__pvcn_1834__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_1834__dram_pitch_value, dagbin_base, 
                    188U);

#endif

#ifdef __PVCN_1836__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1836__dram_pitch
            if (o_args->__pvcn_1836__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_1836__dram_pitch_value, dagbin_base, 
                    228U);

#endif

#ifdef __PVCN_1831__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1831__dram_pitch
            if (o_args->__pvcn_1831__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_1831__dram_pitch_value, dagbin_base, 
                    268U);

#endif

#ifdef CONV5_4__SEP_____BN_CONV5_4__SEP__SCALE_MUL___MULI___68_____BN_CONV5_4__SEP__SCALE_MUL_____SCALE___RELU5_4__SEP_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv5_4__sep_____bn_conv5_4__sep__scale_mul___muli___68_____bn_conv5_4__sep__scale_mul_____scale___relu5_4__sep_dram_pitch
            if (o_args->conv5_4__sep_____bn_conv5_4__sep__scale_mul___muli___68_____bn_conv5_4__sep__scale_mul_____scale___relu5_4__sep_dram_pitch_enable)
                coproc_poke_half(o_args->conv5_4__sep_____bn_conv5_4__sep__scale_mul___muli___68_____bn_conv5_4__sep__scale_mul_____scale___relu5_4__sep_dram_pitch_value, dagbin_base + 668U);

#endif

            /*** dmald() calls ***/
            break;
        case 18:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: conv5_4__sep_____bn_conv5_4__sep__scale_mul___muli___68_____bn_conv5_4__sep__scale_mul_____scale___relu5_4__sep
            coproc_poke_word_offset(dag_base + 694272,
                dagbin_base, 28U);

            // Poke HMB intermediate output buffer: __pvcn_1879_
            coproc_poke_word(dag_base + 0,
                dagbin_base + 1264U);

            // Poke HMB intermediate output buffer: __pvcn_1890_
            coproc_poke_word(dag_base + 33792,
                dagbin_base + 1364U);

            // Poke HMB intermediate output buffer: __pvcn_1901_
            coproc_poke_word(dag_base + 67584,
                dagbin_base + 1464U);

            // Poke HMB intermediate output buffer: __pvcn_1912_
            coproc_poke_word(dag_base + 104064,
                dagbin_base + 1564U);

            // Poke HMB intermediate output buffer: __pvcn_1923_
            coproc_poke_word(dag_base + 136896,
                dagbin_base + 1664U);

            // Poke HMB intermediate output buffer: __pvcn_1934_
            coproc_poke_word(dag_base + 170688,
                dagbin_base + 1764U);

            // Poke HMB intermediate output buffer: __pvcn_1945_
            coproc_poke_word(dag_base + 203520,
                dagbin_base + 1864U);

            // Poke HMB intermediate output buffer: __pvcn_1956_
            coproc_poke_word(dag_base + 240000,
                dagbin_base + 1964U);

            // Poke HMB intermediate output buffer: __pvcn_1967_
            coproc_poke_word(dag_base + 273792,
                dagbin_base + 2064U);

            // Poke HMB intermediate output buffer: __pvcn_1978_
            coproc_poke_word(dag_base + 311232,
                dagbin_base + 2164U);

            // Poke HMB intermediate output buffer: __pvcn_1989_
            coproc_poke_word(dag_base + 345024,
                dagbin_base + 2264U);

            // Poke HMB intermediate output buffer: __pvcn_2000_
            coproc_poke_word(dag_base + 377856,
                dagbin_base + 2364U);

            // Poke HMB intermediate output buffer: __pvcn_2011_
            coproc_poke_word(dag_base + 409792,
                dagbin_base + 2464U);

            // Poke HMB intermediate output buffer: __pvcn_2022_
            coproc_poke_word(dag_base + 446272,
                dagbin_base + 2564U);

            /* Pokable fields */
#ifdef CONV5_4__SEP_____BN_CONV5_4__SEP__SCALE_MUL___MULI___68_____BN_CONV5_4__SEP__SCALE_MUL_____SCALE___RELU5_4__SEP_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv5_4__sep_____bn_conv5_4__sep__scale_mul___muli___68_____bn_conv5_4__sep__scale_mul_____scale___relu5_4__sep_dram_pitch
            if (o_args->conv5_4__sep_____bn_conv5_4__sep__scale_mul___muli___68_____bn_conv5_4__sep__scale_mul_____scale___relu5_4__sep_dram_pitch_enable)
                coproc_poke_half_offset(o_args->conv5_4__sep_____bn_conv5_4__sep__scale_mul___muli___68_____bn_conv5_4__sep__scale_mul_____scale___relu5_4__sep_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef __PVCN_1879__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1879__dram_pitch
            if (o_args->__pvcn_1879__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_1879__dram_pitch_value, dagbin_base + 1268U);

#endif

#ifdef __PVCN_1890__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1890__dram_pitch
            if (o_args->__pvcn_1890__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_1890__dram_pitch_value, dagbin_base + 1368U);

#endif

#ifdef __PVCN_1901__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1901__dram_pitch
            if (o_args->__pvcn_1901__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_1901__dram_pitch_value, dagbin_base + 1468U);

#endif

#ifdef __PVCN_1912__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1912__dram_pitch
            if (o_args->__pvcn_1912__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_1912__dram_pitch_value, dagbin_base + 1568U);

#endif

#ifdef __PVCN_1923__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1923__dram_pitch
            if (o_args->__pvcn_1923__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_1923__dram_pitch_value, dagbin_base + 1668U);

#endif

#ifdef __PVCN_1934__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1934__dram_pitch
            if (o_args->__pvcn_1934__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_1934__dram_pitch_value, dagbin_base + 1768U);

#endif

#ifdef __PVCN_1945__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1945__dram_pitch
            if (o_args->__pvcn_1945__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_1945__dram_pitch_value, dagbin_base + 1868U);

#endif

#ifdef __PVCN_1956__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1956__dram_pitch
            if (o_args->__pvcn_1956__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_1956__dram_pitch_value, dagbin_base + 1968U);

#endif

#ifdef __PVCN_1967__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1967__dram_pitch
            if (o_args->__pvcn_1967__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_1967__dram_pitch_value, dagbin_base + 2068U);

#endif

#ifdef __PVCN_1978__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1978__dram_pitch
            if (o_args->__pvcn_1978__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_1978__dram_pitch_value, dagbin_base + 2168U);

#endif

#ifdef __PVCN_1989__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1989__dram_pitch
            if (o_args->__pvcn_1989__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_1989__dram_pitch_value, dagbin_base + 2268U);

#endif

#ifdef __PVCN_2000__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2000__dram_pitch
            if (o_args->__pvcn_2000__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2000__dram_pitch_value, dagbin_base + 2368U);

#endif

#ifdef __PVCN_2011__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2011__dram_pitch
            if (o_args->__pvcn_2011__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2011__dram_pitch_value, dagbin_base + 2468U);

#endif

#ifdef __PVCN_2022__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2022__dram_pitch
            if (o_args->__pvcn_2022__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2022__dram_pitch_value, dagbin_base + 2568U);

#endif

            /*** dmald() calls ***/
            break;
        case 19:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: __pvcn_1879_
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 508U);

            // Poke HMB intermediate input buffer: __pvcn_1890_
            coproc_poke_word(dag_base + 33792,
                dagbin_base + 544U);

            // Poke HMB intermediate input buffer: __pvcn_1901_
            coproc_poke_word_offset(dag_base + 67584,
                dagbin_base, 148U);

            // Poke HMB intermediate input buffer: __pvcn_1912_
            coproc_poke_word_offset(dag_base + 104064,
                dagbin_base, 68U);

            // Poke HMB intermediate input buffer: __pvcn_1923_
            coproc_poke_word_offset(dag_base + 136896,
                dagbin_base, 428U);

            // Poke HMB intermediate input buffer: __pvcn_1934_
            coproc_poke_word_offset(dag_base + 170688,
                dagbin_base, 468U);

            // Poke HMB intermediate input buffer: __pvcn_1945_
            coproc_poke_word_offset(dag_base + 203520,
                dagbin_base, 348U);

            // Poke HMB intermediate input buffer: __pvcn_1956_
            coproc_poke_word_offset(dag_base + 240000,
                dagbin_base, 388U);

            // Poke HMB intermediate input buffer: __pvcn_1967_
            coproc_poke_word_offset(dag_base + 273792,
                dagbin_base, 308U);

            // Poke HMB intermediate input buffer: __pvcn_1978_
            coproc_poke_word_offset(dag_base + 311232,
                dagbin_base, 268U);

            // Poke HMB intermediate input buffer: __pvcn_1989_
            coproc_poke_word_offset(dag_base + 345024,
                dagbin_base, 228U);

            // Poke HMB intermediate input buffer: __pvcn_2000_
            coproc_poke_word_offset(dag_base + 377856,
                dagbin_base, 28U);

            // Poke HMB intermediate input buffer: __pvcn_2011_
            coproc_poke_word_offset(dag_base + 409792,
                dagbin_base, 188U);

            // Poke HMB intermediate input buffer: __pvcn_2022_
            coproc_poke_word_offset(dag_base + 446272,
                dagbin_base, 108U);

            // Poke HMB intermediate output buffer: conv5_5__sep_____bn_conv5_5__sep__scale_mul___muli___74_____bn_conv5_5__sep__scale_mul_____scale___relu5_5__sep
            coproc_poke_word(dag_base + 467264,
                dagbin_base + 1196U);

            /* Pokable fields */
#ifdef __PVCN_2000__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2000__dram_pitch
            if (o_args->__pvcn_2000__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2000__dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef __PVCN_1912__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1912__dram_pitch
            if (o_args->__pvcn_1912__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_1912__dram_pitch_value, dagbin_base, 
                    72U);

#endif

#ifdef __PVCN_2022__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2022__dram_pitch
            if (o_args->__pvcn_2022__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2022__dram_pitch_value, dagbin_base, 
                    112U);

#endif

#ifdef __PVCN_1901__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1901__dram_pitch
            if (o_args->__pvcn_1901__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_1901__dram_pitch_value, dagbin_base, 
                    152U);

#endif

#ifdef __PVCN_2011__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2011__dram_pitch
            if (o_args->__pvcn_2011__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2011__dram_pitch_value, dagbin_base, 
                    192U);

#endif

#ifdef __PVCN_1989__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1989__dram_pitch
            if (o_args->__pvcn_1989__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_1989__dram_pitch_value, dagbin_base, 
                    232U);

#endif

#ifdef __PVCN_1978__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1978__dram_pitch
            if (o_args->__pvcn_1978__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_1978__dram_pitch_value, dagbin_base, 
                    272U);

#endif

#ifdef __PVCN_1967__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1967__dram_pitch
            if (o_args->__pvcn_1967__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_1967__dram_pitch_value, dagbin_base, 
                    312U);

#endif

#ifdef __PVCN_1945__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1945__dram_pitch
            if (o_args->__pvcn_1945__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_1945__dram_pitch_value, dagbin_base, 
                    352U);

#endif

#ifdef __PVCN_1956__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1956__dram_pitch
            if (o_args->__pvcn_1956__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_1956__dram_pitch_value, dagbin_base, 
                    392U);

#endif

#ifdef __PVCN_1923__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1923__dram_pitch
            if (o_args->__pvcn_1923__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_1923__dram_pitch_value, dagbin_base, 
                    432U);

#endif

#ifdef __PVCN_1934__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1934__dram_pitch
            if (o_args->__pvcn_1934__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_1934__dram_pitch_value, dagbin_base, 
                    472U);

#endif

#ifdef __PVCN_1879__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1879__dram_pitch
            if (o_args->__pvcn_1879__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_1879__dram_pitch_value, dagbin_base + 512U);

#endif

#ifdef __PVCN_1890__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_1890__dram_pitch
            if (o_args->__pvcn_1890__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_1890__dram_pitch_value, dagbin_base + 548U);

#endif

#ifdef CONV5_5__SEP_____BN_CONV5_5__SEP__SCALE_MUL___MULI___74_____BN_CONV5_5__SEP__SCALE_MUL_____SCALE___RELU5_5__SEP_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv5_5__sep_____bn_conv5_5__sep__scale_mul___muli___74_____bn_conv5_5__sep__scale_mul_____scale___relu5_5__sep_dram_pitch
            if (o_args->conv5_5__sep_____bn_conv5_5__sep__scale_mul___muli___74_____bn_conv5_5__sep__scale_mul_____scale___relu5_5__sep_dram_pitch_enable)
                coproc_poke_half(o_args->conv5_5__sep_____bn_conv5_5__sep__scale_mul___muli___74_____bn_conv5_5__sep__scale_mul_____scale___relu5_5__sep_dram_pitch_value, dagbin_base + 1200U);

#endif

            /*** dmald() calls ***/
            break;
        case 20:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: conv5_5__sep_____bn_conv5_5__sep__scale_mul___muli___74_____bn_conv5_5__sep__scale_mul_____scale___relu5_5__sep
            coproc_poke_word_offset(dag_base + 467264,
                dagbin_base, 28U);

            // Poke HMB intermediate output buffer: __pvcn_2100_
            coproc_poke_word(dag_base + 0,
                dagbin_base + 1300U);

            // Poke HMB intermediate output buffer: __pvcn_2111_
            coproc_poke_word(dag_base + 9408,
                dagbin_base + 1356U);

            // Poke HMB intermediate output buffer: __pvcn_2122_
            coproc_poke_word(dag_base + 19008,
                dagbin_base + 1412U);

            // Poke HMB intermediate output buffer: __pvcn_2133_
            coproc_poke_word(dag_base + 27648,
                dagbin_base + 1468U);

            // Poke HMB intermediate output buffer: __pvcn_2144_
            coproc_poke_word(dag_base + 36096,
                dagbin_base + 1524U);

            // Poke HMB intermediate output buffer: __pvcn_2155_
            coproc_poke_word(dag_base + 45504,
                dagbin_base + 1580U);

            // Poke HMB intermediate output buffer: __pvcn_2166_
            coproc_poke_word(dag_base + 55360,
                dagbin_base + 1636U);

            // Poke HMB intermediate output buffer: __pvcn_2177_
            coproc_poke_word(dag_base + 64960,
                dagbin_base + 1692U);

            // Poke HMB intermediate output buffer: __pvcn_2188_
            coproc_poke_word(dag_base + 73856,
                dagbin_base + 1748U);

            // Poke HMB intermediate output buffer: __pvcn_2199_
            coproc_poke_word(dag_base + 82496,
                dagbin_base + 1804U);

            // Poke HMB intermediate output buffer: __pvcn_2210_
            coproc_poke_word(dag_base + 92352,
                dagbin_base + 1860U);

            // Poke HMB intermediate output buffer: __pvcn_2221_
            coproc_poke_word(dag_base + 101248,
                dagbin_base + 1916U);

            // Poke HMB intermediate output buffer: __pvcn_2232_
            coproc_poke_word(dag_base + 110848,
                dagbin_base + 1972U);

            // Poke HMB intermediate output buffer: __pvcn_2243_
            coproc_poke_word(dag_base + 119488,
                dagbin_base + 2028U);

            // Poke HMB intermediate output buffer: conv5_5_mbox_conf___conv2i___76______loc___conv2i___136______muli___370___1
            coproc_poke_word(dag_base + 123136,
                dagbin_base + 2080U);

            /* Pokable fields */
#ifdef CONV5_5__SEP_____BN_CONV5_5__SEP__SCALE_MUL___MULI___74_____BN_CONV5_5__SEP__SCALE_MUL_____SCALE___RELU5_5__SEP_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv5_5__sep_____bn_conv5_5__sep__scale_mul___muli___74_____bn_conv5_5__sep__scale_mul_____scale___relu5_5__sep_dram_pitch
            if (o_args->conv5_5__sep_____bn_conv5_5__sep__scale_mul___muli___74_____bn_conv5_5__sep__scale_mul_____scale___relu5_5__sep_dram_pitch_enable)
                coproc_poke_half_offset(o_args->conv5_5__sep_____bn_conv5_5__sep__scale_mul___muli___74_____bn_conv5_5__sep__scale_mul_____scale___relu5_5__sep_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef __PVCN_2100__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2100__dram_pitch
            if (o_args->__pvcn_2100__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2100__dram_pitch_value, dagbin_base + 1304U);

#endif

#ifdef __PVCN_2111__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2111__dram_pitch
            if (o_args->__pvcn_2111__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2111__dram_pitch_value, dagbin_base + 1360U);

#endif

#ifdef __PVCN_2122__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2122__dram_pitch
            if (o_args->__pvcn_2122__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2122__dram_pitch_value, dagbin_base + 1416U);

#endif

#ifdef __PVCN_2133__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2133__dram_pitch
            if (o_args->__pvcn_2133__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2133__dram_pitch_value, dagbin_base + 1472U);

#endif

#ifdef __PVCN_2144__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2144__dram_pitch
            if (o_args->__pvcn_2144__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2144__dram_pitch_value, dagbin_base + 1528U);

#endif

#ifdef __PVCN_2155__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2155__dram_pitch
            if (o_args->__pvcn_2155__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2155__dram_pitch_value, dagbin_base + 1584U);

#endif

#ifdef __PVCN_2166__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2166__dram_pitch
            if (o_args->__pvcn_2166__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2166__dram_pitch_value, dagbin_base + 1640U);

#endif

#ifdef __PVCN_2177__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2177__dram_pitch
            if (o_args->__pvcn_2177__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2177__dram_pitch_value, dagbin_base + 1696U);

#endif

#ifdef __PVCN_2188__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2188__dram_pitch
            if (o_args->__pvcn_2188__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2188__dram_pitch_value, dagbin_base + 1752U);

#endif

#ifdef __PVCN_2199__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2199__dram_pitch
            if (o_args->__pvcn_2199__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2199__dram_pitch_value, dagbin_base + 1808U);

#endif

#ifdef __PVCN_2210__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2210__dram_pitch
            if (o_args->__pvcn_2210__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2210__dram_pitch_value, dagbin_base + 1864U);

#endif

#ifdef __PVCN_2221__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2221__dram_pitch
            if (o_args->__pvcn_2221__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2221__dram_pitch_value, dagbin_base + 1920U);

#endif

#ifdef __PVCN_2232__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2232__dram_pitch
            if (o_args->__pvcn_2232__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2232__dram_pitch_value, dagbin_base + 1976U);

#endif

#ifdef __PVCN_2243__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2243__dram_pitch
            if (o_args->__pvcn_2243__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2243__dram_pitch_value, dagbin_base + 2032U);

#endif

#ifdef CONV5_5_MBOX_CONF___CONV2I___76______LOC___CONV2I___136______MULI___370___1_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv5_5_mbox_conf___conv2i___76______loc___conv2i___136______muli___370___1_dram_pitch
            if (o_args->conv5_5_mbox_conf___conv2i___76______loc___conv2i___136______muli___370___1_dram_pitch_enable)
                coproc_poke_half(o_args->conv5_5_mbox_conf___conv2i___76______loc___conv2i___136______muli___370___1_dram_pitch_value, dagbin_base + 2084U);

#endif

            /*** dmald() calls ***/
            break;
        case 21:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: __pvcn_2100_
            coproc_poke_word(dag_base + 0,
                dagbin_base + 588U);

            // Poke HMB intermediate input buffer: __pvcn_2111_
            coproc_poke_word(dag_base + 9408,
                dagbin_base + 548U);

            // Poke HMB intermediate input buffer: __pvcn_2122_
            coproc_poke_word_offset(dag_base + 19008,
                dagbin_base, 508U);

            // Poke HMB intermediate input buffer: __pvcn_2133_
            coproc_poke_word_offset(dag_base + 27648,
                dagbin_base, 468U);

            // Poke HMB intermediate input buffer: __pvcn_2144_
            coproc_poke_word_offset(dag_base + 36096,
                dagbin_base, 308U);

            // Poke HMB intermediate input buffer: __pvcn_2155_
            coproc_poke_word_offset(dag_base + 45504,
                dagbin_base, 188U);

            // Poke HMB intermediate input buffer: __pvcn_2166_
            coproc_poke_word_offset(dag_base + 55360,
                dagbin_base, 28U);

            // Poke HMB intermediate input buffer: __pvcn_2177_
            coproc_poke_word_offset(dag_base + 64960,
                dagbin_base, 108U);

            // Poke HMB intermediate input buffer: __pvcn_2188_
            coproc_poke_word_offset(dag_base + 73856,
                dagbin_base, 148U);

            // Poke HMB intermediate input buffer: __pvcn_2199_
            coproc_poke_word_offset(dag_base + 82496,
                dagbin_base, 348U);

            // Poke HMB intermediate input buffer: __pvcn_2210_
            coproc_poke_word_offset(dag_base + 92352,
                dagbin_base, 228U);

            // Poke HMB intermediate input buffer: __pvcn_2221_
            coproc_poke_word_offset(dag_base + 101248,
                dagbin_base, 268U);

            // Poke HMB intermediate input buffer: __pvcn_2232_
            coproc_poke_word_offset(dag_base + 110848,
                dagbin_base, 428U);

            // Poke HMB intermediate input buffer: __pvcn_2243_
            coproc_poke_word_offset(dag_base + 119488,
                dagbin_base, 68U);

            // Poke HMB intermediate input buffer: conv5_5_mbox_conf___conv2i___76______loc___conv2i___136______muli___370___1
            coproc_poke_word_offset(dag_base + 123136,
                dagbin_base, 388U);

            // Poke HMB intermediate output buffer: __pvcn_2094_
            coproc_poke_word(dag_base + 138240,
                dagbin_base + 636U);

            // Poke HMB intermediate output buffer: __pvcn_2105_
            coproc_poke_word(dag_base + 147648,
                dagbin_base + 684U);

            // Poke HMB intermediate output buffer: __pvcn_2116_
            coproc_poke_word(dag_base + 157248,
                dagbin_base + 732U);

            // Poke HMB intermediate output buffer: __pvcn_2127_
            coproc_poke_word(dag_base + 165888,
                dagbin_base + 780U);

            // Poke HMB intermediate output buffer: __pvcn_2138_
            coproc_poke_word(dag_base + 174336,
                dagbin_base + 828U);

            // Poke HMB intermediate output buffer: __pvcn_2149_
            coproc_poke_word(dag_base + 183744,
                dagbin_base + 876U);

            // Poke HMB intermediate output buffer: __pvcn_2160_
            coproc_poke_word(dag_base + 193600,
                dagbin_base + 924U);

            // Poke HMB intermediate output buffer: __pvcn_2171_
            coproc_poke_word(dag_base + 203200,
                dagbin_base + 972U);

            // Poke HMB intermediate output buffer: __pvcn_2182_
            coproc_poke_word(dag_base + 212096,
                dagbin_base + 1020U);

            // Poke HMB intermediate output buffer: __pvcn_2193_
            coproc_poke_word(dag_base + 220736,
                dagbin_base + 1068U);

            // Poke HMB intermediate output buffer: __pvcn_2204_
            coproc_poke_word(dag_base + 230592,
                dagbin_base + 1116U);

            // Poke HMB intermediate output buffer: __pvcn_2215_
            coproc_poke_word(dag_base + 239488,
                dagbin_base + 1164U);

            // Poke HMB intermediate output buffer: __pvcn_2226_
            coproc_poke_word(dag_base + 249088,
                dagbin_base + 1212U);

            // Poke HMB intermediate output buffer: __pvcn_2237_
            coproc_poke_word(dag_base + 257728,
                dagbin_base + 1260U);

            // Poke HMB intermediate output buffer: conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___321
            coproc_poke_word(dag_base + 261376,
                dagbin_base + 1312U);

            // Poke HMB intermediate output buffer: conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___320
            coproc_poke_word(dag_base + 275968,
                dagbin_base + 1364U);

            /* Pokable fields */
#ifdef __PVCN_2166__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2166__dram_pitch
            if (o_args->__pvcn_2166__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2166__dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef __PVCN_2243__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2243__dram_pitch
            if (o_args->__pvcn_2243__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2243__dram_pitch_value, dagbin_base, 
                    72U);

#endif

#ifdef __PVCN_2177__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2177__dram_pitch
            if (o_args->__pvcn_2177__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2177__dram_pitch_value, dagbin_base, 
                    112U);

#endif

#ifdef __PVCN_2188__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2188__dram_pitch
            if (o_args->__pvcn_2188__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2188__dram_pitch_value, dagbin_base, 
                    152U);

#endif

#ifdef __PVCN_2155__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2155__dram_pitch
            if (o_args->__pvcn_2155__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2155__dram_pitch_value, dagbin_base, 
                    192U);

#endif

#ifdef __PVCN_2210__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2210__dram_pitch
            if (o_args->__pvcn_2210__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2210__dram_pitch_value, dagbin_base, 
                    232U);

#endif

#ifdef __PVCN_2221__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2221__dram_pitch
            if (o_args->__pvcn_2221__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2221__dram_pitch_value, dagbin_base, 
                    272U);

#endif

#ifdef __PVCN_2144__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2144__dram_pitch
            if (o_args->__pvcn_2144__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2144__dram_pitch_value, dagbin_base, 
                    312U);

#endif

#ifdef __PVCN_2199__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2199__dram_pitch
            if (o_args->__pvcn_2199__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2199__dram_pitch_value, dagbin_base, 
                    352U);

#endif

#ifdef CONV5_5_MBOX_CONF___CONV2I___76______LOC___CONV2I___136______MULI___370___1_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv5_5_mbox_conf___conv2i___76______loc___conv2i___136______muli___370___1_dram_pitch
            if (o_args->conv5_5_mbox_conf___conv2i___76______loc___conv2i___136______muli___370___1_dram_pitch_enable)
                coproc_poke_half_offset(o_args->conv5_5_mbox_conf___conv2i___76______loc___conv2i___136______muli___370___1_dram_pitch_value, dagbin_base, 
                    392U);

#endif

#ifdef __PVCN_2232__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2232__dram_pitch
            if (o_args->__pvcn_2232__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2232__dram_pitch_value, dagbin_base, 
                    432U);

#endif

#ifdef __PVCN_2133__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2133__dram_pitch
            if (o_args->__pvcn_2133__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2133__dram_pitch_value, dagbin_base, 
                    472U);

#endif

#ifdef __PVCN_2122__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2122__dram_pitch
            if (o_args->__pvcn_2122__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2122__dram_pitch_value, dagbin_base + 512U);

#endif

#ifdef __PVCN_2111__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2111__dram_pitch
            if (o_args->__pvcn_2111__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2111__dram_pitch_value, dagbin_base + 552U);

#endif

#ifdef __PVCN_2100__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2100__dram_pitch
            if (o_args->__pvcn_2100__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2100__dram_pitch_value, dagbin_base + 592U);

#endif

#ifdef __PVCN_2094__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2094__dram_pitch
            if (o_args->__pvcn_2094__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2094__dram_pitch_value, dagbin_base + 640U);

#endif

#ifdef __PVCN_2105__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2105__dram_pitch
            if (o_args->__pvcn_2105__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2105__dram_pitch_value, dagbin_base + 688U);

#endif

#ifdef __PVCN_2116__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2116__dram_pitch
            if (o_args->__pvcn_2116__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2116__dram_pitch_value, dagbin_base + 736U);

#endif

#ifdef __PVCN_2127__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2127__dram_pitch
            if (o_args->__pvcn_2127__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2127__dram_pitch_value, dagbin_base + 784U);

#endif

#ifdef __PVCN_2138__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2138__dram_pitch
            if (o_args->__pvcn_2138__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2138__dram_pitch_value, dagbin_base + 832U);

#endif

#ifdef __PVCN_2149__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2149__dram_pitch
            if (o_args->__pvcn_2149__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2149__dram_pitch_value, dagbin_base + 880U);

#endif

#ifdef __PVCN_2160__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2160__dram_pitch
            if (o_args->__pvcn_2160__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2160__dram_pitch_value, dagbin_base + 928U);

#endif

#ifdef __PVCN_2171__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2171__dram_pitch
            if (o_args->__pvcn_2171__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2171__dram_pitch_value, dagbin_base + 976U);

#endif

#ifdef __PVCN_2182__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2182__dram_pitch
            if (o_args->__pvcn_2182__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2182__dram_pitch_value, dagbin_base + 1024U);

#endif

#ifdef __PVCN_2193__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2193__dram_pitch
            if (o_args->__pvcn_2193__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2193__dram_pitch_value, dagbin_base + 1072U);

#endif

#ifdef __PVCN_2204__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2204__dram_pitch
            if (o_args->__pvcn_2204__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2204__dram_pitch_value, dagbin_base + 1120U);

#endif

#ifdef __PVCN_2215__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2215__dram_pitch
            if (o_args->__pvcn_2215__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2215__dram_pitch_value, dagbin_base + 1168U);

#endif

#ifdef __PVCN_2226__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2226__dram_pitch
            if (o_args->__pvcn_2226__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2226__dram_pitch_value, dagbin_base + 1216U);

#endif

#ifdef __PVCN_2237__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2237__dram_pitch
            if (o_args->__pvcn_2237__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2237__dram_pitch_value, dagbin_base + 1264U);

#endif

#ifdef CONV5_5_MBOX_CONF___CONV2I___76______LOC___CONV2I___136___CROP___321_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___321_dram_pitch
            if (o_args->conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___321_dram_pitch_enable)
                coproc_poke_half(o_args->conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___321_dram_pitch_value, dagbin_base + 1316U);

#endif

#ifdef CONV5_5_MBOX_CONF___CONV2I___76______LOC___CONV2I___136___CROP___320_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___320_dram_pitch
            if (o_args->conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___320_dram_pitch_enable)
                coproc_poke_half(o_args->conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___320_dram_pitch_value, dagbin_base + 1368U);

#endif

            /*** dmald() calls ***/
            break;
        case 22:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: __pvcn_2094_
            coproc_poke_word(dag_base + 138240,
                dagbin_base + 588U);

            // Poke HMB intermediate input buffer: __pvcn_2105_
            coproc_poke_word(dag_base + 147648,
                dagbin_base + 624U);

            // Poke HMB intermediate input buffer: __pvcn_2116_
            coproc_poke_word_offset(dag_base + 157248,
                dagbin_base, 108U);

            // Poke HMB intermediate input buffer: __pvcn_2127_
            coproc_poke_word(dag_base + 165888,
                dagbin_base + 548U);

            // Poke HMB intermediate input buffer: __pvcn_2138_
            coproc_poke_word_offset(dag_base + 174336,
                dagbin_base, 268U);

            // Poke HMB intermediate input buffer: __pvcn_2149_
            coproc_poke_word_offset(dag_base + 183744,
                dagbin_base, 508U);

            // Poke HMB intermediate input buffer: __pvcn_2160_
            coproc_poke_word_offset(dag_base + 193600,
                dagbin_base, 468U);

            // Poke HMB intermediate input buffer: __pvcn_2171_
            coproc_poke_word_offset(dag_base + 203200,
                dagbin_base, 188U);

            // Poke HMB intermediate input buffer: __pvcn_2182_
            coproc_poke_word_offset(dag_base + 212096,
                dagbin_base, 148U);

            // Poke HMB intermediate input buffer: __pvcn_2193_
            coproc_poke_word_offset(dag_base + 220736,
                dagbin_base, 428U);

            // Poke HMB intermediate input buffer: __pvcn_2204_
            coproc_poke_word_offset(dag_base + 230592,
                dagbin_base, 388U);

            // Poke HMB intermediate input buffer: __pvcn_2215_
            coproc_poke_word_offset(dag_base + 239488,
                dagbin_base, 68U);

            // Poke HMB intermediate input buffer: __pvcn_2226_
            coproc_poke_word_offset(dag_base + 249088,
                dagbin_base, 28U);

            // Poke HMB intermediate input buffer: __pvcn_2237_
            coproc_poke_word_offset(dag_base + 257728,
                dagbin_base, 348U);

            // Poke HMB intermediate input buffer: conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___321
            coproc_poke_word_offset(dag_base + 261376,
                dagbin_base, 308U);

            // Poke HMB intermediate input buffer: conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___320
            coproc_poke_word_offset(dag_base + 275968,
                dagbin_base, 228U);

            // Poke HMB intermediate output buffer: conv5_6__dw_____bn_conv5_6__dw__scale_mul___muli___79_____bn_conv5_6__dw__scale_mul_____scale___relu5_6__dw
            coproc_poke_word(dag_base + 0,
                dagbin_base + 1100U);

            // Poke HMB intermediate output buffer: conv5_5_mbox_loc_flat
            coproc_poke_word(dag_base + 122880,
                dagbin_base + 1160U);

            // Poke HMB intermediate output buffer: conv5_5_mbox_conf_flat
            coproc_poke_word(dag_base + 127232,
                dagbin_base + 1220U);

            /* Pokable fields */
#ifdef __PVCN_2226__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2226__dram_pitch
            if (o_args->__pvcn_2226__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2226__dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef __PVCN_2215__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2215__dram_pitch
            if (o_args->__pvcn_2215__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2215__dram_pitch_value, dagbin_base, 
                    72U);

#endif

#ifdef __PVCN_2116__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2116__dram_pitch
            if (o_args->__pvcn_2116__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2116__dram_pitch_value, dagbin_base, 
                    112U);

#endif

#ifdef __PVCN_2182__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2182__dram_pitch
            if (o_args->__pvcn_2182__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2182__dram_pitch_value, dagbin_base, 
                    152U);

#endif

#ifdef __PVCN_2171__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2171__dram_pitch
            if (o_args->__pvcn_2171__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2171__dram_pitch_value, dagbin_base, 
                    192U);

#endif

#ifdef CONV5_5_MBOX_CONF___CONV2I___76______LOC___CONV2I___136___CROP___320_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___320_dram_pitch
            if (o_args->conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___320_dram_pitch_enable)
                coproc_poke_half_offset(o_args->conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___320_dram_pitch_value, dagbin_base, 
                    232U);

#endif

#ifdef __PVCN_2138__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2138__dram_pitch
            if (o_args->__pvcn_2138__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2138__dram_pitch_value, dagbin_base, 
                    272U);

#endif

#ifdef CONV5_5_MBOX_CONF___CONV2I___76______LOC___CONV2I___136___CROP___321_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___321_dram_pitch
            if (o_args->conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___321_dram_pitch_enable)
                coproc_poke_half_offset(o_args->conv5_5_mbox_conf___conv2i___76______loc___conv2i___136___crop___321_dram_pitch_value, dagbin_base, 
                    312U);

#endif

#ifdef __PVCN_2237__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2237__dram_pitch
            if (o_args->__pvcn_2237__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2237__dram_pitch_value, dagbin_base, 
                    352U);

#endif

#ifdef __PVCN_2204__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2204__dram_pitch
            if (o_args->__pvcn_2204__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2204__dram_pitch_value, dagbin_base, 
                    392U);

#endif

#ifdef __PVCN_2193__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2193__dram_pitch
            if (o_args->__pvcn_2193__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2193__dram_pitch_value, dagbin_base, 
                    432U);

#endif

#ifdef __PVCN_2160__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2160__dram_pitch
            if (o_args->__pvcn_2160__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2160__dram_pitch_value, dagbin_base, 
                    472U);

#endif

#ifdef __PVCN_2149__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2149__dram_pitch
            if (o_args->__pvcn_2149__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2149__dram_pitch_value, dagbin_base + 512U);

#endif

#ifdef __PVCN_2127__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2127__dram_pitch
            if (o_args->__pvcn_2127__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2127__dram_pitch_value, dagbin_base + 552U);

#endif

#ifdef __PVCN_2094__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2094__dram_pitch
            if (o_args->__pvcn_2094__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2094__dram_pitch_value, dagbin_base + 592U);

#endif

#ifdef __PVCN_2105__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2105__dram_pitch
            if (o_args->__pvcn_2105__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2105__dram_pitch_value, dagbin_base + 628U);

#endif

#ifdef CONV5_6__DW_____BN_CONV5_6__DW__SCALE_MUL___MULI___79_____BN_CONV5_6__DW__SCALE_MUL_____SCALE___RELU5_6__DW_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv5_6__dw_____bn_conv5_6__dw__scale_mul___muli___79_____bn_conv5_6__dw__scale_mul_____scale___relu5_6__dw_dram_pitch
            if (o_args->conv5_6__dw_____bn_conv5_6__dw__scale_mul___muli___79_____bn_conv5_6__dw__scale_mul_____scale___relu5_6__dw_dram_pitch_enable)
                coproc_poke_half(o_args->conv5_6__dw_____bn_conv5_6__dw__scale_mul___muli___79_____bn_conv5_6__dw__scale_mul_____scale___relu5_6__dw_dram_pitch_value, dagbin_base + 1104U);

#endif

#ifdef CONV5_5_MBOX_LOC_FLAT_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv5_5_mbox_loc_flat_dram_pitch
            if (o_args->conv5_5_mbox_loc_flat_dram_pitch_enable)
                coproc_poke_half(o_args->conv5_5_mbox_loc_flat_dram_pitch_value, dagbin_base + 1164U);

#endif

#ifdef CONV5_5_MBOX_CONF_FLAT_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv5_5_mbox_conf_flat_dram_pitch
            if (o_args->conv5_5_mbox_conf_flat_dram_pitch_enable)
                coproc_poke_half(o_args->conv5_5_mbox_conf_flat_dram_pitch_value, dagbin_base + 1224U);

#endif

            /*** dmald() calls ***/
            break;
        case 23:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: conv5_6__dw_____bn_conv5_6__dw__scale_mul___muli___79_____bn_conv5_6__dw__scale_mul_____scale___relu5_6__dw
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 28U);

            // Poke HMB intermediate output buffer: conv5_6__dw_____bn_conv5_6__dw__scale_mul___muli___79_____bn_conv5_6__dw__scale_mul_____scale___muli___358
            coproc_poke_word_offset(dag_base + 134848,
                dagbin_base, 168U);

            // Poke HMB intermediate output buffer: __pvcn_2281_
            coproc_poke_word_offset(dag_base + 196288,
                dagbin_base, 220U);

            /* Pokable fields */
#ifdef CONV5_6__DW_____BN_CONV5_6__DW__SCALE_MUL___MULI___79_____BN_CONV5_6__DW__SCALE_MUL_____SCALE___RELU5_6__DW_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv5_6__dw_____bn_conv5_6__dw__scale_mul___muli___79_____bn_conv5_6__dw__scale_mul_____scale___relu5_6__dw_dram_pitch
            if (o_args->conv5_6__dw_____bn_conv5_6__dw__scale_mul___muli___79_____bn_conv5_6__dw__scale_mul_____scale___relu5_6__dw_dram_pitch_enable)
                coproc_poke_half_offset(o_args->conv5_6__dw_____bn_conv5_6__dw__scale_mul___muli___79_____bn_conv5_6__dw__scale_mul_____scale___relu5_6__dw_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef CONV5_6__DW_____BN_CONV5_6__DW__SCALE_MUL___MULI___79_____BN_CONV5_6__DW__SCALE_MUL_____SCALE___MULI___358_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv5_6__dw_____bn_conv5_6__dw__scale_mul___muli___79_____bn_conv5_6__dw__scale_mul_____scale___muli___358_dram_pitch
            if (o_args->conv5_6__dw_____bn_conv5_6__dw__scale_mul___muli___79_____bn_conv5_6__dw__scale_mul_____scale___muli___358_dram_pitch_enable)
                coproc_poke_half_offset(o_args->conv5_6__dw_____bn_conv5_6__dw__scale_mul___muli___79_____bn_conv5_6__dw__scale_mul_____scale___muli___358_dram_pitch_value, dagbin_base, 
                    172U);

#endif

#ifdef __PVCN_2281__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2281__dram_pitch
            if (o_args->__pvcn_2281__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2281__dram_pitch_value, dagbin_base, 
                    224U);

#endif

            /*** dmald() calls ***/
            break;
        case 24:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: conv5_6__dw_____bn_conv5_6__dw__scale_mul___muli___79_____bn_conv5_6__dw__scale_mul_____scale___muli___358
            coproc_poke_word_offset(dag_base + 134848,
                dagbin_base, 28U);

            // Poke HMB intermediate output buffer: __pvcn_2292_
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 156U);

            /* Pokable fields */
#ifdef CONV5_6__DW_____BN_CONV5_6__DW__SCALE_MUL___MULI___79_____BN_CONV5_6__DW__SCALE_MUL_____SCALE___MULI___358_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv5_6__dw_____bn_conv5_6__dw__scale_mul___muli___79_____bn_conv5_6__dw__scale_mul_____scale___muli___358_dram_pitch
            if (o_args->conv5_6__dw_____bn_conv5_6__dw__scale_mul___muli___79_____bn_conv5_6__dw__scale_mul_____scale___muli___358_dram_pitch_enable)
                coproc_poke_half_offset(o_args->conv5_6__dw_____bn_conv5_6__dw__scale_mul___muli___79_____bn_conv5_6__dw__scale_mul_____scale___muli___358_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef __PVCN_2292__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2292__dram_pitch
            if (o_args->__pvcn_2292__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2292__dram_pitch_value, dagbin_base, 
                    160U);

#endif

            /*** dmald() calls ***/
            break;
        case 25:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: __pvcn_2292_
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 64U);

            // Poke HMB intermediate input buffer: __pvcn_2281_
            coproc_poke_word_offset(dag_base + 196288,
                dagbin_base, 28U);

            // Poke HMB intermediate output buffer: __pvcn_2365_
            coproc_poke_word(dag_base + 134848,
                dagbin_base + 1504U);

            // Poke HMB intermediate output buffer: __pvcn_2376_
            coproc_poke_word(dag_base + 153600,
                dagbin_base + 1560U);

            // Poke HMB intermediate output buffer: __pvcn_2387_
            coproc_poke_word(dag_base + 170176,
                dagbin_base + 1616U);

            // Poke HMB intermediate output buffer: __pvcn_2519_
            coproc_poke_word(dag_base + 188224,
                dagbin_base + 2288U);

            // Poke HMB intermediate output buffer: __pvcn_2398_
            coproc_poke_word(dag_base + 319168,
                dagbin_base + 1672U);

            // Poke HMB intermediate output buffer: __pvcn_2409_
            coproc_poke_word(dag_base + 337216,
                dagbin_base + 1728U);

            // Poke HMB intermediate output buffer: __pvcn_2420_
            coproc_poke_word(dag_base + 354048,
                dagbin_base + 1784U);

            // Poke HMB intermediate output buffer: __pvcn_2431_
            coproc_poke_word(dag_base + 371328,
                dagbin_base + 1840U);

            // Poke HMB intermediate output buffer: __pvcn_2442_
            coproc_poke_word(dag_base + 388864,
                dagbin_base + 1896U);

            // Poke HMB intermediate output buffer: __pvcn_2453_
            coproc_poke_word(dag_base + 405696,
                dagbin_base + 1952U);

            // Poke HMB intermediate output buffer: __pvcn_2464_
            coproc_poke_word(dag_base + 423488,
                dagbin_base + 2008U);

            // Poke HMB intermediate output buffer: __pvcn_2475_
            coproc_poke_word(dag_base + 440768,
                dagbin_base + 2064U);

            // Poke HMB intermediate output buffer: __pvcn_2486_
            coproc_poke_word(dag_base + 459008,
                dagbin_base + 2120U);

            // Poke HMB intermediate output buffer: __pvcn_2497_
            coproc_poke_word(dag_base + 476096,
                dagbin_base + 2176U);

            // Poke HMB intermediate output buffer: __pvcn_2508_
            coproc_poke_word(dag_base + 493632,
                dagbin_base + 2232U);

            /* Pokable fields */
#ifdef __PVCN_2281__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2281__dram_pitch
            if (o_args->__pvcn_2281__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2281__dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef __PVCN_2292__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2292__dram_pitch
            if (o_args->__pvcn_2292__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2292__dram_pitch_value, dagbin_base, 
                    68U);

#endif

#ifdef __PVCN_2365__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2365__dram_pitch
            if (o_args->__pvcn_2365__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2365__dram_pitch_value, dagbin_base + 1508U);

#endif

#ifdef __PVCN_2376__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2376__dram_pitch
            if (o_args->__pvcn_2376__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2376__dram_pitch_value, dagbin_base + 1564U);

#endif

#ifdef __PVCN_2387__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2387__dram_pitch
            if (o_args->__pvcn_2387__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2387__dram_pitch_value, dagbin_base + 1620U);

#endif

#ifdef __PVCN_2398__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2398__dram_pitch
            if (o_args->__pvcn_2398__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2398__dram_pitch_value, dagbin_base + 1676U);

#endif

#ifdef __PVCN_2409__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2409__dram_pitch
            if (o_args->__pvcn_2409__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2409__dram_pitch_value, dagbin_base + 1732U);

#endif

#ifdef __PVCN_2420__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2420__dram_pitch
            if (o_args->__pvcn_2420__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2420__dram_pitch_value, dagbin_base + 1788U);

#endif

#ifdef __PVCN_2431__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2431__dram_pitch
            if (o_args->__pvcn_2431__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2431__dram_pitch_value, dagbin_base + 1844U);

#endif

#ifdef __PVCN_2442__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2442__dram_pitch
            if (o_args->__pvcn_2442__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2442__dram_pitch_value, dagbin_base + 1900U);

#endif

#ifdef __PVCN_2453__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2453__dram_pitch
            if (o_args->__pvcn_2453__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2453__dram_pitch_value, dagbin_base + 1956U);

#endif

#ifdef __PVCN_2464__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2464__dram_pitch
            if (o_args->__pvcn_2464__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2464__dram_pitch_value, dagbin_base + 2012U);

#endif

#ifdef __PVCN_2475__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2475__dram_pitch
            if (o_args->__pvcn_2475__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2475__dram_pitch_value, dagbin_base + 2068U);

#endif

#ifdef __PVCN_2486__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2486__dram_pitch
            if (o_args->__pvcn_2486__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2486__dram_pitch_value, dagbin_base + 2124U);

#endif

#ifdef __PVCN_2497__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2497__dram_pitch
            if (o_args->__pvcn_2497__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2497__dram_pitch_value, dagbin_base + 2180U);

#endif

#ifdef __PVCN_2508__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2508__dram_pitch
            if (o_args->__pvcn_2508__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2508__dram_pitch_value, dagbin_base + 2236U);

#endif

#ifdef __PVCN_2519__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2519__dram_pitch
            if (o_args->__pvcn_2519__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2519__dram_pitch_value, dagbin_base + 2292U);

#endif

            /*** dmald() calls ***/
            break;
        case 26:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: __pvcn_2365_
            coproc_poke_word_offset(dag_base + 134848,
                dagbin_base, 428U);

            // Poke HMB intermediate input buffer: __pvcn_2376_
            coproc_poke_word(dag_base + 153600,
                dagbin_base + 584U);

            // Poke HMB intermediate input buffer: __pvcn_2387_
            coproc_poke_word(dag_base + 170176,
                dagbin_base + 544U);

            // Poke HMB intermediate input buffer: __pvcn_2519_
            coproc_poke_word_offset(dag_base + 188224,
                dagbin_base, 148U);

            // Poke HMB intermediate input buffer: __pvcn_2398_
            coproc_poke_word_offset(dag_base + 319168,
                dagbin_base, 504U);

            // Poke HMB intermediate input buffer: __pvcn_2409_
            coproc_poke_word_offset(dag_base + 337216,
                dagbin_base, 464U);

            // Poke HMB intermediate input buffer: __pvcn_2420_
            coproc_poke_word_offset(dag_base + 354048,
                dagbin_base, 28U);

            // Poke HMB intermediate input buffer: __pvcn_2431_
            coproc_poke_word_offset(dag_base + 371328,
                dagbin_base, 348U);

            // Poke HMB intermediate input buffer: __pvcn_2442_
            coproc_poke_word_offset(dag_base + 388864,
                dagbin_base, 68U);

            // Poke HMB intermediate input buffer: __pvcn_2453_
            coproc_poke_word_offset(dag_base + 405696,
                dagbin_base, 108U);

            // Poke HMB intermediate input buffer: __pvcn_2464_
            coproc_poke_word_offset(dag_base + 423488,
                dagbin_base, 308U);

            // Poke HMB intermediate input buffer: __pvcn_2475_
            coproc_poke_word_offset(dag_base + 440768,
                dagbin_base, 228U);

            // Poke HMB intermediate input buffer: __pvcn_2486_
            coproc_poke_word_offset(dag_base + 459008,
                dagbin_base, 388U);

            // Poke HMB intermediate input buffer: __pvcn_2497_
            coproc_poke_word_offset(dag_base + 476096,
                dagbin_base, 188U);

            // Poke HMB intermediate input buffer: __pvcn_2508_
            coproc_poke_word_offset(dag_base + 493632,
                dagbin_base, 268U);

            // Poke HMB intermediate output buffer: __pvcn_2359_
            coproc_poke_word(dag_base + 0,
                dagbin_base + 636U);

            // Poke HMB intermediate output buffer: __pvcn_2370_
            coproc_poke_word(dag_base + 18752,
                dagbin_base + 684U);

            // Poke HMB intermediate output buffer: __pvcn_2381_
            coproc_poke_word(dag_base + 35328,
                dagbin_base + 732U);

            // Poke HMB intermediate output buffer: __pvcn_2392_
            coproc_poke_word(dag_base + 53376,
                dagbin_base + 780U);

            // Poke HMB intermediate output buffer: __pvcn_2403_
            coproc_poke_word(dag_base + 71424,
                dagbin_base + 828U);

            // Poke HMB intermediate output buffer: __pvcn_2414_
            coproc_poke_word(dag_base + 88256,
                dagbin_base + 876U);

            // Poke HMB intermediate output buffer: __pvcn_2436_
            coproc_poke_word(dag_base + 105536,
                dagbin_base + 972U);

            // Poke HMB intermediate output buffer: __pvcn_2425_
            coproc_poke_word(dag_base + 189696,
                dagbin_base + 924U);

            // Poke HMB intermediate output buffer: __pvcn_2447_
            coproc_poke_word(dag_base + 207232,
                dagbin_base + 1020U);

            // Poke HMB intermediate output buffer: __pvcn_2458_
            coproc_poke_word(dag_base + 225024,
                dagbin_base + 1068U);

            // Poke HMB intermediate output buffer: __pvcn_2469_
            coproc_poke_word(dag_base + 242304,
                dagbin_base + 1116U);

            // Poke HMB intermediate output buffer: __pvcn_2480_
            coproc_poke_word(dag_base + 260544,
                dagbin_base + 1164U);

            // Poke HMB intermediate output buffer: __pvcn_2491_
            coproc_poke_word(dag_base + 277632,
                dagbin_base + 1212U);

            // Poke HMB intermediate output buffer: __pvcn_2502_
            coproc_poke_word(dag_base + 295168,
                dagbin_base + 1260U);

            // Poke HMB intermediate output buffer: __pvcn_2513_
            coproc_poke_word(dag_base + 312000,
                dagbin_base + 1308U);

            /* Pokable fields */
#ifdef __PVCN_2420__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2420__dram_pitch
            if (o_args->__pvcn_2420__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2420__dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef __PVCN_2442__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2442__dram_pitch
            if (o_args->__pvcn_2442__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2442__dram_pitch_value, dagbin_base, 
                    72U);

#endif

#ifdef __PVCN_2453__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2453__dram_pitch
            if (o_args->__pvcn_2453__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2453__dram_pitch_value, dagbin_base, 
                    112U);

#endif

#ifdef __PVCN_2519__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2519__dram_pitch
            if (o_args->__pvcn_2519__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2519__dram_pitch_value, dagbin_base, 
                    152U);

#endif

#ifdef __PVCN_2497__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2497__dram_pitch
            if (o_args->__pvcn_2497__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2497__dram_pitch_value, dagbin_base, 
                    192U);

#endif

#ifdef __PVCN_2475__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2475__dram_pitch
            if (o_args->__pvcn_2475__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2475__dram_pitch_value, dagbin_base, 
                    232U);

#endif

#ifdef __PVCN_2508__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2508__dram_pitch
            if (o_args->__pvcn_2508__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2508__dram_pitch_value, dagbin_base, 
                    272U);

#endif

#ifdef __PVCN_2464__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2464__dram_pitch
            if (o_args->__pvcn_2464__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2464__dram_pitch_value, dagbin_base, 
                    312U);

#endif

#ifdef __PVCN_2431__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2431__dram_pitch
            if (o_args->__pvcn_2431__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2431__dram_pitch_value, dagbin_base, 
                    352U);

#endif

#ifdef __PVCN_2486__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2486__dram_pitch
            if (o_args->__pvcn_2486__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2486__dram_pitch_value, dagbin_base, 
                    392U);

#endif

#ifdef __PVCN_2365__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2365__dram_pitch
            if (o_args->__pvcn_2365__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2365__dram_pitch_value, dagbin_base, 
                    432U);

#endif

#ifdef __PVCN_2409__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2409__dram_pitch
            if (o_args->__pvcn_2409__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2409__dram_pitch_value, dagbin_base, 
                    468U);

#endif

#ifdef __PVCN_2398__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2398__dram_pitch
            if (o_args->__pvcn_2398__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2398__dram_pitch_value, dagbin_base, 
                    508U);

#endif

#ifdef __PVCN_2387__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2387__dram_pitch
            if (o_args->__pvcn_2387__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2387__dram_pitch_value, dagbin_base + 548U);

#endif

#ifdef __PVCN_2376__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2376__dram_pitch
            if (o_args->__pvcn_2376__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2376__dram_pitch_value, dagbin_base + 588U);

#endif

#ifdef __PVCN_2359__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2359__dram_pitch
            if (o_args->__pvcn_2359__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2359__dram_pitch_value, dagbin_base + 640U);

#endif

#ifdef __PVCN_2370__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2370__dram_pitch
            if (o_args->__pvcn_2370__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2370__dram_pitch_value, dagbin_base + 688U);

#endif

#ifdef __PVCN_2381__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2381__dram_pitch
            if (o_args->__pvcn_2381__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2381__dram_pitch_value, dagbin_base + 736U);

#endif

#ifdef __PVCN_2392__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2392__dram_pitch
            if (o_args->__pvcn_2392__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2392__dram_pitch_value, dagbin_base + 784U);

#endif

#ifdef __PVCN_2403__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2403__dram_pitch
            if (o_args->__pvcn_2403__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2403__dram_pitch_value, dagbin_base + 832U);

#endif

#ifdef __PVCN_2414__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2414__dram_pitch
            if (o_args->__pvcn_2414__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2414__dram_pitch_value, dagbin_base + 880U);

#endif

#ifdef __PVCN_2425__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2425__dram_pitch
            if (o_args->__pvcn_2425__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2425__dram_pitch_value, dagbin_base + 928U);

#endif

#ifdef __PVCN_2436__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2436__dram_pitch
            if (o_args->__pvcn_2436__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2436__dram_pitch_value, dagbin_base + 976U);

#endif

#ifdef __PVCN_2447__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2447__dram_pitch
            if (o_args->__pvcn_2447__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2447__dram_pitch_value, dagbin_base + 1024U);

#endif

#ifdef __PVCN_2458__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2458__dram_pitch
            if (o_args->__pvcn_2458__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2458__dram_pitch_value, dagbin_base + 1072U);

#endif

#ifdef __PVCN_2469__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2469__dram_pitch
            if (o_args->__pvcn_2469__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2469__dram_pitch_value, dagbin_base + 1120U);

#endif

#ifdef __PVCN_2480__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2480__dram_pitch
            if (o_args->__pvcn_2480__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2480__dram_pitch_value, dagbin_base + 1168U);

#endif

#ifdef __PVCN_2491__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2491__dram_pitch
            if (o_args->__pvcn_2491__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2491__dram_pitch_value, dagbin_base + 1216U);

#endif

#ifdef __PVCN_2502__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2502__dram_pitch
            if (o_args->__pvcn_2502__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2502__dram_pitch_value, dagbin_base + 1264U);

#endif

#ifdef __PVCN_2513__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2513__dram_pitch
            if (o_args->__pvcn_2513__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2513__dram_pitch_value, dagbin_base + 1312U);

#endif

            /*** dmald() calls ***/
            break;
        case 27:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: __pvcn_2359_
            coproc_poke_word(dag_base + 0,
                dagbin_base + 548U);

            // Poke HMB intermediate input buffer: __pvcn_2370_
            coproc_poke_word(dag_base + 18752,
                dagbin_base + 584U);

            // Poke HMB intermediate input buffer: __pvcn_2381_
            coproc_poke_word_offset(dag_base + 35328,
                dagbin_base, 308U);

            // Poke HMB intermediate input buffer: __pvcn_2392_
            coproc_poke_word_offset(dag_base + 53376,
                dagbin_base, 508U);

            // Poke HMB intermediate input buffer: __pvcn_2403_
            coproc_poke_word_offset(dag_base + 71424,
                dagbin_base, 428U);

            // Poke HMB intermediate input buffer: __pvcn_2414_
            coproc_poke_word_offset(dag_base + 88256,
                dagbin_base, 468U);

            // Poke HMB intermediate input buffer: __pvcn_2436_
            coproc_poke_word_offset(dag_base + 105536,
                dagbin_base, 388U);

            // Poke HMB intermediate input buffer: __pvcn_2425_
            coproc_poke_word_offset(dag_base + 189696,
                dagbin_base, 268U);

            // Poke HMB intermediate input buffer: __pvcn_2447_
            coproc_poke_word_offset(dag_base + 207232,
                dagbin_base, 228U);

            // Poke HMB intermediate input buffer: __pvcn_2458_
            coproc_poke_word_offset(dag_base + 225024,
                dagbin_base, 348U);

            // Poke HMB intermediate input buffer: __pvcn_2469_
            coproc_poke_word_offset(dag_base + 242304,
                dagbin_base, 148U);

            // Poke HMB intermediate input buffer: __pvcn_2480_
            coproc_poke_word_offset(dag_base + 260544,
                dagbin_base, 188U);

            // Poke HMB intermediate input buffer: __pvcn_2491_
            coproc_poke_word_offset(dag_base + 277632,
                dagbin_base, 28U);

            // Poke HMB intermediate input buffer: __pvcn_2502_
            coproc_poke_word_offset(dag_base + 295168,
                dagbin_base, 108U);

            // Poke HMB intermediate input buffer: __pvcn_2513_
            coproc_poke_word_offset(dag_base + 312000,
                dagbin_base, 68U);

            // Poke HMB intermediate output buffer: conv6__dw_____bn_conv6__dw__scale_mul___muli___85_____bn_conv6__dw__scale_mul_____scale___muli___360
            coproc_poke_word(dag_base + 313472,
                dagbin_base + 1144U);

            /* Pokable fields */
#ifdef __PVCN_2491__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2491__dram_pitch
            if (o_args->__pvcn_2491__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2491__dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef __PVCN_2513__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2513__dram_pitch
            if (o_args->__pvcn_2513__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2513__dram_pitch_value, dagbin_base, 
                    72U);

#endif

#ifdef __PVCN_2502__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2502__dram_pitch
            if (o_args->__pvcn_2502__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2502__dram_pitch_value, dagbin_base, 
                    112U);

#endif

#ifdef __PVCN_2469__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2469__dram_pitch
            if (o_args->__pvcn_2469__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2469__dram_pitch_value, dagbin_base, 
                    152U);

#endif

#ifdef __PVCN_2480__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2480__dram_pitch
            if (o_args->__pvcn_2480__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2480__dram_pitch_value, dagbin_base, 
                    192U);

#endif

#ifdef __PVCN_2447__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2447__dram_pitch
            if (o_args->__pvcn_2447__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2447__dram_pitch_value, dagbin_base, 
                    232U);

#endif

#ifdef __PVCN_2425__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2425__dram_pitch
            if (o_args->__pvcn_2425__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2425__dram_pitch_value, dagbin_base, 
                    272U);

#endif

#ifdef __PVCN_2381__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2381__dram_pitch
            if (o_args->__pvcn_2381__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2381__dram_pitch_value, dagbin_base, 
                    312U);

#endif

#ifdef __PVCN_2458__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2458__dram_pitch
            if (o_args->__pvcn_2458__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2458__dram_pitch_value, dagbin_base, 
                    352U);

#endif

#ifdef __PVCN_2436__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2436__dram_pitch
            if (o_args->__pvcn_2436__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2436__dram_pitch_value, dagbin_base, 
                    392U);

#endif

#ifdef __PVCN_2403__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2403__dram_pitch
            if (o_args->__pvcn_2403__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2403__dram_pitch_value, dagbin_base, 
                    432U);

#endif

#ifdef __PVCN_2414__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2414__dram_pitch
            if (o_args->__pvcn_2414__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2414__dram_pitch_value, dagbin_base, 
                    472U);

#endif

#ifdef __PVCN_2392__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2392__dram_pitch
            if (o_args->__pvcn_2392__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2392__dram_pitch_value, dagbin_base + 512U);

#endif

#ifdef __PVCN_2359__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2359__dram_pitch
            if (o_args->__pvcn_2359__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2359__dram_pitch_value, dagbin_base + 552U);

#endif

#ifdef __PVCN_2370__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2370__dram_pitch
            if (o_args->__pvcn_2370__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2370__dram_pitch_value, dagbin_base + 588U);

#endif

#ifdef CONV6__DW_____BN_CONV6__DW__SCALE_MUL___MULI___85_____BN_CONV6__DW__SCALE_MUL_____SCALE___MULI___360_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv6__dw_____bn_conv6__dw__scale_mul___muli___85_____bn_conv6__dw__scale_mul_____scale___muli___360_dram_pitch
            if (o_args->conv6__dw_____bn_conv6__dw__scale_mul___muli___85_____bn_conv6__dw__scale_mul_____scale___muli___360_dram_pitch_enable)
                coproc_poke_half(o_args->conv6__dw_____bn_conv6__dw__scale_mul___muli___85_____bn_conv6__dw__scale_mul_____scale___muli___360_dram_pitch_value, dagbin_base + 1148U);

#endif

            /*** dmald() calls ***/
            break;
        case 28:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: conv6__dw_____bn_conv6__dw__scale_mul___muli___85_____bn_conv6__dw__scale_mul_____scale___muli___360
            coproc_poke_word_offset(dag_base + 313472,
                dagbin_base, 28U);

            // Poke HMB intermediate output buffer: __pvcn_2556_
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 156U);

            /* Pokable fields */
#ifdef CONV6__DW_____BN_CONV6__DW__SCALE_MUL___MULI___85_____BN_CONV6__DW__SCALE_MUL_____SCALE___MULI___360_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv6__dw_____bn_conv6__dw__scale_mul___muli___85_____bn_conv6__dw__scale_mul_____scale___muli___360_dram_pitch
            if (o_args->conv6__dw_____bn_conv6__dw__scale_mul___muli___85_____bn_conv6__dw__scale_mul_____scale___muli___360_dram_pitch_enable)
                coproc_poke_half_offset(o_args->conv6__dw_____bn_conv6__dw__scale_mul___muli___85_____bn_conv6__dw__scale_mul_____scale___muli___360_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef __PVCN_2556__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2556__dram_pitch
            if (o_args->__pvcn_2556__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2556__dram_pitch_value, dagbin_base, 
                    160U);

#endif

            /*** dmald() calls ***/
            break;
        case 29:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: conv6__dw_____bn_conv6__dw__scale_mul___muli___85_____bn_conv6__dw__scale_mul_____scale___muli___360
            coproc_poke_word_offset(dag_base + 313472,
                dagbin_base, 28U);

            // Poke HMB intermediate output buffer: __pvcn_2564_
            coproc_poke_word_offset(dag_base + 40576,
                dagbin_base, 156U);

            /* Pokable fields */
#ifdef CONV6__DW_____BN_CONV6__DW__SCALE_MUL___MULI___85_____BN_CONV6__DW__SCALE_MUL_____SCALE___MULI___360_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv6__dw_____bn_conv6__dw__scale_mul___muli___85_____bn_conv6__dw__scale_mul_____scale___muli___360_dram_pitch
            if (o_args->conv6__dw_____bn_conv6__dw__scale_mul___muli___85_____bn_conv6__dw__scale_mul_____scale___muli___360_dram_pitch_enable)
                coproc_poke_half_offset(o_args->conv6__dw_____bn_conv6__dw__scale_mul___muli___85_____bn_conv6__dw__scale_mul_____scale___muli___360_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef __PVCN_2564__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2564__dram_pitch
            if (o_args->__pvcn_2564__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2564__dram_pitch_value, dagbin_base, 
                    160U);

#endif

            /*** dmald() calls ***/
            break;
        case 30:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: __pvcn_2556_
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 68U);

            // Poke HMB intermediate input buffer: __pvcn_2564_
            coproc_poke_word_offset(dag_base + 40576,
                dagbin_base, 28U);

            // Poke HMB intermediate input buffer: conv6__dw_____bn_conv6__dw__scale_mul___muli___85_____bn_conv6__dw__scale_mul_____scale___muli___360
            coproc_poke_word_offset(dag_base + 313472,
                dagbin_base, 108U);

            // Poke HMB intermediate output buffer: conv6__sep_____bn_conv6__sep__scale_mul___muli___88_____bn_conv6__sep__scale_mul_____scale___relu6__sep
            coproc_poke_word_offset(dag_base + 134848,
                dagbin_base, 220U);

            /* Pokable fields */
#ifdef __PVCN_2564__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2564__dram_pitch
            if (o_args->__pvcn_2564__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2564__dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef __PVCN_2556__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2556__dram_pitch
            if (o_args->__pvcn_2556__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2556__dram_pitch_value, dagbin_base, 
                    72U);

#endif

#ifdef CONV6__DW_____BN_CONV6__DW__SCALE_MUL___MULI___85_____BN_CONV6__DW__SCALE_MUL_____SCALE___MULI___360_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv6__dw_____bn_conv6__dw__scale_mul___muli___85_____bn_conv6__dw__scale_mul_____scale___muli___360_dram_pitch
            if (o_args->conv6__dw_____bn_conv6__dw__scale_mul___muli___85_____bn_conv6__dw__scale_mul_____scale___muli___360_dram_pitch_enable)
                coproc_poke_half_offset(o_args->conv6__dw_____bn_conv6__dw__scale_mul___muli___85_____bn_conv6__dw__scale_mul_____scale___muli___360_dram_pitch_value, dagbin_base, 
                    112U);

#endif

#ifdef CONV6__SEP_____BN_CONV6__SEP__SCALE_MUL___MULI___88_____BN_CONV6__SEP__SCALE_MUL_____SCALE___RELU6__SEP_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv6__sep_____bn_conv6__sep__scale_mul___muli___88_____bn_conv6__sep__scale_mul_____scale___relu6__sep_dram_pitch
            if (o_args->conv6__sep_____bn_conv6__sep__scale_mul___muli___88_____bn_conv6__sep__scale_mul_____scale___relu6__sep_dram_pitch_enable)
                coproc_poke_half_offset(o_args->conv6__sep_____bn_conv6__sep__scale_mul___muli___88_____bn_conv6__sep__scale_mul_____scale___relu6__sep_dram_pitch_value, dagbin_base, 
                    224U);

#endif

            /*** dmald() calls ***/
            break;
        case 31:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: conv6__sep_____bn_conv6__sep__scale_mul___muli___88_____bn_conv6__sep__scale_mul_____scale___relu6__sep
            coproc_poke_word_offset(dag_base + 134848,
                dagbin_base, 104U);

            // Poke HMB intermediate output buffer: conv6_mbox_conf___conv2i___90______7_1_____bn_conv7_1__scale_mul___muli___93_____bn_conv7_1__scale_mul_____scale___loc___conv2i___138______muli___367___8___9
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 156U);

            /* Pokable fields */
#ifdef CONV6__SEP_____BN_CONV6__SEP__SCALE_MUL___MULI___88_____BN_CONV6__SEP__SCALE_MUL_____SCALE___RELU6__SEP_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv6__sep_____bn_conv6__sep__scale_mul___muli___88_____bn_conv6__sep__scale_mul_____scale___relu6__sep_dram_pitch
            if (o_args->conv6__sep_____bn_conv6__sep__scale_mul___muli___88_____bn_conv6__sep__scale_mul_____scale___relu6__sep_dram_pitch_enable)
                coproc_poke_half_offset(o_args->conv6__sep_____bn_conv6__sep__scale_mul___muli___88_____bn_conv6__sep__scale_mul_____scale___relu6__sep_dram_pitch_value, dagbin_base, 
                    108U);

#endif

#ifdef CONV6_MBOX_CONF___CONV2I___90______7_1_____BN_CONV7_1__SCALE_MUL___MULI___93_____BN_CONV7_1__SCALE_MUL_____SCALE___LOC___CONV2I___138______MULI___367___8___9_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv6_mbox_conf___conv2i___90______7_1_____bn_conv7_1__scale_mul___muli___93_____bn_conv7_1__scale_mul_____scale___loc___conv2i___138______muli___367___8___9_dram_pitch
            if (o_args->conv6_mbox_conf___conv2i___90______7_1_____bn_conv7_1__scale_mul___muli___93_____bn_conv7_1__scale_mul_____scale___loc___conv2i___138______muli___367___8___9_dram_pitch_enable)
                coproc_poke_half_offset(o_args->conv6_mbox_conf___conv2i___90______7_1_____bn_conv7_1__scale_mul___muli___93_____bn_conv7_1__scale_mul_____scale___loc___conv2i___138______muli___367___8___9_dram_pitch_value, dagbin_base, 
                    160U);

#endif

            /*** dmald() calls ***/
            break;
        case 32:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: conv6_mbox_conf___conv2i___90______7_1_____bn_conv7_1__scale_mul___muli___93_____bn_conv7_1__scale_mul_____scale___loc___conv2i___138______muli___367___8___9
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 28U);

            // Poke HMB intermediate output buffer: conv6_mbox_loc_flat
            coproc_poke_word_offset(dag_base + 38656,
                dagbin_base, 220U);

            // Poke HMB intermediate output buffer: relu7_1
            coproc_poke_word_offset(dag_base + 41088,
                dagbin_base, 304U);

            // Poke HMB intermediate output buffer: __pvcn_2592_
            coproc_poke_word_offset(dag_base + 71808,
                dagbin_base, 364U);

            /* Pokable fields */
#ifdef CONV6_MBOX_CONF___CONV2I___90______7_1_____BN_CONV7_1__SCALE_MUL___MULI___93_____BN_CONV7_1__SCALE_MUL_____SCALE___LOC___CONV2I___138______MULI___367___8___9_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv6_mbox_conf___conv2i___90______7_1_____bn_conv7_1__scale_mul___muli___93_____bn_conv7_1__scale_mul_____scale___loc___conv2i___138______muli___367___8___9_dram_pitch
            if (o_args->conv6_mbox_conf___conv2i___90______7_1_____bn_conv7_1__scale_mul___muli___93_____bn_conv7_1__scale_mul_____scale___loc___conv2i___138______muli___367___8___9_dram_pitch_enable)
                coproc_poke_half_offset(o_args->conv6_mbox_conf___conv2i___90______7_1_____bn_conv7_1__scale_mul___muli___93_____bn_conv7_1__scale_mul_____scale___loc___conv2i___138______muli___367___8___9_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef CONV6_MBOX_LOC_FLAT_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv6_mbox_loc_flat_dram_pitch
            if (o_args->conv6_mbox_loc_flat_dram_pitch_enable)
                coproc_poke_half_offset(o_args->conv6_mbox_loc_flat_dram_pitch_value, dagbin_base, 
                    224U);

#endif

#ifdef RELU7_1_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: relu7_1_dram_pitch
            if (o_args->relu7_1_dram_pitch_enable)
                coproc_poke_half_offset(o_args->relu7_1_dram_pitch_value, dagbin_base, 
                    308U);

#endif

#ifdef __PVCN_2592__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2592__dram_pitch
            if (o_args->__pvcn_2592__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2592__dram_pitch_value, dagbin_base, 
                    368U);

#endif

            /*** dmald() calls ***/
            break;
        case 33:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: relu7_1
            coproc_poke_word_offset(dag_base + 41088,
                dagbin_base, 28U);

            // Poke HMB intermediate output buffer: __pvcn_2600_
            coproc_poke_word_offset(dag_base + 76928,
                dagbin_base, 164U);

            /* Pokable fields */
#ifdef RELU7_1_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: relu7_1_dram_pitch
            if (o_args->relu7_1_dram_pitch_enable)
                coproc_poke_half_offset(o_args->relu7_1_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef __PVCN_2600__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2600__dram_pitch
            if (o_args->__pvcn_2600__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2600__dram_pitch_value, dagbin_base, 
                    168U);

#endif

            /*** dmald() calls ***/
            break;
        case 34:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: relu7_1
            coproc_poke_word_offset(dag_base + 41088,
                dagbin_base, 28U);

            // Poke HMB intermediate output buffer: __pvcn_2608_
            coproc_poke_word_offset(dag_base + 82048,
                dagbin_base, 164U);

            /* Pokable fields */
#ifdef RELU7_1_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: relu7_1_dram_pitch
            if (o_args->relu7_1_dram_pitch_enable)
                coproc_poke_half_offset(o_args->relu7_1_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef __PVCN_2608__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2608__dram_pitch
            if (o_args->__pvcn_2608__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2608__dram_pitch_value, dagbin_base, 
                    168U);

#endif

            /*** dmald() calls ***/
            break;
        case 35:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: conv6_mbox_conf___conv2i___90______7_1_____bn_conv7_1__scale_mul___muli___93_____bn_conv7_1__scale_mul_____scale___loc___conv2i___138______muli___367___8___9
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 292U);

            // Poke HMB intermediate input buffer: conv6_mbox_loc_flat
            coproc_poke_word_offset(dag_base + 38656,
                dagbin_base, 100U);

            // Poke HMB intermediate input buffer: relu7_1
            coproc_poke_word_offset(dag_base + 41088,
                dagbin_base, 140U);

            // Poke HMB intermediate input buffer: __pvcn_2592_
            coproc_poke_word_offset(dag_base + 71808,
                dagbin_base, 180U);

            // Poke HMB intermediate input buffer: __pvcn_2600_
            coproc_poke_word_offset(dag_base + 76928,
                dagbin_base, 64U);

            // Poke HMB intermediate input buffer: __pvcn_2608_
            coproc_poke_word_offset(dag_base + 82048,
                dagbin_base, 28U);

            // Poke HMB intermediate input buffer: conv5_5_mbox_loc_flat
            coproc_poke_word_offset(dag_base + 122880,
                dagbin_base, 252U);

            // Poke HMB intermediate input buffer: conv5_5_mbox_conf_flat
            coproc_poke_word_offset(dag_base + 127232,
                dagbin_base, 216U);

            // Poke HMB intermediate output buffer: conv7_2_____bn_conv7_2__scale_mul___muli___96_____bn_conv7_2__scale_mul_____scale___relu7_2
            coproc_poke_word_offset(dag_base + 87168,
                dagbin_base, 412U);

            // Poke HMB intermediate output buffer: __pvcn_2663_
            coproc_poke_word(dag_base + 107648,
                dagbin_base + 548U);

            // Poke HMB intermediate output buffer: __pvcn_2683_
            coproc_poke_word(dag_base + 134848,
                dagbin_base + 680U);

            /* Pokable fields */
#ifdef __PVCN_2608__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2608__dram_pitch
            if (o_args->__pvcn_2608__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2608__dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef __PVCN_2600__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2600__dram_pitch
            if (o_args->__pvcn_2600__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2600__dram_pitch_value, dagbin_base, 
                    68U);

#endif

#ifdef CONV6_MBOX_LOC_FLAT_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv6_mbox_loc_flat_dram_pitch
            if (o_args->conv6_mbox_loc_flat_dram_pitch_enable)
                coproc_poke_half_offset(o_args->conv6_mbox_loc_flat_dram_pitch_value, dagbin_base, 
                    104U);

#endif

#ifdef RELU7_1_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: relu7_1_dram_pitch
            if (o_args->relu7_1_dram_pitch_enable)
                coproc_poke_half_offset(o_args->relu7_1_dram_pitch_value, dagbin_base, 
                    144U);

#endif

#ifdef __PVCN_2592__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2592__dram_pitch
            if (o_args->__pvcn_2592__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2592__dram_pitch_value, dagbin_base, 
                    184U);

#endif

#ifdef CONV5_5_MBOX_CONF_FLAT_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv5_5_mbox_conf_flat_dram_pitch
            if (o_args->conv5_5_mbox_conf_flat_dram_pitch_enable)
                coproc_poke_half_offset(o_args->conv5_5_mbox_conf_flat_dram_pitch_value, dagbin_base, 
                    220U);

#endif

#ifdef CONV5_5_MBOX_LOC_FLAT_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv5_5_mbox_loc_flat_dram_pitch
            if (o_args->conv5_5_mbox_loc_flat_dram_pitch_enable)
                coproc_poke_half_offset(o_args->conv5_5_mbox_loc_flat_dram_pitch_value, dagbin_base, 
                    256U);

#endif

#ifdef CONV6_MBOX_CONF___CONV2I___90______7_1_____BN_CONV7_1__SCALE_MUL___MULI___93_____BN_CONV7_1__SCALE_MUL_____SCALE___LOC___CONV2I___138______MULI___367___8___9_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv6_mbox_conf___conv2i___90______7_1_____bn_conv7_1__scale_mul___muli___93_____bn_conv7_1__scale_mul_____scale___loc___conv2i___138______muli___367___8___9_dram_pitch
            if (o_args->conv6_mbox_conf___conv2i___90______7_1_____bn_conv7_1__scale_mul___muli___93_____bn_conv7_1__scale_mul_____scale___loc___conv2i___138______muli___367___8___9_dram_pitch_enable)
                coproc_poke_half_offset(o_args->conv6_mbox_conf___conv2i___90______7_1_____bn_conv7_1__scale_mul___muli___93_____bn_conv7_1__scale_mul_____scale___loc___conv2i___138______muli___367___8___9_dram_pitch_value, dagbin_base, 
                    296U);

#endif

#ifdef CONV7_2_____BN_CONV7_2__SCALE_MUL___MULI___96_____BN_CONV7_2__SCALE_MUL_____SCALE___RELU7_2_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv7_2_____bn_conv7_2__scale_mul___muli___96_____bn_conv7_2__scale_mul_____scale___relu7_2_dram_pitch
            if (o_args->conv7_2_____bn_conv7_2__scale_mul___muli___96_____bn_conv7_2__scale_mul_____scale___relu7_2_dram_pitch_enable)
                coproc_poke_half_offset(o_args->conv7_2_____bn_conv7_2__scale_mul___muli___96_____bn_conv7_2__scale_mul_____scale___relu7_2_dram_pitch_value, dagbin_base, 
                    416U);

#endif

#ifdef __PVCN_2663__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2663__dram_pitch
            if (o_args->__pvcn_2663__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2663__dram_pitch_value, dagbin_base + 552U);

#endif

#ifdef __PVCN_2683__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2683__dram_pitch
            if (o_args->__pvcn_2683__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2683__dram_pitch_value, dagbin_base + 684U);

#endif

            /*** dmald() calls ***/
            break;
        case 36:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: conv7_2_____bn_conv7_2__scale_mul___muli___96_____bn_conv7_2__scale_mul_____scale___relu7_2
            coproc_poke_word_offset(dag_base + 87168,
                dagbin_base, 196U);

            // Poke HMB intermediate output buffer: conv8_2_____bn_conv8_2__scale_mul___muli___104_____bn_conv8_2__scale_mul_____scale___relu8_2
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 376U);

            // Poke HMB intermediate output buffer: conv7_2_mbox_loc_flat
            coproc_poke_word(dag_base + 3072,
                dagbin_base + 528U);

            // Poke HMB intermediate output buffer: conv7_2_mbox_conf_flat
            coproc_poke_word(dag_base + 3712,
                dagbin_base + 636U);

            /* Pokable fields */
#ifdef CONV7_2_____BN_CONV7_2__SCALE_MUL___MULI___96_____BN_CONV7_2__SCALE_MUL_____SCALE___RELU7_2_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv7_2_____bn_conv7_2__scale_mul___muli___96_____bn_conv7_2__scale_mul_____scale___relu7_2_dram_pitch
            if (o_args->conv7_2_____bn_conv7_2__scale_mul___muli___96_____bn_conv7_2__scale_mul_____scale___relu7_2_dram_pitch_enable)
                coproc_poke_half_offset(o_args->conv7_2_____bn_conv7_2__scale_mul___muli___96_____bn_conv7_2__scale_mul_____scale___relu7_2_dram_pitch_value, dagbin_base, 
                    200U);

#endif

#ifdef CONV8_2_____BN_CONV8_2__SCALE_MUL___MULI___104_____BN_CONV8_2__SCALE_MUL_____SCALE___RELU8_2_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv8_2_____bn_conv8_2__scale_mul___muli___104_____bn_conv8_2__scale_mul_____scale___relu8_2_dram_pitch
            if (o_args->conv8_2_____bn_conv8_2__scale_mul___muli___104_____bn_conv8_2__scale_mul_____scale___relu8_2_dram_pitch_enable)
                coproc_poke_half_offset(o_args->conv8_2_____bn_conv8_2__scale_mul___muli___104_____bn_conv8_2__scale_mul_____scale___relu8_2_dram_pitch_value, dagbin_base, 
                    380U);

#endif

#ifdef CONV7_2_MBOX_LOC_FLAT_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv7_2_mbox_loc_flat_dram_pitch
            if (o_args->conv7_2_mbox_loc_flat_dram_pitch_enable)
                coproc_poke_half(o_args->conv7_2_mbox_loc_flat_dram_pitch_value, dagbin_base + 532U);

#endif

#ifdef CONV7_2_MBOX_CONF_FLAT_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv7_2_mbox_conf_flat_dram_pitch
            if (o_args->conv7_2_mbox_conf_flat_dram_pitch_enable)
                coproc_poke_half(o_args->conv7_2_mbox_conf_flat_dram_pitch_value, dagbin_base + 640U);

#endif

            /*** dmald() calls ***/
            break;
        case 37:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: conv8_2_____bn_conv8_2__scale_mul___muli___104_____bn_conv8_2__scale_mul_____scale___relu8_2
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 28U);

            // Poke HMB intermediate output buffer: conv9_2_____bn_conv9_2__scale_mul___muli___112_____bn_conv9_2__scale_mul_____scale___relu9_2
            coproc_poke_word_offset(dag_base + 4800,
                dagbin_base, 372U);

            // Poke HMB intermediate output buffer: conv8_2_mbox_loc_flat
            coproc_poke_word(dag_base + 6848,
                dagbin_base + 520U);

            // Poke HMB intermediate output buffer: conv8_2_mbox_conf_flat
            coproc_poke_word(dag_base + 7104,
                dagbin_base + 624U);

            /* Pokable fields */
#ifdef CONV8_2_____BN_CONV8_2__SCALE_MUL___MULI___104_____BN_CONV8_2__SCALE_MUL_____SCALE___RELU8_2_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv8_2_____bn_conv8_2__scale_mul___muli___104_____bn_conv8_2__scale_mul_____scale___relu8_2_dram_pitch
            if (o_args->conv8_2_____bn_conv8_2__scale_mul___muli___104_____bn_conv8_2__scale_mul_____scale___relu8_2_dram_pitch_enable)
                coproc_poke_half_offset(o_args->conv8_2_____bn_conv8_2__scale_mul___muli___104_____bn_conv8_2__scale_mul_____scale___relu8_2_dram_pitch_value, dagbin_base, 
                    32U);

#endif

#ifdef CONV9_2_____BN_CONV9_2__SCALE_MUL___MULI___112_____BN_CONV9_2__SCALE_MUL_____SCALE___RELU9_2_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv9_2_____bn_conv9_2__scale_mul___muli___112_____bn_conv9_2__scale_mul_____scale___relu9_2_dram_pitch
            if (o_args->conv9_2_____bn_conv9_2__scale_mul___muli___112_____bn_conv9_2__scale_mul_____scale___relu9_2_dram_pitch_enable)
                coproc_poke_half_offset(o_args->conv9_2_____bn_conv9_2__scale_mul___muli___112_____bn_conv9_2__scale_mul_____scale___relu9_2_dram_pitch_value, dagbin_base, 
                    376U);

#endif

#ifdef CONV8_2_MBOX_LOC_FLAT_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv8_2_mbox_loc_flat_dram_pitch
            if (o_args->conv8_2_mbox_loc_flat_dram_pitch_enable)
                coproc_poke_half(o_args->conv8_2_mbox_loc_flat_dram_pitch_value, dagbin_base + 524U);

#endif

#ifdef CONV8_2_MBOX_CONF_FLAT_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv8_2_mbox_conf_flat_dram_pitch
            if (o_args->conv8_2_mbox_conf_flat_dram_pitch_enable)
                coproc_poke_half(o_args->conv8_2_mbox_conf_flat_dram_pitch_value, dagbin_base + 628U);

#endif

            /*** dmald() calls ***/
            break;
        case 38:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: conv7_2_mbox_loc_flat
            coproc_poke_word_offset(dag_base + 3072,
                dagbin_base, 60U);

            // Poke HMB intermediate input buffer: conv7_2_mbox_conf_flat
            coproc_poke_word_offset(dag_base + 3712,
                dagbin_base, 96U);

            // Poke HMB intermediate input buffer: conv9_2_____bn_conv9_2__scale_mul___muli___112_____bn_conv9_2__scale_mul_____scale___relu9_2
            coproc_poke_word_offset(dag_base + 4800,
                dagbin_base, 208U);

            // Poke HMB intermediate input buffer: conv8_2_mbox_loc_flat
            coproc_poke_word_offset(dag_base + 6848,
                dagbin_base, 24U);

            // Poke HMB intermediate input buffer: conv8_2_mbox_conf_flat
            coproc_poke_word_offset(dag_base + 7104,
                dagbin_base, 240U);

            // Poke HMB intermediate input buffer: __pvcn_2663_
            coproc_poke_word_offset(dag_base + 107648,
                dagbin_base, 168U);

            // Poke HMB intermediate input buffer: __pvcn_2683_
            coproc_poke_word_offset(dag_base + 134848,
                dagbin_base, 132U);

            // Poke HMB intermediate output buffer: __pvcn_263_
            coproc_poke_word(dag_base + 0,
                dagbin_base + 636U);

            // Poke HMB intermediate output buffer: conv9_2_mbox_loc_flat
            coproc_poke_word(dag_base + 128,
                dagbin_base + 784U);

            // Poke HMB intermediate output buffer: conv9_2_mbox_conf_flat
            coproc_poke_word(dag_base + 256,
                dagbin_base + 960U);

            // Poke HMB intermediate output buffer: __pvcn_270_
            coproc_poke_word(dag_base + 448,
                dagbin_base + 1004U);

            // Poke HMB intermediate output buffer: __pvcn_2666_
            coproc_poke_word(dag_base + 7488,
                dagbin_base + 856U);

            // Poke HMB intermediate output buffer: __pvcn_2686_
            coproc_poke_word(dag_base + 15040,
                dagbin_base + 1076U);

            /* Pokable fields */
#ifdef CONV8_2_MBOX_LOC_FLAT_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv8_2_mbox_loc_flat_dram_pitch
            if (o_args->conv8_2_mbox_loc_flat_dram_pitch_enable)
                coproc_poke_half_offset(o_args->conv8_2_mbox_loc_flat_dram_pitch_value, dagbin_base, 
                    28U);

#endif

#ifdef CONV7_2_MBOX_LOC_FLAT_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv7_2_mbox_loc_flat_dram_pitch
            if (o_args->conv7_2_mbox_loc_flat_dram_pitch_enable)
                coproc_poke_half_offset(o_args->conv7_2_mbox_loc_flat_dram_pitch_value, dagbin_base, 
                    64U);

#endif

#ifdef CONV7_2_MBOX_CONF_FLAT_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv7_2_mbox_conf_flat_dram_pitch
            if (o_args->conv7_2_mbox_conf_flat_dram_pitch_enable)
                coproc_poke_half_offset(o_args->conv7_2_mbox_conf_flat_dram_pitch_value, dagbin_base, 
                    100U);

#endif

#ifdef __PVCN_2683__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2683__dram_pitch
            if (o_args->__pvcn_2683__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2683__dram_pitch_value, dagbin_base, 
                    136U);

#endif

#ifdef __PVCN_2663__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2663__dram_pitch
            if (o_args->__pvcn_2663__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2663__dram_pitch_value, dagbin_base, 
                    172U);

#endif

#ifdef CONV9_2_____BN_CONV9_2__SCALE_MUL___MULI___112_____BN_CONV9_2__SCALE_MUL_____SCALE___RELU9_2_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv9_2_____bn_conv9_2__scale_mul___muli___112_____bn_conv9_2__scale_mul_____scale___relu9_2_dram_pitch
            if (o_args->conv9_2_____bn_conv9_2__scale_mul___muli___112_____bn_conv9_2__scale_mul_____scale___relu9_2_dram_pitch_enable)
                coproc_poke_half_offset(o_args->conv9_2_____bn_conv9_2__scale_mul___muli___112_____bn_conv9_2__scale_mul_____scale___relu9_2_dram_pitch_value, dagbin_base, 
                    212U);

#endif

#ifdef CONV8_2_MBOX_CONF_FLAT_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv8_2_mbox_conf_flat_dram_pitch
            if (o_args->conv8_2_mbox_conf_flat_dram_pitch_enable)
                coproc_poke_half_offset(o_args->conv8_2_mbox_conf_flat_dram_pitch_value, dagbin_base, 
                    244U);

#endif

#ifdef __PVCN_263__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_263__dram_pitch
            if (o_args->__pvcn_263__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_263__dram_pitch_value, dagbin_base + 640U);

#endif

#ifdef CONV9_2_MBOX_LOC_FLAT_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv9_2_mbox_loc_flat_dram_pitch
            if (o_args->conv9_2_mbox_loc_flat_dram_pitch_enable)
                coproc_poke_half(o_args->conv9_2_mbox_loc_flat_dram_pitch_value, dagbin_base + 788U);

#endif

#ifdef __PVCN_2666__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2666__dram_pitch
            if (o_args->__pvcn_2666__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2666__dram_pitch_value, dagbin_base + 860U);

#endif

#ifdef CONV9_2_MBOX_CONF_FLAT_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv9_2_mbox_conf_flat_dram_pitch
            if (o_args->conv9_2_mbox_conf_flat_dram_pitch_enable)
                coproc_poke_half(o_args->conv9_2_mbox_conf_flat_dram_pitch_value, dagbin_base + 964U);

#endif

#ifdef __PVCN_270__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_270__dram_pitch
            if (o_args->__pvcn_270__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_270__dram_pitch_value, dagbin_base + 1008U);

#endif

#ifdef __PVCN_2686__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2686__dram_pitch
            if (o_args->__pvcn_2686__dram_pitch_enable)
                coproc_poke_half(o_args->__pvcn_2686__dram_pitch_value, dagbin_base + 1080U);

#endif

            /*** dmald() calls ***/
            break;
        case 39:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: __pvcn_263_
            coproc_poke_word_offset(dag_base + 0,
                dagbin_base, 96U);

            // Poke HMB intermediate input buffer: conv9_2_mbox_loc_flat
            coproc_poke_word_offset(dag_base + 128,
                dagbin_base, 164U);

            // Poke HMB intermediate input buffer: conv9_2_mbox_conf_flat
            coproc_poke_word_offset(dag_base + 256,
                dagbin_base, 128U);

            // Poke HMB intermediate input buffer: __pvcn_270_
            coproc_poke_word_offset(dag_base + 448,
                dagbin_base, 24U);

            // Poke HMB intermediate input buffer: __pvcn_2666_
            coproc_poke_word_offset(dag_base + 7488,
                dagbin_base, 60U);

            // Poke HMB intermediate input buffer: __pvcn_2686_
            coproc_poke_word_offset(dag_base + 15040,
                dagbin_base, 200U);

            // Poke HMB intermediate output buffer: mbox_conf_flatten_
            coproc_poke_word(dag_base + 28288,
                dagbin_base + 940U);

            /* Primary outputs */
            // Poke HMB output buffer: mbox_loc
            coproc_poke_word(r_args->mbox_loc_addr + 0,
                dagbin_base + 520U);

            /* Pokable fields */
#ifdef __PVCN_270__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_270__dram_pitch
            if (o_args->__pvcn_270__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_270__dram_pitch_value, dagbin_base, 
                    28U);

#endif

#ifdef __PVCN_2666__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2666__dram_pitch
            if (o_args->__pvcn_2666__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2666__dram_pitch_value, dagbin_base, 
                    64U);

#endif

#ifdef __PVCN_263__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_263__dram_pitch
            if (o_args->__pvcn_263__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_263__dram_pitch_value, dagbin_base, 
                    100U);

#endif

#ifdef CONV9_2_MBOX_CONF_FLAT_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv9_2_mbox_conf_flat_dram_pitch
            if (o_args->conv9_2_mbox_conf_flat_dram_pitch_enable)
                coproc_poke_half_offset(o_args->conv9_2_mbox_conf_flat_dram_pitch_value, dagbin_base, 
                    132U);

#endif

#ifdef CONV9_2_MBOX_LOC_FLAT_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: conv9_2_mbox_loc_flat_dram_pitch
            if (o_args->conv9_2_mbox_loc_flat_dram_pitch_enable)
                coproc_poke_half_offset(o_args->conv9_2_mbox_loc_flat_dram_pitch_value, dagbin_base, 
                    168U);

#endif

#ifdef __PVCN_2686__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: __pvcn_2686__dram_pitch
            if (o_args->__pvcn_2686__dram_pitch_enable)
                coproc_poke_half_offset(o_args->__pvcn_2686__dram_pitch_value, dagbin_base, 
                    204U);

#endif

#ifdef MBOX_LOC_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: mbox_loc_dram_pitch
            if (o_args->mbox_loc_dram_pitch_enable)
                coproc_poke_half(o_args->mbox_loc_dram_pitch_value, dagbin_base + 524U);

#endif

#ifdef MBOX_CONF_FLATTEN__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: mbox_conf_flatten__dram_pitch
            if (o_args->mbox_conf_flatten__dram_pitch_enable)
                coproc_poke_half(o_args->mbox_conf_flatten__dram_pitch_value, dagbin_base + 944U);

#endif

            /*** dmald() calls ***/
            break;
        case 40:
            /*** poke() calls ***/
            /* Intermediate buffers */
            // Poke HMB intermediate input buffer: mbox_conf_flatten_
            coproc_poke_word_offset(dag_base + 28288,
                dagbin_base, 24U);

            /* Primary outputs */
            // Poke HMB output buffer: mbox_conf_flatten
            coproc_poke_word_offset(r_args->mbox_conf_flatten_addr + 0,
                dagbin_base, 80U);

            /* Pokable fields */
#ifdef MBOX_CONF_FLATTEN__DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: mbox_conf_flatten__dram_pitch
            if (o_args->mbox_conf_flatten__dram_pitch_enable)
                coproc_poke_half_offset(o_args->mbox_conf_flatten__dram_pitch_value, dagbin_base, 
                    28U);

#endif

#ifdef MBOX_CONF_FLATTEN_DRAM_PITCH_MODIFIABLE
            // Poke 16-bit variable: mbox_conf_flatten_dram_pitch
            if (o_args->mbox_conf_flatten_dram_pitch_enable)
                coproc_poke_half_offset(o_args->mbox_conf_flatten_dram_pitch_value, dagbin_base, 
                    84U);

#endif

            /*** dmald() calls ***/
            break;
        default:
            break;
    }

    return;
} // end of mnet_ssd_adas_flex_pic_ag_poke_split()

/* Prompts VP to execute code */
inline void mnet_ssd_adas_flex_pic_ag_run_split(
    uint32_t split_id
) {
    coproc_run(vmem_dagbin_bases[split_id]);
    return;
} // end of mnet_ssd_adas_flex_pic_ag_run_split()

/* Handle SMB buffers of a particular split DAG */
inline void mnet_ssd_adas_flex_pic_ag_peek_split(
    mnet_ssd_adas_flex_pic_ag_required_fields_t *r_args,
    struct mnet_ssd_adas_flex_pic_ag_split_context *split_ctxt,
    uint32_t split_id
) {
    uint32_t dagbin_base = vmem_dagbin_bases[split_id];
    (void) dagbin_base; // avoid unused variable warning

    switch (split_id) {
        case 0:
            /*** dmast() calls ***/
            break;
        case 1:
            /*** dmast() calls ***/
            break;
        case 2:
            /*** dmast() calls ***/
            break;
        case 3:
            /*** dmast() calls ***/
            break;
        case 4:
            /*** dmast() calls ***/
            break;
        case 5:
            /*** dmast() calls ***/
            break;
        case 6:
            /*** dmast() calls ***/
            break;
        case 7:
            /*** dmast() calls ***/
            break;
        case 8:
            /*** dmast() calls ***/
            break;
        case 9:
            /*** dmast() calls ***/
            break;
        case 10:
            /*** dmast() calls ***/
            break;
        case 11:
            /*** dmast() calls ***/
            break;
        case 12:
            /*** dmast() calls ***/
            break;
        case 13:
            /*** dmast() calls ***/
            break;
        case 14:
            /*** dmast() calls ***/
            break;
        case 15:
            /*** dmast() calls ***/
            break;
        case 16:
            /*** dmast() calls ***/
            break;
        case 17:
            /*** dmast() calls ***/
            break;
        case 18:
            /*** dmast() calls ***/
            break;
        case 19:
            /*** dmast() calls ***/
            break;
        case 20:
            /*** dmast() calls ***/
            break;
        case 21:
            /*** dmast() calls ***/
            break;
        case 22:
            /*** dmast() calls ***/
            break;
        case 23:
            /*** dmast() calls ***/
            break;
        case 24:
            /*** dmast() calls ***/
            break;
        case 25:
            /*** dmast() calls ***/
            break;
        case 26:
            /*** dmast() calls ***/
            break;
        case 27:
            /*** dmast() calls ***/
            break;
        case 28:
            /*** dmast() calls ***/
            break;
        case 29:
            /*** dmast() calls ***/
            break;
        case 30:
            /*** dmast() calls ***/
            break;
        case 31:
            /*** dmast() calls ***/
            break;
        case 32:
            /*** dmast() calls ***/
            break;
        case 33:
            /*** dmast() calls ***/
            break;
        case 34:
            /*** dmast() calls ***/
            break;
        case 35:
            /*** dmast() calls ***/
            break;
        case 36:
            /*** dmast() calls ***/
            break;
        case 37:
            /*** dmast() calls ***/
            break;
        case 38:
            /*** dmast() calls ***/
            break;
        case 39:
            /*** dmast() calls ***/
            break;
        case 40:
            /*** dmast() calls ***/
            break;
        default:
            break;
    }

    coproc_peek_word(0U, 0x4U); // 0x4 is "status_register" in VMEM
    return;
} // end of mnet_ssd_adas_flex_pic_ag_peek_split()

/****************** USED BY MNET_SSD_ADAS_FLEX_PIC_AG_PEEK_SPLIT() *******************/
/* Check the health of VP and handle SMB buffers of a particular split DAG */
inline errcode_enum_t mnet_ssd_adas_flex_pic_ag_vp_status()
{
    // Read status register
    uint32_t status_register = get_peek_value_imm(0U);
#ifdef CVTASK_PRINT
    cvtask_printf(LVL_DEBUG, "   > VP status register is %X.",
        status_register, 0, 0, 0, 0);
#endif

    // Single out relevant bits
    uint32_t parse_err_flag       = (status_register & 0x80000000U) >> 31;
    uint32_t runtime_error        = (status_register & 0x40000000U) >> 30;
    uint32_t nan_flag             = (status_register & 0x20000000U) >> 29;
    uint32_t overflow_flag        = (status_register & 0x10000000U) >> 28;
#ifdef CVTASK_PRINT
    uint32_t first_op_runtime_id  = (status_register &   0xFF0000U) >> 16;
    uint32_t first_op_nan_id      = (status_register &     0xFF00U) >>  8;
    uint32_t first_op_overflow_id =  status_register &       0xFFU       ;
#endif
    uint32_t parse_error_code     = (status_register &     0xFF00U) >>  8;
    uint32_t parse_id             =  status_register &       0xFFU       ;

    errcode_enum_t status = ERRCODE_NONE;
    if (parse_err_flag) {
        cvtask_printf(LVL_CRITICAL, "  > Error: error #%d encountered at "
            "operator %d.", parse_error_code, parse_id, 0, 0, 0);
        return ERRCODE_GENERIC;
    }
    if (nan_flag) {
#ifdef CVTASK_PRINT
        cvtask_printf(LVL_CRITICAL, "  > Warning: NaN error encountered at "
            "operator %d.", first_op_nan_id, 0, 0, 0, 0);
#endif
        status |= ERRCODE_NONE;
    }
    if (overflow_flag) {
#ifdef CVTASK_PRINT
        cvtask_printf(LVL_CRITICAL, "  > Warning: overflow error encountered "
            "at operator %d.", first_op_overflow_id, 0, 0, 0, 0);
#endif
        status |= ERRCODE_NONE;
    }
    if (runtime_error) {
#ifdef CVTASK_PRINT
        cvtask_printf(LVL_CRITICAL, "  > Warning: runtime error encountered "
            "at operator %d.", first_op_runtime_id, 0, 0, 0, 0);
#endif
        status |= ERRCODE_NONE;
    }

    return status;
} // end of mnet_ssd_adas_flex_pic_ag_vp_status()

