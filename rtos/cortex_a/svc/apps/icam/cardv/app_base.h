#ifndef __APP_BASE_H__
#define __APP_BASE_H__

#include "platform.h"
#include "cardv_modules/cardv_include.h"
#include "msg_queue/msg_queue.h"
#include "user_setting/user_setting.h"
#include "timer/timer.h"
#include "rec/rec_dvr.h"
#include "app_helper.h"
#include "AmbaDSP_Capability.h"
#include "SvcUserPref.h"
#include "SvcUserPrefCustom.h"
#include "SvcSysStat.h"
#include "SvcAppStat.h"
#include "AmbaMisraFix.h"
#include "SvcRecMain.h"
#include "pmic.h"
#include "bsp.h"
#ifdef CONFIG_ENABLE_AMBALINK
#include "AmbaLink.h"
#endif
#include "AmbaKAL.h"
#include "AmbaMisraFix.h"
#include "AmbaDSP_Event.h"
#include "AmbaDSP_EventInfo.h"
#include "AmbaSYS.h"
#include "AmbaNVM_Partition.h"
#include "SvcTask.h"
#include "AmbaDMA_Def.h"
#include "AmbaAudio_AIN.h"
#include "AmbaAudio_ADEC.h"
#include "AmbaAudio_AENC.h"
#include "AmbaAudio_AOUT.h"
#include "AmbaAudioEfx.h"
#include "AmbaAudioBuf.h"
#include "AmbaAudioBSBuf.h"
#include "AmbaAudio.h"
#include "SvcDemuxer.h"
#include "AmbaAEncFlow.h"
#include "AmbaADecFlow.h"
#include "AmbaCache.h"
#include "AmbaMMU.h"
//#include "SvcStorage.h"
#include "AmbaSD_Def.h"
#include "AmbaSD_Ctrl.h"
#include "AmbaSD_STD.h"
#include "AmbaSD.h"
//#include ".svc_autogen"
#include "AmbaRTSL_SD.h"
#include "AmbaAudio_CODEC.h"
#include "AmbaDCF.h"
#include "SvcUtil.h"
#include "AmbaKAL.h"
#include "AmbaRTSL_RTC.h"
#include "AmbaUART_Def.h"
#include "AmbaUART.h"
#include <SvcTaskList.h>
#include "AmbaDSP_Liveview.h"
#include "SvcResCfg.h"
#include "AmbaImg_AaaDef.h"
#include "AmbaWDT.h"
#include "AmbaADC.h"
#include "AmbaPWM.h"
#include "AmbaShell.h"
#include "AmbaSensor.h"
#include "SvcCmd.h"
#include "SvcRtspLink.h"
#include "AmbaSvcUsb.h"
#ifdef CONFIG_APP_FLOW_CARDV_UVC
#include "uvc/uvc.h"
#endif
#ifdef CONFIG_BUILD_FOSS_LIBJPEG_TURBO
#include "turbojpeg.h"
#endif
#include "custom.h"
#include "rec/rec_dvr.h"
#include "system/beep.h"
#include "system/wifi.h"
#include "system/metadata.h"
#include "system/async_task.h"
#include "system/app_util.h"

#include "system/key_manager.h"
#include "system/stream_share.h"
#include "system/rtos_log_share.h"
#include "system/bt_stream_share.h"
#include "system/offline_log.h"
#include "system/linux_api_service.h"
#include "system/mcu_update.h"
#include "system/scan_file_info_task.h"
#include "record/pcm_record.h"
#include "record/aac_record.h"
#include "record/gnss_record.h"
#include "record/imu_record.h"
#include "imu.h"
#include "peripheral_task/imu_task.h"
#include "thermal.h"
#include "peripheral_task/thermal_task.h"
#include "peripheral_task/gnss_task.h"
#include "peripheral_task/agnss_task.h"
#include "peripheral_task/ir_task.h"
#include "peripheral_task/button_task.h"
#include "peripheral_task/battery_task.h"
#include "peripheral_task/usb_device_task.h"
#if defined(CONFIG_GNSS_PPS_TIME_SYNC_ENABLE)
#include "peripheral_task/gnss_pps_task.h"
#endif
#include "AmbaTime.h"
#include "AmbaRTSL_NAND.h"
#include "SvcControlTask.h"
#include "SvcStgMonitor.h"
#include "AmbaDSP_ImageFilter.h"
#include "AmbaIQParamHandlerSample.h"
#include "AmbaImg_External_CtrlFunc.h"
#include "AmbaDCF_Dashcam.h"
#include "AmbaRTSL_PLL_Def.h"
#include "AmbaNAND_FTL.h"
#include "SvcNvm.h"

extern int sd_bad_or_not_exist;

#ifdef CONFIG_ENABLE_AMBALINK
extern void AmbaLink_SetBootFromA(int boot_a);
extern int AmbaLink_GetBootFromA(void);
extern void AmbaLink_SetLinuxConsoleEnable(int enable);
extern void AmbaLink_SetEraseData(int reset);
#endif
extern UINT32 AmbaDiag_GetIrqCounter(UINT32 IntID);

#if defined(CONFIG_ARM64)
    #define POINTER_MASK        0xFFFFFFFFFFFFFFFF
#else
    #define POINTER_MASK        0x00000000FFFFFFFF
#endif
#define CAST_TO_UINTPRT(ptr)         ((UINT64)((UINT32)(ptr)) & POINTER_MASK)
#define CAST_TO_PTR(uintPtr)         (void*)((UINT32)(((UINT64)(uintPtr)) & POINTER_MASK))

#endif//__APP_BASE_H__

