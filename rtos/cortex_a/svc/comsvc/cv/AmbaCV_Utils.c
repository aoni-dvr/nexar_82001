/**
 *  @file AmbaCV_Utils.c
 *
 *  Copyright (c) 2020 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  This file includes sample code and is only for internal testing and evaluation.  If you
 *  distribute this sample code (whether in source, object, or binary code form), it will be
 *  without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details AmbaCV Shell Command APIs
 *
 */

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"
#include "AmbaKAL.h"
#include "AmbaFS.h"
#include "AmbaShell.h"
#include "AmbaUtility.h"
#include "AmbaPrint.h"
#include "AmbaCache.h"
#include "AmbaNAND.h"
#include "AmbaSYS.h"
#include "schdr_api.h"
#include "cvsched_api.h"
#include "cvapi_ambacv_flexidag.h"
#include "AmbaCV_Init.h"

extern void* ambacv_c2v(uint64_t ca);
extern uint32_t ambacv_cache_invalidate(void *ptr, uint64_t size);

#define NUM_VISPERF_DUMP    1U
#define VIS_PERF_COPY_SIZE  0x50000U

#define NUM_FLEXIDAG_DUMP   64U
#define FLEXIDAG_INFO_SIZE  0x800U

static uint8_t  *pVISPerfCopy[SYSTEM_MAX_NUM_VISORC];
static uint32_t visperf_size[SYSTEM_MAX_NUM_VISORC];
static uint8_t  *pFlexidag_Info[FLEXIDAG_MAX_SLOTS];
static uint32_t flexidag_info_size[FLEXIDAG_MAX_SLOTS];
static flexidag_trace_t *pFlexidagTrace;

static void PrintInt(AMBA_SHELL_PRINT_f PrintFunc, const char *pFmtString, UINT32 Value, UINT32 Digits)
{
    char StrBuf[64];
    UINT32 ArgsUInt32[2];
    UINT32 StringLength, i;

    ArgsUInt32[0] = Value;

    StringLength = AmbaUtility_StringPrintUInt32(StrBuf, (uint32_t)sizeof(StrBuf), pFmtString, 1U, ArgsUInt32);
    if (StringLength < Digits) {
        for (i = 0; i < (Digits - StringLength); i++) {
            PrintFunc(" ");
        }
    }
    PrintFunc(StrBuf);
}

static void PrintStr(AMBA_SHELL_PRINT_f PrintFunc, const char *pFmtString, const char *Value)
{
    char StrBuf[64];
    const char *ArgsStr[2];
    uint32_t ret = 0U;

    ArgsStr[0] = Value;
    ret = AmbaUtility_StringPrintStr(StrBuf, (uint32_t)sizeof(StrBuf), pFmtString, 1U, ArgsStr);
    PrintFunc(StrBuf);
    if(ret == 0U) {
        ShellPrintFunc("[ERROR] PrintStr fail ");
    }
}

