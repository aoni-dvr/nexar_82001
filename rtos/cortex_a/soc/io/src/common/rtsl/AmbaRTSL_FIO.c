/*
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
 */

#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaMisraFix.h"

#include "AmbaINT_Def.h"
#ifndef CONFIG_QNX
#include "AmbaRTSL_GIC.h"
#include "AmbaRTSL_Cache.h"
#include "AmbaRTSL_MMU.h"
#include "AmbaRTSL_PLL.h"
#else
#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>

#include "AmbaCache.h"
#include "AmbaMMU.h"
#endif

#include "AmbaRTSL_NAND.h"
#include "AmbaRTSL_NAND_Ctrl.h"
#include "AmbaRTSL_FIO.h"
#include "AmbaCSL_NAND.h"
#include "AmbaCSL_FIO.h"
#include "AmbaCSL_DMA.h"

#if defined(CONFIG_DEVICE_TREE_SUPPORT)
#include <AmbaIOUtility.h>
#endif

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
#include "AmbaCortexA76.h"
#else
#include "AmbaCortexA53.h"
#endif

#ifdef USE_FIODMA_DESCRIPTORS
AMBA_FDMA_DESCRIPTOR_s _AmbaFIO_Desc
GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;
#endif
#ifndef CONFIG_QNX
static void FIO_NandCmdISR(UINT32 IntID, UINT32 IsrArg);
#endif
void (*AmbaRTSL_FioNandCmdIsrCallBack)(void) = NULL;

#ifdef CONFIG_QNX
#define FDMA_WORK_BUF_MAIN_SIZE  (64U * 4U * 1024U)
#define FDMA_WORK_BUF_SPARE_SIZE (64U * 256U)
#else
/* Work Buffer for 1 Block. Due to the FDMA 8-Byte Boundary issue */
static UINT8 FdmaWorkBufMain[64 * 4 * 1024]
GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;

static UINT8 FdmaWorkBufSpare[64 * 256]
GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;
#endif

AMBA_RTSL_FIO_CTRL_s AmbaRTSL_FioCtrl = {
    .FdmaMainStatus  = 0,
    .FdmaSpareStatus = 0,
    .FioDmaStatus    = 0,
    .EccStatus       = 0,
#ifndef CONFIG_QNX
    .pWorkBufMain = &(FdmaWorkBufMain[0]),      /* pointer to the Work Buffer for Main area */
    .pWorkBufSpare = &(FdmaWorkBufSpare[0]),    /* pointer to the Work Buffer for Spare area */
#else
    .pWorkBufMain = NULL,      /* pointer to the Work Buffer for Main area */
    .pWorkBufSpare = NULL,    /* pointer to the Work Buffer for Spare area */
#endif
    .FdmaCtrl = {0},
};

#ifdef CONFIG_QNX
int get_fio_work_buf(ULONG *pAddr, UINT32 Size)
{
    int Rval = 0;
    struct posix_typed_mem_info info;
    void *virt_addr;
    int MemFd;
    ULONG Addr;

    MemFd = posix_typed_mem_open("/ram/fio_work", O_RDWR, POSIX_TYPED_MEM_ALLOCATE_CONTIG);
    if (MemFd < 0) {
        printf("[%s] get_fio_work_buf() : posix_typed_mem_open fail(/ram/fio_work) fd = %d",__FUNCTION__, MemFd);
    } else {
        Rval = posix_typed_mem_get_info(MemFd, &info);
        if (Rval < 0) {
            printf("[%s] get_fio_work_buf() : posix_typed_mem_get_info fail Rval = %d",__FUNCTION__, Rval);
        } else {
            virt_addr = mmap(NULL, Size, PROT_READ | PROT_WRITE, MAP_SHARED, MemFd, 0);
            if (virt_addr == MAP_FAILED) {
                printf("[%s] ut_get_dsp_work_buf() : mmap fail",__FUNCTION__);
            } else {
                (void)AmbaMisra_TypeCast(pAddr, &virt_addr);
            }
        }
    }
    return Rval;
}
#endif

