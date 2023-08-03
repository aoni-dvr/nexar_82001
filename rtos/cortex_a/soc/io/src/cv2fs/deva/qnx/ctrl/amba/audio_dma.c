/*
 * $QNXLicenseC:
 * Copyright 2016, QNX Software Systems.
 * Copyright 2016, Freescale Semiconductor, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"). You
 * may not reproduce, modify or distribute this software except in
 * compliance with the License. You may obtain a copy of the License
 * at: http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OF ANY KIND, either express or implied.
 *
 * This file may contain contributions from others, either as
 * contributors under the License or as licensors under other terms.
 * Please review this entire file for other proprietary rights or license
 * notices, as well as the QNX Development Suite License Guide at
 * http://licensing.qnx.com/license-guide/ for other information.
 * $
 */
#include "AmbaKAL.h"
#include "AmbaUtility.h"
#include "AmbaDMA.h"
#include "AmbaMMU.h"
#include "AmbaMisraFix.h"

#include "amba.h"
#include "AmbaRTSL_I2S.h"

struct audio_dma_pulse_struct {
    pthread_t tid;
    int      rtn;
    void     (*handler) (HW_CONTEXT_T * hw_context);
    HW_CONTEXT_T *hw_context;
};

typedef struct {
    ULONG       Base;       /* Virtual Address */
    uint32_t    PhysBase;   /* Physical Address, u32 only */
    uint32_t    size;
    uint32_t    UsedSize;
    uint32_t    is_cached;
} audio_buf_pool_t;

static audio_buf_pool_t audio_buf_pool = {0};
static uint32_t audio_rx_desc_buf_alloc = 0;
static uint32_t audio_tx_desc_buf_alloc = 0;

#define AU_RX_TIMEOUT   1000U
#define AU_TX_TIMEOUT   1000U

ULONG ADDR_ALIGN_NUM(ULONG Addr, UINT32 Num)
{
    return (Addr + (Num - (Addr & (Num-1U))));
}

uint32_t audio_osal_eventflag_init(osal_even_t *event, const char *pName)
{
#ifdef USE_AMBA_KAL_EVNT
    return OSAL_EventFlagCreate(event, pName);
#else
    uint32_t retcode = 0;
    int32_t rval;

    (void)pName;
    if (event == NULL) {
        printf("[ERR] audio_osal_eventflag_init : event == NULL retcode = 0x%x\n", retcode);
    } else {
        atomic_set(&event->flag, 0);
        rval = sem_init(&event->wait_queue, 1, 0);
        if (rval == -1) {
            printf("[ERR] audio_osal_eventflag_init : fail rval = %d errno %d\n", rval, errno);
        }
    }
    return retcode;
#endif
}

uint32_t audio_osal_eventflag_set(osal_even_t *event, uint32_t flag)
{
#ifdef USE_AMBA_KAL_EVNT
    return OSAL_EventFlagSet(event, flag);
#else
    uint32_t retcode = 0;
    int32_t rval;

    if (event == NULL) {
        printf("[ERR] audio_osal_eventflag_set : event == NULL retcode = 0x%x\n", retcode);
    } else {
        atomic_set(&event->flag, flag);
        rval = sem_post(&event->wait_queue);
        if (rval == -1) {
            printf("[ERR] audio_osal_eventflag_set : fail rval = %d errno %d\n", rval, errno);
        }
    }
    return retcode;
#endif
}

