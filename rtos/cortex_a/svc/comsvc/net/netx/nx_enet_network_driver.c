/**
 *  @file nx_enet_network_driver.c
 *
 * Copyright (c) [2020] Ambarella International LP
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
 * This file includes sample code and is only for internal testing and evaluation.  If you
 * distribute this sample code (whether in source, object, or binary code form), it will be
 * without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Enet driver for NetX
 *
 */

/* Include necessary system files.  */

#include "AmbaKAL.h"
#include "AmbaENET.h"
#include "NetStack.h"
#include "EnetUtility.h"
#include "NetXStack.h"

#define NX_LINK_MTU     (ETH_FRAMES_SIZE - 14U)


/* Define Ethernet address format.  This is prepended to the incoming IP
   and ARP/RARP messages.  The frame beginning is 14 bytes, but for speed
   purposes, we are going to assume there are 16 bytes free in front of the
   prepend pointer and that the prepend pointer is 32-bit aligned.

    Byte Offset     Size            Meaning

        0           6           Destination Ethernet Address
        6           6           Source Ethernet Address
        12          2           Ethernet Frame Type, where:

                                        0x0800 -> IP Datagram
                                        0x0806 -> ARP Request/Reply
                                        0x0835 -> RARP request reply

        42          18          Padding on ARP and RARP messages only.  */

#define NX_ETHERNET_IP      0x0800U
#define NX_ETHERNET_ARP     0x0806U
#define NX_ETHERNET_RARP    0x8035U
#define NX_ETHERNET_SIZE    14U

/* For the simulated ethernet driver, physical addresses are allocated starting
   at the preset value and then incremented before the next allocation.  */

/* Define driver prototypes.  */

VOID nx_enet_network_driver(NX_IP_DRIVER *driver_req_ptr, const NETX_STACK_CONFIG_s *pNetXConfig);
void nx_enet_network_driver_output(const NX_IP *ip_ptr, NX_PACKET *packet_ptr, UINT32 Idx);
void nx_enet_network_driver_receive(NX_IP *ip_ptr, const NETX_STACK_CONFIG_s *pNetXConfig, const void *frame, UINT16 RxLen, const UINT16 type);
void nx_convert_little_endia(UINT32 *arg);

#define NX_MAX_RAM_INTERFACES NETX_STACK_INSTANCES


typedef struct
{
    UINT                 nx_ram_network_driver_in_use;

    UINT                 nx_ram_network_driver_id;

    NX_INTERFACE        *nx_ram_driver_interface_ptr;

    NX_IP               *nx_ram_driver_ip_ptr;

    UINT32               nx_ram_address_msw;
    UINT32               nx_ram_address_lsw;

} nx_driver_instance;

static nx_driver_instance nx_ram_driver[NX_MAX_RAM_INTERFACES];
void nx_convert_little_endia(UINT32 *arg)
{
    UINT32 i;
    UINT32 tmp;
    i = (UINT) *arg;
    /* i = A, B, C, D */
    tmp = i ^ (((i) >> 16) | (i << 16));
    /* tmp = i ^ (i ROR 16) = A^C, B^D, C^A, D^B */
    tmp &= 0xff00ffffU;
    /* tmp = A^C, 0, C^A, D^B */
    i = ((i) >> 8) | (i<<24);
    /* i = D, A, B, C */
    i = i ^ ((tmp) >> 8);
    /* i = D, C, B, A */
    *arg = i;
}

static void find_driver_instance(NX_IP_DRIVER *driver_req_ptr, const NX_IP *ip_ptr)
{
    INT i;

    if(driver_req_ptr -> nx_ip_driver_command != NX_LINK_INTERFACE_ATTACH)
    {
        for(i = 0; i < (INT)NX_MAX_RAM_INTERFACES;i++)
        {
            if(nx_ram_driver[i].nx_ram_network_driver_in_use == 0U) {
                continue;
            }

            if(nx_ram_driver[i].nx_ram_driver_ip_ptr != ip_ptr) {
                continue;
            }

            if(nx_ram_driver[i].nx_ram_driver_interface_ptr != driver_req_ptr -> nx_ip_driver_interface) {
                continue;
            }

            break;
        }

        if(i == (INT)NX_MAX_RAM_INTERFACES)
        {
            driver_req_ptr -> nx_ip_driver_status =  NX_INVALID_INTERFACE;
            driver_req_ptr -> nx_ip_driver_command = 0xff;
        }
    }
}

