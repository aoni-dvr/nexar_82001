/**
 *  @file AmbaTransfer_impl.h
 *
 *  @copyright Copyright (c) 2018 Ambarella, Inc.
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
 *  @details Header of AmbaTransfer interface implementation
 *
 */

#ifndef AMBATRANSFER_IMPL_H
#define AMBATRANSFER_IMPL_H

typedef unsigned int UINT32;
typedef int INT32;
typedef unsigned char UINT8;
typedef unsigned short UINT16;

typedef INT32 (*AmbaTransfer_RecvCBFunc)(UINT32 PayloadSize, UINT8 *PayloadAddr);

/******
 * One Packet is defined as the payload to a single send().
 * Depends on low level transfer interface implementation, it might be seperated into one or more DataBlk when transferred.
 *
 * When a DataBlk is send out, there will be always a header before it.
 * The whole thing transfered over the interface looks like:
 *
 *      |Header|DataBlk|Header|DataBlk|...
 *
 * The Header contents these info:
 *  1. Magic Number for Header identification. It is always 0xAB.
 *  2. Version Number of Header. Could be 0x1~0xf.
 *  3. Mark bit. This bit will be set as 0x1 to indecate this is last DataBlk of the payload.
 *  4. Size. The actual size of DataBlk. (Does not include Header size)
 *  5. Checksum. CheckSum value for the DataBlk. (Does not include Header contents)
 ******/

typedef struct {
    UINT32 (*Init)(void *Input);
    UINT32 (*Release)(void *Input);
    UINT32 (*Register)(void *Input);
    UINT32 (*Unregister)(void *Input);
    UINT32 (*Recv)(void *Input);
    UINT32 (*Connect)(void *Input);
    UINT32 (*Disconnect)(void *Input);
    UINT32 (*Send)(void *Input);
    UINT32 (*GetStatus)(void *Input);
} AMBA_TRANSFER_IMPL_s;

extern AMBA_TRANSFER_IMPL_s AmbaTransfer_Impl_ThXQueue;
extern AMBA_TRANSFER_IMPL_s Transfer_Impl_Memio_Inst;

#endif /* AMBATRANSFER_IMPL_H */

