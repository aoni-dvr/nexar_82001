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
#include "Generic.h"
#include <hw/dma.h>

#include "AmbaTypes.h"
#include "AmbaCache.h"
#include "AmbaDMA.h"
#include "AmbaMMU.h"
#include "AmbaMisraFix.h"

void DmaDesSetup(AMBA_DMA_DESC_s *pDmaDesc)
{
    ULONG StartDescPhysAddr = 0U;
    AMBA_DMA_DESC_s *pWorkDmaDesc = pDmaDesc;
    const void *pNextDesc;
    ULONG Tmp = 0U;

    AmbaMMU_VirtToPhys((ULONG)pDmaDesc, &StartDescPhysAddr);
    while (pWorkDmaDesc != NULL) {
        pNextDesc = pWorkDmaDesc->pNextDesc;

        AmbaMMU_VirtToPhys((ULONG)pWorkDmaDesc->pSrcAddr, &Tmp);
        pWorkDmaDesc->PrivData[0] = (UINT32)Tmp;
        AmbaMMU_VirtToPhys((ULONG)pWorkDmaDesc->pDstAddr, &Tmp);
        pWorkDmaDesc->PrivData[1] = (UINT32)Tmp;
        AmbaMMU_VirtToPhys((ULONG)pWorkDmaDesc->pNextDesc, &Tmp);
        pWorkDmaDesc->PrivData[2] = (UINT32)Tmp;
        AmbaMMU_VirtToPhys((ULONG)pWorkDmaDesc->pStatus, &Tmp);
        pWorkDmaDesc->PrivData[3] = (UINT32)Tmp;

        AmbaCache_DataClean((ULONG)pWorkDmaDesc, sizeof(AMBA_DMA_DESC_s));

        /* Loop detection */
        if (pWorkDmaDesc->PrivData[2] == (UINT32)StartDescPhysAddr) {
            break;
        }
        /* LP64 */
        AmbaMisra_TypeCast(&pWorkDmaDesc, &pNextDesc);
    }
}

/**
 *  AmbaDMA_DrvEntry - Initializes driver-wide data structures and resources
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaDMA_DrvEntry(void)
{
    UINT32 RetVal = DMA_ERR_NONE;

    return RetVal;
}

/**
 *  AmbaDMA_ChannelAllocate - Allocate a dedidated dma channel for the specified purpose
 *  @param[in] DmaChanType The desired dma function
 *  @param[out] pDmaChanNo The allocated dma channel id
 *  @return error code
 */
UINT32 AmbaDMA_ChannelAllocate(UINT32 DmaChanType, UINT32 *pDmaChanNo)
{
    UINT32 RetVal = DMA_ERR_NONE;
    amba_dma_config_t DmaConfig = {0U};
    int fd, err;

    if (pDmaChanNo != NULL) {
        fd = open("/dev/dma", O_RDWR);
        if (fd == -1) {
            fprintf(stderr, "%s: Can't attach to /dev/dma manager.\n", __FUNCTION__);
        } else {
            DmaConfig.DmaChanType = DmaChanType;
            err = devctl(fd, DCMD_DMA_CHANNEL_ALLOCATE, &DmaConfig, sizeof(amba_dma_config_t), NULL);
            if (err) {
                fprintf(stderr, "%s: failed: %s", __FUNCTION__, strerror(errno));
            } else {
                *pDmaChanNo = DmaConfig.DmaChanNo;
            }
            close(fd);
        }
    } else {
        RetVal = DMA_ERR_ARG;
    }

    return RetVal;
}

/**
 *  AmbaDMA_ChannelRelease - Release an allocated dma channel
 *  @param[in] DmaChanNo The allocated dma channel id
 *  @return error code
 */
UINT32 AmbaDMA_ChannelRelease(UINT32 DmaChanNo)
{
    UINT32 RetVal = DMA_ERR_NONE;
    amba_dma_config_t DmaConfig = {0U};
    int fd, err;

    if (DmaChanNo < AMBA_NUM_DMA_CHANNEL ) {
        fd = open("/dev/dma", O_RDWR);
        if (fd == -1) {
            fprintf(stderr, "%s: Can't attach to /dev/dma manager.\n", __FUNCTION__);
        } else {
            DmaConfig.DmaChanNo = DmaChanNo;
            err = devctl(fd, DCMD_DMA_RELEASE, &DmaConfig, sizeof(amba_dma_config_t), NULL);
            if (err) {
                fprintf(stderr, "%s: failed: %s", __FUNCTION__, strerror(errno));
            }
            close(fd);
        }
    } else {
        RetVal = DMA_ERR_ARG;
    }

    return RetVal;
}

/**
 *  AmbaDMA_Transfer - Start the dma transfer
 *  @param[in] DmaChanNo A dma channel id (must acquire one before the transfer)
 *  @param[in] pDmaDesc The control block of the dma transfer
 *  @return error code
 */