static void dump_version(void)
{
    uint32_t numwritten = 0U;
    const void *ptr;
    char Path[256];
    uint32_t len;
    const char *StrArg[1U];
    UINT32 uint32_args[1U];
    uint32_t ret = 0U;

    pFlexidagTrace = schdr_get_pFlexidagTrace();
    if (pFlexidagTrace == NULL) {
        AmbaPrint_PrintUInt5("[ERROR] : No active components in the system \n", 0U, 0U, 0U, 0U, 0U);
    } else {
        const flexidag_trace_t       *pFlexidag_Trace;
        const component_build_info_t *pEntry;
        uint32_t  slot_loop;

        ret = ambacv_cache_invalidate(pFlexidagTrace, sizeof(flexidag_trace_t));
        pFlexidag_Trace = (flexidag_trace_t *)pFlexidagTrace;
        /* Report scheduler components */
        if (pFlexidag_Trace->sys_buildinfo_table_daddr != 0U) {
            ptr = ambacv_c2v(pFlexidag_Trace->sys_buildinfo_table_daddr);
            AmbaMisra_TypeCast(&pEntry, &ptr);
            if ((pEntry->component_type != 0x0U) && (pEntry->component_name_daddr != 0U)) {  /* 0x0U = COMPONENT_TYPE_END_OF_LIST */
                numwritten = 0U;
                len = (uint32_t)AmbaUtility_StringLength("========== CVFLOW System Components ====================================================\n");
                AmbaUtility_StringCopy(&Path[numwritten], ((SIZE_t)len + (SIZE_t)1U), "========== CVFLOW System Components ====================================================\n");
                numwritten += len;
                (void) numwritten;
                ShellPrintFunc(Path);
                while ((pEntry->component_type != 0x0U) && (pEntry->component_name_daddr != 0U)) {  /* 0x0U = COMPONENT_TYPE_END_OF_LIST */
                    const char *pName;

                    numwritten = 0U;
                    ptr = ambacv_c2v(pEntry->component_name_daddr);
                    AmbaMisra_TypeCast(&pName, &ptr);
                    StrArg[0] = pName;
                    numwritten += AmbaUtility_StringPrintStr(&Path[numwritten], (uint32_t)sizeof(Path), "%s\n", 1, StrArg);
                    StrArg[0] = pEntry->component_version;
                    numwritten += AmbaUtility_StringPrintStr(&Path[numwritten], (uint32_t)sizeof(Path), " sourcever : %s\n", 1, StrArg);
                    StrArg[0] = pEntry->tool_version;
                    numwritten += AmbaUtility_StringPrintStr(&Path[numwritten], (uint32_t)sizeof(Path), " toolchain : %s\n", 1, StrArg);
                    (void) numwritten;
                    ShellPrintFunc(Path);
                    pEntry++;
                } /* while ((pEntry->component_type != COMPONENT_TYPE_END_OF_LIST) && (pEntry->component_name_daddr != NULL)) */
            } /* if ((pEntry->component_type != COMPONENT_TYPE_END_OF_LIST) && (pEntry->component_name_daddr != 0U)) */
        } /* if (pFlexidag_Trace->sys_buildinfo_table_daddr != 0U) */

        for (slot_loop = 0; slot_loop < pFlexidag_Trace->num_slots_enabled; slot_loop++) {
            if (pFlexidag_Trace->slottrace[slot_loop].fd_buildinfo_table_daddr != 0U) {
                ptr = ambacv_c2v(pFlexidag_Trace->slottrace[slot_loop].fd_buildinfo_table_daddr);
                AmbaMisra_TypeCast(&pEntry, &ptr);
                if (pEntry->component_type != 0x0U) { /* 0x0U = COMPONENT_TYPE_END_OF_LIST */
                    numwritten = 0U;
                    uint32_args[0] = slot_loop;
                    numwritten += AmbaUtility_StringPrintUInt32(&Path[numwritten], (uint32_t)sizeof(Path), "========== FlexiDAG Slot[%3u] Components ===============================================\n", 1, uint32_args);
                    (void) numwritten;
                    ShellPrintFunc(Path);
                    while (pEntry->component_type != 0x0U) {  /* 0x0U = COMPONENT_TYPE_END_OF_LIST */
                        if (pEntry->component_name_daddr != 0U) {
                            const char *pName;

                            numwritten = 0U;
                            ptr = ambacv_c2v(pEntry->component_name_daddr);
                            AmbaMisra_TypeCast(&pName, &ptr);
                            StrArg[0] = pName;
                            numwritten += AmbaUtility_StringPrintStr(&Path[numwritten], (uint32_t)sizeof(Path), "%s\n", 1, StrArg);
                            StrArg[0] = pEntry->component_version;
                            numwritten += AmbaUtility_StringPrintStr(&Path[numwritten], (uint32_t)sizeof(Path), " sourcever : %s\n", 1, StrArg);
                            StrArg[0] = pEntry->tool_version;
                            numwritten += AmbaUtility_StringPrintStr(&Path[numwritten], (uint32_t)sizeof(Path), " toolchain : %s\n", 1, StrArg);
                            ShellPrintFunc(Path);
                        } /* if (pEntry->component_name_daddr != 0U) */
                        pEntry++;
                    } /* while ((pEntry->component_type != COMPONENT_TYPE_END_OF_LIST) && (pEntry->component_name_daddr != NULL)) */
                } /* if ((pEntry->component_type != COMPONENT_TYPE_END_OF_LIST) && (pEntry->component_name_daddr != 0U)) */
            } /* if (pFlexidag_Trace->slottrace[slot_loop].fd_buildinfo_table_daddr != 0U) */
        } /* for (slot_loop = 0; slot_loop < pSysSupport->num_slots_enabled; slot_loop++) */
    } /* if (pSysSupport->vpFlexidagTrace != NULL) */

    ShellPrintFunc(Path);
    if(ret != 0U) {
        ShellPrintFunc("[ERROR] dump_version fail ");
    }
}

typedef struct {
    char        instance_name[NAME_MAX_LENGTH];
    char        algorithm_name[NAME_MAX_LENGTH];
    char        step_name[NAME_MAX_LENGTH];
    char        cvtask_name[NAME_MAX_LENGTH];
} sysflow_name_t;

