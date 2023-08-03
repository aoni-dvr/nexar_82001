/**
 * @file AmbaIPC_EventHandler.c
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
 * @details AmbaIPC event handler programs are implemented here
 *
 */
#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaCache.h"
#include "AmbaLink.h"
#include "AmbaIPC_Lock.h"
#include "AmbaINT.h"
#include "AmbaLinkPrivate.h"
#include "AmbaRTSL_GIC.h"
#include "AmbaRTSL_SpiNOR_Ctrl.h"
#include "AmbaCSL_DMA.h"
#include "AmbaWrap.h"
#if !defined(CONFIG_CPU_CORTEX_R52)
#include "AmbaMMU.h"
#endif

////////////////////////////////////////////////////////////////////////////////
#if defined(IPCDEBUG)
#include <stdio.h>
#include <stdarg.h>
UINT32 IPCQUIET = 0U;
#ifdef CONFIG_CPU_CORTEX_R52
#define UART_CHANNEL 1U
#else
#define UART_CHANNEL 0U
#endif
#endif //defined(IPCDEBUG)

void IpcDebug(const char *fmt, ...)
{
#ifdef IPCDEBUG
    extern UINT32 AmbaUART_Write(UINT32 UartCh, UINT32 FlowCtrlMode, UINT32 TxSize, const UINT8 *pTxBuf, UINT32 *pActualTxSize, UINT32 TimeOut);
    UINT8 Buf[512];
    va_list args;
    INT32 n;
    UINT32 SentSize;

    va_start(args, fmt);
    n = vsnprintf((char *)Buf, 512-3, (const char *)fmt, args);
    Buf[n] = (UINT8)'\r';
    Buf[n+1] = (UINT8)'\n';
    Buf[n+2] = (UINT8)'\0';
    va_end(args);
    if (IPCQUIET == 0U) {
        (void) AmbaUART_Write(UART_CHANNEL, 0U, n + 3, Buf, &SentSize, 1U);
    }
#else
    const char *dummy;

    dummy = fmt;
    (void)dummy;
#endif
}

void IpcCacheClean(const void *pAddr, UINT32 Size)
{
    UINT32 Reg;

    /*Reg = (UINT32)pAddr;*/
    if (AmbaWrap_memcpy(&Reg, &pAddr, sizeof(Reg))!= 0U) { }
#ifdef IPCDEBUG
    IpcDebug("IPC %s(%d) AmbaCache_Clean %p 0x%x", __func__, __LINE__, pAddr, Size);
    if ((Reg & (CACHE_LINE_SIZE - 1)) != 0x0) {
        IpcDebug("\x1b" "[1;32m IPC %s(%d) %p NOT aligned" "\x1b" "[0m", __func__, __LINE__, pAddr);
    }
#endif
    (void) AmbaCache_DataClean(Reg, Size);
}

void IpcCacheInvalidate(const void *pAddr, UINT32 Size)
{
    UINT32 Reg;

    /*Reg = (UINT32)pAddr;*/
    if (AmbaWrap_memcpy(&Reg, &pAddr, sizeof(Reg))!= 0U) { }

#ifdef IPCDEBUG
    IpcDebug("IPC %s(%d) AmbaCache_Invalidate %p 0x%x", __func__, __LINE__, pAddr, Size);
    if ((Reg & (CACHE_LINE_SIZE - 1)) != 0x0) {
        IpcDebug("\x1b" "[1;32m IPC %s(%d) %p NOT aligned" "\x1b" "[0m", __func__, __LINE__, pAddr);
    }
#endif
    (void) AmbaCache_DataInvalidate(Reg, Size);
}

void IpcCacheFlush(const void *pAddr, UINT32 Size)
{
    UINT32 Reg;

    /*Reg = (UINT32)pAddr;*/
    if (AmbaWrap_memcpy(&Reg, &pAddr, sizeof(Reg))!= 0U) { }

#ifdef IPCDEBUG
    IpcDebug("IPC %s(%d) AmbaCache_Flush %p 0x%x", __func__, __LINE__, pAddr, Size);
    if ((Reg & (CACHE_LINE_SIZE - 1)) != 0x0) {
        IpcDebug("\x1b" "[1;32m IPC %s(%d) %p NOT aligned" "\x1b" "[0m", __func__, __LINE__, pAddr);
    }
#endif
    (void) AmbaCache_DataFlush(Reg, Size);
}

