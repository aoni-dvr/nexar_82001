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
#include "AmbaMisraFix.h"

#ifdef CONFIG_QNX
#include <sys/siginfo.h>
#include <sys/neutrino.h>
#else
#include "AmbaMMU.h"
#include "AmbaRTSL_GIC.h"
#endif
#include "AmbaRTSL_DRAMC.h"
#include "AmbaRTSL_GDMA.h"
#include "AmbaCSL_GDMA.h"

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
#include "AmbaCortexA76.h"
#else
#include "AmbaCortexA53.h"
#endif

#if defined(CONFIG_DEVICE_TREE_SUPPORT)
#include <AmbaIOUtility.h>
#endif

AMBA_GDMA_REG_s *pAmbaGDMA_Reg;

/* Call back functions when completed transactions */
static AMBA_GDMA_ISR_f AmbaGdmaIntHandler = NULL;

#ifdef CONFIG_THREADX /*only threadx use virtual addr*/
#ifdef CONFIG_ARM32
/**
 *  GDMA_GetPhysAddr - Translate a virtual address to a physical address
 *  @param[in] pVirtAddr A virtual address
 *  @return Translated physical address
 */
static UINT32 GDMA_GetPhysAddr(const void *pVirtAddr)
{
    UINT32 VirtAddr32 = 0, PhysAddr32 = 0;

    /* ILP32 */
    AmbaMisra_TypeCast32(&VirtAddr32, &pVirtAddr);
    (void)AmbaMMU_Virt32ToPhys32(VirtAddr32, &PhysAddr32);

    return PhysAddr32;
}
#endif

#ifdef CONFIG_ARM64
/**
 *  GDMA_GetPhysAddr - Translate a virtual address to a physical address
 *  @param[in] pVirtAddr A virtual address
 *  @return Translated physical address
 */
static UINT32 GDMA_GetPhysAddr(const void *pVirtAddr)
{
    ULONG VirtAddr64 = 0U, PhysAddr64;
    UINT32 PhysAddr32;

    AmbaMisra_TypeCast(&PhysAddr64, &pVirtAddr);
    (void) AmbaRTSL_DramQueryAttP2V(PhysAddr64, AMBA_DRAM_CLIENT_GDMA, &VirtAddr64);
    PhysAddr32 = (UINT32) VirtAddr64;

    return PhysAddr32;
}
#endif
#endif

/**
 *  GDMA_SetCompositeBuffer - Configure a BLIT
 *  @param[in] pBlockBlit BLIT parameters
 */
static void GDMA_SetCompositeBuffer(const AMBA_GDMA_BLOCK_s * pBlockBlit)
{
    AmbaCSL_GdmaSetPixelFormat(pBlockBlit->PixelFormat);
#ifdef CONFIG_THREADX /*only threadx use virtual addr*/
    AmbaCSL_GdmaSetSrc1Base(GDMA_GetPhysAddr(pBlockBlit->pDstImg)); /* background input */
    AmbaCSL_GdmaSetSrc2Base(GDMA_GetPhysAddr(pBlockBlit->pSrcImg)); /* foreground input */
    AmbaCSL_GdmaSetDestBase(GDMA_GetPhysAddr(pBlockBlit->pDstImg)); /* output */
#else
    AmbaCSL_GdmaSetSrc1Base((UINT32)(ULONG)pBlockBlit->pDstImg); /* background input */
    AmbaCSL_GdmaSetSrc2Base((UINT32)(ULONG)pBlockBlit->pSrcImg); /* foreground input */
    AmbaCSL_GdmaSetDestBase((UINT32)(ULONG)pBlockBlit->pDstImg); /* output */
#endif
    AmbaCSL_GdmaSetSrc1Pitch(pBlockBlit->DstRowStride);
    AmbaCSL_GdmaSetSrc2Pitch(pBlockBlit->SrcRowStride);
    AmbaCSL_GdmaSetDestPitch(pBlockBlit->DstRowStride);
    AmbaCSL_GdmaSetWidth(pBlockBlit->BltWidth);
    AmbaCSL_GdmaSetHeight(pBlockBlit->BltHeight);
}

/**
 *  GDMA_IntHandler - Interrupt service routine of GDMA
 *  @param[in] IntID Interrupt ID
 *  @param[in] IsrArg Optional argument of interrupt handler
 */
