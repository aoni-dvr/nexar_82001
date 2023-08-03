/**
 *  @file ambarella_startup.h
 *
 *  @copyright Copyright (c) 2020 Ambarella, Inc.
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
 *  @details Definitions & Constants for Startup Flow
 *
 */

#ifndef __AMBARELLA_STARTUP_H
#define __AMBARELLA_STARTUP_H

uint32_t amba_get_cpu_clk(void);
void amba_init_raminfo(void);

/* AMBA clock init functions */
void amba_init_vp_clock(void);
uint32_t amba_get_vp_clock(void);

#if !defined(CONFIG_XEN_SUPPORT_QNX)

extern void Amba_init_Uart(unsigned channel, const char *init, const char *defaults);
extern void Amba_put_char(int);

extern struct callout_rtn    display_char_ambarella;
extern struct callout_rtn    poll_key_ambarella;
extern struct callout_rtn    break_detect_ambarella;

#else

extern void xen_ambarella_console_init(unsigned, const char *, const char *);
extern void xen_ambarella_console_putc(int);
extern struct callout_rtn display_char_xen_ambarella;
extern struct callout_rtn poll_key_xen_ambarella;
extern struct callout_rtn break_detect_xen_ambarella;

#endif

#endif