uint32_t audio_osal_eventflag_get(osal_even_t *event, uint32_t reqflag, uint32_t all, uint32_t clear, uint32_t *actflag, uint32_t timeout)
{
#ifdef USE_AMBA_KAL_EVNT
    return OSAL_EventFlagGet(event, reqflag, all, clear, actflag, timeout);
#else
    uint32_t retcode = 0;
    struct timespec tm;
    int32_t rval;
    uint64_t time;
    uint32_t whole_one = 1U;

    if ((event == NULL) || (actflag == NULL)) {
        printf("[ERR] audio_osal_eventflag_get : event == NULL or actflag == NULL retcode = 0x%x\n", retcode);
    } else {
        rval = clock_gettime(CLOCK_MONOTONIC, &tm);
        if (rval < 0) {
            printf("[ERR] audio_osal_eventflag_get : clock_gettime fail rval = %d errno %d\n", rval, errno);
        }

        if (rval == 0) {
            time = timespec2nsec(&tm);
            time += ((uint64_t)timeout*1000000U);
            nsec2timespec(&tm, time);

            while (whole_one == 1U) {
                rval = sem_timedwait_monotonic(&event->wait_queue, &tm);
                if (rval == -1) {
                    if (errno != ETIMEDOUT) {
                        whole_one = 0;
                        printf("[ERR] audio_osal_eventflag_get : fail rval = %d errno %d\n", rval, errno);
                    } else {
                        whole_one = 0;
                        *actflag = event->flag;
                    }
                } else {
                    retcode = 0;
                    *actflag = event->flag;
                    if (all > 0) {
                        if ((*actflag & reqflag) == reqflag) {
                            /* all flag collected */
                            if (clear > 0) {
                                atomic_clr(&event->flag, reqflag);
                            }
                            whole_one = 0;
                        } else {
                            /* some flag no appeared yet, keep wait */
                        }
                    } else {
                        if ((*actflag & reqflag) > 0) {
                            /* any flag collected */
                            if (clear > 0) {
                                atomic_clr(&event->flag, (*actflag & reqflag));
                            }
                            whole_one = 0;
                        } else {
                            /* flag no appeared yet, keep wait */
                        }
                    }
                }
            }
        }
    }
    return retcode;
#endif
}

uint32_t audio_osal_eventflag_clear(osal_even_t *event, uint32_t flag)
{
#ifdef USE_AMBA_KAL_EVNT
    return OSAL_EventFlagClear(event, flag);
#else
    uint32_t retcode = 0;

    if (event == NULL) {
        printf("[ERR] audio_osal_eventflag_clear : event == NULL or actflag == NULL retcode = 0x%x\n", retcode);
    } else {
        atomic_clr(&event->flag, flag);
    }
    return retcode;
#endif
}

static int32_t alloc_fd_audio_dma = 0;

/* allocate audio dma descriptor and work buffer */
uint32_t audio_dma_alloc_addr(void)
{
    int32_t Rval = 0;
    struct posix_typed_mem_info info;
    void *virt_addr;
    off_t offset;

    /* From experience, first time use ALLOC_CONTIG, following use MAP_ALLOCATABLE */
    alloc_fd_audio_dma = posix_typed_mem_open("/ram/audio_dma", O_RDWR, POSIX_TYPED_MEM_ALLOCATE_CONTIG);
    if (alloc_fd_audio_dma < 0) {
        printf("[ERR] audio_dma_alloc_addr() : posix_typed_mem_open fail(/ram/audio_dma) fd = %d errno = %d\n", alloc_fd_audio_dma, errno);
    } else {
        Rval = posix_typed_mem_get_info(alloc_fd_audio_dma, &info);
        if (Rval < 0) {
            printf("[ERR] audio_dma_alloc_addr() : posix_typed_mem_get_info fail ret = %d errno = %d\n", Rval, errno);
        } else {
            virt_addr = mmap(NULL, info.posix_tmi_length, PROT_NOCACHE | PROT_READ | PROT_WRITE, MAP_SHARED, alloc_fd_audio_dma, 0);
            if (virt_addr == MAP_FAILED) {
                printf("[ERR] audio_dma_alloc_addr() : mmap fail errno = %d\n", errno);
            } else {
                Rval = mem_offset(virt_addr, NOFD, 1, &offset, 0);
                if (Rval < 0) {
                    printf("[ERR] audio_dma_alloc_addr() : mem_offset fail ret = %d errno = %d\n", Rval, errno);
                } else {
                    /* update buf_pool */
                    memcpy(&audio_buf_pool.Base, &virt_addr, sizeof(void *));
                    audio_buf_pool.size = (uint32_t)info.posix_tmi_length;
                    audio_buf_pool.UsedSize = 0U;
                    audio_buf_pool.is_cached = 0U;
                    memcpy(&audio_buf_pool.PhysBase, &offset, sizeof(uint32_t));
                    /*
                    printf("[audio_dma] mempool Virt %lu Phys 0x%X Sz %d Cached %d\n",
                                      audio_buf_pool.Base,
                                      audio_buf_pool.PhysBase,
                                      audio_buf_pool.size,
                                      audio_buf_pool.is_cached);*/
                }
            }
        }
    }

    return Rval;
}

