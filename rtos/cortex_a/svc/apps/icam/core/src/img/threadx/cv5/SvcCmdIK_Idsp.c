/**
 *  @file SvcCmdIK_Idsp.c
 *
 * Copyright (c) [2020] Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella International LP.
 *
 * This file includes sample code and is only for internal testing and evaluation.  If you 
 * distribute this sample code (whether in source, object, or binary code form), it will be 
 * without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details svc application 'ik/idsp' shell command
 *
 */

#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaMisraFix.h"
#include "AmbaWrap.h"
#include "AmbaUtility.h"
#include "AmbaMMU.h"
#include "AmbaCache.h"
#include "AmbaKAL.h"

#include "AmbaDSP.h"
#include "AmbaDSP_Liveview_Def.h"
#include "AmbaDSP_ImageFilter.h"

#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcResCfg.h"
#include "SvcPlat.h"

#include "../inc/SvcCmdIK.h"

#define SVC_CMD_IK             "CMD_IK"

#define SVC_CMD_IK_MAX_MEM_NUM    (2U)

typedef struct {
    UINT64 VirtBase;
    UINT32 PhysBase;
    UINT32 Size;
} SVC_CMD_IK_MEM_BLK_s;

#define SVC_CMD_IK_IDSP_DUMP_LVL0   (0U)
#define SVC_CMD_IK_IDSP_DUMP_LVL1   (0x1U)
#define SVC_CMD_IK_IDSP_DUMP_LVL2   (0x2U)
#define SVC_CMD_IK_IDSP_DUMP_LVL3   (0x4U)
#define SVC_CMD_IK_IDSP_DUMP_LVL4   (0x8U)
#define SVC_CMD_IK_IDSP_DUMP_LVL5   (0x10U)
#define SVC_CMD_IK_IDSP_DUMP_LVL6   (0x20U)
#define SVC_CMD_IK_IDSP_DUMP_LVL7   (0x40U)
#define SVC_CMD_IK_IDSP_DUMP_LVL8   (0x80U)
#define SVC_CMD_IK_IDSP_DUMP_LVL9   (0x100U)

#define SVC_CMD_IK_IDSP_DUMP_LVL0_NUM   (0x2U)
static SVC_CMD_IK_IDSP_DUMP_INFO_s SvcCmdIKIdspDumpInfo0[SVC_CMD_IK_IDSP_DUMP_LVL0_NUM] GNU_SECTION_NOZEROINIT;

#define SVC_CMD_IK_IDSP_DUMP_LVL1_NUM   (0x5U)
static SVC_CMD_IK_IDSP_DUMP_INFO_s SvcCmdIKIdspDumpInfo1[SVC_CMD_IK_IDSP_DUMP_LVL1_NUM] GNU_SECTION_NOZEROINIT;

#define SVC_CMD_IK_IDSP_DUMP_LVL2_NUM   (0x9U)
static SVC_CMD_IK_IDSP_DUMP_INFO_s SvcCmdIKIdspDumpInfo2[SVC_CMD_IK_IDSP_DUMP_LVL2_NUM] GNU_SECTION_NOZEROINIT;

#define SVC_CMD_IK_IDSP_DUMP_LVL3_NUM   (0x5U)
static SVC_CMD_IK_IDSP_DUMP_INFO_s SvcCmdIKIdspDumpInfo3[SVC_CMD_IK_IDSP_DUMP_LVL3_NUM] GNU_SECTION_NOZEROINIT;

#define SVC_CMD_IK_IDSP_DUMP_LVL4_NUM   (0x9U)
static SVC_CMD_IK_IDSP_DUMP_INFO_s SvcCmdIKIdspDumpInfo4[SVC_CMD_IK_IDSP_DUMP_LVL4_NUM] GNU_SECTION_NOZEROINIT;

#define SVC_CMD_IK_IDSP_DUMP_LVL5_NUM   (0x50U)
static SVC_CMD_IK_IDSP_DUMP_INFO_s SvcCmdIKIdspDumpInfo5[SVC_CMD_IK_IDSP_DUMP_LVL5_NUM] GNU_SECTION_NOZEROINIT;

#define SVC_CMD_IK_IDSP_DUMP_LVL6_NUM   (0x3U)
static SVC_CMD_IK_IDSP_DUMP_INFO_s SvcCmdIKIdspDumpInfo6[SVC_CMD_IK_IDSP_DUMP_LVL6_NUM] GNU_SECTION_NOZEROINIT;

#define SVC_CMD_IK_IDSP_DUMP_LVL7_NUM   (0x134U)
static SVC_CMD_IK_IDSP_DUMP_INFO_s SvcCmdIKIdspDumpInfo7[SVC_CMD_IK_IDSP_DUMP_LVL7_NUM] GNU_SECTION_NOZEROINIT;

#define SVC_CMD_IK_IDSP_DUMP_LVL8_NUM   (0x180U)
static SVC_CMD_IK_IDSP_DUMP_INFO_s SvcCmdIKIdspDumpInfo8[SVC_CMD_IK_IDSP_DUMP_LVL8_NUM] GNU_SECTION_NOZEROINIT;

#define SVC_CMD_IK_IDSP_DUMP_LVL9_NUM   (0x2U)
static SVC_CMD_IK_IDSP_DUMP_INFO_s SvcCmdIKIdspDumpInfo9[SVC_CMD_IK_IDSP_DUMP_LVL9_NUM] GNU_SECTION_NOZEROINIT;

#define SVC_CMD_IK_IDSP_DUMP_PROC_NUM   (10U)
static SVC_CMD_IK_IDSP_DUMP_FUNC_s SvcCmdIKIdspDumpFunc[SVC_CMD_IK_IDSP_DUMP_PROC_NUM] GNU_SECTION_NOZEROINIT;

static SVC_CMD_IK_MEM_BLK_s SvcCmdIkMemBlkMap[SVC_CMD_IK_MAX_MEM_NUM] GNU_SECTION_NOZEROINIT;

#define PRN_CMD_IK_ERR_HDLR SvcCmdIK_ErrHdlr(__func__, __LINE__, PRetVal);
static void SvcCmdIK_ErrHdlr(const char *pCaller, UINT32 CodeLine, UINT32 ErrCode);

static UINT32 SvcCmdIk_IdspCfg1Update(void *pCfgBuf, void *pBuf);
static UINT32 SvcCmdIk_IdspCfg2Update(void *pCfgBuf, void *pBuf);
static UINT32 SvcCmdIk_IdspCfg3Update(void *pCfgBuf, void *pBuf);
static UINT32 SvcCmdIk_IdspCfg4Update(void *pCfgBuf, void *pBuf);
static UINT32 SvcCmdIk_IdspCfg5Update(void *pCfgBuf, void *pBuf);
static UINT32 SvcCmdIk_IdspCfg6Update(void *pCfgBuf, void *pBuf);
static UINT32 SvcCmdIk_IdspCfg7Update(void *pCfgBuf, void *pBuf);
static UINT32 SvcCmdIk_IdspCfg18Update(void *pCfgBuf, void *pBuf);
static UINT32 SvcCmdIK_ConfigUpdate(UINT32 CfgID, void *pCfgBuf, void *pUserBuf);

static UINT32 SvcCmdIK_InvalidCache(const void *pDataBuf, UINT32 DataSize)
{
    UINT32 RetVal = SVC_NG;
    const void *pAlign;
#ifdef CONFIG_ARM64
    UINT64 DataAddr, AlignAddr, AlignSize;

    AmbaMisra_TypeCast(&(DataAddr), &(pDataBuf));

    if (DataAddr >= 64U) {
        AlignAddr = DataAddr & 0xFFFFFFFFFFFFFFC0U;
        AmbaMisra_TypeCast(&(AlignSize), &(DataSize)); AlignSize &= 0x00000000FFFFFFFFU;

        if (AlignAddr < DataAddr) {
            AlignSize += DataAddr - AlignAddr;
        }
        AlignSize = GetAlignedValU64(AlignSize, 64U);

        RetVal = SvcPlat_CacheInvalidate((ULONG)AlignAddr, (ULONG)AlignSize);
        AmbaMisra_TypeCast(&(pAlign), &(AlignAddr));

        if (RetVal != 0U) {
            SVC_WRAP_PRINT "Fail to process invalid data fail! ErrCode(0x%08x), VirtAddr(%p) Size(0x%x)"
                SVC_PRN_ARG_S SVC_CMD_IK
                SVC_PRN_ARG_UINT32 RetVal            SVC_PRN_ARG_POST
                SVC_PRN_ARG_CPOINT pAlign            SVC_PRN_ARG_POST
                SVC_PRN_ARG_UINT32 (UINT32)AlignSize SVC_PRN_ARG_POST
                SVC_PRN_ARG_E
        }
    }
#else
    UINT32 DataAddr, AlignAddr, AlignSize;

    AmbaMisra_TypeCast(&(DataAddr), &(pDataBuf));

    if (DataAddr >= 64U) {
        AlignAddr = DataAddr & 0xFFFFFFC0U;
        AlignSize = DataSize;
        if (AlignAddr < DataAddr) {
            AlignSize += DataAddr - AlignAddr;
        }
        AlignSize = GetAlignedValU32(AlignSize, 64U);

        RetVal = SvcPlat_CacheInvalidate(AlignAddr, AlignSize);
        AmbaMisra_TypeCast(&(pAlign), &(AlignAddr));

        if (RetVal != 0U) {
            SVC_WRAP_PRINT "Fail to process invalid data fail! ErrCode(0x%08x), VirtAddr(%p) Size(0x%x)"
                SVC_PRN_ARG_S SVC_CMD_IK
                SVC_PRN_ARG_UINT32 RetVal    SVC_PRN_ARG_POST
                SVC_PRN_ARG_CPOINT pAlign    SVC_PRN_ARG_POST
                SVC_PRN_ARG_UINT32 AlignSize SVC_PRN_ARG_POST
                SVC_PRN_ARG_E
        }
    }
#endif

    return RetVal;
}

static void SvcCmdIK_MemCopyEx(void *pDst, void *pSrc, UINT32 CopySize)
{
    if ((pDst != NULL) && (pSrc != NULL) && (CopySize > 0U)) {
        extern UINT32 SvcCmdIK_IsCacheMemory(void *pBuf);
        
        if (SVC_OK == SvcCmdIK_IsCacheMemory(pSrc)) {
            if (0U != SvcCmdIK_InvalidCache(pSrc, CopySize)) {
                SVC_WRAP_PRINT "Fail to copy memory - invalid cache! pSrc(%p) Size(0x%x)"
                    SVC_PRN_ARG_S SVC_CMD_IK
                    SVC_PRN_ARG_CPOINT pSrc     SVC_PRN_ARG_POST
                    SVC_PRN_ARG_UINT32 CopySize SVC_PRN_ARG_POST
                    SVC_PRN_ARG_E
            }
        }

        AmbaSvcWrap_MisraMemcpy(pDst, pSrc, CopySize);
        AmbaMisra_TouchUnused(pDst);
    }
}

static void *SvcCmdIK_PhysToVirt(UINT32 PhysAddr)
{
    void *pOutPtr = NULL;
    UINT64 PhyOffsetPos = 0U;
    UINT64 PhyOffsetNeg = 0U;
    UINT64 VirtAddr     = 0U;

    UINT32 PhyStart0 = SvcCmdIkMemBlkMap[0].PhysBase;
    UINT32 PhyEnd0   = SvcCmdIkMemBlkMap[0].PhysBase + SvcCmdIkMemBlkMap[0].Size;
    UINT32 PhyStart1 = SvcCmdIkMemBlkMap[1].PhysBase;
    UINT32 PhyEnd1   = SvcCmdIkMemBlkMap[1].PhysBase + SvcCmdIkMemBlkMap[1].Size;

    if((PhysAddr >= PhyStart0) && (PhysAddr <= PhyEnd0)) {
        if(SvcCmdIkMemBlkMap[0].VirtBase >= SvcCmdIkMemBlkMap[0].PhysBase){
            PhyOffsetPos = SvcCmdIkMemBlkMap[0].VirtBase - SvcCmdIkMemBlkMap[0].PhysBase;
            PhyOffsetNeg = 0u;
        } else {
            PhyOffsetPos = 0u;
            PhyOffsetNeg = SvcCmdIkMemBlkMap[0].PhysBase - SvcCmdIkMemBlkMap[0].VirtBase;
        }
    } else if((PhysAddr >= PhyStart1) && (PhysAddr <= PhyEnd1)) {
        if(SvcCmdIkMemBlkMap[1].VirtBase >= SvcCmdIkMemBlkMap[1].PhysBase){
            PhyOffsetPos = SvcCmdIkMemBlkMap[1].VirtBase - SvcCmdIkMemBlkMap[1].PhysBase;
            PhyOffsetNeg = 0u;
        } else {
            PhyOffsetPos = 0u;
            PhyOffsetNeg = SvcCmdIkMemBlkMap[1].PhysBase - SvcCmdIkMemBlkMap[1].VirtBase;
        }
    } else {
        PhyOffsetPos = 0u;
        PhyOffsetNeg = 0u;
    }

    AmbaMisra_TouchUnused(&PhyOffsetPos);
    AmbaMisra_TouchUnused(&PhyOffsetNeg);

    if((PhyOffsetPos != 0u) && (PhyOffsetNeg == 0u)) {
        VirtAddr = (UINT64)PhyOffsetPos + (UINT64)PhysAddr;
    } else if ((PhyOffsetPos == 0u) && (PhyOffsetNeg != 0u)) {
        VirtAddr = (UINT64)PhysAddr - (UINT64)PhyOffsetNeg;
    } else {
        VirtAddr = (UINT64)PhysAddr;
    }

    AmbaMisra_TypeCast(&pOutPtr, &VirtAddr);

    return pOutPtr;
}