static void nx_link_interface_attach(const UINT8 *const hwaddr, NX_IP_DRIVER *driver_req_ptr, NX_IP *ip_ptr)
{
    INT i;

    /* Find an available driver instance to attach the interface. */
    for(i = 0; i < (INT)NX_MAX_RAM_INTERFACES;i++)
    {
        if(nx_ram_driver[i].nx_ram_network_driver_in_use == 0U) {
            break;
        }
    }
    /* An available entry is found. */
    if(i < (INT)NX_MAX_RAM_INTERFACES)
    {
        /* Set the IN USE flag.*/
        nx_ram_driver[i].nx_ram_network_driver_in_use  = 1U;

        nx_ram_driver[i].nx_ram_network_driver_id = (UINT)i;

        /* Record the interface attached to the IP instance. */
        nx_ram_driver[i].nx_ram_driver_interface_ptr = driver_req_ptr -> nx_ip_driver_interface;

        /* Record the IP instance. */
        nx_ram_driver[i].nx_ram_driver_ip_ptr = ip_ptr;
        nx_ram_driver[i].nx_ram_address_msw = ((UINT32)hwaddr[0] << 8) | (UINT32)hwaddr[1];
        nx_ram_driver[i].nx_ram_address_lsw = ((UINT32)hwaddr[2] << 24) |
                                          ((UINT32)hwaddr[3] << 16) |
                                          ((UINT32)hwaddr[4] << 8) |
                                          (UINT32)hwaddr[5];
    }
    else {
        driver_req_ptr -> nx_ip_driver_status =  NX_INVALID_INTERFACE;
    }
}

static void build_ethernet_frame(UINT32 *ethernet_frame_ptr, const NX_IP_DRIVER *driver_req_ptr, const NX_INTERFACE *interface_ptr)
{
    ethernet_frame_ptr[0] =  driver_req_ptr -> nx_ip_driver_physical_address_msw;
    ethernet_frame_ptr[1] =  driver_req_ptr -> nx_ip_driver_physical_address_lsw;
    ethernet_frame_ptr[2] =  (interface_ptr -> nx_interface_physical_address_msw << 16) |
                             (interface_ptr -> nx_interface_physical_address_lsw >> 16);
    ethernet_frame_ptr[3] =  (interface_ptr -> nx_interface_physical_address_lsw << 16);

    if(driver_req_ptr -> nx_ip_driver_command == NX_LINK_ARP_SEND) {
        ethernet_frame_ptr[3] |= NX_ETHERNET_ARP;
    } else if(driver_req_ptr -> nx_ip_driver_command == NX_LINK_ARP_RESPONSE_SEND) {
        ethernet_frame_ptr[3] |= NX_ETHERNET_ARP;
    } else if(driver_req_ptr -> nx_ip_driver_command == NX_LINK_RARP_SEND) {
        ethernet_frame_ptr[3] |= NX_ETHERNET_RARP;
    } else {
        ethernet_frame_ptr[3] |= NX_ETHERNET_IP;
    }
}