uint32_t audio_dma_alloc_buf(ULONG *pBase, uint32_t ReqSize, uint32_t Align)
{
    uint32_t Rval = 0;
    uint32_t NeededSize;
    ULONG Offset, CurBase;

    if ((pBase == NULL) || (ReqSize == 0U)) {
        Rval = 0xffffffff;
        printf("[ERR] audio_dma_alloc_buf : Null address or Zero size\n");
    } else {
        /* Get dsp protocol mem range */
        if (audio_buf_pool.Base == 0UL) {
            Rval = audio_dma_alloc_addr();
        }

        if (Rval == 0) {
            CurBase = audio_buf_pool.Base + audio_buf_pool.UsedSize;
            Offset = ADDR_ALIGN_NUM(CurBase, Align) - CurBase;
            NeededSize = Offset + ReqSize;
            if (((CurBase + NeededSize) - audio_buf_pool.Base) > audio_buf_pool.size) {
                printf("[ERR] audio_dma_alloc_buf : Over audio dma pool size\n");
                Rval = 0xffffffff;
            } else {
                *pBase = CurBase + Offset;

                /* Update pool */
                audio_buf_pool.UsedSize += NeededSize;
            }
        }
    }

    return Rval;
}

uint32_t audio_rx_dma_init(uint32_t Index, amba_context_t *amba)
{
    UINT32 RxChannel = AMBA_DMA_CHANNEL_I2S_RX;

    if (Index == 0U) {
        RxChannel = AMBA_DMA_CHANNEL_I2S_RX;
    }

    return AmbaDMA_ChannelAllocate(RxChannel, &amba->capture.DmaChanNo);
}

uint32_t audio_tx_dma_init(uint32_t Index, amba_context_t *amba)
{
    UINT32 TxChannel = AMBA_DMA_CHANNEL_I2S_TX;

    if (Index == 0U) {
        TxChannel = AMBA_DMA_CHANNEL_I2S_TX;
    }

    return AmbaDMA_ChannelAllocate(TxChannel, &amba->playback.DmaChanNo);
}

static uint32_t audio_rx_dma_desc_alloc(amba_context_t *amba)
{
    uint32_t Rval = 0;
    ULONG  ULAddr;
    AMBA_DMA_DESC_s *pDesc;
    UINT32 *pReport;
    uint32_t Size;

    if (audio_rx_desc_buf_alloc == 0) {
        Size = sizeof(AMBA_DMA_DESC_s) * amba->capture.frags_total;
        Rval = audio_dma_alloc_buf(&ULAddr, Size, 8);
        if (Rval != 0) {
            printf("Rx DMA desc allc error, %d\n", Size);
        } else {
            AmbaMisra_TypeCast(&pDesc, &ULAddr);
            memset(pDesc, 0, sizeof(AMBA_DMA_DESC_s) * amba->capture.frags_total);
            amba->capture.pDesc = pDesc;
            Rval = audio_dma_alloc_buf(&ULAddr, sizeof(uint32_t) * amba->capture.frags_total, 4);
            if (Rval != 0) {
                printf("Rx DMA report allc error, %d\n", Size);
            } else {
                AmbaMisra_TypeCast(&pReport, &ULAddr);
                memset(pReport, 0, sizeof(uint32_t) * amba->capture.frags_total);
                amba->capture.pStatus = pReport;
            }
        }
    }
    return Rval;
}