static uint32_t dump_perf_step1(uint32_t flexidag_num_enabled, uint32_t *num_active)
{
    uint32_t loop = 0U;
    const void *ptr;
    uint32_t error_exit = 0U;
    static uint8_t Flexidag_Info[NUM_FLEXIDAG_DUMP][FLEXIDAG_INFO_SIZE] GNU_SECTION_NOZEROINIT;
    const char *StrArg[1U];
    UINT32 uint32_args[1U];
    uint32_t ret = 0U;

    while (loop < flexidag_num_enabled) {
        uint32_t  slot_state;
        uint32_t  num_sysflow_entries;
        uint32_t  flexidag_sysflow_daddr;
        uint32_t  flexidag_name_daddr;

        slot_state              = pFlexidagTrace->slottrace[loop].slot_state;
        num_sysflow_entries     = pFlexidagTrace->slottrace[loop].sysflow_name_table_numlines;
        flexidag_sysflow_daddr  = pFlexidagTrace->slottrace[loop].sysflow_name_table_daddr;
        flexidag_name_daddr     = pFlexidagTrace->slottrace[loop].flexidag_name_daddr;

        if((slot_state == 100U) || (slot_state == 255U)) {  /* 100U = run , 255U = close */
            uint32_t    allocsize;
            sysflow_name_t *pFlexidagSysflowName;
            char  *pFlexidagName;
            uint8_t flexidag_name_len;

            if (flexidag_sysflow_daddr != 0U) {
                ptr = ambacv_c2v(flexidag_sysflow_daddr);
                AmbaMisra_TypeCast(&pFlexidagSysflowName, &ptr);
                ret |= ambacv_cache_invalidate(pFlexidagSysflowName, sizeof(sysflow_name_t));
                if (pFlexidagSysflowName == NULL) {
                    AmbaPrint_PrintUInt5("[ERROR] : flexidag_schdr() : FD[%3d] : Trace could not map sysflow_name_table_daddr (0x%08x)\n", loop, flexidag_sysflow_daddr, 0U, 0U, 0U);
                    error_exit = 1U;
                } /* if (pFlexidagSysflowName == NULL) */else {

                    if (flexidag_name_daddr != 0U) {
                        ptr   = ambacv_c2v(flexidag_name_daddr);
                        AmbaMisra_TypeCast(&pFlexidagName, &ptr);
                        ret |= ambacv_cache_invalidate(pFlexidagName, FLEXIDAG_NAME_MAX_LENGTH);
                        if (pFlexidagName != NULL) {
                            flexidag_name_len = (uint8_t)(AmbaUtility_StringLength(pFlexidagName) + 1U);
                            if (flexidag_name_len > (uint8_t)FLEXIDAG_NAME_MAX_LENGTH) {
                                flexidag_name_len = (uint8_t)FLEXIDAG_NAME_MAX_LENGTH;
                            } /* if (flexidag_name_len > FLEXIDAG_NAME_MAX_LENGTH) */
                        } else {
                            AmbaPrint_PrintUInt5("[WARN!] : flexidag_schdr() : FD[%3d] : Trace could not map flexidag_name_daddr\n", loop, 0U, 0U, 0U, 0U);
                            pFlexidagName   = NULL;
                            flexidag_name_len = 0U;
                        }
                    } /* if (flexidag_name_daddr != 0) */
                    else { /* if (flexidag_name_daddr == 0) */
                        AmbaPrint_PrintUInt5("[WARN!] : flexidag_schdr() : FD[%3d] : Trace has no flexidag_name entry\n", loop, 0U, 0U, 0U, 0U);
                        pFlexidagName     = NULL;
                        flexidag_name_len = 0U;
                    } /* if (flexidag_name_daddr == 0) */

                    allocsize = ((uint32_t)sizeof(ambaprof_flexidag_header_t)) + (num_sysflow_entries * (uint32_t)AMBAPROF_MAX_NAME_LEN);
                    flexidag_info_size[loop]    = allocsize;
                    pFlexidag_Info[loop]        = &Flexidag_Info[loop][0];
                    if (allocsize > FLEXIDAG_INFO_SIZE) {
                        error_exit = 1U;
                        AmbaPrint_PrintUInt5("[ERROR] : flexidag_schdr() : Could not allocate flexidag_info for slot %d (size = %d)\n", loop, allocsize, 0U, 0U, 0U);
                    } /* if (pFlexidag_Info[loop] == NULL) */
                    else { /* if (pFlexidag_Info[loop] != NULL) */
                        ambaprof_flexidag_header_t *pPerfFDHeader;
                        char *pPerfFDSysflowName;
                        uint32_t  sysflow_loop;
                        uint32_t  numwritten,offset = 0U;

                        ret |= AmbaWrap_memset(pFlexidag_Info[loop], 0, allocsize);

                        AmbaMisra_TypeCast(&pPerfFDHeader, &pFlexidag_Info[loop]);
                        pPerfFDSysflowName  = (char *)&pPerfFDHeader[1];
                        pPerfFDHeader->flexidag_slot_id             = loop;
                        pPerfFDHeader->flexidag_num_sysflow_entries = num_sysflow_entries;
                        if (pFlexidagName != NULL) {
                            ret |= AmbaWrap_memcpy(&pPerfFDHeader->flexidag_name[0], pFlexidagName, flexidag_name_len);
                        }

                        for (sysflow_loop = 0U; sysflow_loop < num_sysflow_entries; sysflow_loop++) {
                            StrArg[0] = pFlexidagSysflowName[sysflow_loop].cvtask_name;
                            numwritten = AmbaUtility_StringPrintStr(&pPerfFDSysflowName[offset], AMBAPROF_MAX_NAME_LEN, "%s ", 1, StrArg);
                            uint32_args[0] = 0U;
                            ret |= AmbaUtility_StringPrintUInt32(&pPerfFDSysflowName[offset + numwritten], AMBAPROF_MAX_NAME_LEN, "(%d)", 1, uint32_args);
                            offset += (uint32_t)AMBAPROF_MAX_NAME_LEN;
                        } /* for (sysflow_loop = 0; sysflow_loop < num_sysflow_entries; sysflow_loop++) */
                        (*num_active)++;
                    } /* if (pFlexidag_Info[loop] != NULL) */
                }
            }
        } /* if ((slot_state > 0) && (slot_state < 255)) */
        loop++;
    }

    if(ret != 0U) {
        ShellPrintFunc("[ERROR] dump_perf_step1 fail ");
    }
    return error_exit;
}