/*
 *  @RoutineName:: AmbaRTSL_FioInit
 *
 *  @Description:: Initialize the FIO controller
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
void AmbaRTSL_FioInit(void)
{
#ifndef CONFIG_QNX
    ULONG base_addr;

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    base_addr = AMBA_CA53_FLASH_CPU_BASE_ADDR;
#elif defined (CONFIG_SOC_CV5)|| defined (CONFIG_SOC_CV52)
    base_addr = AMBA_CORTEX_A76_FLASH_CPU_BASE_ADDR;
#else
    base_addr = AMBA_CORTEX_A53_FLASH_CPU_BASE_ADDR;
#endif
    AmbaMisra_TypeCast(&pAmbaFIO_Reg, &base_addr);

#if defined(CONFIG_DEVICE_TREE_SUPPORT)
    base_addr = IO_UtilityFDTPropertyU32Quick(0, "ambarella,nand", "reg", 0U);
    if (base_addr != 0U) {
#if defined (CONFIG_SOC_CV5)|| defined (CONFIG_SOC_CV52)
        base_addr = base_addr | AMBA_CORTEX_A76_AHB_PHYS_BASE_ADDR;
#endif
        AmbaMisra_TypeCast(&pAmbaFIO_Reg, &base_addr);
    }
#endif

#else
    ULONG Addr;

    get_fio_work_buf(&Addr, FDMA_WORK_BUF_MAIN_SIZE);
    AmbaMisra_TypeCast(&AmbaRTSL_FioCtrl.pWorkBufMain, &Addr);

    get_fio_work_buf(&Addr, FDMA_WORK_BUF_SPARE_SIZE);
    AmbaMisra_TypeCast(&AmbaRTSL_FioCtrl.pWorkBufSpare, &Addr);
#endif
}

void AmbaRTSL_FioInitInt(void)
{
#ifndef CONFIG_QNX
    AMBA_INT_CONFIG_s IntConfig;
    UINT32 k;

    IntConfig.TriggerType = INT_TRIG_HIGH_LEVEL;
    IntConfig.IrqType     = INT_TYPE_FIQ;       /* Since Linux does not suuport GIC group1 interrupt handling, set it as FIQ for AmbaLink. */
    IntConfig.CpuTargets  = 0x01;               /* Target cores */
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    k = AMBA_INT_SPI_ID104_FIO;
#elif defined (CONFIG_SOC_CV5)|| defined (CONFIG_SOC_CV52)
    k = AMBA_INT_SPI_ID100_FIO;
#else
    k = AMBA_INT_SPI_ID100_FIO;
#endif
    AmbaMisra_TouchUnused(&k);

#if defined(CONFIG_DEVICE_TREE_SUPPORT)
    k = IO_UtilityFDTPropertyU32Quick(0, "ambarella,nand", "interrupts", 1U) + 32U;
#endif
    (void)AmbaRTSL_GicIntConfig(k, &IntConfig, FIO_NandCmdISR, 0U);
    (void)AmbaRTSL_GicIntEnable(k);               /* Enable the Interrupt */
#endif
}

#ifdef USE_FIODMA_DESCRIPTORS
/*
 *  @RoutineName:: AmbaRTSL_FdmaReadSetupDesc
 *
 *  @Description:: Setup FDMA for reading data from DMA-FIFO with desc mode
 *
 *  @Input      ::
 *      MainDataSize:  Transfer count (in Byte) for main area
 *      pMainBuf:      DRAM buffer for main area data
 *      SpareDataSize: Transfer count (in Byte) for spare area
 *      pSpareBuf:     DRAM buffer for spare area data
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
void AmbaRTSL_FdmaReadSetupDesc(UINT32 PageAddr, UINT32 MainDataSize, UINT8 *pMainBuf, UINT32 SpareDataSize, UINT8 *pSpareBuf)
{
    AMBA_NAND_COMMON_INFO_s *pNandDevInfo = AmbaRTSL_NandCommonInfo;
    AMBA_FIO_CTRL_REG_s   FioCtrlRegVal;
    AMBA_FDMA_CTRL_REG_s  FdmaCtrlRegVal = {0};

    UINT64 SrcAddr = PageAddr * pNandDevInfo->MainByteSize;
    FioCtrlRegVal.Data = AmbaCSL_FioGetCtrlReg();

    AmbaCSL_FioFdmaClearMainStatus();
    if (AmbaWrap_memset(&_AmbaFIO_Desc, 0x0, sizeof(AMBA_FDMA_DESCRIPTOR_s)) != OK) { /* Do nothing */ };

    /* FDMA setup for Spare area */
    _AmbaFIO_Desc.pSpareAddr = pSpareBuf;

    /* FDMA setup for Main area */
    _AmbaFIO_Desc.pMainAddr = pMainBuf;

    _AmbaFIO_Desc.NandAddr = (UINT32)SrcAddr;
    _AmbaFIO_Desc.NandAddrHigh.Bits.AddrHigh = SrcAddr >> 32;

    _AmbaFIO_Desc.CpAddrHighXferLen.Bits.TransferCnt = MainDataSize + SpareDataSize;

    _AmbaFIO_Desc.FioCtrl.Bits.StopOnError = 1;
    _AmbaFIO_Desc.FioCtrl.Bits.IntOnError  = 1;
    _AmbaFIO_Desc.FioCtrl.Bits.IntOnDone   = 1;
    _AmbaFIO_Desc.FioCtrl.Bits.WriteMem    = 1;
    _AmbaFIO_Desc.FioCtrl.Bits.EndOfChain  = 1;
    _AmbaFIO_Desc.FioCtrl.Bits.BchEnable   = 1;
    _AmbaFIO_Desc.FioCtrl.Bits.WriteWithAutoStatusUpdate =  1;
    _AmbaFIO_Desc.FioCtrl.Bits.MainStrideSize  = AMBA_FDMA_DSM_STRIDE_SIZE_512B;

    _AmbaFIO_Desc.FioCtrl.Bits.SpareStrideSize = (FioCtrlRegVal.Bits.Bch8Bits) ?
            AMBA_FDMA_DSM_STRIDE_SIZE_32B :
            AMBA_FDMA_DSM_STRIDE_SIZE_16B;

    /* blk setting for FPGA must be 2~4. If DMA address is not 8-byte aligned, it has to be 2~3 */
    _AmbaFIO_Desc.FioCtrl.Bits.BusBlockSize   = (MainDataSize > 16) ? DMA_BUS_BLOCK_32BYTE :
            DMA_BUS_BLOCK_16BYTE;

    _AmbaFIO_Desc.CustomCmdWord.NandCustomCmdWord.Bits.Cmd1Val0 = 0x00;
    _AmbaFIO_Desc.CustomCmdWord.NandCustomCmdWord.Bits.Cmd2Val0 = 0x30;

    _AmbaFIO_Desc.CustomCmd.NandCustomCmd.Bits.WaitCycle           = AMBA_NAND_CMD_WAIT_RB;
    _AmbaFIO_Desc.CustomCmd.NandCustomCmd.Bits.ReadWriteType       = AMBA_NAND_CMD_RE_TYPE;
    _AmbaFIO_Desc.CustomCmd.NandCustomCmd.Bits.CmdPhase2Count      = AMBA_NAND_CMD_PHASE2_PRESENT;
    _AmbaFIO_Desc.CustomCmd.NandCustomCmd.Bits.AdressCycle         = 5;
    _AmbaFIO_Desc.CustomCmd.NandCustomCmd.Bits.CmdPhase1Count      = AMBA_NAND_CMD_PHASE1_PRESENT_CMD1;
    _AmbaFIO_Desc.CustomCmd.NandCustomCmd.Bits.AddrSrc             = 2;
    _AmbaFIO_Desc.CustomCmd.NandCustomCmd.Bits.Addr1NoIncrease     = 0;
    _AmbaFIO_Desc.CustomCmd.NandCustomCmd.Bits.Addr2NoIncrease     = 0;
    _AmbaFIO_Desc.CustomCmd.NandCustomCmd.Bits.DataSrcSelect       = AMBA_NAND_DATA_FROM_DMA_ENGINE;
    _AmbaFIO_Desc.CustomCmd.NandCustomCmd.Bits.ChipEnableTerminate = 1;

    (void)AmbaRTSL_CacheCleanDataPtr((void *) &_AmbaFIO_Desc, sizeof(AMBA_FDMA_DESCRIPTOR_s));

    AmbaCSL_FdmaSetDescAddr(&_AmbaFIO_Desc);

    FdmaCtrlRegVal.DescMode  = 1;
    FdmaCtrlRegVal.Enable    = 1;
    AmbaCSL_FdmaSetMainCtrlReg(FdmaCtrlRegVal);
}