VOID nx_enet_network_driver(NX_IP_DRIVER *driver_req_ptr, const NETX_STACK_CONFIG_s *pNetXConfig)
{
    INT             i =  0;
    NX_IP           *ip_ptr;
    NX_PACKET       *packet_ptr;
    UINT32          *ethernet_frame_ptr;
    NX_INTERFACE    *interface_ptr;
    UINT32 Idx = pNetXConfig->Idx;
    const UINT8 *const hwaddr = &pNetXConfig->Mac[0];


    /* Setup the IP pointer from the driver request.  */
    ip_ptr =  driver_req_ptr -> nx_ip_driver_ptr;

    /* Default to successful return.  */
    driver_req_ptr -> nx_ip_driver_status =  NX_SUCCESS;

    /* Setup interface pointer.  */
    interface_ptr = driver_req_ptr -> nx_ip_driver_interface;

    /* Find out the driver instance if the driver command is not ATTACH. */
    find_driver_instance(driver_req_ptr, ip_ptr);

    NetXDebug("[eth%d] cmd %u", Idx, driver_req_ptr->nx_ip_driver_command);
    /* Process according to the driver request type in the IP control
       block.  */
    switch (driver_req_ptr -> nx_ip_driver_command)
    {

        case NX_LINK_INTERFACE_ATTACH:
        {
            nx_link_interface_attach(hwaddr, driver_req_ptr, ip_ptr);
            break;
        }

        case NX_LINK_INITIALIZE:
        {

            /* Process driver initialization.  */
#ifdef NX_DEBUG
            printf("NetX RAM Driver Initialization - %s\n", ip_ptr -> nx_ip_name);
            printf("  IP Address =%08X\n", ip_ptr -> nx_ip_address);
#endif

            /* Setup the link maximum transfer unit. Note that the MTU should
               take into account the physical header needs and alignment
               requirements. For example, we are going to report actual
               MTU less the ethernet header and 2 bytes to keep alignment. */
            interface_ptr -> nx_interface_ip_mtu_size =  NX_LINK_MTU;

            /* Setup the physical address of this IP instance.  Increment the
               physical address lsw to simulate multiple nodes hanging on the
               ethernet.  */
            interface_ptr -> nx_interface_physical_address_msw =  nx_ram_driver[i].nx_ram_address_msw;
            interface_ptr -> nx_interface_physical_address_lsw =  nx_ram_driver[i].nx_ram_address_lsw;

            /* Indicate to the IP software that IP to physical mapping
               is required.  */
            interface_ptr -> nx_interface_address_mapping_needed =  1U;

            break;
        }

        case NX_LINK_UNINITIALIZE:
        {

            /* Zero out the driver instance. */
            if (AmbaWrap_memset(&(nx_ram_driver[i]), 0 , sizeof(nx_driver_instance))!= 0U) { }
            break;
        }

        case NX_LINK_ENABLE:
        {

            /* Process driver link enable.  */

            /* In the RAM driver, just set the enabled flag.  */
            interface_ptr -> nx_interface_link_up = 1U;

#ifdef NX_DEBUG
            printf("NetX RAM Driver Link Enabled - %s\n", ip_ptr -> nx_ip_name);
#endif
            break;
        }

        case NX_LINK_DISABLE:
        {

            /* Process driver link disable.  */

            /* In the RAM driver, just clear the enabled flag.  */
            interface_ptr -> nx_interface_link_up =  0U;

#ifdef NX_DEBUG
            printf("NetX RAM Driver Link Disabled - %s\n", ip_ptr -> nx_ip_name);
#endif
            break;
        }

        case NX_LINK_PACKET_SEND:
        case NX_LINK_PACKET_BROADCAST:
        case NX_LINK_ARP_SEND:
        case NX_LINK_ARP_RESPONSE_SEND:
        case NX_LINK_RARP_SEND:
        {
            const UCHAR *pstr;
            ULONG addr;
            /* Process driver send packet.  */

            /* Place the ethernet frame at the front of the packet.  */
            packet_ptr =  driver_req_ptr -> nx_ip_driver_packet;

            /* Adjust the prepend pointer.  */
            // packet_ptr -> nx_packet_prepend_ptr =  packet_ptr -> nx_packet_prepend_ptr - NX_ETHERNET_SIZE;
            AmbaMisra_TypeCast(&addr, &packet_ptr->nx_packet_prepend_ptr);
            addr -= NX_ETHERNET_SIZE;
            AmbaMisra_TypeCast(&packet_ptr->nx_packet_prepend_ptr, &addr);

            /* Adjust the packet length.  */
            packet_ptr -> nx_packet_length =  packet_ptr -> nx_packet_length + (UINT32)NX_ETHERNET_SIZE;

            /* Setup the ethernet frame pointer to build the ethernet frame.  Backup another 2
               bytes to get 32-bit word alignment.  */
            AmbaMisra_TypeCast(&pstr, &packet_ptr->nx_packet_prepend_ptr);
            // pstr = pstr - 2;
            pstr--;
            pstr--;
            AmbaMisra_TypeCast(&ethernet_frame_ptr, &pstr);

            /* Build the ethernet frame.  */
            build_ethernet_frame(ethernet_frame_ptr, driver_req_ptr, interface_ptr);

            /* Endian swapping if NX_LITTLE_ENDIAN is defined.  */
            nx_convert_little_endia(&ethernet_frame_ptr[0]);
            nx_convert_little_endia(&ethernet_frame_ptr[1]);
            nx_convert_little_endia(&ethernet_frame_ptr[2]);
            nx_convert_little_endia(&ethernet_frame_ptr[3]);
#ifdef NX_DEBUG_PACKET
            printf("NetX RAM Driver Packet Send - %s\n", ip_ptr -> nx_ip_name);
#endif
            nx_enet_network_driver_output(ip_ptr, packet_ptr, Idx);
            break;
        }

        case NX_LINK_MULTICAST_JOIN:
        {

            /* For real ethernet devices the hardware registers that support IP multicast
               need to be searched for an open entry.  If found, the multicast ethernet
               address contained in the driver request structure
               (nx_ip_driver_physical_address_msw & nx_ip_driver_physical_address_lsw)
               needs to be loaded into ethernet chip.  If no free entries are found,
               an NX_NO_MORE_ENTRIES error should be returned to the caller.  */
            break;
        }

        case NX_LINK_MULTICAST_LEAVE:
        {

            /* For real ethernet devices the hardware registers that support IP multicast
               need to be searched for a matching entry.  If found, the multicast ethernet
               address should be cleared in the hardware so that a new entry may use it
               on the next join operation.  */
            break;
        }

        case NX_LINK_GET_STATUS:
        {

            /* Return the link status in the supplied return pointer.  */
            *(driver_req_ptr -> nx_ip_driver_return_ptr) =  interface_ptr->nx_interface_link_up;
            break;
        }

        case NX_LINK_GET_SPEED:
        {

            /* Return the link's line speed in the supplied return pointer. Unsupported feature.  */
            *(driver_req_ptr -> nx_ip_driver_return_ptr) = 0;
            break;
        }

        case NX_LINK_GET_DUPLEX_TYPE:
        {

            /* Return the link's line speed in the supplied return pointer. Unsupported feature.  */
            *(driver_req_ptr -> nx_ip_driver_return_ptr) = 0;
            break;
        }

        case NX_LINK_GET_ERROR_COUNT:
        {

            /* Return the link's line speed in the supplied return pointer. Unsupported feature.  */
            *(driver_req_ptr -> nx_ip_driver_return_ptr) = 0;
            break;
        }

        case NX_LINK_GET_RX_COUNT:
        {

            /* Return the link's line speed in the supplied return pointer. Unsupported feature.  */
            *(driver_req_ptr -> nx_ip_driver_return_ptr) = 0;
            break;
        }

        case NX_LINK_GET_TX_COUNT:
        {

            /* Return the link's line speed in the supplied return pointer. Unsupported feature.  */
            *(driver_req_ptr -> nx_ip_driver_return_ptr) = 0;
            break;
        }

        case NX_LINK_GET_ALLOC_ERRORS:
        {

            /* Return the link's line speed in the supplied return pointer. Unsupported feature.  */
            *(driver_req_ptr -> nx_ip_driver_return_ptr) = 0;
            break;
        }

        case NX_LINK_DEFERRED_PROCESSING:
        {

            /* Driver defined deferred processing... this is typically used to defer interrupt
               processing to the thread level. In this driver, nothing is done here!  */
            break;
        }

        default:
        {

            /* Invalid driver request.  */

            /* Return the unhandled command status.  */
            driver_req_ptr -> nx_ip_driver_status =  NX_UNHANDLED_COMMAND;

#ifdef NX_DEBUG
            printf("NetX RAM Driver Received invalid request - %s\n", ip_ptr -> nx_ip_name);
#endif
            break;
        }
    }
}

