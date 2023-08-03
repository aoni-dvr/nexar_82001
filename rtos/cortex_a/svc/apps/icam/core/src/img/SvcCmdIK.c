/**
 *  @file SvcCmdIK.c
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
 *  @details svc application 'img' shell command
 *
 */

#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaMisraFix.h"
#include "AmbaWrap.h"
#include "AmbaUtility.h"
#include "AmbaMMU.h"
#include "AmbaShell.h"
#include "AmbaCache.h"
#include "AmbaFS.h"
#include "bsp.h"
#include "AmbaSvcWrap.h"

#include "AmbaDSP.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaDSP_ImageUtility.h"
#if defined(CONFIG_ICAM_IMGITUNER_USED)
#include "AmbaTUNE_HdlrManager.h"
#endif
#include "AmbaVIN_Def.h"

#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcCmd.h"
#include "SvcMem.h"
#include "SvcPlat.h"

#include "SvcResCfg.h"
#ifdef CONFIG_BUILD_IMGFRW_AAA
#include "SvcImg.h"
#endif
#include "SvcBuffer.h"
#include "inc/SvcCmdIK.h"

#define SVC_CMD_IK_ITUNER_MEM (0x0U)
#define SVC_CMD_IK_DUMP_MEM   (0x1U)
#define SVC_CMD_IK_MEM_NUM    (0x2U)
typedef struct {
    UINT32  Enable;
    UINT8  *pBuf;
    UINT32  BufSize;
} SVC_CMD_IK_MEM_CTRL_s;

typedef struct {
    AMBA_IK_MODE_CFG_s           ImgMode;
    SVC_CMD_IK_MEM_CTRL_s        MemCtrl[SVC_CMD_IK_MEM_NUM];
    UINT32                       NumOfIdspDumpFunc;
    SVC_CMD_IK_IDSP_DUMP_FUNC_s *pIdspDumpFunc;
} SVC_CMD_IK_CTRL_s;

typedef UINT32 (*SVC_CMD_IK_PROC_f)(UINT32 ArgCount, char * const *pArgVector);
typedef void   (*SVC_CMD_IK_USAGE_f)(void);
typedef struct {
    UINT32 Enable;
    char   Name[32];
    SVC_CMD_IK_PROC_f  pFunc;
    SVC_CMD_IK_USAGE_f pUsage;
} SVC_CMD_IK_FUNC_s;

#define SVC_CMD_IK_IDSP_HEADER_SIZE (16U)
#define SVC_CMD_IK_IDSP_DUMP_SIZE   (0xDBBA00U) // 14400000U

static void   SvcCmdIK_ErrHdlr(const char *pCaller, UINT32 CodeLine, UINT32 ErrCode);
static void   SvcCmdIK_MemAllocate(UINT32 ReqMemSize, UINT32 *pMemType, UINT8 **pReqBuf, UINT32 *pBufSize);
static void   SvcCmdIK_MemRelease(UINT32 MemType);
static UINT32 SvcCmdIK_InvalidCache(const UINT8 *pDataBuf, UINT32 DataSize);
static UINT32 SvcCmdIK_PhysToVirt(UINT8 *pPhys, UINT8 **pVirt);