static uint32_t dump_perf_step2(void)
{
    uint32_t loop = 0U;
    uint32_t error_exit = 0U;
    const void *ptr;
    static uint8_t VISPerfCopy[NUM_VISPERF_DUMP][VIS_PERF_COPY_SIZE] GNU_SECTION_NOZEROINIT;
    uint32_t ret = 0U;

    while (loop < NUM_VISPERF_DUMP) {
        uint32_t perf_daddr;
        uint32_t perf_size;

        perf_daddr  = pFlexidagTrace->sysvis_printbuf_perf_daddr[loop];
        perf_size   = pFlexidagTrace->sysvis_printbuf_perf_size[loop];
        if ((perf_daddr != 0U) && (perf_size != 0U)) {
            const uint8_t *pSrcBuf;

            ptr = ambacv_c2v(perf_daddr);
            AmbaMisra_TypeCast(&pSrcBuf, &ptr);
            if (pSrcBuf == NULL) {
                error_exit = 1U;
                AmbaPrint_PrintUInt5("[ERROR] : flexidag_schdr() : FD[%3d] : Trace could not map fdvis_printbuf_perf_daddr (0x%08x)\n", loop, perf_daddr, 0U, 0U, 0U);
            } /* if (pSrcBuf == NULL) */
            else { /* if (pSrcBuf != NULL) */
                pVISPerfCopy[loop] = &VISPerfCopy[loop][0];
                if (perf_size > VIS_PERF_COPY_SIZE) {
                    error_exit = 1U;
                    AmbaPrint_PrintUInt5("[ERROR] : flexidag_schdr() : Could not allocate buffer for VISPERF[%d] (size = %d)\n", loop, perf_size, 0U, 0U, 0U);
                } /* if (pVISPerfCopy[loop] == NULL) */
                else { /* if (pVISPerfCopy[loop] != NULL) */
                    ret |= AmbaWrap_memcpy(pVISPerfCopy[loop], pSrcBuf, perf_size);
                    visperf_size[loop] = perf_size;
                } /* if (pVISPerfCopy[loop] != NULL) */
            } /* if (pSrcBuf != NULL) */
        } /* if ((perf_daddr != 0) && (perf_size != 0)) */
        else { /* if ((perf_daddr == 0) || (perf_size == 0)) */
            AmbaPrint_PrintUInt5("[WARN!] : flexidag_schdr() : VISPERF[%d] : dump requested, but no perf buffer exists for this core\n", loop, 0U, 0U, 0U, 0U);
        } /* if ((perf_daddr == 0) || (perf_size == 0)) */
        loop++;
    } /* while ((loop < num_visperf_dump) && (error_exit == 0)) */

    if(ret != 0U) {
        ShellPrintFunc("[ERROR] dump_perf_step2 fail ");
    }
    return error_exit;
}