void nx_enet_network_driver_output(const NX_IP *ip_ptr, NX_PACKET *packet_ptr, UINT32 Idx)
{
    UINT8 *txbuf;
    const void *ethpld;
    void *ptr;
    UINT16 lenpld, lenfull;
    NETX_STACK_CONFIG_s *pNetXConfig = NULL;
    const ETH_HDR_s *ethhdr;
#ifdef ENET_PREEMPT
    UINT32 old_threshold;
    extern UINT _tx_thread_preemption_change(TX_THREAD *thread_ptr, UINT new_threshold, UINT *old_threshold);
    extern TX_THREAD *_tx_thread_identify(VOID);
#ifndef tx_thread_identify
#define tx_thread_identify                          _tx_thread_identify
#endif
#ifndef tx_thread_preemption_change
#define tx_thread_preemption_change                 _tx_thread_preemption_change
#endif
#endif
#if defined(ETHDEBUG)
    extern UINT32 ENETQUIET;
    /*Be Quiet to Prevent AmbaShell exception*/
    ENETQUIET = 1U;
#endif
    (void)ip_ptr;
    (void)NetXStack_GetConfig(Idx, &pNetXConfig);

    if (pNetXConfig != NULL) {
        (void)pNetXConfig->pGetTxBufCb(Idx, &ptr);
        AmbaMisra_TypeCast(&txbuf, &ptr);

        /* Remove the Ethernet header.  In real hardware environments, this is typically
        done after a transmit complete interrupt.  */
        AmbaMisra_TypeCast(&ethhdr, &packet_ptr->nx_packet_prepend_ptr);

        packet_ptr->nx_packet_prepend_ptr = &packet_ptr->nx_packet_prepend_ptr[NX_ETHERNET_SIZE];
        AmbaMisra_TypeCast(&ethpld, &packet_ptr->nx_packet_prepend_ptr);

        /* Adjust the packet length.  */
        lenfull = (UINT16)(packet_ptr->nx_packet_length);
        packet_ptr->nx_packet_length = packet_ptr->nx_packet_length - (UINT32)NX_ETHERNET_SIZE;
        lenpld = (UINT16)(packet_ptr->nx_packet_length);

    #ifdef ENET_PREEMPT
        /* Disable preemption.  */
        (void) tx_thread_preemption_change(tx_thread_identify(), 0, &old_threshold);
    #endif

        if (pNetXConfig->tci != 0U) {
            /* Insert the Ethernet header */
            lenfull = lenpld;
            AmbaMisra_TypeCast(&ptr, &txbuf);
            if (AmbaWrap_memcpy(ptr, ethhdr, 12)!= 0U) { }
            (void)AmbaNetStack_InsertEthHdr(&ptr, &lenfull, pNetXConfig->tci, AmbaNetStack_Ntohs(ethhdr->type));
            AmbaMisra_TypeCast(&txbuf, &ptr);

            /* Insert the Ethernet payload */
            if (AmbaWrap_memcpy(txbuf, ethpld, lenpld)!= 0U) { }
        } else {
            if (AmbaWrap_memcpy(txbuf, ethhdr, lenfull)!= 0U) { }
        }

        /* direct_tx: netx blocking until tx complete */
        (void) pNetXConfig->pDoTxCb(Idx, lenfull);

        /* free nx packet because we have copied to dma buf */
        /* Now that the Ethernet frame has been removed, release the packet.  */
        if (0U != nx_packet_transmit_release(packet_ptr)) {
            NetXDebug("nx_packet_transmit_release[%d] ERR: nx_packet_prepend_ptr %p nx_packet_data_start %p, nx_packet_length %d",
                Idx, packet_ptr->nx_packet_prepend_ptr, packet_ptr ->nx_packet_data_start, packet_ptr->nx_packet_length);
        }

    #ifdef ENET_PREEMPT
        /* Restore preemption.  */
        (void) tx_thread_preemption_change(tx_thread_identify(), old_threshold, &old_threshold);
    #endif

    #if defined(ETHDEBUG)
        ENETQUIET = 0U;
    #endif
    }
}

