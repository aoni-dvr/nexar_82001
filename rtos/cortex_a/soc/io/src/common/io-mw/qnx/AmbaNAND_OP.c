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
#include "AmbaNAND_Ctrl.h"
#include "AmbaNAND_OP.h"

UINT32 (*AmbaNandOp_Copyback)(UINT32 SrcPageAddr, UINT32 DestPageAddr, UINT32 TimeOut) = NULL;
UINT32 (*AmbaNandOp_EraseBlock)(UINT32 BlkAddr, UINT32 TimeOut) = NULL;
UINT32 (*AmbaNandOp_Read)(UINT32 PageAddr, UINT32 NumPage, UINT8 *pMainBuf, UINT8 *pSpareBuf, UINT32 TimeOut) = NULL;
UINT32 (*AmbaNandOp_Program)(UINT32 PageAddr, UINT32 NumPage, UINT8 *pMainBuf, UINT8 *pSpareBuf, UINT32 TimeOut) = NULL;

void (*AmbaNandOp_PreAccessNotify)(void)  = NULL;
void (*AmbaNandOp_PostAccessNotify)(void) = NULL;
#if 0
/*
 *  @RoutineName:: AmbaNAND_CopybackEmpty
 *
 *  @Description:: Read a page of data from one location and then move that data to a second location
 *
 *  @Input      ::
 *      SrcPageAddr:    Source page address
 *      DstPageAddr:    Destination page address
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
INT32 AmbaNAND_CopybackEmpty(UINT32 SrcPageAddr, UINT32 DestPageAddr, UINT32 TimeOut)
{
    AmbaMisra_TouchUnused(&SrcPageAddr);
    AmbaMisra_TouchUnused(&DestPageAddr);
    AmbaMisra_TouchUnused(&TimeOut);

    return -1;
}

/*
 *  @RoutineName:: AmbaNAND_EraseBlockEmpty
 *
 *  @Description:: To erase the block of data identified by the block address parameter
 *
 *  @Input      ::
 *      BlkAddr:  Block address
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
INT32 AmbaNAND_EraseBlockEmpty(UINT32 BlkAddr, UINT32 TimeOut)
{
    AmbaMisra_TouchUnused(&BlkAddr);
    AmbaMisra_TouchUnused(&TimeOut);

    return -1;
}

/*
 *  @RoutineName:: AmbaNAND_ReadEmpty
 *
 *  @Description:: Read pages from NAND flash
 *
 *  @Input      ::
 *      PageAddr:  The first page address to read
 *      NumPage:   Number of pages to read
 *      pMainBuf:  pointer to DRAM buffer for main area data
 *      pSpareBuf: pointer to DRAM buffer for spare area data
 *
 *  @Output     ::
 *      pMainBuf:  pointer to main area data
 *      pSpareBuf: pointer to spare area data
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
INT32 AmbaNAND_ReadEmpty(UINT32 PageAddr, UINT32 NumPage, UINT8 *pMainBuf, UINT8 *pSpareBuf, UINT32 TimeOut)
{
    AmbaMisra_TouchUnused(&PageAddr);
    AmbaMisra_TouchUnused(&NumPage);
    AmbaMisra_TouchUnused(pMainBuf);
    AmbaMisra_TouchUnused(pSpareBuf);
    AmbaMisra_TouchUnused(&TimeOut);

    return -1;
}

/*
 *  @RoutineName:: AmbaNAND_ProgramEmpty
 *
 *  @Description:: Program NAND flash memory
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
INT32 AmbaNAND_ProgramEmpty(UINT32 PageAddr, UINT32 NumPage, UINT8 *pMainBuf, UINT8 *pSpareBuf, UINT32 TimeOut)
{
    AmbaMisra_TouchUnused(&PageAddr);
    AmbaMisra_TouchUnused(&NumPage);
    AmbaMisra_TouchUnused(pMainBuf);
    AmbaMisra_TouchUnused(pSpareBuf);
    AmbaMisra_TouchUnused(&TimeOut);

    return -1;
}

void AmbaNAND_OpHookApi(void)
{
    AmbaNandOp_Copyback   = AmbaNAND_CopyBack;
    AmbaNandOp_EraseBlock = AmbaNAND_EraseBlock;
    AmbaNandOp_Read       = AmbaNAND_Read;
    AmbaNandOp_Program    = AmbaNAND_Program;
}
#endif

