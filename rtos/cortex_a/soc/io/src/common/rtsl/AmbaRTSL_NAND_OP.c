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

#include "AmbaRTSL_NAND_OP.h"

UINT32 (*AmbaRTSL_NandOpCopyback)(UINT32 SrcPageAddr, UINT32 DestPageAddr);
UINT32 (*AmbaRTSL_NandOpEraseBlock)(UINT32 BlkAddr);
UINT32 (*AmbaRTSL_NandOpRead)(UINT32 PageAddr, UINT32 NumPage, UINT8 *pMainBuf, UINT8 *pSpareBuf);
UINT32 (*AmbaRTSL_NandOpProgram)(UINT32 PageAddr, UINT32 NumPage, UINT8 *pMainBuf, UINT8 *pSpareBuf);
#if 0
/*
 *  @RoutineName:: AmbaRTSL_NandCopybackEmpty
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
 *          INT32 : OK(0)/NG(-1)
 */
static INT32 AmbaRTSL_NandCopybackEmpty(UINT32 SrcPageAddr, UINT32 DestPageAddr)
{
    AmbaMisra_TouchUnused(&SrcPageAddr);
    AmbaMisra_TouchUnused(&DestPageAddr);
    return -1;
}

/*
 *  @RoutineName:: AmbaRTSL_NandEraseBlockEmpty
 *
 *  @Description:: To erase the block of data identified by the block address parameter
 *
 *  @Input      ::
 *      BlkAddr:  Block address
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
static INT32 AmbaRTSL_NandEraseBlockEmpty(UINT32 BlkAddr)
{
    AmbaMisra_TouchUnused(&BlkAddr);
    return -1;
}

/*
 *  @RoutineName:: AmbaRTSL_NandReadEmpty
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
 *          INT32 : OK(0)/NG(-1)
 */
static INT32 AmbaRTSL_NandReadEmpty(UINT32 PageAddr, UINT32 NumPage, UINT8 *pMainBuf, UINT8 *pSpareBuf)
{
    AmbaMisra_TouchUnused(&PageAddr);
    AmbaMisra_TouchUnused(&NumPage);
    AmbaMisra_TouchUnused(&pMainBuf);
    AmbaMisra_TouchUnused(&pSpareBuf);

    return -1;
}

/*
 *  @RoutineName:: AmbaRTSL_NandProgramEmpty
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
 *          INT32 : OK(0)/NG(-1)
 */
static INT32 AmbaRTSL_NandProgramEmpty(UINT32 PageAddr, UINT32 NumPage, UINT8 *pMainBuf, UINT8 *pSpareBuf)
{
    AmbaMisra_TouchUnused(&PageAddr);
    AmbaMisra_TouchUnused(&NumPage);
    AmbaMisra_TouchUnused(&pMainBuf);
    AmbaMisra_TouchUnused(&pSpareBuf);

    return -1;
}

void AmbaRTSL_NandOpHookEmptyApi(void)
{
    AmbaRTSL_NandOpCopyback   = AmbaRTSL_NandCopybackEmpty;
    AmbaRTSL_NandOpEraseBlock = AmbaRTSL_NandEraseBlockEmpty;
    AmbaRTSL_NandOpRead       = AmbaRTSL_NandReadEmpty;
    AmbaRTSL_NandOpProgram    = AmbaRTSL_NandProgramEmpty;
}
#endif