static void dump_perf_step3(const char *path,uint32_t num_active,uint32_t num_enabled)
{
    AMBA_FS_FILE *outfile;
    char Path[256];
    uint32_t numwritten,tmp_numwritten;
    uint32_t loop = 0U;
    uint32_t count;
    const char *StrArg[1U];
    UINT32 uint32_args[1U];
    uint32_t ret = 0U;

    while (loop < NUM_VISPERF_DUMP) {
        if ((pVISPerfCopy[loop] != NULL) && (visperf_size[loop] != 0U)) {
            numwritten = 0U;
            StrArg[0] = path;
            tmp_numwritten = AmbaUtility_StringPrintStr(&Path[numwritten], (uint32_t)sizeof(Path), "%s\\visorc", 1, StrArg);
            numwritten += tmp_numwritten;
            uint32_args[0] = loop;
            tmp_numwritten = AmbaUtility_StringPrintUInt32(&Path[numwritten], ((uint32_t)sizeof(Path) - numwritten), "%d.prf", 1, uint32_args);
            numwritten += tmp_numwritten;
            ret |= AmbaFS_FileOpen(Path, "wb", &outfile);
            if (outfile != NULL) {
                ambaprof_header_t   prof_header;
                ambaprof_section_t  prof_section;
                uint32_t    slotloop;
                uint32_t    sectionpos;

                prof_header.version     = 2;
                prof_header.magic_num   = 0xBABEFACEU;
                prof_header.section_count = 1U + num_active;
                prof_header.padding     = 0U;

                ret |= AmbaFS_FileWrite(&prof_header, (uint32_t)sizeof(prof_header), 1U, outfile, &count);
                sectionpos = ((uint32_t)sizeof(prof_header)) + (prof_header.section_count * (uint32_t)sizeof(prof_section));
                for (slotloop = 0U; slotloop < num_enabled; slotloop++) {
                    if ((pFlexidag_Info[slotloop] != NULL) && (flexidag_info_size[slotloop] != 0U)) {
                        ret |= AmbaWrap_memset(&prof_section, 0, sizeof(prof_section));
                        AmbaUtility_StringCopy(&prof_section.name[0], sizeof(prof_section.name),"flexidag names");
                        prof_section.size = flexidag_info_size[slotloop];
                        prof_section.base = sectionpos;
                        sectionpos += prof_section.size;
                        ret |= AmbaFS_FileWrite(&prof_section, (uint32_t)sizeof(prof_section), 1U, outfile, &count);
                    } /* if ((pFlexidag_Info[slotloop] != NULL) && (flexidag_info_size[slotloop] != 0)) */
                } /* if ((pFlexidag_Info[slotloop] != NULL) && (flexidag_info_size[slotloop] != 0)) */

                ret |= AmbaWrap_memset(&prof_section, 0, sizeof(prof_section));
                AmbaUtility_StringCopy(&prof_section.name[0], sizeof(prof_section.name),"profile events");
                prof_section.size = visperf_size[loop];
                prof_section.base = sectionpos;
                ret |= AmbaFS_FileWrite(&prof_section, (uint32_t)sizeof(prof_section), 1U, outfile, &count);
                for (slotloop = 0U; slotloop < num_enabled; slotloop++) {
                    if ((pFlexidag_Info[slotloop] != NULL) && (flexidag_info_size[slotloop] != 0U)) {
                        ret |= AmbaFS_FileWrite(pFlexidag_Info[slotloop], flexidag_info_size[slotloop], 1U, outfile, &count);
                    } /* if ((pFlexidag_Info[slotloop] != NULL) && (flexidag_info_size[slotloop] != 0)) */
                } /* if ((pFlexidag_Info[slotloop] != NULL) && (flexidag_info_size[slotloop] != 0)) */

                ret |= AmbaFS_FileWrite(pVISPerfCopy[loop], visperf_size[loop], 1U, outfile, &count);
                ret |= AmbaFS_FileClose(outfile);
            } /* if (outfile != NULL) */
            else { /* if (outfile == NULL) */
                AmbaPrint_PrintStr5("[ERROR] : flexidag_schdr() : VISPERF : dump requested, but could not create file %s\n", Path, NULL, NULL, NULL, NULL);
            } /* if (outfile == NULL) */
        } /* if ((pVISPerfCopy[loop] != NULL) && (visperf_size[loop] != 0)) */
        loop++;
    } /* while ((loop < num_visperf_dump) && (error_exit == 0)) */

    if(ret != 0U) {
        ShellPrintFunc("[ERROR] dump_perf_step3 fail ");
    }
}

static void dump_perf(const char *path)
{
    uint32_t flexidag_num_enabled;
    uint32_t flexidag_num_active;
    uint32_t loop;
    uint32_t error_exit = 0U;
    uint32_t ret = 0U;

    pFlexidagTrace = schdr_get_pFlexidagTrace();
    if(pFlexidagTrace == NULL) {
        AmbaPrint_PrintUInt5("[ERROR] : No active components in the system \n", 0U, 0U, 0U, 0U, 0U);
    } else {
        ret |= ambacv_cache_invalidate(pFlexidagTrace, sizeof(flexidag_trace_t));
        flexidag_num_enabled    = pFlexidagTrace->num_slots_enabled;  /* May change during run */
        flexidag_num_active = 0U;

        for (loop = 0U; loop < (uint32_t)SYSTEM_MAX_NUM_VISORC; loop++) {
            pVISPerfCopy[loop]        = NULL;
            visperf_size[loop]        = 0U;
        } /* for (loop = 0; loop < SYSTEM_MAX_NUM_VISORC; loop++) */

        for (loop = 0U; loop < (uint32_t)FLEXIDAG_MAX_SLOTS; loop++) {
            pFlexidag_Info[loop]      = NULL;
            flexidag_info_size[loop]  = 0U;
        }
        flexidag_num_active   = 0U;

        if(flexidag_num_enabled > NUM_FLEXIDAG_DUMP) {
            AmbaPrint_PrintUInt5("[ERROR] : flexidag_num_enabled (%d ) >  NUM_FLEXIDAG_DUMP (%d)\n", flexidag_num_enabled, NUM_FLEXIDAG_DUMP, 0U, 0U, 0U);
            error_exit = 1U;
        }

        if(error_exit == 0U) {
            /* Retrieve flexidag information */
            error_exit = dump_perf_step1(flexidag_num_enabled, &flexidag_num_active);
        }

        if(error_exit == 0U) {
            error_exit = dump_perf_step2();
        }

        if (error_exit == 0U) {
            dump_perf_step3(path,flexidag_num_active,flexidag_num_enabled);
        } /* if (error_exit == 0) */
    }

    if(ret != 0U) {
        ShellPrintFunc("[ERROR] dump_perf fail ");
    }
}

