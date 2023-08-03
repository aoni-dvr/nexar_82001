/**
 * @file EnetUtility.c
 * Enet common utility
 *
 * @ingroup enet
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
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "AmbaENET.h"

#if defined(CONFIG_THREADX)
#include "AmbaUtility.h"
#include "AmbaPrint.h"
#include "NetStack.h"
#endif

#include "EnetUtility.h"

#if defined(ETHDEBUG) || defined(AVBDEBUG) || defined(ETHDEBUG_ISR)
#if defined(CONFIG_THREADX)
#include "AmbaKAL.h"
#include "AmbaUART.h"
#endif
#include <stdio.h>
#include <stdarg.h>
UINT32 ENETQUIET = 0U;
#endif

#ifdef ETHDEBUG_ISR
static char EnetDebugQueueBuffer[16][64];
#if defined(CONFIG_THREADX)
static AMBA_KAL_TASK_t EnetDebugTask = { 0 };
#define EnetDebugTaskStackSize (8192)
static UINT8 EnetDebugTaskStack[EnetDebugTaskStackSize] = { 0 };
#define EnetDebugTaskPriority  (160)
static AMBA_KAL_MSG_QUEUE_t EnetDebugQueue;
static UINT32 EnetDebugQueueBase[16];
#else
sem_t EnetDebugQueue;
#endif
#endif

/******* debug start *******/
#ifdef ETHDEBUG_ISR
static void *EnetDebugTaskEntry(void *EntryArg)
{
#if defined(CONFIG_THREADX)
    UINT32 msg;
    /*msg = (UINT32)EntryArg;*/
    if (AmbaWrap_memcpy(&msg, &EntryArg, sizeof(msg))!= 0U) { }
#else
    static UINT32 msg = 0;
#endif

    while (1) {
#if defined(CONFIG_THREADX)
        (void) AmbaKAL_MsgQueueReceive(&EnetDebugQueue, &msg, AMBA_KAL_WAIT_FOREVER);
#else
        sem_wait(&EnetDebugQueue);
        msg++;
        msg %= 16U;
#endif
#if defined(ETHDEBUG)
        EnetDebug("\x1b" "[1;36m ENET %s" "\x1b" "[0m", EnetDebugQueueBuffer[msg]);
#elif defined(AVBDEBUG)
        AvbDebug("\x1b" "[1;36m ENET %s" "\x1b" "[0m", EnetDebugQueueBuffer[msg]);
#else
        AmbaPrint_ModulePrintStr5(ETH_MODULE_ID, "%s %s %s", ANSI_CYAN, EnetDebugQueueBuffer[msg], ANSI_RESET, NULL, NULL);
#endif
    }
    return NULL;
}

#if defined(CONFIG_THREADX)
void EnetDebugTaskCreate(void)
{
    UINT32 err;

    if (EnetDebugQueue.tx_queue_id == 0) {
        if (OK != AmbaKAL_MsgQueueCreate(&EnetDebugQueue, NULL, sizeof(UINT32), EnetDebugQueueBase, sizeof(EnetDebugQueueBase))) {
            EnetDebug("ENET %s(%d) AmbaKAL_MsgQueueCreate failed!", __func__, __LINE__);
        }
    }

    if (EnetDebugTask.tx_thread_id == 0) {
        void *EnetDebugTaskEntry(void *EntryArg);
        err = AmbaKAL_TaskCreate(&EnetDebugTask,
                                 "EnetDebugTask",
                                 EnetDebugTaskPriority,
                                 EnetDebugTaskEntry,
                                 NULL,
                                 (void *) EnetDebugTaskStack,
                                 EnetDebugTaskStackSize,
                                 0U);
        if (err != 0U) {
            EnetDebug("ENET %s(%d) failed!", __func__, __LINE__);
        }

        // Using Core-0 (0x01)
        err = AmbaKAL_TaskSetSmpAffinity(&EnetDebugTask, 1U);
        if (err != 0U) {
            EnetDebug("ENET %s(%d) AmbaKAL_TaskSmpCoreExclusionSet failed!", __func__, __LINE__);
        }
        err = (UINT32)AmbaKAL_TaskResume(&EnetDebugTask);
        if (err != 0U) {
            EnetDebug("ENET %s(%d) AmbaKAL_TaskResume failed!", __func__, __LINE__);
        }
        EnetDebug("ENET %s(%d) ID: 0x%08X", __func__, __LINE__, &EnetDebugTask);
    }
}
#else
void EnetDebugTaskCreate(void)
{
    typedef void *(*pthread_f) (void *);
    pthread_t tid;
    sem_init(&EnetDebugQueue, 0, 0);
    pthread_create(&tid, NULL, (pthread_f)&EnetDebugTaskEntry, NULL);
}
#endif
#endif