UINT32 AmbaDMA_Transfer(UINT32 DmaChanNo, AMBA_DMA_DESC_s *pDmaDesc)
{
    UINT32 RetVal = DMA_ERR_NONE;
    amba_dma_config_t DmaConfig = {0U};
    amba_dma_des_config_t DmaDesConfig = {0U};
    int fd, err;
    ULONG Src, Dst;

    if (DmaChanNo < AMBA_NUM_DMA_CHANNEL ) {
        fd = open("/dev/dma", O_RDWR);
        if (fd == -1) {
            fprintf(stderr, "%s: Can't attach to /dev/dma manager.\n", __FUNCTION__);
        } else {
            if (pDmaDesc->pNextDesc != NULL) {
                DmaDesSetup(pDmaDesc);
                DmaDesConfig.DmaChanNo = DmaChanNo;
                AmbaMisra_TypeCast(&Src, &pDmaDesc);
                AmbaMMU_VirtToPhys(Src, &Src);
                DmaDesConfig.DmaDescAddr = (unsigned int)Src;
                err = devctl(fd, DCMD_DMA_TRANSFER_DES, &DmaDesConfig, sizeof(amba_dma_des_config_t), NULL);
            } else {
                DmaConfig.DmaChanNo = DmaChanNo;
                AmbaMMU_VirtToPhys((ULONG)pDmaDesc->pSrcAddr, &Src);
                AmbaMMU_VirtToPhys((ULONG)pDmaDesc->pDstAddr, &Dst);
                AmbaMisra_TypeCast(&pDmaDesc->pSrcAddr, &Src);
                AmbaMisra_TypeCast(&pDmaDesc->pDstAddr, &Dst);
                memcpy(&DmaConfig.DmaDesc, pDmaDesc, sizeof(AMBA_DMA_DESC_s));
                err = devctl(fd, DCMD_DMA_TRANSFER, &DmaConfig, sizeof(amba_dma_config_t), NULL);
            }
            if (err) {
                fprintf(stderr, "%s: failed: %s", __FUNCTION__, strerror(errno));
            }
            close(fd);
        }
    } else {
        fprintf(stderr, "%s: DMA_ERR_ARG.\n", __FUNCTION__);
        RetVal = DMA_ERR_ARG;
    }

    return RetVal;
}

/**
 *  AmbaDMA_Wait - Wait for dma transfer compeltion
 *  @param[in] DmaChanNo A dma channel id (must acquire one before the transfer to prevent conflict)
 *  @param[in] TimeOut The maximum wait time
 *  @return error code
 */
UINT32 AmbaDMA_Wait(UINT32 DmaChanNo, UINT32 TimeOut)
{
    UINT32 RetVal = DMA_ERR_NONE;
    amba_dma_config_t DmaConfig = {0U};
    int fd, err;

    if (DmaChanNo < AMBA_NUM_DMA_CHANNEL) {
        fd = open("/dev/dma", O_RDWR);
        if (fd == -1) {
            fprintf(stderr, "%s: Can't attach to /dev/dma manager.\n", __FUNCTION__);
        } else {
            DmaConfig.DmaChanNo = DmaChanNo;
            DmaConfig.TimeOut = TimeOut;
            err = devctl(fd, DCMD_DMA_WAIT, &DmaConfig, sizeof(amba_dma_config_t), NULL);
            if (err) {
                fprintf(stderr, "%s: failed: %s", __FUNCTION__, strerror(errno));
            }
            close(fd);
        }
    } else {
        RetVal = DMA_ERR_ARG;
    }

    return RetVal;
}

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
/**
 *  AmbaDMA_GetIntCount - Get interrupt count of a dma channel
 *  @param[in] DmaChanNo The allocated dma channel id
 *  @return error code
 */
UINT32 AmbaDMA_GetIntCount(UINT32 DmaChanNo)
{
    UINT32 RetVal = DMA_ERR_NONE;
    amba_dma_config_t DmaConfig = {0U};
    int fd, err;

    if (DmaChanNo < AMBA_NUM_DMA_CHANNEL ) {
        fd = open("/dev/dma", O_RDWR);
        if (fd == -1) {
            fprintf(stderr, "%s: Can't attach to /dev/dma manager.\n", __FUNCTION__);
        } else {
            DmaConfig.DmaChanNo = DmaChanNo;
            err = devctl(fd, DCMD_DMA_GET_INT_COUNT, &DmaConfig, sizeof(amba_dma_config_t), NULL);
            if (err) {
                fprintf(stderr, "%s: failed: %s", __FUNCTION__, strerror(errno));
            } else {
                RetVal = DmaConfig.IntCount;
            }

            close(fd);
        }
    } else {
        RetVal = DMA_ERR_ARG;
    }

    return RetVal;
}

/**
 *  AmbaDMA_ResetIntCount - Reset interrupt count of a DMA channel
 *  @param[in] DmaChanNo The allocated dma channel id
 *  @return error code
 */
UINT32 AmbaDMA_ResetIntCount(UINT32 DmaChanNo)
{
    UINT32 RetVal = DMA_ERR_NONE;
    amba_dma_config_t DmaConfig = {0U};
    int fd, err;

    if (DmaChanNo < AMBA_NUM_DMA_CHANNEL ) {
        fd = open("/dev/dma", O_RDWR);
        if (fd == -1) {
            fprintf(stderr, "%s: Can't attach to /dev/dma manager.\n", __FUNCTION__);
        } else {
            DmaConfig.DmaChanNo = DmaChanNo;
            err = devctl(fd, DCMD_DMA_RESET_INIT_COUNT, &DmaConfig, sizeof(amba_dma_config_t), NULL);
            if (err) {
                fprintf(stderr, "%s: failed: %s", __FUNCTION__, strerror(errno));
            }
            close(fd);
        }
    } else {
        RetVal = DMA_ERR_ARG;
    }

    return RetVal;
}
#endif