void *IpcVirtToPhys(void *pAddr)
{
#if defined(CONFIG_CPU_CORTEX_R52)
    return pAddr;
#else
    ULONG VirtAddr;
    ULONG PhysAddr = 0U;
    void *ptr;

    (void)pAddr;
    /*VirtAddr = (ULONG)pAddr;*/
    if (AmbaWrap_memcpy(&VirtAddr, &pAddr, sizeof(VirtAddr))!= 0U) { }
    (void)AmbaMMU_VirtToPhys(VirtAddr, &PhysAddr);

    AmbaMisra_TypeCast(&ptr, &PhysAddr);
    return ptr;
#endif
}

void *IpcPhysToVirt(void *pAddr)
{
#if defined(CONFIG_CPU_CORTEX_R52)
    return pAddr;
#else
    ULONG PhysAddr;
    ULONG VirtAddr = 0U;
    void *ptr;

    (void)pAddr;
    /*PhysAddr = (ULONG)pAddr;*/
    if (AmbaWrap_memcpy(&PhysAddr, &pAddr, sizeof(PhysAddr))!= 0U) { }
    (void)AmbaMMU_PhysToVirt(PhysAddr, &VirtAddr);

    AmbaMisra_TypeCast(&ptr, &VirtAddr);
    return ptr;
#endif
}

UINT32 IpcReadReg(const ULONG Addr)
{
    const UINT32 *pAddr;
    UINT32 Value;

    /*pAddr = (UINT32 *)Addr;*/
    pAddr = NULL;
    if (AmbaWrap_memcpy(&pAddr, &Addr, sizeof(Addr))!= 0U) { }

    Value = *(pAddr);
    IpcDebug("IPC %s(%d) 0x%08x 0x%08x", __func__, __LINE__, Addr, Value);
    return Value;
}

void IpcWriteReg(const ULONG Addr, const UINT32 Value)
{
    volatile UINT32 *pAddr;

    /*pAddr = (UINT32 *)Addr;*/
    pAddr = NULL;
    if (AmbaWrap_memcpy(&pAddr, &Addr, sizeof(Addr))!= 0U) { }
    *(pAddr) = Value;
    IpcDebug("IPC %s(%d) 0x%08x 0x%08x", __func__, __LINE__, Addr, Value);
}

void IpcSPWriteBit(const ULONG Addr, const UINT32 bit)
{
    volatile UINT32 *pAddr;
    if (bit > 31U) {
        IpcDebug("IPC %s(%d) bit=%d", __func__, __LINE__, bit);
    } else {
        const UINT32 Value = (UINT32)((UINT32)1U << bit);
        /*pAddr = (UINT32 *)Addr;*/
        pAddr = NULL;
        if (AmbaWrap_memcpy(&pAddr, &Addr, sizeof(Addr))!= 0U) { }
        *(pAddr) = Value;
        IpcDebug("IPC %s(%d) 0x%08x 0x%08x", __func__, __LINE__, Addr, Value);
    }
}

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief This function is used to lock NAND Mutex
 */
void AmbaIPC_NandLock(void);
void AmbaIPC_NandLock(void)
{

    if (AmbaLink_Enable != 0U) {
        (void)AmbaIPC_MutexTake(AMBA_IPC_MUTEX_NAND, AMBA_KAL_WAIT_FOREVER);
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25) || \
    defined(CONFIG_SOC_CV28) || defined(CONFIG_SOC_H32) || defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        AmbaRTSL_GicSetCpuTarget(AMBA_INT_SPI_ID100_FIO, AmbaLinkCtrl.AmbaLinkRunTarget);
        AmbaINT_Enable(AMBA_INT_SPI_ID100_FIO);
#elif defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
        AmbaRTSL_GicSetCpuTarget(AMBA_INT_SPI_ID104_FIO, AmbaLinkCtrl.AmbaLinkRunTarget);
        (void)AmbaINT_Enable(AMBA_INT_SPI_ID104_FIO);
#elif defined(CONFIG_SOC_H22)
        AmbaRTSL_GicSetCpuTarget(AMBA_INT_SPI_ID104_FIO_CMD, AmbaLinkCtrl.AmbaLinkRunTarget);
        AmbaRTSL_GicSetCpuTarget(AMBA_INT_SPI_ID105_FIO_DMA, AmbaLinkCtrl.AmbaLinkRunTarget);
        AmbaRTSL_GicSetCpuTarget(AMBA_INT_SPI_ID102_FDMA, AmbaLinkCtrl.AmbaLinkRunTarget);
        AmbaINT_Enable(AMBA_INT_SPI_ID104_FIO_CMD);
        AmbaINT_Enable(AMBA_INT_SPI_ID105_FIO_DMA);
        AmbaINT_Enable(AMBA_INT_SPI_ID102_FDMA);
#else
#error unsupport CONFIG_SOC
#endif
    }
}