void EnetInfo(const char *func, const UINT32 line, const UINT16 ModuleID, const char *color, const char *msg)
{
#if defined(CONFIG_THREADX)
    char Buf[22];

    (void)AmbaUtility_UInt32ToStr(Buf, sizeof(Buf), line, 10U);
    AmbaPrint_ModulePrintStr5(ModuleID, "%s ENET %s(%s) %s %s", color, func, Buf, msg, ANSI_RESET);
#else
    fprintf(stderr, "%s ENET %s(%d) %s %s\n", color, func, line, msg, ANSI_RESET);
#endif
}

void EnetDebug(const char *fmt, ...)
{
#ifdef ETHDEBUG
    UINT8 Buf[512];
    va_list args;
    INT32 n;
#if defined(CONFIG_THREADX)
    UINT32 SentSize;
#endif

    va_start(args, fmt);
    n = vsnprintf((char *)Buf, 512-3, (const char *)fmt, args);
    Buf[n] = (UINT8)'\r';
    Buf[n+1] = (UINT8)'\n';
    Buf[n+2] = (UINT8)'\0';
    va_end(args);
    if (ENETQUIET == 0U) {
#if defined(CONFIG_THREADX)
        (void) AmbaUART_Write(0U, 0U, n + 3, Buf, &SentSize, 1U);
#else
        fprintf(stderr, "%s", Buf);
#endif
    }
#else
    const char *dummy;

    dummy = fmt;
    (void)dummy;
#endif
}

#ifdef CONFIG_AMBA_AVB
void AvbDebug(const char *fmt, ...)
{
#ifdef AVBDEBUG
    UINT8 Buf[512];
    va_list args;
    INT32 n;
#if defined(ETHDEBUG)
    UINT32 SentSize;
#endif

    va_start(args, fmt);
    n = vsnprintf((char *)Buf, 512-3, (const char *)fmt, args);
    Buf[n] = (UINT8)'\r';
    Buf[n+1] = (UINT8)'\n';
    Buf[n+2] = (UINT8)'\0';
    va_end(args);
#ifdef ETHDEBUG
#if defined(CONFIG_THREADX)
    (void) AmbaUART_Write(0U, 0U, n + 3, Buf, &SentSize, 1U);
#else
    fprintf(stderr, "%s", Buf);
#endif
#else
    AmbaPrint_ModulePrintStr5(ETH_MODULE_ID, "%s", Buf, NULL, NULL, NULL, NULL);
#endif
#else
    const char *dummy;

    dummy = fmt;
    (void)dummy;
#endif
}
#endif

void EnetIsrDebug(const char *fmt, ...)
{
#ifdef ETHDEBUG_ISR
    static UINT32 msg = 0;
    va_list args;

    va_start(args, fmt);
    if (vsnprintf(EnetDebugQueueBuffer[msg], 64, fmt, args) >= 64) {
        EnetDebugQueueBuffer[msg][63] = '\0';
    }
    va_end(args);
#if defined(CONFIG_THREADX)
    (void) AmbaKAL_MsgQueueSend(&EnetDebugQueue, &msg, AMBA_KAL_NO_WAIT);
#else
    //sem_post(&EnetDebugQueue);
    fprintf(stderr, "\x1b" "[1;36m ENET %s\n" "\x1b" "[0m", EnetDebugQueueBuffer[msg]);
#endif
    msg++;
    msg %= 16U;
#else
    const char *dummy;

    dummy = fmt;
    (void)dummy;
#endif
}

#ifdef ETHDEBUG_DUMP
static void EnetDebugRaw(const char *fmt, ...)
{
    UINT8 Buf[512];
    va_list args;
    INT32 n;
#if defined(CONFIG_THREADX)
    UINT32 SentSize;
#endif

    va_start(args, fmt);
    n = vsnprintf((char *)Buf, 512-1, (const char *)fmt, args);
    Buf[n] = '\0';
    va_end(args);
#if defined(CONFIG_THREADX)
    AmbaUART_Write(0U, 0U, n + 1, Buf, &SentSize, 1U);
#else
    fprintf(stderr, "%s", Buf);
#endif
}
#endif