/*
 *  @RoutineName:: AmbaRTSL_FdmaWriteSetupDesc
 *
 *  @Description:: Setup FDMA for reading data from DRAM and writing to DMA-FIFO
 *
 *  @Input      ::
 *      MainDataSize:  Transfer count (in Byte) for main area
 *      pMainBuf:      DRAM buffer for main area data
 *      SpareDataSize: Transfer count (in Byte) for spare area
 *      pSpareBuf:     DRAM buffer for spare area data
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
void AmbaRTSL_FdmaWriteSetupDesc(UINT32 PageAddr, UINT32 MainDataSize, UINT8 *pMainBuf, UINT32 SpareDataSize, UINT8 *pSpareBuf)
{
    AMBA_NAND_COMMON_INFO_s *pNandDevInfo = AmbaRTSL_NandCommonInfo;
    AMBA_FIO_CTRL_REG_s   FioCtrlRegVal;
    AMBA_FDMA_CTRL_REG_s  FdmaCtrlRegVal = {0};

    UINT64 SrcAddr = PageAddr * pNandDevInfo->MainByteSize;
    FioCtrlRegVal.Data = AmbaCSL_FioGetCtrlReg();

    AmbaCSL_FioFdmaClearMainStatus();
    if (AmbaWrap_memset(&_AmbaFIO_Desc, 0x0, sizeof(AMBA_FDMA_DESCRIPTOR_s)) != OK) { /* Do nothing */ };

    /* FDMA setup for Spare area */
    _AmbaFIO_Desc.pSpareAddr = pSpareBuf;

    /* FDMA setup for Main area */
    _AmbaFIO_Desc.pMainAddr = pMainBuf;

    _AmbaFIO_Desc.NandAddr = (UINT32)SrcAddr;
    _AmbaFIO_Desc.NandAddrHigh.Bits.AddrHigh = SrcAddr >> 32;

    _AmbaFIO_Desc.CpAddrHighXferLen.Bits.TransferCnt = MainDataSize + SpareDataSize;

    _AmbaFIO_Desc.FioCtrl.Bits.StopOnError = 1;
    _AmbaFIO_Desc.FioCtrl.Bits.IntOnError  = 1;
    _AmbaFIO_Desc.FioCtrl.Bits.IntOnDone   = 1;
    _AmbaFIO_Desc.FioCtrl.Bits.WriteMem    = 0;
    _AmbaFIO_Desc.FioCtrl.Bits.EndOfChain  = 1;
    _AmbaFIO_Desc.FioCtrl.Bits.BchEnable   = 1;
    _AmbaFIO_Desc.FioCtrl.Bits.WriteWithAutoStatusUpdate =  1;
    _AmbaFIO_Desc.FioCtrl.Bits.MainStrideSize  = AMBA_FDMA_DSM_STRIDE_SIZE_512B;

    _AmbaFIO_Desc.FioCtrl.Bits.SpareStrideSize = (FioCtrlRegVal.Bits.Bch8Bits) ?
            AMBA_FDMA_DSM_STRIDE_SIZE_32B :
            AMBA_FDMA_DSM_STRIDE_SIZE_16B;

    /* blk setting for FPGA must be 2~4. If DMA address is not 8-byte aligned, it has to be 2~3 */
    _AmbaFIO_Desc.FioCtrl.Bits.BusBlockSize   = (MainDataSize > 16) ? DMA_BUS_BLOCK_32BYTE :
            DMA_BUS_BLOCK_16BYTE;

    _AmbaFIO_Desc.CustomCmdWord.NandCustomCmdWord.Bits.Cmd1Val0 = 0x80;
    _AmbaFIO_Desc.CustomCmdWord.NandCustomCmdWord.Bits.Cmd2Val0 = 0x10;

    _AmbaFIO_Desc.CustomCmd.NandCustomCmd.Bits.WaitCycle           = AMBA_NAND_CMD_WAIT_RB;
    _AmbaFIO_Desc.CustomCmd.NandCustomCmd.Bits.ReadWriteType       = AMBA_NAND_CMD_WE_TYPE;
    _AmbaFIO_Desc.CustomCmd.NandCustomCmd.Bits.CmdPhase2Count      = AMBA_NAND_CMD_PHASE2_PRESENT;
    _AmbaFIO_Desc.CustomCmd.NandCustomCmd.Bits.AdressCycle         = 5;
    _AmbaFIO_Desc.CustomCmd.NandCustomCmd.Bits.CmdPhase1Count      = AMBA_NAND_CMD_PHASE1_PRESENT_CMD1;
    _AmbaFIO_Desc.CustomCmd.NandCustomCmd.Bits.AddrSrc             = 2;
    _AmbaFIO_Desc.CustomCmd.NandCustomCmd.Bits.Addr1NoIncrease     = 0;
    _AmbaFIO_Desc.CustomCmd.NandCustomCmd.Bits.Addr2NoIncrease     = 0;
    _AmbaFIO_Desc.CustomCmd.NandCustomCmd.Bits.DataSrcSelect       = AMBA_NAND_DATA_FROM_DMA_ENGINE;
    _AmbaFIO_Desc.CustomCmd.NandCustomCmd.Bits.ChipEnableTerminate = 1;

    (void)AmbaRTSL_CacheCleanDataPtr(&_AmbaFIO_Desc, sizeof(AMBA_FDMA_DESCRIPTOR_s));

    AmbaCSL_FdmaSetDescAddr(&_AmbaFIO_Desc);

    FdmaCtrlRegVal.DescMode  = 1;
    FdmaCtrlRegVal.Enable    = 1;
    AmbaCSL_FdmaSetMainCtrlReg(FdmaCtrlRegVal);
}
#endif