static void SvcCmdIK_ErrHdlr(const char *pCaller, UINT32 CodeLine, UINT32 ErrCode)
{
    if (pCaller != NULL) {
        if (ErrCode != 0U) {

            SVC_WRAP_PRINT "Catch ErrCode(0x%08x) @ %s, %d"
                SVC_PRN_ARG_S SVC_CMD_IK
                SVC_PRN_ARG_PROC SvcLog_NG
                SVC_PRN_ARG_UINT32 ErrCode  SVC_PRN_ARG_POST
                SVC_PRN_ARG_CSTR   pCaller  SVC_PRN_ARG_POST
                SVC_PRN_ARG_UINT32 CodeLine SVC_PRN_ARG_POST
                SVC_PRN_ARG_E
        }
    }
}

/**
 * Initial idsp dump function setting
 *
 * @param [out] pDumpFuncNum dump function number
 * @param [out] pDumFunc dump function list
 * @return ErrCode SVC_OK(0)/SVC_NG(1)
 */
UINT32 SvcCmdIK_DumpIdspInit(UINT32 *pDumpFuncNum, SVC_CMD_IK_IDSP_DUMP_FUNC_s **pDumFunc)
{
    static UINT32 SvcCmdIKCmdDumpInit = 0U;

    if (SvcCmdIKCmdDumpInit == 0U) {

        AmbaSvcWrap_MisraMemset(SvcCmdIKIdspDumpInfo0, 0, sizeof(SvcCmdIKIdspDumpInfo0));
        SvcCmdIKIdspDumpInfo0[ 0] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000108U, 0xF2000118U, 0xF200011AU, 0xF200011CU, NULL };
        SvcCmdIKIdspDumpInfo0[ 1] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000110U, 0xF200011EU, 0xF2000120U, 0xF2000122U, NULL };

        AmbaSvcWrap_MisraMemset(SvcCmdIKIdspDumpInfo1, 0, sizeof(SvcCmdIKIdspDumpInfo1));
        SvcCmdIKIdspDumpInfo1[ 0] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000020U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo1[ 1] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000030U, 0xF2000040U, 0xF2000042U, 0xF2000044U, NULL };
        SvcCmdIKIdspDumpInfo1[ 2] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000038U, 0xF2000046U, 0xF2000048U, 0xF200004AU, NULL };
        SvcCmdIKIdspDumpInfo1[ 3] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000050U, 0xF2000058U, 0xF200005AU, 0xF200005CU, NULL };
        SvcCmdIKIdspDumpInfo1[ 4] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000010U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };

        AmbaSvcWrap_MisraMemset(SvcCmdIKIdspDumpInfo2, 0, sizeof(SvcCmdIKIdspDumpInfo2));
        SvcCmdIKIdspDumpInfo2[ 0] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40000A8U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo2[ 1] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40000D8U, 0xF20000E0U, 0xF20000E2U, 0xF20000E4U, NULL };
        SvcCmdIKIdspDumpInfo2[ 2] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40000B8U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo2[ 3] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40000E8U, 0xF20000F8U, 0xF20000FAU, 0xF20000FCU, NULL };
        SvcCmdIKIdspDumpInfo2[ 4] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40000F0U, 0xF20000FEU, 0xF2000100U, 0xF2000102U, NULL };
        SvcCmdIKIdspDumpInfo2[ 5] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40000C8U, 0xF40000D0U, 0xF40000D0U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo2[ 6] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000108U, 0xF2000118U, 0xF200011AU, 0xF200011CU, NULL };
        SvcCmdIKIdspDumpInfo2[ 7] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000110U, 0xF200011EU, 0xF2000120U, 0xF2000122U, NULL };
        SvcCmdIKIdspDumpInfo2[ 8] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000128U, 0xF2000130U, 0xF2000132U, 0xF2000134U, NULL };

        AmbaSvcWrap_MisraMemset(SvcCmdIKIdspDumpInfo3, 0, sizeof(SvcCmdIKIdspDumpInfo3));
        SvcCmdIKIdspDumpInfo3[ 0] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000180U, 0xF4000188U, 0xF4000188U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo3[ 1] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40001A0U, 0xF40001A8U, 0xF40001A8U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo3[ 2] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000190U, 0xF4000198U, 0xF4000198U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo3[ 3] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40001B0U, 0xF20001B8U, 0xF20001BAU, 0xF20001BCU, NULL };
        SvcCmdIKIdspDumpInfo3[ 4] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40001C0U, 0xF20001C8U, 0xF20001CAU, 0xF20001CCU, NULL };

        AmbaSvcWrap_MisraMemset(SvcCmdIKIdspDumpInfo4, 0, sizeof(SvcCmdIKIdspDumpInfo4));
        SvcCmdIKIdspDumpInfo4[ 0] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000218U, 0xF4000220U, 0xF4000220U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo4[ 1] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000228U, 0xF4000230U, 0xF4000230U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo4[ 2] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000238U, 0xF4000240U, 0xF4000240U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo4[ 3] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000258U, 0xF4000260U, 0xF4000260U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo4[ 4] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000248U, 0xF4000250U, 0xF4000250U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo4[ 5] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000268U, 0xF4000270U, 0xF4000270U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo4[ 6] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000278U, 0xF2000280U, 0xF2000282U, 0xF2000284U, NULL };
        SvcCmdIKIdspDumpInfo4[ 7] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000288U, 0xF2000290U, 0xF2000292U, 0xF2000294U, NULL };
        SvcCmdIKIdspDumpInfo4[ 8] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000298U, 0xF20002A0U, 0xF20002A2U, 0xF20002A4U, NULL };

        AmbaSvcWrap_MisraMemset(SvcCmdIKIdspDumpInfo5, 0, sizeof(SvcCmdIKIdspDumpInfo5));
        SvcCmdIKIdspDumpInfo5[ 0] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40002F0U, 0xF000D702U, 0xF000D702U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[ 1] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40002F8U, 0xF000D702U, 0xF000D702U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[ 2] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000300U, 0xF000D702U, 0xF000D702U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[ 3] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000308U, 0xF000D702U, 0xF000D702U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[ 4] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000310U, 0xF000D702U, 0xF000D702U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[ 5] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000318U, 0xF000D702U, 0xF000D702U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[ 6] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000320U, 0xF000D702U, 0xF000D702U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[ 7] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000328U, 0xF000D702U, 0xF000D702U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[ 8] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000330U, 0xF000D702U, 0xF000D702U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[ 9] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000338U, 0xF000D702U, 0xF000D702U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[10] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000340U, 0xF000D702U, 0xF000D702U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[11] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000348U, 0xF000D702U, 0xF000D702U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[12] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000350U, 0xF000D702U, 0xF000D702U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[13] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000358U, 0xF000D702U, 0xF000D702U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[14] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000360U, 0xF000D702U, 0xF000D702U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[15] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000368U, 0xF000D702U, 0xF000D702U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[16] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40003C0U, 0xF0001402U, 0xF0001402U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[17] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40003C8U, 0xF0001402U, 0xF0001402U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[18] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40003D0U, 0xF0001402U, 0xF0001402U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[19] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40003D8U, 0xF0001402U, 0xF0001402U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[20] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40003E0U, 0xF0001402U, 0xF0001402U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[21] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40003E8U, 0xF0001402U, 0xF0001402U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[22] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40003F0U, 0xF0001402U, 0xF0001402U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[23] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40003F8U, 0xF0001402U, 0xF0001402U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[24] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000400U, 0xF0001402U, 0xF0001402U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[25] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000408U, 0xF0001402U, 0xF0001402U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[26] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000410U, 0xF0001402U, 0xF0001402U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[27] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000418U, 0xF0001402U, 0xF0001402U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[28] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000420U, 0xF0001402U, 0xF0001402U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[29] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000428U, 0xF0001402U, 0xF0001402U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[30] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000430U, 0xF0001402U, 0xF0001402U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[31] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000438U, 0xF0001402U, 0xF0001402U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[32] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000490U, 0xF0000900U, 0xF0000900U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[33] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000498U, 0xF0000900U, 0xF0000900U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[34] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40004A0U, 0xF0000900U, 0xF0000900U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[35] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40004A8U, 0xF0000900U, 0xF0000900U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[36] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40004B0U, 0xF0000900U, 0xF0000900U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[37] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40004B8U, 0xF0000900U, 0xF0000900U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[38] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40004C0U, 0xF0000900U, 0xF0000900U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[39] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40004C8U, 0xF0000900U, 0xF0000900U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[40] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40004D0U, 0xF0000900U, 0xF0000900U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[41] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40004D8U, 0xF0000900U, 0xF0000900U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[42] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40004E0U, 0xF0000900U, 0xF0000900U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[43] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40004E8U, 0xF0000900U, 0xF0000900U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[44] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40004F0U, 0xF0000900U, 0xF0000900U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[45] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40004F8U, 0xF0000900U, 0xF0000900U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[46] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000500U, 0xF0000900U, 0xF0000900U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[47] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000508U, 0xF0000900U, 0xF0000900U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[48] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000560U, 0xF0000900U, 0xF0000900U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[49] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000568U, 0xF0000900U, 0xF0000900U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[50] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000570U, 0xF0000900U, 0xF0000900U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[51] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000578U, 0xF0000900U, 0xF0000900U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[52] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000580U, 0xF0000900U, 0xF0000900U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[53] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000588U, 0xF0000900U, 0xF0000900U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[54] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000590U, 0xF0000900U, 0xF0000900U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[55] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000598U, 0xF0000900U, 0xF0000900U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[56] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40005A0U, 0xF0000900U, 0xF0000900U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[57] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40005A8U, 0xF0000900U, 0xF0000900U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[58] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40005B0U, 0xF0000900U, 0xF0000900U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[59] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40005B8U, 0xF0000900U, 0xF0000900U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[60] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40005C0U, 0xF0000900U, 0xF0000900U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[61] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40005C8U, 0xF0000900U, 0xF0000900U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[62] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40005D0U, 0xF0000900U, 0xF0000900U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[63] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40005D8U, 0xF0000900U, 0xF0000900U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[64] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000630U, 0xF0000900U, 0xF0000900U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[65] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000638U, 0xF0000900U, 0xF0000900U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[66] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000640U, 0xF0000900U, 0xF0000900U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[67] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000648U, 0xF0000900U, 0xF0000900U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[68] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000650U, 0xF0000900U, 0xF0000900U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[69] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000658U, 0xF0000900U, 0xF0000900U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[70] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000660U, 0xF0000900U, 0xF0000900U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[71] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000668U, 0xF0000900U, 0xF0000900U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[72] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000670U, 0xF0000900U, 0xF0000900U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[73] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000678U, 0xF0000900U, 0xF0000900U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[74] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000680U, 0xF0000900U, 0xF0000900U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[75] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000688U, 0xF0000900U, 0xF0000900U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[76] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000690U, 0xF0000900U, 0xF0000900U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[77] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000698U, 0xF0000900U, 0xF0000900U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[78] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40006A0U, 0xF0000900U, 0xF0000900U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo5[79] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40006A8U, 0xF0000900U, 0xF0000900U, 0xF0000001U, NULL };

        AmbaSvcWrap_MisraMemset(SvcCmdIKIdspDumpInfo6, 0, sizeof(SvcCmdIKIdspDumpInfo6));
        SvcCmdIKIdspDumpInfo6[ 0] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000700U, 0xF0000580U, 0xF0000580U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo6[ 1] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000710U, 0xF00000B4U, 0xF00000B4U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo6[ 2] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000718U, 0xF0000244U, 0xF0000244U, 0xF0000001U, NULL };

        AmbaSvcWrap_MisraMemset(SvcCmdIKIdspDumpInfo7, 0, sizeof(SvcCmdIKIdspDumpInfo7));
        SvcCmdIKIdspDumpInfo7[  0] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000D68U, 0xF4000D70U, 0xF4000D70U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo7[  1] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000D78U, 0xF4000D80U, 0xF4000D80U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo7[  2] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000D88U, 0xF4000D90U, 0xF4000D90U, 0xF0000001U, SvcCmdIk_IdspCfg5Update };
        SvcCmdIKIdspDumpInfo7[  3] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000D98U, 0xF4000DA0U, 0xF4000DA0U, 0xF0000001U, SvcCmdIk_IdspCfg6Update };
        SvcCmdIKIdspDumpInfo7[  4] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000DA8U, 0xF4000DB0U, 0xF4000DB0U, 0xF0000001U, SvcCmdIk_IdspCfg7Update };
        SvcCmdIKIdspDumpInfo7[  5] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000DB8U, 0xF4000DC0U, 0xF4000DC0U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo7[  6] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000DC8U, 0xF4000DD0U, 0xF4000DD0U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo7[  7] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000DD8U, 0xF4000DE0U, 0xF4000DE0U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo7[  8] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000DE8U, 0xF2000DF8U, 0xF2000DFAU, 0xF2000DFCU, NULL };
        SvcCmdIKIdspDumpInfo7[  9] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000DF0U, 0xF2000DFEU, 0xF2000E00U, 0xF2000E02U, NULL };
        SvcCmdIKIdspDumpInfo7[ 10] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000E08U, 0xF2000E18U, 0xF2000E1AU, 0xF2000E1CU, NULL };
        SvcCmdIKIdspDumpInfo7[ 11] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000E10U, 0xF2000E1EU, 0xF2000E20U, 0xF2000E22U, NULL };
        SvcCmdIKIdspDumpInfo7[ 12] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000E28U, 0xF2000E38U, 0xF2000E3AU, 0xF2000E3CU, NULL };
        SvcCmdIKIdspDumpInfo7[ 13] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000E30U, 0xF2000E3EU, 0xF2000E40U, 0xF2000E42U, NULL };
        SvcCmdIKIdspDumpInfo7[ 14] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000E48U, 0xF2000E58U, 0xF2000E5AU, 0xF2000E5CU, NULL };
        SvcCmdIKIdspDumpInfo7[ 15] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000E50U, 0xF2000E5EU, 0xF2000E60U, 0xF2000E62U, NULL };
        SvcCmdIKIdspDumpInfo7[ 16] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000E68U, 0xF2000E78U, 0xF2000E7AU, 0xF2000E7CU, NULL };
        SvcCmdIKIdspDumpInfo7[ 17] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000E70U, 0xF2000E7EU, 0xF2000E80U, 0xF2000E82U, NULL };
        SvcCmdIKIdspDumpInfo7[ 18] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000E88U, 0xF2000E98U, 0xF2000E9AU, 0xF2000E9CU, NULL };
        SvcCmdIKIdspDumpInfo7[ 19] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000E90U, 0xF2000E9EU, 0xF2000EA0U, 0xF2000EA2U, NULL };
        SvcCmdIKIdspDumpInfo7[ 20] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000EA8U, 0xF2000EB8U, 0xF2000EBAU, 0xF2000EBCU, NULL };
        SvcCmdIKIdspDumpInfo7[ 21] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000EB0U, 0xF2000EBEU, 0xF2000EC0U, 0xF2000EC2U, NULL };
        SvcCmdIKIdspDumpInfo7[ 22] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000F00U, 0xF4000F08U, 0xF4000F08U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo7[ 23] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000F10U, 0xF4000F18U, 0xF4000F18U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo7[ 24] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000F20U, 0xF4000F28U, 0xF4000F28U, 0xF0000001U, SvcCmdIk_IdspCfg5Update };
        SvcCmdIKIdspDumpInfo7[ 25] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000F30U, 0xF4000F38U, 0xF4000F38U, 0xF0000001U, SvcCmdIk_IdspCfg6Update };
        SvcCmdIKIdspDumpInfo7[ 26] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000F40U, 0xF4000F48U, 0xF4000F48U, 0xF0000001U, SvcCmdIk_IdspCfg7Update };
        SvcCmdIKIdspDumpInfo7[ 27] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000F50U, 0xF4000F58U, 0xF4000F58U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo7[ 28] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000F60U, 0xF4000F68U, 0xF4000F68U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo7[ 29] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000F70U, 0xF4000F78U, 0xF4000F78U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo7[ 30] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000F80U, 0xF2000F90U, 0xF2000F92U, 0xF2000F94U, NULL };
        SvcCmdIKIdspDumpInfo7[ 31] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000F88U, 0xF2000F96U, 0xF2000F98U, 0xF2000F9AU, NULL };
        SvcCmdIKIdspDumpInfo7[ 32] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000FA0U, 0xF2000FB0U, 0xF2000FB2U, 0xF2000FB4U, NULL };
        SvcCmdIKIdspDumpInfo7[ 33] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000FA8U, 0xF2000FB6U, 0xF2000FB8U, 0xF2000FBAU, NULL };
        SvcCmdIKIdspDumpInfo7[ 34] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000FC0U, 0xF2000FD0U, 0xF2000FD2U, 0xF2000FD4U, NULL };
        SvcCmdIKIdspDumpInfo7[ 35] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000FC8U, 0xF2000FD6U, 0xF2000FD8U, 0xF2000FDAU, NULL };
        SvcCmdIKIdspDumpInfo7[ 36] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000FE0U, 0xF2000FF0U, 0xF2000FF2U, 0xF2000FF4U, NULL };
        SvcCmdIKIdspDumpInfo7[ 37] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000FE8U, 0xF2000FF6U, 0xF2000FF8U, 0xF2000FFAU, NULL };
        SvcCmdIKIdspDumpInfo7[ 38] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001000U, 0xF2001010U, 0xF2001012U, 0xF2001014U, NULL };
        SvcCmdIKIdspDumpInfo7[ 39] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001008U, 0xF2001016U, 0xF2001018U, 0xF200101AU, NULL };
        SvcCmdIKIdspDumpInfo7[ 40] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001020U, 0xF2001030U, 0xF2001032U, 0xF2001034U, NULL };
        SvcCmdIKIdspDumpInfo7[ 41] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001028U, 0xF2001036U, 0xF2001038U, 0xF200103AU, NULL };
        SvcCmdIKIdspDumpInfo7[ 42] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001040U, 0xF2001050U, 0xF2001052U, 0xF2001054U, NULL };
        SvcCmdIKIdspDumpInfo7[ 43] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001048U, 0xF2001056U, 0xF2001058U, 0xF200105AU, NULL };
        SvcCmdIKIdspDumpInfo7[ 44] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001098U, 0xF40010A0U, 0xF40010A0U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo7[ 45] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40010A8U, 0xF40010B0U, 0xF40010B0U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo7[ 46] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40010B8U, 0xF40010C0U, 0xF40010C0U, 0xF0000001U, SvcCmdIk_IdspCfg5Update };
        SvcCmdIKIdspDumpInfo7[ 47] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40010C8U, 0xF40010D0U, 0xF40010D0U, 0xF0000001U, SvcCmdIk_IdspCfg6Update };
        SvcCmdIKIdspDumpInfo7[ 48] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40010D8U, 0xF40010E0U, 0xF40010E0U, 0xF0000001U, SvcCmdIk_IdspCfg7Update };
        SvcCmdIKIdspDumpInfo7[ 49] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40010E8U, 0xF40010F0U, 0xF40010F0U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo7[ 50] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40010F8U, 0xF4001100U, 0xF4001100U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo7[ 51] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001108U, 0xF4001110U, 0xF4001110U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo7[ 52] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001118U, 0xF2001128U, 0xF200112AU, 0xF200112CU, NULL };
        SvcCmdIKIdspDumpInfo7[ 53] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001120U, 0xF200112EU, 0xF2001130U, 0xF2001132U, NULL };
        SvcCmdIKIdspDumpInfo7[ 54] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001138U, 0xF2001148U, 0xF200114AU, 0xF200114CU, NULL };
        SvcCmdIKIdspDumpInfo7[ 55] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001140U, 0xF200114EU, 0xF2001150U, 0xF2001152U, NULL };
        SvcCmdIKIdspDumpInfo7[ 56] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001158U, 0xF2001168U, 0xF200116AU, 0xF200116CU, NULL };
        SvcCmdIKIdspDumpInfo7[ 57] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001160U, 0xF200116EU, 0xF2001170U, 0xF2001172U, NULL };
        SvcCmdIKIdspDumpInfo7[ 58] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001178U, 0xF2001188U, 0xF200118AU, 0xF200118CU, NULL };
        SvcCmdIKIdspDumpInfo7[ 59] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001180U, 0xF200118EU, 0xF2001190U, 0xF2001192U, NULL };
        SvcCmdIKIdspDumpInfo7[ 60] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001198U, 0xF20011A8U, 0xF20011AAU, 0xF20011ACU, NULL };
        SvcCmdIKIdspDumpInfo7[ 61] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40011A0U, 0xF20011AEU, 0xF20011B0U, 0xF20011B2U, NULL };
        SvcCmdIKIdspDumpInfo7[ 62] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40011B8U, 0xF20011C8U, 0xF20011CAU, 0xF20011CCU, NULL };
        SvcCmdIKIdspDumpInfo7[ 63] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40011C0U, 0xF20011CEU, 0xF20011D0U, 0xF20011D2U, NULL };
        SvcCmdIKIdspDumpInfo7[ 64] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40011D8U, 0xF20011E8U, 0xF20011EAU, 0xF20011ECU, NULL };
        SvcCmdIKIdspDumpInfo7[ 65] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40011E0U, 0xF20011EEU, 0xF20011F0U, 0xF20011F2U, NULL };
        SvcCmdIKIdspDumpInfo7[ 66] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001230U, 0xF4001238U, 0xF4001238U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo7[ 67] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001240U, 0xF4001248U, 0xF4001248U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo7[ 68] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001250U, 0xF4001258U, 0xF4001258U, 0xF0000001U, SvcCmdIk_IdspCfg5Update };
        SvcCmdIKIdspDumpInfo7[ 69] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001260U, 0xF4001268U, 0xF4001268U, 0xF0000001U, SvcCmdIk_IdspCfg6Update };
        SvcCmdIKIdspDumpInfo7[ 70] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001270U, 0xF4001278U, 0xF4001278U, 0xF0000001U, SvcCmdIk_IdspCfg7Update };
        SvcCmdIKIdspDumpInfo7[ 71] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001280U, 0xF4001288U, 0xF4001288U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo7[ 72] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001290U, 0xF4001298U, 0xF4001298U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo7[ 73] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40012A0U, 0xF40012A8U, 0xF40012A8U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo7[ 74] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40012B0U, 0xF20012C0U, 0xF20012C2U, 0xF20012C4U, NULL };
        SvcCmdIKIdspDumpInfo7[ 75] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40012B8U, 0xF20012C6U, 0xF20012C8U, 0xF20012CAU, NULL };
        SvcCmdIKIdspDumpInfo7[ 76] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40012D0U, 0xF20012E0U, 0xF20012E2U, 0xF20012E4U, NULL };
        SvcCmdIKIdspDumpInfo7[ 77] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40012D8U, 0xF20012E6U, 0xF20012E8U, 0xF20012EAU, NULL };
        SvcCmdIKIdspDumpInfo7[ 78] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40012F0U, 0xF2001300U, 0xF2001302U, 0xF2001304U, NULL };
        SvcCmdIKIdspDumpInfo7[ 79] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40012F8U, 0xF2001306U, 0xF2001308U, 0xF200130AU, NULL };
        SvcCmdIKIdspDumpInfo7[ 80] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001310U, 0xF2001320U, 0xF2001322U, 0xF2001324U, NULL };
        SvcCmdIKIdspDumpInfo7[ 81] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001318U, 0xF2001326U, 0xF2001328U, 0xF200132AU, NULL };
        SvcCmdIKIdspDumpInfo7[ 82] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001330U, 0xF2001340U, 0xF2001342U, 0xF2001344U, NULL };
        SvcCmdIKIdspDumpInfo7[ 83] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001338U, 0xF2001346U, 0xF2001348U, 0xF200134AU, NULL };
        SvcCmdIKIdspDumpInfo7[ 84] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001350U, 0xF2001360U, 0xF2001362U, 0xF2001364U, NULL };
        SvcCmdIKIdspDumpInfo7[ 85] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001358U, 0xF2001366U, 0xF2001368U, 0xF200136AU, NULL };
        SvcCmdIKIdspDumpInfo7[ 86] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001370U, 0xF2001380U, 0xF2001382U, 0xF2001384U, NULL };
        SvcCmdIKIdspDumpInfo7[ 87] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001378U, 0xF2001386U, 0xF2001388U, 0xF200138AU, NULL };
        SvcCmdIKIdspDumpInfo7[ 88] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40013C8U, 0xF40013D0U, 0xF40013D0U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo7[ 89] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40013D8U, 0xF40013E0U, 0xF40013E0U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo7[ 90] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40013E8U, 0xF40013F0U, 0xF40013F0U, 0xF0000001U, SvcCmdIk_IdspCfg5Update };
        SvcCmdIKIdspDumpInfo7[ 91] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40013F8U, 0xF4001400U, 0xF4001400U, 0xF0000001U, SvcCmdIk_IdspCfg6Update };
        SvcCmdIKIdspDumpInfo7[ 92] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001408U, 0xF4001410U, 0xF4001410U, 0xF0000001U, SvcCmdIk_IdspCfg7Update };
        SvcCmdIKIdspDumpInfo7[ 93] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001418U, 0xF4001420U, 0xF4001420U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo7[ 94] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001428U, 0xF4001430U, 0xF4001430U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo7[ 95] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001438U, 0xF4001440U, 0xF4001440U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo7[ 96] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001448U, 0xF2001458U, 0xF200145AU, 0xF200145CU, NULL };
        SvcCmdIKIdspDumpInfo7[ 97] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001450U, 0xF200145EU, 0xF2001460U, 0xF2001462U, NULL };
        SvcCmdIKIdspDumpInfo7[ 98] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001468U, 0xF2001478U, 0xF200147AU, 0xF200147CU, NULL };
        SvcCmdIKIdspDumpInfo7[ 99] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001470U, 0xF200147EU, 0xF2001480U, 0xF2001482U, NULL };
        SvcCmdIKIdspDumpInfo7[100] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001488U, 0xF2001498U, 0xF200149AU, 0xF200149CU, NULL };
        SvcCmdIKIdspDumpInfo7[101] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001490U, 0xF200149EU, 0xF20014A0U, 0xF20014A2U, NULL };
        SvcCmdIKIdspDumpInfo7[102] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40014A8U, 0xF20014B8U, 0xF20014BAU, 0xF20014BCU, NULL };
        SvcCmdIKIdspDumpInfo7[103] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40014B0U, 0xF20014BEU, 0xF20014C0U, 0xF20014C2U, NULL };
        SvcCmdIKIdspDumpInfo7[104] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40014C8U, 0xF20014D8U, 0xF20014DAU, 0xF20014DCU, NULL };
        SvcCmdIKIdspDumpInfo7[105] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40014D0U, 0xF20014DEU, 0xF20014E0U, 0xF20014E2U, NULL };
        SvcCmdIKIdspDumpInfo7[106] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40014E8U, 0xF20014F8U, 0xF20014FAU, 0xF20014FCU, NULL };
        SvcCmdIKIdspDumpInfo7[107] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40014F0U, 0xF20014FEU, 0xF2001500U, 0xF2001502U, NULL };
        SvcCmdIKIdspDumpInfo7[108] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001508U, 0xF2001518U, 0xF200151AU, 0xF200151CU, NULL };
        SvcCmdIKIdspDumpInfo7[109] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001510U, 0xF200151EU, 0xF2001520U, 0xF2001522U, NULL };
        SvcCmdIKIdspDumpInfo7[110] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001560U, 0xF4001568U, 0xF4001568U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo7[111] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001570U, 0xF4001578U, 0xF4001578U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo7[112] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001580U, 0xF4001588U, 0xF4001588U, 0xF0000001U, SvcCmdIk_IdspCfg5Update };
        SvcCmdIKIdspDumpInfo7[113] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001590U, 0xF4001598U, 0xF4001598U, 0xF0000001U, SvcCmdIk_IdspCfg6Update };
        SvcCmdIKIdspDumpInfo7[114] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40015A0U, 0xF40015A8U, 0xF40015A8U, 0xF0000001U, SvcCmdIk_IdspCfg7Update };
        SvcCmdIKIdspDumpInfo7[115] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40015B0U, 0xF40015B8U, 0xF40015B8U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo7[116] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40015C0U, 0xF40015C8U, 0xF40015C8U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo7[117] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40015D0U, 0xF40015D8U, 0xF40015D8U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo7[118] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40015E0U, 0xF20015F0U, 0xF20015F2U, 0xF20015F4U, NULL };
        SvcCmdIKIdspDumpInfo7[119] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40015E8U, 0xF20015F6U, 0xF20015F8U, 0xF20015FAU, NULL };
        SvcCmdIKIdspDumpInfo7[120] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001600U, 0xF2001610U, 0xF2001612U, 0xF2001614U, NULL };
        SvcCmdIKIdspDumpInfo7[121] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001608U, 0xF2001616U, 0xF2001618U, 0xF200161AU, NULL };
        SvcCmdIKIdspDumpInfo7[122] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001620U, 0xF2001630U, 0xF2001632U, 0xF2001634U, NULL };
        SvcCmdIKIdspDumpInfo7[123] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001628U, 0xF2001636U, 0xF2001638U, 0xF200163AU, NULL };
        SvcCmdIKIdspDumpInfo7[124] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001640U, 0xF2001650U, 0xF2001652U, 0xF2001654U, NULL };
        SvcCmdIKIdspDumpInfo7[125] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001648U, 0xF2001656U, 0xF2001658U, 0xF200165AU, NULL };
        SvcCmdIKIdspDumpInfo7[126] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001660U, 0xF2001670U, 0xF2001672U, 0xF2001674U, NULL };
        SvcCmdIKIdspDumpInfo7[127] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001668U, 0xF2001676U, 0xF2001678U, 0xF200167AU, NULL };
        SvcCmdIKIdspDumpInfo7[128] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001680U, 0xF2001690U, 0xF2001692U, 0xF2001694U, NULL };
        SvcCmdIKIdspDumpInfo7[129] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001688U, 0xF2001696U, 0xF2001698U, 0xF200169AU, NULL };
        SvcCmdIKIdspDumpInfo7[130] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40016A0U, 0xF20016B0U, 0xF20016B2U, 0xF20016B4U, NULL };
        SvcCmdIKIdspDumpInfo7[131] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40016A8U, 0xF20016B6U, 0xF20016B8U, 0xF20016BAU, NULL };
        SvcCmdIKIdspDumpInfo7[132] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40016F8U, 0xF4001700U, 0xF4001700U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo7[133] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001708U, 0xF4001710U, 0xF4001710U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo7[134] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001718U, 0xF4001720U, 0xF4001720U, 0xF0000001U, SvcCmdIk_IdspCfg5Update };
        SvcCmdIKIdspDumpInfo7[135] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001728U, 0xF4001730U, 0xF4001730U, 0xF0000001U, SvcCmdIk_IdspCfg6Update };
        SvcCmdIKIdspDumpInfo7[136] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001738U, 0xF4001740U, 0xF4001740U, 0xF0000001U, SvcCmdIk_IdspCfg7Update };
        SvcCmdIKIdspDumpInfo7[137] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001748U, 0xF4001750U, 0xF4001750U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo7[138] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001758U, 0xF4001760U, 0xF4001760U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo7[139] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001768U, 0xF4001770U, 0xF4001770U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo7[140] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001778U, 0xF2001788U, 0xF200178AU, 0xF200178CU, NULL };
        SvcCmdIKIdspDumpInfo7[141] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001780U, 0xF200178EU, 0xF2001790U, 0xF2001792U, NULL };
        SvcCmdIKIdspDumpInfo7[142] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001798U, 0xF20017A8U, 0xF20017AAU, 0xF20017ACU, NULL };
        SvcCmdIKIdspDumpInfo7[143] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40017A0U, 0xF20017AEU, 0xF20017B0U, 0xF20017B2U, NULL };
        SvcCmdIKIdspDumpInfo7[144] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40017B8U, 0xF20017C8U, 0xF20017CAU, 0xF20017CCU, NULL };
        SvcCmdIKIdspDumpInfo7[145] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40017C0U, 0xF20017CEU, 0xF20017D0U, 0xF20017D2U, NULL };
        SvcCmdIKIdspDumpInfo7[146] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40017D8U, 0xF20017E8U, 0xF20017EAU, 0xF20017ECU, NULL };
        SvcCmdIKIdspDumpInfo7[147] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40017E0U, 0xF20017EEU, 0xF20017F0U, 0xF20017F2U, NULL };
        SvcCmdIKIdspDumpInfo7[148] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40017F8U, 0xF2001808U, 0xF200180AU, 0xF200180CU, NULL };
        SvcCmdIKIdspDumpInfo7[149] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001800U, 0xF200180EU, 0xF2001810U, 0xF2001812U, NULL };
        SvcCmdIKIdspDumpInfo7[150] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001818U, 0xF2001828U, 0xF200182AU, 0xF200182CU, NULL };
        SvcCmdIKIdspDumpInfo7[151] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001820U, 0xF200182EU, 0xF2001830U, 0xF2001832U, NULL };
        SvcCmdIKIdspDumpInfo7[152] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001838U, 0xF2001848U, 0xF200184AU, 0xF200184CU, NULL };
        SvcCmdIKIdspDumpInfo7[153] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001840U, 0xF200184EU, 0xF2001850U, 0xF2001852U, NULL };
        SvcCmdIKIdspDumpInfo7[154] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001890U, 0xF4001898U, 0xF4001898U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo7[155] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40018A0U, 0xF40018A8U, 0xF40018A8U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo7[156] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40018B0U, 0xF40018B8U, 0xF40018B8U, 0xF0000001U, SvcCmdIk_IdspCfg5Update };
        SvcCmdIKIdspDumpInfo7[157] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40018C0U, 0xF40018C8U, 0xF40018C8U, 0xF0000001U, SvcCmdIk_IdspCfg6Update };
        SvcCmdIKIdspDumpInfo7[158] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40018D0U, 0xF40018D8U, 0xF40018D8U, 0xF0000001U, SvcCmdIk_IdspCfg7Update };
        SvcCmdIKIdspDumpInfo7[159] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40018E0U, 0xF40018E8U, 0xF40018E8U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo7[160] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40018F0U, 0xF40018F8U, 0xF40018F8U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo7[161] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001900U, 0xF4001908U, 0xF4001908U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo7[162] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001910U, 0xF2001920U, 0xF2001922U, 0xF2001924U, NULL };
        SvcCmdIKIdspDumpInfo7[163] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001918U, 0xF2001926U, 0xF2001928U, 0xF200192AU, NULL };
        SvcCmdIKIdspDumpInfo7[164] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001930U, 0xF2001940U, 0xF2001942U, 0xF2001944U, NULL };
        SvcCmdIKIdspDumpInfo7[165] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001938U, 0xF2001946U, 0xF2001948U, 0xF200194AU, NULL };
        SvcCmdIKIdspDumpInfo7[166] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001950U, 0xF2001960U, 0xF2001962U, 0xF2001964U, NULL };
        SvcCmdIKIdspDumpInfo7[167] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001958U, 0xF2001966U, 0xF2001968U, 0xF200196AU, NULL };
        SvcCmdIKIdspDumpInfo7[168] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001970U, 0xF2001980U, 0xF2001982U, 0xF2001984U, NULL };
        SvcCmdIKIdspDumpInfo7[169] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001978U, 0xF2001986U, 0xF2001988U, 0xF200198AU, NULL };
        SvcCmdIKIdspDumpInfo7[170] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001990U, 0xF20019A0U, 0xF20019A2U, 0xF20019A4U, NULL };
        SvcCmdIKIdspDumpInfo7[171] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001998U, 0xF20019A6U, 0xF20019A8U, 0xF20019AAU, NULL };
        SvcCmdIKIdspDumpInfo7[172] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40019B0U, 0xF20019C0U, 0xF20019C2U, 0xF20019C4U, NULL };
        SvcCmdIKIdspDumpInfo7[173] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40019B8U, 0xF20019C6U, 0xF20019C8U, 0xF20019CAU, NULL };
        SvcCmdIKIdspDumpInfo7[174] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40019D0U, 0xF20019E0U, 0xF20019E2U, 0xF20019E4U, NULL };
        SvcCmdIKIdspDumpInfo7[175] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40019D8U, 0xF20019E6U, 0xF20019E8U, 0xF20019EAU, NULL };
        SvcCmdIKIdspDumpInfo7[176] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001A28U, 0xF4001A30U, 0xF4001A30U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo7[177] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001A38U, 0xF4001A40U, 0xF4001A40U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo7[178] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001A48U, 0xF4001A50U, 0xF4001A50U, 0xF0000001U, SvcCmdIk_IdspCfg5Update };
        SvcCmdIKIdspDumpInfo7[179] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001A58U, 0xF4001A60U, 0xF4001A60U, 0xF0000001U, SvcCmdIk_IdspCfg6Update };
        SvcCmdIKIdspDumpInfo7[180] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001A68U, 0xF4001A70U, 0xF4001A70U, 0xF0000001U, SvcCmdIk_IdspCfg7Update };
        SvcCmdIKIdspDumpInfo7[181] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001A78U, 0xF4001A80U, 0xF4001A80U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo7[182] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001A88U, 0xF4001A90U, 0xF4001A90U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo7[183] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001A98U, 0xF4001AA0U, 0xF4001AA0U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo7[184] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001AA8U, 0xF2001AB8U, 0xF2001ABAU, 0xF2001ABCU, NULL };
        SvcCmdIKIdspDumpInfo7[185] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001AB0U, 0xF2001ABEU, 0xF2001AC0U, 0xF2001AC2U, NULL };
        SvcCmdIKIdspDumpInfo7[186] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001AC8U, 0xF2001AD8U, 0xF2001ADAU, 0xF2001ADCU, NULL };
        SvcCmdIKIdspDumpInfo7[187] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001AD0U, 0xF2001ADEU, 0xF2001AE0U, 0xF2001AE2U, NULL };
        SvcCmdIKIdspDumpInfo7[188] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001AE8U, 0xF2001AF8U, 0xF2001AFAU, 0xF2001AFCU, NULL };
        SvcCmdIKIdspDumpInfo7[189] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001AF0U, 0xF2001AFEU, 0xF2001B00U, 0xF2001B02U, NULL };
        SvcCmdIKIdspDumpInfo7[190] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001B08U, 0xF2001B18U, 0xF2001B1AU, 0xF2001B1CU, NULL };
        SvcCmdIKIdspDumpInfo7[191] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001B10U, 0xF2001B1EU, 0xF2001B20U, 0xF2001B22U, NULL };
        SvcCmdIKIdspDumpInfo7[192] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001B28U, 0xF2001B38U, 0xF2001B3AU, 0xF2001B3CU, NULL };
        SvcCmdIKIdspDumpInfo7[193] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001B30U, 0xF2001B3EU, 0xF2001B40U, 0xF2001B42U, NULL };
        SvcCmdIKIdspDumpInfo7[194] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001B48U, 0xF2001B58U, 0xF2001B5AU, 0xF2001B5CU, NULL };
        SvcCmdIKIdspDumpInfo7[195] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001B50U, 0xF2001B5EU, 0xF2001B60U, 0xF2001B62U, NULL };
        SvcCmdIKIdspDumpInfo7[196] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001B68U, 0xF2001B78U, 0xF2001B7AU, 0xF2001B7CU, NULL };
        SvcCmdIKIdspDumpInfo7[197] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001B70U, 0xF2001B7EU, 0xF2001B80U, 0xF2001B82U, NULL };
        SvcCmdIKIdspDumpInfo7[198] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001BC0U, 0xF4001BC8U, 0xF4001BC8U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo7[199] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001BD0U, 0xF4001BD8U, 0xF4001BD8U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo7[200] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001BE0U, 0xF4001BE8U, 0xF4001BE8U, 0xF0000001U, SvcCmdIk_IdspCfg5Update };
        SvcCmdIKIdspDumpInfo7[201] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001BF0U, 0xF4001BF8U, 0xF4001BF8U, 0xF0000001U, SvcCmdIk_IdspCfg6Update };
        SvcCmdIKIdspDumpInfo7[202] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001C00U, 0xF4001C08U, 0xF4001C08U, 0xF0000001U, SvcCmdIk_IdspCfg7Update };
        SvcCmdIKIdspDumpInfo7[203] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001C10U, 0xF4001C18U, 0xF4001C18U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo7[204] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001C20U, 0xF4001C28U, 0xF4001C28U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo7[205] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001C30U, 0xF4001C38U, 0xF4001C38U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo7[206] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001C40U, 0xF2001C50U, 0xF2001C52U, 0xF2001C54U, NULL };
        SvcCmdIKIdspDumpInfo7[207] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001C48U, 0xF2001C56U, 0xF2001C58U, 0xF2001C5AU, NULL };
        SvcCmdIKIdspDumpInfo7[208] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001C60U, 0xF2001C70U, 0xF2001C72U, 0xF2001C74U, NULL };
        SvcCmdIKIdspDumpInfo7[209] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001C68U, 0xF2001C76U, 0xF2001C78U, 0xF2001C7AU, NULL };
        SvcCmdIKIdspDumpInfo7[210] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001C80U, 0xF2001C90U, 0xF2001C92U, 0xF2001C94U, NULL };
        SvcCmdIKIdspDumpInfo7[211] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001C88U, 0xF2001C96U, 0xF2001C98U, 0xF2001C9AU, NULL };
        SvcCmdIKIdspDumpInfo7[212] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001CA0U, 0xF2001CB0U, 0xF2001CB2U, 0xF2001CB4U, NULL };
        SvcCmdIKIdspDumpInfo7[213] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001CA8U, 0xF2001CB6U, 0xF2001CB8U, 0xF2001CBAU, NULL };
        SvcCmdIKIdspDumpInfo7[214] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001CC0U, 0xF2001CD0U, 0xF2001CD2U, 0xF2001CD4U, NULL };
        SvcCmdIKIdspDumpInfo7[215] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001CC8U, 0xF2001CD6U, 0xF2001CD8U, 0xF2001CDAU, NULL };
        SvcCmdIKIdspDumpInfo7[216] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001CE0U, 0xF2001CF0U, 0xF2001CF2U, 0xF2001CF4U, NULL };
        SvcCmdIKIdspDumpInfo7[217] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001CE8U, 0xF2001CF6U, 0xF2001CF8U, 0xF2001CFAU, NULL };
        SvcCmdIKIdspDumpInfo7[218] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001D00U, 0xF2001D10U, 0xF2001D12U, 0xF2001D14U, NULL };
        SvcCmdIKIdspDumpInfo7[219] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001D08U, 0xF2001D16U, 0xF2001D18U, 0xF2001D1AU, NULL };
        SvcCmdIKIdspDumpInfo7[220] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001D58U, 0xF4001D60U, 0xF4001D60U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo7[221] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001D68U, 0xF4001D70U, 0xF4001D70U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo7[222] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001D78U, 0xF4001D80U, 0xF4001D80U, 0xF0000001U, SvcCmdIk_IdspCfg5Update };
        SvcCmdIKIdspDumpInfo7[223] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001D88U, 0xF4001D90U, 0xF4001D90U, 0xF0000001U, SvcCmdIk_IdspCfg6Update };
        SvcCmdIKIdspDumpInfo7[224] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001D98U, 0xF4001DA0U, 0xF4001DA0U, 0xF0000001U, SvcCmdIk_IdspCfg7Update };
        SvcCmdIKIdspDumpInfo7[225] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001DA8U, 0xF4001DB0U, 0xF4001DB0U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo7[226] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001DB8U, 0xF4001DC0U, 0xF4001DC0U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo7[227] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001DC8U, 0xF4001DD0U, 0xF4001DD0U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo7[228] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001DD8U, 0xF2001DE8U, 0xF2001DEAU, 0xF2001DECU, NULL };
        SvcCmdIKIdspDumpInfo7[229] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001DE0U, 0xF2001DEEU, 0xF2001DF0U, 0xF2001DF2U, NULL };
        SvcCmdIKIdspDumpInfo7[230] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001DF8U, 0xF2001E08U, 0xF2001E0AU, 0xF2001E0CU, NULL };
        SvcCmdIKIdspDumpInfo7[231] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001E00U, 0xF2001E0EU, 0xF2001E10U, 0xF2001E12U, NULL };
        SvcCmdIKIdspDumpInfo7[232] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001E18U, 0xF2001E28U, 0xF2001E2AU, 0xF2001E2CU, NULL };
        SvcCmdIKIdspDumpInfo7[233] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001E20U, 0xF2001E2EU, 0xF2001E30U, 0xF2001E32U, NULL };
        SvcCmdIKIdspDumpInfo7[234] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001E38U, 0xF2001E48U, 0xF2001E4AU, 0xF2001E4CU, NULL };
        SvcCmdIKIdspDumpInfo7[235] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001E40U, 0xF2001E4EU, 0xF2001E50U, 0xF2001E52U, NULL };
        SvcCmdIKIdspDumpInfo7[236] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001E58U, 0xF2001E68U, 0xF2001E6AU, 0xF2001E6CU, NULL };
        SvcCmdIKIdspDumpInfo7[237] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001E60U, 0xF2001E6EU, 0xF2001E70U, 0xF2001E72U, NULL };
        SvcCmdIKIdspDumpInfo7[238] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001E78U, 0xF2001E88U, 0xF2001E8AU, 0xF2001E8CU, NULL };
        SvcCmdIKIdspDumpInfo7[239] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001E80U, 0xF2001E8EU, 0xF2001E90U, 0xF2001E92U, NULL };
        SvcCmdIKIdspDumpInfo7[240] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001E98U, 0xF2001EA8U, 0xF2001EAAU, 0xF2001EACU, NULL };
        SvcCmdIKIdspDumpInfo7[241] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001EA0U, 0xF2001EAEU, 0xF2001EB0U, 0xF2001EB2U, NULL };
        SvcCmdIKIdspDumpInfo7[242] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001EF0U, 0xF4001EF8U, 0xF4001EF8U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo7[243] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001F00U, 0xF4001F08U, 0xF4001F08U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo7[244] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001F10U, 0xF4001F18U, 0xF4001F18U, 0xF0000001U, SvcCmdIk_IdspCfg5Update };
        SvcCmdIKIdspDumpInfo7[245] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001F20U, 0xF4001F28U, 0xF4001F28U, 0xF0000001U, SvcCmdIk_IdspCfg6Update };
        SvcCmdIKIdspDumpInfo7[246] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001F30U, 0xF4001F38U, 0xF4001F38U, 0xF0000001U, SvcCmdIk_IdspCfg7Update };
        SvcCmdIKIdspDumpInfo7[247] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001F40U, 0xF4001F48U, 0xF4001F48U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo7[248] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001F50U, 0xF4001F58U, 0xF4001F58U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo7[249] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001F60U, 0xF4001F68U, 0xF4001F68U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo7[250] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001F70U, 0xF2001F80U, 0xF2001F82U, 0xF2001F84U, NULL };
        SvcCmdIKIdspDumpInfo7[251] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001F78U, 0xF2001F86U, 0xF2001F88U, 0xF2001F8AU, NULL };
        SvcCmdIKIdspDumpInfo7[252] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001F90U, 0xF2001FA0U, 0xF2001FA2U, 0xF2001FA4U, NULL };
        SvcCmdIKIdspDumpInfo7[253] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001F98U, 0xF2001FA6U, 0xF2001FA8U, 0xF2001FAAU, NULL };
        SvcCmdIKIdspDumpInfo7[254] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001FB0U, 0xF2001FC0U, 0xF2001FC2U, 0xF2001FC4U, NULL };
        SvcCmdIKIdspDumpInfo7[255] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001FB8U, 0xF2001FC6U, 0xF2001FC8U, 0xF2001FCAU, NULL };
        SvcCmdIKIdspDumpInfo7[256] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001FD0U, 0xF2001FE0U, 0xF2001FE2U, 0xF2001FE4U, NULL };
        SvcCmdIKIdspDumpInfo7[257] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001FD8U, 0xF2001FE6U, 0xF2001FE8U, 0xF2001FEAU, NULL };
        SvcCmdIKIdspDumpInfo7[258] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001FF0U, 0xF2002000U, 0xF2002002U, 0xF2002004U, NULL };
        SvcCmdIKIdspDumpInfo7[259] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4001FF8U, 0xF2002006U, 0xF2002008U, 0xF200200AU, NULL };
        SvcCmdIKIdspDumpInfo7[260] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002010U, 0xF2002020U, 0xF2002022U, 0xF2002024U, NULL };
        SvcCmdIKIdspDumpInfo7[261] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002018U, 0xF2002026U, 0xF2002028U, 0xF200202AU, NULL };
        SvcCmdIKIdspDumpInfo7[262] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002030U, 0xF2002040U, 0xF2002042U, 0xF2002044U, NULL };
        SvcCmdIKIdspDumpInfo7[263] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002038U, 0xF2002046U, 0xF2002048U, 0xF200204AU, NULL };
        SvcCmdIKIdspDumpInfo7[264] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002088U, 0xF4002090U, 0xF4002090U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo7[265] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002098U, 0xF40020A0U, 0xF40020A0U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo7[266] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40020A8U, 0xF40020B0U, 0xF40020B0U, 0xF0000001U, SvcCmdIk_IdspCfg5Update };
        SvcCmdIKIdspDumpInfo7[267] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40020B8U, 0xF40020C0U, 0xF40020C0U, 0xF0000001U, SvcCmdIk_IdspCfg6Update };
        SvcCmdIKIdspDumpInfo7[268] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40020C8U, 0xF40020D0U, 0xF40020D0U, 0xF0000001U, SvcCmdIk_IdspCfg7Update };
        SvcCmdIKIdspDumpInfo7[269] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40020D8U, 0xF40020E0U, 0xF40020E0U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo7[270] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40020E8U, 0xF40020F0U, 0xF40020F0U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo7[271] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40020F8U, 0xF4002100U, 0xF4002100U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo7[272] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002108U, 0xF2002118U, 0xF200211AU, 0xF200211CU, NULL };
        SvcCmdIKIdspDumpInfo7[273] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002110U, 0xF200211EU, 0xF2002120U, 0xF2002122U, NULL };
        SvcCmdIKIdspDumpInfo7[274] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002128U, 0xF2002138U, 0xF200213AU, 0xF200213CU, NULL };
        SvcCmdIKIdspDumpInfo7[275] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002130U, 0xF200213EU, 0xF2002140U, 0xF2002142U, NULL };
        SvcCmdIKIdspDumpInfo7[276] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002148U, 0xF2002158U, 0xF200215AU, 0xF200215CU, NULL };
        SvcCmdIKIdspDumpInfo7[277] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002150U, 0xF200215EU, 0xF2002160U, 0xF2002162U, NULL };
        SvcCmdIKIdspDumpInfo7[278] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002168U, 0xF2002178U, 0xF200217AU, 0xF200217CU, NULL };
        SvcCmdIKIdspDumpInfo7[279] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002170U, 0xF200217EU, 0xF2002180U, 0xF2002182U, NULL };
        SvcCmdIKIdspDumpInfo7[280] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002188U, 0xF2002198U, 0xF200219AU, 0xF200219CU, NULL };
        SvcCmdIKIdspDumpInfo7[281] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002190U, 0xF200219EU, 0xF20021A0U, 0xF20021A2U, NULL };
        SvcCmdIKIdspDumpInfo7[282] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40021A8U, 0xF20021B8U, 0xF20021BAU, 0xF20021BCU, NULL };
        SvcCmdIKIdspDumpInfo7[283] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40021B0U, 0xF20021BEU, 0xF20021C0U, 0xF20021C2U, NULL };
        SvcCmdIKIdspDumpInfo7[284] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40021C8U, 0xF20021D8U, 0xF20021DAU, 0xF20021DCU, NULL };
        SvcCmdIKIdspDumpInfo7[285] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40021D0U, 0xF20021DEU, 0xF20021E0U, 0xF20021E2U, NULL };
        SvcCmdIKIdspDumpInfo7[286] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002220U, 0xF4002228U, 0xF4002228U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo7[287] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002230U, 0xF4002238U, 0xF4002238U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo7[288] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002240U, 0xF4002248U, 0xF4002248U, 0xF0000001U, SvcCmdIk_IdspCfg5Update };
        SvcCmdIKIdspDumpInfo7[289] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002250U, 0xF4002258U, 0xF4002258U, 0xF0000001U, SvcCmdIk_IdspCfg6Update };
        SvcCmdIKIdspDumpInfo7[290] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002260U, 0xF4002268U, 0xF4002268U, 0xF0000001U, SvcCmdIk_IdspCfg7Update };
        SvcCmdIKIdspDumpInfo7[291] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002270U, 0xF4002278U, 0xF4002278U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo7[292] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002280U, 0xF4002288U, 0xF4002288U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo7[293] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002290U, 0xF4002298U, 0xF4002298U, 0xF0000001U, NULL };
        SvcCmdIKIdspDumpInfo7[294] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40022A0U, 0xF20022B0U, 0xF20022B2U, 0xF20022B4U, NULL };
        SvcCmdIKIdspDumpInfo7[295] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40022A8U, 0xF20022B6U, 0xF20022B8U, 0xF20022BAU, NULL };
        SvcCmdIKIdspDumpInfo7[296] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40022C0U, 0xF20022D0U, 0xF20022D2U, 0xF20022D4U, NULL };
        SvcCmdIKIdspDumpInfo7[297] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40022C8U, 0xF20022D6U, 0xF20022D8U, 0xF20022DAU, NULL };
        SvcCmdIKIdspDumpInfo7[298] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40022E0U, 0xF20022F0U, 0xF20022F2U, 0xF20022F4U, NULL };
        SvcCmdIKIdspDumpInfo7[299] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40022E8U, 0xF20022F6U, 0xF20022F8U, 0xF20022FAU, NULL };
        SvcCmdIKIdspDumpInfo7[300] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002300U, 0xF2002310U, 0xF2002312U, 0xF2002314U, NULL };
        SvcCmdIKIdspDumpInfo7[301] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002308U, 0xF2002316U, 0xF2002318U, 0xF200231AU, NULL };
        SvcCmdIKIdspDumpInfo7[302] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002320U, 0xF2002330U, 0xF2002332U, 0xF2002334U, NULL };
        SvcCmdIKIdspDumpInfo7[303] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002328U, 0xF2002336U, 0xF2002338U, 0xF200233AU, NULL };
        SvcCmdIKIdspDumpInfo7[304] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002340U, 0xF2002350U, 0xF2002352U, 0xF2002354U, NULL };
        SvcCmdIKIdspDumpInfo7[305] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002348U, 0xF2002356U, 0xF2002358U, 0xF200235AU, NULL };
        SvcCmdIKIdspDumpInfo7[306] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002360U, 0xF2002370U, 0xF2002372U, 0xF2002374U, NULL };
        SvcCmdIKIdspDumpInfo7[307] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002368U, 0xF2002376U, 0xF2002378U, 0xF200237AU, NULL };

        AmbaSvcWrap_MisraMemset(SvcCmdIKIdspDumpInfo8, 0, sizeof(SvcCmdIKIdspDumpInfo8));
        SvcCmdIKIdspDumpInfo8[  0] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000760U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[  1] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000770U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[  2] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000780U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[  3] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000790U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[  4] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40007A0U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[  5] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40007B0U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[  6] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40007C0U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[  7] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40007D0U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[  8] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40007E0U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[  9] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40007F0U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 10] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000800U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 11] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000810U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 12] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000820U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 13] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000830U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 14] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000840U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 15] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000850U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 16] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000860U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 17] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000870U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 18] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000880U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 19] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000890U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 20] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40008A0U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 21] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40008B0U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 22] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40008C0U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 23] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40008D0U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 24] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40008E0U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 25] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40008F0U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 26] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000900U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 27] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000910U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 28] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000920U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 29] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000930U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 30] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000940U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 31] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000950U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 32] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000960U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 33] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000970U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 34] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000980U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 35] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000990U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 36] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40009A0U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 37] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40009B0U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 38] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40009C0U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 39] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40009D0U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 40] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40009E0U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 41] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40009F0U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 42] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000A00U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 43] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000A10U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 44] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000A20U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 45] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000A30U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 46] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000A40U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 47] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000A50U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 48] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000A60U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 49] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000A70U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 50] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000A80U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 51] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000A90U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 52] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000AA0U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 53] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000AB0U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 54] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000AC0U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 55] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000AD0U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 56] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000AE0U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 57] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000AF0U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 58] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000B00U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 59] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000B10U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 60] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000B20U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 61] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000B30U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 62] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000B40U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 63] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000B50U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 64] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000B60U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 65] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000B70U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 66] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000B80U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 67] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000B90U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 68] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000BA0U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 69] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000BB0U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 70] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000BC0U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 71] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000BD0U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 72] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000BE0U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 73] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000BF0U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 74] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000C00U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 75] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000C10U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 76] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000C20U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 77] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000C30U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 78] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000C40U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 79] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000C50U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 80] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000C60U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 81] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000C70U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 82] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000C80U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 83] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000C90U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 84] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000CA0U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 85] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000CB0U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 86] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000CC0U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 87] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000CD0U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 88] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000CE0U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 89] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000CF0U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 90] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000D00U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 91] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000D10U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 92] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000D20U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 93] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000D30U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 94] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000D40U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 95] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4000D50U, 0xF0013100U, 0xF0013100U, 0xF0000001U, SvcCmdIk_IdspCfg2Update };
        SvcCmdIKIdspDumpInfo8[ 96] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40023B0U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[ 97] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40023C0U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[ 98] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40023D0U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[ 99] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40023E0U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[100] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40023F0U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[101] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002400U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[102] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002410U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[103] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002420U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[104] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002430U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[105] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002440U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[106] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002450U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[107] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002460U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[108] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002470U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[109] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002480U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[110] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002490U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[111] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40024A0U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[112] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40024B0U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[113] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40024C0U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[114] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40024D0U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[115] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40024E0U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[116] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40024F0U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[117] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002500U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[118] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002510U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[119] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002520U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[120] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002530U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[121] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002540U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[122] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002550U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[123] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002560U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[124] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002570U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[125] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002580U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[126] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002590U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[127] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40025A0U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[128] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40025B0U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[129] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40025C0U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[130] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40025D0U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[131] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40025E0U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[132] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40025F0U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[133] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002600U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[134] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002610U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[135] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002620U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[136] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002630U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[137] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002640U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[138] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002650U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[139] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002660U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[140] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002670U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[141] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002680U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[142] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002690U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[143] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40026A0U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[144] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40026B0U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[145] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40026C0U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[146] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40026D0U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[147] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40026E0U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[148] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40026F0U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[149] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002700U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[150] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002710U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[151] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002720U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[152] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002730U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[153] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002740U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[154] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002750U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[155] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002760U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[156] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002770U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[157] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002780U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[158] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002790U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[159] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40027A0U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[160] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40027B0U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[161] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40027C0U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[162] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40027D0U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[163] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40027E0U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[164] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40027F0U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[165] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002800U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[166] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002810U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[167] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002820U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[168] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002830U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[169] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002840U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[170] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002850U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[171] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002860U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[172] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002870U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[173] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002880U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[174] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002890U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[175] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40028A0U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[176] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40028B0U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[177] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40028C0U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[178] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40028D0U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[179] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40028E0U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[180] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40028F0U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[181] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002900U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[182] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002910U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[183] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002920U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[184] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002930U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[185] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002940U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[186] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002950U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[187] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002960U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[188] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002970U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[189] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002980U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[190] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002990U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[191] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40029A0U, 0xF0003100U, 0xF0003100U, 0xF0000001U, SvcCmdIk_IdspCfg4Update };
        SvcCmdIKIdspDumpInfo8[192] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40029B0U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[193] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40029C0U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[194] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40029D0U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[195] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40029E0U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[196] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40029F0U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[197] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002A00U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[198] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002A10U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[199] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002A20U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[200] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002A30U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[201] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002A40U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[202] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002A50U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[203] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002A60U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[204] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002A70U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[205] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002A80U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[206] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002A90U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[207] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002AA0U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[208] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002AB0U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[209] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002AC0U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[210] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002AD0U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[211] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002AE0U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[212] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002AF0U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[213] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002B00U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[214] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002B10U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[215] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002B20U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[216] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002B30U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[217] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002B40U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[218] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002B50U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[219] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002B60U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[220] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002B70U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[221] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002B80U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[222] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002B90U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[223] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002BA0U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[224] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002BB0U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[225] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002BC0U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[226] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002BD0U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[227] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002BE0U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[228] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002BF0U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[229] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002C00U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[230] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002C10U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[231] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002C20U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[232] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002C30U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[233] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002C40U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[234] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002C50U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[235] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002C60U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[236] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002C70U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[237] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002C80U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[238] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002C90U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[239] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002CA0U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[240] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002CB0U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[241] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002CC0U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[242] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002CD0U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[243] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002CE0U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[244] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002CF0U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[245] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002D00U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[246] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002D10U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[247] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002D20U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[248] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002D30U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[249] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002D40U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[250] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002D50U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[251] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002D60U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[252] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002D70U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[253] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002D80U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[254] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002D90U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[255] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002DA0U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[256] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002DB0U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[257] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002DC0U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[258] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002DD0U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[259] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002DE0U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[260] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002DF0U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[261] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002E00U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[262] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002E10U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[263] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002E20U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[264] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002E30U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[265] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002E40U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[266] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002E50U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[267] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002E60U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[268] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002E70U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[269] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002E80U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[270] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002E90U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[271] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002EA0U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[272] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002EB0U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[273] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002EC0U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[274] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002ED0U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[275] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002EE0U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[276] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002EF0U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[277] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002F00U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[278] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002F10U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[279] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002F20U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[280] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002F30U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[281] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002F40U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[282] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002F50U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[283] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002F60U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[284] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002F70U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[285] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002F80U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[286] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002F90U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[287] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002FA0U, 0xF0008280U, 0xF0008280U, 0xF0000001U, SvcCmdIk_IdspCfg3Update };
        SvcCmdIKIdspDumpInfo8[288] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002FB0U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[289] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002FC0U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[290] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002FD0U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[291] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002FE0U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[292] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4002FF0U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[293] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4003000U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[294] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4003010U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[295] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4003020U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[296] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4003030U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[297] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4003040U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[298] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4003050U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[299] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4003060U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[300] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4003070U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[301] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4003080U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[302] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4003090U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[303] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40030A0U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[304] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40030B0U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[305] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40030C0U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[306] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40030D0U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[307] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40030E0U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[308] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40030F0U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[309] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4003100U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[310] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4003110U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[311] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4003120U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[312] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4003130U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[313] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4003140U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[314] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4003150U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[315] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4003160U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[316] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4003170U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[317] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4003180U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[318] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4003190U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[319] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40031A0U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[320] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40031B0U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[321] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40031C0U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[322] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40031D0U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[323] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40031E0U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[324] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40031F0U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[325] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4003200U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[326] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4003210U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[327] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4003220U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[328] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4003230U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[329] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4003240U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[330] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4003250U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[331] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4003260U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[332] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4003270U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[333] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4003280U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[334] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4003290U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[335] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40032A0U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[336] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40032B0U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[337] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40032C0U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[338] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40032D0U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[339] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40032E0U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[340] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40032F0U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[341] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4003300U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[342] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4003310U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[343] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4003320U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[344] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4003330U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[345] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4003340U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[346] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4003350U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[347] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4003360U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[348] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4003370U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[349] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4003380U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[350] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4003390U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[351] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40033A0U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[352] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40033B0U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[353] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40033C0U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[354] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40033D0U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[355] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40033E0U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[356] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40033F0U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[357] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4003400U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[358] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4003410U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[359] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4003420U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[360] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4003430U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[361] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4003440U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[362] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4003450U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[363] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4003460U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[364] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4003470U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[365] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4003480U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[366] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4003490U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[367] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40034A0U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[368] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40034B0U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[369] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40034C0U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[370] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40034D0U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[371] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40034E0U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[372] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40034F0U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[373] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4003500U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[374] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4003510U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[375] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4003520U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[376] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4003530U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[377] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4003540U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[378] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4003550U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[379] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4003560U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[380] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4003570U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[381] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4003580U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[382] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF4003590U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };
        SvcCmdIKIdspDumpInfo8[383] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40035A0U, 0xF0003980U, 0xF0003980U, 0xF0000001U, SvcCmdIk_IdspCfg18Update };

        AmbaSvcWrap_MisraMemset(SvcCmdIKIdspDumpInfo9, 0, sizeof(SvcCmdIKIdspDumpInfo9));
        SvcCmdIKIdspDumpInfo9[  0] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40035B0U, 0xF20035B8U, 0xF20035BAU, 0xF20035BCU, NULL };
        SvcCmdIKIdspDumpInfo9[  1] = (SVC_CMD_IK_IDSP_DUMP_INFO_s) { 0xF40035E0U, 0xF20035E8U, 0xF20035EAU, 0xF20035ECU, NULL };

        AmbaSvcWrap_MisraMemset(&SvcCmdIKIdspDumpFunc, 0, sizeof(SvcCmdIKIdspDumpFunc));
        SvcCmdIKIdspDumpFunc[0U] = (SVC_CMD_IK_IDSP_DUMP_FUNC_s) { SVC_CMD_IK_IDSP_DUMP_LVL0, NULL, SVC_CMD_IK_IDSP_DUMP_LVL0_NUM, SvcCmdIKIdspDumpInfo0 };
        SvcCmdIKIdspDumpFunc[1U] = (SVC_CMD_IK_IDSP_DUMP_FUNC_s) { SVC_CMD_IK_IDSP_DUMP_LVL1, NULL, SVC_CMD_IK_IDSP_DUMP_LVL1_NUM, SvcCmdIKIdspDumpInfo1 };
        SvcCmdIKIdspDumpFunc[2U] = (SVC_CMD_IK_IDSP_DUMP_FUNC_s) { SVC_CMD_IK_IDSP_DUMP_LVL2, NULL, SVC_CMD_IK_IDSP_DUMP_LVL2_NUM, SvcCmdIKIdspDumpInfo2 };
        SvcCmdIKIdspDumpFunc[3U] = (SVC_CMD_IK_IDSP_DUMP_FUNC_s) { SVC_CMD_IK_IDSP_DUMP_LVL3, NULL, SVC_CMD_IK_IDSP_DUMP_LVL3_NUM, SvcCmdIKIdspDumpInfo3 };
        SvcCmdIKIdspDumpFunc[4U] = (SVC_CMD_IK_IDSP_DUMP_FUNC_s) { SVC_CMD_IK_IDSP_DUMP_LVL4, NULL, SVC_CMD_IK_IDSP_DUMP_LVL4_NUM, SvcCmdIKIdspDumpInfo4 };
        SvcCmdIKIdspDumpFunc[5U] = (SVC_CMD_IK_IDSP_DUMP_FUNC_s) { SVC_CMD_IK_IDSP_DUMP_LVL5, NULL, SVC_CMD_IK_IDSP_DUMP_LVL5_NUM, SvcCmdIKIdspDumpInfo5 };
        SvcCmdIKIdspDumpFunc[6U] = (SVC_CMD_IK_IDSP_DUMP_FUNC_s) { SVC_CMD_IK_IDSP_DUMP_LVL6, NULL, SVC_CMD_IK_IDSP_DUMP_LVL6_NUM, SvcCmdIKIdspDumpInfo6 };
        SvcCmdIKIdspDumpFunc[7U] = (SVC_CMD_IK_IDSP_DUMP_FUNC_s) { SVC_CMD_IK_IDSP_DUMP_LVL7, NULL, SVC_CMD_IK_IDSP_DUMP_LVL7_NUM, SvcCmdIKIdspDumpInfo7 };
        SvcCmdIKIdspDumpFunc[8U] = (SVC_CMD_IK_IDSP_DUMP_FUNC_s) { SVC_CMD_IK_IDSP_DUMP_LVL8, NULL, SVC_CMD_IK_IDSP_DUMP_LVL8_NUM, SvcCmdIKIdspDumpInfo8 };
        SvcCmdIKIdspDumpFunc[9U] = (SVC_CMD_IK_IDSP_DUMP_FUNC_s) { SVC_CMD_IK_IDSP_DUMP_LVL9, NULL, SVC_CMD_IK_IDSP_DUMP_LVL9_NUM, SvcCmdIKIdspDumpInfo9 };

        SvcCmdIKCmdDumpInit = 1U;
    }

    if (pDumpFuncNum != NULL) {
        *pDumpFuncNum = SVC_CMD_IK_IDSP_DUMP_PROC_NUM;
    }

    if (pDumFunc != NULL) {
        *pDumFunc = SvcCmdIKIdspDumpFunc;
    }

    return SVC_OK;
}