uint32_t audio_rx_dma_desc_setup(amba_context_t *amba)
{
    uint32_t Rval = 0;
    AMBA_DMA_DESC_s *pDesc;
    uint32_t Loop;
    AMBA_DMA_DESC_CTRL_s DmaDescCtrl;
    UINT32 *pReport;
    void   *pI2sRxDmaAddr;
    ULONG   DmaAddr, DstAddr;
    //ULONG Tmp, Tmp2, Tmp3;

    Rval = audio_rx_dma_desc_alloc(amba);
    if (Rval == 0) {
        pDesc = amba->capture.pDesc;
        memset(pDesc, 0, sizeof(AMBA_DMA_DESC_s) * amba->capture.frags_total);
        pReport = amba->capture.pStatus;
        memset(pReport, 0, sizeof(uint32_t) * amba->capture.frags_total);
        (void)AmbaRTSL_I2sRxGetDmaAddress(amba->capture.i2s_index, &pI2sRxDmaAddr);
        DmaDescCtrl.StopOnError = 1U;
        DmaDescCtrl.IrqOnError = 1U;
        DmaDescCtrl.IrqOnDone = 1U;
        DmaDescCtrl.Reserved0 = 0U;
        DmaDescCtrl.BusBlockSize = AMBA_DMA_BUS_BLOCK_32BYTE;
        DmaDescCtrl.BusDataSize = AMBA_DMA_BUS_DATA_4BYTE;
        DmaDescCtrl.NoBusAddrInc = 1U;
        DmaDescCtrl.ReadMem = 0U;
        DmaDescCtrl.WriteMem = 1U;
        DmaDescCtrl.EndOfChain = 0U;
        DmaDescCtrl.Reserved1 = 0U;
        AmbaMisra_TypeCast(&DmaAddr, &amba->capture.pDmaBuf);
        //printf("%s DMA data buffer, vir: %lu\n", __func__, DmaAddr);
        for (Loop = 0U; Loop < (amba->capture.frags_total - 1U); Loop++) {
            pDesc->pSrcAddr = pI2sRxDmaAddr;
            DstAddr = DmaAddr + amba->capture.frag_size * Loop;
            AmbaMisra_TypeCast(&pDesc->pDstAddr, &DstAddr);
            pDesc->pNextDesc = &pDesc[1];
            pDesc->pStatus = pReport;
            //AmbaMMU_VirtToPhys((ULONG)pDesc->pDstAddr, &Tmp);
            //AmbaMisra_TypeCast(&Tmp2, &pDesc);
            //AmbaMisra_TypeCast(&Tmp3, &pDesc->pNextDesc);
            //printf("%s: 0x%x -> 0x%x, pDesc: 0x%x, Next: 0x%x\n", __func__, (UINT32)DstAddr, (UINT32)Tmp, (UINT32)Tmp2, (UINT32)Tmp3);
            memcpy(&pDesc->Ctrl, &DmaDescCtrl, sizeof(AMBA_DMA_DESC_CTRL_s));
            pDesc->DataSize = amba->capture.frag_size;
            pDesc++;
            pReport++;
        }
        pDesc->pSrcAddr = pI2sRxDmaAddr;
        DstAddr = DmaAddr + amba->capture.frag_size * Loop;
        AmbaMisra_TypeCast(&pDesc->pDstAddr, &DstAddr);
        pDesc->pNextDesc = amba->capture.pDesc;
        pDesc->pStatus = pReport;
        //AmbaMMU_VirtToPhys((ULONG)pDesc->pDstAddr, &Tmp);
        //AmbaMisra_TypeCast(&Tmp2, &pDesc);
        //AmbaMisra_TypeCast(&Tmp3, &pDesc->pNextDesc);
        //printf("%s: 0x%x -> 0x%x, pDesc: 0x%x, Next: 0x%x\n", __func__, (UINT32)DstAddr, (UINT32)Tmp, (UINT32)Tmp2, (UINT32)Tmp3);
        memcpy(&pDesc->Ctrl, &DmaDescCtrl, sizeof(AMBA_DMA_DESC_CTRL_s));
        pDesc->DataSize = amba->capture.frag_size;
        amba->capture.cur_dma_frag = 0;
    }

    return Rval;
}