#pragma GCC push_options
#pragma GCC target("general-regs-only")
#if defined(CONFIG_THREADX)
static void GDMA_IntHandler(UINT32 IntID, UINT32 IsrArg)
{
    AmbaMisra_TouchUnused(&IntID);

    if (AmbaGdmaIntHandler != NULL) {
        AmbaGdmaIntHandler(IsrArg);
    }
}
#endif
#pragma GCC pop_options

/**
 *  AmbaRTSL_GdmaInit - GDMA module initialization
 */
void AmbaRTSL_GdmaInit(void)
{
#if defined(CONFIG_THREADX)
    const AMBA_INT_CONFIG_s IntConfig = {
        .TriggerType   = INT_TRIG_RISING_EDGE,
        .IrqType       = INT_TYPE_IRQ,
        .CpuTargets    = 1U
    };

    ULONG base_addr;
    INT32 offset = 0;
    (void) offset;

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    base_addr = AMBA_CA53_GDMA_BASE_ADDR;
#elif defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    base_addr = AMBA_CORTEX_A76_GDMA_BASE_ADDR;
#else
    base_addr = AMBA_CORTEX_A53_GDMA_BASE_ADDR;
#endif
    AmbaMisra_TypeCast(&pAmbaGDMA_Reg, &base_addr);

#if defined(CONFIG_DEVICE_TREE_SUPPORT)
    base_addr = IO_UtilityFDTPropertyU32Quick(offset, "ambarella,gdma", "reg", 0U);
    if ( base_addr != 0U ) {
#if defined (CONFIG_SOC_CV5)|| defined (CONFIG_SOC_CV52)
        base_addr = base_addr | AMBA_CORTEX_A76_AHB_PHYS_BASE_ADDR;
#endif
        AmbaMisra_TypeCast(&pAmbaGDMA_Reg, &base_addr);
    }
#endif

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    (void)AmbaRTSL_GicIntConfig(AMBA_INT_SPI_ID106_GDMA, &IntConfig, GDMA_IntHandler, 0);
    (void)AmbaRTSL_GicIntEnable(AMBA_INT_SPI_ID106_GDMA);
#else
    (void)AmbaRTSL_GicIntConfig(AMBA_INT_SPI_ID102_GDMA, &IntConfig, GDMA_IntHandler, 0);
    (void)AmbaRTSL_GicIntEnable(AMBA_INT_SPI_ID102_GDMA);
#endif
#endif  //end of CONFIG_THREADX

    /* clear the pointer of Call back function when completed transactions */
    AmbaGdmaIntHandler = NULL;

    /* Set the GDMA DRAM throttle max outstanding request */
    AmbaCSL_GdmaSetDramThrottle(1U);
}

/**
 *  AmbaRTSL_GdmaHookIntHandler - Hook interrupt handler
 *  @param[in] pIsr Interrupt service routine
 */
void AmbaRTSL_GdmaHookIntHandler(AMBA_GDMA_ISR_f pIsr)
{
    AmbaGdmaIntHandler = pIsr;
}

/**
 *  AmbaRTSL_GdmaGetNumAvails - Get the number of available instances
 *  @return Number of available instances
 */
UINT32 AmbaRTSL_GdmaGetNumAvails(void)
{
    return (GDMA_NUM_INSTANCE - AmbaCSL_GdmaGetNumPendingInstance());
}

/**
 *  AmbaRTSL_GdmaLinearCopy - Schedule a linear copy
 *  @param[in] pLinearBlit The control block of the linear copy
 *  @return error code
 */
UINT32 AmbaRTSL_GdmaLinearCopy(const AMBA_GDMA_LINEAR_s * pLinearBlit)
{
    UINT32 RetVal = GDMA_ERR_NONE;
    UINT32 NumPixels = pLinearBlit->NumPixels;

    if (NumPixels > GDMA_MAX_WIDTH) {
        RetVal = GDMA_ERR_ARG;
    } else {
        AmbaCSL_GdmaSetPixelFormat(pLinearBlit->PixelFormat);
#ifdef CONFIG_THREADX /*only threadx use virtual addr*/
        AmbaCSL_GdmaSetSrc1Base(GDMA_GetPhysAddr(pLinearBlit->pSrcImg));
        AmbaCSL_GdmaSetDestBase(GDMA_GetPhysAddr(pLinearBlit->pDstImg));
#else
        AmbaCSL_GdmaSetSrc1Base((UINT32)(ULONG)pLinearBlit->pSrcImg);
        AmbaCSL_GdmaSetDestBase((UINT32)(ULONG)pLinearBlit->pDstImg);
#endif
        AmbaCSL_GdmaSetWidth(NumPixels);
        AmbaCSL_GdmaGo(GDMA_OPCODE_LINEAR_COPY);
    }

    return RetVal;
}

