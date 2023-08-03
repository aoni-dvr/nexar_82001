/**
 *  @file AppUSB_SoftwareFifo.h
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
 *  @details Software FIFO for USB unit tests.
 */

#ifndef APPUSB_SWFIFO_H
#define APPUSB_SWFIFO_H

#define FIFO_ERR_TIMEOUT        -1
#define FIFO_ERR_GET_MEM_FAIL   -2
#define FIFO_ERR_REL_MEM_FAIL   -3
#define FIFO_ERR_INVALID_MODE   -4
#define FIFO_ERR_FIFO_NOT_EXIST -5
#define FIFO_ERR_ILLEGAL_SIZE   -6
#define FIFO_ERR_INVALID_ID     -7
#define FIFO_ERR_PARAM          -8
#define FIFO_RUN_OUT_OF_ID      -9
#define FIFO_ERR_NO_INIT        -10
#define FIFO_ERR_UNKNOWN        -1000

extern INT32 AppUsb_SwFifoCreate(INT32 Size);
extern INT32 AppUsb_SwFifoDestroy(INT32 FifoID);
extern INT32 AppUsb_SwFifoOpen(INT32 FifoID, char Mode);
extern INT32 AppUsb_SwFifoClose(INT32 FifoID, char Mode);
extern INT32 AppUsb_SwFifoRead(INT32 FifoID, UINT8 *Buffer, INT32 Size, UINT32 Timeout);
extern INT32 AppUsb_SwFifoWrite(INT32 FifoID, const UINT8 *Buffer, INT32 Size, UINT32 Timeout);
extern void  AppUsb_SwFifoInitZero(void);

#endif /* APPUSB_SWFIFO_H */