/*
 *  @RoutineName:: AmbaCSL_FdmaReadSetup
 *
 *  @Description:: Setup FDMA for reading data from DMA-FIFO and writing to DRAM
 *
 *  @Input      ::
 *      MainDataSize:  Transfer count (in Byte) for main area
 *      pMainBuf:      DRAM buffer for main area data
 *      SpareDataSize: Transfer count (in Byte) for spare area
 *      pSpareBuf:     DRAM buffer for spare area data
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
static void AmbaCSL_FdmaReadSetup(UINT32 MainDataSize, UINT8 *pMainBuf, UINT32 SpareDataSize, UINT8 *pSpareBuf)
{
#ifdef __aarch64__
    ULONG PhysAddr = 0U;
    ULONG Tmp;
#else
    UINT32 PhysAddr = 0U;
    UINT32 Tmp;
#endif
    AMBA_FDMA_CTRL_REG_s FdmaCtrlRegVal = {0};

    AmbaMisra_TouchUnused(pMainBuf);
    AmbaMisra_TouchUnused(pSpareBuf);

    AmbaCSL_FioFdmaClearMainStatus();

#ifdef __aarch64__
    /* FDMA setup for Spare area */
    if (AmbaWrap_memcpy(&Tmp, &pSpareBuf, sizeof(Tmp)) != OK) { /* Do nothing */ };
#ifndef CONFIG_QNX
    (void) AmbaRTSL_MmuVirt64ToPhys64(Tmp, &PhysAddr);
