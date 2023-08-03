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

#ifndef CVTASK_INTERFACE_H_FILE
#define CVTASK_INTERFACE_H_FILE

/*-= cvtask_interface.h =-------------------------------------------------------------------------+
| CVTask interface, external API information for users designing DAGs for the CV system.          |
+------------------------------------------------------------------------------------------------*/
#include <cvtask_api.h>
#include <cvtask_errno.h>
#include <ucode_debug.h>
#include <vp_common.h> /* for vp_static_rc_t */

errcode_enum_t  cvtask_get_private_msg(uint8_t **ppBuffer);

/* Generalized utilities; accessible via JumpTable */
void  visorc_strcpy(char *dst, const char *src, uint32_t maxsize);
void  visorc_memclr(void *dst, uint32_t size);
errcode_enum_t  visorc_memcpy(void *pDst_aligned_4byte, void *pSrc_aligned_4byte, uint32_t size_multiple_of_4byte);

/* Generalized recasting converters */
#ifdef MISRA_COMPLIANCE
uint32_t  visrc_volatileptr_to_u32(volatile void *pSrc);
uint32_t  visrc_constptr_to_u32(const void *pSrc);
uint32_t  visrc_ptr_to_u32(void *pSrc);
uint32_t  printrc_constptr_to_u32(const void *pSrc);
void     *visrc_u32_to_voidptr(uint32_t srcval);
uint8_t  *visrc_u32_to_u8ptr(uint32_t srcval);
uint32_t *visrc_u32_to_u32ptr(uint32_t srcval);
char     *visrc_u32_to_charptr(uint32_t srcval);
uint8_t  *visrc_ptr_to_u8ptr(void *pSrc);
int16_t  *visrc_ptr_to_i16ptr(void *pSrc);
uint32_t *visrc_ptr_to_u32ptr(void *pSrc);
uint32_t *visrc_volptr_to_u32ptr(volatile void *pSrc);
char     *visrc_ptr_to_charptr(void *pSrc);
void     *visrc_ptr_to_voidptr(void *pSrc);
vp_static_rc_t *visrc_u32_to_vp_static_rc(uint32_t daddr);

#else /* !MISRA_COMPLIANCE */
#define visrc_volatileptr_to_u32(x)     (uint32_t)(x)
#define visrc_constptr_to_u32(x)        (uint32_t)(x)
#define visrc_ptr_to_u32(x)             (uint32_t)(x)
#define printrc_constptr_to_u32(x)      (uint32_t)(x)
#define visrc_u32_to_voidptr(x)         (void *)(x)
#define visrc_u32_to_u8ptr(x)           (uint8_t *)(x)
#define visrc_u32_to_u32ptr(x)          (uint32_t *)(x)
#define visrc_u32_to_charptr(x)         (char *)(x)
#define visrc_ptr_to_u8ptr(x)           (char *)(x)
#define visrc_ptr_to_i16ptr(x)          (int16_t *)(x)
#define visrc_ptr_to_u32ptr(x)          (uint32_t *)(x)
#define visrc_volptr_to_u32ptr(x)       (uint32_t *)(x)
#define visrc_ptr_to_charptr(x)         (char *)(x)
#define visrc_ptr_to_voidptr(x)         (void *)(x)

#define visrc_u32_to_vp_static_rc(x)    (vp_static_rc_t *)(x)

#endif /* ?MISRA_COMPLIANCE */

uint32_t  visasm_fl1(uint32_t value);
uint32_t  visasm_ff1(uint32_t value);
char      vis_dec_to_char(uint32_t value);
uint32_t  vis_digit_to_u32(char digitchar);
void      visasm_membar(void);        /* Enforce a small memory barrier */
void      visasm_large_membar(void);  /* Enforce a large memory barrier */
void      visasm_discard_l1_cache(void);  /* Discard L1 cache via single call */

#endif /* ?CVTASK_INTERFACE_H_FILE */