/**
 * Idsp dump function shell cmd usage
 *
 */
void SvcCmdIK_DumpIdspUsage(void)
{
    SVC_WRAP_PRINT "    %sidsp%s            : dump idsp data from dsp"
        SVC_PRN_ARG_S SVC_CMD_IK
        SVC_PRN_ARG_CSTR "\033""[38;2;100;255;255m" SVC_PRN_ARG_POST
        SVC_PRN_ARG_CSTR "\033""[0m"                SVC_PRN_ARG_POST
        SVC_PRN_ARG_E
    SVC_WRAP_PRINT "      [output path] : configure the output folder or prefix name" SVC_PRN_ARG_S SVC_CMD_IK SVC_PRN_ARG_E
    SVC_WRAP_PRINT "      [dump bits]   : default is 0" SVC_PRN_ARG_S SVC_CMD_IK SVC_PRN_ARG_E
}

/**
 * Get idsp config
 *
 * @param [in] ArgCount argument counter
 * @param [in] pArgVector argument vector
 * @param [in] pCfg idsp config buffer
 * @return ErrCode SVC_OK(0)/SVC_NG(1)
 */
UINT32 SvcCmdIK_GetIdspCfg(UINT32 DumpLevel, UINT32 CtrlFlg, SVC_CMD_IK_IDSP_CFG *pCfg)
{
    UINT32 RetVal = SVC_OK, PRetVal;
    const AMBA_IK_MODE_CFG_s *pImgMode = NULL;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();

    if (pCfg == NULL) {
        RetVal = SVC_NG;
        SVC_WRAP_PRINT "Fail to get idsp cfg - invalid cfg!" SVC_PRN_ARG_S SVC_CMD_IK SVC_PRN_ARG_PROC SvcLog_NG SVC_PRN_ARG_E
    } else {
        AmbaMisra_TypeCast(&(pImgMode), &(pCfg->pMode));
        if (pImgMode == NULL) {
            RetVal = SVC_NG;
            SVC_WRAP_PRINT "Fail to get idsp cfg - invalid image mode!" SVC_PRN_ARG_S SVC_CMD_IK SVC_PRN_ARG_PROC SvcLog_NG SVC_PRN_ARG_E
        } else if (pCfg->pBuf == NULL) {
            RetVal = SVC_NG;
            SVC_WRAP_PRINT "Fail to get idsp cfg - invalid buffer base. %p"
                SVC_PRN_ARG_S SVC_CMD_IK
                SVC_PRN_ARG_PROC SvcLog_NG
                SVC_PRN_ARG_CPOINT pCfg->pBuf SVC_PRN_ARG_POST
                SVC_PRN_ARG_E
        } else if (pCfg->BufSize == 0U) {
            RetVal = SVC_NG;
            SVC_WRAP_PRINT "Fail to get idsp cfg - buffer size should not zero!" SVC_PRN_ARG_S SVC_CMD_IK SVC_PRN_ARG_PROC SvcLog_NG SVC_PRN_ARG_E
        } else if (pCfg->BufSize < (SVC_CMD_IK_IDSP_CFG_SIZE + 4U)) {
            RetVal = SVC_NG;
            SVC_WRAP_PRINT "Fail to get idsp cfg - buffer size is too small to service it!" SVC_PRN_ARG_S SVC_CMD_IK SVC_PRN_ARG_PROC SvcLog_NG SVC_PRN_ARG_E
        } else if (pCfg->pCfgAddr == NULL) {
            RetVal = SVC_NG;
            SVC_WRAP_PRINT "Fail to get idsp cfg - output config offset should not null" SVC_PRN_ARG_S SVC_CMD_IK SVC_PRN_ARG_PROC SvcLog_NG SVC_PRN_ARG_E
        } else {
            typedef UINT32 (*SVC_FP_GET_IDSP_CFG)(UINT16 ViewZoneId, ULONG *CfgAddr);
            extern UINT32 AmbaDSP_LiveviewGetIDspCfg(UINT16 ViewZoneId, ULONG *CfgAddr) GNU_WEAK_SYMBOL;
            SVC_FP_GET_IDSP_CFG pFpGetIdspCfg = AmbaDSP_LiveviewGetIDspCfg;

            UINT8 *pBufHeader    = pCfg->pBuf;
            UINT32 BufHeaderSize = 4U;
            UINT8 *pCfgData      = &(pBufHeader[BufHeaderSize]);
            UINT32 CfgDataSize   = SVC_CMD_IK_IDSP_CFG_SIZE;
            UINT32 MaxDelayTime  = 500U;

            if (pResCfg != NULL) {
                UINT32 VinIdx;
                UINT32 CurFrameSync, MaxFrameSync = 0U;

                for (VinIdx = 0U; VinIdx < pResCfg->VinNum; VinIdx ++) {
                    CurFrameSync  = pResCfg->VinCfg[VinIdx].FrameRate.NumUnitsInTick * 1000U;
                    CurFrameSync /= pResCfg->VinCfg[VinIdx].FrameRate.TimeScale;
                    CurFrameSync += 1U;

                    if (CurFrameSync > MaxFrameSync) {
                        MaxFrameSync = CurFrameSync;
                    }
                }

                MaxDelayTime = MaxFrameSync << 1U;
            }

            if (pBufHeader == NULL) {
                RetVal = SVC_NG;
                SVC_WRAP_PRINT "Fail to get idsp cfg - invalid dump buffer" SVC_PRN_ARG_S SVC_CMD_IK SVC_PRN_ARG_PROC SvcLog_NG SVC_PRN_ARG_E
            } else {
                pBufHeader[0] = (UINT8) DumpLevel;
                pBufHeader[1] = (UINT8)(DumpLevel >> 8U);
                pBufHeader[2] = (UINT8)(DumpLevel >> 16U);
                pBufHeader[3] = (UINT8)(DumpLevel >> 24U);
                if (pCfg->pCfgAddr != NULL) {
                    *(pCfg->pCfgAddr) = pCfgData;
                }

                if (pFpGetIdspCfg == NULL) {
                    RetVal = SVC_NG;
                    SVC_WRAP_PRINT "Fail to get idsp cfg - not support to dump idsp cfg" SVC_PRN_ARG_S SVC_CMD_IK SVC_PRN_ARG_PROC SvcLog_NG SVC_PRN_ARG_E
                } else {
                    typedef UINT32 (*SVC_FP_DSP_STOP)(UINT32 CmdType);
                    extern UINT32 AmbaDSP_Stop(UINT32 CmdType) GNU_WEAK_SYMBOL;
                    SVC_FP_DSP_STOP pFpDSPStop = AmbaDSP_Stop;
                    ULONG IdspCfgAddr = 0U;
                    UINT16 ViewZoneId  = (UINT16)(pImgMode->ContextId);
                    INT32 MaxGetIdspTimes = 5;

                    if ((CtrlFlg & SVC_CMD_IK_SAVE_IDSP_CFG_STILL_CAP) == 0U) {
                        if (pFpDSPStop != NULL) {
                            SVC_WRAP_PRINT "Stop dsp" SVC_PRN_ARG_S SVC_CMD_IK SVC_PRN_ARG_E
                            PRetVal = (pFpDSPStop)(1U); PRN_CMD_IK_ERR_HDLR
                        }

                        do {
                            if (pFpDSPStop != NULL) {
                                SVC_WRAP_PRINT "Delay %d ms before get idsp cfg!"
                                    SVC_PRN_ARG_S SVC_CMD_IK
                                    SVC_PRN_ARG_UINT32 MaxDelayTime SVC_PRN_ARG_POST
                                    SVC_PRN_ARG_E
                                PRetVal = AmbaKAL_TaskSleep(MaxDelayTime); PRN_CMD_IK_ERR_HDLR
                            }

                            SVC_WRAP_PRINT "Try to get ViewZoneID(%d) idsp cfg!"
                                SVC_PRN_ARG_S SVC_CMD_IK
                                SVC_PRN_ARG_UINT32 pImgMode->ContextId SVC_PRN_ARG_POST
                            SVC_PRN_ARG_E
                            PRetVal = (pFpGetIdspCfg)(ViewZoneId, &IdspCfgAddr); PRN_CMD_IK_ERR_HDLR
                            SVC_WRAP_PRINT "  Get idsp cfg 0x%016llx" SVC_PRN_ARG_S SVC_CMD_IK SVC_PRN_ARG_UINT64 IdspCfgAddr SVC_PRN_ARG_E

                            MaxGetIdspTimes --;
                        } while ((MaxGetIdspTimes > 0) && ((PRetVal != 0U) || (IdspCfgAddr == 0U)));
                    } else {
                        SVC_WRAP_PRINT "Try to get ViewZoneID(%d) idsp cfg!"
                            SVC_PRN_ARG_S SVC_CMD_IK
                            SVC_PRN_ARG_UINT32 pImgMode->ContextId SVC_PRN_ARG_POST
                        SVC_PRN_ARG_E
                        PRetVal = (pFpGetIdspCfg)(ViewZoneId, &IdspCfgAddr); PRN_CMD_IK_ERR_HDLR
                        SVC_WRAP_PRINT "  Get idsp cfg 0x%016llx" SVC_PRN_ARG_S SVC_CMD_IK SVC_PRN_ARG_UINT64 IdspCfgAddr SVC_PRN_ARG_E
                    }

                    if (PRetVal != 0U) {
                        RetVal = SVC_NG;
                        SVC_WRAP_PRINT "Fail to get idsp cfg - get IK contex id(%d) idsp cfg fail. RetVal: 0x%x"
                            SVC_PRN_ARG_S SVC_CMD_IK
                            SVC_PRN_ARG_PROC SvcLog_NG
                            SVC_PRN_ARG_UINT32 pImgMode->ContextId SVC_PRN_ARG_POST
                            SVC_PRN_ARG_UINT32 PRetVal             SVC_PRN_ARG_POST
                            SVC_PRN_ARG_E
                    } else {
                        if (IdspCfgAddr == 0U) {
                            RetVal = SVC_NG;
                            SVC_WRAP_PRINT "Fail to get idsp cfg - invalid idsp cfg addr!" SVC_PRN_ARG_S SVC_CMD_IK SVC_PRN_ARG_PROC SvcLog_NG SVC_PRN_ARG_E
                        } else {
                            void *pIdspCfg = NULL;

                            AmbaMisra_TypeCast(&(pIdspCfg), &(IdspCfgAddr));

                            AmbaSvcWrap_MisraMemset(pCfgData, 0, CfgDataSize);

                            SvcCmdIK_MemCopyEx(pCfgData, pIdspCfg, CfgDataSize);

                            AmbaSvcWrap_MisraMemset(SvcCmdIkMemBlkMap, 0, sizeof(SvcCmdIkMemBlkMap));

                            SvcCmdIK_MemCopyEx(SvcCmdIkMemBlkMap, &(pCfgData[0x35C0]), (UINT32)sizeof(SvcCmdIkMemBlkMap));
                        }
                    }

                    AmbaMisra_TouchUnused(SvcCmdIKIdspDumpFunc);
                    AmbaMisra_TouchUnused(SvcCmdIKIdspDumpInfo0);
                    AmbaMisra_TouchUnused(SvcCmdIKIdspDumpInfo1);
                    AmbaMisra_TouchUnused(SvcCmdIKIdspDumpInfo2);
                    AmbaMisra_TouchUnused(SvcCmdIKIdspDumpInfo3);
                    AmbaMisra_TouchUnused(SvcCmdIKIdspDumpInfo4);
                    AmbaMisra_TouchUnused(SvcCmdIKIdspDumpInfo5);
                    AmbaMisra_TouchUnused(SvcCmdIKIdspDumpInfo6);
                    AmbaMisra_TouchUnused(SvcCmdIKIdspDumpInfo7);
                    AmbaMisra_TouchUnused(SvcCmdIKIdspDumpInfo8);
                    AmbaMisra_TouchUnused(SvcCmdIKIdspDumpInfo9);

                    (void) SvcCmdIk_IdspCfg1Update(NULL, NULL);
                    (void) SvcCmdIk_IdspCfg5Update(NULL, NULL);
                    (void) SvcCmdIk_IdspCfg6Update(NULL, NULL);
                    (void) SvcCmdIk_IdspCfg7Update(NULL, NULL);
                    (void) SvcCmdIk_IdspCfg18Update(NULL, NULL);

                    AmbaMisra_TouchUnused(pCfg);
                }
            }
        }
    }

    return RetVal;
}