/**
 * @brief This function is used to unlock NAND Mutex
 */
void AmbaIPC_NandUnlock(void);
void AmbaIPC_NandUnlock(void)
{
    if (AmbaLink_Enable != 0U) {
        (void)AmbaIPC_MutexGive(AMBA_IPC_MUTEX_NAND);
    }
}

/**
 * @brief This function is used to lock SD Mutex
 *
 * @param [in] SdChanNo SD channel number
 *
 * @return 0 - OK, others - NG
 */
UINT32 AmbaIPC_SDLock(UINT32 SdChanNo);
UINT32 AmbaIPC_SDLock(UINT32 SdChanNo)
{
    UINT32 ret = 0U;

    if (AmbaLink_Enable != 0U) {
        if (SdChanNo == 0U) {
            (void)AmbaIPC_MutexTake(AMBA_IPC_MUTEX_SD0, AMBA_KAL_WAIT_FOREVER);

#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25)|| \
    defined(CONFIG_SOC_CV28) || defined(CONFIG_SOC_H32) || defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
            AmbaRTSL_GicSetCpuTarget(AMBA_INT_SPI_ID104_SD, AmbaLinkCtrl.AmbaLinkRunTarget);
            AmbaINT_Enable(AMBA_INT_SPI_ID104_SD);
#elif defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
            AmbaRTSL_GicSetCpuTarget(AMBA_INT_SPI_ID108_SD, AmbaLinkCtrl.AmbaLinkRunTarget);
            (void)AmbaINT_Enable(AMBA_INT_SPI_ID108_SD);
#elif defined(CONFIG_SOC_H22)
            AmbaRTSL_GicSetCpuTarget(AMBA_INT_SPI_ID107_SD, AmbaLinkCtrl.AmbaLinkRunTarget);
            (void)AmbaINT_Enable(AMBA_INT_SPI_ID107_SD);
#else
#error unsupport CONFIG_SOC
#endif
        }
        else if (SdChanNo == 1U) {
            (void)AmbaIPC_MutexTake(AMBA_IPC_MUTEX_SD1, AMBA_KAL_WAIT_FOREVER);

#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22)
            AmbaRTSL_GicSetCpuTarget(AMBA_INT_SPI_ID103_SDXC, AmbaLinkCtrl.AmbaLinkRunTarget);
            AmbaINT_Enable(AMBA_INT_SPI_ID103_SDXC);
#elif defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
            AmbaRTSL_GicSetCpuTarget(AMBA_INT_SPI_ID107_SDIO0, AmbaLinkCtrl.AmbaLinkRunTarget);
            (void)AmbaINT_Enable(AMBA_INT_SPI_ID107_SDIO0);
#elif defined(CONFIG_SOC_H32) || defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_CV28) || defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
            AmbaRTSL_GicSetCpuTarget(AMBA_INT_SPI_ID103_SDIO0, AmbaLinkCtrl.AmbaLinkRunTarget);
            (void)AmbaINT_Enable(AMBA_INT_SPI_ID103_SDIO0);
#elif defined(CONFIG_SOC_H22)
            AmbaRTSL_GicSetCpuTarget(AMBA_INT_SPI_ID106_SDXC, AmbaLinkCtrl.AmbaLinkRunTarget);
            (void)AmbaINT_Enable(AMBA_INT_SPI_ID106_SDXC);
#else
#error unsupport CONFIG_SOC
#endif
        }
        else if (SdChanNo == 2U) {
            (void)AmbaIPC_MutexTake(AMBA_IPC_MUTEX_SD2, AMBA_KAL_WAIT_FOREVER);
#if defined(CONFIG_SOC_H32) || defined(CONFIG_SOC_CV28)
            AmbaRTSL_GicSetCpuTarget(AMBA_INT_SPI_ID96_SDIO1, AmbaLinkCtrl.AmbaLinkRunTarget);
            (void)AmbaINT_Enable(AMBA_INT_SPI_ID96_SDIO1);
#elif defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
            AmbaRTSL_GicSetCpuTarget(AMBA_INT_SPI_ID161_SDIO1, AmbaLinkCtrl.AmbaLinkRunTarget);
            (void)AmbaINT_Enable(AMBA_INT_SPI_ID161_SDIO1);
#else
            ret = IPC_ERR_EINVAL;
#endif
        }
        else {
            ret = IPC_ERR_EINVAL;
        }
    }

    return ret;
}

