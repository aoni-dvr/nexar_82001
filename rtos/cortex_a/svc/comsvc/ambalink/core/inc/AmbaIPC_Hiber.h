/**
 * @file AmbaIPC_Hiber.h
 *
 * Copyright (c) 2020 Ambarella International LP
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
 * @details Definitions for Hibernation.
 *
 */

#ifndef _AMBA_IPC_HIBER_H_
#define _AMBA_IPC_HIBER_H_


extern UINT32 LinuxSuspendMode;

#define LINUX_IS_SUSPEND_TO_DISK_MODE()  (LinuxSuspendMode == AMBA_LINK_SUSPEND_TO_DISK)
#define LINUX_IS_SUSPEND_TO_RAM_MODE()   (LinuxSuspendMode == AMBA_LINK_SUSPEND_TO_RAM)

#define LINUX_IS_HIBER_MODE()            (LinuxSuspendMode == AMBA_LINK_SUSPEND_TO_DISK)
#define LINUX_IS_STANDBY_MODE()          (LinuxSuspendMode == AMBA_LINK_SUSPEND_TO_RAM)

/*-----------------------------------------------------------------------------------------------*\
 * Definition of Macros
\*-----------------------------------------------------------------------------------------------*/
#define AMBA_HIBER_AOSS_MAGIC                   0x19531110
#define AMBA_HIBER_AOSS_OS_STACK_SIZE           (72)
#define AMBA_HIBER_SAVING_MAGIC                    (0x55aaaa55)

/*-----------------------------------------------------------------------------------------------*\
 * Definition of Aoss structures
 * Below structures have to sync with aoss.S and linux kernel pm.c
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_HIBER_PAGE_INFO_s_ {
    UINT64      Src;
    UINT64      Dst;
    UINT32      Size;
} AMBA_HIBER_PAGE_INFO_s;

typedef union _AMBA_HIBER_AOSS_PARAM_u_ {
    UINT32  Data[4];

    struct {
        UINT32  In;
        UINT32  Post;
        UINT32  Out;
        UINT32  Rsv;
    } Func;

    struct {
        UINT32  Rsv0;
        UINT32  Rsv1;
        UINT32  Lnx;
        UINT32  Rfs;
    } Crc;

    struct {
        UINT32  ImgSize;
        UINT32  ImgCrc;
        UINT32  Magic;
        UINT32  HeaderBlkNum;
    } HiberInfo;

} AMBA_HIBER_AOSS_PARAM_u;

typedef struct _AMBA_AOSS_OS_INFO_s_ {
    UINT32              R0;
    UINT32              R1;
    UINT32              R2;
    UINT32              R3;
    UINT32              R4;
    UINT32              R5;
    UINT32              R6;
    UINT32              R7;
    UINT32              R8;
    UINT32              R9;
    UINT32              R10;
    UINT32              R11;
    UINT32              R12;
    UINT32              Sp;
    UINT32              Lr;
    UINT32              Pc;
    UINT32              Cpsr;

    UINT32              Usr_Sp;
    UINT32              Usr_Lr;

    UINT32              Svr_Sp;
    UINT32              Svr_Lr;
    UINT32              Svr_Spsr;

    UINT32              Abt_Sp;
    UINT32              Abt_Lr;
    UINT32              Abt_Spsr;

    UINT32              Irq_Sp;
    UINT32              Irq_Lr;
    UINT32              Irq_Spsr;

    UINT32              Und_Sp;
    UINT32              Und_Lr;
    UINT32              Und_Spsr;

    UINT32              Fiq_Sp;
    UINT32              Fiq_Lr;
    UINT32              Fiq_Spsr;
    UINT32              Fiq_R8;
    UINT32              Fiq_R9;
    UINT32              Fiq_R10;
    UINT32              Fiq_R11;
    UINT32              Fiq_R12;

    UINT32              Cp15_Sctlr;         /* c1,  c0, 0 */
    UINT32              Cp15_Dacr;          /* c3,  c0, 0 */
    UINT32              Cp15_Ttbr0;         /* c2,  c0, 0 */
    UINT32              Cp15_Fcseidr;       /* c13, c0, 0 */
    UINT32              Cp15_Actlr;         /* c1,  c0, 1 */
    UINT32              Cp15_Cpacr;         /* c1,  c0, 2 */
    UINT32              Cp15_Ttbr1;         /* c2,  c0, 1 */
    UINT32              Cp15_Contextidr;    /* c13, c0, 1 */
    UINT32              Rsv_Lcp15[2];
    UINT32              Cp15_Diagr;         /* c15, c0, 1 */
    UINT32              Cp15_Prrr;          /* c10, c2, 0 */
    UINT32              Cp15_Nmrr;          /* c10, c2, 1 */

    UINT32              Stack[AMBA_HIBER_AOSS_OS_STACK_SIZE];
} AMBA_AOSS_OS_INFO_s;

typedef struct _AMBA_HIBER_AOSS_INFO_Full_s_ {
    AMBA_HIBER_AOSS_PARAM_u     Param;
    AMBA_AOSS_OS_INFO_s         Host;
    AMBA_AOSS_OS_INFO_s         Client;
    UINT32                      Magic;
    UINT32                      TotalPages;
    UINT32                      CopyPages;
    AMBA_HIBER_PAGE_INFO_s      *pPageInfo;
} AMBA_HIBER_AOSS_INFO_Full_s;

typedef struct _AMBA_HIBER_AOSS_INFO_Simple_s_ {
    UINT32                      Info[252];
    UINT32                      Magic;
    UINT32                      TotalPages;
    UINT32                      CopyPages;
    AMBA_HIBER_PAGE_INFO_s      *pPageInfo;
} AMBA_HIBER_AOSS_INFO_Simple_s;