uint32_t audio_rx_dma_start(amba_context_t *amba)
{
    uint32_t Rval;

    Rval = AmbaDMA_Transfer(amba->capture.DmaChanNo, amba->capture.pDesc);
    //printf("%s: Rval: 0x%x\n", __func__, Rval);

    return Rval;
}

uint32_t audio_rx_dma_stop(amba_context_t *amba)
{
    uint32_t i, index;
    AMBA_DMA_DESC_s *pDesc;

    for(i = 2; i < (amba->capture.frags_total + 2); i++) {
        index = (amba->capture.cur_dma_frag + i) % amba->capture.frags_total;
        pDesc = &amba->capture.pDesc[index];
        pDesc->Ctrl.EndOfChain = 1U;
    }
    //printf("%s\n", __func__);
    return 0U;
}

static uint32_t audio_tx_dma_desc_alloc(amba_context_t *amba)
{
    uint32_t Rval = 0;
    ULONG  ULAddr;
    AMBA_DMA_DESC_s *pDesc;
    UINT32 *pReport;
    uint32_t Size;

    if (audio_tx_desc_buf_alloc == 0) {
        Size = sizeof(AMBA_DMA_DESC_s) * amba->playback.frags_total;
        Rval = audio_dma_alloc_buf(&ULAddr, Size, 8);
        if (Rval != 0) {
            printf("Tx DMA desc allc error, %d\n", Size);
        } else {
            AmbaMisra_TypeCast(&pDesc, &ULAddr);
            memset(pDesc, 0, sizeof(AMBA_DMA_DESC_s) * amba->playback.frags_total);
            amba->playback.pDesc = pDesc;
            Rval = audio_dma_alloc_buf(&ULAddr, sizeof(uint32_t) * amba->playback.frags_total, 4);
            if (Rval != 0) {
                printf("Tx DMA report allc error, %d\n", Size);
            } else {
                AmbaMisra_TypeCast(&pReport, &ULAddr);
                memset(pReport, 0, sizeof(uint32_t) * amba->playback.frags_total);
                amba->playback.pStatus = pReport;
            }
        }
    }
    return Rval;
}