/**
 * @brief This function is used to unlock SD Mutex
 *
 * @param [in] SdChanNo SD channel number
 *
 * @return 0 - OK, others - NG
 */
UINT32 AmbaIPC_SDUnlock(UINT32 SdChanNo);
UINT32 AmbaIPC_SDUnlock(UINT32 SdChanNo)
{
    UINT32 ret = 0U;

    if (AmbaLink_Enable == 0U) {
        ret = IPC_ERR_EINVAL;
    }
    else {
        if (SdChanNo == 0U) {
            (void)AmbaIPC_MutexGive(AMBA_IPC_MUTEX_SD0);
        }
        else if (SdChanNo == 1U) {
            (void)AmbaIPC_MutexGive(AMBA_IPC_MUTEX_SD1);
        }
        else if (SdChanNo == 2U) {
            (void)AmbaIPC_MutexGive(AMBA_IPC_MUTEX_SD2);
        }
        else {
            ret = IPC_ERR_EINVAL;
        }
    }

    return ret;
}

/**
 * @brief This function is used to lock SPINAND Mutex
 */
void AmbaIPC_SpiNANDLock(void);
void AmbaIPC_SpiNANDLock(void)
{
    if (AmbaLink_Enable != 0U) {
        (void)AmbaIPC_MutexTake(AMBA_IPC_MUTEX_NAND, AMBA_KAL_WAIT_FOREVER);
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25) || \
    defined(CONFIG_SOC_CV28) || defined(CONFIG_SOC_H32) || defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        AmbaRTSL_GicSetCpuTarget(AMBA_INT_SPI_ID100_FIO, AmbaLinkCtrl.AmbaLinkRunTarget);
        AmbaINT_Enable(AMBA_INT_SPI_ID100_FIO);
#elif defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
        AmbaRTSL_GicSetCpuTarget(AMBA_INT_SPI_ID104_FIO, AmbaLinkCtrl.AmbaLinkRunTarget);
        (void)AmbaINT_Enable(AMBA_INT_SPI_ID104_FIO);
#elif defined(CONFIG_SOC_H22)
        AmbaRTSL_GicSetCpuTarget(AMBA_INT_SPI_ID104_FIO_CMD, AmbaLinkCtrl.AmbaLinkRunTarget);
        AmbaRTSL_GicSetCpuTarget(AMBA_INT_SPI_ID105_FIO_DMA, AmbaLinkCtrl.AmbaLinkRunTarget);
        AmbaRTSL_GicSetCpuTarget(AMBA_INT_SPI_ID102_FDMA, AmbaLinkCtrl.AmbaLinkRunTarget);
        AmbaINT_Enable(AMBA_INT_SPI_ID104_FIO_CMD);
        AmbaINT_Enable(AMBA_INT_SPI_ID105_FIO_DMA);
        AmbaINT_Enable(AMBA_INT_SPI_ID102_FDMA);
#else
#error unsupport CONFIG_SOC
#endif
    }
}

/**
 * @brief This function is used to unlock SPINAND Mutex
 */
void AmbaIPC_SpiNANDUnlock(void);
void AmbaIPC_SpiNANDUnlock(void)
{
    if (AmbaLink_Enable != 0U) {
        (void)AmbaIPC_MutexGive(AMBA_IPC_MUTEX_NAND);
    }
}

/**
 * @brief This function is used to lock SPINOR Mutex
 */