typedef union _AMBA_HIBER_AOSS_INFO_u_ {
    AMBA_HIBER_AOSS_INFO_Full_s     Full;
    AMBA_HIBER_AOSS_INFO_Simple_s   Simple;
} AMBA_HIBER_AOSS_INFO_u;

typedef struct _AMBA_HIBER_PREPARE_INFO_s_ {
    AMBA_HIBER_AOSS_INFO_u          *AossInfo;
} AMBA_HIBER_PREPARE_INFO_s;


typedef struct _AMBA_HIBER_PART_INFO_s_ {
    UINT32  StartBlock;
    UINT32  EndBlock;
    UINT32  MainSize;
    UINT32  PagesPerBlock;
} AMBA_HIBER_PART_INFO_s;
/*-----------------------------------------------------------------------------------------------*\
 * Definition of Hiber structures
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_HIBER_s_ {
    UINT64      AossAddr;
    UINT32      AossSize;
} AMBA_HIBER_s;


typedef INT32 (*HIBER_WRITE_HEADER_f) (AMBA_HIBER_PART_INFO_s *pPartInfo,
                                       AMBA_HIBER_AOSS_INFO_u *pAossInfo, UINT32 AossSize);
typedef INT32 (*HIBER_WRITE_DATA_PAGE_f) (UINT32 DataStartBlk, AMBA_HIBER_PART_INFO_s *pPartInfo,
        AMBA_HIBER_AOSS_INFO_u *pAossInfo, UINT32 AossSize);
typedef INT32 (*HIBER_WRITE_HEADER2_f) (UINT32 HeaderBlk, AMBA_HIBER_PART_INFO_s *pPartInfo,
                                        AMBA_HIBER_AOSS_INFO_u *pAossInfo, UINT32 AossSize);
typedef INT32 (*HIBER_SAVE_RPMSG_f) (AMBA_HIBER_AOSS_INFO_u *pAossInfo, AMBA_HIBER_PART_INFO_s *pPartInfo,
                                     UINT8 *pData, UINT32 Size);
typedef INT32 (*HIBER_READ_HEADER_f) (AMBA_HIBER_PART_INFO_s *pPartInfo, AMBA_HIBER_AOSS_INFO_u *pAossInfo,
                                      UINT32 AossSize);
typedef INT32 (*HIBER_READ_HEADER2_f) (AMBA_HIBER_PART_INFO_s *pPartInfo, AMBA_HIBER_AOSS_INFO_u *pAossInfo,
                                       UINT32 Block, UINT32 OpPages);
typedef INT32 (*HIBER_CHECK_CRC_f) (AMBA_HIBER_PART_INFO_s *pPartInfo, AMBA_HIBER_AOSS_INFO_u *pAossInfo);
typedef INT32 (*HIBER_READ_DATA_PAGE_f) (UINT32 DataStartBlk, AMBA_HIBER_PART_INFO_s *pPartInfo,
        AMBA_HIBER_AOSS_INFO_u *pAossInfo, UINT32 AossSize);
typedef INT32 (*HIBER_RESTORE_RPMSG_f) (AMBA_HIBER_AOSS_INFO_u *pAossInfo, AMBA_HIBER_PART_INFO_s *pPartInfo,
                                        UINT8 *pData, UINT32 Size);

typedef struct _AMBA_IPC_HIBER_CTRL_s_ {
    HIBER_WRITE_HEADER_f        HiberWriteHeader;
    HIBER_WRITE_HEADER2_f       HiberWriteHeader2;
    HIBER_WRITE_DATA_PAGE_f     HiberWriteDataPage;
    HIBER_SAVE_RPMSG_f          HiberSaveRpmsg;

    HIBER_READ_HEADER_f         HiberReadHeader;
    HIBER_READ_HEADER2_f        HiberReadHeader2;
    HIBER_CHECK_CRC_f           HiberCheckCrc;
    HIBER_READ_DATA_PAGE_f      HiberReadDataPage;
    HIBER_RESTORE_RPMSG_f       HiberRestoreRpmsg;

} AMBA_IPC_HIBER_CTRL_s;
/*-----------------------------------------------------------------------------------------------*\
 * Declaration of functrions
\*-----------------------------------------------------------------------------------------------*/
void AmbaIPC_HiberInitNVMCtrl(void);
INT32 AmbaIPC_HiberSaveRpmsgInfo(UINT8 *pData, UINT32 Size);
INT32 AmbaIPC_HiberRestoreRpmsgInfo(UINT8 *pData, UINT32 Size);
void AmbaIPC_HiberRestoreRpmsgFinish(void);
INT32 AmbaIPC_HiberPrepare(void *pInfo);
void AmbaIPC_HiberSuspend(UINT32 Flag);
INT32 AmbaIPC_HiberResume(UINT32 Flag);
void AmbaIPC_HiberWakeupRemote(void);
INT32 AmbaIPC_HiberReturn(UINT32 SuspendMode);
INT32 AmbaIPC_HiberWipeout(UINT32 Flag);
INT32 AmbaIPC_HiberDump(const char *Path);
void AmbaIPC_HiberBootSetup(void);
UINT32 AmbaIPC_HiberInit(AMBA_RPDEV_LINK_CTRL_OBJ_s *pObj, AMBA_KAL_MSG_QUEUE_t *pMsgQueue);

void AmbaIPC_HiberResetSuspendBackupSize(void);

#endif