void ambhw_dump_buffer(const char *msg, const UINT8 *data, UINT32 length)
{
#ifdef ETHDEBUG_DUMP
    UINT32 i;

    if (data == NULL) {
        EnetDebugRaw("%s(%d) NULL \r\n",  __func__, __LINE__);
        return;
    }
    if (ENETQUIET == 0U) {
        if (msg != NULL) {
            EnetDebugRaw("%s %s:", msg, __func__);
        }
        for (i = 0U; i < length; i++) {
            if (i % 16U == 0U) {
                EnetDebugRaw("\r\n%03X:", i);
            } else if (i % 8U == 0U) {
                EnetDebugRaw(" ");
            }
            EnetDebugRaw(" %02x", data[i]);
        }
        EnetDebugRaw("\r\n");
    }
#else
    const UINT8 *foo;

    foo = data;
    (void)msg;
    (void)foo;
    (void)length;
#endif
}

/******* debug end *******/
#if defined(CONFIG_THREADX)
/**
 * @ingroup enet
 * This function converts 4 separate IPv4 digits into one UINT32 value
 *
 * @param [in] a       IP address
 * @param [in] b       IP address
 * @param [in] c       IP address
 * @param [in] d       IP address
 * @return Converted value
 */
UINT32 AmbaNetStack_Digits2IP(UINT32 a, UINT32 b, UINT32 c, UINT32 d)
{
  return ((a & 0xffU) << 24) |
         ((b & 0xffU) << 16) |
         ((c & 0xffU) << 8) |
         ((d & 0xffU));
}

/* if tci not NULL, insert VLAN Tag, increase Addr & increase pLen */
UINT32 AmbaNetStack_InsertVlan(void **Addr, UINT16 *pLen, UINT16 tci, UINT16 type)
{
    UINT16 *buf16;
    void *ptr = *Addr;

    if (tci != 0U) {
#if defined(AMBA_MISRA_FIX_H)
        AmbaMisra_TypeCast(&buf16, &ptr);
#else
        buf16 = (UINT16 *)ptr;
#endif
        *buf16 = AmbaNetStack_Htons(tci);
        buf16 = &buf16[1];
        *buf16 = AmbaNetStack_Htons(type);
        buf16 = &buf16[1];
#if defined(AMBA_MISRA_FIX_H)
        AmbaMisra_TypeCast(&ptr, &buf16);
#else
        ptr = (void *)buf16;
#endif

        *Addr = ptr;
        *pLen += 4U;
    }

    return 0;
}

/* remove VLAN tci, increase Addr & decrease pLen */
UINT32 AmbaNetStack_RemoveVlan(void **Addr, UINT16 *pLen, UINT16 *type)
{
    UINT16 *buf16;
    void *ptr = *Addr;

#if defined(AMBA_MISRA_FIX_H)
    AmbaMisra_TypeCast(&buf16, &ptr);
#else
    buf16 = (UINT16 *)ptr;
#endif
    buf16 = &buf16[1];
    *type = AmbaNetStack_Ntohs(*buf16);
    buf16 = &buf16[1];
#if defined(AMBA_MISRA_FIX_H)
    AmbaMisra_TypeCast(&ptr, &buf16);
#else
    ptr = (void *)buf16;
#endif

    *Addr = ptr;
    *pLen -= 4U;

    return 0;
}

/**
 * @ingroup enet
 * This function updates ethernet header
 * it inserts the 802.1Q Tag Control Info value if the value is not zeo
 *
 * @param [in] Addr    Addr points to start address of ethernet header
 * @param [in] pLen    pLen value equals to size of ethernet packet payload
 * @param [in] tci     802.1Q Tag Control Info value in ethernet header
 * @param [in] type    type value to be inserted in ethernet header
 * @param [in] Addr    Addr value will be increased and points to the start address of ethernet packet payload
 * @param [in] pLen    pLen value will be increased and equals to size of a complete ethernet frame
 * @return Converted value
 */