static void show_perf(const AMBA_CV_FLEXIDAG_PERF_s *flexidag_perf_array, uint32_t num_of_flexidag)
{
    uint32_t i,totoal_flexidag_run = 0U, percent,total_time = 0U;
    FLOAT fltmp,value;

    ShellPrintFunc("               label                  runs      vp total_run(us)     total(us)     vp min(us)     vp avg(us)     vp max(us)     frame min(us)     frame avg(us)     frame max(us)\n");
    ShellPrintFunc("=================================================================================================================================================================================\n");
    for(i = 0U; i < num_of_flexidag; i++) {
        value = (FLOAT) flexidag_perf_array[i].total_time;
        fltmp = (FLOAT) (flexidag_perf_array[i].total_run_time);
        fltmp = (FLOAT) ((fltmp*100.0) / value);
        percent = (uint32_t)fltmp;

        PrintStr(ShellPrintFunc, "%32s", flexidag_perf_array[i].label);
        PrintInt(ShellPrintFunc, "%d", flexidag_perf_array[i].runs, 10U);
        PrintInt(ShellPrintFunc, "%d", flexidag_perf_array[i].total_run_time, 15U);
        ShellPrintFunc(" (");
        PrintInt(ShellPrintFunc, "%02d", percent,3U);
        ShellPrintFunc(" %)");
        PrintInt(ShellPrintFunc, "%d", flexidag_perf_array[i].total_time, 12U);
        PrintInt(ShellPrintFunc, "%d", flexidag_perf_array[i].min_time, 15U);
        PrintInt(ShellPrintFunc, "%d", flexidag_perf_array[i].avg_time, 15U);
        PrintInt(ShellPrintFunc, "%d", flexidag_perf_array[i].max_time, 15U);
        PrintInt(ShellPrintFunc, "%d", flexidag_perf_array[i].min_frame_time, 18U);
        PrintInt(ShellPrintFunc, "%d", flexidag_perf_array[i].avg_frame_time, 18U);
        PrintInt(ShellPrintFunc, "%d", flexidag_perf_array[i].max_frame_time, 18U);
        ShellPrintFunc("\n");
        totoal_flexidag_run += flexidag_perf_array[i].total_run_time;
        total_time = flexidag_perf_array[i].total_time;
    }
    ShellPrintFunc("=================================================================================================================================================================================\n");
    if(num_of_flexidag == 0U) {
        percent = 0U;
    } else {
        value = (FLOAT) flexidag_perf_array[num_of_flexidag - 1U].total_time;
        fltmp = (FLOAT) (totoal_flexidag_run);
        fltmp = (FLOAT) ((fltmp*100.0) / value);
        percent = (uint32_t)fltmp;
    }

    PrintInt(ShellPrintFunc, "%d", totoal_flexidag_run, 57U);
    ShellPrintFunc(" (");
    PrintInt(ShellPrintFunc, "%02d", percent,3U);
    ShellPrintFunc(" %)");
    PrintInt(ShellPrintFunc, "%d", total_time, 12U);
    ShellPrintFunc("\n\n");
}

static void dump_loading(void)
{
    uint32_t  thread_loop;
    scheduler_trace_t *pSchedulerTrace;
    cvthread_trace_t  *pCVThreadLoad;
    uint32_t clock_ms = 12288,freq;
    uint32_t ret = 0U;

    if(AmbaSYS_GetClkFreq(AMBA_SYS_CLK_AUD_0, &freq) == 0U) {
        clock_ms = freq/1000U;
    } else {
        ShellPrintFunc("[ERROR] dump_loading() : AmbaSYS_GetClkFreq fail \n");
    }

    pSchedulerTrace = schdr_get_pSchedulerTrace();
    if(pSchedulerTrace != NULL) {
        ret |= ambacv_cache_invalidate(pSchedulerTrace, sizeof(scheduler_trace_t));
        pCVThreadLoad   = &pSchedulerTrace->cvthread_loadtrace[0];
        ret |= ambacv_cache_invalidate(pCVThreadLoad, sizeof(cvthread_trace_t));
        ShellPrintFunc("--- CORE NAME ---+- rpt. time -+--500ms--+-- 2.5s--+--10.0s--\n");
        for (thread_loop = 0U; thread_loop < CVTRACE_MAX_ENTRIES; thread_loop++) {
            if (pCVThreadLoad->cvcore_name[0] != '\0') {
                uint32_t  cvload_report_time  = pCVThreadLoad->cvload_report_time;
                uint32_t  cvload_500ms        = pCVThreadLoad->cvload_500ms;
                uint32_t  cvload_2500ms       = pCVThreadLoad->cvload_2500ms;
                uint32_t  cvload_10000ms      = pCVThreadLoad->cvload_10000ms;
                uint32_t  cvload_500ms_a, cvload_500ms_b;
                uint32_t  cvload_2500ms_a, cvload_2500ms_b;
                uint32_t  cvload_10000ms_a, cvload_10000ms_b;

                if (cvload_500ms   > (clock_ms*500U)) {
                    cvload_500ms = (clock_ms*500U);
                }
                if (cvload_2500ms  > (clock_ms*2500U)) {
                    cvload_2500ms = (clock_ms*2500U);
                }
                if (cvload_10000ms > (clock_ms*10000U)) {
                    cvload_10000ms = (clock_ms*10000U);
                }
                cvload_500ms_a    = (uint32_t) (cvload_500ms / (clock_ms*5U));
                cvload_500ms_b    = (uint32_t) ((cvload_500ms - (cvload_500ms_a * (clock_ms*5U))) / (clock_ms/20U));
                cvload_2500ms_a   = (uint32_t) (cvload_2500ms / (clock_ms*25U));
                cvload_2500ms_b   = (uint32_t) ((cvload_2500ms - (cvload_2500ms_a * (clock_ms*25U))) / (clock_ms/4U));
                cvload_10000ms_a  = (uint32_t) (cvload_10000ms / (clock_ms*100U));
                cvload_10000ms_b  = (uint32_t) ((cvload_10000ms - (cvload_10000ms_a * (clock_ms*100U))) / clock_ms);

                ShellPrintFunc("VIS:");
                PrintStr(ShellPrintFunc, "%12s", pCVThreadLoad->cvcore_name);
                ShellPrintFunc(" | @");
                PrintInt(ShellPrintFunc, "%10u", cvload_report_time, 10U);
                ShellPrintFunc(" | ");
                PrintInt(ShellPrintFunc, "%3d", cvload_500ms_a, 3U);
                ShellPrintFunc(".");
                PrintInt(ShellPrintFunc, "%02d", cvload_500ms_b, 2U);
                ShellPrintFunc("% | ");
                PrintInt(ShellPrintFunc, "%3d", cvload_2500ms_a, 3U);
                ShellPrintFunc(".");
                PrintInt(ShellPrintFunc, "%02d", cvload_2500ms_b, 2U);
                ShellPrintFunc("% | ");
                PrintInt(ShellPrintFunc, "%3d", cvload_10000ms_a, 3U);
                ShellPrintFunc(".");
                PrintInt(ShellPrintFunc, "%02d", cvload_10000ms_b, 2U);
                ShellPrintFunc("% \n");
            } /* if (pCVThreadLoad->cvcore_name[0] != 0) */
            pCVThreadLoad++;
        } /* for (thread_loop = 0; thread_loop < CVTRACE_MAX_ENTRIES; thread_loop++) */
        if(ret != 0U) {
            ShellPrintFunc("[ERROR] dump_loading fail ");
        }
    }
}