static UINT32 SvcCmdIk_IdspCfg1Update(void *pCfgBuf, void *pBuf)  { return SvcCmdIK_ConfigUpdate( 1U, pCfgBuf, pBuf); }
static UINT32 SvcCmdIk_IdspCfg2Update(void *pCfgBuf, void *pBuf)  { return SvcCmdIK_ConfigUpdate( 2U, pCfgBuf, pBuf); }
static UINT32 SvcCmdIk_IdspCfg3Update(void *pCfgBuf, void *pBuf)  { return SvcCmdIK_ConfigUpdate( 3U, pCfgBuf, pBuf); }
static UINT32 SvcCmdIk_IdspCfg4Update(void *pCfgBuf, void *pBuf)  { return SvcCmdIK_ConfigUpdate( 4U, pCfgBuf, pBuf); }
static UINT32 SvcCmdIk_IdspCfg5Update(void *pCfgBuf, void *pBuf)  { return SvcCmdIK_ConfigUpdate( 5U, pCfgBuf, pBuf); }
static UINT32 SvcCmdIk_IdspCfg6Update(void *pCfgBuf, void *pBuf)  { return SvcCmdIK_ConfigUpdate( 6U, pCfgBuf, pBuf); }
static UINT32 SvcCmdIk_IdspCfg7Update(void *pCfgBuf, void *pBuf)  { return SvcCmdIK_ConfigUpdate( 7U, pCfgBuf, pBuf); }
static UINT32 SvcCmdIk_IdspCfg18Update(void *pCfgBuf, void *pBuf) { return SvcCmdIK_ConfigUpdate(18U, pCfgBuf, pBuf); }