void AmbaIPC_SpiNORLock(void);
void AmbaIPC_SpiNORLock(void)
{
    if (AmbaLink_Enable != 0U) {
        extern void AmbaRTSL_NorSetSPIClk(UINT32 Frequency);
        extern void AmbaRTSL_NorSetSPISetting(const AMBA_SERIAL_SPI_CONFIG_s *SpiConfig);

        (void)AmbaIPC_MutexTake(AMBA_IPC_MUTEX_SPINOR, AMBA_KAL_WAIT_FOREVER);
        /* For the case of running SpiNOR native driver on uitron and linux, */
        /* switch irq to RTOS for current request. */

#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25) || \
    defined(CONFIG_SOC_CV28) || defined(CONFIG_SOC_H32)
        AmbaRTSL_GicSetCpuTarget(AMBA_INT_SPI_ID105_SPI_NOR, AmbaLinkCtrl.AmbaLinkRunTarget);
        AmbaRTSL_GicSetCpuTarget(AMBA_INT_SPI_ID115_DMA0, AmbaLinkCtrl.AmbaLinkRunTarget);
        /*
         * Restore SpiNOR setting in DMA0
         * TODO: get from _AmbaNOR_SPI_Ctrl or _AmbaRTSL_NorSpiCtrl
         * TODO: should be mapped to AmbaDmaChanFunc[]
         */
        AmbaCSL_DmaSetChanFunc(0, AMBA_DMA_CHANNEL_NOR_SPI_TX);
        AmbaCSL_DmaSetChanFunc(1, AMBA_DMA_CHANNEL_NOR_SPI_RX);
#elif defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        AmbaRTSL_GicSetCpuTarget(AMBA_INT_SPI_ID105_SPI_NOR, AmbaLinkCtrl.AmbaLinkRunTarget);
        AmbaRTSL_GicSetCpuTarget(AMBA_INT_SPI_ID117_DMA0, AmbaLinkCtrl.AmbaLinkRunTarget);

        AmbaCSL_DmaSetChanFunc(0, AMBA_DMA_CHANNEL_NOR_SPI_TX);
        AmbaCSL_DmaSetChanFunc(1, AMBA_DMA_CHANNEL_NOR_SPI_RX);
#elif defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
        AmbaRTSL_GicSetCpuTarget(AMBA_INT_SPI_ID109_SPI_NOR, AmbaLinkCtrl.AmbaLinkRunTarget);
        AmbaRTSL_GicSetCpuTarget(AMBA_INT_SPI_ID117_DMA_ENGINE0, AmbaLinkCtrl.AmbaLinkRunTarget);

        AmbaCSL_DmaSetChanFunc(0, AMBA_DMA_CHANNEL_NOR_SPI_TX);
        AmbaCSL_DmaSetChanFunc(1, AMBA_DMA_CHANNEL_NOR_SPI_RX);
#elif defined(CONFIG_SOC_H22)
        AmbaRTSL_GicSetCpuTarget(AMBA_INT_SPI_ID108_SPI_NOR, AmbaLinkCtrl.AmbaLinkRunTarget);
        AmbaRTSL_GicSetCpuTarget(AMBA_INT_SPI_ID101_DMA, AmbaLinkCtrl.AmbaLinkRunTarget);
#else
#error unsupport CONFIG_SOC
#endif
        /* Reset the SPI interface to Default Value */
        AmbaRTSL_NorSetSPIClk(AmbaRTSL_NorSpiCtrl.pNorDevInfo->SpiFrequncy);
        AmbaRTSL_NorSetSPISetting(AmbaRTSL_NorSpiCtrl.pSpiSetting);
    }
}

/**
 * @brief This function is used to unlock SPINOR Mutex
 */
void AmbaIPC_SpiNORUnlock(void);
void AmbaIPC_SpiNORUnlock(void)
{
    if (AmbaLink_Enable != 0U) {
#if !defined(CONFIG_SOC_H22)
        /*
         * Clear SpiNOR setting in DMA0
         * TODO: get from _AmbaNOR_SPI_Ctrl or _AmbaRTSL_NorSpiCtrl
         * TODO: should be mapped to AmbaDmaChanFunc[]
         */
        AmbaCSL_DmaSetChanFunc(0, 0);
        AmbaCSL_DmaSetChanFunc(1, 0);
#endif
        (void)AmbaIPC_MutexGive(AMBA_IPC_MUTEX_SPINOR);
    }
}