static void dump_autorun(void)
{
    autorun_trace_t *pAutoRunTraceEntry;
    uint32_t  num_entries;
    uint32_t ret = 0U;

    pAutoRunTraceEntry = schdr_get_pAutoRunTrace();
    if(pAutoRunTraceEntry != NULL) {
        ret |= ambacv_cache_invalidate(pAutoRunTraceEntry, (sizeof(autorun_trace_t)*MAX_AUTORUN_CVTASKS));
        num_entries = pAutoRunTraceEntry->autorun_cvtask_num;
        if (num_entries == 0U) {
            ShellPrintFunc("[AUTORUN] : No AutoRUN tasks exist in the system\n");
        } /* if (num_entries == 0) */
        else if (num_entries < MAX_AUTORUN_CVTASKS) {
            uint32_t  loop;
            ShellPrintFunc("--- AUTORUN_CVTASK_NAME -------- : per(ms) | ptime(ms) |   num_pass @(  passtime) |   num_fail @(  failtime) | failrc\n");
            for (loop = 0; loop < num_entries; loop++) {
                PrintStr(ShellPrintFunc, "%32s", &pAutoRunTraceEntry->autorun_cvtask_name[0]);
                ShellPrintFunc(" :   ");
                PrintInt(ShellPrintFunc, "%5u", pAutoRunTraceEntry->autorun_period_ms, 5U);
                ShellPrintFunc(" |     ");
                PrintInt(ShellPrintFunc, "%5u", pAutoRunTraceEntry->autorun_timeout_ms, 5U);
                ShellPrintFunc(" | ");
                PrintInt(ShellPrintFunc, "%10u", pAutoRunTraceEntry->autorun_num_pass, 10U);
                ShellPrintFunc(" @(");
                PrintInt(ShellPrintFunc, "%10u", pAutoRunTraceEntry->autorun_last_pass_tick, 10U);
                ShellPrintFunc(") | ");
                PrintInt(ShellPrintFunc, "%10u", pAutoRunTraceEntry->autorun_num_fail, 10U);
                ShellPrintFunc(" @(");
                PrintInt(ShellPrintFunc, "%10u", pAutoRunTraceEntry->autorun_last_fail_tick, 10U);
                ShellPrintFunc(") | 0x");
                PrintInt(ShellPrintFunc, "%08x\n", pAutoRunTraceEntry->autorun_last_fail_retcode, 8U);
                ShellPrintFunc("\n");
                pAutoRunTraceEntry++;
            } /* for (loop = 0; loop < num_entries; loop++) */
        } /* if (num_entries < MAX_AUTORUN_CVTASKS) */
        else { /* if (num_entries >= MAX_AUTORUN_CVTASKS) */
            ShellPrintFunc("[AUTORUN] : System corrupted : MAX_AUTORUN_CVTASKS = ");
            PrintInt(ShellPrintFunc, "%d", MAX_AUTORUN_CVTASKS, 10U);
            ShellPrintFunc(", num_entries in table = ");
            PrintInt(ShellPrintFunc, "%d", num_entries, 10U);
            ShellPrintFunc("\n");
        } /* if (num_entries >= MAX_AUTORUN_CVTASKS) */
        if(ret != 0U) {
            ShellPrintFunc("[ERROR] dump_autorun fail ");
        }
    } else {
        ShellPrintFunc("[AUTORUN] : No AutoRUN tasks exist in the system\n");
    }
}