static UINT32 SvcCmdIK_ConfigUpdate(UINT32 CfgID, void *pCfgBuf, void *pUserBuf)
{
    UINT32 Rval = SVC_OK;

    if ((pCfgBuf != NULL) && (pUserBuf != NULL)) {

        UINT32 Offset[9] = {80, 52, 48, 96, 60, 52, 52, 48, 52};
        UINT32 Offset2[38] = { 512, 640,  128,   128,  128,  128, 8192, 384,  768, 896,
                               1920, 128,  384,  3072, 3072, 3072, 3072, 384,  128, 256,
                               256, 128, 4864, 16384,  256,  384, 1280, 128,  128, 768,
                               24576, 768,  256,   256,  128,  128,  128, 128};
        UINT32 Offset3[3] = {8448, 24576, 128};
        UINT32 Offset4[8] = {512, 640, 128, 8192, 384, 256, 256, 1920};
        const UINT32 Offset11[4] = {768, 24576, 128, 128};
        UINT32 Offset18[11] = {256, 1792, 256, 768, 4096, 256, 1920, 768, 4096, 128, 128};
        UINT32 Offset5[12] = {128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128};
        UINT32 Offset6[8] = {128, 128, 128, 128, 128, 128, 128, 128};
        UINT32 Offset7[12] = {128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128};
        UINT32 Offset1[4] = {128, 128, 768, 128};

        UINT32 *p_cr = NULL;
        UINT8 *pBase, *pU8;
        void *p_tmp;
        UINT32 i;
        const UINT32 *pCfgID;
        UINT32 CurCfgID;

        AmbaMisra_TypeCast(&(pCfgID), &(pCfgBuf));
        if (pCfgID != NULL) {
            ++pCfgID;

            CurCfgID = (*pCfgID) & 0x1FU;

            SVC_WRAP_PRINT "Current CfgID(%d) should same with Requested CfgID(%d)"
                SVC_PRN_ARG_S SVC_CMD_IK
                SVC_PRN_ARG_UINT32 CurCfgID SVC_PRN_ARG_POST
                SVC_PRN_ARG_UINT32 CfgID    SVC_PRN_ARG_POST
                SVC_PRN_ARG_E
            if (CfgID != CurCfgID) {
                Rval = SVC_NG;
                SVC_WRAP_PRINT "Fail to update cfg - CfgID mismatch!" SVC_PRN_ARG_S SVC_CMD_IK SVC_PRN_ARG_PROC SvcLog_NG SVC_PRN_ARG_E
            } else {
                SvcCmdIK_MemCopyEx(pUserBuf, pCfgBuf, 256U);

                AmbaMisra_TypeCast(&pBase, &pUserBuf);
                pBase = &(pBase[256U]);

                switch (CfgID) {
                case 2U:

                    AmbaMisra_TypeCast(&pU8, &pUserBuf);
                    pU8 = &(pU8[Offset[0]]);
                    AmbaMisra_TypeCast(&p_cr, &pU8);
                    for(i=0; i<32U; i++) {
                        p_tmp = SvcCmdIK_PhysToVirt(((*p_cr) >> 7u) << 7u);
                        SvcCmdIK_MemCopyEx(pBase, p_tmp, Offset2[i]);
                        ++p_cr;
                        pBase = &(pBase[Offset2[i]]);
                    }

                    p_tmp = SvcCmdIK_PhysToVirt(((*p_cr) >> 7u) << 7u);
                    pU8 = &(pBase[Offset2[32] + Offset2[33] + Offset2[34] + Offset2[35]]);
                    SvcCmdIK_MemCopyEx(pU8, p_tmp, Offset2[36]);
                    ++p_cr;

                    p_tmp = SvcCmdIK_PhysToVirt(((*p_cr) >> 7u) << 7u);
                    SvcCmdIK_MemCopyEx(pBase, p_tmp, Offset2[32]);
                    ++p_cr;

                    p_tmp = SvcCmdIK_PhysToVirt(((*p_cr) >> 7u) << 7u);
                    pU8 = &(pBase[Offset2[32]]);
                    SvcCmdIK_MemCopyEx(pU8, p_tmp, Offset2[33]);
                    ++p_cr;

                    p_tmp = SvcCmdIK_PhysToVirt(((*p_cr) >> 7u) << 7u);
                    pU8 = &(pBase[Offset2[32] + Offset2[33]]);
                    SvcCmdIK_MemCopyEx(pU8, p_tmp, Offset2[34]);
                    ++p_cr;

                    p_tmp = SvcCmdIK_PhysToVirt(((*p_cr) >> 7u) << 7u);
                    pU8 = &(pBase[Offset2[32] + Offset2[33] + Offset2[34]]);
                    SvcCmdIK_MemCopyEx(pU8, p_tmp, Offset2[35]);
                    ++p_cr;

                    p_tmp = SvcCmdIK_PhysToVirt(((*p_cr) >> 7u) << 7u);
                    pU8 = &(pBase[Offset2[32] + Offset2[33] + Offset2[34] + Offset2[35] + Offset2[36]]);
                    SvcCmdIK_MemCopyEx(pU8, p_tmp, Offset2[37]);
                    ++p_cr;

                    break;

                case 3U:
                    AmbaMisra_TypeCast(&pU8, &pUserBuf);
                    pU8 = &(pU8[Offset[1]]);
                    AmbaMisra_TypeCast(&p_cr, &pU8);
                    for(i=0; i<3U; i++) {
                        p_tmp = SvcCmdIK_PhysToVirt(((*p_cr) >> 7u) << 7u);
                        SvcCmdIK_MemCopyEx(pBase, p_tmp, Offset3[i]);
                        ++p_cr;
                        pBase = &(pBase[Offset3[i]]);
                    }
                    break;

                case 4U:
                    AmbaMisra_TypeCast(&pU8, &pUserBuf);
                    pU8 = &(pU8[Offset[2]]);
                    AmbaMisra_TypeCast(&p_cr, &pU8);
                    for(i=0; i<8U; i++) {
                        p_tmp = SvcCmdIK_PhysToVirt(((*p_cr) >> 7u) << 7u);
                        SvcCmdIK_MemCopyEx(pBase, p_tmp, Offset4[i]);
                        ++p_cr;
                        pBase = &(pBase[Offset4[i]]);
                    }

                    break;
                case 18U:
                    AmbaMisra_TypeCast(&pU8, &pUserBuf);
                    pU8 = &(pU8[Offset[3]]);
                    AmbaMisra_TypeCast(&p_cr, &pU8);
                    for(i=0; i<11U; i++) {
                        p_tmp = SvcCmdIK_PhysToVirt(((*p_cr) >> 7u) << 7u);
                        SvcCmdIK_MemCopyEx(pBase, p_tmp, Offset18[i]);
                        ++p_cr;
                        pBase = &(pBase[Offset18[i]]);
                    }

                    break;
                case 5U:
                    AmbaMisra_TypeCast(&pU8, &pUserBuf);
                    pU8 = &(pU8[Offset[4]]);
                    AmbaMisra_TypeCast(&p_cr, &pU8);
                    for(i=0; i<12U; i++) {
                        p_tmp = SvcCmdIK_PhysToVirt(((*p_cr) >> 7u) << 7u);
                        SvcCmdIK_MemCopyEx(pBase, p_tmp, Offset5[i]);
                        ++p_cr;
                        pBase = &(pBase[Offset5[i]]);
                    }

                    break;
                case 6U:
                    AmbaMisra_TypeCast(&pU8, &pUserBuf);
                    pU8 = &(pU8[Offset[5]]);
                    AmbaMisra_TypeCast(&p_cr, &pU8);
                    for(i=0; i<8U; i++) {
                        p_tmp = SvcCmdIK_PhysToVirt(((*p_cr) >> 7u) << 7u);
                        SvcCmdIK_MemCopyEx(pBase, p_tmp, Offset6[i]);
                        ++p_cr;
                        pBase = &(pBase[Offset6[i]]);
                    }

                    break;
                case 7U:
                    AmbaMisra_TypeCast(&pU8, &pUserBuf);
                    pU8 = &(pU8[Offset[6]]);
                    AmbaMisra_TypeCast(&p_cr, &pU8);
                    for(i=0; i<12U; i++) {
                        p_tmp = SvcCmdIK_PhysToVirt(((*p_cr) >> 7u) << 7u);
                        SvcCmdIK_MemCopyEx(pBase, p_tmp, Offset7[i]);
                        ++p_cr;
                        pBase = &(pBase[Offset7[i]]);
                    }

                    break;
                case 1U:
                    AmbaMisra_TypeCast(&pU8, &pUserBuf);
                    pU8 = &(pU8[Offset[7]]);
                    AmbaMisra_TypeCast(&p_cr, &pU8);
                    for(i=0; i<4U; i++) {
                        p_tmp = SvcCmdIK_PhysToVirt(((*p_cr) >> 7u) << 7u);
                        SvcCmdIK_MemCopyEx(pBase, p_tmp, Offset1[i]);
                        ++p_cr;
                        pBase = &(pBase[Offset1[i]]);
                    }

                    break;
                case 11U:
                    AmbaMisra_TypeCast(&pU8, &pUserBuf);
                    pU8 = &(pU8[Offset[8]]);
                    AmbaMisra_TypeCast(&p_cr, &pU8);
                    for(i=0; i<4U; i++) {
                        p_tmp = SvcCmdIK_PhysToVirt(((*p_cr) >> 7u) << 7u);
                        SvcCmdIK_MemCopyEx(pBase, p_tmp, Offset11[i]);
                        ++p_cr;
                        pBase = &(pBase[Offset11[i]]);
                    }

                    break;
                default:
                    Rval = SVC_NG;
                    SVC_WRAP_PRINT "Fail to update cfg - does not support CfgID(%d)!"
                        SVC_PRN_ARG_S SVC_CMD_IK
                        SVC_PRN_ARG_UINT32 CfgID SVC_PRN_ARG_POST
                        SVC_PRN_ARG_E
                    break;
                }
            }

            AmbaMisra_TouchUnused(pCfgBuf);
            AmbaMisra_TouchUnused(pUserBuf);
        }

        AmbaMisra_TouchUnused(Offset);
        AmbaMisra_TouchUnused(Offset1);
        AmbaMisra_TouchUnused(Offset2);
        AmbaMisra_TouchUnused(Offset3);
        AmbaMisra_TouchUnused(Offset4);
        AmbaMisra_TouchUnused(Offset5);
        AmbaMisra_TouchUnused(Offset6);
        AmbaMisra_TouchUnused(Offset7);
        AmbaMisra_TouchUnused(Offset18);
        AmbaMisra_TouchUnused(p_cr);
    }

    return Rval;
}