#else
    AmbaMMU_Virt64ToPhys64(Tmp, &PhysAddr);
#endif
    AmbaCSL_FdmaSetSpareMemAddr((UINT32)(PhysAddr & 0xffffffffU));

    /* FDMA setup for Main area */
    if (AmbaWrap_memcpy(&Tmp, &pMainBuf, sizeof(Tmp)) != OK) { /* Do nothing */ };
#ifndef CONFIG_QNX
    (void) AmbaRTSL_MmuVirt64ToPhys64(Tmp, &PhysAddr);
#else
    AmbaMMU_Virt64ToPhys64(Tmp, &PhysAddr);
#endif
    AmbaCSL_FdmaSetMainDestAddr((UINT32)(PhysAddr & 0xffffffffU));
#else
    /* FDMA setup for Spare area */
    if (AmbaWrap_memcpy(&Tmp, &pSpareBuf, sizeof(Tmp)) != OK) { /* Do nothing */ };
    (void)AmbaRTSL_MmuVirt32ToPhys32(Tmp, &PhysAddr);
    AmbaCSL_FdmaSetSpareMemAddr(PhysAddr);

    /* FDMA setup for Main area */
    if (AmbaWrap_memcpy(&Tmp, &pMainBuf, sizeof(Tmp)) != OK) { /* Do nothing */ };
    (void)AmbaRTSL_MmuVirt32ToPhys32(Tmp, &PhysAddr);
    AmbaCSL_FdmaSetMainDestAddr(PhysAddr);
#endif

    FdmaCtrlRegVal.ByteCount = MainDataSize + SpareDataSize;
    FdmaCtrlRegVal.WriteMem  = 1U;       /* Read data from DMA-FIFO and Write to DRAM */
    FdmaCtrlRegVal.DescMode  = 0;
    FdmaCtrlRegVal.Enable    = 1U;
#if 1
    FdmaCtrlRegVal.BusBlockSize = (MainDataSize > 16U) ? DMA_BUS_BLOCK_512BYTE :
                                  DMA_BUS_BLOCK_16BYTE;
#else
    /* blk setting for FPGA must be 2~4. If DMA address is not 8-byte aligned, it has to be 2~3 */
    FdmaCtrlRegVal.BusBlockSize = (MainDataSize > 16U) ? DMA_BUS_BLOCK_32BYTE :
                                  DMA_BUS_BLOCK_16BYTE;
#endif
    AmbaCSL_FdmaSetMainCtrlReg(FdmaCtrlRegVal);
}

/*
 *  @RoutineName:: AmbaCSL_FdmaWriteSetup
 *
 *  @Description:: Setup FDMA for reading data from DRAM and writing to DMA-FIFO
 *
 *  @Input      ::
 *      MainDataSize:  Transfer count (in Byte) for main area
 *      pMainBuf:      DRAM buffer for main area data
 *      SpareDataSize: Transfer count (in Byte) for spare area
 *      pSpareBuf:     DRAM buffer for spare area data
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
static void AmbaCSL_FdmaWriteSetup(UINT32 MainDataSize, UINT8 *pMainBuf, UINT32 SpareDataSize, UINT8 *pSpareBuf)
{
#ifdef __aarch64__
    ULONG PhysAddr = 0UL;
    ULONG Tmp;
#else
    UINT32 PhysAddr = 0U;
    UINT32 Tmp;
#endif
    AMBA_FDMA_CTRL_REG_s FdmaCtrlRegVal = {0};

    AmbaMisra_TouchUnused(pMainBuf);
    AmbaMisra_TouchUnused(pSpareBuf);

    AmbaCSL_FioFdmaClearMainStatus();

#ifdef __aarch64__
    /* FDMA setup for Spare area */
    if (AmbaWrap_memcpy(&Tmp, &pSpareBuf, sizeof(Tmp)) != OK) { /* Do nothing */ };
#ifndef CONFIG_QNX
    (void) AmbaRTSL_MmuVirt64ToPhys64(Tmp, &PhysAddr);
#else
    (void) AmbaMMU_Virt64ToPhys64(Tmp, &PhysAddr);
#endif
    AmbaCSL_FdmaSetSpareMemAddr((UINT32)(PhysAddr & 0xffffffffU));

    /* FDMA setup for Main area */
    if (AmbaWrap_memcpy(&Tmp, &pMainBuf, sizeof(Tmp)) != OK) { /* Do nothing */ };
#ifndef CONFIG_QNX
    (void) AmbaRTSL_MmuVirt64ToPhys64(Tmp, &PhysAddr);
#else
    (void) AmbaMMU_Virt64ToPhys64(Tmp, &PhysAddr);
#endif
    AmbaCSL_FdmaSetMainDestAddr((UINT32)(PhysAddr & 0xffffffffU));
