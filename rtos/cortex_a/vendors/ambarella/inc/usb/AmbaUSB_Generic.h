/**
 *  @file AmbaUSB_Generic.h
 *
 *  Copyright (c) 2020 Ambarella International LP
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
 *  @details USB System Generic.
 */

#ifndef AMBAUSB_GENERIC_H
#define AMBAUSB_GENERIC_H

#ifndef AMBA_TYPES_H
#include <AmbaTypes.h>
#endif

#ifndef AMBA_KAL_H
#include <AmbaKAL.h>
#endif

typedef VOID  *(*USB_TASK_ENTRY_f)(VOID *Arg);

typedef struct {
    UINT32      Priority;               // [Input] Task priority
    UINT32      AffinityMask;           // [Input] Task Affinity Mask. 0x00000001 is processor #0. 0x00000003 is processors #0 and #1.
    // [Input] 0xFFFFFFFF is all processors (#0 through #31)
    UINT32      StackSize;              // [Input] The stack size of this task
    UINT8       *StackPtr;              // [Input] The stack pointer of this task
} USB_TASKINFO_s;

typedef struct {
    UINT32      Priority;               // [Input] Task priority
    UINT32      AffinityMask;           // [Input] Task Affinity Mask. 0x00000001 is processor #0. 0x00000003 is processors #0 and #1.
    // [Input] 0xFFFFFFFF is all processors (#0 through #31)
    UINT32      StackSize;              // [Input] The stack size of this task
} USBX_TASKINFO_s;

typedef struct {
    UINT32 MajorVersion;
    UINT32 MinorVersion;
} USB_VER_INFO_s;

#endif