uint32_t audio_tx_dma_desc_setup(amba_context_t *amba)
{
    uint32_t Rval = 0;
    AMBA_DMA_DESC_s *pDesc;
    uint32_t Loop;
    AMBA_DMA_DESC_CTRL_s DmaDescCtrl;
    UINT32 *pReport;
    void   *pI2sTxDmaAddr;
    ULONG   DmaAddr, SrcAddr;
    //ULONG Tmp, Tmp2, Tmp3;

    Rval = audio_tx_dma_desc_alloc(amba);
    if (Rval == 0) {
        pDesc = amba->playback.pDesc;
        memset(pDesc, 0, sizeof(AMBA_DMA_DESC_s) * amba->playback.frags_total);
        pReport = amba->playback.pStatus;
        memset(pReport, 0, sizeof(uint32_t) * amba->playback.frags_total);
        (void)AmbaRTSL_I2sTxGetDmaAddress(amba->playback.i2s_index, &pI2sTxDmaAddr);
        DmaDescCtrl.StopOnError = 1U;
        DmaDescCtrl.IrqOnError = 1U;
        DmaDescCtrl.IrqOnDone = 1U;
        DmaDescCtrl.Reserved0 = 0U;
        DmaDescCtrl.BusBlockSize = AMBA_DMA_BUS_BLOCK_32BYTE;
        DmaDescCtrl.BusDataSize = AMBA_DMA_BUS_DATA_4BYTE;
        DmaDescCtrl.NoBusAddrInc = 1U;
        DmaDescCtrl.ReadMem = 1U;
        DmaDescCtrl.WriteMem = 0U;
        DmaDescCtrl.EndOfChain = 0U;
        DmaDescCtrl.Reserved1 = 0U;
        AmbaMisra_TypeCast(&DmaAddr, &amba->playback.pDmaBuf);
        //printf("%s DMA data buffer, vir: %lu\n", __func__, DmaAddr);
        for (Loop = 0U; Loop < (amba->playback.frags_total - 1U); Loop++) {
            SrcAddr = DmaAddr + amba->playback.frag_size * Loop;
            AmbaMisra_TypeCast(&pDesc->pSrcAddr, &SrcAddr);
            pDesc->pDstAddr = pI2sTxDmaAddr;
            pDesc->pNextDesc = &pDesc[1];
            pDesc->pStatus = pReport;
            //AmbaMMU_VirtToPhys((ULONG)pDesc->pSrcAddr, &Tmp);
            //AmbaMisra_TypeCast(&Tmp2, &pDesc);
            //AmbaMisra_TypeCast(&Tmp3, &pDesc->pNextDesc);
            //printf("%s: 0x%x -> 0x%x, pDesc: 0x%x, Next: 0x%x\n", __func__, (UINT32)SrcAddr, (UINT32)Tmp, (UINT32)Tmp2, (UINT32)Tmp3);
            memcpy(&pDesc->Ctrl, &DmaDescCtrl, sizeof(AMBA_DMA_DESC_CTRL_s));
            pDesc->DataSize = amba->playback.frag_size;
            pDesc++;
            pReport++;
        }
        SrcAddr = DmaAddr + amba->playback.frag_size * Loop;
        AmbaMisra_TypeCast(&pDesc->pSrcAddr, &SrcAddr);
        pDesc->pDstAddr = pI2sTxDmaAddr;
        pDesc->pNextDesc = amba->playback.pDesc;
        pDesc->pStatus = pReport;
        //AmbaMMU_VirtToPhys((ULONG)pDesc->pSrcAddr, &Tmp);
        //AmbaMisra_TypeCast(&Tmp2, &pDesc);
        //AmbaMisra_TypeCast(&Tmp3, &pDesc->pNextDesc);
        //printf("%s: 0x%x -> 0x%x, pDesc: 0x%x, Next: 0x%x\n", __func__, (UINT32)SrcAddr, (UINT32)Tmp, (UINT32)Tmp2, (UINT32)Tmp3);
        memcpy(&pDesc->Ctrl, &DmaDescCtrl, sizeof(AMBA_DMA_DESC_CTRL_s));
        pDesc->DataSize = amba->playback.frag_size;
        amba->playback.cur_dma_frag = 0;
    }

    return Rval;
}

uint32_t audio_tx_dma_start(amba_context_t *amba)
{
    uint32_t Rval;

    Rval = AmbaDMA_Transfer(amba->playback.DmaChanNo, amba->playback.pDesc);

    return Rval;
}

uint32_t audio_tx_dma_stop(amba_context_t *amba)
{
    uint32_t i, index;
    AMBA_DMA_DESC_s *pDesc;

    for(i = 2; i < (amba->playback.frags_total + 2); i++) {
        index = (amba->playback.cur_dma_frag + i) % amba->playback.frags_total;
        pDesc = &amba->playback.pDesc[index];
        pDesc->Ctrl.EndOfChain = 1U;
    }

    return 0U;
}