#else
    /* FDMA setup for Spare area */
    if (AmbaWrap_memcpy(&Tmp, &pSpareBuf, sizeof(Tmp)) != OK) { /* Do nothing */ };
    (void)AmbaRTSL_MmuVirt32ToPhys32(Tmp, &PhysAddr);
    AmbaCSL_FdmaSetSpareMemAddr(PhysAddr);

    /* FDMA setup for Main area */
    if (AmbaWrap_memcpy(&Tmp, &pMainBuf, sizeof(Tmp)) != OK) { /* Do nothing */ };
    (void)AmbaRTSL_MmuVirt32ToPhys32(Tmp, &PhysAddr);
    AmbaCSL_FdmaSetMainDestAddr(PhysAddr);
#endif

    FdmaCtrlRegVal.ByteCount = MainDataSize + SpareDataSize;
    FdmaCtrlRegVal.WriteMem  = 0;        /* Read data DRAM and Write to DMA-FIFO */
    FdmaCtrlRegVal.DescMode  = 0;
    FdmaCtrlRegVal.Enable    = 1U;
#if 1
    FdmaCtrlRegVal.BusBlockSize = (MainDataSize > 16U) ? DMA_BUS_BLOCK_512BYTE :
                                  DMA_BUS_BLOCK_16BYTE;
#else
    /* blk setting for FPGA must be 2~4. If DMA address is not 8-byte aligned, it has to be 2~3 */
    FdmaCtrlRegVal.BusBlockSize = (MainDataSize > 16U) ? DMA_BUS_BLOCK_32BYTE :
                                  DMA_BUS_BLOCK_16BYTE;
#endif
    AmbaCSL_FdmaSetMainCtrlReg(FdmaCtrlRegVal);
}

/*
 *  @RoutineName:: AmbaRTSL_FdmaRead
 *
 *  @Description:: FDMA read data from FIFO to DRAM
 *
 *  @Input      ::
 *      PageAddr:  The first page address to read
 *      NumPage:   Number of pages to read
 *      pMainBuf:  pointer to DRAM buffer for main area data
 *      pSpareBuf: pointer to DRAM buffer for spare area data
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
UINT32 AmbaRTSL_FdmaRead(UINT32 PageAddr, UINT32 NumPage, UINT8 *pMainBuf, UINT8 *pSpareBuf)
{
    UINT32 Rval = OK;
    const AMBA_NAND_COMMON_INFO_s *pNandDevInfo = AmbaRTSL_NandCommonInfo;
    AMBA_RTSL_FDMA_CTRL_s *pFdmaCtrl   = (AMBA_RTSL_FDMA_CTRL_s *) &AmbaRTSL_FioCtrl.FdmaCtrl;
    ULONG MainAddr = 0x0, SpareAddr = 0x0;

    AmbaMisra_TouchUnused(&PageAddr);

    if ((NumPage == 0U) || ((pMainBuf == NULL) && (pSpareBuf == NULL))) {
        Rval = NAND_ERR_ARG;  /* wrong parameter */
    } else {

        pFdmaCtrl->MainByteCount  = NumPage * pNandDevInfo->MainByteSize;
        pFdmaCtrl->SpareByteCount = NumPage * pNandDevInfo->SpareByteSize;

        AmbaMisra_TypeCast(&MainAddr,  &pMainBuf);
        AmbaMisra_TypeCast(&SpareAddr, &pSpareBuf);

        /* FDMA Memory base address has to be at 8-Byte boundary */
        if ((pMainBuf == NULL) ||
#ifdef __aarch64__
            ((MainAddr >> 32U) != 0U) ||
#endif
            ((MainAddr & (CACHE_LINE_SIZE - 1U)) != 0U)) {
            pFdmaCtrl->pMainBuf = AmbaRTSL_FioCtrl.pWorkBufMain; /* Internal Work Buffer for Main area */
        } else {
            pFdmaCtrl->pMainBuf = pMainBuf;
        }

        /* FDMA Memory base address has to be at 8-Byte boundary */
        if ((pSpareBuf == NULL) ||
#ifdef __aarch64__
            ((SpareAddr >> 32U) != 0U) ||
#endif
            ((SpareAddr & (CACHE_LINE_SIZE - 1U)) != 0U)) {
            pFdmaCtrl->pSpareBuf = AmbaRTSL_FioCtrl.pWorkBufSpare; /* Internal Work Buffer for Spare area */
        } else {
            pFdmaCtrl->pSpareBuf = pSpareBuf;
        }

        AmbaMisra_TypeCast(&MainAddr,  &pFdmaCtrl->pMainBuf);
        AmbaMisra_TypeCast(&SpareAddr, &pFdmaCtrl->pSpareBuf);
#ifndef CONFIG_QNX
        (void)AmbaRTSL_CacheInvalData(MainAddr, pFdmaCtrl->MainByteCount);
        (void)AmbaRTSL_CacheInvalData(SpareAddr, pFdmaCtrl->SpareByteCount);
#else
        (void)AmbaCache_DataInvalidate(MainAddr, pFdmaCtrl->MainByteCount);
        (void)AmbaCache_DataInvalidate(SpareAddr, pFdmaCtrl->SpareByteCount);
#endif

#ifdef USE_FIODMA_DESCRIPTORS
        AmbaRTSL_FdmaReadSetupDesc(PageAddr, pFdmaCtrl->MainByteCount, pFdmaCtrl->pMainBuf, pFdmaCtrl->SpareByteCount, pFdmaCtrl->pSpareBuf);
#else
        AmbaCSL_FdmaReadSetup(pFdmaCtrl->MainByteCount, pFdmaCtrl->pMainBuf, pFdmaCtrl->SpareByteCount, pFdmaCtrl->pSpareBuf);
#endif
    }
    return Rval;
}

