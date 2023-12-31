/**
 *  @file hook.h
 *
 *  @copyright Copyright (c) 2018- Ambarella, Inc.
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
 *  @details Hook functions
 *           Refer to lwip-a.b.c/src/include/lwip/opt.h for detail.
 *
 */

#ifndef LWIP_AMBA_HOOK_H
#define LWIP_AMBA_HOOK_H

#if defined(ETHARP_SUPPORT_VLAN) && ETHARP_SUPPORT_VLAN
#include <netif/ethernet.h>

extern int myhook_vlan_chk(struct netif *netif, struct eth_hdr *eth_hdr,
                           struct eth_vlan_hdr *vlan_hdr);
extern s32_t myhook_vlan_set(struct netif *netif, struct pbuf *pbuf,
                             const struct eth_addr *src,
                             const struct eth_addr *dst,
                             u16_t eth_type);

#define LWIP_HOOK_VLAN_CHECK    myhook_vlan_chk
#define LWIP_HOOK_VLAN_SET      myhook_vlan_set
#endif // ETHARP_SUPPORT_VLAN

#endif // LWIP_AMBA_HOOK_H

