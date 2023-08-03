#ifndef GCOVR_USB_H
#define GCOVR_USB_H

#if defined(WIN32) || defined(__i386__)
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#endif

#include <AmbaTypes.h>

#define USB_CACHE_MEM_SIZE     (1024*1024)
#define USB_NON_CACHE_MEM_SIZE (1024*1024)
#define USB_OHCI_MEM_SIZE      (1024*1024)
#define USB_EHCI_MEM_SIZE      (1024*1024)
#define USB_RCT_MEM_SIZE       (1024*1024)

extern UINT8 usb_cache_mem[USB_CACHE_MEM_SIZE];
extern UINT8 usb_noncache_mem[USB_NON_CACHE_MEM_SIZE];
extern UINT8 usb_ehci_mem[USB_EHCI_MEM_SIZE];
extern UINT8 usb_ohci_mem[USB_OHCI_MEM_SIZE];
extern UINT8 usb_rct_mem[USB_RCT_MEM_SIZE];

extern UINT32 kal_flag_task_create;
extern UINT32 kal_flag_task_delete;
extern UINT32 kal_flag_task_suspend;
extern UINT32 kal_flag_task_resume;
extern UINT32 kal_flag_task_sleep;
extern UINT32 kal_flag_task_set_affinity;
extern UINT32 kal_flag_task_identify;
extern UINT32 kal_flag_task_query;
extern UINT32 kal_flag_task_state;
extern UINT32 kal_flag_task_terminate;

extern UINT32 kal_flag_event_flag_create;
extern UINT32 kal_flag_event_flag_delete;
extern UINT32 kal_flag_event_flag_get   ;
extern UINT32 kal_flag_event_flag_set   ;
extern UINT32 kal_flag_event_flag_clear ;

extern UINT32 kal_flag_queue_create;
extern UINT32 kal_flag_queue_delete;
extern UINT32 kal_flag_queue_flush;
extern UINT32 kal_flag_queue_recv;
extern UINT32 kal_flag_queue_send;
extern UINT32 kal_flag_queue_query;

extern UINT32 kal_flag_mutex_create;
extern UINT32 kal_flag_mutex_delete;
extern UINT32 kal_flag_mutex_take;
extern UINT32 kal_flag_mutex_give;

extern UINT32 kal_flag_semaphore_create;
extern UINT32 kal_flag_semaphore_delete;
extern UINT32 kal_flag_semaphore_take;
extern UINT32 kal_flag_semaphore_give;
extern UINT32 kal_flag_semaphore_query;

void test_value_verify(UINT32 ValueToVerify, UINT32 ValueExpected, const char *TestCase, UINT32 TestID);

void test_AmbaRTSL_RctSetUsbPortState(void);
void test_AmbaRTSL_USBReadAddress(void);
void test_AmbaRTSL_USBIsrFunctions(void);
void test_AmbaRTSL_USBMutexFunctions(void);
void test_AmbaRTSL_USBSetHwInfo(void);
void test_AmbaCSL_USBReadWrite(void);

#endif