void AmbaShell_CommandFlexidagSchdrUtils(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 Rval = 0U;

    if (AmbaUtility_StringCompare(pArgVector[2], "schdr", 5U) == 0) {
        if ( (AmbaUtility_StringCompare(pArgVector[3], "version", 7U) == 0) && (ArgCount == 4U)) {
            dump_version();
        } else if ( (AmbaUtility_StringCompare(pArgVector[3], "loading", 7U) == 0) && (ArgCount == 4U)) {
            dump_loading();
        } else if ( (AmbaUtility_StringCompare(pArgVector[3], "autorun", 7U) == 0) && (ArgCount == 4U)) {
            dump_autorun();
        } else {
            cv_test_help(PrintFunc);
        }
    }

    (void) Rval;
}

void AmbaShell_CommandFlexidagUtils(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 Rval = 0U;
    uint32_t ret = 0U;

    if ( (AmbaUtility_StringCompare(pArgVector[2], "log_console", 11U) == 0) && (ArgCount == 4U)) {
        const AMBA_CV_FLEXIDAG_HANDLE_s *handle;
        uint32_t value = 0U;
        ULONG addr = 0U;

        Rval = AmbaUtility_StringToUInt32(pArgVector[3],&value);
        addr = value;
        AmbaMisra_TypeCast(&handle, &addr);
        ret |= AmbaPrint_ModuleSetAllowList(SCHDR_PRINT_MODULE_ID, (UINT8)1U);
        ret |= AmbaCV_FlexidagDumpLog(handle,NULL, FLEXILOG_ECHO_TO_CONSOLE|FLEXILOG_VIS_CVTASK);
        ret |= AmbaPrint_ModuleSetAllowList(SCHDR_PRINT_MODULE_ID, (UINT8)0U);
    } else if (AmbaUtility_StringCompare(pArgVector[2], "perf", 4U) == 0) {
        AMBA_CV_FLEXIDAG_PERF_s flexidag_perf_array[16];
        uint32_t num_of_flexidag = 0U;

        if ((AmbaUtility_StringCompare(pArgVector[3], "start", 5U) == 0) && (ArgCount == 4U)) {
            ret |= AmbaCV_FlexidagPerfAll(FLEXIDAG_PERF_START, (uint32_t)CVCORE_VP0, flexidag_perf_array, 16U, &num_of_flexidag);
        } else if ((AmbaUtility_StringCompare(pArgVector[3], "stop", 4U) == 0) && (ArgCount == 4U)) {
            ShellPrintFunc("runs            : how many runs in this perf\n");
            ShellPrintFunc("vp total_run    : total vp run time in this perf (us)\n");
            ShellPrintFunc("total           : total time in this perf (us)\n");
            ShellPrintFunc("vp min          : min time of vp run in one frame (us) - without yeild\n");
            ShellPrintFunc("vp avg          : avg time of vp run in one frame (us) - without yeild\n");
            ShellPrintFunc("vp max          : max time of vp run in one frame (us) - without yeild\n");
            ShellPrintFunc("frame min       : min time in one frame (us) - with yeild\n");
            ShellPrintFunc("frame avg       : avg time in one frame (us) - with yeild\n");
            ShellPrintFunc("frame max       : max time in one frame (us) - with yeild\n");

            ret |= AmbaCV_FlexidagPerfAll(FLEXIDAG_PERF_STOP, (uint32_t)CVCORE_VP0, flexidag_perf_array, 16U, &num_of_flexidag);
            ShellPrintFunc("CVCORE_VP0 :\n");
            show_perf(flexidag_perf_array,num_of_flexidag);
#if defined(CONFIG_SOC_CV2)
            ret |= AmbaCV_FlexidagPerfAll(FLEXIDAG_PERF_STOP, (uint32_t)CVCORE_SP, flexidag_perf_array, 16U, &num_of_flexidag);
            ShellPrintFunc("CVCORE_SP :\n");
            show_perf(flexidag_perf_array,num_of_flexidag);

            ret |= AmbaCV_FlexidagPerfAll(FLEXIDAG_PERF_STOP, (uint32_t)CVCORE_FMA, flexidag_perf_array, 16U, &num_of_flexidag);
            ShellPrintFunc("CVCORE_FMA :\n");
            show_perf(flexidag_perf_array,num_of_flexidag);
#elif defined(CONFIG_SOC_CV2FS)
            ret |= AmbaCV_FlexidagPerfAll(FLEXIDAG_PERF_STOP, (uint32_t)CVCORE_FEX, flexidag_perf_array, 16U, &num_of_flexidag);
            ShellPrintFunc("CVCORE_FEX :\n");
            show_perf(flexidag_perf_array,num_of_flexidag);
#endif
        } else if ((AmbaUtility_StringCompare(pArgVector[3], "dump", 4U) == 0) && (ArgCount == 5U)) {
            dump_perf(pArgVector[4]);
        }  else {
            cv_test_help(PrintFunc);
        }
    } else {
        cv_test_help(PrintFunc);
    }

    (void) Rval;
    if(ret != 0U) {
        ShellPrintFunc("[ERROR] AmbaShell_CommandFlexidagUtils fail ");
    }
}

