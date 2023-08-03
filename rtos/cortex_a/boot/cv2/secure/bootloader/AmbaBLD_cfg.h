/**
 *  @file AmbaBLD_cfg.h
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
 *  @details Bootloader configuration
 *
 */

#ifndef AMBABLD_CFG_H
#define AMBABLD_CFG_H

/*
 * For better coding style.
 * Convert defined Kconfig as 1 or valid value,
 *         undefined Kconfig as 0 or invalid value.
 * So that using if-else to replace #if-#else-#endif
 */

#ifndef YES
#define YES (1u)
#endif

#ifndef NO
#define NO (0u)
#endif

#ifndef CONFIG_ATF_HAVE_BL2
#define BLDCFG_ATF_HAVE_BL2 (NO)
#else
#define BLDCFG_ATF_HAVE_BL2 (YES)
#endif

#ifndef CONFIG_ATF_FIP_RTOS
#define BLDCFG_ATF_FIP_RTOS (NO)
#else
#define BLDCFG_ATF_FIP_RTOS (YES)
#endif

#ifndef CONFIG_BLD_SEQ_SRTOS
#define BLDCFG_BLD_SEQ_SRTOS (NO)
#else
#define BLDCFG_BLD_SEQ_SRTOS (YES)
#endif

#ifndef CONFIG_BLD_SEQ_ATF_SRTOS
#define BLDCFG_BLD_SEQ_ATF_SRTOS (NO)
#else
#define BLDCFG_BLD_SEQ_ATF_SRTOS (YES)
#endif

#ifndef CONFIG_BLD_SEQ_LINUX
#define BLDCFG_BLD_SEQ_LINUX (NO)
#else
#define BLDCFG_BLD_SEQ_LINUX (YES)
#endif

#ifndef CONFIG_BLD_SEQ_ATF_LINUX
#define BLDCFG_BLD_SEQ_ATF_LINUX (NO)
#else
#define BLDCFG_BLD_SEQ_ATF_LINUX (YES)
#endif

#ifndef CONFIG_BLD_SEQ_ATF_XEN
#define BLDCFG_BLD_SEQ_ATF_XEN (NO)
#else
#define BLDCFG_BLD_SEQ_ATF_XEN (YES)
#endif

#ifndef CONFIG_BLD_SEQ_SHELL
#define BLDCFG_BLD_SEQ_SHELL (NO)
#else
#define BLDCFG_BLD_SEQ_SHELL (YES)
#endif

#ifndef CONFIG_BLD_SEQ_ATF
#define BLDCFG_BLD_SEQ_ATF (NO)
#else
#define BLDCFG_BLD_SEQ_ATF (YES)
#endif

#ifdef CONFIG_BOOT_CORE_LINUX
#define BLDCFG_BOOT_CORE_LINUX CONFIG_BOOT_CORE_LINUX
#else
#define BLDCFG_BOOT_CORE_LINUX (0x80000000u)
#endif
#define isValid_BLDCFG_BOOT_CORE_LINUX() ((BLDCFG_BOOT_CORE_LINUX & 0x80000000u) ? NO : YES)

#ifdef CONFIG_BOOT_CORE_SRTOS
#define BLDCFG_BOOT_CORE_SRTOS CONFIG_BOOT_CORE_SRTOS
#else
#define BLDCFG_BOOT_CORE_SRTOS (0x80000000u)
#endif
#define isValid_BLDCFG_BOOT_CORE_SRTOS() ((BLDCFG_BOOT_CORE_SRTOS & 0x80000000u) ? NO : YES)


#endif /* AMBABLD_CFG_H */