/*
 *  @RoutineName:: AmbaRTSL_FdmaWrite
 *
 *  @Description:: FDMA write data from DRAM to FIFO
 *
 *  @Input      ::
 *      PageAddr:  The first page address to write
 *      NumPage:   Number of pages to write
 *      pMainBuf:  pointer to DRAM buffer for main area data
 *      pSpareBuf: pointer to DRAM buffer for spare area data
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
UINT32 AmbaRTSL_FdmaWrite(UINT32 PageAddr, UINT32 NumPage, UINT8 *pMainBuf, UINT8 *pSpareBuf)
{
    UINT32 Rval = OK;
    const AMBA_NAND_COMMON_INFO_s *pNandDevInfo = AmbaRTSL_NandCommonInfo;
    AMBA_RTSL_FDMA_CTRL_s *pFdmaCtrl = (AMBA_RTSL_FDMA_CTRL_s *) &AmbaRTSL_FioCtrl.FdmaCtrl;
    UINT32 SpareByteSize, MainByteSize;
    ULONG MainAddr = 0x0, SpareAddr = 0x0;

    AmbaMisra_TouchUnused(&PageAddr);

    if ((NumPage == 0U) || ((pMainBuf == NULL) && (pSpareBuf == NULL))) {
        Rval = NAND_ERR_ARG;  /* wrong parameter */
    } else {

        SpareByteSize = pNandDevInfo->SpareByteSize;
        MainByteSize  = pNandDevInfo->MainByteSize;

        pFdmaCtrl->MainByteCount  = NumPage * MainByteSize;
        pFdmaCtrl->SpareByteCount = NumPage * SpareByteSize;

        AmbaMisra_TypeCast(&MainAddr,  &pMainBuf);
        AmbaMisra_TypeCast(&SpareAddr, &pSpareBuf);

        /* FDMA Memory base address has to be at 8-Byte boundary */
        if (pMainBuf == NULL) {
            pFdmaCtrl->pMainBuf = AmbaRTSL_FioCtrl.pWorkBufMain;
            if (AmbaWrap_memset(pFdmaCtrl->pMainBuf, 0xFF, pFdmaCtrl->MainByteCount) != OK) { /* Do nothing */ };
#ifdef __aarch64__
        } else if (((MainAddr & (CACHE_LINE_SIZE - 1U)) != 0U) ||
                   ((MainAddr >> 32U) != 0U)) {
#else
        } else if ((MainAddr & (CACHE_LINE_SIZE - 1U)) != 0U) {
#endif
            /* pointer to Internal Work Buffer for Main area (8-Byte alignment) */
            pFdmaCtrl->pMainBuf = AmbaRTSL_FioCtrl.pWorkBufMain;
            if (AmbaWrap_memcpy(pFdmaCtrl->pMainBuf, pMainBuf, pFdmaCtrl->MainByteCount) != OK) { /* Do nothing */ };
        } else {
            pFdmaCtrl->pMainBuf = pMainBuf;
        }

        /* FDMA Memory base address has to be at 8-Byte boundary */
        if (pSpareBuf == NULL) {
            pFdmaCtrl->pSpareBuf = AmbaRTSL_FioCtrl.pWorkBufSpare;
            if (AmbaWrap_memset(pFdmaCtrl->pSpareBuf, 0xFF, pFdmaCtrl->SpareByteCount) != OK) { /* Do nothing */ };
#ifdef __aarch64__
        } else if (((SpareAddr & (CACHE_LINE_SIZE - 1U)) != 0U) ||
                   ((SpareAddr >> 32U) != 0U)) {
#else
        } else if ((SpareAddr & (CACHE_LINE_SIZE - 1U)) != 0U) {
#endif
            /* pointer to Internal Work Buffer for Spare area (8-Byte alignment) */
            pFdmaCtrl->pSpareBuf = AmbaRTSL_FioCtrl.pWorkBufSpare;
            if (AmbaWrap_memcpy(pFdmaCtrl->pSpareBuf, pSpareBuf, pFdmaCtrl->SpareByteCount) != OK) { /* Do nothing */ };
        } else {
            pFdmaCtrl->pSpareBuf = pSpareBuf;
        }

        AmbaMisra_TypeCast(&MainAddr,  &pFdmaCtrl->pMainBuf);
        AmbaMisra_TypeCast(&SpareAddr, &pFdmaCtrl->pSpareBuf);

#ifndef CONFIG_QNX
        (void)AmbaRTSL_CacheInvalData(MainAddr, pFdmaCtrl->MainByteCount);
        (void)AmbaRTSL_CacheInvalData(SpareAddr, pFdmaCtrl->SpareByteCount);
#else
        (void)AmbaCache_DataInvalidate(MainAddr, pFdmaCtrl->MainByteCount);
        (void)AmbaCache_DataInvalidate(SpareAddr, pFdmaCtrl->SpareByteCount);
#endif

#ifdef USE_FIODMA_DESCRIPTORS
        AmbaRTSL_FdmaWriteSetupDesc(PageAddr, pFdmaCtrl->MainByteCount, pFdmaCtrl->pMainBuf, pFdmaCtrl->SpareByteCount, pFdmaCtrl->pSpareBuf);
#else
        AmbaCSL_FdmaWriteSetup(pFdmaCtrl->MainByteCount, pFdmaCtrl->pMainBuf, pFdmaCtrl->SpareByteCount, pFdmaCtrl->pSpareBuf);
#endif
    }
    return Rval;
}