static UINT32 SvcCmdIK_CmdDumpFolderCreate(const char *pPath);
static UINT32 SvcCmdIK_CmdDumpBinSave(UINT8 *pBuf, UINT32 BufSize, const char *pFileName);
static UINT8 *SvcCmdIK_CmdDumpIdspGetAddr(UINT8 *pBase, UINT32 Offset);
static UINT32 SvcCmdIK_CmdDumpIdspGetVal(UINT8 *pBase, UINT32 Offset);
static UINT32 SvcCmdIK_CmdDumpIdspBinCpy(UINT8 *pSrc, UINT8 *pDst, UINT32 DstBufSize, UINT32 Pitch, UINT32 Width, UINT32 Height);
static void   SvcCmdIK_CmdDumpIdspProc(UINT8 *pCfgBuf, UINT8 *pDumpBuf, UINT32 DumpBufSize, const void *pDumpFuncInfo, char *pPrefixStr);
static UINT32 SvcCmdIK_CmdDumpIdsp(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCmdIK_CmdDumpIdspU(void);
static UINT32 SvcCmdIK_CmdDumpIso(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCmdIK_CmdDumpIsoU(void);
static UINT32 SvcCmdIK_CmdDumpMem(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCmdIK_CmdDumpMemU(void);
static UINT32 SvcCmdIK_CmdIkMode(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCmdIK_CmdIkModeU(void);
#if defined(CONFIG_ICAM_IMGITUNER_USED)
static UINT32 SvcCmdIK_CmdItuner(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCmdIK_CmdItunerU(void);
#endif
#if defined(CONFIG_FWPROG_R52SYS_ENABLE)
static UINT32 SvcCmdIK_CmdSafety(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCmdIK_CmdSafetyU(void);
#endif
static UINT32 SvcCmdIK_CmdDump(UINT32 ArgCount, char * const *pArgVector);
static void   SvcCmdIK_CmdDumpU(void);
static void   SvcCmdIK_CmdEntryInit(void);
static void   SvcCmdIK_CmdEntry(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
static void   SvcCmdIK_CmdUsage(void);
static void   SvcCmdIK_CmdStrToPointer(const char *pStr, UINT8 **pPointer);
static void   SvcCmdIK_CmdStrToU32(const char *pString, UINT32 *pValue);

UINT32 SvcCmdIK_IsCacheMemory(void *pBuf);

static SVC_CMD_IK_CTRL_s SvcCmdIKCtrl GNU_SECTION_NOZEROINIT;

#define SVC_CMD_IK_SHELL_NUM (4U)
static SVC_CMD_IK_FUNC_s SvcCmdIKFunc[SVC_CMD_IK_SHELL_NUM] GNU_SECTION_NOZEROINIT;

#define SVC_CMD_IK                "CMD_IK"
#define SVC_CMD_IK_MSG_OK         (0x100U)
#define SVC_CMD_IK_MSG_NG         (0x200U)
#define SVC_CMD_IK_MSG_API        (0x400U)
#define SVC_LOG_CMD_IK_HL_TITLE_0 ("\033""[38;2;255;125;38m")
#define SVC_LOG_CMD_IK_HL_TITLE_1 ("\033""[38;2;255;255;128m")
#define SVC_LOG_CMD_IK_HL_TITLE_2 ("\033""[38;2;100;255;255m")
#define SVC_LOG_CMD_IK_HL_NUM     ("\033""[38;2;153;217;234m")
#define SVC_LOG_CMD_IK_HL_STR     ("\033""[38;2;255;174;201m")
#define SVC_LOG_CMD_IK_HL_END     ("\033""[0m")
#define PRN_CMD_IK_LOG        { SVC_WRAP_PRINT_s SvcCmdIkPrint; AmbaSvcWrap_MisraMemset(&(SvcCmdIkPrint), 0, sizeof(SvcCmdIkPrint)); SvcCmdIkPrint.Argc --; SvcCmdIkPrint.pStrFmt =
#define PRN_CMD_IK_ARG_UINT32 ; SvcCmdIkPrint.Argc ++; SvcCmdIkPrint.Argv[SvcCmdIkPrint.Argc].Uint64   = (UINT64)((
#define PRN_CMD_IK_ARG_CSTR   ; SvcCmdIkPrint.Argc ++; SvcCmdIkPrint.Argv[SvcCmdIkPrint.Argc].pCStr    = ((
#define PRN_CMD_IK_ARG_CPOINT ; SvcCmdIkPrint.Argc ++; SvcCmdIkPrint.Argv[SvcCmdIkPrint.Argc].pPointer = ((
#define PRN_CMD_IK_ARG_POST   ))
#define PRN_CMD_IK_OK         ; SvcCmdIkPrint.Argc ++; SvcCmdIK_PrintLog(SVC_CMD_IK_MSG_OK , &(SvcCmdIkPrint)); }
#define PRN_CMD_IK_NG         ; SvcCmdIkPrint.Argc ++; SvcCmdIK_PrintLog(SVC_CMD_IK_MSG_NG , &(SvcCmdIkPrint)); }
#define PRN_CMD_IK_API        ; SvcCmdIkPrint.Argc ++; SvcCmdIK_PrintLog(SVC_CMD_IK_MSG_API, &(SvcCmdIkPrint)); }
#define PRN_CMD_IK_ERR_HDLR   SvcCmdIK_ErrHdlr(__func__, __LINE__, PRetVal);

static void SvcCmdIK_PrintLog(UINT32 LogLevel, SVC_WRAP_PRINT_s *pPrint)
{
    if (pPrint != NULL) {
        switch (LogLevel) {
        case SVC_CMD_IK_MSG_OK :
            pPrint->pProc = SvcLog_OK;
            break;
        case SVC_CMD_IK_MSG_NG :
            pPrint->pProc = SvcLog_NG;
            break;
        default :
            pPrint->pProc = SvcLog_DBG;
            break;
        }

        SvcWrap_Print(SVC_CMD_IK, pPrint);
    }
}

static void SvcCmdIK_ErrHdlr(const char *pCaller, UINT32 CodeLine, UINT32 ErrCode)
{
    if (pCaller != NULL) {
        if (ErrCode != 0U) {
            PRN_CMD_IK_LOG "Catch ErrCode(0x%08x) @ %s, %d"
                PRN_CMD_IK_ARG_UINT32 ErrCode  PRN_CMD_IK_ARG_POST
                PRN_CMD_IK_ARG_CSTR   pCaller  PRN_CMD_IK_ARG_POST
                PRN_CMD_IK_ARG_UINT32 CodeLine PRN_CMD_IK_ARG_POST
            PRN_CMD_IK_NG
        }
    }
}

static void SvcCmdIK_MemAllocate(UINT32 ReqMemSize, UINT32 *pMemType, UINT8 **pReqBuf, UINT32 *pBufSize)
{
    UINT32 PRetVal;

    if (ReqMemSize > 0U) {
        if ((pMemType != NULL) && (pReqBuf != NULL)) {
            UINT32 ReqMemType = 255U;
            UINT32 FreeSize   = 0U;
            ULONG  FreeBase   = 0U;
            UINT8 *pCurBuf;

            if (0U == SvcBuffer_LockFreeSpace(SVC_MEM_TYPE_CA, &FreeBase, &FreeSize)) {
                if (FreeSize > ReqMemSize) {
                    ReqMemType = SVC_MEM_TYPE_CA;
                }
            }

            if (ReqMemType == 255U) {
                PRetVal = SvcBuffer_UnLockFreeSpace(SVC_MEM_TYPE_CA); PRN_CMD_IK_ERR_HDLR
#ifndef CONFIG_SOC_CV2FS
                if (0U == SvcBuffer_LockFreeSpace(SVC_MEM_TYPE_NC, &FreeBase, &FreeSize)) {
                    if (FreeSize > ReqMemSize) {
                        ReqMemType = SVC_MEM_TYPE_NC;
                    }
                }
            }

            if (ReqMemType == 255U) {
                PRetVal = SvcBuffer_UnLockFreeSpace(SVC_MEM_TYPE_NC); PRN_CMD_IK_ERR_HDLR
#endif
            } else {
                AmbaMisra_TypeCast(&(pCurBuf), &(FreeBase));
                *pMemType = ReqMemType;
                *pReqBuf = pCurBuf;
                if (pBufSize != NULL) {
                    *pBufSize = FreeSize;
                }
            }
        }
    }
}

static void SvcCmdIK_MemRelease(UINT32 MemType)
{
    UINT32 ErrCode;
    if ((MemType == SVC_MEM_TYPE_CA) || (MemType == SVC_MEM_TYPE_NC)) {
        ErrCode = SvcBuffer_UnLockFreeSpace(MemType);
        if (ErrCode != 0U) {
            AmbaMisra_TouchUnused(&ErrCode);
        }
    }
}

static void SvcCmdIK_CmdStrToPointer(const char *pStr, UINT8 **pPointer)
{
    if ((pStr != NULL) && (pPointer != NULL)) {
        UINT8 *pCurPointer = NULL;
#ifdef CONFIG_ARM64
        UINT64 U64Val = 0U;
        if (0U == SvcWrap_strtoull(pStr, &U64Val)) {
            AmbaMisra_TypeCast(&(pCurPointer), &(U64Val));
        }
#else
        UINT32 U32Val = 0U;
        if (0U == SvcWrap_strtoul(pStr, &U32Val)) {
            AmbaMisra_TypeCast(&(pCurPointer), &(U32Val));
        }
#endif
        *pPointer = pCurPointer;
    }
}

static UINT32 SvcCmdIK_InvalidCache(const UINT8 *pDataBuf, UINT32 DataSize)
{
    UINT32 RetVal = SVC_NG;
#ifdef CONFIG_ARM64
    UINT64 DataAddr, AlignAddr, AlignSize;

    AmbaMisra_TypeCast(&(DataAddr), &(pDataBuf));

    if (DataAddr >= 64ULL) {
        AlignAddr = DataAddr & 0xFFFFFFFFFFFFFFC0ULL;
        AlignSize = DataSize;
        if (AlignAddr < DataAddr) {
            AlignSize += DataAddr - AlignAddr;
        }
        AlignSize = GetAlignedValU64(AlignSize, 64ULL);

        RetVal = SvcPlat_CacheInvalidate((ULONG)AlignAddr, (ULONG)AlignSize);
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

        RetVal = SvcPlat_CacheInvalidate((ULONG)AlignAddr, (ULONG)AlignSize);
    }
#endif

    return RetVal;
}

static UINT32 SvcCmdIK_PhysToVirt(UINT8 *pPhys, UINT8 **pVirt)
{
    UINT32 RetVal = SVC_NG;

    if ((pPhys != NULL) && (pVirt != NULL)) {
#if defined(CONFIG_THREADX)
        ULONG PhysAddr = 0U, VirtAddr = 0U;
        UINT8 *pCurVirt = NULL;

        AmbaMisra_TypeCast(&(PhysAddr), &(pPhys));

        RetVal = AmbaMMU_PhysToVirt(PhysAddr, &VirtAddr);

        AmbaMisra_TypeCast(&(pCurVirt), &(VirtAddr));

        *pVirt = pCurVirt;
#elif (defined(CONFIG_QNX) || defined(CONFIG_LINUX))
        // The address should be virt address
        *pVirt = (UINT8*)pPhys;
        RetVal = SVC_OK;
#endif
        AmbaMisra_TouchUnused(pPhys);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcCmdIK_CmdDumpFolderCreate
 *
 *  @Description:: create idsp dump folder
 *
 *  @Input      ::
 *         pPath : The folder path
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     SVC_OK(0) : Success
 *     SVC_NG(1) : Failure
\*-----------------------------------------------------------------------------------------------*/
static UINT32 SvcCmdIK_CmdDumpFolderCreate(const char *pPath)
{
    UINT32 RetVal = SVC_OK;

    if (pPath == NULL) {
        RetVal = SVC_NG;
        PRN_CMD_IK_LOG "Fail to create idsp folder - invalid path!" PRN_CMD_IK_NG
    } else {
        UINT32 FolderNum = 0U;
        char   FolderStrs[UTIL_MAX_STR_TOKEN_NUMBER][UTIL_MAX_STR_TOKEN_LENGTH];
        char   FolderToken = '\\';

        if (0U == AmbaUtility_StringToken(pPath, FolderToken, FolderStrs, &FolderNum)) {

            if (FolderNum > 0U) {
                if ((FolderStrs[0][0] != 'c') && (FolderStrs[0][0] != 'C')
                    && (FolderStrs[0][0] != 'd') && (FolderStrs[0][0] != 'D')) {
                    RetVal = SVC_NG;
                    PRN_CMD_IK_LOG "Fail to create idsp folder - only support SD slot 'C/D'!" PRN_CMD_IK_NG
                } else {
                    const char *pStr = SvcWrap_strstr(pPath, FolderStrs[FolderNum - 1U]);
                    if (pStr != NULL) {
                        if (NULL == SvcWrap_strstr(pStr, "\\")) {
                            FolderNum -= 1U;
                        }
                    }
                }
            }

            if ((RetVal == SVC_OK) && (FolderNum > 0U)) {

                if (0U == AmbaFS_ChangeDir(FolderStrs[0])) {
                    UINT32 Idx;

                    for (Idx = 1U; Idx < FolderNum; Idx ++) {
                        if (0U != AmbaFS_MakeDir(FolderStrs[Idx])) {
                            PRN_CMD_IK_LOG "Sub-folder(%s) has been created or i/o error"
                                PRN_CMD_IK_ARG_CSTR FolderStrs[Idx] PRN_CMD_IK_ARG_POST
                                PRN_CMD_IK_API
                        }
                        if (0U != AmbaFS_ChangeDir(FolderStrs[Idx])) {
                            PRN_CMD_IK_LOG "Fail to switch to Sub-folder(%s)"
                                PRN_CMD_IK_ARG_CSTR FolderStrs[Idx] PRN_CMD_IK_ARG_POST
                                PRN_CMD_IK_NG
                            break;
                        }
                    }
                }
            }
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcCmdIK_CmdDumpBinSave
 *
 *  @Description:: save buffer to file
 *
 *  @Input      ::
 *       BufAddr : The buffer address
 *       BufSize : The buffer size
 *     pFileName : file name
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     SVC_OK(0) : Success
 *     SVC_NG(1) : Failure
\*-----------------------------------------------------------------------------------------------*/
static UINT32 SvcCmdIK_CmdDumpBinSave(UINT8* pBuf, UINT32 BufSize, const char *pFileName)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((pBuf == NULL) || (BufSize == 0U)) {
        RetVal = SVC_NG;
        PRN_CMD_IK_LOG "Fail to save idsp bin file - invalid buffer setting!" PRN_CMD_IK_NG
    } else if (pFileName == NULL) {
        RetVal = SVC_NG;
        PRN_CMD_IK_LOG "Fail to save idsp bin file - invalid saving file path!" PRN_CMD_IK_NG
    } else {
        AMBA_FS_FILE *pFile = NULL;

        RetVal = AmbaFS_FileOpen(pFileName, "wb", &pFile);
        if (RetVal != SVC_OK) {
            PRN_CMD_IK_LOG "Fail to save idsp bin file - create file fail! %s"
                PRN_CMD_IK_ARG_CSTR   pFileName PRN_CMD_IK_ARG_POST
                PRN_CMD_IK_NG
        } else {
            UINT32 WriteSize = BufSize, WrittenSize = 0U;

            RetVal = AmbaFS_FileWrite(pBuf, 1, WriteSize, pFile, &WrittenSize);
            if (RetVal != SVC_OK) {
                PRN_CMD_IK_LOG "Fail to save idsp bin file - write data to file fail!" PRN_CMD_IK_NG
            } else {
                if (WrittenSize != WriteSize) {
                    PRN_CMD_IK_LOG "Warning to save idsp bin file - written size (0x%x) is not match request size (0x%x)!"
                        PRN_CMD_IK_ARG_UINT32 WrittenSize PRN_CMD_IK_ARG_POST
                        PRN_CMD_IK_ARG_UINT32 WriteSize   PRN_CMD_IK_ARG_POST
                        PRN_CMD_IK_API
                }
            }

            PRetVal = AmbaFS_FileClose(pFile); PRN_CMD_IK_ERR_HDLR
        }
    }

    return RetVal;
}

static UINT8 *SvcCmdIK_CmdDumpIdspGetAddr(UINT8 *pBase, UINT32 Offset)
{
    UINT8 *pAddr = NULL;

    if ((pBase != NULL) && (Offset != 0U)) {
        const UINT8 *pShift = &(pBase[Offset & 0x00FFFFFFU]);
#ifdef CONFIG_ARM64
        UINT64 *pField = NULL, CurAddr;
#else
        UINT32 *pField = NULL, CurAddr;
#endif
        AmbaMisra_TypeCast(&(pField), &(pShift));
        if (pField != NULL) {
            CurAddr = *pField;

            AmbaMisra_TypeCast(&(pAddr), &(CurAddr));
        }

        AmbaMisra_TouchUnused(pBase);
        AmbaMisra_TouchUnused(pField);
    }

    return pAddr;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcCmdIK_CmdDumpIdspGetVal
 *
 *  @Description:: idsp dump - get value
 *
 *  @Input      ::
 *          Base : memory base
 *        offset : memory offset
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     SVC_OK(0) : Success
 *     SVC_NG(1) : Failure
\*-----------------------------------------------------------------------------------------------*/
static UINT32 SvcCmdIK_CmdDumpIdspGetVal(UINT8 *pBase, UINT32 Offset)
{
    UINT32 RetVal = 0U;

    if ((pBase != NULL) && (Offset != 0U)) {
        UINT32 Mask = Offset & 0xFF000000U;
        const UINT8 *pOfsBase = &(pBase[Offset & 0x00FFFFFFU]);

        // Fetch 4byte variable
        if (Mask == 0xF4000000U) {
            const UINT32 *pU32;
            AmbaMisra_TypeCast(&(pU32), &(pOfsBase));
            if (pU32 != NULL) {
                RetVal = *pU32;
            } else {
                RetVal = 0U;
            }
        } else if (Mask == 0xF2000000U) {
            const UINT16 *pU16;
            UINT16 U16Val;
            AmbaMisra_TypeCast(&(pU16), &(pOfsBase));
            if (pU16 != NULL) {
                U16Val = *pU16;
                RetVal = (UINT32)U16Val;
            } else {
                RetVal = 0U;
            }
        } else if (Mask == 0xF0000000U) {
            RetVal = ( Offset & 0x00FFFFFFU );
        } else {
            // Not supported mask rule
            RetVal = 0U;
        }

        AmbaMisra_TouchUnused(pBase);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcCmdIK_CmdDumpIdspBinCpy
 *
 *  @Description:: idsp dump - binary copy
 *
 *  @Input      ::
 *       SrcAddr : source address
 *       DstAddr : destination address
 *    DstBufSize : destination buffer size
 *         Pitch : buffer pitch
 *         Width : buffer width
 *        Height : buffer height
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     SVC_OK(0) : Success
 *     SVC_NG(1) : Failure
\*-----------------------------------------------------------------------------------------------*/
static UINT32 SvcCmdIK_CmdDumpIdspBinCpy(UINT8 *pSrc, UINT8 *pDst, UINT32 DstBufSize, UINT32 Pitch, UINT32 Width, UINT32 Height)
{
    UINT32 RetVal = SVC_OK, PRetVal;
    UINT32 CurPitch = Pitch, CurWidth = Width, CurHeight = Height;

    if (pSrc == NULL) {
        RetVal = SVC_NG;
        PRN_CMD_IK_LOG "Fail to dump idsp data - source address should not zero!" PRN_CMD_IK_NG
    } else if ((CurPitch == 0U) ||
               (CurWidth == 0U) ||
               (CurHeight == 0U)) {
        PRN_CMD_IK_LOG "Fail to dump idsp data - invalid win setting %d %d %d!"
            PRN_CMD_IK_ARG_UINT32 CurPitch  PRN_CMD_IK_ARG_POST
            PRN_CMD_IK_ARG_UINT32 CurWidth  PRN_CMD_IK_ARG_POST
            PRN_CMD_IK_ARG_UINT32 CurHeight PRN_CMD_IK_ARG_POST
            PRN_CMD_IK_NG

        CurPitch = 78080U;
        CurWidth = 78080U;
        CurHeight = 1U;
        PRN_CMD_IK_LOG "Refine win setting as %d %d %d!"
            PRN_CMD_IK_ARG_UINT32 CurPitch  PRN_CMD_IK_ARG_POST
            PRN_CMD_IK_ARG_UINT32 CurWidth  PRN_CMD_IK_ARG_POST
            PRN_CMD_IK_ARG_UINT32 CurHeight PRN_CMD_IK_ARG_POST
        PRN_CMD_IK_API
    } else if ((pDst == NULL) || (DstBufSize == 0U)) {
        RetVal = SVC_NG;
        PRN_CMD_IK_LOG "Fail to dump idsp data - invalid destination buf %p, 0x%x!"
            PRN_CMD_IK_ARG_CPOINT pDst       PRN_CMD_IK_ARG_POST
            PRN_CMD_IK_ARG_UINT32 DstBufSize PRN_CMD_IK_ARG_POST
            PRN_CMD_IK_NG
    } else if (DstBufSize < ( CurWidth * CurHeight ) ) {
        RetVal = SVC_NG;
        PRN_CMD_IK_LOG "Fail to dump idsp data - destination buffer size too small!" PRN_CMD_IK_NG
    } else {
        UINT8 *pVirSrc;

        if (MMU_ERR_NONE != SvcCmdIK_PhysToVirt(pSrc, &pVirSrc)) {
            RetVal = SVC_NG;
            PRN_CMD_IK_LOG "Fail to dump idsp data - Get the virtual source address fail! SrcAddr %p"
                PRN_CMD_IK_ARG_CPOINT pSrc PRN_CMD_IK_ARG_POST
                PRN_CMD_IK_NG
        } else {
            UINT32 RowIdx, CopySize = 0U;
            void  *pCurDst;
            const void *pCurSrc;

            PRetVal = SvcCmdIK_InvalidCache(pVirSrc, CurPitch * CurHeight); PRN_CMD_IK_ERR_HDLR

            for (RowIdx = 0U; RowIdx < CurHeight; RowIdx ++) {

                pCurSrc = &(pVirSrc[RowIdx * CurPitch]);
                pCurDst = &(pDst[RowIdx * CurWidth]);

                AmbaSvcWrap_MisraMemcpy(pCurDst, pCurSrc, CurWidth);
                CopySize += CurWidth;
            }

            RetVal = CopySize;
        }
    }

    AmbaMisra_TouchUnused(pSrc);

    return RetVal;
}

static void SvcCmdIK_CmdDumpIdspProc(UINT8 *pCfgBuf, UINT8 *pDumpBuf, UINT32 DumpBufSize, const void *pDumpFuncInfo, char *pPrefixStr)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    const SVC_CMD_IK_IDSP_DUMP_FUNC_s *pFuncInfo;

    AmbaMisra_TypeCast(&(pFuncInfo), &(pDumpFuncInfo));

    if ((pDumpBuf == NULL) || (DumpBufSize == 0U)) {
        RetVal = SVC_NG;
        PRN_CMD_IK_LOG "Fail to dump level idsp data - invalid memory control setting!" PRN_CMD_IK_NG
    } else if (pFuncInfo == NULL) {
        RetVal = SVC_NG;
        PRN_CMD_IK_LOG "Fail to dump level idsp data - input dump func info should not null!" PRN_CMD_IK_NG
    } else if (pCfgBuf == NULL) {
        RetVal = SVC_NG;
        PRN_CMD_IK_LOG "Fail to dump level idsp data - invalid configuration buffer base!" PRN_CMD_IK_NG
    } else {
        UINT32 InfoIdx, WriteSize;
        UINT8 *pSrc;
        UINT32 Pitch, Width, Height;
        const SVC_CMD_IK_IDSP_DUMP_INFO_s *pInfo = pFuncInfo->pInfo;
        char DumpFilePath[128];
        SVC_WRAP_SNPRINT_s LocalSnPrint;

        for (InfoIdx = 0U; InfoIdx < pFuncInfo->NumOfInfo; InfoIdx ++) {

            PRetVal = pInfo->Ofst_Addr & 0xFF000000U;

            if ((PRetVal == 0xF4000000U) || (PRetVal == 0xF8000000U)) {

                pSrc    = SvcCmdIK_CmdDumpIdspGetAddr(pCfgBuf, pInfo->Ofst_Addr);
                Pitch   = SvcCmdIK_CmdDumpIdspGetVal( pCfgBuf, pInfo->Ofst_Pitch);
                Width   = SvcCmdIK_CmdDumpIdspGetVal( pCfgBuf, pInfo->Ofst_Width);
                Height  = SvcCmdIK_CmdDumpIdspGetVal( pCfgBuf, pInfo->Ofst_Height);

                if (pSrc != NULL) {

                    PRN_CMD_IK_LOG "[%03d][%03d] Fetch bin data info from 0x%08XU, 0x%08XU, 0x%08XU, 0x%08XU"
                        PRN_CMD_IK_ARG_UINT32 pFuncInfo->Level   PRN_CMD_IK_ARG_POST
                        PRN_CMD_IK_ARG_UINT32 InfoIdx            PRN_CMD_IK_ARG_POST
                        PRN_CMD_IK_ARG_UINT32 pInfo->Ofst_Addr   PRN_CMD_IK_ARG_POST
                        PRN_CMD_IK_ARG_UINT32 pInfo->Ofst_Pitch  PRN_CMD_IK_ARG_POST
                        PRN_CMD_IK_ARG_UINT32 pInfo->Ofst_Width  PRN_CMD_IK_ARG_POST
                        PRN_CMD_IK_ARG_UINT32 pInfo->Ofst_Height PRN_CMD_IK_ARG_POST
                    PRN_CMD_IK_API

                    PRN_CMD_IK_LOG "[%03d][%03d] Dump bin data from %p %d %d %d"
                        PRN_CMD_IK_ARG_UINT32 pFuncInfo->Level PRN_CMD_IK_ARG_POST
                        PRN_CMD_IK_ARG_UINT32 InfoIdx          PRN_CMD_IK_ARG_POST
                        PRN_CMD_IK_ARG_CPOINT pSrc             PRN_CMD_IK_ARG_POST
                        PRN_CMD_IK_ARG_UINT32 Pitch            PRN_CMD_IK_ARG_POST
                        PRN_CMD_IK_ARG_UINT32 Width            PRN_CMD_IK_ARG_POST
                        PRN_CMD_IK_ARG_UINT32 Height           PRN_CMD_IK_ARG_POST
                    PRN_CMD_IK_API

                    if (pInfo->pFunc != NULL) {
                        if (0U == (pInfo->pFunc)(pSrc, pDumpBuf)) {
                            WriteSize = Pitch * Height;
                            PRN_CMD_IK_LOG "[%03d][%03d] Update configuration done"
                                PRN_CMD_IK_ARG_UINT32 pFuncInfo->Level PRN_CMD_IK_ARG_POST
                                PRN_CMD_IK_ARG_UINT32 InfoIdx          PRN_CMD_IK_ARG_POST
                            PRN_CMD_IK_API
                        } else {
                            WriteSize = 0U;
                            PRN_CMD_IK_LOG " [%03d][%03d] Update configuration fail"
                                PRN_CMD_IK_ARG_UINT32 pFuncInfo->Level PRN_CMD_IK_ARG_POST
                                PRN_CMD_IK_ARG_UINT32 InfoIdx          PRN_CMD_IK_ARG_POST
                                PRN_CMD_IK_NG
                        }
                    } else {
                        WriteSize = SvcCmdIK_CmdDumpIdspBinCpy(pSrc, pDumpBuf, DumpBufSize, Pitch, Width, Height);
                    }

                    if (WriteSize > DumpBufSize) {
                        RetVal = SVC_NG;
                        PRN_CMD_IK_LOG " [%03d][%03d] Fail to dump data! WriteSize(0x%08X) is out-of range(0x%08X)"
                            PRN_CMD_IK_ARG_UINT32 pFuncInfo->Level PRN_CMD_IK_ARG_POST
                            PRN_CMD_IK_ARG_UINT32 InfoIdx          PRN_CMD_IK_ARG_POST
                            PRN_CMD_IK_ARG_UINT32 WriteSize        PRN_CMD_IK_ARG_POST
                            PRN_CMD_IK_ARG_UINT32 DumpBufSize      PRN_CMD_IK_ARG_POST
                            PRN_CMD_IK_NG
                    } else if (WriteSize > 1U) {
                        AmbaSvcWrap_MisraMemset(DumpFilePath, 0, sizeof(DumpFilePath));

                        AmbaSvcWrap_MisraMemset(&LocalSnPrint, 0, sizeof(LocalSnPrint));
                        LocalSnPrint.pStrFmt = "%s_idsp_%p.bin";
                        LocalSnPrint.Argv[LocalSnPrint.Argc].pCStr = pPrefixStr; LocalSnPrint.Argc ++;
                        LocalSnPrint.Argv[LocalSnPrint.Argc].pPointer = pSrc; LocalSnPrint.Argc ++;
                        PRetVal = SvcWrap_snprintf(DumpFilePath, (UINT32)sizeof(DumpFilePath), &LocalSnPrint);

                        if (PRetVal > 0U) {
                            RetVal = SvcCmdIK_CmdDumpBinSave(pDumpBuf, WriteSize, DumpFilePath);
                            if (RetVal == SVC_OK) {
                                PRN_CMD_IK_LOG " [%03d][%03d] Successful to save data - %s!"
                                    PRN_CMD_IK_ARG_UINT32 pFuncInfo->Level PRN_CMD_IK_ARG_POST
                                    PRN_CMD_IK_ARG_UINT32 InfoIdx          PRN_CMD_IK_ARG_POST
                                    PRN_CMD_IK_ARG_CSTR   DumpFilePath     PRN_CMD_IK_ARG_POST
                                PRN_CMD_IK_OK
                            }
                        }
                    } else {
                        PRN_CMD_IK_LOG " [%03d][%03d] Fail to dump data! Ofst_Addr = 0x%08X."
                            PRN_CMD_IK_ARG_UINT32 pFuncInfo->Level PRN_CMD_IK_ARG_POST
                            PRN_CMD_IK_ARG_UINT32 InfoIdx          PRN_CMD_IK_ARG_POST
                            PRN_CMD_IK_ARG_UINT32 pInfo->Ofst_Addr PRN_CMD_IK_ARG_POST
                            PRN_CMD_IK_NG
                    }
                }
            } else {
                PRN_CMD_IK_LOG " [%03d][%03d] Ignore dump info. invalid Ofst_Addr = 0x%08X."
                    PRN_CMD_IK_ARG_UINT32 pFuncInfo->Level PRN_CMD_IK_ARG_POST
                    PRN_CMD_IK_ARG_UINT32 InfoIdx          PRN_CMD_IK_ARG_POST
                    PRN_CMD_IK_ARG_UINT32 pInfo->Ofst_Addr PRN_CMD_IK_ARG_POST
                    PRN_CMD_IK_NG
            }

            pInfo ++;
        }
    }

    AmbaMisra_TouchUnused(pPrefixStr);
    AmbaMisra_TouchUnused(&RetVal);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcCmdIK_CmdIkMode
 *
 *  @Description:: ik shell cmd - configure ik context id
 *
 *  @Input      ::
 *      ArgCount : Input arguments counter
 *    pArgVector : Input arguments data
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     SVC_OK(0) : Success
 *     SVC_NG(1) : Failure
\*-----------------------------------------------------------------------------------------------*/
static UINT32 SvcCmdIK_CmdIkMode(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK;

    if (ArgCount <= 2U) {
        RetVal = SVC_NG;
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
    } else {

        SvcCmdIK_CmdStrToU32(pArgVector[2U], &(SvcCmdIKCtrl.ImgMode.ContextId));

        PRN_CMD_IK_LOG "Config IK mode %d"
            PRN_CMD_IK_ARG_UINT32 SvcCmdIKCtrl.ImgMode.ContextId PRN_CMD_IK_ARG_POST
        PRN_CMD_IK_API
    }

    return RetVal;
}


static void SvcCmdIK_CmdIkModeU(void)
{
    PRN_CMD_IK_LOG "  %s-mode%s"
        PRN_CMD_IK_ARG_CSTR   SVC_LOG_CMD_IK_HL_TITLE_1 PRN_CMD_IK_ARG_POST
        PRN_CMD_IK_ARG_CSTR   SVC_LOG_CMD_IK_HL_END     PRN_CMD_IK_ARG_POST
    PRN_CMD_IK_API
    PRN_CMD_IK_LOG "    [IK context id] : the image kernel context id." PRN_CMD_IK_API
}

#if defined(CONFIG_ICAM_IMGITUNER_USED)
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcCmdIK_CmdItuner
 *
 *  @Description:: ik shell cmd - ituner save/load
 *
 *  @Input      ::
 *      ArgCount : Input arguments counter
 *    pArgVector : Input arguments data
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     SVC_OK(0) : Success
 *     SVC_NG(1) : Failure
\*-----------------------------------------------------------------------------------------------*/
static UINT32 SvcCmdIK_CmdItuner(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK;

    if (ArgCount <= 3U) {
        RetVal = SVC_NG;
        PRN_CMD_IK_LOG "Fail to process '-ituner' - invalid argument number. %d <= 3"
            PRN_CMD_IK_ARG_UINT32 ArgCount PRN_CMD_IK_ARG_POST
            PRN_CMD_IK_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_CMD_IK_LOG "Fail to process '-ituner' - invalid arguments" PRN_CMD_IK_NG
    } else {
        if (0 == SvcWrap_strcmp(pArgVector[2U], "load")) {
            SvcCmdIK_LoadItuner(SvcCmdIKCtrl.ImgMode.ContextId, pArgVector[3U]);
        } else if (0 == SvcWrap_strcmp(pArgVector[2U], "save")) {
            SvcCmdIK_SaveItuner(SvcCmdIKCtrl.ImgMode.ContextId, pArgVector[3U]);
        } else {
            RetVal = SVC_NG;
        }
    }

    return RetVal;
}


static void SvcCmdIK_CmdItunerU(void)
{
    PRN_CMD_IK_LOG "  %s-ituner%s           : image tuner tool"
        PRN_CMD_IK_ARG_CSTR   SVC_LOG_CMD_IK_HL_TITLE_1 PRN_CMD_IK_ARG_POST
        PRN_CMD_IK_ARG_CSTR   SVC_LOG_CMD_IK_HL_END     PRN_CMD_IK_ARG_POST
    PRN_CMD_IK_API
    PRN_CMD_IK_LOG "    %sload%s            : load the idsp setting from file."
        PRN_CMD_IK_ARG_CSTR   SVC_LOG_CMD_IK_HL_TITLE_2 PRN_CMD_IK_ARG_POST
        PRN_CMD_IK_ARG_CSTR   SVC_LOG_CMD_IK_HL_END     PRN_CMD_IK_ARG_POST
    PRN_CMD_IK_API
    PRN_CMD_IK_LOG "      [file path]   : the ituner file path." PRN_CMD_IK_API

    PRN_CMD_IK_LOG "    %ssave%s            : save the idsp setting from file. It depend on each IK context id"
        PRN_CMD_IK_ARG_CSTR   SVC_LOG_CMD_IK_HL_TITLE_2 PRN_CMD_IK_ARG_POST
        PRN_CMD_IK_ARG_CSTR   SVC_LOG_CMD_IK_HL_END     PRN_CMD_IK_ARG_POST
    PRN_CMD_IK_API
    PRN_CMD_IK_LOG "      [file path]   : the ituner file path." PRN_CMD_IK_API
}
#endif

#if defined(CONFIG_FWPROG_R52SYS_ENABLE)
static UINT32 SvcCmdIK_CmdSafety(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK;

    if (ArgCount <= 3U) {
        RetVal = SVC_NG;
        PRN_CMD_IK_LOG "Fail to process '-safety' - invalid argument number. %d <= 3"
            PRN_CMD_IK_ARG_UINT32 ArgCount PRN_CMD_IK_ARG_POST
            PRN_CMD_IK_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_CMD_IK_LOG "Fail to process '-safety' - invalid arguments" PRN_CMD_IK_NG
    } else {
        const AMBA_IK_MODE_CFG_s *pImgMode = &(SvcCmdIKCtrl.ImgMode);

        PRN_CMD_IK_LOG "Current IK mode %d"
            PRN_CMD_IK_ARG_UINT32 pImgMode->ContextId PRN_CMD_IK_ARG_POST
        PRN_CMD_IK_API

        if (0 == SvcWrap_strcmp(pArgVector[2U], "fltinj")) {
            UINT32 Select;
            if (SVC_OK == SvcWrap_strtoul(pArgVector[3U], &Select)) {
                if (Select <= 5U) {
                    RetVal = AmbaIK_InjectSaftyError(&(SvcCmdIKCtrl.ImgMode), Select);
                    if (RetVal == OK) {
                        SvcLog_OK(SVC_CMD_IK, "AmbaIK_InjectSaftyError %u OK", Select, 0U);
                    } else {
                        SvcLog_NG(SVC_CMD_IK, "AmbaIK_InjectSaftyError %u return 0x%x", Select, RetVal);
                    }
                }
            }
        }
    }
    return RetVal;
}

static void SvcCmdIK_CmdSafetyU(void)
{
    PRN_CMD_IK_LOG "  %s-safety%s           : safety tool"
        PRN_CMD_IK_ARG_CSTR   SVC_LOG_CMD_IK_HL_TITLE_1 PRN_CMD_IK_ARG_POST
        PRN_CMD_IK_ARG_CSTR   SVC_LOG_CMD_IK_HL_END     PRN_CMD_IK_ARG_POST
    PRN_CMD_IK_API
    PRN_CMD_IK_LOG "    %sfltinj%s          : fault injection test"
        PRN_CMD_IK_ARG_CSTR   SVC_LOG_CMD_IK_HL_TITLE_2 PRN_CMD_IK_ARG_POST
        PRN_CMD_IK_ARG_CSTR   SVC_LOG_CMD_IK_HL_END     PRN_CMD_IK_ARG_POST
    PRN_CMD_IK_API
    PRN_CMD_IK_LOG "      [err_id]      : error id (0 ~ 5)." PRN_CMD_IK_API
}
#endif

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcCmdIK_CmdDumpIdsp
 *
 *  @Description:: ik shell cmd - idsp dump
 *
 *  @Input      ::
 *      ArgCount : Input arguments counter
 *    pArgVector : Input arguments data
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     SVC_OK(0) : Success
 *     SVC_NG(1) : Failure
\*-----------------------------------------------------------------------------------------------*/
static UINT32 SvcCmdIK_CmdDumpIdsp(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK;

    if (ArgCount <= 4U) {
        RetVal = SVC_NG;
        PRN_CMD_IK_LOG "Fail to dump idsp data - invalid argument number. %d <= 4"
            PRN_CMD_IK_ARG_UINT32 ArgCount PRN_CMD_IK_ARG_POST
            PRN_CMD_IK_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_CMD_IK_LOG "Fail to dump idsp data - invalid arguments!" PRN_CMD_IK_NG
    } else {
        UINT32 DumpLevel  = 0U;
        UINT32 CtrlFlag   = SVC_CMD_IK_SAVE_IDSP_CFG_NORMAL;
        UINT32 ViewZoneID = SvcCmdIKCtrl.ImgMode.ContextId;

        SvcCmdIK_CmdStrToU32(pArgVector[4U], &DumpLevel);
        if (ArgCount > 5U) {
            SvcCmdIK_CmdStrToU32(pArgVector[5U], &CtrlFlag);
        }

        SvcCmdIK_SaveIdspCfg(ViewZoneID, DumpLevel, CtrlFlag, pArgVector[3U]);
    }

    return RetVal;
}


static void SvcCmdIK_CmdDumpIdspU(void)
{
    SvcCmdIK_DumpIdspUsage();
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcCmdIK_CmdDumpIso
 *
 *  @Description:: ik shell cmd - iso dump
 *
 *  @Input      ::
 *      ArgCount : Input arguments counter
 *    pArgVector : Input arguments data
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     SVC_OK(0) : Success
 *     SVC_NG(1) : Failure
\*-----------------------------------------------------------------------------------------------*/
static UINT32 SvcCmdIK_CmdDumpIso(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK;

    if (ArgCount <= 3U) {
        RetVal = SVC_NG;
        PRN_CMD_IK_LOG "Fail to dump idsp data - invalid argument number. %d <= 3"
            PRN_CMD_IK_ARG_UINT32 ArgCount PRN_CMD_IK_ARG_POST
            PRN_CMD_IK_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_CMD_IK_LOG "Fail to dump idsp data - invalid arguments!" PRN_CMD_IK_NG
    } else {
        UINT32 PreCnt = 0U;

        if (ArgCount > 4U) {
            SvcCmdIK_CmdStrToU32(pArgVector[4U], &PreCnt);
        }
        SvcCmdIK_SaveIsoCfg(SvcCmdIKCtrl.ImgMode.ContextId, PreCnt, pArgVector[3U]);
    }

    return RetVal;
}


static void SvcCmdIK_CmdDumpIsoU(void)
{
    PRN_CMD_IK_LOG "    %siso%s             : "
        PRN_CMD_IK_ARG_CSTR   SVC_LOG_CMD_IK_HL_TITLE_2 PRN_CMD_IK_ARG_POST
        PRN_CMD_IK_ARG_CSTR   SVC_LOG_CMD_IK_HL_END     PRN_CMD_IK_ARG_POST
    PRN_CMD_IK_API
    PRN_CMD_IK_LOG "      [output path] : configure the output folder or prefix name" PRN_CMD_IK_API
    PRN_CMD_IK_LOG "      [pre count]   : default is 0. dump previous n sync data." PRN_CMD_IK_API
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcCmdIK_CmdDumpMem
 *
 *  @Description:: ik shell cmd - hook dump memory
 *
 *  @Input      ::
 *      ArgCount : Input arguments counter
 *    pArgVector : Input arguments data
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     SVC_OK(0) : Success
 *     SVC_NG(1) : Failure
\*-----------------------------------------------------------------------------------------------*/
static UINT32 SvcCmdIK_CmdDumpMem(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK;

    if (ArgCount <= 4U) {
        RetVal = SVC_NG;
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
    } else {
        UINT8 *pBuf    = NULL;
        UINT32 BufSize = 0U;

        SvcCmdIK_CmdStrToPointer(pArgVector[3U], &pBuf);
        SvcCmdIK_CmdStrToU32(pArgVector[4U], &BufSize);

        PRN_CMD_IK_LOG "Configure BufAddr(%p) BufSize(0x%x)"
            PRN_CMD_IK_ARG_CPOINT pBuf    PRN_CMD_IK_ARG_POST
            PRN_CMD_IK_ARG_UINT32 BufSize PRN_CMD_IK_ARG_POST
        PRN_CMD_IK_API

#ifdef CONFIG_SOC_CV2FS
        if (SVC_OK != SvcCmdIK_IsCacheMemory(pBuf)) {
            RetVal = SVC_NG;
            PRN_CMD_IK_LOG "SVC IK only support cache memory in CV2FS" PRN_CMD_IK_API
        } else
#endif
        {
            if ((pBuf != NULL) && (BufSize != 0U)) {
                SvcCmdIKCtrl.MemCtrl[SVC_CMD_IK_DUMP_MEM].Enable  = 1U;
                SvcCmdIKCtrl.MemCtrl[SVC_CMD_IK_DUMP_MEM].pBuf    = pBuf;
                SvcCmdIKCtrl.MemCtrl[SVC_CMD_IK_DUMP_MEM].BufSize = BufSize;
                PRN_CMD_IK_LOG "SVC IK command dump memory setting. BufAddr(%p) BufSize(0x%x)"
                    PRN_CMD_IK_ARG_CPOINT SvcCmdIKCtrl.MemCtrl[SVC_CMD_IK_DUMP_MEM].pBuf    PRN_CMD_IK_ARG_POST
                    PRN_CMD_IK_ARG_UINT32 SvcCmdIKCtrl.MemCtrl[SVC_CMD_IK_DUMP_MEM].BufSize PRN_CMD_IK_ARG_POST
                PRN_CMD_IK_API
            }
        }
    }

    return RetVal;
}


static void SvcCmdIK_CmdDumpMemU(void)
{
    UINT32 PRetVal;
    SIZE_t DumpIsoMemSize = 0;

    PRN_CMD_IK_LOG "    %smem%s             : "
        PRN_CMD_IK_ARG_CSTR   SVC_LOG_CMD_IK_HL_TITLE_2 PRN_CMD_IK_ARG_POST
        PRN_CMD_IK_ARG_CSTR   SVC_LOG_CMD_IK_HL_END     PRN_CMD_IK_ARG_POST
    PRN_CMD_IK_API
#ifdef CONFIG_SOC_CV2FS
    PRN_CMD_IK_LOG "      [Buf Addr]    : Configure cache memory addr for dump" PRN_CMD_IK_API
#else
    PRN_CMD_IK_LOG "      [Buf Addr]    : Configure memory addr for dump" PRN_CMD_IK_API
#endif
    PRN_CMD_IK_LOG "      [Buf Size]    : Configure memory size for dump" PRN_CMD_IK_API
    PRN_CMD_IK_LOG "                    : For idsp, memory size >= 0x%x"
        PRN_CMD_IK_ARG_UINT32 SVC_CMD_IK_IDSP_DUMP_SIZE PRN_CMD_IK_ARG_POST
    PRN_CMD_IK_API

#if defined(CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
    PRetVal = AmbaIK_QueryDebugConfigSize(&(SvcCmdIKCtrl.ImgMode), &DumpIsoMemSize); PRN_CMD_IK_ERR_HDLR
#else
    PRetVal = AmbaIK_QueryDebugConfigSize(&DumpIsoMemSize); PRN_CMD_IK_ERR_HDLR
#endif
    PRN_CMD_IK_LOG "                    : For iso, memory size >= 0x%x"
        PRN_CMD_IK_ARG_UINT32 DumpIsoMemSize PRN_CMD_IK_ARG_POST
    PRN_CMD_IK_API

}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcCmdIK_CmdDump
 *
 *  @Description:: ik shell cmd - dump cmd entry
 *
 *  @Input      ::
 *      ArgCount : Input arguments counter
 *    pArgVector : Input arguments data
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     SVC_OK(0) : Success
 *     SVC_NG(1) : Failure
\*-----------------------------------------------------------------------------------------------*/
static UINT32 SvcCmdIK_CmdDump(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK;

    if (ArgCount <= 2U) {
        RetVal = SVC_NG;
        PRN_CMD_IK_LOG "Fail to process '-dump' - invalid arg count!" PRN_CMD_IK_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
    } else {
        if (0 == SvcWrap_strcmp(pArgVector[2U], "idsp")) {
            if (SVC_OK != SvcCmdIK_CmdDumpIdsp(ArgCount, pArgVector)) {
                SvcCmdIK_CmdDumpIdspU();
            }
        } else if (0 == SvcWrap_strcmp(pArgVector[2U], "iso")) {
            if (SVC_OK != SvcCmdIK_CmdDumpIso(ArgCount, pArgVector)) {
                SvcCmdIK_CmdDumpIsoU();
            }
        } else if (0 == SvcWrap_strcmp(pArgVector[2U], "mem")) {
            if (SVC_OK != SvcCmdIK_CmdDumpMem(ArgCount, pArgVector)) {
                SvcCmdIK_CmdDumpMemU();
            }
        } else {
            RetVal = SVC_NG;
        }
    }

    return RetVal;
}


static void SvcCmdIK_CmdDumpU(void)
{
    PRN_CMD_IK_LOG "  %s-dump%s"
        PRN_CMD_IK_ARG_CSTR   SVC_LOG_CMD_IK_HL_TITLE_1 PRN_CMD_IK_ARG_POST
        PRN_CMD_IK_ARG_CSTR   SVC_LOG_CMD_IK_HL_END     PRN_CMD_IK_ARG_POST
    PRN_CMD_IK_API
    SvcCmdIK_CmdDumpIdspU();
    SvcCmdIK_CmdDumpIsoU();
    SvcCmdIK_CmdDumpMemU();
}


static void SvcCmdIK_CmdEntryInit(void)
{
    static UINT32 SvcCmdIKEntryInit = 0U;

    if (SvcCmdIKEntryInit == 0U) {
        UINT32 FuncNum = 0U;

        AmbaSvcWrap_MisraMemset(SvcCmdIKFunc, 0, sizeof(SvcCmdIKFunc));
        SvcCmdIKFunc[FuncNum] = (SVC_CMD_IK_FUNC_s) { 1, "-mode",   SvcCmdIK_CmdIkMode, SvcCmdIK_CmdIkModeU}; FuncNum ++;
#if defined(CONFIG_ICAM_IMGITUNER_USED)
        SvcCmdIKFunc[FuncNum] = (SVC_CMD_IK_FUNC_s) { 1, "-ituner", SvcCmdIK_CmdItuner, SvcCmdIK_CmdItunerU}; FuncNum ++;
#endif
#if defined(CONFIG_FWPROG_R52SYS_ENABLE)
        SvcCmdIKFunc[FuncNum] = (SVC_CMD_IK_FUNC_s) { 1, "-safety", SvcCmdIK_CmdSafety, SvcCmdIK_CmdSafetyU}; FuncNum ++;
#endif
        SvcCmdIKFunc[FuncNum] = (SVC_CMD_IK_FUNC_s) { 1, "-dump",   SvcCmdIK_CmdDump,   SvcCmdIK_CmdDumpU};

        SvcCmdIKEntryInit = 1U;
    }
}


static void SvcCmdIK_CmdEntry(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 RetVal = SVC_OK;

    SvcCmdIK_CmdEntryInit();

    if(ArgCount == 0xCafeU) {
        UINT32 SIdx;

        PRN_CMD_IK_LOG " " PRN_CMD_IK_API
        PRN_CMD_IK_LOG "==== %sShell Arguments Setting%s (%s%d%s) ===="
            PRN_CMD_IK_ARG_CSTR   SVC_LOG_CMD_IK_HL_TITLE_0 PRN_CMD_IK_ARG_POST
            PRN_CMD_IK_ARG_CSTR   SVC_LOG_CMD_IK_HL_END     PRN_CMD_IK_ARG_POST
            PRN_CMD_IK_ARG_CSTR   SVC_LOG_CMD_IK_HL_NUM     PRN_CMD_IK_ARG_POST
            PRN_CMD_IK_ARG_UINT32 ArgCount                  PRN_CMD_IK_ARG_POST
            PRN_CMD_IK_ARG_CSTR   SVC_LOG_CMD_IK_HL_END     PRN_CMD_IK_ARG_POST
        PRN_CMD_IK_API

        for (SIdx = 0U; SIdx < ArgCount; SIdx ++) {
            PRN_CMD_IK_LOG "  pArgVector[%s%d%s] : %s%s%s"
                PRN_CMD_IK_ARG_CSTR   SVC_LOG_CMD_IK_HL_NUM PRN_CMD_IK_ARG_POST
                PRN_CMD_IK_ARG_UINT32 SIdx                  PRN_CMD_IK_ARG_POST
                PRN_CMD_IK_ARG_CSTR   SVC_LOG_CMD_IK_HL_END PRN_CMD_IK_ARG_POST
                PRN_CMD_IK_ARG_CSTR   SVC_LOG_CMD_IK_HL_STR PRN_CMD_IK_ARG_POST
                PRN_CMD_IK_ARG_CSTR   pArgVector[SIdx]      PRN_CMD_IK_ARG_POST
                PRN_CMD_IK_ARG_CSTR   SVC_LOG_CMD_IK_HL_END PRN_CMD_IK_ARG_POST
            PRN_CMD_IK_API
        }
    }

    if (ArgCount < 1U) {
        RetVal = SVC_NG;
        PRN_CMD_IK_LOG "Fail to proc raw enc shell cmd - invalid arg count!" PRN_CMD_IK_NG
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
        PRN_CMD_IK_LOG "Fail to proc raw enc shell cmd - invalid arg vector!" PRN_CMD_IK_NG
    } else {
        UINT32 ShellIdx, ShellCnt = ((UINT32)(sizeof(SvcCmdIKFunc))) / ((UINT32)(sizeof(SvcCmdIKFunc[0])));

        RetVal = SVC_NG;

        for (ShellIdx = 0U; ShellIdx < ShellCnt; ShellIdx ++) {
            if (SvcCmdIKFunc[ShellIdx].Enable > 0U) {

                if (0 == SvcWrap_strcmp(SvcCmdIKFunc[ShellIdx].Name, pArgVector[1U])) {

                    if (SvcCmdIKFunc[ShellIdx].pFunc == NULL) {
                        PRN_CMD_IK_LOG "Shell command '%s' function be not hook."
                            PRN_CMD_IK_ARG_CSTR   SvcCmdIKFunc[ShellIdx].Name PRN_CMD_IK_ARG_POST
                        PRN_CMD_IK_API
                    } else {
                        if (SVC_OK != (SvcCmdIKFunc[ShellIdx].pFunc)(ArgCount, pArgVector)) {
                            if (SvcCmdIKFunc[ShellIdx].pUsage != NULL) {
                                (SvcCmdIKFunc[ShellIdx].pUsage)();
                            }
                        }
                    }

                    RetVal = SVC_OK;
                    break;
                }
            }
        }

    }

    if (RetVal != SVC_OK) {
        SvcCmdIK_CmdUsage();
        AmbaMisra_TouchUnused(&PrintFunc);
    }
}

static void SvcCmdIK_CmdUsage(void)
{
    UINT32 ShellIdx, ShellCnt = ((UINT32)(sizeof(SvcCmdIKFunc))) / ((UINT32)(sizeof(SvcCmdIKFunc[0])));

    PRN_CMD_IK_LOG "====== %sSVC Shell IK Command Usage%s ===="
        PRN_CMD_IK_ARG_CSTR   SVC_LOG_CMD_IK_HL_TITLE_0 PRN_CMD_IK_ARG_POST
        PRN_CMD_IK_ARG_CSTR   SVC_LOG_CMD_IK_HL_END     PRN_CMD_IK_ARG_POST
    PRN_CMD_IK_API

    for (ShellIdx = 0U; ShellIdx < ShellCnt; ShellIdx ++) {
        if (SvcCmdIKFunc[ShellIdx].Enable > 0U) {
            if (SvcCmdIKFunc[ShellIdx].pUsage == NULL) {
                PRN_CMD_IK_LOG "  %s"
                    PRN_CMD_IK_ARG_CSTR   SvcCmdIKFunc[ShellIdx].Name PRN_CMD_IK_ARG_POST
                PRN_CMD_IK_API
            } else {
                (SvcCmdIKFunc[ShellIdx].pUsage)();
            }
        }
    }
}

static void SvcCmdIK_CmdStrToU32(const char *pString, UINT32 *pValue)
{
    UINT32 ErrCode = AmbaUtility_StringToUInt32(pString, pValue);
    if (ErrCode != 0U) {
        AmbaMisra_TouchUnused(&ErrCode);
    }
}

/**
 *  @private
 *  Check the memory is cache or non-cache
 *  @param[in] pBuf buffer pointer
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 SvcCmdIK_IsCacheMemory(void *pBuf)
{
    UINT32 RetVal = SVC_NG;

    if (pBuf != NULL) {
        RetVal = SVC_OK;    //  cached
        if ((g_pPlatCbEntry != NULL) && (g_pPlatCbEntry->pCbCacheChk != NULL)) {
            if (g_pPlatCbEntry->pCbCacheChk(pBuf, 1U) == 0U) {
                RetVal = SVC_NG;    //  non-cached
            }
        }
    }

    return RetVal;
}

/**
 * Configure ituner buffer to enable it
 *
 * @param [in] BufAddr ituner buffer address
 * @param [in] BufSize ituner buffer size
 * 
 */
void SvcCmdIK_ItunerOn(ULONG BufAddr, UINT32 BufSize)
{
    if ((BufAddr != 0U) && (BufSize != 0U)) {
        SvcCmdIKCtrl.MemCtrl[SVC_CMD_IK_ITUNER_MEM].Enable  = 1U;
        SvcCmdIKCtrl.MemCtrl[SVC_CMD_IK_ITUNER_MEM].BufSize = BufSize;
        AmbaMisra_TypeCast(&(SvcCmdIKCtrl.MemCtrl[SVC_CMD_IK_ITUNER_MEM].pBuf), &(BufAddr));

        PRN_CMD_IK_LOG "Enable the Shell IK iTuner Buffer %p 0x%x"
            PRN_CMD_IK_ARG_CPOINT SvcCmdIKCtrl.MemCtrl[SVC_CMD_IK_ITUNER_MEM].pBuf    PRN_CMD_IK_ARG_POST
            PRN_CMD_IK_ARG_UINT32 SvcCmdIKCtrl.MemCtrl[SVC_CMD_IK_ITUNER_MEM].BufSize PRN_CMD_IK_ARG_POST
        PRN_CMD_IK_API
    }
}

/**
 * Query buffer size of idsp/iso dump mechanism
 *
 * @param [out] pBufSize Requested buffer size
 *
 */
void SvcCmdIK_DumpMemQry(UINT32 *pBufSize)
{
    UINT32 MaxDumpMemSize = SVC_CMD_IK_IDSP_DUMP_SIZE;

    if (pBufSize != NULL) {
        *pBufSize = MaxDumpMemSize;

        PRN_CMD_IK_LOG "Query raw cap memory size 0x%x"
            PRN_CMD_IK_ARG_UINT32 *pBufSize PRN_CMD_IK_ARG_POST
        PRN_CMD_IK_API
    }
}

/**
 * Config buffer of idsp/iso dump mechanism
 *
 * @param [in] BufAddr buffer address
 * @param [in] BufSize buffer size
 *
 */
void SvcCmdIK_DumpMemCfg(ULONG BufAddr, UINT32 BufSize)
{
    if ((BufAddr != 0U) && (BufSize != 0U)) {
        SvcCmdIKCtrl.MemCtrl[SVC_CMD_IK_DUMP_MEM].Enable  = 1U;
        SvcCmdIKCtrl.MemCtrl[SVC_CMD_IK_DUMP_MEM].BufSize = BufSize;
        AmbaMisra_TypeCast(&(SvcCmdIKCtrl.MemCtrl[SVC_CMD_IK_DUMP_MEM].pBuf), &(BufAddr));

        PRN_CMD_IK_LOG "Enable the Shell IK Dump Buffer %p 0x%X"
            PRN_CMD_IK_ARG_CPOINT SvcCmdIKCtrl.MemCtrl[SVC_CMD_IK_DUMP_MEM].pBuf    PRN_CMD_IK_ARG_POST
            PRN_CMD_IK_ARG_UINT32 SvcCmdIKCtrl.MemCtrl[SVC_CMD_IK_DUMP_MEM].BufSize PRN_CMD_IK_ARG_POST
        PRN_CMD_IK_API
    }
}

/**
 * Dump ituner data by specify image kernel context id
 *
 * @param [in] IkCtxID image kernel context id
 * @param [in] pFileName ituner text file name
 *
 */
void SvcCmdIK_SaveItuner(UINT32 IkCtxID, char *pFileName)
{
#ifdef CONFIG_ICAM_IMGITUNER_USED
    const SVC_CMD_IK_MEM_CTRL_s *pMemCtrl = &(SvcCmdIKCtrl.MemCtrl[SVC_CMD_IK_ITUNER_MEM]);

    if (IkCtxID >= MAX_CONTEXT_NUM) {
        PRN_CMD_IK_LOG "Fail to save ituner - invalid IkCtxID(%d)" PRN_CMD_IK_ARG_UINT32 IkCtxID PRN_CMD_IK_ARG_POST PRN_CMD_IK_NG
    } else if (pFileName == NULL) {
        PRN_CMD_IK_LOG "Fail to save ituner - filename should not null" PRN_CMD_IK_NG
    } else if (pMemCtrl->Enable == 0U) {
        PRN_CMD_IK_LOG "Fail to save ituner - invalid buffer setting" PRN_CMD_IK_NG
    } else {
        UINT32 PRetVal;
        void *pMemBuf;
        UINT32 MemSize = pMemCtrl->BufSize;
        AMBA_IK_MODE_CFG_s ImgMode = { .ContextId = IkCtxID };

        // Configure the ituner buffer
        pMemBuf = pMemCtrl->pBuf;

        PRN_CMD_IK_LOG "Save ituner with IkCtxID(%d)"
            PRN_CMD_IK_ARG_UINT32 ImgMode.ContextId PRN_CMD_IK_ARG_POST
        PRN_CMD_IK_API

        // Change the tuner module mode as TEXT
        PRetVal = AmbaItn_Change_Parser_Mode(TEXT_TUNE); PRN_CMD_IK_ERR_HDLR

        // Initial ituner module
        PRetVal = AmbaItn_Init(pMemBuf, MemSize);
        if (TUNE_OK != PRetVal) {
            PRN_CMD_IK_LOG "Fail to save ituner - initial tuner module fail! ErrCode(0x%08X)"
                PRN_CMD_IK_ARG_UINT32 PRetVal PRN_CMD_IK_ARG_POST
            PRN_CMD_IK_NG
        }

        // Update the ituner system info
        if (PRetVal == SVC_OK) {
            AMBA_ITN_SYSTEM_s Ituner_SysInfo;

            AmbaSvcWrap_MisraMemset(&Ituner_SysInfo, 0, sizeof(Ituner_SysInfo));

            // Get current system info
            PRetVal = AmbaItn_Get_SystemInfo(&Ituner_SysInfo);
            if (TUNE_OK != PRetVal) {
                PRN_CMD_IK_LOG "Fail to save ituner - get current ituner system info fail! ErrCode(0x%08X)"
                    PRN_CMD_IK_ARG_UINT32 PRetVal PRN_CMD_IK_ARG_POST
                PRN_CMD_IK_NG
            } else {
                AMBA_IK_WINDOW_SIZE_INFO_s ImgWinInfo;

                // Get image win info by IK context id
                AmbaSvcWrap_MisraMemset(&ImgWinInfo, 0, sizeof(ImgWinInfo));
                PRetVal = AmbaIK_GetWindowSizeInfo( &ImgMode, &ImgWinInfo );
                if ( 0U != PRetVal ) {
                    PRN_CMD_IK_LOG "Fail to save ituner - get IkCtxID(%d) image win info fail! ErrCode(0x%08X)"
                        PRN_CMD_IK_ARG_UINT32 ImgMode.ContextId PRN_CMD_IK_ARG_POST
                        PRN_CMD_IK_ARG_UINT32 PRetVal           PRN_CMD_IK_ARG_POST
                    PRN_CMD_IK_NG
                } else {

                    // Update system raw info
                    Ituner_SysInfo.RawStartX           = ImgWinInfo.VinSensor.StartX;
                    Ituner_SysInfo.RawStartY           = ImgWinInfo.VinSensor.StartY;
                    Ituner_SysInfo.RawWidth            = ImgWinInfo.VinSensor.Width;
                    Ituner_SysInfo.RawHeight           = ImgWinInfo.VinSensor.Height;
                    Ituner_SysInfo.HSubSampleFactorNum = ImgWinInfo.VinSensor.HSubSample.FactorNum;
                    Ituner_SysInfo.HSubSampleFactorDen = ImgWinInfo.VinSensor.HSubSample.FactorDen;
                    Ituner_SysInfo.VSubSampleFactorNum = ImgWinInfo.VinSensor.VSubSample.FactorNum;
                    Ituner_SysInfo.VSubSampleFactorDen = ImgWinInfo.VinSensor.VSubSample.FactorDen;

                    // Update the system info
                    PRetVal = AmbaItn_Set_SystemInfo(&Ituner_SysInfo);
                    if (TUNE_OK != PRetVal) {
                        PRN_CMD_IK_LOG "Fail to save ituner - set ituner system info fail! ErrCode(0x%08X)"
                            PRN_CMD_IK_ARG_UINT32 PRetVal PRN_CMD_IK_ARG_POST
                        PRN_CMD_IK_NG
                    }
                }
            }
        }

        // Save the ituner to file
        if (PRetVal == SVC_OK) {
            AMBA_ITN_Save_Param_s Ituner_SaveParam;

            AmbaSvcWrap_MisraMemset(&Ituner_SaveParam, 0, sizeof(Ituner_SaveParam));
            Ituner_SaveParam.Text.Filepath = pFileName;

            PRetVal = AmbaItn_Save_IDSP(&ImgMode, &Ituner_SaveParam);
            if (TUNE_OK != PRetVal) {
                PRN_CMD_IK_LOG "Fail to save ituner - save ituner fail! ErrCode(0x%08X)"
                    PRN_CMD_IK_ARG_UINT32 PRetVal PRN_CMD_IK_ARG_POST
                PRN_CMD_IK_NG
            }
        }
    }
#else
    PRN_CMD_IK_LOG "Enable ituner module first" PRN_CMD_IK_API
#endif

    AmbaMisra_TouchUnused(&IkCtxID);
    AmbaMisra_TouchUnused(pFileName);
}

/**
 * Load ituner data to specify image kernel context id
 *
 * @param [in] IkCtxID image kernel context id
 * @param [in] pFileName ituner text file name
 *
 */
void SvcCmdIK_LoadItuner(UINT32 IkCtxID, char *pFileName)
{
#ifdef CONFIG_ICAM_IMGITUNER_USED
    const SVC_CMD_IK_MEM_CTRL_s *pMemCtrl = &(SvcCmdIKCtrl.MemCtrl[SVC_CMD_IK_ITUNER_MEM]);

    if (IkCtxID >= MAX_CONTEXT_NUM) {
        PRN_CMD_IK_LOG "Fail to load ituner - invalid IkCtxID(%d)" PRN_CMD_IK_ARG_UINT32 IkCtxID PRN_CMD_IK_ARG_POST PRN_CMD_IK_NG
    } else if (pFileName == NULL) {
        PRN_CMD_IK_LOG "Fail to load ituner - filename should not null" PRN_CMD_IK_NG
    } else if (pMemCtrl->Enable == 0U) {
        PRN_CMD_IK_LOG "Fail to load ituner - invalid buffer setting" PRN_CMD_IK_NG
    } else {
        UINT32 VinID = 0xDEADBEEFU;
        UINT32 PRetVal;
        void  *pMemBuf = pMemCtrl->pBuf;
        UINT32 MemSize = pMemCtrl->BufSize;
        AMBA_IK_MODE_CFG_s ImgMode = { .ContextId = IkCtxID };
        AMBA_IK_ABILITY_s ImgAbility;

        PRN_CMD_IK_LOG "Load ituner with IkCtxID(%d)"
            PRN_CMD_IK_ARG_UINT32 ImgMode.ContextId PRN_CMD_IK_ARG_POST
        PRN_CMD_IK_API

        AmbaSvcWrap_MisraMemset(&ImgAbility, 0, sizeof(ImgAbility));
        PRetVal = AmbaIK_GetContextAbility(&ImgMode, &ImgAbility);
        if (PRetVal != 0U) {
            PRN_CMD_IK_LOG "Fail to load ituner - get image ability! ErrCode(0x%08X)"
                PRN_CMD_IK_ARG_UINT32 PRetVal PRN_CMD_IK_ARG_POST
            PRN_CMD_IK_NG
        } else {
            // Need to pause image framework to avoid video ituner loading process be interrupted by ik execute.
            if (ImgAbility.Pipe == AMBA_IK_PIPE_VIDEO) {
                PRetVal = SvcResCfg_GetVinIDOfFovIdx(ImgMode.ContextId, &VinID); PRN_CMD_IK_ERR_HDLR
            }
        }

        // Change the tuner module mode as TEXT
        PRetVal = AmbaItn_Change_Parser_Mode(TEXT_TUNE); PRN_CMD_IK_ERR_HDLR

        // Initial ituner module
        PRetVal = AmbaItn_Init(pMemBuf, MemSize);
        if (TUNE_OK != PRetVal) {
            PRN_CMD_IK_LOG "Fail to load ituner - initial tuner module fail! ErrCode(0x%08X)"
                PRN_CMD_IK_ARG_UINT32 PRetVal PRN_CMD_IK_ARG_POST
            PRN_CMD_IK_NG
        }

        // Load the idsp setting from ituner file
        if (PRetVal == SVC_OK) {
            AMBA_ITN_Load_Param_s Ituner_LoadParam;

            AmbaSvcWrap_MisraMemset(&Ituner_LoadParam, 0, sizeof(Ituner_LoadParam));
            Ituner_LoadParam.Text.FilePath = pFileName;

            PRetVal = AmbaItn_Load_IDSP(&Ituner_LoadParam);
            if (TUNE_OK != PRetVal) {
                PRN_CMD_IK_LOG "Fail to load ituner - load ituner! ErrCode(0x%08X)"
                    PRN_CMD_IK_ARG_UINT32 PRetVal PRN_CMD_IK_ARG_POST
                PRN_CMD_IK_NG
            }
        }

        // Execute idsp
        if (PRetVal == SVC_OK) {

#ifdef CONFIG_BUILD_IMGFRW_AAA
            // Disable Image Framework Sync task
            if (VinID < AMBA_NUM_VIN_CHANNEL) {
                SvcImg_SyncEnable(VinID, 0U);
                PRetVal = AmbaKAL_TaskSleep(100U); PRN_CMD_IK_ERR_HDLR
            }
#endif

            PRetVal = AmbaItn_Execute_IDSP(&ImgMode);
            if (TUNE_OK != PRetVal) {
                PRN_CMD_IK_LOG "Fail to load ituner - execute idsp! ErrCode(0x%08X)"
                    PRN_CMD_IK_ARG_UINT32 PRetVal PRN_CMD_IK_ARG_POST
                PRN_CMD_IK_NG
            }

#ifdef CONFIG_BUILD_IMGFRW_AAA
            // Enable Image Framework Sync task
            if (VinID < AMBA_NUM_VIN_CHANNEL) {
                SvcImg_SyncEnable(VinID, 1U);
            }
#endif
        }
    }
#else
    PRN_CMD_IK_LOG "Enable ituner module first" PRN_CMD_IK_API
#endif

    AmbaMisra_TouchUnused(&IkCtxID);
    AmbaMisra_TouchUnused(pFileName);
}

void SvcCmdIK_SaveIsoCfg(UINT32 IkCtxID, UINT32 PreCnt, const char *pFileName)
{
    const SVC_CMD_IK_MEM_CTRL_s *pMemCtrl = &(SvcCmdIKCtrl.MemCtrl[SVC_CMD_IK_ITUNER_MEM]);

    if (IkCtxID >= MAX_CONTEXT_NUM) {
        PRN_CMD_IK_LOG "Fail to save ik cr - invalid IkCtxID(%d)" PRN_CMD_IK_ARG_UINT32 IkCtxID PRN_CMD_IK_ARG_POST PRN_CMD_IK_NG
    } else if (pFileName == NULL) {
        PRN_CMD_IK_LOG "Fail to save ik cr - filename should not null" PRN_CMD_IK_NG
    } else if (pMemCtrl->Enable == 0U) {
        PRN_CMD_IK_LOG "Fail to save ik cr - invalid buffer setting" PRN_CMD_IK_NG
    } else {
        UINT32 PRetVal;
        AMBA_IK_MODE_CFG_s ImgMode = { .ContextId = IkCtxID };
        UINT8 *pCurBuf    = NULL;
        UINT32 CurBufSize = 0U;
        UINT32 IkCrSize;
        UINT32 ReqMemType = SVC_MEM_TYPE_MAX;
        SIZE_t ReqSize;

        PRN_CMD_IK_LOG "Save IK CR with IkCtxID(%d), PreCnt(%d)"
            PRN_CMD_IK_ARG_UINT32 ImgMode.ContextId PRN_CMD_IK_ARG_POST
            PRN_CMD_IK_ARG_UINT32 PreCnt PRN_CMD_IK_ARG_POST
        PRN_CMD_IK_API

#if defined(CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
        PRetVal = AmbaIK_QueryDebugConfigSize(&(SvcCmdIKCtrl.ImgMode), &ReqSize);
#else
        PRetVal = AmbaIK_QueryDebugConfigSize(&ReqSize);
#endif
        AmbaMisra_TouchUnused(&ReqSize);
        if (PRetVal != 0U) {
            PRN_CMD_IK_LOG "Fail to save ik cr - get IK CR data size! ErrCode(0x%08X)"
                PRN_CMD_IK_ARG_UINT32 PRetVal PRN_CMD_IK_ARG_POST
            PRN_CMD_IK_NG
        } else {
            IkCrSize = (UINT32)ReqSize;

            if (SvcCmdIKCtrl.MemCtrl[SVC_CMD_IK_DUMP_MEM].Enable == 0U) {
                SvcCmdIK_MemAllocate(IkCrSize, &ReqMemType, &pCurBuf, &CurBufSize);
            } else {
                pCurBuf    = SvcCmdIKCtrl.MemCtrl[SVC_CMD_IK_DUMP_MEM].pBuf;
                CurBufSize = SvcCmdIKCtrl.MemCtrl[SVC_CMD_IK_DUMP_MEM].BufSize;
            }

            if ((pCurBuf == NULL) || (CurBufSize == 0U)) {
                PRN_CMD_IK_LOG "Fail to save ik cr - invalid memory setting!" PRN_CMD_IK_NG
            } else if (CurBufSize < IkCrSize) {
                PRN_CMD_IK_LOG "Fail to save ik cr - dump memory size is too small! IkCr(0x%X) > CurBuf(0x%X)"
                    PRN_CMD_IK_ARG_UINT32 IkCrSize PRN_CMD_IK_ARG_POST
                    PRN_CMD_IK_ARG_UINT32 CurBufSize PRN_CMD_IK_ARG_POST
                PRN_CMD_IK_NG
            } else {
                SIZE_t DumpBufSize;
#ifdef CONFIG_ARM64
                UINT64 TmpU64 = (UINT64)IkCrSize;
                TmpU64 &= 0xFFFFFFFFU;
                DumpBufSize = (SIZE_t)TmpU64;
#else
                DumpBufSize = (SIZE_t)IkCrSize;
#endif

                PRetVal = AmbaIK_DumpDebugConfig(&ImgMode, PreCnt, pCurBuf, DumpBufSize);
                if (PRetVal != 0U) {
                    PRN_CMD_IK_LOG "Fail to save ik cr. ErrCode(0x%08X)"
                        PRN_CMD_IK_ARG_UINT32 PRetVal PRN_CMD_IK_ARG_POST
                    PRN_CMD_IK_NG
                } else {
                    if (SVC_OK == SvcCmdIK_CmdDumpBinSave(pCurBuf, IkCrSize, pFileName)) {
                        PRN_CMD_IK_LOG "Success to save ik cr to %s"
                            PRN_CMD_IK_ARG_CSTR pFileName PRN_CMD_IK_ARG_POST
                            PRN_CMD_IK_OK
                    }
                }
            }

            if (SvcCmdIKCtrl.MemCtrl[SVC_CMD_IK_DUMP_MEM].Enable == 0U) {
                SvcCmdIK_MemRelease(ReqMemType);
            }
        }
    }
}

void SvcCmdIK_SaveIdspCfg(UINT32 ViewZoneID, UINT32 DumpLevel, UINT32 CtrlFlg, char *pDumpPath)
{
    if ((ViewZoneID < AMBA_DSP_MAX_VIEWZONE_NUM) && (pDumpPath != NULL)) {
        UINT32 PRetVal;
        UINT8 *pCurBuf = NULL;
        UINT32 CurBufSize = 0U;
        UINT32 ReqMemSize = SVC_CMD_IK_IDSP_CFG_SIZE + SVC_CMD_IK_IDSP_HEADER_SIZE;
        UINT32 ReqMemType = SVC_MEM_TYPE_MAX;

        if (SvcCmdIKCtrl.MemCtrl[SVC_CMD_IK_DUMP_MEM].Enable == 0U) {
            SvcCmdIK_MemAllocate(ReqMemSize, &ReqMemType, &pCurBuf, &CurBufSize);
        } else {
            pCurBuf    = SvcCmdIKCtrl.MemCtrl[SVC_CMD_IK_DUMP_MEM].pBuf;
            CurBufSize = SvcCmdIKCtrl.MemCtrl[SVC_CMD_IK_DUMP_MEM].BufSize;
        }

        if ((pCurBuf == NULL) || (CurBufSize == 0U)) {
            PRN_CMD_IK_LOG "Fail to save ucode cr - invalid memory setting!" PRN_CMD_IK_NG
        } else if (CurBufSize < ReqMemSize) {
            PRN_CMD_IK_LOG "Fail to save ucode cr - dump memory size is too small!" PRN_CMD_IK_NG
        } else {
            UINT8 *pIdspBuf       = pCurBuf;
            UINT32 IdspBufSize    = SVC_CMD_IK_IDSP_CFG_SIZE + SVC_CMD_IK_IDSP_HEADER_SIZE;
            UINT8 *pCurIdspCfg    = NULL;
            UINT8 *pDumpBinBuf    = &(pIdspBuf[IdspBufSize]);
            UINT32 DumpBinBufSize = CurBufSize - IdspBufSize;
            SVC_CMD_IK_IDSP_CFG CfgInfo;

            PRN_CMD_IK_LOG "Save uCode CR with ViewZoneID(%d), DumpLevel(%d), CtrlFlg(0x%X)"
                PRN_CMD_IK_ARG_UINT32 ViewZoneID PRN_CMD_IK_ARG_POST
                PRN_CMD_IK_ARG_UINT32 DumpLevel  PRN_CMD_IK_ARG_POST
                PRN_CMD_IK_ARG_UINT32 CtrlFlg    PRN_CMD_IK_ARG_POST
            PRN_CMD_IK_API

            AmbaSvcWrap_MisraMemset(&CfgInfo, 0, sizeof(CfgInfo));
            CfgInfo.pMode    = &(SvcCmdIKCtrl.ImgMode);
            CfgInfo.pBuf     = pIdspBuf;
            CfgInfo.BufSize  = IdspBufSize;
            CfgInfo.pCfgAddr = &pCurIdspCfg;

            PRetVal = SvcCmdIK_GetIdspCfg(DumpLevel, CtrlFlg, &CfgInfo);
            if (PRetVal != 0U) {
                PRN_CMD_IK_LOG "Fail to save ucode cr - get idsp cfg fail!" PRN_CMD_IK_NG
            } else {
                char *pFilePrefix = pDumpPath;
                char  IdspCfgFileName[128];

                // Create the saving folder if exist
                PRetVal = SvcCmdIK_CmdDumpFolderCreate(pFilePrefix); PRN_CMD_IK_ERR_HDLR

                // Configure cfg file pathg
                AmbaSvcWrap_MisraMemset(IdspCfgFileName, 0, sizeof(IdspCfgFileName));
                AmbaUtility_StringCopy(IdspCfgFileName, sizeof(IdspCfgFileName), pFilePrefix);
                AmbaUtility_StringAppend(IdspCfgFileName, (UINT32)sizeof(IdspCfgFileName), "_idsp_cfg.bin");

                // Save cfg file
                PRetVal = SvcCmdIK_CmdDumpBinSave(pCurBuf, SVC_CMD_IK_IDSP_CFG_SIZE + 4U, IdspCfgFileName);
                if (PRetVal != SVC_OK) {
                    PRN_CMD_IK_LOG "Fail to save ucode cr - save cfg file fail" PRN_CMD_IK_NG
                } else {
                    PRN_CMD_IK_LOG "Success to save ucode cr - %s"
                        PRN_CMD_IK_ARG_CSTR IdspCfgFileName PRN_CMD_IK_ARG_POST
                        PRN_CMD_IK_OK
                }

                // Dump the idsp bin
                if ((SvcCmdIKCtrl.NumOfIdspDumpFunc > 0U) && (SvcCmdIKCtrl.pIdspDumpFunc != NULL)) {
                    if (DumpLevel == SvcCmdIKCtrl.pIdspDumpFunc[0].Level) {
                        if (SvcCmdIKCtrl.pIdspDumpFunc[0].pFunc != NULL) {

                            PRN_CMD_IK_LOG "---- devel level %d ----"
                                PRN_CMD_IK_ARG_UINT32 SvcCmdIKCtrl.pIdspDumpFunc[0].Level PRN_CMD_IK_ARG_POST
                            PRN_CMD_IK_API

                            (SvcCmdIKCtrl.pIdspDumpFunc[0].pFunc) (pCurIdspCfg, pDumpBinBuf, DumpBinBufSize, &(SvcCmdIKCtrl.pIdspDumpFunc[0]), pFilePrefix);
                        } else {
                            PRN_CMD_IK_LOG "Success to save ucode cr - No.0 dump info function should not null" PRN_CMD_IK_NG
                        }
                    } else {
                        UINT32 DumpFuncIdx;

                        for (DumpFuncIdx = 1U; DumpFuncIdx < SvcCmdIKCtrl.NumOfIdspDumpFunc; DumpFuncIdx ++) {
                            PRN_CMD_IK_LOG "---- devel level %d, %d ----"
                                PRN_CMD_IK_ARG_UINT32 DumpFuncIdx                                   PRN_CMD_IK_ARG_POST
                                PRN_CMD_IK_ARG_UINT32 SvcCmdIKCtrl.pIdspDumpFunc[DumpFuncIdx].Level PRN_CMD_IK_ARG_POST
                            PRN_CMD_IK_API

                            if ( SvcCmdIKCtrl.pIdspDumpFunc[DumpFuncIdx].pFunc != NULL ) {
                                if ((DumpLevel & SvcCmdIKCtrl.pIdspDumpFunc[DumpFuncIdx].Level ) > 0U) {
                                    (SvcCmdIKCtrl.pIdspDumpFunc[DumpFuncIdx].pFunc)(pCurIdspCfg, pDumpBinBuf, DumpBinBufSize, &(SvcCmdIKCtrl.pIdspDumpFunc[DumpFuncIdx]), pFilePrefix);
                                }
                            }
                        }
                    }
                }
            }

        }

        if (SvcCmdIKCtrl.MemCtrl[SVC_CMD_IK_DUMP_MEM].Enable == 0U) {
            SvcCmdIK_MemRelease(ReqMemType);
        }

    }
}

/**
 * Install ik shell command entry
 *
 */
void SvcCmdIK_Install(void)
{
    AMBA_SHELL_COMMAND_s SvcCmdIK;
    UINT32 Rval, Idx;

    AmbaSvcWrap_MisraMemset(&SvcCmdIKCtrl, 0, sizeof(SvcCmdIKCtrl));
    Rval = SvcCmdIK_DumpIdspInit(&(SvcCmdIKCtrl.NumOfIdspDumpFunc), &(SvcCmdIKCtrl.pIdspDumpFunc));

    if (Rval == 0xCafeU) {
        PRN_CMD_IK_LOG " " PRN_CMD_IK_API
        PRN_CMD_IK_LOG "---- IDSP Dump Function ----" PRN_CMD_IK_API
        PRN_CMD_IK_LOG "  NumOfIdspDumpFunc : %d"
            PRN_CMD_IK_ARG_UINT32 SvcCmdIKCtrl.NumOfIdspDumpFunc PRN_CMD_IK_ARG_POST
        PRN_CMD_IK_API
        PRN_CMD_IK_LOG "      pIdspDumpFunc : %p"
            PRN_CMD_IK_ARG_CPOINT SvcCmdIKCtrl.pIdspDumpFunc PRN_CMD_IK_ARG_POST
        PRN_CMD_IK_API
    }

    if (SvcCmdIKCtrl.pIdspDumpFunc != NULL) {
        UINT32 TempU32;

        if (Rval == 0xCafeU) {
            PRN_CMD_IK_LOG "          Idx |  DumpLevel | Info Number | Info Array | pFunc" PRN_CMD_IK_API
        }

        for (Idx = 0U; Idx < SvcCmdIKCtrl.NumOfIdspDumpFunc; Idx ++) {
            if (SvcCmdIKCtrl.pIdspDumpFunc[Idx].pFunc == NULL) {
                SvcCmdIKCtrl.pIdspDumpFunc[Idx].pFunc = SvcCmdIK_CmdDumpIdspProc;
            }

            if (Rval == 0xCafeU) {
                AmbaMisra_TypeCast(&(TempU32), &(SvcCmdIKCtrl.pIdspDumpFunc[Idx].pFunc));

                PRN_CMD_IK_LOG "           %02d | 0x%08x |  0x%08x | %p | 0x%08x"
                    PRN_CMD_IK_ARG_UINT32 Idx                                       PRN_CMD_IK_ARG_POST
                    PRN_CMD_IK_ARG_UINT32 SvcCmdIKCtrl.pIdspDumpFunc[Idx].Level     PRN_CMD_IK_ARG_POST
                    PRN_CMD_IK_ARG_UINT32 SvcCmdIKCtrl.pIdspDumpFunc[Idx].NumOfInfo PRN_CMD_IK_ARG_POST
                    PRN_CMD_IK_ARG_CPOINT SvcCmdIKCtrl.pIdspDumpFunc[Idx].pInfo     PRN_CMD_IK_ARG_POST
                    PRN_CMD_IK_ARG_UINT32 TempU32                                   PRN_CMD_IK_ARG_POST
                PRN_CMD_IK_API
            }
        }
    }

    AmbaSvcWrap_MisraMemset(&SvcCmdIK, 0, sizeof(SvcCmdIK));
    SvcCmdIK.pName    = "svc_ik";
    SvcCmdIK.MainFunc = SvcCmdIK_CmdEntry;
    SvcCmdIK.pNext    = NULL;

    Rval = SvcCmd_CommandRegister(&SvcCmdIK);
    if (SHELL_ERR_SUCCESS != Rval) {
        PRN_CMD_IK_LOG "## fail to install svc shell ik command" PRN_CMD_IK_API
    }
}