static void *audio_rx_entry(void *args)
{
    struct audio_dma_pulse_struct *audps = args;
    amba_context_t *amba = audps->hw_context;
    audps->rtn = 1;                      /* we're ok signal parent thread */

    for (;;) {
        if (amba->capture.active == 1) {
            (void)AmbaDMA_Wait(amba->capture.DmaChanNo, AU_RX_TIMEOUT);
            audps->handler (audps->hw_context);
        } else {
            AmbaKAL_TaskSleep(10);
        }
    }
    return (NULL);
}

uint32_t audio_rx_attach_dma(void **x, void (*handler) (HW_CONTEXT_T * hw_context), HW_CONTEXT_T * hw_context)
{
    struct audio_dma_pulse_struct *audps;
    int     policy, err;
    pthread_attr_t attr;
    struct sched_param param;

    if ((audps = (struct audio_dma_pulse_struct *) calloc (1, sizeof (struct audio_dma_pulse_struct))) == NULL)
        return (-1);

    audps->handler = handler;
    audps->hw_context = hw_context;

    pthread_attr_init (&attr);
    pthread_attr_setinheritsched (&attr, PTHREAD_EXPLICIT_SCHED);
    pthread_getschedparam (pthread_self (), &policy, &param);
    param.sched_priority = DEVA_RX_PRIORITY;
    pthread_attr_setschedparam (&attr, &param);
    pthread_attr_setschedpolicy (&attr, SCHED_RR);

    if ((err = pthread_create (&audps->tid, &attr, audio_rx_entry, audps)) != EOK) {
        ado_error ("unable to attach to pulse (%s)", strerror (err));
        return -err;
    }

    /* wait for the intr thread to fail or succeed */
    while (audps->rtn == 0)
        delay (1);
    if (audps->rtn == -1) {
        errno = EINVAL;
        return (-errno);
    }

    *x = audps;
    return (0);
}

static void *audio_tx_entry(void *args)
{
    struct audio_dma_pulse_struct *audps = args;
    amba_context_t *amba = audps->hw_context;
    audps->rtn = 1;                      /* we're ok signal parent thread */

    for (;;) {
        if (amba->playback.active == 1) {
            (void)AmbaDMA_Wait(amba->playback.DmaChanNo, AU_TX_TIMEOUT);
            audps->handler (audps->hw_context);
        } else {
            AmbaKAL_TaskSleep(10);
        }
    }
    return (NULL);
}

uint32_t audio_tx_attach_dma(void **x, void (*handler) (HW_CONTEXT_T * hw_context), HW_CONTEXT_T * hw_context)
{
    struct audio_dma_pulse_struct *audps;
    int     policy, err;
    pthread_attr_t attr;
    struct sched_param param;

    if ((audps = (struct audio_dma_pulse_struct *) calloc (1, sizeof (struct audio_dma_pulse_struct))) == NULL)
        return (-1);

    audps->handler = handler;
    audps->hw_context = hw_context;

    pthread_attr_init (&attr);
    pthread_attr_setinheritsched (&attr, PTHREAD_EXPLICIT_SCHED);
    pthread_getschedparam (pthread_self (), &policy, &param);
    param.sched_priority = DEVA_TX_PRIORITY;
    pthread_attr_setschedparam (&attr, &param);
    pthread_attr_setschedpolicy (&attr, SCHED_RR);

    if ((err = pthread_create (&audps->tid, &attr, audio_tx_entry, audps)) != EOK) {
        ado_error ("unable to attach to pulse (%s)", strerror (err));
        return -err;
    }

    /* wait for the intr thread to fail or succeed */
    while (audps->rtn == 0)
        delay (1);
    if (audps->rtn == -1) {
        errno = EINVAL;
        return (-errno);
    }

    *x = audps;
    return (0);
}