/*
 *  @RoutineName:: FIO_NandCmdISR
 *
 *  @Description:: NAND Command done ISR
 *
 *  @Input      ::
 *      IntID: Interrupt ID
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
#ifndef CONFIG_QNX
/* disable NEON registers usage in ISR */
#pragma GCC push_options
#pragma GCC target("general-regs-only")

static void FIO_NandCmdISR(UINT32 IntID, UINT32 IsrArg)
{
    AMBA_FIO_RIS_REG_s IntStatusRaw = pAmbaFIO_Reg->IntStatusRaw;

    AmbaMisra_TypeCast32(&AmbaRTSL_FioCtrl.IrqStatus, &IntStatusRaw);

    AmbaMisra_TouchUnused(&IntID);
    AmbaMisra_TouchUnused(&IsrArg);

    AmbaRTSL_FdmaGetStatus();

    AmbaCSL_FioClearIrqStatus();   /* clear IRQ status */

    if (AmbaRTSL_FioNandCmdIsrCallBack != NULL) {
        AmbaRTSL_FioNandCmdIsrCallBack();
    }
}
#pragma GCC pop_options
#endif

/*
 *  @RoutineName:: AmbaRTSL_FdmaGetStatus
 *
 *  @Description:: Wait fo FDMA operation done
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
void AmbaRTSL_FdmaGetStatus(void)
{
    AMBA_FIO_ECC_STATUS_REG_s EccStatus        = AmbaCSL_FioGetEccStatus();
    AMBA_FDMA_MAIN_STATUS_REG_s FdmaMainStatus = AmbaCSL_FioFdmaGetMainStatus();

    AmbaMisra_TypeCast32(&AmbaRTSL_FioCtrl.EccStatus, &EccStatus);
    AmbaMisra_TypeCast32(&AmbaRTSL_FioCtrl.FdmaMainStatus, &FdmaMainStatus);

    AmbaCSL_FioFdmaClearMainStatus();
}

void AmbaRTSL_FioReset(void)
{
#ifndef CONFIG_QNX
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    UINT32 DelayMS = (AmbaRTSL_PllGetCortex1Clk() / 1000U) * 15U;
#else
    UINT32 DelayMS = (AmbaRTSL_PllGetCortexClk() / 1000U) * 3U;
#endif
    AmbaCSL_RctSetFlashControllerReset();
    AmbaDelayCycles(DelayMS);   /* Wait for 1ms otherwise NAND flash won't work. */
    AmbaCSL_RctClearFlashControllerReset();
    AmbaDelayCycles(DelayMS);   /* Wait for 1ms otherwise NAND flash won't work. */
#endif
}

/*
 *  @RoutineName:: AmbaRTSL_FioCheckEccStatus
 *
 *  @Description:: Check and return the irq status register of FIO controller
 *
 *  @Input      ::
 *      UINT32: ErrorBitMask
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(<0)
 */
INT32 AmbaRTSL_FioCheckIrqStatus(UINT32 ErrorBitMask)
{
    INT32 Rval = 0;
    if ((AmbaRTSL_FioCtrl.IrqStatus & ErrorBitMask) != 0U) {
        Rval = -1;
    }
    return Rval;
}

/*
 *  @RoutineName:: AmbaRTSL_FioCheckEccStatus
 *
 *  @Description:: Check and return the ECC report status register of FIO controller
 *
 *  @Input      ::
 *      pEccRpt: ECC report status
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(<0)
 */
INT32 AmbaRTSL_FioCheckEccStatus(void)
{
    INT32 Rval = 0;

    if ((AmbaRTSL_FioCtrl.EccStatus & 0x40000000U) != 0U) {
        //AMBA_FIO_ECC_STATUS2_REG_u EccStatus2Val;
        //EccStatus2Val.Data = AmbaCSL_FioGetEccStatus2();

        Rval = -1;
    }

    return Rval;
}