/**
 *  AmbaRTSL_GdmaBlockCopy - Schedule a 2D block copy
 *  @param[in] pBlockBlit The control block of the 2D block copy
 *  @return error code
 */
UINT32 AmbaRTSL_GdmaBlockCopy(const AMBA_GDMA_BLOCK_s * pBlockBlit)
{
    UINT32 RetVal = GDMA_ERR_NONE;
    UINT32 Width, Height;

    Width = pBlockBlit->BltWidth;
    Height = pBlockBlit->BltHeight;

    if ((Width > GDMA_MAX_WIDTH) || (Height > GDMA_MAX_HEIGHT)) {
        RetVal = GDMA_ERR_ARG;
    } else {
        AmbaCSL_GdmaSetPixelFormat(pBlockBlit->PixelFormat);
#ifdef CONFIG_THREADX /*only threadx use virtual addr*/
        AmbaCSL_GdmaSetSrc1Base(GDMA_GetPhysAddr(pBlockBlit->pSrcImg));
        AmbaCSL_GdmaSetDestBase(GDMA_GetPhysAddr(pBlockBlit->pDstImg));
#else
        AmbaCSL_GdmaSetSrc1Base((UINT32)(ULONG)pBlockBlit->pSrcImg);
        AmbaCSL_GdmaSetDestBase((UINT32)(ULONG)pBlockBlit->pDstImg);
#endif
        AmbaCSL_GdmaSetSrc1Pitch(pBlockBlit->SrcRowStride);
        AmbaCSL_GdmaSetDestPitch(pBlockBlit->DstRowStride);
        AmbaCSL_GdmaSetWidth(Width);
        AmbaCSL_GdmaSetHeight(Height);
        AmbaCSL_GdmaGo(GDMA_OPCODE_2D_COPY_ONE_SRC);
    }

    return RetVal;
}

/**
 *  AmbaRTSL_GdmaColorKeying - Schedule a chroma key compositing
 *  @param[in] pBlockBlit The control block of the chroma key compositing
 *  @param[in] TransparentColor The color key of the transprent color
 *  @return error code
 */
UINT32 AmbaRTSL_GdmaColorKeying(const AMBA_GDMA_BLOCK_s * pBlockBlit, UINT32 TransparentColor)
{
    UINT32 RetVal = GDMA_ERR_NONE;
    UINT32 Width, Height;

    Width = pBlockBlit->BltWidth;
    Height = pBlockBlit->BltHeight;

    if ((Width > GDMA_MAX_WIDTH) || (Height > GDMA_MAX_HEIGHT)) {
        RetVal = GDMA_ERR_ARG;
    } else {
        GDMA_SetCompositeBuffer(pBlockBlit);
        AmbaCSL_GdmaSetTransparent(TransparentColor);
        AmbaCSL_GdmaGo(GDMA_OPCODE_2D_COPY_TWO_SRC);
    }

    return RetVal;
}

/**
 *  AmbaRTSL_GdmaAlphaBlending - Schedule a alpha blending
 *  @param[in] pBlockBlit The control block of the alpha blending
 *  @param[in] AlphaVal The global alpha value
 *  @param[in] BlendMode The alpha value is premultiplied or not
 *  @return error code
 */
UINT32 AmbaRTSL_GdmaAlphaBlending(const AMBA_GDMA_BLOCK_s * pBlockBlit, UINT32 AlphaVal, UINT32 BlendMode)
{
    UINT32 RetVal = GDMA_ERR_NONE;
    UINT32 Width, Height;

    Width = pBlockBlit->BltWidth;
    Height = pBlockBlit->BltHeight;

    if ((Width > GDMA_MAX_WIDTH) || (Height > GDMA_MAX_HEIGHT)) {
        RetVal = GDMA_ERR_ARG;
    } else {
        GDMA_SetCompositeBuffer(pBlockBlit);
        AmbaCSL_GdmaSetAlpha(AlphaVal);

        if (BlendMode != 0U) {
            AmbaCSL_GdmaGo(GDMA_OPCODE_ALPHA_WITHOUT_PREMUL);   /* Source has not yet multiplied alpha */
        } else {
            AmbaCSL_GdmaGo(GDMA_OPCODE_ALPHA_WITH_PREMUL);      /* Source has already multiplied alpha */
        }
    }

    return RetVal;
}