UINT32 AmbaNetStack_InsertEthHdr(void **Addr, UINT16 *pLen, UINT16 tci, UINT16 type)
{
    ETH_VLAN_HDR_s *vlanhdr;
    const UINT16 vlanhdrlen = (UINT16)sizeof(ETH_VLAN_HDR_s);
    ETH_HDR_s *ethhdr;
    const UINT16 ethhdrlen = (UINT16)sizeof(ETH_HDR_s);
    UINT8 *buf;
    void *frame;

    frame = *Addr;
#if defined(AMBA_MISRA_FIX_H)
    AmbaMisra_TypeCast(&buf, &frame);
    AmbaMisra_TypeCast(&ethhdr, &frame);
    AmbaMisra_TypeCast(&vlanhdr, &frame);
#else
    buf = (UINT8 *)frame;
    ethhdr = (ETH_HDR_s *)frame;
    vlanhdr = (ETH_VLAN_HDR_s *)frame;
#endif
    /* if tci not NULL, insert VLAN Tag */
    if (tci != 0U) {
        /** Tag Protocol Identifier = 0x8100 */
        vlanhdr->vlan.tpid = AmbaNetStack_Htons(ETH_VLAN_TYPE);
        /* tci Bit[11:0]  VID (VLAN ID) */
        /* tci Bit[15:13] PCP (Priority Code Point) */
        vlanhdr->vlan.tci = AmbaNetStack_Htons(tci);
        vlanhdr->type = AmbaNetStack_Htons(type);
        buf = &buf[vlanhdrlen];
        *pLen = *pLen + vlanhdrlen;
    } else {
        ethhdr->type = AmbaNetStack_Htons(type);
        buf = &buf[ethhdrlen];
        *pLen = *pLen + ethhdrlen;
    }
#if defined(AMBA_MISRA_FIX_H)
    AmbaMisra_TypeCast(&frame, &buf);
#else
    frame = (void *)buf;
#endif

    *Addr = frame;

    return 0;
}

/**
 * @ingroup enet
 * This function removes ethernet header
 * it removes the 802.1Q VLAN tag if the packet has the tag
 *
 * @param [in] Addr Addr points to start address of ethernet header
 * @param [in] pLen pLen value equals to size of a complete ethernet frame
 * @param [in] Addr Addr value will be increased and points to the start address of ethernet packet payload
 * @param [in] pLen pLen value will be decreased and equals to size of ethernet packet payload
 * @return enet error number
 */
UINT32 AmbaNetStack_RemoveEthHdr(void **Addr, UINT16 *pLen)
{
    const UINT16 vlanhdrlen = (UINT16)sizeof(ETH_VLAN_HDR_s);
    const ETH_HDR_s *ethhdr;
    const UINT16 ethhdrlen = (UINT16)sizeof(ETH_HDR_s);
    UINT8 *buf;
    void *frame;

    frame = *Addr;
#if defined(AMBA_MISRA_FIX_H)
    AmbaMisra_TypeCast(&buf, &frame);
    AmbaMisra_TypeCast(&ethhdr, &frame);
#else
    buf = (UINT8 *)frame;
    ethhdr = (const ETH_HDR_s *)frame;
#endif
    if (AmbaNetStack_Ntohs(ethhdr->type) == ETH_VLAN_TYPE) {
        buf = &buf[vlanhdrlen];
        *pLen = *pLen - vlanhdrlen;
    } else {
        buf = &buf[ethhdrlen];
        *pLen = *pLen - ethhdrlen;
    }
#if defined(AMBA_MISRA_FIX_H)
    AmbaMisra_TypeCast(&frame, &buf);
#else
    frame = (void *)buf;
#endif

    *Addr = frame;

    return 0;
}

/**
 * @ingroup enet
 * convert unsigned 16bits values to network order before frame transmit
 *
 * @param [in] n unsigned 16bits values to be converted
 * @return Converted value
 */
UINT16 AmbaNetStack_Htons(UINT16 n)
{
    return ((n & 0xffU) << 8) | ((n & 0xff00U) >> 8);
}

/**
 * @ingroup enet
 * convert unsigned 32bits values to network order before frame transmit
 *
 * @param [in] n unsigned 32bits values to be converted
 * @return Converted value
 */
UINT32 AmbaNetStack_Htonl(UINT32 n)
{
  return ((n & 0xffU) << 24) |
    ((n & 0xff00U) << 8) |
    ((n & 0xff0000U) >> 8) |
    ((n & 0xff000000U) >> 24);
}

/**
 * @ingroup enet
 * convert unsigned 16bits values to host order after frame receive
 *
 * @param [in] n unsigned 16bits values to be converted
 * @return Converted value
 */
UINT16 AmbaNetStack_Ntohs(UINT16 n)
{
    return ((n & 0xffU) << 8) | ((n & 0xff00U) >> 8);
}

/**
 * @ingroup enet
 * convert unsigned 32bits values to host order after frame receive
 *
 * @param [in] n unsigned 32bits values to be converted
 * @return Converted value
 */
UINT32 AmbaNetStack_Ntohl(UINT32 n)
{
  return ((n & 0xffU) << 24) |
    ((n & 0xff00U) << 8) |
    ((n & 0xff0000U) >> 8) |
    ((n & 0xff000000U) >> 24);
}
#endif //#if defined(CONFIG_THREADX)