void nx_enet_network_driver_receive(NX_IP *ip_ptr, const NETX_STACK_CONFIG_s *pNetXConfig, const void *frame, UINT16 RxLen, const UINT16 type)
{
    UINT32 Idx = pNetXConfig->Idx;
    UINT32 nIdx = 0U;
    NX_PACKET *packet_ptr = NULL;
    void *ptr;
    UINT16 Len1 = RxLen;
    NX_DRIVER nx0 = pNetXConfig->pNxIp->nx_ip_interface[0].nx_interface_link_driver_entry;
    extern NX_DRIVER NetXDriver[NX_MAX_RAM_INTERFACES];
    const UINT32 icmp_const = NX_ICMP_PACKET;
    const UINT32 ip_const = NX_IP_PACKET;

    /* Pickup the packet header to determine where the packet needs to be
       sent.  */


    /* Setup interface pointer.  */


    /* Route the incoming packet according to its ethernet type.  */
    AmbaMisra_TypeCast(&ptr, &frame);

    switch (type) {
    case NX_ETHERNET_IP:
        /* Note:  The length reported by some Ethernet hardware includes bytes after the packet
           as well as the Ethernet header.  In some cases, the actual packet length after the
           Ethernet header should be derived from the length in the IP header (lower 16 bits of
           the first 32-bit word).  */

        /* Clean off the Ethernet header.  */

        /* Adjust the packet length.  */

        /* Route to the ip receive function.  */
        (void) nx_packet_allocate(ip_ptr->nx_ip_default_packet_pool, &packet_ptr, ip_const, KAL_WAIT_FOREVER);
        (void) AmbaNetStack_RemoveEthHdr(&ptr, &Len1);

        if (packet_ptr != NULL) {
            if (AmbaWrap_memcpy(packet_ptr->nx_packet_prepend_ptr, ptr, (UINT32)Len1)!= 0U) { }
            packet_ptr->nx_packet_length     = (UINT32)Len1;
            packet_ptr->nx_packet_append_ptr = &packet_ptr->nx_packet_prepend_ptr[packet_ptr->nx_packet_length];

            if (nx0 == NetXDriver[0]) {
                nIdx = Idx;
            }
    #if (NX_MAX_RAM_INTERFACES > 1U)
            else {
                if (nx0 == NetXDriver[1]) {
                    nIdx = (Idx + 1U) % 2U;
                }
            }
    #endif
            NetXDebug("eth%d nx%d Rx type 0x%x", Idx, nIdx, type);
            packet_ptr->nx_packet_ip_interface = nx_ram_driver[nIdx].nx_ram_driver_interface_ptr;
    #ifdef NX_DIRECT_ISR_CALL
            _nx_ip_packet_receive(ip_ptr, packet_ptr);
    #else
            _nx_ip_packet_deferred_receive(ip_ptr, packet_ptr);
    #endif
        }
        break;

    case NX_ETHERNET_ARP:
        /* Clean off the Ethernet header.  */

        /* Adjust the packet length.  */

        /* Route to the ARP receive function.  */
        (void) nx_packet_allocate(ip_ptr->nx_ip_default_packet_pool, &packet_ptr, icmp_const, KAL_WAIT_FOREVER);
        (void) AmbaNetStack_RemoveEthHdr(&ptr, &Len1);
        if (packet_ptr != NULL) {
            if (AmbaWrap_memcpy(packet_ptr->nx_packet_prepend_ptr, ptr, (UINT32)Len1)!= 0U) { }
            packet_ptr->nx_packet_length     = (UINT32)Len1;
            packet_ptr->nx_packet_append_ptr = &packet_ptr->nx_packet_prepend_ptr[packet_ptr->nx_packet_length];
            if (nx0 == NetXDriver[0]) {
                nIdx = Idx;
            }
    #if (NX_MAX_RAM_INTERFACES > 1U)
            else {
                if (nx0 == NetXDriver[1]) {
                    nIdx = (Idx + 1U) % 2U;
                }
            }
    #endif
            NetXDebug("eth%d nx%d Rx type 0x%x", Idx, nIdx, type);
            packet_ptr->nx_packet_ip_interface = nx_ram_driver[nIdx].nx_ram_driver_interface_ptr;

            _nx_arp_packet_deferred_receive(ip_ptr, packet_ptr);
        }
        break;
    default:
        EnetInfo(__func__, __LINE__, NETX_MODULE_ID, ANSI_RED, "Unknown Ethernet type");
        break;
    }
}
