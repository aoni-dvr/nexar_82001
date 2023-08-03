/**
 *  @file AmbaNAND_BBM.c
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details NAND Bad Block Management (BBM) APIs
 *
 */

#include "AmbaTypes.h"

#include "AmbaNAND.h"
#include "AmbaNAND_Ctrl.h"
#include "AmbaNAND_OP.h"

#include "AmbaRTSL_NAND.h"
//#include "AmbaRTSL_NAND_Ctrl.h"
//#include "AmbaRTSL_FIO.h"
#include "AmbaRTSL_NAND_BBM.h"
#include "AmbaMisraFix.h"

#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <mtd/mtd-user.h>
#include <unistd.h>

/*
 *  @RoutineName:: AmbaNAND_CreateBBT
 *
 *  @Description:: Create the BBT(Bad Block Table)
 *
 *  @Input      ::
 *      TimeOut: Time out value
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaNAND_CreateBBT(UINT32 TimeOut)
{
    UINT32 Rval = NAND_ERR_NONE;
    (void) TimeOut;
    return Rval;
}

/*
 *  @RoutineName:: AmbaNAND_GetBlkMark
 *
 *  @Description:: Get the Block Mark of a Block
 *
 *  @Input      ::
 *      BlkAddr: Block address
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : the Block Mark
 */
UINT32 AmbaNAND_GetBlkMark(UINT32 BlkAddr)
{
    UINT32 RetVal = AMBA_NAND_BLK_FACTORY_BAD;
    UINT32 Offset;
    mtd_info_t mtd_info;
    INT32 ret, fd = -1;
    char Mtd[16] = "/dev/mtd0";
    UINT32 BlkPageSize = 0U, PageByteSize = 0U;

    fd = open(Mtd, O_RDONLY);
    if (fd < 0){
        fprintf(stderr, "open %s failed!\n", Mtd);
        RetVal = AMBA_NAND_BLK_FACTORY_BAD;
    }
    else {
        ret = ioctl(fd, MEMGETINFO, &mtd_info);   // get the device info
        if (ret < 0) {
            fprintf(stderr, "get MEMGETINFO failed!\n");
            RetVal = AMBA_NAND_BLK_FACTORY_BAD;
        }
        else {
            BlkPageSize  = mtd_info.erasesize / mtd_info.writesize;
            PageByteSize = mtd_info.writesize;

            Offset = BlkAddr * BlkPageSize * PageByteSize;

            /* check bad block */
            if (ioctl(fd, MEMGETBADBLOCK, &Offset) != 0) {
                RetVal = AMBA_NAND_BLK_GOOD;
            } else {
                RetVal = AMBA_NAND_BLK_FACTORY_BAD;
            }
        }
        close(fd);
    }
    //fprintf(stderr, "%s  BlkAddr = %d, mtd=%s, Offset = %d, RetVal = %d\r\n", __func__, BlkAddr, Mtd, Offset, RetVal);

    return RetVal;
}

/*
 *  @RoutineName:: AmbaNAND_SetBlkMark
 *
 *  @Description:: Mark a bad block in BBT
 *
 *  @Input      ::
 *      BlkAddr:    Block address
 *      BlkMark:    Block mark
 *      TimeOut:    Time out value
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaNAND_SetBlkMark(UINT32 BlkAddr, UINT32 BlkMark, UINT32 TimeOut)
{
    UINT32 Rval = NAND_ERR_NONE;
    (void) BlkAddr;
    (void) BlkMark;
    (void) TimeOut;
    return Rval;
}

/*
 *  @RoutineName:: AmbaNAND_FindGoodBlkBackward
 *
 *  @Description:: Find a good block in BBT Backward
 *
 *  @Input      ::
 *      BlkAddr : start Block address for searching
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : Good Block Number/NG(-1)
 */
INT32 AmbaNAND_FindGoodBlkBackward(UINT32 BlkAddr)
{
    (void) BlkAddr;
    return 1;
}

/*
 *  @RoutineName:: AmbaNAND_FindGoodBlkForward
 *
 *  @Description:: Find a good block in BBT Forward
 *
 *  @Input      ::
 *      BlkAddr : start Block address for searching
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : Good Block Number/NG(-1)
 */
INT32 AmbaNAND_FindGoodBlkForward(UINT32 BlkAddr)
{
    (void) BlkAddr;
    return 1;
}

/*
 *  @RoutineName:: AmbaNAND_UpdateBBT
 *
 *  @Description:: Update NAND BBTs
 *
 *  @Input      ::
 *      Version:        the Version of BBT
 *      BlkAddrPrimary: Block address of Primary BBT, don't write if it is negative
 *      BlkAddrMirror:  Block address of Mirror BBT, don't write if it is negative
 *      TimeOut:        Time out value
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaNAND_UpdateBBT(UINT32 Version, UINT32 BlkAddrPrimary, UINT32 BlkAddrMirror, UINT32 TimeOut)
{
    UINT32 RetVal = 1;;
    (void) Version;
    (void) BlkAddrPrimary;
    (void) BlkAddrMirror;
    (void) TimeOut;
    return RetVal;
}

/*
 *  @RoutineName:: AmbaNAND_WriteBBT
 *
 *  @Description:: Write NAND BBT to NAND form Dram
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaNAND_WriteBBT(UINT32 TimeOut)
{
    (void) TimeOut;
    return 1;
}

/*
 *  @RoutineName:: AmbaNAND_LoadBBT
 *
 *  @Description:: Load bad block table from flash
 *
 *  @Input      ::
 *      BlkAddr : Block address
 *      TimeOut:  Time out value
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaNAND_LoadBBT(UINT32 BlkAddr, UINT32 TimeOut)
{
    UINT32 RetVal = NAND_ERR_IO_FAIL;
    (void) TimeOut;
    (void) BlkAddr;
    return RetVal;
}
