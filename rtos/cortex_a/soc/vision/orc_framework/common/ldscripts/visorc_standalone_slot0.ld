/*
 * Copyright (c) 2017-2018 Ambarella, Inc.
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign copyrights.
 * This Software is also the confidential and proprietary information of
 * Ambarella, Inc. and its licensors.  You may not use, reproduce, disclose,
 * distribute, modify, or otherwise prepare derivative works of this Software
 * or any portion thereof except pursuant to a signed license agreement or
 * nondisclosure agreement with Ambarella, Inc. or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and
 * return this Software to Ambarella, Inc.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

MEMORY
        {
        corebase_sod    : ORIGIN = 0x00200000, LENGTH = 0x00200000
        corebase_vp     : ORIGIN = 0x00400000, LENGTH = 0x00200000
        slot            : ORIGIN = 0xff000000, LENGTH = 0x00100000
        cmem            : ORIGIN = 0xfffe0000, LENGTH = 0x00008000
        }
 
SECTIONS
{
      .text :
        {
        .ld_codebase_start = . ;
        *(.cvtasklist_entry)
        *(.cvtasklist_separator)
        *(.bootstrap)
        *(.text)
        } > slot
      .coretext_sod :
        {
        *(.corebase_sod_start)
        } > corebase_sod
      .coretext_vp :
        {
        *(.corebase_vp_start)
        } > corebase_vp
      .data :
        {
        *(.data)
        .ld_fmtstring_start = . ;
        *(.fmtstring_start)
        *(.fmtstring)
        .ld_fmtstring_end = . ;
        *(.fmtstring_end)
        } > slot
        PROVIDE( _fmtstring_start_offset  = .ld_fmtstring_start - .ld_codebase_start);
        PROVIDE( _fmtstring_end_offset    = .ld_fmtstring_end - .ld_codebase_start);
        PROVIDE( _cmem = ABSOLUTE(0xfffe0000) );
      .bss :
        {
        *(.bss)
        } > slot
      .cmem :
        {
        *(.cmem)
        } > cmem
}

